library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity tb_KeyExtensionTop is
--Port ( );
end tb_KeyExtensionTop;



architecture Behavioral of tb_KeyExtensionTop is

component KeyExpansionTop is
  Port (
    KEYEXPASIONTOP_i_CLK       : in  std_logic;
    KEYEXPASIONTOP_i_RST       : in  std_logic;
    KEYEXPASIONTOP_i_KEYNUMBER : in  std_logic_vector(3 downto 0);
    KEYEXPASIONTOP_o_READY     : out std_logic;
    KEYEXPASIONTOP_o_KEY       : out std_logic_vector(127 downto 0)
    );
end component;

signal clk: std_logic;
signal rst: std_logic;
signal key_number: std_logic_vector ( 3 downto 0 );
signal ready: std_logic;
signal key: std_logic_vector ( 127 downto 0 );



begin

DUT: KeyExpansionTop
port map (
  KEYEXPASIONTOP_i_CLK       => clk,
  KEYEXPASIONTOP_i_RST       => rst,
  KEYEXPASIONTOP_i_KEYNUMBER => key_number,
  KEYEXPASIONTOP_o_READY     => ready,
  KEYEXPASIONTOP_o_KEY       => key
);

clk_process : process
begin
    clk <= '0';
    wait for 10 ns;
    clk <= '1';
    wait for 10 ns;
end process;

rst <= '0', '1' after 10 ns, '0' after 40 ns;
key_number <= "0100", "0000" after 300 ns;

end Behavioral;
