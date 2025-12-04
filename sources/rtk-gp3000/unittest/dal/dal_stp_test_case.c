/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Test STP API
 *
 * Feature : STP API test
 *
 */

/*
 * Include Files
 */
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <rtk/stp.h>
#include <dal/dal_stp_test_case.h>
#include <hal/common/halctrl.h>
/*
 * Function Declaration
 */


/*
 * Function Declaration
 */

int32 dal_stp_mstpState_test(uint32 caseNo)
{
    int32  ret;
    uint32 port,msti;
    rtk_stp_state_t   getState,setState;

    /*skip not support feature*/
    msti = HAL_MAX_NUM_OF_MSTI()-1;
    if ((RT_ERR_DRIVER_NOT_FOUND == rtk_stp_mstpState_get(msti, 0, &getState))
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_stp_mstpState_set(msti, 0, STP_STATE_DISABLED))
        || (RT_ERR_DRIVER_NOT_FOUND == rtk_stp_init()))
    {
        return RT_ERR_OK;
    }

    /*error input check*/
    /*1. out of range*/
    /*port out of range*/
    setState = STP_STATE_DISABLED;
    if( rtk_stp_mstpState_set(msti,(HAL_GET_MAX_PORT()+1), setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if( rtk_stp_mstpState_get(msti,(HAL_GET_MAX_PORT()+1), &getState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*state out of range*/
    setState = STP_STATE_END;
    port = HAL_GET_MAX_PORT();
    if( rtk_stp_mstpState_set(msti, port, setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*msti out of range*/
    setState = STP_STATE_DISABLED;
    port = HAL_GET_MAX_PORT();
    msti = HAL_MAX_NUM_OF_MSTI();
    if( rtk_stp_mstpState_set(msti, port, setState) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*2. null pointer*/
    if( rtk_stp_mstpState_get(msti, HAL_GET_MAX_PORT(), NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    if (rtk_stp_init() != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    }

    HAL_SCAN_ALL_PORT(port)
    {
        for (setState = 0; setState < STP_STATE_END; setState++)
        {
            for (msti = 0; msti < HAL_MAX_NUM_OF_MSTI(); msti++)
            {
                /*set*/
                if( rtk_stp_mstpState_set(msti,port, setState) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }
                /*get*/
                if( rtk_stp_mstpState_get(msti, port, &getState) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d msti:%d port:%d\n",__FUNCTION__,__LINE__,msti,port);
                    return RT_ERR_FAILED;
                }

                /*compare*/
                if(setState != getState)
                {
                #if 1
                    osal_printf("\n[write]\n");
                    osal_printf("\n %d\n",setState);

                    osal_printf("\n[read]\n");
                    osal_printf("\n %d\n",getState);
                #endif
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}
