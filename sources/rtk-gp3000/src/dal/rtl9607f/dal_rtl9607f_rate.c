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
 * $Revision: 45446 $
 * $Date: 2013-12-19 10:51:19 +0800 (?±å?, 19 ?ä???2013) $
 *
 * Purpose : Definition of Port Bandwidth Control and Storm Control API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Configuration of Ingress Port Bandwidth Control [Ingress Rate Limit]
 *           (2) Configuration of Egress  Port Bandwidth Control [Egress  Rate Limit]
 *           (3) Configuration of Storm Control
 *           (3) Configuration of meter
 *
 */

/*
 * Include Files
 */

#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/rate.h>
#include <hal/chipdef/rtl9607f/rtl9607f_def.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_rate.h>
#include <dal/rtl9607f/dal_rtl9607f_trap.h>


#include <aal_l2_specpkt.h>
#include <aal_l2_tm.h>
#include <aal_l3_tm.h>
#include <aal_port.h>
#include <aal_l2_te.h>
#include <aal_arb.h>
#include <aal_l3_te.h>
#include <aal_common.h>
#include <aal_pon.h>


/*
 * Symbol Definition
 */

typedef struct {
    rtk_enable_t uuc_enable;
    rtk_enable_t umc_enable;
    rtk_enable_t bc_enable;
    rtk_enable_t mc_enable;
    rtk_enable_t dhcp_enable;
    rtk_enable_t arp_enable;
    rtk_enable_t igmp_mld_enable;
    rtk_enable_t ethertype_enable;
    rtk_enable_t loopdetect_enable;
    rtk_enable_t dot1x_enable;
    ca_uint16_t uuc_flow_id;
    ca_uint16_t umc_flow_id;
    ca_uint16_t bc_flow_id;
    ca_uint16_t mc_flow_id;
    ca_uint16_t dhcp_flow_id;
    ca_uint16_t arp_flow_id;
    ca_uint16_t igmp_mld_flow_id;
    ca_uint16_t ethertype_flow_id;
    ca_uint16_t loopdetect_flow_id;
    ca_uint16_t dot1x_flow_id;
    ca_uint32_t bc_storm_index;
    ca_uint32_t mc_storm_index;
    ca_uint32_t dhcp_storm_index0;
    ca_uint32_t dhcp_storm_index1;
    ca_uint32_t arp_storm_index;
    ca_uint32_t igmp_mld_storm_index0;
    ca_uint32_t igmp_mld_storm_index1;
    ca_uint32_t igmp_mld_storm_index2;
    ca_uint32_t igmp_mld_storm_index3;
    ca_uint32_t igmp_mld_storm_index4;
    ca_uint32_t ethertype_storm_index;
    ca_uint32_t loopdetect_storm_index;
    ca_uint32_t dot1x_storm_index;
} rate_storm_cfg_t;

rate_storm_cfg_t stormCfg[RTK_MAX_NUM_OF_PORTS];

typedef struct portQueueMeterCfg_s
{
    uint8 enable;
    ca_uint32_t meter_index;
} portQueueMeterCfg_t;


typedef struct meter_cfg_s
{
    aal_l2_te_policer_type_t    mode; /* policer mode: none or RFCxxx */
    ca_boolean_t                            pps;  /* True - PPS mode, false - PBS mode */
    ca_uint32_t                             cir;  /* commited rate           */
    ca_uint32_t                             cbs;  /* commited burst size     */
    ca_uint32_t                             pir;  /* eir for rfc4115         */
    ca_uint32_t                             pbs;  /* ebs for rfc4115&rfc2697 */
    ca_uint32_t                             ipg;  /* 0 means disable. >0 means enable */
} meter_cfg_t;

#define USD_METER_MAX_NUM   (CA_AAL_MAX_AGR_FLOW_ID + CA_AAL_POLICER_PKT_TYPE_END - 1)

/*
 * Data Declaration
 */
static uint32 rate_init = {INIT_NOT_COMPLETED};

static meter_cfg_t meterEntry[USD_METER_MAX_NUM];


/*
 * Function Declaration
 */

int32 _dal_rtl9607f_rate_glb_db_dump(void)
{
    rtk_port_t port;
    uint32 queue, idx;

    HAL_SCAN_ALL_ETH_PORT(port)
    {
        if (!HAL_IS_PORT_EXIST(port))
            continue;

        if (HAL_IS_XE_PORT(port) && port != HAL_GET_PON_PORT())
            continue;

        osal_printf("stormCfg[%u].uuc_enable=%x\n", port, stormCfg[port].uuc_enable);
        osal_printf("stormCfg[%u].umc_enable=%x\n", port, stormCfg[port].umc_enable);
        osal_printf("stormCfg[%u].bc_enable=%x\n", port, stormCfg[port].bc_enable);
        osal_printf("stormCfg[%u].mc_enable=%x\n", port, stormCfg[port].mc_enable);
        osal_printf("stormCfg[%u].dhcp_enable=%x\n", port, stormCfg[port].dhcp_enable);
        osal_printf("stormCfg[%u].arp_enable=%x\n", port, stormCfg[port].arp_enable);
        osal_printf("stormCfg[%u].igmp_mld_enable=%x\n", port, stormCfg[port].igmp_mld_enable);
        osal_printf("stormCfg[%u].ethertype_enable=%x\n", port, stormCfg[port].ethertype_enable);
        osal_printf("stormCfg[%u].loopdetect_enable=%x\n", port, stormCfg[port].loopdetect_enable);
        osal_printf("stormCfg[%u].dot1x_enable=%x\n", port, stormCfg[port].dot1x_enable);
        osal_printf("stormCfg[%u].uuc_flow_id=%x\n", port, stormCfg[port].uuc_flow_id);
        osal_printf("stormCfg[%u].umc_flow_id=%x\n", port, stormCfg[port].umc_flow_id );
        osal_printf("stormCfg[%u].bc_flow_id=%x\n", port, stormCfg[port].bc_flow_id);
        osal_printf("stormCfg[%u].mc_flow_id=%x\n", port, stormCfg[port].mc_flow_id);
        osal_printf("stormCfg[%u].dhcp_flow_id=%x\n", port, stormCfg[port].dhcp_flow_id);
        osal_printf("stormCfg[%u].arp_flow_id=%x\n", port, stormCfg[port].arp_flow_id);
        osal_printf("stormCfg[%u].igmp_mld_flow_id=%x\n", port, stormCfg[port].igmp_mld_flow_id);
        osal_printf("stormCfg[%u].ethertype_flow_id=%x\n", port, stormCfg[port].ethertype_flow_id);
        osal_printf("stormCfg[%u].loopdetect_flow_id=%x\n", port, stormCfg[port].loopdetect_flow_id);
        osal_printf("stormCfg[%u].dot1x_flow_id=%x\n", port, stormCfg[port].dot1x_flow_id);
    }

    for(idx = 0 ; idx < USD_METER_MAX_NUM ;  idx++)
    {
        osal_printf("meterEntry[%u].mode=%u\n", idx, meterEntry[idx].mode);
        osal_printf("meterEntry[%u].pps=%u\n", idx, meterEntry[idx].pps);
        osal_printf("meterEntry[%u].cir=%u\n", idx, meterEntry[idx].cir);
        osal_printf("meterEntry[%u].cbs=%u\n", idx, meterEntry[idx].cbs);
        osal_printf("meterEntry[%u].pir=%u\n", idx, meterEntry[idx].pir);
        osal_printf("meterEntry[%u].pbs=%u\n", idx, meterEntry[idx].pbs);
        osal_printf("meterEntry[%u].ipg=%u\n", idx, meterEntry[idx].ipg);
    }
    return RT_ERR_OK;
}

