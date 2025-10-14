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
 * Feature : Here is a list of all functions and variables in Storm module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_storm.h>
#include <dal/rtl8367d/rtl8367d_asicdrv.h>

#define    RTL8367D_STORM_UNDA_METER_CTRL_REG(port)             (RTL8367D_REG_STORM_UNDA_METER_CTRL0 + (port >> 1))
#define    RTL8367D_STORM_UNDA_METER_CTRL_MASK(port)            (RTL8367D_STORM_UNDA_METER_CTRL0_PORT0_METERIDX_MASK << ((port & 0x1) << 3))

#define    RTL8367D_STORM_UNMC_METER_CTRL_REG(port)             (RTL8367D_REG_STORM_UNMC_METER_CTRL0 + (port >> 1))
#define    RTL8367D_STORM_UNMC_METER_CTRL_MASK(port)            (RTL8367D_STORM_UNMC_METER_CTRL0_PORT0_METERIDX_MASK << ((port & 0x1) << 3))

#define    RTL8367D_STORM_MCAST_METER_CTRL_REG(port)            (RTL8367D_REG_STORM_MCAST_METER_CTRL0 + (port >> 1))
#define    RTL8367D_STORM_MCAST_METER_CTRL_MASK(port)           (RTL8367D_STORM_MCAST_METER_CTRL0_PORT0_METERIDX_MASK << ((port & 0x1) << 3))

#define    RTL8367D_STORM_BCAST_METER_CTRL_REG(port)            (RTL8367D_REG_STORM_BCAST_METER_CTRL0 + (port >> 1))
#define    RTL8367D_STORM_BCAST_METER_CTRL_MASK(port)           (RTL8367D_STORM_BCAST_METER_CTRL0_PORT0_METERIDX_MASK << ((port & 0x1) << 3))

