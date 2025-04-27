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
#include <rtk/default.h>
#include <dal/dal_phy.h>
#include <dal/dal_waMon.h>
#include <hal/common/halctrl.h>

#ifdef PHY_ONLY
#include <rtk/port.h>
#include <hwp/hw_profile.h>
#include <hal/common/miim.h>
#include <osal/thread.h>
#include <osal/sem.h>
#else /*not define PHY_ONLY*/
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/thread.h>

#include <dal/dal_common.h>

#if defined(CONFIG_SDK_RTL8380)
#include <dal/maple/dal_maple_led.h>
#include <dal/maple/dal_maple_waFunc.h>
#include <dal/maple/dal_maple_rate.h>
#endif
#if defined(CONFIG_SDK_RTL8390)
#include <dal/cypress/dal_cypress_waFunc.h>
#include <dal/cypress/dal_cypress_port.h>
#endif
#if defined(CONFIG_SDK_RTL9300)
#include <dal/longan/dal_longan_waFunc.h>
#endif
#include <hal/common/miim.h>

#include <dal/dal_linkMon.h>

#include <hal/mac/reg.h>
#if defined(CONFIG_SDK_RTL9310)
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <dal/mango/dal_mango_stack.h>
#include <dal/mango/dal_mango_waFunc.h>
#include <dal/mango/dal_mango_sds.h>
#endif

#if defined(CONFIG_SDK_DRIVER_L2NTFY) && defined(CONFIG_SDK_L2_NTFY_PG_THREAD)
#include <drv/l2ntfy/l2ntfy.h>
#include <dal/mango/dal_mango_l2.h>
#endif

#if defined(CONFIG_SFLOW_PG_THREAD)
#include <dal/mango/dal_mango_mirror.h>
#endif

#if defined(CONFIG_SDK_WA_SERDES_FIBER_LINKDOWN_WATCHDOG)
#include <hal/phy/phy_rtl8390.h>
#endif

#if defined(CONFIG_SDK_RTL8295R)
#include <dal/longan/dal_longan_sds.h>
#endif

#endif /* PHY_ONLY */


/*
 * Symbol Definition
 */
#ifndef PHY_ONLY
#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    #define CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS              1
#endif
#if defined(CONFIG_SDK_L2_NTFY_PG_THREAD) || defined(CONFIG_SFLOW_PG_THREAD)
#define         PG_TICK                             1000
#endif

#if defined(CONFIG_SDK_L2_NTFY_PG_THREAD)
#define         L2_NTFY_PG_DISABLE_LRN_AGE_THR_HI   200
#define         L2_NTFY_PG_DISABLE_LRN_AGE_THR_LO   4
#endif

#if defined(CONFIG_SFLOW_PG_THREAD)
#define         CPU_MONITORING_QUEUE_NUM            32
#endif
#endif

/* workaround monitor control block */
#define WAMON_NAME_LEN      16
typedef void (*waMon_function)(void *);

typedef struct dal_waMon_cb_s {
    osal_thread_t       thread_id;
    uint32              scan_interval_us;
    waMon_function      waMon_func;
    osal_sem_t          wa_change_sem;
    char                name[WAMON_NAME_LEN];
} dal_waMon_cb_t;


/*
 * Function Declaration
 */

static void _dal_waMon1000_thread(void *pInput);

#ifndef PHY_ONLY
static void _dal_waMon500_thread(void *pInput);
#if defined(CONFIG_SDK_RTL9310) && ((defined(CONFIG_SDK_DRIVER_L2NTFY) && defined(CONFIG_SDK_L2_NTFY_PG_THREAD)) || defined(CONFIG_SFLOW_PG_THREAD))
static void _dal_waMonPgMon_thread(void *pInput);
#endif
uint32 _dal_waMon_enableCfg_chk(void);

#if defined(CONFIG_SDK_RTL8295R)
void dal_waMon_8295r_clkRecover_watchdog(uint32 unit);
#endif/* CONFIG_SDK_RTL8295R */

#endif /* ifndef PHY_ONLY */

/*
 * Data Declaration
 */

uint32   waMon_init;
static dal_waMon_cb_t   pWaMon_cb[]=
    {
#ifndef PHY_ONLY
        {0, 500000, _dal_waMon500_thread, 0, "waMon 500" },
#if defined(CONFIG_SDK_RTL9310) && ((defined(CONFIG_SDK_DRIVER_L2NTFY) && defined(CONFIG_SDK_L2_NTFY_PG_THREAD)) || defined(CONFIG_SFLOW_PG_THREAD))
        {0, PG_TICK, _dal_waMonPgMon_thread, 0, "waMon pgMon" },
#endif
#endif /* ifndef PHY_ONLY */
        {0,1000000,_dal_waMon1000_thread, 0, "waMon 1000"},
    };

rtk_port_phyReconfig_callback_t pPhyReconfig_cb = NULL;
uint32 waMon_enable = ENABLED;
uint32 pktBuf_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};
uint32 macSerdes_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};
uint32 phySerdes_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};
uint32 phy_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};
uint32 fiber_rx_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};


#ifndef PHY_ONLY
uint32 phy95rClkRecov_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};
uint32 phy95rClkRecovOK_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};
uint32 drainTx_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};
uint32 drainTxPathHasPkt_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};
uint32 drainTxQFailed_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};
uint32 syncPhyToMacFailed_watchdog_cnt[RTK_MAX_NUM_OF_UNIT]={0};
#endif /* ifndef PHY_ONLY */

/* Indicate PHY was reset by ESD. PHY's base mac ID will be set to 1 */
rtk_portmask_t dal_waMon_phyEsdRstEvn[RTK_MAX_NUM_OF_UNIT];

#ifndef PHY_ONLY
#if defined(CONFIG_SDK_RTL8295R)
rtk_portmask_t dal_waMon_95rClkRecovChkFlag[RTK_MAX_NUM_OF_UNIT];
uint8 dal_waMon_95rClkRecovChkRetryCnt[RTK_MAX_NUM_OF_UNIT][RTK_MAX_PORT_PER_UNIT];
rtk_portmask_t dal_waMon_95rClkRstFlag[RTK_MAX_NUM_OF_UNIT];
rtk_port_phySdsRxCaliStatus_t dal_waMon_phySdsCaliStatus[RTK_MAX_NUM_OF_UNIT][RTK_MAX_PORT_PER_UNIT];
#endif

