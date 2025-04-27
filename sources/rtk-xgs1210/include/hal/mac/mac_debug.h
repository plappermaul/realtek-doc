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
 * $Revision: 98467 $
 * $Date: 2019-07-09 10:06:55 +0800 (Tue, 09 Jul 2019) $
 *
 * Purpose : register service APIs in the SDK.
 *
 * Feature : register service APIs
 *
 */

#ifndef __HAL_MAC_DEBUG_H__
#define __HAL_MAC_DEBUG_H__

#include <common/rt_autoconf.h>
#include <rtk/port.h>
#if defined(CONFIG_SDK_RTL9300)
#include <dal/longan/dal_longan_sds.h>
#endif

struct hsb_param_s
{
    union
    {
        struct
        {
            uint32 page_cnt:6;
            uint32 endsc:12;            /* End page point */
            uint32 bgsc:12;             /* Start page point */
            uint32 field_seltor11:16;   /* Non IPv6 : User defined 16-bit field IPv6 : SIP[127:112] */
            uint32 field_seltor10:16;   /* Non IPv6 : User defined 16-bit field IPv6 : SIP[111:96] */
            uint32 field_seltor9:16;    /* Non IPv6 : User defined 16-bit field IPv6 : SIP[95:80] */
            uint32 field_seltor8:16;    /* Non IPv6 : User defined 16-bit field IPv6 : SIP[79:64] */
            uint32 field_seltor7:16;    /* Non IPv6 : User defined 16-bit field IPv6 : SIP[63:48] */
            uint32 field_seltor6:16;    /* Non IPv6 : User defined 16-bit field IPv6 : SIP[47:32] */
            uint32 field_seltor5:16;    /* User defined 16-bit field */
            uint32 field_seltor4:16;    /* User defined 16-bit field, CPUTAG[79:64] */
            uint32 field_seltor3:16;    /* User defined 16-bit field, CPUTAG[63:48] */
            uint32 field_seltor2:16;    /* User defined 16-bit field, CPUTAG[47:32] */
            uint32 field_seltor1:16;    /* User defined 16-bit field, shared for CPUTAG[31:16] */
            uint32 field_seltor0:16;    /* User defined 16-bit field, shared for CPUTAG[15:0] */
            uint32 field_sel_vld:12;    /* Indicates which field_selector is valid */
            uint32 impls_exp:3;         /* Inner MPLS EXP */
            uint32 impls_label:20;      /* Inner MPLS label */
            uint32 ompls_exp:3;         /* Outer MPLS EXP */
            uint32 ompls_label:20;      /* Outer MPLS label */
            uint32 arpopcode:16;        /* ARP/RARP : Operation field. IPv6 : Extented header length */
            uint8  target_mac[6];       /* ARP/RARP :Target MAC of ARP/RARP packet, for ACL. IPV6 : DIP[127:80] */
            uint8  sender_mac[6];       /* ARP/RARP: Sender MAC of ARP/RARP packet, for ACL. IPV6 : DIP[79:32] */
            uint32 dport:16;            /* TCP/UDP destination port, ICMP code B[7:0] */
            uint32 sport:16;            /* TCP/UDP source port, ICMP type B[7:0] */
            uint32 tcp_flag:8;          /* TCP : TCP Flag. IGMP : IGMP Type field */
            uint32 ipver:4;             /* IP header version (For Ipv4 and Ipv6) */
            uint32 ipttl:8;             /* IP header - TTL field */
            uint32 ip_tos:8;            /* IP header - DSCP/TOS */
            uint32 ip_protocol:8;       /* IP header - protocol field, IPv6: Next header */
            uint32 ip_flag:3;           /* IPV4 : IP header- flag. IPV6 : IP flag in fragment header */
            uint32 ip_offset:13;        /* IPV4 : IP header- offset. IPV6 : IP offset in fragment header */
            uint32 dip;                 /* IPv4: { DIP[31:0] }, IPv6: DIP[31:0], target IP for ARP/RARP
                                            B[15:0]=MEPID of CCM packet
                                            B[23:16]=OpCode of CFM packet
                                            B[26:24]=MD Level of CFM packet
                                            B[29:27]=CFM Interval Field
                                            B[31:30]=reserved */
            uint32 sip;                 /* IPv4: {SIP[31:0] }, IPv6: SIP[31:0], source IP for ARP/RARP */
            uint32 otag:16;             /* Outer VLAN Tag, B[15:13]=user-priority,B[12]=DEI,B[11:0]=VID */
            uint32 itag:16;             /* Inner VLAN Tag, B[15:13]=user-priority,B[12]=CFI,B[11:0]=VID */
            uint32 otpid_idx:2;         /* TPID index for outer tag */
            uint32 itpid_idx:2;         /* TPID index for inner tag */
            uint32 etp:16;              /* Ethernet/Length field(Ether Type) */
            uint8  smac[6];             /* Source MAC address */
            uint8  dmac[6];             /* Destination MAC address */
            uint32 iplen:16;            /* IPv4 Total length or IPv6 payload length(For Attack prevention) */
            uint32 ipv4hdlen:4;         /* IPv4 header length */
            uint32 tcpseqzero:1;        /* TCP sequence number is zero */
            uint32 ipv6hop:1;           /* IPv6 packet with Hop-by-Hop header */
            uint32 ipv6rout:1;          /* IPv6 packet with routing header */
            uint32 ipv6frag:1;          /* IPv6 packet with fragment header */
            uint32 ipv6dest:1;          /* Ipv6 packet with destination header */
            uint32 ipv6auth:1;          /* IPv6 packet with authentication header */
            uint32 ipcsok:1;            /* IP checksum is ok */
            uint32 ipv6ext_long:1;      /* IPv6 header is too long for parser */
            uint32 ipv6:1;              /* IPv6 Type */
            uint32 ipv4:1;              /* IPv4 Type */
            uint32 ompls_if:1;          /* Has outer MPLS interface */
            uint32 impls_if:1;          /* Has inner MPLS interface */
            uint32 etag_if:1;           /* Extra Tag */
            uint32 cputag_if:1;         /* Has CPU Tag */
            uint32 otag_if:1;           /* Has Outer VLAN Tag(Tag or Priority tag) */
            uint32 itag_if:1;           /* Has Inner VLAN Tag(Tag or priority tag) */
            uint32 oampdu:1;            /* OAM control frame */
            uint32 llc_other:1;         /* LEN < 0x0600, {DSAP, SSAP, CTL, OUI}  != 0xaaaa03_000000 */
            uint32 pppoe_if:1;          /* PPPoE packet */
            uint32 rfc1042:1;           /* LLC SNAP */
            uint32 spa:6;               /* Source Port ID */
            uint32 errpkt:1;            /* Error packet(CRC,Symbol) */
            uint32 l4hdchk:1;           /* Check the non-fragment or first fragment of IP packet(TCP/UDP/ICMP) has enough length for L4 header(For attack prevention) */
            uint32 pktlen:14;           /* Packet length (Include CRC) */
        } r8390;
        struct
        {
            uint32 rtkpp:2;             /*b11:this is a rldp/rlpp pkt; b01:this is a rrcp pkt; b00:this is not a realtek protocol pkt*/
            uint32 ip_mf:1;             /*ipv4 MF flag bit*/
            uint32 ip_dstype:8;         /*ipv4/ipv6 dstype value*/
            uint32 ip_protocol:8;       /*the protocol item in ipv4/v6 header or "next header" item in extension header*/
            uint32 ip_offset:13;        /*the offset item in ipv4 header or fragment extension header*/
            uint32 ip_length:16;        /*the payload length item in ipv4/v6 header*/
            uint32 ip_ttl:8;            /*the ttl item in ipv4/v6 header*/
            uint8  dip[16];              /*ipv6 pkt;(48bit don't care,DMAC,DIP)arp pkt;(32bit don't care,12bytes content following 0x8899)rtkpp pkt;
                                                            (96bit don't care, dip)ipv4 pkt*/
            uint8  sip[16];              /*128bit sip for ipv6 pkt;(32bit don't care,OP,SMAC,SIP) arp pkt;(96bit don't care ,sip) ipv4 pkt*/
            uint32 ipv4_hdrlen:4;       /*ipv4 header length for ipv4 pkt;{DEST,FRAG,ROUT,HOP}extension header exist for ipv6 pkt*/
            uint32 ipv4_df:1;           /*the "DF" iterm in ipv4 header for ipv4 pkt; AUTH extionsion header exist for ipv6 pkt*/
            uint32 tcp_hdrlen:4;        /*tcp header length in tcp header for tcp pkt;group ip [27:24] for igmp pkt*/
            uint32 tcp_flag:8;          /*tcp flag in tcp header for tcp pkt;group ip[23:16] for igmp pkt*/
            uint32 tcp_sn_eq_0:1;       /*tcp sequence number is all 0*/
            uint32 l4_content;       /*{sport,dport} for tcp/udp pkt;{type,code,16bit don¡¯t care}for icmp pkt;{type,8bit don¡¯t care,group ip[15:0]} for igmp pkt*/
            uint32 rx_rm_rtag:1;             /*rx remove rspan tag*/
            uint32 icmp_pkt:1;          /*this is a icmp pkt;the icmp header must be complete*/
            uint32 igmp_pkt:1;          /*this is a igmp pkt*/
            uint32 udp_pkt:1;           /*this is a udp pkt*/
            uint32 tcp_pkt:1;           /*this is a tcp pkt*/
            uint32 pppoe_pkt:1;         /*this is a pppoe pkt*/
            uint32 arp_pkt:1;           /*this a arp pkt*/
            uint32 ipv6_pkt:1;          /*this is a ipv6 pkt*/
            uint32 ipv4_pkt:1;          /*this is a ipv4 pkt*/
            uint32 frame_type:2;        /*ethernet II or snap LLC or length encapulation according to spec:
                                                           *Ethernet II:0b00;RFC_1042:0b01;LLC other:0b10;others:0b11
                                                           */
            uint32 itag_content:16;     /*the {pri, cfi, ivid} of itag*/
            uint32 otag_content:16;     /*the {pri, dei, ovid} of otag, the {pri, 13bit don't care} of rspan tag*/

            /*ctag content exclude 'proto' and 'reserve' is 41bit*/
            uint32 ctag_acl_act:1;      /*take effect of ingress ACL or not*/
            uint32 ctag_bp_fltr_1:1;    /*if value is 0b1, bypass the following filtering:(1)egress port isolation (2)egress mirror isolation*/
            uint32 ctag_bp_fltr_2:1;    /*if value is 0b1, bypass the following checking:(1)egress spanning tree port state in blocking/listening or learning (2)egress vlan filtering*/
            uint32 ctag_as_tagsts:1;    /*decide whether proceed outer.inner Tag*/
            uint32 ctag_rvid_sel:1;     /*relay vid selection:0b0 follow the inner tag;0b1 follow the outer tag*/
            uint32 ctag_l2learning:1;   /*decide whether proceed l2 learning*/
            uint32 ctag_as_pri:1;       /*if CPU provides the priority directly*/
            uint32 ctag_pri:3;          /*assigned priority*/
            uint32 ctag_as_dpm:1;       /*0b1:the address lookup does not take effect and the assigned DPM is used as the destination port mask*/
            uint32 ctag_physical_dpm:1; /*decide DPM property:0 logical port; 1 physical port*/
            uint32 ctag_dpm:29;         /*the assigned packet destination port mask*/

            uint32 itag_exist:1;        /*this pkt is with itag*/
            uint32 otag_exist:1;        /*this pkt is with otag*/
            uint32 rtag_exist:1;        /*this pkt is with rspan tag*/
            uint32 etag_exist:1;        /*this pkt is with extra tag*/
            uint32 ctag_exist:1;        /*this pkt is with cpu tag*/
            uint32 eav_b_pkt:1;         /*no use at present*/
            uint32 eav_a_pkt:1;         /*no use at present*/
            uint32 crceq:1;             /*no use at present*/
            uint32 typelen:16;          /*the type or length field of pkt*/
            uint32 fs3_data:16;         /*field selector 3 select data*/
            uint32 fs2_data:16;         /*field selector 2 select data*/
            uint32 fs1_data:16;         /*field selector 1 select data*/
            uint32 fs0_data:16;         /*field selector 0 select data*/
            uint32 fs3_valid:1;         /*field selector 3 select data is valid*/
            uint32 fs2_valid:1;         /*field selector 2 select data is valid*/
            uint32 fs1_valid:1;         /*field selector 1 select data is valid*/
            uint32 fs0_valid:1;         /*field selector 0 select data is valid*/
            uint8 smac[6];              /*source mac address*/
            uint8 dmac[6];              /*destination mac address*/
            uint32 pktlen:14;             /*the total bytes store in pkt buffer*/
            uint32 sphy:5;              /*the id of this rx port*/
            uint32 rxdrop:1;            /*the pkt is decieded to drop by rx*/
            uint32 oampdu:1;            /*the pkt is oampdu pkt*/
            uint32 itag_index:2;        /*the tpid index of itag pkt*/
            uint32 otag_index:2;        /*the tpid index of otag/rspan tag pkt*/
            uint32 ipv6_extension_hdrlen:14;      /*ipv6 extenstion header length*/
            uint32 ipv6_flow_label:20;          /*ipv6 flow label*/
            uint32 ipv6_mob_ext_hdr:1;      /*ipv6 mobility extension header*/
            uint32 ipv6_hbh_ext_hdr_err:1;  /*ipv6 hop by hop extension header error*/
            uint32 ipv6_unknown_hdr:1;   /*ipv6 unknown hdr*/
            uint32 reserved1 : 2;
            uint32 reserved2: 27;
        } r8380;
        struct
        {
            uint32 lbhdr_vld:1;         /* loopback header field is valid */
            uint32 lbhdr_org_srcport:5;
            uint32 lbhdr_lbttl:3;
            uint32 lbhdr_metadata:8;

            uint32 sthdr_vld:1;         /* Stacking header field is valid */

            uint32 sthdr_comm_ver:2;
            uint32 sthdr_comm_info_hdr_type:1;
            uint32 sthdr_comm_sp_info:10;
            uint32 sthdr_comm_sp_filter:1;
            uint32 sthdr_comm_dp_fmt:1;
            uint32 sthdr_comm_dp_info:12;
            uint32 sthdr_comm_fwd_type:2;
            uint32 sthdr_comm_sa_lrn:1;
            uint32 sthdr_comm_vlan_tag:2;

            uint32 sthdr_info_cpu_tag_if:2;
            uint32 sthdr_info_int_pri:3;
            uint32 sthdr_info_trk_hash:6;
            uint32 sthdr_info_class:2;
            uint32 sthdr_info_da_hit:1;
            uint32 sthdr_info_vb_iso_mbr:1;
            uint32 sthdr_info_int_otag_hit:1;
            uint32 sthdr_info_int_otag_if:1;
            uint32 sthdr_info_int_otag_tpid_hit:1;
            uint32 sthdr_info_int_otag_tpid_idx:2;
            uint32 sthdr_info_int_otag_pri_hit:1;
            uint32 sthdr_info_int_otag_pri:3;
            uint32 sthdr_info_int_ocfi:1;
            uint32 sthdr_info_int_ovid:12;
            uint32 sthdr_info_int_itag_hit:1;
            uint32 sthdr_info_int_itag_if:1;
            uint32 sthdr_info_int_itag_tpid_hit:1;
            uint32 sthdr_info_int_itag_tpid_idx:2;
            uint32 sthdr_info_int_itag_pri_hit:1;
            uint32 sthdr_info_int_itag_pri:3;
            uint32 sthdr_info_int_icfi:1;
            uint32 sthdr_info_int_ivid:12;
            uint32 sthdr_info_fwd_base:1;
            uint32 sthdr_info_dp:2;
            uint32 sthdr_info_int_dscp_hit:1;
            uint32 sthdr_info_mir_id:2;
            uint32 sthdr_info_rout_tunnel_if:1;
            uint32 sthdr_info_igr_l3_intf_type:1;
            uint32 sthdr_info_igr_l3_intf_id:12;
            uint32 sthdr_vlan_ori_opri:3;
            uint32 sthdr_vlan_ori_ocfi:1;
            uint32 sthdr_vlan_ori_ipri:3;
            uint32 sthdr_vlan_ori_icfi:1;

            uint32 sthdr_txCtag_fwd_type:4;
            uint32 sthdr_txCtag_acl_act:1;
            uint32 sthdr_txCtag_cngst_drop:1;
            uint32 sthdr_txCtag_dm_pkt:1;
            uint32 sthdr_txCtag_dg_pkt:1;
            uint32 sthdr_txCtag_bp_filter:1;
            uint32 sthdr_txCtag_bp_stp:1;
            uint32 sthdr_txCtag_bp_vlan_egr:1;
            uint32 sthdr_txCtag_ale_as_tagsts:1;
            uint32 sthdr_txCtag_l3_act:1;
            uint32 sthdr_txCtag_as_qid:1;
            uint32 sthdr_txCtag_qid:5;
            uint32 sthdr_txCtag_src_filter_en:1;
            uint32 sthdr_txCtag_sp_is_trk:1;
            uint32 sthdr_txCtag_spn:10;
            uint32 sthdr_txCtag_sw_unit:4;
            rtk_portmask_t sthdr_txCtag_dpm;

            uint32 sthdr_rxCtag_of_lu_mis_tbl_id:2;
            uint32 sthdr_rxCtag_acl_of_hit:2;
            uint32 sthdr_rxCtag_of_tbl_id:2;
            uint32 sthdr_rxCtag_l2_err_pkt:1;
            uint32 sthdr_rxCtag_l3_err_pkt:1;
            uint32 sthdr_rxCtag_atk_type:5;
            uint32 sthdr_rxCtag_qid:5;
            uint32 sthdr_rxCtag_idx:15;
            uint32 sthdr_rxCtag_sflow:2;
            uint32 sthdr_rxCtag_tt_hit:1;
            uint32 sthdr_rxCtag_tt_idx:9;
            uint32 sthdr_rxCtag_mac_cst:1;
            uint32 sthdr_rxCtag_dm_rxidx_of_lb_times:6;
            uint32 sthdr_rxCtag_new_sa:1;
            uint32 sthdr_rxCtag_pmv_forbid:1;
            uint32 sthdr_rxCtag_l2_sttc_pmv:1;
            uint32 sthdr_rxCtag_l2_dyn_pmv:1;
            uint32 sthdr_rxCtag_port_data_is_trk:1;
            uint32 sthdr_rxCtag_port_data:10;
            uint32 sthdr_rxCtag_hash_full:1;
            uint32 sthdr_rxCtag_invalid_sa:1;
            uint32 sthdr_rxCtag_rsn:6;

            uint32 sthdr_acl_fwd_cpu_pkt_hit:1;
            uint32 sthdr_acl_fwd_cpu_pkt_fmt:1;

            uint32 page_cnt:6;
            uint32 lst_dsc:12;          /* End page point */
            uint32 fst_dsc:12;          /* Start page point */
            uint32 fs11_data:16;        /* Non IPv6 : User defined 16-bit field IPv6 : SIP[127:112] */
            uint32 fs10_data:16;        /* Non IPv6 : User defined 16-bit field IPv6 : SIP[111:96] */
            uint32 fs9_data:16;         /* Non IPv6 : User defined 16-bit field IPv6 : SIP[95:80] */
            uint32 fs8_data:16;         /* Non IPv6 : User defined 16-bit field IPv6 : SIP[79:64] */
            uint32 fs7_data:16;         /* Non IPv6 : User defined 16-bit field IPv6 : SIP[63:48] */
            uint32 fs6_data:16;         /* Non IPv6 : User defined 16-bit field IPv6 : SIP[47:32] */
            uint32 fs5_data:16;         /* User defined 16-bit field */
            uint32 fs4_data:16;         /* User defined 16-bit field, CPUTAG[79:64] */
            uint32 fs3_data:16;         /* User defined 16-bit field, CPUTAG[63:48] */
            uint32 fs2_data:16;         /* User defined 16-bit field, CPUTAG[47:32] */
            uint32 fs1_data:16;         /* User defined 16-bit field, shared for CPUTAG[31:16] */
            uint32 fs0_data:16;         /* User defined 16-bit field, shared for CPUTAG[15:0] */
            uint32 fs_valid:12;         /* Indicates which field_selector is valid */
            uint32 igr_err:1;           /* Ingress error packet */
            uint32 ipv6_flow_label:20;  /* IPv6 flow label */
            uint32 udp_ptp:1;           /* Is UDP PTP */
            uint32 l2_ptp:1;            /* Is L2 PTP */
            uint32 dsap_ssap:24;        /* LLC DSAP,SSAP,CTRL or SNAP OUI */
            uint32 arp_opcode:16;       /* ARP/RARP: Operation field
                                           IPv6: {5'h0, exist HOP header in error position,
                                                  exist ESP header, exist MOBI header, Extented header length} */
            uint8  target_mac[6];       /* ARP/RARP :Target MAC of ARP/RARP packet, for ACL. IPV6 : DIP[127:80] */
            uint8  sender_mac[6];       /* ARP/RARP: Sender MAC of ARP/RARP packet, for ACL. IPV6 : DIP[79:32] */
            uint32 dport:16;            /* TCP/UDP destination port, ICMP code B[7:0] */
            uint32 sport:16;            /* TCP/UDP source port, ICMP type B[7:0] */
            uint32 tcp_flag:8;          /* TCP : TCP Flag. IGMP : IGMP Type field */
            uint32 ip_ver:4;            /* IP header version (For Ipv4 and Ipv6) */
            uint32 ip_ttl:8;            /* IP header - TTL field */
            uint32 ip_tos:8;            /* IP header - DSCP/TOS */
            uint32 ip_protocol:8;       /* IP header - protocol field, IPv6: Next header */
            uint32 ip_flag:3;           /* IPV4 : IP header- flag. IPV6 : IP flag in fragment header */
            uint32 ip_offset:13;        /* IPV4 : IP header- offset. IPV6 : IP offset in fragment header */
            uint32 dip;                 /* IPv4: { DIP[31:0] }, IPv6: DIP[31:0], target IP for ARP/RARP
                                            B[15:0]=MEPID of CCM packet
                                            B[23:16]=OpCode of CFM packet
                                            B[26:24]=MD Level of CFM packet
                                            B[29:27]=CFM Interval Field
                                            B[31:30]=reserved */
            uint32 sip;                 /* IPv4: {SIP[31:0] }, IPv6: SIP[31:0], source IP for ARP/RARP */
            uint32 opri:3;              /* Outer VLAN Tag, B[15:13]=user-priority,B[12]=DEI,B[11:0]=VID */
            uint32 ocfi:1;
            uint32 ovid:12;
            uint32 ipri:3;              /* Inner VLAN Tag, B[15:13]=user-priority,B[12]=CFI,B[11:0]=VID */
            uint32 icfi:1;
            uint32 ivid:12;
            uint32 otpid_idx:2;         /* TPID index for outer tag */
            uint32 itpid_idx:2;         /* TPID index for inner tag */
            uint32 typelen:16;          /* Ethernet/Length field(Ether Type) */
            uint8  smac[6];             /* Source MAC address */
            uint8  dmac[6];             /* Destination MAC address */
            uint32 ip_length:16;        /* IPv4 Total length or IPv6 payload length(For Attack prevention) */
            uint32 ipv4_hdrlen:4;       /* IPv4 header length */
            uint32 tcp_sn_eq_0:1;       /* TCP sequence number is zero */
            uint32 ipv6_hop:1;          /* IPv6 packet with Hop-by-Hop header */
            uint32 ipv6_rout:1;         /* IPv6 packet with routing header */
            uint32 ipv6_frag:1;         /* IPv6 packet with fragment header */
            uint32 ipv6_dest:1;         /* Ipv6 packet with destination header */
            uint32 ipv6_auth:1;         /* IPv6 packet with authentication header */
            uint32 ipv4_chksum_ok:1;    /* IP checksum is ok */
            uint32 ipv6_ext_long:1;     /* IPv6 header is too long for parser */
            uint32 ipv6_pkt:1;          /* IPv6 Type */
            uint32 ipv4_pkt:1;          /* IPv4 Type */
            uint32 rtag_type:1;         /* RSPAN tagged tpid hit inner/outer tpid */
            uint32 rtag_exist:1;        /* Has RSPAN Tag */
            uint32 etag_exist:1;        /* Extra Tag */
            uint32 ctag_exist:1;        /* Has CPU Tag */
            uint32 otag_exist:1;        /* Has Outer VLAN Tag(Tag or Priority tag) */
            uint32 itag_exist:1;        /* Has Inner VLAN Tag(Tag or priority tag) */
            uint32 oam_pdu:1;           /* OAM control frame */
            uint32 llc_other:1;         /* LEN < 0x0600, {DSAP, SSAP, CTL, OUI}  != 0xaaaa03_000000 */
            uint32 pppoe_pkt:1;         /* PPPoE packet */
            uint32 rfc1042:1;           /* LLC SNAP */
            uint32 sphy:5;              /* Source Port ID */
            uint32 l2_err_pkt:1;        /* Error packet(CRC,Symbol) */
            uint32 l4_hdr_chk:1;        /* Check the non-fragment or first fragment of IP packet(TCP/UDP/ICMP) has enough length for L4 header(For attack prevention) */
            uint32 pkt_len:14;          /* Packet length (Include CRC) */

