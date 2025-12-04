/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision:  $
 * $Date: 2012-08-07
 *
 * Purpose : GPON MAC register access APIs
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_gpon.h>
#include <osal/time.h>

#include "cortina-api/include/gpon.h"
#include "cortina-api/include/port.h"
#include "aal_pon.h"
#include "aal_ploam.h"
#include "scfg.h"

uint32 orig_gpon_init = {INIT_NOT_COMPLETED};

/* Function Name:
 *      dal_rtl9607f_gpon_init
 * Description:
 *      gpon register level initial function
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9607f_gpon_init(void)
{
    int32 ret=RT_ERR_OK;
    ca_uint32_t pon_mode = CA_PON_MODE;
    
#ifdef CONFIG_CORTINA_BOARD_FPGA
    printk("%s %d FPGA gpon init\n",__FUNCTION__,__LINE__);
#else
    pon_mode = aal_pon_mac_mode_get(0);
    if(pon_mode != ONU_PON_MAC_GPON && pon_mode != ONU_PON_MAC_GPON_BI2G5 && pon_mode != ONU_PON_MAC_XGPON1 && pon_mode != ONU_PON_MAC_XGSPON)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        orig_gpon_init = INIT_NOT_COMPLETED;
        return RT_ERR_OK;
    }
#endif
    orig_gpon_init = INIT_COMPLETED;
    return RT_ERR_OK;    
}   /* end of dal_rtl9607f_gpon_init */


/* Function Name:
 *      dal_rtl9607f_gpon_onuState_set
 * Description:
 *      Set ONU State .
 * Input:
 *      onuState: onu state
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9607f_gpon_onuState_set(rtk_gpon_onuState_t  onuState)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "onuState=%d",onuState);

    /* check Init status */
    RT_INIT_CHK(orig_gpon_init);

    /* parameter check */
    RT_PARAM_CHK((GPON_STATE_END <=onuState), RT_ERR_INPUT);

    /* function body */

    return RT_ERR_OK;
}   /* end of   dal_rtl9607f_gpon_onuState_set */


/* Function Name:
 *      dal_rtl9607f_gpon_onuState_get
 * Description:
 *      Get ONU State
 * Input:
 *
 * Output:
 *      pOnuState: point for get onu state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9607f_gpon_onuState_get(rtk_gpon_onuState_t  *pOnuState)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(orig_gpon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pOnuState), RT_ERR_NULL_POINTER);

    /* function body */
    *pOnuState = GPON_STATE_O1;
	
    return RT_ERR_OK;
}   /* end of   dal_rtl9607f_gpon_onuState_get */

/* Function Name:
 *      dal_rtl9607f_gpon_password_set
 * Description:
 *      Set ONU PLOAM password/registration ID .
 * Input:
 *      rtk_gpon_password_t: password
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9607f_gpon_password_set(rtk_gpon_password_t  *pwd)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON), "");

    /* check Init status */
    RT_INIT_CHK(orig_gpon_init);

    /* function body */

    return RT_ERR_OK;
}   /* end of   dal_rtl9607f_gpon_password_set */
