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
 * Purpose : Definition of QoS API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Ingress Priority Decision
 *           (2) Egress Remarking
 *           (3) Queue Scheduling
 *           (4) Congestion avoidance
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/init.h> 
#include <rtk/default.h> 
#include <rtk/qos.h> 
#include <dal/dal_mgmt.h> 

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
 *      rtk_qos_init
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
 *      - CPU port init 8 using prioroty to queue mapping index 0
 *      - Other port init 1 queue using prioroty to queue mapping index 1
 */
int32
rtk_qos_init(void)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->qos_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_init */


/* Function Name:
 *      rtk_qos_priSelGroup_get
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
int32
rtk_qos_priSelGroup_get(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_priSelGroup_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_priSelGroup_get( grpIdx, pWeightOfPriSel);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_priSelGroup_get */

/* Function Name:
 *      rtk_qos_priSelGroup_set
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
int32
rtk_qos_priSelGroup_set(uint32 grpIdx, rtk_qos_priSelWeight_t *pWeightOfPriSel)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_priSelGroup_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_priSelGroup_set( grpIdx, pWeightOfPriSel);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_priSelGroup_set */

/* Function Name:
 *      rtk_qos_portPri_get
 * Description:
 *      Get internal priority of one port.
 * Input:
 *      port     - port id
 * Output:
 *      pIntPri  - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    None
 */
int32
rtk_qos_portPri_get(rtk_port_t port, rtk_pri_t *pIntPri)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_portPri_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_portPri_get( port, pIntPri);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_portPri_get */


/* Function Name:
 *      rtk_qos_portPri_set
 * Description:
 *      Get internal priority of one port.
 * Input:
 *      port     - port id
 *      intPri   - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
 *                 the highest prioirty)
 * Output:
 *    None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    None
 */
int32
rtk_qos_portPri_set(rtk_port_t port, rtk_pri_t intPri)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_portPri_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_portPri_set(port, intPri);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_portPri_set */


/* Function Name:
 *      rtk_qos_dscpPriRemapGroup_get
 * Description:
 *      Get remapped internal priority of DSCP on specified DSCP remapping group.
 * Input:
 *      unit     - unit id
 *      grpIdx   - index of dscp remapping group
 *      dscp     - DSCP
 * Output:
 *      pIntPri  - pointer to internal priority
 *      pDp      - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_QOS_DSCP_VALUE - invalid DSCP value
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 *      Apollo only support group 0
 */
