/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Use to Management each device
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Initialize system
 *           2) Initialize device
 *           3) Mangement Devices
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
#include <dal/dal_mgmt.h>
#include <dal/dal_mapper.h>
#include <dal/dal_common.h>
#include <dal/dal_linkMon.h>
#include <rtk/trunk.h>
#include <rtk/port.h>
#include <rtk/default.h>
#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
#include <common/util/rt_util_intr.h>
#if defined(CONFIG_SDK_RTL8390)
#include <dal/cypress/dal_cypress_rate.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#endif
#if defined(CONFIG_SDK_RTL8380)
#include <private/drv/swcore/swcore_rtl8380.h>
#endif
#if defined(CONFIG_SDK_RTL9300)
#include <private/drv/swcore/swcore_rtl9300.h>
#endif
#if defined(CONFIG_SDK_RTL9310)
#include <private/drv/swcore/swcore_rtl9310.h>
#endif
#include <ioal/mem32.h>
#include <osal/isr.h>
#include <osal/time.h>
#if defined(CONFIG_SDK_DRIVER_GPIO)
#include <drv/gpio/generalCtrl_gpio.h>
#endif
#include <osal/wait.h>
#include <hal/mac/reg.h>
#endif
#if defined(CONFIG_SDK_RTL8390)
  #include <dal/cypress/dal_cypress_port.h>
#endif
#if defined(CONFIG_SDK_RTL9300)
  #include <dal/longan/dal_longan_port.h>
#endif

#if defined(CONFIG_SDK_RTL9310)
  #include <dal/mango/dal_mango_sds.h>
#endif


/*
 * Symbol Definition
 */
/* link monitor control block */
typedef struct dal_linkMon_cb_s {
    osal_thread_t       polling_thread_id;
    osal_thread_t       interrupt_thread_id;
    uint32              scan_interval_us;
    uint32              stop_polling_thread;
    rtk_bitmap_t        link_change_units[BITMAP_ARRAY_CNT(RTK_MAX_NUM_OF_UNIT)]; /* interrupt usage */
    rtk_portmask_t      link_change_portmask[RTK_MAX_NUM_OF_UNIT]; /* interrupt usage */
    rtk_bitmap_t        link_swScan_units[BITMAP_ARRAY_CNT(RTK_MAX_NUM_OF_UNIT)]; /* polling usage */
    rtk_portmask_t      link_swScan_portmask[RTK_MAX_NUM_OF_UNIT]; /* polling usage */
    rtk_portmask_t      link_status[RTK_MAX_NUM_OF_UNIT];
    uint32              num_of_linkMon_callback_f;
    rtk_port_linkMon_callback_t  linkMon_callback_f[RTK_MAX_NUM_OF_LINKMON_CB];
} dal_linkMon_cb_t;


/*
 * Data Declaration
 */
static uint32   linkMon_init;
static osal_sem_t   link_change_sem;
static osal_sem_t   link_scan_sem;
static dal_linkMon_cb_t     *pLinkMon_cb;

#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
static uint32 sw_intr_status = 0;
static uint32 sw_sisr0 = 0;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
static uint32 sw_sisr1 = 0;
#endif
static osal_event_t linkMon_event;
#endif


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
static int32 _linkmon_intr_handler(void *isr_param);
static int _linkmon_interrupt_handler(void);
static void _dal_linkMon_interrupt_thread(void *pInput);

static void _dal_linkMon_hwScan(void);
static void _dal_linkMon_interrupt_update_port_status(uint32 unit, rtk_port_t port);
#endif
#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
static void _dal_linkMon_polling_thread(void *pInput);
static void _dal_linkMon_swScan(void);
static void _dal_linkMon_polling_update_port_status(uint32 unit, rtk_port_t port);
#endif


/* Module Name : */

