/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision$
 * $Date: 2012-08-07
 *
 * Purpose : GPON MAC register access APIs
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */
#ifndef _GPON_V2_H_
#define _GPON_V2_H_

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/port.h>


#define HW_OLT_DISTANCE_ACCURATE        0

#define GPON_REG_STATUS_CLEAR           0
#define GPON_REG_STATUS_SET             1
#define GPON_REG_16BITS_LOW             0
#define GPON_REG_16BITS_HIGH            8

#define GPON_REG_OPERRATION_NO          0
#define GPON_REG_OPERRATION_WRITE       1
#define GPON_REG_OPERRATION_READ        2
#define GPON_REG_OPERRATION_CLEAN       3

typedef enum rtk_gpon_dsGtc_pmMiscType_e{
    GPON_REG_DSGTCPMMISC_BIP_BLOCK_ERR = 0,
    GPON_REG_DSGTCPMMISC_BIP_BITS_ERR,
    GPON_REG_DSGTCPMMISC_FEC_CORRECTED_BITS,
    GPON_REG_DSGTCPMMISC_FEC_CORRECTED_BYTES,
    /* Rename from XXX_BLOCKS to XXX_CWS in GPON_MAC_SWIO_r1.1 */
    GPON_REG_DSGTCPMMISC_FEC_CORRETED_CWS,
    GPON_REG_DSGTCPMMISC_FEC_UNCOR_CWS,
    GPON_REG_DSGTCPMMISC_LOM,
    GPON_REG_DSGTCPMMISC_PLOAM_ACCEPT,
    GPON_REG_DSGTCPMMISC_PLOAM_FAIL,
    GPON_REG_DSGTCPMMISC_BWMAP_FAIL,
    GPON_REG_DSGTCPMMISC_BWMAP_INV,
    GPON_REG_DSGTCPMMISC_ACTIVE,
    GPON_REG_DSGTCPMMISC_BWMAP_ACPT,
    GPON_REG_DSGTCPMMISC_GEM_LOS,
    GPON_REG_DSGTCPMMISC_HEC_COR,
    GPON_REG_DSGTCPMMISC_GEM_IDLE,
    GPON_REG_DSGTCPMMISC_GEM_FAIL,
    GPON_REG_DSGTCPMMISC_RX_GEM_NON_IDLE,
    GPON_REG_DSGTCPMMISC_PLEND_CORRECTIONS,
    GPON_REG_DSGTCPMMISC_END,

}rtk_gpon_dsGtc_pmMiscType_t;

typedef enum rtk_gpon_dsGem_pmMiscType_e{
    GPON_REG_DSGEMPMMISC_MULTICAST_RX = 0,
    GPON_REG_DSGEMPMMISC_UNICAST_RX = 1,
    GPON_REG_DSGEMPMMISC_MULTICAST_FWD = 2,
    GPON_REG_DSGEMPMMISC_MULTICAST_LEAK = 3,
    GPON_REG_DSGEMPMMISC_ETH_CRC_ERR = 4,
    GPON_REG_DSGEMPMMISC_OVER_INTERLEAV = 5,
    GPON_REG_DSGEMPMMISC_OMCI_RX = 6,
    GPON_REG_DSGEMPMMISC_END,
}rtk_gpon_dsGem_pmMiscType_t;

typedef enum rtk_gpon_usGtc_pmMiscType_e{
    GPON_REG_USGTCPMMISC_PLOAM_BOH_TX = 0,
    GPON_REG_USGTCPMMISC_GEM_DBRU_TX = 1,
    GPON_REG_USGTCPMMISC_PLOAM_CPU_TX = 2,
    GPON_REG_USGTCPMMISC_PLOAM_AUTO_TX = 3,
    GPON_REG_USGTCPMMISC_GEM_BYTE_TX = 4,
    GPON_REG_USGTCPMMISC_END,
}rtk_gpon_usGtc_pmMiscType_t;




typedef enum rtk_gpon_intrType_e{
    GPON_INTR_GEM_US=0,
    GPON_INTR_GTC_US,
    GPON_INTR_GEM_DS,
    GPON_INTR_AES_DECRYPT,
    GPON_INTR_GTC_DS_CAP,
    GPON_INTR_GTC_DS,
    GPON_INTR_ALL,
    GPON_INTR_END
}rtk_gpon_intrType_t;


typedef enum rtk_gpon_gtcDsIntrType_e{
    GPON_GTC_DS_INTR=0,
    GPON_PLM_BUF_REQ,
    GPON_RNG_REQ_HIS,
    GPON_SN_REQ_HIS,
    GPON_LOM_DLT,
    GPON_DS_FEC_STS_DLT,
    GPON_LOF_DLT,
    GPON_LOS_DLT,
    GPON_PPS_DLT,
    GPON_GTC_DS_INTR_ALL,
    GPON_GTC_DS_INTR_END
}rtk_gpon_gtcDsIntrType_t;


typedef enum rtk_gpon_onuState_e{
    GPON_STATE_UNKNOWN=0,
    GPON_STATE_O1,
    GPON_STATE_O2,
    GPON_STATE_O3,
    GPON_STATE_O4,
    GPON_STATE_O5,
    GPON_STATE_O6,
    GPON_STATE_O7,
    GPON_STATE_END
}rtk_gpon_onuState_t;


typedef enum rtk_gpon_gtcUsIntrType_e{
    GPON_GTC_US_INTR=0,
    GPON_OPTIC_SD_MISM,
    GPON_OPTIC_SD_TOOLONG,
    GPON_PLM_URG_EMPTY,
    GPON_PLM_NRM_EMPTY,
    GPON_US_FEC_STS,
    GPON_DG_MSG_TX,
    GPON_GTC_US_INTR_ALL,
    GPON_GTC_US_INTR_END
}rtk_gpon_gtcUsIntrType_t;


typedef enum rtk_gpon_gemUsIntrType_e{
    GPON_GEM_US_INTR=0,
    GPON_SD_VALID_LONG,
    GPON_SD_DIFF_HUGE,
    GPON_REQUEST_DELAY,
    GPON_BC_LESS6,
    GPON_ERR_PLI,
    GPON_BURST_TM_LARGER_GTC,
    GPON_BANK_TOO_MUCH_AT_END,
    GPON_BANK_REMAIN_AFRD,
    GPON_BANK_OVERFL,
    GPON_BANK_UNDERFL,
    GPON_GEM_US_INTR_ALL,
    GPON_GEM_US_INTR_END
}rtk_gpon_gemUsIntrType_t;


/*
 * GPON MAC Laser Status
 */
typedef enum rtk_gpon_laser_status_e{
    RTK_GPON_LASER_STATUS_NORMAL = 0,
    RTK_GPON_LASER_STATUS_FORCE_ON = 1,
    RTK_GPON_LASER_STATUS_FORCE_OFF = 2,
    RTK_GPON_LASER_STATUS_END
}rtk_gpon_laser_status_t;


