#pragma once
#include <systemc>
#include <stdio.h>

// This class is a SystemC module
class digital_core : sc_core::sc_module
{

public:

	// Ports declaration
	sc_core::sc_in<bool> clk;						// Clock
	sc_core::sc_in<bool> start;						// Start conversion (strobe pulse)
	sc_core::sc_in<bool> p, q;						// 1.5-bit comparator output
	sc_core::sc_out<uint16_t> cnt;					// Counter value
	sc_core::sc_out<bool> done;						// End of conversion
	sc_core::sc_out<int16_t> code;					// Conversion result code
	sc_core::sc_out<int16_t> code_raw;				// Raw conversion code (updated after each cycle)

	// 
	bool conversion_running = false;

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(digital_core);// Declare that this module have a process
	digital_core(const sc_core::sc_module_name& name)
		: sc_module(name) // Construct parent
		// Initialize parameters
		, n_bits("n_bits", 10, "ADC resolution, in bits")
	{
		// Initialize
		ctrl_cnt = 0;
		conversion_running = false;

		// Start SystemC threads

		// ADC Control thread
		SC_METHOD(do_control);
		sensitive << clk.pos();
		dont_initialize();

		// ADC Data reconstruction thread
		SC_METHOD(do_data_reconstruction);
		sensitive << clk.pos();
		dont_initialize();

	}

private:
	cci::cci_param<int> n_bits;

protected:
	// Local variables
	uint16_t ctrl_cnt;
	int16_t code_val, bit_val, bit_weight;

	// Control thread
	void do_control() {

		// Check if start of conversion
		if (start.read()) {
			done.write(false);
			ctrl_cnt = 0;
			conversion_running = true;// Set running flag
		}

		// Pefrom control operation once conversion has started
		if (conversion_running) {
			cnt.write(ctrl_cnt);// Update counter value

			// Check if conversion finished
			if (ctrl_cnt == n_bits - 1) {
				// Conversion finished
				done.write(true);
				ctrl_cnt = 0;
				conversion_running = false;// Clear running flag
			}
			else {
				ctrl_cnt++;// Count up
			}
		}
	}

	// Data reconstruction thread
	void do_data_reconstruction() {

		// Check if start of conversion
		if (start.read()) {
			code_val = 0;
		}

		// Reconstruct code from p and q
		pq_to_bit_val(p.read(), q.read(), &bit_val);// Convert p, q to bit value (-1, 0 or +1)
		bit_weight = (int16_t)pow(2, ((n_bits - ctrl_cnt) - 1));
		code_val += bit_val*bit_weight;// Multiply bit value by it's weight
		code_raw.write(code_val);

		if (ctrl_cnt == n_bits - 1) {
			// Conversion finished
			code.write(code_val);
			code_val = 0;
		}
	}

	// Convert 1.5-bit comparator decision (p and q) to actual bit value
	void pq_to_bit_val(bool p, bool q, int16_t *bit_val) {
		*bit_val = 0;
		if (p == 1 && q == 0)
			* bit_val = +1;// Vin >= Vref / 4, Vdac = -Vref
		else if (p == 0 && q == 1)
			* bit_val = -1;// Vin <= -Vref / 4, Vdac = +Vref
		else if (p == 0 && q == 0)
			* bit_val = 0;// -Vref / 4 < Vin < Vref / 4, Vdac = 0 * Vref
	}

};