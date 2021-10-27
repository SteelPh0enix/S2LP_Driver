/*
 * s2lp_rf.h
 *
 *  Created on: 11 sie 2021
 *      Author: steelph0enix
 */

#ifndef S2LP_S2LP_RF_H_
#define S2LP_S2LP_RF_H_

#include "s2lp_mcu_interface.h"

// ==== Radio management ====

// Set charge pump current (see Table 37 on datasheet)
void S2LP_RF_SetChargePumpCurrent(S2LP_Handle* handle, S2LP_ChargePumpCurrent current);
// Set synthesizer band (high band = 826-1055MHz, mid band = 413-527MHz)
void S2LP_RF_SetSynthBand(S2LP_Handle* handle, S2LP_SynthesizerBand band);
// Set synthesizer value (see section 5.3.1 of datasheet)
void S2LP_RF_SetSynthValue(S2LP_Handle* handle, uint32_t value);
// Set base frequency
void S2LP_RF_SetBaseFrequency(S2LP_Handle* handle, uint32_t frequency);
// Set channel spacing. NOTE: this function will write directly to the register.
void S2LP_RF_SetChannelSpacing(S2LP_Handle* handle, uint8_t value);
// Set channel number. This value is multiplied by the channel spacing and added
// to the synthesizer base frequency to generate the actual RF carrier
// frequency. See the datasheet for more info.
void S2LP_RF_SetChannelNumber(S2LP_Handle* handle, uint8_t number);
// Set modulation type
void S2LP_RF_SetModulationType(S2LP_Handle* handle, S2LP_Modulation modulation);
// Set datarate by setting mantissa and exponent parts. Use
// S2LP_RF_CalculateDataRateValue to calculate the real datarate based on these values.
void S2LP_RF_SetDataRateRaw(S2LP_Handle* handle, uint16_t mantissa, uint8_t exponent);
// Set datarate, mantissa and exponent are automatically calculated.
void S2LP_RF_SetDataRate(S2LP_Handle* handle, uint32_t datarate);
// Set frequency deviation mantissa/exponent
void S2LP_RF_SetFrequencyDeviationRaw(S2LP_Handle* handle, uint8_t mantissa, uint8_t exponent);
// Calculates closest factors for frequency deviation and writes it to S2-LP
// Read it from S2-LP to check the real deviation
void S2LP_RF_SetFrequencyDeviation(S2LP_Handle* handle, uint32_t deviation);
// Set constelattion mapping (see table 41 and 42 in datasheet for details)
void S2LP_RF_SetConstellationMapping(S2LP_Handle* handle, uint8_t mapping);
// Set frequency interpolation state for GFSK shaping
void S2LP_RF_SetFrequencyInterpolation(S2LP_Handle* handle, bool state);

// Helper calculation functions - datarate
/* HOW TO USE THEM
 * First, you HAVE to init S2LP_Handle with the correct clock frequency,
 * because it's used in all the calculations.
 *
 * To calculate the real datarate based on it's coefficients (mantissa and exponent),
 * you can use CalculateDataRateValue function.
 *
 * To calculate coefficients for certain datarate, you can use CalculateDataRateCoeffs
 * function. It uses output arguments to return mantissa and exponent. You can check the
 * real datarate by passing these values to CalculateDataRateValue - it may be slightly
 * different from expected value, usually by margin of few bytes per second.
 *
 * To calculate mantissa or exponent separately, you can use CalculateDataRateExponent
 * and CalculateDataRateMantissa functions. These are used in CalculateDataRateCoeffs,
 * which is used in SetDataRate function of this module.
 */

// Calculate the sync word for specified frequency. Does not access S2-LP
uint32_t S2LP_RF_CalculateSyncForBaseFrequency(S2LP_Handle* handle, uint32_t frequency);

