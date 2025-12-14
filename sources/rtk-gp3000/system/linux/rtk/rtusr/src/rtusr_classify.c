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
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>

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
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    SETSOCKOPT(RTDRV_CLASSIFY_INIT, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_init */

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
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None.
 */
int32
rtk_classify_cfgEntry_add(rtk_classify_cfg_t *pClassifyCfg)
{
    rtdrv_classifyCfg_t   classify_cfg;
    rtk_classify_field_t  *cfField;

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyCfg), RT_ERR_NULL_POINTER);

    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    /*check field for classify rule*/
    osal_memcpy(&classify_cfg.classifyCfg, pClassifyCfg, sizeof(rtk_classify_cfg_t));
    classify_cfg.classifyCfg.field.pFieldHead = NULL;

    /* function body */
    cfField = pClassifyCfg->field.pFieldHead;
    while(cfField != NULL)
    {
        osal_memcpy(&classify_cfg.classifyField, cfField, sizeof(rtk_classify_field_t));
        SETSOCKOPT(RTDRV_CLASSIFY_FIELD_ADD, &classify_cfg, rtdrv_classifyCfg_t, 1);

        cfField = cfField->next;
    }

    SETSOCKOPT(RTDRV_CLASSIFY_CFGENTRY_ADD, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_cfgEntry_add */


/* Function Name:
 *      rtk_classify_cfgEntry_get
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
int32
rtk_classify_cfgEntry_get(rtk_classify_cfg_t *pClassifyCfg)
{
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyCfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&classify_cfg.classifyCfg, pClassifyCfg, sizeof(rtk_classify_cfg_t));
    GETSOCKOPT(RTDRV_CLASSIFY_CFGENTRY_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pClassifyCfg, &classify_cfg.classifyCfg, sizeof(rtk_classify_cfg_t));

    return RT_ERR_OK;
}   /* end of rtk_classify_cfgEntry_get */


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
 *      RT_ERR_ENTRY_INDEX      - Invalid classification index .
 * Note:
 *      None.
 */
int32
rtk_classify_cfgEntry_del(uint32 entryIdx)
{
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.entryIdx, &entryIdx, sizeof(uint32));
    SETSOCKOPT(RTDRV_CLASSIFY_CFGENTRY_DEL, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_cfgEntry_del */


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
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_NULL_POINTER     - Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT            - Invalid input parameters.
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
    rtk_classify_field_t *tmp_field;

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pClassifyField), RT_ERR_NULL_POINTER);

    /* function body */
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
}   /* end of rtk_classify_field_add */


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
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.action, &action, sizeof(rtk_classify_unmatch_action_t));
    SETSOCKOPT(RTDRV_CLASSIFY_UNMATCHACTION_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_unmatchAction_set */


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
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&classify_cfg.action, pAction, sizeof(rtk_classify_unmatch_action_t));
    GETSOCKOPT(RTDRV_CLASSIFY_UNMATCHACTION_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pAction, &classify_cfg.action, sizeof(rtk_classify_unmatch_action_t));

    return RT_ERR_OK;
}   /* end of rtk_classify_unmatchAction_get */

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
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.dsaction, &action, sizeof(rtk_classify_unmatch_action_ds_t));
    SETSOCKOPT(RTDRV_CLASSIFY_UNMATCHACTION_DS_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_unmatchAction_set */


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
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_CLASSIFY_UNMATCHACTION_DS_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pAction, &classify_cfg.dsaction, sizeof(rtk_classify_unmatch_action_ds_t));

    return RT_ERR_OK;
}   /* end of rtk_classify_unmatchAction_get */

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
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.rangeEntry, pRangeEntry, sizeof(rtk_classify_rangeCheck_l4Port_t));
    SETSOCKOPT(RTDRV_CLASSIFY_PORTRANGE_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_portRange_set */


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
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRangeEntry), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&classify_cfg.rangeEntry, pRangeEntry, sizeof(rtk_classify_rangeCheck_l4Port_t));
    GETSOCKOPT(RTDRV_CLASSIFY_PORTRANGE_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pRangeEntry, &classify_cfg.rangeEntry, sizeof(rtk_classify_rangeCheck_l4Port_t));

    return RT_ERR_OK;
}   /* end of rtk_classify_portRange_get */


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
rtk_classify_ipRange_set(rtk_classify_rangeCheck_ip_t *pIpRangeEntry)
{
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpRangeEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.ipRangeEntry, pIpRangeEntry, sizeof(rtk_classify_rangeCheck_ip_t));
    SETSOCKOPT(RTDRV_CLASSIFY_IPRANGE_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_ipRange_set */


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
rtk_classify_ipRange_get(rtk_classify_rangeCheck_ip_t *pIpRangeEntry)
{
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpRangeEntry), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&classify_cfg.ipRangeEntry, pIpRangeEntry, sizeof(rtk_classify_rangeCheck_ip_t));
    GETSOCKOPT(RTDRV_CLASSIFY_IPRANGE_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pIpRangeEntry, &classify_cfg.ipRangeEntry, sizeof(rtk_classify_rangeCheck_ip_t));

    return RT_ERR_OK;
}   /* end of rtk_classify_ipRange_get */

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
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&classify_cfg.cfSel, &cfSel, sizeof(rtk_classify_cf_sel_t));
    SETSOCKOPT(RTDRV_CLASSIFY_CFSEL_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_cfSel_set */

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
int32
rtk_classify_cfSel_get(rtk_port_t port, rtk_classify_cf_sel_t *pCfSel)
{
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCfSel), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&classify_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_CLASSIFY_CFSEL_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pCfSel, &classify_cfg.cfSel, sizeof(rtk_classify_cf_sel_t));

    return RT_ERR_OK;
}   /* end of rtk_classify_cfSel_get */

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
int32
rtk_classify_cfPri2Dscp_set(rtk_pri_t pri, rtk_dscp_t dscp)
{
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.pri, &pri, sizeof(rtk_pri_t));
    osal_memcpy(&classify_cfg.dscp, &dscp, sizeof(rtk_dscp_t));
    SETSOCKOPT(RTDRV_CLASSIFY_CFPRI2DSCP_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_cfPri2Dscp_set */

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
int32
rtk_classify_cfPri2Dscp_get(rtk_pri_t pri, rtk_dscp_t *pDscp)
{
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&classify_cfg.pri, &pri, sizeof(rtk_pri_t));
    GETSOCKOPT(RTDRV_CLASSIFY_CFPRI2DSCP_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pDscp, &classify_cfg.dscp, sizeof(rtk_dscp_t));

    return RT_ERR_OK;
}   /* end of rtk_classify_cfPri2Dscp_get */


