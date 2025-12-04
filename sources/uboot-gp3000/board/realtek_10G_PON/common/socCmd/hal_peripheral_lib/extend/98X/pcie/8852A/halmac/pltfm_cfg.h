/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_PLTFM_H_
#define _MAC_AX_PLTFM_H_

/* Include header file which contain the following definitions, */
/* or modify this file to meet your platform */
#include "general.h"

#ifndef NULL
#define NULL	((void *)0)
#endif

typedef unsigned char	u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef signed char s8;
typedef int16_t		s16;
typedef int32_t		s32;

typedef u16 __le16;
typedef u32 __le32;
typedef u16 __be16;
typedef u32 __be32;

typedef	CRITICAL_SECTION	mac_ax_mutex;

#define ARRAY_SIZE(arr)		_ARRAY_SIZE(arr)

/* 1: the platform is Little Endian. */
/* 0: the platform is Big Endian. */
#define MAC_AX_IS_LITTLE_ENDIAN	1

/* 1: enable MAC debug messages */
/* 0: disable MAC debug messages */
#define MAC_AX_DBG_MSG_EN	1

/* Set debug message level */
#define MAC_AX_MSG_LEVEL_TRACE		3
#define MAC_AX_MSG_LEVEL_WARNING	2
#define MAC_AX_MSG_LEVEL_ERR		1
#define MAC_AX_MSG_LEVEL		MAC_AX_MSG_LEVEL_TRACE

#define SET_CLR_WORD(_w, _v, _f)                                               \
				(((_w) & ~((_f##_MSK) << (_f##_SH))) |         \
				(((_v) & (_f##_MSK)) << (_f##_SH)))
#define SET_WORD(_v, _f)	(((_v) & (_f##_MSK)) << (_f##_SH))
#define GET_FIELD(_w, _f)         (((_w) >> (_f##_SH)) & (_f##_MSK))

#define SWAP32(x)                                                              \
	((u32)((((u32)(x) & (u32)0x000000ff) << 24) |                          \
	       (((u32)(x) & (u32)0x0000ff00) << 8) |                           \
	       (((u32)(x) & (u32)0x00ff0000) >> 8) |                           \
	       (((u32)(x) & (u32)0xff000000) >> 24)))

#define SWAP16(x)                                                              \
	((u16)((((u16)(x) & (u16)0x00ff) << 8) |                               \
	       (((u16)(x) & (u16)0xff00) >> 8)))

#if MAC_AX_IS_LITTLE_ENDIAN
#define cpu_to_le32(x)              ((u32)(x))
#define le32_to_cpu(x)              ((u32)(x))
#define cpu_to_le16(x)              ((u16)(x))
#define le16_to_cpu(x)              ((u16)(x))
#define cpu_to_be32(x)              SWAP32((x))
#define be32_to_cpu(x)              SWAP32((x))
#define cpu_to_be16(x)              SWAP16((x))
#define be16_to_cpu(x)              SWAP16((x))
#else
#define cpu_to_le32(x)              SWAP32((x))
#define le32_to_cpu(x)              SWAP32((x))
#define cpu_to_le16(x)              SWAP16((x))
#define le16_to_cpu(x)              SWAP16((x))
#define cpu_to_be32(x)              ((u32)(x))
#define be32_to_cpu(x)              ((u32)(x))
#define cpu_to_be16(x)              ((u16)(x))
#define be16_to_cpu(x)              ((u16)(x))
#endif

#endif
