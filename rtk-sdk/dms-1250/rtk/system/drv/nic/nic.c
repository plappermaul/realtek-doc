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
 * Purpose : Definition of Network Interface Controller API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) CPU Tag
 *           (2) NIC Tx
 *           (3) NIC Rx
 *
 */

/*
 * Include Files
 */
#include <dev_config.h>
#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
#include <osal/wait.h>
#include <osal/workqueue.h>
#include <osal/atomic.h>
#endif
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_mapper.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <drv/l2ntfy/l2ntfy.h>
#include <private/drv/l2ntfy/l2ntfy_util.h>
#include <soc/type.h>
#include <common/debug/rt_log.h>
#include <common/error.h>
#include <ioal/mem32.h>
#include <osal/isr.h>
#include <osal/cache.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/print.h>
#include <osal/thread.h>
#include <osal/time.h>
#include <osal/sem.h>
#include <osal/spl.h>
#include <private/drv/nic/nic_diag.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#include <hal/chipdef/chip.h>
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
#include <drv/l2ntfy/l2ntfy.h>
#endif
#include <hwp/hw_profile.h>
#include <private/drv/swcore/swcore_rtl9310.h>
#include <common/util/rt_util_time.h>

/*
 * Symbol Definition
 */
#define CACHE_LINE_SIZE                 32

#define NIC_RING_WRAPBIT                (0x1 << 1)
#define NIC_RING_SWOWNBIT               (0x1 << 0)
#define NIC_ADDR_MASK                   0xFFFFFFFC
#define DEBUG_DUMP_PKT_LEN              256
#define MAX_NIC_PKT_CNT                 10000
#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
#define JUMBO_CLUSTER_NUM               5
#define NIC_RX_THREAD_STACK_SIZE        8192
#define NIC_RX_THREAD_PRI               0
#endif


/*
 * Data Type Definition
 */
typedef struct nic_rx_cb_entry_s
{
    drv_nic_rx_cb_f rx_callback;
    void *pCookie;
} nic_rx_cb_entry_t;

typedef struct nic_pkthdr_s
{
    uint8 *buf_addr;
#if defined(CONFIG_SDK_ENDIAN_LITTLE)
    /* word [0] */
    uint16  buf_size;
    uint16  reserve;
    /* word [1] */
    uint16  buf_len;
    uint16  pkt_offset:14;
    uint16      :1;
    uint16  more:1;
#else
    /* word [0] */
    uint16  reserve;
    uint16  buf_size;
    /* word [1] */
    uint16  more:1;
    uint16      :1;
    uint16  pkt_offset:14;
    uint16  buf_len;
#endif
    uint8   cpuTag[CPUTAG_SIZE];

    /* Used by Software */
    struct drv_nic_pkt_s *packet;
    uint32  *ring_entry;
    drv_nic_tx_cb_f tx_callback;
    void    *cookie;
} nic_pkthdr_t;

#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
typedef struct nic_collectArrayList_s
{
    uint32                  unit;
    osal_list_head_t        list;
    struct drv_nic_pkt_s    *pPacket;
    uint8                   jumbo;
} nic_collectArrayList_t;
#endif

/*
 * Data Declaration
 */
static uint32   nic_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static nic_rx_cb_entry_t _nic_rx_cb_tbl[NIC_RX_CB_PRIORITY_NUMBER];
static drv_nic_initCfg_t _nic_init_conf;

/* Pointer for Software */
static uint32       **pNic_rxFDPBase;
static uint32       **pNic_txFDPBase;
static uint32       **pNic_rxCDPIdx;
static uint32       **pNic_txCDPIdx;
static uint32       **pNic_rxRDPIdx;
static uint32       **pNic_txRDPIdx;
nic_pkthdr_t        *pRxPktHdr;
nic_pkthdr_t        *pTxPktHdr;

static drv_nic_pkt_t **pPacket;
static uint32       *jumboFlag;

static uint32       *rxRingIdSize;
static uint32       *txRingIdSize;
static uint32       rxRingNum;
static uint32       txRingNum;
static uint32       totalRxRingNum = 0, totalTxRingNum = 0;

/* NIC Tx/Rx debug information
 * The machanism is always enabled
 */
static uint32       nic_debug_flag;
static uint32       nic_tx_success_cntr;
static uint32       nic_tx_failed_cntr;
static uint32       nic_rx_success_cntr;
static uint32       nic_rx_failed_cntr;
static uint32       nic_tx_isr_cntr;
static uint32       nic_tx_ring_cntr;

static uint32       cpuTagId;
static uint32       rxCRCInclude;
static uint32       gJumboSize;

#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
static osal_event_t                 drv_nicIsr_event;
static osal_atomic_t                nic_rx_pkt_cnt = OSAL_ATOMIC_INIT(0);
static osal_atomic_t                nic_mem_lock = OSAL_ATOMIC_INIT(0);
static uint32                       nicLockCnt, nicReleaseCnt;
static nic_collectArrayList_t       nicListHead;
#endif


static isr_param_t      my_isr_param[RTK_MAX_NUM_OF_UNIT];
static uint32           gIsrRegistered;
#ifndef __BOOTLOADER__
osal_spinlock_t         spl_rx_lock;
osal_spinlock_t         spl_tx_lock;
#endif


/*
 * Macro Definition
 */
#ifdef __BOOTLOADER__
  #define NIC_RX_LOCK(unit, flags)
  #define NIC_RX_UNLOCK(unit, flags)
  #define NIC_TX_LOCK(unit, flags)
  #define NIC_TX_UNLOCK(unit, flags)
#else
#define NIC_RX_LOCK(unit, flags)               osal_spl_spin_lock_irqsave(spl_rx_lock, flags)
#define NIC_RX_UNLOCK(unit, flags)             osal_spl_spin_unlock_irqrestore(spl_rx_lock, flags)
#define NIC_TX_LOCK(unit, flags)               osal_spl_spin_lock_irqsave(spl_tx_lock, flags)
#define NIC_TX_UNLOCK(unit, flags)             osal_spl_spin_unlock_irqrestore(spl_tx_lock, flags)
#endif
#define MEMORY_BARRIER()        ({ __asm__ __volatile__ ("": : :"memory"); })

/*
 * Function Declaration
 */
int32 _nic_ringInfo_get(uint32 unit, nic_dir_t type, uint32 **rxRingIdSizeList, uint32 *pRingNum)
{
    return NIC_CTRL(unit).ringInfo_get(unit, type, rxRingIdSizeList, pRingNum);
}

int32 _nic_cpuPortTxRxEnable_set(uint32 unit, rtk_enable_t enable)
{
    return NIC_CTRL(unit).cpuPortTxRxEnable_set(unit, enable);
}

int32 _nic_intrMask_get(uint32 unit, nic_intr_type_t type, uint32 *pMask)
{
    return NIC_CTRL(unit).intrMask_get(unit, type, pMask);
}

int32 _nic_intrMask_set(uint32 unit, nic_intr_type_t type, uint32 mask)
{
    return NIC_CTRL(unit).intrMask_set(unit, type, mask);
}

int32 _nic_intrSts_get(uint32 unit, nic_intr_type_t type, uint32 *pVal)
{
    return NIC_CTRL(unit).intrSts_get(unit, type, pVal);
}

int32 _nic_intrSts_set(uint32 unit, nic_intr_type_t type, uint32 val)
{
    return NIC_CTRL(unit).intrSts_set(unit, type, val);
}

int32 _nic_swNicRst_get(uint32 unit, uint32 *pStatus)
{
    return NIC_CTRL(unit).swNicRst_get(unit, pStatus);
}

int32 _nic_swNicRst_set(uint32 unit)
{
    return NIC_CTRL(unit).swNicRst_set(unit);
}

int32 _nic_swQueRst_get(uint32 unit, uint32 *pStatus)
{
    return NIC_CTRL(unit).swQueRst_get(unit, pStatus);
}

int32 _nic_swQueRst_set(uint32 unit)
{
    return NIC_CTRL(unit).swQueRst_set(unit);
}

int32 _nic_cpuL2FloodMask_add(uint32 unit)
{
    return NIC_CTRL(unit).cpuL2FloodMask_add(unit);
}

int32 _nic_cpuL2FloodMask_remove(uint32 unit)
{
    return NIC_CTRL(unit).cpuL2FloodMask_remove(unit);
}

int32 _nic_cpuForceLinkupEnable_set(uint32 unit, rtk_enable_t enable)
{
    return NIC_CTRL(unit).cpuForceLinkupEnable_set(unit, enable);
}

int32 _nic_holRingCnt_get(uint32 unit, uint32 ring, uint32 *pVal)
{
    return NIC_CTRL(unit).holRingCnt_get(unit, ring, pVal);
}

int32 _nic_holRingSize_set(uint32 unit, uint32 ring, uint32 val)
{
    return NIC_CTRL(unit).holRingSize_set(unit, ring, val);
}

int32 _nic_holRingCnt_set(uint32 unit, uint32 ring, uint32 val)
{
    return NIC_CTRL(unit).holRingCnt_set(unit, ring, val);
}

int32 _nic_ntfyBaseAddr_get(uint32 unit, uintptr *pAddr)
{
    return NIC_CTRL(unit).ntfyBaseAddr_get(unit, pAddr);
}

int32 _nic_ntfyBaseAddr_set(uint32 unit, uintptr addr)
{
    return NIC_CTRL(unit).ntfyBaseAddr_set(unit, addr);
}

int32 _nic_ringBaseAddr_get(uint32 unit, nic_dir_t dir, uint32 ring, uintptr *pAddr)
{
    return NIC_CTRL(unit).ringBaseAddr_get(unit, dir, ring, pAddr);
}

