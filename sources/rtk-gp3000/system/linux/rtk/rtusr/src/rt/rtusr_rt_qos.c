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
#include <rtk/rt/rt_port.h>

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
 *      rt_qos_init
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
rt_qos_init(void)
{
    rtdrv_rt_qosCfg_t rt_qos_cfg;

    /* function body */
    memset(&rt_qos_cfg, 0, sizeof(rtdrv_rt_qosCfg_t));
    SETSOCKOPT(RTDRV_RT_QOS_INIT, &rt_qos_cfg, rtdrv_rt_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_qos_init */

/* Module Name    : QoS              */
/* Sub-module Name: Queue scheduling */

/* Function Name:
 *      rt_qos_schedulingQueue_get
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
rt_qos_schedulingQueue_get(rt_port_t port, rt_qos_queue_weights_t *pQweights)
{
    rtdrv_rt_qosCfg_t rt_qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_qos_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_QOS_SCHEDULINGQUEUE_GET, &rt_qos_cfg, rtdrv_rt_qosCfg_t, 1);
    osal_memcpy(pQweights, &rt_qos_cfg.qweights, sizeof(rt_qos_queue_weights_t));

    return RT_ERR_OK;
}   /* end of rt_qos_schedulingQueue_get */

/* Function Name:
 *      rt_qos_schedulingQueue_set
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
rt_qos_schedulingQueue_set(rt_port_t port, rt_qos_queue_weights_t *pQweights)
{
    rtdrv_rt_qosCfg_t rt_qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&rt_qos_cfg, 0, sizeof(rtdrv_rt_qosCfg_t));
    osal_memcpy(&rt_qos_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_qos_cfg.qweights, pQweights, sizeof(rt_qos_queue_weights_t));
    SETSOCKOPT(RTDRV_RT_QOS_SCHEDULINGQUEUE_SET, &rt_qos_cfg, rtdrv_rt_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_qos_schedulingQueue_set */

/* Function Name:
 *      rt_qos_dscp2pbit_get
 * Description:
 *      Set the dscp to P-bit mapping table
 * Input:
 *      None.
 * Output:
 *      pDscp2PbitTbl - the array of Dscp to P-bit mapping
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      
 *      
 */
int32
rt_qos_dscp2pbit_get(rt_qos_dscp2Pbit_t *pDscp2PbitTbl)
{
    rtdrv_rt_qosCfg_t rt_qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp2PbitTbl), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_QOS_DSCP2PBIT_GET, &rt_qos_cfg, rtdrv_rt_qosCfg_t, 1);
    osal_memcpy(pDscp2PbitTbl, &rt_qos_cfg.dscp2Pbit, sizeof(rt_qos_dscp2Pbit_t));

    return RT_ERR_OK;
}   /* end of rt_qos_dscp2pbit_get */


/* Function Name:
 *      rt_qos_dscp2pbit_set
 * Description:
 *      Set the dscp to P-bit mapping table
 * Input:
 *      pDscp2PbitTbl - the array of Dscp to P-bit mapping
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
int32
rt_qos_dscp2pbit_set(rt_qos_dscp2Pbit_t *pDscp2PbitTbl)
{
    rtdrv_rt_qosCfg_t rt_qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp2PbitTbl), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&rt_qos_cfg,0,sizeof(rtdrv_rt_qosCfg_t));
    osal_memcpy(&rt_qos_cfg.dscp2Pbit, pDscp2PbitTbl, sizeof(rt_qos_dscp2Pbit_t));
    SETSOCKOPT(RTDRV_RT_QOS_DCSP2PBIT_SET, &rt_qos_cfg, rtdrv_rt_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_qos_dscp2pbit_set */

/* Function Name:
 *      rt_qos_ingress_qos_set
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
int32
rt_qos_ingress_qos_set(uint8 index, rt_qos_ingress_qos_t *pIgrQos)
{
    rtdrv_rt_qosCfg_t rt_qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIgrQos), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&rt_qos_cfg,0,sizeof(rtdrv_rt_qosCfg_t));
    osal_memcpy(&rt_qos_cfg.index, &index, sizeof(uint8));
    osal_memcpy(&rt_qos_cfg.igrQos, pIgrQos, sizeof(rt_qos_ingress_qos_t));
    SETSOCKOPT(RTDRV_RT_QOS_INGRESS_QOS_SET, &rt_qos_cfg, rtdrv_rt_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_qos_ingress_qos_set */

/* Function Name:
 *      rt_qos_ingress_qos_del
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
int32
rt_qos_ingress_qos_del(uint8 index)
{
    rtdrv_rt_qosCfg_t rt_qos_cfg;

    /* function body */
    memset(&rt_qos_cfg,0,sizeof(rtdrv_rt_qosCfg_t));
    osal_memcpy(&rt_qos_cfg.index, &index, sizeof(uint8));
    SETSOCKOPT(RTDRV_RT_QOS_INGRESS_QOS_DEL, &rt_qos_cfg, rtdrv_rt_qosCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_qos_ingress_qos_del */

/* Function Name:
 *      rt_qos_ingress_qos_get
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
int32
rt_qos_ingress_qos_get(uint8 index, rt_qos_ingress_qos_t *pIgrQos)
{
    rtdrv_rt_qosCfg_t rt_qos_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIgrQos), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&rt_qos_cfg,0,sizeof(rtdrv_rt_qosCfg_t));
    osal_memcpy(&rt_qos_cfg.index, &index, sizeof(uint8));
    GETSOCKOPT(RTDRV_RT_QOS_INGRESS_QOS_GET, &rt_qos_cfg, rtdrv_rt_qosCfg_t, 1);
    osal_memcpy(pIgrQos, &rt_qos_cfg.igrQos, sizeof(rt_qos_ingress_qos_t));

    return RT_ERR_OK;
}   /* end of rt_qos_ingress_qos_get */
