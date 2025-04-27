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

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/acl.h>

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

/* Module Name     : ACL */

/* Function Name:
 *      rtk_acl_init
 * Description:
 *      Initialize ACL module of the specified device.
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
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize ACL module before calling any ACL APIs.
 * Changes:
 *      None
 */
int32
rtk_acl_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_init(unit);
} /* end of rtk_acl_init */
/* Function Name:
 *      rtk_acl_portLookupEnable_get
 * Description:
 *      Get the acl per port lookup state.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      Change to use rtk_acl_portPhaseLookupEnable_get(unit, port, phase, pEnable) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_portLookupEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_portLookupEnable_get(unit, port, pEnable);
} /* end of rtk_acl_portLookupEnable_get */

/* Function Name:
 *      rtk_acl_portLookupEnable_set
 * Description:
 *      Set the acl per port lookup state.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      Change to use rtk_acl_portPhaseLookupEnable_set(unit, port, phase, enable) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_portLookupEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_portLookupEnable_set(unit, port, enable);
} /* end of rtk_acl_portLookupEnable_set */

/* Function Name:
 *      rtk_acl_lookupMissAct_get
 * Description:
 *      Get the acl per port default action when acl look up miss.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pLmAct - default action of acl look up miss
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_lookupMissAct_get(uint32 unit, rtk_port_t port, rtk_acl_lookupMissAct_t *pLmAct)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_lookupMissAct_get(unit, port, pLmAct);

} /* end of rtk_acl_lookupMissAct_get */

/* Function Name:
 *      rtk_acl_lookupMissAct_set
 * Description:
 *     Set the acl per port default action when acl look up miss.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      lmAct - default action of acl look up miss
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_lookupMissAct_set(uint32 unit, rtk_port_t port, rtk_acl_lookupMissAct_t lmAct)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_lookupMissAct_set(unit, port, lmAct);

} /* end of rtk_acl_lookupMissAct_set */

/* Function Name:
 *      rtk_acl_rangeCheckFieldSelector_get
 * Description:
 *      Get the configuration of field selector range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of field selector
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckFieldSelector_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_fieldSelector_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckFieldSelector_get(unit, index, pData);
} /* end of rtk_acl_rangeCheckFieldSelector_get */

/* Function Name:
 *      rtk_acl_rangeCheckFieldSelector_set
 * Description:
 *      Set the configuration of field selector range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of field selector
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
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckFieldSelector_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_fieldSelector_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckFieldSelector_set(unit, index, pData);
} /* end of rtk_acl_rangeCheckFieldSelector_set */

/* Function Name:
 *      rtk_acl_ruleEntryFieldSize_get
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
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The unit of size is bit.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleEntryFieldSize_get(uint32 unit, rtk_acl_fieldType_t type, uint32 *pField_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleEntryFieldSize_get(unit, type, pField_size);
} /* end of rtk_acl_ruleEntryFieldSize_get */

/* Function Name:
 *      rtk_acl_ruleEntrySize_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The unit of size is byte.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleEntrySize_get(uint32 unit, rtk_acl_phase_t phase, uint32 *pEntry_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleEntrySize_get(unit, phase, pEntry_size);
} /* end of rtk_acl_ruleEntrySize_get */

/* Function Name:
 *      rtk_acl_ruleValidate_get
 * Description:
 *      Validate ACL rule without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - entry index
 * Output:
 *      pValid    - pointer buffer of valid state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_ruleValidate_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              *pValid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleValidate_get(unit, phase, entry_idx, pValid);
} /* end of rtk_acl_ruleValidate_get */

/* Function Name:
 *      rtk_acl_ruleValidate_set
 * Description:
 *      Validate ACL rule without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - entry index
 *      valid     - valid state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ACL_PHASE   - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_ruleValidate_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint32              valid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleValidate_set(unit, phase, entry_idx, valid);
} /* end of rtk_acl_ruleValidate_set */

/* Function Name:
 *      rtk_acl_ruleEntry_read
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The API reads the rule data from chip to entry buffer. Use rtk_acl_ruleEntryField_get to
 *      get the field data from the entry buffer.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleEntry_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleEntry_read(unit, phase, entry_idx, pEntry_buffer);
} /* end of rtk_acl_ruleEntry_read */

