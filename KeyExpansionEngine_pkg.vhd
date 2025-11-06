library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



package KeyExpansionEngine_pkg is

-- Component RotWord
component RotWord is
    Port ( 
        ROTWORD_i_KEYWORD: IN std_logic_vector (31 downto 0);
        ROTWORD_o_KEYWORD: OUT std_logic_vector (31 downto 0)
    );
end component;

-- Component SubWord
component SubWord is
    Port (
        SUBWORD_i_KEYWORD: IN std_logic_vector(31 downto 0);
        SUBWORD_o_KEYWORD: OUT std_logic_vector(31 downto 0)
    );
end component;

-- Component Rcon
component Rcon is
    Port ( 
        RCON_i_KEYWORD: IN std_logic_vector(31 downto 0);
        RCON_i_KEYNUMBER: IN std_logic_vector(3 downto 0);
        RCON_o_KEYWORD: OUT std_logic_vector(31 downto 0)
    );
end component;

-- Component KeyXor
component KeyXor is
    Port ( 
        KEYXOR_i_KEY: IN std_logic_vector ( 127 downto 0 );
        KEYXOR_i_TEMP: IN std_logic_vector ( 31 downto 0 );
        KEYXOR_o_KEY: OUT std_logic_vector ( 127 downto 0 )
    );
end component;

end package KeyExpansionEngine_pkg;