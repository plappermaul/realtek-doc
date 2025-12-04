/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * $Revision$
 * $Date$
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Ingress priority decision
 *           2) Egress remark
 *           3) Queue scheduling
 */

/*
 * Include Files
 */

#include <dal/apollomp/raw/apollomp_raw_qos.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */



/*
 * Function Declaration
 */

/* Function Name:
 *      apollomp_raw_qos_priToQidMappingTable_set
 * Description:
 *      Set priority to QID mapping table parameters
 * Input:
 *      index 		- Mapping table index
 *      pri     	- The priority value
 *      qid 		- Queue id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_QUEUE_ID
 *  	RT_ERR_PRIORITY
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_priToQidMappingTable_set(uint32 index, uint32 pri, uint32 qid)
{
    int ret;

    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pri ), RT_ERR_PRIORITY);

    if ((ret = reg_array_field_write(APOLLOMP_QOS_INTPRI_TO_QIDr, index, pri, APOLLOMP_PRI_TO_QIDf, &qid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_priToQidMappingTable_get
 * Description:
 *      Set priority to QID mapping table parameters
 * Input:
 *      index 		- Mapping table index
 *      pri     	- The priority value
 * Output:
 *      pQid 		- Queue id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_QUEUE_ID
 *  	RT_ERR_NULL_POINTER
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_priToQidMappingTable_get(uint32 index, uint32 pri, uint32 *pQid)
{
    int ret;

    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pQid), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pri ), RT_ERR_PRIORITY);

    if ((ret = reg_array_field_read(APOLLOMP_QOS_INTPRI_TO_QIDr, index, pri, APOLLOMP_PRI_TO_QIDf, pQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_portQidMapIdx_set
 * Description:
 *      Set output queue mapping table index for each port
 * Input:
 *      port 	- Physical port number (0~6)
 *      index 	- Mapping table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_PORT_ID
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_portQidMapIdx_set(rtk_port_t port, uint32 index)
{
    int ret;

    RT_PARAM_CHK((HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() < index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_write(APOLLOMP_QOS_PORT_QMAP_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IDXf, &index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_portQidMapIdx_get
 * Description:
 *      Get output queue mapping table index for each port
 * Input:
 *      port 	- Physical port number (0~6)
 * Output:
 *      index 	- Mapping table index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_ID
 *  	RT_ERR_NULL_POINTER
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_portQidMapIdx_get(rtk_port_t port, uint32 *pIndex)
{
    int ret;

    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(APOLLOMP_QOS_PORT_QMAP_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IDXf, pIndex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_priRemapToCpu_set
 * Description:
 *      Set remapping priority for packet forward to CPU port
 * Input:
 *      pri 	    - priority
 *      remapPri	- remapping priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *  	RT_ERR_PRIORITY
 * Note:
 *      None
 */
