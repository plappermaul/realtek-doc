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
 * Purpose : Definition those public NIC(Network Interface Controller) APIs and
 *           its data type in the SDK.
 *
 * Feature : Just dispatch information to Multiplex layer
 *
 */

/*
 * Include Files
 */
#include <hal/chipdef/chip.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <private/drv/l2ntfy/l2ntfy_mapper.h>
#include <common/debug/rt_log.h>
#include <hwp/hw_profile.h>

#if (defined(CONFIG_SDK_DRIVER_L2NTFY_R8390) || defined(CONFIG_SDK_DRIVER_L2NTFY_R8390_MODULE))
    #include <private/drv/l2ntfy/l2ntfy_rtl8390.h>
#endif

#if (defined(CONFIG_SDK_DRIVER_L2NTFY_R9300) || defined(CONFIG_SDK_DRIVER_L2NTFY_R9300_MODULE))
    #include <private/drv/l2ntfy/l2ntfy_rtl9300.h>
#endif

#if (defined(CONFIG_SDK_DRIVER_L2NTFY_R9310) || defined(CONFIG_SDK_DRIVER_L2NTFY_R9310_MODULE))
    #include <private/drv/l2ntfy/l2ntfy_rtl9310.h>
#endif


/*
 * Symbol Definition
 */
#define L2NTFY_DB_SIZE     (sizeof(l2ntfy_db)/sizeof(cid_prefix_group_t))

/*
 * Data Declaration
 */
const static cid_prefix_group_t l2ntfy_db[] =
{
#if defined(CONFIG_SDK_DRIVER_L2NTFY_R8390)
    {RTL8390_FAMILY_ID, L2NTFY_R8390},
    {RTL8350_FAMILY_ID, L2NTFY_R8390},
#endif
#if (defined(CONFIG_SDK_DRIVER_L2NTFY_R9300) || defined(CONFIG_SDK_DRIVER_NOTIF_R9300_MODULE))
    {RTL9300_FAMILY_ID, L2NTFY_R9300},
#endif
#if defined(CONFIG_SDK_DRIVER_L2NTFY_R9310)
    {RTL9310_FAMILY_ID, L2NTFY_R9310},
#endif
};

