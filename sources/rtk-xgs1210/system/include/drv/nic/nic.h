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
 * $Revision: 85751 $
 * $Date: 2018-01-25 19:22:44 +0800 (Thu, 25 Jan 2018) $
 *
 * Purpose : Definition of Network Interface Controller API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) CPU Tag
 *           (2) NIC Tx
 *           (3) NIC Rx
 *
 */

#ifndef __DRV_NIC_H__
#define __DRV_NIC_H__

/*
 * Include Files
 */
#include <osal/isr.h>
#include <private/drv/nic/nic_common.h>

/*
 * Symbol Definition
 */
#define CPUTAG_SIZE                     20
#define NIC_FLAG_RX_CRC_INCLUDE		    0x00000001
#define REALTEK_CPUTAG_ID               0x8899
#define CPU_TAG_ID_LEN                  2         /* CPU tag ID length: 2 bytes */
#define REALTEK_CPUTAG_ID               0x8899
#define NIC_RX_RING_ALL                 0xffffffff
#define FLAG_JUMBO_PKT		            0x80000000
#define JUMBO_BUFF_SIZE                 12308


#define QUEUE_SHARE_POOL_SIZE           600
#define QUEUE_RX_POOL_SIZE              200
#define QUEUE_TX_POOL_SIZE              200
#define QUEUE_MAX_SIZE                  (QUEUE_SHARE_POOL_SIZE + QUEUE_RX_POOL_SIZE + QUEUE_TX_POOL_SIZE)
#define JUMBO_QUEUE_MAX_SIZE            20

/*
 * Data Type Definition
 */
typedef enum nic_intr_type_e
{
    NIC_RX_DONE = 0,
    NIC_RX_RUNOUT,
    NIC_TX_DONE,
    NIC_TX_ALLDONE,
    NIC_NTFY_DONE,
    NIC_NTFY_BUF_RUNOUT,
    NIC_NTFY_LOCALBUF_RUNOUT,
    NIC_END
} nic_intr_type_t;

typedef enum nic_dir_e
{
    NIC_DIR_TX = 0,
    NIC_DIR_RX,
    NIC_DIR_END
} nic_dir_t;

typedef enum pktBuf_dir_e
{
    PKTBUF_TX = 0,
    PKTBUF_RX,
    PKTBUF_DIR_END
} pktBuf_dir_t;

typedef enum nic_txRing_e
{
    NIC_TXRING_HIGH = 0,
    NIC_TXRING_LOW,
    NIC_TXRING_BOTH,
    NIC_TXRING_END
} nic_txRing_t;

typedef struct myIsrParam
{
    uint32  unit;
    uint32  ringId;
}isr_param_t;


typedef drv_nic_rx_t (*nic_rx_thread_cb_f)(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie);


/*
 * Data Declaration
 */

/*
 * Macro Definition
 */
#define READ16(ptr) ((*(ptr) << 8) | (*(ptr + 1)))

/*
 * Function Declaration
 */


/* Function Name:
 *      drv_nic_pkt_tx
 * Description:
 *      Transmit a packet via nic of the specified device.
 * Input:
 *      unit    - unit id
 *      pPacket - pointer to a single packet struct
 *      fTxCb   - pointer to a handler of transmited packets
 *      pCookie - application data returned with callback (can be null)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      When fTxCb is NULL, driver will free packet and not callback any more.
 * Changes:
 *      [SDK_3.0.0]
 *          The members of drv_nic_pkt_t has been updated.
 */
extern int32
drv_nic_pkt_tx(uint32 unit, drv_nic_pkt_t *pPacket, drv_nic_tx_cb_f fTxCb, void *pCookie);


/* Function Name:
 *      drv_nic_rx_start
 * Description:
 *      Start the rx action of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_nic_rx_start(uint32 unit);

/* Function Name:
 *      drv_nic_rx_stop
 * Description:
 *      Stop the rx action of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_nic_rx_stop(uint32 unit);

/* Function Name:
 *      drv_nic_rx_status_get
 * Description:
 *      Get NIC rx status of the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pStatus - rx status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_FOUND
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_nic_rx_status_get(uint32 unit, uint32 *pStatus);

/* Function Name:
 *      drv_nic_rx_register
 * Description:
 *      Register to receive callbacks for received packets of the specified device.
 * Input:
 *      unit     - unit id
 *      priority - Relative priority of the callback (255 is lowest)
 *      fRxCb    - pointer to a handler of received packets
 *      pCookie  - application data returned with callback (can be null)
 *      flags    - optional flags for reserved
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If flags have turn on the NIC_FLAG_RX_CRC_INCLUDE flag, means that asking packet
 *      handed to upper layer should include CRC.
 * Changes:
 *      [SDK_3.0.0]
 *          The members of drv_nic_pkt_t within drv_nic_rx_cb_f has been updated.
 */
extern int32
drv_nic_rx_register(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb, void *pCookie, uint32 flags);


/* Function Name:
 *      drv_nic_rx_unregister
 * Description:
 *      Unregister to receive callbacks for received packets of the specified device.
 * Input:
 *      unit     - unit id
 *      priority - Relative priority of the callback (255 is lowest)
 *      fRxCb    - pointer to a handler of received packets (can be null)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_nic_rx_unregister(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb);


/* Function Name:
 *      drv_nic_pkt_alloc
 * Description:
 *      Packet allocate API in the specified device.
 * Input:
 *      unit     - unit id
 *      size     - packet size
 *      flags    - flags
 * Output:
 *      ppPacket - pointer buffer of packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_nic_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket);


/* Function Name:
 *      drv_nic_pkt_free
 * Description:
 *      Packet free API in the specified device.
 * Input:
 *      unit     - unit id
 *      pPacket  - pointer buffer of packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_nic_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket);


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
extern int32
nic_rx_thread_cb_register(uint32 unit, uint8 priority, nic_rx_thread_cb_f fRxCb, void *pCookie, uint32 flags);

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
extern int32
nic_rx_thread_cb_unregister(uint32 unit, uint8 priority, nic_rx_thread_cb_f fRxCb);

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
nic_rx_queueWeight_set(uint32 unit, uint32 qid, uint32 qweight);

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
extern int32
nic_rx_queueWeight_get(uint32 unit, uint32 qid, uint32 *pQweight);


/* Function Name:
 *      nic_probe
 * Description:
 *      Probe nic module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
nic_probe(uint32 unit);


/* Function Name:
 *      drv_nic_init
 * Description:
 *      Initialize nic module of the specified device.
 * Input:
 *      unit     - unit id
 *      pInitCfg - pointer to initial config struct of NIC
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize nic module before calling any nic APIs.
 * Changes:
 *      None
 */
extern int32
drv_nic_init(uint32 unit, drv_nic_initCfg_t *pInitCfg);


#endif /* __DRV_NIC_H__ */

