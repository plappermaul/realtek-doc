/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * $Revision: 80316 $
 * $Date: 2017-07-03 13:42:26 +0800 (Mon, 03 Jul 2017) $
 *
 * Purpose : Realtek Switch SDK Rtdrv Netfilter Extension Module in the SDK.
 *
 * Feature : Realtek Switch SDK Rtdrv Netfilter Extension Module
 *
 */

#ifndef __RTDRV_NETFILTER_EXT_H__
#define __RTDRV_NETFILTER_EXT_H__
/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <private/drv/nic/nic_common.h>
#include <hal/mac/reg.h>
#include <hal/mac/mac_debug.h>
#include <rtk/l2.h>
#include <rtk/trap.h>
#include <rtk/port.h>
#include <rtk/vlan.h>
#include <rtk/init.h>
#include <rtk/stp.h>
#include <rtk/stat.h>
#include <rtk/trap.h>
#include <rtk/qos.h>
#include <rtk/trunk.h>
#include <rtk/mirror.h>
#include <rtk/flowctrl.h>
#include <rtk/rate.h>
#include <rtk/switch.h>
#include <rtk/oam.h>
#include <rtk/l3.h>
#include <rtk/eee.h>
#include <rtk/sec.h>
#include <rtk/pie.h>
#include <rtk/led.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <drv/nic/nic.h>


/*
 * Symbol Definition
 */
#define RTDRV_EXT_BASE_CTL                      (64+1024+64+64+9000+RTDRV_END_OFFSET+100)
#define RTDRV_EXT_REG_OFFSET                    1  /* 0 will be used by RTDRV_EXT_INIT_RTKAPI */
#define RTDRV_EXT_PORT_OFFSET                   (RTDRV_EXT_REG_OFFSET + 10)
#define RTDRV_EXT_MIB_OFFSET                    (RTDRV_EXT_PORT_OFFSET + 10)
#define RTDRV_EXT_SWITCH_OFFSET                 (RTDRV_EXT_MIB_OFFSET + 20)
#define RTDRV_EXT_QOS_OFFSET                    (RTDRV_EXT_SWITCH_OFFSET + 20)
#define RTDRV_EXT_EEE_OFFSET                    (RTDRV_EXT_QOS_OFFSET + 10)
#define RTDRV_EXT_IOL_OFFSET                    (RTDRV_EXT_EEE_OFFSET + 20)
#define RTDRV_EXT_MODEL_OFFSET                  (RTDRV_EXT_IOL_OFFSET + 20)
#define RTDRV_EXT_PKTGEN_OFFSET                 (RTDRV_EXT_MODEL_OFFSET + 20)
#define RTDRV_EXT_L2_OFFSET                     (RTDRV_EXT_PKTGEN_OFFSET + 40)
#define RTDRV_EXT_ACL_OFFSET                    (RTDRV_EXT_L2_OFFSET + 10)
#define RTDRV_EXT_TIME_OFFSET                   (RTDRV_EXT_ACL_OFFSET + 10)
#define RTDRV_EXT_NIC_OFFSET                    (RTDRV_EXT_TIME_OFFSET + 10)
#define RTDRV_EXT_END_OFFSET                    (RTDRV_EXT_NIC_OFFSET + 10)
#define SDK_CFG_ITEM                            32


/***** RTDRV_SET *****/
#define RTDRV_INIT_RTKAPI                       (RTDRV_BASE_CTL)

enum rtdrv_ext_spg_payload_type_e
{
    RTDRV_EXT_SPG_PAYLOAD_ZERO,
    RTDRV_EXT_SPG_PAYLOAD_INCR,
    RTDRV_EXT_SPG_PAYLOAD_FIX,
    RTDRV_EXT_SPG_PAYLOAD_REPEAT,
};

enum rtdrv_ext_spg_txcmd_e
{
    RTDRV_EXT_SPG_TXCMD_NOP,
    RTDRV_EXT_SPG_TXCMD_START,
    RTDRV_EXT_SPG_TXCMD_STOP,
    RTDRV_EXT_SPG_TXCMD_PAUSE,
};

