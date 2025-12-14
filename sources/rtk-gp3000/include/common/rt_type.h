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
 * Purpose : Definition the basic types in the SDK.
 *
 * Feature : type definition
 *
 */

#ifndef __COMMON_RT_TYPE_H__
#define __COMMON_RT_TYPE_H__

/*
 * Include Files
 */
#include <common/type.h>
/*
 * Symbol Definition
 */
#define RTK_MAX_NUM_OF_PORTS                        32
#define RTK_MAX_PORT_ID                             (RTK_MAX_NUM_OF_PORTS - 1)
#define RTK_TOTAL_NUM_OF_BYTE_FOR_1BIT_PORT_LIST    ((RTK_MAX_NUM_OF_PORTS+7)/8)
#define RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST    ((RTK_MAX_NUM_OF_PORTS+31)/32)
#define RTK_MAX_NUM_OF_MIRRORING_PORT               1
#define RTK_MAX_NUM_OF_TRUNK_HASH_VAL               16
#define RTK_MAX_NUM_OF_SVLAN_ID                     4096
#define RTK_VLAN_ID_MIN                             0
#define RTK_VLAN_ID_MAX                             4095
#define RTK_EXT_VLAN_ID_MAX                         8191
#define RTK_ETHERTYPE_MAX                           0xFFFF
#define RTK_MAX_NUM_OF_QUEUE                        8
#define RTK_MAX_NUM_OF_PRIORITY                     8
#define RTK_VALUE_OF_DSCP_MAX                       63
#define RTK_VALUE_OF_DSCP_MIN                       0
#define RTK_VALUE_OF_TOS_TC_MAX                       255
#define RTK_VALUE_OF_TOS_TC_MIN                       0
#define RTK_DOT1P_PRIORITY_MAX                      7
#define RTK_DOT1P_DEI_MAX                           1
#define RTK_DROP_PRECEDENCE_MAX                     2
#define RTK_LINKMON_SCAN_INTERVAL_MIN               10000
#define RTK_WA_SCAN_INTERVAL_MIN                    500000
#define RTK_EEE_TX_SLEEP_RATE_FE_MAX                3125
#define RTK_EEE_TX_SLEEP_RATE_GE_MAX                31250
#define RTK_PRI_SEL_WEIGHT_MAX                      15
/*
 * Data Type Declaration
 */
typedef uint32  rtk_vlan_t;         /* vlan id type                     */
typedef uint32  rtk_fid_t;          /* filter id type                   */
typedef uint32  rtk_efid_t;         /* enhanced filter id type          */
typedef uint32  rtk_stg_t;          /* spanning tree instance id type   */
typedef uint32  rtk_port_t;         /* port is type                     */
typedef uint32  rtk_pri_t;          /* priority vlaue                   */
typedef uint32  rtk_qid_t;          /* queue id type                    */
typedef uint32  rtk_filter_id_t;    /* filter id type                   */
typedef uint32  rtk_pie_id_t;       /* PIE id type                      */
typedef uint32  rtk_acl_id_t;       /* ACL id type                      */
typedef uint32  rtk_dscp_t;         /* DSCP value                       */

typedef enum rtk_port_media_e
{
    PORT_MEDIA_COPPER = 0,
    PORT_MEDIA_FIBER,
    PORT_MEDIA_COPPER_AUTO,
    PORT_MEDIA_FIBER_AUTO,
    PORT_MEDIA_END
} rtk_port_media_t;

typedef enum rtk_flowctrl_patch_e
{
    FLOWCTRL_PATCH_20M = 0,
    FLOWCTRL_PATCH_35M_FIBER,
    FLOWCTRL_PATCH_35M_GPON,
    FLOWCTRL_PATCH_TCP_QOS_PON,
    FLOWCTRL_PATCH_DEFAULT,
    FLOWCTRL_PATCH_END
} rtk_flowctrl_patch_t;

typedef struct rtk_portmask_s
{
    uint32  bits[RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST];
} rtk_portmask_t;


#define RTK_MASK_MAX_LEN       128

typedef uint32                  rtk_bitmap_t;
#define RTK_BMP_BIT_LEN        32

#define RTK_BMP_WIDTH(_len)    (((_len) + RTK_BMP_BIT_LEN - 1) / RTK_BMP_BIT_LEN)

typedef struct rtk_bmp_s
{
    rtk_bitmap_t   bits[RTK_BMP_WIDTH(RTK_MASK_MAX_LEN)];
} rtk_bmp_t;



typedef struct rt_portType_info_s
{
    uint32  portNum;
    int32   max;    /* use (-1) for VALUE_NO_INIT */
    int32   min;    /* use (-1) for VALUE_NO_INIT */
    rtk_portmask_t  portmask;
} rt_portType_info_t;

/* frame type of protocol vlan - reference 802.1v standard */
typedef enum rtk_vlan_protoVlan_frameType_e
{
    FRAME_TYPE_ETHERNET = 0,
    FRAME_TYPE_RFC1042,
    FRAME_TYPE_SNAP8021H,/* Not supported by any chip */
    FRAME_TYPE_SNAPOTHER,/* Not supported by any chip */
    FRAME_TYPE_LLCOTHER,
    FRAME_TYPE_END
} rtk_vlan_protoVlan_frameType_t;

