//
//  test_all.cpp
//  MyMonero
//
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
// Test module setup
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE LibMoneroTests
#include <boost/test/unit_test.hpp> // last
//
// Includes & namespaces
#include <iostream>
#include <iterator>
#include <sstream>
using namespace std;
#include "string_tools.h"
using namespace epee;
using namespace boost;
#include "cryptonote_format_utils.h"
//
// Shared code
//
// Test suites
 #include "../src/monero_address_utils.hpp"
BOOST_AUTO_TEST_CASE(decodeAddress)
{
	string address = "43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg";
	auto result = monero::address_utils::decodedAddress(address, cryptonote::MAINNET);
	if (result.err_string) {
		std::cout << *result.err_string << endl;
		BOOST_REQUIRE(!result.err_string);
	}
	BOOST_REQUIRE(result.pub_viewKey_string != none);
	BOOST_REQUIRE(result.pub_spendKey_string != none);
	BOOST_REQUIRE(result.isSubaddress == false);
	BOOST_REQUIRE(result.paymentID_string == none);
	std::cout << "decodeAddress: " << address << std::endl;
}
//
//
#include "../src/monero_paymentID_utils.hpp"
BOOST_AUTO_TEST_CASE(paymentID)
{
	string paymentID_string = monero_paymentID_utils::new_short_plain_paymentID_string();
	std::cout << "paymentID: paymentID_string: " << paymentID_string << std::endl;
	BOOST_REQUIRE_MESSAGE(paymentID_string.size() == 16, "Expected payment ID to be of length 16");
	//
	crypto::hash parsed__payment_id;
	bool didParse = monero_paymentID_utils::parse_payment_id(paymentID_string, parsed__payment_id);
	BOOST_REQUIRE_MESSAGE(didParse, "Couldn't parse payment ID");
	std::string parsed__payment_id_as_string = epee::string_tools::pod_to_hex(parsed__payment_id);
	BOOST_REQUIRE_MESSAGE(paymentID_string.compare(parsed__payment_id_as_string), "Expected parsed payment ID to equal original payment ID");
	std::cout << "paymentID: parsed__payment_id: " << parsed__payment_id << std::endl;
}
//
//
#include "../src/monero_key_image_utils.hpp"
BOOST_AUTO_TEST_CASE(keyImage)
{
}
//
//
#include "../src/monero_wallet_utils.hpp"
BOOST_AUTO_TEST_CASE(wallet)
{
}
//
//
#include "../src/monero_transfer_utils.hpp"
#include "../src/monero_fork_rules.hpp"
BOOST_AUTO_TEST_CASE(transfers__fee)
{
	monero_fork_rules::use_fork_rules_fn_type use_fork_rules_fn = [] (uint8_t version, int64_t early_blocks) -> bool
	{
		return monero_fork_rules::lightwallet_hardcoded__use_fork_rules(version, early_blocks);
	};
	uint64_t fee_per_kb = 9000000;
	uint32_t priority = 2;
	uint64_t est_fee = monero_transfer_utils::estimated_tx_network_fee(fee_per_kb, priority, use_fork_rules_fn);
	std::cout << "transfers__fee: est_fee with fee_per_kb " << fee_per_kb << ": " << est_fee << std::endl;
	BOOST_REQUIRE(est_fee > 0);
}
BOOST_AUTO_TEST_CASE(transfers__create)
{
	using namespace monero_transfer_utils;
	using namespace monero_fork_rules;

	TransactionConstruction_RetVals retVals;
	monero_fork_rules::use_fork_rules_fn_type use_fork_rules_fn = [] (uint8_t version, int64_t early_blocks) -> bool
	{
		return monero_fork_rules::lightwallet_hardcoded__use_fork_rules(version, early_blocks);
	};

	cryptonote::network_type nettype = cryptonote::MAINNET;

	string from_addressString = "43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg";
	string sec_viewKey_string = "7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104";
	string sec_spendKey_string = "4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803";

	cryptonote::address_parse_info from_addr_info;
	BOOST_REQUIRE(cryptonote::get_account_address_from_str(from_addr_info, nettype, from_addressString));
	cryptonote::account_keys account_keys;
	{
		account_keys.m_account_address = from_addr_info.address;
		//
		crypto::secret_key sec_viewKey;
		BOOST_REQUIRE(string_tools::hex_to_pod(sec_viewKey_string, sec_viewKey));
		account_keys.m_view_secret_key = sec_viewKey;
		//
		crypto::secret_key sec_spendKey;
		BOOST_REQUIRE(string_tools::hex_to_pod(sec_spendKey_string, sec_spendKey));
		account_keys.m_spend_secret_key = sec_spendKey;
	}
	cout << "transfers__create: spend sec key: " << string_tools::pod_to_hex(account_keys.m_spend_secret_key) << endl;
	cout << "transfers__create: spend pub key: " << string_tools::pod_to_hex(from_addr_info.address.m_spend_public_key) << endl;
	//
	optional<string> payment_id_string = string("b79f8efc81f58f67");
	uint64_t sending_amount = 10000000000;
	uint64_t fee_amount = 2167750000;
	string to_address_string("43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg");
	cryptonote::address_parse_info to_addr_info;
	BOOST_REQUIRE(cryptonote::get_account_address_from_str(to_addr_info, nettype, to_address_string));
	//
	std::vector<uint8_t> extra;
	bool payment_id_seen = false;
	{ // Detect hash8 or hash32 char hex string as pid and configure 'extra' accordingly
		bool r = false;
		if (payment_id_string != none) {
			crypto::hash payment_id;
			r = monero_paymentID_utils::parse_long_payment_id(*payment_id_string, payment_id);
			if (r) {
				std::string extra_nonce;
				cryptonote::set_payment_id_to_tx_extra_nonce(extra_nonce, payment_id);
				r = cryptonote::add_extra_nonce_to_tx_extra(extra, extra_nonce);
			} else {
				crypto::hash8 payment_id8;
				r = monero_paymentID_utils::parse_short_payment_id(*payment_id_string, payment_id8);
				if (r) {
					std::string extra_nonce;
					cryptonote::set_encrypted_payment_id_to_tx_extra_nonce(extra_nonce, payment_id8);
					r = cryptonote::add_extra_nonce_to_tx_extra(extra, extra_nonce);
				}
			}
			payment_id_seen = true;
		}
	}
	//
	{ // payment id
		if (to_addr_info.is_subaddress && payment_id_seen) {
			BOOST_REQUIRE_MESSAGE(false, "Illegal: Never supply a pid with a subaddress."); // TODO: is this true?
			return;
		}
		if (to_addr_info.has_payment_id) {
			if (payment_id_seen) {
				BOOST_REQUIRE(false); // can't use int addr at same time as supplying manual pid
				return;
			}
			if (to_addr_info.is_subaddress) {
				BOOST_REQUIRE(false); // should never happen .. logic err?
				return;
			}
			std::string extra_nonce;
			cryptonote::set_encrypted_payment_id_to_tx_extra_nonce(extra_nonce, to_addr_info.payment_id);
			bool r = cryptonote::add_extra_nonce_to_tx_extra(extra, extra_nonce);
			if (!r) {
				BOOST_REQUIRE(false);
				return;
			}
			payment_id_seen = true;
		}
	}
	//
	vector<SpendableOutput> outputs;
	{ // required info from output
		auto out = SpendableOutput{};
		out.amount = 125000000000;
		out.public_key = "596fa47b6b3905269503435099a05e3ede54564026c93cbe5285e2df074c7118";
		out.rct = "920ee8d99299f304d17fdb104720d1f62be0b03383c7bb466ff39c6a264d80d616ce1eccd6c4de1cc0fba87e463f2e0c373146c475e8a1517f36e7a37351d50034688cc8cb528c14188cae45d89b313d444e583c9d68a32cb80938a5e2aa200b";
		out.global_index = 6451664;
		out.index = 0;
		out.tx_pub_key = "0a86e588dc67ca11993737e003a9e60c57174a663a47495e3b1d764f486fc88f";
		outputs.push_back(out);
	}
	//
	vector<RandomAmountOutputs> mix_outs;
	{
		auto amountAndOuts = RandomAmountOutputs{};
		amountAndOuts.amount = 0;
		{
			auto amountOutput = RandomAmountOutput{};
			amountOutput.global_index = 5260585; // this is, I believe, presently supplied as a string by the API, probably to avoid overflow
			amountOutput.public_key = "da77082624fce921891c4fb80a1e7076a6714ca8c9fc547311737926a0b85a46";
			amountOutput.rct = "bb227b27e36b7f3e695dffb641c29bb60bfd991accdb5ef4b580c9acd48c16b6";
			amountAndOuts.outputs.push_back(amountOutput);
		}
		{
			auto amountOutput = RandomAmountOutput{};
			amountOutput.global_index = 1929918;
			amountOutput.public_key = "8c983e7053d7a1dc9de8ac00468bcf11836a787d712dc0c02bd54a3ee00a55e8";
			amountOutput.rct = "8dec45867644d1a76aafe4487292d7cf401302e6bbbb99a61c2f3b6cef4f4f34";
			amountAndOuts.outputs.push_back(amountOutput);
		}
		{
			auto amountOutput = RandomAmountOutput{};
			amountOutput.global_index = 3921094;
			amountOutput.public_key = "0133219bd5e247eef51003921ec792784c41fc34289c703e9326d46f78d9b10a";
			amountOutput.rct = "75082f4ce31904acba4af37699c28d8d4f0f74fdf63b1e4a8069ebed50df3220";
			amountAndOuts.outputs.push_back(amountOutput);
		}
		{
			auto amountOutput = RandomAmountOutput{};
			amountOutput.global_index = 6627106;
			amountOutput.public_key = "daef1663dd1084bd7fe585c3d493480ee1c4cefb93254eac5855afdf38f662b1";
			amountOutput.rct = "1d96763c5bc3300090c286705b7d544f02c185d9be8c32baac6bbfb8e0d0d283";
			amountAndOuts.outputs.push_back(amountOutput);
		}
		{
			auto amountOutput = RandomAmountOutput{};
			amountOutput.global_index = 3308654;
			amountOutput.public_key = "ae135f58762b1133667002538f8c353a1869db815aa686e2544b5243c2d2212f";
			amountOutput.rct = "15046b93bb181189f2917eed38173202fbbb9cdbfcf3d1bc3e432df999ae1b1c";
			amountAndOuts.outputs.push_back(amountOutput);
		}
		{
			auto amountOutput = RandomAmountOutput{};
			amountOutput.global_index = 1972531;
			amountOutput.public_key = "39e44fa88d684d71762c40eb64ac80ddc694b74a99ac445667bf433536c09c8f";
			amountOutput.rct = "66a42d0e8123768b392ad4a230759258d9156fab1aea00a19b041832326aca0a";
			amountAndOuts.outputs.push_back(amountOutput);
		}
		{
			auto amountOutput = RandomAmountOutput{};
			amountOutput.global_index = 3274424;
			amountOutput.public_key = "a89b91648645ba6f32e214ba5720f5387376e5a144e698d5d5d1ebac971de349";
			amountOutput.rct = "815a6b1da6fc6a3bd791c4342782381cf948ee822ac9da7149f1b3717e0266d2";
			amountAndOuts.outputs.push_back(amountOutput);
		}
		mix_outs.push_back(amountAndOuts);
	}
	//
	uint32_t subaddr_account_idx = 0;
	std::unordered_map<crypto::public_key, cryptonote::subaddress_index> subaddresses;
	subaddresses[account_keys.m_account_address.m_spend_public_key] = {0,0};
	cout << "transfers__create: account_keys.m_account_address.m_spend_public_key: " << string_tools::pod_to_hex(account_keys.m_account_address.m_spend_public_key) << endl;
	//
	monero_transfer_utils::create_transaction(
		retVals,
		account_keys,
		subaddr_account_idx,
		subaddresses,
		sending_amount,
		112832250000, // change amount
		fee_amount,
		outputs,
		mix_outs,
		extra,
		use_fork_rules_fn,
		0, // unlock_time
		true, // rct
		nettype
	);
	if (retVals.errCode != noError) {
		BOOST_REQUIRE_MESSAGE(false, "create_transaction failed");
		return;
	}
	auto txBlob = t_serializable_object_to_blob(*retVals.tx);
	size_t txBlob_byteLength = txBlob.size();
//	cout << "txBlob: " << txBlob << endl;
	cout << "transfers__create: txBlob_byteLength: " << txBlob_byteLength << endl;
	BOOST_REQUIRE(txBlob_byteLength > 0);

	// tx hash
	auto tx_hash_string = epee::string_tools::pod_to_hex(cryptonote::get_transaction_hash(*retVals.tx));
	auto signed_serialized_tx_string = epee::string_tools::buff_to_hex_nodelimer(cryptonote::tx_to_blob(*retVals.tx));
	
	cout << "transfers__create: tx_hash_string: " << tx_hash_string << endl;
	cout << "transfers__create: signed_serialized_tx_string: " << signed_serialized_tx_string << endl;
}
//
//
// Serialization bridge
//
#include "../src/serial_bridge_index.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
//
string args_string_from_root(const boost::property_tree::ptree &root)
{
	stringstream args_ss;
	boost::property_tree::write_json(args_ss, root, false/*pretty*/);
	//
	return args_ss.str();
}
BOOST_AUTO_TEST_CASE(bridged__transfers__create)
{
	using namespace serial_bridge;
	using namespace monero_transfer_utils;
	//
	string from_address_string = "43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg";
	string to_address_string = "43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg";
	string amount_string = "10000000000";
	//
	boost::property_tree::ptree root;
	root.put("nettype_string", string_from_nettype(MAINNET));
	root.put("from_address_string", from_address_string);
	root.put("sec_viewKey_string", "7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104");
	root.put("sec_spendKey_string", "4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803");
	root.put("to_address_string", to_address_string);
	root.put("payment_id_string", "b79f8efc81f58f67");
	root.put("sending_amount", amount_string);
	root.put("change_amount", "112832250000");
	root.put("fee_amount", "2167750000");
	//
	boost::property_tree::ptree outputs;
	{
		boost::property_tree::ptree out;
		out.put("amount", "125000000000");
		out.put("public_key", "596fa47b6b3905269503435099a05e3ede54564026c93cbe5285e2df074c7118");
		out.put("rct", "920ee8d99299f304d17fdb104720d1f62be0b03383c7bb466ff39c6a264d80d616ce1eccd6c4de1cc0fba87e463f2e0c373146c475e8a1517f36e7a37351d50034688cc8cb528c14188cae45d89b313d444e583c9d68a32cb80938a5e2aa200b");
		out.put("global_index", "6451664");
		out.put("index", "0");
		out.put("tx_pub_key", "0a86e588dc67ca11993737e003a9e60c57174a663a47495e3b1d764f486fc88f");
		outputs.push_back(std::make_pair("", out));
	}
	root.add_child("outputs", outputs);
	//
	boost::property_tree::ptree mix_outs;
	{
		boost::property_tree::ptree mix_out;
		mix_out.put("amount", "0");
		boost::property_tree::ptree mix_out_outputs;
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5260585");
			mix_out_output.put("public_key", "da77082624fce921891c4fb80a1e7076a6714ca8c9fc547311737926a0b85a46");
			mix_out_output.put("rct", "bb227b27e36b7f3e695dffb641c29bb60bfd991accdb5ef4b580c9acd48c16b6");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "1929918");
			mix_out_output.put("public_key", "8c983e7053d7a1dc9de8ac00468bcf11836a787d712dc0c02bd54a3ee00a55e8");
			mix_out_output.put("rct", "8dec45867644d1a76aafe4487292d7cf401302e6bbbb99a61c2f3b6cef4f4f34");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "3921094");
			mix_out_output.put("public_key", "0133219bd5e247eef51003921ec792784c41fc34289c703e9326d46f78d9b10a");
			mix_out_output.put("rct", "75082f4ce31904acba4af37699c28d8d4f0f74fdf63b1e4a8069ebed50df3220");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6627106");
			mix_out_output.put("public_key", "daef1663dd1084bd7fe585c3d493480ee1c4cefb93254eac5855afdf38f662b1");
			mix_out_output.put("rct", "1d96763c5bc3300090c286705b7d544f02c185d9be8c32baac6bbfb8e0d0d283");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "3308654");
			mix_out_output.put("public_key", "ae135f58762b1133667002538f8c353a1869db815aa686e2544b5243c2d2212f");
			mix_out_output.put("rct", "15046b93bb181189f2917eed38173202fbbb9cdbfcf3d1bc3e432df999ae1b1c");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "1972531");
			mix_out_output.put("public_key", "39e44fa88d684d71762c40eb64ac80ddc694b74a99ac445667bf433536c09c8f");
			mix_out_output.put("rct", "66a42d0e8123768b392ad4a230759258d9156fab1aea00a19b041832326aca0a");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "3274424");
			mix_out_output.put("public_key", "a89b91648645ba6f32e214ba5720f5387376e5a144e698d5d5d1ebac971de349");
			mix_out_output.put("rct", "815a6b1da6fc6a3bd791c4342782381cf948ee822ac9da7149f1b3717e0266d2");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}							  
		mix_out.add_child("outputs", mix_out_outputs);
		mix_outs.push_back(std::make_pair("", mix_out));
	}
	root.add_child("mix_outs", mix_outs);
	//
	auto ret_string = serial_bridge::create_transaction(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<uint32_t> err_code = ret_tree.get_optional<uint32_t>(ret_json_key__any__err_code());
	if (err_code != none && (CreateTransactionErrorCode)*err_code != monero_transfer_utils::noError) {
		auto err_msg = err_msg_from_err_code__create_transaction((CreateTransactionErrorCode)*err_code);
		BOOST_REQUIRE_MESSAGE(false, err_msg);
	}
	optional<string> tx_hash = ret_tree.get_optional<string>(ret_json_key__create_transaction__tx_hash());
	optional<string> tx_key_string = ret_tree.get_optional<string>(ret_json_key__create_transaction__tx_key());
	optional<string> serialized_signed_tx = ret_tree.get_optional<string>(ret_json_key__create_transaction__serialized_signed_tx());
	BOOST_REQUIRE(serialized_signed_tx != none);
	BOOST_REQUIRE((*serialized_signed_tx).size() > 0);
	cout << "bridged__transfers__create: serialized_signed_tx: " << *serialized_signed_tx << endl;
	BOOST_REQUIRE(tx_hash != none);
	BOOST_REQUIRE((*tx_hash).size() > 0);
	cout << "bridged__transfers__create: tx_hash: " << *tx_hash << endl;
	BOOST_REQUIRE(tx_key_string != none);
	BOOST_REQUIRE((*tx_key_string).size() > 0);
	cout << "bridged__transfers__create: tx_key_string: " << *tx_key_string << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__decode_address)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("nettype_string", string_from_nettype(MAINNET));
	root.put("address", "4L6Gcy9TAHqPVPMnqa5cPtJK25tr7maE7LrJe67vzumiCtWwjDBvYnHZr18wFexJpih71Mxsjv8b7EpQftpB9NjPaL41VrjstLM5WevLZx");
	//
	auto ret_string = serial_bridge::decode_address(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> pub_viewKey_string = ret_tree.get_optional<string>(ret_json_key__pub_viewKey_string());
	BOOST_REQUIRE(pub_viewKey_string != none);
	BOOST_REQUIRE((*pub_viewKey_string).size() > 0);
	cout << "bridged__decode_address: pub_viewKey_string: " << *pub_viewKey_string << endl;
	optional<string> pub_spendKey_string = ret_tree.get_optional<string>(ret_json_key__pub_spendKey_string());
	BOOST_REQUIRE(pub_spendKey_string != none);
	BOOST_REQUIRE((*pub_spendKey_string).size() > 0);
	cout << "bridged__decode_address: pub_spendKey_string: " << *pub_spendKey_string << endl;
	optional<string> paymentID_string = ret_tree.get_optional<string>(ret_json_key__paymentID_string());
	BOOST_REQUIRE(paymentID_string != none);
	BOOST_REQUIRE((*paymentID_string).size() > 0);
	cout << "bridged__decode_address: paymentID_string: " << *paymentID_string << endl;
	optional<bool> isSubaddress = ret_tree.get_optional<bool>(ret_json_key__isSubaddress());
	BOOST_REQUIRE(isSubaddress != none);
	BOOST_REQUIRE(*isSubaddress == false);
	cout << "bridged__decode_address: isSubaddress: " << *isSubaddress << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__is_subaddress)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("nettype_string", string_from_nettype(MAINNET));
	root.put("address", "4L6Gcy9TAHqPVPMnqa5cPtJK25tr7maE7LrJe67vzumiCtWwjDBvYnHZr18wFexJpih71Mxsjv8b7EpQftpB9NjPaL41VrjstLM5WevLZx");
	//
	auto ret_string = serial_bridge::is_subaddress(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<bool> value = ret_tree.get_optional<bool>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(value != none);
	BOOST_REQUIRE(*value == false);
	cout << "bridged__is_subaddress: " << *value << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__is_integrated_address)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("nettype_string", string_from_nettype(MAINNET));
	root.put("address", "4L6Gcy9TAHqPVPMnqa5cPtJK25tr7maE7LrJe67vzumiCtWwjDBvYnHZr18wFexJpih71Mxsjv8b7EpQftpB9NjPaL41VrjstLM5WevLZx");
	//
	auto ret_string = serial_bridge::is_integrated_address(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<bool> value = ret_tree.get_optional<bool>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(value != none);
	BOOST_REQUIRE(*value == true);
	cout << "bridged__is_integrated_address: " << *value << endl;
}
//
#include <boost/algorithm/string/predicate.hpp>
BOOST_AUTO_TEST_CASE(bridged__new_integrated_address)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("nettype_string", string_from_nettype(MAINNET));
	root.put("address", "43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg");
	root.put("short_pid", "b79f8efc81f58f67");
	//
	auto ret_string = serial_bridge::new_integrated_address(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> value = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(value != none);
	BOOST_REQUIRE((*value).size() > 0);
	BOOST_REQUIRE(boost::equal(*value, string("4DhdwdSDgoS9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqmCzkMP1DMV6WCiNCdsp")));
	cout << "bridged__new_integrated_address: " << *value << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__new_fake_address_for_rct_tx)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("nettype_string", string_from_nettype(MAINNET));
	//
	auto ret_string = serial_bridge::new_fake_address_for_rct_tx(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> value = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(value != none);
	BOOST_REQUIRE((*value).size() > 0);
	cout << "bridged__new_fake_address_for_rct_tx: " << *value << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__new_short_payment_id)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	//
	auto ret_string = serial_bridge::new_payment_id(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> value = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(value != none);
	BOOST_REQUIRE((*value).size() > 0);
	cout << "bridged__new_short_payment_id: " << *value << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__new_wallet)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("nettype_string", string_from_nettype(MAINNET));
	root.put("locale_language_code", "en-US");
	//
	auto ret_string = serial_bridge::newly_created_wallet(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> mnemonic_string = ret_tree.get_optional<string>(ret_json_key__mnemonic_string());
	BOOST_REQUIRE(mnemonic_string != none);
	BOOST_REQUIRE((*mnemonic_string).size() > 0);
	cout << "bridged__new_wallet: mnemonic: " << *mnemonic_string << endl;
	optional<string> mnemonic_language = ret_tree.get_optional<string>(ret_json_key__mnemonic_language());
	BOOST_REQUIRE(mnemonic_language != none);
	BOOST_REQUIRE((*mnemonic_language).size() > 0);
	cout << "bridged__new_wallet: mnemonic_language: " << *mnemonic_language << endl;
	optional<string> sec_seed_string = ret_tree.get_optional<string>(ret_json_key__sec_seed_string());
	BOOST_REQUIRE(sec_seed_string != none);
	BOOST_REQUIRE((*sec_seed_string).size() > 0);
	cout << "bridged__new_wallet: sec_seed: " << *sec_seed_string << endl;
	optional<string> address_string = ret_tree.get_optional<string>(ret_json_key__address_string());
	BOOST_REQUIRE(address_string != none);
	BOOST_REQUIRE((*address_string).size() > 0);
	cout << "bridged__new_wallet: address: " << *address_string << endl;
	optional<string> pub_viewKey_string = ret_tree.get_optional<string>(ret_json_key__pub_viewKey_string());
	BOOST_REQUIRE(pub_viewKey_string != none);
	BOOST_REQUIRE((*pub_viewKey_string).size() > 0);
	cout << "bridged__new_wallet: pub_viewKey_string: " << *pub_viewKey_string << endl;
	optional<string> pub_spendKey_string = ret_tree.get_optional<string>(ret_json_key__pub_spendKey_string());
	BOOST_REQUIRE(pub_spendKey_string != none);
	BOOST_REQUIRE((*pub_spendKey_string).size() > 0);
	cout << "bridged__new_wallet: pub_spendKey_string: " << *pub_spendKey_string << endl;
	optional<string> sec_viewKey_string = ret_tree.get_optional<string>(ret_json_key__sec_viewKey_string());
	BOOST_REQUIRE(sec_viewKey_string != none);
	BOOST_REQUIRE((*sec_viewKey_string).size() > 0);
	cout << "bridged__new_wallet: sec_viewKey_string: " << *sec_viewKey_string << endl;
	optional<string> sec_spendKey_string = ret_tree.get_optional<string>(ret_json_key__sec_spendKey_string());
	BOOST_REQUIRE(sec_spendKey_string != none);
	BOOST_REQUIRE((*sec_spendKey_string).size() > 0);
	cout << "bridged__new_wallet: sec_spendKey_string: " << *sec_spendKey_string << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__are_equal_mnemonics)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("a", "foxe selfish hum nexus juven dodeg pepp ember biscuti elap jazz vibrate biscui");
	root.put("b", "fox sel hum nex juv dod pep emb bis ela jaz vib bis");
	//
	auto ret_string = serial_bridge::are_equal_mnemonics(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<bool> value = ret_tree.get_optional<bool>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(value != none);
	BOOST_REQUIRE(*value != false);
	cout << "bridged__are_equal_mnemonics: " << *value << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__mnemonic_from_seed)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("seed_string", "9c973aa296b79bbf452781dd3d32ad7f");
	root.put("wordset_name", "English");
	//
	auto ret_string = serial_bridge::mnemonic_from_seed(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> mnemonic_string = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(mnemonic_string != none);
	BOOST_REQUIRE((*mnemonic_string).size() > 0);
	cout << "bridged__mnemonic_from_seed: mnemonic: " << *mnemonic_string << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__seed_and_keys_from_mnemonic)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("mnemonic_string", "foxe selfish hum nexus juven dodeg pepp ember biscuti elap jazz vibrate biscui");
	root.put("nettype_string", string_from_nettype(MAINNET));
	//
	auto ret_string = serial_bridge::seed_and_keys_from_mnemonic(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> mnemonic_language = ret_tree.get_optional<string>(ret_json_key__mnemonic_language());
	BOOST_REQUIRE(mnemonic_language != none);
	BOOST_REQUIRE((*mnemonic_language).size() > 0);
	cout << "bridged__seed_and_keys_from_mnemonic: mnemonic_language: " << *mnemonic_language << endl;
	optional<string> sec_seed_string = ret_tree.get_optional<string>(ret_json_key__sec_seed_string());
	BOOST_REQUIRE(sec_seed_string != none);
	BOOST_REQUIRE((*sec_seed_string).size() > 0);
	cout << "bridged__seed_and_keys_from_mnemonic: sec_seed: " << *sec_seed_string << endl;
	optional<string> address_string = ret_tree.get_optional<string>(ret_json_key__address_string());
	BOOST_REQUIRE(address_string != none);
	BOOST_REQUIRE((*address_string).size() > 0);
	cout << "bridged__seed_and_keys_from_mnemonic: address: " << *address_string << endl;
	optional<string> pub_viewKey_string = ret_tree.get_optional<string>(ret_json_key__pub_viewKey_string());
	BOOST_REQUIRE(pub_viewKey_string != none);
	BOOST_REQUIRE((*pub_viewKey_string).size() > 0);
	cout << "bridged__seed_and_keys_from_mnemonic: pub_viewKey_string: " << *pub_viewKey_string << endl;
	optional<string> pub_spendKey_string = ret_tree.get_optional<string>(ret_json_key__pub_spendKey_string());
	BOOST_REQUIRE(pub_spendKey_string != none);
	BOOST_REQUIRE((*pub_spendKey_string).size() > 0);
	cout << "bridged__seed_and_keys_from_mnemonic: pub_spendKey_string: " << *pub_spendKey_string << endl;
	optional<string> sec_viewKey_string = ret_tree.get_optional<string>(ret_json_key__sec_viewKey_string());
	BOOST_REQUIRE(sec_viewKey_string != none);
	BOOST_REQUIRE((*sec_viewKey_string).size() > 0);
	cout << "bridged__seed_and_keys_from_mnemonic: sec_viewKey_string: " << *sec_viewKey_string << endl;
	optional<string> sec_spendKey_string = ret_tree.get_optional<string>(ret_json_key__sec_spendKey_string());
	BOOST_REQUIRE(sec_spendKey_string != none);
	BOOST_REQUIRE((*sec_spendKey_string).size() > 0);
	cout << "bridged__seed_and_keys_from_mnemonic: sec_spendKey_string: " << *sec_spendKey_string << endl;
}

