library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity MulX3 is
Port ( 
    MULX3_i_INPUT: IN std_logic_vector ( 7 downto 0 );
    MULX3_i_MULX2: IN std_logic_vector ( 7 downto 0 );
    MULX3_o_OUTPUT: OUT std_logic_vector ( 7 downto 0 )
);
end MulX3;



architecture Behavioral of MulX3 is

begin

MULX3_o_OUTPUT <= MULX3_i_INPUT xor MULX3_i_MULX2;

end Behavioral;
