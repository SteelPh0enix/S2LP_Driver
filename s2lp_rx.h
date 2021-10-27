/*
 * s2lp_rx.h
 *
 *  Created on: 30 sie 2021
 *      Author: steelph0enix
 */

#ifndef S2LP_S2LP_RX_H_
#define S2LP_S2LP_RX_H_

#include "s2lp_mcu_interface.h"

// === RX-related functionalities ===

// Automatic frequency compensation (datasheet section 5.5.1)

// Set RSSI threshold. You can use RSSITodBm and dBmToRSSI functions
// from Utils module to convert dBm <-> RSSI
void S2LP_RX_SetRSSIThreshold(S2LP_Handle* handle, uint8_t rssi);
// Set AFC loop gain log2 in fast and slow mode (range: 0..15)
void S2LP_RX_SetAFCFastLoopGain(S2LP_Handle* handle, uint8_t gain);
void S2LP_RX_SetAFCSlowLoopGain(S2LP_Handle* handle, uint8_t gain);
// Set AFC fast period (length of the fast period in number of samples).
// Consult manual for more information (section 5.5.1)
void S2LP_RX_SetAFCFastPeriod(S2LP_Handle* handle, uint8_t period);
// Set AFC mode. NOTE: This will not enable AFC, you have to use
// SetAFCState to enable it!
void S2LP_RX_SetAFCMode(S2LP_Handle* handle, S2LP_AFC_Mode mode);
void S2LP_RX_SetAFCState(S2LP_Handle* handle, bool enabled);
// Enable/disable AFC freeze on sync word detection
void S2LP_RX_SetAFCFreezeOnSyncState(S2LP_Handle* handle, bool enabled);

// Automatic gain control (datasheet section 5.5.2)

// Set AGC high threshold
void S2LP_RX_SetAGCHighThreshold(S2LP_Handle* handle, uint8_t value);
// Set AGC low threshold (since S2-LP has two low thresholds, you have
// to specify which one you're setting via enum argument)
void S2LP_RX_SetAGCLowThreshold(S2LP_Handle* handle, S2LP_AGC_Low_Threshold threshold, uint8_t value);
// Set AGC measurement interval raw register value, see datasheet for more info.
// Range: 0..15
void S2LP_RX_SetAGCMeasureTimeRaw(S2LP_Handle* handle, uint8_t time);
// Set AGC wait time raw register value, see datasheet for more info. Range: 0..31
void S2LP_RX_SetAGCHoldTimeRaw(S2LP_Handle* handle, uint8_t time);
// Enable/disable AGC
void S2LP_RX_SetAGCState(S2LP_Handle* handle, bool enabled);
// Enable/disable AGC level freezing after sync word has been received
void S2LP_RX_SetAGCFreezeOnSyncState(S2LP_Handle* handle, bool enabled);
void S2LP_RX_SetChannelFilterValueRaw(S2LP_Handle* handle, uint8_t mantissa, uint8_t exponent);

// Set carrier sense mode (static or dynamic with threshold)
void S2LP_RX_SetCarrierSenseMode(S2LP_Handle* handle, S2LP_CS_Mode mode);

// RX timer management - see datasheet, section 5.5.6 for more details
void S2LP_RX_SetTimerStopConfig(S2LP_Handle* handle, bool rx_timeout_and_or, bool cs_timeout, bool sqi_timeout,
		bool pqi_timeout);

// Enables or disables CS blanking, see datasheet section 5.5.9 for details
void S2LP_RX_SetCSBlankingState(S2LP_Handle* handle, bool enabled);

// Set data source for RX mode to bypass packet handler engine and
// output raw data either into FIFO or GPIO pin. See datasheet, section
// 5.5.7 for more information
void S2LP_RX_SetDataSource(S2LP_Handle* handle, S2LP_RX_Source source);

// FIFO management
void S2LP_RX_SetFIFOAlmostFullThreshold(S2LP_Handle* handle, uint8_t threshold);
void S2LP_RX_SetFIFOAlmostEmptyThreshold(S2LP_Handle* handle, uint8_t threshold);

