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
 * $Revision: 76016 $
 * $Date: 2017-02-24 11:41:35 +0800 (Fri, 24 Feb 2017) $
 *
 * Purpose : Definition those waFunc APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Port
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phy_rtl9310.h>
#include <dal/mango/dal_mango_waFunc.h>

/*Auto Recovery Debug Counter*/
extern uint32 macSerdes_watchdog_cnt[];

/*
 * Data Declaration
 */

/* Function Name:
 *      dal_mango_port_serdes_watchdog
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
int32
dal_mango_port_serdes_watchdog(uint32 unit)
{
    rt_serdesMode_t sdsMode;
    uint32  phyIdx, baseMacId, sds;
    int32   ret;
    uint8   linkChk;

    HWP_PHY_TRAVS(unit, phyIdx)
    {
        baseMacId = HWP_PHY_BASE_MACID_BY_IDX(unit, phyIdx);
        sds = HWP_PORT_SDSID(unit, baseMacId);

        ret = phy_rtl9310_sds_mode_get(unit, sds, &sdsMode);
        if (ret != RT_ERR_OK || sdsMode == RTK_MII_DISABLE)
            continue;

        ret = phy_rtl9310_linkSts_chk(unit, sds, &linkChk);
        if (ret != RT_ERR_OK)
            continue;

        if (0 == linkChk)
        {
            macSerdes_watchdog_cnt[unit]++;
            phy_rtl9310_sds_rst(unit, sds);
        }
    }

    return RT_ERR_OK;
}/* end of dal_mango_port_serdes_watchdog */


