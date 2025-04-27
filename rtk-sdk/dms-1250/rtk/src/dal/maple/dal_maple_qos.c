/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : Definition those public port bandwidth control and storm control APIs and its data type
 *           in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *             1) Configuration of ingress port bandwidth control (ingress rate limit ).
 *             2) Configuration of egress port bandwidth control (egress rate limit).
 *             3) Configuration of storm control
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_qos.h>
#include <dal/maple/dal_maple_port.h>
#include <dal/maple/dal_maple_rate.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/qos.h>

/*
 * Symbol Definition
 */
#define QOS_CHK_QUEUE_EMPTY_TIMES   (1200000)
#define QUEUE_EMPTY_VALUE           (BITMASK_28B)
#define CPU_QUEUE_NUM               (8)

/*
 * Data Declaration
 */
static uint32           qos_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t     qos_sem[RTK_MAX_NUM_OF_UNIT];

rtk_qos_scheduling_type_t portSche_type[RTK_MAX_NUM_OF_UNIT][29];


/*
 * Macro Definition
 */
/* qos semaphore handling */
#define QOS_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(qos_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_QOS), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define QOS_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(qos_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_QOS), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

#define RT_IF_ERR_GOTO_HANDLE(op, errHandle, ret) \
do {\
    if ((ret = (op)) != RT_ERR_OK)\
        goto errHandle;\
} while(0)

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_maple_qosMapper_init
 * Description:
 *      Hook qos module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook qos module before calling any qos APIs.
 */
int32
dal_maple_qosMapper_init(dal_mapper_t *pMapper)
{
    /* Qos function */
    pMapper->qos_init = dal_maple_qos_init;
    pMapper->qos_priSelGroup_get = dal_maple_qos_priSelGroup_get;
    pMapper->qos_priSelGroup_set = dal_maple_qos_priSelGroup_set;
    pMapper->qos_portPriSelGroup_get = dal_maple_qos_portPriSelGroup_get;
    pMapper->qos_portPriSelGroup_set = dal_maple_qos_portPriSelGroup_set;
    pMapper->qos_priRemap_get = dal_maple_qos_priRemap_get;
    pMapper->qos_priRemap_set = dal_maple_qos_priRemap_set;
    pMapper->qos_priMap_get = dal_maple_qos_priMap_get;
    pMapper->qos_priMap_set = dal_maple_qos_priMap_set;
    pMapper->qos_1pDfltPri_get = dal_maple_qos_1pDfltPri_get;
    pMapper->qos_1pDfltPri_set = dal_maple_qos_1pDfltPri_set;
    pMapper->qos_port1pRemarkEnable_get = dal_maple_qos_port1pRemarkEnable_get;
    pMapper->qos_port1pRemarkEnable_set = dal_maple_qos_port1pRemarkEnable_set;
    pMapper->qos_1pRemarking_get = dal_maple_qos_1pRemarking_get;
    pMapper->qos_1pRemarking_set = dal_maple_qos_1pRemarking_set;
    pMapper->qos_1pRemark_get = dal_maple_qos_1pRemark_get;
    pMapper->qos_1pRemark_set = dal_maple_qos_1pRemark_set;
    pMapper->qos_1pRemarkSrcSel_get = dal_maple_qos_1pRemarkSrcSel_get;
    pMapper->qos_1pRemarkSrcSel_set = dal_maple_qos_1pRemarkSrcSel_set;
    pMapper->qos_portOut1pRemarkEnable_get = dal_maple_qos_portOut1pRemarkEnable_get;
    pMapper->qos_portOut1pRemarkEnable_set = dal_maple_qos_portOut1pRemarkEnable_set;
    pMapper->qos_outer1pRemark_get = dal_maple_qos_outer1pRemark_get;
    pMapper->qos_outer1pRemark_set = dal_maple_qos_outer1pRemark_set;
    pMapper->qos_outer1pRemarking_get = dal_maple_qos_outer1pRemarking_get;
    pMapper->qos_outer1pRemarking_set = dal_maple_qos_outer1pRemarking_set;
    pMapper->qos_outer1pDfltPriCfgSrcSel_get = dal_maple_qos_outer1pDfltPriCfgSrcSel_get;
    pMapper->qos_outer1pDfltPriCfgSrcSel_set = dal_maple_qos_outer1pDfltPriCfgSrcSel_set;
    pMapper->qos_portOuter1pDfltPriSrcSel_get = dal_maple_qos_portOuter1pDfltPriSrcSel_get;
    pMapper->qos_portOuter1pDfltPriSrcSel_set = dal_maple_qos_portOuter1pDfltPriSrcSel_set;
    pMapper->qos_portDscpRemarkEnable_get = dal_maple_qos_portDscpRemarkEnable_get;
    pMapper->qos_portDscpRemarkEnable_set = dal_maple_qos_portDscpRemarkEnable_set;
    pMapper->qos_dscpRemarking_get = dal_maple_qos_dscpRemarking_get;
    pMapper->qos_dscpRemarking_set = dal_maple_qos_dscpRemarking_set;
    pMapper->qos_dscpRemark_get = dal_maple_qos_dscpRemark_get;
    pMapper->qos_dscpRemark_set = dal_maple_qos_dscpRemark_set;
    pMapper->qos_dscp2DscpRemark_get = dal_maple_qos_dscp2DscpRemark_get;
    pMapper->qos_dscp2DscpRemark_set = dal_maple_qos_dscp2DscpRemark_set;
    pMapper->qos_dscpRemarkSrcSel_get = dal_maple_qos_dscpRemarkSrcSel_get;
    pMapper->qos_dscpRemarkSrcSel_set = dal_maple_qos_dscpRemarkSrcSel_set;
    pMapper->qos_schedulingAlgorithm_get = dal_maple_qos_schedulingAlgorithm_get;
    pMapper->qos_schedulingAlgorithm_set = dal_maple_qos_schedulingAlgorithm_set;
    pMapper->qos_schedulingQueue_get = dal_maple_qos_schedulingQueue_get;
    pMapper->qos_schedulingQueue_set = dal_maple_qos_schedulingQueue_set;
    pMapper->qos_portPri2IgrQMapEnable_get = dal_maple_qos_portPri2IgrQMapEnable_get;
    pMapper->qos_portPri2IgrQMapEnable_set = dal_maple_qos_portPri2IgrQMapEnable_set;
    pMapper->qos_portPri2IgrQMap_get = dal_maple_qos_portPri2IgrQMap_get;
    pMapper->qos_portPri2IgrQMap_set = dal_maple_qos_portPri2IgrQMap_set;
    pMapper->qos_1pDfltPriSrcSel_get = dal_maple_qos_1pDfltPriSrcSel_get;
    pMapper->qos_1pDfltPriSrcSel_set = dal_maple_qos_1pDfltPriSrcSel_set;
    pMapper->qos_portOuter1pRemarkSrcSel_get = dal_maple_qos_portOuter1pRemarkSrcSel_get;
    pMapper->qos_portOuter1pRemarkSrcSel_set = dal_maple_qos_portOuter1pRemarkSrcSel_set;
    pMapper->qos_outer1pDfltPri_get = dal_maple_qos_outer1pDfltPri_get;
    pMapper->qos_outer1pDfltPri_set = dal_maple_qos_outer1pDfltPri_set;
    pMapper->qos_congAvoidAlgo_get = dal_maple_qos_congAvoidAlgo_get;
    pMapper->qos_congAvoidAlgo_set = dal_maple_qos_congAvoidAlgo_set;
    pMapper->qos_portAvbStreamReservationClassEnable_get = dal_maple_qos_portAvbStreamReservationClassEnable_get;
    pMapper->qos_portAvbStreamReservationClassEnable_set = dal_maple_qos_portAvbStreamReservationClassEnable_set;
    pMapper->qos_avbStreamReservationConfig_get = dal_maple_qos_avbStreamReservationConfig_get;
    pMapper->qos_avbStreamReservationConfig_set = dal_maple_qos_avbStreamReservationConfig_set;
    pMapper->qos_portInnerPri_get = dal_maple_qos_portInnerPri_get;
    pMapper->qos_portInnerPri_set = dal_maple_qos_portInnerPri_set;
    pMapper->qos_portOuterPri_get = dal_maple_qos_portOuterPri_get;
    pMapper->qos_portOuterPri_set = dal_maple_qos_portOuterPri_set;
    pMapper->qos_rspanPriRemap_get = dal_maple_qos_rspanPriRemap_get;
    pMapper->qos_rspanPriRemap_set = dal_maple_qos_rspanPriRemap_set;
    pMapper->qos_pkt2CpuPriRemap_get = dal_maple_qos_pkt2CpuPriRemap_get;
    pMapper->qos_pkt2CpuPriRemap_set = dal_maple_qos_pkt2CpuPriRemap_set;
    pMapper->qos_portIgrQueueWeight_get = dal_maple_qos_portIgrQueueWeight_get;
    pMapper->qos_portIgrQueueWeight_set = dal_maple_qos_portIgrQueueWeight_set;
    pMapper->qos_invldDscpVal_get = dal_maple_qos_invldDscpVal_get;
    pMapper->qos_invldDscpVal_set = dal_maple_qos_invldDscpVal_set;
    pMapper->qos_invldDscpMask_get = dal_maple_qos_invldDscpMask_get;
    pMapper->qos_invldDscpMask_set = dal_maple_qos_invldDscpMask_set;
    pMapper->qos_portInvldDscpEnable_get = dal_maple_qos_portInvldDscpEnable_get;
    pMapper->qos_portInvldDscpEnable_set = dal_maple_qos_portInvldDscpEnable_set;
    pMapper->qos_portInnerPriRemapEnable_get = dal_maple_qos_portInnerPriRemapEnable_get;
    pMapper->qos_portInnerPriRemapEnable_set = dal_maple_qos_portInnerPriRemapEnable_set;
    pMapper->qos_portOuterPriRemapEnable_get = dal_maple_qos_portOuterPriRemapEnable_get;
    pMapper->qos_portOuterPriRemapEnable_set = dal_maple_qos_portOuterPriRemapEnable_set;
    pMapper->qos_portQueueStrictEnable_get = dal_maple_qos_portQueueStrictEnable_get;
    pMapper->qos_portQueueStrictEnable_set = dal_maple_qos_portQueueStrictEnable_set;
    pMapper->qos_priRemap_get = dal_maple_qos_priRemap_get;
    pMapper->qos_priRemap_set = dal_maple_qos_priRemap_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_init
 * Description:
 *      Configure QoS initial settings with queue number assigment to each port
 * Input:
 *      unit     - unit id
 *      queueNum - Queue number of each port, it is available at 1~8
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Note:
 *      This API will initialize related QoS setting with queue number assignment.
 *      The initialization does the following actions:
 *      1) set input bandwidth control parameters to default values
 *      2) set priority decision parameters
 *      3) set scheduling parameters
 *      4) disable port remark ability
 *      5) set flow control thresholds
 */
