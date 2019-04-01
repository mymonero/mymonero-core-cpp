//
//  monero_send_routine.cpp
//  Copyright © 2018 MyMonero. All rights reserved.
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are
//  permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//	conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//	of conditions and the following disclaimer in the documentation and/or other
//	materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be
//	used to endorse or promote products derived from this software without specific
//	prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
//  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
//  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//
//
//
#include <boost/property_tree/json_parser.hpp>
#include "wallet_errors.h"
#include "string_tools.h"
//
#include "monero_send_routine.hpp"
//
#include "monero_transfer_utils.hpp"
#include "monero_fork_rules.hpp"
#include "monero_key_image_utils.hpp"
#include "monero_address_utils.hpp"
//
using namespace crypto;
using namespace std;
using namespace boost;
using namespace epee;
using namespace cryptonote;
using namespace tools; // for error::
using namespace monero_transfer_utils;
using namespace monero_fork_rules;
using namespace monero_key_image_utils; // for API response parsing
using namespace monero_send_routine;
//
//
optional<uint64_t> _possible_uint64_from_json(
	const boost::property_tree::ptree &res,
	const string &fieldname
) { // throws
	auto optl_str = res.get_optional<string>(fieldname);
//	cout << fieldname << ": " << optl_str << endl;
	if (optl_str != none) {
		return stoull(*optl_str);
	}
	auto optl_uint32 = res.get_optional<uint32_t>(fieldname); // not uint64 b/c JSON can't store such values
	if (optl_uint32 != none) {
		return (uint64_t)*optl_uint32; // cast
	}
	return none;
}
//
LightwalletAPI_Req_GetUnspentOuts monero_send_routine::new__req_params__get_unspent_outs(
	string from_address_string,
	string sec_viewKey_string
) {
	ostringstream dustT_ss;
	dustT_ss << dust_threshold();
	return {
		std::move(from_address_string),
		std::move(sec_viewKey_string),
		"0", // amount - always sent as "0"
		fixed_mixinsize(),
		true, // use dust
		dustT_ss.str()
	};
}
LightwalletAPI_Req_GetRandomOuts monero_send_routine::new__req_params__get_random_outs(
	vector<SpendableOutput> &step1__using_outs
) {
	vector<string> decoy_req__amounts;
	BOOST_FOREACH(SpendableOutput &using_out, step1__using_outs)
	{
		if (using_out.rct != none && (*(using_out.rct)).size() > 0) {
			decoy_req__amounts.push_back("0");
		} else {
			ostringstream amount_ss;
			amount_ss << using_out.amount;
			decoy_req__amounts.push_back(amount_ss.str());
		}
	}
	return LightwalletAPI_Req_GetRandomOuts{
		decoy_req__amounts,
		fixed_mixinsize() + 1 // count; Add one to mixin so we can skip real output key if necessary
	};
}
//
LightwalletAPI_Res_GetUnspentOuts monero_send_routine::new__parsed_res__get_unspent_outs(
	const property_tree::ptree &res,
	const secret_key &sec_viewKey,
	const secret_key &sec_spendKey,
	const public_key &pub_spendKey
) {
	uint64_t final__per_byte_fee = 0;
	uint64_t fee_mask = 10000; // just a fallback value - no real reason to set this here normally
	try {
		optional<uint64_t> possible__uint64 = _possible_uint64_from_json(res, "per_byte_fee");
		if (possible__uint64 != none) {
			final__per_byte_fee = *possible__uint64;
		}
	} catch (const std::exception &e) {
		cout << "Unspent outs per-byte-fee parse error: " << e.what() << endl;
		string err_msg = "Unspent outs: Unrecognized per-byte fee format";
		return {
			err_msg,
			none, none, none
		};
	}
	try {
		optional<uint64_t> possible__uint64 = _possible_uint64_from_json(res, "fee_mask");
		if (possible__uint64 != none) {
			fee_mask = *possible__uint64;
		}
	} catch (const std::exception &e) {
		cout << "Unspent outs fee_mask parse error: " << e.what() << endl;
		string err_msg = "Unspent outs: Unrecognized fee_mask format";
		return {
			err_msg,
			none, none, none
		};
	}
	if (final__per_byte_fee == 0) {
		try {
			optional<uint64_t> possible__uint64 = _possible_uint64_from_json(res, "per_kb_fee");
			if (possible__uint64 != none) {
				final__per_byte_fee = (*possible__uint64) / 1024; // scale from kib to b
				fee_mask = 10000; // just to be explicit
			}
		} catch (const std::exception &e) {
			cout << "Unspent outs per-kb-fee parse error: " << e.what() << endl;
			string err_msg = "Unspent outs: Unrecognized per-kb fee format";
			return {
				err_msg,
				none, none, none
			};
		}
	}
	if (final__per_byte_fee == 0) {
		string err_msg = "Unable to get a per-byte fee from server response.";
		return {
			err_msg,
			none, none, none
		};
	}
	vector<SpendableOutput> unspent_outs;
	BOOST_FOREACH(const boost::property_tree::ptree::value_type &output_desc, res.get_child("outputs"))
	{
		assert(output_desc.first.empty()); // array elements have no names
		//
		auto optl__tx_pub_key = output_desc.second.get_optional<string>("tx_pub_key");
		if (optl__tx_pub_key == none) { // TODO: do we ever actually expect these not to exist?
			cout << "Warn: This unspent out was missing a tx_pub_key. Skipping." << endl;
			continue; // skip
		}
		crypto::public_key tx_pub_key{};
		{
			bool r = epee::string_tools::hex_to_pod(*optl__tx_pub_key, tx_pub_key);
			if (!r) {
				string err_msg = "Invalid tx pub key";
				return {
					err_msg,
					none, none, none
				};
			}
		}
		uint64_t output__index;
		try {
			optional<uint64_t> possible__uint64 = _possible_uint64_from_json(output_desc.second, "index");
			if (possible__uint64 != none) {
				output__index = *possible__uint64; // expecting this to exist
			} else { // bail
				string err_msg = "Expected unspent output to have an \"index\"";
				return {
					err_msg,
					none, none, none
				};
			}
		} catch (const std::exception &e) {
			cout << "Unspent outs output index parse error: " << e.what() << endl;
			string err_msg = "Unspent outs: Unrecognized output index format";
			return {
				err_msg,
				none, none, none
			};
		}
		bool isOutputSpent = false; // let's see…
		{
			BOOST_FOREACH(const boost::property_tree::ptree::value_type &spend_key_image_string, output_desc.second.get_child("spend_key_images"))
			{
//				cout << "spend_key_image_string: " << spend_key_image_string.second.data() << endl;
				KeyImageRetVals retVals;
				bool r = new__key_image(
					pub_spendKey, sec_spendKey, sec_viewKey, tx_pub_key,
					output__index,
					retVals
				);
				if (!r) {
					string err_msg = "Unable to generate key image";
					return {
						err_msg,
						none, none, none
					};
				}
				auto calculated_key_image_string = epee::string_tools::pod_to_hex(retVals.calculated_key_image);
//				cout << "calculated_key_image_string: " << calculated_key_image_string << endl;
				auto areEqual = calculated_key_image_string == spend_key_image_string.second.data();
				if (areEqual) {
					isOutputSpent = true; // output was spent… exclude
					break; // exit spend key img loop to evaluate isOutputSpent
				}
			}
		}
		if (isOutputSpent == false) {
			SpendableOutput out{};
			out.amount = stoull(output_desc.second.get<string>("amount"));
			out.public_key = output_desc.second.get<string>("public_key");
			out.rct = output_desc.second.get_optional<string>("rct");
			out.global_index = stoull(output_desc.second.get<string>("global_index"));
			out.index = output__index;
			out.tx_pub_key = *optl__tx_pub_key; // just b/c we've already accessed it above
			//
			unspent_outs.push_back(std::move(out));
		}
	}
	auto fork_version = res.get_optional<uint8_t>("fork_version");
	return LightwalletAPI_Res_GetUnspentOuts{
		none,
		final__per_byte_fee, fee_mask, unspent_outs,
		fork_version ? *fork_version : static_cast<uint8_t>(0)
	};
}
LightwalletAPI_Res_GetRandomOuts monero_send_routine::new__parsed_res__get_random_outs(
	const property_tree::ptree &res
) {
	vector<RandomAmountOutputs> mix_outs;
	BOOST_FOREACH(const boost::property_tree::ptree::value_type &mix_out_desc, res.get_child("amount_outs"))
	{
		assert(mix_out_desc.first.empty()); // array elements have no names
		auto amountAndOuts = RandomAmountOutputs{};
		try {
			optional<uint64_t> possible__uint64 = _possible_uint64_from_json(mix_out_desc.second, "amount");
			if (possible__uint64 != none) {
				amountAndOuts.amount = *possible__uint64;
			}
		} catch (const std::exception &e) {
			cout << "Random outs response 'amount' parse error: " << e.what() << endl;
			string err_msg = "Random outs: Unrecognized 'amount' format";
			return {err_msg, none};
		}
		BOOST_FOREACH(const boost::property_tree::ptree::value_type &mix_out_output_desc, mix_out_desc.second.get_child("outputs"))
		{
			assert(mix_out_output_desc.first.empty()); // array elements have no names
			auto amountOutput = RandomAmountOutput{};
			try {
				optional<uint64_t> possible__uint64 = _possible_uint64_from_json(mix_out_output_desc.second, "global_index");
				if (possible__uint64 != none) {
					amountOutput.global_index = *possible__uint64;
				}
			} catch (const std::exception &e) {
				cout << "Random outs response 'global_index' parse error: " << e.what() << endl;
				string err_msg = "Random outs: Unrecognized 'global_index' format";
				return {err_msg, none};
			}
			amountOutput.public_key = mix_out_output_desc.second.get<string>("public_key");
			amountOutput.rct = mix_out_output_desc.second.get_optional<string>("rct");
			//
			amountAndOuts.outputs.push_back(std::move(amountOutput));
		}
		mix_outs.push_back(std::move(amountAndOuts));
	}
	return {
		none, mix_outs
	};
}
//
struct _SendFunds_ConstructAndSendTx_Args
{
	const string &from_address_string;
	const string &sec_viewKey_string;
	const string &sec_spendKey_string;
	const string &to_address_string;
	optional<string> payment_id_string;
	uint64_t sending_amount;
	bool is_sweeping;
	uint32_t simple_priority;
	const send__get_random_outs_fn_type &get_random_outs_fn;
	const send__submit_raw_tx_fn_type &submit_raw_tx_fn;
	const send__status_update_fn_type &status_update_fn;
	const send__error_cb_fn_type &error_cb_fn;
	const send__success_cb_fn_type &success_cb_fn;
	uint64_t unlock_time;
	cryptonote::network_type nettype;
	//
	const vector<SpendableOutput> &unspent_outs;
	uint64_t fee_per_b;
	uint64_t fee_quantization_mask;
	uint8_t fork_version;
	//
	// cached
	const secret_key &sec_viewKey;
	const secret_key &sec_spendKey;
	//
	optional<uint64_t> passedIn_attemptAt_fee;
	size_t constructionAttempt;
};
void _reenterable_construct_and_send_tx(
	const _SendFunds_ConstructAndSendTx_Args &args,
	//
	// re-entry params
	optional<uint64_t> passedIn_attemptAt_fee						= none,
	size_t constructionAttempt 										= 0
) {
	args.status_update_fn(calculatingFee);
	//
	auto use_fork_rules = monero_fork_rules::make_use_fork_rules_fn(args.fork_version);
	//
	Send_Step1_RetVals step1_retVals;
	monero_transfer_utils::send_step1__prepare_params_for_get_decoys(
		step1_retVals,
		//
		args.payment_id_string,
		args.sending_amount,
		args.is_sweeping,
		args.simple_priority,
		use_fork_rules,
		args.unspent_outs,
		args.fee_per_b,
		args.fee_quantization_mask,
		//
		passedIn_attemptAt_fee // use this for passing step2 "must-reconstruct" return values back in, i.e. re-entry; when nil, defaults to attempt at network min
	);
	if (step1_retVals.errCode != noError) {
		SendFunds_Error_RetVals error_retVals;
		error_retVals.errCode = step1_retVals.errCode;
		error_retVals.spendable_balance = step1_retVals.spendable_balance;
		error_retVals.required_balance = step1_retVals.required_balance;
		args.error_cb_fn(error_retVals);
		return;
	}
	api_fetch_cb_fn get_random_outs_fn__cb_fn = [
		args,
		step1_retVals,
		constructionAttempt,
		use_fork_rules
	] (
		const property_tree::ptree &res
	) -> void {
		auto parsed_res = new__parsed_res__get_random_outs(res);
		if (parsed_res.err_msg != none) {
			SendFunds_Error_RetVals error_retVals;
			error_retVals.explicit_errMsg = std::move(*(parsed_res.err_msg));
			args.error_cb_fn(error_retVals);
			return;
		}
		Send_Step2_RetVals step2_retVals;
		monero_transfer_utils::send_step2__try_create_transaction(
			step2_retVals,
			//
			args.from_address_string,
			args.sec_viewKey_string,
			args.sec_spendKey_string,
			args.to_address_string,
			args.payment_id_string,
			step1_retVals.final_total_wo_fee,
			step1_retVals.change_amount,
			step1_retVals.using_fee,
			args.simple_priority,
			step1_retVals.using_outs,
			args.fee_per_b,
			args.fee_quantization_mask,
			*(parsed_res.mix_outs),
			std::move(use_fork_rules),
			args.unlock_time,
			args.nettype
		);
		if (step2_retVals.errCode != noError) {
			SendFunds_Error_RetVals error_retVals;
			error_retVals.errCode = step2_retVals.errCode;
			args.error_cb_fn(error_retVals);
			return;
		}
		if (step2_retVals.tx_must_be_reconstructed) {
			// this will update status back to .calculatingFee
			if (constructionAttempt > 15) { // just going to avoid an infinite loop here or particularly long stack
				SendFunds_Error_RetVals error_retVals;
				error_retVals.explicit_errMsg = "Unable to construct a transaction with sufficient fee for unknown reason.";
				args.error_cb_fn(error_retVals);
				return;
			}
//			cout << "step2_retVals.fee_actually_needed: " << step2_retVals.fee_actually_needed << endl;
			_reenterable_construct_and_send_tx(
				args,
				//
				step2_retVals.fee_actually_needed, // -> reconstruction attempt's step1's passedIn_attemptAt_fee
				constructionAttempt+1
			);
			return;
		}
		args.status_update_fn(submittingTransaction);
		//
		api_fetch_cb_fn submit_raw_tx_fn__cb_fn = [
			args,
			step1_retVals,
			step2_retVals
		] (
			const property_tree::ptree &res
		) -> void {
			// not actually expecting anything in a success response, so no need to parse
			SendFunds_Success_RetVals success_retVals;
			success_retVals.used_fee = step1_retVals.using_fee; // NOTE: not the same thing as step2_retVals.fee_actually_needed
			success_retVals.total_sent = step1_retVals.final_total_wo_fee + step1_retVals.using_fee;
			success_retVals.mixin = step1_retVals.mixin;
			{
				optional<string> returning__payment_id = args.payment_id_string; // separated from submit_raw_tx_fn so that it can be captured w/o capturing all of args (FIXME: does this matter?)
				if (returning__payment_id == none) {
					auto decoded = monero::address_utils::decodedAddress(args.to_address_string, args.nettype);
					if (decoded.did_error) { // would be very strange...
						SendFunds_Error_RetVals error_retVals;
						error_retVals.explicit_errMsg = *(decoded.err_string);
						args.error_cb_fn(error_retVals);
						return;
					}
					if (decoded.paymentID_string != none) {
						returning__payment_id = std::move(*(decoded.paymentID_string)); // just preserving this as an original return value - this can probably eventually be removed
					}
				}
				success_retVals.final_payment_id = returning__payment_id;
			}
			success_retVals.signed_serialized_tx_string = std::move(*(step2_retVals.signed_serialized_tx_string));
			success_retVals.tx_hash_string = std::move(*(step2_retVals.tx_hash_string));
			success_retVals.tx_key_string = std::move(*(step2_retVals.tx_key_string));
			success_retVals.tx_pub_key_string = std::move(*(step2_retVals.tx_pub_key_string));
			//
			args.success_cb_fn(success_retVals);
		};
		args.submit_raw_tx_fn(LightwalletAPI_Req_SubmitRawTx{
			args.from_address_string,
			args.sec_viewKey_string,
			*(step2_retVals.signed_serialized_tx_string)
		}, submit_raw_tx_fn__cb_fn);
	};
	//
	args.status_update_fn(fetchingDecoyOutputs);
	//
	args.get_random_outs_fn(
		new__req_params__get_random_outs(step1_retVals.using_outs),
		get_random_outs_fn__cb_fn
	);
}
//
//
// Entrypoint
void monero_send_routine::async__send_funds(Async_SendFunds_Args args)
{
	uint64_t usable__sending_amount = args.is_sweeping ? 0 : args.sending_amount;
	crypto::secret_key sec_viewKey{};
	crypto::secret_key sec_spendKey{};
	crypto::public_key pub_spendKey{};
	{
		bool r = false;
		r = epee::string_tools::hex_to_pod(args.sec_viewKey_string, sec_viewKey);
		if (!r) {
			SendFunds_Error_RetVals error_retVals;
			error_retVals.explicit_errMsg = "Invalid secret view key";
			args.error_cb_fn(error_retVals);
			return;
		}
		r = epee::string_tools::hex_to_pod(args.sec_spendKey_string, sec_spendKey);
		if (!r) {
			SendFunds_Error_RetVals error_retVals;
			error_retVals.explicit_errMsg = "Invalid sec spend key";
			args.error_cb_fn(error_retVals);
			return;
		}
		r = epee::string_tools::hex_to_pod(args.pub_spendKey_string, pub_spendKey);
		if (!r) {
			SendFunds_Error_RetVals error_retVals;
			error_retVals.explicit_errMsg = "Invalid public spend key";
			args.error_cb_fn(error_retVals);
			return;
		}
	}
	api_fetch_cb_fn get_unspent_outs_fn__cb_fn = [
		args,
		usable__sending_amount,
		sec_viewKey, sec_spendKey, pub_spendKey
	] (
		const property_tree::ptree &res
	) -> void {
		auto parsed_res = new__parsed_res__get_unspent_outs(
			res,
			sec_viewKey, sec_spendKey, pub_spendKey
		);
		if (parsed_res.err_msg != none) {
			SendFunds_Error_RetVals error_retVals;
			error_retVals.explicit_errMsg = std::move(*(parsed_res.err_msg));
			args.error_cb_fn(error_retVals);
			return;
		}
		_reenterable_construct_and_send_tx(_SendFunds_ConstructAndSendTx_Args{
			args.from_address_string, args.sec_viewKey_string, args.sec_spendKey_string,
			args.to_address_string, args.payment_id_string, usable__sending_amount, args.is_sweeping, args.simple_priority,
			args.get_random_outs_fn, args.submit_raw_tx_fn, args.status_update_fn, args.error_cb_fn, args.success_cb_fn,
			args.unlock_time == none ? 0 : *(args.unlock_time),
			args.nettype == none ? MAINNET : *(args.nettype),
			//
			*(parsed_res.unspent_outs),
			*(parsed_res.per_byte_fee),
			*(parsed_res.fee_mask),
			parsed_res.fork_version,
			//
			sec_viewKey, sec_spendKey
		});
	};
	args.status_update_fn(fetchingLatestBalance);
	//
	args.get_unspent_outs_fn(
		new__req_params__get_unspent_outs(
			args.from_address_string,
			args.sec_viewKey_string
		),
		get_unspent_outs_fn__cb_fn
	);
}
