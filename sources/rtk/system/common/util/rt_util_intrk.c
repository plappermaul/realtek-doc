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
 * $Revision: 93545 $
 * $Date: 2018-11-15 10:43:52 +0800 (Thu, 15 Nov 2018) $
 *
 * Purpose : Define the utility macro and function in the SDK.
 *
 * Feature : SDK common utility (interrupt)
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <rtk/default.h>
#include <osal/isr.h>
#include <osal/wait.h>
#include <drv/intr/intr.h>
#include <common/util/rt_util_intr.h>
#include <common/util/rt_util_intrk.h>
#include <dev_config.h>
#include <hwp/hw_profile.h>
#if defined(CONFIG_SDK_DRIVER_GPIO)
  #include <drv/gpio/gpio.h>
#endif

#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
#include <common/rtcore/rtcore_init.h>
#endif

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
uint32 util_intr_registerCnt[RTK_DEV_MAX] = { 0 };



/*
 * Macro Declaration
 */



/*
 * Function Declaration
 */



/* Function Name:
 *      rt_util_intrk_swcore_handler
 * Description:
 *      swcore interrupt handler
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rt_util_intrk_swcore_handler(uint32 unit)
{
    uint32          isr_glb_sts;
    int32           isrId;

    /* Get ISR_GLB */
    if (drv_intr_isrSts_get(unit, &isr_glb_sts) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    RT_LOG(LOG_INFO, (MOD_RTDRV), "unit %u swcore isr_glb_sts=0x%X", unit, isr_glb_sts);

    /* Disable IMR */
    for (isrId = 0; isrId < INTR_ISR_END; isrId++)
    {
        if (INTR_ISR_CHK(isr_glb_sts, isrId))
        {
            drv_intr_imrEnable_set(unit, isrId, DISABLED);
        }
    }

#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
    RT_LOG(LOG_INFO, (MOD_RTDRV), "wakeup: rtcore");
    rtcore_swcoreIntr_wakeup(&isr_glb_sts);
    return RT_ERR_OK;
#else
    RT_LOG(LOG_INFO, (MOD_RTDRV), "wakeup: KRN intr handler");
    _rt_util_intr_handler(&isr_glb_sts);
    return RT_ERR_OK;
#endif
}


/* Function Name:
 *      _rt_util_intr_swcore_isr
 * Description:
 *      ISR callback handler of OSAL.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
osal_isrret_t
_rt_util_intr_swcore_isr(void *pParam)
{
    uint32          unit = HWP_MY_UNIT_ID();

    if (rt_util_intrk_swcore_handler(unit) == RT_ERR_OK)
    {
        return OSAL_INT_HANDLED;
    }
    else
    {
        return OSAL_INT_NONE;
    }
}


/* Function Name:
 *      _rt_util_intr_gpio_isr
 * Description:
 *      ISR callback handler of OSAL for Internal GPIO.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
osal_isrret_t
_rt_util_intr_gpio_isr(void *pParam)
{
    uint32          isr_glb_sts;

    isr_glb_sts = INTR_ISR_BIT(INTR_ISR_GPIO);
    RT_LOG(LOG_INFO, (MOD_RTDRV), "gpio isr_glb_sts=0x%X", isr_glb_sts);

#if defined(CONFIG_SDK_DRIVER_GPIO)
    drv_gpio_isrStsShadow_backup();
#endif /* CONFIG_SDK_DRIVER_GPIO */

  #if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
    RT_LOG(LOG_INFO, (MOD_RTDRV), "wakeup: rtcore");
    rtcore_swcoreIntr_wakeup(&isr_glb_sts);
    return OSAL_INT_HANDLED;
  #else
    RT_LOG(LOG_INFO, (MOD_RTDRV), "wakeup: KRN intr handler");
    _rt_util_intr_handler(&isr_glb_sts);
    return OSAL_INT_HANDLED;
  #endif
}

/* Function Name:
 *      _rt_util_intr_DevCb_get
 * Description:
 *      Get Device type and osal registration callback API
 * Input:
 *      isr_id - ISR ID
 * Output:
 *      pDev_type  - device type
 *      pOsCb - callback function for osal isr registration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_rt_util_intr_DevCb_get(drv_intr_isr_t isr_id, uint32 *pDev_type, osal_isrret_t (**pOsCb)(void *pParam))
{
    int32   ret;
    switch(isr_id)
    {
      case INTR_ISR_PORT_LINK_CHG:
      case INTR_ISR_EXT_GPIO:
      case INTR_ISR_OAM_DYGASP:
      case INTR_ISR_CCM:
        *pDev_type = RTK_DEV_SWCORE;
        *pOsCb = _rt_util_intr_swcore_isr;
        break;

      case INTR_ISR_GPIO:
        *pDev_type = RTK_DEV_GPIO_ABCD;
        *pOsCb = _rt_util_intr_gpio_isr;
        break;

      default:
        ret = RT_ERR_INPUT;
        RT_ERR(ret, (MOD_RTDRV), "Unknown isr_id=%u", isr_id);
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      _rt_util_intr_os_register
 * Description:
 *      Register callback to the system
 * Input:
 *      isr_id - ISR ID
 * Output:
 *      pDev_type  - device type
 *      pOsCb - callback function for osal isr registration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_rt_util_intr_os_register(drv_intr_isr_t isr_id)
{
    int32   ret;
    uint32  dev_type;
    osal_isrret_t       (*osCb)(void *pParam);

    if ((ret = _rt_util_intr_DevCb_get(isr_id, &dev_type, &osCb)) != RT_ERR_OK)
    {
        return ret;
    }

    RT_LOG(LOG_INFO, (MOD_RTDRV), "isr_id=%u dev_type=%u cnt=%u", isr_id, dev_type, util_intr_registerCnt[dev_type]);

    if (util_intr_registerCnt[dev_type] == 0)
    {
#if !defined(CONFIG_SDK_EXTERNAL_CPU)
        RT_LOG(LOG_INFO, (MOD_RTDRV), "isr_id=%u register to system", isr_id);
        osal_isr_register(dev_type, osCb, NULL);
#endif
    }
    util_intr_registerCnt[dev_type]++;

    return RT_ERR_OK;
}


/* Function Name:
 *      _rt_util_intr_os_unregister
 * Description:
 *      Register callback to the system
 * Input:
 *      isr_id - ISR ID
 * Output:
 *      pDev_type  - device type
 *      pOsCb - callback function for osal isr registration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_rt_util_intr_os_unregister(drv_intr_isr_t isr_id)
{
    int32   ret;
    uint32  dev_type;
    osal_isrret_t       (*osCb)(void *pParam);

    if ((ret = _rt_util_intr_DevCb_get(isr_id, &dev_type, &osCb)) != RT_ERR_OK)
    {
        return ret;
    }

    RT_LOG(LOG_INFO, (MOD_RTDRV), "isr_id=%u dev_type=%u cnt=%u", isr_id, dev_type, util_intr_registerCnt[dev_type]);

    if (util_intr_registerCnt[dev_type] > 0)
        util_intr_registerCnt[dev_type]--;

    if (util_intr_registerCnt[dev_type] == 0)
    {
#if !defined(CONFIG_SDK_EXTERNAL_CPU)
        RT_LOG(LOG_INFO, (MOD_RTDRV), "isr_id=%u unregister from system", isr_id);
        osal_isr_unregister(dev_type);
#endif
    }

    return RT_ERR_OK;
}


