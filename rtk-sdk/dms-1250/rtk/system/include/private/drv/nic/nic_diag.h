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
 * $Revision: 102110 $
 * $Date: 2019-11-28 21:16:01 +0800 (Thu, 28 Nov 2019) $
 *
 * Purpose : Definition those public NIC(Network Interface Controller) APIs and
 *           its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) CPU tag
 *            2) NIC tx
 *            3) NIC rx
 *
 */
#ifndef __NIC_DIAG_H__
#define __NIC_DIAG_H__

/*
 * Include Files
 */
#include <common/error.h>
#include <osal/isr.h>
#include <private/drv/nic/nic_common.h>
#include <common/rt_type.h>
#include <drv/nic/nic.h>

/*
 * Symbol Definition
 */
#define NIC_PKTHDR_MBUF_MODE_RX (0)
#define NIC_PKTHDR_MBUF_MODE_TX (1)

#define DEBUG_RX_RAW_LEN_BIT    (1 << NIC_DEBUG_RX_RAW_LEN)    /* 0x00000001 */
#define DEBUG_RX_CPU_TAG_BIT    (1 << NIC_DEBUG_RX_CPU_TAG)    /* 0x00000002 */
#define DEBUG_PRINT_TIME_BIT    (1 << NIC_DEBUG_PRINT_TIME)    /* 0x00000004 */
#define DEBUG_CPU_LOOPBACK_BIT  (1 << NIC_DEBUG_CPU_LOOPBACK)  /* 0x00000008 */
#define DEBUG_CPU_CALCRC_BIT    (1 << NIC_DEBUG_CPU_CALCRC)    /* 0x00000010 */
#define DEBUG_TX_RAW_LEN_BIT    (1 << NIC_DEBUG_TX_RAW_LEN)    /* 0x00010000 */
#define DEBUG_TX_CPU_TAG_BIT    (1 << NIC_DEBUG_TX_CPU_TAG)    /* 0x00020000 */

typedef enum nic_debug_e
{
    /* Rx debug flags */
    NIC_DEBUG_RX_RAW_LEN = 0,   /* 0 */
    NIC_DEBUG_RX_CPU_TAG,       /* 1 */
    NIC_DEBUG_PRINT_TIME,       /* 2 */
    NIC_DEBUG_CPU_LOOPBACK,     /* 3 */
    NIC_DEBUG_CPU_CALCRC,       /* 4 */

    /* Tx debug flags */
    NIC_DEBUG_TX_RAW_LEN = 16,  /* 16 */
    NIC_DEBUG_TX_CPU_TAG,       /* 17 */
    NIC_DEBUG_TX_PACKET,        /* 18 */
} nic_debug_t;

typedef enum nic_txTagStatus_e
{
    NIC_TXTAG_NONE = 0,   /* 0 */
    NIC_TXTAG_AUTO,       /* 1 */
    NIC_TXTAG_MANUAL       /* 2 */
} nic_txTagStatus_t;

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
 *      drv_nic_reset
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
 *      8380
 * Note:
 *      None
 */
extern int32
drv_nic_reset(uint32 unit);

/* Function Name:
 *      drv_nic_dbg_get
 * Description:
 *      Get NIC debug flags of the specified device.
 * Input:
 *      unit   - unit id
 * Output:
 *      pFlags - NIC debug flags
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      There are 4 BIT flags as following:
 *      - DEBUG_RX_RAW_LEN_BIT
 *      - DEBUG_RX_CPU_TAG_BIT
 *      - DEBUG_TX_RAW_LEN_BIT
 *      - DEBUG_TX_CPU_TAG_BIT
 */
extern int32
drv_nic_dbg_get(uint32 unit, uint32 *pFlags);

/* Function Name:
 *      drv_nic_dbg_set
 * Description:
 *      Set NIC debug flags of the specified device.
 * Input:
 *      unit  - unit id
 *      flags - NIC debug flags
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      There are 4 BIT flags can be selected as following:
 *      - DEBUG_RX_RAW_LEN_BIT
 *      - DEBUG_RX_CPU_TAG_BIT
 *      - DEBUG_TX_RAW_LEN_BIT
 *      - DEBUG_TX_CPU_TAG_BIT
 */
extern int32
drv_nic_dbg_set(uint32 unit, uint32 flags);

/* Function Name:
 *      drv_nic_cntr_dump
 * Description:
 *      Dump NIC debug counter information of the specified device.
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
 *      There are 4 debug counters be dump as following:
 *      - nic_tx_success_cntr
 *      - nic_tx_failed_cntr
 *      - nic_rx_success_cntr
 *      - nic_rx_failed_cntr
 */
extern int32
drv_nic_cntr_dump(uint32 unit);

/* Function Name:
 *      drv_nic_cntr_clear
 * Description:
 *      Clear NIC debug counter information of the specified device.
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
 *      Clear following NIC debug counters
 *      - nic_tx_success_cntr
 *      - nic_tx_failed_cntr
 *      - nic_rx_success_cntr
 *      - nic_rx_failed_cntr
 */
extern int32
drv_nic_cntr_clear(uint32 unit);

