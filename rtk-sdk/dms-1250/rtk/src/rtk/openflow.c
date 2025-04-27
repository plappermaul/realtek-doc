/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2015
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public ACL APIs and its data type in the SDK .
 *
 * Feature : The file have include the following module and sub-modules
 *            1) OpenFlow
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/openflow.h>


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

/* Module Name     : OpenFlow */

/* Function Name:
 *      rtk_of_init
 * Description:
 *      Initialize OpenFlow module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Applicable:
 *      9310
 * Note:
 *      Must initialize OpenFlow module before calling any OpenFlow APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_init(unit);
} /* end of rtk_of_init */

/* Function Name:
 *      rtk_of_classifier_get
 * Description:
 *      Get OpenFlow classification mechanism.
 * Input:
 *      unit	- unit id
 *      type	- classifier type
 * Output:
 *      pData 	- classifier data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  	- invalid unit id
 *      RT_ERR_NOT_INIT 	- The module is not initial
 *      RT_ERR_INPUT    	- invalid input parameter
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_NULL_POINTER	- input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      OF_CLASSIFIER_TYPE_PORT: Packet from OpenFlow enabled port is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN: Packet from OpenFlow enabled VLAN is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN_AND_PORT: Send packet to OpenFlow Pipeline only if packet's corresponding
 *		OF_PORT_EN and OF_VLAN_EN are both enabled
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_classifier_get(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_classifier_get(unit, type, pData);
} /* end of rtk_of_classifier_get */

/* Function Name:
 *      rtk_of_classifier_set
 * Description:
 *      Set OpenFlow classification mechanism.
 * Input:
 *      unit	- unit id
 *      type	- classifier type
 *      data 	- classifier data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Applicable:
 *      9310
 * Note:
 *      OF_CLASSIFIER_TYPE_PORT: Packet from OpenFlow enabled port is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN: Packet from OpenFlow enabled VLAN is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN_AND_PORT: Send packet to OpenFlow Pipeline only if packet's corresponding
 *		OF_PORT_EN and OF_VLAN_EN are both enabled
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_classifier_set(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t data)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_classifier_set(unit, type, data);
} /* end of rtk_of_classifier_set */

/* Function Name:
 *      rtk_of_flowMatchFieldSize_get
 * Description:
 *      Get the match field size of flow entry.
 * Input:
 *      unit        - unit id
 *      type        - type of entry field
 * Output:
 *      pField_size - match field size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The unit of size is bit.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowMatchFieldSize_get(uint32 unit, rtk_of_matchfieldType_t type, uint32 *pField_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowMatchFieldSize_get(unit, type, pField_size);
} /* end of rtk_of_flowMatchFieldSize_get */

/* Function Name:
 *      rtk_of_flowEntrySize_get
 * Description:
 *      Get the flow entry size
 * Input:
 *      unit        - unit id
 *      phase       - Flow Table phase
 * Output:
 *      pEntry_size - flow entry size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) The unit of size is byte.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntrySize_get(uint32 unit, rtk_of_flowtable_phase_t phase, uint32 *pEntry_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntrySize_get(unit, phase, pEntry_size);
} /* end of rtk_of_flowEntrySize_get */

/* Function Name:
 *      rtk_of_flowEntryValidate_get
 * Description:
 *      Validate flow entry without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 * Output:
 *      pValid    - pointer buffer of valid state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryValidate_get(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		*pValid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryValidate_get(unit, phase, entry_idx, pValid);
} /* end of rtk_of_flowEntryValidate_get */

/* Function Name:
 *      rtk_of_flowEntryValidate_set
 * Description:
 *      Validate flow entry without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      valid     - valid state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     	- invalid unit id
 *      RT_ERR_NOT_INIT    	- The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryValidate_set(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		valid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryValidate_set(unit, phase, entry_idx, valid);
} /* end of rtk_of_flowEntryValidate_set */

/* Function Name:
 *      rtk_of_flowEntryFieldList_get
 * Description:
 *      Get supported match field list of the specified phase.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      type  - match field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryFieldList_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldList_t  *list)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryFieldList_get(unit, phase, list);
}   /* end of rtk_mango_of_flowEntryFieldList_get */

/* Function Name:
 *      rtk_of_flowEntryField_check
 * Description:
 *      Check whether the match field type is supported on the specified phase.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      type  - match field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_OF_FT_PHASE    - invalid Flow Table phase
 *      RT_ERR_OF_FIELD_TYPE  - invalid match field type
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldType_t  type)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryField_check(unit, phase, type);
} /* end of rtk_of_flowEntryField_check */

