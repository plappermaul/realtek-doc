/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 79582 $
 * $Date: 2017-06-13 16:50:29 +0800 (Tue, 13 Jun 2017) $
 *
 * Purpose : Realtek Switch SDK Debug Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) SDK Debug Module for Linux User Mode
 *
 */

/*
 * Include Files
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <common/debug/rt_log.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <rtdrv/ext/rtdrv_netfilter_ext_8380.h>
#include <virtualmac/vmac_target.h>
#include <dal/rtrpc/rtrpc_ext_9300.h>
#include <dal/rtrpc/rtrpc_msg.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */

#if 0
/*
 * Function Declaration
 */
int32 vmac_getTarget(enum IC_TYPE *type)
{
    rtdrv_ext_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(unit_cfg));

    GETSOCKOPT(RTDRV_EXT_MODEL_TARGET_GET, &unit_cfg, rtdrv_ext_unitCfg_t, 1);
    *type = unit_cfg.data;

    return RT_ERR_OK;
}

int32 vmac_setTarget(enum IC_TYPE type)
{
    rtdrv_ext_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(unit_cfg));

    unit_cfg.data = type;
    SETSOCKOPT(RTDRV_EXT_MODEL_TARGET_SET, &unit_cfg, rtdrv_ext_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 vmac_getRegAccessType(enum REG_ACCESS_TYPE *type)
{
    rtdrv_ext_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(unit_cfg));

    GETSOCKOPT(RTDRV_EXT_MODEL_REG_ACCESS_GET, &unit_cfg, rtdrv_ext_unitCfg_t, 1);
    *type = unit_cfg.data;

    return RT_ERR_OK;
}

int32 vmac_setRegAccessType(enum REG_ACCESS_TYPE type)
{
    rtdrv_ext_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(unit_cfg));

    unit_cfg.data = type;
    SETSOCKOPT(RTDRV_EXT_MODEL_REG_ACCESS_SET, &unit_cfg, rtdrv_ext_unitCfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32
tc_exec(uint32 start, uint32 end)
{
    rtdrv_ext_modelCfg_t modelTest_cfg;
    uint32 unit = 0;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&modelTest_cfg, 0, sizeof(modelTest_cfg));

    modelTest_cfg.startID = start;
    modelTest_cfg.endID = end;
    SETSOCKOPT(RTDRV_EXT_MODEL_TEST_SET, &modelTest_cfg, rtdrv_ext_modelCfg_t, 1);

    return RT_ERR_OK;
}


