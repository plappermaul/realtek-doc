/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 42383 $
 * $Date: 2013-08-28 14:10:44 +0800 (Wed, 28 Aug 2013) $
 *
 * Purpose : Definition of QoS API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Ingress Priority Decision
 *           (2) Egress Remarking
 *           (3) Queue Scheduling
 *           (4) Congestion avoidance
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/rtl8277c/dal_rtl8277c.h>
#include <dal/rtl8277c/dal_rtl8277c_qos.h>
#include <osal/time.h>
#include <hal/chipdef/rtl8277c/rtl8277c_def.h>
#include <aal_l2_classification.h>
#include <cortina-api/include/queue.h>
#include <cortina-api/include/port.h>
#include <cortina-api/include/qos.h>
#include <aal_l3_te.h>
#include <aal_l3_tm.h>
#include <aal_l2_tm.h>
#include <aal_l2_qos.h>
#include <aal_port.h>
#include <aal_pon.h>

/*
 * Data Declaration
 */
static uint32 qos_init = INIT_NOT_COMPLETED;

typedef struct queueWeightCfg_s
{
    uint32 weight_of_dot1q;
    uint32 weight_of_dscp;
} queueWeightCfg_t;

typedef struct queuePriMapCfg_s
{
    uint32 pri2queue[RTK_MAX_NUM_OF_QUEUE];
} queuePriMapCfg_t;

queueWeightCfg_t queueWeightCfg[RTL8277C_PRI_SEL_GROUP_INDEX_MAX];
queuePriMapCfg_t queuePriMapCfg[RTL8277C_PRI_SEL_GROUP_INDEX_MAX];

uint32 lockPort2Weight[RTK_MAX_NUM_OF_PORTS];
uint32 lockPort2PriMap[RTK_MAX_NUM_OF_PORTS];
rt_qos_ingress_qos_t igrQos[RTL8277C_QOS_INGRESS_QOS_RESERVED_NUM];
uint8 igrQosUsage[RTL8277C_QOS_INGRESS_QOS_RESERVED_NUM]={0};

/*
 * Function Declaration
 */

int32 _dal_rtl8277c_qos_weight_set(rtk_port_t port,uint32 w_dot1q,uint32 w_dscp)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    aal_ilpb_cfg_msk_t ilpb_cfg_mask;
    aal_ilpb_cfg_t ilpb_cfg;

    port_id = RTK2CA_PORT_ID(port);

    memset(&ilpb_cfg_mask, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));

    ilpb_cfg_mask.s.cos_mode_sel        = 1;


    if(w_dot1q >= w_dscp)
    {
        ilpb_cfg.cos_mode_sel = AAL_COS_MODE_MAP_RX_TOP_DOT1P;
    }
    else
    {
        ilpb_cfg.cos_mode_sel = AAL_COS_MODE_MAP_RX_DSCP;
    }

    ret = aal_port_ilpb_cfg_set(0, PORT_ID(port_id), ilpb_cfg_mask, &ilpb_cfg);
    if (CA_E_OK != ret)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_rtl8277c_qos_priMap_set(rtk_port_t port,uint32* pPri2queue)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_uint8_t qIdx;
    ca_uint8_t prio;
    ca_boolean_t egress=1;
    aal_l2_qos_txdot1p_map_mask_t txdot1p_map_mask;
    aal_l2_qos_txdot1p_map_t txdot1p_map;
    aal_l2_qos_dscp_map_mask_t map_mask;
    aal_l2_qos_dscp_map_t      map;
    
    port_id = RTK2CA_PORT_ID(port);

    for(qIdx=0;qIdx<RTK_MAX_NUM_OF_QUEUE;qIdx++)
    {
        prio = pPri2queue[qIdx];

        memset(&txdot1p_map, 0, sizeof(txdot1p_map));
        txdot1p_map_mask.u32                      = 0;
        txdot1p_map_mask.s.txdot1p_map_mapped_cos = 1;
        txdot1p_map.txdot1p_map_mapped_cos        = prio;

        aal_l2_qos_txdot1p_map_set(0, 
            PORT_ID(port_id) == AAL_LPORT_ETH_WAN ? AAL_L2_QOS_DIR_DOWNSTREAM : AAL_L2_QOS_DIR_UPSTREAM,
            qIdx, txdot1p_map_mask, &txdot1p_map);

        memset(&map, 0, sizeof(map));
        map_mask.u32 = 0;
        map_mask.s.dscp_map_mapped_cos = 1;

        map.dscp_map_mapped_cos = prio;

        ret = aal_l2_qos_dscp_map_set(0, 
            PORT_ID(port_id) == AAL_LPORT_ETH_WAN ? AAL_L2_QOS_DIR_DOWNSTREAM : AAL_L2_QOS_DIR_UPSTREAM,
            qIdx, map_mask, &map);
    }

    return RT_ERR_OK;
}

