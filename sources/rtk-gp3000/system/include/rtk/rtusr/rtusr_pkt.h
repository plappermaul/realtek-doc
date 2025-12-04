/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Realtek Switch SDK Rtusr API Module
 *
 * Feature : The file include the debug tools
 *           1) packet send/receive
 *
 */

#ifndef __RTUSR_PKT_H__
#define __RTUSR_PKT_H__

/*
 * Include Files
 */

/*
 * Symbol Definition
 */
#if 0
/* pkt_dbg_tx_info is reference to tx_info in re8686.h */
struct pkt_dbg_tx_info{
    union{
        struct{
            uint32 own:1;//31
            uint32 eor:1;//30
            uint32 fs:1;//29
            uint32 ls:1;//28
            uint32 ipcs:1;//27
            uint32 l4cs:1;//26
            uint32 keep:1;//25
            uint32 blu:1;//24
            uint32 crc:1;//23
            uint32 vsel:1;//22
            uint32 dislrn:1;//21
            uint32 cputag_ipcs:1;//20
            uint32 cputag_l4cs:1;//19
            uint32 cputag_psel:1;//18
            uint32 rsvd:1;//17
            uint32 data_length:17;//0~16
        }bit;
        uint32 dw;//double word
    }opts1;
    uint32 addr;
    union{
        struct{
            uint32 cputag:1;//31
            uint32 aspri:1;//30
            uint32 cputag_pri:3;//27~29
            uint32 tx_vlan_action:2;//25~26
            uint32 tx_pppoe_action:2;//23~24
            uint32 tx_pppoe_idx:3;//20~22
            uint32 efid:1;//19
            uint32 enhance_fid:3;//16~18
            uint32 vidl:8;//8~15
            uint32 prio:3;//5~7
            uint32 cfi:1;// 4
            uint32 vidh:4;//0~3
        }bit;
        uint32 dw;//double word
    }opts2;
    union{
        struct{
            uint32 extspa:3;//29~31
            uint32 tx_portmask:6;//23~28
            uint32 tx_dst_stream_id:7;//16~22
            uint32 rsvd:12;// 4~15
            uint32 sb:1;// 3
            uint32 rsvd0:1;// 2
            uint32 l34keep:1;// 1
            uint32 ptp:1;//0
        }bit;
        uint32 dw;//double word
    }opts3;
    union{
        uint32 dw;
    }opts4;
};

struct pkt_dbg_rx_info{
    union{
        struct{
            uint32 own:1;//31
            uint32 eor:1;//30
            uint32 fs:1;//29
            uint32 ls:1;//28
            uint32 crcerr:1;//27
            uint32 ipv4csf:1;//26
            uint32 l4csf:1;//25
            uint32 rcdf:1;//24
            uint32 ipfrag:1;//23
            uint32 pppoetag:1;//22
            uint32 rwt:1;//21
            uint32 pkttype:4;//17~20
            uint32 l3routing:1;//16
            uint32 origformat:1;//15
            uint32 pctrl:1;//14
            uint32 rsvd:2;//12~13
            uint32 data_length:12;//0~11
        }bit;
        uint32 dw;//double word
    }opts1;
    uint32 addr;
    union{
        struct{
            uint32 cputag:1;//31
            uint32 ptp_in_cpu_tag_exist:1;//30
            uint32 svlan_tag_exist:1;//29
            uint32 rsvd_2:2;//27~28
            uint32 pon_stream_id:7;//20~26
            uint32 rsvd_1:3;//17~19
            uint32 ctagva:1;//16
            uint32 cvlan_tag:16;//0~15
        }bit;
        uint32 dw;//double word
    }opts2;
    union{
        struct{
            uint32 src_port_num:5;//27~31
            uint32 dst_port_mask:6;//21~26
            uint32 reason:8;//13~20
            uint32 internal_priority:3;//10~12
            uint32 ext_port_ttl_1:5;//5~9
            uint32 rsvd:5;//0~4
        }bit_apollo;
        struct{
            uint32 src_port_num:4;//28~31
            uint32 dst_port_mask:7;//21~27
            uint32 reason:8;//13~20
            uint32 internal_priority:3;//10~12
            uint32 ext_port_ttl_1:5;//5~9
            uint32 rsvd:5;//0~4
        }bit_apollofe;
        uint32 dw;//double word
    }opts3;
};


