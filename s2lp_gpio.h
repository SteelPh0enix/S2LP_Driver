/*
 * s2lp_gpio.h
 *
 *  Created on: 11 sie 2021
 *      Author: steelph0enix
 */

#ifndef S2LP_S2LP_GPIO_H_
#define S2LP_S2LP_GPIO_H_

#include "s2lp_constants.h"
#include "s2lp_mcu_interface.h"

// ==== GPIO management ====

// Set GPIO mode
void S2LP_GPIO_SetPinOutput(S2LP_Handle* handle, S2LP_Pin pin, S2LP_GPIO_Output_Mode mode);
void S2LP_GPIO_SetPinOutputEx(S2LP_Handle* handle, S2LP_Pin pin, S2LP_GPIO_Output_Mode mode, bool low_power);
void S2LP_GPIO_SetPinInput(S2LP_Handle* handle, S2LP_Pin pin, S2LP_GPIO_Input_Mode mode);

// Get GPIO pin mode. IMPORTANT: GetPinInput/OutputMode functions checks the pin mode.
// If you try to read input mode of output pin (or vice versa),
// it will return invalid value (S2LP_GPIO_IN/OUT_INVALID -> 0xFF)
S2LP_PinMode S2LP_GPIO_GetPinMode(S2LP_Handle* handle, S2LP_Pin pin);
S2LP_GPIO_Input_Mode S2LP_GPIO_GetPinInputMode(S2LP_Handle* handle, S2LP_Pin pin);
S2LP_GPIO_Output_Mode S2LP_GPIO_GetPinOutputMode(S2LP_Handle* handle, S2LP_Pin pin);

#endif /* S2LP_S2LP_GPIO_H_ */
