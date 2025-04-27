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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of the default values in the SDK
 *
 * Feature : Define the default values
 *
 */

#ifndef __RTK_DEFAULT_H__
#define __RTK_DEFAULT_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <rtk/trunk.h>
#include <rtk/vlan.h>
#include <rtk/stp.h>
#include <rtk/qos.h>
#include <rtk/l2.h>
#include <rtk/switch.h>
#include <rtk/pie.h>
#include <rtk/rate.h>
#include <rtk/l3.h>
#include <rtk/led.h>


/*
 * Symbol Definition
 */

/* VLAN module */
#define RTK_DEFAULT_VLAN_ID                         1
#define RTK_DEFAULT_VLAN_FUNCTION                   ENABLED
#define RTK_DEFAULT_VLAN_TAGMODE                    VLAN_TAG_MODE_ORIGINAL
#define RTK_DEFAULT_VLAN_IGNORE_INNER_TAG           DISABLED
#define RTK_DEFAULT_VLAN_IGNORE_OUTER_TAG           DISABLED
#define RTK_DEFAULT_VLAN_EGRESS_FILTER_ENABLE       ENABLED
#define RTK_DEFAULT_VLAN_SA_LRN_MODE                VLAN_SALRN_MODE_LEARN_BY_VLAN_MEMBER

/* STP module */
#define RTK_DEFAULT_MSTI                            0
#define RTK_DEFAULT_STP_PORT_STATE                  STP_STATE_FORWARDING

/* L2 module */
#define RTK_DEFAULT_L2_FLUSH_LINKDOWN_MAC           DISABLED
#define RTK_DEFAULT_L2_IP6_CACHE_TBL_SIZE           512
#define RTK_DEFAULT_L2_NON_UCAST_SA_ACTION          ACTION_DROP
#define RTK_DEFAULT_L2_NEW_SA_ACTION                ACTION_FORWARD
#define RTK_DEFAULT_L2_NEW_SA_LEARN_MODE            HARDWARE_LEARNING
#define RTK_DEFAULT_L2_CPU_NEW_SA_LEARN_MODE        NOT_LEARNING
#define RTK_DEFAULT_L2_IP4_MCAST_DIP_VALIDATION_STATE ENABLED
#define RTK_DEFAULT_L2_IP_MCAST_IVL_LOOKUP_STATE    ENABLED
#define RTK_DEFAULT_L2_IP6_MCAST_LOOKUP_SIP_CARE_BYTE 0x5210
#define RTK_DEFAULT_L2_IP6_MCAST_LOOKUP_DIP_CARE_BYTE 0x3210
#define RTK_DEFAULT_L2_SA_BLOCK_PORT_STATE          ENABLED
#define RTK_DEFAULT_L2_SA_BLOCK_CPU_PORT_STATE      DISABLED
#define RTK_DEFAULT_L2_DA_BLOCK_PORT_STATE          ENABLED
#define RTK_DEFAULT_L2_DA_BLOCK_CPU_PORT_STATE      DISABLED

#define RTK_DEFAULT_L2_STTC_PM_LRN_EN	DISABLED
#define RTK_DEFAULT_L2_NTFY_LD_FLUSH_EN	ENABLED
#define RTK_DEFAULT_L2_NTFY_DYN_EN		ENABLED


/* switch module */
#define RTK_DEFAULT_SWITCH_CPU_PORT_TX_MAX_LEN      0x600
#define RTK_DEFAULT_LED_POWER_ON_BLINK              RTK_LED_PWR_ON_BLINK_800MS

#if defined(CONFIG_SDK_RTL8390)
#define RTK_DEFAULT_L2_IP6_CACHE_IDX_TBL_SIZE       (16384+64)  /* Applicable to 8390 */
#else
#define RTK_DEFAULT_L2_IP6_CACHE_IDX_TBL_SIZE       (8192+64)   /* Applicable to 8380 */
#endif

