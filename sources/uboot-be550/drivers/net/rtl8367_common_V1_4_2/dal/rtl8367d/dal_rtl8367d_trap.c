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
 * $Revision$
 * $Date$
 *
 * Purpose : RTK switch high-level API for RTL8367/RTL8367C
 * Feature : Here is a list of all functions and variables in Trap module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_trap.h>
#include <dal/rtl8367d/rtl8367d_asicdrv.h>

/* Function Name:
 *      dal_rtl8367d_trap_unknownUnicastPktAction_set
 * Description:
 *      Set unknown unicast packet action configuration.
 * Input:
 *      port            - ingress port ID for unknown unicast packet
 *      ucast_action    - Unknown unicast action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 *          - UCAST_ACTION_FLOODING
 */
rtk_api_ret_t dal_rtl8367d_trap_unknownUnicastPktAction_set(rtk_port_t port, rtk_trap_ucast_action_t ucast_action)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* check port valid */
    RTK_CHK_PORT_VALID(port);

    if (ucast_action >= UCAST_ACTION_COPY28051)
        return RT_ERR_INPUT;

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_UNKNOWN_UNICAST_DA_PORT_BEHAVE, RTL8367D_Port0_ACTION_MASK << (phyPort * 2), (rtk_uint32)ucast_action)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_unknownUnicastPktAction_get
 * Description:
 *      Get unknown unicast packet action configuration.
 * Input:
 *      port            - ingress port ID for unknown unicast packet
 * Output:
 *      pUcast_action   - Unknown unicast action.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 *      RT_ERR_NULL_POINTER        - Null pointer
 * Note:
 *      This API can get unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 *          - UCAST_ACTION_FLOODING
 */
rtk_api_ret_t dal_rtl8367d_trap_unknownUnicastPktAction_get(rtk_port_t port, rtk_trap_ucast_action_t *pUcast_action)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* check port valid */
    RTK_CHK_PORT_VALID(port);

    if (NULL == pUcast_action)
        return RT_ERR_NULL_POINTER;

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_UNKNOWN_UNICAST_DA_PORT_BEHAVE, RTL8367D_Port0_ACTION_MASK << (phyPort * 2), &regData)) != RT_ERR_OK)
        return retVal;

    switch (regData)
    {
        case 0:
            *pUcast_action = UCAST_ACTION_FORWARD_PMASK;
            break;
        case 1:
            *pUcast_action = UCAST_ACTION_DROP;
            break;
        case 2:
            *pUcast_action = UCAST_ACTION_TRAP2CPU;
            break;
        case 3:
            *pUcast_action = UCAST_ACTION_FLOODING;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_unmatchMacMoving_set
 * Description:
 *      Set unmatch source MAC packet moving state.
 * Input:
 *      port        - Port ID.
 *      enable      - ENABLED: allow SA moving, DISABLE: don't allow SA moving.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 */
rtk_api_ret_t dal_rtl8367d_trap_unmatchMacMoving_set(rtk_port_t port, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* check port valid */
    RTK_CHK_PORT_VALID(port);

    if(enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_L2_SA_MOVING_FORBID, rtk_switch_port_L2P_get(port), (enable == ENABLED) ? 0 : 1)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_unmatchMacMoving_get
 * Description:
 *      Set unmatch source MAC packet moving state.
 * Input:
 *      port        - Port ID.
 * Output:
 *      pEnable     - ENABLED: allow SA moving, DISABLE: don't allow SA moving.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 */
rtk_api_ret_t dal_rtl8367d_trap_unmatchMacMoving_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* check port valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_L2_SA_MOVING_FORBID, rtk_switch_port_L2P_get(port), &regData)) != RT_ERR_OK)
        return retVal;

    *pEnable = (regData == 0) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_unknownMcastPktAction_set
 * Description:
 *      Set behavior of unknown multicast
 * Input:
 *      port            - Port id.
 *      type            - unknown multicast packet type.
 *      mcast_action    - unknown multicast action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID         - Invalid port number.
 *      RT_ERR_NOT_ALLOWED     - Invalid action.
 *      RT_ERR_INPUT         - Invalid input parameters.
 * Note:
 *      When receives an unknown multicast packet, switch may trap, drop or flood this packet
 *      (1) The unknown multicast packet type is as following:
 *          - MCAST_L2
 *          - MCAST_IPV4
 *          - MCAST_IPV6
 *      (2) The unknown multicast action is as following:
 *          - MCAST_ACTION_FORWARD
 *          - MCAST_ACTION_DROP
 *          - MCAST_ACTION_TRAP2CPU
 */
