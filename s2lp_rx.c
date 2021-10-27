/*
 * s2lp_rx.c
 *
 *  Created on: 30 sie 2021
 *      Author: steelph0enix
 */

#include "s2lp.h"
#include "s2lp_rx.h"
#include "bit_helpers.h"

// Channel filter words table

#define S2LP_CHANNEL_FILTER_WORDS_M 9
#define S2LP_CHANNEL_FILTER_WORDS_E 10
#define S2LP_CHANNEL_FILTER_WORDS_LENGTH (S2LP_CHANNEL_FILTER_WORDS_M * S2LP_CHANNEL_FILTER_WORDS_E)
#define S2LP_CHANNEL_FILTER_WORDS_SIZE (sizeof(double) * S2LP_CHANNEL_FILTER_WORDS_LENGTH)
// @formatter:off
double const S2LP_CHANNEL_FILTER_WORDS[S2LP_CHANNEL_FILTER_WORDS_M][S2LP_CHANNEL_FILTER_WORDS_E] =
        {
        /* E =            0      1      2      3     4     5     6    7    8    9 */
        /* M = 0 */ { 800.1, 450.9, 224.7, 112.3, 56.1, 28.0, 14.0, 7.0, 3.5, 1.8},
        /* M = 1 */ { 795.1, 425.9, 212.4, 106.2, 53.0, 26.5, 13.3, 6.6, 3.3, 1.7},
        /* M = 2 */ { 768.4, 403.2, 201.1, 100.5, 50.2, 25.1, 12.6, 6.3, 3.1, 1.6},
        /* M = 3 */ { 736.8, 380.8, 190.0,  95.0, 47.4, 23.7, 11.9, 5.9, 3.0, 1.5},
        /* M = 4 */ { 705.1, 362.1, 180.7,  90.3, 45.1, 22.6, 11.3, 5.6, 2.8, 1.4},
        /* M = 5 */ { 670.9, 341.7, 170.6,  85.3, 42.6, 21.3, 10.6, 5.3, 2.7, 1.3},
        /* M = 6 */ { 642.3, 325.4, 162.4,  81.2, 40.6, 20.3, 10.1, 5.1, 2.5, 1.3},
        /* M = 7 */ { 586.7, 294.5, 147.1,  73.5, 36.7, 18.4,  9.2, 4.6, 2.3, 1.2},
        /* M = 8 */ { 541.4, 270.3, 135.0,  67.5, 33.7, 16.9,  8.4, 4.2, 2.1, 1.1},
        };
// @formatter:on

void S2LP_RX_SetRSSIThreshold(S2LP_Handle* handle, uint8_t rssi) {
	S2LP_WriteRegister(handle, S2LP_REG_RSSI_TH, rssi);
}

void S2LP_RX_SetAFCFastLoopGain(S2LP_Handle* handle, uint8_t gain) {
	if (gain > 15) {
		return;
	}

	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AFC0);
	CLEARBITS(reg_val, 0xF, 4);
	SETBITS(reg_val, gain, 0xF, 4);
	S2LP_WriteRegister(handle, S2LP_REG_AFC0, reg_val);
}

void S2LP_RX_SetAFCSlowLoopGain(S2LP_Handle* handle, uint8_t gain) {
	if (gain > 15) {
		return;
	}

	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AFC0);
	CLEARBITS(reg_val, 0xF, 0);
	SETBITS(reg_val, gain, 0xF, 0);
	S2LP_WriteRegister(handle, S2LP_REG_AFC0, reg_val);
}

void S2LP_RX_SetAFCFastPeriod(S2LP_Handle* handle, uint8_t period) {
	S2LP_WriteRegister(handle, S2LP_REG_AFC1, period);
}

void S2LP_RX_SetAFCMode(S2LP_Handle* handle, S2LP_AFC_Mode mode) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AFC2);
	switch (mode) {
		case S2LP_AFC_SLICER_CORRECTION:
			CLEARBIT(reg_val, 5);
			break;
		case S2LP_AFC_2ND_IF_CORRECTION:
			SETBIT(reg_val, 5);
			break;
	}

	S2LP_WriteRegister(handle, S2LP_REG_AFC2, reg_val);
}

void S2LP_RX_SetAFCState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AFC2);
	if (enabled) {
		SETBIT(reg_val, 6);
	} else {
		CLEARBIT(reg_val, 6);
	}
	S2LP_WriteRegister(handle, S2LP_REG_AFC2, reg_val);
}

