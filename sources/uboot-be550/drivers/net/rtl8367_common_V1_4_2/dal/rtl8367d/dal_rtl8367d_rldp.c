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
 * $Revision: $
 * $Date: $
 *
 * Purpose : Declaration of RLDP and RLPP API
 *
 * Feature : The file have include the following module and sub-modules
 *           1) RLDP and RLPP configuration and status
 *
 */


/*
 * Include Files
 */
#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_rldp.h>
#include <rtl8367d_asicdrv.h>


/* Function Name:
 *      dal_rtl8367d_rldp_config_set
 * Description:
 *      Set RLDP module configuration
 * Input:
 *      pConfig - configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8367d_rldp_config_set(rtk_rldp_config_t *pConfig)
{
    rtk_api_ret_t retVal;
    rtk_uint16 *magic;
    rtk_uint32 regData, i;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (pConfig->rldp_enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if (pConfig->trigger_mode >= RTK_RLDP_TRIGGER_END)
        return RT_ERR_INPUT;

    if (pConfig->compare_type >= RTK_RLDP_CMPTYPE_END)
        return RT_ERR_INPUT;

    if (pConfig->num_check >= RTK_RLDP_NUM_MAX)
        return RT_ERR_INPUT;

    if (pConfig->interval_check >= RTK_RLDP_INTERVAL_MAX)
        return RT_ERR_INPUT;

    if (pConfig->num_loop >= RTK_RLDP_NUM_MAX)
        return RT_ERR_INPUT;

    if (pConfig->interval_loop >= RTK_RLDP_INTERVAL_MAX)
        return RT_ERR_INPUT;


    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RLDP_CTRL0, RTL8367D_RLDP_ENABLE_OFFSET, pConfig->rldp_enable))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RLDP_CTRL0, RTL8367D_RLDP_TRIGGER_MODE_OFFSET, pConfig->trigger_mode))!=RT_ERR_OK)
        return retVal;

    magic = (rtk_uint16*)&pConfig->magic;
    for (i = 0; i < 3; i++)
    {
        regData = *magic;
        retVal = rtl8367d_setAsicReg(RTL8367D_REG_RLDP_MAGIC_NUM0 + i, regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        magic++;
    }

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_RLDP_CTRL0, RTL8367D_RLDP_COMP_ID_OFFSET, pConfig->compare_type))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RLDP_CTRL1, RTL8367D_RLDP_RETRY_COUNT_CHKSTATE_MASK, pConfig->num_check))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_RLDP_CTRL3, pConfig->interval_check))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RLDP_CTRL1, RTL8367D_RLDP_RETRY_COUNT_LOOPSTATE_MASK, pConfig->num_loop))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_RLDP_CTRL2, pConfig->interval_loop))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rldp_config_get
 * Description:
 *      Get RLDP module configuration
 * Input:
 *      None
 * Output:
 *      pConfig - configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8367d_rldp_config_get(rtk_rldp_config_t *pConfig)
{
    rtk_api_ret_t retVal;
    rtk_uint16 *magic;
    rtk_uint32 regData, i;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RLDP_CTRL0, RTL8367D_RLDP_ENABLE_OFFSET, &pConfig->rldp_enable))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RLDP_CTRL0, RTL8367D_RLDP_TRIGGER_MODE_OFFSET, &pConfig->trigger_mode))!=RT_ERR_OK)
        return retVal;

    magic = (rtk_uint16*)&pConfig->magic;
    for (i = 0; i < 3; i++)
    {
        retVal = rtl8367d_getAsicReg(RTL8367D_REG_RLDP_MAGIC_NUM0 + i, &regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        *magic = regData;
        magic++;
    }

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_RLDP_CTRL0, RTL8367D_RLDP_COMP_ID_OFFSET, &pConfig->compare_type))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RLDP_CTRL1, RTL8367D_RLDP_RETRY_COUNT_CHKSTATE_MASK, &pConfig->num_check))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_RLDP_CTRL3, &pConfig->interval_check))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RLDP_CTRL1, RTL8367D_RLDP_RETRY_COUNT_LOOPSTATE_MASK, &pConfig->num_loop))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_RLDP_CTRL2, &pConfig->interval_loop))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_rldp_portConfig_set
 * Description:
 *      Set per port RLDP module configuration
 * Input:
 *      port   - port number to be configured
 *      pPortConfig - per port configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8367d_rldp_portConfig_set(rtk_port_t port, rtk_rldp_portConfig_t *pPortConfig)
{
    rtk_api_ret_t retVal;
    rtk_uint32 portmask;
    rtk_uint32 phy_port;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (pPortConfig->tx_enable>= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    phy_port = rtk_switch_port_L2P_get(port);
    if (phy_port == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RLDP_CTRL4, RTL8367D_RLDP_CTRL4_MASK, &portmask))!=RT_ERR_OK)
        return retVal;

    if (pPortConfig->tx_enable)
    {
         portmask |=(1<<phy_port);
    }
    else
    {
         portmask &= ~(1<<phy_port);
    }

    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RLDP_CTRL4, RTL8367D_RLDP_CTRL4_MASK, portmask))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl8367d_rldp_portConfig_get
 * Description:
 *      Get per port RLDP module configuration
 * Input:
 *      port    - port number to be get
 * Output:
 *      pPortConfig - per port configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8367d_rldp_portConfig_get(rtk_port_t port, rtk_rldp_portConfig_t *pPortConfig)
{
    rtk_api_ret_t retVal;
    rtk_uint32 portmask;
    rtk_portmask_t logicalPmask;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RLDP_CTRL4, RTL8367D_RLDP_CTRL4_MASK, &portmask))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtk_switch_portmask_P2L_get(portmask, &logicalPmask)) != RT_ERR_OK)
        return retVal;


    if (logicalPmask.bits[0] & (1<<port))
    {
         pPortConfig->tx_enable = ENABLED;
    }
    else
    {
         pPortConfig->tx_enable = DISABLED;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_rtl8367d_rldp_status_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      None
 * Output:
 *      pStatus - status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8367d_rldp_status_get(rtk_rldp_status_t *pStatus)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;
    rtk_uint16 *accessPtr;
    rtk_uint32 i;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    accessPtr = (rtk_uint16*)&pStatus->id;
    for(i = 0; i < 3; i++)
    {
        retVal = rtl8367d_getAsicReg(RTL8367D_REG_RLDP_RAND_NUM0+ i, &regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        *accessPtr = regData;
        accessPtr++;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_rldp_portStatus_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      port    - port number to be get
 * Output:
 *      pPortStatus - per port status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8367d_rldp_portStatus_get(rtk_port_t port, rtk_rldp_portStatus_t *pPortStatus)
{
    rtk_api_ret_t retVal;
    rtk_uint32 portmask;
    rtk_portmask_t logicalPmask;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RLDP_LOOPSTATUS_INDICATOR, RTL8367D_RLDP_LOOPSTATUS_INDICATOR_MASK, &portmask))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtk_switch_portmask_P2L_get(portmask, &logicalPmask)) != RT_ERR_OK)
        return retVal;

    if (logicalPmask.bits[0] & (1<<port))
    {
         pPortStatus->loop_status = RTK_RLDP_LOOPSTS_LOOPING;
    }
    else
    {
         pPortStatus->loop_status  = RTK_RLDP_LOOPSTS_NONE;
    }

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RLDP_LOOPED_INDICATOR, RTL8367D_RLDP_LOOPED_INDICATOR_MASK, &portmask))!=RT_ERR_OK)
        return retVal;
    if ((retVal = rtk_switch_portmask_P2L_get(portmask, &logicalPmask)) != RT_ERR_OK)
        return retVal;

    if (logicalPmask.bits[0] & (1<<port))
    {
         pPortStatus->loop_enter = RTK_RLDP_LOOPSTS_LOOPING;
    }
    else
    {
         pPortStatus->loop_enter  = RTK_RLDP_LOOPSTS_NONE;
    }

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RLDP_RELEASED_INDICATOR, RTL8367D_RLDP_RELEASED_INDICATOR_MASK, &portmask))!=RT_ERR_OK)
        return retVal;
    if ((retVal = rtk_switch_portmask_P2L_get(portmask, &logicalPmask)) != RT_ERR_OK)
        return retVal;

    if (logicalPmask.bits[0] & (1<<port))
    {
         pPortStatus->loop_leave = RTK_RLDP_LOOPSTS_LOOPING;
    }
    else
    {
         pPortStatus->loop_leave  = RTK_RLDP_LOOPSTS_NONE;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_rldp_portStatus_clear
 * Description:
 *      Clear RLDP module status
 * Input:
 *      port    - port number to be clear
 *      pPortStatus - per port status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Clear operation effect loop_enter and loop_leave only, other field in
 *      the structure are don't care
 */
