#pragma once
#include <systemc>
#include <stdio.h>

// This class is a SystemC module
class digital_core : sc_core::sc_module
{

public:

	// Ports declaration
	sc_core::sc_in_clk clk;
	sc_core::sc_in<bool> start;			// Start conversion
	sc_core::sc_in<bool> p, q;
	sc_core::sc_out<uint16_t> cnt;
	sc_core::sc_out<bool> done;
	sc_core::sc_out<int16_t> code, code_raw;

	// Class (SystemC module) constructor
	SC_HAS_PROCESS(digital_core);// Declare that this module have a process
	digital_core(const sc_core::sc_module_name& name, uint16_t n_bits_ = 10)
		: sc_module(name) // Construct parent
		, n_bits(n_bits_)
	{
		// Initialize
		ctrl_cnt = 0;
		adc_en = false;

		// Start SystemC threads

		// ADC Control thread
		SC_METHOD(do_control);
		sensitive << clk.pos();
		dont_initialize();

		// ADC Data reconstruction thread
		SC_METHOD(do_data_reconstruction);
		sensitive << clk.pos();
		dont_initialize();

		// Print info
		std::cout << "The module " << name << " was constructed"
			<< " at time " << sc_core::sc_time_stamp() << std::endl;
	}

protected:
	// Local variables
	uint16_t n_bits;
	bool adc_en;
	uint16_t ctrl_cnt;
	int16_t code_val, bit_val, bit_weight;

	// Control thread
	void do_control() {
		if (start.read()) {
			done.write(false);
			ctrl_cnt = 0;
			adc_en = true;
		}
		if (adc_en) {
			cnt.write(ctrl_cnt);

			if (ctrl_cnt == n_bits - 1) {
				// Conversion finished
				done.write(true);
				ctrl_cnt = 0;
				adc_en = false;
			}
			else {
				ctrl_cnt++;// Count up
			}
		}
	}

	// Data reconstruction thread
	void do_data_reconstruction() {
		if (start.read()) {
			code_val = 0;
		}

		// Reconstruct code from p and q
		pq_to_bit_val(p.read(), q.read(), &bit_val);// Convert p, q to bit value (-1, 0 or +1)
		bit_weight = (int16_t)pow(2, ((n_bits - ctrl_cnt) - 1));
		code_val += bit_val*bit_weight;
		code_raw.write(code_val);

		if (ctrl_cnt == n_bits - 1) {
			// Conversion finished
			code.write(code_val);
			code_val = 0;
		}
	}

	void pq_to_bit_val(bool p, bool q, int16_t *bit_val) {
		*bit_val = 0;
		if (p == 1 && q == 0)
			// Vin >= Vref / 4
			* bit_val = +1;// -Vref
		else if (p == 0 && q == 1)
			// Vin <= -Vref / 4
			* bit_val = -1;// +Vref
		else if (p == 0 && q == 0)
			// -Vref / 4 < Vin < Vref / 4
			* bit_val = 0;// 0 * Vref
	}

};