BOOST_AUTO_TEST_CASE(bridged__validate_components_for_login)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("address_string", "43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg");
	root.put("sec_viewKey_string", "7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104");
	root.put("sec_spendKey_string", "4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803");
	root.put("seed_string", "9c973aa296b79bbf452781dd3d32ad7f");
	root.put("nettype_string", string_from_nettype(MAINNET));
	//
	auto ret_string = serial_bridge::validate_components_for_login(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<bool> isValid = ret_tree.get_optional<bool>(ret_json_key__isValid());
	BOOST_REQUIRE(isValid == true);
	cout << "bridged__validate_components_for_login: isValid: " << isValid << endl;
	optional<bool> isInViewOnlyMode = ret_tree.get_optional<bool>(ret_json_key__isInViewOnlyMode());
	BOOST_REQUIRE(isInViewOnlyMode == false);
	cout << "bridged__validate_components_for_login: isInViewOnlyMode: " << isInViewOnlyMode << endl;
	optional<string> pub_viewKey_string = ret_tree.get_optional<string>(ret_json_key__pub_viewKey_string());
	BOOST_REQUIRE(pub_viewKey_string != none);
	BOOST_REQUIRE((*pub_viewKey_string).size() > 0);
	cout << "bridged__validate_components_for_login: pub_viewKey_string: " << *pub_viewKey_string << endl;
	optional<string> pub_spendKey_string = ret_tree.get_optional<string>(ret_json_key__pub_spendKey_string());
	BOOST_REQUIRE(pub_spendKey_string != none);
	BOOST_REQUIRE((*pub_spendKey_string).size() > 0);
	cout << "bridged__validate_components_for_login: pub_spendKey_string: " << *pub_spendKey_string << endl;
}

