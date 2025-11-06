library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity AES128CTR is
Port ( 

    AES128CTR_i_CLK: IN std_logic;
    AES128CTR_i_RST: IN std_logic;
    
    -- Plaintext Input
    s_axis_tready           : OUT std_logic;
    s_axis_tvalid           : IN std_logic;
    s_axis_tlast            : IN std_logic;
    s_axis_tdata            : IN std_logic_vector(127 downto 0);
    
    -- Ciphertext Output
    m_axis_tready           : IN std_logic;
    m_axis_tvalid           : OUT std_logic;
    m_axis_tlast            : OUT std_logic;
    m_axis_tdata            : OUT std_logic_vector(127 downto 0)
    
);
end AES128CTR;



architecture Behavioral of AES128CTR is

component KeyExpansionTop
Port (
    KEYEXPASIONTOP_i_CLK       : IN std_logic;
    KEYEXPASIONTOP_i_RST       : IN std_logic;
    KEYEXPASIONTOP_i_KEYNUMBER : IN std_logic_vector(3 downto 0);
    KEYEXPASIONTOP_o_READY     : OUT std_logic;
    KEYEXPASIONTOP_o_KEY       : OUT std_logic_vector(127 downto 0)
);
end component;

component Counter
Port (
    COUNTER_i_CLK     : IN std_logic;
    COUNTER_i_RST     : IN std_logic;
    COUNTER_i_INC     : IN std_logic;
    COUNTER_o_OUTPUT  : OUT std_logic_vector(63 downto 0)
);
end component;

component AESEngine
Port (
    AESENGINE_i_INPUT  : IN std_logic_vector(127 downto 0);
    AESENGINE_i_KEY    : IN std_logic_vector(127 downto 0);
    AESENGINE_i_BLOCK  : IN std_logic_vector(1 downto 0);
    AESENGINE_o_OUTPUT : OUT std_logic_vector(127 downto 0)
);
end component;

component AESFSM
Port (
    AESFSM_i_CLK         : IN std_logic;
    AESFSM_i_RST         : IN std_logic;
    AESFSM_i_KEYREADY    : IN std_logic;
    AESFSM_o_COUNTERINC  : OUT std_logic;
    AESFSM_o_KEYNUMBER   : OUT std_logic_vector(3 downto 0);
    AESFSM_o_BLOCK       : OUT std_logic_vector(1 downto 0);
    AESFSM_o_LOADNONCE: OUT std_logic;
    AESFSM_o_LOADINPUT: OUT std_logic;
    AESFSM_o_LOADOUTPUT: OUT std_logic;
    s_axis_tready        : OUT std_logic;
    s_axis_tvalid        : IN std_logic;
    s_axis_tlast         : IN std_logic;
    m_axis_tready        : IN std_logic;
    m_axis_tvalid        : OUT std_logic;
    m_axis_tlast         : OUT std_logic
);
end component;



-- KeyExpansionTop
signal sv_keynumber         : std_logic_vector(3 downto 0);
signal sv_key_ready         : std_logic;
signal sv_key           : std_logic_vector(127 downto 0);

-- Counter
signal sv_counter_inc       : std_logic;
signal sv_counter_out       : std_logic_vector(63 downto 0);

-- AESEngine
signal sv_aes_input         : std_logic_vector(127 downto 0);
signal sv_aes_key           : std_logic_vector(127 downto 0);
signal sv_aes_block         : std_logic_vector(1 downto 0);
signal sv_aes_output        : std_logic_vector(127 downto 0);

-- AESFSM
signal sv_fsm_keyready      : std_logic;
signal sv_fsm_counterinc    : std_logic;
signal sv_fsm_keynumber     : std_logic_vector(3 downto 0);
signal sv_fsm_block         : std_logic_vector(1 downto 0);
signal sv_fsm_loadnonce     : std_logic;
signal sv_fsm_loadinput     : std_logic;
signal sv_fsm_loadoutput    : std_logic;

signal sv_nonce     : std_logic_vector(63 downto 0);
signal sv_aes_feedback : std_logic_vector(127 downto 0);
signal sv_s_axis_tdata : std_logic_vector(127 downto 0);
signal sv_m_axis_tdata : std_logic_vector(127 downto 0);



begin

