//
//  serial_bridge_index.hpp
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

#ifndef serial_bridge_index_hpp
#define serial_bridge_index_hpp
//
#include <string>
#include "cryptonote_config.h"
//
// See serial_bridge_utils.hpp
//
namespace serial_bridge
{
	using namespace std;
	using namespace cryptonote;
	//
	// Bridging Functions - these take and return JSON strings
	string send_step1__prepare_params_for_get_decoys(const string &args_string);
	string send_step2__try_create_transaction(const string &args_string);
	//
	string decode_address(const string &args_string);
	string is_subaddress(const string &args_string);
	string is_integrated_address(const string &args_string);
	//
	string new_integrated_address(const string &args_string);
	string new_payment_id(const string &args_string);
	//
	string newly_created_wallet(const string &args_string);
	string are_equal_mnemonics(const string &args_string);
	string address_and_keys_from_seed(const string &args_string); // aka legacy mymonero-core-js:create_address
	string mnemonic_from_seed(const string &args_string);
	string seed_and_keys_from_mnemonic(const string &args_string);
	string validate_components_for_login(const string &args_string);
	//
	string estimated_tx_network_fee(const string &args_string);
	string estimate_fee(const string &args_string);
	string estimate_tx_weight(const string &args_string);
	string estimate_rct_tx_size(const string &args_string);
	//
	string generate_key_image(const string &args_string);
	//
	string generate_key_derivation(const string &args_string);
	string derive_public_key(const string &args_string);
	string derive_subaddress_public_key(const string &args_string);
	string derivation_to_scalar(const string &args_string);
	string decodeRct(const string &args_string);
	string decodeRctSimple(const string &args_string);
	string encrypt_payment_id(const string &args_string);
}

#endif /* serial_bridge_index_hpp */
