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

#ifndef __RTK_CLASSIFY_H__
#define __RTK_CLASSIFY_H__


/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */

#define CLASSIFY_RAW_FIELD_NUMBER 3

typedef enum rtk_classify_dirct_e
{
    CLASSIFY_DIRECTION_US = 0,
    CLASSIFY_DIRECTION_DS,
    CLASSIFY_DIRECTION_END
}rtk_classify_dirct_t;

typedef enum rtk_classify_invert_e
{
    CLASSIFY_INVERT_DISABLE = 0,
    CLASSIFY_INVERT_ENABLE,
    CLASSIFY_INVERT_END,
} rtk_classify_invert_t;

typedef enum rtk_classify_unmatch_action_e
{
    CLASSIFY_UNMATCH_DROP = 0,
    CLASSIFY_UNMATCH_PERMIT_WITHOUT_PON,
    CLASSIFY_UNMATCH_PERMIT,
    CLASSIFY_UNMATCH_END,
} rtk_classify_unmatch_action_t;

typedef enum rtk_classify_unmatch_action_ds_e
{
    CLASSIFY_UNMATCH_DS_DROP = 0,
    CLASSIFY_UNMATCH_DS_PERMIT,
    CLASSIFY_UNMATCH_DS_END,
} rtk_classify_unmatch_action_ds_t;


/*down stream action*/
typedef enum rtk_classify_ds_csact_e
{
    CLASSIFY_DS_CSACT_NOP = 0,
    CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID,
    CLASSIFY_DS_CSACT_ADD_TAG_8100,
    CLASSIFY_DS_CSACT_DEL_STAG,
    CLASSIFY_DS_CSACT_TRANSPARENT, /* not support in test chip */
    CLASSIFY_DS_CSACT_SP2C, /* not support in test chip */
    CLASSIFY_DS_CSACT_ADD_TAG_STAG_TPID,
    CLASSIFY_DS_CSACT_ACTCTRL_DISABLE,     /*for action ctrl bit, not support in Apollomp*/
    CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2,
    CLASSIFY_DS_CSACT_END,
} rtk_classify_ds_csact_t;

typedef enum rtk_classify_ds_cact_e
{
    CLASSIFY_DS_CACT_NOP = 0,
    CLASSIFY_DS_CACT_ADD_CTAG_8100,
    CLASSIFY_DS_CACT_TRANSLATION_SP2C,
    CLASSIFY_DS_CACT_DEL_CTAG, /* not support in test chip */
    CLASSIFY_DS_CACT_TRANSPARENT,
    CLASSIFY_DS_CACT_ACTCTRL_DISABLE,     /*for action ctrl bit, not support in Apollomp*/
    CLASSIFY_DS_CACT_END,
} rtk_classify_ds_cact_t;

typedef enum rtk_classify_ds_vid_act_e
{
    CLASSIFY_DS_VID_ACT_ASSIGN = 0,
    CLASSIFY_DS_VID_ACT_FROM_1ST_TAG,
    CLASSIFY_DS_VID_ACT_FROM_2ND_TAG, 
    CLASSIFY_DS_VID_ACT_FROM_LUT,
    CLASSIFY_DS_VID_ACT_TRANSLATION_SP2C,
    CLASSIFY_DS_VID_ACT_NOP,             /*not support in Apollomp*/
    CLASSIFY_DS_VID_ACT_END,
} rtk_classify_ds_vid_act_t;

typedef enum rtk_classify_ds_pri_act_e
{
    CLASSIFY_DS_PRI_ACT_ASSIGN = 0,
    CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG,
    CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG, 
    CLASSIFY_DS_PRI_ACT_FROM_INTERNAL,
    CLASSIFY_DS_PRI_ACT_TRANSLATION_SP2C,
    CLASSIFY_DS_PRI_ACT_FROM_DSCP,
    CLASSIFY_DS_PRI_ACT_NOP,          /*not support in Apollomp*/
    CLASSIFY_DS_PRI_ACT_END,
} rtk_classify_ds_pri_act_t;

