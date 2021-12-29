/*
 * s2lp_utils.h
 *
 *  Created on: 20 sie 2021
 *      Author: steelph0enix
 */

#ifndef S2LP_S2LP_UTILS_H_
#define S2LP_S2LP_UTILS_H_

#include "s2lp_constants.h"
#include <stdint.h>

// Misc utilities for S2-LP diagnostics and debugging

#define S2LP_RSSI_DBM_OFFSET 146

// Convert some enums into strings
char const* S2LP_Utils_ChargePumpCurrentToString(S2LP_ChargePumpCurrent current);
char const* S2LP_Utils_SynthesizerBandToString(S2LP_SynthesizerBand band);
char const* S2LP_Utils_ModulationToString(S2LP_Modulation modulation);
char const* S2LP_Utils_PacketFormatToString(S2LP_Packet_Format format);
char const* S2LP_Utils_StateToString(S2LP_State state);
char const* S2LP_Utils_CommandToString(S2LP_Command command);
char const* S2LP_Utils_TXDataSourceToString(S2LP_TX_Source source);
char const* S2LP_Utils_PreambleToString(S2LP_Preamble preamble);
char const* S2LP_Utils_DataCodingToString(S2LP_Data_Coding coding);

// Get numerical value of CRC from enum
uint32_t S2LP_Utils_CRCToInt(S2LP_CRC_Mode crc_mode);

// Convert RSSI <-> dBm
int16_t S2LP_Utils_RSSITodBm(uint8_t rssi);
int16_t S2LP_Utils_dBmToRSSI(int8_t dbm);

#endif /* S2LP_S2LP_UTILS_H_ */
