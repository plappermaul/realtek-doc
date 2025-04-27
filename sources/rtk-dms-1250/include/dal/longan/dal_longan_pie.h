/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public PIE APIs and its data type in the SDK .
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Ingress ACL
 *            2) Egress ACL
 *            3) Field Selector
 *            4) Range Check
 *            5) Meter
 *            6) Counter
 *
 */

#ifndef __DAL_LONGAN_PIE_H__
#define __DAL_LONGAN_PIE_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/pie.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef enum dal_longan_pie_blkType_e
{
    PIE_BLK_TYPE_IGR_ACL,
    PIE_BLK_TYPE_EGR_ACL,
    PIE_BLK_TYPE_END
} dal_longan_pie_blkType_t;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */
extern int32
dal_longan_pie_blockIdx_trans(uint32 unit, rtk_pie_phase_t type, uint32 blk_idx, uint32 *phy_blk_idx);

extern int32
dal_longan_pie_entryIdx_trans(uint32 unit, rtk_pie_phase_t type, uint32 entry_idx, uint32 *phy_entry_idx);

extern int32
dal_longan_pie_index_to_physical(uint32 _u, rtk_pie_phase_t _type, uint32 _logicIdx, uint32 *_phyIdx);

extern int32
dal_longan_pie_physical_index_to_logic(uint32 unit, uint32 phyIdx, rtk_pie_phase_t *phase, uint32 *logicIdx);

extern int32
dal_longan_pie_index_chk(uint32 _u, rtk_pie_phase_t _t, uint32 _idx);

extern int32
dal_longan_pie_templateSelector_get(uint32 unit, uint32 block_idx, uint32 *template0_id, uint32 *template1_id);

extern int32
dal_longan_pie_templateSelector_set(uint32 unit, uint32 block_idx, uint32 template0_id, uint32 template1_id);

/* Function Name:
 *      dal_longan_pieMapper_init
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
dal_longan_pieMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_longan_pie_init
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
dal_longan_pie_init(uint32 unit);

/* Function Name:
 *      dal_longan_pie_phase_get
 * Description:
 *      Get the pie block phase.
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
 *      Each pie block can be configured to ingress or egress or post(only for 931x).
 */
extern int32
dal_longan_pie_phase_get(uint32 unit, uint32 block_idx, rtk_pie_phase_t *pPhase);

/* Function Name:
 *      dal_longan_pie_phase_set
 * Description:
 *      Set the pie block phase configuration.
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
 *      Each pie block can be configured to ingress or egress or post(only for 931x).
 */
extern int32
dal_longan_pie_phase_set(uint32 unit, uint32 block_idx, rtk_pie_phase_t phase);

/* Function Name:
 *      dal_longan_pie_blockLookupEnable_get
 * Description:
 *      Get the pie block lookup state.
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
dal_longan_pie_blockLookupEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_pie_blockLookupEnable_set
 * Description:
 *      Set the pie block lookup state.
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
dal_longan_pie_blockLookupEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_pie_blockGrouping_get
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
dal_longan_pie_blockGrouping_get(
    uint32                     unit,
    uint32                     block_idx,
    uint32                     *group_id,
    uint32                     *logic_id);

/* Function Name:
 *      dal_longan_pie_blockGrouping_set
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
dal_longan_pie_blockGrouping_set(
    uint32                     unit,
    uint32                     block_idx,
    uint32                     group_id,
    uint32                     logic_id);

/* Function Name:
 *      dal_longan_pie_template_get
 * Description:
 *      Get the template content of specific template index.
 * Input:
 *      unit         - unit id
 *      template_id  - template ID
 * Output:
 *      pTemplate    - template content
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
dal_longan_pie_template_get(uint32 unit, uint32 template_id,
    rtk_pie_template_t *pTemplate);

/* Function Name:
 *      dal_longan_pie_template_set
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
dal_longan_pie_template_set(uint32 unit, uint32 template_id, rtk_pie_template_t *pTemplate);

/* Function Name:
 *      dal_longan_pie_templateField_check
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
dal_longan_pie_templateField_check(uint32 unit, rtk_pie_phase_t phase,
    rtk_pie_templateFieldType_t type);

/* Function Name:
 *      dal_longan_pie_rangeCheckIp_get
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
dal_longan_pie_rangeCheckIp_get(uint32 unit, uint32 index, rtk_pie_rangeCheck_ip_t *pData);

/* Function Name:
 *      dal_longan_pie_rangeCheckIp_set
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
dal_longan_pie_rangeCheckIp_set(uint32 unit, uint32 index, rtk_pie_rangeCheck_ip_t *pData);

/* Function Name:
 *      dal_longan_pie_rangeCheck_get
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
dal_longan_pie_rangeCheck_get(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData);

/* Function Name:
 *      dal_longan_pie_rangeCheck_set
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
dal_longan_pie_rangeCheck_set(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData);

/* Function Name:
 *      dal_longan_pie_fieldSelector_get
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
 *      For 8390, only the first 180B(since DA) of packet can be inspected by field selector.
 *      Thus, (start position + offest) must less than 164B(since DA) in order to grab a
 *      complete 16-bit user define field.
 */