typedef enum rtk_classify_cf_pri_act_e
{
    CLASSIFY_CF_PRI_ACT_NOP = 0,
    CLASSIFY_CF_PRI_ACT_ASSIGN,
    CLASSIFY_CF_PRI_ACT_ACTCTRL_DISABLE,    /*for action ctrl bit, not support in Apollomp*/
    CLASSIFY_CF_PRI_ACT_END,
} rtk_classify_cf_pri_act_t;

typedef enum rtk_classify_ds_uni_act_e
{
    CLASSIFY_DS_UNI_ACT_NOP =0 ,
    CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK,
    CLASSIFY_DS_UNI_ACT_FORCE_FORWARD,
    CLASSIFY_DS_UNI_ACT_TRAP,
    CLASSIFY_DS_UNI_ACT_ACTCTRL_DISABLE,     /*for action ctrl bit, not support in Apollomp*/
    CLASSIFY_DS_UNI_ACT_END,
} rtk_classify_ds_uni_act_t;

typedef enum rtk_classify_dscp_act_e
{
    CLASSIFY_DSCP_ACT_DISABLE = 0,
    CLASSIFY_DSCP_ACT_ENABLE,
    CLASSIFY_DSCP_ACT_ACTCTRL_DISABLE,     /*for action ctrl bit, not support in Apollomp*/
    CLASSIFY_DSCP_ACT_END,
} rtk_classify_dscp_act_t;

/*up stream action*/
typedef enum rtk_classify_us_csact_e
{
    CLASSIFY_US_CSACT_NOP = 0,
    CLASSIFY_US_CSACT_ADD_TAG_VS_TPID,
    CLASSIFY_US_CSACT_ADD_TAG_8100,
    CLASSIFY_US_CSACT_DEL_STAG, /* not support in test chip */
    CLASSIFY_US_CSACT_TRANSPARENT, /* not support in test chip */
    CLASSIFY_US_CSACT_ADD_TAG_STAG_TPID,    
    CLASSIFY_US_CSACT_ACTCTRL_DISABLE,     /*for action ctrl bit, not support in Apollomp*/
    CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2,
    CLASSIFY_US_CSACT_END,
} rtk_classify_us_csact_t;

typedef enum rtk_classify_us_cact_e
{
    CLASSIFY_US_CACT_NOP = 0,
    CLASSIFY_US_CACT_ADD_CTAG_8100, /* not support in test chip */
    CLASSIFY_US_CACT_TRANSLATION_C2S,
    CLASSIFY_US_CACT_DEL_CTAG,
    CLASSIFY_US_CACT_TRANSPARENT,
    CLASSIFY_US_CACT_ACTCTRL_DISABLE,      /*for action ctrl bit, not support in Apollomp*/
    CLASSIFY_US_CACT_END,
} rtk_classify_us_cact_t;

typedef enum rtk_classify_us_vid_act_e
{
    CLASSIFY_US_VID_ACT_ASSIGN = 0,
    CLASSIFY_US_VID_ACT_FROM_1ST_TAG,
    CLASSIFY_US_VID_ACT_FROM_2ND_TAG, 
    CLASSIFY_US_VID_ACT_FROM_INTERNAL,
    CLASSIFY_US_VID_ACT_NOP,          /*not support in Apollomp*/
    CLASSIFY_US_VID_ACT_END,
} rtk_classify_us_vid_act_t;

typedef enum rtk_classify_us_pri_act_e
{
    CLASSIFY_US_PRI_ACT_ASSIGN = 0,
    CLASSIFY_US_PRI_ACT_FROM_1ST_TAG,
    CLASSIFY_US_PRI_ACT_FROM_2ND_TAG, 
    CLASSIFY_US_PRI_ACT_FROM_INTERNAL,
    CLASSIFY_US_PRI_ACT_FROM_DSCP,
    CLASSIFY_US_PRI_ACT_NOP,          /*not support in Apollomp*/
    CLASSIFY_US_PRI_ACT_END,
} rtk_classify_us_pri_act_t;

