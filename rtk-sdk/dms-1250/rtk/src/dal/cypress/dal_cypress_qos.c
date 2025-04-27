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
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/cypress/dal_cypress_qos.h>
#include <dal/cypress/dal_cypress_port.h>
#include <dal/cypress/dal_cypress_flowctrl.h>
#include <dal/cypress/dal_cypress_common.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/qos.h>


/*
 * Symbol Definition
 */
#define CPU_QUEUE_NUM                       (8)

/*
 * Data Declaration
 */
static uint32               qos_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         qos_sem[RTK_MAX_NUM_OF_UNIT];

const static uint16 qos_scheduling_queueWeight[8][8] = {{0,0,0,0,0,0,0,0},
                                                        {0,1,0,0,0,0,0,0},
                                                        {0,1,2,0,0,0,0,0},
                                                        {0,1,2,3,0,0,0,0},
                                                        {0,1,2,3,4,0,0,0},
                                                        {0,1,2,3,4,5,0,0},
                                                        {0,1,2,3,4,5,6,0},
                                                        {0,1,2,3,4,5,6,7}};

const static rtk_qos_pri2queue_t qos_scheduling_priorityToQueue[8]= {{{0,0,0,0,0,0,0,0}},
                                                                     {{0,0,0,0,1,1,1,1}},
                                                                     {{0,0,0,0,1,1,2,2}},
                                                                     {{0,0,1,1,2,2,3,3}},
                                                                     {{0,0,1,1,2,3,4,4}},
                                                                     {{0,0,1,2,3,4,5,5}},
                                                                     {{0,0,1,2,3,4,5,6}},
                                                                     {{0,1,2,3,4,5,6,7}}};

const static uint16 schedWeight_fieldidx[] = {CYPRESS_SCHED_WEIGHT_Q0tf, CYPRESS_SCHED_WEIGHT_Q1tf,
                                              CYPRESS_SCHED_WEIGHT_Q2tf, CYPRESS_SCHED_WEIGHT_Q3tf,
                                              CYPRESS_SCHED_WEIGHT_Q4tf, CYPRESS_SCHED_WEIGHT_Q5tf,
                                              CYPRESS_SCHED_WEIGHT_Q6tf, CYPRESS_SCHED_WEIGHT_Q7tf};
/*
const static uint16 egrBandwidth_queue_share_fieldidx[] = {CYPRESS_SCHED_BORW_TKN_Q0tf, CYPRESS_SCHED_BORW_TKN_Q1tf,
                                              CYPRESS_SCHED_BORW_TKN_Q2tf, CYPRESS_SCHED_BORW_TKN_Q3tf,
                                              CYPRESS_SCHED_BORW_TKN_Q4tf, CYPRESS_SCHED_BORW_TKN_Q5tf,
                                              CYPRESS_SCHED_BORW_TKN_Q6tf, CYPRESS_SCHED_BORW_TKN_Q7tf};
*/

/*
 * Macro Definition
 */
/* rate semaphore handling */
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
static int32 _dal_cypress_qos_init_config(uint32 unit, uint32 queueNum);
static int32 _dal_cypress_qos_outputQueueNumber_set(uint32 unit, rtk_port_t port, uint32 qNum);
static int32 _dal_cypress_qos_outputQueueNumber_get(uint32 unit, rtk_port_t port, uint32 *pQNum);
static int32 _dal_cypress_qos_cpuQueueNum_set(uint32 unit, uint32 queue_num);

/* Function Name:
 *      dal_cypress_qosMapper_init
 * Description:
 *      Hook qos module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook qos module before calling any qos APIs.
 */
