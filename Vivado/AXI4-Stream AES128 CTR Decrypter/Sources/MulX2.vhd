library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity MulX2 is
Port ( 
    MULX2_i_INPUT: IN std_logic_vector ( 7 downto 0 );
    MULX2_o_OUTPUT: OUT std_logic_vector ( 7 downto 0 )
);
end MulX2;



architecture Behavioral of MulX2 is

begin

process (MULX2_i_INPUT)
begin
    if MULX2_i_INPUT(7) = '0' then
        MULX2_o_OUTPUT <= MULX2_i_INPUT(6 downto 0) & '0';
    else
        MULX2_o_OUTPUT <= (MULX2_i_INPUT(6 downto 0) & '0') xor x"1B";
    end if;
end process;


end Behavioral;
