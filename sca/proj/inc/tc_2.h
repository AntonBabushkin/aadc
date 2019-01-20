#pragma once

#include <systemc>
#include <systemc-ams>

#include <vector>

#include "aadc_if.h"
#include "tinymatwriter.h"

#define CREATE_MAT_FILE_FOR_POST_PROCESSING

// This class is a SystemC module
class tc_2 : sc_core::sc_module
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
			, uint16_t n_bits_ = 10, double vref_ = 1.0)
		: sc_module(name) // Construct parent
		// Initialize local variable
		, enable_checker(enable_checker)
		, aadc_vif(aadc_vif_)
		, st_base(st_base_)
		, n_bits(n_bits_)
		, vref(vref_)
	{
		*enable_checker = false;// Disable checker
		fine_step = 10;	// Amount of voltage steps between two ADC codes = DNL granularity in fractions of LSB

		SC_THREAD(do_stimuli);
		sensitive << aadc_vif->clk.posedge_event();
		dont_initialize();

		SC_METHOD(do_measure_dnl);
		sensitive << aadc_vif->clk.posedge_event();
		dont_initialize();
	}

	~tc_2() {

	}

protected:
	// Local variables
	
	bool* enable_checker;
	aadc_if* aadc_vif;				// ADC virtual interface

	sca_core::sca_time st_base;
	uint16_t n_bits;
	double vref;
	int fine_step;
	double vin = 0;
	std::vector<dnl_vin_code_t> vin_code_dnl_tbl;
	


	// Local methods

	void do_stimuli() {

		// Test case 2: Measure DNL (sweep voltage at ADC input from -Vref to +Vref with fine step (Vref/2^n_bits)/10)
		vin = -1.0*vref;
		vin_code_dnl_tbl.clear();
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
			if (max_dnl < i->dnl) {
				max_dnl = i->dnl;
				max_idx = i - vin_code_dnl_tbl.begin();
			}
		}
		std::cout << "Max DNL error " << (vin_code_dnl_tbl[max_idx].dnl - 1.0) << " LSB at code " << vin_code_dnl_tbl[max_idx].code << std::endl;

#ifdef CREATE_MAT_FILE_FOR_POST_PROCESSING
		// Create .mat file using TinyMAT library https://github.com/jkriege2/TinyMAT
		std::stringstream filename;
		filename << "./../post_processing/" << "vin_code_dnl" << ".mat";
		TinyMATWriterFile* mat = TinyMATWriter_open(filename.str().c_str());
		if (mat) {
			// Convert vector to double array
			double* vin_code_dnl_arr = new double[vin_code_dnl_tbl.size()*3];
			for (auto const& value : vin_code_dnl_tbl) {
				static int i = 0;
				vin_code_dnl_arr[i] = value.vin;
				vin_code_dnl_arr[i+1] = value.code;
				vin_code_dnl_arr[i+2] = value.dnl;
				i += 3;
			}
			int32_t arr_size[3] = { 3,(int32_t)vin_code_dnl_tbl.size(),1 };
			TinyMATWriter_writeMatrixND_rowmajor(mat, "vin_code_dnl", vin_code_dnl_arr, arr_size, 2);
			delete[] vin_code_dnl_arr;
			TinyMATWriter_close(mat);
		}
#endif // CREATE_MAT_FILE_FOR_POST_PROCESSING


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