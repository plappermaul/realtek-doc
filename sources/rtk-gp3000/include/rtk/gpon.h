/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
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
 * Purpose : Definition of GPON Mac API
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

#ifndef __RTK_GPON_H__
#define __RTK_GPON_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>

/*
 * GPON MAC Device Version Infomation. Read from ASIC. For Debugging
 */
typedef struct rtk_gpon_device_ver_s{
#if 0 /* design_id is removed in GPON_MAC_SWIO_v1.1 */
    uint8 design_id;
#endif
    uint32 gmac_ver;
}rtk_gpon_device_ver_t;

/*
 * GPON MAC Driver Software Version Infomation. Read from GPON Mac Driver Software. For Debugging
 */
typedef struct rtk_gpon_driver_ver_s{
    uint8   release;
    uint8   load;
    uint16  build;
}rtk_gpon_driver_ver_t;

/*
 * GPON MAC Serial Number Structure.
 */
#define RTK_GPON_VENDOR_ID_LEN           4
#define RTK_GPON_VENDOR_SPECIFIC_LEN     4

typedef struct rtk_gpon_serialNumber_s{
    uint8 vendor[RTK_GPON_VENDOR_ID_LEN];
    uint8 specific[RTK_GPON_VENDOR_SPECIFIC_LEN];
}rtk_gpon_serialNumber_t;

/*
 * GPON MAC Password Structure.
 */
#define RTK_GPON_PASSWORD_LEN            10

typedef struct rtk_gpon_password_s{
    uint8 password[RTK_GPON_PASSWORD_LEN];
}rtk_gpon_password_t;

/*
 * GPON MAC Parameters Structure.
 */

typedef enum rtk_gpon_patameter_type_e{
    RTK_GPON_PARA_TYPE_ONU_ACTIVATION = 0,
    RTK_GPON_PARA_TYPE_LASER,
    RTK_GPON_PARA_TYPE_DS_PHY,
    RTK_GPON_PARA_TYPE_DS_PLOAM,
    RTK_GPON_PARA_TYPE_DS_BWMAP,
    RTK_GPON_PARA_TYPE_DS_GEM,
    RTK_GPON_PARA_TYPE_DS_ETH,
    RTK_GPON_PARA_TYPE_DS_OMCI,
#if 0
    RTK_GPON_PARA_TYPE_DS_TDM,
#endif
    RTK_GPON_PARA_TYPE_US_PHY,
    RTK_GPON_PARA_TYPE_US_PLOAM,
    RTK_GPON_PARA_TYPE_US_DBR,

#if 0 /* page US_FRAG is removed */
    RTK_GPON_PARA_TYPE_US_GEM,
    RTK_GPON_PARA_TYPE_US_ETH,
#endif
#if 0 /* GEM US omci register is removed */
    RTK_GPON_PARA_TYPE_US_OMCI,
#endif
#if 0 /* GPON_GEM_US_OPT_CFG is removed in GPON_MAC_SWIO_r1.1 */
    RTK_GPON_PARA_TYPE_US_SAV,
#endif

    RTK_GPON_PARA_TYPE_ONUID,
    RTK_GPON_PARA_TYPE_DRV_STATUS,

    RTK_GPON_PARA_TYPE_MAX
}rtk_gpon_patameter_type_t;

typedef struct rtk_gpon_onu_activation_para_s{
    uint32      to1_timer;                  /* the maximum time for ONU to wait for SN request in O3 state or ranging request in O4 state */
    uint32      to2_timer;                  /* the maximum time for ONU to wait for a POPUP message in O6 state */
}rtk_gpon_onu_activation_para_t;

typedef struct rtk_gpon_laser_para_s{
    int32    laser_optic_los_en;         /* whether the optical LOS input is enabled */
    int32    laser_optic_los_polar;      /* the optical LOS input polarity */
    int32    laser_cdr_los_en;           /* whether the CDR LOS input is enabled */
    int32    laser_cdr_los_polar;        /* the CDR LOS input polarity */
    int32    laser_los_filter_en;        /* whether LOS filter is enabled, if enabled, the LOS is raised after being stable for more than 1 ms */
    uint8    laser_us_on;                /* the optical control offset for the laser on */
    uint8    laser_us_off;               /* the optical control offset for the laser off */
}rtk_gpon_laser_para_t;

typedef struct rtk_gpon_ds_physical_para_s
{
    int32    ds_scramble_en;             /* whether the D/S frame is scrambled */
    int32    ds_fec_bypass;              /* whether the D/S FEC is bypass */
    uint8    ds_fec_thrd;                /* the threshold for continuous FEC indicator before it is taken effect */
}rtk_gpon_ds_physical_para_t;

typedef struct rtk_gpon_ds_ploam_para_s{
#if 0 /* rename ploam_crc_check to ploam_drop_crce GPON_MAC_SWIO_v1.1 */
    int32    ds_ploam_crc_chk;           /* whether the D/S ploam crc is checked */
#endif
    int32    ds_ploam_onuid_filter;      /* whether the D/S ploam is filtered according to onu id */
    int32    ds_ploam_broadcast_accpt;   /* whether the broadcast D/S ploam is accepted */
    int32    ds_ploam_drop_crc_err;      /* whether the D/S ploam with CRC error is drop */
}rtk_gpon_ds_ploam_para_t;

typedef struct rtk_gpon_ds_bwMap_para_s{
    int32    ds_bwmap_crc_chk;           /* whether the bwmap crc is checked */
    int32    ds_bwmap_onuid_filter;      /* whether the bwmap is filtered onu id */
    int32    ds_bwmap_plend_mode;        /* mode for process received PLENd */
}rtk_gpon_ds_bwMap_para_t;

typedef struct rtk_gpon_ds_gem_para_s{
    uint8    assemble_timer;             /* number of GPON frames for assembly timeout threshold */
}rtk_gpon_ds_gem_para_t;

typedef struct rtk_gpon_ds_eth_para_s{
    int32    ds_eth_crc_chk;             /* whether the D/S Ethernet packet fcs is checked */
#if 0 /* GEM FCB page is removed */
    int32    ds_eth_crc_rm;              /* whether the D/S Ethernet packet fcs is removed */
#endif
    uint8    ds_eth_pti_mask;            /* the Ethernet flow PTI mask */
    uint8    ds_eth_pti_ptn;             /* the Ethernet flow PTI pattern to indicate the end of the fragment */
}rtk_gpon_ds_eth_para_t;

typedef struct rtk_gpon_ds_omci_para_s{
#if 0 /* GEM FCB page is removed */
    int32    ds_omci_crc_chk;            /* whether the D/S OMCI crc is checked */
#endif
    uint8    ds_omci_pti_mask;           /* the OMCI PTI mask */
    uint8    ds_omci_pti_ptn;            /* the OMCI PTI pattern to indicate the end of the fragment */
}rtk_gpon_ds_omci_para_t;

#if 0 /* TDM PTI is removed in GPON_MAC_SWIO_r1.1 */
typedef struct rtk_gpon_ds_tdm_para_s{
    uint8    ds_tdm_pti_mask;            /* the TDM PTI mask */
    uint8    ds_tdm_pti_ptn;             /* the TDM PTI pattern to indicate the end of the fragment */
}rtk_gpon_ds_tdm_para_t;
#endif

typedef struct rtk_gpon_us_physical_para_s{
    int32    us_scramble_en;             /* whether the U/S burst is scrambled */
    int32    us_burst_en_polar;          /* the polarity of the burst enable */
    int32    small_sstart_en;            /* whether enable the process small SSTART(<BOH len) automatically */
    int32    suppress_laser_en;          /* whether the function of suppressing laser when ONT is outside of state O3, O4 and O5 */
}rtk_gpon_us_physical_para_t;