static int32 _dal_rtl9607f_agr_flow_profile_id_get(rtk_rate_storm_group_t stormType, ca_uint32_t *pProfile_id)
{
// assign DHCP to use agr flow profile id 7
// assign ARP to use agr flow profile id 6
// assign igmp/mld to use agr flow profile id 5
// assign ethertype to use agr flow profile id 4
// assign loopdetect to use agr flow profile id 3
// assign dot1x to use agr flow profile id 2

    switch(stormType)
    {
        case STORM_GROUP_DHCP:
            *pProfile_id = CA_AAL_MAX_AGR_FLOW_ID;
            break;
        case STORM_GROUP_ARP:
            *pProfile_id = CA_AAL_MAX_AGR_FLOW_ID - 1;
            break;
        case STORM_GROUP_IGMP_MLD:
            *pProfile_id = CA_AAL_MAX_AGR_FLOW_ID - 2;
            break;
        case STORM_GROUP_ETHERTYPE:
            *pProfile_id = CA_AAL_MAX_AGR_FLOW_ID - 3;
            break;
        case STORM_GROUP_LOOP_DETECT:
            *pProfile_id = CA_AAL_MAX_AGR_FLOW_ID - 4;
            break;
        case STORM_GROUP_DOT1X:
            *pProfile_id = CA_AAL_MAX_AGR_FLOW_ID - 5;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
}

static int32 _dal_rtl9607f_meter_cfg_get(uint32 index, meter_cfg_t *pMeter)
{
    rt_error_common_t ret = RT_ERR_OK;
    uint32  port;
    aal_l2_te_shaper_tbc_cfg_t     profile;
    aal_l3_te_shaper_tbc_cfg_t     l3_profile;
    ca_uint32_t port_type = CA_PORT_TYPE_INVALID;
    ca_port_id_t map_port = 0;
    ca_port_id_t port_id;
    ca_uint32_t cbs = 0, ipg=20, mode = 0, pbs = 0, pps = 0;
    ca_uint16_t           flood_pol_id, ipg_value;
    aal_l2_te_policer_cfg_t pol_cfg;
    aal_l2_te_policer_profile_t profile_cfg;
    ca_uint8_t  ipg_prof_id;

    HAL_SCAN_ALL_PORT(port)
    {
        port_id = RTK2CA_PORT_ID(port);
        port_type = PORT_TYPE(port_id);
        map_port      = PORT_ID(port_id);

        if (port_type == CA_PORT_TYPE_ETHERNET)
        {
            if (map_port > CA_AAL_MAX_PORT_ID)
                continue;
        }
        else if (port_type == CA_PORT_TYPE_CPU)
        {
            if (map_port < CA_PORT_ID_CPU0 || map_port > CA_PORT_ID_CPU7)
                continue;
        }
        else if (port_type == CA_PORT_TYPE_L3)
        {
            if (map_port != CA_PORT_ID_NI7)
                continue;
        }
        else
        {
            continue;
        }

        if(index == stormCfg[port].uuc_flow_id)
        {
            RT_ERR_CHK((rt_error_common_t)aal_arb_non_known_pol_map_get(0, port, AAL_ARB_PKT_FWD_TYPE_UUC, &flood_pol_id), ret);
            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_pkt_type_cfg_get(0, flood_pol_id, &pol_cfg), ret);

            /* set to l2te HW */
            memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));
            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_pkt_type_profile_get(0, flood_pol_id, &profile_cfg), ret);

            ipg =pol_cfg.ipg;
            cbs = profile_cfg.cbs;
            pbs = profile_cfg.pbs;
            pps = (pol_cfg.pol_update_mode == CA_AAL_POLICER_UPDATE_MODE_PKT) ? TRUE : FALSE;
            mode = pol_cfg.type;
        }

        if(index == stormCfg[port].bc_flow_id)
        {
            RT_ERR_CHK((rt_error_common_t)aal_arb_non_known_pol_map_get(0, port, AAL_ARB_PKT_FWD_TYPE_BC, &flood_pol_id), ret);
            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_pkt_type_cfg_get(0, flood_pol_id, &pol_cfg), ret);

            /* set to l2te HW */
            memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));
            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_pkt_type_profile_get(0, flood_pol_id, &profile_cfg), ret);

            ipg =pol_cfg.ipg;
            cbs = profile_cfg.cbs;
            pbs = profile_cfg.pbs;
            pps = (pol_cfg.pol_update_mode == CA_AAL_POLICER_UPDATE_MODE_PKT) ? TRUE : FALSE;
            mode = pol_cfg.type;
        }

        if (index == stormCfg[port].arp_flow_id)
        {
            ca_uint32_t agr_pol_id = -1;

            RT_ERR_CHK((rt_error_common_t)_dal_rtl9607f_agr_flow_profile_id_get(STORM_GROUP_ARP, &agr_pol_id), ret);

            memset((void *)&pol_cfg, 0, sizeof(aal_l2_te_policer_cfg_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_cfg_get(0, agr_pol_id, &pol_cfg), ret);

            memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_profile_get(0, agr_pol_id, &profile_cfg), ret);

            ipg =pol_cfg.ipg;
            cbs = profile_cfg.cbs;
            pbs = profile_cfg.pbs;
            pps = (pol_cfg.pol_update_mode == CA_AAL_POLICER_UPDATE_MODE_PKT) ? TRUE : FALSE;
            mode = pol_cfg.type;
        }

        if (index == stormCfg[port].dhcp_flow_id)
        {
            ca_uint32_t agr_pol_id = -1;

            RT_ERR_CHK((rt_error_common_t)_dal_rtl9607f_agr_flow_profile_id_get(STORM_GROUP_DHCP, &agr_pol_id), ret);

            memset((void *)&pol_cfg, 0, sizeof(aal_l2_te_policer_cfg_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_cfg_get(0, agr_pol_id, &pol_cfg), ret);

            memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_profile_get(0, agr_pol_id, &profile_cfg), ret);

            ipg =pol_cfg.ipg;
            cbs = profile_cfg.cbs;
            pbs = profile_cfg.pbs;
            pps = (pol_cfg.pol_update_mode == CA_AAL_POLICER_UPDATE_MODE_PKT) ? TRUE : FALSE;
            mode = pol_cfg.type;
        }

        if (index == stormCfg[port].igmp_mld_flow_id)
        {
            ca_uint32_t agr_pol_id = -1;

            RT_ERR_CHK((rt_error_common_t)_dal_rtl9607f_agr_flow_profile_id_get(STORM_GROUP_IGMP_MLD, &agr_pol_id), ret);

            memset((void *)&pol_cfg, 0, sizeof(aal_l2_te_policer_cfg_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_cfg_get(0, agr_pol_id, &pol_cfg), ret);

            memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_profile_get(0, agr_pol_id, &profile_cfg), ret);

            ipg =pol_cfg.ipg;
            cbs = profile_cfg.cbs;
            pbs = profile_cfg.pbs;
            pps = (pol_cfg.pol_update_mode == CA_AAL_POLICER_UPDATE_MODE_PKT) ? TRUE : FALSE;
            mode = pol_cfg.type;
        }

        if (index == stormCfg[port].ethertype_flow_id)
        {
            ca_uint32_t agr_pol_id = -1;

            RT_ERR_CHK((rt_error_common_t)_dal_rtl9607f_agr_flow_profile_id_get(STORM_GROUP_ETHERTYPE, &agr_pol_id), ret);

            memset((void *)&pol_cfg, 0, sizeof(aal_l2_te_policer_cfg_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_cfg_get(0, agr_pol_id, &pol_cfg), ret);

            memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_profile_get(0, agr_pol_id, &profile_cfg), ret);

            ipg =pol_cfg.ipg;
            cbs = profile_cfg.cbs;
            pbs = profile_cfg.pbs;
            pps = (pol_cfg.pol_update_mode == CA_AAL_POLICER_UPDATE_MODE_PKT) ? TRUE : FALSE;
            mode = pol_cfg.type;
        }

        if (index == stormCfg[port].loopdetect_flow_id)
        {
            ca_uint32_t agr_pol_id = -1;

            RT_ERR_CHK((rt_error_common_t)_dal_rtl9607f_agr_flow_profile_id_get(STORM_GROUP_LOOP_DETECT, &agr_pol_id), ret);

            memset((void *)&pol_cfg, 0, sizeof(aal_l2_te_policer_cfg_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_cfg_get(0, agr_pol_id, &pol_cfg), ret);

            memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_profile_get(0, agr_pol_id, &profile_cfg), ret);

            ipg =pol_cfg.ipg;
            cbs = profile_cfg.cbs;
            pbs = profile_cfg.pbs;
            pps = (pol_cfg.pol_update_mode == CA_AAL_POLICER_UPDATE_MODE_PKT) ? TRUE : FALSE;
            mode = pol_cfg.type;
        }

        if (index == stormCfg[port].dot1x_flow_id)
        {
            ca_uint32_t agr_pol_id = -1;

            RT_ERR_CHK((rt_error_common_t)_dal_rtl9607f_agr_flow_profile_id_get(STORM_GROUP_DOT1X, &agr_pol_id), ret);

            memset((void *)&pol_cfg, 0, sizeof(aal_l2_te_policer_cfg_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_cfg_get(0, agr_pol_id, &pol_cfg), ret);

            memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));

            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_profile_get(0, agr_pol_id, &profile_cfg), ret);

            ipg =pol_cfg.ipg;
            cbs = profile_cfg.cbs;
            pbs = profile_cfg.pbs;
            pps = (pol_cfg.pol_update_mode == CA_AAL_POLICER_UPDATE_MODE_PKT) ? TRUE : FALSE;
            mode = pol_cfg.type;
        }

    }

    pMeter->ipg = ipg;
    pMeter->cbs = cbs;
    pMeter->pbs = pbs;
    pMeter->pps = pps;
    pMeter->mode = mode;

    return RT_ERR_OK;
}

int32 _dal_rtl9607f_use_ca_agr_flow_set(rtk_rate_storm_group_t stormType, ca_uint16_t meterEntryIdx)
{
    rt_error_common_t ret = RT_ERR_OK;
    aal_l2_te_policer_cfg_msk_t pol_msk;
    aal_l2_te_policer_cfg_t pol_cfg;
    aal_l2_te_policer_profile_msk_t profile_msk;
    aal_l2_te_policer_profile_t profile_cfg;
    ca_uint32_t agr_pol_id = -1;

    RT_ERR_CHK((rt_error_common_t)_dal_rtl9607f_agr_flow_profile_id_get(stormType, &agr_pol_id), ret);

    memset((void *)&pol_cfg, 0, sizeof(aal_l2_te_policer_cfg_t));

    RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_cfg_get(0, agr_pol_id, &pol_cfg), ret);

    pol_msk.u32 = 0;

    pol_msk.s.pol_update_mode = 1;
    pol_cfg.pol_update_mode =(meterEntry[meterEntryIdx].pps == 0 ? CA_AAL_POLICER_UPDATE_MODE_BYTE : CA_AAL_POLICER_UPDATE_MODE_PKT);

    pol_msk.s.type = 1;
    pol_cfg.type = (meterEntry[meterEntryIdx].mode == CA_AAL_POLICER_TYPE_NONE  ? CA_AAL_POLICER_TYPE_NONE : CA_AAL_POLICER_TYPE_RFC2697);

    pol_msk.s.ipg = 1;
    pol_cfg.ipg = meterEntry[meterEntryIdx].ipg;

    RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_cfg_set(0, agr_pol_id, pol_msk, &pol_cfg), ret);

    memset(&profile_msk, 0, sizeof(aal_l2_te_policer_profile_msk_t));
    memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));

    RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_profile_get(0, agr_pol_id, &profile_cfg), ret);

    profile_msk.s.pir_m = 1;
    profile_msk.s.pir_k = 1;
    profile_msk.s.pbs = 1;
    profile_cfg.pir_m = meterEntry[meterEntryIdx].cir / 1000;
    profile_cfg.pir_k = meterEntry[meterEntryIdx].cir % 1000;
    profile_cfg.pbs = (meterEntry[meterEntryIdx].pbs == 0 ?  1 : meterEntry[meterEntryIdx].pbs);

    profile_msk.s.cir_m = 1;
    profile_msk.s.cir_k = 1;
    profile_msk.s.cbs = 1;
    profile_cfg.cir_m = profile_cfg.pir_m;
    profile_cfg.cir_k = profile_cfg.pir_k;
    profile_cfg.cbs = (meterEntry[meterEntryIdx].cbs == 0 ? 1 : meterEntry[meterEntryIdx].cbs);

    RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_profile_set(0, agr_pol_id, profile_msk, &profile_cfg), ret);

    if (STORM_GROUP_ARP == stormType)
        RT_ERR_CHK((rt_error_common_t)aal_l2_specpkt_arp_filter_set(0, 1, 0, 0), ret);

    return RT_ERR_OK;

}

int32 _dal_rtl9607f_use_ca_specpkt_ctrl_set(rtk_port_t port, rtk_rate_storm_group_t stormType, rtk_enable_t enable)
{
    rt_error_common_t ret = RT_ERR_OK;
    aal_l2_specpkt_type_t  specpkt_type = AAL_L2_SPECPKT_TYPE_INVALID;
    aal_l2_specpkt_ctrl_mask_t  _mask;
    aal_l2_specpkt_ctrl_t  config;
    aal_ilpb_cfg_t ilpb_cfg;
    ca_uint32_t agr_pol_id = 0;

    RT_ERR_CHK((rt_error_common_t)_dal_rtl9607f_agr_flow_profile_id_get(stormType, &agr_pol_id), ret);

    memset(&_mask,0xff,sizeof(aal_l2_specpkt_ctrl_mask_t));
    memset(&config,0x00,sizeof(aal_l2_specpkt_ctrl_t));
    memset(&ilpb_cfg, 0x00, sizeof(aal_ilpb_cfg_t));

    if (STORM_GROUP_ARP == stormType)
        specpkt_type = AAL_L2_SPECPKT_TYPE_ARP;
    if (STORM_GROUP_IGMP_MLD == stormType)
        specpkt_type = AAL_L2_SPECPKT_TYPE_IGMP_MLD;
    if (STORM_GROUP_DHCP == stormType)
        specpkt_type = AAL_L2_SPECPKT_TYPE_DHCP;
    if (STORM_GROUP_ETHERTYPE == stormType)
        specpkt_type = AAL_L2_SPECPKT_TYPE_UDF_0;
    if (STORM_GROUP_DOT1X == stormType)
        specpkt_type = AAL_L2_SPECPKT_TYPE_802_DOT1X;
    if (STORM_GROUP_LOOP_DETECT == stormType)
        return dal_rtl9607f_trap_portLoopDetectPktPolicer_set(port, enable, agr_pol_id);

    RT_ERR_CHK((rt_error_common_t)aal_port_ilpb_cfg_get(0, port, &ilpb_cfg), ret);
    RT_ERR_CHK((rt_error_common_t)aal_l2_specpkt_ctrl_get(0, ilpb_cfg.spcl_pkt_idx, specpkt_type, &config), ret);

    config.grp_flow_en = enable;

    config.flowid = (ca_uint16_t)(agr_pol_id  << 9);
    // bypass_per_port = 0, or no_bypass_per_port=1
    if(!enable)
    {
        if(config.ldpid != AAL_LPORT_CPU_0)
            config.spcl_fwd = DISABLED;
    }
    else if(config.spcl_fwd == DISABLED )
    {
        config.spcl_fwd = enable;
        // SD5 said doesn't set CA_PORT_ID_CPU0
        //config.ldpid = CA_PORT_ID_CPU1;
        // Keep original data path behavior
#if defined(CONFIG_SDK_RTL8198X)
        if (HAL_IS_PON_PORT(port) || ilpb_cfg.cf_sel_wan_en)
#else
        if (HAL_IS_PON_PORT(port))
#endif
            config.ldpid = CA_PORT_ID_L3_WAN;
        else
            config.ldpid = CA_PORT_ID_L3_LAN;
    }

    // fdb limit/hash collision fwd permit to avoid FE drop
    config.limit_fwd_permit = 1;
    config.hash_cllsn_fwd_permit = 1;

    RT_ERR_CHK((rt_error_common_t)aal_l2_specpkt_ctrl_set(0, ilpb_cfg.spcl_pkt_idx, specpkt_type, _mask, &config), ret);

    return RT_ERR_OK;
}

