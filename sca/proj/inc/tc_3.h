#pragma once

#include <systemc>
#include <systemc-ams>

#include <vector>

#include "aadc_if.h"
#include "aadc_cnfg.h"


// This class is a SystemC module
class tc_3 : public sc_core::sc_module
{

public:

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(tc_3);
	tc_3(const sc_core::sc_module_name& name, bool* enable_checker = nullptr, aadc_if* aadc_vif_ = nullptr, aadc_cnfg* aadc_cfg_ = nullptr
		, sca_core::sca_time st_base_ = sca_core::sca_time(1000.0, sc_core::SC_NS)
		, uint16_t n_bits_ = 10, double vref_ = 1.0)
		: sc_module(name) // Construct parent
		// Initialize local variable
		, aadc_cfg(aadc_cfg_)
		, enable_checker(enable_checker)
		, aadc_vif(aadc_vif_)
		, st_base(st_base_)
		, n_bits(n_bits_)
		, vref(vref_)
	{
		*enable_checker = false;// Disable checker

		std::cout << "Executing test tc_3" << std::endl;

		SC_THREAD(do_stimuli);
		sensitive << aadc_vif->clk.posedge_event();
		dont_initialize();

		SC_METHOD(do_measure_result);
		sensitive << aadc_vif->clk.posedge_event();
		dont_initialize();
	}

	~tc_3() {

	}

protected:
	// Local variables
	aadc_cnfg* aadc_cfg;
	bool* enable_checker;
	aadc_if* aadc_vif;				// ADC virtual interface

	sca_core::sca_time st_base;
	uint16_t n_bits;
	double vref;
	int fine_step;
	double vin = 0;
	std::vector<double> vin_vec;
	std::vector<int16_t> code_vec;



	// Local methods

	void do_stimuli() {

		// Test case 3: Drive Vin and measure Code
		aadc_cfg->integ_gain_error = 0.0;

		// Read input volatge vin from *.csv file
		std::stringstream input_filename;
		input_filename << "./../analysis/" << "vin.csv";
		std::ifstream input_csv_file;
		input_csv_file.open(input_filename.str());
		if (!input_csv_file.is_open()) {
			std::stringstream err_ss;
			err_ss << "Open file error. Can't open " << input_filename.str() << " file.";
			SC_REPORT_ERROR("tc_3", err_ss.str().c_str());
			sc_core::sc_stop();
		}
		std::string line;
		while (std::getline(input_csv_file, line)) {
			vin_vec.push_back(std::stod(line));
		}
		input_csv_file.close();

		// For all Vin values
		for (auto const& vin : vin_vec) {
			// Apply input voltage, reference and start adc conversion
			aadc_vif->vin_drive.write(vin);
			aadc_vif->vref_drive.write(vref);
			wait();
			aadc_vif->start.write(true);
			wait();
			aadc_vif->start.write(false);

			// Wait conversion finished
			wait(n_bits - 1);
		}
		wait();// Wait one more clock cycle to let last conversion finish

		// Save resulting code as *.csv file
		std::stringstream out_filename;
		out_filename << "./../analysis/" << "code.csv";
		std::ofstream out_csv_file;
		out_csv_file.open(out_filename.str());
		for (auto const& code : code_vec) {
			out_csv_file << code << "\n";
		}
		out_csv_file.close();

		// Stop simulation
		sc_core::sc_stop();
	}

	void do_measure_result() {
		static bool done_prev = 0;

		// Get conversion result
		if (!done_prev && aadc_vif->done.read()) {
			// done rose
			// Save resulting code
			code_vec.push_back(aadc_vif->code.read());
		}
		done_prev = aadc_vif->done.read();
	}

};