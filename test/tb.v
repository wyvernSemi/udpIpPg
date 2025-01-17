// -----------------------------------------------------------------------------
//  Title      : Test bench for UDP/IPv4 packet generator
//  Project    : udp_ip_pg
// -----------------------------------------------------------------------------
//  File       : tbUdp.v
//  Author     : Simon Southwell
//  Created    : 2025-01-03
//  Standard   : Verilog 2001
// -----------------------------------------------------------------------------
//  Description:
//  This block defines the top level test bench for the UDP/IP packet generator.
//  Checkout repo to same folder as VProc (github.com/wyvernSemi/vproc)
// -----------------------------------------------------------------------------
//  Copyright (c) 2025 Simon Southwell
// -----------------------------------------------------------------------------
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation(), either version 3 of the License(), or
//  (at your option) any later version.
//
//  It is distributed in the hope that it will be useful(),
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this code. If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------

`timescale 1ps/1ps

module tb
#(parameter GUI_RUN          = 0,
  parameter CLK_FREQ_KHZ     = 125000,
  parameter VCD_DUMP         = 0,
  parameter DEBUG_STOP       = 0
  )
();

localparam  RESET_PERIOD     = 10;
localparam  TIMEOUT_COUNT    = 400000;

// Clock, reset and simulation control state
reg            clk;
integer        count;

wire  [1:0]    halt;

wire  [7:0]    txd0,  txd1;
wire           txen0, txen1;
wire           txer0, txer1;

wire  [7:0]    rxd0,  rxd1;
wire           rxdv0, rxdv1;
wire           rxer0, rxer1;

wire [3:0]     rgmii_rxd;
wire           rgmii_rxctl;
wire [3:0]     rgmii_txd;
wire           rgmii_txctl;

// -----------------------------------------------
// Initialisation, clock and reset
// -----------------------------------------------

initial
begin
   if (VCD_DUMP != 0)
   begin
     $dumpfile("waves.vcd");
     $dumpvars(0, tb);
   end

   clk                                 = 1'b1;
   count                               = -1;

`ifndef VERILATOR
   #0 // Ensure first x->1 clock edge is complete before initialisation
`endif

   if (DEBUG_STOP != 0)
   begin
     $display("\n***********************************************");
     $display("* Stopping simulation for debugger attachment *");
     $display("***********************************************\n");
     $stop;
   end

   // Generate a clock
   forever #(500000000.0/CLK_FREQ_KHZ) clk = ~clk;
end

// -----------------------------------------------
// Simulation control process
// -----------------------------------------------
always @(posedge clk)
begin
  count                                <= count + 1;

  // Stop/finish the simulations of timeout or a halt signal
  if (count == TIMEOUT_COUNT || |halt == 1'b1)
  begin
    if (GUI_RUN == 0)
    begin
      $finish;
    end
    else
    begin
      $stop;
    end
  end
end

// -----------------------------------------------
// UDP/IPv4 node 0
// -----------------------------------------------

  udp_ip_pg #(.NODE(0)) node0
  (
    .clk                     (clk),

    .txd                     (txd0),
    .txen                    (txen0),
    .txer                    (txer0),

    .rxd                     (rxd0),
    .rxdv                    (rxdv0),
    .rxer                    (rxer0),

    .halt                    (halt[0])
  );


// -----------------------------------------------
// Convert between GMII/RGMII
// -----------------------------------------------

  gmii_rgmii_conv conv0
  (
    .clk                       (clk),
  
    .gmiitxd                   (txd0),
    .gmiitxen                  (txen0),
    .gmiitxer                  (txer0),
  
    .rgmiitxd                  (rgmii_txd),
    .rgmiitxctl                (rgmii_txctl),
  
    .rgmiirxd                  (rgmii_rxd),
    .rgmiirxctl                (rgmii_rxctl),
    
    .gmiirxd                   (rxd0),
    .gmiirxdv                  (rxdv0),
    .gmiirxer                  (rxer0)
  );


// -----------------------------------------------
// Convert between GMII/RGMII
// -----------------------------------------------

  gmii_rgmii_conv conv1
  (
    .clk                       (clk),
  
    .gmiitxd                   (txd1),
    .gmiitxen                  (txen1),
    .gmiitxer                  (txer1),
  
    .rgmiitxd                  (rgmii_rxd),
    .rgmiitxctl                (rgmii_rxctl),
  
    .rgmiirxd                  (rgmii_txd),
    .rgmiirxctl                (rgmii_txctl),
    
    .gmiirxd                   (rxd1),
    .gmiirxdv                  (rxdv1),
    .gmiirxer                  (rxer1)
  );

// -----------------------------------------------
// UDP/IPv4 node 1
// -----------------------------------------------

  udp_ip_pg #(.NODE(1)) node1
  (
    .clk                     (clk),

    .txd                     (txd1),
    .txen                    (txen1),
    .txer                    (txen1),

    .rxd                     (rxd1),
    .rxdv                    (rxdv1),
    .rxer                    (rxer1),

    .halt                    (halt[1])
  );

endmodule