typedef enum rtk_gpon_prbs_e{
    RTK_GPON_PRBS_OFF,
    RTK_GPON_PRBS_31,
    RTK_GPON_PRBS_23,
    RTK_GPON_PRBS_15,
    RTK_GPON_PRBS_7,
    RTK_GPON_PRBS_END
}rtk_gpon_prbs_t;


typedef enum rtk_gpon_polarity_e{
    GPON_POLARITY_LOW=0,
    GPON_POLARITY_HIGH,
    GPON_POLARITY_END
}rtk_gpon_polarity_t;


typedef enum rtk_gpon_usAutoPloamType_e{
    GPON_REG_USAUTOPLOAM_NOMSG = 0,
    GPON_REG_USAUTOPLOAM_SN = 1,
    GPON_REG_USAUTOPLOAM_DYINGGASP = 2,
    GPON_REG_USAUTOPOLAM_END
}rtk_gpon_usAutoPloamType_t;


/*
 * GPON MAC MACAddress Table Operation Mode
 */
typedef enum rtk_gpon_macTable_exclude_mode_e{
    RTK_GPON_MACTBL_MODE_INCLUDE = 0,
    RTK_GPON_MACTBL_MODE_EXCLUDE,
    RTK_GPON_MACTBL_MODE_END
}rtk_gpon_macTable_exclude_mode_t;

/*
 * GPON MAC MACAddress filter Operation Mode
 */
typedef enum rtk_gpon_mc_force_mode_e{
    RTK_GPON_MCFORCE_MODE_NORMAL = 0,
    RTK_GPON_MCFORCE_MODE_PASS,
    RTK_GPON_MCFORCE_MODE_DROP,
    RTK_GPON_MCFORCE_MODE_END
}rtk_gpon_mc_force_mode_t;

typedef enum rtk_gpon_ipVer_e{
    GPON_IP_VER_V4=0,
    GPON_IP_VER_V6,
    GPON_IP_VER_END
}rtk_gpon_ipVer_t;


typedef struct rtk_gem_cfg_s{
    uint32       gemPortId;
    rtk_enable_t isMulticast;
    rtk_enable_t isEthernet;
    rtk_enable_t isOMCI;
    rtk_enable_t enAES;
}rtk_gem_cfg_t;


typedef enum rtk_gpon_tx_ptn_mode_e{
    RTK_GPON_TX_PTN_MODE_NORMAL = 0,
    RTK_GPON_TX_PTN_MODE_PRBS = 1,
    RTK_GPON_TX_PTN_MODE_CUSTOM = 2,
    RTK_GPON_TX_PTN_MODE_NONE = 3

}rtk_gpon_tx_ptn_mode_t;


typedef struct rtk_gpon_schedule_info_s{
    uint32      omcc_tcont;
    uint32      omcc_queue;
    uint32      omcc_flow;
    uint32      max_tcont;
    uint32      max_tcon_queue;
    uint32      max_flow;
    uint32      max_pon_queue;
}rtk_gpon_schedule_info_t;

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

typedef struct rtk_gpon_ds_omci_s{
    uint32 rx_omci;                     /* Received OMCI Counter */
    uint32 rx_omci_crc_err;             /* Received OMCI CRC Error Counter */
    uint32 rx_omci_drop;                /* Received OMCI Dropped Counter */
    uint32 rx_omci_byte;                /* Received OMCI byte Counter */
}rtk_gpon_ds_omci_t;

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
    uint32 rx_plen_correct;             /* Received PLen corrected Counter */
    uint32 rx_fec_total_block;          /* Received FEC total block Counter */
    uint32 rx_fec_total_cw;          /* Received FEC total Codeword Counter */
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
    uint16 rx_ploam_unknown;            /* Received PLOAM unknown Counter */
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
#ifdef CONFIG_10G_GPON_FEATURE
#define RTK_GPON_PASSWORD_LEN            36
#else
#define RTK_GPON_PASSWORD_LEN            10
#endif

typedef struct rtk_gpon_password_s{
    uint8 password[RTK_GPON_PASSWORD_LEN];
}rtk_gpon_password_t;

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
typedef rtk_gpon_onuState_t rtk_gpon_fsm_status_t;
#if 0 /* for coverity check */
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
#endif
/*
 * GPON MAC Alarm Type
 */
typedef enum rtk_gpon_alarm_type_e{
    RTK_GPON_ALARM_NONE = 0,
    RTK_GPON_ALARM_LOS = 1,
    RTK_GPON_ALARM_LOF = 2,
    RTK_GPON_ALARM_LOM = 3,
    RTK_GPON_ALARM_SF,
    RTK_GPON_ALARM_SD,
    RTK_GPON_ALARM_SD_TOOLONG,
    RTK_GPON_ALARM_SD_MISMATCH,
    RTK_GPON_ALARM_MAX
} rtk_gpon_alarm_type_t;

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

typedef struct rtk_gpon_us_omci_s{
    uint32 tx_omci;                     /* Transmit OMCI Counter */
    uint32 tx_omci_byte;                /* Transmit OMCI byte Counter */
}rtk_gpon_us_omci_t;

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
    RTK_GPON_PARA_TYPE_US_PHY,
    RTK_GPON_PARA_TYPE_US_PLOAM,
    RTK_GPON_PARA_TYPE_US_DBR,
    RTK_GPON_PARA_TYPE_ONUID,
	RTK_GPON_PARA_TYPE_DRV_STATUS,
    RTK_GPON_PARA_TYPE_MAX
}rtk_gpon_patameter_type_t;


typedef enum gpon_rgIgmp_ioctl_e
{
    GPON_RGIGMP_IOCTL_INIT_PARAM_SET,
    GPON_RGIGMP_IOCTL_PROFILE_PER_PORT_ADD,
    GPON_RGIGMP_IOCTL_PROFILE_PER_PORT_DEL,
    GPON_RGIGMP_IOCTL_PROFILE_ADD,
    GPON_RGIGMP_IOCTL_PROFILE_DEL,
    GPON_RGIGMP_IOCTL_FAST_LEAVE_SET,
    GPON_RGIGMP_IOCTL_US_TAG_SET,
    //GPON_RGIGMP_IOCTL_US_IGMP_RATE_SET,
    GPON_RGIGMP_IOCTL_DYNAMIC_ACL_SET,
    GPON_RGIGMP_IOCTL_DYNAMIC_ACL_DEL,
    //GPON_RGIGMP_IOCTL_STATIC_ACL_SET,
    //GPON_RGIGMP_IOCTL_STATIC_ACL_DEL,
    //GPON_RGIGMP_IOCTL_ROBUSTNESS_SET,
    //GPON_RGIGMP_IOCTL_QUERIER_IPADDR_SET,
    //GPON_RGIGMP_IOCTL_QUERY_INTERVAL_SET,
    //GPON_RGIGMP_IOCTL_QUERY_MAX_RESPONSE_TIME_SET,
    GPON_RGIGMP_IOCTL_LAST_MBR_QUERY_INTERVAL_SET,
    GPON_RGIGMP_IOCTL_UNAUTHORIZED_JOIN_BEHAVIOR_SET,
    GPON_RGIGMP_IOCTL_DS_TAG_SET,
    GPON_RGIGMP_IOCTL_MAX_GRP_NUM_SET,
    GPON_RGIGMP_IOCTL_IGMP_VERSION_SET,
    GPON_RGIGMP_IOCTL_IGMP_FUNCTION_SET,
    GPON_RGIGMP_IOCTL_END
}gpon_rgIgmp_ioctl_t;

