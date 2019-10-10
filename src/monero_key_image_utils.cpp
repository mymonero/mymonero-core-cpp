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
#include <unordered_map>
#include "cryptonote_basic/subaddress_index.h"
#include "cryptonote_format_utils.h"
//
using namespace crypto;
using namespace cryptonote;
//
bool monero_key_image_utils::new__key_image(
	const cryptonote::account_keys &sender_account_keys,
	const crypto::public_key& tx_public_key,
	const crypto::public_key& out_public_key,
	uint64_t out_index,
	KeyImageRetVals &retVals
) {
	cryptonote::keypair in_ephemeral;
	std::unordered_map<crypto::public_key, cryptonote::subaddress_index> subaddresses;
	subaddresses[sender_account_keys.m_account_address.m_spend_public_key] = {0,0}; // the main address
	std::vector<crypto::public_key> empty__additional_tx_pub_keys; // TODO: is this correct??
	//
	bool r = generate_key_image_helper(
		sender_account_keys,
		subaddresses,
		out_public_key,
		tx_public_key,
		empty__additional_tx_pub_keys,
		out_index,
		in_ephemeral,
		retVals.calculated_key_image,
		sender_account_keys.get_device()
	);
	if (!r) {
		LOG_ERROR("Key image generation failed!");
		return false;
	}
	return true;
}