typedef struct rtk_gpon_us_ploam_para_s{
    int32    us_ploam_en;                /* whether the PLOAMu is enabled. If not, all zero is sent */
    int32    us_ind_normal_ploam;        /* 1: the indicator[7] is set on when there is any PLOAMu watting. 0: only urgent PLOAMu */
}rtk_gpon_us_ploam_para_t;

typedef struct rtk_gpon_us_dbr_para_s{
    int32    us_dbru_en;                 /* whether the DBRu is enabled. If not, all zero is sent */
#if 0 /* page DBRu is removed. */
    int32    us_dbru_plou_en;            /* whether the status bits in the PLOu is enabled. if disabled, the chipset will generate those bits automatically */
    int32    us_dbru_piggyback_en;       /* whether the piggyback DBRu data is overwritten. if disabled, the chipset will generate data automatically */
#endif
}rtk_gpon_us_dbr_para_t;

#if 0 /* page US_FRAG is removed */
typedef struct rtk_gpon_us_gem_para_s{
    uint16   us_max_pdu;                 /* the maximum protocol data unit length */
}rtk_gpon_us_gem_para_t;

typedef struct rtk_gpon_us_eth_para_s{
    int32    us_frag_en;                 /* whether enable the fragmentation of the OMCI packet */
    int32    us_eth_fcs_insert_en;       /* whether the fcs is inserted into Ethernet packet */
}rtk_gpon_us_eth_para_t;
#endif

#if 0 /* GEM US omci register is removed */
typedef struct rtk_gpon_us_omci_para_s{
    uint8    us_omci_pti;                /* the U/S OMCI PTI bit 2 and bit 1 */
    int32    us_omci_frag_en;            /* whether the OMCI can be fragmentized */
}rtk_gpon_us_omci_para_t;
#endif

#if 0 /* GPON_GEM_US_OPT_CFG is removed in GPON_MAC_SWIO_r1.1 */
typedef struct rtk_gpon_us_opt_pwr_sav_s{
    uint32   us_opt_pwr_sav_en;            /* the U/S OMCI PTI bit 2 and bit 1 */
    uint32   us_opt_ahead_cycle;           /* whether the OMCI can be fragmentized */
    uint32   us_opt_behind_cycle;          /* whether the OMCI can be fragmentized */
}rtk_gpon_us_opt_pwr_sav_t;
#endif

/*
 * GPON MAC Initial FSM State.
 */
typedef enum rtk_gpon_initialState_e{
    RTK_GPONMAC_INIT_STATE_O1 = 0,          /* GPON ONU FSM start from Initial(O1) state */
    RTK_GPONMAC_INIT_STATE_O7 = 1,          /* GPON ONU FSM start from Emergency Stop(O7) state */
}rtk_gpon_initialState_t;

/*
 * GPON MAC FSM State.
 */
typedef enum rtk_gpon_fsm_status_e{
    RTK_GPONMAC_FSM_STATE_UNKNOWN = 0,      /* Unknown State: Get Status fail */
    RTK_GPONMAC_FSM_STATE_O1,               /* Initial State */
    RTK_GPONMAC_FSM_STATE_O2,               /* Standby State */
    RTK_GPONMAC_FSM_STATE_O3,               /* Serial Number State */
    RTK_GPONMAC_FSM_STATE_O4,               /* Ranging State */
    RTK_GPONMAC_FSM_STATE_O5,               /* Operation State */
    RTK_GPONMAC_FSM_STATE_O6,               /* POPUP State */
    RTK_GPONMAC_FSM_STATE_O7,               /* Emergency Stop State */
}rtk_gpon_fsm_status_t;

/*
 * GPON MAC TCONT Structure
 */
#define RTK_GPON_ALLOC_ID_NOUSE          0xFFFF

typedef enum rtk_gpon_tcontType_e{
    RTK_GPON_TCONT_TYPE_1 = 1,
    RTK_GPON_TCONT_TYPE_2,
    RTK_GPON_TCONT_TYPE_3,
    RTK_GPON_TCONT_TYPE_4,
    RTK_GPON_TCONT_TYPE_5
}rtk_gpon_tcontType_t;

typedef struct rtk_gpon_tcont_ind_s{
    uint32                alloc_id;
    rtk_gpon_tcontType_t  type;
}rtk_gpon_tcont_ind_t;

typedef struct rtk_gpon_tcont_attr_s{
    uint32 tcont_id;
}rtk_gpon_tcont_attr_t;

/*
 * GPON MAC Flow Structure
 */
typedef enum rtk_gpon_flowType_e{
    RTK_GPON_FLOW_TYPE_OMCI = 0,
    RTK_GPON_FLOW_TYPE_ETH,
    RTK_GPON_FLOW_TYPE_TDM
}rtk_gpon_flowType_t;

typedef enum rtk_gpon_flowPriority_e{
    RTK_GPON_FLOW_PRIO_LOW = 0,
    RTK_GPON_FLOW_PRIO_HIGH
}rtk_gpon_flowPriority_t;

#define RTK_GPON_GEMPORT_ID_NOUSE        0xFFFF

typedef struct rtk_gpon_dsFlow_attr_s{
    uint32                      gem_port_id;
    rtk_gpon_flowType_t         type;
    int32                       multicast;
    int32                       aes_en;
}rtk_gpon_dsFlow_attr_t;

typedef struct rtk_gpon_usFlow_attr_s{
    uint32                      gem_port_id;
    rtk_gpon_flowType_t         type;
    uint32                      tcont_id;
    uint8                       channel;
}rtk_gpon_usFlow_attr_t;
/*
 * GPON MAC PLOAM Message Structure, without CRC octet.
 */
#define RTK_GPON_PLOAM_MSG_LEN           10

typedef struct rtk_gpon_ploam_e{
    uint8 onuid;
    uint8 type;
    uint8 msg[RTK_GPON_PLOAM_MSG_LEN];
}rtk_gpon_ploam_t;

/*
 * GPON MAC OMCI Message Structure, a whole message with CRC octets
 */
//#define RTK_GPON_OMCI_MSG_LEN            64
#define RTK_GPON_OMCI_MSG_LEN            48

enum {
	RTK_GPON_MSG_OMCI,
	RTK_GPON_MSG_PLOAM,
};

typedef struct rtk_gpon_omci_msg_s{
    uint8 msg[RTK_GPON_OMCI_MSG_LEN];
}rtk_gpon_omci_msg_t;


typedef union rtk_gpon_msg_u{
	rtk_gpon_omci_msg_t 	omci;
	rtk_gpon_ploam_t 		ploam;
}rtk_gpon_msg_t;

typedef struct rtk_gpon_pkt_s {
	int type;
	rtk_gpon_msg_t msg;
}rtk_gpon_pkt_t;

/*
 * GPON MAC AES KEY Structure
 */
#define RTK_GPON_AES_KEY_LEN             16

typedef struct rtk_gpon_aes_key_s{
    uint8 key[RTK_GPON_AES_KEY_LEN];
}rtk_gpon_aes_key_t;

/*
 * GPON DBRu Piggy Back Mode Data Structure
 */
#define RTK_GPON_PDBR_DATA_LEN_MODE0     1
#define RTK_GPON_PDBR_DATA_LEN_MODE1     2
#define RTK_GPON_PDBR_DATA_LEN_MODE2     4

