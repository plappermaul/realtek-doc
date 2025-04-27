/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 79582 $
 * $Date: 2017-06-13 16:50:29 +0800 (Tue, 13 Jun 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Hardware Profile
 *
 */

/*
 * Include Files
 */

#include <dal/rtrpc/rtrpc_hwp.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

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


int32
hwp_unit_get_next(int32 unit, int32 *pNextUnit)
{
    rtdrv_hwp_unitCfg_t     hwpUnitCfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, (uint32*)&unit);

    osal_memset(&hwpUnitCfg, 0, sizeof(rtdrv_hwp_unitCfg_t));

    hwpUnitCfg.unit = unit;
    GETSOCKOPT(RTDRV_HWP_UNIT_GET_NEXT, &hwpUnitCfg, rtdrv_hwp_unitCfg_t, 1);
    *pNextUnit = hwpUnitCfg.nextUnit;
    return RT_ERR_OK;
}



