/*
 * Copyright (C) 2015-2022 Realtek Semiconductor Corp.
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
#include <ioal/mem32.h>
#include <private/drv/nic/nic_rtl9300.h>
#include <private/drv/swcore/swcore_rtl9300.h>
#include <drv/nic/nic.h>
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <common/rt_type.h>
#include <osal/time.h>

/*
 * Symbol Definition
 */
#define NIC_9300_RXRING_NUM          (32)
#define NIC_9300_TXRING_NUM          (2)
#define NIC_9300_CPU_PORT           28

/*
 * Data Declaration
 */
static uint32       reasonTbl[][2] =
{
    {0,                                         },              /* 0 */
    {NIC_RX_REASON_CPU2CPU,                     0, },
    {NIC_RX_REASON_DUMMY,                0, },
    {NIC_RX_REASON_DUMMY,                0, },
    {NIC_RX_REASON_DUMMY,                0, },

    {NIC_RX_REASON_DUMMY,                   0, },               /* 5 */
    {NIC_RX_REASON_DUMMY,                   0, },
    {NIC_RX_REASON_OAM,                       0, },
    {NIC_RX_REASON_DUMMY,                   0, },
    {NIC_RX_REASON_DUMMY,                   0, },

    {NIC_RX_REASON_DUMMY,                   0, },                   /* 10 */
    {NIC_RX_REASON_DUMMY,                   0, },
    {NIC_RX_REASON_DUMMY,                   0, },
    {NIC_RX_REASON_IPUC_RPF,               0, },
    {NIC_RX_REASON_INNER_OUTTER_CFI, 0, },

    {NIC_RX_REASON_IVC,                         0, },               /* 15 */
    {NIC_RX_REASON_IGR_VLAN_FILTER,    0, },
    {NIC_RX_REASON_L2_UC_MC_LUMIS,    0, },
    {NIC_RX_REASON_IP4_6_MC_LUMIS,     0, },
    {NIC_RX_REASON_RMA_PTP,                         NIC_RX_REASON_RMA, },

    {NIC_RX_REASON_RMA_USR_DEF,                NIC_RX_REASON_RMA, },   /* 20 */
    {NIC_RX_REASON_DUMMY,                       0, },
    {NIC_RX_REASON_DUMMY,                       0, },
    {NIC_RX_REASON_DUMMY,                       0, },
    {NIC_RX_REASON_RMA_BPDU,                    NIC_RX_REASON_RMA, },

    {NIC_RX_REASON_RMA_LACP,                      NIC_RX_REASON_RMA, },    /* 25 */
    {NIC_RX_REASON_RMA_LLDP,                      NIC_RX_REASON_RMA, },
    {NIC_RX_REASON_EAPOL,                           NIC_RX_REASON_RMA, },
    {NIC_RX_REASON_RMA,                         0, },
    {NIC_RX_REASON_DUMMY,                     0, },

    {NIC_RX_REASON_DUMMY,                     0, },                   /* 30 */
    {NIC_RX_REASON_ROUTER_IF_MAC,             0, },
    {NIC_RX_REASON_L3UC_NON_IP,                 0, },
    {NIC_RX_REASON_IP4_6_HDR_ERROR,          0, },
    {NIC_RX_REASON_ROUTE_IP_CHK,                0, },

    {NIC_RX_REASON_L3_IP_MAC_MISMATCH,           0, },              /* 35 */
    {NIC_RX_REASON_IP6_HOPBYHOP_OPTION,         0, },
    {NIC_RX_REASON_IP6_ROUTE_HDR,               0, },
    {NIC_RX_REASON_IP4_OPTIONS,                    0, },
    {NIC_RX_REASON_IP4_6_ROUTE_LUMIS,         0, },

    {NIC_RX_REASON_IPUC_NULL_ROUTE,            0, },                   /* 40 */
    {NIC_RX_REASON_IPUC_PBR_NULL_ROUTE,    0, },
    {NIC_RX_REASON_IPUC_HOST_ROUTE,           0, },
    {NIC_RX_REASON_IPUC_NET_ROUTE,             0, },
    {NIC_RX_REASON_IPMC_BDG_ENTRY,            0, },

    {NIC_RX_REASON_IPMC_ROUTE_ENTRY,        0, },                   /*45*/
    {NIC_RX_REASON_IPMC_ASSERT,                 0, },
    {NIC_RX_REASON_DUMMY,                           0, },
    {NIC_RX_REASON_ROUT_EXCPT_NH_AGE,      0, },
    {NIC_RX_REASON_DUMMY,                             0, },

    {NIC_RX_REASON_IPUC_TTL,                            0, },          /* 50 */
    {NIC_RX_REASON_IPMC_TTL,                       0, },
    {NIC_RX_REASON_IPUC_MTU,                       0, },
    {NIC_RX_REASON_IPMC_MTU,                        0, },
    {NIC_RX_REASON_IP4_6_ICMP_REDIR,                        0, },

    {NIC_RX_REASON_IGMP_MLD,                       0, },                /* 55 */
    {NIC_RX_REASON_DHCP_DHCP6,                  0, },
    {NIC_RX_REASON_ARP_REQ_REP_GRA,         0, },
    {NIC_RX_REASON_NEIGHBOR_DISCOVER,     0, },
    {NIC_RX_REASON_IP4_6_RSVD_ADDR,          0, },

    {NIC_RX_REASON_DUMMY,                            0, },              /* 60 */
    {NIC_RX_REASON_RLDP_RLPP,                      0, },
    {NIC_RX_REASON_L2_NTFY,                          0, },
    {NIC_RX_REASON_NORMAL_FWD,                  0, },
};


/*QueueId:                             0                       8                       16                      24                   31 */
static uint32   rxRingIdSize_9300[] = {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
static uint32   txRingIdSize_9300[] = {32, 32};
static uint8    nic_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};


/*
 * Function Declaration
 */
