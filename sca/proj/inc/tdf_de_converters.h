#pragma once

#include <systemc>
#include <systemc-ams>




// TDF to DE converter
// This class is a SystemC-AMS TDF MoC (module of computation)
template <typename T>
class tdf2de : sca_tdf::sca_module {

public:

	sca_tdf::sca_in<T> in;				// From TDF domain
	sca_tdf::sca_de::sca_out<T> out;	// To DE domain

	tdf2de(sc_core::sc_module_name nm)
		: in("in"), out("out") { }

	void set_attributes() {	}

	void processing() {
		out.write(in.read());
	}

private:
	

};


// DE to TDF converter
// This class is a SystemC-AMS TDF MoC (module of computation)
template <typename T>
class de2tdf : sca_tdf::sca_module {

public:

	sca_tdf::sca_de::sca_in<T> in;		// From DE domain
	sca_tdf::sca_out<T> out;			// To TDF domain

	de2tdf(sc_core::sc_module_name nm, sca_core::sca_time Tp_ = sca_core::sca_time(1.0, sc_core::SC_NS))
		: in("in"), out("out"), Tp(Tp_) { }

	void set_attributes() {
		out.set_timestep(Tp);
	}

	void processing() {
		out.write(in.read());
	}


private:
	sca_core::sca_time Tp; // Port time step

};
