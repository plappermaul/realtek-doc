/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 61949 $
 * $Date: 2015-09-15 20:10:29 +0800 (Tue, 15 Sep 2015) $
 *
 * Purpose : Definition of Statistic API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Statistic Counter Reset
 *           (2) Statistic Counter Get
 *
 */

#ifndef __RT_STAT_H__
#define __RT_STAT_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/rt/rt_port.h>


/*
 * Symbol Definition
 */

#define RT_STAT_VLAN_CNT_FILTER_VID_INVAILD 0xffff
#define RT_STAT_VLAN_CNT_FILTER_PRI_INVAILD 0xff


/* port statistic counter index */
typedef enum rt_stat_port_type_e
{
    RT_IF_IN_OCTETS_INDEX = 0,                         /* RFC 2863 ifEntry */
    RT_IF_IN_UCAST_PKTS_INDEX,                         /* RFC 2863 ifEntry */
    RT_IF_IN_MULTICAST_PKTS_INDEX,                     /* RFC 2863 ifEntry */
    RT_IF_IN_BROADCAST_PKTS_INDEX,                     /* RFC 2863 ifEntry */
    RT_IF_IN_DISCARDS_INDEX,                           /* RFC 2863 ifEntry */
    RT_IF_OUT_OCTETS_INDEX,                            /* RFC 2863 ifEntry */
    RT_IF_OUT_DISCARDS_INDEX,                          /* RFC 2863 ifEntry */
    RT_IF_OUT_UCAST_PKTS_CNT_INDEX,                    /* RFC 2863 IfXEntry */
    RT_IF_OUT_MULTICAST_PKTS_CNT_INDEX,                /* RFC 2863 IfXEntry */
    RT_IF_OUT_BROADCAST_PKTS_CNT_INDEX,                /* RFC 2863 IfXEntry */
    RT_DOT1D_BASE_PORT_DELAY_EXCEEDED_DISCARDS_INDEX,  /* RFC 1493 Dot1dBasePortEntry */
    RT_DOT1D_TP_PORT_IN_DISCARDS_INDEX,                /* RFC 1493 */
    RT_DOT1D_TP_HC_PORT_IN_DISCARDS_INDEX,             /* RFC 2674 Dot1dTpHCPortEntry */
    RT_DOT3_IN_PAUSE_FRAMES_INDEX,                     /* RFC 2665 Dot3PauseEntry */
    RT_DOT3_OUT_PAUSE_FRAMES_INDEX,                    /* RFC 2665 Dot3PauseEntry */
    RT_DOT3_OUT_PAUSE_ON_FRAMES_INDEX,                 /* Proprietary counter */
    RT_DOT3_STATS_ALIGNMENT_ERRORS_INDEX,              /* RFC 2665 Dot3StatsEntry */
    RT_DOT3_STATS_FCS_ERRORS_INDEX,                    /* RFC 2665 Dot3StatsEntry */
    RT_DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX,       /* RFC 2665 Dot3StatsEntry */
    RT_DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX,     /* RFC 2665 Dot3StatsEntry */
    RT_DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX,        /* RFC 2665 Dot3StatsEntry */
    RT_DOT3_STATS_LATE_COLLISIONS_INDEX,               /* RFC 2665 Dot3StatsEntry */
    RT_DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX,          /* RFC 2665 Dot3StatsEntry */
    RT_DOT3_STATS_FRAME_TOO_LONGS_INDEX,               /* RFC 2665 Dot3StatsEntry */
    RT_DOT3_STATS_SYMBOL_ERRORS_INDEX,                 /* RFC 2665 Dot3StatsEntry */
    RT_DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX,          /* RFC 2665 Dot3ControlEntry */
    RT_ETHER_STATS_DROP_EVENTS_INDEX,                  /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_OCTETS_INDEX,                       /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_BROADCAST_PKTS_INDEX,               /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_MULTICAST_PKTS_INDEX,               /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_UNDER_SIZE_PKTS_INDEX,              /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_OVERSIZE_PKTS_INDEX,                /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_FRAGMENTS_INDEX,                    /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_JABBERS_INDEX,                      /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_COLLISIONS_INDEX,                   /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX,             /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_PKTS_64OCTETS_INDEX,                /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_PKTS_65TO127OCTETS_INDEX,           /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_PKTS_128TO255OCTETS_INDEX,          /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_PKTS_256TO511OCTETS_INDEX,          /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_PKTS_512TO1023OCTETS_INDEX,         /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX,        /* RFC 2819 EtherStatsEntry */
    RT_ETHER_STATS_TX_OCTETS_INDEX,                    /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX,           /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_OVERSIZE_PKTS_INDEX,             /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_PKTS_64OCTETS_INDEX,             /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX,        /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX,       /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX,       /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX,      /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX,     /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX,      /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_BROADCAST_PKTS_INDEX,            /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_MULTICAST_PKTS_INDEX,            /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_FRAGMENTS_INDEX,                 /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_JABBERS_INDEX,                   /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_TX_CRC_ALIGN_ERROR_INDEX,           /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    RT_ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX,           /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    RT_ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX,      /* Proprietary counter. */
    RT_ETHER_STATS_RX_OVERSIZE_PKTS_INDEX,             /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    RT_ETHER_STATS_RX_PKTS_64OCTETS_INDEX,             /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    RT_ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX,        /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    RT_ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX,       /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    RT_ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX,       /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    RT_ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX,      /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    RT_ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX,     /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    RT_ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX,      /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */

    RT_IN_OAM_PDU_PKTS_INDEX,                          /* Proprietary counter */
    RT_OUT_OAM_PDU_PKTS_INDEX,                         /* Proprietary counter */

    RT_IF_IN_ERR_OCTETS_INDEX,                         /* Proprietary counter */
    RT_IF_OUT_ERR_OCTETS_INDEX,                        /* Proprietary counter */
    
    RT_IF_IN_ERRORS_INDEX,                             /* RFC 2863 ifEntry */
    
    RT_MIB_PORT_CNTR_END
}rt_stat_port_type_t;

