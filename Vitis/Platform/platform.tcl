# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\Users\stage_gbaglioni\Vitis\ZynqUltrascalePlusB\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\Users\stage_gbaglioni\Vitis\ZynqUltrascalePlusB\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {ZynqUltrascalePlusB}\
-hw {C:\Users\stage_gbaglioni\Zynq_Codesign_Ethernet_16092025\Zynq_Codesign_Ethernet_16102025_1.xsa}\
-proc {psu_cortexa53_0} -os {standalone} -arch {64-bit} -fsbl-target {psu_cortexa53_0} -out {C:/Users/stage_gbaglioni/Vitis}

platform write
platform generate -domains 
platform active {ZynqUltrascalePlusB}
bsp reload
bsp setlib -name lwip211 -ver 1.8
bsp setlib -name xilpm -ver 4.1
bsp setlib -name xilsecure -ver 5.0
bsp setlib -name xilskey -ver 7.3
bsp removelib -name xilpm
bsp setlib -name xilfpga -ver 6.3
bsp config phy_link_speed "CONFIG_LINKSPEED1000"
bsp write
bsp reload
catch {bsp regenerate}
platform generate
platform active {ZynqUltrascalePlusB}
platform config -updatehw {C:/Users/stage_gbaglioni/Zynq_Codesign_Ethernet_16092025/Immagine_B.xsa}
platform generate
platform generate -domains standalone_domain 
platform active {ZynqUltrascalePlusB}
platform config -updatehw {C:/Users/stage_gbaglioni/Zynq_Codesign_Ethernet_16092025/Design_Finale.xsa}
platform generate -domains 
platform config -updatehw {C:/Users/stage_gbaglioni/Zynq_Codesign_Ethernet_16092025/Design_Finale.xsa}
platform generate -domains 
platform config -updatehw {C:/Users/stage_gbaglioni/Zynq_Codesign_Ethernet_16092025/Immagine_B.xsa}
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