/* Function Name:
 *      dal_linkMon_init
 * Description:
 *      Initial Link Monitor component
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
int32 dal_linkMon_init(void)
{
#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    int32   ret;

    ret = osal_wait_module_create(&linkMon_event);
    if(ret != RT_ERR_OK)
        return RT_ERR_FAILED;
#endif


    /* init value */
    linkMon_init = INIT_NOT_COMPLETED;

    /* allocate memory for control block */
    pLinkMon_cb = osal_alloc(sizeof(dal_linkMon_cb_t));

    if (0 == pLinkMon_cb){
        RT_INIT_ERR(RT_ERR_FAILED, (MOD_INIT|MOD_DAL), "link monitor allocate memory failed");
        return RT_ERR_FAILED;
    }

    /* create semaphore for sync, this semaphore is empty in beginning */
    link_change_sem = osal_sem_create(0);

    if (0 == link_change_sem){
        osal_free(pLinkMon_cb);
        RT_INIT_ERR(RT_ERR_FAILED, (MOD_INIT|MOD_DAL), "link monitor semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* create semaphore for sync, this semaphore is empty in beginning */
    link_scan_sem = osal_sem_create(0);
    if (0 == link_scan_sem){
        osal_free(pLinkMon_cb);
        osal_sem_destroy(link_change_sem);
        RT_INIT_ERR(RT_ERR_FAILED, (MOD_INIT|MOD_DAL), "link monitor semaphore create failed");
        return RT_ERR_FAILED;
    }
    osal_sem_give(link_scan_sem);

    osal_memset(pLinkMon_cb, 0, sizeof(dal_linkMon_cb_t));

    linkMon_init = INIT_COMPLETED;

    return RT_ERR_OK;

} /* end of dal_linkMon_init */

/* Function Name:
 *      dal_linkMon_devInit
 * Description:
 *      Initial Link Monitor component for each device
 * Input:
 *      unit            - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *
 */
int32 dal_linkMon_devInit(uint32 unit)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%u", unit);

    /* check Init status */
    RT_INIT_CHK(linkMon_init);

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || NULL == RT_MGMT(unit), RT_ERR_UNIT_ID);

    return ret;

} /* end of dal_linkMon_devInit */


#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
#if defined(CONFIG_SDK_DRIVER_GPIO)
static int32 dal_linkMon_cascadeInterrupt_set(uint32  unit)
{
    drv_generalCtrlGpio_pinConf_t cascade_gpio_pin;
    drv_generalCtrlGpio_devConf_t cascade_gpio_dev;
    int32	ret;

    if(HWP_CASCADE_MODE() == TRUE) {
        ret = drv_generalCtrlGPIO_dev_init(unit, GEN_GPIO_DEV_ID0_INTERNAL, &cascade_gpio_dev);
        cascade_gpio_pin.default_value = 1;
        cascade_gpio_pin.direction = GPIO_DIR_IN;
        cascade_gpio_pin.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
        cascade_gpio_pin.int_gpio.interruptEnable = GPIO_INT_FALLING_EDGE;
        drv_generalCtrlGPIO_devEnable_set(unit, GEN_GPIO_DEV_ID0_INTERNAL, ENABLED);
        ret = drv_generalCtrlGPIO_pin_init(unit, GEN_GPIO_DEV_ID0_INTERNAL, HWP_CASCADE_SLAVE_INTR_PIN(), &cascade_gpio_pin);
        ret = drv_generalCtrlGPIO_intrHandler_register(unit,GEN_GPIO_DEV_ID0_INTERNAL, HWP_CASCADE_SLAVE_INTR_PIN(), _linkmon_intr_handler);
    }
    return ret;
}
#endif /* defined(CONFIG_SDK_DRIVER_GPIO) */
#endif /* defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) */
#endif


/* Function Name:
 *      dal_linkMon_enable
 * Description:
 *      Enable link monitor thread
 * Input:
 *      scan_interval_us        - scan interval in us.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_THREAD_EXIST - The LinkMon thread is created already, and it's work.
 *      RT_ERR_THREAD_CREATE_FAILED - The LinkMon thread created failed.
 * Note:
 *      When enable link monitor thread, all link change interrupt will be handled by thread.
 *
 */
int32 dal_linkMon_enable(uint32 scan_interval_us)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "scan_interval_us=%u",
           scan_interval_us);

    /* check Init status */
    RT_INIT_CHK(linkMon_init);

