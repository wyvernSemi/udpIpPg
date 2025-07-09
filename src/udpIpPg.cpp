//=============================================================
//
// Copyright (c) 2024 - 2025 Simon Southwell. All rights reserved.
//
// Date: 16th December 2024
//
// Class method definitions for UDP/IPv4 packet generation
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

#include <cinttypes>

#include "udpIpPg.h"

// --------------------------------------------------
// Calculate CRC32 for ethernet frame.
// --------------------------------------------------

uint32_t udpIpPg::crc32(uint32_t *buf, uint32_t len, uint32_t poly, uint32_t init, bool debug)
{

    uint32_t val;
    uint32_t crc;
    uint8_t i;

    crc                                = init;

    while (len--)
    {
        val                            = (crc ^ *buf++) & 0xFF;
        for(i = 0; i < 8; i++)
        {
            val                        = (val & 1) ? (val >> 1) ^ poly : val >> 1;
        }
        crc                            = val ^ crc >> 8;
    }
    return crc ^ 0xFFFFFFFF;
}

// --------------------------------------------------
// Checksum calculation for IPV4. Also used to
// calculate UDP checksum, which includes parts of
// IPV4 settings in a 'pseudo-header'
// --------------------------------------------------

uint32_t udpIpPg::ipv4_chksum (uint32_t* buf, uint32_t len, bool debug)
{
    uint32_t sum                       = 0;

    for (int idx = 0; idx < (len-1); idx+=2)
    {
        sum                            += ((buf[idx] & 0xff) << 8) | (buf[idx+1] & 0xff);

        if (debug)
        {
            printf ("sum=0x%04x word=0x%04x\n", sum, ((buf[idx] & 0xff) << 8) | (buf[idx+1] & 0xff));
        }
    }

    if (len & 1)
    {
        sum                            += (buf[len-1] & 0xff) << 8 ;
    }

    return sum;
}

// --------------------------------------------------
// Generate a UDP/IP packet. Parameters passed in
// with cfg, and any data in payload (with payload
// length). Data stored in frm_buf, which must be
// sufficiently large to receive data.
// --------------------------------------------------

uint32_t udpIpPg::genUdpIpPkt (udpConfig_t &cfg, uint32_t* frm_buf, uint32_t* payload, uint32_t payload_len)
{
    // Intermediate buffers for UDP and IPV4 data
    uint32_t udp_payload[2048];
    uint32_t ipv4_payload[2048];

    // Construct a UDP segment and place in udp_payoad. Returns total length of segment
    uint32_t udplen = udpSegment(udp_payload,
                                 payload,
                                 payload_len,
                                 cfg.dst_port);

    // Wrap UDP in an IPV4 frame, and add checksum to UDP (which includes pseudo-IP header).
    // Data places in ipv4_payload and method returns total length.
    uint32_t iplen  = ipv4Frame (ipv4_payload, udp_payload, udplen, cfg.ip_dst_addr);

    // Wrap IPV4 Frame in an ethernet frame, placing in frm_buf and returning total length of data
    uint32_t flen   = ethFrame  (frm_buf, ipv4_payload, iplen, cfg.mac_dst_addr);

    // Return length of data in bytes.
    return flen;
}

// --------------------------------------------------
// Construct UDP segment
// -------------------------------------------------

uint32_t udpIpPg::udpSegment (uint32_t* udp_seg,
                              uint32_t* payload,
                              uint32_t  payload_len,
                              uint32_t  dst_port)
{
    // Initialise a frame index
    uint32_t fidx                      = 0;

    // Add source port
    udp_seg[fidx++]                    = (udp_port >> 8) & 0xff;
    udp_seg[fidx++]                    = udp_port & 0xff;

    // Add destination port
    udp_seg[fidx++]                    = (dst_port >> 8) & 0xff;
    udp_seg[fidx++]                    = dst_port & 0xff;

    // Add datagram length
    uint16_t udplen                    = (UDP_MIN_HDR_LEN *4) + payload_len;
    udp_seg[fidx++]                    = (udplen >> 8) & 0xff;
    udp_seg[fidx++]                    = udplen & 0xff;

    // Blank checksum holder (need IP values for IP pseudo header before we can calculate, so postpone)
    udp_seg[fidx++]                    = 0;
    udp_seg[fidx++]                    = 0;

    // Add payload (if any)
    for (int idx = 0; idx < payload_len; idx++)
    {
        udp_seg[fidx++]                = payload[idx] & 0xff;
    }

    // Return the length of the UDP segment
    return fidx;
}

