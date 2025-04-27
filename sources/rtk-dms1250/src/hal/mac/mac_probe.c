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
 * Purpose : MAC probe and init service APIs in the SDK.
 *
 * Feature : MAC probe and init service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <hal/chipdef/chip.h>
#include <hal/chipdef/driver.h>
#include <hal/common/halctrl.h>
#if defined(CONFIG_SDK_RTL8380)
#include <dal/maple/dal_maple_construct.h>
#endif
#if defined(CONFIG_SDK_RTL8390)
#include <dal/cypress/dal_cypress_construct.h>
#endif
#include <hwp/hw_profile.h>
#include <hwp/hwp_util.h>


/*
 * Symbol Definition
 */


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      mac_probe
 * Description:
 *      Probe the MAC chip in the specified chip.
 * Input:
 *      unit    - unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
mac_probe(uint32 unit)
{

    rt_device_t *pDev;
    rt_driver_t *pMdriver = NULL;

    RT_INIT_MSG("  MAC probe (unit %u)\n",unit);

    RT_LOG(LOG_INFO, MOD_HAL, "unit=%d, chip_id=0x%X",
       unit, RT_CHIP_ID_DISPLAY(HWP_CHIP_ID(unit)));

    /* Find device */
    if ((pDev = hal_find_device(HWP_CHIP_ID(unit), HWP_CHIP_REV(unit))) == NULL)
    {
        RT_INIT_ERR(RT_ERR_CHIP_NOT_FOUND, MOD_HAL, "hal_find_device(unit=%u, chip_id=0x%X) failed!!\n",\
               unit, RT_CHIP_ID_DISPLAY(HWP_CHIP_ID(unit)));

        return RT_ERR_CHIP_NOT_FOUND;
    }

    if ((HWP_8330_FAMILY_ID(unit) ||
         HWP_8380_FAMILY_ID(unit)) &&
        (HWP_CHIP_REV(unit) <= CHIP_REV_ID_B) )
    {
        pDev->pCapacityInfo->max_num_of_metering = 64;
    }

    hal_ctrl[unit].pDev_info = pDev;

#ifdef RTK_HWP_DEFAULT_PROFILE
    if (HWP_USEDEFHWP())
    {
        hwp_defaultProfilePort_build(hwp_myHwProfile, pDev);
        hwp_parsedInfo_buildup((hwp_parsedInfo_t **)&hwp_myHwProfile->parsed_info, hwp_myHwProfile);
        if (HWP_UNIT_VALID_LOCAL(unit))
        {
            hwp_chipInfo_update(unit);
        }
    }
#endif /* RTK_HWP_DEFAULT_PROFILE */

    /* Find chip major driver */
    if ((pMdriver = hal_find_driver(hal_ctrl[unit].pDev_info->driver_id,\
        hal_ctrl[unit].pDev_info->driver_rev_id)) == NULL)
    {
        RT_INIT_ERR(RT_ERR_DRIVER_NOT_FOUND, MOD_HAL, "hal_find_driver(unit=%u, drv_id=0x%X, drv_rev_id=0x%X) failed!!\n",\
           unit, hal_ctrl[unit].pDev_info->driver_id, hal_ctrl[unit].pDev_info->driver_rev_id);

        return RT_ERR_DRIVER_NOT_FOUND;
    }
    hal_ctrl[unit].pChip_driver = pMdriver;
    hal_ctrl[unit].chip_flags |= HAL_CHIP_INITED;
    hal_ctrl[unit].chip_flags |= HAL_CHIP_ATTACHED;

    RT_INIT_MSG("    Chip %X (found)\n", RT_CHIP_ID_DISPLAY(HWP_CHIP_ID(unit)));

    return RT_ERR_OK;
} /* end of mac_probe */


/* Function Name:
 *      mac_init
 * Description:
 *      Init the MAC chip in the specified chip.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
mac_init(uint32 unit)
{
    int32   ret = RT_ERR_FAILED;

    RT_INIT_MSG("  MAC init (unit %u)\n",unit);

    /* Initialize MAC */
    if ((ret = hal_ctrl[unit].pChip_driver->pMacdrv->fMdrv_init(unit)) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_HAL, "mac driver init failed!!\n");
        return ret;
    }

    return RT_ERR_OK;
} /* end of mac_init */

