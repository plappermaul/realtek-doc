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
#include <rtk/port.h>
#include <dal/luna_g3/dal_luna_g3.h>
#include <dal/luna_g3/dal_luna_g3_qos.h>
#include <osal/time.h>

#include <cortina-api/include/queue.h>
#include <cortina-api/include/port.h>
#include <aal/include/aal_l3_te.h>
#include <aal/include/aal_l3_tm.h>

/*
 * Data Declaration
 */
static uint32 qos_init = INIT_NOT_COMPLETED;
/*
 * Function Declaration
 */

/* Module Name    : QoS              */
/* Sub-module Name: Queue scheduling */

/* Function Name:
 *      dal_luna_g3_qos_init
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
int32
dal_luna_g3_qos_init(void)
{
    qos_init = INIT_COMPLETED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_luna_g3_qos_schedulingQueue_get
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
dal_luna_g3_qos_schedulingQueue_get(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    ca_status_t ret=CA_E_OK;
    ca_queue_schedule_mode_t mode;
    ca_queue_weights_t weights;
    uint32 queueId;
    ca_port_id_t port_id;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    if(port == CA_PORT_ID_NI7)
    {
        port_id = CA_PORT_ID(CA_PORT_TYPE_L3,port);
    }

    if((ret = ca_queue_schedule_get(0,port_id,&mode,&weights)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return RT_ERR_FAILED;
    }

    for(queueId = 0;queueId < HAL_MAX_NUM_OF_QUEUE();queueId++)
    {
        if(queueId > CA_QUEUE_COUNT)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return RT_ERR_QUEUE_NUM;
        }

        pQweights->weights[queueId] = weights.weights[queueId];
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_luna_g3_qos_schedulingQueue_set
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
dal_luna_g3_qos_schedulingQueue_set(rtk_port_t port, rtk_qos_queue_weights_t *pQweights)
{
    ca_status_t ret=CA_E_OK;
    ca_queue_schedule_mode_t mode;
    ca_queue_weights_t weights;
    uint32 queueId;
    ca_port_id_t port_id;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_QOS),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(qos_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pQweights), RT_ERR_NULL_POINTER);

    for(queueId = 0;queueId < HAL_MAX_NUM_OF_QUEUE();queueId++)
    {
        if(queueId > CA_QUEUE_COUNT)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return RT_ERR_QUEUE_NUM;
        }
        weights.weights[queueId] = pQweights->weights[queueId];
    }

    mode = CA_SCHEDULE_DEFICIT_WEIGHT_ROUND_ROBIN;

    if(port == CA_PORT_ID_NI7)
    {
        port_id = CA_PORT_ID(CA_PORT_TYPE_L3,port);
    }

    if((ret = ca_queue_schedule_set(0,port_id,mode,&weights)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}