/*
 * s2lp.c
 *
 *  Created on: 28 cze 2021
 *      Author: SteelPh0enixLocal
 */

#include "s2lp.h"
#include "bit_helpers.h"
#include <string.h>

void S2LP_Initialize(S2LP_Handle* handle, S2LP_ClockFrequency frequency) {
	S2LP_InitHandle(handle);

	handle->frequency = frequency;

	S2LP_Reset(handle);

	// Set ref div according to fXO frequency
	S2LP_SetRefDivState(handle, S2LP_IsDigitalClockDivided(handle));
}

S2LP_Status S2LP_ParseStatus(uint8_t state_0, uint8_t state_1) {
	S2LP_Status status;

	// Parse status bits from handle
	status.xo_on = GETBIT(state_0, 0);
	status.state = (S2LP_State) GETBITS(state_0, 0b1111111, 1);
	status.rco_calibrator_error = GETBIT(state_1, 0);
	status.rx_fifo_empty = GETBIT(state_1, 1);
	status.tx_fifo_full = GETBIT(state_1, 2);
	status.ant_sel = GETBIT(state_1, 3);
	status.rco_cal_ok = GETBIT(state_1, 4);

	return status;
}

S2LP_Status S2LP_GetStatus(S2LP_Handle* handle) {
	return S2LP_ParseStatus(handle->status[0], handle->status[1]);
}

S2LP_Status S2LP_ReadStatus(S2LP_Handle* handle) {
	uint8_t states[2] = { 0 };
	S2LP_BatchReadRegisters(handle, S2LP_REG_MC_STATE1, states, 2);
	return S2LP_ParseStatus(states[1], states[0]);
}

uint32_t S2LP_GetInterruptsEx(S2LP_Handle* handle, bool clearFlags) {
	uint32_t irqs = 0;
	uint8_t reg_vals[4] = { 0 };

	S2LP_BatchReadRegisters(handle, S2LP_REG_IRQ_STATUS3, reg_vals, 4);
	SETBITS(irqs, reg_vals[0], 0xFF, 24);
	SETBITS(irqs, reg_vals[1], 0xFF, 16);
	SETBITS(irqs, reg_vals[2], 0xFF, 8);
	SETBITS(irqs, reg_vals[3], 0xFF, 0);

	// Clear up the register after read
	if (clearFlags) {
		memset((void*) reg_vals, 0, 4);
		S2LP_BatchWriteRegisters(handle, S2LP_REG_IRQ_STATUS3, reg_vals, 4);
	}

	return irqs;
}

uint32_t S2LP_GetInterrupts(S2LP_Handle* handle) {
	return S2LP_GetInterruptsEx(handle, true);
}

void S2LP_SetInterruptMasks(S2LP_Handle* handle, uint32_t mask) {
	uint8_t reg_vals[4] = { 0 };

	reg_vals[0] = GETBITS(mask, 0xFF, 24);
	reg_vals[1] = GETBITS(mask, 0xFF, 16);
	reg_vals[2] = GETBITS(mask, 0xFF, 8);
	reg_vals[3] = GETBITS(mask, 0xFF, 0);

	S2LP_BatchWriteRegisters(handle, S2LP_REG_IRQ_MASK3, reg_vals, 4);
}

uint32_t S2LP_GetInterruptMasks(S2LP_Handle* handle) {
	uint32_t masks = 0;
	uint8_t reg_vals[4] = { 0 };

	S2LP_BatchReadRegisters(handle, S2LP_REG_IRQ_MASK3, reg_vals, 4);
	SETBITS(masks, reg_vals[0], 0xFF, 24);
	SETBITS(masks, reg_vals[1], 0xFF, 16);
	SETBITS(masks, reg_vals[2], 0xFF, 8);
	SETBITS(masks, reg_vals[3], 0xFF, 0);

	return masks;
}

