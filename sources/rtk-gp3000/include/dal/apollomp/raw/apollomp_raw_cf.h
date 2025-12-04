#ifndef _APOLLOMP_RAW_CLASSIFICATION_H_
#define _APOLLOMP_RAW_CLASSIFICATION_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/apollomp/raw/apollomp_raw.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define APOLLOMP_RAW_CF_ETHERTYPE_NONE			0XFFFF
#define APOLLOMP_RAW_CF_ENTRYLEN                    5
#define APOLLOMP_RAW_CF_UPACTTBLEN                  2
#define APOLLOMP_RAW_CF_DSACTTBLEN                  2



/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
typedef enum apollomp_raw_cf_patternNum_e{
    APOLLOMP_RAW_CF_PATTERN_0 = 0,
    APOLLOMP_RAW_CF_PATTERN_1,
    APOLLOMP_RAW_CF_PATTERN_2,
    APOLLOMP_RAW_CF_PATTERN_3,
    APOLLOMP_RAW_CF_PATTERN_END,
}apollomp_raw_cf_patternNum_t;

typedef enum apollomp_raw_cf_usPermit_e{
    CF_US_PERMIT_NORMAL = 0,
    CF_US_PERMIT_NOPON,
    CF_US_PERMIT_DROP,
    CF_US_PERMIT_END,
}apollomp_raw_cf_usPermit_t;

typedef enum apollomp_raw_cf_valid_e{
    CF_ENTRY_INVALID = 0,
    CF_ENTRY_VALID,
    CF_ENTRY_VALID_END,
}apollomp_raw_cf_valid_t;

typedef enum apollomp_raw_cf_enable_t
{
    CF_DISABLE = 0,
    CF_ENABLE,
    CF_ENABLE_END,
} apollomp_raw_cf_enable_t;

typedef enum apollomp_raw_cf_unmatch_action_e
{
    CF_UNMATCH_DROP = 0,
    CF_UNMATCH_PERMIT_WITHOUT_PON,
    CF_UNMATCH_PERMIT,
    CF_UNMATCH_END,
} apollomp_raw_cf_unmatch_action_t;

typedef enum apollomp_raw_cf_dirct_e
{
    CF_DIRECTION_US = 0,
    CF_DIRECTION_DS,
    CF_DIRECTION_END
}apollomp_raw_cf_dirct_t;

/*down stream action*/
typedef enum apollomp_raw_cf_ds_csact_e
{
    CF_DS_CSACT_NOP = 0,
    CF_DS_CSACT_ADD_TAG_VS_TPID,
    CF_DS_CSACT_ADD_TAG_8100,
    CF_DS_CSACT_DEL_STAG,
    CF_DS_CSACT_TRANSPARENT, /* not support in test chip */
    CF_DS_CSACT_SP2C, /* not support in test chip */
    CF_DS_CSACT_END,
} apollomp_raw_cf_ds_csact_t;

typedef enum apollomp_raw_cf_ds_cact_e
{
    CF_DS_CACT_NOP = 0,
    CF_DS_CACT_ADD_CTAG_8100,
    CF_DS_CACT_TRANSLATION_SP2C,
    CF_DS_CACT_DEL_CTAG, /* not support in test chip */
    CF_DS_CACT_TRANSPARENT,
    CF_DS_CACT_END,
} apollomp_raw_cf_ds_cact_t;

typedef enum apollomp_raw_cf_ds_vid_act_e
{
    CF_DS_VID_ACT_ASSIGN = 0,
    CF_DS_VID_ACT_FROM_1ST_TAG,
    CF_DS_VID_ACT_FROM_2ND_TAG, /* not support in test chip */
    CF_DS_VID_ACT_FROM_LUT,
    CF_DS_VID_ACT_END,
} apollomp_raw_cf_ds_vid_act_t;

typedef enum apollomp_raw_cf_ds_pri_act_e
{
    CF_DS_PRI_ACT_ASSIGN = 0,
    CF_DS_PRI_ACT_FROM_1ST_TAG,
    CF_DS_PRI_ACT_FROM_2ND_TAG, /* not support in test chip */
    CF_DS_PRI_ACT_FROM_INTERNAL,
    CF_DS_PRI_ACT_END,
} apollomp_raw_cf_ds_pri_act_t;

typedef enum apollomp_raw_cf_cf_pri_act_e
{
    CF_CF_PRI_ACT_NOP = 0,
    CF_CF_PRI_ACT_ASSIGN,
    CF_CF_PRI_ACT_END,
} apollomp_raw_cf_cf_pri_act_t;

typedef enum apollomp_raw_cf_ds_uni_act_e
{
    CF_DS_UNI_ACT_NOP = 0,
    CF_DS_UNI_ACT_MASK_BY_UNIMASK,
    CF_DS_UNI_ACT_FORCE_FORWARD,
    CF_DS_UNI_ACT_END,
} apollomp_raw_cf_ds_uni_act_t;

/*up stream action*/
typedef enum apollomp_raw_cf_us_csact_e
{
    CF_US_CSACT_NOP = 0,
    CF_US_CSACT_ADD_TAG_VS_TPID,
    CF_US_CSACT_ADD_TAG_8100,
    CF_US_CSACT_DEL_STAG, /* not support in test chip */
    CF_US_CSACT_TRANSPARENT, /* not support in test chip */
    CF_US_CSACT_END,
} apollomp_raw_cf_us_csact_t;