int32 _dal_rtl9607f_use_ca_l2_flood_set(rtk_port_t port,rtk_rate_storm_group_t stormType,rtk_enable_t enable,ca_uint16_t flow_id)
{
    rt_error_common_t ret = RT_ERR_OK;
    aal_l2_te_policer_cfg_msk_t pol_msk;
    aal_l2_te_policer_cfg_t pol_cfg;
    aal_l2_te_policer_profile_msk_t profile_msk;
    aal_l2_te_policer_profile_t profile_cfg;
    ca_uint16_t flood_pol_id;
    aal_arb_pkt_fwd_type_t fwd_type = AAL_ARB_PKT_FWD_TYPE_MAX;
    aal_ilpb_cfg_t ilpb_cfg;

    fwd_type = (stormType == STORM_GROUP_UNKNOWN_UNICAST ? AAL_ARB_PKT_FWD_TYPE_UUC : fwd_type);
    fwd_type = (stormType == STORM_GROUP_BROADCAST ? AAL_ARB_PKT_FWD_TYPE_BC : fwd_type);

    RT_ERR_CHK((rt_error_common_t)aal_port_ilpb_cfg_get(0, port, &ilpb_cfg), ret);

    RT_ERR_CHK((rt_error_common_t)aal_arb_non_known_pol_map_get(0, ilpb_cfg.unkwn_pol_idx, fwd_type, &flood_pol_id), ret);

    memset((void *)&pol_cfg, 0, sizeof(aal_l2_te_policer_cfg_t));

    RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_pkt_type_cfg_get(0, flood_pol_id, &pol_cfg), ret);

    //init at aal_init
    //uuc: port0~3 => pkt1~4, port7 => pkt5, other port => 11
    //bc: port0~3 => pkt6~9, port7 => pkt10, other port => 12
    //printk("%s %d port=%d ilpb_cfg.unkwn_pol_idx=%d flood_pol_id=%d\n",__FUNCTION__,__LINE__,port,ilpb_cfg.unkwn_pol_idx,flood_pol_id);

    pol_msk.u32 = 0;

    pol_msk.s.pol_update_mode = 1;
    pol_cfg.pol_update_mode =(meterEntry[flow_id].pps == 0 ? CA_AAL_POLICER_UPDATE_MODE_BYTE : CA_AAL_POLICER_UPDATE_MODE_PKT);

    pol_msk.s.type = 1;
    pol_cfg.type = (DISABLED == enable ? CA_AAL_POLICER_TYPE_NONE : meterEntry[flow_id].mode );

    pol_msk.s.ipg = 1;
    pol_cfg.ipg = meterEntry[flow_id].ipg;

    RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_pkt_type_cfg_set(0, flood_pol_id, pol_msk, &pol_cfg), ret);

    memset(&profile_msk, 0, sizeof(aal_l2_te_policer_profile_msk_t));
    memset(&profile_cfg, 0, sizeof(aal_l2_te_policer_profile_t));

    RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_pkt_type_profile_get(0, flood_pol_id, &profile_cfg), ret);

    profile_msk.s.pir_m = 1;
    profile_msk.s.pir_k = 1;
    profile_msk.s.pbs = 1;
    profile_cfg.pir_m = meterEntry[flow_id].cir / 1000;
    profile_cfg.pir_k = meterEntry[flow_id].cir % 1000;
    profile_cfg.pbs = (meterEntry[flow_id].pbs == 0 ? 1 : meterEntry[flow_id].pbs);

    profile_msk.s.cir_m = 1;
    profile_msk.s.cir_k = 1;
    profile_msk.s.cbs = 1;
    profile_cfg.cir_m = profile_cfg.pir_m;
    profile_cfg.cir_k = profile_cfg.pir_k;
    profile_cfg.cbs = (meterEntry[flow_id].cbs == 0 ? 1 : meterEntry[flow_id].cbs);

    RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_pkt_type_profile_set(0, flood_pol_id, profile_msk, &profile_cfg), ret);

    return RT_ERR_OK;
}


int32 _dal_rtl9607f_check_meter_change_set(uint32 meterIndex)
{
    int32 ret = RT_ERR_OK;
    uint32  port,queue;

    HAL_SCAN_ALL_PORT(port)
    {
        if(meterIndex == stormCfg[port].uuc_flow_id)
        {
            RT_ERR_CHK(_dal_rtl9607f_use_ca_l2_flood_set(port,STORM_GROUP_UNKNOWN_UNICAST,stormCfg[port].uuc_enable,stormCfg[port].uuc_flow_id), ret);
        }

        if(meterIndex == stormCfg[port].bc_flow_id)
        {
            RT_ERR_CHK(_dal_rtl9607f_use_ca_l2_flood_set(port,STORM_GROUP_BROADCAST,stormCfg[port].bc_enable,stormCfg[port].bc_flow_id), ret);
        }

        if ( meterIndex == stormCfg[port].arp_flow_id)
        {
            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_ARP, stormCfg[port].arp_flow_id), ret);
            RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, STORM_GROUP_ARP, stormCfg[port].arp_enable), ret);
            if ( CA_AAL_POLICER_TYPE_NONE == meterEntry[meterIndex].mode)
            {
                stormCfg[port].arp_flow_id = -1;
            }
        }

        if (meterIndex == stormCfg[port].dhcp_flow_id)
        {
            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_DHCP, stormCfg[port].dhcp_flow_id), ret);
            RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, STORM_GROUP_DHCP, stormCfg[port].dhcp_enable), ret);

            if ( CA_AAL_POLICER_TYPE_NONE == meterEntry[meterIndex].mode)
            {
                stormCfg[port].dhcp_flow_id = -1;
            }
        }

        if (meterIndex == stormCfg[port].igmp_mld_flow_id)
        {
            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_IGMP_MLD, stormCfg[port].igmp_mld_flow_id), ret);
            RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, STORM_GROUP_IGMP_MLD, stormCfg[port].igmp_mld_enable), ret);

            if ( CA_AAL_POLICER_TYPE_NONE == meterEntry[meterIndex].mode)
            {
                stormCfg[port].igmp_mld_flow_id = -1;
            }
        }

        if ( meterIndex == stormCfg[port].ethertype_flow_id)
        {
            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_ETHERTYPE, stormCfg[port].ethertype_flow_id), ret);
            RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, STORM_GROUP_ETHERTYPE, stormCfg[port].ethertype_enable), ret);
            if ( CA_AAL_POLICER_TYPE_NONE == meterEntry[meterIndex].mode)
            {
                stormCfg[port].ethertype_flow_id = -1;
            }
        }

        if ( meterIndex == stormCfg[port].loopdetect_flow_id)
        {
            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_LOOP_DETECT, stormCfg[port].loopdetect_flow_id), ret);
            RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, STORM_GROUP_LOOP_DETECT, stormCfg[port].loopdetect_enable), ret);
            if ( CA_AAL_POLICER_TYPE_NONE == meterEntry[meterIndex].mode)
            {
                stormCfg[port].loopdetect_flow_id = -1;
            }
        }

        if ( meterIndex == stormCfg[port].dot1x_flow_id)
        {
            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_DOT1X, stormCfg[port].dot1x_flow_id), ret);
            RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, STORM_GROUP_DOT1X, stormCfg[port].dot1x_enable), ret);
            if ( CA_AAL_POLICER_TYPE_NONE == meterEntry[meterIndex].mode)
            {
                stormCfg[port].dot1x_flow_id = -1;
            }
        }
    }


    return RT_ERR_OK;
}

/* Module Name    : Rate                                           */
/* Sub-module Name: Configuration of egress port bandwidth control */

