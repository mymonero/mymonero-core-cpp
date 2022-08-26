//
//  monero_fork_rules.cpp
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
//
#include "monero_fork_rules.hpp"
//
using namespace monero_fork_rules;
//
bool monero_fork_rules::lightwallet_hardcoded__use_fork_rules(uint8_t version, int64_t early_blocks)
{
	return true; // TODO - we don't have the actual fork rules from thje lightwallet server yet
	//
	// full wallets do:
//	uint64_t height, earliest_height;
//	boost::optional<std::string> result = m_node_rpc_proxy.get_height(height);
//	throw_on_rpc_response_error(result, "get_info");
//	result = m_node_rpc_proxy.get_earliest_height(version, earliest_height);
//	throw_on_rpc_response_error(result, "get_hard_fork_info");
//
//	bool close_enough = height >=  earliest_height - early_blocks; // start using the rules that many blocks beforehand
//	if (close_enough)
//		LOG_PRINT_L2("Using v" << (unsigned)version << " rules");
//	else
//		LOG_PRINT_L2("Not using v" << (unsigned)version << " rules");
//	return close_enough;	
}
//
// Protocol / Defaults
uint32_t monero_fork_rules::fixed_ringsize()
{
	return 16; // v15
}
uint32_t monero_fork_rules::fixed_mixinsize()
{
	return fixed_ringsize() - 1;
}
uint64_t monero_fork_rules::dust_threshold()
{
	return 2000000000;
}
