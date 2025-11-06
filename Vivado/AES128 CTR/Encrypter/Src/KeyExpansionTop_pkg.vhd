library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;



package KeyExpansionTop_pkg is

  -- Component: KeyRegister
  component KeyRegister is
    Port (
      KEYREGISTER_i_CLK     : in  std_logic;
      KEYREGISTER_i_LOAD    : in  std_logic;
      KEYREGISTER_i_KEY     : in  std_logic_vector(127 downto 0);
      KEYREGISTER_i_ADDRESS : in  std_logic_vector(3 downto 0);
      KEYREGISTER_o_KEY     : out std_logic_vector(127 downto 0)
    );
  end component;

  -- Component: KeyExpansionEngine
  component KeyExpansionEngine is
    Port (
      KEYEXPANSIONENGINE_i_KEY       : in  std_logic_vector(127 downto 0);
      KEYEXPANSIONENGINE_i_KEYNUMBER : in  std_logic_vector(3 downto 0);
      KEYEXPANSIONENGINE_o_ROUNDKEY  : out std_logic_vector(127 downto 0)
    );
  end component;

  -- Component: KeyExpansionFSM
  component KeyExpansionFSM is
    Port (
      KEYEXPANSIONFSM_i_CLK       : in  std_logic;
      KEYEXPANSIONFSM_i_RST       : in  std_logic;
      KEYEXPANSIONFSM_o_READY     : out std_logic;
      KEYEXPANSIONFSM_o_LOAD        : out std_logic;
      KEYEXPANSIONFSM_o_KEYNUMBER : out std_logic_vector(3 downto 0);
      KEYEXPANSIONFSM_o_ADDRESS   : out std_logic_vector(3 downto 0)
    );
  end component;

end package KeyExpansionTop_pkg;