int32 _nic_ringBaseAddr_set(uint32 unit, nic_dir_t dir, uint32 ring, uintptr addr)
{
    return NIC_CTRL(unit).ringBaseAddr_set(unit, dir, ring, addr);
}

int32 _nic_ringCurAddr_get(uint32 unit, nic_dir_t dir, uint32 ring, uintptr *pAddr)
{
    return NIC_CTRL(unit).ringCurAddr_get(unit, dir, ring, pAddr);
}

int32 _nic_rxTruncateLength_get(uint32 unit, uint32 *pVal)
{
    return NIC_CTRL(unit).rxTruncateLength_get(unit, pVal);
}

int32 _nic_rxTruncateLength_set(uint32 unit, uint32 val)
{
    return NIC_CTRL(unit).rxTruncateLength_set(unit, val);
}

int32 _nic_nicEnable_get(uint32 unit, nic_dir_t dir, uint32 *pVal)
{
    return NIC_CTRL(unit).nicEnable_get(unit, dir, pVal);
}

int32 _nic_nicEnable_set(uint32 unit, nic_dir_t dir, uint32 val)
{
    return NIC_CTRL(unit).nicEnable_set(unit, dir, val);
}

int32 _nic_nicTxFetch_set(uint32 unit, nic_txRing_t type, uint32 val)
{
    return NIC_CTRL(unit).nicTxFetch_set(unit, type, val);
}

int32 _nic_nicTxBusySts_get(uint32 unit, nic_txRing_t type, uint32 *pVal)
{
    return NIC_CTRL(unit).nicTxBusySts_get(unit, type, pVal);
}

int32 _nic_cpuTagId_get(uint32 unit, uint32 *pVal)
{
    return NIC_CTRL(unit).cpuTagId_get(unit, pVal);
}

int32 _nic_cpuTagId_set(uint32 unit, uint32 val)
{
    return NIC_CTRL(unit).cpuTagId_set(unit, val);
}

int32 _nic_supportJumboSize_get(uint32 unit, uint32 *pVal)
{
    return NIC_CTRL(unit).supportJumboSize_get(unit, pVal);
}

int32 _nic_cpuTagFromRaw_cnvt(uint32 unit, uint8 *pCpuTag, drv_nic_pkt_t *pPacket)
{
    return NIC_CTRL(unit).cpuTagFromRaw_cnvt(unit, pCpuTag, pPacket);
}

int32 _nic_cpuTagToRaw_cnvt(uint32 unit, drv_nic_pkt_t *pPacket, uint8 *pCpuTag)
{
    return NIC_CTRL(unit).cpuTagToRaw_cnvt(unit, pPacket, pCpuTag);
}

int32 _nic_cpuTag_dump(uint32 unit, drv_nic_pkt_t *pPacket)
{
    return NIC_CTRL(unit).cpuTag_dump(unit, pPacket);
}

int32 _nic_rawTag_dump(uint32 unit, uint8 *pCpuTag)
{
    return NIC_CTRL(unit).rawTag_dump(unit, pCpuTag);
}


/*
 * Function Declaration
 */
int32
_nic_isr_rxRoutine(uint32 unit, uint32 ringId)
{
    uint32  i;
    uintptr temp;
    int32   ret = RT_ERR_FAILED;
    uint32  releaseCnt = 0;
    drv_nic_rx_t nic_rx_handle = NIC_RX_NOT_HANDLED;
#if !defined(__BOOTLOADER__)
    unsigned long   spl_flags;
#endif

    RT_LOG(LOG_DEBUG, MOD_NIC, "ringId = %d", ringId);

    NIC_RX_LOCK(unit, &spl_flags);

    if (ringId >= rxRingNum)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "invalid ringId(%d)!", ringId);
        nic_rx_failed_cntr++;
        NIC_RX_UNLOCK(unit, &spl_flags);
        return RT_ERR_FAILED;
    }

    /* Update software current pointer */
    _nic_ringCurAddr_get(unit, NIC_DIR_RX, ringId, &temp);
    pNic_rxCDPIdx[ringId] = (uint32 *)temp;    /* The limitation */


    do
    {
        uint32          offset = 0, scan_offset = 0;
        uint8           pkt_data;
        uint8           jumbo_first_cluster = 0;
        nic_pkthdr_t    *pktHdr;
#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
        uint32                  value;
        nic_collectArrayList_t  *pEntry;
#endif

        if ((*pNic_rxRDPIdx[ringId] & NIC_RING_SWOWNBIT) != 0)
            break;

        /* Prepare to be reclaim */
        pktHdr = (nic_pkthdr_t *)(*pNic_rxRDPIdx[ringId] & NIC_ADDR_MASK);
        if (HWP_8380_30_FAMILY(unit))
            pktHdr = (nic_pkthdr_t*)((uint32)pktHdr | 0xa0000000);

        if (NULL == pktHdr || NULL == pktHdr->buf_addr || NULL == pktHdr->packet)
            break;

#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
        if (!jumboFlag[ringId])
        {
            if (osal_atomic_read(&nic_mem_lock))
            {
                NIC_RX_UNLOCK(unit, &spl_flags);
                return RT_ERR_OK;
            }
            if ((value = osal_atomic_read(&nic_rx_pkt_cnt)) + 1 > MAX_NIC_PKT_CNT)
            {
                osal_atomic_set(&nic_mem_lock, 1);
                _nic_intrMask_set(unit, NIC_RX_DONE, 0);
                _nic_intrMask_set(unit, NIC_RX_RUNOUT, 0);
                nicLockCnt++;
                NIC_RX_UNLOCK(unit, &spl_flags);
                return RT_ERR_OK;
            }

            if (pktHdr->more)
                osal_atomic_add_return(JUMBO_CLUSTER_NUM, &nic_rx_pkt_cnt);
            else
            {
                osal_atomic_add_return(1, &nic_rx_pkt_cnt);
            }
        }
#endif

        /* NIC Rx debug message */
        if (nic_debug_flag & DEBUG_RX_RAW_LEN_BIT)
        {
            int dump_len = DEBUG_DUMP_PKT_LEN; /* debug dump maximum length */

            osal_printf("=== [NIC RX Debug] ================================= Len: %d \n", pktHdr->buf_len);
            for (i = 0; i < dump_len; i++)
            {
                if (i == (pktHdr->buf_len))
                    break;
                if (0 == (i % 16))
                    osal_printf("[%04X] ", i);
                osal_printf("%02X ", *(pktHdr->packet->data + i));
                if (15 == (i % 16))
                    osal_printf("\n");
            }
            osal_printf("\n");
        }


        if (pktHdr->more && jumboFlag[ringId] == FALSE)     /* Jumbo head */
        {
            jumboFlag[ringId] = TRUE;
            if (RT_ERR_OK == _nic_init_conf.pkt_alloc(unit, gJumboSize, 0, &pPacket[ringId]))
            {
                jumbo_first_cluster = TRUE;
                pPacket[ringId]->length = pktHdr->buf_len;
                osal_memcpy(pPacket[ringId]->data, pktHdr->packet->data, pktHdr->buf_len);
            }
            else
            {
                RT_LOG(LOG_DEBUG, MOD_NIC, "Allocate jumbo frame failed.");
                pPacket[ringId] = NULL;
                nic_rx_failed_cntr++;
            }
        }
        else if (jumboFlag[ringId] == TRUE)     /* Jumbo other */
        {
            if (pPacket[ringId])
            {
                osal_memcpy(pPacket[ringId]->data + pPacket[ringId]->length, pktHdr->packet->data, pktHdr->buf_len);
                pPacket[ringId]->length += pktHdr->buf_len;
                pPacket[ringId]->tail = pPacket[ringId]->data + pPacket[ringId]->length;
            }
        }
        else        /* Normal packet */
        {
            pPacket[ringId] = pktHdr->packet;
            pPacket[ringId]->length = pktHdr->buf_len;
            pPacket[ringId]->tail = pPacket[ringId]->data + pPacket[ringId]->length;
        }

        if ((jumbo_first_cluster || jumboFlag[ringId] == 0) && pPacket[ringId] != NULL)
        {
            if (RT_ERR_OK != _nic_cpuTagFromRaw_cnvt(unit, pktHdr->cpuTag, pPacket[ringId]))
            {
                nic_rx_failed_cntr++;
                _nic_init_conf.pkt_free(unit, pPacket[ringId]);

                /* Jump to next */
                pNic_rxRDPIdx[ringId] += 1;
                if (pNic_rxRDPIdx[ringId] == (pNic_rxFDPBase[ringId] + rxRingIdSize[ringId]))
                    pNic_rxRDPIdx[ringId] = pNic_rxFDPBase[ringId];

                NIC_RX_UNLOCK(unit, &spl_flags);
                return RT_ERR_FAILED;
            }

            scan_offset = 12;
            if (pPacket[ringId]->rx_tag.svid_tagged)
            {
                pkt_data = *(pPacket[ringId]->data + scan_offset + 2);
                pPacket[ringId]->rx_tag.outer_pri = (pkt_data >> 5) & 0x7;
                pPacket[ringId]->rx_tag.outer_vid = ((pkt_data & 0xF) << 8);
                pkt_data = *(pPacket[ringId]->data + scan_offset + 3);
                pPacket[ringId]->rx_tag.outer_vid |= (pkt_data & 0xFF);
                scan_offset += 4;
            }
            else
            {
                 pPacket[ringId]->rx_tag.outer_pri = 0;
                 pPacket[ringId]->rx_tag.outer_vid = 0;
            }
            if (pPacket[ringId]->rx_tag.cvid_tagged)
            {
                pkt_data = *(pPacket[ringId]->data + scan_offset + 2);
                pPacket[ringId]->rx_tag.inner_pri = (pkt_data >> 5) & 0x7;
                pPacket[ringId]->rx_tag.inner_vid = ((pkt_data & 0xF) << 8);
                pkt_data = *(pPacket[ringId]->data + scan_offset + 3);
                pPacket[ringId]->rx_tag.inner_vid |= (pkt_data & 0xFF);
            }
            else
            {
                 pPacket[ringId]->rx_tag.inner_pri = 0;
                 pPacket[ringId]->rx_tag.inner_vid = 0;
            }
            pPacket[ringId]->length -= offset;

            /* NIC Rx debug message */
            if (nic_debug_flag & DEBUG_RX_CPU_TAG_BIT)
                _nic_cpuTag_dump(unit, pPacket[ringId]);
        }



        if (jumboFlag[ringId] == 0 || (jumboFlag[ringId] && pktHdr->more == 0 && pPacket[ringId] != NULL))
        {
            nic_rx_handle = NIC_RX_NOT_HANDLED;
            if (0 == rxCRCInclude)
            {
                /* packet passed to higher layer doesn't need CRC field */
                pPacket[ringId]->length -= 4;
                pPacket[ringId]->tail -= 4;
            }

#if !defined(CONFIG_SDK_NIC_RX_CB_IN_THREAD)
            for (i = 0; i < NIC_RX_CB_PRIORITY_NUMBER; i++)
            {
                if (_nic_rx_cb_tbl[i].rx_callback != NULL)
                {
                    nic_rx_handle = _nic_rx_cb_tbl[i].rx_callback(unit, pPacket[ringId], _nic_rx_cb_tbl[i].pCookie);
                    if (NIC_RX_HANDLED_OWNED == nic_rx_handle)
                    {
                        break;
                    }
                }
            }

            if (nic_rx_handle != NIC_RX_HANDLED_OWNED)
            {   /* We have to free this packet here */
                _nic_init_conf.pkt_free(unit, pPacket[ringId]);
            }
#else
            pEntry = osal_alloc(sizeof(nic_collectArrayList_t));
            if (NULL != pEntry)
            {
                pEntry->pPacket = pPacket[ringId];
                if (jumboFlag[ringId] && pktHdr->more == 0)
                    pEntry->jumbo = 1;
                else
                {
                    pEntry->jumbo = 0;
                }
                pEntry->unit = unit;
                osal_list_add_tail(&pEntry->list, &nicListHead.list);
            }
            else
            {
                _nic_init_conf.pkt_free(unit, pPacket[ringId]);
            }
#endif
        }

        if (jumboFlag[ringId] == 0)
        {
            pktHdr->packet = NULL;
            /* Alloc a new packet data buffer */
            if (RT_ERR_OK == _nic_init_conf.pkt_alloc(unit, _nic_init_conf.pkt_size, 0, &pPacket[ringId]))
            {
                pktHdr->packet = pPacket[ringId];
                if (HWP_8380_30_FAMILY(unit))
                    pktHdr->buf_addr = (uint8 *)(UNCACHE(pPacket[ringId]->data) & 0x0fffffff);
                else
                    pktHdr->buf_addr = (uint8 *)(UNCACHE(pPacket[ringId]->data));
                pktHdr->buf_size = _nic_init_conf.pkt_size;
                pktHdr->buf_len = 0;
                if ((ret = osal_cache_memory_flush((uint32)pPacket[ringId]->head, (pPacket[ringId]->end - pPacket[ringId]->head))) != RT_ERR_OK)
                {
                    _nic_init_conf.pkt_free(unit, pPacket[ringId]);
                    NIC_RX_UNLOCK(unit, &spl_flags);
                    return ret;
                }

                MEMORY_BARRIER();
                *(pktHdr->ring_entry) |= NIC_RING_SWOWNBIT;    /* Only set the SwOwn bit */
                releaseCnt++;
            }
        }
        else
        {
            MEMORY_BARRIER();
            pktHdr->buf_len = 0;
            *(pktHdr->ring_entry) |= NIC_RING_SWOWNBIT;    /* Only set the SwOwn bit */
        }

        if (jumboFlag[ringId] && pktHdr->more == 0)
        {
            releaseCnt++;
            jumboFlag[ringId] = FALSE;
        }


        /* Jump to next */
        pNic_rxRDPIdx[ringId] += 1;
        if (pNic_rxRDPIdx[ringId] == (pNic_rxFDPBase[ringId] + rxRingIdSize[ringId]))
            pNic_rxRDPIdx[ringId] = pNic_rxFDPBase[ringId];

        if (jumboFlag == 0 || (jumboFlag && pktHdr->more == 0))
            nic_rx_success_cntr++;
    } while (pNic_rxRDPIdx[ringId] != pNic_rxCDPIdx[ringId]);


    NIC_RX_UNLOCK(unit, &spl_flags);

