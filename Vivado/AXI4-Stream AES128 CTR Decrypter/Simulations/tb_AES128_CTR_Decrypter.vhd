library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;



entity tb_AES128_CTR_Decrypter is
end tb_AES128_CTR_Decrypter;



architecture Behavioral of tb_AES128_CTR_Decrypter is

    -- Component Declaration
    component AES128_CTR_Decrypter
        port (
            clk     : in  std_logic;
            rst_n     : in  std_logic;

            -- Plaintext Input
            s_axis_tready       : out std_logic;
            s_axis_tvalid       : in  std_logic;
            s_axis_tlast        : in  std_logic;
            s_axis_tkeep        : in  std_logic_vector(15 downto 0);
            s_axis_tdata        : in  std_logic_vector(127 downto 0);

            -- Ciphertext Output
            m_axis_tready       : in  std_logic;
            m_axis_tvalid       : out std_logic;
            m_axis_tlast        : out std_logic;
            m_axis_tkeep        : out std_logic_vector(15 downto 0);
            m_axis_tdata        : out std_logic_vector(127 downto 0)
        );
    end component;

    -- Signal Declarations
    signal clk     : std_logic;
    signal rst_n     : std_logic;

    signal s_axis_tready       : std_logic;
    signal s_axis_tvalid       : std_logic;
    signal s_axis_tlast        : std_logic;
    signal s_axis_tkeep        : std_logic_vector(15 downto 0);
    signal s_axis_tdata        : std_logic_vector(127 downto 0);

    signal m_axis_tready       : std_logic;
    signal m_axis_tvalid       : std_logic;
    signal m_axis_tlast        : std_logic;
    signal m_axis_tkeep        : std_logic_vector(15 downto 0);
    signal m_axis_tdata        : std_logic_vector(127 downto 0);

begin

    -- DUT Instantiation
    uut: AES128_CTR_Decrypter
        port map (
            clk     => clk,
            rst_n     => rst_n,
            s_axis_tready       => s_axis_tready,
            s_axis_tvalid       => s_axis_tvalid,
            s_axis_tlast        => s_axis_tlast,
            s_axis_tdata        => s_axis_tdata,
            s_axis_tkeep        => s_axis_tkeep,
            m_axis_tready       => m_axis_tready,
            m_axis_tvalid       => m_axis_tvalid,
            m_axis_tlast        => m_axis_tlast,
            m_axis_tkeep        => m_axis_tkeep,
            m_axis_tdata        => m_axis_tdata
        );

    -- Clock Generation
    clk_process : process
    begin
        clk <= '0';
        wait for 10 ns;
        clk <= '1';
        wait for 10 ns;
    end process;

rst_n <= '1';

stim_proc: process
begin

    -- Primo blocco
    s_axis_tdata  <= x"ffeeddccbbaa99887766554433221100";
    s_axis_tvalid <= '1';
    s_axis_tlast  <= '0';
    m_axis_tready <= '0';
    wait until rising_edge(clk) and s_axis_tready = '1';
    
    s_axis_tdata  <= (Others => 'Z');
    s_axis_tvalid <= '0';
    s_axis_tlast  <= 'Z';
    

    -- Secondo blocco
    s_axis_tdata  <= x"3181da2b2c9fe2115cd48d11d2f8a9f7";
    s_axis_tvalid <= '1';
    s_axis_tlast  <= '0';
    
    wait until rising_edge(clk) and s_axis_tready = '1';

    -- Terzo blocco con `tlast` attivo
    s_axis_tdata  <= x"D786F14BF49F8726D4717AB0348AA828";
    s_axis_tvalid <= '1';
    s_axis_tlast  <= '1';
    
    wait until rising_edge(clk) and m_axis_tvalid = '1';


    m_axis_tready <= '1';
    
    wait until rising_edge(clk);
    
    m_axis_tready <= '0';
    
    wait until rising_edge(clk) and m_axis_tvalid = '1';

    m_axis_tready <= '1';
    
    wait until rising_edge(clk);
    
    m_axis_tready <= '0';
    
    wait until rising_edge(clk) and s_axis_tready = '1';

    -- Fine trasmissione
    s_axis_tvalid <= '0';
    s_axis_tlast  <= '0';

    wait;
end process;

end behavioral;
