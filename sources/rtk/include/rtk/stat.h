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
 * Purpose : Definition of Statistic API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Statistic Counter Reset
 *           (2) Statistic Counter Get
 *
 */

#ifndef __RTK_STAT_H__
#define __RTK_STAT_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */
/* global statistic counter index */
typedef enum rtk_stat_global_type_e
{
    DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX = 0,
    MIB_GLOBAL_CNTR_END
} rtk_stat_global_type_t;


/* port statistic counter index */
typedef enum rtk_stat_port_type_e
{
     /*************************** Interface Group Mib(RFC2863) ************************/
    IF_IN_OCTETS_INDEX = 0,                         /* RFC 2863 ifEntry */
    IF_HC_IN_OCTETS_INDEX,                          /* RFC 2863 IfXEntry */
    IF_IN_UCAST_PKTS_INDEX,                         /* RFC 2863 ifEntry */
    IF_HC_IN_UCAST_PKTS_INDEX,                      /* RFC 2863 IfXEntry */
    IF_IN_MULTICAST_PKTS_INDEX,                     /* RFC 2863 ifEntry */
    IF_HC_IN_MULTICAST_PKTS_INDEX,                  /* RFC 2863 IfXEntry */
    IF_IN_BROADCAST_PKTS_INDEX,                     /* RFC 2863 ifEntry */
    IF_HC_IN_BROADCAST_PKTS_INDEX,                  /* RFC 2863 IfXEntry */
    IF_OUT_OCTETS_INDEX,                                        /* RFC 2863 ifEntry */
    IF_HC_OUT_OCTETS_INDEX,                                 /* RFC 2863 IfXEntry */
    IF_OUT_UCAST_PKTS_CNT_INDEX,                     /* RFC 2863 IfXEntry */
    IF_HC_OUT_UCAST_PKTS_INDEX,                         /* RFC 2863 IfXEntry */
    IF_OUT_MULTICAST_PKTS_CNT_INDEX,                /* RFC 2863 IfXEntry */
    IF_HC_OUT_MULTICAST_PKTS_INDEX,                 /* RFC 2863 IfXEntry */
    IF_OUT_BROADCAST_PKTS_CNT_INDEX,                /* RFC 2863 IfXEntry */
    IF_HC_OUT_BROADCAST_PKTS_INDEX,                 /* RFC 2863 IfXEntry */

    /*************************** TCP/IP-based MIB-II (RFC 1213) ***********************/
    IF_OUT_DISCARDS_INDEX,                          /* RFC 2863 ifEntry */

    /*************************** Ethernet-like MIB (RFC 3635) **************************/
    DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX,       /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX,         /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX,             /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_LATE_COLLISIONS_INDEX,                            /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX,                  /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_SYMBOL_ERRORS_INDEX,                             /* RFC 2665 Dot3StatsEntry */
    DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX,          /* RFC 2665 Dot3ControlEntry */
    DOT3_IN_PAUSE_FRAMES_INDEX,                                 /* RFC 2665 Dot3PauseEntry */
    DOT3_OUT_PAUSE_FRAMES_INDEX,                            /* RFC 2665 Dot3PauseEntry */

    /********************* RMON(Remote Network Monitoring) MIB (RFC 2819)  *************/
    ETHER_STATS_DROP_EVENTS_INDEX,                          /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_BROADCAST_PKTS_INDEX,                    /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_TX_BROADCAST_PKTS_INDEX,                /* Proprietary counter.  Count TX packets ONLY */
    ETHER_STATS_MULTICAST_PKTS_INDEX,                        /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_TX_MULTICAST_PKTS_INDEX,                 /* Proprietary counter. Count TX packets ONLY */

    ETHER_STATS_CRC_ALIGN_ERRORS_INDEX,                     /* RFC 2819 EtherStatsEntry */
    RX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX,               /* Proprietary counter.  Count RX packets ONLY */
    TX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX,              /* Proprietary counter */

    ETHER_STATS_UNDER_SIZE_PKTS_INDEX,                       /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX,                /* Proprietary counter. Count RX packets ONLY */
    ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX,                   /* Proprietary counter.  Count TX packets ONLY */

    ETHER_STATS_OVERSIZE_PKTS_INDEX,                            /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_RX_OVERSIZE_PKTS_INDEX,                      /* Proprietary counter. Count RX packets ONLY */
    ETHER_STATS_TX_OVERSIZE_PKTS_INDEX,                     /* Proprietary counter.  Count TX packets ONLY */

    ETHER_STATS_FRAGMENTS_INDEX,                                    /* RFC 2819 EtherStatsEntry */
    RX_ETHER_STATS_FRAGMENTS_INDEX,                             /* Proprietary counter */
    TX_ETHER_STATS_FRAGMENTS_INDEX,                              /* Proprietary counter */

    ETHER_STATS_JABBERS_INDEX,                                          /* RFC 2819 EtherStatsEntry */
    RX_ETHER_STATS_JABBERS_INDEX,                                   /* Proprietary counter */
    TX_ETHER_STATS_JABBERS_INDEX,                                   /* Proprietary counter */

    ETHER_STATS_COLLISIONS_INDEX,                                   /* RFC 2819 EtherStatsEntry */

    ETHER_STATS_PKTS_64OCTETS_INDEX,                         /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_RX_PKTS_64OCTETS_INDEX,                     /* Proprietary counter.  Count RX packets ONLY */
    ETHER_STATS_TX_PKTS_64OCTETS_INDEX,                     /* Proprietary counter.  Count TX packets ONLY */
    ETHER_STATS_PKTS_65TO127OCTETS_INDEX,                   /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX,             /* Proprietary counter.  Count RX packets ONLY */
    ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX,            /* Proprietary counter.  Count TX packets ONLY */
    ETHER_STATS_PKTS_128TO255OCTETS_INDEX,                  /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX,           /* Proprietary counter. Count RX packets ONLY */
    ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX,           /* Proprietary counter. Count TX packets ONLY */
    ETHER_STATS_PKTS_256TO511OCTETS_INDEX,                  /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX,           /* Proprietary counter.  Count RX packets ONLY */
    ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX,           /* Proprietary counter.  Count TX packets ONLY */
    ETHER_STATS_PKTS_512TO1023OCTETS_INDEX,                 /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX,          /* Proprietary counter.  Count RX packets ONLY */
    ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX,          /* Proprietary counter. Count TX packets ONLY */
    ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX,             /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX,     /* Proprietary counter.  Count RX packets ONLY */
    ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX,     /* Proprietary counter. Count TX packets ONLY */


    /*************************** Private MIB Counter ***************************/
    ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX,      /* Proprietary counter. */

    ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX,      /* Proprietary counter.  Count RX packets ONLY */
    ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX,      /* Proprietary counter. Count TX packets ONLY */
    ETHER_STATS_RX_PKTS_OVERMAXOCTETS_INDEX,            /* Proprietary counter.  Count RX packets ONLY */
    ETHER_STATS_TX_PKTS_OVERMAXOCTETS_INDEX,             /* Proprietary counter.  Count TX packets ONLY */

    RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX,  /* Proprietary counter */
    TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX,   /* Proprietary counter */
    RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX,   /* Proprietary counter */
    TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX,    /* Proprietary counter */

    RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX,  /* Proprietary counter */
    TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX,   /* Proprietary counter */
    RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX,   /* Proprietary counter */
    TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX,    /* Proprietary counter */

    RX_LENGTH_FIELD_ERROR_INDEX,                    /* Proprietary counter */
    RX_FALSE_CARRIER_TIMES_INDEX,                   /* Proprietary counter */
    RX_UNDER_SIZE_OCTETS_INDEX,                     /* Proprietary counter */
    RX_FRAMING_ERRORS_INDEX,                            /* Proprietary counter */

    RX_PARSER_ERROR_INDEX,

    RX_MAC_IPGSHORTDROP_INDEX,                      /* Proprietary counter */
    RX_MAC_DISCARDS_INDEX,                          /* Proprietary counter */
    TX_QUEUE_0_OUT_PKTS_INDEX,                      /* Proprietary counter - 9310 */
    TX_QUEUE_1_OUT_PKTS_INDEX,                      /* Proprietary counter - 9310 */
    TX_QUEUE_2_OUT_PKTS_INDEX,                      /* Proprietary counter - 9310 */
    TX_QUEUE_3_OUT_PKTS_INDEX,                      /* Proprietary counter - 9310 */
    TX_QUEUE_4_OUT_PKTS_INDEX,                      /* Proprietary counter - 9310 */
    TX_QUEUE_5_OUT_PKTS_INDEX,                      /* Proprietary counter - 9310 */
    TX_QUEUE_6_OUT_PKTS_INDEX,                      /* Proprietary counter - 9310 */
    TX_QUEUE_7_OUT_PKTS_INDEX,                      /* Proprietary counter - 9310 */
    TX_QUEUE_0_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300, 9310 */
    TX_QUEUE_1_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300, 9310 */
    TX_QUEUE_2_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300, 9310 */
    TX_QUEUE_3_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300, 9310 */
    TX_QUEUE_4_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300, 9310 */
    TX_QUEUE_5_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300, 9310 */
    TX_QUEUE_6_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300, 9310 */
    TX_QUEUE_7_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300, 9310 */
    TX_QUEUE_8_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_9_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_10_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_11_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_12_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_13_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_14_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_15_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_16_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_17_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_18_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_19_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_20_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_21_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_22_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_23_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_24_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_25_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_26_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_27_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_28_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_29_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_30_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */
    TX_QUEUE_31_DROP_PKTS_INDEX,                     /* Proprietary counter - 9300 */

    /*************************** Bridge MIB (RFC 1493) *********************************/
    DOT1D_TP_PORT_IN_DISCARDS_INDEX,                /* RFC 1493 */

    MIB_PORT_CNTR_END
} rtk_stat_port_type_t;