-- Key Expansion
KEYEXPANSION_Inst : KeyExpansionTop
port map (
    KEYEXPASIONTOP_i_CLK       => AES128CTR_i_CLK,
    KEYEXPASIONTOP_i_RST       => AES128CTR_i_RST,
    KEYEXPASIONTOP_i_KEYNUMBER => sv_keynumber,
    KEYEXPASIONTOP_o_READY     => sv_key_ready,
    KEYEXPASIONTOP_o_KEY       => sv_key
);

-- Counter
COUNTER_Inst : Counter
port map (
    COUNTER_i_CLK    => AES128CTR_i_CLK,
    COUNTER_i_RST    => AES128CTR_i_RST,
    COUNTER_i_INC    => sv_counter_inc,
    COUNTER_o_OUTPUT => sv_counter_out
);

-- AES Engine
AESENGINE_Inst : AESEngine
port map (
    AESENGINE_i_INPUT  => sv_aes_input,
    AESENGINE_i_KEY    => sv_key, 
    AESENGINE_i_BLOCK  => sv_fsm_block,
    AESENGINE_o_OUTPUT => sv_aes_output
);

-- AES FSM
AESFSM_Inst : AESFSM
port map (
    AESFSM_i_CLK         => AES128CTR_i_CLK,
    AESFSM_i_RST         => AES128CTR_i_RST,
    AESFSM_i_KEYREADY    => sv_key_ready,
    AESFSM_o_COUNTERINC  => sv_counter_inc,
    AESFSM_o_KEYNUMBER   => sv_keynumber,
    AESFSM_o_BLOCK       => sv_fsm_block,
    AESFSM_o_LOADNONCE   => sv_fsm_loadnonce,
    AESFSM_o_LOADINPUT   => sv_fsm_loadinput,
    AESFSM_o_LOADOUTPUT  => sv_fsm_loadoutput,
    s_axis_tready        => s_axis_tready,
    s_axis_tvalid        => s_axis_tvalid,
    s_axis_tlast         => s_axis_tlast,
    m_axis_tready        => m_axis_tready,
    m_axis_tvalid        => m_axis_tvalid,
    m_axis_tlast         => m_axis_tlast
);



FF_Nonce: process(AES128CTR_i_CLK)
begin
if AES128CTR_i_CLK = '1' and AES128CTR_i_CLK' event then 
    if AES128CTR_i_RST = '1' then
        sv_nonce <= ( others => '0' );
    elsif sv_fsm_loadnonce = '1' then
        sv_nonce <= s_axis_tdata(127 downto 64);
    end if;
end if;
end process;

--FF_Input: process(AES128CTR_i_CLK)
--begin
--if AES128CTR_i_CLK = '1' and AES128CTR_i_CLK' event then 
--    if AES128CTR_i_RST = '1' then
--        sv_s_axis_tdata <= ( others => '0' );
--    elsif sv_fsm_loadinput = '1' then
--        sv_s_axis_tdata <= s_axis_tdata;
--    end if;
--end if;
--end process;

FF_AES_Feedback: process(AES128CTR_i_CLK)
begin
if AES128CTR_i_CLK = '1' and AES128CTR_i_CLK' event then 
    if AES128CTR_i_RST = '1' then
        sv_aes_feedback <= ( others => '0' );
    else
        sv_aes_feedback <= sv_aes_output;
    end if; 
end if;
end process;

FF_Output: process(AES128CTR_i_CLK)
begin
if AES128CTR_i_CLK = '1' and AES128CTR_i_CLK' event then 
    if AES128CTR_i_RST = '1' then
        sv_m_axis_tdata <= ( others => '0' );
    elsif sv_fsm_loadoutput = '1' then
        sv_m_axis_tdata <= sv_aes_output xor s_axis_tdata;
    end if; 
end if;
end process;

MUX_AESEngine_Input: process(sv_fsm_block, sv_nonce, sv_counter_out, sv_aes_feedback)
begin
    if sv_fsm_block = "01" then
        sv_aes_input <= sv_nonce & sv_counter_out;
    else
        sv_aes_input <= sv_aes_feedback;
    end if;
end process;

m_axis_tdata <= sv_m_axis_tdata;
-- sv_s_axis_tdata <= s_axis_tdata;

end Behavioral;
