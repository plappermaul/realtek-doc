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
 * Purpose : Definition those public GPIO APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <hwp/hw_profile.h>
#include <drv/gpio/gpio.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <private/drv/gpio/gpio_mapper.h>
#if (defined(CONFIG_SDK_RTL9310))
  #include <private/drv/gpio/gpio_common.h>
#endif
#if (defined(CONFIG_SDK_RTL9300))
  #include <private/drv/gpio/gpio_rtl9300.h>
#endif/* defined(CONFIG_SDK_RTL9300) */
#if (defined(CONFIG_SDK_RTL8390))
  #include <private/drv/gpio/gpio_rtl8390.h>
#endif/* defined(CONFIG_SDK_RTL8390) */
#if (defined(CONFIG_SDK_RTL8380))
  #include <private/drv/gpio/gpio_rtl8380.h>
#endif/* defined(CONFIG_SDK_RTL8380) */

/*
 * Symbol Definition
 */
#define GPIO_DB_SIZE        (sizeof(gpio_db)/sizeof(cid_prefix_group_t))



/*
 * Data Declaration
 */
const static cid_prefix_group_t gpio_db[] =
{
#if (defined(CONFIG_SDK_RTL9310))
    {RTL9310_FAMILY_ID, GPIO_R9310},
#endif
#if (defined(CONFIG_SDK_RTL9300))
    {RTL9300_FAMILY_ID, GPIO_R9300},
#endif
#if (defined(CONFIG_SDK_RTL8390))
    {RTL8390_FAMILY_ID, GPIO_R8390},
    {RTL8350_FAMILY_ID, GPIO_R8390},
#endif
#if (defined(CONFIG_SDK_RTL8380))
    {RTL8380_FAMILY_ID, GPIO_R8380},
    {RTL8330_FAMILY_ID, GPIO_R8380},
#endif
};

gpio_mapper_operation_t gpio_ops[GPIO_CTRL_END] =
{
#if (defined(CONFIG_SDK_RTL9310))
  [GPIO_R9310] =
    {
        .init               = gpio_common_init,
        .direction_set      = gpio_common_direction_set,
        .control_set        = NULL,     /* not support */
        .intrMode_set       = gpio_common_intrMode_set,
        .dataBit_init       = NULL,     /* not needed */
        .dataBit_get        = gpio_common_dataBit_get,
        .dataBit_set        = gpio_common_dataBit_set,
        .intrStatus_get     = gpio_common_intrStatus_get,
        .intrStatus_clear   = gpio_common_intrStatus_clear,
        .portRange_get      = gpio_common_portRange_get,
    },
#endif /* defined(CONFIG_SDK_RTL9310) */
#if (defined(CONFIG_SDK_RTL9300))
  [GPIO_R9300] =
    {
        .init               = rtl9300_gpio_init,
        .direction_set      = rtl9300_gpio_direction_set,
        .control_set        = NULL,     /* not support */
        .intrMode_set       = rtl9300_gpio_intrMode_set,
        .dataBit_init       = NULL,     /* not needed */
        .dataBit_get        = rtl9300_gpio_dataBit_get,
        .dataBit_set        = rtl9300_gpio_dataBit_set,
        .intrStatus_get     = rtl9300_gpio_intrStatus_get,
        .intrStatus_clear   = rtl9300_gpio_intrStatus_clear,
        .portRange_get      = rtl9300_gpio_portRange_get,
    },
#endif /* defined(CONFIG_SDK_RTL9300) */
#if (defined(CONFIG_SDK_RTL8390))
  [GPIO_R8390] =
    {
        .init               = rtl8390_gpio_init,
        .direction_set      = rtl8390_gpio_direction_set,
        .control_set        = rtl8390_gpio_control_set,
        .intrMode_set       = rtl8390_gpio_intrMode_set,
        .dataBit_init       = rtl8390_gpio_dataBit_init,
        .dataBit_get        = rtl8390_gpio_dataBit_get,
        .dataBit_set        = rtl8390_gpio_dataBit_set,
        .intrStatus_get     = rtl8390_gpio_intrStatus_get,
        .intrStatus_clear   = rtl8390_gpio_intrStatus_clear,
        .portRange_get      = rtl8390_gpio_portRange_get,
    },
#endif/* defined(CONFIG_SDK_RTL8390) */
#if (defined(CONFIG_SDK_RTL8380))
  [GPIO_R8380] =
    {
        .init               = rtl8380_gpio_init,
        .direction_set      = rtl8380_gpio_direction_set,
        .control_set        = rtl8380_gpio_control_set,
        .intrMode_set       = rtl8380_gpio_intrMode_set,
        .dataBit_init       = rtl8380_gpio_dataBit_init,
        .dataBit_get        = rtl8380_gpio_dataBit_get,
        .dataBit_set        = rtl8380_gpio_dataBit_set,
        .intrStatus_get     = rtl8380_gpio_intrStatus_get,
        .intrStatus_clear   = rtl8380_gpio_intrStatus_clear,
        .portRange_get      = rtl8380_gpio_portRange_get,
    },
#endif/* defined(CONFIG_SDK_RTL8380) */
};

uint32 gpio_if[RTK_MAX_NUM_OF_UNIT] = {CID_GROUP_NONE};

/*
 * Function Declaration
 */

/* Function Name:
 *      gpio_probe
 * Description:
 *      Probe GPIO module of the specified device.
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
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
gpio_probe(uint32 unit)
{
    int32           i;
    uint32          cid;

    RT_INIT_MSG("  GPIO probe (unit %u): ", unit);

    cid = HWP_CHIP_ID(unit);

    gpio_if[unit] = CID_GROUP_NONE;
    for (i = 0; i < GPIO_DB_SIZE; i++)
    {
        if (CID_PREFIX_MATCH(gpio_db[i], cid))
        {
            gpio_if[unit] = gpio_db[i].gid;
            RT_INIT_MSG("(found)\n");
            return RT_ERR_OK;
        }
    }

    RT_INIT_MSG("(Not found)\n");
    return RT_ERR_CHIP_NOT_FOUND;

} /* end of gpio_probe */




