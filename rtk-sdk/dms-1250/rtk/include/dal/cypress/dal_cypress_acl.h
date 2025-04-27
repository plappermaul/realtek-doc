/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public ACL APIs and its data type in the SDK .
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

#ifndef __DAL_CYPRESS_ACL_H__
#define __DAL_CYPRESS_ACL_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/acl.h>
#include <rtk/pie.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */
#define METER_LB_BPS_TICK_250M        288
#define METER_LB_BPS_TOKEN_250M       151
#define METER_LB_BPS_TICK_50M         214
#define METER_LB_BPS_TOKEN_50M        561
#define METER_LB_BPS_TICK_250M_10G    211
#define METER_LB_BPS_TOKEN_250M_10G   1770
#define METER_LB_BPS_TICK_50M_10G     211
#define METER_LB_BPS_TOKEN_50M_10G    8850
#define METER_LB_BPS_HIGH_THRESH      65535

#define METER_LB_PPS_TICK_250M        238
#define METER_LB_PPS_TOKEN_250M       1
#define METER_LB_PPS_TICK_50M         240
#define METER_LB_PPS_TOKEN_50M        5
#define METER_LB_PPS_TICK_250M_10G    238
#define METER_LB_PPS_TOKEN_250M_10G   1
#define METER_LB_PPS_TICK_50M_10G     240
#define METER_LB_PPS_TOKEN_50M_10G    5
#define METER_LB_PPS_HIGH_THRESH      8

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_cypress_aclMapper_init
 * Description:
 *      Hook acl module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook acl module before calling any acl APIs.
 */
extern int32
dal_cypress_aclMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_cypress_acl_init
 * Description:
 *      Initialize ACL module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize ACL module before calling any ACL APIs.
 */
extern int32
dal_cypress_acl_init(uint32 unit);

/* Function Name:
 *      dal_cypress_acl_partition_get
 * Description:
 *      Get the acl partition configuration from the specified device.
 * Input:
 *      unit       - unit id
 * Output:
 *      pPartition - pointer buffer of partition value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_partition_get(uint32 unit, uint32 *pPartition);

/* Function Name:
 *      dal_cypress_acl_partition_set
 * Description:
 *      Set the acl partition configuration to the specified device.
 * Input:
 *      unit        - unit id
 *      partition   - partition value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_OUT_OF_RANGE   - partition value is out of range
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_partition_set(uint32 unit, uint32 partition);

/* Function Name:
 *      dal_cypress_acl_blockPwrEnable_get
 * Description:
 *      Get the acl block power state.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      The rule data are cleared if the block power is disabled.
 */
extern int32
dal_cypress_acl_blockPwrEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_acl_blockPwrEnable_set
 * Description:
 *      Set the acl block power state.
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
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      The rule data are cleared if the block power is disabled.
 */
extern int32
dal_cypress_acl_blockPwrEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_acl_blockLookupEnable_get
 * Description:
 *      Get the acl block lookup state.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      1.The rule data are kept regardless of the lookup status.
 *      2.The lookup result is always false if the lookup state is disabled.
 */
extern int32
dal_cypress_acl_blockLookupEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_acl_blockLookupEnable_set
 * Description:
 *      Set the acl block lookup state.
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
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      1.The rule data are kept regardless of the lookup status.
 *      2.The lookup result is always false if the lookup state is disabled.
 */
extern int32
dal_cypress_acl_blockLookupEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_acl_ruleEntryFieldSize_get
 * Description:
 *      Get the field size of ACL entry.
 * Input:
 *      unit        - unit id
 *      type        - type of entry field
 * Output:
 *      pField_size - field size of ACL entry.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      The unit of size is bit.
 */
extern int32
dal_cypress_acl_ruleEntryFieldSize_get(uint32 unit, rtk_acl_fieldType_t type, uint32 *pField_size);

/* Function Name:
 *      dal_cypress_acl_ruleEntrySize_get
 * Description:
 *      Get the rule entry size of ACL.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 * Output:
 *      pEntry_size - rule entry size of ingress ACL
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The unit of size is byte.
 */
