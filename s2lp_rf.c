/*
 * s2lp_rf.c
 *
 *  Created on: 11 sie 2021
 *      Author: steelph0enix
 */

#include "s2lp_rf.h"
#include "bit_helpers.h"
#include "s2lp.h"
#include <math.h>

void S2LP_RF_SetChargePumpCurrent(S2LP_Handle* handle, S2LP_ChargePumpCurrent current) {
	uint8_t isel_value = 0;
	bool pfd_split = false;

	switch (current) {
		case S2LP_CHARGE_PUMP_120UA:
			isel_value = 0b010;
			pfd_split = false;
			break;
		case S2LP_CHARGE_PUMP_200UA:
			isel_value = 0b001;
			pfd_split = true;
			break;
		case S2LP_CHARGE_PUMP_140UA:
			isel_value = 0b011;
			pfd_split = false;
			break;
		case S2LP_CHARGE_PUMP_240UA:
			isel_value = 0b010;
			pfd_split = true;
			break;
		case S2LP_CHARGE_PUMP_INVALID:
		default:
			return;
	}

	uint8_t synt3_val = S2LP_ReadRegister(handle, S2LP_REG_SYNT3);
	uint8_t synth_config_val = S2LP_ReadRegister(handle, S2LP_REG_SYNTH_CONFIG2);

	CLEARBITS(synt3_val, 0b111, 5);
	SETBITS(synt3_val, isel_value, 0b111, 5);

	if (pfd_split) {
		SETBIT(synth_config_val, 2);
	} else {
		CLEARBIT(synth_config_val, 2);
	}

	S2LP_WriteRegister(handle, S2LP_REG_SYNT3, synt3_val);
	S2LP_WriteRegister(handle, S2LP_REG_SYNTH_CONFIG2, synth_config_val);
}

void S2LP_RF_SetSynthBand(S2LP_Handle* handle, S2LP_SynthesizerBand band) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_SYNT3);
	switch (band) {
		case S2LP_SYNTH_BAND_HIGH:
			CLEARBIT(reg_val, 4);
			break;
		case S2LP_SYNTH_BAND_MID:
			SETBIT(reg_val, 4);
			break;
		default:
			break;
	}

	S2LP_WriteRegister(handle, S2LP_REG_SYNT3, reg_val);
}

void S2LP_RF_SetSynthValue(S2LP_Handle* handle, uint32_t value) {
	// Register order is reversed (index 0 is SYNT3, 1 is SYNT2, and so on)
	uint8_t reg_vals[4] = { 0 };

	// 3 out of 4 SYNT registers are used only for storing divider,
	// so i'm gonna read only SYNT3 as it contains other settings
	reg_vals[0] = S2LP_ReadRegister(handle, S2LP_REG_SYNT3);
	CLEARBITS(reg_vals[0], 0b1111, 0);

	// Write the values to correct registers
	SETBITS(reg_vals[0], GETBITS(value, 0b1111, 24), 0b1111, 0);
	reg_vals[1] = GETBITS(value, 0xFF, 16);
	reg_vals[2] = GETBITS(value, 0xFF, 8);
	reg_vals[3] = GETBITS(value, 0xFF, 0);

	S2LP_BatchWriteRegisters(handle, S2LP_REG_SYNT3, reg_vals, 4);
}

void S2LP_RF_SetBaseFrequency(S2LP_Handle* handle, uint32_t frequency) {
	uint32_t sync_word = S2LP_RF_CalculateSyncForBaseFrequency(handle, frequency);
	S2LP_RF_SetSynthValue(handle, sync_word);
}

void S2LP_RF_SetChannelSpacing(S2LP_Handle* handle, uint8_t value) {
	S2LP_WriteRegister(handle, S2LP_REG_CHSPACE, value);
}

void S2LP_RF_SetChannelNumber(S2LP_Handle* handle, uint8_t number) {
	S2LP_WriteRegister(handle, S2LP_REG_CHNUM, number);
}

void S2LP_RF_SetModulationType(S2LP_Handle* handle, S2LP_Modulation modulation) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_MOD2);
	CLEARBITS(reg_val, 0b1111, 4);
	SETBITS(reg_val, modulation, 0b1111, 4);
	S2LP_WriteRegister(handle, S2LP_REG_MOD2, reg_val);
}

void S2LP_RF_SetDataRateRaw(S2LP_Handle* handle, uint16_t mantissa, uint8_t exponent) {
	// Normalize exponent, just in case
	exponent = (exponent > 15 ? 15 : exponent);

	uint8_t reg_vals[3] = { 0 };
	reg_vals[2] = S2LP_ReadRegister(handle, S2LP_REG_MOD2);

	// Put mantissa in MOD4/MOD3, and exponent in MOD2
	reg_vals[0] = GETBITS(mantissa, 0xFF, 8);
	reg_vals[1] = GETBITS(mantissa, 0xFF, 0);
	CLEARBITS(reg_vals[2], 0b1111, 0);
	SETBITS(reg_vals[2], exponent, 0b1111, 0);

	S2LP_BatchWriteRegisters(handle, S2LP_REG_MOD4, reg_vals, 3);
}