/* Function Name:
 *      rtk_acl_ruleEntry_write
 * Description:
 *      Write the entry data to specified ACL entry.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - entry index
 *      pEntry_buffer - data buffer of ACL entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Use rtk_acl_ruleEntryField_set to fill the entry buffer and then use this API to write the
 *      entry buffer to the chip.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleEntry_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleEntry_write(unit, phase, entry_idx, pEntry_buffer);
} /* end of rtk_acl_ruleEntry_write */

/* Function Name:
 *      rtk_acl_ruleEntryField_get
 * Description:
 *      Get the field data from specified ACL entry buffer.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - ACL entry index
 *      pEntry_buffer - data buffer of ACL entry
 *      type          - field type
 * Output:
 *      pData         - pointer buffer of field data
 *      pMask         - pointer buffer of field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The API reads the field data/mask from the entry buffer. Use rtk_acl_ruleEntry_read to
 *      read the rule data to the entry buffer.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleEntryField_get(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleEntryField_get(unit, phase, entry_idx, pEntry_buffer, type, pData, pMask);
} /* end of rtk_acl_ruleEntryField_get */

/* Function Name:
 *      rtk_acl_ruleEntryField_set
 * Description:
 *      Set the field data to specified ACL entry buffer.
 * Input:
 *      unit          - unit id
 *      phase         - ACL lookup phase
 *      entry_idx     - ACL entry index
 *      pEntry_buffer - data buffer of ACL entry
 *      type          - field type
 *      pData         - pointer buffer of field data
 *      pMask         - pointer buffer of field mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The API writes the field data/mask to the entry buffer. After the fields are configured,
 *      use rtk_acl_ruleEntry_write to write the entry buffer to ASIC at once.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleEntryField_set(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    uint8               *pEntry_buffer,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleEntryField_set(unit, phase, entry_idx, pEntry_buffer, type, pData, pMask);
} /* end of rtk_acl_ruleEntryField_set */

/* Function Name:
 *      rtk_acl_ruleEntryField_read
 * Description:
 *      Read the field data from specified ACL entry.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      type      - field type
 * Output:
 *      pData     - pointer buffer of field data
 *      pMask     - pointer buffer of field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_ruleEntryField_read(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleEntryField_read(unit, phase, entry_idx, type, pData, pMask);
} /* end of rtk_acl_ruleEntryField_read */

/* Function Name:
 *      rtk_acl_ruleEntryField_write
 * Description:
 *      Write the field data to specified ACL entry.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      type      - field type
 *      pData     - pointer buffer of field data
 *      pMask     - pointer buffer of field mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX    - invalid entry index
 *      RT_ERR_ACL_FIELD_TYPE - invalid entry field type
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_ruleEntryField_write(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_id_t        entry_idx,
    rtk_acl_fieldType_t type,
    uint8               *pData,
    uint8               *pMask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleEntryField_write(unit, phase, entry_idx, type, pData, pMask);
} /* end of rtk_acl_ruleEntryField_write */

/* Function Name:
 *      rtk_acl_ruleOperation_get
 * Description:
 *      Get ACL rule operation.
 * Input:
 *      unit       - unit id
 *      phase      - ACL lookup phase
 *      entry_idx  - ACL entry index
 * Output:
 *      pOperation - operation configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) For reverse operation, valid index is N where N = 0,1,2...
 *      (2) For aggr_1 operation, index must be 2N where N = 0,1,2...
 *      (3) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,2...
 *      (4) For aggregating 4 rules, both aggr_1 and aggr_2 must be enabled.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleOperation_get(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleOperation_get(unit, phase, entry_idx, pOperation);
} /* end of rtk_acl_ruleOperation_get */

/* Function Name:
 *      rtk_acl_ruleOperation_set
 * Description:
 *      Set ACL rule operation.
 * Input:
 *      unit       - unit id
 *      phase      - ACL lookup phase
 *      entry_idx  - ACL entry index
 *      pOperation - operation configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) For reverse operation, valid index is N where N = 0,1,2...
 *      (2) For aggr_1 operation, index must be 2N where N = 0,1,2...
 *      (3) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,2...
 *      (4) For aggregating 4 rules, both aggr_1 and aggr_2 must be enabled.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleOperation_set(
    uint32                  unit,
    rtk_acl_phase_t         phase,
    rtk_acl_id_t            entry_idx,
    rtk_acl_operation_t     *pOperation)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleOperation_set(unit, phase, entry_idx, pOperation);
} /* end of rtk_acl_ruleOperation_set */

