/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Use to Management link fault detect interrupt
 *
 * Feature : The file have include the following module and sub-modules
 *           1) handle link fault detect interrupt
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/thread.h>
#include <hal/common/halctrl.h>
#include <rtk/default.h>
#include <rtk/oam.h>
#include <osal/wait.h>
#include <dev_config.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#include <ioal/mem32.h>
#include <osal/isr.h>
#include <osal/time.h>

/*
 * Symbol Definition
 */
/* link monitor control block */
typedef struct dal_linkFaultMon_cb_s
{
    osal_thread_t                   thread_id;
    rtk_oam_linkFaultMon_callback_t lfm_cb_f;
} dal_linkFaultMon_cb_t;


/*
 * Data Declaration
 */
static uint32                   linkFaultMon_init;
static osal_sem_t               lfm_sem;
static dal_linkFaultMon_cb_t    *pLinkFaultMon_cb;

static volatile int             lfm_thread_status;
static uint32                   intr_bitmap[RTK_MAX_NUM_OF_UNIT];
static osal_event_t             linkFaultMon_event;


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : */
/* Function Name:
 *      _dal_linkFaultMon_handle_thread
 * Description:
 *      Link fault monitor handle thread
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *
 */
static void _dal_linkFaultMon_handle_thread(void *pInput)
{
    rtk_oam_linkFaultEvent_t    event;
    uint32                      intr_status;
    uint32                      mask;
    int8                        i;
    uint32                      unit = 0;

    while(1)
    {
        osal_wait_event(linkFaultMon_event);

        if (pLinkFaultMon_cb->lfm_cb_f != NULL)
        {
            HWP_UNIT_TRAVS_LOCAL(unit){
                if (0 != intr_bitmap[unit])
                {
                    intr_status = intr_bitmap[unit];
                    intr_bitmap[unit] &= !intr_status;

                    i = 0;
                    mask = 1;
                    do
                    {
                        if (intr_status & mask)
                        {
                            event.instance = i / RTK_CFM_CCM_PORT_MAX;
                            event.instancePort = i % RTK_CFM_CCM_PORT_MAX;
                            (pLinkFaultMon_cb->lfm_cb_f)(&event);
                        }
                        mask <<= 1;
                        ++i;
                    } while (i < (RTK_CFM_RX_INSTANCE_MAX * RTK_CFM_CCM_PORT_MAX));
                }
            }
        }

        if (lfm_thread_status)
            break;

    }

    lfm_thread_status = 0;
    /* reset pLinkFaultMon_cb->thread_id to 0 */
    pLinkFaultMon_cb->thread_id = 0;

    osal_thread_exit(0);

    return;
} /* end of _dal_linkFaultMon_handle_thread */

static int32
_dal_linkFaultMon_intr_handler(void *isr_param)
{
    uint32  unit = 0;
    uint32  value;

    HWP_UNIT_TRAVS_LOCAL(unit){
        ioal_mem32_field_read(unit, RTL8390_ISR_MISC_ADDR, \
                RTL8390_ISR_MISC_ISR_CCM_OFFSET, RTL8390_ISR_MISC_ISR_CCM_MASK, &value);
        intr_bitmap[unit] |= value;

        ioal_mem32_field_write(unit, RTL8390_ISR_MISC_ADDR, \
                RTL8390_ISR_MISC_ISR_CCM_OFFSET, RTL8390_ISR_MISC_ISR_CCM_MASK, value);
    }
    osal_wake_up(linkFaultMon_event);

    return RT_ERR_OK;
} /* end of _linkmon_intr_handler */

/* Function Name:
 *      dal_linkFaultMonEnable_set
 * Description:
 *      Set enable status of link fault monitor
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8390
 * Note:
 *      When enable link fault monitor, all CCM interrupt will be callback to upper layer.
 */
