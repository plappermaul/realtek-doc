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
#include <rtk/qos.h>

#include <dal/rtl9602c/dal_rtl9602c.h>
#include <dal/rtl9602c/dal_rtl9602c_qos.h>
/*
 * Symbol Definition
 */
typedef enum rtl9602c_queue_type_e
{
    RTL9602C_QOS_QUEUE_STRICT = 0,
    RTL9602C_QOS_QUEUE_WFQ,
    RTL9602C_QOS_TYPE_END
} rtl9602c_queue_type_t;


/*
 * Data Declaration
 */
static uint32    qos_init = INIT_NOT_COMPLETED;
/*
 * Function Declaration
 */



static int32 _dal_rtl9602c_qos_queueType_set(rtk_port_t port, uint32 qid, rtl9602c_queue_type_t type)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTL9602C_QOS_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() < qid), RT_ERR_QUEUE_ID);

    if ((ret = reg_array_field_write(RTL9602C_WFQ_TYPE_PORT_CFGr, port, qid, RTL9602C_QUEUE_TYPEf, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



static int32 _dal_rtl9602c_qos_queueType_get(rtk_port_t port, uint32 qid, rtl9602c_queue_type_t *pType)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() < qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_WFQ_TYPE_PORT_CFGr, port, qid, RTL9602C_QUEUE_TYPEf, pType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32 _dal_rtl9602c_qos_wfqWeight_set(rtk_port_t port, uint32 qid, uint32 weight)
{
    int ret;
    uint32 reg;
    uint32 field;
    uint32 phyQid;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() < qid), RT_ERR_QUEUE_ID);

    if(0 == qid)
    {
        RT_PARAM_CHK((65535 < weight), RT_ERR_QOS_QUEUE_WEIGHT);
        reg    = RTL9602C_WFQ_PORT_CFG0r;
        field  = RTL9602C_WEIGHT0f;
        phyQid = REG_ARRAY_INDEX_NONE;
    }
    else
    {
        RT_PARAM_CHK((1024 < weight), RT_ERR_QOS_QUEUE_WEIGHT);
        reg    = RTL9602C_WFQ_PORT_CFG1_7r;
        field  = RTL9602C_WEIGHT1_7f;
        phyQid = qid;
    }

    if ((ret = reg_array_field_write(reg, port, phyQid, field, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}


static int32 _dal_rtl9602c_qos_wfqWeight_get(rtk_port_t port, uint32 qid, uint32 *pWeight)
{
    int ret;
    uint32 reg;
    uint32 field;
    uint32 phyQid;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() < qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pWeight), RT_ERR_NULL_POINTER);

    if(0 == qid)
    {
        reg    = RTL9602C_WFQ_PORT_CFG0r;
        field  = RTL9602C_WEIGHT0f;
        phyQid = REG_ARRAY_INDEX_NONE;
    }
    else
    {
        reg    = RTL9602C_WFQ_PORT_CFG1_7r;
        field  = RTL9602C_WEIGHT1_7f;
        phyQid = qid;
    }
    if ((ret = reg_array_field_read(reg, port, phyQid, field, pWeight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }



    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9602c_qos_init
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
dal_rtl9602c_qos_init(void)
{
    int32   ret;
    rtk_qos_priSelWeight_t  priSelWeight;
    rtk_port_t  port, max_port;
    uint32  priority, dscpVal;
    uint32  burstsize;
    uint32  debugQosVal;


    rtk_pri_t   dot1pPriRemap[RTK_DOT1P_PRIORITY_MAX + 1] = {RTK_DEFAULT_QOS_1P_PRIORITY0_REMAP, RTK_DEFAULT_QOS_1P_PRIORITY1_REMAP,
                                                   RTK_DEFAULT_QOS_1P_PRIORITY2_REMAP, RTK_DEFAULT_QOS_1P_PRIORITY3_REMAP,
                                                   RTK_DEFAULT_QOS_1P_PRIORITY4_REMAP, RTK_DEFAULT_QOS_1P_PRIORITY5_REMAP,
                                                   RTK_DEFAULT_QOS_1P_PRIORITY6_REMAP, RTK_DEFAULT_QOS_1P_PRIORITY7_REMAP};
    rtk_qos_queue_weights_t queueWeight ={{0,0,0,0,0,0,0,0}};
    rtk_qos_pri2queue_t queueMap0={{0,1,2,3,4,5,6,7}};
    rtk_qos_pri2queue_t queueMap1={{0,0,0,0,0,0,0,0}};

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    burstsize = 0x3FFF;
    if ((ret = reg_field_write(RTL9602C_WFQ_CTRLr, RTL9602C_WFQ_BURSTSIZEf, &burstsize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    qos_init = INIT_COMPLETED;

    /*set priority decision parameters*/
    priSelWeight.weight_of_portBased = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_PORT;
    priSelWeight.weight_of_dot1q     = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_DOT1P;
    priSelWeight.weight_of_dscp      = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_DSCP;
    priSelWeight.weight_of_acl       = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_ACL;
    priSelWeight.weight_of_lutFwd    = 0;
    priSelWeight.weight_of_saBaed    = 0;
    priSelWeight.weight_of_vlanBased = 0;
    priSelWeight.weight_of_svlanBased= RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_SVLAN;
    priSelWeight.weight_of_l4Based   = 0;

    if ((ret = dal_rtl9602c_qos_priSelGroup_set(0, &priSelWeight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        qos_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_rtl9602c_qos_priSelGroup_set(1, &priSelWeight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        qos_init = INIT_NOT_COMPLETED;
        return ret;
    }


    /*set 1p priority remapping group 0*/
    for (priority = 0; priority <= RTK_DOT1P_PRIORITY_MAX; priority++)
    {
        if ((ret = dal_rtl9602c_qos_1pPriRemapGroup_set(0,priority,dot1pPriRemap[priority],0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            qos_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }
    /*set dscp priority remapping group 0*/
    /*dscp value all map internal priority 0*/
    for (dscpVal = 0; dscpVal <= RTK_VALUE_OF_DSCP_MAX; dscpVal++)
    {
        if ((ret = dal_rtl9602c_qos_dscpPriRemapGroup_set(0,dscpVal,0,0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            qos_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    /*init priority to queue group 0 and 1
      group 0 for CPU port
      group 1 for other ports
    */
    if ((ret = dal_rtl9602c_qos_priMap_set(0, &queueMap0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        qos_init = INIT_NOT_COMPLETED;
        return ret;
    }
    if ((ret = dal_rtl9602c_qos_priMap_set(1, &queueMap1)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        qos_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /*disable port remark ability*/
    max_port = HAL_GET_MAX_PORT();
    for (port = 0; port <= max_port; port++)
    {
        if (!HAL_IS_PORT_EXIST(port))
        {
            continue;
        }

        /*set port based priority*/
        if ((ret = dal_rtl9602c_qos_portPri_set(port, RTK_DEFAULT_QOS_PORT_PRIORITY)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        /*disable DSCP remarking*/
        if ((ret = dal_rtl9602c_qos_dscpRemarkEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            qos_init = INIT_NOT_COMPLETED;
            return ret;
        }

        /*disable 1p remarking*/
        if ((ret = dal_rtl9602c_qos_1pRemarkEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            qos_init = INIT_NOT_COMPLETED;
            return ret;
        }

        /*set scheduling parameters all use strict priority*/
        if(HAL_GET_PON_PORT() != port)
        {/*skip pon port*/
            if ((ret = dal_rtl9602c_qos_schedulingQueue_set(port, &queueWeight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                qos_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
        /*set internal priority to queue mapping
          cpu port map to group 0
          other port map to group 1
        */
        if(HAL_IS_CPU_PORT(port))
        {
            if ((ret = dal_rtl9602c_qos_portPriMap_set(port, 0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                qos_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
        else
        {
            if ((ret = dal_rtl9602c_qos_portPriMap_set(port, 1)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                qos_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
        /*set port priority select group to group 0*/
        if ((ret = dal_rtl9602c_qos_portPriSelGroup_set(port, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            qos_init = INIT_NOT_COMPLETED;
            return ret;
        }

    }

#if 1
    /* disable auto-detection when CIR exceed */
    debugQosVal = 0xffffffff;
    if((ret = reg_write(RTL9602C_MOCIR_FRC_MDr, &debugQosVal))!=RT_ERR_OK)
    {
       RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            qos_init = INIT_NOT_COMPLETED;
            return ret;
    }
    debugQosVal = 0xffffffff;
    if((ret = reg_write(RTL9602C_MOCIR_FRC_VALr, &debugQosVal))!=RT_ERR_OK)
    {
       RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            qos_init = INIT_NOT_COMPLETED;
            return ret;
    }
#endif
    qos_init = INIT_COMPLETED;
    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_init */


/* Function Name:
 *      dal_rtl9602c_qos_priSelGroup_get
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
dal_rtl9602c_qos_priSelGroup_get(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    int32   ret;
    uint32 weight;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9602C_PRI_SEL_GROUP_INDEX_MAX <= grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_SVLAN_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    pWeightOfPriSel->weight_of_svlanBased = weight;

    if ((ret = reg_array_field_read(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_ACL_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    pWeightOfPriSel->weight_of_acl = weight;

    if ((ret = reg_array_field_read(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_DSCP_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    pWeightOfPriSel->weight_of_dscp = weight;

    if ((ret = reg_array_field_read(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_DOT1Q_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    pWeightOfPriSel->weight_of_dot1q = weight;

    if ((ret = reg_array_field_read(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_PORT_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    pWeightOfPriSel->weight_of_portBased = weight;

    if ((ret = reg_array_field_read(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_L4_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    pWeightOfPriSel->weight_of_l4Based = weight;

	pWeightOfPriSel->weight_of_saBaed = 0;
    pWeightOfPriSel->weight_of_lutFwd = 0;
    pWeightOfPriSel->weight_of_vlanBased = 0;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_priSelGroup_get */

/* Function Name:
 *      dal_rtl9602c_qos_priSelGroup_set
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
dal_rtl9602c_qos_priSelGroup_set(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    int32   ret;
    uint32 weight;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9602C_PRI_SEL_GROUP_INDEX_MAX <= grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);

    weight = pWeightOfPriSel->weight_of_svlanBased;
    if(weight > HAL_PRI_SEL_WEIGHT_MAX())
        return RT_ERR_INPUT;
    if ((ret = reg_array_field_write(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_SVLAN_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    weight = pWeightOfPriSel->weight_of_acl;
    if(weight > HAL_PRI_SEL_WEIGHT_MAX())
        return RT_ERR_INPUT;
    if ((ret = reg_array_field_write(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_ACL_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    weight = pWeightOfPriSel->weight_of_dscp;
    if(weight > HAL_PRI_SEL_WEIGHT_MAX())
        return RT_ERR_INPUT;
    if ((ret = reg_array_field_write(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_DSCP_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    weight = pWeightOfPriSel->weight_of_dot1q;
    if(weight > HAL_PRI_SEL_WEIGHT_MAX())
        return RT_ERR_INPUT;
    if ((ret = reg_array_field_write(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_DOT1Q_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    weight = pWeightOfPriSel->weight_of_portBased;
    if(weight > HAL_PRI_SEL_WEIGHT_MAX())
        return RT_ERR_INPUT;
    if ((ret = reg_array_field_write(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_PORT_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    weight = pWeightOfPriSel->weight_of_l4Based;
    if(weight > HAL_PRI_SEL_WEIGHT_MAX())
        return RT_ERR_INPUT;
    if ((ret = reg_array_field_write(RTL9602C_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grpIdx, RTL9602C_L4_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_priSelGroup_set */


/* Function Name:
 *      dal_rtl9602c_qos_portPri_get
 * Description:
 *      Get internal priority of one port.
 * Input:
 *      port     - port id
 * Output:
 *      pIntPri  - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    None
 */
int32
dal_rtl9602c_qos_portPri_get(rtk_port_t port, rtk_pri_t *pIntPri)
{
    int32   ret;
    uint32  intPri;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIntPri), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(RTL9602C_QOS_PB_PRIr, port, REG_ARRAY_INDEX_NONE, RTL9602C_INTPRI_PBf, &intPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    *pIntPri = intPri;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_portPri_get */


/* Function Name:
 *      dal_rtl9602c_qos_portPri_set
 * Description:
 *      Get internal priority of one port.
 * Input:
 *      port     - port id
 *      intPri  - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Output:
 *    None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    None
 */
int32
dal_rtl9602c_qos_portPri_set(rtk_port_t port, rtk_pri_t intPri)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri), RT_ERR_INPUT);


    if ((ret = reg_array_field_write(RTL9602C_QOS_PB_PRIr, port, REG_ARRAY_INDEX_NONE, RTL9602C_INTPRI_PBf, &intPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_portPri_set */


/* Function Name:
 *      dal_rtl9602c_qos_dscpPriRemapGroup_get
 * Description:
 *      Get remapped internal priority of DSCP on specified DSCP remapping group.
 * Input:
 *      unit     - unit id
 *      grpIdx   - index of dscp remapping group
 *      dscp     - DSCP
 * Output:
 *      pIntPri  - pointer to internal priority
 *      pDp      - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_QOS_DSCP_VALUE - invalid DSCP value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 *      Apollo only support group 0
 */
int32
dal_rtl9602c_qos_dscpPriRemapGroup_get(
    uint32      grpIdx,
    uint32      dscp,
    rtk_pri_t   *pIntPri,
    uint32      *pDp)
{
    int32   ret;
    uint32  intPri;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_REMAP_GROUP_IDX_MAX() < grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIntPri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDp), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(RTL9602C_QOS_DSCP_REMAPr, REG_ARRAY_INDEX_NONE, dscp, RTL9602C_INTPRI_DSCPf, &intPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    *pDp = 0;
    *pIntPri = intPri;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_dscpPriRemapGroup_get */

/* Function Name:
 *      dal_rtl9602c_qos_dscpPriRemapGroup_set
 * Description:
 *      Set remapped internal priority of DSCP on specified DSCP remapping group.
 * Input:
 *      grpIdx  - index of dscp remapping group
 *      dscp    - DSCP
 *      intPri - internal priority
 *      dp      - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE   - invalid DSCP value
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Apollo only support group 0
 */
int32
dal_rtl9602c_qos_dscpPriRemapGroup_set(
    uint32      grpIdx,
    uint32      dscp,
    rtk_pri_t   intPri,
    uint32      dp)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_REMAP_GROUP_IDX_MAX() < grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX  < dscp), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_DROP_PRECEDENCE_MAX() < dp), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(RTL9602C_QOS_DSCP_REMAPr, REG_ARRAY_INDEX_NONE, dscp, RTL9602C_INTPRI_DSCPf, &intPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_dscpPriRemapGroup_set */


/* Function Name:
 *      dal_rtl9602c_qos_1pPriRemapGroup_get
 * Description:
 *      Get remapped internal priority of dot1p priority on specified dot1p priority remapping group.
 * Input:
 *      unit      - unit id
 *      grpIdx   - index of outer dot1p remapping group
 *      dot1pPri - dot1p priority
 * Output:
 *      pIntPri  - pointer to internal priority
 *      pDp       - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - invalid dot1p priority
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      Apollo only support group 0
 */
int32
dal_rtl9602c_qos_1pPriRemapGroup_get(
    uint32      grpIdx,
    rtk_pri_t   dot1pPri,
    rtk_pri_t   *pIntPri,
    uint32      *pDp)
{
    int32   ret;
    uint32  intPri;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_REMAP_GROUP_IDX_MAX() < grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < dot1pPri), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIntPri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDp), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_QOS_1Q_PRI_REMAPr, REG_ARRAY_INDEX_NONE, dot1pPri, RTL9602C_INTPRI_1Qf, &intPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    *pDp = 0;
    *pIntPri = intPri;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_1pPriRemapGroup_get */

/* Function Name:
 *      dal_rtl9602c_qos_1pPriRemapGroup_set
 * Description:
 *      Set remapped internal priority of dot1p priority on specified dot1p priority remapping group.
 * Input:
 *      grpIdx   - index of dot1p remapping group
 *      dot1pPri - dot1p priority
 *      intPri   - internal priority
 *      dp        - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY  - invalid dot1p priority
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9602c_qos_1pPriRemapGroup_set(
    uint32      grpIdx,
    rtk_pri_t   dot1pPri,
    rtk_pri_t   intPri,
    uint32      dp)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_REMAP_GROUP_IDX_MAX() < grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < dot1pPri), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_DROP_PRECEDENCE_MAX() < dp), RT_ERR_DROP_PRECEDENCE);

    if ((ret = reg_array_field_write(RTL9602C_QOS_1Q_PRI_REMAPr, REG_ARRAY_INDEX_NONE, dot1pPri, RTL9602C_INTPRI_1Qf, &intPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_1pPriRemapGroup_set */


/* Function Name:
 *      dal_rtl9602c_qos_priMap_set
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
dal_rtl9602c_qos_priMap_set(uint32 group, rtk_qos_pri2queue_t *pPri2qid)
{
    int32   ret;
    int32   priority;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < group), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);


    for(priority = 0;priority <= HAL_INTERNAL_PRIORITY_MAX();priority++)
    {
        if(pPri2qid->pri2queue[priority] >= HAL_MAX_NUM_OF_QUEUE())
            return RT_ERR_INPUT;
    }

    for(priority = 0;priority <= HAL_INTERNAL_PRIORITY_MAX();priority++)
    {
        if ((ret = reg_array_field_write(RTL9602C_QOS_INTPRI_TO_QIDr, group, priority, RTL9602C_PRI_TO_QIDf, (uint32 *)&(pPri2qid->pri2queue[priority]))) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_priMap_set */


/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl9602c_qos_priMap_get
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
dal_rtl9602c_qos_priMap_get(uint32 group, rtk_qos_pri2queue_t *pPri2qid)
{
    int32   ret;
    int32   priority;
    uint32  qid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < group), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

    for(priority = 0;priority <= HAL_INTERNAL_PRIORITY_MAX();priority++)
    {
        if ((ret = reg_array_field_read(RTL9602C_QOS_INTPRI_TO_QIDr, group, priority, RTL9602C_PRI_TO_QIDf, &qid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
        pPri2qid->pri2queue[priority] = qid;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_priMap_get */


/* Function Name:
 *      dal_rtl9602c_qos_portPriMap_get
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
dal_rtl9602c_qos_portPriMap_get(rtk_port_t port, uint32 *pGroup)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(RTL9602C_QOS_PORT_QMAP_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_IDXf, pGroup)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

} /* end of dal_rtl9602c_qos_portPriMap_get */

/* Function Name:
 *      dal_rtl9602c_qos_portPriMap_set
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
dal_rtl9602c_qos_portPriMap_set(rtk_port_t port, uint32 group)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < group), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(RTL9602C_QOS_PORT_QMAP_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_IDXf, &group)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_portPriMap_set */

/* Module Name    : QoS           */
/* Sub-module Name: Egress remark */
/* Function Name:
 *      dal_rtl9602c_qos_1pRemarkEnable_get
 * Description:
 *      Get 802.1p remark status for a port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status of 802.1p remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9602c_qos_1pRemarkEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_RMK_DOT1Q_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_1pRemarkEnable_get */

/* Function Name:
 *      dal_rtl9602c_qos_1pRemarkEnable_set
 * Description:
 *      Set 802.1p remark status for a port
 * Input:
 *      port   - port id.
 *      enable - status of 802.1p remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9602c_qos_1pRemarkEnable_set(rtk_port_t port, rtk_enable_t enable)
{

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(RTL9602C_RMK_DOT1Q_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_1pRemarkEnable_set */




/* Function Name:
 *      dal_rtl9602c_qos_1pRemarkGroup_get
 * Description:
 *      Get remarked dot1p priority of internal priority on specified dot1p remark group.
 * Input:
 *      grpIdx    - index of dot1p remark group
 *      intPri    - internal priority
 *      dp         - drop precedence
 * Output:
 *      pDot1pPri - pointer to dot1p priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_qos_1pRemarkGroup_get(
    uint32      grpIdx,
    rtk_pri_t   intPri,
    uint32      dp,
    rtk_pri_t   *pDot1pPri)
{
    int32   ret;
    uint32  do1pPri;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_REMAP_GROUP_IDX_MAX() < grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((HAL_DROP_PRECEDENCE_MAX() < dp), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK((NULL == pDot1pPri), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_RMK_1Q_CTRLr, REG_ARRAY_INDEX_NONE, intPri, RTL9602C_INTPRI_1Qf, &do1pPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    *pDot1pPri = (rtk_pri_t)do1pPri;
    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_1pRemarkGroup_get */

/* Function Name:
 *      dal_rtl9602c_qos_1pRemarkGroup_set
 * Description:
 *      Set remarked dot1p priority of internal priority on specified dot1p remark group.
 * Input:
 *      grpIdx   - index of dot1p remark group
 *      intPri   - internal priority
 *      dp       - drop precedence
 *      dot1pPri - dot1p priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY  - invalid dot1p priority
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9602c_qos_1pRemarkGroup_set(
    uint32 grpIdx,
    rtk_pri_t intPri,
    uint32 dp,
    rtk_pri_t dot1pPri)
{

    int32   ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_REMAP_GROUP_IDX_MAX() < grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((HAL_DROP_PRECEDENCE_MAX() < dp), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK((RTK_MAX_NUM_OF_PRIORITY <= dot1pPri), RT_ERR_QOS_1P_PRIORITY);

    if ((ret = reg_array_field_write(RTL9602C_RMK_1Q_CTRLr, REG_ARRAY_INDEX_NONE, intPri, RTL9602C_INTPRI_1Qf, &dot1pPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_1pRemarkGroup_set */

/* Function Name:
 *      dal_rtl9602c_qos_dscpRemarkEnable_get
 * Description:
 *      Get DSCP remark status for a port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status of DSCP remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9602c_qos_dscpRemarkEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_RMK_DSCP_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_dscpRemarkEnable_get */

/* Function Name:
 *      dal_rtl9602c_qos_dscpRemarkEnable_set
 * Description:
 *      Set DSCP remark status for a port
 * Input:
 *      port   - port id
 *      enable - status of DSCP remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 * Note:
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9602c_qos_dscpRemarkEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(RTL9602C_RMK_DSCP_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_dscpRemarkEnable_set */


/* Function Name:
 *      dal_rtl9602c_qos_dscpRemarkGroup_get
 * Description:
 *      Get remarked DSCP of internal priority on specified dscp remark group.
 * Input:
 *      grpIdx - index of dot1p remapping group
 *      intPri - internal priority
 *      dp      - drop precedence
 * Output:
 *      pDscp   - pointer to DSCP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      (1) The valid range of grp_idx is 0 for apollo
 */
int32
dal_rtl9602c_qos_dscpRemarkGroup_get(
    uint32      grpIdx,
    rtk_pri_t   intPri,
    uint32      dp,
    uint32      *pDscp)
{
    int32   ret;
    uint32  dscpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_REMARK_GROUP_IDX_MAX() < grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((HAL_DROP_PRECEDENCE_MAX() < dp), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_RMK_DSCP_INT_PRI_CTRLr, REG_ARRAY_INDEX_NONE, intPri, RTL9602C_INTPRI_DSCPf, &dscpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    *pDscp = dscpVal;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_dscpRemarkGroup_get */

/* Function Name:
 *      dal_rtl9602c_qos_dscpRemarkGroup_set
 * Description:
 *      Set remarked DSCP of internal priority on specified dscp remark group.
 * Input:
 *      grpIdx - index of dot1p remapping group
 *      intPri - internal priority
 *      dp      - drop precedence
 *      dscp    - DSCP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE   - invalid DSCP value
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      (1) The valid range of grp_idx is 0 for apollo
 */
int32
dal_rtl9602c_qos_dscpRemarkGroup_set(
    uint32      grpIdx,
    rtk_pri_t   intPri,
    uint32      dp,
    uint32      dscp)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PRIORITY_REMARK_GROUP_IDX_MAX() < grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((HAL_DROP_PRECEDENCE_MAX() < dp), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp), RT_ERR_QOS_DSCP_VALUE);

    if ((ret = reg_array_field_write(RTL9602C_RMK_DSCP_INT_PRI_CTRLr, REG_ARRAY_INDEX_NONE, intPri, RTL9602C_INTPRI_DSCPf, &dscp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_dscpRemarkGroup_set */


/* Function Name:
 *      dal_rtl9602c_qos_portDscpRemarkSrcSel_get
 * Description:
 *      Get remarking source of DSCP remarking.
 * Input:
 *      port                 - port id
 * Output:
 *      pType      - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer

 * Note:
 *      None
 */
int32
dal_rtl9602c_qos_portDscpRemarkSrcSel_get(rtk_port_t port, rtk_qos_dscpRmkSrc_t *pType)
{
    int32   ret;
    uint32 dscpSource;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);
    if ((ret = reg_array_field_read(RTL9602C_RMK_P_DSCP_SELr, port, REG_ARRAY_INDEX_NONE, RTL9602C_SELf, &dscpSource)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }


    if(dscpSource == 0)
        *pType = DSCP_RMK_SRC_INT_PRI;
    else if(dscpSource == 1)
        *pType = DSCP_RMK_SRC_DSCP;
    else
        return RT_ERR_FAILED;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_dscpRemarkSrcSel_get */

/* Function Name:
 *      dal_rtl9602c_qos_dscpRemarkSrcSel_set
 * Description:
 *      Set remarking source of DSCP remarking.
 * Input:
 *      port                 - port id
 *      type      - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      The API can configure DSCP remark functionality to map original DSCP value or internal
 *      priority to TX DSCP value.
 */
int32
dal_rtl9602c_qos_portDscpRemarkSrcSel_set(rtk_port_t port, rtk_qos_dscpRmkSrc_t type)
{
    int32   ret;
    uint32 dscpSource;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((DSCP_RMK_SRC_END <= type), RT_ERR_INPUT);


    if(type == DSCP_RMK_SRC_INT_PRI)
        dscpSource = 0;
    else if(type == DSCP_RMK_SRC_DSCP)
        dscpSource = 1;
    else
        return RT_ERR_INPUT;

    if ((ret = reg_array_field_write(RTL9602C_RMK_P_DSCP_SELr, port, REG_ARRAY_INDEX_NONE, RTL9602C_SELf, &dscpSource)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_portDscpRemarkSrcSel_set */


/* Function Name:
 *      dal_rtl9602c_qos_dscp2DscpRemarkGroup_get
 * Description:
 *      Get DSCP to remarked DSCP mapping.
 * Input:
 *      dscp    - DSCP value
 *      grpIdx  - group index
 * Output:
 *      pDscp   - remarked DSCP value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_rtl9602c_qos_dscp2DscpRemarkGroup_get(uint32 grpIdx, uint32 dscp, uint32 *pDscp)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((1 <= grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, dscp, RTL9602C_INTPRI_DSCPf, pDscp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_dscp2DscpRemark_get */

/* Function Name:
 *      dal_rtl9602c_qos_dscp2DscpRemarkGroup_set
 * Description:
 *      Set DSCP to remarked DSCP mapping.
 * Input:
 *      grpIdx  - group index
 *      dscp    - DSCP value
 *      rmkDscp - remarked DSCP value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 * Note:
 *      dscp parameter can be DSCP value or internal priority according to configuration of API
 *      dal_rtl9602c_qos_dscpRemarkSrcSel_set(), because DSCP remark functionality can map original DSCP
 *      value or internal priority to TX DSCP value.
 */
int32
dal_rtl9602c_qos_dscp2DscpRemarkGroup_set(uint32 grpIdx,uint32 dscp, uint32 rmkDscp)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((1 <= grpIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < rmkDscp), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(RTL9602C_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, dscp, RTL9602C_INTPRI_DSCPf, &rmkDscp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_dscp2DscpRemark_set */


/* Function Name:
 *      dal_rtl9602c_qos_fwd2CpuPriRemap_get
 * Description:
 *      Get forward to CPU port remapped priority for internal priority.
 * Input:
 *      intPri  -  internal priority
 * Output:
 *      pRempPri  - pointer to remapping priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 */
int32
dal_rtl9602c_qos_fwd2CpuPriRemap_get(rtk_pri_t   intPri,
                            rtk_pri_t   *pRempPri)
{
    int32   ret;
    uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((NULL == pRempPri), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_QOS_PRI_REMAP_IN_CPUr, REG_ARRAY_INDEX_NONE, intPri, RTL9602C_PRIf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    *pRempPri = tmpVal;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_fwd2CpuPriRemap_get */


/* Function Name:
 *      dal_rtl9602c_qos_fwd2CpuPriRemap_set
 * Description:
 *      Set remapped internal priority of DSCP on specified DSCP remapping group.
 * Input:
 *      intPri  -  internal priority
 *      rempPri - remapping priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 */
int32
dal_rtl9602c_qos_fwd2CpuPriRemap_set(rtk_pri_t intPri,rtk_pri_t rempPri)
{
    int32   ret;
    uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < rempPri), RT_ERR_INPUT);

    tmpVal = rempPri;
    if ((ret = reg_array_field_write(RTL9602C_QOS_PRI_REMAP_IN_CPUr, REG_ARRAY_INDEX_NONE, intPri, RTL9602C_PRIf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_fwd2CpuPriRemap_set */



/* Module Name    : QoS              */
/* Sub-module Name: Queue scheduling */

/* Function Name:
 *      dal_rtl9602c_qos_schedulingQueue_get
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
dal_rtl9602c_qos_schedulingQueue_get(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32    ret;
    uint32   queueId;
    uint32   weight;
    rtl9602c_queue_type_t type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    for(queueId = 0;queueId < HAL_MAX_NUM_OF_QUEUE();queueId++)
    {
        /*get queue type*/
        if ((ret = _dal_rtl9602c_qos_queueType_get(port, queueId, &type)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        if(type == RTL9602C_QOS_QUEUE_STRICT)
        {   /*strict priority set to 0*/
            pQweights->weights[queueId] = 0;
        }
        else
        {
            /*get WFQ weight*/
            if ((ret = _dal_rtl9602c_qos_wfqWeight_get(port, queueId, &weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            pQweights->weights[queueId] = weight;
        }
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_schedulingQueue_get */

/* Function Name:
 *      dal_rtl9602c_qos_schedulingQueue_set
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
dal_rtl9602c_qos_schedulingQueue_set(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32   ret;
    uint32   queueId;
    uint32   weight;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    for(queueId = 0;queueId < HAL_MAX_NUM_OF_QUEUE();queueId++)
    {
        weight = pQweights->weights[queueId];
        if(weight == 0)
        {
            /*set queue type*/
            if ((ret = _dal_rtl9602c_qos_queueType_set(port, queueId, RTL9602C_QOS_QUEUE_STRICT)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
        }
        else
        {
            weight = weight;
            /*set queue type*/
            if ((ret = _dal_rtl9602c_qos_queueType_set(port, queueId, RTL9602C_QOS_QUEUE_WFQ)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            /*set WFQ weightqueue*/
            if ((ret = _dal_rtl9602c_qos_wfqWeight_set(port, queueId, weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }

        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_qos_schedulingQueue_set */



/* Function Name:
 *      dal_rtl9602c_qos_portPriSelGroup_get
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
dal_rtl9602c_qos_portPriSelGroup_get(rtk_port_t port, uint32 *pPriSelGrpIdx)
{
    int32   ret;
    uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "port=%d",port);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPriSelGrpIdx), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_array_field_read(RTL9602C_PRI_SEL_TBL_CFGr, port, REG_ARRAY_INDEX_NONE, RTL9602C_SELf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    *pPriSelGrpIdx = tmpVal;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_qos_portPriSelGroup_get */

/* Function Name:
 *      dal_rtl9602c_qos_portPriSelGroup_set
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
dal_rtl9602c_qos_portPriSelGroup_set(rtk_port_t port, uint32 priSelGrpIdx)
{
    int32   ret;
    uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "port=%d,priSelGrpIdx=%d",port, priSelGrpIdx);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTL9602C_PRI_SEL_GROUP_INDEX_MAX <= priSelGrpIdx), RT_ERR_INPUT);

    /* function body */
    tmpVal = priSelGrpIdx;
    if ((ret = reg_array_field_write(RTL9602C_PRI_SEL_TBL_CFGr, port, REG_ARRAY_INDEX_NONE, RTL9602C_SELf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_qos_portPriSelGroup_set */


/* Function Name:
 *      dal_rtl9602c_qos_schedulingType_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      pQtype    - the WFQ schedule type (DAL_RTL9602C_QOS_WFQ or DAL_RTL9602C_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
dal_rtl9602c_qos_schedulingType_get(rtk_qos_scheduling_type_t *pQtype)
{
	int32  ret;
	uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "");

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQtype), RT_ERR_NULL_POINTER);


    /* function body */
	if ((ret = reg_field_read(RTL9602C_WFQ_CTRLr,RTL9602C_WFQ_MODEf, &val)) != RT_ERR_OK)
	{
	    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	    return ret;
	}

	if(0 == val)
        *pQtype=RTK_QOS_WFQ;
	else
        *pQtype=RTK_QOS_WRR;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_qos_schedulingType_get */

/* Function Name:
 *      dal_rtl9602c_qos_schedulingType_set
 * Description:
 *      Set the scheduling type.
 * Input:
 *      queueType - the WFQ schedule type (DAL_RTL9602C_QOS_WFQ or DAL_RTL9602C_QOS_WRR)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rtl9602c_qos_schedulingType_set(rtk_qos_scheduling_type_t queueType)
{
	int32  ret;
	uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "queueType=%d",queueType);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK((SCHEDULING_TYPE_END <=queueType), RT_ERR_INPUT);


    /* function body */
	if(RTK_QOS_WFQ == queueType)
	    val = 0;
	else
	    val = 1;

	if ((ret = reg_field_write(RTL9602C_WFQ_CTRLr,RTL9602C_WFQ_MODEf, &val)) != RT_ERR_OK)
	{
	    RT_ERR(ret, (MOD_QOS | MOD_DAL), "");
	    return ret;
	}

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_qos_schedulingType_set */


/* Function Name:
 *      dal_rtl9602c_qos_portDot1pRemarkSrcSel_get
 * Description:
 *      Get remarking source of dot1p remarking.
 * Input:
 *      port                 - port id
 * Output:
 *      pType      - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer

 * Note:
 *      None
 */
int32
dal_rtl9602c_qos_portDot1pRemarkSrcSel_get(rtk_port_t port, rtk_qos_do1pRmkSrc_t *pType)
{
    int32   ret;
    uint32  source;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "port=%d",port);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_array_field_read(RTL9602C_RMK_P_1P_SELr, port, REG_ARRAY_INDEX_NONE, RTL9602C_SELf, &source)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    if(source == 0)
        *pType = DOT1P_RMK_SRC_USER_PRI;
    else
        *pType = DOT1P_RMK_ORI_CTAG_PRI;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_qos_portDot1pRemarkSrcSel_get */

/* Function Name:
 *      dal_rtl9602c_qos_portDot1pRemarkSrcSel_set
 * Description:
 *      Set remarking source of dot1p remarking.
 * Input:
 *      port                 - port id
 *      type      - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      The API can configure dot1p remark source.
 */
int32
dal_rtl9602c_qos_portDot1pRemarkSrcSel_set(rtk_port_t port, rtk_qos_do1pRmkSrc_t type)
{
    int32   ret;
    uint32  source;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "port=%d,type=%d",port, type);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    /* function body */

    if(type == DOT1P_RMK_SRC_USER_PRI)
        source = 0;
    else if(type == DOT1P_RMK_ORI_CTAG_PRI)
        source = 1;
    else
        return RT_ERR_INPUT;

    if ((ret = reg_array_field_read(RTL9602C_RMK_P_1P_SELr, port, REG_ARRAY_INDEX_NONE, RTL9602C_SELf, &source)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9602c_qos_portDot1pRemarkSrcSel_set */
