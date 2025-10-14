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
 * Feature : Here is a list of all functions and variables in Mirror module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_mirror.h>
#include <dal/rtl8367d/rtl8367d_asicdrv.h>


/* Function Name:
 *      dal_rtl8367d_mirror_portBased_set
 * Description:
 *      Set port mirror function.
 * Input:
 *      mirroring_port          - Monitor port.
 *      pMirrored_rx_portmask   - Rx mirror port mask.
 *      pMirrored_tx_portmask   - Tx mirror port mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 * Note:
 *      The API is to set mirror function of source port and mirror port.
 *      The mirror port can only be set to one port and the TX and RX mirror ports
 *      should be identical.
 */
rtk_api_ret_t dal_rtl8367d_mirror_portBased_set(rtk_port_t mirroring_port, rtk_portmask_t *pMirrored_rx_portmask, rtk_portmask_t *pMirrored_tx_portmask)
{
    rtk_api_ret_t retVal;
    rtk_uint32 rxPmask;
    rtk_uint32 txPmask;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check port valid */
    RTK_CHK_PORT_VALID(mirroring_port);

    if(NULL == pMirrored_rx_portmask)
        return RT_ERR_NULL_POINTER;

    if(NULL == pMirrored_tx_portmask)
        return RT_ERR_NULL_POINTER;

    RTK_CHK_PORTMASK_VALID(pMirrored_rx_portmask);

    RTK_CHK_PORTMASK_VALID(pMirrored_tx_portmask);

    /*mirror port != source port*/
    if(RTK_PORTMASK_IS_PORT_SET((*pMirrored_tx_portmask), mirroring_port) || RTK_PORTMASK_IS_PORT_SET((*pMirrored_rx_portmask), mirroring_port))
        return RT_ERR_PORT_MASK;

    /* Configure source portmask */
    if ((retVal = rtk_switch_portmask_L2P_get(pMirrored_rx_portmask, &rxPmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtk_switch_portmask_L2P_get(pMirrored_tx_portmask, &txPmask)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_MIRROR_SRC_PMSK, RTL8367D_MIRROR_TX_PMSK_MASK, txPmask)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_MIRROR_SRC_PMSK, RTL8367D_MIRROR_RX_PMSK_MASK, rxPmask)) != RT_ERR_OK)
        return retVal;

    /* Configure monitor(destination) port */
    if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_MIRROR_CTRL, RTL8367D_MIRROR_MONITOR_PORT_MASK, rtk_switch_port_L2P_get(mirroring_port))) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl8367d_mirror_portBased_get
 * Description:
 *      Get port mirror function.
 * Input:
 *      None
 * Output:
 *      pMirroring_port         - Monitor port.
 *      pMirrored_rx_portmask   - Rx mirror port mask.
 *      pMirrored_tx_portmask   - Tx mirror port mask.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror function of source port and mirror port.
 */
rtk_api_ret_t dal_rtl8367d_mirror_portBased_get(rtk_port_t *pMirroring_port, rtk_portmask_t *pMirrored_rx_portmask, rtk_portmask_t *pMirrored_tx_portmask)
{
    rtk_api_ret_t retVal;
    rtk_uint32 rxPmask;
    rtk_uint32 txPmask;
    rtk_uint32 mport;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pMirrored_rx_portmask)
        return RT_ERR_NULL_POINTER;

    if(NULL == pMirrored_tx_portmask)
        return RT_ERR_NULL_POINTER;

    if(NULL == pMirroring_port)
        return RT_ERR_NULL_POINTER;

    /* Get source portmask */
    if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_MIRROR_SRC_PMSK, RTL8367D_MIRROR_TX_PMSK_MASK, &txPmask)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_MIRROR_SRC_PMSK, RTL8367D_MIRROR_RX_PMSK_MASK, &rxPmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtk_switch_portmask_P2L_get(txPmask, pMirrored_tx_portmask)) != RT_ERR_OK)
            return retVal;

    if ((retVal = rtk_switch_portmask_P2L_get(rxPmask, pMirrored_rx_portmask)) != RT_ERR_OK)
            return retVal;

    /* Get monitor(destination) port */
    if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_MIRROR_CTRL, RTL8367D_MIRROR_MONITOR_PORT_MASK, &mport)) != RT_ERR_OK)
        return retVal;

    *pMirroring_port = rtk_switch_port_P2L_get(mport);
    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl8367d_mirror_portIso_set
 * Description:
 *      Set mirror port isolation.
 * Input:
 *      enable |Mirror isolation status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set mirror isolation function that prevent normal forwarding packets to miror port.
 */
