/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision: $
 * $Date: $
 *
 * Purpose : EPON MAC register access APIs
 *
 * Feature : Provide the APIs to access EPON MAC
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
#include <hal/chipdef/chip.h>
#include <rtk/rt/rt_epon.h>

/* Function Name:
 *      rt_epon_init
 * Description:
 *      epon register level initial function
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_init(void)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;
    
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));
    /* function body */
    SETSOCKOPT(RTDRV_RT_EPON_INIT, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_epon_init */

/* Function Name:
 *      rt_epon_llid_entry_get
 * Description:
 *      Get llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_llid_entry_get(rt_epon_llid_entry_t *pLlidEntry)  
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.llidEntry, pLlidEntry, sizeof(rt_epon_llid_entry_t));
    GETSOCKOPT(RTDRV_RT_EPON_LLID_ENTRY_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pLlidEntry, &rt_epon_cfg.llidEntry, sizeof(rt_epon_llid_entry_t));

    return RT_ERR_OK;
}   /* end of rt_epon_llid_entry_get */

/* Function Name:
 *      rt_epon_llid_entry_set
 * Description:
 *      Set llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_llid_entry_set(rt_epon_llid_entry_t *pLlidEntry)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);

    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* function body */
    osal_memcpy(&rt_epon_cfg.llidEntry, pLlidEntry, sizeof(rt_epon_llid_entry_t));
    SETSOCKOPT(RTDRV_RT_EPON_LLID_ENTRY_SET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_epon_llid_entry_set */

/* Function Name:
 *      rt_epon_registerReq_get
 * Description:
 *      Get register request relative parameter
 * Input:
 *      pRegEntry : register request relative parament
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_registerReq_get(rt_epon_regReq_t *pRegEntry)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.regEntry, pRegEntry, sizeof(rt_epon_regReq_t));
    GETSOCKOPT(RTDRV_RT_EPON_REGISTERREQ_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pRegEntry, &rt_epon_cfg.regEntry, sizeof(rt_epon_regReq_t));

    return RT_ERR_OK;
}   /* end of rt_epon_registerReq_get */

/* Function Name:
 *      rt_epon_registerReq_set
 * Description:
 *      Set register request relative parameter
 * Input:
 *       None
 * Output:
 *       pRegEntry : register request relative parament 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_registerReq_set(rt_epon_regReq_t *pRegEntry)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* function body */
    osal_memcpy(&rt_epon_cfg.regEntry, pRegEntry, sizeof(rt_epon_regReq_t));
    SETSOCKOPT(RTDRV_RT_EPON_REGISTERREQ_SET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_epon_registerReq_set */

/* Function Name:
 *      rt_epon_churningKey_get
 * Description:
 *      Get churning key entry
 * Input:
 *       None
 * Output:
 *       pEntry : churning key relative parameter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_churningKey_get(rt_epon_churningKeyEntry_t *pEntry)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.entry, pEntry, sizeof(rt_epon_churningKeyEntry_t));
    GETSOCKOPT(RTDRV_RT_EPON_CHURNINGKEY_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pEntry, &rt_epon_cfg.entry, sizeof(rt_epon_churningKeyEntry_t));

    return RT_ERR_OK;
}   /* end of rt_epon_churningKey_get */




/* Function Name:
 *      rt_epon_churningStatus_get
 * Description:
 *      Get EPON tripple churning status
 * Input:
 *	    none
 * Output:
 *      pEnabled: pointer of status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
int32 rt_epon_churningStatus_get(rt_enable_t *pState)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.state, pState, sizeof(rt_enable_t));
    GETSOCKOPT(RTDRV_RT_EPON_CHURNING_STATE_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pState, &rt_epon_cfg.state, sizeof(rt_enable_t));

    return RT_ERR_OK;
} /* end of rt_epon_churningStatus_get */



/* Function Name:
 *      rt_epon_churningKey_set
 * Description:
 *      Set churning key entry
 * Input:
 *       pEntry : churning key relative parameter 
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_churningKey_set(rt_epon_churningKeyEntry_t *pEntry)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* function body */
    osal_memcpy(&rt_epon_cfg.entry, pEntry, sizeof(rt_epon_churningKeyEntry_t));
    SETSOCKOPT(RTDRV_RT_EPON_CHURNINGKEY_SET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_epon_churningKey_set */

/* Function Name:
 *      rt_epon_usFecState_get
 * Description:
 *      Get upstream fec state
 * Input:
 *       None
 * Output:
 *       *pState : upstream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_usFecState_get(rt_enable_t *pState)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.state, pState, sizeof(rt_enable_t));
    GETSOCKOPT(RTDRV_RT_EPON_USFECSTATE_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pState, &rt_epon_cfg.state, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_epon_usFecState_get */


/* Function Name:
 *      rt_epon_usFecState_set
 * Description:
 *      Set upstream fec state
 * Input:
 *       state : upstream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_usFecState_set(rt_enable_t state)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* function body */
    osal_memcpy(&rt_epon_cfg.state, &state, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_EPON_USFECSTATE_SET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_epon_usFecState_set */


/* Function Name:
 *      rt_epon_dsFecState_get
 * Description:
 *      Get down-stream fec state
 * Input:
 *       None
 * Output:
 *       *pState : down-stream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_dsFecState_get(rt_enable_t *pState)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.state, pState, sizeof(rt_enable_t));
    GETSOCKOPT(RTDRV_RT_EPON_DSFECSTATE_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pState, &rt_epon_cfg.state, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_epon_dsFecState_get */


/* Function Name:
 *      rt_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       state : down-stream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_dsFecState_set(rt_enable_t state)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* function body */
    osal_memcpy(&rt_epon_cfg.state, &state, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_EPON_DSFECSTATE_SET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_epon_dsFecState_set */

/* Function Name:
 *      rt_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       None
 * Output:
 *       pCounter : EPON mib counter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_mibCounter_get(rt_epon_counter_t *pCounter)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCounter), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.counter, pCounter, sizeof(rt_epon_counter_t));
    GETSOCKOPT(RTDRV_RT_EPON_MIBCOUNTER_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pCounter, &rt_epon_cfg.counter, sizeof(rt_epon_counter_t));

    return RT_ERR_OK;
}   /* end of rt_epon_mibCounter_get */


/* Function Name:
 *      rt_epon_mibCounter_reset
 * Description:
 *      Clear the statistics for EPON mac.
 * Input:
 *       None
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      This API routine is used to claer statistics data for EPON mac.
 */
int32 rt_epon_mibCounter_reset(void)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    memset(&rt_epon_cfg,0x0,sizeof(rtdrv_rt_eponCfg_t));
    /* function body */
    SETSOCKOPT(RTDRV_RT_EPON_MIBCOUNTER_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;    
}

/* Function Name:
 *      rt_epon_losState_get
 * Description:
 *      Get laser lose of signal state.
 * Input:
 *      pState LOS state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rt_epon_losState_get(rt_enable_t *pState)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.state, pState, sizeof(rt_enable_t));
    GETSOCKOPT(RTDRV_RT_EPON_LOSSTATE_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pState, &rt_epon_cfg.state, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_epon_losState_get */

/* Function Name:
 *      rt_epon_ponQueue_set
 * Description:
 *      Set the pon queue
 * Input:
 *      llid    - logical llid index
 *      queueId  - logical llid queue id
 *      pQueuecfg  - pointer to the queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_epon_ponQueue_set(uint32 llid, uint32 queueId, rt_epon_queueCfg_t *pQueuecfg)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueuecfg), RT_ERR_NULL_POINTER);
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* function body */
    osal_memcpy(&rt_epon_cfg.llid, &llid, sizeof(uint32));
    osal_memcpy(&rt_epon_cfg.queueId, &queueId, sizeof(uint32));
    osal_memcpy(&rt_epon_cfg.queuecfg, pQueuecfg, sizeof(rt_epon_queueCfg_t));
    SETSOCKOPT(RTDRV_RT_EPON_PONQUEUE_SET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_epon_ponQueue_set */

/* Function Name:
 *      rt_epon_ponQueue_get
 * Description:
 *      Get the pon queue
 * Input:
 *      llid    - logical llid index
 *      queueId  - logical llid queue id
 * Output:
 *      pQueuecfg - pointer to the queue configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_epon_ponQueue_get(uint32 llid, uint32 queueId, rt_epon_queueCfg_t *pQueuecfg)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueuecfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.llid, &llid, sizeof(uint32));
    osal_memcpy(&rt_epon_cfg.queueId, &queueId, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_EPON_PONQUEUE_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pQueuecfg, &rt_epon_cfg.queuecfg, sizeof(rt_epon_queueCfg_t));

    return RT_ERR_OK;
}   /* end of rt_epon_ponQueue_get */

/* Function Name:
 *      rt_epon_ponQueue_del
 * Description:
 *      Delete the pon queue
 * Input:
 *      llid    - logical llid index
 *      queueId  - logical llid queue id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
rt_epon_ponQueue_del(uint32 llid, uint32 queueId)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* function body */
    osal_memcpy(&rt_epon_cfg.llid, &llid, sizeof(uint32));
    osal_memcpy(&rt_epon_cfg.queueId, &queueId, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_EPON_PONQUEUE_DEL, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_epon_ponQueue_del */

/* Function Name:
 *      rt_epon_egrBandwidthCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      None
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
int32 rt_epon_egrBandwidthCtrlRate_get(uint32 *pRate)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.rate, pRate, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_EPON_EGRBANDWIDTHCTRLRATE_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pRate, &rt_epon_cfg.rate, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rt_epon_egrBandwidthCtrlRate_get */

/* Function Name:
 *      rt_epon_egrBandwidthCtrlRate_set
 * Description:
 *      Set the pon port egress bandwidth control rate.
 * Input:
 *      rate - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
int32 rt_epon_egrBandwidthCtrlRate_set(uint32 rate)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* function body */
    osal_memcpy(&rt_epon_cfg.rate, &rate, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_EPON_EGRBANDWIDTHCTRLRATE_SET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_epon_egrBandwidthCtrlRate_set */




/* Function Name:
 *      rt_epon_mpcp_info_get
 * Description:
 *      Get epon mpcp information
 * Input:
 * Output:
 *      info - mpcp relative information 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *
 *   This API routine is used to get generic ONU MPCP information and can be invoked at any time. 
 *   However some of fields like laser on laser_off sync_time and olt_discinfo will be valid if ONU is not in registered
 *   state. Note that field onu_discinfo and olt_discinfo are only appropriate for 10G ONU. 
 */
int32 
rt_epon_mpcp_info_get(rt_epon_mpcp_info_t *info)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.info, info, sizeof(rt_epon_mpcp_info_t));
    GETSOCKOPT(RTDRV_RT_EPON_MPCP_INFO_GET,            &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(info, &rt_epon_cfg.info, sizeof(rt_epon_mpcp_info_t));

    return RT_ERR_OK;
}   /* end of rt_epon_mpcp_info_get */



/* Function Name:
 *      rt_epon_mpcp_queue_threshold_set
 * Description:
 *      Set epon threshold report
 * Input:
 *      llidIdx       - llid index
 *      pThresholdRpt - threshole report setting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 * Note:
 *      The the report level 1 for normal report type, max value is 4.
 */
int32 rt_epon_mpcp_queue_threshold_set(rt_epon_report_threshold_t *pThresholdRpt)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresholdRpt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.thresholdRpt, pThresholdRpt, sizeof(rt_epon_report_threshold_t));
    SETSOCKOPT(RTDRV_RT_EPON_MPCP_QUEUE_THRESHOLD_SET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);

    return RT_ERR_OK;      
}   /* end of rt_epon_mpcp_queue_threshold_set */

/* Function Name:
 *      rt_epon_mpcp_queue_threshold_get
 * Description:
 *      Get epon threshold report setting
 * Input:
 *      pThresholdRpt - threshole report setting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 * Note:
 */
int32
rt_epon_mpcp_queue_threshold_get(rt_epon_report_threshold_t *pThresholdRpt)
{
    rtdrv_rt_eponCfg_t rt_epon_cfg;
    memset(&rt_epon_cfg, 0x0, sizeof(rtdrv_rt_eponCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresholdRpt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_epon_cfg.thresholdRpt, pThresholdRpt, sizeof(rt_epon_report_threshold_t));
    GETSOCKOPT(RTDRV_RT_EPON_MPCP_QUEUE_THRESHOLD_GET, &rt_epon_cfg, rtdrv_rt_eponCfg_t, 1);
    osal_memcpy(pThresholdRpt, &rt_epon_cfg.thresholdRpt, sizeof(rt_epon_report_threshold_t));



    return RT_ERR_OK;   
    
}   /* end of rt_epon_mpcp_queue_threshold_get */
