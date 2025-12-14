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

#ifndef __DAL_RTL9601B_CLASSIFY_H__
#define __DAL_RTL9601B_CLASSIFY_H__

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/rtl9601b/dal_rtl9601b.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define RTL9601B_RAW_CF_ETHERTYPE_NONE			0XFFFF
#define RTL9601B_RAW_CF_ENTRYLEN                    5
#define RTL9601B_RAW_CF_UPACTTBLEN                  2
#define RTL9601B_RAW_CF_DSACTTBLEN                  2



/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
typedef enum rtl9601b_raw_cf_patternNum_e{
    RTL9601B_RAW_CF_PATTERN_0 = 0,
    RTL9601B_RAW_CF_PATTERN_1,
    RTL9601B_RAW_CF_PATTERN_2,
    RTL9601B_RAW_CF_PATTERN_3,
    RTL9601B_RAW_CF_PATTERN_END,
}rtl9601b_raw_cf_patternNum_t;

typedef enum rtl9601b_raw_cf_usPermit_e{
    CF_US_PERMIT_DROP = 0,
    CF_US_PERMIT_NORMAL,
    CF_US_PERMIT_NOPON,
    CF_US_PERMIT_END,
}rtl9601b_raw_cf_usPermit_t;

typedef enum rtl9601b_raw_cf_valid_e{
    CF_ENTRY_INVALID = 0,
    CF_ENTRY_VALID,
    CF_ENTRY_VALID_END,
}rtl9601b_raw_cf_valid_t;

typedef enum rtl9601b_raw_cf_enable_t
{
    CF_DISABLE = 0,
    CF_ENABLE,
    CF_ENABLE_END,
} rtl9601b_raw_cf_enable_t;

typedef enum rtl9601b_raw_cf_unmatch_action_e
{
    CF_UNMATCH_DROP = 0,
    CF_UNMATCH_PERMIT_WITHOUT_PON,
    CF_UNMATCH_PERMIT,
    CF_UNMATCH_END,
} rtl9601b_raw_cf_unmatch_action_t;

typedef enum rtl9601b_raw_cf_dirct_e
{
    CF_DIRECTION_US = 0,
    CF_DIRECTION_DS,
    CF_DIRECTION_END
}rtl9601b_raw_cf_dirct_t;

/*down stream action*/
typedef enum rtl9601b_raw_cf_ds_csact_e
{
    CF_DS_CSACT_NOP = 0,
    CF_DS_CSACT_ADD_TAG_VS_TPID,
    CF_DS_CSACT_ADD_TAG_8100,
    CF_DS_CSACT_DEL_STAG,
    CF_DS_CSACT_TRANSPARENT, /* not support in test chip */
    CF_DS_CSACT_SP2C, /* not support in test chip */
    CF_DS_CSACT_END,
} rtl9601b_raw_cf_ds_csact_t;

typedef enum rtl9601b_raw_cf_ds_cact_e
{
    CF_DS_CACT_NOP = 0,
    CF_DS_CACT_ADD_CTAG_8100,
    CF_DS_CACT_TRANSLATION_SP2C,
    CF_DS_CACT_DEL_CTAG, /* not support in test chip */
    CF_DS_CACT_TRANSPARENT,
    CF_DS_CACT_END,
} rtl9601b_raw_cf_ds_cact_t;

typedef enum rtl9601b_raw_cf_ds_vid_act_e
{
    CF_DS_VID_ACT_ASSIGN = 0,
    CF_DS_VID_ACT_FROM_1ST_TAG,
    CF_DS_VID_ACT_FROM_2ND_TAG, /* not support in test chip */
    CF_DS_VID_ACT_FROM_LUT,
    CF_DS_VID_ACT_END,
} rtl9601b_raw_cf_ds_vid_act_t;

typedef enum rtl9601b_raw_cf_ds_pri_act_e
{
    CF_DS_PRI_ACT_ASSIGN = 0,
    CF_DS_PRI_ACT_FROM_1ST_TAG,
    CF_DS_PRI_ACT_FROM_2ND_TAG, /* not support in test chip */
    CF_DS_PRI_ACT_FROM_INTERNAL,
    CF_DS_PRI_ACT_END,
} rtl9601b_raw_cf_ds_pri_act_t;

typedef enum rtl9601b_raw_cf_cf_pri_act_e
{
    CF_CF_PRI_ACT_NOP = 0,
    CF_CF_PRI_ACT_ASSIGN,
    CF_CF_PRI_ACT_END,
} rtl9601b_raw_cf_cf_pri_act_t;

