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
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of Classifyication API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) classfication rule add/delete/get
 */

#ifndef __DAL_RTL9603CVD_CLASSIFY_H__
#define __DAL_RTL9603CVD_CLASSIFY_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/classify.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define RTL9603CVD_RAW_CF_ETHERTYPE_NONE          0XFFFF
#define RTL9603CVD_RAW_CF_ENTRYLEN                    2
#define RTL9603CVD_RAW_CF_UPACTTBLEN                  3
#define RTL9603CVD_RAW_CF_DSACTTBLEN                  3
#define RTL9603CVD_RAW_CF_TEMPLATE_MAX                    2
/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

typedef enum rtl9603cvd_raw_cf_usPermit_e{
    RTL9603CVD_RAW_CF_US_PERMIT_NORMAL = 0,
    RTL9603CVD_RAW_CF_US_PERMIT_NOPON,
    RTL9603CVD_RAW_CF_US_PERMIT_END,
}rtl9603cvd_raw_cf_usPermit_t;

typedef enum rtl9603cvd_raw_cf_valid_e{
    RTL9603CVD_RAW_CF_ENTRY_INVALID = 0,
    RTL9603CVD_RAW_CF_ENTRY_VALID,
    RTL9603CVD_RAW_CF_ENTRY_VALID_END,
}rtl9603cvd_raw_cf_valid_t;

typedef enum rtl9603cvd_raw_cf_enable_t
{
    RTL9603CVD_RAW_CF_DISABLE = 0,
    RTL9603CVD_RAW_CF_ENABLE,
    RTL9603CVD_RAW_CF_ENABLE_END,
} rtl9603cvd_raw_cf_enable_t;

typedef enum rtl9603cvd_raw_cf_dirct_e
{
    RTL9603CVD_RAW_CF_DIRECTION_US = 0,
    RTL9603CVD_RAW_CF_DIRECTION_DS,
    RTL9603CVD_RAW_CF_DIRECTION_END
}rtl9603cvd_raw_cf_dirct_t;

/*down stream action*/
typedef enum rtl9603cvd_raw_cf_ds_csact_e
{
    RTL9603CVD_RAW_CF_DS_CSACT_NOP = 0,
    RTL9603CVD_RAW_CF_DS_CSACT_ADD_TAG_VS_TPID,
    RTL9603CVD_RAW_CF_DS_CSACT_ADD_TAG_VS_TPID2,
    RTL9603CVD_RAW_CF_DS_CSACT_DEL_STAG,
    RTL9603CVD_RAW_CF_DS_CSACT_TRANSPARENT,
    RTL9603CVD_RAW_CF_DS_CSACT_ADD_TAG_STAG_TPID,
    RTL9603CVD_RAW_CF_DS_CSACT_END,
} rtl9603cvd_raw_cf_ds_csact_t;

typedef enum rtl9603cvd_raw_cf_ds_cact_e
{
    RTL9603CVD_RAW_CF_DS_CACT_NOP = 0,
    RTL9603CVD_RAW_CF_DS_CACT_ADD_CTAG_8100,
    RTL9603CVD_RAW_CF_DS_CACT_DEL_CTAG,
    RTL9603CVD_RAW_CF_DS_CACT_TRANSPARENT,
    RTL9603CVD_RAW_CF_DS_CACT_END,
} rtl9603cvd_raw_cf_ds_cact_t;

typedef enum rtl9603cvd_raw_cf_ds_vid_act_e
{
    RTL9603CVD_RAW_CF_DS_VID_ACT_NOP =0,
    RTL9603CVD_RAW_CF_DS_VID_ACT_ASSIGN,
    RTL9603CVD_RAW_CF_DS_VID_ACT_FROM_1ST_TAG,
    RTL9603CVD_RAW_CF_DS_VID_ACT_FROM_2ND_TAG,
    RTL9603CVD_RAW_CF_DS_VID_ACT_TRANSLATION_SP2C,
    RTL9603CVD_RAW_CF_DS_VID_ACT_FROM_LUT,
    RTL9603CVD_RAW_CF_DS_VID_ACT_END,
} rtl9603cvd_raw_cf_ds_vid_act_t;

