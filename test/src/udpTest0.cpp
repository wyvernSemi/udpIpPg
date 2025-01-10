//=============================================================
//
// Copyright (c) 2025 Simon Southwell. All rights reserved.
//
// Date: 3rd January 2025
//
// Class method definitions UDP client example test program
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
#include <cstring>

#include "udpIpPg.h"
#include "udpTest0.h"

// --------------------------------------------
// Helper method to send a UDP text message
// --------------------------------------------
void udpTest0::sendTextMessage(const char* mess_str, const uint32_t dst_port, const uint32_t ip_dst_addr, const uint64_t mac_dst_addr, udpIpPg *pUdp)
{
    udpIpPg::udpConfig_t pktCfg;
    
    uint32_t payloadlen = strlen(mess_str);

    // Copy string bytes to payload buffer (not bytes)
    for (int idx = 0; idx < payloadlen; idx++)
    {
        payload[idx]    = mess_str[idx];
    }

    // Configure a transmission
    pktCfg.dst_port     = dst_port;
    pktCfg.ip_dst_addr  = ip_dst_addr;
    pktCfg.mac_dst_addr = mac_dst_addr;
    
    // Generate a frame of data using configuration and payload
    uint32_t len = pUdp->genUdpIpPkt (pktCfg, frmBuf, payload, payloadlen);

    // Transmit packet over node's bus
    pUdp->UdpVpSendRawEthFrame (frmBuf, len);
}

// --------------------------------------------
// Top level test method
// --------------------------------------------

uint32_t udpTest0::runTest()
{
    char     sbuf[STRBUFSIZE];
    char     vstr    [12];
    udpIpPg::rxInfo_t connLastPkt;

    // Create a udpIpPg object
    pUdp = new udpIpPg(node, CLIENT_IPV4_ADDR, CLIENT_MAC_ADDR, UDP_PORT_NUM);

    pUdp->getVersionString(vstr);

    VPrint("\nudpIpPg version %s\n\n", vstr);

    // Let the simulation run for a few ticks
    pUdp->UdpVpSendIdle(SMALL_PAUSE);

    // Register RX call back function
    pUdp->registerUsrRxCbFunc(rxCallback, (void*)this);

    // -----------------------------------
    // Send packet(s) with data...

    // Send first message
    sprintf(sbuf, "*** Data Packet #1 from node %d ***\n\n", node);
    sendTextMessage(sbuf, UDP_PORT_NUM+1, SERVER_IPV4_ADDR, SERVER_MAC_ADDR, pUdp);

    // Wait a bit
    pUdp->UdpVpSendIdle(20);

    // Send second message
    sprintf(sbuf, "*** Data Packet #2 from node %d ***\n\n", node);
    sendTextMessage(sbuf, UDP_PORT_NUM+1, SERVER_IPV4_ADDR, SERVER_MAC_ADDR, pUdp);

    // Wait a bit
    pUdp->UdpVpSendIdle(20);

    // Request simulation to finish
    pUdp->UdpVpSetHalt(1);

    pUdp->UdpVpSendIdle(END_PAUSE);

    return 0;
}