            /* CPU tag */
            uint32 ctag_fwd_type:4;     /* Forwarding type */
            uint32 ctag_acl_act:1;      /*take effect of ingress ACL or not*/
            uint32 ctag_cngst_drop:1;   /* Congestion Dropable */
            uint32 ctag_dg_pkt:1;       /* Dying Gasp packet */
            uint32 ctag_bp_fltr:1;      /* if value is 0b1, bypass the following filtering:(1)egress port isolation (2)egress mirror isolation*/
            uint32 ctag_bp_stp:1;       /* bypass STP */
            uint32 ctag_bp_vlan_egr:1;  /* bypass egress VLAN filtering */
            uint32 ctag_ale_as_tagsts:1;/* decide whether proceed outer/inner Tag by ALE */
            uint32 ctag_l3_act:1;       /* Indicates whether take the result of L3 routing and multicast replication */
            uint32 ctag_ori_tagif_en:1; /* Indicate whether the packet's tag status is parsed from ingress port's view */
            uint32 ctag_as_qid:1;       /* CPU assign the output queue ID */
            uint32 ctag_qid:5;          /* Assigned QID */
            uint32 ctag_ori_itagif:1;   /* Indicate whether this packet contains the inner tag. */
            uint32 ctag_ori_otagif:1;   /* Indicate whether this packet contains the outer tag. */
            uint32 ctag_fwd_vid_sel:1;  /* Forwarding VID selection */
            uint32 ctag_fwd_vid_en:1;   /* follow ALE decision or force use FVID */
            uint32 ctag_fwd_vid:12;     /* Forwarding VID */
            uint32 ctag_src_filter_en:1;/* Source port filtered from outgoing portmask */
            uint32 ctag_sp_is_trk:1;    /* Source port is trunk */
            uint32 ctag_spn:10;          /* Source port/trunk ID */
            uint32 ctag_dev_id:4;          /* destination device ID */
            rtk_portmask_t ctag_dpm;     /*the assigned packet destination port mask*/
        }r9300;
        struct
        {
            uint32 OHSB_ERR_PKT:1;
            uint32 OHSB_L2_CRC_ERR:1;
            uint32 OHSB_PKT_LEN:14;
            uint32 OHSB_PARSER_CANT_HANDLE:1;
            uint32 OHSB_MALFORMED_PKT:1;
            uint32 OHSB_SPP:6;
            uint32 OHSB_RTAG_IF:3;
            uint32 OHSB_RTAG_239_208;
            uint32 OHSB_RTAG_207_176;
            uint32 OHSB_RTAG_175_144;
            uint32 OHSB_RTAG_143_128:16;
            uint32 OHSB_DMAC_47_32:16;
            uint32 OHSB_DMAC_31_0;
            uint32 OHSB_SMAC_47_16;
            uint32 OHSB_SMAC_15_0:16;
            uint32 OHSB_OTAG:16;
            uint32 OHSB_ITAG:16;
            uint32 OHSB_OTAG_IF:1;
            uint32 OHSB_OTAG_TPID_IDX:2;
            uint32 OHSB_ITAG_IF:1;
            uint32 OHSB_ITAG_TPID_IDX:2;
            uint32 OHSB_ETAG_IF:1;
            uint32 OHSB_OMPLS_IF:1;
            uint32 OHSB_IMPLS_IF:1;
            uint32 OHSB_OMPLS_HDR;
            uint32 OHSB_IMPLS_HDR;
            uint32 OHSB_ETHTYPE:16;
            uint32 OHSB_FRAME_TYPE:2;
            uint32 OHSB_OAMPDU:1;
            uint32 OHSB_PPPOE_IF_MPLS_TYPE:1;
            uint32 OHSB_IPV4_TYPE:2;
            uint32 OHSB_IPV6_TYPE:2;
            uint32 OHSB_IP_VER:4;
            uint32 OHSB_IP_HDR_LEN:4;
            uint32 OHSB_IP_TOS:8;
            uint32 OHSB_IP6_FLOW_LBL:20;
            uint32 OHSB_IP_LEN:16;
            uint32 OHSB_IP_FLAG:3;
            uint32 OHSB_IP_OFFSET:13;
            uint32 OHSB_IP_TTL:8;
            uint32 OHSB_IP_PROTO:8;
            uint32 OHSB_L4_OFFSET:8;
            uint32 OHSB_IP_SIP3_FIELD_SEL11_10;
            uint32 OHSB_IP_SIP2_FIELD_SEL9_8;
            uint32 OHSB_IP_SIP1_FIELD_SEL7_6;
            uint32 OHSB_IP_SIP;
            uint32 OHSB_IP_DIP3;
            uint32 OHSB_IP_DIP2;
            uint32 OHSB_IP_DIP1;
            uint32 OHSB_IP_DIP;
            uint32 OHSB_IP6_EH_HOPBYHOP_ERR:1;
            uint32 OHSB_IP6_EH_HOPBYHOP_EXIST:1;
            uint32 OHSB_IP6_EH_ROUTE_EXIST:1;
            uint32 OHSB_IP6_EH_FRAGMENT_EXIST:1;
            uint32 OHSB_IP6_EH_ESP_EXIST:1;
            uint32 OHSB_IP6_EH_AUTH_EXIST:1;
            uint32 OHSB_IP6_EH_DESTINATION_EXIST:1;
            uint32 OHSB_IP6_EH_MOBILITY_EXIST:1;
            uint32 OHSB_IP6_EH_REPEAT:1;
            uint32 OHSB_IP6_EH_TOTAL_LEN:6;
            uint32 OHSB_L4_HDR_RDY:1;
            uint32 OHSB_L4_SPORT:16;
            uint32 OHSB_L4_DPORT:16;
            uint32 OHSB_L4_OPCODE:16;
            uint32 OHSB_L4_DATA:16;
            uint32 OHSB_CAPWAP_PRE:8;
            uint32 OHSB_CAPWAP_HLEN:5;
            uint32 OHSB_CAPWAP_RID:5;
            uint32 OHSB_CAPWAP_WBID:5;
            uint32 OHSB_CAPWAP_FLAG:6;
            uint32 OHSB_CAPWAP_RMAC_LEN:8;
            uint32 OHSB_CAPWAP_HDR_RDY:1;
            uint32 OHSB_CAPWAP_BSSID_VALID:1;
            uint32 OHSB_CAPWAP_BSSID_47_16;
            uint32 OHSB_CAPWAP_BSSID_15_0:16;
            uint32 IHSB_L3_OFFSET:8;
            uint32 IHSB_L4_OFFSET:8;
            uint32 IHSB_PARSER_CANT_HANDLE:1;
            uint32 IHSB_MALFORMED_PKT:1;
            uint32 IHSB_L2_RDY:1;
            uint32 IHSB_FRAME_CTRL:16;
            uint32 IHSB_QOS_CTRL:16;
            uint32 IHSB_DMAC_47_16;
            uint32 IHSB_DMAC_15_0:16;
            uint32 IHSB_SMAC_47_32:16;
            uint32 IHSB_SMAC_31_0;
            uint32 IHSB_OTAG:16;
            uint32 IHSB_ITAG:16;
            uint32 IHSB_ETHTYPE:16;
            uint32 IHSB_OTAG_IF:1;
            uint32 IHSB_OTAG_TPID_IDX:2;
            uint32 IHSB_ITAG_IF:1;
            uint32 IHSB_ITAG_TPID_IDX:2;
            uint32 IHSB_FRAME_TYPE:2;
            uint32 IHSB_IPV4_TYPE:2;
            uint32 IHSB_IPV6_TYPE:2;
            uint32 IHSB_IP_VER:4;
            uint32 IHSB_IP_HDR_LEN:4;
            uint32 IHSB_IP_TOS:8;
            uint32 IHSB_IP6_FLOW_LBL:20;
            uint32 IHSB_IP_LEN:16;
            uint32 IHSB_IP_FLAG:3;
            uint32 IHSB_IP_OFFSET:13;
            uint32 IHSB_IP_TTL:8;
            uint32 IHSB_IP_PROTO:8;
            uint32 IHSB_IP_SIP3_FIELD_SEL5_4;
            uint32 IHSB_IP_SIP2_FIELD_SEL3_2;
            uint32 IHSB_IP_SIP1_FIELD_SEL1_0;
            uint32 IHSB_IP_SIP;
            uint32 IHSB_IP_DIP3;
            uint32 IHSB_IP_DIP2;
            uint32 IHSB_IP_DIP1;
            uint32 IHSB_IP_DIP;
            uint32 IHSB_IP6_EH_HOPBYHOP_ERR:1;
            uint32 IHSB_IP6_EH_HOPBYHOP_EXIST:1;
            uint32 IHSB_IP6_EH_ROUTE_EXIST:1;
            uint32 IHSB_IP6_EH_FRAGMENT_EXIST:1;
            uint32 IHSB_IP6_EH_ESP_EXIST:1;
            uint32 IHSB_IP6_EH_AUTH_EXIST:1;
            uint32 IHSB_IP6_EH_DESTINATION_EXIST:1;
            uint32 IHSB_IP6_EH_MOBILITY_EXIST:1;
            uint32 IHSB_IP6_EH_REPEAT:1;
            uint32 IHSB_L4_HDR_RDY:1;
            uint32 IHSB_IP6_EH_TOTAL_LEN:6;
            uint32 IHSB_L4_SPORT:16;
            uint32 IHSB_L4_DPORT:16;
            uint32 IHSB_L4_OPCODE:16;
            uint32 IHSB_L4_DATA:16;
            uint32 HSB_FIELD_SEL12:16;
            uint32 HSB_FIELD_SEL13:16;
            uint32 HSB_FIELD_VALID:16;
            uint32 HSB_PKT_TYPE:3;
            uint32 HSB_EST_INVALID:1;
            uint32 HSB_PE_ETAG_IF:1;
            uint32 HSB_PE_ETAG_PRI:3;
            uint32 HSB_PE_ETAG_DEI:1;
            uint32 HSB_PE_IGR_ECID_BASE:12;
            uint32 HSB_PE_ETAG_RSVD:2;
            uint32 HSB_PE_ECID_GRP:2;
            uint32 HSB_PE_ECID_BASE:12;
            uint32 HSB_PE_IGR_ECID_EXT:8;
            uint32 HSB_PE_ECID_EXT:8;
            uint32 HSB_BEGIN_DESC:13;
            uint32 HSB_END_DESC:13;
            uint32 HSB_PAGE:6;
        } r9310;

#if 0/*use same structure 'r9310', translate the meaning inside hal_dumpHsb_openflow() */
        struct
        {
            uint32 OHSB_ERR_PKT:1;
            uint32 OHSB_L2_CRC_ERR:1;
            uint32 OHSB_PKT_LEN:14;
            uint32 OHSB_PARSER_CANT_HANDLE:1;
            uint32 OHSB_MALFORMED_PKT:1;
            uint32 OHSB_SPP:6;
            uint32 OHSB_RTAG_IF:3;

            uint32 sport:6;/*OHSB_RTAG_239_208[31:26]*/
            uint32 org_itag:16;/*OHSB_RTAG_239_208[25:10]*/
            uint32 org_itag_tpid_idx:2;/*OHSB_RTAG_239_208[9:8]*/
            uint32 org_itag_if:1;/*OHSB_RTAG_239_208[7]*/
            uint32 org_otag:16;/*OHSB_RTAG_239_208[6:0],OHSB_RTAG_207_176[31:23]*/
            uint32 org_otag_tpid_idx:2;/*OHSB_RTAG_207_176[22:21]*/
            uint32 org_otag_if:1;/*OHSB_RTAG_207_176[20]*/
            uint32 to_of_pipe:1;/*OHSB_RTAG_207_176[19]*/
            uint32 current_lb_time:6;/*OHSB_RTAG_207_176[18:13]*/
            uint32 target_lb_time:6;/*OHSB_RTAG_207_176[12:7]*/
            uint32 next_tbl_id:2;/*OHSB_RTAG_207_176[6:5]*/
            uint32 metadata:12;/*OHSB_RTAG_207_176[4:0],OHSB_RTAG_175_144[31:25]*/
            uint32 valid_action_set:1;/*OHSB_RTAG_175_144[24]*/
            uint32 cp_ttl_inward:1;/*OHSB_RTAG_175_144[23]*/
            uint32 pop_vlan:1;/*OHSB_RTAG_175_144[22]*/
            uint32 pop_mpls:2;/*OHSB_RTAG_175_144[21:20]*/
            uint32 push_mpls:2;/*OHSB_RTAG_175_144[19:18]*/
            uint32 push_mpls_type:1;/*OHSB_RTAG_175_144[17]*/
            uint32 push_mpls_lib_idx:11;/*OHSB_RTAG_175_144[16:6]*/
            uint32 push_mpls_ethtype:1;/*OHSB_RTAG_175_144[5]*/
            uint32 push_vlan:1;/*OHSB_RTAG_175_144[4]*/
            uint32 push_vlan_ethtype:2;/*OHSB_RTAG_175_144[3:2]*/
            uint32 cp_ttl_outward:1;/*OHSB_RTAG_175_144[1]*/
            uint32 dec_mpls_ttl:1;/*OHSB_RTAG_175_144[0]*/
            uint32 dec_ip_ttl:1;/*OHSB_RTAG_143_128[15]*/
            uint32 set_field_sa:1;/*OHSB_RTAG_143_128[14]*/
            uint32 set_filed_sa_data:10;/*OHSB_RTAG_143_128[13:4]*/
            uint32 set_field_da:1;/*OHSB_RTAG_143_128[3]*/
            uint32 set_filed_da_data:16;/*OHSB_RTAG_143_128[2:0],IHSB_FIELD_SEL13[15:3]*/
            uint32 set_field_vid:1;/*IHSB_FIELD_SEL13[2]*/
            uint32 set_filed_vid_data:12;/*IHSB_FIELD_SEL13[1:0],IHSB_FIELD_SEL12[15:6]*/
            uint32 set_field_pcp:1;/*IHSB_FIELD_SEL12[5]*/
            uint32 set_field_pcp_data:3;/*IHSB_FIELD_SEL12[4:2]*/
            uint32 set_filed_mpls_lib:1;/*IHSB_FIELD_SEL12[1]*/
            uint32 set_filed_mpls_lib_data:11;/*IHSB_FIELD_SEL12[0],IHSB_IP_SIP3_FIELD_SEL5_4[31:22]*/
            uint32 set_field_mpls_tc:1;/*IHSB_IP_SIP3_FIELD_SEL5_4[21]*/
            uint32 set_field_mpls_tc_data:3;/*IHSB_IP_SIP3_FIELD_SEL5_4[20:18]*/
            uint32 set_field_mpls_ttl:1;/*IHSB_IP_SIP3_FIELD_SEL5_4[17]*/
            uint32 set_field_mpls_ttl_data:8;/*IHSB_IP_SIP3_FIELD_SEL5_4[16:9]*/
            uint32 set_field_ip_dscp:1;/*IHSB_IP_SIP3_FIELD_SEL5_4[8]*/
            uint32 set_field_ip_dscp_data:6;/*IHSB_IP_SIP3_FIELD_SEL5_4[7:2]*/
            uint32 set_field_ip_ttl:1;/*IHSB_IP_SIP3_FIELD_SEL5_4[1]*/
            uint32 set_field_ip_ttl_data:8;/*IHSB_IP_SIP3_FIELD_SEL5_4[0],IHSB_IP_SIP2_FIELD_SEL3_2[31:25]*/
            uint32 set_field_ip_sip:1;/*IHSB_IP_SIP2_FIELD_SEL3_2[24]*/
            uint32 set_field_ip_sip_data:32;/*IHSB_IP_SIP2_FIELD_SEL3_2[23:0],IHSB_IP_SIP1_FIELD_SEL1_0[31:24]*/
            uint32 set_field_ip_dip:1;/*IHSB_IP_SIP1_FIELD_SEL1_0[23]*/
            uint32 set_field_ip_dip_data:32;/*IHSB_IP_SIP1_FIELD_SEL1_0[22:0],IHSB_DMAC_47_16[31:23]*/
            uint32 set_field_l4_sport:1;/*IHSB_DMAC_47_16[22]*/
            uint32 set_field_l4_sport_data:16;/*IHSB_DMAC_47_16[21:6]*/
            uint32 set_field_l4_dport:1;/*IHSB_DMAC_47_16[5]*/
            uint32 set_field_l4_dport_data:16;/*IHSB_DMAC_47_16[4:0],IHSB_DMAC_15_0[15:5]*/
            uint32 set_field_ip_rsvd:1;/*IHSB_DMAC_15_0[4]*/
            uint32 set_queue:1;/*IHSB_DMAC_15_0[3]*/
            uint32 set_queue_data:3;/*IHSB_DMAC_15_0[2:0]*/
            uint32 group:1;/*IHSB_SMAC_47_32[15]*/
            uint32 group_idx:11;/*IHSB_SMAC_47_32[14:8],IHSB_SMAC_47_32[7:4]*/
            uint32 output_type:4;/*IHSB_SMAC_47_32[3:0]*/
            uint32 output_data:12;/*IHSB_SMAC_31_0[31:20]*/

            uint32 OHSB_DMAC_47_32:16;
            uint32 OHSB_DMAC_31_0;
            uint32 OHSB_SMAC_47_16;
            uint32 OHSB_SMAC_15_0:16;
            uint32 OHSB_OTAG:16;
            uint32 OHSB_ITAG:16;
            uint32 OHSB_OTAG_IF:1;
            uint32 OHSB_OTAG_TPID_IDX:2;
            uint32 OHSB_ITAG_IF:1;
            uint32 OHSB_ITAG_TPID_IDX:2;
            uint32 OHSB_ETAG_IF:1;
            uint32 OHSB_OMPLS_IF:1;
            uint32 OHSB_IMPLS_IF:1;
            uint32 OHSB_OMPLS_HDR;
            uint32 OHSB_IMPLS_HDR;
            uint32 OHSB_ETHTYPE:16;
            uint32 OHSB_FRAME_TYPE:2;
            uint32 OHSB_OAMPDU:1;
            uint32 OHSB_PPPOE_IF_MPLS_TYPE:1;
            uint32 OHSB_IPV4_TYPE:2;
            uint32 OHSB_IPV6_TYPE:2;
            uint32 OHSB_IP_VER:4;
            uint32 OHSB_IP_HDR_LEN:4;
            uint32 OHSB_IP_TOS:8;
            uint32 OHSB_IP6_FLOW_LBL:20;
            uint32 OHSB_IP_LEN:16;
            uint32 OHSB_IP_FLAG:3;
            uint32 OHSB_IP_OFFSET:13;
            uint32 OHSB_IP_TTL:8;
            uint32 OHSB_IP_PROTO:8;
            uint32 OHSB_L4_OFFSET:8;
            uint32 OHSB_IP_SIP3_FIELD_SEL11_10;
            uint32 OHSB_IP_SIP2_FIELD_SEL9_8;
            uint32 OHSB_IP_SIP1_FIELD_SEL7_6;
            uint32 OHSB_IP_SIP;
            uint32 OHSB_IP_DIP3;
            uint32 OHSB_IP_DIP2;
            uint32 OHSB_IP_DIP1;
            uint32 OHSB_IP_DIP;
            uint32 OHSB_IP6_EH_HOPBYHOP_ERR:1;
            uint32 OHSB_IP6_EH_HOPBYHOP_EXIST:1;
            uint32 OHSB_IP6_EH_ROUTE_EXIST:1;
            uint32 OHSB_IP6_EH_FRAGMENT_EXIST:1;
            uint32 OHSB_IP6_EH_ESP_EXIST:1;
            uint32 OHSB_IP6_EH_AUTH_EXIST:1;
            uint32 OHSB_IP6_EH_DESTINATION_EXIST:1;
            uint32 OHSB_IP6_EH_MOBILITY_EXIST:1;
            uint32 OHSB_IP6_EH_REPEAT:1;
            uint32 OHSB_IP6_EH_TOTAL_LEN:6;
            uint32 OHSB_L4_HDR_RDY:1;
            uint32 OHSB_L4_SPORT:16;
            uint32 OHSB_L4_DPORT:16;
            uint32 OHSB_L4_OPCODE:16;
            uint32 OHSB_L4_DATA:16;
            uint32 OHSB_CAPWAP_PRE:8;
            uint32 OHSB_CAPWAP_HLEN:5;
            uint32 OHSB_CAPWAP_RID:5;
            uint32 OHSB_CAPWAP_WBID:5;
            uint32 OHSB_CAPWAP_FLAG:6;
            uint32 OHSB_CAPWAP_RMAC_LEN:8;
            uint32 OHSB_CAPWAP_HDR_RDY:1;
            uint32 OHSB_CAPWAP_BSSID_VALID:1;
            uint32 OHSB_CAPWAP_BSSID_47_16;
            uint32 OHSB_CAPWAP_BSSID_15_0:16;
            uint32 IHSB_L3_OFFSET:8;
            uint32 IHSB_L4_OFFSET:8;
            uint32 IHSB_PARSER_CANT_HANDLE:1;
            uint32 IHSB_MALFORMED_PKT:1;
            uint32 IHSB_L2_RDY:1;
            uint32 IHSB_FRAME_CTRL:16;
            uint32 IHSB_QOS_CTRL:16;
            uint32 IHSB_OTAG:16;
            uint32 IHSB_ITAG:16;
            uint32 IHSB_ETHTYPE:16;
            uint32 IHSB_OTAG_IF:1;
            uint32 IHSB_OTAG_TPID_IDX:2;
            uint32 IHSB_ITAG_IF:1;
            uint32 IHSB_ITAG_TPID_IDX:2;
            uint32 IHSB_FRAME_TYPE:2;
            uint32 IHSB_IPV4_TYPE:2;
            uint32 IHSB_IPV6_TYPE:2;
            uint32 IHSB_IP_VER:4;
            uint32 IHSB_IP_HDR_LEN:4;
            uint32 IHSB_IP_TOS:8;
            uint32 IHSB_IP6_FLOW_LBL:20;
            uint32 IHSB_IP_LEN:16;
            uint32 IHSB_IP_FLAG:3;
            uint32 IHSB_IP_OFFSET:13;
            uint32 IHSB_IP_TTL:8;
            uint32 IHSB_IP_PROTO:8;
            uint32 IHSB_IP_SIP;
            uint32 IHSB_IP_DIP3;
            uint32 IHSB_IP_DIP2;
            uint32 IHSB_IP_DIP1;
            uint32 IHSB_IP_DIP;
            uint32 IHSB_IP6_EH_HOPBYHOP_ERR:1;
            uint32 IHSB_IP6_EH_HOPBYHOP_EXIST:1;
            uint32 IHSB_IP6_EH_ROUTE_EXIST:1;
            uint32 IHSB_IP6_EH_FRAGMENT_EXIST:1;
            uint32 IHSB_IP6_EH_ESP_EXIST:1;
            uint32 IHSB_IP6_EH_AUTH_EXIST:1;
            uint32 IHSB_IP6_EH_DESTINATION_EXIST:1;
            uint32 IHSB_IP6_EH_MOBILITY_EXIST:1;
            uint32 IHSB_IP6_EH_REPEAT:1;
            uint32 IHSB_L4_HDR_RDY:1;
            uint32 IHSB_IP6_EH_TOTAL_LEN:6;
            uint32 IHSB_L4_SPORT:16;
            uint32 IHSB_L4_DPORT:16;
            uint32 IHSB_L4_OPCODE:16;
            uint32 IHSB_L4_DATA:16;
            uint32 HSB_FIELD_VALID:16;
            uint32 HSB_PKT_TYPE:3;
            uint32 HSB_EST_INVALID:1;
            uint32 HSB_PE_ETAG_IF:1;
            uint32 HSB_PE_ETAG_PRI:3;
            uint32 HSB_PE_ETAG_DEI:1;
            uint32 HSB_PE_IGR_ECID_BASE:12;
            uint32 HSB_PE_ETAG_RSVD:2;
            uint32 HSB_PE_ECID_GRP:2;
            uint32 HSB_PE_ECID_BASE:12;
            uint32 HSB_PE_IGR_ECID_EXT:8;
            uint32 HSB_PE_ECID_EXT:8;
            uint32 HSB_BEGIN_DESC:13;
            uint32 HSB_END_DESC:13;
            uint32 HSB_PAGE:6;
        } r9310_of;
#endif
    };
};
typedef struct hsb_param_s hsb_param_t;


struct ppi_param_s
{
    uint32  sphy:5;
    uint32  crsvlan:1;
    uint32  keeporigivid:1;
    uint32  keeporigovid:1;
    uint32  pbivid:12;
    uint32  pbovid:12;


    uint32  pbipri:3;
    uint32  pbodei:1;
    uint32  ipuc:1;
    uint32  pbopri:3;
    uint32  ppbivid:12;
    uint32  ppbovid:12;


    uint32  ppbipri:3;
    uint32  ppbodei:1;
    uint32  upLinkMac_0:1;
    uint32  ppbopri:3;
    uint32  fcivid:12;
    uint32  fcovid:12;

    uint32  ttldec:1;
    uint32  fcseldot1qipri:1;
    uint32  fcseldot1qopri:1;
    uint32  rrcpType:3;
    uint32  rrcpAuth:1;
    uint32  ivid:12;
    uint32  ovid:12;
    uint32  drop:1;


    uint32  cpu:1;
    uint32  cputag:1;
    uint32  toGVlan:1;
    uint32  ipmcPkt:1;
    uint32  dot1qodp:2;
    uint32  sml:1;
    uint32  redir:1;
    uint32  lkmiss:1;
    uint32  ftidx:10;
    uint32  dpn:5;
    uint32  pbdp:2;
    uint32  pbpri:3;
    uint32  ftIdxValid:1;
    uint32  dot1qidp:2;
    uint32  dpm:29;
    uint32  dot1qipri:3;
    uint32  dscppriexist:1;
    uint32  dscpdp:2;
    uint32  dscppri:3;
    uint32  fbpriexist:1;
    uint32  fbdp:2;
    uint32  fbpri:3;
    uint32  ipriexist:1;
    uint32  idp:2;
    uint32  ipri:3;
    uint32  opriexist:1;
    uint32  odp:2;
    uint32  opri:3;
    uint32  dp:2;
    uint32  pri:3;
    uint32  macdot1x:1;
    uint32  fcdfi:1;
    uint32  fcdfo:1;
    uint32  tmidx:2;
    uint32  vidrch:1;
    uint32  fwd:4;
    uint32  mir:4;
    uint32  mirorg:4;
    uint32  spm:16;
    uint32  msti:7;
    uint32  fid:12;
    uint32  miriutag:4;
    uint32  miroutag:4;
    uint32  flood:1;
    uint32  ppbdfi:1;
    uint32  ppbdfo:1;
    uint32  tm:1;
    uint32  ipmc:1;
    uint32  copyToCpu:1;
    uint32  vidrc:5;
    uint32  reason:16;
    uint32  dscprmk:1;
    uint32  dot1qopri:3;
    uint32  ce:1;
    uint32  iutagstatus:29;
    uint32  iuntagValid:1;
    uint32  ountagValid:1;
    uint32  outagstatus:29;

    uint32  dscp:6;
    uint32  orgvid:12;
    uint32  dmacidx:14;

    uint32  rrcpRegData;

    uint32  prc:16;
    uint8   iprc[8];

    uint8   bpdu:1; /*for sw use*/
    uint32  egressvid:12;

    uint32  aclidx0:8;
    uint32  aclidx1:8;

    uint32  aclidx2:8;
    uint32  aclidx3:8;
    uint32  aclidx4:8;
    uint32  aclidx5:8;

    uint32  aclidx6:8;
    uint32  aclidx7:8;
    uint32  aclidx8:8;
    uint32  aclidx9:8;

    uint32  aclidx10:8;
    uint32  aclidx11:8;
    uint32  aclidx12:8;
    uint32  aclidx13:8;

    uint32  aclidx14:8;
    uint32  aclidx15:8;

};
typedef struct ppi_param_s ppi_param_t;


struct pmi_param_s
{
    uint32  dpm:29;         /*destination port mask*/
    uint32  l2err:1;        /*Recalculate the layer2 CRC*/
    uint32  l3err:1;        /*Recalculate the IP checksum*/
    uint32  pppoe:1;        /*Whether PPPoE header exists*/

