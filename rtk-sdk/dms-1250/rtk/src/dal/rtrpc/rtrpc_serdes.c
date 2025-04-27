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
 *           1) SerDes
 *
 */

#include <dal/rtrpc/rtrpc_serdes.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>


int32
rtrpc_hal_serdes_reg_get(uint32 unit, uint32 sdsId, uint32 page, uint32 reg, uint32 *pData)
{
    rtdrv_serdes_reg_t      sdsReg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sdsReg, 0, sizeof(rtdrv_serdes_reg_t));
    sdsReg.unit = unit;
    sdsReg.sdsId = sdsId;
    sdsReg.page = page;
    sdsReg.reg = reg;

    GETSOCKOPT(RTDRV_SERDES_REG_GET, &sdsReg, rtdrv_serdes_reg_t, 1);
    *pData = sdsReg.data;
    return RT_ERR_OK;
}


int32
rtrpc_hal_serdes_reg_set(uint32 unit, uint32 sdsId, uint32 page, uint32 reg, uint32 data)
{
    rtdrv_serdes_reg_t      sdsReg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sdsReg, 0, sizeof(rtdrv_serdes_reg_t));
    sdsReg.unit = unit;
    sdsReg.sdsId = sdsId;
    sdsReg.page = page;
    sdsReg.reg = reg;
    sdsReg.data = data;

    SETSOCKOPT(RTDRV_SERDES_REG_SET, &sdsReg, rtdrv_serdes_reg_t, 1);
    return RT_ERR_OK;
}



