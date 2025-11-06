library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;



entity tb_AES128CTR is
end tb_AES128CTR;



architecture Behavioral of tb_AES128CTR is

    -- Component Declaration
    component AES128CTR
        port (
            AES128CTR_i_CLK     : in  std_logic;
            AES128CTR_i_RST     : in  std_logic;

            -- Plaintext Input
            s_axis_tready       : out std_logic;
            s_axis_tvalid       : in  std_logic;
            s_axis_tlast        : in  std_logic;
            s_axis_tdata        : in  std_logic_vector(127 downto 0);

            -- Ciphertext Output
            m_axis_tready       : in  std_logic;
            m_axis_tvalid       : out std_logic;
            m_axis_tlast        : out std_logic;
            m_axis_tdata        : out std_logic_vector(127 downto 0)
        );
    end component;

    -- Signal Declarations
    signal AES128CTR_i_CLK     : std_logic;
    signal AES128CTR_i_RST     : std_logic;

    signal s_axis_tready       : std_logic;
    signal s_axis_tvalid       : std_logic;
    signal s_axis_tlast        : std_logic;
    signal s_axis_tdata        : std_logic_vector(127 downto 0);

    signal m_axis_tready       : std_logic;
    signal m_axis_tvalid       : std_logic;
    signal m_axis_tlast        : std_logic;
    signal m_axis_tdata        : std_logic_vector(127 downto 0);

begin

    -- DUT Instantiation
    uut: AES128CTR
        port map (
            AES128CTR_i_CLK     => AES128CTR_i_CLK,
            AES128CTR_i_RST     => AES128CTR_i_RST,
            s_axis_tready       => s_axis_tready,
            s_axis_tvalid       => s_axis_tvalid,
            s_axis_tlast        => s_axis_tlast,
            s_axis_tdata        => s_axis_tdata,
            m_axis_tready       => m_axis_tready,
            m_axis_tvalid       => m_axis_tvalid,
            m_axis_tlast        => m_axis_tlast,
            m_axis_tdata        => m_axis_tdata
        );

    -- Clock Generation
    clk_process : process
    begin
        AES128CTR_i_CLK <= '0';
        wait for 10 ns;
        AES128CTR_i_CLK <= '1';
        wait for 10 ns;
    end process;

--    -- Stimuli Process (da completare)
--    AES128CTR_i_RST <= '1', '0' after 30 ns;
--    s_axis_tvalid <= '1';
--    s_axis_tlast <= '0', '1' after 550 ns;
--    s_axis_tdata <= x"00112233445566778899aabbccddeeff"; --, x"00010203040506070a0b0c0d0e0f" after 270 ns;
--    m_axis_tready <= '0', '1' after 510 ns;

stim_proc: process
begin
    -- Reset sincrono
    AES128CTR_i_RST <= '1';
    wait until rising_edge(AES128CTR_i_CLK);
    wait until rising_edge(AES128CTR_i_CLK);
    AES128CTR_i_RST <= '0';
    
    -- Primo blocco
    s_axis_tdata  <= x"00112233445566778899aabbccddeeff";
    s_axis_tvalid <= '1';
    s_axis_tlast  <= '0';
    m_axis_tready <= '0';
    wait until rising_edge(AES128CTR_i_CLK) and s_axis_tready = '1';
    
    s_axis_tdata  <= (Others => 'Z');
    s_axis_tvalid <= '0';
    s_axis_tlast  <= 'Z';
    
    wait for 500 ns;

    -- Secondo blocco
    s_axis_tdata  <= x"000102030405060708090a0b0c0d0e0f";
    s_axis_tvalid <= '1';
    s_axis_tlast  <= '0';
    
    wait until rising_edge(AES128CTR_i_CLK) and m_axis_tvalid = '1';

    wait for 100 ns;
    wait until rising_edge(AES128CTR_i_CLK);

    m_axis_tready <= '1';
    
    wait until rising_edge(AES128CTR_i_CLK);
    
    m_axis_tready <= '0';
    
    wait until rising_edge(AES128CTR_i_CLK) and s_axis_tready = '1';

    -- Terzo blocco con `tlast` attivo
    s_axis_tdata  <= x"ffeeddccbbaa99887766554433221100";
    s_axis_tvalid <= '1';
    s_axis_tlast  <= '1';
    
    wait until rising_edge(AES128CTR_i_CLK) and m_axis_tvalid = '1';

    m_axis_tready <= '1';
    
    wait until rising_edge(AES128CTR_i_CLK);
    
    m_axis_tready <= '0';
    
    wait until rising_edge(AES128CTR_i_CLK) and s_axis_tready = '1';

    -- Fine trasmissione
    s_axis_tvalid <= '0';
    s_axis_tlast  <= '0';

    -- Continua la simulazione per osservare gli output
    wait;
end process;

end behavioral;
