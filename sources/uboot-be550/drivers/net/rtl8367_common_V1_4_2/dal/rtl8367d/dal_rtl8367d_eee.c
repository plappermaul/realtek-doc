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
 * Feature : Here is a list of all functions and variables in EEE module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_eee.h>
#include <dal/rtl8367d/dal_rtl8367d_port.h>
#include <linux/string.h>

#include <dal/rtl8367d/rtl8367d_asicdrv.h>

/* Function Name:
 *      dal_rtl8367d_eee_portEnable_set
 * Description:
 *      Set enable status of EEE function.
 * Input:
 *      port - port id.
 *      enable - enable EEE status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_ID - Invalid port number.
 *      RT_ERR_ENABLE - Invalid enable input.
 * Note:
 *      This API can set EEE function to the specific port.
 *      The configuration of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtk_api_ret_t dal_rtl8367d_eee_portEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check port is UTP port */
    RTK_CHK_PORT_IS_UTP(port);

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xA5D0, &regData)) != RT_ERR_OK)
        return retVal;

    if(enable)
    {
        regData |= (0x0001 << 1);
        regData |= (0x0001 << 2);
    }
    else
    {
        regData &= ~(0x0001 << 1);
        regData &= ~(0x0001 << 2);
    }

    if((retVal = dal_rtl8367d_port_phyOCPReg_set(port, 0xA5D0, regData)) != RT_ERR_OK)
        return retVal;

    /* Restart Nway */
    if ((retVal = dal_rtl8367d_port_phyReg_get(port, 0, &regData))!=RT_ERR_OK)
        return retVal;

    regData |= 0x0200;
    if ((retVal = dal_rtl8367d_port_phyReg_set(port, 0, regData))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_eee_portEnable_get
 * Description:
 *      Get enable status of EEE function
 * Input:
 *      port - Port id.
 * Output:
 *      pEnable - Back pressure status.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_ID - Invalid port number.
 * Note:
 *      This API can get EEE function to the specific port.
 *      The configuration of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtk_api_ret_t dal_rtl8367d_eee_portEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check port is UTP port */
    RTK_CHK_PORT_IS_UTP(port);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if((retVal = dal_rtl8367d_port_phyOCPReg_get(port, 0xA5D0, &regData)) != RT_ERR_OK)
        return retVal;

    if ((regData & 0x0006) == 0x0006)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}

