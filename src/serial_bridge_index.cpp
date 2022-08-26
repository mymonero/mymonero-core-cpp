//
//  serial_bridge_index.cpp
//  Copyright (c) 2014-2019, MyMonero.com
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
#include "serial_bridge_index.hpp"
//
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
//
#include "monero_fork_rules.hpp"
#include "monero_transfer_utils.hpp"
#include "monero_address_utils.hpp" // TODO: split this/these out into a different namespaces or file so this file can scale (leave file for shared utils)
#include "monero_paymentID_utils.hpp"
#include "monero_wallet_utils.hpp"
#include "monero_key_image_utils.hpp"
#include "wallet_errors.h"
#include "string_tools.h"
#include "ringct/rctSigs.h"
//
#include "serial_bridge_utils.hpp"

using namespace std;
using namespace boost;
using namespace cryptonote;
using namespace monero_transfer_utils;
using namespace monero_fork_rules;
//
using namespace serial_bridge;
using namespace serial_bridge_utils;
//
//
// Bridge Function Implementations
//
string serial_bridge::decode_address(const string address, const string nettype)
{
	auto retVals = monero::address_utils::decodedAddress(address, nettype_from_string(nettype));
	if (retVals.did_error) {
		return error_ret_json_from_message(*(retVals.err_string));
	}
	boost::property_tree::ptree root;
	root.put("isSubaddress", retVals.isSubaddress);
	root.put("publicViewKey", *(retVals.pub_viewKey_string));
	root.put("publicSpendKey", *(retVals.pub_spendKey_string));
	if (retVals.paymentID_string != none) {
		root.put("paymentId", *(retVals.paymentID_string));
	}
	//
	return ret_json_from_root(root);
}
bool serial_bridge::is_subaddress(const string address, const string nettype)
{
	return monero::address_utils::isSubAddress(address, nettype_from_string(nettype));
}
bool serial_bridge::is_integrated_address(const string address, const string nettype)
{
	return monero::address_utils::isIntegratedAddress(address, nettype_from_string(nettype));
}
string serial_bridge::new_integrated_address(const string address, const string paymentId, const string nettype)
{
	return monero::address_utils::new_integratedAddrFromStdAddr(address, paymentId, nettype_from_string(nettype));
}
string serial_bridge::new_payment_id()
{
	return monero_paymentID_utils::new_short_plain_paymentID_string();
}

