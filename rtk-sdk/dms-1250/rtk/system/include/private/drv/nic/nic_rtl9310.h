/*
 * Copyright (C) 2015 Realtek Semiconductor Corp.
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
 * Purpose : Definition those public NIC(Network Interface Controller) APIs and
 *           its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) CPU tag
 *            2) NIC tx
 *            3) NIC rx
 *
 */
#ifndef __NIC_RTL9310_H__
#define __NIC_RTL9310_H__

/*
 * Include Files
 */
#include <common/error.h>
#include <private/drv/nic/nic_common.h>
#include <drv/nic/nic.h>

/*
 * Symbol Definition
 */
#define NIC_9310_JUMBO_SIZE     (12288)
#define NIC_9310_SFLOW_RX       1
#define NIC_9310_SFLOW_TX       2


typedef struct nic_9310_cpuTag_s
{
    union {
        struct {
            uint8   CPUTAGIF:1;
            uint8       :1;
            uint8   SPA:6;
            uint8       :2;
            uint8   OF_LUMIS_TBL_ID:2;
            uint8   ACL_OF_HIT:2;
            uint8   OF_TBL_ID:2;
            uint8   SPN_IS_TRK:1;
            uint8   TRK_ID:7;
            uint8       :1;
            uint8   L2_ERR_PKT:1;
            uint8   L3_ERR_PKT:1;
            uint8   ATK_TYPE:5;
            uint16  QID:5;
            uint16      :1;
            uint16  SPN:10;
            uint16      :1;
            uint16  IDX:15;
            uint16  MIR_HIT:4;
            uint16  SFLOW:2;
            uint16  TT_HIT:1;
            uint16  TT_IDX:9;
            uint16      :1;
            uint16  OTAGIF:1;
            uint16  ITAGIF:1;
            uint16  FVID_SEL:1;
            uint16  FVID:12;
            uint8       :1;
            uint8   MAC_CST:1;
            uint8   DM_RXIDX:6;
            uint16  NEW_SA:1;
            uint16  PMV_FBD:1;
            uint16  L2_STTC_PMV:1;
            uint16  L2_DYN_PMV:1;
            uint16  OVERSIZE:1;
            uint16  PORT_DATA_IS_TRK:1;
            uint16  PORT_DATA:10;
            uint8   HASH_FULL:1;
            uint8   INVLD_SA:1;
            uint8   REASON:6;
        } __attribute__ ((aligned(1), packed)) rx;
        struct {
            uint16  CPUTAGIF:1;
            uint16      :15;
            uint8       :4;
            uint8   FWD_TYPE:4;
            uint8   ACL_ACT:1;
            uint8   CNGST_DROP:1;
            uint8   DM_PKT:1;
            uint8   DG_PKT:1;
            uint8   BP_FLTR:1;
            uint8   BP_STP:1;
            uint8   BP_VLAN_EGR:1;
            uint8   AS_TAGSTS:1;
            uint8   L3_ACT:1;
            uint8   ORI_TAGIF_EN:1;
            uint8   AS_QID:1;
            uint8   QID:5;
            uint16  ORI_ITAGIF:1;
            uint16  ORI_OTAGIF:1;
            uint16  FVID_SEL:1;
            uint16  FVID_EN:1;
            uint16  FVID:12;
            uint8   SRC_FLTR_EN:1;
            uint8   SP_IS_TRK:1;
            uint8   SPN9_4:6;
            uint32  SPN3_0:4;
            uint32  SW_DEV_ID:4;
            uint32  DPM55_32:24;
            uint32  DPM31_0;
        } __attribute__ ((aligned(1), packed)) tx;
    } un;
} nic_9310_cpuTag_t;

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Module Name    : NIC     */

/* Function Name:
 *      r9310_init
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Must initialize nic module before calling any nic APIs.
 */
extern int32
r9310_init(uint32 unit, drv_nic_initCfg_t *pInitCfg);

/* Function Name:
 *      r9310_pkt_tx
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      When fTxCb is NULL, driver will free packet and not callback any more.
 */
extern int32
r9310_pkt_tx(uint32 unit, drv_nic_pkt_t *pPacket, drv_nic_tx_cb_f fTxCb, void *pCookie);

/* Function Name:
 *      r9310_rx_start
 * Description:
 *      Start the rx action of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
extern int32
r9310_rx_start(uint32 unit);

/* Function Name:
 *      r9310_rx_stop
 * Description:
 *      Stop the rx action of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
extern int32
r9310_rx_stop(uint32 unit);

/* Function Name:
 *      r9310_rx_register
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
r9310_rx_register(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb, void *pCookie, uint32 flags);

/* Function Name:
 *      r9310_rx_unregister
 * Description:
 *      Unregister to receive callbacks for received packets of the specified device.
 * Input:
 *      unit     - unit id
 *      priority - Relative priority of the callback
 *      fRxCb    - pointer to a handler of received packets (can be null)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
r9310_rx_unregister(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb);

/* Function Name:
 *      r9310_pkt_alloc
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
 * Note:
 *      None
 */
extern int32
r9310_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket);

/* Function Name:
 *      r9310_pkt_free
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
 * Note:
 *      None
 */
extern int32
r9310_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket);

/* NIC Tx/Rx debug */
/* Function Name:
 *      r9310_debug_set
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
 * Note:
 *      There are 4 BIT flags can be selected as following:
 *      - DEBUG_RX_RAW_LEN_BIT
 *      - DEBUG_RX_CPU_TAG_BIT
 *      - DEBUG_TX_RAW_LEN_BIT
 *      - DEBUG_TX_CPU_TAG_BIT
 */
extern int32
r9310_debug_set(uint32 unit, uint32 flags);

