/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
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
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */

#ifndef __RTK_DEBUG_H__
#define __RTK_DEBUG_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>

/*
 * Symbol Definition
 */



/*
 * Data Declaration
 */

typedef struct rtk_hsb_s
{

#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
	uint32 bgdsc;
	uint32 endsc;
	uint32 spa;
	uint32 user_field_7;
	uint32 user_field_6;
	uint32 user_field_5;
	uint32 user_field_4;
	uint32 user_field_3;
	uint32 user_field_2;
	uint32 user_field_1;
	uint32 user_field_0;
    uint32 user_valid;
    uint32 default_field_3;
    uint32 default_field_2;
    uint32 default_field_1;
    uint32 default_field_0;

	uint32 if_gre_seq;
	uint32 pptp_l2tp_seq_session;

	uint32 ip_proto_nh;
	
	rtk_ipv6_addr_t dip6;
	rtk_ipv6_addr_t sip6;
	rtk_ip_addr_t dip2;
	rtk_ip_addr_t sip2;

	uint32 ip6_nh_rg;
	
	uint32 cks_ok_l4;
	uint32 cks_ok_l3;
	uint32 ttl_gt1;
	uint32 ihl_gt5;
	uint32 ttl_gt1_inner;
	uint32 ihl_gt5_inner;
	uint32 cks_ok_l4_inner;
	uint32 cks_ok_l3_inner;

	uint32 l4_type;
	uint32 pptp_l2tp_id;
	uint32 pppoe_session;
	rtk_ip_addr_t dip;
	rtk_ip_addr_t sip;

	uint32 tos_tc_inner;
	uint32 tos_tc;

	uint32 ip6_if;
	uint32 ip4_inner_if;
	uint32 ip4_if;

	uint32 ptp_if;
	uint32 oampdu;
	uint32 rldp_if;
	uint32 llc_other;

	uint32 dual_ip;
	
	uint32 pppoe_if;
	uint32 snap_if;
	uint32 ether_type;
	uint32 ctag;
	uint32 ctag_if;
	uint32 stag;
	uint32 stag_tpid;
	uint32 stag_if;
	uint32 cputag_pon_sid;
	uint32 cputag_pppoe_idx;
	uint32 cputag_pppoe_act;
	uint32 cputag_extspa;
	uint32 cputag_dirtx;
	uint32 cputag_psel;
	uint32 cputag_dislrn;
	uint32 cputag_keep;
	uint32 cputag_pri;
	uint32 cputag_prisel;
	uint32 cputag_pmsk_10_8;
	uint32 cputag_pmsk_7_0;
	uint32 cputag_if;
	rtk_mac_t sa;
	rtk_mac_t da;
	uint32 pon_idx;
	uint32 pkt_len;

	uint32 rng_parser_sel_dmac;		
	uint32 rng_parser_rx_pktlen;		
	uint32 rng_parser_mpcp_omci;
	uint32 rng_parser_ponidx;
	uint32 rng_parser_gatewaymac;
	uint32 rng_parser_myrldp;
	uint32 rng_parser_pri_ctag;
	uint32 rng_parser_ipv4_resv_mc;
	uint32 rng_parser_ipv6_resv_mc;
	uint32 rng_parser_ext_pmsk;
	uint32 rng_parser_phy_pmsk;
	uint32 rng_parser_cisco_rma_type;
	uint32 rng_parser_rma_type;
	uint32 rng_parser_icmp_if;
	uint32 rng_parser_igmp_mld_if;
	uint32 rng_parser_udp_if;
	uint32 rng_parser_tcp_if;
	uint32 rng_parser_frm_wan;
	uint32 rng_parser_frm_provider;
	uint32 rng_parser_frm_ext;
	uint32 rng_parser_frm_dspbo;
	uint32 rng_parser_dualip_err;
	uint32 rng_parser_brd;
	uint32 rng_parser_ipv6mlt;
	uint32 rng_parser_ipv4mlt;
	uint32 rng_parser_l2mlt;
	uint32 rng_parser_uni;

#else

    uint32 spa;
    uint32 user_field_15;
    uint32 user_field_14;
    uint32 user_field_13;
    uint32 user_field_12;
    uint32 user_field_11;
    uint32 user_field_10;
    uint32 user_field_9;
    uint32 user_field_8;
    uint32 user_field_7;
    uint32 user_field_6;
    uint32 user_field_5;
    uint32 user_field_4;
    uint32 user_field_3;
    uint32 user_field_2;
    uint32 user_field_1;
    uint32 user_field_0;
    uint32 user_valid;
    uint32 len_of_nhs;
    uint32 l3proto_nh;
	rtk_ipv6_addr_t dip6;
	rtk_ipv6_addr_t sip6;
	
    uint32 ip6_nh_rg;
    uint32 cks_ok_l4;
    uint32 cks_ok_l3;
    uint32 ttl_gt1;
    uint32 ttl_gt5;
    uint32 gre_if;
    uint32 icmp_if;
    uint32 igmp_if;
    uint32 udp_if;
    uint32 tcp_if;
	uint32 l4_type;
    uint32 pppoe_session;
    rtk_ip_addr_t dip;
    rtk_ip_addr_t sip;
    uint32 tc;
    uint32 tos_dscp;
    uint32 ip_rsv_mc_addr;
    uint32 ip_type;
    uint32 ip4_if;
    uint32 ip6_if;
    uint32 ptp_if;
    uint32 oampdu;
    uint32 rlpp_if;
    uint32 rldp_if;
    uint32 llc_other;
    uint32 pppoe_if;
    uint32 snap_if;
    uint32 ether_type;
    uint32 ctag;
    uint32 ctag_if;
    uint32 stag;
    uint32 stag_tpid;
    uint32 stag_if;
    uint32 cputag_dsl_vcmsk;
    uint32 cputag_pon_sid;
    uint32 cputag_l2br;
    uint32 cputag_pppoe_idx;
    uint32 cputag_pppoe_act;
    uint32 cputag_extspa;
    uint32 reserved;
    uint32 cputag_l34keep;
    uint32 reserved2;
    uint32 cputag_sb;
    uint32 cputag_psel;
    uint32 cputag_dislrn;
    uint32 cputag_vsel;
    uint32 cputag_keep;
    uint32 cputag_pri;
    uint32 cputag_prisel;
    uint32 cputag_efid;
    uint32 cputag_efid_en;
    uint32 cputag_txpmsk;
    uint32 cputag_l4c;
    uint32 cputag_l3c;
    uint32 cputag_if;
    rtk_mac_t sa;
    rtk_mac_t da;
    uint32 pon_idx;
    uint32 pkt_len;
	
#if defined(CONFIG_SDK_RTL9602C)

	uint32 par_uni;
	uint32 par_mlt;
	uint32 par_brd;
	uint32 par_l2mlt;
	uint32 par_ipv4mlt;
	uint32 par_ipv6mlt;
	uint32 par_frm_provider;
	uint32 par_frm_wan;
	uint32 par_tcp_if;
	uint32 par_udp_if;
	uint32 par_igmp_if;
	uint32 par_mld_if;
	uint32 par_l34pkt;
	uint32 par_rma_type;
	uint32 par_fld00_vld;
	uint32 par_sw_pkt;
	uint32 par_utp_pmsk;
	uint32 par_ext_pmsk;
	uint32 par_cpu_sel;
	uint32 par_ipv6_resv_mc;
	uint32 par_ipv4_resv_mc;
	uint32 par_pri_ctag;
	uint32 par_dslite_match_idx;
	uint32 par_myrldp;
	uint32 par_ponidx;
	uint32 par_mpcp_omci;
	uint32 par_rx_pktlen;
#endif

#endif
	
}rtk_hsb_t;




