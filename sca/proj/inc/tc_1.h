#pragma once

#include <systemc>
#include <systemc-ams>

#include "aadc_if.h"


// This class is a SystemC module
class tc_1 : public sc_core::sc_module
{

public:

	// ADC virtual interface
	aadc_if* aadc_vif;

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(tc_1);
	tc_1(const sc_core::sc_module_name& name, bool* enable_checker = nullptr, aadc_if* aadc_vif_ = nullptr
			, sca_core::sca_time st_base_ = sca_core::sca_time(1000.0, sc_core::SC_NS), double vref_ = 1.0)
		: sc_module(name) // Construct parent
		// Get CCI configuration handle specific for this module
		, m_broker(cci::cci_get_broker())
		// Initialize local variable
		, aadc_vif(aadc_vif_)
		, st_base(st_base_)
		, vref(vref_)
	{
		std::cout << "Executing test tc_1" << std::endl;

		SC_THREAD(do_stimuli);
		sensitive << aadc_vif->clk.posedge_event();
		dont_initialize();
	}

private:
	cci::cci_broker_handle m_broker; // CCI configuration handle

protected:
	// Local variables
	sca_core::sca_time st_base;
	double vref;
	double vin = 0;
	int16_t expected_code;

	// Local methods

	void do_stimuli() {
		// Set gain error to 0.0
		//m_broker.get_param_handle("tb.aadc.analog_core.res_integr.gain_err").set_cci_value(cci::cci_value(0.0));
		m_broker.get_param_handle("tb.aadc.gain_err").set_cci_value(cci::cci_value(0.0));

		// Test case 1: sweep voltage at ADC input from -Vref to +Vref with step (Vref/2^n_bits)
		vin = -1.0*vref;
		uint16_t n_bits = m_broker.get_param_handle("tb.aadc.n_bits").get_cci_value().get_int();// Get AADC N bits configuration
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