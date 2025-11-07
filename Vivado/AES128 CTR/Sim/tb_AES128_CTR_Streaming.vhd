library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity tb_AES128_CTR_Streaming is
--  Port ( );
end tb_AES128_CTR_Streaming;



architecture Behavioral of tb_AES128_CTR_Streaming is

-- Component Declaration
component AES128_CTR_Encrypter is
Port ( 
    AES128CTRENCRYPTER_i_CLK   : IN std_logic;
    AES128CTRENCRYPTER_i_RST   : IN std_logic;
    s_axis_tready              : OUT std_logic;
    s_axis_tvalid              : IN std_logic;
    s_axis_tlast               : IN std_logic;
    s_axis_tdata               : IN std_logic_vector(127 downto 0);
    m_axis_tready              : IN std_logic;
    m_axis_tvalid              : OUT std_logic;
    m_axis_tlast               : OUT std_logic;
    m_axis_tdata               : OUT std_logic_vector(127 downto 0)
);
end component;

component AES128_CTR_Decrypter is
Port ( 
    AES128CTRDECRYPTER_i_CLK   : IN std_logic;
    AES128CTRDECRYPTER_i_RST   : IN std_logic;
    s_axis_tready              : OUT std_logic;
    s_axis_tvalid              : IN std_logic;
    s_axis_tlast               : IN std_logic;
    s_axis_tdata               : IN std_logic_vector(127 downto 0);
    m_axis_tready              : IN std_logic;
    m_axis_tvalid              : OUT std_logic;
    m_axis_tlast               : OUT std_logic;
    m_axis_tdata               : OUT std_logic_vector(127 downto 0)
);
end component;



-- Signal Declaration
signal sv_clk               : std_logic;
signal sv_rst               : std_logic;

-- Input to Encrypter
signal sv_s_axis_tvalid     : std_logic;
signal sv_s_axis_tlast      : std_logic;
signal sv_s_axis_tdata      : std_logic_vector(127 downto 0);
signal sv_s_axis_tready     : std_logic;

-- Output from Encrypter / Input to Decrypter
signal sv_m_axis_tvalid_enc : std_logic;
signal sv_m_axis_tlast_enc  : std_logic;
signal sv_m_axis_tdata_enc  : std_logic_vector(127 downto 0);
signal sv_m_axis_tready_enc : std_logic;

-- Output from Decrypter
signal sv_m_axis_tvalid_dec : std_logic;
signal sv_m_axis_tlast_dec  : std_logic;
signal sv_m_axis_tdata_dec  : std_logic_vector(127 downto 0);
signal sv_m_axis_tready_dec : std_logic := '1';



begin


-- Port Map Encrypter
aes_encrypter_inst : AES128_CTR_Encrypter
port map (
    AES128CTRENCRYPTER_i_CLK   => sv_clk,
    AES128CTRENCRYPTER_i_RST   => sv_rst,
    s_axis_tready              => sv_s_axis_tready,
    s_axis_tvalid              => sv_s_axis_tvalid,
    s_axis_tlast               => sv_s_axis_tlast,
    s_axis_tdata               => sv_s_axis_tdata,
    m_axis_tready              => sv_m_axis_tready_enc,
    m_axis_tvalid              => sv_m_axis_tvalid_enc,
    m_axis_tlast               => sv_m_axis_tlast_enc,
    m_axis_tdata               => sv_m_axis_tdata_enc
);

-- Port Map Decrypter
aes_decrypter_inst : AES128_CTR_Decrypter
port map (
    AES128CTRDECRYPTER_i_CLK   => sv_clk,
    AES128CTRDECRYPTER_i_RST   => sv_rst,
    s_axis_tready              => sv_m_axis_tready_enc,
    s_axis_tvalid              => sv_m_axis_tvalid_enc,
    s_axis_tlast               => sv_m_axis_tlast_enc,
    s_axis_tdata               => sv_m_axis_tdata_enc,
    m_axis_tready              => sv_m_axis_tready_dec,
    m_axis_tvalid              => sv_m_axis_tvalid_dec,
    m_axis_tlast               => sv_m_axis_tlast_dec,
    m_axis_tdata               => sv_m_axis_tdata_dec
);

    -- Clock Generation
    clk_process : process
    begin
        sv_clk <= '0';
        wait for 10 ns;
        sv_clk <= '1';
        wait for 10 ns;
    end process;

    stim_proc: process
    begin
        -- Reset sincrono
        sv_rst <= '1';
        wait until rising_edge(sv_clk);
        wait until rising_edge(sv_clk);
        sv_rst <= '0';
        
        -- Primo blocco
        sv_s_axis_tdata  <= x"00112233445566778899aabbccddeeff";
        sv_s_axis_tvalid <= '1';
        sv_s_axis_tlast  <= '0';
        wait until rising_edge(sv_clk) and sv_s_axis_tready = '1';
        
        sv_s_axis_tvalid <= '0';
        sv_s_axis_tlast  <= '0';
        
        wait for 500 ns;
    
        -- Secondo blocco
        sv_s_axis_tdata  <= x"000102030405060708090A0B0C0D0E0F";
        sv_s_axis_tvalid <= '1';
        sv_s_axis_tlast  <= '0';
        
        wait until rising_edge(sv_clk) and sv_s_axis_tready = '1';
    
        sv_s_axis_tdata  <= x"FFEEDDCCBBAA99887766554433221100";
        sv_s_axis_tvalid <= '1';
        sv_s_axis_tlast  <= '1';
        
        wait until rising_edge(sv_clk) and sv_s_axis_tready = '1';
    
        -- Fine trasmissione
        sv_s_axis_tvalid <= '0';
        sv_s_axis_tlast  <= '0';
    
        -- Continua la simulazione per osservare gli output
        wait;
    end process;

end Behavioral;
