library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.KeyExpansionTop_pkg.all;



entity KeyExpansionTop is
Port ( 
    KEYEXPASIONTOP_i_CLK: IN std_logic;
    KEYEXPASIONTOP_i_RST: IN std_logic;  -- Key expand start ?
    KEYEXPASIONTOP_i_KEYNUMBER: IN std_logic_vector ( 3 downto 0 );
    KEYEXPASIONTOP_o_READY: OUT std_logic;
    KEYEXPASIONTOP_o_KEY: OUT std_logic_vector ( 127 downto 0 )
);
end KeyExpansionTop;



architecture Behavioral of KeyExpansionTop is

signal sv_ready: std_logic;
signal sv_load: std_logic;
signal sv_save: std_logic;
signal sv_key_number: std_logic_vector ( 3 downto 0 );
signal sv_key_address: std_logic_vector ( 3 downto 0 );
signal sv_key_address_to_register: std_logic_vector ( 3 downto 0 );
signal sv_current_key: std_logic_vector ( 127 downto 0 ) := ( others => '0' );
signal sv_processed_key: std_logic_vector ( 127 downto 0 );
signal sv_output_key: std_logic_vector ( 127 downto 0 );


begin

KeyRegister_Inst : KeyRegister
port map (
  KEYREGISTER_i_CLK      => KEYEXPASIONTOP_i_CLK,
  KEYREGISTER_i_LOAD     => sv_load,
  KEYREGISTER_i_KEY      => sv_processed_key,
  KEYREGISTER_i_ADDRESS  => sv_key_address_to_register,
  KEYREGISTER_o_KEY      => sv_output_key
);

KeyExpansionEngine_Inst : KeyExpansionEngine
port map (
  KEYEXPANSIONENGINE_i_KEY       => sv_current_key,
  KEYEXPANSIONENGINE_i_KEYNUMBER => sv_key_number,
  KEYEXPANSIONENGINE_o_ROUNDKEY  => sv_processed_key
);

KeyExpansionFSM_Inst : KeyExpansionFSM
port map (
  KEYEXPANSIONFSM_i_CLK       => KEYEXPASIONTOP_i_CLK,
  KEYEXPANSIONFSM_i_RST       => KEYEXPASIONTOP_i_RST,
  KEYEXPANSIONFSM_o_READY     => sv_ready,
  KEYEXPANSIONFSM_o_LOAD        => sv_load,
  KEYEXPANSIONFSM_o_SAVE    =>  sv_save,
  KEYEXPANSIONFSM_o_KEYNUMBER => sv_key_number,
  KEYEXPANSIONFSM_o_ADDRESS   => sv_key_address
);



MUX_Address: process(sv_ready, sv_key_address, KEYEXPASIONTOP_i_KEYNUMBER)
begin   
    if sv_ready = '1' then
        sv_key_address_to_register <= KEYEXPASIONTOP_i_KEYNUMBER;
    else
        sv_key_address_to_register <= sv_key_address;
    end if;
end process MUX_Address;



FF_Key: process(KEYEXPASIONTOP_i_CLK)
begin
    if KEYEXPASIONTOP_i_CLK = '1' and KEYEXPASIONTOP_i_CLK' event then
        if KEYEXPASIONTOP_i_RST = '1' then
            sv_current_key <= ( others => '0' );
        elsif sv_save = '1' then
            if sv_load = '1' then
                sv_current_key <= sv_processed_key;
            else
                sv_current_key <= sv_output_key;
            end if;
        end if;
    end if; 
end process FF_Key;



KEYEXPASIONTOP_o_READY <= sv_ready;
KEYEXPASIONTOP_o_KEY <= sv_output_key;

end Behavioral;
