library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity AESFSM_Decrypter is
Port ( 
    AESFSM_i_CLK: IN std_logic;
    AESFSM_i_RST: IN std_logic;
    
    AESFSM_i_KEYREADY: IN std_logic;
    
    AESFSM_o_COUNTERINC: OUT std_logic;
    AESFSM_o_KEYNUMBER: OUT std_logic_vector ( 3 downto 0 );
    AESFSM_o_BLOCK: OUT std_logic_vector ( 1 downto 0 );
    AESFSM_o_LOADNONCE: OUT std_logic;
    AESFSM_o_LOADINPUT: OUT std_logic;
    AESFSM_o_LOADOUTPUT: OUT std_logic;
    
    s_axis_tready           : OUT STD_LOGIC;
    s_axis_tvalid           : IN STD_LOGIC;
    s_axis_tlast            : IN STD_LOGIC;
    
    m_axis_tready           : IN STD_LOGIC;
    m_axis_tvalid           : OUT STD_LOGIC;
    m_axis_tlast            : OUT STD_LOGIC
);
end AESFSM_Decrypter;



architecture Behavioral of AESFSM_Decrypter is

signal sv_state: std_logic_vector (3 downto 0) := "0000";
signal sv_next_state: std_logic_vector (3 downto 0);
signal sv_s_axis_tready: std_logic;
signal sv_s_axis_tlast: std_logic;



begin

STATE_UPDATE: process (AESFSM_i_CLK, AESFSM_i_RST)      
begin
    if AESFSM_i_RST = '1' then
        sv_state <= ( others => '0' );
    elsif AESFSM_i_CLK = '1' and AESFSM_i_CLK' event then 
        sv_state <= sv_next_state;
    end if;
end process;

FSM: process(sv_state, AESFSM_i_KEYREADY, s_axis_tvalid, sv_s_axis_tlast, m_axis_tready)
begin

    sv_next_state       <= sv_state;

    AESFSM_o_COUNTERINC <= '0';
    AESFSM_o_KEYNUMBER  <= ( others => '0' );
    AESFSM_o_BLOCK      <= ( others => '0' );
    AESFSM_o_LOADNONCE  <= '0';
    AESFSM_o_LOADINPUT  <= '0';
    AESFSM_o_LOADOUTPUT <= '0';

    sv_s_axis_tready       <= '0';

    m_axis_tvalid       <= '0';
    m_axis_tlast        <= '0';
    
    case sv_state is

        when "0000" =>  -- Idle per Key Ready
            
            if AESFSM_i_KEYREADY = '1' then
                sv_next_state <= "0001";
            end if; 

        when "0001" =>  -- Idle per il Nonce
            
            m_axis_tvalid <= '0';
            m_axis_tlast <= '0';

            if s_axis_tvalid = '1' then

                AESFSM_o_LOADNONCE <= '1';
                sv_s_axis_tready <= '1';
                sv_next_state <= "0010";

            else
                sv_next_state <= "0001";
            end if;

        when "0010" =>  -- Idle per il blocco
            
            m_axis_tvalid <= '0';
            m_axis_tlast <= '0';

            sv_s_axis_tready <= '0';

            AESFSM_o_LOADNONCE <= '0';
            
            if s_axis_tvalid = '1' then     

                sv_s_axis_tready <= '1';
                AESFSM_o_LOADINPUT <= '1';
                AESFSM_o_KEYNUMBER <= "0000";
                AESFSM_o_COUNTERINC <= '1';

                sv_next_state <= "0011";
            
            else
                sv_next_state <= "0010";
            end if;

        when "0011" =>  -- Round 1 
            
            sv_s_axis_tready <= '0';

            AESFSM_o_LOADINPUT <= '0';
            AESFSM_o_COUNTERINC <= '0';
            AESFSM_o_KEYNUMBER <= "0001";
            AESFSM_o_BLOCK <= "01";  -- Primo Blocco

            sv_next_state <= "0100";

        when "0100" =>  -- Round 2 
            
            AESFSM_o_KEYNUMBER <= "0010";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "0101";

        when "0101" =>  -- Round 3 

            AESFSM_o_KEYNUMBER <= "0011";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "0110";

        when "0110" =>  -- Round 4 

            AESFSM_o_KEYNUMBER <= "0100";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "0111";

        when "0111" =>  -- Round 5 

            AESFSM_o_KEYNUMBER <= "0101";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "1000";

        when "1000" =>  -- Round 6 

            AESFSM_o_KEYNUMBER <= "0110";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "1001";

        when "1001" =>  -- Round 7
            
            AESFSM_o_KEYNUMBER <= "0111";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "1010";

        when "1010" =>  -- Round 8
            
            AESFSM_o_KEYNUMBER <= "1000";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "1011";

        when "1011" =>  -- Round 9
            
            AESFSM_o_KEYNUMBER <= "1001";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "1100";

        when "1100" =>  -- Round 10  

            AESFSM_o_KEYNUMBER <= "1010";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "1101";
                            
        when "1101" =>  -- Round 11 - Load Output

            AESFSM_o_KEYNUMBER <= "0000";
            AESFSM_o_BLOCK <= "11";  -- Ultimo blocco
            AESFSM_o_LOADOUTPUT <= '1';

            sv_next_state <= "1110";
            
        when "1110" =>  -- Idle per i blocchi successivi 
            
            AESFSM_o_LOADOUTPUT <= '0';

            m_axis_tvalid <= '1';
            m_axis_tlast <= sv_s_axis_tlast;

            if m_axis_tready = '1' then

                if sv_s_axis_tlast = '1' then
                    sv_next_state <= "0001";  -- Torno allo stato iniziale
                else
                    sv_next_state <= "0010";  -- Torno a ricevere un altro blocco
                end if;

            else
                sv_next_state <= "1110";
            end if;

        when others =>
            sv_next_state <= "0000"; -- Stato di default sicuro

    end case;
end process;

s_axis_tready <= sv_s_axis_tready;

-- Flip-Flop per salvataggio  TLAST
TLAST_REGISTER: process(AESFSM_i_CLK, AESFSM_i_RST)
begin
    if AESFSM_i_RST = '1' then
        sv_s_axis_tlast <= '0';
    elsif AESFSM_i_CLK = '1' and AESFSM_i_CLK' event then
        if sv_s_axis_tready = '1' and s_axis_tvalid = '1' then
            sv_s_axis_tlast <= s_axis_tlast;
        end if;
    end if;
end process;

end behavioral;