/* Function Name:
 *      dal_rtl8367d_rate_stormControlMeterIdx_set
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
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_rate_stormControlMeterIdx_set(rtk_port_t port, rtk_rate_storm_group_t stormType, rtk_uint32 index)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (index > RTK_MAX_METER_ID)
        return RT_ERR_FILTER_METER_ID;

    phy_port = rtk_switch_port_L2P_get(port);

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_STORM_UNDA_METER_CTRL_REG(phy_port), RTL8367D_STORM_UNDA_METER_CTRL_MASK(phy_port), index)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_STORM_UNMC_METER_CTRL_REG(phy_port), RTL8367D_STORM_UNMC_METER_CTRL_MASK(phy_port), index)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_MULTICAST:
            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_STORM_MCAST_METER_CTRL_REG(phy_port), RTL8367D_STORM_MCAST_METER_CTRL_MASK(phy_port), index)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_BROADCAST:
            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_STORM_BCAST_METER_CTRL_REG(phy_port), RTL8367D_STORM_BCAST_METER_CTRL_MASK(phy_port), index)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rate_stormControlMeterIdx_get
 * Description:
 *      Get the storm control meter index.
 * Input:
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pIndex     - storm control meter index.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_rate_stormControlMeterIdx_get(rtk_port_t port, rtk_rate_storm_group_t stormType, rtk_uint32 *pIndex)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (NULL == pIndex )
        return RT_ERR_NULL_POINTER;

    phy_port = rtk_switch_port_L2P_get(port);

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            if((retVal = rtl8367d_getAsicRegBits(RTL8367D_STORM_UNDA_METER_CTRL_REG(phy_port), RTL8367D_STORM_UNDA_METER_CTRL_MASK(phy_port), pIndex)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if((retVal = rtl8367d_getAsicRegBits(RTL8367D_STORM_UNMC_METER_CTRL_REG(phy_port), RTL8367D_STORM_UNMC_METER_CTRL_MASK(phy_port), pIndex)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_MULTICAST:
            if((retVal = rtl8367d_getAsicRegBits(RTL8367D_STORM_MCAST_METER_CTRL_REG(phy_port), RTL8367D_STORM_MCAST_METER_CTRL_MASK(phy_port), pIndex)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_BROADCAST:
            if((retVal = rtl8367d_getAsicRegBits(RTL8367D_STORM_BCAST_METER_CTRL_REG(phy_port), RTL8367D_STORM_BCAST_METER_CTRL_MASK(phy_port), pIndex)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rate_stormControlPortEnable_set
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
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_rate_stormControlPortEnable_set(rtk_port_t port, rtk_rate_storm_group_t stormType, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_STORM_UNKOWN_UCAST, rtk_switch_port_L2P_get(port), (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_STORM_UNKOWN_MCAST, rtk_switch_port_L2P_get(port), (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_MULTICAST:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_STORM_MCAST, rtk_switch_port_L2P_get(port), (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_BROADCAST:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_STORM_BCAST, rtk_switch_port_L2P_get(port), (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rate_stormControlPortEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 * Input:
 *      port       - port id
 *      stormType  - storm group type
 * Output:
 *      pEnable     - enable status of storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_rate_stormControlPortEnable_get(rtk_port_t port, rtk_rate_storm_group_t stormType, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData = 0;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (NULL == pEnable)
        return RT_ERR_ENABLE;

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_STORM_UNKOWN_UCAST, rtk_switch_port_L2P_get(port), &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_STORM_UNKOWN_MCAST, rtk_switch_port_L2P_get(port), &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_MULTICAST:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_STORM_MCAST, rtk_switch_port_L2P_get(port), &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_BROADCAST:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_STORM_BCAST, rtk_switch_port_L2P_get(port), &regData)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            break;
    }

    *pEnable = (regData == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_storm_bypass_set
 * Description:
 *      Set bypass storm filter control configuration.
 * Input:
 *      type    - Bypass storm filter control type.
 *      enable  - Bypass status.
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
 *      This API can set per-port bypass stomr filter control frame type including RMA and igmp.
 *      The bypass frame type is as following:
 *      - BYPASS_BRG_GROUP,
 *      - BYPASS_FD_PAUSE,
 *      - BYPASS_SP_MCAST,
 *      - BYPASS_1X_PAE,
 *      - BYPASS_UNDEF_BRG_04,
 *      - BYPASS_UNDEF_BRG_05,
 *      - BYPASS_UNDEF_BRG_06,
 *      - BYPASS_UNDEF_BRG_07,
 *      - BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - BYPASS_UNDEF_BRG_09,
 *      - BYPASS_UNDEF_BRG_0A,
 *      - BYPASS_UNDEF_BRG_0B,
 *      - BYPASS_UNDEF_BRG_0C,
 *      - BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - BYPASS_8021AB,
 *      - BYPASS_UNDEF_BRG_0F,
 *      - BYPASS_BRG_MNGEMENT,
 *      - BYPASS_UNDEFINED_11,
 *      - BYPASS_UNDEFINED_12,
 *      - BYPASS_UNDEFINED_13,
 *      - BYPASS_UNDEFINED_14,
 *      - BYPASS_UNDEFINED_15,
 *      - BYPASS_UNDEFINED_16,
 *      - BYPASS_UNDEFINED_17,
 *      - BYPASS_UNDEFINED_18,
 *      - BYPASS_UNDEFINED_19,
 *      - BYPASS_UNDEFINED_1A,
 *      - BYPASS_UNDEFINED_1B,
 *      - BYPASS_UNDEFINED_1C,
 *      - BYPASS_UNDEFINED_1D,
 *      - BYPASS_UNDEFINED_1E,
 *      - BYPASS_UNDEFINED_1F,
 *      - BYPASS_GMRP,
 *      - BYPASS_GVRP,
 *      - BYPASS_UNDEF_GARP_22,
 *      - BYPASS_UNDEF_GARP_23,
 *      - BYPASS_UNDEF_GARP_24,
 *      - BYPASS_UNDEF_GARP_25,
 *      - BYPASS_UNDEF_GARP_26,
 *      - BYPASS_UNDEF_GARP_27,
 *      - BYPASS_UNDEF_GARP_28,
 *      - BYPASS_UNDEF_GARP_29,
 *      - BYPASS_UNDEF_GARP_2A,
 *      - BYPASS_UNDEF_GARP_2B,
 *      - BYPASS_UNDEF_GARP_2C,
 *      - BYPASS_UNDEF_GARP_2D,
 *      - BYPASS_UNDEF_GARP_2E,
 *      - BYPASS_UNDEF_GARP_2F,
 *      - BYPASS_IGMP.
 *      - BYPASS_CDP.
 *      - BYPASS_CSSTP.
 *      - BYPASS_LLDP.
 */
