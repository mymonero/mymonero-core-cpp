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
#include "monero_address_utils.hpp" // TODO: split this/these out into a different namespaces or file so this file can scale (leave file for shared utils)
#include "monero_paymentID_utils.hpp"
#include "monero_wallet_utils.hpp"
#include "monero_key_image_utils.hpp"
#include "wallet_errors.h"
#include "string_tools.h"
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
network_type serial_bridge::nettype_from_string(const string &nettype_string)
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
string serial_bridge::string_from_nettype(network_type nettype)
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
// Shared - Parsing - Args
bool parsed_json_root(const string &args_string, boost::property_tree::ptree &json_root)
{
	std::stringstream ss;
	ss << args_string;
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
string ret_json_from_root(const boost::property_tree::ptree &root)
{
	stringstream ret_ss;
	boost::property_tree::write_json(ret_ss, root, false/*pretty*/);
	//
	return ret_ss.str();
}
string error_ret_json_from_message(string err_msg)
{
	boost::property_tree::ptree root;
	root.put(ret_json_key__any__err_msg(), err_msg);
	//
	return ret_json_from_root(root);
}
string error_ret_json_from_code(int code)
{
	boost::property_tree::ptree root;
	root.put(ret_json_key__any__err_code(), code);
	//
	return ret_json_from_root(root);
}
//
//
// Bridge Function Implementations
//
string serial_bridge::decode_address(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	auto retVals = monero::address_utils::decodedAddress(json_root.get<string>("address"), nettype_from_string(json_root.get<string>("nettype_string")));
	if (retVals.did_error) {
		return error_ret_json_from_message(*(retVals.err_string));
	}
	boost::property_tree::ptree root;
	root.put(ret_json_key__isSubaddress(), retVals.isSubaddress);
	root.put(ret_json_key__pub_viewKey_string(), std::move(*(retVals.pub_viewKey_string)));
	root.put(ret_json_key__pub_spendKey_string(), std::move(*(retVals.pub_spendKey_string)));
	if (retVals.paymentID_string != none) {
		root.put(ret_json_key__paymentID_string(), std::move(*(retVals.paymentID_string)));
	}
	//
	return ret_json_from_root(root);
}
string serial_bridge::is_subaddress(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	bool retVal = monero::address_utils::isSubAddress(json_root.get<string>("address"), nettype_from_string(json_root.get<string>("nettype_string")));
	boost::property_tree::ptree root;
	root.put(ret_json_key__generic_retVal(), retVal);
	//
	return ret_json_from_root(root);
}
string serial_bridge::is_integrated_address(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	bool retVal = monero::address_utils::isIntegratedAddress(json_root.get<string>("address"), nettype_from_string(json_root.get<string>("nettype_string")));
	boost::property_tree::ptree root;
	root.put(ret_json_key__generic_retVal(), retVal);
	//
	return ret_json_from_root(root);
}
string serial_bridge::new_integrated_address(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	optional<string> retVal = monero::address_utils::new_integratedAddrFromStdAddr(json_root.get<string>("address"), json_root.get<string>("short_pid"), nettype_from_string(json_root.get<string>("nettype_string")));
	boost::property_tree::ptree root;
	if (retVal != none) {
		root.put(ret_json_key__generic_retVal(), std::move(*retVal));
	}
	//
	return ret_json_from_root(root);
}
string serial_bridge::new_fake_address_for_rct_tx(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	optional<string> retVal = monero::address_utils::new_fake_address_string_for_rct_tx(
		nettype_from_string(json_root.get<string>("nettype_string"))
	);
	boost::property_tree::ptree root;
	if (retVal != none) {
		root.put(ret_json_key__generic_retVal(), std::move(*retVal));
	}
	//
	return ret_json_from_root(root);
}
string serial_bridge::new_payment_id(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	optional<string> retVal = monero_paymentID_utils::new_short_plain_paymentID_string();
	boost::property_tree::ptree root;
	if (retVal != none) {
		root.put(ret_json_key__generic_retVal(), std::move(*retVal));
	}
	//
	return ret_json_from_root(root);
}
//
string serial_bridge::newly_created_wallet(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	monero_wallet_utils::WalletDescriptionRetVals retVals;
	bool r = monero_wallet_utils::convenience__new_wallet_with_language_code(
		json_root.get<string>("locale_language_code"),
		retVals,
		nettype_from_string(json_root.get<string>("nettype_string"))
	);
	bool did_error = retVals.did_error;
	if (!r) {
		return error_ret_json_from_message(*(retVals.err_string));
	}
	THROW_WALLET_EXCEPTION_IF(did_error, error::wallet_internal_error, "Illegal success flag but did_error");
	//
	boost::property_tree::ptree root;
	root.put(ret_json_key__mnemonic_string(), (*(retVals.optl__desc)).mnemonic_string);
	root.put(ret_json_key__mnemonic_language(), (*(retVals.optl__desc)).mnemonic_language);
	root.put(ret_json_key__sec_seed_string(), (*(retVals.optl__desc)).sec_seed_string);
	root.put(ret_json_key__address_string(), (*(retVals.optl__desc)).address_string);
	root.put(ret_json_key__pub_viewKey_string(), epee::string_tools::pod_to_hex((*(retVals.optl__desc)).pub_viewKey));
	root.put(ret_json_key__sec_viewKey_string(), epee::string_tools::pod_to_hex((*(retVals.optl__desc)).sec_viewKey));
	root.put(ret_json_key__pub_spendKey_string(), epee::string_tools::pod_to_hex((*(retVals.optl__desc)).pub_spendKey));
	root.put(ret_json_key__sec_spendKey_string(), epee::string_tools::pod_to_hex((*(retVals.optl__desc)).sec_spendKey));
	//
	return ret_json_from_root(root);
}
string serial_bridge::are_equal_mnemonics(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	bool equal;
	try {
		equal = monero_wallet_utils::are_equal_mnemonics(
			json_root.get<string>("a"),
			json_root.get<string>("b")
		);
	} catch (std::exception const& e) {
		return error_ret_json_from_message(e.what());
	}
	boost::property_tree::ptree root;
	root.put(ret_json_key__generic_retVal(), equal);
	//
	return ret_json_from_root(root);
}
string serial_bridge::mnemonic_from_seed(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	monero_wallet_utils::SeedDecodedMnemonic_RetVals retVals = monero_wallet_utils::mnemonic_string_from_seed_hex_string(
		json_root.get<string>("seed_string"),
		json_root.get<string>("wordset_name")
	);
	boost::property_tree::ptree root;
	if (retVals.err_string != none) {
		return error_ret_json_from_message(*(retVals.err_string));
	}
	root.put(ret_json_key__generic_retVal(), *(retVals.mnemonic_string));
	//
	return ret_json_from_root(root);
}
string serial_bridge::seed_and_keys_from_mnemonic(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	monero_wallet_utils::WalletDescriptionRetVals retVals;
	bool r = monero_wallet_utils::wallet_with(
		json_root.get<string>("mnemonic_string"),
		retVals,
		nettype_from_string(json_root.get<string>("nettype_string"))
	);
	bool did_error = retVals.did_error;
	if (!r) {
		return error_ret_json_from_message(*retVals.err_string);
	}
	monero_wallet_utils::WalletDescription walletDescription = *(retVals.optl__desc);
	THROW_WALLET_EXCEPTION_IF(did_error, error::wallet_internal_error, "Illegal success flag but did_error");
	//
	boost::property_tree::ptree root;
	root.put(ret_json_key__sec_seed_string(), (*(retVals.optl__desc)).sec_seed_string);
	root.put(ret_json_key__mnemonic_language(), (*(retVals.optl__desc)).mnemonic_language);
	root.put(ret_json_key__address_string(), (*(retVals.optl__desc)).address_string);
	root.put(ret_json_key__pub_viewKey_string(), epee::string_tools::pod_to_hex((*(retVals.optl__desc)).pub_viewKey));
	root.put(ret_json_key__sec_viewKey_string(), epee::string_tools::pod_to_hex((*(retVals.optl__desc)).sec_viewKey));
	root.put(ret_json_key__pub_spendKey_string(), epee::string_tools::pod_to_hex((*(retVals.optl__desc)).pub_spendKey));
	root.put(ret_json_key__sec_spendKey_string(), epee::string_tools::pod_to_hex((*(retVals.optl__desc)).sec_spendKey));
	//
	return ret_json_from_root(root);
}
string serial_bridge::validate_components_for_login(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	monero_wallet_utils::WalletComponentsValidationResults retVals;
	bool r = monero_wallet_utils::validate_wallet_components_with( // returns !did_error
		json_root.get<string>("address_string"),
		json_root.get<string>("sec_viewKey_string"),
		json_root.get<string>("sec_spendKey_string"),
		json_root.get<string>("seed_string"),
		nettype_from_string(json_root.get<string>("nettype_string")),
		retVals
	);
	bool did_error = retVals.did_error;
	if (!r) {
		return error_ret_json_from_message(*retVals.err_string);
	}
	THROW_WALLET_EXCEPTION_IF(did_error, error::wallet_internal_error, "Illegal success flag but did_error");
	//
	boost::property_tree::ptree root;
	root.put(ret_json_key__isValid(), retVals.isValid);
	root.put(ret_json_key__isInViewOnlyMode(), retVals.isInViewOnlyMode);
	root.put(ret_json_key__pub_viewKey_string(), std::move(retVals.pub_viewKey_string));
	root.put(ret_json_key__pub_spendKey_string(), std::move(retVals.pub_spendKey_string));
	//
	return ret_json_from_root(root);
}
//
string serial_bridge::estimate_rct_tx_size(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	uint32_t size = monero_transfer_utils::estimate_rct_tx_size(
		json_root.get<int>("n_inputs"),
		json_root.get<int>("mixin"),
		json_root.get<int>("n_outputs"),
		json_root.get<int>("extra_size"),
		json_root.get<bool>("bulletproof")
	);
	std::ostringstream o;
	o << size;
	//
	boost::property_tree::ptree root;
	root.put(ret_json_key__generic_retVal(), o.str());
	//
	return ret_json_from_root(root);
}
string serial_bridge::calculate_fee(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	uint64_t fee = monero_transfer_utils::calculate_fee(
		stoull(json_root.get<string>("fee_per_kb")),
		stoul(json_root.get<string>("num_bytes")),
		stoull(json_root.get<string>("fee_multiplier"))
	);
	std::ostringstream o;
	o << fee;
	//
	boost::property_tree::ptree root;
	root.put(ret_json_key__generic_retVal(), o.str());
	//
	return ret_json_from_root(root);
}
string serial_bridge::estimated_tx_network_fee(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	uint64_t fee = monero_transfer_utils::estimated_tx_network_fee(
		stoull(json_root.get<string>("fee_per_kb")),
		stoul(json_root.get<string>("priority")),
		[] (uint8_t version, int64_t early_blocks) -> bool
		{
			return lightwallet_hardcoded__use_fork_rules(version, early_blocks);
		}
	);
	std::ostringstream o;
	o << fee;
	//
	boost::property_tree::ptree root;
	root.put(ret_json_key__generic_retVal(), o.str());
	//
	return ret_json_from_root(root);
}
//
string serial_bridge::generate_key_image(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
	}
	crypto::secret_key sec_viewKey{};
	crypto::secret_key sec_spendKey{};
	crypto::public_key pub_spendKey{};
	crypto::public_key tx_pub_key{};
	{
		bool r = false;
		r = epee::string_tools::hex_to_pod(std::string(json_root.get<string>("sec_viewKey_string")), sec_viewKey);
		THROW_WALLET_EXCEPTION_IF(!r, error::wallet_internal_error, "Invalid secret view key");
		r = epee::string_tools::hex_to_pod(std::string(json_root.get<string>("sec_spendKey_string")), sec_spendKey);
		THROW_WALLET_EXCEPTION_IF(!r, error::wallet_internal_error, "Invalid secret spend key");
		r = epee::string_tools::hex_to_pod(std::string(json_root.get<string>("pub_spendKey_string")), pub_spendKey);
		THROW_WALLET_EXCEPTION_IF(!r, error::wallet_internal_error, "Invalid public spend key");
		r = epee::string_tools::hex_to_pod(std::string(json_root.get<string>("tx_pub_key")), tx_pub_key);
		THROW_WALLET_EXCEPTION_IF(!r, error::wallet_internal_error, "Invalid tx pub key");
	}
	monero_key_image_utils::KeyImageRetVals retVals;
	bool r = monero_key_image_utils::new__key_image(
		pub_spendKey, sec_spendKey, sec_viewKey, tx_pub_key,
		stoull(json_root.get<string>("out_index")),
		retVals
	);
	if (!r) {
		return error_ret_json_from_message("Unable to generate key image"); // TODO: return error string? (unwrap optional)
	}
	boost::property_tree::ptree root;
	root.put(ret_json_key__generic_retVal(), epee::string_tools::pod_to_hex(retVals.calculated_key_image));
	//
	return ret_json_from_root(root);
}
//
//
// TODO: probably take transaction secret key as an argument so we don't have to worry about randomness there
string serial_bridge::create_transaction(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// it will already have thrown an exception
		return error_ret_json_from_message("Invalid JSON");
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
	root.put(ret_json_key__create_transaction__tx_key(), std::move(*(retVals.tx_key_string)));
	//
	return ret_json_from_root(root);
}
