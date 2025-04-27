/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public OAM & CFM APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *              1) PIE
 */

#ifndef __DAL_MANGO_PIE_H__
#define __DAL_MANGO_PIE_H__

/*
 * Include Files
 */
#include <rtk/pie.h>
#include <dal/dal_mapper.h>


/*
 * Symbol Definition
 */
#define DAL_MANGO_DATA_BITS                     8
#define DAL_MANGO_FILED_BITS                    16
#define DAL_MANGO_BUFFER_UNIT_LENGTH_BITS       32

#define DAL_MANGO_MAX_INFO_IDX                \
    (DAL_MANGO_BUFFER_UNIT_LENGTH_BITS / DAL_MANGO_DATA_BITS)


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */
#define DAL_MANGO_DATA_WIDTH_GET(_data_len)                   \
    (((_data_len) + DAL_MANGO_DATA_BITS - 1) / DAL_MANGO_DATA_BITS)

#define DAL_MANGO_GET_BYTE_IDX(_offset)                       \
    ((_offset) / DAL_MANGO_DATA_BITS)

#define DAL_MANGO_GET_INFO_IDX(_size, _offset)                \
    (DAL_MANGO_DATA_WIDTH_GET((_size)) - DAL_MANGO_DATA_WIDTH_GET((_offset)))

#define DAL_MANGO_GET_INFO_OFFSET(_max, _idx)                       \
    ((((_idx) - ((_max) % DAL_MANGO_MAX_INFO_IDX)) % DAL_MANGO_MAX_INFO_IDX) * DAL_MANGO_DATA_BITS)

#define DAL_MANGO_PIE_INDEX_CHK(_u, _t, _idx)    \
do {\
    rtk_pie_phase_t _phase;\
    RT_PARAM_CHK((_idx >= HAL_MAX_NUM_OF_PIE_FILTER_ID(_u)), RT_ERR_ENTRY_INDEX);\
    dal_mango_pie_phase_get(_u, _idx / HAL_MAX_NUM_OF_PIE_BLOCKSIZE(_u), &_phase);\
    if (_t != _phase)\
        return RT_ERR_ENTRY_INDEX;\
} while(0)

/* translate entry index from logical(phase) to physical */
#define DAL_MANGO_PIE_INDEX_TO_PHYSICAL(_u, _type, _logicIdx, _phyIdx)    \
do {\
    RT_PARAM_CHK((_logicIdx >= HAL_MAX_NUM_OF_PIE_FILTER_ID(_u)), RT_ERR_ENTRY_INDEX);\
    _dal_mango_pie_entryIdx_trans(_u, _type, _logicIdx, &_phyIdx);\
} while(0)


/*
 * Function Declaration
 */

/*
 * Get PIE entry hit status.
 */
extern int32
_dal_mango_pie_entryHitSts_get(
    uint32                   unit,
    rtk_pie_id_t             entry_idx,
    uint32                   reset,
    uint32                   *pIsHit);

/*
 * Delete PIE entries by PIE phase and logical index.
 */
extern int32
_dal_mango_pie_entry_del(
    uint32          unit,
    rtk_pie_phase_t phase,
    uint32          start_idx,
    uint32          end_idx);

/*
 * Move PIE entries by PIE phase and logical index.
 */
extern int32
_dal_mango_pie_entry_move(
    uint32          unit,
    rtk_pie_phase_t phase,
    uint32          length,
    uint32          move_from,
    uint32          move_to);

/*
 * Get the mapping template of specific PIE block.
 */
extern int32
_dal_mango_pie_templateSelector_get(
    uint32 unit,
    uint32 block_idx,
    uint32 *template0_id,
    uint32 *template1_id);

/*
 * Set the mapping template of specific PIE block.
 */
int32
_dal_mango_pie_templateSelector_set(
    uint32 unit,
    uint32 block_idx,
    uint32 template0_id,
    uint32 template1_id);

/*
 * Translate logical block index to physical block index.
 */
