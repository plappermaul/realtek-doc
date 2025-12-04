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
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <rtk/port.h>

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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    SETSOCKOPT(RTDRV_QOS_INIT, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_init */


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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    GETSOCKOPT(RTDRV_QOS_PRISELGROUP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pWeightOfPriSel, &qos_cfg.weightOfPriSel, sizeof(rtk_qos_priSelWeight_t));

    return RT_ERR_OK;
}   /* end of rtk_qos_priSelGroup_get */

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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.weightOfPriSel, pWeightOfPriSel, sizeof(rtk_qos_priSelWeight_t));
    SETSOCKOPT(RTDRV_QOS_PRISELGROUP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_priSelGroup_set */

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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntPri), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_QOS_PORTPRI_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pIntPri, &qos_cfg.intPri, sizeof(rtk_pri_t));

    return RT_ERR_OK;
}   /* end of rtk_qos_portPri_get */


/* Function Name:
 *      rtk_qos_portPri_set
 * Description:
 *      Get internal priority of one port.
 * Input:
 *      port     - port id
 *      intPri  - Priorities assigment for specific port. (range from 0 ~ 7, 7 is
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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&qos_cfg.intPri, &intPri, sizeof(rtk_pri_t));
    SETSOCKOPT(RTDRV_QOS_PORTPRI_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_portPri_set */


