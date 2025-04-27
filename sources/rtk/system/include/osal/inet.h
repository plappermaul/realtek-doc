/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : INET relative API
 *
 */

#ifndef __OSAL_INET_H__
#define __OSAL_INET_H__

/*
 * Include Files
 */
#ifdef __BOOTLOADER__

#else
#include <common/rt_autoconf.h>
#endif
#include <common/type.h>

/*
 * Symbol Definition
 */
#define swapl32(x)\
        ((((x) & 0xff000000U) >> 24) | \
         (((x) & 0x00ff0000U) >>  8) | \
         (((x) & 0x0000ff00U) <<  8) | \
         (((x) & 0x000000ffU) << 24))
#define swaps16(x)        \
        ((((x) & 0xff00) >> 8) | \
         (((x) & 0x00ff) << 8))

/* Definition osal_ntohs, osal_ntohl, osal_htons and osal_htonl macro */
#if defined(CONFIG_SDK_ENDIAN_LITTLE)
    #define osal_ntohs(x)   (swaps16(x))
    #define osal_ntohl(x)   (swapl32(x))
    #define osal_htons(x)   (swaps16(x))
    #define osal_htonl(x)   (swapl32(x))
#else
    #define osal_ntohs(x)   (x)
    #define osal_ntohl(x)   (x)
    #define osal_htons(x)   (x)
    #define osal_htonl(x)   (x)
#endif

#endif /* __OSAL_INET_H__ */

