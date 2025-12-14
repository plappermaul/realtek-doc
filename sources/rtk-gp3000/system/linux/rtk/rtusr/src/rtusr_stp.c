/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
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
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <rtk/stp.h>

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
    rtdrv_stpCfg_t stp_cfg;
    osal_memset(&stp_cfg, 0x0, sizeof(rtdrv_stpCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_STP_INIT, &stp_cfg, rtdrv_stpCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stp_init */


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
    rtdrv_stpCfg_t stp_cfg;
    osal_memset(&stp_cfg, 0x0, sizeof(rtdrv_stpCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pStpState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&stp_cfg.msti, &msti, sizeof(uint32));
    osal_memcpy(&stp_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_STP_MSTPSTATE_GET, &stp_cfg, rtdrv_stpCfg_t, 1);
    osal_memcpy(pStpState, &stp_cfg.stpState, sizeof(rtk_stp_state_t));

    return RT_ERR_OK;
}   /* end of rtk_stp_mstpState_get */


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
    rtdrv_stpCfg_t stp_cfg;
    osal_memset(&stp_cfg, 0x0, sizeof(rtdrv_stpCfg_t));

    /* function body */
    osal_memcpy(&stp_cfg.msti, &msti, sizeof(uint32));
    osal_memcpy(&stp_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&stp_cfg.stpState, &stpState, sizeof(rtk_stp_state_t));
    SETSOCKOPT(RTDRV_STP_MSTPSTATE_SET, &stp_cfg, rtdrv_stpCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_stp_mstpState_set */
