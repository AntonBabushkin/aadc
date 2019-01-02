#pragma once

#include <systemc>
#include <systemc-ams>

#include "tdf_de_converters.h"


// This class is a SystemC module
class aadc_tb : sc_core::sc_module
{

public:

	sc_core::sc_in_clk adc_clk;
	sc_core::sc_out<bool> adc_start;
	sca_tdf::sca_out<double> adc_vin, adc_vref;
	sc_core::sc_in<int16_t> adc_code;
	sc_core::sc_in<bool> adc_done;

	de2tdf<double> de2tdf_vin;
	de2tdf<double> de2tdf_vref;

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(aadc_tb);
	aadc_tb(const sc_core::sc_module_name& name, sca_core::sca_time st_base_ = sca_core::sca_time(1000.0, sc_core::SC_NS), uint16_t n_bits_ = 10, double vref_ = 1.0)
		: sc_module(name) // Construct parent
		// Initialize local variable
		, n_bits(n_bits_)
		, vref(vref_)
		// Construct modules (and assign sampling time for TDF data producers)
		, de2tdf_vin("de2tdf_vin", st_base_)
		, de2tdf_vref("de2tdf_vref", st_base_)
	{
		de2tdf_vin.in(vin_de);
		de2tdf_vin.out(adc_vin);
		
		de2tdf_vref.in(vref_de);
		de2tdf_vref.out(adc_vref);

		SC_THREAD(do_stimuli);
		sensitive << adc_clk.pos();
		dont_initialize();
		
		SC_METHOD(do_check);
		sensitive << adc_clk.pos();
		dont_initialize();
	}

protected:
	// Local variables
	uint16_t n_bits;
	double vref;
	double vin = 0;
	int16_t expected_code;

	sc_core::sc_signal<double> vin_de, vref_de;

	// Local methods

	void do_stimuli() {

		// Test case 1: sweep voltage at ADC input from -Vref to +Vref with step (Vref/2^n_bits)
		vin = -1.0*vref;
		//expected_code = -1023;
		for (unsigned int i = 0; i < 2*pow(2, n_bits)+1; i++) {
			// Apply input voltage, reference and start adc conversion
			vin_de = vin;
			vref_de = vref;
			wait();
			adc_start.write(true);
			wait();
			adc_start.write(false);

			// Wait conversion finished
			wait(n_bits-1);

			// Increnent voltage
			vin += vref / pow(2, n_bits);

			//if (i) expected_code += 1;
		}

		// Stop simulation
		sc_core::sc_stop();
	}

	void do_check() {
		static bool done_prev = 0, start_prev = 0;
		
		// Calculate expected ADC code
		if (!start_prev && adc_start.read()) {
			// adc_start rose
			expected_code = std::round((vin_de.read() / vref_de.read()) * pow(2, n_bits));
			if (expected_code == -1024) expected_code = -1023;// Algorithmic ADC doesn't produce code -1024
		}

		// Get conversion result and check vs expected value
		if (!done_prev && adc_done.read()) {
			// adc_done rose
			if (adc_code.read() != expected_code) {
				std::cout << "ADC code mismatch " << "Exp[" << expected_code << "], Got[" << adc_code.read() << "] at time " << sc_core::sc_time_stamp() << std::endl;
			}
		}

		start_prev = adc_start.read();
		done_prev = adc_done.read();
	}

};