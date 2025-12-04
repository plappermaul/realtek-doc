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
#include <hal/chipdef/ca8279/ca8279_def.h>
#include <dal/ca8279/dal_ca8279.h>
#include <dal/ca8279/dal_ca8279_rate.h>
#include <osal/time.h>

#include <cortina-api/include/rate.h>
#include <cortina-api/include/l2.h>
#include <cortina-api/include/classifier.h>
#if defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
#include <cortina-api/include/port.h>
#endif

#include <aal_l2_tm.h>
#include <aal_l3_tm.h>
#include <aal_port.h>
#include <aal_l2_te.h>
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
    ca_uint16_t uuc_flow_id;
    ca_uint16_t umc_flow_id;
    ca_uint16_t bc_flow_id;
    ca_uint16_t mc_flow_id;
    ca_uint16_t dhcp_flow_id;
    ca_uint16_t arp_flow_id;
    ca_uint16_t igmp_mld_flow_id;
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
} rate_storm_cfg_t;

rate_storm_cfg_t stormCfg[RTK_MAX_NUM_OF_PORTS];

typedef struct portQueueMeterCfg_s
{
    uint8 enable;
    ca_uint32_t meter_index;
} portQueueMeterCfg_t;

/*
 * Data Declaration
 */
static uint32 rate_init = {INIT_NOT_COMPLETED};

static portQueueMeterCfg_t portQMCfg[RTK_MAX_NUM_OF_PORTS][RTK_MAX_NUM_OF_QUEUE];

uint32 storm_prio = 5;

/*
 * Function Declaration
 */