typedef struct gpon_rgIgmp_ipAddr_s
{
    uint32 isIpv6B;
    union
    {
        rtk_ip_addr_t ipv4Addr;
        rtk_ipv6_addr_t ipv6Addr;
    } ipAddr;
}gpon_rgIgmp_ipAddr_t;

typedef struct gpon_rgIgmp_dynamic_acl_entry_s
{
    uint32 entry_id;
    //uint32 imputed_group_bw;
    uint16 preview_length;
    uint16 preview_repeat_time;
    uint16 preview_repeat_count;
    uint16 preview_reset_time;
    //uint32 vendor_specific_use;
    gpon_rgIgmp_ipAddr_t dip_start_range;
    gpon_rgIgmp_ipAddr_t dip_end_range;
}gpon_rgIgmp_dynamic_acl_entry_t;

typedef enum gpon_rgIgmp_tag_ctrl_e
{
    GPON_RGIGMP_TAG_CTRL_PASS,
    GPON_RGIGMP_TAG_CTRL_ADD_VLAN,
    GPON_RGIGMP_TAG_CTRL_REPLACE_VLAN,
    GPON_RGIGMP_TAG_CTRL_REPLACE_VID,
    GPON_RGIGMP_TAG_CTRL_STRIP_OUT_VLAN
}gpon_rgIgmp_tag_ctrl_t;

typedef enum gpon_rgIgmp_version_e
{
    GPON_RGIGMP_VERSION_1,
    GPON_RGIGMP_VERSION_2,
    GPON_RGIGMP_VERSION_3
}gpon_rgIgmp_version_t;

typedef enum gpon_rgIgmp_function_e
{
    GPON_RGIGMP_FUNCTION_SNOOPING,      // multicast control mode = snooping only
    GPON_RGIGMP_FUNCTION_SPR,           // multicast control mode = snooping with proxy reporting
    GPON_RGIGMP_FUNCTION_PROXY          // multicast control mode = proxy
}gpon_rgIgmp_function_t;

typedef struct gpon_rgIgmp_cfg_s
{
    uint32 uni_id;
    uint32 profile_id;
    uint32 fastLeaveB;
    gpon_rgIgmp_tag_ctrl_t us_igmp_tag_ctrl;
    uint32 us_igmp_tci;
    gpon_rgIgmp_tag_ctrl_t ds_igmp_mcast_tag_ctrl;
    uint32 ds_igmp_mcast_tci;
    gpon_rgIgmp_dynamic_acl_entry_t dy_acl;
    uint32 last_mbr_query_interval;
    uint32 unautheroizedB;
    uint32 max_group_number;
    gpon_rgIgmp_version_t version;
    gpon_rgIgmp_function_t function;
}gpon_rgIgmp_cfg_t;

typedef struct rtk_gpon_rgIgmp_cfg_msg_s
{
    gpon_rgIgmp_ioctl_t opt_id;
    gpon_rgIgmp_cfg_t igmp_cfg;
}rtk_gpon_rgIgmp_cfg_msg_t;

typedef struct rtk_gpon_sig_para_s{
    rtk_gpon_alarm_type_t   type;
    uint32                  threshold;
}rtk_gpon_sig_para_t;

/* for GPON DS ploam buffer software */
#define GPON_DS_PLOAM_BUF_MAX   128
#define GPON_PLOAM_LEN          13
typedef struct gpon_ds_ploam_buf_s{
    uint32          head;
    uint32          tail;
    uint8           ploam_array[GPON_DS_PLOAM_BUF_MAX][GPON_PLOAM_LEN];
}gpon_ds_ploam_buf_t;

typedef enum rtk_gpon_prbs_rx_state_e{
    RTK_GPON_PRBS_RX_READY=0,
    RTK_GPON_PRBS_RX_LOS=1,
    RTK_GPON_PRBS_RX_NOTAVAILABLE=2,
    RTK_GPON_PRBS_RX_END
}rtk_gpon_prbs_rx_state_t;

typedef struct rtk_gpon_prbsRxStatus_s {
    uint32 prbsRxState;
    uint32 prbsRxErrCnt;
    uint32 prbsRxErrRate;
} rtk_gpon_prbsRxStatus_t;

/* Function Name:
 *      rtk_gpon_init
 * Description:
 *      gpon register level initial function
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_init(void);



/* Function Name:
 *      rtk_gpon_resetState_set
 * Description:
 *      reset gpon register
 * Input:
 *    state: enable for reset gpon register
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_resetState_set(rtk_enable_t state);


/* Function Name:
 *      rtk_gpon_resetDoneState_get
 * Description:
 *      get the reset status
 * Input:
 *
 * Output:
 *      pState: point of reset status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_resetDoneState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_macVersion_get
 * Description:
 *      Read the gpon version
 * Input:
 *
 * Output:
 *      pVersion: point for get gpon version
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_macVersion_get(uint32 *pVersion);

/* Function Name:
 *      rtk_gpon_test_get
 * Description:
 *      For test get gpon test data
 * Input:
 *
 * Output:
 *      pTestData: point for get test data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_test_get(uint32 *pTestData);

/* Function Name:
 *      rtk_gpon_test_set
 * Description:
 *      For test set gpon test data
 * Input:
 *      testData: assign test data
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_test_set(uint32 testData);

/* Function Name:
 *      rtk_gpon_topIntrMask_get
 * Description:
 *      Get GPON Top level interrupt mask
 * Input:
 *    topIntrType: type of top interrupt
 * Output:
 *      pState: point of get interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_topIntrMask_get(rtk_gpon_intrType_t topIntrType,rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_topIntrMask_set
 * Description:
 *      Set GPON Top level interrupt mask
 * Input:
 *    topIntrType: type of top interrupt
 *      state: set interrupt mask state
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_topIntrMask_set(rtk_gpon_intrType_t topIntrType,rtk_enable_t state);




/* Function Name:
 *      rtk_gpon_topIntr_get
 * Description:
 *      Set GPON Top level interrupt state
 * Input:
 *    topIntrType: type of top interrupt
 * Output:
 *      pState: point for get  interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_topIntr_get(rtk_gpon_intrType_t topIntrType,rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_topIntr_disableAll
 * Description:
 *      Disable all of top interrupt for GPON
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_topIntr_disableAll(void);

/*

page 0x01 */