#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    /* parameter check */
    RT_PARAM_CHK((scan_interval_us < RTK_LINKMON_SCAN_INTERVAL_MIN), RT_ERR_OUT_OF_RANGE);

    pLinkMon_cb->scan_interval_us = scan_interval_us;

    if ((pLinkMon_cb->polling_thread_id) != 0)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "linkMon thread exist: id = %d", pLinkMon_cb->polling_thread_id);
        return RT_ERR_THREAD_EXIST;
    }

    /* create polling thread */
    pLinkMon_cb->polling_thread_id = osal_thread_create("LinkMonPollThread", RTK_DEFAULT_LINK_MON_POLLING_STACK_SIZE, RTK_DEFAULT_LINK_MON_POLLING_THREAD_PRI
                            , (void *)_dal_linkMon_polling_thread, NULL);

    if (0 == (pLinkMon_cb->polling_thread_id))
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "linkMon thread create failed: id = %d", pLinkMon_cb->polling_thread_id);
        return RT_ERR_THREAD_CREATE_FAILED;
    }
#endif

#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    if ((pLinkMon_cb->interrupt_thread_id) != 0)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "linkMon thread exist: id = %d", pLinkMon_cb->interrupt_thread_id);
        return RT_ERR_THREAD_EXIST;
    }

    /* create interrupt thread */
    pLinkMon_cb->interrupt_thread_id = osal_thread_create("LinkMonIsrThread", RTK_DEFAULT_LINK_MON_ISR_STACK_SIZE, RTK_DEFAULT_LINK_MON_ISR_THREAD_PRI
                            , (void *)_dal_linkMon_interrupt_thread, NULL);

    if (0 == (pLinkMon_cb->interrupt_thread_id))
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "linkMon thread create failed: id = %d", pLinkMon_cb->polling_thread_id);
        return RT_ERR_THREAD_CREATE_FAILED;
    }
#endif

    return RT_ERR_OK;
} /* end of dal_linkMon_enable */

/* Function Name:
 *      dal_linkMon_disable
 * Description:
 *      Disable link scan thread
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      When disable link monitor thread, all link change interrupt will be callback to upper layer.
 */
int32 dal_linkMon_disable(void)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "");

    /* check Init status */
    RT_INIT_CHK(linkMon_init);

    /* parameter check */

    /* reset scan_interval_us to 0, thread will suicide after finish all waiting job */
    pLinkMon_cb->scan_interval_us = 0;

    /* let thread continue */
    osal_sem_give(link_change_sem);

#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    /* reset pLinkMon_cb->polling_thread_id to 0 */
    pLinkMon_cb->polling_thread_id = 0;
#endif

#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    /* reset pLinkMon_cb->interrupt_thread_id to 0 */
    pLinkMon_cb->interrupt_thread_id = 0;
#endif

    return RT_ERR_OK;

} /* end of dal_linkMon_disable */

/* Function Name:
 *      dal_linkMonPolling_stop
 * Description:
 *      Make linkmon polling thread stop/resume polling port status.
 *      polling.
 * Input:
 *      stop_bool - TRUE:stop / FALSE:resume
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - success
 *      RT_ERR_FAILED   - fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      A switch may have multiple units but only one linkmon module.
 *      This API takes effects on the linkmon polling thread.
 *
 */
