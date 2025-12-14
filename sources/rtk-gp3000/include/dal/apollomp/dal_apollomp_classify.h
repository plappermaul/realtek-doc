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

#ifndef __DAL_APOLLOMP_CLASSIFY_H__
#define __DAL_APOLLOMP_CLASSIFY_H__


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
extern int32
dal_apollomp_classify_init(void);

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
extern int32
dal_apollomp_classify_cfgEntry_add(rtk_classify_cfg_t *pClassifyCfg);


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
extern int32
dal_apollomp_classify_cfgEntry_get(rtk_classify_cfg_t *pClassifyCfg);


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
extern int32
dal_apollomp_classify_cfgEntry_del(uint32 entryIdx);


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
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL
 *      comparison rules by means of linked list. Pointer pAclField will be added to linked
 *      list keeped by structure that pAclEntry points to.
 *      - caller should not free (*pClassifyField) before dal_apollomp_classify_cfgEntry_add is called
 */
extern int32
dal_apollomp_classify_field_add(rtk_classify_cfg_t *pClassifyEntry, rtk_classify_field_t *pClassifyField);


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
extern int32
dal_apollomp_classify_unmatchAction_set(rtk_classify_unmatch_action_t action);


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
extern int32
dal_apollomp_classify_unmatchAction_get(rtk_classify_unmatch_action_t *pAction);


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
 *      upperPort must be larger or equal than lowerPort.
 */
extern int32
dal_apollomp_classify_portRange_set(rtk_classify_rangeCheck_l4Port_t *pRangeEntry);


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
 *      None.
 */
extern int32
dal_apollomp_classify_portRange_get(rtk_classify_rangeCheck_l4Port_t *pRangeEntry);


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
 *      upperIp must be larger or equal than lowerIp.
 */
extern int32
dal_apollomp_classify_ipRange_set(rtk_classify_rangeCheck_ip_t *pRangeEntry);


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
 *      None.
 */
extern int32
dal_apollomp_classify_ipRange_get(rtk_classify_rangeCheck_ip_t *pRangeEntry);

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
 */
extern int32
dal_apollomp_classify_cf_sel_set(rtk_port_t port, rtk_classify_cf_sel_t cfSel);

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
 */
extern int32
dal_apollomp_classify_cf_sel_get(rtk_port_t port, rtk_classify_cf_sel_t *pCfSel);

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
dal_apollomp_classify_cfPri2Dscp_set(rtk_pri_t pri, rtk_dscp_t dscp);

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
dal_apollomp_classify_cfPri2Dscp_get(rtk_pri_t pri, rtk_dscp_t *pDscp);

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
extern int32
dal_apollomp_classify_permit_sel_get( rtk_classify_permit_sel_t *permitSel);

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
extern int32
dal_apollomp_classify_permit_sel_set( rtk_classify_permit_sel_t permitSel);

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
dal_apollomp_classify_us1pRemarkPrior_set( rtk_classify_us_1premark_prior_t prior);

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
dal_apollomp_classify_us1pRemarkPrior_get( rtk_classify_us_1premark_prior_t *pPrior);

#endif /* __DAL_APOLLOMP_CLASSIFY_H__ */