#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
    osal_wake_up(drv_nicIsr_event);
#endif

    return RT_ERR_OK;
}

int32
_nic_isr_txRoutine(uint32 unit, uint32 ringId)
{
#if !defined(__BOOTLOADER__)
    unsigned long   spl_flags;
#endif
    RT_LOG(LOG_DEBUG, MOD_NIC, "ringId = %d", ringId);

    if (ringId >= txRingNum)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "invalid ringId(%d)!", ringId);
        return RT_ERR_FAILED;
    }

    NIC_TX_LOCK(unit, &spl_flags);

    nic_tx_isr_cntr++;
    do
    {
        nic_pkthdr_t *pktHdr;

        if ((*pNic_txRDPIdx[ringId] & NIC_RING_SWOWNBIT) != 0)
            break;

        /* Prepare to be reclaim */
        pktHdr = (nic_pkthdr_t *)(*pNic_txRDPIdx[ringId] & NIC_ADDR_MASK);
        if (HWP_8380_30_FAMILY(unit))
            pktHdr = (nic_pkthdr_t*)((uint32)pktHdr | 0xa0000000);

        if (NULL == pktHdr || NULL == pktHdr->buf_addr || NULL == pktHdr->packet)
        {
            break;
        }
        /* Callback Tx CB Function (auto-free the abandoned packet) */
        if (pktHdr->tx_callback == NULL)
        {
            _nic_init_conf.pkt_free(unit, pktHdr->packet);
        }
        else
        {
            pktHdr->tx_callback(unit, pktHdr->packet, pktHdr->cookie);
            pktHdr->tx_callback = NULL;
        }
        pktHdr->packet = NULL;

        /* Jump to next */
        pNic_txRDPIdx[ringId] += 1;
        nic_tx_ring_cntr++;
        if (pNic_txRDPIdx[ringId] == (pNic_txFDPBase[ringId] + txRingIdSize[ringId]))
            pNic_txRDPIdx[ringId] = pNic_txFDPBase[ringId];
    } while (pNic_txRDPIdx[ringId] != pNic_txCDPIdx[ringId]);

    NIC_TX_UNLOCK(unit, &spl_flags);

    return RT_ERR_OK;
}
static int32
_nic_isr_mbRoutine(uint32 unit)
{
    uintptr temp;
    int32  ret = RT_ERR_FAILED;
    int32   qId;
    drv_nic_pkt_t *pPacket;
#if !defined(__BOOTLOADER__)
    unsigned long   spl_flags;
#endif


    NIC_RX_LOCK(unit, &spl_flags);

    for (qId = 0; qId < rxRingNum; qId++)
    {
        /* Update software current pointer */
        _nic_ringCurAddr_get(unit, NIC_DIR_RX, qId, &temp);
        pNic_rxCDPIdx[qId] = (uint32 *)temp;
        do
        {
            nic_pkthdr_t *pPktHdr;

            if ((*pNic_rxRDPIdx[qId] & NIC_RING_SWOWNBIT) != 0)
                break;

            /* Prepare to be reclaim */
            pPktHdr = (nic_pkthdr_t *)(*pNic_rxRDPIdx[qId] & NIC_ADDR_MASK);
            if (HWP_8380_30_FAMILY(unit))
                pPktHdr = (nic_pkthdr_t*)((uint32)pPktHdr | 0xa0000000);

            if (NULL == pPktHdr || pPktHdr->packet != NULL)
                break;

            /* Alloc a new packet data buffer */
            if (RT_ERR_OK != _nic_init_conf.pkt_alloc(unit, _nic_init_conf.pkt_size, 0, &pPacket))
            {
                RT_LOG(LOG_DEBUG, MOD_NIC, "Out of memory ! (alloc a new packet data buffer failed)");
                break;
            }

            pPktHdr->packet = pPacket;
            if (HWP_8380_30_FAMILY(unit))
                pPktHdr->buf_addr = (uint8 *)(UNCACHE(pPacket->data) & 0x0fffffff);
            else
                pPktHdr->buf_addr = (uint8 *)(UNCACHE(pPacket->data));
            pPktHdr->buf_size = _nic_init_conf.pkt_size;
            pPktHdr->buf_len = 0;
            if ((ret = osal_cache_memory_flush((uint32)pPacket->head, (pPacket->end - pPacket->head))) != RT_ERR_OK)
            {
                NIC_RX_UNLOCK(unit, &spl_flags);
                return ret;
            }
            MEMORY_BARRIER();
            *(pPktHdr->ring_entry) |= NIC_RING_SWOWNBIT;    /* Only set the SwOwn bit */


            /* Jump to next */
            pNic_rxRDPIdx[qId] += 1;
            if (pNic_rxRDPIdx[qId] == (pNic_rxFDPBase[qId] + rxRingIdSize[qId]))
                pNic_rxRDPIdx[qId] = pNic_rxFDPBase[qId];
        } while (pNic_rxRDPIdx[qId] != pNic_rxCDPIdx[qId]);
    }

    NIC_RX_UNLOCK(unit, &spl_flags);

    return RT_ERR_OK;
}

