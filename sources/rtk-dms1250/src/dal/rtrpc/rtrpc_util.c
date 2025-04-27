/*
 * Copyright (C) 2009-2019 Realtek Semiconductor Corp.
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
 * $Revision: $
 * $Date: $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file constitute all utility functions.
 */

#include <common/rt_type.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <dal/rtrpc/rtrpc_util.h>
#include <dal/rtrpc/rtrpc_msg.h>

int32
rtrpc_util_tblEntry2Field(uint32 unit,uint32 table,uint32 field,uint32 * pField,uint32 * pEntry)
{
    rtdrv_tblCfg_t tbl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&tbl_cfg, 0, sizeof(rtdrv_tblCfg_t));
    tbl_cfg.unit = unit;
    tbl_cfg.table = table;
    tbl_cfg.field = field;
    osal_memcpy(tbl_cfg.value, pEntry, sizeof(tbl_cfg.value));

    GETSOCKOPT(RTDRV_UTIL_TBL_FIELD2ENTRY, &tbl_cfg, rtdrv_tblCfg_t, 1);
    osal_memcpy(pField, tbl_cfg.fieldValue, RTK_DIAG_TBL_FIELD_WORDS_MAX * (sizeof(uint32)));

    return RT_ERR_OK;
}


int32
rtrpc_util_field2TblEntry(uint32 unit,uint32 table,uint32 field,uint32 * pField,uint32 * pEntry)

{
    rtdrv_tblCfg_t tbl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&tbl_cfg, 0, sizeof(rtdrv_tblCfg_t));
    tbl_cfg.unit = unit;
    tbl_cfg.table = table;
    tbl_cfg.field = field;
    osal_memcpy(tbl_cfg.fieldValue, pField, sizeof(tbl_cfg.fieldValue));
    osal_memcpy(tbl_cfg.value, pEntry, sizeof(tbl_cfg.value));
    GETSOCKOPT(RTDRV_UTIL_TBL_ENTRY2FIELD, &tbl_cfg, rtdrv_tblCfg_t, 1);
    osal_memcpy(pEntry, tbl_cfg.value, RTK_DIAG_TBL_DATAREG_NUM_MAX * (sizeof(uint32)));
    return RT_ERR_OK;
}

int32
rtrpc_util_serdesTxEyeParam_scan(uint32 unit,
                                        rt_sdsTxScanParam_t *txScanParam,
                                        rt_sdsTxScanChart_t *scanResult)
{
    rtdrv_sdsTxScan_t txScan;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&txScan, 0, sizeof(rtdrv_sdsTxScan_t));
    txScan.unit = unit;
    osal_memcpy(&txScan.txScanParam, txScanParam, sizeof(rt_sdsTxScanParam_t));
    GETSOCKOPT(RTDRV_UTIL_SDS_TXSCAN_CHART_GET, &txScan, rtdrv_sdsTxScan_t, 1);
    osal_memcpy(scanResult, &txScan.scanResult, sizeof(rt_sdsTxScanChart_t));
    return RT_ERR_OK;
}