extern int32
dal_cypress_acl_ruleEntrySize_get(uint32 unit, rtk_acl_phase_t phase, uint32 *pEntry_size);

/* Function Name:
 *      dal_cypress_acl_ruleValidate_get
 * Description:
 *      Validate ACL rule without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - entry index
 * Output:
 *      valid     - valid state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_ruleValidate_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              *pValid);

/* Function Name:
 *      dal_cypress_acl_ruleValidate_set
 * Description:
 *      Validate ACL rule without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - entry index
 * Output:
 *      valid     - valid state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_ruleValidate_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              valid);

/* Function Name:
 *      dal_cypress_acl_ruleEntry_read
 * Description:
 *      Read the entry data from specified ACL entry.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - entry index
 * Output:
 *      pEntry_buffer - data buffer of ACL entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_ruleEntry_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer);

/* Function Name:
 *      dal_cypress_acl_ruleEntry_write
 * Description:
 *      Write the entry data to specified ACL entry.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - entry index
 *      pEntry_buffer - data buffer of ACL entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_ruleEntry_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer);

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_get
 * Description:
 *      Get the field data from specified ACL entry buffer.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - ACL entry index
 *      pEntry_buffer - data buffer of ACL entry
 *      type          - field type
 * Output:
 *      pData         - field data
 *      pMask         - field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      The API reads the field data/mask from the entry buffer. Use rtk_acl_ruleEntry_read to
 *      read the rule data to the entry buffer.
 */
extern int32
dal_cypress_acl_ruleEntryField_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_set
 * Description:
 *      Set the field data to specified ACL entry buffer.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - ACL entry index
 *      pEntry_buffer - data buffer of ACL entry
 *      type          - field type
 *      pData         - field data
 *      pMask         - field mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      The API writes the field data/mask to the entry buffer. After the fields are configured,
 *      use rtk_acl_ruleEntry_write to write the entry buffer to ASIC at once.
 */
extern int32
dal_cypress_acl_ruleEntryField_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_read
 * Description:
 *      Read the field data from specified ACL entry.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      type      - field type
 * Output:
 *      pData     - field data
 *      pMask     - field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_acl_ruleEntryField_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_write
 * Description:
 *      Write the field data to specified ACL entry.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      type      - field type
 * Output:
 *      pData     - field data
 *      pMask     - field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_acl_ruleEntryField_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

/* Function Name:
 *      dal_cypress_acl_ruleEntryField_check
 * Description:
 *      Check whether the specified field type is supported on the chip.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      type        - field type to be checked
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_ACL_PHASE        - invalid ACL phase
 *      RT_ERR_ACL_FIELD_TYPE   - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_ruleEntryField_check(uint32 unit, rtk_acl_phase_t phase,
        rtk_acl_fieldType_t type);

/* Function Name:
 *      dal_cypress_acl_ruleOperation_get
 * Description:
 *      Get ACL rule operation.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      entry_idx   - ACL entry index
 * Output:
 *      pOperation  - operation configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) For reverse operation, valid index is N where N = 0,1,2...
 *      (2) For aggr_1 operation, index must be 2N where N = 0,1,2...
 *      (3) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,2...
 *      (4) For aggregating 4 rules, both aggr_1 and aggr_2 must be enabled.
 */
extern int32
dal_cypress_acl_ruleOperation_get(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);

/* Function Name:
 *      dal_cypress_acl_ruleOperation_set
 * Description:
 *      Set ACL rule operation.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      entry_idx   - ACL entry index
 *      pOperation  - operation configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) For reverse operation, valid index is N where N = 0,1,2...
 *      (2) For aggr_1 operation, index must be 2N where N = 0,1,2...
 *      (3) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,2...
 *      (4) For aggregating 4 rules, both aggr_1 and aggr_2 must be enabled.
 */
extern int32
dal_cypress_acl_ruleOperation_set(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);

