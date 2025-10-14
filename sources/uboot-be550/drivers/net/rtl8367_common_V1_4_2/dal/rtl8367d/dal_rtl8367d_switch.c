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
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *
 */


/*
 * Include Files
 */
#include <dal/rtl8367d/dal_rtl8367d_switch.h>
#include <dal/rtl8367d/rtl8367d_asicdrv.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl8367d_switch_init
 * Description:
 *      Set chip to default configuration enviroment
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can set chip registers to default configuration for different release chip model.
 */
rtk_api_ret_t dal_rtl8367d_switch_init(void)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_switch_portMaxPktLen_set
 * Description:
 *      Set Max packet length
 * Input:
 *      port    - Port ID
 *      speed   - Speed
 *      cfgId   - Configuration ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
rtk_api_ret_t dal_rtl8367d_switch_portMaxPktLen_set(rtk_port_t port, rtk_switch_maxPktLen_linkSpeed_t speed, rtk_uint32 cfgId)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(speed >= MAXPKTLEN_LINK_SPEED_END)
        return RT_ERR_INPUT;

    if(cfgId > MAXPKTLEN_CFG_ID_MAX)
        return RT_ERR_INPUT;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_MAX_LENGTH_CFG, (speed * 8) + rtk_switch_port_L2P_get(port), cfgId)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_switch_portMaxPktLen_get
 * Description:
 *      Get Max packet length
 * Input:
 *      port    - Port ID
 *      speed   - Speed
 * Output:
 *      pCfgId  - Configuration ID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
rtk_api_ret_t dal_rtl8367d_switch_portMaxPktLen_get(rtk_port_t port, rtk_switch_maxPktLen_linkSpeed_t speed, rtk_uint32 *pCfgId)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(speed >= MAXPKTLEN_LINK_SPEED_END)
        return RT_ERR_INPUT;

    if(NULL == pCfgId)
        return RT_ERR_NULL_POINTER;

    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_MAX_LENGTH_CFG, (speed * 8) + rtk_switch_port_L2P_get(port), pCfgId)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_switch_maxPktLenCfg_set
 * Description:
 *      Set Max packet length configuration
 * Input:
 *      cfgId   - Configuration ID
 *      pktLen  - Max packet length
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
rtk_api_ret_t dal_rtl8367d_switch_maxPktLenCfg_set(rtk_uint32 cfgId, rtk_uint32 pktLen)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(cfgId > MAXPKTLEN_CFG_ID_MAX)
        return RT_ERR_INPUT;

    if(pktLen > RTK_SWITCH_MAX_PKTLEN)
        return RT_ERR_INPUT;

    if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_MAX_LEN_RX_TX_CFG0 + cfgId, RTL8367D_MAX_LEN_RX_TX_CFG0_MASK, pktLen)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_switch_maxPktLenCfg_get
 * Description:
 *      Get Max packet length configuration
 * Input:
 *      cfgId   - Configuration ID
 *      pPktLen - Max packet length
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
rtk_api_ret_t dal_rtl8367d_switch_maxPktLenCfg_get(rtk_uint32 cfgId, rtk_uint32 *pPktLen)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(cfgId > MAXPKTLEN_CFG_ID_MAX)
        return RT_ERR_INPUT;

    if(NULL == pPktLen)
        return RT_ERR_NULL_POINTER;

    if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_MAX_LEN_RX_TX_CFG0 + cfgId, RTL8367D_MAX_LEN_RX_TX_CFG0_MASK, pPktLen)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

