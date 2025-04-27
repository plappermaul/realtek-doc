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
 *            1) Ingress PIE
 *            2) Egress PIE
 *            3) Field Selector
 *            4) Range Check
 *            5) Meter
 *            6) Counter
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/pie.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name     : PIE */

/* Function Name:
 *      rtk_pie_init
 * Description:
 *      Initialize PIE module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      9300, 9310
 * Note:
 *      Must initialize PIE module before calling any PIE APIs.
 * Changes:
 *      None
 */
int32
rtk_pie_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_init(unit);
} /* end of rtk_pie_init */

/* Function Name:
 *      rtk_pie_phase_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_phase_get(uint32 unit, uint32 block_idx, rtk_pie_phase_t *pPhase)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_phase_get(unit, block_idx, pPhase);
}

/* Function Name:
 *      rtk_pie_phase_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_phase_set(uint32 unit, uint32 block_idx, rtk_pie_phase_t phase)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_phase_set(unit, block_idx, phase);
}

/* Function Name:
 *      rtk_pie_blockLookupEnable_get
 * Description:
 *      Get the pie block lookup state.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 * Output:
 *      pEnable   - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - block index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The rule data are kept regardless of the lookup status.
 *      (2) The lookup result is always false if the lookup state is disabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_blockLookupEnable_get)
 */
int32
rtk_pie_blockLookupEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_blockLookupEnable_get(unit, block_idx, pEnable);
} /* end of rtk_pie_blockLookupEnable_get */

/* Function Name:
 *      rtk_pie_blockLookupEnable_set
 * Description:
 *      Set the pie block lookup state.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 *      enable    - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - block index is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The rule data are kept regardless of the lookup status.
 *      (2) The lookup result is always false if the lookup state is disabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_blockLookupEnable_set)
 */
int32
rtk_pie_blockLookupEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_blockLookupEnable_set(unit, block_idx, enable);
} /* end of rtk_pie_blockLookupEnable_set */

/* Function Name:
 *      rtk_pie_blockGrouping_get
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
 *      9300, 9310
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group blocks which belong to different phase is forbidden.
 *      (3) Group id > logic id > physical block id
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_blockGrouping_get(uint32 unit, uint32 block_idx, uint32 *group_id,
    uint32 *logic_id)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_blockGrouping_get(unit, block_idx, group_id, logic_id);
}   /* end of rtk_pie_blockGrouping_get */

/* Function Name:
 *      rtk_pie_blockGrouping_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group blocks which belong to different phase is forbidden.
 *      (3) Group id > logic id > physical block id
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_blockGrouping_set(uint32 unit, uint32 block_idx, uint32 group_id,
    uint32 logic_id)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_blockGrouping_set(unit, block_idx, group_id, logic_id);
}   /* end of rtk_pie_blockGrouping_set */

/* Function Name:
 *      rtk_pie_template_get
 * Description:
 *      Get the content of specific template.
 * Input:
 *      unit        - unit id
 *      template_id - template ID
 * Output:
 *      pTemplate   - template content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX - invalid template index
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      ID 0-4 are fixed template, ID 5-9 are user defined template.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *           This function name is changed compared with sdk-2(rtk_acl_template_get)
 */
int32
rtk_pie_template_get(uint32 unit, uint32 template_id,
    rtk_pie_template_t *pTemplate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_template_get(unit, template_id, pTemplate);
} /* end of rtk_pie_template_get */

/* Function Name:
 *      rtk_pie_template_set
 * Description:
 *      Set the content of specific template.
 * Input:
 *      unit        - unit id
 *      template_id - template ID
 *      pTemplate   - template content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_PIE_TEMPLATE_INDEX - invalid template index
 *      RT_ERR_PIE_FIELD_TYPE     - invalid PIE field type
 *      RT_ERR_PIE_FIELD_LOCATION - invalid field location
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) ID 0-4 are fixed template, ID 5-9 are user defined template.
 *      (2) Configure fixed templates is prohibited.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *           This function name is changed compared with sdk-2(rtk_acl_template_get)
 */