/* Function Name:
 *      dal_rtl9607f_rate_init
 * Description:
 *      Initial the rate module.
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9607f_rate_init(void)
{
    rt_error_common_t ret = RT_ERR_OK;
    ca_uint16_t flow_id;
    rtk_port_t port;

    ca_uint32_t maxidx = 0;
    TE_TE_GLB_CTRL_t glb_ctrl;

    aal_l2_te_policer_cfg_t     cfg;
    aal_l2_te_policer_cfg_msk_t msk;
    ca_uint32_t i = 0;

    aal_l3_te_glb_cfg_t l3_te_glb_cfg;
    aal_l3_te_policer_cfg_t l3_cfg;
    aal_l3_te_policer_cfg_msk_t l3_msk;
    aal_l3_te_policer_profile_t l3_profile;
    aal_l3_te_policer_profile_msk_t l3_profile_msk;

    aal_ilpb_cfg_msk_t ilpbCfgMsk;
    aal_ilpb_cfg_t ilpbCfg;
    ca_int32_t profile_id = -1;

    aal_l2_specpkt_type_t  type;
    aal_l2_specpkt_ctrl_mask_t  _mask;
    aal_l2_specpkt_ctrl_t  config;


    memset(stormCfg, 0, sizeof(rate_storm_cfg_t)*RTK_MAX_NUM_OF_PORTS);
    /* DHCP, ARP, IGMP need Special_Pkt or L2CLS or CF to assign pol_grp_id */

    for(port=0;port<RTK_MAX_NUM_OF_PORTS;port++)
    {
        stormCfg[port].uuc_enable = DISABLED;
        stormCfg[port].umc_enable = DISABLED;
        stormCfg[port].bc_enable = DISABLED;
        stormCfg[port].mc_enable = DISABLED;
        stormCfg[port].dhcp_enable = DISABLED;
        stormCfg[port].arp_enable = DISABLED;
        stormCfg[port].igmp_mld_enable = DISABLED;
        stormCfg[port].ethertype_enable = DISABLED;
        stormCfg[port].loopdetect_enable = DISABLED;
        stormCfg[port].dot1x_enable = DISABLED;
        stormCfg[port].uuc_flow_id =-1;
        stormCfg[port].umc_flow_id =-1;
        stormCfg[port].bc_flow_id =-1;
        stormCfg[port].mc_flow_id =-1;
        stormCfg[port].dhcp_flow_id =-1;
        stormCfg[port].arp_flow_id =-1;
        stormCfg[port].igmp_mld_flow_id =-1;
        stormCfg[port].ethertype_flow_id =-1;
        stormCfg[port].loopdetect_flow_id =-1;
        stormCfg[port].dot1x_flow_id =-1;
        stormCfg[port].bc_storm_index =-1;
        stormCfg[port].mc_storm_index =-1;
        stormCfg[port].dhcp_storm_index0 =-1;
        stormCfg[port].dhcp_storm_index1 =-1;
        stormCfg[port].arp_storm_index =-1;
        stormCfg[port].igmp_mld_storm_index0 =-1;
        stormCfg[port].igmp_mld_storm_index1 =-1;
        stormCfg[port].igmp_mld_storm_index2 =-1;
        stormCfg[port].igmp_mld_storm_index3 =-1;
        stormCfg[port].igmp_mld_storm_index4 =-1;
        stormCfg[port].ethertype_storm_index =-1;
        stormCfg[port].loopdetect_storm_index =-1;
        stormCfg[port].dot1x_storm_index =-1;

    }


    for (flow_id = 0; flow_id <= CA_AAL_L3_MAX_FLOW_ID; flow_id++)
    {
        memset((void *)&l3_cfg, 0, sizeof(l3_cfg));
        memset((void *)&l3_msk, 0, sizeof(l3_msk));
        memset((void *)&l3_profile, 0, sizeof(l3_profile));
        memset((void *)&l3_profile_msk, 0, sizeof(l3_profile_msk));

        /* l3 flow engine */
        l3_msk.s.type = 1;
        l3_msk.s.pol_update_mode = 1;

        l3_cfg.ipg   = 20;
        l3_msk.s.ipg = 1;

        l3_cfg.type = CA_AAL_POLICER_TYPE_NONE;

        RT_ERR_CHK((rt_error_common_t)aal_l3_te_policer_flow_cfg_set(0, flow_id, l3_msk, &l3_cfg), ret);

        l3_profile_msk.s.cir_k = 1;
        l3_profile_msk.s.cir_m = 1;
        l3_profile_msk.s.cbs   = 1;
        l3_profile_msk.s.pir_k = 1;
        l3_profile_msk.s.pir_m = 1;
        l3_profile_msk.s.pbs   = 1;

        l3_profile.cir_k = CA_AAL_L3_TE_POL_MAX_RATE_K;
        l3_profile.cir_m = CA_AAL_L3_TE_POL_MAX_RATE_M;
        l3_profile.cbs   = CA_AAL_L3_TE_POL_MAX_CBS;
        l3_profile.pir_k = CA_AAL_L3_TE_POL_MAX_RATE_K;
        l3_profile.pir_m = CA_AAL_L3_TE_POL_MAX_RATE_M;
        l3_profile.pbs   = CA_AAL_L3_TE_POL_MAX_PBS;

        RT_ERR_CHK((rt_error_common_t)aal_l3_te_policer_flow_profile_set(0, flow_id, l3_profile_msk, &l3_profile), ret);
    }



    memset((void *)&cfg, 0, sizeof(cfg));
    memset((void *)&msk, 0, sizeof(msk));

    cfg.ipg   = 20;
    msk.s.ipg = 1;

    for (i=0; i<=(ca_uint32_t)CA_AAL_POLICER_PKT_TYPE_UBC; i++)
    {
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_pkt_type_cfg_set(0, (aal_l2_te_policer_pkt_type_t)i, msk, &cfg), ret);
    }

    for (i=0; i<=CA_AAL_MAX_PORT_ID; i++)
    {
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_port_cfg_set(0, i, msk, &cfg), ret);
    }

    for (i=0; i<=CA_AAL_MAX_AGR_FLOW_ID; i++)
    {
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_agr_flow_cfg_set(0, i, msk, &cfg), ret);
    }


    memset((void *)&l3_cfg, 0, sizeof(l3_cfg));
    memset((void *)&l3_msk, 0, sizeof(l3_msk));

    l3_cfg.ipg   = 20;
    l3_msk.s.ipg = 1;
    for (i=0; i<=(ca_uint32_t)CA_AAL_L3_POLICER_PKT_TYPE_UBC; i++)
    {
        RT_ERR_CHK((rt_error_common_t)aal_l3_te_policer_pkt_type_cfg_set(0, (aal_l3_te_policer_pkt_type_t)i, l3_msk, &l3_cfg), ret);
    }

    for (i=0; i<=CA_AAL_L3_MAX_PORT_ID; i++)
    {
        RT_ERR_CHK((rt_error_common_t)aal_l3_te_policer_port_cfg_set(0, i, l3_msk, &l3_cfg), ret);
    }

    memset((void *)&l3_te_glb_cfg, 0, sizeof(aal_l3_te_glb_cfg_t));
    RT_ERR_CHK((rt_error_common_t)aal_l3_te_glb_cfg_get(0, &l3_te_glb_cfg), ret);
    maxidx = (l3_te_glb_cfg.sel3flows == 0 ? CA_AAL_L3_MAX_AGR_FLOW_ID : CA_AAL_L3_MAX_FLOW2_ID);

    for (i=0; i<=maxidx; i++)
    {
        RT_ERR_CHK((rt_error_common_t)aal_l3_te_policer_agr_flow_cfg_set(0, i, l3_msk, &l3_cfg), ret);
    }
    for (i=0; i<=CA_AAL_L3_MAX_FLOW3_ID; i++)
    {
        RT_ERR_CHK((rt_error_common_t)aal_l3_te_policer_flow3_cfg_set(0, i, l3_msk, &l3_cfg), ret);
    }

    RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_ipq_set(0, 0, cfg.ipg), ret);

    RT_ERR_CHK((rt_error_common_t)aal_l3_te_shaper_ipq_set(0, 0, l3_cfg.ipg), ret);

    for (i=0; i<=CA_AAL_MAX_PORT_ID; i++)
    {
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_port_ipq_set(0, i, 0), ret);
        RT_ERR_CHK((rt_error_common_t)aal_l3_te_shaper_port_ipq_set(0, i, 0), ret);
    }


    memset(meterEntry, 0, USD_METER_MAX_NUM * sizeof(meter_cfg_t));

    HAL_SCAN_ALL_ETH_PORT(port)
    {
        if (HAL_IS_XE_PORT(port))
            continue;

        for (type = AAL_L2_SPECPKT_TYPE_IGMP_MLD; type <= AAL_L2_SPECPKT_TYPE_DHCP; type++)
        {
                memset(&_mask, 0xff, sizeof(aal_l2_specpkt_ctrl_mask_t));
                memset(&config, 0x00, sizeof(aal_l2_specpkt_ctrl_t));
                memset(&ilpbCfg, 0x00, sizeof(aal_ilpb_cfg_t));
                RT_ERR_CHK((rt_error_common_t)aal_port_ilpb_cfg_get(0, port, &ilpbCfg), ret);
                RT_ERR_CHK((rt_error_common_t)aal_l2_specpkt_ctrl_get(0, ilpbCfg.spcl_pkt_idx, type, &config), ret);
                config.grp_flow_en = 0;
                config.flowid = 0;
                config.spcl_fwd = 0;
                config.ldpid = 0;
                RT_ERR_CHK((rt_error_common_t)aal_l2_specpkt_ctrl_set(0, ilpbCfg.spcl_pkt_idx, type, _mask, &config), ret);
        }
    }

    HAL_SCAN_ALL_ETH_PORT(port)
    {
        dal_rtl9607f_rate_portIgrBandwidthCtrlRate_set(port,RTL9607F_INGRESS_RATE_MAX);
        dal_rtl9607f_rate_portEgrBandwidthCtrlRate_set(port,RTL9607F_EGRESS_RATE_MAX);
    }

    RT_ERR_CHK((rt_error_common_t)aal_l2_specpkt_arp_filter_set(0, 0, 0, 0), ret);

    rate_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_init */

/* Function Name:
 *      dal_rtl9607f_rate_portIgrBandwidthCtrlRate_get
 * Description:
 *      Get the ingress bandwidth control rate.
 * Input:
 *      port  - port id
 * Output:
 *      pRate - ingress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) The rate unit is 1 kbps and the range is from 8k to 1048568k.
 *      (2) The granularity of rate is 8 kbps
 */
int32
dal_rtl9607f_rate_portIgrBandwidthCtrlRate_get(rtk_port_t port, uint32 *pRate)
{
    rt_error_common_t ret = RT_ERR_OK;
    ca_port_id_t port_id;
    aal_l2_te_policer_cfg_t     cfg;
    aal_l2_te_policer_profile_t profile;
    aal_l3_te_policer_cfg_t     l3_cfg;
    aal_l3_te_policer_profile_t l3_profile;
    ca_port_id_t map_port = 0;
    ca_uint32_t port_type = CA_PORT_TYPE_INVALID;
    ca_uint32_t cir, mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    port_type = PORT_TYPE(port_id);
    RT_PARAM_CHK((port_type != CA_PORT_TYPE_ETHERNET && \
        port_type != CA_PORT_TYPE_L3 && port_type != CA_PORT_TYPE_EPON &&
        port_type != CA_PORT_TYPE_GPON), RT_ERR_FAILED);

    map_port      = PORT_ID(port_id);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_ETHERNET && \
        map_port > CA_AAL_MAX_PORT_ID), RT_ERR_FAILED);

    if (port_type == CA_PORT_TYPE_L3 || port_type == CA_PORT_TYPE_EPON || \
            port_type == CA_PORT_TYPE_GPON)
    {
        RT_PARAM_CHK((map_port != CA_PORT_ID_NI7), RT_ERR_PORT_ID);
    }

    memset((void *)&cfg, 0, sizeof(cfg));
    memset((void *)&profile, 0, sizeof(profile));
    memset((void *)&l3_cfg, 0, sizeof(l3_cfg));
    memset((void *)&l3_profile, 0, sizeof(l3_profile));

    if (port_type == CA_PORT_TYPE_L3)   /* WAN */
    {
        map_port = HAL_GET_PON_PORT();   /* entry 7 is used for traffic from WAN port */

        RT_ERR_CHK((rt_error_common_t)aal_l3_te_policer_port_cfg_get(0, map_port, &l3_cfg), ret);

        RT_ERR_CHK((rt_error_common_t)aal_l3_te_policer_port_profile_get(0, map_port, &l3_profile), ret);

        cir = l3_profile.cir_m * 1000 + l3_profile.cir_k;
        mode = l3_cfg.type;

        if (mode == CA_AAL_L3_POLICER_TYPE_NONE)
            cir = RTL9607F_INGRESS_RATE_MAX;
    }
    else
    {
        map_port = (map_port == CA_PORT_ID_NI7 ? 10 /*NI_RXMUX_L3FE_PSPID*/ : map_port);

        RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_port_cfg_get(0, map_port, &cfg), ret);

        RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_port_profile_get(0, map_port, &profile), ret);

        cir = profile.cir_m * 1000 + profile.cir_k;
        mode = cfg.type;

        if(mode == CA_AAL_POLICER_TYPE_NONE)
            cir = RTL9607F_INGRESS_RATE_MAX;
    }

    if(cir >= RTL9607F_INGRESS_RATE_MAX)
        *pRate = RTL9607F_INGRESS_RATE_MAX;
    else
    {
        *pRate = cir;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_portIgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_rtl9607f_rate_portIgrBandwidthCtrlRate_set
 * Description:
 *      Set the ingress bandwidth control rate.
 * Input:
 *      port - port id
 *      rate - ingress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The rate unit is 1 kbps and the range is from 8k to 1048568k.
 *      (2) The granularity of rate is 8 kbps
 */
int32
dal_rtl9607f_rate_portIgrBandwidthCtrlRate_set(rtk_port_t port, uint32 rate)
{
    rt_error_common_t ret = RT_ERR_OK;
    ca_port_id_t port_id;

    ca_uint32_t cir, pir, cbs = 0, pbs = 0;
    aal_l2_te_policer_cfg_t cfg;
    aal_l2_te_policer_cfg_msk_t msk;
    aal_l2_te_policer_profile_t profile;
    aal_l2_te_policer_profile_msk_t profile_msk;
    aal_l3_te_policer_cfg_t l3_cfg;
    aal_l3_te_policer_cfg_msk_t l3_msk;
    aal_l3_te_policer_profile_t l3_profile;
    aal_l3_te_policer_profile_msk_t l3_profile_msk;
    ca_port_id_t map_port_id = 0;
    ca_uint32_t  port_type = CA_PORT_TYPE_INVALID;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    port_type = PORT_TYPE(port_id);

    RT_PARAM_CHK((port_type != CA_PORT_TYPE_ETHERNET && \
            port_type != CA_PORT_TYPE_L3 && port_type != CA_PORT_TYPE_EPON &&
            port_type != CA_PORT_TYPE_GPON), RT_ERR_FAILED);

    map_port_id = PORT_ID(port_id);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_ETHERNET && \
        map_port_id > CA_AAL_MAX_PORT_ID), RT_ERR_FAILED);

    if (port_type == CA_PORT_TYPE_L3 || port_type == CA_PORT_TYPE_EPON || \
            port_type == CA_PORT_TYPE_GPON)
    {
        RT_PARAM_CHK((map_port_id != CA_PORT_ID_NI7), RT_ERR_PORT_ID);
    }


    /* L3 only supports LSPID 0x0-0x7, l3 stream has LSPID 0x18 or 0x19 and therefore L3 TM cannot support WAN port */

    if (port_type == CA_PORT_TYPE_L3)   /* WAN */
    {
        /* need to configure L3 engine for WAN port */

        memset((void *)&l3_cfg, 0, sizeof(l3_cfg));
        memset((void *)&l3_msk, 0, sizeof(l3_msk));
        memset((void *)&l3_profile, 0, sizeof(l3_profile));
        memset((void *)&l3_profile_msk, 0, sizeof(l3_profile_msk));

        l3_msk.s.type = 1;
        l3_msk.s.pol_update_mode = 1;

        l3_cfg.pol_update_mode =  CA_AAL_L3_POLICER_UPDATE_MODE_BYTE;

        if(rate >= RTL9607F_INGRESS_RATE_MAX)
        {
            cir = RTL9607F_INGRESS_RATE_MAX;
            pir = RTL9607F_INGRESS_RATE_MAX;

            l3_cfg.type = CA_AAL_L3_POLICER_TYPE_NONE;
        }
        else
        {
            cir = rate;
            pir = rate;

            l3_cfg.type = CA_AAL_L3_POLICER_TYPE_RFC2697;
        }
        #if 0
            // original sw_def:
            cbs = (cir/1000 + 511) >> 9;
        #else
            // QA_tune_def:  
            // for byte mode
            // if CIR=1_Mbps, CBS=30
            // if CIR=2_Mbps, CBS=60
            // if CIR=10-100MBps, CBS=300
            // if CIR=100-1Gbps, CBS=3000
            //...
            // for packet mode. CBS= CIR_M/2
            if (cir < 1000)
                cbs = 30;
            if (1000 <= cir && cir < 10000) 
                cbs = (cir/1000) * 30;
        
            if (10000 <= cir && cir < 100000)
                cbs = 10000/1000 * 30;
            
            if (100000 <= cir && cir < 1000000)
                cbs = 100000/1000 * 30;

            if (1000000 <= cir)
                cbs = 0xFFF;    
        #endif
        cbs = (cbs == 0) ? 1: cbs;
        /*The burst size granularity is 256 bytes or 1 packet.
        The minimum value is roundup (committed_rate_m/512) in byte mode
        and roudup (committed_rate_m/2) in packet mode.*/
        pbs = (pir/1000 + 511) >> 9;
        pbs = (pbs == 0) ? 1: pbs;

        /*
        ** Entry 7 is used for traffic from WAN port, which is mapped from LSPID 0x7 and 0x18.
        ** Other entires are invalid for software.
        */
        map_port_id = HAL_GET_PON_PORT();

        RT_ERR_CHK((rt_error_common_t)aal_l3_te_policer_port_cfg_set(0, map_port_id, l3_msk, &l3_cfg), ret);

        l3_profile_msk.s.cir_k = 1;
        l3_profile_msk.s.cir_m = 1;
        l3_profile_msk.s.cbs   = 1;
        l3_profile_msk.s.pir_k = 1;
        l3_profile_msk.s.pir_m = 1;
        l3_profile_msk.s.pbs   = 1;

        l3_profile.cir_k = cir % 1000;
        l3_profile.cir_m = cir / 1000;
        l3_profile.cbs   = cbs;
        l3_profile.pir_k = pir % 1000;
        l3_profile.pir_m = pir / 1000;
        l3_profile.pbs   = pbs;

        RT_ERR_CHK((rt_error_common_t)aal_l3_te_policer_port_profile_set(0, map_port_id, l3_profile_msk, &l3_profile), ret);
    }
    else
    {
        /* ethernet port configuration */
        memset((void *)&cfg, 0, sizeof(cfg));
        memset((void *)&msk, 0, sizeof(msk));
        memset((void *)&profile, 0, sizeof(profile));
        memset((void *)&profile_msk, 0, sizeof(profile_msk));

        msk.s.type = 1;
        msk.s.pol_update_mode = 1;

        cfg.pol_update_mode = CA_AAL_POLICER_UPDATE_MODE_BYTE;

        if(rate >= RTL9607F_INGRESS_RATE_MAX)
        {
            cir = RTL9607F_INGRESS_RATE_MAX;
            pir = RTL9607F_INGRESS_RATE_MAX;

            cfg.type = CA_AAL_POLICER_TYPE_NONE;
        }
        else
        {
            cir = rate;
            pir = rate;

            cfg.type = CA_AAL_POLICER_TYPE_RFC2697;
        }
        #if 0
        // original sw_def:
            cbs = (cir/1000 + 511) >> 9;
        #else
        // QA_tune_def:  
        // for byte mode
        // if CIR=1_Mbps, CBS=30
        // if CIR=2_Mbps, CBS=60
        // if CIR=10-100MBps, CBS=300
        // if CIR=100-1Gbps, CBS=3000
        //...
        // for packet mode. CBS= CIR_M/2
            if (cir < 1000)
                cbs = 30;
      
            if (1000 <= cir && cir < 10000) 
                cbs = (cir/1000) * 30;
        
            if (10000 <= cir && cir < 100000)
                cbs = 10000/1000 * 30;
            
            if (100000 <= cir && cir < 1000000)
                cbs = 100000/1000 * 30;

            if (1000000 <= cir)
                cbs = 0xFFF;    
        #endif
        cbs = (cbs == 0) ? 1: cbs;
        /*The burst size granularity is 256 bytes or 1 packet.
        The minimum value is roundup (committed_rate_m/512) in byte mode
        and roudup (committed_rate_m/2) in packet mode.*/
        pbs = (pir/1000 + 511) >> 9;
        pbs = (pbs == 0) ? 1: pbs;

        if (map_port_id == CA_PORT_ID_NI7)
        {
            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_port_cfg_set(0, map_port_id, msk, &cfg), ret);
            map_port_id = 10;//NI_RXMUX_L3FE_PSPID;
        }

        RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_port_cfg_set(0, map_port_id, msk, &cfg), ret);

        profile_msk.s.cir_k = 1;
        profile_msk.s.cir_m = 1;
        profile_msk.s.cbs   = 1;
        profile_msk.s.pir_k = 1;
        profile_msk.s.pir_m = 1;
        profile_msk.s.pbs   = 1;

        profile.cir_k = cir % 1000;
        profile.cir_m = cir / 1000;
        profile.cbs   = (cbs == 0) ? 1: cbs;
        profile.pir_k = pir % 1000;
        profile.pir_m = pir / 1000;
        profile.pbs   = (pbs == 0) ? 1: pbs;

        /* other L2 port */
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_port_profile_set(0, map_port_id, profile_msk, &profile), ret);

        if (map_port_id == 10/*NI_RXMUX_L3FE_PSPID*/)
        {
            RT_ERR_CHK((rt_error_common_t)aal_l2_te_policer_port_profile_set(0, CA_PORT_ID_NI7, profile_msk, &profile), ret);
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_portIgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_rtl9607f_rate_portEgrBandwidthCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      port  - port id
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) The rate unit is 1 kbps and the range is from 1k to 1048568k.
 *      (2) The granularity of rate is 1 kbps
 */
