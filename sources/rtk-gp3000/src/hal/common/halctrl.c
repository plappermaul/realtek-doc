/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 * Purpose : Hardware Abstraction Layer (HAL) control structure and definition in the SDK.
 *
 * Feature : HAL control structure and definition
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <hal/mac/mac_probe.h>
#include <hal/phy/phy_probe.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
hal_control_t hal_ctrl;


/*
 * Function Declaration
 */

/* Function Name:
 *      hal_init
 * Description:
 *      Initialize the hal layer API.
 * Input:
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - General Error
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_DRIVER_NOT_FOUND - The driver can not found
 * Note:
 *      Initialize the hal layer API
 */
int32
hal_init(void)
{
    int32           ret = RT_ERR_FAILED;

    /* Probe MAC */
    if ((ret = mac_probe()) != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_HAL, "mac_probe failed!!");
        return ret;
    }
#if defined(CONFIG_LUNA_G3_SERIES)
    //Luna G3 don't need mac_init,phy_probe and phy_init
#else
    /* Init MAC */
    if ((ret = mac_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_HAL, "mac_init failed!!");
        return ret;
    }

    /* Probe PHY */
    if ((ret = phy_probe()) != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_HAL, "phy_probe failed!!");
        return ret;
    }

    /* Init PHY */
    if ((ret = phy_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_HAL, "phy_init failed!!");
        return ret;
    }
#endif
    return RT_ERR_OK;
} /* end of hal_init */

/* Function Name:
 *      hal_ctrlInfo_get
 * Description:
 *      Find the hal control information structure for this chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of hal control information structure that found
 * Note:
 *      The function have found the exactly hal control information structure.
 */
hal_control_t *
hal_ctrlInfo_get(void)
{
    return &hal_ctrl;
} /* end of hal_ctrlInfo_get */

#if defined(CONFIG_SDK_KERNEL_LINUX)
EXPORT_SYMBOL(hal_ctrl);
#endif