    uint32  fwd:4;
    uint32  mir:4;
    uint32  mirorg:4;
    uint32  pri:3;          /*Priority*/
    uint32  rxrspan:1;      /*rspan tag exist*/
    uint32  miriutag:4;
    uint32  miroutag:4;
    uint32  stphy:5;
    uint32  rrcpType:2;     /*rrcp packet type*/
    uint32  rxcpuTag:1;     /*cpu tag exist when packet is rx from cpu port*/

    uint32  dpcnt:5;
    uint32  pnxt:11;
    uint32  reason:16;      /*cpu reason*/

    uint32  framelength:14; /*frame length,include CRC*/
    uint32  dp:2;           /*Drop precedence*/
    uint32  lb:1;           /*oam loopback*/
    uint32  occupy:1;       /*occupied or not*/
    uint32  sphy:5;         /*Source port number*/
    uint32  extraTag:1;     /*extra tag exist*/
    uint32  asdp:1;         /*Packet priority and drop precedence is assigned*/
    uint32  asdprmk:1;      /*Packet priority and drop precedence is assigned and need remarking*/
    uint32  evten:1;
    uint32  ttldec:1;       /*TTL decrease for routing */
    uint32  txcputag:1;     /*Insert CPU tag when packet is sent to CPU port*/
    uint32  dopri:3;        /*destination outer tag priority*/

    uint32  divid:12;       /*Destination Inner VID*/
    uint32  l2fmt:4;        /*Layer 2 format*/
    uint32  dovid:12;       /*Destination Outer VID*/
    uint32  dipri:3;
    uint32  l4err:1;        /*Recalculate the TCP, UDP, ICMP and IGMP checksum*/

    uint32  iutagStatus:29;
    uint32  dscprmk:1;      /*Weather DSCP remarking or not*/
    uint32  oam_lbsa:2;     /*oam loopback smac address operation*/

    uint32  ipuc:1;         /*IPv4 unicast routing*/
    uint32  ipmc:1;         /*ip multicast routing*/
    uint32  l34fmt:4;       /*Layer 3/4 format*/
    uint32  dmacidx:14;     /*When perform IPv4 unicast routing, this field point to the DMAC position stored in address hash table*/
    uint32  dscp:6;         /*DSCP*/
    uint32  ce:1;           /*Whether set CE bit in ip header to */
    uint32  rrcpRegData31_27:5; /*rrcp register data bit31-bit26*/

    uint32  outagStatus:29;
    uint32  rrcp:1;         /*rrcp packet*/
    uint32  oam_lbda:2;     /*oam loopback dmac address operation*/

    uint32  keeporigivid:1;
    uint32  keeporigovid:1;

    uint32  diprivalid:1;
    uint32  dcfivalid:1;
    uint32  dcfi:1;
    uint32  doprivalid:1;
    uint32  ddeivalid:1;
    uint32  ddei:1;
    uint32  spididxvalid:1;
    uint32  spididx:4;
    uint32  reserved:19;

    /*used for model code, not real ASIC field*/
    uint32  drop:1;         /*1:dropped by last PIE lookup phase*/
    uint32  curPortNum:5;   /*current port number*/
    uint32  egvidiuvalid:1;
    uint32  egvidouvalid:1;
    uint32  egvidkeepi:1;
    uint32  egvidkeepo:1;
};
typedef struct pmi_param_s pmi_param_t;

typedef struct hsa_8390_qid_s
{
    uint32 qid:3;   /* per port queue ID 0~7 */
} hsa_8390_qid_t;

typedef struct hsa_normal_qid_s
{
    uint32 qid:3;   /* per port queue ID 0~7 */
} hsa_normal_qid_t;

typedef struct hsa_stacking_qid_s
{
    uint32 qid:4;   /* stacking port queue ID 0~12 */
} hsa_stacking_qid_t;

typedef struct hsa_cpu_qid_s
{
    uint32 qid:5;   /* cpu port queue ID 0~31 */
} hsa_cpu_qid_t;

struct hsa_param_s
{
    union
    {
        struct
        {
            uint32 page_cnt:6;
            uint32 acl_mpls_hit:1;      /* Hit MPLS rule */
            uint32 acl_mpls_act:1;      /* 0=Push outer, 1=push inner & outer */
            uint32 mpls_index:9;        /* MPLS index, B[0]=is no used when push inner and outer */
            uint32 astagsts:1;          /* Decide whether processed Outer/Inner TAG(From CPUTAG)
                                            0: don't touch
                                            1: follow ALE decision */
            uint32 ctag_dm:1;           /* ETH-DM packet indicate:
                                            0b1: Notify Tx Modifier to fill the TxTimestamp field based-on OpCode
                                            0b0: This is NOT an ETH-DM packet */
            uint32 ctag_dgpkt:1;        /* Dying Gasp packet:
                                            0b1: It's a dying gasp packet
                                            0b0: It's not s dying gasp packet */
            uint32 tx_ptp_log:1;        /* PTP log function
                                            0b0: do not log the tx timestamp (normal)
                                            0b1: need to log the tx timestamp for this packet */
            uint32 tx_ptp_offload:2;    /* PTP offload mechanism
                                            0b00: None (disabled)
                                            0b01: Follow_Up (grandmaster)
                                            0b10: Follow_Up (transparent)
                                            0b11: Pdelay_Resp_Follow_Up (responder) */
            uint32 new_sa:1;            /* MAC constraint (learn limit) */
            uint32 mac_cst:1;           /* MAC constraint (learn limit) */
            uint32 l2_pmv:1;            /* L2 entry port move */
            uint32 atk_type:5;          /* Attack type */
            uint32 dm_rxidx:6;          /* (1) CFM packet : FIFO index for CFM DM packet
                                           (2) B[4:0] indicates the routing point ID, Total routing point={dm_rxidx[4:0],da_pid},
                                               B[5] is no used for routing point. The field is used for routing function only when the l3_route bit is 1 */
            uint32 acl_hit:1;           /* Hit ACL rule */
            uint32 acl_id:12;           /* ACL index */
            uint32 eav_class_b:1;       /* Incoming is EAV packet with class B */
            uint32 eav_class_a:1;       /* Incoming is EAV packet with class A */
            uint32 reason:5;            /* Reason for CPU */
            uint32 acl_tos:1;           /* Modify TOS field */
            uint32 new_tos:8;           /* New TOS field */
            uint32 internal_pri:3;      /* Internal priority, for egress remarking */
            uint32 opri_act:1;          /* Egress ACL force the priority of O-TAG or C2SC */
            uint32 ipri_act:1;          /* Egress ACL force the priority of I-TAG */
            uint32 c2sc_pri:1;          /* C2SC force inner priority */
            uint32 eacl_itag:1;         /* Egress ACL force inner tag status */
            uint32 eacl_otag:1;         /* Egress ACL force outer tag status */
            uint32 c2sc_itag:1;         /* C2SC force inner tag status */
            uint32 c2sc_otag:1;         /* C2SC force outer tag status */
            uint32 itag_status:2;       /* Tag status for inner VLAN. 0=untag, 1=tag,2=Keep format */
            uint32 otag_status:2;       /* Tag status for outer VLAN. 0=untag, 1=tag,2=Keep format */
            uint32 eacl_ivid:1;         /* Egress ACL force inner VLAN ID */
            uint32 c2sc_ivid:1;         /* C2SC force inner VLAN ID */
            uint32 eacl_otpid:1;        /* Egress ACL and C2SC force outer TPID, 1=force, 0=no */
            uint32 otpid_idx:2;         /* Index of Egress TPID(Combine result of Egress ACL and C2SC) */
            uint32 eacl_itpid:1;        /* Egress ACL force inner TPID, 1=force, 0=no */
            uint32 c2sc_itpid:1;        /* C2SC table force inner TPID, 1=force, 0=no */
            uint32 itpid_idx:2;         /* Index of Inner TPID(Combine result of ACL & C2SC) */
            uint32 new_otag:16;         /* New Outer tag (VLAN ID is combination result of Egressa ACL, C2SC, IACL, port-based) */
            uint32 new_itag:16;         /* New inner tag(VLAN ID is combinational result of ACL, C2SC, port-protocol) */
            uint32 ale_ovid:1;          /* ACL force outer VLAN ID */
            uint32 l3_route:1;          /* Do L3 routing function */
            uint32 da_pid:11;           /* Pointer ID for DA, L3 routing function */
            uint32 normal_fwd:4;        /* Send a normal packet for mirroring port */
            uint32 swap_mac:1;          /* Swap DA and SA for oam loopback */
            uint32 cpu_fwd:1;           /* Forwarding a normal packet to CPU */
            uint32 sflow_info:6;        /* TX sFlow sample information.
                                            0~53 Sampling for port-idxx
                                            62: Ingress sampling
                                            63: No sampling */
            uint32 mirror_1_info:6;     /* mirroring information for set 1
                                            0~53 Egress mirroring for port-Idxx
                                            62 : Ingress mirroring
                                            63 : no mirroring */
            uint32 mirror_2_info:6;     /* mirroring information for set 2
                                            0~53 Egress mirroring for port-Idxx
                                            62 : Ingress mirroring
                                            63 : no mirroring */
            uint32 mirror_3_info:6;     /* mirroring information for set 3
                                            0~53 Egress mirroring for port-Idxx
                                            62 : Ingress mirroring
                                            63 : no mirroring */
            uint32 mirror_4_info:6;     /* mirroring information for set 4
                                            0~53 Egress mirroring for port-Idxx
                                            62 : Ingress mirroring
                                            63 : no mirroring */
            hsa_8390_qid_t qid[53];     /* Queue ID per-port. 53 ports x 3bit */
            uint32 color:2;             /* Color of packet */
            uint32 rvid_sel:1;          /* The source of RVID, 1=RVID get from outer tag, 0= RVID get from inner tag */
            uint32 rvid:12;             /* Relayed VID */
            uint32 dpc:6;               /* destination copy counter(port counter) */
            rtk_portmask_t dpm;         /* destination port mask */
            uint32 endsc:12;            /* end page point */
            uint32 bgdsc:12;            /* start page point */
            uint32 stpid_idx:2;         /* original TPID index for outer tag */
            uint32 ctpid_idx:2;         /* original TPID index for inner tag */
            uint32 org_otag:16;         /* Outer VLAN Tag */
            uint32 org_itag:16;         /* Inner VLAN Tag */
            uint32 ompls_if:1;          /* Has outer MPLS interface */
            uint32 impls_if:1;          /* Has inner MPLS interface */
            uint32 org_etag_if:1;       /* Has Extra tag */
            uint32 org_cputag_if:1;     /* Has CPU Tag */
            uint32 org_otag_if:1;       /* Has outer VLAN Tag(Tag or priority tag) */
            uint32 org_itag_if:1;       /* Has inner VLAN Tag(Tag or priority tag) */
            uint32 ipv6:1;              /* IPv6 Type */
            uint32 llc_other:1;         /* LEN < 0x0600, {DSAP, SSAP, CTL, OUI} != 0xaaaa03_000000 */
            uint32 pppoe_if:1;          /* PPPoE packet */
            uint32 rfc1042:1;           /* LLC SNAP */
            uint32 spa:6;               /* Source Port ID */
            uint32 errpkt:1;            /* Error packet(CRC,Symbol) */
            uint32 ipv4:1;              /* IPv4 Type */
            uint32 pktlen:14;           /* Packet length (Include CRC) */
        } r8390;
        struct
        {
            uint32 cpu_tag_if:1;        /*has cpu tag*/
            uint32 astagsts:1;          /*cpu assign tag status*/
            uint32 ale_otag_hit:1;      /*ale hit outer tag status rule*/
            uint32 ale_itag_hit:1;      /*ale hit inner tag status rule*/
            uint32 ale_otag_sts:2;      /*ale outer tag status*/
            uint32 ale_itag_sts:2;      /*ale inner tag status*/
            uint32 org_otag_if:1;       /*incoming packet has outer tag*/
            uint32 org_itag_if:1;       /*incoming packet has inner tag*/
            uint32 org_ovid:12;         /*incoming packet's outer vlan vid*/
            uint32 org_opri:3;          /*incoming packet's outer vlan pri*/
            uint32 org_ocfi:1;          /*incoming packet's outer vlan cfi*/
            uint32 org_ivid:12;         /*incoming packet's inner vlan vid*/
            uint32 org_ipri:3;          /*incoming packet's inner vlan pri*/
            uint32 org_icfi:1;          /*incoming packet's inner vlan cfi*/
            uint32 ale_ovid:12;         /*outer vlan vid determined by ale*/
            uint32 ale_ivid:12;         /*inner vlan vid determined by ale*/
            uint32 fwd_base:1;          /*fwd base vid sel*/
            uint32 fwd_vid:12;          /*fwd vid*/
            uint32 acl_otpid_hit:1;     /*acl outer tpid rule hit*/
            uint32 acl_otpid_idx:2;     /*acl outer tpid rule index*/
            uint32 acl_itpid_hit:1;     /*acl inner tpid rule hit*/
            uint32 acl_itpid_idx:2;     /*acl inner tpid rule index*/
            uint32 org_otpid_idx:2;     /*org outer tpid index*/
            uint32 org_itpid_idx:2;     /*org inner tpid index*/
            uint32 ale_int_pri:3;       /*ale internal priority*/
            uint32 reason:6;            /*cpu tag: reason to cpu*/
            uint32 l2_pmv:1;            /*cpu tag: L2 entry port move*/
            uint32 new_sa:1;            /*cpu tag: new source mac address*/
            uint32 atk_type:5;          /*cpu tag: attack type*/
            uint32 atk_hit:1;           /*cpu tag: hit HW attack*/
            uint32 mac_cst:1;           /*cpu tag: mac constraint*/
            uint32 acl_idx_9_0:10;      /*cpu tag: acl index*/
            uint32 acl_idx_10:1;        /*cpu tag: acl index*/
            uint32 acl_hit:1;           /*cpu tag: acl hit*/
            uint32 etag:1;              /*has extra tag*/
            uint32 rfc1042:1;           /*has snap header*/
            uint32 pppoe:1;             /*has pppoe*/
            uint32 ipv4:1;              /*is ipv4 packet*/
            uint32 ipv6:1;              /*is ipv6 packet*/
            uint32 acl_rmk:7;           /*HSA_ACL_RMK[6:3] = 0b0010 not hit ACL remarking action
                                          HSA_ACL_RMK[6] = 1(dscp remarking value), HSA_ACL_RMK[5:0] is DSCP remarking value;
                                          HSA_ACL_RMK[5:0] is DSCP remarking value;
                                          HSA_ACL_RMK[6] = 0(other remarking action),
                                          HSA_ACL_RMK[5:3] is other remarking action,
                                          HSA_ACL_RMK[2:0] is other remarking action value
                                         */
            uint32 l3_routing_idx:9;    /*routing mac table*/
            uint32 l3_routing:1;        /*1:reg l3 routing*/
            uint32 ale_ovid_hit:1;      /*ale hit outer tag vid rule*/
            uint32 ale_ivid_hit:1;      /*ale hit inner tag vid rule*/
            uint32 sp:5;                /*source port*/
            uint32 pktlen:14;           /*pkt length*/
            uint32 dpc:5;               /*destination port number*/
            uint32 mir1_info:5;         /*mirror set 0 information:
                                            0~28:tx mirror
                                            29: rx mirror
                                            30: rspan mirror
                                            31: no mirror*/
            uint32 mir2_info:5;         /*mirror set 1 information*/
            uint32 mir3_info:5;         /*mirror set 2 information*/
            uint32 mir4_info:5;         /*mirror set 3 information*/
            uint32 mir_nor_fwd:4;       /*normal forward to mirroring port*/
            uint32 is_eav_a:1;          /*is eav a pkt*/
            uint32 is_eav_b:1;          /*is eav b pkt*/
            uint32 da_lk_hit:1;         /*da lookup hit*/
            uint32 acl_as_pri:1;        /*acl assign priority*/
            uint32 fst_dsc:11;          /*first dsc*/
            uint32 lst_dsc:11;          /*last dsc*/
            uint32 dp_bf_rxq:1;         /*pkt droped before RXQ*/
            uint32 dpm:29;              /*destination port mask*/
            uint32 inbw_pktlen:14;      /*pkt length for rxq/rxp rate control*/
            uint32 txqid_nrm:3;         /*tx qid for normal port*/
            uint32 txqid_cpu:3;         /*tx qid for cpu port*/
            uint32 rxqid:2;             /*rx qid*/
            uint32 cpu_as_pri:1;        /*cpu tag assign priority*/
            uint32 ale_ctag_hit:1;      /*ALE add cpu tag or not*/
            uint32 rspan_rm:1;          /*RSPAN TAG remove or not*/
        } r8380;
        struct
        {
            uint32 nor_cpu_sel:1;
            uint32 sflow_smpl_port:3;
            uint32 sflow_info:5;
            uint32 drop_rsn:7;          /* Drop reason */
            uint32 fst_dsc:12;          /* start page */
            uint32 lb_pkt:1;             /* Is loopback packet */
            uint32 meta_data:8;         /* meta data for loopback */
            uint32 lb_ttl:3;            /* loopback TTL(times) */
            hsa_normal_qid_t txqid_normal[24];   /* Queue ID per normal port */
            hsa_stacking_qid_t txqid_stk[4];     /* for stacking port 24-27 */
            hsa_cpu_qid_t txqid_cpu;            /* CPU QID */
            rtk_portmask_t dpm;         /* destination port mask */
            uint32 dpm_lb:1;            /* packet sent to loopback port */
            uint32 replicate_lst_cp:1;  /* IPMC last copy */
            uint32 replicate_fst_cp:1;  /* IPMC first copy */
            uint32 replicate_noe:1;     /* Pkt from non-replication queue */
            uint32 replicate_type:2;    /* IPMC packet type*/
            uint32 mir3_info:5;         /* mirroring information for set 3 */
            uint32 mir2_info:5;         /* mirroring information for set 2 */
            uint32 mir1_info:5;         /* mirroring information for set 1 */
            uint32 mir0_info:5;         /* mirroring information for set 0
                                            0~28: TX mirror
                                            29: RX mirror
                                            30: RSPAN mirror
                                            31: not mirror
                                        */
            uint32 mir_normal_fwd:4;    /* Normal forward to mirroring port */
            uint32 dy_gasp:1;           /* Is dying gasp packet */
            uint32 dpc:5;               /* How many destination port this packet has */
            uint32 page_cnt:6;          /* Page count */
            uint32 pkt_len:14;          /* Packet length (Include CRC) */
            uint32 lst_dsc:12;          /* end page */
            uint32 ipmc_qid:1;          /* IPMC Queue UD */
            uint32 ipmc_pkt:1;          /* Is IPMC packet */
            uint32 ip_rsvd_invert:1;    /* invert ip rsvd flag bit in ipv4 header */
            uint32 pmv_forbid:1;        /* Pmove is forbade */
            uint32 new_sa:1;            /* MAC constraint (learn limit) */
            uint32 l2_mac_cst:1;        /* MAC constraint (learn limit) */
            uint32 l2_dyn_pmv:1;        /* Dynamic L2 entry port move */
            uint32 l2_sttc_pmv:1;       /* Static L2 entry port move */
            uint32 l2_org_port:10;       /* Pmove origin port in L2 table */
            uint32 l2_org_trk_if:1;     /* Pmove orign port is trunk port */
            uint32 atk_type:5;          /* Attack packet type */
            uint32 l3_sa_if:1;          /* Is L3 route SA enabled */
            uint32 l3_da_if:1;          /* Is L3 route DA enabled */
            uint32 acl_hit:1;           /* Hit ACL rule */
            uint32 acl_idx:15;          /* ACL index */
            uint32 is_eav_a:1;          /* Incoming is EAV packet with class A */
            uint32 is_eav_b:1;          /* Incoming is EAV packet with class B */
            uint32 reason:6;            /* Reason for CPU */
            uint32 acl_rmk_dscp:1;       /* Modify dscp(include tos) field */
            uint32 new_dscp:8;           /* New DSCP(include TOS) field */
            uint32 int_pri:3;           /* Internal priority, for egress remarking */
            uint32 cpu_tagsts_as:1;          /* CPU Assign tag status,0=don't touch,1=ALE */
            uint32 iacl_itpid:1;         /* ivc , stacking,assign itpid */
            uint32 iacl_otpid:1;         /* ivc , stacking,assign otpid */
            uint32 eacl_itpid:1;         /* eacl assign otpid */
            uint32 eacl_otpid:1;         /* eacl assign otpid */
            uint32 otpid_idx:2;          /* Index of Egress outer TPID */
            uint32 itpid_idx:2;          /* Index of Egress Inner TPID */
            uint32 iacl_ipri_as:1;        /* iacl/ivc/stk modify iprio */
            uint32 iacl_opri_as:1;        /* iacl/ivc/stk modify iprio */
            uint32 eacl_ipri_as:1;        /* eacl modify iprio */
            uint32 eacl_opri_as:1;        /* eacl modify oprio */
            uint32 eacl_itag_status_as:1;         /* Egress ACL force inner tag status */
            uint32 eacl_otag_status_as:1;         /* Egress ACL force outer tag status */
            uint32 iacl_itag_status_as:1;         /* iacl/ivc/stk modify itag status */
            uint32 iacl_otag_status_as:1;         /* iacl/ivc/stk modify otag status */
            uint32 ale_itag_status:2;       /* Tag status for inner VLAN. 0=untag, 1=tag,2=Keep format */
            uint32 ale_otag_status:2;       /* Tag status for outer VLAN. 0=untag, 1=tag,2=Keep format */
            uint32 eacl_ivid_as:1;          /* egress acl , routing modify ivid */
            uint32 eacl_ovid_as:1;          /* egress acl , routing modify ivid */
            uint32 n_1_ivid_as:1;           /* n:1 modify ivid */
            uint32 n_1_ovid_as:1;           /* n:1 modify ovid */
            uint32 ale_otag_pri:3;          /* New Outer tag (VLAN ID is combination result of Egressa ACL, C2SC, IACL, port-based) */
            uint32 ale_otag_cfi:1;
            uint32 ale_otag_vid:12;
            uint32 ale_itag_pri:3;         /* New inner tag(VLAN ID is combinational result of ACL, C2SC, port-protocol) */
            uint32 ale_itag_cfi:1;
            uint32 ale_itag_vid:12;
            uint32 ttl_dec:1;           /* L3 route ttl */
            uint32 l2_hash_full:1;      /* Is LUT hash bucket full */
            uint32 invalid_sa:1;        /* Invalid SA */
            uint32 routed_pkt:1;        /* Routed packet*/
            uint32 cpu_fmt_en:1;        /* ACL trap/copy to cpu pkt format(original or modify) enable */
            uint32 cpu_fmt:1;           /* ACL trap/copy to cpu pkt format(original or modify) */
            uint32 l3_sa_idx:6;         /* Pointer ID for DA, L3 routing function */
            uint32 l3_da_idx:11;        /* Pointer ID for DA, L3 routing function */
            uint32 oam_swap_mac:1;      /* Swap DA and SA for oam loopback */
            uint32 dp:2;                /* Color of packet */
            uint32 fwd_base:1;          /* The source of RVID, 1=RVID get from outer tag, 0= RVID get from inner tag */
            uint32 fwd_vid:12;          /* Relayed VID */
            uint32 org_otpid_idx:2;     /* original TPID index for outer tag */
            uint32 org_itpid_idx:2;     /* original TPID index for inner tag */
            uint32 org_otag_pri:3;      /* Outer VLAN Tag B[15:13]=User-priority, B[12]=CFI, B[11:0]=VID */
            uint32 org_otag_cfi:1;
            uint32 org_otag_vid:12;
            uint32 org_itag_pri:3;      /* Inner VLAN Tag B[15:13]=User-priority, B[12]=CFI, B[11:0]=VID */
            uint32 org_itag_cfi:1;
            uint32 org_itag_vid:12;
            uint32 etag_if:1;           /* Has Extra tag */
            uint32 ctag_if:1;           /* Has CPU Tag */
            uint32 org_otag_if:1;       /* Has outer VLAN Tag(Tag or priority tag) */
            uint32 org_itag_if:1;       /* Has inner VLAN Tag(Tag or priority tag) */
            uint32 llc_other:1;         /* LEN < 0x0600, {DSAP, SSAP, CTL, OUI} != 0xaaaa03_000000 */
            uint32 pppoe_if:1;          /* PPPoE packet */
            uint32 rfc1042:1;           /* LLC SNAP */
            uint32 sphy:5;              /* Source Port ID */
            uint32 l3_err_pkt:1;        /* Is l3 checksum error packet */
            uint32 l2_err_pkt:1;        /* Error packet(CRC,Symbol) */
            uint32 ipv4_pkt:1;          /* IPv4 Type */
            uint32 ipv6_pkt:1;          /* IPv6 Type */

            /* for stackig */
            uint32 stk_src_unit:4;
            uint32 stk_spa:6;
            uint32 stk_spa_is_trk:1;
            uint32 stk_spa_trk_id:6;
            uint32 igr_intf_id:12;
            uint32 trk_hash:6;
            uint32 vb_iso_mbr:1;
            uint32 sa_lrn:1;
            uint32 fwd_type:2;
            uint32 dp_fmt:1;
            uint32 dp_info:12;
            uint32 da_hit:1;
            uint32 sp_fltr_en:1;

            rtk_portmask_t ctx_dpm;
            uint32 ctx_fwd_type:4;
            uint32 ctx_unit_id:4;
            uint32 ctx_sp_fltr_en:1;
            uint32 ctx_sp_is_trk:1;
            uint32 ctx_spn:10;
            uint32 ctx_fvid_en:1;
            uint32 ctx_qid_as:1;
            uint32 ctx_qid:5;
            uint32 ctx_bp_vlan_egr:1;
            uint32 ctx_bp_stp_egr:1;
            uint32 ctx_bp_fltr_egr:1;
            uint32 ctx_cngst_drop:1;
            uint32 ctx_acl_act:1;
            uint32 ctx_l3_act:1;

