//=============================================================
//
// Copyright (c) 2025 Simon Southwell. All rights reserved.
//
// Date: 3rd January 2024
//
// Class definition of UDP server example test program
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

#ifndef _UDP_TEST1_H_
#define _UDP_TEST1_H_

#include <vector>

#include "udpTestBase.h"

class udpTest1 : public udpTestBase
{
public:

    // Constructor
    udpTest1(int nodeIn) : udpTestBase(nodeIn) {};

    // Test method specific to this class
    uint32_t runTest     ();

};

#endif