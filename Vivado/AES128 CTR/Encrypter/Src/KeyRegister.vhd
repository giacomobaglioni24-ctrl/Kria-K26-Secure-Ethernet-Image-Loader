library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;



entity KeyRegister is
Port ( 
    KEYREGISTER_i_CLK: IN std_logic;
    KEYREGISTER_i_LOAD: IN std_logic;
    KEYREGISTER_i_KEY: IN std_logic_vector ( 127 downto 0 );
    KEYREGISTER_i_ADDRESS: IN std_logic_vector ( 3 downto 0 );
    KEYREGISTER_o_KEY  : out std_logic_vector(127 downto 0)
--    KEYREGISTER_o_KEY0  : out std_logic_vector(127 downto 0);
--    KEYREGISTER_o_KEY1  : out std_logic_vector(127 downto 0);
--    KEYREGISTER_o_KEY2  : out std_logic_vector(127 downto 0);
--    KEYREGISTER_o_KEY3  : out std_logic_vector(127 downto 0);
--    KEYREGISTER_o_KEY4  : out std_logic_vector(127 downto 0);
--    KEYREGISTER_o_KEY5  : out std_logic_vector(127 downto 0);
--    KEYREGISTER_o_KEY6  : out std_logic_vector(127 downto 0);
--    KEYREGISTER_o_KEY7  : out std_logic_vector(127 downto 0);
--    KEYREGISTER_o_KEY8  : out std_logic_vector(127 downto 0);
--    KEYREGISTER_o_KEY9  : out std_logic_vector(127 downto 0);
--    KEYREGISTER_o_KEY10 : out std_logic_vector(127 downto 0)
);
end KeyRegister;



architecture Behavioral of KeyRegister is

type st_key_array is array(0 to 15) of std_logic_vector( 127 downto 0 );
signal key_register : st_key_array := (

        0 => x"00112233445566778899AABBCCDDEEFF",
        
        others => (others => '0')
        
    );



begin

WRITEREAD: process(KEYREGISTER_i_CLK) -- vedi se rimettere KEYREGISTER_i_LOAD nella sensitivity list
begin

    if KEYREGISTER_i_CLK = '1' and KEYREGISTER_i_CLK' event then 
    
        if KEYREGISTER_i_LOAD = '1' then
        
            key_register(to_integer(unsigned(KEYREGISTER_i_ADDRESS))) <= KEYREGISTER_i_KEY;

        end if;
        
        KEYREGISTER_o_KEY <= key_register(to_integer(unsigned(KEYREGISTER_i_ADDRESS)));
                
    end if;
    
end process;

-- Tutti gli output insieme
--KEYREGISTER_o_KEY0  <= key_register(0);
--KEYREGISTER_o_KEY1  <= key_register(1);
--KEYREGISTER_o_KEY2  <= key_register(2);
--KEYREGISTER_o_KEY3  <= key_register(3);
--KEYREGISTER_o_KEY4  <= key_register(4);
--KEYREGISTER_o_KEY5  <= key_register(5);
--KEYREGISTER_o_KEY6  <= key_register(6);
--KEYREGISTER_o_KEY7  <= key_register(7);
--KEYREGISTER_o_KEY8  <= key_register(8);
--KEYREGISTER_o_KEY9  <= key_register(9);
--KEYREGISTER_o_KEY10 <= key_register(10);

end Behavioral;

