/*
 * s2lp_packet.c
 *
 *  Created on: 21 wrz 2021
 *      Author: steelph0enix
 */

#include "bit_helpers.h"
#include "s2lp_packet.h"

void S2LP_PCKT_SetPacketFormat(S2LP_Handle* handle, S2LP_Packet_Format format) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL3);

	CLEARBITS(reg_val, 0b11, 6);
	SETBITS(reg_val, (uint8_t )format, 0b11, 6);

	S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL3, reg_val);
}

void S2LP_PCKT_SetPreambleType(S2LP_Handle* handle, S2LP_Preamble preamble) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL3);

	CLEARBITS(reg_val, 0b11, 0);
	SETBITS(reg_val, (uint8_t )preamble, 0b11, 0);

	S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL3, reg_val);
}

void S2LP_PCKT_SetPreambleLength(S2LP_Handle* handle, size_t length) {
	if (length > 1024) {
		return;
	}

	uint8_t regs_val[2] = { 0 };
	S2LP_BatchReadRegisters(handle, S2LP_REG_PCKTCTRL6, regs_val, 2);

	CLEARBITS(regs_val[0], 0b11, 0);
	CLEARBITS(regs_val[1], 0xFF, 0);
	SETBITS(regs_val[0], GETBITS(length, 0b11, 8), 0b11, 0);
	SETBITS(regs_val[1], GETBITS(length, 0xFF, 0), 0xFF, 0);

	S2LP_BatchWriteRegisters(handle, S2LP_REG_PCKTCTRL6, regs_val, 2);
}

void S2LP_PCKT_SetSyncLength(S2LP_Handle* handle, size_t length) {
	if (length > 32) {
		return;
	}

	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL6);
	CLEARBITS(reg_val, 0b111111, 2);
	SETBITS(reg_val, length, 0b111111, 2);
	S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL6, reg_val);
}

void S2LP_PCKT_SetPacketLength(S2LP_Handle* handle, size_t length) {
	if (length > 65535) {
		return;
	}

	uint8_t reg_vals[2] = { 0 };

	reg_vals[0] = GETBITS(length, 0xFF, 8); // MSB
	reg_vals[1] = GETBITS(length, 0xFF, 0); // LSB

	S2LP_BatchWriteRegisters(handle, S2LP_REG_PCKTLEN1, reg_vals, 2);
}

void S2LP_PCKT_SetVariablePacketLengthState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL2);

	if (enabled) {
		SETBIT(reg_val, 0);
	} else {
		CLEARBIT(reg_val, 0);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL2, reg_val);
}

void S2LP_PCKT_SetLengthFieldSize(S2LP_Handle* handle, S2LP_Length_Field_Size size) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL4);

	switch (size) {
		case S2LP_PAYLOAD_LENGTH_1B:
			CLEARBIT(reg_val, 7);
			break;
		case S2LP_PAYLOAD_LENGTH_2B:
			SETBIT(reg_val, 7);
			break;
	}

	S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL4, reg_val);
}

void S2LP_PCKT_SetDestinationAddressState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL4);

	if (enabled) {
		SETBIT(reg_val, 3);
	} else {
		CLEARBIT(reg_val, 3);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL4, reg_val);
}

void S2LP_PCKT_SetDestinationAddress(S2LP_Handle* handle, uint8_t address) {
	S2LP_WriteRegister(handle, S2LP_REG_PCKT_FLT_GOALS3, address);
}

void S2LP_PCKT_SetSourceAddress(S2LP_Handle* handle, uint8_t address) {
	S2LP_WriteRegister(handle, S2LP_REG_PCKT_FLT_GOALS0, address);
}

void S2LP_PCKT_SetPostambleLength(S2LP_Handle* handle, size_t length) {
	if (length > 255) {
		return;
	}

	S2LP_WriteRegister(handle, S2LP_REG_PCKT_PSTMBL, (uint8_t) length);
}

void S2LP_PCKT_SetCRCMode(S2LP_Handle* handle, S2LP_CRC_Mode mode) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL1);

	CLEARBITS(reg_val, 0b111, 5);
	SETBITS(reg_val, (uint8_t )mode, 0b111, 5);

	S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL1, reg_val);
}

void S2LP_PCKT_DisableDataCoding(S2LP_Handle* handle) {
	uint8_t reg_vals[2] = { 0 };
	S2LP_BatchReadRegisters(handle, S2LP_REG_PCKTCTRL2, reg_vals, 2);

	CLEARBIT(reg_vals[0], 2); // disable 3of6
	CLEARBIT(reg_vals[0], 1); // disable manchester
	CLEARBIT(reg_vals[1], 0); // disable fec

	S2LP_BatchWriteRegisters(handle, S2LP_REG_PCKTCTRL2, reg_vals, 2);
}

void S2LP_PCKT_SetDataCoding(S2LP_Handle* handle, S2LP_Data_Coding mode) {
	S2LP_PCKT_DisableDataCoding(handle);
	uint8_t reg_val = 0;

	switch (mode) {
		case S2LP_CODING_FEC: {
			reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL1);
			SETBIT(reg_val, 0);
			S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL1, reg_val);
			break;
		}
		case S2LP_CODING_MANCHESTER: {
			reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL2);
			SETBIT(reg_val, 1);
			S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL2, reg_val);
			break;
		}
		case S2LP_CODING_3_OUT_OF_6: {
			reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL2);
			SETBIT(reg_val, 2);
			S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL2, reg_val);
			break;
		}
		default: {
		}
	}
}