/* Function Name:
 *      dal_cypress_acl_ruleAction_get
 * Description:
 *      Get the ACL rule action configuration.
 * Input:
 *      unit       - unit id
 *      phase      - ACL lookup phase
 *      entry_idx  - ACL entry index
 * Output:
 *      pAction    - action mask and data configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_acl_ruleAction_get(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction);

/* Function Name:
 *      dal_cypress_acl_ruleAction_set
 * Description:
 *      Set the ACL rule action configuration.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      entry_idx   - ACL entry index
 *      pAction     - action mask and data configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_acl_ruleAction_set(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction);

/* Function Name:
 *      dal_cypress_acl_ruleHitIndication_get
 * Description:
 *      Get the ACL rule hit indication.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      entry_idx   - ACL entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_acl_ruleHitIndication_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              reset,
    uint32              *pIsHit);


/* Function Name:
 *      dal_cypress_acl_ruleHitIndicationMask_get
 * Description:
 *      Get the ACL rule hit indication bitmask of the specified phase.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      reset       - reset the hit status
 * Output:
 *      pHitMask    - pointer to hit status bitmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_acl_ruleHitIndicationMask_get(uint32 unit, rtk_acl_phase_t phase,
    uint32 reset, rtk_acl_hitMask_t *pHitMask);


/* Function Name:
 *      dal_cypress_acl_rule_del
 * Description:
 *      Delete the specified ACL rules.
 * Input:
 *      unit    - unit id
 *      phase   - ACL lookup phase
 *      pClrIdx - rule index to clear
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_ACL_PHASE       - invalid ACL phase
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_ACL_CLEAR_INDEX - end index is lower than start index
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Note:
 *      Entry fields, operations and actions are all cleared.
 */
extern int32
dal_cypress_acl_rule_del(uint32 unit, rtk_acl_phase_t phase, rtk_acl_clear_t *pClrIdx);

/* Function Name:
 *      dal_cypress_acl_rule_move
 * Description:
 *      Move the specified ACL rules.
 * Input:
 *      unit    - unit id
 *      phase   - ACL lookup phase
 *      pData   - movement info
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      1.Entry fields, operations and actions are all moved.
 *      2.The vacant entries due to movement are auto cleared to be invalid by H/W.
 *      3.(move_from + length) and (move_to + length) must <= the number of ACL rule
 */
extern int32
dal_cypress_acl_rule_move(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pData);

/* Function Name:
 *      dal_cypress_acl_templateSelector_get
 * Description:
 *      Get the mapping template of specific block.
 * Input:
 *      unit          - unit id
 *      block_idx     - block index
 * Output:
 *      pTemplate_idx - template index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      block_idx 0-17
 */
extern int32
dal_cypress_acl_templateSelector_get(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   *pTemplate_idx);

/* Function Name:
 *      dal_cypress_acl_templateSelector_set
 * Description:
 *      Set the mapping template of specific block.
 * Input:
 *      unit         - unit id
 *      block_idx    - block index
 *      template_idx - template index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT                     - The module is not initial
 *      RT_ERR_ACL_TEMPLATE_INDEX           - invalid template index
 *      RT_ERR_ACL_TEMPLATE_INCOMPATIBLE    - try to map a ACL block to an incompatible template
 *      RT_ERR_INPUT                        - invalid input parameter
 * Note:
 *      block_idx 0-17, template_idx 0-7
 */
extern int32
dal_cypress_acl_templateSelector_set(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   template_idx);

/* Function Name:
 *      dal_cypress_acl_template_get
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
 *      RT_ERR_ACL_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_template_get(uint32 unit, uint32 template_id, rtk_acl_template_t *pTemplate);

/* Function Name:
 *      dal_cypress_acl_template_set
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
 *      RT_ERR_ACL_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_template_set(uint32 unit, uint32 template_id, rtk_acl_template_t *pTemplate);

/* Function Name:
 *      dal_cypress_acl_templateField_check
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
 *      RT_ERR_ACL_PHASE                    - invalid ACL phase
 *      RT_ERR_ACL_FIELD_TYPE               - invalid ACL field type
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_templateField_check(uint32 unit, rtk_pie_phase_t phase,
    rtk_acl_templateFieldType_t type);

/* Function Name:
 *      dal_cypress_acl_blockResultMode_get
 * Description:
 *      Get the acl block result mode.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 * Output:
 *      pMode       - block result mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      If a packet hit multiple rules and the mode is configured to ACL_BLOCK_RESULT_SINGLE, then
 *      the hit result will be the rule with the lowest index.
 */
