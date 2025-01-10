//=============================================================
//
// Copyright (c) 2025 Simon Southwell. All rights reserved.
//
// Date: 3rd December 2025
//
// Class method definitions UDP server example test program
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

#include <vector>

#include "udpIpPg.h"
#include "udpTest1.h"
#include "udpCommon.h"

static char sbuf [udpIpPg::ETH_MTU];

// --------------------------------------------
// --------------------------------------------

uint32_t udpTest1::runTest()
{
    udpIpPg::rxInfo_t pkt;

    pUdp = new udpIpPg(node, SERVER_IPV4_ADDR, SERVER_MAC_ADDR, UDP_PORT_NUM);

    // Register RX call back function
    pUdp->registerUsrRxCbFunc(rxCallback, (void*)this);
    
    while (true)
    {
        // Wait for Packet
        while(rxQueue.empty())
        {
            pUdp->UdpVpSendIdle(20);
        }
        
        pkt = rxQueue.front();
        rxQueue.erase(rxQueue.begin());
        
        // Process any packet data
        if (pkt.rx_len)
        {
            for(int idx = 0; idx < pkt.rx_len; idx++)
            {
                sbuf[idx] = pkt.rx_payload[idx];
            }
            sbuf[pkt.rx_len] = 0;
            VPrint("Node%d: %s\n", node, sbuf);
        }
    }


    return 0;
}