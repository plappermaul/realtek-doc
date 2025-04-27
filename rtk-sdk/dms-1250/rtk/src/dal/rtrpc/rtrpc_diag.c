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
 * $Revision: 82537 $
 * $Date: 2017-09-29 13:54:04 +0800 (Fri, 29 Sep 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file constitute all functions which is not call RTK layer APIs
 *           1) register APIs
 *           2) nic debug APIs
 *           3) external GPIO APIs
 *           4) rtl8231 APIs
 *           5) internal GPIO APIs
 *           6) I2C APIs
 *
 */

#include <common/rt_type.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <dal/rtrpc/rtrpc_diag.h>
#include <dal/rtrpc/rtrpc_msg.h>


/* Switch Register APIs */
int32 rtrpc_ioal_mem32_read(uint32 unit, uint32 reg, uint32 *pValue)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = reg;

    GETSOCKOPT(RTDRV_REG_REGISTER_GET, &reg_cfg, rtdrv_regCfg_t, 1);
    *pValue = reg_cfg.value;

    return RT_ERR_OK;
}

int32 rtrpc_ioal_mem32_write(uint32 unit, uint32 reg, uint32 value)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = reg;
    reg_cfg.value = value;
    SETSOCKOPT(RTDRV_REG_REGISTER_SET, &reg_cfg, rtdrv_regCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_reg_idx2Addr_get(uint32 unit, uint32 regIdx, uint32 *pAddr)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = regIdx;

    GETSOCKOPT(RTDRV_REG_IDX2ADDR_GET, &reg_cfg, rtdrv_regCfg_t, 1);
    *pAddr = reg_cfg.value;

    return RT_ERR_OK;
}

int32 rtrpc_reg_idxMax_get(uint32 unit, uint32 *pMax)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;

    GETSOCKOPT(RTDRV_REG_IDXMAX_GET, &reg_cfg, rtdrv_regCfg_t, 1);
    *pMax = reg_cfg.value;

    return RT_ERR_OK;
}


int32 rtrpc_debug_mem_read(uint32 unit, uint32 addr, uint32 *pValue)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = addr;

    GETSOCKOPT(RTDRV_DEBUG_MEM_READ, &reg_cfg, rtdrv_regCfg_t, 1);
    *pValue = reg_cfg.value;

    return RT_ERR_OK;
}

int32 rtrpc_debug_mem_write(uint32 unit, uint32 addr, uint32 value)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = addr;
    reg_cfg.value = value;
    SETSOCKOPT(RTDRV_DEBUG_MEM_WRITE, &reg_cfg, rtdrv_regCfg_t, 1);

    return RT_ERR_OK;
}

// REG get
int32 rtrpc_diag_reg_get(uint32 unit,uint32 reg, uint32 * pValue)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = reg;

    GETSOCKOPT(RTDRV_REG_GET, &reg_cfg, rtdrv_regCfg_t, 1);
    osal_memcpy(pValue, reg_cfg.buf, RTK_DIAG_REG_WORD_NUM_MAX);

    return RT_ERR_OK;
}

int32 rtrpc_diag_regField_get(uint32 unit,uint32 reg,uint32 field,uint32 * pValue)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = reg;
    reg_cfg.field = field;

    GETSOCKOPT(RTDRV_REG_FIELD_GET, &reg_cfg, rtdrv_regCfg_t, 1);
    *pValue =  reg_cfg.buf[0];
    return RT_ERR_OK;
}

int32 rtrpc_diag_regArray_get(uint32 unit,uint32 reg,int32 index_1,int32 index_2, uint32 * pValue)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = reg;
    reg_cfg.idx1 = index_1;
    reg_cfg.idx2 = index_2;

    GETSOCKOPT(RTDRV_REG_ARRAY_GET, &reg_cfg, rtdrv_regCfg_t, 1);
    osal_memcpy(pValue, reg_cfg.buf, RTK_DIAG_REG_WORD_NUM_MAX);
    return RT_ERR_OK;
}

int32 rtrpc_diag_regArrayField_get(uint32 unit,uint32 reg,int32 index_1,int32 index_2,uint32 field,uint32 * pValue)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = reg;
    reg_cfg.idx1 = index_1;
    reg_cfg.idx2 = index_2;
    reg_cfg.field = field;

    GETSOCKOPT(RTDRV_REG_ARRAY_FIELD_GET, &reg_cfg, rtdrv_regCfg_t, 1);
    *pValue =  reg_cfg.buf[0];
    return RT_ERR_OK;
}

// REG set
int32 rtrpc_diag_reg_set(uint32 unit,uint32 reg, uint32 * pValue)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = reg;
    osal_memcpy(reg_cfg.buf, pValue, RTK_DIAG_REG_WORD_NUM_MAX);

    SETSOCKOPT(RTDRV_REG_SET, &reg_cfg, rtdrv_regCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_diag_regField_set(uint32 unit,uint32 reg,uint32 field,uint32 * pValue)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = reg;
    reg_cfg.field = field;
    reg_cfg.buf[0] = *pValue;

    SETSOCKOPT(RTDRV_REG_FIELD_SET, &reg_cfg, rtdrv_regCfg_t, 1);

    return RT_ERR_OK;
}


