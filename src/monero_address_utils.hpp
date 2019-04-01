//
//  monero_address_utils.hpp
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
#include <string>
#include <boost/optional.hpp>
//
#include "cryptonote_config.h"
#include "cryptonote_basic/account.h"

#include "tools__ret_vals.hpp"
//
namespace monero
{
	namespace address_utils
	{
		using namespace std;
		using namespace boost;
		using namespace cryptonote;
		
		struct DecodedAddress_RetVals: tools::RetVals_base
		{
			optional<string> pub_viewKey_string;
			optional<string> pub_spendKey_string;
			bool isSubaddress;
			optional<string> paymentID_string;
		};
		//
		DecodedAddress_RetVals decodedAddress(const string &addressString, network_type nettype);
		bool isSubAddress(const string &addressString, network_type nettype);
		bool isIntegratedAddress(const string &addressString, network_type nettype);
		//
		optional<string> new_integratedAddrFromStdAddr(const string &std_address_string, const string &short_paymentID, cryptonote::network_type nettype);
	}
}
