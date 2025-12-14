/*
 * Copyright (c) Cortina-Access Limited 2015.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * aal_ni_l2.h: Hardware Abstraction Layer for NI -> L2 FE driver 
 *
 */
#ifndef __AAL_NI_L2_H__
#define __AAL_NI_L2_H__

#include "ca_types.h"


/* NI port MACROs */
//#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
//#define  AAL_NI_PORT_NUM  2  /* refer to APB0_NI_HV_PT_COUNT */
//#else
#define  AAL_NI_PORT_NUM  5  /* refer to APB0_NI_HV_PT_COUNT */
//#endif

#define  AAL_NI_PORT_GE0  0  /* 1 Gbps NI Port (Integrated GPHY) */
#define  AAL_NI_PORT_GE1  1  /* 1 Gbps NI Port (Integrated GPHY) */
#define  AAL_NI_PORT_GE2  2  /* 1 Gbps NI Port (Integrated GPHY) */
#define  AAL_NI_PORT_GE3  3  /* 1 Gbps NI Port (Integrated GPHY) */
#define  AAL_NI_PORT_GE4  4  /* 1 Gbps NI Port (RGMII) */
#define  AAL_NI_PORT_XGE0 5  /* 10 Gbps XFI NI Port (RXAUI/2.5GSGMII/RGMII) */
#define  AAL_NI_PORT_XGE1 6  /* 10 Gbps XFI NI Port (LAN) */
#define  AAL_NI_PORT_XGE2 7  /* 10 Gbps XFI NI Port (WAN) */

/* NI Frame length */
#define  AAL_NI_PKT_LEN_MIN  64   /* mininal Frame size, include FCS */
#define  AAL_NI_PKT_LEN_MAX  9201 /* to support Jumbo frame */

/* Tx FIFO threshold: Transmit FIFO Packet Transfer Start Threshold value */
#define  AAL_NI_PORT_TX_FIFO_THOLD_MAX   0x1FF

/* Transmit MAC defers for 8 + tx_ipg_sel bytes after sending a frame */
#define  AAL_NI_PORT_TX_IPG_SEL_MAX       7

/* NI MCE block MACROs */
#if 0//Yocto, G3?
#define  AAL_NI_MC_GROUP_NUM 1024  /* Max groups for packet replication */
#else//uboot for 98F @ FPGA
#define  AAL_NI_MC_GROUP_NUM 256        /* Max groups for packet replication */
#endif

#define AAL_NI_XAUI_IFG_DIC8_MIN_5         5
#define AAL_NI_XAUI_IFG_DIC8_MAX_11        11
#define AAL_NI_XAUI_IFG_DIC12_MIN_12       12
#define AAL_NI_XAUI_IFG_DIC12_MAX_15       15

typedef enum {
    AAL_NI_RGMII_CLK_SEL_EXT = 0,   /* Clock is sourced from external source and is input to chip */
    AAL_NI_RGMII_CLK_SEL_REF = 1    /* Clock is sourced from reference clock and given out of chip. */
} aal_ni_rgmii_clock_sel_t;

typedef enum {
    AAL_NI_PHY_MODE_MAC = 0,/* Ethernet interface connect to PHY */
    AAL_NI_PHY_MODE_PHY = 1 /* Ethernet interface connect to MAC */
} aal_ni_phy_mode_t;

typedef enum {
    AAL_NI_IF_MODE_GE_GMII  = 0, /* 1xGE, GMII mode */
    AAL_NI_IF_MODE_FE_MII   = 1, /* 1xFE, MII mode */
    AAL_NI_IF_MODE_GE_RGMII = 2, /* 1xGE, RGMII(Reduced Gigabit MII) mode */
    AAL_NI_IF_MODE_FE_RGMII = 3, /* 1xFE, RGMII(Reduced Gigabit MII) mode */
    AAL_NI_IF_MODE_GE_TBI   = 4, /* 1XGE, Ten Bit Interface mode */
    AAL_NI_IF_MODE_FE_RMII  = 5, /* 1xFE, RMII(Reduced MII) mode */
    AAL_NI_IF_MODE_NBT      = 6, /* N-Base-T mode */
    AAL_NI_IF_MODE_FE_SSSMII= 7, /* 1xFE, SSSMII(Source Sync Serial MII) mode */
    AAL_NI_IF_MODE_RXAUI    = 8, /* 10xGE, RXAUI mode */
    AAL_NI_IF_MODE_XGMII    = 8, /* 10XGE, XGMII-XFI mode */
    AAL_NI_IF_MODE_SXGMII   = 8, /* 10XGE, SXGMII-XFI mode */
    AAL_NI_IF_MODE_GE_SGMII = 9, /* 1xGE, SGMII mode */
    AAL_NI_IF_MODE_HISGMII  = 10 /* 2.5xGE, HISGMII mode */
} aal_ni_if_mode_t;

typedef enum {
    AAL_NI_MAC_LPBK_MODE_NONE = 0,  /* No loopback */
    AAL_NI_MAC_LPBK_MODE_RX2TX = 1, /* Ethernet Rx-to-Tx (Shallow) Loopback after MAC */
    AAL_NI_MAC_LPBK_MODE_TX2RX = 2  /* Ethernet Tx-to-Rx (Deep) Loopback (after MAC) */
} aal_ni_mac_lpbk_mode_t;

typedef enum {
    AAL_NI_MAC_DUPLEX_HALF = 0, /* Half duplex */
    AAL_NI_MAC_DUPLEX_FULL = 1  /* Full duplex */

} aal_ni_mac_duplex_t;

typedef enum {
    AAL_NI_MAC_SPEED_10M   = 0, /* 100 Mbps  */
    AAL_NI_MAC_SPEED_100M  = 1, /* 10 Mbps   */
    AAL_NI_MAC_SPEED_1000M = 2, /* 10 Mbps   */
    AAL_NI_MAC_SPEED_OTHER = 3  /* reserved  */

} aal_ni_mac_speed_t;

typedef enum {
    AAL_NI_WOL_PASSWORD_MODE_6B = 0,/* indicates the 6 bytes of password to be detected for WOL packets */
    AAL_NI_WOL_PASSWORD_MODE_4B = 1 /* indicates the 4 bytes of password to be detected for WOL packets */
} aal_ni_wol_password_mode_t;

typedef enum {
    AAL_NI_WOL_DA_CHK_DISABLE = 0,  /* DA is not checked for detecting WOL packets. */
    AAL_NI_WOL_DA_CHK_MC_BC = 1,    /* WOL packets are detected only if the DA is multicast address (also includes broadcast address) */
    AAL_NI_WOL_DA_CHK_ONLY_BC = 2   /* WOL packets are detected only if the DA is broadcast address of 'hFF_FF_FF_FF_FF_FF */
} aal_ni_wol_da_chk_t;

typedef enum {
    AAL_NI_XAUI_IFG_MODE_DIC12 = 0,   /* DIC12 mode, the IPG range is from 9 to 15 */
    AAL_NI_XAUI_IFG_MODE_DIC8 = 1,    /* DIC8 mode, the IPG range is from 5 to 11 */
    AAL_NI_XAUI_IFG_MODE_DISABLE = 2    /* disable DIC function , and IPG must be larger than 16 */
} aal_ni_xaui_ifg_mode_t;

typedef union {
    struct {
#   ifdef CA_BIG_ENDIAN
        ca_uint32 rsrvd1     :27;
        ca_uint32 intf_rst_p4:1;    /* bits 4:4 */
        ca_uint32 intf_rst_p3:1;    /* bits 3:3 */
        ca_uint32 intf_rst_p2:1;    /* bits 2:2 */
        ca_uint32 intf_rst_p1:1;    /* bits 1:1 */
        ca_uint32 intf_rst_p0:1;    /* bits 0:0 */
#   else            /* CA_LITTLE_ENDIAN */
        ca_uint32 intf_rst_p0:1;    /* bits 0:0 */
        ca_uint32 intf_rst_p1:1;    /* bits 1:1 */
        ca_uint32 intf_rst_p2:1;    /* bits 2:2 */
        ca_uint32 intf_rst_p3:1;    /* bits 3:3 */
        ca_uint32 intf_rst_p4:1;    /* bits 4:4 */
        ca_uint32 rsrvd1     :27;
#   endif
    } s;

    ca_uint32_t u32;

} aal_ni_eth_if_reset_ctrl_mask_t;

typedef struct {
    ca_boolean_t intf_rst_p0;   /* reset port 0 */
    ca_boolean_t intf_rst_p1;   /* reset port 1 */
    ca_boolean_t intf_rst_p2;   /* reset port 2 */
    ca_boolean_t intf_rst_p3;   /* reset port 3 */
    ca_boolean_t intf_rst_p4;   /* reset port 4 */
} aal_ni_eth_if_reset_ctrl_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN

        ca_uint32_t rsvd:20;
        ca_uint32_t mac_addr6:1;
        ca_uint32_t sup_tx_to_rx_lpbk_data:1;
        ca_uint32_t lpbk_mode:1;
        ca_uint32_t crs_polarity:1;
        ca_uint32_t smii_tx_stat:1;
        ca_uint32_t tx_use_gefifo:1;
        ca_uint32_t inv_rxclk_out:1;
        ca_uint32_t inv_clk_out:1;
        ca_uint32_t inv_clk_in:1;
        ca_uint32_t rmii_clksrc:1;
        ca_uint32_t phy_mode:1;
        ca_uint32_t int_cfg:1;

#else               /* CA_LITTLE_ENDIAN */

        ca_uint32_t int_cfg:1;
        ca_uint32_t phy_mode:1;
        ca_uint32_t rmii_clksrc:1;
        ca_uint32_t inv_clk_in:1;
        ca_uint32_t inv_clk_out:1;
        ca_uint32_t inv_rxclk_out:1;
        ca_uint32_t tx_use_gefifo:1;
        ca_uint32_t smii_tx_stat:1;
        ca_uint32_t crs_polarity:1;
        ca_uint32_t lpbk_mode:1;
        ca_uint32_t sup_tx_to_rx_lpbk_data:1;
        ca_uint32_t mac_addr6:1;
        ca_uint32_t rsvd:20;
#endif
    } s;

    ca_uint32_t u32;

} aal_ni_eth_if_config_mask_t;

typedef struct {
    ca_uint32_t  int_cfg;  /* refer to aal_ni_if_mode_t */
    ca_uint32_t  phy_mode; /* refer to aal_ni_phy_mode_t */
    ca_uint32_t  rmii_clksrc;/* refer to aal_ni_rgmii_clock_sel_t */
    ca_boolean_t inv_clk_in;/* Inverts the selected clock to sample data in or not */
    ca_boolean_t inv_clk_out;/* Gives out inverted selected clock or not */
    ca_boolean_t inv_rxclk_out;/* Gives out inverted selected receive clock or not */
    ca_boolean_t tx_use_gefifo;/* use GE FIFO or not, Valid only in 1xFE mode */
    ca_uint32_t  smii_tx_stat;/* giving status or zeros out on Tx Data in SSSMII MAC mode*/
    ca_boolean_t crs_polarity;/* High indicates CRS or not */
    ca_uint32_t  lpbk_mode;/* refer to aal_ni_mac_lpbk_mode_t*/
    ca_boolean_t sup_tx_to_rx_lpbk_data;/* give out on Transmit pins or not when Tx-to-Rx loopback mode */
    ca_uint32_t  mac_addr6;/* last byte of MAC address of Port */
} aal_ni_eth_if_config_t;


typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd:29;
        ca_uint32_t max_store_pkt_len:1;
        ca_uint32_t max_pkt_size:1;
        ca_uint32_t min_pkt_size:1;

#else               /* CA_LITTLE_ENDIAN */

        ca_uint32_t min_pkt_size:1;
        ca_uint32_t max_pkt_size:1;
        ca_uint32_t max_store_pkt_len:1;
        ca_uint32_t rsvd:29;
#endif
    } s;

    ca_uint32_t u32;

} aal_ni_pkt_len_config_mask_t;

typedef struct {
    ca_uint16_t min_pkt_size;   /* Minimum allowed Rx Packet size for UNI ports, range: 64B ~ MAX */
    ca_uint16_t max_pkt_size;   /* Maximum allowed Rx Packet size for UNI ports, range: MIN ~ 16383B */
    ca_uint16_t max_store_pkt_len;  /* Maximum  packet length allowed in Golden Gate(include Tx/Rx and processing). range: 64B ~ 16383B */
} aal_ni_pkt_len_config_t;

typedef enum {
    AAL_NI_TX_PAUSE_SEL_NONE                = 0,
    AAL_NI_TX_PAUSE_SEL_RX_MAC_FIFO_THRD    = 1,
    AAL_NI_TX_PAUSE_SEL_TE_FC_THRED         = 2,
    AAL_NI_TX_PAUSE_SEL_BOTH                = 3,
    AAL_NI_TX_PAUSE_SEL_MAX                 = AAL_NI_TX_PAUSE_SEL_BOTH
}aal_ni_tx_pause_sel_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd            :10;
        ca_uint32_t tx_pause_sel    :1;
        ca_uint32_t rx_pause_en     :1;
        ca_uint32_t tx_pause_en     :1;
        ca_uint32_t speed           :1;
        ca_uint32_t duplex          :1;
        ca_uint32_t power_dwn_rx    :1;
        ca_uint32_t power_dwn_tx    :1;
        ca_uint32_t mac_tx_rst      :1;
        ca_uint32_t tx_auto_xon     :1;
        ca_uint32_t tx_pfc_disable  :1;
        ca_uint32_t tx_drain        :1;
        ca_uint32_t tx_flow_disable :1;
        ca_uint32_t tx_ipg_sel      :1;
        ca_uint32_t mac_crc_calc_en :1;
        ca_uint32_t tx_en           :1;
        ca_uint32_t mac_rx_rst      :1;
        ca_uint32_t send_pg_data    :1;
        ca_uint32_t pass_thru_hdre  :1;
        ca_uint32_t rx_pfc_disable  :1;
        ca_uint32_t rx_flow_to_tx_en:1;
        ca_uint32_t rx_flow_disable :1;
        ca_uint32_t rx_en           :1;

#else               /* CA_LITTLE_ENDIAN */

        ca_uint32_t rx_en           :1;
        ca_uint32_t rx_flow_disable :1;
        ca_uint32_t rx_flow_to_tx_en:1;
        ca_uint32_t rx_pfc_disable  :1;
        ca_uint32_t pass_thru_hdre  :1;
        ca_uint32_t send_pg_data    :1;
        ca_uint32_t mac_rx_rst      :1;
        ca_uint32_t tx_en           :1;
        ca_uint32_t mac_crc_calc_en :1;
        ca_uint32_t tx_ipg_sel      :1;
        ca_uint32_t tx_flow_disable :1;
        ca_uint32_t tx_drain        :1;
        ca_uint32_t tx_pfc_disable  :1;
        ca_uint32_t tx_auto_xon     :1;
        ca_uint32_t mac_tx_rst      :1;
        ca_uint32_t power_dwn_tx    :1;
        ca_uint32_t power_dwn_rx    :1;
        ca_uint32_t duplex          :1;
        ca_uint32_t speed           :1;
        ca_uint32_t tx_pause_en     :1;
        ca_uint32_t rx_pause_en     :1;
        ca_uint32_t tx_pause_sel    :1;
        ca_uint32_t rsvd            :10;
#endif
    } s;

    ca_uint32_t u32;

} aal_ni_eth_mac_config_mask_t;