#if defined(CONFIG_SDK_RTL9310) && defined(CONFIG_SDK_DRIVER_L2NTFY) && defined(CONFIG_SDK_L2_NTFY_PG_THREAD)
static uint32                           gNtfy_suppressed = 0;
#endif
#if defined(CONFIG_SDK_RTL9310) && defined(CONFIG_SFLOW_PG_THREAD)
sflowStsChg_cb_f                        sflowStsChg_cb = NULL;
static uint32                           gSflow_suppressed = 0;
static uint32                           gPG_mon_que[]     = {0, 1};

static uint32                           sflow_thr_hi_offset[] = {20, 20, 20, 20, 20, 20, 20, 20,
                                                                 20, 20, 20, 20, 20, 20, 20, 20,
                                                                 20, 20, 20, 20, 20, 20, 20, 20,
                                                                 20, 20, 20, 20, 20, 20, 20, 20};
static uint32                           sflow_thr_lo_offset[] = {0, 0, 0, 0, 0, 0, 0, 0,
                                                                 0, 0, 0, 0, 0, 0, 0, 0,
                                                                 0, 0, 0, 0, 0, 0, 0, 0,
                                                                 0, 0, 0, 0, 0, 0, 0, 0};
static uint32                           que_drop_thr_hi[] = {0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e,
                                                             0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e,
                                                             0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e,
                                                             0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e};
static uint32                           que_drop_thr_lo[] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
                                                             0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
                                                             0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
                                                             0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40};
#endif

#if defined(CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS)
  rtk_portmask_t        waMonLinkChang[RTK_MAX_NUM_OF_UNIT];     /* 0: no change; 1: link changed */
  rtk_portmask_t        waMonLinkStat[RTK_MAX_NUM_OF_UNIT];      /* 0: down; 1: up */
  static osal_mutex_t   waMon_linkSem;

  #define WAMON_LINK_SEM_LOCK()    \
    do {\
        if (osal_sem_mutex_take(waMon_linkSem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
        {\
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_PORT), "semaphore lock failed");\
            return RT_ERR_SEM_LOCK_FAILED;\
        }\
    } while(0)


  #define WAMON_LINK_SEM_UNLOCK()   \
    do {\
        if (osal_sem_mutex_give(waMon_linkSem) != RT_ERR_OK)\
        {\
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_PORT), "semaphore unlock failed");\
            return RT_ERR_SEM_UNLOCK_FAILED;\
        }\
    } while(0)
#endif/* CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS */

#endif /* ifndef PHY_ONLY */


/*
 * Macro Declaration
 */

#ifndef PHY_ONLY
#if defined(CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS)
  #if defined(CONFIG_SDK_RTL8390)
    extern int32 dal_cypress_rate_egrBandwidthCtrlLinkChange_handler(int32 unit, rtk_port_t port, rtk_port_linkStatus_t new_linkStatus);
  #endif
  #if defined(CONFIG_SDK_RTL9310)
    extern int32 dal_mango_rate_portEgrQueueDrain_process(int32 unit, rtk_port_t port);
    extern int32 dal_mango_portMacSyncPhySts_process(int32 unit, rtk_port_t port);
    extern int32 _dal_mango_flowctrl_portEgrPktBufCntr_get(uint32 unit, rtk_port_t port, uint32 * pCntr, uint32 * pMaxCntr);
  #endif
    int32 _dal_waMon_portLinkChange_hdlr(int32 unit, rtk_port_t port, rtk_port_linkStatus_t new_linkStatus);
    int32 _dal_waMon_portLinkChange_process(uint32 unit);
#endif

#if defined(CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS)
void
dal_waMon_linkChangSwScanPort_set(void)
{
#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    uint32  unit, port;
    uint32  is_95r;
    uint32  port_cnt;
    int32   ret;
    rtk_portmask_t swScan_portmask;

    RTK_PORTMASK_RESET(swScan_portmask);

    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        if (HWP_9300_FAMILY_ID(unit))   /* 95R MAC-POLL-PHY */
        {
            is_95r = 0;
            port_cnt = 0;
            if ((ret = dal_linkMon_swScanPorts_get(unit, &swScan_portmask)) != RT_ERR_OK)
            {
                continue;
            }
            HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
            {
                if ((HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R) || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R_C22))
                {
                    is_95r = 1;
                    RTK_PORTMASK_PORT_SET(swScan_portmask, port);
                }
                if(HWP_SERDES_PORT(unit, port)
#if defined(CONFIG_SDK_RTL8226)
                    || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8226) || (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8226B) /* for software change 9300 SerDes mode */
#endif /* CONFIG_SDK_RTL8226 */
                  )
                {
                    RTK_PORTMASK_PORT_SET(swScan_portmask, port);
                    port_cnt++;
                }
            }

            if (is_95r || port_cnt)
            {
                dal_linkMon_swScanPorts_set(unit, &swScan_portmask);
            }
        }

        #if defined(CONFIG_SDK_RTL9310)
        if (HWP_9310_FAMILY_ID(unit))
        {
            HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
            {
                RTK_PORTMASK_PORT_SET(swScan_portmask, port);
            }

            dal_linkMon_swScanPorts_set(unit, &swScan_portmask);
        }
        #endif  /* CONFIG_SDK_RTL9310 */
    }/* HWP_UNIT_TRAVS_LOCAL */
#endif
}

/* When this API returns the RT_ERR_OK, then the linkMon should be enabled by waMon*/
int32
dal_waMon_linkMon_demand_check(void)
{
    uint32 unit, mac_id;

    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        if(HWP_8390_50_FAMILY(unit))
            return RT_ERR_OK;
        if(HWP_9300_FAMILY_ID(unit))
        {
            HWP_10GE_PORT_TRAVS(unit, mac_id)
            {
                if((HWP_PHY_MODEL_BY_PORT(unit, mac_id) == RTK_PHYTYPE_RTL8295R) || (HWP_PHY_MODEL_BY_PORT(unit, mac_id) == RTK_PHYTYPE_RTL8295R_C22))
                {
                    return RT_ERR_OK;
                }
                if(HWP_SERDES_PORT(unit, mac_id))
                {
                    return RT_ERR_OK;
                }
            }
        }

        #if defined(CONFIG_SDK_RTL9310)
        if (HWP_9310_FAMILY_ID(unit))
            return RT_ERR_OK;
        #endif  /* CONFIG_SDK_RTL9310 */
    }

    return RT_ERR_FAILED;
}
#endif