/* Function Name:
 *      rtk_of_flowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified phase and field ID.
 * Input:
 *      unit     - unit id
 *      phase    - Flow Table phase
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntrySetField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   field_id,
    rtk_of_setFieldType_t    type)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntrySetField_check(unit, phase, field_id, type);
} /* end of rtk_of_flowEntrySetField_check */

/* Function Name:
 *      rtk_of_flowEntryField_read
 * Description:
 *      Read match field data from specified flow entry.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      type      - match field type
 * Output:
 *      pData     - pointer buffer of field data
 *      pMask     - pointer buffer of field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID			- invalid unit id
 *      RT_ERR_NOT_INIT			- The module is not initial
 *      RT_ERR_OF_FT_PHASE		- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX		- invalid entry index
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER		- input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryField_read(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryField_read(unit, phase, entry_idx, type, pData, pMask);
} /* end of rtk_of_flowEntryField_read */


/* Function Name:
 *      rtk_of_flowEntryField_write
 * Description:
 *      Write match field data to specified flow entry.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      type      - match field type
 *      pData     - pointer buffer of field data
 *      pMask     - pointer buffer of field mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID			- invalid unit id
 *      RT_ERR_NOT_INIT			- The module is not initial
 *      RT_ERR_OF_FT_PHASE      - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX		- invalid entry index
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER		- input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryField_write(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryField_write(unit, phase, entry_idx, type, pData, pMask);
} /* end of rtk_of_flowEntryField_write */

/* Function Name:
 *      rtk_of_flowEntryOperation_get
 * Description:
 *      Get flow entry operation.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - flow entry index
 * Output:
 *      pOperation - operation configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) For reverse operation, valid index is N where N = 0,1,(2) ..
 *      (3) For aggr_1 operation, index must be 2N where N = 0,1,(2) ..
 *      (4) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,(2) ..
 *      (5) For aggregating 4 entries, both aggr_1 and aggr_2 must be enabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryOperation_get(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryOperation_get(unit, phase, entry_idx, pOperation);
} /* end of rtk_of_flowEntryOperation_get */

/* Function Name:
 *      rtk_of_flowEntryOperation_set
 * Description:
 *      Set flow entry operation.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - flow entry index
 *      pOperation - operation configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) For reverse operation, valid index is N where N = 0,1,(2) ..
 *      (3) For aggr_1 operation, index must be 2N where N = 0,1,(2) ..
 *      (4) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,(2) ..
 *      (5) For aggregating 4 entries, both aggr_1 and aggr_2 must be enabled.
 *      (6) Egress flow table 0 doesn't support aggr_1 operation.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryOperation_set(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryOperation_set(unit, phase, entry_idx, pOperation);
} /* end of rtk_of_flowEntryOperation_set */

/* Function Name:
 *      rtk_of_flowEntryInstruction_get
 * Description:
 *      Get the flow entry instruction configuration.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 * Output:
 *      pAction   - instruction mask and data configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryInstruction_get(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryInstruction_get(unit, phase, entry_idx, pData);
} /* end of rtk_of_flowEntryInstruction_get */

/* Function Name:
 *      rtk_of_flowEntryInstruction_set
 * Description:
 *      Set the flow entry instruction configuration.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      pData     - instruction mask and data configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryInstruction_set(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryInstruction_set(unit, phase, entry_idx, pData);
} /* end of rtk_of_flowEntryInstruction_set */

/* Function Name:
 *      rtk_of_flowEntryHitSts_get
 * Description:
 *      Get the flow entry hit status.
 * Input:
 *      unit        - unit id
 *      phase       - Flow Table phase
 *      entry_idx   - flow entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) The hit status can be cleared by configuring parameter 'reset' to (1)
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntryHitSts_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint32                   reset,
    uint32                   *pIsHit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntryHitSts_get(unit, phase, entry_idx, reset, pIsHit);
} /* end of rtk_of_flowEntryHitSts_get */

/* Function Name:
 *      rtk_of_flowEntry_del
 * Description:
 *      Delete the specified flow entries.
 * Input:
 *      unit    - unit id
 *      phase   - Flow Table phase
 *      pClrIdx - entry index to clear
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_OF_FT_PHASE     - invalid Flow Table phase
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_OF_CLEAR_INDEX  - end index is lower than start index
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Match fields, Operations, Priority and Instructions are all cleared.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntry_del(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowClear_t *pClrIdx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntry_del(unit,  phase, pClrIdx);
} /* end of rtk_of_flowEntry_del */

