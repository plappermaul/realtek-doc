#include <common/rt_type.h>
#include <linux/pci.h>
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#if !defined(__KERNEL__)
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <drv/nic/nic.h>
#include <ioal/mem32.h>
#include <ioal/ioal_init.h>
#include <osal/cache.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/print.h>
#include <osal/thread.h>
#include <osal/time.h>
#include <osal/sem.h>
#include <osal/spl.h>
#include <hwp/hw_profile.h>
#include <private/drv/nic/nic_diag.h>
#include <private/drv/nic/nic_rx.h>
#include <private/drv/nic_pci/nic_pci_9310.h>
#include <private/drv/swcore/swcore_rtl9310.h>


/*
 * Symbol Definition
 */
#define DEBUG_DUMP_PKT_LEN      256
#define CRCPOLY                 0xEDB88320
#define CRC_TBL_SIZE            256

typedef struct nic_rx_cb_entry_s
{
    drv_nic_rx_cb_f rx_callback;
    void *pCookie;
} nic_rx_cb_entry_t;

typedef struct stats_s
{
    uint64  nic_rx_success_cntr;
    uint32  rx_length_errors;
    uint32  rx_crc_errors;
    uint64  rx_errors;
    uint32  rx_dropped;
    uint64  rx_lack_buf_cntr;
    uint64  nic_tx_success_cntr;
    uint32  tx_dropped;
    uint32  tx_bytes;
    uint32  tx_packets;
    uint32  tx_fail;
} stats_t;

struct rtl8168_counters {
        u64 tx_packets;
        u64 rx_packets;
        u64 tx_errors;
        u32 rx_errors;
        u16 rx_missed;
        u16 align_errors;
        u32 tx_one_collision;
        u32 tx_multi_collision;
        u64 rx_unicast;
        u64 rx_broadcast;
        u32 rx_multicast;
        u16 tx_aborted;
        u16 tx_underun;
};


/*
 * Data Declaration
 */
#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
static osal_mutex_t     nic_sem;
static osal_mutex_t     nic_reset_sem;
#elif defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
osal_spinlock_t         spl_lock;
#endif

struct rtl8168_private      *tp;
int                     msi_feature;
uint32                  rxCRCInclude;
static uint32           _nic_rx_intr_cb_cnt = 0;
stats_t                 stats;
drv_nic_initCfg_t       _nic_init_conf;
static nic_rx_cb_entry_t _nic_rx_cb_tbl[NIC_RX_CB_PRIORITY_NUMBER];
uint32                  nic_init;
static uint32           nic_debug_flag;
static uint32           crcTable[CRC_TBL_SIZE];

uint64                  nml_rx_alloc_cnt;
uint64                  nml_tx_alloc_cnt;
uint64                  nml_free_cnt;
uint64                  jumbo_rx_alloc_cnt;
uint64                  jumbo_tx_alloc_cnt;
uint64                  jumbo_free_cnt;
static uint64           rtl8168_free_cnt;
static uint64           nic_end_free_cnt;
static uint64           intr_runout_alloc_fail_cnt;
uint32                  free_fail_cnt;


extern void             *ioaddr;
uintptr                 tally_vaddr;
uintptr                 tally_phy_addr;
uintptr                 tx_desc_vaddr;
uintptr                 tx_desc_phy_addr;
uintptr                 rx_desc_vaddr;
uintptr                 rx_desc_phy_addr;
uintptr                 pkt_buf_vir_addres;
uintptr                 pkt_phy_addr;

static uint32             reasonTbl[][2] =
{
    {0,                                         },
    {NIC_RX_REASON_CPU2CPU,                     0, },
    {NIC_RX_REASON_OF_TTL_EXCEED,               0, },
    {NIC_RX_REASON_OF_TBL_LUMIS,                0, },
    {NIC_RX_REASON_DUMMY,                       0, },
    {NIC_RX_REASON_DUMMY,                       0, },
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
    {NIC_RX_REASON_RMA_USR_DEF0,                NIC_RX_REASON_RMA, },   /* 20 */
    {NIC_RX_REASON_RMA_USR_DEF1,                NIC_RX_REASON_RMA, },
    {NIC_RX_REASON_RMA_USR_DEF2,                NIC_RX_REASON_RMA, },
    {NIC_RX_REASON_RMA_USR_DEF3,                NIC_RX_REASON_RMA, },
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
    {NIC_RX_REASON_IP4_6_ICMP_REDIR,            0, },
    {NIC_RX_REASON_IPUC_MTU,                    0, },
    {NIC_RX_REASON_IPMC_MTU,                    0, },
    {NIC_RX_REASON_IPUC_TTL,                    0, },
    {NIC_RX_REASON_IPMC_TTL,                    0, },
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


/*
 * Macro Definition
 */
#ifndef DMA_BIT_MASK
#define DMA_BIT_MASK(value)             ((1ULL << value) - 1)
#endif

#if !defined(__KERNEL__)
#define cpu_to_le16                     CPU_to_LE16
#define cpu_to_le32                     CPU_to_LE32
#define cpu_to_le64                     CPU_to_LE64
#define le16_to_cpu                     LE16_to_CPU
#define le32_to_cpu                     LE32_to_CPU
#define le64_to_cpu                     LE64_to_CPU
#endif /* !defined(__KERNEL__) */

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif
#define MEMORY_BARRIER()        ({ __asm__ __volatile__ ("": : :"memory"); })

#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
#define NIC_LOCK(unit, flags)    \
do {\
    if (osal_sem_mutex_take(nic_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        osal_printf("nic semaphore lock failed");\
    }\
} while(0)
#define NIC_UNLOCK(unit, flags)   \
do {\
    if (osal_sem_mutex_give(nic_sem) != RT_ERR_OK)\
    {\
        osal_printf("nic semaphore unlock failed");\
    }\
} while(0)

#define NIC_RESET_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(nic_reset_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        osal_printf("nicreset semaphore lock failed");\
    }\
} while(0)
#define NIC_RESET_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(nic_reset_sem) != RT_ERR_OK)\
    {\
        osal_printf("nicreset semaphore unlock failed");\
    }\
} while(0)
#else
#define NIC_LOCK(unit, flags)       osal_spl_spin_lock_irqsave(spl_lock, flags)
#define NIC_UNLOCK(unit, flags)     osal_spl_spin_unlock_irqrestore(spl_lock, flags)
#define NIC_RESET_LOCK(unit)
#define NIC_RESET_UNLOCK(unit)
#endif

#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
#define DMA_ADDR_USR2PHY(usr_addr)    ((usr_addr - pkt_buf_vir_addres) + pkt_phy_addr)
#endif






void _makeCrcTable(void)
{
    uint32 c;
    uint16 n, k;

    for (n = 0; n < CRC_TBL_SIZE; n++)
    {
        c = n;
        for (k = 8; k > 0; k--)
        {
            if ((c & 1) != 0)
                c = CRCPOLY ^ (c >> 1);
            else
                c = c >> 1;
        }
        crcTable[n] = c;
    }
}

static int32
_extractCPUTag(uint8 *pPkt, nic_9310_cpuTag_t *pCpuTag)
{
    uint8   *byte;

    if ((NULL == pPkt) || (NULL == pCpuTag))
        return RT_ERR_FAILED;

    /* cpuTag copy: offset 14 bytes = DA(6) + SA(6)+ CPU_TAG_ID(2) */
    osal_memcpy((uint8 *)pCpuTag, (uint8 *)(pPkt + 14), sizeof(nic_9310_cpuTag_t));

    byte = pPkt + 16;
    pCpuTag->un.rx.CPUTAGIF         = 1;
    pCpuTag->un.rx.SPA              = byte[0] & 0x3f;
    pCpuTag->un.rx.OF_LUMIS_TBL_ID  = (byte[1] >> 4) & 0x3;
    pCpuTag->un.rx.ACL_OF_HIT       = (byte[1] >> 2) & 0x3;
    pCpuTag->un.rx.OF_TBL_ID        = byte[1] & 0x3;
    pCpuTag->un.rx.SPN_IS_TRK       = byte[2] >> 7;
    pCpuTag->un.rx.TRK_ID           = byte[2] & 0x7f;
    pCpuTag->un.rx.L2_ERR_PKT       = (byte[3] >> 6) & 0x1;
    pCpuTag->un.rx.L3_ERR_PKT       = (byte[3] >> 5) & 0x1;
    pCpuTag->un.rx.ATK_TYPE         = byte[3] & 0x1f;
    pCpuTag->un.rx.QID              = (byte[4] >> 3) & 0x1f;
    pCpuTag->un.rx.SPN              = ((uint16)(byte[4] & 0x3) << 8) | byte[5];
    pCpuTag->un.rx.IDX              = ((uint16)(byte[6] & 0x7f) << 8) | byte[7];
    pCpuTag->un.rx.MIR_HIT          = (byte[8] >> 4) & 0xf;
    pCpuTag->un.rx.SFLOW            = (byte[8] >> 2) & 0x3;
    pCpuTag->un.rx.TT_HIT           = (byte[8] >> 1) & 0x1;
    pCpuTag->un.rx.TT_IDX           = ((uint16)(byte[8] & 0x1) << 8) | byte[9];
    pCpuTag->un.rx.OTAGIF           = (byte[10] >> 6) & 0x1;
    pCpuTag->un.rx.ITAGIF           = (byte[10] >> 5) & 0x1;
    pCpuTag->un.rx.FVID_SEL         = (byte[10] >> 4) & 0x1;
    pCpuTag->un.rx.FVID             = ((uint16)(byte[10] & 0xf) << 8) | byte[11];
    pCpuTag->un.rx.MAC_CST          = (byte[12] >> 6) & 0x1;
    pCpuTag->un.rx.DM_RXIDX         = (byte[12]) & 0x3f;
    pCpuTag->un.rx.NEW_SA           = (byte[13] >> 7) & 0x1;
    pCpuTag->un.rx.PMV_FBD          = (byte[13] >> 6) & 0x1;
    pCpuTag->un.rx.L2_STTC_PMV      = (byte[13] >> 5) & 0x1;
    pCpuTag->un.rx.L2_DYN_PMV       = (byte[13] >> 4) & 0x1;
    pCpuTag->un.rx.OVERSIZE         = (byte[13] >> 3) & 0x1;
    pCpuTag->un.rx.PORT_DATA_IS_TRK = (byte[13] >> 2) & 0x1;
    pCpuTag->un.rx.PORT_DATA        = ((uint16)(byte[13] & 0x3) << 8) | byte[14];
    pCpuTag->un.rx.HASH_FULL        = (byte[15] >> 7) & 0x1;
    pCpuTag->un.rx.INVLD_SA         = (byte[15] >> 6) & 0x1;
    pCpuTag->un.rx.REASON           = (byte[15]) & 0x3f;

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


    return RT_ERR_OK;
}

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