static int32 _nic_rx_reason_translate(drv_nic_pkt_t *pPacket, nic_9300_cpuTag_t *pCpuTag)
{
#define RTL9300_MAX_REASON_NM  64
    uint16 reason = pCpuTag->un.rx.REASON;

    if (pCpuTag->un.rx.ACL_HIT)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ACL_HIT);
    if (pCpuTag->un.rx.MAC_CST)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_MAC_CONSTRAINT);
    if (pCpuTag->un.rx.NEW_SA)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_NEW_SA);
    if (pCpuTag->un.rx.PMV_FORRBID)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_PMV_FBD);
    if (pCpuTag->un.rx.L2_STC_PMV)
    {
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_STC_L2_PMV);
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_PMV);
    }
    if (pCpuTag->un.rx.L2_DYN_PMV)
    {
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_DYN_L2_PMV);
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_PMV);
    }
    if (pCpuTag->un.rx.HASH_FULL)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_HASH_FULL);
    if (pCpuTag->un.rx.INVALID_SA)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_INVALID_SA);
    if (pCpuTag->un.rx.ATK_TYPE)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ATTACK);
    if (pCpuTag->un.rx.MIR_HIT)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_MIRROR);

    if (NIC_9300_SFLOW_RX == pCpuTag->un.rx.SFLOW)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_RX);
    else if (NIC_9300_SFLOW_TX == pCpuTag->un.rx.SFLOW)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_TX);
    if (pCpuTag->un.rx.L2_ERR_PKT)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_CRC_ERROR);

    if (pCpuTag->un.rx.L3_ERR_PKT)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L3_CHKSUM_ERROR);
    else if (reason < RTL9300_MAX_REASON_NM)
    {
        NIC_REASON_MASK_SET(*pPacket, reasonTbl[reason][0]);
        NIC_REASON_MASK_SET(*pPacket, reasonTbl[reason][1]);
    }

    return RT_ERR_OK;
}


static int32
_removeCPUTag(uint8 *pPkt, uint8 **ppNewPkt, uint32 *pOffset, nic_9300_cpuTag_t *pCpuTag)
{
    if ((NULL == pPkt) || (NULL == ppNewPkt) || (NULL == pOffset) || (NULL == pCpuTag))
        return RT_ERR_FAILED;

    /* cpuTag copy: offset 14 bytes = DA(6) + SA(6)+ CPU_TAG_ID(2) */
    osal_memcpy((uint8 *)pCpuTag, (uint8 *)(pPkt + 14), sizeof(nic_9300_cpuTag_t));

    /* fix the packet */
    *(pPkt + 31) = *(pPkt + 11);
    *(pPkt + 30) = *(pPkt + 10);
    *(pPkt + 29) = *(pPkt + 9);
    *(pPkt + 28) = *(pPkt + 8);
    *(pPkt + 27) = *(pPkt + 7);
    *(pPkt + 26) = *(pPkt + 6);
    *(pPkt + 25) = *(pPkt + 5);
    *(pPkt + 24) = *(pPkt + 4);
    *(pPkt + 23) = *(pPkt + 3);
    *(pPkt + 22) = *(pPkt + 2);
    *(pPkt + 21) = *(pPkt + 1);
    *(pPkt + 20) = *(pPkt + 0);

    if (ppNewPkt)
    {
        *ppNewPkt = (pPkt + (CPU_TAG_ID_LEN + sizeof(nic_9300_cpuTag_t)));
    }

    if (pOffset)
    {
        *pOffset = (CPU_TAG_ID_LEN + sizeof(nic_9300_cpuTag_t));
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_init
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
r9300_init(uint32 unit, drv_nic_initCfg_t *pInitCfg)
{
    RT_INIT_REENTRY_CHK(nic_init[unit]);

    nic_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_pkt_tx
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
r9300_pkt_tx(uint32 unit, drv_nic_pkt_t *pPacket, drv_nic_tx_cb_f fTxCb, void *pCookie)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_rx_start
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
r9300_rx_start(uint32 unit)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_rx_stop
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
r9300_rx_stop(uint32 unit)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_rx_register
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
r9300_rx_register(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb, void *pCookie, uint32 flags)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_rx_unregister
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
r9300_rx_unregister(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_pkt_alloc
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
r9300_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_pkt_free
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
r9300_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket)
{
    return RT_ERR_OK;
}


/* NIC Tx/Rx debug */
/* Function Name:
 *      r9300_debug_set
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
r9300_debug_set(uint32 unit, uint32 flags)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_debug_get
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
r9300_debug_get(uint32 unit, uint32 *pFlags)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_counter_dump
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
r9300_counter_dump(uint32 unit)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_counter_clear
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
r9300_counter_clear(uint32 unit)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_bufStatus_dump
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
r9300_bufStatus_dump(uint32 unit)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_pkthdrMbuf_dump
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
r9300_pkthdrMbuf_dump(uint32 unit, uint32 mode, uint32 start, uint32 end, uint32 flags)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_rxStatus_get
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
r9300_rxStatus_get(uint32 unit, uint32 *pStatus)
{
    return RT_ERR_OK;
}


int32 r9300_ringInfo_get(uint32 unit, nic_dir_t type, uint32 **ringIdSizeList, uint32 *pRingNum)
{
    if (type == NIC_DIR_RX)
    {
        *ringIdSizeList = rxRingIdSize_9300;
        *pRingNum = NIC_9300_RXRING_NUM;
    }
    else
    {
        *ringIdSizeList = txRingIdSize_9300;
        *pRingNum = NIC_9300_TXRING_NUM;
    }

    return RT_ERR_OK;
}


int32 r9300_cpuPortTxRxEnable_set(uint32 unit, rtk_enable_t enable)
{
    uint32      val;

    val = (enable == ENABLED) ? 1 : 0;

    /* CPU port: MAC Tx/Rx */
    ioal_mem32_field_write(unit, RTL9300_MAC_L2_PORT_CTRL_ADDR(NIC_9300_CPU_PORT),
            RTL9300_MAC_L2_PORT_CTRL_TX_EN_OFFSET, RTL9300_MAC_L2_PORT_CTRL_TX_EN_MASK, val);
    ioal_mem32_field_write(unit, RTL9300_MAC_L2_PORT_CTRL_ADDR(NIC_9300_CPU_PORT),
            RTL9300_MAC_L2_PORT_CTRL_RX_EN_OFFSET, RTL9300_MAC_L2_PORT_CTRL_RX_EN_MASK, val);

    return RT_ERR_OK;
}


int32 r9300_intrMask_get(uint32 unit, nic_intr_type_t type, uint32 *pMask)
{
    if (type == NIC_RX_DONE)
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_INTR_RX_DONE_MSK_ADDR, RTL9300_DMA_IF_INTR_RX_DONE_MSK_RX_DONE_OFFSET, RTL9300_DMA_IF_INTR_RX_DONE_MSK_RX_DONE_MASK, pMask);
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_INTR_RX_RUNOUT_MSK_ADDR, RTL9300_DMA_IF_INTR_RX_RUNOUT_MSK_RX_RUN_OUT_OFFSET, RTL9300_DMA_IF_INTR_RX_RUNOUT_MSK_RX_RUN_OUT_MASK, pMask);
    else if (type == NIC_TX_DONE)
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_INTR_TX_DONE_MSK_ADDR, RTL9300_DMA_IF_INTR_TX_DONE_MSK_TX_DONE_OFFSET, RTL9300_DMA_IF_INTR_TX_DONE_MSK_TX_DONE_MASK, pMask);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_INTR_TX_DONE_MSK_ADDR, RTL9300_DMA_IF_INTR_TX_DONE_MSK_TX_ALL_DONE_OFFSET, RTL9300_DMA_IF_INTR_TX_DONE_MSK_TX_ALL_DONE_MASK, pMask);
    else if (type == NIC_NTFY_DONE)
        ioal_mem32_field_read(unit, RTL9300_L2_NOTIFY_IF_INTR_MSK_ADDR, RTL9300_L2_NOTIFY_IF_INTR_MSK_NTFY_DONE_OFFSET, RTL9300_L2_NOTIFY_IF_INTR_MSK_NTFY_DONE_MASK, pMask);
    else if (type == NIC_NTFY_BUF_RUNOUT)
        ioal_mem32_field_read(unit, RTL9300_L2_NOTIFY_IF_INTR_MSK_ADDR, RTL9300_L2_NOTIFY_IF_INTR_MSK_NTFY_BUF_RUN_OUT_OFFSET, RTL9300_L2_NOTIFY_IF_INTR_MSK_NTFY_BUF_RUN_OUT_MASK, pMask);
    else if (type == NIC_NTFY_LOCALBUF_RUNOUT)
        ioal_mem32_field_read(unit, RTL9300_L2_NOTIFY_IF_INTR_MSK_ADDR, RTL9300_L2_NOTIFY_IF_INTR_MSK_LOCAL_NTFY_BUF_RUN_OUT_OFFSET, RTL9300_L2_NOTIFY_IF_INTR_MSK_LOCAL_NTFY_BUF_RUN_OUT_MASK, pMask);

    return RT_ERR_OK;
}


