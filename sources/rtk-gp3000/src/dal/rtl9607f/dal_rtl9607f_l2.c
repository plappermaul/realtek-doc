/*
 * Copyright (C) 2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date: $
 *
 * Purpose : Definition of L2 API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Mac address flush
 *           (2) Address learning limit
 *           (3) Parameter for L2 lookup and learning engine
 *           (4) Unicast address
 *           (5) L2 multicast
 *           (6) IP multicast
 *           (7) Multicast forwarding table
 *           (8) CPU mac
 *           (9) Port move
 *           (10) Parameter for lookup miss
 *           (11) Parameter for MISC
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_l2.h>
#include <osal/time.h>

#include <ca_event.h>
#include <event_core.h>

#include <cortina-api/include/l2.h>
#include <cortina-api/include/port.h>
#include <cortina-api/include/eth_port.h>
#include <aal_arb.h>
#include <aal_port.h>
#include <aal_fdb.h>
#include <aal_mcast.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32 l2_init = INIT_NOT_COMPLETED;

static rtk_enable_t LUNA_G3_L2_FLUSH_INTR = DISABLED;

/*
 * Macro Declaration
 */

/* Module Name    : L2                                          */
/* Sub-module Name: Parameter for L2 lookup and learning engine */
  
ca_uint32_t ca_rtk_link_down_l2_flush_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    aal_fdb_cpu_cmd_word4_msk_t  flush_mask;
    aal_fdb_flush_cfg_t          flush_cfg;
    ca_uint32_t                  aal_port = 0;
    ca_status_t ret=CA_E_OK;
    ca_event_eth_port_link_t* eth_port_link_ptr = (ca_event_eth_port_link_t *)event;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    if(eth_port_link_ptr->status == CA_PORT_LINK_DOWN)
    {

        aal_port = PORT_ID(eth_port_link_ptr->port_id);

        flush_mask.wrd = ~0U;
        memset(&flush_cfg,0,sizeof(aal_fdb_flush_cfg_t));

        flush_mask.bf.lpid  = 0;
        flush_cfg.lpid_high = aal_port;
        flush_cfg.lpid_low  = aal_port;

        flush_mask.bf.static_flag = 0;
        flush_cfg.static_flag     = 0;

        if((ret = aal_fdb_scan_flush_field_set(0, flush_mask, &flush_cfg)!=CA_E_OK))
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return CA_EVT_ERROR;
        }

        if((ret = aal_fdb_entry_flush(0)!=CA_E_OK))
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return CA_EVT_ERROR;
        }

    }

    return CA_EVT_OK;
}

int32 __dal_rtl9607f_nextValidAddr_Get(
    rtk_enable_t        isByPort,
    rtk_port_t          port,
    int32               *pScanIdx,
    aal_fdb_entry_cfg_t  *pFdbEntry)

{
    ca_uint32_t hit_addr = 0, start_addr = 0;
    ca_uint32_t ret = CA_E_OK, cmd_ret = 0;

    aal_fdb_flush_cfg_t          flush_cfg;
    aal_fdb_cpu_cmd_word4_msk_t  mask;

    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFdbEntry), RT_ERR_NULL_POINTER);


    memset((void *)&flush_cfg, 0, sizeof(aal_fdb_flush_cfg_t));

    aal_fdb_cmp_cfg_load(&flush_cfg);

    mask.wrd = ~0U;

    /* only join compare lpid */
    if(isByPort)
    {
        mask.bf.lpid = 0;
        flush_cfg.lpid_high = port;
        flush_cfg.lpid_low  = port;
    }

    ret = aal_fdb_scan_flush_field_set(0, mask, &flush_cfg);
    if(ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    start_addr = *pScanIdx;

    memset(pFdbEntry, 0, sizeof(aal_fdb_entry_cfg_t));

    ret = aal_fdb_entry_scan(0,
                            start_addr, &hit_addr,
                            pFdbEntry, &cmd_ret);



    if (ret != CA_E_OK || cmd_ret != CA_AAL_FDB_ACCESS_HIT) {        
         return RT_ERR_L2_ENTRY_NOTFOUND;
    }
#if 0
    printk("hit_addr:%d, mac:0x%x:%x:%x:%x:%x:%x, key_vid:%d, sc_ind:%d, dot1p:%d, lrnt_vid:%d, sa:%d, da:%d, static:%d, aging:%d, mc:%d, pid:%d, valid:%d, tmp:%d\r\n",
            hit_addr,
            pFdbEntry->mac[0], pFdbEntry->mac[1], pFdbEntry->mac[2], pFdbEntry->mac[3], pFdbEntry->mac[4], pFdbEntry->mac[5],
            pFdbEntry->key_vid    ,
            pFdbEntry->key_sc_ind ,
            pFdbEntry->dot1p      ,
            pFdbEntry->lrnt_vid   ,
            pFdbEntry->sa_permit  ,
            pFdbEntry->da_permit  ,
            pFdbEntry->static_flag,
            pFdbEntry->aging_sts  ,
            pFdbEntry->mc_flag    ,
            pFdbEntry->port_id,
            pFdbEntry->valid,
            pFdbEntry->tmp_flag);
#endif
    *pScanIdx = hit_addr;

    return RT_ERR_OK;
}