int32
dal_cypress_qosMapper_init(dal_mapper_t *pMapper)
{
    pMapper->qos_init = dal_cypress_qos_init;
    pMapper->qos_priSelGroup_get = dal_cypress_qos_priSelGroup_get;
    pMapper->qos_priSelGroup_set = dal_cypress_qos_priSelGroup_set;
    pMapper->qos_portPriSelGroup_get = dal_cypress_qos_portPriSelGroup_get;
    pMapper->qos_portPriSelGroup_set = dal_cypress_qos_portPriSelGroup_set;
    pMapper->qos_dpSrcSel_get = dal_cypress_qos_dpSrcSel_get;
    pMapper->qos_dpSrcSel_set = dal_cypress_qos_dpSrcSel_set;
    pMapper->qos_portDEISrcSel_get = dal_cypress_qos_portDEISrcSel_get;
    pMapper->qos_portDEISrcSel_set = dal_cypress_qos_portDEISrcSel_set;
    pMapper->qos_deiDpRemap_get = dal_cypress_qos_deiDpRemap_get;
    pMapper->qos_deiDpRemap_set = dal_cypress_qos_deiDpRemap_set;
    pMapper->qos_dpRemap_get = dal_cypress_qos_dpRemap_get;
    pMapper->qos_dpRemap_set = dal_cypress_qos_dpRemap_set;
    pMapper->qos_priRemap_get = dal_cypress_qos_priRemap_get;
    pMapper->qos_priRemap_set = dal_cypress_qos_priRemap_set;
    pMapper->qos_queueNum_get = dal_cypress_qos_queueNum_get;
    pMapper->qos_queueNum_set = dal_cypress_qos_queueNum_set;
    pMapper->qos_priMap_get = dal_cypress_qos_priMap_get;
    pMapper->qos_priMap_set = dal_cypress_qos_priMap_set;
    pMapper->qos_port1pRemarkEnable_get = dal_cypress_qos_port1pRemarkEnable_get;
    pMapper->qos_port1pRemarkEnable_set = dal_cypress_qos_port1pRemarkEnable_set;
    pMapper->qos_1pRemarking_get = dal_cypress_qos_1pRemarking_get;
    pMapper->qos_1pRemarking_set = dal_cypress_qos_1pRemarking_set;
    pMapper->qos_portOut1pRemarkEnable_get = dal_cypress_qos_portOut1pRemarkEnable_get;
    pMapper->qos_portOut1pRemarkEnable_set = dal_cypress_qos_portOut1pRemarkEnable_set;
    pMapper->qos_portDscpRemarkEnable_get = dal_cypress_qos_portDscpRemarkEnable_get;
    pMapper->qos_portDscpRemarkEnable_set = dal_cypress_qos_portDscpRemarkEnable_set;
    pMapper->qos_dscpRemarking_get = dal_cypress_qos_dscpRemarking_get;
    pMapper->qos_dscpRemarking_set = dal_cypress_qos_dscpRemarking_set;
    pMapper->qos_portDeiRemarkTagSel_get = dal_cypress_qos_portDeiRemarkTagSel_get;
    pMapper->qos_portDeiRemarkTagSel_set = dal_cypress_qos_portDeiRemarkTagSel_set;
    pMapper->qos_portDeiRemarkEnable_get = dal_cypress_qos_portDeiRemarkEnable_get;
    pMapper->qos_portDeiRemarkEnable_set = dal_cypress_qos_portDeiRemarkEnable_set;
    pMapper->qos_deiRemark_get = dal_cypress_qos_deiRemark_get;
    pMapper->qos_deiRemark_set = dal_cypress_qos_deiRemark_set;
    pMapper->qos_deiRemarking_get = dal_cypress_qos_deiRemarking_get;
    pMapper->qos_deiRemarking_set = dal_cypress_qos_deiRemarking_set;
    pMapper->qos_1pDfltPri_get = dal_cypress_qos_1pDfltPri_get;
    pMapper->qos_1pDfltPri_set = dal_cypress_qos_1pDfltPri_set;
    pMapper->qos_1pRemark_get = dal_cypress_qos_1pRemark_get;
    pMapper->qos_1pRemark_set = dal_cypress_qos_1pRemark_set;
    pMapper->qos_1pRemarkSrcSel_get = dal_cypress_qos_1pRemarkSrcSel_get;
    pMapper->qos_1pRemarkSrcSel_set = dal_cypress_qos_1pRemarkSrcSel_set;
    pMapper->qos_outer1pRemark_get = dal_cypress_qos_outer1pRemark_get;
    pMapper->qos_outer1pRemark_set = dal_cypress_qos_outer1pRemark_set;
    pMapper->qos_outer1pRemarking_get = dal_cypress_qos_outer1pRemarking_get;
    pMapper->qos_outer1pRemarking_set = dal_cypress_qos_outer1pRemarking_set;
    pMapper->qos_outer1pRemarkSrcSel_get = dal_cypress_qos_outer1pRemarkSrcSel_get;
    pMapper->qos_outer1pRemarkSrcSel_set = dal_cypress_qos_outer1pRemarkSrcSel_set;
    pMapper->qos_outer1pDfltPriCfgSrcSel_get = dal_cypress_qos_outer1pDfltPriCfgSrcSel_get;
    pMapper->qos_outer1pDfltPriCfgSrcSel_set = dal_cypress_qos_outer1pDfltPriCfgSrcSel_set;
    pMapper->qos_portOuter1pDfltPriSrcSel_get = dal_cypress_qos_portOuter1pDfltPriSrcSel_get;
    pMapper->qos_portOuter1pDfltPriSrcSel_set = dal_cypress_qos_portOuter1pDfltPriSrcSel_set;
    pMapper->qos_dscpRemark_get = dal_cypress_qos_dscpRemark_get;
    pMapper->qos_dscpRemark_set = dal_cypress_qos_dscpRemark_set;
    pMapper->qos_dscp2Dot1pRemark_get = dal_cypress_qos_dscp2Dot1pRemark_get;
    pMapper->qos_dscp2Dot1pRemark_set = dal_cypress_qos_dscp2Dot1pRemark_set;
    pMapper->qos_dscp2Outer1pRemark_get = dal_cypress_qos_dscp2Outer1pRemark_get;
    pMapper->qos_dscp2Outer1pRemark_set = dal_cypress_qos_dscp2Outer1pRemark_set;
    pMapper->qos_dscp2DscpRemark_get = dal_cypress_qos_dscp2DscpRemark_get;
    pMapper->qos_dscp2DscpRemark_set = dal_cypress_qos_dscp2DscpRemark_set;
    pMapper->qos_dscpRemarkSrcSel_get = dal_cypress_qos_dscpRemarkSrcSel_get;
    pMapper->qos_dscpRemarkSrcSel_set = dal_cypress_qos_dscpRemarkSrcSel_set;
    pMapper->qos_schedulingAlgorithm_get = dal_cypress_qos_schedulingAlgorithm_get;
    pMapper->qos_schedulingAlgorithm_set = dal_cypress_qos_schedulingAlgorithm_set;
    pMapper->qos_schedulingQueue_get = dal_cypress_qos_schedulingQueue_get;
    pMapper->qos_schedulingQueue_set = dal_cypress_qos_schedulingQueue_set;
    pMapper->qos_congAvoidAlgo_get = dal_cypress_qos_congAvoidAlgo_get;
    pMapper->qos_congAvoidAlgo_set = dal_cypress_qos_congAvoidAlgo_set;
    pMapper->qos_congAvoidSysThresh_get = dal_cypress_qos_congAvoidSysThresh_get;
    pMapper->qos_congAvoidSysThresh_set = dal_cypress_qos_congAvoidSysThresh_set;
    pMapper->qos_congAvoidSysDropProbability_get = dal_cypress_qos_congAvoidSysDropProbability_get;
    pMapper->qos_congAvoidSysDropProbability_set = dal_cypress_qos_congAvoidSysDropProbability_set;
    pMapper->qos_congAvoidGlobalQueueThresh_get = dal_cypress_qos_congAvoidGlobalQueueThresh_get;
    pMapper->qos_congAvoidGlobalQueueThresh_set = dal_cypress_qos_congAvoidGlobalQueueThresh_set;
    pMapper->qos_congAvoidGlobalQueueDropProbability_get = dal_cypress_qos_congAvoidGlobalQueueDropProbability_get;
    pMapper->qos_congAvoidGlobalQueueDropProbability_set = dal_cypress_qos_congAvoidGlobalQueueDropProbability_set;
    pMapper->qos_congAvoidGlobalQueueConfig_get = dal_cypress_qos_congAvoidGlobalQueueConfig_get;
    pMapper->qos_congAvoidGlobalQueueConfig_set = dal_cypress_qos_congAvoidGlobalQueueConfig_set;
    pMapper->qos_portAvbStreamReservationClassEnable_get = dal_cypress_qos_portAvbStreamReservationClassEnable_get;
    pMapper->qos_portAvbStreamReservationClassEnable_set = dal_cypress_qos_portAvbStreamReservationClassEnable_set;
    pMapper->qos_avbStreamReservationConfig_get = dal_cypress_qos_avbStreamReservationConfig_get;
    pMapper->qos_avbStreamReservationConfig_set = dal_cypress_qos_avbStreamReservationConfig_set;
    pMapper->qos_pkt2CpuPriRemap_get = dal_cypress_qos_pkt2CpuPriRemap_get;
    pMapper->qos_pkt2CpuPriRemap_set = dal_cypress_qos_pkt2CpuPriRemap_set;
    pMapper->qos_invldDscpVal_get = dal_cypress_qos_invldDscpVal_get;
    pMapper->qos_invldDscpVal_set = dal_cypress_qos_invldDscpVal_set;
    pMapper->qos_invldDscpEnable_get = dal_cypress_qos_invldDscpEnable_get;
    pMapper->qos_invldDscpEnable_set = dal_cypress_qos_invldDscpEnable_set;
    pMapper->qos_priRemapEnable_get = dal_cypress_qos_priRemapEnable_get;
    pMapper->qos_priRemapEnable_set = dal_cypress_qos_priRemapEnable_set;
    pMapper->qos_1pDfltPriSrcSel_get = dal_cypress_qos_1pDfltPriSrcSel_get;
    pMapper->qos_1pDfltPriSrcSel_set = dal_cypress_qos_1pDfltPriSrcSel_set;
    pMapper->qos_outer1pDfltPri_get = dal_cypress_qos_outer1pDfltPri_get;
    pMapper->qos_outer1pDfltPri_set = dal_cypress_qos_outer1pDfltPri_set;
    pMapper->qos_sysPortPriRemapSel_get = dal_cypress_qos_sysPortPriRemapSel_get;
    pMapper->qos_sysPortPriRemapSel_set = dal_cypress_qos_sysPortPriRemapSel_set;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_cypress_qos_init
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
dal_cypress_qos_init(uint32 unit, uint32 queueNum)
{
    int32       ret;

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

    if ((ret = _dal_cypress_qos_init_config(unit, queueNum)) != RT_ERR_OK)
    {
        qos_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_cypress_qos_init */

/* Function Name:
 *      dal_cypress_qos_priSelGroup_get
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
dal_cypress_qos_priSelGroup_get(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, grp_idx=%u", unit, grp_idx);

    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((grp_idx > HAL_PRI_SEL_GROUP_INDEX_MAX(unit)), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_PORT_WTf, &(pWeightOfPriSel->weight_of_portBased))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_DSCP_WTf, &(pWeightOfPriSel->weight_of_dscp))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_ING_ACL_WTf, &(pWeightOfPriSel->weight_of_inAcl))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_ITAG_WTf, &(pWeightOfPriSel->weight_of_innerTag))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_OTAG_WTf, &(pWeightOfPriSel->weight_of_outerTag))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_MAC_VLAN_WTf, &(pWeightOfPriSel->weight_of_macVlan))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_PROTO_VLAN_WTf, &(pWeightOfPriSel->weight_of_protoVlan))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_priSelGroup_get */

/* Function Name:
 *      dal_cypress_qos_priSelGroup_set
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
dal_cypress_qos_priSelGroup_set(uint32 unit, uint32 grp_idx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    uint32  val;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, grp_idx=%u, weight_of_portBased=%d, \
           weight_of_dscp=%d, weight_of_inAcl=%d, weight_of_innerTag=%d, \
           weight_of_outerTag=%d, weight_of_macVlan=%d, weight_of_protoVlan=%d"
           , unit, grp_idx, pWeightOfPriSel->weight_of_portBased
           , pWeightOfPriSel->weight_of_dscp, pWeightOfPriSel->weight_of_inAcl
           , pWeightOfPriSel->weight_of_innerTag , pWeightOfPriSel->weight_of_outerTag
           , pWeightOfPriSel->weight_of_macVlan , pWeightOfPriSel->weight_of_protoVlan);

    RT_PARAM_CHK((grp_idx > HAL_PRI_SEL_GROUP_INDEX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_portBased < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_portBased > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_dscp < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_dscp > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_inAcl < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_inAcl > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_innerTag < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_innerTag > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_outerTag < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_outerTag > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_macVlan < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_macVlan > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pWeightOfPriSel->weight_of_protoVlan < HAL_PRI_OF_SELECTION_MIN(unit))
               ||(pWeightOfPriSel->weight_of_protoVlan > HAL_PRI_OF_SELECTION_MAX(unit)), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    val = pWeightOfPriSel->weight_of_portBased;
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_PORT_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_dscp;
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_DSCP_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_inAcl;
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_ING_ACL_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_innerTag;
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_ITAG_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_outerTag;
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_OTAG_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_macVlan;
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_MAC_VLAN_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    val = pWeightOfPriSel->weight_of_protoVlan;
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_TBL_CTRLr, REG_ARRAY_INDEX_NONE, grp_idx,
                CYPRESS_PROTO_VLAN_WTf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_priSelGroup_set */

/* Function Name:
 *      dal_cypress_qos_portPriSelGroup_get
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
dal_cypress_qos_portPriSelGroup_get(uint32 unit, rtk_port_t port, uint32 *pPriSelGrp_idx)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPriSelGrp_idx), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_PORT_TBL_IDX_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_TBL_IDXf, pPriSelGrp_idx)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portPriSelGroup_get */

/* Function Name:
 *      dal_cypress_qos_portPriSelGroup_set
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
dal_cypress_qos_portPriSelGroup_set(uint32 unit, rtk_port_t port, uint32 priSelGrp_idx)
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
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_PORT_TBL_IDX_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_TBL_IDXf, &priSelGrp_idx)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portPriSelGroup_set */

/* Function Name:
 *      dal_cypress_qos_portPri_get
 * Description:
 *      Get internal priority of one port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pInt_pri - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    None.
 */
int32
dal_cypress_qos_portPri_get(uint32 unit, rtk_port_t port, rtk_pri_t *pInt_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInt_pri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_PORT_PRIr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_INTPRI_PORTf, pInt_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pInt_pri=%d", *pInt_pri);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portPri_get */

/* Function Name:
 *      dal_cypress_qos_portPri_set
 * Description:
 *      Set internal priority of one port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      int_pri - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                the highest prioirty)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - Invalid port id
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *    This API can set port to 3 bits internal priority mapping.
 *    When a packet is received from a port, a port based priority will be assigned
 *    by the mapping setting.
 *    By default, the mapping priorities for all ports are 0.
 */
int32
dal_cypress_qos_portPri_set(uint32 unit, rtk_port_t port, rtk_pri_t int_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, int_pri=%d",
           unit, port, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((int_pri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_PORT_PRIr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_INTPRI_PORTf, &int_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_PORT_PRI_COPYr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_INTPRI_PORTf, &int_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portPri_set */

/* Function Name:
 *      dal_cypress_qos_portPriRemapEnable_get
 * Description:
 *      Get status of port-based priority remapping.
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable   - status of port-based priority remapping
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    None.
 */
int32
dal_cypress_qos_portPriRemapEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_PRI_SEL_CTRLr, CYPRESS_INTPRI_PORT_REMAP_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portPriRemapEnable_get */

/* Function Name:
 *      dal_cypress_qos_portPriRemapEnable_set
 * Description:
 *      Set status of port-based priority remapping.
 * Input:
 *      unit      - unit id
 *      pEnable   - status of port-based priority remapping
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
dal_cypress_qos_portPriRemapEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_PRI_SEL_CTRLr, CYPRESS_INTPRI_PORT_REMAP_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portPriRemapEnable_set */

/* Function Name:
 *      dal_cypress_qos_sysPortPriRemapSel_get
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
dal_cypress_qos_sysPortPriRemapSel_get(uint32 unit, rtk_qos_portPriRemapSel_t *pType)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_PRI_SEL_CTRLr, CYPRESS_INTPRI_PORT_REMAP_SELf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = P_PRI_REMAP_INNER_PRI_TBL;
    else
        *pType = P_PRI_REMAP_OUTER_PRI_DEI0_TBL;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_sysPortPriRemapSel_get */

/* Function Name:
 *      dal_cypress_qos_sysPortPriRemapSel_set
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
dal_cypress_qos_sysPortPriRemapSel_set(uint32 unit, rtk_qos_portPriRemapSel_t type)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%u", unit, type);

    RT_PARAM_CHK(type >= P_PRI_REMAP_END, RT_ERR_INPUT);

    if (P_PRI_REMAP_INNER_PRI_TBL == type)
        value = 0;
    else if (P_PRI_REMAP_OUTER_PRI_DEI0_TBL == type)
        value = 1;
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_PRI_SEL_CTRLr, CYPRESS_INTPRI_PORT_REMAP_SELf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_sysPortPriRemapSel_set */

/* Function Name:
 *      dal_cypress_qos_dpSrcSel_get
 * Description:
 *      Get drop precedence source.
 * Input:
 *      unit - unit id
 * Output:
 *      pType  - DP mapping source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_qos_dpSrcSel_get(uint32 unit, rtk_qos_dpSrc_t *pType)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, pType=%x"
            , unit, pType);

    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_PRI_SEL_CTRLr, CYPRESS_DP_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = DP_SRC_DEI_BASED;
    else if (1 == value)
        *pType = DP_SRC_DSCP_BASED;
    else
        return RT_ERR_FAILED;

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dpSrcSel_get */

/* Function Name:
 *      dal_cypress_qos_dpSrcSel_set
 * Description:
 *      Set drop precedence source.
 * Input:
 *      unit - unit id
 *      port - port id
 *      pType   - DP mapping source
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
dal_cypress_qos_dpSrcSel_set(uint32 unit, rtk_qos_dpSrc_t type)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%u"
            , unit, type);

    RT_PARAM_CHK(type >= DP_SRC_END, RT_ERR_INPUT);

    if (DP_SRC_DEI_BASED == type)
        value = 0;
    else if (DP_SRC_DSCP_BASED == type)
        value = 1;
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_PRI_SEL_CTRLr, CYPRESS_DP_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dpSrcSel_set */

/* Function Name:
 *      dal_cypress_qos_portDEISrcSel_get
 * Description:
 *      Get DEI source of specified port.
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
dal_cypress_qos_portDEISrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t *pType)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, pType=%x"
            , unit, port, pType);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_PORT_DEI_TAG_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_PORT_DEI_TAG_SRCf, &value)) != RT_ERR_OK)
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
} /* end of dal_cypress_qos_portDEISrcSel_get */

/* Function Name:
 *      dal_cypress_qos_portDEISrcSel_set
 * Description:
 *      Set DEI source of specified port.
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
dal_cypress_qos_portDEISrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type)
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
    else
        value = 1;

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_PORT_DEI_TAG_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_PORT_DEI_TAG_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portDEISrcSel_set */

/* Function Name:
 *      dal_cypress_qos_deiDpRemap_get
 * Description:
 *      Get drop precedence of specified DEI.
 * Input:
 *      unit - unit id
 *      dei  - DEI
 * Output:
 *      pDp  - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_qos_deiDpRemap_get(uint32 unit, uint32 dei, uint32 *pDp)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dei=%d, pDp=%x"
            , unit, dei, pDp);

    RT_PARAM_CHK((dei > RTK_DOT1P_DEI_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pDp), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_DEI2DP_REMAPr, REG_ARRAY_INDEX_NONE,
                    dei, CYPRESS_DEI2DP_VALf, pDp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_deiDpRemap_get */

/* Function Name:
 *      dal_cypress_qos_deiDpRemap_set
 * Description:
 *      Set drop precedence to specified DEI.
 * Input:
 *      unit - unit id
 *      dei  - DEI
 *      dp   - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 * Note:
 *      None
 */
int32
dal_cypress_qos_deiDpRemap_set(uint32 unit, uint32 dei, uint32 dp)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dei=%d, dp=%u"
            , unit, dei, dp);

    RT_PARAM_CHK((dei > RTK_DOT1P_DEI_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_DEI2DP_REMAPr, REG_ARRAY_INDEX_NONE,
                    dei, CYPRESS_DEI2DP_VALf, &dp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_deiDpRemap_set */

/* Function Name:
 *      dal_cypress_qos_dscpDpRemap_get
 * Description:
 *      Get drop precedence of specified DSCP.
 * Input:
 *      unit - unit id
 *      dscp - DSCP value of receiving frame (0~63)
 * Output:
 *      pDp  - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value
 * Note:
 *      None
 */
int32
dal_cypress_qos_dscpDpRemap_get(uint32 unit, uint32 dscp, uint32 *pDp)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d, pDp=%x"
            , unit, dscp, pDp);

    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK((NULL == pDp), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_DSCP2DP_REMAPr, REG_ARRAY_INDEX_NONE,
                    dscp, CYPRESS_DSCP2DP_VALf, pDp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscpDpRemap_get */

/* Function Name:
 *      dal_cypress_qos_dscpDpRemap_set
 * Description:
 *      Set drop precedence to specified DSCP.
 * Input:
 *      unit - unit id
 *      dscp - DSCP value of receiving frame (0~63)
 *      pDp  - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 * Note:
 *      None
 */
int32
dal_cypress_qos_dscpDpRemap_set(uint32 unit, uint32 dscp, uint32 dp)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d, dp=%u"
            , unit, dscp, dp);

    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_DSCP2DP_REMAPr, REG_ARRAY_INDEX_NONE,
                    dscp, CYPRESS_DSCP2DP_VALf, &dp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscpDpRemap_set */

/* Function Name:
 *      dal_cypress_qos_dpRemap_get
 * Description:
 *      Get DEI/DSCP mapping to drop precedence.
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
 *      The API can get configuration of DEI/DSCP to DP remapping table.
 */
int32
dal_cypress_qos_dpRemap_get(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 *pDp)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch (src)
    {
        case DP_SRC_DEI_BASED:
            return dal_cypress_qos_deiDpRemap_get(unit, srcVal.src.tag.dei, pDp);
        case DP_SRC_DSCP_BASED:
            return dal_cypress_qos_dscpDpRemap_get(unit, srcVal.src.dscp, pDp);
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_QOS), "src %d is invalid.", src);
            return RT_ERR_INPUT;
    }
}

/* Function Name:
 *      dal_cypress_qos_dpRemap_set
 * Description:
 *      Set DEI/DSCP mapping to drop precedence.
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
 *      RT_ERR_DROP_PRECEDENCE - invalid drop precedence
 * Note:
 *      The API can configure DEI/DSCP to DP remapping table.
 */
int32
dal_cypress_qos_dpRemap_set(uint32 unit, rtk_qos_dpSrc_t src, rtk_qos_dpSrcRemap_t srcVal, uint32 dp)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* set value */
    switch (src)
    {
        case DP_SRC_DEI_BASED:
            return dal_cypress_qos_deiDpRemap_set(unit, srcVal.src.tag.dei, dp);
        case DP_SRC_DSCP_BASED:
            return dal_cypress_qos_dscpDpRemap_set(unit, srcVal.src.dscp, dp);
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_QOS), "src %d is invalid.", src);
            return RT_ERR_INPUT;
    }

/*    return RT_ERR_OK; */
}

/* Function Name:
 *      dal_cypress_qos_dscpPriRemap_get
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
 *      RT_ERR_QOS_DSCP_VALUE - Invalid DSCP value
 *      RT_ERR_NULL_POINTER   - NULL pointer
 * Note:
 *      None.
 */
int32
dal_cypress_qos_dscpPriRemap_get(uint32 unit, uint32 dscp, rtk_pri_t *pInt_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d", unit, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInt_pri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_DSCP_REMAPr, REG_ARRAY_INDEX_NONE,
                    dscp, CYPRESS_INTPRI_DSCPf, pInt_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pInt_pri=%d", *pInt_pri);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscpPriRemap_get */

/* Function Name:
 *      dal_cypress_qos_dscpPriRemap_set
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
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid DSCP value
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *      The Differentiated Service Code Point is a selector for router's per-hop behaviours.
 *      As a selector, there is no implication that a numerically greater DSCP implies a better
 *      network service. As can be seen, the DSCP totally overlaps the old precedence field of TOS.
 *      So if values of DSCP are carefully chosen then backward compatibility can be achieved.
 */
int32
dal_cypress_qos_dscpPriRemap_set(uint32 unit, uint32 dscp, rtk_pri_t int_pri)
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

    /* program value to chip CHIP*/
    val = int_pri;
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_DSCP_REMAPr, REG_ARRAY_INDEX_NONE,
                    dscp, CYPRESS_INTPRI_DSCPf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscpPriRemap_set */

/* Function Name:
 *      dal_cypress_qos_1pPriRemap_get
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
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_NULL_POINTER    - NULL pointer
 * Note:
 *      None.
 */
int32
dal_cypress_qos_1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t *pInt_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d",
           unit, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInt_pri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);


    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_IPRI_REMAPr, REG_ARRAY_INDEX_NONE,
                    dot1p_pri, CYPRESS_INTPRI_IPRIf, pInt_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pInt_pri=%d", *pInt_pri);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_1pPriRemap_get */

/* Function Name:
 *      dal_cypress_qos_1pPriRemap_set
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
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid 802.1p priority
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *      None.
 */
int32
dal_cypress_qos_1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, rtk_pri_t int_pri)
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
    if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_IPRI_REMAPr, REG_ARRAY_INDEX_NONE,
                    dot1p_pri, CYPRESS_INTPRI_IPRIf, &val)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_1pPriRemap_set */

