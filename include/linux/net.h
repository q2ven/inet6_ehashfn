#ifndef _LINUX_NET_H
#define _LINUX_NET_H

#define net_get_random_once(buf, nbytes)			\
	get_random_once((buf), (nbytes))

#endif
