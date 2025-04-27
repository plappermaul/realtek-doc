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
 * Purpose : Definition of STP API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) 802.1D/w/s, STP/RSTP/MSTP
 *
 */

#ifndef __RTK_STP_H__
#define __RTK_STP_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */

/* spanning tree state */
typedef enum rtk_stp_state_e
{
    STP_STATE_DISABLED = 0,
    STP_STATE_BLOCKING,
    STP_STATE_LEARNING,
    STP_STATE_FORWARDING,
    STP_STATE_END
} rtk_stp_state_t;

/* multiple spanning tree instance mode */
typedef enum rtk_stp_mstiMode_e
{
    STP_MSTI_MODE_NORMAL = 0,   /* MSTI from VLAN table */
    STP_MSTI_MODE_CIST,         /* MSTI is always 0 */
    STP_MSTI_MODE_END
} rtk_stp_mstiMode_t;


/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : STP */

/* Function Name:
 *      rtk_stp_init
 * Description:
 *      Initialize stp module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize stp module before calling any stp APIs.
 * Changes:
 *      None
 */
extern int32
rtk_stp_init(uint32 unit);

/* Function Name:
 *      rtk_stp_mstpInstance_create
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
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_MSTI       - invalid msti
 *      RT_ERR_MSTI_EXIST - MSTI is already exist.
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of msti is 0~255 in 8390; 0~63 in 8380
 * Changes:
 *      None
 */
extern int32
rtk_stp_mstpInstance_create(uint32 unit, uint32 msti);

/* Function Name:
 *      rtk_stp_mstpInstance_destroy
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
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_MSTI           - invalid msti
 *      RT_ERR_MSTI_NOT_EXIST - msti is not exist
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of msti is 0~255 in 8390; 0~63 in 8380
 * Changes:
 *      None
 */
extern int32
rtk_stp_mstpInstance_destroy(uint32 unit, uint32 msti);

/* Function Name:
 *      rtk_stp_isMstpInstanceExist_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MSTI         - invalid msti
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of msti is 0~255 in 8390; 0~63 in 8380
 *      (2) The pMsti_exist value as following:
 *          - 0: this mstp instance not exist
 *          - 1: this mstp instance exist
 * Changes:
 *      None
 */
extern int32
rtk_stp_isMstpInstanceExist_get(uint32 unit, uint32 msti, uint32 *pMsti_exist);

/* Function Name:
 *      rtk_stp_mstpState_get
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
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_MSTI           - invalid msti
 *      RT_ERR_MSTI_NOT_EXIST - msti is not exist
 *      RT_ERR_PORT_ID        - invalid port id
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) For single spanning tree mode, input CIST0 (msti=0).
 *      (2) The valid range of msti is 0~255 in 8390; 0~63 in 8380
 *      (3) Spanning tree state as following
 *          - STP_STATE_DISABLED
 *          - STP_STATE_BLOCKING
 *          - STP_STATE_LEARNING
 *          - STP_STATE_FORWARDING
 * Changes:
 *      None
 */
extern int32
rtk_stp_mstpState_get(
    uint32          unit,
    uint32          msti,
    rtk_port_t      port,
    rtk_stp_state_t *pStp_state);

/* Function Name:
 *      rtk_stp_mstpState_set
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
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_MSTI           - invalid msti
 *      RT_ERR_MSTI_NOT_EXIST - msti is not exist
 *      RT_ERR_PORT_ID        - invalid port id
 *      RT_ERR_MSTP_STATE     - invalid spanning tree status
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) For single spanning tree mode, input CIST0 (msti=0).
 *      (2) The valid range of msti is 0~255 in 8390; 0~63 in 8380
 *      (3) Spanning tree state as following
 *          - STP_STATE_DISABLED
 *          - STP_STATE_BLOCKING
 *          - STP_STATE_LEARNING
 *          - STP_STATE_FORWARDING
 * Changes:
 *      None
 */
extern int32
rtk_stp_mstpState_set(
    uint32          unit,
    uint32          msti,
    rtk_port_t      port,
    rtk_stp_state_t stp_state);

/* Function Name:
 *      rtk_stp_mstpInstanceMode_get
 * Description:
 *      Get mstp instance source
 * Input:
 *      unit        - unit id
 * Output:
 *      pMsti_mode  - mstp instance mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      MSTI is either from VLAN table lookup or always be 0.
 * Changes:
 *      None
 */
extern int32
rtk_stp_mstpInstanceMode_get(uint32 unit, rtk_stp_mstiMode_t *pMsti_mode);

/* Function Name:
 *      rtk_stp_mstpInstanceMode_set
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
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      MSTI is either from VLAN table lookup or always be 0.
 * Changes:
 *      None
 */
extern int32
rtk_stp_mstpInstanceMode_set(uint32 unit, rtk_stp_mstiMode_t msti_mode);

#endif /* __RTK_STP_H__ */

