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
 *
 * *
 * $Revision$
 * $Date$
 *
 * Purpose : GPON Mac Device Driver Internal Data Structure
 *
 * Feature : Provide the GPON Mac Device Driver Internal Data Structure
 *
 */
#ifndef __GPON_DEFS_H__
#define __GPON_DEFS_H__

#include <osal/lib.h>
#include <osal/sem.h>
#include <common/rt_error.h>
#include <module/gpon/gpon.h>
#include <module/gpon/gpon_platform.h>

#define GPON_DRIVER_SW_VERSION_RELEASE  1
#define GPON_DRIVER_SW_VERSION_LOAD     0
#define GPON_DRIVER_SW_VERSION_BUILD    10

#define GPON_DEV_MAX_TCONT_NUM          32
#define GPON_DEV_MAX_FLOW_NUM           128
#define GPON_DEV_MAX_MACTBL_NUM         256
#define GPON_DEV_MAX_TDM_CHANNEL_NUM    16

#define GPON_DEV_MAX_ALLOC_ID           4095
#define GPON_DEV_MAX_GEMPORT_ID         4095

#define GPON_DEV_DEFAULT_ONU_ID         255

#define GPON_DEV_BURST_OVERHEAD_LEN     12
#define GPON_DEV_TOTAL_OVERHEAD_BITS    96

#define GPON_DEV_MAX_LASER_ON           55
#define GPON_DEV_MAX_LASER_OFF          55

#define GPON_DEV_MAX_DS_FEC_DET_THRSH   0x7

#define GPON_DEV_MAX_ASSM_TIMEOUT_FRM   0x1F

#define GPON_DEV_MAX_PTI                0x7

#define GPON_DEV_MAX_OPT_AHEAD_CYCLE    0x1FF
#define GPON_DEV_MAX_OPT_BEHIND_CYCLE   0xF

#define GPON_DEV_MAX_TX_PWR_LEVEL       0x3

#if defined(FPGA_DEFINED)
#define GPON_OMCI_TCONT_ID              7
#else
#define GPON_OMCI_TCONT_ID              31
#endif
#define GPON_OMCI_QUEUE_ID              24

#define GPON_OMCI_FLOW_ID               127
#define GPON_MULTICAST_FLOW_ID          126
#define GPON_MULTICAST_GEM_PORT         4095

#define GPON_DEV_MAX_BOH_LEN            252




typedef enum gpon_drv_status_e{
    GPON_DRV_STATUS_INIT = 0,   /* Driver is initialized but no device is installed */
    GPON_DRV_STATUS_READY,      /* Device is initialized but not activated */
    GPON_DRV_STATUS_ACTIVATE,   /* Device is activated */
}gpon_drv_status_t;


typedef struct gpon_tcont_obj_s{
    uint32                   tcont_id;
    uint32                   alloc_id;
    rtk_gpon_tcontType_t     type;
    int32                    ploam_en;   /* enable if PLOAM is received, reset for each ranging */
    int32                    omci_en;    /* enable if the OMCI is received */
}gpon_tcont_obj_t;

typedef struct gpon_ds_flow_obj_s{
    uint32                   flow_id;
    uint32                   gem_port;
    rtk_gpon_flowType_t      type;
    int32                    multicast;
    int32                    aes_en;
    rtk_gpon_flowPriority_t  priority;
}gpon_ds_flow_obj_t;

typedef struct gpon_us_flow_obj_s{
    uint32                   flow_id;
    uint32                   gem_port;
    rtk_gpon_flowType_t      type;
    uint32                   tcont;
    uint8                    channel;    /* TDM channel ID: 0~15 */
}gpon_us_flow_obj_t;

typedef struct gpon_macTable_obj_s{
    uint8                   addr[RTK_GPON_MACTABLE_ADDR_LEN];
}gpon_macTable_obj_t;