struct pkt_dbg_cputag {
    union{
        struct{
            uint16 rsvd:8;
            uint16 l3cs:1;
            uint16 l4cs:1;
            uint16 txmask_vidx:6;
        }bit;
        uint16 w;
    }word1;
    union{
        struct{
            uint16 efid:1;
            uint16 efid_value:3;
            uint16 prisel:1;
            uint16 priority:3;
            uint16 keep:1;
            uint16 vsel:1;
            uint16 dislrn:1;
            uint16 psel:1;
            uint16 sb;
            uint16 rsvd:1;
            uint16 l34keep:1;
            uint16 rsvd1:1;
        }bit;
        uint16 w;
    }word2;
    union{
        struct{
            uint16 extspa:3;
            uint16 pppoeact:2;
            uint16 pppoeidx:3;
            uint16 l2br:1;
            uint16 pon_streamid:7;
        }bit;
        uint16 w;
    }word3;
    uint16 rsvd;
};

struct pkt_dbg_rx_cputag {
    union{
        struct{
            uint16 etherType:16;
        }bit;
        uint16 w;
    }word1;
    union{
        struct{
            uint16 protocol:8;
            uint16 reason:8;
        }bit;
        uint16 w;
    }word2;
    union{
        struct{
            uint16 priority:3;
            uint16 ttl_1_ext_port_mask:5;
            uint16 l3r:1;
            uint16 org:1;
            uint16 rsvd:1;
            uint16 spa:5;
        }bit;
        uint16 w;
    }word3;
    union{
        struct{
            uint16 rsvd:2;
            uint16 ptcl:1;
            uint16 pon_stream_id:7;
            uint16 ext_port_mask:6;
        }bit;
        uint16 w;
    }word4;
};
#else
/* pkt_dbg_tx_info is reference to tx_info in re8686.h */
struct pkt_dbg_tx_info{
    uint32 own;
    uint32 eor;
    uint32 fs;
    uint32 ls;
    uint32 ipcs;
    uint32 l4cs;
    uint32 keep;
    uint32 blu;
    uint32 crc;
    uint32 vsel;
    uint32 dislrn;
    uint32 cputag_ipcs;
    uint32 cputag_l4cs;
    uint32 cputag_psel;
    uint32 data_length;
    uint32 addr;
    uint32 cputag;
    uint32 aspri;
    uint32 cputag_pri;
    uint32 tx_vlan_action;
    uint32 tx_pppoe_action;
    uint32 tx_pppoe_idx;
    uint32 efid;
    uint32 enhance_fid;
    uint32 vidl;
    uint32 prio;
    uint32 cfi;
    uint32 vidh;
    uint32 extspa;
    uint32 tx_portmask;
    uint32 tx_dst_stream_id;
    uint32 sb;
    uint32 l34keep;
    uint32 ptp;
    uint32 tx_svlan_action;
    uint32 tx_cvlan_action;
    uint32 tpid_sel;
    uint32 stag_aware;
    uint32 lgsen;
    uint32 lgmtu;
    uint32 svlan_vidl;
    uint32 svlan_prio;
    uint32 svlan_cfi;
    uint32 svlan_vidh;
    uint32 tx_gmac;
    /* ---------------------------------- 
     * LSO parameter
     */
    struct {
        uint32 lenfix_en;
        uint32 bypass_en;
        uint32 udp_en;
        uint32 tcp_en;
        uint32 ipv6_en;
        uint32 ipv4_en;
        uint32 segment_en;
        uint32 unknow_tag_en;
        uint32 segment_size;
        uint32 packetSize;
    } lso_param;
    struct {
        uint32 lenfix_en:1;
        uint32 bypass_en:1;
        uint32 udp_en:1;
        uint32 tcp_en:1;
        uint32 ipv6_en:1;
        uint32 ipv4_en:1;
        uint32 segment_en:1;
        uint32 unknow_tag_en:1;
        uint32 segment_size:1;
        uint32 packetSize:1;
        uint32 reserved:22;
    } lso_param_valid;
    
