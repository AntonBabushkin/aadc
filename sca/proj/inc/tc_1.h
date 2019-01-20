#pragma once

#include <systemc>
#include <systemc-ams>

#include "aadc_if.h"
#include "aadc_cnfg.h"


// This class is a SystemC module
class tc_1 : public sc_core::sc_module
{

public:

	// ADC virtual interface
	aadc_if* aadc_vif;

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(tc_1);
	tc_1(const sc_core::sc_module_name& name, bool* enable_checker = nullptr, aadc_if* aadc_vif_ = nullptr, aadc_cnfg* aadc_cfg_ = nullptr
			, sca_core::sca_time st_base_ = sca_core::sca_time(1000.0, sc_core::SC_NS), uint16_t n_bits_ = 10, double vref_ = 1.0)
		: sc_module(name) // Construct parent
		// Initialize local variable
		, aadc_vif(aadc_vif_)
		, st_base(st_base_)
		, n_bits(n_bits_)
		, vref(vref_)
	{
		std::cout << "Executing test tc_1" << std::endl;

		SC_THREAD(do_stimuli);
		sensitive << aadc_vif->clk.posedge_event();
		dont_initialize();
	}

protected:
	// Local variables
	sca_core::sca_time st_base;
	uint16_t n_bits;
	double vref;
	double vin = 0;
	int16_t expected_code;

	// Local methods

	void do_stimuli() {

		// Test case 1: sweep voltage at ADC input from -Vref to +Vref with step (Vref/2^n_bits)
		vin = -1.0*vref;
		//expected_code = -1023;
		for (unsigned int i = 0; i < 2 * pow(2, n_bits) + 1; i++) {
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
			vin += vref / pow(2, n_bits);

		}

		// Stop simulation
		sc_core::sc_stop();
	}

};