typedef enum rtk_classify_us_sqid_act_e
{
    CLASSIFY_US_SQID_ACT_ASSIGN_NOP = 0,
    CLASSIFY_US_SQID_ACT_ASSIGN_SID,
    CLASSIFY_US_SQID_ACT_ASSIGN_QID,
    CLASSIFY_US_SQID_ACT_ACTCTRL_DISABLE,      /*for action ctrl bit, not support in Apollomp*/
    CLASSIFY_US_SQID_ACT_END,
} rtk_classify_us_sqid_act_t;

typedef enum rtk_classify_drop_act_e
{
    CLASSIFY_DROP_ACT_NONE = 0,
    CLASSIFY_DROP_ACT_ENABLE,
    CLASSIFY_DROP_ACT_TRAP,
    CLASSIFY_DROP_ACT_DROP_PON,
    CLASSIFY_DROP_ACT_ACTCTRL_DISABLE,      /*for action ctrl bit, not support in Apollomp*/
    CLASSIFY_DROP_ACT_END,
} rtk_classify_drop_act_t;

typedef enum rtk_classify_log_act_e
{
    CLASSIFY_US_LOG_ACT_NONE = 0,
    CLASSIFY_US_LOG_ACT_ENABLE,
    CLASSIFY_US_LOG_ACT_END,
} rtk_classify_log_act_t;

typedef enum rtk_classify_field_type_e
{
    CLASSIFY_FIELD_ETHERTYPE = 0,
    CLASSIFY_FIELD_TOS_DSIDX,
    CLASSIFY_FIELD_TAG_VID,
    CLASSIFY_FIELD_TAG_PRI,
    CLASSIFY_FIELD_INTER_PRI,
    CLASSIFY_FIELD_IS_CTAG,
    CLASSIFY_FIELD_IS_STAG,
    CLASSIFY_FIELD_UNI,
    CLASSIFY_FIELD_PORT_RANGE, /* not support in test chip */
    CLASSIFY_FIELD_IP_RANGE, /* not support in test chip */
    CLASSIFY_FIELD_ACL_HIT, /* not support in test chip */
    CLASSIFY_FIELD_WAN_IF, /* not support in test chip */
    CLASSIFY_FIELD_IP6_MC, /* not support in test chip */
    CLASSIFY_FIELD_IP4_MC, /* not support in test chip */
    CLASSIFY_FIELD_MLD, /* not support in test chip */
    CLASSIFY_FIELD_IGMP, /* not support in test chip */
    CLASSIFY_FIELD_DEI, /* not support in test chip */
    CLASSIFY_FIELD_INNER_VLAN, /* not support in apollomp */
    CLASSIFY_FIELD_PPPOE,      /* not support in apollomp, 9601b */
    CLASSIFY_FIELD_IPV4,       /* not support in apollomp, 9601b */
    CLASSIFY_FIELD_IPV6,       /* not support in apollomp, 9601b */
    CLASSIFY_FIELD_INTERNAL_VID_TAG_IF, /* not support in apollomp, 9601b */
    CLASSIFY_FIELD_CF_ACL_HIT, /* not support in apollomp, 9601b */
    CLASSIFY_FIELD_STPID_SEL,  /* not support in apollomp, 9601b */
    CLASSIFY_FIELD_IGMP_MLD,  /* not support in apollomp, 9601b */
    CLASSIFY_FIELD_IPMC,  /* not support in apollomp, 9601b */
    CLASSIFY_FIELD_END
} rtk_classify_field_type_t;