void S2LP_RX_SetAFCFreezeOnSyncState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AFC2);
	if (enabled) {
		SETBIT(reg_val, 7);
	} else {
		CLEARBIT(reg_val, 7);
	}
	S2LP_WriteRegister(handle, S2LP_REG_AFC2, reg_val);
}

void S2LP_RX_SetAGCHighThreshold(S2LP_Handle* handle, uint8_t value) {
	if (value > 15) {
		return;
	}

	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL1);
	CLEARBITS(reg_val, 0xF, 4);
	SETBITS(reg_val, value, 0xF, 4);
	S2LP_WriteRegister(handle, S2LP_REG_AGCCTRL1, reg_val);
}

void S2LP_RX_SetAGCLowThreshold(S2LP_Handle* handle, S2LP_AGC_Low_Threshold threshold, uint8_t value) {
	if (value > 15) {
		return;
	}

	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL4);
	switch (threshold) {
		case S2LP_AGC_LOW_THRESHOLD_0:
			CLEARBITS(reg_val, 0xF, 4);
			SETBITS(reg_val, value, 0xF, 4);
			break;
		case S2LP_AGC_LOW_THRESHOLD_1:
			CLEARBITS(reg_val, 0xF, 0);
			SETBITS(reg_val, value, 0xF, 0);
			break;
	}

	S2LP_WriteRegister(handle, S2LP_REG_AGCCTRL4, reg_val);
}

void S2LP_RX_SetAGCMeasureTimeRaw(S2LP_Handle* handle, uint8_t time) {
	if (time > 15) {
		return;
	}

	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL2);
	CLEARBITS(reg_val, 0xF, 0);
	SETBITS(reg_val, time, 0xF, 0);
	S2LP_WriteRegister(handle, S2LP_REG_AGCCTRL2, reg_val);
}

void S2LP_RX_SetAGCHoldTimeRaw(S2LP_Handle* handle, uint8_t time) {
	if (time > 31) {
		return;
	}

	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL0);
	CLEARBITS(reg_val, 0b11111, 0);
	SETBITS(reg_val, time, 0b11111, 0);
	S2LP_WriteRegister(handle, S2LP_REG_AGCCTRL0, reg_val);
}

void S2LP_RX_SetAGCState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL0);
	if (enabled) {
		SETBIT(reg_val, 7);
	} else {
		CLEARBIT(reg_val, 7);
	}

	S2LP_WriteRegister(handle, S2LP_REG_AGCCTRL0, reg_val);
}

void S2LP_RX_SetAGCFreezeOnSyncState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL2);
	if (enabled) {
		SETBIT(reg_val, 5);
	} else {
		CLEARBIT(reg_val, 5);
	}

	S2LP_WriteRegister(handle, S2LP_REG_AGCCTRL2, reg_val);
}

void S2LP_RX_SetChannelFilterValueRaw(S2LP_Handle* handle, uint8_t mantissa, uint8_t exponent) {
	uint8_t reg_val = 0;
	SETBITS(reg_val, mantissa, 0xF, 4);
	SETBITS(reg_val, exponent, 0xF, 0);
	S2LP_WriteRegister(handle, S2LP_REG_CHFLT, reg_val);
}

void S2LP_RX_SetCarrierSenseMode(S2LP_Handle* handle, S2LP_CS_Mode mode) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_RSSI_FLT);
	CLEARBITS(reg_val, 0b11, 2);
	SETBITS(reg_val, (uint8_t )mode, 0b11, 2);
	S2LP_WriteRegister(handle, S2LP_REG_RSSI_FLT, reg_val);
}

void S2LP_RX_SetTimerStopConfig(S2LP_Handle* handle, bool rx_timeout_and_or,
bool cs_timeout, bool sqi_timeout, bool pqi_timeout) {
	uint8_t proto_reg_val = S2LP_ReadRegister(handle, S2LP_REG_PROTOCOL2);
	uint8_t rxt_reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKT_FLT_OPTIONS);

	if (rx_timeout_and_or) {
		SETBIT(rxt_reg_val, 6);
	} else {
		CLEARBIT(rxt_reg_val, 6);
	}

	if (cs_timeout) {
		SETBIT(proto_reg_val, 7);
	} else {
		CLEARBIT(proto_reg_val, 7);
	}

	if (sqi_timeout) {
		SETBIT(proto_reg_val, 6);
	} else {
		CLEARBIT(proto_reg_val, 6);
	}

	if (pqi_timeout) {
		SETBIT(proto_reg_val, 5);
	} else {
		CLEARBIT(proto_reg_val, 5);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PROTOCOL2, proto_reg_val);
	S2LP_WriteRegister(handle, S2LP_REG_PCKT_FLT_OPTIONS, rxt_reg_val);
}