#endif /* ifndef PHY_ONLY */





/* Module Name : */

/* Function Name:
 *      dal_waMon_init
 * Description:
 *      Initial Workaround Monitor component
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
int32 dal_waMon_init(void)
{
    int i;

    /* init value */
    waMon_init = INIT_NOT_COMPLETED;

    /* create semaphore for sync, this semaphore is empty in beginning */
    for(i = 0; i < sizeof(pWaMon_cb) / sizeof(dal_waMon_cb_t); i++)
    {
        pWaMon_cb[i].wa_change_sem = osal_sem_create(0);

        if (0 == pWaMon_cb[i].wa_change_sem){
            RT_INIT_ERR(RT_ERR_FAILED, (MOD_INIT|MOD_DAL), "workaround monitor semaphore create failed");
            return RT_ERR_FAILED;
        }
    }
#ifndef PHY_ONLY
#if defined(CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS)
    {
        int32           ret;
    if(dal_waMon_linkMon_demand_check() == RT_ERR_OK)
    {
            waMon_linkSem = osal_sem_mutex_create();
            if (0 == waMon_linkSem)
            {
                RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "semaphore create failed");
            }

            ret = dal_linkMon_enable(1000000);
            if ((ret != RT_ERR_OK) && (ret != RT_ERR_THREAD_EXIST))
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            }
            if ((ret = dal_linkMon_register(_dal_waMon_portLinkChange_hdlr)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            }
            dal_waMon_linkChangSwScanPort_set();
        }
    }
#endif/* CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS */

#if defined(CONFIG_SDK_RTL8295R)
    osal_memset(dal_waMon_95rClkRecovChkFlag, 0, (sizeof(rtk_portmask_t) * RTK_MAX_NUM_OF_UNIT));
    osal_memset(dal_waMon_95rClkRstFlag, 0, (sizeof(rtk_portmask_t) * RTK_MAX_NUM_OF_UNIT));
    osal_memset(dal_waMon_95rClkRecovChkRetryCnt, 0, (sizeof(uint8) * RTK_MAX_NUM_OF_UNIT * RTK_MAX_PORT_PER_UNIT));
    osal_memset(dal_waMon_phySdsCaliStatus, 0, (sizeof(rtk_port_phySdsRxCaliStatus_t) * RTK_MAX_NUM_OF_UNIT * RTK_MAX_PORT_PER_UNIT));
#endif
#endif /* ifndef PHY_ONLY */

    waMon_init = INIT_COMPLETED;

    return RT_ERR_OK;

} /* end of dal_waMon_init */

/* Function Name:
 *      dal_waMon_enable
 * Description:
 *      Enable workaround monitor thread
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - scan interval is too small
 * Note:
 *      When enable workaround monitor thread
 *
 */
int32 dal_waMon_enable(void)
{
    int i;

#ifndef PHY_ONLY
    /* check Init status */
    RT_INIT_CHK(waMon_init);

    if (_dal_waMon_enableCfg_chk() == FALSE)
    {
        return RT_ERR_OK;
    }
#endif /* ifndef PHY_ONLY */

    /* create thread */
    for(i = 0; i < sizeof(pWaMon_cb) / sizeof(dal_waMon_cb_t); i++)
    {
        if ((pWaMon_cb[i].thread_id) != 0)
            continue;

        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "scan_interval_us=%u",
               pWaMon_cb[i].scan_interval_us);

#ifdef PHY_ONLY
        pWaMon_cb[i].thread_id = osal_thread_create(pWaMon_cb[i].name, RTK_DEFAULT_WA_MON_STACK_SIZE, RTK_DEFAULT_WA_MON_THREAD_PRI
                                , (void *)pWaMon_cb[i].waMon_func, &pWaMon_cb[i]);
#else
        pWaMon_cb[i].thread_id = osal_thread_create(pWaMon_cb[i].name, RTK_DEFAULT_WA_MON_STACK_SIZE, RTK_DEFAULT_WA_MON_THREAD_PRI
                                , (void *)pWaMon_cb[i].waMon_func, &pWaMon_cb[i]);
#endif /* PHY_ONLY */
        if (0 == (pWaMon_cb[i].thread_id))
        {
            RT_ERR(pWaMon_cb[i].thread_id, (MOD_DAL|MOD_PORT), "");
            return RT_ERR_THREAD_CREATE_FAILED;
        }
    }

    return RT_ERR_OK;
} /* end of dal_waMon_enable */

#ifndef PHY_ONLY
/* Function Name:
 *      _dal_waMon_enableCfg_chk
 * Description:
 *      Return FALSE will disable waMon, TURE will enable.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      TRUE          - enabled
 *      FALSE         - disabled
 * Note:
 *
 */
uint32
_dal_waMon_enableCfg_chk(void)
{

    return TRUE;
}

/* Function Name:
 *      dal_waMon_disable
 * Description:
 *      Disable workaround scan thread
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      When disable workaround monitor thread
 */
int32 dal_waMon_disable(void)
{
    int i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "");

    /* check Init status */
    RT_INIT_CHK(waMon_init);

    /* parameter check */

    /* reset scan_interval_us to 0, thread will suicide after finish all waiting job */
    for(i = 0; i < sizeof(pWaMon_cb) / sizeof(dal_waMon_cb_t); i++)
    {
        pWaMon_cb[i].scan_interval_us = 0;

        /* let thread continue */
        osal_sem_give(pWaMon_cb[i].wa_change_sem);
    }

    return RT_ERR_OK;

} /* end of dal_waMon_disable */