l2ntfy_mapper_operation_t l2ntfy_ops[L2NTFY_CTRL_END] =
{
#if defined(CONFIG_SDK_DRIVER_L2NTFY_R8390) && !defined(__MODEL_USER__)
    {   /* NOTIF_R8390 */
        .l2ntfy_init = r8390_l2ntfy_init,
        .l2ntfy_enable_get = r8390_l2ntfy_enable_get,
        .l2ntfy_enable_set = r8390_l2ntfy_enable_set,
        .l2ntfy_backPressureThresh_get = r8390_l2ntfy_backPressureThresh_get,
        .l2ntfy_backPressureThresh_set = r8390_l2ntfy_backPressureThresh_set,
        .l2ntfy_notificationEventEnable_get = r8390_l2ntfy_notificationEventEnable_get,
        .l2ntfy_notificationEventEnable_set = r8390_l2ntfy_notificationEventEnable_set,
        .l2ntfy_fifoEmptyStatus_get = r8390_l2ntfy_fifoEmptyStatus_get,

        .l2ntfy_dst_get  = r8390_l2ntfy_dst_get,
        .l2ntfy_dst_set  = r8390_l2ntfy_dst_set,
        .l2ntfy_reset  = r8390_l2ntfy_reset,
        .l2ntfy_iTag_get  = r8390_l2ntfy_iTag_get,
        .l2ntfy_iTag_set  = r8390_l2ntfy_iTag_set,
        .l2ntfy_magicNum_get  = r8390_l2ntfy_magicNum_get,
        .l2ntfy_magicNum_set  = r8390_l2ntfy_magicNum_set,
        .l2ntfy_macAddr_get  = r8390_l2ntfy_macAddr_get,
        .l2ntfy_macAddr_set  = r8390_l2ntfy_macAddr_set,
        .l2ntfy_maxEvent_get  = r8390_l2ntfy_maxEvent_get,
        .l2ntfy_maxEvent_set  = r8390_l2ntfy_maxEvent_set,
        .l2ntfy_timeout_get  = r8390_l2ntfy_timeout_get,
        .l2ntfy_timeout_set  = r8390_l2ntfy_timeout_set,
        .l2ntfy_rawToEvent_cnvt = r8390_l2ntfy_rawToEvent_cnvt,
        .l2ntfy_sizeInfo_get = r8390_l2ntfy_sizeInfo_get,
        .l2ntfy_entryLen_get = r8390_l2ntfy_entryLen_get,
        .l2ntfy_pktCpuQue_set = r8390_l2ntfy_pktCpuQue_set,
        .l2ntfy_l2LearningAge_disable = r8390_l2ntfy_l2LearningAge_disable,
        .l2ntfy_l2LearningAge_restore = r8390_l2ntfy_l2LearningAge_restore,
    },
#endif
#if defined(CONFIG_SDK_DRIVER_L2NTFY_R9300) && !defined(__MODEL_USER__)
    {   /* NOTIF_R9300 */
        .l2ntfy_init = r9300_l2ntfy_init,
        .l2ntfy_enable_get = r9300_l2ntfy_enable_get,
        .l2ntfy_enable_set = r9300_l2ntfy_enable_set,
        .l2ntfy_backPressureThresh_get = r9300_l2ntfy_backPressureThresh_get,
        .l2ntfy_backPressureThresh_set = r9300_l2ntfy_backPressureThresh_set,
        .l2ntfy_notificationEventEnable_get = r9300_l2ntfy_notificationEventEnable_get,
        .l2ntfy_notificationEventEnable_set = r9300_l2ntfy_notificationEventEnable_set,
        .l2ntfy_fifoEmptyStatus_get = r9300_l2ntfy_fifoEmptyStatus_get,

        .l2ntfy_dst_get  = r9300_l2ntfy_dst_get,
        .l2ntfy_dst_set  = r9300_l2ntfy_dst_set,
        .l2ntfy_reset  = r9300_l2ntfy_reset,
        .l2ntfy_iTag_get  = r9300_l2ntfy_iTag_get,
        .l2ntfy_iTag_set  = r9300_l2ntfy_iTag_set,
        .l2ntfy_magicNum_get  = r9300_l2ntfy_magicNum_get,
        .l2ntfy_magicNum_set  = r9300_l2ntfy_magicNum_set,
        .l2ntfy_macAddr_get  = r9300_l2ntfy_macAddr_get,
        .l2ntfy_macAddr_set  = r9300_l2ntfy_macAddr_set,
        .l2ntfy_maxEvent_get  = r9300_l2ntfy_maxEvent_get,
        .l2ntfy_maxEvent_set  = r9300_l2ntfy_maxEvent_set,
        .l2ntfy_timeout_get  = r9300_l2ntfy_timeout_get,
        .l2ntfy_timeout_set  = r9300_l2ntfy_timeout_set,
        .l2ntfy_rawToEvent_cnvt = r9300_l2ntfy_rawToEvent_cnvt,
        .l2ntfy_sizeInfo_get = r9300_l2ntfy_sizeInfo_get,
        .l2ntfy_entryLen_get = r9300_l2ntfy_entryLen_get,
        .l2ntfy_pktCpuQue_set = r9300_l2ntfy_pktCpuQue_set,
        .l2ntfy_pktCpuQueBwCtrlEnable_set = r9300_l2ntfy_pktCpuQueBwCtrlEnable_set,
        .l2ntfy_pktCpuQueRate_set = r9300_l2ntfy_pktCpuQueRate_set,
        .l2ntfy_l2LearningAge_disable = r9300_l2ntfy_l2LearningAge_disable,
        .l2ntfy_l2LearningAge_restore = r9300_l2ntfy_l2LearningAge_restore,
    },
#endif
#if defined(CONFIG_SDK_DRIVER_L2NTFY_R9310) && !defined(__MODEL_USER__)
    {   /* NOTIF_R9310 */
        .l2ntfy_init = r9310_l2ntfy_init,
        .l2ntfy_enable_get = r9310_l2ntfy_enable_get,
        .l2ntfy_enable_set = r9310_l2ntfy_enable_set,
        .l2ntfy_backPressureThresh_get = r9310_l2ntfy_backPressureThresh_get,
        .l2ntfy_backPressureThresh_set = r9310_l2ntfy_backPressureThresh_set,
        .l2ntfy_notificationEventEnable_get = r9310_l2ntfy_notificationEventEnable_get,
        .l2ntfy_notificationEventEnable_set = r9310_l2ntfy_notificationEventEnable_set,
        .l2ntfy_fifoEmptyStatus_get = r9310_l2ntfy_fifoEmptyStatus_get,

        .l2ntfy_dst_get  = r9310_l2ntfy_dst_get,
        .l2ntfy_dst_set  = r9310_l2ntfy_dst_set,
        .l2ntfy_reset  = r9310_l2ntfy_reset,
        .l2ntfy_iTag_get  = r9310_l2ntfy_iTag_get,
        .l2ntfy_iTag_set  = r9310_l2ntfy_iTag_set,
        .l2ntfy_magicNum_get  = r9310_l2ntfy_magicNum_get,
        .l2ntfy_magicNum_set  = r9310_l2ntfy_magicNum_set,
        .l2ntfy_macAddr_get  = r9310_l2ntfy_macAddr_get,
        .l2ntfy_macAddr_set  = r9310_l2ntfy_macAddr_set,
        .l2ntfy_maxEvent_get  = r9310_l2ntfy_maxEvent_get,
        .l2ntfy_maxEvent_set  = r9310_l2ntfy_maxEvent_set,
        .l2ntfy_timeout_get  = r9310_l2ntfy_timeout_get,
        .l2ntfy_timeout_set  = r9310_l2ntfy_timeout_set,
        .l2ntfy_rawToEvent_cnvt = r9310_l2ntfy_rawToEvent_cnvt,
        .l2ntfy_sizeInfo_get = r9310_l2ntfy_sizeInfo_get,
        .l2ntfy_entryLen_get = r9310_l2ntfy_entryLen_get,
        .l2ntfy_pktCpuQue_set = r9310_l2ntfy_pktCpuQue_set,
        .l2ntfy_pktCpuQueBwCtrlEnable_set = r9310_l2ntfy_pktCpuQueBwCtrlEnable_set,
        .l2ntfy_pktCpuQueRate_set = r9310_l2ntfy_pktCpuQueRate_set,
        .l2ntfy_l2LearningAge_disable = r9310_l2ntfy_l2LearningAge_disable,
        .l2ntfy_l2LearningAge_restore = r9310_l2ntfy_l2LearningAge_restore,
        .l2ntfy_queueCnt_get = r9310_l2ntfy_queueCnt_get,
    },
#endif
};

uint32 l2ntfy_if[RTK_MAX_NUM_OF_UNIT] = {CID_GROUP_NONE};


/*
 * Function Declaration
 */

/* Function Name:
 *      l2ntfy_probe
 * Description:
 *      Probe l2ntfy module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
l2ntfy_probe(uint32 unit)
{
    uint32      i;
    uint32      cid;

    RT_INIT_MSG("  L2Ntfy probe (unit %u): ", unit);

    cid = HWP_CHIP_ID(unit);

    l2ntfy_if[unit] = CID_GROUP_NONE;
    for (i = 0; i < L2NTFY_DB_SIZE; i++)
    {
        if (CID_PREFIX_MATCH(l2ntfy_db[i], cid))
        {
            l2ntfy_if[unit] = l2ntfy_db[i].gid;
            RT_INIT_MSG("(found)\n");
            return RT_ERR_OK;
        }
    }

    RT_INIT_MSG("(Not found)\n");
    return RT_ERR_CHIP_NOT_FOUND;

}