typedef struct {
    ca_boolean_t rx_en;
    ca_boolean_t rx_flow_disable;
    ca_boolean_t rx_flow_to_tx_en;  /* Enables sending PAUSE flow control information to Tx to stop sending framesValid when rx_flow_disable is 0 */
    ca_boolean_t rx_pfc_disable;
    ca_boolean_t pass_thru_hdre;
    ca_boolean_t send_pg_data;
    ca_uint32_t mac_rx_rst;
    ca_boolean_t tx_en;
    ca_boolean_t mac_crc_calc_en;
    ca_uint32_t tx_ipg_sel;
    ca_boolean_t tx_flow_disable;
    ca_boolean_t tx_drain;
    ca_boolean_t tx_pfc_disable;
    ca_boolean_t tx_auto_xon;
    ca_boolean_t mac_tx_rst;
    ca_boolean_t power_dwn_tx;
    ca_boolean_t power_dwn_rx;
    ca_uint32_t duplex;
    ca_uint32_t speed;
    ca_boolean_t tx_pause_en;
    ca_boolean_t rx_pause_en;
    aal_ni_tx_pause_sel_t tx_pause_sel;
} aal_ni_eth_mac_config_t;

typedef struct {
    ca_uint64_t rx_uc_pkt_cnt;  /* Rx good unicast frame counter */
    ca_uint64_t rx_mc_frame_cnt;    /* Rx good multicast frames counter */
    ca_uint64_t rx_bc_frame_cnt;    /* Rx good broadcast frames counter */
    ca_uint64_t rx_oam_frame_cnt;   /* Rx good EFM OAM i.e.EthType=0x8809 frames counter */
    ca_uint64_t rx_jumbo_frame_cnt; /* Rx good MAC layer Jumbo i.e.Type=0x8870 frames counter */
    ca_uint64_t rx_pause_frame_cnt; /* Rx good MAC layer PAUSE frames counter */
    ca_uint64_t rx_pfc_frame_cnt;   /* Rx good MAC layer PFC frames counter */
    ca_uint64_t rx_unk_opcode_frame_cnt;    /* Rx good MAC Control frame with unknown Opcode */
    ca_uint64_t rx_crc_error_frame_cnt; /* Rx bad CRC frame counter */
    ca_uint64_t rx_undersize_frame_cnt; /* Rx Undersize frame counter */
    ca_uint64_t rx_runt_frame_cnt;  /* Rx Undersize frame with CRC error counter */
    ca_uint64_t rx_ovsize_frame_cnt;    /* Rx Oversize Frame counter */
    ca_uint64_t rx_jabber_frame_cnt;    /* Rx Oversize Frame with CRC error counter */
    ca_uint64_t rx_invalid_frame_cnt;   /* Rx invalid frames due to length exceed max storable size or internal FIFO overrun condition */
    ca_uint64_t rx_invalid_flow_frame_cnt;  /* Rx invalid flow frames. Invalid flow frames are pause packet with matching Ethnet Type and opcode field but length not equal to 64 */
    ca_uint64_t rx_frame_64_cnt;    /* Rx received good frames with size 64 bytes */
    ca_uint64_t rx_frame_65to127_cnt;   /* Rx received good frames with size 65-127 bytes */
    ca_uint64_t rx_frame_128to255_cnt;  /* Rx received good frames with size 128-255 bytes */
    ca_uint64_t rx_frame_256to511_cnt;  /* Rx received good frames with size 256-511 bytes */
    ca_uint64_t rx_frame_512to1023_cnt; /* Rx received good frames with size 512-1023 bytes */
    ca_uint64_t rx_frame_1024to1518_cnt;    /* Rx received good frames with size 1024-1518 bytes */
    ca_uint64_t rx_frame_1519to2100_cnt;    /* Rx received good frames with size 1519-2100 bytes */
    ca_uint64_t rx_frame_2101to9200_cnt;    /* Rx received good frames with size 2101-9200 bytes */
    ca_uint64_t rx_frame_9201tomax_cnt; /* Rx received good frames with size 9201-Max bytes */
    ca_uint64_t rx_byte_cnt_low;    /* Rx byte counter low word */
    ca_uint64_t rx_byte_cnt_high;   /* Rx byte counter high word */

    ca_uint64_t tx_uc_pkt_cnt;  /* Tx good unicast frame counter */
    ca_uint64_t tx_mc_frame_cnt;    /* Tx good multicast frames counter */
    ca_uint64_t tx_bc_frame_cnt;    /* Tx good broadcast frames counter */
    ca_uint64_t tx_oam_frame_cnt;   /* Tx EFM OAM frames counter */
    ca_uint64_t tx_jumbo_frame_cnt; /* Tx MAC Jumbo i.e.EthType=0x8870 frames counter */
    ca_uint64_t tx_pause_frame_cnt; /* Tx MAC layer PAUSE frames counter */
    ca_uint64_t tx_pfc_frame_cnt;   /* Tx MAC layer PFC frames counter */
    ca_uint64_t tx_crc_error_frame_cnt; /* Tx bad CRC frame counter */
    ca_uint64_t tx_ovsize_frame_cnt;    /* Tx Oversize Frame counter */
    ca_uint64_t tx_single_col_frame_cnt;    /* Count of transmitted frames with exactly one collision */
    ca_uint64_t tx_multi_col_frame_cnt; /* Count of transmitted frames with more than one collision  */
    ca_uint64_t tx_late_col_frame_cnt;  /* Count of frames in which late collision was detected */
    ca_uint64_t tx_rsvd_1;  /* Reserved stats 1 */
    ca_uint64_t tx_exess_col_frame_cnt; /* Count of frames with excess collisions */
    ca_uint64_t tx_frame_64_cnt;    /* Tx transmitted good frames with size 64 bytes */
    ca_uint64_t tx_frame_65to127_cnt;   /* Tx transmitted good frames with size 65-127 bytes */
    ca_uint64_t tx_frame_128to255_cnt;  /* Tx transmitted good frames with size 128-255 bytes */
    ca_uint64_t tx_frame_256to511_cnt;  /* Tx transmitted good frames with size 256-511 bytes */
    ca_uint64_t tx_frame_512to1023_cnt; /* Tx transmitted good frames with size 512-1023 bytes */
    ca_uint64_t tx_frame_1024to1518_cnt;    /* Tx transmitted good frames with size 1024-1518 bytes */
    ca_uint64_t tx_frame_1519to2100_cnt;    /* Tx transmitted good frames with size 1519-2100 bytes */
    ca_uint64_t tx_frame_2101to9200_cnt;    /* Tx transmitted good frames with size 2101-9200 bytes */
    ca_uint64_t tx_frame_9201tomax_cnt; /* Tx transmitted good frames with size 9201-Max. */
    ca_uint64_t tx_rsvd_2;  /* Reserved stats 2 */
    ca_uint64_t tx_byte_cnt_low;    /* Tx byte counter low word */
    ca_uint64_t tx_byte_cnt_high;   /* Tx byte counter high word */

} aal_ni_mib_stats_t;


typedef union {
    struct {
#   ifdef CA_BIG_ENDIAN

        ca_uint32_t rxctrl_byp_en:1;
        ca_uint32_t rxctrl_byp_cos:1;
        ca_uint32_t rxctrl_byp_dpid:1;
        ca_uint32_t pfc_drop_dis:1;
        ca_uint32_t vlan_tag_num:1;
        ca_uint32_t linkstat_drop_dis:1;
        ca_uint32_t oam_drop_dis:1;
        ca_uint32_t ukopcode_drop_dis:1;
        ca_uint32_t flow_drop_dis:1;
        ca_uint32_t runt_drop_dis:1;
        ca_uint32_t ovr_drop_dis:1;
        ca_uint32_t crc_drop_dis:1;
        ca_uint32_t ff_overrun_drop_dis:1;
        ca_uint32_t rsrvd:11;

#   else            /* CA_LITTLE_ENDIAN */

        ca_uint32_t rsrvd:11;
        ca_uint32_t ff_overrun_drop_dis:1;
        ca_uint32_t crc_drop_dis:1;
        ca_uint32_t ovr_drop_dis:1;
        ca_uint32_t runt_drop_dis:1;
        ca_uint32_t flow_drop_dis:1;
        ca_uint32_t ukopcode_drop_dis:1;
        ca_uint32_t oam_drop_dis:1;
        ca_uint32_t linkstat_drop_dis:1;
        ca_uint32_t vlan_tag_num:1;
        ca_uint32_t pfc_drop_dis:1;
        ca_uint32_t rxctrl_byp_dpid:1;
        ca_uint32_t rxctrl_byp_cos:1;
        ca_uint32_t rxctrl_byp_en:1;

#   endif
    } s;
    ca_uint32_t u32;

} aal_ni_port_rx_ctrl_mask_t;

typedef struct {
    ca_boolean_t ff_overrun_drop_dis;   /* Disable frame dropping when FIFO overrun */
    ca_boolean_t crc_drop_dis;  /* Disable CRC error frame dropping */
    ca_boolean_t ovr_drop_dis;  /* Disable packeing dropping when Rx FIFO overrun */
    ca_boolean_t runt_drop_dis; /* Disable runt frame dropping */
    ca_boolean_t flow_drop_dis; /* Disable PAUSE frame dropping */
    ca_boolean_t ukopcode_drop_dis; /* Disable unkown OP code packet dropping */
    ca_boolean_t oam_drop_dis;  /* Disable OAM dropping */
    ca_boolean_t linkstat_drop_dis; /* Disable packeing dropping when ethernet port is link down(MAC link status) */
    ca_uint32_t vlan_tag_num;   /* Support VLAN Tag number for insert */
    ca_boolean_t pfc_drop_dis;  /* Mark packet with drop indication when it is a PFC(Priority Flow Control) packet */
    ca_boolean_t rxctrl_byp_dpid;   /* destination Port ID for bypass mode */
    ca_boolean_t rxctrl_byp_cos;    /* COS for bypass mode */
    ca_boolean_t rxctrl_byp_en; /* Enable bypass Forwarding engine */

} aal_ni_port_rx_ctrl_t;



typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd:23;
        ca_uint32_t ni_wol_int:1;
        ca_uint32_t rx_lpi_stat_chg:1;
        ca_uint32_t lpi_assert_stat_chg:1;
        ca_uint32_t txmib_cntmsb_set:1;
        ca_uint32_t rxmib_cntmsb_set:1;
        ca_uint32_t rxcntrl_overrun:1;
        ca_uint32_t txfifo_overrun:1;
        ca_uint32_t txfifo_underrun:1;
        ca_uint32_t link_stat_chg:1;

#else               /* CA_LITTLE_ENDIAN */

        ca_uint32_t link_stat_chg:1;
        ca_uint32_t txfifo_underrun:1;
        ca_uint32_t txfifo_overrun:1;
        ca_uint32_t rxcntrl_overrun:1;
        ca_uint32_t rxmib_cntmsb_set:1;
        ca_uint32_t txmib_cntmsb_set:1;
        ca_uint32_t lpi_assert_stat_chg:1;
        ca_uint32_t rx_lpi_stat_chg:1;
        ca_uint32_t ni_wol_int:1;
        ca_uint32_t rsvd:23;
#endif
    } s;
    ca_uint32_t u32;

} aal_ni_port_intr_mask_t;

typedef struct {
    ca_boolean_t link_stat_chg;
    ca_boolean_t txfifo_underrun;
    ca_boolean_t txfifo_overrun;
    ca_boolean_t rxcntrl_overrun;
    ca_boolean_t rxmib_cntmsb_set;
    ca_boolean_t txmib_cntmsb_set;
    ca_boolean_t lpi_assert_stat_chg;
    ca_boolean_t rx_lpi_stat_chg;
    ca_boolean_t ni_wol_int;
} aal_ni_port_intr_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd:28;
        ca_uint32_t hi_threshold:1;
        ca_uint32_t low_threshold:1;
        ca_uint32_t mid_threshold:1;
        ca_uint32_t mid_cos:1;

#else               /* CA_LITTLE_ENDIAN */

        ca_uint32_t mid_cos:1;
        ca_uint32_t mid_threshold:1;
        ca_uint32_t low_threshold:1;
        ca_uint32_t hi_threshold:1;
        ca_uint32_t rsvd:28;
#endif
    } s;
    ca_uint32_t u32;

} aal_ni_flow_ctrl_threshold_mask_t;

typedef struct {
    ca_uint16_t mid_cos;
    ca_uint16_t mid_threshold;
    ca_uint16_t low_threshold;
    ca_uint16_t hi_threshold;
} aal_ni_flow_ctrl_threshold_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd:30;
        ca_uint32_t hi_threshold:1;
        ca_uint32_t low_threshold:1;
#else               /* CA_LITTLE_ENDIAN */

        ca_uint32_t low_threshold:1;
        ca_uint32_t hi_threshold:1;
        ca_uint32_t rsvd:30;
#endif
    } s;
    ca_uint32_t u32;

} aal_ni_port_txfifo_threshold_mask_t;


typedef struct {
    ca_uint16_t low_threshold;
    ca_uint16_t hi_threshold;
} aal_ni_port_txfifo_threshold_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd:30;
        ca_uint32_t hi_threshold:1;
        ca_uint32_t low_threshold:1;
#else               /* CA_LITTLE_ENDIAN */

        ca_uint32_t low_threshold:1;
        ca_uint32_t hi_threshold:1;
        ca_uint32_t rsvd:30;
#endif
    } s;
    ca_uint32_t u32;

} aal_ni_rxmux_fc_thrshld_mask_t;


typedef struct {
    ca_uint16_t low_threshold;
    ca_uint16_t hi_threshold;
} aal_ni_rxmux_fc_thrshld_t;


typedef union {
    struct {
#ifdef CA_BIG_ENDIAN

        ca_uint32_t rsvd:11;
        ca_uint32_t cpuxram_txpkt:1;
        ca_uint32_t cpuxram_rxpkt:1;
        ca_uint32_t int_stat_mceng:1;
        ca_uint32_t int_stat_ptp:1;
        ca_uint32_t cpuxram_int_stat_err:1;
        ca_uint32_t cpuxram_int_stat_cntr:1;
        ca_uint32_t int_stat_pc6:1;
        ca_uint32_t int_stat_pc4:1;
        ca_uint32_t int_stat_pc3:1;
        ca_uint32_t int_stat_pc2:1;
        ca_uint32_t int_stat_pc1:1;
        ca_uint32_t int_stat_pc0:1;
        ca_uint32_t int_stat_txem:1;
        ca_uint32_t int_stat_pspid_10:1;
        ca_uint32_t int_stat_pspid_9:1;
        ca_uint32_t int_stat_pspid_8:1;
        ca_uint32_t int_stat_pspid_4:1;
        ca_uint32_t int_stat_pspid_3:1;
        ca_uint32_t int_stat_pspid_2:1;
        ca_uint32_t int_stat_pspid_1:1;
        ca_uint32_t int_stat_pspid_0:1;

#else               /* CA_LITTLE_ENDIAN */

        ca_uint32_t int_stat_pspid_0:1;
        ca_uint32_t int_stat_pspid_1:1;
        ca_uint32_t int_stat_pspid_2:1;
        ca_uint32_t int_stat_pspid_3:1;
        ca_uint32_t int_stat_pspid_4:1;
        ca_uint32_t int_stat_pspid_8:1;
        ca_uint32_t int_stat_pspid_9:1;
        ca_uint32_t int_stat_pspid_10:1;
        ca_uint32_t int_stat_txem:1;
        ca_uint32_t int_stat_pc0:1;
        ca_uint32_t int_stat_pc1:1;
        ca_uint32_t int_stat_pc2:1;
        ca_uint32_t int_stat_pc3:1;
        ca_uint32_t int_stat_pc4:1;
        ca_uint32_t int_stat_pc6:1;
        ca_uint32_t cpuxram_int_stat_cntr:1;
        ca_uint32_t cpuxram_int_stat_err:1;
        ca_uint32_t int_stat_ptp:1;
        ca_uint32_t int_stat_mceng:1;
        ca_uint32_t cpuxram_rxpkt:1;
        ca_uint32_t cpuxram_txpkt:1;
        ca_uint32_t rsvd:11;
#endif
    } s;
    ca_uint32_t u32;

} aal_ni_glb_intr_mask_t;

