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

/*
 * Include Files
 */
#include <common/error.h>
#include <common/debug/rt_log.h>
#include <ioal/mem32.h>
#include <private/drv/nic/nic_rtl8380.h>
#include <private/drv/swcore/swcore_rtl8380.h>
#include <drv/nic/nic.h>
#include <osal/lib.h>
#include <osal/time.h>
#include <common/rt_type.h>

/*
 * Symbol Definition
 */


/*
 * Data Type Definition
 */


/*
 * Data Declaration
 */
static uint32 		reasonTbl[][2] =
{
    {0,  											0, },
    {NIC_RX_REASON_RLDP_RLPP,						0, },
    {NIC_RX_REASON_RMA,								0, },
    {NIC_RX_REASON_IGR_VLAN_FILTER,					0, },
    {NIC_RX_REASON_INNER_OUTTER_CFI,				0, },
    {NIC_RX_REASON_MY_MAC,							0, },
    {NIC_RX_REASON_SPECIAL_TRAP,					0, },
    {NIC_RX_REASON_SPECIAL_COPY,					0, },
    {NIC_RX_REASON_ROUTING_EXCEPTION,				0, },
    {NIC_RX_REASON_UNKWN_UCST_MCST,					0, },
    {NIC_RX_REASON_MAC_CONSTRAINT_SYS,				NIC_RX_REASON_MAC_CONSTRAINT, },
    {NIC_RX_REASON_MAC_CONSTRAINT_VLAN,				NIC_RX_REASON_MAC_CONSTRAINT, },
    {NIC_RX_REASON_MAC_CONSTRAINT_PORT,				NIC_RX_REASON_MAC_CONSTRAINT, },
    {NIC_RX_REASON_CRC_ERROR,						0, },
    {NIC_RX_REASON_IP6_UNKWN_EXT_HDR,				0, },
    {NIC_RX_REASON_NORMAL_FWD,						0, },
};


/*QueueId:                             0                               8                       16                      24                   31 */
static uint32   rxRingIdSize_8380[] = {32, 32, 32, 32, 32, 32, 32, 32   };
static uint32   txRingIdSize_8380[] = {128, 128};
static uint8    nic_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

static int32 _nic_rx_reason_translate(drv_nic_pkt_t *pPacket, nic_8380_cpuTag_t *pCpuTag)
{
#define RTL8380_MAX_REASON_NM  15
    uint16 reason = pCpuTag->un.rx.REASON;

    if (pCpuTag->un.rx.ACL_HIT == 1)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ACL_HIT);
    if (pCpuTag->un.rx.MAC_CST)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_MAC_CONSTRAINT);
    if (pCpuTag->un.rx.NEW_SA)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_NEW_SA);
    if (pCpuTag->un.rx.L2_PMV)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_PMV);
    if (pCpuTag->un.rx.ATK_HIT)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ATTACK);
    if (pCpuTag->un.rx.MIR_HIT)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_MIRROR);

    if (reason <= RTL8380_MAX_REASON_NM)
    {
        NIC_REASON_MASK_SET(*pPacket, reasonTbl[reason][0]);
        NIC_REASON_MASK_SET(*pPacket, reasonTbl[reason][1]);
    }

    return RT_ERR_OK;
}