BOOST_AUTO_TEST_CASE(bridged__estimated_tx_network_fee)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("fee_per_kb", "9000000");
	root.put("priority", "2");
	//
	auto ret_string = serial_bridge::estimated_tx_network_fee(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> fee_string = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(fee_string != none);
	BOOST_REQUIRE((*fee_string).size() > 0);
	uint64_t fee = stoull(*fee_string);
	BOOST_REQUIRE(fee == 504000000);
	cout << "bridged__estimated_tx_network_fee: " << fee << endl;
}

BOOST_AUTO_TEST_CASE(bridged__estimate_rct_tx_size)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("n_inputs", 2);
	root.put("mixin", monero_transfer_utils::fixed_mixinsize());
	root.put("n_outputs", 2);
	std::vector<uint8_t> extra; // blank extra
	root.put("extra_size", extra.size());
	root.put("bulletproof", monero_fork_rules::lightwallet_hardeded__use_bulletproofs());
	//
	auto ret_string = serial_bridge::estimate_rct_tx_size(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> size_string = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(size_string != none);
	BOOST_REQUIRE((*size_string).size() > 0);
	uint64_t size = stoull(*size_string);
	BOOST_REQUIRE(size == 13762);
	cout << "bridged__estimate_rct_tx_size: " << size << endl;
}

