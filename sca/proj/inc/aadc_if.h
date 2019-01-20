#pragma once

#include <systemc>
#include <systemc-ams>

class aadc_if
{
public:
	// ADC signals

	// Digital signals
	sc_core::sc_signal<bool> clk{ "clk" };						// AADC Clock
	sc_core::sc_signal<bool> start{ "start" };					// Start of conversion strobe pulse
	sc_core::sc_signal<bool> done{ "done" };					// Conversion done
	sc_core::sc_signal<int16_t> code{ "code" };					// Conversion result code

	// Analog signals
	sc_core::sc_signal<double> vin_drive{ "vin_drive" };		// Input voltage		Drive side
	sc_core::sc_signal<double> vref_drive{ "vref_drive" };		// Reference voltage
	sc_core::sc_signal<double> vin_sense{ "vin_sense" };		// Input voltage		Sense side
	sc_core::sc_signal<double> vref_sense{ "vref_sense" };		// Reference voltage

	// Constructor
	aadc_if(std::string name) {}

};
