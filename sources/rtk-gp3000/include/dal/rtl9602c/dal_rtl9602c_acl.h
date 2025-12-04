/*
 * Copyright (C) 2014 Realtek Semiconductor Corp. 
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
 * $Date: 2013-05-03 17:35:27 +0800 (週五, 03 五月 2013) $
 *
 * Purpose : Definition of ACL API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) ACL rule action configure and modification
 *
 */
 
#ifndef __DAL_RTL9602C_ACL_H__
#define __DAL_RTL9602C_ACL_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/rtl9602c/dal_rtl9602c.h>


/*
 * Symbol Definition
 */
#define RTL9602C_ACLRULETBLEN				   	5


typedef struct rtl9602c_raw_acl_rule_s
{
	uint32 active_portmsk;
	uint32 type;
	uint32 tag_exist;
	uint32 field[RTL9602C_MAX_NUM_OF_ACL_RULE_FIELD];
    
}rtl9602c_raw_acl_rule_t;

typedef struct rtl9602c_raw_acl_ruleEntry_s
{
    uint32 idx;
    
    rtl9602c_raw_acl_rule_t data_bits;
	uint32 valid;
    rtl9602c_raw_acl_rule_t care_bits;

}rtl9602c_raw_acl_ruleEntry_t;


typedef enum rtl9602c_dal_acl_fieldTypes_e
{
	RTL9602C_DAL_ACL_UNUSED,
	RTL9602C_DAL_ACL_DMAC0,
	RTL9602C_DAL_ACL_DMAC1,
	RTL9602C_DAL_ACL_DMAC2,
	RTL9602C_DAL_ACL_SMAC0,
	RTL9602C_DAL_ACL_SMAC1,
	RTL9602C_DAL_ACL_SMAC2,
	RTL9602C_DAL_ACL_ETHERTYPE,
	RTL9602C_DAL_ACL_STAG,
	RTL9602C_DAL_ACL_CTAG,
	RTL9602C_DAL_ACL_GEMIDXLLIDX,
	RTL9602C_DAL_ACL_IP4SIP0 = 0x10,
	RTL9602C_DAL_ACL_IP4SIP1,
	RTL9602C_DAL_ACL_IP4DIP0,
	RTL9602C_DAL_ACL_IP4DIP1,
	RTL9602C_DAL_ACL_TOS_PROTO,
	RTL9602C_DAL_ACL_TC_NH,
	RTL9602C_DAL_ACL_IP6SIP0 = 0x20,
	RTL9602C_DAL_ACL_IP6SIP1,
    RTL9602C_DAL_ACL_IP6SIP2,
	RTL9602C_DAL_ACL_IP6SIP3,
	RTL9602C_DAL_ACL_IP6SIP4,
	RTL9602C_DAL_ACL_IP6SIP5,
	RTL9602C_DAL_ACL_IP6SIP6,
	RTL9602C_DAL_ACL_IP6SIP7,
	RTL9602C_DAL_ACL_IP6DIP0,
	RTL9602C_DAL_ACL_IP6DIP1,
	RTL9602C_DAL_ACL_IP6DIP2,
	RTL9602C_DAL_ACL_IP6DIP3,
	RTL9602C_DAL_ACL_IP6DIP4,
	RTL9602C_DAL_ACL_IP6DIP5,
	RTL9602C_DAL_ACL_IP6DIP6,
	RTL9602C_DAL_ACL_IP6DIP7,
	RTL9602C_DAL_ACL_VIDRANGE = 0x30,
	RTL9602C_DAL_ACL_IPRANGE,
	RTL9602C_DAL_ACL_PORTRANGE,
	RTL9602C_DAL_ACL_PKTLENRANGE,
	RTL9602C_DAL_ACL_FIELD_VALID,
	RTL9602C_DAL_ACL_EXT_PMSK,
	RTL9602C_DAL_ACL_IPRANGE_0,
	RTL9602C_DAL_ACL_IPRANGE_1,
	RTL9602C_DAL_ACL_PORTRANGE_0,	
	RTL9602C_DAL_ACL_PORTRANGE_1,	
	RTL9602C_DAL_ACL_FIELD_SELECT00 = 0x40,
	RTL9602C_DAL_ACL_FIELD_SELECT01,
	RTL9602C_DAL_ACL_FIELD_SELECT02,
	RTL9602C_DAL_ACL_FIELD_SELECT03,
	RTL9602C_DAL_ACL_FIELD_SELECT04,
	RTL9602C_DAL_ACL_FIELD_SELECT05,
	RTL9602C_DAL_ACL_FIELD_SELECT06,
	RTL9602C_DAL_ACL_FIELD_SELECT07,
	RTL9602C_DAL_ACL_FIELD_SELECT08,
	RTL9602C_DAL_ACL_FIELD_SELECT09,
	RTL9602C_DAL_ACL_FIELD_SELECT10,
	RTL9602C_DAL_ACL_FIELD_SELECT11,
	RTL9602C_DAL_ACL_FIELD_SELECT12,
	RTL9602C_DAL_ACL_FIELD_SELECT13,
	RTL9602C_DAL_ACL_FIELD_SELECT14,
	RTL9602C_DAL_ACL_FIELD_SELECT15,
	RTL9602C_DAL_ACL_TYPE_END
}rtl9602c_dal_acl_fieldTypes_t;


