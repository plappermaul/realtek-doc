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
 * Purpose : Definition those public RTL8231 APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) RTL8231 probe
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <common/rt_type.h>
#include <hwp/hw_profile.h>
#include <private/drv/rtl8231/rtl8231_probe.h>

#if defined(CONFIG_SDK_RTL9310)
#include <private/drv/rtl8231/rtl8231_rtl9310.h>
#endif

#if defined(CONFIG_SDK_RTL9300)
#include <private/drv/rtl8231/rtl8231_rtl9300.h>
#endif
#if defined(CONFIG_SDK_RTL8380)
#include <private/drv/rtl8231/rtl8231_rtl8380.h>
#endif
#if defined(CONFIG_SDK_RTL8390)
#include <private/drv/rtl8231/rtl8231_rtl8390.h>
#endif
/*
 * Symbol Definition
 */
#define RTL8231_DB_SIZE (sizeof(rtl8231_db)/sizeof(cid_prefix_group_t))

int32 rtl8231_common_unavail(void);

/*
 * Data Declaration
 */
const static cid_prefix_group_t rtl8231_db[] =
{
#if defined(CONFIG_SDK_RTL9310)
    /* RTL9310 series chips */
    {RTL9310_FAMILY_ID, RTL8231_R9310},
#endif
#if defined(CONFIG_SDK_RTL9300)
    /* RTL9300 series chips */
    {RTL9300_FAMILY_ID, RTL8231_R9300},
#endif
#if defined(CONFIG_SDK_RTL8390)
    /* RTL8390 series chips */
    {RTL8390_FAMILY_ID, RTL8231_R8390},
    {RTL8350_FAMILY_ID, RTL8231_R8390},
#endif
#if defined(CONFIG_SDK_RTL8380)
    /* RTL8380 series chips */
    {RTL8380_FAMILY_ID, RTL8231_R8380},
    {RTL8330_FAMILY_ID, RTL8231_R8380},
#endif
};

