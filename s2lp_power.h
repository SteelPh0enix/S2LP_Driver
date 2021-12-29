/*
 * s2lp_power.h
 *
 *  Created on: 11 sie 2021
 *      Author: steelph0enix
 */

#ifndef S2LP_S2LP_POWER_H_
#define S2LP_S2LP_POWER_H_

#include "s2lp_constants.h"
#include "s2lp_mcu_interface.h"

// ==== Power/SMPS management ====

// Set SMPS voltage
void S2LP_Power_SetSMPSVoltage(S2LP_Handle* handle, S2LP_SMPS_Voltage voltage);
// Enable or disable KRM (rate multiplier for SMPS)
void S2LP_Power_SetKRMState(S2LP_Handle* handle, bool enabled);
// Set KRM divider ratio
void S2LP_Power_SetKRMRatio(S2LP_Handle* handle, uint16_t ratio);
// Set S2-LP sleep mode
void S2LP_Power_SetSleepMode(S2LP_Handle* handle, S2LP_Sleep_Mode mode);
// Enable or disable LDO bypass
void S2LP_Power_SetLDOBypassState(S2LP_Handle* handle, bool enabled);
// Enable or disable internal SMPS
void S2LP_Power_SetInternalSMPSState(S2LP_Handle* handle, bool enabled);
// Enable or disable battery detection circuit
void S2LP_Power_SetBatteryDetectionState(S2LP_Handle* handle, bool enabled);
// Set BLD threshold
void S2LP_Power_SetBLDThreshold(S2LP_Handle* handle,
        S2LP_BLD_Threshold threshold);
// Set SMPS level mode (should the output level apply to both RX and TX, or just for TX)
void S2LP_Power_SetSMPSLevelMode(S2LP_Handle* handle, S2LP_SMPS_Level_Mode mode);
// Set FIR state (used to smooth the OOK signal)
void S2LP_Power_SetFIRState(S2LP_Handle* handle, bool enabled);

// Configuration values getters
S2LP_SMPS_Voltage S2LP_Power_GetSMPSVoltage(S2LP_Handle* handle);
bool S2LP_Power_GetKRMState(S2LP_Handle* handle);
uint16_t S2LP_Power_GetKRMRatio(S2LP_Handle* handle);
S2LP_Sleep_Mode S2LP_Power_GetSleepMode(S2LP_Handle* handle);
bool S2LP_Power_GetLDOBypassState(S2LP_Handle* handle);
bool S2LP_Power_GetSMPSState(S2LP_Handle* handle);
bool S2LP_Power_GetBatteryDetectionState(S2LP_Handle* handle);
S2LP_BLD_Threshold S2LP_Power_GetBLDThreshold(S2LP_Handle* handle);
S2LP_SMPS_Level_Mode S2LP_Power_GetSMPSLevelMode(S2LP_Handle* handle);
bool S2LP_Power_GetFIRState(S2LP_Handle* handle);

#endif /* S2LP_S2LP_POWER_H_ */
