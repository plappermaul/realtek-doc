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
 * Feature : Here is a list of all functions and variables in Leaky module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_leaky.h>
#include <linux/string.h>
#include <dal/rtl8367d/rtl8367d_asicdrv.h>

/* Function Name:
 *      dal_rtl8367d_leaky_vlan_set
 * Description:
 *      Set VLAN leaky.
 * Input:
 *      type - Packet type for VLAN leaky.
 *      enable - Leaky status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      This API can set VLAN leaky for RMA ,IGMP/MLD, CDP, CSSTP, and LLDP packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_IPMULTICAST.
 *      - LEAKY_CDP,
 *      - LEAKY_CSSTP,
 *      - LEAKY_LLDP.
 */
rtk_api_ret_t dal_rtl8367d_leaky_vlan_set(rtk_leaky_type_t type, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 port;
    rtk_uint32 index;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= LEAKY_END)
        return RT_ERR_INPUT;

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if( (type >= 0x4 && type <= 0x7) || (type >= 0x9 && type <= 0x0C) || (0x0F == type))
        index = 0x04;
    else if((type >= 0x13 && type <= 0x17) || (0x19 == type) || (type >= 0x1B && type <= 0x1f))
        index = 0x13;
    else if(type >= 0x22 && type <= 0x2F)
        index = 0x22;
    else
        index = type;	

    if (index <= LEAKY_UNDEF_GARP_2F)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL00+index, RTL8367D_RMA_CTRL00_VLAN_LEAKY_OFFSET, enable)) != RT_ERR_OK)
            return retVal;
    }
    else if (LEAKY_IPMULTICAST == index)
    {
        for (port = 0; port <= RTK_PORT_ID_MAX; port++)
        {          
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_IPMCAST_VLAN_LEAKY, port, enable)) != RT_ERR_OK)
                return retVal;
        }
    }
    else if (LEAKY_IGMP == index)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG0, RTL8367D_IGMP_MLD_VLAN_LEAKY_OFFSET, enable)) != RT_ERR_OK)
            return retVal;
    }
    else if (LEAKY_CDP == index)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_CDP, RTL8367D_RMA_CTRL_CDP_VLAN_LEAKY_OFFSET, enable)) != RT_ERR_OK)
            return retVal;
    }
    else if (LEAKY_CSSTP == index)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_CSSTP, RTL8367D_RMA_CTRL_CSSTP_VLAN_LEAKY_OFFSET, enable)) != RT_ERR_OK)
            return retVal;
    }
    else if (LEAKY_LLDP == index)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_LLDP, RTL8367D_RMA_CTRL_LLDP_VLAN_LEAKY_OFFSET, enable)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_leaky_vlan_get
 * Description:
 *      Get VLAN leaky.
 * Input:
 *      type - Packet type for VLAN leaky.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get VLAN leaky status for RMA ,IGMP/MLD, CDP, CSSTP, and LLDP  packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_IPMULTICAST.
 *      - LEAKY_CDP,
 *      - LEAKY_CSSTP,
 *      - LEAKY_LLDP.
 */
rtk_api_ret_t dal_rtl8367d_leaky_vlan_get(rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 port,tmp, index;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= LEAKY_END)
        return RT_ERR_INPUT;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if( (type >= 0x4 && type <= 0x7) || (type >= 0x9 && type <= 0x0C) || (0x0F == type))
        index = 0x04;
    else if((type >= 0x13 && type <= 0x17) || (0x19 == type) || (type >= 0x1B && type <= 0x1f))
        index = 0x13;
    else if(type >= 0x22 && type <= 0x2F)
        index = 0x22;
    else
        index = type;

    if (index <= LEAKY_UNDEF_GARP_2F)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL00+index, RTL8367D_RMA_CTRL00_VLAN_LEAKY_OFFSET, &tmp)) != RT_ERR_OK)
            return retVal;
        *pEnable = tmp;
    }
    else if (LEAKY_IPMULTICAST == index)
    {
        for (port = 0; port <= RTK_PORT_ID_MAX; port++)
        {
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_IPMCAST_VLAN_LEAKY, port, &tmp)) != RT_ERR_OK)
                return retVal;
            if (port>0&&(tmp!=*pEnable))
                return RT_ERR_FAILED;
            *pEnable = tmp;
        }
    }
    else if (LEAKY_IGMP == index)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG0, RTL8367D_IGMP_MLD_VLAN_LEAKY_OFFSET, &tmp)) != RT_ERR_OK)
            return retVal;    
        *pEnable = tmp;
    }
    else if (LEAKY_CDP == index)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_CDP, RTL8367D_RMA_CTRL_CDP_VLAN_LEAKY_OFFSET, &tmp)) != RT_ERR_OK)
            return retVal;
        *pEnable = tmp;
    }
    else if (LEAKY_CSSTP == index)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_CSSTP, RTL8367D_RMA_CTRL_CSSTP_VLAN_LEAKY_OFFSET, &tmp)) != RT_ERR_OK)
            return retVal;
        *pEnable = tmp;
    }
    else if (LEAKY_LLDP == index)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_LLDP, RTL8367D_RMA_CTRL_LLDP_VLAN_LEAKY_OFFSET, &tmp)) != RT_ERR_OK)
            return retVal;
        *pEnable = tmp;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_leaky_portIsolation_set
 * Description:
 *      Set port isolation leaky.
 * Input:
 *      type - Packet type for port isolation leaky.
 *      enable - Leaky status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      This API can set port isolation leaky for RMA ,IGMP/MLD, CDP, CSSTP, and LLDP  packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_IPMULTICAST.
 *      - LEAKY_CDP,
 *      - LEAKY_CSSTP,
 *      - LEAKY_LLDP.
 */
