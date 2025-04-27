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
 *           1) ACL
 *
 */

#include <rtk/acl.h>
#include <dal/rtrpc/rtrpc_acl.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>


int32
rtrpc_acl_ruleEntryFieldSize_get(uint32 unit, rtk_acl_fieldType_t type, uint32 *pField_size)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.field_type = type;
    GETSOCKOPT(RTDRV_ACL_ENTRY_FIELD_SIZE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pField_size = acl_cfg.size;

    return RT_ERR_OK;
}

int32
rtrpc_acl_ruleEntrySize_get(uint32 unit, rtk_acl_phase_t phase, uint32 *pEntry_size)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    GETSOCKOPT(RTDRV_ACL_ENTRY_SIZE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pEntry_size = acl_cfg.size;

    return RT_ERR_OK;
}

int32
rtrpc_acl_ruleEntry_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    rtdrv_aclCfg_t  acl_cfg;
    uint32          entry_size = 0;
    int32           ret;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));
    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;
    GETSOCKOPT(RTDRV_ACL_ENTRY_DATA_READ, &acl_cfg, rtdrv_aclCfg_t, 1);

    RT_ERR_CHK(rtk_acl_ruleEntrySize_get(master_view_unit, phase, &entry_size), ret);
    osal_memcpy(pEntry_buffer, acl_cfg.entry_buffer, entry_size);

    return RT_ERR_OK;
}

