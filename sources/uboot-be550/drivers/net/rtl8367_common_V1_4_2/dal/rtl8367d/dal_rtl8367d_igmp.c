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
 * Feature : Here is a list of all functions and variables in IGMP module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/rtl8367d_asicdrv.h>
#include <dal/rtl8367d/dal_rtl8367d_igmp.h>
#include <linux/string.h>

#define RTL8367D_PROTOCOL_OP_FLOOD   1
#define RTL8367D_PROTOCOL_OP_TRAP    2
#define RTL8367D_PROTOCOL_OP_DROP    3

/* Function Name:
 *      dal_rtl8367d_igmp_protocol_set
 * Description:
 *      set IGMP/MLD protocol action
 * Input:
 *      port        - Port ID
 *      protocol    - IGMP/MLD protocol
 *      action      - Per-port and per-protocol IGMP action seeting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_MASK       - Error parameter
 * Note:
 *      This API set IGMP/MLD protocol action
 */
rtk_api_ret_t dal_rtl8367d_igmp_protocol_set(rtk_port_t port, rtk_igmp_protocol_t protocol, rtk_igmp_action_t action)
{
    rtk_uint32      operation;
    rtk_api_ret_t   retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check port valid */
    RTK_CHK_PORT_VALID(port);

    if(protocol >= PROTOCOL_END)
        return RT_ERR_INPUT;

    if(action >= IGMP_ACTION_END)
        return RT_ERR_INPUT;

    switch(action)
    {
        case IGMP_ACTION_FORWARD:
            operation = RTL8367D_PROTOCOL_OP_FLOOD;
            break;
        case IGMP_ACTION_TRAP2CPU:
            operation = RTL8367D_PROTOCOL_OP_TRAP;
            break;
        case IGMP_ACTION_DROP:
            operation = RTL8367D_PROTOCOL_OP_DROP;
            break;
        case IGMP_ACTION_ASIC:
        default:
            return RT_ERR_INPUT;
    }

    switch(protocol)
    {
        case PROTOCOL_IGMPv1:
            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_IGMP_PORT0_CONTROL + rtk_switch_port_L2P_get(port)), RTL8367D_IGMP_PORT0_CONTROL_IGMPV1_OP_MASK, operation))!=RT_ERR_OK)
                return retVal;
            break;
        case PROTOCOL_IGMPv2:
            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_IGMP_PORT0_CONTROL + rtk_switch_port_L2P_get(port)), RTL8367D_IGMP_PORT0_CONTROL_IGMPV2_OP_MASK, operation))!=RT_ERR_OK)
                return retVal;
            break;
        case PROTOCOL_IGMPv3:
            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_IGMP_PORT0_CONTROL + rtk_switch_port_L2P_get(port)), RTL8367D_IGMP_PORT0_CONTROL_IGMPV3_OP_MASK, operation))!=RT_ERR_OK)
                return retVal;
            break;
        case PROTOCOL_MLDv1:
            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_IGMP_PORT0_CONTROL + rtk_switch_port_L2P_get(port)), RTL8367D_IGMP_PORT0_CONTROL_MLDv1_OP_MASK, operation))!=RT_ERR_OK)
                return retVal;
            break;
        case PROTOCOL_MLDv2:
            if ((retVal = rtl8367d_setAsicRegBits((RTL8367D_REG_IGMP_PORT0_CONTROL + rtk_switch_port_L2P_get(port)), RTL8367D_IGMP_PORT0_CONTROL_MLDv2_OP_MASK, operation))!=RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;

    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_igmp_protocol_get
 * Description:
 *      set IGMP/MLD protocol action
 * Input:
 *      port        - Port ID
 *      protocol    - IGMP/MLD protocol
 *      action      - Per-port and per-protocol IGMP action seeting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_MASK       - Error parameter
 * Note:
 *      This API set IGMP/MLD protocol action
 */
rtk_api_ret_t dal_rtl8367d_igmp_protocol_get(rtk_port_t port, rtk_igmp_protocol_t protocol, rtk_igmp_action_t *pAction)
{
    rtk_uint32      operation;
    rtk_api_ret_t   retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check port valid */
    RTK_CHK_PORT_VALID(port);

    if(protocol >= PROTOCOL_END)
        return RT_ERR_INPUT;

    if(pAction == NULL)
        return RT_ERR_NULL_POINTER;

    switch(protocol)
    {
        case PROTOCOL_IGMPv1:
            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_IGMP_PORT0_CONTROL + rtk_switch_port_L2P_get(port)), RTL8367D_IGMP_PORT0_CONTROL_IGMPV1_OP_MASK, &operation))!=RT_ERR_OK)
                return retVal;
            break;
        case PROTOCOL_IGMPv2:
            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_IGMP_PORT0_CONTROL + rtk_switch_port_L2P_get(port)), RTL8367D_IGMP_PORT0_CONTROL_IGMPV2_OP_MASK, &operation))!=RT_ERR_OK)
                return retVal;
            break;
        case PROTOCOL_IGMPv3:
            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_IGMP_PORT0_CONTROL + rtk_switch_port_L2P_get(port)), RTL8367D_IGMP_PORT0_CONTROL_IGMPV3_OP_MASK, &operation))!=RT_ERR_OK)
                return retVal;
            break;
        case PROTOCOL_MLDv1:
            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_IGMP_PORT0_CONTROL + rtk_switch_port_L2P_get(port)), RTL8367D_IGMP_PORT0_CONTROL_MLDv1_OP_MASK, &operation))!=RT_ERR_OK)
                return retVal;
            break;
        case PROTOCOL_MLDv2:
            if ((retVal = rtl8367d_getAsicRegBits((RTL8367D_REG_IGMP_PORT0_CONTROL + rtk_switch_port_L2P_get(port)), RTL8367D_IGMP_PORT0_CONTROL_MLDv2_OP_MASK, &operation))!=RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;

    }

    switch(operation)
    {
        case RTL8367D_PROTOCOL_OP_FLOOD:
            *pAction = IGMP_ACTION_FORWARD;
            break;
        case RTL8367D_PROTOCOL_OP_TRAP:
            *pAction = IGMP_ACTION_TRAP2CPU;
            break;
        case RTL8367D_PROTOCOL_OP_DROP:
            *pAction = IGMP_ACTION_DROP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_igmp_bypassGroupRange_set
 * Description:
 *      Set Bypass group
 * Input:
 *      group       - bypassed group
 *      enabled     - enabled 1: Bypassed, 0: not bypass
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_igmp_bypassGroupRange_set(rtk_igmp_bypassGroup_t group, rtk_enable_t enabled)
{
    rtk_api_ret_t   retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(group >= IGMP_BYPASS_GROUP_END)
        return RT_ERR_INPUT;

    if(enabled >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    switch (group)
    {
        case IGMP_BYPASS_224_0_0_X:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG3, RTL8367D_IGMP_MLD_IP4_BYPASS_224_0_0_OFFSET, (rtk_uint32)enabled)) != RT_ERR_OK)
                return retVal;
            break;
        case IGMP_BYPASS_224_0_1_X:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG3, RTL8367D_IGMP_MLD_IP4_BYPASS_224_0_1_OFFSET, (rtk_uint32)enabled)) != RT_ERR_OK)
                return retVal;
            break;
        case IGMP_BYPASS_239_255_255_X:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG3, RTL8367D_IGMP_MLD_IP4_BYPASS_239_255_255_OFFSET, (rtk_uint32)enabled)) != RT_ERR_OK)
                return retVal;
            break;
        case IGMP_BYPASS_IPV6_00XX:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG3, RTL8367D_IGMP_MLD_IP6_BYPASS_OFFSET, (rtk_uint32)enabled)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_igmp_bypassGroupRange_get
 * Description:
 *      get Bypass group
 * Input:
 *      group       - bypassed group
 * Output:
 *      pEnable     - enabled 1: Bypassed, 0: not bypass
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_NULL_POINTER    - Null Pointer
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_igmp_bypassGroupRange_get(rtk_igmp_bypassGroup_t group, rtk_enable_t *pEnable)
{
    rtk_api_ret_t   retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(group >= IGMP_BYPASS_GROUP_END)
        return RT_ERR_INPUT;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    switch (group)
    {
        case IGMP_BYPASS_224_0_0_X:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG3, RTL8367D_IGMP_MLD_IP4_BYPASS_224_0_0_OFFSET, (rtk_uint32 *)pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case IGMP_BYPASS_224_0_1_X:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG3, RTL8367D_IGMP_MLD_IP4_BYPASS_224_0_1_OFFSET, (rtk_uint32 *)pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case IGMP_BYPASS_239_255_255_X:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG3, RTL8367D_IGMP_MLD_IP4_BYPASS_239_255_255_OFFSET, (rtk_uint32 *)pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case IGMP_BYPASS_IPV6_00XX:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG3, RTL8367D_IGMP_MLD_IP6_BYPASS_OFFSET, (rtk_uint32 *)pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