/* Function Name:
 *      rtk_gpon_gtcDsIntr_get
 * Description:
 *      Get GTC DS interrupt
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_gtcDsIntr_get(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_gtcDsIntrDlt_get
 * Description:
 *      Get GTC DS interrupt indicator
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_gtcDsIntrDlt_get(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_gtcDsIntrMask_get
 * Description:
 *      Get GTC DS Interrupt Mask state
 * Input:
 *       gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  mask state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_gtcDsIntrMask_get(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t  *pState);

/* Function Name:
 *      rtk_gpon_gtcDsIntrMask_set
 * Description:
 *      Set GTC DS Interrupt Mask state
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 *       state: set gtc interrupt  mask state
 * Output:
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_gtcDsIntrMask_set(rtk_gpon_gtcDsIntrType_t gtcIntrType,rtk_enable_t  state);


/* Function Name:
 *      rtk_gpon_onuId_set
 * Description:
 *      Set GPON ONU ID
 * Input:
 *      onuId: value of ONUID
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_onuId_set(uint8 onuId);

/* Function Name:
 *      rtk_gpon_onuId_set
 * Description:
 *      Set GPON ONU ID
 * Input:
 *      onuId: value of ONUID
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_onuId_get(uint8 *pOnuId);

/* Function Name:
 *      rtk_gpon_onuState_set
 * Description:
 *      Set ONU State .
 * Input:
 *      onuState: onu state
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_onuState_set(rtk_gpon_onuState_t  onuState);

/* Function Name:
 *      rtk_gpon_onuState_get
 * Description:
 *      Get ONU State
 * Input:
 *
 * Output:
 *      pOnuState: point for get onu state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_onuState_get(rtk_gpon_onuState_t  *pOnuState);

/* Function Name:
 *      rtk_gpon_dsBwmapCrcCheckState_set
 * Description:
 *      Set DS Bandwidth map CRC check enable
 * Input:
 *      state: enable or disable DS Bandwidth map CRC check
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED     - Failed
 * Note:
 */
extern int32 rtk_gpon_dsBwmapCrcCheckState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_dsBwmapCrcCheckState_get
 * Description:
 *      Get DS Bandwidth map CRC check enable
 * Input:
 *
 * Output:
 *      pState: point for get enable or disable of DS Bandwidth map CRC check
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsBwmapCrcCheckState_get(rtk_enable_t *pState);

/* Added in GPON_MAC_SWIO_v1.1 */

/* Function Name:
 *      rtk_gpon_dsBwmapFilterOnuIdState_set
 * Description:
 *      Set DS bandwidth map filter oun state
 * Input:
 *      state: state of DS bandwidth map filter oun
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsBwmapFilterOnuIdState_set(rtk_enable_t state);


/* Function Name:
 *      rtk_gpon_dsBwmapFilterOnuIdState_get
 * Description:
 *      Get DS bandwidth map filter oun state
 * Input:
 *
 * Output:
 *      pState: point for get DS bandwidth map filter oun state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
 extern int32 rtk_gpon_dsBwmapFilterOnuIdState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_dsPlendStrictMode_set
 * Description:
 *      Set DS Plen Strict Mode
 * Input:
 *      state: state of DS Plen Strict Mode
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPlendStrictMode_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_dsPlendStrictMode_get
 * Description:
 *      Get DS Plen Strict Mode
 * Input:
 *
 * Output:
 *      pState: point of state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPlendStrictMode_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_dsScrambleState_set
 * Description:
 *      Set DS scramble
 * Input:
 *      state: state of DS scramble
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsScrambleState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_dsScrambleState_get
 * Description:
 *      Get DS scramble
 * Input:
 *
 * Output:
 *        pState: state of DS scramble
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsScrambleState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_dsFecBypass_set
 * Description:
 *      Set DS FEC bypass
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsFecBypass_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_dsFecBypass_get
 * Description:
 *      Get DS FEC bypass
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsFecBypass_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_dsFecThrd_set
 * Description:
 *      Set DS Fec threshhold
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsFecThrd_set(uint8 fecThrdValue);

/* Function Name:
 *      rtk_gpon_dsFecThrd_get
 * Description:
 *      Get DS Fec threshhold
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsFecThrd_get(uint8 *pFecThrdValue);

/* Function Name:
 *      rtk_gpon_extraSnTxTimes_set
 * Description:
 *      Set extra serial number tx times
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_extraSnTxTimes_set(uint8 exSnTxTimes);


/* Function Name:
 *      rtk_gpon_extraSnTxTimes_get
 * Description:
 *      Get extra serial number tx times
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_extraSnTxTimes_get(uint8 *pExSnTxTimes);

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Set DS PLOAM no message
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPloamNomsg_set(uint8 ploamNoMsgValue);


/* Function Name:
 *      rtk_gpon_dsPloamNomsg_get
 * Description:
 *      Get DS PLOAM no message
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPloamNomsg_get(uint8 *pPloamNoMsgValue);

/* Function Name:
 *      rtk_gpon_dsPloamOnuIdFilterState_set
 * Description:
 *      Set DS PLOAM ONU ID Filter state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPloamOnuIdFilterState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_dsPloamOnuIdFilterState_get
 * Description:
 *      Get DS PLOAM ONUID Filter state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPloamOnuIdFilterState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_dsPloamBcAcceptState_set
 * Description:
 *      Set DS PLAOM Boardcast accept state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPloamBcAcceptState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_dsPloamBcAcceptState_get
 * Description:
 *    Get DS PLAOM Boardcast accept state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPloamBcAcceptState_get(rtk_enable_t *pState);

/* Added in GPON_MAC_SWIO_v1.1 */


