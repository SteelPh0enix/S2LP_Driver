/*
 * s2lp_utils.c
 *
 *  Created on: 20 sie 2021
 *      Author: steelph0enix
 */

#include "s2lp_utils.h"

char const* S2LP_Utils_ChargePumpCurrentToString(S2LP_ChargePumpCurrent current) {
	switch (current) {
		case S2LP_CHARGE_PUMP_120UA:
			return "120uA";
		case S2LP_CHARGE_PUMP_140UA:
			return "140uA";
		case S2LP_CHARGE_PUMP_200UA:
			return "200uA";
		case S2LP_CHARGE_PUMP_240UA:
			return "240uA";
		case S2LP_CHARGE_PUMP_INVALID:
			return "Invalid";
		default:
			return "Unknown";
	}
}

char const* S2LP_Utils_SynthesizerBandToString(S2LP_SynthesizerBand band) {
	switch (band) {
		case S2LP_SYNTH_BAND_HIGH:
			return "High (826-1055MHz)";
		case S2LP_SYNTH_BAND_MID:
			return "Middle (413-527MHz)";
		default:
			return "Unknown";
	}
}

char const* S2LP_Utils_ModulationToString(S2LP_Modulation modulation) {
	switch (modulation) {
		case S2LP_MODULATION_2FSK:
			return "2-FSK";
		case S2LP_MODULATION_4FSK:
			return "4-FSK";
		case S2LP_MODULATION_2GFSK:
			return "2-GFSK (BT = 1)";
		case S2LP_MODULATION_4GFSK:
			return "4-GFSK (BT = 1)";
		case S2LP_MODULATION_ASK_OOK:
			return "ASK/OOK";
		case S2LP_MODULATION_DIRECT:
			return "Direct polar";
		case S2LP_MODULATION_NONE:
			return "None";
		case S2LP_MODULATION_2GFSK_UNSHAPED:
			return "2-GFSK (BT = 0.5)";
		case S2LP_MODULATION_4GFSK_UNSHAPED:
			return "4-GFSK (BT = 0.5)";
		default:
			return "Unknown";
	}
}

char const* S2LP_Utils_PacketFormatToString(S2LP_Packet_Format format) {
	switch (format) {
		case S2LP_PACKET_BASIC:
			return "BASIC";
		case S2LP_PACKET_802_15_4G:
			return "802.15.4g";
		case S2LP_PACKET_UART:
			return "UART OTA";
		case S2LP_PACKET_STACK:
			return "STack";
		default:
			return "Unknown";
	}
}

char const* S2LP_Utils_StateToString(S2LP_State state) {
	switch (state) {
		case S2LP_STATE_SHUTDOWN:
			return "Shutdown";
		case S2LP_STATE_STANDBY:
			return "Standby";
		case S2LP_STATE_SLEEP_A:
			return "Sleep A";
		case S2LP_STATE_SLEEP_B:
			return "Sleep B";
		case S2LP_STATE_READY:
			return "Ready";
		case S2LP_STATE_LOCK:
			return "Lock";
		case S2LP_STATE_RX:
			return "RX";
		case S2LP_STATE_TX:
			return "TX";
		case S2LP_STATE_SYNTH_SETUP:
			return "Synthesizer setup";
		default:
			return "Unknown";
	}
}

char const* S2LP_Utils_CommandToString(S2LP_Command command) {
	switch (command) {
		case S2LP_CMD_TX:
			return "Transmit";
		case S2LP_CMD_RX:
			return "Receive";
		case S2LP_CMD_READY:
			return "Ready";
		case S2LP_CMD_STANDBY:
			return "Standby";
		case S2LP_CMD_SLEEP:
			return "Sleep";
		case S2LP_CMD_LOCKRX:
			return "Lock RX";
		case S2LP_CMD_LOCKTX:
			return "Lock TX";
		case S2LP_CMD_SABORT:
			return "Abort TX/RX";
		case S2LP_CMD_LDC_RELOAD:
			return "Reload LDC timer";
		case S2LP_CMD_SRES:
			return "Reset";
		case S2LP_CMD_FLUSHRXFIFO:
			return "Clean RX FIFO";
		case S2LP_CMD_FLUSHTXFIFO:
			return "Clean TX FIFO";
		case S2LP_CMD_SEQUENCE_UPDATE:
			return "Reload packet sequence counter";
		default:
			return "Unknown";
	}
}

char const* S2LP_Utils_TXDataSourceToString(S2LP_TX_Source source) {
	switch (source) {
		case S2LP_TX_SOURCE_NORMAL:
			return "Normal";
		case S2LP_TX_SOURCE_DIRECT_FIFO:
			return "Directly from FIFO";
		case S2LP_TX_SOURCE_DIRECT_GPIO:
			return "Directly from GPIO";
		case S2LP_TX_SOURCE_PN9:
			return "Random PN9 sequence";
		default:
			return "Unknown";
	}
}

char const* S2LP_Utils_PreambleToString(S2LP_Preamble preamble) {
	switch (preamble) {
		case S2LP_PREAMBLE_0:
			return "Preamble 0 (0101/0111)";
		case S2LP_PREAMBLE_1:
			return "Preamble 1 (1010/0010)";
		case S2LP_PREAMBLE_2:
			return "Preamble 2 (1100/1101)";
		case S2LP_PREAMBLE_3:
			return "Preamble 3 (0011/1000)";
		default:
			return "Unknown";
	}
}

char const* S2LP_Utils_DataCodingToString(S2LP_Data_Coding coding) {
	switch (coding) {
		case S2LP_CODING_NONE:
			return "None";
		case S2LP_CODING_FEC:
			return "FEC";
		case S2LP_CODING_MANCHESTER:
			return "Manchester";
		case S2LP_CODING_3_OUT_OF_6:
			return "3-out-of-6";
		default:
			return "Unknown";
	}
}

uint32_t S2LP_Utils_CRCToInt(S2LP_CRC_Mode crc_mode) {
	switch (crc_mode) {
		case S2LP_CRC_NO_CRC:
			return 0;
		case S2LP_CRC_POLY_07:
			return 0x07;
		case S2LP_CRC_POLY_8005:
			return 0x8005;
		case S2LP_CRC_POLY_1021:
			return 0x1021;
		case S2LP_CRC_POLY_864CFB:
			return 0x864CFB;
		case S2LP_CRC_POLY_04C011BB7:
			return 0x04C011BB7;
		default:
			return 0;
	}
}

int16_t S2LP_Utils_RSSITodBm(uint8_t rssi) {
	return ((int16_t) rssi) - (int16_t)S2LP_RSSI_DBM_OFFSET;
}

int16_t S2LP_Utils_dBmToRSSI(int8_t dbm) {
	return ((int16_t) dbm) + (int16_t)S2LP_RSSI_DBM_OFFSET;
}