typedef enum rtl9603cvd_raw_cf_ds_pri_act_e
{
    RTL9603CVD_RAW_CF_DS_PRI_ACT_NOP = 0,
    RTL9603CVD_RAW_CF_DS_PRI_ACT_ASSIGN,
    RTL9603CVD_RAW_CF_DS_PRI_ACT_FROM_1ST_TAG,
    RTL9603CVD_RAW_CF_DS_PRI_ACT_FROM_2ND_TAG,
    RTL9603CVD_RAW_CF_DS_PRI_ACT_FROM_INTERNAL,
    RTL9603CVD_RAW_CF_DS_PRI_ACT_FROM_DSCP,
    RTL9603CVD_RAW_CF_DS_PRI_ACT_TRANSLATION_SP2C,
    RTL9603CVD_RAW_CF_DS_PRI_ACT_END,
} rtl9603cvd_raw_cf_ds_pri_act_t;

typedef enum rtl9603cvd_raw_cf_cf_pri_act_e
{
    RTL9603CVD_RAW_CF_CF_PRI_ACT_NOP = 0,
    RTL9603CVD_RAW_CF_CF_PRI_ACT_ASSIGN,
    RTL9603CVD_RAW_CF_CF_PRI_ACT_END,
} rtl9603cvd_raw_cf_cf_pri_act_t;

typedef enum rtl9603cvd_raw_cf_ds_uni_act_e
{
    RTL9603CVD_RAW_CF_DS_UNI_ACT_NOP = 0,
    RTL9603CVD_RAW_CF_DS_UNI_ACT_FORCE_FORWARD,
    RTL9603CVD_RAW_CF_DS_UNI_ACT_TRAP,
    RTL9603CVD_RAW_CF_DS_UNI_ACT_MASK_BY_UNIMASK,
    RTL9603CVD_RAW_CF_DS_UNI_ACT_END,
} rtl9603cvd_raw_cf_ds_uni_act_t;

/*up stream action*/
typedef enum rtl9603cvd_raw_cf_us_csact_e
{
    RTL9603CVD_RAW_CF_US_CSACT_NOP = 0,
    RTL9603CVD_RAW_CF_US_CSACT_ADD_TAG_VS_TPID,
    RTL9603CVD_RAW_CF_US_CSACT_ADD_TAG_VS_TPID2,
    RTL9603CVD_RAW_CF_US_CSACT_DEL_STAG,
    RTL9603CVD_RAW_CF_US_CSACT_TRANSPARENT,
    RTL9603CVD_RAW_CF_US_CSACT_ADD_TAG_STAG_TPID,
    RTL9603CVD_RAW_CF_US_CSACT_END,
} rtl9603cvd_raw_cf_us_csact_t;

typedef enum rtl9603cvd_raw_cf_us_cact_e
{
    RTL9603CVD_RAW_CF_US_CACT_NOP = 0,
    RTL9603CVD_RAW_CF_US_CACT_ADD_CTAG_8100,
    RTL9603CVD_RAW_CF_US_CACT_DEL_CTAG,
    RTL9603CVD_RAW_CF_US_CACT_TRANSPARENT,
    RTL9603CVD_RAW_CF_US_CACT_END,
} rtl9603cvd_raw_cf_us_cact_t;

typedef enum rtl9603cvd_raw_cf_us_vid_act_e
{
    RTL9603CVD_RAW_CF_US_VID_ACT_NOP =0,
    RTL9603CVD_RAW_CF_US_VID_ACT_ASSIGN,
    RTL9603CVD_RAW_CF_US_VID_ACT_FROM_1ST_TAG,
    RTL9603CVD_RAW_CF_US_VID_ACT_FROM_2ND_TAG,
    RTL9603CVD_RAW_CF_US_VID_ACT_END,
} rtl9603cvd_raw_cf_us_vid_act_t;

typedef enum rtl9603cvd_raw_cf_us_pri_act_e
{
    RTL9603CVD_RAW_CF_US_PRI_ACT_NOP = 0,
    RTL9603CVD_RAW_CF_US_PRI_ACT_ASSIGN,
    RTL9603CVD_RAW_CF_US_PRI_ACT_FROM_1ST_TAG,
    RTL9603CVD_RAW_CF_US_PRI_ACT_FROM_2ND_TAG,
    RTL9603CVD_RAW_CF_US_PRI_ACT_FROM_INTERNAL,
    RTL9603CVD_RAW_CF_US_PRI_ACT_FROM_DSCP,
    RTL9603CVD_RAW_CF_US_PRI_ACT_END,
} rtl9603cvd_raw_cf_us_pri_act_t;