rtk_api_ret_t dal_rtl8367d_trap_unknownMcastPktAction_set(rtk_port_t port, rtk_mcast_type_t type, rtk_trap_mcast_action_t mcast_action)
{
    rtk_api_ret_t retVal;
    rtk_uint32 rawAction;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (type >= MCAST_END)
        return RT_ERR_INPUT;

    if (mcast_action >= MCAST_ACTION_END)
        return RT_ERR_INPUT;

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    switch (type)
    {
        case MCAST_L2:
            if (MCAST_ACTION_ROUTER_PORT == mcast_action)
                return RT_ERR_INPUT;
            else if(MCAST_ACTION_DROP_EX_RMA == mcast_action)
                rawAction = RTL8367D_L2_UNKOWN_MULTICAST_DROP_EXCLUDE_RMA;
            else
                rawAction = (rtk_uint32)mcast_action;

            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_UNKNOWN_L2_MULTICAST_CTRL0, (RTL8367D_PORT0_UNKNOWN_L2_MCAST_MASK << (phyPort * 2)), rawAction)) != RT_ERR_OK)
                return retVal;

            break;
        case MCAST_IPV4:
            if ((MCAST_ACTION_DROP_EX_RMA == mcast_action) || (MCAST_ACTION_ROUTER_PORT == mcast_action))
                return RT_ERR_INPUT;
            else
                rawAction = (rtk_uint32)mcast_action;

            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_UNKNOWN_IPV4_MULTICAST_CTRL0, (RTL8367D_PORT0_UNKNOWN_IP4_MCAST_MASK << (phyPort * 2)), rawAction)) != RT_ERR_OK)
                return retVal;

            break;
        case MCAST_IPV6:
            if ((MCAST_ACTION_DROP_EX_RMA == mcast_action) || (MCAST_ACTION_ROUTER_PORT == mcast_action))
                return RT_ERR_INPUT;
            else
                rawAction = (rtk_uint32)mcast_action;

            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_UNKNOWN_IPV6_MULTICAST_CTRL0, (RTL8367D_PORT0_UNKNOWN_IP6_MCAST_MASK << (phyPort * 2)), rawAction)) != RT_ERR_OK)
                return retVal;

            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_unknownMcastPktAction_get
 * Description:
 *      Get behavior of unknown multicast
 * Input:
 *      type - unknown multicast packet type.
 * Output:
 *      pMcast_action - unknown multicast action.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_PORT_ID             - Invalid port number.
 *      RT_ERR_NOT_ALLOWED         - Invalid operation.
 *      RT_ERR_INPUT             - Invalid input parameters.
 * Note:
 *      When receives an unknown multicast packet, switch may trap, drop or flood this packet
 *      (1) The unknown multicast packet type is as following:
 *          - MCAST_L2
 *          - MCAST_IPV4
 *          - MCAST_IPV6
 *      (2) The unknown multicast action is as following:
 *          - MCAST_ACTION_FORWARD
 *          - MCAST_ACTION_DROP
 *          - MCAST_ACTION_TRAP2CPU
 */
rtk_api_ret_t dal_rtl8367d_trap_unknownMcastPktAction_get(rtk_port_t port, rtk_mcast_type_t type, rtk_trap_mcast_action_t *pMcast_action)
{
    rtk_api_ret_t retVal;
    rtk_uint32 rawAction;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (type >= MCAST_END)
        return RT_ERR_INPUT;

    if(NULL == pMcast_action)
        return RT_ERR_NULL_POINTER;

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    switch (type)
    {
        case MCAST_L2:
            if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_UNKNOWN_L2_MULTICAST_CTRL0, (RTL8367D_PORT0_UNKNOWN_L2_MCAST_MASK << (phyPort * 2)), &rawAction)) != RT_ERR_OK)
                return retVal;

            if(RTL8367D_L2_UNKOWN_MULTICAST_DROP_EXCLUDE_RMA == rawAction)
                *pMcast_action = MCAST_ACTION_DROP_EX_RMA;
            else
                *pMcast_action = (rtk_trap_mcast_action_t)rawAction;

            break;
        case MCAST_IPV4:
            if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_UNKNOWN_IPV4_MULTICAST_CTRL0, (RTL8367D_PORT0_UNKNOWN_IP4_MCAST_MASK << (phyPort * 2)), &rawAction)) != RT_ERR_OK)
                return retVal;

            *pMcast_action = (rtk_trap_mcast_action_t)rawAction;
            break;
        case MCAST_IPV6:
            if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_UNKNOWN_IPV6_MULTICAST_CTRL0, (RTL8367D_PORT0_UNKNOWN_IP6_MCAST_MASK << (phyPort * 2)), &rawAction)) != RT_ERR_OK)
                return retVal;

            *pMcast_action = (rtk_trap_mcast_action_t)rawAction;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_lldpEnable_set
 * Description:
 *      Set LLDP enable.
 * Input:
 *      enabled - LLDP enable, 0: follow RMA, 1: use LLDP action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT             - Invalid input parameters.
 * Note:
 *      - DMAC                                                 Assignment
 *      - 01:80:c2:00:00:0e ethertype = 0x88CC    LLDP
 *      - 01:80:c2:00:00:03 ethertype = 0x88CC
 *      - 01:80:c2:00:00:00 ethertype = 0x88CC

 */