enum rtdrv_ext_reg_set_e
{
    RTDRV_EXT_REG_FIELD_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_REG_OFFSET),
};

enum rtdrv_ext_port_set_e
{
    RTDRV_EXT_PORT_MAC_STATE_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_PORT_OFFSET),
    RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_SET,
    RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_HALF_SET,
    RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_SET,
    RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_HALF_SET,
    RTDRV_EXT_PORT_MACPOLLINGPHYSTATE_SET,
};

enum rtdrv_ext_mib_set_e
{
    RTDRV_EXT_MIB_DEBUG_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_MIB_OFFSET),
    RTDRV_EXT_MIB_RST_VAL_SET,
};

enum rtdrv_ext_switch_set_e
{
    RTDRV_EXT_SWITCH_48PASS1_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_SWITCH_OFFSET),
    RTDRV_EXT_SWITCH_LIMITPAUSE_SET,
    RTDRV_EXT_SWITCH_IPGCOMSTN_SET,
    RTDRV_EXT_SWITCH_IPGCOMSTN_SEL_SET,
    RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SET,
    RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SEL_SET,
    RTDRV_EXT_SWITCH_IPGMINLEN_SET,
    RTDRV_EXT_SWITCH_INTR_LINK_CHANGE_ENABLE_SET,
    RTDRV_EXT_SWITCH_INTR_LINK_CHANGE_DISABLE_SET,
    RTDRV_EXT_SWITCH_PASSALLMODE_SET,
    RTDRV_EXT_SWITCH_RXCHECKCRC_SET,
    RTDRV_EXT_SWITCH_BYPASSTXCRC_SET,
    RTDRV_EXT_SWITCH_BKPRES_SET,
    RTDRV_EXT_SWITCH_PADDINGUNDSIZE_SET,
};

enum rtdrv_ext_qos_set_e
{
    RTDRV_EXT_QOS_PORT_QUEUE_NUM_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_QOS_OFFSET),
    RTDRV_EXT_QOS_PORT_RATE_SET,
};

enum rtdrv_ext_eee_set_e
{
    RTDRV_EXT_EEE_TX_WAKE_MODE_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_EEE_OFFSET),
    RTDRV_EXT_EEE_LINK_UP_DELAY_SET,
    RTDRV_EXT_EEE_MULTI_WAKE_STATE_SET,
    RTDRV_EXT_EEE_MULTI_WAKE_INTERVAL_SET,
    RTDRV_EXT_EEE_MULTI_WAKE_PORT_NUM_SET,
    RTDRV_EXT_EEEP_TX_SLEEP_RATE_SET,
    RTDRV_EXT_EEEP_RX_SLEEP_RATE_SET,
};

enum rtdrv_ext_iol_set_e
{
    RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_IOL_OFFSET),
    RTDRV_EXT_IOL_ERROR_LENGTH_SET,
    RTDRV_EXT_IOL_INVALID_PAUSE_SET,
    RTDRV_EXT_IOL_LATE_COLLISION_SET,
    RTDRV_EXT_IOL_MAX_LENGTH_SET,
};

enum rtdrv_ext_model_set_e
{
    RTDRV_EXT_MODEL_TEST_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_MODEL_OFFSET),
};

enum rtdrv_ext_pktgen_set_e
{
    RTDRV_EXT_PKTGEN_TX_CMD_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_PKTGEN_OFFSET),
    RTDRV_EXT_PKTGEN_STATE_SET,
    RTDRV_EXT_PKTGEN_PORT_STATE_SET,
    RTDRV_EXT_PKTGEN_PORT_FRAG_PKT_ACTION_SET,
    RTDRV_EXT_PKTGEN_PORT_OVERSIZE_PKT_ACTION_SET,
    RTDRV_EXT_PKTGEN_PORT_UNDERSIZE_PKT_ACTION_SET,
    RTDRV_EXT_PKTGEN_PORT_BADCRC_PKT_ACTION_SET,
    RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_SET,
    RTDRV_EXT_PKTGEN_PORT_BADCRC_STATE_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_STATE_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_SA_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_DA_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_ETHTYPE_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_OTAG_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_OTAG_STATE_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_PAYLOAD_TYPE_SET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_PAYLOAD_PATTERN_SET,
    RTDRV_EXT_PKTGEN_PORT_TX,
    RTDRV_EXT_PKTGEN_PORT_TX_DYING_GASP,
};

