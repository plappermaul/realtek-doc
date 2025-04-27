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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition those public NIC(Network Interface Controller) APIs and
 *           its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) CPU tag
 *           2) NIC tx
 *           3) NIC rx
 *
 */

#ifndef __DRV_NIC_COMMON_H__
#define __DRV_NIC_COMMON_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/type.h>

/*
 * Symbol Definition
 */
#define NIC_RX_CB_PRIORITY_NUMBER       (8)
#define NIC_RX_CB_PRIORITY_MIN          (0)
#define NIC_RX_CB_PRIORITY_MAX          (NIC_RX_CB_PRIORITY_NUMBER - 1)


typedef enum drv_nic_rx_reason_e
{
    NIC_RX_REASON_DUMMY = 0,
    NIC_RX_REASON_OAM,
    NIC_RX_REASON_CFM,
    NIC_RX_REASON_CFM_ETHDM,
    NIC_RX_REASON_PARSE_EXCPT,          /* Parser exception (inner/outer frame) */
    NIC_RX_REASON_MALFORM,              /* Malformed packet */
    NIC_RX_REASON_IGR_VLAN_FILTER,      /* VLAN ingress filter */
    NIC_RX_REASON_VLAN_ERROR,           /* VLAN error(VID=4095 or MBR=0x0) */
    NIC_RX_REASON_IVC,                  /* IVC (MAC-based/IP-subnet-based) */
    NIC_RX_REASON_INNER_OUTTER_CFI,     /* Inner/Outer tag CFI/DEI = 1 */
    NIC_RX_REASON_RMA_USR_DEF,         /* RMA User-defined */
    NIC_RX_REASON_RMA_USR_DEF0,         /* RMA User-defined 0 */
    NIC_RX_REASON_RMA_USR_DEF1,         /* RMA User-defined 1 */
    NIC_RX_REASON_RMA_USR_DEF2,         /* RMA User-defined 2 */
    NIC_RX_REASON_RMA_USR_DEF3,         /* RMA User-defined 3 */
    NIC_RX_REASON_RMA_BPDU,             /* RMA BPDU 01-80-C2-00-00-00 */
    NIC_RX_REASON_RMA_LACP,             /* RMA LACP 01-80-C2-00-00-02 */
    NIC_RX_REASON_RMA_PTP,              /* RMA PTP packet, 0x88F7 */
    NIC_RX_REASON_RMA_LLDP,             /* RMA LLDP packet, 0x88CC + 01-80-C2-00-00-0E */
    NIC_RX_REASON_RMA,                  /* RMA */
    NIC_RX_REASON_RLDP_RLPP,            /* RLDP or RLPP packet */
    NIC_RX_REASON_IP4_HDR_ERROR,        /* IP4 header error */
    NIC_RX_REASON_IP4_OPTIONS,          /* IPv4 options */
    NIC_RX_REASON_IP6_HDR_ERROR,        /* IPv6 header error */
    NIC_RX_REASON_IP4_6_HDR_ERROR,      /* IPv4/6 header error */
    NIC_RX_REASON_IP6_HOPBYHOP_EXT_HDR_ERROR,   /* IPv6 Hop-by-Hop Extension Header Position Error */
    NIC_RX_REASON_IP6_HOPBYHOP_OPTION,  /* IPv6 Hop-by-Hop option */
    NIC_RX_REASON_IP6_UNKWN_EXT_HDR,    /* IPv6 Unknown Extension Header */
    NIC_RX_REASON_TTL_EXCEED,           /* TTL exceed */
    NIC_RX_REASON_HOP_EXCEED,           /* Hop Limit exceed */
    NIC_RX_REASON_ROUTING_EXCEPTION,    /* Routing exception (IPv4 header error/IPv4 TTL exceed/IPv4 options/
                                           IPv6 Hop Limit exceed/IPv6 Hop-by-Hop option) or Next-hop entry age out */
    NIC_RX_REASON_ROUT_EXCPT_NOT_NH,    /* Routing exception (NOT a Next-Hop Entry) (tunnel/route) */
    NIC_RX_REASON_ROUT_EXCPT_NH_AGE,    /* Routing exception (Next-Hop Entry Age-out) */
    NIC_RX_REASON_ROUT_EXCPT_NH_ROUTE_TO_TUNL,  /* Routing exception (NH is a tunnel interface while Post Routing) */
    NIC_RX_REASON_GW_MAC_ERROR,         /* GW MAC Error/NextHop Age Out */
    NIC_RX_REASON_SEC_IP_MAC_BIND,      /* IP MAC binding (e.g. arp binding Match/Mismatch) */
    NIC_RX_REASON_IPUC_RPF,             /* L3 IPUC RPF (tunnel/route) */
    NIC_RX_REASON_TUNL_TMNT,            /* Tunnel terminate */
    NIC_RX_REASON_TUNL_IF_MAC,          /* Tunnel Interface MAC */
    NIC_RX_REASON_TUNL_IP_CHK,          /* Tunnel IP addr check */
    NIC_RX_REASON_ROUTER_IF_MAC,        /* Router Interface MAC */
    NIC_RX_REASON_L3UC_NON_IP,          /* NON-IP packets matched ingress router MAC with action configured as L3 process*/
    NIC_RX_REASON_ROUTE_IP_CHK,         /* Routing IP addr check */
    NIC_RX_REASON_L3_IP_MAC_MISMATCH,   /* L3 routing DIP/DMAC mismatch (e.g. multicast IP/MAC addr format) */
    NIC_RX_REASON_IP6_ROUTE_HDR,        /* IP6 UC/MC routing header */
    NIC_RX_REASON_IP4_6_ROUTE_LUMIS,    /* IP4/IP6 MC routing lookup miss */
    NIC_RX_REASON_IPUC_NULL_ROUTE,      /* L3 IPUC null route */
    NIC_RX_REASON_IPUC_PBR_NULL_ROUTE,  /* L3 IPUC PBR null route */
    NIC_RX_REASON_IPUC_HOST_ROUTE,      /* L3 IPUC host route (net route bit == 0) (My IP) */
    NIC_RX_REASON_IPUC_NET_ROUTE,       /* L3 IPUC net route (net route bit == 1) (for sending ARP) */
    NIC_RX_REASON_IPMC_BDG_ENTRY,       /* L3 IPMC bridge entry */
    NIC_RX_REASON_IPMC_ROUTE_ENTRY,     /* L3 IPMC route entry */
    NIC_RX_REASON_IPMC_ASSERT,          /* L3 IPMC assert */
    NIC_RX_REASON_IP4_6_ICMP_REDIR,     /* IP4/IP6 ICMP redirect */
    NIC_RX_REASON_IPUC_MTU,             /* IPUC MTU (tunnel/route) */
    NIC_RX_REASON_IPMC_MTU,             /* IPMC MTU (tunnel/route) */
    NIC_RX_REASON_IPUC_TTL,             /* IPUC TTL (tunnel/route) */
    NIC_RX_REASON_IPMC_TTL,             /* IPMC TTL (tunnel/route) */
    NIC_RX_REASON_IP4_6_RSVD_ADDR,      /* IP4/IP6 reserved address */
    NIC_RX_REASON_MPLS_EXCPT,           /* MPLS exception */
    NIC_RX_REASON_IGMP,                 /* IGMP */
    NIC_RX_REASON_MLD,                  /* MLD */
    NIC_RX_REASON_IGMP_MLD,             /* IGMP/MLD */
    NIC_RX_REASON_DHCP_DHCP6,           /* DHCP/DHCP6 */
    NIC_RX_REASON_EAPOL,                /* EAPOL */
    NIC_RX_REASON_SPECIAL_TRAP,         /* Special trap (IGMP/MLD/EAPOL) */
    NIC_RX_REASON_ARP_REQ,              /* ARP Request */
    NIC_RX_REASON_ARP_REQ_REP_GRA,      /* ARP Request/Reply/Gratuitous ARP */
    NIC_RX_REASON_IP6_NEIGHBOR_DISCOVER,    /* IPv6 header error */
    NIC_RX_REASON_NEIGHBOR_DISCOVER,	/* IP4/6 Neighbor Discovery */
    NIC_RX_REASON_SPECIAL_COPY,         /* Special copy (ARP/IPV6 Neighbor Discovery) */
    NIC_RX_REASON_UNKWN_UCST,           /* Unknown unicast trap */
    NIC_RX_REASON_UNKWN_L2_MCST,        /* Unknown L2 multicast trap */
    NIC_RX_REASON_UNKWN_IP_MCST,        /* Unknown IP multicast trap */
    NIC_RX_REASON_UNKWN_IP6_MCST,       /* Unknown IPv6 multicast trap */
    NIC_RX_REASON_UNKWN_BCST,           /* Unknown broadcast trap */
    NIC_RX_REASON_UNKWN_UCST_MCST,      /* Unknown unicast/multicast trap */
    NIC_RX_REASON_MY_MAC,               /* CPU MAC */
    NIC_RX_REASON_L2_UC_MC_LUMIS,       /* L2 UC/MC bridge lookup miss */
    NIC_RX_REASON_IP4_6_MC_LUMIS,       /* IP4/IP6 MC bridge lookup miss */
    NIC_RX_REASON_INVALID_SA,           /* Invalid SA */
    NIC_RX_REASON_MAC_CONSTRAINT_SYS,   /* MAC Constraint (learn limit) system-based */
    NIC_RX_REASON_MAC_CONSTRAINT_VLAN,  /* MAC Constraint (learn limit) vlan-based */
    NIC_RX_REASON_MAC_CONSTRAINT_PORT,  /* MAC Constraint (learn limit) port-based */
    NIC_RX_REASON_MAC_CONSTRAINT,       /* MAC Constraint (learn limit) */
    NIC_RX_REASON_NEW_SA,               /* New Source MAC Address */
    NIC_RX_REASON_STC_L2_PMV,           /* Static L2 entry port moving */
    NIC_RX_REASON_DYN_L2_PMV,           /* Dynamic L2 entry port moving */
    NIC_RX_REASON_L2_PMV_FBD,           /* Port move forbiden */
    NIC_RX_REASON_L2_PMV,               /* L2 entry port moving */
    NIC_RX_REASON_L2_HASH_FULL,         /* L2 hash full */
    NIC_RX_REASON_ACL_HIT,              /* ACL hit */
    NIC_RX_REASON_OF_HIT,               /* OpenFlow hit */
    NIC_RX_REASON_ATTACK,               /* Attack prevention has detected this attack */
    NIC_RX_REASON_MIRROR,               /* Mirror */
    NIC_RX_REASON_SFLOW_RX,             /* Rx SFLOW Sampling*/
    NIC_RX_REASON_SFLOW_TX,             /* Tx SFLOW Sampling*/
    NIC_RX_REASON_CRC_ERROR,            /* CRC error */
    NIC_RX_REASON_L3_CHKSUM_ERROR,      /* L3 checksum error */
    NIC_RX_REASON_RRCP,                 /* RRCP */
    NIC_RX_REASON_OF_TTL_EXCEED,        /* OF IP/MPLS TTL exceed */
    NIC_RX_REASON_OF_TBL_LUMIS,         /* OF table lookup miss */
    NIC_RX_REASON_OF_INVLD_CAPWAP_HDR,  /* Invalid CAPWAP header */
    NIC_RX_REASON_OF_CAPWAP_CTRL,       /* CAPWAP control packet */
    NIC_RX_REASON_CPU2CPU,              /* CPU to CPU */
    NIC_RX_REASON_L2_NTFY,              /* L2 Notification */
    NIC_RX_REASON_ECID_EQ_PCID,         /* ECID equal to Rx Port PCID */

    NIC_RX_REASON_NORMAL_FWD,           /* Normal forwarding (flooding or L2 entry hit) */
    NIC_RX_REASON_MAX,
} drv_nic_rx_reason_t;