            uint32 stk_mir:1;
            uint32 miring_port_0:5;
            uint32 miring_port_1:5;
            uint32 miring_port_2:5;
            uint32 miring_port_3:5;
            uint32 stk_nor_fwd:4;
            uint32 cpu_fwd_type:4;
        }r9300;
        struct
        {
            uint32 hsb_eadr:13;
            uint32 pkt_len:14;
            uint32 pg_cnt:6;
            uint32 stack_if:1;
            uint32 cpu_tx_if:1;
            uint32 of_if:1;
            uint32 of_pipeline:1;
            uint32 Stack_fmt:2;
            uint32 ori_itag_if:1;
            uint32 ori_otag_if:1;
            uint32 ori_itag_vid:12;
            uint32 ori_itag_cfi:1;
            uint32 ori_itag_pri:3;
            uint32 ori_itag_tpid:2;
            uint32 ori_otag_vid:12;
            uint32 ori_otag_cfi:1;
            uint32 ori_otag_pri:3;
            uint32 ori_otag_tpid:2;
            uint32 class_a:1;
            uint32 class_b:1;
            uint32 pppoe_if:1;
            uint32 frame_type:2;
            uint32 evlan_if:1;
            uint32 ipv4_if:1;
            uint32 ipv6_if:1;
            uint32 org_ompls_if:1;
            uint32 org_impls_if:1;
            uint32 parser_cant_handle:1;
            uint32 IP6_EH_HOPBYHOP_ERR:1;
            uint32 IP6_EH_HOPBYHOP_EXIST:1;
            uint32 IP6_EH_ROUTE_EXIST:1;
            uint32 IP6_EH_FRAGMENT_EXIST:1;
            uint32 IP6_EH_ESP_EXIST:1;
            uint32 IP6_EH_AUTH_EXIST:1;
            uint32 IP6_EH_DESTINATION_EXIST:1;
            uint32 IP6_EH_MOBILITY_EXIST:1;
            uint32 IP6_EH_REPEAT:1;
            uint32 ip_prot:8;
            uint32 ip_offset_zero:1;
            uint32 ip_flag_set:1;
            uint32 l4_hdr_rdy:1;
            uint32 l4_offset:8;
            uint32 l2_err_pkt:1;
            uint32 l3_err_pkt:1;
            uint32 L3_offset:8;
            uint32 in_spn:6;
            uint32 spn:10;
            uint32 sp_trk:7;
            uint32 sp_is_trk:1;
            uint32 int_itag_hit:1;
            uint32 int_otag_hit:1;
            uint32 l2_crc:1;
            uint32 da_hit:1;
            uint32 fwd_vid_en:1;
            uint32 rma_pkt:1;
            uint32 meta_data:12;
            uint32 trk_hash:6;
            uint32 dp:2;
            uint32 internal_Priority:3;
            uint32 l3_intf_id:10;
            uint32 tt_idx:9;
            uint32 tt_hit:1;
            uint32 hsa_sel:1;
            uint32 hsa_sadr:13;
            uint32 hsb_sadr:13;
            rtk_portmask_t dpm;
            uint32 cp_cnt:6;
            uint32 stk_cpu_fwd:16;
            uint32 stk_nm_fwd:16;
            uint32 mirroring_info:24;
            uint32 mirrored_info:24;
            uint32 mirr_nfwd:4;
            uint32 local_cpu_fwd:1;
            uint32 sflow_mirroring:6;
            uint32 sflow_mirrored:6;
            uint32 dm_pkt:1;
            uint32 dg_pkt:1;
            uint32 tx_mir_cancel:1;
            uint32 vb_iso_mbr:1;
            uint32 learn_en:1;
            uint32 fwd_vid_sel:1;
            uint32 fwd_type:2;
            uint32 RVID:12;
            uint32 local_reason:6;
            uint32 remote_reason:6;
            uint32 fwd_port:12;
            uint32 cpu_tx_fwd_type:4;
            uint32 ctx_dpm_55_32:24;
            uint32 ctx_dpm_31_0;
            uint32 sw_unit:4;
            uint32 spf_spn:10;
            uint32 spf_is_trk:1;
            uint32 src_filter_en:1;
            uint32 as_qid:1;
            uint32 ale_as_tagsts:1;
            uint32 bp_vlan_egr:1;
            uint32 bp_stp:1;
            uint32 bp_fltr:1;
            uint32 cngst_drop:1;
            uint32 acl_act:1;
            uint32 stk_mir_hit:2;
            uint32 set_ip_rsv:1;
            uint32 igr_l3_intf_type:1;
            uint32 ts_req:1;
            uint32 mpls_encap_req:1;
            uint32 ts_index:9;
            uint32 bssid_idx:12;
            uint32 mpls_encap_idx:11;
            uint32 mpls_decap_req:1;
            uint32 ip_ttl:8;
            uint32 HSA_N21_IPRI_HIT:1;
            uint32 HSA_INT_DSCP:8;
            uint32 HSA_INT_DSCP_HIT:1;
            uint32 mpls_decap_sta:2;
            uint32 HSA_N21_IPRI:3;
            uint32 HSA_N21_OPRI:3;
            uint32 HSA_N21_OPRI_HIT:1;
            uint32 mpls_tc:3;
            uint32 HSA_INT_ITAG_IF:1;
            uint32 SURF_OVID:12;
            uint32 HSA_ITAG_PRI:3;
            uint32 HSA_EACL_IPRI_HIT:1;
            uint32 HSA_INT_OTAG_VID:12;
            uint32 HSA_IACL_IPRI_HIT:1;
            uint32 SURF_IVID:12;
            uint32 HSA_OTAG_PRI:3;
            uint32 EACL_IVID_HIT:1;
            uint32 HSA_INT_ITAG_VID:12;
            uint32 HSA_EACL_OPRI_HIT:1;
            uint32 HSA_IACL_OPRI_HIT:1;
            uint32 ACL_OTPID_IDX:2;
            uint32 HSA_INT_OTAG_CFI:1;
            uint32 HSA_INT_ITAG_CFI:1;
            uint32 vrf_id:8;
            uint32 l3_mdf_req:1;
            uint32 oam_lbk:1;
            uint32 N21_IVID_HIT:1;
            uint32 fwd_p_type:1;
            uint32 EACL_OTAG_HIT:1;
            uint32 EACL_OTAG_IF:1;
            uint32 HSA_INT_OTAG_IF:1;
            uint32 EACL_ITAG_HIT:1;
            uint32 EACL_ITAG_IF:1;
            uint32 EACL_OVID_HIT:1;
            uint32 N21_OVID_HIT:1;
            uint32 IACL_OTPID_HIT:1;
            uint32 IACL_ITPID_HIT:1;
            uint32 EACL_OTPID_HIT:1;
            uint32 EACL_ITPID_HIT:1;
            uint32 ACL_ITPID_IDX:2;
            uint32 port_data:10;
            uint32 port_is_trk:1;
            uint32 invalid_sa:1;
            uint32 hash_full:1;
            uint32 l2_dyn_pmv:1;
            uint32 l2_sttc_pmv:1;
            uint32 pmv_forbid:1;
            uint32 new_sa:1;
            uint32 dm_rxidx:6;
            uint32 mac_cst:1;
            uint32 acl_idx:15;
            uint32 atk_type:5;
            uint32 cpu_rx_l2_err:1;
            uint32 cpu_rx_l3_err:1;
            uint32 cpu_rx_sflow:2;
            uint32 of_tbl_id:2;
            uint32 acl_of_hit:2;
            uint32 of_miss_tbl_id:2;
            uint8  dmac[6];
            uint8  smac[6];
            hsa_normal_qid_t qid[52];
            hsa_stacking_qid_t stack_qid[4];
            hsa_cpu_qid_t cpu_qid;

            uint32 EST_MATCH:1;
            uint32 DST_PE_ECID:22;
            uint32 PE_LU_MODE:1;
            uint32 PE_NSG:8;
            uint32 INT_PE_ECID_EXT:8;
            uint32 ORI_PE_ECID_EXT:8;
            uint32 ORI_PE_IGR_ECID_EXT:8;
            uint32 ORI_PE_ECID_BASE:12;
            uint32 ORI_PE_ECID_GRP:2;
            uint32 ORI_PE_ETAG_RSVD:2;
            uint32 ORI_PE_IGR_ECID_BASE:12;
            uint32 ORI_PE_ETAG_DEI:1;
            uint32 ORI_PE_ETAG_PRI:3;
            uint32 ORI_PE_ETAG_IF:1;
        } r9310;
        struct
        {
            uint32 hsb_eadr:13;
            uint32 pkt_len:14;
            uint32 pg_cnt:6;
            uint32 stack_if:1;
            uint32 cpu_tx_if:1;
            uint32 of_if:1;
            uint32 of_pipeline:1;
            uint32 stack_fmt:2;
            uint32 ori_itag_if:1;
            uint32 ori_otag_if:1;
            uint32 ori_itag_vid:12;
            uint32 ori_itag_cfi:1;
            uint32 ori_itag_pri:3;
            uint32 ori_itag_tpid:2;
            uint32 ori_otag_vid:12;
            uint32 ori_otag_cfi:1;
            uint32 ori_otag_pri:3;
            uint32 ori_otag_tpid:2;
            uint32 class_a:1;
            uint32 class_b:1;
            uint32 pppoe_if:1;
            uint32 frame_type:2;
            uint32 evlan_if:1;
            uint32 ipv4_if:1;
            uint32 ipv6_if:1;
            uint32 org_ompls_if:1;
            uint32 org_impls_if:1;
            uint32 parser_cant_handle:1;
            uint32 ip6_eh_hopbyhop_err:1;
            uint32 ip6_eh_hopbyhop_exist:1;
            uint32 ip6_eh_route_exist:1;
            uint32 ip6_eh_fragment_exist:1;
            uint32 ip6_eh_esp_exist:1;
            uint32 ip6_eh_auth_exist:1;
            uint32 ip6_eh_destination_exist:1;
            uint32 ip6_eh_mobility_exist:1;
            uint32 ip6_eh_repeat:1;
            uint32 ip_prot:8;
            uint32 ip_offset_zero:1;
            uint32 ip_flag_set:1;
            uint32 l4_hdr_rdy:1;
            uint32 l4_offset:8;
            uint32 l2_err_pkt:1;
            uint32 l3_err_pkt:1;
            uint32 l3_offset:8;
            uint32 in_spn:6;
            uint32 spn:10;
            uint32 sp_trk:7;
            uint32 sp_is_trk:1;
            uint32 loopback_time:6;
            uint32 meta_data:12;
            uint32 trk_hash:6;
            uint32 dp:2;
            uint32 internal_priority:3;
            uint32 l3_intf_id:10;
            uint32 tt_idx:9;
            uint32 tt_hit:1;
            uint32 hsa_sel:1;
            uint32 hsa_sadr:13;
            uint32 hsb_sadr:13;
            rtk_portmask_t dpm;
            uint32 cp_cnt:6;
            uint32 stk_cpu_fwd:16;
            uint32 stk_nm_fwd:16;
            uint32 mirroring_info:24;
            uint32 mirrored_info:24;
            uint32 mirr_nfwd:4;
            uint32 local_cpu_fwd:1;
            uint32 sflow_mirroring:6;
            uint32 sflow_mirrored:6;
            uint32 dm_pkt:1;
            uint32 dg_pkt:1;
            uint32 tx_mir_cancel:1;
            uint32 vb_iso_mbr:1;
            uint32 learn_en:1;
            uint32 fwd_vid_sel:1;
            uint32 fwd_type:2;
            uint32 rvid:12;
            uint32 local_reason:6;
            uint32 remote_reason:6;
            uint32 output_data:12;
            uint32 output_type:4;
            uint32 group_idx:11;
            uint32 group:1;
            uint32 queue_data:3;
            uint32 set_queue_data:1;
            uint32 field_l4_dport:16;
            uint32 set_field_l4_dport:1;
            uint32 field_l4_sport:16;
            uint32 set_field_l4_sport:1;
            uint32 field_ip_dip;
            uint32 set_ip_rsv:1;
            uint32 l2mpls_tt:1;
            uint32 set_field_ip_dip:1;
            uint32 field_ip_sip:32;
            uint32 set_field_ip_sip:1;
            uint32 l3_ttl:8;
            uint32 set_field_ip_ttl:1;
            uint32 field_ip_dscp:6;
            uint32 set_field_ip_dscp:1;
            uint32 field_mpls_ttl:8;
            uint32 set_field_mpls_ttl:1;
            uint32 field_mpls_tc:3;
            uint32 set_field_mpls_tc:1;
            uint32 field_mpls_label:11;
            uint32 set_field_mpls_label:1;
            uint32 field_pcp:3;
            uint32 set_field_pcp:1;
            uint32 field_vid:12;
            uint32 set_field_vid:1;
            uint32 field_da:16;
            uint32 set_field_da:1;
            uint32 field_sa:10;
            uint32 set_field_sa:1;
            uint32 dec_ip_ttl:1;
            uint32 dec_mpls_ttl:1;
            uint32 copy_ttl_outward:1;
            uint32 push_vlan_ethtype:2;
            uint32 push_vlan:1;
            uint32 push_mpls_ethtype:1;
            uint32 push_mpls_lib_idx:11;
            uint32 push_mpls_vpn_type:1;
            uint32 push_mpls_mode:1;
            uint32 push_mpls:1;
            uint32 pop_mpls_type:1;
            uint32 pop_mpls:1;
            uint32 pop_vlan:1;
            uint32 copy_ttl_inward:1;
            uint32 to_of:1;
            uint32 apply_action:1;
            uint32 of_lookup_req:1;
            uint32 next_tab:2;
            uint32 field_ip_ttl:8;
            uint32 pkt_dscp:6;
            uint32 lb_des_time:6;
            uint32 mac_cst:1;
            uint32 acl_idx:15;
            uint32 group_hash:3;
            uint32 reserved:2;
            uint32 cpu_rx_l2_err:1;
            uint32 cpu_rx_l3_err:1;
            uint32 cpu_rx_sflow:2;
            uint32 of_tbl_id:2;
            uint32 acl_of_hit:2;
            uint32 of_miss_tbl_id:2;
            uint8  dmac[6];
            uint8  smac[6];
            hsa_normal_qid_t qid[52];
            hsa_stacking_qid_t stack_qid[4];
            hsa_cpu_qid_t cpu_qid;        } r9310_hsa_of;
    };
};
typedef struct hsa_param_s hsa_param_t;

struct hsm_pre_8390_s
{
    uint32 rma_byp_vlan:1; /* RMA bypass VLAN(Ingress filter, VLAN error, Frame type, CFI) */
    uint32 acl_bypass:3;   /* ACL bypass Ingress bandwidth , stp, ingress/egress drop
                              B[0] : bypass ingress bandwidth & Storm
                              B[1] : STP source check
                              B[2] : ingress/Egress check" */
    uint32 pppoe_ef:1;      /* Has PPPOE header */
    uint32 cmac:1;          /* DA is CPU MAC & VID=CPU VID */
    uint32 gmac:1;          /* DA is me */
    uint32 ipv4_hdr_err:1;  /* IPv4 Header error */
    uint32 ipv4_ttl_zero:1; /* IPv4 & TTL is zero */
    uint32 ipv4_opt:1;      /* IPv4 has opertion header */
    uint32 ipv6_hdr_err:1;  /* IPv6 header error */
    uint32 ipv6_hl_zero:1;  /* IPv6 & TTL=0 */
    uint32 ipv6_hop:1;      /* IPv6 with hop-by-hop header */
    uint32 oam_act:2;       /* OAM actionm0=FWD,1=Trap,2=drop,3=LB */
    uint32 stp:2;           /* Source port port state */
    uint32 vlan_zero:1;     /* VLAN member set is all zero */
    uint32 vlan_spm:1;      /* Source port is member of VLAN */
    uint32 unkey:1;         /* Unicast key type */
    uint32 mckey:1;         /* Multicast key type */
    uint32 vlan_profile:3;  /* VLAN proile */
    uint32 fid:8;           /* VLAN FID */
    uint32 meter_drop:1;    /* Drop by meter; 1=drop, 0=pass */
    uint32 cfm_act:2;       /* CFM action; 0=drop, 1trap, 2=FWD */
    uint32 cfm_idx:6;       /* CFM index */
    uint32 cfi_act:2;       /* CFI action; 0=fwd, 1=drop, 2=trap, 3=no used */
    uint32 tag_act:1;       /* TAG type action, 0=pass, 1=drop */
    uint32 nivid:12;        /* New inner VLAN ID */
    uint32 novid:12;        /* New outer VLAN ID */
    uint32 rvid:12;         /* Relay VLAN ID */
    uint32 acl_meet:1;      /* Hit ACL Forwarding/IVID/OVID action */
    uint32 aclc2sc:1;       /* Default action if the packet does not hit ACL & C2SC rule; 0=FWD,1=Drop */
    uint32 acl_hit:3;       /* ACL hit ; B[2]=MPLS, B[1]=FWD act, B[0]=Mirror Act */
    uint32 acl_idx:12;      /* Hit the index of ACL FWD action */
    uint32 acl_data:29;     /* B[28]=MPLS act
                               B[27:19]=Label Information Base entry index
                               B[18:16]=Forwarding action
                               B[15:2]=Forwarding information
                               B[1:0]=Mirror index */
    uint32 org_ivif_if:1;   /* Has inner VLAN tag */
    uint32 org_ivid:12;     /* Inner VLAN Tag ID */
    uint32 spe_trap_rsn:3;  /* Special trap reason; IGMP=1, MDL=2, EAPOL=3, ARP request=4, IPv6 neighbor=5 */
    uint32 spe_trap_act:1;  /* Special packet trap action, 0=no act(fwd), 1=trap */
    uint32 eav_classa:1;    /* Is EAV & Class A packet */
    uint32 eav_classb:1;    /* Is EAV & Class B packet */
    uint32 hwatt_rsn:5;     /* trap reason of HW attack */
    uint32 hwatt_act:2;     /* HW attack action; 0=normal, 1=drop,2=trap */
    uint32 rma_pkt:1;       /* Is RMA packet */
    uint32 rma_act:2;       /* Action for RMA packet; 0=table lookup,1=Drop,2=Trap */
    uint32 rma_learn:1;     /* Enable learning for RMA packet */
    uint32 rma_bypass:1;    /* Bypass Blocking/listening state for RMA packet */
    uint32 c2sc_hit_mode:1; /* C2SC mode, 0=VLAN, 1=MAC */
    uint32 c2sc_hit:1;      /* Hit C2SC table */
    uint32 c2sc_data:25;    /* C2SC data */
    uint32 internal_prio:3; /* Internal priority */
    uint32 dp_pre:2;        /* Drop precedence/color */
    uint32 protocol_storm:1;/* Drop by protocol-based storm */
    uint32 ibw_pass:1;      /* Pass by input bandwidth controller */
    uint32 slid:6;          /* Source logical port ID */
    uint32 trunk_grp:5;     /* Source port trunk information
                               B[4]=Source port belong to a trunk group
                               B[3:0]=Trunk group ID */
    uint32 sip:32;          /* Source IP address */
    uint32 dip:32;          /* Destination IP address */
    uint32 ipv6:1;          /* Is IPv6 packet */
    uint32 ipv4:1;          /* Is IPv4 packet */
    uint8  smac[6];         /* Source MAC address */
    uint8  dmac[6];         /* Destination MAC address */
    uint32 ctag_ln:1;       /* enable to learn SA by CTAG  */
    uint32 ctag_if:1;       /* Has CPU tag */
    uint32 pktlen:14;       /* Packet length (Include CRC) */

} ;
typedef struct hsm_pre_8390_s hsm_pre_8390_t;

struct hsm_lu_8390_s
{
    uint32 rma_byp_vlan:1; /* RMA bypass VLAN(Ingress filter, VLAN error, Frame type, CFI) */
    uint32 acl_bypass:3;   /* ACL bypass Ingress bandwidth , stp, ingress/egress drop
                              B[0] : bypass ingress bandwidth & Storm
                              B[1] : STP source check
                              B[2] : ingress/Egress check" */


    uint32 dpmove_act:2;        /* Dynamic port-moving action;0=fwd,1=drop,2=trap,3=copy */
    uint32 sport_filter_en:1;   /* Enable source port filter function */
    uint32 gmac_err:1;          /* L3 routing flow & DA!=Me */
    uint32 mc_sa:1;             /* SA is MC */
    uint32 vlan_spfilter:2;     /* VLAN source port filter cfg; 0=FWD,1=drop,2=Trap */
    rtk_portmask_t dpm;         /* destination port mask */
    uint32 lookup_type:2;       /* Lookup type; 0=UC, 1=L2 MC, 2=IPMC, 3=L3 routing */
    uint32 fwd_type:2;          /* Forwading type, 0=UC, 1=MC, 2=IPv4 MC, 3=IPv6 MC */
    uint32 newsa_ac:2;          /* Action of new SA; 0=FWD,1=drop,2=trap,3=Copy */
    uint32 pmove_act:2;         /* port-move Action for static entry;0=FWD,1=drop,2=trap,3=copy */
    uint32 maclimit_act:2;      /* mac limit action, 0=fwd,1=drop,2=trap,3=copy */
    uint32 route_data:30;       /* Routing information
                                    B[29:19]=DA index
                                    B[18]=Select replaced VLAN
                                    B[17:6]=New VID
                                    B[5:0]=Destination port ID */
    uint32 da_hit:1;            /* DA exist in BCAM or SRAM */
    uint32 da_data:26;          /* UC DA information in memory
                                    B[25:20] = Port ID
                                    B[19:17]=Aging value
                                    B[16]=SA blocking
                                    B[15]=DA blocking
                                    B[14]=static
                                    B[13]=Suppression
                                    B[12]=Next Host
                                    B[11:0]=inner VID
                                    MC DA information format
                                    B[25:14]=0
                                    B[13:2]=MC index
                                    B[1:0]=Reserved
                                    IPv4MC & DIP+SIP format
                                    B[25:14]=FRVID
                                    B[13:2]=MC index
                                    B[1:0]=reserved */
    uint32 sa_hit:1;            /* SA exist in BCAM or SRAM */
    uint32 sa_data:26;          /* "SA information in memoryB[12:7] = Port ID
                                    B[6:4]=Aging value
                                    B[3]=SA blocking
                                    B[2]=DA blocking
                                    B[1]=static
                                    B[0]=Suppression" */
    uint32 cmac:1;          /* DA is CPU MAC & VID=CPU VID */
    uint32 gmac:1;          /* DA is me */
    uint32 ipv4_hdr_err:1;  /* IPv4 Header error */
    uint32 ipv4_opt:1;      /* IPv4 has opertion header */
    uint32 ipv4_ttl_zero:1; /* IPv4 & TTL is zero */
    uint32 ipv6_hdr_err:1;  /* IPv6 header error */
    uint32 ipv6_hl_zero:1;  /* IPv6 & TTL=0 */
    uint32 ipv6_hop:1;      /* IPv6 with hop-by-hop header */
    uint32 oam_act:2;       /* OAM actionm0=FWD,1=Trap,2=drop,3=LB */
    uint32 stp:2;           /* Source port port state */
    uint32 vlan_zero:1;     /* VLAN member set is all zero */
    uint32 vlan_spm:1;      /* Source port is member of VLAN */
    uint32 fid:8;           /* VLAN profile */
    uint32 meter_drop:1;    /* Drop by meter; 1=drop, 0=pass */
    uint32 cfm_act:2;       /* CFM action; 0=drop, 1trap, 2=FWD */
    uint32 cfm_idx:6;       /* CFM index */
    uint32 cfi_act:2;       /* CFI action; 0=fwd, 1=drop, 2=trap, 3=no used */
    uint32 tag_act:1;       /* TAG type action, 0=pass, 1=drop */
    uint32 nivid:12;        /* New inner VLAN ID */
    uint32 novid:12;        /* New outer VLAN ID */
    uint32 rvid:12;         /* Relay VLAN ID */
    uint32 acl_meet:1;      /* Hit ACL Forwarding/IVID/OVID action */
    uint32 aclc2sc:1;       /* Default action if the packet does not hit ACL & C2SC rule; 0=FWD,1=Drop */
    uint32 acl_hit:3;       /* ACL hit ; B[2]=MPLS, B[1]=FWD act, B[0]=Mirror Act */
    uint32 acl_idx:12;      /* Hit the index of ACL FWD action */
    uint32 acl_data:29;     /* B[28]=MPLS act
                               B[27:19]=Label Information Base entry index
                               B[18:16]=Forwarding action
                               B[15:2]=Forwarding information
                               B[1:0]=Mirror index */

    uint32 spe_trap_rsn:3;  /* Special trap reason; IGMP=1, MDL=2, EAPOL=3, ARP request=4, IPv6 neighbor=5 */
    uint32 spe_trap_act:1;  /* Special packet trap action, 0=no act(fwd), 1=trap */
    uint32 eav_classa:1;    /* Is EAV & Class A packet */
    uint32 eav_classb:1;    /* Is EAV & Class B packet */
    uint32 hwatt_rsn:5;     /* trap reason of HW attack */
    uint32 hwatt_act:2;     /* HW attack action; 0=normal, 1=drop,2=trap */
    uint32 rma_pkt:1;       /* Is RMA packet */
    uint32 rma_act:2;       /* Action for RMA packet; 0=table lookup,1=Drop,2=Trap */
    uint32 rma_bypass:1;    /* Bypass Blocking/listening state for RMA packet */
    uint32 c2sc_hit_mode:1; /* C2SC mode, 0=VLAN, 1=MAC */
    uint32 c2sc_hit:1;      /* Hit C2SC table */
    uint32 c2sc_data:25;    /* C2SC data */
    uint32 internal_prio:3; /* Internal priority */
    uint32 dp_pre:2;        /* Drop precedence/color */
    uint32 protocol_storm:1;/* Drop by protocol-based storm */
    uint32 ibw_pass:1;      /* Pass by input bandwidth controller */
    uint32 slid:6;          /* Source logical port ID */
    uint32 trunk_grp:5;     /* Source port trunk information
                               B[4]=Source port belong to a trunk group
                               B[3:0]=Trunk group ID */
} ;
typedef struct hsm_lu_8390_s hsm_lu_8390_t;