/* L3 module */
#define RTK_DEFAULT_L3_INTF_VRF_ID                  (0)
#define RTK_DEFAULT_L3_INTF_VID                     (4095)      /* initial VID for a new interface which user doesn't assign any VID */
#define RTK_DEFAULT_L3_INTF_MTU                     (1500)
#define RTK_DEFAULT_L3_INTF_TTL_SCOPE               (0)         /* 0 means that all multicast packets are forwarded out the interface */
#if defined(CONFIG_SDK_RTL9310)
#define RTK_DEFAULT_L3_OPENFLOW_CUTLINE             (12288)     /* divide L3 table into L3 Prefix route and OpenFlow L3 CAM-based flow entries.
                                                                 * 0 means all the entries belong to OpenFlow L3 CAM-based flow entries.
                                                                 * 12288 means all the entries belong to L3 Prefix route.
                                                                 * Note: the value must be divisible by 6. (CUTLINE is equal to 6*N)
                                                                 */
#endif
#define RTK_DEFAULT_L3_NH_AGEOUT_ACT                ACTION_FORWARD

#define RTK_DEFAULT_L3_EXCEPTION_IP6MC_HDR_ROUTE_ACT		RTK_L3_ACT_FORWARD
#define RTK_DEFAULT_L3_EXCEPTION_IP6UC_DMAC_MISMATCH_ACT	RTK_L3_ACT_DROP
#define RTK_DEFAULT_L3_EXCEPTION_IPMC_HDR_OPT_ACT			RTK_L3_ACT_FORWARD
#define RTK_DEFAULT_L3_EXCEPTION_IP_HDR_ERR_ACT				RTK_L3_ACT_FORWARD
#define RTK_DEFAULT_L3_EXCEPTION_IP6_HDR_ERR_ACT			RTK_L3_ACT_FORWARD

#define RTK_DEFAULT_PARSER_PPPOE_EN							ENABLED
#define RTK_DEFAULT_MISC_CHIP_RST_EN						ENABLED
#define RTK_DEFAULT_MISC_WD_RST_EN							ENABLED

/* Tunnel module */
#define RTK_DEFAULT_TUNNEL_INTF_VRF_ID              (0)
#define RTK_DEFAULT_TUNNEL_INTF_TTL_SCOPE           (0)         /* 0 means that all multicast packets are forwarded out the interface */
#define RTK_DEFAULT_TUNNEL_INTF_ENCAP_MTU           (1460)
#define RTK_DEFAULT_TUNNEL_INTF_ENCAP_TTL           (0)

/* PORT module */
#define RTK_DEFAULT_PORT_AUTONEGO_ENABLE            ENABLED
#define RTK_DEFAULT_PORT_10HALF_CAPABLE             ENABLED
#define RTK_DEFAULT_PORT_10FULL_CAPABLE             ENABLED
#define RTK_DEFAULT_PORT_100HALF_CAPABLE            ENABLED
#define RTK_DEFAULT_PORT_100FULL_CAPABLE            ENABLED
#define RTK_DEFAULT_PORT_1000HALF_CAPABLE           ENABLED
#define RTK_DEFAULT_PORT_1000FULL_CAPABLE           ENABLED
#define RTK_DEFAULT_PORT_2_5G_CAPABLE               ENABLED
#define RTK_DEFAULT_PORT_5G_CAPABLE                 ENABLED
#define RTK_DEFAULT_PORT_10GHALF_CAPABLE            ENABLED
#define RTK_DEFAULT_PORT_10GFULL_CAPABLE            ENABLED
#define RTK_DEFAULT_PORT_10GBASET_CAPABLE           ENABLED
#define RTK_DEFAULT_PORT_PAUSE_CAPABILITY           ENABLED
#define RTK_DEFAULT_PORT_ASYPAUSE_CAPABILITY        DISABLED
#define RTK_DEFAULT_PORT_ISOLATION_PORTMASK         0x1FFFFFFF
#define RTK_DEFAULT_PORT_TX_ENABLE                  ENABLED
#define RTK_DEFAULT_PORT_RX_ENABLE                  ENABLED
#define RTK_DEFAULT_PORT_ADMIN_ENABLE               DISABLED
#define RTK_DEFAULT_PORT_VID                        RTK_DEFAULT_VLAN_ID