/* Function Name:
 *      rtk_gpon_dsPloamDropCrcState_set
 * Description:
 *      Set DS PLOAM Drop crc state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPloamDropCrcState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_dsPloamDropCrcState_get
 * Description:
 *      Get DS PLOAM Drop crc state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPloamDropCrcState_get(rtk_enable_t *pState);




/* Function Name:
 *      rtk_gpon_cdrLosStatus_get
 * Description:
 *      Get CDR LOS status
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_cdrLosStatus_get(rtk_enable_t *pState);




/* Function Name:
 *      rtk_gpon_optLosStatus_get
 * Description:
 *      GET OPT LOS Status
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_optLosStatus_get(rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Set LOS configuration
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_losCfg_set(rtk_enable_t opten, int32 optpolar, rtk_enable_t cdren, int32 cdrpolar, rtk_enable_t filter);

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Get LOS configuration
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_losCfg_get(int32 *opten, int32 *optpolar, int32 *cdren, int32 *cdrpolar, int32 *filter);

/* Function Name:
 *      rtk_gpon_dsPloam_get
 * Description:
 *      Get DS PLOAM data
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPloam_get(uint8 *pPloamData);

/* Function Name:
 *      rtk_gpon_dsPloam_getAll
 * Description:
 *      Get all DS PLOAM data from HW and store to SW buf
 * Input:
 *
 * Output:
 *      pPloambuf - ds ploam SW buf
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsPloam_getAll(gpon_ds_ploam_buf_t *pPloambuf);

/* Function Name:
 *      rtk_gpon_usTcont_get
 * Description:
 *      Get Tcont allocate Id
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usTcont_get(uint32 tcontId, uint32 *pAllocateId);

/* Function Name:
 *      rtk_gpon_usTcont_set
 * Description:
 *      Set TCONT allocate Id
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usTcont_set(uint32 tcontId, uint32 allocateId);

/* Function Name:
 *      rtk_gpon_usTcont_del
 * Description:
 *      Delete TCONT
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usTcont_del(uint32 tcontId);

/* Function Name:
 *      rtk_gpon_dsGemPort_get
 * Description:
 *      Get DS gem port configure
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPort_get(uint32 idx, rtk_gem_cfg_t *pGemCfg);

/* Function Name:
 *      rtk_gpon_dsGemPort_set
 * Description:
 *      Set DS gem port configure
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPort_set(uint32 idx, rtk_gem_cfg_t gemCfg);

/* Function Name:
 *      rtk_gpon_dsGemPort_del
 * Description:
 *      Delete DS Gem pot
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPort_del(uint32 idx);

/* Function Name:
 *      rtk_gpon_dsGemPortPktCnt_get
 * Description:
 *      Get DS Gem port packet counter
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortPktCnt_get(uint32 idx, uint32 *pktCnt);


/* Function Name:
 *      rtk_gpon_dsGemPortByteCnt_get
 * Description:
 *      Get DS Gem port byte counter
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortByteCnt_get(uint32 idx, uint32 *byteCnt);


/* Function Name:
 *      rtk_gpon_dsGemPortByteCnt_get
 * Description:
 *      Get DS Gem port misc counter
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGtcMiscCnt_get(rtk_gpon_dsGtc_pmMiscType_t dsGtcPmMiscType, uint32 *miscCnt);


/* Function Name:
 *      rtk_gpon_dsOmciPti_set
 * Description:
 *      Set DS OMCI PTI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsOmciPti_set(uint32 mask, uint32 end);


/* Function Name:
 *      rtk_gpon_dsOmciPti_get
 * Description:
 *      Get DS OMCI PTI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsOmciPti_get(uint32 *pMask, uint32 *pEnd);



/* Function Name:
 *      rtk_gpon_dsEthPti_set
 * Description:
 *      Set DS Ethernet PTI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsEthPti_set(uint32 mask, uint32 end);

/* Function Name:
 *      rtk_gpon_dsEthPti_get
 * Description:
 *      Get DS Ethernet PTI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dsEthPti_get(uint32 *pMask, uint32 *pEnd);


/* Function Name:
 *      rtk_gpon_aesKeySwitch_set
 * Description:
 *      Set AES key switch value(superframe value)
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_aesKeySwitch_set(uint32 superframe);



/* Function Name:
 *      rtk_gpon_aesKeySwitch_get
 * Description:
 *      Get AES key switch value(superframe value)
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_aesKeySwitch_get(uint32 *pSuperframe);


/* Function Name:
 *      rtk_gpon_aesKeyWord_set
 * Description:
 *      Set AES keyword value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_aesKeyWord_set(uint8 *keyword);


/* Function Name:
 *      rtk_gpon_aesKeyWordActive_set
 * Description:
 *      Set AES keyword value for active key, this api is only set by initial.
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_aesKeyWordActive_set(uint8 *keyword);


/* Function Name:
 *      rtk_gpon_irq_get
 * Description:
 *      Get global irq status
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_irq_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_dsGemPortEthRxCnt_get
 * Description:
 *      Get DS Gemport Ethernet RX counter
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortEthRxCnt_get(uint32 idx,uint32 *pEthRxCnt);


/* Function Name:
 *      rtk_gpon_dsGemPortEthFwdCnt_get
 * Description:
 *      Get GPON DS Gemport Ethernet Forward counter
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortEthFwdCnt_get(uint32 idx,uint32 *pEthFwdCnt);


/* Function Name:
 *      rtk_gpon_dsGemPortMiscCnt_get
 * Description:
 *      Get GPON DS Gemport MISC counter
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortMiscCnt_get(rtk_gpon_dsGem_pmMiscType_t idx,uint32 *pMiscCnt);


/* Function Name:
 *      rtk_gpon_dsGemPortFcsCheckState_get
 * Description:
 *      Get GPON DS Gemport FCS check state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortFcsCheckState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_dsGemPortFcsCheckState_set
 * Description:
 *      Get GPON DS Gemport FCS check state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortFcsCheckState_set(rtk_enable_t state);


/* Function Name:
 *      rtk_gpon_dsGemPortBcPassState_set
 * Description:
 *      Set GPON DS Gemport Boardcast pass state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortBcPassState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_dsGemPortBcPassState_get
 * Description:
 *      Get GPON DS Gemport Boardcast pass state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortBcPassState_get(rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_dsGemPortNonMcPassState_set
 * Description:
 *      Set GPON DS Gemport None Multicast pass state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortNonMcPassState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_dsGemPortNonMcPassState_get
 * Description:
 *      Get GPON DS Gemport None Multicast pass state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortNonMcPassState_get(rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_dsGemPortMacFilterMode_set
 * Description:
 *      Set GPON DS Gemport MAC filter mode
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortMacFilterMode_set(rtk_gpon_macTable_exclude_mode_t macFilterMode);

/* Function Name:
 *      rtk_gpon_dsGemPortMacFilterMode_get
 * Description:
 *      Get GPON DS Gemport MAC filter mode
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortMacFilterMode_get(rtk_gpon_macTable_exclude_mode_t *pMacFilterMode);

/* Function Name:
 *      rtk_gpon_dsGemPortMacForceMode_set
 * Description:
 *      Set GPON DS Gemport MAC force mode
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortMacForceMode_set(rtk_gpon_ipVer_t ipVer,rtk_gpon_mc_force_mode_t macForceMode);

/* Function Name:
 *      rtk_gpon_dsGemPortMacForceMode_get
 * Description:
 *      Get GPON DS Gemport MAC force mode
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortMacForceMode_get(rtk_gpon_ipVer_t ipVer,rtk_gpon_mc_force_mode_t *pMacForceMode);


/* Function Name:
 *      rtk_gpon_dsGemPortMacEntry_set
 * Description:
 *      Set GPON DS Gemport MAC filter MAC entry
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortMacEntry_set(uint32 idx, rtk_mac_t mac);


/* Function Name:
 *      rtk_gpon_dsGemPortMacEntry_get
 * Description:
 *     Get GPON DS Gemport MAC filter MAC entry
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortMacEntry_get(uint32 idx, rtk_mac_t *pMac);


/* Function Name:
 *      rtk_gpon_dsGemPortMacEntry_del
 * Description:
 *     Delete GPON DS Gemport MAC filter MAC entry
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortMacEntry_del(uint32 idx);



/* Function Name:
 *      rtk_gpon_dsGemPortFrameTimeOut_set
 * Description:
 *      Set GPON DS Gemport Frame Time out value
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortFrameTimeOut_set(uint32 timeOutValue);


/* Function Name:
 *      rtk_gpon_dsGemPortFrameTimeOut_get
 * Description:
 *      Get GPON DS Gemport Frame Time out value
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsGemPortFrameTimeOut_get(uint32 *pTimeOutValue);


/* Function Name:
 *      rtk_gpon_ipv6McAddrPtn_get
 * Description:
 *      Get GPON IPv6 MC address prefix
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
*/

