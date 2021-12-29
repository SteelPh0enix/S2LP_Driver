/*
 * bit_helpers.h
 *
 *  Created on: Nov 4, 2021
 *      Author: steelph0enix
 */

#ifndef INC_BIT_HELPERS_H_
#define INC_BIT_HELPERS_H_

#include <inttypes.h>

#define GETBIT(value, pos) ((value & (1u << pos)) >> pos)
#define SETBIT(output, pos) (output|= (1u << pos))
#define CLEARBIT(value, pos) (value &= (~(1u << pos)))
#define GETBITS(value, mask, pos) ((value & (mask << pos)) >> pos)
#define SETBITS(output, arg, mask, pos) (output|= ((arg & mask) << pos))
#define CLEARBITS(value, mask, pos) (value &= (~(mask << pos)))

#define GETBYTE(value, pos) GETBITS(value, 0xFF, (pos*8u))
#define SETBYTE(output, value, pos) SETBITS(output, value, 0xFF, (pos*8u))
#define CLEARBYTE(value, pos) CLEARBITS(value, 0xFF, (pos*8u))

// BYTEARRAY_TO_XBIT_Y - X is the amount of bits,
// Y is LE for little-endian (LSB is first) and BE for big-endian (MSB if first)
// Endianess is relative to the order of bytes in array.
#define BYTEARRAY_TO_16BIT_VALUE_BE(array) ((((uint16_t)array[0]) << 8u) | array[1])
#define BYTEARRAY_TO_16BIT_VALUE_LE(array) ((((uint16_t)array[1]) << 8u) | array[0])

#define BYTEARRAY_TO_32BIT_VALUE_BE(array) ((((uint32_t)array[0]) << 24u) | \
									  (((uint32_t)array[1]) << 16u) | \
									  (((uint32_t)array[2]) << 8u)  | \
									              array[3])

#define BYTEARRAY_TO_32BIT_VALUE_LE(array) ((((uint32_t)array[3]) << 24u) | \
									  (((uint32_t)array[2]) << 16u) | \
									  (((uint32_t)array[1]) << 8u)  | \
									              array[0])

// Same markings as above, works the other way around
#define VALUE_TO_16BIT_BYTEARRAY_BE(value, array) array[0] = GETBYTE(value, 1); \
												  array[1] = GETBYTE(value, 0)

#define VALUE_TO_16BIT_BYTEARRAY_LE(value, array) array[1] = GETBYTE(value, 1); \
												  array[0] = GETBYTE(value, 0)

#define VALUE_TO_32BIT_BYTEARRAY_BE(value, array) array[0] = GETBYTE(value, 3); \
												  array[1] = GETBYTE(value, 2); \
												  array[2] = GETBYTE(value, 1); \
												  array[3] = GETBYTE(value, 0)

#define VALUE_TO_32BIT_BYTEARRAY_LE(value, array) array[0] = GETBYTE(value, 0); \
												  array[1] = GETBYTE(value, 1); \
												  array[2] = GETBYTE(value, 2); \
												  array[3] = GETBYTE(value, 3)

#define CHANGE_VALUE_RANGE(val, val_range_min, val_range_max, new_range_min, new_range_max) \
		((((val - val_range_min) * (new_range_max - new_range_min)) / (val_range_max - val_range_min)) + new_range_min)

#endif /* INC_BIT_HELPERS_H_ */