extern int32
dal_cypress_acl_blockResultMode_get(uint32 unit, uint32 block_idx, rtk_acl_blockResultMode_t *pMode);

/* Function Name:
 *      dal_cypress_acl_blockResultMode_set
 * Description:
 *      Set the acl block result mode.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      mode        - block result mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      If a packet hit multiple rules and the mode is configured to ACL_BLOCK_RESULT_SINGLE, then
 *      the hit result will be the rule with the lowest index.
 */
extern int32
dal_cypress_acl_blockResultMode_set(uint32 unit, uint32 block_idx, rtk_acl_blockResultMode_t mode);

/* Function Name:
 *      dal_cypress_acl_blockGroupEnable_get
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      group_type  - grouping type
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      1.If multiple physical blocks are grouped to a logical block,
 *        it only outputs a single hit result and the hit result will be
 *        the entry with lowest index.
 *      2.Group ingress ACL block with egress ACL block is forbidden.
 *      3.For ACL_BLOCK_GROUP_2, valid index is 2N where N = 0,1...
 *      4.For ACL_BLOCK_GROUP_4, valid index is 4N where N = 0,1...
 *      5.For ACL_BLOCK_GROUP_8, valid index is 8N where N = 0,1...
 *      6.For ACL_BLOCK_GROUP_ALL, valid index is 0.
 *      7.If multiple grouping types are applied to the same block index, then
 *        the priority will be ACL_BLOCK_GROUP_ALL > ACL_BLOCK_GROUP_8 >
 *        ACL_BLOCK_GROUP_4 > ACL_BLOCK_GROUP_2.
 */
extern int32
dal_cypress_acl_blockGroupEnable_get(
    uint32                     unit,
    uint32                     block_idx,
    rtk_acl_blockGroup_t       group_type,
    rtk_enable_t               *pEnable);

/* Function Name:
 *      dal_cypress_acl_blockGroupEnable_set
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit        - unit id
 *      block_idx   - block index
 *      group_type  - grouping type
 *      enable      - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      1.If multiple physical blocks are grouped to a logical block,
 *        it only outputs a single hit result and the hit result will be
 *        the entry with lowest index.
 *      2.Group ingress ACL block with egress ACL block is forbidden.
 *      3.For ACL_BLOCK_GROUP_2, valid index is 2N where N = 0,1...
 *      4.For ACL_BLOCK_GROUP_4, valid index is 4N where N = 0,1...
 *      5.For ACL_BLOCK_GROUP_8, valid index is 8N where N = 0,1...
 *      6.For ACL_BLOCK_GROUP_ALL, valid index is 0.
 *      7.If multiple grouping types are applied to the same block index, then
 *        the priority will be ACL_BLOCK_GROUP_ALL > ACL_BLOCK_GROUP_8 >
 *        ACL_BLOCK_GROUP_4 > ACL_BLOCK_GROUP_2.
 */
extern int32
dal_cypress_acl_blockGroupEnable_set(
    uint32                     unit,
    uint32                     block_idx,
    rtk_acl_blockGroup_t       group_type,
    rtk_enable_t               enable);

