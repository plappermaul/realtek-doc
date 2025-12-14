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
#include <dal/ca8279/dal_ca8279.h>
#include <dal/ca8279/dal_ca8279_trap.h>
#include <osal/time.h>

#include "cortina-api/include/port.h"
#include "cortina-api/include/special_packet.h"
#include "aal_port.h"
#include "aal_l2_specpkt.h"

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32 ca8279_trap_init = INIT_NOT_COMPLETED;

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_ca8279_trap_init
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
dal_ca8279_trap_init(void)
{   
    ca8279_trap_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_ca8279_trap_init */

/* Function Name:
 *      dal_ca8279_trap_portIgmpMldCtrlPktAction_get
 * Description:
 *      Get the configuration about MLD control packets Action
 * Input:
 *      port        - The ingress port ID.
 *      igmpMldType - IGMP/MLD protocol type;
 * Output:
 *      pAction     - Action of IGMP/MLD control packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
dal_ca8279_trap_portIgmpMldCtrlPktAction_get(rtk_port_t port, rtk_trap_igmpMld_type_t igmpMldType, rtk_action_t *pAction)
{
    ca_status_t ret=CA_E_OK;
    ca_pkt_type_t packet_type;
    ca_boolean_t forward_to_cpu;
    ca_port_id_t cpu_port;
    ca_uint8_t priority;
    ca_uint16_t flow_id;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "port=%d,igmpMldType=%d",port, igmpMldType);

    /* check Init status */
    RT_INIT_CHK(ca8279_trap_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((IGMPMLD_TYPE_END <=igmpMldType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(igmpMldType)
    {
        case IGMPMLD_TYPE_IGMPV1:
        case IGMPMLD_TYPE_IGMPV2:
        case IGMPMLD_TYPE_IGMPV3:
            packet_type=CA_PKT_TYPE_IGMP;
            break;
        case IGMPMLD_TYPE_MLDV1:
        case IGMPMLD_TYPE_MLDV2:
            packet_type=CA_PKT_TYPE_ICMPV6_MLD;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }
#ifdef R0.7.19.0_FIX
    /* function body */
    if((ret = ca_special_packet_get(0, CA_PORT_ID(CA_PORT_TYPE_ETHERNET, port),
                           packet_type, &forward_to_cpu,
                           &cpu_port,
                           &priority, &flow_id)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }
#endif
    *pAction = ((forward_to_cpu == 1) ? ACTION_TRAP2CPU : ACTION_FORWARD);

    return RT_ERR_OK;
}   /* end of dal_ca8279_trap_portIgmpMldCtrlPktAction_get */

/* Function Name:
 *      dal_ca8279_trap_portIgmpMldCtrlPktAction_set
 * Description:
 *      Set the configuration about MLD control packets Action
 * Input:
 *      port        - The ingress port ID.
 *      igmpMldType - IGMP/MLD protocol type;
 *      action      - Action of IGMP/MLD control packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32
dal_ca8279_trap_portIgmpMldCtrlPktAction_set(rtk_port_t port, rtk_trap_igmpMld_type_t igmpMldType, rtk_action_t action)
{
    ca_status_t ret=CA_E_OK;
    ca_pkt_type_t packet_type;
    ca_boolean_t forward_to_cpu;
    ca_special_packet_option_t act_handle;

    act_handle.mask.action_handle = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "port=%d,igmpMldType=%d,action=%d",port, igmpMldType, action);

    /* check Init status */
    RT_INIT_CHK(ca8279_trap_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((IGMPMLD_TYPE_END <=igmpMldType), RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_END <=action), RT_ERR_INPUT);

    switch(igmpMldType)
    {
        case IGMPMLD_TYPE_IGMPV1:
        case IGMPMLD_TYPE_IGMPV2:
        case IGMPMLD_TYPE_IGMPV3:
            packet_type=CA_PKT_TYPE_IGMP;
            break;
        case IGMPMLD_TYPE_MLDV1:
        case IGMPMLD_TYPE_MLDV2:
            packet_type=CA_PKT_TYPE_ICMPV6_MLD;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    switch(action)
    {
        case ACTION_FORWARD:
            forward_to_cpu = 0;
            break;
        case ACTION_TRAP2CPU:
            forward_to_cpu = 1;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    /* function body */
    if((ret = ca_special_packet_set(0, CA_PORT_ID(CA_PORT_TYPE_ETHERNET, port),
                           packet_type, forward_to_cpu,
                           CA_PORT_ID(CA_PORT_TYPE_CPU, CA_PORT_ID_CPU0),
                           0, &act_handle)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8279_trap_portIgmpMldCtrlPktAction_set */

/* Function Name:
 *      dal_ca8279_trap_oamPduAction_get
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
dal_ca8279_trap_oamPduAction_get(rtk_action_t *pAction)
{
    ca_status_t ret=CA_E_OK;
    aal_l2_specpkt_detect_t detect;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ca8279_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&detect,0,sizeof(aal_l2_specpkt_detect_t));

    if((ret = aal_l2_specpkt_detect_get(0, &detect)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }


    *pAction = ((detect.udf0_ethtype_enable == 1) ? ACTION_TRAP2CPU : ACTION_FORWARD);
    return RT_ERR_OK;
} /* end of dal_ca8279_trap_oamPduAction_get */

/* Function Name:
 *      dal_ca8279_trap_oamPduAction_set
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
dal_ca8279_trap_oamPduAction_set(rtk_action_t action)
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
    RT_INIT_CHK(ca8279_trap_init);

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
    ctrl_mask.s.cos = 7;
    ctrl_mask.s.learn_dis = 1;

    ctrl.ldpid = 0x10;
    ctrl.pkt_no_drop = 1;
    ctrl.spcl_fwd = 0;
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
} /* end of dal_ca8279_trap_oamPduAction_set */
