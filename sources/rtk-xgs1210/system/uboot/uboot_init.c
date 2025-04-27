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
*
* Purpose : Initial SDK in U-Boot
*
*/


/*
 * Include Files
 */
#include <common.h>
#include <common/rt_autoconf.h>
#include <ioal/ioal_init.h>
#include <osal/print.h>
#include <hwp/hw_profile.h>
#include <rtk/init.h>
#include <private/drv/rtl8231/rtl8231_probe.h>
#include <private/drv/rtl8231/rtl8231.h>
#include <drv/spi/spi.h>
#include <hal/mac/mac_probe.h>
#include <hal/phy/phy_probe.h>
#include <dal/dal_mgmt.h>
#include <dal/dal_construct.h>
#include <uboot/cmd/uboot_cmd.h>
#include <hwp/hw_profile.h>
#include <common/rtcore/rtcore_init.h>


#if defined(CONFIG_SDK_DRIVER_GPIO)
  #include <drv/gpio/gpio.h>
#endif

#if defined(CONFIG_SDK_DRIVER_NIC)
  #include <drv/nic/nic.h>
#endif

#if defined(CONFIG_SDK_DRIVER_I2C)
  #include <drv/i2c/i2c.h>
#endif


/*
 * Symbol Definition
 */
int sdk_init_times[UBOOT_SDK_INIT_PHASE_MAX]={0}; /* how many times the uboot_sdk_init() has been called */

/*
 * Data Declaration
 */


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */



/* Function Name:
 *      uboot_sdk_init
 * Description:
 *      Initialize sdk in U-Boot environment
 * Input:
 *      phase   - till which phase
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - Success
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32 uboot_sdk_init(uboot_sdk_init_phase_t phase)
{

    if (0 == sdk_init_times[UBOOT_SDK_INIT_PHASE_RTCORE])
    {
        if( rtcore_init() == RT_ERR_FAILED)
            return RT_ERR_FAILED;
        sdk_init_times[UBOOT_SDK_INIT_PHASE_RTCORE]++;
    }

    if (UBOOT_SDK_INIT_PHASE_RTCORE == phase)
        return RT_ERR_OK;

    if (0 == sdk_init_times[UBOOT_SDK_INIT_PHASE_RTK])
    {
        if( rtk_init() == RT_ERR_FAILED)
            return RT_ERR_FAILED;
        sdk_init_times[UBOOT_SDK_INIT_PHASE_RTK]++;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      misc_init_r
 * Description:
 *      Initialize of misc objects.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      0 - Success
 * Note:
 *      None
 */
int misc_init_r(void)
{

    return 0;

} /* end of misc_init_r */