int32 r9300_intrMask_set(uint32 unit, nic_intr_type_t type, uint32 mask)
{
    if (type == NIC_RX_DONE)
        ioal_mem32_field_write(unit, RTL9300_DMA_IF_INTR_RX_DONE_MSK_ADDR, RTL9300_DMA_IF_INTR_RX_DONE_MSK_RX_DONE_OFFSET, RTL9300_DMA_IF_INTR_RX_DONE_MSK_RX_DONE_MASK, mask);
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_field_write(unit, RTL9300_DMA_IF_INTR_RX_RUNOUT_MSK_ADDR, RTL9300_DMA_IF_INTR_RX_RUNOUT_MSK_RX_RUN_OUT_OFFSET, RTL9300_DMA_IF_INTR_RX_RUNOUT_MSK_RX_RUN_OUT_MASK, mask);
    else if (type == NIC_TX_DONE)
        ioal_mem32_field_write(unit, RTL9300_DMA_IF_INTR_TX_DONE_MSK_ADDR, RTL9300_DMA_IF_INTR_TX_DONE_MSK_TX_DONE_OFFSET, RTL9300_DMA_IF_INTR_TX_DONE_MSK_TX_DONE_MASK, mask);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_field_write(unit, RTL9300_DMA_IF_INTR_TX_DONE_MSK_ADDR, RTL9300_DMA_IF_INTR_TX_DONE_MSK_TX_ALL_DONE_OFFSET, RTL9300_DMA_IF_INTR_TX_DONE_MSK_TX_ALL_DONE_MASK, mask);
    else if (type == NIC_NTFY_DONE)
        ioal_mem32_field_write(unit, RTL9300_L2_NOTIFY_IF_INTR_MSK_ADDR, RTL9300_L2_NOTIFY_IF_INTR_MSK_NTFY_DONE_OFFSET, RTL9300_L2_NOTIFY_IF_INTR_MSK_NTFY_DONE_MASK, mask);
    else if (type == NIC_NTFY_BUF_RUNOUT)
        ioal_mem32_field_write(unit, RTL9300_L2_NOTIFY_IF_INTR_MSK_ADDR, RTL9300_L2_NOTIFY_IF_INTR_MSK_NTFY_BUF_RUN_OUT_OFFSET, RTL9300_L2_NOTIFY_IF_INTR_MSK_NTFY_BUF_RUN_OUT_MASK, mask);
    else if (type == NIC_NTFY_LOCALBUF_RUNOUT)
        ioal_mem32_field_write(unit, RTL9300_L2_NOTIFY_IF_INTR_MSK_ADDR, RTL9300_L2_NOTIFY_IF_INTR_MSK_LOCAL_NTFY_BUF_RUN_OUT_OFFSET, RTL9300_L2_NOTIFY_IF_INTR_MSK_LOCAL_NTFY_BUF_RUN_OUT_MASK, mask);

    return RT_ERR_OK;
}


int32 r9300_intrSts_get(uint32 unit, nic_intr_type_t type, uint32 *pVal)
{
    if (type == NIC_RX_DONE)
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_INTR_RX_DONE_STS_ADDR, RTL9300_DMA_IF_INTR_RX_DONE_STS_RX_DONE_OFFSET, RTL9300_DMA_IF_INTR_RX_DONE_STS_RX_DONE_MASK, pVal);
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_INTR_RX_RUNOUT_STS_ADDR, RTL9300_DMA_IF_INTR_RX_RUNOUT_STS_RX_RUN_OUT_OFFSET, RTL9300_DMA_IF_INTR_RX_RUNOUT_STS_RX_RUN_OUT_MASK, pVal);
    else if (type == NIC_TX_DONE)
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_INTR_TX_DONE_STS_ADDR, RTL9300_DMA_IF_INTR_TX_DONE_STS_TX_DONE_OFFSET, RTL9300_DMA_IF_INTR_TX_DONE_STS_TX_DONE_MASK, pVal);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_INTR_TX_DONE_STS_ADDR, RTL9300_DMA_IF_INTR_TX_DONE_STS_TX_ALL_DONE_OFFSET, RTL9300_DMA_IF_INTR_TX_DONE_STS_TX_ALL_DONE_MASK, pVal);
    else if (type == NIC_NTFY_DONE)
        ioal_mem32_field_read(unit, RTL9300_L2_NOTIFY_IF_INTR_STS_ADDR, RTL9300_L2_NOTIFY_IF_INTR_STS_NTFY_DONE_OFFSET, RTL9300_L2_NOTIFY_IF_INTR_STS_NTFY_DONE_MASK, pVal);
    else if (type == NIC_NTFY_BUF_RUNOUT)
        ioal_mem32_field_read(unit, RTL9300_L2_NOTIFY_IF_INTR_STS_ADDR, RTL9300_L2_NOTIFY_IF_INTR_STS_NTFY_BUF_RUN_OUT_OFFSET, RTL9300_L2_NOTIFY_IF_INTR_STS_NTFY_BUF_RUN_OUT_MASK, pVal);
    else if (type == NIC_NTFY_LOCALBUF_RUNOUT)
        ioal_mem32_field_read(unit, RTL9300_L2_NOTIFY_IF_INTR_STS_ADDR, RTL9300_L2_NOTIFY_IF_INTR_STS_LOCAL_NTFY_BUF_RUN_OUT_OFFSET, RTL9300_L2_NOTIFY_IF_INTR_STS_LOCAL_NTFY_BUF_RUN_OUT_MASK, pVal);

    return RT_ERR_OK;
}


