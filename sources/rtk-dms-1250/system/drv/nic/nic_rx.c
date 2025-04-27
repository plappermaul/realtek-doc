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

/*
 * Include Files
 */
#include <common/error.h>
#include <common/debug/rt_log.h>
#include <common/rt_type.h>
#include <private/drv/nic/nic_rx.h>
#include <osal/print.h>
#include <osal/sem.h>
#include <osal/thread.h>


/*
 * Symbol Definition
 */
/* RX Thread parameter */
#define RX_THREAD_STACK_SIZE    (32768)
#define RX_THREAD_PRI           (1)
#define RX_THREAD_DEFAULT_UNIT  (0)
#define NIC_RX_THREAD_CB_PRIORITY_MAX   (8)

/*
 * Data Declaration
 */
typedef struct nic_rx_thread_cb_entry_s
{
    nic_rx_thread_cb_f rx_callback;
    void    *pCookie;
    uint32  flags;
} nic_rx_thread_cb_entry_t;

static osal_sem_t       _nic_rx_notify_sem;
static uint32           _nic_rx_thread_id;
static nic_rx_queue_t   _nic_pkt_queue[NIC_RXRING_NUMBER];
static osal_mutex_t     _nic_rx_sem[RTK_MAX_NUM_OF_UNIT];
static nic_rx_thread_cb_entry_t _nic_rx_thread_cb_tbl[NIC_RX_THREAD_CB_PRIORITY_MAX];

extern drv_nic_initCfg_t _nic_init_conf;

/*
 * Macro Definition
 */
/* nic semaphore handling */
#define NIC_RX_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(_nic_rx_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_NIC), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define NIC_RX_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(_nic_rx_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_NIC), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */
static int32 _nic_rx_pktProcess_thread(void *pInput);
static int32 _nic_rx_pkt_scheduler(uint32 *pCosq, uint32 *pCount);
static int32 _nic_rx_pkt_process(uint32 unit, uint32 qid, uint32 count);

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
nic_rx_thread_init(uint32 unit)
{
    uint32 i, j;

    /* create semaphore */
    _nic_rx_sem[unit] = osal_sem_mutex_create();
    if (0 == _nic_rx_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_NIC), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    for (i = 0; i < NIC_RXRING_NUMBER; i++)
    {
        _nic_pkt_queue[i].enQueueDPBase = _nic_pkt_queue[i].rxPktDesc;
        _nic_pkt_queue[i].deQueueDPBase = _nic_pkt_queue[i].rxPktDesc;
        _nic_pkt_queue[i].count = 0;
        _nic_pkt_queue[i].drop_thresh = QUEUE_DROP_THRESH;
        _nic_pkt_queue[i].weight = 0;
        RT_LOG(LOG_DEBUG, MOD_NIC, "_nic_pkt_queue[%d] = 0x%08x\n", i, (uintptr)&_nic_pkt_queue[i]);
        RT_LOG(LOG_DEBUG, MOD_NIC, "_nic_pkt_queue[%d].enQueueDPBase = 0x%08x\n", i, (uintptr)&_nic_pkt_queue[i].enQueueDPBase);
        RT_LOG(LOG_DEBUG, MOD_NIC, "_nic_pkt_queue[%d].deQueueDPBase = 0x%08x\n", i, (uintptr)&_nic_pkt_queue[i].deQueueDPBase);
        RT_LOG(LOG_DEBUG, MOD_NIC, "_nic_pkt_queue[%d].count = %d\n", i, _nic_pkt_queue[i].count);
        RT_LOG(LOG_DEBUG, MOD_NIC, "_nic_pkt_queue[%d].drop_thresh = %d\n", i, _nic_pkt_queue[i].drop_thresh);
        RT_LOG(LOG_DEBUG, MOD_NIC, "_nic_pkt_queue[%d].weight = %d\n", i, _nic_pkt_queue[i].weight);
        for (j = 0; j < QUEUE_RX_POOL_SIZE; j++)
        {
            _nic_pkt_queue[i].rxPktDesc[j] = NULL;
        }
    }

    return RT_ERR_OK;
} /* end of nic_rx_thread_init */



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
int32
nic_rx_thread_create(uint32 unit)
{
    /* create semaphore for sync, this semaphore is empty in beginning */
    _nic_rx_notify_sem = osal_sem_create(0);
    if (0 == _nic_rx_notify_sem)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_NIC), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* create Rx threads to process packet */
    if ((osal_thread_t)(uintptr)NULL == (_nic_rx_thread_id = osal_thread_create("RX Thread", 8096,
          0, (void *)_nic_rx_pktProcess_thread, NULL)))
    {
        RT_ERR(RT_ERR_FAILED, (MOD_NIC), "RX thread create failed");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

} /* end of _nic_rx_start_thread */

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
int32
nic_rx_thread_destroy(uint32 unit)
{
    int32   ret;

    /* destroy RX threads */
    if ((ret = osal_thread_destroy(_nic_rx_thread_id)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_NIC), "RX thread destroy failed");
        return ret;
    }

    return RT_ERR_OK;
} /* end of nic_rx_thread_destroy */

