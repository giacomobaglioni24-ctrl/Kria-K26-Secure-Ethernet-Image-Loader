library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity AESFSM is
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
end AESFSM;



architecture Behavioral of AESFSM is

signal sv_state: std_logic_vector (4 downto 0) := "00000";
signal sv_next_state: std_logic_vector (4 downto 0);
signal sv_s_axis_tlast: std_logic;



begin

STATE_UPDATE: process (AESFSM_i_CLK)      
begin
    if AESFSM_i_RST = '1' then
        sv_state <= ( others => '0' );
    elsif AESFSM_i_CLK = '1' and AESFSM_i_CLK' event then 
        sv_state <= sv_next_state;
    end if;
end process;

FSM: process(sv_state, AESFSM_i_RST, AESFSM_i_CLK)
begin

    if AESFSM_i_RST = '1' then
    
        AESFSM_o_COUNTERINC <= '0';
        AESFSM_o_KEYNUMBER <= ( others => '0' );
        AESFSM_o_BLOCK <= ( others => '0' );
        AESFSM_o_LOADNONCE <= '0';
        AESFSM_o_LOADINPUT <= '0';
        AESFSM_o_LOADOUTPUT <= '0';
        
        s_axis_tready <= '0';
        
        m_axis_tvalid <= '0';
        m_axis_tlast <= '0';
    
    end if;
    
    case sv_state is

        when "00000" =>  -- Idle per Key Ready
            
            if AESFSM_i_KEYREADY = '1' then
                sv_next_state <= "00001";
            else
                sv_next_state <= "00000";
            end if; 

        when "00001" =>  -- Idle per il Nonce
            
            m_axis_tvalid <= '0';
            m_axis_tlast <= '0';

            if s_axis_tvalid = '1' then

                AESFSM_o_COUNTERINC <= '1';
                AESFSM_o_KEYNUMBER <= "0000";
                AESFSM_o_LOADNONCE <= '1';

                s_axis_tready <= '1';
                sv_s_axis_tlast <= s_axis_tlast;

                sv_next_state <= "00010";

            else

                sv_next_state <= "00001";

            end if;

        when "00010" =>  -- Round 1

            s_axis_tready <= '0';

            AESFSM_o_LOADNONCE <= '0';
            AESFSM_o_COUNTERINC <= '0';
            
            if s_axis_tvalid = '1' then        

                AESFSM_o_KEYNUMBER <= "0001";
                AESFSM_o_BLOCK <= "01";  -- Primo blocco

                sv_s_axis_tlast <= s_axis_tlast;

                sv_next_state <= "00011";
            
            else

                sv_next_state <= "00010";

            end if;

        when "00011" =>  -- Blocco il t_ready e inizio il Round 2
            
            --s_axis_tready <= '0';

            AESFSM_o_KEYNUMBER <= "0010";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "00100";

        when "00100" =>  -- Round 3
            
            AESFSM_o_KEYNUMBER <= "0011";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "00101";

        when "00101" =>  -- Round 4

            AESFSM_o_KEYNUMBER <= "0100";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "00110";

        when "00110" =>  -- Round 5

            AESFSM_o_KEYNUMBER <= "0101";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "00111";

        when "00111" =>  -- Round 6

            AESFSM_o_KEYNUMBER <= "0110";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "01000";

        when "01000" =>  -- Round 7

            AESFSM_o_KEYNUMBER <= "0111";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "01001";

        when "01001" =>  -- Round 8
            
            AESFSM_o_KEYNUMBER <= "1000";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "01010";

        when "01010" =>  -- Round 9
            
            AESFSM_o_KEYNUMBER <= "1001";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "01011";

        when "01011" =>  -- Round 10
            
            AESFSM_o_KEYNUMBER <= "1010";
            AESFSM_o_BLOCK <= "10";  -- Blocchi intermedi

            sv_next_state <= "01100";

        when "01100" =>  -- Round 11 - Load Output  

            AESFSM_o_KEYNUMBER <= "0000";
            AESFSM_o_BLOCK <= "11";  -- Ultimo blocco
            AESFSM_o_LOADOUTPUT <= '1';

            sv_next_state <= "01101";
                            
        when "01101" =>  -- Aspetto m_axis_tready 

            AESFSM_o_KEYNUMBER <= "0000";
            AESFSM_o_LOADOUTPUT <= '0';
            
            m_axis_tvalid <= '1';
            m_axis_tlast <= sv_s_axis_tlast;

            if m_axis_tready = '1' then

                if sv_s_axis_tlast = '1' then

                    sv_next_state <= "00001";  -- Torno allo stato iniziale

                else

                    sv_next_state <= "01110";  -- Torno a ricevere un altro blocco

                end if;

            else

                sv_next_state <= "01101";

            end if;

        when "01110" =>  -- Idle per i blocchi successivi
            
            m_axis_tvalid <= '0';
            m_axis_tlast <= '0';

            if s_axis_tvalid = '1' then

                s_axis_tready <= '1';

                AESFSM_o_COUNTERINC <= '1';

                sv_next_state <= "00010";

            else

                sv_next_state <= "01110";

            end if;

        when "01111" =>  -- Stato 15
            NULL;

        when "10000" =>  -- Stato 16
            NULL;

        when "10001" =>  -- Stato 17
            NULL;

        when "10010" =>  -- Stato 18
            NULL;

        when "10011" =>  -- Stato 19
            NULL;

        when "10100" =>  -- Stato 20
            NULL;

        when "10101" =>  -- Stato 21
            NULL;

        when "10110" =>  -- Stato 22
            NULL;

        when "10111" =>  -- Stato 23
            NULL;

        when "11000" =>  -- Stato 24
            NULL;

        when "11001" =>  -- Stato 25
            NULL;

        when "11010" =>  -- Stato 26
            NULL;

        when "11011" =>  -- Stato 27
            NULL;

        when "11100" =>  -- Stato 28
            NULL;

        when "11101" =>  -- Stato 29
            NULL;

        when "11110" =>  -- Stato 30
            NULL;

        when "11111" =>  -- Stato 31
            NULL;

        when others =>
            NULL;

    end case;
end process;


end Behavioral;