typedef enum rtk_stat_smon_type_e
{
    SMON_PRIO_STATS_PKTS = 0,
    SMON_PRIO_STATS_OCTETS,
    SMON_PRIO_STATS_END
} rtk_stat_smon_type_t;

/* tag counting index */
typedef enum rtk_stat_tagCnt_type_e
{
    TAG_CNT_TYPE_RX = 0,
    TAG_CNT_TYPE_TX,
    TAG_CNT_TYPE_END
} rtk_stat_tagCnt_type_t;

/* stacking header counting index */
typedef enum rtk_stat_stackHdrCnt_type_e
{
    STACK_HDR_CNT_TYPE_RX = 0,
    STACK_HDR_CNT_TYPE_TX,
    STACK_HDR_CNT_TYPE_END
} rtk_stat_stackHdrCnt_type_t;

/* global statistic counter structure */
typedef struct rtk_stat_global_cntr_s
{
    uint32 dot1dTpLearnedEntryDiscards;
} rtk_stat_global_cntr_t;

/* port statistic counter structure */
typedef struct rtk_stat_port_cntr_s
{
     /*************************** Interface Group Mib(RFC2863) ************************/
    uint64 ifInOctets;                                      /* RFC 2863  */
    uint64 ifHCInOctets;                                /* RFC 2863  */

    uint32 ifInUcastPkts;                               /* RFC 2863  */
#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint64 ifHCInUcastPkts;                           /* RFC 2863  */
#endif

    uint32 ifInMulticastPkts;                           /* RFC 2863 */
#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint64 ifHCInMulticastPkts;                       /* RFC 2863 */
#endif

    uint32 ifInBroadcastPkts;                           /* RFC 2863 */
#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint64 ifHCInBroadcastPkts;                       /* RFC 2863 */
#endif

    uint64 ifOutOctets;                                     /* RFC 2863  */
    uint64 ifHCOutOctets;                                 /* RFC 2863  */

    uint32 ifOutUcastPkts;                               /* RFC 2863  */
#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint64 ifHCOutUcastPkts;                           /* RFC 2863  */
#endif

    uint32 ifOutMulticastPkts;                          /* RFC 2863  */
#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint64 ifHCOutMulticastPkts;                           /* RFC 2863  */
#endif

    uint32 ifOutBrocastPkts;                            /* RFC 2863  */
#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint64 ifHCOutBrocastPkts;                             /* RFC 2863  */
#endif


     /*************************** TCP/IP-based MIB-II (RFC 1213) ***********************/
    uint32 ifOutDiscards;                           /* RFC 2863 ifEntry */



     /*************************** Ethernet-like MIB (RFC 3635) **************************/
    uint32 dot3StatsSingleCollisionFrames;          /* RFC 2665 Dot3PauseEntry */
    uint32 dot3StatsMultipleCollisionFrames;        /* RFC 2665 Dot3PauseEntry */
    uint32 dot3StatsDeferredTransmissions;          /* RFC 2665 Dot3PauseEntry */
    uint32 dot3StatsLateCollisions;                         /* RFC 2665 Dot3PauseEntry */
    uint32 dot3StatsExcessiveCollisions;                /* RFC 2665 Dot3PauseEntry */
    uint32 dot3StatsSymbolErrors;                          /* RFC 2665 Dot3PauseEntry */
    uint32 dot3ControlInUnknownOpcodes;             /* RFC 2665 Dot3PauseEntry */
    uint32 dot3InPauseFrames;                               /* RFC 2665 Dot3PauseEntry */
    uint32 dot3OutPauseFrames;                              /* RFC 2665 Dot3PauseEntry */


     /********************* RMON(Remote Network Monitoring) MIB (RFC 2819)  *************/
    uint32 etherStatsDropEvents;                    /* RFC 2819 EtherStatsEntry */

    uint32 etherStatsBroadcastPkts;                 /* RFC 2819 EtherStatsEntry */ //RX+TX
    uint32 etherStatsTxBroadcastPkts;               /* Proprietary counter */

    uint32 etherStatsMulticastPkts;                 /* RFC 2819 EtherStatsEntry */ //RX+TX
    uint32 etherStatsTxMulticastPkts;               /* Proprietary counter */

    uint32 etherStatsCRCAlignErrors;                /* RFC 2819 EtherStatsEntry */
#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint32 rxEtherStatsCRCAlignErrors;               /* Proprietary counter */
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint32 txEtherStatsCRCAlignErrors;               /* Proprietary counter */
#endif

    uint32 etherStatsUndersizePkts;                 /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsRxUndersizePkts;               /* Proprietary counter. Count RX packets ONLY */
    uint32 etherStatsTxUndersizePkts;               /* Proprietary counter. Count TX packets ONLY */

    uint32 etherStatsOversizePkts;                  /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsRxOversizePkts;                /* Proprietary counter. Count RX packets ONLY */
    uint32 etherStatsTxOversizePkts;                /* Proprietary counter. Count TX packets ONLY */

    uint32 etherStatsFragments;                     /* RFC 2819 EtherStatsEntry */
#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint32 rxEtherStatsFragments;                   /* Proprietary counter */
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint32 txEtherStatsFragments;                   /* Proprietary counter */
#endif

    uint32 etherStatsJabbers;                       /* RFC 2819 EtherStatsEntry */
#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint32 rxEtherStatsJabbers;                     /* Proprietary counter */
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint32 txEtherStatsJabbers;                     /* Proprietary counter */
#endif

    uint32 etherStatsCollisions;                            /* RFC 2819 EtherStatsEntry */ //(only TX)

    uint32 etherStatsPkts64Octets;                        /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsRxPkts64Octets;                    /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsTxPkts64Octets;                    /* RFC 2819 EtherStatsEntry */

    uint32 etherStatsPkts65to127Octets;               /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsRxPkts65to127Octets;           /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsTxPkts65to127Octets;           /* RFC 2819 EtherStatsEntry */

    uint32 etherStatsPkts128to255Octets;             /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsRxPkts128to255Octets;          /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsTxPkts128to255Octets;          /* RFC 2819 EtherStatsEntry */


    uint32 etherStatsPkts256to511Octets;              /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsRxPkts256to511Octets;          /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsTxPkts256to511Octets;          /* RFC 2819 EtherStatsEntry */

    uint32 etherStatsPkts512to1023Octets;            /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsRxPkts512to1023Octets;        /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsTxPkts512to1023Octets;         /* RFC 2819 EtherStatsEntry */

    uint32 etherStatsPkts1024to1518Octets;           /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsRxPkts1024to1518Octets;       /* RFC 2819 EtherStatsEntry */
    uint32 etherStatsTxPkts1024to1518Octets;        /* RFC 2819 EtherStatsEntry */


    /*************************** Private MIB Counter ***************************/
    uint32 etherStatsRxUndersizeDropPkts;                     /* Proprietary counter */
    uint32 etherStatsRxPkts1519toMaxOctets;                 /* Proprietary counter */
    uint32 etherStatsTxPkts1519toMaxOctets;                 /* Proprietary counter */

#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint32 etherStatsRxPktsOverMaxOctets;                   /* Proprietary counter */
    uint32 etherStatsTxPktsOverMaxOctets;                   /* Proprietary counter */

//    uint32 parserErrorsRT;                                              /* Proprietary counter */
    uint32 rxEtherStatsPktsFlexibleOctetsSet0RT;         /* Proprietary counter */
    uint32 txEtherStatsPktsFlexibleOctetsSet0RT;         /* Proprietary counter */
    uint32 rxEtherStatsPktsFlexibleOctetsCRCSet0RT;  /* Proprietary counter */
    uint32 txetherStatsPktsFlexibleOctetsCRCSet0RT;  /* Proprietary counter */

    uint32 rxEtherStatsPktsFlexibleOctetsSet1RT;        /* Proprietary counter */
    uint32 txEtherStatsPktsFlexibleOctetsSet1RT;         /* Proprietary counter */
    uint32 rxEtherStatsPktsFlexibleOctetsCRCSet1RT;  /* Proprietary counter */
    uint32 txetherStatsPktsFlexibleOctetsCRCSet1RT;  /* Proprietary counter */
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint32 rxLengthFieldError;                                      /* Proprietary counter */
    uint32 rxFalseCarrierTimes;                                    /* Proprietary counter */
    uint32 rxUnderSizeOctets;                                       /* Proprietary counter */
    uint32 rxFramingErrors;                                          /* Proprietary counter */
#endif

#if defined(CONFIG_SDK_RTL9300)||defined(CONFIG_SDK_RTL9310)
    uint32 parserErrors;                                                     /* Proprietary counter */
    uint32 rxMacIPGShortDrop;                                           /* Proprietary counter */
#endif

#if defined(CONFIG_SDK_RTL9310)
    uint32 egrQueue0OutPkts;                       /* Proprietary counter */
    uint32 egrQueue1OutPkts;                       /* Proprietary counter */
    uint32 egrQueue2OutPkts;                       /* Proprietary counter */
    uint32 egrQueue3OutPkts;                       /* Proprietary counter */
    uint32 egrQueue4OutPkts;                       /* Proprietary counter */
    uint32 egrQueue5OutPkts;                       /* Proprietary counter */
    uint32 egrQueue6OutPkts;                       /* Proprietary counter */
    uint32 egrQueue7OutPkts;                       /* Proprietary counter */
    uint32 egrQueue0DropPkts;                      /* Proprietary counter */
    uint32 egrQueue1DropPkts;                      /* Proprietary counter */
    uint32 egrQueue2DropPkts;                      /* Proprietary counter */
    uint32 egrQueue3DropPkts;                      /* Proprietary counter */
    uint32 egrQueue4DropPkts;                      /* Proprietary counter */
    uint32 egrQueue5DropPkts;                      /* Proprietary counter */
    uint32 egrQueue6DropPkts;                      /* Proprietary counter */
    uint32 egrQueue7DropPkts;                      /* Proprietary counter */
#endif

#if defined(CONFIG_SDK_RTL9300)
    uint32 egrQueueDropPkts[RTK_MAX_NUM_OF_QUEUE];                      /* Proprietary counter egrQueue0DropPkts ~ egrQueue31DropPkts*/
#endif

    uint32 rxMacDiscards;                            /* Proprietary counter */


    /*************************** Bridge MIB (RFC 1493) *********************************/
    uint32 dot1dTpPortInDiscards;                                    /* RFC 1493 */

    /* The end */
    /********************************************************************************/
} rtk_stat_port_cntr_t;

