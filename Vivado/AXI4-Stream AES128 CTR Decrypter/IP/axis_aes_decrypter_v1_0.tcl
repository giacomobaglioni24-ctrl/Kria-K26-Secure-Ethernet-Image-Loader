# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "ROUND_KEY" -parent ${Page_0}


}

proc update_PARAM_VALUE.COUNTER_INITIAL_VALUE { PARAM_VALUE.COUNTER_INITIAL_VALUE } {
	# Procedure called to update COUNTER_INITIAL_VALUE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.COUNTER_INITIAL_VALUE { PARAM_VALUE.COUNTER_INITIAL_VALUE } {
	# Procedure called to validate COUNTER_INITIAL_VALUE
	return true
}

proc update_PARAM_VALUE.ROUND_KEY { PARAM_VALUE.ROUND_KEY } {
	# Procedure called to update ROUND_KEY when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.ROUND_KEY { PARAM_VALUE.ROUND_KEY } {
	# Procedure called to validate ROUND_KEY
	return true
}


proc update_MODELPARAM_VALUE.ROUND_KEY { MODELPARAM_VALUE.ROUND_KEY PARAM_VALUE.ROUND_KEY } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.ROUND_KEY}] ${MODELPARAM_VALUE.ROUND_KEY}
}

proc update_MODELPARAM_VALUE.COUNTER_INITIAL_VALUE { MODELPARAM_VALUE.COUNTER_INITIAL_VALUE PARAM_VALUE.COUNTER_INITIAL_VALUE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.COUNTER_INITIAL_VALUE}] ${MODELPARAM_VALUE.COUNTER_INITIAL_VALUE}
}