int32
rtk_qos_dscpPriRemapGroup_get(
    uint32      grpIdx,
    uint32      dscp,
    rtk_pri_t   *pIntPri,
    uint32      *pDp)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_dscpPriRemapGroup_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_dscpPriRemapGroup_get( grpIdx, dscp, pIntPri, pDp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_dscpPriRemapGroup_get */

/* Function Name:
 *      rtk_qos_dscpPriRemapGroup_set
 * Description:
 *      Set remapped internal priority of DSCP on specified DSCP remapping group.
 * Input:
 *      grpIdx  - index of dscp remapping group
 *      dscp    - DSCP
 *      intPri - internal priority
 *      dp      - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_DSCP_VALUE   - invalid DSCP value
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Apollo only support group 0
 */
int32
rtk_qos_dscpPriRemapGroup_set(
    uint32      grpIdx,
    uint32      dscp,
    rtk_pri_t   intPri,
    uint32      dp)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_dscpPriRemapGroup_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_dscpPriRemapGroup_set( grpIdx, dscp, intPri, dp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_dscpPriRemapGroup_set */

/* Function Name:
 *      rtk_qos_1pPriRemapGroup_get
 * Description:
 *      Get remapped internal priority of dot1p priority on specified dot1p priority remapping group.
 * Input:
 *      unit      - unit id
 *      grpIdx   - index of outer dot1p remapping group
 *      dot1pPri - dot1p priority
 * Output:
 *      pIntPri  - pointer to internal priority
 *      pDp       - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY - invalid dot1p priority
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      Apollo only support group 0
 */
int32
rtk_qos_1pPriRemapGroup_get(
    uint32      grpIdx,
    rtk_pri_t   dot1pPri,
    rtk_pri_t   *pIntPri,
    uint32      *pDp)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_1pPriRemapGroup_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_1pPriRemapGroup_get( grpIdx, dot1pPri, pIntPri, pDp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_1pPriRemapGroup_get */

/* Function Name:
 *      rtk_qos_1pPriRemapGroup_set
 * Description:
 *      Set remapped internal priority of dot1p priority on specified dot1p priority remapping group.
 * Input:
 *      grpIdx   - index of dot1p remapping group
 *      dot1pPri - dot1p priority
 *      intPri   - internal priority
 *      dp        - drop precedence
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY  - invalid dot1p priority
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_qos_1pPriRemapGroup_set(
    uint32      grpIdx,
    rtk_pri_t   dot1pPri,
    rtk_pri_t   intPri,
    uint32      dp)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_1pPriRemapGroup_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_1pPriRemapGroup_set( grpIdx, dot1pPri, intPri, dp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_1pPriRemapGroup_set */



/* Function Name:
 *      rtk_qos_priMap_set
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
int32
rtk_qos_priMap_set(uint32 group, rtk_qos_pri2queue_t *pPri2qid)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_priMap_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_priMap_set( group, pPri2qid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_priMap_set */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_qos_priMap_get
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
int32
rtk_qos_priMap_get(uint32 group, rtk_qos_pri2queue_t *pPri2qid)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_priMap_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_priMap_get( group, pPri2qid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_priMap_get */


/* Function Name:
 *      rtk_qos_portPriMap_get
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
int32 
rtk_qos_portPriMap_get(rtk_port_t port, uint32 *pGroup)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_portPriMap_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_portPriMap_get( port, pGroup);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_portPriMap_get */

/* Function Name:
 *      rtk_qos_portPriMap_set
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
int32
rtk_qos_portPriMap_set(rtk_port_t port, uint32 group)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_portPriMap_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_portPriMap_set( port, group);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_portPriMap_set */

/* Module Name    : QoS           */
/* Sub-module Name: Egress remark */
/* Function Name:
 *      rtk_qos_1pRemarkEnable_get
 * Description:
 *      Get 802.1p remark status for a port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status of 802.1p remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_qos_1pRemarkEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_1pRemarkEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_1pRemarkEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_1pRemarkEnable_get */

/* Function Name:
 *      rtk_qos_1pRemarkEnable_set
 * Description:
 *      Set 802.1p remark status for a port
 * Input:
 *      port   - port id.
 *      enable - status of 802.1p remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 * Note:
 *      The status of 802.1p remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_qos_1pRemarkEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_1pRemarkEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_1pRemarkEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_1pRemarkEnable_set */

/* Function Name:
 *      rtk_qos_1pRemarkGroup_get
 * Description:
 *      Get remarked dot1p priority of internal priority on specified dot1p remark group.
 * Input:
 *      grpIdx    - index of dot1p remark group
 *      intPri    - internal priority
 *      dp         - drop precedence
 * Output:
 *      pDot1pPri - pointer to dot1p priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_qos_1pRemarkGroup_get(
    uint32      grpIdx,
    rtk_pri_t   intPri,
    uint32      dp,
    rtk_pri_t   *pDot1pPri)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_1pRemarkGroup_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_1pRemarkGroup_get( grpIdx, intPri, dp, pDot1pPri);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_1pRemarkGroup_get */

/* Function Name:
 *      rtk_qos_1pRemarkGroup_set
 * Description:
 *      Set remarked dot1p priority of internal priority on specified dot1p remark group.
 * Input:
 *      grpIdx   - index of dot1p remark group
 *      intPri   - internal priority
 *      dp       - drop precedence
 *      dot1pPri - dot1p priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_1P_PRIORITY  - invalid dot1p priority
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_qos_1pRemarkGroup_set(
    uint32 grpIdx,
    rtk_pri_t intPri,
    uint32 dp,
    rtk_pri_t dot1pPri)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_1pRemarkGroup_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_1pRemarkGroup_set( grpIdx, intPri, dp, dot1pPri);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_1pRemarkGroup_set */


/* Function Name:
 *      rtk_qos_dscpRemarkEnable_get
 * Description:
 *      Get DSCP remark status for a port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status of DSCP remark
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_qos_dscpRemarkEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_dscpRemarkEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_dscpRemarkEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_dscpRemarkEnable_get */

/* Function Name:
 *      rtk_qos_dscpRemarkEnable_set
 * Description:
 *      Set DSCP remark status for a port
 * Input:
 *      port   - port id
 *      enable - status of DSCP remark
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 * Note:
 *      The status of DSCP remark:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_qos_dscpRemarkEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_dscpRemarkEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_dscpRemarkEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_dscpRemarkEnable_set */


/* Function Name:
 *      rtk_qos_dscpRemarkGroup_get
 * Description:
 *      Get remarked DSCP of internal priority on specified dscp remark group.
 * Input:
 *      grpIdx - index of dot1p remapping group
 *      intPri - internal priority
 *      dp      - drop precedence
 * Output:
 *      pDscp   - pointer to DSCP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      (1) The valid range of grp_idx is 0 for apollo
 */
int32
rtk_qos_dscpRemarkGroup_get(
    uint32      grpIdx,
    rtk_pri_t   intPri,
    uint32      dp,
    uint32      *pDscp)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_dscpRemarkGroup_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_dscpRemarkGroup_get( grpIdx, intPri, dp, pDscp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_dscpRemarkGroup_get */

/* Function Name:
 *      rtk_qos_dscpRemarkGroup_set
 * Description:
 *      Set remarked DSCP of internal priority on specified dscp remark group.
 * Input:
 *      grpIdx - index of dot1p remapping group
 *      intPri - internal priority
 *      dp      - drop precedence
 *      dscp    - DSCP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_QOS_DSCP_VALUE   - invalid DSCP value
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      (1) The valid range of grp_idx is 0 for apollo
 */
int32
rtk_qos_dscpRemarkGroup_set(
    uint32      grpIdx,
    rtk_pri_t   intPri,
    uint32      dp,
    uint32      dscp)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_dscpRemarkGroup_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_dscpRemarkGroup_set( grpIdx, intPri, dp, dscp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_dscpRemarkGroup_set */


/* Function Name:
 *      rtk_qos_portDscpRemarkSrcSel_get
 * Description:
 *      Get remarking source of DSCP remarking.
 * Input:
 *      port                 - port id
 * Output:
 *      pType      - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer

 * Note:
 *      None
 */
int32
rtk_qos_portDscpRemarkSrcSel_get(rtk_port_t port, rtk_qos_dscpRmkSrc_t *pType)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_portDscpRemarkSrcSel_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_portDscpRemarkSrcSel_get( port, pType);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_dscpRemarkSrcSel_get */

/* Function Name:
 *      rtk_qos_dscpRemarkSrcSel_set
 * Description:
 *      Set remarking source of DSCP remarking.
 * Input:
 *      port                 - port id
 *      type      - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      The API can configure DSCP remark functionality to map original DSCP value or internal
 *      priority to TX DSCP value.
 */
int32
rtk_qos_portDscpRemarkSrcSel_set(rtk_port_t port, rtk_qos_dscpRmkSrc_t type)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_portDscpRemarkSrcSel_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_portDscpRemarkSrcSel_set( port, type);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_portDscpRemarkSrcSel_set */



/* Function Name:
 *      rtk_qos_dscp2DscpRemarkGroup_get
 * Description:
 *      Get DSCP to remarked DSCP mapping.
 * Input:
 *      dscp    - DSCP value
 *      grpIdx  - group index
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
rtk_qos_dscp2DscpRemarkGroup_get(uint32 grpIdx, uint32 dscp, uint32 *pDscp)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_dscp2DscpRemarkGroup_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_dscp2DscpRemarkGroup_get(grpIdx, dscp, pDscp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_dscp2DscpRemark_get */


/* Function Name:
 *      rtk_qos_dscp2DscpRemarkGroup_set
 * Description:
 *      Set DSCP to remarked DSCP mapping.
 * Input:
 *      grpIdx  - group index
 *      dscp    - DSCP value
 *      rmkDscp - remarked DSCP value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 * Note:
 *      dscp parameter can be DSCP value or internal priority according to configuration of API 
 *      dal_apollo_qos_dscpRemarkSrcSel_set(), because DSCP remark functionality can map original DSCP 
 *      value or internal priority to TX DSCP value.
 */
int32
rtk_qos_dscp2DscpRemarkGroup_set(uint32 grpIdx,uint32 dscp, uint32 rmkDscp)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_dscp2DscpRemarkGroup_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_dscp2DscpRemarkGroup_set(grpIdx, dscp, rmkDscp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_dscp2DscpRemark_set */




/* Function Name:
 *      rtk_qos_fwd2CpuPriRemap_get
 * Description:
 *      Get forward to CPU port remapped priority for internal priority.
 * Input:
 *      intPri  -  internal priority
 * Output:
 *      pRempPri  - pointer to remapping priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 */
int32
rtk_qos_fwd2CpuPriRemap_get(rtk_pri_t   intPri,
                            rtk_pri_t   *pRempPri)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_fwd2CpuPriRemap_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_fwd2CpuPriRemap_get(intPri, pRempPri);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_fwd2CpuPriRemap_get */


/* Function Name:
 *      rtk_qos_fwd2CpuPriRemap_set
 * Description:
 *      Set remapped internal priority of DSCP on specified DSCP remapping group.
 * Input:
 *      intPri  -  internal priority
 *      rempPri - remapping priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - invalid internal priority
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 */
int32
rtk_qos_fwd2CpuPriRemap_set(rtk_pri_t intPri,rtk_pri_t rempPri)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_fwd2CpuPriRemap_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_fwd2CpuPriRemap_set(intPri, rempPri);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_fwd2CpuPriRemap_set */


/* Module Name    : QoS              */
/* Sub-module Name: Queue scheduling */

/* Function Name:
 *      rtk_qos_schedulingQueue_get
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
int32
rtk_qos_schedulingQueue_get(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_schedulingQueue_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_schedulingQueue_get( port, pQweights);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_schedulingQueue_get */

/* Function Name:
 *      rtk_qos_schedulingQueue_set
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
int32
rtk_qos_schedulingQueue_set(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    int32   ret;
    if (NULL == RT_MAPPER->qos_schedulingQueue_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_schedulingQueue_set( port, pQweights);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_qos_schedulingQueue_set */



/* Function Name:
 *      rtk_qos_portPriSelGroup_get
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
int32
rtk_qos_portPriSelGroup_get(rtk_port_t port, uint32 *pPriSelGrpIdx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->qos_portPriSelGroup_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_portPriSelGroup_get(port, pPriSelGrpIdx);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_qos_portPriSelGroup_get */

/* Function Name:
 *      rtk_qos_portPriSelGroup_set
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
int32
rtk_qos_portPriSelGroup_set(rtk_port_t port, uint32 priSelGrpIdx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->qos_portPriSelGroup_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_portPriSelGroup_set(port, priSelGrpIdx);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_qos_portPriSelGroup_set */


/* Function Name:
 *      rtk_qos_schedulingType_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      pQtype    - the WFQ schedule type (RTK_QOS_WFQ or RTK_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_qos_schedulingType_get(rtk_qos_scheduling_type_t *pQtype)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->qos_schedulingType_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_schedulingType_get(pQtype);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_qos_schedulingType_get */

/* Function Name:
 *      rtk_qos_schedulingType_set
 * Description:
 *      Set the scheduling type.
 * Input:
 *      queueType - the WFQ schedule type (RTK_QOS_WFQ or RTK_QOS_WRR)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_qos_schedulingType_set(rtk_qos_scheduling_type_t queueType)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->qos_schedulingType_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_schedulingType_set(queueType);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_qos_schedulingType_set */


/* Function Name:
 *      rtk_qos_portDot1pRemarkSrcSel_get
 * Description:
 *      Get remarking source of dot1p remarking.
 * Input:
 *      port                 - port id
 * Output:
 *      pType      - remarking source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer

 * Note:
 *      None
 */
int32
rtk_qos_portDot1pRemarkSrcSel_get(rtk_port_t port, rtk_qos_do1pRmkSrc_t *pType)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->qos_portDot1pRemarkSrcSel_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_portDot1pRemarkSrcSel_get(port, pType);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_qos_portDot1pRemarkSrcSel_get */

/* Function Name:
 *      rtk_qos_portDot1pRemarkSrcSel_set
 * Description:
 *      Set remarking source of dot1p remarking.
 * Input:
 *      port                 - port id
 *      type      - remarking source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter

 * Note:
 *      The API can configure dot1p remark source.
 */
int32
rtk_qos_portDot1pRemarkSrcSel_set(rtk_port_t port, rtk_qos_do1pRmkSrc_t type)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->qos_portDot1pRemarkSrcSel_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_portDot1pRemarkSrcSel_set(port, type);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_qos_portDot1pRemarkSrcSel_set */

