/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 43213 $
 * $Date: 2013-09-27 15:59:36 +0800 (Fri, 27 Sep 2013) $
 *
 * Purpose : Definition of Statistic API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Statistic Counter Reset
 *           (2) Statistic Counter Get
 *
 */

#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/ca8279/dal_ca8279.h>
#include <dal/ca8279/dal_ca8279_stat.h>
#include <osal/time.h>

#include <cortina-api/include/port.h>
#include <cortina-api/include/eth_port.h>
#include <aal_ni_l2.h>
#include <aal_l2_te.h>
#include <aal_l3_te.h>

/*
 * Include Files
 */

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32    stat_init = INIT_NOT_COMPLETED;

static dal_port_cntr_t lanPortCntrs[17];

/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      dal_ca8279_stat_init
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
int32
dal_ca8279_stat_init(void)
{
    int32 ret;
    rtk_port_t port;

    stat_init = INIT_COMPLETED;

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_ca8279_stat_port_reset(port)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_ca8279_stat_port_reset
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
int32
dal_ca8279_stat_port_reset(rtk_port_t port)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t read_keep;
    ca_eth_port_stats_t data;

    /* check Init status */
    RT_INIT_CHK(stat_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    
    if(port == AAL_NI_PORT_XGE1 || port == AAL_NI_PORT_XGE2) //poort 6 and port 7
    {
        aal_ni_mib_stats_t  stats ; 

        memset(&lanPortCntrs[port],0,sizeof(dal_port_cntr_t));

        memset(&stats,0x00,sizeof(stats));
        if((ret = aal_ni_xge_mac_mib_get(0,port-AAL_NI_PORT_XGE1,&stats)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    else if(port==CA_PORT_ID_CPU0)
    {
        aal_l2_te_pm_policer_t l2_pm;
        aal_l3_te_pm_policer_t l3_pm;

        memset(&lanPortCntrs[port],0,sizeof(dal_port_cntr_t));

        memset(&l2_pm,0,sizeof(aal_l2_te_pm_policer_t));
        if((ret = aal_l2_te_pm_policer_port_get(0,9,&l2_pm)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        memset(&l3_pm,0,sizeof(aal_l3_te_pm_policer_t));
        if((ret = aal_l3_te_pm_policer_port_get(0,8,&l3_pm)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    else if(port>=CA_PORT_ID_CPU1 && port<=CA_PORT_ID_CPU7)
    {
        return RT_ERR_OK;
    }
    else
    {
        read_keep = 1;

        memset(&data, 0, sizeof(ca_eth_port_stats_t));

        if((ret = ca_eth_port_stats_get(0,port_id,read_keep,&data)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_ca8279_stat_port_get
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
int32
dal_ca8279_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntrIdx, uint64 *pCntr)
{
    int32 ret;
    rtk_stat_port_cntr_t portCntrs;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK((MIB_PORT_CNTR_END <= cntrIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((16 < port), RT_ERR_NULL_POINTER);

    if ((ret = dal_ca8279_stat_port_getAll(port,&portCntrs)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    *pCntr = 0;

    switch(cntrIdx)
    {
        case DOT1D_TP_PORT_IN_DISCARDS_INDEX:
            *pCntr = lanPortCntrs[port].ifInDiscards;
            break;
        case IF_IN_OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].ifInOctets;
            break;
        case IF_IN_UCAST_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].ifInUcastPkts;
            break;
        case IF_IN_MULTICAST_PKTS_INDEX:
            *pCntr = portCntrs.ifInMulticastPkts;
            break;
        case IF_IN_BROADCAST_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].ifInBroadcastPkts;
            break;
        case IF_OUT_OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].ifOutOctets;
            break;
        case IF_OUT_UCAST_PKTS_CNT_INDEX:
            *pCntr = lanPortCntrs[port].ifOutUcastPkts;
            break;
        case IF_OUT_MULTICAST_PKTS_CNT_INDEX:
            *pCntr = lanPortCntrs[port].ifOutMulticastPkts;
            break;
        case IF_OUT_BROADCAST_PKTS_CNT_INDEX:
            *pCntr = lanPortCntrs[port].ifOutBrocastPkts;
            break;
        case IF_OUT_DISCARDS_INDEX:
            *pCntr = lanPortCntrs[port].ifOutDiscards;
            break;
        case DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX:
            *pCntr = lanPortCntrs[port].dot3StatsSingleCollisionFrames;
            break;
        case DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX:
            *pCntr = lanPortCntrs[port].dot3StatsMultipleCollisionFrames;
            break;
        case DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX:
            *pCntr = lanPortCntrs[port].dot3StatsDeferredTransmissions;
            break;
        case DOT3_STATS_LATE_COLLISIONS_INDEX:
            *pCntr = lanPortCntrs[port].dot3StatsLateCollisions;
            break;
        case DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX:
            *pCntr = lanPortCntrs[port].dot3StatsExcessiveCollisions;
            break;
        case DOT3_STATS_SYMBOL_ERRORS_INDEX:
            *pCntr = lanPortCntrs[port].dot3StatsSymbolErrors;
            break;
        case DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX:
            *pCntr = lanPortCntrs[port].dot3ControlInUnknownOpcodes;
            break;
        case DOT3_IN_PAUSE_FRAMES_INDEX:
            *pCntr = lanPortCntrs[port].dot3InPauseFrames;
            break;
        case DOT3_OUT_PAUSE_FRAMES_INDEX:
            *pCntr = lanPortCntrs[port].dot3OutPauseFrames;
            break;
        case ETHER_STATS_DROP_EVENTS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsDropEvents;
            break;
        case ETHER_STATS_TX_BROADCAST_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxBcastPkts;
            break;
        case ETHER_STATS_TX_MULTICAST_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxMcastPkts;
            break;
        case ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsCRCAlignErrors;
            break;
        case ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxUndersizePkts;
            break;
        case ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsRxUndersizePkts;
            break;
        case ETHER_STATS_TX_OVERSIZE_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxOversizePkts;
            break;
        case ETHER_STATS_RX_OVERSIZE_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsRxOversizePkts;
            break;
        case ETHER_STATS_FRAGMENTS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsFragments;
            break;
        case ETHER_STATS_JABBERS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsJabbers;
            break;
        case ETHER_STATS_COLLISIONS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsCollisions;
            break;
        case ETHER_STATS_TX_PKTS_64OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxPkts64Octets;
            break;
        case ETHER_STATS_RX_PKTS_64OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsRxPkts64Octets;
            break;
        case ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxPkts65to127Octets;
            break;
        case ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsRxPkts65to127Octets;
            break;
        case ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxPkts128to255Octets;
            break;
        case ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsRxPkts128to255Octets;
            break;
        case ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxPkts256to511Octets;
            break;
        case ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsRxPkts256to511Octets;
            break;
        case ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxPkts512to1023Octets;
            break;
        case ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsRxPkts512to1023Octets;
            break;
        case ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxPkts1024to1518Octets;
            break;
        case ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsRxPkts1024to1518Octets;
            break;
        case ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsTxPkts1519toMaxOctets;
            break;
        case ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX:
            *pCntr = lanPortCntrs[port].etherStatsRxPkts1519toMaxOctets;
            break;
        case IN_OAM_PDU_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].inOampduPkts;
            break;
        case OUT_OAM_PDU_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].outOampduPkts;
            break;
        default:
            *pCntr = 0;
            break;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_ca8279_stat_port_getAll
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
int32
dal_ca8279_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t read_keep;

    /* check Init status */
    RT_INIT_CHK(stat_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortCntrs), RT_ERR_NULL_POINTER);

    memset(pPortCntrs, 0, sizeof(rtk_stat_port_cntr_t));

    if(port == AAL_NI_PORT_XGE1 || port == AAL_NI_PORT_XGE2) //poort 6 and port 7
    {
        aal_ni_mib_stats_t  stats ; 

        memset(&stats,0x00,sizeof(stats));

        if((ret = aal_ni_xge_mac_mib_get(0,port-AAL_NI_PORT_XGE1,&stats)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        
        pPortCntrs->ifInOctets                           = lanPortCntrs[port].ifInOctets                           += stats.rx_byte_cnt_low;
        pPortCntrs->ifInUcastPkts                        = lanPortCntrs[port].ifInUcastPkts                        += stats.rx_uc_pkt_cnt;        /* received unicast frames                                                   */
        pPortCntrs->ifInMulticastPkts                    = lanPortCntrs[port].ifInMulticastPkts                    += stats.rx_mc_frame_cnt;        /* received multicast frames                                                 */
        pPortCntrs->ifInBroadcastPkts                    = lanPortCntrs[port].ifInBroadcastPkts                    += stats.rx_bc_frame_cnt;        /* received broadcast frames                                                 */
        pPortCntrs->ifInDiscards                         = lanPortCntrs[port].ifInDiscards                         += 0;
        pPortCntrs->ifOutOctets                          = lanPortCntrs[port].ifOutOctets                          += stats.tx_byte_cnt_low;
        pPortCntrs->ifOutDiscards                        = lanPortCntrs[port].ifOutDiscards                        += 0; 
        pPortCntrs->ifOutUcastPkts                       = lanPortCntrs[port].ifOutUcastPkts                       += stats.tx_uc_pkt_cnt;        /* transmitted unicast frames                                                */
        pPortCntrs->ifOutMulticastPkts                   = lanPortCntrs[port].ifOutMulticastPkts                   += stats.tx_mc_frame_cnt;        /* transmitted multicast frames                                              */
        pPortCntrs->ifOutBrocastPkts                     = lanPortCntrs[port].ifOutBrocastPkts                     += stats.tx_bc_frame_cnt;        /* transmitted broadcast frames                                              */
        pPortCntrs->dot1dBasePortDelayExceededDiscards   = lanPortCntrs[port].dot1dBasePortDelayExceededDiscards   += 0;
        pPortCntrs->dot1dTpPortInDiscards                = lanPortCntrs[port].dot1dTpPortInDiscards                += 0;
        pPortCntrs->dot1dTpHcPortInDiscards              = lanPortCntrs[port].dot1dTpHcPortInDiscards              += 0;
        pPortCntrs->dot3InPauseFrames                    = lanPortCntrs[port].dot3InPauseFrames                    += stats.rx_pause_frame_cnt;
        pPortCntrs->dot3OutPauseFrames                   = lanPortCntrs[port].dot3OutPauseFrames                   += stats.tx_pause_frame_cnt;
        pPortCntrs->dot3OutPauseOnFrames                 = lanPortCntrs[port].dot3OutPauseOnFrames                 += 0;
        pPortCntrs->dot3StatsAligmentErrors              = lanPortCntrs[port].dot3StatsAligmentErrors              += 0;
        pPortCntrs->dot3StatsFCSErrors                   = lanPortCntrs[port].dot3StatsFCSErrors                   += stats.rx_crc_error_frame_cnt;
        pPortCntrs->dot3StatsSingleCollisionFrames       = lanPortCntrs[port].dot3StatsSingleCollisionFrames       += 0;
        pPortCntrs->dot3StatsMultipleCollisionFrames     = lanPortCntrs[port].dot3StatsMultipleCollisionFrames     += 0;
        pPortCntrs->dot3StatsDeferredTransmissions       = lanPortCntrs[port].dot3StatsDeferredTransmissions       += 0;
        pPortCntrs->dot3StatsLateCollisions              = lanPortCntrs[port].dot3StatsLateCollisions              += 0;
        pPortCntrs->dot3StatsExcessiveCollisions         = lanPortCntrs[port].dot3StatsExcessiveCollisions         += 0;
        pPortCntrs->dot3StatsFrameTooLongs               = lanPortCntrs[port].dot3StatsFrameTooLongs               += 0;
        pPortCntrs->dot3StatsSymbolErrors                = lanPortCntrs[port].dot3StatsSymbolErrors                += 0;
        pPortCntrs->dot3ControlInUnknownOpcodes          = lanPortCntrs[port].dot3ControlInUnknownOpcodes          += 0;
        pPortCntrs->etherStatsDropEvents                 = lanPortCntrs[port].etherStatsDropEvents                 += 0;
        pPortCntrs->etherStatsOctets                     = lanPortCntrs[port].etherStatsOctets                     += 0;
        pPortCntrs->etherStatsBcastPkts                  = lanPortCntrs[port].etherStatsBcastPkts                  += 0;
        pPortCntrs->etherStatsMcastPkts                  = lanPortCntrs[port].etherStatsMcastPkts                  += 0;
        pPortCntrs->etherStatsUndersizePkts              = lanPortCntrs[port].etherStatsUndersizePkts              += 0;
        pPortCntrs->etherStatsOversizePkts               = lanPortCntrs[port].etherStatsOversizePkts               += 0;
        pPortCntrs->etherStatsFragments                  = lanPortCntrs[port].etherStatsFragments                  += 0;
        pPortCntrs->etherStatsJabbers                    = lanPortCntrs[port].etherStatsJabbers                    += 0;
        pPortCntrs->etherStatsCollisions                 = lanPortCntrs[port].etherStatsCollisions                 += 0;
        pPortCntrs->etherStatsCRCAlignErrors             = lanPortCntrs[port].etherStatsCRCAlignErrors             += 0;
        pPortCntrs->etherStatsPkts64Octets               = lanPortCntrs[port].etherStatsPkts64Octets               += 0;
        pPortCntrs->etherStatsPkts65to127Octets          = lanPortCntrs[port].etherStatsPkts65to127Octets          += 0;
        pPortCntrs->etherStatsPkts128to255Octets         = lanPortCntrs[port].etherStatsPkts128to255Octets         += 0;
        pPortCntrs->etherStatsPkts256to511Octets         = lanPortCntrs[port].etherStatsPkts256to511Octets         += 0;
        pPortCntrs->etherStatsPkts512to1023Octets        = lanPortCntrs[port].etherStatsPkts512to1023Octets        += 0;
        pPortCntrs->etherStatsPkts1024to1518Octets       = lanPortCntrs[port].etherStatsPkts1024to1518Octets       += 0;
        pPortCntrs->etherStatsTxOctets                   = lanPortCntrs[port].etherStatsTxOctets                   += 0;
        pPortCntrs->etherStatsTxUndersizePkts            = lanPortCntrs[port].etherStatsTxUndersizePkts            += 0;
        pPortCntrs->etherStatsTxOversizePkts             = lanPortCntrs[port].etherStatsTxOversizePkts             += 0;
        pPortCntrs->etherStatsTxPkts64Octets             = lanPortCntrs[port].etherStatsTxPkts64Octets             += stats.tx_frame_64_cnt;        /* transmitted frames with 64 bytes                                          */
        pPortCntrs->etherStatsTxPkts65to127Octets        = lanPortCntrs[port].etherStatsTxPkts65to127Octets        += stats.tx_frame_65to127_cnt;    /* transmitted frames with 65 ~ 127 bytes                                    */
        pPortCntrs->etherStatsTxPkts128to255Octets       = lanPortCntrs[port].etherStatsTxPkts128to255Octets       += stats.tx_frame_128to255_cnt;   /* transmitted frames with 128 ~ 255 bytes                                   */
        pPortCntrs->etherStatsTxPkts256to511Octets       = lanPortCntrs[port].etherStatsTxPkts256to511Octets       += stats.tx_frame_256to511_cnt;   /* transmitted frames with 256 ~ 511 bytes                                   */
        pPortCntrs->etherStatsTxPkts512to1023Octets      = lanPortCntrs[port].etherStatsTxPkts512to1023Octets      += stats.tx_frame_512to1023_cnt;  /* transmitted frames with 512 ~ 1023 bytes                                  */
        pPortCntrs->etherStatsTxPkts1024to1518Octets     = lanPortCntrs[port].etherStatsTxPkts1024to1518Octets     += stats.tx_frame_1024to1518_cnt; /* transmitted frames with 1024 ~ 1518 bytes                                 */
        pPortCntrs->etherStatsTxPkts1519toMaxOctets      = lanPortCntrs[port].etherStatsTxPkts1519toMaxOctets      += stats.tx_frame_1519to2100_cnt + stats.tx_frame_2101to9200_cnt + stats.tx_frame_9201tomax_cnt;  /* transmitted frames of which length is equal to or greater than 1519 bytes */
        pPortCntrs->etherStatsTxBcastPkts                = lanPortCntrs[port].etherStatsTxBcastPkts                += stats.tx_bc_frame_cnt;        /* transmitted broadcast frames                                              */
        pPortCntrs->etherStatsTxMcastPkts                = lanPortCntrs[port].etherStatsTxMcastPkts                += stats.tx_mc_frame_cnt;        /* transmitted multicast frames                                              */
        pPortCntrs->etherStatsTxFragments                = lanPortCntrs[port].etherStatsTxFragments                += 0;
        pPortCntrs->etherStatsTxJabbers                  = lanPortCntrs[port].etherStatsTxJabbers                  += 0;
        pPortCntrs->etherStatsTxCRCAlignErrors           = lanPortCntrs[port].etherStatsTxCRCAlignErrors           += 0;
        pPortCntrs->etherStatsRxUndersizePkts            = lanPortCntrs[port].etherStatsRxUndersizePkts            += stats.rx_undersize_frame_cnt; /* received frames undersize frame without FCS err               */
        pPortCntrs->etherStatsRxUndersizeDropPkts        = lanPortCntrs[port].etherStatsRxUndersizeDropPkts        += stats.rx_runt_frame_cnt - stats.rx_undersize_frame_cnt;
        pPortCntrs->etherStatsRxOversizePkts             = lanPortCntrs[port].etherStatsRxOversizePkts             += 0;
        pPortCntrs->etherStatsRxPkts64Octets             = lanPortCntrs[port].etherStatsRxPkts64Octets             += stats.rx_frame_64_cnt;        /* received frames with 64 bytes                                             */
        pPortCntrs->etherStatsRxPkts65to127Octets        = lanPortCntrs[port].etherStatsRxPkts65to127Octets        += stats.rx_frame_65to127_cnt;    /* received frames with 65 ~ 127 bytes                                       */
        pPortCntrs->etherStatsRxPkts128to255Octets       = lanPortCntrs[port].etherStatsRxPkts128to255Octets       += stats.rx_frame_128to255_cnt;   /* received frames with 128 ~ 255 bytes                                      */
        pPortCntrs->etherStatsRxPkts256to511Octets       = lanPortCntrs[port].etherStatsRxPkts256to511Octets       += stats.rx_frame_256to511_cnt;   /* received frames with 256 ~ 511 bytes                                      */
        pPortCntrs->etherStatsRxPkts512to1023Octets      = lanPortCntrs[port].etherStatsRxPkts512to1023Octets      += stats.rx_frame_512to1023_cnt;  /* received frames with 512 ~ 1023 bytes                                     */
        pPortCntrs->etherStatsRxPkts1024to1518Octets     = lanPortCntrs[port].etherStatsRxPkts1024to1518Octets     += stats.rx_frame_1024to1518_cnt; /* received frames with 1024 ~ 1518 bytes                                    */
        pPortCntrs->etherStatsRxPkts1519toMaxOctets      = lanPortCntrs[port].etherStatsRxPkts1519toMaxOctets      += stats.rx_frame_1519to2100_cnt + stats.rx_frame_2101to9200_cnt + stats.rx_frame_9201tomax_cnt;  /* received frames of which length is equal to or greater than 1519 bytes    */
        pPortCntrs->inOampduPkts                         = lanPortCntrs[port].inOampduPkts                         += 0; //TBD waiting for Saturn Chip
        pPortCntrs->outOampduPkts                        = lanPortCntrs[port].outOampduPkts                        += 0; //TBD waiting for Saturn Chip


    }
    else if(port==CA_PORT_ID_CPU0)
    {
        aal_l2_te_pm_policer_t l2_pm;
        aal_l3_te_pm_policer_t l3_pm;

        memset(&l2_pm,0,sizeof(aal_l2_te_pm_policer_t));
        if((ret = aal_l2_te_pm_policer_port_get(0,9,&l2_pm)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        memset(&l3_pm,0,sizeof(aal_l3_te_pm_policer_t));
        if((ret = aal_l3_te_pm_policer_port_get(0,8,&l3_pm)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        pPortCntrs->ifInOctets                           = lanPortCntrs[port].ifInOctets                           += l2_pm.total_bytes;
        pPortCntrs->ifInUcastPkts                        = lanPortCntrs[port].ifInUcastPkts                        += 0;
        pPortCntrs->ifInMulticastPkts                    = lanPortCntrs[port].ifInMulticastPkts                    += 0;
        pPortCntrs->ifInBroadcastPkts                    = lanPortCntrs[port].ifInBroadcastPkts                    += 0;
        pPortCntrs->ifInDiscards                         = lanPortCntrs[port].ifInDiscards                         += l2_pm.red_pkt;
        pPortCntrs->ifOutOctets                          = lanPortCntrs[port].ifOutOctets                          += l3_pm.total_bytes;
        pPortCntrs->ifOutDiscards                        = lanPortCntrs[port].ifOutDiscards                        += l3_pm.red_pkt;
        pPortCntrs->ifOutUcastPkts                       = lanPortCntrs[port].ifOutUcastPkts                       += 0;
        pPortCntrs->ifOutMulticastPkts                   = lanPortCntrs[port].ifOutMulticastPkts                   += 0;
        pPortCntrs->ifOutBrocastPkts                     = lanPortCntrs[port].ifOutBrocastPkts                     += 0;
        pPortCntrs->dot1dBasePortDelayExceededDiscards   = lanPortCntrs[port].dot1dBasePortDelayExceededDiscards   += 0;
        pPortCntrs->dot1dTpPortInDiscards                = lanPortCntrs[port].dot1dTpPortInDiscards                += 0;
        pPortCntrs->dot1dTpHcPortInDiscards              = lanPortCntrs[port].dot1dTpHcPortInDiscards              += 0;
        pPortCntrs->dot3InPauseFrames                    = lanPortCntrs[port].dot3InPauseFrames                    += 0;
        pPortCntrs->dot3OutPauseFrames                   = lanPortCntrs[port].dot3OutPauseFrames                   += 0;
        pPortCntrs->dot3OutPauseOnFrames                 = lanPortCntrs[port].dot3OutPauseOnFrames                 += 0;
        pPortCntrs->dot3StatsAligmentErrors              = lanPortCntrs[port].dot3StatsAligmentErrors              += 0;
        pPortCntrs->dot3StatsFCSErrors                   = lanPortCntrs[port].dot3StatsFCSErrors                   += 0;
        pPortCntrs->dot3StatsSingleCollisionFrames       = lanPortCntrs[port].dot3StatsSingleCollisionFrames       += 0;
        pPortCntrs->dot3StatsMultipleCollisionFrames     = lanPortCntrs[port].dot3StatsMultipleCollisionFrames     += 0;
        pPortCntrs->dot3StatsDeferredTransmissions       = lanPortCntrs[port].dot3StatsDeferredTransmissions       += 0;
        pPortCntrs->dot3StatsLateCollisions              = lanPortCntrs[port].dot3StatsLateCollisions              += 0;
        pPortCntrs->dot3StatsExcessiveCollisions         = lanPortCntrs[port].dot3StatsExcessiveCollisions         += 0;
        pPortCntrs->dot3StatsFrameTooLongs               = lanPortCntrs[port].dot3StatsFrameTooLongs               += 0;
        pPortCntrs->dot3StatsSymbolErrors                = lanPortCntrs[port].dot3StatsSymbolErrors                += 0;
        pPortCntrs->dot3ControlInUnknownOpcodes          = lanPortCntrs[port].dot3ControlInUnknownOpcodes          += 0;
        pPortCntrs->etherStatsDropEvents                 = lanPortCntrs[port].etherStatsDropEvents                 += 0;
        pPortCntrs->etherStatsOctets                     = lanPortCntrs[port].etherStatsOctets                     += l2_pm.total_bytes + l3_pm.total_bytes;
        pPortCntrs->etherStatsBcastPkts                  = lanPortCntrs[port].etherStatsBcastPkts                  += 0;
        pPortCntrs->etherStatsMcastPkts                  = lanPortCntrs[port].etherStatsMcastPkts                  += 0;
        pPortCntrs->etherStatsUndersizePkts              = lanPortCntrs[port].etherStatsUndersizePkts              += 0;
        pPortCntrs->etherStatsOversizePkts               = lanPortCntrs[port].etherStatsOversizePkts               += 0;
        pPortCntrs->etherStatsFragments                  = lanPortCntrs[port].etherStatsFragments                  += 0;
        pPortCntrs->etherStatsJabbers                    = lanPortCntrs[port].etherStatsJabbers                    += 0;
        pPortCntrs->etherStatsCollisions                 = lanPortCntrs[port].etherStatsCollisions                 += 0;
        pPortCntrs->etherStatsCRCAlignErrors             = lanPortCntrs[port].etherStatsCRCAlignErrors             += 0;
        pPortCntrs->etherStatsPkts64Octets               = lanPortCntrs[port].etherStatsPkts64Octets               += 0;
        pPortCntrs->etherStatsPkts65to127Octets          = lanPortCntrs[port].etherStatsPkts65to127Octets          += 0;
        pPortCntrs->etherStatsPkts128to255Octets         = lanPortCntrs[port].etherStatsPkts128to255Octets         += 0;
        pPortCntrs->etherStatsPkts256to511Octets         = lanPortCntrs[port].etherStatsPkts256to511Octets         += 0;
        pPortCntrs->etherStatsPkts512to1023Octets        = lanPortCntrs[port].etherStatsPkts512to1023Octets        += 0;
        pPortCntrs->etherStatsPkts1024to1518Octets       = lanPortCntrs[port].etherStatsPkts1024to1518Octets       += 0;
        pPortCntrs->etherStatsTxOctets                   = lanPortCntrs[port].etherStatsTxOctets                   += l3_pm.total_bytes;
        pPortCntrs->etherStatsTxUndersizePkts            = lanPortCntrs[port].etherStatsTxUndersizePkts            += l3_pm.total_pkt;
        pPortCntrs->etherStatsTxOversizePkts             = lanPortCntrs[port].etherStatsTxOversizePkts             += 0;
        pPortCntrs->etherStatsTxPkts64Octets             = lanPortCntrs[port].etherStatsTxPkts64Octets             += 0;
        pPortCntrs->etherStatsTxPkts65to127Octets        = lanPortCntrs[port].etherStatsTxPkts65to127Octets        += 0;
        pPortCntrs->etherStatsTxPkts128to255Octets       = lanPortCntrs[port].etherStatsTxPkts128to255Octets       += 0;
        pPortCntrs->etherStatsTxPkts256to511Octets       = lanPortCntrs[port].etherStatsTxPkts256to511Octets       += 0;
        pPortCntrs->etherStatsTxPkts512to1023Octets      = lanPortCntrs[port].etherStatsTxPkts512to1023Octets      += 0;
        pPortCntrs->etherStatsTxPkts1024to1518Octets     = lanPortCntrs[port].etherStatsTxPkts1024to1518Octets     += 0;
        pPortCntrs->etherStatsTxPkts1519toMaxOctets      = lanPortCntrs[port].etherStatsTxPkts1519toMaxOctets      += 0;
        pPortCntrs->etherStatsTxBcastPkts                = lanPortCntrs[port].etherStatsTxBcastPkts                += 0;
        pPortCntrs->etherStatsTxMcastPkts                = lanPortCntrs[port].etherStatsTxMcastPkts                += 0;
        pPortCntrs->etherStatsTxFragments                = lanPortCntrs[port].etherStatsTxFragments                += 0;
        pPortCntrs->etherStatsTxJabbers                  = lanPortCntrs[port].etherStatsTxJabbers                  += 0;
        pPortCntrs->etherStatsTxCRCAlignErrors           = lanPortCntrs[port].etherStatsTxCRCAlignErrors           += 0;
        pPortCntrs->etherStatsRxUndersizePkts            = lanPortCntrs[port].etherStatsRxUndersizePkts            += l2_pm.total_pkt;
        pPortCntrs->etherStatsRxUndersizeDropPkts        = lanPortCntrs[port].etherStatsRxUndersizeDropPkts        += 0;
        pPortCntrs->etherStatsRxOversizePkts             = lanPortCntrs[port].etherStatsRxOversizePkts             += 0;
        pPortCntrs->etherStatsRxPkts64Octets             = lanPortCntrs[port].etherStatsRxPkts64Octets             += 0;
        pPortCntrs->etherStatsRxPkts65to127Octets        = lanPortCntrs[port].etherStatsRxPkts65to127Octets        += 0;
        pPortCntrs->etherStatsRxPkts128to255Octets       = lanPortCntrs[port].etherStatsRxPkts128to255Octets       += 0;
        pPortCntrs->etherStatsRxPkts256to511Octets       = lanPortCntrs[port].etherStatsRxPkts256to511Octets       += 0;
        pPortCntrs->etherStatsRxPkts512to1023Octets      = lanPortCntrs[port].etherStatsRxPkts512to1023Octets      += 0;
        pPortCntrs->etherStatsRxPkts1024to1518Octets     = lanPortCntrs[port].etherStatsRxPkts1024to1518Octets     += 0;
        pPortCntrs->etherStatsRxPkts1519toMaxOctets      = lanPortCntrs[port].etherStatsRxPkts1519toMaxOctets      += 0;
        pPortCntrs->inOampduPkts                         = lanPortCntrs[port].inOampduPkts                         += 0;
        pPortCntrs->outOampduPkts                        = lanPortCntrs[port].outOampduPkts                        += 0;
    

    }
    else if(port>=CA_PORT_ID_CPU1 && port<=CA_PORT_ID_CPU7)
    {
        memset(pPortCntrs, 0, sizeof(rtk_stat_port_cntr_t));
    }
    else
    {
        ca_eth_port_stats_t data;

        read_keep = 0;

        memset(&data, 0, sizeof(ca_eth_port_stats_t));

        if((ret = ca_eth_port_stats_get(0,port_id,read_keep,&data)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        pPortCntrs->ifInOctets                           = lanPortCntrs[port].ifInOctets                           = data.rx_good_octets;
        pPortCntrs->ifInUcastPkts                        = lanPortCntrs[port].ifInUcastPkts                        = data.rx_uc_frames;        /* received unicast frames                                                   */
        pPortCntrs->ifInMulticastPkts                    = lanPortCntrs[port].ifInMulticastPkts                    = data.rx_mc_frames;        /* received multicast frames                                                 */
        pPortCntrs->ifInBroadcastPkts                    = lanPortCntrs[port].ifInBroadcastPkts                    = data.rx_bc_frames;        /* received broadcast frames                                                 */
        pPortCntrs->ifInDiscards                         = lanPortCntrs[port].ifInDiscards                         = 0;
        pPortCntrs->ifOutOctets                          = lanPortCntrs[port].ifOutOctets                          = data.tx_good_octets;
        pPortCntrs->ifOutDiscards                        = lanPortCntrs[port].ifOutDiscards                        = 0; 
        pPortCntrs->ifOutUcastPkts                       = lanPortCntrs[port].ifOutUcastPkts                       = data.tx_uc_frames;        /* transmitted unicast frames                                                */
        pPortCntrs->ifOutMulticastPkts                   = lanPortCntrs[port].ifOutMulticastPkts                   = data.tx_mc_frames;        /* transmitted multicast frames                                              */
        pPortCntrs->ifOutBrocastPkts                     = lanPortCntrs[port].ifOutBrocastPkts                     = data.tx_bc_frames;        /* transmitted broadcast frames                                              */
        pPortCntrs->dot1dBasePortDelayExceededDiscards   = lanPortCntrs[port].dot1dBasePortDelayExceededDiscards   = 0;
        pPortCntrs->dot1dTpPortInDiscards                = lanPortCntrs[port].dot1dTpPortInDiscards                = 0;
        pPortCntrs->dot1dTpHcPortInDiscards              = lanPortCntrs[port].dot1dTpHcPortInDiscards              = 0;
        pPortCntrs->dot3InPauseFrames                    = lanPortCntrs[port].dot3InPauseFrames                    = data.rx_pause_frames;
        pPortCntrs->dot3OutPauseFrames                   = lanPortCntrs[port].dot3OutPauseFrames                   = data.tx_pause_frames;
        pPortCntrs->dot3OutPauseOnFrames                 = lanPortCntrs[port].dot3OutPauseOnFrames                 = 0;
        pPortCntrs->dot3StatsAligmentErrors              = lanPortCntrs[port].dot3StatsAligmentErrors              = data.phy_stats.alignmentErrors;
        pPortCntrs->dot3StatsFCSErrors                   = lanPortCntrs[port].dot3StatsFCSErrors                   = data.rx_fcs_error_frames;
        pPortCntrs->dot3StatsSingleCollisionFrames       = lanPortCntrs[port].dot3StatsSingleCollisionFrames       = data.phy_stats.single_collision_frames;
        pPortCntrs->dot3StatsMultipleCollisionFrames     = lanPortCntrs[port].dot3StatsMultipleCollisionFrames     = data.phy_stats.multiple_collision_frames;
        pPortCntrs->dot3StatsDeferredTransmissions       = lanPortCntrs[port].dot3StatsDeferredTransmissions       = data.phy_stats.frames_with_deferredXmissions;
        pPortCntrs->dot3StatsLateCollisions              = lanPortCntrs[port].dot3StatsLateCollisions              = data.phy_stats.late_collisions;
        pPortCntrs->dot3StatsExcessiveCollisions         = lanPortCntrs[port].dot3StatsExcessiveCollisions         = 0;
        pPortCntrs->dot3StatsFrameTooLongs               = lanPortCntrs[port].dot3StatsFrameTooLongs               = 0;
        pPortCntrs->dot3StatsSymbolErrors                = lanPortCntrs[port].dot3StatsSymbolErrors                = data.phy_stats.symbol_error_during_carrier;
        pPortCntrs->dot3ControlInUnknownOpcodes          = lanPortCntrs[port].dot3ControlInUnknownOpcodes          = data.phy_stats.unsupported_opcodes_received;
        pPortCntrs->etherStatsDropEvents                 = lanPortCntrs[port].etherStatsDropEvents                 = 0;
        pPortCntrs->etherStatsOctets                     = lanPortCntrs[port].etherStatsOctets                     = 0;
        pPortCntrs->etherStatsBcastPkts                  = lanPortCntrs[port].etherStatsBcastPkts                  = 0;
        pPortCntrs->etherStatsMcastPkts                  = lanPortCntrs[port].etherStatsMcastPkts                  = 0;
        pPortCntrs->etherStatsUndersizePkts              = lanPortCntrs[port].etherStatsUndersizePkts              = 0;
        pPortCntrs->etherStatsOversizePkts               = lanPortCntrs[port].etherStatsOversizePkts               = 0;
        pPortCntrs->etherStatsFragments                  = lanPortCntrs[port].etherStatsFragments                  = 0;
        pPortCntrs->etherStatsJabbers                    = lanPortCntrs[port].etherStatsJabbers                    = 0;
        pPortCntrs->etherStatsCollisions                 = lanPortCntrs[port].etherStatsCollisions                 = 0;
        pPortCntrs->etherStatsCRCAlignErrors             = lanPortCntrs[port].etherStatsCRCAlignErrors             = 0;
        pPortCntrs->etherStatsPkts64Octets               = lanPortCntrs[port].etherStatsPkts64Octets               = 0;
        pPortCntrs->etherStatsPkts65to127Octets          = lanPortCntrs[port].etherStatsPkts65to127Octets          = 0;
        pPortCntrs->etherStatsPkts128to255Octets         = lanPortCntrs[port].etherStatsPkts128to255Octets         = 0;
        pPortCntrs->etherStatsPkts256to511Octets         = lanPortCntrs[port].etherStatsPkts256to511Octets         = 0;
        pPortCntrs->etherStatsPkts512to1023Octets        = lanPortCntrs[port].etherStatsPkts512to1023Octets        = 0;
        pPortCntrs->etherStatsPkts1024to1518Octets       = lanPortCntrs[port].etherStatsPkts1024to1518Octets       = 0;
        pPortCntrs->etherStatsTxOctets                   = lanPortCntrs[port].etherStatsTxOctets                   = 0;
        pPortCntrs->etherStatsTxUndersizePkts            = lanPortCntrs[port].etherStatsTxUndersizePkts            = 0;
        pPortCntrs->etherStatsTxOversizePkts             = lanPortCntrs[port].etherStatsTxOversizePkts             = 0;
        pPortCntrs->etherStatsTxPkts64Octets             = lanPortCntrs[port].etherStatsTxPkts64Octets             = data.tx_64_frames;        /* transmitted frames with 64 bytes                                          */
        pPortCntrs->etherStatsTxPkts65to127Octets        = lanPortCntrs[port].etherStatsTxPkts65to127Octets        = data.tx_65_127_frames;    /* transmitted frames with 65 ~ 127 bytes                                    */
        pPortCntrs->etherStatsTxPkts128to255Octets       = lanPortCntrs[port].etherStatsTxPkts128to255Octets       = data.tx_128_255_frames;   /* transmitted frames with 128 ~ 255 bytes                                   */
        pPortCntrs->etherStatsTxPkts256to511Octets       = lanPortCntrs[port].etherStatsTxPkts256to511Octets       = data.tx_256_511_frames;   /* transmitted frames with 256 ~ 511 bytes                                   */
        pPortCntrs->etherStatsTxPkts512to1023Octets      = lanPortCntrs[port].etherStatsTxPkts512to1023Octets      = data.tx_512_1023_frames;  /* transmitted frames with 512 ~ 1023 bytes                                  */
        pPortCntrs->etherStatsTxPkts1024to1518Octets     = lanPortCntrs[port].etherStatsTxPkts1024to1518Octets     = data.tx_1024_1518_frames; /* transmitted frames with 1024 ~ 1518 bytes                                 */
        pPortCntrs->etherStatsTxPkts1519toMaxOctets      = lanPortCntrs[port].etherStatsTxPkts1519toMaxOctets      = data.tx_1519_max_frames;  /* transmitted frames of which length is equal to or greater than 1519 bytes */
        pPortCntrs->etherStatsTxBcastPkts                = lanPortCntrs[port].etherStatsTxBcastPkts                = data.tx_bc_frames;        /* transmitted broadcast frames                                              */
        pPortCntrs->etherStatsTxMcastPkts                = lanPortCntrs[port].etherStatsTxMcastPkts                = data.tx_mc_frames;        /* transmitted multicast frames                                              */
        pPortCntrs->etherStatsTxFragments                = lanPortCntrs[port].etherStatsTxFragments                = 0;
        pPortCntrs->etherStatsTxJabbers                  = lanPortCntrs[port].etherStatsTxJabbers                  = 0;
        pPortCntrs->etherStatsTxCRCAlignErrors           = lanPortCntrs[port].etherStatsTxCRCAlignErrors           = 0;
        pPortCntrs->etherStatsRxUndersizePkts            = lanPortCntrs[port].etherStatsRxUndersizePkts            = data.rx_undersize_frames; /* received frames undersize frame without FCS err               */
        pPortCntrs->etherStatsRxUndersizeDropPkts        = lanPortCntrs[port].etherStatsRxUndersizeDropPkts        = data.rx_runt_frames - data.rx_undersize_frames;
        pPortCntrs->etherStatsRxOversizePkts             = lanPortCntrs[port].etherStatsRxOversizePkts             = 0;
        pPortCntrs->etherStatsRxPkts64Octets             = lanPortCntrs[port].etherStatsRxPkts64Octets             = data.rx_64_frames;        /* received frames with 64 bytes                                             */
        pPortCntrs->etherStatsRxPkts65to127Octets        = lanPortCntrs[port].etherStatsRxPkts65to127Octets        = data.rx_65_127_frames;    /* received frames with 65 ~ 127 bytes                                       */
        pPortCntrs->etherStatsRxPkts128to255Octets       = lanPortCntrs[port].etherStatsRxPkts128to255Octets       = data.rx_128_255_frames;   /* received frames with 128 ~ 255 bytes                                      */
        pPortCntrs->etherStatsRxPkts256to511Octets       = lanPortCntrs[port].etherStatsRxPkts256to511Octets       = data.rx_256_511_frames;   /* received frames with 256 ~ 511 bytes                                      */
        pPortCntrs->etherStatsRxPkts512to1023Octets      = lanPortCntrs[port].etherStatsRxPkts512to1023Octets      = data.rx_512_1023_frames;  /* received frames with 512 ~ 1023 bytes                                     */
        pPortCntrs->etherStatsRxPkts1024to1518Octets     = lanPortCntrs[port].etherStatsRxPkts1024to1518Octets     = data.rx_1024_1518_frames; /* received frames with 1024 ~ 1518 bytes                                    */
        pPortCntrs->etherStatsRxPkts1519toMaxOctets      = lanPortCntrs[port].etherStatsRxPkts1519toMaxOctets      = data.rx_1519_max_frames;  /* received frames of which length is equal to or greater than 1519 bytes    */
        pPortCntrs->inOampduPkts                         = lanPortCntrs[port].inOampduPkts                         = 0; //TBD waiting for Saturn Chip
        pPortCntrs->outOampduPkts                        = lanPortCntrs[port].outOampduPkts                        = 0; //TBD waiting for Saturn Chip
    }
    return RT_ERR_OK;
}
