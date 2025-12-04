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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of Classifyication API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) classfication rule add/delete/get
 */



/*
 * Include Files
 */
#include <rtk/port.h>
#include <rtk/classify.h>
#include <hal/mac/reg.h>
#include <hal/mac/drv.h>
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/raw/apollomp_raw_cf.h>
#include <dal/apollomp/dal_apollomp_classify.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32    classify_init = INIT_NOT_COMPLETED;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      dal_apollomp_classify_init
 * Description:
 *      Initialize classification module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize classification module before calling any classification APIs.
 */
int32
dal_apollomp_classify_init(void)
{
    apollomp_raw_cf_RuleEntry_t entry;
    apollomp_raw_cf_dsAct_t ds_act;
    apollomp_raw_cf_usAct_t us_act;
    uint32  idx;
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* clear table */
    osal_memset(&entry, 0x0, sizeof(apollomp_raw_cf_RuleEntry_t));
    osal_memset(&ds_act, 0x0, sizeof(apollomp_raw_cf_dsAct_t));
    osal_memset(&us_act, 0x0, sizeof(apollomp_raw_cf_usAct_t));

    for(idx = 0; idx < APOLLOMP_CLASSIFY_ENTRY_MAX; idx++)
    {
        entry.idx  = idx;
        entry.valid = CF_ENTRY_VALID; /* always set to 1 for Tcam valid bit */
        ds_act.idx = idx;
        us_act.idx = idx;

        RT_ERR_CHK(apollomp_raw_cf_valid_set(entry.idx, CF_ENTRY_INVALID), ret);
        RT_ERR_CHK(apollomp_raw_cf_rule_set(&entry), ret);
        RT_ERR_CHK(apollomp_raw_cf_dsAct_set(&ds_act), ret);
        RT_ERR_CHK(apollomp_raw_cf_usAct_set(&us_act), ret);
        RT_ERR_CHK(apollomp_raw_cf_dsOp_set(entry.idx, CLASSIFY_INVERT_DISABLE), ret);
        RT_ERR_CHK(apollomp_raw_cf_usOp_set(entry.idx, CLASSIFY_INVERT_DISABLE), ret);
    }


    /*set the classify/ACL 1p remarking priority, default ACL > classify */
    data = 0;
    if((ret = reg_field_write(APOLLOMP_CF_US_1P_REMARK_ENr, APOLLOMP_CF_US_1P_REMARK_ENf, &data)) !=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    classify_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_init */

static int
_dal_apollomp_classify_cfg2raw_us_csAct(rtk_classify_us_csact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_US_CSACT_NOP:
            *pRawAct = CF_US_CSACT_NOP;
            break;
        case CLASSIFY_US_CSACT_ADD_TAG_VS_TPID:
            *pRawAct = CF_US_CSACT_ADD_TAG_VS_TPID;
            break;
        case CLASSIFY_US_CSACT_ADD_TAG_8100:
            *pRawAct = CF_US_CSACT_ADD_TAG_8100;
            break;
        case CLASSIFY_US_CSACT_DEL_STAG:
            *pRawAct = CF_US_CSACT_DEL_STAG;
            break;
        case CLASSIFY_US_CSACT_TRANSPARENT:
            *pRawAct = CF_US_CSACT_TRANSPARENT;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_us_cAct(rtk_classify_us_cact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_US_CACT_NOP:
            *pRawAct = CF_US_CACT_NOP;
            break;
        case CLASSIFY_US_CACT_TRANSLATION_C2S:
            *pRawAct = CF_US_CACT_TRANSLATION_C2S;
            break;
        case CLASSIFY_US_CACT_DEL_CTAG:
            *pRawAct = CF_US_CACT_DEL_CTAG;
            break;
        case CLASSIFY_US_CACT_TRANSPARENT:
            *pRawAct = CF_US_CACT_TRANSPARENT;
            break;
        case CLASSIFY_US_CACT_ADD_CTAG_8100:
            *pRawAct = CF_US_CACT_ADD_CTAG_8100;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_us_vidAct(rtk_classify_us_vid_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_US_VID_ACT_ASSIGN:
            *pRawAct = CF_US_VID_ACT_ASSIGN;
            break;
        case CLASSIFY_US_VID_ACT_FROM_1ST_TAG:
            *pRawAct = CF_US_VID_ACT_FROM_1ST_TAG;
            break;
        case CLASSIFY_US_VID_ACT_FROM_2ND_TAG:
            *pRawAct = CF_US_VID_ACT_FROM_2ND_TAG;
            break;
        case CLASSIFY_US_VID_ACT_FROM_INTERNAL:
            *pRawAct = CF_US_VID_ACT_FROM_INTERNAL;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_us_priAct(rtk_classify_us_pri_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_US_PRI_ACT_ASSIGN:
            *pRawAct = CF_US_PRI_ACT_ASSIGN;
            break;
        case CLASSIFY_US_PRI_ACT_FROM_1ST_TAG:
            *pRawAct = CF_US_PRI_ACT_FROM_1ST_TAG;
            break;
        case CLASSIFY_US_PRI_ACT_FROM_INTERNAL:
            *pRawAct = CF_US_PRI_ACT_FROM_INTERNAL;
            break;
        case CLASSIFY_US_PRI_ACT_FROM_2ND_TAG:
            *pRawAct = CF_US_PRI_ACT_FROM_2ND_TAG;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_ds_csAct(rtk_classify_ds_csact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DS_CSACT_NOP:
            *pRawAct = CF_DS_CSACT_NOP;
            break;
        case CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID:
            *pRawAct = CF_DS_CSACT_ADD_TAG_VS_TPID;
            break;
        case CLASSIFY_DS_CSACT_ADD_TAG_8100:
            *pRawAct = CF_DS_CSACT_ADD_TAG_8100;
            break;
        case CLASSIFY_DS_CSACT_DEL_STAG:
            *pRawAct = CF_DS_CSACT_DEL_STAG;
            break;
        case CLASSIFY_DS_CSACT_TRANSPARENT:
            *pRawAct = CF_DS_CSACT_TRANSPARENT;
            break;
        case CLASSIFY_DS_CSACT_SP2C:
            *pRawAct = CF_DS_CSACT_SP2C;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_ds_cAct(rtk_classify_ds_cact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DS_CACT_NOP:
            *pRawAct = CF_DS_CACT_NOP;
            break;
        case CLASSIFY_DS_CACT_ADD_CTAG_8100:
            *pRawAct = CF_DS_CACT_ADD_CTAG_8100;
            break;
        case CLASSIFY_DS_CACT_TRANSLATION_SP2C:
            *pRawAct = CF_DS_CACT_TRANSLATION_SP2C;
            break;
        case CLASSIFY_DS_CACT_TRANSPARENT:
            *pRawAct = CF_DS_CACT_TRANSPARENT;
            break;
        case CLASSIFY_DS_CACT_DEL_CTAG:
            *pRawAct = CF_DS_CACT_DEL_CTAG;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_ds_vidAct(rtk_classify_ds_vid_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DS_VID_ACT_ASSIGN:
            *pRawAct = CF_DS_VID_ACT_ASSIGN;
            break;
        case CLASSIFY_DS_VID_ACT_FROM_1ST_TAG:
            *pRawAct = CF_DS_VID_ACT_FROM_1ST_TAG;
            break;
        case CLASSIFY_DS_VID_ACT_FROM_2ND_TAG:
            *pRawAct = CF_DS_VID_ACT_FROM_2ND_TAG;
            break;
        case CLASSIFY_DS_VID_ACT_FROM_LUT:
            *pRawAct = CF_DS_VID_ACT_FROM_LUT;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_ds_priAct(rtk_classify_ds_pri_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DS_PRI_ACT_ASSIGN:
            *pRawAct = CF_DS_PRI_ACT_ASSIGN;
            break;
        case CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG:
            *pRawAct = CF_DS_PRI_ACT_FROM_1ST_TAG;
            break;
        case CLASSIFY_DS_PRI_ACT_FROM_INTERNAL:
            *pRawAct = CF_DS_PRI_ACT_FROM_INTERNAL;
            break;
        case CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG:
            *pRawAct = CF_DS_PRI_ACT_FROM_2ND_TAG;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_cfPriAct(rtk_classify_cf_pri_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_CF_PRI_ACT_NOP:
            *pRawAct = CF_CF_PRI_ACT_NOP;
            break;
        case CLASSIFY_CF_PRI_ACT_ASSIGN:
            *pRawAct = CF_CF_PRI_ACT_ASSIGN;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_dscpAct(rtk_classify_dscp_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DSCP_ACT_DISABLE:
            *pRawAct = CF_DISABLE;
            break;
        case CLASSIFY_DSCP_ACT_ENABLE:
            *pRawAct = CF_ENABLE;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}


static int
_dal_apollomp_classify_cfg2raw_uniAct(rtk_classify_ds_uni_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    /*assign uni act, since new register designed, 2012.05.28*/
	 switch(cfgAct){
	 case CLASSIFY_DS_UNI_ACT_NOP:
	 	 *pRawAct  = CF_DS_UNI_ACT_NOP;
	 break;
	 case CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK:
		 *pRawAct   = CF_DS_UNI_ACT_MASK_BY_UNIMASK;
 	 break;
	 case CLASSIFY_DS_UNI_ACT_FORCE_FORWARD:
	 	*pRawAct   = CF_DS_UNI_ACT_FORCE_FORWARD ;
	 break;
	 default:
	 	ret = RT_ERR_CHIP_NOT_SUPPORTED;
	 break;
	 }
	 return ret;
}


static int
_dal_apollomp_classify_cfg2raw_dropAct(rtk_classify_drop_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DROP_ACT_NONE:
            *pRawAct = CF_DISABLE;
            break;
        case CLASSIFY_DROP_ACT_ENABLE:
            *pRawAct = CF_ENABLE;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_logAct(rtk_classify_log_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_US_LOG_ACT_NONE:
            *pRawAct = CF_DISABLE;
            break;
        case CLASSIFY_US_LOG_ACT_ENABLE:
            *pRawAct = CF_ENABLE;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_cfg2raw_sidAct(rtk_classify_us_sqid_act_t sqidAct, uint32 *pRawAct)
{
       int32 ret=RT_ERR_OK;

	 /*assign sid act, since new register designed, 2012.05.28*/
	 switch(sqidAct){
	 case CLASSIFY_US_SQID_ACT_ASSIGN_SID:
	 	*pRawAct  = CF_US_SQID_ACT_ASSIGN_SID;
	 break;
	 case CLASSIFY_US_SQID_ACT_ASSIGN_NOP:
	 	*pRawAct   =CF_US_SQID_ACT_ASSIGN_NOP ;
	 break;
	 case CLASSIFY_US_SQID_ACT_ASSIGN_QID:
	 default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
	 break;
	 }

	 return ret;
}

static int
_dal_apollomp_classify_raw2cfg_us_csAct(uint32 rawAct, rtk_classify_us_csact_t *pCcfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_US_CSACT_NOP:
            *pCcfgAct = CLASSIFY_US_CSACT_NOP;
            break;
        case CF_US_CSACT_ADD_TAG_VS_TPID:
            *pCcfgAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID;
            break;
        case CF_US_CSACT_ADD_TAG_8100:
            *pCcfgAct = CLASSIFY_US_CSACT_ADD_TAG_8100;
            break;
        case CF_US_CSACT_DEL_STAG:
            *pCcfgAct = CLASSIFY_US_CSACT_DEL_STAG;
            break;
        case CF_US_CSACT_TRANSPARENT:
            *pCcfgAct = CLASSIFY_US_CSACT_TRANSPARENT;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_us_cAct(uint32 rawAct, rtk_classify_us_cact_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_US_CACT_NOP:
            *pCfgAct = CLASSIFY_US_CACT_NOP;
            break;
        case CF_US_CACT_ADD_CTAG_8100:
            *pCfgAct = CLASSIFY_US_CACT_ADD_CTAG_8100;
            break;
        case CF_US_CACT_DEL_CTAG:
            *pCfgAct = CLASSIFY_US_CACT_DEL_CTAG;
            break;
        case CF_US_CACT_TRANSLATION_C2S:
            *pCfgAct = CLASSIFY_US_CACT_TRANSLATION_C2S;
            break;
        case CF_US_CACT_TRANSPARENT:
            *pCfgAct = CLASSIFY_US_CACT_TRANSPARENT;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_us_vidAct(uint32 rawAct, rtk_classify_us_vid_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_US_VID_ACT_ASSIGN:
            *pCfgAct = CLASSIFY_US_VID_ACT_ASSIGN;
            break;
        case CF_US_VID_ACT_FROM_1ST_TAG:
            *pCfgAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
            break;
        case CF_US_VID_ACT_FROM_2ND_TAG:
            *pCfgAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
            break;
        case CF_US_VID_ACT_FROM_INTERNAL:
            *pCfgAct = CLASSIFY_US_VID_ACT_FROM_INTERNAL;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_us_priAct(uint32 rawAct, rtk_classify_us_pri_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_US_PRI_ACT_ASSIGN:
            *pCfgAct = CLASSIFY_US_PRI_ACT_ASSIGN;
            break;
        case CF_US_PRI_ACT_FROM_1ST_TAG:
            *pCfgAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
            break;
        case CF_US_PRI_ACT_FROM_2ND_TAG:
            *pCfgAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
            break;
        case CF_US_PRI_ACT_FROM_INTERNAL:
            *pCfgAct = CLASSIFY_US_PRI_ACT_FROM_INTERNAL;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_ds_csAct(uint32 rawAct, rtk_classify_ds_csact_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_DS_CSACT_NOP:
            *pCfgAct = CLASSIFY_DS_CSACT_NOP;
            break;
        case CF_DS_CSACT_ADD_TAG_VS_TPID:
            *pCfgAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID;
            break;
        case CF_DS_CSACT_ADD_TAG_8100:
            *pCfgAct = CLASSIFY_DS_CSACT_ADD_TAG_8100;
            break;
        case CF_DS_CSACT_DEL_STAG:
            *pCfgAct = CLASSIFY_DS_CSACT_DEL_STAG;
            break;
        case CF_DS_CSACT_TRANSPARENT:
            *pCfgAct = CLASSIFY_DS_CSACT_TRANSPARENT;
            break;
        case CF_DS_CSACT_SP2C:
            *pCfgAct = CLASSIFY_DS_CSACT_SP2C;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_ds_cAct(uint32 rawAct, rtk_classify_ds_cact_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_DS_CACT_NOP:
            *pCfgAct = CLASSIFY_DS_CACT_NOP;
            break;
        case CF_DS_CACT_ADD_CTAG_8100:
            *pCfgAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
            break;
        case CF_DS_CACT_TRANSLATION_SP2C:
            *pCfgAct = CLASSIFY_DS_CACT_TRANSLATION_SP2C;
            break;
        case CF_DS_CACT_DEL_CTAG:
            *pCfgAct = CLASSIFY_DS_CACT_DEL_CTAG;
            break;
        case CF_DS_CACT_TRANSPARENT:
            *pCfgAct = CLASSIFY_DS_CACT_TRANSPARENT;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_ds_vidAct(uint32 rawAct, rtk_classify_ds_vid_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_DS_VID_ACT_ASSIGN:
            *pCfgAct = CLASSIFY_DS_VID_ACT_ASSIGN;
            break;
        case CF_DS_VID_ACT_FROM_1ST_TAG:
            *pCfgAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
            break;
        case CF_DS_VID_ACT_FROM_2ND_TAG:
            *pCfgAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
            break;
        case CF_DS_VID_ACT_FROM_LUT:
            *pCfgAct = CLASSIFY_DS_VID_ACT_FROM_LUT;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_ds_priAct(uint32 rawAct, rtk_classify_ds_pri_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_DS_PRI_ACT_ASSIGN:
            *pCfgAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
            break;
        case CF_DS_PRI_ACT_FROM_1ST_TAG:
            *pCfgAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
            break;
        case CF_DS_PRI_ACT_FROM_2ND_TAG:
            *pCfgAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
            break;
        case CF_DS_PRI_ACT_FROM_INTERNAL:
            *pCfgAct = CLASSIFY_DS_PRI_ACT_FROM_INTERNAL;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_cfPriAct(uint32 rawAct, rtk_classify_cf_pri_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_CF_PRI_ACT_NOP:
            *pCfgAct = CLASSIFY_CF_PRI_ACT_NOP;
            break;
        case CF_CF_PRI_ACT_ASSIGN:
            *pCfgAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_dscpAct(uint32 rawAct, rtk_classify_dscp_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_DISABLE:
            *pCfgAct = CLASSIFY_DSCP_ACT_DISABLE;
            break;
        case CF_ENABLE:
            *pCfgAct = CLASSIFY_DSCP_ACT_ENABLE;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_dropAct(uint32 rawAct, rtk_classify_drop_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_DISABLE:
            *pCfgAct = CLASSIFY_DROP_ACT_NONE;
            break;
        case CF_ENABLE:
            *pCfgAct = CLASSIFY_DROP_ACT_ENABLE;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_apollomp_classify_raw2cfg_logAct(uint32 rawAct, rtk_classify_log_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_DISABLE:
            *pCfgAct = CLASSIFY_US_LOG_ACT_NONE;
            break;
        case CF_ENABLE:
            *pCfgAct = CLASSIFY_US_LOG_ACT_ENABLE;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}



static int
_dal_apollomp_classify_raw2cfg_sidAct(uint32 rawAct, rtk_classify_us_sqid_act_t *psidAct)
{
	int32 ret=RT_ERR_OK;

	/*assign sid act, since new register designed, 2012.05.28*/
	switch(rawAct){
	case CF_US_SQID_ACT_ASSIGN_SID:
		*psidAct = CLASSIFY_US_SQID_ACT_ASSIGN_SID;
	break;
	case CF_US_SQID_ACT_ASSIGN_NOP :
		*psidAct    =CLASSIFY_US_SQID_ACT_ASSIGN_NOP ;
	break;
	default:
		ret = RT_ERR_CHIP_NOT_SUPPORTED;
	break;
	}
	return ret;
}

static int
_dal_apollomp_classify_raw2cfg_uniAct(uint32 rawAct, rtk_classify_ds_uni_act_t *puniAct)
{
	int32 ret=RT_ERR_OK;

	/*assign uni act, since new register designed, 2012.05.28*/
	switch(rawAct) {
	case CF_DS_UNI_ACT_NOP:
		*puniAct   = CLASSIFY_DS_UNI_ACT_NOP;
	break;
	case CF_DS_UNI_ACT_MASK_BY_UNIMASK:
		*puniAct  = CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK;
	break;
	case CF_DS_UNI_ACT_FORCE_FORWARD:
		*puniAct   = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD ;
	break;
	default:
		ret = RT_ERR_CHIP_NOT_SUPPORTED;
	break;
	}
      return ret;
}


static int
dal_apollomp_classify_cfg2entry(rtk_classify_cfg_t          *pClassifyCfg,
                                apollomp_raw_cf_RuleEntry_t *pRule,
                                apollomp_raw_cf_usAct_t     *pUsAction,
                                apollomp_raw_cf_dsAct_t     *pDsAction)
{
    rtk_classify_field_t *tmp_field;
    int32 ret=RT_ERR_OK;

    osal_memset(pRule, 0, sizeof(apollomp_raw_cf_RuleEntry_t));
    osal_memset(pUsAction, 0, sizeof(apollomp_raw_cf_usAct_t));
    osal_memset(pDsAction, 0, sizeof(apollomp_raw_cf_dsAct_t));

    /* rule */
    pRule->idx   = pClassifyCfg->index;
    pRule->valid = 1;
    pRule->data_bits.direction  = pClassifyCfg->direction;
    pRule->care_bits.direction  = 0x1;
    tmp_field = pClassifyCfg->field.pFieldHead;
    while(tmp_field != NULL)
    {
        switch(tmp_field->fieldType)
        {
            case CLASSIFY_FIELD_ETHERTYPE:
                pRule->data_bits.etherType = tmp_field->classify_pattern.etherType.value;
                pRule->care_bits.etherType = tmp_field->classify_pattern.etherType.mask;
                break;

            case CLASSIFY_FIELD_TOS_DSIDX:
                pRule->data_bits.tos_gemidx = tmp_field->classify_pattern.tosDsidx.value;
                pRule->care_bits.tos_gemidx = tmp_field->classify_pattern.tosDsidx.mask;
                break;

            case CLASSIFY_FIELD_TAG_VID:
                pRule->data_bits.tagVid = tmp_field->classify_pattern.tagVid.value;
                pRule->care_bits.tagVid = tmp_field->classify_pattern.tagVid.mask;
                break;

            case CLASSIFY_FIELD_TAG_PRI:
                pRule->data_bits.tagPri = tmp_field->classify_pattern.tagPri.value;
                pRule->care_bits.tagPri = tmp_field->classify_pattern.tagPri.mask;
                break;

            case CLASSIFY_FIELD_INTER_PRI:
                pRule->data_bits.interPri = tmp_field->classify_pattern.interPri.value;
                pRule->care_bits.interPri = tmp_field->classify_pattern.interPri.mask;
                break;

            case CLASSIFY_FIELD_IS_CTAG:
                pRule->data_bits.ifCtag = tmp_field->classify_pattern.isCtag.value;
                pRule->care_bits.ifCtag = tmp_field->classify_pattern.isCtag.mask;
                break;

            case CLASSIFY_FIELD_IS_STAG:
                pRule->data_bits.ifStag = tmp_field->classify_pattern.isStag.value;
                pRule->care_bits.ifStag = tmp_field->classify_pattern.isStag.mask;
                break;

            case CLASSIFY_FIELD_UNI:
                pRule->data_bits.uni = tmp_field->classify_pattern.uni.value;
                pRule->care_bits.uni = tmp_field->classify_pattern.uni.mask;
                break;

            case CLASSIFY_FIELD_PORT_RANGE:
                pRule->data_bits.portRange = tmp_field->classify_pattern.portRange.value;
                pRule->care_bits.portRange = tmp_field->classify_pattern.portRange.mask;
                break;

            case CLASSIFY_FIELD_IP_RANGE:
                pRule->data_bits.ipRange = tmp_field->classify_pattern.ipRange.value;
                pRule->care_bits.ipRange = tmp_field->classify_pattern.ipRange.mask;
                break;

            case CLASSIFY_FIELD_ACL_HIT:
                pRule->data_bits.aclHit = tmp_field->classify_pattern.aclHit.value;
                pRule->care_bits.aclHit = tmp_field->classify_pattern.aclHit.mask;
                break;

            case CLASSIFY_FIELD_WAN_IF:
                pRule->data_bits.wanIf = tmp_field->classify_pattern.wanIf.value;
                pRule->care_bits.wanIf = tmp_field->classify_pattern.wanIf.mask;
                break;

            case CLASSIFY_FIELD_IP6_MC:
                pRule->data_bits.ip6Mc = tmp_field->classify_pattern.ip6Mc.value;
                pRule->care_bits.ip6Mc = tmp_field->classify_pattern.ip6Mc.mask;
                break;

            case CLASSIFY_FIELD_IP4_MC:
                pRule->data_bits.ip4Mc = tmp_field->classify_pattern.ip4Mc.value;
                pRule->care_bits.ip4Mc = tmp_field->classify_pattern.ip4Mc.mask;
                break;

            case CLASSIFY_FIELD_MLD:
                pRule->data_bits.mld = tmp_field->classify_pattern.mld.value;
                pRule->care_bits.mld = tmp_field->classify_pattern.mld.mask;
                break;

            case CLASSIFY_FIELD_IGMP:
                pRule->data_bits.igmp = tmp_field->classify_pattern.igmp.value;
                pRule->care_bits.igmp = tmp_field->classify_pattern.igmp.mask;
                break;

            case CLASSIFY_FIELD_DEI:
                pRule->data_bits.dei = tmp_field->classify_pattern.dei.value;
                pRule->care_bits.dei = tmp_field->classify_pattern.dei.mask;
                break;

            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }

        tmp_field = tmp_field->next;
    }

    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {   /* us action */
        pUsAction->idx = pClassifyCfg->index;
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_us_csAct(pClassifyCfg->act.usAct.csAct, &pUsAction->csact), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_us_vidAct(pClassifyCfg->act.usAct.csVidAct, &pUsAction->svid_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_us_priAct(pClassifyCfg->act.usAct.csPriAct, &pUsAction->spri_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_us_cAct(pClassifyCfg->act.usAct.cAct, &pUsAction->cact), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_us_vidAct(pClassifyCfg->act.usAct.cVidAct, &pUsAction->cvid_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_us_priAct(pClassifyCfg->act.usAct.cPriAct, &pUsAction->cpri_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_cfPriAct(pClassifyCfg->act.usAct.interPriAct, &pUsAction->cfpri_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_dscpAct(pClassifyCfg->act.usAct.dscp, &pUsAction->dscp_remark), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_dropAct(pClassifyCfg->act.usAct.drop, &pUsAction->drop_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_logAct(pClassifyCfg->act.usAct.log, &pUsAction->log_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_sidAct(pClassifyCfg->act.usAct.sidQidAct, &pUsAction->sid_act), ret);

	  pUsAction->s_vid        = pClassifyCfg->act.usAct.sTagVid;
        pUsAction->s_pri        = pClassifyCfg->act.usAct.sTagPri;
        pUsAction->c_vid        = pClassifyCfg->act.usAct.cTagVid;
        pUsAction->c_pri        = pClassifyCfg->act.usAct.cTagPri;
        pUsAction->assign_idx   = pClassifyCfg->act.usAct.sidQid;
        pUsAction->cfpri        = pClassifyCfg->act.usAct.cfPri;
        pUsAction->log_idx      = pClassifyCfg->act.usAct.logCntIdx;



    }
    else /* ds action */
    {
        pDsAction->idx = pClassifyCfg->index;
        /*Modify DS csVidAct to copy outer when csAct is remove, for keep ori svid to CPU*/
        if(pClassifyCfg->act.dsAct.csAct == CLASSIFY_DS_CSACT_DEL_STAG)
        {
            pClassifyCfg->act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
        }
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_ds_csAct(pClassifyCfg->act.dsAct.csAct, &pDsAction->csact), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_ds_vidAct(pClassifyCfg->act.dsAct.csVidAct, &pDsAction->svid_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_ds_priAct(pClassifyCfg->act.dsAct.csPriAct, &pDsAction->spri_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_ds_cAct(pClassifyCfg->act.dsAct.cAct, &pDsAction->cact), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_ds_vidAct(pClassifyCfg->act.dsAct.cVidAct, &pDsAction->cvid_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_ds_priAct(pClassifyCfg->act.dsAct.cPriAct, &pDsAction->cpri_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_cfPriAct(pClassifyCfg->act.dsAct.interPriAct, &pDsAction->cfpri_act), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_dscpAct(pClassifyCfg->act.dsAct.dscp, &pDsAction->dscp_remark), ret);
        RT_ERR_CHK(_dal_apollomp_classify_cfg2raw_uniAct(pClassifyCfg->act.dsAct.uniAct, &pDsAction->uni_act), ret);

        pDsAction->s_vid        = pClassifyCfg->act.dsAct.sTagVid;
        pDsAction->s_pri        = pClassifyCfg->act.dsAct.sTagPri;
        pDsAction->c_vid        = pClassifyCfg->act.dsAct.cTagVid;
        pDsAction->c_pri        = pClassifyCfg->act.dsAct.cTagPri;
        pDsAction->cfpri        = pClassifyCfg->act.dsAct.cfPri;
        pDsAction->uni_mask     = pClassifyCfg->act.dsAct.uniMask.bits[0];

    }
    return ret;
}

static int
dal_apollomp_classify_entry2cfg(apollomp_raw_cf_usAct_t     *pUsAction,
                                apollomp_raw_cf_dsAct_t     *pDsAction,
                                rtk_classify_cfg_t          *pClassifyCfg)
{
    int32 ret=RT_ERR_OK;

    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {   /* us action */
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_us_csAct(pUsAction->csact, &pClassifyCfg->act.usAct.csAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_us_vidAct(pUsAction->svid_act, &pClassifyCfg->act.usAct.csVidAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_us_priAct(pUsAction->spri_act, &pClassifyCfg->act.usAct.csPriAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_us_cAct(pUsAction->cact, &pClassifyCfg->act.usAct.cAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_us_vidAct(pUsAction->cvid_act, &pClassifyCfg->act.usAct.cVidAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_us_priAct(pUsAction->cpri_act, &pClassifyCfg->act.usAct.cPriAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_cfPriAct(pUsAction->cfpri_act, &pClassifyCfg->act.usAct.interPriAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_dscpAct(pUsAction->dscp_remark, &pClassifyCfg->act.usAct.dscp), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_dropAct(pUsAction->drop_act, &pClassifyCfg->act.usAct.drop), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_logAct(pUsAction->log_act, &pClassifyCfg->act.usAct.log), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_sidAct(pUsAction->sid_act, &pClassifyCfg->act.usAct.sidQidAct), ret);

	  pClassifyCfg->act.usAct.sTagVid     = pUsAction->s_vid;
        pClassifyCfg->act.usAct.sTagPri     = pUsAction->s_pri;
        pClassifyCfg->act.usAct.cTagVid     = pUsAction->c_vid;
        pClassifyCfg->act.usAct.cTagPri     = pUsAction->c_pri;
        pClassifyCfg->act.usAct.sidQid      = pUsAction->assign_idx;
        pClassifyCfg->act.usAct.cfPri       = pUsAction->cfpri;
        pClassifyCfg->act.usAct.logCntIdx   = pUsAction->log_idx;

    }
    else /* ds action */
    {
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_ds_csAct(pDsAction->csact, &pClassifyCfg->act.dsAct.csAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_ds_vidAct(pDsAction->svid_act, &pClassifyCfg->act.dsAct.csVidAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_ds_priAct(pDsAction->spri_act, &pClassifyCfg->act.dsAct.csPriAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_ds_cAct(pDsAction->cact, &pClassifyCfg->act.dsAct.cAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_ds_vidAct(pDsAction->cvid_act, &pClassifyCfg->act.dsAct.cVidAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_ds_priAct(pDsAction->cpri_act, &pClassifyCfg->act.dsAct.cPriAct), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_dscpAct(pDsAction->dscp_remark, &pClassifyCfg->act.dsAct.dscp), ret);
        RT_ERR_CHK(_dal_apollomp_classify_raw2cfg_uniAct(pDsAction->uni_act, &pClassifyCfg->act.dsAct.uniAct), ret);
        pClassifyCfg->act.dsAct.sTagVid     = pDsAction->s_vid;
        pClassifyCfg->act.dsAct.sTagPri     = pDsAction->s_pri;
        pClassifyCfg->act.dsAct.cTagVid     = pDsAction->c_vid;
        pClassifyCfg->act.dsAct.cTagPri     = pDsAction->c_pri;
        pClassifyCfg->act.dsAct.interPriAct = pDsAction->cfpri_act;
        pClassifyCfg->act.dsAct.cfPri       = pDsAction->cfpri;
        pClassifyCfg->act.dsAct.uniMask.bits[0] = pDsAction->uni_mask;


    }
    return ret;
}

/* Function Name:
 *      dal_apollomp_classify_cfgEntry_add
 * Description:
 *      Add an classification entry to ASIC
 * Input:
 *      entryIdx       - index of classification entry.
 *      pClassifyCfg     - The classification configuration that this function will add comparison rule
 *      pClassifyAct     - Action(s) of classification configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None.
 */
int32
dal_apollomp_classify_cfgEntry_add(rtk_classify_cfg_t *pClassifyCfg)
{
    apollomp_raw_cf_RuleEntry_t entry;
    apollomp_raw_cf_dsAct_t ds_act;
    apollomp_raw_cf_usAct_t us_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyCfg), RT_ERR_NULL_POINTER);

    /* translate data structure */
    RT_ERR_CHK(dal_apollomp_classify_cfg2entry(pClassifyCfg, &entry, &us_act, &ds_act), ret);

    /* set entry invalid */
    RT_ERR_CHK(apollomp_raw_cf_valid_set(entry.idx, CF_ENTRY_INVALID), ret);

    /* set entry rule */
    RT_ERR_CHK(apollomp_raw_cf_rule_set(&entry), ret);

    /* set entry action and set not operation */
    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {
        RT_ERR_CHK(apollomp_raw_cf_usAct_set(&us_act), ret);
        RT_ERR_CHK(apollomp_raw_cf_usOp_set(pClassifyCfg->index, pClassifyCfg->invert), ret);
    }
    else
    {
        RT_ERR_CHK(apollomp_raw_cf_dsAct_set(&ds_act), ret);
        RT_ERR_CHK(apollomp_raw_cf_dsOp_set(pClassifyCfg->index, pClassifyCfg->invert), ret);
    }

    /* set entry valid */
    RT_ERR_CHK(apollomp_raw_cf_valid_set(entry.idx, CF_ENTRY_VALID), ret);

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_cfgEntry_add */


/* Function Name:
 *      dal_apollomp_classify_cfgEntry_get
 * Description:
 *      Gdd an classification entry from ASIC
 * Input:
 *      None.
 * Output:
 *      pClassifyCfg     - The classification configuration that this function will add comparison rule
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None.
 */
int32
dal_apollomp_classify_cfgEntry_get(rtk_classify_cfg_t *pClassifyCfg)
{
    uint32 entryIdx;
    uint32 patternIdx;
    uint32 fieldData[APOLLOMP_RAW_CF_ENTRYLEN];
    uint32 fieldMask[APOLLOMP_RAW_CF_ENTRYLEN];
    apollomp_raw_cf_dsAct_t ds_act;
    apollomp_raw_cf_usAct_t us_act;
    rtk_enable_t not_op;
    int32 ret;
    uint32 idx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_CLASSIFY_ENTRY_MAX() <= pClassifyCfg->index), RT_ERR_NULL_POINTER);

    /* get entry valid */
    RT_ERR_CHK(apollomp_raw_cf_valid_get(pClassifyCfg->index, &pClassifyCfg->valid), ret);

    /* get entry rule raw data */
    entryIdx = pClassifyCfg->index&0x7f;
    patternIdx = pClassifyCfg->index>>7;
    RT_ERR_CHK(table_read(APOLLOMP_CF_RULEt, entryIdx, fieldData), ret);
    RT_ERR_CHK(table_read(APOLLOMP_CF_MASKt, entryIdx, fieldMask), ret);
    for(idx=0; idx<APOLLOMP_RAW_CF_ENTRYLEN; idx++)
        fieldMask[idx] = fieldMask[idx]^fieldData[idx];

    osal_memset(&pClassifyCfg->field.readField, 0, sizeof(rtk_classify_raw_field_t));	
    pClassifyCfg->field.readField.dataFieldRaw[0] = (fieldData[patternIdx+1]&0xFFFF);
    pClassifyCfg->field.readField.dataFieldRaw[1] = ((fieldData[patternIdx+1]>>16)&0xFFFF);
    pClassifyCfg->field.readField.careFieldRaw[0] = (fieldMask[patternIdx+1]&0xFFFF);
    pClassifyCfg->field.readField.careFieldRaw[1] = ((fieldMask[patternIdx+1]>>16)&0xFFFF);
    if(0 == patternIdx)
    {
        pClassifyCfg->field.readField.dataFieldRaw[2] = (fieldData[patternIdx]&0xFFFF);
        pClassifyCfg->field.readField.careFieldRaw[2] = (fieldMask[patternIdx]&0xFFFF);
    }

    /* direction: bit 31 */
     if(((pClassifyCfg->field.readField.dataFieldRaw[1] & 0x8000) &
         (pClassifyCfg->field.readField.careFieldRaw[1] & 0x8000)) == 0)
         pClassifyCfg->direction = CLASSIFY_DIRECTION_US;
     else
         pClassifyCfg->direction = CLASSIFY_DIRECTION_DS;

    osal_memset(&ds_act, 0, sizeof(apollomp_raw_cf_dsAct_t));
    osal_memset(&us_act, 0, sizeof(apollomp_raw_cf_usAct_t));
    ds_act.idx = pClassifyCfg->index;
    us_act.idx = pClassifyCfg->index;
    /* get entry action and get not operation */
    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {
        RT_ERR_CHK(apollomp_raw_cf_usAct_get(&us_act), ret);
        RT_ERR_CHK(apollomp_raw_cf_usOp_get(pClassifyCfg->index, &not_op), ret);
    }
    else
    {
        RT_ERR_CHK(apollomp_raw_cf_dsAct_get(&ds_act), ret);
        RT_ERR_CHK(apollomp_raw_cf_dsOp_get(pClassifyCfg->index, &not_op), ret);
    }

    /* translate data structure */
    RT_ERR_CHK(dal_apollomp_classify_entry2cfg(&us_act, &ds_act, pClassifyCfg), ret);
    if(DISABLED == not_op)
        pClassifyCfg->invert = CLASSIFY_INVERT_DISABLE;
    else
        pClassifyCfg->invert = CLASSIFY_INVERT_ENABLE;

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_cfgEntry_get */


/* Function Name:
 *      dal_apollomp_classify_cfgEntry_del
 * Description:
 *      Delete an classification configuration from ASIC
 * Input:
 *      entryIdx    - index of classification entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_ENTRY_INDEX 		- Invalid classification index .
 * Note:
 *      None.
 */
int32
dal_apollomp_classify_cfgEntry_del(uint32 entryIdx)
{
    apollomp_raw_cf_RuleEntry_t entry;
    apollomp_raw_cf_dsAct_t ds_act;
    apollomp_raw_cf_usAct_t us_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_ENTRY_MAX() <= entryIdx), RT_ERR_INPUT);

    osal_memset(&entry, 0x0, sizeof(apollomp_raw_cf_RuleEntry_t));
    osal_memset(&ds_act, 0x0, sizeof(apollomp_raw_cf_dsAct_t));
    osal_memset(&us_act, 0x0, sizeof(apollomp_raw_cf_usAct_t));

    entry.idx  = entryIdx;
    entry.valid = 1; /* always set to 1 for tcam valid bit */
    ds_act.idx = entryIdx;
    us_act.idx = entryIdx;

    RT_ERR_CHK(apollomp_raw_cf_valid_set(entryIdx, CF_ENTRY_INVALID), ret);

    RT_ERR_CHK(apollomp_raw_cf_rule_set(&entry), ret);
    RT_ERR_CHK(apollomp_raw_cf_dsAct_set(&ds_act), ret);
    RT_ERR_CHK(apollomp_raw_cf_usAct_set(&us_act), ret);

    RT_ERR_CHK(apollomp_raw_cf_dsOp_set(entryIdx, CLASSIFY_INVERT_DISABLE), ret);
    RT_ERR_CHK(apollomp_raw_cf_usOp_set(entryIdx, CLASSIFY_INVERT_DISABLE), ret);

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_cfgEntry_del */


/* Function Name:
 *      dal_apollomp_classify_field_add
 * Description:
 *      Add comparison field to an classfication configuration
 * Input:
 *      pClassifyEntry     - The classfication configuration that this function will add comparison rule
 *      pClassifyField     - The comparison rule that will be added.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER    	- Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT 			- Invalid input parameters.
 * Note:
 *      This function add a comparison rule (*pClassifyField) to an ACL configuration (*pClassifyEntry).
 *      Pointer pFieldHead points to an Classification configuration structure, this structure keeps multiple
 *      comparison rules by means of linked list. Pointer pClassifyField will be added to linked
 *      list keeped by structure that pClassifyEntry points to.
 *      - caller should not free (*pClassifyField) before dal_apollomp_classify_cfgEntry_add is called
 */
int32
dal_apollomp_classify_field_add(rtk_classify_cfg_t *pClassifyEntry, rtk_classify_field_t *pClassifyField)
{
    rtk_classify_field_t *tmp_field;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pClassifyField), RT_ERR_NULL_POINTER);

    switch(pClassifyField->fieldType)
    {
        case CLASSIFY_FIELD_ETHERTYPE:
        case CLASSIFY_FIELD_TOS_DSIDX:
        case CLASSIFY_FIELD_TAG_VID:
        case CLASSIFY_FIELD_TAG_PRI:
        case CLASSIFY_FIELD_INTER_PRI:
        case CLASSIFY_FIELD_IS_CTAG:
        case CLASSIFY_FIELD_IS_STAG:
        case CLASSIFY_FIELD_UNI:

        case CLASSIFY_FIELD_PORT_RANGE:
        case CLASSIFY_FIELD_IP_RANGE:
        case CLASSIFY_FIELD_ACL_HIT:
        case CLASSIFY_FIELD_WAN_IF:
        case CLASSIFY_FIELD_IP6_MC:
        case CLASSIFY_FIELD_IP4_MC:
        case CLASSIFY_FIELD_MLD:
        case CLASSIFY_FIELD_IGMP:
        case CLASSIFY_FIELD_DEI:
        break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
        break;

    }

    if(NULL == pClassifyEntry->field.pFieldHead)
    {
        pClassifyEntry->field.pFieldHead = pClassifyField;
        pClassifyEntry->field.pFieldHead->next = NULL;
    }
    else
    {
        tmp_field = pClassifyEntry->field.pFieldHead;
        while(tmp_field->next != NULL)
            tmp_field = tmp_field->next;

        pClassifyField->next = NULL;
        tmp_field->next = pClassifyField;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_field_add */


/* Function Name:
 *      dal_apollomp_classify_unmatchAction_set
 * Description:
 *      Apply action to packets when no classfication configuration match
 * Input:
 *      None
 * Output:
 *      action - unmatch action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no classfication configruation matches.
 */
int32
dal_apollomp_classify_unmatchAction_set(rtk_classify_unmatch_action_t action)
{
    apollomp_raw_cf_usPermit_t raw_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((CLASSIFY_UNMATCH_END <= action), RT_ERR_INPUT);

    switch(action)
    {
        case CLASSIFY_UNMATCH_DROP:
            raw_act = CF_US_PERMIT_DROP;
            break;
        case CLASSIFY_UNMATCH_PERMIT_WITHOUT_PON:
            raw_act = CF_US_PERMIT_NOPON;
            break;
        case CLASSIFY_UNMATCH_PERMIT:
            raw_act = CF_US_PERMIT_NORMAL;
            break;
        default:
            return RT_ERR_INPUT;
    }
    RT_ERR_CHK(apollomp_raw_cf_usPermit_set(raw_act), ret);

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_unmatchAction_set */


/* Function Name:
 *      dal_apollomp_classify_unmatchAction_get
 * Description:
 *      Get action to packets when no classfication configuration match
 * Input:
 *      None
 * Output:
 *      pAction - Action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Pointer pAction point to NULL.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no classfication configruation matches.
 */
int32
dal_apollomp_classify_unmatchAction_get(rtk_classify_unmatch_action_t *pAction)
{
    apollomp_raw_cf_usPermit_t raw_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    RT_ERR_CHK(apollomp_raw_cf_usPermit_get(&raw_act), ret);
    switch(raw_act)
    {
        case CF_US_PERMIT_NORMAL:
            *pAction = CLASSIFY_UNMATCH_PERMIT;
            break;
        case CF_US_PERMIT_NOPON:
            *pAction = CLASSIFY_UNMATCH_PERMIT_WITHOUT_PON;
            break;
        case CF_US_PERMIT_DROP:
            *pAction = CLASSIFY_UNMATCH_DROP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_unmatchAction_get */


/* Function Name:
 *      dal_apollomp_classify_portRange_set
 * Description:
 *      Set Port Range check
 * Input:
 *      pRangeEntry - L4 Port Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      UpperPort must be larger or equal than lowerPort.
 *      This function is not supported in Test chip.
 */
int32
dal_apollomp_classify_portRange_set(rtk_classify_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_CLASSIFY_L4PORT_RANGE_NUM(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    switch(pRangeEntry->type)
    {
        case CLASSIFY_PORTRANGE_SPORT:
            val = CF_PORT_RNG_SPORT;
        break;

        case CLASSIFY_PORTRANGE_DPORT:
            val = CF_PORT_RNG_DPORT;
        break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }

	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_L4PORT_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->upperPort;
	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_L4PORT_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_L4PORT_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->lowerPort;
	if((ret = reg_array_field_write(APOLLOMP_RNG_CHK_L4PORT_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_L4PORT_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_portRange_set */


/* Function Name:
 *      dal_apollomp_classify_portRange_get
 * Description:
 *      Set Port Range check
 * Input:
 *      None
 * Output:
 *      pRangeEntry - L4 Port Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      This function is not supported in Test chip.
 */
int32
dal_apollomp_classify_portRange_get(rtk_classify_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_CLASSIFY_L4PORT_RANGE_NUM(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_L4PORT_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    switch(val)
    {
        case CF_PORT_RNG_SPORT:
            pRangeEntry->type = CLASSIFY_PORTRANGE_SPORT;
        break;

        case CF_PORT_RNG_DPORT:
            pRangeEntry->type = CLASSIFY_PORTRANGE_DPORT;
        break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_L4PORT_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_L4PORT_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->upperPort = (ipaddr_t)val;

	if((ret = reg_array_field_read(APOLLOMP_RNG_CHK_L4PORT_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_L4PORT_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->lowerPort = (ipaddr_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_portRange_get */


/* Function Name:
 *      dal_apollomp_classify_ipRange_set
 * Description:
 *      Set IP Range check
 * Input:
 *      pRangeEntry - IP Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      UpperIp must be larger or equal than lowerIp.
 *      This function is not supported in Test chip.
 */
int32
dal_apollomp_classify_ipRange_set(rtk_classify_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_CLASSIFY_IP_RANGE_NUM(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

    switch(pRangeEntry->type)
    {
        case CLASSIFY_IPRANGE_IPV4_SIP:
            val = CF_IP_RNG_SIP;
        break;

        case CLASSIFY_IPRANGE_IPV4_DIP:
            val = CF_IP_RNG_DIP;
        break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }

	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_IP_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->upperIp;
	if ((ret = reg_array_field_write(APOLLOMP_RNG_CHK_IP_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_IP_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    val = (uint32)pRangeEntry->lowerIp;
	if((ret = reg_array_field_write(APOLLOMP_RNG_CHK_IP_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_IP_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_ipRange_set */


/* Function Name:
 *      dal_apollomp_classify_ipRange_get
 * Description:
 *      Set IP Range check
 * Input:
 *      None.
 * Output:
 *      pRangeEntry - IP Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      This function is not supported in Test chip.
 */
int32
dal_apollomp_classify_ipRange_get(rtk_classify_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;
    uint32  index;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRangeEntry->index >= HAL_CLASSIFY_IP_RANGE_NUM(), RT_ERR_OUT_OF_RANGE);

    index = (uint32)pRangeEntry->index;

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_IP_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_TYPEf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    switch(val)
    {
        case CF_IP_RNG_SIP:
            pRangeEntry->type = CLASSIFY_IPRANGE_IPV4_SIP;
        break;

        case CF_IP_RNG_DIP:
            pRangeEntry->type = CLASSIFY_IPRANGE_IPV4_DIP;
        break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }

	if ((ret = reg_array_field_read(APOLLOMP_RNG_CHK_IP_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_IP_UPPERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->upperIp = (ipaddr_t)val;

	if((ret = reg_array_field_read(APOLLOMP_RNG_CHK_IP_RNG_CFr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_IP_LOWERf, &val)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    pRangeEntry->lowerIp = (ipaddr_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_ipRange_get */

/* Function Name:
 *      dal_apollomp_classify_cf_sel_set
 * Description:
 *      Set CF port selection, only pon port and RGMII port can be set
 * Input:
 *      port    - port id, only pon port and RGMII port can be set.
 *      cfSel   - CF port selection.
 * Output:
 *      pRangeEntry - IP Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      Only accept pon port and RGMII port.
 *      This function is not supported in Test chip.
 */
int32
dal_apollomp_classify_cf_sel_set(rtk_port_t port, rtk_classify_cf_sel_t cfSel)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    switch(cfSel)
    {
        case CLASSIFY_CF_SEL_DISABLE:
            val = CF_DISABLE;
        break;
        case CLASSIFY_CF_SEL_ENABLE:
            val = CF_ENABLE;
        break;
        default:
            return RT_ERR_OUT_OF_RANGE;
        break;
    }

    switch(port)
    {
        case 4:
            ret = reg_field_write(APOLLOMP_CF_CFGr, APOLLOMP_CF_SEL_PON_ENf, &val);
        break;
        case 5:
            ret = reg_field_write(APOLLOMP_CF_CFGr, APOLLOMP_CF_SEL_RGMII_ENf, &val);
        break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }

	if(ret != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_cf_sel_set */

/* Function Name:
 *      dal_apollomp_classify_cf_sel_get
 * Description:
 *      Get CF port selection, only pon port and RGMII port can be get
 * Input:
 *      port    - port id, only pon port and RGMII port can be get.
 *      pCfSel  - pointer of CF port selection.
 * Output:
 *      pRangeEntry - IP Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      Only accept pon port and RGMII port.
 *      This function is not supported in Test chip.
 */
int32
dal_apollomp_classify_cf_sel_get(rtk_port_t port, rtk_classify_cf_sel_t *pCfSel)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCfSel), RT_ERR_NULL_POINTER);

    switch(port)
    {
        case 4:
            ret = reg_field_read(APOLLOMP_CF_CFGr, APOLLOMP_CF_SEL_PON_ENf, &val);
        break;
        case 5:
            ret = reg_field_read(APOLLOMP_CF_CFGr, APOLLOMP_CF_SEL_RGMII_ENf, &val);
        break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }

    *pCfSel = (rtk_classify_cf_sel_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_cf_sel_get */

/* Function Name:
 *      dal_apollomp_classify_cfPri2Dscp_set
 * Description:
 *      Set CF priority to DSCP value mapping
 * Input:
 *      pri    - priority value
 *      dscp   - DSCP value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      This function is not supported in Test chip.
 */
extern int32
dal_apollomp_classify_cfPri2Dscp_set(rtk_pri_t pri, rtk_dscp_t dscp)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pri), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp), RT_ERR_INPUT);

    if((ret = reg_array_field_write(APOLLOMP_RMK_DSCP_CF_PRI_CTRLr, REG_ARRAY_INDEX_NONE, pri, APOLLOMP_CFPRI_DSCPf, &dscp)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_classify_cfPri2Dscp_get
 * Description:
 *      Get CF priority to DSCP value mapping
 * Input:
 *      pri    - priority value
 * Output:
 *      pDscp  - pointer of DSCP value.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_NULL_POINTER    - Pointer pClassifyCfg point to NULL.
 * Note:
 *      This function is not supported in Test chip.
 */
extern int32
dal_apollomp_classify_cfPri2Dscp_get(rtk_pri_t pri, rtk_dscp_t *pDscp)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pri), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(APOLLOMP_RMK_DSCP_CF_PRI_CTRLr, REG_ARRAY_INDEX_NONE, pri, APOLLOMP_CFPRI_DSCPf, pDscp)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_apollomp_classify_permit_sel_set
 * Description:
 *      Set classification permit selection, from 0 to 511 or from 64 to 511
 * Input:
 *      permitSel   - CF permit selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *      Only accept from 0 to 511 or from 64 to 511
 */
int32
dal_apollomp_classify_permit_sel_set( rtk_classify_permit_sel_t permitSel)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    switch(permitSel)
    {
        case CLASSIFY_CF_SEL_FROM_0:
            val = CF_DISABLE;
        break;
        case CLASSIFY_CF_SEL_FROM_64:
            val = CF_ENABLE;
        break;
        default:
            return RT_ERR_OUT_OF_RANGE;
        break;
    }


     if((ret = reg_field_write(APOLLOMP_CF_CFGr, APOLLOMP_CF_PERMIT_SELf, &val)) !=RT_ERR_OK)
     {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
     }


    return RT_ERR_OK;
} /* end of dal_apollomp_classify_cf_sel_set */


/* Function Name:
 *      dal_apollomp_classify_permit_sel_get
 * Description:
 *      Set classification permit selection, from 0 to 511 or from 64 to 511
 * Input:
 *      permitSel   -point of CF permit selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *      Only accept from 0 to 511 or from 64 to 511
 */
int32
dal_apollomp_classify_permit_sel_get( rtk_classify_permit_sel_t *permitSel)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);
    RT_PARAM_CHK((NULL == permitSel), RT_ERR_NULL_POINTER);

   if((ret = reg_field_read(APOLLOMP_CF_CFGr, APOLLOMP_CF_PERMIT_SELf, &val)) !=RT_ERR_OK)
     {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
     }

    switch(val)
    {
        case CF_DISABLE :
            *permitSel = CLASSIFY_CF_SEL_FROM_0;
        break;
        case CF_ENABLE :
            *permitSel = CLASSIFY_CF_SEL_FROM_64;
        break;
        default:
            return RT_ERR_OUT_OF_RANGE;
        break;
    }
    return RT_ERR_OK;
} /* end of dal_apollomp_classify_cf_sel_set */


/* Function Name:
 *      dal_apollomp_classify_us1pRemarkPrior_set
 * Description:
 *      Set classification U/S 1p remark is prior than ACL U/S 1p remarking
 * Input:
 *      prior   - CF US 1p remarking is prior than ACL or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
int32
dal_apollomp_classify_us1pRemarkPrior_set( rtk_classify_us_1premark_prior_t prior)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    switch(prior)
    {
        case CLASSIFY_US_1PREMARK_PRIOR_ACL:
            val = 0;
        break;
        case CLASSIFY_US_1PREMARK_PRIOR_CF:
            val = 1;
        break;
        default:
            return RT_ERR_OUT_OF_RANGE;
        break;
    }

    if((ret = reg_field_write(APOLLOMP_CF_US_1P_REMARK_ENr, APOLLOMP_CF_US_1P_REMARK_ENf, &val)) !=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_classify_us1pRemarkPrior_set */


/* Function Name:
 *      dal_apollomp_classify_us1pRemarkPrior_get
 * Description:
 *      Get classification U/S 1p remark is prior than ACL U/S 1p remarking or not
 * Input:
 *      prior   - CF US 1p remarking is prior than ACL or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
int32
dal_apollomp_classify_us1pRemarkPrior_get( rtk_classify_us_1premark_prior_t *pPrior)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);
    RT_PARAM_CHK((NULL == pPrior), RT_ERR_NULL_POINTER);

   if((ret = reg_field_read(APOLLOMP_CF_US_1P_REMARK_ENr, APOLLOMP_CF_US_1P_REMARK_ENf, &val)) !=RT_ERR_OK)
     {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
     }

    switch(val)
    {
        case 1 :
            *pPrior = CLASSIFY_US_1PREMARK_PRIOR_ACL;
        break;
        case 0 :
            *pPrior = CLASSIFY_US_1PREMARK_PRIOR_CF;
        break;
        default:
            return RT_ERR_OUT_OF_RANGE;
        break;
    }
    return RT_ERR_OK;
} /* end of dal_apollomp_classify_us1pRemarkPrior_get */