int32 dal_linkMonPolling_stop(uint32 unit, uint32 stop_bool)
{
    if(pLinkMon_cb == NULL)
    {
        return RT_ERR_FAILED;
    }

    pLinkMon_cb->stop_polling_thread = (FALSE==stop_bool)?FALSE:TRUE;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_linkMon_register
 * Description:
 *      Register callback function for link change notification
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
int32 dal_linkMon_register(rtk_port_linkMon_callback_t linkMon_callback)
{
#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    uint32  unit = 0;
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    uint32  value;
#endif
#endif
    uint32  i, flag_find_available = 0, available_index = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "linkMon_callback=%x",
           linkMon_callback);
#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    unit = HWP_MY_UNIT_ID();
#endif

    /* check Init status */
    RT_INIT_CHK(linkMon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == linkMon_callback), RT_ERR_NULL_POINTER);

    /* check the callback function available index and check CB already exist or not? */
    for (i = 0; i < RTK_MAX_NUM_OF_LINKMON_CB; i++)
    {
        if (pLinkMon_cb->linkMon_callback_f[i] == NULL && flag_find_available == 0)
        {
            flag_find_available = 1;
            available_index = i;
        }
        if ((pLinkMon_cb->linkMon_callback_f[i] != NULL) && (pLinkMon_cb->linkMon_callback_f[i] == linkMon_callback))
            return RT_ERR_CB_FUNCTION_EXIST;
    }

    if (flag_find_available)
        pLinkMon_cb->linkMon_callback_f[available_index] = linkMon_callback;
    else
        return RT_ERR_CB_FUNCTION_FULL;

    if (pLinkMon_cb->num_of_linkMon_callback_f == 0)
    {   /* Do this only when first CB register */
#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
        rt_util_intr_isr_register(INTR_ISR_PORT_LINK_CHG, _linkmon_intr_handler, NULL);
  #if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
  #if defined(CONFIG_SDK_DRIVER_GPIO)
        dal_linkMon_cascadeInterrupt_set(unit);
  #endif /* defined(CONFIG_SDK_DRIVER_GPIO) */
  #endif

        HWP_UNIT_TRAVS_LOCAL(unit){
  #if defined(CONFIG_SDK_RTL8390)
            if (HWP_8390_50_FAMILY(unit))
            {
                ioal_mem32_read(unit, RTL8390_ISR_GLB_SRC_ADDR, &value);
                value |= RTL8390_ISR_GLB_SRC_ISR_GLB_LINK_CHG_MASK;
                ioal_mem32_write(unit, RTL8390_ISR_GLB_SRC_ADDR, value);
                ioal_mem32_write(unit, RTL8390_IMR_PORT_LINK_STS_CHG_ADDR(0), 0xFFFFFFFF);
                ioal_mem32_write(unit, RTL8390_IMR_PORT_LINK_STS_CHG_ADDR(32), 0xFFFFF);
            }
  #endif
  #if defined(CONFIG_SDK_RTL8380)
            if (HWP_8380_30_FAMILY(unit))
            {
                ioal_mem32_read(unit, RTL8380_ISR_GLB_SRC_ADDR, &value);
                value |= RTL8380_ISR_GLB_SRC_ISR_GLB_LINK_CHG_MASK;
                ioal_mem32_write(unit, RTL8380_ISR_GLB_SRC_ADDR, value);
                ioal_mem32_write(unit, RTL8380_ISR_PORT_LINK_STS_CHG_ADDR, 0x0FFFFFFF);
                ioal_mem32_write(unit, RTL8380_IMR_PORT_LINK_STS_CHG_ADDR, 0x0FFFFFFF);
                ioal_mem32_write(unit, RTL8380_IMR_GLB_ADDR, 0x1);
            }
  #endif
  #if defined(CONFIG_SDK_RTL9300)
            if (HWP_9300_FAMILY_ID(unit))
            {
                ioal_mem32_write(unit, RTL9300_ISR_PORT_LINK_STS_CHG_ADDR, 0x0FFFFFFF);
                ioal_mem32_write(unit, RTL9300_IMR_PORT_LINK_STS_CHG_ADDR, 0x0FFFFFFF);
                ioal_mem32_write(unit, RTL9300_IMR_GLB_ADDR, 0x1);
            }
  #endif
  #if defined(CONFIG_SDK_RTL9310)
            if (HWP_9310_FAMILY_ID(unit))
            {
                ioal_mem32_write(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ADDR(0), 0xFFFFFFFF);
                ioal_mem32_write(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ADDR(32), 0x00FFFFFF);
                ioal_mem32_write(unit, RTL9310_IMR_PORT_LINK_STS_CHG_ADDR(0), 0xFFFFFFFF);
                ioal_mem32_write(unit, RTL9310_IMR_PORT_LINK_STS_CHG_ADDR(32), 0x00FFFFFF);
            }
  #endif
        }
#endif
    }
    pLinkMon_cb->num_of_linkMon_callback_f++;

    return RT_ERR_OK;
} /* end of dal_linkMon_register */

/* Function Name:
 *      dal_linkMon_unregister
 * Description:
 *      Unregister callback function for link change notification
 * Input:
 *      linkMon_callback    - callback function for link change
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
int32 dal_linkMon_unregister(rtk_port_linkMon_callback_t linkMon_callback)
{
    uint32  i, flag_find_exist = 0, exist_index = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "");

    /* check Init status */
    RT_INIT_CHK(linkMon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == linkMon_callback), RT_ERR_NULL_POINTER);

    /* find the exist callback function */
    for (i = 0; i < RTK_MAX_NUM_OF_LINKMON_CB; i++)
    {
        if ((pLinkMon_cb->linkMon_callback_f[i] != NULL) && (pLinkMon_cb->linkMon_callback_f[i] == linkMon_callback))
        {
            flag_find_exist = 1;
            exist_index = i;
        }
    }

    if (flag_find_exist)
        pLinkMon_cb->linkMon_callback_f[exist_index] = NULL;
    else
        return RT_ERR_CB_FUNCTION_NOT_FOUND;

    pLinkMon_cb->num_of_linkMon_callback_f--;

    return RT_ERR_OK;
} /* end of dal_linkMon_unregister */

