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
 * Purpose : Implementation of the watchdog driver
 *
 * Feature : Watchdog driver
 *
 */

/*
 * Include Files
 */
#include <drv/watchdog/watchdog.h>
#include <private/drv/watchdog/watchdog_mapper.h>
#include <common/debug/rt_log.h>
#include <hwp/hw_profile.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      drv_watchdog_init
 * Description:
 *      Init the watchdog module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_watchdog_init(uint32 unit)
{
    RT_INIT_MSG("  Watchdog init (unit %u)\n",unit);
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return WDG_CTRL(unit).init(unit);
} /* end of drv_watchdog_init */

/* Function Name:
 *      drv_watchdog_scale_set
 * Description:
 *      Set watchdog expired period
 * Input:
 *      unit  - unit id
 *      scale - period scale
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - succeed in setting watchdog expired period.
 *      RT_ERR_FAILED - fail to set watchdog expired period.
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_watchdog_scale_set(uint32 unit, drv_watchdog_scale_t scale)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return WDG_CTRL(unit).scale_set(unit, scale);
} /* end of drv_watchdog_scale_set */

/* Function Name:
 *      drv_watchdog_scale_get
 * Description:
 *      Get watchdog expired period scale
 * Input:
 *      unit   - unit id
 * Output:
 *      pScale - period scale
 * Return:
 *      RT_ERR_OK           - get watchdog expired period scale successfully.
 *      RT_ERR_FAILED       - fail to get get watchdog expired period scale.
 *      RT_ERR_NULL_POINTER - pScale is a null pointer.
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_watchdog_scale_get(uint32 unit, drv_watchdog_scale_t *pScale)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return WDG_CTRL(unit).scale_get(unit, pScale);
} /* end of drv_watchdog_scale_get */

/* Function Name:
 *      drv_watchdog_enable_set
 * Description:
 *      Set watchdog enable/disable
 * Input:
 *      unit   - unit id
 *      enable - enable or disable request
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - enable watchdog successfully.
 *      RT_ERR_FAILED - fail to enable watchdog.
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_watchdog_enable_set(uint32 unit, uint32 enable)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return WDG_CTRL(unit).enable_set(unit, enable);
} /* end of drv_watchdog_enable_set */

/* Function Name:
 *      drv_watchdog_enable_get
 * Description:
 *      Get watchdog enable/disable status
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - watchdog enable/disable status
 * Return:
 *      RT_ERR_OK           - get watchdog enable/disable status successfully.
 *      RT_ERR_NULL_POINTER - pEnable is a null pointer.
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_watchdog_enable_get(uint32 unit, uint32 *pEnable)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return WDG_CTRL(unit).enable_get(unit, pEnable);
} /* end of drv_watchdog_enable_get */

/* Function Name:
 *      drv_watchdog_kick
 * Description:
 *      Kick watchdog
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - kick watchdog successfully.
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_watchdog_kick(uint32 unit)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return WDG_CTRL(unit).kick(unit);
} /* end of drv_watchdog_kick */

/* Function Name:
 *      drv_watchdog_threshold_set
 * Description:
 *      Set watchdog threshold counter of the specified device
 * Input:
 *      unit       - unit id
 *      pThreshold - watchdog threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - successfully.
 *      RT_ERR_NULL_POINTER - pThreshold is a null pointer.
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_watchdog_threshold_set(uint32 unit, drv_watchdog_threshold_t *pThreshold)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return WDG_CTRL(unit).threshold_set(unit, pThreshold);
} /* end of drv_watchdog_threshold_set */

/* Function Name:
 *      drv_watchdog_threshold_get
 * Description:
 *      Get watchdog threshold counter of the specified device
 * Input:
 *      unit       - unit id
 * Output:
 *      pThreshold - watchdog threshold
 * Return:
 *      RT_ERR_OK - successfully.
 *      RT_ERR_NULL_POINTER - pThreshold is a null pointer.
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_watchdog_threshold_get(uint32 unit, drv_watchdog_threshold_t *pThreshold)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return WDG_CTRL(unit).threshold_get(unit, pThreshold);
} /* end of drv_watchdog_threshold_get */