void S2LP_RX_SetCSBlankingState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_ANT_SELECT_CONF);

	if (enabled) {
		SETBIT(reg_val, 4);
	} else {
		CLEARBIT(reg_val, 4);
	}

	S2LP_WriteRegister(handle, S2LP_REG_ANT_SELECT_CONF, reg_val);
}

void S2LP_RX_SetDataSource(S2LP_Handle* handle, S2LP_RX_Source source) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL3);

	CLEARBITS(reg_val, 0b11, 4);
	SETBITS(reg_val, (uint8_t )source, 0b11, 4);

	S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL3, reg_val);
}

void S2LP_RX_SetFIFOAlmostFullThreshold(S2LP_Handle* handle, uint8_t threshold) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_FIFO_CONFIG3);

	CLEARBITS(reg_val, 0b1111111, 0);
	SETBITS(reg_val, threshold, 0b1111111, 0);

	S2LP_WriteRegister(handle, S2LP_REG_FIFO_CONFIG3, reg_val);
}

void S2LP_RX_SetFIFOAlmostEmptyThreshold(S2LP_Handle* handle, uint8_t threshold) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_FIFO_CONFIG2);

	CLEARBITS(reg_val, 0b1111111, 0);
	SETBITS(reg_val, threshold, 0b1111111, 0);

	S2LP_WriteRegister(handle, S2LP_REG_FIFO_CONFIG2, reg_val);
}

double S2LP_RX_CalculateAGCMeasureTime(S2LP_Handle* handle, uint8_t time) {
	double const fdig = S2LP_GetDigitalClockFrequency(handle);
	double const fdig_divider = 12.0 / fdig;
	double const meas_time_pow = (double) (1ull << time);
	return fdig_divider * meas_time_pow;
}

double S2LP_RX_CalculateAGCHoldTime(S2LP_Handle* handle, uint8_t time) {
	double const fdig = S2LP_GetDigitalClockFrequency(handle);
	double const fdig_divider = 12.0 / fdig;
	return fdig_divider * ((double) time);
}

double S2LP_RX_CalculateChannelFilterBandwidth(S2LP_Handle* handle, uint8_t mantissa, uint8_t exponent) {
	double const fdig = S2LP_GetDigitalClockFrequency(handle);
	return S2LP_CHANNEL_FILTER_WORDS[mantissa][exponent] * (fdig / 26000000.0);
}

uint8_t S2LP_RX_GetRSSIThreshold(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_RSSI_TH);
}

uint8_t S2LP_RX_GetAFCFastLoopGain(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AFC0);
	return GETBITS(reg_val, 0xF, 4);
}

uint8_t S2LP_RX_GetAFCSlowLoopGain(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AFC0);
	return GETBITS(reg_val, 0xF, 0);
}

uint8_t S2LP_RX_GetAFCFastPeriod(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_AFC1);
}

S2LP_AFC_Mode S2LP_RX_GetAFCMode(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AFC2);
	return (S2LP_AFC_Mode) GETBIT(reg_val, 5);
}

bool S2LP_RX_GetAFCState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AFC2);
	return (bool) GETBIT(reg_val, 6);
}

bool S2LP_RX_GetAFCFreezeOnSyncState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AFC2);
	return (bool) GETBIT(reg_val, 7);
}

uint8_t S2LP_RX_GetAGCHighThreshold(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL1);
	return (uint8_t) GETBITS(reg_val, 0xF, 4);
}

uint8_t S2LP_RX_GetAGCLowThreshold(S2LP_Handle* handle, S2LP_AGC_Low_Threshold threshold) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL4);

	switch (threshold) {
		case S2LP_AGC_LOW_THRESHOLD_0:
			return (uint8_t) GETBITS(reg_val, 0xF, 4);
		case S2LP_AGC_LOW_THRESHOLD_1:
			return (uint8_t) GETBITS(reg_val, 0xF, 0);
	}

	return 0;
}

uint8_t S2LP_RX_GetAGCMeasureTimeRaw(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL2);
	return (uint8_t) GETBITS(reg_val, 0xF, 0);
}

uint8_t S2LP_RX_GetAGCHoldTimeRaw(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL0);
	return (uint8_t) GETBITS(reg_val, 0b11111, 0);
}

