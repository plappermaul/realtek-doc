/*
 * Copyright (C) 2009-2020 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 91355 $
 * $Date: 2018-08-13 21:54:45 +0800 (Mon, 13 Aug 2018) $
 *
 * Purpose : Definition rtk util function.
 *
 * Feature : rtk util function
 *
 */

#ifndef __RTK_UTIL_H__
#define __RTK_UTIL_H__

/* Function Name:
 *      rtk_restartCallBack_execute
 * Description:
 *      Execute hook function if this pointer is hook.
 * Input:
 *      N/A
 * Output:
 *      None
 * Return:
 *      N/A
 * Note:
 *      N/A
 */
extern
void rtk_restartCallBack_execute(void);

/* Function Name:
 *      rtk_watchdog_kick
 * Description:
 *      Execute kick watchdog function.
 * Input:
 *      N/A
 * Output:
 *      None
 * Return:
 *      N/A
 * Note:
 *      N/A
 */
extern
int rtk_watchdog_kick(void);


#endif /* __RTK_UTIL_H__ */

