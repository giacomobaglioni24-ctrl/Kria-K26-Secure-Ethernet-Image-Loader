library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



package AESEngine_pkg is

    -- AddRoundKey component
    component AddRoundKey is
        Port (
            ADDROUNDKEY_i_INPUT  : in  std_logic_vector(127 downto 0);
            ADDROUNDKEY_i_KEY    : in  std_logic_vector(127 downto 0);
            ADDROUNDKEY_o_OUTPUT : out std_logic_vector(127 downto 0)
        );
    end component;

    -- ShiftRows component
    component ShiftRows is
        Port (
            SHIFTROWS_i_INPUT  : in  std_logic_vector(127 downto 0);
            SHIFTROWS_o_OUTPUT : out std_logic_vector(127 downto 0)
        );
    end component;

    -- SubBytes component
    component SubBytes is
        Port (
            SUBBYTES_i_INPUT  : in  std_logic_vector(127 downto 0);
            SUBBYTES_o_OUTPUT : out std_logic_vector(127 downto 0)
        );
    end component;

    -- MixColunms component
    component MixColunms is
        Port (
            MIXCOLUNMS_i_INPUT  : in  std_logic_vector(127 downto 0);
            MIXCOLUNMS_o_OUTPUT : out std_logic_vector(127 downto 0)
        );
    end component;

end package AESEngine_pkg;

