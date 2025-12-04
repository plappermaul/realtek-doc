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
 * $Revision: 79393 $
 * $Date: 2017-06-05 13:37:38 +0800 (Mon, 05 Jun 2017) $
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
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_trap.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Module Name    : Trap                                    */
/* Sub-module Name: Configuration for traping packet to CPU */
/* Function Name:
 *      rt_trap_init
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
rt_trap_init(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->trap_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->trap_init();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_init */

/* Function Name:
 *      rt_trap_portIgmpMldCtrlPktAction_get
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
rt_trap_portIgmpMldCtrlPktAction_get(rt_port_t port, rt_trap_igmpMld_type_t igmpMldType, rt_action_t *pAction)
{
    int32   ret;
#ifdef CONFIG_RTL8277C_SERIES
    /*TBD*/
    if(!HAL_IS_PORT_EXIST(port))
        ret = RT_ERR_PORT_ID;
    else if(IGMPMLD_TYPE_END <=igmpMldType)
        ret = RT_ERR_INPUT;
    else if(NULL == pAction)
        ret = RT_ERR_NULL_POINTER;
    else
        ret = RT_ERR_OK;

    if(RT_ERR_OK != ret)
        return ret;

    if (NULL == RT_MAPPER->rt_trap_portIgmpMldPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portIgmpMldPktAction_get(port, pAction);
    RTK_API_UNLOCK();
#else
    rtk_action_t action;

    /* function body */
    if (NULL == RT_MAPPER->trap_portIgmpMldCtrlPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->trap_portIgmpMldCtrlPktAction_get(port, igmpMldType, &action);
    RTK_API_UNLOCK();
    if(ret == RT_ERR_OK)
    {
        switch(action)
        {
            case ACTION_FORWARD:
                *pAction = RT_ACTION_FORWARD;
                break;
            case ACTION_DROP:
                *pAction = RT_ACTION_DROP;
                break;
            case ACTION_TRAP2CPU:
                *pAction = RT_ACTION_TRAP2CPU;
                break;
            case ACTION_COPY2CPU:
                *pAction = RT_ACTION_COPY2CPU;
                break;
            case ACTION_TO_GUESTVLAN:
                *pAction = RT_ACTION_TO_GUESTVLAN;
                break;
            case ACTION_FLOOD_IN_VLAN:
                *pAction = RT_ACTION_FLOOD_IN_VLAN;
                break;
            case ACTION_FLOOD_IN_ALL_PORT:
                *pAction = RT_ACTION_FLOOD_IN_ALL_PORT;
                break;
            case ACTION_FLOOD_IN_ROUTER_PORTS:
                *pAction = RT_ACTION_FLOOD_IN_ROUTER_PORTS;
                break;
            case ACTION_FORWARD_EXCLUDE_CPU:
                *pAction = RT_ACTION_FORWARD_EXCLUDE_CPU;
                break;
            case ACTION_DROP_EXCLUDE_RMA:
                *pAction = RT_ACTION_DROP_EXCLUDE_RMA;
                break;
            case ACTION_FOLLOW_FB:
                *pAction = RT_ACTION_FOLLOW_FB;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
#endif
    return ret;
}   /* end of rt_trap_portIgmpMldCtrlPktAction_get */

/* Function Name:
 *      rt_trap_portIgmpMldCtrlPktAction_set
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
rt_trap_portIgmpMldCtrlPktAction_set(rt_port_t port, rt_trap_igmpMld_type_t igmpMldType, rt_action_t action)
{
    int32   ret;
#ifdef CONFIG_RTL8277C_SERIES
    /*TBD*/
    if(!HAL_IS_PORT_EXIST(port))
        ret = RT_ERR_PORT_ID;
    else if(IGMPMLD_TYPE_END <=igmpMldType)
        ret = RT_ERR_INPUT;
    else if(ACTION_END <= action)
        ret = RT_ERR_INPUT;
    else
        ret = RT_ERR_OK;

    if(RT_ERR_OK != ret)
        return ret;

    if (NULL == RT_MAPPER->rt_trap_portIgmpMldPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portIgmpMldPktAction_set(port, action);
    RTK_API_UNLOCK();
#else
    rtk_action_t rtk_action;

    /* function body */
    if (NULL == RT_MAPPER->trap_portIgmpMldCtrlPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    switch(action)
    {
        case RT_ACTION_FORWARD:
            rtk_action = ACTION_FORWARD;
            break;
        case RT_ACTION_DROP:
            rtk_action = ACTION_DROP;
            break;
        case RT_ACTION_TRAP2CPU:
            rtk_action = ACTION_TRAP2CPU;
            break;
        case RT_ACTION_COPY2CPU:
            rtk_action = ACTION_COPY2CPU;
            break;
        case RT_ACTION_TO_GUESTVLAN:
            rtk_action = ACTION_TO_GUESTVLAN;
            break;
        case RT_ACTION_FLOOD_IN_VLAN:
            rtk_action = ACTION_FLOOD_IN_VLAN;
            break;
        case RT_ACTION_FLOOD_IN_ALL_PORT:
            rtk_action = ACTION_FLOOD_IN_ALL_PORT;
            break;
        case RT_ACTION_FLOOD_IN_ROUTER_PORTS:
            rtk_action = ACTION_FLOOD_IN_ROUTER_PORTS;
            break;
        case RT_ACTION_FORWARD_EXCLUDE_CPU:
            rtk_action = ACTION_FORWARD_EXCLUDE_CPU;
            break;
        case RT_ACTION_DROP_EXCLUDE_RMA:
            rtk_action = ACTION_DROP_EXCLUDE_RMA;
            break;
        case RT_ACTION_FOLLOW_FB:
            rtk_action = ACTION_FOLLOW_FB;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->trap_portIgmpMldCtrlPktAction_set(port, igmpMldType, rtk_action);
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_trap_portIgmpMldCtrlPktAction_set */

/* Module Name    : Trap       */
/* Sub-module Name: OAM packet */

/* Function Name:
 *      rt_trap_oamPduAction_get
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
rt_trap_oamPduAction_get(rt_action_t *pAction)
{
    int32   ret;
    rtk_action_t action;

    /* function body */
    if (NULL == RT_MAPPER->trap_oamPduAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->trap_oamPduAction_get(&action);
    RTK_API_UNLOCK();
    if(ret == RT_ERR_OK)
    {
        switch(action)
        {
            case ACTION_FORWARD:
                *pAction = RT_ACTION_FORWARD;
                break;
            case ACTION_DROP:
                *pAction = RT_ACTION_DROP;
                break;
            case ACTION_TRAP2CPU:
                *pAction = RT_ACTION_TRAP2CPU;
                break;
            case ACTION_COPY2CPU:
                *pAction = RT_ACTION_COPY2CPU;
                break;
            case ACTION_TO_GUESTVLAN:
                *pAction = RT_ACTION_TO_GUESTVLAN;
                break;
            case ACTION_FLOOD_IN_VLAN:
                *pAction = RT_ACTION_FLOOD_IN_VLAN;
                break;
            case ACTION_FLOOD_IN_ALL_PORT:
                *pAction = RT_ACTION_FLOOD_IN_ALL_PORT;
                break;
            case ACTION_FLOOD_IN_ROUTER_PORTS:
                *pAction = RT_ACTION_FLOOD_IN_ROUTER_PORTS;
                break;
            case ACTION_FORWARD_EXCLUDE_CPU:
                *pAction = RT_ACTION_FORWARD_EXCLUDE_CPU;
                break;
            case ACTION_DROP_EXCLUDE_RMA:
                *pAction = RT_ACTION_DROP_EXCLUDE_RMA;
                break;
            case ACTION_FOLLOW_FB:
                *pAction = RT_ACTION_FOLLOW_FB;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
    return ret;
}   /* end of rt_trap_oamPduAction_get */

/* Function Name:
 *      rt_trap_oamPduAction_set
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
rt_trap_oamPduAction_set(rt_action_t action)
{
    int32   ret;
    rtk_action_t rtk_action;

    /* function body */
    if (NULL == RT_MAPPER->trap_oamPduAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    switch(action)
    {
        case RT_ACTION_FORWARD:
            rtk_action = ACTION_FORWARD;
            break;
        case RT_ACTION_DROP:
            rtk_action = ACTION_DROP;
            break;
        case RT_ACTION_TRAP2CPU:
            rtk_action = ACTION_TRAP2CPU;
            break;
        case RT_ACTION_COPY2CPU:
            rtk_action = ACTION_COPY2CPU;
            break;
        case RT_ACTION_TO_GUESTVLAN:
            rtk_action = ACTION_TO_GUESTVLAN;
            break;
        case RT_ACTION_FLOOD_IN_VLAN:
            rtk_action = ACTION_FLOOD_IN_VLAN;
            break;
        case RT_ACTION_FLOOD_IN_ALL_PORT:
            rtk_action = ACTION_FLOOD_IN_ALL_PORT;
            break;
        case RT_ACTION_FLOOD_IN_ROUTER_PORTS:
            rtk_action = ACTION_FLOOD_IN_ROUTER_PORTS;
            break;
        case RT_ACTION_FORWARD_EXCLUDE_CPU:
            rtk_action = ACTION_FORWARD_EXCLUDE_CPU;
            break;
        case RT_ACTION_DROP_EXCLUDE_RMA:
            rtk_action = ACTION_DROP_EXCLUDE_RMA;
            break;
        case RT_ACTION_FOLLOW_FB:
            rtk_action = ACTION_FOLLOW_FB;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->trap_oamPduAction_set(rtk_action);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_oamPduAction_set */

/* Function Name:
 *      rt_trap_omciAction_get
 * Description:
 *      Get forwarding action of OMCI.
 * Input:
 *      None.
 * Output:
 *      pAction - pointer to forwarding action of OMCI
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
rt_trap_omciAction_get(rt_action_t *pAction)
{
    int32   ret;
#ifdef CONFIG_LUNA_G3_SERIES
    rtk_action_t action;

    /* function body */
    if (NULL == RT_MAPPER->trap_omciAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->trap_omciAction_get(&action);
    RTK_API_UNLOCK();
    if(ret == RT_ERR_OK)
    {
        switch(action)
        {
            case ACTION_FORWARD:
                *pAction = RT_ACTION_FORWARD;
                break;
            case ACTION_DROP:
                *pAction = RT_ACTION_DROP;
                break;
            case ACTION_TRAP2CPU:
                *pAction = RT_ACTION_TRAP2CPU;
                break;
            case ACTION_COPY2CPU:
                *pAction = RT_ACTION_COPY2CPU;
                break;
            case ACTION_TO_GUESTVLAN:
                *pAction = RT_ACTION_TO_GUESTVLAN;
                break;
            case ACTION_FLOOD_IN_VLAN:
                *pAction = RT_ACTION_FLOOD_IN_VLAN;
                break;
            case ACTION_FLOOD_IN_ALL_PORT:
                *pAction = RT_ACTION_FLOOD_IN_ALL_PORT;
                break;
            case ACTION_FLOOD_IN_ROUTER_PORTS:
                *pAction = RT_ACTION_FLOOD_IN_ROUTER_PORTS;
                break;
            case ACTION_FORWARD_EXCLUDE_CPU:
                *pAction = RT_ACTION_FORWARD_EXCLUDE_CPU;
                break;
            case ACTION_DROP_EXCLUDE_RMA:
                *pAction = RT_ACTION_DROP_EXCLUDE_RMA;
                break;
            case ACTION_FOLLOW_FB:
                *pAction = RT_ACTION_FOLLOW_FB;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
#else
    *pAction = RT_ACTION_TRAP2CPU;
    ret = RT_ERR_OK;
#endif
    return ret;
}   /* end of rt_trap_omciAction_get */

/* Function Name:
 *      rt_trap_omciAction_set
 * Description:
 *      Set forwarding action of OMCI.
 * Input:
 *      action - forwarding action of OMCI
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
rt_trap_omciAction_set(rt_action_t action)
{

    int32   ret;
#ifdef CONFIG_LUNA_G3_SERIES
    rtk_action_t rtk_action;

    if (NULL == RT_MAPPER->trap_omciAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    switch(action)
    {
        case RT_ACTION_FORWARD:
            rtk_action = ACTION_FORWARD;
            break;
        case RT_ACTION_DROP:
            rtk_action = ACTION_DROP;
            break;
        case RT_ACTION_TRAP2CPU:
            rtk_action = ACTION_TRAP2CPU;
            break;
        case RT_ACTION_COPY2CPU:
            rtk_action = ACTION_COPY2CPU;
            break;
        case RT_ACTION_TO_GUESTVLAN:
            rtk_action = ACTION_TO_GUESTVLAN;
            break;
        case RT_ACTION_FLOOD_IN_VLAN:
            rtk_action = ACTION_FLOOD_IN_VLAN;
            break;
        case RT_ACTION_FLOOD_IN_ALL_PORT:
            rtk_action = ACTION_FLOOD_IN_ALL_PORT;
            break;
        case RT_ACTION_FLOOD_IN_ROUTER_PORTS:
            rtk_action = ACTION_FLOOD_IN_ROUTER_PORTS;
            break;
        case RT_ACTION_FORWARD_EXCLUDE_CPU:
            rtk_action = ACTION_FORWARD_EXCLUDE_CPU;
            break;
        case RT_ACTION_DROP_EXCLUDE_RMA:
            rtk_action = ACTION_DROP_EXCLUDE_RMA;
            break;
        case RT_ACTION_FOLLOW_FB:
            rtk_action = ACTION_FOLLOW_FB;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->trap_omciAction_set(rtk_action);
    RTK_API_UNLOCK();
#else
    if(action >= RT_ACTION_END)
        ret = RT_ERR_FAILED;
    ret = RT_ERR_OK;
#endif
    return ret;
}   /* end of rt_trap_omciAction_set */

/* Function Name:
 *      rt_trap_portDhcpPktAction_get
 * Description:
 *      Get the configuration about DHCP packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of DHCP packet
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
rt_trap_portDhcpPktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portDhcpPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portDhcpPktAction_get(port, pAction);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portDhcpPktAction_get */

/* Function Name:
 *      rt_trap_portDhcpPktAction_set
 * Description:
 *      Set the configuration about DHCP packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of DHCP control packet
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
rt_trap_portDhcpPktAction_set(rt_port_t port,  rt_action_t action)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portDhcpPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portDhcpPktAction_set(port, action);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portDhcpPktAction_set */


/* Function Name:
 *      rt_trap_portPppoePktAction_get
 * Description:
 *      Get the configuration about PPPoE packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of PPPoE packet
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
rt_trap_portPppoePktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portPppoePktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portPppoePktAction_get(port, pAction);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portPppoePktAction_get */

/* Function Name:
 *      rt_trap_portPppoePktAction_set
 * Description:
 *      Set the configuration about PPPoE packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of PPPoE control packet
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
rt_trap_portPppoePktAction_set(rt_port_t port,  rt_action_t action)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portPppoePktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portPppoePktAction_set(port, action);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portPppoePktAction_set */


/* Function Name:
 *      rt_trap_portStpPktAction_get
 * Description:
 *      Get the configuration about STP packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of STP packet
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
rt_trap_portStpPktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portStpPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portStpPktAction_get(port, pAction);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portStpPktAction_get */

/* Function Name:
 *      rt_trap_portStpPktAction_set
 * Description:
 *      Set the configuration about STP packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of STP control packet
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
rt_trap_portStpPktAction_set(rt_port_t port,  rt_action_t action)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portStpPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portStpPktAction_set(port, action);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portStpPktAction_set */


/* Function Name:
 *      rt_trap_portHostPktAction_get
 * Description:
 *      Get the configuration about host packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of host packet
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
rt_trap_portHostPktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portHostPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portHostPktAction_get(port, pAction);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portHostPktAction_get */

/* Function Name:
 *      rt_trap_portHostPktAction_set
 * Description:
 *      Set the configuration about host packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of host control packet
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
rt_trap_portHostPktAction_set(rt_port_t port,  rt_action_t action)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portHostPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portHostPktAction_set(port, action);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portHostPktAction_set */


/* Function Name:
 *      rt_trap_portEthertypePktAction_get
 * Description:
 *      Get the configuration about special ethertype packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of special ethertype packet
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
rt_trap_portEthertypePktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portEthertypePktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portEthertypePktAction_get(port, pAction);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portEthertypePktAction_get */

/* Function Name:
 *      rt_trap_portEthertypePktAction_set
 * Description:
 *      Set the configuration about special ethertype packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of special ethertype control packet
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
rt_trap_portEthertypePktAction_set(rt_port_t port,  rt_action_t action)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portEthertypePktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portEthertypePktAction_set(port, action);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portEthertypePktAction_set */


/* Function Name:
 *      rt_trap_portArpPktAction_get
 * Description:
 *      Get the configuration about ARP packets Action
 * Input:
 *      port        - The ingress port ID.
 * Output:
 *      pAction     - Action of ARP packet
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
rt_trap_portArpPktAction_get(rt_port_t port,             rt_action_t *pAction)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portArpPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portArpPktAction_get(port, pAction);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portArpPktAction_get */

/* Function Name:
 *      rt_trap_portArpPktAction_set
 * Description:
 *      Set the configuration about ARP packets Action
 * Input:
 *      port        - The ingress port ID.
 *      action      - Action of ARP control packet
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
rt_trap_portArpPktAction_set(rt_port_t port,  rt_action_t action)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portArpPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portArpPktAction_set(port, action);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portArpPktAction_set */


/* Function Name:
 *      rt_trap_portLoopDetectPktAction_get
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
int32
rt_trap_portLoopDetectPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portLoopDetectPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portLoopDetectPktAction_get(port, pAction);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portLoopDetectPktAction_get */

/* Function Name:
 *      rt_trap_portLoopDetectPktAction_set
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
int32
rt_trap_portLoopDetectPktAction_set(rt_port_t port, rt_action_t action)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portLoopDetectPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portLoopDetectPktAction_set(port, action);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portLoopDetectPktAction_set */

/* Function Name:
 *      rt_trap_portDot1xPktAction_get
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
int32
rt_trap_portDot1xPktAction_get(rt_port_t port, rt_action_t *pAction)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portDot1xPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portDot1xPktAction_get(port, pAction);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portDot1xPktAction_get */

/* Function Name:
 *      rt_trap_portDot1xPktAction_set
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
int32
rt_trap_portDot1xPktAction_set(rt_port_t port, rt_action_t action)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_portDot1xPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_portDot1xPktAction_set(port, action);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_portDot1xPktAction_set */



/* Function Name:
 *      rt_trap_pattern_get
 * Description:
 *      Get the configuration about ARP packets Action
 * Input:
 *      type        - Pattern type.
 * Output:
 *      pattern     - Trap pattern data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
rt_trap_pattern_get(rt_trap_pattern_type_t type, rt_trap_pattern_t *pattern)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_pattern_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_pattern_get(type, pattern);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_pattern_get */


/* Function Name:
 *      rt_trap_portArpPktAction_set
 * Description:
 *      Set the configuration about ARP packets Action
 * Input:
 *      type        - Pattern type.
 *      pattern     - Trap pattern data
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32
rt_trap_pattern_set(rt_trap_pattern_type_t type, rt_trap_pattern_t pattern)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_pattern_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_pattern_set(type, pattern);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_pattern_set */

/* Function Name:
 *      rt_trap_priority_get
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
int32
rt_trap_priority_get(rt_port_t port, rt_trap_protocol_t type, uint8 *pPri)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_priority_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_priority_get(port, type, pPri);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_priority_get */


/* Function Name:
 *      rt_trap_priority_set
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
int32
rt_trap_priority_set(rt_port_t port, rt_trap_protocol_t type, uint8 pri)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_trap_priority_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_trap_priority_set(port, type, pri);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_trap_priority_set */




