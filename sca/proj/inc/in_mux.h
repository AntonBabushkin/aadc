#pragma once

#include <systemc-ams>

// This class is a SystemC-AMS TDF MoC (module of computation)
class in_mux : sca_tdf::sca_module
{

public:

	// Ports declaration
	sca_tdf::sca_de::sca_in<bool> start;
	sca_tdf::sca_in<double> vin, vres;
	sca_tdf::sca_out<double> vmux;

	void set_attributes() {	}

	void initialize() { }

	// Class (SystemC-AMS MoC) constructor
	in_mux(sc_core::sc_module_name nm)
		// Construct ports
		: start("start")
		, vin("vin")
		, vres("vres")
		, vmux("vmux")
	{ }
	
	void ac_processing() { }

	void processing() {
		if (start.read()) vmux.write(vin.read());// Use Vin as first sample
		else vmux.write(vres.read());// Else use Vres
	}

protected:
	
};