osal_isrret_t
drv_nic_isr_handler(void *isr_param)
{
    int32  i;
    uint32  cpu_iisr;
    uint32  unit = ((isr_param_t *)isr_param)->unit;
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
    uint32  intrMask;
#endif

    /* Handle Rx */
    _nic_intrSts_get(unit, NIC_RX_DONE, &cpu_iisr);
    if (cpu_iisr)
    {
        for (i = rxRingNum - 1; i >= 0; i--)
        {
            if (cpu_iisr & (0x1 << i))
            {
                _nic_intrSts_set(unit, NIC_RX_DONE, 0x1 << i);
                _nic_isr_rxRoutine(unit, i);
            }
        }
    }

    /* Handle Tx */
    _nic_intrSts_get(unit, NIC_TX_DONE, &cpu_iisr);
    if (cpu_iisr)
    {
        for (i = txRingNum - 1; i >= 0; i--)
        {
            if (cpu_iisr & (0x1 << i))
            {
                _nic_intrSts_set(unit, NIC_TX_DONE, 0x1 << i);
                _nic_isr_txRoutine(unit, i);
            }
        }
    }

    /* Tx all done */
    _nic_intrSts_get(unit, NIC_TX_ALLDONE, &cpu_iisr);
    _nic_intrSts_set(unit, NIC_TX_ALLDONE, cpu_iisr);

    /* mBuffer Runout */
    _nic_intrSts_get(unit, NIC_RX_RUNOUT, &cpu_iisr);
    if (cpu_iisr)
    {
        _nic_intrSts_set(unit, NIC_RX_RUNOUT, cpu_iisr);
        _nic_isr_mbRoutine(unit);
    }

#if defined(CONFIG_SDK_DRIVER_L2NTFY)
    /* L2 Notification handler */
    _nic_intrMask_get(unit, NIC_NTFY_DONE, &intrMask);
    _nic_intrSts_get(unit, NIC_NTFY_DONE, &cpu_iisr);
    if (cpu_iisr & intrMask)
    {
        _nic_intrSts_set(unit, NIC_NTFY_DONE, cpu_iisr);
        drv_l2ntfy_isr_handler(unit, isr_param);
    }
    _nic_intrMask_get(unit, NIC_NTFY_BUF_RUNOUT, &intrMask);
    _nic_intrSts_get(unit, NIC_NTFY_BUF_RUNOUT, &cpu_iisr);
    if (cpu_iisr & intrMask)
    {
        _nic_intrSts_set(unit, NIC_NTFY_BUF_RUNOUT, cpu_iisr);
        drv_l2ntfy_bufRunout_handler(unit, isr_param);
    }
    _nic_intrMask_get(unit, NIC_NTFY_LOCALBUF_RUNOUT, &intrMask);
    _nic_intrSts_get(unit, NIC_NTFY_LOCALBUF_RUNOUT, &cpu_iisr);
    if (cpu_iisr & intrMask)
    {
        _nic_intrSts_set(unit, NIC_NTFY_LOCALBUF_RUNOUT, cpu_iisr);
        drv_l2ntfy_localBufRunout_handler(unit, isr_param);
    }
#endif

    return OSAL_INT_HANDLED;
}

int32
_nic_pkt_tx(uint32 unit, drv_nic_pkt_t *pPacket, drv_nic_tx_cb_f fTxCb, void *pCookie)
{
    uint32  txRingId;
    uint32  packetNum __attribute__((unused));
    int32   ret = RT_ERR_FAILED;
    nic_pkthdr_t    *pPktHdr;

    if (NULL == pPacket)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "Error - pPacket is NULL!");
        return RT_ERR_FAILED;
    }

    /* Step: Decide the target queue */
    txRingId = (pPacket->tx_tag.priority > 3) ? 1 : 0;    /* mapping 8 priority to 2 queues */

    /* Step: Find a pktHdr */
    if ((*pNic_txCDPIdx[txRingId] & NIC_RING_SWOWNBIT) != 0)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "No Tx Descriptor [%08x] = 0x%08x can be used!",
            (uint32)pNic_txCDPIdx[txRingId], *pNic_txCDPIdx[txRingId]);
        return RT_ERR_FAILED;
    }

    if ((pPacket->data + pPacket->length) > pPacket->tail)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "pPacket->data + pPacket->length > pPacket->tail!");
        return RT_ERR_FAILED;
    }

    pPktHdr = (nic_pkthdr_t *)(*pNic_txCDPIdx[txRingId] & NIC_ADDR_MASK);
    if (HWP_8380_30_FAMILY(unit))
        pPktHdr = (nic_pkthdr_t*)((uint32)pPktHdr | 0xa0000000);


    /* Step: Double Confirm (Check the pktHdr status) */
    if (NULL == pPktHdr || NULL != pPktHdr->packet)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "pPktHdr is NOT available!");
        return RT_ERR_FAILED;
    }
    RT_LOG(LOG_DEBUG, MOD_NIC, "%s():%d  pPktHdr->packet:%p   as_txtag:%d\n", __FUNCTION__, __LINE__, pPktHdr->packet, pPacket->as_txtag);

    /* Insert the CPU Tx tag into the packet data buffer */
    if ((ret = _nic_cpuTagToRaw_cnvt(unit, pPacket, pPktHdr->cpuTag))!= RT_ERR_OK)
    {
        return ret;
    }

    /* Step: Calc the pktbuf number */
    packetNum = 1;    /* Support single pktBuf now - one descriptor vs. one mbuf */

    pPktHdr->tx_callback = fTxCb;    /* Tx Callback function */
    pPktHdr->cookie = pCookie;
    pPktHdr->packet = pPacket;
    if (HWP_8380_30_FAMILY(unit))
        pPktHdr->buf_addr = (uint8 *)(UNCACHE(pPacket->data) & 0x0fffffff);
    else
        pPktHdr->buf_addr = (uint8 *)UNCACHE(pPacket->data);
    pPktHdr->buf_size = pPacket->length;
    pPktHdr->buf_len = pPacket->length;

    if (0 == pPacket->txIncludeCRC)
    {
        pPktHdr->buf_size += 4;
        pPktHdr->buf_len += 4;
    }

    if ((ret = osal_cache_memory_flush((uint32)pPacket->head, (pPacket->end - pPacket->head))) != RT_ERR_OK)
    {
        return ret;
    }

    MEMORY_BARRIER();
    *(pPktHdr->ring_entry) |= NIC_RING_SWOWNBIT;

    /* To guarantee it's write done */
    do
    {
        uint32 chk __attribute__((unused));
        chk = *(pPktHdr->ring_entry);
    } while (0);

    /* Jump to next */
    pNic_txCDPIdx[txRingId] += 1;
    if (pNic_txCDPIdx[txRingId] == (pNic_txFDPBase[txRingId] + txRingIdSize[txRingId]))
        pNic_txCDPIdx[txRingId] = pNic_txFDPBase[txRingId];

    /* NIC Tx debug message */
    if (nic_debug_flag & DEBUG_TX_RAW_LEN_BIT)
    {
        int i;
        int dump_len = DEBUG_DUMP_PKT_LEN; /* debug dump maximum length */

        osal_printf("=== [NIC TX Debug] ================================= Len: %d \n", pPktHdr->buf_len);

        for (i = 0; i < dump_len; i++)
        {
            if (i == (pPktHdr->buf_len))
                break;
            if (0 == (i % 16))
                osal_printf("[%04X] ", i);
            if (HWP_8380_30_FAMILY(unit))
                osal_printf("%02X ", *(uint8*)((uint32)(pPktHdr->buf_addr + i) | 0xa0000000));
            else
                osal_printf("%02X ", *(uint8*)(pPktHdr->buf_addr + i));
            if (15 == (i % 16))
                osal_printf("\n");
        }
        osal_printf("\n");
    }

    if ((nic_debug_flag & DEBUG_TX_CPU_TAG_BIT) && pPacket->as_txtag)
    {
        _nic_rawTag_dump(unit, pPktHdr->cpuTag);
    }

    /* Set the TX Fetch Notify bit */
    if (txRingId)
        _nic_nicTxFetch_set(unit, NIC_TXRING_HIGH, TRUE);
    else
        _nic_nicTxFetch_set(unit, NIC_TXRING_LOW, TRUE);

    return RT_ERR_OK;
}

#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
static void _nic_rx_thread(void *pInput)
{
    uint32  i, curNic_rx_pkt_cnt, ring, jumbo;
    uint32  unit;
    nic_collectArrayList_t  *pEntry, *n;
    drv_nic_rx_t nic_rx_handle = NIC_RX_NOT_HANDLED;
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags;
#endif

    while(1)
    {
        osal_wait_event(drv_nicIsr_event);

        osal_list_for_each_entry_safe(pEntry, n, &nicListHead.list, list)
        {
            unit = pEntry->unit;
            for (i = 0; i < NIC_RX_CB_PRIORITY_NUMBER; i++)
            {
                if (_nic_rx_cb_tbl[i].rx_callback != NULL)
                {
                    nic_rx_handle = _nic_rx_cb_tbl[i].rx_callback(unit, pEntry->pPacket, _nic_rx_cb_tbl[i].pCookie);
                    if (NIC_RX_HANDLED_OWNED == nic_rx_handle)
                    {
                        break;
                    }
                }
            }
            if (nic_rx_handle != NIC_RX_HANDLED_OWNED)
            {   /* We have to free this packet here */
                _nic_init_conf.pkt_free(unit, pEntry->pPacket);
            }
            jumbo = pEntry->jumbo;

            NIC_RX_LOCK(unit, &spl_rx_flags);
            osal_list_del(&pEntry->list);
            NIC_RX_UNLOCK(unit, &spl_rx_flags);

            osal_free(pEntry);

            if (jumbo)
                curNic_rx_pkt_cnt = osal_atomic_sub_return(JUMBO_CLUSTER_NUM, &nic_rx_pkt_cnt);
            else
                curNic_rx_pkt_cnt = osal_atomic_sub_return(1, &nic_rx_pkt_cnt);
            if (curNic_rx_pkt_cnt == 0 && osal_atomic_read(&nic_mem_lock))
            {
                osal_atomic_set(&nic_mem_lock, 0);
                for (ring = 0; ring < rxRingNum; ring++)
                    _nic_isr_rxRoutine(unit, ring);
                _nic_intrMask_set(unit, NIC_RX_DONE, NIC_RX_RING_ALL);
                _nic_intrMask_set(unit, NIC_RX_RUNOUT, NIC_RX_RING_ALL);
                nicReleaseCnt++;
            }
        }
    }
}
#endif

