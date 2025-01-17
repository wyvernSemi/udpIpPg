-- -----------------------------------------------------------------------------
--  Title      : Test bench for UDP/IPv4 packet generator
--  Project    : udp_ip_pg
-- -----------------------------------------------------------------------------
--  File       : tbUdp.vhd
--  Author     : Simon Southwell
--  Created    : 2025-01-03
--  Standard   : Verilog 2001
-- -----------------------------------------------------------------------------
--  Description:
--  This block defines the top level test bench for the UDP/IP packet generator.
--  Checkout repo to same folder as VProc (github.com/wyvernSemi/vproc)
-- -----------------------------------------------------------------------------
--  Copyright (c) 2025 Simon Southwell
-- -----------------------------------------------------------------------------
--
--  This is free software: you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation(), either version 3 of the License(), or
--  (at your option) any later version.
--
--  It is distributed in the hope that it will be useful(),
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this code. If not, see <http://www.gnu.org/licenses/>.
--
-- -----------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

library std;
use std.env.all;

entity tb is
generic (GUI_RUN          : integer := 0;
         CLK_FREQ_KHZ     : real    := 125000.0;
         VCD_DUMP         : integer := 0;
         DEBUG_STOP       : integer := 0
  );
end entity;

architecture sim of tb is

constant RESET_PERIOD     : integer := 10;
constant TIMEOUT_COUNT    : integer := 400000;
constant CLK_PERIOD       : time    := 1 ms / CLK_FREQ_KHZ;

-- Clock, reset and simulation control state
signal         clk        : std_logic := '1';
signal         count      : integer   := -1;

signal         txd0       : std_logic_vector( 7 downto 0);
signal         txen0      : std_logic;
signal         txer0      : std_logic;
signal         rxd0       : std_logic_vector( 7 downto 0);
signal         rxdv0      : std_logic;
signal         rxer0      : std_logic;

signal         txd1       : std_logic_vector( 7 downto 0);
signal         txen1      : std_logic;
signal         txer1      : std_logic;
signal         rxd1       : std_logic_vector( 7 downto 0);
signal         rxdv1      : std_logic;
signal         rxer1      : std_logic;


signal        rgmii_rxd   : std_logic_vector(3 downto 0);
signal        rgmii_rxctl : std_logic;
signal        rgmii_txd   : std_logic_vector(3 downto 0);
signal        rgmii_txctl : std_logic;

signal         halt       : std_logic_vector( 1 downto 0);

begin
-- -----------------------------------------------
-- Generate a clock
-- -----------------------------------------------

  P_CLKGEN : process
  begin
    -- Generate a clock cycle
    loop
      clk                              <= '1';
      wait for CLK_PERIOD/2.0;
      clk                              <= '0';
      wait for CLK_PERIOD/2.0;
    end loop;
  end process;

-- -----------------------------------------------
-- Simulation control process
-- -----------------------------------------------

  P_SIM_CTRL : process(clk)
  begin
    if clk'event and clk = '1' then
      count                            <= count + 1;

      if count >= TIMEOUT_COUNT or halt /= 2x"00" then
        if GUI_RUN = 0 then
          finish(0);
        else
          stop(0);
        end if;
      end if;
    end if;
  end process;

-- -----------------------------------------------
-- UDP/IPv4 node 0
-- -----------------------------------------------

  node0 : entity work.udp_ip_pg
  generic map (
    NODE_NUM                 => 0
  )
  port map (
     clk                     => clk,

     txd                     => txd0,
     txen                    => txen0,
     txer                    => txer0,

     rxd                     => rxd0,
     rxdv                    => rxdv0,
     rxer                    => rxer0,

     halt                    => halt(0)
  );

-- -----------------------------------------------
-- Convert between GMII/RGMII
-- -----------------------------------------------

  conv0 : entity work.gmii_rgmii_conv
  port map (
     clk                     => clk,

     gmiitxd                 => txd0,
     gmiitxen                => txen0,
     gmiitxer                => txer0,

     rgmiitxd                => rgmii_txd,
     rgmiitxctl              => rgmii_txctl,

     rgmiirxd                => rgmii_rxd,
     rgmiirxctl              => rgmii_rxctl,

     gmiirxd                 => rxd0,
     gmiirxdv                => rxdv0,
     gmiirxer                => rxer0
  );

-- -----------------------------------------------
-- Convert between GMII/RGMII
-- -----------------------------------------------

  conv1 : entity work.gmii_rgmii_conv
  port map (
     clk                     => clk,

     gmiitxd                 => txd1,
     gmiitxen                => txen1,
     gmiitxer                => txer1,

     rgmiitxd                => rgmii_rxd,
     rgmiitxctl              => rgmii_rxctl,

     rgmiirxd                => rgmii_txd,
     rgmiirxctl              => rgmii_txctl,

     gmiirxd                 => rxd1,
     gmiirxdv                => rxdv1,
     gmiirxer                => rxer1
  );

-- -----------------------------------------------
-- UDP/IPv4 node 1
-- -----------------------------------------------

  node1 : entity work.udp_ip_pg
  generic map (
    NODE_NUM                 => 1
  )
  port map (
     clk                     => clk,

     txd                     => txd1,
     txen                    => txen1,
     txer                    => txer1,

     rxd                     => rxd1,
     rxdv                    => rxdv1,
     rxer                    => rxer1,

     halt                    => halt(1)
  );

end architecture;