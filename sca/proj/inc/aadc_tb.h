#pragma once

#include <systemc>
#include <systemc-ams>

#include "aadc_if.h"
#include "tdf_de_converters.h"


// This class is a SystemC module
class aadc_tb : sc_core::sc_module
{

public:
	
	// ADC virtual interface
	aadc_if* aadc_vif;

	de2tdf<double> de2tdf_vin;
	de2tdf<double> de2tdf_vref;

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(aadc_tb);
	aadc_tb(const sc_core::sc_module_name& name, aadc_if* aadc_vif_ = nullptr, sca_core::sca_time st_base_=sca_core::sca_time(1000.0, sc_core::SC_NS), uint16_t n_bits_=10, double vref_=1.0)
		: sc_module(name) // Construct parent
		// Initialize local variable
		, aadc_vif(aadc_vif_)
		, st_base(st_base_)
		, n_bits(n_bits_)
		, vref(vref_)
		// Construct modules (and assign sampling time for TDF data producers)
		, de2tdf_vin("de2tdf_vin", st_base_)
		, de2tdf_vref("de2tdf_vref", st_base_)
	{
		de2tdf_vin.in(vin_de);
		de2tdf_vin.out(aadc_vif->vin);
		
		de2tdf_vref.in(vref_de);
		de2tdf_vref.out(aadc_vif->vref);

		SC_THREAD(do_clk_gen);

		SC_THREAD(do_stimuli);
		sensitive << aadc_vif->clk.posedge_event();
		dont_initialize();
		
		SC_METHOD(do_check);
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

	sc_core::sc_signal<double> vin_de, vref_de;

	// Local methods

	void do_clk_gen() {
		// Wait for clock intialization time
		aadc_vif->clk.write(false);
		wait(st_base);
		// Start clock generation
		while (true) {
			aadc_vif->clk.write(true);
			wait(st_base / 2);
			aadc_vif->clk.write(false);
			wait(st_base / 2);
		}
	}

	void do_stimuli() {

		// Test case 1: sweep voltage at ADC input from -Vref to +Vref with step (Vref/2^n_bits)
		vin = -1.0*vref;
		//expected_code = -1023;
		for (unsigned int i = 0; i < 2*pow(2, n_bits)+1; i++) {
			// Apply input voltage, reference and start adc conversion
			vin_de = vin;
			vref_de = vref;
			wait();
			aadc_vif->start.write(true);
			wait();
			aadc_vif->start.write(false);

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
		if (!start_prev && aadc_vif->start.read()) {
			// aadc_vif->start rose
			expected_code = std::round((vin_de.read() / vref_de.read()) * pow(2, n_bits));
			if (expected_code == -1024) expected_code = -1023;// Algorithmic ADC doesn't produce code -1024
		}

		// Get conversion result and check vs expected value
		if (!done_prev && aadc_vif->done.read()) {
			// adc_done rose
			if (aadc_vif->code.read() != expected_code) {
				std::cout << "ADC code mismatch " << "Exp[" << expected_code << "], Got[" << aadc_vif->code.read() << "] at time " << sc_core::sc_time_stamp() << std::endl;
			}
		}

		start_prev = aadc_vif->start.read();
		done_prev = aadc_vif->done.read();
	}

};