#pragma once

#include <systemc-ams>


// This class is a SystemC-AMS TDF MoC (module of computation)
class sampl_hold : sca_tdf::sca_module
{

public:

	// Ports declaration
	sca_tdf::sca_in<double> in;
	sca_tdf::sca_out<double> out;

	void set_attributes() {	
		out.set_delay(1);
	}

	void initialize() {
		out.initialize(0.0);
	}
	
	// Class (SystemC-AMS MoC) constructor
	sampl_hold(sc_core::sc_module_name nm)
		// Construct ports
		: in("in"), out("out")
	{ }

	void ac_processing() { }

	void processing() {
		out.write(in.read());
	}

protected:
	
};