int32
rtk_pie_template_set(uint32 unit, uint32 template_id, rtk_pie_template_t *pTemplate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_template_set(unit, template_id, pTemplate);
} /* end of rtk_pie_template_set */

/* Function Name:
 *      rtk_pie_templateField_check
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
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_PIE_PHASE      - invalid PIE phase
 *      RT_ERR_PIE_FIELD_TYPE - invalid PIE field type
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_templateField_check)
 */
int32
rtk_pie_templateField_check(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    rtk_pie_templateFieldType_t type)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_templateField_check(unit, phase, type);
} /* end of rtk_pie_templateField_check */

/* Sub-module Name :
 *  Range Check
 */

/* Function Name:
 *      rtk_pie_rangeCheckIp_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) For IPv6 range check, index 0/4 means IP6[31:0], index 1/5 means IP6[63:32],
 *          index 2/6 means IP6[95:64], index 3/7 means IP6[127:96]. Index 0~3/4~7 must
 *          be used together in order to filter a full IPv6 address.
 *      (2) For IPv6 suffix range check, index 0/2/4/6 means IP6[31:0], index 1/3/5/7 means IP6[63:32],
 *          Index 0&1/2&3/4&5/6&7 must be used together in order to filter a IPv6 suffix address.
 *      (3) If conditions for (1)/(2) is not satisfied, each range check entry should compare independently.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *           This function name is changed compared with sdk-2(rtk_acl_rangeCheckIp_get)
 */
int32
rtk_pie_rangeCheckIp_get(uint32 unit, uint32 index, rtk_pie_rangeCheck_ip_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_rangeCheckIp_get(unit, index, pData);
} /* end of rtk_pie_rangeCheckIp_get */

/* Function Name:
 *      rtk_pie_rangeCheckIp_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) For IPv6 range check, index 0/4 means IP6[31:0], index 1/5 means IP6[63:32],
 *          index 2/6 means IP6[95:64], index 3/7 means IP6[127:96]. Index 0~3/4~7 must
 *          be used together in order to filter a full IPv6 address.
 *      (2) For IPv6 suffix range check, index 0/2/4/6 means IP6[31:0], index 1/3/5/7 means IP6[63:32],
 *          Index 0&1/2&3/4&5/6&7 must be used together in order to filter a IPv6 suffix address.
 *      (3) If conditions for (1)/(2) is not satisfied, each range check entry should compare independently.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *           This function name is changed compared with sdk-2(rtk_acl_rangeCheckIp_set)
 */
int32
rtk_pie_rangeCheckIp_set(uint32 unit, uint32 index, rtk_pie_rangeCheck_ip_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_rangeCheckIp_set(unit, index, pData);
} /* end of rtk_pie_rangeCheckIp_set */

/* Function Name:
 *      rtk_pie_rangeCheck_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_rangeCheck_get(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_rangeCheck_get(unit, index, pData);
}   /* end of rtk_pie_rangeCheck_get */

/* Function Name:
 *      rtk_pie_rangeCheck_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_rangeCheck_set(uint32 unit, uint32 index,
    rtk_pie_rangeCheck_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_rangeCheck_set(unit, index, pData);
}   /* end of rtk_pie_rangeCheck_set */

/* Sub-module Name :
 *  Field Selector
 */

/* Function Name:
 *      rtk_pie_fieldSelector_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If offset is longer than the actual packet length or packet type is not matched, the value retrieved by
 *      field selector should be 0x0, also field selector valid mask(pie field type) should be 0x0.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_fieldSelector_get)
 */
int32
rtk_pie_fieldSelector_get(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_pie_fieldSelector_data_t *pFs)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_fieldSelector_get(unit, fs_idx, pFs);
} /* end of rtk_pie_fieldSelector_get */

/* Function Name:
 *      rtk_pie_fieldSelector_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If offset is longer than the actual packet length or packet type is not matched, the value retrieved by
 *      field selector should be 0x0, also field selector valid mask(pie field type) should be 0x0.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_fieldSelector_set)
 */
