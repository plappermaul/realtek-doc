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
#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/ponmac.h>
#include <dal/dal_mgmt.h>
#include <osal/lib.h>
#include <module/gpon/gpon.h>
#include <hal/common/halctrl.h>

/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */


/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */

/* By default, TX output power with LSB equal to 0.1 uW*/
static uint32 transceiver_tx_pwr_scale = 1;
static uint32 transceiver_addrs[][2]={
    {20,16},
    {68,16},
    {40,16},
    {96,2},
    {98,2},
    {100,2},
    {102,2},
    {104,2},
};
/* Function Name:
 *      rtk_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_init(void)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_init */



/* Function Name:
 *      rtk_ponmac_switchBufferMode_set
 * Description:
 *      Configure PON MAC switch buffer setting 
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_switchBufferMode_set(rtk_flowctrl_patch_t patch_type)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_switchBufferMode_set(patch_type);
    RTK_API_UNLOCK();
    return ret;    
    
}/* end of rtk_ponmac_switchBufferMode_set */



/* Function Name:
 *      rtk_ponmac_queue_add
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
rtk_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_queue_add)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queue_add( pQueue, pQueueCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_queue_add */



/* Function Name:
 *      rtk_ponmac_queue_get
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
rtk_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_queue_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queue_get( pQueue, pQueueCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_queue_get */


/* Function Name:
 *      rtk_ponmac_queue_del
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
rtk_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_queue_del)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queue_del( pQueue);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_queue_del */



/* Function Name:
 *      rtk_ponmac_flow2Queue_set
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
rtk_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_flow2Queue_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_flow2Queue_set( flow, pQueue);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_flow2Queue_set */


/* Function Name:
 *      rtk_ponmac_flow2Queue_get
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
rtk_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_flow2Queue_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_flow2Queue_get( flow, pQueue);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_ponmac_flow2Queue_get */


/* Function Name:
 *      dal_apollo_ponmac_mode_set
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 *    state          - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                                   - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ponmac_mode_set(rtk_ponmac_mode_t mode)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_set( mode);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      dal_apollo_ponmac_mode_get
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 * Output:
 *      *state       - point of state, enable or disable
 * Return:
 *      RT_ERR_OK                                   - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ponmac_mode_get(rtk_ponmac_mode_t *pMode)
{
    int32   ret;

    if (NULL == RT_MAPPER->ponmac_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_mode_get(pMode);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_ponmac_queueDrainOut_set
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
rtk_ponmac_queueDrainOut_set(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_queueDrainOut_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queueDrainOut_set(pQueue);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_queueDrainOut_set */

/* Function Name:
 *      rtk_ponmac_opticalPolarity_get
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
rtk_ponmac_opticalPolarity_get(rtk_ponmac_polarity_t *pPolarity)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_opticalPolarity_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_opticalPolarity_get(pPolarity);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_opticalPolarity_get */

/* Function Name:
 *      rtk_ponmac_opticalPolarity_set
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
rtk_ponmac_opticalPolarity_set(rtk_ponmac_polarity_t polarity)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_opticalPolarity_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_opticalPolarity_set(polarity);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_opticalPolarity_set */

/* Function Name:
 *      rtk_ponmac_losState_get
 * Description:
 *      Get the current optical lost of signal (LOS) state
 * Input:
 *      None
 * Output:
 *      pState  - the current optical lost of signal state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_ponmac_losState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_losState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_losState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_losState_get */