/* Function Name:
 *      rtk_acl_ruleAction_get
 * Description:
 *      Get the ACL rule action configuration.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 * Output:
 *      pAction   - action mask and data configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      [8380/8390]
 *          For ingress ACL phase, the data is put to pAction->igr_acl.
 *          For egress ACL phase, the data is put to pAction->egr_acl.
 *      [9300/9310]
 *          For VACL phase, the data is put to pAction->vact.
 *          For IACL phase, the data is put to pAction->iact.
 *          For EACL phase, the data is put to pAction->eact.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleAction_get(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleAction_get(unit, phase, entry_idx, pAction);
} /* end of rtk_acl_ruleAction_get */

/* Function Name:
 *      rtk_acl_ruleAction_set
 * Description:
 *      Set the ACL rule action configuration.
 * Input:
 *      unit      - unit id
 *      phase     - ACL lookup phase
 *      entry_idx - ACL entry index
 *      pAction   - action mask and data configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ACL_PHASE    - invalid ACL phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      [8380/8390]
 *          For ingress ACL phase, the data is put to pAction->igr_acl.
 *          For egress ACL phase, the data is put to pAction->egr_acl.
 *      [9300/9310]
 *          For VACL phase, the data is put to pAction->vact.
 *          For IACL phase, the data is put to pAction->iact.
 *          For EACL phase, the data is put to pAction->eact.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleAction_set(
    uint32               unit,
    rtk_acl_phase_t      phase,
    rtk_acl_id_t         entry_idx,
    rtk_acl_action_t     *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleAction_set(unit, phase, entry_idx, pAction);
} /* end of rtk_acl_ruleAction_set */

/* Function Name:
 *      rtk_acl_blockGroupEnable_get
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit       - unit id
 *      block_idx  - block index
 *      group_type - grouping type
 * Output:
 *      pEnable    - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group ingress ACL block with egress ACL block is forbidden.
 *      (3) For ACL_BLOCK_GROUP_2, valid index is 2N where N = 0,1...
 *      (4) For ACL_BLOCK_GROUP_4, valid index is 4N where N = 0,1...
 *      (5) For ACL_BLOCK_GROUP_8, valid index is 8N where N = 0,1...
 *      (6) For ACL_BLOCK_GROUP_ALL, valid index is 0.
 *      (7) If multiple grouping types are applied to the same block index, then
 *          the priority will be ACL_BLOCK_GROUP_ALL > ACL_BLOCK_GROUP_8 >
 *          ACL_BLOCK_GROUP_4 > ACL_BLOCK_GROUP_2.
 *      (8) ACL_BLOCK_GROUP_1 is applicable to 8380 only.
 *      (9) Change to use rtk_pie_blockGrouping_get(unit, block_idx, group_id) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_blockGroupEnable_get(
    uint32                     unit,
    uint32                     block_idx,
    rtk_acl_blockGroup_t       group_type,
    rtk_enable_t               *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_blockGroupEnable_get(unit, block_idx, group_type, pEnable);
} /* end of rtk_acl_blockGroupEnable_get */

/* Function Name:
 *      rtk_acl_blockGroupEnable_set
 * Description:
 *      Set the block grouping.
 * Input:
 *      unit       - unit id
 *      block_idx  - block index
 *      group_type - grouping type
 *      enable     - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) If multiple physical blocks are grouped to a logical block,
 *          it only outputs a single hit result and the hit result will be
 *          the entry with lowest index.
 *      (2) Group ingress ACL block with egress ACL block is forbidden.
 *      (3) For ACL_BLOCK_GROUP_2, valid index is 2N where N = 0,1...
 *      (4) For ACL_BLOCK_GROUP_4, valid index is 4N where N = 0,1...
 *      (5) For ACL_BLOCK_GROUP_8, valid index is 8N where N = 0,1...
 *      (6) For ACL_BLOCK_GROUP_ALL, valid index is 0.
 *      (7) If multiple grouping types are applied to the same block index, then
 *          the priority will be ACL_BLOCK_GROUP_ALL > ACL_BLOCK_GROUP_8 >
 *          ACL_BLOCK_GROUP_4 > ACL_BLOCK_GROUP_2.
 *      (8) ACL_BLOCK_GROUP_1 is applicable to 8380 only.
 *      (9) Change to use rtk_pie_blockGrouping_set(unit, block_idx, group_id) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_blockGroupEnable_set(
    uint32                     unit,
    uint32                     block_idx,
    rtk_acl_blockGroup_t       group_type,
    rtk_enable_t               enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_blockGroupEnable_set(unit, block_idx, group_type, enable);
} /* end of rtk_acl_blockGroupEnable_set */

