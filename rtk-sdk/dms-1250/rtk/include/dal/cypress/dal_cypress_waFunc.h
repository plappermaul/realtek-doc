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

#ifndef __DAL_CYPRESS_WAFUNC_H__
#define __DAL_CYPRESS_WAFUNC_H__

/* Function Name:
 *      dal_cypress_port_phy_watchdog
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
 *      The API is monitor for phy problem and patch it.
 *      Protect PHY page by port semaphore.
 */
extern int32
dal_cypress_port_phy_watchdog(uint32 unit);

#if defined(CONFIG_SDK_WA_SERDES_WATCHDOG)
/* Function Name:
 *      dal_cypress_port_serdes_watchdog
 * Description:
 *      Monitor for serdes link statuse.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      The API is monitor serdes link down and patch it.
 *      Protect PHY page by port semaphore.
 */
int32
dal_cypress_port_serdes_watchdog(uint32 unit);
#endif

#endif /* __DAL_CYPRESS_WAFUNC_H__ */