static int32 _wapper_i2c_read(uint32 devID, uint32 idx, uint32 *dataP)
{
    int32 ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->i2c_read(TRANSCEIVER_PORT, devID, idx, dataP);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
*      rtk_ponmac_transceiver_get
* Description:
*      Get transceiver value by the specific type
* Input:
*      type            - the transceiver parameter type
* Output:
*      pPara           - the pointer of data for the specific transceiver parameter
* Return:
*      RT_ERR_OK       - successful
*      others          - fail
* Note:
*
*/
int32
rtk_ponmac_transceiver_get(rtk_transceiver_parameter_type_t type, rtk_transceiver_data_t *pData)
{
    int32   ret;
    uint32  devID = 0, idx;
    uint32 transceiver_data[TRANSCEIVER_LEN];

    osal_memset(transceiver_data, 0, TRANSCEIVER_LEN);

    /* function body */
    if (NULL == RT_MAPPER->i2c_read)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (RTK_TRANSCEIVER_PARA_TYPE_MAX == type)
        return RT_ERR_INPUT;

    if  ((type == RTK_TRANSCEIVER_PARA_TYPE_VENDOR_NAME)||(type == RTK_TRANSCEIVER_PARA_TYPE_SN)|| (type == RTK_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM))
    {
        devID = TRANSCEIVER_A0;
    }else if ((type > RTK_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM)  && (type < RTK_TRANSCEIVER_PARA_TYPE_MAX))
    {
        devID = TRANSCEIVER_A2;
    }

    for (idx = transceiver_addrs[type][0]; idx < transceiver_addrs[type][0]+transceiver_addrs[type][1]; idx++)
    {
    //ret = _wapper_i2c_read(devID, idx, &(transceiver_data[idx-transceiver_addrs[type][0]]));
    ret = rtk_i2c_read(TRANSCEIVER_PORT, devID, idx, &(transceiver_data[idx-transceiver_addrs[type][0]]));
    if(ret != RT_ERR_OK)
    {
        return ret;
    }
    pData->buf[idx-transceiver_addrs[type][0]]=transceiver_data[idx-transceiver_addrs[type][0]];
    }
	pData->tx_pwr_scale=transceiver_tx_pwr_scale;
    return ret;
}   /* end of rtk_ponmac_transceiver_get */

/* Function Name:
*              rtk_ponmac_transceiver_tx_power_scale_set
* Description:
*              Set transceiver tx power scale 
* Input:
*              scale   - scale for TX output power with LSB (in unit 0.1 uW)
* Output:
*              N/A
* Return:
*              RT_ERR_OK       
*              others  
* Note:
*
*/

int32
rtk_ponmac_transceiver_tx_power_scale_set(uint32 scale)
{
	int32 ret = RT_ERR_OK;
	transceiver_tx_pwr_scale = scale;
	return ret;
}

/* Function Name:
 *      rtk_ponmac_serdesCdr_reset
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
rtk_ponmac_serdesCdr_reset(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_serdesCdr_reset)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_serdesCdr_reset();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_serdesCdr_reset */

/* Function Name:
 *      rtk_ponmac_linkState_get
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
rtk_ponmac_linkState_get(rtk_ponmac_mode_t mode, uint32 *pSd, uint32 *pSync)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_linkState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_linkState_get(mode, pSd, pSync);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_linkState_get */

/* Function Name:
 *      rtk_ponmac_forceBerNotify_set
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
rtk_ponmac_forceBerNotify_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_forceBerNotify_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_forceBerNotify_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_linkState_get */

/* Function Name:
 *      rtk_ponmac_bwthreshold_set
 * Description:
 *      Set PON Port threshold
 * Input:
 *      bwThreshold    - PON BW_THRESHOLD
 *      reqBwThreshold - PON REQ_BW_THRESHOLD
 * Output:
 *      N/A
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_bwThreshold_set(uint32 bwThreshold, uint32 reqBwThreshold)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_bwThreshold_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_bwThreshold_set(bwThreshold, reqBwThreshold);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_bwThreshold_set */

/* Function Name:
 *      rtk_ponmac_bwThreshold_get
 * Description:
 *      Get PON Port threshold
 * Input:
 *      N/A
 * Output:
 *      pBwThreshold    - PON BW_THRESHOLD
 *      pReqBwThreshold - PON REQ_BW_THRESHOLD
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_bwThreshold_get(uint32 *pBwThreshold, uint32 *pReqBwThreshold)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_bwThreshold_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_bwThreshold_get(pBwThreshold, pReqBwThreshold);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_linkState_get */

/* Function Name:
 *      rtk_ponmac_maxPktLen_set
 * Description:
 *      set pon port max packet length
 * Input:
 *      length       - max accept packet length
 *    state          - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                                   - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None
 */
int32 rtk_ponmac_maxPktLen_set(uint32 length)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_maxPktLen_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_maxPktLen_set(length);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_maxPktLen_set */
/* Function Name:
 *      rtk_ponmac_sidValid_get
 * Description:
 *      Get sid valid
 * Input:
 *      sid     - sid
 * Output:
 *      pValid  - the current sid valid
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_ponmac_sidValid_get(uint32 sid, uint32 *pValid)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_sidValid_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_sidValid_get(sid, pValid);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_sidValid_get */

/* Function Name:
 *      rtk_ponmac_sidValid_set
 * Description:
 *      Set sid valit
 * Input:
 *      sid     - sid
 *      valid   - valid or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_sidValid_set(uint32 sid, uint32 valid)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_sidValid_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_sidValid_set(sid, valid);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_sidValid_set */


/* Function Name:
 *      rtk_ponmac_schedulingType_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      pQueueType  - the WFQ schedule type (RTK_QOS_WFQ or RTK_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_ponmac_schedulingType_get(rtk_qos_scheduling_type_t *pQueueType)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_schedulingType_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_schedulingType_get(pQueueType);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_schedulingType_get */


/* Function Name:
 *      rtk_ponmac_schedulingType_set
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      queueType  - the WFQ schedule type (RTK_QOS_WFQ or RTK_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_ponmac_schedulingType_set(rtk_qos_scheduling_type_t queueType)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_schedulingType_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_schedulingType_set(queueType);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_schedulingType_set */


/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlRate_get
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
int32
rtk_ponmac_egrBandwidthCtrlRate_get(uint32 *pRate)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrBandwidthCtrlRate_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrBandwidthCtrlRate_get(pRate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_egrBandwidthCtrlRate_get */

/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlRate_set
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
int32
rtk_ponmac_egrBandwidthCtrlRate_set(uint32 rate)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrBandwidthCtrlRate_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrBandwidthCtrlRate_set(rate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_egrBandwidthCtrlRate_set */



/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlRate_get
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
rtk_ponmac_egrScheduleIdRate_get(uint32 scheduleId,uint32 *pRate)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrScheduleIdRate_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrScheduleIdRate_get(scheduleId, pRate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_egrScheduleIdRate_get */





/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlRate_set
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
rtk_ponmac_egrScheduleIdRate_set(uint32 scheduleId,uint32 rate)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrScheduleIdRate_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrScheduleIdRate_set(scheduleId, rate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_egrScheduleIdRate_set */

/* Function Name:
 *      rtk_ponmac_txPll_relock
 * Description:
 *      Re-lock tx PLL by toggle CMU_EN_GPHY.
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
rtk_ponmac_txPll_relock(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_txPll_relock)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_txPll_relock();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_txPll_relock */

/* Function Name:
 *      rtk_ponmac_synce_get
 * Description:
 *      Get the current optical synce state
 * Input:
 *      None
 * Output:
 *      enable  - the current optical synce state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_ponmac_synce_get(rtk_enable_t *enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_synce_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_synce_get(enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_losState_get */

/* Function Name:
 *      rtk_ponmac_synce_set
 * Description:
 *      Set the current optical synce state
 * Input:
 *      enable  - the current optical synce state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_synce_set(rtk_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_synce_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_synce_set(enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_linkState_get */


/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlIncludeIfg_get
 * Description:
 *      get the pon port egress bandwidth control IFG state.
 * Input:
 *      None
 * Output:
 *      pIfgInclude - pointer of state of IFG 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_egrBandwidthCtrlIncludeIfg_get(rtk_enable_t *pIfgInclude)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrBandwidthCtrlIncludeIfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrBandwidthCtrlIncludeIfg_get(pIfgInclude);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_egrBandwidthCtrlIncludeIfg_get */


/* Function Name:
 *      rtk_ponmac_egrBandwidthCtrlIncludeIfg_set
 * Description:
 *      set the pon port egress bandwidth control IFG state.
 * Input:
 *      ifgInclude - state of IFG 
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_egrBandwidthCtrlIncludeIfg_set(rtk_enable_t ifgInclude)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrBandwidthCtrlIncludeIfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrBandwidthCtrlIncludeIfg_set(ifgInclude);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_egrBandwidthCtrlIncludeIfg_set */


/* Function Name:
 *      rtk_ponmac_egrScheduleIdIncludeIfg_get
 * Description:
 *      get the tcont egress bandwidth control IFG state.
 * Input:
 *      None
 * Output:
 *      pIfgInclude - pointer of state of IFG 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_egrScheduleIdIncludeIfg_get(rtk_enable_t *pIfgInclude)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrScheduleIdIncludeIfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrScheduleIdIncludeIfg_get(pIfgInclude);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_egrScheduleIdIncludeIfg_get */


/* Function Name:
 *      rtk_ponmac_egrScheduleIdIncludeIfg_set
 * Description:
 *      set the tcont egress bandwidth control IFG state.
 * Input:
 *      ifgInclude - state of IFG 
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_egrScheduleIdIncludeIfg_set(rtk_enable_t ifgInclude)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrScheduleIdIncludeIfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrScheduleIdIncludeIfg_set(ifgInclude);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_egrScheduleIdIncludeIfg_set */

/* Function Name:
 *      rtk_ponmac_txDisGpioId_get
 * Description:
 *      Get PON TxDis GPIO PIN id
 * Input:
 *      N/A
 * Output:
 *      gpioPinId    - gpio pin id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_txDisGpioId_get(uint32 *gpioPinId)
{

#if defined(CONFIG_TX_DISABLE_GPIO_PIN)
    *gpioPinId = CONFIG_TX_DISABLE_GPIO_PIN;
    return RT_ERR_OK;
#else
    *gpioPinId=0;
    return RT_ERR_FAILED;
#endif

}   /* end of rtk_ponmac_txDisGpioId_get */


/* Function Name:
 *      rtk_ponmac_txPowerGpioId_get
 * Description:
 *      Get PON Tx Power GPIO PIN id
 * Input:
 *      N/A
 * Output:
 *      gpioPinId    - gpio pin id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_txPowerGpioId_get(uint32 *gpioPinId)
{
#if defined(CONFIG_TX_POWER_GPO_PIN)
    *gpioPinId = CONFIG_TX_POWER_GPO_PIN;
    return RT_ERR_OK;
#else
    *gpioPinId=0;
    return RT_ERR_FAILED;
#endif
}   /* end of rtk_ponmac_txPowerGpioId_get */


/* Function Name:
 *      rtk_ponmac_aggTcontState_get
 * Description:
 *      get the aggregate tcont state.
 * Input:
 *      aggIndex  - aggregate tcont index
 * Output:
 *      pState - pointer of state of aggregate tcont 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_aggTcontState_get(rtk_ponmac_aggregate_tcont_t aggIndex, rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_aggTcontState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_aggTcontState_get(aggIndex, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_aggTcontState_get */


/* Function Name:
 *      rtk_ponmac_aggTcontState_set
 * Description:
 *      set the aggregate tcont state.
 * Input:
 *      aggIndex - aggregate tcont index
 *      state - state of aggregate tcont
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_aggTcontState_set(rtk_ponmac_aggregate_tcont_t aggIndex, rtk_enable_t state)
{
    int32   ret;
    uint32  len, tcont, q;
    rtk_ponmac_queue_t q_idx;
    rtk_ponmac_queueCfg_t q_cfg;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_aggTcontState_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    if(aggIndex >= PONMAC_AGG_TCONT_END || aggIndex < 0)
        return RT_ERR_INPUT;

    /* set PON port rx max length 0 */
    if((ret = rtk_switch_maxPktLenByPort_get(HAL_GET_PON_PORT(), &len)) != RT_ERR_OK)
    {
        osal_printf("rtk_switch_maxPktLenByPort_get pon, ret=0x%x\n", ret);
        return ret;
    }

    if((ret = rtk_switch_maxPktLenByPort_set(HAL_GET_PON_PORT(), 0)) != RT_ERR_OK)
    {
        osal_printf("rtk_switch_maxPktLenByPort_set pon len=0, ret=0x%x\n", ret);
        return ret;
    }

    /* drain out tcont queue 0-31 */
    for(tcont=(aggIndex*4); tcont<(aggIndex*4+4); tcont++)
    {
        for(q=0; q<7; q++)
        {   
            q_idx.schedulerId = tcont;
            q_idx.queueId = q;
            if((ret = rtk_ponmac_queueDrainOut_set(&q_idx)) != RT_ERR_OK)
            {
                osal_printf("rtk_ponmac_queueDrainOut_set tcont=%d, queue=%d, ret=0x%x\n",tcont, q, ret);
            }
        }
    }

#if defined(CONFIG_GPON_FEATURE)
#ifndef CONFIG_LUNA_G3_SERIES
    if((ret = rtk_gponapp_aggTcont_clear(aggIndex)) != RT_ERR_OK)
    {
        osal_printf("rtk_gponapp_aggTcont_clear idx=%d, ret=0x%x\n",aggIndex, ret);
    }
#endif
#endif

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_aggTcontState_set(aggIndex, state);
    RTK_API_UNLOCK();


    if(state == ENABLED)
    {/* add tcont queue for queue 0-31 */
        for(tcont=(aggIndex*4); tcont<(aggIndex*4+4); tcont++)
        {
            for(q=0; q<8; q++)
            {   
                q_idx.schedulerId = tcont;
                q_idx.queueId = q;

                q_cfg.cir = 0x0;
                q_cfg.pir = 0x3ffff;
                q_cfg.type = STRICT_PRIORITY;
                q_cfg.weight = 0;
                q_cfg.egrssDrop = DISABLED;

                if((ret = rtk_ponmac_queue_add(&q_idx, &q_cfg)) != RT_ERR_OK)
                {
                    osal_printf("rtk_ponmac_queue_add tcont=%d, queue=%d, ret=0x%x\n",tcont, q, ret);
                }
            }
        }
    }
    else
    {/* del tcont queue for queue 0-31 */
        for(tcont=(aggIndex*4); tcont<(aggIndex*4+4); tcont++)
        {
            for(q=0; q<8; q++)
            {   
                q_idx.schedulerId = tcont;
                q_idx.queueId = q;
                if((ret = rtk_ponmac_queue_del(&q_idx)) != RT_ERR_OK)
                {
                    osal_printf("rtk_ponmac_queue_del tcont=%d, queue=%d, ret=0x%x\n",tcont, q, ret);
                }
            }
        }
    }

    /* recover PON port rx max length */
    if((ret = rtk_switch_maxPktLenByPort_set(HAL_GET_PON_PORT(), len)) != RT_ERR_OK)
    {
        osal_printf("rtk_switch_maxPktLenByPort_set pon len=%d, ret=0x%x\n", len, ret);
    }

    return ret;
}   /* end of rtk_ponmac_aggTcontState_set */

/* Function Name:
 *      rtk_ponmac_txDisableGpio_get
 * Description:
 *      get the pon module tx disable state.
 * Input:
 *      aggIndex  - aggregate tcont index
 * Output:
 *      pEnable - pointer of state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_txDisableGpio_get(rtk_enable_t *pEnable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_txDisableGpio_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_txDisableGpio_get(pEnable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_txDisableGpio_get */

/* Function Name:
 *      rtk_ponmac_txDisableGpio_set
 * Description:
 *      set the pon module tx disable state.
 * Input:
 *      enable - state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_txDisableGpio_set(rtk_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_txDisableGpio_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_txDisableGpio_set(enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_txDisableGpio_set */

/* Function Name:
 *      rtk_ponmac_txPowerDisableGpio_get
 * Description:
 *      get the pon module tx power disable state.
 * Input:
 *      aggIndex  - aggregate tcont index
 * Output:
 *      pEnable - pointer of state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_txPowerDisableGpio_get(rtk_enable_t *pEnable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_txPowerDisableGpio_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_txPowerDisableGpio_get(pEnable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_txPowerDisableGpio_get */

/* Function Name:
 *      rtk_ponmac_txPowerDisableGpio_set
 * Description:
 *      set the pon module tx power disable state.
 * Input:
 *      enable - state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_ponmac_txPowerDisableGpio_set(rtk_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_txPowerDisableGpio_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_txPowerDisableGpio_set(enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ponmac_txPowerDisableGpio_set */