static int32
_nic_init(uint32 unit, drv_nic_initCfg_t *pInitCfg)
{
    uint32  ntfy_support = 0;
    uint32  **ptr;
    uintptr ntfyBase;
    uint32  temp;
    uint32  val1, val2, val3;
    int32   i, j, k;
    int32   ret = RT_ERR_FAILED;

    /* Check arguments */
    RT_PARAM_CHK(NULL == pInitCfg->pkt_alloc, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pInitCfg->pkt_free, RT_ERR_NULL_POINTER);

    pRxPktHdr       = NULL;
    pTxPktHdr       = NULL;
    pNic_rxFDPBase  = NULL;
    pNic_txFDPBase  = NULL;

    /* Reset the NIC Tx/Rx debug information */
    nic_debug_flag = 0;
    nic_tx_success_cntr = 0;
    nic_tx_failed_cntr = 0;
    nic_rx_success_cntr = 0;
    nic_rx_failed_cntr = 0;
    totalRxRingNum = 0;
    totalTxRingNum = 0;

    _nic_init_conf.pkt_size  = pInitCfg->pkt_size;
    _nic_init_conf.pkt_alloc = pInitCfg->pkt_alloc;
    _nic_init_conf.pkt_free  = pInitCfg->pkt_free;

    /* Reset NIC only */
    _nic_cpuPortTxRxEnable_set(unit, DISABLED);
    _nic_cpuForceLinkupEnable_set(unit, FALSE);


    /* Save the setting used by L2 notification */
    ntfy_support = _nic_intrMask_get(unit, NIC_NTFY_DONE, &val1);
    if (ntfy_support != RT_ERR_CHIP_NOT_SUPPORTED)
    {
        _nic_intrMask_get(unit, NIC_NTFY_BUF_RUNOUT, &val2);
        _nic_intrMask_get(unit, NIC_NTFY_LOCALBUF_RUNOUT, &val3);
        _nic_ntfyBaseAddr_get(unit, &ntfyBase);
    }

    _nic_swQueRst_set(unit);
    do
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "Wait ... ");
        _nic_swQueRst_get(unit, &temp);
    } while (temp != 0);
    RT_LOG(LOG_DEBUG, MOD_NIC, "OK");

    /* Restore the setting used by L2 notification */
    if (ntfy_support != RT_ERR_CHIP_NOT_SUPPORTED)
    {
        _nic_intrMask_set(unit, NIC_NTFY_DONE, val1);
        _nic_intrMask_set(unit, NIC_NTFY_BUF_RUNOUT, val2);
        _nic_intrMask_set(unit, NIC_NTFY_LOCALBUF_RUNOUT, val3);
        _nic_ntfyBaseAddr_set(unit, ntfyBase);
    }

    /* Set CPU port to join the Lookup Miss Flooding Portmask */
    _nic_cpuL2FloodMask_add(unit);

    /* Reset to default value */
    _nic_intrMask_set(unit, NIC_RX_DONE, 0);
    _nic_intrMask_set(unit, NIC_RX_RUNOUT, 0);
    _nic_intrMask_set(unit, NIC_TX_DONE, 0);
    _nic_intrMask_set(unit, NIC_TX_ALLDONE, 0);
    _nic_intrSts_set(unit, NIC_RX_DONE, NIC_RX_RING_ALL);
    _nic_intrSts_set(unit, NIC_RX_RUNOUT, NIC_RX_RING_ALL);
    _nic_intrSts_set(unit, NIC_TX_DONE, 0x3);
    _nic_intrSts_set(unit, NIC_TX_ALLDONE, 0x3);
    _nic_rxTruncateLength_set(unit, 0x640);

    _nic_ringInfo_get(unit, NIC_DIR_RX, &rxRingIdSize, &rxRingNum);
    _nic_ringInfo_get(unit, NIC_DIR_TX, &txRingIdSize, &txRingNum);

    pPacket = osal_alloc(sizeof(drv_nic_pkt_t *) * rxRingNum);
    osal_memset(pPacket, 0, sizeof(drv_nic_pkt_t *) * rxRingNum);
    jumboFlag = osal_alloc(sizeof(uint32) * rxRingNum);
    osal_memset(jumboFlag, 0, sizeof(uint32) * rxRingNum);

    ptr = osal_alloc(sizeof(uint32) * rxRingNum * 3);       /* 3: pNic_rxFDPBase, pNic_rxCDPIdx, pNic_rxRDPIdx and */
    if (NULL == ptr)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "Error: Out of memory!");
        return RT_ERR_FAILED;
    }
    osal_memset(ptr, 0, sizeof(uint32) * rxRingNum * 3);
    pNic_rxFDPBase = ptr;
    ptr += rxRingNum;
    pNic_rxCDPIdx = ptr;
    ptr += rxRingNum;
    pNic_rxRDPIdx = ptr;

    ptr = osal_alloc(sizeof(uint32) * txRingNum * 3);
    if (NULL == ptr)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "Error: Out of memory!");
        goto err_alloc;
    }
    osal_memset(ptr, 0, sizeof(uint32) * txRingNum * 3);
    pNic_txFDPBase = ptr;
    ptr += txRingNum;
    pNic_txCDPIdx = ptr;
    ptr += txRingNum;
    pNic_txRDPIdx = ptr;


    for (i = 0; i < rxRingNum; i++)
    {
        pNic_rxFDPBase[i] = (uint32*) osal_alloc(sizeof(uint32) * rxRingIdSize[i] + CACHE_LINE_SIZE * 2);
        if (NULL == pNic_rxFDPBase[i])
        {
            ret = RT_ERR_MEM_ALLOC;
            RT_LOG(LOG_DEBUG, MOD_NIC, "Error: Out of memory!");
            goto err_alloc;
        }
        if (((uint32)pNic_rxFDPBase[i] & 0x3) != 0)
        {
            osal_printf("FATAL Error: pNic_rxFDPBase[%d](0x%08X) is NOT 4 Byte-Align!\n", i, (uint32)pNic_rxFDPBase[i]);
            goto err_alloc;
        }
        osal_memset(pNic_rxFDPBase[i], 0, sizeof(uint32) * rxRingIdSize[i] + CACHE_LINE_SIZE * 2);
        RT_ERR_HDL(osal_cache_memory_flush((uint32)pNic_rxFDPBase[i], sizeof(uint32) * rxRingIdSize[i] + CACHE_LINE_SIZE * 2), err_alloc, ret);
        pNic_rxFDPBase[i] = (uint32*)UNCACHE(pNic_rxFDPBase[i]) + 8;
        totalRxRingNum += rxRingIdSize[i];
    }

    for (i = 0; i < txRingNum; i++)
    {
        pNic_txFDPBase[i] = (uint32*) osal_alloc(sizeof(uint32) * txRingIdSize[i] + CACHE_LINE_SIZE * 2);
        if (NULL == pNic_txFDPBase[i])
        {
            ret = RT_ERR_MEM_ALLOC;
            RT_LOG(LOG_DEBUG, MOD_NIC, "Error: Out of memory!");
            goto err_alloc;
        }
        if (((uint32)pNic_txFDPBase[i] & 0x3) != 0)
        {
            osal_printf("FATAL Error: pNic_txFDPBase[%d](0x%08X) is NOT 4 Byte-Align!\n", i, (uint32)pNic_txFDPBase[i]);
            goto err_alloc;
        }
        osal_memset(pNic_txFDPBase[i], 0, sizeof(uint32) * txRingIdSize[i] + CACHE_LINE_SIZE * 2);
        RT_ERR_HDL(osal_cache_memory_flush((uint32)pNic_txFDPBase[i], sizeof(uint32) * txRingIdSize[i] + CACHE_LINE_SIZE * 2), err_alloc, ret);
        pNic_txFDPBase[i] = (uint32*)UNCACHE(pNic_txFDPBase[i]) + 8;
        totalTxRingNum += txRingIdSize[i];
    }

    pRxPktHdr = (nic_pkthdr_t*) osal_alloc(sizeof(nic_pkthdr_t) * totalRxRingNum + CACHE_LINE_SIZE * 2);
    pTxPktHdr = (nic_pkthdr_t*) osal_alloc(sizeof(nic_pkthdr_t) * totalTxRingNum + CACHE_LINE_SIZE * 2);
    if (NULL == pRxPktHdr || NULL == pTxPktHdr)
    {
        ret = RT_ERR_MEM_ALLOC;
        RT_LOG(LOG_DEBUG, MOD_NIC, "Error: Out of memory!");
        goto err_alloc;
    }
    osal_memset(pRxPktHdr, 0, sizeof(nic_pkthdr_t) * totalRxRingNum + CACHE_LINE_SIZE * 2);
    osal_memset(pTxPktHdr, 0, sizeof(nic_pkthdr_t) * totalTxRingNum + CACHE_LINE_SIZE * 2);
    RT_ERR_HDL(osal_cache_memory_flush((uint32)pRxPktHdr, sizeof(nic_pkthdr_t) * totalRxRingNum + CACHE_LINE_SIZE * 2), err_alloc, ret);
    RT_ERR_HDL(osal_cache_memory_flush((uint32)pTxPktHdr, sizeof(nic_pkthdr_t) * totalTxRingNum + CACHE_LINE_SIZE * 2), err_alloc, ret);
    pRxPktHdr = (nic_pkthdr_t *)UNCACHE(((uint32 *)pRxPktHdr + 8));
    pTxPktHdr = (nic_pkthdr_t *)UNCACHE(((uint32 *)pTxPktHdr + 8));


    for (i = 0; i < rxRingNum; i++)
    {
        pNic_rxCDPIdx[i] = pNic_rxFDPBase[i];
        pNic_rxRDPIdx[i] = pNic_rxFDPBase[i];
    }
    for (i = 0; i < txRingNum; i++)
    {
        pNic_txCDPIdx[i] = pNic_txFDPBase[i];
        pNic_txRDPIdx[i] = pNic_txFDPBase[i];
    }

    /* Rx pktHdrs */
    i = 0;
    for (j = 0; j < rxRingNum; j++)
    {
        for (k = 0; k < rxRingIdSize[j]; k++)
        {
            nic_pkthdr_t *pPktHdr;

            pPktHdr = &pRxPktHdr[i];
            pPktHdr->buf_size   = 0;
            pPktHdr->pkt_offset = 0;
            pPktHdr->more       = 0;
            pPktHdr->buf_len    = 0;
            pPktHdr->ring_entry = (pNic_rxFDPBase[j] + k);

            if (HWP_8380_30_FAMILY(unit))
                pPktHdr = (nic_pkthdr_t *)((uint32)pPktHdr & 0x0fffffff);

            *(pNic_rxFDPBase[j] + k) = ((k + 1) == rxRingIdSize[j])? \
                (uint32)pPktHdr | NIC_RING_WRAPBIT : \
                (uint32)pPktHdr;

            i++;
        }
    }

    /* Tx pktHdrs */
    i = 0;
    for (j = 0; j < txRingNum; j++)
    {
        for (k = 0; k < txRingIdSize[j]; k++)
        {
            nic_pkthdr_t *pPktHdr;

            pPktHdr = &pTxPktHdr[i];
            pPktHdr->buf_size   = 0;
            pPktHdr->pkt_offset = 0;
            pPktHdr->buf_len    = 0;
            pPktHdr->ring_entry  = (pNic_txFDPBase[j] + k);

            if (HWP_8380_30_FAMILY(unit))
                pPktHdr = (nic_pkthdr_t *)((uint32)pPktHdr & 0x0fffffff);

            *(pNic_txFDPBase[j] + k) = ((k + 1) == txRingIdSize[j])? \
                (uint32)pPktHdr | NIC_RING_WRAPBIT : \
                (uint32)pPktHdr;

            i++;
        }
    }


    /* Register NIC IRQ handler */
    my_isr_param[unit].unit = unit;
    if (gIsrRegistered == FALSE)
    {
        if (RT_ERR_OK != (ret = osal_isr_register(RTK_DEV_NIC, drv_nic_isr_handler, (void *)&my_isr_param[unit])))
        {
            RT_LOG(LOG_DEBUG, MOD_NIC, "Error - Register NIC IRQ handler failed!");
            goto err_alloc;
        }
        gIsrRegistered = TRUE;
    }

    /* Disable HOL */
    for (i = 0; i < rxRingNum; i++)
    {
        _nic_holRingSize_set(unit, i, 0);
    }

    for (i = 0; i < rxRingNum; i++)
    {
        if (HWP_8380_30_FAMILY(unit))
            _nic_ringBaseAddr_set(unit, NIC_DIR_RX, i, (uint32)pNic_rxFDPBase[i] & 0x0fffffff);
        else
            _nic_ringBaseAddr_set(unit, NIC_DIR_RX, i, (uint32)pNic_rxFDPBase[i]);
    }

    for (i = 0; i < txRingNum; i++)
    {
        if (HWP_8380_30_FAMILY(unit))
            _nic_ringBaseAddr_set(unit, NIC_DIR_TX, i, (uint32)pNic_txFDPBase[i] & 0x0fffffff);
        else
            _nic_ringBaseAddr_set(unit, NIC_DIR_TX, i, (uint32)pNic_txFDPBase[i]);
    }

    _nic_cpuTagId_get(unit, &cpuTagId);