void _cpu_txTag_prcess(drv_nic_pkt_t *pPacket)
{
    uint8   *base;
    if (unlikely(((uintptr)pPacket->data - NIC_9310_CPU_TAG_SIZE) < ((uintptr)pPacket->head)))
    {
        osal_printf("%s():%d payload reserved space is not enough  data:%p  %p  %p\n", __FUNCTION__, __LINE__, pPacket->data, (pPacket->data - NIC_9310_CPU_TAG_SIZE), (pPacket->head));
        return;
    }

    osal_memcpy(pPacket->data - NIC_9310_CPU_TAG_SIZE, pPacket->data, NIC_9310_CPU_TAG_SIZE);
    pPacket->data -= NIC_9310_CPU_TAG_SIZE;
    pPacket->length += 20;

    base = pPacket->data + 12;  //DA & SA offset
    *(uint32*)base = CPU_to_BE32(0x88990400);
    base += 4;
    *(uint16*)base = 0x0;
    base += 2;

    switch (pPacket->tx_tag.fwd_type)
    {
        case NIC_FWD_TYPE_ALE:
            base[0] = 0;break;
        case NIC_FWD_TYPE_PHYISCAL:
            base[0] = 1;break;
        case NIC_FWD_TYPE_LOGICAL:
            base[0] = 2;break;
        case NIC_FWD_TYPE_TRUNK:
            base[0] = 3;break;
        case NIC_FWD_TYPE_PHYISCAL_ONE_HOP:
            base[0] = 4;break;
        case NIC_FWD_TYPE_LOGICAL_ONE_HOP:
            base[0] = 5;break;
        case NIC_FWD_TYPE_UCST_CPU_MIN_PORT:
            base[0] = 6;break;
        case NIC_FWD_TYPE_UCST_CPU:
            base[0] = 7;break;
        case NIC_FWD_TYPE_BCST_CPU:
            base[0] = 8;break;
        default:
            osal_printf("FATAL Error: fwd_type is not support!\n");
            return;
    }

    base++;
    *base = (pPacket->tx_tag.acl_act << 7) | (pPacket->tx_tag.cngst_drop << 6) | (pPacket->tx_tag.dm_pkt << 5) | (pPacket->tx_tag.dg_pkt << 4) |
            (pPacket->tx_tag.bp_fltr << 3) | (pPacket->tx_tag.bp_stp << 2) | (pPacket->tx_tag.bp_vlan_egr << 1) | (pPacket->tx_tag.as_tagSts << 0);

    base++;
    *base = (pPacket->tx_tag.l3_act << 7) | (pPacket->tx_tag.ori_tagif_en << 6) | (pPacket->tx_tag.as_priority << 5) | (pPacket->tx_tag.priority);

    base++;
    *base = (pPacket->tx_tag.ori_itagif << 7) | (pPacket->tx_tag.ori_otagif << 6) | (pPacket->tx_tag.fvid_sel << 5) | (pPacket->tx_tag.fvid_en << 4) |
            ((pPacket->tx_tag.fvid >> 8) & 0xf);

    base++;
    *base = pPacket->tx_tag.fvid & 0xff;

    base++;
    *base = (pPacket->tx_tag.src_filter_en << 7) | (pPacket->tx_tag.sp_is_trk << 6) |
            ((pPacket->tx_tag.spn >> 4) & 0x3f);

    base++;
    *base = ((pPacket->tx_tag.spn & 0xf) << 4) | ((pPacket->tx_tag.dev_id) & 0xf);

    base++;
    *base = (pPacket->tx_tag.dst_port_mask_1 >> 16)& 0xff;
    base++;
    *base = (pPacket->tx_tag.dst_port_mask_1 >> 8)& 0xff;
    base++;
    *base = pPacket->tx_tag.dst_port_mask_1 & 0xff;
    base++;
    *(uint32*)base = htonl(pPacket->tx_tag.dst_port_mask);
}