struct hsm_pre_9300_s
{
    uint32 st_rt_if:1;          /* stacking header route_tunnel_if field */
    uint32 lut_haddr1:6;        /* trunk hash addr1 for lut known uc pkt */
    uint32 lut_haddr0:6;        /* trunk hash addr0 for lut known uc pkt */
    uint64 route_act:62;        /* route info
                                   for multicast routing
                                       proc_id[2:0]:
                                       event[12:3]: bit[9]trap/copy to loacl or master,bit[8:7](1-trap 2-copy 3-drop 4-fwd), bit[6:0] reason
                                       mcrt_rpf_asst[13]: multicast rpf fail assert action
                                       mcrt_oil_idx[22:14]: oil index
                                       mcrt_oil_en[23]: oil enable for mcast.
                                       mc_l3_hit[24] for mcast. URPF_EN[23]: urpf enable(cfg urpf enable, check pkt type pass) for ucast
                                       pbr[25]: acl pbr
                                       mactb_hit[26]: hit mactable
                                       hittcam[27]: hit prefix table, for egress acl template
                                       hitram[28]: hit host table,for egress acl template
                                       l3_en[29]:l3 enable
                                       stk_pmsk[33:30]
                                       mc_l2_hit[34]:
                                       mc_l2_fwdidx[44:35]: l2 forward index
                                       dmac_idx[61:47]: lit table index
                                   for unicast routing
                                       proc_id[2:0]:
                                       event[12:3]: bit[9]trap/copy to loacl or master,bit[8:7](1-trap 2-copy 3-drop 4-fwd), bit[6:0] reason                                       ucrt_ttl_dec[12]: ttl decrease. Dec TTL
                                       ucrt_ttl_del[13]: ttl decrease. Dec TTL
                                       ucrt_ttl_chk[14]: ttl check. Check ttl
                                       null_intf_trap[15]: invalid dmac index trap
                                       null_intf_drop[16]: invalid dmac index drop
                                       ucrt_otg_idx[23:17]: urpf index(urpf lookup outgoing interface index,
                                                                       to get vid for strict URPF mode) only need vid
                                       urpf_en[24]: urpf enable(cfg urpf enable, check pkt type pass) for ucast
                                       pbr[25]: acl pbr
                                       mactb_hit[26]: hit mactable
                                       hittcam[27]: hit prefix table, for egress acl template
                                       hitram[28]: hit host table,for egress acl template
                                       urpf_act[31:29]: urpf action (urpf action when urpf fail)
                                       urpf_mode[32]: urpf mode(0-loose mode, 1-strict mode)
                                       urpf_hit[33]: urpf hit (hit, dfLt rt check pass,~dst null) &(~(STRICT & NXHIDX ==7FFF))
                                       [34] resverd
                                       urpf_idx[41:35]: urpf index (urpf lookup outgoing interface index,
                                                                    to get vid for strict URPF mode) only need vid
                                       [46:42]reserved
                                       dmac_idx[61:47]: lit table index
                                */
    uint32 sptrsn:9;            /* Special packet trap reason ([1:0]igmp/mld, [3:2]dhcp/dhcpv6, [5:4]arp_rep/arp_req)
                                   (001-trap local/010-copy local/011-trap master/100-copy master) */
    uint32 ipv6_pkt:1;          /* ipv6 pkt. From HSB */
    uint32 ipv4_pkt:1;          /* ipv4 pkt. From HSB */
    uint32 igr_vlan_mbr:29;     /* ingress vlan member */
    uint32 macMis_act:12;
    uint32 ip_rsv_act:3;        /* ip reserved pkt action (000-fwd/001-drop/010-trap local/011-copy local/100-trap master/101-copy master) */
    uint32 rip_if:1;            /* rip pkt */
    uint32 dhcp_if:1;           /* dhcp pkt */
    uint32 rtag_if:1;           /* rspan pkt */
    uint32 rma_rsn:7;           /* rma reason */
    uint32 rma_usr_pkt:1;       /* rma user define pkt */
    uint32 rma_lldp_pkt:1;      /* lldp pkt */
    uint32 rma_eapol_pkt:1;     /* eapol pkt */
    uint32 err_pkt:1;           /* error pkt */
    uint32 rma_byp_vlan_fltr:1; /* rma bypass vlan ingress filter */
    uint32 iacl_metadata:8;    /* iacl meta action */
    uint32 iacl_byp_act:4;      /* iacl bypass action */
    uint32 oam_dis_lrn:1;       /* oam pkt disable sa learn */
    uint32 oam_act:2;           /* oam pkt action(00-fwd, 01-trap, 10-drop, 11-lb) */
    uint32 igr_stp_status:2;    /* spanding tree status(00-dis, 01-block, 10-learn, 11-fwd)*/
    uint32 rx_port_in_vlan:1;   /* Source port in VLAN member */
    uint32 ucast_hkey:1;        /* vlan table uc key */
    uint32 mcast_hkey:1;        /* vlan table mc key*/
    uint32 vlan_prof:3;         /* vlan table profile */
    uint32 msti:6;              /* vlan table mst intance id */
    uint32 vlan_group_mask:8;   /* vlan table group bit */
    uint32 iacl_meter_drop:1;   /* Drop by meter; 1=drop, 0=pass */
    uint32 vlan_cfi_act:2;      /* CFI action; 0=fwd, 1=drop, 2=trap, 3=no used */
    uint32 vlan_aft_drop:1;     /* i/otag accept drop */
    uint32 igr_ivid:12;         /* internal ivid */
    uint32 igr_ovid:12;         /* internal ovid */
    uint32 fwd_vid:12;          /* fwd VLAN ID */
    uint32 fwd_base:1;          /* fwd vid select (1-internal ovid, 0-internal ivid) */
    uint32 ivc_drop:1;          /* c2sc drop */




    uint32 acl_hit:9;           /* ACL action hit bits
                                    actr_hiprsv[0]: IP Rsvd flag invert
                                    actr_hmir[1]: Mirror
                                    actr_hfwd[2]: Forward
                                    actr_hdrp[3]: Drop
                                    actr_htag[4]: Tag status
                                    actr_hopri[5]: OPRI
                                    actr_hipri[6]: IPRI
                                    actr_hrmk[7]: Remark
                                    actr_hqid[8]: Qid
                                */
    uint32 acl_idx:11;          /* acl hit index */
    uint64 acl_act:52;          /* acl action do
                                    actr_mirror[3:0]:
                                    actr_fwd[21:4]:
                                    actr_drp[22]:
                                    actr_tag[26:23]:
                                    actr_opri[31:27]:
                                    actr_ipri[36:32]:
                                    actr_rmk[46:37]:
                                    actr_qid[51:47]:
                                */
    uint32 org_otag_if:1;       /* otag if. From HSB */
    uint32 org_ovid:12;         /* otag vid. From HSB */
    uint32 org_itag_if:1;       /* itag if. From HSB */
    uint32 org_ivid:12;         /* itag vid. From HSB */
    uint32 capwap_trap:1;       /* Special packet trap */
    uint32 eav_class_a:1;       /* Is EAV & Class A packet */
    uint32 eav_class_b:1;       /* Is EAV & Class B packet */
    uint32 atk_type:5;          /* attack reason */
    uint32 atk_act:2;           /* attack action */
    uint32 atk_hit:1;           /* attack pkt hit */
    uint32 grap_act:2;          /* RMA packet bypass new sa drop (00-fwd, 01-drop, 10-trap, 11-copy) */
    uint32 rma_byp_new_sa_drop:1;   /* rma pkt bypass new sa drop */
    uint32 rma_pkt:1;           /* rma pkt */
    uint32 rma_act:3;           /* rma pkt action */
    uint32 rma_lrn:1;           /* rma pkt sa learn enable */
    uint32 rma_byp_stp:1;       /* rma pkt bypass stp */
    uint32 ivc_mode:1;          /* 1: mac&ip-sub based; 0: ivc */
    uint32 ivc_hit:1;           /* c2sc hit */
                                /* c2sc data
                                    for ivc
                                        c2scact_ipri[2:0]: New inner priority (active when c2scact_aipri = 1)
                                        c2scact_aipri[3]: New inner priority (Assign inner priority (PCP) value)
                                        c2scact_itag[4]: New inner tag status (active when c2scact_aitag = 1)
                                        c2scact_aitag[5]: Assign the I-TAG status
                                        c2scact_itpid[7:6]: Index of the I-TPID (active when c2scact_aitpid = 1)
                                        c2scact_aitpid[8]: Assign I-TPID
                                        c2scact_opri[11:9]: New outer priority (active when c2scact_aopri = 1)
                                        c2scact_aopri[13:12]: Assign outer priority (PCP) value
                                        c2scact_otag[14]: New inner tag status (active when c2scact_aotag = 1)
                                        c2scact_aotag[15]: Assign the O-TAG status
                                        c2scact_itpid[17:16]: Index of the I-TPID (active when c2scact_aitpid = 1)
                                        c2scact_aitpid[18]: Assign the O-TAG status
                                    for mac-based
                                        c2scact_pri[2:0]: New tag priority (active when PRI_ASSIGN = 1)
                                        c2scact_pri[3]: Assign tag priority (PCP) value
                                        c2scact_tag[4]: New tag status (active when c2scact_atag = 1)
                                        c2scact_atag[5]: Assign the TAG status
                                        c2scact_tpid[7:6]: Index of the TPID (active when c2scact_atpid = 1)
                                        c2scact_atpid[8]: Assign TPID according to VLAN_TYPE
                                        c2scact_type[9]: To indicate the VLAN type
                                        c2scact_ignore[10]: Indicate the packet should ignore Ingress VLAN Filtering
                                        c2scact_fwdact[12:11]: Assign the forwarding action
                                */
    uint32 c2scact_ipri:3;
    uint32 c2scact_aipri:1;
    uint32 c2scact_itag:1;
    uint32 c2scact_aitag:1;
    uint32 c2scact_itpid:2;
    uint32 c2scact_aitpid:1;
    uint32 c2scact_opri:3;
    uint32 c2scact_aopri:2;
    uint32 c2scact_otag:1;
    uint32 c2scact_aotag:1;
    uint32 c2scact_otpid:2;
    uint32 c2scact_aotpid:1;
    uint32 macipact_pri:3;
    uint32 macipact_apri:1;
    uint32 macipact_tag:1;
    uint32 macipact_atag:1;
    uint32 macipact_tpid:2;
    uint32 macipact_atpid:1;
    uint32 macipact_type:1;
    uint32 macipact_ignore:1;
    uint32 macipact_fwdact:2;
    uint32 int_pri_route:3;     /* internal priority including route */
    uint32 int_pri:3;           /* internal priority */
    uint32 dp:2;                /* ingress acl decision packet color */
    uint32 proto_storm_drop:1;  /* protocol storm packet drop */
    uint32 byp_igr_bwctrl:1;    /* ingress bandwidth control bypass packet */
    uint32 st_ctag_if:1;        /* cpu tag if in stacking header info */
    uint32 st_sa_lrn:1;         /* sa learn in stacking header common info */
    uint32 dp_fmt:1;            /* dp fmt in stacking header common info */
    uint32 dp_info:12;          /* dp info in stacking header common info */
    uint32 fwd_type:2;          /* forward type in stacking header common info */
    uint32 sp_info:10;          /* source port info in stacking header common info */
    uint32 sth_vld:1;           /* packet from stacking port */
    uint32 sphy:6;              /* source port ID */
    uint32 trk_id:7;            /* [6]:trunk enable;[5:0]:trunk id */
    uint8  smac[6];             /* Source MAC address */
    uint8  dmac[6];             /* Destinaton MAC address */
    uint32 ctag_if:1;           /* Has CPU tag */
    uint32 pkt_len:14;          /* Packet length (Include CRC) */
};
typedef struct hsm_pre_9300_s hsm_pre_9300_t;


struct hsm_lut_9300_s
{
    uint32 trk_hit_idx:1;       /*lut hit entry to trunk hash mask index */
    uint32 trk_uc:1;            /*lut hit entry to trunk*/
    uint32 sa_lrn:1;            /* sa learn after lut */
    uint32 dp_fmt:1;            /* dp format after lut */
    uint32 dp_info:12;          /* dp info after lut */
    uint32 fwd_type:2;          /* forward type after lut */
    uint32 sp_info:10;          /* source port info in stacking header common info */
    uint32 macMis_act:12;
    uint32 port_mv:1;           /* is port moved */
    uint64 route_act:42;        /* route info
                                   for multicast routing
                                       proc_id[2:0]:
                                       event[12:3]: bit[9]trap/copy to loacl or master,bit[8:7](1-trap 2-copy 3-drop 4-fwd), bit[6:0] reason
                                       mcrt_rpf_asst[13]: multicast rpf fail assert action
                                       mcrt_oil_idx[22:14]: oil index
                                       mcrt_oil_en[23]: oil enable for mcast.
                                       mc_l3_hit[24] for mcast. URPF_EN[23]: urpf enable(cfg urpf enable, check pkt type pass) for ucast
                                       pbr[25]: acl pbr
                                       mactb_hit[26]: hit mactable
                                       hittcam[27]: hit prefix table, for egress acl template
                                       hitram[28]: hit host table,for egress acl template
                                       l3_en[29]:l3 enable
                                       stk_pmsk[33:30]
                                       mc_l2_hit[34]:
                                       [41:35]: Reserved
                                   for unicast routing
                                       proc_id[2:0]:
                                       event[12:3]: bit[9]trap/copy to loacl or master,bit[8:7](1-trap 2-copy 3-drop 4-fwd), bit[6:0] reason                                       ucrt_ttl_dec[12]: ttl decrease. Dec TTL
                                       ucrt_ttl_del[13]: ttl decrease. Dec TTL
                                       ucrt_ttl_chk[14]: ttl check. Check ttl
                                       null_intf_trap[15]: invalid dmac index trap
                                       null_intf_drop[16]: invalid dmac index drop
                                       ucrt_otg_idx[23:17]: urpf index(urpf lookup outgoing interface index,
                                                                       to get vid for strict URPF mode) only need vid
                                       urpf_en[24]: urpf enable(cfg urpf enable, check pkt type pass) for ucast
                                       pbr[25]: acl pbr
                                       mactb_hit[26]: hit mactable
                                       hittcam[27]: hit prefix table, for egress acl template
                                       hitram[28]: hit host table,for egress acl template
                                       urpf_act[31:29]: urpf action (urpf action when urpf fail)
                                       urpf_mode[32]: urpf mode(0-loose mode, 1-strict mode)
                                       urpf_hit[33]: urpf hit (hit, dfLt rt check pass,~dst null) &(~(STRICT & NXHIDX ==7FFF))
                                       [34] resverd
                                       urpf_idx[41:35]: urpf index (urpf lookup outgoing interface index,
                                                                    to get vid for strict URPF mode) only need vid
                                */
    uint32 sptrsn:9;            /* Special packet trap reason ([1:0]igmp/mld, [3:2]dhcp/dhcpv6, [5:4]arp_rep/arp_req)(01-trap, 10-copy) */
    uint32 ip_rsv_act:2;        /* ip reserved pkt action (00-fwd, 01-trap, 10-copy) */
    uint32 rip_if:1;            /* rip pkt */
    uint32 dhcp_if:1;           /* dhcp pkt */
    uint32 rma_rsn:7;           /* RMA reason */
    uint32 l2_hash_full:3;      /* lut table hashfull action (000-fwd, 001-drop, 010-trap local, 011-copy local, 100-trap master, 101-copy master) */
    uint32 iacl_metadata:8;    /* iacl meta action */
    uint32 nexthop_if:1;        /* nexthop entry */
    uint32 nexthop_ageout:1;       /* nexthop ageout entry */
    uint32 rma_byp_new_sa_drop:1;   /* rma pkt bypass new sa drop */
    uint32 rma_byp_vlan_fltr:1; /* rma bypass vlan ingress filter */
    uint32 iacl_byp_act:4;      /* iacl bypass action */
    uint32 ctag_flag:6;         /* trap to cpu tag (mac_limit/newsa/pm_forbid/static_pm/dynamic_pm/hash_full) */
    uint32 org_spa:11;           /* l2 entry original spa */
    uint32 forbid_pm_act:2;     /* forbid port move action (00-fwd, 01-drop, 10-trap, 11-copy) */
    uint32 dyn_pm_act:2;        /* dynamic port move action (00-fwd, 01-drop, 10-trap, 11-copy) */
    uint32 src_port_fltr_en:1;  /* source port filter enable */
    uint32 is_mcast_sa:1;       /* sa is multicast */
    uint32 igr_vlan_fltr_act:2; /* vlan ingress filter action (00-fwd, 01-drop, 10-trap) */
    uint32 dpm:29;              /* lut dpm */
    uint32 hash_type:2;         /* hash type (11-route, 01-mc, 00-uc) */
    uint32 da_type:2;           /* fwd type (01-mc, 00-uc) */
    uint32 new_sa_act:3;        /* new sa action */
    uint32 sttc_pm_act:3;       /* static portmove action (00-fwd, 01-drop, 10-trap, 11-copy) */
    uint32 mac_limit_vlan_act:3;/* maclimit vlan action (000-fwd/001-drop/010-trap local/011-copy local/100-trap master/101-copy master) */
    uint32 mac_limit_act:3;     /* maclimit action (00-fwd, 01-drop, 10-trap, 11-copy) */
    uint32 l3_da_idx:11;        /* da index */
    uint32 l2_da_hit:1;         /* da hit*/
                                /* da hit entry (trunk, slp, age, sab, dab, st, suspend, nh, vid) */
    uint32 l2_da_trunk;
    uint32 l2_da_slp;
    uint32 l2_da_age;
    uint32 l2_da_sab;
    uint32 l2_da_dab;
    uint32 l2_da_st;
    uint32 l2_da_suspend;
    uint32 l2_da_nh;
    uint32 l2_da_vid;
    uint32 l2_sa_hit:1;         /* sa hit*/
                                /* sa hit entry (trunk, slp, age, sab, dab, st, suspend) */
    uint32 l2_sa_trunk;
    uint32 l2_sa_slp;
    uint32 l2_sa_age;
    uint32 l2_sa_sab;
    uint32 l2_sa_dab;
    uint32 l2_sa_st;
    uint32 l2_sa_suspend;
    uint32 oam_act:2;           /* oam pkt action(00-fwd, 01-trap, 10-drop, 11-lb) */
    uint32 igr_stp_status:2;    /* spanding tree status(00-dis, 01-block, 10-learn, 11-fwd)*/
    uint32 rx_port_in_vlan:1;   /* Source port in VLAN member */
    uint32 msti:6;              /* vlan table mst instance id */
    uint32 vlan_group_mask:8;   /* vlan table group bit */
    uint32 iacl_meter_drop:1;   /* Drop by meter; 1=drop, 0=pass */
    uint32 vlan_cfi_act:2;      /* CFI action; 0=fwd, 1=drop, 2=trap, 3=no used */
    uint32 vlan_aft_drop:1;      /* i/otag accept drop */
    uint32 igr_ivid:12;         /* internal ivid */
    uint32 igr_ovid:12;         /* internal ovid */
    uint32 fwd_vid:12;          /* fwd VLAN ID */
    uint32 fwd_base:1;          /* fwd vid select (1-internal ovid, 0-internal ivid) */
    uint32 ivc_drop:1;          /* c2sc drop */
    uint32 acl_hit:9;           /* ACL action hit bits */
    uint32 acl_idx:11;          /* acl hit index */
    uint64 acl_act:52;          /* acl action do */
    uint32 capwap_trap:1;       /* Special packet trap */
    uint32 eav_class_a:1;       /* Is EAV & Class A packet */
    uint32 eav_class_b:1;       /* Is EAV & Class B packet */
    uint32 atk_type:5;          /* attack reason */
    uint32 atk_act:2;           /* attack action */
    uint32 atk_hit:1;           /* attack pkt hit */
    uint32 grap_act:2;          /* RMA packet bypass new sa drop (00-fwd, 01-drop, 10-trap, 11-copy) */
    uint32 rma_pkt:1;           /* rma pkt */
    uint32 rma_act:3;           /* rma pkt action */
    uint32 rma_byp_stp:1;       /* rma pkt bypass stp */
    uint32 ivc_mode:1;          /* 1: mac&ip-sub based; 0: ivc */
    uint32 ivc_hit:1;           /* c2sc hit */
                                /* c2sc data
                                    for ivc
                                        c2scact_ipri[2:0]: New inner priority (active when c2scact_aipri = 1)
                                        c2scact_aipri[3]: New inner priority (Assign inner priority (PCP) value)
                                        c2scact_itag[4]: New inner tag status (active when c2scact_aitag = 1)
                                        c2scact_aitag[5]: Assign the I-TAG status
                                        c2scact_itpid[7:6]: Index of the I-TPID (active when c2scact_aitpid = 1)
                                        c2scact_aitpid[8]: Assign I-TPID
                                        c2scact_opri[11:9]: New outer priority (active when c2scact_aopri = 1)
                                        c2scact_aopri[13:12]: Assign outer priority (PCP) value
                                        c2scact_otag[14]: New inner tag status (active when c2scact_aotag = 1)
                                        c2scact_aotag[15]: Assign the O-TAG status
                                        c2scact_itpid[17:16]: Index of the I-TPID (active when c2scact_aitpid = 1)
                                        c2scact_aitpid[18]: Assign the O-TAG status
                                    for mac-based
                                        c2scact_pri[2:0]: New tag priority (active when PRI_ASSIGN = 1)
                                        c2scact_pri[3]: Assign tag priority (PCP) value
                                        c2scact_tag[4]: New tag status (active when c2scact_atag = 1)
                                        c2scact_atag[5]: Assign the TAG status
                                        c2scact_tpid[7:6]: Index of the TPID (active when c2scact_atpid = 1)
                                        c2scact_atpid[8]: Assign TPID according to VLAN_TYPE
                                        c2scact_type[9]: To indicate the VLAN type
                                        c2scact_ignore[10]: Indicate the packet should ignore Ingress VLAN Filtering
                                        c2scact_fwdact[12:11]: Assign the forwarding action
                                */
    uint32 c2scact_ipri:3;
    uint32 c2scact_aipri:1;
    uint32 c2scact_itag:1;
    uint32 c2scact_aitag:1;
    uint32 c2scact_itpid:2;
    uint32 c2scact_aitpid:1;
    uint32 c2scact_opri:3;
    uint32 c2scact_aopri:2;
    uint32 c2scact_otag:1;
    uint32 c2scact_aotag:1;
    uint32 c2scact_otpid:2;
    uint32 c2scact_aotpid:1;
    uint32 macipact_pri:3;
    uint32 macipact_apri:1;
    uint32 macipact_tag:1;
    uint32 macipact_atag:1;
    uint32 macipact_tpid:2;
    uint32 macipact_atpid:1;
    uint32 macipact_type:1;
    uint32 macipact_ignore:1;
    uint32 macipact_fwdact:2;
    uint32 int_pri_route:3;     /* internal priority including route */
    uint32 int_pri:3;           /* internal priority */
    uint32 dp:2;                /* ingress acl decision packet color */
    uint32 proto_storm_drop:1;  /* protocol storm packet drop */
    uint32 byp_igr_bwctrl:1;    /* ingress bandwidth control bypass packet */
    uint32 sphy:6;              /* source port ID */
    uint32 trk_id:7;            /* [6]:trunk enable;[5:0]:trunk id */
};
typedef struct hsm_lut_9300_s hsm_lut_9300_t;


struct hsm_post0_9300_s
{
    uint32 acl_rediCopy:1;          /* acl redirect or copy , for strom known unknown */
    uint32 st_pmsk:4;           /* stack fwd port mask in routing entry */
    uint32 l3_uc_kn:1;          /* L3 uc known pkt, hit mactable and act != bridging */
    uint32 ip_route_en:1;       /* routing modify vid. Routing pkt or not */
    uint32 l3_ttl_dec:1;        /* routing ttl decrease */
    uint32 l3_da_rep:1;         /* routing da replace */
    uint32 l3_sa_rep:1;         /* routing sa replace */
    uint32 l3_sa_idx:6;         /* rouing sa index */
    uint32 rt_event:9;          /* routing event */

    uint32 crx_data[3];        /* cpu rx tag[79:0] in stacking header */
    uint32 ctx_acl_act:1;      /* cpu tx take the acl effect */
    uint32 ctx_cngst_drop:1;   /* cpu tx congest drop */
    uint32 ctx_dg_gasp:1;      /* dying gasp */
    uint32 ctx_bp_fltr:1;      /* cpu tx bypass filter*/
    uint32 ctx_bp_stp:1;       /* ctx bypass STP */
    uint32 ctx_bp_egr_vlan:1;  /* ctx bypass Egress */
    uint32 ctx_as_tag:1;        /* cpu tx assign tag */
    uint32 ctx_l3_act:1;        /* cpu tx l3 act */
    uint32 ctx_org_tag:3;       /* cpu tx original tag */
    uint32 ctx_qid:4;           /* cpu tx qid */
    uint32 ctx_as_qid:1;        /* cpu tx assign qid */
    uint32 ctx_fwd_vid_en:1;    /* cpu tx enable forwarding vid */
    uint32 ctx_sp_fltr:12;      /* cpu tx src filter  */
    uint32 ctx_dpm_type:4;      /* cpu tx dpm type */
    uint64 ctx_dpm:56;          /* cpu tx dpm */
    uint32 ctx_unit:4;          /* cpu tx switch unit id */