rtk_api_ret_t dal_rtl8367d_leaky_portIsolation_set(rtk_leaky_type_t type, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 port, index;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= LEAKY_END)
        return RT_ERR_INPUT;

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if( (type >= 0x4 && type <= 0x7) || (type >= 0x9 && type <= 0x0C) || (0x0F == type))
        index = 0x04;
    else if((type >= 0x13 && type <= 0x17) || (0x19 == type) || (type >= 0x1B && type <= 0x1f))
        index = 0x13;
    else if(type >= 0x22 && type <= 0x2F)
        index = 0x22;
    else
        index = type;

    if (index <= LEAKY_UNDEF_GARP_2F)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL00+index, RTL8367D_RMA_CTRL00_PORTISO_LEAKY_OFFSET, enable)) != RT_ERR_OK)
            return retVal;
    }
    else if (LEAKY_IPMULTICAST == index)
    {
        for (port = 0; port <= RTK_PORT_ID_MAX; port++)
        {          
            if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_IPMCAST_PORTISO_LEAKY, port, enable)) != RT_ERR_OK)
                return retVal;
        }
    }
    else if (LEAKY_IGMP == index)
    {  
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG0, RTL8367D_IGMP_MLD_PORTISO_LEAKY_OFFSET, enable)) != RT_ERR_OK)
            return retVal;
    }
    else if (LEAKY_CDP == index)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_CDP, RTL8367D_RMA_CTRL_CDP_PORTISO_LEAKY_OFFSET, enable)) != RT_ERR_OK)
            return retVal;
    }
    else if (LEAKY_CSSTP == index)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_CSSTP, RTL8367D_RMA_CTRL_CSSTP_PORTISO_LEAKY_OFFSET, enable)) != RT_ERR_OK)
            return retVal;
    }
    else if (LEAKY_LLDP == index)
    {
        if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RMA_CTRL_LLDP, RTL8367D_RMA_CTRL_LLDP_PORTISO_LEAKY_OFFSET, enable)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_leaky_portIsolation_get
 * Description:
 *      Get port isolation leaky.
 * Input:
 *      type - Packet type for port isolation leaky.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get port isolation leaky status for RMA ,IGMP/MLD, CDP, CSSTP, and LLDP  packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_IPMULTICAST.
 *      - LEAKY_CDP,
 *      - LEAKY_CSSTP,
 *      - LEAKY_LLDP.
 */
rtk_api_ret_t dal_rtl8367d_leaky_portIsolation_get(rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 port, tmp, index;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (type >= LEAKY_END)
        return RT_ERR_INPUT;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if( (type >= 0x4 && type <= 0x7) || (type >= 0x9 && type <= 0x0C) || (0x0F == type))
        index = 0x04;
    else if((type >= 0x13 && type <= 0x17) || (0x19 == type) || (type >= 0x1B && type <= 0x1f))
        index = 0x13;
    else if(type >= 0x22 && type <= 0x2F)
        index = 0x22;
	else
        index = type;

    if (index <= LEAKY_UNDEF_GARP_2F)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL00+index, RTL8367D_RMA_CTRL00_PORTISO_LEAKY_OFFSET, &tmp)) != RT_ERR_OK)
            return retVal;
        *pEnable = tmp;
    }
    else if (LEAKY_IPMULTICAST == index)
    {
        for (port = 0; port <= RTK_PORT_ID_MAX; port++)
        {
            if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_IPMCAST_PORTISO_LEAKY, port, &tmp)) != RT_ERR_OK)
                return retVal;
            if (port>0&&(tmp!=*pEnable))
                return RT_ERR_FAILED;
            *pEnable = tmp;
        }
    }
    else if (LEAKY_IGMP == index)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_IGMP_MLD_CFG0, RTL8367D_IGMP_MLD_PORTISO_LEAKY_OFFSET, &tmp)) != RT_ERR_OK)
            return retVal;    
        *pEnable = tmp;
    }
    else if (LEAKY_CDP == index)
    {	
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_CDP, RTL8367D_RMA_CTRL_CDP_PORTISO_LEAKY_OFFSET, &tmp)) != RT_ERR_OK)
            return retVal;
        *pEnable = tmp;
    }
    else if (LEAKY_CSSTP == index)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_CSSTP, RTL8367D_RMA_CTRL_CSSTP_PORTISO_LEAKY_OFFSET, &tmp)) != RT_ERR_OK)
            return retVal;
        *pEnable = tmp;
    }
    else if (LEAKY_LLDP == index)
    {
        if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RMA_CTRL_LLDP, RTL8367D_RMA_CTRL_LLDP_PORTISO_LEAKY_OFFSET, &tmp)) != RT_ERR_OK)
            return retVal;
        *pEnable = tmp;
    }

    return RT_ERR_OK;
}

