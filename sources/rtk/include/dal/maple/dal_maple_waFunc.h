/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : Definition those public waFunc APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Port
 *
 */

#ifndef __DAL_MAPLE_WAFUNC_H__
#define __DAL_MAPLE_WAFUNC_H__

/* Function Name:
 *      dal_maple_port_pktbuf_watchdog
 * Description:
 *      Monitor for packet buffer problem.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is monitor for detect packet buffer problem and recover it.
 */
extern int32
dal_maple_port_pktbuf_watchdog(uint32 unit);

/* Function Name:
 *      dal_maple_port_serdes_watchdog
 * Description:
 *      Monitor for serdes link status.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is monitor serdes link down and recover it.
 */
extern int32
dal_maple_port_serdes_watchdog(uint32 unit);


/* Function Name:
 *      dal_maple_port_phy_watchdog
 * Description:
 *      Monitor for phy problem.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is monitor for phy problem and recover it.
 */
extern int32
dal_maple_port_phy_watchdog(uint32 unit);

/* Function Name:
 *      dal_maple_port_watchdog_debug
 * Description:
 *      Set watchdog debug counter.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is used to set watchdog debug counter.
 */
extern int32
dal_maple_port_watchdog_debug(uint32 unit);


#if defined(CONFIG_SDK_WA_COMBO_FLOWCONTROL)
/* Function Name:
 *      dal_maple_port_comboPort_workaround
 * Description:
 *      Monitor for phy(combo port) flow control on fiber mode.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is Monitor for phy(combo port) flow control on fiber mode.
 */
extern int32
dal_maple_port_comboPort_workaround(uint32 unit);
#endif

#endif /* __DAL_MAPLE_WAFUNC_H__ */


