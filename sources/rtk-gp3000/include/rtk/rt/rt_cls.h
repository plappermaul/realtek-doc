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
 * Purpose : Definition of classify rule global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) classify rule add/delete
 *
 */
 
#ifndef __RT_CLS_H__
#define __RT_CLS_H__

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>

/*
 * Symbol Definition
 */
 
#define RT_CLS_HW_INDEX_MAX 10

typedef enum rt_cls_get_op_e{
    RT_CLS_GET_OP_FIRST,
    RT_CLS_GET_OP_NEXT,
    RT_CLS_GET_OP_INDEX
} rt_cls_get_op_t;

typedef enum rt_cls_dir_e{
    RT_CLS_DIR_DS,
    RT_CLS_DIR_US
} rt_cls_dir_t;

typedef enum rt_cls_ext_pkt_type_e{
    RT_CLS_EXT_PKT_TYPE_UC,
    RT_CLS_EXT_PKT_TYPE_BC,
    RT_CLS_EXT_PKT_TYPE_MC,
    RT_CLS_EXT_PKT_TYPE_UUC,
    RT_CLS_EXT_PKT_TYPE_UMC,
    RT_CLS_EXT_PKT_TYPE_END
}rt_cls_ext_pkt_type_t;



typedef enum rt_cls_filter_mask_e
{
    RT_CLS_FILTER_FLOWID_BIT       =(1<<0),
    RT_CLS_FILTER_OUTER_TAGIf_BIT  =(1<<1),
    RT_CLS_FILTER_INNER_TAGIf_BIT  =(1<<2),
    RT_CLS_FILTER_OUTER_TPID_BIT   =(1<<3),
    RT_CLS_FILTER_OUTER_VID_BIT    =(1<<4),
    RT_CLS_FILTER_OUTER_PRI_BIT    =(1<<5),
    RT_CLS_FILTER_OUTER_DEI_BIT    =(1<<6),
    RT_CLS_FILTER_INNER_TPID_BIT   =(1<<7),
    RT_CLS_FILTER_INNER_VID_BIT    =(1<<8),
    RT_CLS_FILTER_INNER_PRI_BIT    =(1<<9),
    RT_CLS_FILTER_INNER_DEI_BIT    =(1<<10),
    RT_CLS_FILTER_ETHERTYPE        =(1<<11),
    RT_CLS_FILTER_DSCP_BIT         =(1<<12),
    RT_CLS_FILTER_EGRESS_PORT_BIT  =(1<<13)
}rt_cls_filter_mask_t;

typedef enum rt_cls_tag_action_e
{
    RT_CLS_TAG_TRANSPARENT,
    RT_CLS_TAG_TAGGING,
    RT_CLS_TAG_MODIFY,
    RT_CLS_TAG_REMOVE,
    RT_CLS_TAG_END
}rt_cls_tag_action_t;

typedef enum rt_cls_tpid_action_e{
    RT_CLS_TPID_ASSIGN,
    RT_CLS_TPID_COPY_FROM_1ST_TAG,        //outer
    RT_CLS_TPID_COPY_FROM_2ND_TAG,        //inner
    RT_CLS_TPID_END
}rt_cls_tpid_action_t;

typedef enum rt_cls_vid_action_e
{
    RT_CLS_VID_ASSIGN,
    RT_CLS_VID_COPY_FROM_1ST_TAG,        //outer
    RT_CLS_VID_COPY_FROM_2ND_TAG,        //inner
    RT_CLS_VID_TRANSPARENT,
    RT_CLS_VID_END,
}rt_cls_vid_action_t;

typedef enum rt_cls_pri_action_e{
    RT_CLS_PRI_ASSIGN,
    RT_CLS_PRI_COPY_FROM_1ST_TAG,        //outer
    RT_CLS_PRI_COPY_FROM_2ND_TAG,        //inner
    RT_CLS_PRI_COPY_FROM_DSCP_REMAP,
    RT_CLS_PRI_TRANSPARENT,
    RT_CLS_PRI_END,
}rt_cls_pri_action_t;