/* range check */
typedef enum rtk_classify_portrange_e
{
    CLASSIFY_PORTRANGE_SPORT = 0,
    CLASSIFY_PORTRANGE_DPORT,
    CLASSIFY_PORTRANGE_END
}rtk_classify_portrange_t;

typedef enum rtk_classify_iprange_e
{
    CLASSIFY_IPRANGE_IPV4_SIP = 0,
    CLASSIFY_IPRANGE_IPV4_DIP,
    CLASSIFY_IPRANGE_END
}rtk_classify_iprange_t;

/* CF port selection */
typedef enum rtk_classify_cf_sel_e
{
    CLASSIFY_CF_SEL_DISABLE = 0,
    CLASSIFY_CF_SEL_ENABLE,
    CLASSIFY_CF_SEL_END,
} rtk_classify_cf_sel_t;


typedef enum rtk_classify_permit_sel_e
{
    CLASSIFY_CF_SEL_FROM_0 = 0,
    CLASSIFY_CF_SEL_FROM_64,
    CLASSIFY_CF_SEL_PERMIT_END,
} rtk_classify_permit_sel_t;

typedef enum rtk_classify_us_1premark_prior_e
{
    CLASSIFY_US_1PREMARK_PRIOR_ACL = 0,
    CLASSIFY_US_1PREMARK_PRIOR_CF,
    CLASSIFY_US_1PREMARK_PRIOR_END,
} rtk_classify_us_1premark_prior_t;

typedef enum rtk_classify_template_vid_cfg_e
{
    CLASSIFY_TEMPLATE_VID_CFG_INGRESS_CVID = 0,
    CLASSIFY_TEMPLATE_VID_CFG_INGRESS_OUTERTAG_VID,
    CLASSIFY_TEMPLATE_VID_CFG_INTERNAL_CVID,
    CLASSIFY_TEMPLATE_VID_CFG_END,
} rtk_classify_template_vid_cfg_t;

typedef enum rtk_classify_template_pri_cfg_e
{
    CLASSIFY_TEMPLATE_PRI_CFG_INGRESS_CPRI = 0,
    CLASSIFY_TEMPLATE_PRI_CFG_INTERNAL_PRI,
    CLASSIFY_TEMPLATE_PRI_CFG_REMARK_CPRI,
    CLASSIFY_TEMPLATE_PRI_CFG_END,
} rtk_classify_template_pri_cfg_t;


/*
 * Data Declaration
 */

typedef struct rtk_classify_value_s
{
    uint16 value;
    uint16 mask;
} rtk_classify_value_t;


typedef struct rtk_classify_field_s
{
    rtk_classify_field_type_t fieldType;
    union
    {
        rtk_classify_value_t etherType;
        rtk_classify_value_t tosDsidx;
        rtk_classify_value_t tagVid;
        rtk_classify_value_t tagPri;
        rtk_classify_value_t interPri;
        rtk_classify_value_t isCtag;
        rtk_classify_value_t isStag;
        rtk_classify_value_t uni;
        rtk_classify_value_t portRange; /* not support in test chip */
        rtk_classify_value_t ipRange; /* not support in test chip */
        rtk_classify_value_t aclHit; /* not support in test chip */
        rtk_classify_value_t wanIf; /* not support in test chip */
        rtk_classify_value_t ip6Mc; /* not support in test chip */
        rtk_classify_value_t ip4Mc; /* not support in test chip */
        rtk_classify_value_t mld; /* not support in test chip */
        rtk_classify_value_t igmp; /* not support in test chip */
        rtk_classify_value_t dei; /* not support in test chip */
        rtk_classify_value_t innerVlan; /* not support in test chip & apollomp */
        rtk_classify_value_t pppoe; /* not support in apollomp, 9601b */
        rtk_classify_value_t ipv4; /* not support in apollomp, 9601b */
        rtk_classify_value_t ipv6; /* not support in apollomp, 9601b */
        rtk_classify_value_t intVidTagIf; /* not support in apollomp, 9601b */
        rtk_classify_value_t cfAclHit; /* not support in apollomp, 9601b */
        rtk_classify_value_t stpidSel; /* not support in apollomp, 9601b */
        rtk_classify_value_t igmp_mld; /* not support in apollomp, 9601b */
        rtk_classify_value_t ipmc;     /* not support in apollomp, 9601b */
        rtk_classify_value_t fieldData;

	} classify_pattern;
    struct rtk_classify_field_s *next;
} rtk_classify_field_t;



