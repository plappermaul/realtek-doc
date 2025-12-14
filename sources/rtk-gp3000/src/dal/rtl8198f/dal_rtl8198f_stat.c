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
#include <dal/rtl8198f/dal_rtl8198f.h>
#include <dal/rtl8198f/dal_rtl8198f_stat.h>
#include <osal/time.h>

#include <cortina-api/include/port.h>
#include <cortina-api/include/eth_port.h>
#include <aal/include/aal_ni_l2.h>


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
#ifdef CONFIG_RTL_83XX_SUPPORT
uint32    stat_83xx_init = INIT_NOT_COMPLETED;
#endif

/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      dal_rtl8198f_stat_init
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
dal_rtl8198f_stat_init(void)
{
    int32 ret;
    rtk_port_t port;

    stat_init = INIT_COMPLETED;
#ifdef CONFIG_RTL_83XX_SUPPORT
    stat_83xx_init = INIT_COMPLETED;
#endif

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_rtl8198f_stat_port_reset(port)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8198f_stat_port_reset
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
dal_rtl8198f_stat_port_reset(rtk_port_t port)
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
    
    read_keep = 0;

    memset(&data, 0, sizeof(ca_eth_port_stats_t));

    if((ret = ca_eth_port_stats_get(0,port_id,read_keep,&data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8198f_stat_port_get
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
dal_rtl8198f_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntrIdx, uint64 *pCntr)
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

    if ((ret = dal_rtl8198f_stat_port_getAll(port,&portCntrs)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    *pCntr = 0;

    switch(cntrIdx)
    {
        case DOT1D_TP_PORT_IN_DISCARDS_INDEX:
            *pCntr = portCntrs.ifInDiscards;
            break;
        case IF_IN_OCTETS_INDEX:
            *pCntr = portCntrs.ifInOctets;
            break;
        case IF_IN_UCAST_PKTS_INDEX:
            *pCntr = portCntrs.ifInUcastPkts;
            break;
        case IF_IN_MULTICAST_PKTS_INDEX:
            *pCntr = portCntrs.ifInMulticastPkts;
            break;
        case IF_IN_BROADCAST_PKTS_INDEX:
            *pCntr = portCntrs.ifInBroadcastPkts;
            break;
        case IF_OUT_OCTETS_INDEX:
            *pCntr = portCntrs.ifOutOctets;
            break;
        case IF_OUT_UCAST_PKTS_CNT_INDEX:
            *pCntr = portCntrs.ifOutUcastPkts;
            break;
        case IF_OUT_MULTICAST_PKTS_CNT_INDEX:
            *pCntr = portCntrs.ifOutMulticastPkts;
            break;
        case IF_OUT_BROADCAST_PKTS_CNT_INDEX:
            *pCntr = portCntrs.ifOutBrocastPkts;
            break;
        case IF_OUT_DISCARDS_INDEX:
            *pCntr = portCntrs.ifOutDiscards;
            break;
        case DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX:
            *pCntr = portCntrs.dot3StatsSingleCollisionFrames;
            break;
        case DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX:
            *pCntr = portCntrs.dot3StatsMultipleCollisionFrames;
            break;
        case DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX:
            *pCntr = portCntrs.dot3StatsDeferredTransmissions;
            break;
        case DOT3_STATS_LATE_COLLISIONS_INDEX:
            *pCntr = portCntrs.dot3StatsLateCollisions;
            break;
        case DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX:
            *pCntr = portCntrs.dot3StatsExcessiveCollisions;
            break;
        case DOT3_STATS_SYMBOL_ERRORS_INDEX:
            *pCntr = portCntrs.dot3StatsSymbolErrors;
            break;
        case DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX:
            *pCntr = portCntrs.dot3ControlInUnknownOpcodes;
            break;
        case DOT3_IN_PAUSE_FRAMES_INDEX:
            *pCntr = portCntrs.dot3InPauseFrames;
            break;
        case DOT3_OUT_PAUSE_FRAMES_INDEX:
            *pCntr = portCntrs.dot3OutPauseFrames;
            break;
        case ETHER_STATS_DROP_EVENTS_INDEX:
            *pCntr = portCntrs.etherStatsDropEvents;
            break;
        case ETHER_STATS_TX_BROADCAST_PKTS_INDEX:
            *pCntr = portCntrs.etherStatsTxBcastPkts;
            break;
        case ETHER_STATS_TX_MULTICAST_PKTS_INDEX:
            *pCntr = portCntrs.etherStatsTxMcastPkts;
            break;
        case ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
            *pCntr = portCntrs.etherStatsCRCAlignErrors;
            break;
        case ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX:
            *pCntr = portCntrs.etherStatsTxUndersizePkts;
            break;
        case ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX:
            *pCntr = portCntrs.etherStatsRxUndersizePkts;
            break;
        case ETHER_STATS_TX_OVERSIZE_PKTS_INDEX:
            *pCntr = portCntrs.etherStatsTxOversizePkts;
            break;
        case ETHER_STATS_RX_OVERSIZE_PKTS_INDEX:
            *pCntr = portCntrs.etherStatsRxOversizePkts;
            break;
        case ETHER_STATS_FRAGMENTS_INDEX:
            *pCntr = portCntrs.etherStatsFragments;
            break;
        case ETHER_STATS_JABBERS_INDEX:
            *pCntr = portCntrs.etherStatsJabbers;
            break;
        case ETHER_STATS_COLLISIONS_INDEX:
            *pCntr = portCntrs.etherStatsCollisions;
            break;
        case ETHER_STATS_TX_PKTS_64OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsTxPkts64Octets;
            break;
        case ETHER_STATS_RX_PKTS_64OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsRxPkts64Octets;
            break;
        case ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsTxPkts65to127Octets;
            break;
        case ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsRxPkts65to127Octets;
            break;
        case ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsTxPkts128to255Octets;
            break;
        case ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsRxPkts128to255Octets;
            break;
        case ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsTxPkts256to511Octets;
            break;
        case ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsRxPkts256to511Octets;
            break;
        case ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsTxPkts512to1023Octets;
            break;
        case ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsRxPkts512to1023Octets;
            break;
        case ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsTxPkts1024to1518Octets;
            break;
        case ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX:
            *pCntr = portCntrs.etherStatsRxPkts1024to1518Octets;
            break;
        case ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX:
            *pCntr = portCntrs.etherStatsTxPkts1519toMaxOctets;
            break;
        case ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX:
            *pCntr = portCntrs.etherStatsRxPkts1519toMaxOctets;
            break;
        case IN_OAM_PDU_PKTS_INDEX:
            *pCntr = portCntrs.inOampduPkts;
            break;
        case OUT_OAM_PDU_PKTS_INDEX:
            *pCntr = portCntrs.outOampduPkts;
            break;
        default:
            *pCntr = 0;
            break;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8198f_stat_embedded_port_get
 * Description:
 *      Get all counters of one specified port in 8198F embedded port.
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
dal_rtl8198f_stat_embedded_port_get(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t read_keep;
    //ca_eth_port_stats_t data;
    aal_ni_mib_stats_t data;

    /* check Init status */
    RT_INIT_CHK(stat_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortCntrs), RT_ERR_NULL_POINTER);

    read_keep = 0;

    //memset(&data, 0, sizeof(ca_eth_port_stats_t));
    memset(&data, 0, sizeof(aal_ni_mib_stats_t));
    memset(pPortCntrs, 0, sizeof(rtk_stat_port_cntr_t));

    /*
     * Originally we use ca_* series API to get the value of HW counter.
     * But ca_eth_port_stats_t only contains partial HW counter, instead
     * we use aal_* series API to get full content of MIB.
     * NOTE : There is no need to do RTK2CA port mapping for aal_* API
     */
    //if((ret = ca_eth_port_stats_get(0,port_id,read_keep,&data)) != CA_E_OK)
    if ((ret = aal_ni_eth_port_mib_get(0, port, read_keep, &data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    pPortCntrs->ifInOctets                           = data.rx_byte_cnt_low + data.rx_byte_cnt_high;
    pPortCntrs->ifInUcastPkts                        = data.rx_uc_pkt_cnt;          /* received unicast frames */
    pPortCntrs->ifInMulticastPkts                    = data.rx_mc_frame_cnt;        /* received multicast frames */
    pPortCntrs->ifInBroadcastPkts                    = data.rx_bc_frame_cnt;        /* received broadcast frames */
    pPortCntrs->ifInDiscards                         = 0;
    pPortCntrs->ifOutOctets                          = data.tx_byte_cnt_low + data.tx_byte_cnt_high;
    pPortCntrs->ifOutDiscards                        = data.tx_crc_error_frame_cnt + data.tx_late_col_frame_cnt +
							data.tx_exess_col_frame_cnt; 
    pPortCntrs->ifOutUcastPkts                       = data.tx_uc_pkt_cnt;          /* transmitted unicast frames  */
    pPortCntrs->ifOutMulticastPkts                   = data.tx_mc_frame_cnt;        /* transmitted multicast frames */
    pPortCntrs->ifOutBrocastPkts                     = data.tx_bc_frame_cnt;        /* transmitted broadcast frames */
    pPortCntrs->dot1dBasePortDelayExceededDiscards   = 0;
    pPortCntrs->dot1dTpPortInDiscards                = 0;
    pPortCntrs->dot1dTpHcPortInDiscards              = 0;
    pPortCntrs->dot3InPauseFrames                    = data.rx_pause_frame_cnt;
    pPortCntrs->dot3OutPauseFrames                   = data.tx_pause_frame_cnt;
    pPortCntrs->dot3OutPauseOnFrames                 = 0;
    pPortCntrs->dot3StatsAligmentErrors              = data.rx_align_err_frame_cnt;
    pPortCntrs->dot3StatsFCSErrors                   = data.rx_crc_error_frame_cnt;
    pPortCntrs->dot3StatsSingleCollisionFrames       = data.tx_single_col_frame_cnt;
    pPortCntrs->dot3StatsMultipleCollisionFrames     = data.tx_multi_col_frame_cnt;
    pPortCntrs->dot3StatsDeferredTransmissions       = 0; //data.phy_stats.frames_with_deferredXmissions;
    pPortCntrs->dot3StatsLateCollisions              = data.tx_late_col_frame_cnt;
    pPortCntrs->dot3StatsExcessiveCollisions         = data.tx_exess_col_frame_cnt;
    pPortCntrs->dot3StatsFrameTooLongs               = 0;
    pPortCntrs->dot3StatsSymbolErrors                = 0; //data.phy_stats.symbol_error_during_carrier;
    pPortCntrs->dot3ControlInUnknownOpcodes          = data.rx_unk_opcode_frame_cnt;
    pPortCntrs->etherStatsDropEvents                 = data.drop_rx_crc_err_frame_cnt + data.drop_rx_align_err_frame_cnt;
    pPortCntrs->etherStatsOctets                     = 0;
    pPortCntrs->etherStatsBcastPkts                  = 0;
    pPortCntrs->etherStatsMcastPkts                  = 0;
    pPortCntrs->etherStatsUndersizePkts              = 0;
    pPortCntrs->etherStatsOversizePkts               = 0;
    pPortCntrs->etherStatsFragments                  = 0;
    pPortCntrs->etherStatsJabbers                    = 0;
    pPortCntrs->etherStatsCollisions                 = data.tx_single_col_frame_cnt + data.tx_multi_col_frame_cnt +
							data.tx_late_col_frame_cnt + data.tx_exess_col_frame_cnt;
    pPortCntrs->etherStatsCRCAlignErrors             = data.rx_crc_error_frame_cnt;
    pPortCntrs->etherStatsPkts64Octets               = 0;
    pPortCntrs->etherStatsPkts65to127Octets          = 0;
    pPortCntrs->etherStatsPkts128to255Octets         = 0;
    pPortCntrs->etherStatsPkts256to511Octets         = 0;
    pPortCntrs->etherStatsPkts512to1023Octets        = 0;
    pPortCntrs->etherStatsPkts1024to1518Octets       = 0;
    pPortCntrs->etherStatsTxOctets                   = 0;
    pPortCntrs->etherStatsTxUndersizePkts            = 0;
    pPortCntrs->etherStatsTxOversizePkts             = 0;
    pPortCntrs->etherStatsTxPkts64Octets             = data.tx_frame_64_cnt;         /* transmitted frames with 64 bytes */
    pPortCntrs->etherStatsTxPkts65to127Octets        = data.tx_frame_65to127_cnt;    /* transmitted frames with 65 ~ 127 bytes */
    pPortCntrs->etherStatsTxPkts128to255Octets       = data.tx_frame_128to255_cnt;   /* transmitted frames with 128 ~ 255 bytes */
    pPortCntrs->etherStatsTxPkts256to511Octets       = data.tx_frame_256to511_cnt;   /* transmitted frames with 256 ~ 511 bytes */
    pPortCntrs->etherStatsTxPkts512to1023Octets      = data.tx_frame_512to1023_cnt;  /* transmitted frames with 512 ~ 1023 bytes */
    pPortCntrs->etherStatsTxPkts1024to1518Octets     = data.tx_frame_1024to1518_cnt; /* transmitted frames with 1024 ~ 1518 bytes */

    /* transmitted frames of which length is equal to or greater than 1519 bytes */
    pPortCntrs->etherStatsTxPkts1519toMaxOctets      = data.tx_frame_1519to2100_cnt +
							data.tx_frame_2101to9200_cnt +
							data.tx_frame_9201tomax_cnt;
    pPortCntrs->etherStatsTxBcastPkts                = data.tx_bc_frame_cnt;         /* transmitted broadcast frames */
    pPortCntrs->etherStatsTxMcastPkts                = data.tx_mc_frame_cnt;         /* transmitted multicast frames */
    pPortCntrs->etherStatsTxFragments                = 0;
    pPortCntrs->etherStatsTxJabbers                  = 0;
    pPortCntrs->etherStatsTxCRCAlignErrors           = 0;
    pPortCntrs->etherStatsRxUndersizePkts            = data.rx_undersize_frame_cnt;  /* received frames undersize frame without FCS err */
    pPortCntrs->etherStatsRxUndersizeDropPkts        = data.rx_runt_frame_cnt - data.rx_undersize_frame_cnt;
    pPortCntrs->etherStatsRxOversizePkts             = 0;
    pPortCntrs->etherStatsRxPkts64Octets             = data.tx_frame_64_cnt;         /* received frames with 64 bytes */
    pPortCntrs->etherStatsRxPkts65to127Octets        = data.rx_frame_65to127_cnt;    /* received frames with 65 ~ 127 bytes */
    pPortCntrs->etherStatsRxPkts128to255Octets       = data.rx_frame_128to255_cnt;   /* received frames with 128 ~ 255 bytes */
    pPortCntrs->etherStatsRxPkts256to511Octets       = data.rx_frame_256to511_cnt;   /* received frames with 256 ~ 511 bytes */
    pPortCntrs->etherStatsRxPkts512to1023Octets      = data.rx_frame_512to1023_cnt;  /* received frames with 512 ~ 1023 bytes */
    pPortCntrs->etherStatsRxPkts1024to1518Octets     = data.rx_frame_1024to1518_cnt; /* received frames with 1024 ~ 1518 bytes */
    pPortCntrs->etherStatsRxPkts1519toMaxOctets      = data.rx_frame_1519to2100_cnt +
							data.rx_frame_2101to9200_cnt +
							data.rx_frame_9201tomax_cnt;  /* received frames of which length is equal to or greater than 1519 bytes */
    pPortCntrs->inOampduPkts                         = 0; //TBD waiting for Saturn Chip
    pPortCntrs->outOampduPkts                        = 0; //TBD waiting for Saturn Chip

    return RT_ERR_OK;
}
