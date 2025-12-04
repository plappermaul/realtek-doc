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
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_ponmac.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#include <dal/apollomp/raw/apollomp_raw_ponmac.h>
#include <dal/apollomp/raw/apollomp_raw_flowctrl.h>
#include <osal/time.h>
#include <ioal/mem32.h>
#include <rtk/gpio.h>
#include <dal/apollomp/dal_apollomp_gpio.h>

/*
 * Symbol Definition
 */



static uint32    ponmac_init = INIT_NOT_COMPLETED;
static rtk_ponmac_mode_t ponMode = PONMAC_MODE_GPON;
/*
 * Macro Declaration
 */


static int32 _ponmac_physicalQueueId_get(uint32 schedulerId, uint32 logicalQId, uint32 *pPhysicalQid)
{

#if(GPON_MAC_MODE)
    *pPhysicalQid = APOLLOMP_TCONT_QUEUE_MAX *(schedulerId/8) + logicalQId;
#else
    *pPhysicalQid = APOLLOMP_TCONT_QUEUE_MAX *(schedulerId/2) + logicalQId;
#endif

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


#if(GPON_MAC_MODE)
    maxSchedhlerId = 8;
    schedhlerBase = (physicalQid/32) * 8;
#else
    maxSchedhlerId = 2;
    schedhlerBase = (physicalQid/32) * 2;
#endif


    for(i = schedhlerBase; i<schedhlerBase+maxSchedhlerId ; i++ )
    {
        if ((ret = apollomp_raw_ponMacScheQmap_get(i, &qMask)) != RT_ERR_OK)
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


/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */


/* Function Name:
 *      dal_apollomp_ponmac_init
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
dal_apollomp_ponmac_init(void)
{
    int32   ret;
    uint32 tcont,wData;
    uint32 physicalQid;
    uint32 wfqTbValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

#ifdef FPGA_DEFINED
    /*fpga do not have APOLLOMP_WSDS_ANA_24*/
#else /*FPGA_DEFINED*/

#ifdef CONFIG_BEN_DIFFERENTIAL
    wData = 0;
    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_BEN_TTL_OUTf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    wData = 1;
    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_18r, APOLLOMP_REG_BEN_SEL_CMLf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#else /*CONFIG_BEN_DIFFERENTIAL*/
    wData = 1;
    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_BEN_TTL_OUTf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif /*CONFIG_BEN_DIFFERENTIAL*/
#endif /*FPGA_DEFINED*/

    /*init PON BW_THRESHOLD*/
    wData = 21;
    if ((ret = reg_field_write(APOLLOMP_PON_PORT_CTRLr, APOLLOMP_BW_THRESHOLDf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*init PON REQ_BW_THRESHOLD*/
    wData = 14;
    if ((ret = reg_field_write(APOLLOMP_PON_PORT_CTRLr, APOLLOMP_REQ_BW_THRESHOLDf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    wData = 1;
    if ((ret = reg_field_write(APOLLOMP_PON_CFGr, APOLLOMP_PIR_EXCEED_DROPf, &wData)) != RT_ERR_OK)
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

        for(schedulerId = 0 ; schedulerId < HAL_MAX_NUM_OF_GPON_TCONT() ; schedulerId++ )
        {
            memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));
            logicalQueue.schedulerId = schedulerId;

            for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ )
            {
                logicalQueue.queueId = queueId;

                if((ret= dal_apollomp_ponmac_queue_get(&logicalQueue, &queueCfg)) == RT_ERR_OK)
                {
                    /*delete queue*/
                    if((ret= dal_apollomp_ponmac_queue_del(&logicalQueue)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                        return ret;
                    }
                }
            }
        }
    }


    for(tcont = 0; tcont <HAL_MAX_NUM_OF_GPON_TCONT(); tcont++ )
    {
        /*disable T-cont schedule */
        if ((ret = apollomp_raw_ponMacTcontEnable_set(tcont, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /*clear T-cont schedule mask*/
        if ((ret = apollomp_raw_ponMacScheQmap_set(tcont, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }



    /*set all queue to strict, disable CIR/PIR and disable egress drop*/

    for(physicalQid=0 ; physicalQid<HAL_MAX_NUM_OF_PON_QUEUE() ; physicalQid++)
    {
        /*set queue schedule type*/
        if ((ret = apollomp_raw_ponMac_queueType_set(physicalQid, APOLLOMP_QOS_QUEUE_STRICT)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*set PIR CIR*/
        if ((ret = apollomp_raw_ponMacCirRate_set(physicalQid, 0x0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        if ((ret = apollomp_raw_ponMacPirRate_set(physicalQid, HAL_PONMAC_PIR_CIR_RATE_MAX())) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }


        /*set egress drop*/
        if ((ret = apollomp_raw_ponMac_egrDropState_set(physicalQid, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*set default weight to 1*/
        if ((ret = apollomp_raw_ponMac_wfqWeight_set(physicalQid, 0x1)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

#if !defined(CONFIG_GPON_FEATURE) && !defined(CONFIG_EPON_FEATURE) && !defined(CONFIG_FIBER_FEATURE)
    /* For ether wan */
    /*mapping queue 0~7 to schedule id 0*/
    {
        rtk_ponmac_queueCfg_t   queueCfg;
        rtk_ponmac_queue_t logicalQueue;
        uint32 queueId;
        uint32 flowId;

        memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));

        logicalQueue.schedulerId = 0 ;

        queueCfg.cir       = 0x0;
        queueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
        queueCfg.type      = STRICT_PRIORITY;
        queueCfg.egrssDrop = DISABLED;
        ponmac_init = INIT_COMPLETED;
        for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ )
        {
            logicalQueue.queueId = queueId;

            if((ret= dal_apollomp_ponmac_queue_add(&logicalQueue, &queueCfg)) != RT_ERR_OK)
            {
                ponmac_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }

    /*mapping all flow to t-cont 0 queue 0*/
        logicalQueue.schedulerId = 0;
        logicalQueue.queueId     = 0;

        ponmac_init = INIT_COMPLETED;
        for(flowId=0 ; flowId<HAL_CLASSIFY_SID_NUM() ; flowId++)
        {
            if((ret= dal_apollomp_ponmac_flow2Queue_set(flowId, &logicalQueue)) != RT_ERR_OK)
            {
                ponmac_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }
#endif

    /*configure WFQ specific tb value on RSVD_EGR_SCH to 0xff*/
   wfqTbValue = 0xff;
   if((ret = reg_field_write(APOLLOMP_RSVD_EGR_SCH2r, APOLLOMP_PON_WFQ_LEAKY_BUCKETf, &wfqTbValue))!=RT_ERR_OK)
   {
       ponmac_init = INIT_NOT_COMPLETED;
       return ret;
   }

    ponmac_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_init */


/* Function Name:
 *      dal_apollomp_ponmac_queue_add
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
dal_apollomp_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
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
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    RT_PARAM_CHK((pQueueCfg->type >= QUEUE_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueueCfg->egrssDrop >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* add queue to t-cont schedule mask*/
    if ((ret = apollomp_raw_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(qMask == 0)
    {/*first queue add to this t-cont enable t-cont schedule*/
        if ((ret = apollomp_raw_ponMacTcontEnable_set(pQueue->schedulerId, ENABLED)) != RT_ERR_OK)
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
        if ((ret = apollomp_raw_ponMacQueueDrainOutState_set(physicalQid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /*add queue to qMask*/
        qMask = qMask | (1<<pQueue->queueId);
        if ((ret = apollomp_raw_ponMacScheQmap_set(pQueue->schedulerId, qMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    
    /*set queue schedule type*/
    if(pQueueCfg->type == STRICT_PRIORITY)
    {
        if ((ret = apollomp_raw_ponMac_queueType_set(physicalQid, APOLLOMP_QOS_QUEUE_STRICT)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = apollomp_raw_ponMac_queueType_set(physicalQid, APOLLOMP_QOS_QUEUE_WFQ)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /*set wfq weight*/
#if 0
        if ((ret = apollomp_raw_ponMac_wfqWeight_set(physicalQid, pQueueCfg->weight)) != RT_ERR_OK)
#else
        if ((ret = apollomp_raw_ponMac_wfqWeight_set(pQueue->queueId, pQueueCfg->weight)) != RT_ERR_OK)
#endif
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }


    /*set PIR CIR*/
    if ((ret = apollomp_raw_ponMacCirRate_set(physicalQid, pQueueCfg->cir)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = apollomp_raw_ponMacPirRate_set(physicalQid, pQueueCfg->pir)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    /*set egress drop*/
    if ((ret = apollomp_raw_ponMac_egrDropState_set(physicalQid, pQueueCfg->egrssDrop)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_queue_add */



/* Function Name:
 *      dal_apollomp_ponmac_queue_get
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
dal_apollomp_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    rtk_enable_t   enable;
    uint32  qMask;
    uint32  physicalQid;
    apollomp_raw_queue_type_t  queueType;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if ((ret = apollomp_raw_ponMacTcontEnable_get(pQueue->schedulerId, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(DISABLED==enable)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = apollomp_raw_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(!(qMask & 1<<(pQueue->queueId)))
        return RT_ERR_ENTRY_NOTFOUND;

    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    if ((ret = apollomp_raw_ponMac_queueType_get(physicalQid, &queueType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if(APOLLOMP_QOS_QUEUE_STRICT == queueType)
        pQueueCfg->type = STRICT_PRIORITY;
    else
        pQueueCfg->type = WFQ_WRR_PRIORITY;

    /*get wfq weight*/
#if 0
    if ((ret = apollomp_raw_ponMac_wfqWeight_get(physicalQid, &(pQueueCfg->weight))) != RT_ERR_OK)
#else
    if ((ret = apollomp_raw_ponMac_wfqWeight_get(pQueue->queueId, &(pQueueCfg->weight))) != RT_ERR_OK)
#endif
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*set PIR CIR*/
    if ((ret = apollomp_raw_ponMacCirRate_get(physicalQid, &(pQueueCfg->cir))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = apollomp_raw_ponMacPirRate_get(physicalQid, &(pQueueCfg->pir))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    /*set egress drop*/
    if ((ret = apollomp_raw_ponMac_egrDropState_get(physicalQid, &(pQueueCfg->egrssDrop))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_queue_get */


/* Function Name:
 *      dal_apollomp_ponmac_queue_del
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
dal_apollomp_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
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
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);


    if ((ret = apollomp_raw_ponMacTcontEnable_get(pQueue->schedulerId, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(DISABLED==enable)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = apollomp_raw_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(!(qMask & 1<<(pQueue->queueId)))
        return RT_ERR_ENTRY_NOTFOUND;

    /* change sid to default queue 127 before drain out */
    for(sid=0; sid<HAL_CLASSIFY_SID_NUM(); sid++)
    {
        if((ret = dal_apollomp_ponmac_flow2Queue_get(sid, &tmpQ))==RT_ERR_OK)
        {
            if((tmpQ.schedulerId == pQueue->schedulerId) && (tmpQ.queueId == pQueue->queueId))
            {
                tmpQ.schedulerId = 31;
                tmpQ.queueId = 31;
                if((ret = dal_apollomp_ponmac_flow2Queue_set(sid, &tmpQ))!=RT_ERR_OK)
                    osal_printf("set to default queue fail! ret=0x%x\n",ret);
            }
        }
        else
            osal_printf("get to sid2queue fail! ret=0x%x\n",ret);
        
    }

    /*drant out queue*/
    queueId = ((pQueue->schedulerId)/8) * 32 + pQueue->queueId;
    if ((ret = apollomp_raw_ponMacQueueDrainOutState_set(queueId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*remove queue from qMask*/
    qMask = qMask & (~(1<<pQueue->queueId));
    if ((ret = apollomp_raw_ponMacScheQmap_set(pQueue->schedulerId, qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    if(0 == qMask)
    {/*no queue map to this t-cont disable schedule for this t-cont*/
        if ((ret = apollomp_raw_ponMacTcontEnable_set(pQueue->schedulerId, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_queue_del */



/* Function Name:
 *      dal_apollomp_ponmac_flow2Queue_set
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
dal_apollomp_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue)
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
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    if ((ret = apollomp_raw_ponMacSidToQueueMap_set(flow, physicalQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_flow2Queue_set */


/* Function Name:
 *      dal_apollomp_ponmac_flow2Queue_get
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
dal_apollomp_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  physicalQid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() <= flow), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_ponMacSidToQueueMap_get(flow, &physicalQid)) != RT_ERR_OK)
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
} /* end of dal_apollomp_ponmac_flow2Queue_get */

/* Function Name:
 *      dal_apollomp_ponmac_maxPktLen_set
 * Description:
 *      set pon port max packet length
 * Input:
 *      length         - max accept packet length
 *	  state 	     - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 * Note:
 *      None
 */
int32 dal_apollomp_ponmac_maxPktLen_set(uint32 length)
{
    int32  ret;
    
    if ((ret = dal_apollomp_switch_maxPktLenByPort_set(HAL_GET_PON_PORT(), length)) != RT_ERR_OK)
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
    if ((ret = dal_apollomp_gpio_mode_set(CONFIG_TX_POWER_GPO_PIN, GPIO_OUTPUT)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if ((ret = dal_apollomp_gpio_state_set(CONFIG_TX_POWER_GPO_PIN, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if(enable == ENABLED)
    {
        if ((ret = dal_apollomp_gpio_databit_set(CONFIG_TX_POWER_GPO_PIN,0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = dal_apollomp_gpio_databit_set(CONFIG_TX_POWER_GPO_PIN,1)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;

}
#endif

/* Function Name:
 *      dal_apollomp_ponmac_mode_set
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
dal_apollomp_ponmac_mode_set(rtk_ponmac_mode_t mode)
{
    int32 ret;
    uint32 data;
	uint32 bound;
    uint32 flowId;
    rtk_ponmac_queueCfg_t   queueCfg;
    rtk_ponmac_queue_t logicalQueue;
    uint32 queueId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /*RT_PARAM_CHK((PONMAC_MODE_END <= mode), RT_ERR_INPUT);*/

    /* Move from ponmac init */
    /*mapping queue 0~7 to schedule id 0*/
    if(mode != PONMAC_MODE_GPON)
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

            if((ret= dal_apollomp_ponmac_queue_add(&logicalQueue, &queueCfg)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
        }
    }

    if(mode == PONMAC_MODE_GPON)
    {
    /* mapping all flow to t-cont 31 queue 31 */
        logicalQueue.schedulerId = 31;
        logicalQueue.queueId     = 31;
    }
    else
    {
        logicalQueue.schedulerId = 0;
        logicalQueue.queueId     = 0;
    }
    for(flowId=0 ; flowId<HAL_CLASSIFY_SID_NUM() ; flowId++)
    {
        if((ret= dal_apollomp_ponmac_flow2Queue_set(flowId, &logicalQueue)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
    }

    data = 0x0;
    /* PON MAC select phy 4 enable/disable */
    if ((ret = reg_field_write(APOLLOMP_CHIP_INF_SELr, APOLLOMP_PHY4_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
        return ret;
    }

	data = 0xb;
	if ((ret = reg_field_write(APOLLOMP_BOND_INFOr, APOLLOMP_BOND_INFO_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

	if ((ret = reg_read(APOLLOMP_BOND_INFOr, &bound)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    /*change pon mode must foce be to laser off*/
    /* set ben force mode to 0 */

    data = 0;
    if ((ret = reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_VALUEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }
    data = 1;
    if ((ret = reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }
    data = 1;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_CFG_FRC_BENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }


    switch(mode)
    {
        case PONMAC_MODE_GPON:
        {
            /* set RX=IDLE, for CDR recovery clock using local ring PLL */
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_RX_SEL_SDf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* set the GPON mode enable */
            data = 0;
            if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_EPON_ENf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 1;
            if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_GPON_ENf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* set the serdes mode to GPON mode */
            data = 0x8;
            if ((ret = reg_field_write(APOLLOMP_SDS_CFGr, APOLLOMP_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* WSDS_ANA_1A.REG_RX_KP2_2 = 6 */
            data = 0x6;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_RX_KP2_2f, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* reset serdes including digital and analog */
            /* no force sds */
            data = 0x0;
            if ((ret = reg_write(APOLLOMP_WSDS_DIG_01r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* reset serdes including digital and analog, and GPON MAC */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr, APOLLOMP_CMD_SDS_RST_PSf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            osal_time_mdelay(10);

            /* force sds for not reset GPON MAC when SD down */
            data = 0xc;
            if ((ret = reg_write(APOLLOMP_WSDS_DIG_01r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 0x0;
            if ((ret = reg_write(APOLLOMP_WSDS_ANA_1Cr, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* Serdes amplitude 500 mv */
            data = 0x1164;
            if ((ret = reg_write(APOLLOMP_WSDS_ANA_0Er, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* BEN on */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_BEN_OEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* pon port allow undersize packet */
            data = 1;
            if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, APOLLOMP_RX_SPCf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
             /* turn off 'serdes_tx_dis when ben=0' */
            data = 0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_06r, APOLLOMP_CFG_DMY1f, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /*WSDS_ANA_10, TX output impedance=100 ohm, 20130523 by Arshian.*/
            data = 0x11B8;
            if ((ret = reg_write(APOLLOMP_WSDS_ANA_10r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            if((bound & 0xf000) >= 0x1000)
            {
                /* adjust for sync PCM */
                data = 77;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_DIVN_GPHY_REFf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                data = 1;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_07r, APOLLOMP_REG_CMU_SEL_FREFf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* modify CMU_N_PLL_TX to 0x24 for tune jitter for 3X BW */
                data = 0x24;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_22r, APOLLOMP_REG_CMU_N_PLL_TXf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                
                /* modify REG_SDS_ANA_RSV to 0x83 for tune jitter for 3X BW */
                if ((ret = reg_field_read(APOLLOMP_WSDS_ANA_23r, APOLLOMP_REG_SDS_ANA_RSVf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                data &= 0xff00;
                data |= 0x83;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_23r, APOLLOMP_REG_SDS_ANA_RSVf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                data = 1;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_PREDIV_BYPASS_TXf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* end of adjust for sync PCM */

                /* TX jitter */
                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_16r, APOLLOMP_REG_CMU_PREDIV_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                data = 0x4D;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_DIVN_GPHY_REFf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                data = 0x19;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_22r, APOLLOMP_REG_CMU_N_PLL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                data = 0x143;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_21r, APOLLOMP_REG_CMU_LOCK_UP_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                data = 0x143;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_20r, APOLLOMP_REG_CMU_LOCK_DN_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_18r, APOLLOMP_REG_TX_CLK_SELf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                data = 0x7;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_23r, APOLLOMP_REG_CMU_SEL_R_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                data = 0x3;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_25r, APOLLOMP_REG_CMU_LC_BUF_SEL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                data = 0x3;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Fr, APOLLOMP_REG_CMU_ISTANK_SEL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                data = 0xf;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Fr, APOLLOMP_REG_CMU_ICP_SEL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                /* here set RX_SEL_SD=0 to use OOBS, it use internal clock when do tx lock PLL. */
                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_RX_SEL_SDf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* use internal clock */
                data = 0x2;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_FREF_SEL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* set up limit */
                data = 0x18e;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_21r, APOLLOMP_REG_CMU_LOCK_UP_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* set down limit */
                data = 0x18e;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_20r, APOLLOMP_REG_CMU_LOCK_DN_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                osal_time_udelay(100);
                /* tx lock PLL */
                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                data = 0x1;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                osal_time_udelay(100);
                /* tune jitter from Eason/YuHsiang, REG_FREF_SEL_GPHY =01 */
                data = 0x1;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_FREF_SEL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* recover up limit, tune jitter from Eason/YuHsiang, REG_CMU_LOCK_UP_LIMIT_GPHY=0x1d1 */
                data = 0x1d1;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_21r, APOLLOMP_REG_CMU_LOCK_UP_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* recover down limit, tune jitter from Eason/YuHsiang, REG_CMU_LOCK_DN_LIMIT_GPHY=0x1d1 */
                data = 0x1d1;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_20r, APOLLOMP_REG_CMU_LOCK_DN_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* tune jitter from Eason/YuHsiang, REG_CMU_DIVIDE_NUM_GPHY=0x3E */
                data = 0x3E;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Er, APOLLOMP_REG_CMU_DIVIDE_NUM_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* here set RX_SEL_SD=1 to use RX_SD */
                data = 0x1;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_RX_SEL_SDf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* tune jitter from Eason/YuHsiang, REG_DIVN_GPHY_REF=0x1a for 3X BW */
                data = 0x1a;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_DIVN_GPHY_REFf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* tune jitter from Eason/YuHsiang, REG_CMU_N_PLL_GPHY=0x8 for 3X BW */
                data = 0x8;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_22r, APOLLOMP_REG_CMU_N_PLL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* tune jitter from Eason/YuHsiang, REG_CMU_POSTDIV_GPHY_25M=0x17 for 3X BW */
                data = 0x17;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_17r, APOLLOMP_REG_CMU_POSTDIV_GPHY_25Mf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* if CMU_EN_TX=1, toggle CMU_EN_TX */
                if ((ret = reg_field_read(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_CMU_EN_TXf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                if(data == 1)
                {
                    data = 0x0;
                    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_CMU_EN_TXf, &data)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                        return ret;
                    }
                    data = 0x1;
                    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_CMU_EN_TXf, &data)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                        return ret;
                    }	
                }
                /* end of TX jitter */
            }

            /* Update the flow control thresholds */
            if ((ret = apollomp_raw_flowctrl_ponThreshold_set()) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* set max packet len to 2031 (2047-4(ctag)-4(stag)-8(pppoe)=2031) */
            if((ret = dal_apollomp_ponmac_maxPktLen_set(2031)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

#ifdef CONFIG_TX_POWER_TURN_ON_FEATURE
            /* turn on tx power */
            if((ret =_ponmac_txPower_enable(ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
#endif
            /* set dying gasp pin out */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_DYNGASP_CTRLr,APOLLOMP_DYNGASP_OUT_ENf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_DYNGASP_CTRLr,APOLLOMP_DYNGASP_OUT_INVf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }

            /* serdes fifo reset for fifo r/w pointer issue */
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_1Dr,APOLLOMP_CFG_SFT_RSTB_INFf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_1Dr,APOLLOMP_CFG_SFT_RSTB_INFf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            }

            osal_time_mdelay(10);
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr, APOLLOMP_SW_RSTf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
        }
        break;

        case PONMAC_MODE_EPON:
        {
            rtk_port_macAbility_t portAbility;
            
            /*Link down pon port first*/
    		if ((ret = rtk_port_macForceAbility_get(4, &portAbility)) != RT_ERR_OK)
    		{
    		    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    		    return ret;
    		}
    	    portAbility.linkStatus = PORT_LINKDOWN;
    		if ((ret = rtk_port_macForceAbility_set(4, portAbility)) != RT_ERR_OK)
    		{
    		    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
    		    return ret;
    		}
            data = 0;
            if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_GPON_ENf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 1;
            if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_EPON_ENf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* set the serdes mode to EPON mode */
            data = 0xc;
            if ((ret = reg_field_write(APOLLOMP_SDS_CFGr, APOLLOMP_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* change mode to EPON, must reset switch */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr, APOLLOMP_SW_RSTf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            osal_time_mdelay(10);


            /* no force sds */
            data = 0x0;
            if ((ret = reg_write(APOLLOMP_WSDS_DIG_01r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

             /* force sds for not reset GPON MAC when SD down */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr, APOLLOMP_CMD_SDS_RST_PSf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            osal_time_mdelay(10);

            /* force sds for not reset GPON MAC when SD down */
            data = 0xc;
            if ((ret = reg_write(APOLLOMP_WSDS_DIG_01r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* Serdes amplitude 500 mv */
            data = 0x1164;
            if ((ret = reg_write(APOLLOMP_WSDS_ANA_0Er, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* set N code = 0x60 */
            data = 0x60;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_0Cr, APOLLOMP_CFG_N_CODE_RX_25Mf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* set F code = 0x0*/
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_0Ar, APOLLOMP_CFG_F_CODE_RX_25Mf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* reset serdes including digital and analog */
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r, APOLLOMP_CFG_SFT_RSTBf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_00r, APOLLOMP_CFG_SFT_RSTBf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            osal_time_mdelay(10);

            /* BEN on */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_BEN_OEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /*serdes patch*/
            /* WSDS_ANA_18.REG_RX_KP1_2 = 3 */
            data = 0x3;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_18r, APOLLOMP_REG_RX_KP1_2f, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* WSDS_ANA_1A.REG_RX_KP2_2 = 4 */
            data = 0x4;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_RX_KP2_2f, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* SDS_REG1.SDS_FRC_TX = 0 */
            data = 0;
            if ((ret = reg_field_write(APOLLOMP_SDS_REG1r, APOLLOMP_SDS_FRC_RXf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /*PCS derdes patch*/
            data = 0x3;
            if ((ret = reg_write(APOLLOMP_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /*data = 0x4e6a;*/
            data = 0x4eff;
            if ((ret = reg_write(APOLLOMP_SDS_EXT_REG13r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /*data = 0x1562;*/
            data = 0x1400;
            if ((ret = reg_write(APOLLOMP_SDS_EXT_REG15r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 0xbd2a;
            if ((ret = reg_write(APOLLOMP_SDS_EXT_REG16r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            data = 0x1059;
            if ((ret = reg_write(APOLLOMP_SDS_REG7r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            data = 0x3F;
            if ((ret = reg_field_write(APOLLOMP_SDS_REG24r,APOLLOMP_CFG_FIB2G_TXCFG_NP_P0f,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* byapss F code */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_08r, APOLLOMP_CFG_BYPASS_PI_RXf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* byapss F code */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_CMU_BYPASS_PI_RXf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* use RX 1.25G */
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_SPDSELf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            if((bound & 0xf000) >= 0x1000)
            {
                /*WSDS_ANA_25.REG_CMU_LC_BUF_SEL_GPHY = 0x3*/
                data = 0x3;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_25r, APOLLOMP_REG_CMU_LC_BUF_SEL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;

                }
                /*WSDS_ANA_18.REG_TX_CLK_SEL = 0, use option 2*/
                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_18r, APOLLOMP_REG_TX_CLK_SELf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /*WSDS_ANA_18.RX_KP1_2 = 4*/
                data = 0x4;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_18r, APOLLOMP_REG_RX_KP1_2f, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /*WSDS_ANA_18.REG_CMU_TX_OFF = 0*/
                /*#reg 0x22060 [7]=1b'0    */
                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_18r, APOLLOMP_REG_CMU_TX_OFFf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                /*WSDS_ANA_1A.REG_DIVN_GPHY_REF= 0x95*/
                /*#reg 0x22068 [12:5]= 8'b10010101*/
                /*Reg 0x22068 = 0x0000D2A4         */
                data = 0x95;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_DIVN_GPHY_REFf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                /*WSDS_ANA_22.REG_CMU_N_PLL_GPHY= 0x31*/
                /*#reg 0x22088 [17:10]=8'b00110001 */
                data = 0x31;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_22r, APOLLOMP_REG_CMU_N_PLL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /*WSDS_ANA_22.REG_CMU_N_PLL_TX= 0xb8*/
                data = 0xb8;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_22r, APOLLOMP_REG_CMU_N_PLL_TXf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                /*WSDS_ANA_16.REG_CMU_PREDIV_GPHY= 0x0*/
                /*WSDS_ANA_16.REG_CMU_SEL_CP_GPHY= 0x0*/
                /*reg set 0x22058  0x00000000      */
                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_16r, APOLLOMP_REG_CMU_PREDIV_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_16r, APOLLOMP_REG_CMU_SEL_CP_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                /*WSDS_ANA_20.REG_CMU_LOCK_DN_LIMIT_GPHY = 0x264*/
                /*#reg 0x22080[9:0]=10b'1001100100 */
                /*reg set 0x22080  0x264           */
                data = 0x264;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_20r, APOLLOMP_REG_CMU_LOCK_DN_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                /*WSDS_ANA_21.REG_CMU_LOCK_UP_LIMIT_GPHY = 0x264*/
                /*#reg 0x22084[9:0]=10b'1001100100 */
                /*reg set 0x22084  0x264           */
                data = 0x264;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_21r, APOLLOMP_REG_CMU_LOCK_UP_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                /*WSDS_ANA_23.REG_SDS_ANA_RSV =0x79*/
                /*#reg 0x2208C[23:16]=8'b01111001  */
                /*reg set 0x2208C  0xC0790608      */
                if ((ret = reg_field_read(APOLLOMP_WSDS_ANA_23r, APOLLOMP_REG_SDS_ANA_RSVf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                data &= 0xff00;
                data |= 0x79;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_23r, APOLLOMP_REG_SDS_ANA_RSVf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                /*WSDS_ANA_1F.REG_CMU_ISTANK_SEL_GPHY = 0x3*/
                /*#reg 0x2207C[7:6]=2'b11          */
                /*reg set 0x2207C  0xC9F0          */
                data = 0x3;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Fr, APOLLOMP_REG_CMU_ISTANK_SEL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

                /* here set RX_SEL_SD=0 to use OOBS, it use internal clock when do tx lock PLL. */
                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_RX_SEL_SDf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* use internal clock */
                data = 0x2;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_FREF_SEL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* set up limit */
                data = 0x190;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_21r, APOLLOMP_REG_CMU_LOCK_UP_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* set down limit */
                data = 0x190;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_20r, APOLLOMP_REG_CMU_LOCK_DN_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                osal_time_udelay(100);
                /* tx lock PLL */
                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                data = 0x1;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                osal_time_udelay(100);
                data = 0x0;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_24r, APOLLOMP_REG_FREF_SEL_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* recover up limit */
                data = 0x264;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_21r, APOLLOMP_REG_CMU_LOCK_UP_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* recover down limit */
                data = 0x264;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_20r, APOLLOMP_REG_CMU_LOCK_DN_LIMIT_GPHYf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* here set RX_SEL_SD=1 to use RX_SD */
                data = 0x1;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_RX_SEL_SDf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /*for voip PON clock sync to PCM clock*/
                data = 1;
                if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_PREDIV_BYPASS_TXf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }

            } /* end of TX jitter */

            /*WSDS_ANA_10*/
            data = 0x11B8;
            if ((ret = reg_write(APOLLOMP_WSDS_ANA_10r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /*WSDS_ANA_1C*/
            data = 0;
            if ((ret = reg_write(APOLLOMP_WSDS_ANA_1Cr, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* Update the flow control thresholds */
            if ((ret = apollomp_raw_flowctrl_ponThreshold_set()) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* set max packet len to 2031 (2047-4(ctag)-4(stag)-8(pppoe)=2031) */
            if((ret = dal_apollomp_ponmac_maxPktLen_set(2031)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

#ifdef CONFIG_TX_POWER_TURN_ON_FEATURE
            /* turn on tx power */
            if((ret =_ponmac_txPower_enable(ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
#endif
        }
        break;

        case PONMAC_MODE_FIBER_1G:
        case PONMAC_MODE_FIBER_100M:
        case PONMAC_MODE_FIBER_AUTO:
        case PONMAC_MODE_FIBER_FORCE_1G:
        case PONMAC_MODE_FIBER_FORCE_100M:
        {
            data = 0;
            if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_GPON_ENf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 0;
            if ((ret = reg_field_write(APOLLOMP_PON_MODE_CFGr,APOLLOMP_EPON_ENf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* =========== Port config =========== */
            /* disable EEE for port 4 */
            data = 0;
            if ((ret = reg_array_write(APOLLOMP_P_EEECFGr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
                return ret;
            }

            /* =========== Serdes patch =========== */
            /* WSDS_ANA_18 - REG_TX_CLK_SEL, Tx clock will NOT follow Rx clock */
            data = 0x01;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_18r, APOLLOMP_REG_TX_CLK_SELf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
#ifdef CONFIG_FIBER_DETECT_OOBS
            /* WSDS_ANA_1A - REG_RX_SEL_SD, use oobs for signal detect instead signal detect pin */
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Ar, APOLLOMP_REG_RX_SEL_SDf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* WSDS_ANA_0A - REG_OOBS_SEN, change OOBS detection threshold to 200mV */
            data = 0xf;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_0Ar, APOLLOMP_REG_OOBS_SENf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
#endif
            /* WSDS_DIG_0A - CFG_F_CODE_RX_25M, change PLL divisor */
            data = 0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_0Ar, APOLLOMP_CFG_F_CODE_RX_25Mf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* WSDS_DIG_0C - CFG_N_CODE_RX_25M, change PLL diviso */
            data = 0x60;
            if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_0Cr, APOLLOMP_CFG_N_CODE_RX_25Mf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

#ifdef CONFIG_FIBER_DETECT_SD
    #ifdef CONFIG_FIBER_MODULE_LOSS
            /* =========== Fiber module SD/LOSS output config =========== */
            /* WSDS_ANA_00 - fiber output loss, so it has to be inverted to allow chip detect */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_RX_SD_POR_SELf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
    #else
            /* WSDS_ANA_00 - fiber output SD, so chip can detect directly */
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_RX_SD_POR_SELf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
    #endif
#endif
            /* WSDS_ANA_00 - REG_CDR_RESET_SEL to let RX CDR reset when SD signal toggled */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_CDR_RESET_SELf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* WSDS_ANA_00 - REG_CMU_BYPASS_PI_RX, no floating points so don't use phase interpolation */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_CMU_BYPASS_PI_RXf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* WSDS_ANA_00 - REG_SPDSEL, Rx 1.25G */
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_SPDSELf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* WSDS_ANA_0E - REG_TX_AMP, 700mV */
            data = 0x4;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_0Er, APOLLOMP_REG_TX_AMPf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* WSDS_ANA_0E - REG_TX_BAMP */
            data = 0x4;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_0Er, APOLLOMP_REG_TX_BAMPf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* WSDS_ANA_0E - REG_TX_EN_EMPHAS */
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_0Er, APOLLOMP_REG_TX_EN_EMPHASf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* WSDS_ANA_10 - REG_Z0_PADJR, REG_Z0_PAUTO_K, REG_Z0_TUNE */
            data = 0x10;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_10r, APOLLOMP_REG_Z0_PADJRf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_10r, APOLLOMP_REG_Z0_PAUTO_Kf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_10r, APOLLOMP_REG_Z0_TUNEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* WSDS_ANA_1C - REG_RX_FILT_CONFIG */
            data = 0x8;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_1Cr, APOLLOMP_REG_RX_FILT_CONFIGf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* =========== Fiber phy config =========== */
            /* Set flow control ability */
            data = 0x01A0;
            if ((ret = reg_write(APOLLOMP_FIB_REG4r, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* select fiber */
            data = 1;
            if ((ret = reg_field_write(APOLLOMP_UTP_FIBER_AUTODETr, APOLLOMP_SEL_FIBf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

    	    /* SDS_REG1 - CFG_RG1X54, turn on auto down speed ability */
            data = 0x2;
            if ((ret = reg_field_write(APOLLOMP_SDS_REG1r, APOLLOMP_CFG_RG1X54f, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            /* SDS mode, force SDS mode and Fiber capability */
            /* Disable force mode first */
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_SDS_REG4r, APOLLOMP_CFG_FRC_SDS_MODE_ENf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }

            if((mode == PONMAC_MODE_FIBER_100M) || (mode == PONMAC_MODE_FIBER_FORCE_100M))
            {
        	    /* set the serdes mode to fiber 100M mode */
                data = 0x5;
                if ((ret = reg_field_write(APOLLOMP_SDS_CFGr, APOLLOMP_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* SDS_REG4 - CFG_FRC_SDS_MODE, set force 100M */
                data = 0x5;
                if ((ret = reg_field_write(APOLLOMP_SDS_REG4r, APOLLOMP_CFG_FRC_SDS_MODEf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* Set AN disable, speed 100M */
                data = 0x2100;
                if ((ret = reg_write(APOLLOMP_FIB_REG0r, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
            }
            else if((mode == PONMAC_MODE_FIBER_1G) || (mode == PONMAC_MODE_FIBER_FORCE_1G))
            {
        	    /* set the serdes mode to fiber 1G mode */
                data = 0x4;
                if ((ret = reg_field_write(APOLLOMP_SDS_CFGr, APOLLOMP_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* SDS_REG4 - CFG_FRC_SDS_MODE, set force 1G */
                data = 0x4;
                if ((ret = reg_field_write(APOLLOMP_SDS_REG4r, APOLLOMP_CFG_FRC_SDS_MODEf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                if(mode == PONMAC_MODE_FIBER_1G)
                {
                    /* Set AN enable, full duplex */
                    data = 0x1140;
                }
                else
                {
                    /* Set AN disable, full duplex */
                    data = 0x0140;
                }
                if ((ret = reg_write(APOLLOMP_FIB_REG0r, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
            }
            else
            {
        	    /* set the serdes mode to fiber auto mode */
                data = 0x7;
                if ((ret = reg_field_write(APOLLOMP_SDS_CFGr, APOLLOMP_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
                /* Set AN enable, full duplex */
                data = 0x1140;
                if ((ret = reg_write(APOLLOMP_FIB_REG0r, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
            }

    	    /* Enable or keep disable force mode */
            if((mode == PONMAC_MODE_FIBER_FORCE_100M) || (mode == PONMAC_MODE_FIBER_FORCE_1G))
            {
                data = 0x1;
                if ((ret = reg_field_write(APOLLOMP_SDS_REG4r, APOLLOMP_CFG_FRC_SDS_MODE_ENf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                    return ret;
                }
            }

            /* change mode to Fiber, must reset switch */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_SOFTWARE_RSTr, APOLLOMP_SW_RSTf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            osal_time_mdelay(10);

            /* WSDS_ANA_00 - REG_CDR_RESET_MANUAL, toggle CDR manual reset */
            data = 0x1;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_CDR_RESET_MANUALf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            data = 0x0;
            if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_CDR_RESET_MANUALf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
            /* Update the flow control thresholds */
            if ((ret = apollomp_raw_flowctrl_fiberThreshold_set()) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return ret;
            }
        }
        break;

        default:
            break;
    }

    /* set ben force mode to 0 */
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_CFG_FRC_BENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }


    ponMode = mode;
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_apollomp_ponmac_mode_get
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
dal_apollomp_ponmac_mode_get(rtk_ponmac_mode_t *pMode)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    *pMode=ponMode;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_ponmac_queueDrainOut_set
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
dal_apollomp_ponmac_queueDrainOut_set(rtk_ponmac_queue_t *pQueue)
{
    uint32 queueId;
	int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    /* function body */

    queueId = ((pQueue->schedulerId)/8) * 32 + pQueue->queueId;
    if ((ret = apollomp_raw_ponMacQueueDrainOutState_set(queueId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_ponmac_queueDrainOut_set */

/* Function Name:
 *      dal_apollomp_ponmac_serdesCdr_reset
 * Description:
 *      Serdes CDR reset by change polarity of RXSD
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
dal_apollomp_ponmac_serdesCdr_reset(void)
{
    uint32 data;
    uint32 org_sd;
    int32 ret;

    /* force ber_notify 0 */
    data = 0x1;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_01r, APOLLOMP_CFG_FRC_NOTIFYf , &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    /* toggle RX SD */
    if ((ret = reg_field_read(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_RX_SD_POR_SELf, &org_sd)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    data = !org_sd;
    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_RX_SD_POR_SELf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }
    osal_time_usleep(10000);

    if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_00r, APOLLOMP_REG_RX_SD_POR_SELf, &org_sd)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    /* force ber_notify 1 */
    data = 0x3;
    if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_01r, APOLLOMP_CFG_FRC_NOTIFYf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_serdesCdr_reset */

/* Function Name:
 *      dal_apollomp_ponmac_linkState_get
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
dal_apollomp_ponmac_linkState_get(rtk_ponmac_mode_t mode, uint32 *pSd, uint32 *pSync)
{
    uint32 val, val1;
    int32 ret;

    switch(mode)
    {
        case PONMAC_MODE_GPON:
            if ((ret = reg_field_read(APOLLOMP_GPON_GTC_DS_INTR_STSr,APOLLOMP_LOSf,&val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            *pSd = !val;
            if ((ret = reg_field_read(APOLLOMP_GPON_GTC_DS_INTR_STSr,APOLLOMP_LOFf,&val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            if ((ret = reg_field_read(APOLLOMP_GPON_GTC_DS_INTR_STSr,APOLLOMP_LOMf,&val1)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            *pSync = !(val|val1);
            break;

        case PONMAC_MODE_EPON:
            if ((ret = reg_field_read(APOLLOMP_SDS_EXT_REG29r, APOLLOMP_SIGNOK_LATf, pSd)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                return ret;
            }

            if ((ret = reg_field_read(APOLLOMP_SDS_EXT_REG29r, APOLLOMP_LINKOK_LATf, pSync)) != RT_ERR_OK)
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
}   /* end of dal_apollomp_ponmac_linkState_get */

/* Function Name:
 *      dal_apollomp_ponmac_bwThreshold_set
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
dal_apollomp_ponmac_bwThreshold_set(uint32 bwThreshold, uint32 reqBwThreshold)
{
	uint32 wData;
	int32 ret;

	/*Set PON BW_THRESHOLD*/
    wData = bwThreshold;
    if ((ret = reg_field_write(APOLLOMP_PON_PORT_CTRLr, APOLLOMP_BW_THRESHOLDf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*Set PON REQ_BW_THRESHOLD*/
    wData = reqBwThreshold;
    if ((ret = reg_field_write(APOLLOMP_PON_PORT_CTRLr, APOLLOMP_REQ_BW_THRESHOLDf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_ponmac_bwThreshold_get
 * Description:
 *      Get PON Port threshold
 * Input:
 *      N/A
 * Output:
 *		pBwThreshold    - PON BW_THRESHOLD
 *      pReqBwThreshold - PON REQ_BW_THRESHOLD
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_apollomp_ponmac_bwThreshold_get(uint32 *pBwThreshold, uint32 *pReqBwThreshold)
{
	uint32 rData;
	int32 ret;

	/*Get PON BW_THRESHOLD*/
    if ((ret = reg_field_read(APOLLOMP_PON_PORT_CTRLr, APOLLOMP_BW_THRESHOLDf, &rData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
	*pBwThreshold = rData;

    /*Get PON REQ_BW_THRESHOLD*/
    if ((ret = reg_field_read(APOLLOMP_PON_PORT_CTRLr, APOLLOMP_REQ_BW_THRESHOLDf, &rData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
	*pReqBwThreshold = rData;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_ponmac_losState_get
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
dal_apollomp_ponmac_losState_get(rtk_enable_t *pState)
{
	uint32  tmpVal;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(APOLLOMP_SDS_EXT_REG29r,APOLLOMP_SIGNOK_LATf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(1==tmpVal)
        *pState = DISABLED;
    else
        *pState = ENABLED;

    return RT_ERR_OK;
}   /* end of dal_apollomp_ponmac_losState_get */

/* Function Name:
 *      dal_apollomp_rtk_ponmac_txPll_relock
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
 *      None
 */
int32
dal_apollomp_rtk_ponmac_txPll_relock(void)
{
	uint32  data;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* function body */
    /* re-lock tx PLL. MP2099 pull RX_SD by power on in high optic (> -12dbm), it casuse tx Pll lock wrong clock rate */
    if ((ret = reg_field_read(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }
    if(data==1)
    {
        data = 0x0;
        if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        data = 0x1;
        if ((ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        osal_printf("!!!re-lock tx PLL!!!\n\r");

        /* serdes fifo reset for fifo r/w pointer issue */
        data = 0x0;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_1Dr,APOLLOMP_CFG_SFT_RSTB_INFf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
            return ret;
        }
        data = 0x1;
        if ((ret = reg_field_write(APOLLOMP_WSDS_DIG_1Dr,APOLLOMP_CFG_SFT_RSTB_INFf,&data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_rtk_ponmac_txPll_relock */