rtk_api_ret_t dal_rtl8367d_mirror_portIso_set(rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 isoEn;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    isoEn = (enable == ENABLED) ? 1 : 0;
    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIRROR_CTRL, RTL8367D_MIRROR_ISO_OFFSET, isoEn)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_mirror_portIso_get
 * Description:
 *      Get mirror port isolation.
 * Input:
 *      None
 * Output:
 *      pEnable |Mirror isolation status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror isolation status.
 */
rtk_api_ret_t dal_rtl8367d_mirror_portIso_get(rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 isoEn;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIRROR_CTRL, RTL8367D_MIRROR_ISO_OFFSET, &isoEn)) != RT_ERR_OK)
        return retVal;

    *pEnable = (isoEn == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_mirror_vlanLeaky_set
 * Description:
 *      Set mirror VLAN leaky.
 * Input:
 *      txenable -TX leaky enable.
 *      rxenable - RX leaky enable.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set mirror VLAN leaky function forwarding packets to miror port.
 */
rtk_api_ret_t dal_rtl8367d_mirror_vlanLeaky_set(rtk_enable_t txenable, rtk_enable_t rxenable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 txEn, rxEn;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((txenable >= RTK_ENABLE_END) ||(rxenable >= RTK_ENABLE_END))
        return RT_ERR_ENABLE;

    txEn = (txenable == ENABLED) ? 1 : 0;
    rxEn = (rxenable == ENABLED) ? 1 : 0;
    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIRROR_CTRL2, RTL8367D_MIRROR_TX_VLAN_LEAKY_OFFSET, txEn)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIRROR_CTRL2, RTL8367D_MIRROR_RX_VLAN_LEAKY_OFFSET, rxEn)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_mirror_vlanLeaky_get
 * Description:
 *      Get mirror VLAN leaky.
 * Input:
 *      None
 * Output:
 *      pTxenable - TX leaky enable.
 *      pRxenable - RX leaky enable.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror VLAN leaky status.
 */
rtk_api_ret_t dal_rtl8367d_mirror_vlanLeaky_get(rtk_enable_t *pTxenable, rtk_enable_t *pRxenable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 txEn, rxEn;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if( (NULL == pTxenable) || (NULL == pRxenable) )
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIRROR_CTRL2, RTL8367D_MIRROR_TX_VLAN_LEAKY_OFFSET, &txEn)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIRROR_CTRL2, RTL8367D_MIRROR_RX_VLAN_LEAKY_OFFSET, &rxEn)) != RT_ERR_OK)
        return retVal;

    *pTxenable = (txEn == 1) ? ENABLED : DISABLED;
    *pRxenable = (rxEn == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_mirror_isolationLeaky_set
 * Description:
 *      Set mirror Isolation leaky.
 * Input:
 *      txenable -TX leaky enable.
 *      rxenable - RX leaky enable.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set mirror VLAN leaky function forwarding packets to miror port.
 */
rtk_api_ret_t dal_rtl8367d_mirror_isolationLeaky_set(rtk_enable_t txenable, rtk_enable_t rxenable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 txEn, rxEn;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((txenable >= RTK_ENABLE_END) ||(rxenable >= RTK_ENABLE_END))
        return RT_ERR_ENABLE;

    txEn = (txenable == ENABLED) ? 1 : 0;
    rxEn = (rxenable == ENABLED) ? 1 : 0;
    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIRROR_CTRL2, RTL8367D_MIRROR_TX_ISOLATION_LEAKY_OFFSET, txEn)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIRROR_CTRL2, RTL8367D_MIRROR_RX_ISOLATION_LEAKY_OFFSET, rxEn)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_mirror_isolationLeaky_get
 * Description:
 *      Get mirror isolation leaky.
 * Input:
 *      None
 * Output:
 *      pTxenable - TX leaky enable.
 *      pRxenable - RX leaky enable.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror isolation leaky status.
 */