int32 _dal_ca8279_use_ca_l2_flood_set(rtk_port_t port,rtk_rate_storm_group_t stormType,rtk_enable_t enable,ca_uint16_t flow_id)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_l2_flooding_type_t ptype;
    ca_uint32_t rate;
    ca_policer_t policer;
    ca_boolean_t pps=0;

    port_id = RTK2CA_PORT_ID(port);

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_MULTICAST:
            ptype = CA_L2_FLOODING_TYPE_MC; //Cortina packet type MC is unknown multicast
            break;
        case STORM_GROUP_BROADCAST:
            ptype = CA_L2_FLOODING_TYPE_BC;
            break;
        default:
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_INPUT;
    }

    if(enable == DISABLED)
        rate = CA_UINT32_INVALID;
    else
    {
        if((ret = ca_l2_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        rate = policer.cir;
    }

    if((ret = ca_l2_flooding_rate_set(0,port_id,ptype,pps,&rate)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

int32 _dal_ca8279_bc_storm_classification_add(rtk_port_t port,ca_uint16_t flow_id)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;

    port_id = RTK2CA_PORT_ID(port);

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    key.src_port = port_id;
    key_mask.src_port = 1;

    if(HAL_IS_PON_PORT(port))
    {
        key.key_handle.flow_id = CA_UINT16_INVALID;
        key.key_handle.gem_index = CA_UINT16_INVALID;
        key.key_handle.llid_cos_index = CA_UINT16_INVALID;
        key_mask.key_handle = TRUE;

        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_WAN;
    }
    else
    {
#if defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
		action.forward = CA_CLASSIFIER_FORWARD_PORT;
		action.dest.port = G3_LOOPBACK_P_NEWSPA;
		action.options.masks.outer_vlan_act = 1;
		action.options.masks.outer_tpid = 1;
		action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_PUSH;
		action.options.outer_vid = G3_LOOPBACK_UPSTREAM_VID(port);
#else		
        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_LAN;
#endif		
    }

    key.l2.mac_da.mac_max[0] = 0xff;
    key.l2.mac_da.mac_max[1] = 0xff;
    key.l2.mac_da.mac_max[2] = 0xff;
    key.l2.mac_da.mac_max[3] = 0xff;
    key.l2.mac_da.mac_max[4] = 0xff;
    key.l2.mac_da.mac_max[5] = 0xff;

    key.l2.mac_da.mac_min[0] = 0xff;
    key.l2.mac_da.mac_min[1] = 0xff;
    key.l2.mac_da.mac_min[2] = 0xff;
    key.l2.mac_da.mac_min[3] = 0xff;
    key.l2.mac_da.mac_min[4] = 0xff;
    key.l2.mac_da.mac_min[5] = 0xff;

    key_mask.l2 = 1;
    key_mask.l2_mask.mac_da = 1;

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,storm_prio,&key,&key_mask,&action,&stormCfg[port].bc_storm_index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_bc_storm_classification_delete(rtk_port_t port)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].bc_storm_index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_mc_storm_classification_add(rtk_port_t port,ca_uint16_t flow_id)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;

    port_id = RTK2CA_PORT_ID(port);

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    key.src_port = port_id;
    key_mask.src_port = 1;

    if(HAL_IS_PON_PORT(port))
    {
        key.key_handle.flow_id = CA_UINT16_INVALID;
        key.key_handle.gem_index = CA_UINT16_INVALID;
        key.key_handle.llid_cos_index = CA_UINT16_INVALID;
        key_mask.key_handle = TRUE;

        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_WAN;
    }
    else
    {
#if defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
		action.forward = CA_CLASSIFIER_FORWARD_PORT;
		action.dest.port = G3_LOOPBACK_P_NEWSPA;
		action.options.masks.outer_vlan_act = 1;
		action.options.masks.outer_tpid = 1;
		action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_PUSH;
		action.options.outer_vid = G3_LOOPBACK_UPSTREAM_VID(port);
#else		
        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_LAN;
#endif		
    }

    key.l2.mac_da.mac_max[0] = 0x01;
    key.l2.mac_da.mac_max[1] = 0xff;
    key.l2.mac_da.mac_max[2] = 0xff;
    key.l2.mac_da.mac_max[3] = 0xff;
    key.l2.mac_da.mac_max[4] = 0xff;
    key.l2.mac_da.mac_max[5] = 0xff;

    key.l2.mac_da.mac_min[0] = 0x01;
    key.l2.mac_da.mac_min[1] = 0x00;
    key.l2.mac_da.mac_min[2] = 0x00;
    key.l2.mac_da.mac_min[3] = 0x00;
    key.l2.mac_da.mac_min[4] = 0x00;
    key.l2.mac_da.mac_min[5] = 0x00;

    key_mask.l2 = 1;
    key_mask.l2_mask.mac_da = 1;

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,storm_prio,&key,&key_mask,&action,&stormCfg[port].mc_storm_index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_mc_storm_classification_delete(rtk_port_t port)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].mc_storm_index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_dhcp_storm_classification_add(rtk_port_t port,ca_uint16_t flow_id)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;

    port_id = RTK2CA_PORT_ID(port);

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    key.src_port = port_id;
    key_mask.src_port = 1;

    if(HAL_IS_PON_PORT(port))
    {
        key.key_handle.flow_id = CA_UINT16_INVALID;
        key.key_handle.gem_index = CA_UINT16_INVALID;
        key.key_handle.llid_cos_index = CA_UINT16_INVALID;
        key_mask.key_handle = TRUE;

        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_WAN;
    }
    else
    {
#if defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
		action.forward = CA_CLASSIFIER_FORWARD_PORT;
		action.dest.port = G3_LOOPBACK_P_NEWSPA;
		action.options.masks.outer_vlan_act = 1;
		action.options.masks.outer_tpid = 1;
		action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_PUSH;
		action.options.outer_vid = G3_LOOPBACK_UPSTREAM_VID(port);
#else		
        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_LAN;
#endif		
    }

    key.ip.ip_valid = 1;
    key.ip.ip_protocol = 17; //UDP
    key_mask.ip = 1;
//    key_mask.ip_mask.ip_valid = 1;
    key_mask.ip_mask.ip_protocol = 1;

    key.l4.l4_valid = 1;
    key.l4.src_port.min = 67;
    key.l4.src_port.max = 67;
    key.l4.dst_port.min = 68;
    key.l4.dst_port.max = 68;

    key_mask.l4 = 1;
//    key_mask.l4_mask.l4_valid = 1;
    key_mask.l4_mask.src_port = 1;
    key_mask.l4_mask.dst_port = 1;

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,storm_prio,&key,&key_mask,&action,&stormCfg[port].dhcp_storm_index0)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    key.l4.src_port.min = 68;
    key.l4.src_port.max = 68;
    key.l4.dst_port.min = 67;
    key.l4.dst_port.max = 67;

    if((ret = ca_classifier_rule_add(0,storm_prio,&key,&key_mask,&action,&stormCfg[port].dhcp_storm_index1)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_dhcp_storm_classification_delete(rtk_port_t port)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].dhcp_storm_index0)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].dhcp_storm_index1)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_arp_storm_classification_add(rtk_port_t port,ca_uint16_t flow_id)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;

    port_id = RTK2CA_PORT_ID(port);

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    key.src_port = port_id;
    key_mask.src_port = 1;

    if(HAL_IS_PON_PORT(port))
    {
        key.key_handle.flow_id = CA_UINT16_INVALID;
        key.key_handle.gem_index = CA_UINT16_INVALID;
        key.key_handle.llid_cos_index = CA_UINT16_INVALID;
        key_mask.key_handle = TRUE;

        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_WAN;
    }
    else
    {
#if defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
		action.forward = CA_CLASSIFIER_FORWARD_PORT;
		action.dest.port = G3_LOOPBACK_P_NEWSPA;
		action.options.masks.outer_vlan_act = 1;
		action.options.masks.outer_tpid = 1;
		action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_PUSH;
		action.options.outer_vid = G3_LOOPBACK_UPSTREAM_VID(port);
#else		
        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_LAN;
#endif		
    }

    key.l2.ethertype = 0x0806; // ARP
    key_mask.l2 = 1;
    key_mask.l2_mask.ethertype = 1;

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,storm_prio,&key,&key_mask,&action,&stormCfg[port].arp_storm_index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_arp_storm_classification_delete(rtk_port_t port)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].arp_storm_index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_igmp_mld_storm_classification_add(rtk_port_t port,ca_uint16_t flow_id)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;

    port_id = RTK2CA_PORT_ID(port);

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    key.src_port = port_id;
    key_mask.src_port = 1;

    if(HAL_IS_PON_PORT(port))
    {
        key.key_handle.flow_id = CA_UINT16_INVALID;
        key.key_handle.gem_index = CA_UINT16_INVALID;
        key.key_handle.llid_cos_index = CA_UINT16_INVALID;
        key_mask.key_handle = TRUE;

        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_WAN;
    }
    else
    {
#if defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
		action.forward = CA_CLASSIFIER_FORWARD_PORT;
		action.dest.port = G3_LOOPBACK_P_NEWSPA;
		action.options.masks.outer_vlan_act = 1;
		action.options.masks.outer_tpid = 1;
		action.options.outer_vlan_act = CA_CLASSIFIER_VLAN_ACTION_PUSH;
		action.options.outer_vid = G3_LOOPBACK_UPSTREAM_VID(port);
#else		
        action.forward = CA_CLASSIFIER_FORWARD_PORT;
        action.dest.port = AAL_LPORT_L3_LAN;
#endif		
    }

    key.ip.ip_valid = 1;
    key.ip.ip_version = 4; //IPv4
    key.ip.ip_protocol = 2; //IGMP
    key_mask.ip = 1;