typedef struct {
    ca_boolean_t int_stat_pspid_0;
    ca_boolean_t int_stat_pspid_1;
    ca_boolean_t int_stat_pspid_2;
    ca_boolean_t int_stat_pspid_3;
    ca_boolean_t int_stat_pspid_4;
    ca_boolean_t int_stat_pspid_8;
    ca_boolean_t int_stat_pspid_9;
    ca_boolean_t int_stat_pspid_10;
    ca_boolean_t int_stat_txem;
    ca_boolean_t int_stat_pc0;
    ca_boolean_t int_stat_pc1;
    ca_boolean_t int_stat_pc2;
    ca_boolean_t int_stat_pc3;
    ca_boolean_t int_stat_pc4;
    ca_boolean_t int_stat_pc6;
    ca_boolean_t cpuxram_int_stat_cntr;
    ca_boolean_t cpuxram_int_stat_err;
    ca_boolean_t int_stat_ptp;
    ca_boolean_t int_stat_mceng;
    ca_boolean_t cpuxram_rxpkt;
    ca_boolean_t cpuxram_txpkt;

} aal_ni_glb_intr_t;

typedef struct {
    ca_boolean_t link_status;   /* link up or down */
    aal_ni_mac_duplex_t duplex; /* link duplex */
    aal_ni_mac_speed_t speed;   /* link speed */

} aal_eth_port_status_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd:25;
        ca_uint32_t deny_spid_en:1;
        ca_uint32_t ldpid:1;
        ca_uint32_t pol_en:1;
        ca_uint32_t pol_grp_id : 1;
        ca_uint32_t pol_id:1;
        ca_uint32_t mcgid_en:1;
        ca_uint32_t mcgid:1;

#else               /* CA_LITTLE_ENDIAN */

        ca_uint32_t mcgid:1;
        ca_uint32_t mcgid_en:1;
        ca_uint32_t pol_id:1;
        ca_uint32_t pol_grp_id : 1;
        ca_uint32_t pol_en:1;
        ca_uint32_t ldpid:1;
        ca_uint32_t deny_spid_en:1;
        ca_uint32_t rsvd:25;
#endif
    } s;

    ca_uint32_t u32;

} aal_ni_mc_fib_mask_t;

#define AAL_NI_XGE_MAC_VLAN_NUM   3
#define AAL_NI_XGE_MAC_MINLEN_MAX 127
#define AAL_NI_XGE_MAC_MAXLEN_MAX 65535

typedef enum {
    AAL_NI_XGE_MODE_1G = 0,
    AAL_NI_XGE_MODE_XG = 1
} aal_ni_xge_mode_t;

typedef enum {
    AAL_NI_XGE_CLK_312dot5M = 0,
    AAL_NI_XGE_CLK_322M     = 1
} aal_ni_xge_clk_1g_sel_t;

typedef struct {
    aal_ni_xge_mode_t        mode;
    aal_ni_xge_clk_1g_sel_t  clk;
} aal_ni_xge_misc_cfg_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd : 30;
        ca_uint32_t mode : 1;
        ca_uint32_t clk  : 1;
#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t clk  : 1;
        ca_uint32_t mode : 1;
        ca_uint32_t rsvd : 30;
#endif
    } s;
    ca_uint32_t u32;
}aal_ni_xge_misc_cfg_mask_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd                 : 19 ; 
        ca_uint32_t ptp_en               : 1;
        ca_uint32_t pps_in_en            : 1;
        ca_uint32_t pps_edge_sel_sample  : 1;
        ca_uint32_t pps_edge_sel_load    : 1;
        ca_uint32_t pps_load_en          : 1;
        ca_uint32_t pps_out_en           : 1;
        ca_uint32_t pps_width            : 1;
        ca_uint32_t ptp_sum_val          : 1;
        ca_uint32_t pps_timestamp_load   : 1;
        ca_uint32_t s_pps                : 1;
        ca_uint32_t s_tod                : 1;
        ca_uint32_t load_matching_tmr    : 1;
        ca_uint32_t load_matching_tmr_en : 1;

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t load_matching_tmr_en : 1;
        ca_uint32_t load_matching_tmr    : 1;
        ca_uint32_t s_tod                : 1;
        ca_uint32_t s_pps                : 1;
        ca_uint32_t pps_timestamp_load   : 1;
        ca_uint32_t ptp_sum_val          : 1;
        ca_uint32_t pps_width            : 1;
        ca_uint32_t pps_out_en           : 1;
        ca_uint32_t pps_load_en          : 1;
        ca_uint32_t pps_edge_sel_load    : 1;
        ca_uint32_t pps_edge_sel_sample  : 1;
        ca_uint32_t pps_in_en            : 1;
        ca_uint32_t ptp_en               : 1;
        ca_uint32_t rsvd                 : 19 ; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_ptp_pps_cfg_mask_t;



typedef struct {
    ca_boolean_t ptp_en             ;/* Enables PTP timer. PTP timer is base timer for PTP/PPS signal generation. */
    ca_boolean_t pps_in_en          ;/* Indicates that pps input to chip is valid*/
    ca_boolean_t pps_edge_sel_sample;/* Uses rising or falling edge of pps input to sample PTP timer*/
    ca_boolean_t pps_edge_sel_load  ;/* Uses rising or falling edge of pps input to load PTP timer*/
    ca_boolean_t pps_load_en        ;/* PTP timer is loaded with pps_timestamp_load value on selected edge of pps, or free running */
    ca_boolean_t pps_out_en         ;/*  Enables generation of pps_out*/
    ca_uint32_t  pps_width          ;/*  required width for pps signal.
                                        Once pps is made high, it is made low after pps_width+1 units.
                                        Each unit represents 512*8 = 4096 nsec with 125 Mhz ptp clock.
                                      */
    ca_uint32_t  ptp_sum_val        ;/* Value to be added to PTP timer LSB every cycle, 
                                        msb is aligned to 8ns.
                                        PTP timer runs on local 125 MHz 
                                        and timer value LSB is aligned to 16 ns.
                                       */
    ca_uint32_t  pps_timestamp_load ;/* Valid with pps_load_en is 1,
                                        This value is loaded to PTP timer on 
                                        selected edge of pps input
                                       */
    ca_uint32_t  s_pps              ;/* Valid with pps_out_en is 1,
                                        PTP timer value when the PPS signal out will be driven high. 
                                      */
    ca_uint32_t  s_tod              ;/* Valid with tod_en from PER block high,
                                        PTP timer value when start of ToD output will be driven high
                                      */
    ca_uint32_t  load_matching_tmr    ;/*Valid with load_matching_tmr_en is 1*/

    ca_uint32_t  load_matching_tmr_en ;/* when this value set to 1, and CSR load_matching_tmr value is matchingPTP timer,
                                         NOTE: software should write load_matching_tmr first and then setload_matching_tmr_en
                                       */                                  
} aal_ni_ptp_pps_cfg_t;

typedef struct {
    ca_boolean_t tx_rst;   /* transmit logic is reset or not */
    ca_boolean_t rx_rst;   /*receive logic is reset or not*/
    ca_boolean_t cntr_rst; /*statistics counters reset or not */
    ca_boolean_t core_rx2tx_lb; /* CORE Receive to CORE Transmit loopback */
    ca_boolean_t core_tx2rx_lb; /* CORE Transmit to CORE Receive loopback */
    ca_boolean_t stats_drop_en; /* Statistics for dropped frames (unrecognized Destination Address) configuration.
                                   When set HIGH , selective counters are updated based on accepted and dropped frames.
                                   When set LOW , selective counters are only updated based on accepted frames.
                                 */
    ca_uint8_t   stats_err_order; /* Statistics error order configuration. 
                                    IEEE 802.3 section 5.2.2 defines ahierarchical
                                    order when multiple error statuses are associated with one frame so that only one
                                    status is counted. This priority (in descending order) is as follows: 1)
                                    frameTooLong,2) alignmentError, 3) frameCheckError and 4) lengthError.
                                    Configuration bit stats_err_order[0] determines the mode of IEEE counters.
                                    Configuration bit stats_err_order[1] determines the mode of MIB counters.
                                    Configuration bit stats_err_order[2] determines the mode of RMON counters.
                                    When set LOW, all errors are reported and counted for frames with multiple
                                    errors.
                                    When set HIGH, a single prioritized error is reported and counted for frames with
                                    multiple errors.
                                    */
    ca_boolean_t clronread; /* Statistics counter clear-on-read or not */
    ca_boolean_t stack_en;  /* XGMAC is configured for EXT stacking operation */
    ca_boolean_t rpr_mode;  /* 1: RPR Transport mode, 0: Ethernet mode. */
} aal_ni_xge_cmn_cfg_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd            : 22;
        ca_uint32_t tx_rst          : 1;
        ca_uint32_t rx_rst          : 1;
        ca_uint32_t cntr_rst        : 1;
        ca_uint32_t core_rx2tx_lb   : 1;
        ca_uint32_t core_tx2rx_lb   : 1;
        ca_uint32_t stats_drop_en   : 1;
        ca_uint32_t stats_err_order : 1;
        ca_uint32_t clronread       : 1;
        ca_uint32_t stack_en        : 1;
        ca_uint32_t rpr_mode        : 1;
#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t rpr_mode        : 1;
        ca_uint32_t stack_en        : 1;
        ca_uint32_t clronread       : 1;
        ca_uint32_t stats_err_order : 1;
        ca_uint32_t stats_drop_en   : 1;
        ca_uint32_t core_tx2rx_lb   : 1;
        ca_uint32_t core_rx2tx_lb   : 1;
        ca_uint32_t cntr_rst        : 1;
        ca_uint32_t rx_rst          : 1;
        ca_uint32_t tx_rst          : 1;
        ca_uint32_t rsvd            : 22;
#endif
    } s;

    ca_uint32_t u32;
} aal_ni_xge_cmn_cfg_mask_t;

typedef struct {

    ca_boolean_t prmbl_sfd_gmii_err; /*
                                      * Determines whether RX GMII_ERR asserted during PREAMBLE/SFD causes alignmentError.
                                      *      1: Cause error, 0: Do not cause error.
                                      */
    ca_boolean_t maxlen_mode;   /*
                                 * Determines the interpretation of the MAXLEN configuration register.
                                 *      1'b0 FIX MAXLEN denotes the maximum frame length for any frame type (tagged or untagged).
                                 *      1'b1 AUTO MAXLEN denotes the maximum untagged frame length. For tagged frames, the
                                 *      maximum frame length is calculated on a frame-by-frame basis: 1 VLAN tag: MAXLEN+4,
                                 *      2 VLAN tags: MAXLEN+8, 3 VLAN tags: MAXLEN+12 and 4 VLAN tags: MAXLEN+16.
                                 */

    ca_boolean_t crc_err_fatal; /*
                                 * When set, the crc error status is contributing to the RX_ERR output to the CORE Receiver.
                                 * When clear, the crc error status is not contributing to the RX_ERR output to the CORE Receiver.
                                 * This setting does not affect the RX_CRC_ERR output to the CORE Receiver.
                                 */

    ca_boolean_t ifg_mode_rx;/*
                              * Inter Frame Gap mode. 
                              *      1'b0 STD Supports standard IEEE 802.3ae clause 4 InterFrameGap of minimum 40 Bit Times.
                              *      1'b1 BEST Supports non-standard best effort InterFrameGap of minimum 0 Bit Times. Incoming
                              *          data must comply with IEEE 802.3ae section 49 64B/66B encoding w.r.t. Start/Terminate characters.
                              */
                                 
    ca_boolean_t fcs_chk_en;     /* Enable FCS check. */ 

    ca_boolean_t len_chk_en_pad;/*
                                 * Enable frame length check of padded frames (as indicated by the Type/Length field).
                                 * Note that the Frame Maximum Length check is not affected by this setting.
                                 */
    
    ca_boolean_t len_chk_en_pause; /* Enable frame length check of pause frames */
    ca_boolean_t len_chk_en;    /* Enable frame length check of non-pause/non-padded frames. */

    ca_boolean_t pause_extract;    /* 
                                    * Pause Quanta extraction. When 0, the Pause Quanta transfer
                                    * to the MAC Transmitter is disabled. When 1, it is enabled.
                                    */

    ca_boolean_t pause_term;    /*
                                 * Pause Frame termination. When 0, Pause frames are forwarded to the CORE Receiver. When 1, Pause frames
                                 * are terminated in the MAC.
                                 */

    ca_uint8_t  strip;   /*
                          * Padding and FCS stripping setting. This setting has no effect in RPR Transport mode.
                          *      2'b00 NONE No Pad/FCS stripping
                          *      2'b01 FCS FCS stripping active
                          *      2'b1x PADFCS Pad/FCS stripping active
                          */

    ca_boolean_t drop_short; /*
                              * Short frame drop enable for frames with recognized Destination
                              * Address. Frames are classified as being 'short' if the received
                              * frame length is shorter than the value programmed in the MINLEN register.
                              */
    ca_boolean_t promis_mode; /* Promiscuous mode enable */
} aal_ni_xge_cfg_rx_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd               :19;
        ca_uint32_t prmbl_sfd_gmii_err :1;
        ca_uint32_t maxlen_mode        :1;
        ca_uint32_t crc_err_fatal      :1;
        ca_uint32_t ifg_mode_rx        :1;
        ca_uint32_t fcs_chk_en         :1;
        ca_uint32_t len_chk_en_pad     :1;
        ca_uint32_t len_chk_en_pause   :1;
        ca_uint32_t len_chk_en         :1;
        ca_uint32_t pause_extract      :1;
        ca_uint32_t pause_term         :1;
        ca_uint32_t strip              :1;
        ca_uint32_t drop_short         :1;
        ca_uint32_t promis_mode        :1;
#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t promis_mode        :1;
        ca_uint32_t drop_short         :1;
        ca_uint32_t strip              :1;
        ca_uint32_t pause_term         :1;
        ca_uint32_t pause_extract      :1;
        ca_uint32_t len_chk_en         :1;
        ca_uint32_t len_chk_en_pause   :1;
        ca_uint32_t len_chk_en_pad     :1;
        ca_uint32_t fcs_chk_en         :1;
        ca_uint32_t ifg_mode_rx        :1;
        ca_uint32_t crc_err_fatal      :1;
        ca_uint32_t maxlen_mode        :1;
        ca_uint32_t prmbl_sfd_gmii_err :1;
        ca_uint32_t rsvd               :19;
#endif
    }s;

    ca_uint32_t u32;
} aal_ni_xge_cfg_rx_mask_t;