/* Function Name:
 *      dal_linkMon_swScanPorts_set
 * Description:
 *      Configure portmask of software linkscan for certain unit
 * Input:
 *      unit                - callback function for link change
 *      pSwScan_portmask    - portmask for software scan
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - initialize success
 *      RT_ERR_FAILED       - initialize fail
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32 dal_linkMon_swScanPorts_set(uint32 unit, rtk_portmask_t *pSwScan_portmask)
{
    /* check Init status */
    RT_INIT_CHK(linkMon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSwScan_portmask), RT_ERR_NULL_POINTER);

    if (0 == RTK_PORTMASK_GET_PORT_COUNT(*pSwScan_portmask))
    {
        BITMAP_CLEAR(pLinkMon_cb->link_swScan_units, unit);
    }
    else
    {
        BITMAP_SET(pLinkMon_cb->link_swScan_units, unit);
    }

    RTK_PORTMASK_ASSIGN(pLinkMon_cb->link_swScan_portmask[unit], *pSwScan_portmask);

    return RT_ERR_OK;
} /* end of dal_linkMon_swScanPorts_set */

/* Function Name:
 *      dal_linkMon_swScanPorts_get
 * Description:
 *      Get portmask of software linkscan for certain unit
 * Input:
 *      unit                - callback function for link change
 * Output:
 *      pSwScan_portmask    - portmask for software scan
 * Return:
 *      RT_ERR_OK           - initialize success
 *      RT_ERR_FAILED       - initialize fail
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32 dal_linkMon_swScanPorts_get(uint32 unit, rtk_portmask_t *pSwScan_portmask)
{
    /* check Init status */
    RT_INIT_CHK(linkMon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSwScan_portmask), RT_ERR_NULL_POINTER);

    RTK_PORTMASK_ASSIGN(*pSwScan_portmask, pLinkMon_cb->link_swScan_portmask[unit]);

    return RT_ERR_OK;
} /* end of dal_linkMon_swScanPorts_get */


#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
/* Function Name:
 *      _dal_linkMon_polling_thread
 * Description:
 *      Link monitor polling thread
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
static void _dal_linkMon_polling_thread(void *pInput)
{
    int32   ret;

    /* forever loop */
    while (pLinkMon_cb->scan_interval_us != 0)
    {

        /* wait semaphore for link scan interval */
        ret = osal_sem_take(link_change_sem, pLinkMon_cb->scan_interval_us);

        if(pLinkMon_cb->stop_polling_thread == TRUE)
        {
            continue;
        }

        /* Link Scan Part */
        if (RT_ERR_OK == ret)
        {/* if take semaphore, mean signal from hardware */
#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
            _dal_linkMon_hwScan();
#endif
        }
        else
        {
            /* todo: need to reduce scan interval */
            _dal_linkMon_swScan();
        }
    }

    osal_thread_exit(0);

    return;
} /* end of _dal_linkMon_polling_thread */
#endif


#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
/* Function Name:
 *      _dal_linkMon_interrupt_thread
 * Description:
 *      Link monitor interrupt thread
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
static void _dal_linkMon_interrupt_thread(void *pInput)
{
    while(1)
    {

        osal_wait_event(linkMon_event);

        _linkmon_interrupt_handler();
    }
    return;
} /* end of _dal_linkMon_interrupt_thread */
#endif


#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
/* Function Name:
 *      _dal_linkMon_hwScan
 * Description:
 *      Unregister callback function for link change notification
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      None.
 * Note:
 *
 */