int32 r9300_intrSts_set(uint32 unit, nic_intr_type_t type, uint32 val)
{
    if (type == NIC_RX_DONE)
        ioal_mem32_write(unit, RTL9300_DMA_IF_INTR_RX_DONE_STS_ADDR, (val << RTL9300_DMA_IF_INTR_RX_DONE_STS_RX_DONE_OFFSET) & RTL9300_DMA_IF_INTR_RX_DONE_STS_RX_DONE_MASK);
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_write(unit, RTL9300_DMA_IF_INTR_RX_RUNOUT_STS_ADDR, (val << RTL9300_DMA_IF_INTR_RX_RUNOUT_STS_RX_RUN_OUT_OFFSET) & RTL9300_DMA_IF_INTR_RX_RUNOUT_STS_RX_RUN_OUT_MASK);
    else if (type == NIC_TX_DONE)
        ioal_mem32_write(unit, RTL9300_DMA_IF_INTR_TX_DONE_STS_ADDR, (val << RTL9300_DMA_IF_INTR_TX_DONE_STS_TX_DONE_OFFSET) & RTL9300_DMA_IF_INTR_TX_DONE_STS_TX_DONE_MASK);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_write(unit, RTL9300_DMA_IF_INTR_TX_DONE_STS_ADDR, (val << RTL9300_DMA_IF_INTR_TX_DONE_STS_TX_ALL_DONE_OFFSET) & RTL9300_DMA_IF_INTR_TX_DONE_STS_TX_ALL_DONE_MASK);
    else if (type == NIC_NTFY_DONE)
        ioal_mem32_write(unit, RTL9300_L2_NOTIFY_IF_INTR_STS_ADDR, (val << RTL9300_L2_NOTIFY_IF_INTR_STS_NTFY_DONE_OFFSET) & RTL9300_L2_NOTIFY_IF_INTR_STS_NTFY_DONE_MASK);
    else if (type == NIC_NTFY_BUF_RUNOUT)
        ioal_mem32_write(unit, RTL9300_L2_NOTIFY_IF_INTR_STS_ADDR, (val << RTL9300_L2_NOTIFY_IF_INTR_STS_NTFY_BUF_RUN_OUT_OFFSET) & RTL9300_L2_NOTIFY_IF_INTR_STS_NTFY_BUF_RUN_OUT_MASK);
    else if (type == NIC_NTFY_LOCALBUF_RUNOUT)
        ioal_mem32_write(unit, RTL9300_L2_NOTIFY_IF_INTR_STS_ADDR, (val << RTL9300_L2_NOTIFY_IF_INTR_STS_LOCAL_NTFY_BUF_RUN_OUT_OFFSET) & RTL9300_L2_NOTIFY_IF_INTR_STS_LOCAL_NTFY_BUF_RUN_OUT_MASK);

    return RT_ERR_OK;
}


int32 r9300_swNicRst_get(uint32 unit, uint32 *pStatus)
{
    ioal_mem32_field_read(unit, RTL9300_RST_GLB_CTRL_0_ADDR, RTL9300_RST_GLB_CTRL_0_SW_NIC_RST_OFFSET, RTL9300_RST_GLB_CTRL_0_SW_NIC_RST_MASK, pStatus);
    return RT_ERR_OK;
}


int32 r9300_swNicRst_set(uint32 unit)
{
    RT_LOG(LOG_DEBUG, MOD_NIC, "Reset NIC (R9300)... ");
    ioal_mem32_field_write(unit, RTL9300_RST_GLB_CTRL_0_ADDR, RTL9300_RST_GLB_CTRL_0_SW_NIC_RST_OFFSET, RTL9300_RST_GLB_CTRL_0_SW_NIC_RST_MASK, 1);
    return RT_ERR_OK;
}


int32 r9300_swQueRst_get(uint32 unit, uint32 *pStatus)
{
    ioal_mem32_field_read(unit, RTL9300_RST_GLB_CTRL_0_ADDR, RTL9300_RST_GLB_CTRL_0_SW_Q_RST_OFFSET, RTL9300_RST_GLB_CTRL_0_SW_Q_RST_MASK, pStatus);

    return RT_ERR_OK;
}


int32 r9300_swQueRst_set(uint32 unit)
{
    ioal_mem32_field_write(unit, RTL9300_RST_GLB_CTRL_0_ADDR, RTL9300_RST_GLB_CTRL_0_SW_Q_RST_OFFSET, RTL9300_RST_GLB_CTRL_0_SW_Q_RST_MASK, 1);
    osal_time_mdelay(50); /* delay 50mS */

    return RT_ERR_OK;
}


int32 r9300_cpuL2FloodMask_add(uint32 unit)
{
    uint32 val;

    ioal_mem32_field_read(unit, RTL9300_L2_UNKN_UC_FLD_PMSK_ADDR, RTL9300_L2_UNKN_UC_FLD_PMSK_PORTMASK_OFFSET, RTL9300_L2_UNKN_UC_FLD_PMSK_PORTMASK_MASK, &val);
    val |= (0x1 << NIC_9300_CPU_PORT);
    ioal_mem32_field_write(unit, RTL9300_L2_UNKN_UC_FLD_PMSK_ADDR, RTL9300_L2_UNKN_UC_FLD_PMSK_PORTMASK_OFFSET, RTL9300_L2_UNKN_UC_FLD_PMSK_PORTMASK_MASK, val);
    return RT_ERR_OK;
}