typedef struct {
    
    ca_boolean_t vlan_pad_mode;/*
                                * Padding mode for VLAN tagged frames
                                *      1'b0 MD_64B Number of Pad octets = max(0,46 - length(DATA) - length(VLAN)).
                                *      1'b1 MD_VAR Number of Pad octets = max(0,46 - length(DATA)).
                                */
    
    ca_boolean_t lf_auto_flush_en;/*
                                   * Enable/disable the automatic flush of transmit data when 'link_fault' is asserted from the Reconciliation Sublayer.
                                   */

    ca_boolean_t pause_req_auto_xon; /*
                                      * When set, the XGMAC will automatically insert an XON frame every time
                                      * the XGMAC 'pause_req' input is deasserted (1-to-0 transition). When clear,
                                      * automatic XON frame insertion is disabled.
                                      */

    ca_boolean_t pause_req_en; /* Enable/disable the XGMAC 'pause_req' input */
    ca_boolean_t insert_sa;  /* Insert Source Address field enable.*/  

    ca_boolean_t halt;/*
                       * When set, the transmitter is halted (on a frame boundary) and
                       * the pause frame generation logic is reset. This setting has
                       * higher priority than 'link_fault' in the transmitter.
                       */
    
    ca_uint8_t ifg_mode_tx; /*
                             * Inter Frame Gap mode for 10G operation. Note that the Inter Frame Gap mode is always STD for 1G operation.
                             *      2'b00 STD STD Mode: Insert Idle. Minimum IFG=CFG_TX_IFG[ifg], will insert 1, 2 or 3 idles to align.
                             *      2'b01 DIC12 DIC Mode: Deficit Idle Counter. Minimum IFG=9. IFG oscillates between 9-15 during back-to-back traffic with the average being IFG=12.
                             *      2'b10 DIC8 DIC Mode: Deficit Idle Counter. Minimum IFG=5. IFG oscillates between 5-11 during back-to-back traffic with the average being IFG=8.
                             *      2'b11 RSVD Reserved
                             */
    ca_uint8_t  fifo1_read_thr; /* FIFO1 read threshold, The XGMII Formatter decides on
                                  starting reading FIFO1 based on two factors: 1) there
                                  is a complete packet in FIFO1 or 2) there is at least
                                   treshold in FIFO1. */
    ca_uint8_t  fifo1_thr_afull;/* FIFO1 almost full threshold */
    ca_uint16_t ifg; /* 1G at least 1, 10G at least 5 */
} aal_ni_xge_cfg_tx_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd               :22;
        ca_uint32_t vlan_pad_mode      :1;
        ca_uint32_t lf_auto_flush_en   :1;
        ca_uint32_t pause_req_auto_xon :1;
        ca_uint32_t pause_req_en       :1;
        ca_uint32_t insert_sa          :1;
        ca_uint32_t halt               :1;
        ca_uint32_t ifg_mode_tx        :1;
        ca_uint32_t fifo1_read_thr     :1;
        ca_uint32_t fifo1_thr_afull    :1;
        ca_uint32_t ifg                :1;

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t ifg                :1;
        ca_uint32_t fifo1_thr_afull    :1;
        ca_uint32_t fifo1_read_thr     :1;
        ca_uint32_t ifg_mode_tx        :1;
        ca_uint32_t halt               :1;
        ca_uint32_t insert_sa          :1;
        ca_uint32_t pause_req_en       :1;
        ca_uint32_t pause_req_auto_xon :1;
        ca_uint32_t lf_auto_flush_en   :1;
        ca_uint32_t vlan_pad_mode      :1;
        ca_uint32_t rsvd               :22;
#endif
    } s;

    ca_uint32_t u32;
} aal_ni_xge_cfg_tx_mask_t;


#define  AAL_XGE_MAC_INTR_EN_BMP_RX_NO_SFD         0x0001
#define  AAL_XGE_MAC_INTR_EN_BMP_TX_PAUSE_DONE     0x0002
#define  AAL_XGE_MAC_INTR_EN_BMP_TX_FIFO_OVERFLOW  0x0004
#define  AAL_XGE_MAC_INTR_EN_BMP_TX_FIFO_UNDERFLOW 0x0008

typedef enum {
    AAL_XGE_MAC_STATS_ID_TX_GOOD_FRAMES = 0x00,
    AAL_XGE_MAC_STATS_ID_TX_GOOD_OCTETS = 0x01,
    AAL_XGE_MAC_STATS_ID_TX_PAUSE_FRAMES = 0x02,
    AAL_XGE_MAC_STATS_ID_RX_GOOD_FRAMES = 0x03,
    AAL_XGE_MAC_STATS_ID_RX_GOOD_OCTETS = 0x04,
    AAL_XGE_MAC_STATS_ID_RX_PAUSE_FRAMES = 0x05,
    AAL_XGE_MAC_STATS_ID_RX_CRC_ERR_FRAMES = 0x06,
    AAL_XGE_MAC_STATS_ID_RX_ALIGN_ERR_FRAMES = 0x07,
    AAL_XGE_MAC_STATS_ID_TX_UCAST_FRAMES = 0x08,
    AAL_XGE_MAC_STATS_ID_TX_MCAST_FRAMES = 0x09,
    AAL_XGE_MAC_STATS_ID_TX_BCAST_FRAMES = 0x0a,
    AAL_XGE_MAC_STATS_ID_TX_ERR_FRAMES = 0x0b,
    AAL_XGE_MAC_STATS_ID_RX_UCAST_FRAMES = 0x0c,
    AAL_XGE_MAC_STATS_ID_RX_MCAST_FRAMES = 0x0d,
    AAL_XGE_MAC_STATS_ID_RX_BCAST_FRAMES = 0x0e,
    AAL_XGE_MAC_STATS_ID_RX_ERR_FRAMES = 0x0f,
    AAL_XGE_MAC_STATS_ID_RX_OCTETS = 0x10,
    AAL_XGE_MAC_STATS_ID_RX_FRAMES = 0x11,
    AAL_XGE_MAC_STATS_ID_RX_RUNT_FRAMES = 0x12,
    AAL_XGE_MAC_STATS_ID_RX_SHORT_FRAMES = 0x13,
    AAL_XGE_MAC_STATS_ID_RX_UNDERSIZE_FRAMES = 0x14,
    AAL_XGE_MAC_STATS_ID_RX_GIANT_FRAMES = 0x15,
    AAL_XGE_MAC_STATS_ID_RX_JABBER_FRAMES = 0x16,
    AAL_XGE_MAC_STATS_ID_RX_64B_FRAMES= 0x17,
    AAL_XGE_MAC_STATS_ID_RX_65B_TO_127B_FRAMES = 0x18,
    AAL_XGE_MAC_STATS_ID_RX_128B_TO_255B_FRAMES = 0x19,
    AAL_XGE_MAC_STATS_ID_RX_256B_TO_511B_FRAMES = 0x1a,
    AAL_XGE_MAC_STATS_ID_RX_512B_TO_1023B_FRAMES = 0x1b,
    AAL_XGE_MAC_STATS_ID_RX_1024B_TO_1518B_FRAMES = 0x1c,
    AAL_XGE_MAC_STATS_ID_RX_1519B_TO_MAXB_FRAMES= 0x1d,
    AAL_XGE_MAC_STATS_ID_TX_OCTETS = 0x1e,
    AAL_XGE_MAC_STATS_ID_TX_FRAMES = 0x1f,
    AAL_XGE_MAC_STATS_ID_TX_64B_FRAMES = 0x20,
    AAL_XGE_MAC_STATS_ID_TX_65B_TO_127B_FRAMES = 0x21,
    AAL_XGE_MAC_STATS_ID_TX_128B_TO_255B_FRAMES = 0x22,
    AAL_XGE_MAC_STATS_ID_TX_256B_TO_511B_FRAMES = 0x23,
    AAL_XGE_MAC_STATS_ID_TX_512B_TO_1023B_FRAMES = 0x24,
    AAL_XGE_MAC_STATS_ID_TX_1024B_TO_1518B_FRAMES= 0x25,
    AAL_XGE_MAC_STATS_ID_TX_1519B_FRAMES = 0x26,
    AAL_XGE_MAC_STATS_ID_DROP_RX_CRC_ERR_FRAMES = 0x27,
    AAL_XGE_MAC_STATS_ID_DROP_RX_ALIGN_ERR_FRAMES = 0x28,
    AAL_XGE_MAC_STATS_ID_NUM
} aal_ni_xge_mac_stats_id_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd           : 30 ; 
        ca_uint32_t max_len        :  1 ; 
        ca_uint32_t min_len        :  1 ; 

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t min_len        :  1 ; 
        ca_uint32_t max_len        :  1 ; 
        ca_uint32_t rsvd           : 30 ; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xge_mac_frame_len_config_mask_t;

typedef struct {
    ca_uint16_t min_len;
    ca_uint16_t max_len;
} aal_ni_xge_mac_frame_len_config_t;


typedef struct {
    ca_uint8_t lopwrable  ;
    ca_uint8_t rxlinksts  ;
    ca_uint8_t fault      ;
    ca_uint8_t rxlpiind   ;
    ca_uint8_t txlpiind   ;
    ca_uint8_t rxlpircvd  ;
    ca_uint8_t txlpircvd  ;
    ca_uint8_t rxfault    ;
    ca_uint8_t txfault    ;
    ca_uint8_t devprsnt   ;
} aal_ni_xge_pcs_status_t;  

typedef  union {
    struct {
#ifdef CA_BIG_ENDIAN
    ca_uint32_t rsrvd                :  21;
    ca_uint32_t te_pausereq_sel      :  1 ; 
    ca_uint32_t rx_pausereq_sel      :  1 ; 
    ca_uint32_t tx_remheader_dis     :  1 ;
    ca_uint32_t internal_txstop_sel  :  1 ; 
    ca_uint32_t rx_drp_pau_en        :  1 ; 
    ca_uint32_t qbb_qen              :  1 ; 
    ca_uint32_t adjust_pause_cnt     :  1 ; 
    ca_uint32_t pau_rx_en            :  1 ; 
    ca_uint32_t pau_tx_en            :  1 ; 
    ca_uint32_t qbb_rx_en            :  1 ; 
    ca_uint32_t qbb_tx_en            :  1 ; 

#else               /* CA_LITTLE_ENDIAN */
    ca_uint32_t qbb_tx_en            :  1 ; 
    ca_uint32_t qbb_rx_en            :  1 ; 
    ca_uint32_t pau_tx_en            :  1 ; 
    ca_uint32_t pau_rx_en            :  1 ; 
    ca_uint32_t adjust_pause_cnt     :  1 ; 
    ca_uint32_t qbb_qen              :  1 ; 
    ca_uint32_t rx_drp_pau_en        :  1 ; 
    ca_uint32_t internal_txstop_sel  :  1 ; 
    ca_uint32_t tx_remheader_dis     :  1 ; 
    ca_uint32_t rx_pausereq_sel      :  1 ; 
    ca_uint32_t te_pausereq_sel      :  1 ; 
    ca_uint32_t rsrvd                :  21;

#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xge_flowctrl_mask_t;


typedef struct {

    ca_boolean_t qbb_tx_en          ;
    ca_boolean_t qbb_rx_en          ;
    ca_boolean_t pau_tx_en          ;
    ca_boolean_t pau_rx_en          ;
    ca_uint16_t  adjust_pause_cnt   ;
    ca_uint8_t   qbb_qen            ;
    ca_boolean_t rx_drp_pau_en      ;
    ca_boolean_t internal_txstop_sel;
    ca_boolean_t tx_remheader_dis   ;
    ca_boolean_t rx_pausereq_sel    ;
    ca_boolean_t te_pausereq_sel    ;

} aal_ni_xge_flowctrl_t;  


typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd                 : 26 ; 
        ca_uint32_t tx_localOrdEn        :  1 ; 
        ca_uint32_t tx_remoteOrdEn       :  1 ; 
        ca_uint32_t tx_userOrdEn         :  1 ; 
        ca_uint32_t tx_dataTruncEn       :  1 ; 
        ca_uint32_t tx_pcs6_line_txd_en  :  1 ; 
        ca_uint32_t tx_logicReset        :  1 ; 

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t tx_logicReset        :  1 ; 
        ca_uint32_t tx_pcs6_line_txd_en  :  1 ; 
        ca_uint32_t tx_dataTruncEn       :  1 ; 
        ca_uint32_t tx_userOrdEn         :  1 ; 
        ca_uint32_t tx_remoteOrdEn       :  1 ; 
        ca_uint32_t tx_localOrdEn        :  1 ; 
        ca_uint32_t rsvd                 : 26 ; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xaui_xgrs_tx_cfg_mask_t;

typedef struct {
    ca_boolean_t tx_localOrdEn      ;
    ca_boolean_t tx_remoteOrdEn     ;
    ca_boolean_t tx_userOrdEn       ;
    ca_boolean_t tx_dataTruncEn     ;
    ca_boolean_t tx_pcs6_line_txd_en;
    ca_boolean_t tx_logicReset      ;
} aal_ni_xaui_xgrs_tx_cfg_t;


typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd                 : 21 ; 
        ca_uint32_t rx_dataLosEn         : 1 ;
        ca_uint32_t rx_lockLosEn         : 1 ;
        ca_uint32_t rx_sigLosEn          : 1 ;
        ca_uint32_t rx_phyRLFEn          : 1 ;
        ca_uint32_t rx_losPolarity       : 1 ;
        ca_uint32_t rx_noIpg             : 1 ;
        ca_uint32_t rx_ofsmUNHEn         : 1 ;
        ca_uint32_t rx_pcs6_line_rxd_en  : 1 ;
        ca_uint32_t rx_logicReset        : 1 ;
        ca_uint32_t rx_col_thr           : 1 ;
        ca_uint32_t rx_seq_thr           : 1 ;

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t rx_seq_thr           : 1 ;
        ca_uint32_t rx_col_thr           : 1 ;
        ca_uint32_t rx_logicReset        : 1 ;
        ca_uint32_t rx_pcs6_line_rxd_en  : 1 ;
        ca_uint32_t rx_ofsmUNHEn         : 1 ;
        ca_uint32_t rx_noIpg             : 1 ;
        ca_uint32_t rx_losPolarity       : 1 ;
        ca_uint32_t rx_phyRLFEn          : 1 ;
        ca_uint32_t rx_sigLosEn          : 1 ;
        ca_uint32_t rx_lockLosEn         : 1 ;
        ca_uint32_t rx_dataLosEn         : 1 ;
        ca_uint32_t rsvd                 : 21 ; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xaui_xgrs_rx_cfg_mask_t;

typedef struct {
    ca_uint8_t rx_dataLosEn       ;
    ca_uint8_t rx_lockLosEn       ;
    ca_uint8_t rx_sigLosEn        ;
    ca_uint8_t rx_phyRLFEn        ;
    ca_uint8_t rx_losPolarity     ;
    ca_uint8_t rx_noIpg           ;
    ca_uint8_t rx_ofsmUNHEn       ;
    ca_uint8_t rx_pcs6_line_rxd_en;
    ca_uint8_t rx_logicReset      ;
    ca_uint8_t rx_col_thr         ;
    ca_uint8_t rx_seq_thr         ;
} aal_ni_xaui_xgrs_rx_cfg_t;
typedef struct {
    ca_uint32_t  rx_good_cnt;
    ca_uint32_t  rx_err_cnt;
    ca_uint32_t  tx_good_cnt;
    ca_uint32_t  tx_err_cnt;

} aal_ni_xaui_xgxs_stats_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd             : 21 ; 
        ca_uint32_t xgxs_rx_mode     : 1  ;
        ca_uint32_t xgxs_rx_random_6 : 1  ;
        ca_uint32_t xgxs_rx_err_inj  : 1  ;
        ca_uint32_t xgxs_fpat_sel    : 1  ;
        ca_uint32_t xgxs_fpat_0      : 1  ;
        ca_uint32_t xgxs_fpat_1      : 1  ;
        ca_uint32_t xgxs_fpat_2      : 1  ;
        ca_uint32_t xgxs_fpat_3      : 1  ;
        ca_uint32_t xgxs_rx_phase    : 1  ;
        ca_uint32_t xgxs_rx_fzero    : 1  ;
        ca_uint32_t xgxs_rx_fone     : 1  ;

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t xgxs_rx_fone     : 1  ;
        ca_uint32_t xgxs_rx_fzero    : 1  ;
        ca_uint32_t xgxs_rx_phase    : 1  ;
        ca_uint32_t xgxs_fpat_3      : 1  ;
        ca_uint32_t xgxs_fpat_2      : 1  ;
        ca_uint32_t xgxs_fpat_1      : 1  ;
        ca_uint32_t xgxs_fpat_0      : 1  ;
        ca_uint32_t xgxs_fpat_sel    : 1  ;
        ca_uint32_t xgxs_rx_err_inj  : 1  ;
        ca_uint32_t xgxs_rx_random_6 : 1  ;
        ca_uint32_t xgxs_rx_mode     : 1  ;
        ca_uint32_t rsvd             : 21 ; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xaui_xgxs_rx_cfg_mask_t;

typedef struct {
    ca_uint8_t  xgxs_rx_mode      ; 
    ca_uint8_t  xgxs_rx_random_6  ; 
    ca_uint8_t  xgxs_rx_err_inj   ; 
    ca_uint8_t  xgxs_fpat_sel     ; 
    ca_uint16_t xgxs_fpat_0       ; 
    ca_uint16_t xgxs_fpat_1       ; 
    ca_uint16_t xgxs_fpat_2       ; 
    ca_uint16_t xgxs_fpat_3       ; 
    ca_uint8_t  xgxs_rx_phase     ; 
    ca_uint8_t  xgxs_rx_fzero     ; 
    ca_uint8_t  xgxs_rx_fone      ; 
} aal_ni_xaui_xgxs_rx_cfg_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd                       : 18 ; 
        ca_uint32_t xgxs_tx_mode               : 1  ;
        ca_uint32_t xgxs_lf_enable             : 1  ;
        ca_uint32_t xgxs_ddsa_enable           : 1  ;
        ca_uint32_t xgxs_fpat_sel              : 1  ;
        ca_uint32_t xgxs_fpat_0                : 1  ;
        ca_uint32_t xgxs_fpat_1                : 1  ;
        ca_uint32_t xgxs_fpat_2                : 1  ;
        ca_uint32_t xgxs_fpat_3                : 1  ;
        ca_uint32_t rxaui_cgalign_ena_mode     : 1  ;
        ca_uint32_t rxaui_a_column_ena_mode    : 1  ;
        ca_uint32_t rxaui_a_column_verify_mode : 1  ;
        ca_uint32_t rxaui_lane_swap_01         : 1  ;
        ca_uint32_t rxaui_lane_swap_23         : 1  ;
        ca_uint32_t rxaui_comma_thresh         : 1  ;

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t rxaui_comma_thresh         : 1  ;
        ca_uint32_t rxaui_lane_swap_23         : 1  ;
        ca_uint32_t rxaui_lane_swap_01         : 1  ;
        ca_uint32_t rxaui_a_column_verify_mode : 1  ;
        ca_uint32_t rxaui_a_column_ena_mode    : 1  ;
        ca_uint32_t rxaui_cgalign_ena_mode     : 1  ;
        ca_uint32_t xgxs_fpat_3                : 1  ;
        ca_uint32_t xgxs_fpat_2                : 1  ;
        ca_uint32_t xgxs_fpat_1                : 1  ;
        ca_uint32_t xgxs_fpat_0                : 1  ;
        ca_uint32_t xgxs_fpat_sel              : 1  ;
        ca_uint32_t xgxs_ddsa_enable           : 1  ;
        ca_uint32_t xgxs_lf_enable             : 1  ;
        ca_uint32_t xgxs_tx_mode               : 1  ;
        ca_uint32_t rsvd                       : 18 ; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xaui_xgxs_tx_cfg_mask_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd                 :  28;
        ca_uint32_t rs_rmtfault_en       :  1 ; 
        ca_uint32_t rs_locfault_en       :  1 ; 
        ca_uint32_t tx2rx_lpbk_en        :  1 ; 
        ca_uint32_t rx2tx_lpbk_en        :  1 ; 

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t rx2tx_lpbk_en        :  1 ; 
        ca_uint32_t tx2rx_lpbk_en        :  1 ; 
        ca_uint32_t rs_locfault_en       :  1 ; 
        ca_uint32_t rs_rmtfault_en       :  1 ; 
        ca_uint32_t rsvd                 :  28;
#endif
    } s;

    ca_uint32_t u32;
} aal_ni_xaui_mac_ctrl_mask_t;

