library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity KeyExpansionFSM is
Port ( 
    KEYEXPANSIONFSM_i_CLK: IN std_logic;
    KEYEXPANSIONFSM_i_RST: IN std_logic;
--    KEYEXPANSIONFSM_i_EN: IN std_logic;
    KEYEXPANSIONFSM_o_READY: OUT std_logic;   
    KEYEXPANSIONFSM_o_LOAD: OUT std_logic;   
    KEYEXPANSIONFSM_o_KEYNUMBER: OUT std_logic_vector( 3 downto 0 );
    KEYEXPANSIONFSM_o_ADDRESS: OUT std_logic_vector( 3 downto 0 )     
);
end KeyExpansionFSM;



architecture Behavioral of KeyExpansionFSM is

signal sv_state: std_logic_vector (3 downto 0) := "0000";
signal sv_next_state: std_logic_vector (3 downto 0);



begin

STATE_UPDATE: process (KEYEXPANSIONFSM_i_CLK, KEYEXPANSIONFSM_i_RST)      
begin
    if KEYEXPANSIONFSM_i_RST = '1' then
        sv_state <= "0000";
    elsif KEYEXPANSIONFSM_i_CLK = '1' and KEYEXPANSIONFSM_i_CLK' event then 
        sv_state <= sv_next_state;
    end if;
end process;



FSM: process (sv_state)

begin
    
    KEYEXPANSIONFSM_o_READY <= '0';
    KEYEXPANSIONFSM_o_LOAD <= '0';
    KEYEXPANSIONFSM_o_KEYNUMBER <= (others => '0');
    KEYEXPANSIONFSM_o_ADDRESS <= (others => '0');
    
    sv_next_state <= "0000";

    case sv_state is
    
        when "0000" =>

            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '0';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state;

            sv_next_state <= "0001";
            
        when "0001" =>     
        
            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '1';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state;
            
            sv_next_state <= "0010";
            
        when "0010" =>        
        
            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '1';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state;

            sv_next_state <= "0011";
                                
        when "0011" =>

            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '1';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state;        
                
            sv_next_state <= "0100";

        when "0100" =>
            
            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '1';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state; 

            sv_next_state <= "0101";
            
        when "0101" =>

            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '1';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state; 
            
            sv_next_state <= "0110";

        when "0110" =>

            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '1';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state; 

            sv_next_state <= "0111";

        when "0111" =>

            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '1';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state; 

            sv_next_state <= "1000";

        when "1000" =>

            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '1';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state; 

            sv_next_state <= "1001";

        when "1001" =>  

            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '1';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state; 

            sv_next_state <= "1010";

        when "1010" =>

            KEYEXPANSIONFSM_o_READY <= '0';
            KEYEXPANSIONFSM_o_LOAD <= '1';
            KEYEXPANSIONFSM_o_ADDRESS <= sv_state;
            KEYEXPANSIONFSM_o_KEYNUMBER <= sv_state; 

            sv_next_state <= "1011";

        when "1011" =>

            KEYEXPANSIONFSM_o_READY <= '1';
            KEYEXPANSIONFSM_o_LOAD <= '0';
            KEYEXPANSIONFSM_o_ADDRESS <= "0000";
            KEYEXPANSIONFSM_o_KEYNUMBER <= "0000"; 

            sv_next_state <= "1100";

        when "1100" =>

            KEYEXPANSIONFSM_o_READY <= '1';

            sv_next_state <= "1100";

        when "1101" =>

            NULL;

        when "1110" =>

            NULL;

        when "1111" =>

            NULL;

        when others =>

            NULL;

    end case;
    
end process;

end Behavioral;

