/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : switch asic-level PON MAC API
 * Feature : PON MAC related functions
 *
 */

#include <dal/apollomp/raw/apollomp_raw_ponmac.h>

/* Function Name:
 *      apollomp_raw_ponMacQueueDrainOut_set
 * Description:
 *      Set PON mac per queue draint out state
 * Input:
 *      qid         - Specify the draint out Queue id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacQueueDrainOutState_set(uint32 qid)
{
    int32 ret;
    uint32  data=0;
    uint32  tmpData;
    uint32  busy;
    uint32  i;
    
   
    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    if ((ret = reg_read(APOLLOMP_PONMAC_DRN_CTRLr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /* queue drain out */
    tmpData = 0;
    if ((ret = reg_field_set(APOLLOMP_PONMAC_DRN_CTRLr, APOLLOMP_PON_DRN_SELf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_set(APOLLOMP_PONMAC_DRN_CTRLr, APOLLOMP_PON_DRN_IDXf, &qid,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    tmpData = 1;
    if ((ret = reg_field_set(APOLLOMP_PONMAC_DRN_CTRLr, APOLLOMP_PON_DRN_ENf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /* drain out */
    if ((ret = reg_write(APOLLOMP_PONMAC_DRN_CTRLr,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /* check busy bit */
    i = 0;
    do
    {
        if ((ret = reg_field_read(APOLLOMP_PONMAC_DRN_CTRLr, APOLLOMP_PON_DRN_BUSYf, &busy)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        i++;
    } while (busy && (i < 10000));

    /* stop drain out */
    data = 0;
    if ((ret = reg_write(APOLLOMP_PONMAC_DRN_CTRLr,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_ponMacQueueDrainOutState_get
 * Description:
 *      Get PON mac queue draint out state
 * Input:
 *      None
 * Output:
 *      pState - pon queue draint out status
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacQueueDrainOutState_get(apollomp_raw_ponmac_draintOutState_t *pState)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((pState  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_field_read(APOLLOMP_PONMAC_DRN_CTRLr, APOLLOMP_PON_DRN_ENf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_ponMacTcontDrainOutState_set
 * Description:
 *      Set PON mac per T-cont draint out state
 * Input:
 *      tcont       - Specify the draint out T-cont id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_GPON_EXCEED_MAX_TCONT
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacTcontDrainOutState_set(uint32 tcont)
{
    int32 ret;
    uint32 data=0;
    uint32 tmpData;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);



    /* set to register */
    tmpData = 0;
    if ((ret = reg_field_set(APOLLOMP_PONMAC_DRN_CTRLr, APOLLOMP_PON_DRN_SELf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_set(APOLLOMP_PONMAC_DRN_CTRLr, APOLLOMP_PON_DRN_IDXf, &tcont,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    tmpData = 1;
    if ((ret = reg_field_set(APOLLOMP_PONMAC_DRN_CTRLr, APOLLOMP_PON_DRN_ENf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    /* set to register */
    if ((ret = reg_write(APOLLOMP_PON_PORT_CTRLr,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_ponMacTcontDrainOutState_get
 * Description:
 *      Get PON mac T-cont draint out state
 * Input:
 *      None
 * Output:
 *      pState - T-cont draint out status
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacTcontDrainOutState_get(apollomp_raw_ponmac_draintOutState_t *pState)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((pState  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_field_write(APOLLOMP_PONMAC_DRN_CTRLr, APOLLOMP_PON_DRN_ENf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_ponMacSidToQueueMap_set
 * Description:
 *      Set PON mac session id to queue maping
 * Input:
 *      sid         - Specify the session id
 *      qid         - Specify the mapping queue id id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacSidToQueueMap_set(uint32 sid, uint32 qid)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((128 <= sid),RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(APOLLOMP_PON_SID_TO_QIDr,REG_ARRAY_INDEX_NONE, sid, APOLLOMP_QIDf, &qid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_ponMacSidToQueueMap_get
 * Description:
 *      Get PON mac session id to queue maping
 * Input:
 *      tcont       - Specify the draint out T-cont id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacSidToQueueMap_get(uint32 sid, uint32 *pQid)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((128 <= sid),RT_ERR_INPUT);
    RT_PARAM_CHK((pQid  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(APOLLOMP_PON_SID_TO_QIDr,REG_ARRAY_INDEX_NONE, sid, APOLLOMP_QIDf, pQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_ponMacCirRate_set
 * Description:
 *      Per queue set PON mac CIR rate
 * Input:
 *      qid         - - Specify the queue id
 *      rate        - Specify the CIR rate unit 64kbps
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacCirRate_set(uint32 qid, uint32 rate)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((HAL_PONMAC_PIR_CIR_RATE_MAX() < rate),RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(APOLLOMP_PON_QID_CIR_RATEr,REG_ARRAY_INDEX_NONE, qid, APOLLOMP_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_ponMacCirRate_get
 * Description:
 *      Per queue set PON mac CIR rate
 * Input:
 *      qid         - - Specify the queue id
 * Output:
 *      pRate       - Specify the CIR rate unit 64kbps
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacCirRate_get(uint32 qid, uint32 *pRate)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pRate  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(APOLLOMP_PON_QID_CIR_RATEr,REG_ARRAY_INDEX_NONE, qid, APOLLOMP_RATEf, pRate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_ponMacPirRate_set
 * Description:
 *      Per queue set PON mac PIR rate
 * Input:
 *      qid         - Specify the queue id
 *      rate        - Specify the CIR rate unit 64kbps
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacPirRate_set(uint32 qid, uint32 rate)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((HAL_PONMAC_PIR_CIR_RATE_MAX() < rate),RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(APOLLOMP_PON_QID_PIR_RATEr,REG_ARRAY_INDEX_NONE, qid, APOLLOMP_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_ponMacPirRate_get
 * Description:
 *      Per queue set PON mac PIR rate
 * Input:
 *      qid         - - Specify the queue id
 * Output:
 *      pRate       - Specify the PIR rate unit 64kbps
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacPirRate_get(uint32 qid, uint32 *pRate)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pRate  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(APOLLOMP_PON_QID_PIR_RATEr,REG_ARRAY_INDEX_NONE, qid, APOLLOMP_RATEf, pRate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}





/* Function Name:
 *      apollomp_raw_ponMacScheQmap_set
 * Description:
 *      Per T-CONT/LLID set schedule queue map
 * Input:
 *      tcont       - Specify T-CONT or LLID
 *      map         - Specify schedule queue mapping mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacScheQmap_set(uint32 tcont, uint32 map)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);

    /* set to register */
    if ((ret = reg_array_field_write(APOLLOMP_PON_SCH_QMAPr,REG_ARRAY_INDEX_NONE, tcont, APOLLOMP_MAPPING_TBLf, &map)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_ponMacScheQmap_get
 * Description:
 *      Per T-CONT/LLID get schedule queue map
 * Input:
 *      tcont       - Specify T-CONT or LLID
 * Output:
 *      pMap        - Specify schedule queue mapping mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacScheQmap_get(uint32 tcont, uint32 *pMap)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);

    /* set to register */
    if ((ret = reg_array_field_read(APOLLOMP_PON_SCH_QMAPr,REG_ARRAY_INDEX_NONE, tcont, APOLLOMP_MAPPING_TBLf, pMap)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_ponMac_queueType_set
 * Description:
 *      Set queue set queue type for PON MAC
 * Input:
 *      qid     - The queue ID wanted to set (0~127)
 *      type    - queue type  0: strict 1:WFQ
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMac_queueType_set(uint32 qid, apollomp_raw_queue_type_t type)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((APOLLOMP_QOS_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    /* set to register */
    if ((ret = reg_array_field_write(APOLLOMP_PON_WFQ_TYPEr,REG_ARRAY_INDEX_NONE, qid, APOLLOMP_QUEUE_TYPEf, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_ponMac_queueType_get
 * Description:
 *      Set queue get queue type for PON MAC
 * Input:
 *      qid     - The queue ID wanted to set (0~127)
 *      type    - queue type  0: strict 1:WFQ
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMac_queueType_get(uint32 qid, apollomp_raw_queue_type_t *pType)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((pType  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    /* set to register */
    if ((ret = reg_array_field_read(APOLLOMP_PON_WFQ_TYPEr,REG_ARRAY_INDEX_NONE, qid, APOLLOMP_QUEUE_TYPEf, pType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}






/* Function Name:
 *      apollomp_raw_ponMac_wfqWeight_set
 * Description:
 *      Set queue set WFQ queue weight for PON MAC
 * Input:
 *      qid     - The queue ID wanted to set (0~127)
 *      weight  - WFQ weight
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMac_wfqWeight_set(uint32 qid, uint32 weight)
{
    int32 ret;
    uint32 value;
    
    /* Error Checking */
    if(0 == qid)
        RT_PARAM_CHK((65535 <= weight), RT_ERR_INPUT);
    else
        RT_PARAM_CHK((1024 <= weight), RT_ERR_INPUT);
    
    RT_PARAM_CHK((0 == weight), RT_ERR_INPUT);

    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    /* set to register */
    if ((ret = reg_array_field_write(APOLLOMP_PON_WFQ_WEIGHTr,REG_ARRAY_INDEX_NONE, qid, APOLLOMP_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(0 == qid)
    {
        /* Write [15:10] to APOLLOMP_RSVD_EGR_SCH1r */
        value = ((weight >> 10) & 0x0000003F);
        if ((ret = reg_field_write(APOLLOMP_RSVD_EGR_SCH1r, APOLLOMP_PON_Q0_WEIGHTf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }


    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_ponMac_wfqWeight_get
 * Description:
 *      Set queue set WFQ queue weight for PON MAC
 * Input:
 *      qid     - The queue ID wanted to set (0~127)
  * Output:
 *      pWeight     - WFQ weight
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMac_wfqWeight_get(uint32 qid, uint32 *pWeight)
{
    int32 ret;
    uint32 value;

    /* Error Checking */
    RT_PARAM_CHK((pWeight  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    /* set to register */
    if ((ret = reg_array_field_read(APOLLOMP_PON_WFQ_WEIGHTr,REG_ARRAY_INDEX_NONE, qid, APOLLOMP_WEIGHTf, pWeight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    if(0 == qid)
    {
        /* Read [15:10] to APOLLOMP_RSVD_EGR_SCH1r */
        if ((ret = reg_field_read(APOLLOMP_RSVD_EGR_SCH1r, APOLLOMP_PON_Q0_WEIGHTf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        *pWeight = ((value << 10) | *pWeight);
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_ponMac_egrDropState_set
 * Description:
 *      Set PON MAC queue egress drop enable state
 * Input:
 *      qid     - The queue ID wanted to set (0~127)
 *      state   - egress drop enable state
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMac_egrDropState_set(uint32 qid, rtk_enable_t state)
{
    int32 ret;
    uint32 reg;

    /* Error Checking */
    RT_PARAM_CHK((RTK_ENABLE_END <= state ), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    if (ENABLED == state)
        reg = 0;
    else
        reg = 1;

    /* set to register */
    if ((ret = reg_array_field_write(APOLLOMP_FC_PON_Q_EGR_DROP_ENr,REG_ARRAY_INDEX_NONE, qid, APOLLOMP_THf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_ponMac_egrDropState_get
 * Description:
 *      Get PON MAC queue egress drop enable state
 * Input:
 *      qid     - The queue ID wanted to set (0~127)
  * Output:
 *      pState  - egress drop enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMac_egrDropState_get(uint32 qid, rtk_enable_t *pState)
{
    int32 ret;
    uint32 reg;

    /* Error Checking */
    RT_PARAM_CHK((pState  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    /* set to register */
    if ((ret = reg_array_field_read(APOLLOMP_FC_PON_Q_EGR_DROP_ENr,REG_ARRAY_INDEX_NONE, qid, APOLLOMP_THf, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if (0 == reg)
        *pState = ENABLED;
    else
        *pState = DISABLED;

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_ponMacTcontEnable_set
 * Description:
 *      Per T-CONT set schedule enable state
 * Input:
 *      tcont       - Specify T-CONT
 *      enable      - Specify T-CONT schedule enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacTcontEnable_set(uint32 tcont, rtk_enable_t enable)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable ), RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(APOLLOMP_PON_TCONT_ENr,REG_ARRAY_INDEX_NONE, tcont, APOLLOMP_TCONT_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_ponMacTcontEnable_get
 * Description:
 *      Per T-CONT get schedule enable state
 * Input:
 *      tcont       - Specify T-CONT
 * Output:
 *      pEnable         - T-CONT schedule enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
int32 apollomp_raw_ponMacTcontEnable_get(uint32 tcont, rtk_enable_t *pEnable)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);
    RT_PARAM_CHK((pEnable  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(APOLLOMP_PON_TCONT_ENr,REG_ARRAY_INDEX_NONE, tcont, APOLLOMP_TCONT_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