int32 rtrpc_diag_regArray_set(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 *pValue)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = reg;
    reg_cfg.idx1 = index_1;
    reg_cfg.idx2 = index_2;
    osal_memcpy(reg_cfg.buf, pValue, RTK_DIAG_REG_WORD_NUM_MAX);

    SETSOCKOPT(RTDRV_REG_ARRAY_SET, &reg_cfg, rtdrv_regCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_diag_regArrayField_set(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 field, uint32 *pValue)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = reg;
    reg_cfg.idx1 = index_1;
    reg_cfg.idx2 = index_2;
    reg_cfg.field = field;
    reg_cfg.buf[0] = *pValue;

    SETSOCKOPT(RTDRV_REG_ARRAY_FIELD_SET, &reg_cfg, rtdrv_regCfg_t, 1);

    return RT_ERR_OK;
}



#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
int32
rtrpc_diag_regInfoByStr_get(uint32 unit, char *reg_name, rtk_diag_regInfo_t *pReg_info)
{
    rtdrv_regInfo_t reg_info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if(osal_strlen(reg_name) >= sizeof(reg_info.name))
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    osal_memset(&reg_info, 0, sizeof(rtdrv_regInfo_t));
    reg_info.unit = unit;
    osal_memcpy(reg_info.name, reg_name, osal_strlen(reg_name) + 1);

    GETSOCKOPT(RTDRV_REG_INFO_BYSTR_GET, &reg_info, rtdrv_regInfo_t, 1);
    *pReg_info = reg_info.data;

    return RT_ERR_OK;
}

int32
rtrpc_diag_regFieldInfo_get(uint32 unit, uint32 reg, uint32 field, rtk_diag_regFieldInfo_t *pField_info)
{
    rtdrv_regFieldInfo_t field_info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&field_info, 0, sizeof(rtdrv_regFieldInfo_t));
    field_info.unit = unit;
    field_info.reg = reg;
    field_info.field = field;

    GETSOCKOPT(RTDRV_REG_FIELD_INFO_GET, &field_info, rtdrv_regFieldInfo_t, 1);
    *pField_info = field_info.data;
    return RT_ERR_OK;
}

int32
rtrpc_diag_regInfoByStr_match(uint32 unit, char *keyword, uint32 reg, rtk_diag_regInfo_t *pReg_info)
{
    rtdrv_regInfo_t reg_info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if(osal_strlen(keyword) >= sizeof(reg_info.name))
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    osal_memset(&reg_info, 0, sizeof(rtdrv_regInfo_t));
    reg_info.unit = unit;
    reg_info.reg = reg;
    osal_memcpy(reg_info.name, keyword, osal_strlen(keyword) + 1);

    GETSOCKOPT(RTDRV_REG_INFO_BYSTR_MATCH, &reg_info, rtdrv_regInfo_t, 1);
    *pReg_info = reg_info.data;
    return RT_ERR_OK;
}
#endif //CONFIG_SDK_DUMP_REG_WITH_NAME


int32
rtrpc_diag_tableEntry_get(uint32 unit,uint32 table,uint32 entry,uint32 * pData)
{
    rtdrv_tblCfg_t tbl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&tbl_cfg, 0, sizeof(rtdrv_tblCfg_t));
    tbl_cfg.unit = unit;
    tbl_cfg.table = table;
    tbl_cfg.addr = entry;

    GETSOCKOPT(RTDRV_TABLE_ENTRY_GET, &tbl_cfg, rtdrv_tblCfg_t, 1);
    osal_memcpy(pData, tbl_cfg.value, RTK_DIAG_TBL_DATAREG_NUM_MAX * (sizeof(uint32)));

    return RT_ERR_OK;
}


#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)
int32
rtrpc_diag_tableInfoByStr_get(
    uint32 unit,
    char * tbl_name,
    rtk_diag_tblInfo_t * pTbl_info)
{
    rtdrv_tblInfo_t tbl_info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if(osal_strlen(tbl_name) >= sizeof(tbl_info.name))
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    osal_memset(&tbl_info, 0, sizeof(rtdrv_tblInfo_t));
    tbl_info.unit = unit;
    osal_memcpy(tbl_info.name, tbl_name, osal_strlen(tbl_name) + 1);

    GETSOCKOPT(RTDRV_TABLE_INFO_BYSTR_GET, &tbl_info, rtdrv_tblInfo_t, 1);
    *pTbl_info = tbl_info.data;

    return RT_ERR_OK;
}

int32
rtrpc_diag_tableInfoByStr_match(
    uint32 unit,
    char *keyword,
    uint32 tbl,
    rtk_diag_tblInfo_t *pTbl_info)
{
    rtdrv_tblInfo_t tbl_info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if(osal_strlen(keyword) >= sizeof(tbl_info.name))
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    osal_memset(&tbl_info, 0, sizeof(rtdrv_tblInfo_t));
    tbl_info.unit = unit;
    tbl_info.tbl = tbl;
    osal_memcpy(tbl_info.name, keyword, osal_strlen(keyword) + 1);

    GETSOCKOPT(RTDRV_TABLE_INFO_BYSTR_MATCH, &tbl_info, rtdrv_tblInfo_t, 1);
    *pTbl_info = tbl_info.data;
    return RT_ERR_OK;
}


int32
rtrpc_diag_tableFieldInfo_get(
    uint32 unit,
    uint32 tbl,
    uint32 field,
    rtk_diag_tblFieldInfo_t *pField_info)
{
    rtdrv_tblFieldInfo_t field_info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&field_info, 0, sizeof(rtdrv_tblFieldInfo_t));
    field_info.unit = unit;
    field_info.tbl = tbl;
    field_info.field = field;

    GETSOCKOPT(RTDRV_TABLE_FIELD_INFO_GET, &field_info, rtdrv_tblFieldInfo_t, 1);
    *pField_info = field_info.data;
    return RT_ERR_OK;
}
#endif //CONFIG_SDK_DUMP_TABLE_WITH_NAME


