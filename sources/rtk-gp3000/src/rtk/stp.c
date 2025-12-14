#include <rtk/init.h> 
#include <rtk/default.h> 
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
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


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/stp.h>
#include <rtk/init.h>
#include <dal/dal_mgmt.h>
/*
 * Symbol Definition
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
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize stp module before calling any stp APIs.
 */
int32
rtk_stp_init(void)
{
    int32   ret;
    if (NULL == RT_MAPPER->stp_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->stp_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stp_init */


/* Function Name:
 *      rtk_stp_mstpState_get
 * Description:
 *      Get port spanning tree state of the msti from the specified device.
 * Input:
 *      msti       - multiple spanning tree instance
 *      port       - port id
 * Output:
 *      pStpState - pointer buffer of spanning tree state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MSTI         - invalid msti
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. For single spanning tree mode, input CIST0 (msti=0).
 *      2. Spanning tree state as following
 *          - STP_STATE_DISABLED
 *          - STP_STATE_BLOCKING
 *          - STP_STATE_LEARNING
 *          - STP_STATE_FORWARDING
 */
int32
rtk_stp_mstpState_get(uint32 msti, rtk_port_t port, rtk_stp_state_t *pStpState)
{
    int32   ret;
    if (NULL == RT_MAPPER->stp_mstpState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->stp_mstpState_get( msti, port, pStpState);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stp_mstpState_get */


/* Function Name:
 *      rtk_stp_mstpState_set
 * Description:
 *      Set port spanning tree state of the msti to the specified device.
 * Input:
 *      msti      - multiple spanning tree instance
 *      port      - port id
 *      stpState  - spanning tree state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_MSTI       - invalid msti
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_MSTP_STATE - invalid spanning tree status
 * Note:
 *      1. For single spanning tree mode, input CIST0 (msti=0).
 *      2. Spanning tree state as following
 *          - STP_STATE_DISABLED
 *          - STP_STATE_BLOCKING
 *          - STP_STATE_LEARNING
 *          - STP_STATE_FORWARDING
 */
int32
rtk_stp_mstpState_set(uint32 msti, rtk_port_t port, rtk_stp_state_t stpState)
{
    int32   ret;
    if (NULL == RT_MAPPER->stp_mstpState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->stp_mstpState_set( msti, port, stpState);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_stp_mstpState_set */


