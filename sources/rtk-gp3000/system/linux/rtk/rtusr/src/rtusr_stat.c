/*
 * Copyright (C) 2013 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (星期五, 03 五月 2013) $
 *
 * Purpose : Definition of Statistic API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Statistic Counter Reset
 *           (2) Statistic Counter Get
 *
 */



/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      rtk_stat_init
 * Description:
 *      Initialize stat module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_PORT_CNTR_FAIL   - Could not retrieve/reset Port Counter
 * Note:
 *      Must initialize stat module before calling any stat APIs.
 */
int32
rtk_stat_init(void)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    SETSOCKOPT(RTDRV_STAT_INIT, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_init */


/* Function Name:
 *      rtk_stat_global_reset
 * Description:
 *      Reset the global counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED

 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 * Note:
 *      None
 */
int32
rtk_stat_global_reset(void)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    SETSOCKOPT(RTDRV_STAT_GLOBAL_RESET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_global_reset */


/* Function Name:
 *      rtk_stat_port_reset
 * Description:
 *      Reset the specified port counters in the specified device.
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
int32
rtk_stat_port_reset(rtk_port_t port)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    osal_memcpy(&stat_cfg.port, &port, sizeof(rtk_port_t));
    SETSOCKOPT(RTDRV_STAT_PORT_RESET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_port_reset */

/* Function Name:
 *      rtk_stat_log_reset
 * Description:
 *      Reset the specified ACL log counters.
 * Input:
 *      index - log index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_log_reset(uint32 index)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    osal_memcpy(&stat_cfg.index, &index, sizeof(uint32));
    SETSOCKOPT(RTDRV_STAT_LOG_RESET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_log_reset */

/* Function Name:
 *      rtk_stat_rstCntValue_set
 * Description:
 *      Set the counter value after reset
 * Input:
 *      None
 * Output:
 *      rstValue  - the counter value after reset
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_rstCntValue_set(rtk_mib_rst_value_t rstValue)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    osal_memcpy(&stat_cfg.rstValue, &rstValue, sizeof(rtk_mib_rst_value_t));
    SETSOCKOPT(RTDRV_STAT_RSTCNTVALUE_SET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_rstCntValue_set */

/* Function Name:
 *      rtk_stat_rstCntValue_get
 * Description:
 *      Get the counter value after reset
 * Input:
 *      None
 * Output:
 *      pRstValue  - pointer buffer of value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_stat_rstCntValue_get(rtk_mib_rst_value_t *pRstValue)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRstValue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.rstValue, pRstValue, sizeof(rtk_mib_rst_value_t));
    GETSOCKOPT(RTDRV_STAT_RSTCNTVALUE_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pRstValue, &stat_cfg.rstValue, sizeof(rtk_mib_rst_value_t));

    return RT_ERR_OK;
}   /* end of rtk_stat_rstCntValue_get */

/* Function Name:
 *      rtk_stat_global_get
 * Description:
 *      Get one specified global counter in the specified device.
 * Input:
 *      cntrIdx - specified global counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
int32
rtk_stat_global_get(rtk_stat_global_type_t cntrIdx, uint64 *pCntr)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.cntrIdx, &cntrIdx, sizeof(rtk_stat_global_type_t));
    GETSOCKOPT(RTDRV_STAT_GLOBAL_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pCntr, &stat_cfg.cntr, sizeof(uint64));

    return RT_ERR_OK;
}   /* end of rtk_stat_global_get */


/* Function Name:
 *      rtk_stat_global_getAll
 * Description:
 *      Get all global counters in the specified device.
 * Input:
 *      None
 * Output:
 *      pGlobalCntrs - pointer buffer of global counter structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
int32
rtk_stat_global_getAll(rtk_stat_global_cntr_t *pGlobalCntrs)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGlobalCntrs), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.globalCntrs, pGlobalCntrs, sizeof(rtk_stat_global_cntr_t));
    GETSOCKOPT(RTDRV_STAT_GLOBAL_GETALL, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pGlobalCntrs, &stat_cfg.globalCntrs, sizeof(rtk_stat_global_cntr_t));

    return RT_ERR_OK;
}   /* end of rtk_stat_global_getAll */


/* Function Name:
 *      rtk_stat_port_get
 * Description:
 *      Get one specified port counter.
 * Input:
 *      port     - port id
 *      cntrIdx - specified port counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
int32
rtk_stat_port_get(rtk_port_t port, rtk_stat_port_type_t portCntrIdx, uint64 *pCntr)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&stat_cfg.portCntrIdx, &portCntrIdx, sizeof(rtk_stat_port_type_t));
    GETSOCKOPT(RTDRV_STAT_PORT_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pCntr, &stat_cfg.cntr, sizeof(uint64));

    return RT_ERR_OK;
}   /* end of rtk_stat_port_get */


/* Function Name:
 *      rtk_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pPortCntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
int32
rtk_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortCntrs), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_STAT_PORT_GETALL, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pPortCntrs, &stat_cfg.portCntrs, sizeof(rtk_stat_port_cntr_t));

    return RT_ERR_OK;
}   /* end of rtk_stat_port_getAll */

/* Function Name:
 *      rtk_stat_log_get
 * Description:
 *      Get ACL logging counter.
 * Input:
 *      cntrIdx  - logging index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - invalid index
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_stat_log_get(uint32 index, uint64 *pCntr)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_STAT_LOG_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pCntr, &stat_cfg.cntr, sizeof(uint64));

    return RT_ERR_OK;
}   /* end of rtk_stat_log_get */

/* Function Name:
 *      rtk_stat_logCtrl_set
 * Description:
 *      Set the acl log counters mode for 32-bits or 64-bits counter, and
 *      set the acl log counters type for packet or byte counter
 * Input:
 *      index       - index of ACL log counter
 *      ctrl        - log conter control setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_logCtrl_set(uint32 index, rtk_stat_log_ctrl_t ctrl)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    osal_memcpy(&stat_cfg.index, &index, sizeof(uint32));
    osal_memcpy(&stat_cfg.ctrl, &ctrl, sizeof(rtk_stat_log_ctrl_t));
    SETSOCKOPT(RTDRV_STAT_LOGCTRL_SET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_logCtrl_set */

/* Function Name:
 *      rtk_stat_logCtrl_set
 * Description:
 *      Get the acl log counters mode for 32-bits or 64-bits counter, and
 *      get the acl log counters type for packet or byte counter
 * Input:
 *      index       - index of ACL log counter
 * Output:
 *      pCtrl       - log counter control setting
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_logCtrl_get(uint32 index, rtk_stat_log_ctrl_t *pCtrl)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCtrl), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_STAT_LOGCTRL_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pCtrl, &stat_cfg.ctrl, sizeof(rtk_stat_log_ctrl_t));

    return RT_ERR_OK;
}   /* end of rtk_stat_logCtrl_get */

/* Function Name:
 *      rtk_stat_mibCntMode_get
 * Description:
 *      Get the MIB data update mode
 * Input:
 *      None
 * Output:
 *      pCnt_mode   - pointer buffer of MIB data update mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_stat_mibCntMode_get(rtk_mib_count_mode_t *pCnt_mode)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCnt_mode), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_STAT_MIBCNTMODE_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pCnt_mode, &stat_cfg.cnt_mode, sizeof(rtk_mib_count_mode_t));

    return RT_ERR_OK;
}   /* end of rtk_stat_mibCntMode_get */

/* Function Name:
 *      rtk_stat_mibCntMode_set
 * Description:
 *      Set MIB data update mode
 * Input:
 *      cnt_mode        - MIB counter update mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_mibCntMode_set(rtk_mib_count_mode_t cnt_mode)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    osal_memcpy(&stat_cfg.cnt_mode, &cnt_mode, sizeof(rtk_mib_count_mode_t));
    SETSOCKOPT(RTDRV_STAT_MIBCNTMODE_SET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_mibCntMode_set */

/* Function Name:
 *      rtk_stat_mibLatchTimer_get
 * Description:
 *      Get the MIB latch timer
 * Input:
 *      None
 * Output:
 *      pTimer   - pointer buffer of MIB latch timer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_stat_mibLatchTimer_get(uint32 *pTimer)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimer), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.timer, pTimer, sizeof(uint32));
    GETSOCKOPT(RTDRV_STAT_MIBLATCHTIMER_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pTimer, &stat_cfg.timer, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_stat_mibLatchTimer_get */

/* Function Name:
 *      rtk_stat_mibLatchTimer_set
 * Description:
 *      Set MIB data update mode
 * Input:
 *      timer        - MIB latch timer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_mibLatchTimer_set(uint32 timer)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    osal_memcpy(&stat_cfg.timer, &timer, sizeof(uint32));
    SETSOCKOPT(RTDRV_STAT_MIBLATCHTIMER_SET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_mibLatchTimer_set */

/* Function Name:
 *      rtk_stat_mibSyncMode_get
 * Description:
 *      Get the MIB register data update mode
 * Input:
 *      None
 * Output:
 *      pSync_mode   - pointer buffer of MIB register data update mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_stat_mibSyncMode_get(rtk_mib_sync_mode_t *pSync_mode)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSync_mode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.sync_mode, pSync_mode, sizeof(rtk_mib_sync_mode_t));
    GETSOCKOPT(RTDRV_STAT_MIBSYNCMODE_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pSync_mode, &stat_cfg.sync_mode, sizeof(rtk_mib_sync_mode_t));

    return RT_ERR_OK;
}   /* end of rtk_stat_mibSyncMode_get */

/* Function Name:
 *      rtk_stat_mibSyncMode_set
 * Description:
 *      Set MIB register data update mode
 * Input:
 *      sync_mode        - MIB register data update mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_mibSyncMode_set(rtk_mib_sync_mode_t sync_mode)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    osal_memcpy(&stat_cfg.sync_mode, &sync_mode, sizeof(rtk_mib_sync_mode_t));
    SETSOCKOPT(RTDRV_STAT_MIBSYNCMODE_SET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_mibSyncMode_set */

/* Function Name:
 *      rtk_stat_mibCntTagLen_get
 * Description:
 *      Get counting Tag length state in tx/rx packet
 * Input:
 *      direction - count tx or rx tag length
 * Output:
 *      pState   - pointer buffer of count tx/rx tag length state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_stat_mibCntTagLen_get(rtk_mib_tag_cnt_dir_t direction, rtk_mib_tag_cnt_state_t *pState)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.direction, &direction, sizeof(rtk_mib_tag_cnt_dir_t));
    GETSOCKOPT(RTDRV_STAT_MIBCNTTAGLEN_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pState, &stat_cfg.state, sizeof(rtk_mib_tag_cnt_state_t));

    return RT_ERR_OK;
}   /* end of rtk_stat_mibCntTagLen_get */

/* Function Name:
 *      rtk_stat_mibCntTagLen_set
 * Description:
 *      Set counting length including Ctag length or excluding Ctag length for tx/rx packet
 * Input:
 *      direction - count tx or rx tag length
 *      enable    - count tag length state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_mibCntTagLen_set(rtk_mib_tag_cnt_dir_t direction, rtk_mib_tag_cnt_state_t state)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    osal_memcpy(&stat_cfg.direction, &direction, sizeof(rtk_mib_tag_cnt_dir_t));
    osal_memcpy(&stat_cfg.state, &state, sizeof(rtk_mib_tag_cnt_state_t));
    SETSOCKOPT(RTDRV_STAT_MIBCNTTAGLEN_SET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_mibCntTagLen_set */

/* Function Name:
 *      rtk_stat_pktInfo_get
 * Description:
 *      Get the newest packet trap/drop reason
 * Input:
 *      port - port index
 * Output:
 *      pCode   - the newest packet trap/drop reason code
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
rtk_stat_pktInfo_get(rtk_port_t port, uint32 *pCode)
{
    rtdrv_statCfg_t stat_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stat_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_STAT_PKTINFO_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pCode, &stat_cfg.code, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_stat_pktInfo_get */

/* Function Name:
 *      rtk_stat_hostCnt_reset
 * Description:
 *      Reset the specified host statistic counters.
 * Input:
 *      index - host index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_stat_hostCnt_reset(uint32 index)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    osal_memcpy(&stat_cfg.index, &index, sizeof(uint32));
    SETSOCKOPT(RTDRV_STAT_HOSTCNT_RESET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stat_hostCnt_reset*/

/* Function Name:
 *     rtk_stat_hostCnt_get
 * Description:
 *      Get host statistic counters.
 * Input:
 *      index  - host index
 *      cntrIdx - specified host counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - invalid index
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_stat_hostCnt_get(uint32 index, rtk_stat_host_cnt_type_t cntrIdx, uint64 *pCntr)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memcpy(&stat_cfg.index, &index, sizeof(uint32));
    osal_memcpy(&stat_cfg.hostCntrsIdx, &cntrIdx, sizeof(rtk_stat_host_cnt_type_t));
    GETSOCKOPT(RTDRV_STAT_HOSTCNT_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pCntr, &stat_cfg.cntr, sizeof(uint64));

    return RT_ERR_OK;
} /* end of rtk_stat_hostCnt_get */


/* Function Name:
 *      rtk_stat_hostState_get
 * Description:
 *      Get enable status of host statistic counting function
 * Input:
 *      index   - host index
 * Output:
 *      pState  -statistic counting state.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial 
 *      RT_ERR_INPUT            - Invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_stat_hostState_get(
    uint32 index,
    rtk_enable_t    *pState)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memcpy(&stat_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_STAT_HOSTSTATE_GET, &stat_cfg, rtdrv_statCfg_t, 1);
    osal_memcpy(pState, &stat_cfg.enState, sizeof(rtk_enable_t));


    return RT_ERR_OK;
} /* end of rtk_stat_hostState_get */

/* Function Name:
 *      rtk_stat_hostState_set
 * Description:
 *      Set state of host statistic counting function
 * Input:
 *      index   - host index
 *      state   - statistic counting state.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial 
 *      RT_ERR_INPUT            - Invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_stat_hostState_set(
    uint32 index,
    rtk_enable_t    state)
{
    rtdrv_statCfg_t stat_cfg;

    /* function body */
    osal_memset(&stat_cfg, 0x0, sizeof(rtdrv_statCfg_t));
    osal_memcpy(&stat_cfg.index, &index, sizeof(uint32));
    osal_memcpy(&stat_cfg.enState, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_STAT_HOSTSTATE_SET, &stat_cfg, rtdrv_statCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_stat_hostState_set */