typedef enum rtl9602c_dal_acl_vidrange_e
{
    RTL9602C_DAL_ACL_VIDRANGE_UNUSED = 0,
    RTL9602C_DAL_ACL_VIDRANGE_CVID,
    RTL9602C_DAL_ACL_VIDRANGE_SVID,
    RTL9602C_DAL_ACL_VIDRANGE_END
}rtl9602c_dal_acl_vidrange_t;


typedef enum rtl9602c_dal_acl_iprange_e
{
    RTL9602C_DAL_ACL_IPRANGE_UNUSED = 0,
    RTL9602C_DAL_ACL_IPRANGE_IPV4_SIP,
    RTL9602C_DAL_ACL_IPRANGE_IPV4_DIP,
    RTL9602C_DAL_ACL_IPRANGE_IPV6_SIP,
    RTL9602C_DAL_ACL_IPRANGE_IPV6_DIP,
    RTL9602C_DAL_ACL_IPRANGE_END
}rtl9602c_dal_acl_iprange_t;


typedef enum rtl9602c_dal_acl_portrange_e
{
    RTL9602C_DAL_ACL_PORTRANGE_UNUSED = 0,
    RTL9602C_DAL_ACL_PORTRANGE_SPORT,
    RTL9602C_DAL_ACL_PORTRANGE_DPORT,
    RTL9602C_DAL_ACL_PORTRANGE_END
}rtl9602c_dal_acl_portrange_type_t;


typedef enum rtl9602c_dal_field_sel_e
{
    RTL9602C_DAL_FIELD_FORMAT_DEFAULT = 0,
    RTL9602C_DAL_FIELD_FORMAT_RAW,
    RTL9602C_DAL_FIELD_FORMAT_PPPOE,
    RTL9602C_DAL_FIELD_FORMAT_IPV4,
    RTL9602C_DAL_FIELD_FORMAT_ARP,
    RTL9602C_DAL_FIELD_FORMAT_IPV6,
    RTL9602C_DAL_FIELD_FORMAT_IPPAYLOAD,
    RTL9602C_DAL_FIELD_FORMAT_L4PAYLOAD,
    RTL9602C_DAL_FIELD_FORMAT_END
}rtl9602c_dal_field_sel_t;

