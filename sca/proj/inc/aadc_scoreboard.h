#pragma once

#include <systemc>
#include <systemc-ams>

#include "aadc_if.h"
#include "aadc_cnfg.h"
#include "tdf_de_converters.h"

// This class is a SystemC module
class aadc_scoreboard : sc_core::sc_module
{

public:

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(aadc_scoreboard);
	aadc_scoreboard(const sc_core::sc_module_name& name, bool* enable_checker = nullptr, aadc_if* aadc_vif_ = nullptr, aadc_cnfg* aadc_cfg_ = nullptr
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
		SC_METHOD(do_check);
		sensitive << aadc_vif->clk.posedge_event();
		dont_initialize();
	}

protected:
	// Local variables

	bool* enable_checker;
	aadc_if* aadc_vif;				// ADC virtual interface
	
	sca_core::sca_time st_base;
	uint16_t n_bits;
	double vref;
	double vin = 0;
	int16_t expected_code;

	// Local methods

	void do_check() {
		static bool done_prev = 0, start_prev = 0;

		// Calculate expected ADC code
		if (!start_prev && aadc_vif->start.read()) {
			// aadc_vif->start rose
			expected_code = (int16_t)std::round((aadc_vif->vin_sense.read() / aadc_vif->vref_sense.read()) * pow(2, n_bits));
			if (expected_code == -1024) expected_code = -1023;// Algorithmic ADC doesn't produce code -1024
		}

		// Get conversion result and check vs expected value
		if (!done_prev && aadc_vif->done.read()) {
			// adc_done rose
			if (aadc_vif->code.read() != expected_code) {
				if(*enable_checker) std::cout << "ADC code mismatch " << "Exp[" << expected_code << "], Got[" << aadc_vif->code.read() << "] at time " << sc_core::sc_time_stamp() << std::endl;
			}
		}

		start_prev = aadc_vif->start.read();
		done_prev = aadc_vif->done.read();
	}

};