typedef enum rt_cls_dei_action_e{
    RT_CLS_DEI_ASSIGN,
    RT_CLS_DEI_COPY_FROM_1ST_TAG,        //outer
    RT_CLS_DEI_COPY_FROM_2ND_TAG,        //inner
    RT_CLS_DEI_END,
}rt_cls_dei_action_t;

typedef enum rt_cls_ds_pri_act_e
{
     RT_CLS_DS_PRI_ACT_NOP = 0,
     RT_CLS_DS_PRI_ACT_ASSIGN = 1,
} rt_cls_ds_pri_act_t;


typedef enum rt_cls_ext_stag_action_e
{
    RT_CLS_EXT_STAG_TRANSPARENT,  /* Keep original S tag */
    RT_CLS_EXT_STAG_REMOVE,       /* Delete original S tag */
    RT_CLS_EXT_STAG_TPID_ORI,     /* Add or keep S tag with original TPID */
    RT_CLS_EXT_STAG_TPID_A,       /* Add or keep S tag with S tag TPID_A */
    RT_CLS_EXT_STAG_TPID_B,       /* Add or keep S tag with S tag TPID_B */
    RT_CLS_EXT_STAG_END
}rt_cls_ext_stag_action_t;

typedef enum rt_cls_ext_ctag_action_e
{
    RT_CLS_EXT_CTAG_TRANSPARENT,  /* Keep original C tag */
    RT_CLS_EXT_CTAG_REMOVE,       /* Delete original C tag */
    RT_CLS_EXT_CTAG_TAGING,       /* Add or keep C tag with TPID 0x8100 */
    RT_CLS_EXT_CTAG_END
}rt_cls_ext_ctag_action_t;

typedef enum rt_cls_ext_vid_action_e
{
    RT_CLS_EXT_VID_ASSIGN,
    RT_CLS_EXT_VID_COPY_FROM_1ST_TAG,  /* VID copy from outer tag, if no outer tag then as assian */
    RT_CLS_EXT_VID_COPY_FROM_2ND_TAG,  /* VID copy from inner tag, if no inner tag then as assian */
    RT_CLS_EXT_VID_FROM_FDB,           /* VID copy from the learning VID of DMAC in the MAC table*/
    RT_CLS_EXT_VID_END,
}rt_cls_ext_vid_action_t;

typedef enum rt_cls_ext_pri_action_e{
    RT_CLS_EXT_PRI_ASSIGN,
    RT_CLS_EXT_PRI_COPY_FROM_1ST_TAG,        /* PRI copy from outer tag, if no outer tag then as assian */
    RT_CLS_EXT_PRI_COPY_FROM_2ND_TAG,        /* PRI copy from inner tag, if no inner tag then as assian */
    RT_CLS_EXT_PRI_COPY_FROM_DSCP_REMAP,     /* PRI assian from DSCP remappin table */
    RT_CLS_EXT_PRI_END,
}rt_cls_ext_pri_action_t;

typedef enum rt_cls_ext_dei_action_e{
    RT_CLS_EXT_DEI_ASSIGN_0,   /* Assign DEI to 0 */
    RT_CLS_EXT_DEI_KEEP,       /* Keep ingress DEI */
    RT_CLS_EXT_DEI_END,
}rt_cls_ext_dei_action_t;

typedef enum rt_cls_ext_fwd_action_e{
    RT_CLS_EXT_FWD_NOP,
    RT_CLS_EXT_FWD_DROP,
    RT_CLS_EXT_FWD_TRAP,    /* Trap to CPU*/
    RT_CLS_EXT_FWD_LDPID,   /* Forward to specify port */
    RT_CLS_EXT_FWD_MCGID,   /* Forward to specify MC group */
    RT_CLS_EXT_FWD_END,
}rt_cls_ext_fwd_action_t;

typedef enum rt_cls_ext_action_e{
    RT_CLS_EXT_NOP,
    RT_CLS_EXT_ASSIGN,
    RT_CLS_EXT_END,
}rt_cls_ext_action_t;


