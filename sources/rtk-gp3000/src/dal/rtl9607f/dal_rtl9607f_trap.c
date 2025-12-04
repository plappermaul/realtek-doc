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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (星期五, 03 五月 2013) $
 *
 * Purpose : Definition of TRAP API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Configuration for traping packet to CPU
 *           (2) RMA
 *           (3) User defined RMA
 *           (4) System-wise management frame
 *           (5) System-wise user defined management frame
 *           (6) Per port user defined management frame
 *           (7) Packet with special flag or option
 *           (8) CFM and OAM packet
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/trap.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_trap.h>
#include <osal/time.h>

#include "cortina-api/include/port.h"
#include "cortina-api/include/special_packet.h"
#include "aal_port.h"
#include "aal_l2_specpkt.h"
#include "aal_l2_cls.h"
#include "aal_pdc.h"
/*
 * Symbol Definition
 */

#define DAL_RTL9607F_TRAP_CPU_LDPID 0x10
#define DAL_RTL9607F_TRAP_ETH_PORT_MAX 8
#define DAL_RTL9607F_TRAP_CLS_RULE_PER_PORT 4


typedef struct dal_9607f_trap_config_s
{
    rt_action_t action;
    rt_enable_t stormEn;
    uint8 policerId;
    uint8 clsIndex;
    uint8 clsSubIndex;
    uint8 priority;
}dal_9607f_trap_config_t;



/*
 * Data Declaration
 */
static uint32 rtl9607f_trap_init = INIT_NOT_COMPLETED;

#if defined(CONFIG_SDK_RTL8198X)
uint8 rtl9607f_trap_port_spb_idx_map[8] = {1,2,3,4,5,7,6,7};
#else
uint8 rtl9607f_trap_port_spb_idx_map[8] = {1,2,3,4,5,5,6,7};
#endif

dal_9607f_trap_config_t rtl9607f_trap_loop_detect_config[DAL_RTL9607F_TRAP_ETH_PORT_MAX];


/*
 * Function Declaration
 */