typedef struct rtk_gpon_piggyback_dbr_data_s{
    uint8 mode0[RTK_GPON_PDBR_DATA_LEN_MODE0];
    uint8 mode1[RTK_GPON_PDBR_DATA_LEN_MODE1];
    uint8 mode2[RTK_GPON_PDBR_DATA_LEN_MODE2];
}rtk_gpon_piggyback_dbr_data_t;

/*
 * GPON MAC MACAddress Table Operation Mode
 */
typedef enum rtk_gpon_macTable_exclude_mode_e{
    RTK_GPON_MACTBL_MODE_INCLUDE = 0,
    RTK_GPON_MACTBL_MODE_EXCLUDE,
}rtk_gpon_macTable_exclude_mode_t;

/*
 * GPON MAC MACAddress filter Operation Mode
 */
typedef enum rtk_gpon_mc_force_mode_e{
    RTK_GPON_MCFORCE_MODE_NORMAL = 0,
    RTK_GPON_MCFORCE_MODE_PASS,
    RTK_GPON_MCFORCE_MODE_DROP
}rtk_gpon_mc_force_mode_t;

/*
 * GPON MAC MACAddress Table Entry Structure
 */
#define RTK_GPON_MACTABLE_ADDR_LEN       6

typedef struct rtk_gpon_macTable_entry_s{
    uint8  mac_addr[RTK_GPON_MACTABLE_ADDR_LEN];
}rtk_gpon_macTable_entry_t;

/*
 * GPON MAC Chunk Header Structure
 */
typedef struct rtk_gpon_chunkHeader_s{
    uint8 head_size;
    uint8 task_ind;
    uint8 err_code;
    uint8 src;
}rtk_gpon_chunkHeader_t;

/*
 * GPON MAC GEM FCB Shaper Structure
 */
typedef struct rtk_gpon_shaper_s{
    uint16 tick;
    uint16 inc_size;
    uint16 thrd_high;
    uint16 thrd_low;
}rtk_gpon_shaper_t;

/*
 * GPON MAC GEM Port Packet Drop History Status
 */
#define RTK_GPON_DROP_HIS_TBL_LEN        8

/*
 *               MSB       LSB
 * stat[0]: port  15--port   0
 * stat[1]: port  31--port  16
 * ... ...
 * stat[7]: port 127--port 112
 */
typedef struct rtk_gpon_dropHisStatus_s{
    uint16 stat[RTK_GPON_DROP_HIS_TBL_LEN];
}rtk_gpon_dropHisStatus_t;

/*
 * GPON MAC Alarm Type
 */
typedef enum rtk_gpon_alarm_type_e{
    RTK_GPON_ALARM_NONE = 0,
    RTK_GPON_ALARM_LOS = 1,
    RTK_GPON_ALARM_LOF = 2,
    RTK_GPON_ALARM_LOM = 3,
    RTK_GPON_ALARM_MAX
} rtk_gpon_alarm_type_t;

/*
 * GPON MAC global Performance Type
 */
typedef enum rtk_gpon_global_performance_type_e{
    RTK_GPON_PMTYPE_ONU_ACTIVATION = 0,  /* NOT USED, reserved */
    RTK_GPON_PMTYPE_DS_PHY,              /* D/S Physical Statistics */
    RTK_GPON_PMTYPE_DS_PLOAM,            /* D/S PLOAM Statistics */
    RTK_GPON_PMTYPE_DS_BWMAP,            /* D/S BwMap Statistics */
    RTK_GPON_PMTYPE_DS_GEM,              /* D/S GEM Statistics */
    RTK_GPON_PMTYPE_DS_ETH,              /* D/S Ethernet Traffic Statistics */
    RTK_GPON_PMTYPE_DS_OMCI,             /* D/S OMCI Statistics */
    RTK_GPON_PMTYPE_US_PHY,              /* U/S Physical Statistics */
    RTK_GPON_PMTYPE_US_DBR,              /* U/S DBR Statistics */
    RTK_GPON_PMTYPE_US_PLOAM,            /* U/S PLOAM Statistics */
    RTK_GPON_PMTYPE_US_GEM,              /* U/S GEM Statistics */
    RTK_GPON_PMTYPE_US_ETH,              /* U/S Ethernet Traffic Statistics */
    RTK_GPON_PMTYPE_US_OMCI,             /* U/S OMCI Statistics */
    RTK_GPON_PMTYPE_MAX
}rtk_gpon_global_performance_type_t;

/*
 * GPON MAC TCont Performance Type
 */
typedef enum rtk_gpon_tcont_performance_type_e{
    RTK_GPON_PMTYPE_TCONT_GEM,           /* TCONT GEM Statistics */
    RTK_GPON_PMTYPE_TCONT_ETH,           /* TCONT ETH Statistics */
    RTK_GPON_PMTYPE_TCONT_IDLE,          /* TCONT IDLE Statistics */
    RTK_GPON_PMTYPE_TCONT_MAX
}rtk_gpon_tcont_performance_type_t;

/*
 * GPON MAC Flow Performance Type
 */
typedef enum rtk_gpon_flow_performance_type_e{
    RTK_GPON_PMTYPE_FLOW_DS_GEM,         /* D/S Flow GEM Statistics */
    RTK_GPON_PMTYPE_FLOW_DS_ETH,         /* D/S Flow ETH Statistics */
    RTK_GPON_PMTYPE_FLOW_US_GEM,         /* U/S Flow GEM Statistics */
    RTK_GPON_PMTYPE_FLOW_US_ETH,         /* U/S Flow ETH Statistics */
    RTK_GPON_PMTYPE_FLOW_MAX
}rtk_gpon_flow_performance_type_t;

/*
 * GPON MAC GTC level Performance Counter
 */

/*
 * ONU Activation Performance Counter
 */
typedef struct rtk_gpon_onu_activation_counter_s{
    uint16 rx_sn_req;                   /* Received SN Request Counter */
    uint16 rx_ranging_req;              /* Received Ranging Request Counter */
}rtk_gpon_onu_activation_counter_t;

/*
 * D/S Physical Performance Counter
 */
typedef struct rtk_gpon_ds_physical_counter_s{
    uint32 rx_fec_correct_bit;          /* Received FEC Correct Bits Counter */
    uint32 rx_fec_correct_byte;         /* Received FEC Correct Bytes Counter */
    uint32 rx_fec_correct_cw;           /* Received FEC Correct Codeword Counter */
    uint32 rx_fec_uncor_cw;             /* Received FEC Uncorrect Codeword Counter */
    uint32 rx_bip_err_bit;              /* Received BIP Error Bit Counter */
    uint32 rx_bip_err_block;            /* Received BIP Error Block Counter */
    uint16 rx_lom;                      /* Received Superframe LOS of Synchronization Counter */
    uint16 rx_plen_err;                 /* Received PLen fail Counter */
}rtk_gpon_ds_physical_counter_t;

/*
 * D/S PLOAM Performance Counter
 */
typedef struct rtk_gpon_ds_ploam_counter_s{
    uint32 rx_ploam_cnt;                /* Received PLOAM Counter */
    uint32 rx_ploam_correctted;         /* Received PLOAM Correctted Counter */
    uint16 rx_ploam_err;                /* Received PLOAM CRC Error Counter */
    uint16 rx_ploam_proc;               /* Received PLOAM Processed by CPU Counter */
    uint16 rx_ploam_overflow;           /* Received PLOAM Dropped Counter */
}rtk_gpon_ds_ploam_counter_t;

