
#include <systemc>
#include <systemc-ams>

#include <ctime>

#include "aadc_if.h"
#include "aadc.h"
#include "aadc_tb.h"





int sc_main (int argc, char* argv[])
{
	
	// ADC parameters
	double vref = 2.4;// Refernce voltage in V
	double adc_clk_freq = 0.001;// ADC clock frequency in MHz
	uint16_t adc_n_bit = 10;// ADC number of bits
	sca_core::sca_time st_base((1 / adc_clk_freq), sc_core::SC_NS);// Base sample time of TDF modules
	
	// ADC Interface
	aadc_if* aadc_vif = new aadc_if("aadc_vif");
	
	// Instantiate TB
	aadc_tb aadc_tb("aadc_tb", aadc_vif, st_base, adc_n_bit, vref);

	// Instantiate Algorithmic ADC module
	aadc aadc("aadc", adc_n_bit);
	aadc.clk(aadc_vif->clk);
	aadc.start(aadc_vif->start);
	aadc.vin(aadc_vif->vin);
	aadc.vref(aadc_vif->vref);
	aadc.code(aadc_vif->code);
	aadc.done(aadc_vif->done);

	// Create VCD and probe signals
	bool create_vcd = true;
	sca_util::sca_trace_file *vcdfile;
	if (create_vcd) {
		vcdfile = sca_util::sca_create_vcd_trace_file("./../vcd/aadc.vcd");
		// ADC signals
		sca_trace(vcdfile, aadc.clk, "aadc.clk");
		sca_trace(vcdfile, aadc.start, "aadc.start");
		sca_trace(vcdfile, aadc.vin, "aadc.vin");
		sca_trace(vcdfile, aadc.vref, "aadc.vref");
		sca_trace(vcdfile, aadc.cnt, "aadc.cnt");
		sca_trace(vcdfile, aadc.done, "aadc.done");
		sca_trace(vcdfile, aadc.p, "aadc.p");
		sca_trace(vcdfile, aadc.q, "aadc.q");
		sca_trace(vcdfile, aadc.ana_core.vmux, "aadc.ana_core.vmux");
		sca_trace(vcdfile, aadc.ana_core.vres, "aadc.ana_core.vres");
		sca_trace(vcdfile, aadc.ana_core.vdac, "aadc.ana_core.vdac");
		sca_trace(vcdfile, aadc.ana_core.vsh, "aadc.ana_core.vsh");
		sca_trace(vcdfile, aadc.code, "aadc.code");
		sca_trace(vcdfile, aadc.code_raw, "aadc.code_raw");
		sca_trace(vcdfile, aadc_vif->clk, "aadc_vif->clk");
	}

	// Start simulation
	std::cout << "Starting simulation..." << std::endl;
	clock_t begin = clock();
	sc_start(200, sc_core::SC_MS);
//	sc_start(1, sc_core::SC_MS);

	// End of simulation
	if (create_vcd) sca_util::sca_close_vcd_trace_file(vcdfile);
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Simulation took " << elapsed_secs << " seconds" << std::endl;
	
	return 0;
}