int32
rtrpc_diag_tableEntry_set(uint32 unit,uint32 table,uint32 addr,uint32 * pData)
{
    rtdrv_tblCfg_t tbl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&tbl_cfg, 0, sizeof(rtdrv_tblCfg_t));
    tbl_cfg.unit = unit;
    tbl_cfg.table = table;
    tbl_cfg.addr = addr;
    osal_memcpy(tbl_cfg.value, pData, sizeof(tbl_cfg.value));
    SETSOCKOPT(RTDRV_TABLE_ENTRY_SET, &tbl_cfg, rtdrv_tblCfg_t, 1);

    return RT_ERR_OK;
}

/* NIC Debug APIs */
int32 rtrpc_nic_dbg_get(uint32 unit, uint32 *pFlags)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    GETSOCKOPT(RTDRV_NIC_DEBUG_GET, &nic_cfg, rtdrv_nicCfg_t, 1);
    *pFlags = nic_cfg.flags;

    return RT_ERR_OK;
}

int32 rtrpc_nic_dbg_set(uint32 unit, uint32 flags)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    nic_cfg.flags = flags;
    SETSOCKOPT(RTDRV_NIC_DEBUG_SET, &nic_cfg, rtdrv_nicCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_nic_cntr_dump(uint32 unit)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    SETSOCKOPT(RTDRV_NIC_COUNTER_DUMP, &nic_cfg, rtdrv_nicCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_nic_cntr_clear(uint32 unit)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    SETSOCKOPT(RTDRV_NIC_COUNTER_CLEAR, &nic_cfg, rtdrv_nicCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_nic_ringbuf_dump(uint32 unit, nic_dir_t direct)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    nic_cfg.mode = direct;
    SETSOCKOPT(RTDRV_NIC_BUFFER_DUMP, &nic_cfg, rtdrv_nicCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_nic_pktHdrMBuf_dump(uint32 unit, uint32 mode, uint32 start, uint32 end, uint32 flags)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    nic_cfg.mode = mode;
    nic_cfg.start = start;
    nic_cfg.end = end;
    nic_cfg.flags = flags;
    SETSOCKOPT(RTDRV_NIC_PKTHDR_MBUF_DUMP, &nic_cfg, rtdrv_nicCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_nic_rx_status_get(uint32 unit, uint32 *pStatus)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    GETSOCKOPT(RTDRV_NIC_RX_STATUS_GET, &nic_cfg, rtdrv_nicCfg_t, 1);
    *pStatus = nic_cfg.rx_status;

    return RT_ERR_OK;
}

int32
rtrpc_nic_tag_set(uint32 unit, nic_txTagStatus_t tagStatus, uint8 *pTxTag,  rtk_portmask_t *pPortmask)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    nic_cfg.tagStatus = tagStatus;
    osal_memcpy(&nic_cfg.txTag, pTxTag, sizeof(nic_cfg.txTag));
    osal_memcpy(&nic_cfg.portmask, pPortmask, sizeof(nic_cfg.portmask));

    SETSOCKOPT(RTDRV_NIC_TAG_SET, &nic_cfg, rtdrv_nicCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_nic_txData_set(uint32 unit, uint8 isAuto, uint8 *pTxData, uint32 len)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32  bufSize;
    uint32  master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);


    bufSize = (len <= sizeof(nic_cfg.txData)) ? len : sizeof(nic_cfg.txData);
    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    osal_memcpy(&nic_cfg.txData, pTxData, bufSize);
    nic_cfg.isAuto = isAuto;
    nic_cfg.len = bufSize;

    SETSOCKOPT(RTDRV_NIC_TXDATA_SET, &nic_cfg, rtdrv_nicCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_nic_diagPkt_send(uint32 unit, uint32 num)
{
    rtdrv_nicCfg_t nic_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&nic_cfg, 0, sizeof(rtdrv_nicCfg_t));
    nic_cfg.unit = unit;
    nic_cfg.num  = num;

    SETSOCKOPT(RTDRV_NIC_DIAGPKT_SEND, &nic_cfg, rtdrv_nicCfg_t, 1);

    return RT_ERR_OK;
}


int32
rtrpc_table_write(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtdrv_tblCfg_t tbl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&tbl_cfg, 0, sizeof(rtdrv_tblCfg_t));
    tbl_cfg.unit = unit;
    tbl_cfg.table = table;
    tbl_cfg.addr = addr;
    osal_memcpy(tbl_cfg.value, pData, 20*sizeof(uint32));
    SETSOCKOPT(RTDRV_TABLE_WRITE, &tbl_cfg, rtdrv_tblCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_table_read(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtdrv_tblCfg_t tbl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&tbl_cfg, 0, sizeof(rtdrv_tblCfg_t));
    tbl_cfg.unit = unit;
    tbl_cfg.table = table;
    tbl_cfg.addr = addr;

    GETSOCKOPT(RTDRV_TABLE_READ, &tbl_cfg, rtdrv_tblCfg_t, 1);

    osal_memcpy(pData, tbl_cfg.value, 20*sizeof(uint32));

    return RT_ERR_OK;
}

int32 rtrpc_reg_info_get(uint32 unit, uint32 regIdx, rtk_reg_info_t *pData)
{
    rtdrv_regCfg_t reg_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&reg_cfg, 0, sizeof(rtdrv_regCfg_t));
    reg_cfg.unit = unit;
    reg_cfg.reg = regIdx;

    GETSOCKOPT(RTDRV_REG_INFO_GET, &reg_cfg, rtdrv_regCfg_t, 1);
    osal_memcpy(pData, &reg_cfg.data, sizeof(rtk_reg_info_t));

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_RTL8231)
int32
rtrpc_rtl8231_i2c_read(uint32 unit, uint32 slave_addr, uint32 reg_addr, uint32 *pData)
{
    rtdrv_rtl8231Cfg_t rtl8231_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rtl8231_cfg, 0, sizeof(rtdrv_rtl8231Cfg_t));
    rtl8231_cfg.unit = unit;
    rtl8231_cfg.phyId_or_slaveAddr = slave_addr;
    rtl8231_cfg.reg_addr = reg_addr;
    GETSOCKOPT(RTDRV_RTL8231_I2C_READ, &rtl8231_cfg, rtdrv_rtl8231Cfg_t, 1);
    *pData = rtl8231_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_rtl8231_i2c_write(uint32 unit, uint32 slave_addr, uint32 reg_addr, uint32 data)
{
    rtdrv_rtl8231Cfg_t rtl8231_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rtl8231_cfg, 0, sizeof(rtdrv_rtl8231Cfg_t));
    rtl8231_cfg.unit = unit;
    rtl8231_cfg.phyId_or_slaveAddr = slave_addr;
    rtl8231_cfg.reg_addr = reg_addr;
    rtl8231_cfg.data = data;
    SETSOCKOPT(RTDRV_RTL8231_I2C_WRITE, &rtl8231_cfg, rtdrv_rtl8231Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_rtl8231_mdc_read(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 *pData)
{
    rtdrv_rtl8231Cfg_t rtl8231_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rtl8231_cfg, 0, sizeof(rtdrv_rtl8231Cfg_t));
    rtl8231_cfg.unit = unit;
    rtl8231_cfg.phyId_or_slaveAddr = phy_id;
    rtl8231_cfg.page = page;
    rtl8231_cfg.reg_addr = reg_addr;
    GETSOCKOPT(RTDRV_RTL8231_MDC_READ, &rtl8231_cfg, rtdrv_rtl8231Cfg_t, 1);
    *pData = rtl8231_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_rtl8231_mdc_write(uint32 unit, uint32 phy_id, uint32 page, uint32 reg_addr, uint32 data)
{
    rtdrv_rtl8231Cfg_t rtl8231_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&rtl8231_cfg, 0, sizeof(rtdrv_rtl8231Cfg_t));
    rtl8231_cfg.unit = unit;
    rtl8231_cfg.phyId_or_slaveAddr = phy_id;
    rtl8231_cfg.page = page;
    rtl8231_cfg.reg_addr = reg_addr;
    rtl8231_cfg.data = data;
    SETSOCKOPT(RTDRV_RTL8231_MDC_WRITE, &rtl8231_cfg, rtdrv_rtl8231Cfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_devReady_get(uint32 unit, uint32 dev, uint32 *pIsReady)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    GETSOCKOPT(RTDRV_EXTGPIO_DEV_READY_GET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);
    *pIsReady = extGpio_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_dev_get(uint32 unit, uint32 dev, drv_extGpio_devConf_t *pData)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    GETSOCKOPT(RTDRV_EXTGPIO_DEV_GET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);
    *pData = extGpio_cfg.extGpio_devConfData;

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_dev_init(uint32 unit, uint32 dev, drv_extGpio_devConf_t *pData)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.extGpio_devConfData = *pData;
    SETSOCKOPT(RTDRV_EXTGPIO_DEV_INIT, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_devEnable_get(uint32 unit, uint32 dev, rtk_enable_t *pEnable)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    GETSOCKOPT(RTDRV_EXTGPIO_DEV_ENABLE_GET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);
    *pEnable = extGpio_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_devEnable_set(uint32 unit, uint32 dev, rtk_enable_t enable)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.data = enable;
    SETSOCKOPT(RTDRV_EXTGPIO_DEV_ENABLE_SET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_syncEnable_get(uint32 unit, uint32 dev, rtk_enable_t *pEnable)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    GETSOCKOPT(RTDRV_EXTGPIO_SYNC_ENABLE_GET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);
    *pEnable = extGpio_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_syncEnable_set(uint32 unit, uint32 dev, rtk_enable_t enable)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.data = enable;
    SETSOCKOPT(RTDRV_EXTGPIO_SYNC_ENABLE_SET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_syncStatus_get(uint32 unit, uint32 dev, uint32 *pData)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    GETSOCKOPT(RTDRV_EXTGPIO_SYNC_STATUS_GET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);
    *pData = extGpio_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_i2c_read(uint32 unit, uint32 dev, uint32 reg, uint32 *pData)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.reg = reg;
    GETSOCKOPT(RTDRV_EXTGPIO_I2C_READ, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);
    *pData = extGpio_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_i2c_write(uint32 unit, uint32 dev, uint32 reg, uint32 data)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.reg = reg;
    extGpio_cfg.data = data;
    SETSOCKOPT(RTDRV_EXTGPIO_I2C_WRITE, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_i2c_init(uint32 unit, uint32 dev, uint32 i2c_clock, uint32 i2c_data)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.gpioId = i2c_clock;
    extGpio_cfg.data = i2c_data;
    SETSOCKOPT(RTDRV_EXTGPIO_I2C_INIT, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_sync_start(uint32 unit, uint32 dev)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    SETSOCKOPT(RTDRV_EXTGPIO_SYNC_START, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_pin_get(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpio_conf_t *pData)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.gpioId = gpioId;
    GETSOCKOPT(RTDRV_EXTGPIO_PIN_GET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);
    *pData = extGpio_cfg.extGpio_confData;

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_pin_init(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpio_conf_t *pData)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.gpioId = gpioId;
    extGpio_cfg.extGpio_confData = *pData;
    SETSOCKOPT(RTDRV_EXTGPIO_PIN_INIT, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_dataBit_get(uint32 unit, uint32 dev, uint32 gpioId, uint32 *pData)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.gpioId = gpioId;
    GETSOCKOPT(RTDRV_EXTGPIO_DATABIT_GET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);
    *pData = extGpio_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_dataBit_set(uint32 unit, uint32 dev, uint32 gpioId, uint32 data)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.gpioId = gpioId;
    extGpio_cfg.data = data;
    SETSOCKOPT(RTDRV_EXTGPIO_DATABIT_SET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_reg_read(uint32 unit, uint32 dev, uint32 reg, uint32 *pData)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.reg = reg;
    GETSOCKOPT(RTDRV_EXTGPIO_REG_READ, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);
    *pData = extGpio_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_reg_write(uint32 unit, uint32 dev, uint32 reg, uint32 data)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.reg = reg;
    extGpio_cfg.data = data;
    SETSOCKOPT(RTDRV_EXTGPIO_REG_WRITE, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_direction_get(uint32 unit, uint32 dev, uint32 gpioId, drv_gpio_direction_t *pData)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.gpioId = gpioId;
    GETSOCKOPT(RTDRV_EXTGPIO_DIRECTION_GET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);
    *pData = extGpio_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_extGpio_direction_set(uint32 unit, uint32 dev, uint32 gpioId, drv_gpio_direction_t data)
{
    rtdrv_extGpioCfg_t extGpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&extGpio_cfg, 0, sizeof(rtdrv_extGpioCfg_t));
    extGpio_cfg.unit = unit;
    extGpio_cfg.dev = dev;
    extGpio_cfg.gpioId = gpioId;
    extGpio_cfg.data = data;
    SETSOCKOPT(RTDRV_EXTGPIO_DIRECTION_SET, &extGpio_cfg, rtdrv_extGpioCfg_t, 1);

    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_RTL8231 */

int32
rtrpc_gpio_pin_init(
    uint32 unit,
    gpioID gpioId,
    drv_gpio_control_t function,
    drv_gpio_direction_t direction,
    drv_gpio_interruptType_t interruptEnable)
{
    rtdrv_gpioCfg_t gpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&gpio_cfg, 0, sizeof(rtdrv_gpioCfg_t));
    gpio_cfg.unit = unit;
    gpio_cfg.gpioId = gpioId;
    gpio_cfg.function = function;
    gpio_cfg.direction = direction;
    gpio_cfg.interruptEnable = interruptEnable;
    SETSOCKOPT(RTDRV_GPIO_PIN_INIT, &gpio_cfg, rtdrv_gpioCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_gpio_dataBit_init(uint32 unit, gpioID gpioId, uint32 data)
{
    rtdrv_gpioCfg_t gpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&gpio_cfg, 0, sizeof(rtdrv_gpioCfg_t));
    gpio_cfg.unit = unit;
    gpio_cfg.gpioId = gpioId;
    gpio_cfg.data = data;
    SETSOCKOPT(RTDRV_GPIO_DATABIT_INIT, &gpio_cfg, rtdrv_gpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_gpio_dataBit_get(uint32 unit, gpioID gpioId, uint32 *pData)
{
    rtdrv_gpioCfg_t gpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&gpio_cfg, 0, sizeof(rtdrv_gpioCfg_t));
    gpio_cfg.unit = unit;
    gpio_cfg.gpioId = gpioId;
    GETSOCKOPT(RTDRV_GPIO_DATABIT_GET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    *pData = gpio_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_gpio_dataBit_set(uint32 unit, gpioID gpioId, uint32 data)
{
    rtdrv_gpioCfg_t gpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&gpio_cfg, 0, sizeof(rtdrv_gpioCfg_t));
    gpio_cfg.gpioId = gpioId;
    gpio_cfg.data = data;
    gpio_cfg.unit = unit;
    SETSOCKOPT(RTDRV_GPIO_DATABIT_SET, &gpio_cfg, rtdrv_gpioCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_generalCtrlGPIO_dev_init(uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    drv_generalCtrlGpio_devConf_t *pData)
{
    rtdrv_generalCtrlGpioCfg_t generalCtrl_gpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&generalCtrl_gpio_cfg, 0, sizeof(rtdrv_generalCtrlGpioCfg_t));
    generalCtrl_gpio_cfg.unit = unit;
    generalCtrl_gpio_cfg.dev = dev;

    generalCtrl_gpio_cfg.genCtrl_gpioDev.ext_gpio.access_mode = pData->ext_gpio.access_mode;
    generalCtrl_gpio_cfg.genCtrl_gpioDev.ext_gpio.address= pData->ext_gpio.address;

    SETSOCKOPT(RTDRV_GENCTRL_GPIO_DEV_INIT, &generalCtrl_gpio_cfg, rtdrv_generalCtrlGpioCfg_t, 1);

    return RT_ERR_OK;

}


int32
rtrpc_generalCtrlGPIO_dataBit_set(uint32 unit,	drv_generalCtrlGpio_devId_t dev, uint32 pinId, uint32 data)
{
    rtdrv_generalCtrlGpioCfg_t generalCtrl_gpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&generalCtrl_gpio_cfg, 0, sizeof(rtdrv_generalCtrlGpioCfg_t));
    generalCtrl_gpio_cfg.unit = unit;
    generalCtrl_gpio_cfg.dev = dev;
    generalCtrl_gpio_cfg.gpioId = pinId;
    generalCtrl_gpio_cfg.data = data;

    SETSOCKOPT(RTDRV_GENCTRL_GPIO_DATABIT_SET, &generalCtrl_gpio_cfg, rtdrv_generalCtrlGpioCfg_t, 1);

    return RT_ERR_OK;

}

int32
rtrpc_generalCtrlGPIO_dataBit_get(uint32 unit,	drv_generalCtrlGpio_devId_t dev, uint32 pinId, uint32 *data)
{
    rtdrv_generalCtrlGpioCfg_t generalCtrl_gpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&generalCtrl_gpio_cfg, 0, sizeof(rtdrv_generalCtrlGpioCfg_t));
    generalCtrl_gpio_cfg.unit = unit;
    generalCtrl_gpio_cfg.dev = dev;
    generalCtrl_gpio_cfg.gpioId = pinId;

    GETSOCKOPT(RTDRV_GENCTRL_GPIO_DATABIT_GET, &generalCtrl_gpio_cfg, rtdrv_generalCtrlGpioCfg_t, 1);

    *data = generalCtrl_gpio_cfg.data;

    return RT_ERR_OK;

}


int32
rtrpc_generalCtrlGPIO_pin_init(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId,
    drv_generalCtrlGpio_pinConf_t *pData)
{
    rtdrv_generalCtrlGpioCfg_t generalCtrl_gpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&generalCtrl_gpio_cfg, 0, sizeof(rtdrv_generalCtrlGpioCfg_t));
    generalCtrl_gpio_cfg.unit = unit;
    generalCtrl_gpio_cfg.dev = dev;
    generalCtrl_gpio_cfg.gpioId = pinId;

    generalCtrl_gpio_cfg.genCtrl_gpioDev.direction = pData->direction;
    generalCtrl_gpio_cfg.genCtrl_gpioDev.default_value = pData->default_value;

    generalCtrl_gpio_cfg.genCtrl_gpioPin.direction = pData->direction;
    generalCtrl_gpio_cfg.genCtrl_gpioPin.default_value = pData->default_value;

    generalCtrl_gpio_cfg.genCtrl_gpioPin.int_gpio.function = pData->int_gpio.function;
    generalCtrl_gpio_cfg.genCtrl_gpioPin.int_gpio.interruptEnable= pData->int_gpio.interruptEnable;

    generalCtrl_gpio_cfg.genCtrl_gpioPin.ext_gpio.direction = pData->direction;
    generalCtrl_gpio_cfg.genCtrl_gpioPin.ext_gpio.debounce = 0;
    generalCtrl_gpio_cfg.genCtrl_gpioPin.ext_gpio.inverter = 0;


    SETSOCKOPT(RTDRV_GENCTRL_GPIO_PIN_INIT, &generalCtrl_gpio_cfg, rtdrv_generalCtrlGpioCfg_t, 1);

    return RT_ERR_OK;


}


int32
rtrpc_generalCtrlGPIO_devEnable_set(uint32 unit, drv_generalCtrlGpio_devId_t dev, rtk_enable_t enable)
{
    rtdrv_generalCtrlGpioCfg_t generalCtrl_gpio_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&generalCtrl_gpio_cfg, 0, sizeof(rtdrv_generalCtrlGpioCfg_t));
    generalCtrl_gpio_cfg.unit = unit;
    generalCtrl_gpio_cfg.dev = dev;
    generalCtrl_gpio_cfg.data = enable;

    SETSOCKOPT(RTDRV_GENCTRL_GPIO_DEV_ENABLE, &generalCtrl_gpio_cfg, rtdrv_generalCtrlGpioCfg_t, 1);

    return RT_ERR_OK;


}

#if defined(CONFIG_SDK_DRIVER_I2C)
int32
rtrpc_i2c_init(uint32 unit)
{
    rtdrv_i2c_devCfg_t i2c_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&i2c_cfg, 0, sizeof(rtdrv_i2c_devCfg_t));
    i2c_cfg.unit_id = unit;

    SETSOCKOPT(RTDRV_I2C_INIT, &i2c_cfg, rtdrv_i2c_devCfg_t, 1);

    return RT_ERR_OK;
}


int32
rtrpc_i2c_dev_init(uint32 unit, i2c_devConf_t *i2c_dev)
{
    rtdrv_i2c_devCfg_t i2c_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&i2c_cfg, 0, sizeof(rtdrv_i2c_devCfg_t));
    i2c_cfg.unit_id = unit;

    i2c_cfg.device_id = i2c_dev->device_id;
    i2c_cfg.dev_addr = i2c_dev->dev_addr;
    i2c_cfg.mem_addr_width = i2c_dev->mem_addr_width;
    i2c_cfg.data_width= i2c_dev->data_width;
    i2c_cfg.scl_freq = i2c_dev->clk_freq;
    i2c_cfg.scl_delay= i2c_dev->scl_delay;
    i2c_cfg.scl_dev= i2c_dev->scl_dev;
    i2c_cfg.scl_pin_id= i2c_dev->scl_pin_id;
    i2c_cfg.sda_dev= i2c_dev->sda_dev;
    i2c_cfg.sda_pin_id= i2c_dev->sda_pin_id;
    i2c_cfg.i2c_interface_id = i2c_dev->i2c_interface_id;
    i2c_cfg.read_type  = i2c_dev->read_type;

    SETSOCKOPT(RTDRV_I2C_DEV_INIT, &i2c_cfg, rtdrv_i2c_devCfg_t, 1);

    return RT_ERR_OK;


}


int32
rtrpc_i2c_write(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 reg_idx, uint8 *pBuff)
{
    rtdrv_i2c_devCfg_t i2c_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&i2c_cfg, 0, sizeof(rtdrv_i2c_devCfg_t));
    i2c_cfg.unit_id = unit;
    i2c_cfg.device_id = i2c_dev_id;
    osal_memcpy(i2c_cfg.rwdata, pBuff, I2C_DATA_WIDTH_MAX_LEN);
    i2c_cfg.reg_idx = reg_idx ;

    SETSOCKOPT(RTDRV_I2C_WRITE, &i2c_cfg, rtdrv_i2c_devCfg_t, 1);

    return RT_ERR_OK;

}


int32
rtrpc_i2c_read(uint32 unit, drv_i2c_devId_t i2c_dev_id, uint32 reg_idx, uint8 *pBuff)
{
    rtdrv_i2c_devCfg_t i2c_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&i2c_cfg, 0, sizeof(rtdrv_i2c_devCfg_t));
    i2c_cfg.unit_id = unit;
    i2c_cfg.device_id = i2c_dev_id;
    i2c_cfg.reg_idx = reg_idx;

    GETSOCKOPT(RTDRV_I2C_READ, &i2c_cfg, rtdrv_i2c_devCfg_t, 1);
    osal_memcpy( pBuff, i2c_cfg.rwdata, I2C_DATA_WIDTH_MAX_LEN);

    return RT_ERR_OK;

}
#endif /* CONFIG_SDK_DRIVER_I2C */

#if defined(CONFIG_SDK_DRIVER_SPI)
int32 rtrpc_spiPin_init(uint32 unit, spi_init_info_t *init_info)
{
    rtdrv_spiInitInfo_t spi_init_info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&spi_init_info, 0, sizeof(rtdrv_spiInitInfo_t));
    spi_init_info.unit = unit;
    spi_init_info.init_info = *init_info;
    SETSOCKOPT(RTDRV_SPI_INIT, &spi_init_info, rtdrv_spiInitInfo_t, 1);
    return RT_ERR_OK;
}

int32 rtrpc_spi_write(uint32 unit, uint32 mAddrs, uint32 *buff)
{
    rtdrv_spiCfg_t spi_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&spi_cfg, 0, sizeof(rtdrv_spiCfg_t));
    spi_cfg.unit = unit;
    spi_cfg.addr = mAddrs;
    spi_cfg.data = *buff;
    SETSOCKOPT(RTDRV_SPI_WRITE, &spi_cfg, rtdrv_spiCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_spi_read(uint32 unit, uint32 mAddrs, uint32 *buff)
{
    rtdrv_spiCfg_t spi_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&spi_cfg, 0, sizeof(rtdrv_spiCfg_t));
    spi_cfg.unit = unit;
    spi_cfg.addr = mAddrs;
    GETSOCKOPT(RTDRV_SPI_READ, &spi_cfg, rtdrv_spiCfg_t, 1);
    *buff = spi_cfg.data;

    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_DRIVER_SPI */

int32
rtrpc_diag_portRtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    rtdrv_diagCfg_t diag_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&diag_cfg, 0, sizeof(rtdrv_diagCfg_t));
    diag_cfg.unit = unit;
    diag_cfg.port = port;
    GETSOCKOPT(RTDRV_DIAG_RTCTRESULT_GET, &diag_cfg, rtdrv_diagCfg_t, 1);
    osal_memcpy(pRtctResult, &diag_cfg.rtctResult, sizeof(rtk_rtctResult_t));

    return RT_ERR_OK;
}

int32
rtrpc_diag_rtctEnable_set(uint32 unit, rtk_portmask_t *pPortmask)
{
    rtdrv_diagCfg_t diag_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    diag_cfg.unit = unit;
    osal_memcpy(&diag_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_DIAG_RTCTENABLE_SET, &diag_cfg, rtdrv_diagCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_diag_table_whole_read(uint32 unit, uint32 table_index)
{
    rtdrv_diagCfg_t diag_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&diag_cfg, 0, sizeof(rtdrv_diagCfg_t));
    diag_cfg.unit = unit;
    diag_cfg.target_index= table_index;
    diag_cfg.type= DUMP_TYPE_TABLE;
    GETSOCKOPT(RTDRV_DIAG_TABLE_WHOLE_READ, &diag_cfg, rtdrv_diagCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_diag_table_index_name(uint32 unit, uint32 table_index)
{
    rtdrv_diagCfg_t diag_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&diag_cfg, 0, sizeof(rtdrv_diagCfg_t));
    diag_cfg.unit = unit;
    diag_cfg.target_index= table_index;
    diag_cfg.type= DUMP_TYPE_TABLE;
    GETSOCKOPT(RTDRV_DIAG_TABLE_INDEX_NAME, &diag_cfg, rtdrv_diagCfg_t, 1);

    return RT_ERR_OK;
}


int32
rtrpc_diag_tableEntry_read(uint32 unit, uint32 table_index, uint32 start_index, uint32 end_index, uint32 detail)
{
    rtdrv_diag_tableEntryRead_t     info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&info, 0, sizeof(rtdrv_diag_tableEntryRead_t));
    info.unit            = unit;
    info.table_index     = table_index;
    info.ent_start_index = start_index;
    info.ent_end_index   = end_index;
    info.detail          = detail;

    GETSOCKOPT(RTDRV_DIAG_TABLE_ENTRY_READ, &info, rtdrv_diag_tableEntryRead_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_diag_tableEntryDatareg_write(uint32 unit, uint32 table_index, uint32 entry_index, uint32 datareg_index,uint32* pData)
{
    rtdrv_diag_tableEntryDataregWrite_t     info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&info, 0, sizeof(rtdrv_diag_tableEntryDataregWrite_t));
    info.unit            = unit;
    info.table_index     = table_index;
    info.entry_index = entry_index;
    info.datareg_index = datareg_index;
    info.data = *pData;

    SETSOCKOPT(RTDRV_DIAG_TABLE_ENTRY_DATAREG_WRITE, &info, rtdrv_diag_tableEntryDataregWrite_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_diag_tableEntry_write(uint32 unit, uint32 table_index, uint32 entry_index, uint32* pData, uint32 datareg_num)
{
    rtdrv_diag_tableEntryWrite_t     info;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if(datareg_num > sizeof(info.data)/sizeof(uint32))
    {
        return RT_ERR_OUT_OF_RANGE;
    }
    osal_memset(&info, 0, sizeof(rtdrv_diag_tableEntryWrite_t));
    info.unit            = unit;
    info.table_index     = table_index;
    info.entry_index = entry_index;
    info.datareg_num = datareg_num;
    osal_memcpy(&info.data, pData, datareg_num*sizeof(uint32));
    SETSOCKOPT(RTDRV_DIAG_TABLE_ENTRY_WRITE, &info, rtdrv_diag_tableEntryWrite_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_diag_peripheral_register_dump(uint32 unit)
{
    rtdrv_diagCfg_t diag_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&diag_cfg, 0, sizeof(rtdrv_diagCfg_t));
    diag_cfg.unit = unit;
    GETSOCKOPT(RTDRV_DIAG_PERIPHERAL_REG_READ, &diag_cfg, rtdrv_diagCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_diag_reg_whole_read(uint32 unit)
{
    rtdrv_diagCfg_t diag_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&diag_cfg, 0, sizeof(rtdrv_diagCfg_t));
    diag_cfg.unit = unit;
    diag_cfg.type= DUMP_TYPE_REG;
    GETSOCKOPT(RTDRV_DIAG_REG_WHOLE_READ, &diag_cfg, rtdrv_diagCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_diag_phy_reg_whole_read(uint32 unit)
{
    rtdrv_diagCfg_t diag_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&diag_cfg, 0, sizeof(rtdrv_diagCfg_t));
    diag_cfg.unit = unit;
    diag_cfg.type= DUMP_TYPE_REG;
    GETSOCKOPT(RTDRV_DIAG_PHY_REG_READ, &diag_cfg, rtdrv_diagCfg_t, 1);

    return RT_ERR_OK;
}

#if (defined(CONFIG_SDK_APP_DIAG_EXT) && defined (CONFIG_SDK_RTL9300))
int32
rtrpc_diag_table_reg_field_get(uint32 unit, rtk_diag_debug_t *pCfg)
{
    rtdrv_diag_debug_cfg_t diag_debug_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&diag_debug_cfg, 0, sizeof(rtdrv_diag_debug_cfg_t));
    diag_debug_cfg.unit = unit;
    osal_memcpy(&diag_debug_cfg.diag_debug, pCfg, sizeof(rtk_diag_debug_t));
    GETSOCKOPT(RTDRV_DIAG_DEBUG_FIELD_GET, &diag_debug_cfg, rtdrv_diag_debug_cfg_t, 1);
    osal_memcpy(pCfg->value, &diag_debug_cfg.diag_debug.value, sizeof(pCfg->value));

    return RT_ERR_OK;
}

int32
rtrpc_diag_table_reg_field_set(uint32 unit, rtk_diag_debug_t *pCfg)
{
    rtdrv_diag_debug_cfg_t diag_debug_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&diag_debug_cfg, 0, sizeof(rtdrv_diag_debug_cfg_t));
    diag_debug_cfg.unit = unit;
    osal_memcpy(&diag_debug_cfg.diag_debug, pCfg, sizeof(rtk_diag_debug_t));
    SETSOCKOPT(RTDRV_DIAG_DEBUG_FIELD_SET, &diag_debug_cfg, rtdrv_diag_debug_cfg_t, 1);

    return RT_ERR_OK;
}
#endif

int32
rtrpc_diagMapper_init(dal_mapper_t *pMapper)
{
    pMapper->diag_portRtctResult_get = rtrpc_diag_portRtctResult_get;
    pMapper->diag_rtctEnable_set = rtrpc_diag_rtctEnable_set;
    return RT_ERR_OK;
}

