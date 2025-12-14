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
 * Purpose : Definition the error number in the SDK.
 *
 * Feature : error definition
 *
 */

#ifndef __COMMON_ERROR_H__
#define __COMMON_ERROR_H__

/*
 * Include Files
 */
#include <common/type.h>

/*
 * Data Type Declaration
 */
typedef enum rt_error_common_e
{
    RT_ERR_FAILED = -1,                             /* General Error                                                                    */

    /* 0x0000xxxx for common error code */
    RT_ERR_OK = 0,                                  /* 0x00000000, OK                                                                   */
    RT_ERR_INPUT,                                   /* 0x00000001, invalid input parameter                                              */
    RT_ERR_UNIT_ID,                                 /* 0x00000002, invalid unit id                                                      */
    RT_ERR_PORT_ID,                                 /* 0x00000003, invalid port id                                                      */
    RT_ERR_PORT_MASK,                               /* 0x00000004, invalid port mask                                                    */
    RT_ERR_PORT_LINKDOWN,                           /* 0x00000005, link down port status                                                */
    RT_ERR_ENTRY_INDEX,                             /* 0x00000006, invalid entry index                                                  */
    RT_ERR_NULL_POINTER,                            /* 0x00000007, input parameter is null pointer                                      */
    RT_ERR_QUEUE_ID,                                /* 0x00000008, invalid queue id                                                     */
    RT_ERR_QUEUE_NUM,                               /* 0x00000009, invalid queue number                                                 */
    RT_ERR_BUSYWAIT_TIMEOUT,                        /* 0x0000000a, busy watting time out                                                */
    RT_ERR_MAC,                                     /* 0x0000000b, invalid mac address                                                  */
    RT_ERR_OUT_OF_RANGE,                            /* 0x0000000c, input parameter out of range                                         */
    RT_ERR_CHIP_NOT_SUPPORTED,                      /* 0x0000000d, functions not supported by this chip model                           */
    RT_ERR_SMI,                                     /* 0x0000000e, SMI error                                                            */
    RT_ERR_NOT_INIT,                                /* 0x0000000f, The module is not initial                                            */
    RT_ERR_CHIP_NOT_FOUND,                          /* 0x00000010, The chip can not found                                               */
    RT_ERR_NOT_ALLOWED,                             /* 0x00000011, actions not allowed by the function                                  */
    RT_ERR_DRIVER_NOT_FOUND,                        /* 0x00000012, The driver can not found                                             */
    RT_ERR_SEM_LOCK_FAILED,                         /* 0x00000013, Failed to lock semaphore                                             */
    RT_ERR_SEM_UNLOCK_FAILED,                       /* 0x00000014, Failed to unlock semaphore                                           */
    RT_ERR_THREAD_EXIST,                            /* 0x00000015, Thread exist                                                         */
    RT_ERR_THREAD_CREATE_FAILED,                    /* 0x00000016, Thread create fail                                                   */
    RT_ERR_FWD_ACTION,                              /* 0x00000017, Invalid forwarding Action                                            */
    RT_ERR_IPV4_ADDRESS,                            /* 0x00000018, Invalid IPv4 address                                                 */
    RT_ERR_IPV6_ADDRESS,                            /* 0x00000019, Invalid IPv6 address                                                 */
    RT_ERR_PRIORITY,                                /* 0x0000001a, Invalid Priority value                                               */
    RT_ERR_FID,                                     /* 0x0000001b, invalid fid                                                          */
    RT_ERR_ENTRY_NOTFOUND,                          /* 0x0000001c, specified entry not found                                            */
    RT_ERR_DROP_PRECEDENCE,                         /* 0x0000001d, invalid drop precedence                                              */
    RT_ERR_NOT_FINISH,                              /* 0x0000001e, Action not finish, still need to wait                                */
    RT_ERR_TIMEOUT,                                 /* 0x0000001f, Time out                                                             */
    RT_ERR_REG_ARRAY_INDEX_1,                       /* 0x00000020, invalid index 1 of register array                                    */
    RT_ERR_REG_ARRAY_INDEX_2,                       /* 0x00000021, invalid index 2 of register array                                    */
    RT_ERR_ETHER_TYPE,                              /* 0x00000022, invalid ether type                                                   */
    RT_ERR_ENTRY_FULL,                              /* 0x00000023, entry is full                                                        */
    RT_ERR_EFID,                                    /* 0x00000024, invalid enhanced fid                                                 */
    RT_ERR_FEATURE_NOT_SUPPORTED,                   /* 0x00000025, this feature not support                                             */
    RT_ERR_ENTRY_EXIST,                             /* 0x00000026, existed entry                                                        */
    RT_ERR_EEPROM_NACK,                             /* 0x00000027, EEPROM NACK                                                          */
    RT_ERR_PORT_LINKUP,                             /* 0x00000028, link up port status                                                  */
    RT_ERR_COMMON_END = 0xFFFF                      /* The symbol is the latest symbol of common error                                  */
} rt_error_common_t;

/*
 * Macro Definition
 */
#if defined(CONFIG_CONFIG_LUNA_G3_SERIES) && defined(CONFIG_SDK_KERNEL_LINUX)
extern uint32 ca_rtk_debug;
#define RT_PARAM_CHK(expr, errCode)\
do {\
    if ((int32)(expr)) {\
        if(ca_rtk_debug & 0x1) {printk("\033[1;31m %s %d RT_PARAM_CHK errCode=%d\033[m\n",__FUNCTION__,__LINE__,errCode);} \
        if(ca_rtk_debug & 0x2) {dump_stack();} \
        return errCode; \
    }\
} while (0)

#define RT_ERR_CHK(op, ret)\
do {\
    if ((ret = (op)) != RT_ERR_OK){\
        if(ca_rtk_debug & 0x20) {printk("\033[1;31m %s %d RT_ERR_CHK ret=%d\033[m\n",__FUNCTION__,__LINE__,ret);} \
        return ret; \
    }\
} while(0)

#else
#define RT_PARAM_CHK(expr, errCode)\
do {\
    if ((int32)(expr)) {\
        return errCode; \
    }\
} while (0)

#define RT_ERR_CHK(op, ret)\
do {\
    if ((ret = (op)) != RT_ERR_OK)\
        return ret;\
} while(0)

#endif

#define RT_INIT_CHK(state)\
do {\
    if (INIT_COMPLETED != (state)) {\
        return RT_ERR_NOT_INIT;\
    }\
} while (0)

#define RT_ERR_HDL(op, errHandle, ret)\
do {\
    if ((ret = (op)) != RT_ERR_OK)\
        goto errHandle;\
} while(0)

#endif /* __COMMON_ERROR_H__ */