rtk_api_ret_t dal_rtl8367d_trap_lldpEnable_set(rtk_enable_t enabled)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (enabled >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_LLDP_EN, RTL8367D_RMA_LLDP_EN_OFFSET, (enabled == ENABLED) ? 1 : 0)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_lldpEnable_get
 * Description:
 *      Get LLDP status.
 * Input:
 *      None
 * Output:
 *      pEnabled - LLDP enable, 0: follow RMA, 1: use LLDP action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameters.
 * Note:
 *      LLDP is as following definition.
 *      - DMAC                                                 Assignment
 *      - 01:80:c2:00:00:0e ethertype = 0x88CC    LLDP
 *      - 01:80:c2:00:00:03 ethertype = 0x88CC
 *      - 01:80:c2:00:00:00 ethertype = 0x88CC
 */
rtk_api_ret_t dal_rtl8367d_trap_lldpEnable_get(rtk_enable_t *pEnabled)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pEnabled)
        return RT_ERR_NULL_POINTER;

    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_LLDP_EN, RTL8367D_RMA_LLDP_EN_OFFSET, &regData)) != RT_ERR_OK)
        return retVal;

    *pEnabled = (regData == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_reasonTrapToCpuPriority_set
 * Description:
 *      Set priority value of a packet that trapped to CPU port according to specific reason.
 * Input:
 *      type     - reason that trap to CPU port.
 *      priority - internal priority that is going to be set for specific trap reason.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      Currently the trap reason that supported are listed as follows:
 *      - TRAP_REASON_RMA
 *      - TRAP_REASON_1XUNAUTH
 *      - TRAP_REASON_VLANSTACK
 *      - TRAP_REASON_UNKNOWNMC
 *      - TRAP_REASON_IGMPMLD
 */
rtk_api_ret_t dal_rtl8367d_trap_reasonTrapToCpuPriority_set(rtk_trap_reason_type_t type, rtk_pri_t priority)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= TRAP_REASON_END)
        return RT_ERR_INPUT;

    if (type == TRAP_REASON_OAM)
        return RT_ERR_INPUT;

    if (priority > RTL8367D_TRAP_PRIMAX)
        return  RT_ERR_QOS_INT_PRIORITY;

    switch (type)
    {
        case TRAP_REASON_RMA:
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RMA_CTRL00, RTL8367D_TRAP_PRIORITY_MASK, priority)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_1XUNAUTH:
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_QOS_TRAP_PRIORITY0, RTL8367D_DOT1X_PRIORTY_MASK, priority)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_VLANSTACK:
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_QOS_TRAP_PRIORITY0, RTL8367D_SVLAN_PRIOIRTY_MASK, priority)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_UNKNOWNMC:
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_QOS_TRAP_PRIORITY0, RTL8367D_UNKNOWN_MC_PRIORTY_MASK, priority)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_IGMPMLD:
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_QOS_TRAP_PRIORITY1, RTL8367D_QOS_TRAP_PRIORITY1_MASK, priority)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;
    }


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_reasonTrapToCpuPriority_get
 * Description:
 *      Get priority value of a packet that trapped to CPU port according to specific reason.
 * Input:
 *      type      - reason that trap to CPU port.
 * Output:
 *      pPriority - configured internal priority for such reason.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      Currently the trap reason that supported are listed as follows:
 *      - TRAP_REASON_RMA
 *      - TRAP_REASON_1XUNAUTH
 *      - TRAP_REASON_VLANSTACK
 *      - TRAP_REASON_UNKNOWNMC
 *      - TRAP_REASON_IGMPMLD
 */