/* Module Name    : QoS              */
/* Sub-module Name: Queue scheduling */

/* Function Name:
 *      dal_rtl8277c_qos_init
 * Description:
 *      Configure QoS initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Note:
 *
 *      The initialization does the following actions:
 *      - set input bandwidth control parameters to default values
 *      - set priority decision parameters
 *      - set scheduling parameters
 *      - disable port remark ability
 *      - CPU port init 8 queue using prioroty to queue mapping index 0
 *      - Other port init 1 queue using prioroty to queue mapping index 1
 */
int32
dal_rtl8277c_qos_init(void)
{
    uint32 portIdx,grpIdx,qIdx;

    for(grpIdx=0;grpIdx<RTL8277C_PRI_SEL_GROUP_INDEX_MAX;grpIdx++)
    {
        queueWeightCfg[grpIdx].weight_of_dot1q = 3;
        queueWeightCfg[grpIdx].weight_of_dscp = 0;

        for(qIdx=0;qIdx<RTK_MAX_NUM_OF_QUEUE;qIdx++)
        {
            queuePriMapCfg[grpIdx].pri2queue[qIdx] = qIdx;
        }
    }

    for(portIdx=0;portIdx<RTK_MAX_NUM_OF_PORTS;portIdx++)
    {
        lockPort2Weight[portIdx] = 0;
        lockPort2PriMap[portIdx] = 0;
    }

    memset(igrQos, 0, sizeof(rt_qos_ingress_qos_t)*RTL8277C_QOS_INGRESS_QOS_RESERVED_NUM);
    memset(igrQosUsage, 0, sizeof(uint8)*RTL8277C_QOS_INGRESS_QOS_RESERVED_NUM);

    qos_init = INIT_COMPLETED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8277c_qos_priSelGroup_get
 * Description:
 *      Get weight of each priority assignment on specified priority selection group.
 * Input:
 *      grpIdx         - index of priority selection group
 * Output:
 *      pWeightOfPriSel - pointer to weight of each priority assignment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Apollo only support group 0
 */
int32
dal_rtl8277c_qos_priSelGroup_get(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() <= grpIdx), RT_ERR_INPUT); //Weight only support one group
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);

    memset(pWeightOfPriSel,0,sizeof(rtk_qos_priSelWeight_t));

    pWeightOfPriSel->weight_of_dot1q = queueWeightCfg[grpIdx].weight_of_dot1q;
    pWeightOfPriSel->weight_of_dscp = queueWeightCfg[grpIdx].weight_of_dscp;

    return RT_ERR_OK;
} /* end of dal_rtl8277c_qos_priSelGroup_get */

/* Function Name:
 *      dal_rtl8277c_qos_priSelGroup_set
 * Description:
 *      Set weight of each priority assignment on specified priority selection group.
 * Input:
 *      grpIdx          - index of priority selection group
 *      pWeightOfPriSel - weight of each priority assignment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Apollo only support group 0
 */