int32
dal_rtl9607f_rate_portEgrBandwidthCtrlRate_get(rtk_port_t port, uint32 *pRate)
{
    rt_error_common_t ret = RT_ERR_OK;
    ca_port_id_t port_id;
    aal_l2_te_shaper_tbc_cfg_t     profile;
    aal_l3_te_shaper_tbc_cfg_t     l3_profile;
    ca_uint32_t port_type = CA_PORT_TYPE_INVALID;
    ca_port_id_t map_port = 0;
    ca_uint32_t rate;
    ca_boolean_t enable;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
#if defined(CONFIG_SDK_RTL8198X)
    if (port == CA_PORT_ID_L3_WAN)
        port_id = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, port);
    else
        RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
#else
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
#endif
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    port_id =(port == CA_PORT_ID_NI7 ? CA_PORT_ID(CA_PORT_TYPE_L3, port) : port_id);

    port_type = PORT_TYPE(port_id);

    RT_PARAM_CHK((port_type != CA_PORT_TYPE_ETHERNET && \
        port_type != CA_PORT_TYPE_CPU && port_type != CA_PORT_TYPE_L3), RT_ERR_FAILED);

    map_port      = PORT_ID(port_id);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_ETHERNET && \
        map_port > CA_AAL_MAX_PORT_ID && (map_port != CA_PORT_ID_L3_WAN && \
        map_port != CA_PORT_ID_L3_LAN)), RT_ERR_FAILED);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_CPU && \
        (map_port < CA_PORT_ID_CPU0 || map_port > CA_PORT_ID_CPU7)), RT_ERR_FAILED);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_L3 && \
        map_port != CA_PORT_ID_NI7), RT_ERR_FAILED);

    memset((void *)&profile, 0, sizeof(profile));
    memset((void *)&l3_profile, 0, sizeof(l3_profile));

    switch (port_type)
    {
    case CA_PORT_TYPE_CPU:
        map_port -= CA_PORT_ID_CPU0;
        RT_ERR_CHK((rt_error_common_t)aal_l3_te_shaper_port_tbc_get(0, map_port, &l3_profile), ret);

        rate = l3_profile.rate_m * 1000 + l3_profile.rate_k;
        enable = (l3_profile.state == CA_AAL_L3_SHAPER_ADMIN_STATE_SHAPER) ? TRUE : FALSE;

        break;
    case CA_PORT_TYPE_L3:   /* WAN */
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_port_tbc_get(0, map_port, &profile), ret);

        rate = profile.rate_m * 1000 + profile.rate_k;
        enable = (profile.state == CA_AAL_SHAPER_ADMIN_STATE_SHAPER) ? TRUE : FALSE;
        break;

    case CA_PORT_TYPE_EPON:
    case CA_PORT_TYPE_GPON:
        RT_ERR_CHK ((rt_error_common_t)aal_l2_te_shaper_port_tbc_get(0, map_port, &profile), ret);

        rate = profile.rate_m * 1000 + profile.rate_k;
        enable = (profile.state == CA_AAL_SHAPER_ADMIN_STATE_SHAPER) ? TRUE : FALSE;
        break;

    case CA_PORT_TYPE_ETHERNET:
        /* convert LDPID to TM LDPID */
        if (map_port == CA_PORT_ID_L3_WAN)
            map_port = CA_AAL_L2TM_DPID_L3_WAN;
        else if (map_port == CA_PORT_ID_L3_LAN)
            map_port = CA_AAL_L2TM_DPID_L3_LAN;

        RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_port_tbc_get(0, map_port, &profile), ret);

        rate = profile.rate_m * 1000 + profile.rate_k;
        enable = (profile.state == CA_AAL_SHAPER_ADMIN_STATE_SHAPER) ? TRUE : FALSE;
        break;

    default:
        break;
    }

    if(enable == 0 || rate >= RTL9607F_EGRESS_RATE_MAX)
    {
        *pRate = RTL9607F_EGRESS_RATE_MAX;
    }
#if defined(CONFIG_SDK_RTL8198X)
    else if (port == CA_PORT_ID_L3_WAN && enable == 0)
    {
        *pRate = RTL9607F_EGRESS_RATE_MAX;
    }
