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

/*
 * Include Files
 */
#include <common/error.h>
#include <ioal/mem32.h>
#include <private/drv/nic/nic_rtl9310.h>
#include <private/drv/swcore/swcore_rtl9310.h>
#include <drv/nic/nic.h>
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <common/rt_type.h>
#include <osal/time.h>

/*
 * Symbol Definition
 */
#define NIC_9310_RXRING_NUM     (32)
#define NIC_9310_TXRING_NUM     (2)


/*
 * Data Type Definition
 */


/*
 * Data Declaration
 */
static uint32 		reasonTbl[][2] =
{
    {0,                                         },
    {NIC_RX_REASON_CPU2CPU,                     0, },
    {NIC_RX_REASON_OF_TTL_EXCEED,               0, },
    {NIC_RX_REASON_OF_TBL_LUMIS,                0, },
    {NIC_RX_REASON_OF_INVLD_CAPWAP_HDR,         0, },
    {NIC_RX_REASON_OF_CAPWAP_CTRL,              0, },
    {NIC_RX_REASON_DUMMY,                       0, },
    {NIC_RX_REASON_OAM,                         0, },
    {NIC_RX_REASON_CFM,                         0, },
    {NIC_RX_REASON_CFM_ETHDM,                   0, },
    {NIC_RX_REASON_PARSE_EXCPT,                 0, },                   /* 10 */
    {NIC_RX_REASON_MALFORM,                     0, },
    {NIC_RX_REASON_SEC_IP_MAC_BIND,             0, },
    {NIC_RX_REASON_IPUC_RPF,                    0, },
    {NIC_RX_REASON_INNER_OUTTER_CFI,            0, },
    {NIC_RX_REASON_IVC,                         0, },
    {NIC_RX_REASON_IGR_VLAN_FILTER,             0, },
    {NIC_RX_REASON_L2_UC_MC_LUMIS,              0, },
    {NIC_RX_REASON_IP4_6_MC_LUMIS,              0, },
    {NIC_RX_REASON_RMA_PTP,                     NIC_RX_REASON_RMA, },
    {NIC_RX_REASON_RMA_USR_DEF,                 NIC_RX_REASON_RMA, },   /* 20 */
    {NIC_RX_REASON_DUMMY,                       0, },
    {NIC_RX_REASON_DUMMY,                       0, },
    {NIC_RX_REASON_ECID_EQ_PCID,                0, },
    {NIC_RX_REASON_RMA_BPDU,                    NIC_RX_REASON_RMA, },
    {NIC_RX_REASON_RMA_LACP,                    NIC_RX_REASON_RMA, },
    {NIC_RX_REASON_RMA_LLDP,                    NIC_RX_REASON_RMA, },
    {NIC_RX_REASON_EAPOL,                       NIC_RX_REASON_RMA, },
    {NIC_RX_REASON_RMA,                         0, },
    {NIC_RX_REASON_TUNL_IF_MAC,                 0, },
    {NIC_RX_REASON_TUNL_IP_CHK,                 0, },                   /* 30 */
    {NIC_RX_REASON_ROUTER_IF_MAC,               0, },
    {NIC_RX_REASON_L3UC_NON_IP,                 0, },
    {NIC_RX_REASON_IP4_6_HDR_ERROR,             0, },
    {NIC_RX_REASON_ROUTE_IP_CHK,                0, },
    {NIC_RX_REASON_L3_IP_MAC_MISMATCH,          0, },
    {NIC_RX_REASON_IP6_HOPBYHOP_OPTION,         0, },
    {NIC_RX_REASON_IP6_ROUTE_HDR,               0, },
    {NIC_RX_REASON_IP4_OPTIONS,                 0, },
    {NIC_RX_REASON_IP4_6_ROUTE_LUMIS,           0, },
    {NIC_RX_REASON_IPUC_NULL_ROUTE,             0, },                   /* 40 */
    {NIC_RX_REASON_IPUC_PBR_NULL_ROUTE,         0, },
    {NIC_RX_REASON_IPUC_HOST_ROUTE,             0, },
    {NIC_RX_REASON_IPUC_NET_ROUTE,              0, },
    {NIC_RX_REASON_IPMC_BDG_ENTRY,              0, },
    {NIC_RX_REASON_IPMC_ROUTE_ENTRY,            0, },
    {NIC_RX_REASON_IPMC_ASSERT,                 0, },
    {NIC_RX_REASON_ROUT_EXCPT_NOT_NH,           0, },
    {NIC_RX_REASON_ROUT_EXCPT_NH_AGE,           0, },
    {NIC_RX_REASON_ROUT_EXCPT_NH_ROUTE_TO_TUNL, 0, },
    {NIC_RX_REASON_IPUC_TTL,                    0, },                   /* 50 */
    {NIC_RX_REASON_IPMC_TTL,                    0, },
    {NIC_RX_REASON_IPUC_MTU,                    0, },
    {NIC_RX_REASON_IPMC_MTU,                    0, },
    {NIC_RX_REASON_IP4_6_ICMP_REDIR,            0, },
    {NIC_RX_REASON_IGMP_MLD,                    0, },
    {NIC_RX_REASON_DHCP_DHCP6,                  0, },
    {NIC_RX_REASON_ARP_REQ_REP_GRA,             0, },
    {NIC_RX_REASON_NEIGHBOR_DISCOVER,           0, },
    {NIC_RX_REASON_IP4_6_RSVD_ADDR,             0, },
    {NIC_RX_REASON_MPLS_EXCPT,                  0, },
    {NIC_RX_REASON_DUMMY,                       0, },
    {NIC_RX_REASON_DUMMY,                       0, },
    {NIC_RX_REASON_NORMAL_FWD,                  0, },
};


