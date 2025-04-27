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
 * Purpose : PHY 8218B/8218FB/8214FC Driver APIs.
 *
 * Feature : PHY 8218B/8218FB/8214FC Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8208.h>


/*
 * Symbol Definition
 */

#if defined(CONFIG_SDK_RTL8380)
phy_patch_array_type0_t rtl8380_8214fb_perchip[] = {\
    };
phy_patch_array_type1_t rtl8380_8214fb_perport[] = {\

    };



/* Function Name:
 *      rtl8380_rtl8208_config
 * Description:
 *      Patch the PHY:8208 for RTL8380.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtl8380_rtl8214fb_config(uint32 unit, rtk_port_t port)
{
    int32   ret;
    ret = RT_ERR_OK;
    return ret;
}
#endif


/* Function Name:
 *      sub_phy_8214fb_patch_set
 * Description:
 *      Patch the PHY:8214fb.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
sub_phy_8214fb_patch_set(uint32 unit, rtk_port_t port)
{
    int32   ret;

#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8380_30_FAMILY(unit))
    {
        if ((ret = rtl8380_rtl8214fb_config(unit, port)) != RT_ERR_OK)
            return ret;

        /* For 833x, enable serdes always linkup */
        {
            uint32 phy_data;

            if((port % 8) == 0)
            {
                /*14B*/
                phy_data = 8;
                ret = hal_miim_write(unit,  port,  15,  30, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;

                phy_data = 0x8403;
                ret = hal_miim_write(unit,  port,  15,  21, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;

                phy_data = 8;
                ret = hal_miim_write(unit,  port+1,  15,  30, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;

                phy_data = 0x8403;
                ret = hal_miim_write(unit,  port+1,  15,  21, phy_data);
                if(ret != RT_ERR_OK)
                    return ret;
            }
        }
    }
#endif

    ret = RT_ERR_OK;
    return ret;
} /* end of sub_phy_8208_patch_set */