rtl8231_mapper_operation_t rtl8231_ops[RTL8231_CTRL_END] =
{
#if defined(CONFIG_SDK_RTL9310)
    {   /* RTL8231_R9310 */
        .i2c_read = (int32 (*)(uint32 , uint32 , uint32 , uint32 *))rtl8231_common_unavail,
        .i2c_write = (int32 (*)(uint32 , uint32 , uint32 , uint32 ))rtl8231_common_unavail,
        .mdc_read = r9310_rtl8231_mdc_read,
        .mdc_write = r9310_rtl8231_mdc_write,
        .init = r9310_rtl8231_init,
        .mdcSem_register = (int32 (*)(uint32, drv_rtl8231_mdcSem_cb_f))rtl8231_common_unavail,
        .mdcSem_unregister = (int32 (*)(uint32, drv_rtl8231_mdcSem_cb_f))rtl8231_common_unavail,
        .intrStatus_get = r9310_rtl8231_pinIntrStatus_get,
        .intrStatus_clear = r9310_rtl8231_pinIntrStatus_clear,
        .extGPIOIntrStatus_get = r9310_rtl8231_extGPIOIntrStatus_get,
        .pinIntrEnable_get = r9310_rtl8231_pinIntrEnable_get,
        .pinIntrEnable_set = r9310_rtl8231_pinIntrEnable_set,
        .pinIntrMode_get = r9310_rtl8231_pinIntrMode_get,
        .pinIntrMode_set = r9310_rtl8231_pinIntrMode_set,
        .directAccess_set = r9310_rtl8231_directAccess_set,
    },
#endif
#if defined(CONFIG_SDK_RTL9300)
    {   /* RTL8231_R9300 */
        .i2c_read = (int32 (*)(uint32 , uint32 , uint32 , uint32 *))rtl8231_common_unavail,
        .i2c_write = (int32 (*)(uint32 , uint32 , uint32 , uint32 ))rtl8231_common_unavail,
        .mdc_read = r9300_rtl8231_mdc_read,
        .mdc_write = r9300_rtl8231_mdc_write,
        .init = r9300_rtl8231_init,
        .mdcSem_register = (int32 (*)(uint32, drv_rtl8231_mdcSem_cb_f))rtl8231_common_unavail,
        .mdcSem_unregister = (int32 (*)(uint32, drv_rtl8231_mdcSem_cb_f))rtl8231_common_unavail,
        .intrStatus_get = r9300_rtl8231_pinIntrStatus_get,
        .intrStatus_clear = r9300_rtl8231_pinIntrStatus_clear,
        .extGPIOIntrStatus_get = r9300_rtl8231_extGPIOIntrStatus_get,
        .pinIntrEnable_get = r9300_rtl8231_pinIntrEnable_get,
        .pinIntrEnable_set = r9300_rtl8231_pinIntrEnable_set,
        .pinIntrMode_get = r9300_rtl8231_pinIntrMode_get,
        .pinIntrMode_set = r9300_rtl8231_pinIntrMode_set,
        .directAccess_set = r9300_rtl8231_directAccess_set,
    },
#endif
#if defined(CONFIG_SDK_RTL8390)
    {   /* RTL8231_R8390 */
        .i2c_read = (int32 (*)(uint32 , uint32 , uint32 , uint32 *))rtl8231_common_unavail,
        .i2c_write = (int32 (*)(uint32 , uint32 , uint32 , uint32 ))rtl8231_common_unavail,
        .mdc_read = r8390_rtl8231_mdc_read,
        .mdc_write = r8390_rtl8231_mdc_write,
        .init = r8390_rtl8231_init,
        .mdcSem_register = r8390_rtl8231_mdcSem_register,
        .mdcSem_unregister = r8390_rtl8231_mdcSem_unregister,
        .intrStatus_get = (int32 (*)(uint32 , uint32 , uint32 , uint32 *))rtl8231_common_unavail,
        .intrStatus_clear = (int32 (*)(uint32 , uint32 , uint32 ))rtl8231_common_unavail,
        .extGPIOIntrStatus_get = (int32 (*)(uint32 , uint32 , uint32 *))rtl8231_common_unavail,
        .pinIntrEnable_get = (int32 (*)(uint32 , uint32 , uint32, rtk_enable_t * ))rtl8231_common_unavail,
        .pinIntrEnable_set = (int32 (*)(uint32 , uint32 , uint32, rtk_enable_t))rtl8231_common_unavail,
        .pinIntrMode_get = (int32 (*)(uint32 , uint32 , uint32, drv_extGpio_interruptType_t * ))rtl8231_common_unavail,
        .pinIntrMode_set = (int32 (*)(uint32 , uint32 , uint32, drv_extGpio_interruptType_t))rtl8231_common_unavail,
        .directAccess_set = (int32 (*)(uint32 , uint32 , drv_extGpio_directAccessMode_t, rtk_enable_t))rtl8231_common_unavail,
    },
#endif
#if defined(CONFIG_SDK_RTL8380)
    {   /* RTL8231_R8380 */
        .i2c_read = (int32 (*)(uint32 , uint32 , uint32 , uint32 *))rtl8231_common_unavail,
        .i2c_write = (int32 (*)(uint32 , uint32 , uint32 , uint32 ))rtl8231_common_unavail,
        .mdc_read = r8380_rtl8231_mdc_read,
        .mdc_write = r8380_rtl8231_mdc_write,
        .init = r8380_rtl8231_init,
        .mdcSem_register = r8380_rtl8231_mdcSem_register,
        .mdcSem_unregister = r8380_rtl8231_mdcSem_unregister,
        .intrStatus_get = (int32 (*)(uint32 , uint32 , uint32 , uint32 *))rtl8231_common_unavail,
        .intrStatus_clear = (int32 (*)(uint32 , uint32 , uint32 ))rtl8231_common_unavail,
        .extGPIOIntrStatus_get = (int32 (*)(uint32 , uint32 , uint32 *))rtl8231_common_unavail,
        .pinIntrEnable_get = (int32 (*)(uint32 , uint32 , uint32, rtk_enable_t * ))rtl8231_common_unavail,
        .pinIntrEnable_set = (int32 (*)(uint32 , uint32 , uint32, rtk_enable_t))rtl8231_common_unavail,
        .pinIntrMode_get = (int32 (*)(uint32 , uint32 , uint32, drv_extGpio_interruptType_t * ))rtl8231_common_unavail,
        .pinIntrMode_set = (int32 (*)(uint32 , uint32 , uint32, drv_extGpio_interruptType_t))rtl8231_common_unavail,
        .directAccess_set = (int32 (*)(uint32 , uint32 , drv_extGpio_directAccessMode_t, rtk_enable_t))rtl8231_common_unavail,
    },
#endif
};

uint32 rtl8231_if[RTK_MAX_NUM_OF_UNIT] = {CID_GROUP_NONE};

/*
 * Function Declaration
 */

/* Function Name:
 *      rtl8231_probe
 * Description:
 *      Probe rtl8231 module of the specified device.
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
 *      None
 */
int32
rtl8231_probe(uint32 unit)
{
    uint32      i;
    uint32      cid;

    RT_INIT_MSG("  RTL8231 probe (unit %u): ", unit);

    cid = HWP_CHIP_ID(unit);

    rtl8231_if[unit] = CID_GROUP_NONE;
    for (i = 0; i < RTL8231_DB_SIZE; i++)
    {
        if (CID_PREFIX_MATCH(rtl8231_db[i], cid))
        {
            RT_INIT_MSG("(found)\n");
            rtl8231_if[unit] = rtl8231_db[i].gid;
            return RT_ERR_OK;
        }
    }

    RT_INIT_MSG("(NOT found!)\n");

    return RT_ERR_FAILED;
} /* end of rtl8231_probe */

/* Function Name:
 *      rtl8231_common_unavail
 * Description:
 *      Return chip not support
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_CHIP_NOT_SUPPORTED   - functions not supported by this chip model
 * Note:
 *      None
 */
int32
rtl8231_common_unavail(void)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
} /* end of rtl8231_common_unavail */
