library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;



entity tb_AES128_CTR_Decrypter is
end tb_AES128_CTR_Decrypter;



architecture Behavioral of tb_AES128_CTR_Decrypter is

    -- Component Declaration
    component AES128_CTR_Decrypter
        port (
            AES128CTRDECRYPTER_i_CLK     : in  std_logic;
            AES128CTRDECRYPTER_i_RST     : in  std_logic;

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
    signal AES128CTRDECRYPTER_i_CLK     : std_logic;
    signal AES128CTRDECRYPTER_i_RST     : std_logic;

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
    uut: AES128_CTR_Decrypter
        port map (
            AES128CTRDECRYPTER_i_CLK     => AES128CTRDECRYPTER_i_CLK,
            AES128CTRDECRYPTER_i_RST     => AES128CTRDECRYPTER_i_RST,
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
        AES128CTRDECRYPTER_i_CLK <= '0';
        wait for 10 ns;
        AES128CTRDECRYPTER_i_CLK <= '1';
        wait for 10 ns;
    end process;

stim_proc: process
begin
    -- Reset sincrono
    AES128CTRDECRYPTER_i_RST <= '1';
    wait until rising_edge(AES128CTRDECRYPTER_i_CLK);
    wait until rising_edge(AES128CTRDECRYPTER_i_CLK);
    AES128CTRDECRYPTER_i_RST <= '0';
    
    -- Primo blocco
    s_axis_tdata  <= x"00112233445566778899aabbccddeeff";
    s_axis_tvalid <= '1';
    s_axis_tlast  <= '0';
    m_axis_tready <= '0';
    wait until rising_edge(AES128CTRDECRYPTER_i_CLK) and s_axis_tready = '1';
    
    s_axis_tdata  <= (Others => 'Z');
    s_axis_tvalid <= '0';
    s_axis_tlast  <= 'Z';
    
    wait for 500 ns;

    -- Secondo blocco
    s_axis_tdata  <= x"F7A9F8D2118DD45C11E29F2C2BDA8131";
    s_axis_tvalid <= '1';
    s_axis_tlast  <= '0';
    
    wait until rising_edge(AES128CTRDECRYPTER_i_CLK) and s_axis_tready = '1';

    -- Terzo blocco con `tlast` attivo
    s_axis_tdata  <= x"D786F14BF49F8726D4717AB0348AA828";
    s_axis_tvalid <= '1';
    s_axis_tlast  <= '1';
    
    wait until rising_edge(AES128CTRDECRYPTER_i_CLK) and m_axis_tvalid = '1';

    wait for 100 ns;
    wait until rising_edge(AES128CTRDECRYPTER_i_CLK);

    m_axis_tready <= '1';
    
    wait until rising_edge(AES128CTRDECRYPTER_i_CLK);
    
    m_axis_tready <= '0';
    
    wait until rising_edge(AES128CTRDECRYPTER_i_CLK) and m_axis_tvalid = '1';

    m_axis_tready <= '1';
    
    wait until rising_edge(AES128CTRDECRYPTER_i_CLK);
    
    m_axis_tready <= '0';
    
    wait until rising_edge(AES128CTRDECRYPTER_i_CLK) and s_axis_tready = '1';

    -- Fine trasmissione
    s_axis_tvalid <= '0';
    s_axis_tlast  <= '0';

    -- Continua la simulazione per osservare gli output
    wait;
end process;

end behavioral;