void _cpu_rxTag_prcess(drv_nic_pkt_t *pPacket)
{
    nic_9310_cpuTag_t cputag;

    if (RT_ERR_OK != _extractCPUTag(pPacket->data, &cputag))
    {
        return ;
    }
    pPacket->data = (uint8*)((uintptr)(pPacket->data) + 20);
    pPacket->length -= 20;

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
        pPacket->rx_tag.ext_dev_id      = cputag.un.rx.IDX >> 6;
        pPacket->rx_tag.ext_source_port = cputag.un.rx.IDX & 0x3f;
    }
    else
    {
        pPacket->rx_tag.ext_is_trk      = cputag.un.rx.PORT_DATA_IS_TRK;
        if (pPacket->rx_tag.ext_is_trk)
            pPacket->rx_tag.ext_trk_id      = cputag.un.rx.PORT_DATA;
        else
        {
            pPacket->rx_tag.ext_dev_id      = cputag.un.rx.PORT_DATA >> 6;
            pPacket->rx_tag.ext_source_port = cputag.un.rx.PORT_DATA & 0x3f;
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

int32 _nic_pkt_dump(uint32 unit, drv_nic_pkt_t *pPacket, uint32 dir)
{
    int i;
    int dump_len = DEBUG_DUMP_PKT_LEN; /* debug dump maximum length */

    if (dir == 0)
        osal_printf("=== [NIC TX Debug] ================================= Len: %d \n", pPacket->length);
    else
        osal_printf("=== [NIC RX Debug] ================================= Len: %d \n", pPacket->length);

    for (i = 0; i < dump_len; i++)
    {
        if (i == (pPacket->length))
            break;
        if (0 == (i % 16))
            osal_printf("[%04X] ", i);
        osal_printf("%02X ", *(unsigned char*)(pPacket->data + i));
        if (15 == (i % 16))
            osal_printf("\n");
    }
    osal_printf("\n");

    return 0;
}

int32 _nic_cpu_txTag_dump(uint32 unit, drv_nic_pkt_t *pPacket)
{
    int i;
    int dump_len = 20;
    osal_printf("=== [NIC TX Debug - CPU Tx Tag Information] ============ \n");
    for (i = 0; i < dump_len; i++)
        osal_printf("%02X ", pPacket->data[12 + i]);
    osal_printf("\n");

    osal_printf(" FWD_TYPE : 0x%0x \n", pPacket->tx_tag.fwd_type);
    osal_printf(" ACL_ACT : 0x%0x \n", pPacket->tx_tag.acl_act);
    osal_printf(" CNGST_DROP : 0x%0x \n", pPacket->tx_tag.cngst_drop);
    osal_printf(" DM_PKT : 0x%0x \n", pPacket->tx_tag.dm_pkt);
    osal_printf(" DG_PKT : 0x%0x \n", pPacket->tx_tag.dg_pkt);
    osal_printf(" BP_FLTR : 0x%0x \n", pPacket->tx_tag.bp_fltr);
    osal_printf(" BP_STP : 0x%0x \n", pPacket->tx_tag.bp_stp);
    osal_printf(" BP_VLAN_EGR : 0x%0x \n", pPacket->tx_tag.bp_vlan_egr);
    osal_printf(" AS_TAGSTS : 0x%0x \n", pPacket->tx_tag.as_tagSts);
    osal_printf(" ORI_TAGIF_EN : 0x%0x \n", pPacket->tx_tag.ori_tagif_en);
    osal_printf(" L3_ACT : 0x%0x \n", pPacket->tx_tag.l3_act);
    osal_printf(" AS_QID : 0x%0x \n", pPacket->tx_tag.as_priority);
    osal_printf(" QID : 0x%0x \n", pPacket->tx_tag.priority);
    osal_printf(" ORI_ITAGIF : 0x%0x \n", pPacket->tx_tag.ori_itagif);
    osal_printf(" ORI_OTAGIF : 0x%0x \n", pPacket->tx_tag.ori_otagif);
    osal_printf(" FVID_SEL : 0x%0x \n", pPacket->tx_tag.fvid_sel);
    osal_printf(" FVID_EN : 0x%0x \n", pPacket->tx_tag.fvid_en);
    osal_printf(" FVID : 0x%0x \n", pPacket->tx_tag.fvid);
    osal_printf(" SRC_FLTR_EN : 0x%0x \n", pPacket->tx_tag.src_filter_en);
    osal_printf(" SP_IS_TRK : 0x%0x \n", pPacket->tx_tag.sp_is_trk);
    osal_printf(" SPN : 0x%0x \n", pPacket->tx_tag.spn);
    osal_printf(" SW_DEV_ID : 0x%0x \n", pPacket->tx_tag.dev_id);
    osal_printf(" DPM55_32 : 0x%0x \n", pPacket->tx_tag.dst_port_mask_1);
    osal_printf(" DPM31_0 : 0x%0x \n", pPacket->tx_tag.dst_port_mask);

    return RT_ERR_OK;
}
int32 _nic_cpu_rxTag_dump(uint32 unit, drv_nic_pkt_t *pPacket)
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

uint32 _chksum_crc32 (unsigned char *block, unsigned int length)
{
   uint32 crc;
   unsigned long i;

   crc = 0xFFFFFFFF;
   for (i = 0; i < length; i++)
   {
      crc = ((crc >> 8) & 0x00FFFFFF) ^ crcTable[(crc ^ *block) & 0xFF];
      block++;
   }
   return (crc ^ 0xFFFFFFFF);
}

int _rxChkCRC(drv_nic_pkt_t *pPacket)
{
    int32   ret = 0;
    uint32  crcVal, len;

    len = rxCRCInclude ? pPacket->length : pPacket->length + 4;
    crcVal = _chksum_crc32(pPacket->data, len - 4);
    if ((pPacket->data[len - 1] != (crcVal >> 24) || pPacket->data[len - 2] != ((crcVal >> 16) & 0xff) ||
                pPacket->data[len - 3] != ((crcVal >> 8) & 0xff) || pPacket->data[len - 4] != (crcVal & 0xff)))
    {
        osal_printf("%s():%d CRC error  len:%d\n", __FUNCTION__, __LINE__, len);
        stats.rx_crc_errors++;
        ret = RT_ERR_FAILED;
    }

    return ret;
}

int proc_get_tally_counter(void)
{
        struct rtl8168_counters *counters;
        u32 cmd;
        u32 WaitCnt;

        counters = tp->tally_vaddr;
        RTL_W32(CounterAddrHigh, (u64)tp->tally_paddr >> 32);
        cmd = (u64)tp->tally_paddr & DMA_BIT_MASK(32);
        RTL_W32(CounterAddrLow, cmd);
        RTL_W32(CounterAddrLow, cmd | CounterDump);

        WaitCnt = 0;
        while (RTL_R32(CounterAddrLow) & CounterDump) {
                osal_time_usleep(10);

                WaitCnt++;
                if (WaitCnt > 20)
                        break;
        }

        osal_printf("\nTally Counter:\n");
        osal_printf("tx_packets\t\t   : %llu\n", (uint64)le64_to_cpu(counters->tx_packets));
        osal_printf("rx_packets\t\t   : %llu\n", (uint64)le64_to_cpu(counters->rx_packets));
        osal_printf("tx_errors\t\t   : %llu\n", (uint64)le64_to_cpu(counters->tx_errors));
        osal_printf("rx_missed\t\t   : %llu\n", (uint64)le64_to_cpu(counters->rx_missed));
        osal_printf("align_errors\t\t   : %llu\n", (uint64)le64_to_cpu(counters->align_errors));
        osal_printf("tx_one_collision           : %llu\n", (uint64)le64_to_cpu(counters->tx_one_collision));
        osal_printf("tx_multi_collision\t   : %llu\n", (uint64)le64_to_cpu(counters->tx_multi_collision));
        osal_printf("rx_unicast\t\t   : %llu\n", (uint64)le64_to_cpu(counters->rx_unicast));
        osal_printf("rx_broadcast\t\t   : %llu\n", (uint64)le64_to_cpu(counters->rx_broadcast));
        osal_printf("rx_multicast\t\t   : %llu\n", (uint64)le64_to_cpu(counters->rx_multicast));
        osal_printf("tx_aborted\t\t   : %llu\n", (uint64)le64_to_cpu(counters->tx_aborted));
        osal_printf("tx_underun\t\t   : %llu\n", (uint64)le64_to_cpu(counters->tx_underun));

        return 0;
}

void mac_ocp_write(u16 reg_addr, u16 value)
{
        u32 data32;


        data32 = reg_addr/2;
        data32 <<= OCPR_Addr_Reg_shift;
        data32 += value;
        data32 |= OCPR_Write;

        RTL_W32(MACOCP, data32);
}

u16 mac_ocp_read(u16 reg_addr)
{
        u32 data32;
        u16 data16 = 0;


        data32 = reg_addr/2;
        data32 <<= OCPR_Addr_Reg_shift;

        RTL_W32(MACOCP, data32);
        data16 = (u16)RTL_R32(MACOCP);

        return data16;
}

static void
rtl8168_enable_cfg9346_write(void)
{
    RTL_W8(Cfg9346, RTL_R8(Cfg9346) | Cfg9346_Unlock);
}

static void
rtl8168_disable_cfg9346_write(void)
{
    RTL_W8(Cfg9346, RTL_R8(Cfg9346) & ~Cfg9346_Unlock);
}

static u32
rtl8168_csi_other_fun_read(u8 multi_fun_sel_bit, u32 addr)
{
        u32 cmd;
        int i;
        u32 value = 0;

        cmd = CSIAR_Read | CSIAR_ByteEn << CSIAR_ByteEn_shift | (addr & CSIAR_Addr_Mask);

        multi_fun_sel_bit = 0;

        if( multi_fun_sel_bit > 7 ) {
                return 0xffffffff;
        }

        cmd |= multi_fun_sel_bit << 16;

        RTL_W32(CSIAR, cmd);

        for (i = 0; i < 10; i++) {
                osal_time_usleep(100);

                /* Check if the RTL8168 has completed CSI read */
                if (RTL_R32(CSIAR) & CSIAR_Flag) {
                        value = (u32)RTL_R32(CSIDR);
                        break;
                }
        }

        osal_time_usleep(20);

        return value;
}

static void
rtl8168_csi_other_fun_write(u8 multi_fun_sel_bit, u32 addr, u32 value)
{
        u32 cmd;
        int i;

        RTL_W32(CSIDR, value);
        cmd = CSIAR_Write | CSIAR_ByteEn << CSIAR_ByteEn_shift | (addr & CSIAR_Addr_Mask);
        multi_fun_sel_bit = 0;

        if( multi_fun_sel_bit > 7 ) {
                return;
        }

        cmd |= multi_fun_sel_bit << 16;

        RTL_W32(CSIAR, cmd);

        for (i = 0; i < 10; i++) {
                osal_time_usleep(100);

                /* Check if the RTL8168 has completed CSI write */
                if (!(RTL_R32(CSIAR) & CSIAR_Flag))
                        break;
        }

        osal_time_usleep(20);
}

static u32
rtl8168_csi_read(u32 addr)
{
        u8 multi_fun_sel_bit;

        multi_fun_sel_bit = 0;
        return rtl8168_csi_other_fun_read(multi_fun_sel_bit, addr);
}

static void
rtl8168_csi_write(u32 addr, u32 value)
{
        u8 multi_fun_sel_bit;

        multi_fun_sel_bit = 0;
        rtl8168_csi_other_fun_write(multi_fun_sel_bit, addr, value);
}

u32 rtl8168_eri_read(int addr, int len, int type)
{
        int i, val_shift, shift = 0;
        u32 value1 = 0, value2 = 0, mask;
        u32 eri_cmd;

        if (len > 4 || len <= 0)
                return -1;

        while (len > 0) {
                val_shift = addr % ERIAR_Addr_Align;
                addr = addr & ~0x3;

                eri_cmd = ERIAR_Read |
                          type << ERIAR_Type_shift |
                          ERIAR_ByteEn << ERIAR_ByteEn_shift |
                          (addr & 0x0FFF);
                if (addr & 0xF000) {
                        u32 tmp;

                        tmp = addr & 0xF000;
                        tmp >>= 12;
                        eri_cmd |= (tmp << 20) & 0x00F00000;
                }

                RTL_W32(ERIAR, eri_cmd);

                for (i = 0; i < 10; i++) {
                        osal_time_usleep(100);

                        /* Check if the RTL8168 has completed ERI read */
                        if (RTL_R32(ERIAR) & ERIAR_Flag)
                                break;
                }

                if (len == 1)       mask = (0xFF << (val_shift * 8)) & 0xFFFFFFFF;
                else if (len == 2)  mask = (0xFFFF << (val_shift * 8)) & 0xFFFFFFFF;
                else if (len == 3)  mask = (0xFFFFFF << (val_shift * 8)) & 0xFFFFFFFF;
                else            mask = (0xFFFFFFFF << (val_shift * 8)) & 0xFFFFFFFF;

                value1 = RTL_R32(ERIDR) & mask;
                value2 |= (value1 >> val_shift * 8) << shift * 8;

                if (len <= 4 - val_shift) {
                        len = 0;
                } else {
                        len -= (4 - val_shift);
                        shift = 4 - val_shift;
                        addr += 4;
                }
        }

        osal_time_usleep(20);

        return value2;
}

int rtl8168_eri_write(int addr, int len, u32 value, int type)
{

        int i, val_shift, shift = 0;
        u32 value1 = 0, mask;
        u32 eri_cmd;

        if (len > 4 || len <= 0)
                return -1;

        while (len > 0) {
                val_shift = addr % ERIAR_Addr_Align;
                addr = addr & ~0x3;

                if (len == 1)       mask = (0xFF << (val_shift * 8)) & 0xFFFFFFFF;
                else if (len == 2)  mask = (0xFFFF << (val_shift * 8)) & 0xFFFFFFFF;
                else if (len == 3)  mask = (0xFFFFFF << (val_shift * 8)) & 0xFFFFFFFF;
                else            mask = (0xFFFFFFFF << (val_shift * 8)) & 0xFFFFFFFF;

                value1 = rtl8168_eri_read(addr, 4, type) & ~mask;
                value1 |= ((value << val_shift * 8) >> shift * 8);

                RTL_W32(ERIDR, value1);

                eri_cmd = ERIAR_Write |
                          type << ERIAR_Type_shift |
                          ERIAR_ByteEn << ERIAR_ByteEn_shift |
                          (addr & 0x0FFF);
                if (addr & 0xF000) {
                        u32 tmp;

                        tmp = addr & 0xF000;
                        tmp >>= 12;
                        eri_cmd |= (tmp << 20) & 0x00F00000;
                }

                RTL_W32(ERIAR, eri_cmd);

                for (i = 0; i < 10; i++) {
                        osal_time_usleep(100);

                        /* Check if the RTL8168 has completed ERI write */
                        if (!(RTL_R32(ERIAR) & ERIAR_Flag))
                                break;
                }

                if (len <= 4 - val_shift) {
                        len = 0;
                } else {
                        len -= (4 - val_shift);
                        shift = 4 - val_shift;
                        addr += 4;
                }
        }

        osal_time_usleep(20);


        return 0;
}

static inline void
rtl8168_mark_to_asic(struct RxDesc *desc,
                     u32 rx_buf_sz)
{
        u32 eor = le32_to_cpu(desc->opts1) & RingEnd;

        desc->opts1 = cpu_to_le32(DescOwn | eor | rx_buf_sz);
}

static inline void
rtl8168_map_to_asic(struct RxDesc *desc,
                    uintptr mapping,
                    u32 rx_buf_sz)
{
        desc->addr = cpu_to_le64(mapping);

        MEMORY_BARRIER();

        rtl8168_mark_to_asic(desc, rx_buf_sz);
}

static inline void
rtl8168_disable_hw_interrupt(void)
{
        RTL_W16(IntrMask, 0x0000);
}

static void
rtl8168_irq_mask_and_ack(void)
{
        rtl8168_disable_hw_interrupt();
        RTL_W16(IntrStatus, RTL_R16(IntrStatus));
}

static int
rtl8168_get_mac_address(void)
{

        return 0;
}

static void
rtl8168_tally_counter_addr_fill(void)
{
        if (!tp->tally_paddr)
                return;

        RTL_W32(CounterAddrHigh, (u64)tp->tally_paddr >> 32);
        RTL_W32(CounterAddrLow, (u64)tp->tally_paddr & (DMA_BIT_MASK(32)));
}

static void
rtl8168_tally_counter_clear(void)
{
        if (!tp->tally_paddr)
                return;

        RTL_W32(CounterAddrHigh, (u64)tp->tally_paddr >> 32);
        RTL_W32(CounterAddrLow, ((u64)tp->tally_paddr & (DMA_BIT_MASK(32))) | CounterReset);
}

static inline int
rtl8168_fragmented_frame(u32 status)
{
        return (status & (FirstFrag | LastFrag)) != (FirstFrag | LastFrag);
}

static void
rtl8168_nic_reset(void)
{
        int i;

        RTL_W32(RxConfig, (RX_DMA_BURST << RxCfgDMAShift));

        RTL_W8(0xF2, RTL_R8(0xF2) | BIT_3);
        osal_time_usleep(2000);

        for (i = 0; i < 10; i++) {
            osal_time_usleep(100);
            if (RTL_R32(TxConfig) & BIT_11)
                break;
        }

        for (i = 0; i < 10; i++) {
            osal_time_usleep(100);
            if ((RTL_R8(MCUCmd_reg) & (Txfifo_empty | Rxfifo_empty)) == (Txfifo_empty | Rxfifo_empty))
                break;

        }

        osal_time_usleep(2000);

        /* Soft reset the chip. */
        RTL_W8(ChipCmd, CmdReset);

        /* Check that the chip has finished the reset. */
        for (i = 100; i > 0; i--) {
            osal_time_usleep(100);
            if ((RTL_R8(ChipCmd) & CmdReset) == 0)
                break;
        }

}

static void
rtl8168_exit_oob(void)
{
        RTL_W32(RxConfig, RTL_R32(RxConfig) & ~(AcceptErr | AcceptRunt | AcceptBroadcast | AcceptMulticast | AcceptMyPhys |  AcceptAllPhys));

        switch (tp->mcfg) {
        case CFG_METHOD_30: {
                u32 csi_tmp;
                csi_tmp = rtl8168_eri_read(0x174, 2, ERIAR_ExGMAC);
                csi_tmp &= ~(BIT_8);
                csi_tmp |= (BIT_15);
                rtl8168_eri_write(0x174, 2, csi_tmp, ERIAR_ExGMAC);
                mac_ocp_write(0xE428, 0x0010);
        }
        break;
        }

        rtl8168_nic_reset();
}

static inline void
rtl8168_make_unusable_by_asic(struct RxDesc *desc)
{
        desc->addr = 0x0badbadbadbadbadull;
        desc->opts1 &= ~cpu_to_le32(DescOwn | RsvdMask);
}

static void
rtl8168_hw_set_rx_packet_filter(void)
{
        RTL_W32(RxConfig, 0x2cf3f);
        RTL_W32(MAR0 + 0, 0xffffffff);
        RTL_W32(MAR0 + 4, 0xffffffff);
}

static int
rtl8168_alloc_rx_pktBuf(drv_nic_pkt_t **ppPacket,
                     struct RxDesc *desc,
                     int rx_buf_sz,
                     u8 in_intr)
{
        drv_nic_pkt_t   *pPacket;
        uintptr mapping;
        int ret = 0;

        if (RT_ERR_FAILED == _nic_init_conf.rx_pkt_alloc(0, rx_buf_sz, PKTBUF_RX, &pPacket))
        {
            goto err_out;
        }

#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
        mapping = DMA_ADDR_USR2PHY((uintptr)pPacket->data);
#else
        mapping = (uintptr)pci_map_single(tp->pci_dev, pPacket->data, rx_buf_sz, PCI_DMA_FROMDEVICE);
#endif
        //dma_cache_wback_inv(skb->data, rx_buf_sz + RTK_RX_ALIGN);
        //dma_cache_sync(tp->pci_dev, skb->data, rx_buf_sz + RTK_RX_ALIGN, PCI_DMA_TODEVICE);

        *ppPacket = pPacket;
        rtl8168_map_to_asic(desc, mapping, rx_buf_sz);
out:
        return ret;

err_out:
        ret = RT_ERR_FAILED;
        rtl8168_make_unusable_by_asic(desc);
        goto out;
}

static inline void
rtl8168_mark_as_last_descriptor(struct RxDesc *desc)
{
        desc->opts1 |= cpu_to_le32(RingEnd);
}

static void
rtl8168_free_rx_pktBuf(drv_nic_pkt_t **ppPacket, struct RxDesc *desc)
{
        rtl8168_free_cnt++;
        _nic_init_conf.pkt_free(0, *ppPacket);
        *ppPacket = NULL;
        rtl8168_make_unusable_by_asic(desc);
}

static void
rtl8168_unmap_tx_skb(struct pci_dev *pdev,
                     struct ring_info *tx_skb,
                     struct TxDesc *desc)
{
        desc->opts1 = 0x00;
        desc->opts2 = 0x00;
        desc->addr = 0x00;
        tx_skb->len = 0;
}

static void rtl8168_tx_clear_range(u32 start, unsigned int n)
{
        unsigned int i;

        for (i = 0; i < n; i++) {
                unsigned int entry = (start + i) % NUM_TX_DESC;
                struct ring_info *tx_skb = tp->tx_skb + entry;
                unsigned int len = tx_skb->len;

                if (len) {
                        drv_nic_pkt_t *pPacket = tx_skb->pPacket;

                        rtl8168_unmap_tx_skb(tp->pci_dev, tx_skb,
                                             tp->TxDescArray + entry);
                        if (pPacket) {
                                stats.tx_dropped++;
                                _nic_init_conf.pkt_free(0, pPacket);
                                tx_skb->pPacket = NULL;
                        }
                }
        }
}

static void
rtl8168_tx_clear(void)
{
        rtl8168_tx_clear_range(tp->dirty_tx, NUM_TX_DESC);
        tp->cur_tx = tp->dirty_tx = tp->txRDPIdx = 0;
}

static void
rtl8168_rx_clear(void)
{
        int i;

        for (i = 0; i < NUM_RX_DESC; i++) {
                if (tp->Rx_skbuff[i])
                        rtl8168_free_rx_pktBuf(tp->Rx_skbuff + i,
                                            tp->RxDescArray + i);
        }
}

static u32
rtl8168_rx_fill(u32 start, u32 end, u8 in_intr)
{
        u32 cur;

        for (cur = start; end - cur > 0; cur++) {
                int ret, i = cur % NUM_RX_DESC;

                if (tp->Rx_skbuff[i])
                        continue;

                ret = rtl8168_alloc_rx_pktBuf(tp->Rx_skbuff + i,
                                           tp->RxDescArray + i,
                                           tp->rx_buf_sz,
                                           in_intr);

                if (ret < 0)
                        break;
        }
        return cur - start;
}

static void
rtl8168_desc_addr_fill(void)
{
        if (!tp->TxPhyAddr || !tp->RxPhyAddr)
                return;

        RTL_W32(TxDescStartAddrLow, ((u64) tp->TxPhyAddr & DMA_BIT_MASK(32)));
        RTL_W32(TxDescStartAddrHigh, ((u64) tp->TxPhyAddr >> 32));
        RTL_W32(RxDescAddrLow, ((u64) tp->RxPhyAddr & DMA_BIT_MASK(32)));
        RTL_W32(RxDescAddrHigh, ((u64) tp->RxPhyAddr >> 32));
}

static void
rtl8168_tx_desc_init(void)
{
        int i = 0;

        memset(tp->TxDescArray, 0x0, NUM_TX_DESC * sizeof(struct TxDesc));

        for (i = 0; i < NUM_TX_DESC; i++) {
                if (i == (NUM_TX_DESC - 1))
                        tp->TxDescArray[i].opts1 = cpu_to_le32(RingEnd);
        }
}

static void
rtl8168_rx_desc_init(void)
{
        memset(tp->RxDescArray, 0x0, NUM_RX_DESC * sizeof(struct RxDesc));
}

static int
rtl8168_init_ring(void)
{
        tp->txRDPIdx = 0;
        tp->dirty_tx = 0;
        tp->dirty_rx = 0;
        tp->cur_tx = 0;
        tp->cur_rx = 0;

        memset(tp->tx_skb, 0x0, NUM_TX_DESC * sizeof(struct ring_info));
        memset(tp->Rx_skbuff, 0x0, NUM_RX_DESC * sizeof(drv_nic_pkt_t *));

        /********************************************************/
        /* Note: TX/RX desc ring should uses uncachable address */
        /********************************************************/
        rtl8168_tx_desc_init();
        rtl8168_rx_desc_init();

        if (rtl8168_rx_fill(0, NUM_RX_DESC, 0) != NUM_RX_DESC)
                goto err_out;

        rtl8168_mark_as_last_descriptor(tp->RxDescArray + NUM_RX_DESC - 1);

        return 0;

err_out:
        rtl8168_rx_clear();
        return RT_ERR_FAILED;
}

static void
rtl8168_hw_init(void)
{
        u32 csi_tmp;

        rtl8168_enable_cfg9346_write();
        RTL_W8(Config5, RTL_R8(Config5) & ~BIT_0);
        RTL_W8(Config2, RTL_R8(Config2) & ~BIT_7);
        rtl8168_disable_cfg9346_write();
        RTL_W8(0xF1, RTL_R8(0xF1) & ~BIT_7);



        //Set PCIE uncorrectable error status mask pcie 0x108
        csi_tmp = rtl8168_csi_read(0x108);
        csi_tmp |= BIT_20;
        rtl8168_csi_write(0x108, csi_tmp);
}

static void
rtl8168_hw_reset(void)
{
        /* Disable interrupts */
        rtl8168_irq_mask_and_ack();

        rtl8168_nic_reset();
}

static void
rtl8168_hw_config(void)
{
        u16 mac_ocp_data;
        u32 csi_tmp;


        RTL_W32(RxConfig, (RX_DMA_BURST << RxCfgDMAShift));

        rtl8168_hw_reset();

        rtl8168_enable_cfg9346_write();

        RTL_W8(0xF1, RTL_R8(0xF1) & ~BIT_7);
        RTL_W8(Config2, RTL_R8(Config2) & ~BIT_7);
        RTL_W8(Config5, RTL_R8(Config5) & ~BIT_0);

        //clear io_rdy_l23
        RTL_W8(Config3, RTL_R8(Config3) & ~BIT_1);

        RTL_W8(MTPS, Reserved1_data);

        tp->cp_cmd |= INTT_1;
        if (tp->use_timer_interrrupt)
            tp->cp_cmd |= PktCntrDisable;
        else
            tp->cp_cmd &= ~PktCntrDisable;

        RTL_W16(IntrMitigate, 0x5f51);

        rtl8168_tally_counter_addr_fill();

        rtl8168_desc_addr_fill();

        /* Set DMA burst size and Interframe Gap Time */
        RTL_W32(TxConfig, (TX_DMA_BURST_unlimited << TxDMAShift) |
                        (InterFrameGap << TxInterFrameGapShift));


        if (tp->mcfg == CFG_METHOD_30)
        {
                rtl8168_eri_write(0xC8, 4, 0x00080002, ERIAR_ExGMAC);
                rtl8168_eri_write(0xCC, 1, 0x38, ERIAR_ExGMAC);
                rtl8168_eri_write(0xD0, 1, 0x48, ERIAR_ExGMAC);
                rtl8168_eri_write(0xE8, 4, 0x00100006, ERIAR_ExGMAC);

                RTL_W32(TxConfig, RTL_R32(TxConfig) | BIT_7);

                csi_tmp = rtl8168_eri_read(0xDC, 1, ERIAR_ExGMAC);
                csi_tmp &= ~BIT_0;
                rtl8168_eri_write(0xDC, 1, csi_tmp, ERIAR_ExGMAC);
                csi_tmp |= BIT_0;
                rtl8168_eri_write(0xDC, 1, csi_tmp, ERIAR_ExGMAC);


                if(tp->RequireAdjustUpsTxLinkPulseTiming) {
                        mac_ocp_data = mac_ocp_read(0xD412);
                        mac_ocp_data &= ~(0x0FFF);
                        mac_ocp_data |= tp->SwrCnt1msIni ;
                        mac_ocp_write(0xD412, mac_ocp_data);
                }

                mac_ocp_data = mac_ocp_read(0xE056);
                mac_ocp_data &= ~(BIT_7 | BIT_6 | BIT_5 | BIT_4);
                mac_ocp_data |= (BIT_6 | BIT_5 | BIT_4);
                mac_ocp_write(0xE056, mac_ocp_data);

                mac_ocp_data = mac_ocp_read(0xE052);
                mac_ocp_data &= ~( BIT_14 | BIT_13);
                mac_ocp_data |= BIT_15;
                mac_ocp_data |= BIT_3;
                mac_ocp_write(0xE052, mac_ocp_data);

                mac_ocp_data = mac_ocp_read(0xD420);
                mac_ocp_data &= ~(BIT_11 | BIT_10 | BIT_9 | BIT_8 | BIT_7 | BIT_6 | BIT_5 | BIT_4 | BIT_3 | BIT_2 | BIT_1 | BIT_0);
                mac_ocp_data |= 0x47F;
                mac_ocp_write(0xD420, mac_ocp_data);

                mac_ocp_data = mac_ocp_read(0xE0D6);
                mac_ocp_data &= ~(BIT_8 | BIT_7 | BIT_6 | BIT_5 | BIT_4 | BIT_3 | BIT_2 | BIT_1 | BIT_0);
                mac_ocp_data |= 0x17F;
                mac_ocp_write(0xE0D6, mac_ocp_data);

                mac_ocp_data = mac_ocp_read(0xE0D8);
                mac_ocp_data |= 1;
                mac_ocp_write(0xE0D8, mac_ocp_data);

                RTL_W8(Config3, RTL_R8(Config3) & ~Beacon_en);

                RTL_W8(0x1B, RTL_R8(0x1B) & ~0x07);

                RTL_W8(TDFNR, 0x4);

                RTL_W8(Config2, RTL_R8(Config2) & ~PMSTS_En);


                RTL_W8(0xD0, RTL_R8(0xD0) | BIT_6);
                RTL_W8(0xF2, RTL_R8(0xF2) | BIT_6);

                RTL_W8(0xD0, RTL_R8(0xD0) | BIT_7);

                rtl8168_eri_write(0xC0, 2, 0x0000, ERIAR_ExGMAC);
                rtl8168_eri_write(0xB8, 4, 0x00000000, ERIAR_ExGMAC);

                rtl8168_eri_write(0x5F0, 2, 0x4F87, ERIAR_ExGMAC);


                csi_tmp = rtl8168_eri_read(0xD4, 4, ERIAR_ExGMAC);
                csi_tmp |= (BIT_8 | BIT_9 | BIT_10 | BIT_11 | BIT_12);
                rtl8168_eri_write(0xD4, 4, csi_tmp, ERIAR_ExGMAC);

                csi_tmp = rtl8168_eri_read(0xDC, 4, ERIAR_ExGMAC);
                csi_tmp |= (BIT_2 | BIT_3 | BIT_4);
                rtl8168_eri_write(0xDC, 4, csi_tmp, ERIAR_ExGMAC);


                mac_ocp_write(0xC140, 0xFFFF);
                mac_ocp_write(0xC142, 0xFFFF);

                csi_tmp = rtl8168_eri_read(0x1B0, 4, ERIAR_ExGMAC);
                csi_tmp &= ~BIT_12;
                rtl8168_eri_write(0x1B0, 4, csi_tmp, ERIAR_ExGMAC);

                csi_tmp = rtl8168_eri_read(0x2FC, 1, ERIAR_ExGMAC);
                csi_tmp &= ~(BIT_2);
                rtl8168_eri_write(0x2FC, 1, csi_tmp, ERIAR_ExGMAC);

                csi_tmp = rtl8168_eri_read(0x1D0, 1, ERIAR_ExGMAC);
                csi_tmp |= BIT_1;
                rtl8168_eri_write(0x1D0, 1, csi_tmp, ERIAR_ExGMAC);
        }

        /* csum offload command for RTL8168C/8111C and RTL8168CP/8111CP */
        tp->tx_tcp_csum_cmd = TxTCPCS_C;
        tp->tx_udp_csum_cmd =TxUDPCS_C;
        tp->tx_ip_csum_cmd = TxIPCS_C;
        tp->tx_ipv6_csum_cmd = TxIPV6F_C;



        mac_ocp_write(0xE098, 0x0AA2);


        tp->cp_cmd &= ~(EnableBist | Macdbgo_oe | Force_halfdup |
                        Force_rxflow_en | Force_txflow_en | Cxpl_dbg_sel |
                        ASF | Macdbgo_sel);


        switch (tp->mcfg) {
        case CFG_METHOD_30:	{
                int timeout;
                for (timeout = 0; timeout < 10; timeout++) {
                        if ((rtl8168_eri_read(0x1AE, 2, ERIAR_ExGMAC) & BIT_13)==0)
                                break;
                        osal_time_usleep(1000);
                }
        }
        break;
        }

        RTL_W16(RxMaxSize, _nic_init_conf.jumbo_size + NIC_9310_CPU_TAG_SIZE);

        RTL_W8(0xF2, RTL_R8(0xF2) & ~BIT_3);
        osal_time_usleep(2000);


        /* Set Rx packet filter */
        rtl8168_hw_set_rx_packet_filter();


        rtl8168_disable_cfg9346_write();

        osal_time_usleep(10);
}

static void rtl8168_down(void)
{
        rtl8168_hw_reset();

        rtl8168_tx_clear();

        rtl8168_rx_clear();
}

static void
rtl8168_tx_interrupt(uint32 unit)
{
    uint32      cnt, status;
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    uint32      len;
    unsigned long   spl_flags;
#endif


    NIC_LOCK(unit, &spl_flags);

    for (cnt = 0; cnt < NUM_TX_DESC; cnt++)
    {
        struct ring_info *tx_skb = tp->tx_skb + tp->txRDPIdx;

        status = le32_to_cpu(tp->TxDescArray[tp->txRDPIdx].opts1);
        if (status & DescOwn)
            break;

        if (NULL == tx_skb->pPacket)
        {
            break;
        }

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
        len = tx_skb->pPacket->txIncludeCRC ? tx_skb->pPacket->length - 4 : tx_skb->pPacket->length;
        pci_unmap_single(tp->pci_dev, tp->TxDescArray[tp->txRDPIdx].addr, len, PCI_DMA_TODEVICE);
#endif
        if (tx_skb->tx_callback == NULL)
        {
            _nic_init_conf.pkt_free(unit, tx_skb->pPacket);
        }
        else
        {
            tx_skb->tx_callback(unit, tx_skb->pPacket, tx_skb->cookie);
            tx_skb->tx_callback = NULL;
        }
        tx_skb->pPacket = NULL;

        tp->txRDPIdx = (tp->txRDPIdx + 1) % NUM_TX_DESC;
    }

    NIC_UNLOCK(unit, &spl_flags);
}

static int
rtl8168_rx_interrupt(uint32 unit)
{
    int32           ret = RT_ERR_FAILED;
    uint32          i, cnt;
    uint32          status, pkt_size;
    struct RxDesc   *desc;
    static drv_nic_pkt_t   *pPacket;
    drv_nic_rx_t    nic_rx_handle = NIC_RX_NOT_HANDLED;
    uint32          schd_queId = 0;
    static uint32   jumboFlag, jumboBuffAlloc = FALSE;


    for (cnt = 0; cnt < NUM_RX_DESC; cnt++)
    {
        uint8           handled = FALSE;
        uint8           reclaim_mbuf = TRUE;
        nic_rx_queue_t  *pRx_queue = NULL;
#if defined(CONFIG_SDK_CPU_RATE_PROCESS)
        uint32          bkt_cnt = 0;
        uint32          rate_en = 0;
#endif


        desc = tp->RxDescArray + tp->cur_rx;

        status = le32_to_cpu(desc->opts1);
        if (status & DescOwn)
        {
            break;
        }

        if (unlikely(status & RxRES))
        {
            stats.rx_errors++;
        }

        if (tp->Rx_skbuff[tp->cur_rx] == NULL)
        {
            break;
        }


        nic_rx_handle   = NIC_RX_NOT_HANDLED;
        pkt_size        = status & 0x00003fff;

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
        pci_unmap_single(tp->pci_dev, desc->addr, _nic_init_conf.pkt_size, PCI_DMA_FROMDEVICE);
#endif

        if ((status & FirstFrag) && (status & LastFrag) == FALSE)     /* Jumbo head */
        {
            jumboFlag = TRUE;
            if (RT_ERR_OK == _nic_init_conf.rx_pkt_alloc(unit, _nic_init_conf.jumbo_size, 0, &pPacket))
            {
                jumboBuffAlloc = TRUE;
                osal_memcpy(pPacket->data, tp->Rx_skbuff[tp->cur_rx]->data, pkt_size);
                pPacket->length = pkt_size;
            }
            else
            {
                pPacket = NULL;
                stats.rx_errors++;
            }
        }
        else if (jumboFlag == TRUE)     /* Jumbo other */
        {
            if (jumboBuffAlloc)
            {
                osal_memcpy(pPacket->data + pPacket->length, tp->Rx_skbuff[tp->cur_rx]->data, pkt_size);
                pPacket->length += pkt_size;
                pPacket->tail = pPacket->data + pPacket->length;
                if (unlikely(pPacket->tail > pPacket->end))
                    osal_printf("%s():%d  !!!!  head:%p  data:%p  tail:%p  end:%p\n", __FUNCTION__, __LINE__, pPacket->head, pPacket->data, pPacket->tail, pPacket->end);
            }
        }
        else        /* Normal packet */
        {
            pPacket = tp->Rx_skbuff[tp->cur_rx];
            pPacket->length = pkt_size;
            pPacket->tail   = pPacket->data + pkt_size;
            if (unlikely(pPacket->tail > pPacket->end))
                osal_printf("%s():%d  !!!!  head:%p  data:%p  tail:%p  end:%p\n", __FUNCTION__, __LINE__, pPacket->head, pPacket->data, pPacket->tail, pPacket->end);
        }


        if ((0 == rxCRCInclude) && (status & LastFrag) && pPacket != NULL)
        {
            pPacket->length -= 4;
            pPacket->tail -= 4;
        }


        /* NIC Rx debug message */
        if ((status & LastFrag) && (jumboFlag == 0 || (jumboFlag && jumboBuffAlloc)) && (nic_debug_flag & DEBUG_RX_RAW_LEN_BIT))
            _nic_pkt_dump(unit, pPacket, 1);

        if ((status & LastFrag) && (jumboFlag == 0 || (jumboFlag && jumboBuffAlloc)) && (nic_debug_flag & DEBUG_CPU_CALCRC_BIT))
            _rxChkCRC(pPacket);


        //CPU Tag process
        if ((status & LastFrag) && (jumboFlag == 0 || (jumboFlag && jumboBuffAlloc)) && (pPacket->data[12] == 0x88 && pPacket->data[13] == 0x99 && pPacket->data[14] == 0x04))
        {
            _cpu_rxTag_prcess(pPacket);

            /* NIC Rx debug message */
            if (nic_debug_flag & DEBUG_RX_CPU_TAG_BIT)
                _nic_cpu_rxTag_dump(unit, pPacket);
        }

        if (jumboFlag == 0 || (jumboFlag && (status & LastFrag) && jumboBuffAlloc))
        {
            if (0 != _nic_rx_intr_cb_cnt)
            {
                for (i = 0; i < NIC_RX_CB_PRIORITY_NUMBER; i++)
                {
                    if (_nic_rx_cb_tbl[i].rx_callback != NULL)
                    {
                        nic_rx_handle = _nic_rx_cb_tbl[i].rx_callback(unit, pPacket, _nic_rx_cb_tbl[i].pCookie);
                    }
                    switch (nic_rx_handle)
                    {
                        case NIC_RX_NOT_HANDLED:
                            break;
                        case NIC_RX_HANDLED:
                            handled = TRUE;
                            break;
                        case NIC_RX_HANDLED_OWNED:
                            handled = TRUE;
                            if (jumboFlag == 0)
                                tp->Rx_skbuff[tp->cur_rx] = NULL;
                            break;
                        default:
                            break;
                    }
                    if (handled)
                    {
                        break;
                    }
                }
            }
            else
            {
                /* Process non-interrupt callback function */
                schd_queId = pPacket->rx_tag.qid;
                nic_rx_queueInfo_get(unit, schd_queId, &pRx_queue);
                if ((pRx_queue->drop_thresh > 0) && (pRx_queue->count < pRx_queue->drop_thresh))
                {
                    if ((ret = nic_rx_pkt_enqueue(schd_queId, pPacket)) == RT_ERR_OK)
                    {
                        /* Notify RX thread */
                        nic_rx_thread_notify(unit);
                        nic_rx_handle = NIC_RX_HANDLED_OWNED;
                        if (jumboFlag == 0)
                            tp->Rx_skbuff[tp->cur_rx] = NULL;
                    }
                    else
                    {
                        //RT_LOG(LOG_WARNING, (MOD_NIC), "RX enqueue %d failed!", schd_queId);
                        stats.rx_lack_buf_cntr++;
                    }
                }
                else
                {
                    //RT_LOG(LOG_WARNING, (MOD_NIC), "RX too fast, directly drop\n");
                    stats.rx_lack_buf_cntr++;
#if defined(CONFIG_SDK_CPU_RATE_PROCESS)
                    nic_rx_cpu_cfg_get(CPU_RATE_CFG_CAL_REFILL, &rate_en);
                    if(rate_en == TRUE)
                    {
                        nic_rx_cpu_cnt_get(&bkt_cnt);
                        if (bkt_cnt == 0)
                            nic_rx_thread_notify(unit);
                    }
#endif
                }
            }

            if (NIC_RX_HANDLED_OWNED != nic_rx_handle)
            {
                nic_end_free_cnt++;
                _nic_init_conf.pkt_free(unit, pPacket);
                if (jumboFlag == 0)
                    tp->Rx_skbuff[tp->cur_rx] = NULL;
            }
        }

        if (NULL == tp->Rx_skbuff[tp->cur_rx])
        {
            /* Alloc a new packet data buffer */
            if (RT_ERR_OK == _nic_init_conf.rx_pkt_alloc(unit, _nic_init_conf.pkt_size, PKTBUF_RX, &pPacket))
            {
                tp->Rx_skbuff[tp->cur_rx]   = pPacket;

                #if 0
                if ((ret = osal_cache_memory_flush((uint32)DMA_ADDR_USR2KRN(unit, pPacket->head), (pPacket->end - pPacket->head))) != RT_ERR_OK)
                {
                    return ret;
                }
                #endif
            }
            else
            {
                osal_printf("%s():%d  Out of memory ! (alloc a new packet data buffer failed)\n", __FUNCTION__, __LINE__);
                reclaim_mbuf = FALSE;
            }
        }

        if(TRUE == reclaim_mbuf)
        {
            uintptr mapping;
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
            mapping = (uintptr)pci_map_single(tp->pci_dev, tp->Rx_skbuff[tp->cur_rx]->data, _nic_init_conf.pkt_size, PCI_DMA_FROMDEVICE);
#else
            mapping = DMA_ADDR_USR2PHY((uintptr)tp->Rx_skbuff[tp->cur_rx]->data);
#endif
            rtl8168_map_to_asic(tp->RxDescArray + tp->cur_rx, mapping, _nic_init_conf.pkt_size);
        }

        if (jumboFlag == 0 || (jumboFlag && (status & LastFrag) && jumboBuffAlloc))
            stats.nic_rx_success_cntr++;
        if (jumboFlag && (status & LastFrag))
        {
            jumboFlag = FALSE;
            jumboBuffAlloc = FALSE;
        }

        tp->cur_rx = (tp->cur_rx + 1) % NUM_RX_DESC;

    }

    return RT_ERR_OK;
}

static int
rtl8168_rx_runout_interrupt(uint32 unit)
{
    uint32          cnt;
    struct RxDesc   *desc;
    uint32          status;
    drv_nic_pkt_t   *pPacket;
    uintptr         mapping;

    desc = tp->RxDescArray;
    for (cnt = 0; cnt < NUM_RX_DESC; cnt++, desc++)
    {
        status = le32_to_cpu(desc->opts1);
        //osal_printf("%s():%d  own:%#x\n", __FUNCTION__, __LINE__, status & DescOwn);
        if (!(status & DescOwn) && (tp->Rx_skbuff[cnt] == NULL))
        {
            if (RT_ERR_OK == _nic_init_conf.rx_pkt_alloc(unit, _nic_init_conf.pkt_size, PKTBUF_RX, &pPacket))
            {
                tp->Rx_skbuff[cnt]   = pPacket;
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
                mapping = (uintptr)pci_map_single(tp->pci_dev, pPacket->data, _nic_init_conf.pkt_size, PCI_DMA_FROMDEVICE);
#else
                mapping = DMA_ADDR_USR2PHY((uintptr)pPacket->data);
#endif
                rtl8168_map_to_asic(tp->RxDescArray + cnt, mapping, _nic_init_conf.pkt_size);
            }
            else
            {
                intr_runout_alloc_fail_cnt++;
                osal_time_usleep(1000);
                break;
            }
        }
    }

    return RT_ERR_OK;
}

static void
rtl8168_hw_start(void)
{
        RTL_W8(ChipCmd, CmdTxEnb | CmdRxEnb);

        //enable_hw_interrupt
        RTL_W16(IntrMask, tp->intr_mask);
}

static int
rtl8168_open(void)
{
        int retval;


        RTL_W32(0x44, 0x2CF3F);

        /*
         * Rx and Tx descriptors needs 256 bytes alignment.
         * pci_alloc_consistent provides more.
         */
        tp->TxDescArray = (struct TxDesc *)tx_desc_vaddr;
        tp->TxPhyAddr   = tx_desc_phy_addr;
        if (!tp->TxDescArray)
                goto err_free_all_allocated_mem;

        tp->RxDescArray = (struct RxDesc *)rx_desc_vaddr;
        tp->RxPhyAddr   = rx_desc_phy_addr;
        if (!tp->RxDescArray)
                goto err_free_all_allocated_mem;


        retval = rtl8168_init_ring();
        if (retval < 0)
                goto err_free_all_allocated_mem;


        rtl8168_exit_oob();

        rtl8168_hw_init();

        rtl8168_hw_reset();

        rtl8168_hw_config();

        rtl8168_hw_start();

out:

        RTL_W32(0x8, 0xffffffff);
        RTL_W32(0xc, 0xffffffff);

        _makeCrcTable();
        return retval;

err_free_all_allocated_mem:

        retval = RT_ERR_FAILED;
        goto out;
}

static int rtl8168_close(void)
{
        if (tp->TxDescArray!=NULL && tp->RxDescArray!=NULL) {

                rtl8168_down();

                tp->TxDescArray = NULL;
                tp->RxDescArray = NULL;
        }

        return 0;
}

int32
drv_nic_init(uint32 unit, drv_nic_initCfg_t *pInitCfg)
{
        int rc = 0;

        RT_PARAM_CHK(NULL == pInitCfg, RT_ERR_NULL_POINTER);

        tp = NULL;

        _nic_init_conf.pkt_size     = pInitCfg->pkt_size;
        _nic_init_conf.jumbo_size   = pInitCfg->jumbo_size;
        _nic_init_conf.rx_pkt_alloc = pInitCfg->rx_pkt_alloc;
        _nic_init_conf.pkt_alloc    = pInitCfg->pkt_alloc;
        _nic_init_conf.pkt_free     = pInitCfg->pkt_free;

        tp = osal_alloc(sizeof(struct rtl8168_private));
        if (NULL == tp)
        {
            rc = RT_ERR_MEM_ALLOC;
            goto err_alloc;
        }
        memset(tp, 0, sizeof(struct rtl8168_private));
        tp->pci_dev = (struct pci_dev *)pInitCfg->dev;
        tp->rx_buf_sz = pInitCfg->pkt_size;
        tp->mcfg = CFG_METHOD_30;
        tp->cp_cmd |= RTL_R16(CPlusCmd);
        tp->intr_mask = 0x15;
        msi_feature = 2;
        tp->eeprom_type = EEPROM_TYPE_NONE;
        tp->tally_vaddr = (struct rtl8168_counters *)tally_vaddr;
        tp->tally_paddr = tally_phy_addr;


#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
        nic_sem         = osal_sem_mutex_create();
        nic_reset_sem   = osal_sem_mutex_create();
#elif defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
        spl_lock        = osal_spl_spin_lock_create();
#endif

        drv_nic_cntr_clear(unit);

        rtl8168_exit_oob();

        rtl8168_hw_init();

        rtl8168_hw_reset();

        rtl8168_get_mac_address();

        rtl8168_tally_counter_clear();

        RT_ERR_HDL(rtl8168_open(), err_alloc, rc);

        RTL_W32(RxConfig, 0x2CF3F);
        RTL_W32(MTPS, 0x27);
        ioal_mem32_write(unit, RTL9310_MAC_FORCE_MODE_CTRL_ADDR(HWP_CPU_MACID(unit)), 0x5BFF);
        ioal_mem32_field_write(unit, RTL9310_MAC_L2_CPU_MAX_LEN_CTRL_ADDR, RTL9310_MAC_L2_CPU_MAX_LEN_CTRL_CPU_PORT_RX_MAX_LEN_OFFSET, RTL9310_MAC_L2_CPU_MAX_LEN_CTRL_CPU_PORT_RX_MAX_LEN_MASK, _nic_init_conf.jumbo_size + NIC_9310_CPU_TAG_SIZE);
        ioal_mem32_field_write(unit, RTL9310_MAC_L2_CPU_MAX_LEN_CTRL_ADDR, RTL9310_MAC_L2_CPU_MAX_LEN_CTRL_CPU_PORT_TX_MAX_LEN_OFFSET, RTL9310_MAC_L2_CPU_MAX_LEN_CTRL_CPU_PORT_TX_MAX_LEN_MASK, _nic_init_conf.jumbo_size + NIC_9310_CPU_TAG_SIZE);
        ioal_mem32_field_write(unit, RTL9310_VLAN_PORT_IGR_FLTR_ADDR(HWP_CPU_MACID(unit)), RTL9310_VLAN_PORT_IGR_FLTR_IGR_FLTR_ACT_OFFSET(HWP_CPU_MACID(unit)), RTL9310_VLAN_PORT_IGR_FLTR_IGR_FLTR_ACT_MASK(HWP_CPU_MACID(unit)), 0);

        RT_ERR_HDL(nic_rx_thread_init(unit), err_alloc, rc);

        nic_init = INIT_COMPLETED;

        return rc;

err_alloc:
    if (NULL != tp)
        osal_free(tp);

    return rc;
}

/* Function Name:
 *      drv_nic_exit
 * Description:
 *      Release resources holded by NIC module.
 * Input:
 *      unit        - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.5]
 *          New added function.
 */
int32
drv_nic_exit(uint32 unit)
{
    int32 ret = RT_ERR_FAILED;

    ret = nic_rx_thread_destroy(unit);
    rtl8168_close();
    if (NULL != tp)
        osal_free(tp);

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    osal_spl_spin_lock_destroy(spl_lock);
#endif

    nic_init = INIT_NOT_COMPLETED;

    return ret;
}

int32 drv_nic_isr_handler(void *dev_instance)
{
    int status;
    int handled = 0;
    uint32  unit = ((isr_param_t *)dev_instance)->unit;


    NIC_RESET_LOCK(unit);

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    RTL_W16(IntrMask, 0);
#endif

    status = RTL_R16(IntrStatus);
    if (status & (RxOK))
    {
        RTL_W16(IntrStatus, RxOK);
        rtl8168_rx_interrupt(unit);
    }
    if (status & TxOK)
    {
        RTL_W16(IntrStatus, TxOK);
        rtl8168_tx_interrupt(unit);
    }
    if (status & RxDescUnavail)
    {
        RTL_W16(IntrStatus, RxDescUnavail);
        rtl8168_rx_runout_interrupt(unit);
    }

    RTL_W16(IntrMask, RxDescUnavail | TxOK | RxOK);

    NIC_RESET_UNLOCK(unit);

    return handled;
}

int32
_nic_pkt_tx(uint32 unit, drv_nic_pkt_t *pPacket, drv_nic_tx_cb_f fTxCb, void *pCookie)
{
    uint32              entry;
    uint32              len;
    uint32              opts1, opts2 = 0;
    uintptr              mapping;
    struct TxDesc       *txd;
    struct ring_info    *tx_skb;

    if (NULL == pPacket)
    {
        osal_printf("%s():%d  Error - pPacket is NULL!\n", __FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }
    if (pPacket->length == 0)
    {
        osal_printf("%s():%d  Error - pPacket length is 0!\n", __FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }


    entry   = tp->cur_tx % NUM_TX_DESC;
    txd     = tp->TxDescArray + entry;
    tx_skb  = tp->tx_skb + entry;

    if ((le32_to_cpu(txd->opts1) & DescOwn))
    {
        return RT_ERR_FAILED;
    }


    if (NULL != tx_skb->pPacket)
    {
        return RT_ERR_FAILED;
    }


    tx_skb->tx_callback = fTxCb;    /* Tx Callback function */
    tx_skb->cookie = pCookie;
    tx_skb->pPacket = pPacket;


    /* NIC Tx debug message */
    if (nic_debug_flag & DEBUG_TX_RAW_LEN_BIT)
        _nic_pkt_dump(unit, pPacket, 0);


    if (pPacket->as_txtag)
    {
        _cpu_txTag_prcess(pPacket);

        if (nic_debug_flag & DEBUG_TX_CPU_TAG_BIT)
            _nic_cpu_txTag_dump(unit, pPacket);
    }

    len = pPacket->txIncludeCRC ? pPacket->length - 4 : pPacket->length;
    opts1 = DescOwn | FirstFrag | LastFrag;
    opts1 |= len | (RingEnd * !((entry + 1) % NUM_TX_DESC));
#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
    mapping = DMA_ADDR_USR2PHY((uintptr)pPacket->data);
#else
    mapping = (uintptr)pci_map_single(tp->pci_dev, pPacket->data, len, PCI_DMA_TODEVICE);
#endif
    tp->tx_skb[entry].len = len;
    txd->addr = cpu_to_le64(mapping);
    txd->opts2 = cpu_to_le32(opts2);
    txd->opts1 = cpu_to_le32(opts1&~DescOwn);
    MEMORY_BARRIER();
    txd->opts1 = cpu_to_le32(opts1);

    /* To guarantee it's write done */
    do
    {
        uint32 chk;
        chk = (txd->opts1);
        if (chk);
    } while (0);

    tp->cur_tx = (tp->cur_tx + 1) % NUM_TX_DESC;

    MEMORY_BARRIER();

    RTL_W8(TxPoll, NPQ);    /* set polling bit */


    return RT_ERR_OK;

}

int32
drv_nic_pkt_tx(uint32 unit, drv_nic_pkt_t *pPacket, drv_nic_tx_cb_f fTxCb, void *pCookie)
{
    int32 ret = RT_ERR_FAILED;
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    unsigned long   spl_flags;
#endif


    /* Check init state */
    RT_INIT_CHK(nic_init);

    /* Check arguments */
    RT_PARAM_CHK(NULL == pPacket, RT_ERR_NULL_POINTER);

    /* Dispatch */
    NIC_LOCK(unit, &spl_flags);

    ret = _nic_pkt_tx(unit, pPacket, fTxCb, pCookie);

    if (RT_ERR_OK == ret)
        stats.nic_tx_success_cntr++;
    else
        stats.tx_fail++;

    NIC_UNLOCK(unit, &spl_flags);

    return ret;
}

int32
drv_nic_rx_start(uint32 unit)
{
    int32 ret = RT_ERR_FAILED;
    uint8 cmd;
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    unsigned long   spl_flags;
#endif

    RT_INIT_CHK(nic_init);

    NIC_LOCK(unit, &spl_flags);

    ret = nic_rx_thread_create(unit);

    cmd = RTL_R8(ChipCmd);
    cmd |= CmdRxEnb;
    RTL_W8(ChipCmd, cmd);

    NIC_UNLOCK(unit, &spl_flags);

    return ret;
}

int32
drv_nic_rx_stop(uint32 unit)
{
    uint8 cmd;
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    unsigned long   spl_flags;
#endif

    RT_INIT_CHK(nic_init);

    NIC_LOCK(unit, &spl_flags);

    cmd = RTL_R8(ChipCmd);
    cmd &= ~(uint8)CmdRxEnb;
    RTL_W8(ChipCmd, cmd);

    NIC_UNLOCK(unit, &spl_flags);

    return RT_ERR_OK;
}

int32
drv_nic_rx_status_get(uint32 unit, uint32 *pStatus)
{
    RT_INIT_CHK(nic_init);
    *pStatus = (RTL_R8(ChipCmd) >> 3) & 0x1;

    return RT_ERR_OK;
}

int32
drv_nic_rx_register(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb, void *pCookie, uint32 flags)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    unsigned long   spl_flags;
#endif
    /* Check init state */
    RT_INIT_CHK(nic_init);

    /* Check arguments */
    RT_PARAM_CHK(priority > NIC_RX_CB_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == fRxCb, RT_ERR_NULL_POINTER);

    NIC_LOCK(unit, &spl_flags);

    if (NIC_FLAG_RX_CRC_INCLUDE & flags)
        rxCRCInclude = 1;

    if (NULL == _nic_rx_cb_tbl[priority].rx_callback)
    {
        _nic_rx_cb_tbl[priority].rx_callback = fRxCb;
        _nic_rx_cb_tbl[priority].pCookie     = pCookie;
        _nic_rx_intr_cb_cnt++;
    }
    else
    {
        /* Handler is already existing */
        NIC_UNLOCK(unit, &spl_flags);
        return RT_ERR_FAILED;
    }

    NIC_UNLOCK(unit, &spl_flags);

    return RT_ERR_OK;
}

int32
drv_nic_rx_unregister(uint32 unit, uint8 priority, drv_nic_rx_cb_f fRxCb)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    unsigned long   spl_flags;
#endif
    /* Check init state */
    RT_INIT_CHK(nic_init);

    /* Check arguments */
    RT_PARAM_CHK(priority > NIC_RX_CB_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == fRxCb, RT_ERR_NULL_POINTER);

    NIC_LOCK(unit, &spl_flags);

    if (_nic_rx_cb_tbl[priority].rx_callback == fRxCb)
    {
        _nic_rx_cb_tbl[priority].rx_callback = NULL;
        _nic_rx_cb_tbl[priority].pCookie     = NULL;
        _nic_rx_intr_cb_cnt--;
    }
    else
    {
        /* Handler is nonexistent */
        NIC_UNLOCK(unit, &spl_flags);
        return RT_ERR_FAILED;
    }

    NIC_UNLOCK(unit, &spl_flags);

    return RT_ERR_OK;
}

int32
drv_nic_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket)
{
    int32 ret = RT_ERR_FAILED;

    /* Check init state */
    RT_INIT_CHK(nic_init);

    /* Check arguments */
    RT_PARAM_CHK(NULL == _nic_init_conf.pkt_alloc, RT_ERR_NULL_POINTER);

    /* Dispatch */
    ret = _nic_init_conf.pkt_alloc(unit, size, flags, ppPacket);

    return ret;
}

int32
drv_nic_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket)
{
    int32 ret = RT_ERR_FAILED;


    /* Check init state */
    RT_INIT_CHK(nic_init);

    /* Check arguments */
    RT_PARAM_CHK(NULL == pPacket, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == _nic_init_conf.pkt_free, RT_ERR_NULL_POINTER);

    /* Dispatch */
    ret = _nic_init_conf.pkt_free(unit, pPacket);

    return ret;
}