rtk_api_ret_t dal_rtl8367d_mirror_isolationLeaky_get(rtk_enable_t *pTxenable, rtk_enable_t *pRxenable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 txEn, rxEn;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if( (NULL == pTxenable) || (NULL == pRxenable) )
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIRROR_CTRL2, RTL8367D_MIRROR_TX_ISOLATION_LEAKY_OFFSET, &txEn)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIRROR_CTRL2, RTL8367D_MIRROR_RX_ISOLATION_LEAKY_OFFSET, &rxEn)) != RT_ERR_OK)
        return retVal;

    *pTxenable = (txEn == 1) ? ENABLED : DISABLED;
    *pRxenable = (rxEn == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_mirror_keep_set
 * Description:
 *      Set mirror packet format keep.
 * Input:
 *      mode - -mirror keep mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The API is to set  -mirror keep mode.
 *      The mirror keep mode is as following:
 *      - MIRROR_FOLLOW_VLAN
 *      - MIRROR_KEEP_ORIGINAL
 *      - MIRROR_KEEP_END
 */
rtk_api_ret_t dal_rtl8367d_mirror_keep_set(rtk_mirror_keep_t mode)
{
    rtk_api_ret_t retVal;
    rtk_uint32 keepMode;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if (mode >= MIRROR_KEEP_END)
        return RT_ERR_ENABLE;

    keepMode = (mode == MIRROR_FOLLOW_VLAN) ? 0 : 1;
    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIRROR_CTRL2, RTL8367D_MIRROR_REALKEEP_EN_OFFSET, keepMode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_mirror_keep_get
 * Description:
 *      Get mirror packet format keep.
 * Input:
 *      None
 * Output:
 *      pMode -mirror keep mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API is to get mirror keep mode.
 *      The mirror keep mode is as following:
 *      - MIRROR_FOLLOW_VLAN
 *      - MIRROR_KEEP_ORIGINAL
 *      - MIRROR_KEEP_END
 */
rtk_api_ret_t dal_rtl8367d_mirror_keep_get(rtk_mirror_keep_t *pMode)
{
    rtk_api_ret_t retVal;
    rtk_uint32 keepMode;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pMode)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIRROR_CTRL2, RTL8367D_MIRROR_REALKEEP_EN_OFFSET, &keepMode)) != RT_ERR_OK)
        return retVal;

    *pMode = (keepMode == 0) ? MIRROR_FOLLOW_VLAN : MIRROR_KEEP_ORIGINAL;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_mirror_override_set
 * Description:
 *      Set port mirror override function.
 * Input:
 *      rxMirror        - 1: output mirrored packet, 0: output normal forward packet
 *      txMirror        - 1: output mirrored packet, 0: output normal forward packet
 *      aclMirror       - 1: output mirrored packet, 0: output normal forward packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API is to set mirror override function.
 *      This function control the output format when a port output
 *      normal forward & mirrored packet at the same time.
 */
rtk_api_ret_t dal_rtl8367d_mirror_override_set(rtk_enable_t rxMirror, rtk_enable_t txMirror, rtk_enable_t aclMirror)
{
    rtk_api_ret_t retVal;

    if( (rxMirror >= RTK_ENABLE_END) || (txMirror >= RTK_ENABLE_END) || (aclMirror >= RTK_ENABLE_END))
        return RT_ERR_ENABLE;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIRROR_OVERRIDE_CFG, RTL8367D_MIRROR_RX_OVERRIDE_EN_OFFSET, (rxMirror == ENABLED) ? 1 : 0)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIRROR_OVERRIDE_CFG, RTL8367D_MIRROR_TX_OVERRIDE_EN_OFFSET, (txMirror == ENABLED) ? 1 : 0)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MIRROR_OVERRIDE_CFG, RTL8367D_MIRROR_ACL_OVERRIDE_EN_OFFSET, (aclMirror == ENABLED) ? 1 : 0)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_mirror_override_get
 * Description:
 *      Get port mirror override function.
 * Input:
 *      None
 * Output:
 *      pRxMirror       - 1: output mirrored packet, 0: output normal forward packet
 *      pTxMirror       - 1: output mirrored packet, 0: output normal forward packet
 *      pAclMirror      - 1: output mirrored packet, 0: output normal forward packet
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - Null Pointer
 * Note:
 *      The API is to Get mirror override function.
 *      This function control the output format when a port output
 *      normal forward & mirrored packet at the same time.
 */
rtk_api_ret_t dal_rtl8367d_mirror_override_get(rtk_enable_t *pRxMirror, rtk_enable_t *pTxMirror, rtk_enable_t *pAclMirror)
{
    rtk_api_ret_t retVal;
    rtk_uint32 txEn, rxEn, aclEn;

    if( (pRxMirror == NULL) || (pTxMirror == NULL) || (pAclMirror == NULL))
        return RT_ERR_ENABLE;

    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIRROR_OVERRIDE_CFG, RTL8367D_MIRROR_RX_OVERRIDE_EN_OFFSET, &rxEn)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIRROR_OVERRIDE_CFG, RTL8367D_MIRROR_TX_OVERRIDE_EN_OFFSET, &txEn)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MIRROR_OVERRIDE_CFG, RTL8367D_MIRROR_ACL_OVERRIDE_EN_OFFSET, &aclEn)) != RT_ERR_OK)
        return retVal;

    *pRxMirror = (rxEn == 1) ? ENABLED : DISABLED;
    *pTxMirror = (txEn == 1) ? ENABLED : DISABLED;
    *pAclMirror = (aclEn == 1) ? ENABLED : DISABLED;
    return RT_ERR_OK;
}


