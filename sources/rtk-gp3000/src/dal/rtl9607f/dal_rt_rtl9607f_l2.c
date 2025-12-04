/*
 * Copyright (C) 2020 Realtek Semiconductor Corp.
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
 * Purpose : Definition of L2  API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IVL/SVL mode
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/rtl9607f/rtl9607f_def.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_l2.h>

#include <aal_l2_vlan.h>


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



/* Function Name:
 *      dal_rt_l2_ivlSvl_get
 * Description:
 *      Get L2 IVL SVL mode
 
 * Input:
 *      None.
 * Output:
 *      pIvlEn    - IVL is enable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32
dal_rt_rtl9607f_l2_ivlSvl_get(rt_enable_t *pIvlEn)
{
    int32 ret = RT_ERR_OK;
    aal_l2_vlan_default_cfg_t def_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    RT_PARAM_CHK((NULL == pIvlEn), RT_ERR_NULL_POINTER);

    ret = aal_l2_vlan_default_cfg_get(0, &def_cfg);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;

    }

    *pIvlEn = (def_cfg.vlan_mode==0)?1:0;

    return RT_ERR_OK;
}/* end of dal_rt_rtl9607f_l2_ivlSvl_get */

/* Function Name:
 *      dal_rt_rtl9607f_l2_ivlSvl_set
 * Description:
 *      Set L2 IVL SVL mode
 * Input:
 *      ivlEn    - IVL is enable
 * Output:
 *      pPonQueueMode    - current running PON queue mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rt_rtl9607f_l2_ivlSvl_set(rt_enable_t ivlEn)
{
    int32 ret = RT_ERR_OK;
    aal_l2_vlan_default_cfg_mask_t def_cfg_msk;
    aal_l2_vlan_default_cfg_t def_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    memset(&def_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
    def_cfg_msk.u32 = 0;
    def_cfg_msk.s.vlan_mode = 1;
    def_cfg.vlan_mode = ivlEn?0:1;

    ret = aal_l2_vlan_default_cfg_set(0, def_cfg_msk, &def_cfg);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;

    }

    
    return ret;
}/* end of dal_rt_rtl9607f_l2_ivlSvl_set */

