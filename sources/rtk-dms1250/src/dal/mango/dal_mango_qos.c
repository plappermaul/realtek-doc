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
 * Purpose : None
 *
 * Feature : None
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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_qos.h>
#include <dal/mango/dal_mango_stack.h>
#include <dal/mango/dal_mango_port.h>
#include <dal/mango/dal_mango_common.h>
#include <rtk/default.h>
#include <rtk/qos.h>
#include <rtk/stack.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               qos_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         qos_sem[RTK_MAX_NUM_OF_UNIT];

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

/*
static int32 _dal_mango_qos_outputQueueNumber_set(uint32 unit, rtk_port_t port, uint32 qNum);
static int32 _dal_mango_qos_outputQueueNumber_get(uint32 unit, rtk_port_t port, uint32 *pQNum);
static int32 _dal_mango_qos_cpuQueueNum_set(uint32 unit, uint32 queue_num);
 */

/* Function Name:
 *      dal_mango_qosMapper_init
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
dal_mango_qosMapper_init(dal_mapper_t *pMapper)
{
    /* Qos function */
    pMapper->qos_init = dal_mango_qos_init;
    pMapper->qos_priSelGroup_get = dal_mango_qos_priSelGroup_get;
    pMapper->qos_priSelGroup_set = dal_mango_qos_priSelGroup_set;
    pMapper->qos_portPriSelGroup_get = dal_mango_qos_portPriSelGroup_get;
    pMapper->qos_portPriSelGroup_set = dal_mango_qos_portPriSelGroup_set;
    pMapper->qos_portDpSel_get = dal_mango_qos_portDpSel_get;
    pMapper->qos_portDpSel_set = dal_mango_qos_portDpSel_set;
    pMapper->qos_dpRemap_get = dal_mango_qos_dpRemap_get;
    pMapper->qos_dpRemap_set = dal_mango_qos_dpRemap_set;
    pMapper->qos_portPortPriRemapSel_get = dal_mango_qos_portPortPriRemapSel_get;
    pMapper->qos_portPortPriRemapSel_set = dal_mango_qos_portPortPriRemapSel_set;
    pMapper->qos_priRemap_get = dal_mango_qos_priRemap_get;
    pMapper->qos_priRemap_set = dal_mango_qos_priRemap_set;
    pMapper->qos_pri2QidMap_get = dal_mango_qos_pri2QidMap_get;
    pMapper->qos_pri2QidMap_set = dal_mango_qos_pri2QidMap_set;
    pMapper->qos_cpuQid2QidMap_get = dal_mango_qos_cpuQid2QidMap_get;
    pMapper->qos_cpuQid2QidMap_set = dal_mango_qos_cpuQid2QidMap_set;
    pMapper->qos_cpuQid2StackQidMap_get = dal_mango_qos_cpuQid2StackQidMap_get;
    pMapper->qos_cpuQid2StackQidMap_set = dal_mango_qos_cpuQid2StackQidMap_set;

    pMapper->qos_port1pRemarkEnable_get = dal_mango_qos_port1pRemarkEnable_get;
    pMapper->qos_port1pRemarkEnable_set = dal_mango_qos_port1pRemarkEnable_set;
    pMapper->qos_1pRemarking_get = dal_mango_qos_1pRemarking_get;
    pMapper->qos_1pRemarking_set = dal_mango_qos_1pRemarking_set;
    pMapper->qos_port1pDfltPri_get = dal_mango_qos_port1pDfltPri_get;
    pMapper->qos_port1pDfltPri_set = dal_mango_qos_port1pDfltPri_set;
    pMapper->qos_port1pDfltPriExt_get = dal_mango_qos_port1pDfltPriExt_get;
    pMapper->qos_port1pDfltPriExt_set = dal_mango_qos_port1pDfltPriExt_set;
    pMapper->qos_port1pDfltPriSrcSel_get = dal_mango_qos_port1pDfltPriSrcSel_get;
    pMapper->qos_port1pDfltPriSrcSel_set = dal_mango_qos_port1pDfltPriSrcSel_set;
    pMapper->qos_port1pDfltPriSrcSelExt_get = dal_mango_qos_port1pDfltPriSrcSelExt_get;
    pMapper->qos_port1pDfltPriSrcSelExt_set = dal_mango_qos_port1pDfltPriSrcSelExt_set;
    pMapper->qos_1pDfltPriCfgSrcSel_get = dal_mango_qos_1pDfltPriCfgSrcSel_get;
    pMapper->qos_1pDfltPriCfgSrcSel_set = dal_mango_qos_1pDfltPriCfgSrcSel_set;
    pMapper->qos_portOut1pRemarkEnable_get = dal_mango_qos_portOut1pRemarkEnable_get;
    pMapper->qos_portOut1pRemarkEnable_set = dal_mango_qos_portOut1pRemarkEnable_set;
    pMapper->qos_outer1pRemarking_get = dal_mango_qos_outer1pRemarking_get;
    pMapper->qos_outer1pRemarking_set = dal_mango_qos_outer1pRemarking_set;
    pMapper->qos_portOuter1pDfltPri_get = dal_mango_qos_portOuter1pDfltPri_get;
    pMapper->qos_portOuter1pDfltPri_set = dal_mango_qos_portOuter1pDfltPri_set;
    pMapper->qos_portOuter1pDfltPriExt_get = dal_mango_qos_portOuter1pDfltPriExt_get;
    pMapper->qos_portOuter1pDfltPriExt_set = dal_mango_qos_portOuter1pDfltPriExt_set;
    pMapper->qos_portOuter1pDfltPriSrcSel_get = dal_mango_qos_portOuter1pDfltPriSrcSel_get;
    pMapper->qos_portOuter1pDfltPriSrcSel_set = dal_mango_qos_portOuter1pDfltPriSrcSel_set;
    pMapper->qos_portOuter1pDfltPriSrcSelExt_get = dal_mango_qos_portOuter1pDfltPriSrcSelExt_get;
    pMapper->qos_portOuter1pDfltPriSrcSelExt_set = dal_mango_qos_portOuter1pDfltPriSrcSelExt_set;
    pMapper->qos_portDscpRemarkEnable_get = dal_mango_qos_portDscpRemarkEnable_get;
    pMapper->qos_portDscpRemarkEnable_set = dal_mango_qos_portDscpRemarkEnable_set;
    pMapper->qos_dscpRemarking_get = dal_mango_qos_dscpRemarking_get;
    pMapper->qos_dscpRemarking_set = dal_mango_qos_dscpRemarking_set;
    pMapper->qos_portDeiRemarkTagSel_get = dal_mango_qos_portDeiRemarkTagSel_get;
    pMapper->qos_portDeiRemarkTagSel_set = dal_mango_qos_portDeiRemarkTagSel_set;
    pMapper->qos_portDeiRemarkEnable_get = dal_mango_qos_portDeiRemarkEnable_get;
    pMapper->qos_portDeiRemarkEnable_set = dal_mango_qos_portDeiRemarkEnable_set;
    pMapper->qos_deiRemarking_get = dal_mango_qos_deiRemarking_get;
    pMapper->qos_deiRemarking_set = dal_mango_qos_deiRemarking_set;
    pMapper->qos_deiRemarkSrcSel_get = dal_mango_qos_deiRemarkSrcSel_get;
    pMapper->qos_deiRemarkSrcSel_set = dal_mango_qos_deiRemarkSrcSel_set;
    pMapper->qos_1pDfltPri_get = dal_mango_qos_1pDfltPri_get;
    pMapper->qos_1pDfltPri_set = dal_mango_qos_1pDfltPri_set;
    pMapper->qos_1pRemarkSrcSel_get = dal_mango_qos_1pRemarkSrcSel_get;
    pMapper->qos_1pRemarkSrcSel_set = dal_mango_qos_1pRemarkSrcSel_set;
    pMapper->qos_outer1pRemarkSrcSel_get = dal_mango_qos_outer1pRemarkSrcSel_get;
    pMapper->qos_outer1pRemarkSrcSel_set = dal_mango_qos_outer1pRemarkSrcSel_set;
    pMapper->qos_outer1pDfltPriCfgSrcSel_get = dal_mango_qos_outer1pDfltPriCfgSrcSel_get;
    pMapper->qos_outer1pDfltPriCfgSrcSel_set = dal_mango_qos_outer1pDfltPriCfgSrcSel_set;
    pMapper->qos_dscpRemarkSrcSel_get = dal_mango_qos_dscpRemarkSrcSel_get;
    pMapper->qos_dscpRemarkSrcSel_set = dal_mango_qos_dscpRemarkSrcSel_set;
    pMapper->qos_1pDfltPriSrcSel_get = dal_mango_qos_1pDfltPriSrcSel_get;
    pMapper->qos_1pDfltPriSrcSel_set = dal_mango_qos_1pDfltPriSrcSel_set;
    pMapper->qos_schedulingAlgorithm_get = dal_mango_qos_schedulingAlgorithm_get;
    pMapper->qos_schedulingAlgorithm_set = dal_mango_qos_schedulingAlgorithm_set;
    pMapper->qos_schedulingQueue_get = dal_mango_qos_schedulingQueue_get;
    pMapper->qos_schedulingQueue_set = dal_mango_qos_schedulingQueue_set;
    pMapper->qos_congAvoidAlgo_get = dal_mango_qos_congAvoidAlgo_get;
    pMapper->qos_congAvoidAlgo_set = dal_mango_qos_congAvoidAlgo_set;
    pMapper->qos_portCongAvoidAlgo_get = dal_mango_qos_portCongAvoidAlgo_get;
    pMapper->qos_portCongAvoidAlgo_set = dal_mango_qos_portCongAvoidAlgo_set;
    pMapper->qos_congAvoidGlobalQueueConfig_get = dal_mango_qos_congAvoidGlobalQueueConfig_get;
    pMapper->qos_congAvoidGlobalQueueConfig_set = dal_mango_qos_congAvoidGlobalQueueConfig_set;
    pMapper->qos_portAvbStreamReservationClassEnable_get = dal_mango_qos_portAvbStreamReservationClassEnable_get;
    pMapper->qos_portAvbStreamReservationClassEnable_set = dal_mango_qos_portAvbStreamReservationClassEnable_set;
    pMapper->qos_avbStreamReservationConfig_get = dal_mango_qos_avbStreamReservationConfig_get;
    pMapper->qos_avbStreamReservationConfig_set = dal_mango_qos_avbStreamReservationConfig_set;
    pMapper->qos_invldDscpVal_get = dal_mango_qos_invldDscpVal_get;
    pMapper->qos_invldDscpVal_set = dal_mango_qos_invldDscpVal_set;
    pMapper->qos_invldDscpEnable_get = dal_mango_qos_invldDscpEnable_get;
    pMapper->qos_invldDscpEnable_set = dal_mango_qos_invldDscpEnable_set;
    pMapper->qos_sysPortPriRemapSel_get = dal_mango_qos_sysPortPriRemapSel_get;
    pMapper->qos_sysPortPriRemapSel_set = dal_mango_qos_sysPortPriRemapSel_set;
    pMapper->qos_portQueueStrictEnable_get = dal_mango_qos_portQueueStrictEnable_get;
    pMapper->qos_portQueueStrictEnable_set = dal_mango_qos_portQueueStrictEnable_set;
    pMapper->qos_priRemapEnable_get = dal_mango_qos_priRemapEnable_get;
    pMapper->qos_priRemapEnable_set = dal_mango_qos_priRemapEnable_set;
    pMapper->qos_outer1pDfltPri_get = dal_mango_qos_outer1pDfltPri_get;
    pMapper->qos_outer1pDfltPri_set = dal_mango_qos_outer1pDfltPri_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_init
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
 *      1. set input bandwidth control parameters to default values
 *      2. set priority decision parameters
 *      3. set scheduling parameters
 *      4. disable port remark ability
 *      5. set flow control thresholds
 */
