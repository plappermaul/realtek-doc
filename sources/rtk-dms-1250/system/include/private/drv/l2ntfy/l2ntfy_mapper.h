/*
 * Copyright (C) 2015 Realtek Semiconductor Corp.
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
 * Purpose : Mapper Layer is used to seperate different kind of software or hardware platform
 *
 * Feature : Just dispatch information to Multiplex layer
 *
 */
#ifndef __DRV_L2NTFY_MAPPER_H__
#define __DRV_L2NTFY_MAPPER_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/error.h>
#include <drv/l2ntfy/l2ntfy.h>

/*
 * Symbol Definition
 */
typedef enum l2ntfy_controller_e
{
#if defined(CONFIG_SDK_DRIVER_L2NTFY_R8390)
    L2NTFY_R8390,
#endif
#if defined(CONFIG_SDK_DRIVER_L2NTFY_R9300)
    L2NTFY_R9300,
#endif
#if defined(CONFIG_SDK_DRIVER_L2NTFY_R9310)
    L2NTFY_R9310,
#endif
    L2NTFY_CTRL_END
} l2ntfy_controller_t;

typedef struct l2ntfy_mapper_operation_s
{
    int32   (*l2ntfy_init)(uint32);
    int32   (*l2ntfy_register)(uint32 , drv_l2ntfy_cb_f, drv_l2ntfy_runoutcb_f);
    int32   (*l2ntfy_unregister)(uint32);
    int32   (*l2ntfy_isr_handler)(uint32 , void *);
    int32   (*l2ntfy_bufRunout_handler)(uint32 , void *);
    int32   (*l2ntfy_localBufRunout_handler)(uint32 , void *);

    int32   (*l2ntfy_dst_get)(uint32, rtk_l2ntfy_dst_t *);
    int32   (*l2ntfy_dst_set)(uint32, rtk_l2ntfy_dst_t);
    int32   (*l2ntfy_reset)(uint32);
    int32   (*l2ntfy_iTag_get)(uint32, rtk_l2ntfy_iTagCfg_t *);
    int32   (*l2ntfy_iTag_set)(uint32, rtk_l2ntfy_iTagCfg_t *);
    int32   (*l2ntfy_magicNum_get)(uint32, uint32 *);
    int32   (*l2ntfy_magicNum_set)(uint32, uint32);
    int32   (*l2ntfy_macAddr_get)(uint32, rtk_l2ntfy_addrType_t, rtk_mac_t *);
    int32   (*l2ntfy_macAddr_set)(uint32, rtk_l2ntfy_addrType_t, rtk_mac_t *);
    int32   (*l2ntfy_maxEvent_get)(uint32, uint32 *);
    int32   (*l2ntfy_maxEvent_set)(uint32, uint32);
    int32   (*l2ntfy_timeout_get)(uint32, rtk_l2ntfy_mode_t, uint32 *);
    int32   (*l2ntfy_timeout_set)(uint32, rtk_l2ntfy_mode_t, uint32);
    int32   (*l2ntfy_rawToEvent_cnvt)(uint32, uint8 *, rtk_l2ntfy_eventEntry_t *);

    int32   (*l2ntfy_enable_get)(uint32, uint32 *);
    int32   (*l2ntfy_enable_set)(uint32, uint32);
    int32   (*l2ntfy_backPressureThresh_get)(uint32, uint32 *);
    int32   (*l2ntfy_backPressureThresh_set)(uint32, uint32);
    int32   (*l2ntfy_notificationEventEnable_get)(uint32, rtk_l2ntfy_event_t , rtk_enable_t *);
    int32   (*l2ntfy_notificationEventEnable_set)(uint32, rtk_l2ntfy_event_t , rtk_enable_t);
    int32   (*l2ntfy_fifoEmptyStatus_get)(uint32, uint32 *);
    int32   (*l2ntfy_counter_dump)(uint32);
    int32   (*l2ntfy_counter_clear)(uint32);
    int32   (*l2ntfy_sizeInfo_get)(uint32, uint32 * , uint32 *);
    int32   (*l2ntfy_entryLen_get)(uint32, uint32 *);
    int32   (*l2ntfy_pktCpuQue_set)(uint32, uint32);
    int32   (*l2ntfy_pktCpuQueBwCtrlEnable_set)(uint32, uint32, rtk_enable_t);
    int32   (*l2ntfy_pktCpuQueRate_set)(uint32, uint32, uint32);
    int32   (*l2ntfy_l2LearningAge_disable)(uint32, uint32);
    int32   (*l2ntfy_l2LearningAge_restore)(uint32, uint32);
    int32   (*l2ntfy_queueCnt_get)(uint32, uint32 *);

    int32   (*l2ntfy_dbg_get)(uint32, uint32 *);
    int32   (*l2ntfy_dbg_set)(uint32, uint32);
    int32   (*l2ntfy_event_dump)(uint32);

} l2ntfy_mapper_operation_t;

#define L2NTFY_CTRL(unit)      l2ntfy_ops[l2ntfy_if[unit]]
#define L2NTFY_CHK(unit)       (l2ntfy_if[unit] != CID_GROUP_NONE)

/*
 * Data Declaration
 */
extern l2ntfy_mapper_operation_t l2ntfy_ops[L2NTFY_CTRL_END];
extern uint32 l2ntfy_if[RTK_MAX_NUM_OF_UNIT];


/*
 * Macro Definition
 */

/*
 * Function Declaration
 */



#endif /* __DRV_L2NTFY_MAPPER_H__ */

