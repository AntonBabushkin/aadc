#pragma once
#include <systemc>
#include <stdio.h>

// This class is a SystemC module
class control : sc_core::sc_module
{

public:

	// Ports declaration
	sc_core::sc_in_clk clk;
	sc_core::sc_in<bool> start;			// Start conversion
	sc_core::sc_out<uint16_t> cnt;
	sc_core::sc_out<bool> done;

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(control);// Declare that this module have a process
	control(const sc_core::sc_module_name& name, uint16_t n_bits_ = 10)
		: sc_module(name) // Construct parent
		, n_bits(n_bits_)
	{

		// Initialize counter
		counter = 0;

		// Start SystemC thread
		SC_METHOD(do_control);
		sensitive << clk.pos();
		dont_initialize();

		// Print info
		std::cout << "The module " << name << " was constructed"
			<< " at time " << sc_core::sc_time_stamp() << std::endl;
	}

protected:

	// Control thread
	void do_control() {
		if (start.read()) {
			done.write(false); 
			counter = 0;
		} else {
			uint16_t max_cnt = (n_bits - 1);
			if (counter != max_cnt) counter++;// Count up till top
			if (counter == max_cnt) done.write(true);// Conversion finished
		}
		cnt.write(counter);
	}

	// Local variables
	uint16_t n_bits;
	uint16_t counter;

};