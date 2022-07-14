//
//  monero_fee_utils.hpp
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
#ifndef monero_fee_utils_hpp
#define monero_fee_utils_hpp
//
#include <boost/optional.hpp>
//
#include "string_tools.h"
//
#include "crypto.h"
#include "cryptonote_basic.h"
#include "cryptonote_format_utils.h"
//
#include "monero_fork_rules.hpp"
//
namespace monero_fee_utils
{
	using namespace std;
	using namespace boost;
	using namespace cryptonote;
	using namespace monero_fork_rules;
	using namespace crypto;
	//
	uint32_t default_priority();
	//
	uint64_t get_upper_transaction_weight_limit(uint64_t upper_transaction_weight_limit__or_0_for_default, use_fork_rules_fn_type use_fork_rules_fn);
	uint64_t get_fee_multiplier(uint32_t priority, uint32_t default_priority, int fee_algorithm, use_fork_rules_fn_type use_fork_rules_fn);
	int get_fee_algorithm(use_fork_rules_fn_type use_fork_rules_fn);
	uint64_t get_base_fee(uint64_t fee_per_b);
	//
	uint64_t estimate_fee(bool use_per_byte_fee, bool use_rct, int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof, bool clsag, uint64_t base_fee, uint64_t fee_multiplier, uint64_t fee_quantization_mask);
	//
	uint64_t calculate_fee_from_weight(uint64_t base_fee, uint64_t weight, uint64_t fee_multiplier, uint64_t fee_quantization_mask);
	uint64_t calculate_fee(bool use_per_byte_fee, const cryptonote::transaction &tx, size_t blob_size, uint64_t base_fee, uint64_t fee_multiplier, uint64_t fee_quantization_mask);
	//
	/*Added*/ uint64_t calculate_fee_from_size(uint64_t fee_per_b, size_t bytes, uint64_t fee_multiplier);
	//
	size_t estimate_rct_tx_size(int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof, bool clsag);
	uint64_t estimate_tx_weight(bool use_rct, int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof, bool clsag);
	size_t estimate_tx_size(bool use_rct, int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof, bool clsag);
	uint64_t estimated_tx_network_fee( // convenience function for size + calc
		uint64_t fee_per_b,
		uint32_t priority, // when priority=0, falls back to monero_fee_utils::default_priority()
		use_fork_rules_fn_type use_fork_rules_fn // this is extracted to a function so that implementations can optionally query the daemon (although this presently implies that such a call remains blocking)
	);
}

#endif /* monero_fee_utils_hpp */