typedef struct rtk_stat_smon_cntr_s
{
    uint32 smonPrioStatsPkts;
    uint64 smonPrioStatsOctets;
} rtk_stat_smon_cntr_t;

typedef struct rtk_stat_flexCntSet_s
{
    uint32 len_min;
    uint32 len_max;
} rtk_stat_flexCntSet_t;

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      rtk_stat_init
 * Description:
 *      Initialize stat module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID               - invalid unit id
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_PORT_CNTR_FAIL   - Could not retrieve/reset Port Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize stat module before calling any stat APIs.
 * Changes:
 *      None
 */
extern int32
rtk_stat_init(uint32 unit);

/* Function Name:
 *      rtk_stat_enable_get
 * Description:
 *      Get the status to check whether MIB counters are enabled or not.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of enable or disable MIB counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_stat_enable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_stat_enable_set
 * Description:
 *      Set the status to enable or disable MIB counters
 * Input:
 *      unit   - unit id
 *      enable - enable/disable MIB counters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_stat_enable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_stat_global_reset
 * Description:
 *      Reset the global counters in the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID               - invalid unit id
 *      RT_ERR_NOT_INIT              - The module is not initial
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_stat_global_reset(uint32 unit);

/* Function Name:
 *      rtk_stat_port_reset
 * Description:
 *      Reset the specified port counters in the specified device.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_stat_port_reset(uint32 unit, rtk_port_t port);

/* Function Name:
 *      rtk_stat_global_get
 * Description:
 *      Get one specified global counter in the specified device.
 * Input:
 *      unit     - unit id
 *      cntr_idx - specified global counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported management frame is as following:
 *      rtk_stat_global_type_t \ Chip :             8390    8380    9300    9310
 *      - DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX     O       O       O       O
 * Changes:
 *      None
 */
