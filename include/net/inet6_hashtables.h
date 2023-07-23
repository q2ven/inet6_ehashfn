#ifndef _INET6_HASHTABLES_H
#define _INET6_HASHTABLES_H

#include <linux/jhash.h>

static inline unsigned int __inet6_ehashfn(const u32 lhash,
					   const u16 lport,
					   const u32 fhash,
					   const __be16 fport,
					   const u32 initval)
{
	const u32 ports = (((u32)lport) << 16) | (__force u32)fport;

	return jhash_3words(lhash, fhash, ports, initval);
}

#endif