typedef struct gpon_parameter_s{
    rtk_gpon_onu_activation_para_t onu;
    rtk_gpon_laser_para_t          laser;
    rtk_gpon_ds_physical_para_t    dsphy;
    rtk_gpon_ds_ploam_para_t       dsploam;
    rtk_gpon_ds_bwMap_para_t       dsbwmap;
    rtk_gpon_ds_gem_para_t         dsgem;
    rtk_gpon_ds_eth_para_t         dseth;
    rtk_gpon_ds_omci_para_t        dsomci;
#if 0 /* TDM is removed in GPON_MAC_SWIO_r1.1 */
    rtk_gpon_ds_tdm_para_t         dstdm;
#endif
    rtk_gpon_us_physical_para_t    usphy;
    rtk_gpon_us_ploam_para_t       usploam;
    rtk_gpon_us_dbr_para_t         usdbr;
#if 0 /* page US_FRAG is removed */
    rtk_gpon_us_gem_para_t         usgem;
    rtk_gpon_us_eth_para_t         useth;
#endif
#if 0 /* GEM US omci register is removed */
    rtk_gpon_us_omci_para_t        usomci;
#endif
#if 0 /* OPT power save is removed in GPON_MAC_SWIO_r1.1 */
    rtk_gpon_us_opt_pwr_sav_t      ussav;
#endif
}gpon_parameter_t;

typedef struct gpon_burst_oh_s{
    uint8                   guard_bits;
    uint8                   type1_bits;
    uint8                   type2_bits;
    uint8                   type3_ptn;
    uint8                   type3_preranged;
    uint8                   type3_ranged;
    uint8                   delimiter[3];

    uint8                   boh_len_preranged;
    uint8                   boh_len_ranged;
    uint8                   boh_repeat;
    uint8                   boh_valid;
    uint8                   bursthead[GPON_DEV_BURST_OVERHEAD_LEN];
}gpon_burst_oh_t;

typedef struct gpon_macFilterTable_s{
    rtk_gpon_macTable_exclude_mode_t  op_mode;
    int32                       bc_pass;
    int32                       non_mc_pass;
#if 0 /* wellknown addr is removed in GPON_MAC_SWIO_r1.1
         mc_check_en change to non_mc_pass in v1.2 */
    int32                       wellknown_en;
    int32                       mc_check_en;
    uint32                      wellknown_addr;
#endif
    uint32                      ipv4_mc_check_ptn;
    uint32                      ipv6_mc_check_ptn;

    /* Add ipv4 force mode and ipv6 force mode in v1.2 */
    rtk_gpon_mc_force_mode_t    ipv4_force_mode;
    rtk_gpon_mc_force_mode_t    ipv6_force_mode;

}gpon_macFilterTable_t;

typedef enum gpon_RDI_mode_e
{
    GPON_DRV_RDI_STD= 0,
    GPON_DRV_RDI_SFSD,
    GPON_DRV_RDI_END
} gpon_RDI_mode_t;

