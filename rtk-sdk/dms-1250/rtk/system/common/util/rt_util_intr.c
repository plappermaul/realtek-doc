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
#include <osal/thread.h>
#include <drv/intr/intr.h>
#include <common/util/rt_util_intr.h>
#include <common/util/rt_util_intrk.h>
#include <dev_config.h>
#include <hwp/hw_profile.h>

#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__)
  #include <rtcore/user/rtcore_drv_usr.h>
#endif

/*
 * Symbol Definition
 */
#define RT_UTIL_INTR_THREAD_NAME_LEN            16




typedef struct rt_util_intr_hdlr_s
{
    drv_intr_isr_t  isr_id;
    void                    (*func)(void *);
    char                    name[RT_UTIL_INTR_THREAD_NAME_LEN];
    int                     stack_size;
    int                     thread_pri;
    osal_thread_t           thread_id;
    osal_event_t            event;
    drv_intr_callback_t     callback;
    void                    *pIsr_param;
} rt_util_intr_hdlr_t;



void rt_util_intr_common_thread(void *pInput);
void rt_util_intr_portLinkChg_thread(void *pInput);
void rt_util_intr_oamDygasp_thread(void *pInput);
void rt_util_intr_ccm_thread(void *pInput);


/*
 * Data Declaration
 */


/*
 * swcore ISR handler
 */
rt_util_intr_hdlr_t  rt_util_intr_isrHdlr[INTR_ISR_END] =
    {   [INTR_ISR_PORT_LINK_CHG] =
            {   .isr_id = INTR_ISR_PORT_LINK_CHG,
                .func = rt_util_intr_portLinkChg_thread,
                .name = "linkChgIsr",
                .stack_size = RTK_DEFAULT_LINK_MON_ISR_STACK_SIZE,
                .thread_pri = RTK_DEFAULT_LINK_MON_ISR_THREAD_PRI,
                .thread_id = 0,
                .event = 0,
                .callback = NULL,
                .pIsr_param = NULL,
            } ,
        [INTR_ISR_EXT_GPIO] =
            {   .isr_id = INTR_ISR_EXT_GPIO,
                .func = rt_util_intr_common_thread,
                .name = "extGpioIsr",
                .stack_size = 4096,
                .thread_pri = 0,
                .thread_id = 0,
                .event = 0,
                .callback = NULL,
                .pIsr_param = NULL,
            } ,
        [INTR_ISR_OAM_DYGASP] =
            {   .isr_id = INTR_ISR_OAM_DYGASP,
                .func = rt_util_intr_oamDygasp_thread,
                .name = "oamDygaspIsr",
                .stack_size = 4096,
                .thread_pri = 0,
                .thread_id = 0,
                .event = 0,
                .callback = NULL,
                .pIsr_param = NULL,
            } ,
        [INTR_ISR_CCM] =
            {   .isr_id = INTR_ISR_CCM,
                .func = rt_util_intr_ccm_thread,
                .name = "ccmIsr",
                .stack_size = 4096,
                .thread_pri = 0,
                .thread_id = 0,
                .event = 0,
                .callback = NULL,
                .pIsr_param = NULL,
            } ,
        [INTR_ISR_GPIO] =
            {   .isr_id = INTR_ISR_GPIO,
                .func = rt_util_intr_common_thread,
                .name = "gpio",
                .stack_size = 4096,
                .thread_pri = 0,
                .thread_id = 0,
                .event = 0,
                .callback = NULL,
                .pIsr_param = NULL,
            } ,
    };







/*
 * Macro Declaration
 */



/*
 * Function Declaration
 */

