/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : switch asic-level VLAN API
 * Feature : L2 related functions
 *
 */

#include <dal/apollomp/raw/apollomp_raw_l2.h>
#include <osal/time.h>

/* Function Name:
 *      apollomp_raw_l2_lookUpTb_set
 * Description:
 *      Set filtering database entry
 * Input:
 *      pL2Table 	- L2 table entry writing to 2K+64 filtering database
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32 apollomp_raw_l2_lookUpTb_set(apollomp_lut_table_t *pL2Table)
{
   int32 ret = RT_ERR_FAILED;
   rtk_enable_t state;

    RT_PARAM_CHK(NULL == pL2Table, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pL2Table->table_type > APOLLOMP_RAW_LUT_ENTRY_TYPE_END, RT_ERR_OUT_OF_RANGE);

    if ((ret = apollomp_raw_l2_camEnable_get(&state)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(ENABLED == state)
        RT_PARAM_CHK(pL2Table->address > (APOLLOMP_LUT_4WAY_NO + APOLLOMP_LUT_CAM_NO), RT_ERR_INPUT);
    else
        RT_PARAM_CHK(pL2Table->address > APOLLOMP_LUT_4WAY_NO, RT_ERR_INPUT);

#ifndef CONFIG_SDK_ASICDRV_TEST
    RT_PARAM_CHK(APOLLOMP_RAW_LUT_READ_METHOD_ADDRESS == pL2Table->method, RT_ERR_INPUT);
#endif
    /*commna part check*/
    RT_PARAM_CHK(pL2Table->l3lookup > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->lut_pri > APOLLOMP_RAW_LUT_FWDPRIMAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->fwdpri_en > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->nosalearn > 1, RT_ERR_OUT_OF_RANGE);

    /*L2 check*/
    RT_PARAM_CHK(pL2Table->cvid_fid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);

    /*l2 uc check*/
    RT_PARAM_CHK(pL2Table->fid > HAL_VLAN_FID_MAX(), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->efid > HAL_ENHANCED_FID_MAX(), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->sapri_en > 1, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(!HAL_IS_PORT_EXIST(pL2Table->spa), RT_ERR_PORT_ID);
    RT_PARAM_CHK(pL2Table->age > APOLLOMP_RAW_LUT_AGEMAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pL2Table->auth > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->sa_block > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->da_block > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->ext_dsl_spa > APOLLOMP_RAW_LUT_EXTDSL_SPAMAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->arp_used > 1, RT_ERR_OUT_OF_RANGE);

    /*----L3----*/
    /*RT_PARAM_CHK(pL2Table->gip > APOLLO_APOLLOMP_RAW_LUT_GIPMAX, RT_ERR_OUT_OF_RANGE);*/
    RT_PARAM_CHK(pL2Table->gip_only > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->wan_sa > 1, RT_ERR_OUT_OF_RANGE);

    /* ---L3 MC DSL---*/
    /*RT_PARAM_CHK(pL2Table->sip_vid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);*/

    /* ---L3 MC ROUTE---*/
    RT_PARAM_CHK(pL2Table->l3_idx > APOLLOMP_RAW_LUT_L3IDXAMAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->ext_fr > 1, RT_ERR_OUT_OF_RANGE);

    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
    RT_PARAM_CHK(pL2Table->mbr > APOLLOMP_PORTMASK, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pL2Table->ext_mbr > APOLLOMP_RAW_LUT_EXTMBR, RT_ERR_OUT_OF_RANGE);

    /*static entry will assign age to none-ZERO*/
    if(pL2Table->nosalearn && pL2Table->table_type == APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC)
        pL2Table->age = 1;

    /*write to l2 table*/
    if ((ret = table_write(APOLLOMP_L2_UCt, pL2Table->address, (uint32 *)pL2Table)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_lookUpTb_get
 * Description:
 *      Get filtering database entry
 * Input:
 *      method      -  Lut access method
 * Output:
 *      pL2Table 	- L2 table entry writing to 2K+64 filtering database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_lookUpTb_get(apollomp_lut_table_t *pL2Table)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t state;

    RT_PARAM_CHK(pL2Table == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pL2Table->method > APOLLOMP_RAW_LUT_READ_METHOD_END, RT_ERR_INPUT);

    if ((ret = apollomp_raw_l2_camEnable_get(&state)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(ENABLED == state)
        RT_PARAM_CHK(pL2Table->address > (APOLLOMP_LUT_4WAY_NO + APOLLOMP_LUT_CAM_NO), RT_ERR_INPUT);
    else
        RT_PARAM_CHK(pL2Table->address > APOLLOMP_LUT_4WAY_NO, RT_ERR_INPUT);


    if ((ret = table_read(APOLLOMP_L2_UCt, pL2Table->address, (uint32 *)pL2Table)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

	return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_flushEn_set
 * Description:
 *      Set per port force flush setting
 * Input:
 *      port   		- port id
 *      enable 		- enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 apollomp_raw_l2_flushEn_set(rtk_port_t port, rtk_enable_t enabled)
{
    int32 ret;
    apollomp_raw_l2_flushStatus_t status;
    uint32 cnt=0;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTK_ENABLE_END <= enabled), RT_ERR_INPUT);

 	if ((ret = reg_array_field_write(APOLLOMP_L2_TBL_FLUSH_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enabled)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    /* wait the flush status for non-busy */
    if ((ret = reg_field_read(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_FLUSH_STATUSf ,&status)) != RT_ERR_OK )
    {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return ret;
    }
    while (status != APOLLOMP_RAW_FLUSH_STATUS_NONBUSY)
    {
        if(cnt++ > 0xFFFF)
            return RT_ERR_NOT_FINISH;

        osal_time_mdelay(10);

        if ((ret = reg_field_read(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_FLUSH_STATUSf ,&status)) != RT_ERR_OK )
        {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return ret;
        }
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_flushEn_get
 * Description:
 *      Get per port force flush status
 * Input:
 *      port   		    - port id
 * Output:
 *      pEnable 		- enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_flushEn_get(rtk_port_t port, rtk_enable_t *pEnabled)
{
    int32 ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(NULL == pEnabled, RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(APOLLOMP_L2_TBL_FLUSH_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, pEnabled)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_l2_flushCtrl_set
 * Description:
 *      L2 table flush control configuration.
 * Input:
 *      pCtrl 	- The flush control configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_OK                     - OK
 *      RT_ERR_OUT_OF_RANGE  - input parameter out of range
 *      RT_ERR_NULL_POINTER    - input parameter is null pointer
 *      RT_ERR_VLAN_VID           - invalid vid
 *      RT_ERR_L2_FID                - invalid fid
 *
 * Note:
 *      None
 */
int32 apollomp_raw_l2_flushCtrl_set(apollomp_raw_flush_ctrl_t *pCtrl)
{
    int32 ret = RT_ERR_FAILED;
    uint32 staticFlag;
    uint32 dynamicFlag;

    RT_PARAM_CHK(pCtrl == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pCtrl->flushMode >= APOLLOMP_RAW_FLUSH_MODE_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCtrl->flushType >= APOLLOMP_RAW_FLUSH_TYPE_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCtrl->vid > RTK_VLAN_ID_MAX , RT_ERR_VLAN_VID);
    RT_PARAM_CHK(pCtrl->fid > HAL_ENHANCED_FID_MAX() , RT_ERR_L2_FID);

    if ((ret = reg_field_write(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_LUT_FLUSH_MODEf, &pCtrl->flushMode)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    switch(pCtrl->flushType)
    {
        case APOLLOMP_RAW_FLUSH_TYPE_STATIC:
            staticFlag = 1;
            dynamicFlag = 0;
            break;
        case APOLLOMP_RAW_FLUSH_TYPE_DYNAMIC:
            staticFlag = 0;
            dynamicFlag = 1;
            break;
        case APOLLOMP_RAW_FLUSH_TYPE_BOTH:
            staticFlag = 1;
            dynamicFlag = 1;
            break;
        default:
            return RT_ERR_FAILED;
            break;

    }

    if ((ret = reg_field_write(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_LUT_FLUSH_STATICf, &staticFlag)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_LUT_FLUSH_DYNAMICf, &dynamicFlag)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_LUT_FLUSH_VIDf, &pCtrl->vid)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }


    if ((ret = reg_field_write(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_LUT_FLUSH_FIDf, &pCtrl->fid)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_flushCtrl_get
 * Description:
 *      L2 table flush control configuration.
 * Input:
 *     None
 * Output:
 *     pCtrl 	- The flush control configuration
 * Return:
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_OK                     - OK
 *      RT_ERR_NULL_POINTER    - input parameter is null pointer
 * Note:
 *      None
 */
int32 apollomp_raw_l2_flushCtrl_get(apollomp_raw_flush_ctrl_t *pCtrl)

{
    int32 ret = RT_ERR_FAILED;
    uint32 staticFlag;
    uint32 dynamicFlag;

    RT_PARAM_CHK(pCtrl == NULL, RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_LUT_FLUSH_MODEf, &pCtrl->flushMode)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_LUT_FLUSH_STATICf, &staticFlag)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_LUT_FLUSH_DYNAMICf, &dynamicFlag)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

	if (staticFlag && dynamicFlag)
		pCtrl->flushType = APOLLOMP_RAW_FLUSH_TYPE_BOTH;
	else if (staticFlag)
		pCtrl->flushType = APOLLOMP_RAW_FLUSH_TYPE_STATIC;
	else if (dynamicFlag)
		pCtrl->flushType = APOLLOMP_RAW_FLUSH_TYPE_DYNAMIC;
    else
        pCtrl->flushType = APOLLOMP_RAW_FLUSH_TYPE_END;

    if ((ret = reg_field_read(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_LUT_FLUSH_VIDf, &pCtrl->vid)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_LUT_FLUSH_FIDf, &pCtrl->fid)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_L2_TBL_FLUSH_CTRLr, APOLLOMP_FLUSH_STATUSf ,&pCtrl->flushStatus)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_l2_unMatched_saCtl_set
 * Description:
 *      Set drop/trap packet if SA is not from the same source port as L2 SPA
 * Input:
 *      port 	- Port index (0~6)
 *      action 	- 0b00: normal fowarding
 *                     0b01: drop packet & disable learning
 *                     0b10: trap to CPU
 *                     0b11: copy to CPU
 * Output:
 *      None
 * Return:
 *     RT_ERR_OK
 *     RT_ERR_FAILED
 *     RT_ERR_PORT_ID
 *     RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unMatched_saCtl_set(rtk_port_t port, rtk_action_t action)
{
    int32 ret = RT_ERR_FAILED;
    apollomp_raw_l2_saCtlAct_t rawAction;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    switch (action)
    {
        case ACTION_FORWARD:
            rawAction = APOLLOMP_RAW_LUT_SACTL_ACT_FORWARD;
            break;
        case ACTION_DROP:
            rawAction = APOLLOMP_RAW_LUT_SACTL_ACT_DROP;
            break;
        case ACTION_TRAP2CPU:
            rawAction = APOLLOMP_RAW_LUT_SACTL_ACT_TRAP;
            break;
        case ACTION_COPY2CPU:
            rawAction = APOLLOMP_RAW_LUT_SACTL_ACT_COPY;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;

    }

    if ((ret = reg_array_field_write(APOLLOMP_LUT_UNMATCHED_SA_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &rawAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_unMatched_saCtl_get
 * Description:
 *      Get drop/trap packet if SA is not from the same source port as L2 SPA
 * Input:
 *      port 	- Port index (0~6)
 * Output:
 *       action 	- 0b00: normal fowarding
 *                     0b01: drop packet & disable learning
 *                     0b10: trap to CPU
 *                     0b11: copy to CPU
 * Return:
 *     RT_ERR_FAILED
 *     RT_ERR_OK
 *     RT_ERR_CHIP_NOT_SUPPORTED
 *     RT_ERR_PORT_ID
 *     RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unMatched_saCtl_get(rtk_port_t port, rtk_action_t* pAction)
{
    int32 ret;
    apollomp_raw_l2_commonAct_t rawAction;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_LUT_UNMATCHED_SA_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &rawAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    switch (rawAction)
    {
        case APOLLOMP_RAW_LUT_SACTL_ACT_FORWARD:
            *pAction = ACTION_FORWARD;
            break;
        case APOLLOMP_RAW_LUT_SACTL_ACT_DROP:
            *pAction = ACTION_DROP;
            break;
        case APOLLOMP_RAW_LUT_SACTL_ACT_TRAP:
            *pAction = ACTION_TRAP2CPU;
            break;
        case APOLLOMP_RAW_LUT_SACTL_ACT_COPY:
            *pAction = ACTION_COPY2CPU;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return RT_ERR_OK;

}
/* Function Name:
 *      apollomp_raw_l2_unkn_saCtl_set
 * Description:
 *      Set drop/trap packet if SA is unknown
 * Input:
 *      port 	- Port index (0~6)
 *      action 	- 0b00: normal fowarding
 *                     0b01: drop packet & disable learning
 *                     0b10: trap to CPU
 *                     0b11: copy to CPU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unkn_saCtl_set(rtk_port_t port,rtk_action_t action)
{
    int32 ret;
    apollomp_raw_l2_saCtlAct_t rawAction;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

    switch (action)
    {
        case ACTION_FORWARD:
            rawAction = APOLLOMP_RAW_LUT_SACTL_ACT_FORWARD;
            break;
        case ACTION_DROP:
            rawAction = APOLLOMP_RAW_LUT_SACTL_ACT_DROP;
            break;
        case ACTION_TRAP2CPU:
            rawAction = APOLLOMP_RAW_LUT_SACTL_ACT_TRAP;
            break;
        case ACTION_COPY2CPU:
            rawAction = APOLLOMP_RAW_LUT_SACTL_ACT_COPY;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_array_field_write(APOLLOMP_LUT_UNKN_SA_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &rawAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_unkn_saCtl_get
 * Description:
 *      Get drop/trap packet if SA is unknown
 * Input:
 *      port 	- Port index (0~6)
 * Output:
 *      pAction 	- 0b00: normal fowarding
 *                     0b01: drop packet & disable learning
 *                     0b10: trap to CPU
 *                     0b11: copy to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unkn_saCtl_get(rtk_port_t port, rtk_action_t* pAction)
{
    int32 ret;
    apollomp_raw_l2_commonAct_t rawAction;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_LUT_UNKN_SA_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &rawAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    switch (rawAction)
    {
        case APOLLOMP_RAW_LUT_SACTL_ACT_FORWARD:
            *pAction = ACTION_FORWARD;
            break;
        case APOLLOMP_RAW_LUT_SACTL_ACT_DROP:
            *pAction = ACTION_DROP;
            break;
        case APOLLOMP_RAW_LUT_SACTL_ACT_TRAP:
            *pAction = ACTION_TRAP2CPU;
            break;
        case APOLLOMP_RAW_LUT_SACTL_ACT_COPY:
            *pAction = ACTION_COPY2CPU;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_unkn_ucDaCtl_set
 * Description:
 *      Set drop/trap packet if unicast DA is unknown
 * Input:
 *      port 	- Port index (0~6)
 *      action 	- 0b00: normal flooding
 *                     0b01: drop packet, exclude IGMP/MLD packets
*                      0b10: trap to CPU, exclude IGMP/MLD packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unkn_ucDaCtl_set(rtk_port_t port,rtk_action_t action)
{
    int32 ret;
    apollomp_raw_l2_saCtlAct_t rawAction;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

    switch(action)
    {
        case ACTION_FORWARD:
            rawAction = APOLLOMP_RAW_L2_COMMON_ACT_FORWARD;
            break;
        case ACTION_DROP:
            rawAction = APOLLOMP_RAW_L2_COMMON_ACT_DROP;
            break;
        case ACTION_TRAP2CPU:
            rawAction = APOLLOMP_RAW_L2_COMMON_ACT_TRAP;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_array_field_write(APOLLOMP_LUT_UNKN_UC_DA_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &action)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_unkn_ucDaCtl_get
 * Description:
 *      Get drop/trap packet if unicast DA is unknown
 * Input:
 *      port 	- Port index (0~6)
 * Output:
 *       action 	- 0b00: normal flooding
 *                     0b01: drop packet, exclude IGMP/MLD packets
*                      0b10: trap to CPU, exclude IGMP/MLD packets
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unkn_ucDaCtl_get(rtk_port_t port, rtk_action_t* pAction)
{
    int32 ret;
    apollomp_raw_l2_commonAct_t rawAction;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    if ((ret = reg_array_field_read(APOLLOMP_LUT_UNKN_UC_DA_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &rawAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    switch (rawAction)
    {
        case APOLLOMP_RAW_L2_COMMON_ACT_FORWARD:
            *pAction = ACTION_FORWARD;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_DROP:
            *pAction = ACTION_DROP;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_TRAP:
            *pAction = ACTION_TRAP2CPU;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_l2_learnOverAct_set
 * Description:
 *      Set auto learn over limit number action
 * Input:
 *      port 	- Port index (0~6)
 *      action 	- Learn over action 0:normal, 1:drop 2:trap
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_learnOverAct_set(rtk_port_t port, rtk_action_t action)
{
    int32 ret;
    apollomp_raw_l2_commonAct_t rawAction;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

    switch(action)
    {
        case ACTION_FORWARD:
            rawAction=APOLLOMP_RAW_L2_COMMON_ACT_FORWARD;
            break;
        case ACTION_DROP:
            rawAction=APOLLOMP_RAW_L2_COMMON_ACT_DROP;
            break;
        case ACTION_TRAP2CPU:
            rawAction=APOLLOMP_RAW_L2_COMMON_ACT_TRAP;
            break;
        case ACTION_COPY2CPU:
            rawAction=APOLLOMP_RAW_L2_COMMON_ACT_COPY2CPU;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_array_field_write(APOLLOMP_LUT_LEARN_OVER_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &rawAction))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_learnOverAct_get
 * Description:
 *      Get auto learn over limit number action
 * Input:
 *      port 	- Port index (0~6)
 * Output:
 *      pAction 	- Learn over action 0:normal, 1:drop 2:trap
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_PORT_ID
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_learnOverAct_get(rtk_port_t port, rtk_action_t* pAction)
{
    int32 ret;
    apollomp_raw_l2_commonAct_t rawAction;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_LUT_LEARN_OVER_CTRLr, port,REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &rawAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    switch(rawAction)
    {
        case APOLLOMP_RAW_L2_COMMON_ACT_FORWARD:
            *pAction = ACTION_FORWARD;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_DROP:
            *pAction = ACTION_DROP;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_TRAP:
            *pAction = ACTION_TRAP2CPU;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_COPY2CPU:
            *pAction = ACTION_COPY2CPU;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_l2_agingEnable_set
 * Description:
 *      Set L2 LUT aging per port setting
 * Input:
 *      port   		- port id
 *      enable 		- enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 apollomp_raw_l2_agingEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 tmpData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if (enable == ENABLED)
        tmpData = ENABLED;
    else
        tmpData = DISABLED;

    if ((ret = reg_array_field_write(APOLLOMP_LUT_AGEOUT_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_AGEOUT_OUTf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

}
/* Function Name:
 *      apollomp_raw_l2_agingEnable_get
 * Description:
 *      Get L2 LUT aging per port setting
 * Input:
 *      port   		- port id
 *
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_agingEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 tmpData;

    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(APOLLOMP_LUT_AGEOUT_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_AGEOUT_OUTf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    if (tmpData == ENABLED)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_l2_lrnLimitNo_set
 * Description:
 *      Set L2 learning limitation entry number. ASIC supports 2K LUT entries and L2 learning limitation is
 *       for auto-learning L2 entry number. Set register to value 0xFFF for non-limitation.
 * Input:
 *      port 	- Physical port number (0~6)
 *      num      - set limitation entry number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32 apollomp_raw_l2_lrnLimitNo_set(rtk_port_t port, uint32 num)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(num > APOLLOMP_LUT_TBL_NO, RT_ERR_OUT_OF_RANGE);

    if ((ret = reg_array_field_write(APOLLOMP_LUT_LRN_LIMITNOr, port,REG_ARRAY_INDEX_NONE, APOLLOMP_NUMf, &num)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_lrnLimitNo_get
 * Description:
 *      Get L2 learning limitation entry number. ASIC supports 2K LUT entries and L2 learning limitation is
 *       for auto-learning L2 entry number. Set register to value 0xFFF for non-limitation.
  * Input:
  *      port 	- Physical port number (0~6)
 *
 * Output:
 *      pNum      - get limitation entry number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_lrnLimitNo_get(rtk_port_t port, uint32 *pNum)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(pNum == NULL, RT_ERR_NULL_POINTER);

	if (( ret = reg_array_field_read(APOLLOMP_LUT_LRN_LIMITNOr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_NUMf, pNum)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_lrnCnt_get
 * Description:
 *      Get    Number of SA learned
 * Input:
  *      port 	- Physical port number (0~6)
  * Output:
 *      pNum      - get Number of SA learned on Port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_lrnCnt_get(rtk_port_t port, uint32 *pNum)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(pNum == NULL, RT_ERR_NULL_POINTER);

	if((ret = reg_array_field_read(APOLLOMP_L2_LRN_CNTr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_L2_LRN_CNTf, pNum)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_l2_lrnOverSts_set
 * Description:
 *      Set L2 per-port learning over event to clear register
 * Input:
 *      port 	- Physical port number (0~6)
 *      value      - set 1 to clear
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32 apollomp_raw_l2_lrnOverSts_set(rtk_port_t port, uint32 value)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(value > 1, RT_ERR_OUT_OF_RANGE);

    if (( ret = reg_array_field_write(APOLLOMP_L2_LRN_OVER_STSr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_LRN_OVER_INDf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_lrnOverSts_get
 * Description:
 *      Get  L2 per-port learning over event
 * Input:
*      port 	- Physical port number (0~6)
 *
 * Output:
 *      pValue      - L2 learning over ever occurs

 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_PORT_ID  	- Invalid port id
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      None
 */
int32 apollomp_raw_l2_lrnOverSts_get(rtk_port_t port, uint32 *pValue)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(pValue == NULL, RT_ERR_NULL_POINTER);

	if ((ret = reg_array_field_read(APOLLOMP_L2_LRN_OVER_STSr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_LRN_OVER_INDf, pValue)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_sysLrnLimitNo_set
 * Description:
 *      Set system learning limit number
 * Input:
 *      num - system learning limit number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32 apollomp_raw_l2_sysLrnLimitNo_set(uint32 num)
{

    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK((num > APOLLOMP_LUT_TBL_NO), RT_ERR_OUT_OF_RANGE);
    if ((ret = reg_field_write(APOLLOMP_LUT_SYS_LRN_LIMITNOr, APOLLOMP_SYS_LRN_LIMITNOf, &num)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_sysLrnLimitNo_get
 * Description:
 *      Get system learning limit number
 * Input:
*      None
 * Output:
  *     pNum - system learning limit number
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      None
 */

int32 apollomp_raw_l2_sysLrnLimitNo_get(uint32* pNum)
{
	int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK((NULL == pNum), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_LUT_SYS_LRN_LIMITNOr, APOLLOMP_SYS_LRN_LIMITNOf, pNum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_l2_sysLrnOverAct_set
 * Description:
 *      Set all system auto learn over limit number action
 * Input:
 *      action 	- Learn over action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_sysLrnOverAct_set(rtk_action_t action)
{
	int32 ret = RT_ERR_FAILED;
    apollomp_raw_l2_commonAct_t tmpData;

    switch(action)
    {
        case ACTION_FORWARD:
            tmpData=APOLLOMP_RAW_L2_COMMON_ACT_FORWARD;
            break;
        case ACTION_DROP:
            tmpData=APOLLOMP_RAW_L2_COMMON_ACT_DROP;
            break;
        case ACTION_TRAP2CPU:
            tmpData=APOLLOMP_RAW_L2_COMMON_ACT_TRAP;
            break;
        case ACTION_COPY2CPU:
            tmpData=APOLLOMP_RAW_L2_COMMON_ACT_COPY2CPU;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }


    if ((ret = reg_field_write(APOLLOMP_LUT_SYS_LRN_OVER_CTRLr, APOLLOMP_ACTf, &tmpData))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_learnOverAct_get
 * Description:
 *      Get auto learn over limit number action
 * Input:
 *      None
 * Output:
 *      pAction 	        - Learn over action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NOT_ALLOWED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_sysLrnOverAct_get(rtk_action_t* pAction)
{
	int32 ret = RT_ERR_FAILED;
    apollomp_raw_l2_commonAct_t tmpData;

    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);


    if ((ret = reg_field_read(APOLLOMP_LUT_SYS_LRN_OVER_CTRLr, APOLLOMP_ACTf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    switch(tmpData)
    {
        case APOLLOMP_RAW_L2_COMMON_ACT_FORWARD:
            *pAction = ACTION_FORWARD;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_DROP:
            *pAction = ACTION_DROP;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_TRAP:
            *pAction = ACTION_TRAP2CPU;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_COPY2CPU:
            *pAction = ACTION_COPY2CPU;
            break;
        default:
            return RT_ERR_NOT_ALLOWED;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_sysLrnOverSts_set
 * Description:
 *      Set L2 system learning over event register
 * Input:
 *      value      - set 1 to clear
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
  *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32 apollomp_raw_l2_sysLrnOverSts_set( uint32 value)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(value > 1, RT_ERR_OUT_OF_RANGE);

    if((ret = reg_field_write(APOLLOMP_L2_SYS_LRN_OVER_STSr, APOLLOMP_LRN_OVER_INDf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_sysLrnOverSts_get
 * Description:
 *      Get  L2 system learning over event register
 * Input:
 *      None
 * Output:
 *      pValue      - L2 learning over ever occurs
 * Return:
 *      RT_ERR_OK 			- Success
  *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      None
 */
int32 apollomp_raw_l2_sysLrnOverSts_get( uint32 *pValue)
{
    int32 ret;

    RT_PARAM_CHK(pValue == NULL, RT_ERR_NULL_POINTER);
	if (( ret = reg_field_read(APOLLOMP_L2_SYS_LRN_OVER_STSr, APOLLOMP_LRN_OVER_INDf, pValue)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_SysLrnCnt_get
 * Description:
 *      Get system Number of SA learned register
 * Input:
 *     None
 * Output:
 *      pNum      - get wole system number of SA learned counter
 * Return:
 *      RT_ERR_OK 			- Success
  *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      None
 */
int32 apollomp_raw_l2_SysLrnCnt_get( uint32 *pNum)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(pNum == NULL, RT_ERR_NULL_POINTER);

	if((ret = reg_field_read(APOLLOMP_L2_SYS_LRN_CNTr, APOLLOMP_SYS_L2_LRN_CNTf, pNum)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_unkn_l2Mc_set
 * Description:
 *      Set unknow L2 multicast frame behavior
 * Input:
 *      port 	- Port index (0~6)
 *      action 	- Learn over action
 *                     0b00:normal
 *                     0b01:drop
 *                     0b10:trap to CPU
 *                     0b11:drop packet exclude RMA
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_NOT_ALLOWED  - Invalid learn over action
 *      RT_ERR_PORT_ID  	    - Invalid port id
 * Note:
 *      None
 */
int32 apollomp_raw_unkn_l2Mc_set(rtk_port_t port, rtk_action_t action)
{
    int32 ret;
    apollomp_raw_l2_unknMcAct_t tmpData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    tmpData = APOLLOMP_RAW_L2_UNKNMC_ACT_END;
    switch (action)
    {
        case ACTION_FORWARD:
            tmpData = APOLLOMP_RAW_L2_UNKNMC_ACT_FORWARD;
            break;
        case ACTION_DROP:
            tmpData = APOLLOMP_RAW_L2_UNKNMC_ACT_DROP;
            break;
        case ACTION_TRAP2CPU:
            tmpData = APOLLOMP_RAW_L2_UNKNMC_ACT_TRAP;
            break;
        case ACTION_DROP_EXCLUDE_RMA:
            tmpData = APOLLOMP_RAW_L2_UNKNMC_ACT_EXCLUDE_RMA;
            break;
        default:
            tmpData = APOLLOMP_RAW_L2_UNKNMC_ACT_END;

    }
    RT_PARAM_CHK(tmpData >= APOLLOMP_RAW_L2_UNKNMC_ACT_END, RT_ERR_NOT_ALLOWED);
    if ((ret = reg_array_field_write(APOLLOMP_UNKN_L2_MCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_unkn_l2Mc_get
 * Description:
 *      Get unknow L2 multicast frame behavior
 * Input:
 *      port 	- Port index (0~6)
  *
 * Output:
 *      action 	- Learn over action
 *                     0b00:normal
 *                     0b01:drop
 *                     0b10:trap to CPU
 *                     0b11:drop packet exclude RMA
 * Return:
 *      RT_ERR_OK 			    - Success
 *      RT_ERR_NULL_POINTER   - Null pointer
 *      RT_ERR_PORT_ID  	    - Invalid port id
 * Note:
 *      None
 */
int32 apollomp_raw_unkn_l2Mc_get(rtk_port_t port, rtk_action_t* pAction)
{
    int32 ret;
    apollomp_raw_l2_unknMcAct_t tmpData;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    if ((ret = reg_array_field_read(APOLLOMP_UNKN_L2_MCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    switch (tmpData)
    {

        case APOLLOMP_RAW_L2_UNKNMC_ACT_FORWARD:
            *pAction = ACTION_FORWARD;
            break;
        case APOLLOMP_RAW_L2_UNKNMC_ACT_DROP:
            *pAction = ACTION_DROP;
            break;
        case APOLLOMP_RAW_L2_UNKNMC_ACT_TRAP:
            *pAction = ACTION_TRAP2CPU;
            break;
        case APOLLOMP_RAW_L2_UNKNMC_ACT_EXCLUDE_RMA:
            *pAction = ACTION_DROP_EXCLUDE_RMA;
             break;
        default:
            return RT_ERR_NOT_ALLOWED;
    }
    return RT_ERR_OK;

}
/* Function Name:
 *      apollomp_raw_unkn_ip4Mc_set
 * Description:
 *      Set unknow IPv4 multicast frame behavior
 * Input:
 *      port 	- Port index (0~6)
 *      action 	- Learn over action
 *                     0b00:normal flooding
 *                     0b01:drop packet, exclude IP 224.0.0.x and IGMP packets
 *                     0b10:trap to CPU, exclude IP 224.0.0.x and IGMP packets
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			    - Success
 *      RT_ERR_PORT_ID  	    - Invalid port id
 *      RT_ERR_NOT_ALLOWED  - Invalid learn over action
 * Note:
 *      None
 */
int32 apollomp_raw_unkn_ip4Mc_set(rtk_port_t port, rtk_action_t action)
{
    int32 ret;
    apollomp_raw_l2_commonAct_t tmpData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    tmpData = APOLLOMP_RAW_L2_COMMON_ACT_END;
    switch (action)
    {
        case ACTION_FORWARD:
            tmpData = APOLLOMP_RAW_L2_COMMON_ACT_FORWARD;
            break;
        case ACTION_DROP:
            tmpData = APOLLOMP_RAW_L2_COMMON_ACT_DROP;
            break;
        case ACTION_TRAP2CPU:
            tmpData = APOLLOMP_RAW_L2_COMMON_ACT_TRAP;
            break;
        default:
            tmpData = APOLLOMP_RAW_L2_COMMON_ACT_END;
    }

    RT_PARAM_CHK(tmpData >= APOLLOMP_RAW_L2_COMMON_ACT_END, RT_ERR_NOT_ALLOWED);
    if ((ret = reg_array_field_write(APOLLOMP_UNKN_IP4_MCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_unkn_ip4Mc_get
 * Description:
 *      Get unknow IPv4 multicast frame behavior
 * Input:
 *      port     - Port index (0~6)
  *
 * Output:

 *      action  - Learn over action
   *                     0b00:normal flooding
   *                     0b01:drop packet, exclude IP 224.0.0.x and IGMP packets
   *                     0b10:trap to CPU, exclude IP 224.0.0.x and IGMP packets
 * Return:
 *      RT_ERR_OK 			    - Success
 *      RT_ERR_PORT_ID  	    - Invalid port id
 *      RT_ERR_NULL_POINTER   -Null pointer
 *      RT_ERR_NOT_ALLOWED  - Invalid learn over action
 * Note:
 *      None
 */
int32 apollomp_raw_unkn_ip4Mc_get(rtk_port_t port, rtk_action_t* pAction)
{
    int32 ret = RT_ERR_FAILED;
    apollomp_raw_l2_commonAct_t tmpData;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    if ((ret = reg_array_field_read(APOLLOMP_UNKN_IP4_MCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    switch (tmpData)
    {

        case APOLLOMP_RAW_L2_COMMON_ACT_FORWARD:
            *pAction = ACTION_FORWARD;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_DROP:
            *pAction = ACTION_DROP;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_TRAP:
            *pAction = ACTION_TRAP2CPU;
            break;
        default:
            return RT_ERR_NOT_ALLOWED;
    }
    return RT_ERR_OK;

}
/* Function Name:
 *      apollomp_raw_unkn_ip6Mc_set
 * Description:
 *      Set unknow IPv6 multicast frame behavior
 * Input:
 *      port 	- Port index (0~6)
 *      action 	- Learn over action
 *                     0b00:normal flooding
 *                     0b01:drop packet, exclude IP [FFXX::/8] and MLD packets
 *                     0b10:trap to CPU, exclude IP [FFXX::/8] and MLD packets
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			    - Success
 *      RT_ERR_PORT_ID  	    - Invalid port id
 *      RT_ERR_NOT_ALLOWED  - Invalid learn over action
 * Note:
 *      None
 */
int32 apollomp_raw_unkn_ip6Mc_set(rtk_port_t port, rtk_action_t action)
{
    int32 ret;
    apollomp_raw_l2_commonAct_t tmpData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    tmpData = APOLLOMP_RAW_L2_COMMON_ACT_END;
    switch (action)
    {
        case ACTION_FORWARD:
            tmpData = APOLLOMP_RAW_L2_COMMON_ACT_FORWARD;
            break;
        case ACTION_DROP:
            tmpData = APOLLOMP_RAW_L2_COMMON_ACT_DROP;
            break;
        case ACTION_TRAP2CPU:
            tmpData = APOLLOMP_RAW_L2_COMMON_ACT_TRAP;
            break;
        default:
            tmpData = APOLLOMP_RAW_L2_COMMON_ACT_END;

    }
    RT_PARAM_CHK(tmpData >= APOLLOMP_RAW_L2_COMMON_ACT_END, RT_ERR_NOT_ALLOWED);
    if ((ret=reg_array_field_write(APOLLOMP_UNKN_IP6_MCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_unkn_ip6Mc_get
 * Description:
 *      Get unknow IPv6 multicast frame behavior
 * Input:
  *      port 	- Port index (0~6)
 *
 * Output:
  *      action 	- Learn over action
 *                     0b00:normal flooding
 *                     0b01:drop packet, exclude IP [FFXX::/8] and MLD packets
 *                     0b10:trap to CPU, exclude IP [FFXX::/8] and MLD packets
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_NULL_POINTER      -Null pointer
 *      RT_ERR_PORT_ID  	    - Invalid port id
 *      RT_ERR_NOT_ALLOWED  - Invalid learn over action
 * Note:
 *      None
 */
int32 apollomp_raw_unkn_ip6Mc_get(rtk_port_t port, rtk_action_t* pAction)
{
    int32 ret;
    apollomp_raw_l2_commonAct_t tmpData;
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    if (( ret = reg_array_field_read(APOLLOMP_UNKN_IP6_MCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    switch (tmpData)
    {
        case APOLLOMP_RAW_L2_COMMON_ACT_FORWARD:
            *pAction = ACTION_FORWARD;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_DROP:
            *pAction = ACTION_DROP;
            break;
        case APOLLOMP_RAW_L2_COMMON_ACT_TRAP:
            *pAction = ACTION_TRAP2CPU;
            break;
        default:
            return RT_ERR_NOT_ALLOWED;
    }
    return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_unkn_mcPri_set
 * Description:
 *      Set Trap priority for unknown L2/Ipv4/IPv6 multicast
 * Input:
 *      priority - Trap priority for unknown L2/Ipv4/IPv6 multicast
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32 apollomp_raw_unkn_mcPri_set(uint32 priority)
{
    int32 ret;

    RT_PARAM_CHK((priority > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_OUT_OF_RANGE);

    if ((ret = reg_field_write(APOLLOMP_UNKN_MC_CFGr, APOLLOMP_UNKN_MC_PRIf, &priority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_unkn_mcPri_get
 * Description:
 *      Get Trap priority for unknown L2/Ipv4/IPv6 multicast
 * Input:
 *       None
 * Output:
 *        pPriority - Trap priority for unknown L2/Ipv4/IPv6 multicast
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      None
 */

int32 apollomp_raw_unkn_mcPri_get(uint32* pPriority)
{
	int32 ret;

    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_UNKN_MC_CFGr, APOLLOMP_UNKN_MC_PRIf, pPriority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_bcFlood_set
 * Description:
 *      Set egress port status for broadcast(ff-ff-ff-ff-ff-ff) flooding packets
 * Input:
 *      port 	       - Physical port number (0~6)
 *      enabled       	- 0b0: drop; 0b1: normal flooding
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_l2_bcFlood_set(rtk_port_t port, rtk_enable_t enabled)
{
    int32 ret = RT_ERR_FAILED;
    apollomp_raw_l2_flood_enable_t tmpData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);
 	RT_PARAM_CHK((RTK_ENABLE_END <= enabled), RT_ERR_INPUT);

    if ( enabled == ENABLED)
        tmpData = APOLLOMP_RAW_LUT_FLOOD_FLOODING;
    else
        tmpData = APOLLOMP_RAW_LUT_FLOOD_DROP;

    if ((ret = reg_array_field_write(APOLLOMP_LUT_BC_FLOODr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_bcFlood_get
 * Description:
 *      Get egress port status for broadcast(ff-ff-ff-ff-ff-ff) flooding packets
 * Input:
 *      port 	       - Physical port number (0~6)
 *
 * Output:
 *      pEnabled       	- 0b0: drop; 0b1: normal flooding
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_PORT_ID
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_bcFlood_get(rtk_port_t port, rtk_enable_t *pEnabled)
{
    int32 ret = RT_ERR_FAILED;
    apollomp_raw_l2_flood_enable_t tmpData;

    RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

    if (( ret = reg_array_field_read(APOLLOMP_LUT_BC_FLOODr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    switch(tmpData)
    {
        case APOLLOMP_RAW_LUT_FLOOD_FLOODING:
            *pEnabled = ENABLED;
            break;
        case APOLLOMP_RAW_LUT_FLOOD_DROP:
            *pEnabled = DISABLED;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;

}
/* Function Name:
 *      apollomp_raw_l2_unknUcFlood_set
 * Description:
 *      Set egress port status for unknown multicast flooding
 * Input:
 *      port 	       - Physical port number (0~6)
 *      enabled       	- 0b0: drop; 0b1: normal flooding
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unknUcFlood_set(rtk_port_t port, rtk_enable_t enabled)
{
    int32 ret;
    apollomp_raw_l2_flood_enable_t tmpData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
 	RT_PARAM_CHK((RTK_ENABLE_END <= enabled), RT_ERR_INPUT);

    if ( enabled == ENABLED )
        tmpData = APOLLOMP_RAW_LUT_FLOOD_FLOODING;
    else
        tmpData = APOLLOMP_RAW_LUT_FLOOD_DROP;

    if (( ret = reg_array_field_write(APOLLOMP_LUT_UNKN_UC_FLOODr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_unknUcFlood_get
 * Description:
 *      Get egress port status for unknown multicast flooding
 * Input:
 *      port 	       - Physical port number (0~6)
  * Output:
 *      pEnabled      	- 0b0: drop; 0b1: normal flooding
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_PORT_ID
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unknUcFlood_get(rtk_port_t port, rtk_enable_t *pEnabled)
{
    int32 ret;
    apollomp_raw_l2_flood_enable_t tmpData;

    RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(APOLLOMP_LUT_UNKN_UC_FLOODr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    switch(tmpData)
    {
        case APOLLOMP_RAW_LUT_FLOOD_FLOODING:
            *pEnabled = ENABLED;
            break;
        case APOLLOMP_RAW_LUT_FLOOD_DROP:
            *pEnabled = DISABLED;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_unknMcFlood_set
 * Description:
 *      Set egress port mask for unknown unicast flooding packets
 * Input:
 *      port 	       - Physical port number (0~6)
 *      enabled       	- 0b0: drop; 0b1: normal flooding
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unknMcFlood_set(rtk_port_t port, rtk_enable_t enabled)
{
    int32 ret = RT_ERR_FAILED;
    apollomp_raw_l2_flood_enable_t tmpData;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);
 	RT_PARAM_CHK((RTK_ENABLE_END <= enabled), RT_ERR_INPUT);

    if (enabled == ENABLED)
        tmpData = APOLLOMP_RAW_LUT_FLOOD_FLOODING;
    else
        tmpData = APOLLOMP_RAW_LUT_FLOOD_DROP;

    if ((ret = reg_array_field_write(APOLLOMP_LUT_UNKN_MC_FLOODr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l2_unknMcFlood_get
 * Description:
 *      Get egress port mask for unknown unicast flooding packets
 * Input:
 *      port 	       - Physical port number (0~6)
 * Output:
  *      pEnabled      	- 0b0: drop; 0b1: normal flooding
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_PORT_ID
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unknMcFlood_get(rtk_port_t port, rtk_enable_t *pEnabled)
{
    int32 ret;
    apollomp_raw_l2_flood_enable_t tmpData;

    RT_PARAM_CHK((NULL == pEnabled), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(APOLLOMP_LUT_UNKN_MC_FLOODr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    switch(tmpData)
    {
        case APOLLOMP_RAW_LUT_FLOOD_FLOODING:
            *pEnabled = ENABLED;
            break;
        case APOLLOMP_RAW_LUT_FLOOD_DROP:
            *pEnabled = DISABLED;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_efid_set
 * Description:
 *      Set per port efid
 * Input:
 *      port 	       - Physical port number (0~6)
 *      efid       	- per port EFID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		  - Success
 *      RT_ERR_PORT_ID  - Invalid port number
 *      RT_ERR_FID          - Invalid unit id
 * Note:
 *      None
 */
int32 apollomp_raw_l2_efid_set(rtk_port_t port, rtk_fid_t efid)
{
    int32 ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(efid > HAL_ENHANCED_FID_MAX(), RT_ERR_FID);

    if ((ret = reg_array_field_write(APOLLOMP_L2_EFIDr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_EFIDf, &efid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

}
/* Function Name:
 *      apollomp_raw_l2_efid_get
 * Description:
 *      Get per port efid
  * Input:
 *      port 	       - Physical port number (0~6)
*
 * Output:
 *      pEfid       	- per port EFID

 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 apollomp_raw_l2_efid_get(rtk_port_t port, rtk_fid_t* pEfid)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pEfid), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(APOLLOMP_L2_EFIDr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_EFIDf, pEfid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_ipmcAction_set
 * Description:
 *      Set per port multicast data action
 * Input:
 *      port 	        - Physical port number (0~6)
 *      action          - Allow / Drop
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_ipmcAction_set(rtk_port_t port, rtk_action_t action)
{
    int32                   ret;
    apollomp_raw_l2_McAllow_t raw_mc_act;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port),RT_ERR_PORT_ID);

    switch(action)
	{
	 	case ACTION_FORWARD:
	 		raw_mc_act = APOLLOMP_RAW_L2_MC_ALLOW;
			break;
	 	case ACTION_DROP:
	 		raw_mc_act = APOLLOMP_RAW_L2_MC_DROP;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    if ((ret = reg_array_field_write(APOLLOMP_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ALLOW_MC_DATAf, (uint32 *)&raw_mc_act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_ipmcAction_get
 * Description:
 *      Get per port multicast data action
 * Input:
 *      port 	        - Physical port number (0~6)
 * Output:
 *      pAction         - Allow / Drop.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_ipmcAction_get(rtk_port_t port, rtk_action_t *pAction)
{
    int32                   ret;
    apollomp_raw_l2_McAllow_t raw_mc_act;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ALLOW_MC_DATAf, (uint32 *)&raw_mc_act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    switch(raw_mc_act)
	{
	 	case APOLLOMP_RAW_L2_MC_ALLOW:
	 		*pAction = ACTION_FORWARD;
			break;
	 	case APOLLOMP_RAW_L2_MC_DROP:
	 		*pAction = ACTION_DROP;
			break;
		default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_igmp_Mc_table_set
 * Description:
 *      Set IGMP MC Group table
 * Input:
 *      index           - table index (0 ~ 63)
 *      dip             - Group IP (224.0.0.0 ~ 239.255.255.255)
 *      portmask        - the forwarding port mask of this group.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK 		            - Success
 *      RT_ERR_PORT_MASK            - Invalid port mask
 *      RT_ERR_OUT_OF_RANGE         - Out of range
 *      RT_ERR_INPUT                - Error Input
 * Note:
 *      None
 */
int32 apollomp_raw_l2_igmp_Mc_table_set(uint32 index, rtk_ip_addr_t dip, uint32 portmask)
{
    int32           ret;
    uint32          raw_dip;

    RT_PARAM_CHK((index > APOLLOMP_IPMC_TABLE_IDX_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((dip & 0xF0000000) != 0xE0000000), RT_ERR_INPUT);
    RT_PARAM_CHK((portmask > APOLLOMP_PORTMASK), RT_ERR_PORT_MASK);

    if ((ret = reg_array_field_write(APOLLOMP_IGMP_MC_GROUPr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PMSKf, &portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    raw_dip = dip & 0x0FFFFFFF;
    if ((ret = reg_array_field_write(APOLLOMP_IGMP_MC_GROUPr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_GIPf, &raw_dip)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_igmp_Mc_table_get
 * Description:
 *      Set IGMP MC Group table
 * Input:
 *      index           - table index (0 ~ 63)
 * Output:
 *      pDip            - Group IP (224.0.0.0 ~ 239.255.255.255)
 *      pPortmask       - the forwarding port mask of this group.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32 apollomp_raw_l2_igmp_Mc_table_get(uint32 index, rtk_ip_addr_t *pDip, uint32 *pPortmask)
{
    int32           ret;
    uint32          raw_dip;

    RT_PARAM_CHK((index > APOLLOMP_IPMC_TABLE_IDX_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pDip), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_IGMP_MC_GROUPr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PMSKf, pPortmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(APOLLOMP_IGMP_MC_GROUPr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_GIPf, &raw_dip)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    *pDip = (rtk_ip_addr_t)(raw_dip |= 0xE0000000);
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_camEnable_set
 * Description:
 *      Set cam usage status
 * Input:
 *      enable 		- enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_l2_camEnable_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 tmpData;

	RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if (enable == ENABLED)
        tmpData = DISABLED;
    else
        tmpData = ENABLED;

    if ((ret = reg_field_write(APOLLOMP_LUT_CFGr, APOLLOMP_BCAM_DISf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_l2_camEnable_set */

/* Function Name:
 *      apollomp_raw_l2_camEnable_get
 * Description:
 *      Get cam usage status
 * Input:
 *      None
 * Output:
 *      pEnable 		- enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_l2_camEnable_get(rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 tmpData;

    if ((ret = reg_field_read(APOLLOMP_LUT_CFGr, APOLLOMP_BCAM_DISf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (tmpData == ENABLED)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
} /* end of apollomp_raw_l2_camEnable_get */


/* Function Name:
 *      apollomp_raw_l2_agingTime_set
 * Description:
 *      Set L2 aging time
 * Input:
 *      agingTime 		- L2 aging time, unit 0.1sec
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32 apollomp_raw_l2_agingTime_set(uint32 agingTime)
{
    int32 ret;

    RT_PARAM_CHK((agingTime > APOLLOMP_RAW_LUT_AGESPEEDMAX), RT_ERR_OUT_OF_RANGE);

    if ((ret = reg_field_write(APOLLOMP_LUT_CFGr, APOLLOMP_AGE_SPDf, &agingTime)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_l2_agingTime_set */

/* Function Name:
 *      apollomp_raw_l2_agingTime_get
 * Description:
 *      Get L2 aging time
 * Input:
 *      None
 * Output:
 *      pAgingTime 		- L2 aging time, unit 0.1sec
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_agingTime_get(uint32 *pAgingTime)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pAgingTime), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_LUT_CFGr, APOLLOMP_AGE_SPDf, pAgingTime)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_l2_agingTime_get */

/* Function Name:
 *      apollomp_raw_l2_ipmcLookupOp_set
 * Description:
 *      Set IPv4 multicast lookup hash method while hit IPMC table
 * Input:
 *      type 		- hash method
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_ipmcLookupOp_set(apollomp_raw_l2_ipMcLookupOp_t type)
{
    int32 ret;

	RT_PARAM_CHK((APOLLOMP_RAW_LUT_IPMCLOOKUP_TYPE_END <= type), RT_ERR_CHIP_NOT_SUPPORTED);

    if ((ret = reg_field_write(APOLLOMP_LUT_CFGr, APOLLOMP_LUT_IPMC_LOOKUP_OPf, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_l2_ipmcLookupOp_set */

/* Function Name:
 *      apollomp_raw_l2_ipmcLookupOp_get
 * Description:
 *      Get IPv4 multicast lookup hash method while hit IPMC table
 * Input:
 *      None
 * Output:
 *      pType 		- hash method
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_ipmcLookupOp_get(apollomp_raw_l2_ipMcLookupOp_t *pType)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_LUT_CFGr, APOLLOMP_LUT_IPMC_LOOKUP_OPf, pType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_l2_ipmcLookupOp_get */


/* Function Name:
 *      apollomp_raw_l2_ipmcHashType_set
 * Description:
 *      Set IPv4 multicast lookup hash method
 * Input:
 *      type 		- hash method
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32 apollomp_raw_l2_ipmcHashType_set(apollomp_raw_l2_ipMcHashType_t type)
{
    int32 ret;

	RT_PARAM_CHK((APOLLOMP_RAW_LUT_IPMCHASH_TYPE_END <= type), RT_ERR_CHIP_NOT_SUPPORTED);

    if ((ret = reg_field_write(APOLLOMP_LUT_CFGr, APOLLOMP_LUT_IPMC_HASHf, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_l2_ipmcHashType_set */

/* Function Name:
 *      apollomp_raw_l2_ipmcHashType_get
 * Description:
 *      Get IPv4 multicast lookup hash method
 * Input:
 *      None
 * Output:
 *      pType 		- hash method
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_ipmcHashType_get(apollomp_raw_l2_ipMcHashType_t *pType)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_LUT_CFGr, APOLLOMP_LUT_IPMC_HASHf, pType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_l2_ipmcHashType_set */


/* Function Name:
 *      apollomp_raw_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      enable 		- enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_l2_flushLinkDownPortAddrEnable_set(rtk_enable_t enable)
{
    int32 ret;

	RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_LUT_CFGr, APOLLOMP_LINKDOWN_AGEOUTf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      None
 * Output:
 *      pEnable 		- enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_flushLinkDownPortAddrEnable_get(rtk_enable_t *pEnable)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_LUT_CFGr, APOLLOMP_LINKDOWN_AGEOUTf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_unknReservedMcFlood_set
 * Description:
 *      Set unknown Reserved IP multicast address packet flooding state
 * Input:
 *      state   - ENABLED  : Always flooding, DISABLED : Treat as normal unknown IP multicast packets
 * Output:
 *      Nnoe.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unknReservedMcFlood_set(rtk_enable_t state)
{
    int32 ret;

	RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_UNKN_MC_CFGr, APOLLOMP_UNKN_MC_RES_FLOODf, &state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_l2_unknReservedMcFlood_get
 * Description:
 *      get unknown Reserved IP multicast address packet flooding state
 * Input:
 *      None.
 * Output:
 *      pState  - ENABLED  : Always flooding, DISABLED : Treat as normal unknown IP multicast packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_l2_unknReservedMcFlood_get(rtk_enable_t *pState)
{
    int32 ret;

    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_UNKN_MC_CFGr, APOLLOMP_UNKN_MC_RES_FLOODf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
}
