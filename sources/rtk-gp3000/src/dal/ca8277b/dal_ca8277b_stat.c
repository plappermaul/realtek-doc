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
#include <dal/ca8277b/dal_ca8277b.h>
#include <dal/ca8277b/dal_ca8277b_stat.h>
#include <dal/ca8277b/dal_rt_ca8277b_gpon.h>
#include <dal/ca8277b/dal_rt_ca8277b_epon.h>
#include <osal/time.h>

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

/*CA 8277B GEM flow 0~7 for OMCC use */
#define CA8277B_GEM_IDX_TO_LOGIC_FLOW(_gemFlow) ((_gemFlow)-8)

//reference to storm control
#define PKT_TYPE_POL_ID_UMC (10)
#define PKT_TYPE_POL_ID_BC (9)

/*
 * Data Declaration
 */
typedef struct {
    ca_uint32_t frame_cnt;
    ca_uint64_t byte_cnt;
} dal_mib_t;

typedef struct {
    unsigned char   isValid;
    unsigned short  gemIdx;
    unsigned short  gemPortId;
} dal_ca8277b_dsFlowMap_t;

static uint32    stat_init = INIT_NOT_COMPLETED;
static dal_port_cntr_t lanPortCntrs[17];

rt_gpon_flow_counter_t g8277bUsGemCnts[AAL_XGPON_GEM_PORT_NUM];
rt_gpon_flow_counter_t g8277bDsGemCnts[AAL_XGPON_GEM_PORT_NUM];
static dal_mib_t g8277bPonUsMcCnt;
static dal_mib_t g8277bPonUsBcCnt;
static dal_mib_t g8277bPonDsMcCnt;
static dal_mib_t g8277bPonDsBcCnt;
dal_mib_t g8277bPonUsCnt;
dal_mib_t g8277bPonDsCnt;
extern dal_ca8277b_dsFlowMap_t *g8277bGemDsFlowMap;