extern int32 rtk_gpon_ipv4McAddrPtn_get(uint32 *pPrefix);


/* Function Name:
 *      rtk_gpon_ipv6McAddrPtn_set
 * Description:
 *      Set GPON IPv6 MC address prefix
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
*/

extern int32 rtk_gpon_ipv4McAddrPtn_set(uint32 prefix);

/* Function Name:
 *      rtk_gpon_ipv6McAddrPtn_get
 * Description:
 *      Get GPON IPv6 MC address prefix
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
*/

extern int32 rtk_gpon_ipv6McAddrPtn_get(uint32 *pPrefix);


/* Function Name:
 *      rtk_gpon_ipv6McAddrPtn_set
 * Description:
 *      Set GPON IPv6 MC address prefix
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
*/

extern int32 rtk_gpon_ipv6McAddrPtn_set(uint32 prefix);


/* page 0x05 */

/* Function Name:
 *      rtk_gpon_gtcUsIntr_get
 * Description:
 *      Get US GTC interrupt state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_gtcUsIntr_get(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_gtcUsIntrDlt_get
 * Description:
 *      Get US GTC interrupt status state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_gtcUsIntrDlt_get(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_gtcUsIntrMask_get
 * Description:
 *      Get US GTC Interrupt mask
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_gtcUsIntrMask_get(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_gtcUsIntrMask_set
 * Description:
 *      Set US Interrupt mask
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_gtcUsIntrMask_set(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,rtk_enable_t state);


/* Function Name:
 *      rtk_gpon_forceLaser_set
 * Description:
 *      Set Force Laser status
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_forceLaser_set(rtk_gpon_laser_status_t laserStatus);

/* Function Name:
 *      rtk_gpon_forceLaser_get
 * Description:
 *      Get Force Laser status
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_forceLaser_get(rtk_gpon_laser_status_t *plaserStatus);

/* Function Name:
 *      rtk_gpon_forcePRBS_set
 * Description:
 *      Set force PRBS status
 * Input:
 *      prbsCfg             - PRBS config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_forcePRBS_set(rtk_gpon_prbs_t prbsCfg);

/* Function Name:
 *      rtk_gpon_forcePRBS_get
 * Description:
 *      Get force PRBS status
 * Input:
 *      pPrbsCfg            - pointer of PRBS config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_forcePRBS_get(rtk_gpon_prbs_t *pPrbsCfg);

/* Function Name:
 *      rtk_gpon_ploamState_set
 * Description:
 *      Set PLOAM state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_ploamState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_ploamState_get
 * Description:
 *      Get PLOAM State
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_ploamState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_indNrmPloamState_set
 * Description:
 *      Set Ind normal PLOAM state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_indNrmPloamState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_indNrmPloamState_get
 * Description:
 *     Get Ind normal PLOAM state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_indNrmPloamState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_dbruState_set
 * Description:
 *      Set DBRu state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_dbruState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_dbruState_get
 * Description:
 *       Get DBRu state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dbruState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Set US scramble state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_usScrambleState_set(rtk_enable_t state);;

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Get US scramble state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usScrambleState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_usBurstPolarity_set
 * Description:
 *      Set US burst polarity value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_usBurstPolarity_set(rtk_gpon_polarity_t polarityValue);

/* Function Name:
 *      rtk_gpon_usBurstPolarity_get
 * Description:
 *      Get US burst polarity value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usBurstPolarity_get(rtk_gpon_polarity_t *pPolarityValue);

/* Function Name:
 *      rtk_gpon_eqd_set
 * Description:
 *      Set Eqd value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_eqd_set(uint32 value,int32 offset);

/* Function Name:
 *      rtk_gpon_eqd_get
 * Description:
 *      Get Eqd value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_eqd_get(uint32 *pValue);

/* Function Name:
 *      rtk_gpon_laserTime_set
 * Description:
 *      Set laserTime value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_laserTime_set(uint8 on, uint8 off);

/* Function Name:
 *      rtk_gpon_laserTime_get
 * Description:
 *      Get laser Time value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_laserTime_get(uint8 *on, uint8 *off);

/* Function Name:
 *      rtk_gpon_burstOverhead_set
 * Description:
 *      Set BOH value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_burstOverhead_set(uint8 rep, uint8 len, uint8 size, uint8 *oh);

/* Function Name:
 *      rtk_gpon_usPloam_set
 * Description:
 *      Set US PLOAM
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usPloam_set(rtk_enable_t isUrgent, uint8 *ploamValue);

/* Function Name:
 *      rtk_gpon_usAutoPloam_set
 * Description:
 *      Set US auto ploam
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_usAutoPloam_set(rtk_gpon_usAutoPloamType_t usAutoPloamType, uint8 *ploamValue);

/* Function Name:
 *      rtk_gpon_usPloamCrcGenState_set
 * Description:
 *      Set us PLOAM CRC  State
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_usPloamCrcGenState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *     Get us PLOAM CRC state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usPloamCrcGenState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_usPloamOnuIdFilterState_set
 * Description:
 *      Set US PLOAM ONUID Filter State
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_usPloamOnuIdFilterState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_usPloamOnuIdFilter_get
 * Description:
 *      Get US PLOAM ONUID Filter state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usPloamOnuIdFilter_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_usPloamBuf_flush
 * Description:
 *      Flush us PLOAM buffer
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_usPloamBuf_flush(void);


/* Function Name:
 *      rtk_gpon_usGtcMiscCnt_get
 * Description:
 *      Get US GTC Misc conuter
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usGtcMiscCnt_get(rtk_gpon_usGtc_pmMiscType_t pmMiscType, uint32 *pValue);


/* Function Name:
 *      rtk_gpon_usRdi_set
 * Description:
 *      Set RDI vlaue
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_usRdi_set(int32 value);

/* Function Name:
 *      rtk_gpon_usRdi_get
 * Description:
 *      Get RDI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usRdi_get(int32 *value);


/* Function Name:
 *      rtk_gpon_rdi_set
 * Description:
 *      Set RDI vlaue
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_rdi_set(int32 value);

/* Function Name:
 *      rtk_gpon_rdi_get
 * Description:
 *      Get RDI value
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_rdi_get(int32 *value);


/* Function Name:
 *      rtk_gpon_topGemUsIntr_get
 * Description:
 *      Set US smals start proc state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_usSmalSstartProcState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_usSmalSstartProcState_get
 * Description:
 *      Get US smals start proc state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usSmalSstartProcState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_usSuppressLaserState_set
 * Description:
 *      Set US supper press laser state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_gpon_usSuppressLaserState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_usSuppressLaserState_get
 * Description:
 *      Get US supper press laser state
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usSuppressLaserState_get(rtk_enable_t *pState);


/* page 0x06 */
/* Function Name:
 *      rtk_gpon_gemUsIntr_get
 * Description:
 *      Get US GEM interrupt state
 * Input:
 *      reg_addr    - void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_gemUsIntr_get(rtk_gpon_gemUsIntrType_t gemUsIntrType,rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_gemUsIntrMask_get
 * Description:
 *      Get US GEM interrutp mask state
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_gemUsIntrMask_get(rtk_gpon_gemUsIntrType_t gemUsIntrType,rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_gemUsIntrMask_set
 * Description:
 *      Set US GEM interrutp mask state
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32 rtk_gpon_gemUsIntrMask_set(rtk_gpon_gemUsIntrType_t gemUsIntrType,rtk_enable_t state);



/* Function Name:
 *      rtk_gpon_gemUsForceIdleState_set
 * Description:
 *      Turn on/off force to send IDLE GEM only.
 * Input:
 *      value       - on/off value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32 rtk_gpon_gemUsForceIdleState_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gpon_gemUsForceIdleState_get
 * Description:
 *      Read the setting of force IDLE GEM.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_gemUsForceIdleState_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_gpon_gemUsPtiVector_set
 * Description:
 *      Set the PTI value vector mapping base on (OMCI,END_FRAG).
 * Input:
 *      pti_v0       - the PTI value base on (OMCI,END_FRAG)=(0,0)
 *      pti_v1       - the PTI value  base on (OMCI,END_FRAG)=(0,1)
 *      pti_v2       - the PTI value  base on (OMCI,END_FRAG)=(1,0)
 *      pti_v3       - the PTI value  base on (OMCI,END_FRAG)=(1,1)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32 rtk_gpon_gemUsPtiVector_set(uint8 pti_v0, uint8 pti_v1, uint8 pti_v2, uint8 pti_v3);

/* Function Name:
 *      rtk_gpon_gemUsPtiVector_get
 * Description:
 *      Read the setting of force IDLE GEM.
 * Input:
 *      None
 * Output:
 *      pPti_v0             - the PTI vector 0
 *      pPti_v1             - the PTI vector 1
 *      pPti_v2             - the PTI vector 2
 *      pPti_v3             - the PTI vector 3
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_gemUsPtiVector_get(uint8 *pPti_v0, uint8 *pPti_v1, uint8 *pPti_v2, uint8 *pPti_v3);


/* Function Name:
 *      rtk_gpon_gemUsEthCnt_get
 * Description:
 *      Read the ether counter for local port.
 * Input:
 *      local_idx           - local port index
 * Output:
 *      pEth_cntr           - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
extern int32 rtk_gpon_gemUsEthCnt_get(uint32 idx, uint32 *pEthCntr);

/* Function Name:
 *      rtk_gpon_gemUsGemCnt_read
 * Description:
 *      Read the ether counter for local port.
 * Input:
 *      local_idx           - local port index
 * Output:
 *      pGem_cntr           - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
extern int32 rtk_gpon_gemUsGemCnt_get(uint32 idx, uint32 *pGemCntr);

/* Function Name:
 *      rtk_gpon_gemUsPortCfg_set
 * Description:
 *      Set the mapping of local_idx and gem_port_id.
 * Input:
 *      cycle       - specify the cycle value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_gemUsPortCfg_set(uint32 idx, uint32 gemPortId);

/* Function Name:
 *      rtk_gpon_gemUsPortCfg_get
 * Description:
 *      Read the mapping of local_idx and gem_port_id.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
extern int32 rtk_gpon_gemUsPortCfg_get(uint32 idx, uint32 *pGemPortId);

/* Function Name:
 *      rtk_gpon_gemUsDataByteCnt_get
 * Description:
 *      Read the data byte counter for local port.
 * Input:
 *      local_idx           - local port index
 * Output:
 *      pByte_cntr          - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
extern int32 rtk_gpon_gemUsDataByteCnt_get(uint32 idx, uint64 *pByteCntr);

/* Function Name:
 *      rtk_gpon_gemUsIdleByteCnt_read
 * Description:
 *      Read the IDLE byte counter for TCONT.
 * Input:
 *      local_idx           - local TCONT index
 * Output:
 *      pIdle_cntr          - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
extern int32 rtk_gpon_gemUsIdleByteCnt_get(uint32 idx, uint64 *pIdleCntr);


/* Function Name:
 *      rtk_gpon_dbruPeriod_set
 * Description:
 *      Read the data byte counter for local port.
 * Input:
 *      periodTime           - dbru period time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
extern int32 rtk_gpon_dbruPeriod_set(uint32 periodTime);

/* Function Name:
 *      dal_apollomp_gpon_dbruPeriod_get
 * Description:
 *      Read the data byte counter for local port.
 * Input:
 *      None
 * Output:
 *      *pPeriodTime        - point of period time
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
extern int32 rtk_gpon_dbruPeriod_get(uint32 *pPeriodTime);



/* Function Name:
 *      rtk_gpon_portMacForceMode_set
 * Description:
 *      Set MAC focre mode for PON port
 * Input:
 *       None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_portMacForceMode_set(rtk_port_linkStatus_t linkStatus);


/* Function Name:
 *      rtk_gpon_port_get
 * Description:
 *      Get GPON port id
 * Input:
 *       None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_port_get(rtk_port_t *ponPort);


/* Function Name:
 *      rtk_gpon_gtcDsIntrDlt_check
 * Description:
 *      Check GTC DS interrupt
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_gtcDsIntrDlt_check(rtk_gpon_gtcDsIntrType_t gtcDsIntrType,uint32 gtcDsIntrDltValue,rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_gtcUsIntrDlt_check
 * Description:
 *      Check GTC US interrupt
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_gtcUsIntrDlt_check(rtk_gpon_gtcUsIntrType_t gtcUsIntrType,uint32 gtcUsIntrDltValue,rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_gemUsIntrDlt_check
 * Description:
 *      Check GTC DS interrupt
 * Input:
 *     gtcIntrType: type of gtc interrupt type
 * Output:
 *      pState: point for get gtc interrupt  state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_gemUsIntrDlt_check(rtk_gpon_gemUsIntrType_t gemIntrType,uint32 gemUsIntrDltValue,rtk_enable_t *pState);


/* Function Name:
 *      rtk_gpon_usRogueOnt_set
 * Description:
 *      Config Rogue ONT
 * Input:
 *     rogOntState - enable or disable rogue ont
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_usRogueOnt_set(rtk_enable_t rogOntState);


/* Function Name:
 *      rtk_gpon_rogueOnt_set
 * Description:
 *      Config Rogue ONT
 * Input:
 *     rogOntState - enable or disable rogue ont
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_rogueOnt_set(rtk_enable_t rogOntState);


/* Function Name:
 *      rtk_gpon_drainOutDefaultQueue_set
 * Description:
 *      Drain Out GPON default Queue
 * Input:
 *     rogOntState - enable or disable rogue ont
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_drainOutDefaultQueue_set(void);


/* Function Name:
 *      rtk_gpon_autoDisTx_set
 * Description:
 *      nable or Disable auto disable Tx function
 * Input:
 *     autoDisTxState - enable or disable rogue ont
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_autoDisTx_set(rtk_enable_t autoDisTxState);

/* Function Name:
 *      rtk_gpon_scheInfo_get
 * Description:
 *      get GPON scheduler information.
 * Input:
 *      None
 * Output:
 *      pScheInfo   - pointer of pScheInfo information
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_scheInfo_get(rtk_gpon_schedule_info_t *pScheInfo);

/* Function Name:
 *      rtk_gpon_dataPath_reset
 * Description:
 *      reset GPON data path.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_gpon_dataPath_reset(void);

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
 *      rtk_gpon_berInterval_get
 * Description:
 *      Get Ber Interval value(ber interval value)
 * Input:
 *
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32
rtk_gpon_berInterval_get(int32 *pBerInterval);
/* Function Name:
 *      rtk_gpon_dsOmciCnt_get
 * Description:
 *      Get GPON DS OMCI counter
 * Input:
 *      none
 * Output:
 *      usOmciCnt           - return value of OMCI counter
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dsOmciCnt_get(rtk_gpon_ds_omci_t *dsOmciCnt);

/* Function Name:
 *      rtk_gpon_usOmciCnt_get
 * Description:
 *      Get GPON US OMCI counter
 * Input:
 *      none
 * Output:
 *      usOmciCnt           - return value of OMCI counter
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_usOmciCnt_get(rtk_gpon_us_omci_t *usOmciCnt);

/* Function Name:
 *      rtk_gpon_usDbruBlockSize_get
 * Description:
 *      Get GPON DBRu block size
 * Input:
 *      none
 * Output:
 *      blockSize           - return value of block size
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_usDbruBlockSize_get(uint32 *pBlockSize);

/* Function Name:
 *      rtk_gpon_usDbruBlockSize_set
 * Description:
 *      Set GPON DBRu block size
 * Input:
 *      blockSize           - config value of block size
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_usDbruBlockSize_set(uint32 blockSize);

/* Function Name:
 *      rtk_gpon_dbruBlockSize_get
 * Description:
 *      Get GPON DBRu block size
 * Input:
 *      none
 * Output:
 *      blockSize           - return value of block size
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dbruBlockSize_get(uint32 *pBlockSize);

/* Function Name:
 *      rtk_gpon_dbruBlockSize_set
 * Description:
 *      Set GPON DBRu block size
 * Input:
 *      blockSize           - config value of block size
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gpon_dbruBlockSize_set(uint32 blockSize);

/* Function Name:
 *      rtk_gpon_flowctrl_adjust_byFlowNum
 * Description:
 *      Adjust pbo flowcontrol threshold by flow number
 * Input:
 *      flowNum     - flow number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
rtk_gpon_flowctrl_adjust_byFlowNum(uint32 flowNum);

/* Function Name:
 *      rtk_gpon_usLaserDefault_get
 * Description:
 *      get the default config of us laser on/off offset
 * Input:
 *      None
 * Output:
 *      pLaserOn
 *      pLaserOff
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_DRIVER_NOT_FOUND                 - no driver callback.
 * Note:
 *      None
 */