rtk_api_ret_t dal_rtl8367d_trap_reasonTrapToCpuPriority_get(rtk_trap_reason_type_t type, rtk_pri_t *pPriority)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= TRAP_REASON_END)
        return RT_ERR_INPUT;

    if (type == TRAP_REASON_OAM)
        return RT_ERR_INPUT;

    if(NULL == pPriority)
        return RT_ERR_NULL_POINTER;

    switch (type)
    {
        case TRAP_REASON_RMA:
            if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RMA_CTRL00, RTL8367D_TRAP_PRIORITY_MASK, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_1XUNAUTH:
            if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_QOS_TRAP_PRIORITY0, RTL8367D_DOT1X_PRIORTY_MASK, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_VLANSTACK:
            if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_QOS_TRAP_PRIORITY0, RTL8367D_SVLAN_PRIOIRTY_MASK, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_UNKNOWNMC:
            if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_QOS_TRAP_PRIORITY0, RTL8367D_UNKNOWN_MC_PRIORTY_MASK, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_IGMPMLD:
            if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_QOS_TRAP_PRIORITY1, RTL8367D_QOS_TRAP_PRIORITY1_MASK, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;
    }

    *pPriority = (rtk_pri_t)regData;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_rmaAction_set
 * Description:
 *      Set Reserved multicast address action configuration.
 * Input:
 *      type    - rma type.
 *      rma_action - RMA action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *
 *      There are 48 types of Reserved Multicast Address frame for application usage.
 *      (1)They are as following definition.
 *      - TRAP_BRG_GROUP,
 *      - TRAP_FD_PAUSE,
 *      - TRAP_SP_MCAST,
 *      - TRAP_1X_PAE,
 *      - TRAP_UNDEF_BRG_04,
 *      - TRAP_UNDEF_BRG_05,
 *      - TRAP_UNDEF_BRG_06,
 *      - TRAP_UNDEF_BRG_07,
 *      - TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - TRAP_UNDEF_BRG_09,
 *      - TRAP_UNDEF_BRG_0A,
 *      - TRAP_UNDEF_BRG_0B,
 *      - TRAP_UNDEF_BRG_0C,
 *      - TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - TRAP_8021AB,
 *      - TRAP_UNDEF_BRG_0F,
 *      - TRAP_BRG_MNGEMENT,
 *      - TRAP_UNDEFINED_11,
 *      - TRAP_UNDEFINED_12,
 *      - TRAP_UNDEFINED_13,
 *      - TRAP_UNDEFINED_14,
 *      - TRAP_UNDEFINED_15,
 *      - TRAP_UNDEFINED_16,
 *      - TRAP_UNDEFINED_17,
 *      - TRAP_UNDEFINED_18,
 *      - TRAP_UNDEFINED_19,
 *      - TRAP_UNDEFINED_1A,
 *      - TRAP_UNDEFINED_1B,
 *      - TRAP_UNDEFINED_1C,
 *      - TRAP_UNDEFINED_1D,
 *      - TRAP_UNDEFINED_1E,
 *      - TRAP_UNDEFINED_1F,
 *      - TRAP_GMRP,
 *      - TRAP_GVRP,
 *      - TRAP_UNDEF_GARP_22,
 *      - TRAP_UNDEF_GARP_23,
 *      - TRAP_UNDEF_GARP_24,
 *      - TRAP_UNDEF_GARP_25,
 *      - TRAP_UNDEF_GARP_26,
 *      - TRAP_UNDEF_GARP_27,
 *      - TRAP_UNDEF_GARP_28,
 *      - TRAP_UNDEF_GARP_29,
 *      - TRAP_UNDEF_GARP_2A,
 *      - TRAP_UNDEF_GARP_2B,
 *      - TRAP_UNDEF_GARP_2C,
 *      - TRAP_UNDEF_GARP_2D,
 *      - TRAP_UNDEF_GARP_2E,
 *      - TRAP_UNDEF_GARP_2F,
 *      - TRAP_CDP.
 *      - TRAP_CSSTP.
 *      - TRAP_LLDP.
 *      (2) The RMA action is as following:
 *      - RMA_ACTION_FORWARD
 *      - RMA_ACTION_TRAP2CPU
 *      - RMA_ACTION_DROP
 *      - RMA_ACTION_FORWARD_EXCLUDE_CPU
 */