typedef struct rtk_hsa_s
{
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
	uint32		dbghsa_bgdsc;
	uint32		dbghsa_oq_dspbo;
	uint32		dbghsa_oq_endsc;
	uint32		dbghsa_oq_pktlen;
	uint32		dbghsa_oq_sid;
	uint32		dbghsa_oq_spa;
	uint32		dbghsa_oq_floodpkt;
	uint8 		dbghsa_oq_qid[11];
	uint32		dbghsa_oq_dpm;
	uint32		dbghsa_oq_fb;
	uint32		dbghsa_oq_drppkt;

   	uint32		dbghsa_l2hsa_mtr_da_drp;
	uint32		dbghsa_l2hsa_samib_en;
	uint32		dbghsa_l2hsa_samib_idx;
	uint32		dbghsa_l2hsa_cpu_forced_pri;

	uint32		dbghsa_l2hsa_c_vlaninfo;
	uint32		dbghsa_l2hsa_pon_c_vlaninfo;
	uint32		dbghsa_l2hsa_s_vlaninfo;
	uint32		dbghsa_l2hsa_pon_s_vlaninfo;
	uint32		dbghsa_l2hsa_endsc;
	uint32		dbghsa_l2hsa_dscp_rem_en;
	uint32		dbghsa_l2hsa_dscp_rem_val;
	uint32		dbghsa_l2hsa_trap_hash;
	uint32		dbghsa_l2hsa_c_1prem_en;
	uint32		dbghsa_l2hsa_c_1prem_val;
	uint32		dbghsa_l2hsa_c_tagcfi;
	uint32		dbghsa_l2hsa_c_tagpri;
	uint32		dbghsa_l2hsa_cputag_pri;
	uint32		dbghsa_l2hsa_datype;
	uint32		dbghsa_l2hsa_intpri;
	uint32		dbghsa_l2hsa_l2type;
	uint32		dbghsa_l2hsa_l3type;
	uint32		dbghsa_l2hsa_l4type;
	uint32		dbghsa_l2hsa_mctype;
	uint32		dbghsa_l2hsa_regen_crc;
	uint32		dbghsa_l2hsa_piso_leaky;
	uint32		dbghsa_l2hsa_s_tagdei;
	uint32		dbghsa_l2hsa_s_tagpri;
	uint32		dbghsa_l2hsa_s_tpidtype;
	uint32		dbghsa_l2hsa_userpri;

	uint32		dbghsa_epcom_pbodmp;
	uint8 		dbghsa_epcom_pboqid[5];
	uint32		dbghsa_epcom_flow_mib_en;
	uint32		dbghsa_epcom_flow_mib_idx;
	uint32		dbghsa_epcom_singleip_chksum;	
	uint32		dbghsa_epcom_keep;
	uint32		dbghsa_epcom_cpukeep;
	uint32		dbghsa_epcom_frm_fb;
	uint32		dbghsa_epcom_dmac_trans;
	uint32		dbghsa_epcom_pppoe_act;
	uint32		dbghsa_epcom_pppoe_sid;
	rtk_mac_t 	dbghsa_epcom_dmac;
	
	uint32		dbghsa_epcom_nocputag;
	uint32		dbghsa_epcom_regen_crc;
	uint32		dbghsa_epcom_dualhdr_act;
	uint32		dbghsa_epcom_dualhdr_idx;
	uint32		dbghsa_epcom_da_host_en;
	uint32		dbghsa_epcom_da_host_idx;
	uint32		dbghsa_epcom_s_tpidtype;
	uint32		dbghsa_epcom_l4type;
	uint32		dbghsa_epcom_l3type;
	uint32		dbghsa_epcom_l2type;
	uint32		dbghsa_epcom_datype;
	uint32		dbghsa_epcom_mctype;
	uint32		dbghsa_epcom_s_tag_if;
	uint32		dbghsa_epcom_s_tagpri;
	uint32		dbghsa_epcom_s_tagdei;
	uint32		dbghsa_epcom_s_tagvid;
	uint32		dbghsa_epcom_c_tag_if;
	uint32		dbghsa_epcom_c_tagpri;
	uint32		dbghsa_epcom_c_tagcfi;
	uint32		dbghsa_epcom_c_tagvid;
	uint32		dbghsa_epcom_sp2c_srcvid;
	uint32		dbghsa_epcom_intpri;
	uint32		dbghsa_epcom_dpc;
	uint32		dbghsa_epcom_dmabc;
	uint32		dbghsa_epcom_ponsid;
	uint32		dbghsa_epcom_cputag_pri;
	uint32		dbghsa_epcom_cputag_l3r;
	uint32		dbghsa_epcom_cputag_org;
	uint32		dbghsa_epcom_extspa;
	uint32		dbghsa_epcom_spa;
	uint32		dbghsa_epcom_fwdrsn;
	uint32		dbghsa_epcom_extmask_hash_idx;
	uint32		dbghsa_epcom_dpm_type;
	uint32		dbghsa_epcom_eptype;
	uint32		dbghsa_grp0_o_inf_mib_en;
	uint32		dbghsa_grp0_o_inf;
	uint32		dbghsa_grp0_smac_trans;
	uint32		dbghsa_grp0_dscp_rem_en;
	uint32		dbghsa_grp0_dscp_rem_val;
	uint32		dbghsa_grp0_c_rem_en;
	uint32		dbghsa_grp0_s_tpidtype;
	uint32		dbghsa_grp0_cvlan_info;
	uint32		dbghsa_grp0_svlan_info;

	uint32		dbghsa_ptp_act;
	uint32		dbghsa_ptp_id;
	uint8 		dbghsa_ptp_sec[6];
	uint32		dbghsa_ptp_nsec;

	uint32		dbghsa_omci_dev_id;
	uint32		dbghsa_omci_cont_len;

	uint32		dbghsa_grp1_o_inf_mib_en;
	uint32		dbghsa_grp1_o_inf;
	uint32		dbghsa_grp1_smac_trans;
	uint32		dbghsa_grp1_dscp_rem_en;
	uint32		dbghsa_grp1_dscp_rem_val;
	uint32		dbghsa_grp1_c_rem_en;
	uint32		dbghsa_grp1_s_tpidtype;
	uint32		dbghsa_grp1_cvlan_info;
	uint32		dbghsa_grp1_svlan_info;

	uint32		dbghsa_nat_srcmod;

	uint32		dbghsa_nat_l3trans;
	uint32		dbghsa_nat_l4chksum;
	uint32		dbghsa_nat_l3chksum;
	uint32		dbghsa_nat_newport;
	ipaddr_t	dbghsa_nat_newip;

#if defined(CONFIG_SDK_RTL9603CVD)
    uint32		hsa_acl_reason[6];
    uint32      hsa_cf_reason;
#endif
    
#else

    uint32 rng_nhsab_endsc;
    uint32 rng_nhsab_bgdsc;
    uint32 rng_nhsab_qid;
	uint32 rng_nhsab_floodpkt;
    uint32 rng_nhsab_reserve;
    uint32 rng_nhsab_ipmc;
    uint32 rng_nhsab_issb;
    uint32 rng_nhsab_cpupri;
    uint32 rng_nhsab_fwdrsn;
    uint32 rng_nhsab_pon_sid;
    uint32 rng_nhsab_vc_spa;
    uint32 rng_nhsab_vc_mask;
    uint32 rng_nhsab_ext_mask;	
	uint32 rng_nhsab_l3r;	
    uint32 rng_nhsab_hostmibEn;
    uint32 rng_nhsab_hostinf;
    uint32 rng_nhsab_l34mibEn;
    uint32 rng_nhsab_l34inf;
	uint32 rng_nhsab_org;
    uint32 rng_nhsab_dpm;
    uint32 rng_nhsab_dma_spa;
    uint32 rng_nhsab_spa;
    uint32 rng_nhsab_pktlen;
    uint32 rng_nhsab_omci_pktlen;
    
    uint32 rng_nhsac_org_cvid;
    uint32 rng_nhsac_org_cpri;
    uint32 rng_nhsac_org_cfi;
    uint32 rng_nhsac_cact_tag;
    uint32 rng_nhsac_untagset;
    uint32 rng_nhsac_ctag_ponact;
    uint32 rng_nhsac_ctag_act;
    uint32 rng_nhsac_vidzero;
    uint32 rng_nhsac_pritag_if;
    uint32 rng_nhsac_ctag_if;
    uint32 rng_nhsac_ponvid;
    uint32 rng_nhsac_vid;
    uint32 rng_nhsac_cfi;
    uint32 rng_nhsac_cact_nop;
    uint32 rng_nhsac_ponpri;
    uint32 rng_nhsac_pri;

    uint32 rng_nhsac_tagpri;
    uint32 rng_nhsac_tagcfi;
    uint32 rng_nhsac_tagvid;
    uint32 rng_nhsac_1p_rem_en;
    uint32 rng_nhsac_1p_rem_pon_en;

   
    uint32 rng_nhsas_mdy_svid_pon;
    uint32 rng_nhsas_mdy_svid;
    uint32 rng_nhsas_stag_if;
    uint32 rng_nhsas_stag_type;
	uint32 rng_nhsas_untagset;
    uint32 rng_nhsas_sp2s;
	uint32 rng_nhsas_ponact;

	uint32 rng_nhsas_dei;	
    uint32 rng_nhsas_vidsel;
    uint32 rng_nhsas_frctag;
    uint32 rng_nhsas_frctag_if;
    uint32 rng_nhsas_ponsvid;
    uint32 rng_nhsas_svid;
    uint32 rng_nhsas_svidx;
    uint32 rng_nhsas_pkt_spri;
    uint32 rng_nhsas_ponspri;
    uint32 rng_nhsas_spri;
    uint32 rng_nhsas_sp2cvid;
    uint32 rng_nhsas_sp2cact;
    uint32 rng_nhsas_tpid_type;
    uint32 rng_nhsas_tagpri;
    uint32 rng_nhsas_tagdei;
    uint32 rng_nhsas_tagvid;
	
    uint32 rng_nhsam_cputag_if;
    uint32 rng_nhsam_user_pri;
    uint32 rng_nhsam_intpri;
    uint32 rng_nhsam_1p_rem_pon;
    uint32 rng_nhsam_1p_rem_en_pon;
    uint32 rng_nhsam_dscp_rem_pon;
    uint32 rng_nhsam_dscp_rem_en_pon;
    uint32 rng_nhsam_1p_rem;
    uint32 rng_nhsam_1p_rem_en;
    uint32 rng_nhsam_dscp_rem;
    uint32 rng_nhsam_dscp_rem_en;
    
    uint32 rng_nhsaf_regen_crc;
    uint32 rng_nhsaf_cpukeep;
    uint32 rng_nhsaf_keep;
    uint32 rng_nhsaf_ptp;
    uint32 rng_nhsaf_tcp;
    uint32 rng_nhsaf_udp;
    uint32 rng_nhsaf_ipv4;
    uint32 rng_nhsaf_ipv6;
    uint32 rng_nhsaf_rfc1042;
    uint32 rng_nhsaf_pppoe_if;
    uint8  rng_nhsap_ptp_resv[9];
    uint32 rng_nhsap_ptp_id;
    uint32 rng_nhsap_ptp_act;
    uint8  rng_nhsap_ptp_sec[6];
    uint32 rng_nhsap_ptp_nsec;

    uint32 rng_nhsan_l3;
    uint32 rng_nhsan_org;
    uint32 rng_nhsan_dslite_act;
    uint32 rng_nhsan_dslite_idx;
    uint32 rng_nhsan_ipmc;
    uint32 rng_nhsan_wansa;
    uint32 rng_nhsan_l2trans;
    uint32 rng_nhsan_l34trans;
    uint32 rng_nhsan_src_mode;
    uint32 rng_nhsan_pppoe_idx;
    uint32 rng_nhsan_pppoe_act;
    uint32 rng_nhsan_smac_idx;
    uint32 rng_nhsan_l3chsum;
    uint32 rng_nhsan_l3tridx;
    uint32 rng_nhsan_l4chsum;
    ipaddr_t rng_nhsan_newip;
    uint32 rng_nhsan_newport;
    rtk_mac_t rng_nhsan_newmac;
    uint32 rng_nhsan_ttlm1_extmask;
    uint32 rng_nhsan_ttlm1_pmask;
    uint32 rng_nhsan_omci_msg_type;
    uint32 rng_nhsan_omci_cont_len;
#endif    
}rtk_hsa_t;


