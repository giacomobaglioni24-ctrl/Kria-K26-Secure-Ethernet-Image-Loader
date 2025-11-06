library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.SBox_pkg.all;



entity SubBytes is
Port ( 
    SUBBYTES_i_INPUT  : IN  std_logic_vector(127 downto 0);
    SUBBYTES_o_OUTPUT : OUT std_logic_vector(127 downto 0)
);
end SubBytes;



architecture Behavioral of SubBytes is


begin

Byte_Substituction: for i in 0 to 15 generate
    SUBBYTES_o_OUTPUT( i*8+7 downto i*8 ) <= sbox(to_integer(unsigned(SUBBYTES_i_INPUT( i*8+7 downto i*8 ))));
end generate;

end Behavioral;
