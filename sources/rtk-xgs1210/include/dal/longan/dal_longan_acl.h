/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2009
 * All rights reserved.
 *
 * $Revision: 96094 $
 * $Date: 2019-03-29 17:59:48 +0800 (Fri, 29 Mar 2019) $
 *
 * Purpose : Definition those public ACL APIs and its data type in the SDK .
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Ingress ACL
 *            2) Egress ACL
 *            3) Counter
 *
 */

#ifndef __DAL_LONGAN_ACL_H__
#define __DAL_LONGAN_ACL_H__

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
 *      dal_longan_aclMapper_init
 * Description:
 *      Hook acl module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook acl module before calling any acl APIs.
 */
extern int32
dal_longan_aclMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_longan_acl_init
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
dal_longan_acl_init(uint32 unit);

/* Function Name:
 *      dal_longan_acl_portPhaseLookupEnable_get
 * Description:
 *      Get the acl lookup state of the specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phase   - ACL lookup phase
 * Output:
 *      pEnable - pointer to lookup state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_acl_portPhaseLookupEnable_get(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 *pEnable);

/* Function Name:
 *      dal_longan_acl_portLookupEnable_set
 * Description:
 *      Set the acl lookup state of the specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phase   - ACL lookup phase
 *      enable  - lookup state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_acl_portLookupEnable_set(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 enable);

/* Function Name:
 *      dal_longan_acl_ruleEntryFieldSize_get
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
dal_longan_acl_ruleEntryFieldSize_get(uint32 unit, rtk_acl_fieldType_t type, uint32 *pField_size);

/* Function Name:
 *      dal_longan_acl_ruleEntrySize_get
 * Description:
 *      Get the rule entry size of ACL.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 * Output:
 *      pEntry_size - rule entry size of ACL entry
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
dal_longan_acl_ruleEntrySize_get(uint32 unit, rtk_acl_phase_t phase, uint32 *pEntry_size);

/* Function Name:
 *      dal_longan_acl_ruleValidate_get
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
dal_longan_acl_ruleValidate_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              *pValid);

/* Function Name:
 *      dal_longan_acl_ruleValidate_set
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
dal_longan_acl_ruleValidate_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              valid);

/* Function Name:
 *      dal_longan_acl_ruleEntry_read
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
dal_longan_acl_ruleEntry_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer);

/* Function Name:
 *      dal_longan_acl_ruleEntry_write
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
dal_longan_acl_ruleEntry_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer);

/* Function Name:
 *      dal_longan_acl_ruleEntryField_get
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
dal_longan_acl_ruleEntryField_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

/* Function Name:
 *      dal_longan_acl_ruleEntryField_set
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
dal_longan_acl_ruleEntryField_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

/* Function Name:
 *      dal_longan_acl_ruleEntryField_read
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
dal_longan_acl_ruleEntryField_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

/* Function Name:
 *      dal_longan_acl_ruleEntryField_write
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
dal_longan_acl_ruleEntryField_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask);

/* Function Name:
 *      dal_longan_acl_ruleEntryField_check
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
dal_longan_acl_ruleEntryField_check(uint32 unit, rtk_acl_phase_t phase,
        rtk_acl_fieldType_t type);

/* Function Name:
 *      dal_longan_acl_ruleOperation_get
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
dal_longan_acl_ruleOperation_get(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);

/* Function Name:
 *      dal_longan_acl_ruleOperation_set
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
dal_longan_acl_ruleOperation_set(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation);

/* Function Name:
 *      dal_longan_acl_ruleAction_get
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
dal_longan_acl_ruleAction_get(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction);

/* Function Name:
 *      dal_longan_acl_ruleAction_set
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
dal_longan_acl_ruleAction_set(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction);


/* Function Name:
 *      dal_longan_acl_loopBackEnable_get
 * Description:
 *      Set loopback state.
 * Input:
 *      unit    - unit id
 *      enable  - pointer to loopback state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_longan_acl_loopBackEnable_get(uint32 unit, uint32 *pEnable);


/* Function Name:
 *      dal_longan_acl_loopBackEnable_set
 * Description:
 *      Set loopback state.
 * Input:
 *      unit    - unit id
 *      enable  - loopback state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_longan_acl_loopBackEnable_set(uint32 unit, uint32 enable);


/* Function Name:
 *      dal_longan_acl_limitLoopbackTimes_get
 * Description:
 *      Get the loopback maximum times.
 * Input:
 *      unit    - unit id
 *      pLb_times  - pointer to loopback times
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_longan_acl_limitLoopbackTimes_get(uint32 unit, uint32 *pLb_times);


/* Function Name:
 *      dal_longan_acl_limitLoopbackTimes_set
 * Description:
 *      Set the loopback maximum times.
 * Input:
 *      unit    - unit id
 *      lb_times  - loopback times
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_longan_acl_limitLoopbackTimes_set(uint32 unit, uint32 lb_times);

/* Function Name:
 *      dal_longan_acl_portPhaseLookupEnable_get
 * Description:
 *      Get the acl phase lookup state of the specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phase   - ACL lookup phase
 * Output:
 *      pEnable - pointer to lookup state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_acl_portPhaseLookupEnable_get(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 *pEnable);

/* Function Name:
 *      dal_longan_acl_portPhaseLookupEnable_set
 * Description:
 *      Set the acl phase lookup state of the specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      phase   - ACL lookup phase
 *      enable  - lookup state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_acl_portPhaseLookupEnable_set(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 enable);

/* Function Name:
 *      dal_longan_acl_templateSelector_get
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
dal_longan_acl_templateSelector_get(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   *pTemplate_idx);

/* Function Name:
 *      dal_longan_acl_templateSelector_set
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
dal_longan_acl_templateSelector_set(
    uint32                  unit,
    uint32                  block_idx,
    rtk_acl_templateIdx_t   template_idx);

/* Function Name:
 *      dal_longan_acl_ruleHitIndication_get
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
dal_longan_acl_ruleHitIndication_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              reset,
    uint32              *pIsHit);

/* Function Name:
 *      dal_longan_acl_rule_del
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
dal_longan_acl_rule_del(uint32 unit, rtk_acl_phase_t phase, rtk_acl_clear_t *pClrIdx);

/* Function Name:
 *      dal_longan_acl_rule_move
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
dal_longan_acl_rule_move(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pData);

/* Function Name:
 *      dal_longan_acl_statCnt_get
 * Description:
 *      Get packet-based or byte-based statistic counter of the log id.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      entryidx    - logic entry index
 *      mode        - statistic counter mode
 * Output:
 *      pCnt - pointer buffer of count
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
dal_longan_acl_statCnt_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_statMode_t mode, uint64 *pCnt);

/* Function Name:
 *      dal_longan_acl_statCnt_clear
 * Description:
 *      Clear statistic counter of the log id.
 * Input:
 *      unit    - unit id
 *      phase       - ACL lookup phase
 *      entryidx    - logic entry index
 *      mode        - statistic counter mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Note:
 *      None
 */
