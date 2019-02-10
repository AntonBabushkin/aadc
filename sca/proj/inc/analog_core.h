#pragma once

#include <systemc>
#include <systemc-ams>

// ADC components
#include "in_mux.h"
#include "sub_adc.h"
#include "sampl_hold.h"
#include "sub_dac.h"
#include "res_integr.h"


// This class is a SystemC module
class analog_core : sc_core::sc_module
{

public:

	// Ports declaration
	sc_core::sc_in<bool> start;
	sca_tdf::sca_in<double> vin, vref;
	sc_core::sc_out<bool> p, q;

	// ADC signals
	sca_tdf::sca_signal<double> vres, vdac, vmux, vsh;

	// ADC Analog Core components instantiation
	in_mux in_mux;
	sub_adc sub_adc;
	sampl_hold sampl_hold;
	sub_dac sub_dac;
	res_integr res_integr;
	
	// Class (SystemC module) constructor
	analog_core(const sc_core::sc_module_name& name)
		: sc_module(name) // Construct parent
		// ADC Analog Core components creation
		, in_mux("in_mux")
		, sub_adc("sub_adc")
		, sampl_hold("sampl_hold")
		, sub_dac("sub_dac")
		, res_integr("res_integr")
	{
		// ADC Analog Core components interconnection

		in_mux.start(start);			// Input MUX
		in_mux.vin(vin);
		in_mux.vres(vres);
		in_mux.vmux(vmux);

		sub_adc.vin(vmux);				// sub-ADC
		sub_adc.vref(vref);
		sub_adc.p(p);
		sub_adc.q(q);

		sampl_hold.in(vmux);			// Sample and Hold
		sampl_hold.out(vsh);

		sub_dac.vref(vref);				// sub-DAC
		sub_dac.p(p);					// p and q are of sca_tdf::sca_de::sca_in<T> type, 
		sub_dac.q(q);					//  there will be 1 sample delay due to DE->TDF domain crossing
		sub_dac.vdac(vdac);				//  acc to section 2.4.1. Reading from the discrete-event domain of SystemC AMS extensions User’s Guide (March 8 2010)

		res_integr.vsh(vsh);			// Residue amplifier
		res_integr.vdac(vdac);
		res_integr.vres(vres);

	}

protected:
	
};