typedef enum rt_cls_vlan_attribute_e{
    RT_CLS_VLAN_STAG_MATCH,     /* S TPID match bitmap */
    RT_CLS_VLAN_CTAG_MATCH,     /* C TPID match bitmap */
    RT_CLS_VLAN_OUTER_STAG,
    RT_CLS_VLAN_INNER_STAG,
    RT_CLS_VLAN_OUTER_CTAG,
    RT_CLS_VLAN_INNER_CTAG,
    RT_CLS_VLAN_STAG_TPID_A,
    RT_CLS_VLAN_STAG_TPID_B,
    RT_CLS_VLAN_USER_STAG_TPID,
    RT_CLS_VLAN_USER_CTAG_TPID,
    RT_CLS_VLAN_END
}rt_cls_vlan_attribute_t;

typedef struct rt_cls_filter_rule_s
{
    /* filter mask */
    rt_cls_filter_mask_t filterMask;
    /* filter values */
    uint32               flowId;
    uint32               outerTagIf;
    uint32               innerTagIf;
    uint32               outerTagTpid;
    uint32               outerTagVid;
    uint32               outerTagPri;
    uint32               outerTagDei;
    uint32               innerTagTpid;
    uint32               innerTagVid;
    uint32               innerTagPri;
    uint32               innerTagDei;
    uint32               etherType;
    uint8                dscp[8]; /*64 bit 0~63 */
    uint32               egressPortMask;
} rt_cls_filter_rule_t;

typedef struct rt_cls_rule_action_s
{
    rt_cls_tag_action_t     tagAction;
    rt_cls_tpid_action_t    tagTpidAction;
    rt_cls_vid_action_t     tagVidAction;
    rt_cls_pri_action_t     tagPriAction;
    rt_cls_dei_action_t     tagDeiAction;
    int32 assignedTpid;
    int32 assignedVid;
    int32 assignedPri;
    int32 assignedDei;
}rt_cls_rule_action_t;




typedef struct rt_cls_rule_s
{
    uint32               index;
    uint32               flowId;
    uint32               rulePriority;
    rt_cls_dir_t         direction;
    uint32               ingressPortMask;
    rt_cls_filter_rule_t filterRule;
    rt_cls_rule_action_t outerTagAct;
    rt_cls_rule_action_t innerTagAct;
    rt_cls_ds_pri_act_t  dsPriAct;
    uint32               dsPri;
} rt_cls_rule_t;

typedef struct rt_cls_extend_filter_field_s
{
    uint8 egressPort;
    uint8 ingressPort;
    uint8 sbit;          /* S-tag interface */
    uint8 cbit;          /* C-tag interface */
    uint8 sTpid;         /* S-tag TPID match index */
    uint8 pktType;
    uint8 ipv4;
    uint8 ipv6;
    uint8 igmpMld;
    uint16 flowId;       /* Downstream flow ID */
    uint8 direction;
    uint16 ctagVid;
    uint8 ctagPri;
    uint8 ctagCfi;
    uint16 outerVid;
    uint8 outerPri;
    uint8 outerDei;
    uint8 tosTc;
    uint16 ethertype;
} rt_cls_extend_filter_field_t;


typedef struct rt_cls_extend_filter_rule_s
{
    rt_cls_extend_filter_field_t mask;
    rt_cls_extend_filter_field_t data;
} rt_cls_extend_filter_rule_t;

typedef struct rt_cls_extend_stag_action_s
{
    rt_cls_ext_stag_action_t tagAction;
    rt_cls_ext_vid_action_t vidAction;
    rt_cls_ext_pri_action_t priAction;
    rt_cls_ext_dei_action_t deiAction;
    uint32 assignedVid;                      /* VID value */
    uint32 assignedPri;                      /* PRI value */
} rt_cls_extend_stag_action_t;

typedef struct rt_cls_extend_ctag_action_s
{
    rt_cls_ext_ctag_action_t tagAction;
    rt_cls_ext_vid_action_t vidAction;
    rt_cls_ext_pri_action_t priAction;
    rt_cls_ext_dei_action_t deiAction;
    uint32 assignedVid;                      /* VID value */
    uint32 assignedPri;                      /* PRI value */
} rt_cls_extend_ctag_action_t;

