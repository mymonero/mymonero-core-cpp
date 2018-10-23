//
//  serial_bridge_index.hpp
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

#ifndef serial_bridge_index_hpp
#define serial_bridge_index_hpp
//
#include <string>
#include "cryptonote_config.h" 
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
	//
	string generate_key_image(const string &args_string);
	//
	string generate_key_derivation(const string &args_string);
	string derive_public_key(const string &args_string);
	string derive_subaddress_public_key(const string &args_string);
	string decodeRct(const string &args_string);
	//
	// JSON values
	network_type nettype_from_string(const string &nettype_string);
	string string_from_nettype(network_type nettype);
	//
	// JSON keys - Ret vals
	// - - Error
	static inline string ret_json_key__any__err_msg() { return "err_msg"; } // optional
	static inline string ret_json_key__any__err_code() { return "err_code"; } // optional
	//
	// - - Shared
	static inline string ret_json_key__generic_retVal() { return "retVal"; } 
	// - - create_transaction / send
	static inline string ret_json_key__send__spendable_balance() { return "spendable_balance"; }
	static inline string ret_json_key__send__required_balance() { return "required_balance"; }
	static inline string ret_json_key__send__mixin() { return "mixin"; }
	static inline string ret_json_key__send__using_fee() { return "using_fee"; }
	static inline string ret_json_key__send__final_total_wo_fee() { return "final_total_wo_fee"; }
	static inline string ret_json_key__send__change_amount() { return "change_amount"; }
	static inline string ret_json_key__send__using_outs() { return "using_outs"; } // this list's members' keys should probably be declared (is this the best way to do this?)
	//
	static inline string ret_json_key__send__tx_must_be_reconstructed() { return "tx_must_be_reconstructed"; }
	static inline string ret_json_key__send__fee_actually_needed() { return "fee_actually_needed"; }
	static inline string ret_json_key__send__serialized_signed_tx() { return "serialized_signed_tx"; }
	static inline string ret_json_key__send__tx_hash() { return "tx_hash"; }
	static inline string ret_json_key__send__tx_key() { return "tx_key"; }
	//
	// - - decode_address, etc
	static inline string ret_json_key__paymentID_string() { return "paymentID_string"; } // optional
	static inline string ret_json_key__isSubaddress() { return "isSubaddress"; }
	static inline string ret_json_key__mnemonic_string() { return "mnemonic_string"; }
	static inline string ret_json_key__mnemonic_language() { return "mnemonic_language"; }
	static inline string ret_json_key__sec_seed_string() { return "sec_seed_string"; }
	static inline string ret_json_key__address_string() { return "address_string"; }
	static inline string ret_json_key__pub_viewKey_string() { return "pub_viewKey_string"; }
	static inline string ret_json_key__pub_spendKey_string() { return "pub_spendKey_string"; }
	static inline string ret_json_key__sec_viewKey_string() { return "sec_viewKey_string"; }
	static inline string ret_json_key__sec_spendKey_string() { return "sec_spendKey_string"; }
	static inline string ret_json_key__isValid() { return "isValid"; }
	static inline string ret_json_key__isInViewOnlyMode() { return "isInViewOnlyMode"; }
	static inline string ret_json_key__decodeRct_mask() { return "mask"; }
	static inline string ret_json_key__decodeRct_amount() { return "amount"; }
	// JSON keys - Args
	// TODO: (is there a better way of doing this?) structs with auto parse & serialization?
	//	static inline string args_json_key__
}

#endif /* serial_bridge_index_hpp */