rtk_api_ret_t dal_rtl8367d_storm_bypass_set(rtk_storm_bypass_t type, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 index;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= BYPASS_END)
        return RT_ERR_INPUT;

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if (type >= 0 && type <= BYPASS_UNDEF_GARP_2F)
    {
        if( (type >= 0x4 && type <= 0x7) || (type >= 0x9 && type <= 0x0C) || (0x0F == type))
            index = 0x04;
        else if((type >= 0x13 && type <= 0x17) || (0x19 == type) || (type >= 0x1B && type <= 0x1f))
            index = 0x13;
        else if(type >= 0x22 && type <= 0x2F)
            index = 0x22;
        else
            index = (rtk_uint32)type;

        if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL00 + index, RTL8367D_RMA_CTRL00_DISCARD_STORM_FILTER_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
            return retVal;
    }
    else if(type == BYPASS_IGMP)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG0, RTL8367D_IGMP_MLD_DISCARD_STORM_FILTER_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
            return retVal;
    }
    else if (type == BYPASS_CDP)
    {
        if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_CDP, RTL8367D_RMA_CTRL_CDP_DISCARD_STORM_FILTER_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == BYPASS_CSSTP)
    {
        if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_CSSTP, RTL8367D_RMA_CTRL_CSSTP_DISCARD_STORM_FILTER_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == BYPASS_LLDP)
    {
        if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_LLDP, RTL8367D_RMA_CTRL_LLDP_DISCARD_STORM_FILTER_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
            return retVal;
    }
    else
        return RT_ERR_INPUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_storm_bypass_get
 * Description:
 *      Get bypass storm filter control configuration.
 * Input:
 *      type - Bypass storm filter control type.
 * Output:
 *      pEnable - Bypass status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get per-port bypass stomr filter control frame type including RMA and igmp.
 *      The bypass frame type is as following:
 *      - BYPASS_BRG_GROUP,
 *      - BYPASS_FD_PAUSE,
 *      - BYPASS_SP_MCAST,
 *      - BYPASS_1X_PAE,
 *      - BYPASS_UNDEF_BRG_04,
 *      - BYPASS_UNDEF_BRG_05,
 *      - BYPASS_UNDEF_BRG_06,
 *      - BYPASS_UNDEF_BRG_07,
 *      - BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - BYPASS_UNDEF_BRG_09,
 *      - BYPASS_UNDEF_BRG_0A,
 *      - BYPASS_UNDEF_BRG_0B,
 *      - BYPASS_UNDEF_BRG_0C,
 *      - BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - BYPASS_8021AB,
 *      - BYPASS_UNDEF_BRG_0F,
 *      - BYPASS_BRG_MNGEMENT,
 *      - BYPASS_UNDEFINED_11,
 *      - BYPASS_UNDEFINED_12,
 *      - BYPASS_UNDEFINED_13,
 *      - BYPASS_UNDEFINED_14,
 *      - BYPASS_UNDEFINED_15,
 *      - BYPASS_UNDEFINED_16,
 *      - BYPASS_UNDEFINED_17,
 *      - BYPASS_UNDEFINED_18,
 *      - BYPASS_UNDEFINED_19,
 *      - BYPASS_UNDEFINED_1A,
 *      - BYPASS_UNDEFINED_1B,
 *      - BYPASS_UNDEFINED_1C,
 *      - BYPASS_UNDEFINED_1D,
 *      - BYPASS_UNDEFINED_1E,
 *      - BYPASS_UNDEFINED_1F,
 *      - BYPASS_GMRP,
 *      - BYPASS_GVRP,
 *      - BYPASS_UNDEF_GARP_22,
 *      - BYPASS_UNDEF_GARP_23,
 *      - BYPASS_UNDEF_GARP_24,
 *      - BYPASS_UNDEF_GARP_25,
 *      - BYPASS_UNDEF_GARP_26,
 *      - BYPASS_UNDEF_GARP_27,
 *      - BYPASS_UNDEF_GARP_28,
 *      - BYPASS_UNDEF_GARP_29,
 *      - BYPASS_UNDEF_GARP_2A,
 *      - BYPASS_UNDEF_GARP_2B,
 *      - BYPASS_UNDEF_GARP_2C,
 *      - BYPASS_UNDEF_GARP_2D,
 *      - BYPASS_UNDEF_GARP_2E,
 *      - BYPASS_UNDEF_GARP_2F,
 *      - BYPASS_IGMP.
 *      - BYPASS_CDP.
 *      - BYPASS_CSSTP.
 *      - BYPASS_LLDP.
 */
