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
#include <hal/chipdef/rtl8277c/rtl8277c_def.h>
#include <dal/rtl8277c/dal_rtl8277c.h>
#include <dal/rtl8277c/dal_rt_rtl8277c_l2.h>

#include <aal_l2_vlan.h>
#include <aal_mcast.h>


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
dal_rt_rtl8277c_l2_ivlSvl_get(rt_enable_t *pIvlEn)
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
}/* end of dal_rt_rtl8277c_l2_ivlSvl_get */

/* Function Name:
 *      dal_rt_rtl8277c_l2_ivlSvl_set
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
dal_rt_rtl8277c_l2_ivlSvl_set(rt_enable_t ivlEn)
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
}/* end of dal_rt_rtl8277c_l2_ivlSvl_set */

/* Function Name:
 *      dal_rt_rtl8277c_l2_fwdGroupId_set
 * Description:
 *      Set l2 forward group action mask
 * Input:
 *      idx   - forward group index
 *      mask  - forward group mask
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
extern int32
dal_rt_rtl8277c_l2_fwdGroupId_set(uint8 idx, uint64 mask)
{
    int32 ret = RT_ERR_OK;
    ni_mce_indx_tbl_entry_t vec;
    ca_uint16_t mc_idx;

    vec.mc_vec = mask;
    mc_idx = idx;

    ret = aal_mc_ni_mce_indx_set(0, mc_idx, &vec);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }
    return ret;
}/* end of dal_rt_rtl8277c_l2_fwdGroupId_set */

/* Function Name:
 *      dal_rt_rtl8277c_l2_fwdGroupId_get
 * Description:
 *      Get l2 forward group action mask
 * Input:
 *      idx   - forward group index
 * Output:
 *      pMask  - forward group mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
extern int32
dal_rt_rtl8277c_l2_fwdGroupId_get(uint8 idx, uint64 *pMask)
{
    int32 ret = RT_ERR_OK;
    ni_mce_indx_tbl_entry_t vec;
    ca_uint16_t mc_idx;

    mc_idx = idx;

    ret = aal_mc_ni_mce_indx_get(0, mc_idx, &vec);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    *pMask = vec.mc_vec;

    return ret;
}/* end of dal_rt_rtl8277c_l2_fwdGroupId_get */

/* Function Name:
 *      dal_rt_rtl8277c_l2_fwdGroupAction_set
 * Description:
 *      Set forward group action
 * Input:
 *      idx     - forward group action index
 *      portAct - port forward action
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
extern int32
dal_rt_rtl8277c_l2_fwdGroupAction_set(uint8 idx, rt_l2_portAction_t portAct)
{
    int32 ret = RT_ERR_OK;
    l2fe_arb_fib_tbl_entry_t fib;
    ca_uint16_t fib_idx;

    fib_idx = idx;
    memset(&fib, 0, sizeof(l2fe_arb_fib_tbl_entry_t));

    fib.ldpid = portAct.port;
    switch(portAct.vidAct)
    {
        case RT_L2_VID_ACTION_NOP:
            fib.c_vlan_cmd = ACTION_VLAN_NOP;
            break;
        case RT_L2_VID_ACTION_DELETE:
            fib.c_vlan_cmd = ACTION_VLAN_UNTAG;
            break;
        case RT_L2_VID_ACTION_MODIFY:
            fib.c_vlan_cmd = ACTION_VLAN_MODIFY;
            fib.c_vid = portAct.vid;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }
    switch(portAct.priAct)
    {
        case RT_L2_PRI_ACTION_NOP:
            fib.c_dot1p_cmd = ACTION_VLAN_NOP;
            break;
        case RT_L2_PRI_ACTION_MODIFY:
            fib.c_dot1p_cmd = ACTION_VLANTAG_BYTBL;
            fib.c_dot1p = portAct.pri;
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    fib.cos_cmd = MCFIB_COS_CMD_DIS;

    ret = aal_mc_l2fe_arb_fib_set(0, fib_idx, &fib);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }
    return ret;
}/* end of dal_rt_rtl8277c_l2_fwdGroupAction_set */

/* Function Name:
 *      dal_rt_rtl8277c_l2_fwdGroupAction_get
 * Description:
 *      Get forward group action
 * Input:
 *      idx     - forward group action index
 * Output:
 *      pPortAct - port forward action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
extern int32
dal_rt_rtl8277c_l2_fwdGroupAction_get(uint8 idx, rt_l2_portAction_t *pPortAct)
{
    int32 ret = RT_ERR_OK;
    l2fe_arb_fib_tbl_entry_t fib;
    ca_uint16_t fib_idx;

    fib_idx = idx;
    memset(&fib, 0, sizeof(l2fe_arb_fib_tbl_entry_t));

    ret = aal_mc_l2fe_arb_fib_get(0, fib_idx, &fib);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    pPortAct->port = fib.ldpid;
    switch(fib.c_vlan_cmd)
    {
        case ACTION_VLAN_NOP:
            pPortAct->vidAct = RT_L2_VID_ACTION_NOP;
            break;
        case ACTION_VLAN_UNTAG:
            pPortAct->vidAct = RT_L2_VID_ACTION_DELETE;
            break;
        case ACTION_VLAN_MODIFY:
            pPortAct->vidAct = RT_L2_VID_ACTION_MODIFY;
            pPortAct->vid = fib.c_vid;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }
    switch(fib.c_dot1p_cmd)
    {
        case ACTION_VLAN_NOP:
            pPortAct->priAct = RT_L2_PRI_ACTION_NOP;
            break;
        case ACTION_VLANTAG_BYTBL:
            pPortAct->priAct = RT_L2_PRI_ACTION_MODIFY;
            pPortAct->pri = fib.c_dot1p;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
}/* end of dal_rt_rtl8277c_l2_fwdGroupAction_get */