typedef enum rtl9601b_raw_cf_ds_uni_act_e
{
    CF_DS_UNI_ACT_NOP = 0,
    CF_DS_UNI_ACT_MASK_BY_UNIMASK,
    CF_DS_UNI_ACT_FORCE_FORWARD,
    CF_DS_UNI_ACT_END,
} rtl9601b_raw_cf_ds_uni_act_t;

/*up stream action*/
typedef enum rtl9601b_raw_cf_us_csact_e
{
    CF_US_CSACT_NOP = 0,
    CF_US_CSACT_ADD_TAG_VS_TPID,
    CF_US_CSACT_ADD_TAG_8100,
    CF_US_CSACT_DEL_STAG, /* not support in test chip */
    CF_US_CSACT_TRANSPARENT, /* not support in test chip */
    CF_US_CSACT_END,
} rtl9601b_raw_cf_us_csact_t;

typedef enum rtl9601b_raw_cf_us_cact_e
{
    CF_US_CACT_NOP = 0,
    CF_US_CACT_ADD_CTAG_8100, /* not support in test chip */
    CF_US_CACT_TRANSLATION_C2S, /*original is C2S, not support in rtl9601b*/
    CF_US_CACT_DEL_CTAG,
    CF_US_CACT_TRANSPARENT,
    CF_US_CACT_END,
} rtl9601b_raw_cf_us_cact_t;

typedef enum rtl9601b_raw_cf_us_vid_act_e
{
    CF_US_VID_ACT_ASSIGN = 0,
    CF_US_VID_ACT_FROM_1ST_TAG,
    CF_US_VID_ACT_FROM_2ND_TAG, /* not support in test chip */
    CF_US_VID_ACT_FROM_INTERNAL, /* not support in test chip */
    CF_US_VID_ACT_END,
} rtl9601b_raw_cf_us_vid_act_t;

typedef enum rtl9601b_raw_cf_us_pri_act_e
{
    CF_US_PRI_ACT_ASSIGN = 0,
    CF_US_PRI_ACT_FROM_1ST_TAG,
    CF_US_PRI_ACT_FROM_2ND_TAG, /* not support in test chip */
    CF_US_PRI_ACT_FROM_INTERNAL,
    CF_US_PRI_ACT_END,
} rtl9601b_raw_cf_us_pri_act_t;

typedef enum rtl9601b_raw_cf_us_sqid_act_e
{
    CF_US_SQID_ACT_ASSIGN_NOP =0,
    CF_US_SQID_ACT_ASSIGN_SID,
    CF_US_SQID_ACT_END,
} rtl9601b_raw_cf_us_sqid_act_t;





typedef struct rtl9601b_raw_cf_pattern_s{
    uint32 etherType;
    uint32 direction;     /* upstraeam or downstream configuration*/
    uint32 tos_gemidx;    /* TOS/TC for upstream,GEMIDX/LLIDX for downstream*/
    uint32 outerVid;
    uint32 outerPri;
    uint32 interPri;	  /*internal priority*/
    uint32 ifStag;
    uint32 ifCtag;
    uint32 uni;
    uint32 innerVlan;
    uint32 aclHit;
    uint32 outerDei;

}rtl9601b_raw_cf_pattern_t;

typedef struct rtl9601b_raw_cf_RuleEntry_s{
    uint32 idx;
    uint32 valid;
    rtl9601b_raw_cf_pattern_t data_bits;
    rtl9601b_raw_cf_pattern_t care_bits;
}rtl9601b_raw_cf_RuleEntry_t;


typedef struct rtl9601b_raw_cf_dsAct_s{
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
    uint32 uni_act;
    uint32 uni_mask;
    uint32 dscp_remark;

}rtl9601b_raw_cf_dsAct_t;

typedef struct rtl9601b_raw_cf_usAct_s{
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
    uint32 drop_act;

}rtl9601b_raw_cf_usAct_t;

int32 rtl9601b_raw_cf_valid_set(uint32 index, rtk_enable_t enabled);
int32 rtl9601b_raw_cf_valid_get(uint32 index, rtk_enable_t *pEnabled);
int32 rtl9601b_raw_cf_rule_set(rtl9601b_raw_cf_RuleEntry_t *pattern);
int32 rtl9601b_raw_cf_rule_get(rtl9601b_raw_cf_RuleEntry_t *pattern);
int32 rtl9601b_raw_cf_dsAct_set(rtl9601b_raw_cf_dsAct_t *pCfDsAct);
int32 rtl9601b_raw_cf_dsAct_get(rtl9601b_raw_cf_dsAct_t *pCfDsAct);
int32 rtl9601b_raw_cf_usAct_set(rtl9601b_raw_cf_usAct_t *pCfUsAct);
int32 rtl9601b_raw_cf_usAct_get(rtl9601b_raw_cf_usAct_t *pCfUsAct);
int32 rtl9601b_raw_cf_dsOp_set(uint32 index, rtk_enable_t notDs);
int32 rtl9601b_raw_cf_dsOp_get(uint32 index, rtk_enable_t *notDs);
int32 rtl9601b_raw_cf_usOp_set(uint32 index, rtk_enable_t notUs);
int32 rtl9601b_raw_cf_usOp_get(uint32 index, rtk_enable_t *notUs);
int32 rtl9601b_raw_cf_usPermit_set(rtl9601b_raw_cf_usPermit_t rule);
int32 rtl9601b_raw_cf_usPermit_get(rtl9601b_raw_cf_usPermit_t *pRule);



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/classify.h>

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
 *      dal_rtl9601b_classify_init
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
dal_rtl9601b_classify_init(void);

