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
	string to_address_string = "4L6Gcy9TAHqPVPMnqa5cPtJK25tr7maE7LrJe67vzumiCtWwjDBvYnHZr18wFexJpih71Mxsjv8b7EpQftpB9NjPaRYYBm62jmF59EWcj6";
	string amount_string = "1026830000";
	//
	boost::property_tree::ptree root;
	root.put("nettype_string", string_from_nettype(MAINNET));
	root.put("from_address_string", from_address_string);
	root.put("sec_viewKey_string", "7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104");
	root.put("sec_spendKey_string", "4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803");
	root.put("to_address_string", to_address_string);
	root.put("payment_id_string", "d2f602b240fbe624");
	root.put("sending_amount", amount_string);
	root.put("change_amount", "0");
	root.put("fee_amount", "2550170000");
	root.put("unlock_time", "0");
	//
	boost::property_tree::ptree outputs;
	{
		boost::property_tree::ptree out;
		out.put("amount", "330000000");
		out.put("public_key", "f628f516cce1408ebfca9bb3a144ca044dd3f2de19711cc42fa4bf296b6761ca");
		out.put("rct", "66fafd6d2a1d6e92e30ee7fae132bd24116c325fca4594225fdf6f40cebd39b386ec21d9bd200c6212027e79237ad5fc076caf5c35e625eb2eb10b82debf5900cd9b8805d566e1ae29f69672876921c8a0ce0c7c4d87cf52dbd483403f435c0c");
		out.put("global_index", "7440388");
		out.put("index", "1");
		out.put("tx_pub_key", "d067e192807d4374aa7e8f832a0702b2c6f35fa30a5fae43a9ecddc3fde755d6");
		outputs.push_back(std::make_pair("", out));
	}
	{
		boost::property_tree::ptree out;
		out.put("amount", "200000000");
		out.put("public_key", "e92465cd124e98b3741b93d00346374eaf1cf37d3670eb21a595047d0b12accb");
		out.put("rct", "9b59ae166065aef95ab4380e0827035ff5ad579a5b11e5bef305af5369e61d4a9b91bfb770cbb9ada6b704d118417724e2b5d8d208c13e4bb3af9a355f00500e48c770addbbc0fd595d0b1edf36b591dd58102b100140146e7d93371b01cce0e");
		out.put("global_index", "7460517");
		out.put("index", "0");
		out.put("tx_pub_key", "e6bea5eaaff6beedc54dc5d4d24883fc18537af58425d584dad6d21fa6e1f27f");
		outputs.push_back(std::make_pair("", out));
	}
	{
		boost::property_tree::ptree out;
		out.put("amount", "1000000000");
		out.put("public_key", "14db00825a260f432c9cf1bc52cc65965e76b5519b228bfbfed2ce2c3119e73b");
		out.put("rct", "f18df9fc7363f6d92c25716bb18e2428e4712732e2a67f24ae799406e5d1e90f739658d1831a13fd92d30db16a615624897513c795a62d29166756f5d8a4ca0ff3842a5578d04ab559f2440e4f5c9fc2dae4033d49fac5b57048d14a054fcc02");
		out.put("global_index", "7348866");
		out.put("index", "0");
		out.put("tx_pub_key", "e0d953678f4ddbb39fac96a739d3a1e7ffaebf5f2e5f69b6a064567eb3c053f5");
		outputs.push_back(std::make_pair("", out));
	}
	{
		boost::property_tree::ptree out;
		out.put("amount", "1337000000");
		out.put("public_key", "5f871fe678a9dcba8ef682e8f7362cea200f42696c7dbe03f902ca19173eb147");
		out.put("rct", "50b0100d12c4c8824b3f3a56b97feefa2b0829a3fdeefe19b7d6ebcf7b768450eb4112e86debb0d859da4c0c556c7720c5388eead19863229a3fc610c23093024d8bd6f881687518019c8cd7d61a40f3a7486546df4d318011eacd34f4b17b0f");
		out.put("global_index", "7499194");
		out.put("index", "1");
		out.put("tx_pub_key", "1009aecc5f53f75b0731e0ccb3fd37ba2808a5c645a2a1f56861f121c926e4b5");
		outputs.push_back(std::make_pair("", out));
	}
	{
		boost::property_tree::ptree out;
		out.put("amount", "710000000");
		out.put("public_key", "4b4c3fcc8ab73628f298dc75fa5976e1812cd21bc77f30bfa6bb2eb6e713d2f7");
		out.put("rct", "6b560a85f4b5ec7b4ccd4401717b84307e2b03c646340708028a6d97f41790e39f13da5d8ccd642d5693952a3c67b78ff297c8d718051c782b3f37ac6010cc069938fef32ffbe626e18685b7c13c17b53d3077f5bae52742daac2f29b6c9e708");
		out.put("global_index", "7440663");
		out.put("index", "0");
		out.put("tx_pub_key", "976feee1972bbd05d6ac7d52ccb30614143c31021b3941e28de2cbb1f48a8ddf");
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
			mix_out_output.put("global_index", "6022297");
			mix_out_output.put("public_key", "c0961a16132c2af839abc41d6b9f53fd828997d7deb0fbeb64fa203bcce35c29");
			mix_out_output.put("rct", "e8d57915f3b1d9440199e5bda9bcc6b214eee76a498ed983214f6187880062a4");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5844452");
			mix_out_output.put("public_key", "2b9d22d274dc47783b820778cf5baa65a4f444dada4d0b5797923eefeb265bf6");
			mix_out_output.put("rct", "72a9f57e82a884dd6494e8c816db5323bfeb18c1342f5712b7634afc9a4fa392");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "4517893");
			mix_out_output.put("public_key", "66048decc2ee6fb747fe81855caee828bd164e8c99aa69fe66e936bbd53f877c");
			mix_out_output.put("rct", "dcde26b254956195072da36c4defd7f2666014fea6a96db3498a29e28b26ad6d");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6680101");
			mix_out_output.put("public_key", "f319620061c4edb40976d32eab1e5dce7d12ad40bb2bea09e8ff4dde873bceaf");
			mix_out_output.put("rct", "419b1d691a32f7425eee817eae8fac314db1d01c4793b5e18523b34cbe666fcf");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6367881");
			mix_out_output.put("public_key", "cce4ab9a29a5c199da114a2ba6a1136e0e977024babc4b755651aea36e3bca56");
			mix_out_output.put("rct", "c701a4d8f1b46c0f852ee4806f097a732a1f6250fe1c972e7a797a1a76c13f91");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "4012934");
			mix_out_output.put("public_key", "593be9a93d96a7ffa35b71cf8ff86fa4742bef30e97240edcbeb8dbd692c6744");
			mix_out_output.put("rct", "9b7f8c556e7ef5055c69fc8130d8a06ae06893d7430bcd0ce6cbbbb3fda743ca");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "3658900");
			mix_out_output.put("public_key", "52d33ef82fe443eca4451debe75d02277e735c9755e1eecda7b8ec314d98207a");
			mix_out_output.put("rct", "25b2f9e08f25defa09d9b4184cfd9f052920939b335942b7a21b5b937e712c86");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6271270");
			mix_out_output.put("public_key", "676978466ebeeb3e37f361b97e47531219b24c796cd7a336fa760358f57dd8b9");
			mix_out_output.put("rct", "2fd25fa0a5499132c0e84957e71c0ac968292cdcc2fbd14decc81c64423cba54");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6737337");
			mix_out_output.put("public_key", "bff30dd5754ab7c6c5ff5beac28deb9ce8e3a52806b9534732ef2d463cb19042");
			mix_out_output.put("rct", "a55b847032bbbe23ef54e73a7d0cae37533c260fb308ea7a735d49e5ee24eb11");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "2853615");
			mix_out_output.put("public_key", "d122c1bf187a0bba720c2662e9bf3801f7d9e833de4c0b0fd93c2c10cb8e8274");
			mix_out_output.put("rct", "aacc7e6c7032370ca4d738ae0b4b4082f10c0e8f0f4c84490793ad90f6e261ba");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6988828");
			mix_out_output.put("public_key", "f14eb3ea0127908704e8b4437644106ad567172cf04ba415d93c473d163aacd4");
			mix_out_output.put("rct", "950b40e077358482e2baa20d814998a26d9ff05c27af4b4d3ca6da1aa1b0687f");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}			
		mix_out.add_child("outputs", mix_out_outputs);
		mix_outs.push_back(std::make_pair("", mix_out));
	}
	{
		boost::property_tree::ptree mix_out;
		mix_out.put("amount", "0");
		boost::property_tree::ptree mix_out_outputs;
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5593429");
			mix_out_output.put("public_key", "65dcd81a05df5f6638f144aefd87805c10418b1a6a66e4b86f0d7cf624ee524b");
			mix_out_output.put("rct", "18b3d2851580a9466104c6e3345b252a58af0565d32e0973084abd49d5b995d6");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "2694114");
			mix_out_output.put("public_key", "945e10059f5096368a5b59df0501199053be2701d59d17d1c417f185b911db50");
			mix_out_output.put("rct", "93ca380debeb12079a4a75df2f155c5b22b91a994739e6794c3e883ca735439f");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "3588742");
			mix_out_output.put("public_key", "227f77475495bbffb2233516eb3330d4c3abc8a47c4935cd6fd2618b6df655de");
			mix_out_output.put("rct", "9c2d958dd948403a5dcb54db8db9ec958f07409e20c6389cb481975097bb4e3f");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6599417");
			mix_out_output.put("public_key", "ed819b935266f190a38bc7d14359f852c084b3fc683faca6cca9bc52f7ad57a9");
			mix_out_output.put("rct", "e112009aca4ee5d002e7a70a73da6093b05eb64569b1db0f13725b475c0e6100");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "2836715");
			mix_out_output.put("public_key", "60c37b65a6948241168db496f175df6efe12693eb7f854922093d5f4aa6822b6");
			mix_out_output.put("rct", "221a566f64faec25a6c1b7748353c12f84ed766e3346be60c84985a52fc1a9af");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "293661");
			mix_out_output.put("public_key", "26a971d54288152a77822e3041b2b6637b8a3b421683baa34de9d28910e473b1");
			mix_out_output.put("rct", "ac32d38b0fe3797ee8cb0b1a1e613469ec099723304b683d9c4038bcdfe1d9b3");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "3772025");
			mix_out_output.put("public_key", "de0a55a60cf58eaa39131bd98e60170abad6de0f6d605d8645db655462da17f0");
			mix_out_output.put("rct", "1dfdf954ee0239adc4a57daf75cfa7f3ff601efc4a16c4af70036f33e663789b");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5685659");
			mix_out_output.put("public_key", "bb669c043cdfb9827dc96e8f0016c33ca1099eefa6a065b6cc062eac539df492");
			mix_out_output.put("rct", "fff3e0762ff14ac9281e7c5ae9de42a2d7b83848331fc0a07315a42412482f5c");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "4436753");
			mix_out_output.put("public_key", "d2f349d469c62323f73501357a09219031e19b5580b7375a5ff303ead3b7bdda");
			mix_out_output.put("rct", "f531214063a18eed0dafa7497da165c67d7af11bb2fe225ed05b215c29bfb468");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5725205");
			mix_out_output.put("public_key", "6dc84dfdf3227f619b3017171beed53da8ea463bca1f270c929575addee0e428");
			mix_out_output.put("rct", "47179835b5bf8ab50cfe36b2aec373b91b8119c56fbe73ba142dba5282abb6a9");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6328492");
			mix_out_output.put("public_key", "d373bc1a510934ab1d1cab891f5ddb3490d6a3f4fa8f71cf90ca0b5f9583625c");
			mix_out_output.put("rct", "d08b9958636dacd9cc470e19c40e34855623168053fd6f03d0f05a51a2e2b616");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		mix_out.add_child("outputs", mix_out_outputs);
		mix_outs.push_back(std::make_pair("", mix_out));
	}
	{
		boost::property_tree::ptree mix_out;
		mix_out.put("amount", "0");
		boost::property_tree::ptree mix_out_outputs;
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "1472497");
			mix_out_output.put("public_key", "f52425f85472b0b733f1926ed505d12136bf144763265ac3fe1729746d1819b1");
			mix_out_output.put("rct", "82080aed958e8501c4205753f1b0410a5cdb357afbd00fc5db0457b2b6f04829");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "4114941");
			mix_out_output.put("public_key", "b736816eed46887c1f2fbde7b7be061b1b05cef9b700e9b9d81e48a922de7ff7");
			mix_out_output.put("rct", "e626ecae8d0c05ce7b27250d769d8588734f11b0abde348778b53bea5c9c6136");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6467586");
			mix_out_output.put("public_key", "56cdce2f6fe0472eb41beb8952ad3e1d0a850f8d9a000c19eafff8fdb99a5ffc");
			mix_out_output.put("rct", "a0cb89d5071b4470add8b1e1d9fd8b580a566371cfff68b3c581c10273067e59");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "2092106");
			mix_out_output.put("public_key", "3b727da9cede03d84c67efddd732ff0ff0df00f9b30dc0ef9edb398a2a1aa485");
			mix_out_output.put("rct", "eab9cf089147ea5eb2ce72b0412af6e37dc2457c154b98bfc4872d0eb89025b8");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5311080");
			mix_out_output.put("public_key", "61653c30bc414e61b63802c7b8d03eed7deeec46a43cb551b0758c4aabfc737d");
			mix_out_output.put("rct", "ff6f6268694861466c853b4ced591ff29e83f37a058d0e0fc5977e213c431d29");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "4213697");
			mix_out_output.put("public_key", "9e3dae8a108c9f72ec250e2de1f0820ff1903c5d65e19fb96fb1b53a7908bf18");
			mix_out_output.put("rct", "1209e3c83e761660956db24def7573825bc70890163e921aabf1b32cabf5e66c");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5038411");
			mix_out_output.put("public_key", "669d3298f64b9c511e39a77b23d802f8f415de0a383c215a25a5973c35529606");
			mix_out_output.put("rct", "0d7f4ed0d50e4e45d838dc76fac13e6377f8e37f8054fdd9a7ebdae9e31f88e0");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5416381");
			mix_out_output.put("public_key", "fbcf98f4d25b7aca7c92c554d51a1270dd3794a5de32297acee7a41ba753a71f");
			mix_out_output.put("rct", "c3f666fc7b99983d906db6f50f0bb4043abf64faaa94ee89c196125327bdbb15");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "3124362");
			mix_out_output.put("public_key", "0a062df5f30bf16384c4c840df9cb76bd25d37baaf2250c0a51593c69bbf332a");
			mix_out_output.put("rct", "addb3000d7da14d2dcba39459ccba013c26c0c558a3b9abff80e3d33bb4e7057");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "7424088");
			mix_out_output.put("public_key", "094130eef4b50f62510631fdc1cbdf3608a76267eafa3488c5d748b410a1aced");
			mix_out_output.put("rct", "db70b06b7f24a85cc330d0376d0d6be7ffef7994f8f61e9a67f516cdcb6c6045");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5878908");
			mix_out_output.put("public_key", "3a51d8cd4a985504e7cc5165923f24be258d3155b5fdb7a83241eda2b89dd593");
			mix_out_output.put("rct", "06324483a8212c1f4ab771be9c763dd9eb1a7233c08a9fbbd6abd08e691aac57");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		mix_out.add_child("outputs", mix_out_outputs);
		mix_outs.push_back(std::make_pair("", mix_out));
	}
	{
		boost::property_tree::ptree mix_out;
		mix_out.put("amount", "0");
		boost::property_tree::ptree mix_out_outputs;
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6479541");
			mix_out_output.put("public_key", "b8e9d322417663c0245ae172cadc58f249d61169f6ae0ae7b831cb6f62cdfb85");
			mix_out_output.put("rct", "f2a4ac6c5ff9670158486c03418a5f312bdbe84222f5e89385a51363bab7bdaa");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6104091");
			mix_out_output.put("public_key", "9fc9e838438ababa62835fda758ae9c55ea43c8da1fa94213b4c77b2622b6df5");
			mix_out_output.put("rct", "9341b536f88275932cd44b06373434d39707ab3ba5f1fbdddc02544c2f8bcde7");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "7216082");
			mix_out_output.put("public_key", "387c093f78ac0d734b7af04d05055e11f6c9dc7536a1dfa2ccadd60c79693960");
			mix_out_output.put("rct", "4244f9aec800e371c7c2c7a663108bf2d0b8eb6d59ba08c32040fba2eca80be8");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "4364145");
			mix_out_output.put("public_key", "d99460fd75c8414f565db04ff11b0d3cf6898efef35c528c624a9662f3d49996");
			mix_out_output.put("rct", "98bc320bb539668969eec42a944c429b36ec7e61b8935e4e9c41192b3d39c32f");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "3591861");
			mix_out_output.put("public_key", "543b3eafd209344a02ad8a60035322be2e796a4d718926330faa9adcac2697df");
			mix_out_output.put("rct", "edae851e4e4faeef79f32e582cccb72c89af6636b56cf966f8e670c4e97a50cb");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "2010861");
			mix_out_output.put("public_key", "c748b1f34c16a23943884672b155a0224faab8c9779bd2a244b1480da226637f");
			mix_out_output.put("rct", "c01f86c86759a2e4feb80cef76f16a5fffd61b78d612c04388a1cf36ee71de78");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "2120332");
			mix_out_output.put("public_key", "1d57d35f0b87441599b9c48d292098fb199e962566dfc51a0a3ca6061757ec84");
			mix_out_output.put("rct", "40ff33c909f1216e30dfa723c5518def95da6b8db566cd099a8b6e4e19c1e022");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6647347");
			mix_out_output.put("public_key", "934f700ab34a84e6e72ed30f7f7d54b9b346f4bb6e6d3a74ea21977f293e7167");
			mix_out_output.put("rct", "9734fe3e0face374bcf73a4ad9d25306106af490a9790343718e2c7ad20c1c60");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "7216401");
			mix_out_output.put("public_key", "855707ad257220dcc3bfeeee2dc8efc4ab9e62c07a3f38b4a4ed535db86497cc");
			mix_out_output.put("rct", "9f9d585c34594b4a3e268eb4cbfa45f1e542d233f1655e432ff03055667189d1");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5170016");
			mix_out_output.put("public_key", "56f6c97015849075c8a4831feba5dcc7989f735edde9e6181613e62b3b4bfa1c");
			mix_out_output.put("rct", "37d189c9576596ab3a525901eb0065fffbe3cecccc35337ef1f441d61ed187d5");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6993012");
			mix_out_output.put("public_key", "dfba264e3a2f43c4b6597eccfaafa768dc3903c0aae0b19d55e42f9ffd1089f0");
			mix_out_output.put("rct", "f37edf74d0252af96d73d2ff3881f11352dbd585dc9f7fb80e95b39a33059057");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		mix_out.add_child("outputs", mix_out_outputs);
		mix_outs.push_back(std::make_pair("", mix_out));
	}
	{
		boost::property_tree::ptree mix_out;
		mix_out.put("amount", "0");
		boost::property_tree::ptree mix_out_outputs;
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5657940");
			mix_out_output.put("public_key", "b20f3aefea422d4abb5630a4f0b0edd890b1c1c3a6a206ece616932df0f4a38a");
			mix_out_output.put("rct", "cda43d468ae84656b1a5fa05bde97fe48cf9ab360c73c2194882b525d4cdd49d");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "7439656");
			mix_out_output.put("public_key", "2765f455e2453084e922946a3959c1972317cf72003aebee9f1e2a0f9602f10f");
			mix_out_output.put("rct", "51373d766c61a16a0aa87da436b907392b8fcbcd89fe62a2f512f3a5dddf3180");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5931827");
			mix_out_output.put("public_key", "54a71ff9b44ccfeaaa06ef7a8876bacd77cfb2d33083444f44c3621c6b0e9273");
			mix_out_output.put("rct", "116b1f52b0c1837118f91c20b3c67fc12bff591b0261a192fe80213642ce4c80");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6976496");
			mix_out_output.put("public_key", "2ea7c464e6f725fb3f1bec5e771cea42d6407c9613bb5a0858c0ffce9c3fc587");
			mix_out_output.put("rct", "c11e79051291fb33dbd2e7126a79cbdaf76840075243520e25dd4ce7bf23c079");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6801722");
			mix_out_output.put("public_key", "0352533ce5bc4bfaff6a406374921163d353035b24d9fdb26878537508430461");
			mix_out_output.put("rct", "94bcab060f1fe54f2e96daceb982fc542a3579fdf4508c0cb03de49e59e34ffc");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "7008591");
			mix_out_output.put("public_key", "62ef63653d5e195ee635ff41738fd6c7c98120199c7ebdb7748718e8da013114");
			mix_out_output.put("rct", "41632747629f85f497d59df18d93c53ffed0900033e351fa033e2384c07b256b");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5901945");
			mix_out_output.put("public_key", "de8852684cf5fcc95874544f74c454941da0abb8573cb1ce869c7fe2a3011845");
			mix_out_output.put("rct", "46680c09305e48b4d97df127274fbb03481065080921f5f9c6efa260d8bf1973");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6137605");
			mix_out_output.put("public_key", "70e57222f46ea5ed95a840f66872e42c651fec12eb7bf9ccfa15043152eec1a9");
			mix_out_output.put("rct", "66d2aa8e721a8c4114a68f93193e85930970ce501cde42523905755c1bc46d0b");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "4489250");
			mix_out_output.put("public_key", "4f7cdd2b88421f3869e5539ea492e3f43bb552f6a8c877f1626ec2fe10659cbc");
			mix_out_output.put("rct", "2c1281cd0018325498d7a72778bfe840309b34ba743418b251d4181a0e7c9825");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "5900487");
			mix_out_output.put("public_key", "9c9937b31202ed6d6b030ecd4f923ecad0ef132ad5a808b5c61008b88ccb9dad");
			mix_out_output.put("rct", "ad11ddcc44a83d3acdd02c7db7313d939ba71437ce91dfcdf4e3107618d5e58b");
			mix_out_outputs.push_back(std::make_pair("", mix_out_output));
		}
		{
			boost::property_tree::ptree mix_out_output;
			mix_out_output.put("global_index", "6117071");
			mix_out_output.put("public_key", "8517e656a1e53d4d0a6265cefd71cc48cbab59303695bb5f5007bef635c522fc");
			mix_out_output.put("rct", "fc53cdb49f07621b85bfd0997cd54ab2f83fa70ca73c7962f2b42e05361b832e");
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
	BOOST_REQUIRE(fee == 144000000); // with bulletproofs on
	cout << "bridged__estimated_tx_network_fee: " << fee << endl;
}

BOOST_AUTO_TEST_CASE(bridged__estimate_rct_tx_size)
{
	using namespace serial_bridge;
	//
	bool use_bulletproofs = monero_fork_rules::lightwallet_hardeded__use_bulletproofs();
	//
	boost::property_tree::ptree root;
	root.put("n_inputs", 2);
	root.put("mixin", monero_transfer_utils::fixed_mixinsize());
	root.put("n_outputs", 2);
	std::vector<uint8_t> extra; // blank extra
	root.put("extra_size", extra.size());
	root.put("bulletproof", use_bulletproofs);
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
	BOOST_REQUIRE(use_bulletproofs == true);
	BOOST_REQUIRE(size == 3288);
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