/* Function Name:
 *      rtk_qos_dscpPriRemapGroup_get
 * Description:
 *      Get remapped internal priority of DSCP on specified DSCP remapping group.
 * Input:
 *      grpIdx   - index of dscp remapping group
 *      dscp     - DSCP
 * Output:
 *      pIntPri  - pointer to internal priority
 *      pDp      - pointer to drop precedence
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntPri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDp), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.dscp, &dscp, sizeof(uint32));
    GETSOCKOPT(RTDRV_QOS_DSCPPRIREMAPGROUP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pIntPri, &qos_cfg.intPri, sizeof(rtk_pri_t));
    osal_memcpy(pDp, &qos_cfg.dp, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_qos_dscpPriRemapGroup_get */

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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.dscp, &dscp, sizeof(uint32));
    osal_memcpy(&qos_cfg.intPri, &intPri, sizeof(rtk_pri_t));
    osal_memcpy(&qos_cfg.dp, &dp, sizeof(uint32));
    SETSOCKOPT(RTDRV_QOS_DSCPPRIREMAPGROUP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_dscpPriRemapGroup_set */

/* Function Name:
 *      rtk_qos_1pPriRemapGroup_get
 * Description:
 *      Get remapped internal priority of dot1p priority on specified dot1p priority remapping group.
 * Input:
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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntPri), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDp), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.dot1pPri, &dot1pPri, sizeof(rtk_pri_t));
    GETSOCKOPT(RTDRV_QOS_1PPRIREMAPGROUP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pIntPri, &qos_cfg.intPri, sizeof(rtk_pri_t));
    osal_memcpy(pDp, &qos_cfg.dp, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_qos_1pPriRemapGroup_get */

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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.dot1pPri, &dot1pPri, sizeof(rtk_pri_t));
    osal_memcpy(&qos_cfg.intPri, &intPri, sizeof(rtk_pri_t));
    osal_memcpy(&qos_cfg.dp, &dp, sizeof(uint32));
    SETSOCKOPT(RTDRV_QOS_1PPRIREMAPGROUP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_1pPriRemapGroup_set */


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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.group, &group, sizeof(uint32));
    GETSOCKOPT(RTDRV_QOS_PRIMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pPri2qid, &qos_cfg.pri2qid, sizeof(rtk_qos_pri2queue_t));

    return RT_ERR_OK;
}   /* end of rtk_qos_priMap_get */


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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.group, &group, sizeof(uint32));
    osal_memcpy(&qos_cfg.pri2qid, pPri2qid, sizeof(rtk_qos_pri2queue_t));
    SETSOCKOPT(RTDRV_QOS_PRIMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_priMap_set */


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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_QOS_PORTPRIMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pGroup, &qos_cfg.group, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_qos_portPriMap_get */

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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&qos_cfg.group, &group, sizeof(uint32));
    SETSOCKOPT(RTDRV_QOS_PORTPRIMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_portPriMap_set */

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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_QOS_1PREMARKENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pEnable, &qos_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_qos_1pRemarkEnable_get */

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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&qos_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_QOS_1PREMARKENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_1pRemarkEnable_set */

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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDot1pPri), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.intPri, &intPri, sizeof(rtk_pri_t));
    osal_memcpy(&qos_cfg.dp, &dp, sizeof(uint32));
    GETSOCKOPT(RTDRV_QOS_1PREMARKGROUP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pDot1pPri, &qos_cfg.dot1pPri, sizeof(rtk_pri_t));

    return RT_ERR_OK;
}   /* end of rtk_qos_1pRemarkGroup_get */

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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.intPri, &intPri, sizeof(rtk_pri_t));
    osal_memcpy(&qos_cfg.dp, &dp, sizeof(uint32));
    osal_memcpy(&qos_cfg.dot1pPri, &dot1pPri, sizeof(rtk_pri_t));
    SETSOCKOPT(RTDRV_QOS_1PREMARKGROUP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_1pRemarkGroup_set */


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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_QOS_DSCPREMARKENABLE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pEnable, &qos_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_qos_dscpRemarkEnable_get */

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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&qos_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_QOS_DSCPREMARKENABLE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_dscpRemarkEnable_set */


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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.intPri, &intPri, sizeof(rtk_pri_t));
    osal_memcpy(&qos_cfg.dp, &dp, sizeof(uint32));
    GETSOCKOPT(RTDRV_QOS_DSCPREMARKGROUP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pDscp, &qos_cfg.dscp, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_qos_dscpRemarkGroup_get */

/* Function Name:
 *      rtk_qos_dscpRemarkGroup_set
 * Description:
 *      Set remarked DSCP of internal priority on specified dscp remark group.
 * Input:
 *      grpIdx - index of dot1p remarking group
 *      intPri - internal priority value (range from 0 ~ 7)
 *      dp      - drop precedence
 *      dscp    - DSCP
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
 *      (1) The valid range of grp_idx is 0 for apollo
 *      (2) dp(drop precedence) is not implement in Apollo  
 */
int32
rtk_qos_dscpRemarkGroup_set(
    uint32      grpIdx,
    rtk_pri_t   intPri,
    uint32      dp,
    uint32      dscp)
{
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.intPri, &intPri, sizeof(rtk_pri_t));
    osal_memcpy(&qos_cfg.dp, &dp, sizeof(uint32));
    osal_memcpy(&qos_cfg.dscp, &dscp, sizeof(uint32));
    SETSOCKOPT(RTDRV_QOS_DSCPREMARKGROUP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_dscpRemarkGroup_set */



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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_QOS_PORTDSCPREMARKSRCSEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pType, &qos_cfg.type, sizeof(rtk_qos_dscpRmkSrc_t));

    return RT_ERR_OK;
}   /* end of rtk_qos_portDscpRemarkSrcSel_get */