rtk_api_ret_t dal_rtl8367d_trap_rmaAction_set(rtk_trap_type_t type, rtk_trap_rma_action_t rma_action)
{
    rtk_api_ret_t retVal;
    rtk_uint32 index;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= TRAP_END)
        return RT_ERR_INPUT;

    if (rma_action >= RMA_ACTION_END)
        return RT_ERR_RMA_ACTION;

    if (type >= 0 && type <= TRAP_UNDEF_GARP_2F)
    {
        if( (type >= 0x4 && type <= 0x7) || (type >= 0x9 && type <= 0x0C) || (0x0F == type))
            index = 0x04;
        else if((type >= 0x13 && type <= 0x17) || (0x19 == type) || (type >= 0x1B && type <= 0x1f))
            index = 0x13;
        else if(type >= 0x22 && type <= 0x2F)
            index = 0x22;
        else
            index = (rtk_uint32)type;

        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RMA_CTRL00 + index, RTL8367D_RMA_CTRL00_OPERATION_MASK, (rtk_uint32)rma_action)) != RT_ERR_OK)
            return retVal;
    }
    else if (type == TRAP_CDP)
    {
        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RMA_CTRL_CDP, RTL8367D_RMA_CTRL_CDP_OPERATION_MASK, (rtk_uint32)rma_action)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == TRAP_CSSTP)
    {
        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RMA_CTRL_CSSTP, RTL8367D_RMA_CTRL_CSSTP_OPERATION_MASK, (rtk_uint32)rma_action)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == TRAP_LLDP)
    {
        if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RMA_CTRL_LLDP, RTL8367D_RMA_CTRL_LLDP_OPERATION_MASK, (rtk_uint32)rma_action)) != RT_ERR_OK)
            return retVal;
    }
    else
        return RT_ERR_INPUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_rmaAction_get
 * Description:
 *      Get Reserved multicast address action configuration.
 * Input:
 *      type - rma type.
 * Output:
 *      pRma_action - RMA action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      There are 48 types of Reserved Multicast Address frame for application usage.
 *      (1)They are as following definition.
 *      - TRAP_BRG_GROUP,
 *      - TRAP_FD_PAUSE,
 *      - TRAP_SP_MCAST,
 *      - TRAP_1X_PAE,
 *      - TRAP_UNDEF_BRG_04,
 *      - TRAP_UNDEF_BRG_05,
 *      - TRAP_UNDEF_BRG_06,
 *      - TRAP_UNDEF_BRG_07,
 *      - TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - TRAP_UNDEF_BRG_09,
 *      - TRAP_UNDEF_BRG_0A,
 *      - TRAP_UNDEF_BRG_0B,
 *      - TRAP_UNDEF_BRG_0C,
 *      - TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - TRAP_8021AB,
 *      - TRAP_UNDEF_BRG_0F,
 *      - TRAP_BRG_MNGEMENT,
 *      - TRAP_UNDEFINED_11,
 *      - TRAP_UNDEFINED_12,
 *      - TRAP_UNDEFINED_13,
 *      - TRAP_UNDEFINED_14,
 *      - TRAP_UNDEFINED_15,
 *      - TRAP_UNDEFINED_16,
 *      - TRAP_UNDEFINED_17,
 *      - TRAP_UNDEFINED_18,
 *      - TRAP_UNDEFINED_19,
 *      - TRAP_UNDEFINED_1A,
 *      - TRAP_UNDEFINED_1B,
 *      - TRAP_UNDEFINED_1C,
 *      - TRAP_UNDEFINED_1D,
 *      - TRAP_UNDEFINED_1E,
 *      - TRAP_UNDEFINED_1F,
 *      - TRAP_GMRP,
 *      - TRAP_GVRP,
 *      - TRAP_UNDEF_GARP_22,
 *      - TRAP_UNDEF_GARP_23,
 *      - TRAP_UNDEF_GARP_24,
 *      - TRAP_UNDEF_GARP_25,
 *      - TRAP_UNDEF_GARP_26,
 *      - TRAP_UNDEF_GARP_27,
 *      - TRAP_UNDEF_GARP_28,
 *      - TRAP_UNDEF_GARP_29,
 *      - TRAP_UNDEF_GARP_2A,
 *      - TRAP_UNDEF_GARP_2B,
 *      - TRAP_UNDEF_GARP_2C,
 *      - TRAP_UNDEF_GARP_2D,
 *      - TRAP_UNDEF_GARP_2E,
 *      - TRAP_UNDEF_GARP_2F,
 *      - TRAP_CDP.
 *      - TRAP_CSSTP.
 *      - TRAP_LLDP.
 *      (2) The RMA action is as following:
 *      - RMA_ACTION_FORWARD
 *      - RMA_ACTION_TRAP2CPU
 *      - RMA_ACTION_DROP
 *      - RMA_ACTION_FORWARD_EXCLUDE_CPU
 */