/* Function Name:
 *      rtk_classify_permit_sel_get
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
rtk_classify_permit_sel_get( rtk_classify_permit_sel_t *permitSel)
{
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == permitSel), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&classify_cfg.permitSel, permitSel, sizeof(rtk_classify_permit_sel_t));
    GETSOCKOPT(RTDRV_CLASSIFY_PERMIT_SEL_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(permitSel, &classify_cfg.permitSel, sizeof(rtk_classify_permit_sel_t));

    return RT_ERR_OK;
}   /* end of rtk_classify_permit_sel_get */

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
int32
rtk_classify_permit_sel_set( rtk_classify_permit_sel_t permitSel)
{
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.permitSel, &permitSel, sizeof(rtk_classify_permit_sel_t));
    SETSOCKOPT(RTDRV_CLASSIFY_PERMIT_SEL_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_permit_sel_set */

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
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.prior, &prior, sizeof(rtk_classify_us_1premark_prior_t));
    SETSOCKOPT(RTDRV_CLASSIFY_US1PREMARK_PRIOR_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPrior), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&classify_cfg.prior, pPrior, sizeof(rtk_classify_us_1premark_prior_t));
    GETSOCKOPT(RTDRV_CLASSIFY_US1PREMARK_PRIOR_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pPrior, &classify_cfg.prior, sizeof(rtk_classify_us_1premark_prior_t));

    return RT_ERR_OK;
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
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.p0TemplateCfg, pP0TemplateCfg, sizeof(rtk_classify_template_cfg_pattern0_t));
    SETSOCKOPT(RTDRV_CLASSIFY_TEMPLATECFGPATTERN0_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pP0TemplateCfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&classify_cfg.p0TemplateCfg, pP0TemplateCfg, sizeof(rtk_classify_template_cfg_pattern0_t));
    GETSOCKOPT(RTDRV_CLASSIFY_TEMPLATECFGPATTERN0_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pP0TemplateCfg, &classify_cfg.p0TemplateCfg, sizeof(rtk_classify_template_cfg_pattern0_t));

    return RT_ERR_OK;
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
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.entryNum, &entryNum, sizeof(uint32));
    SETSOCKOPT(RTDRV_CLASSIFY_ENTRYNUMPATTERN1_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
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
rtk_classify_entryNumPattern1_get( uint32 *pEntryNum)
{
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntryNum), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_CLASSIFY_ENTRYNUMPATTERN1_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pEntryNum, &classify_cfg.entryNum, sizeof(uint32));

    return RT_ERR_OK;
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
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.defaultWanIf, pDefaultWanIf, sizeof(rtk_classify_default_wanIf_t));
    SETSOCKOPT(RTDRV_CLASSIFY_DEFAULTWANIF_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDefaultWanIf), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_CLASSIFY_DEFAULTWANIF_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pDefaultWanIf, &classify_cfg.defaultWanIf, sizeof(rtk_classify_default_wanIf_t));

    return RT_ERR_OK;
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
    rtdrv_classifyCfg_t classify_cfg;

    /* function body */
    osal_memset(&classify_cfg, 0x0, sizeof(rtdrv_classifyCfg_t));
    osal_memcpy(&classify_cfg.trapPri, &trap_pri, sizeof(rtk_pri_t));
    SETSOCKOPT(RTDRV_CLASSIFY_TRAPPRI_SET, &classify_cfg, rtdrv_classifyCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_classify_trapPri_set */

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
    rtdrv_classifyCfg_t classify_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTrap_pri), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_CLASSIFY_TRAPPRI_GET, &classify_cfg, rtdrv_classifyCfg_t, 1);
    osal_memcpy(pTrap_pri, &classify_cfg.trapPri, sizeof(rtk_pri_t));

    return RT_ERR_OK;
}   /* end of rtk_classify_trapPri_get */