typedef enum rt_stat_vlanCnt_filter_type_e{
    RT_STAT_VLAN_CNT_FILTER_TYPE_FLOW,   /* Flow filter */
    RT_STAT_VLAN_CNT_FILTER_TYPE_PORT,   /* Port filter */
    RT_STAT_VLAN_CNT_FILTER_TYPE_END
}rt_stat_vlanCnt_filter_type_t;

typedef enum rt_stat_vlanCnt_filter_direction_e{
    RT_STAT_VLAN_CNT_FILTER_DIRECTION_RX,
    RT_STAT_VLAN_CNT_FILTER_DIRECTION_TX,
    RT_STAT_VLAN_CNT_FILTER_DIRECTION_END
}rt_stat_vlanCnt_filter_direction_t;


typedef enum rt_stat_vlanCnt_filter_pkt_type_e{
    RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_ALL,  /* All packet */
    RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_UC,   /* Unicast packet */
    RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_MC,   /* Multicast packet */
    RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_BC,   /* Broadcast packet */
    RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_END
}rt_stat_vlanCnt_pkt_type_t;


/*
 * Data Declaration
 */
/* port statistic counter structure */
typedef struct rt_stat_port_cntr_s
{
    uint64 ifInOctets;
    uint32 ifInUcastPkts;
    uint32 ifInMulticastPkts;
    uint32 ifInBroadcastPkts;
    uint32 ifInDiscards;
    uint64 ifOutOctets;
    uint32 ifOutDiscards;
    uint32 ifOutUcastPkts;
    uint32 ifOutMulticastPkts;
    uint32 ifOutBrocastPkts;
    uint32 dot1dBasePortDelayExceededDiscards;
    uint32 dot1dTpPortInDiscards;
    uint32 dot1dTpHcPortInDiscards;
    uint32 dot3InPauseFrames;
    uint32 dot3OutPauseFrames;
    uint32 dot3OutPauseOnFrames;
    uint32 dot3StatsAligmentErrors;
    uint32 dot3StatsFCSErrors;
    uint32 dot3StatsSingleCollisionFrames;
    uint32 dot3StatsMultipleCollisionFrames;
    uint32 dot3StatsDeferredTransmissions;
    uint32 dot3StatsLateCollisions;
    uint32 dot3StatsExcessiveCollisions;
    uint32 dot3StatsFrameTooLongs;
    uint32 dot3StatsSymbolErrors;
    uint32 dot3ControlInUnknownOpcodes;
    uint32 etherStatsDropEvents;
    uint64 etherStatsOctets;
    uint32 etherStatsBcastPkts;
    uint32 etherStatsMcastPkts;
    uint32 etherStatsUndersizePkts;
    uint32 etherStatsOversizePkts;
    uint32 etherStatsFragments;
    uint32 etherStatsJabbers;
    uint32 etherStatsCollisions;
    uint32 etherStatsCRCAlignErrors;
    uint32 etherStatsPkts64Octets;
    uint32 etherStatsPkts65to127Octets;
    uint32 etherStatsPkts128to255Octets;
    uint32 etherStatsPkts256to511Octets;
    uint32 etherStatsPkts512to1023Octets;
    uint32 etherStatsPkts1024to1518Octets;
    uint64 etherStatsTxOctets;
    uint32 etherStatsTxUndersizePkts;
    uint32 etherStatsTxOversizePkts;
    uint32 etherStatsTxPkts64Octets;
    uint32 etherStatsTxPkts65to127Octets;
    uint32 etherStatsTxPkts128to255Octets;
    uint32 etherStatsTxPkts256to511Octets;
    uint32 etherStatsTxPkts512to1023Octets;
    uint32 etherStatsTxPkts1024to1518Octets;
    uint32 etherStatsTxPkts1519toMaxOctets;
    uint32 etherStatsTxBcastPkts;
    uint32 etherStatsTxMcastPkts;
    uint32 etherStatsTxFragments;
    uint32 etherStatsTxJabbers;
    uint32 etherStatsTxCRCAlignErrors;
    uint32 etherStatsRxUndersizePkts;
    uint32 etherStatsRxUndersizeDropPkts;
    uint32 etherStatsRxOversizePkts;
    uint32 etherStatsRxPkts64Octets;
    uint32 etherStatsRxPkts65to127Octets;
    uint32 etherStatsRxPkts128to255Octets;
    uint32 etherStatsRxPkts256to511Octets;
    uint32 etherStatsRxPkts512to1023Octets;
    uint32 etherStatsRxPkts1024to1518Octets;
    uint32 etherStatsRxPkts1519toMaxOctets;
    uint32 inOampduPkts;
    uint32 outOampduPkts;
    uint64 ifInErrOctets;
    uint64 ifOutErrOctets;
    uint32 ifInErrors;
}rt_stat_port_cntr_t;




