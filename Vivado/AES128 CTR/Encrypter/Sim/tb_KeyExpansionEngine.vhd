library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;



entity tb_KeyExpansionEngine is
--Port ();
end tb_KeyExpansionEngine;



architecture Behavioral of tb_KeyExpansionEngine is

    -- Component under test
    component KeyExpansionEngine is
        Port ( 
            KEYEXPANSIONENGINE_i_KEY: IN std_logic_vector (127 downto 0);
            KEYEXPANSIONENGINE_i_KEYNUMBER: IN std_logic_vector(3 downto 0);
--            TEST_1: OUT std_logic_vector ( 31 downto 0 );
--            TEST_2: OUT std_logic_vector ( 31 downto 0 );
--            TEST_3: OUT std_logic_vector ( 31 downto 0 );
            KEYEXPANSIONENGINE_o_ROUNDKEY: OUT std_logic_vector (127 downto 0)
        );
    end component;



    -- Signals
    signal key: std_logic_vector(127 downto 0) := x"00112233445566778899AABBCCDDEEFF";
    signal key_number: std_logic_vector(3 downto 0) := "0001";  -- Round 1
--    signal test_1: std_logic_vector(31 downto 0);
--    signal test_2: std_logic_vector(31 downto 0);
--    signal test_3: std_logic_vector(31 downto 0);
    signal round_key: std_logic_vector(127 downto 0);



begin

    -- DUT instantiation
    DUT: KeyExpansionEngine
        port map (
            KEYEXPANSIONENGINE_i_KEY => key,
            KEYEXPANSIONENGINE_i_KEYNUMBER => key_number,
--            TEST_1 => test_1,
--            TEST_2 => test_2,
--            TEST_3 => test_3,
            KEYEXPANSIONENGINE_o_ROUNDKEY => round_key
        );
       
end Behavioral;