string serial_bridge::newly_created_wallet(const string localeLanguageCode, const string nettype)
{
	monero_wallet_utils::WalletDescriptionRetVals retVals;
	bool r = monero_wallet_utils::convenience__new_wallet_with_language_code(
		localeLanguageCode,
		retVals,
		nettype_from_string(nettype)
	);
	bool did_error = retVals.did_error;
	if (!r) {
		return error_ret_json_from_message(*(retVals.err_string));
	}

	boost::property_tree::ptree root;
	root.put("mnemonic", std::string((*(retVals.optl__desc)).mnemonic_string.data(), (*(retVals.optl__desc)).mnemonic_string.size()));
	root.put("mnemonicLanguage", (*(retVals.optl__desc)).mnemonic_language);
	root.put("seed", (*(retVals.optl__desc)).sec_seed_string);
	root.put("address", (*(retVals.optl__desc)).address_string);
	root.put("publicViewKey", epee::string_tools::pod_to_hex((*(retVals.optl__desc)).pub_viewKey));
	root.put("privateViewKey", epee::string_tools::pod_to_hex((*(retVals.optl__desc)).sec_viewKey));
	root.put("publicSpendKey", epee::string_tools::pod_to_hex((*(retVals.optl__desc)).pub_spendKey));
	root.put("privateSpendKey", epee::string_tools::pod_to_hex((*(retVals.optl__desc)).sec_spendKey));
	//
	return ret_json_from_root(root);
}
bool serial_bridge::are_equal_mnemonics(const string mnemonicA, const string mnemonicB)
{
	return monero_wallet_utils::are_equal_mnemonics(mnemonicA, mnemonicB);
}
string serial_bridge::address_and_keys_from_seed(const string seed, const string nettype)
{
	monero_wallet_utils::ComponentsFromSeed_RetVals retVals;
	bool r = monero_wallet_utils::address_and_keys_from_seed(
		seed,
		nettype_from_string(nettype),
		retVals
	);
	bool did_error = retVals.did_error;
	if (!r) {
		return error_ret_json_from_message(*(retVals.err_string));
	}

	boost::property_tree::ptree root;
	root.put("address", (*(retVals.optl__val)).address_string);
	root.put("publicViewKey", epee::string_tools::pod_to_hex((*(retVals.optl__val)).pub_viewKey));
	root.put("privateViewKey", epee::string_tools::pod_to_hex((*(retVals.optl__val)).sec_viewKey));
	root.put("publicSpendKey", epee::string_tools::pod_to_hex((*(retVals.optl__val)).pub_spendKey));
	root.put("privateSpendKey", epee::string_tools::pod_to_hex((*(retVals.optl__val)).sec_spendKey));
	//
	return ret_json_from_root(root);
}
string serial_bridge::mnemonic_from_seed(const string seed, const string wordsetName)
{
	monero_wallet_utils::SeedDecodedMnemonic_RetVals retVals = monero_wallet_utils::mnemonic_string_from_seed_hex_string(
		seed,
		wordsetName
	);
	boost::property_tree::ptree root;
	if (retVals.err_string != none) {
		return error_ret_json_from_message(*(retVals.err_string));
	}
	root.put("retVal", std::string((*(retVals.mnemonic_string)).data(), (*(retVals.mnemonic_string)).size()));

	return ret_json_from_root(root);
}
string serial_bridge::seed_and_keys_from_mnemonic(const string mnemonic, const string nettype)
{
	monero_wallet_utils::WalletDescriptionRetVals retVals;
	bool r = monero_wallet_utils::wallet_with(
		mnemonic,
		retVals,
		nettype_from_string(nettype)
	);
	bool did_error = retVals.did_error;
	if (!r) {
		return error_ret_json_from_message(*retVals.err_string);
	}
	monero_wallet_utils::WalletDescription walletDescription = *(retVals.optl__desc);
	boost::property_tree::ptree root;
	root.put("seed", (*(retVals.optl__desc)).sec_seed_string);
	root.put("mnemonicLanguage", (*(retVals.optl__desc)).mnemonic_language);
	root.put("address", (*(retVals.optl__desc)).address_string);
	root.put("publicViewKey", epee::string_tools::pod_to_hex((*(retVals.optl__desc)).pub_viewKey));
	root.put("privateViewKey", epee::string_tools::pod_to_hex((*(retVals.optl__desc)).sec_viewKey));
	root.put("publicSpendKey", epee::string_tools::pod_to_hex((*(retVals.optl__desc)).pub_spendKey));
	root.put("privateSpendKey", epee::string_tools::pod_to_hex((*(retVals.optl__desc)).sec_spendKey));

	return ret_json_from_root(root);
}
string serial_bridge::validate_components_for_login(const string address, const string privateViewKey, const string privateSpendKey, const string seed, const string nettype)
{
	monero_wallet_utils::WalletComponentsValidationResults retVals;
	bool r = monero_wallet_utils::validate_wallet_components_with( // returns !did_error
		address,
		privateViewKey,
		privateSpendKey,
		seed,
		nettype_from_string(nettype),
		retVals
	);
	bool did_error = retVals.did_error;
	if (!r) {
		return error_ret_json_from_message(*retVals.err_string);
	}

	boost::property_tree::ptree root;
	root.put("isValid", retVals.isValid);
	root.put("isViewOnly", retVals.isInViewOnlyMode);
	root.put("publicViewKey", retVals.pub_viewKey_string);
	root.put("publicSpendKey", retVals.pub_spendKey_string);
	//
	return ret_json_from_root(root);
}

