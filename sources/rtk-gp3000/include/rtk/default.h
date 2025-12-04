/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
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
#include <common/rt_type.h>

/*
 * Symbol Definition
 */

/* VLAN module */
#define RTK_DEFAULT_VLAN_ID                         1

/* STP module */
#define RTK_DEFAULT_MSTI                            0
#define RTK_DEFAULT_STP_PORT_STATE                  STP_STATE_FORWARDING

/* L2 module */
#define RTK_DEFAULT_L2_FLUSH_LINKDOWN_MAC           DISABLED

/* PORT module */
#define RTK_DEFAULT_PORT_AUTONEGO_ENABLE            ENABLED
#define RTK_DEFAULT_PORT_10HALF_CAPABLE             ENABLED
#define RTK_DEFAULT_PORT_10FULL_CAPABLE             ENABLED
#define RTK_DEFAULT_PORT_100HALF_CAPABLE            ENABLED
#define RTK_DEFAULT_PORT_100FULL_CAPABLE            ENABLED
#define RTK_DEFAULT_PORT_1000HALF_CAPABLE           ENABLED
#define RTK_DEFAULT_PORT_1000FULL_CAPABLE           ENABLED
#define RTK_DEFAULT_PORT_PAUSE_CAPABILITY           ENABLED
#define RTK_DEFAULT_PORT_ASYPAUSE_CAPABILITY        DISABLED
#define RTK_DEFAULT_PORT_ISOLATION_PORTMASK         0x1FFFFFFF
#define RTK_DEFAULT_PORT_TX_ENABLE                  ENABLED
#define RTK_DEFAULT_PORT_RX_ENABLE                  ENABLED
#define RTK_DEFAULT_PORT_ADMIN_ENABLE               DISABLED
#define RTK_DEFAULT_PORT_VID                        RTK_DEFAULT_VLAN_ID

/* TRUNK module */
#define RTK_DEFAULT_TRUNK_MEMBER_PORTMASK           0
#define RTK_DEFAULT_TRUNK_DISTRIBUTION_ALGORITHM    0

/* RATE module */
#define RTK_DEFAULT_EGR_BANDWIDTH_CTRL_STATUS       DISABLED
#define RTK_DEFAULT_EGR_BANDWIDTH_CTRL_RATE         0xFFFF

/* FILTER module */
#ifdef CONFIG_SDK_FPGA_PLATFORM
#define RTK_DEFAULT_CUTLINE                         1
#else
#define RTK_DEFAULT_CUTLINE                         4
#endif
#define RTK_DEFAULT_PIE_STATUS                      ENABLED


/* QoS module */
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_PORT      3
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_DOT1P     0
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_DSCP      0
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_ACL       0
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_FORWARD   0
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_SA        0
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_VLAN      0
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_SVLAN     0
#define RTK_DEFAULT_QOS_SELECTION_PRIORITY_OF_L4        0

 
#define RTK_DEFAULT_QOS_PORT_PRIORITY               0
 
#define RTK_DEFAULT_QOS_1P_PRIORITY0_REMAP          0
#define RTK_DEFAULT_QOS_1P_PRIORITY1_REMAP          1
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
 
#define RTK_DEFAULT_QOS_SCHED_QUEUE0_WEIGHT         1
#define RTK_DEFAULT_QOS_SCHED_QUEUE1_WEIGHT         2
#define RTK_DEFAULT_QOS_SCHED_QUEUE2_WEIGHT         3
#define RTK_DEFAULT_QOS_SCHED_QUEUE3_WEIGHT         4
#define RTK_DEFAULT_QOS_SCHED_QUEUE4_WEIGHT         5
#define RTK_DEFAULT_QOS_SCHED_QUEUE5_WEIGHT         6
#define RTK_DEFAULT_QOS_SCHED_QUEUE6_WEIGHT         7
#define RTK_DEFAULT_QOS_SCHED_QUEUE7_WEIGHT         8

#define RTK_DEFAULT_QOS_SCHED_LB_BYTE_PER_TOKEN     0x3B
#define RTK_DEFAULT_QOS_SCHED_LB_TICK_PERIOD        0x4B
#define RTK_DEFAULT_QOS_SCHED_LB_PREIFP             1
#define RTK_DEFAULT_QOS_SCHED_LB_WFQ_HIGH_THRESHOLD 0x300
#define RTK_DEFAULT_QOS_SCHED_LB_APR_BURST_SIZE     0xC000
#define RTK_DEFAULT_QOS_SCHED_LB_APR_RATE           0xFFFF

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
#define RTK_DEFAULT_LINK_MON_STACK_SIZE 4096
#define RTK_DEFAULT_LINK_MON_THREAD_PRI 3

/* RTK Workaround Monitor Thread Configuration
 */
#define RTK_DEFAULT_WA_MON_STACK_SIZE 4096
#define RTK_DEFAULT_WA_MON_THREAD_PRI 3

/*
 * Data Declaration
 */


/*
 * Macro Definition
 */


#endif /* __RTK_DEFAULT_H__ */