int32 dal_linkFaultMonEnable_set(rtk_enable_t enable)
{
    uint32  unit = 0;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "enable=%d", enable);

    /* check Init status */
    RT_INIT_CHK(linkFaultMon_init);

    if (ENABLED == enable)
    {
        lfm_thread_status = 0;

        if (0 == (pLinkFaultMon_cb->thread_id))
        {
            /* create interrupt thread */
            pLinkFaultMon_cb->thread_id = osal_thread_create("LinkFaultMonIsrThread",
                    RTK_DEFAULT_LINK_MON_ISR_STACK_SIZE,
                    RTK_DEFAULT_LINK_MON_ISR_THREAD_PRI,
                    (void *)_dal_linkFaultMon_handle_thread, NULL);

            if (0 == (pLinkFaultMon_cb->thread_id))
            {
                RT_ERR(pLinkFaultMon_cb->thread_id, (MOD_DAL|MOD_OAM), "");
                return RT_ERR_THREAD_CREATE_FAILED;
            }
        }

        value = 0xFFFF;
        ioal_mem32_field_write(unit, RTL8390_IMR_MISC_ADDR, \
                RTL8390_IMR_MISC_IMR_CCM_OFFSET, RTL8390_IMR_MISC_IMR_CCM_MASK, value);

        ioal_mem32_field_write(unit, RTL8390_ISR_MISC_ADDR, \
                RTL8390_ISR_MISC_ISR_CCM_OFFSET, RTL8390_ISR_MISC_ISR_CCM_MASK, value);
    }
    else
    {
        if (0 == pLinkFaultMon_cb->thread_id)
            return RT_ERR_OK;

        value = 0x0;
        ioal_mem32_field_write(unit, RTL8390_IMR_MISC_ADDR, \
                RTL8390_IMR_MISC_IMR_CCM_OFFSET, RTL8390_IMR_MISC_IMR_CCM_MASK, value);

        lfm_thread_status = 1;

        osal_wake_up(linkFaultMon_event);
    }

    return RT_ERR_OK;
} /* end of dal_linkFaultMonEnable_set */

/* Function Name:
 *      dal_linkFaultMon_register
 * Description:
 *      Register callback function for link fault detect notification
 * Input:
 *      linkMon_callback    - callback function for link change
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32 dal_linkFaultMon_register(rtk_oam_linkFaultMon_callback_t callback)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "linkMon_callback=%x", callback);

    /* check Init status */
    RT_INIT_CHK(linkFaultMon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == callback), RT_ERR_NULL_POINTER);

    if (pLinkFaultMon_cb->lfm_cb_f)
        return RT_ERR_FAILED;

    pLinkFaultMon_cb->lfm_cb_f = callback;

    return RT_ERR_OK;
} /* end of dal_linkFaultMon_register */

/* Function Name:
 *      dal_linkFaultMon_unregister
 * Description:
 *      Unregister callback function for link fault detect notification
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
int32 dal_linkFaultMon_unregister(void)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_OAM), "");

    /* check Init status */
    RT_INIT_CHK(linkFaultMon_init);

    pLinkFaultMon_cb->lfm_cb_f = NULL;

    return RT_ERR_OK;
} /* end of dal_linkFaultMon_unregister */

/* Function Name:
 *      dal_linkFaultMon_init
 * Description:
 *      Initial Link Fault Monitor component
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *
 */
int32 dal_linkFaultMon_init(void)
{
    int32   ret;

    /* init value */
    linkFaultMon_init = INIT_NOT_COMPLETED;

    /* allocate memory for control block */
    pLinkFaultMon_cb = osal_alloc(sizeof(dal_linkFaultMon_cb_t));
    if (0 == pLinkFaultMon_cb){
        RT_INIT_ERR(RT_ERR_FAILED, (MOD_INIT|MOD_DAL), "link fault monitor allocate memory failed");
        return RT_ERR_FAILED;
    }

    /* create semaphore for sync, this semaphore is empty in beginning */
    lfm_sem = osal_sem_create(0);
    if (0 == lfm_sem){
        osal_free(pLinkFaultMon_cb);
        RT_INIT_ERR(RT_ERR_FAILED, (MOD_INIT|MOD_DAL), "link fault monitor semaphore create failed");
        return RT_ERR_FAILED;
    }

    osal_memset(pLinkFaultMon_cb, 0, sizeof(dal_linkFaultMon_cb_t));
    lfm_thread_status = 0;

    ret = osal_wait_module_create(&linkFaultMon_event);
    if(ret != RT_ERR_OK)
        return RT_ERR_FAILED;

    /* old style */
    osal_isr_register(RTK_DEV_SWCORE, _dal_linkFaultMon_intr_handler, NULL);

    linkFaultMon_init = INIT_COMPLETED;
    return RT_ERR_OK;
} /* end of dal_linkFaultMon_init */