/*QueueId:                             0                       8                       16                      24                   31 */
static uint32   rxRingIdSize_9310[] = {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
static uint32   txRingIdSize_9310[] = {32, 32};
static uint8    nic_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

static int32 _nic_rx_reason_translate(drv_nic_pkt_t *pPacket, nic_9310_cpuTag_t *pCpuTag)
{
#define RTL9310_MAX_REASON_NM  64
    uint16 reason = pCpuTag->un.rx.REASON;

    if (pCpuTag->un.rx.ACL_OF_HIT == 1)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ACL_HIT);
    if (pCpuTag->un.rx.ACL_OF_HIT == 2)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_OF_HIT);
    if (pCpuTag->un.rx.TT_HIT)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_TUNL_TMNT);
    if (pCpuTag->un.rx.MAC_CST)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_MAC_CONSTRAINT);
    if (pCpuTag->un.rx.NEW_SA)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_NEW_SA);
    if (pCpuTag->un.rx.PMV_FBD)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_PMV_FBD);
    if (pCpuTag->un.rx.L2_STTC_PMV)
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
    if (pCpuTag->un.rx.INVLD_SA)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_INVALID_SA);
    if (pCpuTag->un.rx.ATK_TYPE)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ATTACK);
    if (pCpuTag->un.rx.MIR_HIT)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_MIRROR);
    if (NIC_9310_SFLOW_RX == pCpuTag->un.rx.SFLOW)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_RX);
    else if (NIC_9310_SFLOW_TX == pCpuTag->un.rx.SFLOW)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_TX);
    if (pCpuTag->un.rx.L2_ERR_PKT)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_CRC_ERROR);
    if (pCpuTag->un.rx.L3_ERR_PKT)
        NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L3_CHKSUM_ERROR);


    if (reason < RTL9310_MAX_REASON_NM)
    {
        NIC_REASON_MASK_SET(*pPacket, reasonTbl[reason][0]);
        NIC_REASON_MASK_SET(*pPacket, reasonTbl[reason][1]);
    }

    return RT_ERR_OK;
}

static int32
_removeCPUTag(uint8 *pPkt, uint8 **ppNewPkt, uint32 *pOffset, nic_9310_cpuTag_t *pCpuTag)
{
    if ((NULL == pPkt) || (NULL == ppNewPkt) || (NULL == pOffset) || (NULL == pCpuTag))
        return RT_ERR_FAILED;

    /* cpuTag copy: offset 14 bytes = DA(6) + SA(6)+ CPU_TAG_ID(2) */
    osal_memcpy((uint8 *)pCpuTag, (uint8 *)(pPkt + 14), sizeof(nic_9310_cpuTag_t));

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
        *ppNewPkt = (pPkt + (CPU_TAG_ID_LEN + sizeof(nic_9310_cpuTag_t)));
    }

    if (pOffset)
    {
        *pOffset = (CPU_TAG_ID_LEN + sizeof(nic_9310_cpuTag_t));
    }

    return RT_ERR_OK;
}

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
int32
r9310_init(uint32 unit, drv_nic_initCfg_t *pInitCfg)
{
    RT_INIT_REENTRY_CHK(nic_init[unit]);
    nic_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}


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
int32
r9310_pkt_tx(uint32 unit, drv_nic_pkt_t *pPacket, drv_nic_tx_cb_f fTxCb, void *pCookie)
{
    return RT_ERR_OK;
}


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
int32
r9310_rx_start(uint32 unit)
{
    return RT_ERR_OK;
}


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
int32
r9310_rx_stop(uint32 unit)
{
    return RT_ERR_OK;
}


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
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
r9310_rx_register(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb, void *pCookie, uint32 flags)
{
    return RT_ERR_OK;
}


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
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
r9310_rx_unregister(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb)
{
    return RT_ERR_OK;
}


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
int32
r9310_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket)
{
    return RT_ERR_OK;
}

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
int32
r9310_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket)
{
    return RT_ERR_OK;
}

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
int32
r9310_debug_set(uint32 unit, uint32 flags)
{
    return RT_ERR_OK;
}

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
int32
r9310_debug_get(uint32 unit, uint32 *pFlags)
{
    return RT_ERR_OK;
}

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
int32
r9310_counter_dump(uint32 unit)
{
    return RT_ERR_OK;
}

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
int32
r9310_counter_clear(uint32 unit)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      r9310_bufStatus_dump
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
r9310_bufStatus_dump(uint32 unit)
{
    return RT_ERR_OK;
}


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
int32
r9310_pkthdrMbuf_dump(uint32 unit, uint32 mode, uint32 start, uint32 end, uint32 flags)
{
    return RT_ERR_OK;
}

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
int32
r9310_rxStatus_get(uint32 unit, uint32 *pStatus)
{
    return RT_ERR_OK;
}

int32 r9310_ringInfo_get(uint32 unit, nic_dir_t type, uint32 **ringIdSizeList, uint32 *pRingNum)
{
    if (type == NIC_DIR_RX)
    {
        *ringIdSizeList = rxRingIdSize_9310;
        *pRingNum = NIC_9310_RXRING_NUM;
    }
    else
    {
        *ringIdSizeList = txRingIdSize_9310;
        *pRingNum = NIC_9310_TXRING_NUM;
    }

    return RT_ERR_OK;
}

int32 r9310_cpuPortTxRxEnable_set(uint32 unit, rtk_enable_t enable)
{
    ioal_mem32_field_write(unit, RTL9310_MAC_L2_PORT_CTRL_ADDR(56), RTL9310_MAC_L2_PORT_CTRL_TX_EN_OFFSET, RTL9310_MAC_L2_PORT_CTRL_TX_EN_MASK, enable);
    ioal_mem32_field_write(unit, RTL9310_MAC_L2_PORT_CTRL_ADDR(56), RTL9310_MAC_L2_PORT_CTRL_RX_EN_OFFSET, RTL9310_MAC_L2_PORT_CTRL_RX_EN_MASK, enable);

    /* patch */
    if (enable)
        ioal_mem32_field_write(unit, RTL9310_MAC_L2_GLOBAL_CTRL2_ADDR, RTL9310_MAC_L2_GLOBAL_CTRL2_EXT_CPU_EN_OFFSET, RTL9310_MAC_L2_GLOBAL_CTRL2_EXT_CPU_EN_MASK, 0);
    ioal_mem32_field_write(unit, RTL9310_VLAN_PORT_IGR_FLTR_ADDR(56), RTL9310_VLAN_PORT_IGR_FLTR_IGR_FLTR_ACT_OFFSET(56), RTL9310_VLAN_PORT_IGR_FLTR_IGR_FLTR_ACT_MASK(56), 0);
    ioal_mem32_field_write(unit, RTL9310_PS_SOC_CTRL_ADDR, RTL9310_PS_SOC_CTRL_PCIE_PWR_DOWN_OFFSET, RTL9310_PS_SOC_CTRL_PCIE_PWR_DOWN_MASK, 1);

    return RT_ERR_OK;
}