bool S2LP_RX_GetAGCState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL0);
	return (bool) GETBIT(reg_val, 7);
}

bool S2LP_RX_GetAGCFreezeOnSyncState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_AGCCTRL2);
	return (bool) GETBIT(reg_val, 5);
}

uint8_t S2LP_RX_GetChannelFilterMantissa(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_CHFLT);
	return (uint8_t) GETBITS(reg_val, 0xF, 4);
}

uint8_t S2LP_RX_GetChannelFilterExponent(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_CHFLT);
	return (uint8_t) GETBITS(reg_val, 0xF, 0);
}

void S2LP_RX_GetChannelFilterValueRaw(S2LP_Handle* handle, uint8_t* mantissa, uint8_t* exponent) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_CHFLT);
	*mantissa = GETBITS(reg_val, 0xF, 4);
	*exponent = GETBITS(reg_val, 0xF, 0);
}

double S2LP_RX_GetChannelFilterValue(S2LP_Handle* handle) {
	uint8_t mantissa = 0;
	uint8_t exponent = 0;
	S2LP_RX_GetChannelFilterValueRaw(handle, &mantissa, &exponent);
	return S2LP_RX_CalculateChannelFilterBandwidth(handle, mantissa, exponent);
}

S2LP_CS_Mode S2LP_RX_GetCarrierSenseMode(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_RSSI_FLT);
	return (S2LP_CS_Mode) GETBITS(reg_val, 0b11, 2);
}

void S2LP_RX_GetTimerStopConfig(S2LP_Handle* handle, bool* rx_timeout_and_or,
bool* cs_timeout, bool* sqi_timeout, bool* pqi_timeout) {
	uint8_t const proto_reg_val = S2LP_ReadRegister(handle, S2LP_REG_PROTOCOL2);
	uint8_t const rxt_reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKT_FLT_OPTIONS);

	*rx_timeout_and_or = GETBIT(rxt_reg_val, 6);
	*cs_timeout = GETBIT(proto_reg_val, 7);
	*sqi_timeout = GETBIT(proto_reg_val, 6);
	*pqi_timeout = GETBIT(proto_reg_val, 5);
}

S2LP_RX_Source S2LP_RX_GetDataSource(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL3);
	return (S2LP_RX_Source) GETBITS(reg_val, 0b11, 4);
}

uint8_t S2LP_RX_GetCurrentRSSI(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_RSSI_LEVEL_RUN);
}

uint8_t S2LP_RX_GetCapturedRSSI(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_RSSI_LEVEL);
}

bool S2LP_RX_GetCSBlankingState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_ANT_SELECT_CONF);
	return (bool) GETBIT(reg_val, 4);
}

bool S2LP_RX_GetCarrierSenseIndicator(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_LINK_QUALIF1);
	return (bool) GETBIT(reg_val, 7);
}

uint8_t S2LP_RX_GetLastPacketSQI(S2LP_Handle* handle,
bool* is_for_secondary_sync) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_LINK_QUALIF1);

	if (is_for_secondary_sync != NULL) {
		*is_for_secondary_sync = GETBIT(reg_val, 6);
	}

	return (uint8_t) GETBITS(reg_val, 0b11111, 0);
}

uint8_t S2LP_RX_GetLastPacketPQI(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_LINK_QUALIF2);
}

uint8_t S2LP_RX_GetPQIThreshold(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_QI);
	return (uint8_t) GETBITS(reg_val, 0b1111, 1);
}

bool S2LP_RX_GetSQICheckStatus(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_QI);
	return (bool) GETBIT(reg_val, 0);
}

uint8_t S2LP_RX_GetSQIThreshold(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_QI);
	return (uint8_t) GETBITS(reg_val, 0b111, 5);
}

uint8_t S2LP_RX_GetFIFOAlmostFullThreshold(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_FIFO_CONFIG3);
	return (uint8_t) GETBITS(reg_val, 0b1111111, 0);
}

uint8_t S2LP_RX_GetFIFOAlmostEmptyThreshold(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_FIFO_CONFIG2);
	return (uint8_t) GETBITS(reg_val, 0b1111111, 0);
}

uint8_t S2LP_RX_GetFIFOCount(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_RX_FIFO_STATUS);
}

bool S2LP_RX_GetLastPacketNACK(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_RX_PCKT_INFO);
	return GETBIT(reg_val, 2);
}

uint8_t S2LP_RX_GetSequenceNumber(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_RX_PCKT_INFO);
	return (uint8_t) GETBITS(reg_val, 0b11, 0);
}

