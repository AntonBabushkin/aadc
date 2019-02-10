#pragma once

#include <systemc>
#include <systemc-ams>
#include <cci_configuration>

#include "cci_param_sync.h"

// Include ADC components definition
#include "digital_core.h"
#include "analog_core.h"




// This class is a SystemC module
class aadc : sc_core::sc_module
{

public:
	// ADC ports
	sc_core::sc_in<bool> clk;
	sc_core::sc_in<bool> start;
	sca_tdf::sca_in<double> vin, vref;
	sc_core::sc_out<int16_t> code;
	sc_core::sc_out<bool> done;

	// ADC signals
	sc_core::sc_signal<uint16_t> cnt;
	sc_core::sc_signal<bool> p, q;
	sc_core::sc_signal<int16_t> code_raw;
	
	// ADC components instantiation
	digital_core dig_core;				// (DE domain)
	analog_core ana_core;				// (TDF domain)

	// Class (SystemC module) constructor
	aadc(const sc_core::sc_module_name& name_)
		: sc_module(name_) // Construct parent
		// Initialize parameters
		, n_bits("n_bits", 10)
		, gain_err("gain_err", 0.0)
		// Initialize local variables
		
		// ADC components creation
		, dig_core("digital_core")
		, ana_core("analog_core")
	{
		
		// ADC core components interconnect

		dig_core.clk(clk);				// Digital core
		dig_core.start(start);
		dig_core.p(p);
		dig_core.q(q);
		dig_core.cnt(cnt);
		dig_core.done(done);
		dig_core.code(code);
		dig_core.code_raw(code_raw);
		
		ana_core.start(start);			// Analog core
		ana_core.vin(vin);
		ana_core.vref(vref);
		ana_core.p(p);
		ana_core.q(q);

		// Synchronize top level parameters with actual parameters in the sub-block
		cci::cci_param_handle sub_param;
		cci::cci_broker_handle m_broker(cci::cci_get_broker());
		std::vector<cci::cci_param_handle> param_list;// Vector storing parameters to be synchronized
		// Synchronize gain_err top level parameter with actual parameter in the sub-block
		sub_param = m_broker.get_param_handle(name() + std::string(".analog_core.res_integr.gain_err"));
		param_list.clear();
		param_list.push_back(gain_err.create_param_handle());
		param_list.push_back(sub_param);
		param_sync_gain_err = new cci_param_sync("param_sync_gain_err", param_list);// Synchronise parameters value
		gain_err.set_description(sub_param.get_description());// Copy description from sub-parameter
		// Synchronize n_bits top level parameter
		sub_param = m_broker.get_param_handle(name() + std::string(".digital_core.n_bits"));
		param_list.clear();
		param_list.push_back(n_bits.create_param_handle());
		param_list.push_back(sub_param);
		param_sync_n_bits = new cci_param_sync("param_sync_n_bits", param_list);// Synchronise parameters value
		n_bits.set_description(sub_param.get_description());
		

		// Print info
		std::cout << "The module " << name_ << " was constructed at time " << sc_core::sc_time_stamp() << std::endl;
	}
private:
	// Top level parameters
	cci::cci_param<int> n_bits;			// ADC resolution, bits
	cci::cci_param<double> gain_err;	// Gain error, %

protected:
	// Local variables
	cci_param_sync *param_sync_gain_err, *param_sync_n_bits;// CCI parameters synchronisation helpers

};