#if defined(CONFIG_SFLOW_PG_THREAD)
/* Function Name:
 *      dal_waMon_pgMon_sflow_register
 * Description:
 *      Register to receive callbacks for when sFlow enabling status change due to used-page count exceed.
 * Input:
 *      stsChg_cb   - callback function for sFlow enabling status change
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
int32 dal_waMon_pgMon_sflow_register(sflowStsChg_cb_f stsChg_cb)
{
#if defined(CONFIG_SDK_RTL9310)
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "");

    /* check Init status */
    RT_INIT_CHK(waMon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == stsChg_cb), RT_ERR_NULL_POINTER);

    sflowStsChg_cb = stsChg_cb;
#endif

    return RT_ERR_OK;
} /* end of dal_waMon_pgMon_sflow_register */
#endif

#if defined(CONFIG_SFLOW_PG_THREAD)
void
_dal_waMonPgMon_egrCpuQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
#if defined(CONFIG_SDK_RTL9310)
    if (queue < CPU_MONITORING_QUEUE_NUM)
    {
        que_drop_thr_hi[queue] = pThresh->high;
        que_drop_thr_lo[queue] = pThresh->low;
    }
#endif
}
#endif

#if defined(CONFIG_SDK_RTL9310) && ((defined(CONFIG_SDK_DRIVER_L2NTFY) && defined(CONFIG_SDK_L2_NTFY_PG_THREAD)) || defined(CONFIG_SFLOW_PG_THREAD))
static void
_dal_waMonPgMon_thread(void *pInput)
{
#if defined(CONFIG_SDK_DRIVER_L2NTFY) && defined(CONFIG_SDK_L2_NTFY_PG_THREAD)
    uint32          ntfyPageCnt = 0;
#endif
#if defined(CONFIG_SFLOW_PG_THREAD)
    uint32          i;
    uint32          pageCnt[CPU_MONITORING_QUEUE_NUM], qid, exceed, under;
#endif
    uint32          unit = 0;
    hal_control_t   *pHal_ctrl;
    dal_waMon_cb_t  *pWaMon_info = (dal_waMon_cb_t *)pInput;

    /* Check whether device is exist in lower layer(HAL) */
    if ((pHal_ctrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        return;
    }

    RT_LOG(LOG_DEBUG, MOD_L2, "PG-Mon thread start running!\n");


    /* forever loop */
    while (pWaMon_info->scan_interval_us != 0)
    {
        osal_sem_take(pWaMon_info->wa_change_sem, pWaMon_info->scan_interval_us);
        if (waMon_enable == DISABLED)
        {
            continue;
        }

#if defined(CONFIG_SDK_DRIVER_L2NTFY) && defined(CONFIG_SDK_L2_NTFY_PG_THREAD)
        _l2ntfy_queueCnt_get(unit, &ntfyPageCnt);
        if ((gNtfy_suppressed == 0) && (ntfyPageCnt >= L2_NTFY_PG_DISABLE_LRN_AGE_THR_HI))
        {
            drv_l2ntfy_l2LearningAge_disable(unit, ARB_NTFY_1);
            gNtfy_suppressed = 1;
        }

        if (gNtfy_suppressed && ntfyPageCnt < L2_NTFY_PG_DISABLE_LRN_AGE_THR_LO)
        {
            drv_l2ntfy_l2LearningAge_restore(unit, ARB_NTFY_1);
            gNtfy_suppressed = 0;
        }
#endif


#if defined(CONFIG_SFLOW_PG_THREAD)
        exceed  = FALSE;
        under   = TRUE;
        for (i = 0; i < (sizeof(gPG_mon_que)/sizeof(uint32)); i++)
        {
            qid = gPG_mon_que[i];

            reg_array_field_read(unit, MANGO_FC_CPU_Q_EGR_PAGE_CNTr, REG_ARRAY_INDEX_NONE, qid, MANGO_CNTf, &pageCnt[qid]);
            if (pageCnt[qid] >= (que_drop_thr_hi[qid] + sflow_thr_hi_offset[qid]))
                exceed = TRUE;

            if (pageCnt[qid] >= (que_drop_thr_lo[qid] + sflow_thr_lo_offset[qid]))
            {
                under = FALSE;
            }
        }

        if ((gSflow_suppressed == 0) && (TRUE == exceed))
        {
            _dal_mango_mirror_sflow_disable(unit, SFLOW_ARB_0);
            if (sflowStsChg_cb != NULL)
                sflowStsChg_cb(unit, SFLOW_DISABLE);
            gSflow_suppressed = 1;
        }

        if (gSflow_suppressed && (TRUE == under))
        {
            _dal_mango_mirror_sflow_restore(unit, SFLOW_ARB_0);
            if (sflowStsChg_cb != NULL)
                sflowStsChg_cb(unit, SFLOW_ENABLE);
            gSflow_suppressed = 0;
        }
#endif
    }

    return;
}
#endif

/* Function Name:
 *      _dal_waMon500_thread
 * Description:
 *      500 ms task
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
static void _dal_waMon500_thread(void *pInput)
{
    uint32  unit = 0;
    hal_control_t *pHal_ctrl;
    dal_waMon_cb_t *pWaMon_info = (dal_waMon_cb_t *)pInput;

    /* Check whether device is exist in lower layer(HAL) */
    if ((pHal_ctrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        return;
    }

    /* forever loop */
    while (pWaMon_info->scan_interval_us != 0)
    {
        /* wait semaphore for workaround scan interval */
        osal_sem_take(pWaMon_info->wa_change_sem, pWaMon_info->scan_interval_us);
        if (waMon_enable == DISABLED)
        {
            continue;
        }

#if defined(CONFIG_SDK_RTL8380)
        if ((RTL8332M_CHIP_ID == HWP_CHIP_ID(unit) || RTL8330M_CHIP_ID == HWP_CHIP_ID(unit)) ||
            (RTL8382M_CHIP_ID == HWP_CHIP_ID(unit) || RTL8380M_CHIP_ID == HWP_CHIP_ID(unit) || RTL8381M_CHIP_ID == HWP_CHIP_ID(unit)))
        {
  #ifndef CONFIG_SDK_EXTERNAL_CPU
            /* SS-93 */
            dal_maple_rate_cpuEgrBandwidthCtrl_workaround(unit);
  #endif
        }
#endif/* CONFIG_SDK_RTL8380 */

    }

    osal_thread_exit(0);

    return;
} /* end of _dal_waMon500_thread */

/* Function Name:
 *      dal_waMon_phyEsdRstEvn_clear
 * Description:
 *      clear the PHY ESD reset event
 * Input:
 *      unit            - unit
 *      base_mac_id     - PHY's base MAC id for indicating the PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
int32
dal_waMon_phyEsdRstEvn_clear(uint32 unit, rtk_port_t base_mac_id)
{
    if (unit >= RTK_MAX_NUM_OF_UNIT)
        return RT_ERR_UNIT_ID;

    if (base_mac_id >= RTK_MAX_PORT_PER_UNIT)
        return RT_ERR_PORT_ID;

    RT_INIT_CHK(waMon_init);

    RTK_PORTMASK_PORT_CLEAR(dal_waMon_phyEsdRstEvn[unit], base_mac_id);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_waMon_phyEsdRstEvn_get
 * Description:
 *      get the event status
 * Input:
 *      unit            - unit
 *      base_mac_id     - PHY's base MAC id for indicating the PHY
 * Output:
 *      is_evn_set      - TRUE or FALSE
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
int32
dal_waMon_phyEsdRstEvn_get(uint32 unit, rtk_port_t base_mac_id, uint32 *is_evn_set)
{
    if (unit >= RTK_MAX_NUM_OF_UNIT)
        return RT_ERR_UNIT_ID;

    if (base_mac_id >= RTK_MAX_PORT_PER_UNIT)
        return RT_ERR_PORT_ID;

    RT_INIT_CHK(waMon_init);

    if (RTK_PORTMASK_IS_PORT_SET(dal_waMon_phyEsdRstEvn[unit], base_mac_id))
    {
        *is_evn_set = TRUE;
    }
    else
    {
        *is_evn_set = FALSE;
    }
    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      dal_waMon_8295rClkRecovChk_set
 * Description:
 *      set flag for checking 8295R clk
 * Input:
 *      unit            - unit
 *      port     - PHY's base MAC id for indicating the PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
int32
dal_waMon_8295rClkRecovChk_set(uint32 unit, rtk_port_t port)
{
    if (unit >= RTK_MAX_NUM_OF_UNIT)
        return RT_ERR_UNIT_ID;

    if (port >= RTK_MAX_PORT_PER_UNIT)
        return RT_ERR_PORT_ID;

    RT_INIT_CHK(waMon_init);

    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R)
    {
        RTK_PORTMASK_PORT_SET(dal_waMon_95rClkRecovChkFlag[unit], port);
        dal_waMon_95rClkRecovChkRetryCnt[unit][port] = 3;
    }
    return RT_ERR_OK;
}

int32
dal_waMon_8295rClkRecovChk_clear(uint32 unit, rtk_port_t port)
{
    if (unit >= RTK_MAX_NUM_OF_UNIT)
        return RT_ERR_UNIT_ID;

    if (port >= RTK_MAX_PORT_PER_UNIT)
        return RT_ERR_PORT_ID;

    RT_INIT_CHK(waMon_init);

    if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R)
    {
        if (dal_waMon_95rClkRecovChkRetryCnt[unit][port] > 0)
        {
            dal_waMon_95rClkRecovChkRetryCnt[unit][port]--;
        }

        if (dal_waMon_95rClkRecovChkRetryCnt[unit][port] == 0)
        {
            RTK_PORTMASK_PORT_CLEAR(dal_waMon_95rClkRecovChkFlag[unit], port);
        }
    }
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS)
/* Function Name:
 *      _dal_waMon_portLinkChange_hdlr
 * Description:
 *      callback handler by link change monitor.
 *      It sets link change indicate bit of the port and new link status then waiting for
 *      _dal_waMon_portLinkChange_process() to process the event.
 * Input:
 *      unit - unit id
 *      port - port id
 *      new_linkStatus  - Link change to UP/Down
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_dal_waMon_portLinkChange_hdlr(int32 unit, rtk_port_t port, rtk_port_linkStatus_t new_linkStatus)
{
    WAMON_LINK_SEM_LOCK();

    /* set link change indicate bit */
    RTK_PORTMASK_PORT_SET(waMonLinkChang[unit], port);

    if (new_linkStatus == PORT_LINKUP)
        RTK_PORTMASK_PORT_SET(waMonLinkStat[unit], port);
    else
        RTK_PORTMASK_PORT_CLEAR(waMonLinkStat[unit], port);

    WAMON_LINK_SEM_UNLOCK();

    return RT_ERR_OK;
}
#endif/* CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS */

#if defined(CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS)
/* Function Name:
 *      _dal_waMon_portLinkChange_process
 * Description:
 *      Link change handler
 * Input:
 *      unit - unit id
 *      port - port id
 *      new_linkStatus  - Link change to UP/Down
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_dal_waMon_portLinkChange_process(uint32 unit)
{
    rtk_port_t      port;
    rtk_port_linkStatus_t   linkSts;
#if defined(CONFIG_SDK_RTL9310)
    uint32 cntr = 0, maxCntr;
    rtk_port_linkStatus_t   currLinkSts;
#endif

    RTK_PORTMASK_SCAN(waMonLinkChang[unit], port)
    {
        WAMON_LINK_SEM_LOCK();

        linkSts = RTK_PORTMASK_IS_PORT_SET(waMonLinkStat[unit], port) ? PORT_LINKUP : PORT_LINKDOWN;

        RTK_PORTMASK_PORT_CLEAR(waMonLinkChang[unit], port);

        WAMON_LINK_SEM_UNLOCK();

        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "unit=%d,port=%u link:%s", unit, port, (linkSts==PORT_LINKUP)?"Up":"Down");

        dal_phy_portLinkChange_process(unit, port, linkSts);

#if defined(CONFIG_SDK_RTL8390)
        if (HWP_8390_50_FAMILY(unit))
        {
            /* rate shall be processed after phy */
            dal_cypress_rate_egrBandwidthCtrlLinkChange_handler(unit, port, linkSts);
        }
