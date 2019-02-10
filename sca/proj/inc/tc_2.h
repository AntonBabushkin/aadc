#pragma once

#include <systemc>
#include <systemc-ams>
#include <cci_configuration>

#include <vector>

#include "aadc_if.h"

#define CREATE_CSV_FILE_FOR_POST_PROCESSING

// This class is a SystemC module
class tc_2 : public sc_core::sc_module
{

public:

	struct dnl_vin_code_t {
		double vin;
		int16_t code;
		double dnl;
	};

	sc_core::sc_signal<double> dnl_in_bits{ "dnl_in_bits" };

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(tc_2);
	tc_2(const sc_core::sc_module_name& name, bool* enable_checker = nullptr, aadc_if* aadc_vif_ = nullptr
			, sca_core::sca_time st_base_ = sca_core::sca_time(1000.0, sc_core::SC_NS)
			, double vref_ = 1.0)
		: sc_module(name) // Construct parent
		// Get CCI configuration handle specific for this module
		, m_broker(cci::cci_get_broker())
		// Initialize local variable
		, enable_checker(enable_checker)
		, aadc_vif(aadc_vif_)
		, st_base(st_base_)
		, vref(vref_)
	{
		*enable_checker = false;// Disable checker
		fine_step = 10;	// Amount of voltage steps between two ADC codes = DNL granularity in fractions of LSB

		std::cout << "Executing test tc_2" << std::endl;

		SC_THREAD(do_stimuli);
		sensitive << aadc_vif->clk.posedge_event();
		dont_initialize();

		SC_METHOD(do_measure_dnl);
		sensitive << aadc_vif->clk.posedge_event();
		dont_initialize();
	}

	~tc_2() {

	}

private:
	cci::cci_broker_handle m_broker; // CCI configuration handle

protected:
	// Local variables
	bool* enable_checker;
	aadc_if* aadc_vif;				// ADC virtual interface

	sca_core::sca_time st_base;
	double vref;
	int fine_step;
	double vin = 0;
	std::vector<dnl_vin_code_t> vin_code_dnl_tbl;
	


	// Local methods

	void do_stimuli() {

		// Test case 2: Measure DNL for several integrator gain errors
		std::vector<double> gain_error_vect = { 0.0, 0.01, 0.05, 0.1, 0.5, 1.0 };

		for (auto const& gain_error : gain_error_vect) {

			m_broker.get_param_handle("tb.aadc.gain_err").set_cci_value(cci::cci_value(gain_error));
			std::cout << "Measuring DNL with amplifier gain error = " << gain_error << " %" << std::endl;

			// Measure DNL (sweep voltage at ADC input from -Vref to +Vref with fine step (Vref/2^n_bits)/10)
			vin = -1.0*vref;
			vin_code_dnl_tbl.clear();
			uint16_t n_bits = m_broker.get_param_handle("tb.aadc.n_bits").get_cci_value().get_int();// Get AADC N bits configuration
			for (unsigned int i = 0; i < (2 * pow(2, n_bits)) * fine_step + 1; i++) {
				// Apply input voltage, reference and start adc conversion
				aadc_vif->vin_drive.write(vin);
				aadc_vif->vref_drive.write(vref);
				wait();
				aadc_vif->start.write(true);
				wait();
				aadc_vif->start.write(false);

				// Wait conversion finished
				wait(n_bits - 1);

				// Increnent voltage
				vin += (vref / pow(2, n_bits)) / fine_step;

			}

			// Find max DNL error
			int max_idx;
			for (auto i = vin_code_dnl_tbl.begin(); i != vin_code_dnl_tbl.end(); i++)
			{
				static double max_dnl = 0;
				if (std::abs(max_dnl) < std::abs(i->dnl)) {
					max_dnl = i->dnl;
					max_idx = i - vin_code_dnl_tbl.begin();
				}
			}
			std::cout << "Max DNL error " << (vin_code_dnl_tbl[max_idx].dnl - 1.0) << " LSB at code " << vin_code_dnl_tbl[max_idx].code << std::endl;

#ifdef CREATE_CSV_FILE_FOR_POST_PROCESSING
			std::stringstream filename;
			std::string gain_error_str = std::to_string(gain_error);
			std::replace(gain_error_str.begin(), gain_error_str.end(), '.', 'p');
			filename << "./../analysis/" << "vin_code_dnl_" << gain_error_str << ".csv";
			std::ofstream myfile;
			myfile.open(filename.str());

			for (auto const& value : vin_code_dnl_tbl) {
				myfile << value.vin << "," << value.code << "\n";
			}
			myfile.close();
#endif // CREATE_CSV_FILE_FOR_POST_PROCESSING
		}

		// Stop simulation
		sc_core::sc_stop();
	}

	void do_measure_dnl() {
		static bool done_prev = 0, start_prev = 0;
		static double cur_vin, prev_vin = 0;
		static int16_t prev_code = 0;

		// Memorize voltage at conversion start
		if (!start_prev && aadc_vif->start.read()) {
			// start rose
			cur_vin = aadc_vif->vin_sense.read();
		}

		// Get conversion result and check vs expected value
		if (!done_prev && aadc_vif->done.read()) {
			// done rose
			// Check if code transition occurred
			if (prev_code != aadc_vif->code.read()) {
				
				// Compute DNL at this point
				uint16_t n_bits = m_broker.get_param_handle("tb.aadc.n_bits").get_cci_value().get_int();// Get AADC N bits configuration
				dnl_in_bits = (cur_vin - prev_vin)/(vref / pow(2, n_bits));
				if (aadc_vif->code.read() <= -1022) dnl_in_bits = 0;

				// Save input voltage and code at wich transition occured and DNL
				vin_code_dnl_tbl.push_back({ cur_vin, aadc_vif->code.read(), dnl_in_bits});

				prev_vin = cur_vin;
			}
			prev_code = aadc_vif->code.read();
		}

		start_prev = aadc_vif->start.read();
		done_prev = aadc_vif->done.read();

	}

};