string serial_bridge::estimated_tx_network_fee(const string priority, const string feePerb, const string forkVersion)
{
	uint64_t fee = monero_fee_utils::estimated_tx_network_fee(
		stoull(feePerb),
		stoul(priority),
		monero_fork_rules::make_use_fork_rules_fn(stoul(forkVersion))
	);

	std::ostringstream o;
	o << fee;
	//
	boost::property_tree::ptree root;
	root.put("retVal", o.str());
	//
	return ret_json_from_root(root);
}

string serial_bridge::generate_key_image(const string txPublicKey, const string privateViewKey, const string publicSpendKey, const string privateSpendKey, const string outputIndex)
{
	crypto::secret_key sec_viewKey{};
	crypto::secret_key sec_spendKey{};
	crypto::public_key pub_spendKey{};
	crypto::public_key tx_pub_key{};
	{
		bool r = false;
		r = epee::string_tools::hex_to_pod(privateViewKey, sec_viewKey);
		if (!r) {
			return error_ret_json_from_message("Invalid private view key");
		}
		r = epee::string_tools::hex_to_pod(privateSpendKey, sec_spendKey);
		if (!r) {
			return error_ret_json_from_message("Invalid private spend key");
		}
		r = epee::string_tools::hex_to_pod(publicSpendKey, pub_spendKey);
		if (!r) {
			return error_ret_json_from_message("Invalid public spend key");
		}
		r = epee::string_tools::hex_to_pod(txPublicKey, tx_pub_key);
		if (!r) {
			return error_ret_json_from_message("Invalid tx public key");
		}
	}
	monero_key_image_utils::KeyImageRetVals retVals;
	bool r = monero_key_image_utils::new__key_image(
		pub_spendKey, sec_spendKey, sec_viewKey, tx_pub_key,
		stoull(outputIndex),
		retVals
	);
	if (!r) {
		return error_ret_json_from_message("Unable to generate key image"); // TODO: return error string? (unwrap optional)
	}
	boost::property_tree::ptree root;
	root.put("retVal", epee::string_tools::pod_to_hex(retVals.calculated_key_image));
	//
	return ret_json_from_root(root);
}
//
string serial_bridge::send_step1__prepare_params_for_get_decoys(const string &args_string)
{ // TODO: possibly allow this fn to take tx sec key as an arg, although, random bit gen is now handled well by emscripten
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	//
	vector<SpendableOutput> unspent_outs;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &output_desc, json_root.get_child("unspent_outs"))
	{
		assert(output_desc.first.empty()); // array elements have no names
		SpendableOutput out{};
		out.amount = stoull(output_desc.second.get<string>("amount"));
		out.public_key = output_desc.second.get<string>("public_key");
		out.rct = output_desc.second.get_optional<string>("rct");
		if (out.rct != none && (*out.rct).empty() == true) {
			out.rct = none; // just in case it's an empty string, send to 'none' (even though receiving code now handles empty strs)
		}
		out.global_index = stoull(output_desc.second.get<string>("global_index"));
		out.index = stoull(output_desc.second.get<string>("index"));
		out.tx_pub_key = output_desc.second.get<string>("tx_pub_key");
		//
		unspent_outs.push_back(std::move(out));
	}
	optional<string> optl__prior_attempt_size_calcd_fee_string = json_root.get_optional<string>("prior_attempt_size_calcd_fee");
	optional<uint64_t> optl__prior_attempt_size_calcd_fee = none;
	if (optl__prior_attempt_size_calcd_fee_string != none) {
		optl__prior_attempt_size_calcd_fee = stoull(*optl__prior_attempt_size_calcd_fee_string);
	}
	optional<SpendableOutputToRandomAmountOutputs> optl__prior_attempt_unspent_outs_to_mix_outs;
	SpendableOutputToRandomAmountOutputs prior_attempt_unspent_outs_to_mix_outs;
	optional<boost::property_tree::ptree &> optl__prior_attempt_unspent_outs_to_mix_outs_json = json_root.get_child_optional("prior_attempt_unspent_outs_to_mix_outs");
	if (optl__prior_attempt_unspent_outs_to_mix_outs_json != none)
	{
		BOOST_FOREACH(boost::property_tree::ptree::value_type &outs_to_mix_outs_desc, *optl__prior_attempt_unspent_outs_to_mix_outs_json)
		{
			string out_pub_key = outs_to_mix_outs_desc.first;
			RandomAmountOutputs amountAndOuts{};
			BOOST_FOREACH(boost::property_tree::ptree::value_type &mix_out_output_desc, outs_to_mix_outs_desc.second)
			{
				assert(mix_out_output_desc.first.empty()); // array elements have no names
				auto amountOutput = monero_transfer_utils::RandomAmountOutput{};
				amountOutput.global_index = stoull(mix_out_output_desc.second.get<string>("global_index"));
				amountOutput.public_key = mix_out_output_desc.second.get<string>("public_key");
				amountOutput.rct = mix_out_output_desc.second.get_optional<string>("rct");
				amountAndOuts.outputs.push_back(std::move(amountOutput));
			}
			prior_attempt_unspent_outs_to_mix_outs[out_pub_key] = std::move(amountAndOuts.outputs);
		}
		optl__prior_attempt_unspent_outs_to_mix_outs = std::move(prior_attempt_unspent_outs_to_mix_outs);
	}
	uint8_t fork_version = 0; // if missing
	optional<string> optl__fork_version_string = json_root.get_optional<string>("fork_version");
	if (optl__fork_version_string != none) {
		fork_version = stoul(*optl__fork_version_string);
	}
	Send_Step1_RetVals retVals;
	monero_transfer_utils::send_step1__prepare_params_for_get_decoys(
		retVals,
		//
		json_root.get_optional<string>("payment_id_string"),
		vector<uint64_t>{stoull(json_root.get<string>("sending_amount"))},
		json_root.get<bool>("is_sweeping"),
		stoul(json_root.get<string>("priority")),
		monero_fork_rules::make_use_fork_rules_fn(fork_version),
		unspent_outs,
		stoull(json_root.get<string>("fee_per_b")), // per v8
		stoull(json_root.get<string>("fee_mask")),
		//
		optl__prior_attempt_size_calcd_fee, // use this for passing step2 "must-reconstruct" return values back in, i.e. re-entry; when nil, defaults to attempt at network min
		optl__prior_attempt_unspent_outs_to_mix_outs // on re-entry, re-use the same outs and requested decoys, in order to land on the correct calculated fee
	);
	boost::property_tree::ptree root;
	if (retVals.errCode != noError) {
		root.put("err_code", retVals.errCode);
		root.put("err_msg", err_msg_from_err_code__create_transaction(retVals.errCode));
		//
		// The following will be set if errCode==needMoreMoneyThanFound - and i'm depending on them being 0 otherwise
		root.put("spendable_balance", RetVals_Transforms::str_from(retVals.spendable_balance));
		root.put("required_balance", RetVals_Transforms::str_from(retVals.required_balance));
	} else {
		root.put("mixin", RetVals_Transforms::str_from(retVals.mixin));
		root.put("using_fee", RetVals_Transforms::str_from(retVals.using_fee));
		root.put("final_total_wo_fee", RetVals_Transforms::str_from(retVals.final_total_wo_fee));
		root.put("change_amount", RetVals_Transforms::str_from(retVals.change_amount));
		{
			boost::property_tree::ptree using_outs_ptree;
			BOOST_FOREACH(SpendableOutput &out, retVals.using_outs)
			{ // PROBABLY don't need to shuttle these back (could send only public_key) but consumers might like the feature of being able to send this JSON structure directly back to step2 without reconstructing it for themselves
				auto out_ptree_pair = std::make_pair("", boost::property_tree::ptree{});
 				auto& out_ptree = out_ptree_pair.second;
				out_ptree.put("amount", RetVals_Transforms::str_from(out.amount));
				out_ptree.put("public_key", out.public_key);
				if (out.rct != none && (*out.rct).empty() == false) {
					out_ptree.put("rct", *out.rct); 
				}
				out_ptree.put("global_index", RetVals_Transforms::str_from(out.global_index));
				out_ptree.put("index", RetVals_Transforms::str_from(out.index));
				out_ptree.put("tx_pub_key", out.tx_pub_key);
				using_outs_ptree.push_back(out_ptree_pair);
			}
			root.add_child("using_outs", using_outs_ptree);
		}
	}
	return ret_json_from_root(root);
}
//
string serial_bridge::pre_step2_tie_unspent_outs_to_mix_outs_for_all_future_tx_attempts(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	//
	vector<SpendableOutput> using_outs;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &output_desc, json_root.get_child("using_outs"))
	{
		assert(output_desc.first.empty()); // array elements have no names
		SpendableOutput out{};
		out.amount = stoull(output_desc.second.get<string>("amount"));
		out.public_key = output_desc.second.get<string>("public_key");
		out.rct = output_desc.second.get_optional<string>("rct");
		if (out.rct != none && (*out.rct).empty() == true) {
			out.rct = none; // just in case it's an empty string, send to 'none' (even though receiving code now handles empty strs)
		}
		out.global_index = stoull(output_desc.second.get<string>("global_index"));
		out.index = stoull(output_desc.second.get<string>("index"));
		out.tx_pub_key = output_desc.second.get<string>("tx_pub_key");
		//
		using_outs.push_back(std::move(out));
	}
	//
	vector<RandomAmountOutputs> mix_outs_from_server;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &mix_out_desc, json_root.get_child("mix_outs"))
	{
		assert(mix_out_desc.first.empty()); // array elements have no names
		auto amountAndOuts = RandomAmountOutputs{};
		amountAndOuts.amount = stoull(mix_out_desc.second.get<string>("amount"));
		BOOST_FOREACH(boost::property_tree::ptree::value_type &mix_out_output_desc, mix_out_desc.second.get_child("outputs"))
		{
			assert(mix_out_output_desc.first.empty()); // array elements have no names
			auto amountOutput = RandomAmountOutput{};
			amountOutput.global_index = stoull(mix_out_output_desc.second.get<string>("global_index"));
			amountOutput.public_key = mix_out_output_desc.second.get<string>("public_key");
			amountOutput.rct = mix_out_output_desc.second.get_optional<string>("rct");
			amountAndOuts.outputs.push_back(std::move(amountOutput));
		}
		mix_outs_from_server.push_back(std::move(amountAndOuts));
	}
	//
	optional<SpendableOutputToRandomAmountOutputs> optl__prior_attempt_unspent_outs_to_mix_outs;
	SpendableOutputToRandomAmountOutputs prior_attempt_unspent_outs_to_mix_outs;
	optional<boost::property_tree::ptree &> optl__prior_attempt_unspent_outs_to_mix_outs_json = json_root.get_child_optional("prior_attempt_unspent_outs_to_mix_outs");
	if (optl__prior_attempt_unspent_outs_to_mix_outs_json != none)
	{
		BOOST_FOREACH(boost::property_tree::ptree::value_type &outs_to_mix_outs_desc, *optl__prior_attempt_unspent_outs_to_mix_outs_json)
		{
			string out_pub_key = outs_to_mix_outs_desc.first;
			RandomAmountOutputs amountAndOuts{};
			BOOST_FOREACH(boost::property_tree::ptree::value_type &mix_out_output_desc, outs_to_mix_outs_desc.second)
			{
				assert(mix_out_output_desc.first.empty()); // array elements have no names
				auto amountOutput = monero_transfer_utils::RandomAmountOutput{};
				amountOutput.global_index = stoull(mix_out_output_desc.second.get<string>("global_index"));
				amountOutput.public_key = mix_out_output_desc.second.get<string>("public_key");
				amountOutput.rct = mix_out_output_desc.second.get_optional<string>("rct");
				amountAndOuts.outputs.push_back(std::move(amountOutput));
			}
			prior_attempt_unspent_outs_to_mix_outs[out_pub_key] = std::move(amountAndOuts.outputs);
		}
		optl__prior_attempt_unspent_outs_to_mix_outs = std::move(prior_attempt_unspent_outs_to_mix_outs);
	}
	//
	Tie_Outs_to_Mix_Outs_RetVals retVals;
	monero_transfer_utils::pre_step2_tie_unspent_outs_to_mix_outs_for_all_future_tx_attempts(
		retVals,
		//
		using_outs,
		mix_outs_from_server,
		//
		optl__prior_attempt_unspent_outs_to_mix_outs
	);
	boost::property_tree::ptree root;
	if (retVals.errCode != noError) {
		root.put(ret_json_key__any__err_code(), retVals.errCode);
		root.put(ret_json_key__any__err_msg(), err_msg_from_err_code__create_transaction(retVals.errCode));
	} else {
		{
			boost::property_tree::ptree mix_outs_ptree;
			BOOST_FOREACH(RandomAmountOutputs &mix_outs, retVals.mix_outs)
			{
				auto mix_outs_amount_ptree_pair = std::make_pair("", boost::property_tree::ptree{});
				auto& mix_outs_amount_ptree = mix_outs_amount_ptree_pair.second;
				mix_outs_amount_ptree.put("amount", RetVals_Transforms::str_from(mix_outs.amount));
				auto outputs_ptree_pair = std::make_pair("", boost::property_tree::ptree{});
				auto& outputs_ptree = outputs_ptree_pair.second;
				BOOST_FOREACH(RandomAmountOutput &out, mix_outs.outputs)
				{
					auto mix_out_ptree_pair = std::make_pair("", boost::property_tree::ptree{});
					auto& mix_out_ptree = mix_out_ptree_pair.second;
					mix_out_ptree.put("global_index", RetVals_Transforms::str_from(out.global_index));
					mix_out_ptree.put("public_key", out.public_key);
					if (out.rct != none && (*out.rct).empty() == false) {
						mix_out_ptree.put("rct", *out.rct);
					}
					outputs_ptree.push_back(mix_out_ptree_pair);
				}
				mix_outs_amount_ptree.add_child("outputs", outputs_ptree);
				mix_outs_ptree.push_back(mix_outs_amount_ptree_pair);
			}
			root.add_child(ret_json_key__send__mix_outs(), mix_outs_ptree);
		}
		//
		{
			boost::property_tree::ptree prior_attempt_unspent_outs_to_mix_outs_new_ptree;
			for (const auto &out_pub_key_to_mix_outs : retVals.prior_attempt_unspent_outs_to_mix_outs_new)
			{
				auto outs_ptree_pair = std::make_pair(out_pub_key_to_mix_outs.first, boost::property_tree::ptree{});
				auto& outs_ptree = outs_ptree_pair.second;
				for (const auto &mix_out : out_pub_key_to_mix_outs.second)
				{
					auto mix_out_ptree_pair = std::make_pair("", boost::property_tree::ptree{});
					auto& mix_out_ptree = mix_out_ptree_pair.second;
					mix_out_ptree.put("global_index", RetVals_Transforms::str_from(mix_out.global_index));
					mix_out_ptree.put("public_key", mix_out.public_key);
					if (mix_out.rct != none && (*mix_out.rct).empty() == false) {
						mix_out_ptree.put("rct", *mix_out.rct);
					}
					outs_ptree.push_back(mix_out_ptree_pair);
				}
				prior_attempt_unspent_outs_to_mix_outs_new_ptree.push_back(outs_ptree_pair);
			}
			root.add_child(ret_json_key__send__prior_attempt_unspent_outs_to_mix_outs_new(), prior_attempt_unspent_outs_to_mix_outs_new_ptree);
		}
	}
	return ret_json_from_root(root);
}
//
string serial_bridge::send_step2__try_create_transaction(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	//
	vector<SpendableOutput> using_outs;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &output_desc, json_root.get_child("using_outs"))
	{
		assert(output_desc.first.empty()); // array elements have no names
		SpendableOutput out{};
		out.amount = stoull(output_desc.second.get<string>("amount"));
		out.public_key = output_desc.second.get<string>("public_key");
		out.rct = output_desc.second.get_optional<string>("rct");
		if (out.rct != none && (*out.rct).empty() == true) {
			out.rct = none; // send to 'none' if empty str for safety
		}
		out.global_index = stoull(output_desc.second.get<string>("global_index"));
		out.index = stoull(output_desc.second.get<string>("index"));
		out.tx_pub_key = output_desc.second.get<string>("tx_pub_key");
		//
		using_outs.push_back(std::move(out));
	}
	vector<RandomAmountOutputs> mix_outs;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &mix_out_desc, json_root.get_child("mix_outs"))
	{
		assert(mix_out_desc.first.empty()); // array elements have no names
		auto amountAndOuts = RandomAmountOutputs{};
		amountAndOuts.amount = stoull(mix_out_desc.second.get<string>("amount"));
		BOOST_FOREACH(boost::property_tree::ptree::value_type &mix_out_output_desc, mix_out_desc.second.get_child("outputs"))
		{
			assert(mix_out_output_desc.first.empty()); // array elements have no names
			auto amountOutput = RandomAmountOutput{};
			amountOutput.global_index = stoull(mix_out_output_desc.second.get<string>("global_index")); // this is, I believe, presently supplied as a string by the API, probably to avoid overflow
			amountOutput.public_key = mix_out_output_desc.second.get<string>("public_key");
			amountOutput.rct = mix_out_output_desc.second.get_optional<string>("rct");
			amountAndOuts.outputs.push_back(std::move(amountOutput));
		}
		mix_outs.push_back(std::move(amountAndOuts));
	}
	uint8_t fork_version = 0; // if missing
	optional<string> optl__fork_version_string = json_root.get_optional<string>("fork_version");
	if (optl__fork_version_string != none) {
		fork_version = stoul(*optl__fork_version_string);
	}
	Send_Step2_RetVals retVals;
	monero_transfer_utils::send_step2__try_create_transaction(
		retVals,
		//
		json_root.get<string>("from_address_string"),
		json_root.get<string>("sec_viewKey_string"),
		json_root.get<string>("sec_spendKey_string"),
		vector<string>{json_root.get<string>("to_address_string")},
		json_root.get_optional<string>("payment_id_string"),
		vector<uint64_t>{stoull(json_root.get<string>("final_total_wo_fee"))},
		stoull(json_root.get<string>("change_amount")),
		stoull(json_root.get<string>("fee_amount")),
		stoul(json_root.get<string>("priority")),
		using_outs,
		stoull(json_root.get<string>("fee_per_b")),
		stoull(json_root.get<string>("fee_mask")),
		mix_outs,
		monero_fork_rules::make_use_fork_rules_fn(fork_version),
		stoull(json_root.get<string>("unlock_time")),
		nettype_from_string(json_root.get<string>("nettype_string"))
	);
	boost::property_tree::ptree root;
	if (retVals.errCode != noError) {
		root.put("err_code", retVals.errCode);
		root.put("err_msg", err_msg_from_err_code__create_transaction(retVals.errCode));
	} else {
		if (retVals.tx_must_be_reconstructed) {
			root.put("tx_must_be_reconstructed", true);
			root.put("fee_actually_needed", RetVals_Transforms::str_from(retVals.fee_actually_needed)); // must be passed back
		} else {
			root.put("tx_must_be_reconstructed", false); // so consumers have it available
			root.put("serialized_signed_tx", *(retVals.signed_serialized_tx_string));
			root.put("tx_hash", *(retVals.tx_hash_string));
			root.put("tx_key", *(retVals.tx_key_string));
			root.put("tx_pub_key", *(retVals.tx_pub_key_string));
		}
	}
	return ret_json_from_root(root);
}
	} else if (rv_type_int == rct::RCTTypeBulletproofPlus) {
		rv.type = rct::RCTTypeBulletproofPlus;
	} else if (rv_type_int == rct::RCTTypeBulletproofPlus) {
		rv.type = rct::RCTTypeBulletproofPlus;