int32
_dal_mango_pie_blockIdx_trans(uint32 unit, rtk_pie_phase_t type,
    uint32 blk_idx, uint32 *phy_blk_idx);

/*
 * Translate logical entry index to physical entry index.
 */
int32
_dal_mango_pie_entryIdx_trans(uint32 unit, rtk_pie_phase_t type,
    uint32 entry_idx, uint32 *phy_entry_idx);



/* Function Name:
 *      dal_mango_pie_phase_get
 * Description:
 *      Get the PIE block phase.
 * Input:
 *      unit       - unit id
 *      block_idx - block index
 * Output:
 *      pPhase - pointer buffer of phase value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_phase_get(uint32 unit, uint32 block_idx, rtk_pie_phase_t *pPhase);

/* Function Name:
 *      dal_mango_pie_phase_set
 * Description:
 *      Set the PIE block phase configuration.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      phase       - phase value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 * Note:
 *      None
 */
extern int32
dal_mango_pie_phase_set(uint32 unit, uint32 block_idx, rtk_pie_phase_t phase);

/* Function Name:
 *      dal_mango_pie_blockLookupEnable_get
 * Description:
 *      Get the PIE block lookup state.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - block index is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      1.The rule data are kept regardless of the lookup status.
 *      2.The lookup result is always false if the lookup state is disabled.
 */
extern int32
dal_mango_pie_blockLookupEnable_get(uint32 unit, uint32 block_idx,
    rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_pie_blockLookupEnable_set
 * Description:
 *      Set the PIE block lookup state.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      enable      - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PIE_BLOCK_INDEX  - block index is out of range
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      1.The rule data are kept regardless of the lookup status.
 *      2.The lookup result is always false if the lookup state is disabled.
 */
extern int32
dal_mango_pie_blockLookupEnable_set(uint32 unit, uint32 block_idx,
    rtk_enable_t enable);

/* Function Name:
 *      dal_mango_pie_blockGrouping_get
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit       - unit id
 *      block_idx  - block index
 * Output:
 *      group_id    - block group index
 *      logic_id    - block logic index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group blocks which belong to different phase is forbidden.
 *      (3) Group id > logic id > physical block id
 */
extern int32
dal_mango_pie_blockGrouping_get(uint32 unit, uint32 block_idx,
    uint32 *group_id, uint32 *logic_id);

/* Function Name:
 *      dal_mango_pie_blockGrouping_set
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit       - unit id
 *      block_idx  - block index
 *      group_id   - block group index
 *      logic_id   - block logic index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group blocks which belong to different phase is forbidden.
 *      (3) Group id > logic id > physical block id
 */
extern int32
dal_mango_pie_blockGrouping_set( uint32 unit, uint32 block_idx,
    uint32 group_id, uint32 logic_id);

/* Function Name:
 *      dal_mango_pie_template_get
 * Description:
 *      Get the template content of specific template index.
 * Input:
 *      unit        - unit id
 *      template_id - template ID
 * Output:
 *      pTemplate   - template content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_pie_template_get(uint32 unit, uint32 template_id,
    rtk_pie_template_t *pTemplate);

/* Function Name:
 *      dal_mango_pie_template_set
 * Description:
 *      Set the template content of specific template index.
 * Input:
 *      unit         - unit id
 *      template_id  - template ID
 *      pTemplate    - template content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_template_set(uint32 unit, uint32 template_id,
    rtk_pie_template_t *pTemplate);

/* Function Name:
 *      dal_mango_pie_phaseTemplate_get
 * Description:
 *      Get the template content of specific template index and phase.
 * Input:
 *      unit        - unit id
 *      templateId  - template ID
 * Output:
 *      pTemplate   - template content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_phaseTemplate_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 templateId, rtk_pie_template_t *pTemplate);

/* Function Name:
 *      dal_mango_pie_templateField_check
 * Description:
 *      Check whether the specified template field type is supported on the chip.
 * Input:
 *      unit  - unit id
 *      phase - PIE lookup phase
 *      type  - template field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT                     - The module is not initial
 *      RT_ERR_PIE_PHASE                    - invalid PIE phase
 *      RT_ERR_PIE_FIELD_TYPE               - invalid PIE field type
 * Note:
 *      None
 */
extern int32
dal_mango_pie_templateField_check(uint32 unit, rtk_pie_phase_t phase,
    rtk_pie_templateFieldType_t type);

/* Function Name:
 *      dal_mango_pie_templateVlanSel_get
 * Description:
 *      Get the configuration of VLAN select in pre-defined template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      preTemplate_idx     - pre-defined template index
 * Output:
 *      pVlanSel - pointer to VLAN select in pre-defined template.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      (1) The API does not support pre-defined template3.
 *      (2) Egress PIE has one more option(SRC_FVLAN) than Ingress PIE.
 */
extern int32
dal_mango_pie_templateVlanSel_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 preTemplate_idx, rtk_pie_templateVlanSel_t *pVlanSel);

