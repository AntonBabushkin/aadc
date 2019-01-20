#pragma once

#include <systemc-ams>

#include "aadc_cnfg.h"

// This class is a SystemC-AMS TDF MoC (module of computation)
class res_integr : sca_tdf::sca_module
{

public:

	// Ports declaration
	sca_tdf::sca_in<double> vsh, vdac;
	sca_tdf::sca_out<double> vres;

	void set_attributes() {	}

	void initialize() {	
		
	}
	
	// Class (SystemC-AMS MoC) constructor
	res_integr(sc_core::sc_module_name nm, aadc_cnfg* aadc_cfg_ = nullptr) :
		// Initialize local variable
		aadc_cfg(aadc_cfg_)
		// Construct ports
		, vsh("vsh"), vdac("vdac"), vres("vres")
	{ }

	void ac_processing() { }

	void processing() {
		integ_gain = 2 * (1 + aadc_cfg->integ_gain_error / 100.0);// Compute actual integrator gain
		vres.write((vsh.read() - vdac.read()) * integ_gain);
	}

protected:
	aadc_cnfg* aadc_cfg;
	double integ_gain;// Integator gain
};
