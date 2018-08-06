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
	auto result = monero::address_utils::decodedAddress(address, false);
	if (result.err_string) {
		std::cout << *result.err_string << endl;
		BOOST_REQUIRE(!result.err_string);
	}
	BOOST_REQUIRE(result.pub_viewKey_string != none);
	BOOST_REQUIRE(result.pub_spendKey_string != none);
	BOOST_REQUIRE(result.isSubaddress == false);
	BOOST_REQUIRE(result.paymentID_string == none);
	std::cout << "Decoded: " << address << std::endl;
}
//
//
#include "../src/monero_paymentID_utils.hpp"
BOOST_AUTO_TEST_CASE(paymentID)
{
	string paymentID_string = monero_paymentID_utils::new_short_plain_paymentID_string();
	std::cout << "paymentID_string: " << paymentID_string << std::endl;
	BOOST_REQUIRE_MESSAGE(paymentID_string.size() == 16, "Expected payment ID to be of length 16");
	//
	crypto::hash parsed__payment_id;
	bool didParse = monero_paymentID_utils::parse_payment_id(paymentID_string, parsed__payment_id);
	BOOST_REQUIRE_MESSAGE(didParse, "Couldn't parse payment ID");
	std::string parsed__payment_id_as_string = epee::string_tools::pod_to_hex(parsed__payment_id);
	BOOST_REQUIRE_MESSAGE(paymentID_string.compare(parsed__payment_id_as_string), "Expected parsed payment ID to equal original payment ID");
	std::cout << "parsed__payment_id: " << parsed__payment_id << std::endl;
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
	uint64_t est_fee = monero_transfer_utils::estimated_tx_network_fee(fee_per_kb, priority, cryptonote::MAINNET, use_fork_rules_fn);
	std::cout << "est_fee with fee_per_kb " << fee_per_kb << ": " << est_fee << std::endl;
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
	string sec_viewKey_string = "......";
	string sec_spendKey_string = "......";
	
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
	cout << "spend sec key: " << string_tools::pod_to_hex(account_keys.m_spend_secret_key) << endl;
	cout << "spend pub key: " << string_tools::pod_to_hex(from_addr_info.address.m_spend_public_key) << endl;
	//
	optional<string> payment_id_string = string("b79f8efc81f58f67");
	bool pid_encrypt = true;
	uint64_t amount = 10000000000;
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
	std::vector<cryptonote::tx_destination_entry> dsts; // without change this would normally require a dummy addr with 0 amount pushed as a fake 'change' output ... that should probably be moved into the function
	{ // 0. actual destination address
		cryptonote::tx_destination_entry de;
		de.addr = to_addr_info.address;
		de.amount = amount;
		de.is_subaddress = to_addr_info.is_subaddress;
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
		dsts.push_back(de);
	}
	{ // 1. change
		cryptonote::tx_destination_entry de;
		de.addr = from_addr_info.address;
		de.amount = 112832250000;
		de.is_subaddress = from_addr_info.is_subaddress; // not
		dsts.push_back(de);
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
	cout << "account_keys.m_account_address.m_spend_public_key: " << string_tools::pod_to_hex(account_keys.m_account_address.m_spend_public_key) << endl;
	//
	monero_transfer_utils::create_transaction(
		retVals,
		account_keys,
		subaddr_account_idx,
		subaddresses,
		dsts,
		outputs,
		mix_outs,
		fee_amount,
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
	auto txBlob = t_serializable_object_to_blob(retVals.tx);
	size_t txBlob_byteLength = txBlob.size();
//	cout << "txBlob: " << txBlob << endl;
	cout << "txBlob_byteLength: " << txBlob_byteLength << endl;
	BOOST_REQUIRE(txBlob_byteLength > 0);
	
	// tx hash
	auto tx_hash_string = epee::string_tools::pod_to_hex(cryptonote::get_transaction_hash(retVals.tx));
	auto signed_serialized_tx_string = epee::string_tools::buff_to_hex_nodelimer(cryptonote::tx_to_blob(retVals.tx));

	cout << "tx_hash_string: " << tx_hash_string << endl;
	cout << "signed_serialized_tx_string: " << signed_serialized_tx_string << endl;

}
