//
//  monero_send_routine.hpp
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
#ifndef monero_send_routine_hpp
#define monero_send_routine_hpp
//
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
//
#include "string_tools.h"
#include "crypto.h"
#include "cryptonote_basic.h"
#include "cryptonote_format_utils.h"
//
using namespace tools;
#include "tools__ret_vals.hpp"
//
#include "monero_transfer_utils.hpp"
//
namespace monero_send_routine
{
	using namespace std;
	using namespace boost;
	using namespace cryptonote;
	using namespace monero_transfer_utils;
	using namespace crypto;
	//
	// Abstracted Send routine
	// - Accessory types - Callbacks - Data fetch hooks
	typedef std::function<void(
		const property_tree::ptree & // opted not to send str but already parsed structure - this may not be optimal but makes it so that e.g. emscr_async_bridge doesn't have to send the response as an escaped JSON string nor redundantly parse/stringify
	)> api_fetch_cb_fn;
	//
	struct LightwalletAPI_Req_GetUnspentOuts
	{ // strings are NOT references, so they get copied, allowing scope of struct init not to be an issue
		const string address;
		const string view_key;
		const string amount; // "0" uint64_string
		size_t mixin;
		bool use_dust; // true; send-funds is now coded to filter unmixable and below threshold dust properly when sweeping and not sweeping
		const string dust_threshold; // uint64_string; String(MoneroConstants.dustThreshold, radix: 10)
	};
	static inline string json_string_from_req_GetUnspentOuts(const LightwalletAPI_Req_GetUnspentOuts &req_params)
	{
		boost::property_tree::ptree req_params_root;
		req_params_root.put("address", req_params.address);
		req_params_root.put("view_key", req_params.view_key);
		req_params_root.put("amount", req_params.amount);
		req_params_root.put("dust_threshold", req_params.dust_threshold);
		req_params_root.put("use_dust", req_params.use_dust);
		req_params_root.put("mixin", req_params.mixin);
		stringstream req_params_ss;
		boost::property_tree::write_json(req_params_ss, req_params_root, false/*pretty*/);
		//
		return req_params_ss.str();
	}
	LightwalletAPI_Req_GetUnspentOuts new__req_params__get_unspent_outs( // used internally and by emscr async send impl
		string from_address_string,
		string sec_viewKey_string
	);
	typedef std::function<void(
		LightwalletAPI_Req_GetUnspentOuts, // req_params - use these for making the request
		api_fetch_cb_fn // fn cb … call this after the request responds (successfully)
	)> send__get_unspent_outs_fn_type;
	//
	struct LightwalletAPI_Req_GetRandomOuts
	{
		const vector<string> amounts;
		const size_t count; // =mixin+1
	};
	static inline string json_string_from_req_GetRandomOuts(const LightwalletAPI_Req_GetRandomOuts &req_params)
	{
		boost::property_tree::ptree req_params_root;
		boost::property_tree::ptree amounts_ptree;
		BOOST_FOREACH(const string &amount_string, req_params.amounts)
		{
			property_tree::ptree amount_child;
			amount_child.put("", amount_string);
			amounts_ptree.push_back(std::make_pair("", amount_child));
		}
		req_params_root.add_child("amounts", amounts_ptree);
		req_params_root.put("count", req_params.count);
		stringstream req_params_ss;
		boost::property_tree::write_json(req_params_ss, req_params_root, false/*pretty*/);

		return req_params_ss.str();
	}
	LightwalletAPI_Req_GetRandomOuts new__req_params__get_random_outs( // used internally and by emscr async send impl
		const vector<SpendableOutput> &step1__using_outs,
		const optional<SpendableOutputToRandomAmountOutputs> &prior_attempt_unspent_outs_to_mix_outs
	);
	typedef std::function<void(
		LightwalletAPI_Req_GetRandomOuts, // req_params - use these for making the request
		api_fetch_cb_fn // fn cb … call this after the request responds (successfully)
	)> send__get_random_outs_fn_type;
	//
	struct LightwalletAPI_Req_SubmitRawTx
	{ // strings are NOT references, so they get copied, allowing scope of struct init not to be an issue
		const string address;
		const string view_key;
		const string tx; // serialized tx
	};
	static inline string json_string_from_req_SubmitRawTx(const LightwalletAPI_Req_SubmitRawTx &req_params)
	{
		boost::property_tree::ptree req_params_root;
		boost::property_tree::ptree amounts_ptree;
		req_params_root.put("address", std::move(req_params.address));
		req_params_root.put("view_key", std::move(req_params.view_key));
		req_params_root.put("tx", std::move(req_params.tx));
		stringstream req_params_ss;
		boost::property_tree::write_json(req_params_ss, req_params_root, false/*pretty*/);
		//
		return req_params_ss.str();
	}
	typedef std::function<void(
		LightwalletAPI_Req_SubmitRawTx, // req_params - use these for making the request
		api_fetch_cb_fn // fn cb … call this after the request responds (successfully)
	)> send__submit_raw_tx_fn_type;
	//
	// - Accessory types - Callbacks - Updates
	enum SendFunds_ProcessStep
	{ // These codes have values for serialization
		fetchingLatestBalance = 1,
		calculatingFee = 2,
		fetchingDecoyOutputs = 3,
		constructingTransaction = 4,
		submittingTransaction = 5
	};
	typedef std::function<void(SendFunds_ProcessStep code)> send__status_update_fn_type;
	static inline const char *err_msg_from_err_code__send_funds_step(SendFunds_ProcessStep code)
	{
		switch (code) {
			case fetchingLatestBalance:
				return "Fetching latest balance.";
			case calculatingFee:
				return "Calculating fee.";
			case fetchingDecoyOutputs:
				return "Fetching decoy outputs.";
			case constructingTransaction:
				return "Constructing transaction."; // which may go back to .calculatingFee
			case submittingTransaction:
				return "Submitted transaction.";
		}
        return "Unknown error.";
	}
	// - Accessory types - Callbacks - Routine completions
	struct SendFunds_Error_RetVals
	{
		optional<string> explicit_errMsg;
		optional<CreateTransactionErrorCode> errCode; // if != noError, abort Send process
		// for display / information purposes on errCode=needMoreMoneyThanFound during step1:
		uint64_t spendable_balance; //  (effectively but not the same as spendable_balance)
		uint64_t required_balance; // for display / information purposes on errCode=needMoreMoneyThanFound during step1
	};
	typedef std::function<void(const SendFunds_Error_RetVals &)> send__error_cb_fn_type;
	//
	struct SendFunds_Success_RetVals
	{
		uint64_t used_fee;
		uint64_t total_sent; // final_total_wo_fee + final_fee
		size_t mixin;
		optional<string> final_payment_id; // will be filled if a payment id was passed in or an integrated address was used
		string signed_serialized_tx_string;
		string tx_hash_string;
		string tx_key_string; // this includes additional_tx_keys
		string tx_pub_key_string; // from get_tx_pub_key_from_extra()
	};
	typedef std::function<void(const SendFunds_Success_RetVals &)> send__success_cb_fn_type;
	//
	// Response parsing
	struct LightwalletAPI_Res_GetUnspentOuts
	{
		optional<string> err_msg;
		// OR
		optional<uint64_t> per_byte_fee;
		optional<uint64_t> fee_mask;
		optional<vector<SpendableOutput>> unspent_outs;
		uint8_t fork_version;
	};
	struct LightwalletAPI_Res_GetRandomOuts
	{
		optional<string> err_msg;
		// OR
		optional<vector<RandomAmountOutputs>> mix_outs;
	};
	LightwalletAPI_Res_GetUnspentOuts new__parsed_res__get_unspent_outs(
		const property_tree::ptree &res,
		const secret_key &sec_viewKey,
		const secret_key &sec_spendKey,
		const public_key &pub_spendKey
	);
	LightwalletAPI_Res_GetRandomOuts new__parsed_res__get_random_outs(
		const property_tree::ptree &res
	);
	//
	// - Routine entrypoint
	struct Async_SendFunds_Args
	{ // TODO: add a way to pass native structures if available
		string from_address_string;
		string sec_viewKey_string;
		string sec_spendKey_string;
		string pub_spendKey_string;
		string to_address_string;
		optional<string> payment_id_string;
		uint64_t sending_amount;
		bool is_sweeping;
		uint32_t simple_priority;
		send__get_unspent_outs_fn_type get_unspent_outs_fn;
		send__get_random_outs_fn_type get_random_outs_fn;
		send__submit_raw_tx_fn_type submit_raw_tx_fn;
		send__status_update_fn_type status_update_fn;
		send__error_cb_fn_type error_cb_fn;
		send__success_cb_fn_type success_cb_fn;
		//
		optional<uint64_t> unlock_time; // default 0
		optional<cryptonote::network_type> nettype;
	};
	void async__send_funds(Async_SendFunds_Args args);
}

#endif /* monero_send_routine_hpp */
