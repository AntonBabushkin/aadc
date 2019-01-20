
#pragma once

#include <systemc>
#include <systemc-ams>

// This class is a SystemC module
class clk_gen : sc_core::sc_module
{

public:
	
	sc_core::sc_out<bool> clk{ "clk" };

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(clk_gen);
	clk_gen(const sc_core::sc_module_name& name, sca_core::sca_time st_base_=sca_core::sca_time(1000.0, sc_core::SC_NS))
		: sc_module(name) // Construct parent
		// Initialize local variable
		, st_base(st_base_)
	{
		SC_THREAD(do_clk_gen);
	}

protected:
	// Local variables
	sca_core::sca_time st_base;

	// Local methods

	void do_clk_gen() {
		// Wait for clock intialization time
		clk.write(false);
		wait(st_base);
		// Start clock generation
		while (true) {
			clk.write(true);
			wait(st_base / 2);
			clk.write(false);
			wait(st_base / 2);
		}
	}
	
};