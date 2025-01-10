//=============================================================
//
// Copyright (c) 2025 Simon Southwell. All rights reserved.
//
// Date: 3rd January 2025
//
// Common definitions for TCP/PIv4 packet generator test programs
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

#ifndef _UDP_COMMON_H_
#define _UDP_COMMON_H_

#define CLIENT_MAC_ADDR      0xd89ef3887ec3ULL
#define SERVER_MAC_ADDR      0x90324b070bd1ULL

#define CLIENT_IPV4_ADDR     0xc0a81908 /* 192.168.25.8 */
#define SERVER_IPV4_ADDR     0xc0a89801 /* 192.168.152.1 */

#define UDP_PORT_NUM         0x0400


#define PKTBUFSIZE           ( 2*1024)
#define STRBUFSIZE           200

#define SMALL_PAUSE          20
#define END_PAUSE            50

#endif