int32 r9310_intrMask_get(uint32 unit, nic_intr_type_t type, uint32 *pMask)
{
    if (type == NIC_RX_DONE)
        ioal_mem32_read(unit, RTL9310_DMA_IF_INTR_RX_DONE_MSK_ADDR, pMask);
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_read(unit, RTL9310_DMA_IF_INTR_RX_RUNOUT_MSK_ADDR, pMask);
    else if (type == NIC_TX_DONE)
        ioal_mem32_field_read(unit, RTL9310_DMA_IF_INTR_TX_DONE_MSK_ADDR, RTL9310_DMA_IF_INTR_TX_DONE_MSK_TX_DONE_OFFSET, RTL9310_DMA_IF_INTR_TX_DONE_MSK_TX_DONE_MASK, pMask);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_field_read(unit, RTL9310_DMA_IF_INTR_TX_DONE_MSK_ADDR, RTL9310_DMA_IF_INTR_TX_DONE_MSK_TX_ALL_DONE_OFFSET, RTL9310_DMA_IF_INTR_TX_DONE_MSK_TX_ALL_DONE_MASK, pMask);
    else if (type == NIC_NTFY_DONE)
        ioal_mem32_field_read(unit, RTL9310_L2_NTFY_IF_INTR_MSK_ADDR, RTL9310_L2_NTFY_IF_INTR_MSK_NTFY_DONE_OFFSET, RTL9310_L2_NTFY_IF_INTR_MSK_NTFY_DONE_MASK, pMask);
    else if (type == NIC_NTFY_BUF_RUNOUT)
        ioal_mem32_field_read(unit, RTL9310_L2_NTFY_IF_INTR_MSK_ADDR, RTL9310_L2_NTFY_IF_INTR_MSK_NTFY_BUF_RUN_OUT_OFFSET, RTL9310_L2_NTFY_IF_INTR_MSK_NTFY_BUF_RUN_OUT_MASK, pMask);
    else if (type == NIC_NTFY_LOCALBUF_RUNOUT)
        ioal_mem32_field_read(unit, RTL9310_L2_NTFY_IF_INTR_MSK_ADDR, RTL9310_L2_NTFY_IF_INTR_MSK_LOCAL_NTFY_BUF_RUN_OUT_OFFSET, RTL9310_L2_NTFY_IF_INTR_MSK_LOCAL_NTFY_BUF_RUN_OUT_MASK, pMask);

    return RT_ERR_OK;
}

int32 r9310_intrMask_set(uint32 unit, nic_intr_type_t type, uint32 mask)
{
    if (type == NIC_RX_DONE)
        ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_RX_DONE_MSK_ADDR, mask);
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_RX_RUNOUT_MSK_ADDR, mask);
    else if (type == NIC_TX_DONE)
        ioal_mem32_field_write(unit, RTL9310_DMA_IF_INTR_TX_DONE_MSK_ADDR, RTL9310_DMA_IF_INTR_TX_DONE_MSK_TX_DONE_OFFSET, RTL9310_DMA_IF_INTR_TX_DONE_MSK_TX_DONE_MASK, mask);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_field_write(unit, RTL9310_DMA_IF_INTR_TX_DONE_MSK_ADDR, RTL9310_DMA_IF_INTR_TX_DONE_MSK_TX_ALL_DONE_OFFSET, RTL9310_DMA_IF_INTR_TX_DONE_MSK_TX_ALL_DONE_MASK, mask);
    else if (type == NIC_NTFY_DONE)
        ioal_mem32_field_write(unit, RTL9310_L2_NTFY_IF_INTR_MSK_ADDR, RTL9310_L2_NTFY_IF_INTR_MSK_NTFY_DONE_OFFSET, RTL9310_L2_NTFY_IF_INTR_MSK_NTFY_DONE_MASK, mask);
    else if (type == NIC_NTFY_BUF_RUNOUT)
        ioal_mem32_field_write(unit, RTL9310_L2_NTFY_IF_INTR_MSK_ADDR, RTL9310_L2_NTFY_IF_INTR_MSK_NTFY_BUF_RUN_OUT_OFFSET, RTL9310_L2_NTFY_IF_INTR_MSK_NTFY_BUF_RUN_OUT_MASK, mask);
    else if (type == NIC_NTFY_LOCALBUF_RUNOUT)
        ioal_mem32_field_write(unit, RTL9310_L2_NTFY_IF_INTR_MSK_ADDR, RTL9310_L2_NTFY_IF_INTR_MSK_LOCAL_NTFY_BUF_RUN_OUT_OFFSET, RTL9310_L2_NTFY_IF_INTR_MSK_LOCAL_NTFY_BUF_RUN_OUT_MASK, mask);

    return RT_ERR_OK;
}

int32 r9310_intrSts_get(uint32 unit, nic_intr_type_t type, uint32 *pVal)
{
    if (type == NIC_RX_DONE)
        ioal_mem32_read(unit, RTL9310_DMA_IF_INTR_RX_DONE_STS_ADDR, pVal);
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_read(unit, RTL9310_DMA_IF_INTR_RX_RUNOUT_STS_ADDR, pVal);
    else if (type == NIC_TX_DONE)
        ioal_mem32_field_read(unit, RTL9310_DMA_IF_INTR_TX_DONE_STS_ADDR, RTL9310_DMA_IF_INTR_TX_DONE_STS_TX_DONE_OFFSET, RTL9310_DMA_IF_INTR_TX_DONE_STS_TX_DONE_MASK, pVal);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_field_read(unit, RTL9310_DMA_IF_INTR_TX_DONE_STS_ADDR, RTL9310_DMA_IF_INTR_TX_DONE_STS_TX_ALL_DONE_OFFSET, RTL9310_DMA_IF_INTR_TX_DONE_STS_TX_ALL_DONE_MASK, pVal);
    else if (type == NIC_NTFY_DONE)
        ioal_mem32_field_read(unit, RTL9310_L2_NTFY_IF_INTR_STS_ADDR, RTL9310_L2_NTFY_IF_INTR_STS_NTFY_DONE_OFFSET, RTL9310_L2_NTFY_IF_INTR_STS_NTFY_DONE_MASK, pVal);
    else if (type == NIC_NTFY_BUF_RUNOUT)
        ioal_mem32_field_read(unit, RTL9310_L2_NTFY_IF_INTR_STS_ADDR, RTL9310_L2_NTFY_IF_INTR_STS_NTFY_BUF_RUN_OUT_OFFSET, RTL9310_L2_NTFY_IF_INTR_STS_NTFY_BUF_RUN_OUT_MASK, pVal);
    else if (type == NIC_NTFY_LOCALBUF_RUNOUT)
        ioal_mem32_field_read(unit, RTL9310_L2_NTFY_IF_INTR_STS_ADDR, RTL9310_L2_NTFY_IF_INTR_STS_LOCAL_NTFY_BUF_RUN_OUT_OFFSET, RTL9310_L2_NTFY_IF_INTR_STS_LOCAL_NTFY_BUF_RUN_OUT_MASK, pVal);

    return RT_ERR_OK;
}

