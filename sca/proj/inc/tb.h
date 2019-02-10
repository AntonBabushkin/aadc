#pragma once

#include <systemc>
#include <systemc-ams>


#include "aadc.h"
#include "clk_gen.h"

#include "aadc_if.h"
#include "aadc_scoreboard.h"
#include "tc_1.h"
#include "tc_2.h"
#include "tc_3.h"



// This class is a SystemC module
class tb : public sc_core::sc_module
{

public:
	bool enable_checker = true;

	// ADC virtual interface
	aadc_if* aadc_vif = new aadc_if("aadc_vif");

	sca_tdf::sca_signal<double> vin_tdf{ "vin_tdf" };		// Input voltage
	sca_tdf::sca_signal<double> vref_tdf{ "vref_tdf" };		// Reference voltage

	// TDF <-> DE connector modules
	de2tdf<double> de2tdf_vin;// Objects created in constructor, since sampling time should be assigned
	de2tdf<double> de2tdf_vref;
	tdf2de<double> tdf2de_vin{ "tdf2de_vin" };
	tdf2de<double> tdf2de_vref{ "tdf2de_vref" };

	// Instantiate modules
	aadc aadc;							// Algorithmic ADC module
	clk_gen clk_gen;					// Clock generator
	aadc_scoreboard aadc_scoreboard;	// Scoreboard

	sc_module* testcase;

	// Class (SystemC module) constructor
	tb(const sc_core::sc_module_name& name, std::string tc_name
			, sca_core::sca_time st_base_ = sca_core::sca_time(1000.0, sc_core::SC_NS)
			, uint16_t n_bits_ = 10, double vref_ = 1.0)
		: sc_module(name) // Construct parent
		// Initialize local variables

		// Construct modules
		, de2tdf_vin("de2tdf_vin", st_base_)
		, de2tdf_vref("de2tdf_vref", st_base_)
		, aadc("aadc")
		, clk_gen("clk_gen", st_base_)
		, aadc_scoreboard("aadc_scoreboard", &enable_checker, aadc_vif, st_base_, vref_)
	{
		
		// Create test
		if (tc_name == "tc_1") testcase = new tc_1("testcase", &enable_checker, aadc_vif, st_base_, vref_);
		if (tc_name == "tc_2") testcase = new tc_2("testcase", &enable_checker, aadc_vif, st_base_, vref_);
		if (tc_name == "tc_3") testcase = new tc_3("testcase", &enable_checker, aadc_vif, st_base_, vref_);
		
		// Configure AADC (DUT)
		cci::cci_broker_handle m_broker(cci::cci_get_broker());
		m_broker.get_param_handle("tb.aadc.n_bits").set_cci_value(cci::cci_value(n_bits_));

		// TDF drive side connector
		de2tdf_vin.in(aadc_vif->vin_drive);
		de2tdf_vin.out(vin_tdf);
		de2tdf_vref.in(aadc_vif->vref_drive);
		de2tdf_vref.out(vref_tdf);

		// TDF sense side connector
		tdf2de_vin.in(vin_tdf);
		tdf2de_vin.out(aadc_vif->vin_sense);
		tdf2de_vref.in(vref_tdf);
		tdf2de_vref.out(aadc_vif->vref_sense);

		// Connect Algorithmic ADC module
		aadc.clk(aadc_vif->clk);
		aadc.start(aadc_vif->start);
		aadc.code(aadc_vif->code);
		aadc.done(aadc_vif->done);
		aadc.vin(vin_tdf);
		aadc.vref(vref_tdf);

		// Connect clock generator
		clk_gen.clk(aadc_vif->clk);

	}

protected:
	// Local variables

	// Local methods

};