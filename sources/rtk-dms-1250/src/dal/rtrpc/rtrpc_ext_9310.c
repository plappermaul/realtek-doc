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
 * $Revision: 82906 $
 * $Date: 2017-10-24 18:05:05 +0800 (Tue, 24 Oct 2017) $
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
#include <rtdrv/ext/rtdrv_netfilter_ext_9310.h>
//#include <virtualmac/vmac_target.h>
#include <dal/rtrpc/rtrpc_ext_9310.h>
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

int32
fpga_info_get(uint32 unit, uint32 *pRtl, uint32 *pDate, uint32 *pTime, uint32 *pVersion)
{
    rtdrv_ext_fpgaCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.unit = unit;
    GETSOCKOPT(RTDRV_EXT_FPGA_INFO_GET, &cfg, rtdrv_ext_fpgaCfg_t, 1);
    *pRtl = cfg.rtl_svn_rev;
    *pDate = cfg.build_date;
    *pTime = cfg.build_time;
    *pVersion = cfg.fpga_type_and_reg_profile_ver;

    return RT_ERR_OK;
}

int32
fpga_init(uint32 unit, uint32 fpgaVer)
{
    rtdrv_ext_fpgaCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.unit = unit;
    cfg.fpgaVer = fpgaVer;
    SETSOCKOPT(RTDRV_EXT_FPGA_INIT_SET, &cfg, rtdrv_ext_fpgaCfg_t, 1);

    return RT_ERR_OK;
}

int32
fpga_test(uint32 unit)
{
    rtdrv_ext_fpgaCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.unit = unit;
    SETSOCKOPT(RTDRV_EXT_FPGA_TEST, &cfg, rtdrv_ext_fpgaCfg_t, 1);

    return RT_ERR_OK;
}

int32
testIo_set_ocp(uint32 unit, uint32 addr, uint32 val)
{
    rtdrv_ext_union_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.diag_cfg.unit       = unit;
    cfg.diag_cfg.reg        = addr;
    cfg.diag_cfg.data[0]    = val;
    SETSOCKOPT(RTDRV_EXT_TESTIO_SET_OCP, &cfg, rtdrv_ext_union_t, 1);

    return RT_ERR_OK;
}

int32
testIo_get_ocp(uint32 unit, uint32 addr, uint32 *pVal)
{
    rtdrv_ext_union_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.diag_cfg.unit   = unit;
    cfg.diag_cfg.reg    = addr;
    GETSOCKOPT(RTDRV_EXT_TESTIO_GET_OCP, &cfg, rtdrv_ext_union_t, 1);
    *pVal = cfg.diag_cfg.data[0];

    return RT_ERR_OK;
}

int32
testIo_set_ePhy(uint32 unit, uint32 addr, uint32 val)
{
    rtdrv_ext_union_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.diag_cfg.unit       = unit;
    cfg.diag_cfg.reg        = addr;
    cfg.diag_cfg.data[0]    = val;
    SETSOCKOPT(RTDRV_EXT_TESTIO_SET_EPHY, &cfg, rtdrv_ext_union_t, 1);

    return RT_ERR_OK;
}

int32
testIo_get_ePhy(uint32 unit, uint32 addr, uint32 *pVal)
{
    rtdrv_ext_union_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.diag_cfg.unit   = unit;
    cfg.diag_cfg.reg    = addr;
    GETSOCKOPT(RTDRV_EXT_TESTIO_GET_EPHY, &cfg, rtdrv_ext_union_t, 1);
    *pVal = cfg.diag_cfg.data[0];

    return RT_ERR_OK;
}

int32
testIo_set_eFuse_check(uint32 unit)
{
    rtdrv_ext_union_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.diag_cfg.unit       = unit;
    SETSOCKOPT(RTDRV_EXT_TESTIO_SET_EFUSE_CHECK, &cfg, rtdrv_ext_union_t, 1);

    return RT_ERR_OK;
}

int32
testIo_set_eFuse(uint32 unit, uint32 index, uint32 addr, uint32 val)
{
    rtdrv_ext_union_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.diag_cfg.unit       = unit;
    cfg.diag_cfg.idx1       = index;
    cfg.diag_cfg.reg        = addr;
    cfg.diag_cfg.data[0]    = val;
    SETSOCKOPT(RTDRV_EXT_TESTIO_SET_EFUSE, &cfg, rtdrv_ext_union_t, 1);

    return RT_ERR_OK;
}

int32
testIo_get_eFuse(uint32 unit, uint32 index, uint32 *pAddr, uint32 *pVal)
{
    rtdrv_ext_union_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(cfg));

    cfg.diag_cfg.unit   = unit;
    cfg.diag_cfg.idx1   = index;
    GETSOCKOPT(RTDRV_EXT_TESTIO_GET_EFUSE, &cfg, rtdrv_ext_union_t, 1);
    *pAddr = cfg.diag_cfg.reg;
    *pVal = cfg.diag_cfg.data[0];

    return RT_ERR_OK;
}