/* Function Name:
 *      dal_mango_pie_templateVlanSel_set
 * Description:
 *      set the configuration of VLAN select in pre-defined template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      preTemplate_idx     - pre-defined template index
 *      vlanSel - VLAN select in pre-defined template.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      (1) The API does not support pre-defined template3.
 *      (2) Egress PIE has one more option(SRC_FVLAN) than Ingress PIE.
 */
extern int32
dal_mango_pie_templateVlanSel_set(uint32 unit, rtk_pie_phase_t phase,
    uint32 preTemplate_idx, rtk_pie_templateVlanSel_t vlanSel);

/* Function Name:
 *      dal_mango_pie_rangeCheckIp_get
 * Description:
 *      Get the configuration of IP range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of IP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1.For IPv6 range check, index 0/4 means IP6[31:0], index 1/5 means IP6[63:32],
 *        index 2/6 means IP6[95:64], index 3/7 means IP6[127:96]. Index 0~3/4~7 must
 *        be used together in order to filter a full IPv6 address.
 *      2.For IPv6 suffix range check, index 0/2/4/6 means IP6[31:0], index 1/3/5/7 means IP6[63:32],
 *        Index 0&1/2&3/4&5/6&7 must be used together in order to filter a IPv6 suffix address.
 */
extern int32
dal_mango_pie_rangeCheckIp_get(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_ip_t *pData);

/* Function Name:
 *      dal_mango_pie_rangeCheckIp_set
 * Description:
 *      Set the configuration of IP range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of IP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1.For IPv6 range check, index 0/4 represents IP6[31:0], index 1/5 represents IP6[63:32],
 *        index 2/6 represents IP6[95:64], index 3/7 represents IP6[127:96]. Index 0~3/4~7 must
 *        be used together in order to filter a full IPv6 address.
 *      2.For IPv6 suffix range check, index 0/2/4/6 represents IP6[31:0], index 1/3/5/7 represents IP6[63:32].
 *        Index 0&1/2&3/4&5/6&7 must be used together in order to filter a IPv6 suffix address.
 */
extern int32
dal_mango_pie_rangeCheckIp_set(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_ip_t *pData);