/* TRUNK module */
#define RTK_DEFAULT_TRUNK_MEMBER_PORTMASK           0
#define RTK_DEFAULT_TRUNK_DISTRIBUTION_BIND         0
#define RTK_DEFAULT_TRUNK_DISTRIBUTION_L2_BIND         0
#define RTK_DEFAULT_TRUNK_DISTRIBUTION_IPV4_BIND         0
#define RTK_DEFAULT_TRUNK_DISTRIBUTION_IPV6_BIND         0
#define RTK_DEFAULT_TRUNK_DISTRIBUTION_ALGORITHM    TRUNK_DISTRIBUTION_ALGO_MASKALL
#define RTK_DEFAULT_TRUNK_DISTRIBUTION_L2_ALGORITHM    TRUNK_DISTRIBUTION_ALGO_L2_MASKALL
#define RTK_DEFAULT_TRUNK_DISTRIBUTION_L3_ALGORITHM    TRUNK_DISTRIBUTION_ALGO_L3_MASKALL
#define RTK_DEFAULT_TRUNK_MODE                      TRUNK_MODE_STANDALONE
#define RTK_DEFAULT_TRUNK_STACK_MEMBER_PORTMASK           0
#define RTK_DEFAULT_TRUNK_STACK_DISTRIBUTION_L2_BIND         0
#define RTK_DEFAULT_TRUNK_STACK_DISTRIBUTION_IPV4_BIND     0
#define RTK_DEFAULT_TRUNK_STACK_DISTRIBUTION_IPV6_BIND     0
#define RTK_DEFAULT_TRUNK_STACK_TUNNEL_HASH_SRC         TUNNEL_HASH_SRC_INNER_HEADER
#define RTK_DEFAULT_TRUNK_STACK_HASH         STACK_TRK_HASH_KEEP
#define RTK_DEFAULT_TRUNK_LOCAL_FIRST         DISABLED

/* STACK module */
#define RTK_DEFAULT_STACK_PORTMASK_0           0
#define RTK_DEFAULT_STACK_PORTMASK_1           0
#define RTK_DEFAULT_STACK_LOCAL_UNIT            0
#define RTK_DEFAULT_STACK_MASTER_UNIT          0
#define RTK_DEFAULT_STACK_LOOP_GUARD           DISABLED
#define RTK_DEFAULT_STACK_NONUCAST_BLOCK_PORTMASK_0           0
#define RTK_DEFAULT_STACK_NONUCAST_BLOCK_PORTMASK_1           0


/* TRAP module */
#define RTK_DEFAULT_TRAP_RMA_SP_MCAST_ACTION        ACTION_FORWARD

/* FlowCtrl module */
#define RTK_DEFAULT_FC_HOL_PKT_BC_STATUS            DISABLED
#define RTK_DEFAULT_FC_HOL_PKT_L2_MC_STATUS         DISABLED
#define RTK_DEFAULT_FC_HOL_PKT_IP_MC_STATUS         DISABLED
#define RTK_DEFAULT_FC_HOL_PKT_UNKN_UC_STATUS       DISABLED

/* RATE module */
#define RTK_DEFAULT_EGR_BANDWIDTH_CTRL_STATUS       DISABLED
#define RTK_DEFAULT_EGR_BANDWIDTH_CTRL_RATE         0xFFFF
#define RTK_DEFAULT_EGR_BANDWIDTH_CPU_PORT_RATE_MODE     RATE_MODE_PKT
#define RTK_DEFAULT_EGR_BANDWIDTH_CPU_PORT_BURST    5
#define RTK_DEFAULT_EGR_BANDWIDTH_PORT_BURST        0x4000
#define RTK_DEFAULT_STORM_CONTROL_RATE_MODE         BASED_ON_PKT
#define RTK_DEFAULT_STORM_CONTROL_BURST_SIZE        0x00FF
#define RTK_DEFAULT_METER_IFG_INCLUDE_STATE         ENABLED

