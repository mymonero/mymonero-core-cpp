//
//  serial_bridge_index.cpp
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
//
#include "serial_bridge_index.hpp"
//
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
//
#include "monero_fork_rules.hpp"
#include "monero_transfer_utils.hpp"
#include "wallet_errors.h"
//
//
using namespace std;
using namespace boost;
using namespace cryptonote;
using namespace monero_transfer_utils;
using namespace monero_fork_rules;
//
using namespace serial_bridge;
//
network_type nettype_from_string(string nettype_string)
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
string error_ret_json_from_message(string err_msg)
{
	boost::property_tree::ptree root;
	root.put(ret_json_key__any__err_msg(), err_msg);
	stringstream ss;
	boost::property_tree::write_json(ss, root);
	//
	return ss.str();
}
string error_ret_json_from_code(int code)
{
	boost::property_tree::ptree root;
	root.put(ret_json_key__any__err_code(), code);
	stringstream ss;
	boost::property_tree::write_json(ss, root);
	//
	return ss.str();
}
//
string serial_bridge::create_transaction(std::string args_string)
{
	std::stringstream ss;
	ss << args_string;
	boost::property_tree::ptree json_root;
	try {
		boost::property_tree::read_json(ss, json_root);
	} catch (std::exception const& e) {
		THROW_WALLET_EXCEPTION_IF(false, error::wallet_internal_error, "Invalid JSON");
		return error_ret_json_from_message("Invalid JSON"); // TODO: centralize
	}
	network_type nettype = nettype_from_string(json_root.get<string>("nettype_string"));
	//
	std::vector<cryptonote::tx_destination_entry> dsts;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &dst_desc, json_root.get_child("dsts"))
	{
		assert(dst_desc.first.empty()); // array elements have no names
		cryptonote::tx_destination_entry de;
		cryptonote::address_parse_info de_addr_info;
		THROW_WALLET_EXCEPTION_IF(!cryptonote::get_account_address_from_str(de_addr_info, nettype, dst_desc.second.get<string>("addr")), error::wallet_internal_error, "Invalid dsts.addr");
		de.addr = de_addr_info.address;
		de.is_subaddress = dst_desc.second.get<bool>("is_subaddress");
		THROW_WALLET_EXCEPTION_IF(de.is_subaddress != de_addr_info.is_subaddress, error::wallet_internal_error, "Expected dsts.is_subaddress = parsed is_subaddress");
		de.amount = stoull(dst_desc.second.get<string>("amount"));
		//
		dsts.push_back(de);
	}
	//
	vector<SpendableOutput> outputs;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &output_desc, json_root.get_child("outputs"))
	{
		assert(output_desc.first.empty()); // array elements have no names
		SpendableOutput out{};
		out.amount = stoull(output_desc.second.get<string>("amount"));
		out.public_key = output_desc.second.get<string>("public_key");
		out.rct = output_desc.second.get<string>("rct");
		out.global_index = stoull(output_desc.second.get<string>("global_index"));
		out.index = stoull(output_desc.second.get<string>("index"));
		out.tx_pub_key = output_desc.second.get<string>("tx_pub_key");
		//
		outputs.push_back(out);
	}
	//
	vector<RandomAmountOutputs> mix_outs;
	BOOST_FOREACH(boost::property_tree::ptree::value_type &mix_out_desc, json_root.get_child("mix_outs"))
	{
		assert(mix_out_desc.first.empty()); // array elements have no names
		auto amountAndOuts = RandomAmountOutputs{};
		amountAndOuts.amount = stoull(mix_out_desc.second.get<string>("amount"));
		BOOST_FOREACH(boost::property_tree::ptree::value_type &mix_out_output_desc, mix_out_desc.second.get_child("outputs"))
		{
			assert(mix_out_output_desc.first.empty()); // array elements have no names
			auto amountOutput = RandomAmountOutput{};
			amountOutput.global_index = stoull(mix_out_output_desc.second.get<string>("global_index")); // this is, I believe, presently supplied as a string by the API, probably to avoid overflow
			amountOutput.public_key = mix_out_output_desc.second.get<string>("public_key");
			amountOutput.rct = mix_out_output_desc.second.get<string>("rct");
			amountAndOuts.outputs.push_back(amountOutput);
		}
		mix_outs.push_back(amountAndOuts);
	}
	//
	Convenience_TransactionConstruction_RetVals retVals;
	monero_transfer_utils::convenience__create_transaction(
		retVals,
		json_root.get<string>("from_address_string"),
		json_root.get<string>("sec_viewKey_string"),
		json_root.get<string>("sec_spendKey_string"),
		json_root.get<string>("to_address_string"),
		json_root.get_optional<string>("payment_id_string"),
		stoull(json_root.get<string>("amount")), // to send
		stoull(json_root.get<string>("fee_amount")),
		dsts,
		outputs,
		mix_outs,
		[] (uint8_t version, int64_t early_blocks) -> bool
		{
		   return lightwallet_hardcoded__use_fork_rules(version, early_blocks);
		},
		0, // unlock_time
		nettype
	);
	if (retVals.errCode != noError) {
		return error_ret_json_from_code(retVals.errCode);
	}
	THROW_WALLET_EXCEPTION_IF(retVals.signed_serialized_tx_string == boost::none, error::wallet_internal_error, "Not expecting no signed_serialized_tx_string given no error");
	//
	boost::property_tree::ptree root;
	root.put(ret_json_key__create_transaction__serialized_signed_tx(), std::move(*(retVals.signed_serialized_tx_string)));
	root.put(ret_json_key__create_transaction__tx_hash(), std::move(*(retVals.tx_hash_string)));
	stringstream ret_ss;
	boost::property_tree::write_json(ret_ss, root);
	//
	return ret_ss.str();
}
