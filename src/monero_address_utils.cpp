//
//  monero_address_utils.cpp
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
#include "monero_address_utils.hpp"
using namespace monero;
using namespace address_utils;
//
#include "cryptonote_basic/cryptonote_basic_impl.h"
#include "string_tools.h"
#include "cryptonote_basic/account.h"
using namespace cryptonote;
//
#include "tools__ret_vals.hpp"
using namespace epee;
//
#include "wallet_errors.h"
//
#include "monero_paymentID_utils.hpp"
#include "serial_bridge_utils.hpp"
using namespace serial_bridge_utils;
//
// Accessors - Implementations
DecodedAddress_RetVals address_utils::decodedAddress(const string &addressString, cryptonote::network_type nettype)
{
	DecodedAddress_RetVals retVals; // init
	//
	cryptonote::address_parse_info info;
	bool didSucceed = cryptonote::get_account_address_from_str(
		info,
		nettype,
		addressString
	);
	if (didSucceed == false) {
		retVals.did_error = true;
		retVals.err_string = "Invalid address"; // TODO: return code
		//
		return retVals;
	}
	cryptonote::account_public_address address = info.address;
	std::string pub_viewKey_hexString = string_tools::pod_to_hex(address.m_view_public_key);
	std::string pub_spendKey_hexString = string_tools::pod_to_hex(address.m_spend_public_key);
	{
		retVals.pub_viewKey_string = std::move(pub_viewKey_hexString);
		retVals.pub_spendKey_string = std::move(pub_spendKey_hexString);
		if (info.has_payment_id == true) {
			crypto::hash8 payment_id = info.payment_id;
			retVals.paymentID_string = string_tools::pod_to_hex(payment_id);
		}
		retVals.isSubaddress = info.is_subaddress;
	}
	return retVals;
}
bool address_utils::isSubAddress(const string &addressString, cryptonote::network_type nettype)
{
	DecodedAddress_RetVals retVals = decodedAddress(addressString, nettype);
	if (retVals.did_error) {
		return false; // just treat it as a no
	}
	return retVals.isSubaddress;
}
bool address_utils::isIntegratedAddress(const string &addressString, cryptonote::network_type nettype)
{
	DecodedAddress_RetVals retVals = decodedAddress(addressString, nettype);
	if (retVals.did_error) {
		return false; // just treat it as a no
	}
	return retVals.paymentID_string != boost::none;
}
//
string address_utils::new_integratedAddrFromStdAddr(const string &std_address_string, const string &short_paymentID_string, cryptonote::network_type nettype)
{
	crypto::hash8 payment_id_short;
	bool didParse = monero_paymentID_utils::parse_short_payment_id(short_paymentID_string, payment_id_short);
	if (!didParse) {
		return error_ret_json_from_message("Not a valid payment Id");
	}
	cryptonote::address_parse_info info;
	bool didSucceed = cryptonote::get_account_address_from_str(
		info,
		nettype,
		std_address_string
	);
	if (didSucceed == false) {
		return error_ret_json_from_message("Not a valid address");
	}
	if (info.is_subaddress) {
		THROW_WALLET_EXCEPTION_IF(true, tools::error::wallet_internal_error, "new_integratedAddrFromStdAddr must not be called with a subaddress");
		return error_ret_json_from_message("Is a subaddress");
	}
	if (info.has_payment_id != false) {
		// could even throw / fatalError here
		return error_ret_json_from_message("Already assigned a payment Id"); // that was not a std_address!
	}
	std::string int_address_string = cryptonote::get_account_integrated_address_as_str(
		nettype,
		info.address,
		payment_id_short
	);
	boost::property_tree::ptree root;
	root.put("retVal", int_address_string);
	
	return ret_json_from_root(root);
}