/*clvan action*/
typedef enum rtl9602c_dal_acl_igr_cvlan_act_ctrl_e
{
    RTL9602C_DAL_ACL_IGR_CVLAN_IGR_CVLAN_ACT = 0,
    RTL9602C_DAL_ACL_IGR_CVLAN_EGR_CVLAN_ACT,
    RTL9602C_DAL_ACL_IGR_CVLAN_DS_SVID_ACT,
    RTL9602C_DAL_ACL_IGR_CVLAN_POLICING_ACT,
    RTL9602C_DAL_ACL_IGR_CVLAN_MIB_ACT,
    RTL9602C_DAL_ACL_IGR_CVLAN_1P_REMARK_ACT,
	RTL9602C_DAL_ACL_IGR_CVLAN_BW_METER_ACT,    
    RTL9602C_DAL_ACL_IGR_CVLAN_ACT_END,
} rtl9602c_dal_acl_igr_cvlan_act_ctrl_t;

/*svlan action*/
typedef enum rtl9602c_dal_acl_igr_svlan_act_ctrl_e
{
    RTL9602C_DAL_ACL_IGR_SVLAN_IGR_SVLAN_ACT = 0,
    RTL9602C_DAL_ACL_IGR_SVLAN_EGR_SVLAN_ACT,
    RTL9602C_DAL_ACL_IGR_SVLAN_US_CVID_ACT,
    RTL9602C_DAL_ACL_IGR_SVLAN_POLICING_ACT,
    RTL9602C_DAL_ACL_IGR_SVLAN_MIB_ACT,
    RTL9602C_DAL_ACL_IGR_SVLAN_1P_REMARK_ACT,
    RTL9602C_DAL_ACL_IGR_SVLAN_DSCP_REMARK_ACT,
	RTL9602C_DAL_ACL_IGR_SVLAN_BW_METER_ACT,
    RTL9602C_DAL_ACL_IGR_SVLAN_ACT_END,
} rtl9602c_dal_acl_igr_svlan_act_ctrl_t;

/*priority action*/
typedef enum rtl9602c_dal_acl_igr_pri_act_ctrl_e
{
    RTL9602C_DAL_ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT = 0,
    RTL9602C_DAL_ACL_IGR_PRI_DSCP_REMARK_ACT,
    RTL9602C_DAL_ACL_IGR_PRI_1P_REMARK_ACT,
    RTL9602C_DAL_ACL_IGR_PRI_POLICING_ACT,
    RTL9602C_DAL_ACL_IGR_PRI_MIB_ACT,
    RTL9602C_DAL_ACL_IGR_PRI_BW_METER_ACT,
    RTL9602C_DAL_ACL_IGR_PRI_ACT_END,
} rtl9602c_dal_acl_igr_pri_act_ctrl_t;

typedef enum rtl9602c_dal_acl_igr_log_act_ctrl_e
{
    RTL9602C_DAL_ACL_IGR_LOG_POLICING_ACT = 0,
    RTL9602C_DAL_ACL_IGR_LOG_MIB_ACT,
	RTL9602C_DAL_ACL_IGR_LOG_BW_METER_ACT,
	RTL9602C_DAL_ACL_IGR_LOG_1P_REMARK_ACT,
    RTL9602C_DAL_ACL_IGR_LOG_ACT_END,
} rtl9602c_dal_acl_igr_log_act_ctrl_t;

/*forward action*/
typedef enum rtl9602c_dal_acl_igr_forward_act_ctrl_e
{
    RTL9602C_DAL_ACL_IGR_FORWARD_EGRESSMASK_ACT = 0,
    RTL9602C_DAL_ACL_IGR_FORWARD_REDIRECT_ACT,
    RTL9602C_DAL_ACL_IGR_FORWARD_IGR_MIRROR_ACT,
    RTL9602C_DAL_ACL_IGR_FORWARD_TRAP_ACT,
    RTL9602C_DAL_ACL_IGR_FORWARD_ACT_END,
} rtl9602c_dal_acl_igr_forward_act_ctrl_t;