/* QoS module */
#define RTK_DEFAULT_QOS_QUEUE_NUMBER                8
#define RTK_DEFAULT_QOS_QUEUE_NUMBER_IN_CPU_PORT    8

#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_PORT          1
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_CLASS         1
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_ACL           4
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_DSCP          1
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_ITAG          2
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_OTAG          3
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_MAC_VLAN      1
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_PROTO_VLAN    1

#define RTK_DEFAULT_QOS_PORT_PRIORITY               0

#define RTK_DEFAULT_QOS_1P_PRIORITY0_REMAP          1
#define RTK_DEFAULT_QOS_1P_PRIORITY1_REMAP          0
#define RTK_DEFAULT_QOS_1P_PRIORITY2_REMAP          2
#define RTK_DEFAULT_QOS_1P_PRIORITY3_REMAP          3
#define RTK_DEFAULT_QOS_1P_PRIORITY4_REMAP          4
#define RTK_DEFAULT_QOS_1P_PRIORITY5_REMAP          5
#define RTK_DEFAULT_QOS_1P_PRIORITY6_REMAP          6
#define RTK_DEFAULT_QOS_1P_PRIORITY7_REMAP          7

#define RTK_DEFAULT_QOS_PRIORITY0_QUEUE             0
#define RTK_DEFAULT_QOS_PRIORITY1_QUEUE             1
#define RTK_DEFAULT_QOS_PRIORITY2_QUEUE             2
#define RTK_DEFAULT_QOS_PRIORITY3_QUEUE             3
#define RTK_DEFAULT_QOS_PRIORITY4_QUEUE             4
#define RTK_DEFAULT_QOS_PRIORITY5_QUEUE             5
#define RTK_DEFAULT_QOS_PRIORITY6_QUEUE             6
#define RTK_DEFAULT_QOS_PRIORITY7_QUEUE             7

#define RTK_DEFAULT_QOS_SCHED_CPU_ALGORITHM         WRR
#define RTK_DEFAULT_QOS_SCHED_ALGORITHM             WFQ
#define RTK_DEFAULT_QOS_SCHED_QUEUE_TYPE            WFQ_WRR_PRIORITY
#define RTK_DEFAULT_QOS_SCHED_QUEUE0_WEIGHT         1
#define RTK_DEFAULT_QOS_SCHED_QUEUE1_WEIGHT         1
#define RTK_DEFAULT_QOS_SCHED_QUEUE2_WEIGHT         1
#define RTK_DEFAULT_QOS_SCHED_QUEUE3_WEIGHT         1
#define RTK_DEFAULT_QOS_SCHED_QUEUE4_WEIGHT         1
#define RTK_DEFAULT_QOS_SCHED_QUEUE5_WEIGHT         1
#define RTK_DEFAULT_QOS_SCHED_QUEUE6_WEIGHT         1
#define RTK_DEFAULT_QOS_SCHED_QUEUE7_WEIGHT         1

#define RTK_DEFAULT_QOS_SCHED_LB_BYTE_PER_TOKEN     0x3B
#define RTK_DEFAULT_QOS_SCHED_LB_TICK_PERIOD        0x4B
#define RTK_DEFAULT_QOS_SCHED_LB_PREIFP             1
#define RTK_DEFAULT_QOS_SCHED_LB_WFQ_HIGH_THRESHOLD 0x300
#define RTK_DEFAULT_QOS_SCHED_LB_APR_BURST_SIZE     0xC000
#define RTK_DEFAULT_QOS_SCHED_LB_APR_RATE           0xFFFF