int32
dal_maple_qos_init(uint32 unit, uint32 queueNum)
{
    int32 ret;
    uint32 max_queue, queue;
    rtk_qos_pri2queue_t pri2qid;
    rtk_qos_priSelWeight_t weightOfPriSel;
    rtk_pri_t pri;
    uint32 value;
    rtk_port_t port;

    RT_INIT_REENTRY_CHK(qos_init[unit]);
    qos_init[unit] = INIT_NOT_COMPLETED;

    RT_PARAM_CHK(queueNum > HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_QUEUE_NUM);

    /* create semaphore */
    qos_sem[unit] = osal_sem_mutex_create();
    if (0 == qos_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_QOS), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    qos_init[unit] = INIT_COMPLETED;

    max_queue = HAL_MAX_NUM_OF_QUEUE(unit);
    for (queue = 0; queue < max_queue; queue++)
        pri2qid.pri2queue[queue] = queue;

    if((ret = dal_maple_qos_priMap_set(unit, 8, &pri2qid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if((ret = dal_maple_qos_portInnerPriRemapEnable_set(unit, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if((ret = dal_maple_qos_portOuterPriRemapEnable_set(unit, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    weightOfPriSel.weight_of_outerTag = 7;
    weightOfPriSel.weight_of_innerTag = 6;
    weightOfPriSel.weight_of_dscp = 5;
    weightOfPriSel.weight_of_portBasedOpri = 4;
    weightOfPriSel.weight_of_portBasedIpri = 3;
    if((ret = dal_maple_qos_priSelGroup_set(unit, 0, &weightOfPriSel)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    for(pri = 0; pri <= RTK_DOT1P_PRIORITY_MAX; ++pri)
    {
        dal_maple_qos_outer1pRemark_set(unit, pri, pri);
        dal_maple_qos_1pRemark_set(unit, pri, pri);
    }

    QOS_SEM_LOCK(unit);

    HWP_PORT_TRAVS(unit, port)
    {
        /* WFQ*/
        value=0;
        if ((ret = reg_array_field_write(unit, MAPLE_SCHED_P_TYPE_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_SCHED_TYPEf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        value=1;
        if ((ret = reg_array_field_write(unit, MAPLE_SCHED_LB_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_EGR_LB_ENf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        portSche_type[unit][port] = WFQ;
    }

    value=1;
    if ((ret = reg_array_field_write(unit, MAPLE_SCHED_P_TYPE_CTRLr, HWP_CPU_MACID(unit), REG_ARRAY_INDEX_NONE, MAPLE_SCHED_TYPEf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    value=0;
    if ((ret = reg_array_field_write(unit, MAPLE_SCHED_LB_CTRLr, HWP_CPU_MACID(unit), REG_ARRAY_INDEX_NONE, MAPLE_P_EGR_LB_ENf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    portSche_type[unit][HWP_CPU_MACID(unit)] = WRR;

    value =1;
    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_EGR_DROP_CTRLr, HWP_CPU_MACID(unit), REG_ARRAY_INDEX_NONE, MAPLE_RXCNGST_IGNOREf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    value = 1;
    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_EGR_DROP_CTRLr, HWP_CPU_MACID(unit), REG_ARRAY_INDEX_NONE, MAPLE_ALWAYS_EGRDROP_ENf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_init */

/* Function Name:
 *      dal_maple_qos_priSelGroup_get
 * Description:
 *      Get weight of each priority assignment on specified priority selection group.
 * Input:
 *      unit            - unit id
 *      grp_idx         - index of priority selection group
 * Output:
 *      pWeightOfPriSel - pointer to weight of each priority assignment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_qos_priSelGroup_get(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, grp_idx=%u, pWeightOfPriSel=%x"
           , unit, grp_idx, pWeightOfPriSel);
    RT_PARAM_CHK((grp_idx > HAL_PRI_SEL_GROUP_INDEX_MAX(unit)), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);
    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MAPLE_PORT_WTf, &(pWeightOfPriSel->weight_of_portBasedIpri))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MAPLE_PORT_OTAG_WTf, &(pWeightOfPriSel->weight_of_portBasedOpri))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MAPLE_DSCP_WTf, &(pWeightOfPriSel->weight_of_dscp))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MAPLE_ITAG_WTf, &(pWeightOfPriSel->weight_of_innerTag))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MAPLE_OTAG_WTf, &(pWeightOfPriSel->weight_of_outerTag))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_priSelGroup_get */

/* Function Name:
 *      dal_maple_qos_priSelGroup_set
 * Description:
 *      Set weight of each priority assignment on specified priority selection group.
 * Input:
 *      unit            - unit id
 *      grp_idx         - index of priority selection group
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
 *      None
 */
int32
dal_maple_qos_priSelGroup_set(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, grp_idx=%u, weight_of_portBasedIpri=%d, \
           weight_of_portBasedOpri=%d, weight_of_dscp=%d, weight_of_innerTag=%d"
           , unit, grp_idx, pWeightOfPriSel->weight_of_portBasedIpri
           , pWeightOfPriSel->weight_of_portBasedOpri, pWeightOfPriSel->weight_of_dscp
           , pWeightOfPriSel->weight_of_innerTag , pWeightOfPriSel->weight_of_outerTag);
    RT_PARAM_CHK((grp_idx > HAL_PRI_SEL_GROUP_INDEX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_portBasedIpri < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_portBasedIpri > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_portBasedOpri < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_portBasedOpri > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_dscp < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_dscp > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_innerTag < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_innerTag > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_outerTag < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_outerTag > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);
    /* set value to CHIP*/
    val = pWeightOfPriSel->weight_of_portBasedIpri;
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MAPLE_PORT_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_portBasedOpri;
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MAPLE_PORT_OTAG_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_dscp;
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MAPLE_DSCP_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_innerTag;
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MAPLE_ITAG_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_outerTag;
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MAPLE_OTAG_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_priSelGroup_set */

/* Function Name:
 *      dal_maple_qos_portPriSelGroup_get
 * Description:
 *      Get priority selection group for specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pPriSelGrp_idx - pointer to index of priority selection group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_qos_portPriSelGroup_get(uint32 unit, rtk_port_t port, uint32 *pPriSelGrp_idx)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriSelGrp_idx), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, pPriSelGrp_idx=%x"
            , unit, port, pPriSelGrp_idx);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_PORT_TBL_IDX_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_TBL_IDXf, pPriSelGrp_idx)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portPriSelGroup_get */

/* Function Name:
 *      dal_maple_qos_portPriSelGroup_set
 * Description:
 *      Set priority selection group for specified port.
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      priSelGrp_idx - index of priority selection group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_qos_portPriSelGroup_set(uint32 unit, rtk_port_t port, uint32 priSelGrp_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, priSelGrp_idx=%d"
            , unit, port, priSelGrp_idx);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((priSelGrp_idx > HAL_PRI_SEL_GROUP_INDEX_MAX(unit)), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_PORT_TBL_IDX_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_TBL_IDXf, &priSelGrp_idx)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portPriSelGroup_set */

/* Function Name:
 *      dal_maple_qos_dscpPriRemap_get
 * Description:
 *      Get the internal priority that DSCP value remap.
 * Input:
 *      unit     - unit id
 *      dscp     - DSCP value of receiving frame (0~63)
 * Output:
 *      pInt_pri - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - Invalid DSCP value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_dscpPriRemap_get(uint32 unit, uint32 dscp, rtk_pri_t *pInt_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d", unit, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK((NULL == pInt_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_DSCP_REMAPr, REG_ARRAY_INDEX_NONE,
                    dscp, MAPLE_INTPRI_DSCPf, pInt_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pInt_pri=%d", *pInt_pri);

    return RT_ERR_OK;
} /* end of dal_maple_qos_dscpPriRemap_get */

/* Function Name:
 *      dal_maple_qos_dscpPriRemap_set
 * Description:
 *      Set the internal priority that DSCP value remap.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value of receiving frame (0~63)
 *      int_pri - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                the highest prioirty)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *      The Differentiated Service Code Point is a selector for router's per-hop behaviours.
 *      As a selector, there is no implication that a numerically greater DSCP implies a better
 *      network service. As can be seen, the DSCP totally overlaps the old precedence field of TOS.
 *      So if values of DSCP are carefully chosen then backward compatibility can be achieved.
 */
int32
dal_maple_qos_dscpPriRemap_set(uint32 unit, uint32 dscp, rtk_pri_t int_pri)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d, int_pri=%d",
           unit, dscp, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to chip CHIP */
    val = int_pri;
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_DSCP_REMAPr, REG_ARRAY_INDEX_NONE,
                    dscp, MAPLE_INTPRI_DSCPf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_dscpPriRemap_set */

/* Function Name:
 *      dal_maple_qos_1pPriRemap_get
 * Description:
 *      Get the internal priority that 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      pInt_pri  - internal priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t *pInt_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d",
           unit, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((NULL == pInt_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_IPRI_REMAPr, REG_ARRAY_INDEX_NONE,
                    dot1p_pri, MAPLE_INTPRI_IPRIf, pInt_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pInt_pri=%d", *pInt_pri);

    return RT_ERR_OK;
} /* end of dal_maple_qos_1pPriRemap_get */

/* Function Name:
 *      dal_maple_qos_1pPriRemap_set
 * Description:
 *      Set the internal priority that 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 *      int_pri   - internal priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid 802.1p priority
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *      None.
 */
int32
dal_maple_qos_1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t int_pri)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d, int_pri=%d",
           unit, dot1p_pri, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    val = int_pri;
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_IPRI_REMAPr, REG_ARRAY_INDEX_NONE,
                    dot1p_pri, MAPLE_INTPRI_IPRIf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_1pPriRemap_set */

/* Function Name:
 *      dal_maple_qos_outer1pPriRemap_get
 * Description:
 *      Get the internal priority that outer 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 *      dei       - DEI
 * Output:
 *      pInt_pri  - internal priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_QOS_DEI_VALUE   - invalid dei
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_outer1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, uint32 dei, rtk_pri_t *pInt_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d, dei=%d",
           unit, dot1p_pri, dei);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(dei > RTK_DOT1P_DEI_MAX, RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK((NULL == pInt_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if (0 == dei)
    {
        if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_OPRI_DEI0_REMAPr, REG_ARRAY_INDEX_NONE,
                        dot1p_pri, MAPLE_INTPRI_OPRIf, pInt_pri)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }
    else if (1 == dei)
    {
        if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_OPRI_DEI1_REMAPr, REG_ARRAY_INDEX_NONE,
                        dot1p_pri, MAPLE_INTPRI_OPRIf, pInt_pri)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pInt_pri=%d", *pInt_pri);

    return RT_ERR_OK;
} /* end of dal_maple_qos_outer1pPriRemap_get */

/* Function Name:
 *      dal_maple_qos_outer1pPriRemap_set
 * Description:
 *      Set the internal priority that outer 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 *      dei       - DEI
 *      int_pri   - internal priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid 802.1p priority
 *      RT_ERR_QOS_DEI_VALUE    - invalid dei
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *      None.
 */
int32
dal_maple_qos_outer1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, uint32 dei, rtk_pri_t int_pri)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d, dei=%d, int_pri=%d",
           unit, dot1p_pri, dei, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(dei > RTK_DOT1P_DEI_MAX, RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if (0 == dei)
    {
        val = int_pri;
        if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_OPRI_DEI0_REMAPr, REG_ARRAY_INDEX_NONE,
                        dot1p_pri, MAPLE_INTPRI_OPRIf, &val)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }
    else if (1 == dei)
    {
        val = int_pri;
        if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_OPRI_DEI1_REMAPr, REG_ARRAY_INDEX_NONE,
                        dot1p_pri, MAPLE_INTPRI_OPRIf, &val)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_outer1pPriRemap_set */

/* Function Name:
 *      dal_maple_qos_priRemap_get
 * Description:
 *      Get specified priority source mapping to internal priority.
 * Input:
 *      unit   - unit id
 *      src    - priority remap source type
 *      srcVal - remap source value
 * Output:
 *      pPri   - pointer to internal priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - Invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PORT_ID         - Invalid port id
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_QOS_DEI_VALUE   - Invalid DEI/CFI value
 *      RT_ERR_QOS_1P_PRIORITY - Invalid priority value
 *      RT_ERR_NULL_POINTER    - Input parameter may be null pointer
 * Note:
 *      Supported rtk_qos_priSrc_t is as following:
 *      - PRI_SRC_PB_INNER_PRI
 *      - PRI_SRC_PB_OUTER_PRI
 *      - PRI_SRC_INNER_USER_PRI
 *      - PRI_SRC_OUTER_USER_PRI
 *      - PRI_SRC_DSCP
 */
int32
dal_maple_qos_priRemap_get(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 *pPri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch (src)
    {
        case PRI_SRC_PB_INNER_PRI:
            return dal_maple_qos_portInnerPri_get(unit, srcVal.src.port, pPri);
        case PRI_SRC_PB_OUTER_PRI:
            return dal_maple_qos_portOuterPri_get(unit, srcVal.src.port, pPri);
        case PRI_SRC_INNER_USER_PRI:
            return dal_maple_qos_1pPriRemap_get(unit, srcVal.src.tag.pri, pPri);
        case PRI_SRC_OUTER_USER_PRI:
            return dal_maple_qos_outer1pPriRemap_get(unit, srcVal.src.tag.pri, srcVal.src.tag.dei, pPri);
        case PRI_SRC_DSCP:
            return dal_maple_qos_dscpPriRemap_get(unit, srcVal.src.dscp, pPri);
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }
}

/* Function Name:
 *      dal_maple_qos_priRemap_set
 * Description:
 *      Set specified priority source mapping to internal priority.
 * Input:
 *      unit   - unit id
 *      src    - priority remap source type
 *      srcVal - remap source value
 *      pri    - internal priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - Invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PORT_ID         - Invalid port id
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_QOS_DEI_VALUE   - Invalid DEI/CFI value
 *      RT_ERR_QOS_1P_PRIORITY - Invalid priority value
 *      RT_ERR_QOS_INT_PRIORITY- Invalid internal priority value
 * Note:
 *      Supported rtk_qos_priSrc_t is as following:
 *      - PRI_SRC_PB_INNER_PRI
 *      - PRI_SRC_PB_OUTER_PRI
 *      - PRI_SRC_INNER_USER_PRI
 *      - PRI_SRC_OUTER_USER_PRI
 *      - PRI_SRC_DSCP
 */
int32
dal_maple_qos_priRemap_set(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 pri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch (src)
    {
        case PRI_SRC_PB_INNER_PRI:
            return dal_maple_qos_portInnerPri_set(unit, srcVal.src.port, pri);
        case PRI_SRC_PB_OUTER_PRI:
            return dal_maple_qos_portOuterPri_set(unit, srcVal.src.port, pri);
        case PRI_SRC_INNER_USER_PRI:
            return dal_maple_qos_1pPriRemap_set(unit, srcVal.src.tag.pri, pri);
        case PRI_SRC_OUTER_USER_PRI:
            return dal_maple_qos_outer1pPriRemap_set(unit, srcVal.src.tag.pri, srcVal.src.tag.dei, pri);
        case PRI_SRC_DSCP:
            return dal_maple_qos_dscpPriRemap_set(unit, srcVal.src.dscp, pri);
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }
}

/* Function Name:
 *      dal_maple_qos_priMap_get
 * Description:
 *      Get the value of internal priority to QID mapping table.
 * Input:
 *      unit      - unit id
 *      queue_num - the number of queue (1~8).
 * Output:
 *      pPri2qid  - array of internal priority on a queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_NUM    - Invalid queue number
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_priMap_get(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid)
{
    int32       ret;
    rtk_pri_t   pri;

    uint32 regVal = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue_num=%d", unit, queue_num);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((queue_num > HAL_MAX_NUM_OF_QUEUE(unit)) || (queue_num < HAL_MIN_NUM_OF_QUEUE(unit))), RT_ERR_QUEUE_NUM);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_read(unit, MAPLE_QM_INTPRI2QID_CTRLr, &regVal)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        return ret;
    }

    /* get value from CHIP*/
    for (pri = 0; pri <= HAL_INTERNAL_PRIORITY_MAX(unit); pri++)
    {
        switch (pri)
        {
            case 0:
                if ((ret = reg_field_get(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI0_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 1:
                if ((ret = reg_field_get(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI1_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 2:
                if ((ret = reg_field_get(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI2_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 3:
                if ((ret = reg_field_get(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI3_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 4:
                if ((ret = reg_field_get(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI4_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 5:
                if ((ret = reg_field_get(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI5_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 6:
                if ((ret = reg_field_get(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI6_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 7:
                if ((ret = reg_field_get(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI7_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            default:
                break;
        }
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri2qid[0]=%d, pPri2qid[1]=%d, pPri2qid[2]=%d\
           pPri2qid[3]=%d, pPri2qid[4]=%d, pPri2qid[5]=%d, pPri2qid[6]=%d, pPri2qid[7]=%d",
           pPri2qid->pri2queue[0],
           pPri2qid->pri2queue[1],
           pPri2qid->pri2queue[2],
           pPri2qid->pri2queue[3],
           pPri2qid->pri2queue[4],
           pPri2qid->pri2queue[5],
           pPri2qid->pri2queue[6],
           pPri2qid->pri2queue[7]);
    return RT_ERR_OK;
} /* end of dal_maple_qos_priMap_get */

/* Function Name:
 *      dal_maple_qos_priMap_set
 * Description:
 *      Set the value of internal priority to QID mapping table.
 * Input:
 *      unit      - unit id
 *      queue_num - the number of queue (1~8).
 *      pPri2qid  - array of internal priority on a queue
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_NUM    - Invalid queue number
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Below is an example of internal priority to QID mapping table.
 *      When queue numbers are 8, the pri2qid are pri2qid[0]=0, pri2qid[1]=1, pri2qid[2]=2..., etc.
 *
 *                  Number of Available Output Queue
 *        Priority  1   2   3   4   5   6   7   8
 *              0   0   0   0   0   0   0   0   0
 *              1   0   0   0   0   0   0   0   1
 *              2   0   0   0   1   1   1   1   2
 *              3   0   0   0   1   1   2   2   3
 *              4   0   1   1   2   2   3   3   4
 *              5   0   1   1   2   3   4   4   5
 *              6   0   1   2   3   4   5   5   6
 *              7   0   1   2   3   4   5   6   7
 */
int32
dal_maple_qos_priMap_set(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid)
{
    int32       ret;
    rtk_pri_t   pri;

    uint32 regVal = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue_num=%d", unit, queue_num);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((queue_num > HAL_MAX_NUM_OF_QUEUE(unit)) || (queue_num < HAL_MIN_NUM_OF_QUEUE(unit))), RT_ERR_QUEUE_NUM);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pPri2qid->pri2queue[0] >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pPri2qid->pri2queue[1] >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pPri2qid->pri2queue[2] >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pPri2qid->pri2queue[3] >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pPri2qid->pri2queue[4] >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pPri2qid->pri2queue[5] >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pPri2qid->pri2queue[6] >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pPri2qid->pri2queue[7] >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "Pri2qid[0]=%d, pPri2qid[1]=%d, pPri2qid[2]=%d\
           pPri2qid[3]=%d, pPri2qid[4]=%d, pPri2qid[5]=%d, pPri2qid[6]=%d, pPri2qid[7]=%d",
           pPri2qid->pri2queue[0],
           pPri2qid->pri2queue[1],
           pPri2qid->pri2queue[2],
           pPri2qid->pri2queue[3],
           pPri2qid->pri2queue[4],
           pPri2qid->pri2queue[5],
           pPri2qid->pri2queue[6],
           pPri2qid->pri2queue[7]);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    for (pri = 0; pri <= HAL_INTERNAL_PRIORITY_MAX(unit); pri++)
    {
        switch (pri)
        {
            case 0:
                if ((ret = reg_field_set(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI0_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 1:
                if ((ret = reg_field_set(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI1_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 2:
                if ((ret = reg_field_set(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI2_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 3:
                if ((ret = reg_field_set(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI3_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 4:
                if ((ret = reg_field_set(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI4_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 5:
                if ((ret = reg_field_set(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI5_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 6:
                if ((ret = reg_field_set(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI6_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 7:
                if ((ret = reg_field_set(unit, MAPLE_QM_INTPRI2QID_CTRLr, MAPLE_PRI7_QNUMf,  &pPri2qid->pri2queue[pri], &regVal)) != RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            default:
                break;
        }
    }

    if ((ret = reg_write(unit, MAPLE_QM_INTPRI2QID_CTRLr, &regVal)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_priMap_set */

/* Function Name:
 *      dal_maple_qos_pri2QidMap_get
 * Description:
 *      Get internal priority to normal port QID mapping value.
 * Input:
 *      unit      - unit id
 *      pri       - internal priority
 * Output:
 *      pQid      - queue id of normal port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PRIORITY     - Invalid Priority value
 *      RT_ERR_NULL_POINTER - Input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_qos_pri2QidMap_get(uint32 unit, rtk_pri_t pri, rtk_qid_t *pQid)
{
    int32 ret;
    rtk_qos_pri2queue_t pri2qid;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* get value to CHIP*/
    osal_memset(&pri2qid, 0, sizeof(rtk_qos_pri2queue_t));
    if ((ret = dal_maple_qos_priMap_get(unit, 0, &pri2qid)) != RT_ERR_OK)
    {
        return ret;
    }

    *pQid = pri2qid.pri2queue[pri];

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_pri2QidMap_set
 * Description:
 *      Set internal priority to normal port QID mapping value.
 * Input:
 *      unit      - unit id
 *      pri       - internal priority
 *      qid       - queue id of normal port
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PRIORITY     - Invalid Priority value
 *      RT_ERR_QUEUE_ID     - Invalid queue ID
 * Note:
 *      None
 */
int32
dal_maple_qos_pri2QidMap_set(uint32 unit, rtk_pri_t pri, rtk_qid_t qid)
{
    int32 ret;
    uint32 queue_num;
    rtk_qos_pri2queue_t pri2qid;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* set value to CHIP*/
    osal_memset(&pri2qid, 0, sizeof(rtk_qos_pri2queue_t));

    for (queue_num = HAL_MIN_NUM_OF_QUEUE(unit); queue_num <= HAL_MAX_NUM_OF_QUEUE(unit); queue_num++)
    {
        if ((ret = dal_maple_qos_priMap_get(unit, queue_num, &pri2qid)) != RT_ERR_OK)
        {
            return ret;
        }

        pri2qid.pri2queue[pri] = qid;

        if ((ret = dal_maple_qos_priMap_set(unit, queue_num, &pri2qid)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_port1pRemarkEnable_get
 * Description:
 *      Get 802.1p remark status for a port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of 802.1p remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_port1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    osal_memset(&portmask, 0, sizeof(portmask));

    QOS_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MAPLE_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IPRI_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_qos_port1pRemarkEnable_get */

/* Function Name:
 *      dal_maple_qos_port1pRemarkEnable_set
 * Description:
 *      Set 802.1p remark status for a port
 * Input:
 *      unit   - unit id
 *      port   - port id.
 *      enable - status of 802.1p remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_PORT_ID   - Invalid port id
 *      RT_ERR_INPUT     - invalid input parameter
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_port1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    osal_memset(&portmask, 0, sizeof(portmask));

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_IPRI_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_port1pRemarkEnable_set */

/* Function Name:
 *      dal_maple_qos_1pRemarking_get
 * Description:
 *      Get the internal priority/original inner priority to
 *      remarkd 802.1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 * Output:
 *      pPri - remarked 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_qos_1pRemarking_get(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t *pPri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch(src)
    {
        case DOT_1P_RMK_SRC_INT_PRI:
        case DOT_1P_RMK_SRC_USER_PRI:
            return dal_maple_qos_1pRemark_get(unit, val.pri.val, pPri);
        default:
            return RT_ERR_INPUT;
    }
}

/* Function Name:
 *      dal_maple_qos_1pRemarking_set
 * Description:
 *      Set the internal priority/original inner priority to
 *      remarkd 802.1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 *      pri     - remarked 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      None
 */
int32
dal_maple_qos_1pRemarking_set(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t pri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch(src)
    {
        case DOT_1P_RMK_SRC_INT_PRI:
        case DOT_1P_RMK_SRC_USER_PRI:
            return dal_maple_qos_1pRemark_set(unit, val.pri.val, pri);
        default:
            return RT_ERR_INPUT;
    }
}

/* Function Name:
 *      dal_maple_qos_1pRemark_get
 * Description:
 *      Get the internal priority (3bits) to remarkd 802.1p priority(3bits) mapping.
 * Input:
 *      unit       - unit id
 *      int_pri    - internal priority value (range from 0 ~ 7)
 * Output:
 *      pDot1p_pri - remarked 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d", unit, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_RMK_IPRI_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    MAPLE_IPRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_maple_qos_1pRemark_get */

/* Function Name:
 *      dal_maple_qos_1pRemark_set
 * Description:
 *      Set the internal priority(3bits) to remarked 802.1p priority(3bits) mapping.
 * Input:
 *      unit      - unit id
 *      int_pri   - internal priority value (range from 0 ~ 7)
 *      dot1p_pri - remarked 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 * Note:
 *      802.1p remark functionality can map the internal priority or original tag priority
 *      to 802.1p priority before a packet is going to be transmited.
 */
int32
dal_maple_qos_1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d, dot1p_pri=%d",
           unit, int_pri, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, MAPLE_RMK_IPRI_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    MAPLE_IPRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_1pRemark_set */

/* Function Name:
 *      dal_maple_qos_1pRemarkSrcSel_get
 * Description:
 *      Get the remarking source of 802.1p priority(3bits) remarking.
 * Input:
 *      unit    - unit id
 * Output:
 *      pType   - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_1pRemarkSrcSel_get(uint32 unit, rtk_qos_1pRmkSrc_t *pType)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_RMK_CTRLr, MAPLE_IPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = DOT_1P_RMK_SRC_INT_PRI;
    else
        *pType = DOT_1P_RMK_SRC_USER_PRI;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_maple_qos_1pRemarkSrcSel_get */

/* Function Name:
 *      dal_maple_qos_1pRemarkSrcSel_set
 * Description:
 *      Set the remarking source of 802.1p priority(3bits) remarking.
 * Input:
 *      unit    - unit id
 *      type    - remarking source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_maple_qos_1pRemarkSrcSel_set(uint32 unit, rtk_qos_1pRmkSrc_t type)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d",
           unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= DOT_1P_RMK_SRC_END, RT_ERR_INPUT);

    if (DOT_1P_RMK_SRC_INT_PRI == type)
        value = 0;
    else if (DOT_1P_RMK_SRC_USER_PRI == type)
        value = 1;
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_field_write(unit, MAPLE_RMK_CTRLr, MAPLE_IPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_1pRemarkSrcSel_set */

/* Function Name:
 *      dal_maple_qos_1pDfltPriSrcSel_get
 * Description:
 *      Get default inner-priority source of specified port
 * Input:
 *      unit  - unit id
 * Output:
 *      pType - default outer dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_1pDfltPriSrcSel_get(uint32 unit, rtk_qos_1pDfltPriSrc_t *pType)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);
    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_RMK_CTRLr, MAPLE_IPRI_DFLT_INTPRIf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    if(value == 0)
    {
        QOS_SEM_LOCK(unit);

        /* get value from CHIP*/
        if ((ret = reg_field_read(unit, MAPLE_RMK_CTRLr, MAPLE_IPRI_DFLT_SRCf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        QOS_SEM_UNLOCK(unit);

        if (0 == value)
            *pType = INNER_1P_DFLT_SRC_DFLT_PRI;
        else
            *pType = INNER_1P_DFLT_SRC_PB_INNER_PRI;
    }
    else
        *pType = INNER_1P_DFLT_SRC_INT_PRI;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pType);

    return RT_ERR_OK;
}/* end of dal_maple_qos_1pDfltPriSrcSel_get */

/* Function Name:
 *      dal_maple_qos_1pDfltPriSrcSel_set
 * Description:
 *      Set default inner-priority source of specified port
 * Input:
 *      unit - unit id
 *      type - default outer dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_INPUT     - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_maple_qos_1pDfltPriSrcSel_set(uint32 unit, rtk_qos_1pDfltPriSrc_t type)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= INNER_1P_DFLT_SRC_END || type == INNER_1P_DFLT_SRC_PB_PRI, RT_ERR_INPUT);

    if(type == INNER_1P_DFLT_SRC_INT_PRI)
    {
        value = 1;
        QOS_SEM_LOCK(unit);

        /* program value to chip */
        if ((ret = reg_field_write(unit, MAPLE_RMK_CTRLr, MAPLE_IPRI_DFLT_INTPRIf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        QOS_SEM_UNLOCK(unit);
    }
    else
    {
        switch (type)
        {
            case INNER_1P_DFLT_SRC_DFLT_PRI:
                value = 0;
                break;
            case INNER_1P_DFLT_SRC_PB_INNER_PRI:
                value = 1;
                break;
            default:
                return RT_ERR_INPUT;
        }

        QOS_SEM_LOCK(unit);

        /* program value to chip */
        if ((ret = reg_field_write(unit, MAPLE_RMK_CTRLr, MAPLE_IPRI_DFLT_SRCf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        value = 0;
        if ((ret = reg_field_write(unit, MAPLE_RMK_CTRLr, MAPLE_IPRI_DFLT_INTPRIf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        QOS_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}/* end of dal_maple_qos_1pDfltPriSrcSel_set */

/* Function Name:
 *      dal_maple_qos_outer1pDfltPriCfgSrcSel_get
 * Description:
 *      Get default outer-priority configured source
 * Input:
 *      unit       - unit id
 * Output:
 *      pDflt_sel  - default selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_outer1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t *pDflt_sel)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDflt_sel), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_RMK_CTRLr, MAPLE_OPRI_DFLT_CFGf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pDflt_sel = OUTER_1P_DFLT_CFG_SRC_INGRESS;
    else
        *pDflt_sel = OUTER_1P_DFLT_CFG_SRC_EGRESS;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDflt_sel=%d", *pDflt_sel);

    return RT_ERR_OK;
} /* end of dal_maple_qos_outer1pDfltPriCfgSrcSel_get */

/* Function Name:
 *      dal_maple_qos_outer1pDfltPriCfgSrcSel_set
 * Description:
 *      Set default outer-priority configured source
 * Input:
 *      unit      - unit id
 *      dflt_sel  - default selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_maple_qos_outer1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t dflt_sel)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dflt_sel=%d", unit, dflt_sel);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dflt_sel > OUTER_1P_DFLT_CFG_SRC_END, RT_ERR_INPUT);

    if (OUTER_1P_DFLT_CFG_SRC_INGRESS == dflt_sel)
        value = 0;
    else
        value = 1;

    QOS_SEM_LOCK(unit);

    /* program value to chip */
    if ((ret = reg_field_write(unit, MAPLE_RMK_CTRLr, MAPLE_OPRI_DFLT_CFGf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_outer1pDfltPriCfgSrcSel_set */

/* Function Name:
 *      dal_maple_qos_1pDfltPri_get
 * Description:
 *      Get default inner-priority value
 * Input:
 *      unit       - unit id
 * Output:
 *      pDot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_RMK_CTRLr, MAPLE_IPRI_DFLT_PRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_maple_qos_1pDfltPri_get */

/* Function Name:
 *      dal_maple_qos_1pDfltPri_set
 * Description:
 *      Set default inner-priority value
 * Input:
 *      unit      - unit id
 *      dot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid dot1p priority
 * Note:
 *      None.
 */
int32
dal_maple_qos_1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d", unit, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to chip */
    if ((ret = reg_field_write(unit, MAPLE_RMK_CTRLr, MAPLE_IPRI_DFLT_PRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_1pDfltPri_set */

/* Function Name:
 *      dal_maple_qos_portOut1pRemarkEnable_get
 * Description:
 *      Get enable status of outer dot1p remarking on specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of outer dot1p remarking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_portOut1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    osal_memset(&portmask, 0, sizeof(portmask));

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_OPRI_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portOut1pRemarkEnable_get */

/* Function Name:
 *      dal_maple_qos_portOut1pRemarkEnable_set
 * Description:
 *      Set enable status of outer dot1p remarking on specified port
 * Input:
 *      unit   - unit id
 *      port   - port id.
 *      enable - enable status of outer dot1p remarking
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_portOut1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    osal_memset(&portmask, 0, sizeof(portmask));

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_OPRI_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portOut1pRemarkEnable_set */

/* Function Name:
 *      dal_maple_qos_outer1pRemark_get
 * Description:
 *      Get the internal priority (3bits) to remarkd outer dot1p priority(3bits) mapping.
 * Input:
 *      unit       - unit id
 *      int_pri    - internal priority value (range from 0 ~ 7)
 * Output:
 *      pDot1p_pri - remarked outer dot1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_outer1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d", unit, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_RMK_OPRI_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    MAPLE_OPRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_maple_qos_outer1pRemark_get */

/* Function Name:
 *      dal_maple_qos_outer1pRemark_set
 * Description:
 *      Set the internal priority(3bits) to remarked outer dot1p priority(3bits) mapping.
 * Input:
 *      unit      - unit id
 *      int_pri   - internal priority value (range from 0 ~ 7)
 *      dot1p_pri - remarked 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      Outer dot1p remark functionality can map the internal priority or original tag priority
 *      to outer dot1p priority before a packet is going to be transmited.
 */
int32
dal_maple_qos_outer1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d, dot1p_pri=%d",
           unit, int_pri, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, MAPLE_RMK_OPRI_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    MAPLE_OPRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_outer1pRemark_set */

/* Function Name:
 *      dal_maple_qos_outer1pRemarking_get
 * Description:
 *      Get the original outer priority to remarkd outer 802.1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 * Output:
 *      dot1p_pri - remarked outer 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_outer1pRmkSrc_t
 *      - OUTER_1P_RMK_SRC_INT_PRI
 *      - OUTER_1P_RMK_SRC_USER_PRI
 */
int32
dal_maple_qos_outer1pRemarking_get(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t *pPri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch(src)
    {
        case OUTER_1P_RMK_SRC_INT_PRI:
        case OUTER_1P_RMK_SRC_USER_PRI:
            return dal_maple_qos_outer1pRemark_get(unit, val.pri.val, pPri);
        default:
            return RT_ERR_FAILED;
    }
}

/* Function Name:
 *      dal_maple_qos_outer1pRemarking_set
 * Description:
 *      Set the original outer priority to remarkd outer 802.1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 *      pri     - remarked outer 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_qos_outer1pRmkSrc_t
 *      - OUTER_1P_RMK_SRC_INT_PRI
 *      - OUTER_1P_RMK_SRC_USER_PRI
 */
int32
dal_maple_qos_outer1pRemarking_set(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t pri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch(src)
    {
        case OUTER_1P_RMK_SRC_INT_PRI:
        case OUTER_1P_RMK_SRC_USER_PRI:
            return dal_maple_qos_outer1pRemark_set(unit, val.pri.val, pri);
        default:
            return RT_ERR_FAILED;
    }
}

/* Function Name:
 *      dal_maple_qos_portOuter1pRemarkSrcSel_get
 * Description:
 *      Get the remarking source of outer dot1p priority remarking.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pType - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_portOuter1pRemarkSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pRmkSrc_t *pType)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_RMK_PORT_OPRI_SRC_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_OPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = OUTER_1P_RMK_SRC_INT_PRI;
    else
        *pType = OUTER_1P_RMK_SRC_USER_PRI;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portOuter1pRemarkSrcSel_get */

/* Function Name:
 *      dal_maple_qos_portOuter1pRemarkSrcSel_set
 * Description:
 *      Set the remarking source of outer dot1p priority remarking.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - remarking source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_maple_qos_portOuter1pRemarkSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pRmkSrc_t type)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type >= OUTER_1P_RMK_SRC_END, RT_ERR_INPUT);

    switch(type)
    {
        case OUTER_1P_RMK_SRC_INT_PRI:
            value = 0;
            break;
        case OUTER_1P_RMK_SRC_USER_PRI:
            value = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, MAPLE_RMK_PORT_OPRI_SRC_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_OPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portOuter1pRemarkSrcSel_set */

/* Function Name:
 *      dal_maple_qos_portOuter1pDfltPriSrcSel_get
 * Description:
 *      Get default outer-priority source of specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pType - default outer dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_portOuter1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pType)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_RMK_PORT_OPRI_SRC_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_OPRI_DFLT_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = OUTER_1P_DFLT_SRC_USER_PRI;
    else if (1 == value)
        *pType = OUTER_1P_DFLT_SRC_PB_OUTER_PRI;
    else if (2 == value)
        *pType = OUTER_1P_DFLT_SRC_DFLT_PRI;
    else if (3 == value)
        *pType = OUTER_1P_DFLT_SRC_INT_PRI;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portOuter1pDfltPriSrcSel_get */

/* Function Name:
 *      dal_maple_qos_portOuter1pDfltPriSrcSel_set
 * Description:
 *      Set default outer-priority source of specified port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - default outer dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_maple_qos_portOuter1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t type)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type != OUTER_1P_DFLT_SRC_USER_PRI && type != OUTER_1P_DFLT_SRC_PB_OUTER_PRI &&
        type != OUTER_1P_DFLT_SRC_DFLT_PRI && type != OUTER_1P_DFLT_SRC_INT_PRI, RT_ERR_INPUT);

    if (OUTER_1P_DFLT_SRC_USER_PRI == type)
        value = 0;
    else if (OUTER_1P_DFLT_SRC_PB_OUTER_PRI == type)
        value = 1;
    else if (OUTER_1P_DFLT_SRC_DFLT_PRI == type)
        value = 2;
    else if (OUTER_1P_DFLT_SRC_INT_PRI == type)
        value = 3;

    QOS_SEM_LOCK(unit);

    /* program value to chip */
    if ((ret = reg_array_field_write(unit, MAPLE_RMK_PORT_OPRI_SRC_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_OPRI_DFLT_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portOuter1pDfltPriSrcSel_set */

/* Function Name:
 *      dal_maple_qos_outer1pDfltPri_get
 * Description:
 *      Get default outer-priority value
 * Input:
 *      unit       - unit id
 * Output:
 *      pDot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_outer1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_RMK_CTRLr, MAPLE_OPRI_DFLT_PRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_maple_qos_outer1pDfltPri_get */

/* Function Name:
 *      dal_maple_qos_outer1pDfltPri_set
 * Description:
 *      Set default outer priority value
 * Input:
 *      unit      - unit id
 *      dot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid dot1p priority
 * Note:
 *      None.
 */
int32
dal_maple_qos_outer1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d", unit, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to chip */
    if ((ret = reg_field_write(unit, MAPLE_RMK_CTRLr, MAPLE_OPRI_DFLT_PRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
    } /* end of dal_maple_qos_outer1pDfltPri_set */

/* Function Name:
 *      dal_maple_qos_portDscpRemarkEnable_get
 * Description:
 *      Get DSCP remark status for a port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of DSCP remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_portDscpRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    osal_memset(&portmask, 0, sizeof(portmask));

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DSCP_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portDscpRemarkEnable_get */

/* Function Name:
 *      dal_maple_qos_portDscpRemarkEnable_set
 * Description:
 *      Set DSCP remark status for a port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of DSCP remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_portDscpRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    osal_memset(&portmask, 0, sizeof(portmask));

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MAPLE_DSCP_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portDscpRemarkEnable_set */

/* Function Name:
 *      dal_maple_qos_dscpRemarking_get
 * Description:
 *      Get the internal priority/dscp to remarkd DSCP mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 * Output:
 *      pDscp    - remarked DSCP value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_qos_dscpRemarking_get(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 *pDscp)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch (src)
    {   /* In ASIC, only one remarking table. */
        case DSCP_RMK_SRC_INT_PRI:
            return dal_maple_qos_dscpRemark_get(unit, val.pri.val, pDscp);
        case DSCP_RMK_SRC_DSCP:
            return dal_maple_qos_dscp2DscpRemark_get(unit, val.dscp.val, pDscp);
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_dscpRemarking_set
 * Description:
 *      Set the internal priority/dscp to remarkd DSCP mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 *      dscp    - dscp value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      None
 */
int32
dal_maple_qos_dscpRemarking_set(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 dscp)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch (src)
    {   /* In ASIC, only one remarking table. */
        case DSCP_RMK_SRC_INT_PRI:
            return dal_maple_qos_dscpRemark_set(unit, val.pri.val, dscp);
        case DSCP_RMK_SRC_DSCP:
            return dal_maple_qos_dscp2DscpRemark_set(unit, val.dscp.val, dscp);
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_dscpRemark_get
 * Description:
 *      Get the internal priority (3bits) to remarked DSCP mapping.
 * Input:
 *      unit    - unit id
 *      int_pri - internal priority value (range from 0 ~ 7)
 * Output:
 *      pDscp   - remarked DSCP value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_dscpRemark_get(uint32 unit, rtk_pri_t int_pri, uint32 *pDscp)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d", unit, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(int_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    MAPLE_DSCPf, pDscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDscp=%d", *pDscp);

    return RT_ERR_OK;
} /* end of dal_maple_qos_dscpRemark_get */

/* Function Name:
 *      dal_maple_qos_dscpRemark_set
 * Description:
 *      Set the internal priority (3bits) to remarked DSCP mapping.
 * Input:
 *      unit    - unit id
 *      int_pri - internal priority value (range from 0 ~ 7)
 *      dscp    - remarked DSCP value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 * Note:
 *      DSCP remark functionality can map the internal priority to DSCP before a packet is going
 *      to be transmited.
 */
int32
dal_maple_qos_dscpRemark_set(uint32 unit, rtk_pri_t int_pri, uint32 dscp)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d, dscp=%d", unit, int_pri, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(int_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    MAPLE_DSCPf, &dscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_dscpRemark_set */

/* Function Name:
 *      dal_maple_qos_dscp2DscpRemark_get
 * Description:
 *      Get DSCP to remarked DSCP mapping.
 * Input:
 *      unit  - unit id
 *      dscp  - DSCP value
 * Output:
 *      pDscp - remarked DSCP value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - Invalid dscp value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_dscp2DscpRemark_get(uint32 unit, uint32 dscp, uint32 *pDscp)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d", unit, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    MAPLE_DSCPf, pDscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDscp=%d", *pDscp);

    return RT_ERR_OK;
} /* end of dal_maple_qos_dscp2DscpRemark_get */

/* Function Name:
 *      dal_maple_qos_dscp2DscpRemark_set
 * Description:
 *      Set DSCP to remarked DSCP mapping.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value
 *      rmkDscp - remarked DSCP value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - Invalid dscp value
 * Note:
 *      None.
 */
int32
dal_maple_qos_dscp2DscpRemark_set(uint32 unit, uint32 dscp, uint32 rmkDscp)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d, rmkDscp=%d",
           unit, dscp, rmkDscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK(rmkDscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    MAPLE_DSCPf, &rmkDscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_dscp2DscpRemark_set */

/* Function Name:
 *      dal_maple_qos_dscpRemarkSrcSel_get
 * Description:
 *      Get the remarking source of DSCP remarking.
 * Input:
 *      unit  - unit id
 * Output:
 *      pType - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_dscpRemarkSrcSel_get(uint32 unit, rtk_qos_dscpRmkSrc_t *pType)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_RMK_CTRLr, MAPLE_DSCP_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = DSCP_RMK_SRC_INT_PRI;
    else if (1 == value)
        *pType = DSCP_RMK_SRC_DSCP;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_maple_qos_dscpRemarkSrcSel_get */

/* Function Name:
 *      dal_maple_qos_dscpRemarkSrcSel_set
 * Description:
 *      Set the remarking source of DSCP remarking.
 * Input:
 *      unit - unit id
 *      type - remarking source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_maple_qos_dscpRemarkSrcSel_set(uint32 unit, rtk_qos_dscpRmkSrc_t type)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d",
           unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type != DSCP_RMK_SRC_INT_PRI && type != DSCP_RMK_SRC_DSCP, RT_ERR_INPUT);

    if (DSCP_RMK_SRC_INT_PRI == type)
        value = 0;
    else if (DSCP_RMK_SRC_DSCP == type)
        value = 1;

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_field_write(unit, MAPLE_RMK_CTRLr, MAPLE_DSCP_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_dscpRemarkSrcSel_set */

/* Function Name:
 *      dal_maple_qos_schedulingAlgorithm_get
 * Description:
 *      Get the scheduling algorithm of the port.
 * Input:
 *      unit             - unit id
 *      port             - port id
 * Output:
 *      pScheduling_type - type of scheduling algorithm.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The types of scheduling algorithm:
 *      - WFQ
 *      - WRR
 */
int32
dal_maple_qos_schedulingAlgorithm_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   *pScheduling_type)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pScheduling_type), RT_ERR_NULL_POINTER);

    *pScheduling_type = portSche_type[unit][port];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pScheduling_type=%d", *pScheduling_type);

    return RT_ERR_OK;
} /* end of dal_maple_qos_schedulingAlgorithm_get */

/* Function Name:
 *      dal_maple_qos_schedulingAlgorithm_set
 * Description:
 *      Set the scheduling algorithm of the port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      scheduling_type - type of scheduling algorithm.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_QOS_SCHE_TYPE - Error scheduling algorithm type
 * Note:
 *      The types of scheduling algorithm:
 *      - WFQ
 *      - WRR
 */
int32
dal_maple_qos_schedulingAlgorithm_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   scheduling_type)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, scheduling_type=%d",
           unit, port, scheduling_type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(scheduling_type >= SCHEDULING_TYPE_END, RT_ERR_QOS_SCHE_TYPE);

    switch (scheduling_type)
    {
        case WFQ:
            value = 0;
            break;
        case WRR:
            value = 1;
            break;
        default:
            return RT_ERR_QOS_SCHE_TYPE;
    }

    QOS_SEM_LOCK(unit);
    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_SCHED_P_TYPE_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_SCHED_TYPEf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    portSche_type[unit][port] = scheduling_type;

    if ((ret = dal_maple_rate_portScheChange_set(unit, port, scheduling_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_qos_schedulingAlgorithm_set */

/* Function Name:
 *      dal_maple_qos_schedulingQueue_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pQweights - the array of weights for WRR/WFQ queue (valid:1~128, 0 for STRICT_PRIORITY queue)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *    If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 */
int32
dal_maple_qos_schedulingQueue_get(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32       ret;
    rtk_qid_t   qid;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP */
    for (qid = 0; qid <= (HAL_MAX_NUM_OF_QUEUE(unit) - 1); qid++)
    {
        if ((ret = reg_array_field_read(unit, MAPLE_SCHED_Q_CTRLr, port, qid, MAPLE_Q_WEIGHTf, &pQweights->weights[qid])) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pQweights[0]=%d, pQweights[1]=%d, pQweights[2]=%d\
           pQweights[3]=%d, pQweights[4]=%d, pQweights[5]=%d, pQweights[6]=%d, pQweights[7]=%d",
           pQweights->weights[0],
           pQweights->weights[1],
           pQweights->weights[2],
           pQweights->weights[3],
           pQweights->weights[4],
           pQweights->weights[5],
           pQweights->weights[6],
           pQweights->weights[7]);

    return RT_ERR_OK;
} /* end of dal_maple_qos_schedulingQueue_get */

/* Function Name:
 *      dal_maple_qos_schedulingQueue_set
 * Description:
 *      Set the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      pQweights - the array of weights for WRR/WFQ queue (valid:0~1023, 0 for STRICT_PRIORITY queue)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - Invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight
 * Note:
 *    The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *    If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 */
int32
dal_maple_qos_schedulingQueue_set(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32       ret;
    rtk_qid_t   qid;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);
    for (qid = 0; qid <= (HAL_MAX_NUM_OF_QUEUE(unit) - 1); qid++)
    {
        RT_PARAM_CHK(pQweights->weights[qid] > HAL_QUEUE_WEIGHT_MAX(unit), RT_ERR_QOS_QUEUE_WEIGHT);
    }

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pQweights[0]=%d, \
           pQweights[1]=%d, pQweights[2]=%d, pQweights[3]=%d, pQweights[4]=%d, pQweights[5]=%d, pQweights[6]=%d, \
           pQweights[7]=%d",
           pQweights->weights[0],
           pQweights->weights[1],
           pQweights->weights[2],
           pQweights->weights[3],
           pQweights->weights[4],
           pQweights->weights[5],
           pQweights->weights[6],
           pQweights->weights[7]);

    QOS_SEM_LOCK(unit);

    for (qid = 0; qid <= (HAL_MAX_NUM_OF_QUEUE(unit) - 1); qid++)
    {
        if ((ret = reg_array_field_write(unit, MAPLE_SCHED_Q_CTRLr, port, qid, MAPLE_Q_WEIGHTf, &pQweights->weights[qid])) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;

} /* end of dal_maple_qos_schedulingQueue_set */

/* Function Name:
 *      dal_maple_qos_portAvbStreamReservationClassEnable_get
 * Description:
 *      Destroy one specified mstp instance from the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      srClass - stream class
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT                 - invalid port id
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 *      RT_ERR_AVB_INVALID_SR_CLASS - Invalid SR Class
 * Note:
 *      None
 */
int32
dal_maple_qos_portAvbStreamReservationClassEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            *pEnable)
{
    uint32  en = 0;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, srClass=%d",
           unit, port, srClass);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((srClass >= AVB_SR_CLASS_END), RT_ERR_AVB_INVALID_SR_CLASS);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    switch (srClass)
    {
        case AVB_SR_CLASS_A:
            RT_IF_ERR_GOTO_HANDLE(reg_array_field_read(unit, MAPLE_AVB_PORT_CLASS_A_ENr, port, REG_ARRAY_INDEX_NONE, MAPLE_ENf, &en), errHandle, ret);
            break;
        case AVB_SR_CLASS_B:
            RT_IF_ERR_GOTO_HANDLE(reg_array_field_read(unit, MAPLE_AVB_PORT_CLASS_B_ENr, port, REG_ARRAY_INDEX_NONE, MAPLE_ENf, &en), errHandle, ret);
            break;
        default:
            break;
    }

    *pEnable = (en)? ENABLED : DISABLED;

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    QOS_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    return ret;
} /* end of dal_maple_qos_portAvbStreamReservationClassEnable_get */

/* Function Name:
 *      dal_maple_qos_portAvbStreamReservationClassEnable_set
 * Description:
 *      Set status of the specified stream class of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      srClass - stream class
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT                 - invalid port id
 *      RT_ERR_INPUT                - invalid input parameter
 *      RT_ERR_AVB_INVALID_SR_CLASS - Invalid SR Class
 * Note:
 *      None
 */
int32
dal_maple_qos_portAvbStreamReservationClassEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            enable)
{
    uint32  en;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, srClass=%d, enable=%d",
           unit, port, srClass, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((srClass >= AVB_SR_CLASS_END), RT_ERR_AVB_INVALID_SR_CLASS);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    en = (ENABLED == enable)? 1 : 0;

    QOS_SEM_LOCK(unit);

    switch (srClass)
    {
        case AVB_SR_CLASS_A:
            RT_IF_ERR_GOTO_HANDLE(reg_array_field_write(unit, MAPLE_AVB_PORT_CLASS_A_ENr, port, REG_ARRAY_INDEX_NONE, MAPLE_ENf, &en), errHandle, ret);
            RT_IF_ERR_GOTO_HANDLE(reg_array_field_write(unit, MAPLE_AVB_PORT_CLASS_A_EN_MACr, port, REG_ARRAY_INDEX_NONE, MAPLE_ENf, &en), errHandle, ret);
            break;
        case AVB_SR_CLASS_B:
            RT_IF_ERR_GOTO_HANDLE(reg_array_field_write(unit, MAPLE_AVB_PORT_CLASS_B_ENr, port, REG_ARRAY_INDEX_NONE, MAPLE_ENf, &en), errHandle, ret);
            RT_IF_ERR_GOTO_HANDLE(reg_array_field_write(unit, MAPLE_AVB_PORT_CLASS_B_EN_MACr, port, REG_ARRAY_INDEX_NONE, MAPLE_ENf, &en), errHandle, ret);
            break;
        default:
            break;
    }

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    QOS_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    return ret;
} /* end of dal_maple_qos_portAvbStreamReservationClassEnable_set */

/* Function Name:
 *      dal_maple_qos_avbStreamReservationConfig_get
 * Description:
 *      Get the configuration of Stream Reservation in the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pSrConf - pointer buffer of configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_qos_avbStreamReservationConfig_get(uint32 unit, rtk_qos_avbSrConf_t *pSrConf)
{
    uint32  reg_val=0, val=0;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSrConf), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* MAPLE_AVB_CTRLr */
    RT_IF_ERR_GOTO_HANDLE(reg_read(unit, MAPLE_AVB_CTRLr, &reg_val), errHandle, ret);

    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_A_PRIf, &val, &reg_val), errHandle, ret);
    pSrConf->class_a_priority = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_B_PRIf, &val, &reg_val), errHandle, ret);
    pSrConf->class_b_priority = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_A_QIDf, &val, &reg_val), errHandle, ret);
    pSrConf->class_a_queue_id = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_B_QIDf, &val, &reg_val), errHandle, ret);
    pSrConf->class_b_queue_id = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_NON_A_REDIRECT_QIDf, &val, &reg_val), errHandle, ret);
    pSrConf->class_non_a_redirect_queue_id = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_NON_B_REDIRECT_QIDf, &val, &reg_val), errHandle, ret);
    pSrConf->class_non_b_redirect_queue_id = val & 0x7;

    /* MAPLE_AVB_CTRL_MACr */
    RT_IF_ERR_GOTO_HANDLE(reg_read(unit, MAPLE_AVB_CTRL_MACr, &reg_val), errHandle, ret);

    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, MAPLE_AVB_CTRL_MACr, MAPLE_CLASS_NON_A_RMK_PRIf, &val, &reg_val), errHandle, ret);
    pSrConf->class_non_a_remark_priority = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, MAPLE_AVB_CTRL_MACr, MAPLE_CLASS_NON_B_RMK_PRIf, &val, &reg_val), errHandle, ret);
    pSrConf->class_non_b_remark_priority = val & 0x7;

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    QOS_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    return ret;
} /* end of dal_maple_qos_avbStreamReservationConfig_get */

/* Function Name:
 *      dal_maple_qos_avbStreamReservationConfig_set
 * Description:
 *      Set the configuration of Stream Reservation in the specified device.
 * Input:
 *      unit    - unit id
 *      pSrConf - pointer buffer of configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_QOS_1P_PRIORITY - Invalid dot1p priority
 *      RT_ERR_QUEUE_ID        - invalid queue id
 * Note:
 *      None
 */
int32
dal_maple_qos_avbStreamReservationConfig_set(uint32 unit, rtk_qos_avbSrConf_t *pSrConf)
{
    uint32  reg_val=0, val=0;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSrConf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pSrConf->class_a_priority > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(pSrConf->class_b_priority > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(pSrConf->class_non_a_remark_priority > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(pSrConf->class_non_b_remark_priority > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((pSrConf->class_a_queue_id >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pSrConf->class_b_queue_id >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pSrConf->class_non_a_redirect_queue_id >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pSrConf->class_non_b_redirect_queue_id >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);

    QOS_SEM_LOCK(unit);

    /* MAPLE_AVB_CTRLr */
    RT_IF_ERR_GOTO_HANDLE(reg_read(unit, MAPLE_AVB_CTRLr, &reg_val), errHandle, ret);

    val = pSrConf->class_a_priority & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_A_PRIf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_b_priority & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_B_PRIf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_a_queue_id & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_A_QIDf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_b_queue_id & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_B_QIDf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_non_a_redirect_queue_id & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_NON_A_REDIRECT_QIDf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_non_b_redirect_queue_id & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, MAPLE_AVB_CTRLr, MAPLE_CLASS_NON_B_REDIRECT_QIDf, &val, &reg_val), errHandle, ret);

    RT_IF_ERR_GOTO_HANDLE(reg_write(unit, MAPLE_AVB_CTRLr, &reg_val), errHandle, ret);
    RT_IF_ERR_GOTO_HANDLE(reg_write(unit, MAPLE_AVB_CTRL_ALEr, &reg_val), errHandle, ret);

    /* MAPLE_AVB_CTRL_MACr */
    RT_IF_ERR_GOTO_HANDLE(reg_read(unit, MAPLE_AVB_CTRL_MACr, &reg_val), errHandle, ret);

    val = pSrConf->class_non_a_remark_priority & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, MAPLE_AVB_CTRL_MACr, MAPLE_CLASS_NON_A_RMK_PRIf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_non_b_remark_priority & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, MAPLE_AVB_CTRL_MACr, MAPLE_CLASS_NON_B_RMK_PRIf, &val, &reg_val), errHandle, ret);

    RT_IF_ERR_GOTO_HANDLE(reg_write(unit, MAPLE_AVB_CTRL_MACr, &reg_val), errHandle, ret);

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    QOS_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    return ret;
} /* end of dal_maple_qos_avbStreamReservationConfig_set */

/* Function Name:
 *      dal_maple_qos_rspanPriRemap_get
 * Description:
 *      Get the internal priority that rspan tag  priority remap.
 * Input:
 *      unit      - unit id
 *      rspan_pri - rspan tag  priority value (range from 0 ~ 7)
 * Output:
 *      pInt_pri  - internal priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - Invalid rspan tag  priority
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_rspanPriRemap_get(uint32 unit, rtk_pri_t rspan_pri, rtk_pri_t *pInt_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, rspan_pri=%d",
           unit, rspan_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(rspan_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((NULL == pInt_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_RSPAN_REMAPr, REG_ARRAY_INDEX_NONE,
                    rspan_pri, MAPLE_INTPRI_RSPANf, pInt_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pInt_pri=%d", *pInt_pri);

    return RT_ERR_OK;
} /* end of dal_maple_qos_rspanPriRemap_get */

/* Function Name:
 *      dal_maple_qos_rspanPriRemap_set
 * Description:
 *      Set the internal priority that rspan tag  priority remap..
 * Input:
 *      unit      - unit id
 *      dot1p_pri - rspan tag  priority value (range from 0 ~ 7)
 *      int_pri   - internal priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid rspan tag  priority
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *      None.
 */
int32
dal_maple_qos_rspanPriRemap_set(uint32 unit, rtk_pri_t rspan_pri, rtk_pri_t int_pri)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, rspan_pri=%d, int_pri=%d",
           unit, rspan_pri, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(rspan_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    val = int_pri;
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_RSPAN_REMAPr, REG_ARRAY_INDEX_NONE,
                    rspan_pri, MAPLE_INTPRI_RSPANf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_rspanPriRemap_set */

/* Function Name:
 *      dal_maple_qos_portInnerPri_get
 * Description:
 *      Get inner priority of one port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pPri - inner priority assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    None.
 */
int32
dal_maple_qos_portInnerPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_PORT_PRIr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_INTPRI_PORTf, pPri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pPri);
    return RT_ERR_OK;
} /* end of dal_maple_qos_portInnerPri_get */

/* Function Name:
 *      dal_maple_qos_portInnerPri_set
 * Description:
 *      Set inner priority of one port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      pri  - inner priority assigment for specific port. (range from 0 ~ 7, 7 is
 *                the highest prioirty)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - Invalid port id
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *    none
 */
int32
dal_maple_qos_portInnerPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, pri=%d",
           unit, port, pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_PORT_PRIr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_INTPRI_PORTf, &pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_PORT_PRI_MACr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_INTPRI_PORTf, &pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portInnerPri_set */

/* Function Name:
 *      dal_maple_qos_portOuterPri_get
 * Description:
 *      Get outer priority of one port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pPri - outer priority assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    None.
 */
int32
dal_maple_qos_portOuterPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_PRI_SEL_PORT_OTAG_PRIr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_INTPRI_PORTf, pPri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pPri);
    return RT_ERR_OK;
} /* end of dal_maple_qos_portOuterPri_get */

/* Function Name:
 *      dal_maple_qos_portOuterPri_set
 * Description:
 *      Set outer priority of one port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pPri - outer priority assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    None.
 */
int32
dal_maple_qos_portOuterPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, pri=%d",
           unit, port, pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_PORT_OTAG_PRIr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_INTPRI_PORTf, &pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_PRI_SEL_PORT_OTAG_PRI_MACr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_INTPRI_PORTf, &pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portOuterPri_set */

/* Function Name:
 *      dal_maple_qos_portPri2IgrQMapEnable_get
 * Description:
 *      Get priority to input queue mapping ability.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_portPri2IgrQMapEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port = %d,", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_MAP_TBL_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == enable)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);
    return RT_ERR_OK;
} /* end of dal_maple_qos_portPri2IgrQMapEnable_get */

/* Function Name:
 *      dal_maple_qos_portPri2IgrQMapEnable_set
 * Description:
 *      Set priority to input queue mapping ability.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - ability
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid enable value
 * Note:
 *      None.
 */
int32
dal_maple_qos_portPri2IgrQMapEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port = %d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    /* translate definition to chip's value  */
    if (ENABLED == enable)
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_MAP_TBL_ENf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portPri2IgrQMapEnable_set */

/* Function Name:
 *      dal_maple_qos_portPri2IgrQMap_get
 * Description:
 *      Get the value of internal priority to QID mapping table.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPri2qid - array of internal priority on a queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_portPri2IgrQMap_get(uint32 unit, rtk_port_t port, rtk_qos_pri2queue_t *pPri2qid)
{
    int32       ret;
    rtk_pri_t   pri;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    for (pri = 0; pri <= HAL_INTERNAL_PRIORITY_MAX(unit); pri++)
    {
        switch(pri)
        {
            case 0:
                if ((ret = reg_array_field_read(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI0_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 1:
                if ((ret = reg_array_field_read(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI1_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 2:
                if ((ret = reg_array_field_read(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI2_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 3:
                if ((ret = reg_array_field_read(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI3_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 4:
                if ((ret = reg_array_field_read(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI4_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 5:
                if ((ret = reg_array_field_read(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI5_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 6:
                if ((ret = reg_array_field_read(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI6_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 7:
                if ((ret = reg_array_field_read(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI7_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            default:
                break;
        }
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri2qid[0]=%d, pPri2qid[1]=%d, pPri2qid[2]=%d\
           pPri2qid[3]=%d, pPri2qid[4]=%d, pPri2qid[5]=%d, pPri2qid[6]=%d, pPri2qid[7]=%d",
           pPri2qid->pri2queue[0],
           pPri2qid->pri2queue[1],
           pPri2qid->pri2queue[2],
           pPri2qid->pri2queue[3],
           pPri2qid->pri2queue[4],
           pPri2qid->pri2queue[5],
           pPri2qid->pri2queue[6],
           pPri2qid->pri2queue[7]);
    return RT_ERR_OK;
} /* end of dal_maple_qos_portPri2IgrQMap_get */

/* Function Name:
 *      dal_maple_qos_portPri2IgrQMap_set
 * Description:
 *      Set the value of internal priority to QID mapping table.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPri2qid - array of internal priority on a queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_portPri2IgrQMap_set(uint32 unit, rtk_port_t port, rtk_qos_pri2queue_t *pPri2qid)
{
    int32       ret;
    rtk_pri_t   pri;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

    for (pri = 0; pri <= HAL_INTERNAL_PRIORITY_MAX(unit); pri++)
    {
        RT_PARAM_CHK((pPri2qid->pri2queue[pri] > (HAL_MAX_NUM_OF_IGR_QUEUE(unit) -1)), RT_ERR_QUEUE_ID);
    }

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "Pri2qid[0]=%d, pPri2qid[1]=%d, pPri2qid[2]=%d\
           pPri2qid[3]=%d, pPri2qid[4]=%d, pPri2qid[5]=%d, pPri2qid[6]=%d, pPri2qid[7]=%d",
           pPri2qid->pri2queue[0],
           pPri2qid->pri2queue[1],
           pPri2qid->pri2queue[2],
           pPri2qid->pri2queue[3],
           pPri2qid->pri2queue[4],
           pPri2qid->pri2queue[5],
           pPri2qid->pri2queue[6],
           pPri2qid->pri2queue[7]);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    for (pri = 0; pri <= HAL_INTERNAL_PRIORITY_MAX(unit); pri++)
    {
        switch (pri)
        {
            case 0:
                if ((ret = reg_array_field_write(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI0_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 1:
                if ((ret = reg_array_field_write(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI1_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 2:
                if ((ret = reg_array_field_write(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI2_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 3:
                if ((ret = reg_array_field_write(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI3_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 4:
                if ((ret = reg_array_field_write(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI4_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 5:
                if ((ret = reg_array_field_write(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI5_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 6:
                if ((ret = reg_array_field_write(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI6_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            case 7:
                if ((ret = reg_array_field_write(unit, MAPLE_P_IGR_Q_MAP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI7_QNUMf, &pPri2qid->pri2queue[pri]) )!= RT_ERR_OK)
                {
                    QOS_SEM_UNLOCK(unit);
                    return ret;
                }
                break;
            default:
                break;
        }
    }

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_qos_portPri2IgrQMap_set */

/* Function Name:
 *      dal_maple_qos_pkt2CpuPriRemap_get
 * Description:
 *      Get the internal priority to cpu port priority mapping
 * Input:
 *      unit     - unit id
 *      int_pri  - internal priority value (range from 0 ~ 7)
 * Output:
 *      pCpu_pri - to cpu port priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid internal priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_pkt2CpuPriRemap_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pCpu_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d", unit, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((NULL == pCpu_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    switch(int_pri)
    {
        case 0:
            if ((ret = reg_field_read(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI0f, pCpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 1:
            if ((ret = reg_field_read(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI1f, pCpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 2:
            if ((ret = reg_field_read(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI2f, pCpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 3:
            if ((ret = reg_field_read(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI3f, pCpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 4:
            if ((ret = reg_field_read(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI4f, pCpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 5:
            if ((ret = reg_field_read(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI5f, pCpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 6:
            if ((ret = reg_field_read(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI6f, pCpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 7:
            if ((ret = reg_field_read(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI7f, pCpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        default:
            break;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pCpu_pri=%d", *pCpu_pri);
    return RT_ERR_OK;
} /* end of dal_maple_qos_1pPriRemap_get */

/* Function Name:
 *      dal_maple_qos_pkt2CpuPriRemap_set
 * Description:
 *      Set the internal priority to cpu port priority mapping
 * Input:
 *      unit    - unit id
 *      int_pri - internal priority value (range from 0 ~ 7)
 *      cpu_pir - internal priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *      None.
 */
int32
dal_maple_qos_pkt2CpuPriRemap_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t cpu_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d, cpu_pri=%d",
           unit, int_pri, cpu_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK(cpu_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP */
    switch(int_pri)
    {
        case 0:
            if ((ret = reg_field_write(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI0f, &cpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 1:
            if ((ret = reg_field_write(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI1f, &cpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 2:
            if ((ret = reg_field_write(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI2f, &cpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 3:
            if ((ret = reg_field_write(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI3f, &cpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 4:
            if ((ret = reg_field_write(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI4f, &cpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 5:
            if ((ret = reg_field_write(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI5f, &cpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 6:
            if ((ret = reg_field_write(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI6f, &cpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        case 7:
            if ((ret = reg_field_write(unit, MAPLE_QM_PKT2CPU_INTPRI_MAPr,MAPLE_PRI7f, &cpu_pri)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
            break;
        default:
            break;
    }

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_maple_qos_1pPriRemap_set */

/* Function Name:
 *      dal_maple_qos_congAvoidAlgo_get
 * Description:
 *      Get algorithm of congestion avoidance.
 * Input:
 *      unit  - unit id
 * Output:
 *      pAlgo - pointer to algorithm of congestion avoidance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SRED
 *      - CONG_AVOID_TD
 */
int32
dal_maple_qos_congAvoidAlgo_get(uint32 unit, rtk_qos_congAvoidAlgo_t *pAlgo)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAlgo), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_DROP_ALGO_GLB_CTRLr, MAPLE_DROP_TYPE_SELf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pAlgo = CONG_AVOID_TD;
    else
        *pAlgo = CONG_AVOID_SRED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "drop_type=0x%x", *pAlgo);

    return RT_ERR_OK;
} /* end of dal_maple_qos_congAvoidAlgo_get */

/* Function Name:
 *      dal_maple_qos_congAvoidAlgo_set
 * Description:
 *      Set algorithm of congestion avoidance.
 * Input:
 *      unit - unit id
 *      algo - algorithm of congestion avoidance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SRED
 *      - CONG_AVOID_TD
 */
int32
dal_maple_qos_congAvoidAlgo_set(uint32 unit, rtk_qos_congAvoidAlgo_t algo)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, algo=%d", unit, algo);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    if (CONG_AVOID_TD == algo)
        value = 0;
    else if (CONG_AVOID_SRED == algo)
        value = 1;
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_DROP_ALGO_GLB_CTRLr, MAPLE_DROP_TYPE_SELf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_congAvoidAlgo_set */

/* Function Name:
 *      dal_maple_qos_portIgrQueueWeight_get
 * Description:
 *      Get the weight of ingress queue.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      queue  - queue id
 * Output:
 *      pQweights -the weigh of specified queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    If weight == 0, means the queue is STRICT_PRIORITY
 */
int32
dal_maple_qos_portIgrQueueWeight_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pQweight)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) -1)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pQweight), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_IGR_BWCTRL_WT_CTRLr, port, queue, MAPLE_WEIGHTf, pQweight)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pQweight=%d", *pQweight);

    return RT_ERR_OK;
}/*dal_maple_qos_portIgrQueueWeight_get*/

/* Function Name:
 *      dal_maple_qos_portIgrQueueWeight_set
 * Description:
 *      Get the weight of ingress queue.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      queue  - queue id
 *      qWeights -the weigh of specified queue
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight
 * Note:
 *    If weight == 0, means the queue is STRICT_PRIORITY
 */
int32
dal_maple_qos_portIgrQueueWeight_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 qWeight)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d, qWeight=%d", unit, port, queue,qWeight);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) -1)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK(qWeight > HAL_QUEUE_WEIGHT_MAX(unit), RT_ERR_QOS_QUEUE_WEIGHT);

    QOS_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_IGR_BWCTRL_WT_CTRLr, port, queue, MAPLE_WEIGHTf, &qWeight)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/*dal_maple_qos_portIgrQueueWeight_set*/


/* Function Name:
 *      dal_maple_qos_invldDscpVal_get
 * Description:
 *      Get the invalid dscp value in the specified device
 * Input:
 *      unit      - unit id
 * Output:
 *      pDscp     - pointer to dscp value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_invldDscpVal_get(uint32 unit, uint32 *pDscp)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PRI_DSCP_INVLD_CTRL0r, MAPLE_DSCPf, pDscp)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDscp = %d", *pDscp);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_maple_qos_invldDscpVal_set
 * Description:
 *      Set the invalid dscp value in the specified device
 * Input:
 *      unit     - unit id
 *      dscp     - dscp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - input dscp out of range
 * Note:
 *      None.
 */
int32
dal_maple_qos_invldDscpVal_set(uint32 unit, uint32 dscp)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d", unit, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);

    QOS_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MAPLE_PRI_DSCP_INVLD_CTRL0r, MAPLE_DSCPf, &dscp)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_invldDscpMask_get
 * Description:
 *      Get the invalid dscp mask in the specified device
 * Input:
 *      unit      - unit id
 * Output:
 *      pDscpMask     - pointer to dscp mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_qos_invldDscpMask_get(uint32 unit, uint32 *pDscpMask)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscpMask), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PRI_DSCP_INVLD_CTRL0r, MAPLE_DSCP_MSKf, pDscpMask)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDscpMask = %d", *pDscpMask);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_invldDscpMask_set
 * Description:
 *      Set the invalid dscp mask in the specified device
 * Input:
 *      unit     - unit id
 *      dscpMask     - dscp mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - input dscp mask out of range
 * Note:
 *      None.
 */
int32
dal_maple_qos_invldDscpMask_set(uint32 unit, uint32 dscpMask)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d", unit, dscpMask);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscpMask > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_PRI_DSCP_INVLD_CTRL0r, MAPLE_DSCP_MSKf, &dscpMask)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_portInvldDscpEnable_get
 * Description:
 *      Get invalid DSCP status for a port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of invalid DSCP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of invalid DSCP:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_portInvldDscpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);


    QOS_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PRI_DSCP_INVLD_CTRL1r, MAPLE_INVLD_DSCP_PMSKf, &value)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    value &= (1<<port);

    *pEnable = (value == 0) ? DISABLED : ENABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable = %d", *pEnable);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_portInvldDscpEnable_set
 * Description:
 *      Set invalid DSCP status for a port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of invalid DSCP
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The status of invalid DSCP:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_portInvldDscpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32 value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PRI_DSCP_INVLD_CTRL1r, MAPLE_INVLD_DSCP_PMSKf, &value)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if(enable == ENABLED)
        value |= (1<<port);
    else
        value &= (~(1<<port));

    if ((ret = reg_field_write(unit, MAPLE_PRI_DSCP_INVLD_CTRL1r, MAPLE_INVLD_DSCP_PMSKf, &value)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_portInnerPriRemapEnable_get
 * Description:
 *      Get port-base inner priority remapping status
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - port-base inner priority remapping status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      port-base inner priority remapping status:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_portInnerPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PRI_DSCP_INVLD_CTRL0r, MAPLE_PB_IPRI_REMAP_ENf, &value)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    *pEnable = (value == 0) ? DISABLED : ENABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable = %d", *pEnable);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_portInnerPriRemapEnable_set
 * Description:
 *      Set port-base inner priority remapping status
 * Input:
 *      unit   - unit id
 *      enable - port-base inner priority remapping status
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      port-base inner priority remapping status:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_portInnerPriRemapEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32 value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    value = enable == ENABLED ? 1 : 0;

    if ((ret = reg_field_write(unit, MAPLE_PRI_DSCP_INVLD_CTRL0r, MAPLE_PB_IPRI_REMAP_ENf, &value)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_portOuterPriRemapEnable_get
 * Description:
 *      Get port-base outer priority remapping status
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - port-base outer priority remapping status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      port-base outer priority remapping status:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_portOuterPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PRI_DSCP_INVLD_CTRL0r, MAPLE_PB_OPRI_REMAP_ENf, &value)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    *pEnable = (value == 0) ? DISABLED : ENABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable = %d", *pEnable);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_portOuterPriRemapEnable_set
 * Description:
 *      Get port-base outer priority remapping status
 * Input:
 *      unit   - unit id
 *      enable - port-base outer priority remapping status
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      port-base outer priority remapping status:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_qos_portOuterPriRemapEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32 value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);


    QOS_SEM_LOCK(unit);

    value = enable == ENABLED ? 1 : 0;

    if ((ret = reg_field_write(unit, MAPLE_PRI_DSCP_INVLD_CTRL0r, MAPLE_PB_OPRI_REMAP_ENf, &value)))
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_qos_portQueueStrictEnable_get
 * Description:
 *      Get enable status of egress queue strict priority for specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pEnable - Pointer to enable status of egress queue strict priority.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_qos_portQueueStrictEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_SCHED_Q_CTRLr, port, queue, MAPLE_Q_STRICT_ENf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (value == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portQueueStrictEnable_get */

/* Function Name:
 *      dal_maple_qos_portQueueStrictEnable_set
 * Description:
 *      Set enable status of egress queue strict priority for specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      queue  - queue id
 *      enable - enable status of egress queue strict priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_qos_portQueueStrictEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d, enable=%d", unit, port, queue, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    QOS_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_SCHED_Q_CTRLr, port, queue, MAPLE_Q_STRICT_ENf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_qos_portQueueStrictEnable_set */

