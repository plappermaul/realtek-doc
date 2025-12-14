/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * Purpose : Definition the basic types in the SDK.
 *
 * Feature : type definition
 *
 */

#ifndef __COMMON_TYPE_H__
#define __COMMON_TYPE_H__

/*
 * Symbol Definition
 */

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif

#define RTK_MAX_NUM_OF_UNIT                         1
#define RTK_MAX_UNIT_ID                             (RTK_MAX_NUM_OF_UNIT - 1)

/*
 * Data Type Declaration
 */
#ifndef _RTL_TYPES_H
#ifndef uint64
#define uint64 unsigned long long
#endif
#ifndef int64
#define int64 signed long long
#endif
#ifndef uint32
#define uint32 unsigned int
#endif
#ifndef int32
#define int32 signed int
#endif
#ifndef uint16
#define uint16 unsigned short
#endif
#ifndef int16
#define int16 signed short
#endif
#ifndef uint8
#define uint8 unsigned char
#endif
#ifndef int8
#define int8 signed char
#endif
#ifndef ipaddr_t
#define ipaddr_t uint32 /* ipv4 address type */
#endif
#endif

/* configuration mode type */
typedef enum rtk_enable_e
{
    DISABLED = 0,
    ENABLED,
    RTK_ENABLE_END
} rtk_enable_t;

/* initial state of module */
typedef enum init_state_e
{
    INIT_NOT_COMPLETED = 0,
    INIT_COMPLETED,
    INIT_STATE_END
} init_state_t;

/* ethernet address type */
typedef struct  rtk_mac_s
{
    uint8 octet[ETHER_ADDR_LEN];
} rtk_mac_t;

#if defined(CONFIG_COMMON_RT_API)
#define rt_enable_t rtk_enable_t
typedef struct  rt_mac_s
{
    uint8 octet[ETHER_ADDR_LEN];
} rt_mac_t;
#endif

/*
 * Macro Definition
 */

#endif /* __COMMON_TYPE_H__ */