int32 r9310_intrSts_set(uint32 unit, nic_intr_type_t type, uint32 val)
{
    if (type == NIC_RX_DONE)
        ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_RX_DONE_STS_ADDR, val);
    else if (type == NIC_RX_RUNOUT)
        ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_RX_RUNOUT_STS_ADDR, val);
    else if (type == NIC_TX_DONE)
        ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_TX_DONE_STS_ADDR, (val << RTL9310_DMA_IF_INTR_TX_DONE_STS_TX_DONE_OFFSET) & RTL9310_DMA_IF_INTR_TX_DONE_STS_TX_DONE_MASK);
    else if (type == NIC_TX_ALLDONE)
        ioal_mem32_write(unit, RTL9310_DMA_IF_INTR_TX_DONE_STS_ADDR, (val << RTL9310_DMA_IF_INTR_TX_DONE_STS_TX_ALL_DONE_OFFSET) & RTL9310_DMA_IF_INTR_TX_DONE_STS_TX_ALL_DONE_MASK);
    else if (type == NIC_NTFY_DONE)
        ioal_mem32_write(unit, RTL9310_L2_NTFY_IF_INTR_STS_ADDR, (val << RTL9310_L2_NTFY_IF_INTR_STS_NTFY_DONE_OFFSET) & RTL9310_L2_NTFY_IF_INTR_STS_NTFY_DONE_MASK);
    else if (type == NIC_NTFY_BUF_RUNOUT)
        ioal_mem32_write(unit, RTL9310_L2_NTFY_IF_INTR_STS_ADDR, (val << RTL9310_L2_NTFY_IF_INTR_STS_NTFY_BUF_RUN_OUT_OFFSET) & RTL9310_L2_NTFY_IF_INTR_STS_NTFY_BUF_RUN_OUT_MASK);
    else if (type == NIC_NTFY_LOCALBUF_RUNOUT)
        ioal_mem32_write(unit, RTL9310_L2_NTFY_IF_INTR_STS_ADDR, (val << RTL9310_L2_NTFY_IF_INTR_STS_LOCAL_NTFY_BUF_RUN_OUT_OFFSET) & RTL9310_L2_NTFY_IF_INTR_STS_LOCAL_NTFY_BUF_RUN_OUT_MASK);

    return RT_ERR_OK;
}

int32 r9310_swNicRst_get(uint32 unit, uint32 *pStatus)
{
    ioal_mem32_field_read(unit, RTL9310_RST_GLB_CTRL_ADDR, RTL9310_RST_GLB_CTRL_SW_NIC_RST_OFFSET, RTL9310_RST_GLB_CTRL_SW_NIC_RST_MASK, pStatus);
    return RT_ERR_OK;
}

int32 r9310_swNicRst_set(uint32 unit)
{
    RT_LOG(LOG_DEBUG, MOD_NIC, "Reset NIC (R9310)... ");
    ioal_mem32_field_write(unit, RTL9310_RST_GLB_CTRL_ADDR, RTL9310_RST_GLB_CTRL_SW_NIC_RST_OFFSET, RTL9310_RST_GLB_CTRL_SW_NIC_RST_MASK, 1);
    return RT_ERR_OK;
}

int32 r9310_swQueRst_get(uint32 unit, uint32 *pStatus)
{
    ioal_mem32_field_read(unit, RTL9310_RST_GLB_CTRL_ADDR, RTL9310_RST_GLB_CTRL_SW_Q_RST_OFFSET, RTL9310_RST_GLB_CTRL_SW_Q_RST_MASK, pStatus);

    return RT_ERR_OK;
}

int32 r9310_swQueRst_set(uint32 unit)
{
    ioal_mem32_field_write(unit, RTL9310_RST_GLB_CTRL_ADDR, RTL9310_RST_GLB_CTRL_SW_Q_RST_OFFSET, RTL9310_RST_GLB_CTRL_SW_Q_RST_MASK, 1);
    osal_time_mdelay(50); /* delay 50mS */

    return RT_ERR_OK;
}

int32 r9310_cpuL2FloodMask_add(uint32 unit)
{
    uint32 val;

    ioal_mem32_field_read(unit, RTL9310_L2_UNKN_UC_FLD_PMSK_ADDR, 0, 0x1FFFFFF, &val);
    val |= 0x1000000;
    ioal_mem32_field_write(unit, RTL9310_L2_UNKN_UC_FLD_PMSK_ADDR, 0, 0x1FFFFFF, val);
    return RT_ERR_OK;
}

int32 r9310_cpuL2FloodMask_remove(uint32 unit)
{
    uint32 val;

    /* Remove the CPU port from Lookup Miss Flooding Portmask */
    ioal_mem32_field_read(unit, RTL9310_L2_UNKN_UC_FLD_PMSK_ADDR, 0, 0x1FFFFFF, &val);
    val &= ~(0x1000000);
    ioal_mem32_field_write(unit, RTL9310_L2_UNKN_UC_FLD_PMSK_ADDR, 0, 0x1FFFFFF, val);
    return RT_ERR_OK;
}

