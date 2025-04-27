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
 *           1) NIC
 *
 */

#include <dal/rtrpc/rtrpc_nic.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 drv_nic_rx_start(uint32 unit)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    SETSOCKOPT(RTDRV_NIC_RX_START, &nic_cfg, rtdrv_nicCfg_t, 1);

    return RT_ERR_OK;
}

int32 drv_nic_rx_stop(uint32 unit)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    SETSOCKOPT(RTDRV_NIC_RX_STOP, &nic_cfg, rtdrv_nicCfg_t, 1);

    return RT_ERR_OK;
}