typedef struct gpon_dev_obj_s{
    /* basic informations */
    uint32                      base_addr;
    uint32                      *timer;
    rtk_gpon_fsm_status_t       status;

    /* static configurations */
    rtk_gpon_serialNumber_t     serial_number;
    rtk_gpon_password_t         password;
    gpon_parameter_t            parameter;
    uint8                       power_level;                /* the power level used by ONU */
    rtk_gpon_aes_key_t          aes_key;
    rtk_gpon_chunkHeader_t      chunk_header;
    rtk_gpon_shaper_t           shaper;
    gpon_macFilterTable_t       mactbl;
    int32                       req_en;
    int32                       rdi;
    int32                       tdm_en;

    /* dynamic configurations and status */
    uint8                       onuid;
    int32                       ds_fec;
    int32                       us_fec;
    int32                       us_ploam_req_urg;
    int32                       us_ploam_req_nrm;
    uint32                      us_omci_flow;
#if 0 /* DBA TCONT is removed in GPON_MAC_SWIO_r1.1 */
    uint32                      us_dba_tcont;
#endif
    uint32                      aes_key_switch_time;
    uint8                       key_index;
    gpon_burst_oh_t             burst_overhead;

    /* configuration tables */
    gpon_tcont_obj_t           **p_tcont;
    gpon_ds_flow_obj_t         **p_dsflow;
    gpon_us_flow_obj_t         **p_usflow;
    gpon_macTable_obj_t        *p_mac_tbl[GPON_DEV_MAX_MACTBL_NUM];
    int32                       alarm_tbl[RTK_GPON_ALARM_MAX];

    /* callback hooks */
    rtk_gpon_eventHandleFunc_stateChange_t  state_change_callback;
    rtk_gpon_eventHandleFunc_fecChange_t    dsfec_change_callback;
    rtk_gpon_eventHandleFunc_fecChange_t    usfec_change_callback;
    rtk_gpon_eventHandleFunc_usPloamEmpty_t usploam_urg_epty_callback;
    rtk_gpon_eventHandleFunc_usPloamEmpty_t usploam_nrm_epty_callback;
    rtk_gpon_eventHandleFunc_ploam_t        ploam_callback;
    rtk_gpon_eventHandleFunc_omci_t         omci_callback;
    rtk_gpon_callbackFunc_queryAesKey_t     aeskey_callback;
    rtk_gpon_eventHandleFunc_fault_t        fault_callback[RTK_GPON_ALARM_MAX];
    rtk_gpon_eventHandleFunc_signal_t       signal_callback;
    rtk_gpon_extMsgGetHandleFunc_t          extMsgGet_callback;
    rtk_gpon_extMsgSetHandleFunc_t          extMsgSet_callback;

    /* temporary counters */
    uint16                  cnt_boh_tx;
    uint16                  cnt_ploamu_tx;
    uint16                  cnt_dbru_tx;
    uint16                  cnt_gem_tx;

    /* software counter */
    uint32                  cnt_cpu_ploam_rx;
    uint32                  cnt_cpu_omci_rx;
    uint32                  cnt_cpu_ploam_tx;
    uint32                  cnt_cpu_ploam_tx_urg;
    uint32                  cnt_cpu_ploam_tx_nor;
    uint32                  cnt_cpu_omci_tx;
    uint32                  cnt_cpu_omci_tx_urg;
    uint32                  cnt_cpu_omci_tx_nor;
    uint32                  cnt_cpu_ploam_rx_unknown;

    /* debug purpose */
    int32 auto_tcont_state;
    int32 auto_boh_state;
    int32 eqd_offset;

    /* keep pre eqd for O6 to O4 used */
    uint32 pre_eqd;

	uint32 eqd; /* for display purpose */

    /* BER timer */
    uint32 *ber_timer;
    uint32 ber_interval;

    /* DIS_TX GPIO */
    uint32 gpio_disTx;

    rtk_gpon_schedule_info_t scheInfo;

    /* SF SD threshold */
    uint32 sf_threshold;
    uint32 sd_threshold;

    /* US eth flow unmber */
    uint32 us_flow_eth_num;

    /* sd too-long and sd mismatch cnt */
    uint32 sd_toolong_cnt;
    uint32 sd_mismatch_cnt;

    /* pps count*/
    uint32 pps_cnt;

    /* RDI mode */
    gpon_RDI_mode_t rdiMode;

    /* for fec total block calc */
    int32 ds_fec_superframe;

    /* for us gem bank under_flow and over_flow count */
    uint32 us_gem_underflow_cnt;
    uint32 us_gem_overflow_cnt;
}gpon_dev_obj_t;

typedef struct gpon_drv_obj_s{
    gpon_drv_status_t          status;
    gpon_os_lock_t             lock;
    gpon_dev_obj_t             *dev;
}gpon_drv_obj_t;


#define GPON_UTIL_ERR_CHK(op, ret)\
do {\
    if ((ret = (op)) != RT_ERR_OK)\
    {\
                RT_ERR(ret, (MOD_DAL|MOD_GPON), ""); \
        return ret;\
    }\
} while(0)



#endif  /* __GPON_DEFS_H__ */

