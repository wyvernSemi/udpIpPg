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

wire  [7:0]    txd;
wire           txen;
wire           txer;
wire  [7:0]    rxd;
wire           rxdv;
wire           rxer;

`ifdef VERILATOR
// This nastiness is needed for Verilator to ensure correct registration of inputs
// using delta cycle reads in VProc.
reg   [7:0]    txd_dly;
reg            txen_dly;
reg            txer_dly;
reg   [7:0]    rxd_dly;
reg            rxdv_dly;
reg            rxer_dly;

// Delay by half a cycle
always @(negedge clk)
begin
    txd_dly  <= txd;
    txen_dly <= txen;
    txer_dly <= txer;
    rxd_dly  <= rxd;
    rxdv_dly <= rxdv;
    rxer_dly <= rxer;
end
`else
// For normal event based simulators, patch signals straight through
// without any delays
wire  [7:0]    txd_dly  = txd;
wire           txen_dly = txen;
wire           txer_dly = txer;
wire  [7:0]    rxd_dly  = rxd;
wire           rxdv_dly = rxdv;
wire           rxer_dly = rxer;
`endif

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

    .txd                     (txd),
    .txen                    (txen),
    .txer                    (txer),

    .rxd                     (rxd_dly),
    .rxdv                    (rxdv_dly),
    .rxer                    (rxer_dly),

    .halt                    (halt[0])
  );

// -----------------------------------------------
// UDP/IPv4 node 1
// -----------------------------------------------

  udp_ip_pg #(.NODE(1)) node1
  (
    .clk                     (clk),
    
    .txd                     (rxd),
    .txen                    (rxdv),
    .txer                    (rxer),

    .rxd                     (txd_dly),
    .rxdv                    (txen_dly),
    .rxer                    (txer_dly),

    .halt                    (halt[1])
  );

endmodule