/* Function Name:
 *      dal_cypress_qos_outer1pPriRemap_get
 * Description:
 *      Get the internal priority that outer 802.1p priority remap.
 * Input:
 *      unit      - unit id
 *      dei       - DEI
 *      dot1p_pri - 802.1p priority value (range from 0 ~ 7)
 * Output:
 *      pInt_pri  - internal priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_NULL_POINTER    - NULL pointer
 *      RT_ERR_QOS_DEI_VALUE   - invalid dei
 * Note:
 *      None.
 */
int32
dal_cypress_qos_outer1pPriRemap_get(uint32 unit, rtk_pri_t dot1p_pri, uint32 dei, rtk_pri_t *pInt_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d, dei=%d",
           unit, dot1p_pri, dei);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInt_pri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((dei > RTK_DOT1P_DEI_MAX), RT_ERR_QOS_DEI_VALUE);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if (0 == dei)
    {
        if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_OPRI_DEI0_REMAPr, REG_ARRAY_INDEX_NONE,
                        dot1p_pri, CYPRESS_INTPRI_OPRIf, pInt_pri)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }
    else if (1 == dei)
    {
        if ((ret = reg_array_field_read(unit, CYPRESS_PRI_SEL_OPRI_DEI1_REMAPr, REG_ARRAY_INDEX_NONE,
                        dot1p_pri, CYPRESS_INTPRI_OPRIf, pInt_pri)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pInt_pri=%d", *pInt_pri);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_outer1pPriRemap_get */

/* Function Name:
 *      dal_cypress_qos_outer1pPriRemap_set
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
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid 802.1p priority
 *      RT_ERR_QOS_DEI_VALUE    - invalid dei
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority value
 * Note:
 *      None.
 */
int32
dal_cypress_qos_outer1pPriRemap_set(uint32 unit, rtk_pri_t dot1p_pri, uint32 dei, rtk_pri_t int_pri)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d, dei=%d, int_pri=%d",
           unit, dot1p_pri, dei, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((dei > RTK_DOT1P_DEI_MAX), RT_ERR_QOS_DEI_VALUE);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);


    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if (0 == dei)
    {
        val = int_pri;
        if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_OPRI_DEI0_REMAPr, REG_ARRAY_INDEX_NONE,
                        dot1p_pri, CYPRESS_INTPRI_OPRIf, &val)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }
    else if (1 == dei)
    {
        val = int_pri;
        if ((ret = reg_array_field_write(unit, CYPRESS_PRI_SEL_OPRI_DEI1_REMAPr, REG_ARRAY_INDEX_NONE,
                        dot1p_pri, CYPRESS_INTPRI_OPRIf, &val)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_outer1pPriRemap_set */

/* Function Name:
 *      dal_cypress_qos_priRemap_get
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
 */
int32
dal_cypress_qos_priRemap_get(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 *pPri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch (src)
    {
        case PRI_SRC_INNER_USER_PRI:
            return dal_cypress_qos_1pPriRemap_get(unit, srcVal.src.tag.pri, pPri);
        case PRI_SRC_OUTER_USER_PRI:
            return dal_cypress_qos_outer1pPriRemap_get(unit, srcVal.src.tag.pri, srcVal.src.tag.dei, pPri);
        case PRI_SRC_DSCP:
            return dal_cypress_qos_dscpPriRemap_get(unit, srcVal.src.dscp, pPri);
        case PRI_SRC_PB_PRI:
            return dal_cypress_qos_portPri_get(unit, srcVal.src.port, pPri);
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_QOS), "src %d is invalid.", src);
        return RT_ERR_INPUT;
    }
}

/* Function Name:
 *      dal_cypress_qos_priRemap_set
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
 */
int32
dal_cypress_qos_priRemap_set(uint32 unit, rtk_qos_priSrc_t src, rtk_qos_priSrcRemap_t srcVal, uint32 pri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch (src)
    {
        case PRI_SRC_INNER_USER_PRI:
            return dal_cypress_qos_1pPriRemap_set(unit, srcVal.src.tag.pri, pri);
        case PRI_SRC_OUTER_USER_PRI:
            return dal_cypress_qos_outer1pPriRemap_set(unit, srcVal.src.tag.pri, srcVal.src.tag.dei, pri);
        case PRI_SRC_DSCP:
            return dal_cypress_qos_dscpPriRemap_set(unit, srcVal.src.dscp, pri);
        case PRI_SRC_PB_PRI:
            return dal_cypress_qos_portPri_set(unit, srcVal.src.port, pri);
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_QOS), "src %d is invalid.", src);
        return RT_ERR_INPUT;
    }
}

/* Function Name:
 *      dal_cypress_qos_priRemapEnable_get
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Supported priority remap type rtk_qos_priSrc_t is as following:
 *      - PRI_SRC_PB_PRI
 */
int32
dal_cypress_qos_priRemapEnable_get(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t *pEnable)
{

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d", unit, src);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (src)
    {
        case PRI_SRC_PB_PRI:
            return dal_cypress_qos_portPriRemapEnable_get(unit, pEnable);
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_QOS), "src %d is invalid.", src);
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
} /* end of dal_cypress_qos_priRemapEnable_get */

/* Function Name:
 *      dal_cypress_qos_priRemapEnable_set
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
 */
int32
dal_cypress_qos_priRemapEnable_set(uint32 unit, rtk_qos_priSrc_t src, rtk_enable_t enable)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, src=%d, enable=%d", unit, src, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    /* set value */
    switch (src)
    {
        case PRI_SRC_PB_PRI:
            return dal_cypress_qos_portPriRemapEnable_set(unit, enable);
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_TRAP), "src %d is invalid.", src);
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
} /* end of dal_cypress_qos_priRemapEnable_set */

/* Function Name:
 *      dal_cypress_qos_queueNum_get
 * Description:
 *      Get the number of queue for the system.
 * Input:
 *      unit       - unit id
 * Output:
 *      pQueue_num - the number of queue (1~8).
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
dal_cypress_qos_queueNum_get(uint32 unit, uint32 *pQueue_num)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue_num), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    if ((ret = _dal_cypress_qos_outputQueueNumber_get(unit, 0, pQueue_num)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        return ret;
    }
    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pQueue_num=%d", *pQueue_num);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_queueNum_get */

/* Function Name:
 *      dal_cypress_qos_queueNum_set
 * Description:
 *      Set the number of queue for the system.
 * Input:
 *      unit      - unit id
 *      queue_num - the number of queue (1~8).
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Note:
 *      None.
 */
