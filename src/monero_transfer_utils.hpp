//
//  monero_transfer_utils.hpp
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
#ifndef monero_transfer_utils_hpp
#define monero_transfer_utils_hpp
//
#include <boost/optional.hpp>
//
#include "string_tools.h"
//
#include "crypto.h"
#include "cryptonote_basic.h"
#include "cryptonote_format_utils.h"
#include "cryptonote_tx_utils.h"
#include "ringct/rctSigs.h"
//
#include "monero_fork_rules.hpp"
#include "monero_fee_utils.hpp"
//
using namespace tools;
#include "tools__ret_vals.hpp"
//
namespace monero_transfer_utils
{
	using namespace std;
	using namespace boost;
	using namespace cryptonote;
	using namespace monero_fork_rules;
	using namespace crypto;
	//
	// Types - Arguments
	struct SpendableOutput
	{
		uint64_t amount;
		string public_key;
		optional<string> rct;
		uint64_t global_index;
		uint64_t index;
		string tx_pub_key;
	};
	struct RandomAmountOutput
	{
		uint64_t global_index; // this is, I believe, presently supplied as a string by the API, probably to avoid overflow
		string public_key;
		optional<string> rct;
	};
	struct RandomAmountOutputs
	{
		uint64_t amount;
		vector<RandomAmountOutput> outputs;
	};
	//
	// Types - Return value
	enum CreateTransactionErrorCode // TODO: switch to enum class to fix namespacing
	{ // These codes have values for serialization
		noError 					 	= 0,
		//
		noDestinations				 	= 1,
		wrongNumberOfMixOutsProvided 	= 2,
		notEnoughOutputsForMixing	 	= 3,
		invalidSecretKeys			 	= 4,
		outputAmountOverflow		 	= 5,
		inputAmountOverflow			 	= 6,
		mixRCTOutsMissingCommit		 	= 7,
		resultFeeNotEqualToGiven 		= 8,
		invalidDestinationAddress		= 9,
		nonZeroPIDWithIntAddress		= 10,
		cantUsePIDWithSubAddress		= 11,
		couldntAddPIDNonceToTXExtra		= 12,
		givenAnInvalidPubKey			= 13,
		invalidCommitOrMaskOnOutputRCT	= 14,
		transactionNotConstructed		= 15,
		transactionTooBig				= 16,
		notYetImplemented				= 17,
		couldntDecodeToAddress			= 18,
		invalidPID						= 19,
		enteredAmountTooLow				= 20,
		cantGetDecryptedMaskFromRCTHex	= 21,
		needMoreMoneyThanFound			= 90
	};
	static inline string err_msg_from_err_code__create_transaction(CreateTransactionErrorCode code)
	{
		switch (code) {
			case noError:
				return "No error";
			case couldntDecodeToAddress:
				return "Couldn't decode address";
			case noDestinations:
				return "No destinations provided";
			case wrongNumberOfMixOutsProvided:
				return "Wrong number of mix outputs provided";
			case notEnoughOutputsForMixing:
				return "Not enough outputs for mixing";
			case invalidSecretKeys:
				return "Invalid secret keys";
			case outputAmountOverflow:
				return "Output amount overflow";
			case inputAmountOverflow:
				return "Input amount overflow";
			case mixRCTOutsMissingCommit:
				return "Mix RCT outs missing commit";
			case resultFeeNotEqualToGiven:
				return "Result fee not equal to given fee";
			case needMoreMoneyThanFound:
				return "Spendable balance too low";
			case invalidDestinationAddress:
				return "Invalid destination address";
			case nonZeroPIDWithIntAddress:
				return "Payment ID must be blank when using an integrated address";
			case cantUsePIDWithSubAddress:
				return "Payment ID must be blank when using a subaddress";
			case couldntAddPIDNonceToTXExtra:
				return "Couldn't add nonce to tx extra";
			case givenAnInvalidPubKey:
				return "Invalid pub key";
			case invalidCommitOrMaskOnOutputRCT:
				return "Invalid commit or mask on output rct";
			case transactionNotConstructed:
				return "Transaction not constructed";
			case transactionTooBig:
				return "Transaction too big";
			case notYetImplemented:
				return "Not yet implemented";
			case invalidPID:
				return "Invalid payment ID";
			case enteredAmountTooLow:
				return "The amount you've entered is too low";
			case cantGetDecryptedMaskFromRCTHex:
				return "Can't get decrypted mask from 'rct' hex";
		}
	}
	//
	// See monero_send_routine for actual app-lvl interface used by lightwallets 
	//
	//
	// Send_Step* functions procedure for integrators:
	//	1. call GetUnspentOuts endpoint
	//	2. call step1__prepare_params_for_get_decoys to get params for calling RandomOuts; call GetRandomOuts
	//	3. call step2__try_… with retVals from Step1 (incl using_outs, RandomOuts)
	//		3a. While tx must be reconstructed, re-call step1 passing step2 fee_actually_needed as passedIn_attemptAt_fee, then re-request RandomOuts again, and call step2 again
	//		3b. If good tx constructed, proceed to submit/save the tx
	// Note: This separation of steps fully encodes SendFunds_ProcessStep
	//
	struct Send_Step1_RetVals
	{
		CreateTransactionErrorCode errCode; // if != noError, abort Send process
		// for display / information purposes on errCode=needMoreMoneyThanFound during step1:
		uint64_t spendable_balance; //  (effectively but not the same as spendable_balance)
		uint64_t required_balance; // for display / information purposes on errCode=needMoreMoneyThanFound during step1
		//
		// Success case return values
		uint32_t mixin;
		vector<SpendableOutput> using_outs;
		uint64_t using_fee;
		uint64_t final_total_wo_fee;
		uint64_t change_amount;
	};
	void send_step1__prepare_params_for_get_decoys(
		Send_Step1_RetVals &retVals,
		//
		const optional<string>& payment_id_string,
		uint64_t sending_amount,
		bool is_sweeping,
		uint32_t simple_priority,
		use_fork_rules_fn_type use_fork_rules_fn,
		//
		const vector<SpendableOutput> &unspent_outs,
		uint64_t fee_per_b, // per v8
		uint64_t fee_quantization_mask,
		//
		optional<uint64_t> passedIn_attemptAt_fee // use this for passing step2 "must-reconstruct" return values back in, i.e. re-entry; when nil, defaults to attempt at network min
	);
	//
	struct Send_Step2_RetVals
	{
		CreateTransactionErrorCode errCode; // if != noError, abort Send process
		//
		// Reconstruct-required parameters:
		bool tx_must_be_reconstructed; // if true, re-request RandomOuts with the following parameters and retry step3
		uint64_t fee_actually_needed; // will be non-zero if tx_must_be_reconstructed
		//
		// Success parameters:
		optional<string> signed_serialized_tx_string;
		optional<string> tx_hash_string;
		optional<string> tx_key_string; // this includes additional_tx_keys
		optional<string> tx_pub_key_string; // from get_tx_pub_key_from_extra()
	};
	void send_step2__try_create_transaction(
		Send_Step2_RetVals &retVals,
		//
		const string &from_address_string,
		const string &sec_viewKey_string,
		const string &sec_spendKey_string,
		const string &to_address_string,
		const optional<string>& payment_id_string,
		uint64_t final_total_wo_fee, // this gets passed to create_transaction's 'sending_amount'
		uint64_t change_amount,
		uint64_t fee_amount,
		uint32_t simple_priority,
		const vector<SpendableOutput> &using_outs,
		uint64_t fee_per_b, // per v8
		uint64_t fee_quantization_mask,
		vector<RandomAmountOutputs> &mix_outs, // it gets sorted
		use_fork_rules_fn_type use_fork_rules_fn,
		uint64_t unlock_time, // or 0
		cryptonote::network_type nettype
	);
	//
	//
	// Lower level functions - generally you won't need to call these (these are what used to live in cn_utils.js)
	//
	struct Convenience_TransactionConstruction_RetVals
	{
		CreateTransactionErrorCode errCode;
		//
		optional<string> signed_serialized_tx_string;
		optional<string> tx_hash_string;
		optional<string> tx_key_string; // this includes additional_tx_keys
		optional<string> tx_pub_key_string; // from get_tx_pub_key_from_extra()
		optional<transaction> tx; // for block weight
		optional<size_t> txBlob_byteLength;
	};
	void convenience__create_transaction(
		Convenience_TransactionConstruction_RetVals &retVals,
		const string &from_address_string,
		const string &sec_viewKey_string,
		const string &sec_spendKey_string,
		const string &to_address_string,
		const optional<string>& payment_id_string,
		uint64_t sending_amount,
		uint64_t change_amount,
		uint64_t fee_amount,
		const vector<SpendableOutput> &outputs,
		vector<RandomAmountOutputs> &mix_outs, // get sorted
		use_fork_rules_fn_type use_fork_rules_fn,
		uint64_t unlock_time							= 0, // or 0
		network_type nettype 							= MAINNET
	);
	struct TransactionConstruction_RetVals
	{
		CreateTransactionErrorCode errCode;
		//
		optional<transaction> tx;
		optional<secret_key> tx_key;
		optional<vector<secret_key>> additional_tx_keys;
	};
	void create_transaction(
		TransactionConstruction_RetVals &retVals,
		const account_keys& sender_account_keys, // this will reference a particular hw::device
		const uint32_t subaddr_account_idx, // pass 0 for no subaddrs
		const std::unordered_map<crypto::public_key, cryptonote::subaddress_index> &subaddresses,
		const address_parse_info &to_addr, // this _must_ include correct .is_subaddr
		uint64_t sending_amount,
		uint64_t change_amount,
		uint64_t fee_amount,
		const vector<SpendableOutput> &outputs,
		vector<RandomAmountOutputs> &mix_outs,
		const std::vector<uint8_t> &extra, // this is not declared const b/c it may have the output tx pub key appended to it
		use_fork_rules_fn_type use_fork_rules_fn,
		uint64_t unlock_time							= 0, // or 0
		bool rct 										= true,
		network_type nettype							= MAINNET
	);
}

#endif /* monero_transfer_utils_hpp */
