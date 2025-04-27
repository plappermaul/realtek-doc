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
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) sdk test
 *
 */

#include <dal/rtrpc/rtrpc_private.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 sdktest_run(uint32 unit, uint8 *item)
{
    rtdrv_sdkCfg_t sdk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    sdk_cfg.unit = unit;
    osal_memset(sdk_cfg.item, 0, SDK_CFG_ITEM+1);
    strncpy((char *)sdk_cfg.item, (char *)item, SDK_CFG_ITEM);

    SETSOCKOPT(RTDRV_SDK_TEST, &sdk_cfg, rtdrv_sdkCfg_t, 1);

    return RT_ERR_OK;
} /* sdktest_run */

int32 sdktest_run_id(uint32 unit, uint32 start, uint32 end)
{
    rtdrv_sdkCfg_t sdk_cfg;
    uint32 master_view_unit=unit;
    osal_memset(&sdk_cfg, 0, sizeof(rtdrv_sdkCfg_t));
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    sdk_cfg.unit = unit;
    sdk_cfg.start = start;
    sdk_cfg.end = end;

    SETSOCKOPT(RTDRV_SDK_TEST_ID, &sdk_cfg, rtdrv_sdkCfg_t, 1);

    return RT_ERR_OK;
} /* sdktest_run_id */

int32 sdktest_mode_get(int32 *pMode)
{
    rtdrv_sdkCfg_t sdk_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit=unit;
    osal_memset(&sdk_cfg, 0, sizeof(rtdrv_sdkCfg_t));
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    GETSOCKOPT(RTDRV_SDK_TEST_MODE_GET, &sdk_cfg, rtdrv_sdkCfg_t, 1);
    *pMode = sdk_cfg.mode;
    return RT_ERR_OK;
} /* sdktest_mode_get */

int32 sdktest_mode_set(int32 mode)
{
    rtdrv_sdkCfg_t sdk_cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit=unit;
    osal_memset(&sdk_cfg, 0, sizeof(rtdrv_sdkCfg_t));
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    sdk_cfg.mode = mode;
    SETSOCKOPT(RTDRV_SDK_TEST_MODE_SET, &sdk_cfg, rtdrv_sdkCfg_t, 1);
    return RT_ERR_OK;
} /* sdktest_mode_set */

