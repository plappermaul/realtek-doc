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
#include <hal/chipdef/luna_g3/luna_g3_def.h>
#include <dal/luna_g3/dal_luna_g3.h>
#include <dal/luna_g3/dal_luna_g3_rate.h>
#include <osal/time.h>

#include <cortina-api/include/rate.h>
#include <cortina-api/include/l2.h>
#include <cortina-api/include/classifier.h>

#include <aal/include/aal_l2_tm.h>
#include <aal/include/aal_l3_tm.h>

/*
 * Symbol Definition
 */

typedef struct {
    rtk_enable_t umc_enable;
    rtk_enable_t bc_enable;
    rtk_enable_t mc_enable;
    rtk_enable_t dhcp_enable;
    rtk_enable_t arp_enable;
    rtk_enable_t igmp_mld_enable;
    ca_uint16_t umc_flow_id;
    ca_uint16_t bc_flow_id;
    ca_uint16_t mc_flow_id;
    ca_uint16_t dhcp_flow_id;
    ca_uint16_t arp_flow_id;
    ca_uint16_t igmp_mld_flow_id;
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

#define DAL_LUNA_G3_STORM_UM_IDX         0
#define DAL_LUNA_G3_STORM_UC_IDX         1
#define DAL_LUNA_G3_STORM_MC_IDX         2
#define DAL_LUNA_G3_STORM_BC_IDX         3
#define DAL_LUNA_G3_STORM_DHCP_IDX       4
#define DAL_LUNA_G3_STORM_ARP_IDX        5
#define DAL_LUNA_G3_STORM_IGMP_MLD_IDX   6
#define DAL_LUNA_G3_STORM_MAX_IDX        7

typedef struct dal_luna_g3_storm_entry_s
{
    uint8   enable;
} dal_luna_g3_storm_entry_t;


typedef struct dal_luna_g3_storm_ctrl_s
{
    dal_luna_g3_storm_entry_t entry[DAL_LUNA_G3_STORM_MAX_IDX];
} dal_luna_g3_storm_ctrl_t;

/*
 * Data Declaration
 */
static uint32 rate_init = {INIT_NOT_COMPLETED};

static dal_luna_g3_storm_ctrl_t stormGlobalCfg;

/*
 * Function Declaration
 */

int32 _dal_luna_g3_use_ca_l2_flood_set(rtk_port_t port,rtk_rate_storm_group_t stormType,rtk_enable_t enable,ca_uint16_t flow_id)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_l2_flooding_type_t ptype;
    ca_uint32_t rate;
    ca_policer_t policer;

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
        if((ret = ca_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        rate = policer.cir;
    }

    if((ret = ca_l2_flooding_rate_set(0,port_id,ptype,&rate)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

int32 _dal_luna_g3_mc_storm_classification_add(rtk_port_t port,ca_uint16_t flow_id)
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

    key.l2.is_multicast = 1;
    key_mask.l2 = 1;
    key_mask.l2_mask.is_multicast = 1;

    action.forward = CA_CLASSIFIER_FORWARD_FE;
    action.dest.port = 0;

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&stormCfg[port].mc_storm_index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_luna_g3_mc_storm_classification_delete(rtk_port_t port)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].mc_storm_index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_luna_g3_dhcp_storm_classification_add(rtk_port_t port,ca_uint16_t flow_id)
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

    key.ip.ip_valid = 1;
    key.ip.ip_protocol = 17; //UDP
    key_mask.ip = 1;
    key_mask.ip_mask.ip_valid = 1;
    key_mask.ip_mask.ip_protocol = 1;

    key.l4.l4_valid = 1;
    key.l4.src_port.min = 67;
    key.l4.src_port.max = 67;
    key.l4.dst_port.min = 68;
    key.l4.dst_port.max = 68;

    key_mask.l4 = 1;
    key_mask.l4_mask.l4_valid = 1;
    key_mask.l4_mask.src_port = 1;
    key_mask.l4_mask.dst_port = 1;

    action.forward = CA_CLASSIFIER_FORWARD_FE;
    action.dest.port = 0;

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&stormCfg[port].dhcp_storm_index0)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    key.l4.src_port.min = 68;
    key.l4.src_port.max = 68;
    key.l4.dst_port.min = 67;
    key.l4.dst_port.max = 67;

    if((ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&stormCfg[port].dhcp_storm_index1)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_luna_g3_dhcp_storm_classification_delete(rtk_port_t port)
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

int32 _dal_luna_g3_arp_storm_classification_add(rtk_port_t port,ca_uint16_t flow_id)
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

    key.l2.ethertype = 0x0806; // ARP
    key_mask.l2 = 1;
    key_mask.l2_mask.ethertype = 1;

    action.forward = CA_CLASSIFIER_FORWARD_FE;
    action.dest.port = 0;

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&stormCfg[port].arp_storm_index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_luna_g3_arp_storm_classification_delete(rtk_port_t port)
{
    ca_status_t ret=CA_E_OK;

    if((ret = ca_classifier_rule_delete(0,stormCfg[port].arp_storm_index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_luna_g3_igmp_mld_storm_classification_add(rtk_port_t port,ca_uint16_t flow_id)
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

    key.ip.ip_valid = 1;
    key.ip.ip_version = 4; //IPv4
    key.ip.ip_protocol = 2; //IGMP
    key_mask.ip = 1;
    key_mask.ip_mask.ip_valid = 1;
    key_mask.ip_mask.ip_version = 1;
    key_mask.ip_mask.ip_protocol = 1;

    action.forward = CA_CLASSIFIER_FORWARD_FE;
    action.dest.port = 0;

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&stormCfg[port].igmp_mld_storm_index0)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    key.ip.ip_version = 6; //IPv6
    key.ip.ip_protocol = 58; //ICMPv6
    key.ip.icmp_type = 130; //MLD query
    key_mask.ip_mask.icmp_type = 1;

    if((ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&stormCfg[port].igmp_mld_storm_index1)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    key.ip.icmp_type = 131; //MLD report
    
    if((ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&stormCfg[port].igmp_mld_storm_index2)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    key.ip.icmp_type = 132; //MLD done
    
    if((ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&stormCfg[port].igmp_mld_storm_index3)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    key.ip.icmp_type = 143; //MLD report v2
    
    if((ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&stormCfg[port].igmp_mld_storm_index4)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_luna_g3_igmp_mld_storm_classification_delete(rtk_port_t port)
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

int32 _dal_luna_g3_use_ca_l2_classifier_flood_set(rtk_port_t port,rtk_rate_storm_group_t stormType,rtk_enable_t enable,ca_uint16_t flow_id)
{
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
    ca_uint32_t index;

    port_id = RTK2CA_PORT_ID(port);

    memset(&key,0,sizeof(ca_classifier_key_t));
    memset(&key_mask,0,sizeof(ca_classifier_key_mask_t));
    memset(&action,0,sizeof(ca_classifier_action_t));

    key.src_port = port_id;
    key_mask.src_port = 1;

    switch(stormType)
    {
        case STORM_GROUP_DHCP:
            key.ip.ip_valid = 1;
            key.ip.ip_protocol = 17; //UDP
            key_mask.ip = 1;
            key_mask.ip_mask.ip_valid = 1;
            key_mask.ip_mask.ip_protocol = 1;

            key.l4.l4_valid = 1;
            key.l4.src_port.min = 67;
            key.l4.src_port.max = 68;
            key.l4.dst_port.min = 67;
            key.l4.dst_port.max = 68;

            key_mask.l4 = 1;
            key_mask.l4_mask.l4_valid = 1;
            key_mask.l4_mask.src_port = 1;
            key_mask.l4_mask.dst_port = 1;
            break;
        case STORM_GROUP_ARP:
            key.l2.ethertype = 0x0806; // ARP
            key_mask.l2 = 1;
            key_mask.l2_mask.ethertype = 1;
            break;
        case STORM_GROUP_IGMP_MLD:
            key.ip.ip_valid = 1;
            key.ip.ip_version = 4; //IPv4
            key.ip.ip_protocol = 2; //IGMP
            key_mask.ip = 1;
            key_mask.ip_mask.ip_valid = 1;
            key_mask.ip_mask.ip_version = 1;
            key_mask.ip_mask.ip_protocol = 1;
            break;
        default:
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_INPUT;
    }

    action.forward = CA_CLASSIFIER_FORWARD_FE;
    action.dest.port = 0;

    action.options.action_handle.flow_id = flow_id;
    action.options.masks.action_handle = 1;

    if((ret = ca_classifier_rule_add(0,7,&key,&key_mask,&action,&index)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Module Name    : Rate                                           */
/* Sub-module Name: Configuration of egress port bandwidth control */

/* Function Name:
 *      dal_luna_g3_rate_init
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
dal_luna_g3_rate_init(void)
{
    ca_uint16_t flow_id;
    ca_policer_t policer;

    rate_init = INIT_COMPLETED;

    memset(stormCfg,0,sizeof(rate_storm_cfg_t)*RTK_MAX_NUM_OF_PORTS);

    policer.mode = CA_POLICER_MODE_DISABLE;
    policer.pps = 0;
    policer.cir = 32767499;
    policer.cbs = 0xfff;
    policer.pir = 32767499;
    policer.pbs = 0xfff;
    
    for(flow_id=0;flow_id <= CA_AAL_MAX_FLOW_ID&&flow_id <= CA_AAL_L3_MAX_FLOW_ID;flow_id++)
    {
        if(ca_flow_policer_set(0,flow_id,&policer) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        }
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_init */

/* Function Name:
 *      dal_luna_g3_rate_portIgrBandwidthCtrlRate_get
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
dal_luna_g3_rate_portIgrBandwidthCtrlRate_get(rtk_port_t port, uint32 *pRate)
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

    *pRate = policer.cir;

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_portIgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_luna_g3_rate_portIgrBandwidthCtrlRate_set
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
dal_luna_g3_rate_portIgrBandwidthCtrlRate_set(rtk_port_t port, uint32 rate)
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

    policer.mode = CA_POLICER_MODE_SRTCM;
    policer.pps = 0; /* True - PPS mode, false - BPS mode */
    policer.cir = rate;
    policer.cbs = (policer.cir/1000 + 511) >> 9;
    policer.cbs = (policer.cbs == 0) ? 1: policer.cbs;
    /*The burst size granularity is 256 bytes or 1 packet. 
    The minimum value is roundup (committed_rate_m/512) in byte mode 
    and roudup (committed_rate_m/2) in packet mode.*/
    policer.pir = rate ;
    policer.pbs = (policer.pir/1000 + 511) >> 9;
    policer.pbs = (policer.pbs == 0) ? 1: policer.pbs;

    if((ret = ca_port_policer_set(0,port_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_portIgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_luna_g3_rate_portEgrBandwidthCtrlRate_get
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
dal_luna_g3_rate_portEgrBandwidthCtrlRate_get(rtk_port_t port, uint32 *pRate)
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

    if((ret = ca_port_shaper_get(0,port_id,&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    *pRate = shaper.rate;

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_portEgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_luna_g3_rate_portEgrBandwidthCtrlRate_set
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
dal_luna_g3_rate_portEgrBandwidthCtrlRate_set(rtk_port_t port, uint32 rate)
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

    shaper.enable = 1;
    shaper.pps = 0; /* True - PPS mode, false - BPS mode */
    shaper.rate = rate;
    shaper.burst_size = (shaper.rate/1000 + 511) >> 9;
    shaper.burst_size = (shaper.burst_size == 0) ? 1: shaper.burst_size;
    /*The burst size granularity is 256 bytes or 1 packet. 
    The minimum value is roundup (committed_rate_m/512) in byte mode 
    and roudup (committed_rate_m/2) in packet mode.*/

    if((ret = ca_port_shaper_set(0,port_id,&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_RATE), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_portEgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_luna_g3_rate_stormControlRate_get
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
dal_luna_g3_rate_stormControlMeterIdx_get(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    uint32                  *pIndex)
{
    uint32  groupIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);
    
    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_MULTICAST:
            groupIdx = DAL_LUNA_G3_STORM_UM_IDX;
            *pIndex = stormCfg[port].umc_flow_id;
            break;
        case STORM_GROUP_BROADCAST:
            groupIdx = DAL_LUNA_G3_STORM_BC_IDX;
            *pIndex = stormCfg[port].bc_flow_id;
            break;
        case STORM_GROUP_MULTICAST:
            groupIdx = DAL_LUNA_G3_STORM_MC_IDX;
            *pIndex = stormCfg[port].mc_flow_id;
            break;
        case STORM_GROUP_DHCP:
            groupIdx = DAL_LUNA_G3_STORM_DHCP_IDX;
            *pIndex = stormCfg[port].dhcp_flow_id;
            break;
        case STORM_GROUP_ARP:
            groupIdx = DAL_LUNA_G3_STORM_ARP_IDX;
            *pIndex = stormCfg[port].arp_flow_id;
            break;
        case STORM_GROUP_IGMP_MLD:
            groupIdx = DAL_LUNA_G3_STORM_IGMP_MLD_IDX;
            *pIndex = stormCfg[port].igmp_mld_flow_id;
            break;
        default:
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_INPUT;
    }
    if(!stormGlobalCfg.entry[groupIdx].enable)
        return RT_ERR_ENTRY_NOTFOUND;

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_stormControlMeterIdx_get */

/* Function Name:
 *      dal_luna_g3_rate_stormControlMeterIdx_set
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
dal_luna_g3_rate_stormControlMeterIdx_set(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    uint32                  index)
{
    int32 ret=RT_ERR_OK;
    uint32 groupIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_FILTER_METER_ID);

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_MULTICAST:
            groupIdx = DAL_LUNA_G3_STORM_UM_IDX;
            break;
        case STORM_GROUP_BROADCAST:
            groupIdx = DAL_LUNA_G3_STORM_BC_IDX;
            break;
        case STORM_GROUP_MULTICAST:
            groupIdx = DAL_LUNA_G3_STORM_MC_IDX;
            break;
        case STORM_GROUP_DHCP:
            groupIdx = DAL_LUNA_G3_STORM_DHCP_IDX;
            break;
        case STORM_GROUP_ARP:
            groupIdx = DAL_LUNA_G3_STORM_ARP_IDX;
            break;
        case STORM_GROUP_IGMP_MLD:
            groupIdx = DAL_LUNA_G3_STORM_IGMP_MLD_IDX;
            break;
        default:
           return RT_ERR_INPUT;
    }
    if(!stormGlobalCfg.entry[groupIdx].enable)
        return RT_ERR_ENTRY_NOTFOUND;

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if(stormCfg[port].umc_flow_id != index && stormCfg[port].umc_enable == ENABLED)
            {
                if((ret=_dal_luna_g3_use_ca_l2_flood_set(port,stormType,stormCfg[port].umc_enable,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].umc_flow_id = index;
            break;
        case STORM_GROUP_BROADCAST:
            if(stormCfg[port].bc_flow_id != index && stormCfg[port].bc_enable == ENABLED)
            {
                if((ret=_dal_luna_g3_use_ca_l2_flood_set(port,stormType,stormCfg[port].bc_enable,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].bc_flow_id = index;
            break;
        case STORM_GROUP_MULTICAST:
            if(stormCfg[port].mc_flow_id != index && stormCfg[port].mc_enable == ENABLED)
            {
                if((ret=_dal_luna_g3_mc_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
                if((ret=_dal_luna_g3_mc_storm_classification_add(port,index)) != RT_ERR_OK)
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
                if((ret=_dal_luna_g3_dhcp_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
                if((ret=_dal_luna_g3_dhcp_storm_classification_add(port,index)) != RT_ERR_OK)
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
                if((ret=_dal_luna_g3_arp_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
                if((ret=_dal_luna_g3_arp_storm_classification_add(port,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            stormCfg[port].arp_flow_id = index;
            break;
        case STORM_GROUP_IGMP_MLD:
            if(stormCfg[port].igmp_mld_flow_id != index && stormCfg[port].igmp_mld_enable == ENABLED)
            {
                if((ret=_dal_luna_g3_igmp_mld_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
                
                if((ret=_dal_luna_g3_igmp_mld_storm_classification_add(port,index)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            stormCfg[port].igmp_mld_flow_id = index;
            break;
        default:
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_INPUT;
    }
    
    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_stormControlMeterIdx_set */

/* Function Name:
 *      dal_luna_g3_rate_stormControlPortEnable_get
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
dal_luna_g3_rate_stormControlPortEnable_get(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    rtk_enable_t            *pEnable)
{
    uint32 groupIdx;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((STORM_GROUP_END <= stormType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_MULTICAST:
            *pEnable = stormCfg[port].umc_enable;
            groupIdx = DAL_LUNA_G3_STORM_UM_IDX;
            break;
        case STORM_GROUP_BROADCAST:
            *pEnable = stormCfg[port].bc_enable;
            groupIdx = DAL_LUNA_G3_STORM_BC_IDX;
            break;
        case STORM_GROUP_MULTICAST:
            *pEnable = stormCfg[port].mc_enable;
            groupIdx = DAL_LUNA_G3_STORM_MC_IDX;
            break;
        case STORM_GROUP_DHCP:
            *pEnable = stormCfg[port].dhcp_enable;
            groupIdx = DAL_LUNA_G3_STORM_DHCP_IDX;
            break;
        case STORM_GROUP_ARP:
            *pEnable = stormCfg[port].arp_enable;
            groupIdx = DAL_LUNA_G3_STORM_ARP_IDX;
            break;
        case STORM_GROUP_IGMP_MLD:
            *pEnable = stormCfg[port].igmp_mld_enable;
            groupIdx = DAL_LUNA_G3_STORM_IGMP_MLD_IDX;
            break;
        default:
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_INPUT;
    }
    if(!stormGlobalCfg.entry[groupIdx].enable)
        return RT_ERR_ENTRY_NOTFOUND;

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_stormControlPortEnable_get */

/* Function Name:
 *      dal_luna_g3_rate_stormControlPortEnable_set
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
dal_luna_g3_rate_stormControlPortEnable_set(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    rtk_enable_t            enable)
{
    int32 ret=RT_ERR_OK;
    uint32 groupIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((STORM_GROUP_END <= stormType), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_MULTICAST:
            groupIdx = DAL_LUNA_G3_STORM_UM_IDX;
            break;
        case STORM_GROUP_BROADCAST:
            groupIdx = DAL_LUNA_G3_STORM_BC_IDX;
            break;
        case STORM_GROUP_MULTICAST:
            groupIdx = DAL_LUNA_G3_STORM_MC_IDX;
            break;
        case STORM_GROUP_DHCP:
            groupIdx = DAL_LUNA_G3_STORM_DHCP_IDX;
            break;
        case STORM_GROUP_ARP:
            groupIdx = DAL_LUNA_G3_STORM_ARP_IDX;
            break;
        case STORM_GROUP_IGMP_MLD:
            groupIdx = DAL_LUNA_G3_STORM_IGMP_MLD_IDX;
            break;
        default:
           return RT_ERR_INPUT;
    }
    if(!stormGlobalCfg.entry[groupIdx].enable)
        return RT_ERR_ENTRY_NOTFOUND;

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if(stormCfg[port].umc_enable != enable)
            {
                if((ret=_dal_luna_g3_use_ca_l2_flood_set(port,stormType,enable,stormCfg[port].umc_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].umc_enable = enable;
            break;
        case STORM_GROUP_BROADCAST:
            if(stormCfg[port].bc_enable != enable)
            {
                if((ret=_dal_luna_g3_use_ca_l2_flood_set(port,stormType,enable,stormCfg[port].bc_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].bc_enable = enable;
            break;
        case STORM_GROUP_MULTICAST:
            if((stormCfg[port].mc_enable == DISABLED)&&(enable == ENABLED))
            {
                if((ret=_dal_luna_g3_mc_storm_classification_add(port,stormCfg[port].mc_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            else if((stormCfg[port].mc_enable == ENABLED)&&(enable == DISABLED))
            {
                if((ret=_dal_luna_g3_mc_storm_classification_delete(port)) != RT_ERR_OK)
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
                if((ret=_dal_luna_g3_dhcp_storm_classification_add(port,stormCfg[port].dhcp_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            else if((stormCfg[port].dhcp_enable == ENABLED)&&(enable == DISABLED))
            {
                if((ret=_dal_luna_g3_dhcp_storm_classification_delete(port)) != RT_ERR_OK)
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
                if((ret=_dal_luna_g3_arp_storm_classification_add(port,stormCfg[port].arp_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            else if((stormCfg[port].arp_enable == ENABLED)&&(enable == DISABLED))
            {
                if((ret=_dal_luna_g3_arp_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].arp_enable = enable;
            break;
        case STORM_GROUP_IGMP_MLD:
            if((stormCfg[port].igmp_mld_enable == DISABLED)&&(enable == ENABLED))
            {
                if((ret=_dal_luna_g3_igmp_mld_storm_classification_add(port,stormCfg[port].igmp_mld_flow_id)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }
            else if((stormCfg[port].igmp_mld_enable == ENABLED)&&(enable == DISABLED))
            {
                if((ret=_dal_luna_g3_igmp_mld_storm_classification_delete(port)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                    return ret;
                }
            }

            stormCfg[port].igmp_mld_enable = enable;
            break;
        default:
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_stormControlPortEnable_set */

/* Function Name:
 *      dal_luna_g3_rate_stormControlEnable_get
 * Description:
 *      Get enable status of storm control on specified port.
 * Input:
 *      stormType  - storm group type
 * Output:
 *      pEnable    - pointer to enable status of storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
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
dal_luna_g3_rate_stormControlEnable_get(rtk_rate_storm_group_ctrl_t  *stormCtrl)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == stormCtrl), RT_ERR_NULL_POINTER);

    stormCtrl->unknown_unicast_enable  =stormGlobalCfg.entry[DAL_LUNA_G3_STORM_UC_IDX].enable       ;
    stormCtrl->unknown_multicast_enable=stormGlobalCfg.entry[DAL_LUNA_G3_STORM_UM_IDX].enable       ;
    stormCtrl->broadcast_enable        =stormGlobalCfg.entry[DAL_LUNA_G3_STORM_BC_IDX].enable       ;
    stormCtrl->multicast_enable        =stormGlobalCfg.entry[DAL_LUNA_G3_STORM_MC_IDX].enable       ;
    stormCtrl->dhcp_enable             =stormGlobalCfg.entry[DAL_LUNA_G3_STORM_DHCP_IDX].enable     ;
    stormCtrl->arp_enable              =stormGlobalCfg.entry[DAL_LUNA_G3_STORM_ARP_IDX].enable      ;
    stormCtrl->igmp_mld_enable         =stormGlobalCfg.entry[DAL_LUNA_G3_STORM_IGMP_MLD_IDX].enable ;

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_stormControlEnable_get */


/* Function Name:
 *      dal_luna_g3_rate_stormControlEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 * Input:
 *      stormType  - storm group type
 *      enable     - enable status of storm control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    total 4 storm type can be enabled.
 *      - if total enable group exceed 4 system will return RT_ERR_INPUT
 *      - when global storm type set to disable the per port setting for this
 *        storm type will also set to disable for all port.
 */
int32
dal_luna_g3_rate_stormControlEnable_set(rtk_rate_storm_group_ctrl_t  *stormCtrl)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == stormCtrl), RT_ERR_INPUT);

    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->unknown_unicast_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->unknown_multicast_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->broadcast_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->multicast_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->dhcp_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->arp_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->igmp_mld_enable), RT_ERR_INPUT);

    memset(&stormGlobalCfg,0x0,sizeof(dal_luna_g3_storm_ctrl_t));

    /*disable all strom control for all port*/

    stormGlobalCfg.entry[DAL_LUNA_G3_STORM_UC_IDX].enable       = stormCtrl->unknown_unicast_enable;
    stormGlobalCfg.entry[DAL_LUNA_G3_STORM_UM_IDX].enable       = stormCtrl->unknown_multicast_enable;
    stormGlobalCfg.entry[DAL_LUNA_G3_STORM_BC_IDX].enable       = stormCtrl->broadcast_enable;
    stormGlobalCfg.entry[DAL_LUNA_G3_STORM_MC_IDX].enable       = stormCtrl->multicast_enable;
    stormGlobalCfg.entry[DAL_LUNA_G3_STORM_DHCP_IDX].enable     = stormCtrl->dhcp_enable;
    stormGlobalCfg.entry[DAL_LUNA_G3_STORM_ARP_IDX].enable      = stormCtrl->arp_enable;
    stormGlobalCfg.entry[DAL_LUNA_G3_STORM_IGMP_MLD_IDX].enable = stormCtrl->igmp_mld_enable;

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_stormControlEnable_set */

/* Function Name:
 *      dal_luna_g3_rate_shareMeter_set
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
dal_luna_g3_rate_shareMeter_set (
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
    RT_PARAM_CHK(((LUNA_G3_METER_RATE_MAX) < rate), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= ifgInclude), RT_ERR_INPUT);

    if(rate >= 32767499) //Disable
    {
        flow_id = index;

        if((ret = ca_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        policer.pps = 0;
        policer.cir = 32767499;
        policer.cbs = 0xfff;
        policer.pir = 32767499;
        policer.pbs = 0xfff;

        if((ret = ca_flow_policer_set(0,flow_id,&policer)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        group_base = (index/32)*32;

        for(i=0;i<32;i++) /*Policer is 32 set of 1 group*/
        {
            flow_id = i+group_base;

            if((ret = ca_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            
            if(policer.pir < 32767499 || policer.cir < 32767499)
            {
                RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                return RT_ERR_OK; /*Because RG not retuen Error*/
            }
        }

        flow_id = index;

        if((ret = ca_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        policer.mode = CA_POLICER_MODE_DISABLE;
        policer.pps = 0;
        policer.cir = 32767499;
        policer.cbs = 0xfff;
        policer.pir = 32767499;
        policer.pbs = 0xfff;
    }
    else
    {
        flow_id = index;

        if((ret = ca_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        policer.mode = CA_POLICER_MODE_SRTCM;
        policer.cir = rate;

		/*
		(According to CA recommendation, the value for CBS and PBS should have relation with CIR and PIR.
		CIR and PIR bigger, CBS and PBS should be bigger.)
		The following settings are provided by CA.
		If CIR = 100kbps, cbs can be programmed to 10(2560 byte).
		If CIR = 1Mbps ~10Mbps, cbs can be programmed to 100(25600 byte).
		If CIR = 10Mbps~100Mbps, CBS can be programmed to 1000(256000 byte).
		If CIR >100Mbps, CBS can be programmed to 0xfff.
		*/
		if(policer.cir == 0)
		{
			policer.cbs = 1; //set to minimum value
		}
		else if(policer.cir < 100)
		{
			policer.cbs = 10; 
		}
		else if((policer.cir >= 100) && (policer.cir < 1000))
		{
			policer.cbs = 64; //set to the value that approach to RTK setting (64  * 256 = 16384 ~= 0x3fff)
		}
		else if((policer.cir >= 1000) && (policer.cir < 10000))
		{
			policer.cbs = 100;
		}
		else if((policer.cir >= 10000) && (policer.cir < 100000))
		{
			policer.cbs = 1000;
		}
		else
		{
			policer.cbs = 0xfff; 
		}

        policer.pir = rate;

		if(policer.pir == 0)
		{
			policer.pbs = 1; //set to minimum value
		}
		else if(policer.pir < 100)
		{
			policer.pbs = 10; 
		}
		else if((policer.pir >= 100) && (policer.pir < 1000))
		{
			policer.pbs = 64; //set to the value that approach to RTK setting (64  * 256 = 16384 ~= 0x3fff)
		}
		else if((policer.pir >= 1000) && (policer.pir < 10000))
		{
			policer.pbs = 100;
		}
		else if((policer.pir >= 10000) && (policer.pir < 100000))
		{
			policer.pbs = 1000;
		}
		else
		{
			policer.pbs = 0xfff; 
		}
    }

    if((ret = ca_flow_policer_set(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(ifgInclude == ENABLED)
        config.overhead = 20;
    else
        config.overhead = 0;

    if((ret = ca_policer_config_set(0,&config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_shareMeter_set */


/* Function Name:
 *      dal_luna_g3_rate_shareMeter_get
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
dal_luna_g3_rate_shareMeter_get (
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

    if((ret = ca_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    *pRate = policer.cir;

    if((ret = ca_policer_config_get(0,&config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(config.overhead == 0)
        *pIfgInclude = DISABLED;
    else
        *pIfgInclude = ENABLED;

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_shareMeter_get */

/* Function Name:
 *      dal_luna_g3_rate_shareMeterMode_set
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
dal_luna_g3_rate_shareMeterMode_set (
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

    if(meterMode != METER_MODE_PACKET_RATE)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_INPUT;
    }

    flow_id = index;

    if((ret = ca_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    policer.pps = 0; /* True - PPS mode, false - BPS mode */

    if((ret = ca_flow_policer_set(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_luna_g3_rate_shareMeterMode_set */

/* Function Name:
 *      dal_luna_g3_rate_shareMeterMode_get
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
dal_luna_g3_rate_shareMeterMode_get (
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

    if((ret = ca_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(policer.pps == 1)
        *pMeterMode = METER_MODE_PACKET_RATE; /* True - PPS mode, false - BPS mode */
    else
        *pMeterMode = METER_MODE_BIT_RATE; /* True - PPS mode, false - BPS mode */

    return RT_ERR_OK;
}   /* end of dal_luna_g3_rate_shareMeterMode_get */

/* Function Name:
 *      dal_luna_g3_rate_shareMeterBucket_set
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
dal_luna_g3_rate_shareMeterBucket_set (
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
    RT_PARAM_CHK((LUNA_G3_METER_BUCKETSIZE_MAX < bucketSize), RT_ERR_INPUT);

    flow_id = index;

    if((ret = ca_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(bucketSize < (policer.cir/1000 + 511) >> 9)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    policer.cbs = bucketSize;
    /*The burst size granularity is 256 bytes or 1 packet. 
    The minimum value is roundup (committed_rate_m/512) in byte mode 
    and roudup (committed_rate_m/2) in packet mode.*/

    if(bucketSize <= (policer.pir/1000 + 511) >> 9)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    policer.pbs = bucketSize;

    if((ret = ca_flow_policer_set(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_shareMeterBucket_set */


/* Function Name:
 *      dal_luna_g3_rate_shareMeterBucket_get
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
dal_luna_g3_rate_shareMeterBucket_get (
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

    if((ret = ca_flow_policer_get(0,flow_id,&policer)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    *pBucketSize = policer.cbs;

    return RT_ERR_OK;
} /* end of dal_luna_g3_rate_shareMeterBucket_get */