// --------------------------------------------------
// Construct IPv4 frame
// --------------------------------------------------

uint32_t udpIpPg::ipv4Frame (uint32_t* ipv4_frame, uint32_t* payload, uint32_t payload_len, uint32_t ipv4_dst_addr, bool add_udp_chksum)
{
    // Initialise a frame index
    uint32_t fidx                      = 0;

    // Add IPV4 type and header length
    ipv4_frame[fidx++]                 = (0x4 << 4) | IPV4_MIN_HDR_LEN; // IPv4 and IHL = 5 (DWORDS)
    ipv4_frame[fidx++]                 = 0x00; // DSCP = 0, ECN = 0

    // Add the total length for header and payload (in bytes)
    uint32_t total_len                 = IPV4_MIN_HDR_LEN*4 + payload_len; // Total length in bytes
    ipv4_frame[fidx++]                 = total_len >> 8;
    ipv4_frame[fidx++]                 = total_len & 0xff;

    // Add and ID
    uint32_t id                        = 0x0002;
    ipv4_frame[fidx++]                 = id >> 8;
    ipv4_frame[fidx++]                 = id & 0xff;

    // Add flags and any fragment offset
    uint32_t flags                     = 0;
    uint32_t frag_offset               = 0x400;
    ipv4_frame[fidx++]                 = flags | ((frag_offset >> 8) & 0xf) << 4;
    ipv4_frame[fidx++]                 = frag_offset & 0xff;

    // Set the time to live value
    ipv4_frame[fidx++]                 = 0xff; // Time to Live

    // Set type ID (= UDP)
    ipv4_frame[fidx++]                 = UDP_PROTOCOL_NUM; // Protocol (= UDP)

    // Blank checksum and fill in once addresses added and can calculate
    uint32_t chksum_offset             = fidx;
    ipv4_frame[fidx++]                 = 0;
    ipv4_frame[fidx++]                 = 0;

    // IPV4 SRC address (MSB first)
    for(int idx = 0; idx < 4; idx++)
    {
        ipv4_frame[fidx++]             = ipv4_addr >> 8*(3-idx);
    }

    // IPV4 DST address (MSB first)
    for(int idx = 0; idx < 4; idx++)
    {
        ipv4_frame[fidx++]             = ipv4_dst_addr >> 8*(3-idx);
    }

    // Calculate basic checksum
    uint32_t chksum                    = ipv4_chksum(ipv4_frame, fidx);

    // One's complement checksum
    chksum = ~((chksum & 0xffff) + (chksum >> 16)) & 0xffff;

    // Add the IPV4 checksum
    ipv4_frame[chksum_offset]          = chksum >> 8;
    ipv4_frame[chksum_offset+1]        = chksum & 0xff;

    // Remember the offset where the payload begins
    uint32_t payload_offset            = fidx;

    // Add any payload
    for (int idx = 0; idx < payload_len; idx++)
    {
        ipv4_frame[fidx++]             = payload[idx] & 0xff;
    }
    
    // Clear checksum value
    ipv4_frame[payload_offset + UDP_CHKSUM_OFFSET]   = 0;
    ipv4_frame[payload_offset + UDP_CHKSUM_OFFSET+1] = 0;

    // If requested, calculate and add the UDP checksum, based on UDP packet and IPV4 pseudo header
    if (add_udp_chksum)
    {
        // Calculate the partial checksum for the UDP segment
        uint32_t partial_chksum        = ipv4_chksum(&ipv4_frame[payload_offset], payload_len);

        // Calculate the rest of the checksum with the IP pseudo-header data
        partial_chksum                 += (ipv4_addr >> 16)     & 0xffff;
        partial_chksum                 += (ipv4_addr >>  0)     & 0xffff;
        partial_chksum                 += (ipv4_dst_addr >> 16) & 0xffff;
        partial_chksum                 += (ipv4_dst_addr >>  0) & 0xffff;
        partial_chksum                 += (UDP_PROTOCOL_NUM);
        partial_chksum                 += (payload_len);

        // One's complement checksum
        partial_chksum                 = ~((partial_chksum & 0xffff) + (partial_chksum >> 16)) & 0xffff;

        // Write UDP checksum to buffer
        ipv4_frame[payload_offset + UDP_CHKSUM_OFFSET]   = partial_chksum >> 8;
        ipv4_frame[payload_offset + UDP_CHKSUM_OFFSET+1] = partial_chksum & 0xff;

    }

    // Return the length of the frame (in bytes)
    return fidx;
}

