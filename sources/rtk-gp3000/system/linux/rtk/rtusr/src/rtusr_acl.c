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
 * Purpose : Definition of ACL API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) ACL rule action configure and modification
 *
 */
 

/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <rtk/acl.h>
#include <osal/print.h>

/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_acl_init
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
int32
rtk_acl_init(void)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));
    
    SETSOCKOPT(RTDRV_ACL_INIT, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_init */

/* Function Name:
 *      rtk_acl_template_set
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
int32 
rtk_acl_template_set(rtk_acl_template_t *pAclTemplate)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pAclTemplate), RT_ERR_NULL_POINTER);

    osal_memcpy(&acl_cfg.aclTemplate, pAclTemplate, sizeof(rtk_acl_template_t));
    SETSOCKOPT(RTDRV_ACL_TEMPLATE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_template_set */


/* Function Name:
 *      rtk_acl_template_get
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
int32 
rtk_acl_template_get(rtk_acl_template_t *pAclTemplate)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pAclTemplate), RT_ERR_NULL_POINTER);
    osal_memcpy(&acl_cfg.aclTemplate, pAclTemplate, sizeof(rtk_acl_template_t));
    GETSOCKOPT(RTDRV_ACL_TEMPLATE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pAclTemplate, &acl_cfg.aclTemplate, sizeof(rtk_acl_template_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_template_get */

/* Function Name:
 *      rtk_acl_fieldSelect_set
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
int32 
rtk_acl_fieldSelect_set(rtk_acl_field_entry_t *pFieldEntry)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pFieldEntry), RT_ERR_NULL_POINTER);

    osal_memcpy(&acl_cfg.fieldEntry, pFieldEntry, sizeof(rtk_acl_field_entry_t));
    SETSOCKOPT(RTDRV_ACL_FIELDSELECT_SET, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_fieldSelect_set */

/* Function Name:
 *      rtk_acl_fieldSelect_get
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
int32 
rtk_acl_fieldSelect_get(rtk_acl_field_entry_t *pFieldEntry)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pFieldEntry), RT_ERR_NULL_POINTER);

    osal_memcpy(&acl_cfg.fieldEntry, pFieldEntry, sizeof(rtk_acl_field_entry_t));
    GETSOCKOPT(RTDRV_ACL_FIELDSELECT_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pFieldEntry, &acl_cfg.fieldEntry, sizeof(rtk_acl_field_entry_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_fieldSelect_get */


/* Function Name:
 *      rtk_acl_igrRuleEntry_get
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
int32 
rtk_acl_igrRuleEntry_get(rtk_acl_ingress_entry_t *pAclRule)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);

    osal_memcpy(&acl_cfg.aclRule, pAclRule, sizeof(rtk_acl_ingress_entry_t));
    GETSOCKOPT(RTDRV_ACL_IGRRULEENTRY_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pAclRule, &acl_cfg.aclRule, sizeof(rtk_acl_ingress_entry_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_igrRuleEntry_get */


/* Function Name:
 *      rtk_acl_igrRuleField_add
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
 *      caller should not free (*pAclField) before rtk_acl_igrRuleEntry_add is called
 */
int32 
rtk_acl_igrRuleField_add(rtk_acl_ingress_entry_t *pAclRule, rtk_acl_field_t *pAclField)
{
	rtk_acl_field_t *tailPtr;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAclField), RT_ERR_NULL_POINTER);

    if(NULL == pAclRule->pFieldHead)
    {
        pAclRule->pFieldHead = pAclField;
		pAclField->next = NULL;
    }
    else
    {
        if ( pAclRule->pFieldHead->next == NULL)
        {
             pAclRule->pFieldHead->next = pAclField;
        }
        else
        {
            tailPtr = pAclRule->pFieldHead->next;
            while( tailPtr->next != NULL)
            {
                tailPtr = tailPtr->next;
            }
            tailPtr->next = pAclField;
			pAclField->next = NULL;
        }
    }

	return RT_ERR_OK;
}   /* end of rtk_acl_igrRuleField_add */