    /* ---------------------------------- 
     * Header a
     */
    struct {
        uint32 cmd_asel;
        uint32 cmd_op;
        uint32 cmd_mode;
        uint32 ptp_type;
        uint32 cmd_offset;
        uint32 udp_cksum_offset;
        uint32 cpu_flg;
        uint32 deep_q;
        uint32 pol_grp_id;
        uint32 pol_id;
        uint32 pol_en;
        uint32 mark;
        uint32 mirror;
        uint32 no_drop;
        uint32 rx_pkt_type;
        uint32 drop_code;
        uint32 mcgid;
        uint32 hdr_type;
        uint32 fe_bypass;
        uint32 pkt_size;
        uint32 lspid;
        uint32 ldpid;
        uint32 cos;
    } header_a;
    struct {
        uint32 cmd_asel:1;
        uint32 cmd_op:1;
        uint32 cmd_mode:1;
        uint32 ptp_type:1;
        uint32 cmd_offset:1;
        uint32 udp_cksum_offset:1;
        uint32 cpu_flg:1;
        uint32 deep_q:1;
        uint32 pol_grp_id:1;
        uint32 pol_id:1;
        uint32 pol_en:1;
        uint32 mark:1;
        uint32 mirror:1;
        uint32 no_drop:1;
        uint32 rx_pkt_type:1;
        uint32 drop_code:1;
        uint32 mcgid:1;
        uint32 hdr_type:1;
        uint32 fe_bypass:1;
        uint32 pkt_size:1;
        uint32 lspid:1;
        uint32 ldpid:1;
        uint32 cos:1;
        uint32 reserved:9;
    } header_a_valid;

    /* ---------------------------------- 
     * Header CPU
     */
    struct {
        uint32 crc32;
        uint32 crc16;
        uint32 trap_rsn;
        uint32 sw_idx;
    } header_cpu;
    struct {
        uint32 crc32:1;
        uint32 crc16:1;
        uint32 trap_rsn:1;
        uint32 sw_idx:1;
        uint32 reserved:28;
    } header_cpu_valid;
};

struct pkt_dbg_rx_info{
    uint32 own;
    uint32 eor;
    uint32 fs;
    uint32 ls;
    uint32 crcerr;
    uint32 ipv4csf;
    uint32 l4csf;
    uint32 rcdf;
    uint32 ipfrag;
    uint32 pppoetag;
    uint32 rwt;
    uint32 pkttype;
    uint32 l3routing;
    uint32 origformat;
    uint32 pctrl;
    uint32 data_length;
    uint32 addr;
    uint32 cputag;
    uint32 ptp_in_cpu_tag_exist;
    uint32 svlan_tag_exist;
    uint32 pon_stream_id;
    uint32 ctagva;
    uint32 cvlan_tag;
    uint32 src_port_num;
    uint32 dst_port_mask;
    uint32 reason;
    uint32 internal_priority;
    uint32 ext_port_ttl_1;
    uint32 dw[4];
    uint32 svlan_exist;
    uint32 pon_sid_or_extspa;
    uint32 fbi;
    uint32 fb_hash;
};

struct pkt_dbg_cputag {
    /* List all possible Tx CPU tag fields for all supported ICs */
    uint16 l3cs;
    uint16 l4cs;
    uint16 txmask_vidx;
    uint16 efid;
    uint16 efid_value;
    uint16 prisel;
    uint16 priority;
    uint16 keep;
    uint16 vsel;
    uint16 dislrn;
    uint16 psel;
    uint16 sb;
    uint16 l34keep;
    uint16 extspa;
    uint16 pppoeact;
    uint16 pppoeidx;
    uint16 l2br;
    uint16 pon_streamid;
    uint16 tx_gmac;
};

struct pkt_dbg_rx_cputag {
    /* List all possible Rx CPU tag fields for all supported ICs */
    uint16 etherType;
    uint16 protocol;
    uint16 reason;
    uint16 priority;
    uint16 ttl_1_ext_port_mask;
    uint16 l3r;
    uint16 org;
    uint16 spa;
    uint16 ptcl;
    uint16 pon_stream_id;
    uint16 ext_port_mask;
};

struct pkt_dbg_lso_param {
    /* List all possible lso parameters for all supported ICs */
    uint32 lenfix_en;
    uint32 bypass_en;
    uint32 udp_en;
    uint32 tcp_en;
    uint32 ipv6_en;
    uint32 ipv4_en;
    uint32 segment_en;
    uint32 unknow_tag_en;
    uint32 segment_size;
    uint32 packetSize;
};

