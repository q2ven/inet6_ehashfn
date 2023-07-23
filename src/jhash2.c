#include <common.h>

/* include/net/ipv6.h */
static inline u32 __ipv6_addr_jhash(const struct in6_addr *a, const u32 initval)
{
	return jhash2((__force const u32 *)a->s6_addr32,
		      ARRAY_SIZE(a->s6_addr32), initval);
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
