#ifndef __NET_NET_NAMESPACE_H
#define __NET_NET_NAMESPACE_H

struct net {
	/* Note that @hash_mix can be read millions times per second,
	 * it is critical that it is on a read_mostly cache line.
	 */
	u32			hash_mix;
};

#endif