/* Protocol-and-port-based Vlan structure */
typedef struct rtk_vlan_protoAndPortInfo_s
{
    uint32                          proto_type;
    rtk_vlan_protoVlan_frameType_t  frame_type;
    rtk_vlan_t                      cvid;
    rtk_pri_t                       cpri;
}rtk_vlan_protoAndPortInfo_t;


#define     IPV6_ADDR_LEN   16
typedef uint32 rtk_ip_addr_t;
typedef struct rtk_ipv6_addr_s
{
    uint8   ipv6_addr[IPV6_ADDR_LEN];
} rtk_ipv6_addr_t;

typedef enum rtk_ip_family_e
{
    IPV4_FAMILY,
    IPV6_FAMILY,
    IP_FAMILY_END
} rtk_ip_family_t;

typedef enum rtk_l4Proto_e
{
    L4PROTO_TCP = 0,
    L4PROTO_UDP,
    L4PROTO_END
} rtk_l4Proto_t;

typedef enum rtk_action_e
{
    ACTION_FORWARD = 0,
    ACTION_DROP,
    ACTION_TRAP2CPU,
    ACTION_COPY2CPU,
    ACTION_TO_GUESTVLAN,
    ACTION_FLOOD_IN_VLAN,
    ACTION_FLOOD_IN_ALL_PORT,
    ACTION_FLOOD_IN_ROUTER_PORTS,
    ACTION_FORWARD_EXCLUDE_CPU,
    ACTION_DROP_EXCLUDE_RMA,
    ACTION_FOLLOW_FB,
    ACTION_END
} rtk_action_t;


#define SNAPOUI_LEN     3
typedef struct rtk_snapOui_s
{
    uint8   snapOui[SNAPOUI_LEN];
} rtk_snapOui_t;

typedef enum rtk_pktType_e
{
    ETHERNET_II_PACKET = 0,
    SNAP_PACKET,
    PKT_TYPE_END
} rtk_pktType_t;

typedef enum rtk_vlanType_e
{
    INNER_VLAN = 0,
    OUTER_VLAN,
    VLAN_TYPE_END
} rtk_vlanType_t;

typedef enum rtk_snapMode_e
{
    SNAP_MODE_AAAA03000000 = 0,         /* compare 0xAAAA03000000 */
    SNAP_MODE_AAAA03,                   /* compare 0xAAAA03 */
    SNAP_MODE_END
} rtk_snapMode_t;

typedef enum rtk_pktFormat_e
{
    ORIGINAL_PACKET = 0,
    MODIFIED_PACKET,
    PKT_FORMAT_END
} rtk_pktFormat_t;

typedef enum rtk_direct_e
{
    DIRECT_UPSTREAM = 0,
    DIRECT_DOWNSTREAM,
    DIRECT_END
} rtk_direct_t;

typedef enum rtk_stpid_act_e
{
    US_CSACT_NOP = 0,
    US_CSACT_VS_TPID,
    US_CSACT_8100,
    US_CSACT_END
} rtk_us_csact_t;

typedef enum rtk_us_cact_e
{
    US_CACT_NOP = 0,
    US_CACT_UNTAG,
    US_CACT_C2S,
    US_CACT_TRANSPARENT,
    US_CACT_END
} rtk_us_cact_t;

typedef enum rtk_us_act_e
{
    US_SACT_ASSIGN = 0,
    US_SACT_COPY_C,
    US_SACT_INTERNAL,
    US_SACT_END
} rtk_us_act_t;

typedef enum rtk_us_id_act_e
{
    US_ID_SID = 0,
    US_ID_QID,
    US_ID_END
} rtk_us_id_act_t;

typedef enum rtk_ds_csact_e
{
    DS_CSACT_NOP = 0,
    DS_CSACT_VS_TPID,
    DS_CSACT_8100,
    DS_CSACT_DEL,
    DS_CSACT_END
} rtk_ds_csact_t;

typedef enum rtk_ds_cact_e
{
    DS_CACT_NOP = 0,
    DS_CACT_TAG,
    DS_CACT_SP2C,
    DS_CACT_TRANSPARENT,
    DS_CACT_END
} rtk_ds_cact_t;

typedef enum rtk_ds_act_e
{
    DS_ACT_SWCORE = 0,
    DS_ACT_COPY_S,
    DS_ACT_ASSIGN,
    DS_ACT_LUT,
    DS_ACT_END
} rtk_ds_act_t;

typedef enum rtk_ds_cspri_act_e
{
    DS_CFPRI_SWCORE = 0,
    DS_CFPRI_ASSIGN,
    DS_CFPRI_END
} rtk_ds_cfpri_act_t;

typedef enum rtk_ds_uni_act_e
{
    DS_UNI_FWD = 0,
    DS_UNI_FS_FWD,
    DS_UNI_END
} rtk_ds_uni_act_t;