/* Function Name:
 *      rtk_acl_igrRuleEntry_add
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
int32 
rtk_acl_igrRuleEntry_add(rtk_acl_ingress_entry_t *pAclRule)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    rtk_acl_field_t  *aclField;
	rtk_acl_field_t  *aclFieldNext;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));
	
    RT_PARAM_CHK((NULL == pAclRule), RT_ERR_NULL_POINTER);

	/*check field for ACL rule*/
	osal_memcpy(&acl_cfg.aclRule, pAclRule, sizeof(rtk_acl_ingress_entry_t));
	acl_cfg.aclRule.pFieldHead = NULL;


    /*check field for ACL rule*/
    if(pAclRule->pFieldHead != NULL)
    {
        aclField = (rtk_acl_field_t*)pAclRule->pFieldHead;
		aclFieldNext = aclField->next;
    	osal_memcpy(&acl_cfg.aclField, aclField, sizeof(rtk_acl_field_t));
		SETSOCKOPT(RTDRV_ACL_IGRRULEFIELD_ADD, &acl_cfg, rtdrv_aclCfg_t, 1);

        aclField = aclFieldNext;

		while(aclField != NULL)
        {
        	aclFieldNext = aclField->next;
	    	osal_memcpy(&acl_cfg.aclField, aclField, sizeof(rtk_acl_field_t));
			SETSOCKOPT(RTDRV_ACL_IGRRULEFIELD_ADD, &acl_cfg, rtdrv_aclCfg_t, 1);

            aclField = aclFieldNext;
        }
    }
    /* function body */
    SETSOCKOPT(RTDRV_ACL_IGRRULEENTRY_ADD, &acl_cfg, rtdrv_aclCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_acl_igrRuleEntry_add */

/* Function Name:
 *      rtk_acl_igrRuleEntry_del
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
int32 
rtk_acl_igrRuleEntry_del(uint32 index)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    osal_memcpy(&acl_cfg.index, &index, sizeof(uint32));
    SETSOCKOPT(RTDRV_ACL_IGRRULEENTRY_DEL, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_igrRuleEntry_del */

/* Function Name:
 *      rtk_acl_igrRuleEntry_delAll
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
int32 
rtk_acl_igrRuleEntry_delAll(void)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    SETSOCKOPT(RTDRV_ACL_IGRRULEENTRY_DELALL, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_igrRuleEntry_delAll */


/* Function Name:
 *      rtk_acl_igrUnmatchAction_set
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
int32 
rtk_acl_igrUnmatchAction_set(rtk_port_t port, rtk_filter_unmatch_action_type_t action)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    osal_memcpy(&acl_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&acl_cfg.action, &action, sizeof(rtk_filter_unmatch_action_type_t));
    SETSOCKOPT(RTDRV_ACL_IGRUNMATCHACTION_SET, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_igrUnmatchAction_set */

/* Function Name:
 *      rtk_acl_igrUnmatchAction_get
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
int32 
rtk_acl_igrUnmatchAction_get(rtk_port_t port, rtk_filter_unmatch_action_type_t *pAction)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    osal_memcpy(&acl_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_ACL_IGRUNMATCHACTION_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pAction, &acl_cfg.action, sizeof(rtk_filter_unmatch_action_type_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_igrUnmatchAction_get */


/* Function Name:
 *      rtk_acl_igrState_set
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
 *      This function gets action of packets when no ACL configruation matches.
 */
int32 
rtk_acl_igrState_set(rtk_port_t port, rtk_enable_t state)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    osal_memcpy(&acl_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&acl_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_ACL_IGRSTATE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_igrState_set */


/* Function Name:
 *      rtk_acl_igrState_get
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
int32 
rtk_acl_igrState_get(rtk_port_t port, rtk_enable_t *pState)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);
    osal_memcpy(&acl_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_ACL_IGRSTATE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pState, &acl_cfg.state, sizeof(rtk_enable_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_igrState_get */


/* Function Name:
 *      rtk_acl_ipRange_set
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
 *      upper Ip must be larger or equal than lowerIp.
 */
int32 
rtk_acl_ipRange_set(rtk_acl_rangeCheck_ip_t *pIpRangeEntry)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pIpRangeEntry), RT_ERR_NULL_POINTER);
    osal_memcpy(&acl_cfg.ipRangeEntry, pIpRangeEntry, sizeof(rtk_acl_rangeCheck_ip_t));
    SETSOCKOPT(RTDRV_ACL_IPRANGE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_ipRange_set */


/* Function Name:
 *      rtk_acl_ipRange_get
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
int32 
rtk_acl_ipRange_get(rtk_acl_rangeCheck_ip_t *pIpRangeEntry)
{
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pIpRangeEntry), RT_ERR_NULL_POINTER);
    /* function body */

    osal_memcpy(&acl_cfg.ipRangeEntry, pIpRangeEntry, sizeof(rtk_acl_rangeCheck_ip_t));
    GETSOCKOPT(RTDRV_ACL_IPRANGE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pIpRangeEntry, &acl_cfg.ipRangeEntry, sizeof(rtk_acl_rangeCheck_ip_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_ipRange_get */

/* Function Name:
 *      rtk_acl_vidRange_set
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
int32 
rtk_acl_vidRange_set(rtk_acl_rangeCheck_vid_t *pVidRangeEntry)
{
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pVidRangeEntry), RT_ERR_NULL_POINTER);
    /* function body */

    osal_memcpy(&acl_cfg.vidRangeEntry, pVidRangeEntry, sizeof(rtk_acl_rangeCheck_vid_t));
    SETSOCKOPT(RTDRV_ACL_VIDRANGE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_vidRange_set */

/* Function Name:
 *      rtk_acl_vidRange_get
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
int32 
rtk_acl_vidRange_get(rtk_acl_rangeCheck_vid_t *pVidRangeEntry)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pVidRangeEntry), RT_ERR_NULL_POINTER);

    osal_memcpy(&acl_cfg.vidRangeEntry, pVidRangeEntry, sizeof(rtk_acl_rangeCheck_vid_t));
    GETSOCKOPT(RTDRV_ACL_VIDRANGE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pVidRangeEntry, &acl_cfg.vidRangeEntry, sizeof(rtk_acl_rangeCheck_vid_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_vidRange_get */


/* Function Name:
 *      rtk_acl_portRange_set
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
int32 
rtk_acl_portRange_set(rtk_acl_rangeCheck_l4Port_t *pL4PortRangeEntry)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pL4PortRangeEntry), RT_ERR_NULL_POINTER);
    osal_memcpy(&acl_cfg.l4PortRangeEntry, pL4PortRangeEntry, sizeof(rtk_acl_rangeCheck_l4Port_t));
    SETSOCKOPT(RTDRV_ACL_PORTRANGE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_portRange_set */


/* Function Name:
 *      rtk_acl_portRange_get
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
int32 
rtk_acl_portRange_get(rtk_acl_rangeCheck_l4Port_t *pL4PortRangeEntry)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pL4PortRangeEntry), RT_ERR_NULL_POINTER);

    osal_memcpy(&acl_cfg.l4PortRangeEntry, pL4PortRangeEntry, sizeof(rtk_acl_rangeCheck_l4Port_t));
    GETSOCKOPT(RTDRV_ACL_PORTRANGE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pL4PortRangeEntry, &acl_cfg.l4PortRangeEntry, sizeof(rtk_acl_rangeCheck_l4Port_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_portRange_get */


/* Function Name:
 *      rtk_acl_packetLengthRange_set
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
int32 
rtk_acl_packetLengthRange_set(rtk_acl_rangeCheck_pktLength_t *pPktLenRangeEntry)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pPktLenRangeEntry), RT_ERR_NULL_POINTER);
    osal_memcpy(&acl_cfg.pktLenRangeEntry, pPktLenRangeEntry, sizeof(rtk_acl_rangeCheck_pktLength_t));
    SETSOCKOPT(RTDRV_ACL_PACKETLENGTHRANGE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_packetLengthRange_set */


/* Function Name:
 *      rtk_acl_packetLengthRange_get
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
int32 
rtk_acl_packetLengthRange_get(rtk_acl_rangeCheck_pktLength_t *pPktLenRangeEntry)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pPktLenRangeEntry), RT_ERR_NULL_POINTER);
    osal_memcpy(&acl_cfg.pktLenRangeEntry, pPktLenRangeEntry, sizeof(rtk_acl_rangeCheck_pktLength_t));
    GETSOCKOPT(RTDRV_ACL_PACKETLENGTHRANGE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pPktLenRangeEntry, &acl_cfg.pktLenRangeEntry, sizeof(rtk_acl_rangeCheck_pktLength_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_packetLengthRange_get */


/* Function Name:
 *      rtk_acl_igrRuleMode_set
 * Description:
 *      Set ingress ACL rule mode
 * Input:
 *      mode - ingress ACL rule mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Input error
 * Note:
 *          - ACL_IGR_RULE_MODE_0, 64  rules, the size each rule is 16x8 bits 
 *          - ACL_IGR_RULE_MODE_1, 128 rules, 
 *          -               the size each rule is 16x4 bits(entry 0~63)
 *          -               the size each rule is 16x3 bits(entry 64~127) 
 */
int32 
rtk_acl_igrRuleMode_set(rtk_acl_igr_rule_mode_t mode)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    osal_memcpy(&acl_cfg.mode, &mode, sizeof(rtk_acl_igr_rule_mode_t));
    SETSOCKOPT(RTDRV_ACL_IGRRULEMODE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_igrRuleMode_set */



/* Function Name:
 *      rtk_acl_igrRuleMode_get
 * Description:
 *      Get ingress ACL rule mode
 * Input:
 *      None
 * Output:
 *      pMode - ingress ACL rule mode
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Input error
 * Note:
 *          - ACL_IGR_RULE_MODE_0, 64  rules, the size each rule is 16x8 bits 
 *          - ACL_IGR_RULE_MODE_1, 128 rules, 
 *          -               the size of each rule is 16x4 bits(entry 0~63)
 *          -               the size of each rule is 16x3 bits(entry 64~127) 
 *          Mode chaged all template/rule will be cleared
 */
int32 
rtk_acl_igrRuleMode_get(rtk_acl_igr_rule_mode_t *pMode)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    GETSOCKOPT(RTDRV_ACL_IGRRULEMODE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pMode, &acl_cfg.mode, sizeof(rtk_acl_igr_rule_mode_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_igrRuleMode_get */

/* Function Name:
 *      rtk_acl_igrPermitState_set
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
int32 
rtk_acl_igrPermitState_set(rtk_port_t port, rtk_enable_t state)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    osal_memcpy(&acl_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&acl_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_ACL_IGRPERMITSTATE_SET, &acl_cfg, rtdrv_aclCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_acl_igrPermitState_set */


/* Function Name:
 *      rtk_acl_igrPermitState_get
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
int32 
rtk_acl_igrPermitState_get(rtk_port_t port, rtk_enable_t *pState)
{
    /* function body */
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);
    osal_memcpy(&acl_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_ACL_IGRPERMITSTATE_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pState, &acl_cfg.state, sizeof(rtk_enable_t));
    return RT_ERR_OK;
}   /* end of rtk_acl_igrPermitState_get */

/* Function Name:
 *      rtk_acl_dbgInfo_get
 * Description:
 *      Get EPON debug information
 * Input:
 *	  	None
 * Output:
 *      pDbgCnt: point of relative debug counter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_acl_dbgInfo_get(rtk_acl_dbgCnt_t *pDbgCnt)
{
    rtdrv_aclCfg_t acl_cfg;
    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pDbgCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&acl_cfg.dbgCnt, 0x0, sizeof(rtk_acl_dbgCnt_t));
    GETSOCKOPT(RTDRV_ACL_DBGINFO_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pDbgCnt, &acl_cfg.dbgCnt, sizeof(rtk_acl_dbgCnt_t));

    return RT_ERR_OK;
}   /* end of rtk_acl_dbgInfo_get */


/* Function Name:
 *      rtk_acl_dbgHitReason_get
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
int32 rtk_acl_dbgHitReason_get(rtk_acl_debug_reason_t *pDbgReason)
{
    rtdrv_aclCfg_t acl_cfg;

    osal_memset(&acl_cfg, 0x0, sizeof(rtdrv_aclCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pDbgReason), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&acl_cfg.hitReason, 0x0, sizeof(rtk_acl_debug_reason_t));
    GETSOCKOPT(RTDRV_ACL_HITREASON_GET, &acl_cfg, rtdrv_aclCfg_t, 1);
    osal_memcpy(pDbgReason, &acl_cfg.hitReason, sizeof(rtk_acl_debug_reason_t));

    return RT_ERR_OK;
}   /* end of rtk_acl_dbgHitReason_get */


