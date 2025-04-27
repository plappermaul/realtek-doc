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
 *           1) L2 notification
 *
 */

#include <dal/rtrpc/rtrpc_l2_ntfy.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>


int32
drv_l2ntfy_enable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    GETSOCKOPT(RTDRV_L2NTFY_ENABLE_GET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);
    *pEnable = l2ntfy_cfg.enable;

    return RT_ERR_OK;
}


int32
drv_l2ntfy_enable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    l2ntfy_cfg.enable = enable;
    SETSOCKOPT(RTDRV_L2NTFY_ENABLE_SET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_backPressureThresh_get(uint32 unit, uint32 *pThresh)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    GETSOCKOPT(RTDRV_L2NTFY_BACK_PRESSURE_THR_GET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);
    *pThresh = l2ntfy_cfg.thresh;

    return RT_ERR_OK;
}

int32
drv_l2ntfy_backPressureThresh_set(uint32 unit, uint32 thresh)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    l2ntfy_cfg.thresh = thresh;
    SETSOCKOPT(RTDRV_L2NTFY_BACK_PRESSURE_THR_SET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_notificationEventEnable_get(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t *pEnable)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    l2ntfy_cfg.event= event;
    GETSOCKOPT(RTDRV_L2NTFY_EVENT_ENABLE_GET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);
    *pEnable = l2ntfy_cfg.enable;

    return RT_ERR_OK;
}

int32
drv_l2ntfy_notificationEventEnable_set(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t enable)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    l2ntfy_cfg.event= event;
    l2ntfy_cfg.enable = enable;
    SETSOCKOPT(RTDRV_L2NTFY_EVENT_ENABLE_SET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_counter_dump(uint32 unit)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    SETSOCKOPT(RTDRV_L2NTFY_COUNTER_DUMP, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_counter_clear(uint32 unit)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    SETSOCKOPT(RTDRV_L2NTFY_COUNTER_CLEAR, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_debug_get(uint32 unit, uint32 *pFlags)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    GETSOCKOPT(RTDRV_L2NTFY_DBG_GET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);
    *pFlags = l2ntfy_cfg.dbgFlag;

    return RT_ERR_OK;
}

int32
drv_l2ntfy_debug_set(uint32 unit, uint32 flags)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    l2ntfy_cfg.dbgFlag = flags;
    SETSOCKOPT(RTDRV_L2NTFY_DBG_SET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_event_dump(uint32 unit)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    SETSOCKOPT(RTDRV_L2NTFY_EVENT_DUMP, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_dst_get(uint32 unit, rtk_l2ntfy_dst_t *pDst)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    GETSOCKOPT(RTDRV_L2NTFY_DST_GET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);
    *pDst = l2ntfy_cfg.dst;

    return RT_ERR_OK;
}

int32
drv_l2ntfy_dst_set(uint32 unit, rtk_l2ntfy_dst_t dst)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    l2ntfy_cfg.dst  = dst;
    SETSOCKOPT(RTDRV_L2NTFY_DST_SET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_reset(uint32 unit)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    SETSOCKOPT(RTDRV_L2NTFY_RESET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_magicNum_get(uint32 unit, uint32 *pMagicNum)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    GETSOCKOPT(RTDRV_L2NTFY_MAGIC_NUM_GET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);
    *pMagicNum = l2ntfy_cfg.magicNum;

    return RT_ERR_OK;
}

int32
drv_l2ntfy_magicNum_set(uint32 unit, uint32 magicNum)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    l2ntfy_cfg.magicNum = magicNum;
    SETSOCKOPT(RTDRV_L2NTFY_MAGIC_NUM_SET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_macAddr_get(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit     = unit;
    l2ntfy_cfg.addrType = type;
    GETSOCKOPT(RTDRV_L2NTFY_MACADDR_GET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);
    osal_memcpy(pMac, &l2ntfy_cfg.mac, sizeof(rtk_mac_t));

    return RT_ERR_OK;
}

int32
drv_l2ntfy_macAddr_set(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit     = unit;
    l2ntfy_cfg.addrType = type;
    osal_memcpy(&l2ntfy_cfg.mac, pMac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L2NTFY_MACADDR_SET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_maxEvent_get(uint32 unit, uint32 *pNum)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    GETSOCKOPT(RTDRV_L2NTFY_MAXEVENT_GET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);
    *pNum = l2ntfy_cfg.maxEvent;

    return RT_ERR_OK;
}

int32
drv_l2ntfy_maxEvent_set(uint32 unit, uint32 num)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit     = unit;
    l2ntfy_cfg.maxEvent = num;
    SETSOCKOPT(RTDRV_L2NTFY_MAXEVENT_SET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

int32
drv_l2ntfy_timeout_get(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 *pTimeout)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit = unit;
    l2ntfy_cfg.mode = mode;
    GETSOCKOPT(RTDRV_L2NTFY_TIMEOUT_GET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);
    *pTimeout = l2ntfy_cfg.timeout;

    return RT_ERR_OK;
}

int32
drv_l2ntfy_timeout_set(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 timeout)
{
    rtdrv_l2ntfyCfg_t l2ntfy_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&l2ntfy_cfg, 0, sizeof(rtdrv_l2ntfyCfg_t));
    l2ntfy_cfg.unit     = unit;
    l2ntfy_cfg.mode     = mode;
    l2ntfy_cfg.timeout  = timeout;
    SETSOCKOPT(RTDRV_L2NTFY_TIMEOUT_SET, &l2ntfy_cfg, rtdrv_l2ntfyCfg_t, 1);

    return RT_ERR_OK;
}