/* Function Name:
 *      rtk_of_flowEntry_move
 * Description:
 *      Move the specified flow entries.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      pData - movement info
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Match fields, Operations, Priority and Instructions are all moved.
 *      (3) The vacant entries due to movement are auto cleared to be invalid by H/W.
 *      (4) (move_from + length) and (move_to + length) must less than or equal to the number of flow entries for the specified phase.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowEntry_move(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowMove_t *pData)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowEntry_move(unit, phase, pData);
} /* end of rtk_of_flowEntry_move */

/* Function Name:
 *      rtk_of_ftTemplateSelector_get
 * Description:
 *      Get the mapping template of specific Flow Table block.
 * Input:
 *      unit          - unit id
 *      phase         - Flow Table phase
 *      block_idx     - block index
 * Output:
 *      pTemplate_idx - template index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OF_FT_PHASE      - invalid Flow Table phase
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_OF_BLOCK_PHASE   - The block is not belonged to Flow Table
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_ftTemplateSelector_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   *pTemplate_idx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_ftTemplateSelector_get(unit, phase, block_idx, pTemplate_idx);
} /* end of rtk_of_ftTemplateSelector_get */

/* Function Name:
 *      rtk_of_ftTemplateSelector_set
 * Description:
 *      Set the mapping template of specific Flow Table block.
 * Input:
 *      unit         - unit id
 *      phase        - Flow Table phase
 *      block_idx    - block index
 *      template_idx - template index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_INPUT                - invalid input parameter
 *      RT_ERR_OF_BLOCK_PHASE       - The block is not belonged to Flow Table
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_ftTemplateSelector_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   template_idx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_ftTemplateSelector_set(unit, phase, block_idx, template_idx);
} /* end of rtk_of_ftTemplateSelector_set */

/* Function Name:
 *      rtk_of_flowCntMode_get
 * Description:
 *      Get counter mode for specific flow counters.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 * Output:
 *      pMode     - counter mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Each flow entry has a counter mode configuration, a 36bit counter and a 42bit counter.
 *          In different counter mode, these two counters have different meaning as below:
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT: 36bit packet counter and 42bit byte counter
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH: 36bit packet counter and 42bit packet counter trigger threshold
 *          OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH: 42bit byte counter and 36bit byte counter trigger threshold
 *      (3) If mode is configured to OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH/OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          an interrupt is triggered when the packet/byte counter exceeds the packet/byte counter threshold for the first time.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowCntMode_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     *pMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowCntMode_get(unit, phase, entry_idx, pMode);
} /* end of rtk_of_flowCntMode_get */

/* Function Name:
 *      rtk_of_flowCntMode_set
 * Description:
 *      Set counter mode for specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      mode      - counter mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Each flow entry has a counter mode configuration, a 36bit counter and a 42bit counter.
 *          In different counter mode, these two counters have different meaning as below:
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT: 36bit packet counter and 42bit byte counter
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH: 36bit packet counter and 42bit packet counter trigger threshold
 *          OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH: 42bit byte counter and 36bit byte counter trigger threshold
 *      (3) If mode is configured to OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH/OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          an interrupt is triggered when the packet/byte counter exceeds the packet/byte counter threshold for the first time.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowCntMode_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowCntMode_set(unit, phase, entry_idx, mode);
} /* end of rtk_of_flowCntMode_set */

/* Function Name:
 *      rtk_of_flowCnt_get
 * Description:
 *      Get packet counter or byte counter of specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      type      - counter type
 * Output:
 *      pCnt      - pointer buffer of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_PACKET.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_BYTE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowCnt_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type,
    uint64                   *pCnt)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowCnt_get(unit, phase, entry_idx, type, pCnt);
} /* end of rtk_of_flowCnt_get */

/* Function Name:
 *      rtk_of_flowCnt_clear
 * Description:
 *      Clear counter of specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      type      - counter type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_OF_FT_PHASE - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 *      RT_ERR_INPUT       - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_PACKET.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_BYTE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowCnt_clear(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowCnt_clear(unit, phase, entry_idx, type);
} /* end of rtk_of_flowCnt_clear */

/* Function Name:
 *      rtk_of_flowCntThresh_get
 * Description:
 *      Get packet counter or byte counter interrupt trigger threshold of specific flow counter.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - entry index
 * Output:
 *      pThreshold - pointer buffer of interrupt trigger threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          threshold is the packet counter threshold.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          threshold is the byte counter threshold.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowCntThresh_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   *pThreshold)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowCntThresh_get(unit, phase, entry_idx, pThreshold);
} /* end of rtk_of_flowCntThresh_get */