typedef enum drv_nic_fwd_type_e
{
    NIC_FWD_TYPE_ALE,
    NIC_FWD_TYPE_LOGICAL,
    NIC_FWD_TYPE_PHYISCAL,
    NIC_FWD_TYPE_TRUNK,
    NIC_FWD_TYPE_LOGICAL_ONE_HOP,
    NIC_FWD_TYPE_PHYISCAL_ONE_HOP,
    NIC_FWD_TYPE_UCST_CPU_MIN_PORT,
    NIC_FWD_TYPE_UCST_CPU,
    NIC_FWD_TYPE_BCST_CPU,
    NIC_FWD_TYPE_END,
} drv_nic_fwd_type_t;

#define DRV_NIC_TOTAL_NUM_OF_WORD_FOR_1BIT_REASON_MASK    ((NIC_RX_REASON_MAX+31)/32)

typedef struct drv_nic_pkt_s
{
    uint8  *head;       /* pointer to the head of the packet data buffer */
    uint8  *data;       /* pointer to the base address of the packet */
    uint8  *tail;       /* pointer to the end address of the packet */
    uint8  *end;        /* pointer to the end of the packet data buffer */
    uint32 length;      /* packet length when the packet is valid (not a empty data buffer) */
    void   *buf_id;     /* pointer to the user-defined packet descriptor */

    uint8  as_txtag:1;    /* 0: without tx-tag, 1: with tx-tag */
    uint8  txIncludeCRC:1;    /* Whether the packet transmited from higher layer includes CRC (packets from both Linux kernel and Realtek turnkey protocol are not including CRC) */

    uint32  reason_bitmask[DRV_NIC_TOTAL_NUM_OF_WORD_FOR_1BIT_REASON_MASK]; /* The trap reasons in this member are common to all chip */


    /* Reception information */
    struct
    {
        uint8   source_port;        /* Source port number */
        uint8   reason;             /* Reason to CPU */
        uint16  outer_pri:3;        /* priority of outer tag */
        uint16  outer_vid:12;       /* vid of outer tag */
        uint16  inner_pri:3;        /* priority of inner tag */
        uint16  inner_vid:12;       /* vid of inner tag */
        uint8   svid_tagged:1;      /* Whether the SVID is tagged */
        uint8   cvid_tagged:1;      /* Whether the CVID is tagged (ethertype==0x8100) */
        uint16  acl_index;          /* ACL index */
        uint8   qid;                /* Mapping to Tx Queue ID */
        uint16  mirror_hit:4;       /* Mirrioring hit status for 4-sets of configuration */
        uint16  fvid:12;            /* Forwarding VLAN ID */
        uint8   oversize:1;         /* packet has truncated by NIC */
        uint8   atk_type:5;         /* Indicate which attack type this packet belong to */
        uint8   ext_is_trk:1;       /* whether ext_trk_id represents effectively*/
        uint8   ext_trk_id;         /* The extended trunk ID */
                                    /* The original trunk ID if this received packet move to new port */
        uint8   ext_dev_id;           /* The extended device ID */
                                    /* The original switch device id if port move */
                                    /* The TX/RX sflow sampling device ID */
        uint8   ext_source_port;    /* The extended source port */
                                    /* The extended source port if this received packet move to new port */
                                    /* The TX/RX sflow sampling unit ID */
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
        uint8   dev_id;               /* switch device id */
        uint8   pmv_forbid:1;       /* Port move forbiden */
        uint8   is_trk:1;           /* whether trk_id represents effectively */
        uint8   fvid_sel:1;         /* Indicates where Forwarding VLAN ID comes from */
        uint8   trk_id;             /* trunk Id, if is_trk = 1 */
        uint8   physical_rx_port;   /* Physical port number that Rx packet. In stacking system, this field indicates which stacking
                                       port packet is received if packet is received in another unit */
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
        uint16  dm_rxIdx:6;         /* ETH-DM RxTimestamp Index (0~63) */
#endif
#if defined(CONFIG_SDK_RTL9310)
        uint8   of_lumis_tblId;     /* The table ID when OpenFlow table lookup miss */
        uint8   of_lb_times;        /* The loopback times when OpenFlow table lookup miss */
        uint8   of_tblId;           /* OpenFlow hit table ID */
        uint16  of_index;           /* OpenFlow index */
        uint16  tt_idx;             /* Tunnel Terminate entry index */
#endif
    } __attribute__ ((aligned(1), packed)) rx_tag;

    /* Transmission information */
    struct
    {
        drv_nic_fwd_type_t  fwd_type:4; /* Forward type */
        uint16  as_priority:1;      /* Assign priority and drop precedence directly. */
        uint16  priority:5;         /* Assigned priority */
        uint16  acl_act:1;          /* If ACL_ACT is 0b1, then take effect of ingress/egress ACL */
        uint16  as_tagSts:1;        /* Decide whether proceed Outter/Inner TAG by ALE */
        uint16  dm_pkt:1;           /* ETH-DM packet indicate */
        uint16  dg_pkt:1;           /* Dying Gasp packet */
        uint16  fvid_sel:1;         /* Indicates where Forwarding VLAN ID comes from */
        uint16  cngst_drop:1;       /* Congestion Dropable */
        uint32  dst_port_mask;      /* DPM: When ADSPM is 1, DPM is the packet destination port mask. port 0~31 */
        uint32  dst_port_mask_1;    /* packet destination port, port 32~63 */

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
        uint8   l2_learning:1;      /* Decide whether proceed L2 learning */
        uint8   bp_fltr1:1;         /* If BP_FLTR_1 is 0b1, then bypass the following filtering: (1)Egress OAM mux (2)Egress Port Isolation (3)Egress Mirror Isolation (4)WRED */
        uint8   bp_fltr2:1;         /* If BP_FLTR_2 is 0b1, then bypass the following checking: (1) Egress spanning-tree port state in blocking/listening or learning (2)Egress VLAN filtering */
#endif

#if defined(CONFIG_SDK_RTL8390)
        uint8   ptp_tx_log:1;       /* PTP log function */
        uint8   ptp_tx_offload:2;   /* PTP offload mechanism for one-step */
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
        uint8   dev_id;              /*device id*/
        uint32  bp_fltr:1;          /* Bypass the following filtering: (1)Egress OAM mux (2)Egress Port Isolation (3)Egress Mirror Isolation */
        uint32  bp_stp:1;           /* Bypass egress STP */
        uint32  bp_vlan_egr:1;      /* Bypass egress VLAN filtering */
        uint32  l3_act:1;           /* If L3_ACT is 0b1, then take effect of L3 routing and multicast replication:*/
        uint32  ori_tagif_en:1;     /* Indicate whether the packet's tag status is parsed from ingress port's view*/
        uint32  fvid_en:1;          /* 0: follow ALE decision, 1: force use FVID*/
        uint16  fvid:12;            /* Forwarding VID*/
        uint32  src_filter_en:1;    /* If SRC_FILTER_EN is 0b1, SP should be filtered from outgoing portmask*/
        uint32  sp_is_trk:1;        /* 0: SPN is not trunk ID,1: SPN is trunk ID*/
        uint32  spn:10;             /* The port ID to be filtered from outgoing portmask.*/
        uint32  ori_itagif:1;       /* Original inner tagif */
        uint32  ori_otagif:1;       /* Original outer tagif */
        uint32  master:1;           /* MASTER=0: cpu tag is for Slave,MASTER=1: cpu tag is for Master.*/
#endif
    } __attribute__ ((aligned(1), packed)) tx_tag;

    struct drv_nic_pkt_s *next;         /* pointer to next packet struct if it exist */
} drv_nic_pkt_t;