void S2LP_RF_SetDataRate(S2LP_Handle* handle, uint32_t datarate) {
	uint8_t exponent = 0;
	uint16_t mantissa = 0;
	S2LP_RF_CalculateDataRateCoeffs(handle, datarate, &mantissa, &exponent);
	S2LP_RF_SetDataRateRaw(handle, mantissa, exponent);
}

void S2LP_RF_SetFrequencyDeviationRaw(S2LP_Handle* handle, uint8_t mantissa, uint8_t exponent) {
	if (exponent > 0b1111) {
		return;
	}

	uint8_t mod_vals[2] = { 0 };
	mod_vals[0] = S2LP_ReadRegister(handle, S2LP_REG_MOD1);

	CLEARBITS(mod_vals[0], 0b1111, 0);
	SETBITS(mod_vals[0], exponent, 0b1111, 0);
	mod_vals[1] = mantissa;

	S2LP_BatchWriteRegisters(handle, S2LP_REG_MOD1, mod_vals, 2);
}

void S2LP_RF_SetFrequencyDeviation(S2LP_Handle* handle, uint32_t deviation) {
	uint8_t const exponent = S2LP_RF_CalculateFreqDevExponent(handle, deviation);
	uint8_t const mantissa = S2LP_RF_CalculateFreqDevMantissa(handle, exponent, deviation);
	S2LP_RF_SetFrequencyDeviationRaw(handle, mantissa, exponent);
}

void S2LP_RF_SetConstellationMapping(S2LP_Handle* handle, uint8_t mapping) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_MOD1);
	CLEARBITS(reg_val, 0b11, 4);
	SETBITS(reg_val, mapping, 0b11, 4);
	S2LP_WriteRegister(handle, S2LP_REG_MOD1, reg_val);
}

void S2LP_RF_SetFrequencyInterpolation(S2LP_Handle* handle, bool state) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_MOD1);
	if (state) {
		SETBIT(reg_val, 6);
	} else {
		CLEARBIT(reg_val, 6);
	}

	S2LP_WriteRegister(handle, S2LP_REG_MOD1, reg_val);
}

uint32_t S2LP_RF_CalculateSyncForBaseFrequency(S2LP_Handle* handle, uint32_t frequency) {
	double const fbase = frequency;
	double const pll_div = (S2LP_RF_GetSynthBand(handle) == S2LP_SYNTH_BAND_HIGH ? 4.0 : 8.0);
	double const ref_div = (S2LP_IsRefDivEnabled(handle) ? 2.0 : 1.0);
	double const fxo = S2LP_GetClockFrequency(handle);
	double const pow20 = (1 << 20);

	double const div_const = ((pll_div / 2.0) * ref_div) / fxo;
	return (uint32_t) (fbase * div_const * pow20);
}

uint32_t S2LP_RF_CalculateDataRateValue(S2LP_Handle* handle, uint16_t mantissa, uint8_t exponent) {
	double datarate = 0;
	double fdig = S2LP_GetDigitalClockFrequency(handle);

	if (exponent == S2LP_DATARATE_EXPONENT_MIN) {
		double const pow_32 = (1ull << 32ull);
		datarate = (double) fdig * ((double) mantissa / pow_32);
	} else if (exponent == S2LP_DATARATE_EXPONENT_MAX) {
		datarate = fdig / (8.0 * (double) mantissa);
	} else {
		double const pow_33 = (1ull << 33ull);
		double const pow_16 = (1ull << 16ull);
		double const exponent_pow = (1 << exponent);
		double const nominator = (pow_16 + (double) mantissa) * exponent_pow;
		datarate = (double) fdig * (nominator / pow_33);
	}

	return (uint32_t) round(datarate);
}

uint8_t S2LP_RF_CalculateDataRateExponent(S2LP_Handle* handle, uint32_t datarate) {
	for (uint8_t exponent = S2LP_DATARATE_EXPONENT_MIN; exponent <= S2LP_DATARATE_EXPONENT_MAX; exponent++) {
		if (S2LP_RF_CalculateDataRateValue(handle, S2LP_DATARATE_MANTISSA_MAX, exponent) > datarate) {
			return exponent;
		}
	}

	return S2LP_DATARATE_EXPONENT_INVALID;
}

