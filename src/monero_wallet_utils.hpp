//
//  monero_wallet_utils.hpp
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
#include "generic-ops.h" // mostly to get its includes
//
#include "misc_log_ex.h"
#include "wipeable_string.h"
#include "string_tools.h"
using namespace epee;
//
using namespace tools; // intentionally first
#include "tools__ret_vals.hpp"
//
namespace monero_wallet_utils
{
	using namespace std;
	using namespace boost;
	using namespace tools;
	using namespace crypto;
	using namespace cryptonote;
	//
	// 16B keys
	POD_CLASS ec_nonscalar_16Byte {
		// extension to support old deprecated 16B/13-word seeds
		char data[16];
	};
	using legacy16B_secret_key = tools::scrubbed<ec_nonscalar_16Byte>;
	void coerce_valid_sec_key_from(
		const legacy16B_secret_key &legacy16B_mymonero_sec_seed,
		secret_key &dst__sec_seed
	);
	static_assert(sizeof(legacy16B_secret_key) == 16, "Invalid structure size");
	inline std::ostream &operator <<(std::ostream &o, const legacy16B_secret_key &v) {
		epee::to_hex::formatted(o, epee::as_byte_span(v)); return o;
	}
	const static legacy16B_secret_key null_legacy16B_skey = boost::value_initialized<legacy16B_secret_key>();
	const static unsigned long sec_seed_hex_string_length = sizeof(secret_key) * 2;
	const static unsigned long legacy16B__sec_seed_hex_string_length = sizeof(legacy16B_secret_key) * 2;
	//
	bool words_to_bytes(const epee::wipeable_string &words, legacy16B_secret_key &dst, std::string &language_name);
	bool bytes_to_words(const legacy16B_secret_key& src, epee::wipeable_string &words, const std::string &language_name);
	//
	bool are_equal_mnemonics(const string &words_a, const string &words_b);
 	//
	//
	// Accounts
	struct MnemonicDecodedSeed_RetVals: RetVals_base
	{
		optional<secret_key> optl__sec_seed = none;
		optional<string> optl__sec_seed_string = none;
		optional<string> optl__mnemonic_string = none;
		optional<string> mnemonic_language = none;
		bool from_legacy16B_lw_seed = false;
	};
	bool decoded_seed(
		const epee::wipeable_string &mnemonic_string,
		//
		MnemonicDecodedSeed_RetVals &retVals
	);
	//
	struct SeedDecodedMnemonic_RetVals: RetVals_base
	{
		optional<epee::wipeable_string> mnemonic_string = none;
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
		secret_key sec_spendKey;
		secret_key sec_viewKey;
		public_key pub_spendKey;
		public_key pub_viewKey;
		//
		epee::wipeable_string mnemonic_string;
		string mnemonic_language;
	};
	struct WalletDescriptionRetVals: RetVals_base
	{
		optional<WalletDescription> optl__desc = boost::none;
	};
	inline bool is_prefixed_by(const string &haystack, const string &needle)
	{
		auto res = std::mismatch(needle.begin(), needle.end(), haystack.begin());
		//
		return res.first == needle.end();
	}
	inline optional<string> mnemonic_language_from_code(const string &language_code)
	{ // using is_prefixed_by so that e.g. en-US would match as en
		if (is_prefixed_by(language_code, "en")) {
			return string("English");
		} else if (is_prefixed_by(language_code, "nl")) { // Dutch language
			return string("Nederlands");
		} else if (is_prefixed_by(language_code, "fr")) {
			return string("Français");
		} else if (is_prefixed_by(language_code, "es")) {
			return string("Español");
		} else if (is_prefixed_by(language_code, "pt")) {
			return string("Português");
		} else if (is_prefixed_by(language_code, "ja")) {
			return string("日本語");
		} else if (is_prefixed_by(language_code, "it")) {
			return string("Italiano");
		} else if (is_prefixed_by(language_code, "de")) {
			return string("Deutsch");
		} else if (is_prefixed_by(language_code, "ru")) {
			return string("русский язык");
		} else if (is_prefixed_by(language_code, "zh")) { // Chinese language, simplified
			return string("简体中文 (中国)");
		} else if (is_prefixed_by(language_code, "eo") || is_prefixed_by(language_code, "epo")) {
			return string("Esperanto");
		} else if (is_prefixed_by(language_code, "jbo")) {
			return string("Lojban");
		} else {
			return none; // error .. possibly throw?
		}
	};
	bool convenience__new_wallet_with_language_code(
		const string &locale_language_code,
		WalletDescriptionRetVals &retVals,
		network_type nettype
	);
	bool new_wallet(
		const string &mnemonic_language,
		WalletDescriptionRetVals &retVals,
		network_type nettype
	);
	bool wallet_with(
		const string &mnemonic_string,
		WalletDescriptionRetVals &retVals,
		network_type nettype
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
		network_type nettype,
		WalletComponentsValidationResults &retVals
	);
	//
	struct ComponentsFromSeed
	{
		string address_string;
		//
		secret_key sec_spendKey;
		secret_key sec_viewKey;
		public_key pub_spendKey;
		public_key pub_viewKey;
	};
	struct ComponentsFromSeed_RetVals: RetVals_base
	{
		optional<ComponentsFromSeed> optl__val = boost::none;
	};
	bool address_and_keys_from_seed(
		const string &sec_seed_string,
		network_type nettype,
		ComponentsFromSeed_RetVals &retVals
	);
}


#define MWU__MAKE_COMPARABLE(type) \
namespace monero_wallet_utils { \
inline bool operator==(const type &_v1, const type &_v2) { \
return !memcmp(&_v1, &_v2, sizeof(_v1)); \
} \
inline bool operator!=(const type &_v1, const type &_v2) { \
return !operator==(_v1, _v2); \
} \
}

#define MWU__MAKE_COMPARABLE_CONSTANT_TIME(type) \
namespace monero_wallet_utils { \
inline bool operator==(const type &_v1, const type &_v2) { \
static_assert(sizeof(_v1) == 32, "constant time comparison is only implenmted for 32 bytes"); \
return crypto_verify_32((const unsigned char*)&_v1, (const unsigned char*)&_v2) == 0; \
} \
inline bool operator!=(const type &_v1, const type &_v2) { \
return !operator==(_v1, _v2); \
} \
}

#define MWU__DEFINE_HASH_FUNCTIONS(type) \
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

#define MWU__MAKE_HASHABLE(type) \
MWU__MAKE_COMPARABLE(type) \
MWU__DEFINE_HASH_FUNCTIONS(type)

#define MWU__MAKE_HASHABLE_CONSTANT_TIME(type) \
MWU__MAKE_COMPARABLE_CONSTANT_TIME(type) \
MWU__DEFINE_HASH_FUNCTIONS(type)


MWU__MAKE_HASHABLE(legacy16B_secret_key) // "constant time comparison is only implenmted for 32 bytes"

#endif /* monero_wallet_utils_hpp */
