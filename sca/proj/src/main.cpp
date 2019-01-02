
#include <systemc>
#include <systemc-ams>

#include <ctime>

#include "aadc.h"
#include "aadc_tb.h"





int sc_main (int argc, char* argv[])
{
	
	// ADC parameters
	double vref = 2.4;// Refernce voltage in V
	double adc_clk_freq = 0.001;// ADC clock frequency in MHz
	uint16_t adc_n_bit = 10;// ADC number of bits
	sca_core::sca_time st_base((1 / adc_clk_freq), sc_core::SC_NS);// Base sample time of TDF modules

	// ADC clock
	sca_core::sca_time adc_clk_per(1 / adc_clk_freq, sc_core::SC_NS);
	sc_core::sc_clock adc_clk("adc_clk", adc_clk_per, 0.5, adc_clk_per);
	
	// ADC signals
	sc_core::sc_signal<bool> adc_start;
	sca_tdf::sca_signal<double> adc_vin;
	sca_tdf::sca_signal<double> adc_vref;
	sc_core::sc_signal<int16_t> adc_code;
	sc_core::sc_signal<bool> adc_done;
	
	// Instantiate TB
	aadc_tb aadc_tb("aadc_tb", st_base, adc_n_bit, vref);
	aadc_tb.adc_clk(adc_clk);
	aadc_tb.adc_start(adc_start);
	aadc_tb.adc_vin(adc_vin);
	aadc_tb.adc_vref(adc_vref);
	aadc_tb.adc_code(adc_code);
	aadc_tb.adc_done(adc_done);

	// Instantiate Algorithmic ADC module
	aadc aadc("aadc", adc_n_bit);
	aadc.clk(adc_clk);
	aadc.start(adc_start);
	aadc.vin(adc_vin);
	aadc.vref(adc_vref);
	aadc.code(adc_code);
	aadc.done(adc_done);

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