/* Function Name:
 *      _nic_rx_pktProcess_thread
 * Description:
 *      Main thread used to process packet
 * Input:
 *      pInput     -
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
static int32
_nic_rx_pktProcess_thread(void *pInput)
{
    int32   ret;
    uint32  qid=0, count=0;

    RT_LOG(LOG_DEBUG, MOD_NIC, "Rx pkt process thread start running!\n");
    /* forever loop */
    while (1)
    {
        /* Schedule number of packets to be processed. */
        while ((ret = _nic_rx_pkt_scheduler(&qid, &count)) == RT_ERR_OK)
        {
            /* Check for completion. */
            if (!count)
            {
                break;
            }
            /* Process packets based on scheduler allocation. */
            if ((ret = _nic_rx_pkt_process(RX_THREAD_DEFAULT_UNIT, qid, count))!= RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_NIC), "_nic_rx_pkt_process failed - check the scheduler.");
            }
        }

        /* wait semaphore for rx thread sleep interval */
        osal_sem_take(_nic_rx_notify_sem, OSAL_SEM_WAIT_FOREVER);
    }

    return RT_ERR_OK;
} /* end of _nic_rx_pktProcess_thread */

/* Function Name:
 *      _nic_rx_pkt_scheduler
 * Description:
 *      Schedule packet to be processed base on configured weight of each cos queue
 * Input:
 *      None
 * Output:
 *      pCosq   - CoS queue ID that is going to be dequeued
 *      pCount  - number of packets that is going to be dequeued
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32
_nic_rx_pkt_scheduler(uint32 *pCosq, uint32 *pCount)
{
    int32  ret;
    uint32 weight = 0;
    static int32 ringId = -1;

    if (ringId == -1)
    {
        ringId = (NIC_RXRING_NUMBER - 1);
    }

    for (; ringId >= 0; ringId--)
    {
        /* Check if queue is supported by the device. */
        if (ringId >= NIC_RXRING_NUMBER) {
            RT_LOG(LOG_DEBUG, (MOD_NIC), "invalid ringId(%d)!", ringId);
            return RT_ERR_FAILED;
        }

        /* Get number of packets awaiting processing and dequeue base on weight */
        if (_nic_pkt_queue[ringId].count)
        {
            if ((ret = nic_rx_queueWeight_get(0, ringId, &weight) != RT_ERR_OK))
            {
                RT_LOG(LOG_DEBUG, (MOD_NIC), "Unknown weight of Queue %d\n", ringId);
            }
            *pCosq = (uint32)ringId;
            if (NIC_RX_SCHED_ALL_PACKETS == weight) {
                *pCount = _nic_pkt_queue[ringId].count;
            } else if (weight < _nic_pkt_queue[ringId].count) {
                *pCount = weight;
            }  else {
                *pCount = _nic_pkt_queue[ringId].count;
            }
            //osal_printf("sheduler say %d packets to be processed\n", *count);
            return RT_ERR_OK;
        }
    }

    /* Init variable for next scheduling cycle. */
    ringId = -1;
    *pCount = 0;

    return RT_ERR_OK;
} /* end of _nic_rx_pkt_scheduler */