int32 r9310_cpuForceLinkupEnable_set(uint32 unit, rtk_enable_t enable)
{
    uint32      val;

    /* Force link */
    val = (enable == ENABLED) ? 1 : 0;
    ioal_mem32_field_write(unit, RTL9310_MAC_FORCE_MODE_CTRL_ADDR(56), \
        RTL9310_MAC_FORCE_MODE_CTRL_SMI_FORCE_LINK_EN_OFFSET, RTL9310_MAC_FORCE_MODE_CTRL_SMI_FORCE_LINK_EN_MASK, val);
    ioal_mem32_field_write(unit, RTL9310_MAC_FORCE_MODE_CTRL_ADDR(56), \
        RTL9310_MAC_FORCE_MODE_CTRL_SMI_FORCE_LINK_OFFSET, RTL9310_MAC_FORCE_MODE_CTRL_SMI_FORCE_LINK_MASK, val);

    /* Force duplex: Full duplex */
    ioal_mem32_field_write(unit, RTL9310_MAC_FORCE_MODE_CTRL_ADDR(56), \
        RTL9310_MAC_FORCE_MODE_CTRL_SMI_FORCE_DUP_EN_OFFSET, RTL9310_MAC_FORCE_MODE_CTRL_SMI_FORCE_DUP_EN_MASK, 1);
    ioal_mem32_field_write(unit, RTL9310_MAC_FORCE_MODE_CTRL_ADDR(56), \
        RTL9310_MAC_FORCE_MODE_CTRL_SMI_DUP_SEL_OFFSET, RTL9310_MAC_FORCE_MODE_CTRL_SMI_DUP_SEL_MASK, 1);

    /* Force speed: 1000M */
    ioal_mem32_field_write(unit, RTL9310_MAC_FORCE_MODE_CTRL_ADDR(56), \
        RTL9310_MAC_FORCE_MODE_CTRL_SMI_FORCE_SPD_EN_OFFSET, RTL9310_MAC_FORCE_MODE_CTRL_SMI_FORCE_SPD_EN_MASK, 1);
    ioal_mem32_field_write(unit, RTL9310_MAC_FORCE_MODE_CTRL_ADDR(56), \
        RTL9310_MAC_FORCE_MODE_CTRL_SMI_SPD_SEL_OFFSET, RTL9310_MAC_FORCE_MODE_CTRL_SMI_SPD_SEL_MASK, 2);

    /* Force flow control: disable */
    ioal_mem32_field_write(unit, RTL9310_MAC_FORCE_MODE_CTRL_ADDR(56), \
        RTL9310_MAC_FORCE_MODE_CTRL_SMI_FORCE_FC_EN_OFFSET, RTL9310_MAC_FORCE_MODE_CTRL_SMI_FORCE_FC_EN_MASK, 1);
    ioal_mem32_field_write(unit, RTL9310_MAC_FORCE_MODE_CTRL_ADDR(56), \
        RTL9310_MAC_FORCE_MODE_CTRL_SMI_RX_PAUSE_EN_OFFSET, RTL9310_MAC_FORCE_MODE_CTRL_SMI_RX_PAUSE_EN_MASK, 0);
    ioal_mem32_field_write(unit, RTL9310_MAC_FORCE_MODE_CTRL_ADDR(56), \
        RTL9310_MAC_FORCE_MODE_CTRL_SMI_TX_PAUSE_EN_OFFSET, RTL9310_MAC_FORCE_MODE_CTRL_SMI_TX_PAUSE_EN_MASK, 0);

    /* Egress Queue Drop */
    ioal_mem32_field_write(unit, RTL9310_FC_PORT_EGR_DROP_CTRL_ADDR(56), \
        RTL9310_FC_PORT_EGR_DROP_CTRL_REF_RXCNGST_OFFSET, RTL9310_FC_PORT_EGR_DROP_CTRL_REF_RXCNGST_MASK, 0);
    ioal_mem32_write(unit, RTL9310_FC_CPU_Q_EGR_FORCE_DROP_CTRL_ADDR(0), 0xffffffff);

    return RT_ERR_OK;
}

int32 r9310_holRingSize_set(uint32 unit, uint32 ring, uint32 val)
{
    ioal_mem32_field_write(unit, RTL9310_DMA_IF_RX_RING_SIZE_ADDR(ring), RTL9310_DMA_IF_RX_RING_SIZE_SIZE_OFFSET(ring),
                                RTL9310_DMA_IF_RX_RING_SIZE_SIZE_MASK(ring), val);
    return RT_ERR_OK;
}

int32 r9310_holRingCnt_get(uint32 unit, uint32 ring, uint32 *pVal)
{
    uint32 tmpVal;

    ioal_mem32_read(unit, RTL9310_DMA_IF_RX_RING_CNTR_ADDR(ring), &tmpVal);
    *pVal = (tmpVal & RTL9310_DMA_IF_RX_RING_CNTR_CNTR_MASK(ring)) >> RTL9310_DMA_IF_RX_RING_CNTR_CNTR_OFFSET(ring);

    return RT_ERR_OK;
}

int32 r9310_holRingCnt_set(uint32 unit, uint32 ring, uint32 val)
{
    ioal_mem32_write(unit, RTL9310_DMA_IF_RX_RING_CNTR_ADDR(ring), ((val << RTL9310_DMA_IF_RX_RING_CNTR_CNTR_OFFSET(ring)) & RTL9310_DMA_IF_RX_RING_CNTR_CNTR_MASK(ring)));

    return RT_ERR_OK;
}

int32 r9310_ntfyBaseAddr_get(uint32 unit, uintptr *pVal)
{
    uint32 val;

    ioal_mem32_read(unit, RTL9310_L2_NTFY_RING_BASE_ADDR_ADDR, &val);
    *pVal = (uintptr)val;
    return RT_ERR_OK;
}

int32 r9310_ntfyBaseAddr_set(uint32 unit, uintptr val)
{
    ioal_mem32_write(unit, RTL9310_L2_NTFY_RING_BASE_ADDR_ADDR, (uint32)val);
    return RT_ERR_OK;
}

int32 r9310_ringBaseAddr_get(uint32 unit, nic_dir_t dir, uint32 ring, uintptr *pVal)
{
    uint32  val;

    if (dir == NIC_DIR_RX)
        ioal_mem32_read(unit, RTL9310_DMA_IF_RX_BASE_DESC_ADDR_CTRL_ADDR(ring), &val);
    else
        ioal_mem32_read(unit, RTL9310_DMA_IF_TX_BASE_DESC_ADDR_CTRL_ADDR(ring), &val);

    *pVal = (uintptr)val;

    return RT_ERR_OK;
}

int32 r9310_ringBaseAddr_set(uint32 unit, nic_dir_t dir, uint32 ring, uintptr val)
{
    if (dir == NIC_DIR_RX)
        ioal_mem32_write(unit, RTL9310_DMA_IF_RX_BASE_DESC_ADDR_CTRL_ADDR(ring), (uint32)val);
    else
        ioal_mem32_write(unit, RTL9310_DMA_IF_TX_BASE_DESC_ADDR_CTRL_ADDR(ring), (uint32)val);
    return RT_ERR_OK;
}

int32 r9310_ringCurAddr_get(uint32 unit, nic_dir_t dir, uint32 ring, uintptr *pVal)
{
    uint32  val;

    if (dir == NIC_DIR_RX)
        ioal_mem32_read(unit, RTL9310_DMA_IF_RX_CUR_DESC_ADDR_CTRL_ADDR(ring), &val);
    else
        ioal_mem32_read(unit, RTL9310_DMA_IF_TX_CUR_DESC_ADDR_CTRL_ADDR(ring), &val);

    *pVal = (uintptr)val;

    return RT_ERR_OK;
}

int32 r9310_rxTruncateLength_get(uint32 unit, uint32 *pVal)
{
    ioal_mem32_field_read(unit, RTL9310_DMA_IF_CTRL_ADDR, RTL9310_DMA_IF_CTRL_RX_TRUNCATE_LEN_OFFSET,
                                                          RTL9310_DMA_IF_CTRL_RX_TRUNCATE_LEN_MASK, pVal);
    return RT_ERR_OK;
}

int32 r9310_rxTruncateLength_set(uint32 unit, uint32 val)
{
    ioal_mem32_field_write(unit, RTL9310_DMA_IF_CTRL_ADDR, RTL9310_DMA_IF_CTRL_RX_TRUNCATE_LEN_OFFSET,
                                                          RTL9310_DMA_IF_CTRL_RX_TRUNCATE_LEN_MASK, val);
    return RT_ERR_OK;
}