/* Function Name:
 *      dal_rtl9601b_classify_cfgEntry_add
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
extern int32
dal_rtl9601b_classify_cfgEntry_add(rtk_classify_cfg_t *pClassifyCfg);


/* Function Name:
 *      dal_rtl9601b_classify_cfgEntry_get
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
extern int32
dal_rtl9601b_classify_cfgEntry_get(rtk_classify_cfg_t *pClassifyCfg);


/* Function Name:
 *      dal_rtl9601b_classify_cfgEntry_del
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
extern int32
dal_rtl9601b_classify_cfgEntry_del(uint32 entryIdx);


/* Function Name:
 *      dal_rtl9601b_classify_field_add
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
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL
 *      comparison rules by means of linked list. Pointer pAclField will be added to linked
 *      list keeped by structure that pAclEntry points to.
 *      - caller should not free (*pClassifyField) before dal_rtl9601b_classify_cfgEntry_add is called
 */
extern int32
dal_rtl9601b_classify_field_add(rtk_classify_cfg_t *pClassifyEntry, rtk_classify_field_t *pClassifyField);


/* Function Name:
 *      dal_rtl9601b_classify_unmatchAction_set
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
extern int32
dal_rtl9601b_classify_unmatchAction_set(rtk_classify_unmatch_action_t action);


/* Function Name:
 *      dal_rtl9601b_classify_unmatchAction_get
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
extern int32
dal_rtl9601b_classify_unmatchAction_get(rtk_classify_unmatch_action_t *pAction);


/* Function Name:
 *      dal_rtl9601b_classify_portRange_set
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
 *      upperPort must be larger or equal than lowerPort.
 */
extern int32
dal_rtl9601b_classify_portRange_set(rtk_classify_rangeCheck_l4Port_t *pRangeEntry);


/* Function Name:
 *      dal_rtl9601b_classify_portRange_get
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
 *      None.
 */
extern int32
dal_rtl9601b_classify_portRange_get(rtk_classify_rangeCheck_l4Port_t *pRangeEntry);


/* Function Name:
 *      dal_rtl9601b_classify_ipRange_set
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
 *      upperIp must be larger or equal than lowerIp.
 */
extern int32
dal_rtl9601b_classify_ipRange_set(rtk_classify_rangeCheck_ip_t *pRangeEntry);


/* Function Name:
 *      dal_rtl9601b_classify_ipRange_get
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
 *      None.
 */
extern int32
dal_rtl9601b_classify_ipRange_get(rtk_classify_rangeCheck_ip_t *pRangeEntry);

/* Function Name:
 *      dal_rtl9601b_classify_cf_sel_set
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
dal_rtl9601b_classify_cf_sel_set(rtk_port_t port, rtk_classify_cf_sel_t cfSel);

/* Function Name:
 *      dal_rtl9601b_classify_cf_sel_get
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
dal_rtl9601b_classify_cf_sel_get(rtk_port_t port, rtk_classify_cf_sel_t *pCfSel);

/* Function Name:
 *      dal_rtl9601b_classify_cfPri2Dscp_set
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
dal_rtl9601b_classify_cfPri2Dscp_set(rtk_pri_t pri, rtk_dscp_t dscp);

/* Function Name:
 *      dal_rtl9601b_classify_cfPri2Dscp_get
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
dal_rtl9601b_classify_cfPri2Dscp_get(rtk_pri_t pri, rtk_dscp_t *pDscp);

/* Function Name:
 *      dal_rtl9601b_classify_permit_sel_get
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
extern int32
dal_rtl9601b_classify_permit_sel_get( rtk_classify_permit_sel_t *permitSel);

/* Function Name:
 *      dal_rtl9601b_classify_permit_sel_set
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
extern int32
dal_rtl9601b_classify_permit_sel_set( rtk_classify_permit_sel_t permitSel);

#endif /* __DAL_RTL9601B_CLASSIFY_H__ */

