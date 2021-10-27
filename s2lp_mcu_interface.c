/*
 * s2lp_mcu_interface.c
 *
 *  Created on: 28 cze 2021
 *      Author: SteelPh0enixLocal
 */

#include "s2lp_mcu_interface.h"
#include "main.h"
#include "cmsis_os.h"

#include <string.h>

// Low-level, internal constants

uint8_t const S2LP_HEADER_BYTE_WRITE = 0b00000000;
uint8_t const S2LP_HEADER_BYTE_READ = 0b00000001;
uint8_t const S2LP_HEADER_BYTE_COMMAND = 0b10000000;

uint32_t const S2LP_RESET_TIMEOUT = 2;

// MCU-specific functions implementation

void S2LP_InitHandle(S2LP_Handle* handle) {
	memset(handle->status, 0, 2);
	memset(handle->tx_buffer, 0, S2LP_TX_BUFFER_SIZE);
	memset(handle->rx_buffer, 0, S2LP_RX_BUFFER_SIZE);

	handle->tx_buffer_start = &(handle->tx_buffer[2]);
	handle->rx_buffer_start = &(handle->rx_buffer[2]);

	handle->frequency = S2LP_CLOCK_FREQ_INVALID;

	// You can put your platform init code here
}

void S2LP_GetPin(S2LP_Handle* handle, S2LP_Pin pin, GPIO_TypeDef** port_out, uint16_t* pin_out) {
	switch (pin) {
		case S2LP_PIN_CSN:
			*port_out = handle->gpio_csn_port;
			*pin_out = handle->gpio_csn_pin;
			break;
		case S2LP_PIN_SDN:
			*port_out = handle->gpio_sdn_port;
			*pin_out = handle->gpio_sdn_pin;
			break;
		case S2LP_PIN_GPIO_0:
			*port_out = handle->gpio_port[0];
			*pin_out = handle->gpio_pin[0];
			break;
		case S2LP_PIN_GPIO_1:
			*port_out = handle->gpio_port[1];
			*pin_out = handle->gpio_pin[1];
			break;
		case S2LP_PIN_GPIO_2:
			*port_out = handle->gpio_port[2];
			*pin_out = handle->gpio_pin[2];
			break;
		case S2LP_PIN_GPIO_3:
			*port_out = handle->gpio_port[3];
			*pin_out = handle->gpio_pin[3];
			break;
		default:
			break;
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
void S2LP_WritePin(S2LP_Handle* handle, S2LP_Pin pin, bool state) {
	GPIO_TypeDef* gpio_port = NULL;
	uint16_t gpio_pin = 0;

	S2LP_GetPin(handle, pin, &gpio_port, &gpio_pin);

	if (gpio_port != NULL) {
		HAL_GPIO_WritePin(gpio_port, gpio_pin, state);
	}
}

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
bool S2LP_ReadPin(S2LP_Handle* handle, S2LP_Pin pin) {
	GPIO_TypeDef* gpio_port = NULL;
	uint16_t gpio_pin = 0;

	S2LP_GetPin(handle, pin, &gpio_port, &gpio_pin);

	if (gpio_port != NULL) {
		return (HAL_GPIO_ReadPin(gpio_port, gpio_pin));
	}

	return (false);
}

void S2LP_Write(S2LP_Handle* handle, uint8_t address, size_t length) {
	handle->tx_buffer[0] = S2LP_HEADER_BYTE_WRITE;
	handle->tx_buffer[1] = address;

	// ACTUAL PLATFORM CODE START
	HAL_SPI_TransmitReceive(handle->spi, &(handle->tx_buffer[0]), &(handle->rx_buffer[0]), (uint16_t) (length + 2),
	HAL_MAX_DELAY);
	// ACTUAL PLATFORM CODE END

	// First two bytes are S2-LP status bits, so we copy them (in reverse order)
	handle->status[0] = handle->rx_buffer[1];
	handle->status[1] = handle->rx_buffer[0];
}

void S2LP_Read(S2LP_Handle* handle, uint8_t address, size_t amount) {
	handle->tx_buffer[0] = S2LP_HEADER_BYTE_READ;
	handle->tx_buffer[1] = address;

	// ACTUAL PLATFORM CODE START
	HAL_SPI_TransmitReceive(handle->spi, &(handle->tx_buffer[0]), &(handle->rx_buffer[0]), (uint16_t) (amount + 2),
	HAL_MAX_DELAY);
	// ACTUAL PLATFORM CODE END

	handle->status[0] = handle->rx_buffer[1];
	handle->status[1] = handle->rx_buffer[0];
}

void S2LP_SendCommand(S2LP_Handle* handle, uint8_t command) {
	handle->tx_buffer[0] = S2LP_HEADER_BYTE_COMMAND;
	handle->tx_buffer[1] = command;

	// ACTUAL PLATFORM CODE START
	S2LP_Select(handle);
	HAL_SPI_TransmitReceive(handle->spi, &(handle->tx_buffer[0]), &(handle->rx_buffer[0]), 2,
	HAL_MAX_DELAY);
	S2LP_Deselect(handle);
	// ACTUAL PLATFORM CODE END

	handle->status[0] = handle->rx_buffer[1];
	handle->status[1] = handle->rx_buffer[0];
}

uint8_t S2LP_ReadRegister(S2LP_Handle* handle, S2LP_Register address) {
	S2LP_Select(handle);
	S2LP_Read(handle, address, 1);
	S2LP_Deselect(handle);
	return handle->rx_buffer_start[0];
}

void S2LP_WriteRegister(S2LP_Handle* handle, S2LP_Register address, uint8_t new_value) {
	handle->tx_buffer_start[0] = new_value;
	S2LP_Select(handle);
	S2LP_Write(handle, address, 1);
	S2LP_Deselect(handle);
}

void S2LP_BatchReadRegisters(S2LP_Handle* handle, S2LP_Register start_address, uint8_t* output, size_t amount) {
	S2LP_Select(handle);
	S2LP_Read(handle, start_address, amount);
	S2LP_Deselect(handle);
	memcpy(output, handle->rx_buffer_start, amount);
}

void S2LP_BatchWriteRegisters(S2LP_Handle* handle, S2LP_Register start_address, uint8_t* data, size_t amount) {
	memcpy(handle->tx_buffer_start, data, amount);
	S2LP_Select(handle);
	S2LP_Write(handle, start_address, amount);
	S2LP_Deselect(handle);
}

void S2LP_Select(S2LP_Handle* handle) {
	S2LP_WritePin(handle, S2LP_PIN_CSN, false);
}

void S2LP_Deselect(S2LP_Handle* handle) {
	S2LP_WritePin(handle, S2LP_PIN_CSN, true);
}

void S2LP_Reset(S2LP_Handle* handle) {
	S2LP_Shutdown(handle);
	S2LP_Delay(S2LP_RESET_TIMEOUT);
	S2LP_Wakeup(handle);
	S2LP_Delay(S2LP_RESET_TIMEOUT);
}

void S2LP_Shutdown(S2LP_Handle* handle) {
	S2LP_WritePin(handle, S2LP_PIN_SDN, true);
}

void S2LP_Wakeup(S2LP_Handle* handle) {
	S2LP_WritePin(handle, S2LP_PIN_SDN, false);
}

void S2LP_Delay(uint32_t milliseconds) {
	osDelay(milliseconds);
}
