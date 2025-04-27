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
 * $Revision: 83888 $
 * $Date: 2017-11-30 15:11:21 +0800 (Thu, 30 Nov 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) OpenFlow
 *
 */

#include <dal/rtrpc/rtrpc_openflow.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

/*
 * Function Declaration
 */

int32
rtrpc_of_init(uint32 unit)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    SETSOCKOPT(RTDRV_OF_INIT, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_classifier_get(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t *pData)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    cfg.unit = unit;
    cfg.classifyType = type;
    osal_memcpy(&cfg.classifyData, pData, sizeof(rtk_of_classifierData_t));
    GETSOCKOPT(RTDRV_OF_CLASSIFIER_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pData, &cfg.classifyData, sizeof(rtk_of_classifierData_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_classifier_set(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t data)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.classifyType, &type, sizeof(rtk_of_classifierType_t));
    osal_memcpy(&cfg.classifyData, &data, sizeof(rtk_of_classifierData_t));
    SETSOCKOPT(RTDRV_OF_CLASSIFIER_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowMatchFieldSize_get(uint32 unit, rtk_of_matchfieldType_t type, uint32 *pField_size)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pField_size), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.matchFieldType, &type, sizeof(rtk_of_matchfieldType_t));
    GETSOCKOPT(RTDRV_OF_FLOWMATCHFIELDSIZE_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pField_size, &cfg.field_size, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntrySize_get(uint32 unit, rtk_of_flowtable_phase_t phase, uint32 *pEntry_size)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry_size), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    GETSOCKOPT(RTDRV_OF_FLOWENTRYSIZE_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pEntry_size, &cfg.entry_size, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryValidate_get(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		*pValid)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == 		pValid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    GETSOCKOPT(RTDRV_OF_FLOWENTRYVALIDATE_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pValid, &cfg.pValid, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryValidate_set(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		valid)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    osal_memcpy(&cfg.valid, &valid, sizeof(uint32));
    SETSOCKOPT(RTDRV_OF_FLOWENTRYVALIDATE_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryFieldList_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldList_t  *list)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    GETSOCKOPT(RTDRV_OF_FLOWENTRYFIELDLIST_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(list, &cfg.matchFieldList, sizeof(rtk_of_matchfieldList_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldType_t  type)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.matchFieldType, &type, sizeof(rtk_of_matchfieldType_t));
    GETSOCKOPT(RTDRV_OF_FLOWENTRYFIELD_CHECK, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntrySetField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   field_id,
    rtk_of_setFieldType_t    type)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.field_id, &field_id, sizeof(uint32));
    osal_memcpy(&cfg.setFieldType, &type, sizeof(rtk_of_setFieldType_t));
    GETSOCKOPT(RTDRV_OF_FLOWENTRYSETFIELD_CHECK, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryField_read(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    osal_memcpy(&cfg.matchFieldType, &type, sizeof(rtk_of_matchfieldType_t));
    GETSOCKOPT(RTDRV_OF_FLOWENTRYFIELD_READ, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pData, cfg.fieldData, RTK_OF_MATCH_FIELD_MAX);
    osal_memcpy(pMask, cfg.fieldMask, RTK_OF_MATCH_FIELD_MAX);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryField_write(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    osal_memcpy(&cfg.matchFieldType, &type, sizeof(rtk_of_matchfieldType_t));
    osal_memcpy(&cfg.fieldData, pData, RTK_OF_MATCH_FIELD_MAX);
    osal_memcpy(&cfg.fieldMask, pMask, RTK_OF_MATCH_FIELD_MAX);
    SETSOCKOPT(RTDRV_OF_FLOWENTRYFIELD_WRITE, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryOperation_get(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == 	pOperation), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &	unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    GETSOCKOPT(RTDRV_OF_FLOWENTRYOPERATION_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pOperation, &cfg.pOperation, sizeof(rtk_of_flowOperation_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryOperation_set(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == 	pOperation), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &	unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    osal_memcpy(&cfg.pOperation, pOperation, sizeof(rtk_of_flowOperation_t));
    SETSOCKOPT(RTDRV_OF_FLOWENTRYOPERATION_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryInstruction_get(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    GETSOCKOPT(RTDRV_OF_FLOWENTRYINSTRUCTION_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pData, &cfg.flowInsData, sizeof(rtk_of_flowIns_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryInstruction_set(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    osal_memcpy(&cfg.flowInsData, pData, sizeof(rtk_of_flowIns_t));
    SETSOCKOPT(RTDRV_OF_FLOWENTRYINSTRUCTION_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntryHitSts_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint32                   reset,
    uint32                   *pIsHit)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    osal_memcpy(&cfg.reset, &reset, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_FLOWENTRYHITSTS_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pIsHit, &cfg.isHit, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntry_del(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowClear_t *pClrIdx)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClrIdx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.clrIdx, pClrIdx, sizeof(rtk_of_flowClear_t));
    SETSOCKOPT(RTDRV_OF_FLOWENTRY_DEL, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowEntry_move(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowMove_t *pData)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.moveIdx, pData, sizeof(rtk_of_flowMove_t));
    SETSOCKOPT(RTDRV_OF_FLOWENTRY_MOVE, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_ftTemplateSelector_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   *pTemplate_idx)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTemplate_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.block_idx, &block_idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_FTTEMPLATESELECTOR_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pTemplate_idx, &cfg.template_idx, sizeof(rtk_of_ftTemplateIdx_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_ftTemplateSelector_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   template_idx)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.block_idx, &block_idx, sizeof(uint32));
    osal_memcpy(&cfg.template_idx, &template_idx, sizeof(rtk_of_ftTemplateIdx_t));
    SETSOCKOPT(RTDRV_OF_FTTEMPLATESELECTOR_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowCntMode_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     *pMode)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    GETSOCKOPT(RTDRV_OF_FLOWCNTMODE_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pMode, &cfg.mode, sizeof(rtk_of_flowCntMode_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_flowCntMode_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     mode)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    osal_memcpy(&cfg.mode, &mode, sizeof(rtk_of_flowCntMode_t));
    SETSOCKOPT(RTDRV_OF_FLOWCNTMODE_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowCnt_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type,
    uint64                   *pCnt)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    osal_memcpy(&cfg.flowCntType, &type, sizeof(rtk_of_flowCntType_t));
    GETSOCKOPT(RTDRV_OF_FLOWCNT_GET, &cfg, rtdrv_openflowCfg_t, 1);
    *pCnt = cfg.flowCnt;

    return RT_ERR_OK;
}

int32
rtrpc_of_flowCnt_clear(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    osal_memcpy(&cfg.flowCntType, &type, sizeof(rtk_of_flowCntType_t));
    SETSOCKOPT(RTDRV_OF_FLOWCNT_CLEAR, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowCntThresh_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   *pThreshold)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThreshold), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    GETSOCKOPT(RTDRV_OF_FLOWCNTTHRESH_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pThreshold, &cfg.threshold, sizeof(uint64));

    return RT_ERR_OK;
}

int32
rtrpc_of_flowCntThresh_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   threshold)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(rtk_of_flow_id_t));
    osal_memcpy(&cfg.threshold, &threshold, sizeof(uint64));
    SETSOCKOPT(RTDRV_OF_FLOWCNTTHRESH_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_ttlExcpt_get(uint32 unit, rtk_action_t *pAction)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_TTLEXCPT_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pAction, &cfg.action, sizeof(rtk_action_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_ttlExcpt_set(uint32 unit, rtk_action_t action)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.action, &action, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_OF_TTLEXCPT_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_maxLoopback_get(uint32 unit, uint32 *pTimes)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimes), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_MAXLOOPBACK_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pTimes, &cfg.times, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_of_maxLoopback_set(uint32 unit, uint32 times)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.times, &times, sizeof(uint32));
    SETSOCKOPT(RTDRV_OF_MAXLOOPBACK_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowTblMatchField_get(uint32 unit, rtk_of_l2FlowTblMatchField_t *pField)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_L2FLOWTBLMATCHFIELD_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pField, &cfg.l2Field, sizeof(rtk_of_l2FlowTblMatchField_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowTblMatchField_set(uint32 unit, rtk_of_l2FlowTblMatchField_t field)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.l2Field, &field, sizeof(rtk_of_l2FlowTblMatchField_t));
    SETSOCKOPT(RTDRV_OF_L2FLOWTBLMATCHFIELD_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.field_id, &field_id, sizeof(uint32));
    osal_memcpy(&cfg.setFieldType, &type, sizeof(rtk_of_setFieldType_t));
    GETSOCKOPT(RTDRV_OF_L2FLOWENTRYSETFIELD_CHECK, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowEntry_get(uint32 unit, rtk_of_l2FlowEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.l2Entry, pEntry, sizeof(rtk_of_l2FlowEntry_t));
    GETSOCKOPT(RTDRV_OF_L2FLOWENTRY_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pEntry, &cfg.l2Entry, sizeof(rtk_of_l2FlowEntry_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowEntryNextValid_get(
    uint32               unit,
    int32                *pScan_idx,
    rtk_of_l2FlowEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScan_idx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    cfg.scan_idx = *pScan_idx;
    GETSOCKOPT(RTDRV_OF_L2FLOWENTRYNEXTVALID_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pScan_idx, &cfg.scan_idx, sizeof(int32));
    osal_memcpy(pEntry, &cfg.l2Entry, sizeof(rtk_of_l2FlowEntry_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowEntry_add(uint32 unit, rtk_of_l2FlowEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.l2Entry, pEntry, sizeof(rtk_of_l2FlowEntry_t));
    SETSOCKOPT(RTDRV_OF_L2FLOWENTRY_ADD, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowEntry_del(uint32 unit, rtk_of_l2FlowEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.l2Entry, pEntry, sizeof(rtk_of_l2FlowEntry_t));
    SETSOCKOPT(RTDRV_OF_L2FLOWENTRY_DEL, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowEntry_delAll(uint32 unit)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    SETSOCKOPT(RTDRV_OF_L2FLOWENTRY_DELALL, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l2FlowEntry_t     *pEntry,
    uint32                   reset,
    uint32                   *pIsHit)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.l2Entry, pEntry, sizeof(rtk_of_l2FlowEntry_t));
    osal_memcpy(&cfg.reset, &reset, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_L2FLOWENTRYHITSTS_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pIsHit, &cfg.isHit, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAlgo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.block, &block, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_L2FLOWTBLHASHALGO_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pAlgo, &cfg.algo, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_of_l2FlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.block, &block, sizeof(uint32));
    osal_memcpy(&cfg.algo, &algo, sizeof(uint32));
    SETSOCKOPT(RTDRV_OF_L2FLOWTBLHASHALGO_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3FlowTblPri_get(uint32 unit, rtk_of_l3FlowTblList_t *pTable)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_L3FLOWTBLPRI_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pTable, &cfg.table, sizeof(rtk_of_l3FlowTblList_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_l3FlowTblPri_set(uint32 unit, rtk_of_l3FlowTblList_t table)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.table, &table, sizeof(rtk_of_l3FlowTblList_t));
    SETSOCKOPT(RTDRV_OF_L3FLOWTBLPRI_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3CamFlowTblMatchField_get(uint32 unit, rtk_of_l3CamFlowTblMatchField_t *pField)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_L3CAMFLOWTBLMATCHFIELD_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pField, &cfg.l3CamField, sizeof(rtk_of_l3CamFlowTblMatchField_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_l3CamFlowTblMatchField_set(uint32 unit, rtk_of_l3CamFlowTblMatchField_t field)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.l3CamField, &field, sizeof(rtk_of_l3CamFlowTblMatchField_t));
    SETSOCKOPT(RTDRV_OF_L3CAMFLOWTBLMATCHFIELD_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3HashFlowTblMatchField_get(uint32 unit, rtk_of_l3HashFlowTblMatchField_t *pField)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_L3HASHFLOWTBLMATCHFIELD_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pField, &cfg.l3HashField, sizeof(rtk_of_l3HashFlowTblMatchField_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_l3HashFlowTblMatchField_set(uint32 unit, rtk_of_l3HashFlowTblMatchField_t field)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.l3HashField, &field, sizeof(rtk_of_l3HashFlowTblMatchField_t));
    SETSOCKOPT(RTDRV_OF_L3HASHFLOWTBLMATCHFIELD_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3HashFlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAlgo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.block, &block, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_L3HASHFLOWTBLHASHALGO_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pAlgo, &cfg.algo, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_of_l3HashFlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.block, &block, sizeof(uint32));
    osal_memcpy(&cfg.algo, &algo, sizeof(uint32));
    SETSOCKOPT(RTDRV_OF_L3HASHFLOWTBLHASHALGO_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.field_id, &field_id, sizeof(uint32));
    osal_memcpy(&cfg.setFieldType, &type, sizeof(rtk_of_setFieldType_t));
    GETSOCKOPT(RTDRV_OF_L3FLOWENTRYSETFIELD_CHECK, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3CamFlowEntry_get(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_L3CAMFLOWENTRY_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pEntry, &cfg.l3CamEntry, sizeof(rtk_of_l3CamFlowEntry_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_l3CamFlowEntry_add(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(uint32));
    osal_memcpy(&cfg.l3CamEntry, pEntry, sizeof(rtk_of_l3CamFlowEntry_t));
    SETSOCKOPT(RTDRV_OF_L3CAMFLOWENTRY_ADD, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3CamFlowEntry_del(uint32 unit, uint32 entry_idx)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    cfg.idx = entry_idx;
    SETSOCKOPT(RTDRV_OF_L3CAMFLOWENTRY_DEL, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3CamFlowEntry_move(uint32 unit, rtk_of_flowMove_t *pMoveIdx)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMoveIdx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.moveIdx, pMoveIdx, sizeof(rtk_of_flowMove_t));
    SETSOCKOPT(RTDRV_OF_L3CAMFLOWENTRY_MOVE, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3CamflowEntryHitSts_get(
    uint32    unit,
    uint32    entry_idx,
    uint32    reset,
    uint32    *pIsHit)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(uint32));
    osal_memcpy(&cfg.reset, &reset, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_L3CAMFLOWENTRYHITSTS_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pIsHit, &cfg.isHit, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_of_l3HashFlowEntry_get(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.l3HashEntry, pEntry, sizeof(rtk_of_l3HashFlowEntry_t));
    GETSOCKOPT(RTDRV_OF_L3HASHFLOWENTRY_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pEntry, &cfg.l3HashEntry, sizeof(rtk_of_l3HashFlowEntry_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_l3HashFlowEntryNextValid_get(
    uint32                   unit,
    int32                    *pScan_idx,
    rtk_of_l3HashFlowEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScan_idx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    cfg.scan_idx = *pScan_idx;
    GETSOCKOPT(RTDRV_OF_L3HASHFLOWENTRYNEXTVALID_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pScan_idx, &cfg.scan_idx, sizeof(int32));
    osal_memcpy(pEntry, &cfg.l3HashEntry, sizeof(rtk_of_l3HashFlowEntry_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_l3HashFlowEntry_add(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.l3HashEntry, pEntry, sizeof(rtk_of_l3HashFlowEntry_t));
    SETSOCKOPT(RTDRV_OF_L3HASHFLOWENTRY_ADD, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3HashFlowEntry_del(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.l3HashEntry, pEntry, sizeof(rtk_of_l3HashFlowEntry_t));
    SETSOCKOPT(RTDRV_OF_L3HASHFLOWENTRY_DEL, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3HashFlowEntry_delAll(uint32 unit)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    SETSOCKOPT(RTDRV_OF_L3HASHFLOWENTRY_DELALL, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_l3HashflowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l3HashFlowEntry_t *pEntry,
    uint32                   reset,
    uint32                   *pIsHit)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.reset, &reset, sizeof(uint32));
    osal_memcpy(pEntry, &cfg.l3HashEntry, sizeof(rtk_of_l3HashFlowEntry_t));
    GETSOCKOPT(RTDRV_OF_L3HASHFLOWENTRYHITSTS_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pIsHit, &cfg.isHit, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_of_groupEntry_get(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_GROUPENTRY_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pEntry, &cfg.grpEntry, sizeof(rtk_of_groupEntry_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_groupEntry_set(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(uint32));
    osal_memcpy(&cfg.grpEntry, pEntry, sizeof(rtk_of_groupEntry_t));
    SETSOCKOPT(RTDRV_OF_GROUPENTRY_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_groupTblHashPara_get(uint32 unit, rtk_of_groupTblHashPara_t *para)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == para), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_GROUPTBLHASHPARA_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(para, &cfg.para, sizeof(rtk_of_groupTblHashPara_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_groupTblHashPara_set(uint32 unit, rtk_of_groupTblHashPara_t *para)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == para), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.para, para, sizeof(rtk_of_groupTblHashPara_t));
    SETSOCKOPT(RTDRV_OF_GROUPTBLHASHPARA_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_actionBucket_get(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry)
{
    rtdrv_openflowABCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowABCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_ACTIONBUCKET_GET, &cfg, rtdrv_openflowABCfg_t, 1);
    osal_memcpy(pEntry, &cfg.actionBktEntry, sizeof(rtk_of_actionBucket_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_actionBucket_set(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry)
{
    rtdrv_openflowABCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowABCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(uint32));
    osal_memcpy(&cfg.actionBktEntry, pEntry, sizeof(rtk_of_actionBucket_t));
    SETSOCKOPT(RTDRV_OF_ACTIONBUCKET_SET, &cfg, rtdrv_openflowABCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_OF_TRAPTARGET_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pTarget, &cfg.target, sizeof(rtk_trapTarget_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_trapTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.target, &target, sizeof(rtk_trapTarget_t));
    SETSOCKOPT(RTDRV_OF_TRAPTARGET_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_tblMissAction_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t *pAct)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAct), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    GETSOCKOPT(RTDRV_OF_TBLMISSACTION_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pAct, &cfg.tblMissAct, sizeof(rtk_of_tblMissAct_t));

    return RT_ERR_OK;
}

int32
rtrpc_of_tblMissAction_set(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t act)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.tblMissAct, &act, sizeof(rtk_of_tblMissAct_t));
    SETSOCKOPT(RTDRV_OF_TBLMISSACTION_SET, &cfg, rtdrv_openflowCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_of_flowTblCnt_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowTblCntType_t type, uint32 *pCnt)
{
    rtdrv_openflowCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_openflowCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.phase, &phase, sizeof(rtk_of_flowtable_phase_t));
    osal_memcpy(&cfg.flowtblCntType, &type, sizeof(rtk_of_flowTblCntType_t));
    GETSOCKOPT(RTDRV_OF_FLOWTBLCNT_GET, &cfg, rtdrv_openflowCfg_t, 1);
    osal_memcpy(pCnt, &cfg.tblCnt, sizeof(uint32));

    return RT_ERR_OK;
}

int32
rtrpc_ofMapper_init(dal_mapper_t *pMapper)
{
    pMapper->of_classifier_get = rtrpc_of_classifier_get;
    pMapper->of_classifier_set = rtrpc_of_classifier_set;
    pMapper->of_flowMatchFieldSize_get = rtrpc_of_flowMatchFieldSize_get;
    pMapper->of_flowEntrySize_get = rtrpc_of_flowEntrySize_get;
    pMapper->of_flowEntryValidate_get = rtrpc_of_flowEntryValidate_get;
    pMapper->of_flowEntryValidate_set = rtrpc_of_flowEntryValidate_set;
    pMapper->of_flowEntryFieldList_get = rtrpc_of_flowEntryFieldList_get;
    pMapper->of_flowEntryField_check = rtrpc_of_flowEntryField_check;
    pMapper->of_flowEntrySetField_check = rtrpc_of_flowEntrySetField_check;
    pMapper->of_flowEntryField_read = rtrpc_of_flowEntryField_read;
    pMapper->of_flowEntryField_write = rtrpc_of_flowEntryField_write;
    pMapper->of_flowEntryOperation_get = rtrpc_of_flowEntryOperation_get;
    pMapper->of_flowEntryOperation_set = rtrpc_of_flowEntryOperation_set;
    pMapper->of_flowEntryInstruction_get = rtrpc_of_flowEntryInstruction_get;
    pMapper->of_flowEntryInstruction_set = rtrpc_of_flowEntryInstruction_set;
    pMapper->of_flowEntryHitSts_get = rtrpc_of_flowEntryHitSts_get;
    pMapper->of_flowEntry_del = rtrpc_of_flowEntry_del;
    pMapper->of_flowEntry_move = rtrpc_of_flowEntry_move;
    pMapper->of_ftTemplateSelector_get = rtrpc_of_ftTemplateSelector_get;
    pMapper->of_ftTemplateSelector_set = rtrpc_of_ftTemplateSelector_set;
    pMapper->of_flowCntMode_get = rtrpc_of_flowCntMode_get;
    pMapper->of_flowCntMode_set = rtrpc_of_flowCntMode_set;
    pMapper->of_flowCnt_get = rtrpc_of_flowCnt_get;
    pMapper->of_flowCnt_clear = rtrpc_of_flowCnt_clear;
    pMapper->of_flowCntThresh_get = rtrpc_of_flowCntThresh_get;
    pMapper->of_flowCntThresh_set = rtrpc_of_flowCntThresh_set;
    pMapper->of_ttlExcpt_get = rtrpc_of_ttlExcpt_get;
    pMapper->of_ttlExcpt_set = rtrpc_of_ttlExcpt_set;
    pMapper->of_maxLoopback_get = rtrpc_of_maxLoopback_get;
    pMapper->of_maxLoopback_set = rtrpc_of_maxLoopback_set;
    pMapper->of_l2FlowTblMatchField_get = rtrpc_of_l2FlowTblMatchField_get;
    pMapper->of_l2FlowTblMatchField_set = rtrpc_of_l2FlowTblMatchField_set;
    pMapper->of_l2FlowEntrySetField_check = rtrpc_of_l2FlowEntrySetField_check;
    pMapper->of_l2FlowEntry_get = rtrpc_of_l2FlowEntry_get;
    pMapper->of_l2FlowEntryNextValid_get = rtrpc_of_l2FlowEntryNextValid_get;
    pMapper->of_l2FlowEntry_add = rtrpc_of_l2FlowEntry_add;
    pMapper->of_l2FlowEntry_del = rtrpc_of_l2FlowEntry_del;
    pMapper->of_l2FlowEntry_delAll = rtrpc_of_l2FlowEntry_delAll;
    pMapper->of_l2FlowEntryHitSts_get = rtrpc_of_l2FlowEntryHitSts_get;
    pMapper->of_l2FlowTblHashAlgo_get = rtrpc_of_l2FlowTblHashAlgo_get;
    pMapper->of_l2FlowTblHashAlgo_set = rtrpc_of_l2FlowTblHashAlgo_set;
    pMapper->of_l3FlowTblPri_get = rtrpc_of_l3FlowTblPri_get;
    pMapper->of_l3FlowTblPri_set = rtrpc_of_l3FlowTblPri_set;
    pMapper->of_l3CamFlowTblMatchField_get = rtrpc_of_l3CamFlowTblMatchField_get;
    pMapper->of_l3CamFlowTblMatchField_set = rtrpc_of_l3CamFlowTblMatchField_set;
    pMapper->of_l3HashFlowTblMatchField_get = rtrpc_of_l3HashFlowTblMatchField_get;
    pMapper->of_l3HashFlowTblMatchField_set = rtrpc_of_l3HashFlowTblMatchField_set;
    pMapper->of_l3HashFlowTblHashAlgo_get = rtrpc_of_l3HashFlowTblHashAlgo_get;
    pMapper->of_l3HashFlowTblHashAlgo_set = rtrpc_of_l3HashFlowTblHashAlgo_set;
    pMapper->of_l3FlowEntrySetField_check = rtrpc_of_l3FlowEntrySetField_check;
    pMapper->of_l3CamFlowEntry_get = rtrpc_of_l3CamFlowEntry_get;
    pMapper->of_l3CamFlowEntry_add = rtrpc_of_l3CamFlowEntry_add;
    pMapper->of_l3CamFlowEntry_del = rtrpc_of_l3CamFlowEntry_del;
    pMapper->of_l3CamFlowEntry_move = rtrpc_of_l3CamFlowEntry_move;
    pMapper->of_l3CamflowEntryHitSts_get = rtrpc_of_l3CamflowEntryHitSts_get;
    pMapper->of_l3HashFlowEntry_get = rtrpc_of_l3HashFlowEntry_get;
    pMapper->of_l3HashFlowEntryNextValid_get = rtrpc_of_l3HashFlowEntryNextValid_get;
    pMapper->of_l3HashFlowEntry_add = rtrpc_of_l3HashFlowEntry_add;
    pMapper->of_l3HashFlowEntry_del = rtrpc_of_l3HashFlowEntry_del;
    pMapper->of_l3HashFlowEntry_delAll = rtrpc_of_l3HashFlowEntry_delAll;
    pMapper->of_l3HashflowEntryHitSts_get = rtrpc_of_l3HashflowEntryHitSts_get;
    pMapper->of_groupEntry_get = rtrpc_of_groupEntry_get;
    pMapper->of_groupEntry_set = rtrpc_of_groupEntry_set;
    pMapper->of_groupTblHashPara_get = rtrpc_of_groupTblHashPara_get;
    pMapper->of_groupTblHashPara_set = rtrpc_of_groupTblHashPara_set;
    pMapper->of_actionBucket_get = rtrpc_of_actionBucket_get;
    pMapper->of_actionBucket_set = rtrpc_of_actionBucket_set;
    pMapper->of_trapTarget_get = rtrpc_of_trapTarget_get;
    pMapper->of_trapTarget_set = rtrpc_of_trapTarget_set;
    pMapper->of_tblMissAction_get = rtrpc_of_tblMissAction_get;
    pMapper->of_tblMissAction_set = rtrpc_of_tblMissAction_set;
    pMapper->of_flowTblCnt_get = rtrpc_of_flowTblCnt_get;
    return RT_ERR_OK;
}