/* Function Name:
 *      rtk_acl_statPktCnt_get
 * Description:
 *      Get packet-based statistic counter of the log id.
 * Input:
 *      unit     - unit id
 *      log_id   - log id
 * Output:
 *      pPkt_cnt - pointer buffer of packet count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Change to use rtk_acl_statCnt_get(unit, phase, entryIdx, mode, pCnt) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_statPktCnt_get(uint32 unit, uint32 log_id, uint32 *pPkt_cnt)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_statPktCnt_get(unit, log_id, pPkt_cnt);
} /* end of rtk_acl_statPktCnt_get */

/* Function Name:
 *      rtk_acl_statPktCnt_clear
 * Description:
 *      Set packet-based statistic counter of the log id.
 * Input:
 *      unit   - unit id
 *      log_id - log id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Applicable:
 *      8380, 8390
 * Note:
 *      Change to use rtk_acl_statCnt_clear(unit, phase, entryIdx, mode) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_statPktCnt_clear(uint32 unit, uint32 log_id)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_statPktCnt_clear(unit, log_id);
} /* end of rtk_acl_statPktCnt_clear */

/* Function Name:
 *      rtk_acl_statByteCnt_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Change to use rtk_acl_statCnt_get(unit, phase, entryIdx, mode, pCnt) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_statByteCnt_get(uint32 unit, uint32 log_id, uint64 *pByte_cnt)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_statByteCnt_get(unit, log_id, pByte_cnt);
} /* end of rtk_acl_statByteCnt_get */

/* Function Name:
 *      rtk_acl_statByteCnt_clear
 * Description:
 *      Reset byte-based statistic counter of the log id.
 * Input:
 *      unit   - unit id
 *      log_id - log id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Applicable:
 *      8380, 8390
 * Note:
 *      Change to use  rtk_acl_statCnt_clear(unit, phase, entryIdx, mode) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_statByteCnt_clear(uint32 unit, uint32 log_id)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_statByteCnt_clear(unit, log_id);
} /* end of rtk_acl_statByteCnt_clear */

/* Function Name:
 *      rtk_acl_stat_clearAll
 * Description:
 *      Clear all statistic counter for the specified device.
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
 *      8380, 8390
 * Note:
 *      Change to use  rtk_acl_statCnt_clear(unit, phase, entryIdx, mode) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_stat_clearAll(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_stat_clearAll(unit);
} /* end of rtk_acl_stat_clearAll */

/* Function Name:
 *      rtk_acl_rangeCheckL4Port_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Change to use rtk_pie_rangeCheck_get(unit, index, pData) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckL4Port_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckL4Port_get(unit, index, pData);
} /* end of rtk_acl_rangeCheckL4Port_get */

/* Function Name:
 *      rtk_acl_rangeCheckL4Port_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      Change to use rtk_pie_rangeCheck_set(unit, index, pData) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckL4Port_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_l4Port_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckL4Port_set(unit, index, pData);
} /* end of rtk_acl_rangeCheckL4Port_set */

/* Function Name:
 *      rtk_acl_rangeCheckVid_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) For untag packet, P-VID is used.
 *      (2) Change to use rtk_pie_rangeCheck_get(unit, index, pData) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckVid_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckVid_get(unit, index, pData);
} /* end of rtk_acl_rangeCheckVid_get */

/* Function Name:
 *      rtk_acl_rangeCheckVid_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) For untag packet, P-VID is used.
 *      (2) Change to use rtk_pie_rangeCheck_get(unit, index, pData) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckVid_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_vid_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckVid_set(unit, index, pData);
} /* end of rtk_acl_rangeCheckVid_set */