#endif
#if defined(CONFIG_SDK_RTL9310)
        if (HWP_9310_FAMILY_ID(unit))
        {
            if (PORT_LINKDOWN == linkSts)
            {
                drainTx_watchdog_cnt[unit]++;
                if (dal_mango_rate_portEgrQueueDrain_process(unit, port) != RT_ERR_OK)
                {
                    currLinkSts = RTK_PORTMASK_IS_PORT_SET(waMonLinkStat[unit], port) ? PORT_LINKUP : PORT_LINKDOWN;
                    if (PORT_LINKDOWN == currLinkSts)
                    {
                        drainTxPathHasPkt_watchdog_cnt[unit]++;

                        // Get egress port used page count
                        if ((_dal_mango_flowctrl_portEgrPktBufCntr_get(unit, port, &cntr, &maxCntr) != RT_ERR_OK) || (cntr!=0))
                        {
                            drainTxQFailed_watchdog_cnt[unit]++;
                        }
                        else if (!HWP_SERDES_PORT(unit, port))
                        {
                            if (dal_mango_portMacSyncPhySts_process(unit, port) != RT_ERR_OK)
                            {
                                syncPhyToMacFailed_watchdog_cnt[unit]++;
                            }
                        }
                    }
                }
            }
        }
#endif

    }/* RTK_PORTMASK_SCAN */

    return RT_ERR_OK;

}
#endif /* CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS */

#if defined(CONFIG_SDK_RTL8295R)
/* Function Name:
 *      _dal_waMon_macSdsRxCaliStatus_get
 * Description:
 *      MAC serdes RX calibration status get
 * Input:
 *      unit - unit id
 *      sdsId - serdes id
 *      pStatus - ckeck result status. RT_ERR_OK: status normal; RT_ERR_FAILED: problem occur.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
dal_waMon_macSdsStatus_check(uint32 unit, uint32 sdsId, int32 *pStatus)
{
#if defined(CONFIG_SDK_RTL9300)
    rtk_sds_linkSts_t   linkInfo;
    rt_serdesMode_t     mode;
    uint32  rxIdle;
    int32   ret;

    *pStatus = RT_ERR_OK;
    if (HWP_9300_FAMILY_ID(unit))
    {

        if((ret = dal_longan_sds_mode_get(unit, sdsId, &mode))!= RT_ERR_OK)
            return ret;

        if ((mode != RTK_MII_10GR) && (mode != RTK_MII_1000BX_FIBER))
        {
            return RT_ERR_OK;
        }

        dal_longan_sds_linkSts_get(unit, sdsId, &linkInfo);
        if ((ret = dal_longan_sds_linkSts_get(unit, sdsId, &linkInfo)) != RT_ERR_OK)
            return ret;


        if (((mode == RTK_MII_10GR) && (linkInfo.sts == 1) && (linkInfo.latch_sts == 1)) || ((mode == RTK_MII_1000BX_FIBER) && (linkInfo.sts == 0x111) && (linkInfo.latch_sts == 0x111))) /* link is up */
        {
            return RT_ERR_OK;
        }

        if ((ret = dal_longan_sds_10gRxIdle_get(unit, sdsId, &rxIdle)) != RT_ERR_OK)
            return ret;

        if (rxIdle == 1) /* no idle */
        {
            return RT_ERR_OK;
        }

        /* here is link down and has idle */
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u sds=%u MAC SDS link 95R fail link:%x,%x idle:%u", unit, sdsId, linkInfo.sts, linkInfo.latch_sts, rxIdle);
        *pStatus = RT_ERR_FAILED;
        return RT_ERR_OK;
    }
#endif
    return RT_ERR_CHIP_NOT_SUPPORTED;

}


/* Function Name:
 *      _dal_waMon_macSdsRxCaliStatus_get
 * Description:
 *      MAC serdes RX calibration status get
 * Input:
 *      unit - unit id

 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
int32
_dal_waMon_macSdsRxCaliStatus_get(uint32 unit, uint32 sdsId, rtk_port_phySdsRxCaliStatus_t   *rxCaliStatus)
{
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        return dal_longan_sds_rxCaliStatus_get(unit, sdsId, rxCaliStatus);
    }
#endif
    return RT_ERR_CHIP_NOT_SUPPORTED;
}


/* Function Name:
 *      _dal_waMon_macSdsRx_reset
 * Description:
 *      MAC serdes RX Reset
 * Input:
 *      unit - unit id
 *      sdsId - serdes id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
_dal_waMon_macSdsRx_reset(uint32 unit, uint32 sdsId)
{
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        dal_longan_sds_rx_rst(unit, sdsId);
        dal_longan_sds_rxCaliStatus_clear(unit, sdsId);
    }
#endif/* CONFIG_SDK_RTL9300 */
}

/* Function Name:
 *      dal_waMon_8295r_clkRecover_watchdog
 * Description:
 *      8295R watchdog
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
dal_waMon_8295r_clkRecover_watchdog(uint32 unit)
{
    rtk_port_phySdsRxCaliStatus_t   rxCaliStatus;
    uint32      port, sdsId;
    int32       status;
    int32       ret;
    rtk_enable_t    enable;
    rtk_port_10gMedia_t media;


    if (!HWP_9300_FAMILY_ID(unit))
        return;


    HWP_10GE_PORT_TRAVS(unit, port)
    {
        if (HWP_PHY_MODEL_BY_PORT(unit, port) != RTK_PHYTYPE_RTL8295R)
            continue;

        if (!RTK_PORTMASK_IS_PORT_SET(dal_waMon_95rClkRecovChkFlag[unit], port))
        {
            continue;
        }

        sdsId = HWP_PORT_SDSID(unit, port);
        if (rtk_port_adminEnable_get(unit, port, &enable) != RT_ERR_OK)
        {
            continue;
        }
        if (enable != ENABLED)
        {
            dal_waMon_8295rClkRecovChk_clear(unit, port);
            continue;
        }
        if (rtk_port_10gMedia_get(unit, port, &media) != RT_ERR_OK)
        {
            continue;
        }
        if (media == PORT_10GMEDIA_NONE)
        {
            dal_waMon_8295rClkRecovChk_clear(unit, port);
            continue;
        }


        if (  (media == PORT_10GMEDIA_FIBER_10G) || (media == PORT_10GMEDIA_DAC_50CM) || (media == PORT_10GMEDIA_DAC_100CM)
            ||(media == PORT_10GMEDIA_DAC_300CM) || (media == PORT_10GMEDIA_DAC_500CM))
        {
            if ((ret = _dal_waMon_macSdsRxCaliStatus_get(unit, sdsId, &rxCaliStatus)) != RT_ERR_OK)
            {
                dal_waMon_8295rClkRecovChk_clear(unit, port);
                continue;
            }

            /* rx-cali is not performanced yet */
            if (rxCaliStatus == PHY_SDS_RXCALI_STATUS_NOINFO)
            {
                continue;
            }
        }

        if ((ret = dal_waMon_macSdsStatus_check(unit, sdsId, &status)) != RT_ERR_OK)
        {
            dal_waMon_8295rClkRecovChk_clear(unit, port);
            continue;
        }

        if (status == RT_ERR_OK)
        {
            dal_waMon_8295rClkRecovChk_clear(unit, port);
            if (  (RTK_PORTMASK_IS_PORT_SET(dal_waMon_95rClkRstFlag[unit], port))
                && (dal_waMon_95rClkRecovChkRetryCnt[unit][port] == 0))
            {
                RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u port=%u intr link recovered", unit, port);
                RTK_PORTMASK_PORT_CLEAR(dal_waMon_95rClkRstFlag[unit], port);
                phy95rClkRecovOK_watchdog_cnt[unit]++;
            }
            continue;
        }

        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PORT), "unit=%u port=%u reset MAC SDS RX and 95R", unit, port);
        _dal_phy_macIntfSerdes_reset(unit, port);
        _dal_waMon_macSdsRx_reset(unit, sdsId);

        phy95rClkRecov_watchdog_cnt[unit]++;
        dal_waMon_95rClkRecovChkRetryCnt[unit][port] = 3;
        RTK_PORTMASK_PORT_SET(dal_waMon_95rClkRstFlag[unit], port);
    }
}

