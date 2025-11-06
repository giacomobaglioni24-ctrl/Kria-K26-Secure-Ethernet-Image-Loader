library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity tb_AESEngine is
--  Port ( );
end tb_AESEngine;



architecture Behavioral of tb_AESEngine is

component AESEngine is
    Port (
        AESENGINE_i_INPUT  : in  std_logic_vector(127 downto 0);
        AESENGINE_i_KEY    : in  std_logic_vector(127 downto 0);
        AESENGINE_o_OUTPUT : out std_logic_vector(127 downto 0)
    );
end component;

-- Signals for test
signal AESENGINE_i_INPUT  : std_logic_vector(127 downto 0) := (others => '0');
signal AESENGINE_i_KEY    : std_logic_vector(127 downto 0) := (others => '0');
signal AESENGINE_o_OUTPUT : std_logic_vector(127 downto 0);

begin

-- DUT instantiation
DUT: AESEngine
port map (
    AESENGINE_i_INPUT  => AESENGINE_i_INPUT,
    AESENGINE_i_KEY    => AESENGINE_i_KEY,
    AESENGINE_o_OUTPUT => AESENGINE_o_OUTPUT
);


AESENGINE_i_INPUT <= x"00102030405060708090a0b0c0d0e0f0";
AESENGINE_i_KEY   <= x"d6aa74fdd2af72fadaa678f1d6ab76fe";

end Behavioral;