enum rtdrv_ext_l2_set_e
{
    RTDRV_EXT_L2_AGING_UNIT_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_L2_OFFSET),
    RTDRV_EXT_L2_TBL_CLEAR,
    RTDRV_EXT_L2_CMA_ENABLE_SET,
};

enum rtdrv_ext_acl_set_e
{
    RTDRV_EXT_ACL_METER_COUNTER_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_ACL_OFFSET),
    RTDRV_EXT_ACL_METER_COUNTER_RESET,
};

enum rtdrv_ext_nic_set_e
{
    RTDRV_EXT_NIC_PKT_SEND_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_NIC_OFFSET),
};

enum rtdrv_ext_time_set_e
{
    RTDRV_EXT_TIME_PORT_TPID_SET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_TIME_OFFSET),
    RTDRV_EXT_TIME_PORT_MAC_SET,
    RTDRV_EXT_TIME_PORT_REF_TIME_FREQ_SET,
    RTDRV_EXT_TIME_PORT_INTERRUPT_SET,
};

#define RTDRV_EXT_SET_MAX                       (RTDRV_EXT_BASE_CTL + RTDRV_EXT_END_OFFSET)


/*Reg module*/
enum rtdrv_ext_reg_get_e
{
    RTDRV_EXT_REG_FIELD_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_REG_OFFSET),
};

enum rtdrv_ext_port_get_e
{
    RTDRV_EXT_PORT_MAC_STATE_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_PORT_OFFSET),
    RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_GET,
    RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_GET,
    RTDRV_EXT_PORT_MACPOLLINGPHYSTATE_GET,
};

enum rtdrv_ext_mib_get_e
{
    RTDRV_EXT_MIB_DEBUG_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_MIB_OFFSET),
    RTDRV_EXT_MIB_RST_VAL_GET,
};

enum rtdrv_ext_switch_get_e
{
    RTDRV_EXT_SWITCH_48PASS1_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_SWITCH_OFFSET),
    RTDRV_EXT_SWITCH_LIMITPAUSE_GET,
    RTDRV_EXT_SWITCH_INFO_GET,
    RTDRV_EXT_SWITCH_IPGCOMSTN_GET,
    RTDRV_EXT_SWITCH_IPGCOMSTN_SEL_GET,
    RTDRV_EXT_SWITCH_IPGCOMSTN_10G_GET,
    RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SEL_GET,
    RTDRV_EXT_SWITCH_IPGMINLEN_GET,
    RTDRV_EXT_SWITCH_PASSALLMODE_GET,
    RTDRV_EXT_SWITCH_RXCHECKCRC_GET,
    RTDRV_EXT_SWITCH_BYPASSTXCRC_GET,
    RTDRV_EXT_SWITCH_BKPRES_GET,
    RTDRV_EXT_SWITCH_PADDINGUNDSIZE_GET,
};

enum rtdrv_ext_qos_get_e
{
    RTDRV_EXT_QOS_PORT_QUEUE_NUM_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_QOS_OFFSET),
    RTDRV_EXT_QOS_PORT_RATE_GET,
};

enum rtdrv_ext_eee_get_e
{
    RTDRV_EXT_EEE_TX_WAKE_MODE_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_EEE_OFFSET),
    RTDRV_EXT_EEE_PORT_STATUS_GET,
    RTDRV_EXT_EEE_LINK_UP_DELAY_GET,
    RTDRV_EXT_EEE_MULTI_WAKE_STATE_GET,
    RTDRV_EXT_EEE_MULTI_WAKE_INTERVAL_GET,
    RTDRV_EXT_EEE_MULTI_WAKE_PORT_NUM_GET,
    RTDRV_EXT_EEEP_TX_SLEEP_RATE_GET,
    RTDRV_EXT_EEEP_RX_SLEEP_RATE_GET,
};

