/*
 * s2lp_packet.h
 *
 *  Created on: 21 wrz 2021
 *      Author: steelph0enix
 */

#ifndef S2LP_S2LP_PACKET_H_
#define S2LP_S2LP_PACKET_H_

#include "s2lp_mcu_interface.h"

void S2LP_PCKT_SetPacketFormat(S2LP_Handle* handle, S2LP_Packet_Format format);
// See table 52 on page 45 (section 7.1) in datasheet for more info
void S2LP_PCKT_SetPreambleType(S2LP_Handle* handle, S2LP_Preamble preamble);
// Preamble length is in range (0, 1024), because the register keeps the amount of bit pairs,
// not the amount of bits, so real preamble will be 2x longer
void S2LP_PCKT_SetPreambleLength(S2LP_Handle* handle, size_t length);
// Set sync word length (from 0 to 32)
void S2LP_PCKT_SetSyncLength(S2LP_Handle* handle, size_t length);
// Set packet length in bytes (from 0 to 65535 bytes)
void S2LP_PCKT_SetPacketLength(S2LP_Handle* handle, size_t length);
// IMPORTANT: WORKS ONLY FOR THE RX SIDE! FOR TX, YOU HAVE TO SET PACKET LENGTH ANYWAY!
// If enabled, packet length will be extracted from `length` field in packet.
// If disabled, packet length set using SetPacketLenght will be used instead.
void S2LP_PCKT_SetVariablePacketLengthState(S2LP_Handle* handle, bool enabled);
// Set the `length` field size (1B or 2B). Must be the same for both RX and TX devices.
void S2LP_PCKT_SetLengthFieldSize(S2LP_Handle* handle, S2LP_Length_Field_Size size);
// Enable packet addressing
void S2LP_PCKT_SetDestinationAddressState(S2LP_Handle* handle, bool enabled);
// Set packet destination and source address
// Sorce address is available only in STack packet.
void S2LP_PCKT_SetDestinationAddress(S2LP_Handle* handle, uint8_t address);
void S2LP_PCKT_SetSourceAddress(S2LP_Handle* handle, uint8_t address);
// Set packet postamble length (0 to 255)
void S2LP_PCKT_SetPostambleLength(S2LP_Handle* handle, size_t length);
void S2LP_PCKT_SetCRCMode(S2LP_Handle* handle, S2LP_CRC_Mode mode);
void S2LP_PCKT_SetDataCoding(S2LP_Handle* handle, S2LP_Data_Coding mode);
void S2LP_PCKT_SetDataWhiteningState(S2LP_Handle* handle, bool enabled);
void S2LP_PCKT_SetCRCFilteringState(S2LP_Handle* handle, bool enabled);
void S2LP_PCKT_SetAutoPacketFilteringState(S2LP_Handle* handle, bool enabled);

S2LP_Packet_Format S2LP_PCKT_GetPacketFormat(S2LP_Handle* handle);
S2LP_Preamble S2LP_PCKT_GetPreambleType(S2LP_Handle* handle);
size_t S2LP_PCKT_GetPreambleLength(S2LP_Handle* handle);
size_t S2LP_PCKT_GetSyncLength(S2LP_Handle* handle);
size_t S2LP_PCKT_GetTxPacketLength(S2LP_Handle* handle);
size_t S2LP_PCKT_GetRxPacketLength(S2LP_Handle* handle);
bool S2LP_PCKT_GetVariablePacketLengthState(S2LP_Handle* handle);
S2LP_Length_Field_Size S2LP_PCKT_GetLengthFieldSize(S2LP_Handle* handle);
bool S2LP_PCKT_GetDestinationAddressState(S2LP_Handle* handle);
uint8_t S2LP_PCKT_GetDestinationAddress(S2LP_Handle* handle);
uint8_t S2LP_PCKT_GetSourceAddress(S2LP_Handle* handle);
size_t S2LP_PCKT_GetPostambleLength(S2LP_Handle* handle);
S2LP_CRC_Mode S2LP_PCKT_GetCRCMode(S2LP_Handle* handle);
S2LP_Data_Coding S2LP_PCKT_GetDataCoding(S2LP_Handle* handle);
bool S2LP_PCKT_GetDataWhiteningState(S2LP_Handle* handle);
bool S2LP_PCKT_GetCRCFilteringState(S2LP_Handle* handle);
bool S2LP_PCKT_GetAutoPacketFilteringState(S2LP_Handle* handle);

#endif /* S2LP_S2LP_PACKET_H_ */