static int32
_nic_rx_pkt_process(uint32 unit, uint32 qid, uint32 count)
{
    int32   ret;
    uint32   i;
    int32   nic_rx_handle;
    drv_nic_pkt_t *pPkt_head, *pPkt;

    /*
     * Steal the queue's packets; if rate limiting on, just take as many
     * as can be handled.
     */
    if (0 == count) {
        return RT_ERR_OK;
    }

    if((ret = nic_rx_pkt_dequeue(qid, &pPkt_head, count)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_NIC), "Dequeue failed");
    }

    if (pPkt_head == NULL) {
        return RT_ERR_OK;   /* Strange but Queue is empty.*/
    }

    /* Process packets */
    while(pPkt_head)
    {
        nic_rx_handle = NIC_RX_NOT_HANDLED;
        pPkt = pPkt_head;
        pPkt_head = pPkt_head->next;
        for (i = 0; i < NIC_RX_CB_PRIORITY_NUMBER; i++)
        {
            if (NULL != _nic_rx_thread_cb_tbl[i].rx_callback)
            {
                nic_rx_handle = _nic_rx_thread_cb_tbl[i].rx_callback(unit, pPkt, _nic_rx_thread_cb_tbl[0].pCookie);
                if (NIC_RX_HANDLED_OWNED == nic_rx_handle)
                        break;
            }
        }

        if (nic_rx_handle != NIC_RX_HANDLED_OWNED)
        {   /* We have to free this packet here */
            _nic_init_conf.pkt_free(unit, pPkt);
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      nic_rx_queueWeight_set
 * Description:
 *      Configure the weight of a specific queue for scheduler
 * Input:
 *      unit  - unit id
 *      qid   - queue id that is going to configure weight
 *      qweight - queue weight that determine the packet amount will be processed through scheduler
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter may exceed the limit of the variable
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
nic_rx_queueWeight_set(uint32 unit, uint32 qid, uint32 qweight)
{
    /* Check arguments */
    RT_PARAM_CHK(qid >= NIC_RXRING_NUMBER, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(qweight > NIC_RX_WEIGHT_MAX, RT_ERR_OUT_OF_RANGE);

    RT_LOG(LOG_DEBUG, (MOD_NIC), "unit=%d, qid=%d, qweight=%d", unit, qid, qweight);

    NIC_RX_SEM_LOCK(unit);
    _nic_pkt_queue[qid].weight = qweight;
    NIC_RX_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of nic_rx_queueWeight_set */

/* Function Name:
 *      nic_rx_queueWeight_get
 * Description:
 *      Get the weight configuration of a specific queue for scheduler
 * Input:
 *      unit  - unit id
 *      qid   - queue id that is going to configure weight
 * Output:
 *      pQweight - queue weight that determine the packet amount will be processed through scheduler
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
nic_rx_queueWeight_get(uint32 unit, uint32 qid, uint32 *pQweight)
{
    /* Check arguments */
    RT_PARAM_CHK(qid >= NIC_RXRING_NUMBER, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pQweight, RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_NIC), "unit=%d, qid=%d", unit, qid);

    NIC_RX_SEM_LOCK(unit);
    *pQweight = _nic_pkt_queue[qid].weight;
    NIC_RX_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_NIC), "queue weight=%d", *pQweight);

    return RT_ERR_OK;
} /* end of nic_rx_queueWeight_get */

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
int32
nic_rx_pkt_enqueue(uint32 qid, drv_nic_pkt_t *pPacket)
{
    int32   ret = RT_ERR_FAILED;
    nic_rx_queue_t* pRx_queue;

    RT_PARAM_CHK(qid >= NIC_RXRING_NUMBER, RT_ERR_OUT_OF_RANGE);

    pRx_queue = &_nic_pkt_queue[qid];

    NIC_RX_SEM_LOCK(RX_THREAD_DEFAULT_UNIT);
    if (*pRx_queue->enQueueDPBase == NULL)
    {
        *pRx_queue->enQueueDPBase = pPacket;
        pRx_queue->count++;
        pRx_queue->enQueueDPBase++;

        if (pRx_queue->enQueueDPBase == &pRx_queue->rxPktDesc[QUEUE_RX_POOL_SIZE])
        {
            RT_LOG(LOG_DEBUG, MOD_NIC, "Need to wrap around to enqueue!!\n");
            pRx_queue->enQueueDPBase = pRx_queue->rxPktDesc;
        }
        ret = RT_ERR_OK;
    }
    NIC_RX_SEM_UNLOCK(RX_THREAD_DEFAULT_UNIT);
    return ret;
} /* end of nic_rx_pkt_enqueue */


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
int32
nic_rx_pkt_dequeue(uint32 qid, drv_nic_pkt_t **pFirst_packet, uint32 count)
{
    int32   ret = RT_ERR_FAILED;
    drv_nic_pkt_t *current = NULL;
    nic_rx_queue_t* pRx_queue;

    RT_PARAM_CHK(qid >= NIC_RXRING_NUMBER, RT_ERR_OUT_OF_RANGE);

    pRx_queue = &_nic_pkt_queue[qid];

    if (count > pRx_queue->count)
        count = pRx_queue->count;

    NIC_RX_SEM_LOCK(RX_THREAD_DEFAULT_UNIT);
    while((count > 0))
    {
        /* Check queue content */
        if (NULL == *pRx_queue->deQueueDPBase)
        {
            osal_printf("The dequeue pointer is pointed to NULL\n");
            ret = RT_ERR_FAILED;
            return ret;
        }

        /* Assign first packet */
        if (NULL == current)
        {
            *pFirst_packet = *pRx_queue->deQueueDPBase;
            current = *pRx_queue->deQueueDPBase;
            current->next = NULL;
        }
        else
        {
            current->next = *pRx_queue->deQueueDPBase;
            current = current->next;
            current->next = NULL;                       /* need to confirm if this is needed */
        }
        *pRx_queue->deQueueDPBase = NULL;

        pRx_queue->count--;
        count--;

        pRx_queue->deQueueDPBase++;
        if (pRx_queue->deQueueDPBase == &pRx_queue->rxPktDesc[QUEUE_RX_POOL_SIZE])
            pRx_queue->deQueueDPBase = pRx_queue->rxPktDesc;

        ret = RT_ERR_OK;
    }
    NIC_RX_SEM_UNLOCK(RX_THREAD_DEFAULT_UNIT);
    return ret;
} /* end of nic_rx_pkt_dequeue */

