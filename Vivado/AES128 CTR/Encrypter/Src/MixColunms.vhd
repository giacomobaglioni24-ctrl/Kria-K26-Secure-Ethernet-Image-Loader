library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity MixColunms is
Port ( 
    MIXCOLUNMS_i_INPUT  : in  std_logic_vector(127 downto 0);
    MIXCOLUNMS_o_OUTPUT : out std_logic_vector(127 downto 0)
);
end MixColunms;



architecture Behavioral of MixColunms is

signal sv_mul_x2: std_logic_vector( 127 downto 0 );
signal sv_mul_x3: std_logic_vector( 127 downto 0 );

component MulX2 is
    port(
        MULX2_i_INPUT: IN std_logic_vector ( 7 downto 0 );
        MULX2_o_OUTPUT: OUT std_logic_vector ( 7 downto 0 )
    );
end component;

component MulX3 is
    port(
        MULX3_i_INPUT: IN std_logic_vector ( 7 downto 0 );
        MULX3_i_MULX2: IN std_logic_vector ( 7 downto 0 );
        MULX3_o_OUTPUT: OUT std_logic_vector ( 7 downto 0 )
    );
end component;



begin

Multiplication_X2: for i in 0 to 15 generate
MulX2_Inst: MulX2
    port map(
        MULX2_i_INPUT => MIXCOLUNMS_i_INPUT ( i*8+7 downto i*8 ),
        MULX2_o_OUTPUT => sv_mul_x2 ( i*8+7 downto i*8 )
    );
end generate;

Multiplication_X3: for i in 0 to 15 generate
MulX3_Inst: MulX3
    port map(
        MULX3_i_INPUT => MIXCOLUNMS_i_INPUT ( i*8+7 downto i*8 ),
        MULX3_i_MULX2 => sv_mul_x2 ( i*8+7 downto i*8 ),
        MULX3_o_OUTPUT => sv_mul_x3 ( i*8+7 downto i*8 )
    );
end generate;

Generate_Outputs: for i in 0 to 3 generate

        MIXCOLUNMS_o_OUTPUT( 127-i*32  downto 120-i*32 ) <= sv_mul_x2( 127-i*32  downto 120-i*32 ) xor sv_mul_x3( 119-i*32 downto 112-i*32 ) 
                                                      xor MIXCOLUNMS_i_INPUT( 111-i*32 downto 104-i*32 ) xor MIXCOLUNMS_i_INPUT( 103-i*32 downto 96-i*32 );
        
        MIXCOLUNMS_o_OUTPUT( 119-i*32 downto 112-i*32 ) <= MIXCOLUNMS_i_INPUT( 127-i*32  downto 120-i*32 ) xor sv_mul_x2( 119-i*32 downto 112-i*32 ) 
                                                        xor sv_mul_x3( 111-i*32 downto 104-i*32 ) xor MIXCOLUNMS_i_INPUT( 103-i*32 downto 96-i*32 );  
        
        MIXCOLUNMS_o_OUTPUT( 111-i*32 downto 104-i*32 ) <= MIXCOLUNMS_i_INPUT( 127-i*32  downto 120-i*32 ) xor MIXCOLUNMS_i_INPUT( 119-i*32 downto 112-i*32 ) 
                                                         xor sv_mul_x2( 111-i*32 downto 104-i*32 ) xor sv_mul_x3( 103-i*32 downto 96-i*32 );
        
        MIXCOLUNMS_o_OUTPUT( 103-i*32 downto 96-i*32 ) <= sv_mul_x3( 127-i*32  downto 120-i*32 ) xor MIXCOLUNMS_i_INPUT( 119-i*32 downto 112-i*32 ) 
                                                         xor MIXCOLUNMS_i_INPUT( 111-i*32 downto 104-i*32 ) xor sv_mul_x2( 103-i*32 downto 96-i*32 );
        
end generate;

end Behavioral;

