//=============================================================
//
// Copyright (c) 2025 Simon Southwell. All rights reserved.
//
// Date: 3rd January 2025
//
// Class definition of UDP client example test program
//
// This code is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this code. If not, see <http://www.gnu.org/licenses/>.
//
//=============================================================

#ifndef _UDP_TEST0_H_
#define _UDP_TEST0_H_

#include <vector>

#include "udpTestBase.h"
#include "udpCommon.h"

class udpTest0 : public udpTestBase
{
public:
    // Constructor
    udpTest0(int nodeIn) : udpTestBase(nodeIn) {};

    // Test method, specific to this class
    uint32_t runTest     ();
    
private:
    uint32_t payload [PKTBUFSIZE];
    uint32_t frmBuf  [PKTBUFSIZE];
    
    void sendTextMessage(const char*    mess_str, 
                         const uint32_t dst_port, 
                         const uint32_t ip_dst_addr, 
                         const uint64_t mac_dst_addr, 
                         udpIpPg*       pUdp);
};

#endif