/* Function Name:
 *      dal_mango_pie_rangeCheck_get
 * Description:
 *      Get the configuration of range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of range check
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_rangeCheck_get(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData);

/* Function Name:
 *      dal_mango_pie_rangeCheck_set
 * Description:
 *      Set the configuration of range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of range check
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_pie_rangeCheck_set(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData);

/* Function Name:
 *      dal_mango_pie_fieldSelector_get
 * Description:
 *      Get the configuration of field selector.
 * Input:
 *      unit   - unit id
 *      fs_idx - field selector index
 * Output:
 *      pFs    - configuration of field selector.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_fieldSelector_get(uint32 unit, uint32 fs_idx,
    rtk_pie_fieldSelector_data_t *pFs);

/* Function Name:
 *      dal_mango_pie_fieldSelector_set
 * Description:
 *      Set the configuration of field selector.
 * Input:
 *      unit   - unit id
 *      fs_idx - field selector index
 *      pFs    - configuration of field selector.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_pie_fieldSelector_set(uint32 unit, uint32 fs_idx,
    rtk_pie_fieldSelector_data_t *pFs);

/* Function Name:
 *      dal_mango_pie_meterIncludeIfg_get
 * Description:
 *      Get enable status of includes IFG for meter.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - pointer to enable status of includes IFG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_meterIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

/* Function Name:
 *      dal_mango_pie_meterIncludeIfg_set
 * Description:
 *      Set enable status of includes IFG for meter.
 * Input:
 *      unit        - unit id
 *      ifg_include - enable status of includes IFG
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_pie_meterIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);

/* Function Name:
 *      dal_mango_pie_meterMode_get
 * Description:
 *      Get the meter mode of a specific meter block.
 * Input:
 *      unit        - unit id
 *      blockIdx    - meter block ID
 * Output:
 *      pMeterMode  - meter mode:byte based or packet based
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_meterMode_get(uint32 unit, uint32 blockIdx,
    rtk_pie_meterMode_t *pMeterMode);

/* Function Name:
 *      dal_mango_pie_meterMode_set
 * Description:
 *      Set the meter mode.
 * Input:
 *      unit        - unit id
 *      blockIdx    - meter block ID
 *      meterMode   - meter mode (byte based or packet based)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_pie_meterMode_set(uint32 unit, uint32 blockIdx,
    rtk_pie_meterMode_t meterMode);

/* Function Name:
 *      dal_mango_pie_meterExceed_get
 * Description:
 *      Get the meter exceed flag of a meter entry.
 * Input:
 *      unit        - unit id
 *      meterIdx    - meter entry index
 * Output:
 *      pIsExceed   - pointer to exceed flag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX         - invalid entry index
 * Note:
 *      None
 */
extern int32
dal_mango_pie_meterExceed_get(uint32 unit, uint32 meterIdx, uint32 *pIsExceed);

/* Function Name:
 *      dal_mango_pie_meterExceedAggregation_get
 * Description:
 *      Get the meter exceed flag mask of meter entry exceed aggregated result every 16 entries.
 * Input:
 *      unit      - unit id
 * Output:
 *      pExceedMask - pointer to aggregated exceed flag mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_meterExceedAggregation_get(uint32 unit, uint32 *pExceedMask);

/* Function Name:
 *      dal_mango_pie_meterEntry_get
 * Description:
 *      Get the content of a meter entry.
 * Input:
 *      unit        - unit id
 *      meterIdx    - meter entry index
 * Output:
 *      pMeterEntry - pointer to a meter entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_mango_pie_meterEntry_get(uint32 unit, uint32 meterIdx,
    rtk_pie_meterEntry_t *pMeterEntry);

/* Function Name:
 *      dal_mango_pie_meterEntry_set
 * Description:
 *      Set a meter entry.
 * Input:
 *      unit        - unit id
 *      meterIdx    - meter entry index
 *      pMeterEntry - pointer to meter entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_mango_pie_meterEntry_set(uint32 unit, uint32 meterIdx,
    rtk_pie_meterEntry_t *pMeterEntry);

/* Function Name:
 *      dal_mango_pie_meterDpSel_get
 * Description:
 *      Get the configuration of DP select.
 * Input:
 *      unit        - unit id
 * Output:
 *      pDpSel      - pointer to DP select
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_meterDpSel_get(uint32 unit, rtk_pie_meterDpSel_t *pDpSel);

/* Function Name:
 *      dal_mango_pie_meterDpSel_set
 * Description:
 *      Set the configuration of DP select.
 * Input:
 *      unit        - unit id
 *      dpSel       - DP select
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_meterDpSel_set(uint32 unit, rtk_pie_meterDpSel_t dpSel);

/* Function Name:
 *      dal_mango_pie_arpMacSel_get
 * Description:
 *      Get the configuration of ARP MAC address select.
 * Input:
 *      unit        - unit id
 * Output:
 *      pArpMacSel  - pointer to ARP MAC select
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_arpMacSel_get(uint32 unit, rtk_pie_arpMacSel_t *pArpMacSel);

/* Function Name:
 *      dal_mango_pie_arpMacSel_set
 * Description:
 *      Set the configuration of ARP MAC address select.
 * Input:
 *      unit        - unit id
 *      arpMacSel   - ARP MAC select
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_arpMacSel_set(uint32 unit, rtk_pie_arpMacSel_t arpMacSel);

/* Function Name:
 *      dal_mango_pie_intfSel_get
 * Description:
 *      Get the configuration of filter interface type.
 * Input:
 *      unit       - unit id
 * Output:
 *      intfSel    - select inteface filter type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_intfSel_get(uint32 unit, rtk_pie_intfSel_t *intfSel);

/* Function Name:
 *      dal_mango_pie_intfSel_set
 * Description:
 *      Set the configuration of filter interface type.
 * Input:
 *      unit       - unit id
 *      intfSel    - select inteface filter type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_intfSel_set(uint32 unit, rtk_pie_intfSel_t intfSel);

/* Function Name:
 *      dal_mango_pie_templateVlanFmtSel_get
 * Description:
 *      Get the configuration of otag/itag VLAN format select in template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      template_idx        - pre-defined template index
 * Output:
 *      pVlanFmtSel         - pointer to VLAN format select in template.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      The API onlye support ingress PIE.
 */