int32
rtk_pie_fieldSelector_set(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_pie_fieldSelector_data_t *pFs)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_fieldSelector_set(unit, fs_idx, pFs);
} /* end of rtk_pie_fieldSelector_set */

/* Sub-module Name :
 *  Field Selector
 */

/* Function Name:
 *      rtk_pie_meterIncludeIfg_get
 * Description:
 *      Get IFG including status for packet length calculation of meter module.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - pointer to enable status of includes IFG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterIncludeIfg_get)
 */
int32
rtk_pie_meterIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_meterIncludeIfg_get(unit, pIfg_include);
} /* end of rtk_pie_meterIncludeIfg_get */

/* Function Name:
 *      rtk_pie_meterIncludeIfg_set
 * Description:
 *      Set IFG including status for packet length calculation of meter module.
 * Input:
 *      unit        - unit id
 *      ifg_include - enable status of includes IFG
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterIncludeIfg_get)
 */
int32
rtk_pie_meterIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_meterIncludeIfg_set(unit, ifg_include);
} /* end of rtk_pie_meterIncludeIfg_set */

/* Function Name:
 *      rtk_pie_meterExceed_get
 * Description:
 *      Get the meter exceed flag of a meter entry.
 * Input:
 *      unit      - unit id
 *      meterIdx  - meter entry index
 * Output:
 *      pIsExceed - pointer to exceed flag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterExceed_get)
 */
int32
rtk_pie_meterExceed_get(
    uint32  unit,
    uint32  meterIdx,
    uint32  *pIsExceed)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_meterExceed_get(unit, meterIdx, pIsExceed);
} /* end of rtk_pie_meterExceed_get */

/* Function Name:
 *      rtk_pie_meterExceedAggregation_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterExceedAggregation_get)
 */
int32
rtk_pie_meterExceedAggregation_get(
    uint32  unit,
    uint32  *pExceedMask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_meterExceedAggregation_get(unit, pExceedMask);
} /* end of rtk_pie_meterExceedAggregation_get */

/* Function Name:
 *      rtk_pie_meterEntry_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterEntry_get)
 */
int32
rtk_pie_meterEntry_get(
    uint32                  unit,
    uint32                  meterIdx,
    rtk_pie_meterEntry_t    *pMeterEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_meterEntry_get(unit, meterIdx, pMeterEntry);
} /* end of rtk_pie_meterEntry_get */

/* Function Name:
 *      rtk_pie_meterEntry_set
 * Description:
 *      Set a meter entry.
 * Input:
 *      unit        - unit id
 *      meterIdx    - meter entry index
 *      pMeterEntry - pointer to meter entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_acl_meterEntry_set)
 */
int32
rtk_pie_meterEntry_set(
    uint32                  unit,
    uint32                  meterIdx,
    rtk_pie_meterEntry_t    *pMeterEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_meterEntry_set(unit, meterIdx, pMeterEntry);
} /* end of rtk_pie_meterEntry_set */

/* Function Name:
 *      rtk_pie_meterDpSel_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_meterDpSel_get(
    uint32                  unit,
    rtk_pie_meterDpSel_t    *pDpSel)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_meterDpSel_get(unit, pDpSel);
}

/* Function Name:
 *      rtk_pie_meterDpSel_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_meterDpSel_set(
    uint32                  unit,
    rtk_pie_meterDpSel_t    dpSel)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_meterDpSel_set(unit, dpSel);
}

/* Function Name:
 *      rtk_pie_templateVlanSel_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) The API does not support pre-defined template3.
 *      (2) Egress PIE has one more option(SRC_FVLAN) than Ingress PIE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_templateVlanSel_get(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    uint32                      preTemplate_idx,
    rtk_pie_templateVlanSel_t   *pVlanSel)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_templateVlanSel_get(unit, phase, preTemplate_idx, pVlanSel);
}
/* Function Name:
 *      rtk_pie_templateVlanSel_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) The API does not support pre-defined template3.
 *      (2) Egress PIE has one more option(SRC_FVLAN) than Ingress PIE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_templateVlanSel_set(
    uint32                      unit,
    rtk_pie_phase_t             phase,
    uint32                      preTemplate_idx,
    rtk_pie_templateVlanSel_t   vlanSel)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_templateVlanSel_set(unit, phase, preTemplate_idx, vlanSel);
}

/* Function Name:
 *      rtk_pie_arpMacSel_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_arpMacSel_get(uint32 unit, rtk_pie_arpMacSel_t *pArpMacSel)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_arpMacSel_get(unit, pArpMacSel);
}   /* end of rtk_pie_arpMacSel_get */