/*
 * D/S BwMap Performance Counter
 */
typedef struct rtk_gpon_ds_bwMap_counter_s{
    uint32 rx_bwmap_cnt;                /* Received BwMap Counter */
    uint16 rx_bwmap_crc_err;            /* Received BwMap CRC Error Counter */
    uint16 rx_bwmap_overflow;           /* Received BwMap Dropped Counter (interleaving>32)*/
    uint16 rx_bwmap_inv0;               /* Received invalid BwMap Counter (SStop<SStart) */
    uint16 rx_bwmap_inv1;               /* Received invalid BwMap Counter (SStop>19439) */
}rtk_gpon_ds_bwMap_counter_t;

/*
 * D/S GEM Layer Performance Counter
 */
typedef struct rtk_gpon_ds_gem_counter_s{
    uint32 rx_gem_los;                  /* D/S GEM Los Counter */
    uint32 rx_hec_correct;              /* D/S GEM Header Corrected by HEC Counter */
    uint32 rx_gem_idle;                 /* D/S GEM Idle Counter */
    uint32 rx_gem_non_idle;             /* D/S GEM non Idle Counter */
    uint32 rx_over_interleaving;        /* Received Over Interleaving Counter */
    uint16 rx_gem_len_mis;              /* D/S GEM packet dropped Counter due to the packet length mismatch */
    uint16 rx_match_multi_flow;         /* D/S GEM packet dropped Counter due to port ID match more than 1 flow */
    uint32 rx_chunk_overflow;           /* Chunk Dropped due to Overflow Counter */
    uint32 rx_chunk_fwd;                /* Sent Chunk Forward Counter */
    uint32 rx_chunk_head;               /* Sent Chunk Header Counter */
    uint32 rx_chunk_body;               /* Sent Chunk Body Counter */
    uint32 rx_chunk_last;               /* Sent Chunk Tail Counter */
    uint32 rx_chunk_self;               /* Sent Self-Containing Chunk Counter */
}rtk_gpon_ds_gem_counter_t;

/*
 * D/S Ethernet Traffic Performance Counter
 */
typedef struct rtk_gpon_ds_eth_counter_s{
    uint32 rx_eth_unicast;              /* Received Unicast Ethernet Packet Counter */
    uint32 rx_eth_multicast;            /* Received Multicast Ethernet Packet Counter */
    uint32 rx_eth_multicast_fwd;        /* Received Multicast Ethernet Packet Forworded Counter */
    uint32 rx_eth_multicast_leak;       /* Received Multicast Ethernet Packet Leaked Counter */
    uint32 rx_eth_fcs_err;              /* Ethernet Packet FCS Error Counter */
}rtk_gpon_ds_eth_counter_t;

/*
 * D/S OMCI Performance Counter
 */
typedef struct rtk_gpon_ds_omci_counter_s{
    uint32 rx_omci;                     /* Received OMCI Counter */
    uint32 rx_omci_crc_err;             /* Received OMCI CRC Error Counter */
    uint32 rx_omci_drop;                /* Received OMCI Dropped Counter */
    uint32 rx_omci_proc;                /* Received OMCI Processed by CPU Counter */
	uint32 rx_omci_byte;                /* Received OMCI byte Counter */
}rtk_gpon_ds_omci_counter_t;

/*
 * U/S Physical Performance Counter
 */
typedef struct rtk_gpon_us_physical_counter_s{
    uint16 tx_boh_cnt;                  /* Transmit Burst Overhead Counter */
}rtk_gpon_us_physical_counter_t;

/*
 * U/S DBR Performance Counter
 */
typedef struct rtk_gpon_us_dbr_counter_s{
    uint16 tx_dbru_cnt;                 /* Transmit DBRu Counter */
    uint16 tx_dbru_flag_err;            /* DBRu Packet with Error Flag Counter */
    uint16 tx_dbru_seq_err;             /* DBRu Packet with Error Sequence Counter */
}rtk_gpon_us_dbr_counter_t;

/*
 * U/S PLOAM Performance Counter
 */
typedef struct rtk_gpon_us_ploam_counter_s{
    uint16 tx_ploam_cnt;                /* Transmit PLOAM Counter */
    uint16 tx_ploam_proc;               /* Transmit PLOAM by CPU Counter */
    uint16 tx_ploam_urg;                /* Transmit Urgent PLOAM Counter */
    uint16 tx_ploam_urg_proc;           /* Transmit Urgent PLOAM by CPU Counter */
    uint16 tx_ploam_nor;                /* Transmit Normal PLOAM Counter */
    uint16 tx_ploam_nor_proc;           /* Transmit Normal PLOAM by CPU Counter */
    uint16 tx_ploam_sn;                 /* Transmit SN PLOAM Counter */
    uint16 tx_ploam_nomsg;              /* Transmit No Msg PLOAM Counter */
}rtk_gpon_us_ploam_counter_t;

/*
 * U/S GEM Layer Performance Counter
 */
typedef struct rtk_gpon_us_gem_counter_s{
    uint16 tx_gem_cnt;                  /* Transmit GEM Counter */
    uint32 tx_gem_byte;                 /* Transmit GEM Byte Counter */
    uint32 tx_gem_idle;                 /* Transmit GEM Idle Counter */
    uint32 tx_gem_pkt;                  /* Transmit GEM Packet Counter */
}rtk_gpon_us_gem_counter_t;

/*
 * D/S Traffic Performance Counter
 */
typedef struct rtk_gpon_us_eth_counter_s{
    uint32 tx_eth_abort_ebb;            /* Aborted Ethernet Packets Counter by EBB */
}rtk_gpon_us_eth_counter_t;

/*
 * U/S OMCI Performance Counter
 */
typedef struct rtk_gpon_us_omci_counter_s{
    uint32 tx_omci_urg;                 /* Transmit High Priority OMCI Counter */
    uint32 tx_omci_nor;                 /* Transmit Low Priority OMCI Counter */
    uint32 tx_omci_proc;                /* Transmit OMCI Processed by CPU Counter */
    uint32 tx_omci_proc_urg;            /* Transmit High Priority OMCI Processed by CPU Counter */
    uint32 tx_omci_proc_nor;            /* Transmit Low Priority OMCI Processed by CPU Counter */
	uint32 tx_omci;                     /* Transmit OMCI Counter */
    uint32 tx_omci_byte;                /* Transmit OMCI byte Counter */
}rtk_gpon_us_omci_counter_t;

/*
 * Global Performance Counter
 */
typedef union rtk_gpon_global_counter_u{
    rtk_gpon_onu_activation_counter_t  activate;
    rtk_gpon_ds_physical_counter_t dsphy;
    rtk_gpon_ds_ploam_counter_t dsploam;
    rtk_gpon_ds_bwMap_counter_t dsbwmap;
    rtk_gpon_ds_gem_counter_t dsgem;
    rtk_gpon_ds_eth_counter_t dseth;
    rtk_gpon_ds_omci_counter_t dsomci;
    rtk_gpon_us_physical_counter_t usphy;
    rtk_gpon_us_dbr_counter_t usdbr;
    rtk_gpon_us_ploam_counter_t usploam;
    rtk_gpon_us_gem_counter_t usgem;
    rtk_gpon_us_eth_counter_t useth;
    rtk_gpon_us_omci_counter_t usomci;
}rtk_gpon_global_counter_t;

/*
 * GPON MAC TCONT level Performance Counter
 */
typedef struct rtk_gpon_tcont_gem_counter_s{
    uint32 gem_pkt;                     /* U/S GEM Packet Counter */
}rtk_gpon_tcont_gem_counter_t;

