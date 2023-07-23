#ifndef _LINUX_SIPHASH_H
#define _LINUX_SIPHASH_H

typedef struct {
	u64 key[2];
} siphash_key_t;

typedef struct {
	unsigned long key[2];
} hsiphash_key_t;

/*
 * These macros expose the raw SipHash and HalfSipHash permutations.
 * Do not use them directly! If you think you have a use for them,
 * be sure to CC the maintainer of this file explaining why.
 */

#define SIPHASH_PERMUTATION(a, b, c, d) ( \
	(a) += (b), (b) = rol64((b), 13), (b) ^= (a), (a) = rol64((a), 32), \
	(c) += (d), (d) = rol64((d), 16), (d) ^= (c), \
	(a) += (d), (d) = rol64((d), 21), (d) ^= (a), \
	(c) += (b), (b) = rol64((b), 17), (b) ^= (c), (c) = rol64((c), 32))

#define SIPHASH_CONST_0 0x736f6d6570736575ULL
#define SIPHASH_CONST_1 0x646f72616e646f6dULL
#define SIPHASH_CONST_2 0x6c7967656e657261ULL
#define SIPHASH_CONST_3 0x7465646279746573ULL

#define HSIPHASH_PERMUTATION(a, b, c, d) ( \
	(a) += (b), (b) = rol32((b), 5), (b) ^= (a), (a) = rol32((a), 16), \
	(c) += (d), (d) = rol32((d), 8), (d) ^= (c), \
	(a) += (d), (d) = rol32((d), 7), (d) ^= (a), \
	(c) += (b), (b) = rol32((b), 13), (b) ^= (c), (c) = rol32((c), 16))

#define HSIPHASH_CONST_0 0U
#define HSIPHASH_CONST_1 0U
#define HSIPHASH_CONST_2 0x6c796765U
#define HSIPHASH_CONST_3 0x74656462U

#endif
