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
#include <boost/property_tree/json_parser.hpp>
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
	uint64_t fee_per_b = 24658;
	uint32_t priority = 2;
	uint64_t est_fee = monero_fee_utils::estimated_tx_network_fee(fee_per_b, priority, use_fork_rules_fn);
	std::cout << "transfers__fee: est_fee with fee_per_b " << fee_per_b << ": " << est_fee << std::endl;
	BOOST_REQUIRE(est_fee > 0);
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
//
string DG_presweep__unspent_outs_json = "{\"unspent_outs\":[{\"amount\":\"210000000\",\"public_key\":\"89eb08cf704d4473a17646331d2c425307ef03477e5f18ee6a31a3601ba9cdd0\",\"index\":0,\"global_index\":7510705,\"rct\":\"befe623ad1dcae239e4d9d31e3080db5c339ea8c5c2894444966967a051f27839f1f713d6f6bdc13fec3c20f78bbae6cf08ce185273fa6c913db6ae1f44e270ea9dcfa48ecbae364125e0c4b0cb7a11fe6c250ec9aca1a668a0708e821d6550b\",\"tx_id\":5292354,\"tx_hash\":\"22fa4aaee9399901ece7d9521067aa7791a727ade2dfe9d5e17481800ccbc625\",\"tx_pub_key\":\"4f151192723d3d45372b43e4bf93df8ad7ba5283513c09226fd0603c60683e00\",\"tx_prefix_hash\":\"689580f0804eff0fd9bd76587ed9656e4cda8e70a33f065b5461206bcf9051b7\",\"height\":1681636},{\"amount\":\"230000000\",\"public_key\":\"f659694299d97fc93db504122d40dea1681a896567933635dc6337abc4339c10\",\"index\":1,\"global_index\":7551823,\"rct\":\"dd06d546553044cda0f083fd189cd8ad93ebeca557169eefe1e34dc48c6fac27110a3ff8dc24a61b595a03a034009a6d1f0ced61f19fb6e0d7c2b1a67bb39d06c7d5713e0a394551ec978b64927802f9307ac29c8ddec3857f551b945ef6a407\",\"tx_id\":5309604,\"tx_hash\":\"05704e7402d1373d14dccd383e4071bfae0c2af6eb075e67075b43fd7d26b4c4\",\"tx_pub_key\":\"3511d9117fdeac0423314827188aa187f1eb742a44ab0c01390053b68b00909c\",\"tx_prefix_hash\":\"1b89ac0c818454806686073cd2d6bd501923d6eec2c0e54e300e3ae68a2c5344\",\"height\":1684479},{\"amount\":\"50000000\",\"public_key\":\"6c0828f041fd3383b4823bd619fa9d130b83da8b10aa81bec1f1529890548542\",\"index\":0,\"global_index\":7552032,\"rct\":\"eb3291bd81992ec300e94e8f1bbf0bdfd7bb8b8ac5e1969f985f5642961e30f1bfaf72e1a284ac62da47184165091e2b8673143f8b5d533b9d2143c9e64e5f091a14169fa79f151579eac8e41102244aa148b6142121f5ad1b85d6404c04fd08\",\"tx_id\":5309676,\"tx_hash\":\"22c79cc2e5cefe5b1ed608021efc281f0eab6dbe4cc11051d59cb30b367a5120\",\"tx_pub_key\":\"471fe593e98e65529eeb2b60ad23ad0f7f879a51ef0a41ed34bb371bf346663f\",\"tx_prefix_hash\":\"6551844de3ba9ca5c0a58a8dfcb3fa6b74439c0f1054563ba7cd9a5eee26425d\",\"height\":1684494},{\"amount\":\"100000000\",\"public_key\":\"2721b9cf6cb9e4227cd4c58ea73be9c6e6ebf949d07e90f3cbfc624a18c91933\",\"index\":0,\"global_index\":7571542,\"rct\":\"14972c0692f37de21d4fe2e989262aaef574699258812b39908c6f9f28bfe80f07cae2373c7419eb41bde8425107cd3d87dd0211b1a59e92bad0b09789109d06620a1e4f80dbfa4ada1d432e55b3c23e965e858bc04effb64c473de0da3e4f0a\",\"tx_id\":5318071,\"tx_hash\":\"33fb202b7e1b2382838f17e5078311944870197536df0a560d73605023befcf7\",\"tx_pub_key\":\"3cb2a8fc531565dcc20477ec624fc1ffd60af9f491ab4ec5d54d4e4d6441d0c0\",\"tx_prefix_hash\":\"ce7bec7db0171da5e345103b7d38cdc19fc1db0655e4de8ecbf9b52d11b0ff51\",\"height\":1685790},{\"amount\":\"1630320000\",\"public_key\":\"437e101b61a526098adc98e46c8a674d2b3af3b79cf6ce8947e4effa2b545069\",\"index\":0,\"global_index\":7571614,\"rct\":\"026064f4989c3b19d99d4e3902a6800cdda39127449c639d0d9fcc31d9e4b297211cf6275f64d613cb7b553be07e1d738f72b138c4eaccb91970a10c513f2a005471ce338c4f3ea6bc07a37d10c32236af28c0e85d39ebdf8aa37ce2b9e5060f\",\"tx_id\":5318107,\"tx_hash\":\"19a98217e48c4db9c527dde5c7498317e2a6f258e31c87c3fe8d872d695b44f4\",\"tx_pub_key\":\"95c66b323352e10e7eee3220cc328f89b1b9dd5bfd2da4f61ba5dfcef923722c\",\"tx_prefix_hash\":\"0e053ddb7e60a64a8b501525963a61d8c182f82a66933486e77233055dbce3ac\",\"height\":1685794},{\"amount\":\"100000000\",\"public_key\":\"6a5982bf097116b68fe817abbd31b95821a0d851ac8e4ca34517b1cbe4449501\",\"index\":1,\"global_index\":7571710,\"rct\":\"6d8cd50a778f658b5e2ec7e7e1b8e19e80bb2ece759ed5e40398c33e5983722e45078078a6195d92c3f83d0f443d178a31156702c42f5c4b3e897e9931055f07173349088339daaa080c1f747a9025db0baa60b12beded6c7bc8ce893bda1700\",\"tx_id\":5318152,\"tx_hash\":\"7010c701459d2b897abbfed8f1dbc60b84c4fcd111afd7fcaf60eb1167c0a7a6\",\"tx_pub_key\":\"9321878b374c7f40021635bb03ed966217e5b8e7eb3e125839c55ae48d04ef81\",\"tx_prefix_hash\":\"edb575b5fd86b3e45214742454a4cfcc3d55f1ad457cc598c290dd69d9ebfa73\",\"height\":1685804}]}";
//
string DG_presweep__rand_outs_json = "{\"mix_outs\":[{\"amount\":\"0\",\"outputs\":[{\"global_index\":\"6986524\",\"public_key\":\"3ce9f1231ecebf100a8d0e9c165a2b88a766249cb03eac2c6dbe7587a1f0e9ae\",\"rct\":\"c3b81a937c12c017b4c4eee0ab9acbd10d83f28c1586971b13791c7b475e469b\"},{\"global_index\":\"7282304\",\"public_key\":\"278450b855e4d66dbc1a9ae2801a2f101a10afd22c27466c3cfcc3b434a25047\",\"rct\":\"dd05d1d973be19b4e754c24c6d21e9252a9b99db52ff291930d4cd8c1cd344df\"},{\"global_index\":\"7386837\",\"public_key\":\"0d3cf94dd4e9059900f14bd8d5b71ce43e444efd2b8a1a63a1f9705851d195a1\",\"rct\":\"5c124e0c007e8a2f6371a6d35d50165178667fa9470270e8d7a95ffda34df30d\"},{\"global_index\":\"7459325\",\"public_key\":\"badabeeb71f08917b0cb76ae128e869dab7291d58c7a6b2fbd31d3eed0f003df\",\"rct\":\"a5ca005346fad19624c185dfefb2c4013f6b769f0f0de4b2c8f507ede1cb46a5\"},{\"global_index\":\"7507948\",\"public_key\":\"6f08278bc9d064cfdaa6d896ef70d28fbb3dca84e0a99ea21325f9aaef3bd783\",\"rct\":\"4a70f95a4cc19d9e43cc6b60f30f60571029240df21fb06188766bf92e8d8738\"},{\"global_index\":\"7529692\",\"public_key\":\"8b13f88507f5ca60c72c076ce6bc8ee142abc6e5115ab0c08e10a919c93f912a\",\"rct\":\"6055a2a847938471bd6f00a4d9789e6dc9d70962bb1dc2f51879d04211aaa0b7\"},{\"global_index\":\"7563051\",\"public_key\":\"d44a722cdca3c372081af6e32b758a2bbab9f2534f68a08b71d38c3540209c50\",\"rct\":\"b5ebd41d0c75877cdf109d6b5939072c22a84aee4c46a8299bec8eafc82789e9\"},{\"global_index\":\"7564143\",\"public_key\":\"c12f9e3c53dee0d1327dbca66129b27f8c6174a777976615ee442278960ba369\",\"rct\":\"a8423b9491162813589d3af5e18677f2f38050c10cb5074c097f101ccef089c5\"},{\"global_index\":\"7567982\",\"public_key\":\"9e4347089b0e1cb065cb443899d77b4bd4d61598e80a8946336440920c8a6731\",\"rct\":\"00fc0e9c631a4a2538785b647e6146ba39743d9dc987059f850d1c5a4f97bd2b\"},{\"global_index\":\"7570259\",\"public_key\":\"1be949046425c646a86ac37961a6301ea3d25711426d80a48b11e9282acd222b\",\"rct\":\"7db9d60ac0286189a1833f39db7f3e5372763c557fe2240b4537bf580a902798\"},{\"global_index\":\"7570451\",\"public_key\":\"82a27a521340220805de27aae18a4663b81067145c0b0c3e7ec42341067bf270\",\"rct\":\"a3f46fdc3e4a252604e3f3d082ab1d2cbc3ce34bf62b641b76849c5382199a32\"}]},{\"amount\":\"0\",\"outputs\":[{\"global_index\":\"7442603\",\"public_key\":\"ba89de37e26056629c89b14b3b05a73400c62149fa0de2794d3876f17faeb28f\",\"rct\":\"aa2edfca6622db354add0813ff2b471f6dc20f0d9e56d1f9b6c04b1369ceb1a9\"},{\"global_index\":\"7445670\",\"public_key\":\"a0c3a8bd0d6fa37e7bd514a10ebe6970609919e2f781dc489b771f305f1da4cc\",\"rct\":\"eb78b914307a54cd95481ba8844df3dd2d12cd14cee07de441c2c607b9cfcb24\"},{\"global_index\":\"7474646\",\"public_key\":\"3d325a1222b77d82192e1c051b241e0f79e1cc731c5f03749df33cf1a7165be8\",\"rct\":\"821bfcb255fc815aeab23d890ba252dc590c743c5733bcd278dbd1763e921e4d\"},{\"global_index\":\"7545722\",\"public_key\":\"ec62838ef1ab75055940fd8f31126698af9ff2128a53def09bdaa0d315174d80\",\"rct\":\"547de3a10658167afee6aaf8f3481921d2b1ee3014d40fa4cacc86940b244985\"},{\"global_index\":\"7556262\",\"public_key\":\"4dab027c001473b775f70503b9d68c156d2a8bfa0d7534aaff12a2ab1d8d5f89\",\"rct\":\"5aa838a2f5450408932b53181899861600d3cac864dee8197ac7e9543fbab148\"},{\"global_index\":\"7557709\",\"public_key\":\"bd1813a780e4df3c8ba25b825c3d7be12ce8c5d05f6731384e0d2d8cb8bf3134\",\"rct\":\"49ce757933cdca4a51f77ae41b951a2175d0a0a0378c10c3a02432e5aeb9f79f\"},{\"global_index\":\"7560040\",\"public_key\":\"ea53143df34ccba3c29743964ddc14094f224fa92d45c8fa8e86d7ff1394e51a\",\"rct\":\"455a6083ab6c3d4f026d2b4e1545467666f7affa0cdec365a295c097eefeac46\"},{\"global_index\":\"7563671\",\"public_key\":\"9af80a727bdb148851e79a9a11f55e97435daf65b3d57b54f4d64833cd483f2b\",\"rct\":\"622855010cd03a04d66d71a20d6113cb0507276b4c6ef050297a12e0a6767004\"},{\"global_index\":\"7564234\",\"public_key\":\"404aedc1c299e9a1538bdf7619f42cbf92cb3bb556e0356dce275945e318633d\",\"rct\":\"a1978e496622c2fac054939227a4edb31c4a50215cf8db74b0f1a7ce3477e3cf\"},{\"global_index\":\"7565705\",\"public_key\":\"070c5adc791d0a33390fecb02376e8953e46661a0173a64c003b5ae5709eea3c\",\"rct\":\"09f6c3c9139eefa0ed9ff9613e57bf3fc1b7d2bc42bad4caeb9118cc768cc52f\"},{\"global_index\":\"7566892\",\"public_key\":\"76c03aad2fae21aa7d36bbda699c462b222a76359d92813c06e4ccf4508e77e2\",\"rct\":\"9905946004a01e2884aedfa41b2482ca309226166519c558b5c794eeae109f98\"}]},{\"amount\":\"0\",\"outputs\":[{\"global_index\":\"6898616\",\"public_key\":\"7078ee4031355ca5a60edf9e0240d8c5d42ed180d736628b9386c4cc1dc1cee9\",\"rct\":\"d9a90792ab31fe09094daf43bf1c5f897fac77cd45d72accb5735bb4fe2d98f3\"},{\"global_index\":\"7125047\",\"public_key\":\"556a3792556978326baf4e1cd792c9f8b36f2090afddfd929bf0f87d5b119828\",\"rct\":\"8a9653cbe1b879e562849ff5187af02f49e67b351fda5d39319cffd2a33f56f2\"},{\"global_index\":\"7341447\",\"public_key\":\"f3312f6841d03044e768305ec6166c82917676335a4a751734f2708dfa1dcd97\",\"rct\":\"d20cee9952491177cd1527958c3d57cdc2145f76562f4ab87243349c52318427\"},{\"global_index\":\"7388641\",\"public_key\":\"c3477733871b3a696a81470471ec3996acc27a7ff4c03a899fbec02533c27511\",\"rct\":\"5236a3e790c98624d398e6fbe45b6035da0e4202ac01731722608821a3400d48\"},{\"global_index\":\"7417267\",\"public_key\":\"68f0fa9eefd3fcb6be31ddc49091057b4cf262f4e7500ac85eef93ac32804f5e\",\"rct\":\"e7045d968d7dcbbbdb4f1c3e75703b5f8a151e2381bc44f5f8dc754ea41d0e21\"},{\"global_index\":\"7519867\",\"public_key\":\"d7c4b74dbfb8d00d1e7476124a85e8f756c4a0fa4448aa491acbae3a79500d66\",\"rct\":\"2bbdf12767b7be6d4f15c629cc8c6ce24b63880c15ad7c55c3232ed1d63712c8\"},{\"global_index\":\"7559496\",\"public_key\":\"f06a43153a2c77b55e9af88d21b8c582550d0c03e0ee84492f01aadd1b4c5a7a\",\"rct\":\"f34595267477768ca348689279c4cf2cf4b3f51293015255379aac5b8460afb0\"},{\"global_index\":\"7570229\",\"public_key\":\"a08bf753c3f12ae97702bb30e192d63e62f07558cd7632def98921a2275414a7\",\"rct\":\"a9df67974d97999626c02c72dc1adc90e9bfaaf16742ecccb7e230110a4a3aea\"},{\"global_index\":\"7570862\",\"public_key\":\"174f1d11a4f5c6a08b04dce4c03850faecaae619b3f93d7004754f0a00a49839\",\"rct\":\"9820922f314045452a6022e8cd1c7f5557fd8784194808af1ed0e5f4ba0e7588\"},{\"global_index\":\"7571034\",\"public_key\":\"8859cfe384706c05806818dd77684a7a07c6bff3516c68906871ffeb83a20d60\",\"rct\":\"ad21afb2b1615bead1204096cb412799f0bf89f46411e917f3ba762897d53ff3\"},{\"global_index\":\"7571342\",\"public_key\":\"9292b038cdffeddc25e71356eb5850528e7e8e2b3c877535ffc8eaab4282c225\",\"rct\":\"009da46a328a043b87b774f518018aead88b4c46fd0b412880cdf07b25f52bd3\"}]},{\"amount\":\"0\",\"outputs\":[{\"global_index\":\"6378847\",\"public_key\":\"acc97fed526da1168e8a5aa0bb04c6c2e79f84f2f2cc22c00397d016306226d5\",\"rct\":\"5bff394cf377ca3cfba831c9a3cc70b2cb3e95dfb26773136a61ec720c2d4fda\"},{\"global_index\":\"7450246\",\"public_key\":\"bd8b3c4f38adecb30ea18dbab7512cddb89246ea60319f2eee80b2554455bbd4\",\"rct\":\"add46baca4f3b914b0d2e3ce9f4c568c99331e47ec9f4fcbc8a8ca4ef9f49605\"},{\"global_index\":\"7473242\",\"public_key\":\"b88d96d641378412168bcf80b1c643a79d2e0f503b6cac39ed693042754f6ee3\",\"rct\":\"4ecb6a92321d83a953fceec22516a1ca5d50900dd6cccc4e17abdc70b3328514\"},{\"global_index\":\"7525156\",\"public_key\":\"d3bf363e284257dd2cb24216d1d58ecf5dafedff893d8ada5d28f4c2ad9ef494\",\"rct\":\"d945fd6c04810b99323b3563749fbcd4b1efeda08796394ad05cbcc30c54508a\"},{\"global_index\":\"7531713\",\"public_key\":\"46ea1435fcbae0db704087462eed3a47cff93f738ee51881fc779d248b65fefa\",\"rct\":\"604f96bccca4f79ae7ccce8a5317baa5cd65685dc26dde888cc9049c845bb3d9\"},{\"global_index\":\"7542701\",\"public_key\":\"2319ab37a81b9b012bed4b952ce4c7b85173ea7fa9ed104dcb767580f0af2d1b\",\"rct\":\"3057ece8d0c4a536c4ffa7ff5812b9ba0c951af2b9aa7c788d4d8668480e7f5e\"},{\"global_index\":\"7553340\",\"public_key\":\"e79aa59e773211c29d696aaf5173b4d58e21a6bd8da693d8bf83c6cdf83cfdbc\",\"rct\":\"7d3afea7adf3b3fd946a9b0f4a60616179a5f3cd25027e9a88e9753219ad6a4a\"},{\"global_index\":\"7556989\",\"public_key\":\"33230357a71d8ebeb619a3b613d1d6519090c6b0a208f7c7679f2aac7258ce03\",\"rct\":\"61e1ec7ed3d400d484564687bb18975e73693c47cf9db8bcb0bf3de53c82cfb3\"},{\"global_index\":\"7564935\",\"public_key\":\"8104c3baa28dd4b48554392d7f8bc999e4a0bf3b04a0c11237b17ed0814122ce\",\"rct\":\"5d54aad47d92ada5212ab8f2968799d7235ac27c4497f17143e5cb2b562d210e\"},{\"global_index\":\"7568940\",\"public_key\":\"e750b10591f8ee3f337a55b6490423d31e9b45899d228fed818811c15176667e\",\"rct\":\"37a8725d2d5af7c4dfbbde8cd54f1b1cbfcbe9f8733d2dbb9a1260c1fac437b7\"},{\"global_index\":\"7569092\",\"public_key\":\"74a6306059946b700bb2fde08a12921c799bfb18bbf775246c1dec26c3d5e14b\",\"rct\":\"a4d1566adf3d52a55a518c1533b9ab7ca6bf74e27121cc79c4f15b3b1dc60cdd\"}]},{\"amount\":\"0\",\"outputs\":[{\"global_index\":\"4720715\",\"public_key\":\"8b397b7ee91577dd3f30b86245ec351f8906ac0eadf947f17ee0c6e4b5233a39\",\"rct\":\"2f0ed876b848ecb80647d991082afed2d6f48af3b329757a6cdd84aa323713f4\"},{\"global_index\":\"6747843\",\"public_key\":\"dde0018c7f8c8436907cb62d2b1ed92c14eceafe72cdce3cfacf539b795c2d22\",\"rct\":\"c7867e3f79c322fd2b6d290f76798a8e5626e52df5823d82c5dcdabace44982a\"},{\"global_index\":\"6798372\",\"public_key\":\"955ea5eac6ae37465657c8d3893984fe363f04423889a399ab55261395e1cc05\",\"rct\":\"146f973c2e6644cb39129e2f0127391a1f560fc5235aa8b9afb790a02fa75bac\"},{\"global_index\":\"7141175\",\"public_key\":\"d4db04b8b1dd4b723265b5dcb13ecb0cd4eec8c29580bae631298456717f233a\",\"rct\":\"75908e57c797b43d8c15ec507d03b5785c0e16ef637b4be259638e1e8e7f05e4\"},{\"global_index\":\"7211806\",\"public_key\":\"a68526d2a7d29140473e2d6af52d6487f9c1093caa638d0b6abe21a31ceda161\",\"rct\":\"2beb25022d6b9907ba0678197e0eec0b2edf7bfc6881a66106d43928678ea716\"},{\"global_index\":\"7553092\",\"public_key\":\"d2bf03a9c05de99e250822467e26672b2da3c86feb922ebfcaa4df8c8e2d6e5b\",\"rct\":\"7e06be8140eeb284d687a6d371690877208af23099c2ecd4301bfdd160068c32\"},{\"global_index\":\"7560413\",\"public_key\":\"65ea9940f971560c0cfb0099807c01d63aea0e9cdad98077b4b5abe3e15ffff7\",\"rct\":\"c889ad6c6f2ea64ad43396043ae0421f40567c4778ae89577dd510e992c6d7b5\"},{\"global_index\":\"7562968\",\"public_key\":\"74f327f9a8f2ace334605247c37d52f195f21cb319293e10944855ed8395e18f\",\"rct\":\"c9320c745df7a9dce32ac0ead1d673e4b3faf6034e7efd19bc977f493c6bff4e\"},{\"global_index\":\"7566433\",\"public_key\":\"9620705dead2c2d3665c0787dffb0267362e14d5e509489c9e95f3cfdc3c8457\",\"rct\":\"9a183845a8690f07fa42bb4c442656936ef80e5cb8a859212bd54ec0128934dd\"},{\"global_index\":\"7568871\",\"public_key\":\"fe921b107a193fe4aab99d81df26003eabd62d8e2547eb3d1dd8ebe4c59da7bf\",\"rct\":\"a8b0c65e2c3b4db5e115420fa7eeb897e68e00753e0286491a37f8827f3dd568\"},{\"global_index\":\"7569156\",\"public_key\":\"d2ff286caa1f62bf30c261075d954f517530aed5fa3c74649df19a139dc179f9\",\"rct\":\"40ae688dcf89f2977b2b02c962ead32c205fd72353236aaeb73bec97df788f66\"}]},{\"amount\":\"0\",\"outputs\":[{\"global_index\":\"2633596\",\"public_key\":\"9201769d3105e2c601fc748dde2d265aa8482941df2dd63667ddcfb5c8d0175c\",\"rct\":\"d4254c3aaaeb3328396cf042e4b10cd61a712569349406038621c0b061f97f70\"},{\"global_index\":\"7263762\",\"public_key\":\"bcdc4d2dea674cb7b7c8c65fbb6060ce9ee5f3449e6c65cea258ab1193b9bc35\",\"rct\":\"4e48a02f7056d19234ed1b21ca50344688fc737cd3a21df3f6292283c34db055\"},{\"global_index\":\"7373692\",\"public_key\":\"462f51785b1d90e554d72ebae7db4c0d4fbef32341b260a19fdccfec33d33cfa\",\"rct\":\"7419c1330bcdec0a8b0989b9a1c5d0455862e040b597c58591df8bdf92f433e2\"},{\"global_index\":\"7450519\",\"public_key\":\"48843aeaa09ed9c76aa9083f561a90ec9eca05df4e10fc8a9ae9cef11ab8661c\",\"rct\":\"8d79dfc05adbb2d47d3542eeb33b05e8c7db80d73961d7d99f240713e9f8c63f\"},{\"global_index\":\"7490436\",\"public_key\":\"1f9ee1cad77da88655e6164f8e863674b465257235ea1a219d8cb79f583e1554\",\"rct\":\"8018a907a77fd45ac5bbeca3cf1f7def348fa919978035503d51292622981b33\"},{\"global_index\":\"7531503\",\"public_key\":\"ff195ccc3f09483ce926c499ecdd525b5706148de996f7c506cc3d25db6e3e20\",\"rct\":\"db6e2ba255a3e3be8e00c1cc4a3de24037e36c1861e1092bcf61831d306d873e\"},{\"global_index\":\"7539183\",\"public_key\":\"7ef9ff4f7eeb1fccd290e01ae4fc6bdd88aaeb1742cd4715781f029d0d60a3c8\",\"rct\":\"14b2e69d5f77d9a125a9ee6042b636b5b3305c830f6d1ecf71d5e7c37f1f1941\"},{\"global_index\":\"7563953\",\"public_key\":\"b6523d8a24ca46db67671aec10d521aa990fdc5e6beac2cafcc829f512be1058\",\"rct\":\"b92decdcb4b0a9ca3dd233b6c0293691921211bc08db073e611bf0ec4524d121\"},{\"global_index\":\"7566210\",\"public_key\":\"d994890e9026a5830c6363261355e08d84d008c1d798b92887714a0e1dcdfc27\",\"rct\":\"aa9c6b3f6bb6f196e01b0bcf1a0ce9b9bc0cee72b8e37fcc3f06b87dd66bf81f\"},{\"global_index\":\"7566970\",\"public_key\":\"3684782e3eef48a3ce46b8b9fa0848003744847e6c649f5f45a973fb7ecb86ea\",\"rct\":\"03817bc1c51d5d37e12ecc032ad09a1cf679a39ba969a50d2e0ffda124bb8a80\"},{\"global_index\":\"7570610\",\"public_key\":\"006e51f0e4a3fbbfa3a4c1886c6917ae823f12eebe322da60da6f030f54c1a1d\",\"rct\":\"1250a56450cf2a41d19c4148e8a3466ef6f169aa036ae022a4fc70505c8e7b5b\"}]}]}";
//
BOOST_AUTO_TEST_CASE(bridge__transfers__send__sweepDust)
{
	using namespace serial_bridge;
	using namespace monero_transfer_utils;
	//
	// this being input as JSON merely for convenience
	boost::property_tree::ptree pt;
	stringstream ss;
	ss << DG_presweep__unspent_outs_json;
	boost::property_tree::json_parser::read_json(ss, pt);
	boost::property_tree::ptree unspent_outs = pt.get_child("unspent_outs");
	//
	// NOTE: in the real algorithm you should re-request this _each time step2 must be called_
	// this being input as JSON merely for convenience
	boost::property_tree::ptree mix_outs;
	{
		boost::property_tree::ptree pt;
		stringstream ss;
		ss << DG_presweep__rand_outs_json;
		boost::property_tree::json_parser::read_json(ss, pt);
		mix_outs = pt.get_child("mix_outs");
	}
	//
	// Send algorithm:
	// (Not implemented in C++ b/c the algorithm is split at the points (function interfaces) where requests must be done in e.g. JS-land, and implementing the retry integration in C++ would effectively be emscripten-only since it'd have to call out to C++. Plus this lets us retain the choice to retain synchrony
	bool tx_must_be_reconstructed = true; // for ease of writing this code, start this off true & structure whole thing as while loop
	optional<string> fee_actually_needed_string = none;
	size_t construction_attempt_n = 0;
	while (tx_must_be_reconstructed) {
		construction_attempt_n += 1; // merely kept for assertion purposes
		//
		optional<string> mixin_string;
		optional<string> change_amount_string;
		optional<string> using_fee_string;
		optional<string> final_total_wo_fee_string;
		boost::property_tree::ptree using_outs;
		{
			boost::property_tree::ptree root;
			root.put("is_sweeping", "true");
			root.put("payment_id_string", "d2f602b240fbe624"); // optl
			root.put("sending_amount", "0");
			root.put("fee_per_b", "24658");
			root.put("priority", "1");
			root.add_child("unspent_outs", unspent_outs);
			if (fee_actually_needed_string != none) {
				BOOST_REQUIRE(construction_attempt_n > 1);
				//
				// for next round's integration - if it needs to re-enter... arg "passedIn_attemptAt_fee"
				root.put("passedIn_attemptAt_fee", *fee_actually_needed_string);
			}
			auto ret_string = serial_bridge::send_step1__prepare_params_for_get_decoys(args_string_from_root(root));
			stringstream ret_stream;
			ret_stream << ret_string;
			boost::property_tree::ptree ret_tree;
			boost::property_tree::read_json(ret_stream, ret_tree);
			optional<uint32_t> err_code = ret_tree.get_optional<uint32_t>(ret_json_key__any__err_code());
			if (err_code != none && (CreateTransactionErrorCode)*err_code != monero_transfer_utils::noError) {
				if ((CreateTransactionErrorCode)*err_code == monero_transfer_utils::needMoreMoneyThanFound) {
					optional<string> spendable_balance_string = ret_tree.get_optional<string>(ret_json_key__send__spendable_balance());
					BOOST_REQUIRE(spendable_balance_string != none);
					BOOST_REQUIRE((*spendable_balance_string).size() > 0);
		//			uint64_t fee = stoull(*fee_string);
		//			BOOST_REQUIRE(fee == 135000000);
					cout << "bridge__transfers__send__sweepDust: step1: needMoreMoneyThanFound: spendable_balance " << *spendable_balance_string << endl;
					//
					optional<string> required_balance_string = ret_tree.get_optional<string>(ret_json_key__send__required_balance());
					BOOST_REQUIRE(required_balance_string != none);
					BOOST_REQUIRE((*required_balance_string).size() > 0);
		//			uint64_t fee = stoull(*fee_string);
		//			BOOST_REQUIRE(fee == 135000000);
					cout << "bridge__transfers__send__sweepDust: step1: needMoreMoneyThanFound: required_balance " << *required_balance_string << endl;
				}
				auto err_msg = err_msg_from_err_code__create_transaction((CreateTransactionErrorCode)*err_code);
				BOOST_REQUIRE_MESSAGE(false, err_msg);
				// ^--- not expecting anything like needMoreMoneyThanFound errors here
			}
			mixin_string = ret_tree.get_optional<string>(ret_json_key__send__mixin());
			BOOST_REQUIRE(mixin_string != none);
			BOOST_REQUIRE((*mixin_string).size() > 0);
			uint64_t mixin = stoull(*mixin_string);
			BOOST_REQUIRE(mixin == 10);
			cout << "bridge__transfers__send__sweepDust: mixin " << *mixin_string << endl;
			//
			using_fee_string = ret_tree.get_optional<string>(ret_json_key__send__using_fee()); // save this
			BOOST_REQUIRE(using_fee_string != none);
			BOOST_REQUIRE((*using_fee_string).size() > 0);
	//		uint64_t using_fee = stoull(*using_fee_string);
		//	BOOST_REQUIRE(using_fee == 10); // TODO:?
			cout << "bridge__transfers__send__sweepDust: step1: using_fee " << *using_fee_string << endl;
			//
			using_outs = ret_tree.get_child(ret_json_key__send__using_outs()); // save this for step2
			BOOST_FOREACH(boost::property_tree::ptree::value_type &output_desc, using_outs)
			{
				assert(output_desc.first.empty()); // array elements have no names
				cout << "bridge__transfers__send__sweepDust: step1: using_out " << output_desc.second.get<string>("public_key") << endl;
			}
			//
			change_amount_string = ret_tree.get_optional<string>(ret_json_key__send__change_amount());
			BOOST_REQUIRE(change_amount_string != none);
			BOOST_REQUIRE((*change_amount_string).size() > 0);
	//		uint64_t change_amount = stoull(*change_amount_string);
			//	BOOST_REQUIRE(change_amount == 10); // TODO:?
			cout << "bridge__transfers__send__sweepDust: step1: change_amount " << *change_amount_string << endl;
			//
			final_total_wo_fee_string = ret_tree.get_optional<string>(ret_json_key__send__final_total_wo_fee());
			BOOST_REQUIRE(final_total_wo_fee_string != none);
			BOOST_REQUIRE((*final_total_wo_fee_string).size() > 0);
	//		uint64_t final_total_wo_fee = stoull(*final_total_wo_fee_string);
			//	BOOST_REQUIRE(final_total_wo_fee == 10); // TODO:?
			cout << "bridge__transfers__send__sweepDust: step1: final_total_wo_fee " << *final_total_wo_fee_string << endl;
			//
		}
		{
			boost::property_tree::ptree root;
			root.put("final_total_wo_fee", *final_total_wo_fee_string);
			root.put("change_amount", *change_amount_string); // from step1
			root.put("fee_amount", *using_fee_string); // from step1
			root.add_child("using_outs", using_outs); // from step1
			//
			root.put("payment_id_string", "d2f602b240fbe624"); // optl
			root.put("nettype_string", string_from_nettype(MAINNET));
			root.put("to_address_string", "4APbcAKxZ2KPVPMnqa5cPtJK25tr7maE7LrJe67vzumiCtWwjDBvYnHZr18wFexJpih71Mxsjv8b7EpQftpB9NjPPXmZxHN");
			root.put("from_address_string", "43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg");
			root.put("sec_viewKey_string", "7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104");
			root.put("sec_spendKey_string", "4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803");
			root.put("fee_per_b", "24658");
			root.put("unlock_time", "0");
			root.put("priority", "1");
			root.add_child("mix_outs", mix_outs);
			//
			boost::property_tree::ptree ret_tree;
			auto ret_string = serial_bridge::send_step2__try_create_transaction(args_string_from_root(root));
			stringstream ret_stream;
			ret_stream << ret_string;
			boost::property_tree::read_json(ret_stream, ret_tree);
			optional<uint32_t> err_code = ret_tree.get_optional<uint32_t>(ret_json_key__any__err_code());
			if (err_code != none && (CreateTransactionErrorCode)*err_code != monero_transfer_utils::noError) {
				auto err_msg = err_msg_from_err_code__create_transaction((CreateTransactionErrorCode)*err_code);
				BOOST_REQUIRE_MESSAGE(false, err_msg);
			}
			//
			tx_must_be_reconstructed = ret_tree.get<bool>(ret_json_key__send__tx_must_be_reconstructed());
			cout << "bridge__transfers__send__sweepDust: step2: must tx be reconstructed? " << tx_must_be_reconstructed << endl;
			if (tx_must_be_reconstructed) {
				// declared outside the for-loop so that we can use it for subsequent iterations if needed:
				fee_actually_needed_string = ret_tree.get_optional<string>(ret_json_key__send__fee_actually_needed());
				BOOST_REQUIRE(fee_actually_needed_string != none);
				BOOST_REQUIRE((*fee_actually_needed_string).size() > 0);
				//
				// This means we must go back through step1 to re-pick using_outs
				BOOST_REQUIRE(construction_attempt_n < 7); // not generally expecting to have to do this more than once or twice - i did see < 3 insufficient once so raised this
				continue; // proceed to next iteration (re-enter tx construction at step1(II) with fee_actually_needed_string from step2(I))
			}
			optional<string> tx_hash = ret_tree.get_optional<string>(ret_json_key__send__tx_hash());
			optional<string> tx_key_string = ret_tree.get_optional<string>(ret_json_key__send__tx_key());
			optional<string> serialized_signed_tx = ret_tree.get_optional<string>(ret_json_key__send__serialized_signed_tx());
			BOOST_REQUIRE(serialized_signed_tx != none);
			BOOST_REQUIRE((*serialized_signed_tx).size() > 0);
			cout << "bridge__transfers__send__sweepDust: serialized_signed_tx: " << *serialized_signed_tx << endl;
			BOOST_REQUIRE(tx_hash != none);
			BOOST_REQUIRE((*tx_hash).size() > 0);
			cout << "bridge__transfers__send__sweepDust: tx_hash: " << *tx_hash << endl;
			BOOST_REQUIRE(tx_key_string != none);
			BOOST_REQUIRE((*tx_key_string).size() > 0);
			cout << "bridge__transfers__send__sweepDust: tx_key_string: " << *tx_key_string << endl;
		}
	}
}
BOOST_AUTO_TEST_CASE(bridge__transfers__send__amountWOnlyDusty)
{
	using namespace serial_bridge;
	using namespace monero_transfer_utils;
	//
	// this being input as JSON merely for convenience
	boost::property_tree::ptree pt;
	stringstream ss;
	ss << DG_presweep__unspent_outs_json;
	boost::property_tree::json_parser::read_json(ss, pt);
	boost::property_tree::ptree unspent_outs = pt.get_child("unspent_outs");
	//
	optional<string> mixin_string;
	optional<string> change_amount_string;
	optional<string> using_fee_string;
	optional<string> final_total_wo_fee_string;
	boost::property_tree::ptree using_outs;
	boost::property_tree::ptree root;
	root.put("is_sweeping", "false");
	root.put("payment_id_string", "d2f602b240fbe624"); // optl
	root.put("sending_amount", "1000000");
	root.put("fee_per_b", "24658");
	root.put("priority", "1");
	root.add_child("unspent_outs", unspent_outs);

	auto ret_string = serial_bridge::send_step1__prepare_params_for_get_decoys(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<uint32_t> err_code = ret_tree.get_optional<uint32_t>(ret_json_key__any__err_code());
	BOOST_REQUIRE_MESSAGE(err_code != none && *err_code == needMoreMoneyThanFound, "Expected needMoreMoneyThanFound");
	// ^--- expecting needMoreMoneyThanFound
}
string DG_postsweep__unspent_outs_json = "{\"unspent_outs\":[{\"amount\":\"3000000000\",\"public_key\":\"41be1978f58cabf69a9bed5b6cb3c8d588621ef9b67602328da42a213ee42271\",\"index\":1,\"global_index\":7611174,\"rct\":\"86a2c9f1f8e66848cd99bfda7a14d4ac6c3525d06947e21e4e55fe42a368507eb5b234ccdd70beca8b1fc8de4f2ceb1374e0f1fd8810849e7f11316c2cc063060008ffa5ac9827b776993468df21af8c963d12148622354f950cbe1369a92a0c\",\"tx_id\":5334971,\"tx_hash\":\"9d37c7fdeab91abfd1e7e120f5c49eac17b7ac04a97a0c93b51c172115df21ea\",\"tx_pub_key\":\"bd703d7f37995cc7071fb4d2929594b5e2a4c27d2b7c68a9064500ca7bc638b8\"}]}";
string DG_postsweep__rand_outs_json = "{\"mix_outs\":[{\"amount\":\"0\",\"outputs\":[{\"global_index\":\"7453099\",\"public_key\":\"31f3a7fec0f6f09067e826b6c2904fd4b1684d7893dcf08c5b5d22e317e148bb\",\"rct\":\"ea6bcb193a25ce2787dd6abaaeef1ee0c924b323c6a5873db1406261e86145fc\"},{\"global_index\":\"7500097\",\"public_key\":\"f9d923500671da05a1bf44b932b872f0c4a3c88e6b3d4bf774c8be915e25f42b\",\"rct\":\"dcae4267a6c382bcd71fd1af4d2cbceb3749d576d7a3acc473dd579ea9231a52\"},{\"global_index\":\"7548483\",\"public_key\":\"839cbbb73685654b93e824c4843e745e8d5f7742e83494932307bf300641c480\",\"rct\":\"aa99d492f1d6f1b20dcd95b8fff8f67a219043d0d94b4551759016b4888573e7\"},{\"global_index\":\"7554755\",\"public_key\":\"b8860f0697988c8cefd7b4285fbb8bec463f136c2b9a9cadb3e57cebee10717f\",\"rct\":\"327f9b07bee9c4c25b5a990123cd2444228e5704ebe32016cd632866710279b5\"},{\"global_index\":\"7561477\",\"public_key\":\"561d734cb90bc4a64d49d37f85ea85575243e2ed749a3d6dcb4d27aa6bec6e88\",\"rct\":\"b5393e038df95b94bfda62b44a29141cac9e356127270af97193460d51949841\"},{\"global_index\":\"7567062\",\"public_key\":\"db1024ef67e7e73608ef8afab62f49e2402c8da3dc3197008e3ba720ad3c94a8\",\"rct\":\"1fedf95621881b77f823a70aa83ece26aef62974976d2b8cd87ed4862a4ec92c\"},{\"global_index\":\"7567508\",\"public_key\":\"6283f3cd2f050bba90276443fe04f6076ad2ad46a515bf07b84d424a3ba43d27\",\"rct\":\"10e16bb8a8b7b0c8a4b193467b010976b962809c9f3e6c047335dba09daa351f\"},{\"global_index\":\"7568716\",\"public_key\":\"7a7deb4eef81c1f5ce9cbd0552891cb19f1014a03a5863d549630824c7c7c0d3\",\"rct\":\"735d059dc3526334ac705ddc44c4316bb8805d2426dcea9544cde50cf6c7a850\"},{\"global_index\":\"7571196\",\"public_key\":\"535208e354cae530ed7ce752935e555d630cf2edd7f91525024ed9c332b2a347\",\"rct\":\"c3cf838faa14e993536c5581ca582fb0d96b70f713cf88f7f15c89336e5853ec\"},{\"global_index\":\"7571333\",\"public_key\":\"e73f27b7eb001aa7eac13df82814cda65b42ceeb6ef36227c25d5cbf82f6a5e4\",\"rct\":\"5f45f33c6800cdae202b37abe6d87b53d6873e7b30f3527161f44fa8db3104b6\"},{\"global_index\":\"7571335\",\"public_key\":\"fce982db8e7a6b71a1e632c7de8c5cbf54e8bacdfbf250f1ffc2a8d2f7055ce3\",\"rct\":\"407bdcc48e70eb3ef2cc22cefee6c6b5a3c59fd17bde12fda5f1a44a0fb39d14\"}]}]}";

BOOST_AUTO_TEST_CASE(bridge__transfers__send__amount)
{
	using namespace serial_bridge;
	using namespace monero_transfer_utils;
	//
	// this being input as JSON merely for convenience
	boost::property_tree::ptree pt;
	stringstream ss;
	ss << DG_postsweep__unspent_outs_json;
	boost::property_tree::json_parser::read_json(ss, pt);
	boost::property_tree::ptree unspent_outs = pt.get_child("unspent_outs");
	//
	// NOTE: in the real algorithm you should re-request this _each time step2 must be called_
	// this being input as JSON merely for convenience
	boost::property_tree::ptree mix_outs;
	{
		boost::property_tree::ptree pt;
		stringstream ss;
		ss << DG_postsweep__rand_outs_json;
		boost::property_tree::json_parser::read_json(ss, pt);
		mix_outs = pt.get_child("mix_outs");
	}
	//
	// Send algorithm:
	bool tx_must_be_reconstructed = true; // for ease of writing this code, start this off true & structure whole thing as while loop
	optional<string> fee_actually_needed_string = none;
	size_t construction_attempt_n = 0;
	while (tx_must_be_reconstructed) {
		construction_attempt_n += 1; // merely kept for assertion purposes
		//
		optional<string> mixin_string;
		optional<string> change_amount_string;
		optional<string> using_fee_string;
		optional<string> final_total_wo_fee_string;
		boost::property_tree::ptree using_outs;
		{
			boost::property_tree::ptree root;
			root.put("is_sweeping", "false");
			root.put("payment_id_string", "d2f602b240fbe624"); // optl
			root.put("sending_amount", "200000000");
			root.put("fee_per_b", "24658");
			root.put("priority", "1");
			root.add_child("unspent_outs", unspent_outs);
			if (fee_actually_needed_string != none) {
				BOOST_REQUIRE(construction_attempt_n > 1);
				//
				// for next round's integration - if it needs to re-enter... arg "passedIn_attemptAt_fee"
				root.put("passedIn_attemptAt_fee", *fee_actually_needed_string);
			}
			auto ret_string = serial_bridge::send_step1__prepare_params_for_get_decoys(args_string_from_root(root));
			stringstream ret_stream;
			ret_stream << ret_string;
			boost::property_tree::ptree ret_tree;
			boost::property_tree::read_json(ret_stream, ret_tree);
			optional<uint32_t> err_code = ret_tree.get_optional<uint32_t>(ret_json_key__any__err_code());
			if (err_code != none && (CreateTransactionErrorCode)*err_code != monero_transfer_utils::noError) {
				if ((CreateTransactionErrorCode)*err_code == monero_transfer_utils::needMoreMoneyThanFound) {
					optional<string> spendable_balance_string = ret_tree.get_optional<string>(ret_json_key__send__spendable_balance());
					BOOST_REQUIRE(spendable_balance_string != none);
					BOOST_REQUIRE((*spendable_balance_string).size() > 0);
					//			uint64_t fee = stoull(*fee_string);
					//			BOOST_REQUIRE(fee == 135000000);
					cout << "bridge__transfers__send__amount: step1: needMoreMoneyThanFound: spendable_balance " << *spendable_balance_string << endl;
					//
					optional<string> required_balance_string = ret_tree.get_optional<string>(ret_json_key__send__required_balance());
					BOOST_REQUIRE(required_balance_string != none);
					BOOST_REQUIRE((*required_balance_string).size() > 0);
					//			uint64_t fee = stoull(*fee_string);
					//			BOOST_REQUIRE(fee == 135000000);
					cout << "bridge__transfers__send__amount: step1: needMoreMoneyThanFound: required_balance " << *required_balance_string << endl;
				}
				auto err_msg = err_msg_from_err_code__create_transaction((CreateTransactionErrorCode)*err_code);
				BOOST_REQUIRE_MESSAGE(false, err_msg);
				// ^--- not expecting anything like needMoreMoneyThanFound errors here
			}
			mixin_string = ret_tree.get_optional<string>(ret_json_key__send__mixin());
			BOOST_REQUIRE(mixin_string != none);
			BOOST_REQUIRE((*mixin_string).size() > 0);
			uint64_t mixin = stoull(*mixin_string);
			BOOST_REQUIRE(mixin == 10);
			cout << "bridge__transfers__send__amount: mixin " << *mixin_string << endl;
			//
			using_fee_string = ret_tree.get_optional<string>(ret_json_key__send__using_fee()); // save this
			BOOST_REQUIRE(using_fee_string != none);
			BOOST_REQUIRE((*using_fee_string).size() > 0);
			//		uint64_t using_fee = stoull(*using_fee_string);
			//	BOOST_REQUIRE(using_fee == 10); // TODO:?
			cout << "bridge__transfers__send__amount: step1: using_fee " << *using_fee_string << endl;
			//
			using_outs = ret_tree.get_child(ret_json_key__send__using_outs()); // save this for step2
			BOOST_FOREACH(boost::property_tree::ptree::value_type &output_desc, using_outs)
			{
				assert(output_desc.first.empty()); // array elements have no names
				cout << "bridge__transfers__send__amount: step1: using_out " << output_desc.second.get<string>("public_key") << endl;
			}
			//
			change_amount_string = ret_tree.get_optional<string>(ret_json_key__send__change_amount());
			BOOST_REQUIRE(change_amount_string != none);
			BOOST_REQUIRE((*change_amount_string).size() > 0);
			//		uint64_t change_amount = stoull(*change_amount_string);
			//	BOOST_REQUIRE(change_amount == 10); // TODO:?
			cout << "bridge__transfers__send__amount: step1: change_amount " << *change_amount_string << endl;
			//
			final_total_wo_fee_string = ret_tree.get_optional<string>(ret_json_key__send__final_total_wo_fee());
			BOOST_REQUIRE(final_total_wo_fee_string != none);
			BOOST_REQUIRE((*final_total_wo_fee_string).size() > 0);
			//		uint64_t final_total_wo_fee = stoull(*final_total_wo_fee_string);
			//	BOOST_REQUIRE(final_total_wo_fee == 10); // TODO:?
			cout << "bridge__transfers__send__amount: step1: final_total_wo_fee " << *final_total_wo_fee_string << endl;
			//
		}
		{
			boost::property_tree::ptree root;
			root.put("final_total_wo_fee", *final_total_wo_fee_string);
			root.put("change_amount", *change_amount_string); // from step1
			root.put("fee_amount", *using_fee_string); // from step1
			root.add_child("using_outs", using_outs); // from step1
			//
			root.put("payment_id_string", "d2f602b240fbe624"); // optl
			root.put("nettype_string", string_from_nettype(MAINNET));
			root.put("to_address_string", "4APbcAKxZ2KPVPMnqa5cPtJK25tr7maE7LrJe67vzumiCtWwjDBvYnHZr18wFexJpih71Mxsjv8b7EpQftpB9NjPPXmZxHN");
			root.put("from_address_string", "43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg");
			root.put("sec_viewKey_string", "7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104");
			root.put("sec_spendKey_string", "4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803");
			root.put("fee_per_b", "24658");
			root.put("unlock_time", "0");
			root.put("priority", "1");
			root.add_child("mix_outs", mix_outs);
			//
			boost::property_tree::ptree ret_tree;
			auto ret_string = serial_bridge::send_step2__try_create_transaction(args_string_from_root(root));
			stringstream ret_stream;
			ret_stream << ret_string;
			boost::property_tree::read_json(ret_stream, ret_tree);
			optional<uint32_t> err_code = ret_tree.get_optional<uint32_t>(ret_json_key__any__err_code());
			if (err_code != none && (CreateTransactionErrorCode)*err_code != monero_transfer_utils::noError) {
				auto err_msg = err_msg_from_err_code__create_transaction((CreateTransactionErrorCode)*err_code);
				BOOST_REQUIRE_MESSAGE(false, err_msg);
			}
			//
			tx_must_be_reconstructed = ret_tree.get<bool>(ret_json_key__send__tx_must_be_reconstructed());
			cout << "bridge__transfers__send__amount: step2: must tx be reconstructed? " << tx_must_be_reconstructed << endl;
			if (tx_must_be_reconstructed) {
				// declared outside the for-loop so that we can use it for subsequent iterations if needed:
				fee_actually_needed_string = ret_tree.get_optional<string>(ret_json_key__send__fee_actually_needed());
				BOOST_REQUIRE(fee_actually_needed_string != none);
				BOOST_REQUIRE((*fee_actually_needed_string).size() > 0);
				//
				// This means we must go back through step1 to re-pick using_outs
				BOOST_REQUIRE(construction_attempt_n < 3); // not generally expecting to have to do this more than once or twice
				continue; // proceed to next iteration (re-enter tx construction at step1(II) with fee_actually_needed_string from step2(I))
			}
			optional<string> tx_hash = ret_tree.get_optional<string>(ret_json_key__send__tx_hash());
			optional<string> tx_key_string = ret_tree.get_optional<string>(ret_json_key__send__tx_key());
			optional<string> serialized_signed_tx = ret_tree.get_optional<string>(ret_json_key__send__serialized_signed_tx());
			BOOST_REQUIRE(serialized_signed_tx != none);
			BOOST_REQUIRE((*serialized_signed_tx).size() > 0);
			cout << "bridge__transfers__send__amount: serialized_signed_tx: " << *serialized_signed_tx << endl;
			BOOST_REQUIRE(tx_hash != none);
			BOOST_REQUIRE((*tx_hash).size() > 0);
			cout << "bridge__transfers__send__amount: tx_hash: " << *tx_hash << endl;
			BOOST_REQUIRE(tx_key_string != none);
			BOOST_REQUIRE((*tx_key_string).size() > 0);
			cout << "bridge__transfers__send__amount: tx_key_string: " << *tx_key_string << endl;
		}
	}
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
BOOST_AUTO_TEST_CASE(bridged__validate_components_for_login__subaddress)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("address_string", "852t3x5nfQ7PLnavxP1Q6S2ff18tvy8Cih4ikkd61aqd2BV5iTaeY6PFBeLPcHZdrvfumGJx9z1Md6fwyxKSykyHKJAMrk6");
	root.put("sec_viewKey_string", "…");
	root.put("sec_spendKey_string", "…");
	root.put("seed_string", "…");
	root.put("nettype_string", string_from_nettype(MAINNET));
	//
	auto ret_string = serial_bridge::validate_components_for_login(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	BOOST_REQUIRE(err_string != none);
	BOOST_REQUIRE((*err_string).compare("Can't log in with a sub-address") == 0);
	optional<bool> isValid = ret_tree.get_optional<bool>(ret_json_key__isValid());
	BOOST_REQUIRE(isValid == none || *isValid == false);
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
	root.put("fee_per_b", "24658");
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
	BOOST_REQUIRE(fee == 330047330);
	cout << "bridged__estimated_tx_network_fee: " << fee << endl;
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
//
BOOST_AUTO_TEST_CASE(bridged__address_and_keys_from_seed)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("seed_string", "9c973aa296b79bbf452781dd3d32ad7f");
	root.put("nettype_string", string_from_nettype(MAINNET));
	//
	auto ret_string = serial_bridge::address_and_keys_from_seed(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> address_string = ret_tree.get_optional<string>(ret_json_key__address_string());
	BOOST_REQUIRE(address_string != none);
	BOOST_REQUIRE((*address_string).size() > 0);
	cout << "bridged__address_and_keys_from_seed: address: " << *address_string << endl;
	BOOST_REQUIRE(*address_string == "43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg");
	optional<string> pub_viewKey_string = ret_tree.get_optional<string>(ret_json_key__pub_viewKey_string());
	BOOST_REQUIRE(pub_viewKey_string != none);
	BOOST_REQUIRE((*pub_viewKey_string).size() > 0);
	cout << "bridged__address_and_keys_from_seed: pub_viewKey_string: " << *pub_viewKey_string << endl;
	optional<string> pub_spendKey_string = ret_tree.get_optional<string>(ret_json_key__pub_spendKey_string());
	BOOST_REQUIRE(pub_spendKey_string != none);
	BOOST_REQUIRE((*pub_spendKey_string).size() > 0);
	cout << "bridged__address_and_keys_from_seed: pub_spendKey_string: " << *pub_spendKey_string << endl;
	optional<string> sec_viewKey_string = ret_tree.get_optional<string>(ret_json_key__sec_viewKey_string());
	BOOST_REQUIRE(sec_viewKey_string != none);
	BOOST_REQUIRE((*sec_viewKey_string).size() > 0);
	BOOST_REQUIRE(*sec_viewKey_string == "7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104");
	cout << "bridged__address_and_keys_from_seed: sec_viewKey_string: " << *sec_viewKey_string << endl;
	optional<string> sec_spendKey_string = ret_tree.get_optional<string>(ret_json_key__sec_spendKey_string());
	BOOST_REQUIRE(sec_spendKey_string != none);
	BOOST_REQUIRE((*sec_spendKey_string).size() > 0);
	BOOST_REQUIRE(*sec_spendKey_string == "4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803");
	cout << "bridged__address_and_keys_from_seed: sec_spendKey_string: " << *sec_spendKey_string << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__derive_public_key)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("derivation", "591c749f1868c58f37ec3d2a9d2f08e7f98417ac4f8131e3a57c1fd71273ad00");
	root.put("out_index", "1");
	root.put("pub", "904e49462268d771cc1649084c35aa1296bfb214880fe2e7f373620a3e2ba597");
	//
	auto ret_string = serial_bridge::derive_public_key(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> str = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(str != none);
	BOOST_REQUIRE((*str).size() > 0);
	BOOST_REQUIRE(*str == "da26518ddb54cde24ccfc59f36df13bbe9bdfcb4ef1b223d9ab7bef0a50c8be3");
	cout << "bridged__derive_public_key: " << *str << endl;
}
BOOST_AUTO_TEST_CASE(bridged__derive_subaddress_public_key)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("derivation", "591c749f1868c58f37ec3d2a9d2f08e7f98417ac4f8131e3a57c1fd71273ad00");
	root.put("out_index", "1");
	root.put("output_key", "904e49462268d771cc1649084c35aa1296bfb214880fe2e7f373620a3e2ba597");
	//
	auto ret_string = serial_bridge::derive_subaddress_public_key(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> str = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(str != none);
	BOOST_REQUIRE((*str).size() > 0);
	BOOST_REQUIRE(*str == "dfc9e4a0039e913204c1c0f78e954a7ec7ce291d8ffe88265632f0da9d8de1be");
	cout << "bridged__derive_subaddress_public_key: " << *str << endl;
}
BOOST_AUTO_TEST_CASE(bridged__generate_key_derivation)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("pub", "904e49462268d771cc1649084c35aa1296bfb214880fe2e7f373620a3e2ba597");
	root.put("sec", "52aa4c69b93b780885c9d7f51e6fd5795904962c61a2e07437e130784846f70d");
	//
	auto ret_string = serial_bridge::generate_key_derivation(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	optional<string> derivation = ret_tree.get_optional<string>(ret_json_key__generic_retVal());
	BOOST_REQUIRE(derivation != none);
	BOOST_REQUIRE((*derivation).size() > 0);
	BOOST_REQUIRE(*derivation == "591c749f1868c58f37ec3d2a9d2f08e7f98417ac4f8131e3a57c1fd71273ad00");
	cout << "bridged__generate_key_derivation: derivation: " << *derivation << endl;
}
//
BOOST_AUTO_TEST_CASE(bridged__decodeRct)
{
	using namespace serial_bridge;
	//
	boost::property_tree::ptree root;
	root.put("i", "1");
	root.put("sk", "9b1529acb638f497d05677d7505d354b4ba6bc95484008f6362f93160ef3e503");
	
	boost::property_tree::ptree rv;
	{
		rv.put("type", "1");
		//
		boost::property_tree::ptree ecdhInfo;
		{
			boost::property_tree::ptree ecdh_info;
			ecdh_info.put("mask", "3ad9d0b3398691b94558e0f750e07e5e0d7d12411cd70b3841159e6c6b10db02");
			ecdh_info.put("amount", "b3189d8adb5a26568e497eb8e376a7d7d946ebb1daef4c2c87a2c30b65915506");
			ecdhInfo.push_back(std::make_pair("", ecdh_info));
		}
		{
			boost::property_tree::ptree ecdh_info;
			ecdh_info.put("mask", "97b00af8ecba3cb71b9660cc9e1ac110abd21a4c5e50a2c125f964caa96bef0c");
			ecdh_info.put("amount", "60269d8adb5a26568e497eb8e376a7d7d946ebb1daef4c2c87a2c30b65915506");
			ecdhInfo.push_back(std::make_pair("", ecdh_info));
		}
		{
			boost::property_tree::ptree ecdh_info;
			ecdh_info.put("mask", "db67f5066d9455db404aeaf435ad948bc9f27344bc743e3a32583a9e6695cb08");
			ecdh_info.put("amount", "b3189d8adb5a26568e497eb8e376a7d7d946ebb1daef4c2c87a2c30b65915506");
			ecdhInfo.push_back(std::make_pair("", ecdh_info));
		}
		rv.add_child("ecdhInfo", ecdhInfo);
		//
		boost::property_tree::ptree outPk;
		{
			boost::property_tree::ptree an_outPk;
			an_outPk.put("mask", "9adc531a9c79a49a4257f24e5e5ea49c2fc1fb4eef49e00d5e5aba6cb6963a7d");
			outPk.push_back(std::make_pair("", an_outPk));
		}
		{
			boost::property_tree::ptree an_outPk;
			an_outPk.put("mask", "89f40499d6786a4027a24d6674d0940146fd12d8bc6007d338f19f05040e7a41");
			outPk.push_back(std::make_pair("", an_outPk));
		}
		{
			boost::property_tree::ptree an_outPk;
			an_outPk.put("mask", "f413d28bd5ffdc020528bcb2c19919d7484fbc9c3dd30de34ecff5b8a904e7f6");
			outPk.push_back(std::make_pair("", an_outPk));
		}
		rv.add_child("outPk", outPk);
	}
	root.add_child("rv", rv);
	//
	auto ret_string = serial_bridge::decodeRct(args_string_from_root(root));
	stringstream ret_stream;
	ret_stream << ret_string;
	boost::property_tree::ptree ret_tree;
	boost::property_tree::read_json(ret_stream, ret_tree);
	optional<string> err_string = ret_tree.get_optional<string>(ret_json_key__any__err_msg());
	if (err_string != none) {
		BOOST_REQUIRE_MESSAGE(false, *err_string);
	}
	string mask_string = ret_tree.get<string>(ret_json_key__decodeRct_mask());
	BOOST_REQUIRE(mask_string.size() > 0);
	cout << "bridged__decodeRct: mask_string: " << mask_string << endl;
	BOOST_REQUIRE(mask_string == "3f59c741c9ad560bfea92f42449a180bc8362f1b5ddd957e3b5772dbaf7f840e");
	string amount_string = ret_tree.get<string>(ret_json_key__decodeRct_amount());
	BOOST_REQUIRE(amount_string.size() > 0);
	cout << "bridged__decodeRct: amount_string: " << amount_string << endl;
	BOOST_REQUIRE(amount_string == "4501"); // FIXME is this correct?
}
//
//
// NOTE: output 0's rct field is actually borrowed from output 1 since it got deleted (and the stagenet account which produced this data has since been swept). 
string OM_stagenet__unspent_outs_json = "{\"unspent_outs\":[{\"amount\":\"25281219529517\",\"rct\":\"8b22c4fadb152cb8e6c6dd21d2cf46b668a3657fcf666973c7f2a8354eae384501000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\",\"global_index\":642806,\"height\":189324,\"index\":0,\"public_key\":\"4e8f890509b157125cc529218a97f4c5e5711af3b36e6d04881391105b8de2c7\",\"tx_hash\":\"0bb8cfdb2c5c142c87faaf3cc6523fdf82fb5435cb382218a5df3e338202fa8e\",\"tx_id\":257,\"tx_prefix_hash\":\"51799f1647ec5d98255db8a51ecea3aebdc7c6015f6cef166643d0334f592915\",\"tx_pub_key\":\"e8cd4671aa2be2f1b169c1dc0c5e80ed6b19149b336bb02d825fb1728677eae2\"},{\"amount\":\"25280399800334\",\"global_index\":642823,\"height\":189341,\"index\":0,\"public_key\":\"6c69745ebadc04ff068e55ff113d1c44658f7b5b4311e748a8fcccb261dac263\",\"rct\":\"8b22c4fadb152cb8e6c6dd21d2cf46b668a3657fcf666973c7f2a8354eae384501000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\",\"tx_hash\":\"70d1678c23289d45f6d556bdfed0de52559141edb2645a62935027032c8e891e\",\"tx_id\":258,\"tx_prefix_hash\":\"1e43e7aefcea766096d7ab902dc63dc11dc6cb4d7db8511eb01ef65457c69d9a\",\"tx_pub_key\":\"36fc11368f364e85e3863176487b06d705c3bf901d997b7a2683c567a95e2777\"},{\"amount\":\"25280158708574\",\"global_index\":642828,\"height\":189346,\"index\":0,\"public_key\":\"977f3d24792f583a8c8fc3123c35589c83e1a29e573cfaf35d6957f9bb286469\",\"rct\":\"761f8ac29fc66b8d004600155caf98c5a1db7173d616a696a520969425cd647a01000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\",\"tx_hash\":\"45c7151f27376cdabf3e0e6c0a6412ad7b74cf06e8aff1f1f18eda96c7aa2755\",\"tx_id\":259,\"tx_prefix_hash\":\"67c48668d2340692a348d89b55e6888833a2b219028b29947cebb6b1210b14f3\",\"tx_pub_key\":\"f21b25d38bf0b64eda40434f88fffa2e574d894ee788a97710acea189978a180\"},{\"amount\":\"25280062272513\",\"global_index\":642830,\"height\":189348,\"index\":0,\"public_key\":\"21df141a799f53b262c89293e2dc4f99fa661de3378cd65ad709608720b2ded8\",\"rct\":\"92c27bb967345986e94b02ec14558eaf7d93168f2e7a4376c22e9d55a5fb89b801000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\",\"tx_hash\":\"3a42dc8deaf1a9d1188e36b2354eb140fdd34c36c18b9232b0c163b480ffb762\",\"tx_id\":260,\"tx_prefix_hash\":\"76e3943d64f731071f35b0e0f28b7e084285ff32ee91d4eb4affb51f39bd26e1\",\"tx_pub_key\":\"e1eb916dc0c92c05da1bfa620af6750b8d8713a552e03d470b113d73f84f489f\"},{\"amount\":\"25279628314794\",\"global_index\":642839,\"height\":189357,\"index\":0,\"public_key\":\"0bfe8c187810c13ed423cbeb19219ec41732489a492b492e4069c448eef94b1c\",\"rct\":\"8f4c588457e4c162d12a1927307fa1c4065fff90da75a400629415dddc655b3b01000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\",\"tx_hash\":\"ef9fde50a44ea003d51fe555f75f73995135e10f1783aa095fd4f494a026dde4\",\"tx_id\":261,\"tx_prefix_hash\":\"a1b0cf4ec0415257da3cc4854a9da975a3a5673b161dab2ec31da3858cdfa1f8\",\"tx_pub_key\":\"d2f0cbb319de4de653e683ea552d9856d0de99c323078c74bbcae5d2a6784328\"},{\"amount\":\"25279242580853\",\"global_index\":642847,\"height\":189365,\"index\":0,\"public_key\":\"f7d0bfea20a9a73346c868af3a925e17f30f3eda513023c513a59883e7edfd3d\",\"rct\":\"7f91e3fd454d27e231f5b90909be59f067ca704d9e9e92555b6a255145c369cd01000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\",\"tx_hash\":\"f982fd2b94333f63ac8294037cc6a8afdf6cd87f024fb453a24bad1e6259393d\",\"tx_id\":262,\"tx_prefix_hash\":\"d8c283e8f8d79a72c02c0869d9b629e7d8dafe7f8a32e17e12e16bd68d5fce6c\",\"tx_pub_key\":\"edcdafda5b834b0dd7ecd748a2b73b65c36c09853c32345d4146579eecb9614c\"},{\"amount\":\"25280290942143\",\"global_index\":642850,\"height\":189368,\"index\":0,\"public_key\":\"9e9b9ec98bf50382ad64ae178eda96455cce5b494623f28fb36b09dc750b7f28\",\"rct\":\"f60426634efcff6e84657c1be104fa91c413009c9ca4f0c170c52bbdef99e41f01000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\",\"tx_hash\":\"75cedfd148c291b55b5f70b4bde8215db1fc858287b7eaf2b1245eaf969b2b77\",\"tx_id\":263,\"tx_prefix_hash\":\"1b76cd6dde6b73c427468eee0d8d60e637cfed15809ffa6ac15f3927549d7562\",\"tx_pub_key\":\"0bf1f11812a40b06f05a38e7c0dc75ce3fd3312994e4c4185e85c53b53a7fa48\"},{\"amount\":\"25278712206295\",\"global_index\":642866,\"height\":189376,\"index\":0,\"public_key\":\"c6a90657353543930ff1135923d56aacbbf7f162d639d75e1c8f815852d71824\",\"rct\":\"1c0f1e4f7fe3666088c2f4e58289f36e49544a31bfe3ec597094b94ac90a1f9a01000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\",\"tx_hash\":\"c33a2e9daa82c32a4fcdbc1805785d80dc115314fd6f40a07b399e4e76d637af\",\"tx_id\":264,\"tx_prefix_hash\":\"ad41c4b185fed1359469cb4621d1147d1ff1389299b9a20d8152eab79c46f82a\",\"tx_pub_key\":\"538950955fd8a944a916f0ed6122313a05db1404d55aa603215fcd149c7b264e\"}]}";
string OM_stagenet__rand_outs_json = "{\"mix_outs\":[{\"amount\":\"0\",\"outputs\":[{\"global_index\":396567,\"public_key\":\"841c0540176bf72d0cf44806cb481fd1c1ddb1423c8e158ecb154626de4aa219\",\"rct\":\"66c40a4cda875983e23ae7fa23fab2c3b0ec210b5aad838d85583eb0185b78ea987ca3b45e1c5396a0b493a21e4c765a1dae6e3fdf428d7b6b9718e47557910bf52392367413b6d6901624ee0df46fe2b1b2b820c68b36029677b35d18c55408\"},{\"global_index\":549126,\"public_key\":\"931063eda20cbbccfbec5ca1e776123d84eb9b524c0e10f2e8b9c9245f60ed12\",\"rct\":\"052756b6c25633a544ebda021de45bdca71c13d75dad47550bfde2300bb0effa479b2e96bf533a9342ae66d06fe05d8b6ad0d63d40e5bcee1e1949c015d5fc0e8bee43e1b67cecee6c990fe026c5ad8eb23f46f09f5bbf1132b695a0d295a205\"},{\"global_index\":107684,\"public_key\":\"4c72b36d9fe5cb85a99bd3e2092ea3cc405ee6c8a23d2062f9909b8be17ba68f\",\"rct\":\"f610785155c4841ce28cff5a524eb5a89206667b3886cdf361b5181e2698618900000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\"},{\"global_index\":599301,\"public_key\":\"50fa87714baaa3c4aa0a1521e034c142c33016600c2993707b950233f89cd88d\",\"rct\":\"deb98ad8d8ebb48f977e0b6c7b5631cef2edb071ee6b5336302db3cb1a76db2900000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\"},{\"global_index\":440995,\"public_key\":\"5b38f11f0b532c7bb168fc53180b5044fce17b65f8c46b25d661c3bef7a84d9b\",\"rct\":\"7e097bd6e2655c678d134789c09b6cf8b5f408ab5daf438d5b09d8629e09f6ce429d1d3f8ed030c2b9386c91f2166c65f7bd022e21e7f9d49785abc2dc081609fdabe18cc6216be4148419786299282d6229af1b3242ec1579beb571afcfa607\"},{\"global_index\":623015,\"public_key\":\"07137bc9e412eecae31871279fb80896c771b0ea408cb40d9f9e683de13de6ff\",\"rct\":\"8565e3131cf34a4cb0a62c1f340cfb313d32010786e0ad88fcbdfaec1d3c0c8b00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\"},{\"global_index\":317661,\"public_key\":\"f50a4d572230aba56cb08ca84429dfd77a8564304ef4ba8ba847e00345a655c9\",\"rct\":\"d0d0be0296aad2412058fc65372aee9a61fa7df0efc6bb6cca6b5bec131dc9957573322031219bb1cbaa108a3ecbd3e5cc13a3c7d48dd70c792c80fe6c43640057f55f71b565dd90ca24a403ba93fe48ffae36b6a94d1b773be008fbfd513e05\"},{\"global_index\":153786,\"public_key\":\"68bae4aa3a12b2ccfe4a82f2ca90c9f0a1dade21744a383de26d8e366f468ab8\",\"rct\":\"32e0d57de61a3b4bf70a08686be1b1592db72f9e0be0123df8095d9a4b98b62dd2f283a225d57ccf768be2c4004c875e36371681dc5fb030ef348ba8c22be40740e531ea20c8dd200823c9363eb37a11a1121a83cfe063ca3547d58745a6d101\"},{\"global_index\":360768,\"public_key\":\"758b308d7297d2da2d4872c2db6f61100f5fb7eb7aaa6357aacc69e5ef61280b\",\"rct\":\"54f2c2c827dbbf80a131bb27ac9ab6724001eafb136bd5916c202236211bcfe4d9f68dcb00bfb3fb01e41c91f07d06016db9e420e7277a21cd72b7b52dab7a011f5b7a4c5abf0d9013b42e98b6f16f06c4f61dd8468089002e169058292b1e0a\"},{\"global_index\":613372,\"public_key\":\"e31fa7b495c6e2d1a12511a2833fbd224bb1c8463ecc15d4ba44cd32c6856103\",\"rct\":\"e1a9e52ec110a580f2a444f70efe24c019ca4dac8cafa0c8dca61d26bc0591f200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\"},{\"global_index\":630358,\"public_key\":\"8ab2e91d587142844107cb220dbddfa5f1c49f08c5144b637419165eb15b510c\",\"rct\":\"740969143db8beb3b1d886bfc6a534cfffddc4b4548192a7f9dd67730e1aa8f7c27cb174dd7de99007097c14798f323be74c5f456a38bdb873101998fe35c40f26d3f37113f6f868e0551b8ad53d8cea066ff1faae460efdea4b975e2369840e\"}]}]}";

BOOST_AUTO_TEST_CASE(bridge__transfers__send_stagenet_coinbase)
{
	using namespace serial_bridge;
	using namespace monero_transfer_utils;
	//
	// this being input as JSON merely for convenience
	boost::property_tree::ptree pt;
	stringstream ss;
	ss << OM_stagenet__unspent_outs_json;
	boost::property_tree::json_parser::read_json(ss, pt);
	boost::property_tree::ptree unspent_outs = pt.get_child("unspent_outs");
	//
	// NOTE: in the real algorithm you should re-request this _each time step2 must be called_
	// this being input as JSON merely for convenience
	boost::property_tree::ptree mix_outs;
	{
		boost::property_tree::ptree pt;
		stringstream ss;
		ss << OM_stagenet__rand_outs_json;
		boost::property_tree::json_parser::read_json(ss, pt);
		mix_outs = pt.get_child("mix_outs");
	}
	//
	// Send algorithm:
	bool tx_must_be_reconstructed = true; // for ease of writing this code, start this off true & structure whole thing as while loop
	optional<string> fee_actually_needed_string = none;
	size_t construction_attempt_n = 0;
	while (tx_must_be_reconstructed) {
		construction_attempt_n += 1; // merely kept for assertion purposes
		//
		optional<string> mixin_string;
		optional<string> change_amount_string;
		optional<string> using_fee_string;
		optional<string> final_total_wo_fee_string;
		boost::property_tree::ptree using_outs;
		{
			boost::property_tree::ptree root;
			root.put("is_sweeping", "false");
			root.put("payment_id_string", "d2f602b240fbe624"); // optl
			root.put("sending_amount", "1000000000000");
			root.put("fee_per_b", "166333");
			root.put("priority", "1");
			root.add_child("unspent_outs", unspent_outs);
			if (fee_actually_needed_string != none) {
				BOOST_REQUIRE(construction_attempt_n > 1);
				//
				// for next round's integration - if it needs to re-enter... arg "passedIn_attemptAt_fee"
				root.put("passedIn_attemptAt_fee", *fee_actually_needed_string);
			}
			auto ret_string = serial_bridge::send_step1__prepare_params_for_get_decoys(args_string_from_root(root));
			stringstream ret_stream;
			ret_stream << ret_string;
			boost::property_tree::ptree ret_tree;
			boost::property_tree::read_json(ret_stream, ret_tree);
			optional<uint32_t> err_code = ret_tree.get_optional<uint32_t>(ret_json_key__any__err_code());
			if (err_code != none && (CreateTransactionErrorCode)*err_code != monero_transfer_utils::noError) {
				if ((CreateTransactionErrorCode)*err_code == monero_transfer_utils::needMoreMoneyThanFound) {
					optional<string> spendable_balance_string = ret_tree.get_optional<string>(ret_json_key__send__spendable_balance());
					BOOST_REQUIRE(spendable_balance_string != none);
					BOOST_REQUIRE((*spendable_balance_string).size() > 0);
					//			uint64_t fee = stoull(*fee_string);
					//			BOOST_REQUIRE(fee == 135000000);
					cout << "bridge__transfers__send_stagenet_coinbase: step1: needMoreMoneyThanFound: spendable_balance " << *spendable_balance_string << endl;
					//
					optional<string> required_balance_string = ret_tree.get_optional<string>(ret_json_key__send__required_balance());
					BOOST_REQUIRE(required_balance_string != none);
					BOOST_REQUIRE((*required_balance_string).size() > 0);
					//			uint64_t fee = stoull(*fee_string);
					//			BOOST_REQUIRE(fee == 135000000);
					cout << "bridge__transfers__send_stagenet_coinbase: step1: needMoreMoneyThanFound: required_balance " << *required_balance_string << endl;
				}
				auto err_msg = err_msg_from_err_code__create_transaction((CreateTransactionErrorCode)*err_code);
				BOOST_REQUIRE_MESSAGE(false, err_msg);
				// ^--- not expecting anything like needMoreMoneyThanFound errors here
			}
			mixin_string = ret_tree.get_optional<string>(ret_json_key__send__mixin());
			BOOST_REQUIRE(mixin_string != none);
			BOOST_REQUIRE((*mixin_string).size() > 0);
			uint64_t mixin = stoull(*mixin_string);
			BOOST_REQUIRE(mixin == 10);
			cout << "bridge__transfers__send_stagenet_coinbase: mixin " << *mixin_string << endl;
			//
			using_fee_string = ret_tree.get_optional<string>(ret_json_key__send__using_fee()); // save this
			BOOST_REQUIRE(using_fee_string != none);
			BOOST_REQUIRE((*using_fee_string).size() > 0);
			//		uint64_t using_fee = stoull(*using_fee_string);
			//	BOOST_REQUIRE(using_fee == 10); // TODO:?
			cout << "bridge__transfers__send_stagenet_coinbase: step1: using_fee " << *using_fee_string << endl;
			//
			using_outs = ret_tree.get_child(ret_json_key__send__using_outs()); // save this for step2
			BOOST_FOREACH(boost::property_tree::ptree::value_type &output_desc, using_outs)
			{
				assert(output_desc.first.empty()); // array elements have no names
				cout << "bridge__transfers__send_stagenet_coinbase: step1: using_out " << output_desc.second.get<string>("public_key") << endl;
			}
			//
			change_amount_string = ret_tree.get_optional<string>(ret_json_key__send__change_amount());
			BOOST_REQUIRE(change_amount_string != none);
			BOOST_REQUIRE((*change_amount_string).size() > 0);
			//		uint64_t change_amount = stoull(*change_amount_string);
			//	BOOST_REQUIRE(change_amount == 10); // TODO:?
			cout << "bridge__transfers__send_stagenet_coinbase: step1: change_amount " << *change_amount_string << endl;
			//
			final_total_wo_fee_string = ret_tree.get_optional<string>(ret_json_key__send__final_total_wo_fee());
			BOOST_REQUIRE(final_total_wo_fee_string != none);
			BOOST_REQUIRE((*final_total_wo_fee_string).size() > 0);
			//		uint64_t final_total_wo_fee = stoull(*final_total_wo_fee_string);
			//	BOOST_REQUIRE(final_total_wo_fee == 10); // TODO:?
			cout << "bridge__transfers__send_stagenet_coinbase: step1: final_total_wo_fee " << *final_total_wo_fee_string << endl;
			//
		}
		{
			boost::property_tree::ptree root;
			root.put("final_total_wo_fee", *final_total_wo_fee_string);
			root.put("change_amount", *change_amount_string); // from step1
			root.put("fee_amount", *using_fee_string); // from step1
			root.add_child("using_outs", using_outs); // from step1
			//
			root.put("payment_id_string", "d2f602b240fbe624"); // optl
			root.put("nettype_string", string_from_nettype(STAGENET));
			root.put("to_address_string", "57Hx8QpLUSMjhgoCNkvJ2Ch91mVyxcffESCprnRPrtbphMCv8iGUEfCUJxrpUWUeWrS9vPWnFrnMmTwnFpSKJrSKNuaXc5q");
			root.put("from_address_string", "56bY2v2RJZNEvrKdYuwG73Q2idshQyGc5fV74BZqoVv72MPSBEakPbfWYtQH4PLGhk3uaCjNZ81XJ7o9pimAXzQFCv7bxxf");
			root.put("sec_viewKey_string", "9ef8e116d2c774b207a2dd6a234dab8f5d54becc04aa26ccbd6f1f67e8427308");
			root.put("sec_spendKey_string", "4acde2a96d5085423fcc8713c878448b35e45900f4e9cf2c0b643eb4268e140e");
			root.put("fee_per_b", "166333");
			root.put("unlock_time", "0");
			root.put("priority", "1");
			root.add_child("mix_outs", mix_outs);
			//
			boost::property_tree::ptree ret_tree;
			auto ret_string = serial_bridge::send_step2__try_create_transaction(args_string_from_root(root));
			stringstream ret_stream;
			ret_stream << ret_string;
			boost::property_tree::read_json(ret_stream, ret_tree);
			optional<uint32_t> err_code = ret_tree.get_optional<uint32_t>(ret_json_key__any__err_code());
			if (err_code != none && (CreateTransactionErrorCode)*err_code != monero_transfer_utils::noError) {
				auto err_msg = err_msg_from_err_code__create_transaction((CreateTransactionErrorCode)*err_code);
				BOOST_REQUIRE_MESSAGE(false, err_msg);
			}
			//
			tx_must_be_reconstructed = ret_tree.get<bool>(ret_json_key__send__tx_must_be_reconstructed());
			cout << "bridge__transfers__send_stagenet_coinbase: step2: must tx be reconstructed? " << tx_must_be_reconstructed << endl;
			if (tx_must_be_reconstructed) {
				// declared outside the for-loop so that we can use it for subsequent iterations if needed:
				fee_actually_needed_string = ret_tree.get_optional<string>(ret_json_key__send__fee_actually_needed());
				BOOST_REQUIRE(fee_actually_needed_string != none);
				BOOST_REQUIRE((*fee_actually_needed_string).size() > 0);
				//
				// This means we must go back through step1 to re-pick using_outs
				BOOST_REQUIRE(construction_attempt_n < 3); // not generally expecting to have to do this more than once or twice
				continue; // proceed to next iteration (re-enter tx construction at step1(II) with fee_actually_needed_string from step2(I))
			}
			optional<string> tx_hash = ret_tree.get_optional<string>(ret_json_key__send__tx_hash());
			optional<string> tx_key_string = ret_tree.get_optional<string>(ret_json_key__send__tx_key());
			optional<string> serialized_signed_tx = ret_tree.get_optional<string>(ret_json_key__send__serialized_signed_tx());
			BOOST_REQUIRE(serialized_signed_tx != none);
			BOOST_REQUIRE((*serialized_signed_tx).size() > 0);
			cout << "bridge__transfers__send_stagenet_coinbase: serialized_signed_tx: " << *serialized_signed_tx << endl;
			BOOST_REQUIRE(tx_hash != none);
			BOOST_REQUIRE((*tx_hash).size() > 0);
			cout << "bridge__transfers__send_stagenet_coinbase: tx_hash: " << *tx_hash << endl;
			BOOST_REQUIRE(tx_key_string != none);
			BOOST_REQUIRE((*tx_key_string).size() > 0);
			cout << "bridge__transfers__send_stagenet_coinbase: tx_key_string: " << *tx_key_string << endl;
		}
	}
}
