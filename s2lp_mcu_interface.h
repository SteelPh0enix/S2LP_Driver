/*
 * s2lp_mcu_interface.h
 *
 *  Created on: 28 cze 2021
 *      Author: SteelPh0enixLocal
 */

#ifndef S2LP_S2LP_MCU_INTERFACE_H_
#define S2LP_S2LP_MCU_INTERFACE_H_

// The size of internal S2LP IO buffer, stored in the handle.
#define S2LP_TX_BUFFER_SIZE 130
#define S2LP_RX_BUFFER_SIZE 130

#include "s2lp_constants.h"

// If your toolchain does not have stdbool.h, implement
// the boolean value with enum or macro
#include <stdbool.h>
#include <stdint.h>

#include <stm32l1xx.h>

typedef struct S2LP_Handle_t {
	// S2-LP common handle fields - don't modify directly!
	uint8_t status[2];
	uint8_t tx_buffer[S2LP_TX_BUFFER_SIZE];
	uint8_t rx_buffer[S2LP_RX_BUFFER_SIZE];

	// Since TX and RX buffer are modified by low-level functions below,
	// these two pointers will be initialized to addresses of
	// buffers that are safe to use by API functions
	uint8_t* tx_buffer_start;
	uint8_t const* rx_buffer_start;

	// Additional data required for library to calculate some
	// stuff correctly

	// S2-LP oscillator frequency, going into XIN input
	S2LP_ClockFrequency frequency;

	// Add your platform fields down below.
	// SPI for communication
	SPI_HandleTypeDef* spi;

	// Chip select pin
	GPIO_TypeDef* gpio_csn_port;
	uint16_t gpio_csn_pin;

	// Shutdown pin
	GPIO_TypeDef* gpio_sdn_port;
	uint16_t gpio_sdn_pin;

	// GPIO
	GPIO_TypeDef* gpio_port[4];
	uint16_t gpio_pin[4];
} S2LP_Handle;

// Functions below are implemented for STM32 HAL by default
// change them for your preferred platform if you need
void S2LP_InitHandle(S2LP_Handle* handle);

void S2LP_WritePin(S2LP_Handle* handle, S2LP_Pin pin, bool state);
bool S2LP_ReadPin(S2LP_Handle* handle, S2LP_Pin pin);

// These functions will use internal buffer from handle to store/read data.
// The address for Write/Read operations should be stored in the first byte of I/O buffer
void S2LP_Write(S2LP_Handle* handle, uint8_t address, size_t length);
void S2LP_Read(S2LP_Handle* handle, uint8_t address, size_t amount);
void S2LP_SendCommand(S2LP_Handle* handle, uint8_t command);

// Millisecond delay
void S2LP_Delay(uint32_t milliseconds);

// These are additional functions that depend directly on the functions above, so you
// don't have to worry about reimplementing them.

// Helper functions for register access
uint8_t S2LP_ReadRegister(S2LP_Handle* handle, S2LP_Register address);
void S2LP_WriteRegister(S2LP_Handle* handle, S2LP_Register address, uint8_t new_value);

// IMPORTANT: Make sure you set the correct address as start. S2LP registers are named
// in reverse order relatively to the addresses, so for example register CRC_FIELD3 has
// the "lowest" address (0xA6), while CRC_FIELD0 has the "highest" address (0xA9).
// Therefore, if you'd want to do batch operation on all the CRC registers, you'd have
// to set start_address = CRC_FIELD3 address, and amount = 4.
void S2LP_BatchReadRegisters(S2LP_Handle* handle, S2LP_Register start_address, uint8_t* output, size_t amount);
void S2LP_BatchWriteRegisters(S2LP_Handle* handle, S2LP_Register start_address, uint8_t* data, size_t amount);

// Chip select control
void S2LP_Select(S2LP_Handle* handle);
void S2LP_Deselect(S2LP_Handle* handle);

// Reset S2-LP
void S2LP_Reset(S2LP_Handle* handle);

// Shutdown pin control
void S2LP_Shutdown(S2LP_Handle* handle);
void S2LP_Wakeup(S2LP_Handle* handle);

#endif /* S2LP_S2LP_MCU_INTERFACE_H_ */