int32 apollomp_raw_qos_priRemapToCpu_set(uint32 pri, uint32 remapPri)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pri ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < remapPri ), RT_ERR_PRIORITY);


    if ((ret = reg_array_field_write(APOLLOMP_QOS_PRI_REMAP_IN_CPUr, REG_ARRAY_INDEX_NONE, pri, APOLLOMP_PRIf, &remapPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_priRemapToCpu_get
 * Description:
 *      Get remapping priority for packet forward to CPU port
 * Input:
 *      pri 	    - priority
 *      remapPri	- remapping priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *  	RT_ERR_PRIORITY
 *      RT_ERR_NULL_POINTER
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_priRemapToCpu_get(uint32 pri, uint32 *pRemapPri)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pri ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((NULL == pRemapPri ), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(APOLLOMP_QOS_PRI_REMAP_IN_CPUr, REG_ARRAY_INDEX_NONE, pri, APOLLOMP_PRIf, pRemapPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_1QPriRemap_set
 * Description:
 *      Set 802.1Q absolutely priority
 * Input:
 *      pri 	    - priority
 *      remapPri	- remapping priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *  	RT_ERR_PRIORITY
 * Note:
 *      None
 */
int32 apollomp_raw_qos_1QPriRemap_set(uint32 pri, uint32 remapPri)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pri ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < remapPri ), RT_ERR_PRIORITY);


    if ((ret = reg_array_field_write(APOLLOMP_QOS_1Q_PRI_REMAPr, REG_ARRAY_INDEX_NONE, pri, APOLLOMP_INTPRI_1Qf, &remapPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_1QPriRemap_get
 * Description:
 *      Get 802.1Q absolutely priority
 * Input:
 *      pri 	    - priority
 *      remapPri	- remapping priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *  	RT_ERR_PRIORITY
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_1QPriRemap_get(uint32 pri, uint32 *pRemapPri)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pri ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((NULL == pRemapPri ), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_QOS_1Q_PRI_REMAPr, REG_ARRAY_INDEX_NONE, pri, APOLLOMP_INTPRI_1Qf, pRemapPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_dscpPriRemap_set
 * Description:
 *      Set DSCP-based priority
 * Input:
 *      dscp 	    - dscp value
 *      remapPri	- remapping priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *  	RT_ERR_PRIORITY
 *      RT_ERR_QOS_DSCP_VALUE
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_dscpPriRemap_set(uint32 dscp, uint32 remapPri)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < remapPri ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp ), RT_ERR_QOS_DSCP_VALUE);

    if ((ret = reg_array_field_write(APOLLOMP_QOS_DSCP_REMAPr, REG_ARRAY_INDEX_NONE, dscp, APOLLOMP_INTPRI_DSCPf, &remapPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}





/* Function Name:
 *      apollomp_raw_qos_dscpPriRemap_get
 * Description:
 *      Get DSCP-based priority
 * Input:
 *      dscp 	    - dscp value
 *      remapPri	- remapping priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *  	RT_ERR_NULL_POINTER
 *      RT_ERR_QOS_DSCP_VALUE
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_dscpPriRemap_get(uint32 dscp, uint32 *pRemapPri)
{
    int ret;

    RT_PARAM_CHK((NULL == pRemapPri ), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp ), RT_ERR_QOS_DSCP_VALUE);

    if ((ret = reg_array_field_read(APOLLOMP_QOS_DSCP_REMAPr, REG_ARRAY_INDEX_NONE, dscp, APOLLOMP_INTPRI_DSCPf, pRemapPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_portBasePri_set
 * Description:
 *      Set port-based priority
 * Input:
 *      port 	    - port number
 *      priority 	- priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *  	RT_ERR_PRIORITY
 *      RT_ERR_PORT_ID
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_portBasePri_set(rtk_port_t port, uint32 priority)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority ), RT_ERR_PRIORITY);

    if ((ret = reg_array_field_write(APOLLOMP_QOS_PB_PRIr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_INTPRI_PBf, &priority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_portBasePri_get
 * Description:
 *      Get port-based priority
 * Input:
 *      port 	    - port number
 * Output:
 *      pPri     	- priority
 * Return:
 *      RT_ERR_OK
 *  	RT_ERR_PRIORITY
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_portBasePri_get(rtk_port_t port, uint32 *pPri)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPri ), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_QOS_PB_PRIr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_INTPRI_PBf, pPri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_priDecisionWeight_get
 * Description:
 *      Get priority decision weight for given priority type
 * Input:
 *      type 	    - priority type
 * Output:
 *      pWeight     	- priority selector weight
 * Return:
 *      RT_ERR_OK
 *  	RT_ERR_INPUT
 *  	RT_ERR_NULL_POINTER
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_priDecisionWeight_get(apollomp_raw_qos_priType_t type, uint32 *pWeight)
{
    int ret;
    RT_PARAM_CHK(APOLLOMP_QOS_PRI_TYPE_END <= type , RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pWeight ), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case APOLLOMP_QOS_PRI_TYPE_SVLAN:
            if ((ret = reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_SVLAN_WEIGHTf, pWeight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_SA:
            if ((ret = reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_SA_WEIGHTf, pWeight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_LUTFWD:
            if ((ret = reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_LUTFWD_WEIGHTf, pWeight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_CVLAN:
            if ((ret = reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_CVLAN_WEIGHTf, pWeight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_ACL:
            if ((ret = reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_ACL_WEIGHTf, pWeight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_DSCP:
            if ((ret = reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_DSCP_WEIGHTf, pWeight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_DOT1Q:
            if ((ret = reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_DOT1Q_WEIGHTf, pWeight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_PORT:
            if ((ret = reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_PORT_WEIGHTf, pWeight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_L4:
            if ((ret = reg_field_read(APOLLOMP_PRI_SEL_TBL_CTRL2r, APOLLOMP_L4_WEIGHTf, pWeight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_priDecisionWeight_set
 * Description:
 *      Set priority decision weight for given priority type
 * Input:
 *      type 	    - priority type
 *      weight     	- priority selector weight
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *  	RT_ERR_INPUT
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_priDecisionWeight_set(apollomp_raw_qos_priType_t type, uint32 weight)
{
    int ret;
    RT_PARAM_CHK((APOLLOMP_QOS_PRI_TYPE_END <= type) , RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_PRI_WEIGHT_MAX < weight ), RT_ERR_INPUT);

    switch(type)
    {
        case APOLLOMP_QOS_PRI_TYPE_SVLAN:
            if ((ret = reg_field_write(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_SVLAN_WEIGHTf, &weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_SA:
            if ((ret = reg_field_write(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_SA_WEIGHTf, &weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_LUTFWD:
            if ((ret = reg_field_write(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_LUTFWD_WEIGHTf, &weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_CVLAN:
            if ((ret = reg_field_write(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_CVLAN_WEIGHTf, &weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_ACL:
            if ((ret = reg_field_write(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_ACL_WEIGHTf, &weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_DSCP:
            if ((ret = reg_field_write(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_DSCP_WEIGHTf, &weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_DOT1Q:
            if ((ret = reg_field_write(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_DOT1Q_WEIGHTf, &weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_PORT:
            if ((ret = reg_field_write(APOLLOMP_PRI_SEL_TBL_CTRLr, APOLLOMP_PORT_WEIGHTf, &weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_QOS_PRI_TYPE_L4:
            if ((ret = reg_field_write(APOLLOMP_PRI_SEL_TBL_CTRL2r, APOLLOMP_L4_WEIGHTf, &weight)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_1qRemarkAbility_set
 * Description:
 *      Set 1Q remarking ability
 * Input:
 *      port 	    - port number
 *      enable   	- enable remarking ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *  	RT_ERR_INPUT
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_1qRemarkAbility_set(rtk_port_t port, rtk_enable_t enable)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable ), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_RMK_DOT1Q_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_1qRemarkAbility_get
 * Description:
 *      Get 1Q remarking ability
 * Input:
 *      port 	    - port number
 * Output:
 *      pEnable   	- enable remarking ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_1qRemarkAbility_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_RMK_DOT1Q_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_dscpRemarkAbility_set
 * Description:
 *      Set dscp remarking ability
 * Input:
 *      port 	    - port number
 *      enable   	- enable remarking ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *  	RT_ERR_INPUT
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_dscpRemarkAbility_set(rtk_port_t port, rtk_enable_t enable)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable ), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_RMK_DSCP_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_dscpRemarkAbility_get
 * Description:
 *      Get dscp remarking ability
 * Input:
 *      port 	    - port number
 * Output:
 *      pEnable   	- enable remarking ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *
 * Note:
 *      None
 */
int32 apollomp_raw_qos_dscpRemarkAbility_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_RMK_DSCP_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_qos_1qRemarkPri_set
 * Description:
 *      Get 802.1p remarking priority
 * Input:
 *      priority 	- Priority value
 *      pNewPriority - New priority value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 * Note:
 *      None
 */
int32 apollomp_raw_qos_1qRemarkPri_set(uint32 priority, uint32 newPriority)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < newPriority ), RT_ERR_PRIORITY);


    if ((ret = reg_array_field_write(APOLLOMP_RMK_1Q_CTRLr, REG_ARRAY_INDEX_NONE, priority, APOLLOMP_INTPRI_1Qf, &newPriority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_1qRemarkPri_get
 * Description:
 *      Get 802.1p remarking priority
 * Input:
 *      priority 	- Priority value
 *      pNewPriority - New priority value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_1qRemarkPri_get(uint32 priority, uint32 *pNewPriority)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((NULL == pNewPriority), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_RMK_1Q_CTRLr, REG_ARRAY_INDEX_NONE, priority, APOLLOMP_INTPRI_1Qf, pNewPriority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_dscpRemarkDscp_set
 * Description:
 *      Set DSCP remarking priority for original DSCP value
 * Input:
 *      dscp 	- DSCP value
 *      newDscp - New DSCP value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value
 * Note:
 *      None
 */
int32 apollomp_raw_qos_dscpRemarkDscp_set(uint32 dscp, uint32 newDscp)
{
    int ret;

    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp ), RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < newDscp ), RT_ERR_QOS_DSCP_VALUE);

    if ((ret = reg_array_field_write(APOLLOMP_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, dscp, APOLLOMP_INTPRI_DSCPf, &newDscp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_dscpRemarkDscp_get
 * Description:
 *      Get DSCP remarking value for given DSCP prioirty
 * Input:
 *      dscp 	- DSCP value
 * Output:
 *      pNewDscp - New DSCP value
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_dscpRemarkDscp_get(uint32 dscp, uint32 *pNewDscp)
{
    int ret;

    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp ), RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK((NULL == pNewDscp), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, dscp, APOLLOMP_INTPRI_DSCPf, pNewDscp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_interPriRemarkDscp_set
 * Description:
 *      Set DSCP remarking value for internal priority
 * Input:
 *      intPri 	- internal priority value
 *      newDscp - remarking DSCP value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PRIORITY
 *      RT_ERR_QOS_DSCP_VALUE
 * Note:
 *      None
 */
int32 apollomp_raw_qos_interPriRemarkDscp_set(uint32 intPri, uint32 newDscp)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < newDscp ), RT_ERR_QOS_DSCP_VALUE);

    if ((ret = reg_array_field_write(APOLLOMP_RMK_DSCP_INT_PRI_CTRLr, REG_ARRAY_INDEX_NONE, intPri, APOLLOMP_INTPRI_DSCPf, &newDscp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_interPriRemarkDscp_get
 * Description:
 *      Get DSCP remarking value for given DSCP prioirty
 * Input:
 *      intPri 	 - internal priority value
 * Output:
 *      pNewDscp - remarking DSCP value
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_PRIORITY         - Invalid DSCP value
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_interPriRemarkDscp_get(uint32 intPri, uint32 *pNewDscp)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < intPri ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((NULL == pNewDscp), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_RMK_DSCP_INT_PRI_CTRLr, REG_ARRAY_INDEX_NONE, intPri, APOLLOMP_INTPRI_DSCPf, pNewDscp)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_dscpRemarkSource_set
 * Description:
 *      Set DSCP remarking source
 * Input:
 *      port 	    - port number
 *      source      - New DSCP value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_dscpRemarkSource_set(rtk_port_t port, apollomp_raw_dscp_source_t source)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((APOLLOMP_QOS_DSCP_SOURCE_TYPE_END <= source ), RT_ERR_QOS_DSCP_VALUE);

    if ((ret = reg_array_field_write(APOLLOMP_RMK_P_DSCP_SELr, REG_ARRAY_INDEX_NONE, port, APOLLOMP_SELf, &source)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_dscpRemarkSource_get
 * Description:
 *      Get DSCP remarking source
 * Input:
 *      port 	    - port number
 *      source      - New DSCP value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_dscpRemarkSource_get(rtk_port_t port, apollomp_raw_dscp_source_t *pSource)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSource), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_RMK_P_DSCP_SELr,REG_ARRAY_INDEX_NONE, port , APOLLOMP_SELf, pSource)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_wfqBurstSize_set
 * Description:
 *      Set WFQ leaky bucket burst size
 * Input:
 *      burstsize 	- Leaky bucket burst size, unit byte
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32 apollomp_raw_qos_wfqBurstSize_set(uint32 burstsize)
{
    int ret;

    if ((ret = reg_field_write(APOLLOMP_WFQ_CTRLr, APOLLOMP_WFQ_BURSTSIZEf, &burstsize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_wfqBurstSize_get
 * Description:
 *      Get WFQ leaky bucket burst size
 * Input:
 *      burstsize 	- Leaky bucket burst size, unit byte
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_wfqBurstSize_get(uint32 *pPurstsize)
{
    int ret;

    RT_PARAM_CHK((NULL == pPurstsize), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_write(APOLLOMP_WFQ_CTRLr, APOLLOMP_WFQ_BURSTSIZEf, pPurstsize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_wfqCalIncIfg_set
 * Description:
 *      Set WFQ calculation include IFG
 * Input:
 *      mode 	- WFQ calculation include IFG or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_wfqCalIncIfg_set(apollomp_raw_ifg_include_t mode)
{
    int ret;

    RT_PARAM_CHK((APOLLOMP_QOS_IFG_TYPE_END <= mode ), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_WFQ_CTRLr, APOLLOMP_WFQ_IFGf, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_wfqCalIncIfg_set
 * Description:
 *      Set WFQ calculation include IFG
 * Input:
 *      None
 * Output:
 *      pMode 	- WFQ calculation include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_wfqCalIncIfg_get(apollomp_raw_ifg_include_t *pMode)
{
    int ret;

    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_write(APOLLOMP_WFQ_CTRLr, APOLLOMP_WFQ_IFGf, pMode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_bwCtrlEgrRate_set
 * Description:
 *      Set per-port egress rate
 * Input:
 *      port 	    - port number
 *      rate 		- Egress rate ,unit 8kbps
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlEgrRate_set(rtk_port_t port, uint32 rate)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((APOLLOMP_QOS_GRANULARTY_MAX < rate), RT_ERR_OUT_OF_RANGE);

    if ((ret = reg_array_field_write(APOLLOMP_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_bwCtrlEgrRate_get
 * Description:
 *      Set per-port egress rate
 * Input:
 *      port 	    - port number
 *      rate 		- Egress rate ,unit 8kbps
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlEgrRate_get(rtk_port_t port, uint32 *pRate)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RATEf, pRate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_bwCtrlIgrRate_set
 * Description:
 *      Set per-port ingress rate
 * Input:
 *      port 	    - port number
 *      rate 		- Egress rate ,unit 8kbps
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlIgrRate_set(rtk_port_t port, uint32 rate)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((APOLLOMP_QOS_GRANULARTY_MAX < rate), RT_ERR_OUT_OF_RANGE);

    if ((ret = reg_array_field_write(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_bwCtrlIgrRate_get
 * Description:
 *      Set per-port egress rate
 * Input:
 *      port 	    - port number
 *      rate 		- Egress rate ,unit 8kbps
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlIgrRate_get(rtk_port_t port, uint32 *pRate)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RATEf, pRate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_bwCtrlIgrRateCalIncIfg_set
 * Description:
 *      Set per-port ingress rate calculate include/exclude IFG
 * Input:
 *      port 	    - port number
 *      enable    	- enable flow control
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlIgrRateFc_set(rtk_port_t port, rtk_enable_t enable)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable ), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MODEf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_bwCtrlIgrRateCalIncIfg_get
 * Description:
 *      Get per-port ingress rate calculate include/exclude IFG
 * Input:
 *      port 	    - port number
  * Output:
 *      pEnable    	- enable flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlIgrRateFc_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MODEf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_bwCtrlIgrRateCalIncIfg_set
 * Description:
 *      Set per-port  *      Set per-port igress bandwidth control mode
 * Input:
 *      port 	    - port number
 *      mode    	- 1:flow control 0:drop
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlIgrRateCalIncIfg_set(rtk_port_t port, apollomp_raw_ifg_include_t mode)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((APOLLOMP_QOS_IFG_TYPE_END <= mode ), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IFGf, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_bwCtrlIgrRateCalIncIfg_get
 * Description:
 *      Set per-port igress bandwidth control mode
 * Input:
 *      port 	    - port number
  * Output:
 *      pMode    	- 1:flow control 0:drop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlIgrRateCalIncIfg_get(rtk_port_t port, apollomp_raw_ifg_include_t *pMode)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IFGf, pMode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_bwCtrlIgrRateBypass_set
 * Description:
 *      Set ingress bandwidth control bypass RMA, IGMP/MLD, and ethernet type 0x8899
 * Input:
 *      enable    	- enable bypass
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlIgrRateBypass_set(rtk_enable_t enable)
{
    int ret;
    RT_PARAM_CHK((RTK_ENABLE_END <= enable ), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_IGR_BWCTRL_GLB_CTRLr, APOLLOMP_BYPASS_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_bwCtrlIgrRateBypass_get
 * Description:
 *      get  ingress bandwidth control bypass RMA, IGMP/MLD, and ethernet type 0x8899
 * Input:
 *      None
  * Output:
 *      pEnable    	- enable bypass
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlIgrRateBypass_get(rtk_enable_t *pEnable)
{
    int ret;

    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_IGR_BWCTRL_GLB_CTRLr, APOLLOMP_BYPASS_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_bwCtrlIgrBound_set
 * Description:
 *      Set ingress bandwidth control threshold
 * Input:
 *      HighThreshold       - high threshold
 *      LowThreshold       - low threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlIgrBound_set(uint32 highThreshold, uint32 lowThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((APOLLOMP_QOS_INBW_THRESHOLD_MAX < highThreshold), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_QOS_INBW_THRESHOLD_MAX < lowThreshold), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_INBW_BOUNDr, APOLLOMP_HBOUNDf, &highThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_INBW_BOUNDr, APOLLOMP_LBOUNDf, &lowThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_qos_bwCtrlIgrBound_set */

/* Function Name:
 *      apollomp_raw_qos_bwCtrlIgrBound_get
 * Description:
 *      Get global high on/off threshold when flow contrl off.
 * Input:
 *      None
 * Output:
 *      pHighThreshold       - high threshold
 *      pLowThreshold       - low threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_bwCtrlIgrBound_get(uint32 *pHighThreshold, uint32 *pLowThreshold)
{
    int32   ret;

    /*parameter check*/
    RT_PARAM_CHK((pHighThreshold == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pLowThreshold == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_INBW_BOUNDr,APOLLOMP_HBOUNDf, pHighThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_INBW_BOUNDr, APOLLOMP_LBOUNDf, pLowThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollo_raw_flowctrl_globalFcoffHighThreshold_get */


/* Function Name:
 *      apollomp_raw_qos_wfqWeight_set
 * Description:
 *      Set per-port set weight of a queue
 * Input:
 *      port 	- Physical port number (0~7)
 *      qid 	- The queue ID wanted to set
 *      weight - The weight value wanted to set
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_ID
 *      RT_ERR_QOS_QUEUE_WEIGHT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_wfqWeight_set(rtk_port_t port, uint32 qid, uint32 weight)
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
        reg    = APOLLOMP_WFQ_PORT_CFG0r;
        field  = APOLLOMP_WEIGHT0f;
        phyQid = REG_ARRAY_INDEX_NONE;
    }
    else
    {
        RT_PARAM_CHK((127 < weight), RT_ERR_QOS_QUEUE_WEIGHT);
        reg    = APOLLOMP_WFQ_PORT_CFG1_7r;
        field  = APOLLOMP_WEIGHT1_7f;
        phyQid = qid;
    }

    if ((ret = reg_array_field_write(reg, port, phyQid, field, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
#if 0
    if( (0 == qid) && HAL_IS_PON_PORT(port) )
    {
        /* Write [15:10] to APOLLOMP_RSVD_EGR_SCH1r */
        value = ((weight >> 10) & 0x0000003F);
        if ((ret = reg_field_write(APOLLOMP_RSVD_EGR_SCH1r, APOLLOMP_PON_Q0_WEIGHTf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }
#endif
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_wfqWeight_get
 * Description:
 *      Get per-port set weight of a queue
 * Input:
 *      port 	- Physical port number (0~7)
 *      qid 	- The queue ID wanted to set
  * Output:
 *      pWeight - The weight value wanted to set
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_qos_wfqWeight_get(rtk_port_t port, uint32 qid, uint32 *pWeight)
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
        reg    = APOLLOMP_WFQ_PORT_CFG0r;
        field  = APOLLOMP_WEIGHT0f;
        phyQid = REG_ARRAY_INDEX_NONE;
    }
    else
    {
        reg    = APOLLOMP_WFQ_PORT_CFG1_7r;
        field  = APOLLOMP_WEIGHT1_7f;
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
 *      apollomp_raw_qos_aprEnable_set
 * Description:
 *      Set per-port per queue set APR enable
 * Input:
 *      port 	- Physical port number (0~7)
 *      enable 	- enable APR function
 *      qid 	- The queue ID wanted to set
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_qos_aprEnable_set(rtk_port_t port, uint32 qid, rtk_enable_t enable)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() < qid), RT_ERR_QUEUE_ID);


    if ((ret = reg_array_field_write(APOLLOMP_APR_EN_PORT_CFGr, port, qid, APOLLOMP_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_aprEnable_get
 * Description:
 *      Get per-port per queue set APR enable
 * Input:
 *      port 	- Physical port number (0~7)
 *      qid 	- The queue ID wanted to set
  * Output:
 *      pEnable 	- enable APR function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_qos_aprEnable_get(rtk_port_t port, uint32 qid, rtk_enable_t *pEnable)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() < qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_APR_EN_PORT_CFGr, port, qid, APOLLOMP_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_queueType_set
 * Description:
 *      Set per-port per queue set queue type
 * Input:
 *      port 	- Physical port number (0~7)
 *      qid 	- The queue ID wanted to set
 *      type 	- queue type  0: strict 1:WFQ
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_qos_queueType_set(rtk_port_t port, uint32 qid, apollomp_raw_queue_type_t type)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((APOLLOMP_QOS_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() < qid), RT_ERR_QUEUE_ID);

    if ((ret = reg_array_field_write(APOLLOMP_WFQ_TYPE_PORT_CFGr, port, qid, APOLLOMP_QUEUE_TYPEf, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_queueType_get
 * Description:
 *      Set per-port per queue set queue type
 * Input:
 *      port 	- Physical port number (0~7)
 *      qid 	- The queue ID wanted to set
  * Output:
 *      pType 	- queue type  0: strict 1:WFQ
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_qos_queueType_get(rtk_port_t port, uint32 qid, apollomp_raw_queue_type_t *pType)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() < qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_WFQ_TYPE_PORT_CFGr, port, qid, APOLLOMP_QUEUE_TYPEf, pType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_queueEmpty_get
 * Description:
 *      Get queue empty status
 * Input:
 *      port 	- Physical port number (0~7)
 *      qid 	- The queue ID wanted to set
  * Output:
 *      pType 	- queue type  0: strict 1:WFQ
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_qos_queueEmpty_get(uint32 *qEmpty)
{
    int ret;

    RT_PARAM_CHK((NULL == qEmpty), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_P_QUEUE_EMPTYr, APOLLOMP_EMPTYf, qEmpty)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_aprMeter_get
 * Description:
 *      Get per-port per queue meter index
 * Input:
 *      port 	- Physical port number (0~7)
 *      qid 	- The queue ID wanted to set
 *      type 	- queue type  0: strict 1:WFQ
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_qos_aprMeter_get(rtk_port_t port, uint32 qid, uint32 *pMeterIdx)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() < qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pMeterIdx), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_APR_METER_PORT_CFGr, port, qid, APOLLOMP_IDXf, pMeterIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_aprMeter_set
 * Description:
 *      Set per-port per queue meter index
 * Input:
 *      port 	- Physical port number (0~7)
 *      qid 	- The queue ID wanted to set
 *      meterIdx- meter index
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_qos_aprMeter_set(rtk_port_t port, uint32 qid, uint32 meterIdx)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() < qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((8 <= meterIdx), RT_ERR_INPUT);


    if ((ret = reg_array_field_write(APOLLOMP_APR_METER_PORT_CFGr, port, qid, APOLLOMP_IDXf, &meterIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_qos_ethernetAvEnable_set
 * Description:
 *      Set per-port Ethernet AV enable mode
 * Input:
 *      port 	    - port number
 *      enable    	- enable/diasable Ethernet AV mode
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_qos_ethernetAvEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_AVB_PORT_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_ethernetAvEnable_get
 * Description:
 *      Set per-port egress rate calculate include/exclude IFG
 * Input:
 *      port 	    - port number
  * Output:
 *      pEnable    	- enable/diasable Ethernet AV mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_ethernetAvEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int ret;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_AVB_PORT_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_qos_ethernetAvRemapPri_set
 * Description:
 *      Set Ethernet AV remapping priority
 * Input:
 *      priority 	- Priority value
 *      newPriority - New priority value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 * Note:
 *      None
 */
int32 apollomp_raw_qos_ethernetAvRemapPri_set(uint32 priority, uint32 newPriority)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < newPriority ), RT_ERR_PRIORITY);


    if ((ret = reg_array_field_write(APOLLOMP_AVB_PRI_REMAPr, REG_ARRAY_INDEX_NONE, priority, APOLLOMP_PRIf, &newPriority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_qos_ethernetAvRemapPri_get
 * Description:
 *      Get Ethernet AV remapping priority
 * Input:
 *      priority 	- Priority value
 * Output:
 *      pNewPriority - New priority value
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_qos_ethernetAvRemapPri_get(uint32 priority, uint32 *pNewPriority)
{
    int ret;

    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority ), RT_ERR_PRIORITY);
    RT_PARAM_CHK((NULL == pNewPriority), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_AVB_PRI_REMAPr, REG_ARRAY_INDEX_NONE, priority, APOLLOMP_PRIf, pNewPriority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

