# MyMonero Core C++

1. What's in this Repo?
1. Setup
1. Usage
1. Testing
1. Pull Requests
1. Developing
1. Contributors
1. Embedding the C++
1. API 

## What's in This Repo?

This repository holds the C++ source code for the Monero/CryptoNote cryptography and protocols, plus lightwallet functions which power the official [MyMonero](https://www.mymonero.com) apps.

A primary goal for this repository is its future convergence and reimplementation with an essential Monero core library.


### Contents 

* `contrib/monero-core-custom` is a Git submodule which contains a curated subset of the official Monero C/C++ source code, eventually to be replaced by new official Monero core repo

* `src` contains all the custom code written for this repo. 
	* `serial_bridge_index` contains a central JSON interface to a set of central mymonero-core functions, documented below.
	
* This readme is located at `README.md`, and the license is located at `LICENSE.txt`. All source code copyright © 2014-2018 by MyMonero. All rights reserved.


### Dependencies

* (to use included shell scripts) cmake

* Boost 1.58 library components

	* `system`

	* `thread`

	* If running tests: `unit_test_framework`

* `monero-core-custom` (see "Setup")


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

* Branches and PRs should be made from and to the `develop` branch, which gets merged to `master` for tagged releases

## Authors, Contributors, and Advisors

* 💫 [`Paul Shapiro`](https://github.com/paulshapiro) Lead dev, maintainer

* 🍄 `luigi1111`

* 🤵 `vtnerd` C++ & Monero advisory

* 🐮 `moneromooo-monero` Major Monero contributor; Advisory

* 🦁 `ndorf` C++ & Monero expertise, architecture, code contributions

* 🏍 `gutenye` Code contributions


## Embedding the C++

If you want to embed the C++ or build the source in your own project, please take note of the following:

* `slow-hash.c` must be compiled with `-maes`

* The Monero source, a slightly modified version of which is a dep of this project, can only be built on versions of iOS >= 9 due to required support for `thread_local` as used by `threadpool.cpp`.

* Not all Monero `.cpp` files which are in `monero-core-custom` must be included for `mymonero-core-cpp` - such as when their symbols are not required by any code called by this project. See `CMakeLists.txt` for a list of files required for compilation.

* If you only want to call the C++ directly without using `serial_bridge_index` (described below), then that file-pair does not need to be included in your build. (See mymonero-app-ios link)


## API 

#### Notes:

* If you are implementing the below-described Send routine yourself (examples: [JS](https://github.com/mymonero/mymonero-core-js/blob/master/monero_utils/monero_sendingFunds_utils.js#L100), [Swift](https://github.com/mymonero/mymonero-app-ios/blob/6deb815257e654ee9639a2b7a08a103b29a910f7/Modules/HostedMonero/HostedMonero_SendingFunds.swift#L179)), you should:
	* implement the [necessary re-entry logic](https://github.com/mymonero/mymonero-app-ios/blob/6deb815257e654ee9639a2b7a08a103b29a910f7/Modules/HostedMonero/HostedMonero_SendingFunds.swift#L265) if step2 indicates reconstruction required, and
	* *(optional)* [construct the `err_msg`](https://github.com/mymonero/mymonero-app-ios/blob/6deb815257e654ee9639a2b7a08a103b29a910f7/Modules/MyMoneroCore/Swift/MyMoneroCore_ObjCpp.mm#L563) if the error code indicates not enough spendable balance.

### C++

*Coming soon*

For examples see `src/serial_bridge_index.cpp` and [mymonero-app-ios/MyMoneroCore_ObjCpp.mm](https://github.com/mymonero/mymonero-app-ios/blob/master/Modules/MyMoneroCore/Swift/MyMoneroCore_ObjCpp.mm).


### JSON

`src/serial_bridge_index` exposes this project's core library functions. Each bridge function takes a string-serialized JSON object as an argument and returns a string-serialized JSON object.

Usage of each of these JSON-bridge functions is demonstrated in `tests/test_all.cpp`.

When they fail, some of these functions return only a key-value `err_msg`.

### Argument and return value data types

* Some args must be passed as strings, such as `uint64` args like `index`s and `amount`s. 

* Some boolean return values may be sent (by Boost) in JSON as strings instead, called `BoolString` below ( e.g. `"false"`). 

* Network type is sent as a string (`NettypeString` below), which can be obtained via `serial_bridge::string_from_nettype()`.


**Compressing / reducing size of key names could be a significant optimization, as could a migration to msgpack.**


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

* Returns: `err_msg: String` *OR* `mnemonic_string: String`, `mnemonic_language: WordsetNameString`, `sec_seed_string: String`, `address_string: String`, `pub_spendKey_string: String`, `pub_viewKey_string: String`, `sec_viewKey_string: String`, and `sec_spendKey_string: String`

**`are_equal_mnemonics`**

* Args: `a: String`, `b: String`

* Returns: `err_msg: String` *OR* `retVal: Boolstring`

**`mnemonic_from_seed`**

* Args:
	* `seed_string: String`
	* `wordset_name: WordsetNameString` returned as `mnemonic_language` by `seed_and_keys_from_mnemonic` and `newly_created_wallet`

* Returns: `err_msg: String` *OR* `retVal: String`

**`seed_and_keys_from_mnemonic`**

* Args: `nettype_string: NettypeString`, `mnemonic_string: String`

* Returns: `err_msg: String` *OR* `mnemonic_language: WordsetNameString`, `sec_seed_string: String`, `address_string: String`, `pub_spendKey_string: String`, `pub_viewKey_string: String`, `sec_viewKey_string: String`, and `sec_spendKey_string: String`

**`validate_components_for_login`**

* Args: `nettype_string: NettypeString`, `address_string: String`, `sec_viewKey_string: String`, `sec_spendKey_string: Optional<String>`, `seed_string: Optional<String>`

* Returns: `err_msg: String` *OR* `pub_spendKey_string: String`, `pub_viewKey_string: String`, `isInViewOnlyMode: Boolstring`, and `isValid: Boolstring`

**`address_and_keys_from_seed`**

* AKA `create_address` in legacy mymonero-core-js

* Args: `nettype_string: NettypeString`, `seed_string: String`

* Returns: `err_msg: String` *OR* `address_string: String`, `pub_spendKey_string: String`, `pub_viewKey_string: String`, `sec_viewKey_string: String`, and `sec_spendKey_string: String`


#### Producing Misc. Values

**`new_integrated_address`**

* Args: `nettype_string: NettypeString`, `short_pid: String`, `address: String`

* Returns: `err_msg: String` *OR* `retVal: String`

**`new_payment_id`**

* Args: *empty object*

* Returns: `err_msg: String` *OR* `retVal: String`

**`generate_key_image`**

* Args: `sec_viewKey_string: String`, `sec_spendKey_string: String`, `pub_spendKey_string: String`, `tx_pub_key: String`, `out_index: UInt32String`

* Returns: `err_msg: String` *OR* `retVal: String`
	
#### Fees
	
**`estimated_tx_network_fee`**

Useful for displaying an estimated fee – To obtain exact fees, see "Creating and Sending Transactions"

* Args: 
	* `fee_per_b: UInt64String`
	* `priority: UInt32String`
	* `fork_version: UInt8String`

* Returns: `retVal: UInt64String`
