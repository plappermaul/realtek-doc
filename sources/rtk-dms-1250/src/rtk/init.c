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
 * Purpose : Definition of Init API
 *
 * Feature : Initialize All Layers of RTK Module
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <dal/dal_mgmt.h>
#if defined(CONFIG_SDK_DRIVER_RTK_CUSTOMER)
#include <rtk/customer_hook.h>
#endif
#include <rtk/init.h>
#include <rtk/stack.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
 uint32   rtk_init_status = INIT_NOT_COMPLETED;

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_unit_attach
 * Description:
 *      Attach an unit to HAL/DAL system of RTK
 * Input:
 *      unit_list   - unit ID
 *      devID_list  - device ID list
 *      unit_num    - number of units in the list
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
rtk_unit_attach(uint8 *unit_list, uint8 *devID_list, int32 unit_num)
{
    int32 ret = RT_ERR_FAILED;
    uint32  unit, devID, i;

    /* Initialize the hal layer */
    for (i = 0; i < unit_num; i++)
    {
        unit = unit_list[i];
        devID = devID_list[i];
        if (unit >= RTK_MAX_NUM_OF_UNIT)
        {
            RT_LOG(LOG_EVENT, MOD_INIT, "Error attach unit %u!!\n", unit);
            continue;
        }

        if ((ret = hal_init(unit, devID)) != RT_ERR_OK)
        {
            RT_INIT_ERR(ret, MOD_INIT, "hal_init failed!!(unit %d)\n",unit);
            continue;
        }
    }
    RT_LOG(LOG_EVENT, MOD_INIT, "hal_init Completed!!\n");

    for (i = 0; i < unit_num; i++)
    {
        unit = unit_list[i];
        devID = devID_list[i];
        if (unit >= RTK_MAX_NUM_OF_UNIT)
        {
            RT_LOG(LOG_EVENT, MOD_INIT, "Error attach unit %u!!\n", unit);
            continue;
        }

        if ((ret = dal_mgmt_initDevice(unit)) != RT_ERR_OK)
        {
            RT_INIT_ERR(ret, MOD_INIT, "dal_mgmt_initDevice failed!!\n");
            continue;
        }
    }
    RT_LOG(LOG_EVENT, MOD_INIT, "dal_mgmt_initDevice Completed!!\n");

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_unit_detach
 * Description:
 *      detach an uniti from HAL/DAL system of RTK
 * Input:
 *      unit_list   - unit ID list
 *      unit_num    - number of units in the list
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
rtk_unit_detach(uint8 *unit_list, int32 unit_num)
{
    uint32  unit, i;

    for (i = 0; i < unit_num; i++)
    {
        if ((unit = unit_list[i]) >= RTK_MAX_NUM_OF_UNIT)
        {
            RT_LOG(LOG_EVENT, MOD_INIT, "Error detach unit %u!!\n", unit);
            continue;
        }

        hal_ctrl[unit].chip_flags &= ~(HAL_CHIP_ATTACHED);
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_init
 * Description:
 *      Initialize the specified device
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 * Changes:
 *      None
 */
int32
rtk_init(void)
{
    int32 ret = RT_ERR_FAILED;
    uint32 unit;

    RT_INIT_REENTRY_CHK(rtk_init_status);
    RT_INIT_MSG("RTK Driver Module Initialize\n");

    RT_LOG(LOG_EVENT, MOD_INIT, "rtk_init Start!!\n");

    HWP_UNIT_TRAVS_LOCAL(unit){

        if ((ret = hal_init(unit, unit)) != RT_ERR_OK)
        {
            RT_INIT_ERR(ret, MOD_INIT, "hal_init failed!!(unit %d)\n",unit);
            return ret;
        }
        RT_LOG(LOG_EVENT, MOD_INIT, "hal_init Completed!!\n");
    }

    HWP_UNIT_TRAVS_LOCAL(unit){

        if ((ret = dal_mgmt_initDevice(unit)) != RT_ERR_OK)
        {
            RT_INIT_ERR(ret, MOD_INIT, "dal_mgmt_initDevice failed!!\n");
            return ret;
        }
        RT_LOG(LOG_EVENT, MOD_INIT, "dal_mgmt_initDevice Completed!!\n");
    }

    /* Initialize the dal layer misc items */
#ifndef __BOOTLOADER__
    if ((ret = dal_mgmt_misc_init()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "dal_mgmt_misc_init failed!!\n");
        return ret;
    }
    RT_LOG(LOG_EVENT, MOD_INIT, "dal_mgmt_misc_init Completed!!\n");

#if defined(CONFIG_SDK_DRIVER_RTK_CUSTOMER)
    /*Customer RTK API initial process will be executed over here*/
    if ((ret = rtk_customer_api_init()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "rtk_customer_api_init failed!!\n");
        return ret;
    }
#endif
#endif


    rtk_init_status = INIT_COMPLETED;

    return ret;

} /* end of rtk_init */