int32
drv_nic_reset(uint32 unit)
{
    int32   ret;
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    unsigned long   spl_flags;
#endif

    if (nic_init != INIT_COMPLETED)
        return RT_ERR_FAILED;

    NIC_RESET_LOCK(unit);
    NIC_LOCK(unit, &spl_flags);

    rtl8168_close();
    ret = rtl8168_open();

    NIC_UNLOCK(unit, &spl_flags);
    NIC_RESET_UNLOCK(unit);

    return ret;
}

int32
drv_nic_dbg_get(uint32 unit, uint32 *pFlags)
{
    /* Check init state */
    RT_INIT_CHK(nic_init);

    *pFlags = nic_debug_flag;

    return RT_ERR_OK;
}

int32
drv_nic_dbg_set(uint32 unit, uint32 flags)
{
    /* Check init state */
    RT_INIT_CHK(nic_init);

    nic_debug_flag = flags;

    return RT_ERR_OK;
}

int32
drv_nic_cntr_dump(uint32 unit)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    unsigned long   spl_flags;
#endif
    /* Check init state */
    RT_INIT_CHK(nic_init);

    NIC_LOCK(unit, &spl_flags);

    osal_printf("Tx success counter         : %10llu \n", stats.nic_tx_success_cntr);
    osal_printf("Rx success counter         : %10llu \n", stats.nic_rx_success_cntr);
    osal_printf("Tx fail counter            : %10u \n",   stats.tx_fail);
    osal_printf("Rx error counter           : %10llu \n", stats.rx_errors);
    osal_printf("Rx lack buffer counter     : %10llu \n", stats.rx_lack_buf_cntr);
    osal_printf("intr_runout_alloc_fail_cnt : %10llu \n", intr_runout_alloc_fail_cnt);
    osal_printf("jumbo_rx_alloc_cnt         : %10llu \n", jumbo_rx_alloc_cnt);
    osal_printf("jumbo_tx_alloc_cnt         : %10llu \n", jumbo_tx_alloc_cnt);
    osal_printf("jumbo_free_cnt             : %10llu \n", jumbo_free_cnt);
    osal_printf("nml_rx_alloc_cnt           : %10llu \n", nml_rx_alloc_cnt);
    osal_printf("nml_tx_alloc_cnt           : %10llu \n", nml_tx_alloc_cnt);
    osal_printf("nml_free_cnt               : %10llu    (free_fail_cnt:%d)\n", nml_free_cnt, free_fail_cnt);
    osal_printf("   rtl8168_free_cnt        : %10llu \n", rtl8168_free_cnt);
    osal_printf("   nic_end_free_cnt        : %10llu \n", nic_end_free_cnt);

