#include <common.h>

/* lib/siphash_kunit.c */
static const siphash_key_t test_key_siphash =
	{{ 0x0706050403020100ULL, 0x0f0e0d0c0b0a0908ULL }};

/* include/net/ipv6.h */
static inline u32 __ipv6_addr_jhash(const struct in6_addr *a, const u32 initval)
{
	return siphash_2u64(*(u64 *)a->s6_addr32,
			    *(u64 *)(a->s6_addr32 + 2),
			    &test_key_siphash);
}

/* net/ipv6/inet6_hashtables.c */
u32 inet6_ehashfn(const struct net *net,
		  const struct in6_addr *laddr, const u16 lport,
		  const struct in6_addr *faddr, const __be16 fport)
{
	static u32 inet6_ehash_secret __read_mostly;
	static u32 ipv6_hash_secret __read_mostly;

	u32 lhash, fhash;

	net_get_random_once(&inet6_ehash_secret, sizeof(inet6_ehash_secret));
	net_get_random_once(&ipv6_hash_secret, sizeof(ipv6_hash_secret));

	lhash = (__force u32)laddr->s6_addr32[3];
	fhash = __ipv6_addr_jhash(faddr, ipv6_hash_secret);

	return __inet6_ehashfn(lhash, lport, fhash, fport,
			       inet6_ehash_secret + net_hash_mix(net));
}
