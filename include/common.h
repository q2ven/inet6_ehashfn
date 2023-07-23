#include <stdint.h>
#include <stdlib.h>

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef u8		__u8;
typedef u32		__u32;
typedef u64		__u64;

typedef u16		__be16;
typedef u32		__be32;

#define __force
#define __read_mostly

#define get_random_once(buf, nbytes)		\
	if (!*buf) *buf = rand()

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#include <linux/in6.h>
#include <linux/net.h>
#include <linux/siphash.h>
#include <net/net_namespace.h>
#include <net/netns/hash.h>
#include <net/inet6_hashtables.h>

/* From lib/siphash.c.
 *
 * Note hsiphash_4u32() and siphash_2u64 are NOT inlined
 * in kernel, but here we inline them for llvm-mca.
 */

#define SIPROUND SIPHASH_PERMUTATION(v0, v1, v2, v3)

#define PREAMBLE(len) \
	u64 v0 = SIPHASH_CONST_0; \
	u64 v1 = SIPHASH_CONST_1; \
	u64 v2 = SIPHASH_CONST_2; \
	u64 v3 = SIPHASH_CONST_3; \
	u64 b = ((u64)(len)) << 56; \
	v3 ^= key->key[1]; \
	v2 ^= key->key[0]; \
	v1 ^= key->key[1]; \
	v0 ^= key->key[0];

#define POSTAMBLE \
	v3 ^= b; \
	SIPROUND; \
	SIPROUND; \
	v0 ^= b; \
	v2 ^= 0xff; \
	SIPROUND; \
	SIPROUND; \
	SIPROUND; \
	SIPROUND; \
	return (v0 ^ v1) ^ (v2 ^ v3);

/**
 * siphash_2u64 - compute 64-bit siphash PRF value of 2 u64
 * @first: first u64
 * @second: second u64
 * @key: the siphash key
 */
static inline u64 siphash_2u64(const u64 first, const u64 second, const siphash_key_t *key)
{
	PREAMBLE(16)
	v3 ^= first;
	SIPROUND;
	SIPROUND;
	v0 ^= first;
	v3 ^= second;
	SIPROUND;
	SIPROUND;
	v0 ^= second;
	POSTAMBLE
}

/* Note that on 64-bit, we make HalfSipHash1-3 actually be SipHash1-3, for
 * performance reasons. On 32-bit, below, we actually implement HalfSipHash1-3.
 */

#define HSIPROUND SIPROUND
#define HPREAMBLE(len) PREAMBLE(len)
#define HPOSTAMBLE \
	v3 ^= b; \
	HSIPROUND; \
	v0 ^= b; \
	v2 ^= 0xff; \
	HSIPROUND; \
	HSIPROUND; \
	HSIPROUND; \
	return (v0 ^ v1) ^ (v2 ^ v3);

/**
 * hsiphash_4u32 - compute 32-bit hsiphash PRF value of 4 u32
 * @first: first u32
 * @second: second u32
 * @third: third u32
 * @forth: forth u32
 * @key: the hsiphash key
 */
static inline u32 hsiphash_4u32(const u32 first, const u32 second, const u32 third,
				const u32 forth, const hsiphash_key_t *key)
{
	u64 combined = (u64)second << 32 | first;
	HPREAMBLE(16)
	v3 ^= combined;
	HSIPROUND;
	v0 ^= combined;
	combined = (u64)forth << 32 | third;
	v3 ^= combined;
	HSIPROUND;
	v0 ^= combined;
	HPOSTAMBLE
}

u32 inet6_ehashfn(const struct net *net,
		  const struct in6_addr *laddr, const u16 lport,
		  const struct in6_addr *faddr, const __be16 fport);