// --------------------------------------------------
// Construct ethernet frame
// --------------------------------------------------

uint32_t udpIpPg::ethFrame(uint32_t* frame, uint32_t* payload, uint32_t payload_len, uint64_t dst_addr)
{
    uint32_t fidx                      = 0;

    // Check that any payload can fit in an ethernet packet
    if (payload_len > ETH_MTU)
    {
        printf("NODE%d: ethFrame() : ***ERROR. Specified payload length (%d) too big. Must be <= %d\n", node, payload_len, ETH_MTU);
        return 0;
    }

#ifdef GENERATE_SOF_EOF
    // Add a start-of-frame token
    frame[fidx++]                      = SOF;
#endif

    // Add 7 bytes of preamble
    for (int idx = 0; idx < ETH_PREAMBLE-2; idx++)
    {
        frame[fidx++]                  = PREAMBLE;
    }

    // Add start-of-frame delimiter
    frame[fidx++]                      = SFD;

    uint32_t pkt_start                 = fidx;

    // Add 48 bits of destination address (MSB first)
    for (int idx = 0; idx < 6; idx++)
    {
        frame[fidx++]                  = (dst_addr >> (8*(5-idx))) & 0xff;
    }

    // Add 48 bits of source address (MSB first)
    for (int idx = 0; idx < 6; idx++)
    {
        frame[fidx++]                  = (mac_addr >> (8*(5-idx))) & 0xff;
    }

    // Add 2 bytes of Ethernet type (0x800 = IPv4)
    frame[fidx++]                      = 0x08;
    frame[fidx++]                      = 0x00;

    // Add the payload
    for (int idx = 0; idx < payload_len; idx++)
    {
        frame[fidx++]                  = payload[idx] & 0xff;
    }

    // If the payload runs short of the 64 byte minimum size then pad
    if (payload_len < 46)
    {
        for (int idx = 0; idx < (46 - payload_len); idx++)
        {
            frame[fidx++]              = 0;
        }
    }

    // Calculate the CRC (excluding SOF, SFD and preamble)
    uint32_t crc = crc32(&frame[pkt_start], fidx-pkt_start);

    // Add crc
    for (int idx = 0; idx < 4; idx++)
    {
         frame[fidx++]                 = (crc >> (8*idx)) & 0xff;
    }

#ifdef GENERATE_SOF_EOF
    // Add the EOF delimiter
    frame[fidx++]                      = EoF;
#endif

    // Return the length of the ethernet data (in bytes), including preamble
    return fidx;

}

// --------------------------------------------------
// Process the received frames
// --------------------------------------------------

