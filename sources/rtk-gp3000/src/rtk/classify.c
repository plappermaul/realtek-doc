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
#include <common/rt_type.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
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
int32
rtk_classify_init(void)
{
    int32   ret;
	 /* function body */
    if (NULL == RT_MAPPER->classify_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_init */

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
int32
rtk_classify_cfgEntry_add(rtk_classify_cfg_t *pClassifyCfg)
{
    int32   ret;
	
	if (NULL == RT_MAPPER->classify_cfgEntry_add)
		   return RT_ERR_DRIVER_NOT_FOUND;
	
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cfgEntry_add( pClassifyCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_cfgEntry_add */


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
int32
rtk_classify_cfgEntry_get(rtk_classify_cfg_t *pClassifyCfg)
{
    int32   ret;
	
	if (NULL == RT_MAPPER->classify_cfgEntry_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cfgEntry_get( pClassifyCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_cfgEntry_get */


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
int32
rtk_classify_cfgEntry_del(uint32 entryIdx)
{
    int32   ret;
	
	if (NULL == RT_MAPPER->classify_cfgEntry_del)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cfgEntry_del( entryIdx);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_cfgEntry_del */


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
int32
rtk_classify_field_add(rtk_classify_cfg_t *pClassifyEntry, rtk_classify_field_t *pClassifyField)
{
    int32   ret;
	if (NULL == RT_MAPPER->classify_field_add)
	   return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_field_add( pClassifyEntry, pClassifyField);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_field_add */


/* Function Name:
 *      rtk_classify_unmatchAction_set
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
rtk_classify_unmatchAction_set(rtk_classify_unmatch_action_t action)
{
    int32   ret;
	if (NULL == RT_MAPPER->classify_unmatchAction_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_unmatchAction_set( action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_unmatchAction_set */


/* Function Name:
 *      rtk_classify_unmatchAction_get
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
rtk_classify_unmatchAction_get(rtk_classify_unmatch_action_t *pAction)
{
    int32   ret;
	if (NULL == RT_MAPPER->classify_unmatchAction_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_unmatchAction_get( pAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_unmatchAction_get */

/* Function Name:
 *      rtk_classify_unmatchAction_ds_set
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
rtk_classify_unmatchAction_ds_set(rtk_classify_unmatch_action_ds_t action)
{
    int32   ret;
	if (NULL == RT_MAPPER->classify_unmatchAction_ds_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_unmatchAction_ds_set( action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_unmatchAction_ds_set */


/* Function Name:
 *      rtk_classify_unmatchAction_ds_get
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
rtk_classify_unmatchAction_ds_get(rtk_classify_unmatch_action_ds_t *pAction)
{
    int32   ret;
	if (NULL == RT_MAPPER->classify_unmatchAction_ds_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_unmatchAction_ds_get( pAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_unmatchAction_ds_get */


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
int32
rtk_classify_portRange_set(rtk_classify_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_portRange_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_portRange_set( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_portRange_set */


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
int32
rtk_classify_portRange_get(rtk_classify_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_portRange_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_portRange_get( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_portRange_get */


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
int32
rtk_classify_ipRange_set(rtk_classify_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_ipRange_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_ipRange_set( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_ipRange_set */


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
int32
rtk_classify_ipRange_get(rtk_classify_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_ipRange_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_ipRange_get( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_ipRange_get */

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
int32
rtk_classify_cfSel_set(rtk_port_t port, rtk_classify_cf_sel_t cfSel)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_cf_sel_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cf_sel_set( port, cfSel);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_cf_sel_set */

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
 * Note:
 *      Only accept pon port and RGMII port.
 *      This function is not supported in Test chip.
 */
int32
rtk_classify_cfSel_get(rtk_port_t port, rtk_classify_cf_sel_t *pCfSel)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_cf_sel_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cf_sel_get( port, pCfSel);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_cf_sel_get */

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
rtk_classify_cfPri2Dscp_set(rtk_pri_t pri, rtk_dscp_t dscp)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_cfPri2Dscp_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cfPri2Dscp_set( pri, dscp);
    RTK_API_UNLOCK();
    return ret;
}

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
rtk_classify_cfPri2Dscp_get(rtk_pri_t pri, rtk_dscp_t *pDscp)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_cfPri2Dscp_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cfPri2Dscp_get( pri, pDscp);
    RTK_API_UNLOCK();
    return ret;
}


/* Function Name:
 *      rtk_classify_permit_sel_get
 * Description:
 *      Set classification permit selection, from 0 to 511 or from 64 to 511
 * Input:
 *      PpermitSel   -point of CF permit selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *      Only accept from 0 to 511 or from 64 to 511
 */
extern int32
rtk_classify_permit_sel_get( rtk_classify_permit_sel_t *PpermitSel)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_permit_sel_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_permit_sel_get( PpermitSel);
    RTK_API_UNLOCK();
    return ret;
}

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
rtk_classify_permit_sel_set( rtk_classify_permit_sel_t permitSel)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_permit_sel_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_permit_sel_set( permitSel);
    RTK_API_UNLOCK();
    return ret;
}

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
int32
rtk_classify_us1pRemarkPrior_set( rtk_classify_us_1premark_prior_t prior)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_us1pRemarkPrior_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_us1pRemarkPrior_set( prior);
    RTK_API_UNLOCK();
    return ret;
} /* end of dal_apollomp_classify_us1pRemarkPrior_set */


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
int32
rtk_classify_us1pRemarkPrior_get( rtk_classify_us_1premark_prior_t *pPrior)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_us1pRemarkPrior_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_us1pRemarkPrior_get( pPrior);
    RTK_API_UNLOCK();
    return ret;
} /* end of dal_apollomp_classify_us1pRemarkPrior_get */


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
int32
rtk_classify_templateCfgPattern0_set( rtk_classify_template_cfg_pattern0_t *pP0TemplateCfg)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_templateCfgPattern0_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_templateCfgPattern0_set(pP0TemplateCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_templateCfgPattern0_set */


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
int32
rtk_classify_templateCfgPattern0_get( rtk_classify_template_cfg_pattern0_t *pP0TemplateCfg)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_templateCfgPattern0_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_templateCfgPattern0_get(pP0TemplateCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_templateCfgPattern0_get */


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
int32
rtk_classify_entryNumPattern1_set( uint32 entryNum)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_entryNumPattern1_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_entryNumPattern1_set(entryNum);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_entryNumPattern1_set */


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
int32
rtk_classify_entryNumPattern1_get( uint32 *entryNum)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_entryNumPattern1_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_entryNumPattern1_get(entryNum);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_entryNumPattern1_get */


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
int32
rtk_classify_defaultWanIf_set( rtk_classify_default_wanIf_t *pDefaultWanIf)
{
    int32   ret;

    if (NULL == RT_MAPPER->classify_defaultWanIf_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_defaultWanIf_set(pDefaultWanIf);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_defaultWanIf_set */

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
int32
rtk_classify_defaultWanIf_get( rtk_classify_default_wanIf_t *pDefaultWanIf)
{
    int32   ret;

    if (NULL == RT_MAPPER->classify_defaultWanIf_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_defaultWanIf_get(pDefaultWanIf);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_defaultWanIf_get */

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
int32
rtk_classify_trapPri_set(rtk_pri_t trap_pri)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_trapPri_set)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_trapPri_set(trap_pri);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_trapPri_set */

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
int32
rtk_classify_trapPri_get(rtk_pri_t *pTrap_pri)
{
    int32   ret;

	if (NULL == RT_MAPPER->classify_trapPri_get)
		   return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->classify_trapPri_get(pTrap_pri);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_trapPri_get */

