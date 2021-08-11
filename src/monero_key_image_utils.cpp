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