/* Function Name:
 *      rtk_pie_arpMacSel_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_arpMacSel_set(uint32 unit, rtk_pie_arpMacSel_t arpMacSel)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_arpMacSel_set(unit, arpMacSel);
}   /* end of rtk_pie_arpMacSel_set */

/* Function Name:
 *      rtk_pie_intfSel_get
 * Description:
 *      Get the configuration of filter interface type.
 * Input:
 *      unit       - unit id
 * Output:
 *      intfSel    - select inteface filter type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_intfSel_get(uint32 unit, rtk_pie_intfSel_t *intfSel)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_intfSel_get(unit, intfSel);
}   /* end of rtk_pie_intfSel_get */

/* Function Name:
 *      rtk_pie_intfSel_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_intfSel_set(uint32 unit, rtk_pie_intfSel_t intfSel)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_intfSel_set(unit, intfSel);
}   /* end of rtk_pie_intfSel_set */

/* Function Name:
 *      rtk_pie_templateVlanFmtSel_get
 * Description:
 *      Get the configuration of otag/itag VLAN format select in template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      template_idx        - template index
 * Output:
 *      pVlanFmtSel         - pointer to VLAN format select in template.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      The API onlye support ingress PIE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_templateVlanFmtSel_get(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t *pVlanFmtSel)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_templateVlanFmtSel_get(unit, phase, template_idx, pVlanFmtSel);
}   /* end of rtk_pie_templateVlanFmtSel_get */

/* Function Name:
 *      rtk_pie_templateVlanFmtSel_set
 * Description:
 *      Set the configuration of otag/itag VLAN format select in template.
 * Input:
 *      unit                - unit id
 *      phase               - PIE lookup phase
 *      template_idx        - template index
 *      vlanFmtSel          - VLAN format select in template.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_pie_BLOCK_INDEX - invalid block index
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      The API onlye support ingress PIE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_pie_templateVlanFmtSel_set(uint32 unit, rtk_pie_phase_t phase,
    uint32 template_idx, rtk_pie_templateVlanFmtSel_t vlanFmtSel)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_templateVlanFmtSel_set(unit, phase, template_idx, vlanFmtSel);
}   /* end of rtk_pie_templateVlanFmtSel_set */

/* Function Name:
 *      rtk_pie_meterTrtcmType_get
 * Description:
 *      Get a meter trTCM type.
 * Input:
 *      unit        - unit id
 * Output:
 *      type        - meter TrTCM type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
int32
rtk_pie_meterTrtcmType_get(uint32 unit, rtk_pie_meterTrtcmType_t *type)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_meterTrtcmType_get(unit, type);
}   /* end of rtk_pie_meterTrtcmType_get */

/* Function Name:
 *      rtk_pie_meterTrtcmType_set
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
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
int32
rtk_pie_meterTrtcmType_set(uint32 unit, rtk_pie_meterTrtcmType_t type)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_meterTrtcmType_set(unit, type);
}   /* end of rtk_pie_meterTrtcmType_set */

/* Function Name:
 *      rtk_pie_filter1BR_get
 * Description:
 *      Get filter 802.1BR status.
 * Input:
 *      unit                - unit id
 * Output:
 *      en    - filter 802.1BR status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
rtk_pie_filter1BR_get(uint32 unit, rtk_enable_t *en)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_filter1BR_get(unit, en);
}   /* end of rtk_pie_filter1BR_get */

/* Function Name:
 *      rtk_pie_filter1BR_set
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
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
int32
rtk_pie_filter1BR_set(uint32 unit, rtk_enable_t en)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->pie_filter1BR_set(unit, en);
}   /* end of rtk_pie_filter1BR_set */