typedef struct rtk_gpon_tcont_eth_counter_s{
    uint32 eth_pkt;                     /* U/S Ethernet Packet Counter */
}rtk_gpon_tcont_eth_counter_t;

typedef struct rtk_gpon_tcont_idle_counter_s{
    uint64 idle_byte;                   /* U/S idle byte Counter */
}rtk_gpon_tcont_idle_counter_t;

/*
 * TCont Performance Counter
 */
typedef union rtk_gpon_tcont_counter_u{
    rtk_gpon_tcont_gem_counter_t  gem;
    rtk_gpon_tcont_eth_counter_t  eth;
    rtk_gpon_tcont_idle_counter_t idle;
}rtk_gpon_tcont_counter_t;

/*
 * GPON MAC D/S Flow level Performance Counter
 */
typedef struct rtk_gpon_dsFlow_gem_counter_s{
    uint32 gem_block;                   /* D/S GEM Block Counter */
    uint32 gem_byte;                    /* D/S GEM Byte Counter */
}rtk_gpon_dsFlow_gem_counter_t;

typedef struct rtk_gpon_dsFlow_eth_counter_s{
    uint32 eth_pkt_rx;                  /* D/S Ethernet Packet Counter */
    uint32 eth_pkt_fwd;                 /* D/S Forward Ethernet Packet Counter */
}rtk_gpon_dsFlow_eth_counter_t;

/*
 * GPON MAC U/S Flow level Performance Counter
 */
typedef struct rtk_gpon_usFlow_gem_counter_s{
    uint32 gem_block;                   /* U/S GEM Block Counter */
    uint64 gem_byte;                    /* U/S GEM Byte Counter */
}rtk_gpon_usFlow_gem_counter_t;

typedef struct rtk_gpon_usFlow_eth_counter_s{
    uint32 eth_cnt;                    /* U/S GEM Byte Counter */
}rtk_gpon_usFlow_eth_counter_t;

/*
 * Flow Performance Counter
 */
typedef union rtk_gpon_flow_counter_u{
    rtk_gpon_dsFlow_gem_counter_t dsgem;
    rtk_gpon_dsFlow_eth_counter_t dseth;
    rtk_gpon_usFlow_gem_counter_t usgem;
    rtk_gpon_usFlow_eth_counter_t useth;
}rtk_gpon_flow_counter_t;

/*
 * GPON MAC Register R/W Structure
 */
typedef struct rtk_gpon_register_s{
    uint32 page;
    uint32 offset;
    uint32 value;
}rtk_gpon_register_t;

/*
 * GPON MAC Laser Status
 */
typedef enum rtk_gpon_laser_status_e{
    RTK_GPON_LASER_STATUS_NORMAL = 0,
    RTK_GPON_LASER_STATUS_FORCE_ON = 1,
    RTK_GPON_LASER_STATUS_FORCE_OFF = 2
}rtk_gpon_laser_status_t;

typedef enum rtk_gpon_tx_ptn_mode_e{
    RTK_GPON_TX_PTN_MODE_NORMAL = 0,
    RTK_GPON_TX_PTN_MODE_PRBS = 1,
    RTK_GPON_TX_PTN_MODE_CUSTOM = 2,
    RTK_GPON_TX_PTN_MODE_NONE = 3

}rtk_gpon_tx_ptn_mode_t;

typedef enum rtk_gpon_fsm_event_e{
    GPON_FSM_EVENT_LOS_CLEAR = 0,   /* LOS/LOF clear */
    GPON_FSM_EVENT_RX_UPSTREAM,     /* Receive Upstream Overhead Ploam */
    GPON_FSM_EVENT_RX_SN_REQ,       /* Receive SN Request */
    GPON_FSM_EVENT_RX_ONUID,        /* Receive assign ONU ID Ploam */
    GPON_FSM_EVENT_RX_RANGING_REQ,  /* Receive Ranging Request */
    GPON_FSM_EVENT_RX_EQD,          /* Receive EqD Ploam */
    GPON_FSM_EVENT_TO1_EXPIRE,      /* TO1 expires */
    GPON_FSM_EVENT_RX_DEACTIVATE,   /* Receive DeActivate Ploam */
    GPON_FSM_EVENT_LOS_DETECT,      /* LOS/LOF detect */
    GPON_FSM_EVENT_RX_BC_POPUP,     /* Receive Broadcast Popup Ploam */
    GPON_FSM_EVENT_RX_DIRECT_POPUP, /* Receive Direct Popup Ploam */
    GPON_FSM_EVENT_TO2_EXPIRE,      /* TO2 expires */
    GPON_FSM_EVENT_RX_DISABLE,      /* Receive Disable Ploam */
    GPON_FSM_EVENT_RX_ENABLE,       /* Receive Enable Ploam */
    GPON_FSM_EVENT_MAX
}rtk_gpon_fsm_event_t;

/*
 * GPON MAC FSM State Change Callback.
 * It is to be triggered when the ONU state is changed.
 */
 /*modified by haoship,date:2012.6.12,add a attr "event",reason:support func "rtk_handle_state_change()"in rtk_drv_callback.c  */
typedef void (*rtk_gpon_eventHandleFunc_stateChange_t)(rtk_gpon_fsm_event_t event,rtk_gpon_fsm_status_t newstatus, rtk_gpon_fsm_status_t oldstatus);

/*
 * GPON MAC FEC Status Change Callback.
 * It is to be triggered when the FEC Status is changed.
 */
typedef void (*rtk_gpon_eventHandleFunc_fecChange_t)(int32 newstatus);

/*
 * GPON MAC U/S PLOAM Queue Empty Callback.
 * It is to be triggered when the Queue of the U/S PLOAM is empty after some U/S PLOAMs are requested to send.
 */
typedef void (*rtk_gpon_eventHandleFunc_usPloamEmpty_t)(void);

/*
 * GPON MAC PLOAM Event Report.
 * It is to be triggered when the PLOAM Message is received.
 */
typedef void (*rtk_gpon_eventHandleFunc_ploam_t)(rtk_gpon_ploam_t* ploam);

/*
 * GPON MAC OMCI Event Report.
 * It is to be triggered when the OMCI Message is received.
 */
typedef void (*rtk_gpon_eventHandleFunc_omci_t)(rtk_gpon_omci_msg_t* omci);

/*
 * GPON MAC AES KEY Query Callback.
 * It is to be triggered when the AES Key is queryed by OLT through the PLOAM.
 */
typedef void (*rtk_gpon_callbackFunc_queryAesKey_t)(rtk_gpon_aes_key_t* key);

/*
 * GPON MAC Alarm Event Report.
 * It is to be triggered when the Alarm is set or clear.
 */
typedef void (*rtk_gpon_eventHandleFunc_fault_t)(int32 set);

/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_gpon_driver_initialize
 * Description:
 *      GPON Mac Drv Initialization. To start the GPON Mac Drv.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      Must initialize classification module before calling any GPON APIs.
 */
extern int32
rtk_gpon_driver_initialize(void);

/* Function Name:
 *      rtk_gpon_driver_deInitialize
 * Description:
 *      GPON Mac Drv De-Initialization. To Stop the GPON Mac Drv. The last function call for GPON Mac Drv.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The Device should be de-initialized before this function call.
 */
extern int32
rtk_gpon_driver_deInitialize(void);

