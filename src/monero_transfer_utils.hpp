//
//  monero_transfer_utils.hpp
//  Copyright (c) 2014-2018, MyMonero.com
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
//
using namespace tools;
#include "tools__ret_vals.hpp"
//
// used to choose when to stop adding outputs to a tx
#define APPROXIMATE_INPUT_BYTES 80
//
namespace monero_transfer_utils
{
	using namespace std;
	using namespace boost;
	using namespace cryptonote;
	using namespace monero_fork_rules;
	using namespace crypto;
	//
	uint64_t get_upper_transaction_size_limit(uint64_t upper_transaction_size_limit__or_0_for_default, use_fork_rules_fn_type use_fork_rules_fn);
	uint64_t get_fee_multiplier(uint32_t priority, uint32_t default_priority, int fee_algorithm, use_fork_rules_fn_type use_fork_rules_fn);
	int get_fee_algorithm(use_fork_rules_fn_type use_fork_rules_fn);
	//
	uint64_t calculate_fee(uint64_t fee_per_kb, size_t bytes, uint64_t fee_multiplier);
	uint64_t calculate_fee(uint64_t fee_per_kb, const blobdata &blob, uint64_t fee_multiplier);
	//
	size_t estimate_rct_tx_size(int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof);
	size_t estimate_tx_size(bool use_rct, int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof);
	uint64_t estimated_tx_network_fee( // convenience function for size + calc
		uint64_t fee_per_kb,
		uint32_t priority, // when priority=0, falls back to monero_transfer_utils::default_priority()
		network_type nettype,
		use_fork_rules_fn_type use_fork_rules_fn // this is extracted to a function so that implementations can optionally query the daemon (although this presently implies that such a call remains blocking)
	);
	//
	bool is_transfer_unlocked(uint64_t unlock_time, uint64_t block_height, uint64_t blockchain_size, network_type nettype = MAINNET);
	bool is_tx_spendtime_unlocked(uint64_t unlock_time, uint64_t block_height, uint64_t blockchain_size, network_type nettype = MAINNET);
	//
	uint32_t fixed_ringsize(); // not mixinsize, which would be ringsize-1
	uint32_t fixed_mixinsize(); // not ringsize, which would be mixinsize+1
	uint32_t default_priority();
	//
	string new_dummy_address_string_for_rct_tx(network_type nettype = MAINNET);
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
		needMoreMoneyThanFound			= 9,
		invalidDestinationAddress		= 10,
		nonZeroPIDWithIntAddress		= 11,
		cantUsePIDWithSubAddress		= 12,
		couldntSetPIDToTXExtra			= 13,
		givenAnInvalidPubKey			= 14,
		invalidCommitOrMaskOnOutputRCT	= 15,
		transactionNotConstructed		= 16,
		transactionTooBig				= 17
	};
	struct TransactionConstruction_RetVals
	{
		CreateTransactionErrorCode errCode;
		//
		cryptonote::transaction tx;
	};
	// TODO: add priority
	void create_transaction(
		TransactionConstruction_RetVals &retVals,
		const account_keys& sender_account_keys, // this will reference a particular hw::device
		const uint32_t subaddr_account_idx, // pass 0 for no subaddrs
		const std::unordered_map<crypto::public_key, cryptonote::subaddress_index> &subaddresses,
		const vector<tx_destination_entry> &dsts, // presently, this must include change as well as, if necessary, dummy output
		vector<SpendableOutput> &outputs,
		vector<RandomAmountOutputs> &mix_outs,
		uint64_t fee_amount,
		std::vector<uint8_t> &extra, // this is not declared const b/c it may have the output tx pub key appended to it
		use_fork_rules_fn_type use_fork_rules_fn,
		uint64_t unlock_time							= 0, // or 0
		bool rct 										= true,
		network_type nettype							= MAINNET
	);
}

#endif /* monero_transfer_utils_hpp */
