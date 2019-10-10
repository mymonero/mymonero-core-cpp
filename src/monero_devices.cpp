//
//  monero_devices.cpp
//  MyMonero
//
//  Created by Paul Shapiro on 10/11/19.
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
#include "monero_devices.hpp"
//
#include "../test/device_mymonero/device_mymonero.hpp"
//
using namespace monero_devices;
//
const string device_type__software = "software"; // this is what you'll get if you just specify device_name of 'default'
const string device_type__ledger = "ledger";
const string device_type__trezor = "trezor";
const string device_type__mymonero = "mymonero";
//

bool _is_key_on_device(const string &device_type)
{
	return device_type != device_type__software;
}

monero_account_store::AccountStore::hwdevice_alloc_fn_type monero_devices::Factory::CORE_DEVICE_ALLOC_FN = [](
	const string &device_name, const string &device_type
) {
	try {
		hw::device &hwdev = hw::get_device(device_name);
		//
		return &hwdev;
	} catch (std::exception const& e) {
		hw::device *hwdev_ptr;
		if (device_type == device_type__software) {
			hwdev_ptr = new hw::core::device_default;
		} else if (device_type == device_type__mymonero) {
			// USE_DEVICE_MYMONERO is already defined in device_mymonero.hpp, so all that needs to occur is the inclusion of the latter
			#ifdef USE_DEVICE_MYMONERO
			hwdev_ptr = new hw::mymonero::device_mymonero;
			#else
			throw std::runtime_error("Compile with device_mymonero");
			#endif
		} else if (device_type == device_type__trezor) {
		   throw std::runtime_error("Compile with device_trezor support for hw::get_device(…)");
		} else if (device_type == device_type__ledger) {
			throw std::runtime_error("Compile with device_ledger support for hw::get_device(…)");
		}
		hw::register_device(device_name, hwdev_ptr); // throw this in to stay conformant; monero_account_store does also register just in case but asks for a registered hwdev
		//
		return hwdev_ptr;
	}
};