extern int32
dal_longan_acl_statCnt_clear(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_statMode_t mode);

/* Function Name:
 *      dal_longan_acl_statCnt_clearAll
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
dal_longan_acl_statCnt_clearAll(uint32 unit);

/* Function Name:
 *      dal_longan_acl_ruleEntryField_validate
 * Description:
 *      Check the field if validate for entry.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      type      - field type
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
 *      None.
 */
extern int32
dal_longan_acl_ruleEntryField_validate(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_fieldType_t type);

/* Function Name:
 *      dal_longan_acl_fieldUsr2Template_get
 * Description:
 *      Get template field ID from user field ID.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      type        - user field ID
 * Output:
 *      info        - template field ID list
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Note:
 *      None
 */
extern int32
dal_longan_acl_fieldUsr2Template_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_fieldType_t type, rtk_acl_fieldUsr2Template_t *info);

/* Function Name:
 *      dal_longan_acl_templateId_get
 * Description:
 *      Get physic template Id from entry idx.
 * Input:
 *      unit         - unit id
 *      phase        - ACL lookup phase
 *      entry_idx    - ACL entry index
 * Output:
 *      pTemplate_id - pointer to template id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Note:
 *      None
 */
extern int32
dal_longan_acl_templateId_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 *pTemplate_id);

#endif /* __DAL_LONGAN_ACL_H__ */