void S2LP_PCKT_SetDataWhiteningState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL1);

	if (enabled) {
		SETBIT(reg_val, 4);
	} else {
		CLEARBIT(reg_val, 4);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL1, reg_val);
}

void S2LP_PCKT_SetCRCFilteringState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKT_FLT_OPTIONS);

	if (enabled) {
		SETBIT(reg_val, 0);
	} else {
		CLEARBIT(reg_val, 0);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PCKT_FLT_OPTIONS, reg_val);
}

void S2LP_PCKT_SetAutoPacketFilteringState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PROTOCOL1);

	if (enabled) {
		SETBIT(reg_val, 0);
	} else {
		CLEARBIT(reg_val, 0);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PROTOCOL1, reg_val);
}

void S2LP_PCKT_SetDestinationAddressFilteringState(S2LP_Handle* handle, bool enabled) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKT_FLT_OPTIONS);

	if (enabled) {
		SETBIT(reg_val, 1);
	} else {
		CLEARBIT(reg_val, 1);
	}

	S2LP_WriteRegister(handle, S2LP_REG_PCKT_FLT_OPTIONS, reg_val);
}

S2LP_Packet_Format S2LP_PCKT_GetPacketFormat(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL3);
	return (S2LP_Packet_Format) GETBITS(reg_val, 0b11, 6);
}

S2LP_Preamble S2LP_PCKT_GetPreambleType(S2LP_Handle* handle) {
	uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL3);
	return (S2LP_Preamble) GETBITS(reg_val, 0b11, 0);
}

size_t S2LP_PCKT_GetPreambleLength(S2LP_Handle* handle) {
	uint8_t regs_val[2] = { 0 };
	S2LP_BatchReadRegisters(handle, S2LP_REG_PCKTCTRL6, regs_val, 2);

	size_t length = 0;
	SETBITS(length, GETBITS(regs_val[0], 0b11, 0), 0b11, 8);
	SETBITS(length, regs_val[1], 0xFF, 0);

	return length;
}

size_t S2LP_PCKT_GetSyncLength(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL6);
	return (size_t) GETBITS(reg_val, 0b111111, 2);
}

size_t S2LP_PCKT_GetTxPacketLength(S2LP_Handle* handle) {
	uint8_t reg_vals[2] = { 0 };
	S2LP_BatchReadRegisters(handle, S2LP_REG_PCKTLEN1, reg_vals, 2);
	return (size_t) reg_vals[1] + (((size_t) reg_vals[0]) << 8);
}

size_t S2LP_PCKT_GetRxPacketLength(S2LP_Handle* handle) {
	uint8_t reg_vals[2] = { 0 };
	S2LP_BatchReadRegisters(handle, S2LP_REG_RX_PCKT_LEN1, reg_vals, 2);

	return (size_t) reg_vals[1] + (((size_t) reg_vals[0]) << 8);
}

bool S2LP_PCKT_GetVariablePacketLengthState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL2);
	return (bool) GETBIT(reg_val, 0);
}

S2LP_Length_Field_Size S2LP_PCKT_GetLengthFieldSize(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL4);
	bool const len_state = GETBIT(reg_val, 7);

	if (len_state) {
		return S2LP_PAYLOAD_LENGTH_2B;
	}
	return S2LP_PAYLOAD_LENGTH_1B;
}

bool S2LP_PCKT_GetDestinationAddressState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL4);
	return (bool) GETBIT(reg_val, 3);
}

uint8_t S2LP_PCKT_GetDestinationAddress(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_PCKT_FLT_GOALS3);
}

uint8_t S2LP_PCKT_GetSourceAddress(S2LP_Handle* handle) {
	return S2LP_ReadRegister(handle, S2LP_REG_PCKT_FLT_GOALS0);
}

size_t S2LP_PCKT_GetPostambleLength(S2LP_Handle* handle) {
	return (size_t) S2LP_ReadRegister(handle, S2LP_REG_PCKT_PSTMBL);
}

S2LP_CRC_Mode S2LP_PCKT_GetCRCMode(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL1);
	return (S2LP_CRC_Mode) GETBITS(reg_val, 0b111, 5);
}

S2LP_Data_Coding S2LP_PCKT_GetDataCoding(S2LP_Handle* handle) {
	uint8_t reg_vals[2] = { 0 };
	S2LP_BatchReadRegisters(handle, S2LP_REG_PCKTCTRL2, reg_vals, 2);

	if (GETBIT(reg_vals[0], 2)) {
		return S2LP_CODING_3_OUT_OF_6;
	} else if (GETBIT(reg_vals[0], 1)) {
		return S2LP_CODING_MANCHESTER;
	} else if (GETBIT(reg_vals[1], 0)) {
		return S2LP_CODING_FEC;
	}

	return S2LP_CODING_NONE;
}

bool S2LP_PCKT_GetDataWhiteningState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL1);
	return (bool) GETBIT(reg_val, 4);
}

bool S2LP_PCKT_GetCRCFilteringState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKT_FLT_OPTIONS);
	return GETBIT(reg_val, 0);
}

bool S2LP_PCKT_GetAutoPacketFilteringState(S2LP_Handle* handle) {
	uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PROTOCOL1);
	return GETBIT(reg_val, 0);
}