extern int32
rtk_gpon_usLaserDefault_get(uint8 *pLaserOn, uint8 *pLaserOff);

/* Function Name:
 *      rtk_gpon_smallBwTcont_get
 * Description:
 *      get the bitmask of TCONT which is small bwmap.
 * Input:
 *      smallBw
 * Output:
 *      pTcontBitMask
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_DRIVER_NOT_FOUND                 - no driver callback.
 * Note:
 *      None
 */
extern int32
rtk_gpon_smallBwTcont_get(uint32 smallBw, uint32 *pTcontBitMask);

/* Function Name:
 *      rtk_gpon_autoDG_set
 * Description:
 *      Set GPON auto dying gasp
 * Input:
 *      state           - config value of auto dying gasp
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 
rtk_gpon_autoDG_set(uint32 state);

/* Function Name:
 *      rtk_gpon_autoDG_get
 * Description:
 *      Get GPON auto dying gasp
 * Input:
 *      pState           - pointer of config value of auto dying gasp
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 
rtk_gpon_autoDG_get(uint32 *pState);

/* Function Name:
 *      rtk_gpon_DgCnt_get
 * Description:
 *      Get GPON dying gasp counter
 * Input:
 *      pCnt           - pointer of GPON dying gasp counter
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 
rtk_gpon_DgCnt_get(uint32 *pCnt);

/* Function Name:
 *      rtk_gpon_DgThreshold_set
 * Description:
 *      Set GPON dying gasp threshold
 * Input:
 *      threshold           - config value of dying gasp threshold
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 
rtk_gpon_DgThreshold_set(uint32 threshold);

/* Function Name:
 *      rtk_gpon_DgThreshold_get
 * Description:
 *      Get GPON dying gasp threshold
 * Input:
 *      pThreshold          - pointer of config value of auto dying threshold
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 
rtk_gpon_DgThreshold_get(uint32 *pThreshold);

#endif /*#ifndef _GPON_V2_H_*/