/* Function Name:
 *      rtk_acl_rangeCheckSrcPort_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Not support for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckSrcPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckSrcPort_get(unit, index, pData);

} /* end of rtk_acl_rangeCheckSrcPort_get */

/* Function Name:
 *      rtk_acl_rangeCheckSrcPort_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Not support for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckSrcPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckSrcPort_set(unit, index, pData);

} /* end of rtk_acl_rangeCheckSrcPort_set */

/* Function Name:
 *      rtk_acl_rangeCheckPacketLen_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Packet length includes CRC(4Byte)
 *      (2) Change to use rtk_pie_rangeCheck_get(unit, index, pData) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckPacketLen_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckPacketLen_get(unit, index, pData);
} /* end of rtk_acl_rangeCheckPacketLen_get */

/* Function Name:
 *      rtk_acl_rangeCheckPacketLen_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Packet length includes CRC(4Byte)
 *      (2) Change to use rtk_pie_rangeCheck_set(unit, index, pData) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckPacketLen_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_packetLen_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckPacketLen_set(unit, index, pData);
} /* end of rtk_acl_rangeCheckPacketLen_set */

/* Function Name:
 *      rtk_acl_ruleEntryField_check
 * Description:
 *      Check whether the specified field type is supported on the chip.
 * Input:
 *      unit  - unit id
 *      phase - ACL lookup phase
 *      type  - field type to be checked
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_ACL_PHASE      - invalid ACL phase
 *      RT_ERR_ACL_FIELD_TYPE - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_ruleEntryField_check(
    uint32              unit,
    rtk_acl_phase_t     phase,
    rtk_acl_fieldType_t type)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleEntryField_check(unit, phase, type);
} /* end of rtk_acl_ruleEntryField_check */
/* Function Name:
 *      rtk_acl_blockPwrEnable_get
 * Description:
 *      Get the acl block power state.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 * Output:
 *      pEnable   - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_ACL_BLOCK_INDEX - block index is out of range
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The rule data are cleared if the block power is disabled.
 *      (2) Not support, this feature is always ENABLED for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_blockPwrEnable_get(uint32 unit, uint32 block_idx, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_blockPwrEnable_get(unit, block_idx, pEnable);
} /* end of rtk_acl_blockPwrEnable_get */

/* Function Name:
 *      rtk_acl_blockPwrEnable_set
 * Description:
 *      Set the acl block power state.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 *      enable    - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_ACL_BLOCK_INDEX - block index is out of range
 *      RT_ERR_INPUT           - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The rule data are cleared if the block power is disabled.
 *      (2) Not support, this feature is always ENABLED for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_blockPwrEnable_set(uint32 unit, uint32 block_idx, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_blockPwrEnable_set(unit, block_idx, enable);
} /* end of rtk_acl_blockPwrEnable_set */

/* Function Name:
 *      rtk_acl_meterMode_get
 * Description:
 *      Get the meter mode of a specific meter block.
 * Input:
 *      unit       - unit id
 *      idx        - meter block ID for 8390, meter entry ID for 8380
 * Output:
 *      pMeterMode - meter mode:byte based or packet based
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Change to use rtk_pie_meterEntry_get(unit, meterIdx, pMeterEntry) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_meterMode_get(
    uint32              unit,
    uint32              idx,
    rtk_acl_meterMode_t *pMeterMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_meterMode_get(unit, idx, pMeterMode);
} /* end of rtk_acl_meterMode_get */

/* Function Name:
 *      rtk_acl_meterMode_set
 * Description:
 *      Set the meter mode.
 * Input:
 *      unit      - unit id
 *      idx       - meter block ID for 8390, meter entry ID for 8380
 *      meterMode - meter mode (byte based or packet based)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 *      RT_ERR_INPUT       - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      Change to use rtk_pie_meterEntry_set(unit, meterIdx, pMeterEntry) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_meterMode_set(
    uint32              unit,
    uint32              idx,
    rtk_acl_meterMode_t meterMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_meterMode_set(unit, idx, meterMode);
} /* end of rtk_acl_meterMode_set */

