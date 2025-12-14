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
#include <rtk/rt/rt_gpon.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_stat.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_epon.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_gpon.h>

#include <osal/time.h>
#include "osal_spinlock.h"

#include <cortina-api/include/port.h>
#include <cortina-api/include/eth_port.h>
#include <cortina-api/include/epon.h>
#include <cortina-api/include/l2.h>
#include <aal_ni_l2.h>
#include <aal_l2_te.h>
#include <aal_l3_te.h>
#include <aal_pon.h>
#include <aal_xgpon.h>
#include <aal_gpon.h>
#include <aal_port.h>
#include <aal_arb.h>
#include <aal_sds.h>
#include <aal_vlan_cnt.h>
#include <ca-ne-rtk/ca_ext.h>

/*
 * Include Files
 */

/*
 * Symbol Definition
 */
#ifdef CONFIG_CORTINA_BOARD_SATURN_SFU_TITAN
#define AAL_FOR_ALL_USR_XGEM_DO(gem) \
        for ((gem) = 1; (gem) <= AAL_XGPON_MAX_GEM_PORT_IDX; (gem)++)

#define AAL_FOR_ALL_OMCI_XGEM_DO(gem) \
        for ((gem) = 0; (gem) <= 0; (gem)++)
#else
#define AAL_FOR_ALL_USR_XGEM_DO(gem) \
        for ((gem) = 8; (gem) <= AAL_XGPON_MAX_GEM_PORT_IDX; (gem)++)

#define AAL_FOR_ALL_OMCI_XGEM_DO(gem) \
        for ((gem) = 0; (gem) <= 7; (gem)++)

#define AAL_FOR_ALL_USR_GEM_DO(gem) \
        for ((gem) = 8; (gem) <= AAL_GPON_SYSTEM_MAX_GEM_NUM; (gem)++)
#endif

//reference to storm control
#define PKT_TYPE_POL_ID_UMC (10)
#define PKT_TYPE_POL_ID_BC (9)
#define RTL9607F_MAX_GEM_FLOW (248)


/*
 * Data Declaration
 */
typedef struct {
    ca_uint32_t frame_cnt;
    ca_uint64_t byte_cnt;
} dal_mib_t;

static uint32    stat_init = INIT_NOT_COMPLETED;
static dal_port_cntr_t lanPortCntrs[17];

rt_gpon_flow_counter_t g9607fUsGemCnts[AAL_XGPON_GEM_PORT_NUM];
rt_gpon_flow_counter_t g9607fDsGemCnts[AAL_XGPON_GEM_PORT_NUM];
static dal_mib_t g9607fPonUsMcCnt;
static dal_mib_t g9607fPonUsBcCnt;
static dal_mib_t g9607fPonDsMcCnt;
static dal_mib_t g9607fPonDsBcCnt;
dal_mib_t g9607fPonUsCnt;
dal_mib_t g9607fPonDsCnt;
uint8 usSidMap[RTL9607F_MAX_GEM_FLOW];


/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      dal_rtl9607f_stat_init
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
dal_rtl9607f_stat_init(void)
{
    int32 ret;
    rtk_port_t port;


    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_rtl9607f_stat_port_reset(port)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
        }
    }

