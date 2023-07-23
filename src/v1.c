#include <common.h>

/* include/net/ipv6.h */
static inline u32 __ipv6_addr_jhash(const struct in6_addr *a, const u32 initval)
{
	return jhash_3words((__force u32)a->s6_addr32[1],
			    (__force u32)a->s6_addr32[2],
			    (__force u32)a->s6_addr32[3],
			    initval);
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

	lhash = jhash_3words((__force u32)laddr->s6_addr32[3],
			     (((u32)lport) << 16) | (__force u32)fport,
			     (__force u32)faddr->s6_addr32[0],
			     ipv6_hash_secret);
	fhash = __ipv6_addr_jhash(faddr, ipv6_hash_secret);

	return __inet6_ehashfn(lhash, lport, fhash, fport,
			       inet6_ehash_secret + net_hash_mix(net));
}