int32
dal_rtl9607f_l2_init(void)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    l2_init = INIT_COMPLETED;

    /* unit 0.1 sec to 1 sec */
    if((ret = dal_rtl9607f_l2_aging_set(RTK_L2_DEFAULT_AGING_TIME / 10)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_init */


/* Function Name:
 *      dal_rtl9607f_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9607f_l2_flushLinkDownPortAddrEnable_get(rtk_enable_t *pEnable)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    *pEnable = LUNA_G3_L2_FLUSH_INTR;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_flushLinkDownPortAddrEnable_get */


/* Function Name:
 *      dal_rtl9607f_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration.
 * Input:
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9607f_l2_flushLinkDownPortAddrEnable_set(rtk_enable_t enable)
{
    ca_status_t ret=CA_E_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(LUNA_G3_L2_FLUSH_INTR == DISABLED && enable == ENABLED)
    {
        if((ret = ca_event_register(0,CA_EVENT_ETH_PORT_LINK,ca_rtk_link_down_l2_flush_intr,NULL))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
    }
    if(LUNA_G3_L2_FLUSH_INTR == ENABLED && enable == DISABLED)
    {
        if((ret = ca_event_deregister(0,CA_EVENT_ETH_PORT_LINK,ca_rtk_link_down_l2_flush_intr))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
    }

    LUNA_G3_L2_FLUSH_INTR = enable;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_flushLinkDownPortAddrEnable_set */

/* Function Name:
 *      dal_rtl9607f_l2_portLimitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
int32
dal_rtl9607f_l2_portLimitLearningCnt_get(rtk_port_t port, uint32 *pMacCnt)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_uint32_t number;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    if((ret = aal_fdb_port_limit_get(0, port, &number))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    *pMacCnt = number;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_portLimitLearningCnt_get */


/* Function Name:
 *      dal_rtl9607f_l2_portLimitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the port.
 * Input:
 *      port    - port id
 *      macCnt  - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the macCnt to 0 mean disable learning in the port.
 */
int32
dal_rtl9607f_l2_portLimitLearningCnt_set(rtk_port_t port, uint32 macCnt)
{
    ca_status_t ret=CA_E_OK;
    ca_uint32_t number;
    ca_port_id_t port_id;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);


    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L2_LEARN_LIMIT_CNT_MAX() < macCnt), RT_ERR_INPUT);

    number = macCnt;

    if((ret = aal_fdb_port_limit_set(0, port, number))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_portLimitLearningCnt_set */

/* Function Name:
 *      dal_rtl9607f_l2_portLimitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the port.
 * Input:
 *      port    - port id
 * Output:
 *      pLearningAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
int32
dal_rtl9607f_l2_portLimitLearningCntAction_get(rtk_port_t port, rtk_l2_limitLearnCntAction_t *pLearningAction)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    aal_ilpb_cfg_t ilpb_cfg;
    lrn_fwd_ctrl_t fdbFwdCtrl;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLearningAction), RT_ERR_NULL_POINTER);

    if((ret = aal_port_ilpb_cfg_get(0,port, &ilpb_cfg))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }


    if(ilpb_cfg.sa_lmt_excd_drop)
    {
        *pLearningAction = LIMIT_LEARN_CNT_ACTION_DROP;
    }
    else
    {
        if((ret = aal_fdb_lrn_fwd_ctrl_get(0, &fdbFwdCtrl))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }

        if(SA_LRN_FWD_CTRL_FORWARD == fdbFwdCtrl.limit_excd_mode)
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_FORWARD;
        else    
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_TO_CPU;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_portLimitLearningCntAction_get */


/* Function Name:
 *      dal_rtl9607f_l2_portLimitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the port.
 * Input:
 *      port   - port id
 *      learningAction - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
int32
dal_rtl9607f_l2_portLimitLearningCntAction_set(rtk_port_t port, rtk_l2_limitLearnCntAction_t learningAction)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;

    lrn_fwd_ctrl_mask_t fdbFwdCtrlMask;
    lrn_fwd_ctrl_t fdbFwdCtrl;
    aal_ilpb_cfg_msk_t ilpb_msk;
    aal_ilpb_cfg_t ilpb_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((LIMIT_LEARN_CNT_ACTION_END <= learningAction), RT_ERR_INPUT);


    if(learningAction == LIMIT_LEARN_CNT_ACTION_FORWARD)
    {
        fdbFwdCtrlMask.wrd = 0;
        memset(&fdbFwdCtrl, 0, sizeof(lrn_fwd_ctrl_t));

        fdbFwdCtrlMask.bf.sa_limit_excd_fwd_ctrl = 1;
        fdbFwdCtrl.limit_excd_mode = SA_LRN_FWD_CTRL_FORWARD;   // forward if over limit        
        if((ret = aal_fdb_lrn_fwd_ctrl_set(0, fdbFwdCtrlMask, &fdbFwdCtrl))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }

        memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
        memset(&ilpb_msk, 0, sizeof(aal_ilpb_cfg_msk_t)); //per port drop  

        ilpb_msk.s.sa_lmt_excd_drop = 1;
        ilpb_cfg.sa_lmt_excd_drop = 0;
        if((ret = aal_port_ilpb_cfg_set(0,port, ilpb_msk, &ilpb_cfg))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
    }
    else if(learningAction == LIMIT_LEARN_CNT_ACTION_DROP)  
    {
        memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
        memset(&ilpb_msk, 0, sizeof(aal_ilpb_cfg_msk_t)); //per port drop  

        ilpb_msk.s.sa_lmt_excd_drop = 1;
        ilpb_cfg.sa_lmt_excd_drop = 1;
        if((ret = aal_port_ilpb_cfg_set(0,port, ilpb_msk, &ilpb_cfg))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }

    }
    else if(learningAction == LIMIT_LEARN_CNT_ACTION_TO_CPU)
    {
        fdbFwdCtrlMask.wrd = 0;
        memset(&fdbFwdCtrl, 0, sizeof(lrn_fwd_ctrl_t));

        fdbFwdCtrlMask.bf.sa_limit_excd_fwd_ctrl = 1;
        fdbFwdCtrlMask.bf.sa_limit_excd_ldpid = 1;
        fdbFwdCtrl.limit_excd_mode = SA_LRN_FWD_CTRL_REDIRECT;   // trap to cpu if over limit        
        fdbFwdCtrl.limit_excd_ldpid = AAL_LPORT_CPU_0; 
        if((ret = aal_fdb_lrn_fwd_ctrl_set(0, fdbFwdCtrlMask, &fdbFwdCtrl))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }

        memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
        memset(&ilpb_msk, 0, sizeof(aal_ilpb_cfg_msk_t)); //per port drop  

        ilpb_msk.s.sa_lmt_excd_drop = 1;
        ilpb_cfg.sa_lmt_excd_drop = 0;
        if((ret = aal_port_ilpb_cfg_set(0,port, ilpb_msk, &ilpb_cfg))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }

    }
    else
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_portLimitLearningCntAction_set */

/* Function Name:
 *      da_rtl9607f_l2_aging_get
 * Description:
 *      Get the dynamic address aging time.
 * Input:
 *      None
 * Output:
 *      pAgingTime - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Get aging_time as 0 mean disable aging mechanism. (0.1sec)
 */
int32
dal_rtl9607f_l2_aging_get(uint32 *pAgingTime)
{
    ca_status_t ret=CA_E_OK;
    ca_uint32_t time=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAgingTime), RT_ERR_NULL_POINTER);

    if((ret = aal_fdb_aging_time_get(0,&time))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    *pAgingTime=time;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_aging_get */


/* Function Name:
 *      dal_rtl9607f_l2_aging_set
 * Description:
 *      Set the dynamic address aging time.
 * Input:
 *      agingTime - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) RTL8329/RTL8389 aging time is not configurable.
 *      (2) apply aging_time as 0 mean disable aging mechanism.
 */
int32
dal_rtl9607f_l2_aging_set(uint32 agingTime)
{
    ca_status_t ret=CA_E_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L2_AGING_TIME_MAX() < agingTime), RT_ERR_INPUT);

    if((ret = aal_fdb_aging_time_set(0,agingTime))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_aging_set */

/* Function Name:
 *      dal_rtl9607f_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out setting of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of Age
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9607f_l2_portAgingEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    ca_status_t ret=CA_E_OK;
    aal_fdb_ctrl_t       fdb_ctrl;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = aal_fdb_ctrl_get(0,  &fdb_ctrl))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    *pEnable = fdb_ctrl.aging_en;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_portAgingEnable_get */

/* Function Name:
 *      dal_rtl9607f_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out configuration of the specified port
 * Input:
 *      port    - port id
 *      enable  - enable status of Age
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9607f_l2_portAgingEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    ca_status_t ret=CA_E_OK;
    aal_fdb_ctrl_mask_t  fdb_ctrl_mask;
    aal_fdb_ctrl_t       fdb_ctrl;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    memset(&fdb_ctrl, 0, sizeof(aal_fdb_ctrl_t));
    memset(&fdb_ctrl_mask, 0, sizeof(aal_fdb_ctrl_mask_t));

    fdb_ctrl_mask.s.aging_en = 1;
    fdb_ctrl.aging_en = enable;

    if((ret = aal_fdb_ctrl_set(0, fdb_ctrl_mask,  &fdb_ctrl))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_portAgingEnable_set */

/* Module Name    : L2      */
/* Sub-module Name: Parameter for lookup miss */
/* Function Name:
 *      dal_rtl9607f_l2_lookupMissAction_get
 * Description:
 *      Get forwarding action when destination address lookup miss.
 * Input:
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FLOOD_IN_VLAN
 *      - ACTION_FLOOD_IN_ALL_PORT  (only for DLF_TYPE_MCAST)
 *      - ACTION_FLOOD_IN_ROUTER_PORTS (only for DLF_TYPE_IPMC)
 *      - ACTION_FOLLOW_FB
 */
int32
dal_rtl9607f_l2_lookupMissAction_get(rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if((ret = dal_rtl9607f_l2_portLookupMissAction_get(0, type, pAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_lookupMissAction_get */

/* Function Name:
 *      dal_rtl9607f_l2_lookupMissAction_set
 * Description:
 *      Set forwarding action when destination address lookup miss.
 * Input:
 *      type   - type of lookup miss
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - invalid type of lookup miss
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FLOOD_IN_VLAN
 *      - ACTION_FLOOD_IN_ALL_PORT  (only for DLF_TYPE_MCAST)
 *      - ACTION_FLOOD_IN_ROUTER_PORTS (only for DLF_TYPE_IPMC)
 *      - ACTION_FOLLOW_FB
 */
int32
dal_rtl9607f_l2_lookupMissAction_set(rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    int32 ret;
    rtk_port_t port;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_END <= action), RT_ERR_INPUT);

    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = dal_rtl9607f_l2_portLookupMissAction_set(port, type, action)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_lookupMissAction_set */

/* Function Name:
 *      dal_rtl9607f_l2_portLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_FOLLOW_FB
 */
int32
dal_rtl9607f_l2_portLookupMissAction_get(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    ca_status_t ret=CA_E_OK;
    ca_uint16_t lspid;
    aal_ple_nonkwn_type_t ca_type;
    aal_ple_dft_fwd_ctrl_t cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= DLF_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    lspid = port;

    switch(type)
    {
        case DLF_TYPE_IPMC:
            ca_type = AAL_PLE_NONKWN_TYPE_UL3MC;
            break;
        case DLF_TYPE_IP6MC:
            ca_type = AAL_PLE_NONKWN_TYPE_UL3MC;
            break;
        case DLF_TYPE_UCAST:
            ca_type = AAL_PLE_NONKWN_TYPE_UUC;
            break;
        case DLF_TYPE_MCAST:
            ca_type = AAL_PLE_NONKWN_TYPE_UL2MC;
            break;
        case DLF_TYPE_BCAST:
            ca_type = AAL_PLE_NONKWN_TYPE_BC;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if((ret = aal_ple_dft_fwd_get(0,lspid,ca_type,&cfg))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    if(cfg.dft_fwd_redir_en == 1)
    {
        if(cfg.dft_fwd_mc_group_id == AAL_LPORT_L3_WAN || cfg.dft_fwd_mc_group_id == AAL_LPORT_L3_LAN)
            *pAction = ACTION_FOLLOW_FB;
        else
            *pAction = ACTION_TRAP2CPU;
    }
    else
    {
        if(cfg.dft_fwd_mc_group_id_valid == 1)
            *pAction = ACTION_DROP;
        else
            *pAction = ACTION_FORWARD;
    }

    return RT_ERR_OK;

} /* end of dal_rtl9607f_l2_portLookupMissAction_get */

/* Function Name:
 *      dal_rtl9607f_l2_portLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 *      action  - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_FOLLOW_FB
 */
int32
dal_rtl9607f_l2_portLookupMissAction_set(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    ca_status_t ret=CA_E_OK;
    ca_uint16_t lspid;
    aal_ple_nonkwn_type_t ca_type;
    aal_ple_dft_fwd_ctrl_mask_t mask;
    aal_ple_dft_fwd_ctrl_t cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= DLF_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    lspid = port;

    switch(type)
    {
        case DLF_TYPE_IPMC:
            ca_type = AAL_PLE_NONKWN_TYPE_UL3MC;
            break;
        case DLF_TYPE_IP6MC:
            ca_type = AAL_PLE_NONKWN_TYPE_UL3MC;
            break;
        case DLF_TYPE_UCAST:
            ca_type = AAL_PLE_NONKWN_TYPE_UUC;
            break;
        case DLF_TYPE_MCAST:
            ca_type = AAL_PLE_NONKWN_TYPE_UL2MC;
            break;
        case DLF_TYPE_BCAST:
            ca_type = AAL_PLE_NONKWN_TYPE_BC;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    memset(&mask,0,sizeof(aal_ple_dft_fwd_ctrl_mask_t));
    memset(&cfg,0,sizeof(aal_ple_dft_fwd_ctrl_t));

    switch(action)
    {
        case ACTION_DROP:
            cfg.dft_fwd_deny = 0;
            cfg.dft_fwd_redir_en = 0;
            cfg.dft_fwd_mc_group_id_valid = 1;
            cfg.dft_fwd_mc_group_id = RESERVED_MEC_INDEX;
            mask.bf.dft_fwd_deny = 1;
            mask.bf.dft_fwd_redir_en = 1;
            mask.bf.dft_fwd_mc_group_id_valid = 1;
            mask.bf.dft_fwd_mc_group_id = 1;
            break;
        case ACTION_TRAP2CPU:
            cfg.dft_fwd_deny = 0;
            cfg.dft_fwd_redir_en = 1;
            cfg.dft_fwd_mc_group_id_valid = 1;
            cfg.dft_fwd_mc_group_id = AAL_LPORT_CPU_0;
            mask.bf.dft_fwd_deny = 1;
            mask.bf.dft_fwd_redir_en = 1;
            mask.bf.dft_fwd_mc_group_id_valid = 1;
            mask.bf.dft_fwd_mc_group_id = 1;
            break;
        case ACTION_FORWARD:
            cfg.dft_fwd_deny = 0;
            cfg.dft_fwd_redir_en = 0;
            cfg.dft_fwd_mc_group_id_valid = 0;
            cfg.dft_fwd_mc_group_id = 0;
            mask.bf.dft_fwd_deny = 1;
            mask.bf.dft_fwd_redir_en = 1;
            mask.bf.dft_fwd_mc_group_id_valid = 1;
            mask.bf.dft_fwd_mc_group_id = 1;
            break;
        case ACTION_FOLLOW_FB:
            cfg.dft_fwd_deny = 0;
            cfg.dft_fwd_redir_en = 1;
            cfg.dft_fwd_mc_group_id_valid = 1;
            if(port == HAL_GET_PON_PORT())
                cfg.dft_fwd_mc_group_id = AAL_LPORT_L3_WAN;
            else
                cfg.dft_fwd_mc_group_id = AAL_LPORT_L3_LAN;
            mask.bf.dft_fwd_deny = 1;
            mask.bf.dft_fwd_redir_en = 1;
            mask.bf.dft_fwd_mc_group_id_valid = 1;
            mask.bf.dft_fwd_mc_group_id = 1;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if((ret = aal_ple_dft_fwd_set(0,lspid,ca_type,mask,&cfg))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

} /* end of dal_rtl9607f_l2_portLookupMissAction_set */

/* Function Name:
 *      dal_rtl9607f_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 *      include_static - the get type, include static mac or not.
 * Output:
 *      pL2UcastData   - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
dal_rtl9607f_l2_nextValidAddr_get(
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData)
{
    int32 ret;
    aal_fdb_entry_cfg_t  fdbEntry;

    if((ret = __dal_rtl9607f_nextValidAddr_Get(0, 0, pScanIdx, &fdbEntry))!=RT_ERR_OK)
    {
        if(ret == RT_ERR_L2_ENTRY_NOTFOUND)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_ENTRY_NOTFOUND;
        }
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }


    memcpy(pL2UcastData->mac.octet,fdbEntry.mac,sizeof(ca_mac_addr_t));
    pL2UcastData->vid       = fdbEntry.lrnt_vid;
    pL2UcastData->priority  = fdbEntry.dot1p;
    pL2UcastData->flags     = (fdbEntry.static_flag == 1)? pL2UcastData->flags|RTK_L2_UCAST_FLAG_STATIC : pL2UcastData->flags&(~RTK_L2_UCAST_FLAG_STATIC);
    pL2UcastData->flags     = (fdbEntry.sa_permit == 1)? pL2UcastData->flags&(~RTK_L2_UCAST_FLAG_SA_BLOCK) : pL2UcastData->flags|RTK_L2_UCAST_FLAG_SA_BLOCK;
    pL2UcastData->flags     = (fdbEntry.da_permit == 1)? pL2UcastData->flags&(~RTK_L2_UCAST_FLAG_DA_BLOCK) : pL2UcastData->flags|RTK_L2_UCAST_FLAG_DA_BLOCK;
    pL2UcastData->port      = fdbEntry.port_id;
    pL2UcastData->age       = fdbEntry.aging_sts;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9607f_l2_nextValidAddrOnPort_get
 * Description:
 *      Get next valid L2 unicast address entry from specify port.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 * Output:
 *      pL2UcastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
dal_rtl9607f_l2_nextValidAddrOnPort_get(
    rtk_port_t          port,
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData)
{
    int32 ret;

    aal_fdb_entry_cfg_t  fdbEntry;
  
    if((ret = __dal_rtl9607f_nextValidAddr_Get(1, port, pScanIdx, &fdbEntry))!=RT_ERR_OK)
    {
        if(ret == RT_ERR_L2_ENTRY_NOTFOUND)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_ENTRY_NOTFOUND;
        }
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }
        
    memcpy(pL2UcastData->mac.octet,fdbEntry.mac,sizeof(ca_mac_addr_t));
    pL2UcastData->vid       = fdbEntry.lrnt_vid;
    pL2UcastData->priority  = fdbEntry.dot1p;
    pL2UcastData->flags     = (fdbEntry.static_flag == 1)? pL2UcastData->flags|RTK_L2_UCAST_FLAG_STATIC : pL2UcastData->flags&(~RTK_L2_UCAST_FLAG_STATIC);
    pL2UcastData->flags     = (fdbEntry.sa_permit == 1)? pL2UcastData->flags&(~RTK_L2_UCAST_FLAG_SA_BLOCK) : pL2UcastData->flags|RTK_L2_UCAST_FLAG_SA_BLOCK;
    pL2UcastData->flags     = (fdbEntry.da_permit == 1)? pL2UcastData->flags&(~RTK_L2_UCAST_FLAG_DA_BLOCK) : pL2UcastData->flags|RTK_L2_UCAST_FLAG_DA_BLOCK;
    pL2UcastData->port      = fdbEntry.port_id;
    pL2UcastData->age       = fdbEntry.aging_sts;
    
    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_nextValidAddrOnPort_get */

/* Function Name:
 *      dal_rtl9607f_l2_nextValidEntry_get
 * Description:
 *      Get LUT next valid entry.
 * Input:
 *      pScanIdx - Index field in the structure.
 * Output:
 *      pL2Entry - entry content
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_L2_EMPTY_ENTRY   - Empty LUT entry.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This API is used to get next valid LUT entry.
 */
int32
dal_rtl9607f_l2_nextValidEntry_get(
        int32                   *pScanIdx,
        rtk_l2_addr_table_t     *pL2Entry)
{
    int32 ret;
    aal_fdb_entry_cfg_t  fdbEntry;
   
    if((ret = __dal_rtl9607f_nextValidAddr_Get(0, 0, pScanIdx, &fdbEntry))!=RT_ERR_OK)
    {
        if(ret == RT_ERR_L2_ENTRY_NOTFOUND)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_ENTRY_NOTFOUND;
        }
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    if(fdbEntry.mc_flag)
    {
        pL2Entry->entryType = RTK_LUT_L2MC;
        pL2Entry->method = LUT_READ_METHOD_NEXT_ADDRESS;
        memcpy(pL2Entry->entry.l2McEntry.mac.octet,fdbEntry.mac,sizeof(ca_mac_addr_t));
        pL2Entry->entry.l2McEntry.vid = fdbEntry.lrnt_vid;
        pL2Entry->entry.l2McEntry.priority = fdbEntry.dot1p;

    }
    else
    {
        pL2Entry->entryType = RTK_LUT_L2UC;
        pL2Entry->method = LUT_READ_METHOD_NEXT_ADDRESS;

        memcpy(pL2Entry->entry.l2UcEntry.mac.octet,fdbEntry.mac,sizeof(ca_mac_addr_t));
        pL2Entry->entry.l2UcEntry.vid = fdbEntry.lrnt_vid;
        pL2Entry->entry.l2UcEntry.priority = fdbEntry.dot1p;
        pL2Entry->entry.l2UcEntry.flags = (fdbEntry.static_flag == 1)? pL2Entry->entry.l2UcEntry.flags|RTK_L2_UCAST_FLAG_STATIC : pL2Entry->entry.l2UcEntry.flags&(~RTK_L2_UCAST_FLAG_STATIC);
        pL2Entry->entry.l2UcEntry.flags = (fdbEntry.sa_permit == 1)? pL2Entry->entry.l2UcEntry.flags&(~RTK_L2_UCAST_FLAG_SA_BLOCK) : pL2Entry->entry.l2UcEntry.flags|RTK_L2_UCAST_FLAG_SA_BLOCK;
        pL2Entry->entry.l2UcEntry.flags = (fdbEntry.da_permit == 1)? pL2Entry->entry.l2UcEntry.flags&(~RTK_L2_UCAST_FLAG_DA_BLOCK) : pL2Entry->entry.l2UcEntry.flags|RTK_L2_UCAST_FLAG_DA_BLOCK;
        pL2Entry->entry.l2UcEntry.port = fdbEntry.port_id;
        pL2Entry->entry.l2UcEntry.age = fdbEntry.aging_sts;

    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_nextValidEntry_get */

/* Function Name:
 *      dal_rtl9607f_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
dal_rtl9607f_l2_addr_add(rtk_l2_ucastAddr_t *pL2Addr)
{
    ca_status_t ret=CA_E_OK;
    ca_uint32_t time=0;
    aal_fdb_entry_cfg_t  fdbEntry;
    ca_uint32_t addr=0;

    ca_port_id_t port_id;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(pL2Addr->port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(((pL2Addr->mac.octet[0] & 0x01) != 0), RT_ERR_MAC);
    RT_PARAM_CHK((pL2Addr->flags > RTK_L2_UCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK((pL2Addr->age > HAL_L2_ENTRY_AGING_MAX()), RT_ERR_INPUT);
    

    memset(&fdbEntry, 0, sizeof(aal_fdb_entry_cfg_t));
    memcpy(fdbEntry.mac,pL2Addr->mac.octet,sizeof(ca_mac_addr_t));

    fdbEntry.key_vid        = pL2Addr->vid;
    fdbEntry.lrnt_vid       = pL2Addr->vid;
    fdbEntry.dot1p          = pL2Addr->priority;
    fdbEntry.static_flag    = (pL2Addr->flags&RTK_L2_UCAST_FLAG_STATIC)? 1:0;
    fdbEntry.sa_permit      = (pL2Addr->flags&RTK_L2_UCAST_FLAG_SA_BLOCK)? 0:1;
    fdbEntry.da_permit      = (pL2Addr->flags&RTK_L2_UCAST_FLAG_DA_BLOCK)? 0:1;
    fdbEntry.port_id        = port_id;

    //when setting dynamic lut, aging time can't be zero
    if(pL2Addr->age==0 && !(pL2Addr->flags&RTK_L2_UCAST_FLAG_STATIC))
    {
        if((ret = aal_fdb_aging_time_get(0,&time))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
        fdbEntry.aging_sts = HAL_L2_ENTRY_AGING_MAX();//Set MAX value
    }
    else
        fdbEntry.aging_sts = pL2Addr->age;

    if((ret = aal_fdb_entry_add(0,&fdbEntry))!=CA_E_OK)
    {
        if(CA_E_FULL == ret)
            return RT_ERR_ENTRY_FULL;

        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    /*Get FDB table address*/
    if(CA_E_OK == aal_fdb_entry_offset_get(0, fdbEntry.mac, 
        pL2Addr->vid, pL2Addr->priority, &addr))
    {
        pL2Addr->index = addr;
    }


    return RT_ERR_OK;

} /* end of dal_rtl9607f_l2_addr_add */

/* Function Name:
 *      dal_rtl9607f_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry.
 * Input:
 *      pL2Addr  - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
dal_rtl9607f_l2_addr_del(rtk_l2_ucastAddr_t *pL2Addr)
{
    ca_status_t ret=CA_E_OK;
    ca_mac_addr_t mac;
    ca_vlan_id_t key_vid;
    aal_fdb_entry_cfg_t  fdb_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->flags > RTK_L2_UCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK(((pL2Addr->mac.octet[0] & 0x01) != 0), RT_ERR_MAC);

    memcpy(mac,pL2Addr->mac.octet,sizeof(ca_mac_addr_t));
    key_vid=pL2Addr->vid;
    memset(&fdb_entry,0,sizeof(aal_fdb_entry_cfg_t));
    fdb_entry.key_sc_ind = 1;/*only for stag, the value if rleated with TPID value*/

    if((ret = aal_fdb_entry_get(0,mac,key_vid,&fdb_entry))!=CA_E_OK)
    {
        if(ret==CA_E_NOT_FOUND)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_ENTRY_NOTFOUND;
        }
        else
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
    }

    if((ret = aal_fdb_entry_del(0, &fdb_entry))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_addr_del */


/* Function Name:
 *      dal_rtl9607f_l2_addr_get
 * Description:
 *      Get L2 entry based on specified vid and MAC address
 * Input:
 *      None
 * Output:
 *      pL2Addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the unicast mac address existed in LUT, it will return the port and fid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
int32
dal_rtl9607f_l2_addr_get(rtk_l2_ucastAddr_t *pL2Addr)
{
    ca_status_t ret=CA_E_OK;
    ca_mac_addr_t mac;
    ca_vlan_id_t key_vid;
    aal_fdb_entry_cfg_t  fdb_entry;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->flags > RTK_L2_UCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK(((pL2Addr->mac.octet[0] & 0x01) != 0), RT_ERR_MAC);
    RT_PARAM_CHK((pL2Addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    memcpy(mac,pL2Addr->mac.octet,sizeof(ca_mac_addr_t));
    key_vid=pL2Addr->vid;
    memset(&fdb_entry, 0, sizeof(aal_fdb_entry_cfg_t));

    if((ret = aal_fdb_entry_get(0, mac, key_vid, &fdb_entry))!=CA_E_OK)
    {
        if(ret==CA_E_NOT_FOUND)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_ENTRY_NOTFOUND;
        }
        else
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
    }

    memset(pL2Addr,0,sizeof(rtk_l2_ucastAddr_t));

    memcpy(pL2Addr->mac.octet,fdb_entry.mac,sizeof(ca_mac_addr_t));
    pL2Addr->vid = fdb_entry.lrnt_vid;
    pL2Addr->priority = fdb_entry.dot1p;
    pL2Addr->flags = (fdb_entry.static_flag == 1)? pL2Addr->flags|RTK_L2_UCAST_FLAG_STATIC : pL2Addr->flags&(~RTK_L2_UCAST_FLAG_STATIC);
    pL2Addr->flags = (fdb_entry.sa_permit == 1)? pL2Addr->flags&(~RTK_L2_UCAST_FLAG_SA_BLOCK) : pL2Addr->flags|RTK_L2_UCAST_FLAG_SA_BLOCK;
    pL2Addr->flags = (fdb_entry.da_permit == 1)? pL2Addr->flags&(~RTK_L2_UCAST_FLAG_DA_BLOCK) : pL2Addr->flags|RTK_L2_UCAST_FLAG_DA_BLOCK;
    pL2Addr->port = PORT_ID(fdb_entry.port_id);
    pL2Addr->age = fdb_entry.aging_sts;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_addr_get */


/* Function Name:
 *      dal_rtl9607f_l2_addr_delAll
 * Description:
 *      Delete all L2 unicast address entry.
 * Input:
 *      includeStatic - include static mac or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9607f_l2_addr_delAll(uint32 includeStatic)
{
    ca_status_t ret=CA_E_OK;
    ca_uint32_t flag;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((TRUE < includeStatic), RT_ERR_INPUT);

    if(includeStatic==1)
    {
        flag=CA_L2_ADDR_OPERATION_BOTH;
    }
    else
    {
        flag=CA_L2_ADDR_OPERATION_DYNAMIC;
    }

    if((ret = aal_fdb_addr_delete_all(0,flag))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_addr_delAll */

/* Module Name    : L2        */
/* Sub-module Name: Port move */
/* Function Name:
 *      dal_rtl9607f_l2_illegalPortMoveAction_get
 * Description:
 *      Get forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
dal_rtl9607f_l2_illegalPortMoveAction_get(
    rtk_port_t          port,
    rtk_action_t        *pFwdAction)
{
    ca_status_t ret;
    ca_port_id_t port_id;
    aal_ilpb_cfg_t ilpb_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);


    if((ret = aal_port_ilpb_cfg_get(0,port, &ilpb_cfg))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }


    if(ilpb_cfg.sa_move_dis_fwd == 1)
    {
        *pFwdAction = ACTION_FORWARD;
    }
    else
    {
        *pFwdAction = ACTION_DROP;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_illegalPortMoveAction_get */

/* Function Name:
 *      dal_rtl9607f_l2_illegalPortMoveAction_set
 * Description:
 *      Set forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
dal_rtl9607f_l2_illegalPortMoveAction_set(
    rtk_port_t          port,
    rtk_action_t        fwdAction)
{
    ca_status_t ret;
    ca_port_id_t port_id;
    aal_ilpb_cfg_msk_t ilpb_msk;
    aal_ilpb_cfg_t ilpb_cfg;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ACTION_END <= fwdAction), RT_ERR_INPUT);

    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    memset(&ilpb_msk, 0, sizeof(aal_ilpb_cfg_msk_t));

    ilpb_msk.s.sa_move_dis_fwd = 1;
    ilpb_msk.s.station_move_en = 1;

    switch (fwdAction)
    {
        case ACTION_FORWARD:
            ilpb_cfg.station_move_en = 1;
            ilpb_cfg.sa_move_dis_fwd = 1;
            break;
        case ACTION_DROP:
            ilpb_cfg.station_move_en = 0;
            ilpb_cfg.sa_move_dis_fwd = 0;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if((ret = aal_port_ilpb_cfg_set(0,port, ilpb_msk, &ilpb_cfg))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_illegalPortMoveAction_set */


/* Function Name:
 *      dal_rtl9607f_l2_newMacOp_get
 * Description:
 *      Get forwarding action of new learned address on specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action( ACTION_FORWARD/ACTION_DROP)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */


int32
dal_rtl9607f_l2_newMacOp_get(
        rtk_port_t              port,
        rtk_l2_newMacLrnMode_t  *pLrnMode,
        rtk_action_t            *pFwdAction)

{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    aal_fdb_ctrl_t       fdb_ctrl;
    aal_ilpb_cfg_t ilpb_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);


    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);


    if((ret = aal_port_ilpb_cfg_get(0,port, &ilpb_cfg))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }


    if(ilpb_cfg.sa_miss_fw == 1)
    {
        *pFwdAction = ACTION_FORWARD;
    }
    else
    {
        *pFwdAction = ACTION_DROP;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_newMacOp_get */ 



/* Function Name:
 *      dal_rtl9607f_l2_newMacOp_set
 * Description:
 *      Set forwarding action of new learned address on specified port.
 * Input:
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 */
int32
dal_rtl9607f_l2_newMacOp_set(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction)
{
    ca_status_t ret;
    ca_port_id_t port_id;
    aal_ilpb_cfg_msk_t ilpb_msk;
    aal_ilpb_cfg_t ilpb_cfg;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ACTION_FORWARD != fwdAction && ACTION_DROP != fwdAction), RT_ERR_INPUT);

    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    memset(&ilpb_msk, 0, sizeof(aal_ilpb_cfg_msk_t));

    ilpb_msk.s.sa_miss_fw = 1;

    switch (fwdAction)
    {
        case ACTION_FORWARD:
            ilpb_cfg.sa_miss_fw = 1;
            break;
        case ACTION_DROP:
            ilpb_cfg.sa_miss_fw = 0;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if((ret = aal_port_ilpb_cfg_set(0,port, ilpb_msk, &ilpb_cfg))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_l2_newMacOp_set */        