#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
    OSAL_INIT_LIST_HEAD(&nicListHead.list);
#endif

    /* Prepare the mBufs once */
    RT_ERR_HDL(_nic_isr_mbRoutine(unit), err_alloc, ret);

    /*Every thing is ok now, NIC can RX/TX, and enable interrupt trigger*/
    _nic_intrMask_set(unit, NIC_RX_DONE, NIC_RX_RING_ALL);
    _nic_intrMask_set(unit, NIC_RX_RUNOUT, NIC_RX_RING_ALL);
    _nic_intrMask_set(unit, NIC_TX_DONE, 0x3);
    _nic_intrMask_set(unit, NIC_TX_ALLDONE, 0x3);
    _nic_nicEnable_set(unit, NIC_DIR_TX, TRUE);
    _nic_supportJumboSize_get(unit, &gJumboSize);

    /* CPU port: Enable MAC Tx/Rx */
    _nic_cpuPortTxRxEnable_set(unit, TRUE);
    /* CPU port: Force link-up */
    _nic_cpuForceLinkupEnable_set(unit, TRUE);

    return ret;


err_alloc:
    for (i = 0; i < rxRingNum; i++)
    {
        if (NULL != pNic_rxFDPBase && NULL != pNic_rxFDPBase[i])
            osal_free((uint32*)CACHED(pNic_rxFDPBase[i]) - 8);
    }
    for (i = 0; i < txRingNum; i++)
    {
        if (NULL != pNic_txFDPBase && NULL != pNic_txFDPBase[i])
            osal_free((uint32*)CACHED(pNic_txFDPBase[i]) - 8);
    }
    if (NULL != pNic_rxFDPBase)
        osal_free(pNic_rxFDPBase);
    if (NULL != pNic_txFDPBase)
        osal_free(pNic_txFDPBase);
    if (NULL != pRxPktHdr)
        osal_free((uint32*)CACHED(pRxPktHdr) - 8);
    if (NULL != pTxPktHdr)
        osal_free((uint32*)CACHED(pTxPktHdr) - 8);

    return ret;
}

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
int32
drv_nic_init(uint32 unit, drv_nic_initCfg_t *pInitCfg)
{
    int32 ret = RT_ERR_FAILED;

    RT_INIT_REENTRY_CHK(nic_init[unit]);
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check arguments */
    RT_PARAM_CHK(NULL == pInitCfg, RT_ERR_NULL_POINTER);

    /* Check whether it is inited, if inited, return fail */
    if (INIT_COMPLETED == nic_init[unit])
        return ret;

#ifndef __BOOTLOADER__
    spl_rx_lock = osal_spl_spin_lock_create();
    spl_tx_lock = osal_spl_spin_lock_create();
#endif

    /* Initialize the NIC module */
    if ((ret = _nic_init(unit, pInitCfg)) != RT_ERR_OK)
    {
        return ret;
    }

#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
    ret = osal_wait_module_create(&drv_nicIsr_event);
    if ((osal_thread_t)NULL == (osal_thread_create("RTK NIC Rx Thread", NIC_RX_THREAD_STACK_SIZE, NIC_RX_THREAD_PRI, (void *)_nic_rx_thread, NULL)))
    {
        osal_printf("RTK NIC Rx Thread create failed\n");

        return RT_ERR_FAILED;
    }
#endif

    /* set init flag to complete init */
    nic_init[unit] = INIT_COMPLETED;

    return ret;

}


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
int32
drv_nic_pkt_tx(uint32 unit, drv_nic_pkt_t *pPacket, drv_nic_tx_cb_f fTxCb, void *pCookie)
{
    int32 ret = RT_ERR_FAILED;
#if !defined(__BOOTLOADER__)
    unsigned long   spl_flags;
#endif
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check arguments */
    RT_PARAM_CHK(NULL == pPacket, RT_ERR_NULL_POINTER);

    /* Dispatch */
    NIC_TX_LOCK(unit, &spl_flags);

    /* Check init state */
    if (INIT_COMPLETED != nic_init[unit])
    {
        NIC_TX_UNLOCK(unit, &spl_flags);
        return RT_ERR_NOT_INIT;
    }

    ret = _nic_pkt_tx(unit, pPacket, fTxCb, pCookie);
    if (RT_ERR_OK == ret)
        nic_tx_success_cntr++;
    else
        nic_tx_failed_cntr++;

    NIC_TX_UNLOCK(unit, &spl_flags);

    return ret;
}


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
int32
drv_nic_rx_start(uint32 unit)
{
    int32 ret = RT_ERR_FAILED;
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags, spl_tx_flags;
#endif
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    NIC_RX_LOCK(unit, &spl_rx_flags);
    NIC_TX_LOCK(unit, &spl_tx_flags);
    ret = _nic_nicEnable_set(unit, NIC_DIR_RX, TRUE);
    NIC_TX_UNLOCK(unit, &spl_tx_flags);
    NIC_RX_UNLOCK(unit, &spl_rx_flags);

    return ret;
}


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
int32
drv_nic_rx_stop(uint32 unit)
{
    int32 ret = RT_ERR_FAILED;
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags, spl_tx_flags;
#endif
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    /* Check arguments */

    /* Dispatch */
    NIC_RX_LOCK(unit, &spl_rx_flags);
    NIC_TX_LOCK(unit, &spl_tx_flags);
    ret = _nic_nicEnable_set(unit, NIC_DIR_RX, FALSE);
    NIC_TX_UNLOCK(unit, &spl_tx_flags);
    NIC_RX_UNLOCK(unit, &spl_rx_flags);

    return ret;
}

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
int32
drv_nic_rx_status_get(uint32 unit, uint32 *pStatus)
{
    int32   ret = RT_ERR_FAILED;
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags, spl_tx_flags;
#endif
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    NIC_RX_LOCK(unit, &spl_rx_flags);
    NIC_TX_LOCK(unit, &spl_tx_flags);
    ret = _nic_nicEnable_get(unit, NIC_DIR_RX, pStatus);
    NIC_TX_UNLOCK(unit, &spl_tx_flags);
    NIC_RX_UNLOCK(unit, &spl_rx_flags);

    return ret;
}

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
int32
drv_nic_rx_register(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb, void *pCookie, uint32 flags)
{
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags, spl_tx_flags;
#endif

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check arguments */
    RT_PARAM_CHK(priority > NIC_RX_CB_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == fRxCb, RT_ERR_NULL_POINTER);

    NIC_RX_LOCK(unit, &spl_rx_flags);
    NIC_TX_LOCK(unit, &spl_tx_flags);
    if (NIC_FLAG_RX_CRC_INCLUDE & flags)
        rxCRCInclude = 1;

    if (NULL == _nic_rx_cb_tbl[priority].rx_callback)
    {
        _nic_rx_cb_tbl[priority].rx_callback = fRxCb;
        _nic_rx_cb_tbl[priority].pCookie     = pCookie;
    }
    else
    {
        /* Handler is already existing */
        NIC_TX_UNLOCK(unit, &spl_tx_flags);
        NIC_RX_UNLOCK(unit, &spl_rx_flags);
        return RT_ERR_FAILED;
    }
    NIC_TX_UNLOCK(unit, &spl_tx_flags);
    NIC_RX_UNLOCK(unit, &spl_rx_flags);

    return RT_ERR_OK;
}

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
int32
drv_nic_rx_unregister(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb)
{
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags, spl_tx_flags;
#endif

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check arguments */
    RT_PARAM_CHK(priority > NIC_RX_CB_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == fRxCb, RT_ERR_NULL_POINTER);

    NIC_RX_LOCK(unit, &spl_rx_flags);
    NIC_TX_LOCK(unit, &spl_tx_flags);

    if (_nic_rx_cb_tbl[priority].rx_callback == fRxCb)
    {
        _nic_rx_cb_tbl[priority].rx_callback = NULL;
        _nic_rx_cb_tbl[priority].pCookie     = NULL;
    }
    else
    {
        /* Handler is nonexistent */
        NIC_TX_UNLOCK(unit, &spl_tx_flags);
        NIC_RX_UNLOCK(unit, &spl_rx_flags);
        return RT_ERR_FAILED;
    }

    NIC_TX_UNLOCK(unit, &spl_tx_flags);
    NIC_RX_UNLOCK(unit, &spl_rx_flags);

    return RT_ERR_OK;
}


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
int32
drv_nic_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket)
{
    int32 ret = RT_ERR_FAILED;
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK(NULL == _nic_init_conf.pkt_alloc, RT_ERR_NULL_POINTER);

    /* Dispatch */
    ret = _nic_init_conf.pkt_alloc(unit, size, flags, ppPacket);

    return ret;
}


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
int32
drv_nic_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket)
{
    int32 ret = RT_ERR_FAILED;
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);


    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    /* Check arguments */
    RT_PARAM_CHK(NULL == pPacket, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == _nic_init_conf.pkt_free, RT_ERR_NULL_POINTER);

    /* Dispatch */
    ret = _nic_init_conf.pkt_free(unit, pPacket);

    return ret;
}



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
int32
drv_nic_reset(uint32 unit)
{
    int32   ret;
    uint32  i;
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags, spl_tx_flags;
#endif

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    RT_INIT_CHK(nic_init[unit]);


    NIC_RX_LOCK(unit, &spl_rx_flags);
    NIC_TX_LOCK(unit, &spl_tx_flags);

    nic_init[unit] = INIT_NOT_COMPLETED;

    /* Disable NIC rx/tx*/
    _nic_cpuPortTxRxEnable_set(unit, FALSE);

    NIC_TX_UNLOCK(unit, &spl_tx_flags);
    NIC_RX_UNLOCK(unit, &spl_rx_flags);

    _nic_nicEnable_set(unit, NIC_DIR_TX, FALSE);
    _nic_nicEnable_set(unit, NIC_DIR_RX, FALSE);
    _nic_intrMask_set(unit, NIC_RX_DONE, 0);
    _nic_intrMask_set(unit, NIC_RX_RUNOUT, 0);
    _nic_intrMask_set(unit, NIC_TX_DONE, 0);
    _nic_intrMask_set(unit, NIC_TX_ALLDONE, 0);


    /* Free packet Buf */
    for (i = 0; i < totalRxRingNum; i++)
    {
        if (pRxPktHdr[i].packet != NULL)
        {
            _nic_init_conf.pkt_free(unit, (void*)CACHED(pRxPktHdr[i].packet));
            pRxPktHdr[i].packet = 0;
        }
    }

    /* Free pktHdr */
    pRxPktHdr = (nic_pkthdr_t*)((uint32 *)pRxPktHdr - 8);
    pTxPktHdr = (nic_pkthdr_t*)((uint32 *)pTxPktHdr - 8);
    if (pRxPktHdr != NULL)
    {
        osal_free((void*)CACHED(pRxPktHdr));
        pRxPktHdr = 0;
    }
    if (pTxPktHdr != NULL)
    {
        osal_free((void*)CACHED(pTxPktHdr));
        pTxPktHdr = 0;
    }

    /* Free Ring */
    for (i = 0; i < rxRingNum; i++)
    {
        pNic_rxFDPBase[i] = (uint32*)((uint32 *)pNic_rxFDPBase[i] - 8);
        if (pNic_rxFDPBase[i] != NULL)
        {
            osal_free((void*)CACHED(pNic_rxFDPBase[i]));
            pNic_rxFDPBase[i] = 0;
        }
    }
    if (pNic_rxFDPBase != NULL)
    {
        osal_free((void*)CACHED(pNic_rxFDPBase));
        pNic_rxFDPBase = 0;
    }
    if (pNic_rxFDPBase != NULL)
    {
        osal_free((void*)CACHED(pNic_txFDPBase));
        pNic_txFDPBase = 0;
    }

    ret = _nic_init(unit, &_nic_init_conf);
    if (ret)
    {
        RT_LOG(LOG_DEBUG, MOD_NIC, "NIC reset failed");
        return RT_ERR_FAILED;
    }

    _nic_nicEnable_set(unit, NIC_DIR_RX, TRUE);

    nic_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}

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
 * Note:
 *      There are 4 BIT flags as following:
 *      - DEBUG_RX_RAW_LEN_BIT
 *      - DEBUG_RX_CPU_TAG_BIT
 *      - DEBUG_TX_RAW_LEN_BIT
 *      - DEBUG_TX_CPU_TAG_BIT
 */