uint32_t udpIpPg::processFrame (uint32_t* rx_data, uint32_t rx_len)
{
    uint32_t error                     = 0;

    rxInfo_t rxInfo;

    // -------------------------
    // MAC
    // -------------------------

    // Check frame's CRC
    uint32_t crc                       = crc32(rx_data, rx_len-4);
    uint32_t pktcrc                    = rx_data[rx_len-1] << 24 |
                                         rx_data[rx_len-2] << 16 |
                                         rx_data[rx_len-3] <<  8 |
                                         rx_data[rx_len-4] <<  0 ;

    if (crc != pktcrc)
    {
        //error                          |= RX_BAD_CRC;
        printf("WARNING: bad MAC CRC on received packet (got 0x%08x, exp 0x%08x)\n", pktcrc, crc);
        return error;
    }

    // Check that the MAC address is for us
    uint32_t ridx = 0;
    uint64_t dst_mac_addr              = (uint64_t)rx_data[ridx++] << 40 |
                                         (uint64_t)rx_data[ridx++] << 32 |
                                         (uint64_t)rx_data[ridx++] << 24 |
                                         (uint64_t)rx_data[ridx++] << 16 |
                                         (uint64_t)rx_data[ridx++] <<  8 |
                                         (uint64_t)rx_data[ridx++];

    if (dst_mac_addr != mac_addr)
    {
        error                          |= 1; // Incorrect MAC addr
        printf("WARNING: non-matching MAC address on received packet\n");
        return error;
    }

    // Extract SRC addr
    rxInfo.mac_src_addr                = (uint64_t)rx_data[ridx++] << 40 |
                                         (uint64_t)rx_data[ridx++] << 32 |
                                         (uint64_t)rx_data[ridx++] << 24 |
                                         (uint64_t)rx_data[ridx++] << 16 |
                                         (uint64_t)rx_data[ridx++] <<  8 |
                                         (uint64_t)rx_data[ridx++];

    // -------------------------
    // IPV4
    // -------------------------

    // Check IP header for integrity and addressed to us and, if so, save src address
    uint32_t chksum                    = ipv4_chksum(&rx_data[ETH_HDR_LEN], IPV4_MIN_HDR_LEN*4);
    chksum                             = ~((chksum & 0xffff) + (chksum >> 16)) & 0xffff;

    if (chksum)
    {
        error |= RX_BAD_IPV4_CHECKSUM;
        printf("WARNING: bad IPV4 checksum on received packet\n");
        return error;
    }

    uint32_t total_len                 =  (rx_data[ETH_HDR_LEN+2] << 8) |
                                          (rx_data[ETH_HDR_LEN+3]);

    uint32_t ipv4_payload_len          = total_len - IPV4_MIN_HDR_LEN*4;

    ridx                               = ETH_HDR_LEN + IPV4_SRC_ADDR_OFFSET*4;

    rxInfo.ipv4_src_addr               = rx_data[ridx++] << 24 |
                                         rx_data[ridx++] << 16 |
                                         rx_data[ridx++] <<  8 |
                                         rx_data[ridx++];

    uint32_t ipv4_dst_addr             = rx_data[ridx++] << 24 |
                                         rx_data[ridx++] << 16 |
                                         rx_data[ridx++] <<  8 |
                                         rx_data[ridx++];

    if (ipv4_dst_addr != ipv4_addr)
    {
        error                          |= RX_WRONG_IPV4_ADDR;
        printf("WARNING: non-matching IPV4 address on received packet\n");
        return error;
    }

    // -------------------------
    // UDP
    // -------------------------

    // Check UDP segment integrity and correct port. Save src port #

    // Calculate the partial checksum for the UDP segment
    uint32_t partial_chksum            = ipv4_chksum(&rx_data[ridx], ipv4_payload_len);

    // Calculate the rest of the checksum with the IP pseudo-header data
    partial_chksum                     += (rxInfo.ipv4_src_addr >> 16) & 0xffff;
    partial_chksum                     += (rxInfo.ipv4_src_addr >>  0) & 0xffff;
    partial_chksum                     += (ipv4_dst_addr >> 16)        & 0xffff;
    partial_chksum                     += (ipv4_dst_addr >>  0)        & 0xffff;
    partial_chksum                     += (UDP_PROTOCOL_NUM);
    partial_chksum                     += (ipv4_payload_len);

    // One's complement checksum
    partial_chksum                     = ~((partial_chksum & 0xffff) + (partial_chksum >> 16)) & 0xffff;

    // Extract UDP info
    rxInfo.udp_src_port                = rx_data[ridx++] << 8 |
                                         rx_data[ridx++];

    rxInfo.udp_dst_port                = rx_data[ridx++] << 8 |
                                         rx_data[ridx++];
     
    // Length of payload without the header     
    rxInfo.rx_len                      = (rx_data[ridx++] << 8 |
                                          rx_data[ridx++]) - UDP_MIN_HDR_LEN*4;
                                         
    uint32_t udpchksum                 = rx_data[ridx++] << 8 |
                                         rx_data[ridx++];

    // If calculated partial_chksum isn't zero, and the received checksum was valid (not 0),
    // thisis an error.
    if (partial_chksum && udpchksum)
    {
        error                          |= RX_BAD_UDP_CHECKSUM;
        printf("WARNING: bad UDP checksum on received packet (0x%08x)\n", partial_chksum);
        return error;
    }

    if (false && rxInfo.udp_dst_port != udp_port)
    {
        error                          |= RX_WRONG_UDP_PORT;
        printf("WARNING: non-matching UDP port number on received packet\n");
        return error;
    }


    // If all checks out, extract payload and call user callback, if one registered
    if (!error && usrRxCbFunc != NULL)
    {
        for(int idx = 0; idx < rxInfo.rx_len; idx++)
        {
            rxInfo.rx_payload[idx]     = rx_data[ridx++];
        }
        (*usrRxCbFunc)(rxInfo, hdl);
    }

    return error;

}