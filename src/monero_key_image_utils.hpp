//
//  monero_key_image_utils.hpp
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
#ifndef monero_key_image_utils_hpp
#define monero_key_image_utils_hpp
//
#include "crypto.h"
#include "cryptonote_basic.h"
//
using namespace tools;
#include "tools__ret_vals.hpp"
//
namespace monero_key_image_utils
{
	struct KeyImageRetVals: RetVals_base
	{
		crypto::key_image calculated_key_image;
	};
	bool new__key_image(
		const crypto::public_key& account_pub_spend_key,
		const crypto::secret_key& account_sec_spend_key,
		const crypto::secret_key& account_sec_view_key,
		const crypto::public_key& tx_public_key,
		uint64_t out_index,
		KeyImageRetVals &KeyImageRetVals
	);
}
//
#endif /* monero_key_image_utils_hpp */