/* Function Name:
 *      rtk_gpon_device_initialize
 * Description:
 *      GPON Mac Device Initialization. To start the device of the GPON Mac.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      This function should be called after the Drv Initialization and before any other operation.
 *      It should be called after Device is de-initialized but the Drv is not be de-initialized.
 */
extern int32
rtk_gpon_device_initialize(void);

/* Function Name:
 *      rtk_gpon_device_deInitialize
 * Description:
 *      GPON Mac Device De-Initialization. To stop the device of the GPON Mac.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the Drv is de-initialized and the GPON Mac is not activated.
 */
extern int32
rtk_gpon_device_deInitialize(void);

/* Function Name:
 *      rtk_gpon_evtHdlStateChange_reg
 * Description:
 *      This function is called to register the callback function of the State Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gpon_evtHdlStateChange_reg(rtk_gpon_eventHandleFunc_stateChange_t func);

/* Function Name:
 *      rtk_gpon_evtHdlDsFecChange_reg
 * Description:
 *      This function is called to register the callback function of the D/S FEC Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gpon_evtHdlDsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func);

/* Function Name:
 *      rtk_gpon_evtHdlUsFecChange_reg
 * Description:
 *      This function is called to register the callback function of the U/S FEC Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gpon_evtHdlUsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func);

/* Function Name:
 *      rtk_gpon_evtHdlUsPloamUrgEmpty_reg
 * Description:
 *      This function is called to register the callback function of the U/S PLOAM urgent queue is empty.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gpon_evtHdlUsPloamUrgEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func);

/* Function Name:
 *      rtk_gpon_evtHdlUsPloamNrmEmpty_reg
 * Description:
 *      This function is called to register the callback function of the U/S PLOAM normal queue is empty.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gpon_evtHdlUsPloamNrmEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func);

/* Function Name:
 *      rtk_gpon_evtHdlPloam_reg
 * Description:
 *      This function is called to register the event handler of the Rx PLOAM.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gpon_evtHdlPloam_reg(rtk_gpon_eventHandleFunc_ploam_t func);

/* Function Name:
 *      rtk_gpon_evtHdlOmci_reg
 * Description:
 *      This function is called to register the event handler of the Rx OMCI.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gpon_evtHdlOmci_reg(rtk_gpon_eventHandleFunc_omci_t func);

/* Function Name:
 *      rtk_gpon_callbackQueryAesKey_reg
 * Description:
 *      This function is called to register the callback function of the AES Key Query.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gpon_callbackQueryAesKey_reg(rtk_gpon_callbackFunc_queryAesKey_t func);

/* Function Name:
 *      rtk_gpon_evtHdlAlarm_reg
 * Description:
 *      This function is called to register the alarm event handler of the alarm.
 * Input:
 *      alarmType       - the alarm type
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gpon_evtHdlAlarm_reg(rtk_gpon_alarm_type_t alarmType, rtk_gpon_eventHandleFunc_fault_t func);

/* Function Name:
 *      rtk_gpon_serialNumber_set
 * Description:
 *      GPON MAC Set Serial Number.
 * Input:
 *      pSN             - the pointer of Serial Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
extern int32
rtk_gpon_serialNumber_set(rtk_gpon_serialNumber_t *pSN);

/* Function Name:
 *      rtk_gpon_serialNumber_get
 * Description:
 *      GPON MAC get Serial Number.
 * Input:
 *      None
 * Output:
 *      pSN             - the pointer of Serial Number
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
extern int32
rtk_gpon_serialNumber_get(rtk_gpon_serialNumber_t *pSN);

/* Function Name:
 *      rtk_gpon_password_set
 * Description:
 *      GPON MAC set Password.
 * Input:
 *      pPwd             - the pointer of Password
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
extern int32
rtk_gpon_password_set(rtk_gpon_password_t *pPwd);

/* Function Name:
 *      rtk_gpon_password_get
 * Description:
 *      GPON MAC get Password.
 * Input:
 *      None
 * Output:
 *      pPwd             - the pointer of Password
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
extern int32
rtk_gpon_password_get(rtk_gpon_password_t *pPwd);

/* Function Name:
 *      rtk_gpon_parameter_set
 * Description:
 *      GPON MAC set parameters.
 * Input:
 *      type            - the parameter type
 *      pPara           - the pointer of Parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
extern int32
rtk_gpon_parameter_set(rtk_gpon_patameter_type_t type, void *pPara);

/* Function Name:
 *      rtk_gpon_parameter_get
 * Description:
 *      GPON MAC get parameters, which is set by rtk_gpon_parameter_set.
 * Input:
 *      type            - the parameter type
 * Output:
 *      pPara           - the pointer of Parameter
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_parameter_get(rtk_gpon_patameter_type_t type, void *pPara);

/* Function Name:
 *      rtk_gpon_activate
 * Description:
 *      GPON MAC Activating.
 * Input:
 *      initState       - the initial state when ONU active
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The GPON MAC is working now.
 */
extern int32
rtk_gpon_activate(rtk_gpon_initialState_t initState);

/* Function Name:
 *      rtk_gpon_deActivate
 * Description:
 *      GPON MAC de-Activate.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The GPON MAC is out of work now.
 */
extern int32
rtk_gpon_deActivate(void);

/* Function Name:
 *      rtk_gpon_ponStatus_get
 * Description:
 *      GPON MAC Get PON Status.
 * Input:
 *      None
 * Output:
 *      pStatus         - pointer of status
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      If the device is not activated, an unknown status is returned.
 */
extern int32
rtk_gpon_ponStatus_get(rtk_gpon_fsm_status_t *pStatus);

/* Function Name:
 *      rtk_gpon_isr_entry
 * Description:
 *      GPON MAC ISR entry
 * Input:
*	  None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      It should be called in interrupt process or a polling thread
 */
extern void
rtk_gpon_isr_entry(void);

/* Function Name:
 *      rtk_gpon_tcont_create
 * Description:
 *      GPON MAC Create a TCont by assigning an alloc id.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      aAttr           - the pointer of tcont attribute(TCont id)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      A TCont ID is returned in pAttr.
 */
extern int32
rtk_gpon_tcont_create(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr);

/* Function Name:
 *      rtk_gpon_tcont_destroy
 * Description:
 *      GPON MAC Remove a TCont.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_tcont_destroy(rtk_gpon_tcont_ind_t *pInd);

/* Function Name:
 *      rtk_gpon_tcont_get
 * Description:
 *      GPON MAC Get a TCont with an alloc id.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      aAttr           - the pointer of tcont attribute(TCont id)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The TCont ID is returned in pAttr.
 */
extern int32
rtk_gpon_tcont_get(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr);

/* Function Name:
 *      rtk_gpon_dsFlow_set
 * Description:
 *      GPON MAC set a D/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_dsFlow_set(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr);

/* Function Name:
 *      rtk_gpon_dsFlow_get
 * Description:
 *      GPON MAC get a D/S flow.
 * Input:
 *      flowId          - the flow id
 * Output:
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_dsFlow_get(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr);

/* Function Name:
 *      rtk_gpon_usFlow_set
 * Description:
 *      GPON MAC set a U/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_usFlow_set(uint32 flowId, rtk_gpon_usFlow_attr_t *pAttr);

/* Function Name:
 *      rtk_gpon_usFlow_get
 * Description:
 *      GPON MAC get a U/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_usFlow_get(uint32 flowId, rtk_gpon_usFlow_attr_t *pAttr);

/* Function Name:
 *      rtk_gpon_ploam_send
 * Description:
 *      GPON MAC Send a PLOAM in upstream.
 * Input:
 *      urgent          - specify it is a urgent(1) or normal(0) PLOAM message
 *      pPloam          - the pointer of PLOAM message
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      A error is returned if the PLOAM is not sent.
 */