/* Function Name:
 *      rtk_of_flowCntThresh_set
 * Description:
 *      Set packet counter or byte counter interrupt trigger threshold of specific flow counter.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - entry index
 *      threshold  - interrupt trigger threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          threshold is the packet counter threshold.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          threshold is the byte counter threshold.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowCntThresh_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   threshold)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowCntThresh_set(unit, phase, entry_idx, threshold);
} /* end of rtk_of_flowCntThresh_set */

/* Function Name:
 *      rtk_of_ttlExcpt_get
 * Description:
 *      Get action for invalid IP/MPLS TTL.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer buffer of action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) Invalid TTL checking is executed when applying a decrement TTL action to the packet.
 *      (2) Valid action is ACTION_DROP/ACTION_TRAP.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_ttlExcpt_get(uint32 unit, rtk_action_t *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_ttlExcpt_get(unit, pAction);
} /* end of rtk_of_ttlExcpt_get */

/* Function Name:
 *      rtk_of_ttlExcpt_set
 * Description:
 *      Set action for invalid IP/MPLS TTL.
 * Input:
 *      unit   - unit id
 *      action - action to take
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) Invalid TTL checking is executed when applying a decrement TTL action to the packet.
 *      (2) Valid action is ACTION_DROP/ACTION_TRAP.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_ttlExcpt_set(uint32 unit, rtk_action_t action)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_ttlExcpt_set(unit, action);
} /* end of rtk_of_ttlExcpt_set */

/* Function Name:
 *      rtk_of_maxLoopback_get
 * Description:
 *      Get maximum loopback times of openflow pipeline.
 * Input:
 *      unit   - unit id
 * Output:
 *      pTimes - pointer buffer of loopback times
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      Valid times ranges from 0~63. 0 and 63 means no limitation for loopback times.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_maxLoopback_get(uint32 unit, uint32 *pTimes)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_maxLoopback_get(unit, pTimes);
} /* end of rtk_of_maxLoopback_get */

/* Function Name:
 *      rtk_of_maxLoopback_set
 * Description:
 *      Set maximum loopback times of openflow pipeline.
 * Input:
 *      unit   - unit id
 *      times  - loopback times
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      Valid times ranges from 0~63. 0 and 63 means no limitation for loopback times.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_maxLoopback_set(uint32 unit, uint32 times)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_maxLoopback_set(unit, times);
} /* end of rtk_of_maxLoopback_set */

/* Function Name:
 *      rtk_of_l2FlowTblMatchField_get
 * Description:
 *      Get match field of L2 flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L2 flow table can lookup twice for a certain packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the hit flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowTblMatchField_get(uint32 unit, rtk_of_l2FlowTblMatchField_t *pField)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowTblMatchField_get(unit, pField);
} /* end of rtk_of_l2FlowTblMatchField_get */

/* Function Name:
 *      rtk_of_l2FlowTblMatchField_set
 * Description:
 *      Set match field of L2 flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L2 flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the hit flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowTblMatchField_set(uint32 unit, rtk_of_l2FlowTblMatchField_t field)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowTblMatchField_set(unit, field);
} /* end of rtk_of_l2FlowTblMatchField_set */

/* Function Name:
 *      rtk_of_l2FlowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified field ID.
 * Input:
 *      unit     - unit id
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowEntrySetField_check(unit, field_id, type);
} /* end of rtk_of_l2FlowTblMatchField_set */

/* Function Name:
 *      rtk_of_l2FlowEntry_get
 * Description:
 *      Get L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      pEntry  - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) The metadata in L2 flow entry is 6 bit width.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowEntry_get(uint32 unit, rtk_of_l2FlowEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowEntry_get(unit, pEntry);
} /* end of rtk_of_l2FlowEntry_get */

/* Function Name:
 *      rtk_of_l2FlowEntryNextValid_get
 * Description:
 *      Get next valid L2 flow entry from the specified device.
 * Input:
 *      unit         - unit id
 *      pScan_idx    - currently scan index of l2 flow table to get next.
 * Output:
 *      pEntry       - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE      - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND    - no next valid entry found
 * Applicable:
 *      9310
 * Note:
 *      (1) Input -1 for getting the first valid entry of l2 flow table.
 *      (2) The pScan_idx is both the input and output argument.
 *      (3) The pScan_idx must be multiple of 2.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowEntryNextValid_get(
    uint32               unit,
    int32                *pScan_idx,
    rtk_of_l2FlowEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowEntryNextValid_get(unit, pScan_idx, pEntry);
} /* end of rtk_of_l2FlowEntryNextValid_get */

