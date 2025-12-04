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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of PON MAC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON mac
 */

/*
 * Include Files
 */
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/module.h>
#endif

#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_ponmac.h>
#include <dal/rtl9601b/dal_rtl9601b_pbo.h>
#include <dal/rtl9601b/dal_rtl9601b_switch.h>
#include <osal/time.h>
#include <ioal/mem32.h>
#include <rtk/gpio.h>
#include <dal/rtl9601b/dal_rtl9601b_gpio.h>

/*
 * Symbol Definition
 */

static uint32    ponmac_init = INIT_NOT_COMPLETED;
static rtk_ponmac_mode_t ponMode = PONMAC_MODE_GPON;

/*
 * Macro Declaration
 */


static int32 dal_rtl9601b_ponmac_serdes_patch(uint32 chip_ver);


static int32 _ponmac_physicalQueueId_get(uint32 schedulerId, uint32 logicalQId, uint32 *pPhysicalQid)
{

    if(PONMAC_MODE_GPON == ponMode)
        *pPhysicalQid = RTL9601B_TCONT_QUEUE_MAX *(schedulerId/8) + logicalQId;
    else
        *pPhysicalQid = RTL9601B_TCONT_QUEUE_MAX *(schedulerId/2) + logicalQId;
    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9601b_ponMacQueueDrainOut_set
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
static int32 _rtl9601b_ponMacQueueDrainOutState_set(uint32 qid)
{
    int32 ret;
    uint32  data;
    uint32  tmpData;
    uint32  busy;
    uint32  i;


    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    data = 0;

    /* queue drain out */
    tmpData = 1;
    if ((ret = reg_field_set(RTL9601B_DRN_CMDr, RTL9601B_CFG_DRN_QUEUE_MODEf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_set(RTL9601B_DRN_CMDr, RTL9601B_CFG_DRN_IDXf, &qid,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    tmpData = 1;
    if ((ret = reg_field_set(RTL9601B_DRN_CMDr, RTL9601B_DRN_PSf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /* drain out */
    if ((ret = reg_write(RTL9601B_DRN_CMDr,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /* check busy bit */
    i = 0;
    do
    {
        if ((ret = reg_field_read(RTL9601B_DRN_CMDr, RTL9601B_DRN_FLGf, &busy)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        i++;
    } while (busy && (i < 10000));

    if(i >= 10000)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacQueueDrainOutState_get
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
static int32 _rtl9601b_ponMacQueueDrainOutState_get(rtl9601b_ponmac_draintOutState_t *pState)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((pState  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_field_read(RTL9601B_DRN_CMDr, RTL9601B_DRN_FLGf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9601b_ponMacTcontDrainOutState_set
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
static int32 _rtl9601b_ponMacTcontDrainOutState_set(uint32 tcont)
{
    int32 ret;
    uint32 data;
    uint32 tmpData;
    uint32 i;
    uint32 busy;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);

    data = 0;

    /* set to register */
    tmpData = 0;
    if ((ret = reg_field_set(RTL9601B_DRN_CMDr, RTL9601B_CFG_DRN_QUEUE_MODEf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_set(RTL9601B_DRN_CMDr, RTL9601B_CFG_DRN_IDXf, &tcont,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    tmpData = 1;
    if ((ret = reg_field_set(RTL9601B_DRN_CMDr, RTL9601B_DRN_PSf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /* set to register */
    if ((ret = reg_write(RTL9601B_DRN_CMDr,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /* check busy bit */
    i = 0;
    do
    {
        if ((ret = reg_field_read(RTL9601B_DRN_CMDr, RTL9601B_DRN_FLGf, &busy)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        i++;
    } while (busy && (i < 10000));

    if(i >= 10000)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9601b_ponMacTcontDrainOutState_get
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
static int32 _rtl9601b_ponMacTcontDrainOutState_get(rtl9601b_ponmac_draintOutState_t *pState)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((pState  == NULL), RT_ERR_NULL_POINTER);


    /* set to register */
    if ((ret = reg_field_read(RTL9601B_DRN_CMDr, RTL9601B_DRN_FLGf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacSidToQueueMap_set
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
static int32 _rtl9601b_ponMacSidToQueueMap_set(uint32 sid, uint32 qid)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((128 <= sid),RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_SID2QIDr,REG_ARRAY_INDEX_NONE, sid, RTL9601B_CFG_SID2QIDf, &qid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _rtl9601b_ponMacSidToQueueMap_get
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
static int32 _rtl9601b_ponMacSidToQueueMap_get(uint32 sid, uint32 *pQid)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((128 <= sid),RT_ERR_INPUT);
    RT_PARAM_CHK((pQid  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_SID2QIDr,REG_ARRAY_INDEX_NONE, sid, RTL9601B_CFG_SID2QIDf, pQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacCirRate_set
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
static int32 _rtl9601b_ponMacCirRate_set(uint32 qid, uint32 rate)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((HAL_PONMAC_PIR_CIR_RATE_MAX() < rate),RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_QID_CIR_RATEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      _rtl9601b_ponMacCirRate_get
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
static int32 _rtl9601b_ponMacCirRate_get(uint32 qid, uint32 *pRate)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pRate  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_QID_CIR_RATEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_RATEf, pRate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacPirRate_set
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
static int32 _rtl9601b_ponMacPirRate_set(uint32 qid, uint32 rate)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((HAL_PONMAC_PIR_CIR_RATE_MAX() < rate),RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_QID_PIR_RATEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      _rtl9601b_ponMacPirRate_get
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
static int32 _rtl9601b_ponMacPirRate_get(uint32 qid, uint32 *pRate)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pRate  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_QID_PIR_RATEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_RATEf, pRate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}





/* Function Name:
 *      _rtl9601b_ponMacScheQmap_set
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
static int32 _rtl9601b_ponMacScheQmap_set(uint32 tcont, uint32 map)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK(((HAL_MAX_NUM_OF_GPON_TCONT()) <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);

    /* 9601B tcont 8 always scheduling queue 32 only */
    if(tcont == 8)
    {
        return RT_ERR_OK;
    }

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_SCH_QMAPr,REG_ARRAY_INDEX_NONE, tcont, RTL9601B_MAPPING_TBLf, &map)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacScheQmap_get
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
static int32 _rtl9601b_ponMacScheQmap_get(uint32 tcont, uint32 *pMap)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);

    /* 9601B tcont 8 always scheduling queue 32 only */
    if(tcont == 8)
    {
        *pMap = 0x1;
        return RT_ERR_OK;
    }

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_SCH_QMAPr,REG_ARRAY_INDEX_NONE, tcont, RTL9601B_MAPPING_TBLf, pMap)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      _rtl9601b_ponMac_queueType_set
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
static int32 _rtl9601b_ponMac_queueType_set(uint32 qid, rtk_qos_queue_type_t type)
{
    int32 ret;
    uint32 wData;

    /* Error Checking */
    RT_PARAM_CHK((QUEUE_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    if(STRICT_PRIORITY==type)
        wData = 0;
    else
        wData = 1;

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_WFQ_TYPEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_QUEUE_TYPEf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9601b_ponMac_queueType_get
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
static int32 _rtl9601b_ponMac_queueType_get(uint32 qid, rtk_qos_queue_type_t *pType)
{
    int32 ret;
    uint32 rData;
    /* Error Checking */
    RT_PARAM_CHK((pType  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_WFQ_TYPEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_QUEUE_TYPEf, &rData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if(0==rData)
        *pType= STRICT_PRIORITY;
    else
        *pType= WFQ_WRR_PRIORITY;

    return RT_ERR_OK;
}






/* Function Name:
 *      _rtl9601b_ponMac_wfqWeight_set
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
static int32 _rtl9601b_ponMac_wfqWeight_set(uint32 qid, uint32 weight)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((1024 <= weight), RT_ERR_INPUT);

    RT_PARAM_CHK((0 == weight), RT_ERR_INPUT);

    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_WFQ_WEIGHTr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_WEIGHT_PONf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9601b_ponMac_wfqWeight_get
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
static int32 _rtl9601b_ponMac_wfqWeight_get(uint32 qid, uint32 *pWeight)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((pWeight  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_WFQ_WEIGHTr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_WEIGHT_PONf, pWeight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}






/* Function Name:
 *      _rtl9601b_ponMacTcontEnable_set
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
static int32 _rtl9601b_ponMacTcontEnable_set(uint32 tcont, rtk_enable_t enable)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK(((HAL_MAX_NUM_OF_GPON_TCONT()) <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable ), RT_ERR_INPUT);

    /* 9601B tcont 8 always scheduling queue 32 only */
    if(tcont == 8)
    {
        return RT_ERR_OK;
    }

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_TCONT_ENr,REG_ARRAY_INDEX_NONE, tcont, RTL9601B_TCONT_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacTcontEnable_get
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
static int32 _rtl9601b_ponMacTcontEnable_get(uint32 tcont, rtk_enable_t *pEnable)
{
    int32 ret;

    /* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);
    RT_PARAM_CHK((pEnable  == NULL), RT_ERR_NULL_POINTER);

    /* 9601B tcont 8 always scheduling queue 32 only */
    if(tcont == 8)
    {
        *pEnable = 1;
        return RT_ERR_OK;
    }

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_TCONT_ENr,REG_ARRAY_INDEX_NONE, tcont, RTL9601B_TCONT_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



static int32 _ponmac_schedulerQueue_get(uint32 physicalQid, uint32 *pSchedulerId, uint32 *pLogicalQId)
{
    int32   ret;
    uint32  qMask;
    uint32  maxSchedhlerId;
    uint32  schedhlerBase;
    uint32  i;

    *pLogicalQId = physicalQid%32;

    if(PONMAC_MODE_GPON == ponMode)
    {
    maxSchedhlerId = 8;
    schedhlerBase = (physicalQid/32) * 8;
    }
    else
    {
    maxSchedhlerId = 2;
    schedhlerBase = (physicalQid/32) * 2;
    }

    if(physicalQid == 33)
    {
        *pSchedulerId = 8;
        *pLogicalQId = 0;
        return RT_ERR_OK;
    }

    for(i = schedhlerBase; i<schedhlerBase+maxSchedhlerId ; i++ )
    {
        if ((ret = _rtl9601b_ponMacScheQmap_get(i, &qMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        if(qMask & 1<<(*pLogicalQId))
        {
            *pSchedulerId = i;
            return RT_ERR_OK;
        }
    }
    *pSchedulerId = schedhlerBase;
    return RT_ERR_OK;
}

static int _rtl9601b_serdes_wait_complete(void)
{
    int32 ret;
    uint16 timeout = RTL9601B_SDS_ACCESS_TIMEOUT;
    uint32 value;

    while(timeout != 0)
    {
        timeout --;
        if ((ret = reg_field_read(RTL9601B_SERDES_IND_RDr, RTL9601B_BUSYf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        if(value == 0)
        {
            return RT_ERR_OK;
        }
    }
    return RT_ERR_TIMEOUT;
}

int _rtl9601b_serdes_ind_read(rtl9601b_sds_acc_t sds_acc, uint16 *data)
{
    int32 ret;
    uint32 value;
    uint32 addr = (sds_acc.index << 11) | (sds_acc.page << 5) | (sds_acc.regaddr);

    RT_PARAM_CHK((data  == NULL), RT_ERR_NULL_POINTER);

#ifdef FPGA_DEFINED
    /* Pretend complete */
#else
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_ADRf, &addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    value = 0;
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_WRENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_CMD_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _rtl9601b_serdes_wait_complete()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9601B_SERDES_IND_RDr, RTL9601B_RD_DATf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    *data = value;
#endif

    return RT_ERR_OK;
}

int _rtl9601b_serdes_ind_write(rtl9601b_sds_acc_t sds_acc, uint16 data)
{
    int32 ret;
    uint32 value;
    uint32 addr = (sds_acc.index << 11) | (sds_acc.page << 5) | (sds_acc.regaddr);

#ifdef FPGA_DEFINED
    /* Pretend complete */
#else
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_ADRf, &addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_WRENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    value = data;
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_WDr, RTL9601B_WR_DATf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_CMD_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _rtl9601b_serdes_wait_complete()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif

    return RT_ERR_OK;
}

/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */


/* Function Name:
 *      dal_rtl9601b_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Note:
 */
int32
dal_rtl9601b_ponmac_init(void)
{
    int32   ret;
    uint32 tcont;
    uint32 physicalQid;
    uint32 wData;
    rtl9601b_sds_acc_t sds_acc;
    uint16 wData16;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

#ifdef FPGA_DEFINED
    /*fpga do not have RTL9601B_WSDS_ANA_24*/
#else /*FPGA_DEFINED*/

    sds_acc.index = 1;
    sds_acc.page = 0x21;
    sds_acc.regaddr = 1;
#ifdef CONFIG_BEN_DIFFERENTIAL
    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &wData16)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    /* REG_BEN_TTL_OUT=0 */
    wData16 &= ~(1<<14);
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, wData16)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    /* REG_BEN_SEL_CML=1 */
    wData16 |= (1<<15);
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, wData16)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#else /*CONFIG_BEN_DIFFERENTIAL*/
    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &wData16)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    /* REG_BEN_TTL_OUT=1 */
    wData16 |= (1<<14);
    if ((ret = _rtl9601b_serdes_ind_write(sds_acc, wData16)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif /*CONFIG_BEN_DIFFERENTIAL*/
#endif /*FPGA_DEFINED*/

    /*init PON BW_THRESHOLD*/
    wData = 5;
    if ((ret = reg_field_write(RTL9601B_PON_BW_THRESr, RTL9601B_CFG_BW_LAST_THRESf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*init PON REQ_BW_THRESHOLD*/
    wData = 5;
    if ((ret = reg_field_write(RTL9601B_PON_BW_THRESr, RTL9601B_CFG_BW_RUNT_THRESf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*drant out all queue when pon mac init*/
    if(INIT_COMPLETED == ponmac_init)
    {
        rtk_ponmac_queueCfg_t   queueCfg;
        rtk_ponmac_queue_t logicalQueue;
        uint32 queueId;
        uint32 schedulerId;
        uint32 maxSchedulerId;

        if(PONMAC_MODE_GPON == ponMode)
            maxSchedulerId = HAL_MAX_NUM_OF_GPON_TCONT()-1;
        else
            maxSchedulerId = 1;


        for(schedulerId = 0 ; schedulerId < maxSchedulerId ; schedulerId++ ) {
            memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));
            logicalQueue.schedulerId = schedulerId;

            for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ ) {
                logicalQueue.queueId = queueId;

                if((ret= dal_rtl9601b_ponmac_queue_get(
                             &logicalQueue, 
                             &queueCfg)) == RT_ERR_OK) {

                    /*delete queue*/
                    if((ret= dal_rtl9601b_ponmac_queue_del (
                                 &logicalQueue)) != RT_ERR_OK) {

                        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                        return ret;
                    }
                }
            }
        }
    }

    for(tcont = 0; tcont <HAL_MAX_NUM_OF_GPON_TCONT()-1; tcont++ )
    {
        /*disable T-cont schedule */
        if ((ret = _rtl9601b_ponMacTcontEnable_set(tcont, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /*clear T-cont schedule mask*/
        if ((ret = _rtl9601b_ponMacScheQmap_set(tcont, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

    /*set pbo to enable PIR drop*/
    wData = 1;
    if ((ret = reg_field_write(RTL9601B_PON_SCH_CTRLr, RTL9601B_PON_GEN_PIR_DROPf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PONMAC), "");
        return ret;
    }

    /*set all queue to strict, disable CIR/PIR and disable egress drop*/
    for(physicalQid=0 ; physicalQid<HAL_MAX_NUM_OF_PON_QUEUE(); physicalQid++)
    {
        /*set queue schedule type*/
        if ((ret = _rtl9601b_ponMac_queueType_set(physicalQid, STRICT_PRIORITY)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*set PIR CIR*/
        if ((ret = _rtl9601b_ponMacCirRate_set(physicalQid, 0x0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        if ((ret = _rtl9601b_ponMacPirRate_set(physicalQid, HAL_PONMAC_PIR_CIR_RATE_MAX())) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*set default weight to 1*/
        if ((ret = _rtl9601b_ponMac_wfqWeight_set(physicalQid, 0x1)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

    /* set OMCI MPCP priority to 7 */
    wData = 7;
    if ((ret = reg_field_write(RTL9601B_PON_TRAP_CFGr, RTL9601B_OMCI_MPCP_PRIORITYf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    ponmac_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_init */

#if 0 /* move to dal_rtl9601b_gpon.c*/
static int32
_dal_rtl9601b_ponmac_queue_adjust_flowctrl(void)
{
    int32   ret;
    uint32  qMask;
    uint32  scheduler;
    uint32  totalQueNum;
    uint32  onThreshold, offThreshold;
    uint32  i;

    //osal_printf("_dal_rtl9601b_ponmac_queue_adjust_flowctrl\n\r");

    /* counting total queue number and adjust the flowcontrol threshold */
    totalQueNum=0;
    for(scheduler=0;scheduler<8;scheduler++)
    {
        if ((ret = _rtl9601b_ponMacScheQmap_get(scheduler, &qMask)) == RT_ERR_OK)
        {
            for(i=0;i<32;i++)
            {
                if((qMask>>i) & 0x1)
                {
                    totalQueNum++;
                }
            }
        }
    }
    //osal_printf("totalQueNum=%d \n\r", totalQueNum);
    if(totalQueNum > 24)
        totalQueNum = 24;

    offThreshold = 8000 - totalQueNum*250;
    onThreshold = offThreshold + 50;
    if((ret = rtl9601b_raw_pbo_globalThreshold_set(onThreshold, offThreshold)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}
#endif

/* Function Name:
 *      dal_rtl9601b_ponmac_queue_add
 * Description:
 *      Add queue to given scheduler id and apply queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 *      pQueueCfg     - queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    uint32  qMask;
    uint32  physicalQid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL9601B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    RT_PARAM_CHK((pQueueCfg->type >= QUEUE_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueueCfg->egrssDrop >= RTK_ENABLE_END), RT_ERR_INPUT);


    /* add queue to t-cont schedule mask*/
    if ((ret = _rtl9601b_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(qMask == 0)
    {/*first queue add to this t-cont enable t-cont schedule*/
        if ((ret = _rtl9601b_ponMacTcontEnable_set(pQueue->schedulerId, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    
    /*set Queue id and flow id mapping */
    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);


    /*check if queue already in  qmask do not need drain out/add this queue*/
    if((qMask & (1<<pQueue->queueId))==0)
    {
    
        /* drain out queue before add to tcont */
        if ((ret = _rtl9601b_ponMacQueueDrainOutState_set(physicalQid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    
        /*add queue to qMask*/
        qMask = qMask | (1<<pQueue->queueId);
        if ((ret = _rtl9601b_ponMacScheQmap_set(pQueue->schedulerId, qMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    /*set queue schedule type*/
    if(pQueueCfg->type == STRICT_PRIORITY)
    {
        if ((ret = _rtl9601b_ponMac_queueType_set(physicalQid, STRICT_PRIORITY)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /* for safe, set weight to 1 when strict */
        if ((ret = _rtl9601b_ponMac_wfqWeight_set(physicalQid, 1)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        /*set wfq weight*/
        if ((ret = _rtl9601b_ponMac_wfqWeight_set(physicalQid, pQueueCfg->weight)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        if ((ret = _rtl9601b_ponMac_queueType_set(physicalQid, WFQ_WRR_PRIORITY)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

    /*set PIR CIR*/
    if ((ret = _rtl9601b_ponMacCirRate_set(physicalQid, pQueueCfg->cir)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _rtl9601b_ponMacPirRate_set(physicalQid, pQueueCfg->pir)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#if 0 /* move to dal_rtl9601b_gpon.c*/
    /* adjust the flowcontrol threshold */
    _dal_rtl9601b_ponmac_queue_adjust_flowctrl();
#endif

    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_queue_add */



/* Function Name:
 *      dal_rtl9601b_ponmac_queue_get
 * Description:
 *      get queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      pQueueCfg     - queue configuration
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    rtk_enable_t   enable;
    uint32  qMask;
    uint32  physicalQid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL9601B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if ((ret = _rtl9601b_ponMacTcontEnable_get(pQueue->schedulerId, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(DISABLED==enable)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = _rtl9601b_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(!(qMask & 1<<(pQueue->queueId)))
        return RT_ERR_ENTRY_NOTFOUND;

    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    if ((ret = _rtl9601b_ponMac_queueType_get(physicalQid, &(pQueueCfg->type))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*set wfq weight*/
    if ((ret = _rtl9601b_ponMac_wfqWeight_get(physicalQid, &(pQueueCfg->weight))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*set PIR CIR*/
    if ((ret = _rtl9601b_ponMacCirRate_get(physicalQid, &(pQueueCfg->cir))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _rtl9601b_ponMacPirRate_get(physicalQid, &(pQueueCfg->pir))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    /*get egress drop*/
    pQueueCfg->egrssDrop = DISABLED;


    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_queue_get */


/* Function Name:
 *      dal_rtl9601b_ponmac_queue_del
 * Description:
 *      delete queue from given scheduler id
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  qMask;
    rtk_enable_t   enable;
    uint32  queueId;
    rtk_ponmac_queue_t tmpQ;
    uint32 sid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL9601B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if ((ret = _rtl9601b_ponMacTcontEnable_get(pQueue->schedulerId, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(DISABLED==enable)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = _rtl9601b_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(!(qMask & 1<<(pQueue->queueId)))
        return RT_ERR_ENTRY_NOTFOUND;

    /* change sid to default queue 31 before drain out */
    for(sid=0; sid<HAL_CLASSIFY_SID_NUM(); sid++)
    {
        if((ret = dal_rtl9601b_ponmac_flow2Queue_get(sid, &tmpQ))==RT_ERR_OK)
        {
            if((tmpQ.schedulerId == pQueue->schedulerId) && (tmpQ.queueId == pQueue->queueId))
            {
                if(PONMAC_MODE_GPON == ponMode)
                    tmpQ.schedulerId = 7;
                else
                    tmpQ.schedulerId = 0;

                tmpQ.queueId = 31;
                if((ret = dal_rtl9601b_ponmac_flow2Queue_set(sid, &tmpQ))!=RT_ERR_OK)
                    osal_printf("set to default queue fail! ret=0x%x\n",ret);
            }
        }
        else
            osal_printf("get to sid2queue fail! ret=0x%x\n",ret);
        
    }

    /*drant out queue*/
    queueId = ((pQueue->schedulerId)/8) * 32 + pQueue->queueId;
    if ((ret = _rtl9601b_ponMacQueueDrainOutState_set(queueId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*remove queue from qMask*/
    qMask = qMask & (~(1<<pQueue->queueId));
    if ((ret = _rtl9601b_ponMacScheQmap_set(pQueue->schedulerId, qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(0 == qMask)
    {/*no queue map to this t-cont disable schedule for this t-cont*/
        if ((ret = _rtl9601b_ponMacTcontEnable_set(pQueue->schedulerId, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
#if 0 /* move to dal_rtl9601b_gpon.c*/
    /* adjust the flowcontrol threshold */
    _dal_rtl9601b_ponmac_queue_adjust_flowctrl();
#endif

    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_queue_del */



/* Function Name:
 *      dal_rtl9601b_ponmac_flow2Queue_set
 * Description:
 *      mapping flow to given queue
 * Input:
 *      flow          - flow id.
 *      pQueue        - queue id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  physicalQid;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() <= flow), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL9601B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    if ((ret = _rtl9601b_ponMacSidToQueueMap_set(flow, physicalQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_flow2Queue_set */


/* Function Name:
 *      dal_rtl9601b_ponmac_flow2Queue_get
 * Description:
 *      get queue id for this flow
 * Input:
 *      flow         - flow id.
 * Output:
 *      pQueue       - queue id.
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  physicalQid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() <= flow), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);

    if ((ret = _rtl9601b_ponMacSidToQueueMap_get(flow, &physicalQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _ponmac_schedulerQueue_get(physicalQid, &(pQueue->schedulerId), &(pQueue->queueId))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_flow2Queue_get */

static int32
dal_rtl9601b_ponmac_serdes_patch(uint32 chip_ver)
{
    int32 ret;
    rtl9601b_sds_acc_t sds_acc;
    uint16 data16;

    switch (chip_ver) 
    {
        case CHIP_REV_ID_0:
            /* serdes patch */
            data16 = 0xc36c;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 2;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0x1945;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 6;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0x9188;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 0x16;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0x60b1 ;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_GPON_SPEED;
            sds_acc.regaddr = 3;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0x60b1 ;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_EPON_GSPEED;
            sds_acc.regaddr = 3;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0x60b1 ;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_125G_SPEED;
            sds_acc.regaddr = 3;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0x4a82 ;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 1;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0x6956 ;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 4;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0xcf2 ;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 0xf;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
    
            /* LAN serdes */
            data16 = 0x5ba9 ;
            sds_acc.index = RTL9601B_SDS_IDX_LAN;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 0;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0x8400 ;
            sds_acc.index = RTL9601B_SDS_IDX_LAN;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 3;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0x5558 ;
            sds_acc.index = RTL9601B_SDS_IDX_LAN;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 4;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
        break;

        case CHIP_REV_ID_A:
        case CHIP_REV_ID_B:
        case CHIP_REV_ID_C:
        default:
            data16 = 0x6956 ;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 4;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0xc0c8 ;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 13;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
        break;
    }
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9601b_ponmac_maxPktLen_set
 * Description:
 *      set pon port max packet length
 * Input:
 *      length         - max accept packet length
 *    state          - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                                   - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None
 */
int32 dal_rtl9601b_ponmac_maxPktLen_set(uint32 length)
{
    int32  ret;

    if ((ret = reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_GIGAf, &length)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_10_100f, &length)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9601B_TX_MAX_LEN_CTRLr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_TX_MAX_LENGTH_GIGAf, &length)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9601B_TX_MAX_LEN_CTRLr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_TX_MAX_LENGTH_10_100f, &length)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}

/* Change UTP(0) port max accept/transmit packet length */
static int32 _ponmac_maxPktLenUtp_set(uint32 length)
{
    int32  ret;

    if ((ret = reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, 0, REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_GIGAf, &length)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, 0, REG_ARRAY_INDEX_NONE, RTL9601B_RX_MAX_LENGTH_10_100f, &length)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9601B_TX_MAX_LEN_CTRLr, 0, REG_ARRAY_INDEX_NONE, RTL9601B_TX_MAX_LENGTH_GIGAf, &length)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9601B_TX_MAX_LEN_CTRLr, 0, REG_ARRAY_INDEX_NONE, RTL9601B_TX_MAX_LENGTH_10_100f, &length)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

#ifdef CONFIG_TX_POWER_TURN_ON_FEATURE
static int32 _ponmac_txPower_enable(rtk_enable_t enable)
{
    int32  ret;

    /* set TX power GPIO pin to output 0 */
    if ((ret = dal_rtl9601b_gpio_state_set(CONFIG_TX_POWER_GPO_PIN, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if(enable == ENABLED)
    {
        if ((ret = dal_rtl9601b_gpio_databit_set(CONFIG_TX_POWER_GPO_PIN,0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = dal_rtl9601b_gpio_databit_set(CONFIG_TX_POWER_GPO_PIN,1)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    if ((ret = dal_rtl9601b_gpio_mode_set(CONFIG_TX_POWER_GPO_PIN, GPIO_OUTPUT)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

}
#endif

/* Function Name:
 *      dal_rtl9601b_ponmac_mode_set
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 *    state          - enable or disable
 * Output:
 *      pQueue       - queue id.
 * Return:
 *      RT_ERR_OK                                   - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_mode_set(rtk_ponmac_mode_t mode)
{
    int32 ret;
    uint32 data;
    rtl9601b_sds_acc_t sds_acc;
    uint16 data16;
    uint32 chip_id, chip_ver, dmy_subtype;
    static uint32 pbo_init = 0;
    rtk_ponmac_queueCfg_t   queueCfg;
    rtk_ponmac_queue_t logicalQueue;
    uint32 queueId;
    uint32 flowId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* 0639 need to patch for los case, serdes patch, and lan serdes */
    if((ret = dal_rtl9601b_switch_version_get(&chip_id, &chip_ver, &dmy_subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    /* 0639 need to patch for serdes patch, and lan serdes */
    if ((ret = dal_rtl9601b_ponmac_serdes_patch(chip_ver)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    /* do not add queue 0~7 to scheduler id 0*/
    /*mapping queue 0~7 to schedule id 0*/
    if(PONMAC_MODE_GPON != mode)
    {
        memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));

        logicalQueue.schedulerId = 0 ;

        queueCfg.cir       = 0x0;
        queueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
        queueCfg.type      = STRICT_PRIORITY;
        queueCfg.egrssDrop = DISABLED;
        for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ )
        {
            logicalQueue.queueId = queueId;

            if((ret= dal_rtl9601b_ponmac_queue_add(&logicalQueue, &queueCfg)) != RT_ERR_OK)
            {
                return ret;
            }
        }
    }

    /*mapping all flow to t-cont 0 queue 0*/
    if(PONMAC_MODE_GPON == mode) {
        /* mapping all flow to t-cont 7 queue 31 */
        logicalQueue.schedulerId = 7;
        logicalQueue.queueId     = 31;
    } else {
        logicalQueue.schedulerId = 0;
        logicalQueue.queueId     = 0;
    }

    for(flowId=0 ; flowId<32 ; flowId++) {
        if((ret= dal_rtl9601b_ponmac_flow2Queue_set(
                    flowId, 
                    &logicalQueue)) != RT_ERR_OK) {
            return ret;
        }
    }

    if(PONMAC_MODE_GPON == mode) {
        logicalQueue.schedulerId = 8;
        logicalQueue.queueId = 0;
        if((ret= dal_rtl9601b_ponmac_flow2Queue_set(32, &logicalQueue)) != RT_ERR_OK) {
            return ret;
        }
    }

    switch(mode)
    {
    case PONMAC_MODE_GPON:
#if !defined(FPGA_DEFINED)
        if(!pbo_init)
        {
            if ((ret = rtl9601b_raw_pbo_init()) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            pbo_init = 1;
        }
#endif

        /* 0639 GPON/EPON need to patch for los case */
        if((RTL9601B_CHIP_ID == chip_id) && (0 == chip_ver))
        {
            /* los case patch */
            data16 = 0x4840;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 12;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
        }
        else
        {
            /* CMU_TX bypass ber notify */
            data = 0; /* keep reset digital */
            if ((ret = reg_field_write(RTL9601B_WSDS_DIG_17r, RTL9601B_CFG_SFT_RSTB_INFf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 0; /* keep reset GPON MAC */
            if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r, RTL9601B_CFG_SFT_RSTB_GPONf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 1; /* set clkrd source from original clkrd */
            if ((ret = reg_field_write(RTL9601B_WSDS_DIG_01r, RTL9601B_CFG_CLKRD_ORGf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data16 = 0x4a8a; /* REG_BYPASS_BER_NOTIFY_CMUTX=1 */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 1;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 = 0x4248; /* set RX_SD=IDLE, using OOBS=1 */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 12;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
        }

        /* set the GPON mode enable */
        data = 1;
        if ((ret = reg_write(RTL9601B_PON_MODE_CFGr, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        /* PBO init here */
        if ((ret = rtl9601b_raw_pbo_ponMode_set(RTL9601B_PBO_PONMODE_GPON)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        /* set the serdes mode to GPON mode */
        data = 0x8;
        if ((ret = reg_field_write(RTL9601B_SDS1_CFGr, RTL9601B_CFG_SDS1_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        sds_acc.index = 1;
        sds_acc.page = 0x21;
        sds_acc.regaddr = 11;
        /* REG_RX_FILT_CONFIG=0 */
        data16 = 0;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /* BEN on */
        data = 0x1;
        if ((ret = reg_field_write(RTL9601B_WSDS_DIG_12r, RTL9601B_BEN_OEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        /* pon port allow undersize packet */
        data = 1;
        if ((ret = reg_array_field_write(RTL9601B_P_MISCr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_RX_SPCf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
         /* turn off 'serdes_tx_dis when ben=0' */
        data = 0;
        if ((ret = reg_field_write(RTL9601B_WSDS_DIG_11r, RTL9601B_CFG_EN_PDOWN_BENf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        /* RSVD_PER_PORT_MAC[1][0].bit2 for sys_clk 62.5 anx TXC 155.5M */
        ioal_mem32_read(0x20450, &data);
        data |= 0x4;
        ioal_mem32_write(0x20450, data);

        /* set max packet len to 2031 (2047-4(ctag)-4(stag)-8(pppoe)=2031) */
        if((ret = dal_rtl9601b_ponmac_maxPktLen_set(2031)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        if((ret = _ponmac_maxPktLenUtp_set(2031)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        osal_time_mdelay(10);
        /* 0639 need to patch for los case, serdes patch, and lan serdes */
        if((RTL9601B_CHIP_ID == chip_id) && (chip_ver > 0))
        {
            data16 = 0x4a48; /* set RX_SD */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 12;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            /* for 6244, gpon reset fail issue */
            /* force bernotify */
            if((ret = dal_rtl9601b_ponmac_forceBerNotify_set(ENABLED))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
                return ret;
            }
            data = 1; /* release reset digital */
            if ((ret = reg_field_write(RTL9601B_WSDS_DIG_17r, RTL9601B_CFG_SFT_RSTB_INFf, &data))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
                return ret;
            }
            data = 1; /* release reset GPON MAC */
            if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r, RTL9601B_CFG_SFT_RSTB_GPONf, &data)) != RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
                return ret;
            }
            /* queue reset */
            data = 1;
            if((ret = reg_field_write(RTL9601B_SOFTWARE_RSTr, RTL9601B_CMD_QUEUE_RST_PSf, &data))!=RT_ERR_OK)
            {
                 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
                    return ret;
            }
            data = 0;
            if((ret = reg_field_write(RTL9601B_SOFTWARE_RSTr, RTL9601B_CMD_QUEUE_RST_PSf, &data))!=RT_ERR_OK)
            {
                 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
                    return ret;
            }
        }

        break;
    case PONMAC_MODE_EPON:
#if !defined(FPGA_DEFINED)
        if(!pbo_init)
        {
            if ((ret = rtl9601b_raw_pbo_init()) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            pbo_init = 1;
        }
#endif

        /* 0639 GPON/EPON need to patch for los case */
        if((RTL9601B_CHIP_ID == chip_id) && (0 == chip_ver))
        {
            /* los case patch */
            data16 = 0x4840;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 12;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
        }
        else
        {   /*WSDS_DIG_01[2]=1*/
            data = 4;
            if ((ret = reg_write(RTL9601B_WSDS_DIG_01r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data16 = 0x4a8a;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 1;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            #if 0
            /*CMUTX reference clock change to CDR1*/
            data16 = 0x4860;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 5;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            #endif

            /*REG_CDR_RST_SEL set to 0*/
            data16 = 0xb000;
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
            sds_acc.regaddr = 11;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
        }

        /* set the EPON mode enable */
        data = 2;
        if ((ret = reg_write(RTL9601B_PON_MODE_CFGr, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        /* PBO init here */
        if ((ret = rtl9601b_raw_pbo_ponMode_set(RTL9601B_PBO_PONMODE_EPON)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        if ((ret = ioal_ponNic_write(0xBBF0213C, 0x000003FF)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_FLOWCTRL), "");
            return ret;
        }
        if ((ret = ioal_ponNic_write(0xBBF02140, 0x03FF0000)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_FLOWCTRL), "");
            return ret;
        }
        if ((ret = ioal_ponNic_write(0xBBF0215C, 0x03)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_FLOWCTRL), "");
            return ret;
        }

        /* set the serdes mode to EPON mode */
        data = 0xc;
        if ((ret = reg_field_write(RTL9601B_SDS1_CFGr, RTL9601B_CFG_SDS1_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        /* BEN on */
        data = 0x1;
        if ((ret = reg_field_write(RTL9601B_WSDS_DIG_12r, RTL9601B_BEN_OEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        /*do not accept special packet on pon port*/
        data = 0;
        if ((ret = reg_array_field_write(RTL9601B_P_MISCr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_RX_SPCf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        /*PCS serdes patch*/

        /*SDS_REG*/
        /*SDS_REG01*/
        data16 = 0x0;
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS;
        sds_acc.regaddr = 1;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }


        /*SDS_REG02  for 129 byte ipg issue*/
        data16 = 0x5180;
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS;
        sds_acc.regaddr = 2;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*SDS_REG07*/
        data16 = 0x0059;
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS;
        sds_acc.regaddr = 7;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /*SDS_REG28*/
        data16 = 0xc040;
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS;
        sds_acc.regaddr = 28;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /*SDS_EXT*/
        /*SDS_EXT_REG06*/
        data16 = 0x0043;
        sds_acc.index = 1;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 6;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*SDS_EXT_REG07*/
        data16 = 0x0003;
        sds_acc.index = 1;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 7;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*SDS_EXT_REG12*/
        data16 = 0x5;
        sds_acc.index = 1;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 12;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*SDS_EXT_REG13*/
        data16 = 0x4e6a;
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 13;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*SDS_EXT_REG15*/
        data16 = 0x1162;
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 15;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*SDS_EXT_REG16*/
        data16 = 0xbd2a;
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
        sds_acc.regaddr = 16;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }


         /* turn off 'serdes_tx_dis when ben=0' */
        data = 0;
        if ((ret = reg_field_write(RTL9601B_WSDS_DIG_11r, RTL9601B_CFG_EN_PDOWN_BENf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        /* RSVD_PER_PORT_MAC[1][0].bit2 for sys_clk 62.5 anx TXC 155.5M */
        ioal_mem32_read(0x20450, &data);
        data &= (~0x4);
        ioal_mem32_write(0x20450, data);

        /* set max packet len to 2031 (2047-4(ctag)-4(stag)-8(pppoe)=2031) */
        if((ret = dal_rtl9601b_ponmac_maxPktLen_set(2031)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        if((ret = _ponmac_maxPktLenUtp_set(2031)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        break;
    case PONMAC_MODE_FIBER_1G:
    case PONMAC_MODE_FIBER_100M:
    case PONMAC_MODE_FIBER_AUTO:
    case PONMAC_MODE_FIBER_FORCE_1G:
    case PONMAC_MODE_FIBER_FORCE_100M:
        /* SDS_REG10 */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS;
        sds_acc.regaddr = 10;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        data16 |= (0x3<<13);
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /* Power down Serdes */
        data = 0x1f;
        if ((ret = reg_field_write(RTL9601B_SDS1_CFGr, RTL9601B_CFG_SDS1_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        /* WSDS_DIG_00.CFG_SFT_RSB_ANA - Reset analog Serdes */
        data = 0x0;
        if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r, RTL9601B_CFG_SFT_RSB_ANAf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        data = 0x1;
        if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r, RTL9601B_CFG_SFT_RSB_ANAf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }

        if(mode != PONMAC_MODE_FIBER_AUTO)
        {
            /* SDS_REG4 */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_SDS;
            sds_acc.regaddr = 4;
            if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 |= (0x1<<12);
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 &= ~(0x7<<13);
            if(PONMAC_MODE_FIBER_1G == mode)
            {
                data16 |= (0x4<<13);
            }
            else
            {
                data16 |= (0x5<<13);
            }
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }

            /* FIB_REG00 */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_FIB;
            sds_acc.regaddr = 0;
            if(PONMAC_MODE_FIBER_1G == mode)
            {
                data16 = 0x1140;
            }
            else if(PONMAC_MODE_FIBER_FORCE_1G == mode)
            {
                data16 = 0x0140;
            }
            else
            {
                data16 = 0x2100;
            }
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }

            /* SDS_REG4 */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_SDS;
            sds_acc.regaddr = 4;
            if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            data16 &= ~(0x1<<12);
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
        }
        else
        {
            /* FIB_REG00 */
            sds_acc.index = RTL9601B_SDS_IDX_PON;
            sds_acc.page = RTL9601B_SDS_PAGE_FIB;
            sds_acc.regaddr = 0;
            data16 = 0x1140;
            if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
        }

        /* Config fiber signal detection source */
        /* SDS_PAGE_COMMON REG12 */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
        sds_acc.regaddr = 12;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
#ifdef CONFIG_FIBER_DETECT_OOBS
        /* REG_RX_SEL_SD - SD from OOBS */
        data16 &= ~(1UL<<11);
#else
        /* CONFIG_FIBER_DETECT_SD */
        /* REG_RX_SEL_SD - SD from SD */
        data16 |= (1UL<<11);
    #ifdef CONFIG_FIBER_MODULE_LOSS
        /* REG_RX_SD_POR_SEL */
        data16 |= (1UL<<15);
    #else
        /* REG_RX_SD_POR_SEL */
        data16 &= ~(1UL<<15);
    #endif
#endif
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /* SDS_PAGE_COMMON REG10 */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
        sds_acc.regaddr = 10;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
#ifdef CONFIG_FIBER_DETECT_OOBS
        /* REG_RX_OOBS_EN bit, enable OOBS */
        data16 |= (1UL<<15);
#else
        /* REG_RX_OOBS_EN bit, disable OOBS */
        data16 &= ~(1UL<<15);
#endif
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /* Config serdes mode according to actual settings */
        switch(mode)
        {
        case PONMAC_MODE_FIBER_1G:
        case PONMAC_MODE_FIBER_FORCE_1G:
            /* set the serdes mode to Fiber 1G mode */
            data = 0x4;
            if ((ret = reg_field_write(RTL9601B_SDS1_CFGr, RTL9601B_CFG_SDS1_MODEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            break;
        case PONMAC_MODE_FIBER_100M:
        case PONMAC_MODE_FIBER_FORCE_100M:
            /* set the serdes mode to Fiber 100M mode */
            data = 0x5;
            if ((ret = reg_field_write(RTL9601B_SDS1_CFGr, RTL9601B_CFG_SDS1_MODEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            break;
        /*case PONMAC_MODE_FIBER_AUTO:*/
        default:    
            /* set the serdes mode to Fiber auto mode */
            data = 0x7;
            if ((ret = reg_field_write(RTL9601B_SDS1_CFGr, RTL9601B_CFG_SDS1_MODEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            break;
        }

        /* SDS_REG10 */
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_SDS;
        sds_acc.regaddr = 10;
        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        data16 &= ~(0x3<<13);
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /* RSVD_PER_PORT_MAC[1][0].bit2 for sys_clk 62.5 anx TXC 155.5M */
        ioal_mem32_read(0x20450, &data);
        data &= (~0x4);
        ioal_mem32_write(0x20450, data);

        /* Set UTP/PON port accept/transmit frame size to maximum 16352 */
        if((ret = dal_rtl9601b_ponmac_maxPktLen_set(16352)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        if((ret = _ponmac_maxPktLenUtp_set(16352)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        break;
    default:
        break;
    }

#ifdef CONFIG_TX_POWER_TURN_ON_FEATURE
    /* turn on tx power */
    if((ret =_ponmac_txPower_enable(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }
#endif

    ponMode = mode;
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9601b_ponmac_mode_get
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 * Output:
 *      *pState       - mode state
 * Return:
 *      RT_ERR_OK                                   - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_mode_get(rtk_ponmac_mode_t *pMode)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    *pMode=ponMode;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9601b_ponmac_queueDrainOut_set
 * Description:
 *      Set pon queue drain out.
 * Input:
 *      pQueue - Specified which PON queue will be drain out
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rtl9601b_ponmac_queueDrainOut_set(rtk_ponmac_queue_t *pQueue)
{
    uint32 queueId;
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId > RTL9601B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    /* function body */
    if(pQueue->queueId < RTL9601B_TCONT_QUEUE_MAX)
    {
        queueId = ((pQueue->schedulerId)/8) * 32 + pQueue->queueId;
        if ((ret = _rtl9601b_ponMacQueueDrainOutState_set(queueId)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _rtl9601b_ponMacTcontDrainOutState_set(pQueue->schedulerId)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_queueDrainOut_set */

/* Function Name:
 *      dal_rtl9601b_ponmac_opticalPolarity_get
 * Description:
 *      Get the current optical output polarity
 * Input:
 *      None
 * Output:
 *      pPolarity  - the current output polarity
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
dal_rtl9601b_ponmac_opticalPolarity_get(rtk_ponmac_polarity_t *pPolarity)
{
    int32  ret;
    uint32 val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9601B_WSDS_DIG_12r,RTL9601B_CFG_FRC_BEN_INVf,&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(0 == val)
    {
        *pPolarity = PONMAC_POLARITY_HIGH;
    }
    else
    {
        *pPolarity = PONMAC_POLARITY_LOW;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_opticalPolarity_get */

/* Function Name:
 *      dal_rtl9601b_ponmac_opticalPolarity_set
 * Description:
 *      Set optical output polarity
 * Input:
 *      polarity - the optical output polarity
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rtl9601b_ponmac_opticalPolarity_set(rtk_ponmac_polarity_t polarity)
{
    int32  ret;
    uint32 val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((PONMAC_POLARITY_END <= polarity), RT_ERR_INPUT);

    /* function body */
    if(PONMAC_POLARITY_HIGH == polarity)
    {
        val = 0;
    }
    else
    {
        val = 1;
    }

    if ((ret = reg_field_write(RTL9601B_WSDS_DIG_12r,RTL9601B_CFG_FRC_BEN_INVf,&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_opticalPolarity_set */

/* Function Name:
 *      dal_rtl9601b_ponmac_losState_get
 * Description:
 *      Get the current optical lost of signal (LOS) state
 * Input:
 *      None
 * Output:
 *      pEnable  - the current optical lost of signal state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
dal_rtl9601b_ponmac_losState_get(rtk_enable_t *pState)
{
    uint16  tmpVal;
    int32   ret;
    rtl9601b_sds_acc_t sds_acc;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
    sds_acc.regaddr = 29;

    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if((tmpVal & (1<<8)) != 0)
        *pState = DISABLED;
    else
        *pState = ENABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_losState_get */

/* Function Name:
 *      dal_rtl9601b_ponmac_serdesCdr_reset
 * Description:
 *      Serdes CDR reset
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rtl9601b_ponmac_serdesCdr_reset(void)
{
    rtl9601b_sds_acc_t sds_acc;
    uint16 wData16, rData16;
    int32 ret;
    uint32 chip_id, chip_ver, dmy_subtype;

    /* 0639 need to patch for los case, serdes patch, and lan serdes */
    if((ret = dal_rtl9601b_switch_version_get(&chip_id, &chip_ver, &dmy_subtype)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    /* 0639 user serdes CDR reset */
    if((RTL9601B_CHIP_ID == chip_id) && (0 == chip_ver))
    {
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
        sds_acc.regaddr = 19;

        wData16 = 0x6000;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, wData16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        wData16 = 0x2000;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, wData16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        osal_time_mdelay(1);

    }
    else /* other use toggle RX_SD polarity */
    {
        sds_acc.index = RTL9601B_SDS_IDX_PON;
        sds_acc.page = RTL9601B_SDS_PAGE_COMMON;
        sds_acc.regaddr = 12;

        if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &rData16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        wData16 = (rData16&(~0x8000))|((!((rData16&0x8000)>>15))<<15);
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, wData16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        osal_time_usleep(10000);

        wData16 = rData16;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, wData16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        {
            uint32 data;

            /* fifo reset */
            data = 0; /* keep reset GPON MAC */
            if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r, RTL9601B_CFG_SFT_RSTB_GPONf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 0; /* keep reset digital */
            if ((ret = reg_field_write(RTL9601B_WSDS_DIG_17r, RTL9601B_CFG_SFT_RSTB_INFf, &data))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
                return ret;
            }
            osal_time_mdelay(10);
            data = 1; /* release reset digital */
            if ((ret = reg_field_write(RTL9601B_WSDS_DIG_17r, RTL9601B_CFG_SFT_RSTB_INFf, &data))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
                return ret;
            }
            data = 1; /* release reset GPON MAC */
            if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r, RTL9601B_CFG_SFT_RSTB_GPONf, &data)) != RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "return failed ret value = %x",ret);
                return ret;
            }

            /* queue reset */
            data = 1;
            if((ret = reg_field_write(RTL9601B_SOFTWARE_RSTr, RTL9601B_CMD_QUEUE_RST_PSf, &data))!=RT_ERR_OK)
            {
                 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
                    return ret;
            }
            data = 0;
            if((ret = reg_field_write(RTL9601B_SOFTWARE_RSTr, RTL9601B_CMD_QUEUE_RST_PSf, &data))!=RT_ERR_OK)
            {
                 RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
                    return ret;
            }
        }
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_serdesCdr_reset */

/* Function Name:
 *      dal_rtl9601b_ponmac_linkState_get
 * Description:
 *      check SD and Sync state of GPON/EPON
 * Input:
 *      mode - GPON or EPON mode
 * Output:
 *      pSd     - pointer of signal detect
 *      pSync   - pointer of sync state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rtl9601b_ponmac_linkState_get(rtk_ponmac_mode_t mode, uint32 *pSd, uint32 *pSync)
{
    uint32 val, val1;
    int32 ret;

    switch(mode)
    {
        case PONMAC_MODE_GPON:
            if ((ret = reg_field_read(RTL9601B_GPON_GTC_DS_INTR_STSr,RTL9601B_LOSf,&val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            *pSd = !val;
            if ((ret = reg_field_read(RTL9601B_GPON_GTC_DS_INTR_STSr,RTL9601B_LOFf,&val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            if ((ret = reg_field_read(RTL9601B_GPON_GTC_DS_INTR_STSr,RTL9601B_LOMf,&val1)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            *pSync = !(val|val1);
            break;

        case PONMAC_MODE_EPON:
            if ((ret = reg_field_read(RTL9601B_SDS1_STATUSr, RTL9601B_SDS1_SDET_OUTf, pSd)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            if ((ret = reg_field_read(RTL9601B_SDS1_STATUSr, RTL9601B_SDS1_LINK_OKf, pSync)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_linkState_get */

/* Function Name:
 *      dal_rtl9601b_ponmac_forceBerNotify_set
 * Description:
 *      force Ber Notify set
 * Input:
 *      state: force or un-force Ber Notify
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rtl9601b_ponmac_forceBerNotify_set(rtk_enable_t state)
{
    rtl9601b_sds_acc_t sds_acc;
    uint16 data16;
    int32 ret;

    if(state == ENABLED)
    {
        /* force BER NOTIFY */
        data16 = 0x3000;
        sds_acc.index = 1;
        sds_acc.page = 0x20;
        sds_acc.regaddr = 2;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        /* force BER NOTIFY */
        data16 = 0x0;
        sds_acc.index = 1;
        sds_acc.page = 0x20;
        sds_acc.regaddr = 2;
        if ((ret = _rtl9601b_serdes_ind_write(sds_acc, data16)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9601b_ponmac_schedulingType_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      pQueueType  - the WFQ schedule type (DAL_RTL9601B_QOS_WFQ or DAL_RTL9601B_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
dal_rtl9601b_ponmac_schedulingType_get(rtk_qos_scheduling_type_t *pQueueType)
{
    uint32  val;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueueType), RT_ERR_NULL_POINTER);

    /* function body */

    if ((ret = reg_field_read(RTL9601B_PON_SCH_CTRLr, RTL9601B_PON_WFQ_MODEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PONMAC), "");
        return ret;
    }
    if(0==val)
        *pQueueType=RTK_QOS_WFQ;
    else
        *pQueueType=RTK_QOS_WRR;



    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_schedulingType_get */


/* Function Name:
 *      dal_rtl9601b_ponmac_schedulingType_set
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      queueType  - the WFQ schedule type (DAL_RTL9601B_QOS_WFQ or DAL_RTL9601B_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
dal_rtl9601b_ponmac_schedulingType_set(rtk_qos_scheduling_type_t queueType)
{
    uint32  val;
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "queueType=%d",queueType);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((SCHEDULING_TYPE_END <=queueType), RT_ERR_INPUT);

    /* function body */
    if(RTK_QOS_WFQ==queueType)
        val=0;
    else
        val=1;

    if ((ret = reg_field_write(RTL9601B_PON_SCH_CTRLr, RTL9601B_PON_WFQ_MODEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PONMAC), "");
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_schedulingType_set */


/* Function Name:
 *      dal_rtl9601b_ponmac_egrBandwidthCtrlRate_get
 * Description:
 *      Set the pon port egress bandwidth control rate.
 * Input:
 *      scheduleId - T-cont is or LLID index
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
int32
dal_rtl9601b_ponmac_egrScheduleIdRate_get(uint32 scheduleId,uint32 *pRate)
{
    int32 ret;
    uint32 data;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "scheduleId=%d",scheduleId);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_GPON_TCONT_MAX <=scheduleId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_array_field_read(RTL9601B_PON_SCH_RATEr,REG_ARRAY_INDEX_NONE, scheduleId, RTL9601B_RATEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    *pRate = data;
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_egrScheduleIdRate_get */





/* Function Name:
 *      dal_rtl9601b_ponmac_egrBandwidthCtrlRate_set
 * Description:
 *      Set the pon port egress bandwidth control rate.
 * Input:
 *      scheduleId - T-cont is or LLID index
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
int32
dal_rtl9601b_ponmac_egrScheduleIdRate_set(uint32 scheduleId,uint32 rate)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "scheduleId=%d,rate=%d",scheduleId, rate);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_GPON_TCONT_MAX <=scheduleId), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_PONMAC_PIR_CIR_RATE_MAX() < rate), RT_ERR_INPUT);

    /* function body */
    if ((ret = reg_array_field_write(RTL9601B_PON_SCH_RATEr,REG_ARRAY_INDEX_NONE, scheduleId, RTL9601B_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_egrScheduleIdRate_set */

#if defined(CONFIG_SDK_KERNEL_LINUX)
EXPORT_SYMBOL(_rtl9601b_serdes_ind_write);
EXPORT_SYMBOL(_rtl9601b_serdes_ind_read);
#endif