int32 r9300_cpuL2FloodMask_remove(uint32 unit)
{
    uint32 val;

    /* Remove the CPU port from Lookup Miss Flooding Portmask */
    ioal_mem32_field_read(unit, RTL9300_L2_UNKN_UC_FLD_PMSK_ADDR, RTL9300_L2_UNKN_UC_FLD_PMSK_PORTMASK_OFFSET, RTL9300_L2_UNKN_UC_FLD_PMSK_PORTMASK_MASK, &val);
    val &= ~(0x1 << NIC_9300_CPU_PORT);
    ioal_mem32_field_write(unit, RTL9300_L2_UNKN_UC_FLD_PMSK_ADDR, RTL9300_L2_UNKN_UC_FLD_PMSK_PORTMASK_OFFSET, RTL9300_L2_UNKN_UC_FLD_PMSK_PORTMASK_MASK, val);
    return RT_ERR_OK;
}


int32 r9300_cpuForceLinkupEnable_set(uint32 unit, rtk_enable_t enable)
{
    uint32      val;

    val = (enable == ENABLED) ? 1 : 0;

    /*Force flowctrol disable*/
    ioal_mem32_field_write(unit, RTL9300_MAC_FORCE_MODE_CTRL_ADDR(NIC_9300_CPU_PORT), RTL9300_MAC_FORCE_MODE_CTRL_MAC_FORCE_FC_EN_OFFSET,
            RTL9300_MAC_FORCE_MODE_CTRL_MAC_FORCE_FC_EN_MASK, 1);
    ioal_mem32_field_write(unit, RTL9300_MAC_FORCE_MODE_CTRL_ADDR(NIC_9300_CPU_PORT), RTL9300_MAC_FORCE_MODE_CTRL_RX_PAUSE_EN_OFFSET,
            RTL9300_MAC_FORCE_MODE_CTRL_RX_PAUSE_EN_MASK, 0);
    ioal_mem32_field_write(unit, RTL9300_MAC_FORCE_MODE_CTRL_ADDR(NIC_9300_CPU_PORT), RTL9300_MAC_FORCE_MODE_CTRL_TX_PAUSE_EN_OFFSET,
            RTL9300_MAC_FORCE_MODE_CTRL_TX_PAUSE_EN_MASK, 0);

    ioal_mem32_field_write(unit, RTL9300_MAC_FORCE_MODE_CTRL_ADDR(NIC_9300_CPU_PORT), RTL9300_MAC_FORCE_MODE_CTRL_FORCE_LINK_EN_OFFSET,
            RTL9300_MAC_FORCE_MODE_CTRL_FORCE_LINK_EN_MASK, val);
    ioal_mem32_field_write(unit, RTL9300_MAC_FORCE_MODE_CTRL_ADDR(NIC_9300_CPU_PORT), RTL9300_MAC_FORCE_MODE_CTRL_MAC_FORCE_EN_OFFSET,
            RTL9300_MAC_FORCE_MODE_CTRL_MAC_FORCE_EN_MASK, val);

    return RT_ERR_OK;
}


int32 r9300_holRingSize_set(uint32 unit, uint32 ring, uint32 val)
{
    ioal_mem32_field_write(unit, RTL9300_DMA_IF_RX_RING_SIZE_ADDR(ring), RTL9300_DMA_IF_RX_RING_SIZE_SIZE_OFFSET(ring),
            RTL9300_DMA_IF_RX_RING_SIZE_SIZE_MASK(ring), val);
    return RT_ERR_OK;
}

int32 r9300_holRingCnt_get(uint32 unit, uint32 ring, uint32 *pVal)
{
    uint32 tmpVal;

    ioal_mem32_read(unit, RTL9300_DMA_IF_RX_RING_CNTR_ADDR(ring), &tmpVal);
    *pVal = (tmpVal & RTL9300_DMA_IF_RX_RING_CNTR_CNTR_MASK(ring)) >> RTL9300_DMA_IF_RX_RING_CNTR_CNTR_OFFSET(ring);

    return RT_ERR_OK;
}

int32 r9300_holRingCnt_set(uint32 unit, uint32 ring, uint32 val)
{
    /*Note that other ring counter shoud not be reduced */
    ioal_mem32_write(unit, RTL9300_DMA_IF_RX_RING_CNTR_ADDR(ring), \
            ((val << RTL9300_DMA_IF_RX_RING_CNTR_CNTR_OFFSET(ring)) & RTL9300_DMA_IF_RX_RING_CNTR_CNTR_MASK(ring)));
    return RT_ERR_OK;
}

int32 r9300_ntfyBaseAddr_get(uint32 unit, uint32 *pVal)
{
    ioal_mem32_read(unit, RTL9300_L2_NOTIFY_RING_BASE_ADDR_ADDR, pVal);
    return RT_ERR_OK;
}

int32 r9300_ntfyBaseAddr_set(uint32 unit, uint32 val)
{
    ioal_mem32_write(unit, RTL9300_L2_NOTIFY_RING_BASE_ADDR_ADDR, val);
    return RT_ERR_OK;
}


int32 r9300_ringBaseAddr_get(uint32 unit, nic_dir_t dir, uint32 ring, uint32 *pVal)
{
    if (dir == NIC_DIR_RX)
        ioal_mem32_read(unit, RTL9300_DMA_IF_RX_BASE_DESC_ADDR_CTRL_ADDR(ring), pVal);
    else
        ioal_mem32_read(unit, RTL9300_DMA_IF_TX_BASE_DESC_ADDR_CTRL_ADDR(ring), pVal);
    return RT_ERR_OK;
}

int32 r9300_ringBaseAddr_set(uint32 unit, nic_dir_t dir, uint32 ring, uint32 val)
{
    if (dir == NIC_DIR_RX)
        ioal_mem32_write(unit, RTL9300_DMA_IF_RX_BASE_DESC_ADDR_CTRL_ADDR(ring), val);
    else
        ioal_mem32_write(unit, RTL9300_DMA_IF_TX_BASE_DESC_ADDR_CTRL_ADDR(ring), val);
    return RT_ERR_OK;
}


int32 r9300_ringCurAddr_get(uint32 unit, nic_dir_t dir, uint32 ring, uint32 *pVal)
{
    if (dir == NIC_DIR_RX)
        ioal_mem32_read(unit, RTL9300_DMA_IF_RX_CUR_DESC_ADDR_CTRL_ADDR(ring), pVal);
    else
        ioal_mem32_read(unit, RTL9300_DMA_IF_TX_CUR_DESC_ADDR_CTRL_ADDR(ring), pVal);
    return RT_ERR_OK;
}


int32 r9300_rxTruncateLength_get(uint32 unit, uint32 *pVal)
{
    ioal_mem32_field_read(unit, RTL9300_DMA_IF_CTRL_ADDR, RTL9300_DMA_IF_CTRL_RX_TRUNCATE_LEN_OFFSET,
            RTL9300_DMA_IF_CTRL_RX_TRUNCATE_LEN_MASK, pVal);
    return RT_ERR_OK;
}

