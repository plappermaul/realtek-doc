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
 * Purpose : Definition the common utilities in the SDK.
 *
 * Feature : type definition
 *
 */

#ifndef __COMMON_UTIL_H__
#define __COMMON_UTIL_H__

/*
 * Symbol Definition
 */

/*
 * Data Type Declaration
 */

/*
 * Macro Definition
 */
#define SWAP_L32(x)\
        ((((x) & 0xff000000U) >> 24) | \
         (((x) & 0x00ff0000U) >>  8) | \
         (((x) & 0x0000ff00U) <<  8) | \
         (((x) & 0x000000ffU) << 24))

#define SWAP_S16(x)\
        ((((x) & 0xff00U) >> 8) | \
         (((x) & 0x00ffU) << 8))

#endif /* __COMMON_UTIL_H__ */

