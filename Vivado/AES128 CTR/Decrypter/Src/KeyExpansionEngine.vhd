library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.KeyExpansionEngine_pkg.all;



entity KeyExpansionEngine is
Port ( 
    KEYEXPANSIONENGINE_i_KEY: IN std_logic_vector ( 127 downto 0 );
    KEYEXPANSIONENGINE_i_KEYNUMBER: IN std_logic_vector( 3 downto 0 );
--    TEST_1: OUT std_logic_vector ( 31 downto 0 );
--    TEST_2: OUT std_logic_vector ( 31 downto 0 );
--    TEST_3: OUT std_logic_vector ( 31 downto 0 );
    KEYEXPANSIONENGINE_o_ROUNDKEY: OUT std_logic_vector ( 127 downto 0 )
);
end KeyExpansionEngine;



architecture Behavioral of KeyExpansionEngine is

signal sv_rot_to_sub: std_logic_vector ( 31 downto 0 );
signal sv_sub_to_rcon: std_logic_vector ( 31 downto 0 );
signal sv_rcon_to_xor: std_logic_vector ( 31 downto 0 );
signal sv_current_key_to_xor: std_logic_vector ( 127 downto 0 );



begin

    Rot_inst: RotWord 
    port map(
        ROTWORD_i_KEYWORD => KEYEXPANSIONENGINE_i_KEY( 31 downto 0 ),
        ROTWORD_o_KEYWORD => sv_rot_to_sub
    );
    
--    TEST_1 <= sv_rot_to_sub;

    Sub_inst : SubWord
        port map(
            SUBWORD_i_KEYWORD => sv_rot_to_sub,
            SUBWORD_o_KEYWORD => sv_sub_to_rcon
        );
        
--    TEST_2 <= sv_sub_to_rcon;

    Rcon_inst : Rcon
        port map(
            RCON_i_KEYWORD     => sv_sub_to_rcon,
            RCON_i_KEYNUMBER   => KEYEXPANSIONENGINE_i_KEYNUMBER,
            RCON_o_KEYWORD     => sv_rcon_to_xor
        );

--    TEST_3 <= sv_rcon_to_xor;

    KeyXor_inst : KeyXor
        port map(
            KEYXOR_i_KEY => KEYEXPANSIONENGINE_i_KEY,
            KEYXOR_i_TEMP => sv_rcon_to_xor,
            KEYXOR_o_KEY => KEYEXPANSIONENGINE_o_ROUNDKEY
        );

end Behavioral;