struct pkt_dbg_header_a {
    /* List all possible header A fields for all supported ICs */
    uint32 cmd_asel;
    uint32 cmd_op;
    uint32 cmd_mode;
    uint32 ptp_type;
    uint32 cmd_offset;
    uint32 udp_cksum_offset;
    uint32 cpu_flg;
    uint32 deep_q;
    uint32 pol_grp_id;
    uint32 pol_id;
    uint32 pol_en;
    uint32 mark;
    uint32 mirror;
    uint32 no_drop;
    uint32 rx_pkt_type;
    uint32 drop_code;
    uint32 mcgid;
    uint32 hdr_type;
    uint32 fe_bypass;
    uint32 pkt_size;
    uint32 lspid;
    uint32 ldpid;
    uint32 cos;
};

struct pkt_dbg_header_cpu {
    /* List all possible header CPU fields for all supported ICs */
    uint32 crc32;
    uint32 crc16;
    uint32 trap_rsn;
    uint32 sw_idx;
};
#endif

/*
 * Macro Declaration
 */
#define PKT_DEBUG_PKT_LENGTH_MAX        1536
#define PKT_DEBUG_PKT_MACADDR_OFFSET    12
#define PKT_DEBUG_PKT_MIN               14

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

extern int32 rtk_pkt_rxDump_get(uint8 *pPayload, uint16 max_length, uint16 *pPkt_length, struct pkt_dbg_rx_info *pInfo, uint16 *pEnable);
extern int32 rtk_pkt_rxFifoDump_get(uint16 fifo_idx, uint8 *pPayload);
extern int32 rtk_pkt_rxDumpEnable_set(uint32 enable);
extern int32 rtk_pkt_rxDump_clear(void);
extern int32 rtk_pkt_txAddr_set(rtk_mac_t *pDst, rtk_mac_t *pSrc);
extern int32 rtk_pkt_txAddr_get(rtk_mac_t *pDst, rtk_mac_t *pSrc);
extern int32 rtk_pkt_txPkt_set(uint16 pos, uint8 *pPayload, uint16 length);
extern int32 rtk_pkt_txPadding_set(uint16 start, uint16 end, uint16 length);
extern int32 rtk_pkt_txBuffer_get(uint8 *pPayload, uint16 max_length, uint16 *pPkt_length);
extern int32 rtk_pkt_txBuffer_clear(void);
extern int32 rtk_pkt_tx_send(uint32 count);
extern int32 rtk_pkt_continuouslyTx_send(uint32 enable);
extern int32 rtk_pkt_continuouslyTxCnt_set(uint32 count);
extern int32 rtk_pkt_continuouslyTxCnt_get(uint32 *pCount);
extern int32 rtk_pkt_txCpuTag_set(struct pkt_dbg_cputag cputag);
extern int32 rtk_pkt_txCpuTag_get(struct pkt_dbg_cputag *pCputag);
extern int32 rtk_pkt_txCmd_set(struct pkt_dbg_tx_info inTxInfo);
extern int32 rtk_pkt_txCmd_get(struct pkt_dbg_tx_info *pOutTxInfo);
extern int32 rtk_pkt_txCmdState_set(rtk_enable_t enable);
extern int32 rtk_pkt_txCmdState_get(rtk_enable_t *pEnable);
#if defined(CONFIG_SDK_RTL9607C)
extern int32 rtk_pkt_rxGmacNum_set(uint32 gmac);
extern int32 rtk_pkt_rxGmacNum_get(uint32 *pGmac);
#endif
extern int32 rtk_pkt_txLsoParam_set(struct pkt_dbg_lso_param inLsoParam, struct pkt_dbg_lso_param inLsoParamValid);
extern int32 rtk_pkt_txLsoParam_get(struct pkt_dbg_lso_param *pOutLsoParam, struct pkt_dbg_lso_param *pOutLsoParamValid);
extern int32 rtk_pkt_txLsoParam_clear(void);
extern int32 rtk_pkt_txHeaderA_set(struct pkt_dbg_header_a inHeaderA, struct pkt_dbg_header_a inHeaderAValid);
extern int32 rtk_pkt_txHeaderA_get(struct pkt_dbg_header_a *pOutHeaderA, struct pkt_dbg_header_a *pOutHeaderAValid);
extern int32 rtk_pkt_txHeaderA_clear(void);
extern int32 rtk_pkt_txHeaderCpu_set(struct pkt_dbg_header_cpu inHeaderCpu, struct pkt_dbg_header_cpu inHeaderCpuValid);
extern int32 rtk_pkt_txHeaderCpu_get(struct pkt_dbg_header_cpu *pOutHeaderCpu, struct pkt_dbg_header_cpu *pOutHeaderCpuValid);

#endif /* __RTUSR_PKT_H__ */

