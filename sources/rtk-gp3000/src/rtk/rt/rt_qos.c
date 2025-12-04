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
#include <common/rt_type.h>
#include <rtk/rt/rt_port.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_qos.h>

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
    int32   ret;
    rtk_qos_priSelWeight_t   weight;

    /* function body */
    if (NULL == RT_MAPPER->qos_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_init();
    RTK_API_UNLOCK();

#if defined(CONFIG_SDK_RTL9607C)
    RTK_API_LOCK();    
    memset(&weight, 0x00, sizeof(weight));
    weight.weight_of_dscp = 5;
    weight.weight_of_portBased = 3;
    ret = RT_MAPPER->qos_priSelGroup_set(1, &weight);
    RTK_API_UNLOCK();
#endif
     
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->qos_schedulingQueue_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_schedulingQueue_get(port, (rtk_qos_queue_weights_t *)pQweights);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->qos_schedulingQueue_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->qos_schedulingQueue_set(port, (rtk_qos_queue_weights_t *)pQweights);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret = RT_ERR_DRIVER_NOT_FOUND;
    uint8   i;
    rtk_pri_t   intPri;
    uint32      dp = 0;

    /* function body */
    if (NULL == RT_MAPPER->qos_dscpPriRemapGroup_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    for(i=0; i<RT_MAX_DCSP_NUM;i++)
    {
        ret = RT_MAPPER->qos_dscpPriRemapGroup_get(0, i, &intPri, &dp);
        if(RT_ERR_OK == ret)
            pDscp2PbitTbl->dscp[i] = intPri;
    }
    RTK_API_UNLOCK();

    return ret;
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
    int32   ret = RT_ERR_DRIVER_NOT_FOUND;
    uint8   i;

    /* function body */
    if (NULL == RT_MAPPER->qos_dscpPriRemapGroup_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    for(i=0; i<RT_MAX_DCSP_NUM;i++)
        ret = RT_MAPPER->qos_dscpPriRemapGroup_set(0, i, pDscp2PbitTbl->dscp[i], 0);
    RTK_API_UNLOCK();
    
    return ret;
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
 *      RT_ERR_OK             - OK
 *      RT_ERR_FAILED         - Failed
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      
 *      
 */
int32
rt_qos_ingress_qos_set(uint8 index, rt_qos_ingress_qos_t *pIgrQos)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_qos_ingress_qos_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_qos_ingress_qos_set(index, pIgrQos);
    RTK_API_UNLOCK();
    return ret;
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
 *      RT_ERR_OK             - OK
 *      RT_ERR_FAILED         - No such qos setting
 * Note:
 *      
 *      
 */
int32
rt_qos_ingress_qos_del(uint8 index)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_qos_ingress_qos_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_qos_ingress_qos_del(index);
    RTK_API_UNLOCK();
    return ret;
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
 *      RT_ERR_OK             - OK
 *      RT_ERR_FAILED         - No such qos setting
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      
 *      
 */
int32
rt_qos_ingress_qos_get(uint8 index, rt_qos_ingress_qos_t *pIgrQos)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_qos_ingress_qos_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_qos_ingress_qos_get(index, pIgrQos);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_qos_ingress_qos_get */