int32
dal_rtl8277c_qos_priSelGroup_set(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    int32   ret;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < grpIdx), RT_ERR_INPUT); //Weight only support one group
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);

    HAL_SCAN_ALL_PORT(port)
    {
        if(lockPort2Weight[port] == grpIdx)
        {
            if((ret=_dal_rtl8277c_qos_weight_set(port,pWeightOfPriSel->weight_of_dot1q,pWeightOfPriSel->weight_of_dscp)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS | MOD_DAL), "");
                return ret;
            }
        }
    }

    queueWeightCfg[grpIdx].weight_of_dot1q = pWeightOfPriSel->weight_of_dot1q;
    queueWeightCfg[grpIdx].weight_of_dscp = pWeightOfPriSel->weight_of_dscp;

    return RT_ERR_OK;
} /* end of dal_rtl8277c_qos_priSelGroup_set */

/* Function Name:
 *      dal_rtl8277c_qos_portPriSelGroup_get
 * Description:
 *      Get priority selection group for specified port.
 * Input:
 *      port            - port id
 * Output:
 *      pPriSelGrpIdx  - pointer to index of priority selection group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl8277c_qos_portPriSelGroup_get(rtk_port_t port, uint32 *pPriSelGrpIdx)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "port=%d",port);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPriSelGrpIdx), RT_ERR_NULL_POINTER);

    /* function body */
    *pPriSelGrpIdx = lockPort2Weight[port];

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_qos_portPriSelGroup_get */

/* Function Name:
 *      dal_rtl8277c_qos_portPriSelGroup_set
 * Description:
 *      Set priority selection group for specified port.
 * Input:
 *      port            - port id
 *      priSelGrpIdx   - index of priority selection group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 */
int32
dal_rtl8277c_qos_portPriSelGroup_set(rtk_port_t port, uint32 priSelGrpIdx)
{
    int32 ret=RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "port=%d,priSelGrpIdx=%d",port, priSelGrpIdx);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() <= priSelGrpIdx), RT_ERR_INPUT);

    /* function body */
    if((ret=_dal_rtl8277c_qos_weight_set(port,queueWeightCfg[priSelGrpIdx].weight_of_dot1q,queueWeightCfg[priSelGrpIdx].weight_of_dscp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS | MOD_DAL), "");
        return ret;
    }

    lockPort2Weight[port] = priSelGrpIdx;

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_qos_portPriSelGroup_set */

/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl8277c_qos_priMap_get
 * Description:
 *      Get the entry of internal priority to QID mapping table.
 * Input:
 *      group     - the group of priority to Queue id map(0~3).
 *      pPri2qid  - array of internal priority on a queue
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENTRY_INDEX  - Invalid group index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *     None
 */
int32
dal_rtl8277c_qos_priMap_get(uint32 group, rtk_qos_pri2queue_t *pPri2qid)
{
    uint32  qIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < group), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

    for(qIdx=0;qIdx<RTK_MAX_NUM_OF_QUEUE;qIdx++)
    {
        pPri2qid->pri2queue[qIdx] = queuePriMapCfg[group].pri2queue[qIdx];
    }

    return RT_ERR_OK;
} /* end of dal_rtl8277c_qos_priMap_get */

