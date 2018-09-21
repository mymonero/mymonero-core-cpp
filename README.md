# MyMonero Core C++

1. What's in this Repo?
1. Setup
1. Usage
1. Testing
1. Pull Requests
1. Developing
1. Contributors
1. API Documentation

## What's in This Repo?

This repository holds the C++ source code for the Monero/CryptoNote cryptography and protocols, plus lightwallet functions which power the official [MyMonero](https://www.mymonero.com) apps.

A primary goal for this repository is its future convergence and reimplementation with an essential Monero core library.


### Contents 

* `contrib/monero-core-custom` is a Git submodule which contains a curated subset of the official Monero C/C++ source code, eventually to be replaced by new official Monero core repo

* `src` contains all the custom code written for this repo. 
	* `serial_bridge_index` contains a central JSON interface to a set of central mymonero-core functions, documented below.
	
* This readme is located at `README.md`, and the license is located at `LICENSE.txt`. All source code copyright © 2014-2018 by MyMonero. All rights reserved.


## Setup

* Run `bin/update_submodules` 

## Usage

Simply embed the relevant code in your application, generally via CMake. See `CMakeLists.txt` for required files.

One example usage of this code is its transpilation into wasm and JS by mymonero-core-js.

## Testing

* Run `bin/buildAndRun_tests` to execute Boost test cases.

Please submit any bugs as Issues unless they have already been reported.

Suggestions and feedback are very welcome!

## Pull Requests

We'll merge nearly anything constructive and relevant. There are lots of items slated for development or happening concurrently, so get in touch and we'll let you know if anyone's working on it. We often collaborate over IRC in #mymonero on Freenode.

Contributors credited in releases.

**All development happens off the `develop` branch like the Gitflow Workflow.**

## Developing

* If you use a Mac and want to develop within Xcode, run `bin/genXcodeProj` to generate `./xcode/TEST.xcodeproj`. Once opened, its build target can be switched from **ALL\_BUILD** to **test\_all**, from which a *Build & Run* will display test results in the console.

## Regular and Main Contributors

* 💫 `endogenic` ([Paul Shapiro](https://github.com/paulshapiro)) Lead dev, maintainer

* 🤵 `vtnerd` C++ & Monero advisory

* 🐮 `moneromooo-monero` Major Monero contributor; Advisory


## API Documentation

### C++

*Coming soon*

### JSON

`src/serial_bridge_index` exposes a basic set of functions, each of which takes a string-serialized JSON object as an argument and returns a string-serialized JSON object.

Usage of each of these functions is demonstrated in `tests/test_all.cpp`.

When they fail, some of these functions return only a key-value `err_msg`.

### Argument and return value data types

* Some args must be passed as strings, such as `uint64` args like `index`s and `amount`s.

* Network type is sent as an argument as a string (`NettypeString ` below), which can be obtained via `serial_bridge::string_from_nettype()`.

* `WordsetString` is the value returned by `seed_and_keys_from_mnemonic` and `newly_created_wallet`.

* Some boolean return values may be sent by Boost as strings instead, called `Boolstring` below ( e.g. `"false"`). 

In future, the key names could be compressed.


#### Parsing Addresses

**`decode_address`**
	
* Args: `nettype_string: NettypeString`, `address: String`

* Returns: `err_msg: String` *OR* `pub_spendKey_string: String`, `pub_viewKey_string: String`, `paymentID_string: String`, and `isSubaddress: Boolstring`
	
**`is_subaddress`**
	
* Args: `nettype_string: NettypeString`, `address: String`

* Returns: `err_msg: String` *OR* `retVal: Boolstring`
	
**`is_integrated_address`**

* Args: `nettype_string: NettypeString`, `address: String`

* Returns: `err_msg: String` *OR* `retVal: Boolstring`

#### Parsing and Creating Wallets

**`newly_created_wallet`**

* Args: `nettype_string: NettypeString`, `locale_language_code: String`

* Returns: `err_msg: String` *OR* `mnemonic_string: String`, `mnemonic_language: WordsetName`, `sec_seed_string: String`, `address_string: String`, `pub_spendKey_string: String`, `pub_viewKey_string: String`, `sec_viewKey_string: String`, and `sec_spendKey_string: String`

**`are_equal_mnemonics`**

* Args: `a: String`, `b: String`

* Returns: `err_msg: String` *OR* `retVal: Boolstring`

**`mnemonic_from_seed`**

* Args: `seed_string: String`, `wordset_name: WordsetString`

* Returns: `err_msg: String` *OR* `retVal: String`

**`seed_and_keys_from_mnemonic`**

* Args: `nettype_string: NettypeString`, `mnemonic_string: String`

* Returns: `err_msg: String` *OR* `mnemonic_language: WordsetName`, `sec_seed_string: String`, `address_string: String`, `pub_spendKey_string: String`, `pub_viewKey_string: String`, `sec_viewKey_string: String`, and `sec_spendKey_string: String`

**`validate_components_for_login`**

* Args: `nettype_string: NettypeString`, `address_string: String`, `sec_viewKey_string: String`, `sec_spendKey_string: Optional<String>`, `seed_string: Optional<String>`

* Returns: `err_msg: String` *OR* `pub_spendKey_string: String`, `pub_viewKey_string: String`, `isInViewOnlyMode: Boolstring`, and `isValid: Boolstring`

**`address_and_keys_from_seed`**

* AKA `create_address` in legacy mymonero-core-js

* Args: `nettype_string: NettypeString`, `mnemonic_string: String`

* Returns: `err_msg: String` *OR* `address_string: String`, `pub_spendKey_string: String`, `pub_viewKey_string: String`, `sec_viewKey_string: String`, and `sec_spendKey_string: String`


#### Producing Misc. Values

**`new_integrated_address`**

* Args: `nettype_string: NettypeString`, `short_pid: String`, `address: String`

* Returns: `err_msg: String` *OR* `retVal: String`

**`new_fake_address_for_rct_tx`**

* Args: `nettype_string: NettypeString`

* Returns: `err_msg: String` *OR* `retVal: String`

**`new_payment_id`**

* Args: *empty object*

* Returns: `err_msg: String` *OR* `retVal: String`

**`generate_key_image`**

* Args: `sec_viewKey_string: String`, `sec_spendKey_string: String`, `pub_spendKey_string: String`, `tx_pub_key: String`, `out_index: Int64String`

* Returns: `err_msg: String` *OR* `retVal: String`
	
**`generate_key_derivation`**

* Args: `pub: String`, `sec: String`

* Returns: `err_msg: String` *OR* `retVal: String`

**`derive_public_key`**

* Args: `derivation: String`, `pub: String`, `out_index: UInt32String`

* Returns: `err_msg: String` *OR* `retVal: String`
	
**`derive_subaddress_public_key`**

* Args: `derivation: String`, `output_key: String`, `out_index: UInt32String`

* Returns: `err_msg: String` *OR* `retVal: String`

**`decodeRct`**

* Args: `i: UInt32String`, `sk: String`, `rv: DecodeRCT_RV`

	* `DecodeRCT_RV: Dictionary` with `type: RCTTypeIntString`, `ecdhInfo: [DecodeRCT_ECDHInfo]`, `outPk: [DecodeRCT_OutPK]`

		* `RCTTypeIntString: String` corresponding to values in `rct::RCTType*`

		* `DecodeRCT_ECDHInfo: Dictionary` with `amount: String`, `mask: String`

		* `DecodeRCT_OutPK: Dictionary` with `mask: String`
		
e.g.
```
{"i":"1","sk":"9b1529acb638f497d05677d7505d354b4ba6bc95484008f6362f93160ef3e503","rv":{"type":"1","ecdhInfo":[{"mask":"3ad9d0b3398691b94558e0f750e07e5e0d7d12411cd70b3841159e6c6b10db02","amount":"b3189d8adb5a26568e497eb8e376a7d7d946ebb1daef4c2c87a2c30b65915506"},{"mask":"97b00af8ecba3cb71b9660cc9e1ac110abd21a4c5e50a2c125f964caa96bef0c","amount":"60269d8adb5a26568e497eb8e376a7d7d946ebb1daef4c2c87a2c30b65915506"},{"mask":"db67f5066d9455db404aeaf435ad948bc9f27344bc743e3a32583a9e6695cb08","amount":"b3189d8adb5a26568e497eb8e376a7d7d946ebb1daef4c2c87a2c30b65915506"}],"outPk":[{"mask":"9adc531a9c79a49a4257f24e5e5ea49c2fc1fb4eef49e00d5e5aba6cb6963a7d"},{"mask":"89f40499d6786a4027a24d6674d0940146fd12d8bc6007d338f19f05040e7a41"},{"mask":"f413d28bd5ffdc020528bcb2c19919d7484fbc9c3dd30de34ecff5b8a904e7f6"}]}}
```

* Returns `err_msg: String` *OR* `amount: String` and `mask: String`

#### Fees
	
**`estimate_rct_tx_size`**

* Args: `n_inputs: Int32`, `mixin: Int32`, `n_outputs: Int32`, `extra_size: Int32`, `bulletproof: Bool`

* Returns: `retVal: Int32String`

**`calculate_fee`**

* Args: `fee_per_kb: Int64String`, `num_bytes: Int32String`, `fee_multiplier: Int64String`

* Returns: `retVal: Int64String`

**`estimated_tx_network_fee`**

Convenience wrapper

* Args: `fee_per_kb: Int64String`, `priority: Int32String`

* Returns: `retVal: Int64String`


#### Transactions

**`create_transaction`**

* Args: `from_address_string: String`, `sec_viewKey_string: String`, `sec_spendKey_string: String`, `to_address_string: String`, `sending_amount: Int64String`, `change_amount: Int64String`, `fee_amount: Int64String`, `outputs: [UnspentOutput]`, `mix_outs: [MixAmountAndOuts]`,  `unlock_time: Int64String`, `nettype_string: NettypeString`

	* `UnspentOutput: Dictionary` with `amount: UInt64String`, `public_key: String`, `rct: OptionalString`, `global_index: UInt64String`, `index: UInt64String`, `tx_pub_key: String`

	* `MixAmountAndOuts: Dictionary` with `amount: UInt64String`, `outputs: [MixOut]`

		* `MixOut: Dictionary` with `global_index: UInt64String`, `public_key: String`, `rct: OptionalString`

* Returns: `err_msg: String` *OR* `serialized_signed_tx: String`, `tx_hash: String`, `tx_key: String`