typedef struct rtk_classify_ds_act_s
{
    rtk_classify_ds_csact_t   csAct;
    rtk_classify_ds_vid_act_t csVidAct; /* not support in test chip */
    rtk_classify_ds_pri_act_t csPriAct; /* not support in test chip */
    rtk_classify_ds_cact_t    cAct;
    rtk_classify_ds_vid_act_t cVidAct;
    rtk_classify_ds_pri_act_t cPriAct;
    rtk_classify_cf_pri_act_t interPriAct;
    rtk_classify_ds_uni_act_t uniAct;
    rtk_classify_dscp_act_t dscp; /* not support in test chip */
    uint32 sTagVid; /* not support in test chip */
    uint8  sTagPri; /* not support in test chip */
    uint32 cTagVid;
    uint8  cTagPri;
    uint8  cfPri;
    rtk_portmask_t uniMask;
    uint8  dscpVal; /* not support in test chip */
} rtk_classify_ds_act_t;


typedef struct rtk_classify_us_act_s
{
    rtk_classify_us_csact_t     csAct;
    rtk_classify_us_vid_act_t   csVidAct;
    rtk_classify_us_pri_act_t   csPriAct;
    rtk_classify_us_cact_t      cAct;
    rtk_classify_us_sqid_act_t  sidQidAct;
    rtk_classify_us_vid_act_t   cVidAct; /* not support in test chip */
    rtk_classify_us_pri_act_t   cPriAct; /* not support in test chip */
    rtk_classify_cf_pri_act_t   interPriAct; /* not support in test chip */
    rtk_classify_dscp_act_t     dscp; /* not support in test chip */
    rtk_classify_drop_act_t     drop; /* not support in test chip */
    rtk_classify_log_act_t      log; /* not support in test chip */
    uint32 sTagVid;
    uint8  sTagPri;
    uint32 sidQid;
    uint32 cTagVid; /* not support in test chip */
    uint8  cTagPri; /* not support in test chip */
    uint8  cfPri; /* not support in test chip */
    uint8  logCntIdx; /* not support in test chip */
    uint8  dscpVal; /* not support in test chip */

} rtk_classify_us_act_t;

typedef struct rtk_classify_raw_field_s
{
    uint16      dataFieldRaw[CLASSIFY_RAW_FIELD_NUMBER];
    uint16      careFieldRaw[CLASSIFY_RAW_FIELD_NUMBER];
} rtk_classify_raw_field_t;

typedef struct rtk_classify_cfg_s
{
    uint32 index;
    uint32 patternIdx;
    uint32 templateIdx;
	
    rtk_classify_dirct_t direction;
    //rtk_classify_field_format_t fieldFmt;
    union{
        rtk_classify_field_t *pFieldHead;   /* for set operation */
        rtk_classify_raw_field_t readField; /* for get operation */
	} field;
    union{
        rtk_classify_ds_act_t dsAct;
        rtk_classify_us_act_t usAct;
	} act;
	rtk_classify_invert_t invert;
    rtk_enable_t valid; /* for get operation */
} rtk_classify_cfg_t;

typedef struct rtk_classify_rangeCheck_l4Port_s
{
    uint16  index;
    uint16  upperPort;    /* Port range upper bound */
    uint16  lowerPort;    /* Port range lower bound */
    rtk_classify_portrange_t type;
} rtk_classify_rangeCheck_l4Port_t;