extern int32
rtk_stat_global_get(uint32 unit, rtk_stat_global_type_t cntr_idx, uint64 *pCntr);

/* Function Name:
 *      rtk_stat_global_getAll
 * Description:
 *      Get all global counters in the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pGlobal_cntrs - pointer buffer of global counter structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_stat_global_getAll(uint32 unit, rtk_stat_global_cntr_t *pGlobal_cntrs);

/* Function Name:
 *      rtk_stat_port_get
 * Description:
 *      Get one specified port counter in the specified device.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      cntr_idx - specified port counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE           - input parameter out of range
 *      RT_ERR_INPUT                  - Invalid input parameter
 *      RT_ERR_STAT_INVALID_PORT_CNTR - Invalid Port Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported management frame is as following:
 *      - rtk_stat_port_type_t \ Chip:                  8390    8380    9300    9310
 *      - IF_IN_OCTETS_INDEX                            O       O       O       O
 *      - IF_HC_IN_OCTETS_INDEX                         O       O       O       O
 *      - IF_IN_UCAST_PKTS_INDEX                        O       O       O       O
 *      - IF_HC_IN_UCAST_PKTS_INDEX                     X       X       O       O
 *      - IF_IN_MULTICAST_PKTS_INDEX                    O       O       O       O
 *      - IF_HC_IN_MULTICAST_PKTS_INDEX                 X       X       O       O
 *      - IF_IN_BROADCAST_PKTS_INDEX                    O       O       O       O
 *      - IF_HC_IN_BROADCAST_PKTS_INDEX                 X       X       O       O
 *      - IF_OUT_OCTETS_INDEX                           O       O       O       O
 *      - IF_HC_OUT_OCTETS_INDEX                        O       O       O       O
 *      - IF_OUT_UCAST_PKTS_CNT_INDEX                   O       O       O       O
 *      - IF_HC_OUT_UCAST_PKTS_INDEX                    X       X       O       O
 *      - IF_OUT_MULTICAST_PKTS_CNT_INDEX               O       O       O       O
 *      - IF_HC_OUT_MULTICAST_PKTS_INDEX                X       X       O       O
 *      - IF_OUT_BROADCAST_PKTS_CNT_INDEX               O       O       O       O
 *      - IF_HC_OUT_BROADCAST_PKTS_INDEX                X       X       O       O
 *      - IF_OUT_DISCARDS_INDEX                         O       O       O       O
 *      - DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX      O       O       O       O
 *      - DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX    O       O       O       O
 *      - DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX       O       O       O       O
 *      - DOT3_STATS_LATE_COLLISIONS_INDEX              O       O       O       O
 *      - DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX         O       O       O       O
 *      - DOT3_STATS_SYMBOL_ERRORS_INDEX                O       O       O       O
 *      - DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX         O       O       O       O
 *      - DOT3_IN_PAUSE_FRAMES_INDEX                    O       O       O       O
 *      - DOT3_OUT_PAUSE_FRAMES_INDEX                   O       O       O       O
 *      - ETHER_STATS_DROP_EVENTS_INDEX                 O       O       O       O
 *      - ETHER_STATS_BROADCAST_PKTS_INDEX              O       O       O       O
 *      - ETHER_STATS_TX_BROADCAST_PKTS_INDEX           O       O       O       O
 *      - ETHER_STATS_MULTICAST_PKTS_INDEX              O       O       O       O
 *      - ETHER_STATS_TX_MULTICAST_PKTS_INDEX           O       O       O       O
 *      - ETHER_STATS_CRC_ALIGN_ERRORS_INDEX            O       O       O       O
 *      - RX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX         X       X       O       O
 *      - TX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX         O       X       O       O
 *      - ETHER_STATS_UNDER_SIZE_PKTS_INDEX             O       O       O       O
 *      - ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX          O       O       O       O
 *      - ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX          O       O       O       O
 *      - ETHER_STATS_OVERSIZE_PKTS_INDEX               O       O       O       O
 *      - ETHER_STATS_RX_OVERSIZE_PKTS_INDEX            O       O       O       O
 *      - ETHER_STATS_TX_OVERSIZE_PKTS_INDEX            O       O       O       O
 *      - ETHER_STATS_FRAGMENTS_INDEX                   O       O       O       O
 *      - RX_ETHER_STATS_FRAGMENTS_INDEX                X       X       O       O
 *      - TX_ETHER_STATS_FRAGMENTS_INDEX                O       X       O       O
 *      - ETHER_STATS_JABBERS_INDEX                     O       O       O       O
 *      - RX_ETHER_STATS_JABBERS_INDEX                  X       X       O       O
 *      - TX_ETHER_STATS_JABBERS_INDEX                  O       X       O       O
 *      - ETHER_STATS_COLLISIONS_INDEX                  O       O       O       O
 *      - ETHER_STATS_PKTS_64OCTETS_INDEX               O       O       O       O
 *      - ETHER_STATS_RX_PKTS_64OCTETS_INDEX            O       O       O       O
 *      - ETHER_STATS_TX_PKTS_64OCTETS_INDEX            O       O       O       O
 *      - ETHER_STATS_PKTS_65TO127OCTETS_INDEX          O       O       O       O
 *      - ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX       O       O       O       O
 *      - ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX       O       O       O       O
 *      - ETHER_STATS_PKTS_128TO255OCTETS_INDEX         O       O       O       O
 *      - ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX      O       O       O       O
 *      - ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX      O       O       O       O
 *      - ETHER_STATS_PKTS_256TO511OCTETS_INDEX         O       O       O       O
 *      - ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX      O       O       O       O
 *      - ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX      O       O       O       O
 *      - ETHER_STATS_PKTS_512TO1023OCTETS_INDEX        O       O       O       O
 *      - ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX     O       O       O       O
 *      - ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX     O       O       O       O
 *      - ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX       O       O       O       O
 *      - ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX    O       O       O       O
 *      - ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX    O       O       O       O
 *      - ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX     O       O       O       O
 *      - ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX     O       O       O       O
 *      - ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX     O       O       O       O
 *      - ETHER_STATS_RX_PKTS_OVERMAXOCTETS_INDEX       X       X       O       O
 *      - ETHER_STATS_TX_PKTS_OVERMAXOCTETS_INDEX       X       X       O       O
 *      - RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX     O       O       O       O
 *      - TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX     O       O       O       O
 *      - RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX X       X       O       O
 *      - TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX X       X       O       O
 *      - RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX     O       O       O       O
 *      - TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX     O       O       O       O
 *      - RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX X       X       O       O
 *      - TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX X       X       O       O
 *      - RX_LENGTH_FIELD_ERROR_INDEX                   O       X       O       O
 *      - RX_FALSE_CARRIER_TIMES_INDEX                  O       X       O       O
 *      - RX_UNDER_SIZE_OCTETS_INDEX                    O       X       O       O
 *      - RX_FRAMING_ERRORS_INDEX                       O       X       O       O
 *      - RX_PARSER_ERROR_INDEX                         X       X       O       O
 *      - RX_MAC_DISCARDS_INDEX                         O       O       O       O
 *      - DOT1D_TP_PORT_IN_DISCARDS_INDEX               O       O       O       O
 * Changes:
 *      None
 */
