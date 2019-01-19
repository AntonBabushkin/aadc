#pragma once

#include <systemc>
#include <systemc-ams>

class aadc_if// : sc_core::sc_module, sc_core::sc_interface
{
public:
	// ADC signals
	sc_core::sc_signal<bool> clk;
	sc_core::sc_signal<bool> start;
	sca_tdf::sca_signal<double> vin, vref;
	sc_core::sc_signal<int16_t> code;
	sc_core::sc_signal<bool> done;

	aadc_if(std::string name) {}
	/*aadc_if(const sc_core::sc_module_name& name)
		: sc_module(name) // Construct parent
	{
		
	}*/

};