/* Function Name:
 *      rtk_acl_meterBurstSize_get
 * Description:
 *      Get the meter burst sizes of a specific meter mode.
 * Input:
 *      unit       - unit id
 *      meterMode  - meter mode
 * Output:
 *      pBurstSize - pointer to burst sizes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Double leaky packet/ single rate three color/ two rate three color type
 *          have separated burst size settings.
 *      (2) In meterMode = METER_MODE_BYTE of 8390, the (minimum, maximum) of pBurstSize->dlb_lb0bs and
 *          pBurstSize->dlb_lb1bs setting range is (17, 65535).
 *      (3) Change to use rtk_pie_meterEntry_get(unit, meterIdx, pMeterEntry) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_meterBurstSize_get(
    uint32                      unit,
    rtk_acl_meterMode_t         meterMode,
    rtk_acl_meterBurstSize_t    *pBurstSize)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_meterBurstSize_get(unit, meterMode, pBurstSize);
} /* end of rtk_acl_meterBurstSize_get */

/* Function Name:
 *      rtk_acl_meterBurstSize_set
 * Description:
 *      Set the meter burst sizes of a specific meter mode.
 * Input:
 *      unit       - unit id
 *      meterMode  - meter mode (byte based or packet based)
 *      pBurstSize - pointer to burst sizes
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Double leaky packet/ single rate three color/ two rate three color type
 *          have separated burst size settings.
 *      (2) In meterMode = METER_MODE_BYTE of 8390, the (minimum, maximum) of pBurstSize->dlb_lb0bs and
 *          pBurstSize->dlb_lb1bs setting range is (17, 65535).
 *      (3) Change to use rtk_pie_meterEntry_get(unit, meterIdx, pMeterEntry) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_meterBurstSize_set(
    uint32                      unit,
    rtk_acl_meterMode_t         meterMode,
    rtk_acl_meterBurstSize_t    *pBurstSize)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_meterBurstSize_set(unit, meterMode, pBurstSize);
} /* end of rtk_acl_meterBurstSize_set */

/* Function Name:
 *      rtk_acl_partition_get
 * Description:
 *      Get the acl partition configuration.
 * Input:
 *      unit       - unit id
 * Output:
 *      pPartition - pointer buffer of partition value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) Each block contains 128 ACL rules.
 *      (2) Assumed total block is Y. If partition value is set to X, means block 0~(X-1)
 *          are for ingress ACL while block X~(Y-1) are for egress acl.
 *          Thus, ingress ACL rule is assigned to logical index 0~(X*128-1), egress ACL rule
 *          is assigned to logical index 0~((Y-X)*128-1).
 *      (3) Change to use rtk_pie_phase_get(unit, block_idx, pPhase) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_partition_get(uint32 unit, uint32 *pPartition)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_partition_get(unit, pPartition);
} /* end of rtk_acl_partition_get */

/* Function Name:
 *      rtk_acl_partition_set
 * Description:
 *      Set the acl partition configuration.
 * Input:
 *      unit      - unit id
 *      partition - partition value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 * Applicable:
 *      8390
 * Note:
 *      (1) Each block contains 128 ACL rules.
 *      (2) Assumed total block is Y. If partition value is set to X, means block 0~(X-1)
 *          are for ingress ACL while block X~(Y-1) are for egress acl.
 *          Thus, ingress ACL rule is assigned to logical index 0~(X*128-1), egress ACL rule
 *          is assigned to logical index 0~((Y-X)*128-1).
 *      (3) Change to use rtk_pie_phase_set(unit, block_idx, pPhase) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_partition_set(uint32 unit, uint32 partition)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_partition_set(unit, partition);
} /* end of rtk_acl_partition_set */

/* Function Name:
 *      rtk_acl_blockResultMode_get
 * Description:
 *      Get the acl block result mode.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 * Output:
 *      pMode     - block result mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *     (1) If a packet hit multiple rules and the mode is configured to ACL_BLOCK_RESULT_SINGLE, then
 *      the hit result will be the rule with the lowest index.
 *     (2) Change to use rtk_pie_blockGrouping_set(unit, block_idx, group_id) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_blockResultMode_get(uint32 unit, uint32 block_idx, rtk_acl_blockResultMode_t *pMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_blockResultMode_get(unit, block_idx, pMode);
} /* end of rtk_acl_blockResultMode_get */