typedef struct rtk_hsa_debug_s
{

#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
	uint32 egr_port;
	uint32 rng_hsd_cpuins;
	uint32 rng_hsd_pppoe;
	uint32 rng_hsd_tcp;
	uint32 rng_hsd_l3type;
	uint32 rng_hsd_udp;
	uint32 rng_hsd_34pppoe;
	rtk_mac_t rng_hsd_newsmac;
	uint32 rng_hsd_smactrans;
	uint32 rng_hsd_dmactrans;
	uint32 rng_hsd_l34trans;
	uint32 rng_hsd_singleip_cksum;
	uint32 rng_hsd_l3cksum;
	uint32 rng_hsd_l4cksum;
	ipaddr_t rng_hsd_newip;
	rtk_mac_t rng_hsd_newdmac;
	uint32 rng_hsd_newprt;
	uint32 rng_hsd_pppoe_act;
	uint32 rng_hsd_src_mod;
	uint32 rng_hsd_dualhdr_act;
	uint32 rng_hsd_dualhdr_idx;
	uint32 rng_hsd_sins;
	uint32 rng_hsd_styp;
	uint32 rng_hsd_stag;
	uint32 rng_hsd_cins;
	uint32 rng_hsd_ctag;
	uint32 rng_hsd_ptp;
	uint32 rng_hsd_ptpact;
	uint32 rng_hsd_ptpid;
	uint32 rng_hsd_ptpnsec;
	uint8  rng_hsd_ptpsec[6];
	uint32 rng_hsd_org;
	uint32 rng_hsd_qid;
	uint32 rng_hsd_cpupri;
	uint32 rng_hsd_l3r;
	uint32 rng_hsd_extmask_hashidx;
	uint32 rng_hsd_ponsid;
	uint32 rng_hsd_trprsn;
	uint32 rng_hsd_ttlpmsk;
	uint32 rng_hsd_dpc;
	uint32 rng_hsd_pktlen_dma;
	uint32 rng_hsd_pktlen_pla;
	uint32 rng_hsd_regencrc;
	uint32 rng_hsd_dscp_rem_en;
	uint32 rng_hsd_dscp_rem_pri;
	uint32 rng_hsd_spa;
	uint32 rng_hsd_stdsc;
	uint32 rng_hsd_txpad;
#else
    uint32 hsa_debug_ep;
    uint32 hsa_debug_dsl_vc;
    uint32 hsa_debug_34pppoe;
    rtk_mac_t hsa_debug_34smac;
    uint32 hsa_debug_ttlpmsk;
    uint32 hsa_debug_ttlexmsk;
    rtk_mac_t hsa_debug_newmac;
    uint32 hsa_debug_newprt;
    uint32 hsa_debug_newip;
    uint32 hsa_debug_l4cksum;
    uint32 hsa_debug_l3cksum;
    uint32 hsa_debug_pppoeact;
    uint32 hsa_debug_src_mod;
    uint32 hsa_debug_l34trans;
    uint32 hsa_debug_l2trans;
    uint32 hsa_debug_org;
    uint32 hsa_debug_l3r;
    uint32 hsa_debug_pkt_change;
    uint32 hsa_debug_tcp;
    uint32 hsa_debug_udp;
    uint32 hsa_debug_padg;
    uint32 hsa_debug_sv_dei;
    uint32 hsa_debug_styp;
    uint32 hsa_debug_pktlen_ori;
    uint32 hsa_debug_qid;
    uint32 hsa_debug_stdsc;
    uint32 hsa_debug_cpupri;
    uint32 hsa_debug_spri;
    uint32 hsa_debug_txins;
    uint32 hsa_debug_cori;
    uint32 hsa_debug_cmdy;
    uint32 hsa_debug_crms;
    uint32 hsa_debug_cins;
    uint32 hsa_debug_cvid;
    uint32 hsa_debug_cfi;
    uint32 hsa_debug_ptpnsec;
    uint8  hsa_debug_ptpsec[6];
    uint32 hsa_debug_ptpact;
    uint32 hsa_debug_regencrc;
    uint32 hsa_debug_pppoe;
    uint32 hsa_debug_rfc1042;
    uint32 hsa_debug_ipv6;
    uint32 hsa_debug_ipv4;
    uint32 hsa_debug_ptp;
    uint32 hsa_debug_remdscp_pri;
    uint32 hsa_debug_rem1q_pri;
    uint32 hsa_debug_ctag_pri;
    uint32 hsa_debug_remdscp_en;
    uint32 hsa_debug_rem1q_en;
    uint32 hsa_debug_svid;
    uint32 hsa_debug_instag;
    uint32 hsa_debug_inctag;
    uint32 hsa_debug_pktlen;
    uint32 hsa_debug_spa;
    uint32 hsa_debug_dpc;
    uint32 hsa_debug_extmsk;
    uint32 hsa_debug_vcmsk;
    uint32 hsa_debug_ponsid;
    uint32 hsa_debug_trprsn;

    uint32 hsa_debug_txpad;
	uint32 hsa_debug_dmaspa;
	uint32 hsa_debug_dscp_rem_pri;
	uint32 hsa_debug_dscp_rem_en;
	uint32 hsa_debug_pktlen_pla;
	uint32 hsa_debug_pktlen_dma;
	uint32 hsa_debug_issb;

	uint32 hsa_debug_ptpid;
	
	uint32 hsa_debug_ctag;
	
	uint32 hsa_debug_stag;
	uint32 hsa_debug_sins;
	uint32 hsa_debug_egr_port;

	uint32 hsa_debug_dslite_idx;
	uint32 hsa_debug_dslite_act;
	
	uint32 hsa_debug_pppoe_act;
	
    rtk_mac_t hsa_debug_newdmac;
	
    rtk_mac_t hsa_debug_newsmac;
	uint32 hsa_debug_l34pppoe;
	uint32 hsa_debug_cpuins;
#endif

    
}rtk_hsa_debug_t;



#endif /* __RTK_DEBUG_H__ */