int32
dal_mango_qos_init(uint32 unit, uint32 queueNum)
{
    RT_INIT_REENTRY_CHK(qos_init[unit]);
    qos_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    qos_sem[unit] = osal_sem_mutex_create();
    if (0 == qos_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_QOS), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    qos_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_mango_qos_init */

/* Function Name:
 *      dal_mango_qos_priSelGroup_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_priSelGroup_get(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, grp_idx=%u", unit, grp_idx);

    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((grp_idx > HAL_PRI_SEL_GROUP_INDEX_MAX(unit)), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_PORTf, &(pWeightOfPriSel->weight_of_portBased))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_DSCPf, &(pWeightOfPriSel->weight_of_dscp))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_VACLf, &(pWeightOfPriSel->weight_of_vlanAcl))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
               MANGO_WT_ITAGf, &(pWeightOfPriSel->weight_of_innerTag))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_OTAGf, &(pWeightOfPriSel->weight_of_outerTag))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_1BRf, &(pWeightOfPriSel->weight_of_peTag))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_MAC_VLANf, &(pWeightOfPriSel->weight_of_macVlan))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_PROTO_VLANf, &(pWeightOfPriSel->weight_of_protoVlan))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_ROUTf, &(pWeightOfPriSel->weight_of_routing))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_TUNNELf, &(pWeightOfPriSel->weight_of_tunnel))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_11Ef, &(pWeightOfPriSel->weight_of_80211e))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_MPLSf, &(pWeightOfPriSel->weight_of_mpls))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_priSelGroup_get */

/* Function Name:
 *      dal_mango_qos_priSelGroup_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_priSelGroup_set(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, grp_idx=%u, \
           weight_of_portBased=%d, weight_of_dscp=%d,     weight_of_inAcl=%d,   \
           weight_of_innerTag=%d,  weight_of_outerTag=%d, weight_of_macVlan=%d, \
           weight_of_protoVlan=%d, weight_of_routing=%d,  weight_of_tunnel=%d,  \
           weight_of_80211e=%d,    weight_of_mpls=%d, weight_of_peTag=%d "
           , unit, grp_idx, pWeightOfPriSel->weight_of_portBased
           , pWeightOfPriSel->weight_of_dscp, pWeightOfPriSel->weight_of_vlanAcl
           , pWeightOfPriSel->weight_of_innerTag , pWeightOfPriSel->weight_of_outerTag
           , pWeightOfPriSel->weight_of_macVlan , pWeightOfPriSel->weight_of_protoVlan
           , pWeightOfPriSel->weight_of_routing , pWeightOfPriSel->weight_of_tunnel
           , pWeightOfPriSel->weight_of_80211e , pWeightOfPriSel->weight_of_mpls
           , pWeightOfPriSel->weight_of_peTag);

    RT_PARAM_CHK((grp_idx > HAL_PRI_SEL_GROUP_INDEX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_portBased < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_portBased > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_dscp < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_dscp > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_vlanAcl < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_vlanAcl > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_innerTag < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_innerTag > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_outerTag < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_outerTag > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_peTag < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_peTag > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_macVlan < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_macVlan > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_protoVlan < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_protoVlan > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_routing < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_routing > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_tunnel < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_tunnel > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_80211e < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_80211e > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_mpls < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_mpls > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    val = pWeightOfPriSel->weight_of_portBased;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_PORTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_dscp;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_DSCPf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_vlanAcl;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_VACLf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_innerTag;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_ITAGf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_outerTag;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_OTAGf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_peTag;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_1BRf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_macVlan;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_MAC_VLANf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_protoVlan;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_PROTO_VLANf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_routing;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_ROUTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_tunnel;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_TUNNELf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_80211e;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_11Ef, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_mpls;
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                MANGO_WT_MPLSf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_priSelGroup_set */

/* Function Name:
 *      dal_mango_qos_portPriSelGroup_get
 * Description:
 *      Get priority selection group for specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 * Output:
 *      pPriSelGrp_idx  - pointer to index of priority selection group
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
dal_mango_qos_portPriSelGroup_get(uint32 unit, rtk_port_t port, uint32 *pPriSelGrp_idx)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPriSelGrp_idx), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_PORT_TBL_IDX_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MANGO_IDXf, pPriSelGrp_idx)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_portPriSelGroup_get */

/* Function Name:
 *      dal_mango_qos_portPriSelGroup_set
 * Description:
 *      Set priority selection group for specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      priSelGrp_idx   - index of priority selection group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_qos_portPriSelGroup_set(uint32 unit, rtk_port_t port, uint32 priSelGrp_idx)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, priSelGrp_idx=%d"
            , unit, port, priSelGrp_idx);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((priSelGrp_idx > HAL_PRI_SEL_GROUP_INDEX_MAX(unit)), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_PORT_TBL_IDX_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MANGO_IDXf, &priSelGrp_idx)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_portPriSelGroup_set */

/* Function Name:
 *      dal_mango_qos_invldDscpVal_get
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
dal_mango_qos_invldDscpVal_get(uint32 unit, uint32 *pDscp)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MANGO_PRI_SEL_CTRLr, MANGO_DSCP_INVLD_VALf, pDscp))!=RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDscp = %d", *pDscp);

    return RT_ERR_OK;
}/* end of dal_mango_qos_invldDscpVal_get */

/* Function Name:
 *      dal_mango_qos_invldDscpVal_set
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
dal_mango_qos_invldDscpVal_set(uint32 unit, uint32 dscp)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MANGO_PRI_SEL_CTRLr, MANGO_DSCP_INVLD_VALf, &dscp))!=RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_mango_qos_invldDscpVal_set */

/* Function Name:
 *      dal_mango_qos_invldDscpEnable_get
 * Description:
 *      Get invalid DSCP status
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - status of invalid DSCP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of invalid DSCP:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_mango_qos_invldDscpEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 value;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_PRI_SEL_CTRLr, MANGO_DSCP_INVLD_ENf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if(value==0)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable = %d", *pEnable);

    return RT_ERR_OK;
}/* end of dal_mango_qos_invldDscpEnable_get */

/* Function Name:
 *      dal_mango_qos_invldDscpEnable_set
 * Description:
 *      Set invalid DSCP status
 * Input:
 *      unit   - unit id
 *      enable - status of invalid DSCP
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The status of invalid DSCP:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_mango_qos_invldDscpEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32 value;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if(enable==ENABLED)
        value = 1;
    else
        value = 0;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, MANGO_PRI_SEL_CTRLr, MANGO_DSCP_INVLD_ENf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_mango_qos_invldDscpEnable_set */

/* Function Name:
 *      dal_mango_qos_priRemapEnable_get
 * Description:
 *      Get priority remapping status
 * Input:
 *      unit    - unit id
 *      src     - priority remap type
 * Output:
 *      pEnable - port-base outer priority remapping status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Supported priority remap type rtk_qos_priSrc_t is as following:
 *      - PRI_SRC_PB_PRI
 *      - PRI_SRC_PROTO_VLAN
 *      - PRI_SRC_MAC_VLAN
 */
int32
dal_mango_qos_priRemapEnable_get(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (src)
    {
        case PRI_SRC_PB_PRI:
            field = MANGO_PORT_PRI_REMAP_ENf;
            break;
        case PRI_SRC_PROTO_VLAN:
            field = MANGO_PROTO_VLAN_PRI_REMAP_ENf;
            break;
        case PRI_SRC_MAC_VLAN:
            field = MANGO_MAC_IP_VLAN_PRI_REMAP_ENf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_PRI_SEL_CTRLr, field, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_priRemapEnable_set
 * Description:
 *      Set priority remapping status
 * Input:
 *      unit   - unit id
 *      src    - priority remap type
 *      enable - port-base inner priority remapping status
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Supported priority remap type rtk_qos_priSrc_t is as following:
 *      - PRI_SRC_PB_PRI
 *      - PRI_SRC_PROTO_VLAN
 *      - PRI_SRC_MAC_VLAN
 */
int32
dal_mango_qos_priRemapEnable_set(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t enable)
{
    int32   ret;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    switch (src)
    {
        case PRI_SRC_PB_PRI:
            field = MANGO_PORT_PRI_REMAP_ENf;
            break;
        case PRI_SRC_PROTO_VLAN:
            field = MANGO_PROTO_VLAN_PRI_REMAP_ENf;
            break;
        case PRI_SRC_MAC_VLAN:
            field = MANGO_MAC_IP_VLAN_PRI_REMAP_ENf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_field_write(unit, MANGO_PRI_SEL_CTRLr, field, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_sysPortPriRemapSel_get
 * Description:
 *      Get port-based priority remapping table.
 * Input:
 *      unit    - unit id
 *      pType   - remapping table selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_sysPortPriRemapSel_get(uint32 unit, rtk_qos_portPriRemapSel_t *pType)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* get value from CHIP*/
    if ((ret = dal_mango_qos_portPortPriRemapSel_get(unit, 0, pType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_mango_qos_sysPortPriRemapSel_get */

/* Function Name:
 *      dal_mango_qos_sysPortPriRemapSel_set
 * Description:
 *      Set port-based priority remapping table.
 * Input:
 *      unit    - unit id
 *      type    - remapping table selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_qos_sysPortPriRemapSel_set(uint32 unit, rtk_qos_portPriRemapSel_t type)
{
    int32   ret;
    rtk_port_t  port;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* set value to chip */
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = dal_mango_qos_portPortPriRemapSel_set(unit, port, type)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_mango_qos_sysPortPriRemapSel_set */

/* Function Name:
 *      dal_mango_qos_portPortPriRemapSel_get
 * Description:
 *      Get port-based priority remapping table on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pType   - remapping table selection
 * Output:
 *      None
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
dal_mango_qos_portPortPriRemapSel_get(uint32 unit, rtk_port_t port, rtk_qos_portPriRemapSel_t *pType)
{
    int32   ret;
    uint32  value = 0;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_PRI_SEL_PORT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MANGO_PORT_PRI_REMAP_TBL_SELf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = P_PRI_REMAP_INNER_PRI_CFI0_TBL;
    else
        *pType = P_PRI_REMAP_OUTER_PRI_DEI0_TBL;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portPortPriRemapSel_set
 * Description:
 *      Set port-based priority remapping table on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - remapping table selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_qos_portPortPriRemapSel_set(uint32 unit, rtk_port_t port, rtk_qos_portPriRemapSel_t type)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%u", unit, type);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    if (P_PRI_REMAP_INNER_PRI_CFI0_TBL == type)
        value = 0;
    else if (P_PRI_REMAP_OUTER_PRI_DEI0_TBL == type)
        value = 1;
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_PRI_SEL_PORT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MANGO_PORT_PRI_REMAP_TBL_SELf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portDpSel_get
 * Description:
 *      Get weight of each priority assignment on specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 * Output:
 *      pWeightOfDpSel  - pointer to weight of each priority assignment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_portDpSel_get(uint32 unit, rtk_port_t port, rtk_qos_dpSelWeight_t *pWeightOfDpSel)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pWeightOfDpSel), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_DP_SEL_PORT_TBL_CTRLr, port, REG_ARRAY_INDEX_NONE,
                MANGO_WT_DSCPf, &(pWeightOfDpSel->weight_of_dscp))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, MANGO_DP_SEL_PORT_TBL_CTRLr, port, REG_ARRAY_INDEX_NONE,
                MANGO_WT_ITAGf, &(pWeightOfDpSel->weight_of_innerTag))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, MANGO_DP_SEL_PORT_TBL_CTRLr, port, REG_ARRAY_INDEX_NONE,
                MANGO_WT_OTAGf, &(pWeightOfDpSel->weight_of_outerTag))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_DP_SEL_PORT_TBL_CTRLr, port, REG_ARRAY_INDEX_NONE,
                MANGO_WT_MPLSf, &(pWeightOfDpSel->weight_of_mpls))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portDpSel_set
 * Description:
 *      Set weight of each priority assignment on specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      pWeightOfDpSel  - weight of each priority assignment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_portDpSel_set(uint32 unit, rtk_port_t port, rtk_qos_dpSelWeight_t *pWeightOfDpSel)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, weight_of_dscp=%d, weight_of_innerTag=%d, \
           weight_of_outerTag=%d,  weight_of_mpls=%d ", unit, port
           , pWeightOfDpSel->weight_of_dscp, pWeightOfDpSel->weight_of_innerTag
           , pWeightOfDpSel->weight_of_outerTag , pWeightOfDpSel->weight_of_mpls);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pWeightOfDpSel->weight_of_dscp < HAL_DP_OF_SELECTION_MIN(unit))
               ||(pWeightOfDpSel->weight_of_dscp > HAL_DP_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfDpSel->weight_of_innerTag < HAL_DP_OF_SELECTION_MIN(unit))
               ||(pWeightOfDpSel->weight_of_innerTag > HAL_DP_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfDpSel->weight_of_outerTag < HAL_DP_OF_SELECTION_MIN(unit))
               ||(pWeightOfDpSel->weight_of_outerTag > HAL_DP_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfDpSel->weight_of_mpls < HAL_DP_OF_SELECTION_MIN(unit))
               ||(pWeightOfDpSel->weight_of_mpls > HAL_DP_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    val = pWeightOfDpSel->weight_of_dscp;
    if ((ret = reg_array_field_write(unit, MANGO_DP_SEL_PORT_TBL_CTRLr, port, REG_ARRAY_INDEX_NONE,
                MANGO_WT_DSCPf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfDpSel->weight_of_innerTag;
    if ((ret = reg_array_field_write(unit, MANGO_DP_SEL_PORT_TBL_CTRLr, port, REG_ARRAY_INDEX_NONE,
                MANGO_WT_ITAGf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfDpSel->weight_of_outerTag;
    if ((ret = reg_array_field_write(unit, MANGO_DP_SEL_PORT_TBL_CTRLr, port, REG_ARRAY_INDEX_NONE,
                MANGO_WT_OTAGf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfDpSel->weight_of_mpls;
    if ((ret = reg_array_field_write(unit, MANGO_DP_SEL_PORT_TBL_CTRLr, port, REG_ARRAY_INDEX_NONE,
                MANGO_WT_MPLSf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_dpRemap_get
 * Description:
 *      Get DSCP/Inner-tag/Outer-tag/MPLS mapping to drop precedence.
 * Input:
 *      unit   - unit id
 *      src    - drop precedence remap source type
 *      srcVal - remap source value
 * Output:
 *      pDp    - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_QOS_DEI_VALUE   - Invalid DEI/CFI value
 *      RT_ERR_QOS_1P_PRIORITY - Invalid priority value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      The API can get configuration of DSCP/Inner-tag/Outer-tag/MPLS to DP remapping table.
 */
int32
dal_mango_qos_dpRemap_get(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 *pDp)
{
    int32   ret, index;
    uint32  reg, field;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d, srcVal=%d, pDp=%x"
            , unit, src, srcVal, pDp);

    /* parameter check */
    RT_PARAM_CHK(((DP_SRC_DSCP_BASED == src) && (srcVal.src.dscp > RTK_VALUE_OF_DSCP_MAX)), RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK(((DP_SRC_INNER_PRI_BASED == src) && (srcVal.src.tag.dei > RTK_DOT1P_DEI_MAX)), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(((DP_SRC_INNER_PRI_BASED == src) && (srcVal.src.tag.pri > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(((DP_SRC_OUTER_PRI_BASED == src) && (srcVal.src.tag.dei > RTK_DOT1P_DEI_MAX)), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(((DP_SRC_OUTER_PRI_BASED == src) && (srcVal.src.tag.pri > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_QOS_1P_PRIORITY);
/* [FIX ME] mpls.tc should use another checking boundary value */
    RT_PARAM_CHK(((DP_SRC_MPLS_BASED == src) && (srcVal.src.tc > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pDp), RT_ERR_NULL_POINTER);

    switch (src)
    {
        case DP_SRC_DSCP_BASED:
            reg = MANGO_DP_SEL_REMAP_DSCPr;
            field = MANGO_DP_DSCPf;
            index = srcVal.src.dscp;
            break;
        case DP_SRC_INNER_PRI_BASED:
            if (0 == srcVal.src.tag.dei)
            {
                reg = MANGO_DP_SEL_REMAP_ITAG_CFI0r;
                field = MANGO_DP_CFI0_IPRIf;
            }
            else
            {
                reg = MANGO_DP_SEL_REMAP_ITAG_CFI1r;
                field = MANGO_DP_CFI1_IPRIf;
            }
            index = srcVal.src.tag.pri;
            break;
        case DP_SRC_OUTER_PRI_BASED:
            if (0 == srcVal.src.tag.dei)
            {
                reg = MANGO_DP_SEL_REMAP_OTAG_DEI0r;
                field = MANGO_DP_DEI0_OPRIf;
            }
            else
            {
                reg = MANGO_DP_SEL_REMAP_OTAG_DEI1r;
                field = MANGO_DP_DEI1_OPRIf;
            }
            index = srcVal.src.tag.pri;
            break;
        case DP_SRC_MPLS_BASED:
            reg = MANGO_DP_SEL_REMAP_MPLSr;
            field = MANGO_DP_TCf;
            index = srcVal.src.tc;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg, REG_ARRAY_INDEX_NONE, index, field, pDp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_dpRemap_set
 * Description:
 *      Set DSCP/Inner-tag/Outer-tag/MPLS mapping to drop precedence.
 * Input:
 *      unit   - unit id
 *      src    - drop precedence remap source type
 *      srcVal - remap source value
 *      dp     - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE  - Invalid DSCP value
 *      RT_ERR_QOS_DEI_VALUE   - Invalid DEI/CFI value
 *      RT_ERR_QOS_1P_PRIORITY - Invalid priority value
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 * Note:
 *      The API can configure DSCP/Inner-tag/Outer-tag/MPLS to DP remapping table.
 */
int32
dal_mango_qos_dpRemap_set(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 dp)
{
    int32   ret, index;
    uint32  reg, field;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d, srcVal=%d, dp=%u"
            , unit, src, srcVal, dp);

    /* parameter check */
    RT_PARAM_CHK(((DP_SRC_DSCP_BASED == src) && (srcVal.src.dscp > RTK_VALUE_OF_DSCP_MAX)), RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK(((DP_SRC_INNER_PRI_BASED == src) && (srcVal.src.tag.dei > RTK_DOT1P_DEI_MAX)), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(((DP_SRC_INNER_PRI_BASED == src) && (srcVal.src.tag.pri > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(((DP_SRC_OUTER_PRI_BASED == src) && (srcVal.src.tag.dei > RTK_DOT1P_DEI_MAX)), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(((DP_SRC_OUTER_PRI_BASED == src) && (srcVal.src.tag.pri > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_QOS_1P_PRIORITY);
/* [FIX ME] mpls.tc should use another checking boundary value */
    RT_PARAM_CHK(((DP_SRC_MPLS_BASED == src) && (srcVal.src.tc > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_INPUT);
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);

    switch (src)
    {
        case DP_SRC_DSCP_BASED:
            reg = MANGO_DP_SEL_REMAP_DSCPr;
            field = MANGO_DP_DSCPf;
            index = srcVal.src.dscp;
            break;
        case DP_SRC_INNER_PRI_BASED:
            if (0 == srcVal.src.tag.dei)
            {
                reg = MANGO_DP_SEL_REMAP_ITAG_CFI0r;
                field = MANGO_DP_CFI0_IPRIf;
            }
            else
            {
                reg = MANGO_DP_SEL_REMAP_ITAG_CFI1r;
                field = MANGO_DP_CFI1_IPRIf;
            }
            index = srcVal.src.tag.pri;
            break;
        case DP_SRC_OUTER_PRI_BASED:
            if (0 == srcVal.src.tag.dei)
            {
                reg = MANGO_DP_SEL_REMAP_OTAG_DEI0r;
                field = MANGO_DP_DEI0_OPRIf;
            }
            else
            {
                reg = MANGO_DP_SEL_REMAP_OTAG_DEI1r;
                field = MANGO_DP_DEI1_OPRIf;
            }
            index = srcVal.src.tag.pri;
            break;
        case DP_SRC_MPLS_BASED:
            reg = MANGO_DP_SEL_REMAP_MPLSr;
            field = MANGO_DP_TCf;
            index = srcVal.src.tc;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg, REG_ARRAY_INDEX_NONE, index, field, &dp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_priRemap_get
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
 *      - PRI_SRC_INNER_USER_PRI
 *      - PRI_SRC_OUTER_USER_PRI
 *      - PRI_SRC_DSCP
 *      - PRI_SRC_PB_PRI
 *      - PRI_SRC_80211E
 */
int32
dal_mango_qos_priRemap_get(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 *pPri)
{
    int32   ret, index1, index2;
    uint32  reg, field;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d, srcVal=%d, pPri=%x", unit, src, srcVal, pPri);

    /* parameter check */
    RT_PARAM_CHK(((PRI_SRC_PB_PRI == src) && (!HWP_PORT_EXIST(unit, srcVal.src.port))), RT_ERR_PORT_ID);
    RT_PARAM_CHK(((PRI_SRC_INNER_USER_PRI == src) && (srcVal.src.tag.dei > RTK_DOT1P_DEI_MAX)), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(((PRI_SRC_INNER_USER_PRI == src) && (srcVal.src.tag.pri > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(((PRI_SRC_OUTER_USER_PRI == src) && (srcVal.src.tag.dei > RTK_DOT1P_DEI_MAX)), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(((PRI_SRC_OUTER_USER_PRI == src) && (srcVal.src.tag.pri > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(((PRI_SRC_PE_PRI == src) && (srcVal.src.tag.dei > RTK_DOT1P_DEI_MAX)), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(((PRI_SRC_PE_PRI == src) && (srcVal.src.tag.pri > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(((PRI_SRC_DSCP == src) && (srcVal.src.dscp > RTK_VALUE_OF_DSCP_MAX)), RT_ERR_QOS_DSCP_VALUE);
    /* [FIX ME] ieee_80211e.tid and MPLS.tc should use another checking boundary value */
    RT_PARAM_CHK(((PRI_SRC_80211E == src) && (srcVal.src.tid > 15)), RT_ERR_INPUT);
    RT_PARAM_CHK(((PRI_SRC_MPLS == src) && (srcVal.src.tc > 7)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);

    index1 = REG_ARRAY_INDEX_NONE;
    index2 = REG_ARRAY_INDEX_NONE;
    switch (src)
    {
        case PRI_SRC_PB_PRI:
            reg = MANGO_PRI_SEL_REMAP_PORTr;
            field = MANGO_INTPRI_PORTf;
            index1 = srcVal.src.port;
            break;
        case PRI_SRC_INNER_USER_PRI:
            if (0 == srcVal.src.tag.dei)
            {
                reg = MANGO_PRI_SEL_REMAP_IPRI_CFI0r;
                field = MANGO_INTPRI_CFI0_IPRIf;
            }
            else
            {
                reg = MANGO_PRI_SEL_REMAP_IPRI_CFI1r;
                field = MANGO_INTPRI_CFI1_IPRIf;
            }
            index2 = srcVal.src.tag.pri;
            break;
        case PRI_SRC_OUTER_USER_PRI:
            if (0 == srcVal.src.tag.dei)
            {
                reg = MANGO_PRI_SEL_REMAP_OPRI_DEI0r;
                field = MANGO_INTPRI_DEI0_OPRIf;
            }
            else
            {
                reg = MANGO_PRI_SEL_REMAP_OPRI_DEI1r;
                field = MANGO_INTPRI_DEI1_OPRIf;
            }
            index2 = srcVal.src.tag.pri;
            break;
        case PRI_SRC_PE_PRI:
            reg = MANGO_PRI_SEL_REMAP_1BRr;
            field = MANGO_INTPRI_1BRf;
            index1 = srcVal.src.tag.dei;
            index2 = srcVal.src.tag.pri;
            break;
        case PRI_SRC_DSCP:
            reg = MANGO_PRI_SEL_REMAP_DSCPr;
            field = MANGO_INTPRI_DSCPf;
            index2 = srcVal.src.dscp;
            break;
        case PRI_SRC_80211E:
            reg = MANGO_PRI_SEL_REMAP_11Er;
            field = MANGO_INTPRI_11Ef;
            index2 = srcVal.src.tid;
            break;
        case PRI_SRC_MPLS:
            reg = MANGO_PRI_SEL_REMAP_MPLSr;
            field = MANGO_INTPRI_MPLS_TCf;
            index2 = srcVal.src.tid;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg, index1, index2, field, pPri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_priRemap_set
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
 *      - PRI_SRC_INNER_USER_PRI
 *      - PRI_SRC_OUTER_USER_PRI
 *      - PRI_SRC_DSCP
 *      - PRI_SRC_PB_PRI
 *      - PRI_SRC_80211E
 */
int32
dal_mango_qos_priRemap_set(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 pri)
{
    int32   ret, index1, index2;
    uint32  reg, field;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d, srcVal=%d, pri=%d", unit, src, srcVal, pri);

    /* parameter check */
    RT_PARAM_CHK(((PRI_SRC_PB_PRI == src) && (!HWP_PORT_EXIST(unit, srcVal.src.port))), RT_ERR_PORT_ID);
    RT_PARAM_CHK(((PRI_SRC_INNER_USER_PRI == src) && (srcVal.src.tag.dei > RTK_DOT1P_DEI_MAX)), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(((PRI_SRC_INNER_USER_PRI == src) && (srcVal.src.tag.pri > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(((PRI_SRC_OUTER_USER_PRI == src) && (srcVal.src.tag.dei > RTK_DOT1P_DEI_MAX)), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(((PRI_SRC_OUTER_USER_PRI == src) && (srcVal.src.tag.pri > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(((PRI_SRC_PE_PRI == src) && (srcVal.src.tag.dei > RTK_DOT1P_DEI_MAX)), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(((PRI_SRC_PE_PRI == src) && (srcVal.src.tag.pri > RTK_DOT1P_PRIORITY_MAX)), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(((PRI_SRC_DSCP == src) && (srcVal.src.dscp > RTK_VALUE_OF_DSCP_MAX)), RT_ERR_QOS_DSCP_VALUE);
    /* [FIX ME] ieee_80211e.tid should use another checking boundary value */
    RT_PARAM_CHK(((PRI_SRC_80211E == src) && (srcVal.src.tid > 15)), RT_ERR_INPUT);
    RT_PARAM_CHK(((PRI_SRC_MPLS == src) && (srcVal.src.tc > 7)), RT_ERR_INPUT);
    RT_PARAM_CHK((pri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_QOS_INT_PRIORITY);

    index1 = REG_ARRAY_INDEX_NONE;
    index2 = REG_ARRAY_INDEX_NONE;
    switch (src)
    {
        case PRI_SRC_PB_PRI:
            reg = MANGO_PRI_SEL_REMAP_PORTr;
            field = MANGO_INTPRI_PORTf;
            index1 = srcVal.src.port;
            break;
        case PRI_SRC_INNER_USER_PRI:
            if (0 == srcVal.src.tag.dei)
            {
                reg = MANGO_PRI_SEL_REMAP_IPRI_CFI0r;
                field = MANGO_INTPRI_CFI0_IPRIf;
            }
            else
            {
                reg = MANGO_PRI_SEL_REMAP_IPRI_CFI1r;
                field = MANGO_INTPRI_CFI1_IPRIf;
            }
            index2 = srcVal.src.tag.pri;
            break;
        case PRI_SRC_OUTER_USER_PRI:
            if (0 == srcVal.src.tag.dei)
            {
                reg = MANGO_PRI_SEL_REMAP_OPRI_DEI0r;
                field = MANGO_INTPRI_DEI0_OPRIf;
            }
            else
            {
                reg = MANGO_PRI_SEL_REMAP_OPRI_DEI1r;
                field = MANGO_INTPRI_DEI1_OPRIf;
            }
            index2 = srcVal.src.tag.pri;
            break;
        case PRI_SRC_PE_PRI:
            reg = MANGO_PRI_SEL_REMAP_1BRr;
            field = MANGO_INTPRI_1BRf;
            index1 = srcVal.src.tag.dei;
            index2 = srcVal.src.tag.pri;
            break;
        case PRI_SRC_DSCP:
            reg = MANGO_PRI_SEL_REMAP_DSCPr;
            field = MANGO_INTPRI_DSCPf;
            index2 = srcVal.src.dscp;
            break;
        case PRI_SRC_80211E:
            reg = MANGO_PRI_SEL_REMAP_11Er;
            field = MANGO_INTPRI_11Ef;
            index2 = srcVal.src.tid;
            break;
        case PRI_SRC_MPLS:
            reg = MANGO_PRI_SEL_REMAP_MPLSr;
            field = MANGO_INTPRI_MPLS_TCf;
            index2 = srcVal.src.tid;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg, index1, index2, field, &pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_pri2QidMap_get
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
dal_mango_qos_pri2QidMap_get(uint32 unit, rtk_pri_t pri, rtk_qid_t *pQid)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((pri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_PRIORITY);
    RT_PARAM_CHK((NULL == pQid), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_QM_INTPRI2QID_CTRLr
                        , REG_ARRAY_INDEX_NONE, pri, MANGO_QIDf, pQid)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pQid=%d", pQid);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_pri2QidMap_set
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
dal_mango_qos_pri2QidMap_set(uint32 unit, rtk_pri_t pri, rtk_qid_t qid)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((pri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_PRIORITY);
    RT_PARAM_CHK((qid >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_QM_INTPRI2QID_CTRLr
                        , REG_ARRAY_INDEX_NONE, pri, MANGO_QIDf, &qid)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_cpuQid2QidMap_get
 * Description:
 *      Get CPU QID to normal port QID mapping value.
 * Input:
 *      unit      - unit id
 *      cpuQid    - queue id of CPU port
 * Output:
 *      pQid      - queue id of normal port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_cpuQid2QidMap_get(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t *pQid)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, cpuQid=%d", unit, cpuQid);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((cpuQid >= HAL_MAX_NUM_OF_CPU_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pQid), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_QM_CPUQID2QID_CTRLr
                        , REG_ARRAY_INDEX_NONE, cpuQid, MANGO_QIDf, pQid)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pQid=%d", pQid);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_cpuQid2QidMap_set
 * Description:
 *      Set CPU QID to normal port QID mapping value.
 * Input:
 *      unit      - unit id
 *      cpuQid    - queue id of CPU port
 *      qid       - queue id of normal port
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - Invalid queue ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_cpuQid2QidMap_set(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t qid)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d cpuQid=%d qid=%d", unit, cpuQid, qid);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((cpuQid >= HAL_MAX_NUM_OF_CPU_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((qid >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_QM_CPUQID2QID_CTRLr
                        , REG_ARRAY_INDEX_NONE, cpuQid, MANGO_QIDf, &qid)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_cpuQid2StackQidMap_get
 * Description:
 *      Get CPU QID to normal port QID mapping value.
 * Input:
 *      unit      - unit id
 *      cpuQid    - queue id of CPU port
 * Output:
 *      pQid      - queue id of stacking port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_cpuQid2StackQidMap_get(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t *pQid)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, cpuQid=%d", unit, cpuQid);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((cpuQid >= HAL_MAX_NUM_OF_CPU_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pQid), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_QM_CPUQID2XGSQID_CTRLr
                        , REG_ARRAY_INDEX_NONE, cpuQid, MANGO_QIDf, pQid)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pQid=%d", pQid);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_cpuQid2StackQidMap_set
 * Description:
 *      Set CPU QID to normal port QID mapping value.
 * Input:
 *      unit      - unit id
 *      cpuQid    - queue id of CPU port
 *      qid       - queue id of stacking port
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - Invalid queue ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_cpuQid2StackQidMap_set(uint32 unit, rtk_qid_t cpuQid, rtk_qid_t qid)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d cpuQid=%d qid=%d", unit, cpuQid, qid);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((cpuQid >= HAL_MAX_NUM_OF_CPU_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((qid >= HAL_MAX_NUM_OF_STACK_QUEUE(unit)), RT_ERR_QUEUE_ID);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_QM_CPUQID2XGSQID_CTRLr
                        , REG_ARRAY_INDEX_NONE, cpuQid, MANGO_QIDf, &qid)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_schedulingAlgorithm_get
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
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The types of scheduling algorithm:
 *    - WFQ
 *    - WRR
 */
int32
dal_mango_qos_schedulingAlgorithm_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   *pScheduling_type)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScheduling_type), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_SCHED_PORT_ALGO_CTRLr,
                        port, REG_ARRAY_INDEX_NONE, MANGO_SCHED_TYPEf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pScheduling_type = WFQ;
            break;
        case 1:
            *pScheduling_type = WRR;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pScheduling_type=%d", *pScheduling_type);

    return RT_ERR_OK;
} /* end of dal_mango_qos_schedulingAlgorithm_get */

/* Function Name:
 *      dal_mango_qos_schedulingAlgorithm_set
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
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_QOS_SCHE_TYPE - Error scheduling algorithm type
 * Note:
 *    The types of scheduling algorithm:
 *    - WFQ
 *    - WRR
 */
int32
dal_mango_qos_schedulingAlgorithm_set(
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
        default:
            value = 1;
    }

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_SCHED_PORT_ALGO_CTRLr,
                        port, REG_ARRAY_INDEX_NONE, MANGO_SCHED_TYPEf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return ret;
} /* end of dal_mango_qos_schedulingAlgorithm_set */

/* Function Name:
 *      dal_mango_qos_schedulingQueue_get
 * Description:
 *      Get the scheduling weights of queues on specific port in egress scheduling.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pQweights - the array of weights for WRR/WFQ queue (valid:1~127)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_schedulingQueue_get(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32       ret;
    rtk_qid_t   qid, maxQid;
    uint32      reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    if (HWP_IS_CPU_PORT(unit, port))
    {
        port = REG_ARRAY_INDEX_NONE;
        reg = MANGO_SCHED_CPU_Q_CTRLr;
        maxQid = HAL_MAX_NUM_OF_CPU_QUEUE(unit);
    }
    else if (HWP_UPLINK_PORT(unit, port))
    {
        reg = MANGO_SCHED_PORT_Q_CTRL_SET1r;
        maxQid = HAL_MAX_NUM_OF_STACK_QUEUE(unit);
    }
    else
    {
        reg = MANGO_SCHED_PORT_Q_CTRL_SET0r;
        maxQid = HAL_MAX_NUM_OF_QUEUE(unit);
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP */
    for (qid = 0; qid < maxQid; qid++)
    {
        if ((ret = reg_array_field_read(unit, reg,
                            port, qid, MANGO_WEIGHTf, &pQweights->weights[qid])) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    for (qid = 0; qid < maxQid; qid++)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pQweights[%d]=%d, ", qid, pQweights->weights[qid]);
    }

    return RT_ERR_OK;
} /* end of dal_mango_qos_schedulingQueue_get */

/* Function Name:
 *      dal_mango_qos_schedulingQueue_set
 * Description:
 *      Set the scheduling weights of queues on specific port in egress scheduling.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      pQweights - the array of weights for WRR/WFQ queue (valid:1~127)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - Invalid port id
 *      RT_ERR_NULL_POINTER     - Null pointer
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight
 * Note:
 *      None
 */
int32
dal_mango_qos_schedulingQueue_set(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32       ret;
    rtk_qid_t   qid, maxQid;
    uint32      reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    if (HWP_IS_CPU_PORT(unit, port))
    {
        port = REG_ARRAY_INDEX_NONE;
        reg = MANGO_SCHED_CPU_Q_CTRLr;
        maxQid = HAL_MAX_NUM_OF_CPU_QUEUE(unit);
    }
    else if (HWP_UPLINK_PORT(unit, port))
    {
        reg = MANGO_SCHED_PORT_Q_CTRL_SET1r;
        maxQid = HAL_MAX_NUM_OF_STACK_QUEUE(unit);
    }
    else
    {
        reg = MANGO_SCHED_PORT_Q_CTRL_SET0r;
        maxQid = HAL_MAX_NUM_OF_QUEUE(unit);
    }

    for (qid = 0; qid < maxQid; qid++)
    {
        RT_PARAM_CHK(pQweights->weights[qid] < HAL_QUEUE_WEIGHT_MIN(unit), RT_ERR_QOS_QUEUE_WEIGHT);
        RT_PARAM_CHK(pQweights->weights[qid] > HAL_QUEUE_WEIGHT_MAX(unit), RT_ERR_QOS_QUEUE_WEIGHT);
        /* Display debug message */
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pQweights[%d]=%d, ", qid, pQweights->weights[qid]);
    }

    QOS_SEM_LOCK(unit);

    for (qid = 0; qid < maxQid; qid++)
    {
        if ((ret = reg_array_field_write(unit, reg,
                            port, qid, MANGO_WEIGHTf, &pQweights->weights[qid])) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_schedulingQueue_set */

/* Module Name    : QoS              */
/* Sub-module Name: Congestion avoidance */

/* Function Name:
 *      dal_mango_qos_congAvoidAlgo_get
 * Description:
 *      Get algorithm of congestion avoidance.
 * Input:
 *      unit  - unit id
 * Output:
 *      pAlgo - pointer to algorithm of congestion avoidance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SWRED
 *      - CONG_AVOID_TD
 */
int32
dal_mango_qos_congAvoidAlgo_get(uint32 unit, rtk_qos_congAvoidAlgo_t *pAlgo)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* get value from CHIP*/
    if ((ret = dal_mango_qos_portCongAvoidAlgo_get(unit, 0, pAlgo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_congAvoidAlgo_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SWRED
 *      - CONG_AVOID_TD
 */
int32
dal_mango_qos_congAvoidAlgo_set(uint32 unit, rtk_qos_congAvoidAlgo_t algo)
{
    int32   ret;
    rtk_port_t  port;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* set value to chip */
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = dal_mango_qos_portCongAvoidAlgo_set(unit, port, algo)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portCongAvoidAlgo_get
 * Description:
 *      Get algorithm of congestion avoidance of specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pAlgo - pointer to algorithm of congestion avoidance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SWRED
 *      - CONG_AVOID_TD
 *      System only supports CONG_AVOID_TD in CPU port.
 */
int32
dal_mango_qos_portCongAvoidAlgo_get(uint32 unit, rtk_port_t port, rtk_qos_congAvoidAlgo_t *pAlgo)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(HWP_IS_CPU_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAlgo), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_EGR_DROP_CTRLr,
                            port, REG_ARRAY_INDEX_NONE, MANGO_EGR_DROP_ALGOf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pAlgo = CONG_AVOID_TD;
    else if (1 == value)
        *pAlgo = CONG_AVOID_SWRED;
    else
        return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "drop_type=0x%x", *pAlgo);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portCongAvoidAlgo_set
 * Description:
 *      Set algorithm of congestion avoidance of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      algo - algorithm of congestion avoidance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Algorithm is as following:
 *      - CONG_AVOID_SWRED
 *      - CONG_AVOID_TD
 *      System only supports CONG_AVOID_TD in CPU port.
 */
int32
dal_mango_qos_portCongAvoidAlgo_set(uint32 unit, rtk_port_t port, rtk_qos_congAvoidAlgo_t algo)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, algo=%d", unit, port, algo);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(HWP_IS_CPU_PORT(unit, port), RT_ERR_PORT_ID);

    if(CONG_AVOID_TD == algo)
        value = 0;
    else if(CONG_AVOID_SWRED == algo)
        value = 1;
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_EGR_DROP_CTRLr,
                            port, REG_ARRAY_INDEX_NONE, MANGO_EGR_DROP_ALGOf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_congAvoidGlobalQueueConfig_get
 * Description:
 *      Get queue drop probability and thresholds of congestion avoidance.
 * Input:
 *      unit                   - unit id
 *      queue                  - queue id
 *      dp                     - drop precedence
 * Output:
 *      pCongAvoidThresh       - pointer of drop probability and thresholds
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QUEUE_ID        - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_congAvoidGlobalQueueConfig_get(
    uint32    unit,
    rtk_qid_t queue,
    uint32    dp,
    rtk_qos_congAvoidThresh_t *pCongAvoidThresh)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue=%d, dp=%d", unit, queue, dp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK((NULL == pCongAvoidThresh), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_SWRED_Q_DROP_RATEr,
                        queue, dp, MANGO_RATEf, &(pCongAvoidThresh->probability))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, MANGO_SWRED_Q_THRr,
                        queue, dp, MANGO_MAXf, &(pCongAvoidThresh->maxThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, MANGO_SWRED_Q_THRr,
                        queue, dp, MANGO_MINf, &(pCongAvoidThresh->minThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pCongAvoidThresh->maxThresh=%d, pCongAvoidThresh->minThresh=%d, \
        pCongAvoidThresh->probability=%d", pCongAvoidThresh->maxThresh, pCongAvoidThresh->minThresh, pCongAvoidThresh->probability);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_congAvoidGlobalQueueConfig_set
 * Description:
 *      Set queue drop probability and thresholds of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      queue            - queue id
 *      dp               - drop precedence
 *      pCongAvoidThresh - pointer of drop probability and thresholds
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QUEUE_ID        - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_qos_congAvoidGlobalQueueConfig_set(
    uint32    unit,
    rtk_qid_t queue,
    uint32    dp,
    rtk_qos_congAvoidThresh_t *pCongAvoidThresh)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue=%d, dp=%d, \
        pCongAvoidThresh->maxThresh=%d, pCongAvoidThresh->minThresh=%d, pCongAvoidThresh->probability=%d", unit, queue, dp,
        pCongAvoidThresh->maxThresh, pCongAvoidThresh->minThresh, pCongAvoidThresh->probability);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK(pCongAvoidThresh->maxThresh > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_INPUT);
    RT_PARAM_CHK(pCongAvoidThresh->minThresh > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_INPUT);
    RT_PARAM_CHK(pCongAvoidThresh->probability > HAL_WRED_DROP_PROBABILITY_MAX(unit), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_DROP_RATEr,
                        queue, dp, MANGO_RATEf, &(pCongAvoidThresh->probability))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_THRr,
                        queue, dp, MANGO_MAXf, &(pCongAvoidThresh->maxThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_THRr,
                        queue, dp, MANGO_MINf, &(pCongAvoidThresh->minThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portAvbStreamReservationClassEnable_get
 * Description:
 *      Get status of the specified stream class of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      srClass - stream class
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT                 - invalid port id
 *      RT_ERR_AVB_INVALID_SR_CLASS - Invalid SR Class
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_portAvbStreamReservationClassEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            *pEnable)
{
    uint32  value = 0;
    int32   ret;
    uint32 reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, srClass=%d",
           unit, port, srClass);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((srClass >= AVB_SR_CLASS_END), RT_ERR_AVB_INVALID_SR_CLASS);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (srClass)
    {
        case AVB_SR_CLASS_A:
            reg = MANGO_AVB_PORT_CLASS_A_ENr;
            break;
        case AVB_SR_CLASS_B:
            reg = MANGO_AVB_PORT_CLASS_B_ENr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_read(unit, reg,
                            port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if(value==0)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
} /* end of dal_mango_qos_portAvbStreamReservationClassEnable_get */

/* Function Name:
 *      dal_mango_qos_portAvbStreamReservationClassEnable_set
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
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT                 - invalid port id
 *      RT_ERR_AVB_INVALID_SR_CLASS - Invalid SR Class
 *      RT_ERR_INPUT                - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_qos_portAvbStreamReservationClassEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            enable)
{
    uint32  value = 0;
    int32   ret;
    uint32 reg, regMac;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, srClass=%d",
           unit, port, srClass);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((srClass >= AVB_SR_CLASS_END), RT_ERR_AVB_INVALID_SR_CLASS);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    switch (srClass)
    {
        case AVB_SR_CLASS_A:
            reg = MANGO_AVB_PORT_CLASS_A_ENr;
            regMac = MANGO_AVB_PORT_CLASS_A_EN_MACr;
            break;
        case AVB_SR_CLASS_B:
            reg = MANGO_AVB_PORT_CLASS_B_ENr;
            regMac = MANGO_AVB_PORT_CLASS_B_EN_MACr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if(enable==ENABLED)
        value = 1;
    else
        value = 0;

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg,
                            port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, regMac,
                            port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_portAvbStreamReservationClassEnable_set */

/* Function Name:
 *      dal_mango_qos_avbStreamReservationConfig_get
 * Description:
 *      Get the configuration of Stream Reservation in the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pSrConf - pointer buffer of configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_avbStreamReservationConfig_get(uint32 unit, rtk_qos_avbSrConf_t *pSrConf)
{
    uint32  value = 0;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSrConf), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    if ((ret = reg_read(unit, MANGO_AVB_CTRLr, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_get(unit, MANGO_AVB_CTRLr, MANGO_CLASS_A_PRIf, &pSrConf->class_a_priority, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_field_get(unit, MANGO_AVB_CTRLr, MANGO_CLASS_B_PRIf, &pSrConf->class_b_priority, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_field_get(unit, MANGO_AVB_CTRLr, MANGO_CLASS_A_QIDf, &pSrConf->class_a_queue_id, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_field_get(unit, MANGO_AVB_CTRLr, MANGO_CLASS_B_QIDf, &pSrConf->class_b_queue_id, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_field_get(unit, MANGO_AVB_CTRLr, MANGO_CLASS_NON_A_REDIRECT_QIDf, &pSrConf->class_non_a_redirect_queue_id, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_field_get(unit, MANGO_AVB_CTRLr, MANGO_CLASS_NON_B_REDIRECT_QIDf, &pSrConf->class_non_b_redirect_queue_id, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_LOCK(unit);

    if ((ret = reg_read(unit, MANGO_AVB_CTRL_MACr, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    if ((ret = reg_field_get(unit, MANGO_AVB_CTRL_MACr, MANGO_CLASS_NON_A_RMK_PRIf, &pSrConf->class_non_a_remark_priority, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_field_get(unit, MANGO_AVB_CTRL_MACr, MANGO_CLASS_NON_B_RMK_PRIf, &pSrConf->class_non_b_remark_priority, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_mango_qos_avbStreamReservationConfig_get */

/* Function Name:
 *      dal_mango_qos_avbStreamReservationConfig_set
 * Description:
 *      Set the configuration of Stream Reservation in the specified device.
 * Input:
 *      unit   - unit id
 *      pSrConf - pointer buffer of configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 *      RT_ERR_QUEUE_ID         - invalid queue id
 * Note:
 *      None
 */
int32
dal_mango_qos_avbStreamReservationConfig_set(uint32 unit, rtk_qos_avbSrConf_t *pSrConf)
{
    uint32  value = 0;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSrConf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pSrConf->class_a_priority > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(pSrConf->class_b_priority > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(pSrConf->class_non_a_remark_priority > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(pSrConf->class_non_b_remark_priority > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((pSrConf->class_a_queue_id >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pSrConf->class_b_queue_id >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pSrConf->class_non_a_redirect_queue_id >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pSrConf->class_non_b_redirect_queue_id >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);

    /* set value to CHIP*/
    value=0;
    if ((ret = reg_field_set(unit, MANGO_AVB_CTRLr, MANGO_CLASS_A_PRIf, &pSrConf->class_a_priority, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, MANGO_AVB_CTRLr, MANGO_CLASS_B_PRIf, &pSrConf->class_b_priority, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, MANGO_AVB_CTRLr, MANGO_CLASS_A_QIDf, &pSrConf->class_a_queue_id, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, MANGO_AVB_CTRLr, MANGO_CLASS_B_QIDf, &pSrConf->class_b_queue_id, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_AVB_CTRLr, MANGO_CLASS_NON_A_REDIRECT_QIDf, &pSrConf->class_non_a_redirect_queue_id, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, MANGO_AVB_CTRLr, MANGO_CLASS_NON_B_REDIRECT_QIDf, &pSrConf->class_non_b_redirect_queue_id, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_LOCK(unit);
    if ((ret = reg_write(unit, MANGO_AVB_CTRLr, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    value=0;

    if ((ret = reg_field_set(unit, MANGO_AVB_CTRL_MACr, MANGO_CLASS_NON_A_RMK_PRIf, &pSrConf->class_non_a_remark_priority, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_field_set(unit, MANGO_AVB_CTRL_MACr, MANGO_CLASS_NON_B_RMK_PRIf, &pSrConf->class_non_b_remark_priority, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_LOCK(unit);
    if ((ret = reg_write(unit, MANGO_AVB_CTRL_MACr, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);


    return RT_ERR_OK;
} /* end of dal_mango_qos_avbStreamReservationConfig_set */

/* Function Name:
 *      dal_mango_qos_portQueueStrictEnable_get
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
dal_mango_qos_portQueueStrictEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable)
{
    int32  ret;
    uint32 reg, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, queue=%d", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if (HWP_IS_CPU_PORT(unit, port))
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_SCHED_CPU_Q_CTRLr;
    }
    else if (HWP_UPLINK_PORT(unit, port))
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_SCHED_PORT_Q_CTRL_SET1r;
    }
    else
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_SCHED_PORT_Q_CTRL_SET0r;
    }

    QOS_SEM_LOCK(unit);

    /* read value from CHIP*/
    if (HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = reg_array_field_read(unit, reg, REG_ARRAY_INDEX_NONE, queue, MANGO_STRICT_ENf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_array_field_read(unit, reg, port, queue, MANGO_STRICT_ENf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    if (value == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_mango_qos_portQueueStrictEnable_get */

/* Function Name:
 *      dal_mango_qos_portQueueStrictEnable_set
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
dal_mango_qos_portQueueStrictEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable)
{
    int32   ret;
    uint32  reg, value;;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, queue=%d, enable=%d", unit, port, queue, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (HWP_IS_CPU_PORT(unit, port))
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_SCHED_CPU_Q_CTRLr;
    }
    else if (HWP_UPLINK_PORT(unit, port))
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_SCHED_PORT_Q_CTRL_SET1r;
    }
    else
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_SCHED_PORT_Q_CTRL_SET0r;
    }

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    QOS_SEM_LOCK(unit);

    /* read value from CHIP*/
    if (HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = reg_array_field_write(unit, reg, REG_ARRAY_INDEX_NONE, queue, MANGO_STRICT_ENf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_array_field_write(unit, reg, port, queue, MANGO_STRICT_ENf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_portQueueStrictEnable_set */

/* Function Name:
 *      dal_mango_qos_port1pRemarkEnable_get
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
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_mango_qos_port1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_RMK_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MANGO_IPRI_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_port1pRemarkEnable_set
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
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_mango_qos_port1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_RMK_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MANGO_IPRI_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_1pRemarking_get
 * Description:
 *      Get the internal priority/original inner priority/original outer priority/dscp to
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
dal_mango_qos_1pRemarking_get(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t *pPri)
{
    int32   ret;
    uint32  reg_name, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d", unit, src);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(src >= DOT_1P_RMK_SRC_END, RT_ERR_INPUT);
    if(((DOT_1P_RMK_SRC_DSCP == src) && (val.dscp.val  > 63)) || ((DOT_1P_RMK_SRC_DSCP != src) && (val.pri.val> 7)))
        return RT_ERR_INPUT;

    value = val.pri.val;
    switch(src)
    {
        case DOT_1P_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_USER_PRI:
            reg_name = MANGO_RMK_IPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_OUTER_USER_PRI:
            reg_name = MANGO_RMK_OPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_DSCP:
            reg_name = MANGO_RMK_DSCP2IPRI_CTRLr;
            value = val.dscp.val;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_name, REG_ARRAY_INDEX_NONE, value,
                    MANGO_IPRIf, pPri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pPri);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_1pRemarking_set
 * Description:
 *      Set the internal priority/original inner priority/original outer priority/dscp to
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
dal_mango_qos_1pRemarking_set(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t pri)
{
    int32   ret;
    uint32  reg_name, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d, val=%d, pri=%d", unit, src, val, pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(src >= DOT_1P_RMK_SRC_END, RT_ERR_INPUT);
    if(((DOT_1P_RMK_SRC_DSCP == src) && (val.dscp.val > 63)) || ((DOT_1P_RMK_SRC_DSCP != src) && (val.pri.val > 7)))
        return RT_ERR_INPUT;
    RT_PARAM_CHK(pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    value = val.pri.val;
    switch(src)
    {
        case DOT_1P_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_USER_PRI:
            reg_name = MANGO_RMK_IPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_OUTER_USER_PRI:
            reg_name = MANGO_RMK_OPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_DSCP:
            reg_name = MANGO_RMK_DSCP2IPRI_CTRLr;
            value = val.dscp.val;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, reg_name, REG_ARRAY_INDEX_NONE, value,
                    MANGO_IPRIf, &pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_port1pDfltPri_get
 * Description:
 *      Get default inner-priority of specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pPri    - pointer of default dot1p priority priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_port1pDfltPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri)
{
    int32   ret;
    uint32  idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (HAL_UNIT_TO_DEV_ID(unit) * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_REMARKt, MANGO_REMARK_IPRI_DFLT_PRItf,
                    pPri, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pPri);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_port1pDfltPri_set
 * Description:
 *      Set default inner-priority of specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pri     - default dot1p priority priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_mango_qos_port1pDfltPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri)
{
    int32  ret;
    uint32 idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, pri=%d", unit, pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (HAL_UNIT_TO_DEV_ID(unit) * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* set value to CHIP*/
    if ((ret = table_field_set(unit, MANGO_REMARKt, MANGO_REMARK_IPRI_DFLT_PRItf,
                    &pri, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_write(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_port1pDfltPriExt_get
 * Description:
 *      Get default inner-priority of specified port
 * Input:
 *      unit    - unit id
 *      devID   - devic unit id
 *      port    - port id
 * Output:
 *      pPri    - pointer of default dot1p priority priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      The API works in Stacking architecture.
 */
int32
dal_mango_qos_port1pDfltPriExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t *pPri)
{
    int32   ret;
    uint32  idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(devID >= RTK_MAX_NUM_OF_UNIT, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (devID * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_REMARKt, MANGO_REMARK_IPRI_DFLT_PRItf,
                    pPri, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pPri);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_port1pDfltPriExt_set
 * Description:
 *      Set default inner-priority of specified port
 * Input:
 *      unit    - unit id
 *      devID   - devic unit id
 *      port    - port id
 *      pri     - default dot1p priority priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The API works in Stacking architecture.
 */
int32
dal_mango_qos_port1pDfltPriExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t pri)
{
    int32  ret;
    uint32 idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, pri=%d", unit, pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(devID >= RTK_MAX_NUM_OF_UNIT, RT_ERR_INPUT);
    RT_PARAM_CHK(pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (devID * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* set value to CHIP*/
    if ((ret = table_field_set(unit, MANGO_REMARKt, MANGO_REMARK_IPRI_DFLT_PRItf,
                    &pri, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_write(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_port1pDfltPriSrcSel_get
 * Description:
 *      Get default inner-priority source of specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pSrc    - pointer of default dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_port1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_1pDfltPriSrc_t *pSrc)
{
    int32   ret;
    uint32  value, idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSrc), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (HAL_UNIT_TO_DEV_ID(unit) * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_REMARKt, MANGO_REMARK_IPRI_DFLT_SRCtf,
                    &value, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pSrc = INNER_1P_DFLT_SRC_INT_PRI;
    else if (1 == value)
        *pSrc = INNER_1P_DFLT_SRC_DFLT_PRI;
    else
        return RT_ERR_INPUT;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pSrc=%d", *pSrc);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_port1pDfltPriSrcSel_set
 * Description:
 *      Set default inner-priority source of specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      src     - default dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_mango_qos_port1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_1pDfltPriSrc_t src)
{
    int32  ret;
    uint32 value, idx;
    rmk_entry_t rmkEntry;
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, src=%s", unit, port, src);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    if (INNER_1P_DFLT_SRC_INT_PRI == src)
        value = 0;
    else if (INNER_1P_DFLT_SRC_DFLT_PRI == src)
        value = 1;
    else
        return RT_ERR_INPUT;

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (HAL_UNIT_TO_DEV_ID(unit) * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* set value to CHIP*/
    if ((ret = table_field_set(unit, MANGO_REMARKt, MANGO_REMARK_IPRI_DFLT_SRCtf,
                    &value, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_write(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_port1pDfltPriSrcSelExt_get
 * Description:
 *      Get default inner-priority source of specified port
 * Input:
 *      unit    - unit id
 *      devID   - devic unit id
 *      port    - port id
 * Output:
 *      pSrc    - pointer of default dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      1. The API works in Stacking architecture.
 *      2. Supported default priority sources are as following:
 *      - INNER_1P_DFLT_SRC_INT_PRI
 *      - INNER_1P_DFLT_SRC_DFLT_PRI
 */
int32
dal_mango_qos_port1pDfltPriSrcSelExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_1pDfltPriSrc_t *pSrc)
{
    int32   ret;
    uint32  value, idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, srcUnit=%d, port=%d", unit, devID, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(devID >= RTK_MAX_NUM_OF_UNIT, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pSrc), RT_ERR_NULL_POINTER);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (devID * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_REMARKt, MANGO_REMARK_IPRI_DFLT_SRCtf,
                    &value, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pSrc = INNER_1P_DFLT_SRC_INT_PRI;
    else if (1 == value)
        *pSrc = INNER_1P_DFLT_SRC_DFLT_PRI;
    else
        return RT_ERR_INPUT;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pSrc=%d", *pSrc);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_port1pDfltPriSrcSelExt_set
 * Description:
 *      Set default inner-priority source of specified port
 * Input:
 *      unit    - unit id
 *      devID   - devic unit id
 *      port    - port id
 *      src     - default dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      1. The API works in Stacking architecture.
 *      2. Supported default priority sources are as following:
 *      - INNER_1P_DFLT_SRC_INT_PRI
 *      - INNER_1P_DFLT_SRC_DFLT_PRI
 */
int32
dal_mango_qos_port1pDfltPriSrcSelExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_1pDfltPriSrc_t src)
{
    int32  ret;
    uint32 value, idx;
    rmk_entry_t rmkEntry;
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, srcUnit=%d, port=%d, src=%s", unit, devID, port, src);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(devID >= RTK_MAX_NUM_OF_UNIT, RT_ERR_INPUT);

    if (INNER_1P_DFLT_SRC_INT_PRI == src)
        value = 0;
    else if (INNER_1P_DFLT_SRC_DFLT_PRI == src)
        value = 1;
    else
        return RT_ERR_INPUT;

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (devID * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* set value to CHIP*/
    if ((ret = table_field_set(unit, MANGO_REMARKt, MANGO_REMARK_IPRI_DFLT_SRCtf,
                    &value, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_write(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_1pDfltPriCfgSrcSel_get
 * Description:
 *      Get default inner priority configuration
 * Input:
 *      unit  - unit id
 * Output:
 *      pCfg  - pointer of default dot1p priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_1pDfltCfgSrc_t *pCfg)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_RMK_CTRLr, MANGO_IPRI_DFLT_CFGf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pCfg = INNER_1P_DFLT_CFG_SRC_INGRESS;
    else
        *pCfg = INNER_1P_DFLT_CFG_SRC_EGRESS;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pCfg=%d", *pCfg);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_1pDfltPriCfgSrcSel_set
 * Description:
 *      Set default inner priority configuration
 * Input:
 *      unit - unit id
 *      cfg  - default dot1p priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_1pDfltCfgSrc_t cfg)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, cfg=%d", unit, cfg);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(cfg > INNER_1P_DFLT_CFG_SRC_END, RT_ERR_INPUT);

    if (INNER_1P_DFLT_CFG_SRC_INGRESS == cfg)
        value = 0;
    else
        value = 1;

    QOS_SEM_LOCK(unit);

    /* program value to chip */
    if ((ret = reg_field_write(unit, MANGO_RMK_CTRLr, MANGO_IPRI_DFLT_CFGf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portOut1pRemarkEnable_get
 * Description:
 *      Get enable status of outer dot1p remarking on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id.
 * Output:
 *      pEnable - pointer to enable status of outer dot1p remarking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_qos_portOut1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_RMK_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MANGO_OPRI_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portOut1pRemarkEnable_set
 * Description:
 *      Set enable status of outer dot1p remarking on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id.
 *      enable - enable status of outer dot1p remarking
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_qos_portOut1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_RMK_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MANGO_OPRI_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_outer1pRemarking_get
 * Description:
 *      Get the internal priority/original inner priority/original outer priority/dscp to
 *      remarkd outer 802.1p priority(3bits) mapping.
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
 *      None
 */
int32
dal_mango_qos_outer1pRemarking_get(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t *pPri)
{
    int32   ret;
    uint32  reg_name,value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d", unit, src);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(src >= OUTER_1P_RMK_SRC_END, RT_ERR_INPUT);
    if(((OUTER_1P_RMK_SRC_DSCP == src) && (val.dscp.val > 63)) || ((OUTER_1P_RMK_SRC_DSCP != src) && (val.pri.val > 7)))
        return RT_ERR_INPUT;

    value = val.pri.val;
    switch(src)
    {
        case OUTER_1P_RMK_SRC_INT_PRI:        /* Internal to Outer */
            reg_name = MANGO_RMK_INTPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_USER_PRI:       /* Outer to Outer */
            reg_name = MANGO_RMK_OPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_INNER_USER_PRI: /* Inner to Outer */
            reg_name = MANGO_RMK_IPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_DSCP:
            reg_name = MANGO_RMK_DSCP2OPRI_CTRLr;
                value = val.dscp.val;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_name, REG_ARRAY_INDEX_NONE, value,
                    MANGO_OPRIf, pPri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pPri);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_outer1pRemarking_set
 * Description:
 *      Set the internal priority/original inner priority/original outer priority/dscp to
 *      remarkd outer 802.1p priority(3bits) mapping.
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
 *      None
 */
int32
dal_mango_qos_outer1pRemarking_set(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t pri)
{
    int32   ret;
    uint32  reg_name,value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, pri=%d", unit, pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(src >= OUTER_1P_RMK_SRC_END, RT_ERR_INPUT);
    if(((OUTER_1P_RMK_SRC_DSCP == src) && (val.dscp.val > 63)) || ((OUTER_1P_RMK_SRC_DSCP != src) && (val.pri.val > 7)))
        return RT_ERR_INPUT;
    RT_PARAM_CHK(pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    value = val.pri.val;
    switch(src)
    {
        case OUTER_1P_RMK_SRC_INT_PRI:       /* Internal to Outer */
            reg_name = MANGO_RMK_INTPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_USER_PRI:       /* Outer to Outer */
            reg_name = MANGO_RMK_OPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_INNER_USER_PRI: /* Inner to Outer */
            reg_name = MANGO_RMK_IPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_DSCP:
            reg_name = MANGO_RMK_DSCP2OPRI_CTRLr;
            value = val.dscp.val;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, reg_name, REG_ARRAY_INDEX_NONE, value,
                    MANGO_OPRIf, &pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portOuter1pDfltPri_get
 * Description:
 *      Get default outer-priority source and priority of specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pPri - pointer of default outer dot1p priority priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_portOuter1pDfltPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pPri)
{
    int32   ret;
    uint32  idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (HAL_UNIT_TO_DEV_ID(unit) * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_REMARKt, MANGO_REMARK_OPRI_DFLT_PRItf,
                    pPri, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pPri);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portOuter1pDfltPri_set
 * Description:
 *      Set default outer-priority of specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pri     - default outer dot1p priority priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_mango_qos_portOuter1pDfltPri_set(uint32 unit, rtk_port_t port, rtk_pri_t pri)
{
    int32       ret;
    uint32      idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, pri=%d", unit, pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (HAL_UNIT_TO_DEV_ID(unit) * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* set value to CHIP*/
    if ((ret = table_field_set(unit, MANGO_REMARKt, MANGO_REMARK_OPRI_DFLT_PRItf,
                    &pri, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_write(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portOuter1pDfltPriExt_get
 * Description:
 *      Get default outer-priority of specified port
 * Input:
 *      unit    - unit id
 *      devID   - devic unit id
 *      port    - port id
 * Output:
 *      pPri    - pointer of default outer dot1p priority priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
*      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      1. The API works in Stacking architecture.
 */
int32
dal_mango_qos_portOuter1pDfltPriExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t *pPri)
{
    int32   ret;
    uint32  idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(devID >= RTK_MAX_NUM_OF_UNIT, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (devID * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_REMARKt, MANGO_REMARK_OPRI_DFLT_PRItf,
                    pPri, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pPri);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portOuter1pDfltPriExt_set
 * Description:
 *      Set default outer-priority of specified port
 * Input:
 *      unit    - unit id
 *      devID   - devic unit id
 *      port    - port id
 *      pri     - default outer dot1p priority priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      1. The API works in Stacking architecture.
 */
int32
dal_mango_qos_portOuter1pDfltPriExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_pri_t pri)
{
    int32       ret;
    uint32      idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, pri=%d", unit, pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(devID >= RTK_MAX_NUM_OF_UNIT, RT_ERR_INPUT);
    RT_PARAM_CHK(pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (devID * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* set value to CHIP*/
    if ((ret = table_field_set(unit, MANGO_REMARKt, MANGO_REMARK_OPRI_DFLT_PRItf,
                    &pri, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_write(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portDscpRemarkEnable_get
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
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_mango_qos_portDscpRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_RMK_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MANGO_DSCP_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portDscpRemarkEnable_set
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
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_mango_qos_portDscpRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_RMK_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MANGO_DSCP_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_dscpRemarking_get
 * Description:
 *      Get the internal priority/original inner priority/original outer priority/dscp/dp/internal priority and dp to
 *      remarkd DSCP mapping.
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
dal_mango_qos_dscpRemarking_get(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 *pDscp)
{
    int32   ret;
    uint32  reg_name,value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d", unit, src);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((src >= DSCP_RMK_SRC_END)||(src == DSCP_RMK_SRC_DP)), RT_ERR_INPUT);
    if ((DSCP_RMK_SRC_DP_INT_PRI == src) && ((val.intPriDp.dp > 2)||(val.intPriDp.pri > 7)))
        return RT_ERR_INPUT;
    if (((DSCP_RMK_SRC_DSCP == src) && (val.dscp.val > 63)) || ((DSCP_RMK_SRC_DSCP != src) && (val.pri.val > 7)))
        return RT_ERR_INPUT;

    switch (src)
    {
        case DSCP_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2DSCP_CTRLr;
            value = val.pri.val;
            break;
        case DSCP_RMK_SRC_USER_PRI:
            reg_name = MANGO_RMK_IPRI2DSCP_CTRLr;
            value = val.pri.val;
            break;
        case DSCP_RMK_SRC_OUTER_USER_PRI:
            reg_name = MANGO_RMK_OPRI2DSCP_CTRLr;
            value = val.pri.val;
            break;
        case DSCP_RMK_SRC_DP_INT_PRI:
            reg_name = MANGO_RMK_DPINTPRI2DSCP_CTRLr;
            value = val.intPriDp.pri;
            break;
        case DSCP_RMK_SRC_DSCP:
            reg_name = MANGO_RMK_DSCP2DSCP_CTRLr;
            value = val.dscp.val;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    if (DSCP_RMK_SRC_DP_INT_PRI == src)
    {
        /* get value from CHIP*/
        if ((ret = reg_array_field_read(unit, reg_name, val.intPriDp.dp, value,
                        MANGO_DSCPf, pDscp)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }
    else
    {
        /* get value from CHIP*/
        if ((ret = reg_array_field_read(unit, reg_name, REG_ARRAY_INDEX_NONE, value,
                        MANGO_DSCPf, pDscp)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDscp=%d", *pDscp);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_dscpRemarking_set
 * Description:
 *      Set the internal priority/original inner priority/original outer priority/dscp/dp/internal priority and dp to
 *      remarkd DSCP mapping.
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
dal_mango_qos_dscpRemarking_set(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 dscp)
{
    int32   ret;
    uint32  reg_name, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d", unit, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK(((src >= DSCP_RMK_SRC_END)||(src == DSCP_RMK_SRC_DP)), RT_ERR_INPUT);
    if ((DSCP_RMK_SRC_DP_INT_PRI == src) && ((val.intPriDp.dp > 2)||(val.intPriDp.pri > 7)))
        return RT_ERR_INPUT;
    if (((DSCP_RMK_SRC_DSCP == src) && (val.dscp.val > 63)) || ((DSCP_RMK_SRC_DSCP != src) && (val.pri.val > 7)))
        return RT_ERR_INPUT;

    switch(src)
    {
        case DSCP_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2DSCP_CTRLr;
            value = val.pri.val;
            break;
        case DSCP_RMK_SRC_USER_PRI:
            reg_name = MANGO_RMK_IPRI2DSCP_CTRLr;
            value = val.pri.val;
            break;
        case DSCP_RMK_SRC_OUTER_USER_PRI:
            reg_name = MANGO_RMK_OPRI2DSCP_CTRLr;
            value = val.pri.val;
            break;
        case DSCP_RMK_SRC_DP_INT_PRI:
            reg_name = MANGO_RMK_DPINTPRI2DSCP_CTRLr;
            value = val.intPriDp.pri;
            break;
        case DSCP_RMK_SRC_DSCP:
            reg_name = MANGO_RMK_DSCP2DSCP_CTRLr;
            value = val.dscp.val;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    if (DSCP_RMK_SRC_DP_INT_PRI == src)
    {
        /* program value to CHIP*/
        if ((ret = reg_array_field_write(unit, reg_name, val.intPriDp.dp, value,
                        MANGO_DSCPf, &dscp)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }
    else
    {
        /* program value to CHIP*/
        if ((ret = reg_array_field_write(unit, reg_name, REG_ARRAY_INDEX_NONE, value,
                        MANGO_DSCPf, &dscp)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portDeiRemarkEnable_get
 * Description:
 *      Get DEI remark status for a port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of DEI remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    The status of DEI remark:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_mango_qos_portDeiRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_RMK_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MANGO_DEI_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portDeiRemarkEnable_set
 * Description:
 *      Set DEI remark status for a port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of DEI remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *    The status of DEI remark:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_mango_qos_portDeiRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_RMK_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    MANGO_DEI_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_deiRemarking_get
 * Description:
 *      Get the internal priority/dp to DEI remarking mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 * Output:
 *      pDei    - remarked DEI value
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
dal_mango_qos_deiRemarking_get(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 *pDei)
{
    int32   ret;
    uint32  reg_name, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d", unit, src);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDei), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(src >= DEI_RMK_SRC_END, RT_ERR_INPUT);
    if(((DEI_RMK_SRC_DP == src) && (val.dp.val > 2)) || ((DEI_RMK_SRC_INT_PRI == src) && (val.pri.val > 7)))
        return RT_ERR_INPUT;

    switch(src)
    {
        case DEI_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2DEI_CTRLr;
            value = val.pri.val;
            break;
        case DEI_RMK_SRC_DP:
            reg_name = MANGO_RMK_DP2DEI_CTRLr;
            value = val.dp.val;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_name, REG_ARRAY_INDEX_NONE, value,
                    MANGO_DEIf, pDei)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDei=%d", *pDei);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_deiRemarking_set
 * Description:
 *      Set the internal priority/dp to DEI remarking mapping.
 * Input:
 *      unit    - unit id
 *      src     - remark source
 *      val     - remark source value
 *      dei     - remarked DEI value
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
dal_mango_qos_deiRemarking_set(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 dei)
{
    int32   ret;
    uint32  reg_name, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d, dei=%d",
           unit, src, dei);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dei > RTK_VALUE_OF_DEI_MAX, RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(src >= DEI_RMK_SRC_END, RT_ERR_INPUT);
    if(((DEI_RMK_SRC_DP == src) && (val.dp.val > 2)) || ((DEI_RMK_SRC_INT_PRI == src) && (val.pri.val > 7)))
        return RT_ERR_INPUT;

    switch(src)
    {
        case DEI_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2DEI_CTRLr;
            value = val.pri.val;
            break;
        case DEI_RMK_SRC_DP:
            reg_name = MANGO_RMK_DP2DEI_CTRLr;
            value = val.dp.val;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, reg_name, REG_ARRAY_INDEX_NONE, value,
                    MANGO_DEIf, &dei)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_dscpRemarkSrcSel_get
 * Description:
 *      Get remarking source of DEI remarking.
 * Input:
 *      unit  - unit id
 * Output:
 *      pType - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      After specifing the remarking source, please use the corresponding API to specify the remarking mapping.
 */
int32
dal_mango_qos_deiRemarkSrcSel_get(uint32 unit, rtk_qos_deiRmkSrc_t *pType)
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
    if ((ret = reg_field_read(unit, MANGO_RMK_CTRLr, MANGO_DEI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = DEI_RMK_SRC_INT_PRI;
    else if (1 == value)
        *pType = DEI_RMK_SRC_DP;
    else
        return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_dscpRemarkSrcSel_set
 * Description:
 *      Set remarking source of DEI remarking.
 * Input:
 *      unit - unit id
 *      type - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      After specifing the remarking source, please use the corresponding API to specify the remarking mapping.
 */
int32
dal_mango_qos_deiRemarkSrcSel_set(uint32 unit, rtk_qos_deiRmkSrc_t type)
{
    int32   ret;
    uint32  value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d",
           unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= DEI_RMK_SRC_END, RT_ERR_INPUT);

    if (DEI_RMK_SRC_INT_PRI == type)
    {
        value = 0;
    }
    else if (DEI_RMK_SRC_DP == type)
    {
        value = 1;
    }
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_field_write(unit, MANGO_RMK_CTRLr, MANGO_DEI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_1pRemarkSrcSel_get
 * Description:
 *      Get the remarking source of 802.1p priority(3bits) remarking.
 * Input:
 *      unit    - unit id
 * Output:
 *      pType   - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pRemarkSrcSel_get(uint32 unit, rtk_qos_1pRmkSrc_t *pType)
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
    if ((ret = reg_field_read(unit, MANGO_RMK_CTRLr, MANGO_IPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = DOT_1P_RMK_SRC_INT_PRI;
    else if (1 == value)
        *pType = DOT_1P_RMK_SRC_USER_PRI;
    else if (2 == value)
        *pType = DOT_1P_RMK_SRC_OUTER_USER_PRI;
    else if (3 == value)
        *pType = DOT_1P_RMK_SRC_DSCP;
    else
        return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_mango_qos_1pRemarkSrcSel_get */

/* Function Name:
 *      dal_mango_qos_1pRemarkSrcSel_set
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
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pRemarkSrcSel_set(uint32 unit, rtk_qos_1pRmkSrc_t type)
{
    int32   ret;
    uint32  value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d",
           unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= DOT_1P_RMK_SRC_END, RT_ERR_INPUT);

    if (DOT_1P_RMK_SRC_INT_PRI == type)
    {
        value = 0;
    }
    else if (DOT_1P_RMK_SRC_USER_PRI == type)
    {
        value = 1;
    }
    else if (DOT_1P_RMK_SRC_OUTER_USER_PRI == type)
    {
        value = 2;
    }
    else if (DOT_1P_RMK_SRC_DSCP == type)
    {
        value = 3;
    }
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_field_write(unit, MANGO_RMK_CTRLr, MANGO_IPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_1pRemarkSrcSel_set */

/* Function Name:
 *      dal_mango_qos_portDeiRemarkTagSel_get
 * Description:
 *      Get DEI remarking tag selection of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pType  - type of DEI
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
dal_mango_qos_portDeiRemarkTagSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t *pType)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_RMK_PORT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MANGO_DEI_RMK_TAG_SELf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = DEI_SEL_INNER_TAG;
    else if (1 == value)
        *pType = DEI_SEL_OUTER_TAG;
    else
        return RT_ERR_FAILED;

    return RT_ERR_OK;
} /* end of dal_mango_qos_portDeiRemarkTagSel_get */

/* Function Name:
 *      dal_mango_qos_portDeiRemarkTagSel_set
 * Description:
 *      Set DEI remarking tag selection of specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - type of DEI
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_qos_portDeiRemarkTagSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, type=%u"
            , unit, port, type);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type >= DEI_SEL_END, RT_ERR_INPUT);

    if (DEI_SEL_INNER_TAG == type)
        value = 0;
    else if (DEI_SEL_OUTER_TAG == type)
        value = 1;
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_RMK_PORT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MANGO_DEI_RMK_TAG_SELf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_portDeiRemarkTagSel_set */

/* Function Name:
 *      dal_mango_qos_outer1pRemarkSrcSel_get
 * Description:
 *      Get the remarking source of outer dot1p priority remarking.
 * Input:
 *      unit    - unit id
 * Output:
 *      pType   - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_outer1pRemarkSrcSel_get(uint32 unit, rtk_qos_outer1pRmkSrc_t *pType)
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
    if ((ret = reg_field_read(unit, MANGO_RMK_CTRLr, MANGO_OPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = OUTER_1P_RMK_SRC_INT_PRI;
    else if (1 == value)
        *pType = OUTER_1P_RMK_SRC_INNER_USER_PRI; /* inner-priority */
    else if (2 == value)
        *pType = OUTER_1P_RMK_SRC_USER_PRI;  /* outer-priority */
    else if (3 == value)
        *pType = OUTER_1P_RMK_SRC_DSCP;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_mango_qos_out1pRemarkSrcSel_get */

/* Function Name:
 *      dal_mango_qos_outer1pRemarkSrcSel_set
 * Description:
 *      Set the remarking source of outer dot1p priority remarking.
 * Input:
 *      unit    - unit id
 *      type    - remarking source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_mango_qos_outer1pRemarkSrcSel_set(uint32 unit, rtk_qos_outer1pRmkSrc_t type)
{
    int32   ret;
    uint32  value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d",
           unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= OUTER_1P_RMK_SRC_END, RT_ERR_INPUT);

    if (OUTER_1P_RMK_SRC_INT_PRI == type)
    {
        value = 0;
    }
    else if (OUTER_1P_RMK_SRC_INNER_USER_PRI == type) /* inner-priority */
    {
        value = 1;
    }
    else if (OUTER_1P_RMK_SRC_USER_PRI == type) /* outer-priority */
    {
        value = 2;
    }
    else if (OUTER_1P_RMK_SRC_DSCP == type)
    {
        value = 3;
    }
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_field_write(unit, MANGO_RMK_CTRLr, MANGO_OPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_out1pRemarkSrcSel_set */

/* Function Name:
 *      dal_mango_qos_outer1pDfltPriCfgSrcSel_get
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
dal_mango_qos_outer1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t *pDflt_sel)
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
    if ((ret = reg_field_read(unit, MANGO_RMK_CTRLr, MANGO_OPRI_DFLT_CFGf, &value)) != RT_ERR_OK)
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
} /* end of rtk_qos_outer1pDfltPriCfgSrcSel_get */

/* Function Name:
 *      dal_mango_qos_outer1pDfltPriCfgSrcSel_set
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
dal_mango_qos_outer1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t dflt_sel)
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
    if ((ret = reg_field_write(unit, MANGO_RMK_CTRLr, MANGO_OPRI_DFLT_CFGf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_outer1pDfltPriCfgSrcSel_set */

/* Function Name:
 *      dal_mango_qos_dscpRemarkSrcSel_get
 * Description:
 *      Get the remarking source of DSCP remarking.
 * Input:
 *      unit    - unit id
 * Output:
 *      pType   - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_dscpRemarkSrcSel_get(uint32 unit, rtk_qos_dscpRmkSrc_t *pType)
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
    if ((ret = reg_field_read(unit, MANGO_RMK_CTRLr, MANGO_DSCP_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pType = DSCP_RMK_SRC_INT_PRI;
            break;
        case 1:
            *pType = DSCP_RMK_SRC_USER_PRI;
            break;
        case 2:
            *pType = DSCP_RMK_SRC_OUTER_USER_PRI;
            break;
        case 3:
            *pType = DSCP_RMK_SRC_DSCP;
            break;
        case 4:
            *pType = DSCP_RMK_SRC_DP;
            break;
        case 5:
            *pType = DSCP_RMK_SRC_DP_INT_PRI;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_mango_qos_dscpRemarkSrcSel_get */

/* Function Name:
 *      dal_mango_qos_dscpRemarkSrcSel_set
 * Description:
 *      Set the remarking source of DSCP remarking.
 * Input:
 *      unit    - unit id
 *      type    - remarking source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_mango_qos_dscpRemarkSrcSel_set(uint32 unit, rtk_qos_dscpRmkSrc_t type)
{
    int32   ret;
    uint32  value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d",
           unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= DSCP_RMK_SRC_END, RT_ERR_INPUT);

    switch (type)
    {
        case DSCP_RMK_SRC_INT_PRI:
            value = 0;
            break;
        case DSCP_RMK_SRC_USER_PRI:
            value = 1;
            break;
        case DSCP_RMK_SRC_OUTER_USER_PRI:
            value = 2;
            break;
        case DSCP_RMK_SRC_DSCP:
            value = 3;
            break;
        case DSCP_RMK_SRC_DP:
            value = 4;
            break;
        case DSCP_RMK_SRC_DP_INT_PRI:
            value = 5;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_field_write(unit, MANGO_RMK_CTRLr, MANGO_DSCP_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_dscpRemarkSrcSel_set */

/* Function Name:
 *      dal_mango_qos_portOuter1pDfltPriSrcSel_get
 * Description:
 *      Get default outer-priority source of specified port
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pType      - default outer dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      Supported default priority source is as following:
 *      - rtk_qos_outer1pDfltSrc_t
 *      - OUTER_1P_DFLT_SRC_INT_PRI
 *      - OUTER_1P_DFLT_SRC_USER_PRI
 *      - OUTER_1P_DFLT_SRC_DFLT_PRI
 */
int32
dal_mango_qos_portOuter1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pType)
{
    int32   ret;
    uint32  value, idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (HAL_UNIT_TO_DEV_ID(unit) * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_REMARKt, MANGO_REMARK_OPRI_DFLT_SRCtf,
                    &value, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = OUTER_1P_DFLT_SRC_INT_PRI;
    else if (1 == value)
        *pType = OUTER_1P_DFLT_SRC_DFLT_PRI;
    else
        *pType = OUTER_1P_DFLT_SRC_USER_PRI;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_mango_qos_portOuter1pDfltPriSrcSel_get */

/* Function Name:
 *      dal_mango_qos_portOuter1pDfltPriSrcSel_set
 * Description:
 *      Set default outer-priority source of specified port
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      type      - default outer dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Supported default priority source is as following:
 *      - rtk_qos_outer1pDfltSrc_t
 *      - OUTER_1P_DFLT_SRC_INT_PRI
 *      - OUTER_1P_DFLT_SRC_USER_PRI
 *      - OUTER_1P_DFLT_SRC_DFLT_PRI
 */
int32
dal_mango_qos_portOuter1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t type)
{
    int32       ret;
    uint32      value, idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    if (OUTER_1P_DFLT_SRC_INT_PRI == type)
        value = 0;
    else if (OUTER_1P_DFLT_SRC_DFLT_PRI == type)
        value = 1;
    else if (OUTER_1P_DFLT_SRC_USER_PRI == type)
        value = 2;
    else
        return RT_ERR_INPUT;

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (HAL_UNIT_TO_DEV_ID(unit) * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* set value to CHIP*/
    if ((ret = table_field_set(unit, MANGO_REMARKt, MANGO_REMARK_OPRI_DFLT_SRCtf,
                    &value, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_write(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_portOuter1pDfltPriSrcSel_set */


/* Function Name:
 *      dal_mango_qos_portOuter1pDfltPriSrcSelExt_get
 * Description:
 *      Get default outer-priority source of specified port
 * Input:
 *      unit    - unit id
 *      devID   - devic unit id
 *      port    - port id
 * Output:
 *      pSrc - pointer of default outer dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. The API works in Stacking architecture.
 *      2. Supported default priority source is as following:
 *      - OUTER_1P_DFLT_SRC_INT_PRI
 *      - OUTER_1P_DFLT_SRC_USER_PRI
 *      - OUTER_1P_DFLT_SRC_DFLT_PRI
 */
int32
dal_mango_qos_portOuter1pDfltPriSrcSelExt_get(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pSrc)
{
    int32   ret;
    uint32  value, idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(devID >= RTK_MAX_NUM_OF_UNIT, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pSrc), RT_ERR_NULL_POINTER);

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (devID * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_REMARKt, MANGO_REMARK_OPRI_DFLT_SRCtf,
                    &value, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pSrc = OUTER_1P_DFLT_SRC_INT_PRI;
    else if (1 == value)
        *pSrc = OUTER_1P_DFLT_SRC_DFLT_PRI;
    else
        *pSrc = OUTER_1P_DFLT_SRC_USER_PRI;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pSrc=%d", *pSrc);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_portOuter1pDfltPriSrcSelExt_set
 * Description:
 *      Set default outer-priority source of specified port
 * Input:
 *      unit    - unit id
 *      devID   - devic unit id
 *      port    - port id
 *      src     - default outer dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      1. The API works in Stacking architecture.
 *      2. Supported default priority source is as following:
 *      - OUTER_1P_DFLT_SRC_INT_PRI
 *      - OUTER_1P_DFLT_SRC_USER_PRI
 *      - OUTER_1P_DFLT_SRC_DFLT_PRI
 */
int32
dal_mango_qos_portOuter1pDfltPriSrcSelExt_set(uint32 unit, uint32 devID, rtk_port_t port, rtk_qos_outer1pDfltSrc_t src)
{
    int32       ret;
    uint32      value, idx;
    rmk_entry_t rmkEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(devID >= RTK_MAX_NUM_OF_UNIT, RT_ERR_INPUT);

    if (OUTER_1P_DFLT_SRC_INT_PRI == src)
        value = 0;
    else if (OUTER_1P_DFLT_SRC_DFLT_PRI == src)
        value = 1;
    else if (OUTER_1P_DFLT_SRC_USER_PRI == src)
        value = 2;
    else
        return RT_ERR_INPUT;

    osal_memset(&rmkEntry, 0, sizeof(rmkEntry));
    idx = (devID * RTK_MAX_PORT_PER_UNIT) + port;

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* set value to CHIP*/
    if ((ret = table_field_set(unit, MANGO_REMARKt, MANGO_REMARK_OPRI_DFLT_SRCtf,
                    &value, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_write(unit, MANGO_REMARKt, idx, (uint32 *) &rmkEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_1pDfltPri_get
 * Description:
 *      Get default inner-priority value
 * Input:
 *      unit       - unit id
 * Output:
 *      pDot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* get value from CHIP*/
    if ((ret = dal_mango_qos_port1pDfltPriExt_get(unit, 0, 0, pDot1p_pri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_1pDfltPri_set
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
 *      RT_ERR_QOS_1P_PRIORITY       - Invalid dot1p priority
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri)
{
    int32   ret;
    uint32  srcUnit;
    rtk_port_t  port;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* set value to chip */
    for (srcUnit = 0; srcUnit < RTK_MAX_NUM_OF_UNIT; srcUnit++)
    {
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        {
            if ((ret = dal_mango_qos_port1pDfltPriExt_set(unit, srcUnit, port, dot1p_pri)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_1pDfltPriSrcSel_get
 * Description:
 *      Get default inner-priority source
 * Input:
 *      unit       - unit id
 * Output:
 *      pType      - default inner dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pDfltPriSrcSel_get(uint32 unit, rtk_qos_1pDfltPriSrc_t *pType)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* get value from CHIP*/
    if ((ret = dal_mango_qos_port1pDfltPriExt_get(unit, 0, 0, pType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_1pDfltPriSrcSel_set
 * Description:
 *      Set default inner-priority source
 * Input:
 *      unit      - unit id
 *      type      - default inner dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pDfltPriSrcSel_set(uint32 unit, rtk_qos_1pDfltPriSrc_t type)
{
    int32   ret;
    uint32  srcUnit;
    rtk_port_t  port;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* set value to chip */
    for (srcUnit = 0; srcUnit < RTK_MAX_NUM_OF_UNIT; srcUnit++)
    {
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        {
            if ((ret = dal_mango_qos_port1pDfltPriExt_set(unit, srcUnit, port, type)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_outer1pDfltPri_get
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
dal_mango_qos_outer1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* get value from CHIP*/
    if ((ret = dal_mango_qos_portOuter1pDfltPri_get(unit, 0, pDot1p_pri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_outer1pDfltPri_set
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
dal_mango_qos_outer1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri)
{
    int32   ret;
    rtk_port_t  port;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* set value to chip */
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = dal_mango_qos_portOuter1pDfltPri_set(unit, port, dot1p_pri)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

#if 0
/* Function Name:
 *      dal_mango_qos_1pDfltPri_get
 * Description:
 *      Get default inner-priority value
 * Input:
 *      unit       - unit id
 * Output:
 *      pDot1p_pri - default 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri)
{
    int32   ret;
    uint32  i, got = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, i)
    {
        QOS_SEM_LOCK(unit);
        /* get value from CHIP*/
        if ((ret = reg_array_field_read(unit, MANGO_RMK_PORT_CTRLr,
            i, REG_ARRAY_INDEX_NONE, MANGO_IPRI_DFLT_PRIf, pDot1p_pri)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        QOS_SEM_UNLOCK(unit);

        got = 1;
        break;
    }

    if(!got)
      return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_mango_qos_1pDfltPri_get */

/* Function Name:
 *      dal_mango_qos_1pDfltPri_set
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
 *      RT_ERR_QOS_1P_PRIORITY       - Invalid dot1p priority
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri)
{
    int32       ret;
    uint32      i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d", unit, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, i)
    {
        QOS_SEM_LOCK(unit);
        /* get value from CHIP*/
        if ((ret = reg_array_field_write(unit, MANGO_RMK_PORT_CTRLr,
            i, REG_ARRAY_INDEX_NONE, MANGO_IPRI_DFLT_PRIf, &dot1p_pri)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        QOS_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
} /* end of dal_mango_qos_1pDfltPri_set */

/* Function Name:
 *      dal_mango_qos_1pRemark_get
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
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri)
{
    int32   ret;
    uint32  reg_name;
    rtk_qos_1pRmkSrc_t  type;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d", unit, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    if((ret = dal_mango_qos_1pRemarkSrcSel_get(unit, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    switch(type)
    {
        case DOT_1P_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_USER_PRI:
            reg_name = MANGO_RMK_IPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_OUTER_USER_PRI:
            reg_name = MANGO_RMK_OPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_DSCP:
            reg_name = MANGO_RMK_DSCP2IPRI_CTRLr;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_name, REG_ARRAY_INDEX_NONE, int_pri,
                    MANGO_IPRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_mango_qos_1pRemark_get */

/* Function Name:
 *      dal_mango_qos_1pRemark_set
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
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      802.1p remark functionality can map the internal priority or original tag priority
 *      to 802.1p priority before a packet is going to be transmited.
 */
int32
dal_mango_qos_1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri)
{
    int32   ret;
    uint32  reg_name;
    rtk_qos_1pRmkSrc_t type;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d, dot1p_pri=%d",
           unit, int_pri, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    if((ret = dal_mango_qos_1pRemarkSrcSel_get(unit, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    switch(type)
    {
        case DOT_1P_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_USER_PRI:
            reg_name = MANGO_RMK_IPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_OUTER_USER_PRI:
            reg_name = MANGO_RMK_OPRI2IPRI_CTRLr;
            break;
        case DOT_1P_RMK_SRC_DSCP:
            reg_name = MANGO_RMK_DSCP2IPRI_CTRLr;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, reg_name, REG_ARRAY_INDEX_NONE, int_pri,
                    MANGO_IPRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_1pRemark_set */

/* Function Name:
 *      dal_mango_qos_outer1pRemark_get
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
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_outer1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri)
{
    int32   ret;
    uint32  reg_name;
    rtk_qos_outer1pRmkSrc_t type;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d", unit, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    if((ret = dal_mango_qos_outer1pRemarkSrcSel_get(unit, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    switch(type)
    {
        case OUTER_1P_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_USER_PRI:
            reg_name = MANGO_RMK_IPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_INNER_USER_PRI:
            reg_name = MANGO_RMK_OPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_DSCP:
            reg_name = MANGO_RMK_DSCP2OPRI_CTRLr;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_name, REG_ARRAY_INDEX_NONE, int_pri,
                    MANGO_OPRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_mango_qos_outer1pRemark_get */

/* Function Name:
 *      dal_mango_qos_outer1pRemark_set
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
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      Outer dot1p remark functionality can map the internal priority or original tag priority
 *      to outer dot1p priority before a packet is going to be transmited.
 */
int32
dal_mango_qos_outer1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri)
{
    int32   ret;
    uint32  reg_name;
    rtk_qos_outer1pRmkSrc_t type;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d, dot1p_pri=%d",
           unit, int_pri, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    if((ret = dal_mango_qos_outer1pRemarkSrcSel_get(unit, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    switch(type)
    {
        case OUTER_1P_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_USER_PRI:
            reg_name = MANGO_RMK_IPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_INNER_USER_PRI:
            reg_name = MANGO_RMK_OPRI2OPRI_CTRLr;
            break;
        case OUTER_1P_RMK_SRC_DSCP:
            reg_name = MANGO_RMK_DSCP2OPRI_CTRLr;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, reg_name, REG_ARRAY_INDEX_NONE, int_pri,
                    MANGO_OPRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_outer1pRemark_set */

/* Function Name:
 *      dal_mango_qos_1pDfltPriSrcSel_get
 * Description:
 *      Get default inner-priority source
 * Input:
 *      unit       - unit id
 * Output:
 *      pType      - default inner dot1p priority source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pDfltPriSrcSel_get(uint32 unit, rtk_qos_1pDfltPriSrc_t *pType)
{
    int32   ret;
    uint32  value, i, got = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, i)
    {
        QOS_SEM_LOCK(unit);
        /* get value from CHIP*/
        if ((ret = reg_array_field_read(unit, MANGO_RMK_PORT_CTRLr,
            i, REG_ARRAY_INDEX_NONE, MANGO_IPRI_DFLT_SRCf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        QOS_SEM_UNLOCK(unit);

        got = 1;
        break;
    }

    if(!got)
      return RT_ERR_FAILED;

    if (0 == value)
        *pType = INNER_1P_DFLT_SRC_INT_PRI;
    else if (1 == value)
        *pType = INNER_1P_DFLT_SRC_DFLT_PRI;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_mango_qos_portOuter1pDfltPriSrcSel_get */

/* Function Name:
 *      dal_mango_qos_1pDfltPriSrcSel_set
 * Description:
 *      Set default inner-priority source
 * Input:
 *      unit      - unit id
 *      type      - default inner dot1p priority source
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_mango_qos_1pDfltPriSrcSel_set(uint32 unit, rtk_qos_1pDfltPriSrc_t type)
{
    int32   ret;
    uint32  value, i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= INNER_1P_DFLT_SRC_END, RT_ERR_INPUT);

    switch (type)
    {
        case INNER_1P_DFLT_SRC_INT_PRI:
            value = 0;
            break;
        case INNER_1P_DFLT_SRC_DFLT_PRI:
            value = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, i)
    {
        QOS_SEM_LOCK(unit);
        /* get value from CHIP*/
        if ((ret = reg_array_field_write(unit, MANGO_RMK_PORT_CTRLr,
            i, REG_ARRAY_INDEX_NONE, MANGO_IPRI_DFLT_SRCf, &value)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        QOS_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_qos_outer1pDfltPri_get
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
dal_mango_qos_outer1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri)
{
    int32   ret;
    uint32  i, got = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, i)
    {
        QOS_SEM_LOCK(unit);
        /* get value from CHIP*/
        if ((ret = reg_array_field_read(unit, MANGO_RMK_PORT_CTRLr,
            i, REG_ARRAY_INDEX_NONE, MANGO_OPRI_DFLT_PRIf, pDot1p_pri)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        QOS_SEM_UNLOCK(unit);

        got = 1;
        break;
    }

    if(!got)
      return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_mango_qos_outer1pDfltPri_get */

/* Function Name:
 *      dal_mango_qos_outer1pDfltPri_set
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
dal_mango_qos_outer1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri)
{
    int32       ret;
    uint32      i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d", unit, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        QOS_SEM_LOCK(unit);
        /* get value from CHIP*/
        if ((ret = reg_array_field_write(unit, MANGO_RMK_PORT_CTRLr,
            i, REG_ARRAY_INDEX_NONE, MANGO_OPRI_DFLT_PRIf, &dot1p_pri)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        QOS_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
} /* end of dal_mango_qos_outer1pDfltPri_set */

/* Function Name:
 *      dal_mango_qos_dscpRemark_get
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
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_dscpRemark_get(uint32 unit, rtk_pri_t int_pri, uint32 *pDscp)
{
    int32   ret;
    uint32  reg_name;
    rtk_qos_dscpRmkSrc_t  type;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d", unit, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    if((ret = dal_mango_qos_dscpRemarkSrcSel_get(unit, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    switch(type)
    {
        case DSCP_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2DSCP_CTRLr;
            break;
        case DSCP_RMK_SRC_USER_PRI:
            reg_name = MANGO_RMK_IPRI2DSCP_CTRLr;
            break;
        case DSCP_RMK_SRC_OUTER_USER_PRI:
            reg_name = MANGO_RMK_OPRI2DSCP_CTRLr;
            break;
        case DSCP_RMK_SRC_DP_INT_PRI:
            reg_name = MANGO_RMK_DPINTPRI2DSCP_CTRLr;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_name, REG_ARRAY_INDEX_NONE, int_pri,
                    MANGO_DSCPf, pDscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDscp=%d", *pDscp);

    return RT_ERR_OK;
} /* end of dal_mango_qos_dscpRemark_get */

/* Function Name:
 *      dal_mango_qos_dscpRemark_set
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
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 * Note:
 *      DSCP remark functionality can map the internal priority to DSCP before a packet is going
 *      to be transmited.
 */
int32
dal_mango_qos_dscpRemark_set(uint32 unit, rtk_pri_t int_pri, uint32 dscp)
{
    int32   ret;
    uint32  reg_name;
    rtk_qos_dscpRmkSrc_t  type;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d, dscp=%d",
           unit, int_pri, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    if((ret = dal_mango_qos_dscpRemarkSrcSel_get(unit, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    switch(type)
    {
        case DSCP_RMK_SRC_INT_PRI:
            reg_name = MANGO_RMK_INTPRI2DSCP_CTRLr;
            break;
        case DSCP_RMK_SRC_USER_PRI:
            reg_name = MANGO_RMK_IPRI2DSCP_CTRLr;
            break;
        case DSCP_RMK_SRC_OUTER_USER_PRI:
            reg_name = MANGO_RMK_OPRI2DSCP_CTRLr;
            break;
        case DSCP_RMK_SRC_DP_INT_PRI:
            reg_name = MANGO_RMK_DPINTPRI2DSCP_CTRLr;
            break;
        default:
            return RT_ERR_FAILED;
    }

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg_name, REG_ARRAY_INDEX_NONE, int_pri,
                    MANGO_DSCPf, &dscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_dscpRemark_set */

/* Function Name:
 *      dal_mango_qos_dscp2DscpRemark_get
 * Description:
 *      Get DSCP to remarked DSCP mapping.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value
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
dal_mango_qos_dscp2DscpRemark_get(uint32 unit, uint32 dscp, uint32 *pDscp)
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
    if ((ret = reg_array_field_read(unit, MANGO_RMK_DSCP2DSCP_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    MANGO_DSCPf, pDscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDscp=%d", *pDscp);

    return RT_ERR_OK;
} /* end of dal_mango_qos_dscp2DscpRemark_get */

/* Function Name:
 *      dal_mango_qos_dscp2DscpRemark_set
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
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 * Note:
 *      None.
 */
int32
dal_mango_qos_dscp2DscpRemark_set(uint32 unit, uint32 dscp, uint32 rmkDscp)
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
    if ((ret = reg_array_field_write(unit, MANGO_RMK_DSCP2DSCP_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    MANGO_DSCPf, &rmkDscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_dscp2DscpRemark_set */

/* Function Name:
 *      dal_mango_qos_dscp2Dot1pRemark_get
 * Description:
 *      Get DSCP to remarked 802.1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value
 * Output:
 *      pDot1p_pri   - remarked 802.1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_dscp2Dot1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d", unit, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_RMK_DSCP2IPRI_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    MANGO_IPRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_mango_qos_dscp2DscpRemark_get */

/* Function Name:
 *      dal_mango_qos_dscp2Dot1pRemark_set
 * Description:
 *      Set DSCP to remarked 802.1p priority(3bits) mapping.
 * Input:
 *      unit      - unit id
 *      dscp      - DSCP value
 *      dot1p_pri - remarked 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      None.
 */
int32
dal_mango_qos_dscp2Dot1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d, dot1p_pri=%d",
           unit, dscp, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK(dot1p_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_RMK_DSCP2IPRI_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    MANGO_IPRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_dscp2Dot1pRemark_set */

/* Function Name:
 *      dal_mango_qos_dscp2Outer1pRemark_get
 * Description:
 *      Get DSCP to remarked outer dot1p priority(3bits) mapping.
 * Input:
 *      unit    - unit id
 *      dscp    - DSCP value
 * Output:
 *      pDot1p_pri   - remarked outer dot1p priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_dscp2Outer1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d", unit, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_RMK_DSCP2OPRI_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    MANGO_OPRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_mango_qos_dscp2Outer1pRemark_get */

/* Function Name:
 *      dal_mango_qos_dscp2Outer1pRemark_set
 * Description:
 *      Set DSCP to remarked outer dot1p priority(3bits) mapping.
 * Input:
 *      unit      - unit id
 *      dscp      - DSCP value
 *      dot1p_pri - remarked outer dot1p priority value (range from 0 ~ 7)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 * Note:
 *      None.
 */
int32
dal_mango_qos_dscp2Outer1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d, dot1p_pri=%d",
           unit, dscp, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK(dot1p_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_RMK_DSCP2OPRI_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    MANGO_OPRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_dscp2Outer1pRemark_set */

/* Function Name:
 *      dal_mango_qos_deiRemark_get
 * Description:
 *      Get the internal drop precedence to remarked DEI mapping.
 * Input:
 *      unit       - unit id
 *      int_pri    - internal drop precedence (range from 0 ~ 2)
 * Output:
 *      pDei       - remarked DEI value (range from 0 ~ 1)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DROP_PRECEDENCE  - Invalid drop precedence
 *      RT_ERR_NULL_POINTER         - NULL pointer
 * Note:
 *      None.
 */
int32
dal_mango_qos_deiRemark_get(uint32 unit, uint32 dp, uint32 *pDei)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dp=%d", unit, dp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDei), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(dp > HAL_DROP_PRECEDENCE_MAX(unit), RT_ERR_QOS_DROP_PRECEDENCE);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_RMK_DP2DEI_CTRLr, REG_ARRAY_INDEX_NONE, dp,
                    MANGO_DEIf, pDei)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDei=%d", *pDei);

    return RT_ERR_OK;
} /* end of dal_mango_qos_deiRemark_get */

/* Function Name:
 *      dal_mango_qos_deiRemark_set
 * Description:
 *      Set the internal drop precedence to remarked DEI mapping.
 * Input:
 *      unit      - unit id
 *      dp        - internal drop precedence (range from 0 ~ 2)
 *      dei       - remarked DEI value (range from 0 ~ 1)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_DEI_VALUE        - Invalid DEI value
 *      RT_ERR_QOS_DROP_PRECEDENCE  - Invalid drop precedence
 * Note:
 *      DEI remark functionality can map the internal drop precedence
 *      to DEI in outer tag or inner-tag before a packet is going to be transmited.
 */
int32
dal_mango_qos_deiRemark_set(uint32 unit, uint32 dp, uint32 dei)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dp=%d, dei=%d",
           unit, dp, dei);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dei > RTK_VALUE_OF_DEI_MAX, RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(dp > HAL_DROP_PRECEDENCE_MAX(unit), RT_ERR_QOS_DROP_PRECEDENCE);

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, MANGO_RMK_DP2DEI_CTRLr, REG_ARRAY_INDEX_NONE, dp,
                    MANGO_DEIf, &dei)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_qos_deiRemark_set */
#endif

