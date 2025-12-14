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
#include <dal/luna_g3/dal_luna_g3.h>
#include <dal/luna_g3/dal_luna_g3_l2.h>
#include <osal/time.h>

#include <ca_event.h>
#include <event_core.h>

#include <cortina-api/include/l2.h>
#include <cortina-api/include/port.h>
#include <cortina-api/include/eth_port.h>
#include <aal/include/aal_arb.h>
#include <aal/include/aal_port.h>
#include <aal/include/aal_fdb.h>

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
  
ca_uint32_t ca_rtk_link_down_l2_flush_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void *event,ca_uint32_t len,ca_void *cookie)
{
    ca_status_t ret=CA_E_OK;
    ca_event_eth_port_link_t* eth_port_link_ptr = (ca_event_eth_port_link_t *)event;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    if(eth_port_link_ptr->status == CA_PORT_LINK_DOWN)
    {
        if((ret = ca_l2_addr_delete_by_port(0,eth_port_link_ptr->port_id,CA_L2_ADDR_OP_BOTH))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return CA_EVT_ERROR;
        }
    }

    return CA_EVT_OK;
}

int32
dal_luna_g3_l2_init(void)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    l2_init = INIT_COMPLETED;

    if((ret = dal_luna_g3_l2_aging_set(RTK_L2_DEFAULT_AGING_TIME)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_init */


/* Function Name:
 *      dal_luna_g3_l2_flushLinkDownPortAddrEnable_get
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
dal_luna_g3_l2_flushLinkDownPortAddrEnable_get(rtk_enable_t *pEnable)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    *pEnable = LUNA_G3_L2_FLUSH_INTR;

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_flushLinkDownPortAddrEnable_get */


/* Function Name:
 *      dal_luna_g3_l2_flushLinkDownPortAddrEnable_set
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
dal_luna_g3_l2_flushLinkDownPortAddrEnable_set(rtk_enable_t enable)
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
} /* end of dal_luna_g3_l2_flushLinkDownPortAddrEnable_set */

/* Function Name:
 *      dal_luna_g3_l2_portLimitLearningCnt_get
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
dal_luna_g3_l2_portLimitLearningCnt_get(rtk_port_t port, uint32 *pMacCnt)
{
    ca_status_t ret=CA_E_OK;
    ca_l2_mac_limit_type_t type;
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

    type = CA_L2_MAC_LIMIT_PORT;

    if((ret = ca_l2_mac_limit_get(0,type,port_id,&number))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    *pMacCnt = number;

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_portLimitLearningCnt_get */


/* Function Name:
 *      dal_luna_g3_l2_portLimitLearningCnt_set
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
dal_luna_g3_l2_portLimitLearningCnt_set(rtk_port_t port, uint32 macCnt)
{
    ca_status_t ret=CA_E_OK;
    ca_l2_mac_limit_type_t type;
    ca_port_id_t port_id;
    ca_uint32_t number;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L2_LEARN_LIMIT_CNT_MAX() < macCnt), RT_ERR_INPUT);

    type = CA_L2_MAC_LIMIT_PORT;

    number = macCnt;

    if((ret = ca_l2_mac_limit_set(0,type,port_id,number))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_portLimitLearningCnt_set */

/* Function Name:
 *      dal_luna_g3_l2_portLimitLearningCntAction_get
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
dal_luna_g3_l2_portLimitLearningCntAction_get(rtk_port_t port, rtk_l2_limitLearnCntAction_t *pLearningAction)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_l2_learning_control_t control;
    aal_arb_redir_config_mask_t arb_redir_mask;
    aal_arb_redir_config_t arb_rdir_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLearningAction), RT_ERR_NULL_POINTER);

    if((ret = ca_l2_learning_control_get(0,port_id,&control))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    if(control.sa_mac_table_full_policy == CA_L2_MAC_FULL_FWD_NO_LEARN)
    {
        *pLearningAction = LIMIT_LEARN_CNT_ACTION_FORWARD;
    }
    else if(control.sa_mac_table_full_policy == CA_L2_MAC_FULL_DROP)
    {
        memset(&arb_rdir_cfg,0,sizeof(aal_arb_redir_config_t));
        memset(&arb_redir_mask,0,sizeof(aal_arb_redir_config_mask_t));

        if((ret = aal_arb_redir_drop_src_config_get(0, 14,&arb_rdir_cfg))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }

        if(arb_rdir_cfg.en == 0)
        {
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_DROP;
        }
        else
        {
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_TO_CPU;
        }
    }
    else
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_portLimitLearningCntAction_get */


/* Function Name:
 *      dal_luna_g3_l2_portLimitLearningCntAction_set
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
dal_luna_g3_l2_portLimitLearningCntAction_set(rtk_port_t port, rtk_l2_limitLearnCntAction_t learningAction)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_l2_learning_control_t control;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((LIMIT_LEARN_CNT_ACTION_END <= learningAction), RT_ERR_INPUT);

    if((ret = ca_l2_learning_control_get(0,port_id,&control))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    if(learningAction == LIMIT_LEARN_CNT_ACTION_FORWARD)
    {
        control.sa_mac_table_full_policy = CA_L2_MAC_FULL_FWD_NO_LEARN;
    }
    else if(learningAction == LIMIT_LEARN_CNT_ACTION_DROP)
    {
        control.sa_mac_table_full_policy = CA_L2_MAC_FULL_DROP;
    }
    else if(learningAction == LIMIT_LEARN_CNT_ACTION_TO_CPU)
    {
        aal_arb_redir_config_mask_t arb_redir_mask;
        aal_arb_redir_config_t arb_rdir_cfg;

        memset(&arb_rdir_cfg,0,sizeof(aal_arb_redir_config_t));
        memset(&arb_redir_mask,0,sizeof(aal_arb_redir_config_mask_t));
        arb_rdir_cfg.en = 1;
        arb_rdir_cfg.ldpid = AAL_LPORT_CPU_0;
        arb_redir_mask.bf.en = 1;
        arb_redir_mask.bf.ldpid = 1;
        // sa_mis_or_sa_limit_drp_14
        if((ret = aal_arb_redir_drop_src_config_set(0, 14, arb_redir_mask, &arb_rdir_cfg))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
        control.sa_mac_table_full_policy = CA_L2_MAC_FULL_DROP;
    }
    else
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_l2_learning_control_set(0,port_id,&control))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_portLimitLearningCntAction_set */

/* Function Name:
 *      da_luna_g3_l2_aging_get
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
dal_luna_g3_l2_aging_get(uint32 *pAgingTime)
{
    ca_status_t ret=CA_E_OK;
    ca_uint32_t time=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAgingTime), RT_ERR_NULL_POINTER);

    if((ret = ca_l2_aging_time_get(0,&time))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    *pAgingTime=time;

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_aging_get */


/* Function Name:
 *      dal_luna_g3_l2_aging_set
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
dal_luna_g3_l2_aging_set(uint32 agingTime)
{
    ca_status_t ret=CA_E_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L2_AGING_TIME_MAX() < agingTime), RT_ERR_INPUT);

    if((ret = ca_l2_aging_time_set(0,agingTime))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_aging_set */

/* Function Name:
 *      dal_luna_g3_l2_portAgingEnable_get
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
dal_luna_g3_l2_portAgingEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    ca_status_t ret=CA_E_OK;
    ca_l2_learning_mode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = ca_l2_learning_mode_get(0,&mode))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    if(mode == CA_L2_LEARNING_HARDWARE)
        *pEnable = ENABLED;
    else if(mode == CA_L2_LEARNING_SOFTWARE)
        *pEnable = DISABLED;
    else
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_portAgingEnable_get */

/* Function Name:
 *      dal_luna_g3_l2_portAgingEnable_set
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
dal_luna_g3_l2_portAgingEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    ca_status_t ret=CA_E_OK;
    ca_l2_learning_mode_t mode;
    lrn_fwd_ctrl_t fwd_ctrl;
    lrn_fwd_ctrl_mask_t fwd_ctrl_mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(enable == DISABLED)
    {
        mode = CA_L2_LEARNING_SOFTWARE;

        memset(&fwd_ctrl_mask,0,sizeof(lrn_fwd_ctrl_mask_t));

        fwd_ctrl.swlrn_othr_pkt_fwd = 1;
        fwd_ctrl_mask.bf.swlrn_othr_pkt_fwd = 1;

        if((ret = aal_fdb_lrn_fwd_ctrl_set(0,fwd_ctrl_mask,&fwd_ctrl))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
    }
    else
    {
        mode = CA_L2_LEARNING_HARDWARE;

        memset(&fwd_ctrl_mask,0,sizeof(lrn_fwd_ctrl_mask_t));

        fwd_ctrl.swlrn_othr_pkt_fwd = 0;
        fwd_ctrl_mask.bf.swlrn_othr_pkt_fwd = 1;

        if((ret = aal_fdb_lrn_fwd_ctrl_set(0,fwd_ctrl_mask,&fwd_ctrl))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
    }

    if((ret = ca_l2_learning_mode_set(0,mode))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_portAgingEnable_set */

/* Module Name    : L2      */
/* Sub-module Name: Parameter for lookup miss */
/* Function Name:
 *      dal_luna_g3_l2_lookupMissAction_get
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
dal_luna_g3_l2_lookupMissAction_get(rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if((ret = dal_luna_g3_l2_portLookupMissAction_get(0, type, pAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_lookupMissAction_get */

/* Function Name:
 *      dal_luna_g3_l2_lookupMissAction_set
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
dal_luna_g3_l2_lookupMissAction_set(rtk_l2_lookupMissType_t type, rtk_action_t action)
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
        if ((ret = dal_luna_g3_l2_portLookupMissAction_set(port, type, action)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_lookupMissAction_set */

/* Function Name:
 *      dal_luna_g3_l2_portLookupMissAction_get
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
dal_luna_g3_l2_portLookupMissAction_get(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
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

    switch(cfg.dft_fwd_deny)
    {
        case 0:
            if(cfg.dft_fwd_redir_en == 1 && cfg.dft_fwd_mc_group_id_valid == 1)
            {
                *pAction = ACTION_TRAP2CPU;
            }
            else
            {
                *pAction = ACTION_FORWARD;
            }
            break;
        case 1:
            *pAction = ACTION_DROP;
            break;
        default:
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

} /* end of dal_luna_g3_l2_portLookupMissAction_get */

/* Function Name:
 *      dal_luna_g3_l2_portLookupMissAction_set
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
dal_luna_g3_l2_portLookupMissAction_set(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action)
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
            cfg.dft_fwd_deny = 1;
            mask.bf.dft_fwd_deny = 1;
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

} /* end of dal_luna_g3_l2_portLookupMissAction_set */

/* Function Name:
 *      dal_luna_g3_l2_nextValidAddr_get
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
dal_luna_g3_l2_nextValidAddr_get(
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData)
{
    ca_status_t ret=CA_E_OK;
    ca_iterator_t ca_entry;
    ca_l2_addr_entry_t addr;

    ca_entry.entry_count = 1;
    ca_entry.entry_data = &addr;
    ca_entry.user_buffer_size = sizeof(ca_l2_addr_entry_t);
    ca_entry.prev_handle = *pScanIdx;

    while(1)
    {
        if((ret = ca_l2_addr_iterate(0,&ca_entry))!=CA_E_OK)
        {
            if(ret == CA_E_UNAVAIL)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_L2_ENTRY_NOTFOUND;
            }

            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
        else
            break;

        if(addr.mc_group_id == CA_UINT32_INVALID) /*unicast address only*/
            continue;
    }

    memcpy(pL2UcastData->mac.octet,addr.mac_addr,sizeof(ca_mac_addr_t));
    pL2UcastData->vid = addr.vid;
    pL2UcastData->priority = addr.dot1p;
    pL2UcastData->flags = (addr.static_flag == 1)? pL2UcastData->flags|RTK_L2_UCAST_FLAG_STATIC : pL2UcastData->flags&(~RTK_L2_UCAST_FLAG_STATIC);
    pL2UcastData->flags = (addr.sa_permit == 1)? pL2UcastData->flags&(~RTK_L2_UCAST_FLAG_SA_BLOCK) : pL2UcastData->flags|RTK_L2_UCAST_FLAG_SA_BLOCK;
    pL2UcastData->flags = (addr.da_permit == 1)? pL2UcastData->flags&(~RTK_L2_UCAST_FLAG_DA_BLOCK) : pL2UcastData->flags|RTK_L2_UCAST_FLAG_DA_BLOCK;
    pL2UcastData->port = PORT_ID(addr.port_id);
    pL2UcastData->age = addr.aging_timer;
    *pScanIdx= ((uint32)ca_entry.prev_handle )-1;//hit entry

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_luna_g3_l2_nextValidEntry_get
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
dal_luna_g3_l2_nextValidEntry_get(
        int32                   *pScanIdx,
        rtk_l2_addr_table_t     *pL2Entry)
{
    ca_status_t ret=CA_E_OK;
    ca_iterator_t ca_entry;
    ca_l2_addr_entry_t addr;

    ca_entry.entry_count = 1;
    ca_entry.entry_data = &addr;
    ca_entry.user_buffer_size = sizeof(ca_l2_addr_entry_t);
    ca_entry.prev_handle = *pScanIdx;
	
    while(1)
    {
        if((ret = ca_l2_addr_iterate(0,&ca_entry))!=CA_E_OK)
        {
            if(ret == CA_E_UNAVAIL)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_L2_ENTRY_NOTFOUND;
            }
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
        else
            break;
    }


    if(addr.port_id != CA_UINT32_INVALID && addr.mc_group_id == CA_UINT32_INVALID)
    {
        pL2Entry->entryType = RTK_LUT_L2UC;
        pL2Entry->method = LUT_READ_METHOD_NEXT_ADDRESS;

        memcpy(pL2Entry->entry.l2UcEntry.mac.octet,addr.mac_addr,sizeof(ca_mac_addr_t));
        pL2Entry->entry.l2UcEntry.vid = addr.vid;
        pL2Entry->entry.l2UcEntry.priority = addr.dot1p;
        pL2Entry->entry.l2UcEntry.flags = (addr.static_flag == 1)? pL2Entry->entry.l2UcEntry.flags|RTK_L2_UCAST_FLAG_STATIC : pL2Entry->entry.l2UcEntry.flags&(~RTK_L2_UCAST_FLAG_STATIC);
        pL2Entry->entry.l2UcEntry.flags = (addr.sa_permit == 1)? pL2Entry->entry.l2UcEntry.flags&(~RTK_L2_UCAST_FLAG_SA_BLOCK) : pL2Entry->entry.l2UcEntry.flags|RTK_L2_UCAST_FLAG_SA_BLOCK;
        pL2Entry->entry.l2UcEntry.flags = (addr.da_permit == 1)? pL2Entry->entry.l2UcEntry.flags&(~RTK_L2_UCAST_FLAG_DA_BLOCK) : pL2Entry->entry.l2UcEntry.flags|RTK_L2_UCAST_FLAG_DA_BLOCK;
        pL2Entry->entry.l2UcEntry.port = PORT_ID(addr.port_id);
        pL2Entry->entry.l2UcEntry.age = addr.aging_timer;
    }
    else if(addr.port_id == CA_UINT32_INVALID && addr.mc_group_id != CA_UINT32_INVALID)
    {
        pL2Entry->entryType = RTK_LUT_L2MC;
        pL2Entry->method = LUT_READ_METHOD_NEXT_ADDRESS;
        memcpy(pL2Entry->entry.l2McEntry.mac.octet,addr.mac_addr,sizeof(ca_mac_addr_t));
        pL2Entry->entry.l2McEntry.vid = addr.vid;
        pL2Entry->entry.l2McEntry.priority = addr.dot1p;
        /*TBD for port mask*/
        //addr.mc_group_id
    }

    *pScanIdx= ((uint32)ca_entry.prev_handle )-1;//hit entry

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_nextValidEntry_get */

/* Function Name:
 *      dal_luna_g3_l2_addr_add
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
dal_luna_g3_l2_addr_add(rtk_l2_ucastAddr_t *pL2Addr)
{
    ca_status_t ret=CA_E_OK;
    ca_uint32_t time=0;
    ca_l2_addr_entry_t addr;
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
    
    memcpy(addr.mac_addr,pL2Addr->mac.octet,sizeof(ca_mac_addr_t));
    addr.fwd_vid = pL2Addr->vid;
    addr.vid = pL2Addr->vid;
    addr.dot1p = pL2Addr->priority;
    addr.static_flag = (pL2Addr->flags&RTK_L2_UCAST_FLAG_STATIC)? 1:0;
    addr.sa_permit = (pL2Addr->flags&RTK_L2_UCAST_FLAG_SA_BLOCK)? 0:1;
    addr.da_permit = (pL2Addr->flags&RTK_L2_UCAST_FLAG_DA_BLOCK)? 0:1;
    addr.port_id = port_id;

    addr.mc_group_id = CA_UINT32_INVALID;
    
    //when setting dynamic lut, aging time can't be zero
    if(pL2Addr->age==0 && !(pL2Addr->flags&RTK_L2_UCAST_FLAG_STATIC))
    {
        if((ret = ca_l2_aging_time_get(0,&time))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
        addr.aging_timer = time;
    }
    else
        addr.aging_timer = pL2Addr->age;

    if((ret = ca_l2_addr_add(0,&addr))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_addr_add */

/* Function Name:
 *      dal_luna_g3_l2_addr_del
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
dal_luna_g3_l2_addr_del(rtk_l2_ucastAddr_t *pL2Addr)
{
    ca_status_t ret=CA_E_OK;
    ca_mac_addr_t mac;
    ca_vlan_id_t key_vid;
    ca_l2_addr_entry_t addr;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->flags > RTK_L2_UCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK(((pL2Addr->mac.octet[0] & 0x01) != 0), RT_ERR_MAC);

    memcpy(mac,pL2Addr->mac.octet,sizeof(ca_mac_addr_t));
    key_vid=pL2Addr->vid;
    memset(&addr,0,sizeof(ca_l2_addr_entry_t));

    if((ret = ca_l2_addr_get(0,mac,key_vid,&addr))!=CA_E_OK)
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

    if((ret = ca_l2_addr_delete(0,&addr))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_addr_del */


/* Function Name:
 *      dal_luna_g3_l2_addr_get
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
dal_luna_g3_l2_addr_get(rtk_l2_ucastAddr_t *pL2Addr)
{
    ca_status_t ret=CA_E_OK;
    ca_mac_addr_t mac;
    ca_vlan_id_t key_vid;
    ca_l2_addr_entry_t addr;

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
    memset(&addr,0,sizeof(ca_l2_addr_entry_t));

    if((ret = ca_l2_addr_get(0,mac,key_vid,&addr))!=CA_E_OK)
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

    memcpy(pL2Addr->mac.octet,addr.mac_addr,sizeof(ca_mac_addr_t));
    pL2Addr->vid = addr.vid;
    pL2Addr->priority = addr.dot1p;
    pL2Addr->flags = (addr.static_flag == 1)? pL2Addr->flags|RTK_L2_UCAST_FLAG_STATIC : pL2Addr->flags&(~RTK_L2_UCAST_FLAG_STATIC);
    pL2Addr->flags = (addr.sa_permit == 1)? pL2Addr->flags&(~RTK_L2_UCAST_FLAG_SA_BLOCK) : pL2Addr->flags|RTK_L2_UCAST_FLAG_SA_BLOCK;
    pL2Addr->flags = (addr.da_permit == 1)? pL2Addr->flags&(~RTK_L2_UCAST_FLAG_DA_BLOCK) : pL2Addr->flags|RTK_L2_UCAST_FLAG_DA_BLOCK;
    pL2Addr->port = PORT_ID(addr.port_id);
    pL2Addr->age = addr.aging_timer;

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_addr_get */


/* Function Name:
 *      dal_luna_g3_l2_addr_delAll
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
dal_luna_g3_l2_addr_delAll(uint32 includeStatic)
{
    ca_status_t ret=CA_E_OK;
    ca_l2_addr_op_flags_t flag;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((TRUE < includeStatic), RT_ERR_INPUT);

    if(includeStatic==1)
    {
        flag=CA_L2_ADDR_OP_BOTH;
    }
    else
    {
        flag=CA_L2_ADDR_OP_DYNAMIC;
    }

    if((ret = ca_l2_addr_delete_all(0,flag))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_addr_delAll */

/* Module Name    : L2        */
/* Sub-module Name: Port move */
/* Function Name:
 *      dal_luna_g3_l2_illegalPortMoveAction_get
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
dal_luna_g3_l2_illegalPortMoveAction_get(
    rtk_port_t          port,
    rtk_action_t        *pFwdAction)
{
    ca_status_t ret;
    ca_port_id_t port_id;
    ca_boolean_t enable;
    aal_arb_redir_config_mask_t arb_redir_mask;
    aal_arb_redir_config_t arb_rdir_cfg;
    ca_boolean_t en_12,en_14;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    if((ret = ca_port_station_move_get(0,port_id,&enable))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    if(enable == 1)
    {
        *pFwdAction = ACTION_FORWARD;
    }
    else
    {
        memset(&arb_rdir_cfg,0,sizeof(aal_arb_redir_config_t));
        memset(&arb_redir_mask,0,sizeof(aal_arb_redir_config_mask_t));

        if((ret = aal_arb_redir_drop_src_config_get(0, 12,&arb_rdir_cfg))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }

        en_12 = arb_rdir_cfg.en;

        memset(&arb_rdir_cfg,0,sizeof(aal_arb_redir_config_t));
        memset(&arb_redir_mask,0,sizeof(aal_arb_redir_config_mask_t));

        if((ret = aal_arb_redir_drop_src_config_get(0, 14,&arb_rdir_cfg))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }

        en_14 = arb_rdir_cfg.en;

        if(en_12 == 1 && en_14 ==1)
        {
            *pFwdAction = ACTION_TRAP2CPU;
        }
        else
        {
            *pFwdAction = ACTION_DROP;
        }
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_illegalPortMoveAction_get */

/* Function Name:
 *      dal_luna_g3_l2_illegalPortMoveAction_set
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
dal_luna_g3_l2_illegalPortMoveAction_set(
    rtk_port_t          port,
    rtk_action_t        fwdAction)
{
    ca_status_t ret;
    ca_port_id_t port_id;
    ca_boolean_t enable;
    aal_arb_redir_config_mask_t arb_redir_mask;
    aal_arb_redir_config_t arb_rdir_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ACTION_END <= fwdAction), RT_ERR_INPUT);

    memset(&arb_rdir_cfg,0,sizeof(aal_arb_redir_config_t));
    memset(&arb_redir_mask,0,sizeof(aal_arb_redir_config_mask_t));

    switch (fwdAction)
    {
        case ACTION_FORWARD:
            enable = 1;
            break;
        case ACTION_DROP:
            enable = 0;
            arb_rdir_cfg.en = 0;
            arb_redir_mask.bf.en = 1;
            break;
        case ACTION_TRAP2CPU:
            enable = 0;
            arb_rdir_cfg.en = 1;
            arb_rdir_cfg.ldpid = AAL_LPORT_CPU_0;
            arb_redir_mask.bf.en = 1;
            arb_redir_mask.bf.ldpid = 1;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if((ret = ca_port_station_move_set(0,port_id,enable))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    if(enable == 0)
    {
        // sw_learn_err_drp_12
        if((ret = aal_arb_redir_drop_src_config_set(0, 12, arb_redir_mask, &arb_rdir_cfg))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
        // sa_mis_or_sa_limit_drp_14
        if((ret = aal_arb_redir_drop_src_config_set(0, 14, arb_redir_mask, &arb_rdir_cfg))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_l2_illegalPortMoveAction_set */