/* Function Name:
 *      rtk_of_l2FlowEntry_add
 * Description:
 *      Add a L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 *      (3) The metadata in L2 flow entry is 6 bit width.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowEntry_add(uint32 unit, rtk_of_l2FlowEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowEntry_add(unit, pEntry);
} /* end of rtk_of_l2FlowEntry_add */

/* Function Name:
 *      rtk_of_l2FlowEntry_del
 * Description:
 *      Delete a L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowEntry_del(uint32 unit, rtk_of_l2FlowEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowEntry_del(unit, pEntry);
} /* end of rtk_of_l2FlowEntry_del */

/* Function Name:
 *      rtk_of_l2FlowEntry_delAll
 * Description:
 *      Delete all L2 flow entry.
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowEntry_delAll(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowEntry_delAll(unit);
} /* end of rtk_of_l2FlowEntry_delAll */

/* Function Name:
 *      rtk_of_l2FlowEntryHitSts_get
 * Description:
 *      Get the L2 flow entry hit status.
 * Input:
 *      unit        - unit id
 *      pEntry      - pointer buffer of entry data
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l2FlowEntry_t     *pEntry,
    uint32                   reset,
    uint32                   *pIsHit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowEntryHitSts_get(unit, pEntry, reset, pIsHit);
} /* end of rtk_of_l2FlowEntryHitSts_get */

/* Function Name:
 *      rtk_of_l2FlowTblHashAlgo_get
 * Description:
 *      Get hash algorithm of L2 flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 * Output:
 *      pAlgo   - pointer to hash algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L2 flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l2FlowTblMatchField_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowTblHashAlgo_get(unit, block, pAlgo);
} /* end of rtk_of_l2FlowTblHashAlgo_get */

/* Function Name:
 *      rtk_of_l2FlowTblHashAlgo_set
 * Description:
 *      Set hash algorithm of L2 flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 *      algo    - hash algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L2 flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l2FlowTblMatchField_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l2FlowTblHashAlgo_set(unit, block, algo);
} /* end of rtk_of_l2FlowTblHashAlgo_set */

/* Function Name:
 *      rtk_of_l3FlowTblPri_get
 * Description:
 *      Get precedence of L3 CAM-based and hash-based flow tables
 * Input:
 *      unit    - unit id
 * Output:
 *      pTable  - pointer to target table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 flow table is composed of one CAM-based and two hash-based flow tables.
 *      (2) L3 CAM-based/Hash-based flow tables are physically combo with L3 Prefix/L3 host tables respectively.
 *      (3) The precedence configuration takes effect if a packet hit both L3 CAM-based and L3 hash-based flow tables concurrently.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3FlowTblPri_get(uint32 unit, rtk_of_l3FlowTblList_t *pTable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3FlowTblPri_get(unit, pTable);
} /* end of rtk_of_l3FlowTblPri_get */

/* Function Name:
 *      rtk_of_l3FlowTblPri_set
 * Description:
 *      Set precedence of L3 CAM-based and hash-based flow tables
 * Input:
 *      unit    - unit id
 *      table   - target table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 flow table is composed of one CAM-based and two hash-based flow tables.
 *      (2) L3 CAM-based/Hash-based flow tables are physically combo with L3 Prefix/L3 host tables respectively.
 *      (3) The precedence configuration takes effect if a packet hit both L3 CAM-based and L3 hash-based flow tables concurrently.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3FlowTblPri_set(uint32 unit, rtk_of_l3FlowTblList_t table)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3FlowTblPri_set(unit, table);
} /* end of rtk_of_l3FlowTblPri_set */

/* Function Name:
 *      rtk_of_l3CamFlowTblMatchField_get
 * Description:
 *      Get match field of L3 CAM-based flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 CAM-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3CamFlowTblMatchField_get(uint32 unit, rtk_of_l3CamFlowTblMatchField_t *pField)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3CamFlowTblMatchField_get(unit, pField);
} /* end of rtk_of_l3CamFlowTblMatchField_get */

/* Function Name:
 *      rtk_of_l3CamFlowTblMatchField_set
 * Description:
 *      Set match field of L3 CAM-based flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 CAM-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3CamFlowTblMatchField_set(uint32 unit, rtk_of_l3CamFlowTblMatchField_t field)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3CamFlowTblMatchField_set(unit, field);
} /* end of rtk_of_l3CamFlowTblMatchField_set */

/* Function Name:
 *      rtk_of_l3HashFlowTblMatchField_get
 * Description:
 *      Get match field of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 Hash-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3HashFlowTblMatchField_get(uint32 unit, rtk_of_l3HashFlowTblMatchField_t *pField)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3HashFlowTblMatchField_get(unit, pField);
} /* end of rtk_of_l3HashFlowTblMatchField_get */

