//
//  monero_key_image_utils.cpp
//  MyMonero
//
//  Created by Paul Shapiro on 1/2/18.
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
#include "monero_key_image_utils.hpp"
//
using namespace crypto;
using namespace cryptonote;
//
bool monero_key_image_utils::new__key_image(
	const crypto::public_key& account_pub_spend_key,
	const crypto::secret_key& account_sec_spend_key,
	const crypto::secret_key& account_sec_view_key,
	const crypto::public_key& tx_public_key,
	uint64_t out_index,
	KeyImageRetVals &retVals
) {
	retVals = {};
	//
	bool r = false;
	//
	// "Subaddresses aren't supported in mymonero/openmonero yet. Roll out the original scheme:
	//   compute D = a*R
	//   compute P = Hs(D || i)*G + B
	//   compute x = Hs(D || i) + b      (and check if P==x*G)
	//   compute I = x*Hp(P)"
	crypto::key_derivation derivation;
	r = crypto::generate_key_derivation(tx_public_key, account_sec_view_key, derivation);
	if (!r) {
		retVals.did_error = true;
		std::ostringstream ss{};
		ss << "failed to generate_key_derivation(" << tx_public_key << ", " << account_sec_view_key << ")";
		retVals.err_string = ss.str();
		//
		return false;
	}
	cryptonote::keypair in_ephemeral;
	r = crypto::derive_public_key(derivation, out_index, account_pub_spend_key, in_ephemeral.pub);
	if (!r) {
		retVals.did_error = true;
		std::ostringstream ss{};
		ss << "failed to derive_public_key (" << derivation << ", " << out_index << ", " << account_pub_spend_key << ")";
		retVals.err_string = ss.str();
		//
		return false;
	}
	crypto::derive_secret_key(derivation, out_index, account_sec_spend_key, in_ephemeral.sec);
	crypto::public_key out_pkey_test;
	r = crypto::secret_key_to_public_key(in_ephemeral.sec, out_pkey_test);
	if (!r) {
		retVals.did_error = true;
		std::ostringstream ss{};
		ss << "failed to secret_key_to_public_key(" << in_ephemeral.sec << ")";
		retVals.err_string = ss.str();
		//
		return false;
	}
	if (in_ephemeral.pub != out_pkey_test) {
		retVals.did_error = true;
		retVals.err_string = "derived secret key doesn't match derived public key";
		//
		return false;
	}
	crypto::generate_key_image(in_ephemeral.pub, in_ephemeral.sec, retVals.calculated_key_image);
	//
	return true;
}

//+ (NSString *)new_keyImageFrom_tx_pub_key:(NSString *)tx_pub_key_NSString
//sec_spendKey:(NSString *)sec_spendKey_NSString
//sec_viewKey:(NSString *)sec_viewKey_NSString
//pub_spendKey:(NSString *)pub_spendKey_NSString
//out_index:(uint64_t)out_index
//{
//	crypto::secret_key sec_viewKey{};
//	crypto::secret_key sec_spendKey{};
//	crypto::public_key pub_spendKey{};
//	crypto::public_key tx_pub_key{};
//	{ // Would be nice to find a way to avoid converting these back and forth
//		bool r = false;
//		r = string_tools::hex_to_pod(std::string(sec_viewKey_NSString.UTF8String), sec_viewKey);
//		NSAssert(r, @"Invalid secret view key");
//		r = string_tools::hex_to_pod(std::string(sec_spendKey_NSString.UTF8String), sec_spendKey);
//		NSAssert(r, @"Invalid secret spend key");
//		r = string_tools::hex_to_pod(std::string(pub_spendKey_NSString.UTF8String), pub_spendKey);
//		NSAssert(r, @"Invalid public spend key");
//		r = string_tools::hex_to_pod(std::string(tx_pub_key_NSString.UTF8String), tx_pub_key);
//		NSAssert(r, @"Invalid tx pub key");
//	}
//	monero_key_image_utils::KeyImageRetVals retVals;
//	{
//		bool r = monero_key_image_utils::new__key_image(pub_spendKey, sec_spendKey, sec_viewKey, tx_pub_key, out_index, retVals);
//		if (!r) {
//			return nil; // TODO: return error string? (unwrap optional)
//		}
//	}
//	std::string key_image_hex_string = string_tools::pod_to_hex(retVals.calculated_key_image);
//	NSString *key_image_hex_NSString = [NSString stringWithUTF8String:key_image_hex_string.c_str()];
//	//
//	return key_image_hex_NSString;
//}
