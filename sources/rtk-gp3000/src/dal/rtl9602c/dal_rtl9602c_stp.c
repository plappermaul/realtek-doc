/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (週五, 03 五月 2013) $
 *
 * Purpose : Definition of STP API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) 802.1D/w/s, STP/RSTP/MSTP
 *
 */



/*
 * Include Files
 */

#include <rtk/stp.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <dal/rtl9602c/dal_rtl9602c_stp.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32    stp_init = {INIT_NOT_COMPLETED};

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : STP */


/* Function Name:
 *      dal_rtl9602c_stp_init
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
dal_rtl9602c_stp_init(void)
{
    int32   ret;
    uint32  port, max_port;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STP),"%s",__FUNCTION__);

    stp_init = INIT_COMPLETED;

    max_port = HAL_GET_MAX_PORT();
    for (port = 0; port <= max_port; port++)
    {
        if (!HAL_IS_PORT_EXIST(port))
        {
            continue;
        }

        if (HAL_IS_CPU_PORT(port))
        {
            if ((ret = dal_rtl9602c_stp_mstpState_set(RTK_DEFAULT_MSTI, port, STP_STATE_FORWARDING)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_STP|MOD_DAL), "");
                stp_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
        else
        {
            if ((ret = dal_rtl9602c_stp_mstpState_set(RTK_DEFAULT_MSTI, port, RTK_DEFAULT_STP_PORT_STATE)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_STP|MOD_DAL), "");
                stp_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_stp_init */


/* Function Name:
 *      dal_rtl9602c_stp_mstpState_get
 * Description:
 *      Get port spanning tree state of the msti from the specified device.
 * Input:
 *      msti       - multiple spanning tree instance
 *      port       - port id
 * Output:
 *      pStp_state - pointer buffer of spanning tree state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MSTI         - invalid msti
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) For single spanning tree mode, input CIST0 (msti=0).
 *      (2) Spanning tree state as following
 *          - STP_STATE_DISABLED
 *          - STP_STATE_BLOCKING
 *          - STP_STATE_LEARNING
 *          - STP_STATE_FORWARDING
 */
int32
dal_rtl9602c_stp_mstpState_get(uint32 msti, rtk_port_t port, rtk_stp_state_t *pStp_state)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stp_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStp_state), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((msti >= HAL_MAX_NUM_OF_MSTI()), RT_ERR_MSTI);

    if((ret = reg_array_field_read(RTL9602C_MSTI_CTRLr, port , msti, RTL9602C_STATEf, (uint32 *) pStp_state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STP), "");
        return ret;
    }

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STP), "pStp_state=%x", *pStp_state);


    return RT_ERR_OK;
} /* end of dal_rtl9602c_stp_mstpState_get */


/* Function Name:
 *      dal_rtl9602c_stp_mstpState_set
 * Description:
 *      Set port spanning tree state of the msti to the specified device.
 * Input:
 *      msti      - multiple spanning tree instance
 *      port      - port id
 *      stp_state - spanning tree state
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
 *      (1) For single spanning tree mode, input CIST0 (msti=0).
 *      (2) Spanning tree state as following
 *          - STP_STATE_DISABLED
 *          - STP_STATE_BLOCKING
 *          - STP_STATE_LEARNING
 *          - STP_STATE_FORWARDING
 */
int32
dal_rtl9602c_stp_mstpState_set(uint32 msti, rtk_port_t port, rtk_stp_state_t stp_state)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stp_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((stp_state >= STP_STATE_END), RT_ERR_MSTP_STATE);
    RT_PARAM_CHK((msti >= HAL_MAX_NUM_OF_MSTI()), RT_ERR_MSTI);

    if((ret = reg_array_field_write(RTL9602C_MSTI_CTRLr, port , msti, RTL9602C_STATEf, (uint32 *) &stp_state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_stp_mstpState_set */