int32 r9310_nicEnable_get(uint32 unit, nic_dir_t dir, uint32 *pVal)
{
    if (dir == NIC_DIR_RX)
    {
        ioal_mem32_field_read(unit, RTL9310_DMA_IF_CTRL_ADDR, RTL9310_DMA_IF_CTRL_RX_EN_OFFSET,
                                                              RTL9310_DMA_IF_CTRL_RX_EN_MASK, pVal);
    }
    else
    {
        ioal_mem32_field_read(unit, RTL9310_DMA_IF_CTRL_ADDR, RTL9310_DMA_IF_CTRL_TX_EN_OFFSET,
                                                              RTL9310_DMA_IF_CTRL_TX_EN_MASK, pVal);
    }
    return RT_ERR_OK;
}

int32 r9310_nicEnable_set(uint32 unit, nic_dir_t dir, uint32 val)
{
    if (dir == NIC_DIR_RX)
    {
        ioal_mem32_field_write(unit, RTL9310_DMA_IF_CTRL_ADDR, RTL9310_DMA_IF_CTRL_RX_EN_OFFSET,
                                                              RTL9310_DMA_IF_CTRL_RX_EN_MASK, val);
    }
    else
    {
        ioal_mem32_field_write(unit, RTL9310_DMA_IF_CTRL_ADDR, RTL9310_DMA_IF_CTRL_TX_EN_OFFSET,
                                                              RTL9310_DMA_IF_CTRL_TX_EN_MASK, val);
    }

    if (val)
        RT_LOG(LOG_DEBUG, MOD_NIC, "NIC (R9310) Rx Start... ");
    else
        RT_LOG(LOG_DEBUG, MOD_NIC, "NIC (R9310) Rx Stop... ");

    return RT_ERR_OK;
}

int32 r9310_nicTxFetch_set(uint32 unit, nic_txRing_t type, uint32 val)
{
    if (type == NIC_TXRING_HIGH)
        ioal_mem32_field_write(unit, RTL9310_DMA_IF_CTRL_ADDR, RTL9310_DMA_IF_CTRL_TX_HIGH_FETCH_OFFSET,
                                                          RTL9310_DMA_IF_CTRL_TX_HIGH_FETCH_MASK, val);
    else
        ioal_mem32_field_write(unit, RTL9310_DMA_IF_CTRL_ADDR, RTL9310_DMA_IF_CTRL_TX_LOW_FETCH_OFFSET,
                                                          RTL9310_DMA_IF_CTRL_TX_LOW_FETCH_MASK, val);
    return RT_ERR_OK;
}

int32 r9310_nicTxBusySts_get(uint32 unit, nic_txRing_t type, uint32 *pVal)
{
    if (type == NIC_TXRING_HIGH)
        ioal_mem32_field_read(unit, RTL9310_DMA_IF_CTRL_ADDR, RTL9310_DMA_IF_CTRL_TX_HIGH_BUSY_OFFSET,
                                                          RTL9310_DMA_IF_CTRL_TX_HIGH_BUSY_MASK, pVal);
    else
        ioal_mem32_field_read(unit, RTL9310_DMA_IF_CTRL_ADDR, RTL9310_DMA_IF_CTRL_TX_LOW_BUSY_OFFSET,
                                                          RTL9310_DMA_IF_CTRL_TX_LOW_BUSY_MASK, pVal);
    return RT_ERR_OK;
}

int32 r9310_cpuTagId_get(uint32 unit, uint32 *pVal)
{
    ioal_mem32_field_read(unit, RTL9310_MAC_CPU_TAG_ID_CTRL_ADDR, RTL9310_MAC_CPU_TAG_ID_CTRL_CPU_TAG_ID_OFFSET,
                                                          RTL9310_MAC_CPU_TAG_ID_CTRL_CPU_TAG_ID_MASK, pVal);
    return RT_ERR_OK;
}

int32 r9310_cpuTagId_set(uint32 unit, uint32 val)
{
    ioal_mem32_field_write(unit, RTL9310_MAC_CPU_TAG_ID_CTRL_ADDR, RTL9310_MAC_CPU_TAG_ID_CTRL_CPU_TAG_ID_OFFSET,
                                                          RTL9310_MAC_CPU_TAG_ID_CTRL_CPU_TAG_ID_MASK, val);
    return RT_ERR_OK;
}

int32 r9310_supportJumboSize_get(uint32 unit, uint32 *pVal)
{
    *pVal = NIC_9310_JUMBO_SIZE;
    return RT_ERR_OK;
}