/* Function Name:
 *      rtk_of_l3HashFlowTblMatchField_set
 * Description:
 *      Set match field of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 Hash-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3HashFlowTblMatchField_set(uint32 unit, rtk_of_l3HashFlowTblMatchField_t field)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3HashFlowTblMatchField_set(unit, field);
} /* end of rtk_of_l3HashFlowTblMatchField_set */

/* Function Name:
 *      rtk_of_l3HashFlowTblHashAlgo_get
 * Description:
 *      Get hash algorithm of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 * Output:
 *      pAlgo   - pointer to hash algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 Hash-based flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l3HashFlowTblMatchField_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3HashFlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3HashFlowTblHashAlgo_get(unit, block, pAlgo);
} /* end of rtk_of_l3HashFlowTblHashAlgo_get */

/* Function Name:
 *      rtk_of_l3HashFlowTblHashAlgo_set
 * Description:
 *      Set hash algorithm of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 *      algo    - hash algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 Hash-based flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l3HashFlowTblMatchField_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3HashFlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3HashFlowTblHashAlgo_set(unit, block, algo);
} /* end of rtk_of_l3HashFlowTblHashAlgo_set */

/* Function Name:
 *      rtk_of_l3FlowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified field ID.
 * Input:
 *      unit     - unit id
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3FlowEntrySetField_check(unit, field_id, type);
} /* end of rtk_of_l3FlowEntrySetField_check */

/* Function Name:
 *      rtk_of_l3CamFlowEntry_get
 * Description:
 *      Get a L3 CAM-based flow entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - physical entry index
 * Output:
 *      pEntry      - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX      - invalid entry index
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv4 SIP+DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv6 SIP/DIP entry occupies three physical entries (index 3N & 3N+1 & 3N+2). Valid index is 3N.
 *          An IPv6 SIP+DIP entry occupies six physical entries (index 6N & 6N+1 & 6N+2 & 6N+3 & 6N+4 & 6N+5). Valid index is 6N.
 *      (2) The entry with lowest index is taken for multiple matching.
 *      (3) The metadata in L3 flow entry is 12 bit width.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3CamFlowEntry_get(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3CamFlowEntry_get(unit, entry_idx, pEntry);
} /* end of rtk_of_l3CamFlowEntry_get */

/* Function Name:
 *      rtk_of_l3CamFlowEntry_add
 * Description:
 *      Add a L3 CAM-based flow entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - physical entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_VALID_ENTRY_EXIST- Valid entry is existed
 *      RT_ERR_INPUT            - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv4 SIP+DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv6 SIP/DIP entry occupies three physical entries (index 3N & 3N+1 & 3N+2). Valid index is 3N.
 *          An IPv6 SIP+DIP entry occupies six physical entries (index 6N & 6N+1 & 6N+2 & 6N+3 & 6N+4 & 6N+5). Valid index is 6N.
 *      (2) The entry with lowest index is taken for multiple matching.
 *      (3) The metadata in L3 flow entry is 12 bit width.
 *      (4) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3CamFlowEntry_add(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3CamFlowEntry_add(unit, entry_idx, pEntry);
} /* end of rtk_of_l3CamFlowEntry_add */

/* Function Name:
 *      rtk_of_l3CamFlowEntry_del
 * Description:
 *      Delete the specified L3 CAM-based flow entry.
 * Input:
 *      unit      - unit id
 *      entry_idx - physical entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      To delete an IPv4 SIP/DIP entry, valid index is 2N.
 *      To delete an IPv4 SIP+DIP entry, valid index is 2N.
 *      To delete an IPv6 SIP/DIP entry, valid index is 3N.
 *      To delete an IPv6 SIP+DIP entry, valid index is 6N.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3CamFlowEntry_del(uint32 unit, uint32 entry_idx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3CamFlowEntry_del(unit, entry_idx);
} /* end of rtk_of_l3CamFlowEntry_del */

/* Function Name:
 *      rtk_of_l3CamFlowEntry_move
 * Description:
 *      Move the specified L3 CAM-based flow entries.
 * Input:
 *      unit    - unit id
 *      pClrIdx - entry index to clear
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *		In addition to entry data, hit status is also moved.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3CamFlowEntry_move(uint32 unit, rtk_of_flowMove_t *pClrIdx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3CamFlowEntry_move(unit, pClrIdx);
} /* end of rtk_of_l3CamFlowEntry_move */

