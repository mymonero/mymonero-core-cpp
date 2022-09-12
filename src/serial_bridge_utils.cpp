//
//  serial_bridge_utils.cpp
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
//
#include "serial_bridge_utils.hpp"
#include <boost/foreach.hpp>
#include "wallet_errors.h"
#include "string_tools.h"

using namespace tools;
using namespace std;
using namespace boost;
using namespace cryptonote;
//
using namespace serial_bridge_utils;
//
// TODO: factor these into a monero_bridge_utils and share with serial_bridge_utils (incl keys declarations there)
network_type serial_bridge_utils::nettype_from_string(const string &nettype_string)
{ // TODO: possibly move this to network_type declaration
	if (nettype_string == "MAINNET") {
		return MAINNET;
	} else if (nettype_string == "TESTNET") {
		return TESTNET;
	} else if (nettype_string == "STAGENET") {
		return STAGENET;
	} else if (nettype_string == "FAKECHAIN") {
		return FAKECHAIN;
	} else if (nettype_string == "UNDEFINED") {
		return UNDEFINED;
	}
	THROW_WALLET_EXCEPTION_IF(false, error::wallet_internal_error, "Unrecognized nettype_string")
	return UNDEFINED;
}
string serial_bridge_utils::string_from_nettype(network_type nettype)
{
	switch (nettype) {
		case MAINNET:
			return "MAINNET";
		case TESTNET:
			return "TESTNET";
		case STAGENET:
			return "STAGENET";
		case FAKECHAIN:
			return "FAKECHAIN";
		case UNDEFINED:
			return "UNDEFINED";
		default:
			THROW_WALLET_EXCEPTION_IF(false, error::wallet_internal_error, "Unrecognized nettype for string conversion")
			return "UNDEFINED";
	}
}
//
// Shared - Parsing - Values
optional<double> serial_bridge_utils::none_or_double_from(const boost::property_tree::ptree &json, const string &key)
{
	optional<string> str = json.get_optional<string>(key);
	if (str != none) {
		return stod(*str); // this may throw an exception - allowing it to bubble up here
	}
	optional<double> dbl_orNone = json.get_optional<double>(key);
	//
	return dbl_orNone;
}
optional<bool> serial_bridge_utils::none_or_bool_from(const boost::property_tree::ptree &json, const string &key)
{
	optional<string> str = json.get_optional<string>(key);
	if (str != none) {
		if (*str == "true" || *str == "1") {
			return true;
		} else if (*str == "false" || *str == "0") {
			return false;
		} else {
			BOOST_THROW_EXCEPTION(logic_error("Unable to parse bool string"));
			return none;
		}
	}
	optional<bool> bool_orNone = json.get_optional<bool>(key);
	//
	return bool_orNone;
}
//
// Shared - Parsing - Args
bool serial_bridge_utils::parsed_json_root(const string &args_string, boost::property_tree::ptree &json_root)
{
//	cout << "args_string: " << args_string << endl;
	std::istringstream ss(args_string);
	try {
		boost::property_tree::read_json(ss, json_root);
	} catch (std::exception const& e) {
		THROW_WALLET_EXCEPTION_IF(false, error::wallet_internal_error, "Invalid JSON");
		return false;
	}
	return true;
}
//
// Shared - Factories - Return values
string serial_bridge_utils::ret_json_from_root(const boost::property_tree::ptree &root)
{
	ostringstream ret_ss;
	boost::property_tree::write_json(ret_ss, root, false/*pretty*/);
	//
	return ret_ss.str();
}
string serial_bridge_utils::error_ret_json_from_message(const string &err_msg)
{
	boost::property_tree::ptree root;
	root.put("err_msg", err_msg);
	//
	return ret_json_from_root(root);
}
string serial_bridge_utils::error_ret_json_from_code(int code, optional<string> err_msg)
{
	boost::property_tree::ptree root;
	root.put("err_code", code);
	if (err_msg != none) {
		root.put("err_msg", *err_msg);
	}
	//
	return ret_json_from_root(root);
}
