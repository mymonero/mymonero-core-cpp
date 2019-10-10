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

#include "device_mymonero/device_mymonero.hpp"
#include <boost/algorithm/string.hpp>
#include "cryptonote_basic_impl.h"
#include "string_tools.h"
using namespace epee;

using namespace std;
using namespace crypto;
using namespace cryptonote;
using namespace boost;

namespace hw {
namespace mymonero {

device_mymonero::device_mymonero() {}
device_mymonero::~device_mymonero() {}
//
bool device_mymonero::get_public_address(cryptonote::account_public_address &pubkey)
{
	auto address_string = string("43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg");
	cryptonote::address_parse_info info{};
	bool r = cryptonote::get_account_address_from_str(info, this->network_type, address_string);
	CHECK_AND_ASSERT_MES(r, false, "Could not parse returned address. Address parse failed");
	CHECK_AND_ASSERT_MES(!info.is_subaddress, false, "MyMonero returned a sub address");
	//
	pubkey = info.address;
	return true;
}
bool device_mymonero::get_secret_keys(crypto::secret_key &viewkey, crypto::secret_key &spendkey)
{
	string_tools::hex_to_pod(string("7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104"), viewkey);
	string_tools::hex_to_pod(string("4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803"), spendkey);
	//
	return true;
}
bool device_mymonero::compute_key_image(
	const ::cryptonote::account_keys& ack,
	const ::crypto::public_key& out_key,
	const ::crypto::key_derivation& recv_derivation,
	size_t real_output_index,
	const ::cryptonote::subaddress_index& received_index,
	::cryptonote::keypair& in_ephemeral,
	::crypto::key_image& ki
) {
	// live_refresh(ack.m_view_secret_key, out_key, recv_derivation, real_output_index, received_index, in_ephemeral, ki);
	
	// from deleted live_refresh
	// auto req = std::make_shared<messages::monero::MoneroLiveRefreshStepRequest>();
	// req->set_out_key(out_key.data, 32);
	// req->set_recv_deriv(recv_derivation.data, 32);
	// req->set_real_out_idx(real_output_index);
	// req->set_sub_addr_major(received_index.major);
	// req->set_sub_addr_minor(received_index.minor);

	// auto ack = this->client_exchange<messages::monero::MoneroLiveRefreshStepAck>(req);
	// protocol::ki::live_refresh_ack(view_key_priv, out_key, ack, in_ephemeral, ki);

	// TODO: implement for hot


	return true;
}
//
//
//void device_mymonero::tx_sign(wallet_shim * wallet,
//	const tools::wallet2::unsigned_tx_set & unsigned_tx,
//	tools::wallet2::signed_tx_set & signed_tx,
//	hw::tx_aux_data & aux_data
//) {
//	// TODO: cold
//}
//
//void device_mymonero::tx_sign(wallet_shim * wallet,
//	 const tools::wallet2::unsigned_tx_set & unsigned_tx,
//	 size_t idx,
//	 hw::tx_aux_data & aux_data,
//	 std::shared_ptr<protocol::tx::Signer> & signer
//) {
//	// TODO: cold
//}

}}