typedef enum rtl9603cvd_raw_cf_us_sqid_act_e
{
    RTL9603CVD_RAW_CF_US_SQID_ACT_ASSIGN_NOP =0,
    RTL9603CVD_RAW_CF_US_SQID_ACT_ASSIGN_SID,
    RTL9603CVD_RAW_CF_US_SQID_ACT_END,
} rtl9603cvd_raw_cf_us_sqid_act_t;

typedef enum rtl9603cvd_raw_cf_us_fwd_act_e
{
    RTL9603CVD_RAW_CF_US_FWD_ACT_NOP =0,
    RTL9603CVD_RAW_CF_US_FWD_ACT_DROP,
    RTL9603CVD_RAW_CF_US_FWD_ACT_TRAP,
    RTL9603CVD_RAW_CF_US_FWD_ACT_DROP_PON,
    RTL9603CVD_RAW_CF_US_FWD_ACT_END,
} rtl9603cvd_raw_cf_us_fwd_act_t;


typedef struct rtl9603cvd_raw_cf_pattern_s{
    uint32 etherType;
    uint32 direction;     /* upstraeam or downstream configuration*/
    uint32 tos_gemidx;    /* TOS/TC for upstream,GEMIDX/LLIDX for downstream*/
    uint32 tagVid;
    uint32 tagPri;
    uint32 innerVlan;          /*Include VID+CFI+PRI*/
    uint32 interPri;
    uint32 ifStag;
    uint32 ifCtag;
    uint32 uni;

    uint32 aclHit;
    uint32 wanIf;
    uint32 pppoe;
    uint32 ipv4;
    uint32 ipv6;
    uint32 ipmc;
    uint32 igmp_mld;
    uint32 intVidTagIf;
    uint32 cfAclHit;
    uint32 dei;
    uint32 stpidSel;

}rtl9603cvd_raw_cf_pattern_t;

typedef struct rtl9603cvd_raw_cf_RuleEntry_s{
    uint32 idx;
    uint32 valid;
    rtl9603cvd_raw_cf_pattern_t data_bits;
    rtl9603cvd_raw_cf_pattern_t care_bits;
}rtl9603cvd_raw_cf_RuleEntry_t;


typedef struct rtl9603cvd_raw_cf_dsAct_s{
    uint32 idx;
    uint32 csact;
    uint32 s_vid;
    uint32 s_pri;
    uint32 svid_act;
    uint32 spri_act;
    uint32 cact;
    uint32 c_vid;
    uint32 c_pri;
    uint32 cvid_act;
    uint32 cpri_act;
    uint32 cfpri_act;
    uint32 cfpri;
    uint32 dscp_remark;
    uint32 dscp;
    uint32 uni_act;
    uint32 uni_mask;

}rtl9603cvd_raw_cf_dsAct_t;

typedef struct rtl9603cvd_raw_cf_usAct_s{
    uint32 idx;
    uint32 csact;
    uint32 svid_act;
    uint32 spri_act;
    uint32 s_vid;
    uint32 s_pri;
    uint32 cact;
    uint32 cvid_act;
    uint32 cpri_act;
    uint32 c_vid;
    uint32 c_pri;
    uint32 sid_act;
    uint32 assign_idx;
    uint32 cfpri_act;
    uint32 cfpri;
    uint32 dscp_remark;
    uint32 dscp;
    uint32 forward_act;

}rtl9603cvd_raw_cf_usAct_t;


/*
 * Function Declaration
 */

extern int32 rtl9603cvd_raw_cf_rule_set(rtl9603cvd_raw_cf_RuleEntry_t *pattern);
extern int32 rtl9603cvd_raw_cf_rule_get(rtl9603cvd_raw_cf_RuleEntry_t *pattern);
extern int32 rtl9603cvd_raw_cf_dsAct_set(rtl9603cvd_raw_cf_dsAct_t *pCfDsAct);
extern int32 rtl9603cvd_raw_cf_dsAct_get(rtl9603cvd_raw_cf_dsAct_t *pCfDsAct);
extern int32 rtl9603cvd_raw_cf_usAct_set(rtl9603cvd_raw_cf_usAct_t *pCfUsAct);
extern int32 rtl9603cvd_raw_cf_usAct_get(rtl9603cvd_raw_cf_usAct_t *pCfUsAct);
extern int32 rtl9603cvd_raw_cf_usPermit_set(rtl9603cvd_raw_cf_usPermit_t rule);
extern int32 rtl9603cvd_raw_cf_usPermit_get(rtl9603cvd_raw_cf_usPermit_t *pRule);