int32
dal_cypress_qos_queueNum_set(uint32 unit, uint32 queue_num)
{
    int32       ret;
    rtk_port_t  port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue_num=%d", unit, queue_num);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((queue_num > HAL_MAX_NUM_OF_QUEUE(unit)) || (queue_num < HAL_MIN_NUM_OF_QUEUE(unit))), RT_ERR_QUEUE_NUM);


    QOS_SEM_LOCK(unit);

    HWP_PORT_TRAVS(unit, port)
    {
        if (!HWP_ETHER_PORT(unit, port))
        {
            continue;
        }

        if ((ret = _dal_cypress_qos_outputQueueNumber_set(unit, port, queue_num)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_queueNum_set */

/* Function Name:
 *      dal_cypress_qos_priMap_get
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
 *      RT_ERR_QUEUE_NUM    - Invalid queue number
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
dal_cypress_qos_priMap_get(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid)
{
    int32       ret;
    rtk_pri_t   pri;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue_num=%d", unit, queue_num);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((queue_num > HAL_MAX_NUM_OF_QUEUE(unit)) || (queue_num < HAL_MIN_NUM_OF_QUEUE(unit))), RT_ERR_QUEUE_NUM);
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    for (pri = 0; pri <= HAL_INTERNAL_PRIORITY_MAX(unit); pri++)
    {
        if ((ret = reg_array_field_read(unit, CYPRESS_QM_INTPRI2QID_CTRLr
                            , queue_num - 1, pri, CYPRESS_INTPRI_QIDf, &pPri2qid->pri2queue[pri])) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            return ret;
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
} /* end of dal_cypress_qos_priMap_get */

/* Function Name:
 *      dal_cypress_qos_priMap_set
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
 *      RT_ERR_QUEUE_NUM - Invalid queue number
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
dal_cypress_qos_priMap_set(uint32 unit, uint32 queue_num, rtk_qos_pri2queue_t *pPri2qid)
{
    int32       ret;
    rtk_pri_t   pri;

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

    /* set value to CHIP*/
    for (pri = 0; pri <= HAL_INTERNAL_PRIORITY_MAX(unit); pri++)
    {
        if ((ret = reg_array_field_write(unit, CYPRESS_QM_INTPRI2QID_CTRLr
                            , queue_num - 1, pri, CYPRESS_INTPRI_QIDf, &pPri2qid->pri2queue[pri])) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            return ret;
        }
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_priMap_set */

/* Function Name:
 *      dal_cypress_qos_pri2QidMap_get
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
dal_cypress_qos_pri2QidMap_get(uint32 unit, rtk_pri_t pri, rtk_qid_t *pQid)
{
    int32 ret;

    rtk_qos_pri2queue_t pri2qid;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);
    RT_PARAM_CHK(pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    osal_memset(&pri2qid, 0, sizeof(rtk_qos_pri2queue_t));

    /* get value to CHIP*/
    if ((ret = dal_cypress_qos_priMap_get(unit, 0, &pri2qid)) != RT_ERR_OK)
    {
        return ret;
    }

    *pQid = pri2qid.pri2queue[pri];

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_pri2QidMap_set
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
dal_cypress_qos_pri2QidMap_set(uint32 unit, rtk_pri_t pri, rtk_qid_t qid)
{
    int32 ret;
    uint32 queue_num;
    rtk_qos_pri2queue_t pri2qid;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);
    RT_PARAM_CHK(pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    osal_memset(&pri2qid, 0, sizeof(rtk_qos_pri2queue_t));

    /* set value to CHIP*/
    for (queue_num = HAL_MIN_NUM_OF_QUEUE(unit); queue_num <= HAL_MAX_NUM_OF_QUEUE(unit); queue_num++)
    {

        if ((ret = dal_cypress_qos_priMap_get(unit, queue_num, &pri2qid)) != RT_ERR_OK)
        {
            return ret;
        }

        pri2qid.pri2queue[pri] = qid;

        if ((ret = dal_cypress_qos_priMap_set(unit, queue_num, &pri2qid)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_1pDfltPri_get
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
dal_cypress_qos_1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_RMK_CTRLr, CYPRESS_IPRI_DFLT_PRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_1pDfltPri_get */

/* Function Name:
 *      dal_cypress_qos_1pDfltPri_set
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
dal_cypress_qos_1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d", unit, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to chip */
    if ((ret = reg_field_write(unit, CYPRESS_RMK_CTRLr, CYPRESS_IPRI_DFLT_PRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_1pDfltPri_set */

/* Function Name:
 *      dal_cypress_qos_1pDfltPriSrcSel_get
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
dal_cypress_qos_1pDfltPriSrcSel_get(uint32 unit, rtk_qos_1pDfltPriSrc_t *pType)
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
    if ((ret = reg_field_read(unit, CYPRESS_RMK_CTRLr, CYPRESS_IPRI_DFLT_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value)
        *pType = INNER_1P_DFLT_SRC_DFLT_PRI;
    else if (1 == value)
        *pType = INNER_1P_DFLT_SRC_PB_PRI;
    else
        *pType = INNER_1P_DFLT_SRC_INT_PRI;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pPri=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portOuter1pDfltPriSrcSel_get */

/* Function Name:
 *      dal_cypress_qos_1pDfltPriSrcSel_set
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
dal_cypress_qos_1pDfltPriSrcSel_set(uint32 unit, rtk_qos_1pDfltPriSrc_t type)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= INNER_1P_DFLT_SRC_END, RT_ERR_INPUT);

    switch (type)
    {
        case INNER_1P_DFLT_SRC_DFLT_PRI:
            value = 0;
            break;
        case INNER_1P_DFLT_SRC_INT_PRI:
            value = 2;
            break;
        case INNER_1P_DFLT_SRC_PB_PRI:
            value = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* program value to chip */
    if ((ret = reg_field_write(unit, CYPRESS_RMK_CTRLr, CYPRESS_IPRI_DFLT_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_cypress_qos_port1pRemarkEnable_get
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
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The status of 802.1p remark:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_qos_port1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_IPRI_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_port1pRemarkEnable_get */

/* Function Name:
 *      dal_cypress_qos_port1pRemarkEnable_set
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
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *    The status of 802.1p remark:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_qos_port1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_IPRI_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_port1pRemarkEnable_set */

/* Function Name:
 *      dal_cypress_qos_1pRemarking_get
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
dal_cypress_qos_1pRemarking_get(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t *pPri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch(src)
    {
        case DOT_1P_RMK_SRC_INT_PRI:
        case DOT_1P_RMK_SRC_USER_PRI:
        case DOT_1P_RMK_SRC_OUTER_USER_PRI:
            return dal_cypress_qos_1pRemark_get(unit, val.pri.val, pPri);
        case DOT_1P_RMK_SRC_DSCP:
            return dal_cypress_qos_dscp2Dot1pRemark_get(unit, val.dscp.val, pPri);
        default:
            return RT_ERR_FAILED;
    }
}

/* Function Name:
 *      dal_cypress_qos_1pRemarking_set
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
dal_cypress_qos_1pRemarking_set(uint32 unit, rtk_qos_1pRmkSrc_t src, rtk_qos_1pRmkVal_t val, rtk_pri_t pri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch(src)
    {
        case DOT_1P_RMK_SRC_INT_PRI:
        case DOT_1P_RMK_SRC_USER_PRI:
        case DOT_1P_RMK_SRC_OUTER_USER_PRI:
            return dal_cypress_qos_1pRemark_set(unit, val.pri.val, pri);
        case DOT_1P_RMK_SRC_DSCP:
            return dal_cypress_qos_dscp2Dot1pRemark_set(unit, val.dscp.val, pri);
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_1pRemark_get
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
dal_cypress_qos_1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d", unit, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_IPRI_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    CYPRESS_IPRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_1pRemark_get */

/* Function Name:
 *      dal_cypress_qos_1pRemark_set
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
dal_cypress_qos_1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d, dot1p_pri=%d",
           unit, int_pri, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_IPRI_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    CYPRESS_IPRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_1pRemark_set */

/* Function Name:
 *      dal_cypress_qos_1pRemarkSrcSel_get
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
dal_cypress_qos_1pRemarkSrcSel_get(uint32 unit, rtk_qos_1pRmkSrc_t *pType)
{
    int32   ret;
    uint32  value, value_ext;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_RMK_CTRLr, CYPRESS_IPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_RMK_CTRLr, CYPRESS_IPRI_RMK_SRC_EXTf, &value_ext)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if ((0 == value_ext) && (0 == value))
        *pType = DOT_1P_RMK_SRC_INT_PRI;
    else if ((0 == value_ext) && (1 == value))
        *pType = DOT_1P_RMK_SRC_USER_PRI;
    else if (1 == value_ext)
        *pType = DOT_1P_RMK_SRC_OUTER_USER_PRI;
    else if (2 == value_ext)
        *pType = DOT_1P_RMK_SRC_DSCP;
    else
        return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_1pRemarkSrcSel_get */

/* Function Name:
 *      dal_cypress_qos_1pRemarkSrcSel_set
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
dal_cypress_qos_1pRemarkSrcSel_set(uint32 unit, rtk_qos_1pRmkSrc_t type)
{
    int32   ret;
    uint32  value = 0, value_ext = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d",
           unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= DOT_1P_RMK_SRC_END, RT_ERR_INPUT);

    if (DOT_1P_RMK_SRC_INT_PRI == type)
    {
        value_ext = 0;
        value = 0;
    }
    else if (DOT_1P_RMK_SRC_USER_PRI == type)
    {
        value_ext = 0;
        value = 1;
    }
    else if (DOT_1P_RMK_SRC_OUTER_USER_PRI == type)
    {
        value_ext = 1;
    }
    else if (DOT_1P_RMK_SRC_DSCP == type)
    {
        value_ext = 2;
    }
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_field_write(unit, CYPRESS_RMK_CTRLr, CYPRESS_IPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* program value to chip*/
    if ((ret = reg_field_write(unit, CYPRESS_RMK_CTRLr, CYPRESS_IPRI_RMK_SRC_EXTf, &value_ext)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_1pRemarkSrcSel_set */

/* Function Name:
 *      dal_cypress_qos_portOut1pRemarkEnable_get
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
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The status of 802.1p remark:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_qos_portOut1pRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_OPRI_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portOut1pRemarkEnable_get */

/* Function Name:
 *      dal_cypress_qos_portOut1pRemarkEnable_set
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
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *    The status of 802.1p remark:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_qos_portOut1pRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_OPRI_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portOut1pRemarkEnable_set */

/* Function Name:
 *      dal_cypress_qos_outer1pRemark_get
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
dal_cypress_qos_outer1pRemark_get(uint32 unit, rtk_pri_t int_pri, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d", unit, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_OPRI_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    CYPRESS_OPRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_outer1pRemark_get */

/* Function Name:
 *      dal_cypress_qos_outer1pRemark_set
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
dal_cypress_qos_outer1pRemark_set(uint32 unit, rtk_pri_t int_pri, rtk_pri_t dot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d, dot1p_pri=%d",
           unit, int_pri, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_OPRI_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    CYPRESS_OPRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_outer1pRemark_set */

/* Function Name:
 *      dal_cypress_qos_outer1pRemarking_get
 * Description:
 *      Get the original outer priority/dscp to remarkd outer 802.1p priority(3bits) mapping.
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
 *      - OUTER_1P_RMK_SRC_INNER_USER_PRI
 *      - OUTER_1P_RMK_SRC_DSCP
 */
int32
dal_cypress_qos_outer1pRemarking_get(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t *pPri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch(src)
    {
        case OUTER_1P_RMK_SRC_INT_PRI:
        case OUTER_1P_RMK_SRC_USER_PRI:
        case OUTER_1P_RMK_SRC_INNER_USER_PRI:
            return dal_cypress_qos_outer1pRemark_get(unit, val.pri.val, pPri);
        case OUTER_1P_RMK_SRC_DSCP:
            return dal_cypress_qos_dscp2Outer1pRemark_get(unit, val.dscp.val, pPri);
        default:
            return RT_ERR_FAILED;
    }
}

/* Function Name:
 *      dal_cypress_qos_outer1pRemarking_set
 * Description:
 *      Set the original outer priority/dscp to remarkd outer 802.1p priority(3bits) mapping.
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
 *      - OUTER_1P_RMK_SRC_INNER_USER_PRI
 *      - OUTER_1P_RMK_SRC_DSCP
 */
int32
dal_cypress_qos_outer1pRemarking_set(uint32 unit, rtk_qos_outer1pRmkSrc_t src, rtk_qos_outer1pRmkVal_t val, rtk_pri_t pri)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch(src)
    {
        case OUTER_1P_RMK_SRC_INT_PRI:
        case OUTER_1P_RMK_SRC_USER_PRI:
        case OUTER_1P_RMK_SRC_INNER_USER_PRI:
            return dal_cypress_qos_outer1pRemark_set(unit, val.pri.val, pri);
        case OUTER_1P_RMK_SRC_DSCP:
            return dal_cypress_qos_dscp2Outer1pRemark_set(unit, val.dscp.val, pri);
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_deiRemark_get
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
dal_cypress_qos_deiRemark_get(uint32 unit, uint32 dp, uint32 *pDei)
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
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_DEI_CTRLr, REG_ARRAY_INDEX_NONE, dp,
                    CYPRESS_DP2DEIf, pDei)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDei=%d", *pDei);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_deiRemark_get */

/* Function Name:
 *      dal_cypress_qos_deiRemark_set
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
dal_cypress_qos_deiRemark_set(uint32 unit, uint32 dp, uint32 dei)
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
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_DEI_CTRLr, REG_ARRAY_INDEX_NONE, dp,
                    CYPRESS_DP2DEIf, &dei)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_deiRemark_set */

/* Function Name:
 *      dal_cypress_qos_deiRemarking_get
 * Description:
 *      Get the dp to DEI remarking mapping.
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
dal_cypress_qos_deiRemarking_get(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 *pDei)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch(src)
    {
        case DEI_RMK_SRC_DP:
            return dal_cypress_qos_deiRemark_get(unit, val.dp.val, pDei);
        default:
            return RT_ERR_FAILED;
    }
}

/* Function Name:
 *      dal_cypress_qos_deiRemarking_set
 * Description:
 *      Set the dp to DEI remarking mapping.
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
dal_cypress_qos_deiRemarking_set(uint32 unit, rtk_qos_deiRmkSrc_t src, rtk_qos_deiRmkVal_t val, uint32 dei)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch(src)
    {
        case DEI_RMK_SRC_DP:
            return dal_cypress_qos_deiRemark_set(unit, val.dp.val, dei);
        default:
            return RT_ERR_FAILED;
    }
}

/* Function Name:
 *      dal_cypress_qos_portDeiRemarkEnable_get
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
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The status of DEI remark:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_qos_portDeiRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_DEI_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portDeiRemarkEnable_get */

/* Function Name:
 *      dal_cypress_qos_portDeiRemarkEnable_set
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
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *    The status of DEI remark:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_qos_portDeiRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_DEI_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portDeiRemarkEnable_set */

/* Function Name:
 *      dal_cypress_qos_portDeiRemarkTagSel_get
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
dal_cypress_qos_portDeiRemarkTagSel_get(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t *pType)
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
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_PORT_DEI_TAG_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_DEI_TAG_SELf, &value)) != RT_ERR_OK)
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
} /* end of dal_cypress_qos_portDeiRemarkTagSel_get */

/* Function Name:
 *      dal_cypress_qos_portDeiRemarkTagSel_set
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
dal_cypress_qos_portDeiRemarkTagSel_set(uint32 unit, rtk_port_t port, rtk_qos_deiSel_t type)
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
    else
        value = 1;

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_PORT_DEI_TAG_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_DEI_TAG_SELf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portDeiRemarkTagSel_set */

/* Function Name:
 *      dal_cypress_qos_outer1pRemarkSrcSel_get
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
dal_cypress_qos_outer1pRemarkSrcSel_get(uint32 unit, rtk_qos_outer1pRmkSrc_t *pType)
{
    int32   ret;
    uint32  value, value_ext;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_RMK_CTRLr, CYPRESS_OPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_RMK_CTRLr, CYPRESS_OPRI_RMK_SRC_EXTf, &value_ext)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if ((0 == value_ext) && (0 == value))
        *pType = OUTER_1P_RMK_SRC_INT_PRI;
    else if ((0 == value_ext) && (1 == value))
        *pType = OUTER_1P_RMK_SRC_USER_PRI;
    else if (1 == value_ext)
        *pType = OUTER_1P_RMK_SRC_INNER_USER_PRI;
    else if (2 == value_ext)
        *pType = OUTER_1P_RMK_SRC_DSCP;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_out1pRemarkSrcSel_get */

/* Function Name:
 *      dal_cypress_qos_outer1pRemarkSrcSel_set
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
dal_cypress_qos_outer1pRemarkSrcSel_set(uint32 unit, rtk_qos_outer1pRmkSrc_t type)
{
    int32   ret;
    uint32  value = 0, value_ext = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d",
           unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= OUTER_1P_RMK_SRC_END, RT_ERR_INPUT);

    if (OUTER_1P_RMK_SRC_INT_PRI == type)
    {
        value_ext = 0;
        value = 0;
    }
    else if (OUTER_1P_RMK_SRC_USER_PRI == type)
    {
        value_ext = 0;
        value = 1;
    }
    else if (OUTER_1P_RMK_SRC_INNER_USER_PRI == type)
    {
        value_ext = 1;
    }
    else if (OUTER_1P_RMK_SRC_DSCP == type)
    {
        value_ext = 2;
    }
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_field_write(unit, CYPRESS_RMK_CTRLr, CYPRESS_OPRI_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* program value to chip*/
    if ((ret = reg_field_write(unit, CYPRESS_RMK_CTRLr, CYPRESS_OPRI_RMK_SRC_EXTf, &value_ext)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_out1pRemarkSrcSel_set */

/* Function Name:
 *      dal_cypress_qos_outer1pDfltPri_get
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
dal_cypress_qos_outer1pDfltPri_get(uint32 unit, rtk_pri_t *pDot1p_pri)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1p_pri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_RMK_CTRLr, CYPRESS_OPRI_DFLT_PRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_outer1pDfltPri_get */

/* Function Name:
 *      dal_cypress_qos_outer1pDfltPri_set
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
dal_cypress_qos_outer1pDfltPri_set(uint32 unit, rtk_pri_t dot1p_pri)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dot1p_pri=%d", unit, dot1p_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dot1p_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to chip */
    if ((ret = reg_field_write(unit, CYPRESS_RMK_CTRLr, CYPRESS_OPRI_DFLT_PRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_outer1pDfltPri_set */

/* Function Name:
 *      dal_cypress_qos_outer1pDfltPriCfgSrcSel_get
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
dal_cypress_qos_outer1pDfltPriCfgSrcSel_get(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t *pDflt_sel)
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
    if ((ret = reg_field_read(unit, CYPRESS_RMK_CTRLr, CYPRESS_OPRI_DFLT_CFGf, &value)) != RT_ERR_OK)
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
 *      dal_cypress_qos_outer1pDfltPriCfgSrcSel_set
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
dal_cypress_qos_outer1pDfltPriCfgSrcSel_set(uint32 unit, rtk_qos_outer1pDfltCfgSrc_t dflt_sel)
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
    if ((ret = reg_field_write(unit, CYPRESS_RMK_CTRLr, CYPRESS_OPRI_DFLT_CFGf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_outer1pDfltPriCfgSrcSel_set */

/* Function Name:
 *      dal_cypress_qos_portOuter1pDfltPriSrcSel_get
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
 *      None.
 */
int32
dal_cypress_qos_portOuter1pDfltPriSrcSel_get(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t *pType)
{
    int32   ret;
    uint32  value, value_ext;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_PORT_OPRI_SRC_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_OPRI_DFLT_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_PORT_OPRI_SRC_EXT_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_OPRI_DFLT_SRC_EXTf, &value_ext)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if (0 == value_ext)
    {
        if (0 == value)
            *pType = OUTER_1P_DFLT_SRC_USER_PRI;
        else
            *pType = OUTER_1P_DFLT_SRC_INT_PRI;
    }
    else if (1 == value_ext)
        *pType = OUTER_1P_DFLT_SRC_DFLT_PRI;
    else
        *pType = OUTER_1P_DFLT_SRC_PB_PRI;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portOuter1pDfltPriSrcSel_get */

/* Function Name:
 *      dal_cypress_qos_portOuter1pDfltPriSrcSel_set
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
 *      None.
 */
int32
dal_cypress_qos_portOuter1pDfltPriSrcSel_set(uint32 unit, rtk_port_t port, rtk_qos_outer1pDfltSrc_t type)
{
    int32   ret;
    uint32  value = 0, value_ext = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type >= OUTER_1P_DFLT_SRC_END, RT_ERR_INPUT);

    switch (type)
    {
        case OUTER_1P_DFLT_SRC_USER_PRI:
            value_ext = 0;
            value = 0;
            break;
        case OUTER_1P_DFLT_SRC_INT_PRI:
            value_ext = 0;
            value = 1;
            break;
        case OUTER_1P_DFLT_SRC_DFLT_PRI:
            value_ext = 1;
            value = 0;
            break;
        case OUTER_1P_DFLT_SRC_PB_PRI:
            value_ext = 2;
            value = 0;
            break;
        default:
            return RT_ERR_INPUT;
    }

    QOS_SEM_LOCK(unit);

    /* program value to chip */
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_PORT_OPRI_SRC_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_OPRI_DFLT_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_PORT_OPRI_SRC_EXT_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_OPRI_DFLT_SRC_EXTf, &value_ext)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portOuter1pDfltPriSrcSel_set */

/* Function Name:
 *      dal_cypress_qos_portDscpRemarkEnable_get
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
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The status of DSCP remark:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_qos_portDscpRemarkEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_DSCP_RMK_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portDscpRemarkEnable_get */

/* Function Name:
 *      dal_cypress_qos_portDscpRemarkEnable_set
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
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *    The status of DSCP remark:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_qos_portDscpRemarkEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_PORT_RMK_EN_CTRLr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_DSCP_RMK_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_portDscpRemarkEnable_set */

/* Function Name:
 *      dal_cypress_qos_dscpRemarking_get
 * Description:
 *      Get the internal priority/original inner priority/original outer priority/dscp to
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
dal_cypress_qos_dscpRemarking_get(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 *pDscp)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch (src)
    {   /* In ASIC, only one remarking table. */
        case DSCP_RMK_SRC_INT_PRI:
        case DSCP_RMK_SRC_USER_PRI:
        case DSCP_RMK_SRC_OUTER_USER_PRI:
            return dal_cypress_qos_dscpRemark_get(unit, val.pri.val, pDscp);
        case DSCP_RMK_SRC_DSCP:
            return dal_cypress_qos_dscp2DscpRemark_get(unit, val.dscp.val, pDscp);
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_dscpRemarking_set
 * Description:
 *      Set the internal priority/original inner priority/original outer priority/dscp to
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
dal_cypress_qos_dscpRemarking_set(uint32 unit, rtk_qos_dscpRmkSrc_t src, rtk_qos_dscpRmkVal_t val, uint32 dscp)
{
    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    switch (src)
    {   /* In ASIC, only one remarking table. */
        case DSCP_RMK_SRC_INT_PRI:
        case DSCP_RMK_SRC_USER_PRI:
        case DSCP_RMK_SRC_OUTER_USER_PRI:
            return dal_cypress_qos_dscpRemark_set(unit, val.pri.val, dscp);
        case DSCP_RMK_SRC_DSCP:
            return dal_cypress_qos_dscp2DscpRemark_set(unit, val.dscp.val, dscp);
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_dscpRemark_get
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
dal_cypress_qos_dscpRemark_get(uint32 unit, rtk_pri_t int_pri, uint32 *pDscp)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d", unit, int_pri);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    CYPRESS_DSCPf, pDscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDscp=%d", *pDscp);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscpRemark_get */

/* Function Name:
 *      dal_cypress_qos_dscpRemark_set
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
dal_cypress_qos_dscpRemark_set(uint32 unit, rtk_pri_t int_pri, uint32 dscp)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, int_pri=%d, dscp=%d",
           unit, int_pri, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);
    RT_PARAM_CHK(int_pri > HAL_INTERNAL_PRIORITY_MAX(unit), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, int_pri,
                    CYPRESS_DSCPf, &dscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscpRemark_set */

/* Function Name:
 *      dal_cypress_qos_dscp2Dot1pRemark_get
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
dal_cypress_qos_dscp2Dot1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri)
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
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_DSCP2IPRI_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    CYPRESS_DSCP2IPRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscp2DscpRemark_get */

/* Function Name:
 *      dal_cypress_qos_dscp2Dot1pRemark_set
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
dal_cypress_qos_dscp2Dot1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri)
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
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_DSCP2IPRI_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    CYPRESS_DSCP2IPRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscp2Dot1pRemark_set */

/* Function Name:
 *      dal_cypress_qos_dscp2Outer1pRemark_get
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
dal_cypress_qos_dscp2Outer1pRemark_get(uint32 unit, uint32 dscp, rtk_pri_t *pDot1p_pri)
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
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_DSCP2OPRI_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    CYPRESS_DSCP2OPRIf, pDot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDot1p_pri=%d", *pDot1p_pri);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscp2Outer1pRemark_get */

/* Function Name:
 *      dal_cypress_qos_dscp2Outer1pRemark_set
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
dal_cypress_qos_dscp2Outer1pRemark_set(uint32 unit, uint32 dscp, rtk_pri_t dot1p_pri)
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
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_DSCP2OPRI_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    CYPRESS_DSCP2OPRIf, &dot1p_pri)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscp2Outer1pRemark_set */

/* Function Name:
 *      dal_cypress_qos_dscp2DscpRemark_get
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
dal_cypress_qos_dscp2DscpRemark_get(uint32 unit, uint32 dscp, uint32 *pDscp)
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
    if ((ret = reg_array_field_read(unit, CYPRESS_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    CYPRESS_DSCPf, pDscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pDscp=%d", *pDscp);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscp2DscpRemark_get */

/* Function Name:
 *      dal_cypress_qos_dscp2DscpRemark_set
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
dal_cypress_qos_dscp2DscpRemark_set(uint32 unit, uint32 dscp, uint32 rmkDscp)
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
    if ((ret = reg_array_field_write(unit, CYPRESS_RMK_DSCP_CTRLr, REG_ARRAY_INDEX_NONE, dscp,
                    CYPRESS_DSCPf, &rmkDscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscp2DscpRemark_set */

/* Function Name:
 *      dal_cypress_qos_dscpRemarkSrcSel_get
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
dal_cypress_qos_dscpRemarkSrcSel_get(uint32 unit, rtk_qos_dscpRmkSrc_t *pType)
{
    int32   ret;
    uint32  value, value_ext;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_RMK_CTRLr, CYPRESS_DSCP_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_RMK_CTRLr, CYPRESS_DSCP_RMK_SRC_EXTf, &value_ext)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    if ((0 == value_ext) && (0 == value))
        *pType = DSCP_RMK_SRC_INT_PRI;
    else if ((0 == value_ext) && (1 == value))
        *pType = DSCP_RMK_SRC_DSCP;
    else if ((0 == value_ext) && (2 == value))
        *pType = DSCP_RMK_SRC_DP;
    else if (1 == value_ext)
        *pType = DSCP_RMK_SRC_USER_PRI;
    else if (2 == value_ext)
        *pType = DSCP_RMK_SRC_OUTER_USER_PRI;
    else
        return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pType=%d", *pType);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscpRemarkSrcSel_get */

/* Function Name:
 *      dal_cypress_qos_dscpRemarkSrcSel_set
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
dal_cypress_qos_dscpRemarkSrcSel_set(uint32 unit, rtk_qos_dscpRmkSrc_t type)
{
    int32   ret;
    uint32  value = 0, value_ext = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, type=%d",
           unit, type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= DSCP_RMK_SRC_END, RT_ERR_INPUT);

    if (DSCP_RMK_SRC_INT_PRI == type)
    {
        value_ext = 0;
        value = 0;
    }
    else if (DSCP_RMK_SRC_DSCP == type)
    {
        value_ext = 0;
        value = 1;
    }
    else if (DSCP_RMK_SRC_DP == type)
    {
        value_ext = 0;
        value = 2;
    }
    else if (DSCP_RMK_SRC_USER_PRI == type)
    {
        value_ext = 1;
    }
    else if (DSCP_RMK_SRC_OUTER_USER_PRI == type)
    {
        value_ext = 2;
    }
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* program value to chip*/
    if ((ret = reg_field_write(unit, CYPRESS_RMK_CTRLr, CYPRESS_DSCP_RMK_SRCf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* program value to chip*/
    if ((ret = reg_field_write(unit, CYPRESS_RMK_CTRLr, CYPRESS_DSCP_RMK_SRC_EXTf, &value_ext)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_qos_dscpRemarkSrcSel_set */

/* Function Name:
 *      dal_cypress_qos_schedulingAlgorithm_get
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
dal_cypress_qos_schedulingAlgorithm_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   *pScheduling_type)
{
    int32   ret;
    uint32  value;
    sched_entry_t pSchedEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScheduling_type), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    osal_memset(&pSchedEntry, 0, sizeof(sched_entry_t));

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_SCHEDt, CYPRESS_SCHED_SCHED_TYPEtf,
                    &value, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
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
} /* end of dal_cypress_qos_schedulingAlgorithm_get */

/* Function Name:
 *      dal_cypress_qos_schedulingAlgorithm_set
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
dal_cypress_qos_schedulingAlgorithm_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_qos_scheduling_type_t   scheduling_type)
{
    int32   ret;
    uint32  value;
    sched_entry_t pSchedEntry;
    rtk_qos_scheduling_type_t oldSched;
    uint8 p_flag = FALSE;
    uint32 rate = 0, state;
    rtk_action_t action = ACTION_FORWARD;
    rtk_trap_oam_action_t trap_action;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, scheduling_type=%d",
           unit, port, scheduling_type);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(scheduling_type >= SCHEDULING_TYPE_END, RT_ERR_QOS_SCHE_TYPE);

    /* E0014503: Get current scheduling type and check algorithm state <START> */
    if (!HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = dal_cypress_qos_schedulingAlgorithm_get(unit, port, &oldSched)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        if ((scheduling_type == WFQ) && (oldSched == WRR))
        {
            p_flag = TRUE;
        }
    } /* E0014503: <END> */

    QOS_SEM_LOCK(unit);

    /* E0014503: Clean TX queue and leaky bucket <START> */
    if (!HWP_IS_CPU_PORT(unit, port))
    {
        if (p_flag)
        {
            if ((ret = dal_cypress_common_portEgrQueueEmpty_start(unit, port, &state, &rate,
                                &trap_action, &action)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
        }
    } /* E0014503: <END> */

    /* program value to CHIP*/
    switch (scheduling_type)
    {
        case WFQ:
            value = 0;
            break;
        case WRR:
        default:
            value = 1;
    }

    osal_memset(&pSchedEntry, 0, sizeof(sched_entry_t));
    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        goto Err;
    }
    if ((ret = table_field_set(unit, CYPRESS_SCHEDt, CYPRESS_SCHED_SCHED_TYPEtf,
                    &value, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        goto Err;
    }
    if ((ret = table_write(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    }

Err:
    /* E0014503: Restore TX queue and leaky bucket counting ability <START> */
    if (!HWP_IS_CPU_PORT(unit, port))
    {
        if (p_flag)
        {
            if ((ret = dal_cypress_common_portEgrQueueEmpty_end(unit, port, Q_EMPTY_SCHED_ALGO, state,
                                rate, trap_action, action)) != RT_ERR_OK)
            {
                QOS_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
                return ret;
            }
        }
    } /* E0014503: <END> */

    QOS_SEM_UNLOCK(unit);

    return ret;
} /* end of dal_cypress_qos_schedulingAlgorithm_set */

/* Function Name:
 *      dal_cypress_qos_schedulingQueue_get
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
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *    If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 */
int32
dal_cypress_qos_schedulingQueue_get(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32       ret;
    rtk_qid_t   qid;
    sched_entry_t pSchedEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    osal_memset(&pSchedEntry, 0, sizeof(sched_entry_t));

    /* get value from CHIP */
    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    for (qid = 0; qid <= (HAL_MAX_NUM_OF_QUEUE(unit) - 1); qid++)
    {
        if ((ret = table_field_get(unit, CYPRESS_SCHEDt, (uint32)schedWeight_fieldidx[qid],
                        &pQweights->weights[qid], (uint32 *) &pSchedEntry)) != RT_ERR_OK)
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
} /* end of dal_cypress_qos_schedulingQueue_get */

/* Function Name:
 *      dal_cypress_qos_schedulingQueue_set
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
 *      RT_ERR_PORT_ID          - Invalid port id
 *      RT_ERR_NULL_POINTER     - Null pointer
 *      RT_ERR_QOS_QUEUE_WEIGHT - Invalid queue weight
 * Note:
 *    The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *    If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_WRR_PRIORITY.
 */
int32
dal_cypress_qos_schedulingQueue_set(uint32 unit, rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32       ret;
    rtk_qid_t   qid;
    sched_entry_t pSchedEntry;

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

    osal_memset(&pSchedEntry, 0, sizeof(sched_entry_t));

    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
    }

    for (qid = 0; qid <= (HAL_MAX_NUM_OF_QUEUE(unit) - 1); qid++)
    {
        if ((ret = table_field_set(unit, CYPRESS_SCHEDt, (uint32)schedWeight_fieldidx[qid],
                        &pQweights->weights[qid], (uint32 *) &pSchedEntry)) != RT_ERR_OK)
        {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    if ((ret = table_write(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
            QOS_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;

} /* end of dal_cypress_qos_schedulingQueue_set */


/* Module Name    : QoS              */
/* Sub-module Name: Congestion avoidance */

/* Function Name:
 *      dal_cypress_qos_congAvoidAlgo_get
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
dal_cypress_qos_congAvoidAlgo_get(uint32 unit, rtk_qos_congAvoidAlgo_t *pAlgo)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAlgo), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_WRED_GLB_CTRLr, CYPRESS_DROP_TYPE_SELf, &value)) != RT_ERR_OK)
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
 *      dal_cypress_qos_congAvoidAlgo_set
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
dal_cypress_qos_congAvoidAlgo_set(uint32 unit, rtk_qos_congAvoidAlgo_t algo)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, algo=%d", unit, algo);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(algo >= CONG_AVOID_END, RT_ERR_INPUT);

    if(CONG_AVOID_TD == algo)
        value = 0;
    else if(CONG_AVOID_SWRED == algo)
        value = 1;
    else
        return RT_ERR_INPUT;

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_WRED_GLB_CTRLr, CYPRESS_DROP_TYPE_SELf, &value)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_congAvoidSysThresh_get
 * Description:
 *      Get system threshold of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      dp               - drop precedence
 * Output:
 *      pCongAvoidThresh - pointer to system threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_qos_congAvoidSysThresh_get(
    uint32                      unit,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dp=%d", unit, dp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK((NULL == pCongAvoidThresh), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_WRED_PORT_THR_CTRLr, REG_ARRAY_INDEX_NONE, dp,
                CYPRESS_THMAX_DPf, &(pCongAvoidThresh->maxThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_WRED_PORT_THR_CTRLr, REG_ARRAY_INDEX_NONE, dp,
                CYPRESS_THMIN_DPf, &(pCongAvoidThresh->minThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "maxThresh=0x%x, minThresh=0x%x",
           pCongAvoidThresh->maxThresh, pCongAvoidThresh->minThresh);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_congAvoidSysThresh_set
 * Description:
 *      Set system threshold of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      dp               - drop precedence
 *      pCongAvoidThresh - system threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_qos_congAvoidSysThresh_set(
    uint32                      unit,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pCongAvoidThresh), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, dp=%d, maxThresh=0x%x, minThresh=0x%x",
        unit, dp, pCongAvoidThresh->maxThresh, pCongAvoidThresh->minThresh);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK(pCongAvoidThresh->maxThresh > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_INPUT);
    RT_PARAM_CHK(pCongAvoidThresh->minThresh > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_WRED_PORT_THR_CTRLr, REG_ARRAY_INDEX_NONE, dp,
                CYPRESS_THMAX_DPf, &(pCongAvoidThresh->maxThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_WRED_PORT_THR_CTRLr, REG_ARRAY_INDEX_NONE, dp,
                CYPRESS_THMIN_DPf, &(pCongAvoidThresh->minThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_congAvoidSysDropProbability_get
 * Description:
 *      Get system threshold of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      dp               - drop precedence
 * Output:
 *      pProbability     - pointer to drop probability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_qos_congAvoidSysDropProbability_get(
    uint32  unit,
    uint32  dp,
    uint32  *pProbability)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, dp=%d", unit, dp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK((NULL == pProbability), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_WRED_PORT_THR_CTRLr, REG_ARRAY_INDEX_NONE, dp,
                CYPRESS_DROP_RATE_DPf, pProbability)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pProbability=0x%x",
           *pProbability);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_congAvoidSysDropProbability_set
 * Description:
 *      Set system drop probability of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      dp               - drop precedence
 *      probability      - drop probability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_qos_congAvoidSysDropProbability_set(
    uint32  unit,
    uint32  dp,
    uint32  probability)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, dp=%d, probability", unit, dp, probability);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK(probability > HAL_WRED_DROP_PROBABILITY_MAX(unit), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_WRED_PORT_THR_CTRLr, REG_ARRAY_INDEX_NONE, dp,
                CYPRESS_DROP_RATE_DPf, &probability)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueThresh_get
 * Description:
 *      Get global queue threshold of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      queue            - queue id
 *      dp               - drop precedence
 * Output:
 *      pCongAvoidThresh - pointer to system threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_qos_congAvoidGlobalQueueThresh_get(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue=%d, dp=%d", unit, queue, dp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((queue >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK((NULL == pCongAvoidThresh), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_WRED_QUEUE_THR_CTRLr, queue, dp,
                CYPRESS_Q_THMAX_DPf, &(pCongAvoidThresh->maxThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_WRED_QUEUE_THR_CTRLr, queue, dp,
                CYPRESS_Q_THMIN_DPf, &(pCongAvoidThresh->minThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "maxThresh=0x%x, minThresh=0x%x",
           pCongAvoidThresh->maxThresh, pCongAvoidThresh->minThresh);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueThresh_set
 * Description:
 *      Set global queue threshold of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      queue            - queue id
 *      dp               - drop precedence
 *      pCongAvoidThresh - system threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_qos_congAvoidGlobalQueueThresh_set(
    uint32                      unit,
    rtk_qid_t                   queue,
    uint32                      dp,
    rtk_qos_congAvoidThresh_t   *pCongAvoidThresh)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pCongAvoidThresh), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue=%d, dp=%d, high=0x%x, low=0x%x",
        unit, queue, dp, pCongAvoidThresh->maxThresh, pCongAvoidThresh->minThresh);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((queue >=  HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK(pCongAvoidThresh->maxThresh > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_INPUT);
    RT_PARAM_CHK(pCongAvoidThresh->minThresh > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_WRED_QUEUE_THR_CTRLr, queue, dp,
                CYPRESS_Q_THMAX_DPf, &(pCongAvoidThresh->maxThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_WRED_QUEUE_THR_CTRLr, queue, dp,
                CYPRESS_Q_THMIN_DPf, &(pCongAvoidThresh->minThresh))) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueDropProbability_get
 * Description:
 *      Get global queue drop probability of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      queue            - queue id
 *      dp               - drop precedence
 * Output:
 *      pProbability     - pointer to drop probability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_qos_congAvoidGlobalQueueDropProbability_get(
    uint32      unit,
    rtk_qid_t   queue,
    uint32      dp,
    uint32      *pProbability)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue=%d, dp=%d", unit, queue, dp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK((NULL == pProbability), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_WRED_QUEUE_THR_CTRLr, queue, dp,
                CYPRESS_Q_DROP_RATE_DPf, pProbability)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pProbability=0x%x",
           *pProbability);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueDropProbability_set
 * Description:
 *      Set system drop probability of congestion avoidance.
 * Input:
 *      unit             - unit id
 *      queue            - queue id
 *      dp               - drop precedence
 *      probability      - drop probability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_qos_congAvoidGlobalQueueDropProbability_set(
    uint32      unit,
    rtk_qid_t   queue,
    uint32      dp,
    uint32      probability)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue=%d, dp=%d, probability=0x%x",
        unit, queue, dp, probability);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK(probability > HAL_WRED_DROP_PROBABILITY_MAX(unit), RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_WRED_QUEUE_THR_CTRLr, queue, dp,
                CYPRESS_Q_DROP_RATE_DPf, &probability)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueConfig_get
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
dal_cypress_qos_congAvoidGlobalQueueConfig_get(
    uint32    unit,
    rtk_qid_t queue,
    uint32    dp,
    rtk_qos_congAvoidThresh_t *pCongAvoidThresh)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue=%d, dp=%d", unit, queue, dp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* get value from CHIP */
    if ((ret = dal_cypress_qos_congAvoidSysDropProbability_get(unit, dp,
                                    &(pCongAvoidThresh->probability))) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = dal_cypress_qos_congAvoidSysThresh_get(unit, dp, pCongAvoidThresh)) != RT_ERR_OK)
    {
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS),
        "pCongAvoidThresh->maxThresh=%d, \
         pCongAvoidThresh->minThresh=%d, \
         pCongAvoidThresh->probability=%d",
         pCongAvoidThresh->maxThresh,
         pCongAvoidThresh->minThresh,
         pCongAvoidThresh->probability);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_congAvoidGlobalQueueConfig_set
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
dal_cypress_qos_congAvoidGlobalQueueConfig_set(
    uint32    unit,
    rtk_qid_t queue,
    uint32    dp,
    rtk_qos_congAvoidThresh_t *pCongAvoidThresh)
{
    int32       ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, queue=%d, dp=%d, \
        pCongAvoidThresh->maxThresh=%d, \
        pCongAvoidThresh->minThresh=%d, \
        pCongAvoidThresh->probability=%d", unit, queue, dp,
        pCongAvoidThresh->maxThresh,
        pCongAvoidThresh->minThresh,
        pCongAvoidThresh->probability);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    if ((ret = dal_cypress_qos_congAvoidSysDropProbability_set(unit, dp,
                                    pCongAvoidThresh->probability)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = dal_cypress_qos_congAvoidSysThresh_set(unit, dp, pCongAvoidThresh)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_portAvbStreamReservationClassEnable_get
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
 *      RT_ERR_AVB_INVALID_SR_CLASS - Invalid SR Class
 *      RT_ERR_INPUT                - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_qos_portAvbStreamReservationClassEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            *pEnable)
{
    uint32 en = 0;
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, srClass=%d",
           unit, port, srClass);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((srClass >= AVB_SR_CLASS_END), RT_ERR_AVB_INVALID_SR_CLASS);

    QOS_SEM_LOCK(unit);

    switch (srClass)
    {
    case AVB_SR_CLASS_A:
        RT_IF_ERR_GOTO_HANDLE(reg_array_field_read(unit, CYPRESS_AVB_PORT_CLASS_A_ENr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ENf, &en), errHandle, ret);
        break;

    case AVB_SR_CLASS_B:
        RT_IF_ERR_GOTO_HANDLE(reg_array_field_read(unit, CYPRESS_AVB_PORT_CLASS_B_ENr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ENf, &en), errHandle, ret);
        break;

    default:
        QOS_SEM_UNLOCK(unit);
        return RT_ERR_INPUT;
    }

    *pEnable = (en)? ENABLED : DISABLED;

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    QOS_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    return ret;
} /* end of dal_cypress_qos_portAvbStreamReservationClassEnable_get */

/* Function Name:
 *      dal_cypress_qos_portAvbStreamReservationClassEnable_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_AVB_INVALID_SR_CLASS - Invalid SR Class
 *      RT_ERR_INPUT                - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_qos_portAvbStreamReservationClassEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qos_avbSrClass_t    srClass,
    rtk_enable_t            enable)
{
    uint32 en;
    int32 ret;

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
        RT_IF_ERR_GOTO_HANDLE(reg_array_field_write(unit, CYPRESS_AVB_PORT_CLASS_A_ENr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ENf, &en), errHandle, ret);
        RT_IF_ERR_GOTO_HANDLE(reg_array_field_write(unit, CYPRESS_AVB_PORT_CLASS_A_EN_MACr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ENf, &en), errHandle, ret);
        break;

    case AVB_SR_CLASS_B:
        RT_IF_ERR_GOTO_HANDLE(reg_array_field_write(unit, CYPRESS_AVB_PORT_CLASS_B_ENr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ENf, &en), errHandle, ret);
        RT_IF_ERR_GOTO_HANDLE(reg_array_field_write(unit, CYPRESS_AVB_PORT_CLASS_B_EN_MACr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ENf, &en), errHandle, ret);
        break;

    default:
        QOS_SEM_UNLOCK(unit);
        return RT_ERR_INPUT;
    }

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    QOS_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    return ret;
} /* end of dal_cypress_qos_portAvbStreamReservationClassEnable_set */

/* Function Name:
 *      dal_cypress_qos_avbStreamReservationConfig_get
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
dal_cypress_qos_avbStreamReservationConfig_get(uint32 unit, rtk_qos_avbSrConf_t *pSrConf)
{
    uint32 reg_val = 0, val;
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSrConf), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* CYPRESS_AVB_CTRLr */
    RT_IF_ERR_GOTO_HANDLE(reg_read(unit, CYPRESS_AVB_CTRLr, &reg_val), errHandle, ret);

    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_A_PRIf, &val, &reg_val), errHandle, ret);
    pSrConf->class_a_priority = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_B_PRIf, &val, &reg_val), errHandle, ret);
    pSrConf->class_b_priority = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_A_QIDf, &val, &reg_val), errHandle, ret);
    pSrConf->class_a_queue_id = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_B_QIDf, &val, &reg_val), errHandle, ret);
    pSrConf->class_b_queue_id = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_NON_A_REDIRECT_QIDf, &val, &reg_val), errHandle, ret);
    pSrConf->class_non_a_redirect_queue_id = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_NON_B_REDIRECT_QIDf, &val, &reg_val), errHandle, ret);
    pSrConf->class_non_b_redirect_queue_id = val & 0x7;

    /* CYPRESS_AVB_CTRL_MACr */
    RT_IF_ERR_GOTO_HANDLE(reg_read(unit, CYPRESS_AVB_CTRL_MACr, &reg_val), errHandle, ret);

    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, CYPRESS_AVB_CTRL_MACr, CYPRESS_CLASS_NON_A_RMK_PRIf, &val, &reg_val), errHandle, ret);
    pSrConf->class_non_a_remark_priority = val & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, CYPRESS_AVB_CTRL_MACr, CYPRESS_CLASS_NON_B_RMK_PRIf, &val, &reg_val), errHandle, ret);
    pSrConf->class_non_b_remark_priority = val & 0x7;

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    QOS_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    return ret;
} /* end of dal_cypress_qos_avbStreamReservationConfig_get */

/* Function Name:
 *      dal_cypress_qos_avbStreamReservationConfig_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_QOS_1P_PRIORITY  - Invalid dot1p priority
 *      RT_ERR_QUEUE_ID         - invalid queue id
 * Note:
 *      None
 */
int32
dal_cypress_qos_avbStreamReservationConfig_set(uint32 unit, rtk_qos_avbSrConf_t *pSrConf)
{
    uint32 reg_val = 0, val;
    int32 ret;

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

    /* CYPRESS_AVB_CTRLr */
    RT_IF_ERR_GOTO_HANDLE(reg_read(unit, CYPRESS_AVB_CTRLr, &reg_val), errHandle, ret);

    val = pSrConf->class_a_priority & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_A_PRIf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_b_priority & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_B_PRIf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_a_queue_id & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_A_QIDf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_b_queue_id & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_B_QIDf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_non_a_redirect_queue_id & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_NON_A_REDIRECT_QIDf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_non_b_redirect_queue_id & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, CYPRESS_AVB_CTRLr, CYPRESS_CLASS_NON_B_REDIRECT_QIDf, &val, &reg_val), errHandle, ret);

    RT_IF_ERR_GOTO_HANDLE(reg_write(unit, CYPRESS_AVB_CTRLr, &reg_val), errHandle, ret);

    /* CYPRESS_AVB_CTRL_MACr */
    RT_IF_ERR_GOTO_HANDLE(reg_read(unit, CYPRESS_AVB_CTRL_MACr, &reg_val), errHandle, ret);

    val = pSrConf->class_non_a_remark_priority & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, CYPRESS_AVB_CTRL_MACr, CYPRESS_CLASS_NON_A_RMK_PRIf, &val, &reg_val), errHandle, ret);
    val = pSrConf->class_non_b_remark_priority & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, CYPRESS_AVB_CTRL_MACr, CYPRESS_CLASS_NON_B_RMK_PRIf, &val, &reg_val), errHandle, ret);

    RT_IF_ERR_GOTO_HANDLE(reg_write(unit, CYPRESS_AVB_CTRL_MACr, &reg_val), errHandle, ret);

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    QOS_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    return ret;
} /* end of dal_cypress_qos_avbStreamReservationConfig_set */

/* Function Name:
 *      dal_cypress_qos_pkt2CpuPriRemap_get
 * Description:
 *      Get the value of new priority to remapping the original internal priority for the packets that normal forwarded to CPU.
 * Input:
 *      unit    - unit id
 *      intPri  - original internal
 * Output:
 *      pNewPri - new priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 */
int32
dal_cypress_qos_pkt2CpuPriRemap_get(uint32 unit, rtk_pri_t intPri, rtk_pri_t *pNewPri)
{
    uint32 reg_val = 0, val;
    uint32 fieldPri[] = { CYPRESS_PRI0f, CYPRESS_PRI1f, CYPRESS_PRI2f, CYPRESS_PRI3f, \
                          CYPRESS_PRI4f, CYPRESS_PRI5f, CYPRESS_PRI6f, CYPRESS_PRI7f };
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((intPri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((intPri >= (sizeof(fieldPri)/sizeof(uint32))), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((NULL == pNewPri), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* CYPRESS_QM_PKT2CPU_INTPRI_MAPr */
    RT_IF_ERR_GOTO_HANDLE(reg_read(unit, CYPRESS_QM_PKT2CPU_INTPRI_MAPr, &reg_val), errHandle, ret);
    RT_IF_ERR_GOTO_HANDLE(reg_field_get(unit, CYPRESS_QM_PKT2CPU_INTPRI_MAPr, fieldPri[intPri], &val, &reg_val), errHandle, ret);

    QOS_SEM_UNLOCK(unit);

    *pNewPri = val & 0x7;
    return RT_ERR_OK;

errHandle:
    QOS_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    return ret;
}

/* Function Name:
 *      dal_cypress_qos_pkt2CpuPriRemap_set
 * Description:
 *      Set the value of new priority to remapping the original internal priority for the packets that normal forwarded to CPU.
 * Input:
 *      unit   - unit id
 *      intPri - original internal
 *      newPri - new priority
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority
 * Note:
 */
int32
dal_cypress_qos_pkt2CpuPriRemap_set(uint32 unit, rtk_pri_t intPri, rtk_pri_t newPri)
{
    uint32 reg_val = 0, val;
    uint32 fieldPri[] = { CYPRESS_PRI0f, CYPRESS_PRI1f, CYPRESS_PRI2f, CYPRESS_PRI3f, \
                          CYPRESS_PRI4f, CYPRESS_PRI5f, CYPRESS_PRI6f, CYPRESS_PRI7f };
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((intPri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((intPri >= (sizeof(fieldPri)/sizeof(uint32))), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((newPri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_QOS_INT_PRIORITY);

    QOS_SEM_LOCK(unit);

    /* CYPRESS_QM_PKT2CPU_INTPRI_MAPr */
    RT_IF_ERR_GOTO_HANDLE(reg_read(unit, CYPRESS_QM_PKT2CPU_INTPRI_MAPr, &reg_val), errHandle, ret);

    val = newPri & 0x7;
    RT_IF_ERR_GOTO_HANDLE(reg_field_set(unit, CYPRESS_QM_PKT2CPU_INTPRI_MAPr, fieldPri[intPri], &val, &reg_val), errHandle, ret);
    RT_IF_ERR_GOTO_HANDLE(reg_write(unit, CYPRESS_QM_PKT2CPU_INTPRI_MAPr, &reg_val), errHandle, ret);

    QOS_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    QOS_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
    return ret;
}

/* Function Name:
 *      dal_cypress_qos_invldDscpEnable_get
 * Description:
 *      Get the invalid DSCP status in the specified device
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable   - status of invalid DSCP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_cypress_qos_invldDscpEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_PRI_SEL_CTRLr, CYPRESS_DSCP_INVLD_ENf, pEnable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable = %d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_invldDscpEnable_set
 * Description:
 *      Set the invalid dscp status in the specified device
 * Input:
 *      unit     - unit id
 *      enable   - status of invalid DSCP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 *      None.
 */
int32
dal_cypress_qos_invldDscpEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_PRI_SEL_CTRLr, CYPRESS_DSCP_INVLD_ENf, &enable)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_qos_invldDscpVal_get
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
dal_cypress_qos_invldDscpVal_get(uint32 unit, uint32 *pDscp)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    QOS_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_PRI_SEL_CTRLr, CYPRESS_DSCP_INVLD_VALf, pDscp)) != RT_ERR_OK)
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
 *      dal_cypress_qos_invldDscpVal_set
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
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - Invalid dscp value
 * Note:
 *      None.
 */
int32
dal_cypress_qos_invldDscpVal_set(uint32 unit, uint32 dscp)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d, dscp=%d", unit, dscp);

    /* check Init status */
    RT_INIT_CHK(qos_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_QOS_DSCP_VALUE);

    QOS_SEM_LOCK(unit);

    /* set value from CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_PRI_SEL_CTRLr, CYPRESS_DSCP_INVLD_VALf, &dscp)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_cypress_qos_init_config
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
static int32
_dal_cypress_qos_init_config(uint32 unit, uint32 queueNum)
{
    int32       ret, queue_num;
    rtk_port_t  port;
    rtk_pri_t   dot1p_pri;
    rtk_qid_t   qid;
    rtk_qos_pri2queue_t pri2Queue;
    rtk_qos_priSelWeight_t weightOfPriSel;
    rtk_qos_congAvoidThresh_t congAvoidThresh;
    rtk_pri_t   dot1pPriRemap[RTK_DOT1P_PRIORITY_MAX + 1] = {RTK_DEFAULT_QOS_1P_PRIORITY0_REMAP, RTK_DEFAULT_QOS_1P_PRIORITY1_REMAP,
                                                   RTK_DEFAULT_QOS_1P_PRIORITY2_REMAP, RTK_DEFAULT_QOS_1P_PRIORITY3_REMAP,
                                                   RTK_DEFAULT_QOS_1P_PRIORITY4_REMAP, RTK_DEFAULT_QOS_1P_PRIORITY5_REMAP,
                                                   RTK_DEFAULT_QOS_1P_PRIORITY6_REMAP, RTK_DEFAULT_QOS_1P_PRIORITY7_REMAP};

    rtk_qos_queue_weights_t queue_weights = {{RTK_DEFAULT_QOS_SCHED_QUEUE0_WEIGHT, RTK_DEFAULT_QOS_SCHED_QUEUE1_WEIGHT,
                                             RTK_DEFAULT_QOS_SCHED_QUEUE2_WEIGHT, RTK_DEFAULT_QOS_SCHED_QUEUE3_WEIGHT,
                                             RTK_DEFAULT_QOS_SCHED_QUEUE4_WEIGHT, RTK_DEFAULT_QOS_SCHED_QUEUE5_WEIGHT,
                                             RTK_DEFAULT_QOS_SCHED_QUEUE6_WEIGHT, RTK_DEFAULT_QOS_SCHED_QUEUE7_WEIGHT}};

    if ((ret = dal_cypress_qos_queueNum_set(unit, queueNum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_LOCK(unit);

    if ((ret = _dal_cypress_qos_cpuQueueNum_set(unit, RTK_DEFAULT_QOS_QUEUE_NUMBER_IN_CPU_PORT)) != RT_ERR_OK)
    {
        QOS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    QOS_SEM_UNLOCK(unit);

    weightOfPriSel.weight_of_dscp = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_DSCP;
    weightOfPriSel.weight_of_innerTag = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_ITAG;
    weightOfPriSel.weight_of_outerTag = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_OTAG;
    weightOfPriSel.weight_of_portBased = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_PORT;
    weightOfPriSel.weight_of_inAcl = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_ACL;
    weightOfPriSel.weight_of_macVlan = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_MAC_VLAN;
    weightOfPriSel.weight_of_protoVlan = RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_PROTO_VLAN;
    if ((ret = dal_cypress_qos_priSelGroup_set(unit, 0, &weightOfPriSel)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = dal_cypress_qos_portPri_set(unit, port, RTK_DEFAULT_QOS_PORT_PRIORITY)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        if ((ret = dal_cypress_qos_schedulingAlgorithm_set(unit, port, RTK_DEFAULT_QOS_SCHED_ALGORITHM)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        if ((ret = dal_cypress_qos_schedulingQueue_set(unit, port, &queue_weights)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        if ((ret = dal_cypress_qos_portDeiRemarkTagSel_set(unit, port, RTK_DEFAULT_QOS_REMARK_DEI_SOURCE)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

    }

    for (dot1p_pri = 0; dot1p_pri <= RTK_DOT1P_PRIORITY_MAX; dot1p_pri++)
    {
        if ((ret = dal_cypress_qos_1pPriRemap_set(unit, dot1p_pri, dot1pPriRemap[dot1p_pri])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    for (queue_num = HAL_MIN_NUM_OF_QUEUE(unit); queue_num < HAL_MAX_NUM_OF_QUEUE(unit); queue_num++)
    {
        osal_memcpy(&pri2Queue, &(qos_scheduling_priorityToQueue[queue_num]), sizeof(pri2Queue));
        if ((ret = dal_cypress_qos_priMap_set(unit, queue_num, &pri2Queue)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    if ((ret = dal_cypress_qos_deiDpRemap_set(unit, 0, RTK_DEFAULT_QOS_REMAP_DEI0_TO_DP)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = dal_cypress_qos_deiDpRemap_set(unit, 1, RTK_DEFAULT_QOS_REMAP_DEI1_TO_DP)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = dal_cypress_qos_deiRemark_set(unit, 0, RTK_DEFAULT_QOS_REMARK_DP0_TO_DEI)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = dal_cypress_qos_deiRemark_set(unit, 1, RTK_DEFAULT_QOS_REMARK_DP1_TO_DEI)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    if ((ret = dal_cypress_qos_deiRemark_set(unit, 2, RTK_DEFAULT_QOS_REMARK_DP2_TO_DEI)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /* SWRED threshold */
    congAvoidThresh.maxThresh = QOS_SWRED_SYSTEM_DP0_HIGH_THRESH;
    congAvoidThresh.minThresh = QOS_SWRED_SYSTEM_DP0_LOW_THRESH;
    if ((ret = dal_cypress_qos_congAvoidSysThresh_set(unit, 0, &congAvoidThresh)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = dal_cypress_qos_congAvoidSysDropProbability_set(unit, 0, QOS_SWRED_SYSTEM_DP0_DROP_RATE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    congAvoidThresh.maxThresh = QOS_SWRED_SYSTEM_DP1_HIGH_THRESH;
    congAvoidThresh.minThresh = QOS_SWRED_SYSTEM_DP1_LOW_THRESH;
    if ((ret = dal_cypress_qos_congAvoidSysThresh_set(unit, 1, &congAvoidThresh)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = dal_cypress_qos_congAvoidSysDropProbability_set(unit, 1, QOS_SWRED_SYSTEM_DP1_DROP_RATE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    congAvoidThresh.maxThresh = QOS_SWRED_SYSTEM_DP2_HIGH_THRESH;
    congAvoidThresh.minThresh = QOS_SWRED_SYSTEM_DP2_LOW_THRESH;
    if ((ret = dal_cypress_qos_congAvoidSysThresh_set(unit, 2, &congAvoidThresh)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    if ((ret = dal_cypress_qos_congAvoidSysDropProbability_set(unit, 2, QOS_SWRED_SYSTEM_DP2_DROP_RATE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    for (qid = 0; qid <= (HAL_MAX_NUM_OF_QUEUE(unit) - 1); qid++)
    {
        congAvoidThresh.maxThresh = QOS_SWRED_QUEUE_DP0_HIGH_THRESH;
        congAvoidThresh.minThresh = QOS_SWRED_QUEUE_DP0_LOW_THRESH;
        if ((ret = dal_cypress_qos_congAvoidGlobalQueueThresh_set(unit, qid, 0, &congAvoidThresh)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        if ((ret = dal_cypress_qos_congAvoidGlobalQueueDropProbability_set(unit, qid, 0, QOS_SWRED_QUEUE_DP0_DROP_RATE)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        congAvoidThresh.maxThresh = QOS_SWRED_QUEUE_DP1_HIGH_THRESH;
        congAvoidThresh.minThresh = QOS_SWRED_QUEUE_DP1_LOW_THRESH;
        if ((ret = dal_cypress_qos_congAvoidGlobalQueueThresh_set(unit, qid, 1, &congAvoidThresh)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        if ((ret = dal_cypress_qos_congAvoidGlobalQueueDropProbability_set(unit, qid, 1, QOS_SWRED_QUEUE_DP1_DROP_RATE)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }

        congAvoidThresh.maxThresh = QOS_SWRED_QUEUE_DP2_HIGH_THRESH;
        congAvoidThresh.minThresh = QOS_SWRED_QUEUE_DP2_LOW_THRESH;
        if ((ret = dal_cypress_qos_congAvoidGlobalQueueThresh_set(unit, qid, 2, &congAvoidThresh)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        if ((ret = dal_cypress_qos_congAvoidGlobalQueueDropProbability_set(unit, qid, 2, QOS_SWRED_QUEUE_DP2_DROP_RATE)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of _dal_cypress_qos_init_config */

/* Function Name:
 *      _dal_cypress_qos_outputQueueNumber_get
 * Description:
 *      get queue number for port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      pQnum - queue number
 * Output:
 *      pQnum - queue number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_QUEUE_NUM
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_FAILED
 * Note:
 */
static int32
_dal_cypress_qos_outputQueueNumber_get(uint32 unit, rtk_port_t port, uint32 *pQNum)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_TRACE, (MOD_DAL|MOD_QOS), "unit=%d, port=%d",
           unit, port);

    if ((ret = reg_array_field_read(unit, CYPRESS_QM_PORT_QNUMr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_QNUMf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    *pQNum = val + 1;

    RT_LOG(LOG_TRACE, (MOD_DAL|MOD_QOS), "pQNum=%d", *pQNum);

    return RT_ERR_OK;
} /* end of _dal_cypress_qos_outputQueueNumber_get */


/* Function Name:
 *      _dal_cypress_qos_outputQueueNumber_set
 * Description:
 *      Set queue number for port
 * Input:
 *      unit - unit id
 *      port - port id
 *      qnum - queue number
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
static int32
_dal_cypress_qos_outputQueueNumber_set(uint32 unit, rtk_port_t port, uint32 qNum)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_TRACE, (MOD_DAL|MOD_QOS), "unit=%d, port=%d, qNum=%d",
           unit, port, qNum);

    val = qNum - 1;
    if ((ret = reg_array_field_write(unit, CYPRESS_QM_PORT_QNUMr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_QNUMf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_cypress_qos_outputQueueNumber_set */

static int32
_dal_cypress_qos_cpuQueueNum_set(uint32 unit, uint32 queue_num)
{
    int32       ret;
    rtk_port_t  cpuPort;

    RT_LOG(LOG_TRACE, (MOD_DAL|MOD_QOS), "unit=%d, queue_num=%d", unit, queue_num);

    /* parameter check */
    RT_PARAM_CHK(queue_num > HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_QUEUE_NUM);

    cpuPort = HWP_CPU_MACID(unit);

    if ((ret = _dal_cypress_qos_outputQueueNumber_set(unit, cpuPort, queue_num)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_cypress_qos_cpuQueueNum_set */

