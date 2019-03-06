//
//  monero_fork_rules.hpp
//  MyMonero
//
//  Created by Paul Shapiro on 1/9/18.
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

#ifndef monero_fork_rules_hpp
#define monero_fork_rules_hpp

#include <functional>
#include <stdint.h>

namespace monero_fork_rules
{
	typedef std::function<bool(uint8_t/*version*/, int64_t/*early_blocks*/)> use_fork_rules_fn_type;
	//
	bool lightwallet_hardcoded__use_fork_rules(uint8_t version, int64_t early_blocks); // convenience - to be called by a use_fork_rules_fn_type implementation
	//
	// The fork_version should be the actual current network fork version.
	// If zero, it is ignored and the resulting functor always returns true.
	inline use_fork_rules_fn_type make_use_fork_rules_fn(uint8_t fork_version)
	{
		return 0 != fork_version ?
			[fork_version](uint8_t desired_version, int64_t/*early_blocks is ignored*/)
			{
				return desired_version <= fork_version;
			}
			: use_fork_rules_fn_type(lightwallet_hardcoded__use_fork_rules);
	}
	//
	uint32_t fixed_ringsize(); // not mixinsize, which would be ringsize-1
	uint32_t fixed_mixinsize(); // not ringsize, which would be mixinsize+1
	//
	uint64_t dust_threshold();
}

#endif /* monero_fork_rules */