#if 0 /* TODO_sync37x: need to implement nic_rx */
    if (nic_debug_flag & DEBUG_ADV_CNTR_BIT)
        nic_rx_cntr_dump(unit);
#endif
    proc_get_tally_counter();

    NIC_UNLOCK(unit, &spl_flags);

    return RT_ERR_OK;
}

int32
drv_nic_cntr_clear(uint32 unit)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    unsigned long   spl_flags;
#endif
    /* Check init state */
    RT_INIT_CHK(nic_init);

    NIC_LOCK(unit, &spl_flags);

    osal_memset(&stats, 0, sizeof(stats));
    intr_runout_alloc_fail_cnt = 0;
    jumbo_rx_alloc_cnt  = 0;
    jumbo_tx_alloc_cnt  = 0;
    jumbo_free_cnt      = 0;
    nml_rx_alloc_cnt    = 0;
    nml_tx_alloc_cnt    = 0;
    nml_free_cnt        = 0;
    free_fail_cnt       = 0;
    rtl8168_free_cnt    = 0;
    nic_end_free_cnt    = 0;

#if 0 /* TODO_sync37x: need to implement nic_rx */
    nic_rx_cntr_clear(unit);
#endif
    rtl8168_tally_counter_clear();

    NIC_UNLOCK(unit, &spl_flags);

    return RT_ERR_OK;
}