rtk_api_ret_t dal_rtl8367d_trap_rmaAction_get(rtk_trap_type_t type, rtk_trap_rma_action_t *pRma_action)
{
    rtk_api_ret_t retVal;
    rtk_uint32 index;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= TRAP_END)
        return RT_ERR_INPUT;

    if(NULL == pRma_action)
        return RT_ERR_NULL_POINTER;

    if (type >= 0 && type <= TRAP_UNDEF_GARP_2F)
    {
        if( (type >= 0x4 && type <= 0x7) || (type >= 0x9 && type <= 0x0C) || (0x0F == type))
            index = 0x04;
        else if((type >= 0x13 && type <= 0x17) || (0x19 == type) || (type >= 0x1B && type <= 0x1f))
            index = 0x13;
        else if(type >= 0x22 && type <= 0x2F)
            index = 0x22;
        else
            index = (rtk_uint32)type;

        if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RMA_CTRL00 + index, RTL8367D_RMA_CTRL00_OPERATION_MASK, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else if (type == TRAP_CDP)
    {
        if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RMA_CTRL_CDP, RTL8367D_RMA_CTRL_CDP_OPERATION_MASK, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == TRAP_CSSTP)
    {
        if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RMA_CTRL_CSSTP, RTL8367D_RMA_CTRL_CSSTP_OPERATION_MASK, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == TRAP_LLDP)
    {
        if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RMA_CTRL_LLDP, RTL8367D_RMA_CTRL_LLDP_OPERATION_MASK, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else
        return RT_ERR_INPUT;

    *pRma_action = (rtk_trap_rma_action_t)regData;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_rmaKeepFormat_set
 * Description:
 *      Set Reserved multicast address keep format configuration.
 * Input:
 *      type    - rma type.
 *      enable - enable keep format.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid IFG parameter
 * Note:
 *
 *      There are 48 types of Reserved Multicast Address frame for application usage.
 *      They are as following definition.
 *      - TRAP_BRG_GROUP,
 *      - TRAP_FD_PAUSE,
 *      - TRAP_SP_MCAST,
 *      - TRAP_1X_PAE,
 *      - TRAP_UNDEF_BRG_04,
 *      - TRAP_UNDEF_BRG_05,
 *      - TRAP_UNDEF_BRG_06,
 *      - TRAP_UNDEF_BRG_07,
 *      - TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - TRAP_UNDEF_BRG_09,
 *      - TRAP_UNDEF_BRG_0A,
 *      - TRAP_UNDEF_BRG_0B,
 *      - TRAP_UNDEF_BRG_0C,
 *      - TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - TRAP_8021AB,
 *      - TRAP_UNDEF_BRG_0F,
 *      - TRAP_BRG_MNGEMENT,
 *      - TRAP_UNDEFINED_11,
 *      - TRAP_UNDEFINED_12,
 *      - TRAP_UNDEFINED_13,
 *      - TRAP_UNDEFINED_14,
 *      - TRAP_UNDEFINED_15,
 *      - TRAP_UNDEFINED_16,
 *      - TRAP_UNDEFINED_17,
 *      - TRAP_UNDEFINED_18,
 *      - TRAP_UNDEFINED_19,
 *      - TRAP_UNDEFINED_1A,
 *      - TRAP_UNDEFINED_1B,
 *      - TRAP_UNDEFINED_1C,
 *      - TRAP_UNDEFINED_1D,
 *      - TRAP_UNDEFINED_1E,
 *      - TRAP_UNDEFINED_1F,
 *      - TRAP_GMRP,
 *      - TRAP_GVRP,
 *      - TRAP_UNDEF_GARP_22,
 *      - TRAP_UNDEF_GARP_23,
 *      - TRAP_UNDEF_GARP_24,
 *      - TRAP_UNDEF_GARP_25,
 *      - TRAP_UNDEF_GARP_26,
 *      - TRAP_UNDEF_GARP_27,
 *      - TRAP_UNDEF_GARP_28,
 *      - TRAP_UNDEF_GARP_29,
 *      - TRAP_UNDEF_GARP_2A,
 *      - TRAP_UNDEF_GARP_2B,
 *      - TRAP_UNDEF_GARP_2C,
 *      - TRAP_UNDEF_GARP_2D,
 *      - TRAP_UNDEF_GARP_2E,
 *      - TRAP_UNDEF_GARP_2F,
 *      - TRAP_CDP.
 *      - TRAP_CSSTP.
 *      - TRAP_LLDP.
 */
rtk_api_ret_t dal_rtl8367d_trap_rmaKeepFormat_set(rtk_trap_type_t type, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 index;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= TRAP_END)
        return RT_ERR_INPUT;

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if (type >= 0 && type <= TRAP_UNDEF_GARP_2F)
    {
        if( (type >= 0x4 && type <= 0x7) || (type >= 0x9 && type <= 0x0C) || (0x0F == type))
            index = 0x04;
        else if((type >= 0x13 && type <= 0x17) || (0x19 == type) || (type >= 0x1B && type <= 0x1f))
            index = 0x13;
        else if(type >= 0x22 && type <= 0x2F)
            index = 0x22;
        else
            index = (rtk_uint32)type;

        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL00 + index, RTL8367D_RMA_CTRL00_KEEP_FORMAT_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
            return retVal;
    }
    else if (type == TRAP_CDP)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_CDP, RTL8367D_RMA_CTRL_CDP_KEEP_FORMAT_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == TRAP_CSSTP)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_CSSTP, RTL8367D_RMA_CTRL_CSSTP_KEEP_FORMAT_OFFSET,(enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == TRAP_LLDP)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_LLDP, RTL8367D_RMA_CTRL_LLDP_KEEP_FORMAT_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
            return retVal;
    }
    else
        return RT_ERR_INPUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_rmaKeepFormat_get
 * Description:
 *      Get Reserved multicast address action configuration.
 * Input:
 *      type - rma type.
 * Output:
 *      pEnable - keep format status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      There are 48 types of Reserved Multicast Address frame for application usage.
 *      They are as following definition.
 *      - TRAP_BRG_GROUP,
 *      - TRAP_FD_PAUSE,
 *      - TRAP_SP_MCAST,
 *      - TRAP_1X_PAE,
 *      - TRAP_UNDEF_BRG_04,
 *      - TRAP_UNDEF_BRG_05,
 *      - TRAP_UNDEF_BRG_06,
 *      - TRAP_UNDEF_BRG_07,
 *      - TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - TRAP_UNDEF_BRG_09,
 *      - TRAP_UNDEF_BRG_0A,
 *      - TRAP_UNDEF_BRG_0B,
 *      - TRAP_UNDEF_BRG_0C,
 *      - TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - TRAP_8021AB,
 *      - TRAP_UNDEF_BRG_0F,
 *      - TRAP_BRG_MNGEMENT,
 *      - TRAP_UNDEFINED_11,
 *      - TRAP_UNDEFINED_12,
 *      - TRAP_UNDEFINED_13,
 *      - TRAP_UNDEFINED_14,
 *      - TRAP_UNDEFINED_15,
 *      - TRAP_UNDEFINED_16,
 *      - TRAP_UNDEFINED_17,
 *      - TRAP_UNDEFINED_18,
 *      - TRAP_UNDEFINED_19,
 *      - TRAP_UNDEFINED_1A,
 *      - TRAP_UNDEFINED_1B,
 *      - TRAP_UNDEFINED_1C,
 *      - TRAP_UNDEFINED_1D,
 *      - TRAP_UNDEFINED_1E,
 *      - TRAP_UNDEFINED_1F,
 *      - TRAP_GMRP,
 *      - TRAP_GVRP,
 *      - TRAP_UNDEF_GARP_22,
 *      - TRAP_UNDEF_GARP_23,
 *      - TRAP_UNDEF_GARP_24,
 *      - TRAP_UNDEF_GARP_25,
 *      - TRAP_UNDEF_GARP_26,
 *      - TRAP_UNDEF_GARP_27,
 *      - TRAP_UNDEF_GARP_28,
 *      - TRAP_UNDEF_GARP_29,
 *      - TRAP_UNDEF_GARP_2A,
 *      - TRAP_UNDEF_GARP_2B,
 *      - TRAP_UNDEF_GARP_2C,
 *      - TRAP_UNDEF_GARP_2D,
 *      - TRAP_UNDEF_GARP_2E,
 *      - TRAP_UNDEF_GARP_2F,
 *      - TRAP_CDP.
 *      - TRAP_CSSTP.
 *      - TRAP_LLDP.
 */