extern int32
rtk_gpon_ploam_send(int32 urgent, rtk_gpon_ploam_t *pPloam);

/* Function Name:
 *      rtk_gpon_broadcastPass_set
 * Description:
 *      GPON MAC set the broadcast pass mode.
 * Input:
 *      mode            - turn on(1) or off(0) the broadcast pass mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_broadcastPass_set(int32 mode);

/* Function Name:
 *      rtk_gpon_broadcastPass_get
 * Description:
 *      GPON MAC get the broadcast pass mode.
 * Input:
 *      pMode           - the pointer of broadcast pass mode: turn on(1) or off(0).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_broadcastPass_get(int32 *pMode);

/* Function Name:
 *      rtk_gpon_nonMcastPass_set
 * Description:
 *      GPON MAC set the non-multidcast pass mode.
 * Input:
 *      mode            - turn on(1) or off(0) the non-multidcast pass mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_nonMcastPass_set(int32 mode);

/* Function Name:
 *      rtk_gpon_nonMcastPass_get
 * Description:
 *      GPON MAC get the non-multidcast pass mode.
 * Input:
 *      pMode           - the pointer of non-multidcast pass mode: turn on(1) or off(0).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_nonMcastPass_get(int32 *pMode);

/* Function Name:
 *      rtk_gpon_multicastAddrCheck_set
 * Description:
 *      GPON MAC set the address pattern.
 * Input:
 *      ipv4_pattern    - Address pattern of DA[47:24] for IPv4 packets.
 *      ipv6_pattern    - Address pattern of DA[47:32] for IPv6 packets.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_multicastAddrCheck_set(uint32 ipv4_pattern, uint32 ipv6_pattern);

/* Function Name:
 *      rtk_gpon_multicastAddrCheck_get
 * Description:
 *      GPON MAC get the address pattern.
 * Input:
 *      pIpv4_pattern    - Address pattern of DA[47:24] for IPv4 packets..
 *      pIpv6_pattern    - Address pattern of DA[47:24] for IPv6 packets..
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_multicastAddrCheck_get(uint32 *pIpv4_Pattern, uint32 *pIpv6_Pattern);

/* Function Name:
 *      rtk_gpon_macFilterMode_set
 * Description:
 *      GPON MAC set the mac filter mode.
 * Input:
 *      mode            - MAC table filter mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_macFilterMode_set(rtk_gpon_macTable_exclude_mode_t mode);

/* Function Name:
 *      rtk_gpon_macFilterMode_get
 * Description:
 *      GPON MAC get the mac filter mode.
 * Input:
 *      pMode           - pointer of MAC filter table filter mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t *pMode);

/* Function Name:
 *      rtk_gpon_mcForceMode_set
 * Description:
 *      GPON MAC set the multicast force mode.
 * Input:
 *      ipv4            - IPv4 multicast force mode.
 *      ipv6            - IPv6 multicast force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_mcForceMode_set(rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6);

/* Function Name:
 *      rtk_gpon_mcForceMode_get
 * Description:
 *      GPON MAC get the multicast force mode.
 * Input:
 *      pIpv4           - The pointer of IPv4 multicast force mode.
 *      pIv6            - The pointer of IPv6 multicast force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_mcForceMode_get(rtk_gpon_mc_force_mode_t *pIpv4, rtk_gpon_mc_force_mode_t *pIpv6);

/* Function Name:
 *      rtk_gpon_macEntry_add
 * Description:
 *      GPON MAC Add a MAC entry by the MAC Address.
 * Input:
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_macEntry_add(rtk_gpon_macTable_entry_t *pEntry);

/* Function Name:
 *      rtk_gpon_macEntry_del
 * Description:
 *      GPON MAC Remove a MAC entry by the MAC Address.
 * Input:
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_macEntry_del(rtk_gpon_macTable_entry_t *pEntry);

/* Function Name:
 *      rtk_gpon_macEntry_get
 * Description:
 *      GPON MAC Get a MAC entry by the table index.
 * Input:
 *      index           - index of MAC filter table entry.
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t *pEntry);

/* Function Name:
 *      rtk_gpon_rdi_set
 * Description:
 *      GPON MAC set the RDI indicator in upstream.
 * Input:
 *      enable          - specify to turn on/off RDI.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_rdi_set(int32 enable);

/* Function Name:
 *      rtk_gpon_rdi_get
 * Description:
 *      GPON MAC get the RDI indicator in upstream.
 * Input:
 *      pEnable         - the pointer of RDI indicator.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_rdi_get(int32 *pEnable);

/* Function Name:
 *      rtk_gpon_powerLevel_set
 * Description:
 *      GPON MAC set ONU power level, it will update the TT field of
 *      Serial_Number_ONU PLOAMu message.
 * Input:
 *      level           - the power lever.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_powerLevel_set(uint8 level);

/* Function Name:
 *      rtk_gpon_powerLevel_set
 * Description:
 *      GPON MAC get ONU power level.
 * Input:
 *      pLevel          - the pointer of power lever.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_powerLevel_get(uint8 *pLevel);

/* Function Name:
 *      rtk_gpon_alarmStatus_get
 * Description:
 *      GPON MAC get the alarm status.
 * Input:
 *      alarm           - the alarm type.
 *      pStatus         - the pointer of alarm status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_alarmStatus_get(rtk_gpon_alarm_type_t alarm, int32 *pStatus);

/* Function Name:
 *      rtk_gpon_globalCounter_get
 * Description:
 *      GPON MAC get global performance counter.
 * Input:
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_globalCounter_get (rtk_gpon_global_performance_type_t type, rtk_gpon_global_counter_t *pPara);

/* Function Name:
 *      rtk_gpon_tcontCounter_get
 * Description:
 *      GPON MAC get Tcont performance counter.
 * Input:
 *      tcontId         - the TCont id
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_tcontCounter_get (uint32 tcontId, rtk_gpon_tcont_performance_type_t type, rtk_gpon_tcont_counter_t *pPara);

/* Function Name:
 *      rtk_gpon_flowCounter_get
 * Description:
 *      GPON MAC get Flow performance counter.
 * Input:
 *      flowId          - the flow id
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_flowCounter_get (uint32 flowId, rtk_gpon_flow_performance_type_t type, rtk_gpon_flow_counter_t *pPara);

/* Function Name:
 *      rtk_gpon_version_get
 * Description:
 *      GPON MAC get the version infomation for debug.
 * Input:
 *      pHver           - the pointer of Hardware versiotn
 *      pSver           - the pointer of Software versiotn
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_version_get(rtk_gpon_device_ver_t *pHver, rtk_gpon_driver_ver_t *pSver);

/* Function Name:
 *      rtk_gpon_txForceLaser_set
 * Description:
 *      GPON MAC set the Laser status.
 * Input:
 *      status          - specify to force turn on/off laser
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_txForceLaser_set(rtk_gpon_laser_status_t status);

/* Function Name:
 *      rtk_gpon_txForceLaser_get
 * Description:
 *      GPON MAC get the Laser status.
 * Input:
 *      pStatus         - pointer of force laser status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_txForceLaser_get(rtk_gpon_laser_status_t *pStatus);

/* Function Name:
 *      rtk_gpon_txForceIdle_set
 * Description:
 *      GPON MAC set to force insert the idle in upstream.
 * Input:
 *      pStatus         - specify to force send Idle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_txForceIdle_set(int32 on);

/* Function Name:
 *      rtk_gpon_txForceIdle_get
 * Description:
 *      GPON MAC get the status to force insert the idle in upstream.
 * Input:
 *      pStatus         - pointer of force Idle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_txForceIdle_get(int32 *pOn);

/* Function Name:
 *      rtk_gpon_dsFecSts_get
 * Description:
 *      GPON MAC get the status to FEC in downstream from Ident field.
 * Input:
 *      pStatus         - pointer of D/S FEC status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_dsFecSts_get(int32* pEn);

/* Function Name:
 *      rtk_gpon_version_show
 * Description:
 *      GPON MAC show version infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_version_show(void);

/* Function Name:
 *      rtk_gpon_devInfo_show
 * Description:
 *      GPON MAC show the whole driver infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_devInfo_show(void);

/* Function Name:
 *      rtk_gpon_gtc_show
 * Description:
 *      GPON MAC show the whole GTC infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_gtc_show(void);

/* Function Name:
 *      rtk_gpon_tcont_show
 * Description:
 *      GPON MAC show the TCont infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_tcont_show(uint32 tcont);

/* Function Name:
 *      rtk_gpon_dsFlow_show
 * Description:
 *      GPON MAC show the D/S flow infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_dsFlow_show(uint32 flow);

/* Function Name:
 *      rtk_gpon_usFlow_show
 * Description:
 *      GPON MAC show the U/S flow infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_usFlow_show(uint32 flow);

/* Function Name:
 *      rtk_gpon_macTable_show
 * Description:
 *      GPON MAC show Ethernet Mac Table in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_macTable_show(void);

/* Function Name:
 *      rtk_gpon_globalCounter_show
 * Description:
 *      GPON MAC show Global Counter in COM port.
 * Input:
 *      type        - counter type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_globalCounter_show(rtk_gpon_global_performance_type_t type);

/* Function Name:
 *      rtk_gpon_tcontCounter_show
 * Description:
 *      GPON MAC show TCont Counter in COM port.
 * Input:
 *      idx         - TCont index
 *      type        - counter type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_tcontCounter_show(uint32 idx, rtk_gpon_tcont_performance_type_t type);

/* Function Name:
 *      rtk_gpon_flowCounter_show
 * Description:
 *      GPON MAC show Flow Counter in COM port.
 * Input:
 *      idx         - Flow index
 *      type        - counter type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_flowCounter_show(uint32 idx, rtk_gpon_flow_performance_type_t type);

/* Function Name:
 *      rtk_gpon_omci_tx
 * Description:
 *      Transmit OMCI message.
 * Input:
 *      pOmci       - pointer of OMCI message data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_omci_tx(rtk_gpon_omci_msg_t *omci);

/* Function Name:
 *      rtk_gpon_omci_rx
 * Description:
 *      Receive OMCI message.
 * Input:
 *      pBuf        - pointer of received data
 *      len         - received data length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_omci_rx(uint8 *buf, uint32 len);

/* Function Name:
 *      rtk_gpon_test_set
 * Description:
 *      set GPON MAC test register
 * Input:
 *      data        - register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      For debug used in user space.
 */
