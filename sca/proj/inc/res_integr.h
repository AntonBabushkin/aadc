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
		
	}
	
	// Class (SystemC-AMS MoC) constructor
	res_integr(sc_core::sc_module_name nm) :
		// Initialize parameters
		gain_err("gain_err", 0.1, "Amplifier gain error, in %")
		// Construct ports
		, vsh("vsh"), vdac("vdac"), vres("vres")
	{ }

	void ac_processing() { }

	void processing() {
		integ_gain = 2 * (1 + gain_err.get_cci_value().get_double() / 100.0);// Compute actual integrator gain
		vres.write((vsh.read() - vdac.read()) * integ_gain);
	}

private:
	cci::cci_param<double> gain_err;// Amplifier gain error, in %

protected:
	double integ_gain;// Integator gain

};