/*interrupt, GPIO action*/
typedef enum rtl9602c_acl_igr_extend_act_ctrl_e
{
    RTL9602C_DAL_ACL_IGR_EXTEND_NONE_ACT = 0,
    RTL9602C_DAL_ACL_IGR_EXTEND_SID_ACT,
    RTL9602C_DAL_ACL_IGR_EXTEND_LLID_ACT,
    RTL9602C_DAL_ACL_IGR_EXTEND_1P_REMARK_ACT,
    RTL9602C_DAL_ACL_IGR_EXTEND_ACT_END,
} rtl9602c_acl_igr_extend_act_ctrl_t;

typedef enum rtl9602c_acl_igr_route_act_ctrl_e
{
    RTL9602C_DAL_ACL_IGR_ROUTE_ROUTE_ACT = 0,
    RTL9602C_DAL_ACL_IGR_ROUTE_1P_REMARK_ACT,
    RTL9602C_DAL_ACL_IGR_ROUTE_ACT_END,
} rtl9602c_acl_igr_route_act_ctrl_t;



/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl9602c_acl_init
 * Description:
 *      Initialize ACL module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize ACL module before calling any ACL APIs.
 *      Apollo init acl mode  as ACL_IGR_RULE_MODE_0
 */
extern int32
dal_rtl9602c_acl_init(void);

/* Function Name:
 *      dal_rtl9602c_acl_template_set
 * Description:
 *      Set template of ingress ACL.
 * Input:
 *      template - Ingress ACL template
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function set ACL template.
 */
extern int32 
dal_rtl9602c_acl_template_set(rtk_acl_template_t *aclTemplate);


/* Function Name:
 *      dal_rtl9602c_acl_template_get
 * Description:
 *      Get template of ingress ACL.
 * Input:
 *      template - Ingress ACL template
 * Output:
 *      template - Ingress ACL template
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function get ACL template.
 */
extern int32 
dal_rtl9602c_acl_template_get(rtk_acl_template_t *aclTemplate);

/* Function Name:
 *      dal_rtl9602c_acl_fieldSelect_set
 * Description:
 *      Set user defined field selectors in HSB
 * Input:
 *      pFieldEntry 	- pointer of field selector entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *      System support 16 user defined field selctors.
 * 		Each selector can be enabled or disable.
 *      User can defined retrieving 16-bits in many predefiend
 * 		standard l2/l3/l4 payload.
 */
extern int32 
dal_rtl9602c_acl_fieldSelect_set(rtk_acl_field_entry_t *pFieldEntry);

/* Function Name:
 *      dal_rtl9602c_acl_fieldSelect_get
 * Description:
 *      Get user defined field selectors in HSB
 * Input:
 *      None
 * Output:
 *      pFieldEntry 	- pointer of field selector entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *      None.
 */
extern int32 
dal_rtl9602c_acl_fieldSelect_get(rtk_acl_field_entry_t *pFieldEntry);

/* Function Name:
 *      dal_rtl9602c_acl_igrRuleAction_get
 * Description:
 *      Get an ACL action from ASIC
 * Input:
 *      None.
 * Output:
 *      pAclRule     - The ACL configuration that this function will add comparison rule
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclRule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      use this API to get action control and data
 */
extern int32 
dal_rtl9602c_acl_igrRuleAction_get(rtk_acl_ingress_entry_t *pAclRule);

/* Function Name:
 *      dal_rtl9602c_acl_igrRuleAction_set
 * Description:
 *      Get an ACL action from ASIC
 * Input:
 *      pAclRule     - The ACL configuration that this function will add comparison rule
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclRule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      use this API to set action control and data
 */
extern int32 
dal_rtl9602c_acl_igrRuleAction_set(rtk_acl_ingress_entry_t *pAclRule);


/* Function Name:
 *      dal_rtl9602c_acl_igrRuleEntry_get
 * Description:
 *      Get an ACL entry from ASIC
 * Input:
 *      None.
 * Output:
 *      pAclRule     - The ACL configuration that this function will add comparison rule
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclRule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      use this API to get rule entry the field data will return in raw format
 *      raw data is return in pAclRule->field.readField
 */
extern int32 
dal_rtl9602c_acl_igrRuleEntry_get(rtk_acl_ingress_entry_t *pAclRule);