rtk_api_ret_t dal_rtl8367d_rldp_portStatus_set(rtk_port_t port, rtk_rldp_portStatus_t *pPortStatus)
{
    rtk_api_ret_t retVal;
    rtk_uint32 pmsk;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    pmsk = (pPortStatus->loop_enter) << phyPort;
    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RLDP_LOOPED_INDICATOR, RTL8367D_RLDP_LOOPED_INDICATOR_MASK, pmsk))!=RT_ERR_OK)
        return retVal;
    pmsk = (pPortStatus->loop_leave) << phyPort;
    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_RLDP_RELEASED_INDICATOR, RTL8367D_RLDP_RELEASED_INDICATOR_MASK, pmsk))!=RT_ERR_OK)
        return retVal;


    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_rldp_portLoopPair_get
 * Description:
 *      Get RLDP port loop pairs
 * Input:
 *      port    - port number to be get
 * Output:
 *      pPortmask - per port related loop ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8367d_rldp_portLoopPair_get(rtk_port_t port, rtk_portmask_t *pPortmask)
{
    rtk_api_ret_t retVal;
    rtk_uint32 pmsk;
    rtk_uint32 phy_port;
    rtk_uint32 loopedPair;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    phy_port = rtk_switch_port_L2P_get(port);

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_RLDP_LOOP_PORT_REG0 + (phy_port>>1), RTL8367D_RLDP_LOOP_PORT_00_MASK<<((phy_port&1)<<3), &loopedPair))!=RT_ERR_OK)
        return retVal;

    pmsk = 1 << loopedPair;
    if ((retVal = rtk_switch_portmask_P2L_get(pmsk, pPortmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