/* Function Name:
 *      rtk_of_l3CamflowEntryHitSts_get
 * Description:
 *      Get the L3 Cam-based flow entry hit status.
 * Input:
 *      unit        - unit id
 *      entry_idx   - flow entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_ENTRY_INDEX      - invalid entry index
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3CamflowEntryHitSts_get(
    uint32    unit,
    uint32    entry_idx,
    uint32    reset,
    uint32    *pIsHit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3CamflowEntryHitSts_get(unit, entry_idx, reset, pIsHit);
} /* end of rtk_of_l3CamflowEntryHitSts_get */

/* Function Name:
 *      rtk_of_l3HashFlowEntry_get
 * Description:
 *      Add a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      pEntry  - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) The metadata in L3 flow entry is 12 bit width.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3HashFlowEntry_get(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3HashFlowEntry_get(unit, pEntry);
} /* end of rtk_of_l3HashFlowEntry_get */

/* Function Name:
 *      rtk_of_l3HashFlowEntryNextValid_get
 * Description:
 *      Get next valid L3 Hash-based flow entry from the specified device.
 * Input:
 *      unit         - unit id
 *      pScan_idx    - currently scan index of l3 hash-based flow table to get next.
 * Output:
 *      pEntry       - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE      - input parameter out of range
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND    - no next valid entry found
 * Applicable:
 *      9310
 * Note:
 *      (1) Input -1 for getting the first valid entry of l3 hash-based flow table.
 *      (2) The pScan_idx is both the input and output argument.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3HashFlowEntryNextValid_get(
    uint32                   unit,
    int32                    *pScan_idx,
    rtk_of_l3HashFlowEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3HashFlowEntryNextValid_get(unit, pScan_idx, pEntry);
} /* end of rtk_of_l3HashFlowEntryNextValid_get */

/* Function Name:
 *      rtk_of_l3HashFlowEntry_add
 * Description:
 *      Add a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_ENTRY_EXIST  - entry existed
 *      RT_ERR_TBL_FULL     - The table is full
 * Applicable:
 *      9310
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries.
 *          An IPv4 SIP+DIP entry occupies two physical entries.
 *          An IPv6 SIP/DIP entry occupies three physical entries.
 *          An IPv6 SIP+DIP entry occupies six physical entries.
 *      (2) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *          The metadata in L3 flow entry is 12 bit width.
 *      (3) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3HashFlowEntry_add(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3HashFlowEntry_add(unit, pEntry);
} /* end of rtk_of_l3HashFlowEntry_add */

/* Function Name:
 *      rtk_of_l3HashFlowEntry_del
 * Description:
 *      Delete a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3HashFlowEntry_del(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3HashFlowEntry_del(unit, pEntry);
} /* end of rtk_of_l3HashFlowEntry_del */

/* Function Name:
 *      rtk_of_l3HashFlowEntry_delAll
 * Description:
 *      Delete all L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3HashFlowEntry_delAll(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3HashFlowEntry_delAll(unit);
} /* end of rtk_of_l3HashFlowEntry_delAll */

/* Function Name:
 *      rtk_of_l3HashflowEntryHitSts_get
 * Description:
 *      Get the L3 Hash-based flow entry hit status.
 * Input:
 *      unit        - unit id
 *      pEntry      - pointer buffer of entry data
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
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
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l3HashflowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l3HashFlowEntry_t *pEntry,
    uint32                   reset,
    uint32                   *pIsHit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_l3HashflowEntryHitSts_get(unit, pEntry, reset, pIsHit);
} /* end of rtk_of_l3HashflowEntryHitSts_get */

/* Function Name:
 *      rtk_of_groupEntry_get
 * Description:
 *      Get a group entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 * Output:
 *      pEntry      - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) For group type OF_GROUP_TYPE_ALL, 'bucket_num' consecutive buckets index by 'bucket_id' are executed.
 *      (2) For group type OF_GROUP_TYPE_SELECT, only one of the consecutive buckets is executed.
 *      (3) For group type OF_GROUP_TYPE_INDIRECT, 'bucket_num' is not used.
 *      (4) In 9310, max. bucket_num is 128 and max. bucket_id is 8191.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_groupEntry_get(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_groupEntry_get(unit, entry_idx, pEntry);
} /* end of rtk_of_groupEntry_get */