extern int32
dal_mango_pie_templateVlanFmtSel_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t *pVlanFmtSel);

/* Function Name:
 *      dal_mango_pie_templateVlanFmtSel_set
 * Description:
 *      Set the configuration of otag/itag VLAN format select in template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      template_idx        - pre-defined template index
 *      vlanFmtSel          - VLAN format select in template.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      The API onlye support ingress PIE.
 */
extern int32
dal_mango_pie_templateVlanFmtSel_set(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t vlanFmtSel);

/* Function Name:
 *      dal_mango_pie_meterTrtcmType_get
 * Description:
 *      Get a meter TrTCM type.
 * Input:
 *      unit        - unit id
 * Output:
 *      type        - meter trTCM type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None.
 */
extern int32
dal_mango_pie_meterTrtcmType_get(uint32 unit, rtk_pie_meterTrtcmType_t *type);

/* Function Name:
 *      dal_mango_pie_meterTrtcmType_set
 * Description:
 *      Set a meter trTCM type.
 * Input:
 *      unit        - unit id
 *      type        - meter TrTCM type
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None.
 */
extern int32
dal_mango_pie_meterTrtcmType_set(uint32 unit, rtk_pie_meterTrtcmType_t type);

/* Function Name:
 *      dal_mango_pieMapper_init
 * Description:
 *      Hook pie module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook pie module before calling any pie APIs.
 */
extern int32
dal_mango_pieMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_pie_init
 * Description:
 *      Initialize PIE module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize PIE module before calling any PIE APIs.
 */
extern int32
dal_mango_pie_init(uint32 unit);

/* Function Name:
 *      dal_mango_pie_filter1BR_get
 * Description:
 *      Get filter 802.1BR status.
 * Input:
 *      unit                - unit id
 * Output:
 *      en    - filter 802.1BR status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_filter1BR_get(uint32 unit, rtk_enable_t *en);

/* Function Name:
 *      dal_mango_pie_filter1BR_set
 * Description:
 *      Set filter 802.1BR status.
 * Input:
 *      unit  - unit id
 *      en    - filter 802.1BR status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_pie_filter1BR_set(uint32 unit, rtk_enable_t en);

#endif  /* __DAL_MANGO_PIE_H__ */