int32
drv_nic_ringbuf_dump(uint32 unit, nic_dir_t direct)
{
    uint32  i;
    struct RxDesc       *rxd;
    struct TxDesc       *txd;

    if (direct == NIC_DIR_RX)
    {
        osal_printf("RX ring:    tp->cur_rx:%d\n", tp->cur_rx);
        for (i = 0; i < NUM_RX_DESC; i++)
        {
            rxd = tp->RxDescArray + i;
            osal_printf("entry_%d: %p\n", i, rxd);
            osal_printf("opts1: %#x    OWN:%s\n", rxd->opts1, (rxd->opts1 >> 31) ? "1" : "CPU");
            //osal_printf("opts2: %#x\n", rxd->opts2);
            osal_printf("addr: %#x    packet:%p\n", (uint32)(rxd->addr & 0xffffffff), tp->Rx_skbuff[i]);
            osal_printf("\n");
        }
    }

    if (direct == NIC_DIR_TX)
    {
        osal_printf("\nTX ring:    tp->cur_tx:%d\n", tp->cur_tx);
        for (i = 0; i < NUM_TX_DESC; i++)
        {
            txd = tp->TxDescArray + i;
            osal_printf("entry_%d: %p\n", i, txd);
            osal_printf("opts1: %#x\n", txd->opts1);
            //osal_printf("opts2: %#x\n", txd->opts2);
            osal_printf("addr: %#x\n", (uint32)(txd->addr & 0xffffffff));
            osal_printf("\n");
        }
    }

    return RT_ERR_OK;
}

int32
drv_nic_reg_get(uint32 unit, uint32 type, uint32 addr, uint32 *pVal)
{
    RT_PARAM_CHK(NULL == pVal, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(type == 0 && addr > 0xfc, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(type == 1 && addr > 0xfe, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(type == 2 && addr > 0xff, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(type > 2 , RT_ERR_OUT_OF_RANGE);

    if (type == 0)
        *pVal = RTL_R32(addr);
    else if (type == 1)
        *pVal = RTL_R16(addr);
    else
        *pVal = RTL_R8(addr);
    return RT_ERR_OK;
}

int32
drv_nic_reg_set(uint32 unit, uint32 type, uint32 addr, uint32 val)
{
    RT_PARAM_CHK(type == 0 && addr > 0xfc, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(type == 1 && addr > 0xfe, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(type == 2 && addr > 0xff, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(type > 2 , RT_ERR_OUT_OF_RANGE);

    if (type == 0)
        RTL_W32(addr, val);
    else if (type == 1)
        RTL_W16(addr, val);
    else
        RTL_W8(addr, val);
    return RT_ERR_OK;
}