int32
rtrpc_acl_ruleEntryField_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;
    acl_cfg.field_type = type;
    osal_memcpy(acl_cfg.field_data, pData, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memcpy(acl_cfg.field_mask, pMask, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    SETSOCKOPT(RTDRV_ACL_ENTRY_DATA_WRITE, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_acl_portLookupEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.port = port;

    GETSOCKOPT(RTDRV_ACL_PORTLOOKUPENABLE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pEnable = acl_cfg.status;

    return RT_ERR_OK;
} /* end of rtk_acl_portLookupEnable_get */

int32
rtrpc_acl_portLookupEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.port = port;
    acl_cfg.status = enable;

    SETSOCKOPT(RTDRV_ACL_PORTLOOKUPENABLE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_acl_portLookupEnable_set */

int32
rtrpc_acl_lookupMissAct_get(uint32 unit, rtk_port_t port, rtk_acl_lookupMissAct_t *pLmAct)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.port = port;

    GETSOCKOPT(RTDRV_ACL_LOOKUPMISSACT_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pLmAct = acl_cfg.lmAct;

    return RT_ERR_OK;
} /* end of rtk_acl_lookupMissAct_get */

int32
rtrpc_acl_lookupMissAct_set(uint32 unit, rtk_port_t port, rtk_acl_lookupMissAct_t lmAct)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.port = port;
    acl_cfg.lmAct = lmAct;

    SETSOCKOPT(RTDRV_ACL_LOOKUPMISSACT_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_acl_lookupMissAct_set */

int32
rtrpc_acl_rangeCheckFieldSelector_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_fieldSelector_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;

    GETSOCKOPT(RTDRV_ACL_RANGECHECKFIELDSEL_GET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);

    osal_memcpy(pData, &acl_cfg.range_fieldSel, sizeof(rtk_acl_rangeCheck_fieldSelector_t));

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckL4Port_get */

int32
rtrpc_acl_rangeCheckFieldSelector_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_fieldSelector_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;
    osal_memcpy(&acl_cfg.range_fieldSel, pData, sizeof(rtk_acl_rangeCheck_fieldSelector_t));

    SETSOCKOPT(RTDRV_ACL_RANGECHECKFIELDSEL_SET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckL4Port_set */

int32
rtrpc_acl_ruleValidate_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              *pValid)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;

    GETSOCKOPT(RTDRV_ACL_RULEVALIDATE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pValid = acl_cfg.status;

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleValidate_get */

int32
rtrpc_acl_ruleValidate_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              valid)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;
    acl_cfg.status = valid;

    SETSOCKOPT(RTDRV_ACL_RULEVALIDATE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleValidate_set */

int32
rtrpc_acl_ruleEntry_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    rtdrv_aclCfg_t  acl_cfg;
    uint32          entry_size = 0;
    int32           ret;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;

    RT_ERR_CHK(rtk_acl_ruleEntrySize_get(master_view_unit, phase, &entry_size), ret);
    osal_memcpy(acl_cfg.entry_buffer, pEntry_buffer, entry_size);

    SETSOCKOPT(RTDRV_ACL_RULEENTRY_WRITE, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleEntry_write */

int32
rtrpc_acl_ruleEntryField_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    rtdrv_aclCfg_t  acl_cfg;
    uint32          entry_size = 0;
    int32           ret;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;
    acl_cfg.field_type = type;

    RT_ERR_CHK(rtk_acl_ruleEntrySize_get(master_view_unit, phase, &entry_size), ret);
    osal_memcpy(acl_cfg.entry_buffer, pEntry_buffer, entry_size);

    GETSOCKOPT(RTDRV_ACL_RULEENTRYFIELD_GET, &acl_cfg, rtdrv_aclCfg_t, 1);

    osal_memcpy(pData, acl_cfg.field_data, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memcpy(pMask, acl_cfg.field_mask, RTK_MAX_SIZE_OF_ACL_USER_FIELD);

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleEntryField_get */

int32
rtrpc_acl_ruleEntryField_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    rtdrv_aclCfg_t  acl_cfg;
    uint32          entry_size = 0;
    int32           ret;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;
    acl_cfg.field_type = type;

    RT_ERR_CHK(rtk_acl_ruleEntrySize_get(master_view_unit, phase, &entry_size), ret);
    osal_memcpy(acl_cfg.entry_buffer, pEntry_buffer, entry_size);

    osal_memcpy(acl_cfg.field_data, pData, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memcpy(acl_cfg.field_mask, pMask, RTK_MAX_SIZE_OF_ACL_USER_FIELD);

    SETSOCKOPT(RTDRV_ACL_RULEENTRYFIELD_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    osal_memcpy(pEntry_buffer, acl_cfg.entry_buffer, entry_size);

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleEntryField_set */

int32
rtrpc_acl_ruleEntryField_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));
    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;
    acl_cfg.field_type = type;

    GETSOCKOPT(RTDRV_ACL_RULEENTRYFIELD_READ, &acl_cfg, rtdrv_aclCfg_t, 1);

    osal_memcpy(pData, acl_cfg.field_data, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memcpy(pMask, acl_cfg.field_mask, RTK_MAX_SIZE_OF_ACL_USER_FIELD);

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleEntryField_read */

int32
rtrpc_acl_ruleEntryField_check(uint32 unit, rtk_acl_phase_t phase,
        rtk_acl_fieldType_t type)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.field_type = type;

    GETSOCKOPT(RTDRV_ACL_RULEENTRYFIELD_CHECK, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleEntryField_check */

int32
rtrpc_acl_ruleOperation_get(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;

    GETSOCKOPT(RTDRV_ACL_RULEOPERATION_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pOperation, &acl_cfg.oper, sizeof(rtk_acl_operation_t));

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleOperation_get */

int32
rtrpc_acl_ruleOperation_set(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;
    osal_memcpy(&acl_cfg.oper, pOperation, sizeof(rtk_acl_operation_t));

    SETSOCKOPT(RTDRV_ACL_RULEOPERATION_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleOperation_set */

int32
rtrpc_acl_ruleAction_get(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;

    GETSOCKOPT(RTDRV_ACL_RULEACTION_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pAction, &acl_cfg.action, sizeof(rtk_acl_action_t));

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleAction_get */

int32
rtrpc_acl_ruleAction_set(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.phase = phase;
    acl_cfg.index = entry_idx;
    osal_memcpy(&acl_cfg.action, pAction, sizeof(rtk_acl_action_t));

    SETSOCKOPT(RTDRV_ACL_RULEACTION_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_ruleAction_set */

int32
rtrpc_acl_blockPwrEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.blockIdx = block_idx;

    GETSOCKOPT(RTDRV_ACL_BLOCKPWRENABLE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pEnable = acl_cfg.status;

    return RT_ERR_OK;
}    /* end of rtk_acl_blockPwrEnable_get */

int32
rtrpc_acl_blockPwrEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.blockIdx = block_idx;
    acl_cfg.status = enable;

    SETSOCKOPT(RTDRV_ACL_BLOCKPWRENABLE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_blockPwrEnable_set */

int32
rtrpc_acl_blockGroupEnable_get(
    uint32                     unit,
    uint32                     block_idx,
    rtk_acl_blockGroup_t       group_type,
    rtk_enable_t               *pEnable)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.blockIdx = block_idx;
    acl_cfg.blk_group = group_type;

    GETSOCKOPT(RTDRV_ACL_BLOCKAGGREGATORENABLE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);

    *pEnable = acl_cfg.status;

    return RT_ERR_OK;
}    /* end of rtk_acl_blockGroupEnable_get */

int32
rtrpc_acl_blockGroupEnable_set(
    uint32                     unit,
    uint32                     block_idx,
    rtk_acl_blockGroup_t       group_type,
    rtk_enable_t               enable)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.blockIdx = block_idx;
    acl_cfg.blk_group = group_type;
    acl_cfg.status = enable;

    SETSOCKOPT(RTDRV_ACL_BLOCKAGGREGATORENABLE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_blockGroupEnable_set */

int32
rtrpc_acl_statPktCnt_get(uint32 unit, uint32 log_id, uint32 *pPkt_cnt)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = log_id;

    GETSOCKOPT(RTDRV_ACL_STATPKTCNT_GET, &acl_cfg, rtdrv_aclCfg_t, 1);

    *pPkt_cnt = acl_cfg.size;

    return RT_ERR_OK;
}    /* end of rtk_acl_statPktCnt_get */

int32
rtrpc_acl_statPktCnt_clear(uint32 unit, uint32 log_id)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = log_id;

    SETSOCKOPT(RTDRV_ACL_STATPKTCNT_CLEAR, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_statPktCnt_clear */

int32
rtrpc_acl_statByteCnt_get(uint32 unit, uint32 log_id, uint64 *pByte_cnt)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = log_id;

    GETSOCKOPT(RTDRV_ACL_STATBYTECNT_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pByte_cnt = acl_cfg.count;

    return RT_ERR_OK;
}    /* end of rtk_acl_statByteCnt_get */

int32
rtrpc_acl_statByteCnt_clear(uint32 unit, uint32 log_id)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = log_id;

    SETSOCKOPT(RTDRV_ACL_STATBYTECNT_CLEAR, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_statByteCnt_clear */

int32
rtrpc_acl_stat_clearAll(uint32 unit)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;

    SETSOCKOPT(RTDRV_ACL_STAT_CLEARALL, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_stat_clearAll */

int32
rtrpc_acl_rangeCheckL4Port_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));
    acl_cfg.unit = unit;
    acl_cfg.index = index;

    GETSOCKOPT(RTDRV_ACL_RANGECHECKL4PORT_GET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);

    osal_memcpy(pData, &acl_cfg.range_l4Port, sizeof(rtk_acl_rangeCheck_l4Port_t));

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckL4Port_get */

int32
rtrpc_acl_rangeCheckL4Port_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;
    osal_memcpy(&acl_cfg.range_l4Port, pData, sizeof(rtk_acl_rangeCheck_l4Port_t));

    SETSOCKOPT(RTDRV_ACL_RANGECHECKL4PORT_SET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckL4Port_set */

int32
rtrpc_acl_rangeCheckVid_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;

    GETSOCKOPT(RTDRV_ACL_RANGECHECKVID_GET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);

    osal_memcpy(pData, &acl_cfg.range_vid, sizeof(rtk_acl_rangeCheck_vid_t));

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckVid_get */

int32
rtrpc_acl_rangeCheckVid_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;
    osal_memcpy(&acl_cfg.range_vid, pData, sizeof(rtk_acl_rangeCheck_vid_t));

    SETSOCKOPT(RTDRV_ACL_RANGECHECKVID_SET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckVid_set */

int32
rtrpc_acl_rangeCheckSrcPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;

    GETSOCKOPT(RTDRV_ACL_RANGECHECKSRCPORT_GET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);
    osal_memcpy(pData, &acl_cfg.range_port, sizeof(rtk_acl_rangeCheck_portMask_t));

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckSrcPort_get */

int32
rtrpc_acl_rangeCheckSrcPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;
    osal_memcpy(&acl_cfg.range_port, pData, sizeof(rtk_acl_rangeCheck_portMask_t));

    SETSOCKOPT(RTDRV_ACL_RANGECHECKSRCPORT_SET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckSrcPort_set */

int32
rtrpc_acl_rangeCheckPacketLen_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;

    GETSOCKOPT(RTDRV_ACL_RANGECHECKPACKETLEN_GET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);
    osal_memcpy(pData, &acl_cfg.range_pktLen, sizeof(rtk_acl_rangeCheck_packetLen_t));

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckPacketLen_get */

int32
rtrpc_acl_rangeCheckPacketLen_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;
    osal_memcpy(&acl_cfg.range_pktLen, pData, sizeof(rtk_acl_rangeCheck_packetLen_t));

    SETSOCKOPT(RTDRV_ACL_RANGECHECKPACKETLEN_SET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckPacketLen_set */

int32
rtrpc_acl_meterBurstSize_get(
    uint32              unit,
    rtk_acl_meterMode_t meterMode,
    rtk_acl_meterBurstSize_t  *pBurstSize)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.meterMode = meterMode;
    GETSOCKOPT(RTDRV_ACL_METER_BURST_SIZE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pBurstSize = acl_cfg.burstSize;

    return RT_ERR_OK;
}

int32
rtrpc_acl_meterBurstSize_set(
    uint32              unit,
    rtk_acl_meterMode_t meterMode,
    rtk_acl_meterBurstSize_t  *pBurstSize)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.meterMode = meterMode;
    acl_cfg.burstSize = *pBurstSize;
    SETSOCKOPT(RTDRV_ACL_METER_BURST_SIZE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_acl_meterMode_get(
    uint32  unit,
    uint32  blockIdx,
    rtk_acl_meterMode_t *pMeterMode)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.blockIdx = blockIdx;
    GETSOCKOPT(RTDRV_ACL_METER_MODE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pMeterMode = acl_cfg.meterMode;

    return RT_ERR_OK;
}

int32
rtrpc_acl_meterMode_set(
    uint32  unit,
    uint32  blockIdx,
    rtk_acl_meterMode_t meterMode)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.blockIdx = blockIdx;
    acl_cfg.meterMode = meterMode;

    SETSOCKOPT(RTDRV_ACL_METER_MODE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_acl_loopBackEnable_get(uint32 unit, uint32 *pEnable)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_ACL_LOOPBACKENABLE_GET, &cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_acl_loopBackEnable_get */

int32
rtrpc_acl_loopBackEnable_set(uint32 unit, uint32 enable)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.enable, &enable, sizeof(uint32));
    SETSOCKOPT(RTDRV_ACL_LOOPBACKENABLE_SET, &cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_acl_loopBackEnable_set */

int32
rtrpc_acl_limitLoopbackTimes_get(uint32 unit, uint32 *pLb_times)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLb_times), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_ACL_LIMITLOOPBACKTIMES_GET, &cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pLb_times, &cfg.lb_times, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_acl_limitLoopbackTimes_get */

int32
rtrpc_acl_limitLoopbackTimes_set(uint32 unit, uint32 lb_times)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.lb_times, &lb_times, sizeof(uint32));
    SETSOCKOPT(RTDRV_ACL_LIMITLOOPBACKTIMES_SET, &cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_acl_limitLoopbackTimes_set */

int32
rtrpc_acl_portPhaseLookupEnable_get(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 *pEnable)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    GETSOCKOPT(RTDRV_ACL_PORTPHASELOOKUPENABLE_GET, &cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pEnable, &cfg.status, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_acl_portPhaseLookupEnable_get */

int32
rtrpc_acl_portPhaseLookupEnable_set(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 enable)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    osal_memcpy(&cfg.status, &enable, sizeof(uint32));
    SETSOCKOPT(RTDRV_ACL_PORTPHASELOOKUPENABLE_SET, &cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_acl_portPhaseLookupEnable_set */

int32
rtrpc_acl_partition_get(uint32 unit, uint32 *pPartition)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;

    GETSOCKOPT(RTDRV_ACL_PARTITION_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pPartition = acl_cfg.blockIdx;

    return RT_ERR_OK;
}    /* end of rtk_acl_partition_get */

int32
rtrpc_acl_partition_set(uint32 unit, uint32 partition)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.blockIdx = partition;

    SETSOCKOPT(RTDRV_ACL_PARTITION_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_partition_set */

int32
rtrpc_acl_templateFieldIntentVlanTag_get(uint32 unit,
    rtk_vlan_tagType_t *tagType)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == tagType), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&acl_cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_ACL_TEMPLATEFIELDINTENTVLANTAG_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(tagType, &acl_cfg.tagType, sizeof(rtk_vlan_tagType_t));

    return RT_ERR_OK;
}   /* end of rtk_acl_templateFieldIntentVlanTag_get */

int32
rtrpc_acl_templateFieldIntentVlanTag_set(uint32 unit,
    rtk_vlan_tagType_t tagType)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&acl_cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&acl_cfg.tagType, &tagType, sizeof(rtk_vlan_tagType_t));
    SETSOCKOPT(RTDRV_ACL_TEMPLATEFIELDINTENTVLANTAG_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_acl_templateFieldIntentVlanTag_set */

int32
rtrpc_acl_blockResultMode_get(uint32 unit, uint32 block_idx, rtk_acl_blockResultMode_t *pMode)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.blockIdx = block_idx;

    GETSOCKOPT(RTDRV_ACL_BLOCKRESULTMODE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    *pMode = acl_cfg.blk_mode;

    return RT_ERR_OK;
}    /* end of rtk_acl_blockResultMode_get */

int32
rtrpc_acl_blockResultMode_set(uint32 unit, uint32 block_idx, rtk_acl_blockResultMode_t mode)
{
    rtdrv_aclCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_aclCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.blockIdx = block_idx;
    acl_cfg.blk_mode = mode;

    SETSOCKOPT(RTDRV_ACL_BLOCKRESULTMODE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_blockResultMode_set */

int32
rtrpc_acl_rangeCheckDstPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;

    GETSOCKOPT(RTDRV_ACL_RANGECHECKDSTPORT_GET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);
    osal_memcpy(pData, &acl_cfg.range_port, sizeof(rtk_acl_rangeCheck_portMask_t));

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckDstPort_get */
int32
rtrpc_acl_rangeCheckDstPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    rtdrv_rangeCheckCfg_t acl_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&acl_cfg, 0, sizeof(rtdrv_rangeCheckCfg_t));

    acl_cfg.unit = unit;
    acl_cfg.index = index;
    osal_memcpy(&acl_cfg.range_port, pData, sizeof(rtk_acl_rangeCheck_portMask_t));

    SETSOCKOPT(RTDRV_ACL_RANGECHECKDSTPORT_SET, &acl_cfg, rtdrv_rangeCheckCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_rangeCheckDstPort_set */

int32
rtrpc_acl_templateSelector_get(uint32 unit, uint32 block_idx,
    rtk_acl_templateIdx_t *pTemplate_idx)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    cfg.unit = unit;
    cfg.blockIdx = block_idx;

    GETSOCKOPT(RTDRV_ACL_TEMPLATESELECTOR_GET, &cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pTemplate_idx, &cfg.template_idx, sizeof(rtk_acl_templateIdx_t));

    return RT_ERR_OK;
}    /* end of rtk_acl_templateSelector_get */

int32
rtrpc_acl_templateSelector_set(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   template_idx)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));

    cfg.unit = unit;
    cfg.blockIdx = block_idx;
    osal_memcpy(&cfg.template_idx, &template_idx, sizeof(rtk_acl_templateIdx_t));

    SETSOCKOPT(RTDRV_ACL_TEMPLATESELECTOR_SET, &cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_acl_templateSelector_set */

int32
rtrpc_acl_statCnt_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_statMode_t mode, uint64 *pCnt)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    osal_memcpy(&cfg.index, &entryIdx, sizeof(rtk_acl_id_t));
    osal_memcpy(&cfg.mode, &mode, sizeof(rtk_acl_statMode_t));
    GETSOCKOPT(RTDRV_ACL_STATCNT_GET, &cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pCnt, &cfg.cnt, sizeof(uint64));

    return RT_ERR_OK;
}   /* end of rtk_acl_statCnt_get */

int32
rtrpc_acl_statCnt_clear(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_statMode_t mode)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    osal_memcpy(&cfg.index, &entryIdx, sizeof(rtk_acl_id_t));
    osal_memcpy(&cfg.mode, &mode, sizeof(rtk_acl_statMode_t));
    SETSOCKOPT(RTDRV_ACL_STATCNT_CLEAR, &cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_acl_statCnt_clear */

int32
rtrpc_acl_ruleHitIndication_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 reset, uint32 *pIsHit)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    osal_memcpy(&cfg.index, &entry_idx, sizeof(rtk_acl_id_t));
    osal_memcpy(&cfg.reset, &reset, sizeof(uint32));
    GETSOCKOPT(RTDRV_ACL_RULEHITINDICATION_GET, &cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pIsHit, &cfg.isHit, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_acl_ruleHitIndication_get */

int32
rtrpc_acl_ruleHitIndicationMask_get(uint32 unit, rtk_acl_phase_t phase,
        uint32 reset, rtk_acl_hitMask_t *pHitMask)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHitMask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    osal_memcpy(&cfg.reset, &reset, sizeof(uint32));
    GETSOCKOPT(RTDRV_ACL_RULEHITINDICATIONMASK_GET, &cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pHitMask, &cfg.hit_mask, sizeof(rtk_acl_hitMask_t));

    return RT_ERR_OK;
}   /* end of rtk_acl_ruleHitIndication_get */

int32
rtrpc_acl_rule_del(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_clear_t *pClrIdx)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    osal_memcpy(&cfg.clrIdx, pClrIdx, sizeof(rtk_acl_clear_t));
    SETSOCKOPT(RTDRV_ACL_RULE_DEL, &cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_acl_rule_del */

int32
rtrpc_acl_rule_move(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pData)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    osal_memcpy(&cfg.mv, pData, sizeof(rtk_acl_move_t));
    SETSOCKOPT(RTDRV_ACL_RULE_MOVE, &cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_acl_rule_move */

int32
rtrpc_acl_ruleEntryField_validate(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_fieldType_t type)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    osal_memcpy(&cfg.index, &entry_idx, sizeof(rtk_acl_id_t));
    osal_memcpy(&cfg.field_type, &type, sizeof(rtk_acl_fieldType_t));
    SETSOCKOPT(RTDRV_ACL_RULEENTRYFIELD_VALIDATE, &cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_acl_ruleEntryField_validate */

int32
rtrpc_acl_fieldUsr2Template_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_fieldType_t type, rtk_acl_fieldUsr2Template_t *info)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == info), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    memcpy(&cfg.field_type, &type, sizeof(rtk_acl_fieldType_t));
    GETSOCKOPT(RTDRV_ACL_FIELDUSR2TMPLATE_GET, &cfg, rtdrv_aclCfg_t, 1);
    memcpy(info, &cfg.usr2tmplte, sizeof(rtk_acl_fieldUsr2Template_t));

    return RT_ERR_OK;
}   /* end of rtrpc_acl_fieldUsr2Template_get */

int32
rtrpc_acl_templateId_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 *pTemplate_id)
{
    rtdrv_aclCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTemplate_id), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_aclCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_acl_phase_t));
    osal_memcpy(&cfg.index, &entry_idx, sizeof(rtk_acl_id_t));
    GETSOCKOPT(RTDRV_ACL_TEMPLATEID_GET, &cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pTemplate_id, &cfg.entry_template_id, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtrpc_acl_templateId_get */

int32
rtrpc_aclMapper_init(dal_mapper_t *pMapper)
{
    pMapper->acl_portLookupEnable_get = rtrpc_acl_portLookupEnable_get;
    pMapper->acl_portLookupEnable_set = rtrpc_acl_portLookupEnable_set;
    pMapper->acl_lookupMissAct_get = rtrpc_acl_lookupMissAct_get;
    pMapper->acl_lookupMissAct_set = rtrpc_acl_lookupMissAct_set;
    pMapper->acl_rangeCheckFieldSelector_get = rtrpc_acl_rangeCheckFieldSelector_get;
    pMapper->acl_rangeCheckFieldSelector_set = rtrpc_acl_rangeCheckFieldSelector_set;
    pMapper->acl_ruleEntryFieldSize_get = rtrpc_acl_ruleEntryFieldSize_get;
    pMapper->acl_ruleEntrySize_get = rtrpc_acl_ruleEntrySize_get;
    pMapper->acl_ruleValidate_get = rtrpc_acl_ruleValidate_get;
    pMapper->acl_ruleValidate_set = rtrpc_acl_ruleValidate_set;
    pMapper->acl_ruleEntry_read = rtrpc_acl_ruleEntry_read;
    pMapper->acl_ruleEntry_write = rtrpc_acl_ruleEntry_write;
    pMapper->acl_ruleEntryField_get = rtrpc_acl_ruleEntryField_get;
    pMapper->acl_ruleEntryField_set = rtrpc_acl_ruleEntryField_set;
    pMapper->acl_ruleEntryField_read = rtrpc_acl_ruleEntryField_read;
    pMapper->acl_ruleEntryField_write = rtrpc_acl_ruleEntryField_write;
    pMapper->acl_ruleOperation_get = rtrpc_acl_ruleOperation_get;
    pMapper->acl_ruleOperation_set = rtrpc_acl_ruleOperation_set;
    pMapper->acl_ruleAction_get = rtrpc_acl_ruleAction_get;
    pMapper->acl_ruleAction_set = rtrpc_acl_ruleAction_set;
    pMapper->acl_blockGroupEnable_get = rtrpc_acl_blockGroupEnable_get;
    pMapper->acl_blockGroupEnable_set = rtrpc_acl_blockGroupEnable_set;
    pMapper->acl_statPktCnt_get = rtrpc_acl_statPktCnt_get;
    pMapper->acl_statPktCnt_clear = rtrpc_acl_statPktCnt_clear;
    pMapper->acl_statByteCnt_get = rtrpc_acl_statByteCnt_get;
    pMapper->acl_statByteCnt_clear = rtrpc_acl_statByteCnt_clear;
    pMapper->acl_stat_clearAll = rtrpc_acl_stat_clearAll;
    pMapper->acl_rangeCheckL4Port_get = rtrpc_acl_rangeCheckL4Port_get;
    pMapper->acl_rangeCheckL4Port_set = rtrpc_acl_rangeCheckL4Port_set;
    pMapper->acl_rangeCheckVid_get = rtrpc_acl_rangeCheckVid_get;
    pMapper->acl_rangeCheckVid_set = rtrpc_acl_rangeCheckVid_set;
    pMapper->acl_rangeCheckSrcPort_get = rtrpc_acl_rangeCheckSrcPort_get;
    pMapper->acl_rangeCheckSrcPort_set = rtrpc_acl_rangeCheckSrcPort_set;
    pMapper->acl_rangeCheckPacketLen_get = rtrpc_acl_rangeCheckPacketLen_get;
    pMapper->acl_rangeCheckPacketLen_set = rtrpc_acl_rangeCheckPacketLen_set;
    pMapper->acl_ruleEntryField_check = rtrpc_acl_ruleEntryField_check;
    pMapper->acl_blockPwrEnable_get = rtrpc_acl_blockPwrEnable_get;
    pMapper->acl_blockPwrEnable_set = rtrpc_acl_blockPwrEnable_set;
    pMapper->acl_meterMode_get = rtrpc_acl_meterMode_get;
    pMapper->acl_meterMode_set = rtrpc_acl_meterMode_set;
    pMapper->acl_meterBurstSize_get = rtrpc_acl_meterBurstSize_get;
    pMapper->acl_meterBurstSize_set = rtrpc_acl_meterBurstSize_set;
    pMapper->acl_partition_get = rtrpc_acl_partition_get;
    pMapper->acl_partition_set = rtrpc_acl_partition_set;
    pMapper->acl_blockResultMode_get = rtrpc_acl_blockResultMode_get;
    pMapper->acl_blockResultMode_set = rtrpc_acl_blockResultMode_set;
    pMapper->acl_templateFieldIntentVlanTag_get = rtrpc_acl_templateFieldIntentVlanTag_get;
    pMapper->acl_templateFieldIntentVlanTag_set = rtrpc_acl_templateFieldIntentVlanTag_set;
    pMapper->acl_rangeCheckDstPort_get = rtrpc_acl_rangeCheckDstPort_get;
    pMapper->acl_rangeCheckDstPort_set = rtrpc_acl_rangeCheckDstPort_set;
    pMapper->acl_loopBackEnable_get = rtrpc_acl_loopBackEnable_get;
    pMapper->acl_loopBackEnable_set = rtrpc_acl_loopBackEnable_set;
    pMapper->acl_limitLoopbackTimes_get = rtrpc_acl_limitLoopbackTimes_get;
    pMapper->acl_limitLoopbackTimes_set = rtrpc_acl_limitLoopbackTimes_set;
    pMapper->acl_portPhaseLookupEnable_get = rtrpc_acl_portPhaseLookupEnable_get;
    pMapper->acl_portPhaseLookupEnable_set = rtrpc_acl_portPhaseLookupEnable_set;
    pMapper->acl_templateSelector_get = rtrpc_acl_templateSelector_get;
    pMapper->acl_templateSelector_set = rtrpc_acl_templateSelector_set;
    pMapper->acl_statCnt_get = rtrpc_acl_statCnt_get;
    pMapper->acl_statCnt_clear = rtrpc_acl_statCnt_clear;
    pMapper->acl_ruleHitIndication_get = rtrpc_acl_ruleHitIndication_get;
    pMapper->acl_rule_del = rtrpc_acl_rule_del;
    pMapper->acl_rule_move = rtrpc_acl_rule_move;
    pMapper->acl_ruleEntryField_validate = rtrpc_acl_ruleEntryField_validate;
    pMapper->acl_fieldUsr2Template_get = rtrpc_acl_fieldUsr2Template_get;
    pMapper->acl_templateId_get = rtrpc_acl_templateId_get;
    return RT_ERR_OK;
}