typedef enum apollomp_raw_cf_us_cact_e
{
    CF_US_CACT_NOP = 0,
    CF_US_CACT_ADD_CTAG_8100, /* not support in test chip */
    CF_US_CACT_TRANSLATION_C2S,
    CF_US_CACT_DEL_CTAG,
    CF_US_CACT_TRANSPARENT,
    CF_US_CACT_END,
} apollomp_raw_cf_us_cact_t;

typedef enum apollomp_raw_cf_us_vid_act_e
{
    CF_US_VID_ACT_ASSIGN = 0,
    CF_US_VID_ACT_FROM_1ST_TAG,
    CF_US_VID_ACT_FROM_2ND_TAG, /* not support in test chip */
    CF_US_VID_ACT_FROM_INTERNAL, /* not support in test chip */
    CF_US_VID_ACT_END,
} apollomp_raw_cf_us_vid_act_t;

typedef enum apollomp_raw_cf_us_pri_act_e
{
    CF_US_PRI_ACT_ASSIGN = 0,
    CF_US_PRI_ACT_FROM_1ST_TAG,
    CF_US_PRI_ACT_FROM_2ND_TAG, /* not support in test chip */
    CF_US_PRI_ACT_FROM_INTERNAL,
    CF_US_PRI_ACT_END,
} apollomp_raw_cf_us_pri_act_t;

typedef enum apollomp_raw_cf_us_sqid_act_e
{
    CF_US_SQID_ACT_ASSIGN_NOP =0,
    CF_US_SQID_ACT_ASSIGN_SID,
    CF_US_SQID_ACT_END,
} apollomp_raw_cf_us_sqid_act_t;

typedef enum apollomp_raw_cf_ip_rng_type_e
{
    CF_IP_RNG_SIP = 0,
    CF_IP_RNG_DIP,
    CF_IP_RNG_END,
} apollomp_raw_cf_ip_rng_type_t;

typedef enum apollomp_raw_cf_port_rng_type_e
{
    CF_PORT_RNG_SPORT = 0,
    CF_PORT_RNG_DPORT,
    CF_PORT_RNG_END,
} apollomp_raw_cf_port_rng_type_t;


typedef struct apollomp_raw_cf_pattern_s{
    uint32 etherType;
    uint32 direction;     /* upstraeam or downstream configuration*/
    uint32 tos_gemidx;    /* TOS/TC for upstream,GEMIDX/LLIDX for downstream*/
    uint32 tagVid;
    uint32 tagPri;
    uint32 interPri;
    uint32 ifStag;
    uint32 ifCtag;
    uint32 uni;

    uint32 portRange;
    uint32 ipRange;
    uint32 aclHit;
    uint32 wanIf;
    uint32 ip6Mc;
    uint32 ip4Mc;
    uint32 igmp;
    uint32 mld;
    uint32 dei;

}apollomp_raw_cf_pattern_t;

typedef struct apollomp_raw_cf_RuleEntry_s{
    uint32 idx;
    uint32 valid;
    apollomp_raw_cf_pattern_t data_bits;
    apollomp_raw_cf_pattern_t care_bits;
}apollomp_raw_cf_RuleEntry_t;


typedef struct apollomp_raw_cf_dsAct_s{
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

}apollomp_raw_cf_dsAct_t;

typedef struct apollomp_raw_cf_usAct_s{
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
    uint32 log_act;
    uint32 log_idx;
    uint32 dscp_remark;
    uint32 drop_act;

}apollomp_raw_cf_usAct_t;

extern int32 apollomp_raw_cf_valid_set(uint32 index, rtk_enable_t enabled);
extern int32 apollomp_raw_cf_valid_get(uint32 index, rtk_enable_t *pEnabled);
extern int32 apollomp_raw_cf_rule_set(apollomp_raw_cf_RuleEntry_t *pattern);
extern int32 apollomp_raw_cf_rule_get(apollomp_raw_cf_RuleEntry_t *pattern);
extern int32 apollomp_raw_cf_dsAct_set(apollomp_raw_cf_dsAct_t *pCfDsAct);
extern int32 apollomp_raw_cf_dsAct_get(apollomp_raw_cf_dsAct_t *pCfDsAct);
extern int32 apollomp_raw_cf_usAct_set(apollomp_raw_cf_usAct_t *pCfUsAct);
extern int32 apollomp_raw_cf_usAct_get(apollomp_raw_cf_usAct_t *pCfUsAct);
extern int32 apollomp_raw_cf_dsOp_set(uint32 index, rtk_enable_t notDs);
extern int32 apollomp_raw_cf_dsOp_get(uint32 index, rtk_enable_t *notDs);
extern int32 apollomp_raw_cf_usOp_set(uint32 index, rtk_enable_t notUs);
extern int32 apollomp_raw_cf_usOp_get(uint32 index, rtk_enable_t *notUs);
extern int32 apollomp_raw_cf_usPermit_set(apollomp_raw_cf_usPermit_t rule);
extern int32 apollomp_raw_cf_usPermit_get(apollomp_raw_cf_usPermit_t *pRule);
#endif /*_APOLLOMP_RAW_CLASSIFICATION_H_*/