static int32
_removeCPUTag(uint8 *pPkt, uint8 **ppNewPkt, uint32 *pOffset, nic_8380_cpuTag_t *pCpuTag)
{
    if ((NULL == pPkt) || (NULL == ppNewPkt) || (NULL == pOffset) || (NULL == pCpuTag))
        return RT_ERR_FAILED;

    /* cpuTag copy: offset 14 bytes = DA(6) + SA(6)+ CPU_TAG_ID(2) */
    osal_memcpy((uint8 *)pCpuTag, (uint8 *)(pPkt + 14), sizeof(nic_8380_cpuTag_t));

    /* fix the packet */
    *(pPkt + 23) = *(pPkt + 11);
    *(pPkt + 22) = *(pPkt + 10);
    *(pPkt + 21) = *(pPkt + 9);
    *(pPkt + 20) = *(pPkt + 8);
    *(pPkt + 19) = *(pPkt + 7);
    *(pPkt + 18) = *(pPkt + 6);
    *(pPkt + 17) = *(pPkt + 5);
    *(pPkt + 16) = *(pPkt + 4);
    *(pPkt + 15) = *(pPkt + 3);
    *(pPkt + 14) = *(pPkt + 2);
    *(pPkt + 13)  = *(pPkt + 1);
    *(pPkt + 12)  = *(pPkt + 0);

    if (ppNewPkt)
    {
        *ppNewPkt = (pPkt + (CPU_TAG_ID_LEN + sizeof(nic_8380_cpuTag_t)));
    }

    if (pOffset)
    {
        *pOffset = (CPU_TAG_ID_LEN + sizeof(nic_8380_cpuTag_t));
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      r8380_init
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
int32
r8380_init(uint32 unit, drv_nic_initCfg_t *pInitCfg)
{
    RT_INIT_REENTRY_CHK(nic_init[unit]);
    nic_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}


/* Function Name:
 *      r8380_pkt_tx
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
int32
r8380_pkt_tx(uint32 unit, drv_nic_pkt_t *pPacket, drv_nic_tx_cb_f fTxCb, void *pCookie)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r8380_rx_start
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
int32
r8380_rx_start(uint32 unit)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r8380_rx_stop
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
int32
r8380_rx_stop(uint32 unit)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r8380_rx_register
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
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
r8380_rx_register(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb, void *pCookie, uint32 flags)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r8380_rx_unregister
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
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
r8380_rx_unregister(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r8380_pkt_alloc
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
int32
r8380_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r8380_pkt_free
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
int32
r8380_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket)
{
    return RT_ERR_OK;
}

/* NIC Tx/Rx debug */
/* Function Name:
 *      r8380_debug_set
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
int32
r8380_debug_set(uint32 unit, uint32 flags)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r8380_debug_get
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
int32
r8380_debug_get(uint32 unit, uint32 *pFlags)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r8380_counter_dump
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
int32
r8380_counter_dump(uint32 unit)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r8380_counter_clear
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
int32
r8380_counter_clear(uint32 unit)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r8380_bufStatus_dump
 * Description:
 *      Dump NIC buffer status of the specified device.
 * Input:
 *      unit - unit id
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
 *      2) From HW View
 *      - Rx Ring Packet Header(CDPIdx)
 *      - Tx Ring Packet Header(CDPIdx)
 *      3) Register Information
 *      - CPUIIMR (CPU Interface Interrupt Mask Register)
 *      - CPUIISR (CPU Interface Interrupt Status Register)
 *      - CPUICR  (CPU Interface Control Register)
 */
int32
r8380_bufStatus_dump(uint32 unit)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r8380_pkthdrMbuf_dump
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
int32
r8380_pkthdrMbuf_dump(uint32 unit, uint32 mode, uint32 start, uint32 end, uint32 flags)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r8380_rxStatus_get
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
int32
r8380_rxStatus_get(uint32 unit, uint32 *pStatus)
{
    return RT_ERR_OK;
}

int32 r8380_ringInfo_get(uint32 unit, nic_dir_t type, uint32 **ringIdSizeList, uint32 *pRingNum)
{
    if (type == NIC_DIR_RX)
    {
        *ringIdSizeList = rxRingIdSize_8380;
        *pRingNum = NIC_RXRING_NUM;
    }
    else
    {
        *ringIdSizeList = txRingIdSize_8380;
        *pRingNum = NIC_TXRING_NUM;
    }

    return RT_ERR_OK;
}

int32 r8380_cpuPortTxRxEnable_set(uint32 unit, rtk_enable_t enable)
{
    if (enable)
    {
        ioal_mem32_field_write(unit, RTL8380_MAC_PORT_CTRL_ADDR(28), RTL8380_MAC_PORT_CTRL_TXRX_EN_OFFSET, RTL8380_MAC_PORT_CTRL_TXRX_EN_MASK, 0x3);
        ioal_mem32_field_write(unit, RTL8380_MAC_PORT_CTRL_ADDR(28), \
                                RTL8380_MAC_PORT_CTRL_RX_CHK_CRC_EN_OFFSET, RTL8380_MAC_PORT_CTRL_RX_CHK_CRC_EN_MASK, 1);
    }
    else
    {
        ioal_mem32_field_write(unit, RTL8380_MAC_PORT_CTRL_ADDR(28), RTL8380_MAC_PORT_CTRL_TXRX_EN_OFFSET, RTL8380_MAC_PORT_CTRL_TXRX_EN_MASK, 0);
    }

    return RT_ERR_OK;
}

int32 r8380_intrMask_get(uint32 unit, nic_intr_type_t type, uint32 *pMask)
{
    if (type == NIC_RX_DONE)
        ioal_mem32_field_read(unit, RTL8380_DMA_IF_INTR_MSK_ADDR, RTL8380_DMA_IF_INTR_MSK_RX_DONE_OFFSET, RTL8380_DMA_IF_INTR_MSK_RX_DONE_MASK, pMask);
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_field_read(unit, RTL8380_DMA_IF_INTR_MSK_ADDR, RTL8380_DMA_IF_INTR_MSK_RX_RUN_OUT_OFFSET, RTL8380_DMA_IF_INTR_MSK_RX_RUN_OUT_MASK, pMask);
    else if (type == NIC_TX_DONE)
        ioal_mem32_field_read(unit, RTL8380_DMA_IF_INTR_MSK_ADDR, RTL8380_DMA_IF_INTR_MSK_TX_DONE_OFFSET, RTL8380_DMA_IF_INTR_MSK_TX_DONE_MASK, pMask);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_field_read(unit, RTL8380_DMA_IF_INTR_MSK_ADDR, RTL8380_DMA_IF_INTR_MSK_TX_ALL_DONE_OFFSET, RTL8380_DMA_IF_INTR_MSK_TX_ALL_DONE_MASK, pMask);
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    return RT_ERR_OK;
}

int32 r8380_intrMask_set(uint32 unit, nic_intr_type_t type, uint32 mask)
{
    if (type == NIC_RX_DONE)
    {
        mask &= 0xff;
        ioal_mem32_field_write(unit, RTL8380_DMA_IF_INTR_MSK_ADDR, RTL8380_DMA_IF_INTR_MSK_RX_DONE_OFFSET, RTL8380_DMA_IF_INTR_MSK_RX_DONE_MASK, mask);
    }
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_field_write(unit, RTL8380_DMA_IF_INTR_MSK_ADDR, RTL8380_DMA_IF_INTR_MSK_RX_RUN_OUT_OFFSET, RTL8380_DMA_IF_INTR_MSK_RX_RUN_OUT_MASK, mask);
    else if (type == NIC_TX_DONE)
        ioal_mem32_field_write(unit, RTL8380_DMA_IF_INTR_MSK_ADDR, RTL8380_DMA_IF_INTR_MSK_TX_DONE_OFFSET, RTL8380_DMA_IF_INTR_MSK_TX_DONE_MASK, mask);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_field_write(unit, RTL8380_DMA_IF_INTR_MSK_ADDR, RTL8380_DMA_IF_INTR_MSK_TX_ALL_DONE_OFFSET, RTL8380_DMA_IF_INTR_MSK_TX_ALL_DONE_MASK, mask);
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    return RT_ERR_OK;
}

int32 r8380_intrSts_get(uint32 unit, nic_intr_type_t type, uint32 *pVal)
{
    if (type == NIC_RX_DONE)
        ioal_mem32_field_read(unit, RTL8380_DMA_IF_INTR_STS_ADDR, RTL8380_DMA_IF_INTR_STS_RX_DONE_OFFSET, RTL8380_DMA_IF_INTR_STS_RX_DONE_MASK, pVal);
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_field_read(unit, RTL8380_DMA_IF_INTR_STS_ADDR, RTL8380_DMA_IF_INTR_STS_RX_RUN_OUT_OFFSET, RTL8380_DMA_IF_INTR_STS_RX_RUN_OUT_MASK, pVal);
    else if (type == NIC_TX_DONE)
        ioal_mem32_field_read(unit, RTL8380_DMA_IF_INTR_STS_ADDR, RTL8380_DMA_IF_INTR_STS_TX_DONE_OFFSET, RTL8380_DMA_IF_INTR_STS_TX_DONE_MASK, pVal);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_field_read(unit, RTL8380_DMA_IF_INTR_STS_ADDR, RTL8380_DMA_IF_INTR_STS_TX_ALL_DONE_OFFSET, RTL8380_DMA_IF_INTR_STS_TX_ALL_DONE_MASK, pVal);
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    return RT_ERR_OK;
}

int32 r8380_intrSts_set(uint32 unit, nic_intr_type_t type, uint32 val)
{
    /* Note: Do NOT use ioal_mem32_field_read() on intr status reg */

    if (type == NIC_RX_DONE)
    {
        val &= 0xff;
        ioal_mem32_write(unit, RTL8380_DMA_IF_INTR_STS_ADDR, (val << RTL8380_DMA_IF_INTR_STS_RX_DONE_OFFSET) & RTL8380_DMA_IF_INTR_STS_RX_DONE_MASK);
    }
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_write(unit, RTL8380_DMA_IF_INTR_STS_ADDR, (val << RTL8380_DMA_IF_INTR_STS_RX_RUN_OUT_OFFSET) & RTL8380_DMA_IF_INTR_STS_RX_RUN_OUT_MASK);
    else if (type == NIC_TX_DONE)
        ioal_mem32_write(unit, RTL8380_DMA_IF_INTR_STS_ADDR, (val << RTL8380_DMA_IF_INTR_STS_TX_DONE_OFFSET) & RTL8380_DMA_IF_INTR_STS_TX_DONE_MASK);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_write(unit, RTL8380_DMA_IF_INTR_STS_ADDR, (val << RTL8380_DMA_IF_INTR_STS_TX_ALL_DONE_OFFSET) & RTL8380_DMA_IF_INTR_STS_TX_ALL_DONE_MASK);
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    return RT_ERR_OK;
}

int32 r8380_swNicRst_get(uint32 unit, uint32 *pStatus)
{
    ioal_mem32_field_read(unit, RTL8380_RST_GLB_CTRL_0_ADDR, RTL8380_RST_GLB_CTRL_0_SW_NIC_RST_OFFSET, RTL8380_RST_GLB_CTRL_0_SW_NIC_RST_MASK, pStatus);
    return RT_ERR_OK;
}

int32 r8380_swNicRst_set(uint32 unit)
{
    RT_LOG(LOG_DEBUG, MOD_NIC, "Reset NIC (R8380)... ");
    ioal_mem32_field_write(unit, RTL8380_RST_GLB_CTRL_0_ADDR, RTL8380_RST_GLB_CTRL_0_SW_NIC_RST_OFFSET, RTL8380_RST_GLB_CTRL_0_SW_NIC_RST_MASK, 1);
    osal_time_usleep(50 * 1000); /* delay 50mS */
    return RT_ERR_OK;
}

int32 r8380_swQueRst_get(uint32 unit, uint32 *pStatus)
{
    ioal_mem32_field_read(unit, RTL8380_RST_GLB_CTRL_0_ADDR, RTL8380_RST_GLB_CTRL_0_SW_Q_RST_OFFSET, RTL8380_RST_GLB_CTRL_0_SW_Q_RST_MASK, pStatus);

    return RT_ERR_OK;
}

int32 r8380_swQueRst_set(uint32 unit)
{
    ioal_mem32_field_write(unit, RTL8380_RST_GLB_CTRL_0_ADDR, RTL8380_RST_GLB_CTRL_0_SW_Q_RST_OFFSET, RTL8380_RST_GLB_CTRL_0_SW_Q_RST_MASK, 1);
    osal_time_usleep(50 * 1000); /* delay 50mS */

    return RT_ERR_OK;
}
int32 r8380_cpuL2FloodMask_add(uint32 unit)
{
    uint32 val;

    ioal_mem32_write(unit, RTL8380_TBL_ACCESS_L2_CTRL_ADDR, 0x28000);
    ioal_mem32_read(unit, RTL8380_TBL_ACCESS_L2_DATA_ADDR(0), &val);
    val |= 0x80000000;
    ioal_mem32_write(unit, RTL8380_TBL_ACCESS_L2_DATA_ADDR(0), val);
    ioal_mem32_write(unit, RTL8380_TBL_ACCESS_L2_CTRL_ADDR, 0x38000);
    return RT_ERR_OK;
}

int32 r8380_cpuL2FloodMask_remove(uint32 unit)
{
    uint32 val;

    /* Remove the CPU port from Lookup Miss Flooding Portmask */
    ioal_mem32_write(unit, RTL8380_TBL_ACCESS_L2_CTRL_ADDR, 0x28000);
    ioal_mem32_read(unit, RTL8380_TBL_ACCESS_L2_DATA_ADDR(0), &val);
    val &= ~(0x80000000);
    ioal_mem32_write(unit, RTL8380_TBL_ACCESS_L2_DATA_ADDR(0), val);
    ioal_mem32_write(unit, RTL8380_TBL_ACCESS_L2_CTRL_ADDR, 0x38000);
    return RT_ERR_OK;
}

int32 r8380_cpuForceLinkupEnable_set(uint32 unit, rtk_enable_t enable)
{
    if (enable)
        ioal_mem32_write(unit, RTL8380_MAC_FORCE_MODE_CTRL_ADDR(28), 0x6192F);
    else
        ioal_mem32_write(unit, RTL8380_MAC_FORCE_MODE_CTRL_ADDR(28), 0x6192c);
    return RT_ERR_OK;
}

int32 r8380_holRingSize_set(uint32 unit, uint32 ring, uint32 val)
{
    ioal_mem32_field_write(unit, RTL8380_DMA_IF_RX_RING_SIZE_ADDR(ring), RTL8380_DMA_IF_RX_RING_SIZE_SIZE_OFFSET(ring),
                                RTL8380_DMA_IF_RX_RING_SIZE_SIZE_MASK(ring), val);
    return RT_ERR_OK;
}

int32 r8380_holRingCnt_get(uint32 unit, uint32 ring, uint32 *pVal)
{
    uint32 tmpVal;

    ioal_mem32_read(unit, RTL8380_DMA_IF_RX_RING_CNTR_ADDR(ring), &tmpVal);
    *pVal = (tmpVal >> RTL8380_DMA_IF_RX_RING_CNTR_CNTR_OFFSET(ring)) & RTL8380_DMA_IF_RX_RING_CNTR_CNTR_MASK(ring);
    return RT_ERR_OK;
}

int32 r8380_holRingCnt_set(uint32 unit, uint32 ring, uint32 val)
{
    /*Note that other ring counter shoud not be reduced */
    ioal_mem32_write(unit, RTL8380_DMA_IF_RX_RING_CNTR_ADDR(ring), \
            ((val << RTL8380_DMA_IF_RX_RING_CNTR_CNTR_OFFSET(ring)) & RTL8380_DMA_IF_RX_RING_CNTR_CNTR_MASK(ring)));
    return RT_ERR_OK;
}

int32 r8380_ntfyBaseAddr_get(uint32 unit, uint32 *pVal)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

int32 r8380_ntfyBaseAddr_set(uint32 unit, uint32 val)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

int32 r8380_ringBaseAddr_get(uint32 unit, nic_dir_t dir, uint32 ring, uint32 *pVal)
{
    if (dir == NIC_DIR_RX)
        ioal_mem32_read(unit, RTL8380_DMA_IF_RX_BASE_DESC_ADDR_CTRL_ADDR(ring), pVal);
    else
        ioal_mem32_read(unit, RTL8380_DMA_IF_TX_BASE_DESC_ADDR_CTRL_ADDR(ring), pVal);
    return RT_ERR_OK;
}

int32 r8380_ringBaseAddr_set(uint32 unit, nic_dir_t dir, uint32 ring, uint32 val)
{
    if (dir == NIC_DIR_RX)
        ioal_mem32_write(unit, RTL8380_DMA_IF_RX_BASE_DESC_ADDR_CTRL_ADDR(ring), val);
    else
        ioal_mem32_write(unit, RTL8380_DMA_IF_TX_BASE_DESC_ADDR_CTRL_ADDR(ring), val);
    return RT_ERR_OK;
}

int32 r8380_ringCurAddr_get(uint32 unit, nic_dir_t dir, uint32 ring, uint32 *pVal)
{
    if (dir == NIC_DIR_RX)
        ioal_mem32_read(unit, RTL8380_DMA_IF_RX_CUR_DESC_ADDR_CTRL_ADDR(ring), pVal);
    else
        ioal_mem32_read(unit, RTL8380_DMA_IF_TX_CUR_DESC_ADDR_CTRL_ADDR(ring), pVal);
    return RT_ERR_OK;
}

int32 r8380_rxTruncateLength_get(uint32 unit, uint32 *pVal)
{
    ioal_mem32_field_read(unit, RTL8380_DMA_IF_CTRL_ADDR, RTL8380_DMA_IF_CTRL_RX_TRUNCATE_LEN_OFFSET,
                                                          RTL8380_DMA_IF_CTRL_RX_TRUNCATE_LEN_MASK, pVal);
    return RT_ERR_OK;
}

int32 r8380_rxTruncateLength_set(uint32 unit, uint32 val)
{
    ioal_mem32_field_write(unit, RTL8380_DMA_IF_CTRL_ADDR, RTL8380_DMA_IF_CTRL_RX_TRUNCATE_LEN_OFFSET,
                                                          RTL8380_DMA_IF_CTRL_RX_TRUNCATE_LEN_MASK, val);
    return RT_ERR_OK;
}

int32 r8380_nicEnable_get(uint32 unit, nic_dir_t dir, uint32 *pVal)
{
    if (dir == NIC_DIR_RX)
    {
        ioal_mem32_field_read(unit, RTL8380_DMA_IF_CTRL_ADDR, RTL8380_DMA_IF_CTRL_RX_EN_OFFSET,
                                                              RTL8380_DMA_IF_CTRL_RX_EN_MASK, pVal);
    }
    else
    {
        ioal_mem32_field_read(unit, RTL8380_DMA_IF_CTRL_ADDR, RTL8380_DMA_IF_CTRL_TX_EN_OFFSET,
                                                              RTL8380_DMA_IF_CTRL_TX_EN_MASK, pVal);
    }
    return RT_ERR_OK;
}

int32 r8380_nicEnable_set(uint32 unit, nic_dir_t dir, uint32 val)
{
    if (dir == NIC_DIR_RX)
    {
        ioal_mem32_field_write(unit, RTL8380_DMA_IF_CTRL_ADDR, RTL8380_DMA_IF_CTRL_RX_EN_OFFSET,
                                                              RTL8380_DMA_IF_CTRL_RX_EN_MASK, val);
    }
    else
    {
        ioal_mem32_field_write(unit, RTL8380_DMA_IF_CTRL_ADDR, RTL8380_DMA_IF_CTRL_TX_EN_OFFSET,
                                                              RTL8380_DMA_IF_CTRL_TX_EN_MASK, val);
    }

    if (val)
        RT_LOG(LOG_DEBUG, MOD_NIC, "NIC (R8380) Rx Start... ");
    else
        RT_LOG(LOG_DEBUG, MOD_NIC, "NIC (R8380) Rx Stop... ");

    return RT_ERR_OK;
}

int32 r8380_nicTxFetch_set(uint32 unit, nic_txRing_t type, uint32 val)
{
    {   /*Chig bug, sometimes lextra bus will access wrong data*/
        uint32 times;
        uint32 cnt_val;
        times = 0;
        while(times < 10)
        {
            ioal_mem32_read(unit, RTL8380_DMA_IF_CTRL_ADDR, &cnt_val);
            if((cnt_val & 0xc) == 0xc)break;
            times++;
        }
    }

    ioal_mem32_field_write(unit, RTL8380_DMA_IF_CTRL_ADDR, RTL8380_DMA_IF_CTRL_TX_FETCH_OFFSET,
                                                          RTL8380_DMA_IF_CTRL_TX_FETCH_MASK, val);
    return RT_ERR_OK;
}

int32 r8380_nicTxBusySts_get(uint32 unit, nic_txRing_t type, uint32 *pVal)
{
    ioal_mem32_field_read(unit, RTL8380_DMA_IF_CTRL_ADDR, RTL8380_DMA_IF_CTRL_TX_BUSY_OFFSET,
                                                          RTL8380_DMA_IF_CTRL_TX_BUSY_MASK, pVal);
    return RT_ERR_OK;
}

int32 r8380_cpuTagId_get(uint32 unit, uint32 *pVal)
{
    ioal_mem32_field_read(unit, RTL8380_MAC_CPU_TAG_ID_CTRL_ADDR, RTL8380_MAC_CPU_TAG_ID_CTRL_CPU_TAG_ID_OFFSET,
                                                          RTL8380_MAC_CPU_TAG_ID_CTRL_CPU_TAG_ID_MASK, pVal);
    return RT_ERR_OK;
}

int32 r8380_cpuTagId_set(uint32 unit, uint32 val)
{
    ioal_mem32_field_write(unit, RTL8380_MAC_CPU_TAG_ID_CTRL_ADDR, RTL8380_MAC_CPU_TAG_ID_CTRL_CPU_TAG_ID_OFFSET,
                                                          RTL8380_MAC_CPU_TAG_ID_CTRL_CPU_TAG_ID_MASK, val);
    return RT_ERR_OK;
}

int32 r8380_supportJumboSize_get(uint32 unit, uint32 *pVal)
{
    *pVal = NIC_8380_JUMBO_SIZE;
    return RT_ERR_OK;
}

int32 r8380_cpuTagFromRaw_cnvt(uint32 unit, uint8 *pCpuTag, drv_nic_pkt_t *pPacket)
{
    nic_8380_cpuTag_t   *pHdrtag = (nic_8380_cpuTag_t *)pCpuTag;
    uint32              offset = 0, scan_offset = 0;
    uint8               pkt_data;
    uint8               *pData;
    uint32              ethType;

    RT_PARAM_CHK(NULL == pCpuTag, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pPacket, RT_ERR_NULL_POINTER);

    if (pHdrtag->un.rx.CPUTAGIF)
    {
        _nic_rx_reason_translate(pPacket, pHdrtag);
        pPacket->rx_tag.qid             = pHdrtag->un.rx.QID;
        pPacket->rx_tag.source_port     = pHdrtag->un.rx.SPN;
        pPacket->rx_tag.mirror_hit      = pHdrtag->un.rx.MIR_HIT;
        pPacket->rx_tag.acl_index       = pHdrtag->un.rx.ACL_IDX;
        pPacket->rx_tag.svid_tagged     = pHdrtag->un.rx.OTAGIF;
        pPacket->rx_tag.cvid_tagged     = pHdrtag->un.rx.ITAGIF;
        pPacket->rx_tag.fvid            = pHdrtag->un.rx.RVID;
        pPacket->rx_tag.atk_type        = IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ATTACK) ? pHdrtag->un.rx.ATK_TYPE : 0;
        pPacket->rx_tag.reason          = pHdrtag->un.rx.REASON;
    }
    else
    {
        nic_8380_cpuTag_t cputag;

        scan_offset = 12;
        pData = pPacket->data + scan_offset;
        ethType = READ16(pData);
        pkt_data = *(pData + 2);

        if (ethType == REALTEK_CPUTAG_ID && pkt_data == 0x04)    /* embedded CPU Tag */
        {
            if (RT_ERR_OK != _removeCPUTag(pPacket->data, &pPacket->data, &offset, &cputag))
            {
                return RT_ERR_FAILED;
            }

            _nic_rx_reason_translate(pPacket, &cputag);
            pPacket->rx_tag.source_port     = cputag.un.rx.SPN;
            pPacket->rx_tag.mirror_hit      = cputag.un.rx.MIR_HIT;
            pPacket->rx_tag.acl_index       = cputag.un.rx.ACL_IDX;
            pPacket->rx_tag.svid_tagged     = cputag.un.rx.OTAGIF;
            pPacket->rx_tag.cvid_tagged     = cputag.un.rx.ITAGIF;
            pPacket->rx_tag.fvid            = cputag.un.rx.RVID;
            pPacket->rx_tag.qid             = cputag.un.rx.QID;
        pPacket->rx_tag.atk_type        = IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ATTACK) ? pHdrtag->un.rx.ATK_TYPE : 0;
            pPacket->rx_tag.reason          = cputag.un.rx.REASON;
        }
    }

    return RT_ERR_OK;
}

