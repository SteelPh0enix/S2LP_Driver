/*
 * s2lp.h
 *
 *  Created on: 28 cze 2021
 *      Author: SteelPh0enixLocal
 */

#ifndef S2LP_S2LP_H_
#define S2LP_S2LP_H_

#include "s2lp_mcu_interface.h"
#include "s2lp_constants.h"
#include "s2lp_power.h"
#include "s2lp_gpio.h"
#include "s2lp_rf.h"
#include "s2lp_rx.h"
#include "s2lp_tx.h"
#include "s2lp_packet.h"

// Public macro constants

// The delay between state reads during RCO callibration
#define S2LP_RCO_CALLIB_WAIT_TIME 20
// The amount of tries before returning with failure during
// RCO callibration
#define S2LP_RCO_CALLIB_TRIES 10

// ==== Public structures ====

// S2LP status, refreshed after every I/O operation
typedef struct S2LP_Status_t {
	bool xo_on;
	S2LP_State state;
	bool rco_calibrator_error;
	bool rx_fifo_empty;
	bool tx_fifo_full;
	uint8_t ant_sel;
	bool rco_cal_ok;
} S2LP_Status;

// ==== Initialization ====

// Initialize S2LP. Call this after setting up fields in handle, but before calling any other function.
void S2LP_Initialize(S2LP_Handle* handle, S2LP_ClockFrequency frequency);

// ==== Status & interrupt management ====

// Interpret the status bits from handle and return the filled structure.
// This function does NOT perform any I/O operations.
// You have to perform at least one I/O operation before calling this.
S2LP_Status S2LP_GetStatus(S2LP_Handle* handle);

// Basically same as above, but reads the status bits from S2-LP
S2LP_Status S2LP_ReadStatus(S2LP_Handle* handle);

// Get the interrupt status bits from S2-LP.
// You have to do the checking for exact interrupt manually,
// the S2LP_Interrupt enum may come in handy.
// For example, to check for CRC error, do
// S2LP_GetInterrupts(handle) & (1 << S2LP_INT_CRC_ERROR)
// or use macro from bit_helpers.h and do
// GETBIT(S2LP_GetInterrupts(handle), S2LP_INT_CRC_ERROR)
// GetInterrupts cleans the interrupt flags in S2-LP by default.
// You can keep the flags by using GetInterruptsEx with clearFlags = false
uint32_t S2LP_GetInterruptsEx(S2LP_Handle* handle, bool clearFlags);
uint32_t S2LP_GetInterrupts(S2LP_Handle* handle);

// Set interrupts masks
// Works the same way as interrupt status bits - do
// bitwise operations to get/set masks
void S2LP_SetInterruptMasks(S2LP_Handle* handle, uint32_t mask);

// Get currently masked interrupts.
// Works the same way as interrupt status bits - do
// bitwise operations to get/set masks
uint32_t S2LP_GetInterruptMasks(S2LP_Handle* handle);

// Select the FIFO interrupt source: RX FIFO or TX FIFO
// This choice will change the source of FIFO interrupts,
// for example if RX FIFO is selected, FIFO interrupts
// will be based on RX FIFO state.
void S2LP_SetFIFOInterruptSource(S2LP_Handle* handle, S2LP_FIFOSelect fifo);

// Get current FIFO interrupt source
S2LP_FIFOSelect S2LP_GetFIFOInterruptSource(S2LP_Handle* handle);

// ==== Misc ====

// FIFO I/O
void S2LP_ReadFIFO(S2LP_Handle* handle, size_t length, uint8_t* buffer);
void S2LP_WriteFIFO(S2LP_Handle* handle, size_t length, uint8_t* buffer);

// Read device PN and VN
uint8_t S2LP_GetDevicePartNumber(S2LP_Handle* handle);
uint8_t S2LP_GetDeviceVersionNumber(S2LP_Handle* handle);

// Get the base clock frequency
uint32_t S2LP_GetClockFrequency(S2LP_Handle* handle);

// Check if digital clock is base clock / 2
// You can use this function instead of IsRefDivEnabled to avoid
// reading from S2-LP.
bool S2LP_IsDigitalClockDivided(S2LP_Handle* handle);

// Get the digital domain clock frequency
uint32_t S2LP_GetDigitalClockFrequency(S2LP_Handle* handle);

// Enable or disable reference clock divider
// NOTE: Ref divider is automatically set up after initializing S2-LP,
// do not change it unless you know what you're doing (changing fXO while running?)
void S2LP_SetRefDivState(S2LP_Handle* handle, bool enabled);

// Check if reference clock divider is on (REFDIV)
bool S2LP_IsRefDivEnabled(S2LP_Handle* handle);

// Run automatic RCO callibration. Will block until it's complete.
bool S2LP_CallibrateRCO(S2LP_Handle* handle);

#endif /* S2LP_S2LP_H_ */