/* Function Name:
 *      dal_rtl9602c_acl_igrRuleField_add
 * Description:
 *      Add comparison rule to an ACL configuration
 * Input:
 *      pAclEntry     - The ACL configuration that this function will add comparison rule
 *      pAclField   - The comparison rule that will be added.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER    	- Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT 			- Invalid input parameters.
 * Note:
 *      This function add a comparison rule (*pAclField) to an ACL configuration (*pAclEntry).
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL
 *      comparison rules by means of linked list. Pointer pAclField will be added to linked
 *      list keeped by structure that pAclEntry points to.
 *      caller should not free (*pAclField) before dal_rtl9602c_acl_igrRuleEntry_add is called
 */
extern int32 
dal_rtl9602c_acl_igrRuleField_add(rtk_acl_ingress_entry_t *pAclRule, rtk_acl_field_t *pAclField);


/* Function Name:
 *      dal_rtl9602c_acl_igrRuleEntry_add
 * Description:
 *      Add an ACL configuration to ASIC
 * Input:
 *      pAclRule   - ACL ingress filter rule configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclrule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 *      RT_ERR_ENTRY_INDEX 						- Invalid entryIdx .
 * Note:
 *      None
 */
extern int32 
dal_rtl9602c_acl_igrRuleEntry_add(rtk_acl_ingress_entry_t *pAclRule);

/* Function Name:
 *      dal_rtl9602c_acl_igrRuleEntry_del
 * Description:
 *      Delete an ACL configuration from ASIC
 * Input:
 *      pAclrule   - ACL ingress filter rule configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_ENTRY_INDEX 						- Invalid entryIdx .
 * Note:
 *      None
 */
extern int32 
dal_rtl9602c_acl_igrRuleEntry_del(uint32 index);

/* Function Name:
 *      dal_rtl9602c_acl_igrRuleEntry_delAll
 * Description:
 *      Delete all ACL configuration from ASIC
 * Input:
 *      pAclrule   - ACL ingress filter rule configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 * Note:
 *      None
 */
extern int32 
dal_rtl9602c_acl_igrRuleEntry_delAll(void);


/* Function Name:
 *      dal_rtl9602c_acl_igrUnmatchAction_set
 * Description:
 *      Apply action to packets when no ACL configuration match
 * Input:
 *      port    - Port id.
 *      action - Action.
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
extern int32 
dal_rtl9602c_acl_igrUnmatchAction_set(rtk_port_t port, rtk_filter_unmatch_action_type_t action);

/* Function Name:
 *      dal_rtl9602c_acl_igrUnmatchAction_get
 * Description:
 *      Get action to packets when no ACL configuration match
 * Input:
 *      port    - Port id.
 * Output:
 *      pAction - Action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
extern int32 
dal_rtl9602c_acl_igrUnmatchAction_get(rtk_port_t port, rtk_filter_unmatch_action_type_t *pAction);


/* Function Name:
 *      dal_rtl9602c_acl_igrState_set
 * Description:
 *      Set state of ingress ACL.
 * Input:
 *      port    - Port id.
 *      state  - Ingress ACL state.
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function set per-port ACL filtering state.
 */
extern int32 
dal_rtl9602c_acl_igrState_set(rtk_port_t port, rtk_enable_t state);


/* Function Name:
 *      dal_rtl9602c_acl_igrState_get
 * Description:
 *      Get state of ingress ACL.
 * Input:
 *      port    - Port id.
 * Output:
 *      pState  - Ingress ACL state.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets per-port ACL filtering state.
 */
extern int32 
dal_rtl9602c_acl_igrState_get(rtk_port_t port, rtk_enable_t *pState);


/* Function Name:
 *      dal_rtl9602c_acl_ipRange_set
 * Description:
 *      Set IP Range check
 * Input:
 *      pRangeEntry - IP Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upper IP must be larger or equal than lower IP.
 */
