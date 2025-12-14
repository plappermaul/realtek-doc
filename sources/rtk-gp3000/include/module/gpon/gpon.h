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
#include <rtk/gponv2.h>
#include <rtk/ponmac.h>



/*
 * GPON MAC Device Version Infomation. Read from ASIC. For Debugging
 */
typedef struct rtk_gpon_device_ver_s{
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
}rtk_gpon_us_dbr_para_t;

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
 * GPON MAC TCONT Structure
 */
#define RTK_GPON_ALLOC_ID_NOUSE          0xFFFF

#define RTK_GPON_GEMPORT_ID_NOUSE        0xFFFF

/*
 * GPON MAC PLOAM Message Structure, without CRC octet.
 */
#define RTK_GPON_PLOAM_MSG_LEN           10

typedef struct rtk_gpon_ploam_e{
    uint8 onuid;
    uint8 type;
    uint8 msg[RTK_GPON_PLOAM_MSG_LEN];
}rtk_gpon_ploam_t;

typedef struct rtk_gpon_alarm_s{
    uint8 msg[2];//first byte is type, snd byte is state
}rtk_gpon_alarm_t;

/*
 * GPON MAC OMCI Message Structure, a whole message with CRC octets
 */
//#define RTK_GPON_OMCI_MSG_LEN            64
#define RTK_GPON_OMCI_BASELINE_MSG_LEN_TX  48
#define RTK_GPON_OMCI_MSG_HEADER_LEN       14
#define RTK_GPON_OMCI_MSG_MAX_CONTENT_LEN  1966
#define RTK_GPON_OMCI_MSG_LEN_TX           1980
#define RTK_GPON_OMCI_MSG_LEN              1980


enum {
    RTK_GPON_MSG_OMCI,
    RTK_GPON_MSG_PLOAM,
    RTK_GPON_MSG_ALARM,
};

typedef struct rtk_gpon_omci_msg_s{
    uint8 msg[RTK_GPON_OMCI_MSG_LEN];
    uint32 len;
}rtk_gpon_omci_msg_t;