extern int32
rtk_stat_port_get(uint32 unit, rtk_port_t port, rtk_stat_port_type_t cntr_idx, uint64 *pCntr);

/* Function Name:
 *      rtk_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pPort_cntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_STAT_INVALID_PORT_CNTR - Invalid Port Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_stat_port_getAll(uint32 unit, rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs);

/* Function Name:
 *      rtk_stat_tagLenCntIncEnable_get
 * Description:
 *      Get RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      tagCnt_type - specified RX counter or TX counter
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Inner(4B) and outer(4B) tag length can be included or excluded to the counter through the API.
 * Changes:
 *      None
 */
extern int32
rtk_stat_tagLenCntIncEnable_get(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_stat_tagLenCntIncEnable_set
 * Description:
 *      Set RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      tagCnt_type - specified RX counter or TX counter
 *      enable      - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Inner(4B) and outer(4B) tag length can be included or excluded to the counter through the API.
 * Changes:
 *      None
 */
extern int32
rtk_stat_tagLenCntIncEnable_set(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t enable);

/* Function Name:
 *      rtk_stat_stackHdrLenCntIncEnable_get
 * Description:
 *      Get RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      type        - specified RX counter or TX counter
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      Stacking header length can be included or excluded to the counter through the API.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_stat_stackHdrLenCntIncEnable_get(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_stat_stackHdrLenCntIncEnable_set
 * Description:
 *      Set RX/TX counter to include or exclude stacking header length in the specified device.
 * Input:
 *      unit        - unit id
 *      type        - specified RX counter or TX counter
 *      enable      - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      Stacking header length can be included or excluded to the counter through the API.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_stat_stackHdrLenCntIncEnable_set(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t enable);

/* Function Name:
 *      rtk_stat_flexibleCntRange_get
 * Description:
 *      Get the flexible mib counter max/min boundary.
 * Input:
 *      unit        - unit id
 *      idx         - flexible mib counter set index
 * Output:
 *      pRange      - pointer buffer of the boundary value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      Per flexible counter MAX/MIN boundary value can be up to max frame length.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_stat_flexibleCntRange_get(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange);

/* Function Name:
 *      rtk_stat_flexibleCntRange_set
 * Description:
 *      Set the flexible mib counter max/min boundary.
 * Input:
 *      unit        - unit id
 *      idx         - flexible mib counter set index
 *      pRange      - pointer of the boundary value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      Per flexible counter MAX/MIN boundary value can be up to max frame length.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_stat_flexibleCntRange_set(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange);

#endif /* __RTK_STAT_H__ */