// Calculate real AGC measurement time interval value in seconds.
// Should be ranging from 0.5us to about 15ms
double S2LP_RX_CalculateAGCMeasureTime(S2LP_Handle* handle, uint8_t time);

// Calculate real AGC hold time value in seconds.
// Should be ranging from 0.5us to about 32us
double S2LP_RX_CalculateAGCHoldTime(S2LP_Handle* handle, uint8_t time);

// Calculates real RX channel filter bandwidth
double S2LP_RX_CalculateChannelFilterBandwidth(S2LP_Handle* handle, uint8_t mantissa, uint8_t exponent);

// Getters
uint8_t S2LP_RX_GetRSSIThreshold(S2LP_Handle* handle);
uint8_t S2LP_RX_GetAFCFastLoopGain(S2LP_Handle* handle);
uint8_t S2LP_RX_GetAFCSlowLoopGain(S2LP_Handle* handle);
uint8_t S2LP_RX_GetAFCFastPeriod(S2LP_Handle* handle);
S2LP_AFC_Mode S2LP_RX_GetAFCMode(S2LP_Handle* handle);
bool S2LP_RX_GetAFCState(S2LP_Handle* handle);
bool S2LP_RX_GetAFCFreezeOnSyncState(S2LP_Handle* handle);
uint8_t S2LP_RX_GetAGCHighThreshold(S2LP_Handle* handle);
uint8_t S2LP_RX_GetAGCLowThreshold(S2LP_Handle* handle, S2LP_AGC_Low_Threshold threshold);
uint8_t S2LP_RX_GetAGCMeasureTimeRaw(S2LP_Handle* handle);
uint8_t S2LP_RX_GetAGCHoldTimeRaw(S2LP_Handle* handle);
bool S2LP_RX_GetAGCState(S2LP_Handle* handle);
bool S2LP_RX_GetAGCFreezeOnSyncState(S2LP_Handle* handle);
uint8_t S2LP_RX_GetChannelFilterMantissa(S2LP_Handle* handle);
uint8_t S2LP_RX_GetChannelFilterExponent(S2LP_Handle* handle);
void S2LP_RX_GetChannelFilterValueRaw(S2LP_Handle* handle, uint8_t* mantissa, uint8_t* exponent);
double S2LP_RX_GetChannelFilterValue(S2LP_Handle* handle);
S2LP_CS_Mode S2LP_RX_GetCarrierSenseMode(S2LP_Handle* handle);
void S2LP_RX_GetTimerStopConfig(S2LP_Handle* handle, bool* rx_timeout_and_or, bool* cs_timeout, bool* sqi_timeout,
		bool* pqi_timeout);
S2LP_RX_Source S2LP_RX_GetDataSource(S2LP_Handle* handle);

// GetCurrentRSSI returns value directly from RSSI filter
uint8_t S2LP_RX_GetCurrentRSSI(S2LP_Handle* handle);
// GetCapturedRSSI returns value captured at the end of SYNC/
// on SABORT/on RX timeout expiration
uint8_t S2LP_RX_GetCapturedRSSI(S2LP_Handle* handle);
bool S2LP_RX_GetCSBlankingState(S2LP_Handle* handle);
bool S2LP_RX_GetCarrierSenseIndicator(S2LP_Handle* handle);
// is_for_secondary_sync returns the information if SQI applies
// to first (false) or second (true) SYNC word. Can be NULL.
uint8_t S2LP_RX_GetLastPacketSQI(S2LP_Handle* handle, bool* is_for_secondary_sync);
uint8_t S2LP_RX_GetLastPacketPQI(S2LP_Handle* handle);
uint8_t S2LP_RX_GetPQIThreshold(S2LP_Handle* handle);
bool S2LP_RX_GetSQICheckStatus(S2LP_Handle* handle);
uint8_t S2LP_RX_GetSQIThreshold(S2LP_Handle* handle);
uint8_t S2LP_RX_GetFIFOAlmostFullThreshold(S2LP_Handle* handle);
uint8_t S2LP_RX_GetFIFOAlmostEmptyThreshold(S2LP_Handle* handle);
// Get the amount of items in RX FIFO
uint8_t S2LP_RX_GetFIFOCount(S2LP_Handle* handle);

#endif /* S2LP_S2LP_RX_H_ */