uint16_t S2LP_RF_CalculateDataRateMantissa(S2LP_Handle* handle, uint32_t datarate, uint8_t exponent) {
	double mantissa = 0;
	double fdig = S2LP_GetDigitalClockFrequency(handle);

	if (exponent == S2LP_DATARATE_EXPONENT_MIN) {
		double const pow_32 = (1ull << 32ull);
		mantissa = (pow_32 * (double) datarate) / fdig;
	} else if (exponent == S2LP_DATARATE_EXPONENT_MAX) {
		mantissa = fdig / (8.0 * datarate);
	} else {
		double const pow_33 = (1ull << 33ull);
		double const pow_16 = (1ull << 16ull);
		double const exponent_pow = (1 << exponent);
		double const nominator = pow_33 * datarate;
		double const denominator = fdig * exponent_pow;
		mantissa = (nominator / denominator) - pow_16;
	}

	return (uint16_t) round(mantissa);
}

void S2LP_RF_CalculateDataRateCoeffs(S2LP_Handle* handle, uint32_t datarate, uint16_t* mantissa, uint8_t* exponent) {
	*exponent = S2LP_RF_CalculateDataRateExponent(handle, datarate);
	*mantissa = S2LP_RF_CalculateDataRateMantissa(handle, datarate, *exponent);
}

double S2LP_RF_CalculateBaseFrequency(S2LP_Handle* handle, uint32_t synth_value) {
	double const pll_div = (S2LP_RF_GetSynthBand(handle) == S2LP_SYNTH_BAND_HIGH ? 4.0 : 8.0);
	double const ref_div = (S2LP_IsRefDivEnabled(handle) ? 2.0 : 1.0);
	double const fxo = S2LP_GetClockFrequency(handle);

	double const synth_divided = synth_value / ((double) (1ull << 20ull));
	double const fxo_denominator = (pll_div / 2.0) * ref_div;
	return round((fxo / fxo_denominator) * synth_divided);
}

double S2LP_RF_CalculateCenterFrequency(S2LP_Handle* handle, double base_frequency, uint8_t channel_spacing,
		uint8_t channel_number) {
	double const fxo = S2LP_GetClockFrequency(handle);
	double const fxo_divided = fxo / (double) (1ull << 15ull);
	return round(base_frequency + (fxo_divided * (double) channel_spacing) * (double) channel_number);
}

double S2LP_RF_CalculateBaseFreqResolution(S2LP_Handle* handle) {
	double const first_freq = S2LP_RF_CalculateBaseFrequency(handle, 100000);
	double const second_freq = S2LP_RF_CalculateBaseFrequency(handle, 100001);
	return second_freq - first_freq;
}

double S2LP_RF_CalculateChannelResolution(S2LP_Handle* handle, double base_frequency) {
	double const first_freq = S2LP_RF_CalculateCenterFrequency(handle, base_frequency, 100, 100);
	double const second_freq = S2LP_RF_CalculateCenterFrequency(handle, base_frequency, 100, 101);
// I really have no idea why does it have to be divided by 100,
// but the value corresponds to value in datasheet so i guess it's OK
	return (second_freq - first_freq) / 100.;
}

double S2LP_RF_CalculateFrequencyDeviation(S2LP_Handle* handle, uint8_t mantissa, uint8_t exponent) {
	double const fxo = S2LP_GetClockFrequency(handle);
	double const fxo_divided = fxo / (double) (1ull << 19ull);
	double const pll_div = (S2LP_RF_GetSynthBand(handle) == S2LP_SYNTH_BAND_HIGH ? 4.0 : 8.0);
	double const ref_div = (S2LP_IsRefDivEnabled(handle) ? 2.0 : 1.0);
	double const denominator = pll_div * ref_div;

	if (exponent == 0) {
		double const nominator = round(ref_div * (double) mantissa * (pll_div / 8.0));
		return fxo_divided * (nominator / denominator);
	} else {
		double const d_fdev = ref_div * (256.0 + (double) mantissa);
		double const exponent_pow = (1ull << (exponent - 1));
		double const nominator = round(d_fdev * exponent_pow * (pll_div / 8.0));
		return fxo_divided * (nominator / denominator);
	}
}

uint8_t S2LP_RF_CalculateFreqDevExponent(S2LP_Handle* handle, uint32_t deviation) {
	for (uint8_t exponent = S2LP_FDEV_EXPONENT_MIN; exponent <= S2LP_FDEV_EXPONENT_MAX; exponent++) {
		if (S2LP_RF_CalculateFrequencyDeviation(handle, S2LP_FDEV_MANTISSA_MAX, exponent) > deviation) {
			return exponent;
		}
	}

	return S2LP_FDEV_EXPONENT_INVALID;
}