rtk_api_ret_t dal_rtl8367d_trap_rmaKeepFormat_get(rtk_trap_type_t type, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;
    rtk_uint32 index;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= TRAP_END)
        return RT_ERR_INPUT;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if (type >= 0 && type <= TRAP_UNDEF_GARP_2F)
    {
        if( (type >= 0x4 && type <= 0x7) || (type >= 0x9 && type <= 0x0C) || (0x0F == type))
            index = 0x04;
        else if((type >= 0x13 && type <= 0x17) || (0x19 == type) || (type >= 0x1B && type <= 0x1f))
            index = 0x13;
        else if(type >= 0x22 && type <= 0x2F)
            index = 0x22;
        else
            index = (rtk_uint32)type;

        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL00 + index, RTL8367D_RMA_CTRL00_KEEP_FORMAT_OFFSET, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else if (type == TRAP_CDP)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_CDP, RTL8367D_RMA_CTRL_CDP_KEEP_FORMAT_OFFSET, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == TRAP_CSSTP)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_CSSTP, RTL8367D_RMA_CTRL_CSSTP_KEEP_FORMAT_OFFSET, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == TRAP_LLDP)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_LLDP, RTL8367D_RMA_CTRL_LLDP_KEEP_FORMAT_OFFSET, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else
        return RT_ERR_INPUT;

    *pEnable = (regData == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_portUnknownMacPktAction_set
 * Description:
 *      Set unknown source MAC packet action configuration.
 * Input:
 *      port            - Port ID.
 *      ucast_action    - Unknown source MAC action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 */
rtk_api_ret_t dal_rtl8367d_trap_portUnknownMacPktAction_set(rtk_port_t port, rtk_trap_ucast_action_t ucast_action)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* check port valid */
    RTK_CHK_PORT_VALID(port);

    if (ucast_action >= UCAST_ACTION_FLOODING)
        return RT_ERR_INPUT;

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_LUT_UNKN_SA_CTRL, RTL8367D_LUT_UNKN_SA_CTRL_PORT0_ACT_MASK << (phyPort * 2), (rtk_uint32)ucast_action)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_portUnknownMacPktAction_get
 * Description:
 *      Get unknown source MAC packet action configuration.
 * Input:
 *      port            - Port ID.
 * Output:
 *      pUcast_action   - Unknown source MAC action.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NULL_POINTER        - Null Pointer.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_trap_portUnknownMacPktAction_get(rtk_port_t port, rtk_trap_ucast_action_t *pUcast_action)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* check port valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pUcast_action)
        return RT_ERR_NULL_POINTER;

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_LUT_UNKN_SA_CTRL, RTL8367D_LUT_UNKN_SA_CTRL_PORT0_ACT_MASK << (phyPort * 2), &regData)) != RT_ERR_OK)
        return retVal;

    switch (regData)
    {
        case 0:
            *pUcast_action = UCAST_ACTION_FORWARD_PMASK;
            break;
        case 1:
            *pUcast_action = UCAST_ACTION_DROP;
            break;
        case 2:
            *pUcast_action = UCAST_ACTION_TRAP2CPU;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_portUnmatchMacPktAction_set
 * Description:
 *      Set unmatch source MAC packet action configuration.
 * Input:
 *      port            - Port ID.
 *      ucast_action    - Unmatch source MAC action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 */
rtk_api_ret_t dal_rtl8367d_trap_portUnmatchMacPktAction_set(rtk_port_t port, rtk_trap_ucast_action_t ucast_action)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* check port valid */
    RTK_CHK_PORT_VALID(port);

    if (ucast_action >= UCAST_ACTION_FLOODING)
        return RT_ERR_INPUT;

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_LUT_UNMATCHED_SA_CTRL, RTL8367D_LUT_UNMATCHED_SA_CTRL_PORT0_ACT_MASK << (phyPort * 2), (rtk_uint32)ucast_action)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_trap_portUnmatchMacPktAction_get
 * Description:
 *      Get unmatch source MAC packet action configuration.
 * Input:
 *      port            - Port ID.
 * Output:
 *      pUcast_action   - Unmatch source MAC action.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 */
rtk_api_ret_t dal_rtl8367d_trap_portUnmatchMacPktAction_get(rtk_port_t port, rtk_trap_ucast_action_t *pUcast_action)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* check port valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pUcast_action)
        return RT_ERR_NULL_POINTER;

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_LUT_UNMATCHED_SA_CTRL, RTL8367D_LUT_UNMATCHED_SA_CTRL_PORT0_ACT_MASK << (phyPort * 2), &regData)) != RT_ERR_OK)
        return retVal;

    switch (regData)
    {
        case 0:
            *pUcast_action = UCAST_ACTION_FORWARD_PMASK;
            break;
        case 1:
            *pUcast_action = UCAST_ACTION_DROP;
            break;
        case 2:
            *pUcast_action = UCAST_ACTION_TRAP2CPU;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
}