int32 r9300_rxTruncateLength_set(uint32 unit, uint32 val)
{
    ioal_mem32_field_write(unit, RTL9300_DMA_IF_CTRL_ADDR, RTL9300_DMA_IF_CTRL_RX_TRUNCATE_LEN_OFFSET,
            RTL9300_DMA_IF_CTRL_RX_TRUNCATE_LEN_MASK, val);
    return RT_ERR_OK;
}

int32 r9300_nicEnable_get(uint32 unit, nic_dir_t dir, uint32 *pVal)
{
    if (dir == NIC_DIR_RX)
    {
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_CTRL_ADDR, RTL9300_DMA_IF_CTRL_RX_EN_OFFSET,
                RTL9300_DMA_IF_CTRL_RX_EN_MASK, pVal);
    }
    else
    {
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_CTRL_ADDR, RTL9300_DMA_IF_CTRL_TX_EN_OFFSET,
                RTL9300_DMA_IF_CTRL_TX_EN_MASK, pVal);
    }
    return RT_ERR_OK;
}

int32 r9300_nicEnable_set(uint32 unit, nic_dir_t dir, uint32 val)
{
    if (dir == NIC_DIR_RX)
    {
        ioal_mem32_field_write(unit, RTL9300_DMA_IF_CTRL_ADDR, RTL9300_DMA_IF_CTRL_RX_EN_OFFSET,
                RTL9300_DMA_IF_CTRL_RX_EN_MASK, val);
    }
    else
    {
        ioal_mem32_field_write(unit, RTL9300_DMA_IF_CTRL_ADDR, RTL9300_DMA_IF_CTRL_TX_EN_OFFSET,
                RTL9300_DMA_IF_CTRL_TX_EN_MASK, val);
    }

    if (val)
        RT_LOG(LOG_DEBUG, MOD_NIC, "NIC (R9300) Rx Start... ");
    else
        RT_LOG(LOG_DEBUG, MOD_NIC, "NIC (R9300) Rx Stop... ");

    return RT_ERR_OK;
}


int32 r9300_nicTxFetch_set(uint32 unit, nic_txRing_t type, uint32 val)
{
    if (type == NIC_TXRING_HIGH)
        ioal_mem32_field_write(unit, RTL9300_DMA_IF_CTRL_ADDR, RTL9300_DMA_IF_CTRL_TX_HIGH_FETCH_OFFSET,
                RTL9300_DMA_IF_CTRL_TX_HIGH_FETCH_MASK, val);
    else
        ioal_mem32_field_write(unit, RTL9300_DMA_IF_CTRL_ADDR, RTL9300_DMA_IF_CTRL_TX_LOW_FETCH_OFFSET,
                RTL9300_DMA_IF_CTRL_TX_LOW_FETCH_MASK, val);
    return RT_ERR_OK;
}

int32 r9300_nicTxBusySts_get(uint32 unit, nic_txRing_t type, uint32 *pVal)
{
    if (type == NIC_TXRING_HIGH)
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_CTRL_ADDR, RTL9300_DMA_IF_CTRL_TX_HIGH_BUSY_OFFSET,
                RTL9300_DMA_IF_CTRL_TX_HIGH_BUSY_MASK, pVal);
    else
        ioal_mem32_field_read(unit, RTL9300_DMA_IF_CTRL_ADDR, RTL9300_DMA_IF_CTRL_TX_LOW_BUSY_OFFSET,
                RTL9300_DMA_IF_CTRL_TX_LOW_BUSY_MASK, pVal);
    return RT_ERR_OK;
}


int32 r9300_cpuTagId_get(uint32 unit, uint32 *pVal)
{
    ioal_mem32_field_read(unit, RTL9300_MAC_L2_CPU_TAG_ID_CTRL_ADDR, RTL9300_MAC_L2_CPU_TAG_ID_CTRL_CPU_TAG_ID_OFFSET,
            RTL9300_MAC_L2_CPU_TAG_ID_CTRL_CPU_TAG_ID_MASK, pVal);
    return RT_ERR_OK;
}

int32 r9300_cpuTagId_set(uint32 unit, uint32 val)
{
    ioal_mem32_field_write(unit, RTL9300_MAC_L2_CPU_TAG_ID_CTRL_ADDR, RTL9300_MAC_L2_CPU_TAG_ID_CTRL_CPU_TAG_ID_OFFSET,
            RTL9300_MAC_L2_CPU_TAG_ID_CTRL_CPU_TAG_ID_MASK, val);
    return RT_ERR_OK;
}


int32 r9300_supportJumboSize_get(uint32 unit, uint32 *pVal)
{
    *pVal = NIC_9300_JUMBO_SIZE;
    return RT_ERR_OK;
}