/* Function Name:
 *      drv_nic_ringbuf_dump
 * Description:
 *      Dump NIC buffer status of the specified device.
 * Input:
 *      unit   - unit id
 *      direct - direction (RX/TX)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Following message are dump
 *      1) From SW View
 *      - Rx Ring Packet Header (FDPBase, CDPIdx, RDPIdx)
 *      - Tx Ring Packet Header (FDPBase, CDPIdx, RDPIdx)
 *      - Rx Ring mBuffer (FDPBase, CDPIdx, RDPIdx)
 *      - Tx Ring mBuffer (FDPBase, CDPIdx, RDPIdx)
 *      2) From HW View
 *      - Rx Ring Packet Header(CDPIdx)
 *      - Tx Ring Packet Header(CDPIdx)
 *      - Rx Ring mBuffer (CDPIdx)
 *      - Tx Ring mBuffer (CDPIdx)
 *      3) Register Information
 *      - CPUIIMR (CPU Interface Interrupt Mask Register)
 *      - CPUIISR (CPU Interface Interrupt Status Register)
 *      - CPUICR  (CPU Interface Control Register)
 */
extern int32
drv_nic_ringbuf_dump(uint32 unit, nic_dir_t direct);

#if !defined(CONFIG_SDK_DRIVER_EXTC_NIC)
/* Function Name:
 *      drv_nic_pktHdrMBuf_dump
 * Description:
 *      Dump NIC packet header and mbuf detail information of the specified device.
 * Input:
 *      unit  - unit id
 *      mode  - tx/rx mode
 *      start - start ring id
 *      end   - end ring id
 *      flags - dump flags
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      1) valid 'mode' value:
 *      - NIC_PKTHDR_MBUF_MODE_RX
 *      - NIC_PKTHDR_MBUF_MODE_TX
 *      2) valid ring id (start .. end)
 *      - Rx (0 .. 7)
 *      - Tx (0 .. 1)
 *      3) valid 'flags' value:
 *      - TRUE: include packet raw data
 *      - FALSE: exclude packet raw data
 */
extern int32
drv_nic_pktHdrMBuf_dump(uint32 unit, uint32 mode, uint32 start, uint32 end, uint32 flags);
#endif

/* Function Name:
 *      drv_nic_isr_handler
 * Description:
 *      NIC event handler.
 * Input:
 *      isr_param   - argument passed at interrupt time
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
 */
extern osal_isrret_t
drv_nic_isr_handler(void *isr_param);


#if !defined(CONFIG_SDK_DRIVER_EXTC_NIC)
extern osal_isrret_t
drv_ntfy_isr_handler(void *isr_param);
#endif

#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
/* Function Name:
 *      drv_nic_reg_get
 * Description:
 *      Get content of NIC reg.
 * Input:
 *      unit  - unit id
 *      addr  - reg address
 * Output:
 *      pVal  - value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 */
extern int32
drv_nic_reg_get(uint32 unit, uint32 type, uint32 addr, uint32 *pVal);

/* Function Name:
 *      drv_nic_reg_set
 * Description:
 *      Set content of NIC reg.
 * Input:
 *      unit  - unit id
 *      addr  - reg address
 *      val   - value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      9310
 * Note:
 */
extern int32
drv_nic_reg_set(uint32 unit, uint32 type, uint32 addr, uint32 val);
#endif /* CONFIG_SDK_DRIVER_EXTC_NIC */

/* Function Name:
 *      drv_nic_tag_set
 * Description:
 *      Set TX tag of CPU TX packet.
 * Input:
 *      unit    - unit id
 *      tagStatus - CPU TX tag status
 *      pTxTag    - pointer of packet CPU TX tag
 *      pPortmask
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
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
drv_nic_tag_set(uint32 unit, nic_txTagStatus_t tagStatus, uint8 *pTxTag,  rtk_portmask_t *pPortmask);

/* Function Name:
 *      drv_nic_txData_set
 * Description:
 *      Set TX data of CPU TX packet TX.
 * Input:
 *      unit    - unit id
 *      isAuto - if packet data is auto generated
 *      pTxData    - pointer of packet TX data, it would be ignored if isAuto is true
 *      len   - packet size
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
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
drv_nic_txData_set(uint32 unit, uint8 isAuto, uint8 *pTxData, uint32 len);

/* Function Name:
 *      drv_nic_diagPkt_send
 * Description:
 *      Trigger to send packets from CPU.
 * Input:
 *      unit    - unit id
 *      pPortmask    - pointer to TX portmask, it would be ignored if isByTag is true
 *      num   - number of TX frame
 *      isByTag - if packet is forwarded by CPU TX tag
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
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
drv_nic_diagPkt_send(uint32 unit, uint32 num);

/* Function Name:
 *      drv_nic_loopbackMode_get
 * Description:
 *      Get the loopback mode status of user's callback example function in SDK user mode.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 */
extern int32
drv_nic_loopbackMode_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      drv_nic_loopbackMode_set
 * Description:
 *      Set user's callback example function to loopback mode in SDK user mode.
 * Input:
 *      unit    - unit id
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 */
extern int32
drv_nic_loopbackMode_set(uint32 unit, uint32 num);
#endif /* __NIC_DIAG_H__ */