    uint32 macMiss_act:12;      /* mc bridge lkmiss act */
    uint32 ip_rsv_act:3;        /* IPRSV ACT */
    uint32 sptrsn:9;            /* Special packet trap reason ([1:0]igmp/mld, [3:2]dhcp/dhcpv6, [5:4]arp_rep/arp_req)(01-trap, 10-copy) */
    uint32 new_sa_act:3;        /* new sa act */
    uint32 mac_limit_vlan_act:3;/* vlan based mac limit act */
    uint32 mac_limit_act:3;     /* mac limit act */
    uint32 sttc_pm_act:3;       /* port move act */
    uint32 dyn_pm_act:3;        /* dynamic port move act */
    uint32 force_pmove:3;       /* forbid port move action */
    uint32 l2_hash_full:3;      /* hash full act */
    uint32 ip_rsvd_inv:1;       /* invert ip rsvd flag bit in ipv4 header */
    uint32 ctag_flag:6;         /* trap to cpu tag flag (mac_limit/newsa/pm_forbid/static_pm/dynamic_pm/hash_full) */
    uint32 metadata:8;          /* meta data for loopback */
    uint32 vb_iso_mbr:1;        /* vlan based isolated member*/
    uint32 acl_lb_act:1;        /* acl loopback packet */
    uint32 lb_ttl:3;            /* loopback times */
    uint32 lb_pkt:1;            /* pkt from loopback port*/
    uint32 stk_if:1;            /* pkt from stacking port */
    uint32 sp_info:10;          /* source port info in stacking header */
    uint32 sp_fltr_en:1;        /* source port filter in stacking header */
    uint32 dp_fmt:1;            /* stacking dp fmt */
    uint32 dp_info:12;          /* stacking dp info */
    uint32 fwd_type:2;          /* stacking forward type */
    uint32 st_ctag_if:2;        /* cpu tag if in stacking info */
    uint32 mir_id:2;            /* mirror id in stacking info */
    uint32 trk_hash:6;          /* trunk hash in stacking info */
    uint32 st_da_hit:1;         /* da hit in stacking header */
    uint32 sa_lrn:1;            /* sa learn */
    uint32 rip_if:1;            /* RIP pkt */
    uint32 dhcp_if:1;           /* DHCP pkt */
    uint32 garp_act:2;          /* GARP act */
    uint32 eacl_disable:1;      /* oacl disable */
    uint32 iacl_drop:1;         /* iacl drop */
    uint32 iacl_drop_act:1;     /* iacl drop action drop */
    uint32 iacl_redir_zero:1;   /* iacl redirect to null port */
    uint32 iacl_trap:1;         /* iacl trap */
    uint32 iacl_trap_master:1;  /* iacl trap/copy to master cpu or not */
    uint32 iacl_copy:1;         /* iacl copy */
    uint32 eacl_drop:1;         /* oacl drop */
    uint32 eacl_drop_act:1;     /* iacl drop action drop */
    uint32 eacl_redir_zero:1;   /* oacl redirect to null port */
    uint32 eacl_trap:1;         /* oacl trap */
    uint32 eacl_trap_master:1;  /* eacl trap/copy to master cpu or not */
    uint32 eacl_copy:1;         /* oacl copy */
    uint32 ivc_drop:1;          /* C2SC drop, including ivc lkmiss drop, ivc null operation drop,mac ip based action drop */
    uint32 ivc_trap:1;          /* c2sc trap */
    uint32 ivc_trap_master:1;   /* c2sc trap/copy to master cpu or not!1-master,0-local */
    uint32 ivc_copy:1;          /* c2sc copy */
    uint32 ivc_byp_igr_vlan_fltr:1;       /* c2sc bypass ingress vlan filter */
    uint32 eacl_meter_drop:1;   /* egress meter drop */
    uint32 iacl_meter_drop:1;   /* imeter drop */
    uint32 nexthop_if:1;        /* next hop entry */
    uint32 nexthop_ageout:1;    /* next hop entry and age = 0 */
    uint32 storm_lkmis:1;       /* strom contrl mc lookup miss */
    uint32 trk_mc_hit:1;        /* trunk mc lookup hit */
    uint32 dp:2;                /* pkt color(drop precedence) */
    uint32 hash_type:2;         /* lut type, 0-uc, 1-l2mc,2-ipmc,3-l3 routing */
    uint32 l3_da_idx:11;      /* L3 da index */
    uint32 fwd_base:1;          /* FWD vid select, to HAS */
    uint32 orig_fwd_vid:12;     /* ORIGINAL RVID */
    uint32 fwd_vid:12;          /* FWD vid, to HAS */
    uint32 l2_da_hit:1;         /* da hit */
    uint32 l2_da_block:1;       /* da block */
    uint32 org_spa:11;           /* original spa */
    uint32 mcrt_rpf_asst:1;     /* MC routing RPF assert */
    uint32 mcrt_oil_idx:9;     /* MC routing oil index */
    uint32 oam_act:2;           /* oam action */
    uint32 atk_act:2;           /* hw attack action */
    uint32 atk_type:5;          /* hw attack reson */
    uint32 byp_igr_bwctrl:1;    /* pass by input bandwidth contrl */
    uint32 int_pri:3;           /* internal priority */
    uint32 int_pri_route:3;     /* internal priority with route */
    uint32 iacl_qid:6;          /* iacl assign qid,[5] assign qid,[4:0] qid */
    uint32 acl_hit:1;           /* ACL TRAP */
    uint32 acl_idx:11;          /* ACL ID */
    uint32 iacl_mir_hit:1;      /* iacl hit mirror */
    uint32 iacl_mir_act:3;      /* iacl mirror action */
    uint32 acl_rmk_val:8;       /* new qos */
    uint32 eacl_rmk_hit:1;      /* oacl remark qos */
    uint32 igr_ivid:12;         /* new ivid */
    uint32 eacl_ivid_hit:1;     /* oacl/routing assign ivid*/
    uint32 n_1_ivid_hit:1;      /* n:1 assign ivid */
    uint32 igr_ovid:12;         /* new ovid */
    uint32 eacl_ovid_hit:1;     /* oacl/routing assign ivid */
    uint32 n_1_ovid_hit:1;      /* n:1 assign ovid */
    uint32 eacl_itag_hit:1;     /* oacl assign itag */
    uint32 eacl_otag_hit:1;     /* oacl assign otag */
    uint32 eacl_ipri_hit:1;     /* oacl assign ipri */
    uint32 eacl_opri_hit:1;     /* oacl assign opri */
    uint32 eacl_itpid_hit:1;    /* oacl assign itpid */
    uint32 eacl_otpid_hit:1;    /* oacl assign otpid */
    uint32 eacl_mir_act:4;      /* oacl mirror action */
    uint32 eacl_mir_hit:1;      /* oacl hit mirror */
    uint32 eacl_pri_hit:1;      /* oacl hit priority */
    uint32 eacl_qid:6;          /* oacl qid,[5] assign qid, [4:0] qid */
    uint32 acl_cpu_fmt:2;       /* PKT format when acl trap/copy to cpu */
    uint32 acl_redi:1;          /* acl redirect */
    uint32 ivc_iacl_itag_status_as:1;    /* iacl/c2sc assign itag */
    uint32 ivc_iacl_otag_status_as:1;    /* iacl/c2sc assign otag */
    uint32 ivc_iacl_ipri_as:1;   /* iacl ivc assign ipri */
    uint32 ivc_iacl_opri_as:1;   /* iacl ivc assign opri */
    uint32 ivc_itpid_as:1;      /* ivc/s.int_itpid_hit assign itpid */
    uint32 ivc_otpid_as:1;      /* ivc/s.int_otpid_hit assign otpid */
    uint32 egr_itag_status:2;   /* egress itag status */
    uint32 egr_otag_status:2;   /* egress otag status */
    uint32 egr_ipri:3;          /* egress ipri */
    uint32 egr_opri:3;          /* egress opri */
    uint32 int_icfi:1;          /* int icfi from stacking header */
    uint32 int_ocfi:1;          /* int ocfi from stacking header */
    uint32 egr_itpid_idx:2;     /* egress itpid idx */
    uint32 egr_otpid_idx:2;     /* egress otpid idex */
    uint32 trk_hash_0:6;        /* trunk hash 0 */
    uint32 trk_hash_1:6;        /* trunk hash 1 */
    uint32 unkn_trk_hash_0:6;        /* unkown trunk hash 0 */
    uint32 unkn_trk_hash_1:6;        /* unkown trunk hash 1 */
    uint32 hash_uc:1;           /* hash uc */
    uint32 hash_idx:1;          /* hash idx */
    uint32 src_port_fltr_en:1;  /* enable source port filter */
    uint32 trk_id:7;            /* original source port's trunk group (6:trunk enable; 5:0:trunk id) */
    uint32 rma_byp_new_sa_drop:1;   /* RMA bypasss new sa */
    uint32 iacl_byp_act:3;      /* bypass */
    uint32 vlan_src_port_fltr_act:2;  /* vlan source port filter action */
    uint32 rx_port_in_vlan:1;   /* vlan source port member set */
    uint32 dpm:29;              /* destination poe */
    uint32 da_type:2;           /* fwd type, 0 uc, 1 mc, 2 ipv4, 3 ipv6 */
    uint32 l2_sa_hit:1;         /* sa hit */
    uint32 l2_sa_sab:1;       /* sa block */
    uint32 igr_stp_status:2;    /* span tree */
    uint32 vlan_cfi_act:2;      /* CFI action */
    uint32 vlan_aft_drop:1;      /* tag action */
    uint32 capwap_trap:1;       /* special trap */
    uint32 proto_storm_drop:1;  /* drop by protocol storm */
    uint32 eav_class_a:1;       /* eav class a */
    uint32 eav_class_b:1;       /* eav class b */
    uint32 rma_pkt:1;           /* is RMA pkt */
    uint32 rma_act:3;           /* RMA action */
    uint32 rma_byp_vlan_fltr:1; /* RMA bypass vlan */
    uint32 rma_byp_stp:1;       /* RMA bypass blocking and listning */
    uint32 rma_rsn:7;           /* RMA reason */
    uint32 page_cnt:6;          /* page counter */
    uint32 fst_dsc:12;          /* begin dsc */
    uint32 lst_dsc:12;          /* end dsc */
    uint32 rma_eapol_pkt:1;     /* eapol pkt */
    uint32 arp_pkt:1;           /* arp pkt */
    uint32 rtk_pkt:1;           /* Realtek pkt */
    uint32 rldp_rlpp_pkt:1;     /* Rldp/rlpp pkt */
    uint32 eth:1;               /* eth dm pkt */
    uint32 rma_bpdu_pkt:1;      /* bpdu pkt */
    uint32 da_bcast:1;          /* bc pkt */
    uint32 da_mcast:1;          /* mc pkt */
    uint32 sa_zero:1;           /* sa is zero */
    uint32 is_mcast_sa:1;       /* sa is mc */
    uint32 ipv4_pkt:1;          /* ipv4 pkt. From HSB */
    uint32 ipv6_pkt:1;          /* ipv6 pkt. From HSB */
                                /* original outer tag */
    uint32 opri:3;
    uint32 ocfi:1;
    uint32 ovid:12;
                                /* original inner tag */
    uint32 ipri:3;
    uint32 icfi:1;
    uint32 ivid:12;
    uint32 otag_exist:1;        /* original otag if */
    uint32 itag_exist:1;        /* original itag if */
    uint32 ctag_exist:1;        /* original ctag if */
    uint32 etag_exist:1;        /* original etag if */
    uint32 rtag_exist:1;        /* origian rtag if */
    uint32 rtag_type:1;         /* rtag type */
    uint32 igr_err:1;           /* ingress error */
    uint32 sphy:6;              /* sport physical id */
    uint32 org_itpid_idx:2;         /* original itpid idx */
    uint32 org_otpid_idx:2;         /* original otpid idx */
    uint32 rfc1042:1;           /* snap pkt */
    uint32 pppoe_pkt:1;         /* pppoe pkt */
    uint32 llc_other:1;         /* llc other pkt */
    uint32 pkt_len:14;          /* pkt length */
    uint32 ip_length:16;        /* IPv4 Total length or IPv6 payload length(For Attack prevention) */
    uint32 ip_ttl:8;            /* IP header - TTL field */
    uint32 err_pkt:1;           /* error pkt */
    uint32 ipv4_chksum_ok:1;    /* ipv4 cks */
    uint32 hol_prvnt:1;         /* hol prevent */
    uint32 ipv6_mld:1;          /* ipv6 igmp pkt */
    uint32 ipv4_igmp:1;         /* ipv4 igmp pkt */
    uint32 oam_pdu:1;           /* OAM pdu */
};
typedef struct hsm_post0_9300_s hsm_post0_9300_t;


struct hsm_post_replication_9300_s
{
    uint32 acl_rediCopy:1;          /* acl redirect or copy , for strom known unknown */
    uint32 st_pmsk:4;           /* stack fwd port mask in routing entry */
    uint32 l3_uc_kn:1;          /* L3 uc known pkt, hit mactable and act != bridging */
    uint32 ip_route_en:1;       /* routing modify vid. Routing pkt or not */
    uint32 l3_ttl_dec:1;        /* routing ttl decrease */
    uint32 l3_da_rep:1;         /* routing da replace */
    uint32 l3_sa_rep:1;         /* routing sa replace */
    uint32 l3_sa_idx:6;         /* rouing sa index */
    uint32 route_en:1;          /* ip route enable */
    uint32 ipmc_pkt:1;          /* replication packet */
    uint32 rt_ipmc_ttl_dec:1;        /* ipmc ttl decrease */
    uint32 rt_ipmc_type:2;      /* IPMC type */
    uint32 rt_ipmc_qid:1;       /* IPMC QID */
    uint32 rt_ipmc_uccp:1;      /* IPMC unicast copy */
    uint32 rt_ipmc_fstcp:1;     /* IPMC first copy */
    uint32 rt_ipmc_lstcp:1;     /* IPMC last copy */

    uint32 crx_data[3];        /* cpu rx tag[79:0] in stacking header */
    uint32 ctx_acl_act:1;      /* cpu tx take the acl effect */
    uint32 ctx_cngst_drop:1;   /* cpu tx congest drop */
    uint32 ctx_dg_gasp:1;      /* dying gasp */
    uint32 ctx_bp_fltr:1;      /* cpu tx bypass filter*/
    uint32 ctx_bp_stp:1;       /* ctx bypass STP */
    uint32 ctx_bp_egr_vlan:1;  /* ctx bypass Egress */
    uint32 ctx_as_tag:1;        /* cpu tx assign tag */
    uint32 ctx_l3_act:1;        /* cpu tx l3 act */
    uint32 ctx_org_tag:3;       /* cpu tx original tag */
    uint32 ctx_qid:4;           /* cpu tx qid */
    uint32 ctx_as_qid:1;        /* cpu tx assign qid */
    uint32 ctx_fwd_vid_en:1;
    uint32 ctx_sp_fltr:12;      /* cpu tx src filter  */
    uint32 ctx_dpm_type:4;      /* cpu tx dpm type */
    uint64 ctx_dpm:56;          /* cpu tx dpm */
    uint32 ctx_unit:4;          /* cpu tx switch unit id */

    uint32 macMiss_act:12;      /* mc bridge lkmiss act */
    uint32 ip_rsv_act:3;        /* IPRSV ACT */
    uint32 sptrsn:9;            /* Special packet trap reason ([1:0]igmp/mld, [3:2]dhcp/dhcpv6, [5:4]arp_rep/arp_req)(01-trap, 10-copy) */
    uint32 new_sa_act:3;        /* new sa act */
    uint32 mac_limit_vlan_act:3;/* vlan based mac limit act */
    uint32 mac_limit_act:3;     /* mac limit act */
    uint32 sttc_pm_act:3;       /* port move act */
    uint32 dyn_pm_act:3;        /* dynamic port move act */
    uint32 force_pmove:3;       /* forbid port move action */
    uint32 l2_hash_full:3;      /* hash full act */
    uint32 ip_rsvd_inv:1;       /* invert ip rsvd flag bit in ipv4 header */
    uint32 ctag_flag:6;         /* trap to cpu tag flag (mac_limit/newsa/pm_forbid/static_pm/dynamic_pm/hash_full) */
    uint32 metadata:8;          /* meta data for loopback */
    uint32 vb_iso_mbr:1;        /* vlan based isolated member*/
    uint32 acl_lb_act:1;        /* acl loopback packet */
    uint32 lb_ttl:3;            /* loopback times */
    uint32 lb_pkt:1;            /* pkt from loopback port*/
    uint32 stk_if:1;            /* pkt from stacking port */
    uint32 sp_info:10;          /* source port info in stacking header */
    uint32 sp_fltr_en:1;        /* source port filter in stacking header */
    uint32 dp_fmt:1;            /* stacking dp fmt */
    uint32 dp_info:12;          /* stacking dp info */
    uint32 fwd_type:2;          /* stacking forward type */
    uint32 st_ctag_if:2;        /* cpu tag if in stacking info */
    uint32 mir_id:2;            /* mirror id in stacking info */
    uint32 trk_hash:6;          /* trunk hash in stacking info */
    uint32 st_da_hit:1;         /* da hit in stacking header */
    uint32 sa_lrn:1;            /* sa learn */
    uint32 rip_if:1;            /* RIP pkt */
    uint32 dhcp_if:1;           /* DHCP pkt */
    uint32 garp_act:2;          /* GARP act */
    uint32 eacl_disable:1;      /* oacl disable */
    uint32 iacl_drop:1;         /* iacl drop */
    uint32 iacl_drop_act:1;     /* iacl drop action drop */
    uint32 iacl_redir_zero:1;   /* iacl redirect to null port */
    uint32 iacl_trap:1;         /* iacl trap */
    uint32 iacl_trap_master:1;  /* iacl trap/copy to master cpu or not */
    uint32 iacl_copy:1;         /* iacl copy */
    uint32 eacl_drop:1;         /* oacl drop */
    uint32 eacl_drop_act:1;     /* iacl drop action drop */
    uint32 eacl_redir_zero:1;   /* oacl redirect to null port */
    uint32 eacl_trap:1;         /* oacl trap */
    uint32 eacl_trap_master:1;  /* eacl trap/copy to master cpu or not */
    uint32 eacl_copy:1;         /* oacl copy */
    uint32 ivc_drop:1;          /* C2SC drop, including ivc lkmiss drop, ivc null operation drop,mac ip based action drop */
    uint32 ivc_trap:1;          /* c2sc trap */
    uint32 ivc_trap_master:1;   /* c2sc trap/copy to master cpu or not!1-master,0-local */
    uint32 ivc_copy:1;          /* c2sc copy */
    uint32 ivc_byp_igr_vlan_fltr:1;       /* c2sc bypass ingress vlan filter */
    uint32 eacl_meter_drop:1;   /* egress meter drop */
    uint32 iacl_meter_drop:1;   /* imeter drop */
    uint32 nexthop_if:1;        /* next hop entry */
    uint32 nexthop_ageout:1;    /* next hop entry and age = 0 */
    uint32 storm_lkmis:1;       /* strom contrl mc lookup miss */
    uint32 trk_mc_hit:1;        /* trunk mc lookup hit */
    uint32 dp:2;                /* pkt color(drop precedence) */
    uint32 hash_type:2;         /* lut type, 0-uc, 1-l2mc,2-ipmc,3-l3 routing */
    uint32 l3_da_idx:11;      /* L3 da index */
    uint32 fwd_base:1;          /* FWD vid select, to HAS */
    uint32 orig_fwd_vid:12;     /* ORIGINAL RVID */
    uint32 fwd_vid:12;          /* FWD vid, to HAS */
    uint32 l2_da_hit:1;         /* da hit */
    uint32 l2_da_block:1;       /* da block */
    uint32 org_spa:11;           /* original spa */
    uint32 mcrt_rpf_asst:1;     /* MC routing RPF assert */
    uint32 mcrt_oil_idx:9;     /* MC routing oil index */
    uint32 oam_act:2;           /* oam action */
    uint32 atk_act:2;           /* hw attack action */
    uint32 atk_type:5;          /* hw attack reson */
    uint32 byp_igr_bwctrl:1;    /* pass by input bandwidth contrl */
    uint32 int_pri:3;           /* internal priority */
    uint32 int_pri_route:3;     /* internal priority with route */
    uint32 iacl_qid:6;          /* iacl assign qid,[5] assign qid,[4:0] qid */
    uint32 acl_hit:1;           /* ACL TRAP */
    uint32 acl_idx:11;          /* ACL ID */
    uint32 iacl_mir_hit:1;      /* iacl hit mirror */
    uint32 iacl_mir_act:3;      /* iacl mirror action */
    uint32 acl_rmk_val:8;       /* new qos */
    uint32 eacl_rmk_hit:1;      /* oacl remark qos */
    uint32 igr_ivid:12;         /* new ivid */
    uint32 eacl_ivid_hit:1;     /* oacl/routing assign ivid*/
    uint32 n_1_ivid_hit:1;      /* n:1 assign ivid */
    uint32 igr_ovid:12;         /* new ovid */
    uint32 eacl_ovid_hit:1;     /* oacl/routing assign ivid */
    uint32 n_1_ovid_hit:1;      /* n:1 assign ovid */
    uint32 eacl_itag_hit:1;     /* oacl assign itag */
    uint32 eacl_otag_hit:1;     /* oacl assign otag */
    uint32 eacl_ipri_hit:1;     /* oacl assign ipri */
    uint32 eacl_opri_hit:1;     /* oacl assign opri */
    uint32 eacl_itpid_hit:1;    /* oacl assign itpid */
    uint32 eacl_otpid_hit:1;    /* oacl assign otpid */
    uint32 eacl_mir_act:4;      /* oacl mirror action */
    uint32 eacl_mir_hit:1;      /* oacl hit mirror */
    uint32 eacl_pri_hit:1;      /* oacl hit priority */
    uint32 eacl_qid:6;          /* oacl qid,[5] assign qid, [4:0] qid */
    uint32 acl_cpu_fmt:2;       /* PKT format when acl trap/copy to cpu */
    uint32 acl_redi:1;          /* acl redirect */
    uint32 ivc_iacl_itag_status_as:1;    /* iacl/c2sc assign itag */
    uint32 ivc_iacl_otag_status_as:1;    /* iacl/c2sc assign otag */
    uint32 ivc_iacl_ipri_as:1;   /* iacl ivc assign ipri */
    uint32 ivc_iacl_opri_as:1;   /* iacl ivc assign opri */
    uint32 ivc_itpid_as:1;      /* ivc/s.int_itpid_hit assign itpid */
    uint32 ivc_otpid_as:1;      /* ivc/s.int_otpid_hit assign otpid */
    uint32 egr_itag_status:2;   /* egress itag status */
    uint32 egr_otag_status:2;   /* egress otag status */
    uint32 egr_ipri:3;          /* egress ipri */
    uint32 egr_opri:3;          /* egress opri */
    uint32 int_ocfi:1;          /* int ocfi from stacking header */
    uint32 int_icfi:1;          /* int icfi from stacking header */
    uint32 egr_itpid_idx:2;     /* egress itpid idx */
    uint32 egr_otpid_idx:2;     /* egress otpid idex */
    uint32 trk_hash_0:6;        /* trunk hash 0 */
    uint32 trk_hash_1:6;        /* trunk hash 1 */
    uint32 unkn_trk_hash_0:6;        /* unkown trunk hash 0 */
    uint32 unkn_trk_hash_1:6;        /* unkown trunk hash 1 */
    uint32 hash_uc:1;           /* hash uc */
    uint32 hash_idx:1;          /* hash idx */
    uint32 src_port_fltr_en:1;  /* enable source port filter */
    uint32 trk_id:7;            /* original source port's trunk group (6:trunk enable; 5:0:trunk id) */
    uint32 rma_byp_new_sa_drop:1;   /* RMA bypasss new sa */
    uint32 iacl_byp_act:3;      /* bypass */
    uint32 vlan_src_port_fltr_act:2;  /* vlan source port filter action */
    uint32 rx_port_in_vlan:1;   /* vlan source port member set */
    uint32 dpm:29;              /* destination poe */
    uint32 da_type:2;           /* fwd type, 0 uc, 1 mc, 2 ipv4, 3 ipv6 */
    uint32 l2_sa_hit:1;         /* sa hit */
    uint32 l2_sa_sab:1;       /* sa block */
    uint32 igr_stp_status:2;    /* span tree */
    uint32 vlan_cfi_act:2;      /* CFI action */
    uint32 vlan_aft_drop:1;      /* tag action */
    uint32 capwap_trap:1;       /* special trap */
    uint32 proto_storm_drop:1;  /* drop by protocol storm */
    uint32 eav_class_a:1;       /* eav class a */
    uint32 eav_class_b:1;       /* eav class b */
    uint32 rma_pkt:1;           /* is RMA pkt */
    uint32 rma_act:3;           /* RMA action */
    uint32 rma_byp_vlan_fltr:1; /* RMA bypass vlan */
    uint32 rma_byp_stp:1;       /* RMA bypass blocking and listning */
    uint32 rma_rsn:7;           /* RMA reason */
    uint32 page_cnt:6;          /* page counter */
    uint32 fst_dsc:12;          /* begin dsc */
    uint32 lst_dsc:12;          /* end dsc */
    uint32 rma_eapol_pkt:1;     /* eapol pkt */
    uint32 arp_pkt:1;           /* arp pkt */
    uint32 rtk_pkt:1;           /* Realtek pkt */
    uint32 rldp_rlpp_pkt:1;     /* Rldp/rlpp pkt */
    uint32 eth:1;               /* eth dm pkt */
    uint32 rma_bpdu_pkt:1;      /* bpdu pkt */
    uint32 da_bcast:1;          /* bc pkt */
    uint32 da_mcast:1;          /* mc pkt */
    uint32 sa_zero:1;           /* sa is zero */
    uint32 is_mcast_sa:1;       /* sa is mc */
    uint32 ipv4_pkt:1;          /* ipv4 pkt. From HSB */
    uint32 ipv6_pkt:1;          /* ipv6 pkt. From HSB */
                                /* original outer tag */
    uint32 opri:3;
    uint32 ocfi:1;
    uint32 ovid:12;
                                /* original inner tag */
    uint32 ipri:3;
    uint32 icfi:1;
    uint32 ivid:12;
    uint32 otag_exist:1;        /* original otag if */
    uint32 itag_exist:1;        /* original itag if */
    uint32 ctag_exist:1;        /* original ctag if */
    uint32 etag_exist:1;        /* original etag if */
    uint32 rtag_exist:1;        /* origian rtag if */
    uint32 rtag_type:1;         /* rtag type */
    uint32 igr_err:1;           /* ingress error */
    uint32 sphy:6;              /* sport physical id */
    uint32 org_itpid_idx:2;         /* original itpid idx */
    uint32 org_otpid_idx:2;         /* original otpid idx */
    uint32 rfc1042:1;           /* snap pkt */
    uint32 pppoe_pkt:1;         /* pppoe pkt */
    uint32 llc_other:1;         /* llc other pkt */
    uint32 pkt_len:14;          /* pkt length */
    uint32 ip_length:16;        /* IPv4 Total length or IPv6 payload length(For Attack prevention) */
    uint32 ip_ttl:8;            /* IP header - TTL field */
    uint32 err_pkt:1;           /* error pkt */
    uint32 ipv4_chksum_ok:1;    /* ipv4 cks */
    uint32 hol_prvnt:1;         /* hol prevent */
    uint32 ipv6_mld:1;          /* ipv6 igmp pkt */
    uint32 ipv4_igmp:1;         /* ipv4 igmp pkt */
    uint32 oam_pdu:1;           /* OAM pdu */
};
typedef struct hsm_post_replication_9300_s hsm_post_replication_9300_t;

