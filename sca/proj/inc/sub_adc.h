#pragma once

#include <systemc-ams>

// This class is a SystemC-AMS TDF MoC (module of computation)
class sub_adc : sca_tdf::sca_module
{

public:

	// Ports declaration
	sca_tdf::sca_in<double> vin;
	sca_tdf::sca_in<double> vref;
	sca_tdf::sca_de::sca_out<bool> p, q;

	void set_attributes() {	}

	void initialize() { }
	
	// Class (SystemC-AMS MoC) constructor
	sub_adc(sc_core::sc_module_name nm)
		// Construct ports
		: vin("vin")
		, vref("vref")
		, p("p")
		, q("q")
	{ }
	
	void ac_processing() { };

	void processing() {
		c1 = c2 = false;
		if (vin.read() >= +1 * vref.read() / 4) c1 = true;
		if (vin.read() <= -1 * vref.read() / 4) c2 = true;
		comparators_to_pq(c1, c2, &p_val, &q_val);
		p.write(p_val);
		q.write(q_val);
	}

protected:
	bool c1, c2;// Comparators decision
	bool p_val, q_val;

	void comparators_to_pq(bool c1, bool c2, bool *p, bool *q) {
		*p = *q = 0;
		if (c1 == 1) {
			// Vin >= Vref / 4
			*p = 1; *q = 0;
		}
		else if (c2 == 1) {
			// Vin <= -Vref / 4
			*p = 0; *q = 1;
		}
		else if (c1 == 0 && c2 == 0) {
			// -Vref / 4<Vin<Vref / 4
			*p = 0; *q = 0;
		}
	}

};