/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      dal_ca8277b_stat_init
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
dal_ca8277b_stat_init(void)
{
    int32 ret;
    rtk_port_t port;


    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_ca8277b_stat_port_reset(port)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
        }
    }

    stat_init = INIT_COMPLETED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_ca8277b_stat_port_reset
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
dal_ca8277b_stat_port_reset(rtk_port_t port)
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

        if(CA_PON_MODE_IS_EPON_FAMILY(pon_mode))
        {
            if(_dal_ca8277b_epon_is_inited() == INIT_NOT_COMPLETED)
                return RT_ERR_FAILED;

            ca_epon_port_stats_t data;

            if((ret = ca_epon_port_stats_get(0, port_id, read_clear, &data)) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
        }
        else if (CA_PON_MODE_IS_GPON_FAMILY(pon_mode))
        {
            aal_xgpon_usbg_xgem_mib_config_t us_gem_mib;
            aal_xgpon_dsfp_xgem_mib_config_t ds_gem_mib;
            aal_l2_te_pm_policer_t l2_te_pm;
            ca_uint32_t  gem = 0;
            if(_dal_ca8277b_gpon_is_inited() == INIT_NOT_COMPLETED)
                return RT_ERR_FAILED;

            //clear downstream MC
            ret = aal_l2_te_pm_policer_pkt_type_get(0, PKT_TYPE_POL_ID_UMC, &l2_te_pm);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            memset(&g8277bPonDsMcCnt, 0x0, sizeof(dal_mib_t));

            //clear downstream BC
            ret = aal_l2_te_pm_policer_pkt_type_get(0, PKT_TYPE_POL_ID_BC, &l2_te_pm);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            memset(&g8277bPonDsBcCnt, 0x0, sizeof(dal_mib_t));

            //clear upstream MC/BC
            rtk_nic_cpu_us_mib_clear();
            memset(&g8277bPonUsMcCnt, 0x0, sizeof(dal_mib_t));
            memset(&g8277bPonUsBcCnt, 0x0, sizeof(dal_mib_t));

            //clear upstream&downstream total
            if(pon_mode == ONU_PON_MAC_GPON)
            {
                aal_gpon_ds_gem_port_mib_t ds_gem;
                aal_gpon_us_gem_port_mib_t us_gem;
                AAL_FOR_ALL_USR_GEM_DO(gem)
                {
                    if((ret = aal_gpon_us_gem_port_mib_get(0, AAL_GPON_PLOAM_MIB_READ_CLR, gem, &us_gem)) != CA_E_OK)
                    {
                        RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                    }
                    if(us_gem.fcnt)
                    {
                        g8277bUsGemCnts[gem].usgem.gem_block += us_gem.fcnt;
                        g8277bUsGemCnts[gem].usgem.gem_byte += us_gem.bcnt;
                    }

                    if((ret = aal_gpon_ds_gem_port_mib_get(0, gem, 0, &ds_gem)) != CA_E_OK)
                    {
                        RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                    }
                    if(ds_gem.fcnt)
                    {
                        g8277bDsGemCnts[gem].dsgem.gem_block += ds_gem.fcnt;
                        g8277bDsGemCnts[gem].dsgem.gem_byte += ds_gem.bcnt;
                    }
                }
            }
            else
            {
                AAL_FOR_ALL_USR_XGEM_DO(gem)
                {
                    if((ret = aal_xgpon_usbg_xgem_mib_get(0, gem, &us_gem_mib))  != CA_E_OK)
                    {
                        RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                    }
                    if(us_gem_mib.frame_cnt)
                    {
                        g8277bUsGemCnts[gem].usgem.gem_block += us_gem_mib.frame_cnt;
                        g8277bUsGemCnts[gem].usgem.gem_byte += us_gem_mib.byte_cnt;
                    }
    
                    if((ret = aal_xgpon_dsfp_xgem_mib_get(0, gem, &ds_gem_mib))  != CA_E_OK)
                    {
                        RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                    }
                    if(ds_gem_mib.frame_cnt)
                    {
                        g8277bDsGemCnts[gem].dsgem.gem_block += ds_gem_mib.frame_cnt;
                        g8277bDsGemCnts[gem].dsgem.gem_byte += ds_gem_mib.byte_cnt;
                    }
                }
            }

            memset(&g8277bPonUsCnt, 0x0, sizeof(dal_mib_t));
            memset(&g8277bPonDsCnt, 0x0, sizeof(dal_mib_t));

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
        read_clear = 1;

        memset(&data, 0, sizeof(ca_eth_port_stats_t));

        if((ret = ca_eth_port_stats_get(0,port_id,read_clear,&data)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_ca8277b_stat_port_get
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
dal_ca8277b_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntrIdx, uint64 *pCntr)
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
    RT_PARAM_CHK((16 < port), RT_ERR_NULL_POINTER);


    if(HAL_IS_PON_PORT(port))
    {
        if (CA_PON_MODE_IS_GPON_FAMILY(pon_mode))
        {
            switch(cntrIdx)
            {
                case IF_IN_OCTETS_INDEX:
                case IF_IN_UCAST_PKTS_INDEX:
                case IF_IN_MULTICAST_PKTS_INDEX:
                case IF_IN_BROADCAST_PKTS_INDEX:
                case IF_OUT_OCTETS_INDEX:
                case IF_OUT_UCAST_PKTS_CNT_INDEX:
                case IF_OUT_MULTICAST_PKTS_CNT_INDEX:
                case IF_OUT_BROADCAST_PKTS_CNT_INDEX:
                case ETHER_STATS_TX_BROADCAST_PKTS_INDEX:
                case ETHER_STATS_TX_MULTICAST_PKTS_INDEX:
                    break;
                default:
                    *pCntr = 0;
                    return RT_ERR_OK;
            }
        }
    }

    if ((ret = dal_ca8277b_stat_port_getAll(port,&portCntrs)) != RT_ERR_OK)
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
        default:
            *pCntr = 0;
            break;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_ca8277b_stat_port_getAll
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
dal_ca8277b_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs)
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

    memset(pPortCntrs, 0, sizeof(rtk_stat_port_cntr_t));

    if(HAL_IS_PON_PORT(port))
    {
        read_clear = 0;

        if (CA_PON_MODE_IS_GPON_FAMILY(pon_mode))
        {
            aal_xgpon_usbg_xgem_mib_config_t us_gem_mib;
            aal_xgpon_dsfp_xgem_mib_config_t ds_gem_mib;
            aal_xgpon_dsfp_xgem_cfg_t xgem_cfg;
            aal_xgpon_usbg_gem_config_t usbg_xgem_cfg;
            aal_l2_te_pm_policer_t l2_te_pm;
            ca_uint32_t  gem = 0;
            rtk_sw_mib_t cpu2pon_mib;

            //downstream MC
            memset(&l2_te_pm, 0x0, sizeof(aal_l2_te_pm_policer_t));
            ret = aal_l2_te_pm_policer_pkt_type_get(0, PKT_TYPE_POL_ID_UMC, &l2_te_pm);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            g8277bPonDsMcCnt.frame_cnt += l2_te_pm.total_pkt;
            g8277bPonDsMcCnt.byte_cnt += l2_te_pm.total_bytes;

            //downstream BC
            memset(&l2_te_pm, 0x0, sizeof(aal_l2_te_pm_policer_t));
            ret = aal_l2_te_pm_policer_pkt_type_get(0, PKT_TYPE_POL_ID_BC, &l2_te_pm);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            g8277bPonDsBcCnt.frame_cnt += l2_te_pm.total_pkt;
            g8277bPonDsBcCnt.byte_cnt += l2_te_pm.total_bytes;

            //upstream BC/MC
            if((ret = rtk_nic_cpu_us_mib_get(&cpu2pon_mib)) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            g8277bPonUsMcCnt.frame_cnt = cpu2pon_mib.mc_pkt;
            g8277bPonUsBcCnt.frame_cnt = cpu2pon_mib.bc_pkt;

            //upstream&downstream total
            if(pon_mode == ONU_PON_MAC_GPON)
            {
                aal_gpon_ds_gem_port_mib_t ds_gem;
                aal_gpon_us_gem_port_mib_t us_gem;

                AAL_FOR_ALL_USR_GEM_DO(gem)
                {
                    if((ret = aal_gpon_us_gem_port_mib_get(0, AAL_GPON_PLOAM_MIB_READ_CLR, gem, &us_gem)) != CA_E_OK)
                    {
                        RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                    }
                    if(us_gem.fcnt)
                    {
                        g8277bUsGemCnts[gem].usgem.gem_block += us_gem.fcnt;
                        g8277bUsGemCnts[gem].usgem.gem_byte += us_gem.bcnt;
                
                        g8277bPonUsCnt.byte_cnt += us_gem.bcnt;
                        g8277bPonUsCnt.frame_cnt += us_gem.fcnt;
                    }

                    if((ret = aal_gpon_ds_gem_port_mib_get(0, gem, 0, &ds_gem)) != CA_E_OK)
                    {
                        RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                    }
                    if(ds_gem.fcnt)
                    {
                        if (g8277bGemDsFlowMap[CA8277B_GEM_IDX_TO_LOGIC_FLOW(gem)].isValid)
                        {
                            g8277bDsGemCnts[gem].dsgem.gem_block += ds_gem.fcnt;
                            g8277bDsGemCnts[gem].dsgem.gem_byte += ds_gem.bcnt;

                            g8277bPonDsCnt.byte_cnt += ds_gem.bcnt;
                            g8277bPonDsCnt.frame_cnt += ds_gem.pcnt;
                        }
                    }
                }
            }
            else
            {
                AAL_FOR_ALL_USR_XGEM_DO(gem)
                {
                    if((ret = aal_xgpon_usbg_xgem_cfg_get(0, gem, &usbg_xgem_cfg))  != CA_E_OK)
                    {
                        RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                        return RT_ERR_FAILED;
                    }

                    if(usbg_xgem_cfg.port_en)
                    {
                        if((ret = aal_xgpon_usbg_xgem_mib_get(0, gem, &us_gem_mib))  != CA_E_OK)
                        {
                            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                            return RT_ERR_FAILED;
                        }
                        if(us_gem_mib.frame_cnt)
                        {
                            g8277bUsGemCnts[gem].usgem.gem_block += us_gem_mib.frame_cnt;
                            g8277bUsGemCnts[gem].usgem.gem_byte += us_gem_mib.byte_cnt;
        
                            g8277bPonUsCnt.byte_cnt += us_gem_mib.byte_cnt;
                            g8277bPonUsCnt.frame_cnt += us_gem_mib.frame_cnt;
                        }
                    }
    
                    if((ret = aal_xgpon_dsfp_xgem_get(0, gem ,&xgem_cfg))  != CA_E_OK)
                    {
                        RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                        return RT_ERR_FAILED;
                    }

                    if(xgem_cfg.gem_vld)
                    {
                        memset(&ds_gem_mib, 0, sizeof(ds_gem_mib));
                        if((ret = aal_xgpon_dsfp_xgem_mib_get(0, gem, &ds_gem_mib))  != CA_E_OK)
                        {
                            RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                            return RT_ERR_FAILED;
                        }
                        if(ds_gem_mib.frame_cnt)
                        {
                            g8277bDsGemCnts[gem].dsgem.gem_block += ds_gem_mib.frame_cnt;
                            g8277bDsGemCnts[gem].dsgem.gem_byte += ds_gem_mib.byte_cnt;
        
                            g8277bPonDsCnt.byte_cnt += ds_gem_mib.byte_cnt;
                            g8277bPonDsCnt.frame_cnt += ds_gem_mib.frame_cnt;
                        }
                    }
                }
            }

            if(g8277bPonDsCnt.frame_cnt  < (g8277bPonDsMcCnt.frame_cnt +  g8277bPonDsBcCnt.frame_cnt))
                g8277bPonDsCnt.frame_cnt  = g8277bPonDsMcCnt.frame_cnt +  g8277bPonDsBcCnt.frame_cnt;

            if(g8277bPonUsCnt.frame_cnt  < (g8277bPonUsMcCnt.frame_cnt +  g8277bPonUsBcCnt.frame_cnt))
                g8277bPonUsCnt.frame_cnt  = g8277bPonUsMcCnt.frame_cnt +  g8277bPonUsBcCnt.frame_cnt;

            pPortCntrs->ifInOctets                           = lanPortCntrs[7].ifInOctets                           = g8277bPonDsCnt.byte_cnt;
            pPortCntrs->ifInUcastPkts                        = lanPortCntrs[7].ifInUcastPkts                        = g8277bPonDsCnt.frame_cnt -g8277bPonDsMcCnt.frame_cnt -g8277bPonDsBcCnt.frame_cnt;
            pPortCntrs->ifInMulticastPkts                    = lanPortCntrs[7].ifInMulticastPkts                    = g8277bPonDsMcCnt.frame_cnt;
            pPortCntrs->ifInBroadcastPkts                    = lanPortCntrs[7].ifInBroadcastPkts                    = g8277bPonDsBcCnt.frame_cnt;
            pPortCntrs->ifInDiscards                         = lanPortCntrs[7].ifInDiscards                         = 0;
            pPortCntrs->ifOutOctets                          = lanPortCntrs[7].ifOutOctets                          = g8277bPonUsCnt.byte_cnt;
            pPortCntrs->ifOutDiscards                        = lanPortCntrs[7].ifOutDiscards                        = 0;
            pPortCntrs->ifOutUcastPkts                       = lanPortCntrs[7].ifOutUcastPkts                       = g8277bPonUsCnt.frame_cnt -g8277bPonUsMcCnt.frame_cnt -  g8277bPonUsBcCnt.frame_cnt;
            pPortCntrs->ifOutMulticastPkts                   = lanPortCntrs[7].ifOutMulticastPkts                   = g8277bPonUsMcCnt.frame_cnt;
            pPortCntrs->ifOutBrocastPkts                     = lanPortCntrs[7].ifOutBrocastPkts                     = g8277bPonUsBcCnt.frame_cnt;
            pPortCntrs->dot1dBasePortDelayExceededDiscards   = lanPortCntrs[7].dot1dBasePortDelayExceededDiscards   = 0;
            pPortCntrs->dot1dTpPortInDiscards                = lanPortCntrs[7].dot1dTpPortInDiscards                = 0;
            pPortCntrs->dot1dTpHcPortInDiscards              = lanPortCntrs[7].dot1dTpHcPortInDiscards              = 0;
            pPortCntrs->dot3InPauseFrames                    = lanPortCntrs[7].dot3InPauseFrames                    = 0;
            pPortCntrs->dot3OutPauseFrames                   = lanPortCntrs[7].dot3OutPauseFrames                   = 0;
            pPortCntrs->dot3OutPauseOnFrames                 = lanPortCntrs[7].dot3OutPauseOnFrames                 = 0;
            pPortCntrs->dot3StatsAligmentErrors              = lanPortCntrs[7].dot3StatsAligmentErrors              = 0;
            pPortCntrs->dot3StatsFCSErrors                   = lanPortCntrs[7].dot3StatsFCSErrors                   = 0;
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
            pPortCntrs->etherStatsTxPkts64Octets             = lanPortCntrs[7].etherStatsTxPkts64Octets             = 0;
            pPortCntrs->etherStatsTxPkts65to127Octets        = lanPortCntrs[7].etherStatsTxPkts65to127Octets        = 0;
            pPortCntrs->etherStatsTxPkts128to255Octets       = lanPortCntrs[7].etherStatsTxPkts128to255Octets       = 0;
            pPortCntrs->etherStatsTxPkts256to511Octets       = lanPortCntrs[7].etherStatsTxPkts256to511Octets       = 0;
            pPortCntrs->etherStatsTxPkts512to1023Octets      = lanPortCntrs[7].etherStatsTxPkts512to1023Octets      = 0;
            pPortCntrs->etherStatsTxPkts1024to1518Octets     = lanPortCntrs[7].etherStatsTxPkts1024to1518Octets     = 0;
            pPortCntrs->etherStatsTxPkts1519toMaxOctets      = lanPortCntrs[7].etherStatsTxPkts1519toMaxOctets      = 0;
            pPortCntrs->etherStatsTxBcastPkts                = lanPortCntrs[7].etherStatsTxBcastPkts                = g8277bPonUsBcCnt.frame_cnt;
            pPortCntrs->etherStatsTxMcastPkts                = lanPortCntrs[7].etherStatsTxMcastPkts                = g8277bPonUsMcCnt.frame_cnt;
            pPortCntrs->etherStatsTxFragments                = lanPortCntrs[7].etherStatsTxFragments                = 0;
            pPortCntrs->etherStatsTxJabbers                  = lanPortCntrs[7].etherStatsTxJabbers                  = 0;
            pPortCntrs->etherStatsTxCRCAlignErrors           = lanPortCntrs[7].etherStatsTxCRCAlignErrors           = 0;
            pPortCntrs->etherStatsRxUndersizePkts            = lanPortCntrs[7].etherStatsRxUndersizePkts            = 0;
            pPortCntrs->etherStatsRxUndersizeDropPkts        = lanPortCntrs[7].etherStatsRxUndersizeDropPkts        = 0;
            pPortCntrs->etherStatsRxOversizePkts             = lanPortCntrs[7].etherStatsRxOversizePkts             = 0;
            pPortCntrs->etherStatsRxPkts64Octets             = lanPortCntrs[7].etherStatsRxPkts64Octets             = 0;
            pPortCntrs->etherStatsRxPkts65to127Octets        = lanPortCntrs[7].etherStatsRxPkts65to127Octets        = 0;
            pPortCntrs->etherStatsRxPkts128to255Octets       = lanPortCntrs[7].etherStatsRxPkts128to255Octets       = 0;
            pPortCntrs->etherStatsRxPkts256to511Octets       = lanPortCntrs[7].etherStatsRxPkts256to511Octets       = 0;
            pPortCntrs->etherStatsRxPkts512to1023Octets      = lanPortCntrs[7].etherStatsRxPkts512to1023Octets      = 0;
            pPortCntrs->etherStatsRxPkts1024to1518Octets     = lanPortCntrs[7].etherStatsRxPkts1024to1518Octets     = 0;
            pPortCntrs->etherStatsRxPkts1519toMaxOctets      = lanPortCntrs[7].etherStatsRxPkts1519toMaxOctets      = 0;
            pPortCntrs->inOampduPkts                         = lanPortCntrs[7].inOampduPkts                         = 0;
            pPortCntrs->outOampduPkts                        = lanPortCntrs[7].outOampduPkts                        = 0;

        }
        else if(CA_PON_MODE_IS_EPON_FAMILY(pon_mode))
        {
            ca_epon_port_stats_t epon_stats;

            memset(&epon_stats, 0, sizeof(ca_epon_port_stats_t));
            if((ret = ca_epon_port_stats_get(0, port_id, read_clear, &epon_stats)) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_STAT | MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            pPortCntrs->ifInOctets                           = lanPortCntrs[7].ifInOctets                           = epon_stats.rx_good_octets;
            pPortCntrs->ifInUcastPkts                        = lanPortCntrs[7].ifInUcastPkts                        = epon_stats.rx_uc_frames;        /* received unicast frames                                                   */
            pPortCntrs->ifInMulticastPkts                    = lanPortCntrs[7].ifInMulticastPkts                    = epon_stats.rx_mc_frames;        /* received multicast frames                                                 */
            pPortCntrs->ifInBroadcastPkts                    = lanPortCntrs[7].ifInBroadcastPkts                    = epon_stats.rx_bc_frames;        /* received broadcast frames                                                 */
            pPortCntrs->ifInDiscards                         = lanPortCntrs[7].ifInDiscards                         = 0;
            pPortCntrs->ifOutOctets                          = lanPortCntrs[7].ifOutOctets                          = epon_stats.tx_good_octets;
            pPortCntrs->ifOutDiscards                        = lanPortCntrs[7].ifOutDiscards                        = 0;
            pPortCntrs->ifOutUcastPkts                       = lanPortCntrs[7].ifOutUcastPkts                       = epon_stats.tx_uc_frames;        /* transmitted unicast frames                                                */
            pPortCntrs->ifOutMulticastPkts                   = lanPortCntrs[7].ifOutMulticastPkts                   = epon_stats.tx_mc_frames;        /* transmitted multicast frames                                              */
            pPortCntrs->ifOutBrocastPkts                     = lanPortCntrs[7].ifOutBrocastPkts                     = epon_stats.tx_bc_frames;        /* transmitted broadcast frames                                              */
            pPortCntrs->dot1dBasePortDelayExceededDiscards   = lanPortCntrs[7].dot1dBasePortDelayExceededDiscards   = 0;
            pPortCntrs->dot1dTpPortInDiscards                = lanPortCntrs[7].dot1dTpPortInDiscards                = 0;
            pPortCntrs->dot1dTpHcPortInDiscards              = lanPortCntrs[7].dot1dTpHcPortInDiscards              = 0;
            pPortCntrs->dot3InPauseFrames                    = lanPortCntrs[7].dot3InPauseFrames                    = epon_stats.rx_pause_frames;
            pPortCntrs->dot3OutPauseFrames                   = lanPortCntrs[7].dot3OutPauseFrames                   = epon_stats.tx_pause_frames;
            pPortCntrs->dot3OutPauseOnFrames                 = lanPortCntrs[7].dot3OutPauseOnFrames                 = 0;
            pPortCntrs->dot3StatsAligmentErrors              = lanPortCntrs[7].dot3StatsAligmentErrors              = 0;
            pPortCntrs->dot3StatsFCSErrors                   = lanPortCntrs[7].dot3StatsFCSErrors                   = epon_stats.rx_crc8_sld_err;
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
            pPortCntrs->etherStatsTxPkts64Octets             = lanPortCntrs[7].etherStatsTxPkts64Octets             = epon_stats.tx_64_frames;        /* transmitted frames with 64 bytes                                          */
            pPortCntrs->etherStatsTxPkts65to127Octets        = lanPortCntrs[7].etherStatsTxPkts65to127Octets        = epon_stats.tx_65_127_frames;    /* transmitted frames with 65 ~ 127 bytes                                    */
            pPortCntrs->etherStatsTxPkts128to255Octets       = lanPortCntrs[7].etherStatsTxPkts128to255Octets       = epon_stats.tx_128_255_frames;   /* transmitted frames with 128 ~ 255 bytes                                   */
            pPortCntrs->etherStatsTxPkts256to511Octets       = lanPortCntrs[7].etherStatsTxPkts256to511Octets       = epon_stats.tx_256_511_frames;   /* transmitted frames with 256 ~ 511 bytes                                   */
            pPortCntrs->etherStatsTxPkts512to1023Octets      = lanPortCntrs[7].etherStatsTxPkts512to1023Octets      = epon_stats.tx_512_1023_frames;  /* transmitted frames with 512 ~ 1023 bytes                                  */
            pPortCntrs->etherStatsTxPkts1024to1518Octets     = lanPortCntrs[7].etherStatsTxPkts1024to1518Octets     = epon_stats.tx_1024_1518_frames; /* transmitted frames with 1024 ~ 1518 bytes                                 */
            pPortCntrs->etherStatsTxPkts1519toMaxOctets      = lanPortCntrs[7].etherStatsTxPkts1519toMaxOctets      = epon_stats.tx_1519_max_frames;  /* transmitted frames of which length is equal to or greater than 1519 bytes */
            pPortCntrs->etherStatsTxBcastPkts                = lanPortCntrs[7].etherStatsTxBcastPkts                = epon_stats.tx_bc_frames;        /* transmitted broadcast frames                                              */
            pPortCntrs->etherStatsTxMcastPkts                = lanPortCntrs[7].etherStatsTxMcastPkts                = epon_stats.tx_mc_frames;        /* transmitted multicast frames                                              */
            pPortCntrs->etherStatsTxFragments                = lanPortCntrs[7].etherStatsTxFragments                = 0;
            pPortCntrs->etherStatsTxJabbers                  = lanPortCntrs[7].etherStatsTxJabbers                  = 0;
            pPortCntrs->etherStatsTxCRCAlignErrors           = lanPortCntrs[7].etherStatsTxCRCAlignErrors           = 0;
            pPortCntrs->etherStatsRxUndersizePkts            = lanPortCntrs[7].etherStatsRxUndersizePkts            = epon_stats.rx_runt_frames; /* received frames undersize frame without FCS err               */
            pPortCntrs->etherStatsRxUndersizeDropPkts        = lanPortCntrs[7].etherStatsRxUndersizeDropPkts        = 0;
            pPortCntrs->etherStatsRxOversizePkts             = lanPortCntrs[7].etherStatsRxOversizePkts             = epon_stats.rx_oversize_frames;
            pPortCntrs->etherStatsRxPkts64Octets             = lanPortCntrs[7].etherStatsRxPkts64Octets             = epon_stats.rx_64_frames;        /* received frames with 64 bytes                                             */
            pPortCntrs->etherStatsRxPkts65to127Octets        = lanPortCntrs[7].etherStatsRxPkts65to127Octets        = epon_stats.rx_65_127_frames;    /* received frames with 65 ~ 127 bytes                                       */
            pPortCntrs->etherStatsRxPkts128to255Octets       = lanPortCntrs[7].etherStatsRxPkts128to255Octets       = epon_stats.rx_128_255_frames;   /* received frames with 128 ~ 255 bytes                                      */
            pPortCntrs->etherStatsRxPkts256to511Octets       = lanPortCntrs[7].etherStatsRxPkts256to511Octets       = epon_stats.rx_256_511_frames;   /* received frames with 256 ~ 511 bytes                                      */
            pPortCntrs->etherStatsRxPkts512to1023Octets      = lanPortCntrs[7].etherStatsRxPkts512to1023Octets      = epon_stats.rx_512_1023_frames;  /* received frames with 512 ~ 1023 bytes                                     */
            pPortCntrs->etherStatsRxPkts1024to1518Octets     = lanPortCntrs[7].etherStatsRxPkts1024to1518Octets     = epon_stats.rx_1024_1518_frames; /* received frames with 1024 ~ 1518 bytes                                    */
            pPortCntrs->etherStatsRxPkts1519toMaxOctets      = lanPortCntrs[7].etherStatsRxPkts1519toMaxOctets      = epon_stats.rx_1519_max_frames;  /* received frames of which length is equal to or greater than 1519 bytes    */
            pPortCntrs->inOampduPkts                         = lanPortCntrs[7].inOampduPkts                         = epon_stats.rx_oam_frames;
            pPortCntrs->outOampduPkts                        = lanPortCntrs[7].outOampduPkts                        = epon_stats.tx_oam_frames;

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

        read_clear = 0;

        memset(&data, 0, sizeof(ca_eth_port_stats_t));

        if((ret = ca_eth_port_stats_get(0,port_id,read_clear,&data)) != CA_E_OK)
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
        pPortCntrs->etherStatsRxOversizePkts             = lanPortCntrs[port].etherStatsRxOversizePkts             = data.rx_oversize_frames;  /* received frames of which length is greater than max frames size           */
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