/* Function Name:
 *      dal_rtl8277c_qos_priMap_set
 * Description:
 *      Set the entry of internal priority to QID mapping table.
 * Input:
 *      group     - the group of priority to Queue id map(0~3).
 *      pPri2qid  - array of internal priority on a queue
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENTRY_INDEX  - Invalid group index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Below is an example of internal priority to QID mapping table.
 *      -
 *      -              Priority
 *      -  group        0   1   2   3   4   5   6   7
 *      -              ================================
 *      -        0      0   1   2   3   4   5   6   7
 *      -        1      0   0   0   0   0   0   0   0
 *      -        2      0   0   0   0   6   6   6   6
 *      -        3      0   0   0   1   1   2   2   3
 *      -for table index 0
 *      -    pPri2qid[0] = 0   internal priority 0 map to queue 0
 *      -    pPri2qid[1] = 1   internal priority 1 map to queue 1
 *      -    pPri2qid[2] = 2   internal priority 2 map to queue 2
 *      -    pPri2qid[3] = 3   internal priority 3 map to queue 3
 *      -    pPri2qid[4] = 4   internal priority 4 map to queue 4
 *      -    pPri2qid[5] = 5   internal priority 5 map to queue 5
 *      -    pPri2qid[6] = 6   internal priority 6 map to queue 6
 *      -    pPri2qid[7] = 7   internal priority 7 map to queue 7
 */
int32
dal_rtl8277c_qos_priMap_set(uint32 group, rtk_qos_pri2queue_t *pPri2qid)
{
    int32 ret=RT_ERR_OK;
    rtk_port_t port;
    uint32  qIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < group), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

    HAL_SCAN_ALL_PORT(port)
    {
        if(lockPort2PriMap[port] == group)
        {
            if((ret=_dal_rtl8277c_qos_priMap_set(port,pPri2qid->pri2queue)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS | MOD_DAL), "");
                return ret;
            }
        }
    }

    for(qIdx=0;qIdx<RTK_MAX_NUM_OF_QUEUE;qIdx++)
    {
        queuePriMapCfg[group].pri2queue[qIdx] = pPri2qid->pri2queue[qIdx];
    }

    return RT_ERR_OK;
} /* end of dal_rtl8277c_qos_priMap_set */

/* Function Name:
 *      dal_rtl8277c_qos_portPriMap_get
 * Description:
 *      Get the value of internal priority to QID mapping table on specified port.
 * Input:
 *      port   - port id
 * Output:
 *      pGroup - Priority to queue mapping group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl8277c_qos_portPriMap_get(rtk_port_t port, uint32 *pGroup)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);

    *pGroup = lockPort2PriMap[port];

    return RT_ERR_OK;

} /* end of dal_rtl8277c_qos_portPriMap_get */

/* Function Name:
 *      dal_rtl8277c_qos_portPriMap_set
 * Description:
 *      Set the value of internal priority to QID mapping table on specified port.
 * Input:
 *      port  - port id
 *      index - index to priority to queue table
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 * Note:
 *      None
 */
int32
dal_rtl8277c_qos_portPriMap_set(rtk_port_t port, uint32 group)
{
    int32 ret=RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < group), RT_ERR_INPUT);

    if((ret=_dal_rtl8277c_qos_priMap_set(port,queuePriMapCfg[group].pri2queue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS | MOD_DAL), "");
        return ret;
    }

    lockPort2PriMap[port] = group;

    return RT_ERR_OK;
} /* end of dal_rtl8277c_qos_portPriMap_set */

/* Function Name:
 *      dal_rtl8277c_qos_schedulingQueue_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      port      - port id
 * Output:
 *      pQweights - the array of weights for WRR/WFQ queue (valid:1~128, 0 for STRICT_PRIORITY queue)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *      If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 */