BOOST_AUTO_TEST_CASE(bridged__calculate_fee)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("fee_per_kb", "9000000");
	root.put("num_bytes", "13762");
	root.put("fee_multiplier", "4"); // aka priority idx 1 / number 2
	//
	auto ret_string = serial_bridge::calculate_fee(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> fee_string = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(fee_string != none);
	BOOST_REQUIRE((*fee_string).size() > 0);
	uint64_t fee = stoull(*fee_string);
	BOOST_REQUIRE(fee == 504000000);
	cout << "bridged__calculate_fee: " << fee << endl;
}

BOOST_AUTO_TEST_CASE(bridged__generate_key_image)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("sec_viewKey_string", "7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104");
	root.put("sec_spendKey_string", "4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803");
	root.put("pub_spendKey_string", "3eb884d3440d71326e27cc07a861b873e72abd339feb654660c36a008a0028b3");
	root.put("tx_pub_key", "fc7f85bf64c6e4f6aa612dbc8ddb1bb77a9283656e9c2b9e777c9519798622b2");
	root.put("out_index", "0");
	//
	auto ret_string = serial_bridge::generate_key_image(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> key_image_string = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(key_image_string != none);
	BOOST_REQUIRE((*key_image_string).size() > 0);
	BOOST_REQUIRE(*key_image_string == "ae30ee23051dc0bdf10303fbd3b7d8035a958079eb66516b1740f2c9b02c804e");
	cout << "bridged__generate_key_image: " << *key_image_string << endl;
}
