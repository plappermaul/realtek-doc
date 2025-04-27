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
 * Purpose : Define the PHY hardware related function
 *
 * Feature : SDK PHY reset IOAL
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>


#include <osal/lib.h>
#include <osal/time.h>
#ifdef CONFIG_SDK_DRIVER_GPIO
  #include <drv/gpio/generalCtrl_gpio.h>
#endif /* CONFIG_SDK_DRIVER_GPIO */
#include <hwp/hw_profile.h>
#include <ioal/mem32.h>



/*
 * Symbol Definition
 */


/*
 * Macro Definition
 */

/*
 * Data Declaration
 */



/*
 * Function Declaration
 */


#if defined(CONFIG_SDK_DRIVER_GPIO)
/* Function Name:
 *      rt_bsp_phy_reset_support
 * Description:
 *      Returns support status of hardware PHY reset of the board.
 *      If the board supports hardware PHY reset, also returns the GPIO dev/pin ID.
 * Input:
 *      unit    - unit ID
 * Output:
 *      gpioDevId - GPIO device ID
 *      pinId   - GPIO pin ID
 * Return:
 *      RT_ERR_OK                   - hardware PHY reset is supported
 *      RT_ERR_CHIP_NOT_SUPPORTED   - hardware PHY reset is not supported
 *      RT_ERR_FAILED               - unable to detect the support status.
 * Note:
 *      This function is implemented per RTK demo board design.
 */
static int32
rt_bsp_phy_reset_support(uint32 unit, drv_generalCtrlGpio_devId_t *gpioDevId, uint32 *pinId)
{
    char    *name;

    if (hwp_myHwProfile == NULL)
    {
        RT_ERR(RT_ERR_FAILED, MOD_INIT, "PHY Reset GPIO fail: HW profile not found");
        return RT_ERR_FAILED;
    }

    name = hwp_myHwProfile->identifier.name;
    if (hwp_myHwProfile->sw_count == 1)
    {
        /* assign GPIO pin number according to demo board designs */
        if (osal_memcmp(name, "RTL839", osal_strlen("RTL839")) == 0) /* 839x demo borads */
        {
            *gpioDevId = GEN_GPIO_DEV_ID0_INTERNAL;
            *pinId = 23;
            return RT_ERR_OK;
        }
        else if (osal_memcmp(name, "RTL835", osal_strlen("RTL835")) == 0) /* 835x demo borads */
        {
            *gpioDevId = GEN_GPIO_DEV_ID0_INTERNAL;
            *pinId = 14;
            return RT_ERR_OK;
        }
        else
        {
            return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else
    {
        if (osal_memcmp(name, "RTL839", osal_strlen("RTL839")) == 0)
        {
            if (HWP_CHIP_FAMILY_ID(unit) == RTL8390_FAMILY_ID)
            {
                *gpioDevId = GEN_GPIO_DEV_ID0_INTERNAL;
                *pinId = 23;
                return RT_ERR_OK;
            }
        }
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
}


int ioal_phyReset_init_status=0;
static void
rt_bsp_phyReset_init(drv_generalCtrlGpio_devId_t gpioDevId, uint32 pinId)
{
    drv_generalCtrlGpio_pinConf_t   gpioConfig;
    uint32                          ret;

    if (ioal_phyReset_init_status == 1)
    {
        return;
    }

    ioal_phyReset_init_status = 1;

    ret = drv_generalCtrlGPIO_dev_init(HWP_MY_UNIT_ID(), gpioDevId, NULL);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_INIT, "PHY Reset GPIO initial fail");
        return;
    }

    drv_generalCtrlGPIO_devEnable_set(HWP_MY_UNIT_ID(), gpioDevId, ENABLED);

    gpioConfig.direction = GPIO_DIR_OUT;
    gpioConfig.default_value = 1;
    gpioConfig.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    gpioConfig.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    gpioConfig.ext_gpio.debounce = 0;
    gpioConfig.ext_gpio.inverter = 0;
    gpioConfig.ext_gpio.direction = GPIO_DIR_OUT;
    drv_generalCtrlGPIO_pin_init(HWP_MY_UNIT_ID(), gpioDevId, pinId, &gpioConfig);

}
#endif /* CONFIG_SDK_DRIVER_GPIO */

/* Function Name:
 *      rt_bsp_pinFrcRstout_phy_reset
 * Description:
 *      Control reset I/O pin to do PHY rest when trigger frc_rstout.
 * Input:
 *      unit    - unit ID
 * Output:
        None
 * Return:
 *      RT_ERR_OK                   - hardware PHY reset is supported, and PHY reset success.
 *      RT_ERR_FAILED               - unable to detect the support status or PHY reset fail.
 * Note:
 *      This function is implemented per RTK demo board design.
 */
int32
rt_bsp_pinFrcRstout_phy_reset(uint32 unit)
{
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        /*Trigger frc_rstout*/
        ioal_mem32_field_write(unit, 0xc, 9, 0x1<<9, 1);
        /* Low level output*/
        ioal_mem32_field_write(unit, 0xc, 10, 0x1<<10, 1);
        osal_time_mdelay(10 * 2);
        /*High level output*/
        ioal_mem32_field_write(unit, 0xc, 10, 0x1<<10, 0);
        osal_time_mdelay(10 * 2);
        /*disable trigger frc_rstout*/
        ioal_mem32_field_write(unit, 0xc, 9, 0x1<<9, 0);
    }
#endif
        return RT_ERR_OK;
}

/* Function Name:
 *      rt_bsp_phy_reset
 * Description:
 *      If hardware PHY reset is supported, then do PHY reset.
 * Input:
 *      unit    - unit ID
 * Output:
        None
 * Return:
 *      RT_ERR_OK                   - hardware PHY reset is supported, and PHY reset success.
 *      RT_ERR_CHIP_NOT_SUPPORTED   - hardware PHY reset is not supported
 *      RT_ERR_FAILED               - unable to detect the support status or PHY reset fail.
 * Note:
 *      This function is implemented per RTK demo board design.
 */
int32
rt_bsp_phy_reset(uint32 unit)
{
#ifdef CONFIG_SDK_DRIVER_GPIO
    int32       ret;
    uint32      pinId;
    uint32      cpu_unit = HWP_MY_UNIT_ID();
    drv_generalCtrlGpio_devId_t     gpioDevId;

    if ((ret = rt_bsp_phy_reset_support(unit, &gpioDevId, &pinId)) != RT_ERR_OK)
        return ret;

    rt_bsp_phyReset_init(gpioDevId, pinId);

    drv_generalCtrlGPIO_dataBit_set(cpu_unit, gpioDevId, pinId, 0);
    osal_time_mdelay(10 * 2);    /* Min 10mS (from Spec) * 2 = 20mS (double for safety) */
    drv_generalCtrlGPIO_dataBit_set(cpu_unit, gpioDevId, pinId, 1);
    osal_time_mdelay(140 * 2);   /* 50mS (power-on waiting) + 90mS (basic procedure) = 140mS */
    return RT_ERR_OK;
#else
    return RT_ERR_CHIP_NOT_SUPPORTED;
#endif /* CONFIG_SDK_DRIVER_GPIO */
}


/* Function Name:
 *      ioal_phy_reset
 * Description:
 *      Reset PHY chips through hardware pin
 * Input:
 *      unit    - unit id
 *      port    - base port id of the PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   Reset success
 *      RT_ERR_CHIP_NOT_SUPPORTED   Not support hardware reset PHY
 * Note:
 *    None
 */
int32
ioal_phy_reset(uint32 unit, rtk_port_t port)
{
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
#endif
    return rt_bsp_phy_reset(unit);
}