int32
dal_rtl8277c_qos_schedulingQueue_get(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    ca_status_t ret=CA_E_OK;
    ca_queue_schedule_mode_t mode;
    ca_queue_weights_t weights;
    uint32 queueId;
    ca_port_id_t port_id;
    int32 i;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

#if 0
    if(port == CA_PORT_ID_NI7)
    {
        port_id = CA_PORT_ID(CA_PORT_TYPE_L3,port);
    }

    if((ret = ca_queue_schedule_get(0,port_id,&mode,&weights)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return RT_ERR_FAILED;
    }
#else
    if(HAL_IS_CPU_PORT(port))
    {
        port -= CA_PORT_ID_CPU0;
        for (i=0; i<CA_QUEUE_COUNT; i++)
        {
            if (aal_l3_tm_es_weight_ratio_get(0, port, i, &(weights.weights[i])) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return RT_ERR_FAILED;
            }
        }
    }
    else
    {
        if (port == CA_PORT_ID_L3_WAN)
            port = CA_AAL_L2TM_DPID_L3_WAN;
        else if (port == CA_PORT_ID_L3_LAN)
            port = CA_AAL_L2TM_DPID_L3_LAN;

        /* l2 ES */
        for (i=0; i<CA_QUEUE_COUNT; i++)
        {
            if (aal_l2_tm_es_weight_ratio_get(0, port, i, &(weights.weights[i])) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return RT_ERR_FAILED;
            }
        }
    }
#endif
    for(queueId = 0;queueId < HAL_MAX_NUM_OF_QUEUE();queueId++)
    {
        if(queueId >= CA_QUEUE_COUNT)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return RT_ERR_QUEUE_NUM;
        }

        pQweights->weights[queueId] = weights.weights[queueId];
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8277c_qos_schedulingQueue_set
 * Description:
 *      Set the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      port      - port id
 *      pQweights - the array of weights for WRR/WFQ queue (valid:1~128, 0 for STRICT_PRIORITY queue)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - Invalid port id
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight
 * Note:
 *      The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *      If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 */
int32
dal_rtl8277c_qos_schedulingQueue_set(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    ca_status_t ret=CA_E_OK;
    ca_queue_schedule_mode_t mode;
    ca_queue_weights_t weights;
    uint32 queueId;
    ca_port_id_t port_id;
    int32 i, j;
    uint32 cur_pon_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    port_id = RTK2CA_PORT_ID(port);

//printk("%s %d port=%d weight=%d:%d:%d:%d:%d:%d:%d:%d\n"
//    ,__FUNCTION__,__LINE__,port
//    ,pQweights->weights[0]
//    ,pQweights->weights[1]
//    ,pQweights->weights[2]
//    ,pQweights->weights[3]
//    ,pQweights->weights[4]
//    ,pQweights->weights[5]
//    ,pQweights->weights[6]
//    ,pQweights->weights[7]);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    mode = CA_SCHEDULE_STRICT_PRIORITY;
    for(queueId = 0;queueId < HAL_MAX_NUM_OF_QUEUE();queueId++)
    {
        if(queueId >= CA_QUEUE_COUNT)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return RT_ERR_QUEUE_NUM;
        }
        weights.weights[queueId] = pQweights->weights[queueId];
        
        if(pQweights->weights[queueId] != 0)
            mode = CA_SCHEDULE_DEFICIT_WEIGHT_ROUND_ROBIN;
    }

    if(mode == CA_SCHEDULE_DEFICIT_WEIGHT_ROUND_ROBIN)
    {
        /*DWRR or DWRR + SP */
        for (i=0; i<CA_QUEUE_COUNT; i++)
            if (weights.weights[i] > 0xff) /* SP - 0, WRR range 1 ~ 255 */
                return RT_ERR_QOS_QUEUE_WEIGHT;

        /* SP always has higher priority than DRWW */
        for (i=0; i<CA_QUEUE_COUNT; i++)
        {
            if (weights.weights[i] == 0)   /* SP */
            {
                for (j=i+1; j<CA_QUEUE_COUNT; j++)
                {
                    if (weights.weights[j])    /* */
                        return RT_ERR_QOS_QUEUE_WEIGHT;
                }
            }
        }

    }

#if 0
    if(port == CA_PORT_ID_NI7)
    {
        port_id = CA_PORT_ID(CA_PORT_TYPE_L3,port);
    }

    if((ret = ca_queue_schedule_set(0,port_id,mode,&weights)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return RT_ERR_FAILED;
    }
#else
    if(HAL_IS_CPU_PORT(port))
    {
        port -= CA_PORT_ID_CPU0;     /* CPU entry 0 ~ 7 */
        for (i=0; i<CA_QUEUE_COUNT; i++)
        {
            if (aal_l3_tm_es_weight_ratio_set(0, port, i, weights.weights[i]) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return RT_ERR_FAILED;
            }
        }
    }
    else if(HAL_IS_PON_PORT(port))
    {
        cur_pon_mode = aal_pon_mac_mode_get(0);
        if(CA_PON_MODE_IS_ETHERNET(cur_pon_mode))
        {
            /* l2 ES */
            for (i=0; i<CA_QUEUE_COUNT; i++)
            {
                if (aal_l2_tm_es_weight_ratio_set(0, port, i, weights.weights[i]) != CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                    return RT_ERR_FAILED;
                }
            }
            /* l3 TE */
            for (i=0; i<CA_QUEUE_COUNT; i++)
            {
                if (aal_l3_tm_es_weight_ratio_set(0, port + CA_AAL_L3_TE_NI_PORT_OFFSET, i, weights.weights[i]) != CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                    return RT_ERR_FAILED;
                }
            }
        }
    }
    else
    {
        /* convert LDPID to TM LDPID */
        if (port == CA_PORT_ID_L3_WAN)
            port = CA_AAL_L2TM_DPID_L3_WAN;
        else if (port == CA_PORT_ID_L3_LAN)
            port = CA_AAL_L2TM_DPID_L3_LAN;

        /* l2 ES */
        for (i=0; i<CA_QUEUE_COUNT; i++)
        {
            if (aal_l2_tm_es_weight_ratio_set(0, port, i, weights.weights[i]) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return RT_ERR_FAILED;
            }
        }
    }
#endif
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8277c_qos_dscpPriRemapGroup_get
 * Description:
 *      Get remapped COS of DSCP in upstream direction
 * Input:
 *      grpIdx   - Not in use
 *      dscp     - DSCP
 * Output:
 *      pPri     - pointer to priority bit
 *      pDp      - Not in use
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - invalid DSCP value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 *      8277 serial only support us/ds remapping table
 */
int32
dal_rtl8277c_qos_dscpPriRemapGroup_get(uint32 grpIdx, uint32 dscp, rtk_pri_t *pPri, uint32 *pDp)
{
    int32   ret;
    uint32  intPri;
    aal_l2_qos_dscp_map_t dscpMap;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDp), RT_ERR_NULL_POINTER);

    memset(&dscpMap,0,sizeof(aal_l2_qos_dscp_map_t));
    if(aal_l2_qos_dscp_map_get(0, AAL_L2_QOS_DIR_UPSTREAM, dscp, &dscpMap) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return RT_ERR_FAILED;
    }
    *pDp = 0;
    *pPri = dscpMap.dscp_map_mapped_cos;

    return RT_ERR_OK;
} 

/* Function Name:
 *      dal_rtl8277c_qos_dscpPriRemapGroup_set
 * Description:
 *      Set remapped COS of DSCP in upstream direction
 * Input:
 *      grpIdx   - Not in use
 *      dscp     - DSCP
 * Output:
 *      pPri     - pointer to priority bit
 *      pDp      - Not in use
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - invalid DSCP value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 *      8277 serial only support us/ds remapping table
 */
int32
dal_rtl8277c_qos_dscpPriRemapGroup_set(uint32 grpIdx, uint32 dscp, rtk_pri_t pri, uint32 dp)
{
    int32   ret;
    aal_l2_qos_dscp_map_mask_t dscpMapMask;
    aal_l2_qos_dscp_map_t dscpMap;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX  < dscp), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pri), RT_ERR_INPUT);

    memset(&dscpMap,0,sizeof(aal_l2_qos_dscp_map_t));
    memset(&dscpMapMask,0,sizeof(aal_l2_qos_dscp_map_mask_t));
    dscpMapMask.s.dscp_map_mapped_cos = 1;
    dscpMap.dscp_map_mapped_cos = pri;
    if(aal_l2_qos_dscp_map_set(0, AAL_L2_QOS_DIR_UPSTREAM, dscp, dscpMapMask, &dscpMap) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} 

