/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
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
 * Purpose : Definition those public SPI APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <common/debug/rt_log.h>
#include <hwp/hw_profile.h>
#include <private/drv/tc/tc_mapper.h>
#include <private/drv/tc/tc_common.h>

/*
 * Symbol Definition
 */
#define TC_DB_SIZE     (sizeof(tc_db)/sizeof(cid_prefix_group_t))

/*
 * Data Declaration
 */
const static cid_prefix_group_t tc_db[] =
{
#if defined(CONFIG_SDK_RTL8390)
    {RTL8390_FAMILY_ID, TC_R8390},
    {RTL8350_FAMILY_ID, TC_R8390},
#endif
#if defined(CONFIG_SDK_RTL8380)
    {RTL8380_FAMILY_ID, TC_R8380},
    {RTL8330_FAMILY_ID, TC_R8380},
#endif
#if (defined(CONFIG_SDK_RTL9300))
    {RTL9300_FAMILY_ID, TC_R9300},
#endif
#if (defined(CONFIG_SDK_RTL9310))
    {RTL9310_FAMILY_ID, TC_R9310},
#endif
};


tc_mapper_operation_t tc_ops[TC_CTRL_END] =
{
#if (defined(CONFIG_SDK_RTL8390))
    {   /* TC_R8390 */
        .init = tc_init,
        .tc_enable_set = tc_enable_set,
        .tcMode_set = tc_mode_set,
        .tcDivFactor_set = tc_divFactor_set,
        .tcDataInitValue_set = tc_dataInitValue_set,
        .tcIntEnable_set = tc_intEnable_set,
        .tcIntState_get = tc_intState_get,
        .tcIntState_clear = tc_intState_clear,
        .tcCounterValue_get = tc_counterValue_get,
    },
#endif
#if (defined(CONFIG_SDK_RTL8380))
    {   /* TC_R8380 */
        .init = tc_init,
        .tc_enable_set = tc_enable_set,
        .tcMode_set = tc_mode_set,
        .tcDivFactor_set = tc_divFactor_set,
        .tcDataInitValue_set = tc_dataInitValue_set,
        .tcIntEnable_set = tc_intEnable_set,
        .tcIntState_get = tc_intState_get,
        .tcIntState_clear = tc_intState_clear,
        .tcCounterValue_get = tc_counterValue_get,
    },
#endif
#if (defined(CONFIG_SDK_RTL9300))
    {   /* TC_R9300 */
        .init = tc_init,
        .tc_enable_set = tc_enable_set,
        .tcMode_set = tc_mode_set,
        .tcDivFactor_set = tc_divFactor_set,
        .tcDataInitValue_set = tc_dataInitValue_set,
        .tcIntEnable_set = tc_intEnable_set,
        .tcIntState_get = tc_intState_get,
        .tcIntState_clear = tc_intState_clear,
        .tcCounterValue_get = tc_counterValue_get,
    },
#endif
#if (defined(CONFIG_SDK_RTL9310))
    {   /* TC_R9310 */
        .init = tc_init,
        .tc_enable_set = tc_enable_set,
        .tcMode_set = tc_mode_set,
        .tcDivFactor_set = tc_divFactor_set,
        .tcDataInitValue_set = tc_dataInitValue_set,
        .tcIntEnable_set = tc_intEnable_set,
        .tcIntState_get = tc_intState_get,
        .tcIntState_clear = tc_intState_clear,
        .tcCounterValue_get = tc_counterValue_get,
    },
#endif
};

uint32 tc_if[RTK_MAX_NUM_OF_UNIT] = {CID_GROUP_NONE};

/*
 * Function Declaration
 */

/* Function Name:
 *      tc_probe
 * Description:
 *      Probe tc module of the specified device.
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
tc_probe(uint32 unit)
{
    uint32      i;
    uint32      cid;

    RT_INIT_MSG("  TC probe (unit %u): ", unit);

    cid = HWP_CHIP_ID(unit);

    tc_if[unit] = CID_GROUP_NONE;
    for (i=0; i<TC_DB_SIZE; i++)
    {
        if (CID_PREFIX_MATCH(tc_db[i], cid))
        {
            tc_if[unit] = tc_db[i].gid;
            RT_INIT_MSG("(found)\n");
            return RT_ERR_OK;
        }
    }
    RT_INIT_MSG("(Not found)\n");
    return RT_ERR_CHIP_NOT_FOUND;

} /* end of tc_probe */

