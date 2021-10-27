/*
 * s2lp_power.c
 *
 *  Created on: 11 sie 2021
 *      Author: steelph0enix
 */

#include "s2lp_power.h"
#include "bit_helpers.h"

void S2LP_Power_SetSMPSVoltage(S2LP_Handle* handle, S2LP_SMPS_Voltage voltage) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF0);
	CLEARBITS(reg_val, 0b111, 4);
	SETBITS(reg_val, (uint8_t ) (voltage), 0b111, 4);
	S2LP_WriteRegister(handle, S2LP_REG_PM_CONF0, reg_val);
}

void S2LP_Power_SetKRMState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF3);

	if (enabled) {
		SETBIT(reg_val, 7);
	} else {
		CLEARBIT(reg_val, 7);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PM_CONF3, reg_val);
}

void S2LP_Power_SetKRMRatio(S2LP_Handle* handle, uint16_t ratio) {
	uint8_t conf_vals[2] = { 0 };
	conf_vals[0] = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF3);
	// CONF2 is for the KRM ratio LSB so we just fill it up with proper bits
	conf_vals[1] = GETBITS(ratio, 0xFF, 0);

	// Put MSB in CONF3
	CLEARBITS(conf_vals[0], 0b1111111, 0);
	SETBITS(conf_vals[0], GETBITS(ratio, 0b1111111, 8), 0b1111111, 0);

	S2LP_BatchWriteRegisters(handle, S2LP_REG_PM_CONF3, conf_vals, 2);
}

void S2LP_Power_SetSleepMode(S2LP_Handle* handle, S2LP_Sleep_Mode mode) {
	uint8_t conf0_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF0);

	switch (mode) {
		case S2LP_SLEEP_A:
			CLEARBIT(conf0_val, 0);
			break;
		case S2LP_SLEEP_B:
			SETBIT(conf0_val, 0);
			break;
		default:
			break;
	}

	S2LP_WriteRegister(handle, S2LP_REG_PM_CONF0, conf0_val);
}

void S2LP_Power_SetLDOBypassState(S2LP_Handle* handle, bool enabled) {
	uint8_t conf1_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF1);

	if (enabled) {
		SETBIT(conf1_val, 2);
	} else {
		CLEARBIT(conf1_val, 2);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PM_CONF1, conf1_val);
}

void S2LP_Power_SetInternalSMPSState(S2LP_Handle* handle, bool enabled) {
	uint8_t conf4_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF4);

	if (enabled) {
		CLEARBIT(conf4_val, 5);
	} else {
		SETBIT(conf4_val, 5);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PM_CONF4, conf4_val);
}

void S2LP_Power_SetBatteryDetectionState(S2LP_Handle* handle, bool enabled) {
	uint8_t conf1_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF1);

	if (enabled) {
		SETBIT(conf1_val, 6);
	} else {
		CLEARBIT(conf1_val, 6);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PM_CONF1, conf1_val);
}

void S2LP_Power_SetBLDThreshold(S2LP_Handle* handle, S2LP_BLD_Threshold threshold) {
	uint8_t conf1_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF1);

	CLEARBITS(conf1_val, 0b11, 4);
	SETBITS(conf1_val, threshold, 0b11, 4);

	S2LP_WriteRegister(handle, S2LP_REG_PM_CONF1, conf1_val);
}

void S2LP_Power_SetSMPSLevelMode(S2LP_Handle* handle, S2LP_SMPS_Level_Mode mode) {
	uint8_t conf1_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF1);

	switch (mode) {
		case S2LP_SMPS_LEVEL_RX_TX:
			CLEARBIT(conf1_val, 3);
			break;
		case S2LP_SMPS_LEVEL_TX:
			SETBIT(conf1_val, 3);
			break;
		default:
			break;
	}

	S2LP_WriteRegister(handle, S2LP_REG_PM_CONF1, conf1_val);
}

void S2LP_Power_SetFIRState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PA_CONFIG1);

	if (enabled) {
		SETBIT(reg_val, 1);
	} else {
		CLEARBIT(reg_val, 1);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PA_CONFIG1, reg_val);
}

S2LP_SMPS_Voltage S2LP_Power_GetSMPSVoltage(S2LP_Handle* handle) {
	uint8_t const conf0_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF0);
	return (S2LP_SMPS_Voltage) GETBITS(conf0_val, 0b111, 4);
}

bool S2LP_Power_GetKRMState(S2LP_Handle* handle) {
	uint8_t const conf3_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF3);
	return GETBIT(conf3_val, 7);
}

uint16_t S2LP_Power_GetKRMRatio(S2LP_Handle* handle) {
	uint8_t conf_vals[2];
	S2LP_BatchReadRegisters(handle, S2LP_REG_PM_CONF3, conf_vals, 2);
	uint16_t ret_val = 0;

	// CONF2 contains 8 LSB, CONF3 - 6 MSB
	SETBITS(ret_val, GETBITS(conf_vals[0], 0b1111111, 0), 0b111111, 8);
	SETBITS(ret_val, conf_vals[1], 0xFF, 0);

	return ret_val;
}

S2LP_Sleep_Mode S2LP_Power_GetSleepMode(S2LP_Handle* handle) {
	uint8_t const conf0_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF0);
	return (S2LP_Sleep_Mode) GETBIT(conf0_val, 0);
}

bool S2LP_Power_GetLDOBypassState(S2LP_Handle* handle) {
	uint8_t const conf1_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF1);
	return GETBIT(conf1_val, 2);
}

bool S2LP_Power_GetSMPSState(S2LP_Handle* handle) {
	uint8_t const conf4_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF4);
	return GETBIT(conf4_val, 5);
}

bool S2LP_Power_GetBatteryDetectionState(S2LP_Handle* handle) {
	uint8_t const conf1_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF1);
	return GETBIT(conf1_val, 6);
}

S2LP_BLD_Threshold S2LP_Power_GetBLDThreshold(S2LP_Handle* handle) {
	uint8_t const conf1_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF1);
	return (S2LP_BLD_Threshold) GETBITS(conf1_val, 0b11, 4);
}

S2LP_SMPS_Level_Mode S2LP_Power_GetSMPSLevelMode(S2LP_Handle* handle) {
	uint8_t const conf1_val = S2LP_ReadRegister(handle, S2LP_REG_PM_CONF1);
	return (S2LP_SMPS_Level_Mode) GETBIT(conf1_val, 3);
}

bool S2LP_Power_GetFIRState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PA_CONFIG1);
	return GETBIT(reg_val, 1);
}