typedef enum drv_nic_rx_e
{
    NIC_RX_NOT_HANDLED = 0,     /* The packet is not handled, continue processing */
    NIC_RX_HANDLED,             /* The packet is and caller has taken control of the packet data */
    NIC_RX_HANDLED_OWNED,       /* The packet is handled and processing is complete */
} drv_nic_rx_t;

typedef int32 (*drv_nic_pkt_alloc_f)(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket);
typedef int32 (*drv_nic_pkt_free_f)(uint32 unit, drv_nic_pkt_t *pPacket);
typedef drv_nic_rx_t (*drv_nic_rx_cb_f)(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie);
typedef void (*drv_nic_tx_cb_f)(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie);

typedef struct drv_nic_initCfg_s
{
    int32               pkt_size;       /* Maximum bytes to support in a normal packet */
    int32               jumbo_size;       /* Maximum bytes to support in a jumbo packet */
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
    drv_nic_pkt_alloc_f rx_pkt_alloc;   /* RX Packet structure and packet data allocation routine */
#endif
    drv_nic_pkt_alloc_f pkt_alloc;      /* TX Packet structure and packet data allocation routine */
    drv_nic_pkt_free_f  pkt_free;       /* Packet structure and packet data deallocation routine */
#if defined(CONFIG_SDK_DRIVER_PCI)
    struct pci_dev      *pci_dev;
#endif
    struct device       *dev;
} drv_nic_initCfg_t;


/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

#define NIC_REASON_MASK_SET(packet, reason) \
do{ \
    (packet).reason_bitmask[(reason)/32] |= (1 << ((reason)%32)); \
}while(0)

#define IS_NIC_REASON_MASK_SET(packet, reason) \
    (((reason) < NIC_RX_REASON_MAX) ? ((packet).reason_bitmask[(reason)/32] >> ((reason)%32)) & 1 : 0)

#endif /* __DRV_NIC_COMMON_H__ */