enum rtdrv_ext_iol_get_e
{
    RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_IOL_OFFSET),
    RTDRV_EXT_IOL_ERROR_LENGTH_GET,
    RTDRV_EXT_IOL_INVALID_PAUSE_GET,
    RTDRV_EXT_IOL_LATE_COLLISION_GET,
    RTDRV_EXT_IOL_MAX_LENGTH_GET,
};

enum rtdrv_ext_pktgen_get_e
{
    RTDRV_EXT_PKTGEN_TX_CMD_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_PKTGEN_OFFSET),
    RTDRV_EXT_PKTGEN_STATE_GET,
    RTDRV_EXT_PKTGEN_PORT_STATE_GET,
    RTDRV_EXT_PKTGEN_PORT_TX_DONE_STATE_GET,
    RTDRV_EXT_PKTGEN_PORT_FRAG_PKT_ACTION_GET,
    RTDRV_EXT_PKTGEN_PORT_OVERSIZE_PKT_ACTION_GET,
    RTDRV_EXT_PKTGEN_PORT_UNDERSIZE_PKT_ACTION_GET,
    RTDRV_EXT_PKTGEN_PORT_BADCRC_PKT_ACTION_GET,
    RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_GET,
    RTDRV_EXT_PKTGEN_PORT_BADCRC_STATE_GET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_GET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_STATE_GET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_GET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_GET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_GET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_GET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_GET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_GET,
    RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_GET,
};

enum rtdrv_ext_l2_get_e
{
    RTDRV_EXT_L2_AGING_UNIT_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_L2_OFFSET),
    RTDRV_EXT_L2_UC_SIZE_GET,
    RTDRV_EXT_L2_CMA_ENABLE_GET,
};

enum rtdrv_ext_acl_get_e
{
    RTDRV_EXT_ACL_METER_COUNTER_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_ACL_OFFSET),
};

enum rtdrv_ext_time_get_e
{
    RTDRV_EXT_TIME_PORT_PTP_TIMESTAMP_GET = (RTDRV_EXT_BASE_CTL + RTDRV_EXT_TIME_OFFSET),
    RTDRV_EXT_TIME_PORT_REF_TIME_GET,
    RTDRV_EXT_TIME_PORT_TPID_GET,
    RTDRV_EXT_TIME_PORT_MAC_GET,
    RTDRV_EXT_TIME_PORT_REF_TIME_FREQ_GET,
    RTDRV_EXT_TIME_PORT_INTERRUPT_GET
};

#define RTDRV_EXT_GET_MAX                       (RTDRV_EXT_BASE_CTL + RTDRV_EXT_END_OFFSET)

/*
 * Data Declaration
 */
typedef struct rtdrv_ext_portCfg_s
{
    uint32              unit;
    rtk_port_t          port;
    uint32              state;
    uint32              full_th;
    uint32              half_th;
    uint32              full_sec;
    uint32              half_sec;
} rtdrv_ext_portCfg_t;

typedef struct rtdrv_ext_mibCfg_s
{
    uint32  unit;
    uint32  rst_val;
} rtdrv_ext_mibCfg_t;

typedef struct rtdrv_ext_switchCfg_s
{
    uint32  unit;
    uint32  port;
    uint32  value;
    rtk_enable_t  enable;
    rtk_enable_t  half_48pass1;
    rtk_enable_t  limit_pause;
    rtk_enable_t  ipg_cmpstn;
    uint32  ipg_cmpstn_sel;
    rtk_enable_t  bkpres;
    rtk_enable_t  bypass_tx_crc;
    rtk_enable_t  rx_check_crc;
    rtk_enable_t  pass_all_mode;
    uint32  chip_info;
    uint32  model_char_1st;
    uint32  model_char_2nd;
    uint32  model_char_3rd;
    uint32  data;
    uint32  min_ipg;
} rtdrv_ext_switchCfg_t;

typedef struct rtdrv_ext_qosCfg_s
{
    uint32  unit;
    uint32  port;
    uint32  qnum;
    uint32  data;
} rtdrv_ext_qosCfg_t;

