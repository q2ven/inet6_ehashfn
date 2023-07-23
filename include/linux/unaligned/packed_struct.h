#ifndef _LINUX_UNALIGNED_PACKED_STRUCT_H
#define _LINUX_UNALIGNED_PACKED_STRUCT_H

#include <linux/compiler_attributes.h>

struct __una_u32 { u32 x; } __packed;

static inline u32 __get_unaligned_cpu32(const void *p)
{
	const struct __una_u32 *ptr = (const struct __una_u32 *)p;
	return ptr->x;
}

#endif /* _LINUX_UNALIGNED_PACKED_STRUCT_H */
