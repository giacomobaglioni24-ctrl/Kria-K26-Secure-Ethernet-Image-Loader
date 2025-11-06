library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity KeyXor is
Port ( 
    KEYXOR_i_KEY: IN std_logic_vector ( 127 downto 0 );
    KEYXOR_i_TEMP: IN std_logic_vector ( 31 downto 0 );
    KEYXOR_o_KEY: OUT std_logic_vector ( 127 downto 0 )
);
end KeyXor;

architecture Behavioral of KeyXor is

signal sv_key_word_0: std_logic_vector ( 31 downto 0 );
signal sv_key_word_1: std_logic_vector ( 31 downto 0 );
signal sv_key_word_2: std_logic_vector ( 31 downto 0 );
signal sv_key_word_3: std_logic_vector ( 31 downto 0 );



begin

sv_key_word_0 <= KEYXOR_i_TEMP xor KEYXOR_i_KEY ( 127 downto 96 ); --595fb705
sv_key_word_1 <= sv_key_word_0 xor KEYXOR_i_KEY ( 95 downto 64 ); --5d5ab102
sv_key_word_2 <= sv_key_word_1 xor KEYXOR_i_KEY ( 63 downto 32 ); --5553bb09
sv_key_word_3 <= sv_key_word_2 xor KEYXOR_i_KEY ( 31 downto 0 ); --595e5506

KEYXOR_o_KEY ( 31 downto 0 ) <= sv_key_word_3; --595e5506
KEYXOR_o_KEY ( 63 downto 32 ) <= sv_key_word_2; --5553bb09
KEYXOR_o_KEY ( 95 downto 64 ) <= sv_key_word_1; -- 5d5ab102
KEYXOR_o_KEY ( 127 downto 96 ) <= sv_key_word_0; --595fb705

end Behavioral;
