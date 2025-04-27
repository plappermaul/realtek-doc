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
 * Purpose : Definition those public STP APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) spanning tree (1D, 1w and 1s)
 *
 */

#ifndef __DAL_MAPLE_STP_H__
#define __DAL_MAPLE_STP_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/stp.h>
#include <dal/dal_mgmt.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Module Name : STP */

/* Function Name:
 *      dal_maple_stpMapper_init
 * Description:
 *      Hook stp module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook stp module before calling any stp APIs.
 */
extern int32
dal_maple_stpMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_maple_stp_init
 * Description:
 *      Initialize stp module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize stp module before calling any stp APIs.
 */
extern int32
dal_maple_stp_init(uint32 unit);

/* Function Name:
 *      dal_maple_stp_mstpInstance_create
 * Description:
 *      Create one specified mstp instance of the specified device.
 * Input:
 *      unit - unit id
 *      msti - mstp instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_MSTI     - invalid msti
 *      RT_ERR_MSTI_EXIST - MSTI is already exist.
 * Note:
 *      The msti valid range is 0 .. HAL_MAX_NUM_OF_MSTI(unit)-1
 */
extern int32
dal_maple_stp_mstpInstance_create(uint32 unit, uint32 msti);

/* Function Name:
 *      dal_maple_stp_mstpInstance_destroy
 * Description:
 *      Destroy one specified mstp instance from the specified device.
 * Input:
 *      unit - unit id
 *      msti - mstp instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_MSTI           - invalid msti
 *      RT_ERR_MSTI_NOT_EXIST - msti is not exist
 * Note:
 *      The msti valid range is 0 .. RTK_STP_INSTANCE_ID_MAX-1
 */
extern int32
dal_maple_stp_mstpInstance_destroy(uint32 unit, uint32 msti);

/* Function Name:
 *      dal_maple_stp_isMstpInstanceExist_get
 * Description:
 *      Check one specified mstp instance is existing or not in the specified device.
 * Input:
 *      unit        - unit id
 *      msti        - mstp instance
 * Output:
 *      pMsti_exist - mstp instance exist or not?
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MSTI         - invalid msti
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The pMsti_exist value as following:
 *      0: this mstp instance not exist
 *      1: this mstp instance exist
 */
extern int32
dal_maple_stp_isMstpInstanceExist_get(uint32 unit, uint32 msti, uint32 *pMsti_exist);

/* Function Name:
 *      dal_maple_stp_mstpState_get
 * Description:
 *      Get port spanning tree state of the msti from the specified device.
 * Input:
 *      unit       - unit id
 *      msti       - multiple spanning tree instance
 *      port       - port id
 * Output:
 *      pStp_state - pointer buffer of spanning tree state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_MSTI           - invalid msti
 *      RT_ERR_PORT_ID        - invalid port id
 *      RT_ERR_MSTI_NOT_EXIST - MSTI is not exist
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Note:
 *      1) For single spanning tree mode, input CIST0 (msti=0).
 *      2) Spanning tree state as following
 *         - STP_STATE_DISABLED
 *         - STP_STATE_BLOCKING
 *         - STP_STATE_LEARNING
 *         - STP_STATE_FORWARDING
 */
extern int32
dal_maple_stp_mstpState_get(uint32 unit, uint32 msti, rtk_port_t port, rtk_stp_state_t *pStp_state);

/* Function Name:
 *      dal_maple_stp_mstpState_set
 * Description:
 *      Set port spanning tree state of the msti to the specified device.
 * Input:
 *      unit      - unit id
 *      msti      - multiple spanning tree instance
 *      port      - port id
 *      stp_state - spanning tree state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_MSTI           - invalid msti
 *      RT_ERR_PORT_ID        - invalid port id
 *      RT_ERR_MSTI_NOT_EXIST - MSTI is not exist
 *      RT_ERR_MSTP_STATE     - invalid spanning tree status
 * Note:
 *      1) For single spanning tree mode, input CIST0 (msti=0).
 *      2) Spanning tree state as following
 *         - STP_STATE_DISABLED
 *         - STP_STATE_BLOCKING
 *         - STP_STATE_LEARNING
 *         - STP_STATE_FORWARDING
 */
extern int32
dal_maple_stp_mstpState_set(uint32 unit, uint32 msti, rtk_port_t port, rtk_stp_state_t stp_state);

/* Function Name:
 *      dal_maple_stp_mstpInstanceMode_get
 * Description:
 *      Get mstp instance source
 * Input:
 *      unit        - unit id
 * Output:
 *      pMsti_mode  - mstp instance mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      MSTI is either from VLAN table lookup or always be 0.
 */
extern int32
dal_maple_stp_mstpInstanceMode_get(uint32 unit, rtk_stp_mstiMode_t *pMsti_mode);

/* Function Name:
 *      dal_maple_stp_mstpInstanceMode_set
 * Description:
 *      Set mstp instance source
 * Input:
 *      unit        - unit id
 *      msti_mode   - mstp instance mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      MSTI is either from VLAN table lookup or always be 0.
 */
extern int32
dal_maple_stp_mstpInstanceMode_set(uint32 unit, rtk_stp_mstiMode_t msti_mode);

#endif /* __DAL_MAPLE_STP_H__ */