typedef struct rtdrv_ext_flowctrlCfg_s
{
    uint32                      unit;
    rtk_port_t                  port;
    rtk_qid_t                   queue;
    uint32                      used_page_cnt;
    uint32                      used_page_cnt_max;
} rtdrv_ext_flowctrlCfg_t;

typedef struct rtdrv_ext_eeeCfg_s
{
    uint32  unit;
    uint32  rate;
    uint32  linkUpDelay;
    uint32  interval;
    uint32  portNum;
    rtk_port_t  port;
    rtk_enable_t enable;
    rtk_eee_txWakeMode_t wakeMode;
    rtk_eee_power_state_t rxState;
    rtk_eee_power_state_t txState;
} rtdrv_ext_eeeCfg_t;

typedef struct rtdrv_ext_iolCfg_s
{
    uint32  unit;
    uint32  action;
    rtk_enable_t enable;
} rtdrv_ext_iolCfg_t;

typedef struct rtdrv_ext_modelCfg_s
{
    uint32  startID;
    uint32  endID;
    uint32  caredType;
} rtdrv_ext_modelCfg_t;

typedef struct rtdrv_ext_pktGenCfg_s
{
    uint32          unit;
    uint32          action;
    uint32          status;
    uint32          stream_idx;
    uint32          pktlen_start;
    uint32          pktlen_end;
    rtk_port_t      port;
    rtk_enable_t    enable;
    rtk_mac_t       da;
    rtk_mac_t       sa;
    uint16          etherType;
    uint32          vlanHdr;
    uint32          patternType;
    uint32          pattern;
    rtk_portmask_t  txPortmask;
    uint32          len;
} rtdrv_ext_pktGenCfg_t;

typedef struct rtdrv_ext_l2Cfg_s
{
    uint32  unit;
    uint32  aging_time;
    uint32  flag;
    uint32  data;
    uint32  enable;
} rtdrv_ext_l2Cfg_t;

typedef struct rtdrv_ext_aclCfg_s
{
    uint32  unit;
    uint32  index;
    uint32  counterUnit;
    uint32  greenCounter;
    uint32  yellowCounter;
    uint32  redCounter;
    uint32  totalCounter;
    uint32  clear;
} rtdrv_ext_aclCfg_t;

typedef struct rtdrv_ext_timeCfg_s
{
    uint32      unit;
    rtk_port_t  port;
    uint32      type;
    uint32      sid;
    uint32      sec;
    uint32      nsec;
    rtk_time_timeStamp_t timeStamp;
    rtk_vlanType_t  vlanType;
    uint32  tpid_idx;
    uint32  tpid;
    rtk_mac_t mac;
    uint32 range;
    uint32 freq;
    rtk_enable_t interruptEbl;
    uint32 interruptTrigger;
    uint32 interruptSts;
} rtdrv_ext_timeCfg_t;

typedef struct rtdrv_ext_nicSendCfg_s
{
    uint32      unit;
    rtk_mac_t   src_mac;
    rtk_mac_t   dst_mac;
    rtk_portmask_t txPortmask;
    uint8       isCpuTag;
    uint8       isTrunkHash;
} rtdrv_ext_nicSendCfg_t;

typedef union rtdrv_ext_union_u
{
    rtdrv_ext_portCfg_t   port_cfg;
    rtdrv_ext_mibCfg_t    mib_cfg;
    rtdrv_ext_switchCfg_t switch_cfg;
    rtdrv_ext_qosCfg_t    qos_cfg;
    rtdrv_ext_flowctrlCfg_t     flowctrl_cfg;
    rtdrv_ext_eeeCfg_t    eee_cfg;
    rtdrv_ext_iolCfg_t    iol_cfg;
    rtdrv_ext_modelCfg_t  model_cfg;
    rtdrv_ext_pktGenCfg_t pktgen_cfg;
    rtdrv_ext_l2Cfg_t     l2_cfg;
    rtdrv_ext_aclCfg_t    acl_cfg;
    rtdrv_ext_timeCfg_t   time_cfg;
    rtdrv_ext_nicSendCfg_t  nicSend_cfg;
} rtdrv_ext_union_t;

#endif /* __RTDRV_NETFILTER_EXT_H__ */