int32
drv_nic_dbg_get(uint32 unit, uint32 *pFlags)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    *pFlags = nic_debug_flag;

    return RT_ERR_OK;
}

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
 * Note:
 *      There are 4 BIT flags can be selected as following:
 *      - DEBUG_RX_RAW_LEN_BIT
 *      - DEBUG_RX_CPU_TAG_BIT
 *      - DEBUG_TX_RAW_LEN_BIT
 *      - DEBUG_TX_CPU_TAG_BIT
 */
int32
drv_nic_dbg_set(uint32 unit, uint32 flags)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    nic_debug_flag = flags;

    return RT_ERR_OK;
}

int32
drv_nic_cntr_dump(uint32 unit)
{
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags, spl_tx_flags;
#endif
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    NIC_RX_LOCK(unit, &spl_rx_flags);
    NIC_TX_LOCK(unit, &spl_tx_flags);
    osal_printf("Tx success counter : %0u \n", nic_tx_success_cntr);
    osal_printf("Tx failed counter  : %0u \n", nic_tx_failed_cntr);
    osal_printf("Rx success counter : %0u \n", nic_rx_success_cntr);
    osal_printf("Rx failed counter  : %0u \n", nic_rx_failed_cntr);
#ifdef CONFIG_SDK_NIC_RX_CB_IN_THREAD
    osal_printf("nic_rx_pkt_cnt     : %0d \n", osal_atomic_read(&nic_rx_pkt_cnt));
    osal_printf("nic_mem_lock       : %0d \n", osal_atomic_read(&nic_mem_lock));
    osal_printf("nicLockCnt         : %0d \n", nicLockCnt);
    osal_printf("nicReleaseCnt      : %0d \n", nicReleaseCnt);
#endif
    NIC_TX_UNLOCK(unit, &spl_tx_flags);
    NIC_RX_UNLOCK(unit, &spl_rx_flags);

    return RT_ERR_OK;
}