static void _dal_linkMon_hwScan(void)
{
    uint32  unit;
    uint32  port;

    /* scan each changed port */
    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        if (!BITMAP_IS_SET(pLinkMon_cb->link_change_units, unit))
        {/* if this unit is not changed, continue */
            continue;
        }


        /* already process this unit, clear link change units */
        BITMAP_CLEAR(pLinkMon_cb->link_change_units, unit);

        osal_sem_take(link_scan_sem, OSAL_SEM_WAIT_FOREVER);
        /* update port status in changed portmask */
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        {
            if (RTK_PORTMASK_IS_PORT_SET(pLinkMon_cb->link_change_portmask[unit], port))
            {/* only update changed portmask */
                RTK_PORTMASK_PORT_CLEAR(pLinkMon_cb->link_change_portmask[unit], port);
                _dal_linkMon_interrupt_update_port_status(unit, port);
            }
        }
        osal_sem_give(link_scan_sem);
    }
} /* end of _dal_linkMon_hwScan */
#endif

#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
/* Function Name:
 *      _dal_linkMon_swScan
 * Description:
 *      Unregister callback function for link change notification
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
static void _dal_linkMon_swScan(void)
{
    uint32  unit;
    uint32  port;

    /* todo: need to skip disabled port */
    /* scan each changed port */
    for (unit = 0; unit <= RTK_MAX_UNIT_ID; unit++)
    {
        if (!BITMAP_IS_SET(pLinkMon_cb->link_swScan_units, unit))
        {/* if this unit is not changed, continue */
            continue;
        }
        osal_sem_take(link_scan_sem, OSAL_SEM_WAIT_FOREVER);
        /* update port status in sw scan portmask */
        HWP_PORT_TRAVS(unit, port)
        {
            if (RTK_PORTMASK_IS_PORT_SET(pLinkMon_cb->link_swScan_portmask[unit], port))
            {/* only update software scan portmask */
                _dal_linkMon_polling_update_port_status(unit, port);
            }
        }
        osal_sem_give(link_scan_sem);
    }
} /* end of _dal_linkMon_swScan */
#endif


/* Function Name:
 *      dal_linkMon_swMacPollPhy
 * Description:
 *      software MAC Poll PHY
 * Input:
 *      unit - unit ID
 *      port - port ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
dal_linkMon_swMacPollPhy(uint32 unit, rtk_port_t port)
{
    #if defined(CONFIG_SDK_RTL8390)
    if (HWP_CHIP_ID(unit) == RTL8396M_CHIP_ID)
    {
        _dal_cypress_port_swMacPollPhy(unit, port);
    }
    #endif
    #if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        _dal_longan_port_swMacPollPhy(unit, port);
    }
    #endif
}

/* Function Name:
 *      dal_linkMon_swChgSerdesMode
 * Description:
 *      software Change serdes mode
 * Input:
 *      unit - unit ID
 *      port - port ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
dal_linkMon_swChgSerdesMode(uint32 unit, rtk_port_t port)
{
    #if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        _dal_longan_port_swChgSerdesMode(unit, port);
    }
    #endif
    #if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        dal_mango_sds_swChgSerdesMode(unit, port);
    }
    #endif
}


#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
/* Function Name:
 *      _dal_linkMon_polling_update_port_status
 * Description:
 *      Update port status and callback to upper layer
 * Input:
 *      unit            - the unit need to be updated
 *      port            - the port need to be updated
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *
 */
static void
_dal_linkMon_polling_update_port_status(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  data = 0, i;
    rtk_port_linkStatus_t  new_link, current_link;

    /* Software MAC Poll PHY */
    dal_linkMon_swMacPollPhy(unit, port);
    /* Software change serdes mode */
    dal_linkMon_swChgSerdesMode(unit, port);

    /* get newest port status */
    if ((ret = rtk_port_link_get(unit, port, &data)) != RT_ERR_OK)
    {
        return;
    }
    new_link = data;

    /* get current link status */
    current_link = (RTK_PORTMASK_IS_PORT_SET(pLinkMon_cb->link_status[unit], port))? PORT_LINKUP: PORT_LINKDOWN;

    if (current_link != new_link )
    {
        if (PORT_LINKUP == new_link)
        {
#if defined(CONFIG_SDK_WA_88E6063_COMPATIBLE)
            dal_esw_port_FeNway10MCompatible88E6063_workaround(unit, port, PORT_LINKUP);
#endif
            RTK_PORTMASK_PORT_SET(pLinkMon_cb->link_status[unit], port);
        }
        else
        {
#if defined(CONFIG_SDK_WA_88E6063_COMPATIBLE)
            dal_esw_port_FeNway10MCompatible88E6063_workaround(unit, port, PORT_LINKDOWN);
#endif
            RTK_PORTMASK_PORT_CLEAR(pLinkMon_cb->link_status[unit], port);
        }

#if defined(CONFIG_TRUNK_FAILOVER_HANDLING)
        rtk_trunk_port_link_notification(unit, port, new_link);
#endif

        for (i = 0; i < RTK_MAX_NUM_OF_LINKMON_CB; i++)
        {
            if (pLinkMon_cb->linkMon_callback_f[i] != NULL)
            {
                (pLinkMon_cb->linkMon_callback_f[i])(unit, port, new_link);
            }
        }
    }

    return;
} /* end of _dal_polling_linkMon_update_port_status */
#endif

