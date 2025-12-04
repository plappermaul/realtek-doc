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
#include <dal/luna_g3/dal_luna_g3.h>
#include <dal/luna_g3/dal_luna_g3_trap.h>
#include <osal/time.h>

#include "cortina-api/include/port.h"
#include "cortina-api/include/special_packet.h"
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32 luna_g3_trap_init = INIT_NOT_COMPLETED;

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_luna_g3_trap_init
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
dal_luna_g3_trap_init(void)
{
    int32 ret=RT_ERR_OK;
    
    luna_g3_trap_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_luna_g3_trap_init */

/* Function Name:
 *      dal_luna_g3_trap_portIgmpMldCtrlPktAction_get
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
dal_luna_g3_trap_portIgmpMldCtrlPktAction_get(rtk_port_t port, rtk_trap_igmpMld_type_t igmpMldType, rtk_action_t *pAction)
{
    ca_status_t ret=CA_E_OK;
    ca_pkt_type_t packet_type;
    ca_boolean_t forward_to_cpu;
    ca_port_id_t cpu_port;
    ca_uint8_t priority;
    ca_uint16_t flow_id;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "port=%d,igmpMldType=%d",port, igmpMldType);

    /* check Init status */
    RT_INIT_CHK(luna_g3_trap_init);

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

    /* function body */
    if((ret = ca_special_packet_get(0, CA_PORT_ID(CA_PORT_TYPE_ETHERNET, port),
                           packet_type, &forward_to_cpu,
                           &cpu_port,
                           &priority, &flow_id)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    *pAction = ((forward_to_cpu == 1) ? ACTION_TRAP2CPU : ACTION_FORWARD);

    return RT_ERR_OK;
}   /* end of dal_luna_g3_trap_portIgmpMldCtrlPktAction_get */

/* Function Name:
 *      dal_luna_g3_trap_portIgmpMldCtrlPktAction_set
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
dal_luna_g3_trap_portIgmpMldCtrlPktAction_set(rtk_port_t port, rtk_trap_igmpMld_type_t igmpMldType, rtk_action_t action)
{
    ca_status_t ret=CA_E_OK;
    ca_pkt_type_t packet_type;
    ca_boolean_t forward_to_cpu;
    ca_boolean_t act_handle_valid;
    ca_classifier_handle_t act_handle;

    act_handle_valid = FALSE;
    act_handle.flow_id = CA_FLOW_ID_INVALID;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "port=%d,igmpMldType=%d,action=%d",port, igmpMldType, action);

    /* check Init status */
    RT_INIT_CHK(luna_g3_trap_init);

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
                           0, act_handle_valid, act_handle)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_luna_g3_trap_portIgmpMldCtrlPktAction_set */
