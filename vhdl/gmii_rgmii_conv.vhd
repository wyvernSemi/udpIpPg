
-- GMII - RGMII convertor
--
-- Copyright (c) 2025 Simon Southwell.
--
-- This file is part of udp_ip_pg.
--
-- This code is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- The code is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this code. If not, see <http://www.gnu.org/licenses/>.
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- ============================================
--  MODULE
-- ============================================

entity gmii_rgmii_conv is
port (
  clk                                 : in  std_logic;

  -- GMII to RGMII
  gmiitxd                             : in  std_logic_vector(7 downto 0) := 8x"00";
  gmiitxen                            : in  std_logic                    := '0';
  gmiitxer                            : in  std_logic                    := '0';

  rgmiitxd                            : out std_logic_vector(3 downto 0) := 4x"0";
  rgmiitxctl                          : out std_logic                    := '0';

  -- RGMII to GMII
  rgmiirxd                            : in  std_logic_vector(3 downto 0) := 4x"0";
  rgmiirxctl                          : in  std_logic                    := '0';

  gmiirxd                             : out std_logic_vector(7 downto 0) := 8x"00";
  gmiirxdv                            : out std_logic                    := '0';
  gmiirxer                            : out std_logic                    := '0'
);
end entity;

architecture behavioural of gmii_rgmii_conv is

constant HoldDly                      : time := 1 ns;

signal   gmiirxdlo                    : std_logic_vector(3 downto 0)     := 4x"0";
signal   gmiirxdhi                    : std_logic_vector(3 downto 0)     := 4x"0";
signal   gmiirxdv_int                 : std_logic                        := '0';
signal   gmiirxer_int                 : std_logic                        := '0';

signal   gmiirxdneg                   : std_logic_vector(7 downto 0)     := 8x"00";
signal   gmiirxdvneg                  : std_logic                        := '0';
signal   gmiirxerneg                  : std_logic                        := '0';

begin

-- Time-division multiplex the GMII TX input onto RGMII
-- with a hold delay (neededsince using clock as mux
-- control, and must ensure signals are sample corrrectly
-- at destination).
  rgmiitxd         <= gmiitxd(3 downto 0) when clk = '1' else gmiitxd(7 downto 4) after HoldDly;
  rgmiitxctl       <= gmiitxen            when clk = '1' else gmiitxer            after HoldDly;

  process (clk)
  begin
    if clk'event and clk = '1' then
      -- Sample high nibble RX input
      gmiirxdhi    <= rgmiirxd;
      gmiirxer_int <= rgmiirxctl;

      -- RX output alignment stage on rising edge1
      gmiirxd      <= gmiirxdneg;
      gmiirxdv     <= gmiirxdvneg;
      gmiirxer     <= gmiirxerneg;
    end if;
  end process;

  process (clk)
  begin
    if clk'event and clk = '0' then
      -- Sample low nibble RX input
      gmiirxdlo    <= rgmiirxd;
      gmiirxdv_int <= rgmiirxctl;

      -- Construct wide GMII RX on falling egde
      gmiirxdneg   <= gmiirxdhi & gmiirxdlo;
      gmiirxdvneg  <= gmiirxdv_int;
      gmiirxerneg  <= gmiirxer_int;
    end if;
  end process;

end behavioural;