#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
/* Function Name:
 *      _dal_linkMon_interrupt_update_port_status
 * Description:
 *      Update port status and callback to upper layer by interrupt thread
 * Input:
 *      unit            - the unit need to be updated
 *      port            - the port need to be updated
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *
 */
static void
_dal_linkMon_interrupt_update_port_status(uint32 unit, rtk_port_t port)
{
    int32   ret = 0;
    uint32  data = 0, i;
    rtk_port_linkStatus_t  new_link, current_link;


    /* get newest port status */
    if ((ret = rtk_port_link_get(unit, port, &data)) != RT_ERR_OK)
    {
        return;
    }
    new_link = data;

    /* get current link status */
    current_link = (RTK_PORTMASK_IS_PORT_SET(pLinkMon_cb->link_status[unit], port))? PORT_LINKUP: PORT_LINKDOWN;

    if (current_link != new_link )
    {
        if (PORT_LINKUP == new_link)
        {
            RTK_PORTMASK_PORT_SET(pLinkMon_cb->link_status[unit], port);
        }
        else
        {
            RTK_PORTMASK_PORT_CLEAR(pLinkMon_cb->link_status[unit], port);
        }

        for (i = 0; i < RTK_MAX_NUM_OF_LINKMON_CB; i++)
        {
            if (pLinkMon_cb->linkMon_callback_f[i] != NULL)
            {
                (pLinkMon_cb->linkMon_callback_f[i])(unit, port, new_link);
            }
        }
    }
    else
    {
        /* Process the per-port link change interrupt status is ON, but get link status is not changed case.
         * It is possible due to link change happens twice times, like UP->DOWN->UP or DOWN->UP->DOWN.
         * In this condition, need to callback upper register function twice time.
         */
        if (PORT_LINKUP == new_link)
        {
            RTK_PORTMASK_PORT_SET(pLinkMon_cb->link_status[unit], port);
            /* notification PORT_LINKDOWN */
            for (i = 0; i < RTK_MAX_NUM_OF_LINKMON_CB; i++)
            {
                if (pLinkMon_cb->linkMon_callback_f[i] != NULL)
                {
                    (pLinkMon_cb->linkMon_callback_f[i])(unit, port, PORT_LINKDOWN);
                }
            }
            for (i = 0; i < RTK_MAX_NUM_OF_LINKMON_CB; i++)
            {
                if (pLinkMon_cb->linkMon_callback_f[i] != NULL)
                {
                    (pLinkMon_cb->linkMon_callback_f[i])(unit, port, new_link);
                }
            }
        }
        else
        {
            RTK_PORTMASK_PORT_CLEAR(pLinkMon_cb->link_status[unit], port);

            for (i = 0; i < RTK_MAX_NUM_OF_LINKMON_CB; i++)
            {
                if (pLinkMon_cb->linkMon_callback_f[i] != NULL)
                {
                    (pLinkMon_cb->linkMon_callback_f[i])(unit, port, PORT_LINKUP);
                }
            }

            for (i = 0; i < RTK_MAX_NUM_OF_LINKMON_CB; i++)
            {
                if (pLinkMon_cb->linkMon_callback_f[i] != NULL)
                {
                    (pLinkMon_cb->linkMon_callback_f[i])(unit, port, new_link);
                }
            }
        }
    }

    return;
} /* end of _dal_linkMon_interrupt_update_port_status */
#endif


#if defined(CONFIG_SDK_LINKMON_ISR_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
static int _linkmon_interrupt_handler(void)
{
    _dal_linkMon_hwScan();
    return RT_ERR_OK;
}