typedef struct rtk_classify_rangeCheck_ip_s
{
    uint16 index;
    ipaddr_t upperIp;    /* IP range upper bound */
    ipaddr_t lowerIp;    /* IP range lower bound */
    rtk_classify_iprange_t type;
} rtk_classify_rangeCheck_ip_t;


typedef struct rtk_classify_template_cfg_pattern0_s
{
    uint16 index;
    rtk_classify_dirct_t direction;  
    rtk_classify_template_vid_cfg_t vidCfg; 
    rtk_classify_template_pri_cfg_t priCfg;
} rtk_classify_template_cfg_pattern0_t;

typedef struct rtk_classify_default_wanIf_s
{
	uint32 l2WanIf;
	uint32 mcastWanIf;
} rtk_classify_default_wanIf_t;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_classify_init
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
rtk_classify_init(void);

/* Function Name:
 *      rtk_classify_cfgEntry_add
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
rtk_classify_cfgEntry_add(rtk_classify_cfg_t *pClassifyCfg);


/* Function Name:
 *      rtk_classify_cfgEntry_get
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
rtk_classify_cfgEntry_get(rtk_classify_cfg_t *pClassifyCfg);


/* Function Name:
 *      rtk_classify_cfgEntry_del
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
rtk_classify_cfgEntry_del(uint32 entryIdx);


/* Function Name:
 *      rtk_classify_field_add
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
 *      - caller should not free (*pClassifyField) before rtk_classify_cfgEntry_add is called
 */
extern int32
rtk_classify_field_add(rtk_classify_cfg_t *pClassifyEntry, rtk_classify_field_t *pClassifyField);


/* Function Name:
 *      rtk_classify_unmatchAction_set
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
rtk_classify_unmatchAction_set(rtk_classify_unmatch_action_t action);


/* Function Name:
 *      rtk_classify_unmatchAction_get
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
rtk_classify_unmatchAction_get(rtk_classify_unmatch_action_t *pAction);

/* Function Name:
 *      rtk_classify_unmatchAction_ds_set
 * Description:
 *      Apply action to downstream packets when no classfication configuration match
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
rtk_classify_unmatchAction_ds_set(rtk_classify_unmatch_action_ds_t action);


/* Function Name:
 *      rtk_classify_unmatchAction_get
 * Description:
 *      Get action to downstream packets when no classfication configuration match
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
rtk_classify_unmatchAction_ds_get(rtk_classify_unmatch_action_ds_t *pAction);


/* Function Name:
 *      rtk_classify_portRange_set
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
extern int32
rtk_classify_portRange_set(rtk_classify_rangeCheck_l4Port_t *pRangeEntry);


/* Function Name:
 *      rtk_classify_portRange_get
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
extern int32
rtk_classify_portRange_get(rtk_classify_rangeCheck_l4Port_t *pRangeEntry);


/* Function Name:
 *      rtk_classify_ipRange_set
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
extern int32
rtk_classify_ipRange_set(rtk_classify_rangeCheck_ip_t *pRangeEntry);


/* Function Name:
 *      rtk_classify_ipRange_get
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
extern int32
rtk_classify_ipRange_get(rtk_classify_rangeCheck_ip_t *pRangeEntry);

/* Function Name:
 *      rtk_classify_cfSel_set
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
extern int32
rtk_classify_cfSel_set(rtk_port_t port, rtk_classify_cf_sel_t cfSel);

/* Function Name:
 *      rtk_classify_cfSel_get
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
 *      RT_ERR_NULL_POINTER    - Pointer pClassifyCfg point to NULL.
 * Note:
 *      Only accept pon port and RGMII port.
 *      This function is not supported in Test chip.
 */
extern int32
rtk_classify_cfSel_get(rtk_port_t port, rtk_classify_cf_sel_t *pCfSel);