//    key_mask.ip_mask.ip_valid = 1;
    key_mask.ip_mask.ip_version = 1;
    key_mask.ip_mask.ip_protocol = 1;

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,storm_prio,&key,&key_mask,&action,&stormCfg[port].igmp_mld_storm_index0)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    key.ip.ip_version = 6; //IPv6
    key.ip.ip_protocol = 58; //ICMPv6
    key.ip.icmp_type = 130; //MLD query
    key_mask.ip_mask.icmp_type = 1;

    if((ret = ca_classifier_rule_add(0,storm_prio,&key,&key_mask,&action,&stormCfg[port].igmp_mld_storm_index1)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    key.ip.icmp_type = 131; //MLD report
    
    if((ret = ca_classifier_rule_add(0,storm_prio,&key,&key_mask,&action,&stormCfg[port].igmp_mld_storm_index2)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    key.ip.icmp_type = 132; //MLD done
    
    if((ret = ca_classifier_rule_add(0,storm_prio,&key,&key_mask,&action,&stormCfg[port].igmp_mld_storm_index3)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    key.ip.icmp_type = 143; //MLD report v2
    
    if((ret = ca_classifier_rule_add(0,storm_prio,&key,&key_mask,&action,&stormCfg[port].igmp_mld_storm_index4)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_igmp_mld_storm_classification_delete(rtk_port_t port)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].igmp_mld_storm_index0)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].igmp_mld_storm_index1)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].igmp_mld_storm_index2)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].igmp_mld_storm_index3)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].igmp_mld_storm_index4)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_use_ca_queue_shaper_set(rtk_port_t port,rtk_qid_t queue,uint32 meterIndex)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_policer_t policer;
    ca_shaper_t shaper;

    port_id = RTK2CA_PORT_ID(port);

    memset(&policer,0,sizeof(ca_policer_t));
    if((ret = ca_l2_flow_policer_get(0,meterIndex,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    memset(&shaper,0,sizeof(ca_shaper_t));
    if((ret = ca_queue_shaper_get(0,port_id,queue,&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    shaper.pps = policer.pps;
    if(policer.cir >= CA8279_METER_RATE_MAX)
        shaper.rate = CA8279_METER_RATE_MAX;
    else
        shaper.rate = policer.cir;
    shaper.burst_size = policer.cbs;

    if((ret = ca_queue_shaper_set(0,port_id,queue,&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_ca8279_check_meter_change_set(uint32 meterIndex)
{
    int32 ret=RT_ERR_OK;
    uint32  port,queue;

    HAL_SCAN_ALL_PORT(port)
    {
        for(queue=0 ; queue<HAL_MAX_NUM_OF_QUEUE() ; queue++)
        {
            if(portQMCfg[port][queue].meter_index == meterIndex)
            {
                if(portQMCfg[port][queue].enable == ENABLED)
                {
                    if((ret=_dal_ca8279_use_ca_queue_shaper_set(port,queue,meterIndex)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                        return ret;
                    }
                }   
            }
        }
    }
    return RT_ERR_OK;
}

/* Module Name    : Rate                                           */
/* Sub-module Name: Configuration of egress port bandwidth control */

/* Function Name:
 *      dal_ca8279_rate_init
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
dal_ca8279_rate_init(void)
{
    ca_status_t ret=CA_E_OK;
    ca_uint16_t flow_id;
    ca_policer_t policer;
    rtk_port_t port;

    rate_init = INIT_COMPLETED;
    ca_policer_config_t config;
    ca_shaper_config_t shaper_config;

    memset(stormCfg,0,sizeof(rate_storm_cfg_t)*RTK_MAX_NUM_OF_PORTS);

    for(port=0;port<RTK_MAX_NUM_OF_PORTS;port++)
    {
        stormCfg[port].uuc_flow_id =-1;
        stormCfg[port].umc_flow_id =-1;
        stormCfg[port].bc_flow_id =-1;
        stormCfg[port].mc_flow_id =-1;
        stormCfg[port].dhcp_flow_id =-1;
        stormCfg[port].arp_flow_id =-1;
        stormCfg[port].igmp_mld_flow_id =-1;
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
    }

    policer.mode = CA_POLICER_MODE_DISABLE;
    
    for(flow_id=0;flow_id <= CA_AAL_MAX_FLOW_ID&&flow_id <= CA_AAL_L3_MAX_FLOW_ID;flow_id++)
    {
        if(ca_l2_flow_policer_set(0,flow_id,&policer) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        }

        if(ca_l3_flow_policer_set(0,flow_id,&policer) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        }
    }

    config.overhead = 20;
    if((ret = ca_l2_policer_config_set(0,&config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_l3_policer_config_set(0,&config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    shaper_config.overhead = 20;
    if((ret = ca_shaper_config_set(0,&shaper_config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    memset(portQMCfg,0,sizeof(portQMCfg));

    HAL_SCAN_ALL_PORT(port)
    {
        dal_ca8279_rate_portIgrBandwidthCtrlRate_set(port,CA8279_INGRESS_RATE_MAX);
        dal_ca8279_rate_portEgrBandwidthCtrlRate_set(port,CA8279_EGRESS_RATE_MAX);
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_init */

/* Function Name:
 *      dal_ca8279_rate_portIgrBandwidthCtrlRate_get
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
dal_ca8279_rate_portIgrBandwidthCtrlRate_get(rtk_port_t port, uint32 *pRate)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_policer_t policer;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    if((ret = ca_port_policer_get(0,port_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    if(policer.mode == CA_POLICER_MODE_DISABLE || policer.cir >= CA8279_INGRESS_RATE_MAX)
    {
        *pRate = CA8279_INGRESS_RATE_MAX;
    }
    else
    {
        *pRate = policer.cir;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_portIgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_ca8279_rate_portIgrBandwidthCtrlRate_set
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
dal_ca8279_rate_portIgrBandwidthCtrlRate_set(rtk_port_t port, uint32 rate)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_policer_t policer;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);


    if(rate >= CA8279_INGRESS_RATE_MAX)
    {
        policer.mode = CA_POLICER_MODE_DISABLE;
        policer.cir = CA8279_INGRESS_RATE_MAX;
        policer.pir = CA8279_INGRESS_RATE_MAX ;
    }
    else
    {
        policer.mode = CA_POLICER_MODE_SRTCM;
        policer.cir = rate;
        policer.pir = rate;
    }


    policer.pps = 0; /* True - PPS mode, false - BPS mode */
    policer.cbs = (policer.cir/1000 + 511) >> 9;
    policer.cbs = (policer.cbs == 0) ? 1: policer.cbs;
    /*The burst size granularity is 256 bytes or 1 packet. 
    The minimum value is roundup (committed_rate_m/512) in byte mode 
    and roudup (committed_rate_m/2) in packet mode.*/
    policer.pbs = (policer.pir/1000 + 511) >> 9;
    policer.pbs = (policer.pbs == 0) ? 1: policer.pbs;
    
    if((ret = ca_port_policer_set(0,port_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_portIgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_ca8279_rate_portEgrBandwidthCtrlRate_get
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
dal_ca8279_rate_portEgrBandwidthCtrlRate_get(rtk_port_t port, uint32 *pRate)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_shaper_t shaper;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    if(port == CA_PORT_ID_NI7)
    {
        port_id = CA_PORT_ID(CA_PORT_TYPE_L3,port);
    }

    if((ret = ca_port_shaper_get(0,port_id,&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    if(shaper.enable == 0 || shaper.rate >= CA8279_EGRESS_RATE_MAX)
    {
        *pRate = CA8279_EGRESS_RATE_MAX;
    }
    else
    {
        *pRate = shaper.rate;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_portEgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_ca8279_rate_portEgrBandwidthCtrlRate_set
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
dal_ca8279_rate_portEgrBandwidthCtrlRate_set(rtk_port_t port, uint32 rate)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_shaper_t shaper;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    /* parameter check */
    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if(rate >= CA8279_EGRESS_RATE_MAX)
    {
        shaper.enable = 0;
        shaper.rate = CA8279_EGRESS_RATE_MAX;
    }
    else
    {
        shaper.enable = 1;
        shaper.rate = rate;
    }

    shaper.pps = 0; /* True - PPS mode, false - BPS mode */    
    shaper.burst_size = (shaper.rate/1000 + 511) >> 9;
    shaper.burst_size = (shaper.burst_size == 0) ? 1: shaper.burst_size;
    /*The burst size granularity is 256 bytes or 1 packet. 
    The minimum value is roundup (committed_rate_m/512) in byte mode 
    and roudup (committed_rate_m/2) in packet mode.*/

    if(port == CA_PORT_ID_NI7)
    {
        port_id = CA_PORT_ID(CA_PORT_TYPE_L3,port);
    }

    if((ret = ca_port_shaper_set(0,port_id,&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_portEgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_ca8279_rate_egrQueueBwCtrlEnable_get
 * Description:
 *      Get enable status of egress bandwidth control on specified queue.
 * Input:
 *      port    - port id
 *      queue   - queue id
 * Output:
 *      pEnable - Pointer to enable status of egress queue bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_ca8279_rate_egrQueueBwCtrlEnable_get(
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_shaper_t shaper;

     /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    memset(&shaper,0,sizeof(ca_shaper_t));

    if((ret = ca_queue_shaper_get(0,port_id,queue,&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    if(shaper.enable ==1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_egrQueueBwCtrlEnable_get */

/* Function Name:
 *      dal_ca8279_rate_egrQueueBwCtrlEnable_set
 * Description:
 *      Set enable status of egress bandwidth control on specified queue.
 * Input:
 *      port   - port id
 *      queue  - queue id
 *      enable - enable status of egress queue bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_ca8279_rate_egrQueueBwCtrlEnable_set(
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_shaper_t shaper;

     /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);

    memset(&shaper,0,sizeof(ca_shaper_t));

    if((ret=_dal_ca8279_use_ca_queue_shaper_set(port,queue,portQMCfg[port][queue].meter_index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return ret;
    }

    if((ret = ca_queue_shaper_get(0,port_id,queue,&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    if(enable == ENABLED)
        shaper.enable = 1;
    else
        shaper.enable = 0;

    if((ret = ca_queue_shaper_set(0,port_id,queue,&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    portQMCfg[port][queue].enable = enable;

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_egrQueueBwCtrlEnable_set */

/* Function Name:
 *      dal_ca8279_rate_egrQueueBwCtrlMeterIdx_get
 * Description:
 *      Get rate of egress bandwidth control on specified queue.
 * Input:
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pMeterIndex - meter index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in Apollo is 8Kbps.
 */
int32
dal_ca8279_rate_egrQueueBwCtrlMeterIdx_get(
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pMeterIndex)
{
    int ret;
    ca_port_id_t port_id;

     /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pMeterIndex), RT_ERR_NULL_POINTER);

    *pMeterIndex = portQMCfg[port][queue].meter_index;

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_egrQueueBwCtrlMeterIdx_get */

/* Function Name:
 *      dal_ca8279_rate_egrQueueBwCtrlMeterIdx_set
 * Description:
 *      Set rate of egress bandwidth control on specified queue.
 * Input:
 *      port  - port id
 *      queue - queue id
 *      meterIndex  - meter index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in Apollo is 8Kbps.
 */
int32
dal_ca8279_rate_egrQueueBwCtrlMeterIdx_set(
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      meterIndex)
{
    int32 ret=RT_ERR_OK;
    ca_port_id_t port_id;

     /* check Init status */
    RT_INIT_CHK(rate_init);

    port_id = RTK2CA_PORT_ID(port);

    RT_PARAM_CHK((port_id == CAERR_PORT_ID), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= meterIndex), RT_ERR_FILTER_METER_ID);

    if((ret=_dal_ca8279_use_ca_queue_shaper_set(port,queue,meterIndex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return ret;
    }

    portQMCfg[port][queue].meter_index = meterIndex;

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_egrQueueBwCtrlMeterIdx_set */

/* Function Name:
 *      dal_ca8279_rate_stormControlRate_get
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
dal_ca8279_rate_stormControlMeterIdx_get(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    uint32                  *pIndex)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);
    
    switch(stormType)
    {
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
        case STORM_GROUP_UNKNOWN_UNICAST:
            *pIndex = stormCfg[port].uuc_flow_id; /*Per system setting*/
            break;
#endif
        case STORM_GROUP_UNKNOWN_MULTICAST:
            *pIndex = stormCfg[port].umc_flow_id;
            break;
        case STORM_GROUP_BROADCAST:
            *pIndex = stormCfg[port].bc_flow_id;
            break;
        case STORM_GROUP_MULTICAST:
            *pIndex = stormCfg[port].mc_flow_id;
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
        default:
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_stormControlMeterIdx_get */

/* Function Name:
 *      dal_ca8279_rate_stormControlMeterIdx_set
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
dal_ca8279_rate_stormControlMeterIdx_set(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    uint32                  index)
{
    int32 ret=RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_FILTER_METER_ID);

    switch(stormType)
    {
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
        case STORM_GROUP_UNKNOWN_UNICAST:
            stormCfg[port].uuc_flow_id = index; /*Per system setting*/
            break;
#endif
#if 0
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if(stormCfg[port].umc_flow_id != index && stormCfg[port].umc_enable == ENABLED)
            {
                if((ret=_dal_ca8279_use_ca_l2_flood_set(port,stormType,stormCfg[port].umc_enable,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].umc_flow_id = index;
            break;
#endif
        case STORM_GROUP_BROADCAST:
            if(stormCfg[port].bc_flow_id != index && stormCfg[port].bc_enable == ENABLED)
            {
#if 1
                if((ret=_dal_ca8279_bc_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
                if((ret=_dal_ca8279_bc_storm_classification_add(port,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
#else
                if((ret=_dal_ca8279_use_ca_l2_flood_set(port,stormType,stormCfg[port].bc_enable,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
#endif
            }

            stormCfg[port].bc_flow_id = index;
            break;
        case STORM_GROUP_MULTICAST:
            if(stormCfg[port].mc_flow_id != index && stormCfg[port].mc_enable == ENABLED)
            {
                if((ret=_dal_ca8279_mc_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
                if((ret=_dal_ca8279_mc_storm_classification_add(port,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
            }
            stormCfg[port].mc_flow_id = index;
            break;
        case STORM_GROUP_DHCP:
            if(stormCfg[port].dhcp_flow_id != index && stormCfg[port].dhcp_enable == ENABLED)
            {
                if((ret=_dal_ca8279_dhcp_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
                if((ret=_dal_ca8279_dhcp_storm_classification_add(port,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
            }
            stormCfg[port].dhcp_flow_id = index;
            break;
        case STORM_GROUP_ARP:
            if(stormCfg[port].arp_flow_id != index && stormCfg[port].arp_enable == ENABLED)
            {
                if((ret=_dal_ca8279_arp_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
                if((ret=_dal_ca8279_arp_storm_classification_add(port,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            stormCfg[port].arp_flow_id = index;
            break;
#if 0
        case STORM_GROUP_IGMP_MLD:
            if(stormCfg[port].igmp_mld_flow_id != index && stormCfg[port].igmp_mld_enable == ENABLED)
            {
                if((ret=_dal_ca8279_igmp_mld_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
                if((ret=_dal_ca8279_igmp_mld_storm_classification_add(port,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            stormCfg[port].igmp_mld_flow_id = index;
            break;
#endif
        default:
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    
    return RT_ERR_OK;
} /* end of dal_ca8279_rate_stormControlMeterIdx_set */

/* Function Name:
 *      dal_ca8279_rate_stormControlPortEnable_get
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
dal_ca8279_rate_stormControlPortEnable_get(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    rtk_enable_t            *pEnable)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((STORM_GROUP_END <= stormType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(stormType)
    {
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
        case STORM_GROUP_UNKNOWN_UNICAST:
            *pEnable = stormCfg[port].uuc_enable; /*Per system setting*/
            break;
#endif
#if 0
        case STORM_GROUP_UNKNOWN_MULTICAST:
            *pEnable = stormCfg[port].umc_enable;
            break;
#endif
        case STORM_GROUP_BROADCAST:
            *pEnable = stormCfg[port].bc_enable;
            break;
        case STORM_GROUP_MULTICAST:
            *pEnable = stormCfg[port].mc_enable;
            break;
        case STORM_GROUP_DHCP:
            *pEnable = stormCfg[port].dhcp_enable;
            break;
        case STORM_GROUP_ARP:
            *pEnable = stormCfg[port].arp_enable;
            break;
#if 0
        case STORM_GROUP_IGMP_MLD:
            *pEnable = stormCfg[port].igmp_mld_enable;
            break;
#endif
        default:
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_stormControlPortEnable_get */

/* Function Name:
 *      dal_ca8279_rate_stormControlPortEnable_set
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
dal_ca8279_rate_stormControlPortEnable_set(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    rtk_enable_t            enable)
{
    int32 ret=RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((STORM_GROUP_END <= stormType), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    switch(stormType)
    {
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
        case STORM_GROUP_UNKNOWN_UNICAST:
            stormCfg[port].uuc_enable = enable; /*Per system setting*/
            break;
#endif
#if 0
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if(stormCfg[port].umc_enable != enable)
            {
                if((ret=_dal_ca8279_use_ca_l2_flood_set(port,stormType,enable,stormCfg[port].umc_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].umc_enable = enable;
            break;
#endif
        case STORM_GROUP_BROADCAST:
#if 1
            if((stormCfg[port].bc_enable == DISABLED)&&(enable == ENABLED))
            {
                if((ret=_dal_ca8279_bc_storm_classification_add(port,stormCfg[port].bc_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            else if((stormCfg[port].bc_enable == ENABLED)&&(enable == DISABLED))
            {
                if((ret=_dal_ca8279_bc_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
#else
            if(stormCfg[port].bc_enable != enable)
            {
                if((ret=_dal_ca8279_use_ca_l2_flood_set(port,stormType,enable,stormCfg[port].bc_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
#endif

            stormCfg[port].bc_enable = enable;
            break;
        case STORM_GROUP_MULTICAST:
            if((stormCfg[port].mc_enable == DISABLED)&&(enable == ENABLED))
            {
                if((ret=_dal_ca8279_mc_storm_classification_add(port,stormCfg[port].mc_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            else if((stormCfg[port].mc_enable == ENABLED)&&(enable == DISABLED))
            {
                if((ret=_dal_ca8279_mc_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].mc_enable = enable;
            break;
        case STORM_GROUP_DHCP:
            if((stormCfg[port].dhcp_enable == DISABLED)&&(enable == ENABLED))
            {
                if((ret=_dal_ca8279_dhcp_storm_classification_add(port,stormCfg[port].dhcp_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            else if((stormCfg[port].dhcp_enable == ENABLED)&&(enable == DISABLED))
            {
                if((ret=_dal_ca8279_dhcp_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].dhcp_enable = enable;
            break;
        case STORM_GROUP_ARP:
            if((stormCfg[port].arp_enable == DISABLED)&&(enable == ENABLED))
            {
                if((ret=_dal_ca8279_arp_storm_classification_add(port,stormCfg[port].arp_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            else if((stormCfg[port].arp_enable == ENABLED)&&(enable == DISABLED))
            {
                if((ret=_dal_ca8279_arp_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].arp_enable = enable;
            break;
#if 0
        case STORM_GROUP_IGMP_MLD:
            if((stormCfg[port].igmp_mld_enable == DISABLED)&&(enable == ENABLED))
            {
                if((ret=_dal_ca8279_igmp_mld_storm_classification_add(port,stormCfg[port].igmp_mld_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            else if((stormCfg[port].igmp_mld_enable == ENABLED)&&(enable == DISABLED))
            {
                if((ret=_dal_ca8279_igmp_mld_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].igmp_mld_enable = enable;
            break;
#endif
        default:
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_stormControlPortEnable_set */

/* Function Name:
 *      dal_ca8279_rate_shareMeter_set
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
dal_ca8279_rate_shareMeter_set (
    uint32 index,
    uint32 rate,
    rtk_enable_t ifgInclude )
{
    ca_status_t ret=CA_E_OK;
    ca_uint16_t flow_id,group_base,i;
    ca_policer_t policer;
    ca_policer_config_t config;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_RATE), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK(((CA8279_METER_RATE_MAX) < rate), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= ifgInclude), RT_ERR_INPUT);

    if(rate >= CA8279_METER_RATE_MAX) //Disable
    {
        flow_id = index;

        if((ret = ca_l2_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        group_base = (index/32)*32;

        for(i=0;i<32;i++) /*Policer is 32 set of 1 group*/
        {
            flow_id = i+group_base;

            if((ret = ca_l2_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            if(policer.pir < CA8279_METER_RATE_MAX || policer.cir < CA8279_METER_RATE_MAX)
                break;
        }

        if(i>=32)
        {
            policer.mode = CA_POLICER_MODE_DISABLE;
        }
        else
        {
            policer.mode = CA_POLICER_MODE_SRTCM;
            policer.pps = 0;
            policer.cir = CA8279_METER_RATE_MAX;
            policer.cbs = CA8279_METER_BUCKETSIZE_MAX;
            policer.pir = CA8279_METER_RATE_MAX;
            policer.pbs = CA8279_METER_BUCKETSIZE_MAX;
        }
    }
    else
    {
        flow_id = index;

        if((ret = ca_l2_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        policer.mode = CA_POLICER_MODE_SRTCM;
        policer.cir = rate;
        policer.pir = rate;
    }

    flow_id = index;

    if((ret = ca_l2_flow_policer_set(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(ifgInclude == ENABLED)
        config.overhead = 20;
    else
        config.overhead = 0;

    if((ret = ca_l2_policer_config_set(0,&config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = _dal_ca8279_check_meter_change_set(index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_shareMeter_set */


/* Function Name:
 *      dal_ca8279_rate_shareMeter_get
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
dal_ca8279_rate_shareMeter_get (
    uint32  index,
    uint32* pRate,
    rtk_enable_t* pIfgInclude )
{
    ca_status_t ret=CA_E_OK;
    ca_uint16_t flow_id;
    ca_policer_t policer;
    ca_policer_config_t config;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_RATE), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);
    
    flow_id = index;

    if((ret = ca_l2_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(policer.mode == CA_POLICER_MODE_DISABLE)
        *pRate = CA8279_METER_RATE_MAX;
    else
        *pRate = policer.cir;

    if((ret = ca_l2_policer_config_get(0,&config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(config.overhead == 0)
        *pIfgInclude = DISABLED;
    else
        *pIfgInclude = ENABLED;

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_shareMeter_get */

/* Function Name:
 *      dal_ca8279_rate_shareMeterMode_set
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
dal_ca8279_rate_shareMeterMode_set (
    uint32 index,
    rtk_rate_metet_mode_t meterMode )
{
    ca_status_t ret=CA_E_OK;
    ca_uint16_t flow_id;
    ca_policer_t policer;

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

    flow_id = index;

    if((ret = ca_l2_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(meterMode != METER_MODE_PACKET_RATE)
        policer.pps = 0; /* True - PPS mode, false - BPS mode */
    else
        policer.pps = 1; /* True - PPS mode, false - BPS mode */

    if((ret = ca_l2_flow_policer_set(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = _dal_ca8279_check_meter_change_set(index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8279_rate_shareMeterMode_set */

/* Function Name:
 *      dal_ca8279_rate_shareMeterMode_get
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
dal_ca8279_rate_shareMeterMode_get (
    uint32 index,
    rtk_rate_metet_mode_t* pMeterMode)
{
    ca_status_t ret=CA_E_OK;
    ca_uint16_t flow_id;
    ca_policer_t policer;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_RATE), "index=%d", index);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMeterMode), RT_ERR_NULL_POINTER);

    flow_id = index;

    if((ret = ca_l2_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(policer.pps == 1)
        *pMeterMode = METER_MODE_PACKET_RATE; /* True - PPS mode, false - BPS mode */
    else
        *pMeterMode = METER_MODE_BIT_RATE; /* True - PPS mode, false - BPS mode */

    return RT_ERR_OK;
}   /* end of dal_ca8279_rate_shareMeterMode_get */

/* Function Name:
 *      dal_ca8279_rate_shareMeterBucket_set
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
dal_ca8279_rate_shareMeterBucket_set (
    uint32 index,
    uint32 bucketSize )
{
    ca_status_t ret=CA_E_OK;
    ca_uint16_t flow_id;
    ca_policer_t policer;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_RATE), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((CA8279_METER_BUCKETSIZE_MAX < bucketSize), RT_ERR_INPUT);

    flow_id = index;

    if((ret = ca_l2_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    policer.cbs = bucketSize;
    policer.pbs = bucketSize;

    if((ret = ca_l2_flow_policer_set(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = _dal_ca8279_check_meter_change_set(index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_shareMeterBucket_set */


/* Function Name:
 *      dal_ca8279_rate_shareMeterBucket_get
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
dal_ca8279_rate_shareMeterBucket_get (
    uint32 index,
    uint32* pBucketSize )
{
    ca_status_t ret=CA_E_OK;
    ca_uint16_t flow_id;
    ca_policer_t policer;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_RATE), "%s", __FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBucketSize), RT_ERR_NULL_POINTER);

    flow_id = index;

    if((ret = ca_l2_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    *pBucketSize = policer.cbs;

    return RT_ERR_OK;
} /* end of dal_ca8279_rate_shareMeterBucket_get */
