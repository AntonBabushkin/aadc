#pragma once

#include <systemc>
#include <systemc-ams>

class aadc_cnfg
{
public:
	// ADC config
	sc_core::sc_signal<double> integ_gain_error{ "integ_gain_error" };// Integrator gain error, in %


	// Constructor
	aadc_cnfg(std::string name) {
		integ_gain_error = 0.0;
	}

};