typedef struct {
    ca_boolean_t rs_rmtfault_en       ; 
    ca_boolean_t rs_locfault_en       ; 
    ca_boolean_t tx2rx_lpbk_en        ; 
    ca_boolean_t rx2tx_lpbk_en        ; 
    
} aal_ni_xaui_mac_ctrl_t;

typedef struct {
    ca_uint8_t  xgxs_tx_mode      ; 
    ca_uint8_t  xgxs_lf_enable    ; 
    ca_uint8_t  xgxs_ddsa_enable  ; 
    ca_uint8_t  xgxs_fpat_sel     ; 
    ca_uint16_t xgxs_fpat_0       ; 
    ca_uint16_t xgxs_fpat_1       ; 
    ca_uint16_t xgxs_fpat_2       ; 
    ca_uint16_t xgxs_fpat_3       ; 
    ca_uint8_t  rxaui_cgalign_ena_mode ; 
    ca_uint8_t  rxaui_a_column_ena_mode; 
    ca_uint8_t  rxaui_a_column_verify_mode; 
    ca_uint8_t  rxaui_lane_swap_01; 
    ca_uint8_t  rxaui_lane_swap_23; 
    ca_uint8_t  rxaui_comma_thresh; 
} aal_ni_xaui_xgxs_tx_cfg_t;


typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd                 :  22;
        ca_uint32_t insert_cshead        :  1 ; 
        ca_uint32_t reset                :  1 ; 
        ca_uint32_t flow_en              :  1 ; 
        ca_uint32_t mac_en               :  1 ; 
        ca_uint32_t crc_recaculate_en    :  1 ; 
        ca_uint32_t swap_crc8_en         :  1 ; 
        ca_uint32_t swap_datain_en       :  1 ; 
        ca_uint32_t dic_mode             :  1 ; 
        ca_uint32_t link_fault_en        :  1 ; 
        ca_uint32_t insert_nop           :  1 ; 


#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t insert_nop           :  1 ; 
        ca_uint32_t link_fault_en        :  1 ; 
        ca_uint32_t dic_mode             :  1 ; 
        ca_uint32_t swap_datain_en       :  1 ; 
        ca_uint32_t swap_crc8_en         :  1 ; 
        ca_uint32_t crc_recaculate_en    :  1 ; 
        ca_uint32_t mac_en               :  1 ; 
        ca_uint32_t flow_en              :  1 ; 
        ca_uint32_t reset                :  1 ; 
        ca_uint32_t insert_cshead        :  1 ; 
        ca_uint32_t rsvd                 :  22;
#endif
    } s;

    ca_uint32_t u32;
} aal_ni_xaui_mac_tx_cfg_mask_t;

typedef struct {
    ca_boolean_t insert_cshead       ; 
    ca_boolean_t reset               ; 
    ca_boolean_t flow_en             ; 
    ca_boolean_t mac_en              ; 
    ca_boolean_t crc_recaculate_en   ; 
    ca_boolean_t swap_crc8_en        ; 
    ca_boolean_t swap_datain_en      ; 
    ca_uint32_t  dic_mode            ; 
    ca_boolean_t link_fault_en       ; 
    ca_boolean_t insert_nop          ; 
    
} aal_ni_xaui_mac_tx_cfg_t;

typedef struct {
    ca_boolean_t reset             ; 
    ca_boolean_t dxaui_quanta_mode ; 
    ca_boolean_t swap_crc8_en      ; 
    ca_boolean_t swap_datain_en    ; 
    ca_boolean_t mac_en            ; 
    ca_boolean_t pause_en          ; 
    ca_boolean_t check_idle        ; 
    ca_boolean_t check_start       ; 
    ca_boolean_t check_sld         ; 
    ca_boolean_t check_preamble    ; 
    ca_boolean_t check_crc8        ; 
    ca_boolean_t check_terminate   ; 
    ca_boolean_t rx_revcs          ; 
    ca_uint32_t  max_size          ; 
    
} aal_ni_xaui_mac_rx_cfg_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd              : 18; 
        ca_uint32_t reset             : 1; 
        ca_uint32_t dxaui_quanta_mode : 1; 
        ca_uint32_t swap_crc8_en      : 1; 
        ca_uint32_t swap_datain_en    : 1; 
        ca_uint32_t mac_en            : 1; 
        ca_uint32_t pause_en          : 1; 
        ca_uint32_t check_idle        : 1; 
        ca_uint32_t check_start       : 1; 
        ca_uint32_t check_sld         : 1; 
        ca_uint32_t check_preamble    : 1; 
        ca_uint32_t check_crc8        : 1; 
        ca_uint32_t check_terminate   : 1; 
        ca_uint32_t rx_revcs          : 1; 
        ca_uint32_t max_size          : 1; 

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t max_size          : 1; 
        ca_uint32_t rx_revcs          : 1; 
        ca_uint32_t check_terminate   : 1; 
        ca_uint32_t check_crc8        : 1; 
        ca_uint32_t check_preamble    : 1; 
        ca_uint32_t check_sld         : 1; 
        ca_uint32_t check_start       : 1; 
        ca_uint32_t check_idle        : 1; 
        ca_uint32_t pause_en          : 1; 
        ca_uint32_t mac_en            : 1; 
        ca_uint32_t swap_datain_en    : 1; 
        ca_uint32_t swap_crc8_en      : 1; 
        ca_uint32_t dxaui_quanta_mode : 1; 
        ca_uint32_t reset             : 1; 
        ca_uint32_t rsvd              : 18; 
#endif
    } s;

    ca_uint32_t u32;
} aal_ni_xaui_mac_rx_cfg_mask_t;

typedef struct {
  ca_boolean_t clr_rcffoverrun            ; 
  ca_uint32_t  drop_ptp_pkt_type          ; 
  ca_boolean_t qbb_pause_drop_en          ; 
  ca_boolean_t frame_error_drop_en        ; 
  ca_boolean_t frame_fcs_error_drop_en    ; 
  ca_boolean_t frame_bytecnt_drop_en      ; 
  ca_boolean_t frame_pause_drop_en        ; 
  ca_boolean_t frame_runt_drop_en         ; 
  ca_boolean_t frame_oversize_drop_en     ; 
  ca_boolean_t frame_ffoverflow_drop_en   ; 
  ca_boolean_t frame_management_drop_en   ; 
  ca_boolean_t frame_nomanagement_drop_en ; 
  ca_boolean_t qbb_pause_det_en           ; 
  ca_boolean_t bypass                     ; 
  ca_boolean_t ptp_ing_ts_cmp             ; 
  ca_boolean_t rx_pkt_clr                 ; 
  ca_boolean_t ptp_da_en                  ; 
} aal_ni_xaui_mac_rx_ctrl_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd                       : 15; 
        ca_uint32_t clr_rcffoverrun            : 1; 
        ca_uint32_t drop_ptp_pkt_type          : 1; 
        ca_uint32_t qbb_pause_drop_en          : 1; 
        ca_uint32_t frame_error_drop_en        : 1; 
        ca_uint32_t frame_fcs_error_drop_en    : 1; 
        ca_uint32_t frame_bytecnt_drop_en      : 1; 
        ca_uint32_t frame_pause_drop_en        : 1; 
        ca_uint32_t frame_runt_drop_en         : 1; 
        ca_uint32_t frame_oversize_drop_en     : 1; 
        ca_uint32_t frame_ffoverflow_drop_en   : 1; 
        ca_uint32_t frame_management_drop_en   : 1; 
        ca_uint32_t frame_nomanagement_drop_en : 1; 
        ca_uint32_t qbb_pause_det_en           : 1; 
        ca_uint32_t bypass                     : 1; 
        ca_uint32_t ptp_ing_ts_cmp             : 1; 
        ca_uint32_t rx_pkt_clr                 : 1; 
        ca_uint32_t ptp_da_en                  : 1; 
#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t ptp_da_en                  : 1; 
        ca_uint32_t rx_pkt_clr                 : 1; 
        ca_uint32_t ptp_ing_ts_cmp             : 1; 
        ca_uint32_t bypass                     : 1; 
        ca_uint32_t qbb_pause_det_en           : 1; 
        ca_uint32_t frame_nomanagement_drop_en : 1; 
        ca_uint32_t frame_management_drop_en   : 1; 
        ca_uint32_t frame_ffoverflow_drop_en   : 1; 
        ca_uint32_t frame_oversize_drop_en     : 1; 
        ca_uint32_t frame_runt_drop_en         : 1; 
        ca_uint32_t frame_pause_drop_en        : 1; 
        ca_uint32_t frame_bytecnt_drop_en      : 1; 
        ca_uint32_t frame_fcs_error_drop_en    : 1; 
        ca_uint32_t frame_error_drop_en        : 1; 
        ca_uint32_t qbb_pause_drop_en          : 1; 
        ca_uint32_t drop_ptp_pkt_type          : 1; 
        ca_uint32_t clr_rcffoverrun            : 1; 
        ca_uint32_t rsvd                       : 15; 
#endif
    } s;

    ca_uint32_t u32;
} aal_ni_xaui_mac_rx_ctrl_mask_t;
typedef struct {
    ca_boolean_t tx_remheader_dis      ; 
    ca_boolean_t tx_pkt_clr            ; 
    ca_boolean_t tx_pad_en             ; 
    ca_uint32_t  loopback              ; 
    ca_uint8_t   ptp_eg_ts_comp        ; 
    ca_boolean_t external_pausereq_pol ; 
    ca_boolean_t qbb_flow_en           ; 
    ca_uint8_t   qbb_qen               ; 
    ca_boolean_t external_txstop_sel   ; 
    ca_boolean_t internal_txstop_sel   ; 
    ca_boolean_t external_pausereq_sel ; 
    ca_boolean_t rx_pausereq_sel       ; 

} aal_ni_xaui_mac_tx_ctrl_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd                  : 20;
        ca_uint32_t tx_remheader_dis      : 1 ; 
        ca_uint32_t tx_pkt_clr            : 1 ; 
        ca_uint32_t tx_pad_en             : 1 ; 
        ca_uint32_t loopback              : 1 ; 
        ca_uint32_t ptp_eg_ts_comp        : 1 ; 
        ca_uint32_t external_pausereq_pol : 1 ; 
        ca_uint32_t qbb_flow_en           : 1 ; 
        ca_uint32_t qbb_qen               : 1 ; 
        ca_uint32_t external_txstop_sel   : 1 ; 
        ca_uint32_t internal_txstop_sel   : 1 ; 
        ca_uint32_t external_pausereq_sel : 1 ; 
        ca_uint32_t rx_pausereq_sel       : 1 ; 

        
#else               /* CA_LITTLE_ENDIAN */


        ca_uint32_t rx_pausereq_sel       : 1 ;
        ca_uint32_t external_pausereq_sel : 1 ; 
        ca_uint32_t internal_txstop_sel   : 1 ;
        ca_uint32_t external_txstop_sel   : 1 ;
        ca_uint32_t qbb_qen               : 1 ; 
        ca_uint32_t qbb_flow_en           : 1 ; 
        ca_uint32_t external_pausereq_pol : 1 ; 
        ca_uint32_t ptp_eg_ts_comp        : 1 ; 
        ca_uint32_t loopback              : 1 ; 
        ca_uint32_t tx_pad_en             : 1 ; 
        ca_uint32_t tx_pkt_clr            : 1 ; 
        ca_uint32_t tx_remheader_dis      : 1 ; 
        ca_uint32_t rsvd                  : 20;
#endif
    } s;

    ca_uint32_t u32;
} aal_ni_xaui_mac_tx_ctrl_mask_t;
typedef struct {
    ca_boolean_t ctrl_mode_en       ; 
    ca_boolean_t g3_mode_en         ; 
    ca_uint32_t  flush_pkt_en       ; 
    ca_boolean_t flush_pkt_en1      ; 
    ca_boolean_t flush_pkt_sdid     ; 
    ca_boolean_t pkt_tocsport_en    ; 
    ca_boolean_t pkt_toupport_en    ; 
    ca_boolean_t pkt_tocsport_toup  ; 
    ca_boolean_t pkt_toupport_tocs  ; 
    ca_boolean_t flush_touplink_pkt ; 
    ca_boolean_t mask_nop_en        ; 
    ca_boolean_t mask_ddbm_en       ; 
} aal_ni_xaui_mac_ctrl_mode_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd               : 20; 
        ca_uint32_t ctrl_mode_en       : 1; 
        ca_uint32_t g3_mode_en         : 1; 
        ca_uint32_t flush_pkt_en       : 1; 
        ca_uint32_t flush_pkt_en1      : 1; 
        ca_uint32_t flush_pkt_sdid     : 1; 
        ca_uint32_t pkt_tocsport_en    : 1; 
        ca_uint32_t pkt_toupport_en    : 1; 
        ca_uint32_t pkt_tocsport_toup  : 1; 
        ca_uint32_t pkt_toupport_tocs  : 1; 
        ca_uint32_t flush_touplink_pkt : 1; 
        ca_uint32_t mask_nop_en        : 1; 
        ca_uint32_t mask_ddbm_en       : 1;

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t mask_ddbm_en       : 1;
        ca_uint32_t mask_nop_en        : 1; 
        ca_uint32_t flush_touplink_pkt : 1; 
        ca_uint32_t pkt_toupport_tocs  : 1; 
        ca_uint32_t pkt_tocsport_toup  : 1; 
        ca_uint32_t pkt_toupport_en    : 1; 
        ca_uint32_t pkt_tocsport_en    : 1; 
        ca_uint32_t flush_pkt_sdid     : 1; 
        ca_uint32_t flush_pkt_en1      : 1; 
        ca_uint32_t flush_pkt_en       : 1;  
        ca_uint32_t g3_mode_en         : 1; 
        ca_uint32_t ctrl_mode_en       : 1; 
        ca_uint32_t rsvd               : 20; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xaui_mac_ctrl_mode_mask_t;

