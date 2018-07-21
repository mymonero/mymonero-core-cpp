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
#include "string_tools.h"
#include "crypto.h"
#include "cryptonote_basic.h"
#include "cryptonote_format_utils.h"
#include "cryptonote_tx_utils.h"
//
using namespace tools;
#include "tools__ret_vals.hpp"
//
// used to choose when to stop adding outputs to a tx
#define APPROXIMATE_INPUT_BYTES 80
// used to target a given block size (additional outputs may be added on top to build fee)
#define TX_SIZE_TARGET(bytes) (bytes*2/3)
//
namespace monero_transfer_utils
{
	//
	typedef std::function<bool(uint8_t, int64_t)> use_fork_rules_fn_type;
	//
	uint64_t get_upper_transaction_size_limit(uint64_t upper_transaction_size_limit__or_0_for_default, use_fork_rules_fn_type use_fork_rules_fn);
	uint64_t get_fee_multiplier(uint32_t priority, uint32_t default_priority, int fee_algorithm, use_fork_rules_fn_type use_fork_rules_fn);
	int get_fee_algorithm(use_fork_rules_fn_type use_fork_rules_fn);
	//
	uint64_t calculate_fee(uint64_t fee_per_kb, size_t bytes, uint64_t fee_multiplier);
	uint64_t calculate_fee(uint64_t fee_per_kb, const cryptonote::blobdata &blob, uint64_t fee_multiplier);
	//
	size_t estimate_rct_tx_size(int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof);
	size_t estimate_tx_size(bool use_rct, int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof);
	//
	bool is_transfer_unlocked(uint64_t unlock_time, uint64_t block_height, uint64_t blockchain_size, bool is_testnet = false);
	bool is_tx_spendtime_unlocked(uint64_t unlock_time, uint64_t block_height, uint64_t blockchain_size, bool is_testnet = false);
	//
	uint32_t fixed_ringsize(); // not mixinsize, which would be ringsize-1
	uint32_t fixed_mixinsize(); // not ringsize, which would be mixinsize+1
	//
	std::string new_dummy_address_string_for_rct_tx(bool isTestnet = false);
	//
	uint32_t default_priority();
}

#endif /* monero_transfer_utils_hpp */