typedef enum rtk_cpu_tag_fmt_e
{
    CPU_TAG_FMT_APOLLO = 0,
    CPU_TAG_FMT_NORMAL,
    CPU_TAG_FMT_END
} rtk_cpu_tag_fmt_t;

/*RT definition*/
extern uint32 rt_port0;
extern uint32 rt_port1;
extern uint32 rt_port2;
extern uint32 rt_port3;
extern uint32 rt_port4;
extern uint32 rt_port5;
extern uint32 rt_port6;
extern uint32 rt_port7;
extern uint32 rt_port_pon;
extern uint32 rt_port_cpu0;
extern uint32 rt_port_cpu1;
extern uint32 rt_port_cpu2;
extern uint32 rt_port_cpu3;
extern uint32 rt_port_cpu4;
extern uint32 rt_port_cpu5;
extern uint32 rt_port_cpu6;
extern uint32 rt_port_cpu7;
extern uint32 rt_port_max;

extern uint32 rt_port_phy_port0_bit;
extern uint32 rt_port_phy_port1_bit;
extern uint32 rt_port_phy_port2_bit;
extern uint32 rt_port_phy_port3_bit;
extern uint32 rt_port_phy_port4_bit;
extern uint32 rt_port_phy_pon_bit;

extern uint32 rt_port_phy_port0;
extern uint32 rt_port_phy_port1;
extern uint32 rt_port_phy_port2;
extern uint32 rt_port_phy_port3;
extern uint32 rt_port_phy_port4;
extern uint32 rt_port_phy_pon;

#define RT_PORT0        (rt_port0)
#define RT_PORT1        (rt_port1)
#define RT_PORT2        (rt_port2)
#define RT_PORT3        (rt_port3)
#define RT_PORT4        (rt_port4)
#define RT_PORT5        (rt_port5)
#define RT_PORT6        (rt_port6)
#define RT_PORT7        (rt_port7)
#define RT_PORT_PON     (rt_port_pon)
#define RT_PORT_CPU0    (rt_port_cpu0)
#define RT_PORT_CPU1    (rt_port_cpu1)
#define RT_PORT_CPU2    (rt_port_cpu2)
#define RT_PORT_CPU3    (rt_port_cpu3)
#define RT_PORT_CPU4    (rt_port_cpu4)
#define RT_PORT_CPU5    (rt_port_cpu5)
#define RT_PORT_CPU6    (rt_port_cpu6)
#define RT_PORT_CPU7    (rt_port_cpu7)
#define RT_PORT_MAX     (rt_port_max)

#define RT_PORT_PHY_PORT_PORT0_BIT  (rt_port_phy_port0_bit)
#define RT_PORT_PHY_PORT_PORT1_BIT  (rt_port_phy_port1_bit)
#define RT_PORT_PHY_PORT_PORT2_BIT  (rt_port_phy_port2_bit)
#define RT_PORT_PHY_PORT_PORT3_BIT  (rt_port_phy_port3_bit)
#define RT_PORT_PHY_PORT_PORT4_BIT  (rt_port_phy_port4_bit)
#define RT_PORT_PHY_PORT_PON_BIT    (rt_port_phy_pon_bit)

#define RT_PORT_PHY_PORT_PORT0      (rt_port_phy_port0)
#define RT_PORT_PHY_PORT_PORT1      (rt_port_phy_port1)
#define RT_PORT_PHY_PORT_PORT2      (rt_port_phy_port2)
#define RT_PORT_PHY_PORT_PORT3      (rt_port_phy_port3)
#define RT_PORT_PHY_PORT_PORT4      (rt_port_phy_port4)
#define RT_PORT_PHY_PORT_PON        (rt_port_phy_pon)

#define RT_PORT_INVALID             0xffffffff


#define rt_port_t                   uint32
#define rt_phy_port_t               uint32
#define rt_port_phy_port_mask_t     uint32
#define rt_port_phy_port_index_t    uint32
#define rt_vlan_t                   uint32

typedef struct rt_portmask_s
{
    uint32  bits[RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST];
} rt_portmask_t;

typedef enum rt_action_e
{
    RT_ACTION_FORWARD = 0,
    RT_ACTION_DROP,
    RT_ACTION_TRAP2CPU,
    RT_ACTION_COPY2CPU,
    RT_ACTION_TO_GUESTVLAN,
    RT_ACTION_FLOOD_IN_VLAN,
    RT_ACTION_FLOOD_IN_ALL_PORT,
    RT_ACTION_FLOOD_IN_ROUTER_PORTS,
    RT_ACTION_FORWARD_EXCLUDE_CPU,
    RT_ACTION_DROP_EXCLUDE_RMA,
    RT_ACTION_FOLLOW_FB,
    RT_ACTION_END
} rt_action_t;

typedef struct rt_ipv6_addr_s
{
    uint8   ipv6_addr[IPV6_ADDR_LEN];
} rt_ipv6_addr_t;
/*RT definition*/

#endif /* __COMMON_RT_TYPE_H__ */