typedef struct {
    ca_uint16_t xoff_threshold; 
    ca_uint16_t xon_threshold; 
    ca_uint8_t  adjust_qbb_cnt; 
    ca_uint8_t  adjust_1g_qbb_cnt;   
    ca_uint16_t qbb_pause_quanta0; 
    ca_uint16_t qbb_pause_quanta1; 
    ca_uint16_t qbb_pause_quanta2; 
    ca_uint16_t qbb_pause_quanta3; 
    ca_uint16_t qbb_pause_quanta4; 
    ca_uint16_t qbb_pause_quanta5; 
    ca_uint16_t qbb_pause_quanta6; 
    ca_uint16_t qbb_pause_quanta7; 
} aal_ni_xaui_mac_ctrl_flowctrl_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd               : 20; 
        ca_uint32_t xoff_threshold     : 1; 
        ca_uint32_t xon_threshold      : 1; 
        ca_uint32_t adjust_qbb_cnt     : 1; 
        ca_uint32_t adjust_1g_qbb_cnt  : 1; 
        ca_uint32_t qbb_pause_quanta0  : 1; 
        ca_uint32_t qbb_pause_quanta1  : 1; 
        ca_uint32_t qbb_pause_quanta2  : 1; 
        ca_uint32_t qbb_pause_quanta3  : 1; 
        ca_uint32_t qbb_pause_quanta4  : 1; 
        ca_uint32_t qbb_pause_quanta5  : 1; 
        ca_uint32_t qbb_pause_quanta6  : 1; 
        ca_uint32_t qbb_pause_quanta7  : 1; 
#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t qbb_pause_quanta7  : 1; 
        ca_uint32_t qbb_pause_quanta6  : 1; 
        ca_uint32_t qbb_pause_quanta5  : 1; 
        ca_uint32_t qbb_pause_quanta4  : 1; 
        ca_uint32_t qbb_pause_quanta3  : 1; 
        ca_uint32_t qbb_pause_quanta2  : 1; 
        ca_uint32_t qbb_pause_quanta1  : 1; 
        ca_uint32_t qbb_pause_quanta0  : 1; 
        ca_uint32_t adjust_1g_qbb_cnt  : 1; 
        ca_uint32_t adjust_qbb_cnt     : 1; 
        ca_uint32_t xon_threshold      : 1; 
        ca_uint32_t xoff_threshold     : 1; 
        ca_uint32_t rsvd               : 20; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xaui_mac_ctrl_flowctrl_mask_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd               : 30; 
        ca_uint32_t ram_clr_en         :  1 ; 
        ca_uint32_t ram_sw_initial     :  1 ;

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t ram_sw_initial     :  1 ;
        ca_uint32_t ram_clr_en         :  1 ; 
        ca_uint32_t rsvd               : 30; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xaui_mac_ctrl_mib_ctrl_mask_t;


typedef struct {
    ca_boolean_t ram_clr_en ; /* enable to clear the MIB counter after read */
    ca_boolean_t ram_sw_initial; /* xaui mib sram initial , please write 1 , 
                                    then 0 when want to reset MIB SRAM
                                  */
} aal_ni_xaui_mac_ctrl_mib_ctrl_t;


typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t   rsvd      : 22; 
        ca_uint32_t   da        : 1;
        ca_uint32_t   ethtype0  : 1;
        ca_uint32_t   ethtype1  : 1;
        ca_uint32_t   ethtype2  : 1;
        ca_uint32_t   ethtype3  : 1;
        ca_uint32_t   ethtype4  : 1;
        ca_uint32_t   ethtype5  : 1;
        ca_uint32_t   ethtype6  : 1;
        ca_uint32_t   ethtype7  : 1;
        ca_uint32_t   detect_en_bmp : 1;

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t   detect_en_bmp : 1;
        ca_uint32_t   ethtype7  : 1;
        ca_uint32_t   ethtype6  : 1;
        ca_uint32_t   ethtype5  : 1;
        ca_uint32_t   ethtype4  : 1;
        ca_uint32_t   ethtype3  : 1;
        ca_uint32_t   ethtype2  : 1;
        ca_uint32_t   ethtype1  : 1;
        ca_uint32_t   ethtype0  : 1;
        ca_uint32_t   da        : 1;
        ca_uint32_t   rsvd      : 22; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xaui_mac_ctrl_mgm_cfg_mask_t;



typedef struct {
    ca_mac_addr_t  da;
    ca_uint16_t    ethtype0;
    ca_uint16_t    ethtype1;
    ca_uint16_t    ethtype2;
    ca_uint16_t    ethtype3;
    ca_uint16_t    ethtype4;
    ca_uint16_t    ethtype5;
    ca_uint16_t    ethtype6;
    ca_uint16_t    ethtype7;
    ca_uint8_t     detect_en_bmp;
} aal_ni_xaui_mac_ctrl_mgm_cfg_t;

typedef struct {
    ca_uint32_t  byte_lo; /* 00 BYTECNT_LO frame byte count, low-32bit */
    ca_uint32_t  byte_hi; /* 01 BYTECNT_HI frame byte count, high-32bit*/
    ca_uint32_t  crc_err_cnt; /* 02 CRCERRCNT frame with FCS error (only active when receive port)*/
    ca_uint32_t  uc_cnt; /* 03 UCFRAMECNT unicast frame count*/
    ca_uint32_t  mc_cnt; /* 04 MCFRAMECNT multicast frame*/
    ca_uint32_t  bc_cnt; /* 05 BCFRAMECNT broadcast frame*/
    ca_uint32_t  rsvd6;
    ca_uint32_t  pause_cnt; /* 07 PAUSECNT Pause frame*/
    ca_uint32_t  rsvd8;
    ca_uint32_t  undersize_cnt; /* 09 UNDERSIZECNT frame with size less than 64 bytes without CRC error*/
    ca_uint32_t  oversize_cnt; /* 0A OVERSIZECNT frame with size more than configured value,max is 2047 bytes without CRC error */
    ca_uint32_t  rmon64_cnt; /* 0B RMON64CNT 64-byte frame */
    ca_uint32_t  rmon127_cnt; /* 0C RMON65_127CNT frame with size between 65 and 127 bytes */
    ca_uint32_t  rmon128_255_cnt; /* 0D RMON128_255CNT frame with size between 128 and 255 bytes */
    ca_uint32_t  rmon256_511_cnt; /* 0E RMON256_511CNT frame with size between 256 and 511 bytes */
    ca_uint32_t  rmon512_1023_cnt; /* 0F RMON512_1023CNT frame with size between 512 and 1023 bytes */
    ca_uint32_t  rmon1024_1518_cnt; /* 10 RMON1024_1518CNT frame with size between 1024 and 1518 bytes, */
    ca_uint32_t  rmon1519_max_cnt; /* 11 RMON1519_MAXCNT frame with size between 1519 and configured maximum value */

} aal_ni_xaui_mac_ctrl_rx_mib_t;



typedef struct {
    ca_uint32_t  byte_lo; /* 00 BYTECNT_LO frame byte count, low-32bit */
    ca_uint32_t  byte_hi; /* 01 BYTECNT_HI frame byte count, high-32bit*/
    ca_uint32_t  crc_err_cnt; /* 02 CRCERRCNT frame with FCS error (only active when receive port)*/
    ca_uint32_t  uc_cnt; /* 03 UCFRAMECNT unicast frame count*/
    ca_uint32_t  mc_cnt; /* 04 MCFRAMECNT multicast frame*/
    ca_uint32_t  bc_cnt; /* 05 BCFRAMECNT broadcast frame*/
    ca_uint32_t  rsvd6;
    ca_uint32_t  pause_cnt; /* 07 PAUSECNT Pause frame*/
    ca_uint32_t  rsvd8;
    ca_uint32_t  undersize_cnt; /* 09 UNDERSIZECNT frame with size less than 64 bytes without CRC error*/
    ca_uint32_t  oversize_cnt; /* 0A OVERSIZECNT frame with size more than configured value,max is 2047 bytes without CRC error */
    ca_uint32_t  rmon64_cnt; /* 0B RMON64CNT 64-byte frame */
    ca_uint32_t  rmon127_cnt; /* 0C RMON65_127CNT frame with size between 65 and 127 bytes */
    ca_uint32_t  rmon128_255_cnt; /* 0D RMON128_255CNT frame with size between 128 and 255 bytes */
    ca_uint32_t  rmon256_511_cnt; /* 0E RMON256_511CNT frame with size between 256 and 511 bytes */
    ca_uint32_t  rmon512_1023_cnt; /* 0F RMON512_1023CNT frame with size between 512 and 1023 bytes */
    ca_uint32_t  rmon1024_1518_cnt; /* 10 RMON1024_1518CNT frame with size between 1024 and 1518 bytes, */
    ca_uint32_t  rmon1519_max_cnt; /* 11 RMON1519_MAXCNT frame with size between 1519 and configured maximum value */

} aal_ni_xaui_mac_ctrl_tx_mib_t;

typedef enum {
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_BYTE_LO = 0,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_BYTE_HI = 1,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_CRC_ERR_CNT = 2,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_UC_CNT = 3,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_MC_CNT = 4,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_BC_CNT = 5,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_RSVD6 = 6,
    /* rsvd 6 */
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_PAUSE_CNT = 7,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_RSVD8 = 8,
    /* rsvd 8 */
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_UNDERSIZE_CNT = 9,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_OVERSIZE_CNT = 0xa,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_RMON64_CNT = 0xb,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_RMON127_CNT = 0xc,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_RMON128_255_CNT = 0xd,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_RMON256_511_CNT = 0xe,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_RMON512_1023_CNT = 0xf,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_RMON1024_1518_CNT = 0x10,
    AAL_NI_XAUI_MAC_CTRL_MIB_ID_RMON1519_MAX_CNT = 0x11,
    
} aal_ni_xaui_mac_ctrl_mib_id_t;




typedef struct {
    ca_uint32_t    rx_ptp_tm ; 
    ca_uint32_t    rx_ptp_tm_s_lo ; 
    ca_uint32_t    rx_ptp_tm_s_hi ; 
    ca_uint8_t     rx_ptp_type;
    ca_uint16_t    rx_ptp_sqid;
    ca_uint32_t    tx_ptp_tm ; 
    ca_uint32_t    tx_ptp_tm_s_lo ; 
    ca_uint32_t    tx_ptp_tm_s_hi ; 
    ca_uint8_t     tx_ptp_type;
    ca_uint16_t    tx_ptp_sqid;

} aal_ni_xaui_mac_ctrl_ptp_status_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t   rsvd  : 28; 
        ca_uint32_t   vlan0 : 1;
        ca_uint32_t   vlan1 : 1;
        ca_uint32_t   vlan2 : 1;
        ca_uint32_t   vlan3 : 1;

#else               /* CA_LITTLE_ENDIAN */
        ca_uint32_t   vlan3 : 1;
        ca_uint32_t   vlan2 : 1;
        ca_uint32_t   vlan1 : 1;
        ca_uint32_t   vlan0 : 1;
        ca_uint32_t   rsvd  : 28; 
#endif
    } s;
    ca_uint32_t u32;
} aal_ni_xaui_mac_ctrl_ptp_cfg_mask_t;

typedef struct {
    ca_uint16_t vlan0;
    ca_uint16_t vlan1;
    ca_uint16_t vlan2;
    ca_uint16_t vlan3;
} aal_ni_xaui_mac_ctrl_ptp_cfg_t;
typedef struct {
    ca_uint16_t trunc_pkt_tx;
    ca_uint8_t  oidle_pkt_tx;
    ca_uint8_t  rx_unknownord_l1;
    ca_uint8_t  rx_unknownord_l2;
    ca_uint8_t  rx_unknownord_l3;
} aal_ni_xaui_xgrs_stats_t;


typedef struct {
    ca_boolean_t Tx_SyncDet0             ;
    ca_boolean_t Tx_SyncDet1             ;
    ca_boolean_t Tx_SyncDet2             ;
    ca_boolean_t Tx_SyncDet3             ;
    ca_boolean_t Tx_xgxs_align_det       ;
    ca_boolean_t Tx_xgxs_cpat_lock       ;

} aal_ni_xaui_xgxs_intr_cfg_t;


typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
    ca_uint32_t rsvd              : 26;
    ca_uint32_t Tx_xgxs_cpat_lock : 1;
    ca_uint32_t Tx_xgxs_align_det : 1;
    ca_uint32_t Tx_SyncDet3       : 1;
    ca_uint32_t Tx_SyncDet2       : 1;
    ca_uint32_t Tx_SyncDet1       : 1;
    ca_uint32_t Tx_SyncDet0       : 1;

#else               /* CA_LITTLE_ENDIAN */

    ca_uint32_t Tx_SyncDet0       : 1;
    ca_uint32_t Tx_SyncDet1       : 1;
    ca_uint32_t Tx_SyncDet2       : 1;
    ca_uint32_t Tx_SyncDet3       : 1;
    ca_uint32_t Tx_xgxs_align_det : 1;
    ca_uint32_t Tx_xgxs_cpat_lock : 1;
    ca_uint32_t rsvd              : 26;
#endif
    } s;

    ca_uint32_t u32;
} aal_ni_xaui_xgxs_intr_cfg_mask_t;

typedef struct {
    ca_boolean_t Tx_ErrorPktSat          ;
    ca_boolean_t Tx_GoodPktSat           ;
    ca_boolean_t Tx_InvCodeSat           ;
    ca_boolean_t Tx_xgxs_comma_found_0   ;
    ca_boolean_t Tx_PatErrSat            ;
    ca_boolean_t Tx_xgxs_comma_found_1   ;
    ca_boolean_t Tx_SyncDet0             ;
    ca_boolean_t Tx_SyncDet1             ;
    ca_boolean_t Tx_SyncDet2             ;
    ca_boolean_t Tx_SyncDet3             ;
    ca_boolean_t Tx_xgxs_align_det       ;
    ca_boolean_t Tx_xgxs_cpat_lock       ;
    ca_boolean_t Tx_xgxs_A_column_found_0;
    ca_boolean_t Tx_xgxs_A_column_found_1;
    ca_boolean_t Rx_ErrorPktSat          ;
    ca_boolean_t Rx_GoodPktSat           ;

    
} aal_ni_xaui_xgxs_intr_status_t;

typedef union {
    struct {
#ifdef CA_BIG_ENDIAN
        ca_uint32_t rsvd:24;
        ca_uint32_t p7_ptp_tx_en:1;
        ca_uint32_t p6_ptp_tx_en:1;
        ca_uint32_t p5_ptp_tx_en:1;
        ca_uint32_t p4_ptp_tx_en:1;
        ca_uint32_t p3_ptp_tx_en:1;
        ca_uint32_t p2_ptp_tx_en:1;
        ca_uint32_t p1_ptp_tx_en:1;
        ca_uint32_t p0_ptp_tx_en:1;

#else               /* CA_LITTLE_ENDIAN */

        ca_uint32_t p0_ptp_tx_en:1;
        ca_uint32_t p1_ptp_tx_en:1;
        ca_uint32_t p2_ptp_tx_en:1;
        ca_uint32_t p3_ptp_tx_en:1;
        ca_uint32_t p4_ptp_tx_en:1;
        ca_uint32_t p5_ptp_tx_en:1;
        ca_uint32_t p6_ptp_tx_en:1;
        ca_uint32_t p7_ptp_tx_en:1;
        ca_uint32_t rsvd:24;
#endif
    } s;
    ca_uint32_t u32;

} aal_ni_glb_ptp_tx_cfg_mask_t;