extern int32 
dal_rtl9602c_acl_ipRange_set(rtk_acl_rangeCheck_ip_t *pRangeEntry);


/* Function Name:
 *      dal_rtl9602c_acl_ipRange_get
 * Description:
 *      Set IP Range check
 * Input:
 *      None.
 * Output:
 *      pRangeEntry - IP Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      None.
 */
extern int32 
dal_rtl9602c_acl_ipRange_get(rtk_acl_rangeCheck_ip_t *pRangeEntry);

/* Function Name:
 *      dal_rtl9602c_acl_vidRange_set
 * Description:
 *      Set VID Range check
 * Input:
 *      pRangeEntry - VLAN id Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upper Vid must be larger or equal than lowerVid.
 */
extern int32 
dal_rtl9602c_acl_vidRange_set(rtk_acl_rangeCheck_vid_t *pRangeEntry);

/* Function Name:
 *      dal_rtl9602c_acl_vidRange_get
 * Description:
 *      Get VID Range check
 * Input:
 *      None.
 * Output:
 *      pRangeEntry - VLAN id Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      None.
 */
extern int32 
dal_rtl9602c_acl_vidRange_get(rtk_acl_rangeCheck_vid_t *pRangeEntry);


/* Function Name:
 *      dal_rtl9602c_acl_portRange_set
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
 *      upper Port must be larger or equal than lowerPort.
 */
extern int32 
dal_rtl9602c_acl_portRange_set(rtk_acl_rangeCheck_l4Port_t *pRangeEntry);


/* Function Name:
 *      dal_rtl9602c_acl_portRange_get
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
dal_rtl9602c_acl_portRange_get(rtk_acl_rangeCheck_l4Port_t *pRangeEntry);


/* Function Name:
 *      dal_rtl9602c_acl_packetLengthRange_set
 * Description:
 *      Set packet length Range check
 * Input:
 *      pRangeEntry - packet length range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upper length must be larger or equal than lower length.
 */
extern int32 
dal_rtl9602c_acl_packetLengthRange_set(rtk_acl_rangeCheck_pktLength_t *pRangeEntry);


/* Function Name:
 *      dal_rtl9602c_acl_packetLengthRange_get
 * Description:
 *      Set packet length Range check
 * Input:
 *      None
 * Output:
 *      pRangeEntry - packet length range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      None.
 */
extern int32 
dal_rtl9602c_acl_packetLengthRange_get(rtk_acl_rangeCheck_pktLength_t *pRangeEntry);

/* Function Name:
 *      dal_rtl9602c_acl_igrPermitState_set
 * Description:
 *      Set permit state of ingress ACL.
 * Input:
 *      port    - Port id.
 *      state  - Ingress ACL state.
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function set action of packets when no ACL configruation matches.
 */
extern int32 
dal_rtl9602c_acl_igrPermitState_set(rtk_port_t port, rtk_enable_t state);


/* Function Name:
 *      dal_rtl9602c_acl_igrPermitState_get
 * Description:
 *      Get state of ingress ACL.
 * Input:
 *      port    - Port id.
 * Output:
 *      pState  - Ingress ACL state.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
extern int32 
dal_rtl9602c_acl_igrPermitState_get(rtk_port_t port, rtk_enable_t *pState);


/* Function Name:
 *      dal_rtl9602c_acl_dbgInfo_get
 * Description:
 *      Get ACL debug information
 * Input:
 *	  intrType: type of interrupt
 * Output:
 *      pDbgCnt: point of relative debug counter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 
dal_rtl9602c_acl_dbgInfo_get(rtk_acl_dbgCnt_t *pDbgCnt);

/* Function Name:
 *      dal_rtl9602c_acl_dbgHitReason_get
 * Description:
 *      Get ACL hit reason information
 * Input:
 *	  	None
 * Output:
 *      pDbgReason: point of relative debug reason and index
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 dal_rtl9602c_acl_dbgHitReason_get(rtk_acl_debug_reason_t *pDbgReason);

#endif /* __DAL_RTL9602C_ACL_H__ */

