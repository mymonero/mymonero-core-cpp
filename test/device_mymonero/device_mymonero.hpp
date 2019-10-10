// Copyright (c) 2017-2019, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//		conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//		of conditions and the following disclaimer in the documentation and/or other
//		materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//		used to endorse or promote products derived from this software without specific
//		prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef MONERO_DEVICE_MYMONERO_H
#define MONERO_DEVICE_MYMONERO_H

#include "device/device.hpp"

#ifndef USE_DEVICE_MYMONERO
#define USE_DEVICE_MYMONERO 1
#endif
#ifndef WITH_DEVICE_MYMONERO
#define WITH_DEVICE_MYMONERO 1
#endif

#include <cstddef>
#include <string>
#include <boost/scope_exit.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include "device/device_default.hpp"
//#include "device/device_cold.hpp" // TODO: implement cold signing/KI protocol
#include "cryptonote_config.h"
#include "device_mymonero_base.hpp"

namespace hw {
namespace mymonero {

class device_mymonero;

class device_mymonero : public hw::mymonero::device_mymonero_base/*, public hw::device_cold */
{
protected:


public:
	device_mymonero();
	virtual ~device_mymonero() override;
//			device_mymonero(const device_mymonero &device) = delete ;
//			device_mymonero& operator=(const device_mymonero &device) = delete;

	explicit operator bool() const override {return true;}

	void  set_network_type(cryptonote::network_type network_type) override {
		this->network_type = network_type;
	}

	bool get_public_address(cryptonote::account_public_address &pubkey) override;
	bool get_secret_keys(crypto::secret_key &viewkey , crypto::secret_key &spendkey) override;

	/**
	 * Implements hw::device interface
	 * called from generate_key_image_helper_precomp()
	 */
	bool compute_key_image(
		const ::cryptonote::account_keys& ack,
		const ::crypto::public_key& out_key,
		const ::crypto::key_derivation& recv_derivation,
		size_t real_output_index,
		const ::cryptonote::subaddress_index& received_index,
		::cryptonote::keypair& in_ephemeral,
		::crypto::key_image& ki
	) override;

//			/**
//			 * Signs unsigned transaction with the device.
//			 */
//			void tx_sign(wallet_shim * wallet,
//				const ::tools::wallet2::unsigned_tx_set & unsigned_tx,
//				::tools::wallet2::signed_tx_set & signed_tx,
//				hw::tx_aux_data & aux_data
//			) override;
};

}
}
#endif //MONERO_DEVICE_MYMONERO_H