typedef struct {

     ca_uint32_t p0_ptp_tx_en          ; /* bits 0:0 */
     ca_uint32_t p1_ptp_tx_en          ; /* bits 1:1 */
     ca_uint32_t p2_ptp_tx_en          ; /* bits 2:2 */
     ca_uint32_t p3_ptp_tx_en          ; /* bits 3:3 */
     ca_uint32_t p4_ptp_tx_en          ; /* bits 4:4 */
     ca_uint32_t p5_ptp_tx_en          ; /* bits 5:5 */
     ca_uint32_t p6_ptp_tx_en          ; /* bits 6:6 */
     ca_uint32_t p7_ptp_tx_en          ; /* bits 7:7 */
    
}aal_ni_glb_ptp_tx_cfg_t;

typedef union{
    struct{
#ifdef CA_BIG_ENDIAN
    ca_uint32_t pps_out_falledge_intie           :  1 ; 
    ca_uint32_t pps_event_intie                  :  1 ; 
    ca_uint32_t timer_rollover_intie             :  1 ; 
    ca_uint32_t rsrvd1                           :  5 ;
    ca_uint32_t port7_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port6_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port5_tx_ptpfifo_underflow_intie :  1 ;
    ca_uint32_t port4_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port3_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port2_tx_ptpfifo_underflow_intie :  1 ;
    ca_uint32_t port1_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port0_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port7_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port6_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port5_tx_ptpfifo_overflow_intie  :  1 ;
    ca_uint32_t port4_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port3_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port2_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port1_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port0_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port7_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port6_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port5_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port4_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port3_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port2_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port1_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port0_tx_timestamp_rdy_intie     :  1 ; 

#else     
    ca_uint32_t port0_tx_timestamp_rdy_intie     :  1 ;
    ca_uint32_t port1_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port2_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port3_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port4_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port5_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port6_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port7_tx_timestamp_rdy_intie     :  1 ; 
    ca_uint32_t port0_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port1_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port2_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port3_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port4_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port5_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port6_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port7_tx_ptpfifo_overflow_intie  :  1 ; 
    ca_uint32_t port0_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port1_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port2_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port3_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port4_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port5_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port6_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t port7_tx_ptpfifo_underflow_intie :  1 ; 
    ca_uint32_t rsrvd1                           :  5 ;
    ca_uint32_t timer_rollover_intie             :  1 ; 
    ca_uint32_t pps_event_intie                  :  1 ; 
    ca_uint32_t pps_out_falledge_intie           :  1 ; 
#endif
    }s;
    ca_uint32_t u32;  
}aal_ni_glb_ptp_intr_enable_cfg_mask_t;

typedef struct{
    ca_uint32_t port0_tx_timestamp_rdy_intie      ; 
    ca_uint32_t port1_tx_timestamp_rdy_intie      ; 
    ca_uint32_t port2_tx_timestamp_rdy_intie      ; 
    ca_uint32_t port3_tx_timestamp_rdy_intie      ; 
    ca_uint32_t port4_tx_timestamp_rdy_intie      ; 
    ca_uint32_t port5_tx_timestamp_rdy_intie      ; 
    ca_uint32_t port6_tx_timestamp_rdy_intie      ; 
    ca_uint32_t port7_tx_timestamp_rdy_intie      ; 
    ca_uint32_t port0_tx_ptpfifo_overflow_intie   ; 
    ca_uint32_t port1_tx_ptpfifo_overflow_intie   ; 
    ca_uint32_t port2_tx_ptpfifo_overflow_intie   ; 
    ca_uint32_t port3_tx_ptpfifo_overflow_intie   ; 
    ca_uint32_t port4_tx_ptpfifo_overflow_intie   ; 
    ca_uint32_t port5_tx_ptpfifo_overflow_intie   ; 
    ca_uint32_t port6_tx_ptpfifo_overflow_intie   ;
    ca_uint32_t port7_tx_ptpfifo_overflow_intie   ; 
    ca_uint32_t port0_tx_ptpfifo_underflow_intie  ; 
    ca_uint32_t port1_tx_ptpfifo_underflow_intie  ; 
    ca_uint32_t port2_tx_ptpfifo_underflow_intie  ; 
    ca_uint32_t port3_tx_ptpfifo_underflow_intie  ; 
    ca_uint32_t port4_tx_ptpfifo_underflow_intie  ; 
    ca_uint32_t port5_tx_ptpfifo_underflow_intie  ; 
    ca_uint32_t port6_tx_ptpfifo_underflow_intie  ; 
    ca_uint32_t port7_tx_ptpfifo_underflow_intie  ; 
    ca_uint32_t timer_rollover_intie              ; 
    ca_uint32_t pps_event_intie                   ; 
    ca_uint32_t pps_out_falledge_intie            ; 
}aal_ni_glb_ptp_intr_enable_cfg_t;

typedef union{
  struct {
    ca_uint32_t gpio1en : 32   ; 
  } s ;
  ca_uint32_t u32;  
}aal_global_gpio_mux_cfg_mask_t;


typedef struct{
    ca_uint32_t gpio1en  ; 
}aal_global_gpio_mux_cfg_t;

ca_status_t aal_ni_init(/*AUTO-CLI gen ignore */
                   CA_IN ca_device_id_t device_id);

ca_status_t aal_ni_pkt_len_set(
                    CA_IN ca_device_id_t device_id,
                   CA_IN aal_ni_pkt_len_config_mask_t mask,
                   CA_IN aal_ni_pkt_len_config_t * config);

ca_status_t aal_ni_pkt_len_get(CA_IN ca_device_id_t device_id,
                   CA_OUT aal_ni_pkt_len_config_t * config);

ca_status_t aal_ni_port_rx_ctrl_set(
                    CA_IN ca_device_id_t device_id,
                    CA_IN ca_uint32_t port_id,
                    CA_IN aal_ni_port_rx_ctrl_mask_t mask,
                    CA_IN aal_ni_port_rx_ctrl_t * config);

ca_status_t aal_ni_port_rx_ctrl_get(
                    CA_IN ca_device_id_t device_id,
                    CA_IN ca_uint32_t port_id,
                    CA_OUT aal_ni_port_rx_ctrl_t * config);

ca_status_t aal_ni_eth_if_reset_ctrl_set(
                    CA_IN ca_device_id_t device_id,
                    CA_IN aal_ni_eth_if_reset_ctrl_mask_t mask,
                    CA_IN aal_ni_eth_if_reset_ctrl_t *config);

ca_status_t aal_ni_eth_if_reset_ctrl_get(
                    CA_IN ca_device_id_t device_id,
                    CA_OUT aal_ni_eth_if_reset_ctrl_t *config);

ca_status_t aal_ni_eth_if_set(
                    CA_IN ca_device_id_t device_id,
                    CA_IN ca_uint32_t port_id,
                    CA_IN aal_ni_eth_if_config_mask_t mask,
                    CA_IN aal_ni_eth_if_config_t * config);

ca_status_t aal_ni_eth_if_get(
                    CA_IN ca_device_id_t device_id,
                    CA_IN ca_uint32_t port_id,
                    CA_OUT aal_ni_eth_if_config_t * config);

ca_status_t aal_ni_eth_port_mac_set(
                    CA_IN ca_device_id_t device_id,
                    CA_IN ca_uint32_t port_id,
                    CA_IN aal_ni_eth_mac_config_mask_t mask,
                    CA_IN aal_ni_eth_mac_config_t * config);

ca_status_t aal_ni_eth_port_mac_get(CA_IN ca_device_id_t device_id,
                    CA_IN ca_uint32_t port_id,
                    CA_OUT aal_ni_eth_mac_config_t * config);

ca_status_t aal_ni_eth_pfc_quanta_get(
                    CA_IN ca_device_id_t    device_id,
                    CA_IN ca_uint32_t    port_id,
                    CA_IN ca_uint8_t     cos,
                    CA_OUT ca_uint16_t   *quanta);

ca_status_t aal_ni_eth_pfc_quanta_set(
                    CA_IN ca_device_id_t    device_id,
                    CA_IN ca_uint32_t    port_id,
                    CA_IN ca_uint8_t     cos,
                    CA_IN ca_uint16_t    quanta);

ca_status_t aal_ni_eth_port_mib_get(CA_IN ca_device_id_t device_id,
                    CA_IN ca_uint32_t port_id,
                    CA_IN ca_boolean_t read_clear,
                    CA_OUT aal_ni_mib_stats_t * stats);

ca_status_t aal_ni_mc_lkup_set(
                    CA_IN ca_device_id_t device_id,
                    CA_IN ca_uint32_t mc_group,
                    CA_IN ca_uint64_t port_bmp);

ca_status_t aal_ni_mc_lkup_get(
                    CA_IN  ca_device_id_t   device_id,
                    CA_IN  ca_uint32_t   mc_group,
                    CA_OUT ca_uint64_t * port_bmp);

ca_status_t aal_ni_port_tx_fifo_threshold_set(
                    CA_IN  ca_device_id_t                         device_id,
                    CA_IN  ca_uint32_t                         port_id,
                    CA_IN  aal_ni_port_txfifo_threshold_mask_t mask,
                    CA_IN  aal_ni_port_txfifo_threshold_t     *config );

ca_status_t aal_ni_port_tx_fifo_threshold_get(
                    CA_IN  ca_device_id_t                      device_id,
                    CA_IN  ca_uint32_t                      port_id,
                    CA_OUT aal_ni_port_txfifo_threshold_t  *config );

ca_status_t aal_ni_eth_port_status_get(
                    CA_IN ca_device_id_t device_id,
                    CA_IN  ca_uint32_t port_id,
                    CA_OUT aal_eth_port_status_t * status);

ca_status_t aal_ni_port_intr_enable_set(
                    CA_IN ca_device_id_t device_id,
                    CA_IN ca_uint32_t port_id,
                    CA_IN aal_ni_port_intr_mask_t mask,
                    CA_IN aal_ni_port_intr_t * config);

ca_status_t aal_ni_port_intr_enable_get(CA_IN ca_device_id_t device_id,
                    CA_IN  ca_uint32_t port_id,
                    CA_OUT aal_ni_port_intr_t * config);

ca_status_t aal_ni_port_intr_status_clear(/*AUTO-CLI gen ignore */
                    CA_IN ca_device_id_t device_id,
                    CA_IN ca_uint32_t port_id,
                    CA_IN aal_ni_port_intr_t * status);

ca_status_t aal_ni_port_intr_status_get(
                    CA_IN  ca_device_id_t device_id,
                    CA_IN  ca_uint32_t port_id,
                    CA_OUT aal_ni_port_intr_t * status);

ca_status_t aal_ni_glb_intr_enable_set(
                    CA_IN ca_device_id_t device_id,
                    CA_IN aal_ni_glb_intr_mask_t mask,
                    CA_IN aal_ni_glb_intr_t * config);

ca_status_t aal_ni_glb_intr_enable_get(
                    CA_IN ca_device_id_t device_id,
                    CA_OUT aal_ni_glb_intr_t * config);

ca_status_t aal_ni_glb_intr_status_clear(/*AUTO-CLI gen ignore */
                    CA_IN ca_device_id_t device_id,
                    CA_IN aal_ni_glb_intr_t * status);

ca_status_t aal_ni_glb_intr_status_get(
                    CA_IN ca_device_id_t device_id,
                    CA_OUT aal_ni_glb_intr_t * status);

ca_status_t aal_ni_xge_misc_config_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN ca_uint32_t                 port_id,
    CA_IN aal_ni_xge_misc_cfg_mask_t  mask,
    CA_IN aal_ni_xge_misc_cfg_t      *config);

ca_status_t aal_ni_xge_misc_config_get(
    CA_IN  ca_device_id_t                 device_id,
    CA_IN  ca_uint32_t                 port_id,
    CA_OUT aal_ni_xge_misc_cfg_t      *config);


ca_status_t aal_ni_xge_cmn_config_set(
    CA_IN ca_device_id_t                 device_id,
    CA_IN ca_uint32_t                 port_id,
    CA_IN aal_ni_xge_cmn_cfg_mask_t   mask,
    CA_IN aal_ni_xge_cmn_cfg_t       *config);

ca_status_t aal_ni_xge_cmn_config_get(
    CA_IN  ca_device_id_t            device_id,
    CA_IN  ca_uint32_t             port_id,
    CA_OUT aal_ni_xge_cmn_cfg_t  *config);


ca_status_t aal_ni_xge_rx_config_set(
    CA_IN ca_device_id_t                device_id,
    CA_IN ca_uint32_t                port_id,
    CA_IN aal_ni_xge_cfg_rx_mask_t   mask,
    CA_IN aal_ni_xge_cfg_rx_t       *config);

ca_status_t aal_ni_xge_rx_config_get(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  ca_uint32_t                port_id,
    CA_OUT aal_ni_xge_cfg_rx_t       *config);


ca_status_t aal_ni_xge_tx_config_set(
    CA_IN ca_device_id_t                device_id,
    CA_IN ca_uint32_t                port_id,
    CA_IN aal_ni_xge_cfg_tx_mask_t   mask,
    CA_IN aal_ni_xge_cfg_tx_t       *config);

ca_status_t aal_ni_xge_tx_config_get(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  ca_uint32_t                port_id,
    CA_OUT aal_ni_xge_cfg_tx_t       *config);

ca_status_t aal_ni_xge_mac_addr_set(
    CA_IN ca_device_id_t    device_id,
    CA_IN ca_uint32_t    port_id,
    CA_IN ca_mac_addr_t  mac);


ca_status_t aal_ni_xge_mac_addr_get(
    CA_IN  ca_device_id_t    device_id,
    CA_IN  ca_uint32_t    port_id,
    CA_OUT ca_mac_addr_t  mac);

ca_status_t aal_ni_xge_mac_vlan_set(
    CA_IN  ca_device_id_t    device_id,
    CA_IN  ca_uint32_t    port_id,
    CA_IN  ca_uint32_t    idx,
    CA_IN  ca_uint16_t    vlan);

ca_status_t aal_ni_xge_mac_vlan_get(
    CA_IN  ca_device_id_t    device_id,
    CA_IN  ca_uint32_t    port_id,
    CA_IN  ca_uint32_t    idx,
    CA_OUT ca_uint16_t   *vlan);

ca_status_t aal_ni_xge_mac_frame_len_set(
    CA_IN  ca_device_id_t    device_id,
    CA_IN  ca_uint32_t    port_id,
    CA_IN  aal_ni_xge_mac_frame_len_config_mask_t    mask,
    CA_IN  aal_ni_xge_mac_frame_len_config_t        *config);

ca_status_t aal_ni_xge_mac_frame_len_get(
    CA_IN  ca_device_id_t    device_id,
    CA_IN  ca_uint32_t    port_id,
    CA_OUT aal_ni_xge_mac_frame_len_config_t  *config);

ca_status_t aal_ni_xge_pause_mac_addr_set(
    CA_IN ca_device_id_t    device_id,
    CA_IN ca_uint32_t    port_id,
    CA_IN ca_mac_addr_t  mac);

ca_status_t aal_ni_xge_pause_mac_addr_get(
    CA_IN  ca_device_id_t    device_id,
    CA_IN  ca_uint32_t    port_id,
    CA_OUT ca_mac_addr_t  mac);

ca_status_t aal_ni_xge_pause_quanta_set(
    CA_IN ca_device_id_t    device_id,
    CA_IN ca_uint32_t    port_id,
    CA_IN ca_uint16_t    quanta);

ca_status_t aal_ni_xge_pause_quanta_get(
    CA_IN  ca_device_id_t    device_id,
    CA_IN  ca_uint32_t    port_id,
    CA_OUT ca_uint16_t   *quanta);