/* Function Name:
 *      nic_rx_queueInfo_get
 * Description:
 *      Get the queue related information of a specific queue for scheduler
 * Input:
 *      unit  - unit id
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
int32
nic_rx_queueInfo_get(uint32 unit, uint32 qid, nic_rx_queue_t **ppRx_queue)
{
    RT_PARAM_CHK(qid >= NIC_RXRING_NUMBER, RT_ERR_OUT_OF_RANGE);

    *ppRx_queue = &_nic_pkt_queue[qid];

    //osal_printf("_nic_pkt_queue[%d] = 0x%08x\n", qid, (uint32)&_nic_pkt_queue[qid]);

    return RT_ERR_OK;
} /* end of nic_rx_queueWeight_get */

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
int32
nic_rx_thread_notify(uint32 unit)
{
    osal_sem_give(_nic_rx_notify_sem);

    return RT_ERR_OK;
} /* end of nic_rx_thread_notify */

/* Function Name:
 *      nic_rx_thread_cb_register
 * Description:
 *      Register to receive callbacks for received packets of the specified device.
 * Input:
 *      unit     - unit id
 *      priority - Relative priority of the callback
 *      fRxCb    - pointer to a handler of received packets
 *      pCookie  - application data returned with callback (can be null)
 *      flags    - optional flags for reserved
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
nic_rx_thread_cb_register(uint32 unit, uint8 priority, nic_rx_thread_cb_f fRxCb, void *pCookie, uint32 flags)
{
    RT_LOG(LOG_DEBUG, (MOD_NIC), "");

    /* Check arguments */
    RT_PARAM_CHK(priority >= NIC_RX_THREAD_CB_PRIORITY_MAX, RT_ERR_FAILED);
    RT_PARAM_CHK(NULL == fRxCb, RT_ERR_NULL_POINTER);

    if (NULL == _nic_rx_thread_cb_tbl[priority].rx_callback)
    {
        _nic_rx_thread_cb_tbl[priority].rx_callback = fRxCb;
        _nic_rx_thread_cb_tbl[priority].pCookie     = pCookie;
        _nic_rx_thread_cb_tbl[priority].flags       = flags;
    }
    else
    {
        /* Handler is already existing */
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of nic_rx_thread_cb_register */

/* Function Name:
 *      nic_rx_thread_cb_unregister
 * Description:
 *      Unregister to receive callbacks for received packets of the specified device.
 * Input:
 *      unit     - unit id
 *      priority - Relative priority of the callback
 *      fRxCb    - pointer to a handler of received packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
nic_rx_thread_cb_unregister(uint32 unit, uint8 priority, nic_rx_thread_cb_f fRxCb)
{
    RT_LOG(LOG_DEBUG, (MOD_NIC), "");

    /* Check arguments */
    RT_PARAM_CHK(priority >= NIC_RX_THREAD_CB_PRIORITY_MAX, RT_ERR_FAILED);
    RT_PARAM_CHK(NULL == fRxCb, RT_ERR_NULL_POINTER);

    if (fRxCb == _nic_rx_thread_cb_tbl[priority].rx_callback)
    {
        _nic_rx_thread_cb_tbl[priority].rx_callback = NULL;
        _nic_rx_thread_cb_tbl[priority].pCookie     = NULL;
    }
    else
    {
        /* Handler did not finded */
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of nic_rx_thread_cb_unregister */