/* Function Name:
 *      dal_rtl9603cvd_classify_init
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
extern int32
dal_rtl9603cvd_classify_init(void);

/* Function Name:
 *      dal_rtl9603cvd_classify_cfgEntry_add
 * Description:
 *      Add an classification entry to ASIC
 * Input:
 *      entryIdx       - index of classification entry.
 *      pClassifyCfg     - The classification configuration that this function will add comparison rule
 *      pClassifyAct     - Action(s) of classification configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None.
 */
extern int32
dal_rtl9603cvd_classify_cfgEntry_add(rtk_classify_cfg_t *pClassifyCfg);


/* Function Name:
 *      dal_rtl9603cvd_classify_cfgEntry_get
 * Description:
 *      Gdd an classification entry from ASIC
 * Input:
 *      None.
 * Output:
 *      pClassifyCfg     - The classification configuration that this function will add comparison rule
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None.
 */
extern int32
dal_rtl9603cvd_classify_cfgEntry_get(rtk_classify_cfg_t *pClassifyCfg);


/* Function Name:
 *      dal_rtl9603cvd_classify_cfgEntry_del
 * Description:
 *      Delete an classification configuration from ASIC
 * Input:
 *      entryIdx    - index of classification entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_ENTRY_INDEX      - Invalid classification index .
 * Note:
 *      None.
 */
extern int32
dal_rtl9603cvd_classify_cfgEntry_del(uint32 entryIdx);


/* Function Name:
 *      dal_rtl9603cvd_classify_field_add
 * Description:
 *      Add comparison field to an classfication configuration
 * Input:
 *      pClassifyEntry     - The classfication configuration that this function will add comparison rule
 *      pClassifyField     - The comparison rule that will be added.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_NULL_POINTER     - Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This function add a comparison rule (*pClassifyField) to an ACL configuration (*pClassifyEntry).
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL
 *      comparison rules by means of linked list. Pointer pAclField will be added to linked
 *      list keeped by structure that pAclEntry points to.
 *      - caller should not free (*pClassifyField) before dal_rtl9603cvd_classify_cfgEntry_add is called
 */
extern int32
dal_rtl9603cvd_classify_field_add(rtk_classify_cfg_t *pClassifyEntry, rtk_classify_field_t *pClassifyField);


/* Function Name:
 *      dal_rtl9603cvd_classify_unmatchAction_set
 * Description:
 *      Apply action to upstream packets when no classfication configuration match
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
extern int32
dal_rtl9603cvd_classify_unmatchAction_set(rtk_classify_unmatch_action_t action);


/* Function Name:
 *      dal_rtl9603cvd_classify_unmatchAction_get
 * Description:
 *      Get action to upstream packets when no classfication configuration match
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
extern int32
dal_rtl9603cvd_classify_unmatchAction_get(rtk_classify_unmatch_action_t *pAction);

/* Function Name:
 *      dal_rtl9603cvd_classify_cf_sel_set
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
 */
extern int32
dal_rtl9603cvd_classify_cf_sel_set(rtk_port_t port, rtk_classify_cf_sel_t cfSel);

/* Function Name:
 *      dal_rtl9603cvd_classify_cf_sel_get
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
 */
extern int32
dal_rtl9603cvd_classify_cf_sel_get(rtk_port_t port, rtk_classify_cf_sel_t *pCfSel);

/* Function Name:
 *      dal_rtl9603cvd_classify_trap_pri_set
 * Description:
 *      Set CF trap priority
 * Input:
 *      trap_pri    - trap priority
 * Output:
 *      N/A
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *
 */
extern int32
dal_rtl9603cvd_classify_trap_pri_set(rtk_pri_t trap_pri);

/* Function Name:
 *      dal_rtl9603cvd_classify_trap_pri_get
 * Description:
 *      Get CF trap priority
 * Input:
 *      N/A
 * Output:
 *      pTrap_pri    - trap priority
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *
 */
extern int32
dal_rtl9603cvd_classify_trap_pri_get(rtk_pri_t *pTrap_pri);
#endif /* __DAL_RTL9603CVD_CLASSIFY_H__ */