// Calculate the datarate based on mantissa, exponent and S2-LP clock frequency.
// Does not access S2-LP.
uint32_t S2LP_RF_CalculateDataRateValue(S2LP_Handle* handle, uint16_t mantissa, uint8_t exponent);
// Calculate the closest exponent for specified datarate. Does not access S2-LP
// Returns S2LP_DATARATE_EXPONENT_INVALID on invalid input.
uint8_t S2LP_RF_CalculateDataRateExponent(S2LP_Handle* handle, uint32_t datarate);
// Calculate the closest mantissa for specified data rate and exponent.
// Does not access S2-LP. There's no real way to check for errors here,
// so double-check it by re-calculating datarate.
uint16_t S2LP_RF_CalculateDataRateMantissa(S2LP_Handle* handle, uint32_t datarate, uint8_t exponent);
// Calculate the closest mantissa and exponent for specified datarate.
// Does not access S2-LP
void S2LP_RF_CalculateDataRateCoeffs(S2LP_Handle* handle, uint32_t datarate, uint16_t* mantissa, uint8_t* exponent);

// Helper calculation functions - channel base frequency and spacing

// Returns the base frequency based on frequency band and synthesizer value
// Frequency band and reference divider state is read from S2-LP.
double S2LP_RF_CalculateBaseFrequency(S2LP_Handle* handle, uint32_t synth_value);

// Returns the center frequency of channel, specified by base frequency and channel config
double S2LP_RF_CalculateCenterFrequency(S2LP_Handle* handle, double base_frequency, uint8_t channel_spacing,
		uint8_t channel_number);

// Returns the resolution of base frequency (by calculating the frequency difference
// between calculated frequencies with different synth values)
// Reads information about the current band from S2-LP
double S2LP_RF_CalculateBaseFreqResolution(S2LP_Handle* handle);

// Returns the channel spacing resolution (by calculating the frequency difference
// between neighbour channels)
double S2LP_RF_CalculateChannelResolution(S2LP_Handle* handle, double base_frequency);

// Calculate frequency deviation. Reads info about reference divider and
// synthesizer band from S2-LP
double S2LP_RF_CalculateFrequencyDeviation(S2LP_Handle* handle, uint8_t mantissa, uint8_t exponent);

// Calculate minimal frequency deviation exponent for specified frequency deviation
// Reads the data about band and refdiv from S2-LP
uint8_t S2LP_RF_CalculateFreqDevExponent(S2LP_Handle* handle, uint32_t deviation);

// Calculate closest mantissa for specified deviation with specified exponent
// Reads the data about band and refdiv from S2-LP (calls CalculateFrequencyDeviation function)
uint8_t S2LP_RF_CalculateFreqDevMantissa(S2LP_Handle* handle, uint8_t exponent, uint32_t deviation);

// Getters. Self-explanatory. See functions above and documentation for details.
S2LP_ChargePumpCurrent S2LP_RF_GetChargePumpCurrent(S2LP_Handle* handle);
S2LP_SynthesizerBand S2LP_RF_GetSynthBand(S2LP_Handle* handle);
uint32_t S2LP_RF_GetSynthValue(S2LP_Handle* handle);
uint8_t S2LP_RF_GetChannelSpacing(S2LP_Handle* handle);
uint8_t S2LP_RF_GetChannelNumber(S2LP_Handle* handle);
S2LP_Modulation S2LP_RF_GetModulationType(S2LP_Handle* handle);
uint32_t S2LP_RF_GetDataRate(S2LP_Handle* handle);
uint16_t S2LP_RF_GetDataRateMantissa(S2LP_Handle* handle);
uint8_t S2LP_RF_GetDataRateExponent(S2LP_Handle* handle);
uint8_t S2LP_RF_GetFreqDevMantissa(S2LP_Handle* handle);
uint8_t S2LP_RF_GetFreqDevExponent(S2LP_Handle* handle);
double S2LP_RF_GetFrequencyDeviation(S2LP_Handle* handle);
uint8_t S2LP_RF_GetConstellationMapping(S2LP_Handle* handle);
bool S2LP_RF_GetFrequencyInterpolation(S2LP_Handle* handle);

#endif /* S2LP_S2LP_RF_H_ */