/* Function Name:
 *      rtk_acl_blockResultMode_set
 * Description:
 *      Set the acl block result mode.
 * Input:
 *      unit      - unit id
 *      block_idx - block index
 *      mode      - block result mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *     (1) If a packet hit multiple rules and the mode is configured to ACL_BLOCK_RESULT_SINGLE, then
 *      the hit result will be the rule with the lowest index.
 *     (2) Change to use rtk_pie_blockGrouping_set(unit, block_idx, group_id) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_blockResultMode_set(uint32 unit, uint32 block_idx, rtk_acl_blockResultMode_t mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_blockResultMode_set(unit, block_idx, mode);
} /* end of rtk_acl_blockResultMode_set */

/* Function Name:
 *      rtk_acl_templateFieldIntentVlanTag_get
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
 * Applicable:
 *      8390
 * Note:
 *      Change to use rtk_pie_templateVlanSel_get(unit, phase, preTemplate_idx, pVlanSel) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_templateFieldIntentVlanTag_get(uint32 unit,
    rtk_vlan_tagType_t *tagType)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_templateFieldIntentVlanTag_get(unit, tagType);
}   /* end of rtk_acl_templateFieldIntentVlanTag_get */

/* Function Name:
 *      rtk_acl_templateFieldIntentVlanTag_set
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
 * Applicable:
 *      8390
 * Note:
 *      Change to use rtk_pie_templateVlanSel_set(unit, phase, preTemplate_idx, vlanSel) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_acl_templateFieldIntentVlanTag_set(uint32 unit,
    rtk_vlan_tagType_t tagType)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_templateFieldIntentVlanTag_set(unit, tagType);
}   /* end of rtk_acl_templateFieldIntentVlanTag_set */

/* Function Name:
 *      rtk_acl_rangeCheckDstPort_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckDstPort_get(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckDstPort_get(unit, index, pData);
} /* end of rtk_acl_rangeCheckDstPort_get */

/* Function Name:
 *      rtk_acl_rangeCheckDstPort_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_rangeCheckDstPort_set(uint32 unit, uint32 index, rtk_acl_rangeCheck_portMask_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rangeCheckDstPort_set(unit, index, pData);
} /* end of rtk_acl_rangeCheckDstPort_set */

/* Function Name:
 *      rtk_acl_loopBackEnable_get
 * Description:
 *      Set loopback state.
 * Input:
 *      unit    - unit id
 *      pEnable  - pointer to loopback state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_acl_loopBackEnable_get(uint32 unit, uint32 *pEnable)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_loopBackEnable_get(unit, pEnable);
}   /* end of rtk_acl_loopBackEnable_get */

/* Function Name:
 *      rtk_acl_loopBackEnable_set
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
 * Applicable:
 *      9300
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_acl_loopBackEnable_set(uint32 unit, uint32 enable)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_loopBackEnable_set(unit, enable);
}   /* end of rtk_acl_loopBackEnable_set */

/* Function Name:
 *      rtk_acl_limitLoopbackTimes_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_acl_limitLoopbackTimes_get(uint32 unit, uint32 *pLb_times)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_limitLoopbackTimes_get(unit, pLb_times);
}   /* end of rtk_acl_limitLoopbackTimes_get */

/* Function Name:
 *      rtk_acl_limitLoopbackTimes_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_acl_limitLoopbackTimes_set(uint32 unit, uint32 lb_times){
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_limitLoopbackTimes_set(unit, lb_times);
}   /* end of rtk_acl_limitLoopbackTimes_set */

/* Function Name:
 *      rtk_acl_portPhaseLookupEnable_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_acl_portPhaseLookupEnable_get(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 *pEnable)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_portPhaseLookupEnable_get(unit, port, phase, pEnable);
}   /* end of rtk_acl_portPhaseLookupEnable_get */

/* Function Name:
 *      rtk_acl_portPhaseLookupEnable_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_acl_portPhaseLookupEnable_set(uint32 unit, rtk_port_t port,
    rtk_acl_phase_t phase, uint32 enable)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_portPhaseLookupEnable_set(unit, port, phase, enable);
}   /* end of rtk_acl_portPhaseLookupEnable_set */

