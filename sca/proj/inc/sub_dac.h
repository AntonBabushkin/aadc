#pragma once

#include <systemc-ams>


// This class is a SystemC-AMS TDF MoC (module of computation)
class sub_dac : sca_tdf::sca_module
{

public:

	// Ports declaration
	sca_tdf::sca_in<double> vref;
	sca_tdf::sca_de::sca_in<bool> p, q;
	sca_tdf::sca_out<double> vdac;

	void set_attributes() { }

	void initialize() { }

	// Class (SystemC-AMS MoC) constructor
	sub_dac(sc_core::sc_module_name nm)
		// Construct ports
		: vref("vref")
		, p("p")
		, q("q")
		, vdac("vdac")
	{ }
	
	void ac_processing() { }

	void processing() {
		pq_to_mul(p.read(), q.read(), &vref_mul_val);
		vdac.write(vref_mul_val*vref.read());
	}

protected:
	double vref_mul_val;

	void pq_to_mul(bool p, bool q, double *mul_val) {
		*mul_val = 0;
		if (p == 1 && q == 0) *mul_val = +1*(1.0/2);// Vin >= Vref/4
		else if (p == 0 && q == 1) *mul_val = -1*(1.0/2);// Vin <= -Vref/4
		else if (p == 0 && q == 0) *mul_val = 0;// -Vref/4<Vin<Vref/4
	}

};