ca_status_t aal_ni_xge_mac_pause_tx_cmd_set(/*AUTO-CLI gen ignore */
    CA_IN  ca_device_id_t     device_id,
    CA_IN  ca_uint32_t     port_id,
    CA_IN  ca_boolean_t    xon_xoff,
    CA_IN  ca_boolean_t    x_generate);

ca_status_t aal_ni_xge_mac_pause_tx_cmd_get(
    CA_IN  ca_device_id_t     device_id,
    CA_IN  ca_uint32_t     port_id,
    CA_OUT ca_boolean_t   *xon_xoff,
    CA_OUT ca_boolean_t   *x_generate);


ca_status_t aal_ni_xge_flowctrl_set(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  ca_uint32_t                port_id,
    CA_IN  aal_ni_xge_flowctrl_mask_t mask,
    CA_IN  aal_ni_xge_flowctrl_t     *config);

ca_status_t aal_ni_xge_flowctrl_get(
    CA_IN  ca_device_id_t             device_id,
    CA_IN  ca_uint32_t             port_id,
    CA_OUT aal_ni_xge_flowctrl_t  *config);

ca_status_t aal_ni_xge_mac_stats_get(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  ca_uint32_t                port_id,
    CA_IN  aal_ni_xge_mac_stats_id_t  stats_id,
    CA_OUT ca_uint64_t               *counter);

ca_status_t aal_ni_xge_mac_mib_get(
    CA_IN  ca_device_id_t             device_id,
    CA_IN  ca_uint32_t             port_id,
    CA_OUT aal_ni_mib_stats_t     *stats);

ca_status_t aal_ni_xge_pcs_status_get(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              port_id,
    CA_OUT aal_ni_xge_pcs_status_t *sts);

ca_status_t aal_ni_xge_mac_intr_en_set(
    CA_IN   ca_device_id_t      device_id,
    CA_IN   ca_uint32_t      port_id,
    CA_IN   ca_uint32_t      en_bmp);

ca_status_t aal_ni_xge_mac_intr_en_get(
    CA_IN   ca_device_id_t      device_id,
    CA_IN   ca_uint32_t      port_id,
    CA_OUT  ca_uint32_t     *en_bmp);

ca_status_t aal_ni_xge_mac_intr_status_get(
    CA_IN   ca_device_id_t      device_id,
    CA_IN   ca_uint32_t      port_id,
    CA_OUT  ca_uint32_t     *status_bmp);

ca_status_t aal_ni_xaui_mac_ctrl_set(
    CA_IN  ca_device_id_t                    device_id,
    CA_IN  aal_ni_xaui_mac_ctrl_mask_t    mask,
    CA_IN  aal_ni_xaui_mac_ctrl_t        *config);

ca_status_t aal_ni_xaui_mac_ctrl_get(
    CA_IN  ca_device_id_t                   device_id,
    CA_OUT aal_ni_xaui_mac_ctrl_t       *config);
    
ca_status_t aal_ni_xaui_mac_tx_config_set(
    CA_IN  ca_device_id_t                    device_id,
    CA_IN  aal_ni_xaui_mac_tx_cfg_mask_t  mask,
    CA_IN  aal_ni_xaui_mac_tx_cfg_t      *config);

ca_status_t aal_ni_xaui_mac_tx_config_get(
    CA_IN  ca_device_id_t                   device_id,
    CA_OUT aal_ni_xaui_mac_tx_cfg_t     *config);

ca_status_t aal_ni_xaui_mac_rx_config_set(
    CA_IN  ca_device_id_t                    device_id,
    CA_IN  aal_ni_xaui_mac_rx_cfg_mask_t  mask,
    CA_IN  aal_ni_xaui_mac_rx_cfg_t      *config);

ca_status_t aal_ni_xaui_mac_rx_config_get(
    CA_IN  ca_device_id_t                   device_id,
    CA_OUT aal_ni_xaui_mac_rx_cfg_t     *config);

ca_status_t aal_ni_xaui_mac_rx_ctrl_set(
    CA_IN  ca_device_id_t                     device_id,
    CA_IN  aal_ni_xaui_mac_rx_ctrl_mask_t  mask,
    CA_IN aal_ni_xaui_mac_rx_ctrl_t      *config);

ca_status_t aal_ni_xaui_mac_rx_ctrl_get(
    CA_IN  ca_device_id_t                   device_id,
    CA_OUT aal_ni_xaui_mac_rx_ctrl_t    *config);

ca_status_t aal_ni_xaui_mac_tx_ctrl_set(
    CA_IN  ca_device_id_t                     device_id,
    CA_IN  aal_ni_xaui_mac_tx_ctrl_mask_t  mask,
    CA_IN  aal_ni_xaui_mac_tx_ctrl_t      *config);

ca_status_t aal_ni_xaui_mac_tx_ctrl_get(
    CA_IN  ca_device_id_t                   device_id,
    CA_OUT aal_ni_xaui_mac_tx_ctrl_t    *config);

ca_status_t aal_ni_xaui_mac_ctrl_addr_set(
    CA_IN ca_device_id_t    device_id,
    CA_IN ca_mac_addr_t  mac);

ca_status_t aal_ni_xaui_mac_ctrl_addr_get(
    CA_IN  ca_device_id_t    device_id,
    CA_OUT ca_mac_addr_t  mac);

ca_status_t aal_ni_xaui_mac_ctrl_mode_set(
    CA_IN  ca_device_id_t                       device_id,
    CA_IN  aal_ni_xaui_mac_ctrl_mode_mask_t  mask,
    CA_IN  aal_ni_xaui_mac_ctrl_mode_t      *config);

ca_status_t aal_ni_xaui_mac_ctrl_mode_get(
    CA_IN  ca_device_id_t                   device_id,
    CA_OUT aal_ni_xaui_mac_ctrl_mode_t  *config);

ca_status_t aal_ni_xaui_mac_ctrl_flowctrl_set(
    CA_IN ca_device_id_t                          device_id,
    CA_IN aal_ni_xaui_mac_ctrl_flowctrl_mask_t mask,
    CA_IN aal_ni_xaui_mac_ctrl_flowctrl_t     *config);

ca_status_t aal_ni_xaui_mac_ctrl_flowctrl_get(
    CA_IN  ca_device_id_t                       device_id,
    CA_OUT aal_ni_xaui_mac_ctrl_flowctrl_t  *config);

ca_status_t aal_ni_xaui_mac_ctrl_mib_ctrl_set(
    CA_IN  ca_device_id_t                          device_id,
    CA_IN  aal_ni_xaui_mac_ctrl_mib_ctrl_mask_t mask,
    CA_IN aal_ni_xaui_mac_ctrl_mib_ctrl_t      *config);

ca_status_t aal_ni_xaui_mac_ctrl_mib_ctrl_get(
    CA_IN  ca_device_id_t                       device_id,
    CA_OUT aal_ni_xaui_mac_ctrl_mib_ctrl_t  *config);

ca_status_t aal_ni_xaui_mac_ctrl_mgm_cfg_set(
    CA_IN  ca_device_id_t                         device_id,
    CA_IN  aal_ni_xaui_mac_ctrl_mgm_cfg_mask_t mask,
    CA_IN  aal_ni_xaui_mac_ctrl_mgm_cfg_t     *config);

ca_status_t aal_ni_xaui_mac_ctrl_mgm_cfg_get(
    CA_IN  ca_device_id_t                      device_id,
    CA_OUT aal_ni_xaui_mac_ctrl_mgm_cfg_t  *config);

ca_status_t aal_ni_xaui_mac_ctrl_rx_mib_get(
    CA_IN  ca_device_id_t                     device_id,
    CA_OUT aal_ni_xaui_mac_ctrl_rx_mib_t  *counter);

ca_status_t aal_ni_xaui_mac_ctrl_tx_mib_get(
    CA_IN  ca_device_id_t                     device_id,
    CA_OUT aal_ni_xaui_mac_ctrl_tx_mib_t  *mib);

ca_status_t aal_ni_xaui_mac_ctrl_ptp_status_get(
    CA_IN  ca_device_id_t                         device_id,
    CA_OUT aal_ni_xaui_mac_ctrl_ptp_status_t  *ptp_status);

ca_status_t aal_ni_xaui_mac_ctrl_ptp_cfg_set(
    CA_IN  ca_device_id_t                          device_id,
    CA_IN  aal_ni_xaui_mac_ctrl_ptp_cfg_mask_t  mask,
    CA_IN  aal_ni_xaui_mac_ctrl_ptp_cfg_t      *config);

ca_status_t aal_ni_xaui_mac_ctrl_ptp_cfg_get(
    CA_IN  ca_device_id_t                      device_id,
    CA_OUT aal_ni_xaui_mac_ctrl_ptp_cfg_t  *config);

ca_status_t aal_ni_xaui_xgrs_stats_get(
    CA_IN  ca_device_id_t                device_id,
    CA_OUT aal_ni_xaui_xgrs_stats_t  *stats);

ca_status_t aal_ni_xaui_xgrs_tx_cfg_set(
    CA_IN  ca_device_id_t                    device_id,
    CA_IN  aal_ni_xaui_xgrs_tx_cfg_mask_t mask,
    CA_IN  aal_ni_xaui_xgrs_tx_cfg_t     *config);

ca_status_t aal_ni_xaui_xgrs_tx_cfg_get(
    CA_IN  ca_device_id_t                 device_id,
    CA_OUT aal_ni_xaui_xgrs_tx_cfg_t  *config);

ca_status_t aal_ni_xaui_xgrs_rx_cfg_set(
    CA_IN  ca_device_id_t                    device_id,
    CA_IN  aal_ni_xaui_xgrs_rx_cfg_mask_t mask,
    CA_IN  aal_ni_xaui_xgrs_rx_cfg_t     *config);

ca_status_t aal_ni_xaui_xgrs_rx_cfg_get(
    CA_IN  ca_device_id_t                 device_id,
    CA_OUT aal_ni_xaui_xgrs_rx_cfg_t  *config);

ca_status_t aal_ni_xaui_xgxs_stats_get(
    CA_IN  ca_device_id_t                device_id,
    CA_OUT aal_ni_xaui_xgxs_stats_t  *stats);

ca_status_t aal_ni_xaui_xgxs_rx_cfg_set(
    CA_IN  ca_device_id_t                    device_id,
    CA_IN  aal_ni_xaui_xgxs_rx_cfg_mask_t mask,
    CA_IN  aal_ni_xaui_xgxs_rx_cfg_t     *config);

ca_status_t aal_ni_xaui_xgxs_rx_cfg_get(
    CA_IN  ca_device_id_t                 device_id,
    CA_OUT aal_ni_xaui_xgxs_rx_cfg_t  *config);

ca_status_t aal_ni_xaui_xgxs_tx_cfg_set(
    CA_IN  ca_device_id_t                    device_id,
    CA_IN  aal_ni_xaui_xgxs_tx_cfg_mask_t mask,
    CA_IN  aal_ni_xaui_xgxs_tx_cfg_t     *config);

ca_status_t aal_ni_xaui_xgxs_tx_cfg_get(
    CA_IN  ca_device_id_t                 device_id,
    CA_OUT aal_ni_xaui_xgxs_tx_cfg_t  *config);

ca_status_t aal_ni_xaui_xgxs_intr_cfg_set(
    CA_IN  ca_device_id_t                      device_id,
    CA_IN  aal_ni_xaui_xgxs_intr_cfg_mask_t mask,
    CA_IN  aal_ni_xaui_xgxs_intr_cfg_t     *config);

ca_status_t aal_ni_xaui_xgxs_intr_cfg_get(
    CA_IN  ca_device_id_t                   device_id,
    CA_OUT aal_ni_xaui_xgxs_intr_cfg_t  *config);

ca_status_t aal_ni_xaui_xgxs_intr_status_get(
    CA_IN  ca_device_id_t                     device_id,
    CA_OUT aal_ni_xaui_xgxs_intr_status_t  *status);

ca_status_t aal_ni_xaui_xgxs_intr_status_clear(/*AUTO-CLI gen ignore */
    CA_IN  ca_device_id_t                     device_id,
    CA_IN  aal_ni_xaui_xgxs_intr_cfg_t    *clear);

ca_status_t aal_ni_ptp_pps_cfg_set(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  aal_ni_ptp_pps_cfg_mask_t  mask,
    CA_IN  aal_ni_ptp_pps_cfg_t      *config);

ca_status_t aal_ni_ptp_pps_cfg_get(
    CA_IN  ca_device_id_t            device_id,
    CA_OUT aal_ni_ptp_pps_cfg_t  *config);

ca_status_t aal_ni_ptp_pps_timer_get(
    CA_IN  ca_device_id_t    device_id,
    CA_OUT ca_uint32_t   *time);

ca_status_t aal_ni_ptp_hw_timer_set(
    CA_IN  ca_device_id_t    device_id,
    CA_IN  ca_uint64_t    time);


ca_status_t aal_ni_ptp_hw_timer_get(
    CA_IN  ca_device_id_t    device_id,
    CA_OUT ca_uint64_t   *time);

ca_status_t aal_ni_glb_ptp_tx_cfg_get(
    CA_IN  ca_device_id_t                device_id,
    CA_OUT  aal_ni_glb_ptp_tx_cfg_t     *config);

ca_status_t aal_ni_glb_ptp_tx_cfg_set(
    CA_IN  ca_device_id_t                device_id,
    CA_IN  aal_ni_glb_ptp_tx_cfg_mask_t  mask,
    CA_IN  aal_ni_glb_ptp_tx_cfg_t      *config);

ca_status_t  aal_ni_glb_ptp_intr_enable_set(
     CA_IN ca_device_id_t              device_id,
     CA_IN aal_ni_glb_ptp_intr_enable_cfg_mask_t mask,
     CA_IN aal_ni_glb_ptp_intr_enable_cfg_t *config);

ca_status_t aal_ni_glb_ptp_intr_enable_get(
    CA_IN ca_device_id_t              device_id,
    CA_OUT aal_ni_glb_ptp_intr_enable_cfg_t *config);

ca_status_t aal_global_gpio_mux_cfg_set(
     CA_IN  ca_device_id_t                     device_id,
     CA_IN  aal_global_gpio_mux_cfg_mask_t  mask,
     CA_IN  aal_global_gpio_mux_cfg_t      *config);

ca_status_t aal_global_gpio_mux_cfg_get(
    CA_IN  ca_device_id_t                     device_id,
    CA_OUT aal_global_gpio_mux_cfg_t      *config);

ca_status_t aal_ni_test_loop_self_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint8_t  start_port, /* start port: 0 to 7*/
	CA_IN ca_uint8_t  end_port, /* end port: 0 to 7 */
	CA_OUT ca_boolean_t *enable
);
	
ca_status_t aal_ni_test_loop_self_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint8_t  start_port,/* start port: 0 to 7*/
	CA_IN ca_uint8_t  end_port,/* end port: 0 to 7 */
	CA_IN ca_boolean_t enable /* 1 for enable, 0 for disable */
);
	
ca_status_t aal_ni_test_loop_snake_get(
	CA_IN ca_device_id_t dev,
	CA_OUT ca_boolean_t *enable
);
	
ca_status_t aal_ni_test_loop_snake_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_boolean_t enable /* 1 for enable, 0 for disable */
);
ca_status_t aal_ni_rxmux_fc_thrshld_get(
    CA_IN  ca_device_id_t                      device_id,
    CA_IN  ca_uint32_t                      port_id,
    CA_OUT  aal_ni_rxmux_fc_thrshld_t        *cfg
);

ca_status_t aal_ni_rxmux_fc_thrshld_set(
    CA_IN  ca_device_id_t                      device_id,
    CA_IN  ca_uint32_t                      port_id,
    CA_IN  aal_ni_rxmux_fc_thrshld_mask_t   mask,
    CA_IN  aal_ni_rxmux_fc_thrshld_t        *cfg
);


#endif