void S2LP_SetFIFOInterruptSource(S2LP_Handle* handle, S2LP_FIFOSelect fifo) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PROTOCOL2);

	switch (fifo) {
		case S2LP_FIFO_TX:
			SETBIT(reg_val, 2);
			break;
		case S2LP_FIFO_RX:
			CLEARBIT(reg_val, 2);
			break;
	}

	S2LP_WriteRegister(handle, S2LP_REG_PROTOCOL2, reg_val);
}

S2LP_FIFOSelect S2LP_GetFIFOInterruptSource(S2LP_Handle* handle) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PROTOCOL2);

	return (S2LP_FIFOSelect) GETBIT(reg_val, 2);
}

void S2LP_ReadFIFO(S2LP_Handle* handle, size_t length, uint8_t* buffer) {
	if (length > 128) {
		return;
	}

	S2LP_BatchReadRegisters(handle, S2LP_ADDR_FIFO, buffer, length);
}

void S2LP_WriteFIFO(S2LP_Handle* handle, size_t length, uint8_t* buffer) {
	if (length > 128) {
		return;
	}

	S2LP_BatchWriteRegisters(handle, S2LP_ADDR_FIFO, buffer, length);
}

uint8_t S2LP_GetDevicePartNumber(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_DEVICE_INFO1);
}

uint8_t S2LP_GetDeviceVersionNumber(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_DEVICE_INFO0);
}

uint32_t S2LP_GetClockFrequency(S2LP_Handle* handle) {
	switch (handle->frequency) {
		case S2LP_CLOCK_FREQ_24MHZ:
			return 24000000;
		case S2LP_CLOCK_FREQ_25MHZ:
			return 25000000;
		case S2LP_CLOCK_FREQ_26MHZ:
			return 26000000;
		case S2LP_CLOCK_FREQ_48MHZ:
			return 48000000;
		case S2LP_CLOCK_FREQ_50MHZ:
			return 50000000;
		case S2LP_CLOCK_FREQ_52MHZ:
			return 52000000;
		case S2LP_CLOCK_FREQ_INVALID:
		default:
			return 0;
	}
}

bool S2LP_IsDigitalClockDivided(S2LP_Handle* handle) {
	return (handle->frequency == S2LP_CLOCK_FREQ_48MHZ || handle->frequency == S2LP_CLOCK_FREQ_50MHZ
			|| handle->frequency == S2LP_CLOCK_FREQ_52MHZ);
}

uint32_t S2LP_GetDigitalClockFrequency(S2LP_Handle* handle) {
	uint32_t const base_clock = S2LP_GetClockFrequency(handle);

	if (S2LP_IsDigitalClockDivided(handle)) {
		return base_clock / 2;
	}

	return base_clock;
}

void S2LP_SetRefDivState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_XO_RCO_CONF0);
	if (enabled) {
		SETBIT(reg_val, 3);
	} else {
		CLEARBIT(reg_val, 3);
	}

	S2LP_WriteRegister(handle, S2LP_REG_XO_RCO_CONF0, reg_val);
}

bool S2LP_IsRefDivEnabled(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_XO_RCO_CONF0);
	return GETBIT(reg_val, 3);
}

bool S2LP_CallibrateRCO(S2LP_Handle* handle) {
	// Start callibration
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_XO_RCO_CONF0);

	SETBIT(reg_val, 0);

	S2LP_WriteRegister(handle, S2LP_REG_XO_RCO_CONF0, reg_val);

	// Wait until the callibration is complete

	S2LP_Status status = S2LP_ReadStatus(handle);
	int tries = 0;
	while (!status.rco_cal_ok && tries < S2LP_RCO_CALLIB_TRIES) {
		S2LP_Delay(S2LP_RCO_CALLIB_WAIT_TIME);
		status = S2LP_ReadStatus(handle);
		tries++;
	}

	return !(tries == S2LP_RCO_CALLIB_TRIES && status.rco_calibrator_error);
}
