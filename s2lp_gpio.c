/*
 * s2lp_gpio.c
 *
 *  Created on: 11 sie 2021
 *      Author: steelph0enix
 */

#include "s2lp_gpio.h"
#include "bit_helpers.h"

// ===== Local helper functions =====

inline static void S2LP_WritePinConfig(S2LP_Handle* handle, S2LP_Pin pin) {
	S2LP_Select(handle);
	switch (pin) {
		case S2LP_PIN_GPIO_0: {
			S2LP_Write(handle, S2LP_REG_GPIO0_CONF, 1);
			break;
		}
		case S2LP_PIN_GPIO_1: {
			S2LP_Write(handle, S2LP_REG_GPIO1_CONF, 1);
			break;
		}
		case S2LP_PIN_GPIO_2: {
			S2LP_Write(handle, S2LP_REG_GPIO2_CONF, 1);
			break;
		}
		case S2LP_PIN_GPIO_3: {
			S2LP_Write(handle, S2LP_REG_GPIO3_CONF, 1);
			break;
		}
		case S2LP_PIN_CSN:
		case S2LP_PIN_SDN:
		default:
			break;
	}
	S2LP_Deselect(handle);
}

inline static void S2LP_ReadPinConfig(S2LP_Handle* handle, S2LP_Pin pin) {
	S2LP_Select(handle);
	switch (pin) {
		case S2LP_PIN_GPIO_0: {
			S2LP_Read(handle, S2LP_REG_GPIO0_CONF, 1);
			break;
		}
		case S2LP_PIN_GPIO_1: {
			S2LP_Read(handle, S2LP_REG_GPIO1_CONF, 1);
			break;
		}
		case S2LP_PIN_GPIO_2: {
			S2LP_Read(handle, S2LP_REG_GPIO2_CONF, 1);
			break;
		}
		case S2LP_PIN_GPIO_3: {
			S2LP_Read(handle, S2LP_REG_GPIO3_CONF, 1);
			break;
		}
		case S2LP_PIN_CSN:
		case S2LP_PIN_SDN:
		default:
			break;
	}
	S2LP_Deselect(handle);
}

// ===== Library implementation =====

void S2LP_GPIO_SetPinOutput(S2LP_Handle* handle, S2LP_Pin pin, S2LP_GPIO_Output_Mode mode) {
	S2LP_GPIO_SetPinOutputEx(handle, pin, mode, true);
}

void S2LP_GPIO_SetPinOutputEx(S2LP_Handle* handle, S2LP_Pin pin, S2LP_GPIO_Output_Mode mode, bool low_power) {

	SETBITS(handle->tx_buffer_start[0], (low_power ? S2LP_PINMODE_OUTPUT_LP : S2LP_PINMODE_OUTPUT_HP), 0b11, 0);
	SETBITS(handle->tx_buffer_start[0], mode, 0b11111, 3);

	S2LP_WritePinConfig(handle, pin);
}

void S2LP_GPIO_SetPinInput(S2LP_Handle* handle, S2LP_Pin pin, S2LP_GPIO_Input_Mode mode) {

	SETBITS(handle->tx_buffer_start[0], S2LP_PINMODE_INPUT, 0b11, 0);
	SETBITS(handle->tx_buffer_start[0], mode, 0b11111, 3);

	S2LP_WritePinConfig(handle, pin);
}

S2LP_PinMode S2LP_GPIO_GetPinMode(S2LP_Handle* handle, S2LP_Pin pin) {
	S2LP_ReadPinConfig(handle, pin);
	return (S2LP_PinMode) (GETBITS(handle->rx_buffer_start[0], 0b11, 0));
}

S2LP_GPIO_Input_Mode S2LP_GPIO_GetPinInputMode(S2LP_Handle* handle, S2LP_Pin pin) {
	S2LP_PinMode mode = S2LP_GPIO_GetPinMode(handle, pin);
	// I have no clue in what context analog mode is used, so i'm just gonna handle it in both cases
	if (mode == S2LP_PINMODE_INPUT || mode == S2LP_PINMODE_ANALOG) {
		return (S2LP_GPIO_Input_Mode) (GETBITS(handle->rx_buffer_start[0], 0b11111, 3));
	}
	return S2LP_GPIO_IN_INVALID;
}

S2LP_GPIO_Output_Mode S2LP_GPIO_GetPinOutputMode(S2LP_Handle* handle, S2LP_Pin pin) {
	S2LP_PinMode mode = S2LP_GPIO_GetPinMode(handle, pin);
	// I have no clue in what context analog mode is used, so i'm just gonna handle it in both cases
	if (mode == S2LP_PINMODE_OUTPUT_LP || mode == S2LP_PINMODE_OUTPUT_HP || mode == S2LP_PINMODE_ANALOG) {
		return (S2LP_GPIO_Output_Mode) (GETBITS(handle->rx_buffer_start[0], 0b11111, 3));
	}
	return S2LP_GPIO_OUT_INVALID;
}