int32 _dal_rtl9607f_trap_action_set(aal_l2_specpkt_type_t type, rt_port_t port, rt_action_t action)
{
    aal_l2_specpkt_ctrl_t ctrl;
    aal_l2_specpkt_ctrl_mask_t ctrlMask;
    rt_enable_t stormEn = DISABLED;
    ca_status_t ret=CA_E_OK;

    if(port > HAL_GET_PON_PORT())
        return RT_ERR_PORT_ID;

    memset(&ctrl,0,sizeof(aal_l2_specpkt_ctrl_t));
    ret = aal_l2_specpkt_ctrl_get(0, rtl9607f_trap_port_spb_idx_map[port], type, &ctrl);
    if(ret != CA_E_OK)
        return RT_ERR_FAILED;
    stormEn = ctrl.grp_flow_en;
    memset(&ctrl,0,sizeof(aal_l2_specpkt_ctrl_t));
    memset(&ctrlMask,0,sizeof(aal_l2_specpkt_ctrl_mask_t));
    ctrlMask.s.spcl_fwd = 1;
    ctrlMask.s.ldpid =1;
    if(action == ACTION_TRAP2CPU)
    {
        ctrl.spcl_fwd = 1;
        ctrl.ldpid = DAL_RTL9607F_TRAP_CPU_LDPID;
        ctrlMask.s.bypass_sa_lrn_chk = 1;
        ctrl.bypass_sa_lrn_chk = 1;
    }
    else if(action == ACTION_FORWARD)
    {
        if(stormEn != DISABLED)
        {
            ctrl.spcl_fwd = 1;
            if (HAL_IS_PON_PORT(port))
                ctrl.ldpid = AAL_LPORT_L3_WAN;
            else
                ctrl.ldpid = AAL_LPORT_L3_LAN;
        }
        else
        {
            ctrl.spcl_fwd = 0;
            ctrl.ldpid = 0;
        }
    }
    else
    {
        return RT_ERR_FWD_ACTION;
    }
    ret = aal_l2_specpkt_ctrl_set(0, rtl9607f_trap_port_spb_idx_map[port], type, ctrlMask, &ctrl);
    if(ret != CA_E_OK)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

int32 _dal_rtl9607f_trap_action_get(aal_l2_specpkt_type_t type, rt_port_t port, rt_action_t *pAction)
{
    aal_l2_specpkt_ctrl_t ctrl;
    ca_status_t ret=CA_E_OK;

    if(port > HAL_GET_PON_PORT())
        return RT_ERR_PORT_ID;

    if(pAction == NULL)
        return RT_ERR_NULL_POINTER;

    memset(&ctrl,0,sizeof(aal_l2_specpkt_ctrl_t));

    ret = aal_l2_specpkt_ctrl_get(0, rtl9607f_trap_port_spb_idx_map[port], type, &ctrl);
    if(ret != CA_E_OK)
        return RT_ERR_FAILED;

    if(ctrl.spcl_fwd == 1 && ctrl.ldpid == DAL_RTL9607F_TRAP_CPU_LDPID)
        *pAction = ACTION_TRAP2CPU;
    else
        *pAction = ACTION_FORWARD;

    return RT_ERR_OK;
}

int32 _dal_rtl9607f_trap_loopDetectClsIdx_get(rt_port_t port, uint8 *index, uint8 *subIndex)
{
    aal_ilpb_cfg_t ilpbCfg;
    aal_l2_cls_entry_status_t entryStatus;
    ca_status_t ret=CA_E_OK;
    int i;

    if(port >= DAL_RTL9607F_TRAP_ETH_PORT_MAX)
        return RT_ERR_FAILED;
    if(rtl9607f_trap_loop_detect_config[port].clsIndex != 0xff)
    {
        *index = rtl9607f_trap_loop_detect_config[port].clsIndex;
        *subIndex = rtl9607f_trap_loop_detect_config[port].clsSubIndex;
        return RT_ERR_OK;
    }

    ret = aal_port_ilpb_cfg_get(0,port,&ilpbCfg);
    if(CA_E_OK != ret){
        return RT_ERR_FAILED;
    }

    for(i=ilpbCfg.cls_start; i < (ilpbCfg.cls_start+ilpbCfg.cls_length) ; i++)
    {
        ret = aal_l2_cls_entry_status_get(0, i, &entryStatus);
        if(ret != CA_E_OK){
            continue;
        }
        if(entryStatus.valid == 0)
        {
            *index = i;
            *subIndex = 0;
            return RT_ERR_OK;
        }
        if(entryStatus.entry_type == AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY && entryStatus.rule_0 == AAL_L2_CLS_KEY_2RULE_TYPE_INVALID)
        {
            *index = i;
            *subIndex = 0;
            return RT_ERR_OK;
        }
        if(entryStatus.entry_type == AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY && entryStatus.rule_1 == AAL_L2_CLS_KEY_2RULE_TYPE_INVALID)
        {
            *index = i;
            *subIndex = 1;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
}

int32 _dal_rtl9607f_trap_loopDetectCls_update(rt_port_t port)
{
    aal_l2_cls_entry_status_t entryStatus;
    aal_l2_cls_entry_status_mask_t entryStatusMask;
    aal_l2_cls_l2r_key_mask_t keyMask;
    aal_l2_cls_l2r_key_t key;
    aal_l2_cls_fib_mask_t fibMask;
    aal_l2_cls_fib_t fib;
    ca_status_t ret=CA_E_OK;
    uint8 clsIndex = 0xff,clsSubIndex = 0;

    if(rtl9607f_trap_loop_detect_config[port].action == ACTION_FORWARD && rtl9607f_trap_loop_detect_config[port].stormEn == DISABLED)
    {
        /* disable CLS rule */
        if(rtl9607f_trap_loop_detect_config[port].clsIndex != 0xff)
        {
            memset(&keyMask, 0xff, sizeof(aal_l2_cls_l2r_key_mask_t));
            memset(&key, 0, sizeof(aal_l2_cls_l2r_key_t));
            key.mac_field_ctrl  = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
            key.vlan_field_ctrl = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
            key.dot1p_ctrl      = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
            key.l4_port_field_ctrl = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
            if(rtl9607f_trap_loop_detect_config[port].clsSubIndex == 0)
                aal_l2_cls_l2r_1st_key_set(0, rtl9607f_trap_loop_detect_config[port].clsIndex, keyMask, &key);
            else
                aal_l2_cls_l2r_2nd_key_set(0, rtl9607f_trap_loop_detect_config[port].clsIndex, keyMask, &key);

            memset(&fibMask, 0, sizeof(aal_l2_cls_fib_mask_t));
            memset(&fib, 0, sizeof(aal_l2_cls_fib_t));
            if(rtl9607f_trap_loop_detect_config[port].clsSubIndex == 0)
            {
                fibMask.s.permit_0        = 1;
                fibMask.s.permit_valid_0      = 1;
                fibMask.s.premarked_0     = 1;
                fibMask.s.top_sc_ind_0    = 1;
                fibMask.s.top_vlan_cmd_0      = 1;
                fibMask.s.top_cmd_valid_0   = 1;
                fibMask.s.top_vlanid_0    = 1;
                fibMask.s.inner_sc_ind_0      = 1;
                fibMask.s.inner_vlan_cmd_0  = 1;
                fibMask.s.inner_cmd_valid_0 = 1;
                fibMask.s.inner_vlanid_0      = 1;
                fibMask.s.dot1p_0         = 1;
                fibMask.s.dot1p_valid_0   = 1;
                fibMask.s.dscp_0              = 1;
                fibMask.s.dscp_valid_0    = 1;
                fibMask.s.cos_0           = 1;
                fibMask.s.cos_valid_0     = 1;
                fibMask.s.dp_value_0          = 1;
                fibMask.s.dp_valid_0          = 1;
                fibMask.s.flowid_0        = 1;
                fibMask.s.flowid_valid_0      = 1;
                fibMask.s.groupid_0         = 1;
                fibMask.s.groupid_valid_0   = 1;
                fibMask.s.spt_mode_0          = 1;
                fibMask.s.no_drop_0       = 1;
            }
            else
            {
                fibMask.s.permit_1        = 1;
                fibMask.s.permit_valid_1      = 1;
                fibMask.s.premarked_1     = 1;
                fibMask.s.top_sc_ind_1    = 1;
                fibMask.s.top_vlan_cmd_1      = 1;
                fibMask.s.top_cmd_valid_1   = 1;
                fibMask.s.top_vlanid_1    = 1;
                fibMask.s.inner_sc_ind_1      = 1;
                fibMask.s.inner_vlan_cmd_1  = 1;
                fibMask.s.inner_cmd_valid_1 = 1;
                fibMask.s.inner_vlanid_1      = 1;
                fibMask.s.dot1p_1         = 1;
                fibMask.s.dot1p_valid_1   = 1;
                fibMask.s.dscp_1              = 1;
                fibMask.s.dscp_valid_1    = 1;
                fibMask.s.cos_1           = 1;
                fibMask.s.cos_valid_1     = 1;
                fibMask.s.dp_value_1          = 1;
                fibMask.s.dp_valid_1          = 1;
                fibMask.s.flowid_1        = 1;
                fibMask.s.flowid_valid_1      = 1;
                fibMask.s.groupid_1         = 1;
                fibMask.s.groupid_valid_1   = 1;
                fibMask.s.spt_mode_1          = 1;
                fibMask.s.no_drop_1       = 1;
            }
            aal_l2_cls_fib_set(0, rtl9607f_trap_loop_detect_config[port].clsIndex, fibMask, &fib);

            ret = aal_l2_cls_entry_status_get(0, rtl9607f_trap_loop_detect_config[port].clsIndex, &entryStatus);
            if(ret != CA_E_OK)
            {
                return RT_ERR_FAILED;
            }

            memset(&entryStatusMask, 0xff, sizeof(aal_l2_cls_entry_status_mask_t));

            if(rtl9607f_trap_loop_detect_config[port].clsSubIndex == 0)
                entryStatus.rule_0 = AAL_L2_CLS_KEY_2RULE_TYPE_INVALID;
            else
                entryStatus.rule_1 = AAL_L2_CLS_KEY_2RULE_TYPE_INVALID;

            if(entryStatus.rule_0 == AAL_L2_CLS_KEY_2RULE_TYPE_INVALID && entryStatus.rule_1 == AAL_L2_CLS_KEY_2RULE_TYPE_INVALID)
                entryStatus.valid = 0;

            ret = aal_l2_cls_entry_status_set(0, rtl9607f_trap_loop_detect_config[port].clsIndex, entryStatusMask, &entryStatus);
            if(ret != CA_E_OK)
            {
                return RT_ERR_FAILED;
            }

            rtl9607f_trap_loop_detect_config[port].clsIndex = 0xff;
        }
    }
    else
    {
        if(RT_ERR_OK != _dal_rtl9607f_trap_loopDetectClsIdx_get(port, &clsIndex, &clsSubIndex))
            return RT_ERR_FAILED;

        rtl9607f_trap_loop_detect_config[port].clsIndex = clsIndex;
        rtl9607f_trap_loop_detect_config[port].clsSubIndex = clsSubIndex;

        ret = aal_l2_cls_entry_status_get(0, clsIndex, &entryStatus);
        if(ret != CA_E_OK)
        {
            return RT_ERR_FAILED;
        }

        memset(&entryStatusMask, 0xff, sizeof(aal_l2_cls_entry_status_mask_t));

        entryStatus.valid = 1;
        entryStatus.entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

        if(clsSubIndex == 0)
            entryStatus.rule_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
        else
            entryStatus.rule_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;

        ret = aal_l2_cls_entry_status_set(0, clsIndex, entryStatusMask, &entryStatus);
        if(ret != CA_E_OK)
        {
            return RT_ERR_FAILED;
        }

        memset(&keyMask, 0xff, sizeof(aal_l2_cls_l2r_key_mask_t));
        memset(&key, 0, sizeof(aal_l2_cls_l2r_key_t));
        key.mac_field_ctrl  = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
        key.vlan_field_ctrl = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
        key.dot1p_ctrl      = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
        key.l4_port_field_ctrl = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
        key.ethernet_type_vld = 1;
        key.ethernet_type = RTL9607F_TRAP_LOOP_DETECT_EV2PT_ID;
        if(clsSubIndex == 0)
            aal_l2_cls_l2r_1st_key_set(0, rtl9607f_trap_loop_detect_config[port].clsIndex, keyMask, &key);
        else
            aal_l2_cls_l2r_2nd_key_set(0, rtl9607f_trap_loop_detect_config[port].clsIndex, keyMask, &key);

        memset(&fibMask, 0, sizeof(aal_l2_cls_fib_mask_t));
        memset(&fib, 0, sizeof(aal_l2_cls_fib_t));
        if(clsSubIndex == 0)
        {
            if(rtl9607f_trap_loop_detect_config[port].action == ACTION_TRAP2CPU)
            {
                fib.dp_valid_0 = 1;
                fib.dp_value_0 = AAL_LPORT_SPCL;
            }
            if(rtl9607f_trap_loop_detect_config[port].stormEn == ENABLED)
            {
                fib.groupid_valid_0 = 1;
                fib.groupid_0 = rtl9607f_trap_loop_detect_config[port].policerId;
            }
            if(rtl9607f_trap_loop_detect_config[port].priority != 0xff)
            {
                fib.cos_0 = rtl9607f_trap_loop_detect_config[port].priority;
                fib.cos_valid_0 = 1;
            }

            fibMask.s.permit_0        = 1;
            fibMask.s.permit_valid_0      = 1;
            fibMask.s.premarked_0     = 1;
            fibMask.s.top_sc_ind_0    = 1;
            fibMask.s.top_vlan_cmd_0      = 1;
            fibMask.s.top_cmd_valid_0   = 1;
            fibMask.s.top_vlanid_0    = 1;
            fibMask.s.inner_sc_ind_0      = 1;
            fibMask.s.inner_vlan_cmd_0  = 1;
            fibMask.s.inner_cmd_valid_0 = 1;
            fibMask.s.inner_vlanid_0      = 1;
            fibMask.s.dot1p_0         = 1;
            fibMask.s.dot1p_valid_0   = 1;
            fibMask.s.dscp_0              = 1;
            fibMask.s.dscp_valid_0    = 1;
            fibMask.s.cos_0           = 1;
            fibMask.s.cos_valid_0     = 1;
            fibMask.s.dp_value_0          = 1;
            fibMask.s.dp_valid_0          = 1;
            fibMask.s.flowid_0        = 1;
            fibMask.s.flowid_valid_0      = 1;
            fibMask.s.groupid_0         = 1;
            fibMask.s.groupid_valid_0   = 1;
            fibMask.s.spt_mode_0          = 1;
            fibMask.s.no_drop_0       = 1;
        }
        else
        {
            if(rtl9607f_trap_loop_detect_config[port].action == ACTION_TRAP2CPU)
            {
                fib.dp_valid_1 = 1;
                fib.dp_value_1 = AAL_LPORT_SPCL;
            }
            if(rtl9607f_trap_loop_detect_config[port].stormEn == ENABLED)
            {
                fib.groupid_valid_1 = 1;
                fib.groupid_1 = rtl9607f_trap_loop_detect_config[port].policerId;
            }
            if(rtl9607f_trap_loop_detect_config[port].priority != 0xff)
            {
                fib.cos_1 = rtl9607f_trap_loop_detect_config[port].priority;
                fib.cos_valid_1 = 1;
            }

            fibMask.s.permit_1        = 1;
            fibMask.s.permit_valid_1      = 1;
            fibMask.s.premarked_1     = 1;
            fibMask.s.top_sc_ind_1    = 1;
            fibMask.s.top_vlan_cmd_1      = 1;
            fibMask.s.top_cmd_valid_1   = 1;
            fibMask.s.top_vlanid_1    = 1;
            fibMask.s.inner_sc_ind_1      = 1;
            fibMask.s.inner_vlan_cmd_1  = 1;
            fibMask.s.inner_cmd_valid_1 = 1;
            fibMask.s.inner_vlanid_1      = 1;
            fibMask.s.dot1p_1         = 1;
            fibMask.s.dot1p_valid_1   = 1;
            fibMask.s.dscp_1              = 1;
            fibMask.s.dscp_valid_1    = 1;
            fibMask.s.cos_1           = 1;
            fibMask.s.cos_valid_1     = 1;
            fibMask.s.dp_value_1          = 1;
            fibMask.s.dp_valid_1          = 1;
            fibMask.s.flowid_1        = 1;
            fibMask.s.flowid_valid_1      = 1;
            fibMask.s.groupid_1         = 1;
            fibMask.s.groupid_valid_1   = 1;
            fibMask.s.spt_mode_1          = 1;
            fibMask.s.no_drop_1       = 1;
        }
        ret = aal_l2_cls_fib_set(0, clsIndex, fibMask, &fib);
        if(ret != CA_E_OK)
        {
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9607f_trap_init
 * Description:
 *      Initial the trap module of the specified device..
 * Input:
 *      unit - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9607f_trap_init(void)
{
    aal_ilpb_cfg_t ilpbCfg;
    aal_ilpb_cfg_msk_t ilpbCfgMask;
    aal_l2_specpkt_ethtype_encode_mask_t mask;
    aal_l2_specpkt_ethtype_encode_t config;
    uint8 i = 0, start = 0;
    /*ILPB special packet index setting*/
    for(i=0;i<sizeof(rtl9607f_trap_port_spb_idx_map);i++)
    {
        memset(&ilpbCfgMask,0,sizeof(aal_ilpb_cfg_msk_t));
        ilpbCfgMask.s.spcl_pkt_idx = 1;
        ilpbCfgMask.s.igr_cls_lookup_en = 1;
        ilpbCfgMask.s.cls_start = 1;
        ilpbCfgMask.s.cls_length = 1;
        memset(&ilpbCfg,0,sizeof(aal_ilpb_cfg_t));
        ilpbCfg.spcl_pkt_idx = rtl9607f_trap_port_spb_idx_map[i];
        ilpbCfg.igr_cls_lookup_en = 1;
        ilpbCfg.cls_start = start;
        ilpbCfg.cls_length = DAL_RTL9607F_TRAP_CLS_RULE_PER_PORT;
        start += DAL_RTL9607F_TRAP_CLS_RULE_PER_PORT;
        aal_port_ilpb_cfg_set(0, i, ilpbCfgMask, &ilpbCfg);
    }
    if(aal_l2_specpkt_arp_filter_set(0, 1, 0, 0) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
    }
    rtl9607f_trap_init = INIT_COMPLETED;

    for(i = 0; i < DAL_RTL9607F_TRAP_ETH_PORT_MAX; i++)
    {
        memset(&rtl9607f_trap_loop_detect_config[i], 0 ,sizeof(dal_9607f_trap_config_t));
        rtl9607f_trap_loop_detect_config[i].clsIndex = 0xff;
        rtl9607f_trap_loop_detect_config[i].priority = 0xff;
    }

    mask.s.ethertype = 1;
    mask.s.encoded_value = 1;
    config.ethertype = RTL9607F_TRAP_LOOP_DETECT_ETHERTYPE;
    config.encoded_value = RTL9607F_TRAP_LOOP_DETECT_EV2PT_ID;
    if(CA_E_OK != aal_l2_specpkt_ethtype_encode_set(0, RTL9607F_TRAP_LOOP_DETECT_EV2PT_ID,mask,&config) )
    {
        printk(KERN_ERR"Set ethertype fail, ethertype 0x%04x, entry id %d\n",RTL9607F_TRAP_LOOP_DETECT_ETHERTYPE, RTL9607F_TRAP_LOOP_DETECT_EV2PT_ID);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_trap_init */


/* Function Name:
 *      dal_rtl9607f_trap_oamPduAction_get
 * Description:
 *      Get forwarding action of trapped oam PDU on specified port.
 * Input:
 *      None.
 * Output:
 *      pAction - pointer to forwarding action of trapped oam PDU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
dal_rtl9607f_trap_oamPduAction_get(rtk_action_t *pAction)
{
    ca_status_t ret=CA_E_OK;
    aal_l2_specpkt_detect_t detect;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607f_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&detect,0,sizeof(aal_l2_specpkt_detect_t));

    if((ret = aal_l2_specpkt_detect_get(0, &detect)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }


    *pAction = ((detect.udf0_detect_enable == 1) ? ACTION_TRAP2CPU : ACTION_FORWARD);
    return RT_ERR_OK;
} /* end of dal_rtl9607f_trap_oamPduAction_get */

/* Function Name:
 *      dal_rtl9607f_trap_oamPduAction_set
 * Description:
 *      Set forwarding action of trapped oam PDU on specified port.
 * Input:
 *      action - forwarding action of trapped oam PDU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
dal_rtl9607f_trap_oamPduAction_set(rtk_action_t action)
{
    ca_status_t ret=CA_E_OK;
    aal_ilpb_cfg_t ilpb_cfg;
    aal_ilpb_cfg_msk_t ilpb_msk;
    aal_l2_specpkt_ctrl_t ctrl;
    aal_l2_specpkt_ctrl_mask_t ctrl_mask;
    aal_l2_specpkt_detect_t detect;
    aal_l2_specpkt_detect_mask_t detect_mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607f_trap_init);

    /* function body */
    memset(&ilpb_cfg,0,sizeof(aal_ippb_cfg_t));
    memset(&ilpb_msk,0,sizeof(aal_ilpb_cfg_msk_t));
    memset(&ctrl,0,sizeof(aal_l2_specpkt_ctrl_t));
    memset(&ctrl_mask,0,sizeof(aal_l2_specpkt_ctrl_mask_t));
    memset(&detect,0,sizeof(aal_l2_specpkt_detect_t));
    memset(&detect_mask,0,sizeof(aal_l2_specpkt_detect_mask_t));

    switch(action)
    {
        case ACTION_FORWARD:
            detect.udf0_detect_enable = 0;
            break;
        case ACTION_TRAP2CPU:
            detect.udf0_detect_enable = 1;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if((ret = aal_port_ilpb_cfg_get(0, 7, &ilpb_cfg)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    ilpb_msk.s.spcl_pkt_idx = 1;

    if((ret = aal_port_ilpb_cfg_set(0, 0x20, ilpb_msk, &ilpb_cfg)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    ctrl_mask.s.ldpid = 1;
    ctrl_mask.s.pkt_no_drop = 1;
    ctrl_mask.s.spcl_fwd = 1;
    ctrl_mask.s.cos = 1;
    ctrl_mask.s.learn_dis = 1;

    ctrl.ldpid = 0x10;
    ctrl.pkt_no_drop = 1;
    ctrl.spcl_fwd = 1;
    ctrl.cos = 7;
    ctrl.learn_dis =1;

    if((ret = aal_l2_specpkt_ctrl_set(0, ilpb_cfg.spcl_pkt_idx, AAL_L2_SPECPKT_TYPE_UDF_0, ctrl_mask, &ctrl)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    detect_mask.s.udf0_ethtype_enable = 1;
    detect_mask.s.udf0_ethtype = 1;
    detect_mask.s.udf0_mac_mask = 1;
    detect_mask.s.udf0_mac = 1;
    detect_mask.s.udf0_detect_enable = 1;

    detect.udf0_mac[0] = 0x00;
    detect.udf0_mac[1] = 0x13;
    detect.udf0_mac[2] = 0x25;
    detect.udf0_mac[3] = 0x00;
    detect.udf0_mac[4] = 0x00;
    detect.udf0_mac[5] = 0x00;
    detect.udf0_mac_mask[0] = 0xFF;
    detect.udf0_mac_mask[1] = 0xFF;
    detect.udf0_mac_mask[2] = 0xFF;
    detect.udf0_mac_mask[3] = 0xFF;
    detect.udf0_mac_mask[4] = 0xFF;
    detect.udf0_mac_mask[5] = 0x00;
    detect.udf0_ethtype = 0xfff1;
    detect.udf0_ethtype_enable = 1;

    if((ret = aal_l2_specpkt_detect_set(0, detect_mask, &detect)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_trap_oamPduAction_set */


/* Function Name:
 *      dal_rtl9607f_trap_omciAction_get
 * Description:
 *      Get forwarding action of trapped omci PDU on specified port.
 * Input:
 *      None.
 * Output:
 *      pAction - pointer to forwarding action of trapped omci PDU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
dal_rtl9607f_trap_omciAction_get(rtk_action_t *pAction)
{
    ca_status_t ret=CA_E_OK;
    aal_pdc_ctrl_t pdc_ctrl;

    memset(&pdc_ctrl, 0, sizeof(pdc_ctrl));
    if((ret = aal_pdc_ctrl_get(0, &pdc_ctrl)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    if(pdc_ctrl.omci_hp_en == 1)
        *pAction = ACTION_TRAP2CPU;
    else
        *pAction = ACTION_FORWARD;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_trap_omciAction_get */

/* Function Name:
 *      dal_rtl9607f_trap_omciAction_set
 * Description:
 *      Set forwarding action of trapped omci PDU on specified port.
 * Input:
 *      action - forwarding action of trapped omci PDU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
dal_rtl9607f_trap_omciAction_set(rtk_action_t action)
{
    ca_status_t ret=CA_E_OK;
    aal_pdc_pdc_map_mem_data_mask_t pdc_mask;
    aal_pdc_pdc_map_mem_data_t      pdc_cfg;
    aal_pdc_ctrl_mask_t             pdc_ctrl_mask;
    aal_pdc_ctrl_t                  pdc_ctrl;
    int gem_port;

    memset(&pdc_cfg, 0, sizeof(pdc_cfg));
    pdc_mask.wrd = ~0;

    memset(&pdc_ctrl, 0, sizeof(pdc_ctrl));
    pdc_ctrl_mask.wrd = 0;

    switch(action)
    {
        case ACTION_FORWARD:
            pdc_cfg.fe_bypass = 0;
            pdc_ctrl.omci_hp_en = 0;
            break;
        case ACTION_TRAP2CPU:
            pdc_cfg.fe_bypass = 1;
            pdc_ctrl.omci_hp_en = 1;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }
    pdc_cfg.ldpid     = 0x10;
    pdc_cfg.lspid     = 7;
    pdc_cfg.no_drop   = 1;
    pdc_cfg.pol_en    = 0;
    pdc_cfg.cos       = 6;
    pdc_cfg.pol_grp_id = 0;

    for(gem_port=0;gem_port<8;gem_port++)
    {
        pdc_cfg.pol_id = gem_port + 0x80;
        if((ret = aal_pdc_map_mem_set(0, gem_port, pdc_mask, &pdc_cfg)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return RT_ERR_FAILED;
        }
    }

    pdc_ctrl_mask.bf.omci_hp_cos = 1;
    pdc_ctrl_mask.bf.omci_hp_ldpid = 1;
    pdc_ctrl_mask.bf.omci_hp_en = 1;

    pdc_ctrl.omci_hp_cos = 7;
    pdc_ctrl.omci_hp_ldpid = 0x10;


    if((ret = aal_pdc_ctrl_set(0, pdc_ctrl_mask, &pdc_ctrl)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607f_trap_omciPduAction_set */


/* Function Name:
 *      dal_rtl9607f_trap_portIgmpMldPktAction_get
 * Description:
 *      Get forwarding action of trapped IGMP/MLD on specified port.
 * Input:
 *      port - specified port
 * Output:
 *      pAction - pointer to forwarding action of trapped IGMP/MLD
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portIgmpMldPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    return _dal_rtl9607f_trap_action_get(AAL_L2_SPECPKT_TYPE_IGMP_MLD, port, pAction);
}/* end of dal_rtl9607f_trap_portIgmpMldCtrlPktAction_get */

/* Function Name:
 *      dal_rtl9607f_trap_portIgmpMldPktAction_set
 * Description:
 *      Set forwarding action of trapped IGMP/MLD on specified port.
 * Input:
 *      port - specified port
 *      action - forwarding action of trapped IGMP/MLD
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portIgmpMldPktAction_set(rt_port_t port, rt_action_t action)
{
    return _dal_rtl9607f_trap_action_set(AAL_L2_SPECPKT_TYPE_IGMP_MLD, port, action);
}/* end of dal_rtl9607f_trap_portIgmpMldCtrlPktAction_set */


/* Function Name:
 *      dal_rtl9607f_trap_portDhcpPktAction_get
 * Description:
 *      Get forwarding action of trapped DHCP on specified port.
 * Input:
 *      port - specified port
 * Output:
 *      pAction - pointer to forwarding action of DHCP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portDhcpPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    return _dal_rtl9607f_trap_action_get(AAL_L2_SPECPKT_TYPE_DHCP, port, pAction);
}/* end of dal_rtl9607f_trap_portDhcpPktAction_get */

/* Function Name:
 *      dal_rtl9607f_trap_portDhcpPktAction_set
 * Description:
 *      Set forwarding action of trapped DHCP on specified port.
 * Input:
 *      port - specified port
 *      action - forwarding action of trapped DHCP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portDhcpPktAction_set(rt_port_t port, rt_action_t action)
{
    return _dal_rtl9607f_trap_action_set(AAL_L2_SPECPKT_TYPE_DHCP, port, action);
}/* end of dal_rtl9607f_trap_portDhcpPktAction_set */

/* Function Name:
 *      dal_rtl9607f_trap_portPppoePktAction_get
 * Description:
 *      Get forwarding action of trapped PPPoE on specified port.
 * Input:
 *      port - specified port
 * Output:
 *      pAction - pointer to forwarding action of trapped PPPoE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portPppoePktAction_get(rt_port_t port, rt_action_t *pAction)
{
    return _dal_rtl9607f_trap_action_get(AAL_L2_SPECPKT_TYPE_PPPOE_DISC, port, pAction);
}/* end of dal_rtl9607f_trap_portPppoePktAction_get */

/* Function Name:
 *      dal_rtl9607f_trap_portPppoePktAction_set
 * Description:
 *      Set forwarding action of trapped PPPoE on specified port.
 * Input:
 *      port - specified port
 *      action - forwarding action of trapped PPPoE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portPppoePktAction_set(rt_port_t port, rt_action_t action)
{
    return _dal_rtl9607f_trap_action_set(AAL_L2_SPECPKT_TYPE_PPPOE_DISC, port, action);
}/* end of dal_rtl9607f_trap_portPppoePktAction_set */

/* Function Name:
 *      dal_rtl9607f_trap_portStpPktAction_get
 * Description:
 *      Get forwarding action of trapped STP on specified port.
 * Input:
 *      port - specified port
 * Output:
 *      pAction - pointer to forwarding action of trapped STP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portStpPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    return _dal_rtl9607f_trap_action_get(AAL_L2_SPECPKT_TYPE_BPDU, port, pAction);
}/* end of dal_rtl9607f_trap_portStpPktAction_get */

/* Function Name:
 *      dal_rtl9607f_trap_portStpPktAction_set
 * Description:
 *      Set forwarding action of trapped STP on specified port.
 * Input:
 *      port - specified port
 *      action - forwarding action of trapped STP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portStpPktAction_set(rt_port_t port, rt_action_t action)
{
    return _dal_rtl9607f_trap_action_set(AAL_L2_SPECPKT_TYPE_BPDU, port, action);
}/* end of dal_rtl9607f_trap_portStpPktAction_set */

/* Function Name:
 *      dal_rtl9607f_trap_portHostPktAction_get
 * Description:
 *      Get forwarding action of trapped host packet on specified port.
 * Input:
 *      port - specified port
 * Output:
 *      pAction - pointer to forwarding action of trapped host packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portHostPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    return _dal_rtl9607f_trap_action_get(AAL_L2_SPECPKT_TYPE_MYMAC, port, pAction);
}/* end of dal_rtl9607f_trap_portHostPktAction_get */

/* Function Name:
 *      dal_rtl9607f_trap_portHostPktAction_set
 * Description:
 *      Set forwarding action of trapped host packet on specified port.
 * Input:
 *      action - forwarding action of trapped host packet
 *      port - specified port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portHostPktAction_set(rt_port_t port, rt_action_t action)
{
    return _dal_rtl9607f_trap_action_set(AAL_L2_SPECPKT_TYPE_MYMAC, port, action);
} /* end of dal_rtl9607f_trap_portHostPktAction_set */

/* Function Name:
 *      dal_rtl9607f_trap_portEthertypePktAction_get
 * Description:
 *      Get forwarding action of trapped special ethertype packet on specified port.
 * Input:
 *      port - specified port
 * Output:
 *      pAction - pointer to forwarding action of trapped special ethertype packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portEthertypePktAction_get(rt_port_t port, rt_action_t *pAction)
{
    return _dal_rtl9607f_trap_action_get(AAL_L2_SPECPKT_TYPE_UDF_0, port, pAction);
}/* end of dal_rtl9607f_trap_portEthertypePktAction_get */

/* Function Name:
 *      dal_rtl9607f_trap_portEthertypePktAction_set
 * Description:
 *      Set forwarding action of trapped special ethertype packet on specified port.
 * Input:
 *      port - specified port
 *      action - forwarding action of trapped special ethertype packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portEthertypePktAction_set(rt_port_t port, rt_action_t action)
{
    return _dal_rtl9607f_trap_action_set(AAL_L2_SPECPKT_TYPE_UDF_0, port, action);
}/* end of dal_rtl9607f_trap_portEthertypePktAction_set */

/* Function Name:
 *      dal_rtl9607f_trap_portArpPktAction_get
 * Description:
 *      Get forwarding action of trapped ARP on specified port.
 * Input:
 *      port - specified port
 * Output:
 *      pAction - pointer to forwarding action of trapped ARP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portArpPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    return _dal_rtl9607f_trap_action_get(AAL_L2_SPECPKT_TYPE_ARP, port, pAction);
}/* end of dal_rtl9607f_trap_portArpPktAction_get */

/* Function Name:
 *      dal_rtl9607f_trap_portArpPktAction_set
 * Description:
 *      Set forwarding action of trapped ARP on specified port.
 * Input:
 *      port - specified port
 *      action - forwarding action of trapped ARP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portArpPktAction_set(rt_port_t port, rt_action_t action)
{
    return _dal_rtl9607f_trap_action_set(AAL_L2_SPECPKT_TYPE_ARP, port, action);
}/* end of dal_rtl9607f_trap_portArpPktAction_set */

/* Function Name:
 *      dal_rtl9607f_trap_portLoopDetectPktAction_get
 * Description:
 *      Get the configuration about loop detect(0xfffa) packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of ARP packet
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - pAction is NULL pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portLoopDetectPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    aal_l2_cls_fib_t fib;
    ca_status_t ret=CA_E_OK;

    if(port >= DAL_RTL9607F_TRAP_ETH_PORT_MAX)
        return RT_ERR_PORT_ID;

    if(pAction == NULL)
        return RT_ERR_NULL_POINTER;

    *pAction = rtl9607f_trap_loop_detect_config[port].action;


    return RT_ERR_OK;
}/* end of dal_rtl9607f_trap_portLoopDetectPktAction_get */

/* Function Name:
 *      dal_rtl9607f_trap_portLoopDetectPktAction_set
 * Description:
 *      Set the configuration about loop detect(0xfffa) packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of ARP control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portLoopDetectPktAction_set(rt_port_t port, rt_action_t action)
{
    int32 ret;

    if(port >= DAL_RTL9607F_TRAP_ETH_PORT_MAX)
        return RT_ERR_PORT_ID;

    rtl9607f_trap_loop_detect_config[port].action = action;

    ret = _dal_rtl9607f_trap_loopDetectCls_update(port);

    return ret;
}/* end of dal_rtl9607f_trap_portLoopDetectPktAction_set */

/* Function Name:
 *      dal_rtl9607f_trap_portLoopDetectPktPolicer_set
 * Description:
 *      Set the configuration about loop detect(0xfffa) packets policer
 * Input:
 *      port       - The ingress port ID.
 *      enable     - enable status of policer
 *      flowId     - policer ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 *
 *
 */
extern int32
dal_rtl9607f_trap_portLoopDetectPktPolicer_set(rt_port_t port, rtk_enable_t enable, uint8 policerId)
{
    int32 ret;

    if(port >= DAL_RTL9607F_TRAP_ETH_PORT_MAX)
        return RT_ERR_PORT_ID;

    rtl9607f_trap_loop_detect_config[port].stormEn = enable;
    rtl9607f_trap_loop_detect_config[port].policerId = policerId;

    ret = _dal_rtl9607f_trap_loopDetectCls_update(port);

    return ret;
}/* end of dal_rtl9607f_trap_portLoopDetectPktPolicer_set */



/* Function Name:
 *      dal_rtl9607f_trap_portDot1xPktAction_get
 * Description:
 *      Get the configuration about Dot1x(0x888e) packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of ARP packet
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - pAction is NULL pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portDot1xPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    return _dal_rtl9607f_trap_action_get(AAL_L2_SPECPKT_TYPE_802_DOT1X, port, pAction);
}/* end of dal_rtl9607f_trap_portDot1xPktAction_get */


/* Function Name:
 *      dal_rtl9607f_trap_portDot1xPktAction_set
 * Description:
 *      Set the configuration about Dot1x(0x888e) packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of ARP control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_rtl9607f_trap_portDot1xPktAction_set(rt_port_t port, rt_action_t action)
{
    return _dal_rtl9607f_trap_action_set(AAL_L2_SPECPKT_TYPE_802_DOT1X, port, action);
}/* end of dal_rtl9607f_trap_portDot1xPktAction_set */



/* Function Name:
 *      dal_rtl9607f_trap_pattern_get
 * Description:
 *      Set the trapped pettern setting
 * Input:
 *      type - pattern type
 * Output:
 *      pattern - pattern data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 */
extern int32
dal_rtl9607f_trap_pattern_get(rt_trap_pattern_type_t type, rt_trap_pattern_t *pattern)
{
    aal_l2_specpkt_detect_t detect;
    ca_boolean_t filterDisable = 0;
    ca_uint32_t tip = 0, tipMask = 0;
    ca_status_t ret=CA_E_OK;

    memset(&detect,0,sizeof(aal_l2_specpkt_detect_t));

    switch(type)
    {
        case RT_TRAP_PATTERN_TYPE_HOST_MAC:
            ret = aal_l2_specpkt_detect_get(0, &detect);
            if(ret != CA_E_OK)
                return RT_ERR_FAILED;
            memcpy(&(pattern->hostMac),&(detect.my_mac),sizeof(ca_mac_addr_t));
            break;
        case RT_TRAP_PATTERN_TYPE_ETHERTYPE:
            ret = aal_l2_specpkt_detect_get(0, &detect);
            if(ret != CA_E_OK)
                return RT_ERR_FAILED;
            pattern->ethertype = detect.udf0_ethtype;
            break;
        case RT_TRAP_PATTERN_TYPE_ARP_TIP:
            ret = aal_l2_specpkt_arp_filter_get(0, &filterDisable, &tip, &tipMask);
            if(ret != CA_E_OK)
                return RT_ERR_FAILED;
            pattern->arpTip.filterEnable = filterDisable ? DISABLED : ENABLED;
            pattern->arpTip.tip = tip;
            pattern->arpTip.tipMask = ~tipMask;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/* end of dal_rtl9607f_trap_pattern_get */

/* Function Name:
 *      dal_rtl9607f_trap_pattern_set
 * Description:
 *      Set the trapped pettern setting
 * Input:
 *      type - pattern type
 *      pattern - pattern data
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 */
extern int32
dal_rtl9607f_trap_pattern_set(rt_trap_pattern_type_t type, rt_trap_pattern_t pattern)
{
    aal_l2_specpkt_detect_mask_t detectMask;
    aal_l2_specpkt_detect_t detect;
    ca_boolean_t filterDisable = 0;
    ca_uint32_t tip = 0, tipMask = 0;
    ca_status_t ret=CA_E_OK;

    memset(&detect,0,sizeof(aal_l2_specpkt_detect_t));
    memset(&detectMask,0,sizeof(aal_l2_specpkt_detect_mask_t));

    switch(type)
    {
        case RT_TRAP_PATTERN_TYPE_HOST_MAC:
            detectMask.s.my_mac=1;
            detectMask.s.my_mac1=1;

            memcpy(&(detect.my_mac),&(pattern.hostMac),sizeof(ca_mac_addr_t));
            memcpy(&(detect.my_mac1),&(pattern.hostMac),sizeof(ca_mac_addr_t));
            ret = aal_l2_specpkt_detect_set(0,  detectMask, &detect);
            if(ret != CA_E_OK)
                return RT_ERR_FAILED;
            break;
        case RT_TRAP_PATTERN_TYPE_ETHERTYPE:
            detectMask.s.udf0_detect_enable = 1;
            detectMask.s.udf0_ethtype_enable = 1;
            detectMask.s.udf0_ethtype = 1;
            detectMask.s.udf0_mac_mask = 1;

            detect.udf0_detect_enable = 1;
            detect.udf0_ethtype_enable = 1;
            memset(&detect.udf0_mac_mask,0,sizeof(ca_mac_addr_t));
            detect.udf0_ethtype = pattern.ethertype;
            ret = aal_l2_specpkt_detect_set(0,  detectMask, &detect);
            if(ret != CA_E_OK)
                return RT_ERR_FAILED;
            break;
        case RT_TRAP_PATTERN_TYPE_ARP_TIP:
            filterDisable = pattern.arpTip.filterEnable == ENABLED ? 0 : 1;
            tip = pattern.arpTip.tip;
            tipMask = ~(pattern.arpTip.tipMask);
            ret = aal_l2_specpkt_arp_filter_set(0,  filterDisable, tip, tipMask);
            if(ret != CA_E_OK)
                return RT_ERR_FAILED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/* end of dal_rtl9607f_trap_pattern_set */


/* Function Name:
 *      dal_rtl9607f_trap_priority_get
 * Description:
 *      Get the priority for specify protocol
 * Input:
 *      port        - The ingress port ID.
 *      type        - Protocol type.
 * Output:
 *      priority    - Trap priority
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid protocol type
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 *
 */
extern int32
dal_rtl9607f_trap_priority_get(rt_port_t port, rt_trap_protocol_t type, uint8 *pPri)
{
    aal_l2_specpkt_type_t specialType;
    aal_l2_specpkt_ctrl_t ctrl;
    ca_status_t ret=CA_E_OK;

    RT_PARAM_CHK(pPri == NULL, RT_ERR_FAILED);

    if(port > HAL_GET_PON_PORT())
        return RT_ERR_PORT_ID;

    if(type == RT_TRAP_PROTOCOL_LOOP_DETECT)
    {
        if(rtl9607f_trap_loop_detect_config[port].priority == 0xff)
            *pPri = 0;
        else
            *pPri = rtl9607f_trap_loop_detect_config[port].priority;
        return RT_ERR_OK;
    }

    switch(type)
    {
        case RT_TRAP_PROTOCOL_IGMP:
            specialType = AAL_L2_SPECPKT_TYPE_IGMP_MLD;
            break;
        case RT_TRAP_PROTOCOL_DHCP:
            specialType = AAL_L2_SPECPKT_TYPE_DHCP;
            break;
        case RT_TRAP_PROTOCOL_PPPOE:
            specialType = AAL_L2_SPECPKT_TYPE_PPPOE_DISC;
            break;
        case RT_TRAP_PROTOCOL_STP:
            specialType = AAL_L2_SPECPKT_TYPE_BPDU;
            break;
        case RT_TRAP_PROTOCOL_HOST:
            specialType = AAL_L2_SPECPKT_TYPE_MYMAC;
            break;
        case RT_TRAP_PROTOCOL_ETHERTYPE:
            specialType = AAL_L2_SPECPKT_TYPE_UDF_0;
            break;
        case RT_TRAP_PROTOCOL_ARP:
            specialType = AAL_L2_SPECPKT_TYPE_ARP;
            break;
        case RT_TRAP_PROTOCOL_DOT1X:
            specialType = AAL_L2_SPECPKT_TYPE_802_DOT1X;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    memset(&ctrl,0,sizeof(aal_l2_specpkt_ctrl_t));
    ret = aal_l2_specpkt_ctrl_get(0, rtl9607f_trap_port_spb_idx_map[port], specialType, &ctrl);
    if(ret != CA_E_OK)
        return RT_ERR_FAILED;

    *pPri = ctrl.cos;

    return ret;
}/* end of dal_rtl9607f_trap_priority_get */

/* Function Name:
 *      dal_rtl9607f_trap_priority_set
 * Description:
 *      Set the priority for specify protocol
 * Input:
 *      port        - The ingress port ID.
 *      type        - Protocol type.
 *      priority    - Trap priority
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid protocol type
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 *
 */
extern int32
dal_rtl9607f_trap_priority_set(rt_port_t port, rt_trap_protocol_t type, uint8 pri)
{
    aal_l2_specpkt_type_t specialType;
    aal_l2_specpkt_ctrl_t ctrl;
    aal_l2_specpkt_ctrl_mask_t ctrlMask;
    ca_status_t ret=CA_E_OK;

    RT_PARAM_CHK(pri > 7, RT_ERR_FAILED);

    if(port > HAL_GET_PON_PORT())
        return RT_ERR_PORT_ID;

    if(type == RT_TRAP_PROTOCOL_LOOP_DETECT)
    {
        rtl9607f_trap_loop_detect_config[port].priority = pri;
        return _dal_rtl9607f_trap_loopDetectCls_update(port);
    }

    switch(type)
    {
        case RT_TRAP_PROTOCOL_IGMP:
            specialType = AAL_L2_SPECPKT_TYPE_IGMP_MLD;
            break;
        case RT_TRAP_PROTOCOL_DHCP:
            specialType = AAL_L2_SPECPKT_TYPE_DHCP;
            break;
        case RT_TRAP_PROTOCOL_PPPOE:
            specialType = AAL_L2_SPECPKT_TYPE_PPPOE_DISC;
            break;
        case RT_TRAP_PROTOCOL_STP:
            specialType = AAL_L2_SPECPKT_TYPE_BPDU;
            break;
        case RT_TRAP_PROTOCOL_HOST:
            specialType = AAL_L2_SPECPKT_TYPE_MYMAC;
            break;
        case RT_TRAP_PROTOCOL_ETHERTYPE:
            specialType = AAL_L2_SPECPKT_TYPE_UDF_0;
            break;
        case RT_TRAP_PROTOCOL_ARP:
            specialType = AAL_L2_SPECPKT_TYPE_ARP;
            break;
        case RT_TRAP_PROTOCOL_DOT1X:
            specialType = AAL_L2_SPECPKT_TYPE_802_DOT1X;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    memset(&ctrl,0,sizeof(aal_l2_specpkt_ctrl_t));
    memset(&ctrlMask,0,sizeof(aal_l2_specpkt_ctrl_mask_t));
    ctrlMask.s.cos = 1;
    ctrl.cos = pri;

    ret = aal_l2_specpkt_ctrl_set(0, rtl9607f_trap_port_spb_idx_map[port], specialType, ctrlMask, &ctrl);
    if(ret != CA_E_OK)
        return RT_ERR_FAILED;

    return ret;
}/* end of dal_rtl9607f_trap_priority_set */

