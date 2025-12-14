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

#ifndef __DAL_RTL8198F_QOS_H__
#define __DAL_RTL8198F_QOS_H__
/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/qos.h>

/*
 * Symbol Definition
 */
/****************************************************************/
/* Constant Definition											*/
/****************************************************************/


/****************************************************************/
/* Type Definition												*/
/****************************************************************/

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl8198f_qos_init
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
extern int32
dal_rtl8198f_qos_init(void);

/* Function Name:
 *      dal_rtl8198f_qos_schedulingQueue_get
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
extern int32
dal_rtl8198f_qos_schedulingQueue_get(rtk_port_t port, rtk_qos_queue_weights_t *pQweights);

/* Function Name:
 *      dal_rtl8198f_qos_schedulingQueue_set
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
extern int32
dal_rtl8198f_qos_schedulingQueue_set(rtk_port_t port, rtk_qos_queue_weights_t *pQweights);


#if defined(CONFIG_RTL_83XX_SUPPORT)
extern int32 dal_rtl8198f_qos_priSelGroup_get(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel);
extern int32 dal_rtl8198f_qos_priSelGroup_set(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel);
extern int32 dal_rtl8198f_qos_priMap_get(uint32 group, rtk_qos_pri2queue_t *pPri2qid);
extern int32 dal_rtl8198f_qos_priMap_set(uint32 group, rtk_qos_pri2queue_t *pPri2qid);
extern int32 dal_rtl8198f_qos_1pPriRemap_get(uint32 grpIdx, uint32 dot1pPri, uint32 *pIntPri, uint32 *pDp);
extern int32 dal_rtl8198f_qos_1pPriRemap_set(uint32 grpIdx, uint32 dot1pPri, uint32 intPri, uint32 dp);
extern int32 dal_rtl8198f_qos_1pRemark_get(uint32 grpIdx, uint32 intPri, uint32 dp, uint32 *pDot1pPri);
extern int32 dal_rtl8198f_qos_1pRemark_set(uint32 grpIdx, uint32 intPri, uint32 dp, uint32 dot1pPri);
extern int32 dal_rtl8198f_qos_dscpRemark_get(uint32 grpIdx, uint32 intPri, uint32 dp, uint32 *pDscp);
extern int32 dal_rtl8198f_qos_dscpRemark_set(uint32 grpIdx, uint32 intPri, uint32 dp, uint32 dscp);
extern int32 dal_rtl8198f_qos_1pRemarkEnable_get(uint32 port, uint32 *pEnable);
extern int32 dal_rtl8198f_qos_1pRemarkEnable_set(uint32 port, uint32 enable);
extern int32 dal_rtl8198f_qos_dscpRemarkEnable_get(uint32 port, uint32 *pEnable);
extern int32 dal_rtl8198f_qos_dscpRemarkEnable_set(uint32 port, uint32 enable);
extern int32 rtk_83xx_setFlowControl(uint32 qosEnable);
#endif
#endif /* __DAL_RTL8198F_QOS_H__ */