struct hsm_param_s
{
    union
    {
        struct
        {
            hsm_pre_9300_t  pre_data;
            hsm_lut_9300_t   lu_data;
            hsm_post0_9300_t post0_data;
            hsm_post_replication_9300_t post_replication_data;
        } r9300;
        struct
        {
            hsm_pre_8390_t  pre_data;
            hsm_lu_8390_t   lu_data;
        } r8390;
        struct
        {
            /*data0*/
            uint32 acl_idx0:8;  /*acl block 0 hit entry, bit7 is valid bit, and bit6-bit0 means index*/
            uint32 acl_idx1:8;  /*acl block 1 hit entry, bit7 is valid bit, and bit6-bit0 means index*/
            uint32 acl_idx2:8;  /*acl block 2 hit entry, bit7 is valid bit, and bit6-bit0 means index*/
            uint32 acl_idx3:8;  /*acl block 3 hit entry, bit7 is valid bit, and bit6-bit0 means index*/
            /*data1*/
            uint32 acl_idx4:8;  /*acl block 4 hit entry, bit7 is valid bit, and bit6-bit0 means index*/
            uint32 acl_idx5:8;  /*acl block 5 hit entry, bit7 is valid bit, and bit6-bit0 means index*/
            uint32 acl_idx6:8;  /*acl block 6 hit entry, bit7 is valid bit, and bit6-bit0 means index*/
            uint32 acl_idx7:8;  /*acl block 7 hit entry, bit7 is valid bit, and bit6-bit0 means index*/
            /*data2*/
            uint32 acl_copy_hit:1;  /*acl copy action hit*/
            uint32 acl_dpm:29;      /*acl copy dpm*/
            uint32 acl_redir:1;     /*acl redirect*/
            uint32 acl_rout:1;      /*acl routing*/
            /*data3*/
            uint32 acl_normalPri_hit:1; /*normal priority assign*/
            uint32 acl_cpuPri_hit:1;    /*cpu priority action hit*/
            uint32 acl_cpuPri:3;        /*internal priority for cpu port*/
            uint32 acl_mir_hit:1;       /*acl mirror action hit*/
            uint32 acl_mir_act:1;       /*mirror original pkt or modified pkt*/
            uint32 acl_fwd_info:13;     /*port ID[4:0]/index to multicast table[8:0] /index to  L2 table[12:0]*/
            uint32 acl_log_hit:1;       /*acl log ation hit*/
            uint32 acl_rmk_hit:1;       /*acl remark action hit*/
            uint32 acl_rmk_act:3;       /*000: Remark 802.1p priority of I-TAG
                                          001: Remark 802.1p priority of O-TAG
                                          010: Remark DSCP value
                                          011: Remark IP Precedence bits of TOS byte[IPv4 packets only]
                                          100: Copy priority from I-TAG to O-TAG
                                          101: Copy priority from O-TAG to I-TAG
                                          110: Keep 802.1p priority of I-TAG
                                          111: Keep 802.1p priority of O-TAG*/
            uint32 acl_rmk_val:6;       /*[2:0]: New 802.1p priority of I-TAG
                                          [2:0]: New 802.1p priority of O-TAG
                                          [5:0]: DSCP value
                                          [2:0]: IPv4 precedence*/
            uint32 acl_egr_tagSts_hit:1;/*acl egress tag status hit*/
            /*data4*/
            uint32 acl_ovid_hit:1;  /*ovid action hit*/
            uint32 acl_ivid_hit:1;  /*ivid action hit*/
            uint32 acl_fltr_hit:1;  /*filter action hit*/
            uint32 acl_fltr_dpm:29; /*acl filter dpm*/
            /*data5*/
            uint32 acl_force:1;     /*acl force bit*/
            uint32 acl_cpuTag:1;    /*packet carry cpu tag*/
            uint32 acl_idx_9_0:10;  /*acl drop/trap/copy action hit index*/
            uint32 acl_mir_idx:2;   /*mirror index*/
            uint32 acl_otpid_hit:1; /*acl otpid hit*/
            uint32 acl_itpid_hit:1; /*acl itpid hit*/
            uint32 acl_otpid_idx:2; /*acl otpid index*/
            uint32 acl_itpid_idx:2; /*acl itpid index*/
            uint32 acl_meter:1;     /*acl meter action hit*/
            uint32 rng_chk_ip:8;    /*ipv4 sip(8-bit)/ipv4 dip(8 bit)/ipv6 sip full(2 bit)/ipv6 dip full(2 bit)/ipv6 sip suffix(4 bit)/ipv6 dip suffix(4 bit) range checking result*/
            uint32 ale_drop:1;      /*ALE drop*/
            uint32 ale_trap:1;      /*ALE trap*/
            uint32 copyToCpu:1;     /*copy to cpu*/
            /*data6*/
            uint32 rng_chk_comm:16; /*ivid/ovid/l4sport/l4dport/packet length/fs0/fs1 range checking result*/
            uint32 rng_chk_spm:16;  /*source port mask range checking result*/
            /*data7*/
            uint32 rma_fld:1;       /*rma flood */
            uint32 l2_sa_hit:1;     /*SA look up hit*/
            uint32 l2_sa_idx:14;    /*SA look up hit index
                                      bit[13]==0,bit[12:0] is hash table index
                                      bit[13]==1,bit[12:0] is hash CAM index*/
            uint32 l2_da_lm:1;      /*l2 da look up miss*/
            uint32 l2_da_hit:1;     /*l2 address table look up hit*/
            uint32 l2_da_idx:14;    /*SA look up hit index
                                      bit[13]==0,bit[12:0] is hash table index
                                      bit[13]==1,bit[12:0] is hash CAM index*/
            /*data8*/
            uint32 dpm:29;      /*dpm*/
            uint32 int_pri:3;   /*internal priority*/
            /*data9*/
            uint32 atk_prvnt_rsn:5; /*attack type or reason*/
            uint32 eav_class_a:1;   /*eav class a pkt*/
            uint32 eav_class_b:1;   /*eav class b pkt*/
            uint32 egr_vlan_lky:1;  /* only vlan egress filter leaky*/
            uint32 slp:5;           /*source logic port*/
            uint32 cpu_pri:3;       /*to cpu priority*/
            uint32 byp_storm:1;     /*by pass storm filter*/
            uint32 byp_igr_stp:1;   /*by pass ingress stp filter*/
            uint32 copy_igrVlan_lky:1;
            uint32 vlan_fwd_base:1; /*forward base on ivid or ovid, 1:ovid, 0:ivid*/
            uint32 fid:12;          /*vlan fid*/
            /*data10*/
            uint32 ale_ovid:12;     /*ovid the ALE determined*/
            uint32 ale_ivid:12;     /*ivid the ALE determined*/
            uint32 otag_sts:2;      /*00: keep, 01:untag, 10:tag, 11: ale no determined*/
            uint32 itag_sts:2;      /*00: keep, 01:untag, 10:tag, 11: ale no determined*/
            uint32 input_qid:2;     /*00: queue0, 01:queue1, 10:queue2,  11:special queue*/
            uint32 igr_vlan_lky:1;
            uint32 dis_sa_lrn:1;    /*disbale sa learning*/
            /*data11*/
            uint32 knmc_byp_vlan_efilter:1; /*known l2/ip4/ip6 multicast by pass vlan egress filter */
            uint32 byp_igr_bwctrl:1;        /*by pass ingress bandwidth control*/
            uint32 byp_egr_stp_pmsk:29;     /*bypass egress stp port mask*/
            uint32 copy_igrStp_lky:1;       /*acl copy bypass ingress stp filter*/
            /*data12*/
            uint32 rspan_drop:1;    /*rspan drop because of incrocrect mirror setting.*/
            uint32 rma_drop:1;      /*rma drop*/
            uint32 atk_prvnt_drop:1;/*attack drop*/
            uint32 acl_lm_drop:1;   /*acl look up miss drop*/
            uint32 acl_drop:1;      /*acl drop*/
            uint32 l3_rout_drop:1;  /*routing exception drop*/
            uint32 input_q_drop:1;  /*input queue drop*/
            uint32 acl_redir_drop:1;/*acl redirect drop because invalid destination port*/
            uint32 acl_meter_drop:1;/*acl meter drop*/
            uint32 vlan_aft_drop:1; /*vlan accept type drop*/
            uint32 vlan_cfi_drop:1; /*cfi=1 drop*/
            uint32 vlan_igr_drop:1; /*vlan ingress filter drop*/
            uint32 mstp_igr_drop:1; /*mstp drop*/
            uint32 l2_invldSA_drop:1;       /*invalid SA drop*/
            uint32 sTrap_selfMac_drop:1;    /*Special drop switch MAC*/
            uint32 l2_sa_blk:1;             /*is sa block*/
            uint32 l2_portMove_drop:1;      /*port move drop*/
            uint32 l2_newSA_drop:1;         /*new SA drop*/
            uint32 l2_sys_constrt_drop:1;   /*sys mac constrain drop*/
            uint32 l2_port_constrt_drop:1;  /*port mac constrain drop*/
            uint32 l2_vlan_constrt_drop:1;  /*vlan mac constrain drop*/
            uint32 l2_da_blk:1;             /*is da block*/
            uint32 l2_lm_drop:1;            /*da look up miss drop*/
            uint32 invld_dpm_drop:1;        /*invalid dpm drop*/
            uint32 invld_mcast_dpm_drop:1;  /*invalid multicast dpm drop*/
            uint32 vlan_egr_drop:1;         /*vlan egress filter drop*/
            uint32 mstp_egr_drop:1;         /*stp egress filter drop*/
            uint32 storm_drop:1;            /*storm filter drop*/
            uint32 src_port_filter_drop:1;  /*source port filter drop*/
            uint32 isolation_drop:1;        /*port isolation drop*/
            uint32 trunk_drop:1;            /*incorrect representative port drop*/
            uint32 mir_filter_drop:1;       /*mirror filter drop*/
            /*data13*/
            uint32 acl_filter_drop:1;       /*acl filter dorp*/
            uint32 link_down_drop:1;        /*port link down drop*/
            uint32 flowCtrl_drop:1;         /*flow control drop*/
            uint32 tx_maxLen_drop:1;        /*tx pkt max length drop*/
            uint32 reason_n_drop:12;        /*other reason drop, reserved for more drop*/
            uint32 sTrap_copy:1;            /*special copy*/
            uint32 l2_sys_constrt_copy:1;   /*sys mac constrain copy*/
            uint32 l2_port_constrt_copy:1;  /*port mac constrain copy*/
            uint32 l2_vlan_constrt_copy:1;  /*vlan mac constrain copy*/
            uint32 l2_newSA_copy:1;         /*new SA copy*/
            uint32 l2_portMove_copy:1;      /*port move copy*/
            uint32 l2_lm_copy:1;            /*da look up miss copy*/
            uint32 acl_copy:1;              /*acl copy*/
            uint32 reason_n_copy:8;         /*other reason copy, reserved for more copy*/
            /*data14*/
            uint32 rma_trap:1;              /*rma trap*/
            uint32 atk_prvnt_trap:1;        /*attack trap*/
            uint32 rldp_rlpp_trap:1;        /*RLDP & RLPP trap*/
            uint32 sTrap_comm_trap:1;       /*Special trap IGMP
                                              Special trap MLD
                                              Special trap EAPOL
                                              Special trap ARP
                                              Special trap  IPV6NB*/
            uint32 acl_trap:1;              /*acl trap*/
            uint32 l3_rout_trap:1;          /*routing exception trap*/
            uint32 vlan_cfi_trap:1;         /*cfi=1 trap*/
            uint32 vlan_igr_trap:1;         /*vlan ingress filter trap*/
            uint32 sTrap_selfMac_trap:1;    /*Special trap switch MAC*/
            uint32 l2_portMove_trap:1;      /*port move trap*/
            uint32 l2_newSA_trap:1;         /*new SA trap*/
            uint32 l2_sys_constrt_trap:1;   /*sys mac constrain trap*/
            uint32 l2_port_constrt_trap:1;  /*port mac constrain trap*/
            uint32 l2_vlan_constrt_trap:1;  /*vlan mac constrain trap*/
            uint32 l2_lm_trap:1;            /*da look up miss trap*/
            uint32 reason_n_trap:17;        /*other reason trap, reserved for more trap*/
            /*data15*/
            uint32 acl_idx8:8;
            uint32 acl_idx9:8;
            uint32 acl_idx10:8;
            uint32 acl_idx11:8;
            /*data16*/
           uint32 acl_idx_10:1;
        } r8380;
        struct
        {
            uint32 ori_pe_etag_if:1;
            uint32 ori_pe_etag_pri:3;
            uint32 ori_pe_etag_dei:1;
            uint32 ori_pe_igr_ecid_base:12;
            uint32 ori_pe_etag_rsvd:2;
            uint32 ori_pe_ecid_grp:2;
            uint32 ori_pe_ecid_base:12;
            uint32 ori_pe_igr_ecid_ext:8;
            uint32 ori_pe_ecid_ext:8;
            uint32 int_pe_ecid_ext:8;
            uint32 pe_nsg:8;
            uint32 pe_lu_mode:1;
            uint32 dst_pe_ecid:22;
            uint32 est_match:1;

            uint32 hsb_sadr:13;
            uint32 hsb_eadr:13;
            uint32 pkt_len:14;
            uint32 pg_cnt:6;
            uint32 stack_if:1;
            uint32 cpu_if:1;
            uint32 of_if:1;
            uint32 of_pipeline:1;
            uint32 stack_fmt:2;
            uint32 ori_itag_if:1;
            uint32 ori_otag_if:1;
            uint32 ori_itag_vid:12;
            uint32 ori_itag_cfi:1;
            uint32 ori_itag_pri:3;
            uint32 ori_itag_tpid:2;
            uint32 ori_otag_vid:12;
            uint32 ori_otag_cfi:1;
            uint32 ori_otag_pri:3;
            uint32 ori_otag_tpid:2;
            uint32 class_a:1;
            uint32 class_b:1;
            uint32 pppoe_if:1;
            uint32 frame_type:2;
            uint32 evlan_if:1;
            uint32 ipv4_if:1;
            uint32 ipv6_if:1;
            uint32 org_ompls_if:1;
            uint32 org_impls_if:1;
            uint32 parser_cant_handle:1;
            uint32 ip6_eh_hopbyhop_err:1;
            uint32 ip6_eh_hopbyhop_exist:1;
            uint32 ip6_eh_route_exist:1;
            uint32 ip6_eh_fragment_exist:1;
            uint32 ip6_eh_esp_exist:1;
            uint32 ip6_eh_auth_exist:1;
            uint32 ip6_eh_destination_exist:1;
            uint32 ip6_eh_mobility_exist:1;
            uint32 ip6_eh_repeat:1;
            uint32 ip_prot:8;
            uint32 ip_offset_zero:1;
            uint32 ip_flag_set:1;
            uint32 l4_hdr_rdy:1;
            uint32 l4_offset:8;
            uint32 l2_err_pkt:1;
            uint32 l3_err_pkt:1;
            uint32 l3_offset:8;
            uint32 in_spn:6;
            uint32 spn:10;
            uint32 sp_trk:7;
            uint32 sp_is_trk:1;
            uint32 int_itag_hit:1;
            uint32 int_otag_hit:1;
            uint32 l2_crc:1;
            uint32 da_hit:1;
            uint32 fwd_vid_en:1;
            uint32 rma_pkt:1;
            uint32 meta_data:12;
            uint32 trk_hash:6;
            uint32 dp:2;
            uint32 internal_priority:3;
            uint32 l3_en:1;
            uint32 igr_l3_intf_id:10;
            uint32 tt_idx:9;
            uint32 tt_hit:1;
            uint32 cpu_qid:5;
            uint32 dm_pkt:1;
            uint32 dg_pkt:1;
            uint32 tx_mir_cancel:1;
            uint32 vb_iso_mbr:1;
            uint32 learn_en:1;
            uint32 fwd_vid_sel:1;
            uint32 fwd_type:3;
            uint32 rvid:12;
            uint32 fid:12;
            uint32 trap_to_local_reason:4;
            uint32 trap_to_local_cpu:1;
            uint32 copy_to_local_reason:4;
            uint32 copy_to_local_cpu:1;
            uint32 trap_to_remote_reason:4;
            uint32 trap_to_remote_cpu:1;
            uint32 copy_to_remote_reason:4;
            uint32 copy_to_remote_cpu:1;
            uint32 drop_reason:7;
            uint32 drop_pkt:1;
            uint32 local_reason:6;
            uint32 remote_reason_rx_tag_reason:6;
            uint32 fwd_port:12;
            uint32 cpu_tx_fwd_type:4;
            uint32 ctx_dpm_31_0;
            uint32 ctx_dpm_55_32:24;
            uint32 sw_unit:4;
            uint32 spf_spn:10;
            uint32 spf_is_trk:1;
            uint32 src_filter_en:1;
            uint32 as_qid:1;
            uint32 ale_as_tagsts:1;
            uint32 bp_vlan_egr:1;
            uint32 bp_stp:1;
            uint32 bp_fltr:1;
            uint32 cngst_drop:1;
            uint32 acl_act:1;
            uint32 stk_mir_hit:2;
            uint32 set_ip_rsv:1;
            uint32 igr_l3_intf_type:1;
            uint32 ts_req:1;
            uint32 mpls_encap_req:1;
            uint32 ts_index:9;
            uint32 bssid_idx:12;
            uint32 mpls_encap_idx:11;
            uint32 mpls_decap_req:1;
            uint32 ip_ttl:8;
            uint32 hsa_n21_ipri_hit:1;
            uint32 hsa_int_dscp:8;
            uint32 hsa_int_dscp_hit:1;
            uint32 mpls_decap_sta:2;
            uint32 hsa_n21_ipri:3;
            uint32 hsa_n21_opri:3;
            uint32 hsa_n21_opri_hit:1;
            uint32 mpls_tc:3;
            uint32 hsa_int_itag_if:1;
            uint32 surf_ovid:12;
            uint32 hsa_itag_pri:3;
            uint32 hsa_eacl_ipri_hit:1;
            uint32 hsa_int_otag_vid:12;
            uint32 hsa_iacl_ipri_hit:1;
            uint32 surf_ivid:12;
            uint32 hsa_otag_pri:3;
            uint32 eacl_ivid_hit:1;
            uint32 hsa_int_itag_vid:12;
            uint32 hsa_eacl_opri_hit:1;
            uint32 hsa_iacl_opri_hit:1;
            uint32 acl_otpid_idx:2;
            uint32 hsa_int_otag_cfi:1;
            uint32 hsa_int_itag_cfi:1;
            uint32 vrf_id:8;
            uint32 l3_mdf_req:1;
            uint32 oam_lbk:1;
            uint32 n21_ivid_hit:1;
            uint32 fwd_p_type:1;
            uint32 eacl_otag_hit:1;
            uint32 eacl_otag_if:1;
            uint32 hsa_int_otag_if:1;
            uint32 eacl_itag_hit:1;
            uint32 eacl_itag_if:1;
            uint32 eacl_ovid_hit:1;
            uint32 n21_ovid_hit:1;
            uint32 iacl_otpid_hit:1;
            uint32 iacl_itpid_hit:1;
            uint32 eacl_otpid_hit:1;
            uint32 eacl_itpid_hit:1;
            uint32 acl_itpid_idx:2;
            uint32 port_data:10;
            uint32 port_is_trk:1;
            uint32 invalid_sa:1;
            uint32 hash_full:1;
            uint32 l2_dyn_pmv:1;
            uint32 l2_sttc_pmv:1;
            uint32 pmv_forbid:1;
            uint32 new_sa:1;
            uint32 dm_rxidx:6;
            uint32 mac_cst:1;
            uint32 acl_idx:15;
            uint32 atk_type:5;
            uint32 cpu_rx_l2_err:1;
            uint32 cpu_rx_l3_err:1;
            uint32 cpu_rx_sflow:2;
            uint32 of_tbl_id:2;
            uint32 acl_of_hit:2;
            uint32 acl_as_qid:2;
            uint32 rma_flood_id:3;
            uint32 of_miss_tbl_id:2;
            uint32 app_type:4;
            uint8  dmac[6];
            uint8  smac[6];
            uint32 trk_uc_hash_0:6;
            uint32 trk_uc_hash_1:6;
            uint32 oampdu:1;

            uint32 ip_len:16;
            uint32 drop_trap:1;
            uint32 pkt_type:2;
            uint32 acl_info:12;
            uint32 acl_cmd:3;
            uint32 mir_idx:2;
            uint32 mir_act:2;
            uint32 mir_hit:1;
            uint32 stk_pmsk:4;
            uint32 rpf_flag:1;
            uint32 rpf_assert:1;
            uint32 ts_idx:9;
            uint32 ts_hit:1;
            uint32 oil_idx:14;
            uint32 l3vld:1;
            uint32 fwd_bssid_idx:13;
            uint32 bssid_type:1;
            uint32 wlan_rdy:1;
            uint32 port_info:12;
            uint32 mc_type:1;
            uint32 port_type:1;
            uint32 lan_rdy:1;
            uint32 da_block:1;
            uint32 l2vld:1;
            uint32 ldpm_57_32:26;
            uint32 ldpm_31_0;
            uint32 acl_mirr:5;
            uint32 stk_cdpm:4;
            uint32 stk_ndpm:4;
            uint32 post_pkt_type:2;
            uint32 hsa_sel:1;
            uint32 hsa_addr:13;
        } r9310;
        struct
        {
            uint32 hsb_sadr:13;
            uint32 hsb_eadr:13;
            uint32 pkt_len:14;
            uint32 pg_cnt:6;
            uint32 stack_if:1;
            uint32 cpu_if:1;
            uint32 of_if:1;
            uint32 of_pipeline:1;
            uint32 stack_fmt:2;
            uint32 ori_itag_if:1;
            uint32 ori_otag_if:1;
            uint32 ori_itag_vid:12;
            uint32 ori_itag_cfi:1;
            uint32 ori_itag_pri:3;
            uint32 ori_itag_tpid:2;
            uint32 ori_otag_vid:12;
            uint32 ori_otag_cfi:1;
            uint32 ori_otag_pri:3;
            uint32 ori_otag_tpid:2;
            uint32 class_a:1;
            uint32 class_b:1;
            uint32 pppoe_if:1;
            uint32 frame_type:2;
            uint32 evlan_if:1;
            uint32 ipv4_if:1;
            uint32 ipv6_if:1;
            uint32 org_ompls_if:1;
            uint32 org_impls_if:1;
            uint32 parser_cant_handle:1;
            uint32 ip6_eh_hopbyhop_err:1;
            uint32 ip6_eh_hopbyhop_exist:1;
            uint32 ip6_eh_route_exist:1;
            uint32 ip6_eh_fragment_exist:1;
            uint32 ip6_eh_esp_exist:1;
            uint32 ip6_eh_auth_exist:1;
            uint32 ip6_eh_destination_exist:1;
            uint32 ip6_eh_mobility_exist:1;
            uint32 ip6_eh_repeat:1;
            uint32 ip_prot:8;
            uint32 ip_offset_zero:1;
            uint32 ip_flag_set:1;
            uint32 l4_hdr_rdy:1;
            uint32 l4_offset:8;
            uint32 l2_err_pkt:1;
            uint32 l3_err_pkt:1;
            uint32 l3_offset:8;
            uint32 in_spn:6;
            uint32 spn:10;
            uint32 sp_trk:7;
            uint32 sp_is_trk:1;
            uint32 loopback_time:6;
            uint32 meta_data:12;
            uint32 trk_hash:6;
            uint32 dp:2;
            uint32 internal_priority:3;
            uint32 l3_en:1;
            uint32 igr_l3_intf_id:10;
            uint32 tt_idx:9;
            uint32 tt_hit:1;
            uint32 cpu_qid:5;
            uint32 dm_pkt:1;
            uint32 dg_pkt:1;
            uint32 tx_mir_cancel:1;
            uint32 vb_iso_mbr:1;
            uint32 learn_en:1;
            uint32 fwd_vid_sel:1;
            uint32 fwd_type:3;
            uint32 rvid:12;
            uint32 fid:12;
            uint32 trap_to_local_reason:4;
            uint32 trap_to_local_cpu:1;
            uint32 copy_to_local_reason:4;
            uint32 copy_to_local_cpu:1;
            uint32 trap_to_remote_reason:4;
            uint32 trap_to_remote_cpu:1;
            uint32 copy_to_remote_reason:4;
            uint32 copy_to_remote_cpu:1;
            uint32 drop_reason:7;
            uint32 drop_pkt:1;
            uint32 local_reason:6;
            uint32 remote_reason:6;
            uint32 output_data:12;
            uint32 output_type:4;
            uint32 group_idx:11;
            uint32 group:1;
            uint32 queue_data:3;
            uint32 set_queue_data:1;
            uint32 field_l4_dport:16;
            uint32 set_field_l4_dport:1;
            uint32 field_l4_sport:16;
            uint32 set_field_l4_sport:1;
            uint32 field_ip_dip;
            uint32 set_ip_rsv:1;
            uint32 l2mpls_tt:1;
            uint32 set_field_ip_dip:1;
            uint32 field_ip_sip;
            uint32 set_field_ip_sip:1;
            uint32 ip_ttl:8;
            uint32 set_field_ip_ttl:1;
            uint32 field_ip_dscp:8;
            uint32 set_field_ip_dscp:1;
            uint32 field_mpls_ttl:8;
            uint32 set_field_mpls_ttl:1;
            uint32 field_mpls_tc:3;
            uint32 set_field_mpls_tc:1;
            uint32 field_mpls_label:11;
            uint32 set_field_mpls_label:1;
            uint32 field_pcp:3;
            uint32 set_field_pcp:1;
            uint32 field_vid:12;
            uint32 set_field_vid:1;
            uint32 field_da:16;
            uint32 set_field_da:1;
            uint32 field_sa:10;
            uint32 set_field_sa:1;
            uint32 dec_ip_ttl:1;
            uint32 dec_mpls_ttl:1;
            uint32 copy_ttl_outward:1;
            uint32 push_vlan_ethtype:2;
            uint32 push_vlan:1;
            uint32 push_mpls_ethtype:1;
            uint32 push_mpls_lib_idx:11;
            uint32 push_mpls_vpn_type:1;
            uint32 push_mpls_mode:1;
            uint32 push_mpls:1;
            uint32 pop_mpls_type:1;
            uint32 pop_mpls:1;
            uint32 pop_vlan:1;
            uint32 copy_ttl_inward:1;
            uint32 to_of:1;
            uint32 apply_action:1;
            uint32 of_lookup_req:1;
            uint32 next_tab:2;
            uint32 field_ip_ttl:8;
            uint32 pkt_dscp:6;
            uint32 lb_des_time:6;
            uint32 mac_cst:1;
            uint32 acl_idx:15;
            uint32 group_hash:3;
            uint32 reserved:2;
            uint32 cpu_rx_l2_err:1;
            uint32 cpu_rx_l3_err:1;
            uint32 cpu_rx_sflow:2;
            uint32 of_tbl_id:2;
            uint32 acl_of_hit:2;
            uint32 acl_as_qid:2;
            uint32 rma_flood_id:3;
            uint32 of_miss_tbl_id:2;
            uint32 app_type:4;
            uint8  dmac[6];
            uint8  smac[6];
            uint32 trk_uc_hash_0:6;
            uint32 trk_uc_hash_1:6;
            uint32 oampdu:1;

            uint32 ori_pe_etag_if:1;
            uint32 ori_pe_etag_pri:3;
            uint32 ori_pe_etag_dei:1;
            uint32 ori_pe_igr_ecid_base:12;
            uint32 ori_pe_igr_ecid_ext:8;
            uint32 ori_pe_ecid_ext:8;
            uint32 int_pe_ecid_ext:8;
            uint32 pe_nsg:8;
            uint32 pe_lu_mode:1;
            uint32 dst_pe_ecid:22;
            uint32 est_match:1;
        } r9310_hsm_of;
    };
};
typedef struct hsm_param_s hsm_param_t;

/* flowctrl Queue used page cnt */
typedef struct rtk_dbg_queue_usedPageCnt_s
{
    uint32 cntr[RTK_MAX_NUM_OF_QUEUE];
} rtk_dbg_queue_usedPageCnt_t;

/* flowctrl Replication Queue Counter Info */
typedef struct rtk_dbg_repctQ_CntrInfo_s
{
    uint32 nonRepctCntr;
    uint32 nonRepctCntrMax;
    uint32 repctCntr;
    uint32 repctCntrMax;
    uint32 repctFCOnCntr;
    uint32 repctFCOnCntrMax;
    uint32 repctFCOffCntr;
    uint32 repctFCOffCntrMax;
    uint32 repctFCOffDropPktCntr;
    uint32 extraHsaPktCntr;
    uint32 extraHsaPktCntrMax;
} rtk_dbg_repctQ_CntrInfo_t;