typedef struct rt_cls_extend_rule_s
{
    uint32                      index;
    uint32                      valid;
    rt_cls_extend_filter_rule_t filterRule;
    rt_cls_extend_stag_action_t sTagAct;
    rt_cls_extend_ctag_action_t cTagAct;
    rt_cls_ext_action_t         grpPolAct;
    uint32                      grpPolId;
    rt_cls_ext_action_t         flowAct;
    uint32                      flowId;
    rt_cls_ext_fwd_action_t     fwdAct;
    uint32                      fwdId;
    rt_cls_ext_action_t         cosAct;
    uint32                      cos;
} rt_cls_extend_rule_t;

typedef struct rt_cls_rule_info_s
{
    rt_cls_rule_t        rule;                          /* CLS Rule*/
    uint16               hwIndex[RT_CLS_HW_INDEX_MAX];  /* CLS hardware rule index, 0xff mean not in use*/
} rt_cls_rule_info_t;


/*
 * Function Declaration
 */

/* Module Name    : Classify rule     */
/* Sub-module Name: Add/delete classify rule */

/* Function Name:
 *      rt_cls_init
 * Description:
 *      Initialize cls setting
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None
 */
extern int32 rt_cls_init(void);

/* Function Name:
 *      rt_cls_rule_add
 * Description:
 *      Add  classify rule and apply to ASIC.
 * Input:
 *      pClsRule - The classify rule configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32 rt_cls_rule_add(rt_cls_rule_t *pClsRule);

/* Function Name:
 *      rt_cls_rule_delete
 * Description:
 *      Delete  classify rule.
 * Input:
 *      index     - index of classify rule entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None.
 */
extern int32 rt_cls_rule_delete(uint32 index);

/* Function Name:
 *      rt_cls_rule_get
 * Description:
 *      Get  classify rule.
 * Input:
 *      op        - operation type of get (first/next/index)
 *      index     - index of classify rule entry.
 *      dir       - direction of classify rule
 * Output:
 *      pClsRuleInfo  - classify rule information
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Rule not found
 * Note:
 *      None.
 */
extern int32 rt_cls_rule_get(rt_cls_get_op_t op, uint32 index, rt_cls_dir_t dir, rt_cls_rule_info_t *pClsRuleInfo);

/* Function Name:
 *      rt_cls_extend_rule_add
 * Description:
 *      Add extend classify rule to ASIC.
 * Input:
 *      pClsRule - The extend classify rule configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
extern int32 rt_cls_extend_rule_add(rt_cls_extend_rule_t *pExtClsRule);

/* Function Name:
 *      rt_cls_extend_rule_delete
 * Description:
 *      Delete extend classify rule.
 * Input:
 *      index     - index of extend classify rule.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None.
 */
extern int32 rt_cls_extend_rule_delete(uint32 index);

/* Function Name:
 *      rt_cls_extend_rule_get
 * Description:
 *      Get extend classify rule.
 * Input:
 *      index     - index of classify rule entry.
 * Output:
 *      pClsRule  - extend classify rule information
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Rule not found
 * Note:
 *      None.
 */
extern int32 rt_cls_extend_rule_get(uint32 index, rt_cls_extend_rule_t *pExtClsRule);



/* Function Name:
 *      rt_cls_fwdPort_set
 * Description:
 *      Set VEIP port default forward port.
 * Input:
 *      port     - port index.
 *      fwdPort  - defalt forward port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None.
 */
extern int32 rt_cls_fwdPort_set(rt_port_t port, rt_port_t fwdPort);

/* Function Name:
 *      rt_cls_vlan_paser_set
 * Description:
 *      Set VLAN parser setting.
 * Input:
 *      port     - port index.
 *      type     - vlan parser attribute type
 *      data     - value of vlan parser attribute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None
 */
extern int32 rt_cls_vlan_paser_set(rt_port_t port, rt_cls_vlan_attribute_t type, uint32 data);

/* Function Name:
 *      rt_cls_vlan_paser_get
 * Description:
 *      Get VLAN parser setting.
 * Input:
 *      port     - port index.
 *      type     - vlan parser attribute type
 * Output:
 *      pData     - value of vlan parser attribute
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 * Note:
 *      None
 */
extern int32 rt_cls_vlan_paser_get(rt_port_t port, rt_cls_vlan_attribute_t type, uint32 *pData);



#endif