uint8_t S2LP_RF_CalculateFreqDevMantissa(S2LP_Handle* handle, uint8_t exponent, uint32_t deviation) {
	uint32_t const frequency_max = S2LP_RF_CalculateFrequencyDeviation(handle,
	S2LP_FDEV_MANTISSA_MAX, exponent);
	uint32_t const frequency_min = (exponent > 0 ? S2LP_RF_CalculateFrequencyDeviation(handle,
	S2LP_FDEV_MANTISSA_MAX, exponent - 1) :
													0);

	uint32_t const frequency_range = frequency_max - frequency_min;
	uint32_t const corrected_frequency_start = deviation - frequency_min;
	double const frequency_position = (double) (corrected_frequency_start) / (double) (frequency_range);
	double approx_mantissa = (double) (S2LP_FDEV_MANTISSA_MIN) * (1.0 - frequency_position)
			+ ((double) (S2LP_FDEV_MANTISSA_MAX) * frequency_position);

	return (uint8_t) round(approx_mantissa);
}

S2LP_ChargePumpCurrent S2LP_RF_GetChargePumpCurrent(S2LP_Handle* handle) {
	uint8_t synt3_val = S2LP_ReadRegister(handle, S2LP_REG_SYNT3);
	uint8_t synth_config_val = S2LP_ReadRegister(handle, S2LP_REG_SYNTH_CONFIG2);

	uint8_t isel = GETBITS(synt3_val, 0b111, 5);
	bool split_en = GETBIT(synth_config_val, 2);

	if (isel == 0b010 && !split_en) {
		return S2LP_CHARGE_PUMP_120UA;
	} else if (isel == 0b001 && split_en) {
		return S2LP_CHARGE_PUMP_200UA;
	} else if (isel == 0b011 && !split_en) {
		return S2LP_CHARGE_PUMP_140UA;
	} else if (isel == 0b010 && split_en) {
		return S2LP_CHARGE_PUMP_240UA;
	}

	return S2LP_CHARGE_PUMP_INVALID;
}

S2LP_SynthesizerBand S2LP_RF_GetSynthBand(S2LP_Handle* handle) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_SYNT3);
	return (S2LP_SynthesizerBand) GETBIT(reg_val, 4);
}

uint32_t S2LP_RF_GetSynthValue(S2LP_Handle* handle) {
	uint8_t reg_vals[4] = { 0 };
	S2LP_BatchReadRegisters(handle, S2LP_REG_SYNT3, reg_vals, 4);

	uint32_t synth = 0;
	SETBITS(synth, reg_vals[3], 0xFF, 0);
	SETBITS(synth, reg_vals[2], 0xFF, 8);
	SETBITS(synth, reg_vals[1], 0xFF, 16);
	SETBITS(synth, GETBITS(reg_vals[0], 0xF, 0), 0xF, 24);

	return synth;
}

uint8_t S2LP_RF_GetChannelSpacing(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_CHSPACE);
}

uint8_t S2LP_RF_GetChannelNumber(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_CHNUM);
}

S2LP_Modulation S2LP_RF_GetModulationType(S2LP_Handle* handle) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_MOD2);
	return (S2LP_Modulation) GETBITS(reg_val, 0xF, 4);
}

uint32_t S2LP_RF_GetDataRate(S2LP_Handle* handle) {
	uint16_t mantissa = S2LP_RF_GetDataRateMantissa(handle);
	uint8_t exponent = S2LP_RF_GetDataRateExponent(handle);
	return S2LP_RF_CalculateDataRateValue(handle, mantissa, exponent);
}

uint16_t S2LP_RF_GetDataRateMantissa(S2LP_Handle* handle) {
	uint8_t mod_val[2] = { 0 };
	S2LP_BatchReadRegisters(handle, S2LP_REG_MOD4, mod_val, 2);

	uint16_t mantissa = 0;
	SETBITS(mantissa, mod_val[1], 0xFF, 0);
	SETBITS(mantissa, mod_val[0], 0xFF, 8);
	return mantissa;
}

uint8_t S2LP_RF_GetDataRateExponent(S2LP_Handle* handle) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_MOD2);
	return GETBITS(reg_val, 0xF, 0);
}

uint8_t S2LP_RF_GetFreqDevMantissa(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_MOD0);
}

uint8_t S2LP_RF_GetFreqDevExponent(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_MOD1);
	return GETBITS(reg_val, 0b1111, 0);
}

double S2LP_RF_GetFrequencyDeviation(S2LP_Handle* handle) {
	uint8_t const mantissa = S2LP_RF_GetFreqDevMantissa(handle);
	uint8_t const exponent = S2LP_RF_GetFreqDevExponent(handle);
	return S2LP_RF_CalculateFrequencyDeviation(handle, mantissa, exponent);
}

uint8_t S2LP_RF_GetConstellationMapping(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_MOD1);
	return GETBITS(reg_val, 0b11, 4);
}

bool S2LP_RF_GetFrequencyInterpolation(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_MOD1);
	return GETBIT(reg_val, 6);
}
