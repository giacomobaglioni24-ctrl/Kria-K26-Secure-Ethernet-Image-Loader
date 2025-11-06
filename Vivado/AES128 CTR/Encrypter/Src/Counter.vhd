library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.std_logic_unsigned.all;



entity Counter is
Port ( 
    COUNTER_i_CLK: IN std_logic;
    COUNTER_i_RST: IN std_logic;
    COUNTER_i_INC: IN std_logic;
    COUNTER_o_OUTPUT: OUT std_logic_vector ( 63 downto 0 )
);
end Counter;



architecture Behavioral of Counter is

signal sv_counter: std_logic_vector ( 63 downto 0 );



begin

AES_Counter: process (COUNTER_i_CLK)
begin
    if COUNTER_i_CLK = '1' and COUNTER_i_CLK' event then 
        if COUNTER_i_RST = '1' then
            sv_counter <= ( others => '0' );
        elsif COUNTER_i_INC = '1' then
            sv_counter <= sv_counter + 1;
        end if;
    end if;
end process;

COUNTER_o_OUTPUT <= sv_counter;

end Behavioral;
