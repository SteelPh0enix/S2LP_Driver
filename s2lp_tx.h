/*
 * s2lp_tx.h
 *
 *  Created on: 6 wrz 2021
 *      Author: steelph0enix
 */

#ifndef S2LP_S2LP_TX_H_
#define S2LP_S2LP_TX_H_

#include "s2lp_mcu_interface.h"

// === TX-related functions ===

// Power ramp settings - see datasheet, section 5.6.1 for details
void S2LP_TX_SetPowerRampStepLength(S2LP_Handle* handle, uint8_t length);
void S2LP_TX_SetPowerRampStepMax(S2LP_Handle* handle, uint8_t max_step);
void S2LP_TX_SetRampingState(S2LP_Handle* handle, bool enabled);
// Important: enabling max power transmission will disable power ramping
void S2LP_TX_SetMaxPowerState(S2LP_Handle* handle, bool enabled);
void S2LP_TX_SetRampingInterpolationState(S2LP_Handle* handle, bool enabled);
// Data source - where the TX should get data from
void S2LP_TX_SetDataSource(S2LP_Handle* handle, S2LP_TX_Source source);
// TX FIFO Management
void S2LP_TX_SetFIFOAlmostFullThreshold(S2LP_Handle* handle, uint8_t threshold);
void S2LP_TX_SetFIFOAlmostEmptyThreshold(S2LP_Handle* handle, uint8_t threshold);
// Set retransmission tries amount (from 0 to 15)
void S2LP_TX_SetRetransmissionTries(S2LP_Handle* handle, uint8_t tries);

uint8_t S2LP_TX_GetPowerRampStepLength(S2LP_Handle* handle);
uint8_t S2LP_TX_GetPowerRampStepMax(S2LP_Handle* handle);
bool S2LP_TX_GetRampingState(S2LP_Handle* handle);
bool S2LP_TX_GetMaxPowerState(S2LP_Handle* handle);
bool S2LP_TX_GetRampingInterpolationState(S2LP_Handle* handle);
S2LP_TX_Source S2LP_TX_GetDataSource(S2LP_Handle* handle);
uint8_t S2LP_TX_GetFIFOAlmostFullThreshold(S2LP_Handle* handle);
uint8_t S2LP_TX_GetFIFOAlmostEmptyThreshold(S2LP_Handle* handle);
// Get number of elements from TX FIFO
uint8_t S2LP_TX_GetFIFOCount(S2LP_Handle* handle);
uint8_t S2LP_TX_GetRetransmissionTries(S2LP_Handle* handle);

#endif /* S2LP_S2LP_TX_H_ */
