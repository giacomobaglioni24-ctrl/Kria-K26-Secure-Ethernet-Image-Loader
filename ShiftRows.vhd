library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity ShiftRows is
Port (
    SHIFTROWS_i_INPUT  : IN  std_logic_vector(127 downto 0);    
    SHIFTROWS_o_OUTPUT : OUT std_logic_vector(127 downto 0)
);
end ShiftRows;



architecture Behavioral of ShiftRows is

begin

-- Riga 0 (nessuno shift)
SHIFTROWS_o_OUTPUT(127 downto 120) <= SHIFTROWS_i_INPUT(127 downto 120); -- b0
SHIFTROWS_o_OUTPUT( 95 downto  88) <= SHIFTROWS_i_INPUT( 95 downto  88); -- b4
SHIFTROWS_o_OUTPUT( 63 downto  56) <= SHIFTROWS_i_INPUT( 63 downto  56); -- b8
SHIFTROWS_o_OUTPUT( 31 downto  24) <= SHIFTROWS_i_INPUT( 31 downto  24); -- b12

-- Riga 1 (shift di 1)
SHIFTROWS_o_OUTPUT(119 downto 112) <= SHIFTROWS_i_INPUT( 87 downto  80); -- b5
SHIFTROWS_o_OUTPUT( 87 downto  80) <= SHIFTROWS_i_INPUT( 55 downto  48); -- b9
SHIFTROWS_o_OUTPUT( 55 downto  48) <= SHIFTROWS_i_INPUT( 23 downto  16); -- b13
SHIFTROWS_o_OUTPUT( 23 downto  16) <= SHIFTROWS_i_INPUT(119 downto 112); -- b1

-- Riga 2 (shift di 2)
SHIFTROWS_o_OUTPUT(111 downto 104) <= SHIFTROWS_i_INPUT( 47 downto  40); -- b10
SHIFTROWS_o_OUTPUT( 79 downto  72) <= SHIFTROWS_i_INPUT( 15 downto   8); -- b14
SHIFTROWS_o_OUTPUT( 47 downto  40) <= SHIFTROWS_i_INPUT(111 downto 104); -- b2
SHIFTROWS_o_OUTPUT( 15 downto   8) <= SHIFTROWS_i_INPUT( 79 downto  72); -- b6

-- Riga 3 (shift di 3)
SHIFTROWS_o_OUTPUT(103 downto  96) <= SHIFTROWS_i_INPUT(  7 downto   0); -- b15
SHIFTROWS_o_OUTPUT( 71 downto  64) <= SHIFTROWS_i_INPUT(103 downto  96); -- b3
SHIFTROWS_o_OUTPUT( 39 downto  32) <= SHIFTROWS_i_INPUT( 71 downto  64); -- b7
SHIFTROWS_o_OUTPUT(  7 downto   0) <= SHIFTROWS_i_INPUT( 39 downto  32); -- b11

end Behavioral;
