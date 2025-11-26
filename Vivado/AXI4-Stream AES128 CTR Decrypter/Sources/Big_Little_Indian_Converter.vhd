library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity Big_Little_Indian_Converter is
Port ( 
    BIGLITTLEINDIANCONVERTER_i_INPUT: IN std_logic_vector ( 127 downto 0 );
    BIGLITTLEINDIANCONVERTER_o_OUTPUT: OUT std_logic_vector ( 127 downto 0 )
);
end Big_Little_Indian_Converter;



architecture Behavioral of Big_Little_Indian_Converter is

begin

Generate_Outputs: for i in 0 to 15 generate

        BIGLITTLEINDIANCONVERTER_o_OUTPUT ( 8*i+7 downto 8*i ) <= BIGLITTLEINDIANCONVERTER_i_INPUT ( 127-i*8 downto 120-i*8 );

end generate;

end Behavioral;
