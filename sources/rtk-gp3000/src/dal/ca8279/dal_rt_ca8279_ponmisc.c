/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of PON misc API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON misc
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/rt/rt_ponmisc.h>
#include <hal/chipdef/ca8279/ca8279_def.h>
#include <dal/ca8279/dal_ca8279.h>
#include <dal/ca8279/dal_rt_ca8279_ponmisc.h>
#include <scfg.h>

/*
 * Symbol Definition
 */
static uint32 ponmisc_init = INIT_NOT_COMPLETED;

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
 *      dal_rt_ca8279_ponmisc_init
 * Description:
 *      Configure PON misc initial settings
 * Input:
 *      None.
 * Output:
 *      pPonMode    - current running PON mode
 *      pPonSpeed   - current running PON speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rt_ca8279_ponmisc_init(void)
{
    int32 ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"%s",__FUNCTION__);

    ponmisc_init = INIT_COMPLETED;

    return ret;
}/* end of dal_rt_ca8279_ponmisc_init */

/* Function Name:
 *      dal_rt_ca8279_ponmisc_mode_get
 * Description:
 *      Configure PON misc get settings
 * Input:
 *      None.
 * Output:
 *      pPonMode    - current running PON mode
 *      pPonSpeed   - current running PON speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_FEATURE_NOT_SUPPORTED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32
dal_rt_ca8279_ponmisc_mode_get(rt_ponmisc_ponMode_t *pPonMode,rt_ponmisc_ponSpeed_t *pPonSpeed)
{
    int32 ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    RT_PARAM_CHK((NULL == pPonMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPonSpeed), RT_ERR_NULL_POINTER);

    switch(CA_PON_MODE)
    {
        case ONU_PON_MAC_EPON_1G:
        case ONU_PON_MAC_EPON_D10G:
        case ONU_PON_MAC_EPON_BI10G:
            *pPonMode = RT_EPON_MODE;
            break;
        case ONU_PON_MAC_GPON:
        case ONU_PON_MAC_XGPON1:
        case ONU_PON_MAC_XGSPON:
            *pPonMode = RT_GPON_MODE;
            break;
        default:
            return RT_ERR_FEATURE_NOT_SUPPORTED;
    }

    switch(CA_PON_MODE)
    {
        case ONU_PON_MAC_EPON_1G:
        case ONU_PON_MAC_GPON:
            *pPonSpeed = RT_1G25G_SPEED;
            break;
        case ONU_PON_MAC_EPON_D10G:
        case ONU_PON_MAC_XGPON1:
            *pPonSpeed = RT_DN10G_SPEED;
            break;
        case ONU_PON_MAC_EPON_BI10G:
        case ONU_PON_MAC_XGSPON:
            *pPonSpeed = RT_BOTH10G_SPEED;
            break;
        default:
            return RT_ERR_FEATURE_NOT_SUPPORTED;
    }

    return ret;
}/* end of dal_rt_ca8279_ponmisc_mode_get */