/* Function Name:
 *      rtk_qos_portDscpRemarkSrcSel_set
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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&qos_cfg.type, &type, sizeof(rtk_qos_dscpRmkSrc_t));
    SETSOCKOPT(RTDRV_QOS_PORTDSCPREMARKSRCSEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_portDscpRemarkSrcSel_set */


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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.dscp, &dscp, sizeof(uint32));
    GETSOCKOPT(RTDRV_QOS_DSCP2DSCPREMARKGROUP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pDscp, &qos_cfg.dscp, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_qos_dscp2DscpRemarkGroup_get */


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
 *      RT_ERR_QOS_DSCP_VALUE   - Invalid dscp value
 * Note:
 *      dscp parameter can be DSCP value or internal priority according to configuration of API 
 *      rtk_qos_dscpRemarkSrcSel_set(), because DSCP remark functionality can map original DSCP 
 *      value or internal priority to TX DSCP value.
 */
int32
rtk_qos_dscp2DscpRemarkGroup_set(uint32 grpIdx,uint32 dscp, uint32 rmkDscp)
{
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.grpIdx, &grpIdx, sizeof(uint32));
    osal_memcpy(&qos_cfg.dscp, &dscp, sizeof(uint32));
    osal_memcpy(&qos_cfg.rmkDscp, &rmkDscp, sizeof(uint32));
    SETSOCKOPT(RTDRV_QOS_DSCP2DSCPREMARKGROUP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_dscp2DscpRemarkGroup_set */



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
rtk_qos_fwd2CpuPriRemap_get(
    rtk_pri_t      intPri,
    rtk_pri_t   *pRempPri)
{
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRempPri), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.intPri, &intPri, sizeof(rtk_pri_t));
    GETSOCKOPT(RTDRV_QOS_FWD2CPUPRIREMAP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pRempPri, &qos_cfg.rempPri, sizeof(rtk_pri_t));

    return RT_ERR_OK;
}   /* end of rtk_qos_fwd2CpuPriRemap_get */

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
rtk_qos_fwd2CpuPriRemap_set(
    rtk_pri_t   intPri,
    rtk_pri_t   rempPri)
{
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.intPri, &intPri, sizeof(rtk_pri_t));
    osal_memcpy(&qos_cfg.rempPri, &rempPri, sizeof(rtk_pri_t));
    SETSOCKOPT(RTDRV_QOS_FWD2CPUPRIREMAP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_fwd2CpuPriRemap_set */



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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_QOS_SCHEDULINGQUEUE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pQweights, &qos_cfg.qweights, sizeof(rtk_qos_queue_weights_t));

    return RT_ERR_OK;
}   /* end of rtk_qos_schedulingQueue_get */

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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&qos_cfg.qweights, pQweights, sizeof(rtk_qos_queue_weights_t));
    SETSOCKOPT(RTDRV_QOS_SCHEDULINGQUEUE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_schedulingQueue_set */


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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriSelGrpIdx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_QOS_PORTPRISELGROUP_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pPriSelGrpIdx, &qos_cfg.priSelGrpIdx, sizeof(uint32));

    return RT_ERR_OK;
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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&qos_cfg.priSelGrpIdx, &priSelGrpIdx, sizeof(uint32));
    SETSOCKOPT(RTDRV_QOS_PORTPRISELGROUP_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_portPriSelGroup_set */



/* Function Name:
 *      rtk_qos_schedulingType_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      pQueueType    - the WFQ schedule type (RTK_QOS_WFQ or RTK_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_qos_schedulingType_get(rtk_qos_scheduling_type_t *pQueueType)
{
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueueType), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_QOS_SCHEDULINGTYPE_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pQueueType, &qos_cfg.queueType, sizeof(rtk_qos_scheduling_type_t));

    return RT_ERR_OK;
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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.queueType, &queueType, sizeof(rtk_qos_scheduling_type_t));
    SETSOCKOPT(RTDRV_QOS_SCHEDULINGTYPE_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_qosCfg_t qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_QOS_PORTDOT1PREMARKSRCSEL_GET, &qos_cfg, rtdrv_qosCfg_t, 1);
    osal_memcpy(pType, &qos_cfg.dot1pRmkSrc, sizeof(rtk_qos_do1pRmkSrc_t));

    return RT_ERR_OK;
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
    rtdrv_qosCfg_t qos_cfg;

    /* function body */
    osal_memset(&qos_cfg,0x0,sizeof(rtdrv_qosCfg_t));
    osal_memcpy(&qos_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&qos_cfg.dot1pRmkSrc, &type, sizeof(rtk_qos_do1pRmkSrc_t));
    SETSOCKOPT(RTDRV_QOS_PORTDOT1PREMARKSRCSEL_SET, &qos_cfg, rtdrv_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_qos_portDot1pRemarkSrcSel_set */
