/*
 * s2lp_tx.h
 *
 *  Created on: 6 wrz 2021
 *      Author: steelph0enix
 */

#ifndef S2LP_S2LP_TX_H_
#define S2LP_S2LP_TX_H_

#include "s2lp_mcu_interface.h"
#include "bit_helpers.h"

// === Helper macros ===

#define S2LP_DBM_TO_POWER_LEVEL_STEP(dbm) CHANGE_VALUE_RANGE(dbm, -30, 14, 127, 1)
#define S2LP_POWER_LEVEL_STEP_TO_DBM(step) CHANGE_VALUE_RANGE(step, 127, 1, -30, 14)

// === TX-related functions ===

// Power ramp/level settings - see datasheet, section 5.6.1 for details
// This function does three things: disables power ramping, sets the last
// power ramping step to 8 and sets 8th power slot to provided value,
// effectively setting the TX power level at fixed value. Use this if you
// don't know what you're doing.
void S2LP_TX_SetStaticPowerLevel(S2LP_Handle* handle, uint8_t power_level);
// Set power values for each step. Steps are configured between -30dB and +14dB, with 0.5dB precision.
// You can use helper macro S2LP_DBM_TO_RAMP_STEP and S2LP_RAMP_STEP_TO_DBM to convert the values.
// The value range is from 0 to 90.
// Steps HAVE TO be configured in descending order - steps[0] should have the biggest value
void S2LP_TX_SetPowerRampSteps(S2LP_Handle* handle, uint8_t steps[8]);
// Set power ramping step length (duration)
void S2LP_TX_SetPowerRampStepLength(S2LP_Handle* handle, uint8_t length);
// Set final power ramping step, which will be used as stable setting for TX
// In other words - this step's set power will be the TX power
// Range: [0, 7]
void S2LP_TX_SetPowerRampStepMax(S2LP_Handle* handle, uint8_t max_step);
// Enable or disable power ramping. If disabled, maximum power step will be used
void S2LP_TX_SetRampingState(S2LP_Handle* handle, bool enabled);
// Important: enabling max power transmission will disable power ramping
void S2LP_TX_SetMaxPowerState(S2LP_Handle* handle, bool enabled);
// Let S2-LP linearly interpolate the power levels
void S2LP_TX_SetRampingInterpolationState(S2LP_Handle* handle, bool enabled);
// Data source - where the TX should get data from
void S2LP_TX_SetDataSource(S2LP_Handle* handle, S2LP_TX_Source source);
// TX FIFO Management
void S2LP_TX_SetFIFOAlmostFullThreshold(S2LP_Handle* handle, uint8_t threshold);
void S2LP_TX_SetFIFOAlmostEmptyThreshold(S2LP_Handle* handle,
                                         uint8_t threshold);
// Set retransmission tries amount (from 0 to 15)
void S2LP_TX_SetRetransmissionTries(S2LP_Handle* handle, uint8_t tries);

// IMPORTANT: steps_buffer must be at least 8 bytes long.
void S2LP_TX_GetPowerRampSteps(S2LP_Handle* handle, uint8_t* steps_buffer);
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