static int32
_linkmon_intr_handler(void *isr_param)
{
    uint32  unit = 0;

    HWP_UNIT_TRAVS_LOCAL(unit){
#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8390_50_FAMILY(unit))
    {
        rtk_portmask_t  linkChange_portmask;
        ioal_mem32_read(unit, RTL8390_ISR_GLB_SRC_ADDR, &sw_intr_status);

        ioal_mem32_read(unit, RTL8390_ISR_PORT_LINK_STS_CHG_ADDR(0),  &sw_sisr0);
        ioal_mem32_read(unit, RTL8390_ISR_PORT_LINK_STS_CHG_ADDR(32), &sw_sisr1);
        osal_memset(&linkChange_portmask, 0, sizeof(rtk_portmask_t));
        linkChange_portmask.bits[0] = (sw_sisr0 & 0xffffffff);
        linkChange_portmask.bits[1] = (sw_sisr1 & 0xffffffff);
        RTK_PORTMASK_OR(pLinkMon_cb->link_change_portmask[unit], linkChange_portmask);
        BITMAP_SET(pLinkMon_cb->link_change_units, unit);
        /* clear status */
        ioal_mem32_write(unit, RTL8390_ISR_PORT_LINK_STS_CHG_ADDR(0), sw_sisr0);
        ioal_mem32_write(unit, RTL8390_ISR_PORT_LINK_STS_CHG_ADDR(32), sw_sisr1);
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8380_30_FAMILY(unit))
    {
        rtk_portmask_t  linkChange_portmask;
        ioal_mem32_read(unit, RTL8380_ISR_GLB_SRC_ADDR, &sw_intr_status);

        ioal_mem32_read(unit, RTL8380_ISR_PORT_LINK_STS_CHG_ADDR, &sw_sisr0);
        osal_memset(&linkChange_portmask, 0, sizeof(rtk_portmask_t));
        linkChange_portmask.bits[0] = (sw_sisr0 & 0x0fffffff);
        RTK_PORTMASK_OR(pLinkMon_cb->link_change_portmask[unit], linkChange_portmask);
        BITMAP_SET(pLinkMon_cb->link_change_units, unit);
        /* clear status */
        ioal_mem32_write(unit, RTL8380_ISR_PORT_LINK_STS_CHG_ADDR, sw_sisr0);
    }
#endif
#if defined(CONFIG_SDK_RTL9300)
        if (HWP_9300_FAMILY_ID(unit))
        {
            rtk_portmask_t  linkChange_portmask;
            ioal_mem32_read(unit, RTL9300_ISR_GLB_ADDR, &sw_intr_status);

            ioal_mem32_read(unit, RTL9300_ISR_PORT_LINK_STS_CHG_ADDR, &sw_sisr0);
            osal_memset(&linkChange_portmask, 0, sizeof(rtk_portmask_t));
            linkChange_portmask.bits[0] = (sw_sisr0 & 0x0fffffff);
            RTK_PORTMASK_OR(pLinkMon_cb->link_change_portmask[unit], linkChange_portmask);
            BITMAP_SET(pLinkMon_cb->link_change_units, unit);
            /* clear status */
            ioal_mem32_write(unit, RTL9300_ISR_PORT_LINK_STS_CHG_ADDR, sw_sisr0);
        }
#endif
#if defined(CONFIG_SDK_RTL9310)
        if (HWP_9310_FAMILY_ID(unit))
        {
            rtk_portmask_t  linkChange_portmask;
            ioal_mem32_read(unit, RTL9310_ISR_GLB_SRC_STS_ADDR, &sw_intr_status);

            ioal_mem32_read(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ADDR(0),  &sw_sisr0);
            ioal_mem32_read(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ADDR(32), &sw_sisr1);
            osal_memset(&linkChange_portmask, 0, sizeof(rtk_portmask_t));
            linkChange_portmask.bits[0] = (sw_sisr0 & 0xFFFFFFFF);
            linkChange_portmask.bits[1] = (sw_sisr1 & 0x00FFFFFF);
            RTK_PORTMASK_OR(pLinkMon_cb->link_change_portmask[unit], linkChange_portmask);
            BITMAP_SET(pLinkMon_cb->link_change_units, unit);
            /* clear status */
            ioal_mem32_write(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ADDR(0), sw_sisr0);
            ioal_mem32_write(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ADDR(32), sw_sisr1);
        }
#endif
    }
    osal_wake_up(linkMon_event);

    return RT_ERR_OK;
} /* end of _linkmon_intr_handler */
#endif

