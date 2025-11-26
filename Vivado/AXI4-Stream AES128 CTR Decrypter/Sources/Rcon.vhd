library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;



entity Rcon is
Port ( 
    RCON_i_KEYWORD: IN std_logic_vector( 31 downto 0 );
    RCON_i_KEYNUMBER: IN std_logic_vector( 3 downto 0 );
    RCON_o_KEYWORD: OUT std_logic_vector( 31 downto 0 )
);
end Rcon;



architecture Behavioral of Rcon is

type st_rcon_array is array(0 to 11) of std_logic_vector(7 downto 0);
constant rcon : st_rcon_array := (
    x"00", x"01", x"02", x"04", x"08", x"10", x"20", x"40", x"80", x"1B", x"36", x"00"
);



begin

    RCON_o_KEYWORD(31 downto 24) <= RCON_i_KEYWORD(31 downto 24) xor rcon( to_integer(unsigned(RCON_i_KEYNUMBER)));
    RCON_o_KEYWORD(23 downto 0) <= RCON_i_KEYWORD(23 downto 0);

end Behavioral;
