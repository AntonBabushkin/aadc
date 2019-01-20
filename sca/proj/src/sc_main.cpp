
#include <systemc>
#include <systemc-ams>

#include <ctime>

#include "tb.h"





int sc_main (int argc, char* argv[])
{
	
	// Set ADC parameters
	double vref = 2.4;// Refernce voltage in V
	double adc_clk_freq = 0.001;// ADC clock frequency in MHz
	uint16_t adc_n_bit = 10;// ADC number of bits
	sca_core::sca_time st_base((1 / adc_clk_freq), sc_core::SC_NS);// Base sample time of TDF modules
	
	
	// Instantiate Test Bench
	tb tb("tb", st_base, adc_n_bit, vref);
	

	// Create VCD and probe signals
	bool create_vcd = false;
	sca_util::sca_trace_file *vcdfile;
	if (create_vcd) {
		vcdfile = sca_util::sca_create_vcd_trace_file("./../vcd/aadc.vcd");
		// ADC signals
		sca_trace(vcdfile, tb.aadc.clk, "aadc.clk");
		sca_trace(vcdfile, tb.aadc.start, "aadc.start");
		sca_trace(vcdfile, tb.aadc.vin, "aadc.vin");
		sca_trace(vcdfile, tb.aadc.vref, "aadc.vref");
		sca_trace(vcdfile, tb.aadc.cnt, "aadc.cnt");
		sca_trace(vcdfile, tb.aadc.done, "aadc.done");
		sca_trace(vcdfile, tb.aadc.p, "aadc.p");
		sca_trace(vcdfile, tb.aadc.q, "aadc.q");
		sca_trace(vcdfile, tb.aadc.ana_core.vmux, "aadc.ana_core.vmux");
		sca_trace(vcdfile, tb.aadc.ana_core.vres, "aadc.ana_core.vres");
		sca_trace(vcdfile, tb.aadc.ana_core.vdac, "aadc.ana_core.vdac");
		sca_trace(vcdfile, tb.aadc.ana_core.vsh, "aadc.ana_core.vsh");
		sca_trace(vcdfile, tb.aadc.code, "aadc.code");
		sca_trace(vcdfile, tb.aadc.code_raw, "aadc.code_raw");
		sca_trace(vcdfile, tb.testcase.dnl_in_bits, "tb.testcase.dnl_in_bits");
	}

	// Start simulation
	std::cout << "Starting simulation..." << std::endl;
	clock_t begin = clock();
	sc_start(5000, sc_core::SC_MS);

	// End of simulation
	if (create_vcd) sca_util::sca_close_vcd_trace_file(vcdfile);
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Simulation took " << elapsed_secs << " seconds" << std::endl;
	
	return 0;
}
