
#include <systemc>
#include <systemc-ams>

#include <ctime>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4913 )
#endif // _MSC_VER
#include "args.hxx"
#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER

#include "tb.h"





int sc_main (int argc, char* argv[])
{
	
	// Print header
	std::cout << std::endl << std::endl;

	// Parse arguments using https://github.com/Taywee/args library
	// Invocation: >aadc.exe tc_1
	args::ArgumentParser parser("This is a AADC test program.", "This goes after the options.");
	args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
	args::Positional<std::string> test_case_name(parser, "test_case_name", "Test case name (optional)");
	args::CompletionFlag completion(parser, { "complete" });
	try
	{
		parser.ParseCLI(argc, argv);
	}
	catch (const args::Completion& e)
	{
		std::cout << e.what();
		return 0;
	}
	catch (const args::Help&)
	{
		std::cout << parser;
		return 0;
	}
	catch (const args::ParseError& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}

	std::string tc_name = "tc_1";
	if (test_case_name) tc_name = args::get(test_case_name);



	// Set ADC parameters
	double vref = 2.4;// Refernce voltage in V
	double adc_clk_freq = 0.001;// ADC clock frequency in MHz
	uint16_t adc_n_bit = 10;// ADC number of bits
	sca_core::sca_time st_base((1 / adc_clk_freq), sc_core::SC_NS);// Base sample time of TDF modules
	
	
	// Instantiate Test Bench
	tb tb("tb", tc_name, st_base, adc_n_bit, vref);
	

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
		if(tc_name == "tc_2") sca_trace(vcdfile, static_cast<tc_2*>(tb.testcase)->dnl_in_bits, "tb.testcase.dnl_in_bits");
	}

	// Start simulation
	std::cout << "Starting simulation..." << std::endl;
	clock_t begin = clock();
	sc_start(5000000, sc_core::SC_MS);

	// End of simulation
	if (create_vcd) sca_util::sca_close_vcd_trace_file(vcdfile);
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Simulation took " << elapsed_secs << " seconds" << std::endl;
	
	return 0;
}