typedef struct rt_stat_vlanCnt_filter_s
{
    uint8 enable;
    rt_stat_vlanCnt_filter_direction_t direction;
    rt_stat_vlanCnt_filter_type_t type;
    uint16 portFlowId;
    uint16 vid;
    uint8 pri;
    rt_stat_vlanCnt_pkt_type_t pktType;
}rt_stat_vlanCnt_filter_t;



/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      rt_stat_init
 * Description:
 *      Initialize stat module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_PORT_CNTR_FAIL   - Could not retrieve/reset Port Counter
 * Note:
 *      Must initialize stat module before calling any stat APIs.
 */
extern int32
rt_stat_init(void);

/* Function Name:
 *      rt_stat_port_reset
 * Description:
 *      Reset the specified port counters in the specified device.
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
extern int32
rt_stat_port_reset(rt_port_t port);

/* Function Name:
 *      rt_stat_port_get
 * Description:
 *      Get one specified port counter.
 * Input:
 *      port     - port id
 *      cntrIdx - specified port counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
extern int32
rt_stat_port_get(rt_port_t port, rt_stat_port_type_t cntrIdx, uint64 *pCntr);


/* Function Name:
 *      rt_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pPortCntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
extern int32
rt_stat_port_getAll(rt_port_t port, rt_stat_port_cntr_t *pPortCntrs);


/* Function Name:
 *      rt_stat_vlanCnt_filter_set
 * Description:
 *      Set vlan counter filter 
 * Input:
 *      index        - filter index
 *      filter       - vlan counter filer configure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid input
 * Note:
 *      None
 */
extern int32
rt_stat_vlanCnt_filter_set(uint32 index, rt_stat_vlanCnt_filter_t filter);


/* Function Name:
 *      rt_stat_vlanCnt_filter_get
 * Description:
 *      Get vlan counter filter 
 * Input:
 *      index        - filter index
 * Output:
 *      pFilter       - vlan counter filer configure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid input
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rt_stat_vlanCnt_filter_get(uint32 index, rt_stat_vlanCnt_filter_t *pFilter);

/* Function Name:
 *      rt_stat_vlanCnt_mib_get
 * Description:
 *      Get vlan counters of one specified index
 * Input:
 *      index        - vlan counter index
 * Output:
 *      pPortCntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid index
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset vlan Counter
 * Note:
 *      None
 */
extern int32
rt_stat_vlanCnt_mib_get(uint32 index, rt_stat_port_cntr_t *pPortCntrs);

/* Function Name:
 *      rt_stat_vlanCnt_mib_reset
 * Description:
 *      Reset vlan counters of one specified index
 * Input:
 *      index        - vlan counter index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid index
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset vlan Counter
 * Note:
 *      None
 */
extern int32
rt_stat_vlanCnt_mib_reset(uint32 index);


#endif /* __RT_STAT_H__ */