rtk_api_ret_t dal_rtl8367d_storm_bypass_get(rtk_storm_bypass_t type, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 index;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= BYPASS_END)
        return RT_ERR_INPUT;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if (type >= 0 && type <= BYPASS_UNDEF_GARP_2F)
    {
        if( (type >= 0x4 && type <= 0x7) || (type >= 0x9 && type <= 0x0C) || (0x0F == type))
            index = 0x04;
        else if((type >= 0x13 && type <= 0x17) || (0x19 == type) || (type >= 0x1B && type <= 0x1f))
            index = 0x13;
        else if(type >= 0x22 && type <= 0x2F)
            index = 0x22;
        else
            index = (rtk_uint32)type;

        if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL00 + index, RTL8367D_RMA_CTRL00_DISCARD_STORM_FILTER_OFFSET, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else if(type == BYPASS_IGMP)
    {
        if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG0, RTL8367D_IGMP_MLD_DISCARD_STORM_FILTER_OFFSET, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else if (type == BYPASS_CDP)
    {
        if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_CDP, RTL8367D_RMA_CTRL_CDP_DISCARD_STORM_FILTER_OFFSET, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == BYPASS_CSSTP)
    {
        if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_CSSTP, RTL8367D_RMA_CTRL_CSSTP_DISCARD_STORM_FILTER_OFFSET, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else if (type  == BYPASS_LLDP)
    {
        if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_LLDP, RTL8367D_RMA_CTRL_LLDP_DISCARD_STORM_FILTER_OFFSET, &regData)) != RT_ERR_OK)
            return retVal;
    }
    else
        return RT_ERR_INPUT;

    *pEnable = (regData == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rate_stormControlExtPortmask_set
 * Description:
 *      Set externsion storm control port mask
 * Input:
 *      pPortmask  - port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_rate_stormControlExtPortmask_set(rtk_portmask_t *pPortmask)
{
    rtk_api_ret_t retVal;
    rtk_uint32 pmask;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtk_switch_portmask_L2P_get(pPortmask, &pmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_STORM_EXT_CFG, RTL8367D_STORM_EXT_EN_PORTMASK_MASK, pmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rate_stormControlExtPortmask_get
 * Description:
 *      Set externsion storm control port mask
 * Input:
 *      None
 * Output:
 *      pPortmask  - port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_rate_stormControlExtPortmask_get(rtk_portmask_t *pPortmask)
{
    rtk_api_ret_t retVal;
    rtk_uint32 pmask;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_STORM_EXT_CFG, RTL8367D_STORM_EXT_EN_PORTMASK_MASK, &pmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtk_switch_portmask_P2L_get(pmask, pPortmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rate_stormControlExtEnable_set
 * Description:
 *      Set externsion storm control state
 * Input:
 *      stormType   - storm group type
 *      enable      - externsion storm control state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_rate_stormControlExtEnable_set(rtk_rate_storm_group_t stormType, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_STORM_EXT_CFG, RTL8367D_STORM_UNKNOWN_UCAST_EXT_EN_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_STORM_EXT_CFG, RTL8367D_STORM_UNKNOWN_MCAST_EXT_EN_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_MULTICAST:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_STORM_EXT_CFG, RTL8367D_STORM_MCAST_EXT_EN_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_BROADCAST:
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_STORM_EXT_CFG, RTL8367D_STORM_BCAST_EXT_EN_OFFSET, (enable == ENABLED) ? 1 : 0)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rate_stormControlExtEnable_get
 * Description:
 *      Get externsion storm control state
 * Input:
 *      stormType   - storm group type
 * Output:
 *      pEnable     - externsion storm control state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_rate_stormControlExtEnable_get(rtk_rate_storm_group_t stormType, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData = 0;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_STORM_EXT_CFG, RTL8367D_STORM_UNKNOWN_UCAST_EXT_EN_OFFSET, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_STORM_EXT_CFG, RTL8367D_STORM_UNKNOWN_MCAST_EXT_EN_OFFSET, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_MULTICAST:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_STORM_EXT_CFG, RTL8367D_STORM_MCAST_EXT_EN_OFFSET, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_BROADCAST:
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_STORM_EXT_CFG, RTL8367D_STORM_BCAST_EXT_EN_OFFSET, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            break;
    }

    *pEnable = (regData == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rate_stormControlExtMeterIdx_set
 * Description:
 *      Set externsion storm control meter index
 * Input:
 *      stormType   - storm group type
 *      index       - externsion storm control state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_rate_stormControlExtMeterIdx_set(rtk_rate_storm_group_t stormType, rtk_uint32 index)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (index > RTK_MAX_METER_ID)
        return RT_ERR_FILTER_METER_ID;

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_STORM_EXT_MTRIDX_CFG1, RTL8367D_UNUC_STORM_EXT_METERIDX_MASK, index))!=RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_STORM_EXT_MTRIDX_CFG1, RTL8367D_UNMC_STORM_EXT_METERIDX_MASK, index))!=RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_MULTICAST:
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_STORM_EXT_MTRIDX_CFG0, RTL8367D_MC_STORM_EXT_METERIDX_MASK, index))!=RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_BROADCAST:
            if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_STORM_EXT_MTRIDX_CFG0, RTL8367D_BC_STORM_EXT_METERIDX_MASK, index))!=RT_ERR_OK)
                return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rate_stormControlExtMeterIdx_get
 * Description:
 *      Get externsion storm control meter index
 * Input:
 *      stormType   - storm group type
 *      pIndex      - externsion storm control state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_rate_stormControlExtMeterIdx_get(rtk_rate_storm_group_t stormType, rtk_uint32 *pIndex)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if(NULL == pIndex)
        return RT_ERR_NULL_POINTER;

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
            if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_STORM_EXT_MTRIDX_CFG1, RTL8367D_UNUC_STORM_EXT_METERIDX_MASK, pIndex))!=RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
            if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_STORM_EXT_MTRIDX_CFG1, RTL8367D_UNMC_STORM_EXT_METERIDX_MASK, pIndex))!=RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_MULTICAST:
            if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_STORM_EXT_MTRIDX_CFG0, RTL8367D_MC_STORM_EXT_METERIDX_MASK, pIndex))!=RT_ERR_OK)
                return retVal;
            break;
        case STORM_GROUP_BROADCAST:
            if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_STORM_EXT_MTRIDX_CFG0, RTL8367D_BC_STORM_EXT_METERIDX_MASK, pIndex))!=RT_ERR_OK)
                return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

