library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;



entity KeyRegister is
Generic (
    ROUND_KEY: std_logic_vector(127 downto 0)
    );
Port ( 
    KEYREGISTER_i_CLK: IN std_logic;
    KEYREGISTER_i_LOAD: IN std_logic;
    KEYREGISTER_i_KEY: IN std_logic_vector ( 127 downto 0 );
    KEYREGISTER_i_ADDRESS: IN std_logic_vector ( 3 downto 0 );
    KEYREGISTER_o_KEY  : out std_logic_vector(127 downto 0)
);
end KeyRegister;



architecture Behavioral of KeyRegister is

type st_key_array is array(0 to 15) of std_logic_vector( 127 downto 0 );
signal key_register : st_key_array := (

        0 => ROUND_KEY,
        
        others => (others => '0')
        
    );



begin

WRITEREAD: process(KEYREGISTER_i_CLK)
begin

    if KEYREGISTER_i_CLK = '1' and KEYREGISTER_i_CLK' event then 
    
        if KEYREGISTER_i_LOAD = '1' then
        
            key_register(to_integer(unsigned(KEYREGISTER_i_ADDRESS))) <= KEYREGISTER_i_KEY;

        end if;
        
        KEYREGISTER_o_KEY <= key_register(to_integer(unsigned(KEYREGISTER_i_ADDRESS)));
                
    end if;
    
end process;

end Behavioral;

