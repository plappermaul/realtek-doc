/* Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 93380 $
 * $Date: 2018-11-08 17:49:37 +0800 (Thu, 08 Nov 2018) $
 *
 * Purpose : Definition of Interrupt control API
 *
 * Feature : The file includes the following modules
 *           (1) Interrupt
 *
 */

 /*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <osal/isr.h>
#include <drv/intr/intr.h>
#include <private/drv/intr/intr_mapper.h>
#if defined(CONFIG_SDK_RTL8390)
#include <private/drv/intr/intr_rtl8390.h>
#endif
#if defined(CONFIG_SDK_RTL8380)
#include <private/drv/intr/intr_rtl8380.h>
#endif
#if defined(CONFIG_SDK_RTL9300)
#include <private/drv/intr/intr_rtl9300.h>
#endif
#if defined(CONFIG_SDK_RTL9310)
#include <private/drv/intr/intr_rtl9310.h>
#endif

#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <hwp/hw_profile.h>



/*
 * Symbol Definition
 */
#define INTR_DB_SIZE        (sizeof(intr_db)/sizeof(cid_prefix_group_t))



/*
 * Data Declaration
 */
const static cid_prefix_group_t intr_db[] =
{
#if defined(CONFIG_SDK_RTL9310)
    {RTL9310_FAMILY_ID, INTR_R9310},
#endif
#if defined(CONFIG_SDK_RTL9300)
    {RTL9300_FAMILY_ID, INTR_R9300},
#endif
#if defined(CONFIG_SDK_RTL8390)
    {RTL8390_FAMILY_ID, INTR_R8390},
    {RTL8350_FAMILY_ID, INTR_R8390},
#endif
#if defined(CONFIG_SDK_RTL8380)
    {RTL8380_FAMILY_ID, INTR_R8380},
    {RTL8330_FAMILY_ID, INTR_R8380},
#endif
};


drv_intr_mapper_operation_t intr_ops[INTR_CTRL_END] = {
#if defined(CONFIG_SDK_RTL9310)
    [INTR_R9310] =
    {
        .swcoreIsrSts_get = rtl9310_intr_swcoreIsrSts_get,
        .swcoreImrEnable_set = rtl9310_intr_swcoreImrEnable_set,
        .swcoreSts_get = rtl9310_intr_swcoreSts_get,
    },
#endif
#if defined(CONFIG_SDK_RTL9300)
    [INTR_R9300] =
    {
        .swcoreIsrSts_get = rtl9300_intr_swcoreIsrSts_get,
        .swcoreImrEnable_set = rtl9300_intr_swcoreImrEnable_set,
        .swcoreSts_get = rtl9300_intr_swcoreSts_get,
    },
#endif
#if defined(CONFIG_SDK_RTL8390)
    [INTR_R8390] =
    {
        .swcoreIsrSts_get = rtl8390_intr_swcoreIsrSts_get,
        .swcoreImrEnable_set = rtl8390_intr_swcoreImrEnable_set,
        .swcoreSts_get = rtl8390_intr_swcoreSts_get,
    },
#endif
#if defined(CONFIG_SDK_RTL8380)
    [INTR_R8380] =
    {
        .swcoreIsrSts_get = rtl8380_intr_swcoreIsrSts_get,
        .swcoreImrEnable_set = rtl8380_intr_swcoreImrEnable_set,
        .swcoreSts_get = rtl8380_intr_swcoreSts_get,
    },
#endif
};

uint32 intr_if[RTK_MAX_NUM_OF_UNIT] = {CID_GROUP_NONE};



/*
 * Function Declaration
 */

/* Function Name:
 *      intr_probe
 * Description:
 *      Probe INTR module of the specified device.
 * Input:
 *      None
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
 *      [SDK_3.7.0]
 *          New added function.
 */
int32
intr_probe(uint32 unit)
{
    uint32      i;
    uint32      cid;
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    RT_INIT_MSG("  Intr Probe (unit %u)\n",unit);

    cid = HWP_CHIP_ID(unit);

    intr_if[unit] = CID_GROUP_NONE;
    for (i = 0; i < INTR_DB_SIZE; i++)
    {
        if (CID_PREFIX_MATCH(intr_db[i], cid))
        {
            intr_if[unit] = intr_db[i].gid;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
}