int32 r8380_cpuTagToRaw_cnvt(uint32 unit, drv_nic_pkt_t *pPacket, uint8 *pCpuTag)
{
    nic_8380_cpuTag_t   *pHdrtag = (nic_8380_cpuTag_t *)pCpuTag;

    RT_PARAM_CHK(NULL == pCpuTag, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pPacket, RT_ERR_NULL_POINTER);

    if (pPacket->as_txtag)
    {
        pHdrtag->un.tx.CPUTAGIF     = 0x4; /*PROTO ID*/
        pHdrtag->un.tx.BP_FLTR1     = pPacket->tx_tag.bp_fltr1;
        pHdrtag->un.tx.BP_FLTR2     = pPacket->tx_tag.bp_fltr2;
        pHdrtag->un.tx.AS_TAGSTS    = pPacket->tx_tag.as_tagSts;
        pHdrtag->un.tx.ACL_ACT      = pPacket->tx_tag.acl_act;
        pHdrtag->un.tx.RVID_SEL     = pPacket->tx_tag.fvid_sel;
        pHdrtag->un.tx.L2LEARNING   = pPacket->tx_tag.l2_learning;
        pHdrtag->un.tx.AS_PRI       = pPacket->tx_tag.as_priority;
        pHdrtag->un.tx.PRI          = pPacket->tx_tag.priority;
        if (pPacket->tx_tag.fwd_type == NIC_FWD_TYPE_ALE)
            pHdrtag->un.tx.AS_DPM = 0;
        else if (pPacket->tx_tag.fwd_type == NIC_FWD_TYPE_LOGICAL)
        {
            pHdrtag->un.tx.AS_DPM = 1;
            pHdrtag->un.tx.DPM_TYPE = 0;
        }
        else if (pPacket->tx_tag.fwd_type == NIC_FWD_TYPE_PHYISCAL)
        {
            pHdrtag->un.tx.AS_DPM = 1;
            pHdrtag->un.tx.DPM_TYPE = 1;
        }
        else
        {
            osal_printf("Error: fwd_type:%d is not supported on RTL8380!\n", pPacket->tx_tag.fwd_type);
            return RT_ERR_INPUT;
        }
        pHdrtag->un.tx.DPM          = pPacket->tx_tag.dst_port_mask;
    }
    else
        pHdrtag->un.tx.CPUTAGIF = FALSE;

    return RT_ERR_OK;
}

