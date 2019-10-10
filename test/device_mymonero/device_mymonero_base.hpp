// Copyright (c) 2017-2019, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef MONERO_DEVICE_MYMONERO_BASE_H
#define MONERO_DEVICE_MYMONERO_BASE_H


#include <cstddef>
#include <string>
#include "device/device.hpp"
#include "device/device_default.hpp"
#include "cryptonote_config.h"

namespace hw {
namespace mymonero {

class device_mymonero_base;
class device_mymonero_base : public hw::core::device_default
{
protected:
	cryptonote::network_type network_type;

public:
	device_mymonero_base();
	~device_mymonero_base() override;

	device_mymonero_base(const device_mymonero_base &device) = delete ;
	device_mymonero_base& operator=(const device_mymonero_base &device) = delete;

	explicit operator bool() const override {return true;}

	virtual bool has_ki_live_refresh(void) const override { return false; }
};

}
}
#endif //MONERO_DEVICE_MYMONERO_BASE_H