/* Function Name:
 *      dal_cypress_acl_statPktCnt_get
 * Description:
 *      Get packet-based statistic counter of the log id.
 * Input:
 *      unit      - unit id
 *      log_id    - log id
 * Output:
 *      pPkt_cnt  - pointer buffer of packet count
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
dal_cypress_acl_statPktCnt_get(uint32 unit, uint32 log_id, uint32 *pPkt_cnt);

/* Function Name:
 *      dal_cypress_acl_statPktCnt_clear
 * Description:
 *      Set packet-based statistic counter of the log id.
 * Input:
 *      unit      - unit id
 *      log_id    - log id
 *      pkt_cnt   - packet count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_statPktCnt_clear(uint32 unit, uint32 log_id);

/* Function Name:
 *      dal_cypress_acl_statByteCnt_get
 * Description:
 *      Get byte-based statistic counter of the log id.
 * Input:
 *      unit      - unit id
 *      log_id    - log id
 * Output:
 *      pByte_cnt - byte count
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
dal_cypress_acl_statByteCnt_get(uint32 unit, uint32 log_id, uint64 *pByte_cnt);

/* Function Name:
 *      dal_cypress_acl_statByteCnt_clear
 * Description:
 *      Reset byte-based statistic counter of the log id.
 * Input:
 *      unit      - unit id
 *      log_id    - log id
 *      byte_cnt  - byte count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_statByteCnt_clear(uint32 unit, uint32 log_id);

/* Function Name:
 *      dal_cypress_acl_stat_clearAll
 * Description:
 *      Clear all statistic counter for the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      All the packet counters and byte counters are cleared.
 */
extern int32
dal_cypress_acl_stat_clearAll(uint32 unit);

/* Function Name:
 *      dal_cypress_acl_rangeCheckL4Port_get
 * Description:
 *      Get the configuration of L4 port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of L4 port
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
dal_cypress_acl_rangeCheckL4Port_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckL4Port_set
 * Description:
 *      Set the configuration of L4 port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of L4 port
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
dal_cypress_acl_rangeCheckL4Port_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckVid_get
 * Description:
 *      Get the configuration of VID range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of VID
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
dal_cypress_acl_rangeCheckVid_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckVid_set
 * Description:
 *      Set the configuration of VID range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of VID
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
dal_cypress_acl_rangeCheckVid_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckIp_get
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
dal_cypress_acl_rangeCheckIp_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_ip_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckIp_set
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
dal_cypress_acl_rangeCheckIp_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_ip_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckSrcPort_get
 * Description:
 *      Get the configuration of source port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of source port
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
dal_cypress_acl_rangeCheckSrcPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckSrcPort_set
 * Description:
 *      Set the configuration of source port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of source port
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
dal_cypress_acl_rangeCheckSrcPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckDstPort_get
 * Description:
 *      Get the configuration of destination port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of destination port
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
dal_cypress_acl_rangeCheckDstPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckDstPort_set
 * Description:
 *      Set the configuration of destination port range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of destination port
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
dal_cypress_acl_rangeCheckDstPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckPacketLen_get
 * Description:
 *      Get the configuration of packet length range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Packet length includes CRC(4Byte)
 */
extern int32
dal_cypress_acl_rangeCheckPacketLen_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData);

/* Function Name:
 *      dal_cypress_acl_rangeCheckPacketLen_set
 * Description:
 *      Set the configuration of packet length range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of packet length
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
 *      Packet length includes CRC(4Byte)
 */
extern int32
dal_cypress_acl_rangeCheckPacketLen_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData);