int32 r9300_cpuTagFromRaw_cnvt(uint32 unit, uint8 *pCpuTag, drv_nic_pkt_t *pPacket)
{
    nic_9300_cpuTag_t   *pHdrtag = (nic_9300_cpuTag_t *)pCpuTag, *pCpuTagInfo = NULL;
    nic_9300_cpuTag_t   cputag;

    RT_PARAM_CHK(NULL == pCpuTag, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pPacket, RT_ERR_NULL_POINTER);

    if (pHdrtag->un.rx.CPUTAGIF)
    {
        pCpuTagInfo = pHdrtag;
    }
    else
    {
        uint32              offset = 0, scan_offset = 0;
        uint8               pkt_data;
        uint8               *pData;
        uint32              ethType;

        scan_offset = 12; /* DA(6) + SA(6) */
        pData = pPacket->data + scan_offset;
        ethType = READ16(pData);
        pkt_data = *(pData + 2);

        if (ethType == REALTEK_CPUTAG_ID && pkt_data == 0x04)    /* embedded CPU Tag */
        {
            if (RT_ERR_OK != _removeCPUTag(pPacket->data, &pPacket->data, &offset, &cputag))
            {
                return RT_ERR_FAILED;
            }
            pCpuTagInfo = &cputag;
        }
    }

    if (pCpuTagInfo)
    {
        _nic_rx_reason_translate(pPacket, pCpuTagInfo);

        pPacket->rx_tag.physical_rx_port= pCpuTagInfo->un.rx.PHYSICAL_RX_PORT;
        if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ACL_HIT))
            pPacket->rx_tag.acl_index = pHdrtag->un.rx.ACL_IDX;

        pPacket->rx_tag.mirror_hit      = pCpuTagInfo->un.rx.MIR_HIT;
        pPacket->rx_tag.is_trk          = pCpuTagInfo->un.rx.IS_TRK;
        pPacket->rx_tag.trk_id          = pCpuTagInfo->un.rx.TRK_ID;
        pPacket->rx_tag.atk_type        = pCpuTagInfo->un.rx.ATK_TYPE;
        pPacket->rx_tag.qid             = pCpuTagInfo->un.rx.QID;
        pPacket->rx_tag.dev_id            = pCpuTagInfo->un.rx.SPN >> 6;
        pPacket->rx_tag.source_port     = pCpuTagInfo->un.rx.SPN & 0x3f;
        pPacket->rx_tag.mirror_hit      = pCpuTagInfo->un.rx.MIR_HIT;
        if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_RX))
        {
            pPacket->rx_tag.ext_dev_id        = pPacket->rx_tag.dev_id;
            pPacket->rx_tag.ext_source_port = pPacket->rx_tag.source_port;
        }
        else if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_TX))
        {
            pPacket->rx_tag.ext_dev_id        = pCpuTagInfo->un.rx.ACL_IDX >> 6;
            pPacket->rx_tag.ext_source_port = pCpuTagInfo->un.rx.ACL_IDX & 0x3f;
        }
        else
        {
            pPacket->rx_tag.ext_is_trk      = pCpuTagInfo->un.rx.PORT_DATA_IS_TRK;
            if (pPacket->rx_tag.ext_is_trk)
                pPacket->rx_tag.ext_trk_id      = pHdrtag->un.rx.PORT_DATA;
            else
            {
                pPacket->rx_tag.ext_dev_id        = pHdrtag->un.rx.PORT_DATA >> 6;
                pPacket->rx_tag.ext_source_port = pHdrtag->un.rx.PORT_DATA & 0x3f;
            }
        }
        pPacket->rx_tag.svid_tagged     = pCpuTagInfo->un.rx.OTAG_IF;
        pPacket->rx_tag.cvid_tagged     = pCpuTagInfo->un.rx.ITAG_IF;
        pPacket->rx_tag.fvid_sel        = pCpuTagInfo->un.rx.FWD_SEL;
        pPacket->rx_tag.fvid            = pCpuTagInfo->un.rx.FWD_VID;
        pPacket->rx_tag.oversize          = pCpuTagInfo->un.rx.OVERSIZE;
        pPacket->rx_tag.reason          = pCpuTagInfo->un.rx.REASON;
    }

    return RT_ERR_OK;
}


int32 r9300_cpuTagToRaw_cnvt(uint32 unit, drv_nic_pkt_t *pPacket, uint8 *pCpuTag)
{
    nic_9300_cpuTag_t   *pHdrtag = (nic_9300_cpuTag_t *)pCpuTag;

    RT_PARAM_CHK(NULL == pCpuTag, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pPacket, RT_ERR_NULL_POINTER);

    if (pPacket->as_txtag)
    {
        switch (pPacket->tx_tag.fwd_type)
        {
            case NIC_FWD_TYPE_ALE:
                pHdrtag->un.tx.FWD_TYPE = 0;break;
            case NIC_FWD_TYPE_PHYISCAL:
                pHdrtag->un.tx.FWD_TYPE = 1;break;
            case NIC_FWD_TYPE_LOGICAL:
                pHdrtag->un.tx.FWD_TYPE = 2;break;
            case NIC_FWD_TYPE_TRUNK:
                pHdrtag->un.tx.FWD_TYPE = 3;break;
            case NIC_FWD_TYPE_PHYISCAL_ONE_HOP:
                pHdrtag->un.tx.FWD_TYPE = 4;break;
            case NIC_FWD_TYPE_LOGICAL_ONE_HOP:
                pHdrtag->un.tx.FWD_TYPE = 5;break;
            case NIC_FWD_TYPE_UCST_CPU_MIN_PORT:
                pHdrtag->un.tx.FWD_TYPE = 6;break;
            case NIC_FWD_TYPE_UCST_CPU:
                pHdrtag->un.tx.FWD_TYPE = 7;break;
            case NIC_FWD_TYPE_BCST_CPU:
                pHdrtag->un.tx.FWD_TYPE = 8;break;
            default:
                osal_printf("FATAL Error: fwd_type is not support!\n");
                return RT_ERR_FAILED;
        }

        pHdrtag->un.tx.CPUTAGIF      = TRUE;
        pHdrtag->un.tx.ACL_ACT       = pPacket->tx_tag.acl_act;
        pHdrtag->un.tx.CNGST_DROP    = pPacket->tx_tag.cngst_drop;
        pHdrtag->un.tx.DG_PKT        = pPacket->tx_tag.dg_pkt;
        pHdrtag->un.tx.BP_FLTR       = pPacket->tx_tag.bp_fltr;
        pHdrtag->un.tx.BP_STP        = pPacket->tx_tag.bp_stp;
        pHdrtag->un.tx.BP_VLAN_EGR   = pPacket->tx_tag.bp_vlan_egr;
        pHdrtag->un.tx.ALE_AS_TAGSTS     = pPacket->tx_tag.as_tagSts;
        pHdrtag->un.tx.L3_ACT        = pPacket->tx_tag.l3_act;
        pHdrtag->un.tx.ORI_TAGIF_EN  = pPacket->tx_tag.ori_tagif_en;
        pHdrtag->un.tx.AS_QID        = pPacket->tx_tag.as_priority;
        pHdrtag->un.tx.QID           = pPacket->tx_tag.priority;
        pHdrtag->un.tx.ORI_ITAGIF    = pPacket->tx_tag.ori_itagif;
        pHdrtag->un.tx.ORI_OTAGIF    = pPacket->tx_tag.ori_otagif;
        pHdrtag->un.tx.FWD_VID_SEL      = pPacket->tx_tag.fvid_sel;
        pHdrtag->un.tx.FWD_VID_EN       = pPacket->tx_tag.fvid_en;
        pHdrtag->un.tx.FWD_VID          = pPacket->tx_tag.fvid;
        pHdrtag->un.tx.SRC_FLTR_EN   = pPacket->tx_tag.src_filter_en;
        pHdrtag->un.tx.SP_IS_TRK     = pPacket->tx_tag.sp_is_trk;
        pHdrtag->un.tx.SPN_H        = pPacket->tx_tag.spn >> 4;
        pHdrtag->un.tx.SPN_L        = pPacket->tx_tag.spn & 0xf;
        pHdrtag->un.tx.SW_DEV_ID       = pPacket->tx_tag.dev_id;
        pHdrtag->un.tx.DPM_H      = pPacket->tx_tag.dst_port_mask_1;
        pHdrtag->un.tx.DPM_L       = pPacket->tx_tag.dst_port_mask;
    }
    else
        pHdrtag->un.tx.CPUTAGIF = FALSE;

    return RT_ERR_OK;
}


