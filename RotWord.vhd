library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity RotWord is
Port ( 
    ROTWORD_i_KEYWORD: IN std_logic_vector ( 31 downto 0 );
    ROTWORD_o_KEYWORD: OUT std_logic_vector ( 31 downto 0 )
);
end RotWord;



architecture Behavioral of RotWord is

begin

ROTWORD_o_KEYWORD <= ROTWORD_i_KEYWORD(23 downto 16) & 
                     ROTWORD_i_KEYWORD(15 downto 8)  & 
                     ROTWORD_i_KEYWORD(7 downto 0)   & 
                     ROTWORD_i_KEYWORD(31 downto 24);

end Behavioral;