#define RTK_DEFAULT_QOS_REMAP_DEI0_TO_DP            0
#define RTK_DEFAULT_QOS_REMAP_DEI1_TO_DP            2

#define RTK_DEFAULT_QOS_REMARK_DEI_SOURCE           DEI_SEL_OUTER_TAG
#define RTK_DEFAULT_QOS_REMARK_DP0_TO_DEI           0
#define RTK_DEFAULT_QOS_REMARK_DP1_TO_DEI           1
#define RTK_DEFAULT_QOS_REMARK_DP2_TO_DEI           1

#define RTK_DEFAULT_QOS_AVB_NONA_RMK_PRI           0
#define RTK_DEFAULT_QOS_AVB_NONB_RMK_PRI           0





/* ACL module */
#define RTK_DEFAULT_ACL_PORT_LUT_EN                 ENABLED
#define RTK_DEFAULT_PIE_PHASE                       PIE_PHASE_VACL

/* NIC module */
/* Configure CPU Tag in packet descriptor or in packet raw data
 * ENABLED: in packet descriptor
 * DISABLED: in packet raw data
 */
#define RTK_DEFAULT_NIC_CPU_TAG_IN_DESCRIPTOR               ENABLED

/* Configure CPU Tag parser ability in cpu port
 * ENABLED: have parser cpu tag ability.
 * DISABLED: no parser cpu tag ability.
 */
#define RTK_DEFAULT_CPU_TAG_PARSER_ABILITY_IN_CPU_PORT      ENABLED

/* Configure insert all packets leaving CPU port with CPU tag
 * ENABLED: insert to raw data.
 * DISABLED: no insert.
 */
#define RTK_DEFAULT_CPU_TAG_INSERT_IN_LEAVING_CPU_PORT_PKT  ENABLED

/* RTK Link Monitor Thread Configuration
 */
#define RTK_DEFAULT_LINK_MON_POLLING_STACK_SIZE 4096
#define RTK_DEFAULT_LINK_MON_POLLING_THREAD_PRI 3
#define RTK_DEFAULT_LINK_MON_ISR_STACK_SIZE 4096
#define RTK_DEFAULT_LINK_MON_ISR_THREAD_PRI 0

/* RTK LOS Monitor Thread Configuration
 */
#define RTK_DEFAULT_LOS_MON_STACK_SIZE 4096
#define RTK_DEFAULT_LOS_MON_THREAD_PRI 3

/* RTK Workaround Monitor Thread Configuration
 */
#define RTK_DEFAULT_WA_MON_STACK_SIZE 4096
#define RTK_DEFAULT_WA_MON_THREAD_PRI 3

/* RTK Mac Constraint Workaround Monitor Thread Configuration
 */
#define RTK_DEFAULT_MAC_MON_STACK_SIZE 4096
#define RTK_DEFAULT_MAC_MON_THREAD_PRI 2

/*
 * Data Declaration
 */

/* ACL module */
#define RTK_DEFAULT_ACL_LOOKUP_EN                			ENABLED
#define RTK_DEFAULT_EGR_BANDWIDTH_ASSURE_RATE    			0x0
#define RTK_DEFAULT_QOS_PRI_DECISION_REMAP_EN				DISABLED
#define RTK_DEFAULT_TRUNK_LINK_DOWN_AVOID_EN				ENABLED
#define RTK_DEFAULT_TRUNK_LOCAL_FIRST_EN					ENABLED
#define RTK_DEFAULT_STP_LEAK_EN								DISABLED
#define RTK_DEFAULT_VLAN_CPU_PORT_IGR_FILTER                DISABLED
#define RTK_DEFAULT_VLAN_CPU_PORT_PVID_FMT                  PBVLAN_MODE_UNTAG_AND_PRITAG

/* EEE module */
#define RTK_DEFAULT_EEE_PORT_ENABLE_DEFAULT                 DISABLED


/*
 * Macro Definition
 */
#endif /* __RTK_DEFAULT_H__ */