#endif
    else
    {
        *pRate = rate;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_portEgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_rtl9607f_rate_portEgrBandwidthCtrlRate_set
 * Description:
 *      Set the egress bandwidth control rate.
 * Input:
 *      port - port id
 *      rate - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The rate unit is 1 kbps and the range is from 1k to 1048568k.
 *      (2) The granularity of rate is 1 kbps
 */
int32
dal_rtl9607f_rate_portEgrBandwidthCtrlRate_set(rtk_port_t port, uint32 rate)
{
    rt_error_common_t ret = RT_ERR_OK;
    ca_port_id_t port_id;
    aal_l2_te_shaper_tbc_cfg_msk_t profile_msk;
    aal_l2_te_shaper_tbc_cfg_t     profile;
    aal_l3_te_shaper_tbc_cfg_msk_t l3_profile_msk;
    aal_l3_te_shaper_tbc_cfg_t     l3_profile;
    ca_uint32_t port_type = CA_PORT_TYPE_INVALID;
    ca_port_id_t map_port = 0;
    ca_boolean_t enable,pps;
    ca_uint32_t egr_rate, burst_size;
    uint32 cur_pon_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
#if defined(CONFIG_SDK_RTL8198X)
    if (port == CA_PORT_ID_L3_WAN)
        port_id = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, port);
    else
        RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
#else
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
#endif
    port_id = (port == CA_PORT_ID_NI7 ? CA_PORT_ID(CA_PORT_TYPE_L3, port) : port_id);

    port_type = PORT_TYPE(port_id);

    RT_PARAM_CHK((port_type != CA_PORT_TYPE_ETHERNET && \
        port_type != CA_PORT_TYPE_CPU && port_type != CA_PORT_TYPE_L3), RT_ERR_FAILED);

    map_port      = PORT_ID(port_id);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_ETHERNET && \
        map_port > CA_AAL_MAX_PORT_ID && (map_port != CA_PORT_ID_L3_WAN && \
        map_port != CA_PORT_ID_L3_LAN)), RT_ERR_FAILED);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_CPU && \
        (map_port < CA_PORT_ID_CPU0 || map_port > CA_PORT_ID_CPU7)), RT_ERR_FAILED);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_L3 && \
        map_port != CA_PORT_ID_NI7), RT_ERR_FAILED);

    if(rate >= RTL9607F_EGRESS_RATE_MAX)
    {
        enable = 0;
        egr_rate = RTL9607F_EGRESS_RATE_MAX;
    }
    else
    {
        enable = 1;
        egr_rate = rate;
    }

    pps = 0; /* True - PPS mode, false - BPS mode */
    burst_size = (rate/1000 + 511) >> 9;
    burst_size = (burst_size == 0) ? 1: burst_size;
    /*The burst size granularity is 256 bytes or 1 packet.
    The minimum value is roundup (committed_rate_m/512) in byte mode
    and roudup (committed_rate_m/2) in packet mode.*/

    memset((void *)&profile, 0, sizeof(profile));
    memset((void *)&profile_msk, 0, sizeof(profile_msk));
    memset((void *)&l3_profile, 0, sizeof(l3_profile));
    memset((void *)&l3_profile_msk, 0, sizeof(l3_profile_msk));

    /* configure L2 traffic shaper */
    profile_msk.s.bs     = 1;
    profile_msk.s.rate_k = 1;
    profile_msk.s.rate_m = 1;
    profile_msk.s.mode   = 1;
    profile_msk.s.state  = 1;

    profile.bs     = burst_size;    /* in unit of 256 bytes */
    profile.rate_k = egr_rate % 1000;
    profile.rate_m = egr_rate / 1000;
    profile.mode   = pps ? CA_AAL_SHAPER_MODE_PPS : CA_AAL_SHAPER_MODE_BPS;
    profile.state  = enable ? CA_AAL_SHAPER_ADMIN_STATE_SHAPER : CA_AAL_SHAPER_ADMIN_STATE_FORWARD;

    if (profile.state == CA_AAL_SHAPER_ADMIN_STATE_SHAPER)
    {
        /* check burst size, minimum value is dependent on rate_m */
        if (profile.mode == CA_AAL_SHAPER_MODE_BPS)
        {
            if (profile.bs < ((profile.rate_m + 511) >> 9))
            {
                RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
                return RT_ERR_FAILED;
            }
        }
        else /* pps */
        {
            if (profile.bs < ((profile.rate_m + 1) >> 2))
            {
                RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
                return RT_ERR_FAILED;
            }
        }
    }

    /* configure L3 traffic shaper */
    l3_profile_msk.s.bs     = 1;
    l3_profile_msk.s.rate_k = 1;
    l3_profile_msk.s.rate_m = 1;
    l3_profile_msk.s.mode   = 1;
    l3_profile_msk.s.state  = 1;

    l3_profile.bs     = burst_size;
    l3_profile.rate_k = egr_rate % 1000;
    l3_profile.rate_m = egr_rate / 1000;
    l3_profile.mode   = pps ? CA_AAL_SHAPER_MODE_PPS : CA_AAL_SHAPER_MODE_BPS;
    l3_profile.state  = enable ? CA_AAL_SHAPER_ADMIN_STATE_SHAPER : CA_AAL_SHAPER_ADMIN_STATE_FORWARD;

    switch (port_type)
    {
    case CA_PORT_TYPE_CPU:
        map_port -= CA_PORT_ID_CPU0;
        RT_ERR_CHK((rt_error_common_t)aal_l3_te_shaper_port_tbc_set(0, map_port, l3_profile_msk, &l3_profile), ret);
        break;

    case CA_PORT_TYPE_L3:       /* WAN */
        cur_pon_mode = aal_pon_mac_mode_get(0);
        if(CA_PON_MODE_IS_ETHERNET(cur_pon_mode))
        {
            RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_port_tbc_set(0, map_port, profile_msk, &profile), ret);
            
            map_port += CA_AAL_L3_TE_NI_PORT_OFFSET; /* entey 15 for WAN */
            RT_ERR_CHK((rt_error_common_t)aal_l3_te_shaper_port_tbc_set(0, map_port, l3_profile_msk, &l3_profile), ret);
        }
        break;

    case CA_PORT_TYPE_EPON:
    case CA_PORT_TYPE_GPON:
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_port_tbc_set(0, map_port, profile_msk, &profile), ret);
        break;

    case CA_PORT_TYPE_ETHERNET:
        /* convert LDPID to TM LDPID */
        if (map_port == CA_PORT_ID_L3_WAN)
            map_port = CA_AAL_L2TM_DPID_L3_WAN;
        else if (map_port == CA_PORT_ID_L3_LAN)
            map_port = CA_AAL_L2TM_DPID_L3_LAN;

        RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_port_tbc_set(0, map_port, profile_msk, &profile), ret);
        break;

    default:
        break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_portEgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_rtl9607f_rate_queueEgrBandwidthCtrlRate_get
 * Description:
 *      Get queue egress bandwidth control rate.
 * Input:
 *      port  - port id
 *		queue - queue id
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 *
 */
int32
dal_rtl9607f_rate_queueEgrBandwidthCtrlRate_get(rtk_port_t port, rtk_qid_t queue, uint32 *pRate)
{
	rt_error_common_t ret = RT_ERR_OK;
    aal_l2_te_shaper_tbc_cfg_t     profile;
    aal_l3_te_shaper_tbc_cfg_t     l3_profile;
    ca_uint32_t port_type = CA_PORT_TYPE_INVALID;
    ca_port_id_t port_id = 0, map_port = 0;
    ca_uint32_t voq_id = queue;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(rate_init);

	port_id = RTK2CA_PORT_ID(port);

	/* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
#if defined(CONFIG_SDK_RTL8198X)
    if (port == CA_PORT_ID_L3_WAN)
        port_id = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, port);
    else
        RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
#else
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
#endif
    RT_PARAM_CHK((queue > HAL_QUEUE_ID_MAX()), RT_ERR_QUEUE_ID);

    port_id = (port == CA_PORT_ID_NI7 ? CA_PORT_ID(CA_PORT_TYPE_L3, port) : port_id);

    port_type = PORT_TYPE(port_id);

	RT_PARAM_CHK((port_type != CA_PORT_TYPE_ETHERNET && \
        port_type != CA_PORT_TYPE_CPU && port_type != CA_PORT_TYPE_L3), RT_ERR_FAILED);

	map_port      = PORT_ID(port_id);

	RT_PARAM_CHK((port_type == CA_PORT_TYPE_ETHERNET && \
        map_port > CA_AAL_MAX_PORT_ID && (map_port != CA_PORT_ID_L3_WAN && \
        map_port != CA_PORT_ID_L3_LAN)), RT_ERR_FAILED);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_CPU && \
        (map_port < CA_PORT_ID_CPU0 || map_port > CA_PORT_ID_CPU7)), RT_ERR_FAILED);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_L3 && \
        map_port != CA_PORT_ID_NI7), RT_ERR_FAILED);

    memset((void *)&profile,     0, sizeof(profile));
    memset((void *)&l3_profile,  0, sizeof(l3_profile));

    switch (port_type)
    {
    case CA_PORT_TYPE_ETHERNET:
        /* convert LDPID to TM LDPID */
        if (map_port == CA_PORT_ID_L3_WAN)
            map_port = CA_AAL_L2TM_DPID_L3_WAN;
        else if (map_port == CA_PORT_ID_L3_LAN)
            map_port = CA_AAL_L2TM_DPID_L3_LAN;

        voq_id = map_port * 8 + queue;
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_voq_tbc_get(0, voq_id, &profile), ret);

		if (profile.state == CA_AAL_SHAPER_ADMIN_STATE_SHAPER)
			*pRate = profile.rate_m * 500 + profile.rate_k;
		else
			*pRate = RTL9607F_EGRESS_RATE_MAX;
        break;

    case CA_PORT_TYPE_L3:
        voq_id = map_port * 8 + queue;
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_voq_tbc_get(0, voq_id, &profile), ret);

        if (profile.state == CA_AAL_SHAPER_ADMIN_STATE_SHAPER)
            *pRate = profile.rate_m * 500 + profile.rate_k;
        else
            *pRate = RTL9607F_EGRESS_RATE_MAX;
        break;

    case CA_PORT_TYPE_CPU:
        map_port -= CA_PORT_ID_CPU0;
        voq_id = map_port * 8 + queue;
        RT_ERR_CHK((rt_error_common_t)aal_l3_te_shaper_voq_tbc_get(0, voq_id, &l3_profile), ret);

		if (l3_profile.state == CA_AAL_L3_SHAPER_ADMIN_STATE_SHAPER)
			*pRate = l3_profile.rate_m * 500 + l3_profile.rate_k;
		else
			*pRate = RTL9607F_EGRESS_RATE_MAX;
        break;

    default:
        break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_queueEgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_rtl9607f_rate_queueEgrBandwidthCtrlRate_set
 * Description:
 *      Set queue egress bandwidth control rate.
 * Input:
 *      port  - port id
 *		queue - queue id
 *		rate  - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 *
 */
