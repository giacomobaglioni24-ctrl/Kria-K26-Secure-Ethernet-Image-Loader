library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity AddRoundKey is
Port ( 
    ADDROUNDKEY_i_INPUT  : IN  std_logic_vector(127 downto 0);
    ADDROUNDKEY_i_KEY    : IN  std_logic_vector(127 downto 0);
    ADDROUNDKEY_o_OUTPUT : OUT std_logic_vector(127 downto 0)
);
end AddRoundKey;

architecture Behavioral of AddRoundKey is

begin

ADDROUNDKEY_o_OUTPUT <= ADDROUNDKEY_i_INPUT xor ADDROUNDKEY_i_KEY;

end Behavioral;