/* Function Name:
 *      rtk_of_groupEntry_set
 * Description:
 *      Set a group entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      (1) For group type OF_GROUP_TYPE_ALL, 'bucket_num' consecutive buckets index by 'bucket_id' are executed.
 *      (2) For group type OF_GROUP_TYPE_SELECT, only one of the consecutive buckets is executed.
 *      (3) For group type INDIRECT, use OF_GROUP_TYPE_ALL with bucket_num=1.
 *      (4) In 9310, max. bucket_num is 128 and max. bucket_id is 8191.  Refer to rtk_of_actionBucket_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_groupEntry_set(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_groupEntry_set(unit, entry_idx, pEntry);
} /* end of rtk_of_groupEntry_set */

/* Function Name:
 *      rtk_of_groupTblHashPara_get
 * Description:
 *      Get parameters of the hash algorithm used by group entry type 'select'.
 * Input:
 *      unit    - unit id
 * Output:
 *      para    - pointer buffer of hash parameters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      Each parameter can be included/excluded separately.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_groupTblHashPara_get(uint32 unit, rtk_of_groupTblHashPara_t *para)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_groupTblHashPara_get(unit, para);
} /* end of rtk_of_groupTblHashPara_get */

/* Function Name:
 *      rtk_of_groupTblHashPara_set
 * Description:
 *      Set parameters of the hash algorithm used by group entry type 'select'.
 * Input:
 *      unit    - unit id
 *      para    - pointer buffer of hash parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      Each parameter can be included/excluded separately.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_groupTblHashPara_set(uint32 unit, rtk_of_groupTblHashPara_t *para)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_groupTblHashPara_set(unit, para);
} /* end of rtk_of_groupTblHashPara_set */

/* Function Name:
 *      rtk_of_actionBucket_get
 * Description:
 *      Get a action bucket entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The actino bucket(s) are refered by group entry. Refer to rtk_of_groupEntry_set.
 *      (2) There are 8192 action buckets supported in 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_actionBucket_get(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_actionBucket_get(unit, entry_idx, pEntry);
} /* end of rtk_of_actionBucket_get */

/* Function Name:
 *      rtk_of_actionBucket_set
 * Description:
 *      Set a action bucket entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The actino bucket(s) are refered by group entry. Refer to rtk_of_groupEntry_set.
 *      (2) There are 8192 action buckets supported in 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_actionBucket_set(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_actionBucket_set(unit, entry_idx, pEntry);
} /* end of rtk_of_actionBucket_set */

/* Function Name:
 *      rtk_of_trapTarget_get
 * Description:
 *      Get target device for trap packet.
 * Input:
 *      unit	- unit id
 * Output:
 *      pTarget - pointer to target device
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *		The API is for stacking system to specify the trap target. Trap target can be either local CPU or master CPU.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_trapTarget_get(unit, pTarget);
} /* end of rtk_of_trapTarget_get */

/* Function Name:
 *      rtk_of_trapTarget_set
 * Description:
 *      Set target device for trap packet.
 * Input:
 *      unit	- unit id
 *      target 	- target device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *		The API is for stacking system to specify the trap target. Trap target can be either local CPU or master CPU.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_trapTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_trapTarget_set(unit, target);
} /* end of rtk_of_trapTarget_set */

/* Function Name:
 *      rtk_of_tblMissAction_get
 * Description:
 *      Get table miss action of flow tables.
 * Input:
 *      unit	- unit id
 *      phase   - Flow Table phase
 * Output:
 *      pAct    - pointer to table miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *		None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_tblMissAction_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t *pAct)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_tblMissAction_get(unit, phase, pAct);
} /* end of rtk_of_tblMissAction_get */

/* Function Name:
 *      rtk_of_tblMissAction_set
 * Description:
 *      Set table miss action of flow tables.
 * Input:
 *      unit	- unit id
 *      phase   - Flow Table phase
 *      act     - table miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *		(1) OF_TBLMISS_ACT_FORWARD_NEXT_TBL isn't applicable for the last ingress flow table.
 *		(2) OF_TBLMISS_ACT_TRAP and OF_TBLMISS_ACT_FORWARD_NEXT_TBL aren't applicable for the egress flow table.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_tblMissAction_set(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t act)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_tblMissAction_set(unit, phase, act);
} /* end of rtk_of_tblMissAction_set */

/* Function Name:
 *      rtk_of_flowTblCnt_get
 * Description:
 *      Get lookup and matched packet counters of flow tables.
 * Input:
 *      unit     - unit id
 *      phase    - Flow Table phase
 *      type     - counter type
 * Output:
 *      pCnt     - pointer buffer of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The counter is cleared after reading.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_flowTblCnt_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowTblCntType_t type, uint32 *pCnt)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->of_flowTblCnt_get(unit, phase, type, pCnt);
} /* end of rtk_of_flowTblCnt_get */