int32 r9310_cpuTagFromRaw_cnvt(uint32 unit, uint8 *pCpuTag, drv_nic_pkt_t *pPacket)
{
    nic_9310_cpuTag_t   *pHdrtag = (nic_9310_cpuTag_t *)pCpuTag;
    uint32              offset = 0, scan_offset = 0;
    uint8               pkt_data;
    uint8               *pData;
    uint32              ethType;

    RT_PARAM_CHK(NULL == pCpuTag, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pPacket, RT_ERR_NULL_POINTER);

    if (pHdrtag->un.rx.CPUTAGIF)
    {
        _nic_rx_reason_translate(pPacket, pHdrtag);
        if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ACL_HIT))
            pPacket->rx_tag.acl_index = pHdrtag->un.rx.IDX;
        if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_OF_HIT))
            pPacket->rx_tag.of_index = pHdrtag->un.rx.IDX;
        pPacket->rx_tag.physical_rx_port= pHdrtag->un.rx.SPA;
        pPacket->rx_tag.of_lumis_tblId  = pHdrtag->un.rx.OF_LUMIS_TBL_ID;
        pPacket->rx_tag.of_tblId        = pHdrtag->un.rx.OF_TBL_ID;
        pPacket->rx_tag.is_trk          = pHdrtag->un.rx.SPN_IS_TRK;
        pPacket->rx_tag.trk_id          = pHdrtag->un.rx.TRK_ID;
        pPacket->rx_tag.atk_type        = pHdrtag->un.rx.ATK_TYPE;
        pPacket->rx_tag.qid             = pHdrtag->un.rx.QID;
        pPacket->rx_tag.dev_id          = pHdrtag->un.rx.SPN >> 6;
        pPacket->rx_tag.source_port     = pHdrtag->un.rx.SPN & 0x3f;
        pPacket->rx_tag.mirror_hit      = pHdrtag->un.rx.MIR_HIT;
        if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_RX))
        {
            pPacket->rx_tag.ext_dev_id      = pPacket->rx_tag.dev_id;
            pPacket->rx_tag.ext_source_port = pPacket->rx_tag.source_port;
        }
        else if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_TX))
        {
            pPacket->rx_tag.ext_dev_id      = pHdrtag->un.rx.IDX >> 6;
            pPacket->rx_tag.ext_source_port = pHdrtag->un.rx.IDX & 0x3f;
        }
        else
        {
            pPacket->rx_tag.ext_is_trk      = pHdrtag->un.rx.PORT_DATA_IS_TRK;
            if (pPacket->rx_tag.ext_is_trk)
                pPacket->rx_tag.ext_trk_id      = pHdrtag->un.rx.PORT_DATA;
            else
            {
                pPacket->rx_tag.ext_dev_id      = pHdrtag->un.rx.PORT_DATA >> 6;
                pPacket->rx_tag.ext_source_port = pHdrtag->un.rx.PORT_DATA & 0x3f;
            }
        }
        pPacket->rx_tag.tt_idx          = pHdrtag->un.rx.TT_IDX;
        pPacket->rx_tag.svid_tagged     = pHdrtag->un.rx.OTAGIF;
        pPacket->rx_tag.cvid_tagged     = pHdrtag->un.rx.ITAGIF;
        pPacket->rx_tag.fvid_sel        = pHdrtag->un.rx.FVID_SEL;
        pPacket->rx_tag.fvid            = pHdrtag->un.rx.FVID;
        pPacket->rx_tag.dm_rxIdx        = pHdrtag->un.rx.DM_RXIDX;
        pPacket->rx_tag.oversize        = pHdrtag->un.rx.OVERSIZE;
        pPacket->rx_tag.reason          = pHdrtag->un.rx.REASON;
    }
    else
    {
        nic_9310_cpuTag_t cputag;

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
            if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ACL_HIT))
                pPacket->rx_tag.acl_index = cputag.un.rx.IDX;
            if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_OF_HIT))
                pPacket->rx_tag.of_index = cputag.un.rx.IDX;
            pPacket->rx_tag.physical_rx_port= cputag.un.rx.SPA;
            pPacket->rx_tag.of_lumis_tblId  = cputag.un.rx.OF_LUMIS_TBL_ID;
            pPacket->rx_tag.of_tblId        = cputag.un.rx.OF_TBL_ID;
            pPacket->rx_tag.is_trk          = cputag.un.rx.SPN_IS_TRK;
            pPacket->rx_tag.trk_id          = cputag.un.rx.TRK_ID;
            pPacket->rx_tag.atk_type        = cputag.un.rx.ATK_TYPE;
            pPacket->rx_tag.qid             = cputag.un.rx.QID;
            pPacket->rx_tag.dev_id          = cputag.un.rx.SPN >> 6;
            pPacket->rx_tag.source_port     = cputag.un.rx.SPN & 0x3f;
            pPacket->rx_tag.mirror_hit      = cputag.un.rx.MIR_HIT;
            if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_RX))
            {
                pPacket->rx_tag.ext_dev_id      = pPacket->rx_tag.dev_id;
                pPacket->rx_tag.ext_source_port = pPacket->rx_tag.source_port;
            }
            else if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_TX))
            {
                pPacket->rx_tag.ext_dev_id      = pHdrtag->un.rx.IDX >> 6;
                pPacket->rx_tag.ext_source_port = pHdrtag->un.rx.IDX & 0x3f;
            }
            else
            {
                pPacket->rx_tag.ext_is_trk      = pHdrtag->un.rx.PORT_DATA_IS_TRK;
                if (pPacket->rx_tag.ext_is_trk)
                    pPacket->rx_tag.ext_trk_id      = pHdrtag->un.rx.PORT_DATA;
                else
                {
                    pPacket->rx_tag.ext_dev_id      = pHdrtag->un.rx.PORT_DATA >> 6;
                    pPacket->rx_tag.ext_source_port = pHdrtag->un.rx.PORT_DATA & 0x3f;
                }
            }
            pPacket->rx_tag.tt_idx          = cputag.un.rx.TT_IDX;
            pPacket->rx_tag.svid_tagged     = cputag.un.rx.OTAGIF;
            pPacket->rx_tag.cvid_tagged     = cputag.un.rx.ITAGIF;
            pPacket->rx_tag.fvid_sel        = cputag.un.rx.FVID_SEL;
            pPacket->rx_tag.fvid            = cputag.un.rx.FVID;
            pPacket->rx_tag.dm_rxIdx        = cputag.un.rx.DM_RXIDX;
            pPacket->rx_tag.oversize        = cputag.un.rx.OVERSIZE;
            pPacket->rx_tag.reason          = cputag.un.rx.REASON;
        }
    }

    return RT_ERR_OK;
}

int32 r9310_cpuTagToRaw_cnvt(uint32 unit, drv_nic_pkt_t *pPacket, uint8 *pCpuTag)
{
    nic_9310_cpuTag_t   *pHdrtag = (nic_9310_cpuTag_t *)pCpuTag;

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
        pHdrtag->un.tx.DM_PKT        = pPacket->tx_tag.dm_pkt;
        pHdrtag->un.tx.DG_PKT        = pPacket->tx_tag.dg_pkt;
        pHdrtag->un.tx.BP_FLTR       = pPacket->tx_tag.bp_fltr;
        pHdrtag->un.tx.BP_STP        = pPacket->tx_tag.bp_stp;
        pHdrtag->un.tx.BP_VLAN_EGR   = pPacket->tx_tag.bp_vlan_egr;
        pHdrtag->un.tx.AS_TAGSTS     = pPacket->tx_tag.as_tagSts;
        pHdrtag->un.tx.L3_ACT        = pPacket->tx_tag.l3_act;
        pHdrtag->un.tx.ORI_TAGIF_EN  = pPacket->tx_tag.ori_tagif_en;
        pHdrtag->un.tx.AS_QID        = pPacket->tx_tag.as_priority;
        pHdrtag->un.tx.QID           = pPacket->tx_tag.priority;
        pHdrtag->un.tx.ORI_ITAGIF    = pPacket->tx_tag.ori_itagif;
        pHdrtag->un.tx.ORI_OTAGIF    = pPacket->tx_tag.ori_otagif;
        pHdrtag->un.tx.FVID_SEL      = pPacket->tx_tag.fvid_sel;
        pHdrtag->un.tx.FVID_EN       = pPacket->tx_tag.fvid_en;
        pHdrtag->un.tx.FVID          = pPacket->tx_tag.fvid;
        pHdrtag->un.tx.SRC_FLTR_EN   = pPacket->tx_tag.src_filter_en;
        pHdrtag->un.tx.SP_IS_TRK     = pPacket->tx_tag.sp_is_trk;
        pHdrtag->un.tx.SPN9_4        = pPacket->tx_tag.spn >> 4;
        pHdrtag->un.tx.SPN3_0        = pPacket->tx_tag.spn & 0xf;
        pHdrtag->un.tx.SW_DEV_ID     = pPacket->tx_tag.dev_id;
        pHdrtag->un.tx.DPM55_32      = pPacket->tx_tag.dst_port_mask_1;
        pHdrtag->un.tx.DPM31_0       = pPacket->tx_tag.dst_port_mask;
    }
    else
        pHdrtag->un.tx.CPUTAGIF = FALSE;

    return RT_ERR_OK;
}

