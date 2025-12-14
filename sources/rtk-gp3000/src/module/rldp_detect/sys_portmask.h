/*
 * Copyright (C) 2010 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 9753 $
 * $Date: 2010-05-21 14:57:08 +0800 (Fri, 21 May 2010) $
 *
 * Purpose : Define software system utility
 *
 * Feature : System utility definition
 *
 */

#ifndef __SYS_PORTMASK_H__
#define __SYS_PORTMASK_H__

/*
 * Include Files
 */
#include "sys_bitmap.h"

/*
 * Symbol Definition
 */


/*
 * Data Type Declaration
 */

/*
 * Macro Definition
 */

#define LOGIC_PORTMASK_SET_PORT(portmask, port) \
do{ \
    SYS_BITMAP_SET_BIT((portmask).bits, (port-1)); \
} while(0)

#define LOGIC_PORTMASK_CLEAR_PORT(portmask, port) \
do{ \
    SYS_BITMAP_CLEAR_BIT((portmask).bits, (port-1)); \
} while(0)

#define LOGIC_PORTMASK_SET_ALL(lPortmask) \
do{ \
    SYS_BITMAP_SET_ALL((lPortmask).bits, (HAL_GET_PON_PORT() + 1)); \
} while(0)

#define LOGIC_PORTMASK_CLEAR_ALL(lPortmask) \
do{ \
    SYS_BITMAP_CLEAR_ALL((lPortmask).bits, (HAL_GET_PON_PORT() + 1)); \
} while(0)

#define LOGIC_PORTMASK_IS_CLEAR(portmask, result) \
    SYS_BITMAP_IS_CLEAR((portmask).bits, (HAL_GET_PON_PORT() + 1), result)

#define LOGIC_PORTMASK_COPY(dst, src) \
    SYS_BITMAP_COPY((dst).bits, (src).bits, (HAL_GET_PON_PORT() + 1))

#define LOGIC_PORTMASK_ANDNOT(dst, portmask1, portmask2) \
    SYS_BITMAP_ANDNOT((dst).bits, (portmask1).bits, (portmask2).bits, (HAL_GET_PON_PORT() + 1))

#define LOGIC_PORTMASK_OR(dst, portmask1, portmask2) \
    SYS_BITMAP_OR((dst).bits, (portmask1).bits, (portmask2).bits, (HAL_GET_PON_PORT() + 1))

//#define FOR_EACH_LOGIC_PORT(port)  for((port) = LOGIC_PORT_START; (port) < igmpCtrl.sys_max_port_num - 1; (port++))
#define FOR_EACH_LOGIC_PORT(port)  for((port) = LOGIC_PORT_START; (port) < 1; (port++))

#define FOR_EACH_ETHER_PORT(port)  for((port) = dev_info.ether.min; (port) <= dev_info.ether.max; (port++))

#define FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, portmask) \
    FOR_EACH_LOGIC_PORT((port)) \
        if(IS_LOGIC_PORTMASK_PORTSET((portmask), (port)))

#define IS_LOGIC_PORTMASK_PORTSET(portmask, port) \
    (((port) <= (HAL_GET_PON_PORT() + 1)) ? SYS_BITMAP_IS_BITSET((portmask).bits, (port-1)) : 0)

#define IS_LOGIC_PORTMASK_CLEAR(portmask) \
({ \
    uint32  __ret;\
    SYS_BITMAP_IS_CLEAR(portmask.bits, (HAL_GET_PON_PORT() + 1), __ret);\
    __ret;\
})

#define IS_LOGIC_PORTMASK_EQUAL(portmask1, portmask2) \
({ \
    uint32  __ret;\
    SYS_BITMAP_IS_EQUAL(portmask1.bits, portmask2.bits, (HAL_GET_PON_PORT() + 1), __ret);\
    __ret;\
})

#endif /* __SYS_PORTMASK_H__ */

