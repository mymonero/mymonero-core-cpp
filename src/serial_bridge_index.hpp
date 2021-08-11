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
#include <boost/optional.hpp>
//
// See serial_bridge_utils.hpp
//
namespace serial_bridge
{
	using namespace std;
	using namespace boost;
	using namespace cryptonote;
	//
	// Bridging Functions - these take and return JSON strings
	string send_step1__prepare_params_for_get_decoys(const string &args_string);
	string pre_step2_tie_unspent_outs_to_mix_outs_for_all_future_tx_attempts(const string &args_string);
	string send_step2__try_create_transaction(const string &args_string);
	//
	string decode_address(const string address, const string nettype);
	bool is_subaddress(const string address, const string nettype);
	bool is_integrated_address(const string address, const string nettype);
	//
	string new_integrated_address(const string address, const string paymentId, const string nettype);
	string new_payment_id();
	//
	string newly_created_wallet(const string localeLanguageCode, const string nettype);
	bool are_equal_mnemonics(const string mnemonicA, const string mnemonicB);
	string address_and_keys_from_seed(const string seed, const string nettype); // aka legacy mymonero-core-js:create_address
	string mnemonic_from_seed(const string seed, const string wordsetName);
	string seed_and_keys_from_mnemonic(const string mnemonic, const string nettype);
	string validate_components_for_login(const string address, const string privateViewKey, const string privateSpendKey, const string seed, const string nettype);
	//
	string estimated_tx_network_fee(const string priority, const string feePerb, const string forkVersion);
	string generate_key_image(const string txPublicKey, const string privateViewKey, const string publicSpendKey, const string privateSpendKey, const string outputIndex);
}

#endif /* serial_bridge_index_hpp */
