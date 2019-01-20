#pragma once

#include <systemc-ams>


// This class is a SystemC-AMS TDF MoC (module of computation)
class res_integr : sca_tdf::sca_module
{

public:

	// Ports declaration
	sca_tdf::sca_in<double> vsh, vdac;
	sca_tdf::sca_out<double> vres;

	void set_attributes() {	}

	void initialize() {	
		integ_gain = 2 * (1 + integ_gain_error / 100.0);
	}
	
	// Class (SystemC-AMS MoC) constructor
	res_integr(sc_core::sc_module_name nm)
		// Construct ports
		: vsh("vsh"), vdac("vdac"), vres("vres")
	{ }

	void ac_processing() { }

	void processing() {
		vres.write((vsh.read() - vdac.read()) * integ_gain);
	}

protected:
	double integ_gain;// Integator gain
	double integ_gain_error = 0.1;// Integrator gain error, in %
};
