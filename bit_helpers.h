/*
 * bit_helpers.h
 *
 *  Created on: Aug 4, 2021
 *      Author: steelph0enix
 */

#ifndef BIT_HELPERS_H_
#define BIT_HELPERS_H_

#define GETBIT(value, pos) ((value & (1 << pos)) >> pos)
#define SETBIT(output, pos) (output|= (1 << pos))
#define CLEARBIT(value, pos) (value &= (~(1 << pos)))
#define GETBITS(value, mask, pos) ((value & (mask << pos)) >> pos)
#define SETBITS(output, arg, mask, pos) (output|= ((arg & mask) << pos))
#define CLEARBITS(value, mask, pos) (value &= (~(mask << pos)))

#endif /* BIT_HELPERS_H_ */
