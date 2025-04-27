/*
 * Copyright(c) Realtek Semiconductor Corporation, 2010
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those common public rx thread APIs and data type that would be used by
 *           NIC(Network Interface Controller) module in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) NIC rx
 *
 *
 */
#ifndef __NIC_RX_H__
#define __NIC_RX_H__

/*
 * Include Files
 */
#include <common/type.h>
#include <drv/nic/nic.h>

/*
 * Symbol Definition
 */

#define QUEUE_DROP_THRESH                   (800)
#define NIC_RX_WEIGHT_MAX                   (15)
#define NIC_RX_SCHED_ALL_PACKETS            (0)
#define NIC_RXRING_NUMBER                   (8)


typedef struct nic_rx_queue_s
{
    drv_nic_pkt_t  *rxPktDesc[QUEUE_RX_POOL_SIZE];  /* descriptor pointer array to the packet */
    drv_nic_pkt_t  **enQueueDPBase;             /* pointer to the position that packet can be enqueued */
    drv_nic_pkt_t  **deQueueDPBase;             /* pointer to the position that packet can be dequeued */
    uint32  count;                              /* number of pkts in queue */
    uint32  drop_thresh;                        /* packet is going to be dropped once queue length over this threshhold  */
    uint32  weight;
} nic_rx_queue_t;


/*
 * Data Declaration
 */


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */


/* Function Name:
 *      nic_rx_thread_init
 * Description:
 *      Initialize queue default setting in the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Note:
 *      None
 */
extern int32
nic_rx_thread_init(uint32 unit);


/* Function Name:
 *      nic_rx_thread_create
 * Description:
 *      Create thread for processing recevied packet in the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Note:
 *      None
 */
extern int32
nic_rx_thread_create(uint32 unit);

/* Function Name:
 *      nic_rx_thread_destroy
 * Description:
 *      Destroy thread for processing recevied packet in the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Note:
 *      None
 */
extern int32
nic_rx_thread_destroy(uint32 unit);

/* Function Name:
 *      nic_rx_pkt_enqueue
 * Description:
 *      enqueue rx packet
 * Input:
 *      qid - queue id
 * Output:
 *      pPacket  - pointer to packet that is going to be enqueued.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
nic_rx_pkt_enqueue(uint32 qid, drv_nic_pkt_t *pPacket);

/* Function Name:
 *      nic_rx_pkt_dequeue
 * Description:
 *      dequeue rx packet
 * Input:
 *      qid - queue id
 * Output:
 *      count  - number of packet that is going to be retrieved from the queue.
 *      *pFirst_packet - pointer to the first packet of the queue that being dequeued.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
nic_rx_pkt_dequeue(uint32 qid, drv_nic_pkt_t **pFirst_packet, uint32 count);

/* Function Name:
 *      nic_rx_queueInfo_get
 * Description:
 *      Get the queue related information of a specific queue for scheduler
 * Input:
 *      qid   - queue id that is going to configure weight
 * Output:
 *      pRx_queue - pointer of rx queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
nic_rx_queueInfo_get(uint32 unit, uint32 qid, nic_rx_queue_t **ppRx_queue);

/* Function Name:
 *      nic_rx_thread_notify
 * Description:
 *      Notify RX thread to process packet immediately.
 * Input:
 *      unit  - unit id
 * Output:
 *      pRx_queue - pointer of rx queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
nic_rx_thread_notify(uint32 unit);

#endif /* __NIC_RX_H__ */
