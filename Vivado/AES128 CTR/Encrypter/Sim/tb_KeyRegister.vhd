library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;



entity tb_KeyRegister is
--  Port ( );
end tb_KeyRegister;



architecture Behavioral of tb_KeyRegister is

component KeyRegister
    Port (
        KEYREGISTER_i_CLK      : in  std_logic;
        KEYREGISTER_i_LOAD     : in  std_logic;
        KEYREGISTER_i_KEY      : in  std_logic_vector(127 downto 0);
        KEYREGISTER_i_ADDRESS  : in  std_logic_vector(3 downto 0);
        KEYREGISTER_o_KEY    : out std_logic_vector(127 downto 0)
        );
end component;



-- Segnali
signal tb_CLK        : std_logic;
signal tb_LOAD       : std_logic;
signal tb_ADDRESS    : std_logic_vector(3 downto 0);
signal tb_KEY_IN     : std_logic_vector(127 downto 0);
signal tb_KEY_OUT    : std_logic_vector(127 downto 0);



begin

clk_process : process
begin
    tb_CLK <= '0';
    wait for 10 ns;
    tb_CLK <= '1';
    wait for 10 ns;
end process;

-- Istanziazione KeyRegister
DUT: KeyRegister
port map (
    KEYREGISTER_i_CLK      => tb_CLK,
    KEYREGISTER_i_LOAD     => tb_LOAD,
    KEYREGISTER_i_KEY      => tb_KEY_IN,
    KEYREGISTER_i_ADDRESS  => tb_ADDRESS,
    KEYREGISTER_o_KEY     => tb_KEY_OUT
);



tb_LOAD <= '0', '1' after 70ns; 
tb_KEY_IN <= x"00112233445566778899AABBCCDDEEFF";
tb_ADDRESS <= "0000", "0011" after 70ns;

end behavioral;