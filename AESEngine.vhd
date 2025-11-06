library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.AESEngine_pkg.all;



entity AESEngine is
Port ( 
    AESENGINE_i_INPUT: IN std_logic_vector ( 127 downto 0 );
    AESENGINE_i_KEY: IN std_logic_vector ( 127 downto 0 );
    AESENGINE_i_BLOCK: IN std_logic_vector ( 1 downto 0 );
    AESENGINE_o_OUTPUT: OUT std_logic_vector ( 127 downto 0 )
);
end AESEngine;



architecture Behavioral of AESEngine is

signal sv_sub_to_shift: std_logic_vector ( 127 downto 0 );
signal sv_shift_to_mix: std_logic_vector ( 127 downto 0 );
signal sv_mix_output: std_logic_vector ( 127 downto 0 );
signal sv_mix_to_rndkey: std_logic_vector ( 127 downto 0 );



begin

-- SubBytes
SubBytes_inst : SubBytes
port map (
    SUBBYTES_i_INPUT  => AESENGINE_i_INPUT,
    SUBBYTES_o_OUTPUT => sv_sub_to_shift
);

-- ShiftRows
ShiftRows_inst : ShiftRows
port map (
    SHIFTROWS_i_INPUT  => sv_sub_to_shift,
    SHIFTROWS_o_OUTPUT => sv_shift_to_mix
);

-- MixColunms
MixColunms_inst : MixColunms
port map (
    MIXCOLUNMS_i_INPUT  => sv_shift_to_mix,
    MIXCOLUNMS_o_OUTPUT => sv_mix_output
);

-- AddRoundKey
AddRoundKey_inst : AddRoundKey
port map (
    ADDROUNDKEY_i_INPUT  => sv_mix_to_rndkey,
    ADDROUNDKEY_i_KEY    => AESENGINE_i_KEY,
    ADDROUNDKEY_o_OUTPUT => AESENGINE_o_OUTPUT
);

MUX: process(sv_mix_output, sv_shift_to_mix,AESENGINE_i_BLOCK , AESENGINE_i_INPUT) 
begin
    case AESENGINE_i_BLOCK is
        when "00" => 
            sv_mix_to_rndkey <= sv_shift_to_mix;
        when "01" => 
            sv_mix_to_rndkey <= AESENGINE_i_INPUT;
        when "10" => 
            sv_mix_to_rndkey <= sv_mix_output;
        when "11" => 
            sv_mix_to_rndkey <= sv_shift_to_mix;
        when others => 
            null;
    end case;
end process;

end behavioral;