#endif/* CONFIG_SDK_RTL8295R */

#endif /* ifndef PHY_ONLY */


#ifndef PHY_ONLY
#ifdef CONFIG_SDK_RTL8295R
/* Function Name:
 *      _dal_waMon_phy_8295r_ntfy_mac_cali_watchdog
 * Description:
 *      polling PHY serdes calibration status, when done, notify MAC to do calibration
 * Input:
 *      unit            - unit
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
void
_dal_waMon_phy_8295r_ntfy_mac_cali_watchdog(uint32 unit)
{
    rtk_port_t port;
    HWP_10GE_PORT_TRAVS(unit, port)
    {
        rtk_port_phySdsRxCaliStatus_t   phySdsCaliStatus;
        if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8295R)
        {
            if (dal_phy_sdsRxCaliStatus_get(unit, port, 0, &phySdsCaliStatus) == RT_ERR_OK)
            {
                if ((dal_waMon_phySdsCaliStatus[unit][port] != phySdsCaliStatus) && (phySdsCaliStatus == PHY_SDS_RXCALI_STATUS_OK))
                {
#if defined(CONFIG_SDK_RTL9310)
                    if (HWP_9310_FAMILY_ID(unit))
                    {
                        if (HWP_PORT_SDSID(unit, port) != HWP_NONE)
                        {
                            dal_mango_sds_pcb_adapt(unit, HWP_PORT_SDSID(unit, port));
                        }
                    }
#endif /* CONFIG_SDK_RTL9310 */
                }
                dal_waMon_phySdsCaliStatus[unit][port] = phySdsCaliStatus;
            }
        }
    }
}
#endif/* CONFIG_SDK_RTL8295R */
#endif /* ifndef PHY_ONLY */

/* Function Name:
 *      _dal_waMon1000_thread
 * Description:
 *      1000 ms task
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
static void _dal_waMon1000_thread(void *pInput)
{
    uint32  unit = 0;
    dal_waMon_cb_t *pWaMon_info = (dal_waMon_cb_t *)pInput;

    /* forever loop */
    while (pWaMon_info->scan_interval_us != 0)
    {
        /* wait semaphore for workaround scan interval */
        osal_sem_take(pWaMon_info->wa_change_sem, pWaMon_info->scan_interval_us);
        if (waMon_enable == DISABLED)
        {
            continue;
        }

        HWP_UNIT_TRAVS_LOCAL(unit)
        {
#ifndef PHY_ONLY
            if (!HWP_SWCORE_SUPPORT(unit))
                continue;
#endif

#ifndef PHY_ONLY
#if defined(CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS)
            _dal_waMon_portLinkChange_process(unit);
#endif /* CONFIG_SDK_WAMON_LINK_CHANGE_PROCESS */
#endif

#if defined(CONFIG_SDK_WA_FIBER_RX_WATCHDOG)
            dal_phy_fiberRx_watchdog(unit);
#endif

#ifndef PHY_ONLY
#if defined(CONFIG_SDK_RTL8295R)
            _dal_waMon_phy_8295r_ntfy_mac_cali_watchdog(unit);
#endif/* CONFIG_SDK_RTL8295R */
#endif

#if defined(CONFIG_SDK_WA_PHY_WATCHDOG)
            dal_phy_handler(unit);
#endif  /* CONFIG_SDK_WA_PHY_WATCHDOG */

#ifndef PHY_ONLY

#if defined(CONFIG_SDK_WA_REMOTE_FAULT)
            dal_phy_fiberRemoteFault_watchdog(unit);
#endif

#if defined(CONFIG_SDK_RTL8380)
            /*For RTL838X & RTL833X*/
            if (HWP_8330_FAMILY_ID(unit) || HWP_8380_FAMILY_ID(unit))
            {
#if defined(CONFIG_SDK_WA_PKTBUF_WATCHDOG)
                dal_maple_port_pktbuf_watchdog(unit);
#endif

#if defined(CONFIG_SDK_WA_SERDES_WATCHDOG)
                dal_maple_port_serdes_watchdog(unit);
#endif

#if defined(CONFIG_SDK_WA_PKTBUF_WATCHDOG) || \
      defined(CONFIG_SDK_WA_SERDES_WATCHDOG) ||\
      defined(CONFIG_SDK_WA_PHY_WATCHDOG)
                {
                    dal_maple_port_watchdog_debug(unit);
                }
#endif

#if defined(CONFIG_SDK_WA_COMBO_FLOWCONTROL)
                /*For RTL83X & RTL833X combo-port*/
                dal_maple_port_comboPort_workaround(unit);
#endif
            }
#endif /* end of CONFIG_SDK_RTL8380 */


#if defined(CONFIG_SDK_RTL8390)
            /*For RTL839X & RTL835X*/
            if (HWP_8390_50_FAMILY(unit))
            {
#if defined(CONFIG_SDK_WA_SERDES_WATCHDOG)
                dal_cypress_port_serdes_watchdog(unit);
#endif
            }
#endif //defined(CONFIG_SDK_RTL8390)

#if defined(CONFIG_SDK_RTL9300)

            if (HWP_9300_FAMILY_ID(unit))
            {
#if defined(CONFIG_SDK_WA_PKTBUF_WATCHDOG)
                dal_longan_port_pktbuf_watchdog(unit);
#endif

#if defined(CONFIG_SDK_WA_SERDES_WATCHDOG)
                dal_longan_port_serdes_watchdog(unit);
#endif

#if defined(CONFIG_SDK_WA_PKTBUF_WATCHDOG) || \
      defined(CONFIG_SDK_WA_SERDES_WATCHDOG) ||\
      defined(CONFIG_SDK_WA_FIBER_RX_WATCHDOG) ||\
      defined(CONFIG_SDK_WA_PHY_WATCHDOG)
                {
                    dal_longan_port_watchdog_debug(unit);
                }
#endif
            }
#endif

#if defined(CONFIG_SDK_RTL9310)
            if (HWP_9310_FAMILY_ID(unit))
            {
#if defined(CONFIG_SDK_WA_SERDES_WATCHDOG)
                dal_mango_port_serdes_watchdog(unit);
#endif
            }
#endif  /* CONFIG_SDK_RTL9310 */

#if defined(CONFIG_SDK_RTL8295R)
            dal_waMon_8295r_clkRecover_watchdog(unit);
#endif/* CONFIG_SDK_RTL8295R */

#endif /* ifndef PHY_ONLY */
        } //end of HWP_UNIT_TRAVS_LOCAL()
    }
