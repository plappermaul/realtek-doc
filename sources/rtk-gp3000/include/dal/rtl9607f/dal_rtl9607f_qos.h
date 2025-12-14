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

#ifndef __DAL_RTL9607F_QOS_H__
#define __DAL_RTL9607F_QOS_H__
/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/qos.h>
#include <rtk/rt/rt_qos.h>

/*
 * Symbol Definition
 */
/****************************************************************/
/* Constant Definition											*/
/****************************************************************/

#define RTL9607F_QOS_INGRESS_QOS_RESERVED_NUM 16

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
 *      dal_rtl9607f_qos_init
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
dal_rtl9607f_qos_init(void);

/* Function Name:
 *      dal_rtl9607f_qos_priSelGroup_get
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
extern int32
dal_rtl9607f_qos_priSelGroup_get(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel);

/* Function Name:
 *      dal_rtl9607f_qos_priSelGroup_set
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
extern int32
dal_rtl9607f_qos_priSelGroup_set(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel);

/* Function Name:
 *      dal_rtl9607f_qos_portPriSelGroup_get
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
extern int32
dal_rtl9607f_qos_portPriSelGroup_get(rtk_port_t port, uint32 *pPriSelGrpIdx);

/* Function Name:
 *      dal_rtl9607f_qos_portPriSelGroup_set
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
extern int32
dal_rtl9607f_qos_portPriSelGroup_set(rtk_port_t port, uint32 priSelGrpIdx);

/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl9607f_qos_priMap_get
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
extern int32
dal_rtl9607f_qos_priMap_get(uint32 group, rtk_qos_pri2queue_t *pPri2qid);

/* Function Name:
 *      dal_rtl9607f_qos_priMap_set
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
extern int32
dal_rtl9607f_qos_priMap_set(uint32 group, rtk_qos_pri2queue_t *pPri2qid);

/* Function Name:
 *      dal_rtl9607f_qos_portPriMap_get
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
extern int32
dal_rtl9607f_qos_portPriMap_get(rtk_port_t port, uint32 *pGroup);

/* Function Name:
 *      dal_rtl9607f_qos_portPriMap_set
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
extern int32
dal_rtl9607f_qos_portPriMap_set(rtk_port_t port, uint32 group);

/* Function Name:
 *      dal_rtl9607f_qos_schedulingQueue_get
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
dal_rtl9607f_qos_schedulingQueue_get(rtk_port_t port, rtk_qos_queue_weights_t *pQweights);

/* Function Name:
 *      dal_rtl9607f_qos_schedulingQueue_set
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
dal_rtl9607f_qos_schedulingQueue_set(rtk_port_t port, rtk_qos_queue_weights_t *pQweights);

/* Function Name:
 *      dal_rtl9607f_qos_dscpPriRemapGroup_get
 * Description:
 *      Get remapped COS of DSCP in upstream direction
 * Input:
 *      grpIdx   - Not in use
 *      dscp     - DSCP
 * Output:
 *      pPri     - pointer to priority bit
 *      pDp      - Not in use
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - invalid DSCP value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 *      8277 serial only support us/ds remapping table
 */
extern int32
dal_rtl9607f_qos_dscpPriRemapGroup_get(uint32 grpIdx, uint32 dscp, rtk_pri_t *pPri, uint32 *pDp);

/* Function Name:
 *      dal_rtl9607f_qos_dscpPriRemapGroup_set
 * Description:
 *      Set remapped COS of DSCP in upstream direction
 * Input:
 *      grpIdx   - Not in use
 *      dscp     - DSCP
 * Output:
 *      pPri     - pointer to priority bit
 *      pDp      - Not in use
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE - invalid DSCP value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 *      8277 serial only support us/ds remapping table
 */
extern int32
dal_rtl9607f_qos_dscpPriRemapGroup_set(uint32 grpIdx, uint32 dscp, rtk_pri_t pri, uint32 dp);

/* Function Name:
 *      dal_rtl9607f_qos_ingress_qos_set
 * Description:
 *      Set the ingress qos setting for HGU mode only
 * Input:
 *      index   - index of qos setting 0~15
 *      pIgrQos - Ingress qos setting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      
 *      
 */
extern int32
dal_rtl9607f_qos_ingress_qos_set(uint8 index, rt_qos_ingress_qos_t *pIgrQos);

/* Function Name:
 *      dal_rtl9607f_qos_ingress_qos_del
 * Description:
 *      Delete the ingress qos setting
 * Input:
 *      index   - index of qos setting 0~15
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED         - No such qos setting
 * Note:
 *      
 *      
 */
extern int32
dal_rtl9607f_qos_ingress_qos_del(uint8 index);

/* Function Name:
 *      dal_rtl9607f_qos_ingress_qos_get
 * Description:
 *      Get the ingress qos setting
 * Input:
 *      index   - index of qos setting 0~15
 * Output:
 *      pIgrQos - Ingress qos setting
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED         - No such qos setting
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      
 *      
 */
extern int32
dal_rtl9607f_qos_ingress_qos_get(uint8 index, rt_qos_ingress_qos_t *pIgrQos);

#endif /* __DAL_RTL9607F_QOS_H__ */