/* Function Name:
 *      dal_rtl8277c_qos_ingress_qos_set
 * Description:
 *      Set the ingress qos setting for HGU mode only
 * Input:
 *      index   - index of qos setting 0~15
 *      pIgrQos - Ingress qos setting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      
 *      
 */
int32
dal_rtl8277c_qos_ingress_qos_set(uint8 index, rt_qos_ingress_qos_t *pIgrQos)
{
    aal_l2_cf_rule_t ruleCfg;
    int32   ret;

    if(pIgrQos == NULL)
        return RT_ERR_NULL_POINTER;

    if(index >= RTL8277C_QOS_INGRESS_QOS_RESERVED_NUM)
        return RT_ERR_FAILED;

    memset(&ruleCfg,0,sizeof(aal_l2_cf_rule_t));
    ruleCfg.key.care_bits.valid = 1;
    ruleCfg.key.data_bits.valid = 1;
    ruleCfg.key.care_bits.direction = 1;
    ruleCfg.key.data_bits.direction = AAL_L2_CF_PATTEN_DIRECTION_DOWNSTREAM;
    if(pIgrQos->filterMask & RT_QOS_INGRESS_QOS_FILTER_FLOW_ID)
    {
        ruleCfg.key.data_bits.pol_id = pIgrQos->flowId;
        ruleCfg.key.care_bits.pol_id = 0x1ff;
    }
    if(pIgrQos->filterMask & RT_QOS_INGRESS_QOS_FILTER_VID)
    {
        ruleCfg.key.data_bits.outer_vid = pIgrQos->vid;
        ruleCfg.key.care_bits.outer_vid = 0xfff;
    }
    ruleCfg.action.cf_cos_act = AAL_L2_CF_ACTION_COS_ACT_ASSIGN;
    ruleCfg.action.cf_cos = pIgrQos->priority;
    ruleCfg.action.fwd_act = AAL_L2_CF_ACTION_FWD_ACT_LDPID;
    ruleCfg.action.cf_fwd_id = AAL_LPORT_L3_WAN;
    if(aal_l2_cf_rule_add(0, index, &ruleCfg) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return RT_ERR_FAILED;
    }

    igrQosUsage[index] = 1;
    memcpy(&igrQos[index], pIgrQos, sizeof(rt_qos_ingress_qos_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8277c_qos_ingress_qos_del
 * Description:
 *      Delete the ingress qos setting
 * Input:
 *      index   - index of qos setting 0~15
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED         - No such qos setting
 * Note:
 *      
 *      
 */
int32
dal_rtl8277c_qos_ingress_qos_del(uint8 index)
{
    if(index >= RTL8277C_QOS_INGRESS_QOS_RESERVED_NUM)
        return RT_ERR_FAILED;

    if(aal_l2_cf_rule_delete(0, index) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return RT_ERR_FAILED;
    }

    igrQosUsage[index] = 0;
    memset(&igrQos[index], 0, sizeof(rt_qos_ingress_qos_t));
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8277c_qos_ingress_qos_get
 * Description:
 *      Get the ingress qos setting
 * Input:
 *      index   - index of qos setting 0~15
 * Output:
 *      pIgrQos - Ingress qos setting
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED         - No such qos setting
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      
 *      
 */
int32
dal_rtl8277c_qos_ingress_qos_get(uint8 index, rt_qos_ingress_qos_t *pIgrQos)
{
    if(pIgrQos == NULL)
        return RT_ERR_NULL_POINTER;

    if(index >= RTL8277C_QOS_INGRESS_QOS_RESERVED_NUM || igrQosUsage[index] == 0)
        return RT_ERR_FAILED;

    memcpy(pIgrQos, &igrQos[index], sizeof(rt_qos_ingress_qos_t));
    return RT_ERR_OK;
}
