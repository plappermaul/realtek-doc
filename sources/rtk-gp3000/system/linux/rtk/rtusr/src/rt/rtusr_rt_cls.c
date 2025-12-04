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

/*
 * Symbol Definition                                            
 */



/*
 * Function Declaration
 */

/* Module Name    : Classify rule     */
/* Sub-module Name: Add/delete classify rule */

/* Function Name:
 *      rt_cls_init
 * Description:
 *      Initial RT cls
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
int32 rt_cls_init(void)
{
    rtdrv_rt_clsCfg_t rt_cls_cfg;
    memset(&rt_cls_cfg, 0x0, sizeof(rtdrv_rt_clsCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_RT_CLS_INIT, &rt_cls_cfg, rtdrv_rt_clsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_cls_init */

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
int32 rt_cls_rule_add(rt_cls_rule_t *pClsRule)
{
    rtdrv_rt_clsCfg_t rt_cls_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pClsRule), RT_ERR_NULL_POINTER);

    memset(&rt_cls_cfg, 0x0, sizeof(rtdrv_rt_clsCfg_t));

    /* function body */
    osal_memcpy(&rt_cls_cfg.clsRule, pClsRule, sizeof(rt_cls_rule_t));
    SETSOCKOPT(RTDRV_RT_CLS_RULE_ADD, &rt_cls_cfg, rtdrv_rt_clsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_cls_rule_add */

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
int32 rt_cls_rule_delete(uint32 index)
{
    rtdrv_rt_clsCfg_t rt_cls_cfg;

    memset(&rt_cls_cfg, 0x0, sizeof(rtdrv_rt_clsCfg_t));
    /* function body */
    osal_memcpy(&rt_cls_cfg.index, &index, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_CLS_RULE_DELETE, &rt_cls_cfg, rtdrv_rt_clsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_cls_rule_delete */

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
int32 rt_cls_rule_get(rt_cls_get_op_t op, uint32 index, rt_cls_dir_t dir, rt_cls_rule_info_t *pClsRuleInfo)
{
    rtdrv_rt_clsCfg_t rt_cls_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pClsRuleInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_cls_cfg.op, &op, sizeof(rt_cls_get_op_t));
    osal_memcpy(&rt_cls_cfg.index, &index, sizeof(uint32));
    osal_memcpy(&rt_cls_cfg.dir, &dir, sizeof(rt_cls_dir_t));
    GETSOCKOPT(RTDRV_RT_CLS_RULE_GET, &rt_cls_cfg, rtdrv_rt_clsCfg_t, 1);
    osal_memcpy(pClsRuleInfo, &rt_cls_cfg.clsRuleInfo, sizeof(rt_cls_rule_info_t));

    return RT_ERR_OK;
}   /* end of rt_cls_rule_get */


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
int32 rt_cls_extend_rule_add(rt_cls_extend_rule_t *pExtClsRule)
{
    rtdrv_rt_clsCfg_t rt_cls_cfg;

    memset(&rt_cls_cfg, 0x0, sizeof(rtdrv_rt_clsCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pExtClsRule), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_cls_cfg.extClsRule, pExtClsRule, sizeof(rt_cls_extend_rule_t));
    SETSOCKOPT(RTDRV_RT_CLS_EXTEND_RULE_ADD, &rt_cls_cfg, rtdrv_rt_clsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_cls_extend_rule_add */

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
int32 rt_cls_extend_rule_delete(uint32 index)
{
    rtdrv_rt_clsCfg_t rt_cls_cfg;

    memset(&rt_cls_cfg, 0x0, sizeof(rtdrv_rt_clsCfg_t));

    /* function body */
    osal_memcpy(&rt_cls_cfg.index, &index, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_CLS_EXTEND_RULE_DELETE, &rt_cls_cfg, rtdrv_rt_clsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_cls_extend_rule_delete */

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
int32 rt_cls_extend_rule_get(uint32 index, rt_cls_extend_rule_t *pExtClsRule)
{
    rtdrv_rt_clsCfg_t rt_cls_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pExtClsRule), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_cls_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_CLS_EXTEND_RULE_GET, &rt_cls_cfg, rtdrv_rt_clsCfg_t, 1);
    osal_memcpy(pExtClsRule, &rt_cls_cfg.extClsRule, sizeof(rt_cls_extend_rule_t));

    return RT_ERR_OK;
}   /* end of rt_cls_extend_rule_get */



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
 *      For the device type, the WAN port is not the PON port, ex. ethernet WAN mode,
 *      it needs to change the default forward port of the WAN port from the LAN side to WAN side  
 */
int32 rt_cls_fwdPort_set(rt_port_t port, rt_port_t fwdPort)
{
    rtdrv_rt_clsCfg_t rt_cls_cfg;

    /* function body */
    memset(&rt_cls_cfg, 0x0, sizeof(rtdrv_rt_clsCfg_t));
    osal_memcpy(&rt_cls_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_cls_cfg.fwdPort, &fwdPort, sizeof(rt_port_t));
    SETSOCKOPT(RTDRV_RT_CLS_FWDPORT_SET, &rt_cls_cfg, rtdrv_rt_clsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_cls_fwdPort_set */

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
int32 rt_cls_vlan_paser_set(rt_port_t port, rt_cls_vlan_attribute_t type, uint32 data)
{
    rtdrv_rt_clsCfg_t rt_cls_cfg;

    memset(&rt_cls_cfg, 0x0, sizeof(rtdrv_rt_clsCfg_t));

    /* function body */
    osal_memcpy(&rt_cls_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_cls_cfg.type, &type, sizeof(rt_cls_vlan_attribute_t));
    osal_memcpy(&rt_cls_cfg.data, &data, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_CLS_VLAN_PASER_SET, &rt_cls_cfg, rtdrv_rt_clsCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_cls_vlan_paser_set */

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
int32 rt_cls_vlan_paser_get(rt_port_t port, rt_cls_vlan_attribute_t type, uint32 *pData)
{
    rtdrv_rt_clsCfg_t rt_cls_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_cls_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_cls_cfg.type, &type, sizeof(rt_cls_vlan_attribute_t));
    GETSOCKOPT(RTDRV_RT_CLS_VLAN_PASER_GET, &rt_cls_cfg, rtdrv_rt_clsCfg_t, 1);
    osal_memcpy(pData, &rt_cls_cfg.data, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rt_cls_vlan_paser_get */
