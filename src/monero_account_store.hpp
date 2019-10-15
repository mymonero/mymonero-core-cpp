//
//  monero_account_store.hpp
//  MyMonero
//
//  Created by Paul Shapiro on 10/5/19.
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
#ifndef monero_account_store_hpp
#define monero_account_store_hpp
//
#include <unordered_map>
//
#include "crypto.h"
#include "cryptonote_basic.h"
#include "cryptonote_basic/account.h"
#include "cryptonote_basic_impl.h"
#include "monero_wallet_utils.hpp"
//
using namespace tools;
#include "tools__ret_vals.hpp"
//
namespace monero_account_store
{
	using namespace std;
	using namespace cryptonote;
	using namespace monero_wallet_utils;
	//
	struct AccountStore
	{
		//
	public:
		//
		//
		static AccountStore *shared();
		//
		// Instance - Interface - Accessors
		boost::optional<std::shared_ptr<account_base>> stored_account(const string &name_string)
		{
			std::unordered_map<string, std::shared_ptr<account_base>>::const_iterator it = _account_ptrs_by_name.find(name_string);
			if (it == _account_ptrs_by_name.end()) { // does not exist
				return boost::none;
			}
			return _account_ptrs_by_name[name_string];
		}
		//
		// Instance - Interface - Imperatives
		bool store_account(const string &name_string, std::shared_ptr<account_base> account_ptr)
		{
			auto it = _account_ptrs_by_name.find(name_string);
			if (it != _account_ptrs_by_name.end()) {
				return false; // already stored
			}
			_account_ptrs_by_name[name_string] = account_ptr;
			return true;
		}
		bool unstore_account(const string &name_string)
		{
			auto it = _account_ptrs_by_name.find(name_string);
			if (it == _account_ptrs_by_name.end()) {
				return false; // not present
			}
			_account_ptrs_by_name.erase(it);
			return true;
		}
		//
		enum StoredAccount_ErrCode
		{
			alreadyExists 									= 1,
			didntExistButStoreAccountFailed 				= 2,
			cantDecodeMnemonic 								= 3,
			cantConvertSeedToMnemonic 						= 4,
			invalidAddress 									= 5,
			cantOpenAccountWithSubaddress 					= 6,
			invalidViewKey 									= 7,
			invalidSpendKey 								= 8,
			//
			hwDevSetup_couldntSetName						= 9,
			hwDevSetup_couldntInit							= 10,
			hwDevSetup_couldntConnect						= 11,
			hwDevSetup_ptrStillNULL							= 12, // TODO: rename this
			//
			hwDevPostInit_cantGetDeviceAddress				= 20,
			hwDevPostInit_devAddrDoesntMatchAccount			= 21
		};
		//
		//
		typedef std::function<hw::device *(const string &device_name, const string &device_type)> hwdevice_alloc_fn_type;
		struct HWDevInitParams
		{
			hwdevice_alloc_fn_type alloc_fn; // just initialize and return device, with callbacks set if desired (to approximate metaprogramming)
			string name; // aka device_name aka device_descriptor
			string type; // pass 'default'
			network_type nettype;
			boost::optional<string> derivation_path;

		};
		struct NewStoredAccountParams
		{
			const string &name_string;
			boost::optional<HWDevInitParams> optl__hwdev_init_params;
		};
		struct StoredAccountRetVals
		{
			boost::optional<StoredAccount_ErrCode> err_code;
			boost::optional<std::shared_ptr<account_base>> account_ptr = none;
		};
		struct NewAccountPtrWHWDevRetVals
		{
			boost::optional<StoredAccount_ErrCode> err_code;
			boost::optional<bool> requires_verify;
			boost::optional<std::shared_ptr<cryptonote::account_base>> account_ptr;
		};
		//
		bool __givenLocked_new_account_ptr_w_hwdev_unless_exists( // NOTE: If retVals.requires_verify == true, be sure to call _postinit_hwdev_state_validation_code after setting up the account (create_with_*) after calling this function
			const NewStoredAccountParams &params,
			NewAccountPtrWHWDevRetVals &retVals
		) {
			retVals = {};
			//
			auto it = _account_ptrs_by_name.find(params.name_string);
			if (it != _account_ptrs_by_name.end()) {
				retVals.err_code = alreadyExists;
				return false; // already exists, so return 'none'
			}
			hw::device *hwdev_ptr = NULL; // this will get set
			std::shared_ptr<cryptonote::account_base> account_ptr = std::make_shared<cryptonote::account_base>();
			if (params.optl__hwdev_init_params != none) { // if none is specified, software (default) would be used
				retVals.requires_verify = true; // this must be set so that consumers know to verify
				string device_descriptor = (*params.optl__hwdev_init_params).name; // this could be a device descriptor path
				hwdev_ptr = (*params.optl__hwdev_init_params).alloc_fn(
					device_descriptor,
					(*params.optl__hwdev_init_params).type
				); // Note: this alloc is put on the consumer so this file doesn't have to know what devices have support compiled in - and note; the alloc_fn will *not* create a new device if the hwdevice registry already has a device at that device_descriptor
				{ // Device: Fallback registration implementation 
					try { // determine registration status of device
						hw::get_device(device_descriptor); // throws; discarding the result
					} catch (const std::exception &e) { // it wasn't found, so register it in case it wasnt actually done yet
						hw::register_device(device_descriptor, hwdev_ptr);
					}
				}
				{ // Device: Common config
					hwdev_ptr->set_network_type((*params.optl__hwdev_init_params).nettype); // network type must be set before any address-related operations
					if (!(hwdev_ptr->set_name(device_descriptor))) {
						retVals.err_code = hwDevSetup_couldntSetName;
						return false;
					}
					if ((*params.optl__hwdev_init_params).derivation_path != none) {
						hwdev_ptr->set_derivation_path(*(*params.optl__hwdev_init_params).derivation_path);
					}
				}
				{ // Device: if it requires a connect()
					bool is_key_on_device = (*params.optl__hwdev_init_params).type != "software";
					if (is_key_on_device) {
						if (!hwdev_ptr->init()) {
							retVals.err_code = hwDevSetup_couldntInit;
							return false;
						}
						if (!hwdev_ptr->connect()) {
							retVals.err_code = hwDevSetup_couldntConnect;
							return false;
						}
					}
				}
				// Note: the original wallet code tended to compare account and device state here - but since the account isn't initialized yet, we'll leave that to consumers of this method
				// Note: Device can be considered unconfirmed-initialized here
			} else {
				// must set fallback:
				hwdev_ptr = &(hw::get_device("default"));
			}
			{ // In all cases: associate a device with the account so consumers can rely on it being there
				if (hwdev_ptr == NULL) {
					retVals.err_code = hwDevSetup_ptrStillNULL;
					return false;
				}
				account_ptr->set_device(*hwdev_ptr);
			}
			retVals.account_ptr = account_ptr;
			//
			return true;
		}
		boost::optional<StoredAccount_ErrCode> _postinit_hwdev_state_validation_code(std::shared_ptr<account_base> account_ptr)
		{ // this function may only return one of the StoredAccount_ErrCodes which starts with "hwDevPostInit_"
			account_public_address device_account_public_address;
			if (!account_ptr->get_device().get_public_address(device_account_public_address)) {
				return hwDevPostInit_cantGetDeviceAddress;
			}
			if (device_account_public_address != account_ptr->get_keys().m_account_address) {
				return hwDevPostInit_devAddrDoesntMatchAccount;
			}
			return boost::none;
		}
		//
		bool new_stored_account_with_keys(
			const NewStoredAccountParams &params,
			const cryptonote::account_public_address &address,
			const crypto::secret_key &sec_viewKey,
			const crypto::secret_key &sec_spendKey,
			StoredAccountRetVals &retVals
		) {
//			retVals = {}; // _givenLocked_new_stored_account_with_keys will do this
			store_mutex.lock();
			bool r = _givenLocked_new_stored_account_with_keys(
				params,
				address,
				sec_viewKey,
				sec_spendKey,
				retVals
			);
			store_mutex.unlock();
			return r;
		}
		bool _givenLocked_new_stored_account_with_keys(
			const NewStoredAccountParams &params,
			const cryptonote::account_public_address &address,
			const crypto::secret_key &sec_viewKey,
			const crypto::secret_key &sec_spendKey,
			StoredAccountRetVals &retVals
		) {
			retVals = {};
			//
			NewAccountPtrWHWDevRetVals newPtr_retVals;
			if (!__givenLocked_new_account_ptr_w_hwdev_unless_exists(params, newPtr_retVals)) {
				retVals.err_code = *newPtr_retVals.err_code;
				return false;
			}
			retVals.account_ptr = *newPtr_retVals.account_ptr; // copy existent ptr
			//
			(*retVals.account_ptr)->create_from_keys(address, sec_spendKey, sec_viewKey);
			if (newPtr_retVals.requires_verify != none && *newPtr_retVals.requires_verify == true) {
				boost::optional<StoredAccount_ErrCode> optl__validation_code = _postinit_hwdev_state_validation_code(*retVals.account_ptr);
				if (optl__validation_code != none) { // this check must be done, and after the account is set up
					retVals.err_code = *optl__validation_code;
					return false;
				}
			}
			if (!store_account(params.name_string, *retVals.account_ptr)) {
				retVals.err_code = didntExistButStoreAccountFailed;
				return false;
			}
			return true;
		}
		bool new_stored_account_with_key_strings( // convenience
			const NewStoredAccountParams &params,
			const string &address_string,
			const string &sec_viewKey_string,
			const string &sec_spendKey_string,
			network_type nettype,
			StoredAccountRetVals &retVals
		) {
			store_mutex.lock();
			retVals = {}; // _givenLocked_new_stored_account_with_keys will do this too but we do it here in case an err must be returned
			//
			cryptonote::address_parse_info decoded_address_info;
			bool r = cryptonote::get_account_address_from_str(
				decoded_address_info,
				nettype,
				address_string
			);
			if (!r) {
				retVals.err_code = invalidAddress;
				store_mutex.unlock(); // critical
				return false;
			}
			if (decoded_address_info.is_subaddress) {
				retVals.err_code = cantOpenAccountWithSubaddress;
				store_mutex.unlock(); // critical
				return false;
			}
			crypto::secret_key sec_viewKey;
			r = string_tools::hex_to_pod(sec_viewKey_string, sec_viewKey);
			if (!r) {
				retVals.err_code = invalidViewKey;
				store_mutex.unlock(); // critical
				return false;
			}
			crypto::secret_key sec_spendKey;
			r = string_tools::hex_to_pod(sec_spendKey_string, sec_spendKey);
			if (!r) {
				retVals.err_code = invalidSpendKey;
				store_mutex.unlock(); // critical
				return false;
			}
			bool final_r = _givenLocked_new_stored_account_with_keys(
				params,
				decoded_address_info.address,
				sec_viewKey,
				sec_spendKey,
				retVals
			);
			store_mutex.unlock(); // critical
			return final_r;
		}
		bool new_stored_account_with_mnemonic(
			const NewStoredAccountParams &params,
			const string &mnemonic_string,
			StoredAccountRetVals &retVals
		) {
			store_mutex.lock();
			bool r = _givenLocked_new_stored_account_with_mnemonic(
				params, mnemonic_string, retVals
			);
			store_mutex.unlock();
			return r;
		}
		bool _givenLocked_new_stored_account_with_mnemonic( // NOTE: given locked, do not manage lock within this function at all
			const NewStoredAccountParams &params,
			const string &mnemonic_string,
			StoredAccountRetVals &retVals
		) {
			retVals = {};
			//
			NewAccountPtrWHWDevRetVals newPtr_retVals;
			if (!__givenLocked_new_account_ptr_w_hwdev_unless_exists(params, newPtr_retVals)) {
				retVals.err_code = *newPtr_retVals.err_code;
				return false;
			}
			retVals.account_ptr = *newPtr_retVals.account_ptr; // copy existent ptr
			//
			WalletDescriptionRetVals walletDesc_retVals;
			MnemonicDecodedSeed_RetVals decodedSeed_retVals;
			bool r = account_with(mnemonic_string, walletDesc_retVals, **retVals.account_ptr, decodedSeed_retVals);
			if (!r) { // retVals has already been populated by account_with, so just return
				return false;
			}
			if (newPtr_retVals.requires_verify != none && *newPtr_retVals.requires_verify == true) {
				boost::optional<StoredAccount_ErrCode> optl__validation_code = _postinit_hwdev_state_validation_code(*retVals.account_ptr);
				if (optl__validation_code != none) { // this check must be done, and after the account is set up
					retVals.err_code = *optl__validation_code;
					return false;
				}
			}
			if (!store_account(params.name_string, *retVals.account_ptr)) {
				retVals.err_code = didntExistButStoreAccountFailed;
				return false;
			}
			//
			return true;
		}
		bool new_stored_account_with_seed( // convenience
			const NewStoredAccountParams &params,
			const string &seed_string,
			const string &wordset_name,
			StoredAccountRetVals &retVals
		) {
			store_mutex.lock(); // we must do this here
			retVals = {}; // _givenLocked_new_stored_account_with_mnemonic will do this too but we do it here in case an err must be returned
			//
			monero_wallet_utils::SeedDecodedMnemonic_RetVals retVals_conv = monero_wallet_utils::mnemonic_string_from_seed_hex_string(
				seed_string,
				wordset_name
			);
			if (retVals_conv.err_string != none) {
				retVals.err_code = cantConvertSeedToMnemonic;
				store_mutex.unlock(); // critical
				return false;
			}
			bool r = _givenLocked_new_stored_account_with_mnemonic(
				params,
				std::string((*(retVals_conv.mnemonic_string)).data(), (*(retVals_conv.mnemonic_string)).size()), // TODO: potentially unnecessary copy
				retVals
			);
			store_mutex.unlock(); // critical
			return r;
		}
		bool new_stored_account_with_keys_on_device(
			const NewStoredAccountParams &params,
			network_type nettype,
			StoredAccountRetVals &retVals
		) {
			store_mutex.lock();
			retVals = {};
			//
			NewAccountPtrWHWDevRetVals newPtr_retVals;
			if (!__givenLocked_new_account_ptr_w_hwdev_unless_exists(params, newPtr_retVals)) {
				retVals.err_code = *newPtr_retVals.err_code;
				store_mutex.unlock(); // critical
				return false;
			}
			retVals.account_ptr = *newPtr_retVals.account_ptr; // copy existent ptr
			//
			WalletDescriptionRetVals walletDesc_retVals;
			MnemonicDecodedSeed_RetVals decodedSeed_retVals;
			(*retVals.account_ptr)->create_from_device(
				(*retVals.account_ptr)->get_device()
			); // here we rely upon __givenLocked_new_account_ptr_w_hwdev_unless_exists having associated a hwdevice with the account… then we just tell the account to set up with it
			if (newPtr_retVals.requires_verify != none && *newPtr_retVals.requires_verify == true) {
				boost::optional<StoredAccount_ErrCode> optl__validation_code = _postinit_hwdev_state_validation_code(*retVals.account_ptr);
				if (optl__validation_code != none) { // this check must be done, and after the account is set up
					retVals.err_code = *optl__validation_code;
					store_mutex.unlock(); // critical
					return false;
				}
			}
			if (!store_account(params.name_string, *retVals.account_ptr)) {
				retVals.err_code = didntExistButStoreAccountFailed;
				store_mutex.unlock(); // critical
				return false;
			}
			//
			store_mutex.unlock(); // critical
			return true;
		}
		//
	private:
		std::mutex store_mutex;
		//
		static AccountStore *instance;
		AccountStore() {}
		AccountStore(const AccountStore& source) {}
		AccountStore(AccountStore&& source) {}
		//
		std::unordered_map<string, std::shared_ptr<account_base>> _account_ptrs_by_name;
	};
}
//
#endif /* monero_account_store_hpp */