int32 r8380_cpuTag_dump(uint32 unit, drv_nic_pkt_t *pPacket)
{
    osal_printf("=== [NIC RX Debug - CPU Rx Tag Information] ============ \n");
    osal_printf(" SPN : %d \n", pPacket->rx_tag.source_port);
    osal_printf(" MIR_HIT : %d \n", pPacket->rx_tag.mirror_hit);
    osal_printf(" ACL_HIT : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ACL_HIT));
    osal_printf(" ACL_IDX : %d \n", pPacket->rx_tag.acl_index);
    osal_printf(" OTAGIF : %d \n", pPacket->rx_tag.svid_tagged);
    osal_printf(" ITAGIF : %d \n", pPacket->rx_tag.cvid_tagged);
    osal_printf(" OVID : %d \n", pPacket->rx_tag.outer_vid);
    osal_printf(" IVID : %d \n", pPacket->rx_tag.inner_vid);
    osal_printf(" FVID : %d \n", pPacket->rx_tag.fvid);
    osal_printf(" QID : %d \n", pPacket->rx_tag.qid);
    osal_printf(" ATK_HIT : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ATTACK));
    osal_printf(" ATK_TYPE : %d \n", pPacket->rx_tag.atk_type);
    osal_printf(" MAC_CST : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_MAC_CONSTRAINT));
    osal_printf(" NEW_SA : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_NEW_SA));
    osal_printf(" L2_PMV : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_PMV));
    osal_printf(" REASON : %d \n", pPacket->rx_tag.reason);

    return RT_ERR_OK;
}

int32 r8380_rawTag_dump(uint32 unit, uint8 *pCpuTag)
{
    nic_8380_cpuTag_t   *pHdrtag = (nic_8380_cpuTag_t *)pCpuTag;

    osal_printf("=== [NIC TX Debug - CPU Tx Tag Information] ============ \n");
    osal_printf(" BP_FLTR1 : 0x%0x \n", pHdrtag->un.tx.BP_FLTR1);
    osal_printf(" BP_FLTR2 : 0x%0x \n", pHdrtag->un.tx.BP_FLTR2);
    osal_printf(" AS_TAGSTS : 0x%0x \n", pHdrtag->un.tx.AS_TAGSTS);
    osal_printf(" ACL_ACT : 0x%0x \n", pHdrtag->un.tx.ACL_ACT);
    osal_printf(" RVID_SEL : 0x%0x \n", pHdrtag->un.tx.RVID_SEL);
    osal_printf(" L2LEARNING : 0x%0x \n", pHdrtag->un.tx.L2LEARNING);
    osal_printf(" AS_PRI : 0x%0x \n", pHdrtag->un.tx.AS_PRI);
    osal_printf(" PRI : 0x%0x \n", pHdrtag->un.tx.PRI);
    osal_printf(" DPM_TYPE : 0x%0x \n", pHdrtag->un.tx.DPM_TYPE);
    osal_printf(" AS_DPM : 0x%0x \n", pHdrtag->un.tx.AS_DPM);
    osal_printf(" DPM : 0x%0x \n", pHdrtag->un.tx.DPM);

    return RT_ERR_OK;
}