/* Function Name:
 *      r9310_debug_get
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
 * Note:
 *      There are 4 BIT flags as following:
 *      - DEBUG_RX_RAW_LEN_BIT
 *      - DEBUG_RX_CPU_TAG_BIT
 *      - DEBUG_TX_RAW_LEN_BIT
 *      - DEBUG_TX_CPU_TAG_BIT
 */
extern int32
r9310_debug_get(uint32 unit, uint32 *pFlags);

/* Function Name:
 *      r9310_counter_dump
 * Description:
 *      Dump NIC debug counter information of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      There are 4 debug counters be dump as following:
 *      - nic_tx_success_cntr
 *      - nic_tx_failed_cntr
 *      - nic_rx_success_cntr
 *      - nic_rx_failed_cntr
 */
extern int32
r9310_counter_dump(uint32 unit);

/* Function Name:
 *      r9310_counter_clear
 * Description:
 *      Clear NIC debug counter information of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Clear following NIC debug counters
 *      - nic_tx_success_cntr
 *      - nic_tx_failed_cntr
 *      - nic_rx_success_cntr
 *      - nic_rx_failed_cntr
 */
extern int32
r9310_counter_clear(uint32 unit);

/* Function Name:
 *      r9310_bufStatus_dump
 * Description:
 *      Dump NIC buffer status of the specified device.
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
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
r9310_bufStatus_dump(uint32 unit);

/* Function Name:
 *      r9310_pkthdrMbuf_dump
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
r9310_pkthdrMbuf_dump(uint32 unit, uint32 mode, uint32 start, uint32 end, uint32 flags);

/* Function Name:
 *      r9310_rxStatus_get
 * Description:
 *      Get NIC rx status of the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pStatus - rx status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
extern int32
r9310_rxStatus_get(uint32 unit, uint32 *pStatus);

/* Function Name:
 *      r9310_isr_handler
 * Description:
 *      Nic interrupt handle routine.
 * Input:
 *      intr_status - isr status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      OSAL_INT_HANDLED
 * Note:
 *      None
 */
extern int32
r9310_isr_handler(uint32 intr_status);

extern int32
r9310_ringInfo_get(uint32 unit, nic_dir_t type, uint32 **rxRingIdSizeList, uint32 *pRingNum);

extern int32
r9310_cpuPortTxRxEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
r9310_intrMask_get(uint32 unit, nic_intr_type_t type, uint32 *pMask);

extern int32
r9310_intrMask_set(uint32 unit, nic_intr_type_t type, uint32 mask);

extern int32
r9310_intrSts_get(uint32 unit, nic_intr_type_t type, uint32 *pVal);

extern int32
r9310_intrSts_set(uint32 unit, nic_intr_type_t type, uint32 val);

extern int32
r9310_swNicRst_get(uint32 unit, uint32 *pStatus);

extern int32
r9310_swNicRst_set(uint32 unit);

extern int32
r9310_swQueRst_get(uint32 unit, uint32 *pStatus);

extern int32
r9310_swQueRst_set(uint32 unit);

extern int32
r9310_cpuL2FloodMask_add(uint32 unit);

extern int32
r9310_cpuL2FloodMask_remove(uint32 unit);

extern int32
r9310_cpuForceLinkupEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
r9310_rxRingSize_get(uint32 unit, uint32 ring, uint32 *pVal);

extern int32
r9310_holRingSize_set(uint32 unit, uint32 ring, uint32 val);

extern int32
r9310_rxRingCnt_get(uint32 unit, uint32 ring, uint32 *pVal);

extern int32
r9310_holRingCnt_get(uint32 unit, uint32 ring, uint32 *pVal);

extern int32
r9310_holRingCnt_set(uint32 unit, uint32 ring, uint32 val);

extern int32
r9310_ntfyBaseAddr_get(uint32 unit, uintptr *pVal);

extern int32
r9310_ntfyBaseAddr_set(uint32 unit, uintptr val);

extern int32
r9310_ringBaseAddr_get(uint32 unit, nic_dir_t dir, uint32 ring, uintptr *pVal);

extern int32
r9310_ringBaseAddr_set(uint32 unit, nic_dir_t dir, uint32 ring, uintptr val);

extern int32
r9310_ringCurAddr_get(uint32 unit, nic_dir_t dir, uint32 ring, uintptr *pVal);

extern int32
r9310_rxTruncateLength_get(uint32 unit, uint32 *pVal);

extern int32
r9310_rxTruncateLength_set(uint32 unit, uint32 val);

extern int32
r9310_nicEnable_get(uint32 unit, nic_dir_t dir, uint32 *pVal);

extern int32
r9310_nicEnable_set(uint32 unit, nic_dir_t dir, uint32 val);

extern int32
r9310_nicTxFetch_set(uint32 unit, nic_txRing_t type, uint32 val);

extern int32
r9310_nicTxBusySts_get(uint32 unit, nic_txRing_t type, uint32 *pVal);

extern int32
r9310_cpuTagId_get(uint32 unit, uint32 *pVal);

extern int32
r9310_cpuTagId_set(uint32 unit, uint32 val);

extern int32
r9310_supportJumboSize_get(uint32 unit, uint32 *pVal);

extern int32
r9310_cpuTagFromRaw_cnvt(uint32 unit, uint8 *pCpuTag, drv_nic_pkt_t *pPacket);

extern int32
r9310_cpuTagToRaw_cnvt(uint32 unit, drv_nic_pkt_t *pPacket, uint8 *pCpuTag);

extern int32
r9310_cpuTag_dump(uint32 unit, drv_nic_pkt_t *pPacket);

extern int32
r9310_rawTag_dump(uint32 unit, uint8 *pCpuTag);
#endif /* __NIC_RTL9310_H__ */