int32 r9310_cpuTag_dump(uint32 unit, drv_nic_pkt_t *pPacket)
{
    osal_printf("=== [NIC RX Debug - CPU Rx Tag Information] ============ \n");
    osal_printf(" RPN : %d \n", pPacket->rx_tag.physical_rx_port);
    osal_printf(" UNIT : %d \n", pPacket->rx_tag.dev_id);
    osal_printf(" SPN : %d \n", pPacket->rx_tag.source_port);
    osal_printf(" OF_LU_MIS_TBL_ID : %d \n", pPacket->rx_tag.of_lumis_tblId);
    osal_printf(" MIR_HIT : %d \n", pPacket->rx_tag.mirror_hit);
    if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_RX))
        osal_printf(" SFLOW_HIT : 1 \n");
    else if (IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_SFLOW_TX))
        osal_printf(" SFLOW_HIT : 2 \n");
    else
        osal_printf(" SFLOW_HIT : 0 \n");
    osal_printf(" ACL_HIT : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_ACL_HIT));
    osal_printf(" OF_HIT : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_OF_HIT));
    osal_printf(" IDX : %d \n", pPacket->rx_tag.acl_index);
    osal_printf(" TT_HIT : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_TUNL_TMNT));
    osal_printf(" TT_IDX : %d \n", pPacket->rx_tag.tt_idx);
    osal_printf(" OF_TBL_ID : %d \n", pPacket->rx_tag.of_tblId);
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
    osal_printf(" DM_RXIDX : %d \n", pPacket->rx_tag.dm_rxIdx);
    osal_printf(" NEW_SA : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_NEW_SA));
    osal_printf(" PMV_FORBID : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_PMV_FBD));
    osal_printf(" L2_STTC_PMV : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_STC_L2_PMV));
    osal_printf(" L2_DYN_PMV : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_DYN_L2_PMV));
    osal_printf(" L2_ERR_PKT : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_CRC_ERROR));
    osal_printf(" L3_ERR_PKT : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L3_CHKSUM_ERROR));
    osal_printf(" OVERSIZE : %d \n", pPacket->rx_tag.oversize);
    if (pPacket->rx_tag.ext_is_trk)
        osal_printf(" ext_trkId : %d \n", pPacket->rx_tag.ext_trk_id);
    else
    {
        osal_printf(" ext_unit : %d \n", pPacket->rx_tag.ext_dev_id);
        osal_printf(" ext_source_port : %d \n", pPacket->rx_tag.ext_source_port);
    }
    osal_printf(" HASH_FULL : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_L2_HASH_FULL));
    osal_printf(" INVALID_SA : %d \n", IS_NIC_REASON_MASK_SET(*pPacket, NIC_RX_REASON_INVALID_SA));
    osal_printf(" REASON : %d \n", pPacket->rx_tag.reason);

    return RT_ERR_OK;
}

int32 r9310_rawTag_dump(uint32 unit, uint8 *pCpuTag)
{
    nic_9310_cpuTag_t   *pHdrtag = (nic_9310_cpuTag_t *)pCpuTag;

    osal_printf("=== [NIC TX Debug - CPU Tx Tag Information] ============ \n");
    osal_printf(" FWD_TYPE : 0x%0x \n", pHdrtag->un.tx.FWD_TYPE);
    osal_printf(" ACL_ACT : 0x%0x \n", pHdrtag->un.tx.ACL_ACT);
    osal_printf(" CNGST_DROP : 0x%0x \n", pHdrtag->un.tx.CNGST_DROP);
    osal_printf(" DM_PKT : 0x%0x \n", pHdrtag->un.tx.DM_PKT);
    osal_printf(" DG_PKT : 0x%0x \n", pHdrtag->un.tx.DG_PKT);
    osal_printf(" BP_FLTR : 0x%0x \n", pHdrtag->un.tx.BP_FLTR);
    osal_printf(" BP_STP : 0x%0x \n", pHdrtag->un.tx.BP_STP);
    osal_printf(" BP_VLAN_EGR : 0x%0x \n", pHdrtag->un.tx.BP_VLAN_EGR);
    osal_printf(" AS_TAGSTS : 0x%0x \n", pHdrtag->un.tx.AS_TAGSTS);
    osal_printf(" ORI_TAGIF_EN : 0x%0x \n", pHdrtag->un.tx.ORI_TAGIF_EN);
    osal_printf(" L3_ACT : 0x%0x \n", pHdrtag->un.tx.L3_ACT);
    osal_printf(" AS_QID : 0x%0x \n", pHdrtag->un.tx.AS_QID);
    osal_printf(" QID : 0x%0x \n", pHdrtag->un.tx.QID);
    osal_printf(" ORI_ITAGIF : 0x%0x \n", pHdrtag->un.tx.ORI_ITAGIF);
    osal_printf(" ORI_OTAGIF : 0x%0x \n", pHdrtag->un.tx.ORI_OTAGIF);
    osal_printf(" FVID_SEL : 0x%0x \n", pHdrtag->un.tx.FVID_SEL);
    osal_printf(" FVID_EN : 0x%0x \n", pHdrtag->un.tx.FVID_EN);
    osal_printf(" FVID : 0x%0x \n", pHdrtag->un.tx.FVID);
    osal_printf(" SRC_FLTR_EN : 0x%0x \n", pHdrtag->un.tx.SRC_FLTR_EN);
    osal_printf(" SP_IS_TRK : 0x%0x \n", pHdrtag->un.tx.SP_IS_TRK);
    osal_printf(" SPN : 0x%0x \n", (pHdrtag->un.tx.SPN9_4 << 4) | pHdrtag->un.tx.SPN3_0);
    osal_printf(" SW_DEV_ID : 0x%0x \n", pHdrtag->un.tx.SW_DEV_ID);
    osal_printf(" DPM55_32 : 0x%0x \n", pHdrtag->un.tx.DPM55_32);
    osal_printf(" DPM31_0 : 0x%0x \n", pHdrtag->un.tx.DPM31_0);

    return RT_ERR_OK;
}

