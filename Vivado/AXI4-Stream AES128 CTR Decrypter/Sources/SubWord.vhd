library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.SBox_pkg.all;



entity SubWord is
Port (
    SUBWORD_i_KEYWORD: IN std_logic_vector( 31 downto 0 );
    SUBWORD_o_KEYWORD: OUT std_logic_vector( 31 downto 0 )
);
end SubWord;



architecture Behavioral of SubWord is

begin

SUBWORD_o_KEYWORD(31 downto 24) <= sbox(to_integer(unsigned(SUBWORD_i_KEYWORD(31 downto 24))));
SUBWORD_o_KEYWORD(23 downto 16) <= sbox(to_integer(unsigned(SUBWORD_i_KEYWORD(23 downto 16))));
SUBWORD_o_KEYWORD(15 downto 8)  <= sbox(to_integer(unsigned(SUBWORD_i_KEYWORD(15 downto 8))));
SUBWORD_o_KEYWORD(7 downto 0)   <= sbox(to_integer(unsigned(SUBWORD_i_KEYWORD(7 downto 0))));

end Behavioral;