/* Function Name:
 *      dal_cypress_acl_fieldSelector_get
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
dal_cypress_acl_fieldSelector_get(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_acl_fieldSelector_data_t *pFs);

/* Function Name:
 *      dal_cypress_acl_fieldSelector_set
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
dal_cypress_acl_fieldSelector_set(
    uint32                       unit,
    uint32                       fs_idx,
    rtk_acl_fieldSelector_data_t *pFs);

/* Function Name:
 *      dal_cypress_acl_meterMode_get
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
dal_cypress_acl_meterMode_get(
    uint32  unit,
    uint32  blockIdx,
    rtk_acl_meterMode_t *pMeterMode);

/* Function Name:
 *      dal_cypress_acl_meterMode_set
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
dal_cypress_acl_meterMode_set(
    uint32  unit,
    uint32  blockIdx,
    rtk_acl_meterMode_t meterMode);

/* Function Name:
 *      dal_cypress_acl_meterTokenRefill_get
 * Description:
 *      Get token refill T/B value of meter block.
 * Input:
 *      unit         - unit id
 *      blockIdx     - block id
 * Output:
 *      pTickPeriod  - pointer to tick period (unit:clock)
 *      pTokenLen    - pointer to token length (unit: byte or packet depends on meter mode)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_meterTokenRefill_get(
    uint32  unit,
    uint32  blockIdx,
    uint32  *pTickPeriod,
    uint32  *pTokenLen);

/* Function Name:
 *      dal_cypress_acl_meterTokenRefill_get
 * Description:
 *      Set token refill T/B value of meter block.
 * Input:
 *      unit         - unit id
 *      blockIdx     - block id
 *      pTickPeriod  - tick period (unit:clock)
 *      pTokenLen    - token length (unit: byte or packet depends on meter mode)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_acl_meterTokenRefill_set(
    uint32  unit,
    uint32  blockIdx,
    uint32  tickPeriod,
    uint32  tokenLen);

/* Function Name:
 *      dal_cypress_acl_meterIncludeIfg_get
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
dal_cypress_acl_meterIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

/* Function Name:
 *      dal_cypress_acl_meterIncludeIfg_set
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
dal_cypress_acl_meterIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);

/* Function Name:
 *      dal_cypress_acl_meterBurstSize_get
 * Description:
 *      Get the meter burst sizes of a specific meter mode.
 * Input:
 *      unit        - unit id
 *      meterMode   - meter mode
 * Output:
 *      pBurstSize  - pointer to burst sizes
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
dal_cypress_acl_meterBurstSize_get(
    uint32              unit,
    rtk_acl_meterMode_t meterMode,
    rtk_acl_meterBurstSize_t  *pBurstSize);

/* Function Name:
 *      dal_cypress_acl_meterBurstSize_set
 * Description:
 *      Set the meter burst sizes of a specific meter mode.
 * Input:
 *      unit        - unit id
 *      meterMode   - meter mode (byte based or packet based)
 *      pBurstSize  - pointer to burst sizes
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      In meterMode = METER_MODE_BYTE, the (minimum, maximum) of pBurstSize->dlb_lb0bs and
 *      pBurstSize->dlb_lb1bs setting range is (17, 65535).
 */
extern int32
dal_cypress_acl_meterBurstSize_set(
    uint32              unit,
    rtk_acl_meterMode_t meterMode,
    rtk_acl_meterBurstSize_t  *pBurstSize);

/* Function Name:
 *      dal_cypress_acl_meterExceed_get
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
dal_cypress_acl_meterExceed_get(
    uint32  unit,
    uint32  meterIdx,
    uint32  *pIsExceed);

/* Function Name:
 *      dal_cypress_acl_meterExceedAggregation_get
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
dal_cypress_acl_meterExceedAggregation_get(
    uint32  unit,
    uint32  *pExceedMask);


/* Function Name:
 *      dal_cypress_acl_meterEntry_get
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
dal_cypress_acl_meterEntry_get(
    uint32          unit,
    uint32          meterIdx,
    rtk_acl_meterEntry_t   *pMeterEntry);

/* Function Name:
 *      dal_cypress_acl_meterEntry_set
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
dal_cypress_acl_meterEntry_set(
    uint32          unit,
    uint32          meterIdx,
    rtk_acl_meterEntry_t   *pMeterEntry);


/* Function Name:
 *      dal_cypress_acl_templateFieldIntentVlanTag_get
 * Description:
 *      Get the acl template field VLAN tag status
 * Input:
 *      unit     - unit id
 * Output:
 *      tagType  - template field VLAN tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Only work on template 0.
 */
extern int32
dal_cypress_acl_templateFieldIntentVlanTag_get(uint32 unit,
    rtk_vlan_tagType_t *tagType);

/* Function Name:
 *      dal_cypress_acl_templateFieldIntentVlanTag_set
 * Description:
 *      Set the acl template field VLAN tag status
 * Input:
 *      unit     - unit id
 *      tagType  - template field VLAN tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Only work on template 0.
 */
extern int32
dal_cypress_acl_templateFieldIntentVlanTag_set(uint32 unit,
    rtk_vlan_tagType_t tagType);

#endif /* __DAL_CYPRESS_ACL_H__ */