int32
drv_nic_cntr_clear(uint32 unit)
{
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags, spl_tx_flags;
#endif
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    NIC_RX_LOCK(unit, &spl_rx_flags);
    NIC_TX_LOCK(unit, &spl_tx_flags);
    nic_tx_success_cntr = 0;
    nic_tx_failed_cntr = 0;
    nic_rx_success_cntr = 0;
    nic_rx_failed_cntr = 0;
    nic_tx_isr_cntr = 0;
    nic_tx_ring_cntr = 0;
    NIC_TX_UNLOCK(unit, &spl_tx_flags);
    NIC_RX_UNLOCK(unit, &spl_rx_flags);

    return RT_ERR_OK;
}

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
int32
drv_nic_ringbuf_dump(uint32 unit, nic_dir_t direct)
{
    uintptr addr;
    uint32  i, j, value;
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags, spl_tx_flags;
#endif
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    NIC_RX_LOCK(unit, &spl_rx_flags);
    NIC_TX_LOCK(unit, &spl_tx_flags);

    if (direct == NIC_DIR_RX)
    {
        osal_printf("RXRING  SW_rxFDPBase  SW_RxCDPIdx  HW_RxCDPIdx  SW_RxRDPIdx \n");
        osal_printf("=========================================================== \n");
        for (i = 0; i < rxRingNum; i++)
        {
            _nic_ringCurAddr_get(unit, NIC_DIR_RX, i, &addr);
            osal_printf(" %u(p)   0x%08x    0x%08x   0x%08llx   0x%08x \n",
                        i, ((uint32)pNic_rxFDPBase[i]), ((uint32)pNic_rxCDPIdx[i]), (uint64)addr, ((uint32)pNic_rxRDPIdx[i]));
        }
        osal_printf("\n");

        osal_printf("================================================================================\n");
        for (i = 0; i < rxRingNum; i++)
        {
            osal_printf("RX RING: %u\n", i);
            for (j = 0; j < rxRingIdSize[i]; j++)
                osal_printf("  rxFDPBase %u :(p)%#08x   (v)%#08x\n", i, (uint32)(pNic_rxFDPBase[i] + j), *(pNic_rxFDPBase[i] + j));
            osal_printf("--------------------------------------------------------------------------------\n");
        }
        osal_printf("\n");
    }


    if (direct == NIC_DIR_TX)
    {
        osal_printf("TXRING  SW_txFDPBase  SW_TxCDPIdx  HW_TxCDPIdx  SW_TxRDPIdx \n");
        osal_printf("=========================================================== \n");
        for (i = 0; i < txRingNum; i++)
        {
            _nic_ringCurAddr_get(unit, NIC_DIR_TX, i, &addr);

            osal_printf(" %u(p)   0x%08x    0x%08x   0x%08llx   0x%08x \n",
                        i, ((uint32)pNic_txFDPBase[i]), ((uint32)pNic_txCDPIdx[i]), (uint64)addr, ((uint32)pNic_txRDPIdx[i]));
        }
        osal_printf("\n");

        osal_printf("TXRING txFDPBase  Base+1   Base+2   Base+3\n");
        osal_printf("============================================\n");
        for (i = 0; i < txRingNum; i++)
        {
            osal_printf("TX RING: %u\n", i);
            for (j = 0; j < txRingIdSize[i]; j++)
                osal_printf("  txFDPBase %u :(p)%#08x   (v)%#08x\n", i, (uint32)(pNic_txFDPBase[i] + j), *(pNic_txFDPBase[i] + j));
            osal_printf("--------------------------------------------------------------------------------\n");
        }
        osal_printf("\n");
    }

    _nic_intrMask_get(unit, NIC_RX_RUNOUT, &value);
    osal_printf("[RX_RUNOUT=0x%x, ", value);
    _nic_intrMask_get(unit, NIC_TX_DONE, &value);
    osal_printf("   EN_TX_DONE1_0=0x%x, ", value);
    _nic_intrMask_get(unit, NIC_RX_DONE, &value);
    osal_printf("EN_RX_DONE7_0=0x%x, ", value);
    _nic_intrMask_get(unit, NIC_TX_ALLDONE, &value);
    osal_printf("EN_TX_ALLDONE1_0=0x%x]\n", value);

    _nic_intrSts_get(unit, NIC_RX_RUNOUT, &value);
    osal_printf("[INT_PHDS7_0=0x%x, ", value);
    _nic_intrSts_get(unit, NIC_TX_DONE, &value);
    osal_printf("   INT_TX_DONE1_0=0x%x, ", value);
    _nic_intrSts_get(unit, NIC_RX_DONE, &value);
    osal_printf("INT_RX_DONE7_0=0x%x, ", value);
    _nic_intrSts_get(unit, NIC_TX_ALLDONE, &value);
    osal_printf("INT_TX_ALL_DONE1_0=0x%x]\n", value);

    _nic_nicEnable_get(unit, NIC_DIR_TX, &value);
    osal_printf("[TX_CMD=0x%x, ", value);
    _nic_nicEnable_get(unit, NIC_DIR_RX, &value);
    osal_printf("RX_CMD=0x%x,\n", value);
    osal_printf("CHIP_FAMILY_ID=0x%x]\n", HWP_CHIP_FAMILY_ID(unit));

    if (direct == NIC_DIR_TX)
    {
        if (HWP_8380_30_FAMILY(unit) || HWP_8390_50_FAMILY(unit))
        {
            _nic_nicTxBusySts_get(unit, NIC_TXRING_BOTH, &value);
            osal_printf("TX_BUSY=0x%x]\n", value);
        }
        else
        {
            _nic_nicTxBusySts_get(unit, NIC_TXRING_HIGH, &value);
            osal_printf("TX_H_BUSY=0x%x]\n", value);
            _nic_nicTxBusySts_get(unit, NIC_TXRING_LOW, &value);
            osal_printf("TX_L_BUSY=0x%x]\n", value);
        }
    }

    NIC_TX_UNLOCK(unit, &spl_tx_flags);
    NIC_RX_UNLOCK(unit, &spl_rx_flags);

    return RT_ERR_OK;
}

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
drv_nic_pktHdrMBuf_dump(uint32 unit, uint32 mode, uint32 start, uint32 end, uint32 flags)
{
    uint32  i, j, *ring_size;
#if !defined(__BOOTLOADER__)
    unsigned long   spl_rx_flags, spl_tx_flags;
#endif
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);


    /* Check init state */
    RT_INIT_CHK(nic_init[unit]);

    NIC_RX_LOCK(unit, &spl_rx_flags);
    NIC_TX_LOCK(unit, &spl_tx_flags);
    osal_printf("------- Formal Information -------------------------\n");
    if (NIC_PKTHDR_MBUF_MODE_RX == mode)
    {
        osal_printf("==== Dump Rx packet header and mbuf ====\n");
        ring_size = rxRingIdSize;
    }
    else
    {
        osal_printf("==== Dump Tx packet header and mbuf ====\n");
        ring_size = txRingIdSize;
    }

    for (i = start; i <= end; i++)
    {
        for (j = 0; j < ring_size[i]; j++)
        {
            nic_pkthdr_t    *pRing_pkthdr;
            if (NIC_PKTHDR_MBUF_MODE_RX == mode)
                pRing_pkthdr = (nic_pkthdr_t *)(*((uint32 *)(pNic_rxFDPBase[i]+j)) & NIC_ADDR_MASK);
            else
                pRing_pkthdr = (nic_pkthdr_t *)(*((uint32 *)(pNic_txFDPBase[i]+j)) & NIC_ADDR_MASK);
            if (HWP_8380_30_FAMILY(unit))
                pRing_pkthdr = (nic_pkthdr_t *)((uint32)pRing_pkthdr | 0xa0000000);
            osal_printf("###################################################\n");
            osal_printf("ring[%u]_pkthdr[%u]->buf_addr = 0x%08x\n", i, j, (uint32)pRing_pkthdr->buf_addr);
            osal_printf("ring[%u]_pkthdr[%u]->buf_size = 0x%04x\n", i, j, (uint16)pRing_pkthdr->buf_size);
            osal_printf("ring[%u]_pkthdr[%u]->more = 0x%08x\n", i, j, (uint32)pRing_pkthdr->more);
            osal_printf("ring[%u]_pkthdr[%u]->pkt_offset = 0x%08x\n", i, j, (uint32)pRing_pkthdr->pkt_offset);
            osal_printf("ring[%u]_pkthdr[%u]->buf_len = 0x%04x\n", i, j, (uint16)pRing_pkthdr->buf_len);
            osal_printf("ring[%u]_pkthdr[%u]->tx_callback = 0x%08x\n", i, j, (uint32)pRing_pkthdr->tx_callback);
            osal_printf("ring[%u]_pkthdr[%u]->cookie = 0x%08x\n", i, j, (uint32)pRing_pkthdr->cookie);
            if ((pRing_pkthdr->packet != NULL) && (TRUE == flags))
            {
                uint32  k;
                uint32  dump_len = DEBUG_DUMP_PKT_LEN; /* debug dump maximum length */
                uint32  pkt_len = pRing_pkthdr->buf_len;
                uint8   *pPkt_data = pRing_pkthdr->packet->data;
                osal_printf("------------------- its raw data ----------------------\n");

                for (k = 0; k < dump_len; k++)
                {
                    if (k == pkt_len)
                        break;
                    if (0 == (k % 16))
                        osal_printf("[%04X] ", k);
                    osal_printf("%02X ", *(pPkt_data + k));
                    if (15 == (k % 16))
                        osal_printf("\n");
                }
                osal_printf("\n");
            }
        }
        osal_printf("###################################################\n");
    }

    NIC_TX_UNLOCK(unit, &spl_tx_flags);
    NIC_RX_UNLOCK(unit, &spl_rx_flags);
    return RT_ERR_OK;
}

