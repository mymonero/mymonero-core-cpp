//
//  monero_wallet_utils.hpp
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

#ifndef monero_wallet_utils_hpp
#define monero_wallet_utils_hpp

#include <boost/optional.hpp>
#include "serialization/binary_archive.h"
#include "cryptonote_basic.h"
#include "cryptonote_basic_impl.h"

#include "electrum-words.h"
#include "mnemonics/singleton.h"
#include "mnemonics/english.h"
//
using namespace tools;
#include "tools__ret_vals.hpp"
//
namespace monero_wallet_utils
{
	using namespace std;
	using namespace boost;
	using namespace tools;
	//
	// 16B keys
	POD_CLASS ec_nonscalar_16Byte {
		// extension to support old deprecated 16B/13-word seeds
		char data[16];
	};
	using legacy16B_secret_key = tools::scrubbed<ec_nonscalar_16Byte>;
	void coerce_valid_sec_key_from(
		const legacy16B_secret_key &legacy16B_mymonero_sec_seed,
		crypto::secret_key &dst__sec_seed
	);
	static_assert(sizeof(legacy16B_secret_key) == 16, "Invalid structure size");
	inline std::ostream &operator <<(std::ostream &o, const legacy16B_secret_key &v) {
		epee::to_hex::formatted(o, epee::as_byte_span(v)); return o;
	}
	const static legacy16B_secret_key null_legacy16B_skey = boost::value_initialized<legacy16B_secret_key>();
	const static unsigned long sec_seed_hex_string_length = sizeof(crypto::secret_key) * 2;
	const static unsigned long legacy16B__sec_seed_hex_string_length = sizeof(legacy16B_secret_key) * 2;
	//
	bool words_to_bytes(std::string words, legacy16B_secret_key& dst, std::string &language_name);
	bool bytes_to_words(const legacy16B_secret_key& src, std::string& words, const std::string &language_name);
 	//
	//
	// Accounts
	struct MnemonicDecodedSeed_RetVals: RetVals_base
	{
		optional<crypto::secret_key> optl__sec_seed = none;
		optional<string> optl__sec_seed_string = none;
		optional<string> optl__mnemonic_string = none;
		bool from_legacy16B_lw_seed = false;
	};
	bool decoded_seed(
		string mnemonic_string,
		string mnemonic_language,
		//
		MnemonicDecodedSeed_RetVals &retVals
	);
	//
	struct SeedDecodedMnemonic_RetVals: RetVals_base
	{
		optional<string> mnemonic_string = none;
	};
	SeedDecodedMnemonic_RetVals mnemonic_string_from_seed_hex_string(
		const string &seed_string,
		const string &wordsetName
	);
	//
	// Convenience functions - Wallets
	struct WalletDescription
	{
		string sec_seed_string; // as string bc it might by legacy 16B style aside from crypto::secret_key
		//
		string address_string;
		//
		crypto::secret_key sec_spendKey;
		crypto::secret_key sec_viewKey;
		crypto::public_key pub_spendKey;
		crypto::public_key pub_viewKey;
		//
		std::string mnemonic_string; // mnemonic_language is not returned because it must be provided to all functions which can return a WalletDescription
	};
	struct WalletDescriptionRetVals: RetVals_base
	{
		boost::optional<WalletDescription> optl__desc = boost::none;
	};
	bool new_wallet(
		const string &mnemonic_language,
		WalletDescriptionRetVals &retVals,
		cryptonote::network_type nettype = cryptonote::MAINNET
	);
	bool wallet_with(
		const string &mnemonic_string,
		const string &mnemonic_language,
		WalletDescriptionRetVals &retVals,
		cryptonote::network_type nettype = cryptonote::MAINNET
	);
	//
	struct WalletComponentsValidationResults: RetVals_base
	{
		bool isValid; // this will naturally remain false if did_error=true
		string pub_spendKey_string;
		string pub_viewKey_string;
		bool isInViewOnlyMode; // !sec_seed && !sec_spendKey
	};
	bool validate_wallet_components_with( // returns !did_error
		const string &address_string,
		const string &sec_viewKey_string,
		optional<string> sec_spendKey_string,
		optional<string> sec_seed_string,
		cryptonote::network_type nettype,
		WalletComponentsValidationResults &retVals
	);
}


#define MONEROWALLETUTILS_MAKE_COMPARABLE(type) \
namespace monero_wallet_utils { \
inline bool operator==(const type &_v1, const type &_v2) { \
return std::memcmp(&_v1, &_v2, sizeof(type)) == 0; \
} \
inline bool operator!=(const type &_v1, const type &_v2) { \
return std::memcmp(&_v1, &_v2, sizeof(type)) != 0; \
} \
}

#define MONEROWALLETUTILS_MAKE_HASHABLE(type) \
MONEROWALLETUTILS_MAKE_COMPARABLE(type) \
namespace monero_wallet_utils { \
static_assert(sizeof(std::size_t) <= sizeof(type), "Size of " #type " must be at least that of size_t"); \
inline std::size_t hash_value(const type &_v) { \
return reinterpret_cast<const std::size_t &>(_v); \
} \
} \
namespace std { \
template<> \
struct hash<monero_wallet_utils::type> { \
std::size_t operator()(const monero_wallet_utils::type &_v) const { \
return reinterpret_cast<const std::size_t &>(_v); \
} \
}; \
}

MONEROWALLETUTILS_MAKE_HASHABLE(legacy16B_secret_key)

#endif /* monero_wallet_utils_hpp */