/* Function Name:
 *      rt_util_intr_portLinkChg_thread
 * Description:
 *      Link change interrupt handler thread
 * Input:
 *      isr_glb_sts  - ISR status bitmap
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
rt_util_intr_portLinkChg_thread(void *pInput)
{
    rt_util_intr_hdlr_t   *pisr_hdlr = pInput;
    uint32  unit;

    while (1)
    {
        osal_wait_event(pisr_hdlr->event);

        if (pisr_hdlr->callback != NULL)
        {
            (pisr_hdlr->callback)(pisr_hdlr->pIsr_param);
        }
        HWP_UNIT_TRAVS_LOCAL(unit)
        {
            drv_intr_imrEnable_set(unit, INTR_ISR_PORT_LINK_CHG, ENABLED);
        }
    }
}


/* Function Name:
 *      rt_util_intr_oamDygasp_thread
 * Description:
 *      OAM dying gasp interrupt handler thread
 * Input:
 *      isr_glb_sts  - ISR status bitmap
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
rt_util_intr_oamDygasp_thread(void *pInput)
{
    rt_util_intr_hdlr_t   *pisr_hdlr = pInput;
    uint32  unit;

    while (1)
    {
        osal_wait_event(pisr_hdlr->event);
        if (pisr_hdlr->callback != NULL)
        {
            (pisr_hdlr->callback)(pisr_hdlr->pIsr_param);
        }
        HWP_UNIT_TRAVS_LOCAL(unit)
        {
            drv_intr_imrEnable_set(unit, INTR_ISR_OAM_DYGASP, ENABLED);
        }
    }
}


/* Function Name:
 *      rt_util_intr_ccm_thread
 * Description:
 *      CCM interrupt handler thread
 * Input:
 *      isr_glb_sts  - ISR status bitmap
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
rt_util_intr_ccm_thread(void *pInput)
{
    rt_util_intr_hdlr_t   *pisr_hdlr = pInput;
    uint32  unit;

    while (1)
    {
        osal_wait_event(pisr_hdlr->event);
        if (pisr_hdlr->callback != NULL)
        {
            (pisr_hdlr->callback)(pisr_hdlr->pIsr_param);
        }
        HWP_UNIT_TRAVS_LOCAL(unit)
        {
            drv_intr_imrEnable_set(unit, INTR_ISR_CCM, ENABLED);
        }
    }
}


/* Function Name:
 *      rt_util_intr_common_thread
 * Description:
 *      CCM interrupt handler thread
 * Input:
 *      isr_glb_sts  - ISR status bitmap
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
rt_util_intr_common_thread(void *pInput)
{
    rt_util_intr_hdlr_t   *pisr_hdlr = pInput;

    while (1)
    {
        osal_wait_event(pisr_hdlr->event);
        RT_LOG(LOG_INFO, (MOD_RTDRV), "thread=%s isr_id=%u got event", pisr_hdlr->name, pisr_hdlr->isr_id);

        if (pisr_hdlr->callback != NULL)
        {
            (pisr_hdlr->callback)(pisr_hdlr->pIsr_param);
        }
    }
}

/* Function Name:
 *      _rt_util_intr_handler
 * Description:
 *      Interrupt handler
 * Input:
 *      isr_glb_sts  - ISR status bitmap
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
_rt_util_intr_handler(uint32 *isr_glb_sts)
{
    rt_util_intr_hdlr_t    *pisr_hdlr = NULL;
    int32   isrId;

    /* dispatch ISR */
    for (isrId = 0; isrId < INTR_ISR_END; isrId++)
    {
        if (INTR_ISR_CHK(*isr_glb_sts, isrId))
        {
            pisr_hdlr = &rt_util_intr_isrHdlr[isrId];
            if (pisr_hdlr->thread_id != 0)
            {
                osal_wake_up(pisr_hdlr->event);
            }
        }
    }


    return RT_ERR_OK;
}


/* Function Name:
 *      _rt_util_intr_os_set
 * Description:
 *      Register/Unregisger interrupt to OS
 * Input:
 *      isr_id - enum of ISR
 *      enable - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
_rt_util_intr_os_set(drv_intr_isr_t isr_id, rtk_enable_t enable)
{
    RT_LOG(LOG_INFO, (MOD_RTDRV), "set isr_id %u en=%u", isr_id, enable);

    if (enable == ENABLED)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__)
        /* for Linux user mode SDK, the ISR is registered in Kernel space, and notify back to user space */
        _rtcore_isrId_set(isr_id, enable);
#else
        _rt_util_intr_os_register(isr_id);
#endif
    }
    else
    {
#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__)
        /* for Linux user mode SDK, the ISR is registered in Kernel space, and notify back to user space */
        _rtcore_isrId_set(isr_id, enable);
#else
        _rt_util_intr_os_unregister(isr_id);
#endif
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rt_util_intr_isr_register
 * Description:
 *      Register swcore ISR callback
 * Input:
 *      isr_id - enum of ISR
 *      callback - user callback
 *      pIsr_param - user parameter. Shall give globle memory or malloc memory address.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rt_util_intr_isr_register(drv_intr_isr_t isr_id, drv_intr_callback_t callback, void *pIsr_param)
{
    rt_util_intr_hdlr_t    *pisr_hdlr;

    if (isr_id >= INTR_ISR_END)
    {
        return RT_ERR_INPUT;
    }

    RT_LOG(LOG_INFO, (MOD_RTDRV), "isr_id %u register callback=0x%X", isr_id, (uint32)(uintptr)callback);

    pisr_hdlr = &rt_util_intr_isrHdlr[isr_id];
    pisr_hdlr->callback = callback;
    pisr_hdlr->pIsr_param = pIsr_param;

    if (pisr_hdlr->thread_id == 0)
    {
        osal_wait_module_create(&pisr_hdlr->event);
        pisr_hdlr->thread_id = osal_thread_create(pisr_hdlr->name, pisr_hdlr->stack_size, pisr_hdlr->thread_pri, pisr_hdlr->func, pisr_hdlr);
    }

    _rt_util_intr_os_set(isr_id, ENABLED);
    return RT_ERR_OK;
}


/* Function Name:
 *      rt_util_intr_isr_unregister
 * Description:
 *      Unregister swcore ISR callback
 * Input:
 *      isr_id - enum of ISR
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rt_util_intr_isr_unregister(drv_intr_isr_t isr_id)
{
    if (isr_id >= INTR_ISR_END)
    {
        return RT_ERR_INPUT;
    }

    RT_LOG(LOG_INFO, (MOD_RTDRV), "isr_id %u unregister");

    _rt_util_intr_os_set(isr_id, DISABLED);
    rt_util_intr_isrHdlr[isr_id].callback = NULL;
    rt_util_intr_isrHdlr[isr_id].pIsr_param = NULL;
    return RT_ERR_OK;
}