/* private statistic counter index */
typedef enum rtk_dbg_mib_dbgType_e
{
#if defined(CONFIG_SDK_RTL8390)
    RTL8390_DBG_MIB_ALE_TX_GOOD_PKTS = 0,
    RTL8390_DBG_MIB_ERROR_PKTS,
    RTL8390_DBG_MIB_EGR_ACL_DROP,
    RTL8390_DBG_MIB_EGR_METER_DROP,
    RTL8390_DBG_MIB_OAM,
    RTL8390_DBG_MIB_CFM,
    RTL8390_DBG_MIB_VLAN_IGR_FLTR,
    RTL8390_DBG_MIB_VLAN_ERR,
    RTL8390_DBG_MIB_INNER_OUTER_CFI_EQUAL_1,
    RTL8390_DBG_MIB_VLAN_TAG_FORMAT,
    RTL8390_DBG_MIB_SRC_PORT_SPENDING_TREE = 10,
    RTL8390_DBG_MIB_INBW,
    RTL8390_DBG_MIB_RMA,
    RTL8390_DBG_MIB_HW_ATTACK_PREVENTION,
    RTL8390_DBG_MIB_PROTO_STORM,
    RTL8390_DBG_MIB_MCAST_SA,
    RTL8390_DBG_MIB_IGR_ACL_DROP,
    RTL8390_DBG_MIB_IGR_METER_DROP,
    RTL8390_DBG_MIB_DFLT_ACTION_FOR_MISS_ACL_AND_C2SC,
    RTL8390_DBG_MIB_NEW_SA,
    RTL8390_DBG_MIB_PORT_MOVE = 20,
    RTL8390_DBG_MIB_SA_BLOCKING,
    RTL8390_DBG_MIB_ROUTING_EXCEPTION,
    RTL8390_DBG_MIB_SRC_PORT_SPENDING_TREE_NON_FWDING,
    RTL8390_DBG_MIB_MAC_LIMIT,
    RTL8390_DBG_MIB_UNKNOW_STORM,
    RTL8390_DBG_MIB_MISS_DROP,
    RTL8390_DBG_MIB_CPU_MAC_DROP,
    RTL8390_DBG_MIB_DA_BLOCKING,
    RTL8390_DBG_MIB_SRC_PORT_FILTER_BEFORE_EGR_ACL,
    RTL8390_DBG_MIB_VLAN_EGR_FILTER = 30,
    RTL8390_DBG_MIB_SPANNING_TRE,
    RTL8390_DBG_MIB_PORT_ISOLATION,
    RTL8390_DBG_MIB_OAM_EGRESS_DROP,
    RTL8390_DBG_MIB_MIRROR_ISOLATION,
    RTL8390_DBG_MIB_MAX_LEN_BEFORE_EGR_ACL,
    RTL8390_DBG_MIB_SRC_PORT_FILTER_BEFORE_MIRROR,
    RTL8390_DBG_MIB_MAX_LEN_BEFORE_MIRROR,
    RTL8390_DBG_MIB_SPECIAL_CONGEST_BEFORE_MIRROR,
    RTL8390_DBG_MIB_LINK_STATUS_BEFORE_MIRROR,
    RTL8390_DBG_MIB_WRED_BEFORE_MIRROR = 40,
    RTL8390_DBG_MIB_MAX_LEN_AFTER_MIRROR,
    RTL8390_DBG_MIB_SPECIAL_CONGEST_AFTER_MIRROR,
    RTL8390_DBG_MIB_LINK_STATUS_AFTER_MIRROR,
    RTL8390_DBG_MIB_WRED_AFTER_MIRROR,
#endif
#if defined(CONFIG_SDK_RTL8380)
    RTL8380_DBG_MIB_ALE_TX_GOOD_PKTS,
    RTL8380_DBG_MIB_MAC_RX_DROP,
    RTL8380_DBG_MIB_ACL_FWD_DROP,
    RTL8380_DBG_MIB_HW_ATTACK_PREVENTION_DROP,
    RTL8380_DBG_MIB_RMA_DROP,
    RTL8380_DBG_MIB_VLAN_IGR_FLTR_DROP,
    RTL8380_DBG_MIB_INNER_OUTER_CFI_EQUAL_1_DROP,
    RTL8380_DBG_MIB_PORT_MOVE_DROP,
    RTL8380_DBG_MIB_NEW_SA_DROP,
    RTL8380_DBG_MIB_MAC_LIMIT_SYS_DROP,

    RTL8380_DBG_MIB_MAC_LIMIT_VLAN_DROP,
    RTL8380_DBG_MIB_MAC_LIMIT_PORT_DROP,
    RTL8380_DBG_MIB_SWITCH_MAC_DROP,
    RTL8380_DBG_MIB_ROUTING_EXCEPTION_DROP,
    RTL8380_DBG_MIB_DA_LKMISS_DROP,
    RTL8380_DBG_MIB_RSPAN_DROP,
    RTL8380_DBG_MIB_ACL_LKMISS_DROP,
    RTL8380_DBG_MIB_ACL_DROP,
    RTL8380_DBG_MIB_INBW_DROP,
    RTL8380_DBG_MIB_IGR_METER_DROP,

    RTL8380_DBG_MIB_ACCEPT_FRAME_TYPE_DROP,
    RTL8380_DBG_MIB_STP_IGR_DROP,
    RTL8380_DBG_MIB_INVALID_SA_DROP,
    RTL8380_DBG_MIB_SA_BLOCKING_DROP,
    RTL8380_DBG_MIB_DA_BLOCKING_DROP,
    RTL8380_DBG_MIB_L2_INVALID_DPM_DROP,
    RTL8380_DBG_MIB_MCST_INVALID_DPM_DROP,
    RTL8380_DBG_MIB_RX_FLOW_CONTROL_DROP,
    RTL8380_DBG_MIB_STORM_SPPRS_DROP,
    RTL8380_DBG_MIB_LALS_DROP,

    RTL8380_DBG_MIB_VLAN_EGR_FILTER_DROP,
    RTL8380_DBG_MIB_STP_EGR_DROP,
    RTL8380_DBG_MIB_SRC_PORT_FILTER_DROP,
    RTL8380_DBG_MIB_PORT_ISOLATION_DROP,
    RTL8380_DBG_MIB_ACL_FLTR_DROP,
    RTL8380_DBG_MIB_MIRROR_FLTR_DROP,
    RTL8380_DBG_MIB_TX_MAX_DROP,
    RTL8380_DBG_MIB_LINK_DOWN_DROP,
    RTL8380_DBG_MIB_FLOW_CONTROL_DROP,
#endif
#if defined(CONFIG_SDK_RTL9310)
    RTL9310_DBG_CNTR_ALE_RX_GOOD_PKTS = 0,
    RTL9310_DBG_CNTR_RX_MAX_FRAME_SIZE,
    RTL9310_DBG_CNTR_MAC_RX_DROP,
    RTL9310_DBG_CNTR_OPENFLOW_IP_MPLS_TTL,
    RTL9310_DBG_CNTR_OPENFLOW_TBL_MISS,
    RTL9310_DBG_CNTR_IGR_BW,
    RTL9310_DBG_CNTR_SPECIAL_CONGEST,
    RTL9310_DBG_CNTR_EGR_QUEUE,
    RTL9310_DBG_CNTR_RESERVED,
    RTL9310_DBG_CNTR_EGR_LINK_STATUS,
    RTL9310_DBG_CNTR_STACK_UCAST_NONUCAST_TTL = 10,
    RTL9310_DBG_CNTR_STACK_NONUC_BLOCKING_PMSK,
    RTL9310_DBG_CNTR_L2_CRC,
    RTL9310_DBG_CNTR_SRC_PORT_FILTER,
    RTL9310_DBG_CNTR_PARSER_PACKET_TOO_LONG,
    RTL9310_DBG_CNTR_PARSER_MALFORM_PACKET,
    RTL9310_DBG_CNTR_MPLS_OVER_2_LBL,
    RTL9310_DBG_CNTR_EACL_METER,
    RTL9310_DBG_CNTR_IACL_METER,
    RTL9310_DBG_CNTR_PROTO_STORM,
    RTL9310_DBG_CNTR_INVALID_CAPWAP_HEADER = 20,
    RTL9310_DBG_CNTR_MAC_IP_SUBNET_BASED_VLAN,
    RTL9310_DBG_CNTR_OAM_PARSER,
    RTL9310_DBG_CNTR_UC_MC_RPF,
    RTL9310_DBG_CNTR_IP_MAC_BINDING_MATCH_MISMATCH,
    RTL9310_DBG_CNTR_SA_BLOCK,
    RTL9310_DBG_CNTR_TUNNEL_IP_ADDRESS_CHECK,
    RTL9310_DBG_CNTR_EACL_DROP,
    RTL9310_DBG_CNTR_IACL_DROP,
    RTL9310_DBG_CNTR_ATTACK_PREVENT,
    RTL9310_DBG_CNTR_SYSTEM_PORT_LIMIT_LEARN = 30,
    RTL9310_DBG_CNTR_OAMPDU,
    RTL9310_DBG_CNTR_CCM_RX,
    RTL9310_DBG_CNTR_CFM_UNKNOWN_TYPE,
    RTL9310_DBG_CNTR_LBM_LBR_LTM_LTR,
    RTL9310_DBG_CNTR_Y_1731,
    RTL9310_DBG_CNTR_VLAN_LIMIT_LEARN,
    RTL9310_DBG_CNTR_VLAN_ACCEPT_FRAME_TYPE,
    RTL9310_DBG_CNTR_CFI_1,
    RTL9310_DBG_CNTR_STATIC_DYNAMIC_PORT_MOVING,
    RTL9310_DBG_CNTR_PORT_MOVE_FORBID = 40,
    RTL9310_DBG_CNTR_L3_CRC,
    RTL9310_DBG_CNTR_BPDU_PTP_LLDP_EAPOL_RMA,
    RTL9310_DBG_CNTR_MSTP_SRC_DROP_DISABLED_BLOCKING,
    RTL9310_DBG_CNTR_INVALID_SA,
    RTL9310_DBG_CNTR_NEW_SA,
    RTL9310_DBG_CNTR_VLAN_IGR_FILTER,
    RTL9310_DBG_CNTR_IGR_VLAN_CONVERT,
    RTL9310_DBG_CNTR_GRATUITOUS_ARP,
    RTL9310_DBG_CNTR_MSTP_SRC_DROP,
    RTL9310_DBG_CNTR_L2_HASH_FULL = 50,
    RTL9310_DBG_CNTR_MPLS_UNKNOWN_LBL,
    RTL9310_DBG_CNTR_L3_IPUC_NON_IP,
    RTL9310_DBG_CNTR_TTL,
    RTL9310_DBG_CNTR_MTU,
    RTL9310_DBG_CNTR_ICMP_REDIRECT,
    RTL9310_DBG_CNTR_STORM_CONTROL,
    RTL9310_DBG_CNTR_L3_DIP_DMAC_MISMATCH,
    RTL9310_DBG_CNTR_IP4_IP_OPTION,
    RTL9310_DBG_CNTR_IP6_HBH_EXT_HEADER,
    RTL9310_DBG_CNTR_IP4_IP6_HEADER_ERROR = 60,
    RTL9310_DBG_CNTR_ROUTING_IP_ADDR_CHECK,
    RTL9310_DBG_CNTR_ROUTING_EXCEPTION,
    RTL9310_DBG_CNTR_DA_BLOCK,
    RTL9310_DBG_CNTR_OAM_MUX,
    RTL9310_DBG_CNTR_PORT_ISOLATION,
    RTL9310_DBG_CNTR_VLAN_EGR_FILTER,
    RTL9310_DBG_CNTR_MIRROR_ISOLATE,
    RTL9310_DBG_CNTR_MSTP_DESTINATION_DROP,
    RTL9310_DBG_CNTR_L2_MC_BRIDGE,
    RTL9310_DBG_CNTR_IP_UC_MC_ROUTING_LOOK_UP_MISS = 70,
    RTL9310_DBG_CNTR_L2_UC,
    RTL9310_DBG_CNTR_L2_MC,
    RTL9310_DBG_CNTR_IP4_MC,
    RTL9310_DBG_CNTR_IP6_MC,
    RTL9310_DBG_CNTR_L3_UC_MC_ROUTE,
    RTL9310_DBG_CNTR_UNKNOWN_L2_UC_FLPM,
    RTL9310_DBG_CNTR_BC_FLPM,
    RTL9310_DBG_CNTR_VLAN_PRO_UNKNOWN_L2_MC_FLPM,
    RTL9310_DBG_CNTR_VLAN_PRO_UNKNOWN_IP4_MC_FLPM,
    RTL9310_DBG_CNTR_VLAN_PROFILE_UNKNOWN_IP6_MC_FLPM = 80,
#endif
#if defined(CONFIG_SDK_RTL9300)
    RTL9300_DBG_CNTR_OAM_PARSER     = 0,
    RTL9300_DBG_CNTR_UC_RPF,
    RTL9300_DBG_CNTR_DEI_CFI,
    RTL9300_DBG_CNTR_MAC_IP_SUBNET_BASED_VLAN,
    RTL9300_DBG_CNTR_VLAN_IGR_FILTER,

    RTL9300_DBG_CNTR_L2_UC_MC_BRIDGE,
    RTL9300_DBG_CNTR_IP4_IP6_MC_BRIDGE,
    RTL9300_DBG_CNTR_PTP,
    RTL9300_DBG_CNTR_USER_DEF_0_3,
    RTL9300_DBG_CNTR_RESERVED,

    RTL9300_DBG_CNTR_RESERVED1  = 10,
    RTL9300_DBG_CNTR_RESERVED2,
    RTL9300_DBG_CNTR_BPDU_RMA,
    RTL9300_DBG_CNTR_LACP,
    RTL9300_DBG_CNTR_LLDP,

    RTL9300_DBG_CNTR_EAPOL,
    RTL9300_DBG_CNTR_XX_RMA,
    RTL9300_DBG_CNTR_L3_IPUC_NON_IP,
    RTL9300_DBG_CNTR_IP4_IP6_HEADER_ERROR,
    RTL9300_DBG_CNTR_L3_BAD_IP,

    RTL9300_DBG_CNTR_L3_DIP_DMAC_MISMATCH = 20,
    RTL9300_DBG_CNTR_IP4_IP_OPTION,
    RTL9300_DBG_CNTR_IP_UC_MC_ROUTING_LOOK_UP_MISS,
    RTL9300_DBG_CNTR_L3_DST_NULL_INTF,
    RTL9300_DBG_CNTR_L3_PBR_NULL_INTF,

    RTL9300_DBG_CNTR_HOST_NULL_INTF,
    RTL9300_DBG_CNTR_ROUTE_NULL_INTF,
    RTL9300_DBG_CNTR_BRIDGING_ACTION,
    RTL9300_DBG_CNTR_ROUTING_ACTION,
    RTL9300_DBG_CNTR_IPMC_RPF,

    RTL9300_DBG_CNTR_L2_NEXTHOP_AGE_OUT =30,
    RTL9300_DBG_CNTR_L3_UC_TTL_FAIL,
    RTL9300_DBG_CNTR_L3_MC_TTL_FAIL,
    RTL9300_DBG_CNTR_L3_UC_MTU_FAIL,
    RTL9300_DBG_CNTR_L3_MC_MTU_FAIL,

    RTL9300_DBG_CNTR_L3_UC_ICMP_REDIR,
    RTL9300_DBG_CNTR_IP6_MLD_OTHER_ACT,
    RTL9300_DBG_CNTR_ND,
    RTL9300_DBG_CNTR_IP_MC_RESERVED,
    RTL9300_DBG_CNTR_IP6_HBH,

    RTL9300_DBG_CNTR_INVALID_SA       =40,
    RTL9300_DBG_CNTR_L2_HASH_FULL,
    RTL9300_DBG_CNTR_NEW_SA,
    RTL9300_DBG_CNTR_PORT_MOVE_FORBID,
    RTL9300_DBG_CNTR_STATIC_PORT_MOVING,

    RTL9300_DBG_CNTR_DYNMIC_PORT_MOVING,
    RTL9300_DBG_CNTR_L3_CRC,
    RTL9300_DBG_CNTR_MAC_LIMIT,
    RTL9300_DBG_CNTR_ATTACK_PREVENT,
    RTL9300_DBG_CNTR_ACL_FWD_ACTION,

    RTL9300_DBG_CNTR_OAMPDU     =50,
    RTL9300_DBG_CNTR_OAM_MUX,
    RTL9300_DBG_CNTR_TRUNK_FILTER,
    RTL9300_DBG_CNTR_ACL_DROP,
    RTL9300_DBG_CNTR_IGR_BW,

    RTL9300_DBG_CNTR_ACL_METER,
    RTL9300_DBG_CNTR_VLAN_ACCEPT_FRAME_TYPE,
    RTL9300_DBG_CNTR_MSTP_SRC_DROP_DISABLED_BLOCKING,
    RTL9300_DBG_CNTR_SA_BLOCK,
    RTL9300_DBG_CNTR_DA_BLOCK,

    RTL9300_DBG_CNTR_STORM_CONTROL  =60,
    RTL9300_DBG_CNTR_VLAN_EGR_FILTER,
    RTL9300_DBG_CNTR_MSTP_DESTINATION_DROP,
    RTL9300_DBG_CNTR_SRC_PORT_FILTER,
    RTL9300_DBG_CNTR_PORT_ISOLATION,

    RTL9300_DBG_CNTR_TX_MAX_FRAME_SIZE,
    RTL9300_DBG_CNTR_EGR_LINK_STATUS,
    RTL9300_DBG_CNTR_MAC_TX_DISABLE,
    RTL9300_DBG_CNTR_MAC_PAUSE_FRAME,
    RTL9300_DBG_CNTR_MAC_RX_DROP,                     /*LLC check failure, 48pass1,dscrunout/l2-CRC/oversize/undersize/symbol-error*/

    RTL9300_DBG_CNTR_MIRROR_ISOLATE =70,
    RTL9300_DBG_CNTR_RX_FC,
    RTL9300_DBG_CNTR_EGR_QUEUE,
    RTL9300_DBG_CNTR_HSM_RUNOUT,            /*Replication*/
    RTL9300_DBG_CNTR_ROUTING_DISABLE,

    RTL9300_DBG_CNTR_INVALID_L2_NEXTHOP_ENTRY,
    RTL9300_DBG_CNTR_L3_MC_SRC_FLT,
    RTL9300_DBG_CNTR_CPUTAG_FLT,
    RTL9300_DBG_CNTR_FWD_PMSK_NULL,
    RTL9300_DBG_CNTR_IPUC_ROUTING_LOOKUP_MISS,

    RTL9300_DBG_CNTR_MY_DEV_DROP   =80,
    RTL9300_DBG_CNTR_STACK_NONUC_BLOCKING_PMSK,     /*pmsk = 0*/
    RTL9300_DBG_CNTR_STACK_PORT_NOT_FOUND,
    RTL9300_DBG_CNTR_ACL_LOOPBACK_DROP,
    RTL9300_DBG_CNTR_IP6_ROUTING_EXT_HEADER,
#endif

    RTK_DBG_MIB_DBG_TYPE_END
}rtk_dbg_mib_dbgType_t;


/* Function Name:
 *      hal_dumpHsb
 * Description:
 *      Dump hsb paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_dumpHsb(uint32 unit);

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_dumpHsb_openflow
 * Description:
 *      Dump OpenFlow mode hsb paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_dumpHsb_openflow(uint32 unit);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_dumpHsa
 * Description:
 *      Dump hsa paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_dumpHsa(uint32 unit);
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_OpenFlow mode dumpHsa_openflow
 * Description:
 *      Dump hsa paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_dumpHsa_openflow(uint32 unit);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_dumpHsm
 * Description:
 *      Dump hsm paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_dumpHsm(uint32 unit);
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_dumpHsm_openflow
 * Description:
 *      Dump OpenFlow mode hsm paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_dumpHsm_openflow(uint32 unit);
#endif

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_dumpHsmIdx
 * Description:
 *      Dump hsm paramter of the specified device.
 * Input:
 *      unit - unit id
 *      index -hsm index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_dumpHsmIdx(uint32 unit, uint32 index);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getDbgCntr
 * Description:
 *      Get debug counter of the specified device.
 * Input:
 *      unit  - unit id
 *      type  - debug counter type
 * Output:
 *      pCntr - value of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_getDbgCntr(uint32 unit, rtk_dbg_mib_dbgType_t type, uint32 *pCntr);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310)  || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlIgrPortUsedPageCnt
 * Description:
 *      Get flow control ingress used page count of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pCntr    - value of used page count
 *      pMaxCntr - value of maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_getFlowCtrlIgrPortUsedPageCnt(uint32 unit, rtk_port_t port, uint32 * pCntr, uint32 * pMaxCntr);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310)  || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlEgrPortUsedPageCnt
 * Description:
 *      Get flow control egress used page count of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pCntr    - value of used page count
 *      pMaxCntr - value of maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_getFlowCtrlEgrPortUsedPageCnt(uint32 unit, rtk_port_t port, uint32 *pCntr, uint32 *pMaxCntr);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310)  || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlSystemUsedPageCnt
 * Description:
 *      Get flow control system used page count of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pCntr    - value of used page count
 *      pMaxCntr - value of maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_getFlowCtrlSystemUsedPageCnt(uint32 unit, uint32 *pCntr, uint32 *pMaxCntr);
#endif

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlSystemPublicUsedPageCnt
 * Description:
 *      Get flow control system public used page count of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pCntr    - value of used page count
 *      pMaxCntr - value of maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_getFlowCtrlSystemPublicUsedPageCnt(uint32 unit, uint32 *pCntr, uint32 *pMaxCntr);
#endif

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlSystemPublicResource_get
 * Description:
 *      Get flow control system total public resource of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pCntr    - value of total public resource
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_getFlowCtrlSystemPublicResource_get(uint32 unit, uint32 *pCntr);
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_getFlowCtrlSystemIgrQueueUsedPageCnt
 * Description:
 *      Get flow control system ingress queue used page count of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pCntr    - value of used page count
 *      pMaxCntr - value of maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_getFlowCtrlSystemIgrQueueUsedPageCnt(uint32 unit, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr);
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_getFlowCtrlPortIgrQueueUsedPageCnt
 * Description:
 *      Get flow control ingress queue used page count of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pQCntr    - value of queue used page count
 *      pQMaxCntr - value of queue maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_getFlowCtrlPortIgrQueueUsedPageCnt(uint32 unit, rtk_port_t port, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310)  || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlPortQueueUsedPageCnt
 * Description:
 *      Get flow control egress queue used page count of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pQCntr    - value of queue used page count
 *      pQMaxCntr - value of queue maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_getFlowCtrlPortQueueUsedPageCnt(uint32 unit, rtk_port_t port, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_resetFlowCtrlIgrPortUsedPageCnt
 * Description:
 *      Reset flow control ingress used page count of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_resetFlowCtrlIgrPortUsedPageCnt(uint32 unit, rtk_port_t port);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_resetFlowCtrlEgrPortUsedPageCnt
 * Description:
 *      Reset flow control egress used page count including port and queue of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_resetFlowCtrlEgrPortUsedPageCnt(uint32 unit, rtk_port_t port);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_resetFlowCtrlSystemUsedPageCnt
 * Description:
 *      Reset flow control system used page count of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_resetFlowCtrlSystemUsedPageCnt(uint32 unitr);
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_resetFlowCtrlRepctQueueUsedPageCnt
 * Description:
 *      Reset flow control replication queue used page count of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_resetFlowCtrlRepctQueueUsedPageCnt(uint32 unitr);
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_getWatchdogCnt
 * Description:
 *      Display phyWatchdog, serdesWatchdog and PktbufWatchdog.
 * Input:
 *      unit     - unit id
 *      type    - Which Watchdog value
 * Output:
 *      count   - counter's value
 * Return:
 *      RT_ERR_OK
 */
extern int32 hal_getWatchdogCnt(uint32 unit, uint32 type, uint32 * count);

/* Function Name:
 *      hal_setWatchdogMonitorEnable
 * Description:
 *      Eanble/Disable watchdog monitor
 * Input:
 *      enable - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 */
extern int32 hal_setWatchdogMonitorEnable(uint32 enable);
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_getFlowCtrlRepctQueueCntrInfo
 * Description:
 *      Get flow control replication queue used page count and relative counter info of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pRepctCntr    - the pointer of counter values
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_getFlowCtrlRepctQueueCntrInfo(uint32 unit, rtk_dbg_repctQ_CntrInfo_t *pRepctCntr);

/* Function Name:
 *      hal_repctQueueEmptyStatus_get
 * Description:
 *      Get ALE replication queue empty status.
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable - pointer to empty status of replication queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_repctQueueEmptyStatus_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      hal_repctQueueStickEnable_get
 * Description:
 *      Get ALE replication queue stuck function status.
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable - pointer to enable status of replication queue stuck function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_repctQueueStickEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      hal_repctQueueStickEnable_set
 * Description:
 *      Set ALE replication queue stuck function status.
 * Input:
 *      unit     - unit id
 *      enable - status of replication queue stuck function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_repctQueueStickEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      hal_repctQueueStickEnable_set
 * Description:
 *      Get ALE replication queue fetch function status.
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable - pointer to enable status of replication queue fetch function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_repctQueueFetchEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      hal_repctQueueStickEnable_set
 * Description:
 *      Set ALE replication queue fetch function status.
 * Input:
 *      unit     - unit id
 *      enable - status of replication queue fetch function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 hal_repctQueueFetchEnable_set(uint32 unit, rtk_enable_t enable);
#endif

/* Function Name:
 *      mac_debug_sds_rxCali
 * Description:
 *      execute SerDes rx calibration.
 * Input:
 *      unit          - unit id
 *      sds          - SerDes id
 *      retryCnt   - retry count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
mac_debug_sds_rxCali(uint32 unit, uint32 sds, uint32 retryCnt);

/* Function Name:
 *      mac_debug_sds_rxCaliEnable_set
 * Description:
 *      Enable the SerDes rx calibration
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      enable      - rx calibration enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
mac_debug_sds_rxCaliEnable_set(uint32 unit, uint32 sds, rtk_enable_t enable);

/* Function Name:
 *      mac_debug_sds_rxCaliEnable_get
 * Description:
 *      Get the SerDes rx calibration enable status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pEnable      - pointer to x calibration enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
mac_debug_sds_rxCaliEnable_get(uint32 unit, uint32 sds, uint32 *pEnable);

/* Function Name:
 *      mac_debug_sds_rxCaliStatus_get
 * Description:
 *      Get the SerDes rx calibration status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
mac_debug_sds_rxCaliStatus_get(uint32 unit, uint32 sds, rtk_port_phySdsRxCaliStatus_t *pStatus);

/* Function Name:
 *      mac_debug_sds_rxCali_debugEnable_set
 * Description:
 *      Enable debug msg for SerDes rx calibration
 * Input:
 *      enable      - enable print debug msg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
mac_debug_sds_rxCali_debugEnable_set(rtk_enable_t enable);

#endif  /* __HAL_MAC_DEBUG_H__ */