typedef union rtk_gpon_msg_u{
    rtk_gpon_omci_msg_t     omci;
    rtk_gpon_ploam_t        ploam;
    rtk_gpon_alarm_t        alarm;
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

typedef enum {
    GPON_PLOAM_CONTINUE=0,
    GPON_PLOAM_STOP_WITH_ACK =1,
    GPON_PLOAM_STOP=2,
} rtk_gpon_ploam_state_t;

/*
 * GPON MAC Register R/W Structure
 */
typedef struct rtk_gpon_register_s{
    uint32 page;
    uint32 offset;
    uint32 value;
}rtk_gpon_register_t;


#define RTK_GPON_EXTMESG_MEX_LEN 256

typedef struct rtk_gpon_extMsg_s{
    int optId;
    int len;
    char extValue[RTK_GPON_EXTMESG_MEX_LEN];
}rtk_gpon_extMsg_t;


typedef enum gpon_ploam_dsType_e{
    GPON_PLOAM_DS_OVERHEAD = 0x01,
    GPON_PLOAM_DS_SNMASK = 0x02,/* deprecated in G.984.3(2007)*/
    GPON_PLOAM_DS_ASSIGNONUID = 0x03,
    GPON_PLOAM_DS_RANGINGTIME = 0x04,
    GPON_PLOAM_DS_DEACTIVEONU = 0x05,
    GPON_PLOAM_DS_DISABLESN = 0x06,
    GPON_PLOAM_DS_CFG_VPVC = 0x07,
    GPON_PLOAM_DS_ENCRYPTPORT = 0x08,
    GPON_PLOAM_DS_REQUESTPASSWORD = 0x09,
    GPON_PLOAM_DS_ASSIGNEDALLOCID = 0x0A,
    GPON_PLOAM_DS_NOMESSAGE = 0x0B,
    GPON_PLOAM_DS_POPUP = 0x0C,
    GPON_PLOAM_DS_REQUESTKEY = 0x0D,
    GPON_PLOAM_DS_CONFIGPORT = 0x0E,
    GPON_PLOAM_DS_PEE = 0x0F,
    GPON_PLOAM_DS_POWERLEVEL = 0x10,
    GPON_PLOAM_DS_PST = 0x11,
    GPON_PLOAM_DS_BER_INTERVAL = 0x12,
    GPON_PLOAM_DS_SWITCHINGKEY = 0x13,
    GPON_PLOAM_DS_EXT_BURSTLENGTH = 0x14,
    GPON_PLOAM_DS_SWIFT_POPUP = 0x16,
    GPON_PLOAM_DS_RANG_ADJUST = 0x17,
    GPON_PLOAM_DS_DISABLESN_ZTE = 0x81,

}gpon_ploam_dsType_t;

typedef enum gpon_ploam_usType_e{
    GPON_PLOAM_US_SN = 0x01,
    GPON_PLOAM_US_PASSWORD = 0x02,
    GPON_PLOAM_US_DYINGGASP = 0x03,
    GPON_PLOAM_US_NOMESSAGE = 0x04,
    GPON_PLOAM_US_ENCRYPTKEY = 0x05,
    GPON_PLOAM_US_PEE = 0x06,
    GPON_PLOAM_US_PST = 0x07,
    GPON_PLOAM_US_REI = 0x08,
    GPON_PLOAM_US_ACKNOWLEDGE = 0x09
}gpon_ploam_usType_t;

typedef enum gpon_signal_type_e{
    GPON_SIGNAL_DEACTIVE    = 0x41,
    GPON_SIGNAL_ACTIVE      = 0x42
}gpon_signal_type_t;

typedef struct rtk_gpon_rogue_sd_cnt_s{
    uint32    rogue_sd_toolong;
    uint32    rogue_sd_mismatch;
}rtk_gpon_rogue_sd_cnt_t;

typedef struct rtk_gpon_fec_status_s{
    uint32    ds_fec_status;
    uint32    us_fec_status;
}rtk_gpon_fec_status_t;

#define RTK_GPON_DELIMITER_LEN 3
typedef struct rtk_gpon_gtc_info_s{
    uint8  guard_bits;
    uint8  type1_bits;
    uint8  type2_bits;
    uint8  type3_bits;
    uint8  type3_ptn; 
    uint8  delimiter[RTK_GPON_DELIMITER_LEN];
    uint32 eqd;
}rtk_gpon_gtc_info_t;

/*
 * GPON MAC FSM State Change Callback.
 * It is to be triggered when the ONU state is changed.
 */
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
typedef int (*rtk_gpon_eventHandleFunc_ploam_t)(rtk_gpon_ploam_t* ploam);

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
typedef void (*rtk_gpon_eventHandleFunc_fault_t)(rtk_gpon_alarm_type_t alarmType, int32 set);


typedef int (*rtk_gpon_eventHandleFunc_signal_t)(gpon_signal_type_t type);

/*
 * GPON MAC Alarm Event Report.
 * It is to be triggered when the Alarm is set or clear.
 */
typedef int (*rtk_gpon_extMsgSetHandleFunc_t)(rtk_gpon_extMsg_t *pExtMSg);
typedef int (*rtk_gpon_extMsgGetHandleFunc_t)(rtk_gpon_extMsg_t *pExtMSg);

/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_gponapp_driver_initialize
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
rtk_gponapp_driver_initialize(void);

/* Function Name:
 *      rtk_gponapp_driver_deInitialize
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
rtk_gponapp_driver_deInitialize(void);

/* Function Name:
 *      rtk_gponapp_device_initialize
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
rtk_gponapp_device_initialize(void);

/* Function Name:
 *      rtk_gponapp_device_deInitialize
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
rtk_gponapp_device_deInitialize(void);

/* Function Name:
 *      rtk_gponapp_evtHdlStateChange_reg
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
rtk_gponapp_evtHdlStateChange_reg(rtk_gpon_eventHandleFunc_stateChange_t func);

/* Function Name:
 *      rtk_gponapp_evtHdlStateChange_dereg
 * Description:
 *      This function is called to deregister the callback function of the State Change.
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
rtk_gponapp_evtHdlStateChange_dereg(void);

/* Function Name:
 *      rtk_gponapp_evtHdlDsFecChange_reg
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
rtk_gponapp_evtHdlDsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func);

/* Function Name:
 *      rtk_gponapp_evtHdlUsFecChange_reg
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
rtk_gponapp_evtHdlUsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func);

/* Function Name:
 *      rtk_gponapp_evtHdlUsPloamUrgEmpty_reg
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
rtk_gponapp_evtHdlUsPloamUrgEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func);

/* Function Name:
 *      rtk_gponapp_evtHdlUsPloamNrmEmpty_reg
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
rtk_gponapp_evtHdlUsPloamNrmEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func);

/* Function Name:
 *      rtk_gponapp_evtHdlPloam_reg
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
rtk_gponapp_evtHdlPloam_reg(rtk_gpon_eventHandleFunc_ploam_t func);


/* Function Name:
 *      rtk_gponapp_evtHdlPloam_reg
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
rtk_gponapp_evtHdlPloam_dreg(void);


/* Function Name:
 *      rtk_gponapp_evtHdlOmci_reg
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
rtk_gponapp_evtHdlOmci_reg(rtk_gpon_eventHandleFunc_omci_t func);

/* Function Name:
 *      rtk_gponapp_callbackQueryAesKey_reg
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
rtk_gponapp_callbackQueryAesKey_reg(rtk_gpon_callbackFunc_queryAesKey_t func);

/* Function Name:
 *      rtk_gponapp_evtHdlAlarm_reg
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
rtk_gponapp_evtHdlAlarm_reg(rtk_gpon_alarm_type_t alarmType, rtk_gpon_eventHandleFunc_fault_t func);

/* Function Name:
 *      rtk_gponapp_evtHdlAlarm_dreg
 * Description:
 *      This function is called to deregister the alarm event handler of the alarm by alarm type.
 * Input:
 *      alarmType       - the alarm type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gponapp_evtHdlAlarm_dreg(rtk_gpon_alarm_type_t alarmType);

/* Function Name:
 *      rtk_gponapp_callbackSignal_reg
 * Description:
 *      This function is called to register the callback function of the O2 lost detect.
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
rtk_gponapp_callbackSignal_reg(rtk_gpon_eventHandleFunc_signal_t func);

/* Function Name:
 *      rtk_gponapp_serialNumber_set
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
rtk_gponapp_serialNumber_set(rtk_gpon_serialNumber_t *pSN);

/* Function Name:
 *      rtk_gponapp_serialNumber_get
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
rtk_gponapp_serialNumber_get(rtk_gpon_serialNumber_t *pSN);

/* Function Name:
 *      rtk_gponapp_password_set
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
rtk_gponapp_password_set(rtk_gpon_password_t *pPwd);

/* Function Name:
 *      rtk_gponapp_password_get
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
rtk_gponapp_password_get(rtk_gpon_password_t *pPwd);

/* Function Name:
 *      rtk_gponapp_parameter_set
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
rtk_gponapp_parameter_set(rtk_gpon_patameter_type_t type, void *pPara);

/* Function Name:
 *      rtk_gponapp_parameter_get
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
rtk_gponapp_parameter_get(rtk_gpon_patameter_type_t type, void *pPara);

/* Function Name:
 *      rtk_gponapp_activate
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
rtk_gponapp_activate(rtk_gpon_initialState_t initState);

/* Function Name:
 *      rtk_gponapp_deActivate
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
rtk_gponapp_deActivate(void);

/* Function Name:
 *      rtk_gponapp_ponStatus_get
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
rtk_gponapp_ponStatus_get(rtk_gpon_fsm_status_t *pStatus);

/* Function Name:
 *      rtk_gponapp_isr_entry
 * Description:
 *      GPON MAC ISR entry
 * Input:
*     None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      It should be called in interrupt process or a polling thread
 */
extern void
rtk_gponapp_isr_entry(void);

/* Function Name:
 *      rtk_gponapp_tcont_create
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
rtk_gponapp_tcont_create(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr);

/* Function Name:
 *      rtk_gponapp_tcont_destroy_logical
 * Description:
 *      GPON MAC Remove a logical TCont.
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
rtk_gponapp_tcont_destroy_logical(rtk_gpon_tcont_ind_t *pInd);

/* Function Name:
 *      rtk_gponapp_tcont_destroy
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
rtk_gponapp_tcont_destroy(rtk_gpon_tcont_ind_t *pInd);

/* Function Name:
 *      rtk_gponapp_tcont_get
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
rtk_gponapp_tcont_get(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr);

/* Function Name:
 *      rtk_gponapp_tcont_get_physical
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
rtk_gponapp_tcont_get_physical(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr);

/* Function Name:
 *      rtk_gponapp_dsFlow_set
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
rtk_gponapp_dsFlow_set(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr);

/* Function Name:
 *      rtk_gponapp_dsFlow_get
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
rtk_gponapp_dsFlow_get(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr);

/* Function Name:
 *      rtk_gponapp_usFlow_set
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
rtk_gponapp_usFlow_set(uint32 flowId, rtk_gpon_usFlow_attr_t *pAttr);

/* Function Name:
 *      rtk_gponapp_usFlow_get
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
rtk_gponapp_usFlow_get(uint32 flowId, rtk_gpon_usFlow_attr_t *pAttr);

/* Function Name:
 *      rtk_gponapp_ploam_send
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
rtk_gponapp_ploam_send(int32 urgent, rtk_gpon_ploam_t *pPloam);

/* Function Name:
 *      rtk_gponapp_broadcastPass_set
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
rtk_gponapp_broadcastPass_set(int32 mode);

/* Function Name:
 *      rtk_gponapp_broadcastPass_get
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
rtk_gponapp_broadcastPass_get(int32 *pMode);

/* Function Name:
 *      rtk_gponapp_nonMcastPass_set
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
rtk_gponapp_nonMcastPass_set(int32 mode);

/* Function Name:
 *      rtk_gponapp_nonMcastPass_get
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
rtk_gponapp_nonMcastPass_get(int32 *pMode);

/* Function Name:
 *      rtk_gponapp_multicastAddrCheck_set
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
rtk_gponapp_multicastAddrCheck_set(uint32 ipv4_pattern, uint32 ipv6_pattern);

/* Function Name:
 *      rtk_gponapp_multicastAddrCheck_get
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
rtk_gponapp_multicastAddrCheck_get(uint32 *pIpv4_Pattern, uint32 *pIpv6_Pattern);

/* Function Name:
 *      rtk_gponapp_macFilterMode_set
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
rtk_gponapp_macFilterMode_set(rtk_gpon_macTable_exclude_mode_t mode);

/* Function Name:
 *      rtk_gponapp_macFilterMode_get
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
rtk_gponapp_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t *pMode);

/* Function Name:
 *      rtk_gponapp_mcForceMode_set
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
rtk_gponapp_mcForceMode_set(rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6);

/* Function Name:
 *      rtk_gponapp_mcForceMode_get
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
rtk_gponapp_mcForceMode_get(rtk_gpon_mc_force_mode_t *pIpv4, rtk_gpon_mc_force_mode_t *pIpv6);

/* Function Name:
 *      rtk_gponapp_macEntry_add
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
rtk_gponapp_macEntry_add(rtk_gpon_macTable_entry_t *pEntry);

/* Function Name:
 *      rtk_gponapp_macEntry_del
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
rtk_gponapp_macEntry_del(rtk_gpon_macTable_entry_t *pEntry);

/* Function Name:
 *      rtk_gponapp_macEntry_get
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
rtk_gponapp_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t *pEntry);

/* Function Name:
 *      rtk_gponapp_rdi_set
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
rtk_gponapp_rdi_set(int32 enable);

/* Function Name:
 *      rtk_gponapp_rdi_get
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
rtk_gponapp_rdi_get(int32 *pEnable);

/* Function Name:
 *      rtk_gponapp_powerLevel_set
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
rtk_gponapp_powerLevel_set(uint8 level);

/* Function Name:
 *      rtk_gponapp_powerLevel_set
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
rtk_gponapp_powerLevel_get(uint8 *pLevel);

/* Function Name:
 *      rtk_gponapp_alarmStatus_get
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
rtk_gponapp_alarmStatus_get(rtk_gpon_alarm_type_t alarm, int32 *pStatus);


/* Function Name:
 *      rtk_gponapp_globalCounter_get
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
rtk_gponapp_globalCounter_get (rtk_gpon_global_performance_type_t type, rtk_gpon_global_counter_t *pPara);

/* Function Name:
 *      rtk_gponapp_tcontCounter_get
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
rtk_gponapp_tcontCounter_get (uint32 tcontId, rtk_gpon_tcont_performance_type_t type, rtk_gpon_tcont_counter_t *pPara);

/* Function Name:
 *      rtk_gponapp_flowCounter_get
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
rtk_gponapp_flowCounter_get (uint32 flowId, rtk_gpon_flow_performance_type_t type, rtk_gpon_flow_counter_t *pPara);

/* Function Name:
 *      rtk_gponapp_version_get
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
rtk_gponapp_version_get(rtk_gpon_device_ver_t *pHver, rtk_gpon_driver_ver_t *pSver);

/* Function Name:
 *      rtk_gponapp_txForceLaser_set
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
rtk_gponapp_txForceLaser_set(rtk_gpon_laser_status_t status);

/* Function Name:
 *      rtk_gponapp_txForceLaser_get
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
rtk_gponapp_txForceLaser_get(rtk_gpon_laser_status_t *pStatus);

/* Function Name:
 *      rtk_gponapp_txForceIdle_set
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
rtk_gponapp_txForceIdle_set(int32 on);

/* Function Name:
 *      rtk_gponapp_txForceIdle_get
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
rtk_gponapp_txForceIdle_get(int32 *pOn);

/* Function Name:
 *      rtk_gponapp_txForcePRBS_set
 * Description:
 *      GPON MAC set to force insert PRBS in upstream.
 * Input:
 *      prbsCfg         - specify force PRBS config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gponapp_txForcePRBS_set(rtk_gpon_prbs_t prbsCfg);

/* Function Name:
 *      rtk_gponapp_txForcePRBS_get
 * Description:
 *      GPON MAC get the status of force insert PRBS in upstream.
 * Input:
 *      pPrbsCfg        - pointer of force PRBS config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gponapp_txForcePRBS_get(rtk_gpon_prbs_t *pPrbsCfg);

/* Function Name:
 *      rtk_gponapp_dsFecSts_get
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
rtk_gponapp_dsFecSts_get(int32* pEn);

/* Function Name:
 *      rtk_gponapp_usFecSts_get
 * Description:
 *      GPON MAC get the status to FEC in upstream.
 * Input:
 *      pStatus         - pointer of U/S FEC status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32
rtk_gponapp_usFecSts_get(int32* pEn);

/* Function Name:
 *      rtk_gponapp_version_show
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
rtk_gponapp_version_show(void);

/* Function Name:
 *      rtk_gponapp_devInfo_show
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
rtk_gponapp_devInfo_show(void);

/* Function Name:
 *      rtk_gponapp_gtc_show
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
rtk_gponapp_gtc_show(void);

/* Function Name:
 *      rtk_gponapp_tcont_show
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
rtk_gponapp_tcont_show(uint32 tcont);

/* Function Name:
 *      rtk_gponapp_dsFlow_show
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
rtk_gponapp_dsFlow_show(uint32 flow);

/* Function Name:
 *      rtk_gponapp_usFlow_show
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
rtk_gponapp_usFlow_show(uint32 flow);

/* Function Name:
 *      rtk_gponapp_macTable_show
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
rtk_gponapp_macTable_show(void);

/* Function Name:
 *      rtk_gponapp_globalCounter_show
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
rtk_gponapp_globalCounter_show(rtk_gpon_global_performance_type_t type);

/* Function Name:
 *      rtk_gponapp_tcontCounter_show
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
rtk_gponapp_tcontCounter_show(uint32 idx, rtk_gpon_tcont_performance_type_t type);

/* Function Name:
 *      rtk_gponapp_flowCounter_show
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
rtk_gponapp_flowCounter_show(uint32 idx, rtk_gpon_flow_performance_type_t type);

/* Function Name:
 *      rtk_gponapp_omci_tx
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
rtk_gponapp_omci_tx(rtk_gpon_omci_msg_t *omci);

/* Function Name:
 *      rtk_gponapp_omci_rx
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
rtk_gponapp_omci_rx(uint8 *buf, uint32 len);

/* Function Name:
 *      rtk_gponapp_debug_set
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
rtk_gponapp_debug_set(int32 enable, uint32 level);

/* Function Name:
 *      rtk_gponapp_autoTcont_set
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
rtk_gponapp_autoTcont_set(int32 state);

/* Function Name:
 *      rtk_gponapp_autoTcont_get
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
rtk_gponapp_autoTcont_get(int32 *pState);

/* Function Name:
 *      rtk_gponapp_autoBoh_set
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
rtk_gponapp_autoBoh_set(int32 state);

/* Function Name:
 *      rtk_gponapp_autoBoh_get
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
rtk_gponapp_autoBoh_get(int32 *pState);

/* Function Name:
 *      rtk_gponapp_eqdOffset_set
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
rtk_gponapp_eqdOffset_set(int32 offset);

/* Function Name:
 *      rtk_gponapp_eqdOffset_get
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
rtk_gponapp_eqdOffset_get(int32 *pOffset);


/* Function Name:
 *      rtk_gpon_initial
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
rtk_gponapp_initial(uint32 data);



/* Function Name:
 *      rtk_gpon_deinitial
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
rtk_gponapp_deinitial(void);


/* Function Name:
 *      rtk_gpon_unit_test
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
rtk_gponapp_unit_test(uint32 id);



/* Function Name:
 *      rtk_gponapp_callbackExtMsgSetHandle_reg
 * Description:
 *      This function is called to register the extension set handler
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
rtk_gponapp_callbackExtMsgSetHandle_reg(rtk_gpon_extMsgSetHandleFunc_t func);


/* Function Name:
 *      rtk_gponapp_callbackExtMsgGetHandle_reg
 * Description:
 *      This function is called to register the extension get handler
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
rtk_gponapp_callbackExtMsgGetHandle_reg(rtk_gpon_extMsgGetHandleFunc_t func);



/* Function Name:
 *      rtk_gponapp_runOmciWrapper_set
 * Description:
 *      This function is called for extension message set
 * Input:
 *      optId        - extend option id
 *      pValue          - value for running
 *      count          - count of type value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gponapp_extMsg_set(rtk_gpon_extMsg_t *pExtMsg);


/* Function Name:
 *      rtk_gponapp_extMsg_get
 * Description:
 *      This function is called for run extension message get
 * Input:
 *      pExtMsg   - retunr extension message result
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32
rtk_gponapp_extMsg_get(rtk_gpon_extMsg_t *pExtMsg);

/* Function Name:
 *      rtk_gponapp_aesKeySwitch_get
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
extern int32
rtk_gponapp_aesKeySwitch_get(uint32 *pSuperframe);

/* Function Name:
 *      rtk_gponapp_berInterval_get
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
rtk_gponapp_berInterval_get(int32 *pBerInterval);

/* Function Name:
 *      rtk_gponapp_dbruBlockSize_get
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
extern int32 rtk_gponapp_dbruBlockSize_get(int32* pBlockSize);

/* Function Name:
 *      rtk_gponapp_dbruBlockSize_set
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
extern int32 rtk_gponapp_dbruBlockSize_set(int32 blockSize);

/* Function Name:
 *      rtk_gponapp_rogueOnt_set
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
extern int32 rtk_gponapp_rogueOnt_set(rtk_enable_t rogOntState);

/* Function Name:
 *      rtk_gponapp_signal_parameter_set
 * Description:
 *      GPON MAC Set signal threshold.
 * Input:
 *      pPara           - the pointer of signal threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32 rtk_gponapp_signal_parameter_set(rtk_gpon_sig_para_t *pPara);

/* Function Name:
 *      rtk_gponapp_RDI_mode_set
 * Description:
 *      GPON RDI mode set
 * Input:
 *      RDIMode           - RDI Mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32 rtk_gponapp_RDI_mode_set(uint32 RDIMode);

/* Function Name:
 *      rtk_gponapp_berTimerEnable_set
 * Description:
 *      GPON MAC Set BER timer enable.
 * Input:
 *      enable          - the BER timer enable flag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32 rtk_gponapp_berTimerEnable_set(rtk_enable_t enable);

/* Function Name:
 *      rtk_gponapp_rogueSdCnt_get
 * Description:
 *      GPON MAC get sd too long and mismatch count.
 * Input:
 *      None
 * Output:
 *      pRogueSdCnt : the pointer of rogue sd count
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32 rtk_gponapp_rogueSdCnt_get(rtk_gpon_rogue_sd_cnt_t *pRogueSdCnt);

/* Function Name:
 *      rtk_gponapp_ppsCnt_readClear
 * Description:
 *      Read and clear pps cnt.
 * Input:
 *      None
 * Output:
 *      pPpsCnt         - return pointer of pps cnt
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32 rtk_gponapp_ppsCnt_readClear(uint32 *pPpsCnt);

/* Function Name:
 *      rtk_gponapp_forceEmergencyStop_set
 * Description:
 *      Force emergency stop state
 * Input:
 *      state           - the emergency stop state flag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
extern int32 rtk_gponapp_forceEmergencyStop_set(rtk_enable_t state);

/* Function Name:
 *      rtk_gponapp_omci_mirror_set
 * Description:
 *      GPON MAC mirror omci message.
 * Input:
 *      pMirroringPort  - Mirroring Port Id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_gponapp_omci_mirror_set(uint32 *pMirroringPort);

/* Function Name:
 *      rtk_gponapp_aggTcont_clear
 * Description:
 *      clear tcont for aggregate tcont set
 * Input:
 *      aggIndex  - aggregate tcont index
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern int32 rtk_gponapp_aggTcont_clear(rtk_ponmac_aggregate_tcont_t aggIndex);

/* Function Name:
 *      rtk_gponapp_fec_get
 * Description:
 *      This function is called for fec status get
 * Input:
 *      None
 * Output:
 *      ds_fec - downstream fec status
 *      us_fec - upstream fec status
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32 rtk_gponapp_fec_get(rtk_gpon_fec_status_t *pFec);

/* Function Name:
 *      rtk_gponapp_gtc_get
 * Description:
 *      This function is to get GTC information
 * Input:
 *      None
 * Output:
 *      pGtc - pointer of GTC
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
extern int32 rtk_gponapp_gtc_get(rtk_gpon_gtc_info_t *pGtc);

/* Function Name:
 *      rtk_gponapp_eqd_get
 * Description:
 *      Get GPON EQD
 * Input:
 *      none
 * Output:
 *      eqd                 - return value of EQD
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
extern int32 rtk_gponapp_eqd_get(uint32* eqd);

/* Function Name:
 *      rtk_gponapp_DgThreshold_set
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
extern int32 rtk_gponapp_DgThreshold_set(uint32 threshold);

/* Function Name:
 *      rtk_gponapp_DgThreshold_get
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
extern int32 rtk_gponapp_DgThreshold_get(uint32 *pThreshold);

extern int32 rtk_gpon_parameter_set(rtk_gpon_patameter_type_t type, void *pPara);
extern int32 rtk_gpon_broadcastPass_set(int32 mode);
extern int32 rtk_gpon_broadcastPass_get(int32 *pMode);
extern int32 rtk_gpon_nonMcastPass_set(int32 mode);
extern int32 rtk_gpon_nonMcastPass_get(int32 *pMode);
extern int32 rtk_gpon_multicastAddrCheck_get(uint32 *pIpv4_Pattern, uint32 *pIpv6_Pattern);
extern int32 rtk_gpon_mcForceMode_get(rtk_gpon_mc_force_mode_t *pIpv4, rtk_gpon_mc_force_mode_t *pIpv6);
extern int32 rtk_gpon_mcForceMode_set(rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6);
extern int32 rtk_gpon_macFilterMode_set(rtk_gpon_macTable_exclude_mode_t mode);
extern int32 rtk_gpon_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t *pMode);
extern int32 rtk_gpon_multicastAddrCheck_set(uint32 ipv4_pattern, uint32 ipv6_pattern);
extern int32 rtk_gpon_macEntry_add(rtk_gpon_macTable_entry_t *pEntry);
extern int32 rtk_gpon_macEntry_del(rtk_gpon_macTable_entry_t *pEntry);
extern int32 rtk_gpon_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t *pEntry);
extern int32 rtk_gpon_txForceLaser_set(rtk_gpon_laser_status_t status);
extern int32 rtk_gpon_txForceLaser_get(rtk_gpon_laser_status_t *pStatus);
extern int32 rtk_gpon_gtc_show(void);
extern int32 rtk_gpon_tcont_show(uint32 tcont);
extern int32 rtk_gpon_dsFlow_show(uint32 flow);
extern int32 rtk_gpon_usFlow_show(uint32 flow);
extern int32 rtk_gpon_initial(uint32 data);
extern int32 rtk_gpon_deinitial(void);
extern int32 rtk_gpon_debug_set(int32 enable, uint32 level);
extern int32 rtk_gpon_unit_test(uint32 id);
extern int32 rtk_gpon_txForceIdle_set(int32 on);
extern int32 rtk_gpon_txForceIdle_get(int32 *pOn);
extern int32 rtk_gpon_txForcePRBS_set(rtk_gpon_prbs_t prbsCfg);
extern int32 rtk_gpon_omci_tx(rtk_gpon_omci_msg_t *omci);
extern int32 rtk_gpon_autoTcont_set(int32 state);
extern int32 rtk_gpon_autoTcont_get(int32 *pState);
extern int32 rtk_gpon_autoBoh_set(int32 state);
extern int32 rtk_gpon_autoBoh_get(int32 *pState);
extern int32 rtk_gpon_eqdOffset_set(int32 offset);
extern int32 rtk_gpon_eqdOffset_get(int32 *pOffset);
extern int32 rtk_gpon_berTimerEnable_set(rtk_enable_t enable);
extern int32 rtk_gpon_rogueSdCnt_get(rtk_gpon_rogue_sd_cnt_t *pRogueSdCnt);
extern int32 rtk_gpon_ppsCnt_readClear(uint32  *pPpsCnt);
extern int32 rtk_gpon_fec_get(rtk_gpon_fec_status_t *pFec);
extern int32 rtk_gpon_eqd_get(uint32 *pEqd);

#if defined(__LINUX_KERNEL__) || defined(RTL_CYGWIN_EMULATE)
#if !defined(CONFIG_LUNA_G3_SERIES)
/*wrapper api*/
#define rtk_gpon_serialNumber_set(a) rtk_gponapp_serialNumber_set(a)
#define rtk_gpon_serialNumber_get(a) rtk_gponapp_serialNumber_get(a)
#define rtk_gpon_password_set(a) rtk_gponapp_password_set(a)
#define rtk_gpon_password_get(a) rtk_gponapp_password_get(a)
#define rtk_gpon_parameter_set(a,b) rtk_gponapp_parameter_set(a,b)
#define rtk_gpon_parameter_get(a,b) rtk_gponapp_parameter_get(a,b)
#define rtk_gpon_activate(a) rtk_gponapp_activate(a)
#define rtk_gpon_deActivate(a) rtk_gponapp_deActivate(a)
#define rtk_gpon_ponStatus_get(a) rtk_gponapp_ponStatus_get(a)
#define rtk_gpon_tcont_create(a,b) rtk_gponapp_tcont_create(a,b)
#define rtk_gpon_tcont_destroy_logical(a) rtk_gponapp_tcont_destroy_logical(a)
#define rtk_gpon_tcont_destroy(a) rtk_gponapp_tcont_destroy(a)
#define rtk_gpon_tcont_get(a,b) rtk_gponapp_tcont_get(a,b)
#define rtk_gpon_tcont_get_physical(a,b) rtk_gponapp_tcont_get_physical(a,b)
#define rtk_gpon_dsFlow_set(a,b) rtk_gponapp_dsFlow_set(a,b)
#define rtk_gpon_dsFlow_get(a,b) rtk_gponapp_dsFlow_get(a,b)
#define rtk_gpon_usFlow_set(a,b) rtk_gponapp_usFlow_set(a,b)
#define rtk_gpon_usFlow_get(a,b) rtk_gponapp_usFlow_get(a,b)
#define rtk_gpon_broadcastPass_set(a) rtk_gponapp_broadcastPass_set(a)
#define rtk_gpon_broadcastPass_get(a) rtk_gponapp_broadcastPass_get(a)
#define rtk_gpon_nonMcastPass_set(a) rtk_gponapp_nonMcastPass_set(a)
#define rtk_gpon_nonMcastPass_get(a) rtk_gponapp_nonMcastPass_get(a)
#define rtk_gpon_multicastAddrCheck_set(a,b) rtk_gponapp_multicastAddrCheck_set(a,b)
#define rtk_gpon_multicastAddrCheck_get(a,b) rtk_gponapp_multicastAddrCheck_get(a,b)
#define rtk_gpon_macFilterMode_set(a) rtk_gponapp_macFilterMode_set(a)
#define rtk_gpon_macFilterMode_get(a) rtk_gponapp_macFilterMode_get(a)
#define rtk_gpon_mcForceMode_set(a,b) rtk_gponapp_mcForceMode_set(a,b)
#define rtk_gpon_mcForceMode_get(a,b) rtk_gponapp_mcForceMode_get(a,b)
#define rtk_gpon_macEntry_add(a) rtk_gponapp_macEntry_add(a)
#define rtk_gpon_macEntry_del(a) rtk_gponapp_macEntry_del(a)
#define rtk_gpon_macEntry_get(a,b) rtk_gponapp_macEntry_get(a,b)
#define rtk_gpon_rdi_set(a) rtk_gponapp_rdi_set(a)
#define rtk_gpon_rdi_get(a) rtk_gponapp_rdi_get(a)
#define rtk_gpon_powerLevel_set(a) rtk_gponapp_powerLevel_set(a)
#define rtk_gpon_powerLevel_get(a) rtk_gponapp_powerLevel_get(a)
#define rtk_gpon_alarmStatus_get(a,b) rtk_gponapp_alarmStatus_get(a,b)
#define rtk_gpon_globalCounter_get(a,b) rtk_gponapp_globalCounter_get(a,b)
#define rtk_gpon_tcontCounter_get(a,b,c) rtk_gponapp_tcontCounter_get(a,b,c)
#define rtk_gpon_flowCounter_get(a,b,c) rtk_gponapp_flowCounter_get(a,b,c)
#define rtk_gpon_version_get(a,b) rtk_gponapp_version_get(a,b)
#define rtk_gpon_txForceLaser_set(a) rtk_gponapp_txForceLaser_set(a)
#define rtk_gpon_txForceLaser_get(a) rtk_gponapp_txForceLaser_get(a)
#define rtk_gpon_txForceIdle_set(a) rtk_gponapp_txForceIdle_set(a)
#define rtk_gpon_txForceIdle_get(a) rtk_gponapp_txForceIdle_get(a)
#define rtk_gpon_txForcePRBS_set(a) rtk_gponapp_txForcePRBS_set(a)
#define rtk_gpon_txForcePRBS_get(a) rtk_gponapp_txForcePRBS_get(a)
#define rtk_gpon_dsFecSts_get(a) rtk_gponapp_dsFecSts_get(a)
#define rtk_gpon_usFecSts_get(a) rtk_gponapp_usFecSts_get(a)
#define rtk_gpon_version_show(a) rtk_gponapp_version_show(a)
#define rtk_gpon_devInfo_show(a) rtk_gponapp_devInfo_show(a)
#define rtk_gpon_gtc_show(a) rtk_gponapp_gtc_show(a)
#define rtk_gpon_tcont_show(a) rtk_gponapp_tcont_show(a)
#define rtk_gpon_dsFlow_show(a) rtk_gponapp_dsFlow_show(a)
#define rtk_gpon_usFlow_show(a) rtk_gponapp_usFlow_show(a)
#define rtk_gpon_macTable_show(a) rtk_gponapp_macTable_show(a)
#define rtk_gpon_globalCounter_show(a) rtk_gponapp_globalCounter_show(a)
#define rtk_gpon_tcontCounter_show(a,b) rtk_gponapp_tcontCounter_show(a,b)
#define rtk_gpon_flowCounter_show(a,b) rtk_gponapp_flowCounter_show(a,b)
#define rtk_gpon_debug_set(a,b) rtk_gponapp_debug_set(a,b)
#define rtk_gpon_omci_tx(a) rtk_gponapp_omci_tx(a)
#define rtk_gpon_autoTcont_set(a) rtk_gponapp_autoTcont_set(a)
#define rtk_gpon_autoTcont_get(a) rtk_gponapp_autoTcont_get(a)
#define rtk_gpon_autoBoh_set(a) rtk_gponapp_autoBoh_set(a)
#define rtk_gpon_autoBoh_get(a) rtk_gponapp_autoBoh_get(a)
#define rtk_gpon_eqdOffset_set(a) rtk_gponapp_eqdOffset_set(a)
#define rtk_gpon_eqdOffset_get(a) rtk_gponapp_eqdOffset_get(a)
#define rtk_gpon_initial(a) rtk_gponapp_initial(a)
#define rtk_gpon_deinitial(a) rtk_gponapp_deinitial(a)
#define rtk_gpon_unit_test(a) rtk_gponapp_unit_test(a)
#define rtk_gpon_berInterval_get(a) rtk_gponapp_berInterval_get(a)
#define rtk_gpon_dbruBlockSize_get(a) rtk_gponapp_dbruBlockSize_get(a)
#define rtk_gpon_dbruBlockSize_set(a) rtk_gponapp_dbruBlockSize_set(a)
#define rtk_gpon_rogueOnt_set(a) rtk_gponapp_rogueOnt_set(a)
#define rtk_gpon_berTimerEnable_set(a) rtk_gponapp_berTimerEnable_set(a)
#define rtk_gpon_rogueSdCnt_get(a) rtk_gponapp_rogueSdCnt_get(a)
#define rtk_gpon_ppsCnt_readClear(a) rtk_gponapp_ppsCnt_readClear(a)
#define rtk_gpon_forceEmergencyStop_set(a) rtk_gponapp_forceEmergencyStop_set(a)
#define rtk_gpon_fec_get(a) rtk_gponapp_fec_get(a)
#endif /* CONFIG_LUNA_G3_SERIES */
#endif /*__LINUX_KERNEL__*/


#endif  /* __RTK_GPON_H__ */