#ifndef PHY_ONLY
    osal_thread_exit(0);
#endif

    return;
} /* end of _dal_waMon1000_thread */


/* Function Name:
 *      dal_waMon_phyReconfig_register
 * Description:
 *      Register callback function for PHY need to reconfigure notification
 * Input:
 *      phyReconfig_callback    - callback function for reconfigure notification
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
int32 dal_waMon_phyReconfig_register(rtk_port_phyReconfig_callback_t phyReconfig_callback)
{
    return phy_waMon_phyReconfig_register(phyReconfig_callback);
} /* end of dal_waMon_phyReconfig_register */

/* Function Name:
 *      dal_waMon_phyReconfig_unregister
 * Description:
 *      UnRegister callback function for PHY need to reconfigure notification
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
int32 dal_waMon_phyReconfig_unregister(void)
{
    return phy_waMon_phyReconfig_unregister();
} /* end of dal_waMon_phyReconfig_unregister */

/* Function Name:
 *      dal_waMon_phyReconfig_portMaskSet
 * Description:
 *      Send the need ReConfig port mask back to user.
 * Input:
 *      phyReconfig_callback    - callback function for link change
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
int32 dal_waMon_phyReconfig_portMaskSet(uint32 unit, rtk_port_t port)
{
    hal_control_t *pHal_ctrl;
    rtk_portmask_t portMask;
    int loop;
    int mask_len, base_id;
    phy_type_t phyType;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "");

    osal_memset(&portMask, 0, sizeof(rtk_portmask_t));

    /* Check whether device is exist in lower layer(HAL) */
    if ((pHal_ctrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    /* check Init status */
    RT_INIT_CHK(waMon_init);

    if((phyType = HWP_PHY_MODEL_BY_PORT(unit, port)) == RTK_PHYTYPE_INVALID)
    {
        return RT_ERR_FAILED;
    }
    base_id = HWP_PHY_BASE_MACID(unit, port);
    mask_len = HWP_PHY_BASE_PHY_MAX(unit, port);

    for(loop = base_id; loop < (base_id + mask_len); loop++)
                        RTK_PORTMASK_PORT_SET(portMask,loop);


    if(pPhyReconfig_cb != NULL)
        (pPhyReconfig_cb)(unit, &portMask);
    else
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "No CallBack for port %d\n",port);

    return RT_ERR_OK;
} /* end of dal_waMon_phyReconfig_portMaskSet */

/* Function Name:
 *      dal_waMon_phyReconfig_perPort_portMaskSet
 * Description:
 *      Send the need ReConfig port mask back to user.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      pPortmask       - port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
int32 dal_waMon_phyReconfig_perPort_portMaskSet(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{
    hal_control_t *pHal_ctrl = NULL;
    phy_type_t phyType = RTK_PHYTYPE_NONE;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "");

    /* Check whether device is exist in lower layer(HAL) */
    if ((pHal_ctrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    /* check Init status */
    RT_INIT_CHK(waMon_init);

    if ((phyType = HWP_PHY_MODEL_BY_PORT(unit, port)) == RTK_PHYTYPE_INVALID)
    {
        return RT_ERR_FAILED;
    }

    if (pPhyReconfig_cb != NULL)
        (pPhyReconfig_cb)(unit, pPortmask);
    else
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "No CallBack for port %d\n",port);

    return RT_ERR_OK;
} /* end of dal_waMon_phyReconfig_perPort_portMaskSet */


/* Function Name:
 *      dal_waMon_phyEsdRstEvn_set
 * Description:
 *      set the PHY ESD reset event
 * Input:
 *      unit            - unit
 *      base_mac_id     - PHY's base MAC id for indicating the PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
int32
dal_waMon_phyEsdRstEvn_set(uint32 unit, rtk_port_t base_mac_id)
{
    if (unit >= RTK_MAX_NUM_OF_UNIT)
        return RT_ERR_UNIT_ID;

    if (base_mac_id >= RTK_MAX_PORT_PER_UNIT)
        return RT_ERR_PORT_ID;

    RT_INIT_CHK(waMon_init);

    RTK_PORTMASK_PORT_SET(dal_waMon_phyEsdRstEvn[unit], base_mac_id);
    return RT_ERR_OK;
}