#if defined(CONFIG_10G_EPON_FEATURE) || defined(CONFIG_EPON_FEATURE)
    /*reset epon mib*/
    if((ret = dal_rt_rtl9607f_epon_mibCounter_clear()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
    }
#endif

    memset(usSidMap, 0xff, sizeof(uint8) * RTL9607F_MAX_GEM_FLOW);

    stat_init = INIT_COMPLETED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9607f_stat_port_reset
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
dal_rtl9607f_stat_port_reset(rtk_port_t port)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t read_clear;
    ca_eth_port_stats_t data;
    ca_uint32_t pon_mode = CA_PON_MODE;

    /* check Init status */
    RT_INIT_CHK(stat_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if(HAL_IS_PON_PORT(port))
    {
        read_clear = 1;
        pon_mode = aal_pon_mac_mode_get(0);

        if(CA_PON_MODE_IS_EPON_FAMILY(pon_mode))
        {
            aal_epon_mac_stats_port_eth_t  stats_port_eth;
            aal_epon_mac_stats_port_mpcp_t stats_port_mpcp;
            aal_epon_mac_stats_port_mpcp_ctrl_t mpcp_mac_ctrl;

            memset(&stats_port_eth, 0, sizeof(stats_port_eth));
            memset(&stats_port_mpcp, 0, sizeof(stats_port_mpcp));
            memset(&mpcp_mac_ctrl, 0, sizeof(mpcp_mac_ctrl));

            __PON_LOCK();
            ret = aal_epon_mac_mib_rc_set(0, port_id, read_clear);
            __PON_UNLOCK();

            __PON_LOCK();
            ret = aal_epon_mac_stats_port_eth_get(0, port_id, &stats_port_eth);
            __PON_UNLOCK();

            __PON_LOCK();
            ret = aal_epon_mac_stats_port_mpcp_get(0, port_id, &stats_port_mpcp);
            __PON_UNLOCK();

            __PON_LOCK();
            ret = aal_epon_mac_stats_port_mpcp_ctrl_get(0, port_id, &mpcp_mac_ctrl);
            __PON_UNLOCK();
                    }
        else if (CA_PON_MODE_IS_GPON_FAMILY(pon_mode))
                    {
            aal_ni_mib_stats_t ni_mib;

            read_clear = 1;
            memset(&ni_mib, 0, sizeof(aal_ni_mib_stats_t)); 
            ret = aal_ni_eth_port_mib_get(0, port_id, read_clear, &ni_mib);
        }
        else if(CA_PON_MODE_IS_ETHERNET(pon_mode))
        {
            aal_ni_mib_stats_t ni_mib;

            read_clear = 1;
            memset(&ni_mib, 0, sizeof(aal_ni_mib_stats_t));
            ret = aal_ni_pon_mac_mib_get(0, port_id, read_clear, &ni_mib);
        }
        else
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
        aal_ni_mib_stats_t ni_mib;
        
        read_clear = 1;
        memset(&ni_mib, 0, sizeof(aal_ni_mib_stats_t)); 
        ret = aal_ni_eth_port_mib_get(0, port_id, read_clear, &ni_mib);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9607f_stat_port_get
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
dal_rtl9607f_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntrIdx, uint64 *pCntr)
{
    int32 ret;
    rtk_stat_port_cntr_t portCntrs;
    ca_uint32_t pon_mode = CA_PON_MODE;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK((MIB_PORT_CNTR_END <= cntrIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((port > CA_PORT_ID_CPU7), RT_ERR_PORT_ID);

    if ((ret = dal_rtl9607f_stat_port_getAll(port,&portCntrs)) != RT_ERR_OK)
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
            *pCntr = lanPortCntrs[port].ifInMulticastPkts;
            break;
        case IF_IN_BROADCAST_PKTS_INDEX:
            *pCntr = lanPortCntrs[port].ifInBroadcastPkts;
            break;
        case IF_IN_ERRORS_INDEX:
            *pCntr = lanPortCntrs[port].ifInErrors;
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
        case DOT3_STATS_FCS_ERRORS_INDEX:
            *pCntr = lanPortCntrs[port].dot3StatsFCSErrors;
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
        case IF_IN_ERR_OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].ifInErrOctets;
            break;
        case IF_OUT_ERR_OCTETS_INDEX:
            *pCntr = lanPortCntrs[port].ifOutErrOctets;
            break;
        default:
            *pCntr = 0;
            break;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9607f_stat_port_getAll
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
dal_rtl9607f_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_boolean_t read_clear;
    ca_uint32_t pon_mode = CA_PON_MODE;

    /* check Init status */
    RT_INIT_CHK(stat_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortCntrs), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((port > CA_PORT_ID_CPU7), RT_ERR_PORT_ID);

    memset(pPortCntrs, 0, sizeof(rtk_stat_port_cntr_t));

    if(HAL_IS_PON_PORT(port))
    {
        read_clear = 0;
        pon_mode = aal_pon_mac_mode_get(0);

        if (CA_PON_MODE_IS_GPON_FAMILY(pon_mode))
        {
        aal_ni_mib_stats_t ni_mib;

        read_clear = 0;
        memset(&ni_mib, 0, sizeof(aal_ni_mib_stats_t)); 
        ret = aal_ni_eth_port_mib_get(0, port_id, read_clear, &ni_mib);
        
        pPortCntrs->ifInOctets                           = lanPortCntrs[7].ifInOctets                           = ni_mib.rx_byte_cnt;
        pPortCntrs->ifInUcastPkts                        = lanPortCntrs[7].ifInUcastPkts                        = ni_mib.rx_uc_pkt_cnt;     
        pPortCntrs->ifInMulticastPkts                    = lanPortCntrs[7].ifInMulticastPkts                    = ni_mib.rx_mc_frame_cnt;
        pPortCntrs->ifInBroadcastPkts                    = lanPortCntrs[7].ifInBroadcastPkts                    = ni_mib.rx_bc_frame_cnt;   
        pPortCntrs->ifInDiscards                         = lanPortCntrs[7].ifInDiscards                         = 0;
        pPortCntrs->ifOutOctets                          = lanPortCntrs[7].ifOutOctets                          = ni_mib.tx_byte_cnt;
        pPortCntrs->ifOutDiscards                        = lanPortCntrs[7].ifOutDiscards                        = 0; 
        pPortCntrs->ifInErrors                           = lanPortCntrs[7].ifInErrors                           = ni_mib.rx_invalid_frame_cnt;
        pPortCntrs->ifOutUcastPkts                       = lanPortCntrs[7].ifOutUcastPkts                       = ni_mib.tx_uc_pkt_cnt;  
        pPortCntrs->ifOutMulticastPkts                   = lanPortCntrs[7].ifOutMulticastPkts                   = ni_mib.tx_mc_frame_cnt;
        pPortCntrs->ifOutBrocastPkts                     = lanPortCntrs[7].ifOutBrocastPkts                     = ni_mib.tx_bc_frame_cnt;
        pPortCntrs->dot1dBasePortDelayExceededDiscards   = lanPortCntrs[7].dot1dBasePortDelayExceededDiscards   = 0;
        pPortCntrs->dot1dTpPortInDiscards                = lanPortCntrs[7].dot1dTpPortInDiscards                = 0;
        pPortCntrs->dot1dTpHcPortInDiscards              = lanPortCntrs[7].dot1dTpHcPortInDiscards              = 0;
        pPortCntrs->dot3InPauseFrames                    = lanPortCntrs[7].dot3InPauseFrames                    = ni_mib.rx_pause_frame_cnt;
        pPortCntrs->dot3OutPauseFrames                   = lanPortCntrs[7].dot3OutPauseFrames                   = ni_mib.tx_pause_frame_cnt;
        pPortCntrs->dot3OutPauseOnFrames                 = lanPortCntrs[7].dot3OutPauseOnFrames                 = 0;
        pPortCntrs->dot3StatsAligmentErrors              = lanPortCntrs[7].dot3StatsAligmentErrors              = 0; //data.phy_stats.alignmentErrors;?               
        pPortCntrs->dot3StatsFCSErrors                   = lanPortCntrs[7].dot3StatsFCSErrors                   = ni_mib.rx_crc_error_frame_cnt;
        pPortCntrs->dot3StatsSingleCollisionFrames       = lanPortCntrs[7].dot3StatsSingleCollisionFrames       = ni_mib.tx_single_col_frame_cnt;
        pPortCntrs->dot3StatsMultipleCollisionFrames     = lanPortCntrs[7].dot3StatsMultipleCollisionFrames     = ni_mib.tx_multi_col_frame_cnt;
        pPortCntrs->dot3StatsDeferredTransmissions       = lanPortCntrs[7].dot3StatsDeferredTransmissions       = 0; //data.phy_stats.frames_with_deferredXmissions;?
        pPortCntrs->dot3StatsLateCollisions              = lanPortCntrs[7].dot3StatsLateCollisions              = ni_mib.tx_late_col_frame_cnt;
        pPortCntrs->dot3StatsExcessiveCollisions         = lanPortCntrs[7].dot3StatsExcessiveCollisions         = 0;
        pPortCntrs->dot3StatsFrameTooLongs               = lanPortCntrs[7].dot3StatsFrameTooLongs               = 0;
        pPortCntrs->dot3StatsSymbolErrors                = lanPortCntrs[7].dot3StatsSymbolErrors                = 0; //data.phy_stats.symbol_error_during_carrier;?
        pPortCntrs->dot3ControlInUnknownOpcodes          = lanPortCntrs[7].dot3ControlInUnknownOpcodes          = ni_mib.rx_unk_opcode_frame_cnt;
        pPortCntrs->etherStatsDropEvents                 = lanPortCntrs[7].etherStatsDropEvents                 = 0;
        pPortCntrs->etherStatsOctets                     = lanPortCntrs[7].etherStatsOctets                     = 0;
        pPortCntrs->etherStatsBcastPkts                  = lanPortCntrs[7].etherStatsBcastPkts                  = 0;
        pPortCntrs->etherStatsMcastPkts                  = lanPortCntrs[7].etherStatsMcastPkts                  = 0;
        pPortCntrs->etherStatsUndersizePkts              = lanPortCntrs[7].etherStatsUndersizePkts              = 0;
        pPortCntrs->etherStatsOversizePkts               = lanPortCntrs[7].etherStatsOversizePkts               = 0;
        pPortCntrs->etherStatsFragments                  = lanPortCntrs[7].etherStatsFragments                  = 0;
        pPortCntrs->etherStatsJabbers                    = lanPortCntrs[7].etherStatsJabbers                    = 0;
        pPortCntrs->etherStatsCollisions                 = lanPortCntrs[7].etherStatsCollisions                 = 0;
        pPortCntrs->etherStatsCRCAlignErrors             = lanPortCntrs[7].etherStatsCRCAlignErrors             = 0;
        pPortCntrs->etherStatsPkts64Octets               = lanPortCntrs[7].etherStatsPkts64Octets               = 0;
        pPortCntrs->etherStatsPkts65to127Octets          = lanPortCntrs[7].etherStatsPkts65to127Octets          = 0;
        pPortCntrs->etherStatsPkts128to255Octets         = lanPortCntrs[7].etherStatsPkts128to255Octets         = 0;
        pPortCntrs->etherStatsPkts256to511Octets         = lanPortCntrs[7].etherStatsPkts256to511Octets         = 0;
        pPortCntrs->etherStatsPkts512to1023Octets        = lanPortCntrs[7].etherStatsPkts512to1023Octets        = 0;
        pPortCntrs->etherStatsPkts1024to1518Octets       = lanPortCntrs[7].etherStatsPkts1024to1518Octets       = 0;
        pPortCntrs->etherStatsTxOctets                   = lanPortCntrs[7].etherStatsTxOctets                   = 0;
        pPortCntrs->etherStatsTxUndersizePkts            = lanPortCntrs[7].etherStatsTxUndersizePkts            = 0;
        pPortCntrs->etherStatsTxOversizePkts             = lanPortCntrs[7].etherStatsTxOversizePkts             = 0;
        pPortCntrs->etherStatsTxPkts64Octets             = lanPortCntrs[7].etherStatsTxPkts64Octets             = ni_mib.tx_frame_64_cnt;
        pPortCntrs->etherStatsTxPkts65to127Octets        = lanPortCntrs[7].etherStatsTxPkts65to127Octets        = ni_mib.tx_frame_65to127_cnt;
        pPortCntrs->etherStatsTxPkts128to255Octets       = lanPortCntrs[7].etherStatsTxPkts128to255Octets       = ni_mib.tx_frame_128to255_cnt;
        pPortCntrs->etherStatsTxPkts256to511Octets       = lanPortCntrs[7].etherStatsTxPkts256to511Octets       = ni_mib.tx_frame_256to511_cnt;
        pPortCntrs->etherStatsTxPkts512to1023Octets      = lanPortCntrs[7].etherStatsTxPkts512to1023Octets      = ni_mib.tx_frame_512to1023_cnt;
        pPortCntrs->etherStatsTxPkts1024to1518Octets     = lanPortCntrs[7].etherStatsTxPkts1024to1518Octets     = ni_mib.tx_frame_1024to1518_cnt;
        pPortCntrs->etherStatsTxPkts1519toMaxOctets      = lanPortCntrs[7].etherStatsTxPkts1519toMaxOctets      = ni_mib.tx_frame_1519to2100_cnt + ni_mib.tx_frame_2101to9200_cnt + ni_mib.tx_frame_9201tomax_cnt;
        pPortCntrs->etherStatsTxBcastPkts                = lanPortCntrs[7].etherStatsTxBcastPkts                = ni_mib.tx_bc_frame_cnt;
        pPortCntrs->etherStatsTxMcastPkts                = lanPortCntrs[7].etherStatsTxMcastPkts                = ni_mib.tx_mc_frame_cnt;
        pPortCntrs->etherStatsTxFragments                = lanPortCntrs[7].etherStatsTxFragments                = 0;
        pPortCntrs->etherStatsTxJabbers                  = lanPortCntrs[7].etherStatsTxJabbers                  = 0;
        pPortCntrs->etherStatsTxCRCAlignErrors           = lanPortCntrs[7].etherStatsTxCRCAlignErrors           = 0;
        pPortCntrs->etherStatsRxUndersizePkts            = lanPortCntrs[7].etherStatsRxUndersizePkts            = ni_mib.rx_undersize_frame_cnt;
        pPortCntrs->etherStatsRxUndersizeDropPkts        = lanPortCntrs[7].etherStatsRxUndersizeDropPkts        = ni_mib.rx_runt_frame_cnt - ni_mib.rx_undersize_frame_cnt;
        pPortCntrs->etherStatsRxOversizePkts             = lanPortCntrs[7].etherStatsRxOversizePkts             = ni_mib.rx_ovsize_frame_cnt;
        pPortCntrs->etherStatsRxPkts64Octets             = lanPortCntrs[7].etherStatsRxPkts64Octets             = ni_mib.rx_frame_64_cnt;
        pPortCntrs->etherStatsRxPkts65to127Octets        = lanPortCntrs[7].etherStatsRxPkts65to127Octets        = ni_mib.rx_frame_65to127_cnt;
        pPortCntrs->etherStatsRxPkts128to255Octets       = lanPortCntrs[7].etherStatsRxPkts128to255Octets       = ni_mib.rx_frame_128to255_cnt;
        pPortCntrs->etherStatsRxPkts256to511Octets       = lanPortCntrs[7].etherStatsRxPkts256to511Octets       = ni_mib.rx_frame_256to511_cnt;
        pPortCntrs->etherStatsRxPkts512to1023Octets      = lanPortCntrs[7].etherStatsRxPkts512to1023Octets      = ni_mib.rx_frame_512to1023_cnt;
        pPortCntrs->etherStatsRxPkts1024to1518Octets     = lanPortCntrs[7].etherStatsRxPkts1024to1518Octets     = ni_mib.rx_frame_1024to1518_cnt;
        pPortCntrs->etherStatsRxPkts1519toMaxOctets      = lanPortCntrs[7].etherStatsRxPkts1519toMaxOctets      = ni_mib.rx_frame_1519to2100_cnt + ni_mib.tx_frame_2101to9200_cnt + ni_mib.tx_frame_9201tomax_cnt;
        pPortCntrs->inOampduPkts                         = lanPortCntrs[7].inOampduPkts                         = 0; //TBD waiting for Saturn Chip
        pPortCntrs->outOampduPkts                        = lanPortCntrs[7].outOampduPkts                        = 0; //TBD waiting for Saturn Chip
        pPortCntrs->ifInErrOctets                        = lanPortCntrs[7].ifInErrOctets                        = ni_mib.rx_all_byte_cnt-ni_mib.rx_byte_cnt;
        pPortCntrs->ifOutErrOctets                       = lanPortCntrs[7].ifOutErrOctets                       = ni_mib.tx_all_byte_cnt-ni_mib.tx_byte_cnt;
        }
        else if(CA_PON_MODE_IS_EPON_FAMILY(pon_mode))
        {
            aal_epon_mac_stats_port_eth_t  stats_port_eth;
            aal_epon_mac_stats_port_mpcp_t stats_port_mpcp;
            aal_epon_mac_stats_port_mpcp_ctrl_t mpcp_mac_ctrl;
            
            memset(&stats_port_eth, 0, sizeof(stats_port_eth));
            memset(&stats_port_mpcp, 0, sizeof(stats_port_mpcp));
            memset(&mpcp_mac_ctrl, 0, sizeof(mpcp_mac_ctrl));

            __PON_LOCK();
            ret = aal_epon_mac_mib_rc_set(0, port_id, read_clear);
            __PON_UNLOCK();
            
            __PON_LOCK();
            ret = aal_epon_mac_stats_port_eth_get(0, port_id, &stats_port_eth);
            __PON_UNLOCK();
            
            __PON_LOCK();
            ret = aal_epon_mac_stats_port_mpcp_get(0, port_id, &stats_port_mpcp);
            __PON_UNLOCK();
            
            __PON_LOCK();
            ret = aal_epon_mac_stats_port_mpcp_ctrl_get(0, port_id, &mpcp_mac_ctrl);
            __PON_UNLOCK();

            pPortCntrs->ifInOctets                           = lanPortCntrs[7].ifInOctets                           = (uint64)stats_port_eth.rx_bytecnt_hi << 32 | (uint64)stats_port_eth.rx_bytecnt_lo; //epon_stats.rx_good_octets;
            pPortCntrs->ifInUcastPkts                        = lanPortCntrs[7].ifInUcastPkts                        = stats_port_eth.rx_ucframecnt;   /* received unicast frames                                                   */
            pPortCntrs->ifInMulticastPkts                    = lanPortCntrs[7].ifInMulticastPkts                    = stats_port_eth.rx_mcframecnt;   /* received multicast frames                                                 */
            pPortCntrs->ifInBroadcastPkts                    = lanPortCntrs[7].ifInBroadcastPkts                    = stats_port_eth.rx_bcframecnt;   /* received broadcast frames                                                 */
            pPortCntrs->ifInDiscards                         = lanPortCntrs[7].ifInDiscards                         = 0;
            pPortCntrs->ifOutOctets                          = lanPortCntrs[7].ifOutOctets                          = (uint64)stats_port_eth.tx_bytecnt_hi << 32 | (uint64)stats_port_eth.tx_bytecnt_lo;     
            pPortCntrs->ifOutDiscards                        = lanPortCntrs[7].ifOutDiscards                        = 0;
            pPortCntrs->ifInErrors                           = lanPortCntrs[7].ifInErrors                           = 0;
            pPortCntrs->ifOutUcastPkts                       = lanPortCntrs[7].ifOutUcastPkts                       = stats_port_eth.tx_ucframecnt;   /* transmitted unicast frames                                                */
            pPortCntrs->ifOutMulticastPkts                   = lanPortCntrs[7].ifOutMulticastPkts                   = stats_port_eth.tx_mcframecnt;   /* transmitted multicast frames                                              */
            pPortCntrs->ifOutBrocastPkts                     = lanPortCntrs[7].ifOutBrocastPkts                     = stats_port_eth.tx_bcframecnt;   /* transmitted broadcast frames                                              */
            pPortCntrs->dot1dBasePortDelayExceededDiscards   = lanPortCntrs[7].dot1dBasePortDelayExceededDiscards   = 0;
            pPortCntrs->dot1dTpPortInDiscards                = lanPortCntrs[7].dot1dTpPortInDiscards                = 0;
            pPortCntrs->dot1dTpHcPortInDiscards              = lanPortCntrs[7].dot1dTpHcPortInDiscards              = 0;
            pPortCntrs->dot3InPauseFrames                    = lanPortCntrs[7].dot3InPauseFrames                    = stats_port_eth.rx_pausecnt;
            pPortCntrs->dot3OutPauseFrames                   = lanPortCntrs[7].dot3OutPauseFrames                   = stats_port_eth.tx_pausecnt;
            pPortCntrs->dot3OutPauseOnFrames                 = lanPortCntrs[7].dot3OutPauseOnFrames                 = 0;
            pPortCntrs->dot3StatsAligmentErrors              = lanPortCntrs[7].dot3StatsAligmentErrors              = 0;
            pPortCntrs->dot3StatsFCSErrors                   = lanPortCntrs[7].dot3StatsFCSErrors                   = 0; //epon_stats.rx_crc8_sld_err
            pPortCntrs->dot3StatsSingleCollisionFrames       = lanPortCntrs[7].dot3StatsSingleCollisionFrames       = 0;
            pPortCntrs->dot3StatsMultipleCollisionFrames     = lanPortCntrs[7].dot3StatsMultipleCollisionFrames     = 0;
            pPortCntrs->dot3StatsDeferredTransmissions       = lanPortCntrs[7].dot3StatsDeferredTransmissions       = 0;
            pPortCntrs->dot3StatsLateCollisions              = lanPortCntrs[7].dot3StatsLateCollisions              = 0;
            pPortCntrs->dot3StatsExcessiveCollisions         = lanPortCntrs[7].dot3StatsExcessiveCollisions         = 0;
            pPortCntrs->dot3StatsFrameTooLongs               = lanPortCntrs[7].dot3StatsFrameTooLongs               = 0;
            pPortCntrs->dot3StatsSymbolErrors                = lanPortCntrs[7].dot3StatsSymbolErrors                = 0;
            pPortCntrs->dot3ControlInUnknownOpcodes          = lanPortCntrs[7].dot3ControlInUnknownOpcodes          = 0;
            pPortCntrs->etherStatsDropEvents                 = lanPortCntrs[7].etherStatsDropEvents                 = 0;
            pPortCntrs->etherStatsOctets                     = lanPortCntrs[7].etherStatsOctets                     = 0;
            pPortCntrs->etherStatsBcastPkts                  = lanPortCntrs[7].etherStatsBcastPkts                  = 0;
            pPortCntrs->etherStatsMcastPkts                  = lanPortCntrs[7].etherStatsMcastPkts                  = 0;
            pPortCntrs->etherStatsUndersizePkts              = lanPortCntrs[7].etherStatsUndersizePkts              = 0;
            pPortCntrs->etherStatsOversizePkts               = lanPortCntrs[7].etherStatsOversizePkts               = 0;
            pPortCntrs->etherStatsFragments                  = lanPortCntrs[7].etherStatsFragments                  = 0;
            pPortCntrs->etherStatsJabbers                    = lanPortCntrs[7].etherStatsJabbers                    = 0;
            pPortCntrs->etherStatsCollisions                 = lanPortCntrs[7].etherStatsCollisions                 = 0;
            pPortCntrs->etherStatsCRCAlignErrors             = lanPortCntrs[7].etherStatsCRCAlignErrors             = 0;
            pPortCntrs->etherStatsPkts64Octets               = lanPortCntrs[7].etherStatsPkts64Octets               = 0;
            pPortCntrs->etherStatsPkts65to127Octets          = lanPortCntrs[7].etherStatsPkts65to127Octets          = 0;
            pPortCntrs->etherStatsPkts128to255Octets         = lanPortCntrs[7].etherStatsPkts128to255Octets         = 0;
            pPortCntrs->etherStatsPkts256to511Octets         = lanPortCntrs[7].etherStatsPkts256to511Octets         = 0;
            pPortCntrs->etherStatsPkts512to1023Octets        = lanPortCntrs[7].etherStatsPkts512to1023Octets        = 0;
            pPortCntrs->etherStatsPkts1024to1518Octets       = lanPortCntrs[7].etherStatsPkts1024to1518Octets       = 0;
            pPortCntrs->etherStatsTxOctets                   = lanPortCntrs[7].etherStatsTxOctets                   = 0;
            pPortCntrs->etherStatsTxUndersizePkts            = lanPortCntrs[7].etherStatsTxUndersizePkts            = 0;
            pPortCntrs->etherStatsTxOversizePkts             = lanPortCntrs[7].etherStatsTxOversizePkts             = 0;
            pPortCntrs->etherStatsTxPkts64Octets             = lanPortCntrs[7].etherStatsTxPkts64Octets             = stats_port_eth.tx_rmon64cnt;          /* transmitted frames with 64 bytes                                          */
            pPortCntrs->etherStatsTxPkts65to127Octets        = lanPortCntrs[7].etherStatsTxPkts65to127Octets        = stats_port_eth.tx_rmon65_127cnt;      /* transmitted frames with 65 ~ 127 bytes                                    */
            pPortCntrs->etherStatsTxPkts128to255Octets       = lanPortCntrs[7].etherStatsTxPkts128to255Octets       = stats_port_eth.tx_rmon128_255cnt;     /* transmitted frames with 128 ~ 255 bytes                                   */
            pPortCntrs->etherStatsTxPkts256to511Octets       = lanPortCntrs[7].etherStatsTxPkts256to511Octets       = stats_port_eth.tx_rmon256_511cnt;     /* transmitted frames with 256 ~ 511 bytes                                   */
            pPortCntrs->etherStatsTxPkts512to1023Octets      = lanPortCntrs[7].etherStatsTxPkts512to1023Octets      = stats_port_eth.tx_rmon512_1023cnt;    /* transmitted frames with 512 ~ 1023 bytes                                  */
            pPortCntrs->etherStatsTxPkts1024to1518Octets     = lanPortCntrs[7].etherStatsTxPkts1024to1518Octets     = stats_port_eth.tx_rmon1024_1518cnt;   /* transmitted frames with 1024 ~ 1518 bytes                                 */
            pPortCntrs->etherStatsTxPkts1519toMaxOctets      = lanPortCntrs[7].etherStatsTxPkts1519toMaxOctets      = stats_port_eth.tx_rmon1519_1533cnt + stats_port_eth.tx_rmon1534_2299cnt +
                                                                                                                      stats_port_eth.tx_rmon2300_2599cnt + stats_port_eth.tx_rmon2600_maxcnt;  
                                                                                                                                                            /* transmitted frames of which length is equal to or greater than 1519 bytes */
            pPortCntrs->etherStatsTxBcastPkts                = lanPortCntrs[7].etherStatsTxBcastPkts                = stats_port_eth.tx_bcframecnt;         /* transmitted broadcast frames                                              */
            pPortCntrs->etherStatsTxMcastPkts                = lanPortCntrs[7].etherStatsTxMcastPkts                = stats_port_eth.tx_mcframecnt;         /* transmitted multicast frames                                              */
            pPortCntrs->etherStatsTxFragments                = lanPortCntrs[7].etherStatsTxFragments                = 0;
            pPortCntrs->etherStatsTxJabbers                  = lanPortCntrs[7].etherStatsTxJabbers                  = 0;
            pPortCntrs->etherStatsTxCRCAlignErrors           = lanPortCntrs[7].etherStatsTxCRCAlignErrors           = 0;
            pPortCntrs->etherStatsRxUndersizePkts            = lanPortCntrs[7].etherStatsRxUndersizePkts            = stats_port_eth.rx_undersizecnt;       /* received frames undersize frame without FCS err               */
            pPortCntrs->etherStatsRxUndersizeDropPkts        = lanPortCntrs[7].etherStatsRxUndersizeDropPkts        = 0;
            pPortCntrs->etherStatsRxOversizePkts             = lanPortCntrs[7].etherStatsRxOversizePkts             = stats_port_eth.rx_oversizecnt;
            pPortCntrs->etherStatsRxPkts64Octets             = lanPortCntrs[7].etherStatsRxPkts64Octets             = stats_port_eth.rx_rmon64cnt;          /* received frames with 64 bytes                                             */
            pPortCntrs->etherStatsRxPkts65to127Octets        = lanPortCntrs[7].etherStatsRxPkts65to127Octets        = stats_port_eth.rx_rmon65_127cnt;      /* received frames with 65 ~ 127 bytes                                       */
            pPortCntrs->etherStatsRxPkts128to255Octets       = lanPortCntrs[7].etherStatsRxPkts128to255Octets       = stats_port_eth.rx_rmon128_255cnt;     /* received frames with 128 ~ 255 bytes                                      */
            pPortCntrs->etherStatsRxPkts256to511Octets       = lanPortCntrs[7].etherStatsRxPkts256to511Octets       = stats_port_eth.rx_rmon256_511cnt;     /* received frames with 256 ~ 511 bytes                                      */
            pPortCntrs->etherStatsRxPkts512to1023Octets      = lanPortCntrs[7].etherStatsRxPkts512to1023Octets      = stats_port_eth.rx_rmon512_1023cnt;    /* received frames with 512 ~ 1023 bytes                                     */
            pPortCntrs->etherStatsRxPkts1024to1518Octets     = lanPortCntrs[7].etherStatsRxPkts1024to1518Octets     = stats_port_eth.rx_rmon1024_1518cnt;   /* received frames with 1024 ~ 1518 bytes                                    */
            pPortCntrs->etherStatsRxPkts1519toMaxOctets      = lanPortCntrs[7].etherStatsRxPkts1519toMaxOctets      = stats_port_eth.rx_rmon1519_1533cnt + stats_port_eth.rx_rmon1534_2299cnt +
                                                                                                                      stats_port_eth.rx_rmon2300_2599cnt + stats_port_eth.rx_rmon2600_maxcnt;        
                                                                                                                                                            /* received frames of which length is equal to or greater than 1519 bytes    */
            pPortCntrs->inOampduPkts                         = lanPortCntrs[7].inOampduPkts                         = stats_port_eth.rx_oamframecnt;
            pPortCntrs->outOampduPkts                        = lanPortCntrs[7].outOampduPkts                        = stats_port_eth.tx_oamframecnt;

        }
        else if(CA_PON_MODE_IS_ETHERNET(pon_mode))
        {
            aal_ni_mib_stats_t ni_mib;

            read_clear = 0;
            memset(&ni_mib, 0, sizeof(aal_ni_mib_stats_t)); 
            ret = aal_ni_pon_mac_mib_get(0, port_id, read_clear, &ni_mib);
            
            pPortCntrs->ifInOctets                           = lanPortCntrs[7].ifInOctets                           = ni_mib.rx_byte_cnt;
            pPortCntrs->ifInUcastPkts                        = lanPortCntrs[7].ifInUcastPkts                        = ni_mib.rx_uc_pkt_cnt;     
            pPortCntrs->ifInMulticastPkts                    = lanPortCntrs[7].ifInMulticastPkts                    = ni_mib.rx_mc_frame_cnt;
            pPortCntrs->ifInBroadcastPkts                    = lanPortCntrs[7].ifInBroadcastPkts                    = ni_mib.rx_bc_frame_cnt;   
            pPortCntrs->ifInDiscards                         = lanPortCntrs[7].ifInDiscards                         = 0;
            pPortCntrs->ifOutOctets                          = lanPortCntrs[7].ifOutOctets                          = ni_mib.tx_byte_cnt;
            pPortCntrs->ifOutDiscards                        = lanPortCntrs[7].ifOutDiscards                        = 0; 
            pPortCntrs->ifInErrors                           = lanPortCntrs[7].ifInErrors                           = ni_mib.rx_invalid_frame_cnt;
            pPortCntrs->ifOutUcastPkts                       = lanPortCntrs[7].ifOutUcastPkts                       = ni_mib.tx_uc_pkt_cnt;  
            pPortCntrs->ifOutMulticastPkts                   = lanPortCntrs[7].ifOutMulticastPkts                   = ni_mib.tx_mc_frame_cnt;
            pPortCntrs->ifOutBrocastPkts                     = lanPortCntrs[7].ifOutBrocastPkts                     = ni_mib.tx_bc_frame_cnt;
            pPortCntrs->dot1dBasePortDelayExceededDiscards   = lanPortCntrs[7].dot1dBasePortDelayExceededDiscards   = 0;
            pPortCntrs->dot1dTpPortInDiscards                = lanPortCntrs[7].dot1dTpPortInDiscards                = 0;
            pPortCntrs->dot1dTpHcPortInDiscards              = lanPortCntrs[7].dot1dTpHcPortInDiscards              = 0;
            pPortCntrs->dot3InPauseFrames                    = lanPortCntrs[7].dot3InPauseFrames                    = ni_mib.rx_pause_frame_cnt;
            pPortCntrs->dot3OutPauseFrames                   = lanPortCntrs[7].dot3OutPauseFrames                   = ni_mib.tx_pause_frame_cnt;
            pPortCntrs->dot3OutPauseOnFrames                 = lanPortCntrs[7].dot3OutPauseOnFrames                 = 0;
            pPortCntrs->dot3StatsAligmentErrors              = lanPortCntrs[7].dot3StatsAligmentErrors              = 0; //data.phy_stats.alignmentErrors;?               
            pPortCntrs->dot3StatsFCSErrors                   = lanPortCntrs[7].dot3StatsFCSErrors                   = ni_mib.rx_crc_error_frame_cnt;
            pPortCntrs->dot3StatsSingleCollisionFrames       = lanPortCntrs[7].dot3StatsSingleCollisionFrames       = ni_mib.tx_single_col_frame_cnt;
            pPortCntrs->dot3StatsMultipleCollisionFrames     = lanPortCntrs[7].dot3StatsMultipleCollisionFrames     = ni_mib.tx_multi_col_frame_cnt;
            pPortCntrs->dot3StatsDeferredTransmissions       = lanPortCntrs[7].dot3StatsDeferredTransmissions       = 0; //data.phy_stats.frames_with_deferredXmissions;?
            pPortCntrs->dot3StatsLateCollisions              = lanPortCntrs[7].dot3StatsLateCollisions              = ni_mib.tx_late_col_frame_cnt;
            pPortCntrs->dot3StatsExcessiveCollisions         = lanPortCntrs[7].dot3StatsExcessiveCollisions         = ni_mib.tx_exess_col_frame_cnt;
            pPortCntrs->dot3StatsFrameTooLongs               = lanPortCntrs[7].dot3StatsFrameTooLongs               = 0;
            pPortCntrs->dot3StatsSymbolErrors                = lanPortCntrs[7].dot3StatsSymbolErrors                = 0; //data.phy_stats.symbol_error_during_carrier;?
            pPortCntrs->dot3ControlInUnknownOpcodes          = lanPortCntrs[7].dot3ControlInUnknownOpcodes          = ni_mib.rx_unk_opcode_frame_cnt;
            pPortCntrs->etherStatsDropEvents                 = lanPortCntrs[7].etherStatsDropEvents                 = 0;
            pPortCntrs->etherStatsOctets                     = lanPortCntrs[7].etherStatsOctets                     = 0;
            pPortCntrs->etherStatsBcastPkts                  = lanPortCntrs[7].etherStatsBcastPkts                  = 0;
            pPortCntrs->etherStatsMcastPkts                  = lanPortCntrs[7].etherStatsMcastPkts                  = 0;
            pPortCntrs->etherStatsUndersizePkts              = lanPortCntrs[7].etherStatsUndersizePkts              = 0;
            pPortCntrs->etherStatsOversizePkts               = lanPortCntrs[7].etherStatsOversizePkts               = 0;
            pPortCntrs->etherStatsFragments                  = lanPortCntrs[7].etherStatsFragments                  = 0;
            pPortCntrs->etherStatsJabbers                    = lanPortCntrs[7].etherStatsJabbers                    = 0;
            pPortCntrs->etherStatsCollisions                 = lanPortCntrs[7].etherStatsCollisions                 = 0;
            pPortCntrs->etherStatsCRCAlignErrors             = lanPortCntrs[7].etherStatsCRCAlignErrors             = 0;
            pPortCntrs->etherStatsPkts64Octets               = lanPortCntrs[7].etherStatsPkts64Octets               = 0;
            pPortCntrs->etherStatsPkts65to127Octets          = lanPortCntrs[7].etherStatsPkts65to127Octets          = 0;
            pPortCntrs->etherStatsPkts128to255Octets         = lanPortCntrs[7].etherStatsPkts128to255Octets         = 0;
            pPortCntrs->etherStatsPkts256to511Octets         = lanPortCntrs[7].etherStatsPkts256to511Octets         = 0;
            pPortCntrs->etherStatsPkts512to1023Octets        = lanPortCntrs[7].etherStatsPkts512to1023Octets        = 0;
            pPortCntrs->etherStatsPkts1024to1518Octets       = lanPortCntrs[7].etherStatsPkts1024to1518Octets       = 0;
            pPortCntrs->etherStatsTxOctets                   = lanPortCntrs[7].etherStatsTxOctets                   = 0;
            pPortCntrs->etherStatsTxUndersizePkts            = lanPortCntrs[7].etherStatsTxUndersizePkts            = 0;
            pPortCntrs->etherStatsTxOversizePkts             = lanPortCntrs[7].etherStatsTxOversizePkts             = 0;
            pPortCntrs->etherStatsTxPkts64Octets             = lanPortCntrs[7].etherStatsTxPkts64Octets             = ni_mib.tx_frame_64_cnt;
            pPortCntrs->etherStatsTxPkts65to127Octets        = lanPortCntrs[7].etherStatsTxPkts65to127Octets        = ni_mib.tx_frame_65to127_cnt;
            pPortCntrs->etherStatsTxPkts128to255Octets       = lanPortCntrs[7].etherStatsTxPkts128to255Octets       = ni_mib.tx_frame_128to255_cnt;
            pPortCntrs->etherStatsTxPkts256to511Octets       = lanPortCntrs[7].etherStatsTxPkts256to511Octets       = ni_mib.tx_frame_256to511_cnt;
            pPortCntrs->etherStatsTxPkts512to1023Octets      = lanPortCntrs[7].etherStatsTxPkts512to1023Octets      = ni_mib.tx_frame_512to1023_cnt;
            pPortCntrs->etherStatsTxPkts1024to1518Octets     = lanPortCntrs[7].etherStatsTxPkts1024to1518Octets     = ni_mib.tx_frame_1024to1518_cnt;
            pPortCntrs->etherStatsTxPkts1519toMaxOctets      = lanPortCntrs[7].etherStatsTxPkts1519toMaxOctets      = ni_mib.tx_frame_1519to2100_cnt + ni_mib.tx_frame_2101to9200_cnt + ni_mib.tx_frame_9201tomax_cnt;
            pPortCntrs->etherStatsTxBcastPkts                = lanPortCntrs[7].etherStatsTxBcastPkts                = ni_mib.tx_bc_frame_cnt;
            pPortCntrs->etherStatsTxMcastPkts                = lanPortCntrs[7].etherStatsTxMcastPkts                = ni_mib.tx_mc_frame_cnt;
            pPortCntrs->etherStatsTxFragments                = lanPortCntrs[7].etherStatsTxFragments                = 0;
            pPortCntrs->etherStatsTxJabbers                  = lanPortCntrs[7].etherStatsTxJabbers                  = 0;
            pPortCntrs->etherStatsTxCRCAlignErrors           = lanPortCntrs[7].etherStatsTxCRCAlignErrors           = 0;
            pPortCntrs->etherStatsRxUndersizePkts            = lanPortCntrs[7].etherStatsRxUndersizePkts            = ni_mib.rx_undersize_frame_cnt;
            pPortCntrs->etherStatsRxUndersizeDropPkts        = lanPortCntrs[7].etherStatsRxUndersizeDropPkts        = ni_mib.rx_runt_frame_cnt - ni_mib.rx_undersize_frame_cnt;
            pPortCntrs->etherStatsRxOversizePkts             = lanPortCntrs[7].etherStatsRxOversizePkts             = ni_mib.rx_ovsize_frame_cnt;
            pPortCntrs->etherStatsRxPkts64Octets             = lanPortCntrs[7].etherStatsRxPkts64Octets             = ni_mib.rx_frame_64_cnt;
            pPortCntrs->etherStatsRxPkts65to127Octets        = lanPortCntrs[7].etherStatsRxPkts65to127Octets        = ni_mib.rx_frame_65to127_cnt;
            pPortCntrs->etherStatsRxPkts128to255Octets       = lanPortCntrs[7].etherStatsRxPkts128to255Octets       = ni_mib.rx_frame_128to255_cnt;
            pPortCntrs->etherStatsRxPkts256to511Octets       = lanPortCntrs[7].etherStatsRxPkts256to511Octets       = ni_mib.rx_frame_256to511_cnt;
            pPortCntrs->etherStatsRxPkts512to1023Octets      = lanPortCntrs[7].etherStatsRxPkts512to1023Octets      = ni_mib.rx_frame_512to1023_cnt;
            pPortCntrs->etherStatsRxPkts1024to1518Octets     = lanPortCntrs[7].etherStatsRxPkts1024to1518Octets     = ni_mib.rx_frame_1024to1518_cnt;
            pPortCntrs->etherStatsRxPkts1519toMaxOctets      = lanPortCntrs[7].etherStatsRxPkts1519toMaxOctets      = ni_mib.rx_frame_1519to2100_cnt + ni_mib.tx_frame_2101to9200_cnt + ni_mib.tx_frame_9201tomax_cnt;
            pPortCntrs->inOampduPkts                         = lanPortCntrs[7].inOampduPkts                         = ni_mib.rx_oam_frame_cnt; 
            pPortCntrs->outOampduPkts                        = lanPortCntrs[7].outOampduPkts                        = ni_mib.tx_oam_frame_cnt;
            pPortCntrs->ifInErrOctets                        = lanPortCntrs[7].ifInErrOctets                        = ni_mib.rx_all_byte_cnt-ni_mib.rx_byte_cnt;
            pPortCntrs->ifOutErrOctets                       = lanPortCntrs[7].ifOutErrOctets                       = ni_mib.tx_all_byte_cnt-ni_mib.tx_byte_cnt;
        }
        else
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

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
        pPortCntrs->ifInErrors                           = lanPortCntrs[port].ifInErrors                           += 0;
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
        aal_ni_mib_stats_t ni_mib;

        read_clear = 0;
        memset(&ni_mib, 0, sizeof(aal_ni_mib_stats_t)); 
        ret = aal_ni_eth_port_mib_get(0, port, read_clear, &ni_mib);

        pPortCntrs->ifInOctets                           = lanPortCntrs[port].ifInOctets                           = ni_mib.rx_byte_cnt;
        pPortCntrs->ifInUcastPkts                        = lanPortCntrs[port].ifInUcastPkts                        = ni_mib.rx_uc_pkt_cnt;     
        pPortCntrs->ifInMulticastPkts                    = lanPortCntrs[port].ifInMulticastPkts                    = ni_mib.rx_mc_frame_cnt;
        pPortCntrs->ifInBroadcastPkts                    = lanPortCntrs[port].ifInBroadcastPkts                    = ni_mib.rx_bc_frame_cnt;   
        pPortCntrs->ifInDiscards                         = lanPortCntrs[port].ifInDiscards                         = 0;
        pPortCntrs->ifOutOctets                          = lanPortCntrs[port].ifOutOctets                          = ni_mib.tx_byte_cnt;
        pPortCntrs->ifOutDiscards                        = lanPortCntrs[port].ifOutDiscards                        = 0; 
        pPortCntrs->ifInErrors                           = lanPortCntrs[port].ifInErrors                           = ni_mib.rx_invalid_frame_cnt;
        pPortCntrs->ifOutUcastPkts                       = lanPortCntrs[port].ifOutUcastPkts                       = ni_mib.tx_uc_pkt_cnt;  
        pPortCntrs->ifOutMulticastPkts                   = lanPortCntrs[port].ifOutMulticastPkts                   = ni_mib.tx_mc_frame_cnt;
        pPortCntrs->ifOutBrocastPkts                     = lanPortCntrs[port].ifOutBrocastPkts                     = ni_mib.tx_bc_frame_cnt;
        pPortCntrs->dot1dBasePortDelayExceededDiscards   = lanPortCntrs[port].dot1dBasePortDelayExceededDiscards   = 0;
        pPortCntrs->dot1dTpPortInDiscards                = lanPortCntrs[port].dot1dTpPortInDiscards                = 0;
        pPortCntrs->dot1dTpHcPortInDiscards              = lanPortCntrs[port].dot1dTpHcPortInDiscards              = 0;
        pPortCntrs->dot3InPauseFrames                    = lanPortCntrs[port].dot3InPauseFrames                    = ni_mib.rx_pause_frame_cnt;
        pPortCntrs->dot3OutPauseFrames                   = lanPortCntrs[port].dot3OutPauseFrames                   = ni_mib.tx_pause_frame_cnt;
        pPortCntrs->dot3OutPauseOnFrames                 = lanPortCntrs[port].dot3OutPauseOnFrames                 = 0;
        pPortCntrs->dot3StatsAligmentErrors              = lanPortCntrs[port].dot3StatsAligmentErrors              = 0; //data.phy_stats.alignmentErrors;?               
        pPortCntrs->dot3StatsFCSErrors                   = lanPortCntrs[port].dot3StatsFCSErrors                   = ni_mib.rx_crc_error_frame_cnt;
        pPortCntrs->dot3StatsSingleCollisionFrames       = lanPortCntrs[port].dot3StatsSingleCollisionFrames       = ni_mib.tx_single_col_frame_cnt;
        pPortCntrs->dot3StatsMultipleCollisionFrames     = lanPortCntrs[port].dot3StatsMultipleCollisionFrames     = ni_mib.tx_multi_col_frame_cnt;
        pPortCntrs->dot3StatsDeferredTransmissions       = lanPortCntrs[port].dot3StatsDeferredTransmissions       = 0; //data.phy_stats.frames_with_deferredXmissions;?
        pPortCntrs->dot3StatsLateCollisions              = lanPortCntrs[port].dot3StatsLateCollisions              = ni_mib.tx_late_col_frame_cnt;
        pPortCntrs->dot3StatsExcessiveCollisions         = lanPortCntrs[port].dot3StatsExcessiveCollisions         = 0;
        pPortCntrs->dot3StatsFrameTooLongs               = lanPortCntrs[port].dot3StatsFrameTooLongs               = 0;
        pPortCntrs->dot3StatsSymbolErrors                = lanPortCntrs[port].dot3StatsSymbolErrors                = 0; //data.phy_stats.symbol_error_during_carrier;?
        pPortCntrs->dot3ControlInUnknownOpcodes          = lanPortCntrs[port].dot3ControlInUnknownOpcodes          = ni_mib.rx_unk_opcode_frame_cnt;
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
        pPortCntrs->etherStatsTxPkts64Octets             = lanPortCntrs[port].etherStatsTxPkts64Octets             = ni_mib.tx_frame_64_cnt;
        pPortCntrs->etherStatsTxPkts65to127Octets        = lanPortCntrs[port].etherStatsTxPkts65to127Octets        = ni_mib.tx_frame_65to127_cnt;
        pPortCntrs->etherStatsTxPkts128to255Octets       = lanPortCntrs[port].etherStatsTxPkts128to255Octets       = ni_mib.tx_frame_128to255_cnt;
        pPortCntrs->etherStatsTxPkts256to511Octets       = lanPortCntrs[port].etherStatsTxPkts256to511Octets       = ni_mib.tx_frame_256to511_cnt;
        pPortCntrs->etherStatsTxPkts512to1023Octets      = lanPortCntrs[port].etherStatsTxPkts512to1023Octets      = ni_mib.tx_frame_512to1023_cnt;
        pPortCntrs->etherStatsTxPkts1024to1518Octets     = lanPortCntrs[port].etherStatsTxPkts1024to1518Octets     = ni_mib.tx_frame_1024to1518_cnt;
        pPortCntrs->etherStatsTxPkts1519toMaxOctets      = lanPortCntrs[port].etherStatsTxPkts1519toMaxOctets      = ni_mib.tx_frame_1519to2100_cnt + ni_mib.tx_frame_2101to9200_cnt + ni_mib.tx_frame_9201tomax_cnt;
        pPortCntrs->etherStatsTxBcastPkts                = lanPortCntrs[port].etherStatsTxBcastPkts                = ni_mib.tx_bc_frame_cnt;
        pPortCntrs->etherStatsTxMcastPkts                = lanPortCntrs[port].etherStatsTxMcastPkts                = ni_mib.tx_mc_frame_cnt;
        pPortCntrs->etherStatsTxFragments                = lanPortCntrs[port].etherStatsTxFragments                = 0;
        pPortCntrs->etherStatsTxJabbers                  = lanPortCntrs[port].etherStatsTxJabbers                  = 0;
        pPortCntrs->etherStatsTxCRCAlignErrors           = lanPortCntrs[port].etherStatsTxCRCAlignErrors           = 0;
        pPortCntrs->etherStatsRxUndersizePkts            = lanPortCntrs[port].etherStatsRxUndersizePkts            = ni_mib.rx_undersize_frame_cnt;
        pPortCntrs->etherStatsRxUndersizeDropPkts        = lanPortCntrs[port].etherStatsRxUndersizeDropPkts        = ni_mib.rx_runt_frame_cnt - ni_mib.rx_undersize_frame_cnt;
        pPortCntrs->etherStatsRxOversizePkts             = lanPortCntrs[port].etherStatsRxOversizePkts             = ni_mib.rx_ovsize_frame_cnt;
        pPortCntrs->etherStatsRxPkts64Octets             = lanPortCntrs[port].etherStatsRxPkts64Octets             = ni_mib.rx_frame_64_cnt;
        pPortCntrs->etherStatsRxPkts65to127Octets        = lanPortCntrs[port].etherStatsRxPkts65to127Octets        = ni_mib.rx_frame_65to127_cnt;
        pPortCntrs->etherStatsRxPkts128to255Octets       = lanPortCntrs[port].etherStatsRxPkts128to255Octets       = ni_mib.rx_frame_128to255_cnt;
        pPortCntrs->etherStatsRxPkts256to511Octets       = lanPortCntrs[port].etherStatsRxPkts256to511Octets       = ni_mib.rx_frame_256to511_cnt;
        pPortCntrs->etherStatsRxPkts512to1023Octets      = lanPortCntrs[port].etherStatsRxPkts512to1023Octets      = ni_mib.rx_frame_512to1023_cnt;
        pPortCntrs->etherStatsRxPkts1024to1518Octets     = lanPortCntrs[port].etherStatsRxPkts1024to1518Octets     = ni_mib.rx_frame_1024to1518_cnt;
        pPortCntrs->etherStatsRxPkts1519toMaxOctets      = lanPortCntrs[port].etherStatsRxPkts1519toMaxOctets      = ni_mib.rx_frame_1519to2100_cnt + ni_mib.tx_frame_2101to9200_cnt + ni_mib.tx_frame_9201tomax_cnt;
        pPortCntrs->inOampduPkts                         = lanPortCntrs[port].inOampduPkts                         = 0; //TBD waiting for Saturn Chip
        pPortCntrs->outOampduPkts                        = lanPortCntrs[port].outOampduPkts                        = 0; //TBD waiting for Saturn Chip
        pPortCntrs->ifInErrOctets                        = lanPortCntrs[port].ifInErrOctets                        = ni_mib.rx_all_byte_cnt-ni_mib.rx_byte_cnt;
        pPortCntrs->ifOutErrOctets                       = lanPortCntrs[port].ifOutErrOctets                       = ni_mib.tx_all_byte_cnt-ni_mib.tx_byte_cnt;
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9607f_stat_vlanCnt_filter_set
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
dal_rtl9607f_stat_vlanCnt_filter_set(uint32 index, rt_stat_vlanCnt_filter_t filter)
{
    aal_vlan_cnt_entry_t entry;
    aal_vlan_cnt_entry_mask_t entryMask;
    uint32 tcont=0, flow=0, queue=0;
    ca_status_t ret = CA_E_OK;

    RT_PARAM_CHK((index >= AAL_VLAN_CNT_MAX_ENTRY_ID), RT_ERR_INPUT);

    memset(&entry, 0, sizeof(aal_vlan_cnt_entry_t));
    memset(&entryMask, 0, sizeof(aal_vlan_cnt_entry_mask_t));

    entryMask.s.valid = 1;
    entry.valid = filter.enable;
    entryMask.s.mask = 1;

    if(filter.direction == RT_STAT_VLAN_CNT_FILTER_DIRECTION_RX)
    {
        if(filter.type == RT_STAT_VLAN_CNT_FILTER_TYPE_FLOW)
        {
            entryMask.s.lspid = 1;
            entry.lspid = HAL_GET_PON_PORT();
            entry.mask |= AAL_VLAN_CNT_MASK_LSPID;
            entryMask.s.gem_id = 1;
            entry.gem_id = filter.portFlowId;
            entry.mask |= AAL_VLAN_CNT_MASK_GEMID;
        }
        else
        {
            entryMask.s.lspid = 1;
            entry.lspid = filter.portFlowId;
            entry.mask |= AAL_VLAN_CNT_MASK_LSPID;
        }
    }
    else
    {
        if(filter.type == RT_STAT_VLAN_CNT_FILTER_TYPE_FLOW)
        {
            entryMask.s.egr_wan_dst = 1;
            entry.egr_wan_dst = 1;
            entry.mask |= AAL_VLAN_CNT_MASK_EGR_WAN_DST;
            entryMask.s.gem_id = 1;
#ifdef CONFIG_GPON_FEATURE
            if(RT_ERR_OK != dal_rt_rtl9607f_gpon_usFlow_phyData_get(filter.portFlowId, &tcont, &queue, &flow))
                return RT_ERR_FAILED;
#endif
            if(filter.portFlowId < RTL9607F_MAX_GEM_FLOW)
                usSidMap[filter.portFlowId] = flow;
            entry.gem_id = flow;
            entry.mask |= AAL_VLAN_CNT_MASK_GEMID;
        }
        else
        {
            entryMask.s.ldpid = 1;
            entry.ldpid = filter.portFlowId;
            entry.mask |= AAL_VLAN_CNT_MASK_LDPID;
        }
    }

    if(filter.vid != RT_STAT_VLAN_CNT_FILTER_VID_INVAILD)
    {
        entryMask.s.vid = 1;
        entry.vid = filter.vid;
        entry.mask |= AAL_VLAN_CNT_MASK_VID;
    }

    if(filter.pri != RT_STAT_VLAN_CNT_FILTER_PRI_INVAILD)
    {
        entryMask.s.pri = 1;
        entry.pri = filter.pri;
        entry.mask |= AAL_VLAN_CNT_MASK_PRI;
    }

    if(filter.pktType != RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_ALL)
    {
        entryMask.s.pkt_type = 1;
        if(filter.pktType == RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_UC)
        {
            entry.pkt_type = AAL_VLAN_CNT_PKT_TYPE_UC;
        }
        else if(filter.pktType == RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_MC)
        {
            entry.pkt_type = AAL_VLAN_CNT_PKT_TYPE_MC;
        }
        else
        {
            entry.pkt_type = AAL_VLAN_CNT_PKT_TYPE_BC;
        }

        entry.mask |= AAL_VLAN_CNT_MASK_PKT_TYPE;
    }

    ret = aal_vlan_cnt_entry_set(0, index, entryMask, &entry);
    if(ret != CA_E_OK)
        return RT_ERR_FAILED;
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9607f_stat_vlanCnt_filter_get
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
dal_rtl9607f_stat_vlanCnt_filter_get(uint32 index, rt_stat_vlanCnt_filter_t *pFilter)
{
    aal_vlan_cnt_entry_t entry;
    ca_status_t ret = CA_E_OK;
    int i;

    RT_PARAM_CHK((index >= AAL_VLAN_CNT_MAX_ENTRY_ID), RT_ERR_INPUT);
    RT_PARAM_CHK((pFilter == NULL), RT_ERR_NULL_POINTER);

    memset(&entry, 0, sizeof(aal_vlan_cnt_entry_t));

    ret = aal_vlan_cnt_entry_get(0, index, &entry);
    if(ret != CA_E_OK)
        return RT_ERR_FAILED;

    memset(pFilter, 0, sizeof(rt_stat_vlanCnt_filter_t));

    pFilter->enable = entry.valid;

    if(entry.mask & AAL_VLAN_CNT_MASK_GEMID)
    {
        pFilter->type = RT_STAT_VLAN_CNT_FILTER_TYPE_FLOW;
        pFilter->portFlowId = entry.gem_id;
    }
    else
    {
        pFilter->type = RT_STAT_VLAN_CNT_FILTER_TYPE_PORT;
        if(entry.mask & AAL_VLAN_CNT_MASK_LSPID)
        {
            pFilter->portFlowId = entry.lspid;
        }
        else
        {
            pFilter->portFlowId = entry.ldpid;
        }
    }

    if(entry.mask & AAL_VLAN_CNT_MASK_LSPID)
    {
        pFilter->direction = RT_STAT_VLAN_CNT_FILTER_DIRECTION_RX;
    }
    else
    {
        pFilter->direction = RT_STAT_VLAN_CNT_FILTER_DIRECTION_TX;
        if(pFilter->type == RT_STAT_VLAN_CNT_FILTER_TYPE_FLOW)
        {
            for(i=0; i<RTL9607F_MAX_GEM_FLOW; i++)
            {
                if(usSidMap[i] == entry.gem_id)
                {
                    pFilter->portFlowId = i;
                    break;
                }
            }
        }
    }

    if(entry.mask & AAL_VLAN_CNT_MASK_VID)
        pFilter->vid = entry.vid;
    else
        pFilter->vid = RT_STAT_VLAN_CNT_FILTER_VID_INVAILD;

    if(entry.mask & AAL_VLAN_CNT_MASK_PRI)
        pFilter->pri = entry.pri;
    else
        pFilter->pri = RT_STAT_VLAN_CNT_FILTER_PRI_INVAILD;

    if(entry.mask & AAL_VLAN_CNT_MASK_PKT_TYPE)
    {
        if(entry.pkt_type == AAL_VLAN_CNT_PKT_TYPE_UC)
            pFilter->pktType = RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_UC;
        else if(entry.pkt_type == AAL_VLAN_CNT_PKT_TYPE_MC)
            pFilter->pktType = RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_MC;
        else
            pFilter->pktType = RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_BC;
    }
    else
    {
        pFilter->pktType = RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_ALL;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9607f_stat_vlanCnt_mib_get
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
dal_rtl9607f_stat_vlanCnt_mib_get(uint32 index, rt_stat_port_cntr_t *pPortCntrs)
{
    ca_status_t ret = CA_E_OK;
    aal_vlan_cnt_entry_t entry;
    aal_vlan_cnt_mib_t mib;

    RT_PARAM_CHK((index >= AAL_VLAN_CNT_MAX_ENTRY_ID), RT_ERR_INPUT);
    RT_PARAM_CHK((pPortCntrs == NULL), RT_ERR_NULL_POINTER);

    ret = aal_vlan_cnt_mib_get(0, index, &mib);
    if(ret != CA_E_OK)
        return RT_ERR_STAT_PORT_CNTR_FAIL;

    ret = aal_vlan_cnt_entry_get(0, index, &entry);
    if(ret != CA_E_OK)
        return RT_ERR_FAILED;

    if(entry.mask & AAL_VLAN_CNT_MASK_LSPID)
    {
        pPortCntrs->ifInOctets = mib.octets;
        pPortCntrs->ifInUcastPkts = mib.uc_pkts;
        pPortCntrs->ifInMulticastPkts = mib.mc_pkts;
        pPortCntrs->ifInBroadcastPkts = mib.bc_pkts;
        pPortCntrs->etherStatsRxPkts64Octets = mib.pkts64;
        pPortCntrs->etherStatsRxPkts65to127Octets = mib.pkts65_127;
        pPortCntrs->etherStatsRxPkts128to255Octets = mib.pkts128_255;
        pPortCntrs->etherStatsRxPkts256to511Octets = mib.pkts256_511;
        pPortCntrs->etherStatsRxPkts512to1023Octets = mib.pkts512_1023;
        pPortCntrs->etherStatsRxPkts1024to1518Octets = mib.pkts1024_1518;
        pPortCntrs->etherStatsRxOversizePkts = mib.oversize_pkts;
    }
    else
    {
        pPortCntrs->ifOutOctets = mib.octets;
        pPortCntrs->ifOutUcastPkts = mib.uc_pkts;
        pPortCntrs->ifOutMulticastPkts = mib.mc_pkts;
        pPortCntrs->ifOutBrocastPkts = mib.bc_pkts;
        pPortCntrs->etherStatsTxPkts64Octets = mib.pkts64;
        pPortCntrs->etherStatsTxPkts65to127Octets = mib.pkts65_127;
        pPortCntrs->etherStatsTxPkts128to255Octets = mib.pkts128_255;
        pPortCntrs->etherStatsTxPkts256to511Octets = mib.pkts256_511;
        pPortCntrs->etherStatsTxPkts512to1023Octets = mib.pkts512_1023;
        pPortCntrs->etherStatsTxPkts1024to1518Octets = mib.pkts1024_1518;
        pPortCntrs->etherStatsTxOversizePkts = mib.oversize_pkts;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9607f_stat_vlanCnt_mib_reset
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
dal_rtl9607f_stat_vlanCnt_mib_reset(uint32 index)
{
    ca_status_t ret = CA_E_OK;
    aal_vlan_cnt_mib_mask_t mibMask;
    aal_vlan_cnt_mib_t mib;

    RT_PARAM_CHK((index >= AAL_VLAN_CNT_MAX_ENTRY_ID), RT_ERR_INPUT);

    memset(&mibMask,0xff,sizeof(aal_vlan_cnt_mib_mask_t));
    memset(&mib,0,sizeof(aal_vlan_cnt_mib_t));
    ret = aal_vlan_cnt_mib_set(0, index, mibMask, &mib);
    if(ret != CA_E_OK)
        return RT_ERR_STAT_PORT_CNTR_FAIL;

    return RT_ERR_OK;
}