int32
dal_rtl9607f_rate_queueEgrBandwidthCtrlRate_set(rtk_port_t port, rtk_qid_t queue, uint32 rate)
{
    rt_error_common_t ret = RT_ERR_OK;
    ca_port_id_t port_id;
    aal_l2_te_shaper_tbc_cfg_msk_t profile_msk;
    aal_l2_te_shaper_tbc_cfg_t     profile;
    aal_l3_te_shaper_tbc_cfg_msk_t l3_profile_msk;
    aal_l3_te_shaper_tbc_cfg_t     l3_profile;
	ca_uint32_t voq_id = queue;
    ca_uint32_t port_type = CA_PORT_TYPE_INVALID;
    ca_port_id_t map_port = 0;
    ca_boolean_t enable,pps;
    ca_uint32_t egr_rate, burst_size;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

	/* check Init status */
    RT_INIT_CHK(rate_init);

	port_id = RTK2CA_PORT_ID(port);

	/* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
#if defined(CONFIG_SDK_RTL8198X)
    if (port == CA_PORT_ID_L3_WAN)
        port_id = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, port);
    else
        RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
#else
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
#endif
    RT_PARAM_CHK((queue > HAL_QUEUE_ID_MAX()), RT_ERR_QUEUE_ID);

    port_id = (port == CA_PORT_ID_NI7 ? CA_PORT_ID(CA_PORT_TYPE_L3, port) : port_id);

    port_type = PORT_TYPE(port_id);

	RT_PARAM_CHK((port_type != CA_PORT_TYPE_ETHERNET && \
        port_type != CA_PORT_TYPE_CPU && port_type != CA_PORT_TYPE_L3), RT_ERR_FAILED);

	map_port = PORT_ID(port_id);

	RT_PARAM_CHK((port_type == CA_PORT_TYPE_ETHERNET && \
        map_port > CA_AAL_MAX_PORT_ID && (map_port != CA_PORT_ID_L3_WAN && \
        map_port != CA_PORT_ID_L3_LAN)), RT_ERR_FAILED);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_CPU && \
        (map_port < CA_PORT_ID_CPU0 || map_port > CA_PORT_ID_CPU7)), RT_ERR_FAILED);

    RT_PARAM_CHK((port_type == CA_PORT_TYPE_L3 && \
        map_port != CA_PORT_ID_NI7), RT_ERR_FAILED);

	if(rate >= RTL9607F_EGRESS_RATE_MAX)
    {
        enable = 0;
        egr_rate = RTL9607F_EGRESS_RATE_MAX;
    }
    else
    {
        enable = 1;
        egr_rate = rate;
    }

    pps = 0; /* True - PPS mode, false - BPS mode */
    burst_size = (rate/500 + 511) >> 9;
    burst_size = (burst_size == 0) ? 1: burst_size;
    /*The burst size granularity is 256 bytes or 1 packet.
    The minimum value is roundup (committed_rate_m/512) in byte mode
    and roudup (committed_rate_m/2) in packet mode.*/

    memset((void *)&profile, 0, sizeof(profile));
    memset((void *)&profile_msk, 0, sizeof(profile_msk));

    /* configure L2 shaper */
    profile_msk.s.bs     = 1;
    profile_msk.s.rate_k = 1;
    profile_msk.s.rate_m = 1;
    profile_msk.s.mode   = 1;
    profile_msk.s.state  = 1;

    /* VoQ shaper is based on 500kbps */
    profile.bs     = burst_size;
    profile.rate_k = egr_rate % 500;
    profile.rate_m = egr_rate / 500;
    profile.mode   = pps ? CA_AAL_SHAPER_MODE_PPS : CA_AAL_SHAPER_MODE_BPS;
    profile.state  = enable ? CA_AAL_SHAPER_ADMIN_STATE_SHAPER : CA_AAL_SHAPER_ADMIN_STATE_FORWARD;

    if (profile.state == CA_AAL_SHAPER_ADMIN_STATE_SHAPER)
    {
        /* check burst size, minimum value is dependent on rate_m */
		RT_PARAM_CHK((profile.mode == CA_AAL_SHAPER_MODE_BPS && \
			profile.bs < ((profile.rate_m + 511) >> 9)), RT_ERR_FAILED);

		RT_PARAM_CHK((profile.mode == CA_AAL_SHAPER_MODE_PPS && \
			profile.bs < ((profile.rate_m + 1) >> 2)), RT_ERR_FAILED);
    }

    /* configure L3 shaper */
    memset((void *)&l3_profile, 0, sizeof(l3_profile));
    memset((void *)&l3_profile_msk, 0, sizeof(l3_profile_msk));

    l3_profile_msk.s.bs     = 1;
    l3_profile_msk.s.rate_k = 1;
    l3_profile_msk.s.rate_m = 1;
    l3_profile_msk.s.mode   = 1;
    l3_profile_msk.s.state  = 1;

    /* VoQ shaper is based on 500kbps */
    l3_profile.bs     = burst_size;
    l3_profile.rate_k = egr_rate % 500;
    l3_profile.rate_m = egr_rate / 500;
    l3_profile.mode   = pps ? CA_AAL_L3_SHAPER_MODE_PPS : CA_AAL_L3_SHAPER_MODE_BPS;
    l3_profile.state  = enable ? CA_AAL_L3_SHAPER_ADMIN_STATE_SHAPER : CA_AAL_L3_SHAPER_ADMIN_STATE_FORWARD;


    switch (port_type)
    {
    case CA_PORT_TYPE_ETHERNET:
        /* convert LDPID to TM LDPID */
        if (map_port == CA_PORT_ID_L3_WAN)
            map_port = CA_AAL_L2TM_DPID_L3_WAN;
        else if (map_port == CA_PORT_ID_L3_LAN)
            map_port = CA_AAL_L2TM_DPID_L3_LAN;

        voq_id = map_port * 8 + queue;
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_voq_tbc_set(0, voq_id, profile_msk, &profile), ret);
        break;

    case CA_PORT_TYPE_CPU:
        map_port -= CA_PORT_ID_CPU0;
        voq_id = map_port * 8 + queue;
		RT_ERR_CHK((rt_error_common_t)aal_l3_te_shaper_voq_tbc_set(0, voq_id, l3_profile_msk, &l3_profile), ret);
        break;

    case CA_PORT_TYPE_L3:
        /* configure L3 shaper for WAN port */
        voq_id = map_port * 8 + queue;
        RT_ERR_CHK((rt_error_common_t)aal_l2_te_shaper_voq_tbc_set(0, voq_id, profile_msk, &profile), ret);
        break;

    default:
        break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_queueEgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_rtl9607f_rate_stormControlRate_get
 * Description:
 *      Get the storm control meter index.
 * Input:
 *      port       - port id
 *      stormType  - storm group type
 * Output:
 *      pIndex     - storm control meter index.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_ENTRY_NOTFOUND    - The global strom group is not enable for this group
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 *    - Before call this API must make sure the global strom gruop for given group is enabled,
 *      otherwise this API will return RT_ERR_ENTRY_NOTFOUND
 */
int32
dal_rtl9607f_rate_stormControlMeterIdx_get(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    uint32                  *pIndex)
{
    int32 ret = RT_ERR_OK;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            *pIndex = stormCfg[port].uuc_flow_id;
            break;
        case STORM_GROUP_BROADCAST:
            *pIndex = stormCfg[port].bc_flow_id;
            break;
        case STORM_GROUP_DHCP:
            *pIndex = stormCfg[port].dhcp_flow_id;
            break;
        case STORM_GROUP_ARP:
            *pIndex = stormCfg[port].arp_flow_id;
            break;
        case STORM_GROUP_IGMP_MLD:
            *pIndex = stormCfg[port].igmp_mld_flow_id;
            break;
        case STORM_GROUP_ETHERTYPE:
            *pIndex = stormCfg[port].ethertype_flow_id;
            break;
        case STORM_GROUP_LOOP_DETECT:
            *pIndex = stormCfg[port].loopdetect_flow_id;
            break;
        case STORM_GROUP_DOT1X:
            *pIndex = stormCfg[port].dot1x_flow_id;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_stormControlMeterIdx_get */

/* Function Name:
 *      dal_rtl9607f_rate_stormControlMeterIdx_set
 * Description:
 *      Set the storm control meter index.
 * Input:
 *      port       - port id
 *      storm_type - storm group type
 *      index       - storm control meter index.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_ENTRY_NOTFOUND    - The global strom group is not enable for this group
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE    - Invalid input bandwidth
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 *    - Before call this API must make sure the global strom gruop for given group is enabled,
 *      otherwise this API will return RT_ERR_ENTRY_NOTFOUND
 */
int32
dal_rtl9607f_rate_stormControlMeterIdx_set(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    uint32                  index)
{
    int32 ret = RT_ERR_OK;
    rtk_port_t  p;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_FILTER_METER_ID);

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            if(stormCfg[port].uuc_flow_id != index && stormCfg[port].uuc_enable == ENABLED)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_l2_flood_set(port,stormType,stormCfg[port].uuc_enable,index), ret);
            }

            stormCfg[port].uuc_flow_id = index;
            break;
        case STORM_GROUP_BROADCAST:
            if(stormCfg[port].bc_flow_id != index && stormCfg[port].bc_enable == ENABLED)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_l2_flood_set(port,stormType,stormCfg[port].bc_enable,index), ret) ;
            }

            stormCfg[port].bc_flow_id = index;
            break;
        case STORM_GROUP_DHCP:
            if (stormCfg[port].dhcp_flow_id != index && stormCfg[port].dhcp_enable == ENABLED)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(stormType, index), ret);
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, stormCfg[port].dhcp_enable), ret);
            }
            stormCfg[port].dhcp_flow_id = index;

            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (port == p)
                    continue;

                stormCfg[p].dhcp_flow_id = index;
            }
            break;
        case STORM_GROUP_ARP:
            if (stormCfg[port].arp_flow_id != index && stormCfg[port].arp_enable == ENABLED)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(stormType, index), ret);
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, stormCfg[port].arp_enable), ret);
            }
            stormCfg[port].arp_flow_id = index;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (port == p)
                    continue;

                stormCfg[p].arp_flow_id = index;
            }
            break;
        case STORM_GROUP_IGMP_MLD:
            if (stormCfg[port].igmp_mld_flow_id != index && stormCfg[port].igmp_mld_enable == ENABLED)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(stormType, index), ret);
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, stormCfg[port].igmp_mld_enable), ret);
            }
            stormCfg[port].igmp_mld_flow_id = index;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (port == p)
                    continue;

                stormCfg[p].igmp_mld_flow_id = index;
            }
            break;
        case STORM_GROUP_ETHERTYPE:
            if (stormCfg[port].ethertype_flow_id != index && stormCfg[port].ethertype_enable == ENABLED)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(stormType, index), ret);
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, stormCfg[port].ethertype_enable), ret);
            }
            stormCfg[port].ethertype_flow_id = index;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (port == p)
                    continue;

                stormCfg[p].ethertype_flow_id = index;
            }
            break;
        case STORM_GROUP_LOOP_DETECT:
            if (stormCfg[port].loopdetect_flow_id != index && stormCfg[port].loopdetect_enable == ENABLED)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(stormType, index), ret);
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, stormCfg[port].loopdetect_enable), ret);
            }
            stormCfg[port].loopdetect_flow_id = index;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (port == p)
                    continue;

                stormCfg[p].loopdetect_flow_id = index;
            }
            break;
        case STORM_GROUP_DOT1X:
            if (stormCfg[port].dot1x_flow_id != index && stormCfg[port].dot1x_enable == ENABLED)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(stormType, index), ret);
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, stormCfg[port].dot1x_enable), ret);
            }
            stormCfg[port].dot1x_flow_id = index;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (port == p)
                    continue;

                stormCfg[p].dot1x_flow_id = index;
            }
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_stormControlMeterIdx_set */

/* Function Name:
 *      dal_rtl9607f_rate_stormControlPortEnable_get
 * Description:
 *      Get enable status of storm control on specified port.
 * Input:
 *      port       - port id
 *      stormType  - storm group type
 * Output:
 *      pEnable    - pointer to enable status of storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 */
int32
dal_rtl9607f_rate_stormControlPortEnable_get(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    rtk_enable_t            *pEnable)
{
    int32 ret = RT_ERR_OK;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((STORM_GROUP_END <= stormType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            *pEnable = stormCfg[port].uuc_enable; /*Per system setting*/
            break;
        case STORM_GROUP_BROADCAST:
            *pEnable = stormCfg[port].bc_enable;
            break;
        case STORM_GROUP_DHCP:
            *pEnable = stormCfg[port].dhcp_enable;
            break;
        case STORM_GROUP_ARP:
            *pEnable = stormCfg[port].arp_enable;
            break;
        case STORM_GROUP_IGMP_MLD:
            *pEnable = stormCfg[port].igmp_mld_enable;
            break;
        case STORM_GROUP_ETHERTYPE:
            *pEnable = stormCfg[port].ethertype_enable;
            break;
        case STORM_GROUP_LOOP_DETECT:
            *pEnable = stormCfg[port].loopdetect_enable;
            break;
        case STORM_GROUP_DOT1X:
            *pEnable = stormCfg[port].dot1x_enable;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_stormControlPortEnable_get */

/* Function Name:
 *      dal_rtl9607f_rate_stormControlPortEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 * Input:
 *      port       - port id
 *      stormType  - storm group type
 *      enable     - enable status of storm control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_ENTRY_NOTFOUND    - The global strom group is not enable for this group
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 */
int32
dal_rtl9607f_rate_stormControlPortEnable_set(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    rtk_enable_t            enable)
{
    int32 ret = RT_ERR_OK;
    rtk_port_t p;
    rtk_enable_t flag;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((STORM_GROUP_END <= stormType), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            if(stormCfg[port].uuc_enable != enable)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_l2_flood_set(port,stormType,enable,stormCfg[port].uuc_flow_id), ret);
            }
            stormCfg[port].uuc_enable = enable;
            break;
        case STORM_GROUP_BROADCAST:
            if(stormCfg[port].bc_enable != enable)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_l2_flood_set(port,stormType,enable,stormCfg[port].bc_flow_id), ret) ;
            }
            stormCfg[port].bc_enable = enable;
            break;
        case STORM_GROUP_DHCP: /* only support global configuration of policer agr flow  */
            if (stormCfg[port].dhcp_enable != enable)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, enable), ret);
            }
            stormCfg[port].dhcp_enable = enable;

            /* update policer agr flow type if all port disable storm */
            flag = DISABLED;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (stormCfg[p].dhcp_enable)
                    flag = ENABLED;
            }

            meterEntry[stormCfg[port].dhcp_flow_id].mode = flag ? CA_AAL_POLICER_TYPE_RFC2697:  CA_AAL_POLICER_TYPE_NONE;

            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_DHCP, stormCfg[port].dhcp_flow_id), ret);

            break;
        case STORM_GROUP_ARP: /* only support global configuration of policer agr flow */
            if (stormCfg[port].arp_enable != enable)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, enable), ret);
            }
            stormCfg[port].arp_enable = enable;

            /* update policer agr flow type if all port disable storm */
            flag = DISABLED;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (stormCfg[p].arp_enable)
                    flag = ENABLED;
            }

            meterEntry[stormCfg[port].arp_flow_id].mode = flag ? CA_AAL_POLICER_TYPE_RFC2697 :  CA_AAL_POLICER_TYPE_NONE;

            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_ARP, stormCfg[port].arp_flow_id), ret);

            break;
        case STORM_GROUP_IGMP_MLD: /* only support global configuration of policer agr flow */
            if (stormCfg[port].igmp_mld_enable != enable)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, enable), ret);
            }
            stormCfg[port].igmp_mld_enable = enable;

            /* update policer agr flow type if all port disable storm */
            flag = DISABLED;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (stormCfg[p].igmp_mld_enable)
                    flag = ENABLED;
            }

            meterEntry[stormCfg[port].igmp_mld_flow_id].mode = flag ? CA_AAL_POLICER_TYPE_RFC2697 : CA_AAL_POLICER_TYPE_NONE;

            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_IGMP_MLD, stormCfg[port].igmp_mld_flow_id), ret);

            break;
        case STORM_GROUP_ETHERTYPE: /* only support global configuration of policer agr flow */
            if (stormCfg[port].ethertype_enable != enable)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, enable), ret);
            }
            stormCfg[port].ethertype_enable = enable;

            /* update policer agr flow type if all port disable storm */
            flag = DISABLED;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (stormCfg[p].ethertype_enable)
                    flag = ENABLED;
            }

            meterEntry[stormCfg[port].ethertype_flow_id].mode = flag ? CA_AAL_POLICER_TYPE_RFC2697 :  CA_AAL_POLICER_TYPE_NONE;

            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_ETHERTYPE, stormCfg[port].ethertype_flow_id), ret);

            break;
        case STORM_GROUP_LOOP_DETECT: /* only support global configuration of policer agr flow */
            if (stormCfg[port].loopdetect_enable != enable)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, enable), ret);
            }
            stormCfg[port].loopdetect_enable = enable;

            /* update policer agr flow type if all port disable storm */
            flag = DISABLED;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (stormCfg[p].loopdetect_enable)
                    flag = ENABLED;
            }

            meterEntry[stormCfg[port].loopdetect_flow_id].mode = flag ? CA_AAL_POLICER_TYPE_RFC2697 :  CA_AAL_POLICER_TYPE_NONE;

            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_LOOP_DETECT, stormCfg[port].loopdetect_flow_id), ret);

            break;
        case STORM_GROUP_DOT1X: /* only support global configuration of policer agr flow */
            if (stormCfg[port].dot1x_enable != enable)
            {
                RT_ERR_CHK(_dal_rtl9607f_use_ca_specpkt_ctrl_set(port, stormType, enable), ret);
            }
            stormCfg[port].dot1x_enable = enable;

            /* update policer agr flow type if all port disable storm */
            flag = DISABLED;
            HAL_SCAN_ALL_ETH_PORT(p)
            {
                if (HAL_IS_XE_PORT(p) && p != HAL_GET_PON_PORT())
                    continue;

                if (stormCfg[p].dot1x_enable)
                    flag = ENABLED;
            }

            meterEntry[stormCfg[port].dot1x_flow_id].mode = flag ? CA_AAL_POLICER_TYPE_RFC2697 :  CA_AAL_POLICER_TYPE_NONE;

            RT_ERR_CHK(_dal_rtl9607f_use_ca_agr_flow_set(STORM_GROUP_DOT1X, stormCfg[port].dot1x_flow_id), ret);

            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_stormControlPortEnable_set */