/* Function Name:
 *      rtk_acl_templateSelector_get
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
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_templateSelector_get(uint32 unit, uint32 block_idx,
    rtk_acl_templateIdx_t *pTemplate_idx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_templateSelector_get(unit, block_idx, pTemplate_idx);
} /* end of rtk_acl_templateSelector_get */

/* Function Name:
 *      rtk_acl_templateSelector_set
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
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_ACL_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_ACL_BLOCK_INDEX      - invalid block index
 *      RT_ERR_INPUT                - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_acl_templateSelector_set(uint32 unit, uint32 block_idx,
    rtk_acl_templateIdx_t template_idx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_templateSelector_set(unit, block_idx, template_idx);
} /* end of rtk_acl_templateSelector_set */

/* Function Name:
 *      rtk_acl_statCnt_get
 * Description:
 *      Get packet-based or byte-based statistic counter of the log id.
 * Input:
 *      unit        - unit id
 *      phase       - ACL lookup phase
 *      entry_idx   - logic entry index
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_acl_statCnt_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_statMode_t mode, uint64 *pCnt)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_statCnt_get(unit, phase, entryIdx, mode, pCnt);
}   /* end of rtk_acl_statCnt_get */

/* Function Name:
 *      rtk_acl_statCnt_clear
 * Description:
 *      Clear statistic counter of the log id.
 * Input:
 *      unit        - unit id
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_acl_statCnt_clear(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entryIdx, rtk_acl_statMode_t mode)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_statCnt_clear(unit, phase, entryIdx, mode);
}   /* end of rtk_acl_statCnt_clear */

/* Function Name:
 *      rtk_acl_ruleHitIndication_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleHitIndication_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 reset, uint32 *pIsHit)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleHitIndication_get(unit, phase, entry_idx, reset, pIsHit);
}   /* end of rtk_acl_ruleHitIndication_get */

/* Function Name:
 *      rtk_acl_ruleHitIndicationMask_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleHitIndicationMask_get(uint32 unit, rtk_acl_phase_t phase,
        uint32 reset, rtk_acl_hitMask_t *pHitMask)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleHitIndicationMask_get(unit, phase, reset, pHitMask);
}

/* Function Name:
 *      rtk_acl_rule_del
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Entry fields, operations and actions are all cleared.
 * Changes:
 *      None
 */
int32
rtk_acl_rule_del(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_clear_t *pClrIdx)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rule_del(unit, phase, pClrIdx);
}   /* end of rtk_acl_rule_del */

/* Function Name:
 *      rtk_acl_rule_move
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      1.Entry fields, operations and actions are all moved.
 *      2.The vacant entries due to movement are auto cleared to be invalid by H/W.
 *      3.(move_from + length) and (move_to + length) must less than or equal to the number of ACL rule
 * Changes:
 *      None
 */
int32
rtk_acl_rule_move(uint32 unit, rtk_acl_phase_t phase, rtk_acl_move_t *pData)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_rule_move(unit, phase, pData);
}   /* end of rtk_acl_rule_move */

/* Function Name:
 *      rtk_acl_ruleEntryField_validate
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
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None.
 * Changes:
 *      None
 */
int32
rtk_acl_ruleEntryField_validate(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, rtk_acl_fieldType_t type)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_ruleEntryField_validate(unit, phase, entry_idx, type);
}   /* end of rtk_acl_ruleEntryField_validate */

/* Function Name:
 *      rtk_acl_fieldUsr2Template_get
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
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
int32
rtk_acl_fieldUsr2Template_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_fieldType_t type, rtk_acl_fieldUsr2Template_t *info)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || NULL == RT_MGMT(unit), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_fieldUsr2Template_get(unit, phase, type, info);
}   /* end of rtk_acl_fieldUsr2Template_get */

/* Function Name:
 *      rtk_acl_templateId_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
int32
rtk_acl_templateId_get(uint32 unit, rtk_acl_phase_t phase,
    rtk_acl_id_t entry_idx, uint32 *pTemplate_id)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || NULL == RT_MGMT(unit), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->acl_templateId_get(unit, phase, entry_idx, pTemplate_id);
}   /* end of rtk_acl_templateId_get */