extern int32
rtk_gpon_test_set(uint32 data);

/* Function Name:
 *      rtk_gpon_test_get
 * Description:
 *      get GPON MAC test register
 * Input:
 *      pData       - returned register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      For debug used in user space.
 */
extern int32
rtk_gpon_test_get(uint32 *pData);

/* Function Name:
 *      rtk_gpon_unit_test
 * Description:
 *      gpon unit test
 * Input:
 *      id          - test id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      For debug used in user space.
 */
extern int32
rtk_gpon_unit_test(uint32 id);

/* Function Name:
 *      rtk_gpon_initial
 * Description:
 *      GPON initial, including driver init, device init and irq register.
 * Input:
 *      data        - user data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      Only used in user space.
 */
extern int32
rtk_gpon_initial(uint32 data);

/* Function Name:
 *      rtk_gpon_deinitial
 * Description:
 *      GPON deinitial, including driver deinit, device deinit and irq deregister.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      Only used in user space.
 */
extern int32
rtk_gpon_deinitial(void);

/* Function Name:
 *      rtk_gpon_debug_set
 * Description:
 *      Turn on/off gpon debug print, for debug used.
 * Input:
 *      enable          - turn on/off debug print
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      Only used in user space.
 */
extern int32
rtk_gpon_debug_set(int32 enable, uint32 level);

/* Function Name:
 *      rtk_gpon_autoTcont_set
 * Description:
 *      enable/disable tcont auto learning function
 * Input:
 *      state       - enable(1)/disable(0) state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It is used for debug in user space.
 */
extern int32
rtk_gpon_autoTcont_set(int32 state);

/* Function Name:
 *      rtk_gpon_autoTcont_get
 * Description:
 *      get tcont auto learning state
 * Input:
 *      pState      - enable(1)/disable(0) state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It is used for debug in user space.
 */
extern int32
rtk_gpon_autoTcont_get(int32 *pState);

/* Function Name:
 *      rtk_gpon_autoBoh_set
 * Description:
 *      enable/disable BOH auto configure function
 * Input:
 *      state       - enable(1)/disable(0) state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It is used for debug in user space.
 */
extern int32
rtk_gpon_autoBoh_set(int32 state);

/* Function Name:
 *      rtk_gpon_autoBoh_get
 * Description:
 *      get BOH auto configure state
 * Input:
 *      pState      - enable(1)/disable(0) state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It is used for debug in user space.
 */
extern int32
rtk_gpon_autoBoh_get(int32 *pState);

/* Function Name:
 *      rtk_gpon_eqdOffset_set
 * Description:
 *      configure eqd offset
 * Input:
 *      offset      - offset value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It is used for debug in user space.
 */
extern int32
rtk_gpon_eqdOffset_set(int32 offset);

/* Function Name:
 *      rtk_gpon_eqdOffset_get
 * Description:
 *      get eqd offset
 * Input:
 *      pOffset     - offset value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It is used for debug in user space.
 */
extern int32
rtk_gpon_eqdOffset_get(int32 *pOffset);

/* Function Name:
 *      rtk_gpon_autoDisTx_set
 * Description:
 *      Enable or disable GPON auto TX_DIS function.
 * Input:
 *      state           - enable or disable TX_DIS.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gpon_autoDisTx_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_rogueOnt_set
 * Description:
 *      Config Rogue ONT
 * Input:
 *	   rogOntState - enable or disable rogue ont
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 
rtk_gpon_rogueOnt_set(rtk_enable_t rogOntState);

#if defined(OLD_FPGA_DEFINED)
/* Function Name:
 *      rtk_gpon_pktGen_cfg_set
 * Description:
 *      packet generator configure.
 * Input:
 *      item        - item number
 *      tcont       - tcont id
 *      len         - packet length
 *      gem         - gem port
 *      omci        - omci flag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      Only for debug used
 */
extern int32
rtk_gpon_pktGen_cfg_set(uint32 item, uint32 tcont, uint32 len, uint32 gem, int32 omci);

/* Function Name:
 *      rtk_gpon_pktGen_buf_set
 * Description:
 *      packet generator buffer.
 * Input:
 *      item        - item number
 *      buf         - packet buffer
 *      len         - packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      Only for debug used
 */
extern int32
rtk_gpon_pktGen_buf_set(uint32 item, uint8 *buf, uint32 len);
#endif


extern void  gpon_dbg_enable(int32 enable, uint32 level);
#endif  /* __RTK_GPON_H__ */