/* Function Name:
 *      dal_rtl9607f_rate_shareMeter_set
 * Description:
 *      Set meter configuration
 * Input:
 *      index       - shared meter index
 *      rate        - rate of share meter
 *      ifgInclude  - include IFG or not, ENABLE:include DISABLE:exclude
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE             - Invalid rate
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *      The API can set shared meter rate and ifg include for each meter.
 *      The rate unit is 1 kbps and the range is from 8k to 1048568k.
 *      The granularity of rate is 8 kbps. The ifg_include parameter is used
 *      for rate calculation with/without inter-frame-gap and preamble.
 */
int32
dal_rtl9607f_rate_shareMeter_set (
    uint32 index,
    uint32 rate,
    rtk_enable_t ifgInclude )
{

    int32 ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_RATE), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK(((RTL9607F_METER_RATE_MAX) < rate), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= ifgInclude), RT_ERR_INPUT);

    if(rate >= RTL9607F_METER_RATE_MAX) //Disable
    {
        meterEntry[index].mode = CA_AAL_POLICER_TYPE_NONE;
        meterEntry[index].pps = 0;
        meterEntry[index].cir = 0;
        meterEntry[index].pir = 0;
        meterEntry[index].cbs = RTL9607F_METER_BUCKETSIZE_MAX;
        meterEntry[index].pbs = RTL9607F_METER_BUCKETSIZE_MAX;
    }
    else
    {
        // get original meter cfg
        RT_ERR_CHK(_dal_rtl9607f_meter_cfg_get(index, &(meterEntry[index])), ret);
        // update rate to cir, pir and mode
        meterEntry[index].mode = (meterEntry[index].mode == CA_AAL_POLICER_TYPE_NONE ? CA_AAL_POLICER_TYPE_RFC2697 : meterEntry[index].mode);
        meterEntry[index].cir = rate;
        meterEntry[index].pir = rate;
    }

    if(ifgInclude == DISABLED)
    {
        meterEntry[index].ipg = 0;
    }
    else
    {
        meterEntry[index].ipg = 20;
    }

    RT_ERR_CHK(_dal_rtl9607f_check_meter_change_set(index), ret);

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_shareMeter_set */


/* Function Name:
 *      dal_rtl9607f_rate_shareMeter_get
 * Description:
 *      Get meter configuration
 * Input:
 *      index        - shared meter index
 * Output:
 *      pRate        - pointer of rate of share meter
 *      pIfgInclude  - include IFG or not, ENABLE:include DISABLE:exclude
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter rate and ifg include for each meter.
 *      The rate unit is 1 kbps and the granularity of rate is 8 kbps.
 *      The ifg_include parameter is used for rate calculation with/without inter-frame-gap and preamble
 */
int32
dal_rtl9607f_rate_shareMeter_get (
    uint32  index,
    uint32* pRate,
    rtk_enable_t* pIfgInclude )
{
    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_RATE), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);

    *pRate = ((CA_AAL_POLICER_TYPE_NONE == meterEntry[index].mode) ? RTL9607F_METER_RATE_MAX : meterEntry[index].cir);
    *pIfgInclude = ((meterEntry[index].ipg == 0) ? DISABLED :  ENABLED);
    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_shareMeter_get */

/* Function Name:
 *      dal_rtl9607f_rate_shareMeterMode_set
 * Description:
 *      Set meter mode
 * Input:
 *      index     - shared meter index
 *      meterMode - meter mode(bit rate mode or packet rate mode)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can set shared meter type.
 */
int32
dal_rtl9607f_rate_shareMeterMode_set (
    uint32 index,
    rtk_rate_metet_mode_t meterMode )
{
    int32 ret = RT_ERR_OK;

    RT_DBG(
        LOG_DEBUG,
        (MOD_DAL | MOD_RATE),
        "index=%d,meterMode=%d",
        index,
        meterMode);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((METER_MODE_END <= meterMode), RT_ERR_INPUT);

    meterEntry[index].pps = meterMode;

    RT_ERR_CHK(_dal_rtl9607f_check_meter_change_set(index), ret);


    return RT_ERR_OK;
}   /* end of dal_rtl9607f_rate_shareMeterMode_set */

/* Function Name:
 *      dal_rtl9607f_rate_shareMeterMode_get
 * Description:
 *      Set meter mode
 * Input:
 *      index     - shared meter index
 *      pMeterMode     - meter mode(bit rate mode or packet rate mode)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter mode.
 */
int32
dal_rtl9607f_rate_shareMeterMode_get (
    uint32 index,
    rtk_rate_metet_mode_t* pMeterMode)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_RATE), "index=%d", index);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMeterMode), RT_ERR_NULL_POINTER);

    /* True - PPS mode, false - BPS mode */
   *pMeterMode = ((meterEntry[index].pps == 1) ?  METER_MODE_PACKET_RATE : METER_MODE_BIT_RATE); /* True - PPS mode, false - BPS mode */

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_rate_shareMeterMode_get */

/* Function Name:
 *      dal_rtl9607f_rate_shareMeterBucket_set
 * Description:
 *      Set meter Bucket Size
 * Input:
 *      index        - shared meter index
 *      bucketSize   - Bucket Size
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can set shared meter bucket size.
 */
int32
dal_rtl9607f_rate_shareMeterBucket_set (
    uint32 index,
    uint32 bucketSize )
{
    int32 ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_RATE), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9607F_METER_BUCKETSIZE_MAX < bucketSize), RT_ERR_INPUT);

    meterEntry[index].cbs = bucketSize;
    meterEntry[index].pbs = bucketSize;

    RT_ERR_CHK(_dal_rtl9607f_check_meter_change_set(index), ret);


    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_shareMeterBucket_set */


/* Function Name:
 *      dal_rtl9607f_rate_shareMeterBucket_get
 * Description:
 *      Get meter Bucket Size
 * Input:
 *      index        - shared meter index
 * Output:
 *      pBucketSize - Bucket Size
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter bucket size.
 */
int32
dal_rtl9607f_rate_shareMeterBucket_get (
    uint32 index,
    uint32* pBucketSize )
{
    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_RATE), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBucketSize), RT_ERR_NULL_POINTER);

    *pBucketSize = meterEntry[index].cbs;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_rate_shareMeterBucket_get */

#if defined(CONFIG_SDK_RTL8198X)
/* Function Name:
 *      dal_rtl9607f_ca_specpkt_ctrl_ldpid_update
 * Description:
 *      update ca special ctrl ldpid
 * Input:
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      others		           	- Failed
 * Note:
 *      The API can update ca special ctrl ldpid.
 */
int32 dal_rtl9607f_ca_specpkt_ctrl_ldpid_update (rtk_port_t port)
{
	rt_error_common_t ret = RT_ERR_OK;
	aal_ilpb_cfg_t ilpb_cfg;
	aal_l2_specpkt_type_t  specpkt_type;
	aal_l2_specpkt_ctrl_t  config;
	aal_l2_specpkt_ctrl_mask_t  _mask;

	if (stormCfg[port].igmp_mld_enable || stormCfg[port].arp_enable || stormCfg[port].dhcp_enable) {
		memset(&ilpb_cfg, 0x00, sizeof(aal_ilpb_cfg_t));
		RT_ERR_CHK((rt_error_common_t)aal_port_ilpb_cfg_get(0, port, &ilpb_cfg), ret);
		memset(&_mask, 0xff, sizeof(aal_l2_specpkt_ctrl_mask_t));

		for (specpkt_type = AAL_L2_SPECPKT_TYPE_IGMP_MLD; specpkt_type <= AAL_L2_SPECPKT_TYPE_DHCP; specpkt_type++) {
			switch (specpkt_type) {
			case AAL_L2_SPECPKT_TYPE_IGMP_MLD:
				if (!stormCfg[port].igmp_mld_enable)
					continue;
				break;
			case AAL_L2_SPECPKT_TYPE_ARP:
				if (!stormCfg[port].arp_enable)
					continue;
				break;
			case AAL_L2_SPECPKT_TYPE_DHCP:
				if (!stormCfg[port].dhcp_enable)
					continue;
				break;
			}

			memset(&config,0x00,sizeof(aal_l2_specpkt_ctrl_t));
			RT_ERR_CHK((rt_error_common_t)aal_l2_specpkt_ctrl_get(0, ilpb_cfg.spcl_pkt_idx, specpkt_type, &config), ret);

			if (config.ldpid == CA_PORT_ID_L3_WAN && ilpb_cfg.cf_sel_wan_en) {
				//printk("specpkt_type 0x%x, no need to update, wan port %d\n", specpkt_type, port);
				continue;
			}
			else if (config.ldpid == CA_PORT_ID_L3_LAN && !ilpb_cfg.cf_sel_wan_en) {
				//printk("specpkt_type 0x%x, no need to update, lan port %d\n", specpkt_type, port);
				continue;
			}

			if (ilpb_cfg.cf_sel_wan_en) {
				config.ldpid = CA_PORT_ID_L3_WAN;
			}
			else {
				config.ldpid = CA_PORT_ID_L3_LAN;
			}

			RT_ERR_CHK((rt_error_common_t)aal_l2_specpkt_ctrl_set(0, ilpb_cfg.spcl_pkt_idx, specpkt_type, _mask, &config), ret);
			//printk("specpkt_type 0x%x, update to %s port %d\n", specpkt_type, (ilpb_cfg.cf_sel_wan_en ? "wan" : "lan"), port);
		}
	}

	return RT_ERR_OK;
} /* end of dal_rtl9607f_ca_specpkt_ctrl_ldpid_update */
#endif