int32 r9300_cpuTag_dump(uint32 unit, drv_nic_pkt_t *pPacket)
{
    osal_printf("=== [NIC RX Debug - CPU Rx Tag Information] ============ \n");
    osal_printf(" RPN : %d \n", pPacket->rx_tag.physical_rx_port);
    osal_printf(" DEV ID : %d \n", pPacket->rx_tag.dev_id);
    osal_printf(" SPN : %d \n", pPacket->rx_tag.source_port);
    osal_printf(" MIR_HIT : %d \n", pPacket->rx_tag.mirror_hit);
    osal_printf(" RX SFLOW : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_RX));
    osal_printf(" TX SFLOW : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_TX));
    osal_printf(" ACL_HIT : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ACL_HIT));
    osal_printf(" IDX : %d \n", pPacket->rx_tag.acl_index);
    osal_printf(" IS_TRK : %d \n", pPacket->rx_tag.is_trk);
    osal_printf(" TRK_ID : %d \n", pPacket->rx_tag.trk_id);
    osal_printf(" OTAGIF : %d \n", pPacket->rx_tag.svid_tagged);
    osal_printf(" ITAGIF : %d \n", pPacket->rx_tag.cvid_tagged);
    osal_printf(" OVID : %d \n", pPacket->rx_tag.outer_vid);
    osal_printf(" IVID : %d \n", pPacket->rx_tag.inner_vid);
    osal_printf(" FWD_VID_SEL : %d \n", pPacket->rx_tag.fvid_sel);
    osal_printf(" FVID : %d \n", pPacket->rx_tag.fvid);
    osal_printf(" QID : %d \n", pPacket->rx_tag.qid);
    osal_printf(" ATK_TYPE : %d \n", pPacket->rx_tag.atk_type);
    osal_printf(" MAC_CST : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_MAC_CONSTRAINT));
    osal_printf(" NEW_SA : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_NEW_SA));
    osal_printf(" PMV_FORBID : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_PMV_FBD));
    osal_printf(" L2_STTC_PMV : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_STC_L2_PMV));
    osal_printf(" L2_DYN_PMV : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_DYN_L2_PMV));
    osal_printf(" L2_ERR_PKT : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_CRC_ERROR));
    osal_printf(" L3_ERR_PKT : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L3_CHKSUM_ERROR));
    osal_printf(" OVERSIZE : %d \n", pPacket->rx_tag.oversize);
    if (pPacket->rx_tag.ext_is_trk)
    {
        osal_printf(" ext_trkId : %d \n", pPacket->rx_tag.ext_trk_id);
    }
    else
    {
        osal_printf(" ext_dev_id : %d \n", pPacket->rx_tag.ext_dev_id);
        osal_printf(" ext_source_port : %d \n", pPacket->rx_tag.ext_source_port);
    }
    osal_printf(" HASH_FULL : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_HASH_FULL));
    osal_printf(" INVALID_SA : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_INVALID_SA));
    osal_printf(" REASON : %d \n", pPacket->rx_tag.reason);

    return RT_ERR_OK;
}

int32 r9300_rawTag_dump(uint32 unit, uint8 *pCpuTag)
{
    nic_9300_cpuTag_t   *pHdrtag = (nic_9300_cpuTag_t *)pCpuTag;

    osal_printf("=== [NIC TX Debug - CPU Tx Tag Information] ============ \n");

    osal_printf(" CPUTAGIF : 0x%0x \n", pHdrtag->un.tx.CPUTAGIF);
    osal_printf(" FWD_TYPE : 0x%0x \n", pHdrtag->un.tx.FWD_TYPE);
    osal_printf(" ACL_ACT : 0x%0x \n", pHdrtag->un.tx.ACL_ACT);
    osal_printf(" CNGST_DROP : 0x%0x \n", pHdrtag->un.tx.CNGST_DROP);
    osal_printf(" DG_PKT : 0x%0x \n", pHdrtag->un.tx.DG_PKT);
    osal_printf(" BP_FLTR : 0x%0x \n", pHdrtag->un.tx.BP_FLTR);
    osal_printf(" BP_STP : 0x%0x \n", pHdrtag->un.tx.BP_STP);
    osal_printf(" BP_VLAN_EGR : 0x%0x \n", pHdrtag->un.tx.BP_VLAN_EGR);
    osal_printf(" AS_TAGSTS : 0x%0x \n", pHdrtag->un.tx.ALE_AS_TAGSTS);
    osal_printf(" ORI_TAGIF_EN : 0x%0x \n", pHdrtag->un.tx.ORI_TAGIF_EN);
    osal_printf(" L3_ACT : 0x%0x \n", pHdrtag->un.tx.L3_ACT);
    osal_printf(" AS_QID : 0x%0x \n", pHdrtag->un.tx.AS_QID);
    osal_printf(" QID : 0x%0x \n", pHdrtag->un.tx.QID);
    osal_printf(" ORI_ITAGIF : 0x%0x \n", pHdrtag->un.tx.ORI_ITAGIF);
    osal_printf(" ORI_OTAGIF : 0x%0x \n", pHdrtag->un.tx.ORI_OTAGIF);
    osal_printf(" FVID_SEL : 0x%0x \n", pHdrtag->un.tx.FWD_VID_SEL);
    osal_printf(" FVID_EN : 0x%0x \n", pHdrtag->un.tx.FWD_VID_EN);
    osal_printf(" FVID : 0x%0x \n", pHdrtag->un.tx.FWD_VID);
    osal_printf(" SRC_FLTR_EN : 0x%0x \n", pHdrtag->un.tx.SRC_FLTR_EN);
    osal_printf(" SP_IS_TRK : 0x%0x \n", pHdrtag->un.tx.SP_IS_TRK);
    osal_printf(" SPN : 0x%0x \n", (pHdrtag->un.tx.SPN_H << 4) | pHdrtag->un.tx.SPN_L);
    osal_printf(" SW_DEV_ID : 0x%0x \n", pHdrtag->un.tx.SW_DEV_ID);
    osal_printf(" DPM55_32 : 0x%0x \n", pHdrtag->un.tx.DPM_H);
    osal_printf(" DPM31_0 : 0x%0x \n", pHdrtag->un.tx.DPM_L);
    return RT_ERR_OK;
}