extern int32
dal_longan_pie_fieldSelector_get(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_pie_fieldSelector_data_t *pFs);

/* Function Name:
 *      dal_longan_pie_fieldSelector_set
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
 *      For 8390, only the first 180B(since DA) of packet can be inspected by field selector.
 *      Thus, (start position + offest) must less than 164B(since DA) in order to grab a
 *      complete 16-bit user define field.
 */
extern int32
dal_longan_pie_fieldSelector_set(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_pie_fieldSelector_data_t *pFs);

/* Function Name:
 *      dal_longan_pie_meterMode_get
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
dal_longan_pie_meterMode_get(
    uint32  unit,
    uint32  blockIdx,
    rtk_pie_meterMode_t *pMeterMode);

/* Function Name:
 *      dal_longan_pie_meterMode_set
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
dal_longan_pie_meterMode_set(
    uint32  unit,
    uint32  blockIdx,
    rtk_pie_meterMode_t meterMode);

/* Function Name:
 *      dal_longan_pie_meterIncludeIfg_get
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
dal_longan_pie_meterIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

/* Function Name:
 *      dal_longan_pie_meterIncludeIfg_set
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
dal_longan_pie_meterIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);

/* Function Name:
 *      dal_longan_pie_meterExceed_get
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
dal_longan_pie_meterExceed_get(
    uint32  unit,
    uint32  meterIdx,
    uint32  *pIsExceed);

/* Function Name:
 *      dal_longan_pie_meterExceedAggregation_get
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
dal_longan_pie_meterExceedAggregation_get(
    uint32  unit,
    uint32  *pExceedMask);


/* Function Name:
 *      dal_longan_pie_meterEntry_get
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
dal_longan_pie_meterEntry_get(
    uint32          unit,
    uint32          meterIdx,
    rtk_pie_meterEntry_t   *pMeterEntry);

/* Function Name:
 *      dal_longan_pie_meterEntry_set
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
dal_longan_pie_meterEntry_set(
    uint32          unit,
    uint32          meterIdx,
    rtk_pie_meterEntry_t   *pMeterEntry);

/* Function Name:
 *      dal_longan_pie_meterDpSel_get
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
dal_longan_pie_meterDpSel_get(
    uint32                  unit,
    rtk_pie_meterDpSel_t    *pDpSel);

/* Function Name:
 *      dal_longan_pie_meterDpSel_set
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
dal_longan_pie_meterDpSel_set(
    uint32                  unit,
    rtk_pie_meterDpSel_t    dpSel);

/* Function Name:
 *      dal_longan_pie_templateVlanSel_get
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
dal_longan_pie_templateVlanSel_get(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    uint32                      preTemplate_idx,
    rtk_pie_templateVlanSel_t   *pVlanSel);

/* Function Name:
 *      dal_longan_pie_templateVlanSel_set
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
dal_longan_pie_templateVlanSel_set(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    uint32                      preTemplate_idx,
    rtk_pie_templateVlanSel_t   vlanSel);

/* Function Name:
 *      dal_longan_pie_arpMacSel_get
 * Description:
 *      Get the configuration of ARP MAC address select.
 * Input:
 *      unit        - unit id
 * Output:
 *      pArpMacSel  - pointer to ARP MAC select
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
dal_longan_pie_arpMacSel_get(uint32 unit, rtk_pie_arpMacSel_t *pArpMacSel);

/* Function Name:
 *      dal_longan_pie_arpMacSel_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_pie_arpMacSel_set(uint32 unit, rtk_pie_arpMacSel_t arpMacSel);

/* Function Name:
 *      dal_longan_pie_templateVlanFmtSel_get
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
int32
dal_longan_pie_templateVlanFmtSel_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t *pVlanFmtSel);

/* Function Name:
 *      dal_longan_pie_templateVlanFmtSel_set
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
int32
dal_longan_pie_templateVlanFmtSel_set(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t vlanFmtSel);

#endif /* __DAL_LONGAN_PIE_H__ */