/* Function Name:
 *      rtk_classify_cfPri2Dscp_set
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
rtk_classify_cfPri2Dscp_set(rtk_pri_t pri, rtk_dscp_t dscp);

/* Function Name:
 *      rtk_classify_cfPri2Dscp_get
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
rtk_classify_cfPri2Dscp_get(rtk_pri_t pri, rtk_dscp_t *pDscp);


/* Function Name:
 *      rtk_classify_permit_sel_get
 * Description:
 *      Set classification permit selection, from 0 to 511 or from 64 to 511
 * Input:
 *      pPermitSel   -point of CF permit selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *      Only accept from 0 to 511 or from 64 to 511
 */
extern int32
rtk_classify_permit_sel_get( rtk_classify_permit_sel_t *pPermitSel);

/* Function Name:
 *      rtk_classify_permit_sel_set
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
rtk_classify_permit_sel_set( rtk_classify_permit_sel_t permitSel);

/* Function Name:
 *      rtk_classify_us1pRemarkPrior_set
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
extern int32
rtk_classify_us1pRemarkPrior_set( rtk_classify_us_1premark_prior_t prior);

/* Function Name:
 *      rtk_classify_us1pRemarkPrior_get
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
extern int32
rtk_classify_us1pRemarkPrior_get( rtk_classify_us_1premark_prior_t *pPrior);

/* Function Name:
 *      rtk_classify_templateCfgPattern0_set
 * Description:
 *      Set classification template cfg for pattern 0 
 * Input:
 *      p0TemplateCfg   - template cfg for pattern 0 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
extern int32
rtk_classify_templateCfgPattern0_set( rtk_classify_template_cfg_pattern0_t *pP0TemplateCfg);

/* Function Name:
 *      rtk_classify_templateCfgPattern0_get
 * Description:
 *      Get classification template cfg for pattern 0 
 * Input:
 *      None 
 * Output:
 *      p0TemplateCfg   - template cfg for pattern 0
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
extern int32
rtk_classify_templateCfgPattern0_get( rtk_classify_template_cfg_pattern0_t *pP0TemplateCfg);

/* Function Name:
 *      rtk_classify_entryNumPattern1_set
 * Description:
 *      Set entry number of pattern 1 
 * Input:
 *      entryNum   - Entry number of pattern 1 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
extern int32
rtk_classify_entryNumPattern1_set( uint32 entryNum);

/* Function Name:
 *      rtk_classify_entryNumPattern1_get
 * Description:
 *      Get entry number of pattern 1 
 * Input:
 *      None
 * Output:
 *      entryNum   - Entry number of pattern 1
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
extern int32
rtk_classify_entryNumPattern1_get( uint32 *entryNum);

/* Function Name:
 *      rtk_classify_defaultWanIf_set
 * Description:
 *      Set default WAN interface for packet not assiged wan_if by NAT 
 * Input:
 *      l2WanIf    - WAN interface index for L2 packet
 *      mcastWanIf - WAN interface index for multicast packet
 * Output:
 *      N/A
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
extern int32
rtk_classify_defaultWanIf_set( rtk_classify_default_wanIf_t *pDefaultWanIf);

/* Function Name:
 *      rtk_classify_defaultWanIf_get
 * Description:
 *      Get default WAN interface for packet not assiged wan_if by NAT 
 * Input:
 *      N/A
 * Output:
 *      l2WanIf    - WAN interface index for L2 packet
 *      mcastWanIf - WAN interface index for multicast packet
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
extern int32
rtk_classify_defaultWanIf_get( rtk_classify_default_wanIf_t *pDefaultWanIf);

/* Function Name:
 *      rtk_classify_trapPri_set
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
rtk_classify_trapPri_set(rtk_pri_t trap_pri);

/* Function Name:
 *      rtk_classify_trapPri_get
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
rtk_classify_trapPri_get(rtk_pri_t *pTrap_pri);

#endif /* __RTK_CLASSIFY_H__ */

