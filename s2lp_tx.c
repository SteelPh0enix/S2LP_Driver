/*
 * s2lp_tx.c
 *
 *  Created on: 6 wrz 2021
 *      Author: steelph0enix
 */

#include "bit_helpers.h"
#include "s2lp_tx.h"

void S2LP_TX_SetStaticPowerLevel(S2LP_Handle* handle, uint8_t power_level) {
    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER8, power_level);
    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER7, power_level);
    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER6, power_level);
    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER5, power_level);
    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER4, power_level);
    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER3, power_level);
    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER2, power_level);
    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER1, power_level);
    S2LP_TX_SetPowerRampStepMax(handle, 7);
    S2LP_TX_SetRampingState(handle, false);
}

void S2LP_TX_SetPowerRampSteps(S2LP_Handle* handle, uint8_t steps[8]) {
    S2LP_BatchWriteRegisters(handle, S2LP_REG_PA_POWER8, steps, 8);
}

void S2LP_TX_SetPowerRampStepLength(S2LP_Handle* handle, uint8_t length) {
    if (length > 0b11) {
        return;
    }

    uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PA_POWER0);
    CLEARBITS(reg_val, 0b11, 3);
    SETBITS(reg_val, length, 0b11, 3);
    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER0, reg_val);
}

void S2LP_TX_SetPowerRampStepMax(S2LP_Handle* handle, uint8_t max_step) {
    if (max_step > 0b111) {
        return;
    }

    uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PA_POWER0);
    CLEARBITS(reg_val, 0b111, 0);
    SETBITS(reg_val, max_step, 0b111, 0);
    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER0, reg_val);
}

void S2LP_TX_SetRampingState(S2LP_Handle* handle, bool enabled) {
    uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PA_POWER0);

    if (enabled) {
        SETBIT(reg_val, 5);
    }
    else {
        CLEARBIT(reg_val, 5);
    }

    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER0, reg_val);
}

void S2LP_TX_SetMaxPowerState(S2LP_Handle* handle, bool enabled) {
    uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PA_POWER0);

    if (enabled) {
        SETBIT(reg_val, 6);
    }
    else {
        CLEARBIT(reg_val, 6);
    }

    S2LP_WriteRegister(handle, S2LP_REG_PA_POWER0, reg_val);
}

void S2LP_TX_SetRampingInterpolationState(S2LP_Handle* handle, bool enabled) {
    uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_MOD1);
    if (enabled) {
        SETBIT(reg_val, 7);
    }
    else {
        CLEARBIT(reg_val, 7);
    }

    S2LP_WriteRegister(handle, S2LP_REG_MOD1, reg_val);
}

void S2LP_TX_SetDataSource(S2LP_Handle* handle, S2LP_TX_Source source) {
    uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL1);
    CLEARBITS(reg_val, 0b11, 2);
    SETBITS(reg_val, (uint8_t )source, 0b11, 2);
    S2LP_WriteRegister(handle, S2LP_REG_PCKTCTRL1, reg_val);
}

void S2LP_TX_SetFIFOAlmostFullThreshold(S2LP_Handle* handle, uint8_t threshold) {
    uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_FIFO_CONFIG1);

    CLEARBITS(reg_val, 0b1111111, 0);
    SETBITS(reg_val, threshold, 0b1111111, 0);

    S2LP_WriteRegister(handle, S2LP_REG_FIFO_CONFIG1, reg_val);
}

void S2LP_TX_SetFIFOAlmostEmptyThreshold(S2LP_Handle* handle, uint8_t threshold) {
    uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_FIFO_CONFIG0);

    CLEARBITS(reg_val, 0b1111111, 0);
    SETBITS(reg_val, threshold, 0b1111111, 0);

    S2LP_WriteRegister(handle, S2LP_REG_FIFO_CONFIG0, reg_val);
}

void S2LP_TX_SetRetransmissionTries(S2LP_Handle* handle, uint8_t tries) {
    if (tries > 15) {
        return;
    }

    uint8_t reg_val = S2LP_ReadRegister(handle, S2LP_REG_PROTOCOL0);

    CLEARBITS(reg_val, 0xF, 4);
    SETBITS(reg_val, tries, 0xF, 4);

    S2LP_WriteRegister(handle, S2LP_REG_PROTOCOL0, reg_val);
}

uint8_t S2LP_TX_GetPowerRampStepLength(S2LP_Handle* handle) {
    uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PA_POWER0);
    return (uint8_t) GETBITS(reg_val, 0b11, 3);
}

uint8_t S2LP_TX_GetPowerRampStepMax(S2LP_Handle* handle) {
    uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PA_POWER0);
    return (uint8_t) GETBITS(reg_val, 0b111, 0);
}

bool S2LP_TX_GetRampingState(S2LP_Handle* handle) {
    uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PA_POWER0);
    return (bool) GETBIT(reg_val, 5);
}

bool S2LP_TX_GetMaxPowerState(S2LP_Handle* handle) {
    uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PA_POWER0);
    return (bool) GETBIT(reg_val, 6);
}

bool S2LP_TX_GetRampingInterpolationState(S2LP_Handle* handle) {
    uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_MOD1);
    return GETBIT(reg_val, 7);
}

S2LP_TX_Source S2LP_TX_GetDataSource(S2LP_Handle* handle) {
    uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PCKTCTRL1);
    return (S2LP_TX_Source) GETBITS(reg_val, 0b11, 2);
}

uint8_t S2LP_TX_GetFIFOAlmostFullThreshold(S2LP_Handle* handle) {
    uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_FIFO_CONFIG1);
    return (uint8_t) GETBITS(reg_val, 0b1111111, 0);
}

uint8_t S2LP_TX_GetFIFOAlmostEmptyThreshold(S2LP_Handle* handle) {
    uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_FIFO_CONFIG0);
    return (uint8_t) GETBITS(reg_val, 0b1111111, 0);
}

uint8_t S2LP_TX_GetFIFOCount(S2LP_Handle* handle) {
    return S2LP_ReadRegister(handle, S2LP_REG_TX_FIFO_STATUS);
}

uint8_t S2LP_TX_GetRetransmissionTries(S2LP_Handle* handle) {
    uint8_t const reg_val = S2LP_ReadRegister(handle, S2LP_REG_PROTOCOL0);
    return (uint8_t) GETBITS(reg_val, 0xF, 4);
}
