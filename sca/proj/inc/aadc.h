#pragma once

#include <systemc>
#include <systemc-ams>

#include "aadc_cnfg.h"

// Include ADC components definition
#include "digital_core.h"
#include "analog_core.h"




// This class is a SystemC module
class aadc : sc_core::sc_module
{

public:
	// ADC ports
	sc_core::sc_in<bool> clk;
	sc_core::sc_in<bool> start;
	sca_tdf::sca_in<double> vin, vref;
	sc_core::sc_out<int16_t> code;
	sc_core::sc_out<bool> done;

	// ADC signals
	sc_core::sc_signal<uint16_t> cnt;
	sc_core::sc_signal<bool> p, q;
	sc_core::sc_signal<int16_t> code_raw;
	
	// ADC components instantiation
	digital_core dig_core;				// (DE domain)
	analog_core ana_core;				// (TDF domain)

	// Class (SystemC module) constructor
	aadc(const sc_core::sc_module_name& name, aadc_cnfg* aadc_cfg_ = nullptr, uint16_t n_bits_=10)
		: sc_module(name) // Construct parent
		// Initialize local variable
		
		// ADC components creation
		, dig_core("digital_core", n_bits_)
		, ana_core("analog_core", aadc_cfg_)
	{
		
		// ADC core components interconnect

		dig_core.clk(clk);				// Digital core
		dig_core.start(start);
		dig_core.p(p);
		dig_core.q(q);
		dig_core.cnt(cnt);
		dig_core.done(done);
		dig_core.code(code);
		dig_core.code_raw(code_raw);
		
		ana_core.start(start);			// Analog core
		ana_core.vin(vin);
		ana_core.vref(vref);
		ana_core.p(p);
		ana_core.q(q);

		// Print info
		std::cout << "The module " << name << " was constructed at time " << sc_core::sc_time_stamp() << std::endl;
	}

protected:
	// Local variables
	

};