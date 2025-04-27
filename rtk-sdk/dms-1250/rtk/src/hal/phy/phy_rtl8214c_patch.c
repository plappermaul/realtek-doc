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
 * $Revision: 74248 $
 * $Date: 2016-12-13 12:35:59 +0800 (Tue, 13 Dec 2016) $
 *
 * Purpose : PHY 8218B/8218FB/8214FC Driver APIs.
 *
 * Feature : PHY 8218B/8218FB/8214FC Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8214c.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/phy_construct.h>

/*
 * Symbol Definition
 */


/* Function Name:
 *      sub_phy_8214c_patch_set
 * Description:
 *      Patch the PHY:8214C MP.
 * Input:
 *      unit   - unit id
 *      port   - PHY base port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
sub_phy_8214c_patch_set(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32   ret = RT_ERR_FAILED;
    int32   i;

    RT_PARAM_CHK(((port % PORT_NUM_IN_8214C) != 0), RT_ERR_PORT_ID);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    rtl8214c_config(unit, port);

    for (i = 0; i < PORT_NUM_IN_8214C; ++i)
        pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_init(unit, port + i);

    ret = RT_ERR_OK;
    return ret;

} /* end of sub_phy_8214c_patch_set */


