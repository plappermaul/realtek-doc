/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
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
 * Purpose : Definition of CLS API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) CLS
 */


#ifndef __DAL_RT_RTL8277C_CLS_H__
#define __DAL_RT_RTL8277C_CLS_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/rt/rt_cls.h>
#include <rtk/rt/rt_ponmisc.h>

/*
 * Symbol Definition
 */

#define RTL8277C_CLS_EXT_WAN_PORT 8

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */
 
/* Function Name:
 *      dal_rt_rtl8277c_cls_init
 * Description:
 *      add classification rule
 * Input:
 *      pClsRule   - classification rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

extern int32 dal_rt_rtl8277c_cls_init(void);


/* Function Name:
 *      dal_rt_rtl8277c_cls_veip_port_rule_set
 * Description:
 *      Enable/Disable VEIP port default rule
 * Input:
 *      port   - Port index
 *      state  - VEIP port state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

extern int32 dal_rt_rtl8277c_cls_veipPortRule_set(rtk_port_t port, rtk_enable_t state);

/* Function Name:
 *      dal_rt_rtl8277c_cls_rule_add
 * Description:
 *      add classification rule
 * Input:
 *      pClsRule   - classification rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

extern int32 dal_rt_rtl8277c_cls_rule_add(rt_cls_rule_t *pClsRule);

/* Function Name:
 *      dal_rt_rtl8277c_cls_rule_delete
 * Description:
 *      delete classification rule
 * Input:
 *      index   - classification rule index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

extern int32 dal_rt_rtl8277c_cls_rule_delete(uint32 ruleIndex);

/* Function Name:
 *      dal_rt_rtl8277c_cls_rule_get
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
extern int32 dal_rt_rtl8277c_cls_rule_get(rt_cls_get_op_t op, uint32 index, rt_cls_dir_t dir, rt_cls_rule_info_t *pClsRuleInfo);


/* Function Name:
 *      dal_rt_rtl8277c_cls_extend_rule_add
 * Description:
 *      add classification extend rule
 * Input:
 *      pClsRule   - classification rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

extern int32 dal_rt_rtl8277c_cls_extend_rule_add(rt_cls_extend_rule_t *pClsRule);

/* Function Name:
 *      dal_rt_rtl8277c_cls_extend_rule_delete
 * Description:
 *      delete classification extend rule
 * Input:
 *      index   - classification rule index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

extern int32 dal_rt_rtl8277c_cls_extend_rule_delete(uint32 ruleIndex);

/* Function Name:
 *      dal_rt_rtl8277c_cls_extend_rule_get
 * Description:
 *      Get classification extend rule
 * Input:
 *      index     - index of classify rule entry.
 * Output:
 *      pClsRule  - extend classify rule
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Rule not found
 * Note:
 *      None.
 */
extern int32 dal_rt_rtl8277c_cls_extend_rule_get(uint32 index, rt_cls_extend_rule_t *pClsRule);


/* Function Name:
 *      dal_rt_rtl8277c_cls_pon_sid_set
 * Description:
 *      Set pon sid info
 * Input:
 *      sid     - Stream ID
 *      sidInfo - Stream ID information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

extern int32 dal_rt_rtl8277c_cls_pon_sid_set(uint16 sid,rt_ponmisc_sidInfo_t sidInfo);

/* Function Name:
 *      dal_rt_rtl8277c_cls_fwdPort_set
 * Description:
 *      Set veip port default forward port
 * Input:
 *      port    - port ID
 *      fwdPort - default forward port in VEIP mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

extern int32 dal_rt_rtl8277c_cls_fwdPort_set(rt_port_t port, rt_port_t fwdPort);

/* Function Name:
 *      dal_rt_rtl8277c_cls_vlan_paser_set
 * Description:
 *      Set VLAN parser setting.
 * Input:
 *      port     - port index.
 *      type     - vlan parser attribute type
 *      data     - value of vlan parser attribute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

extern int32 dal_rt_rtl8277c_cls_vlan_paser_set(rt_port_t port, rt_cls_vlan_attribute_t type, uint32 data);


/* Function Name:
 *      dal_rt_rtl8277c_cls_vlan_paser_get
 * Description:
 *      Get VLAN parser setting.
 * Input:
 *      port     - port index.
 *      type     - vlan parser attribute type
 * Output:
 *      pData     - value of vlan parser attribute
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */

extern int32 dal_rt_rtl8277c_cls_vlan_paser_get(rt_port_t port, rt_cls_vlan_attribute_t type, uint32 *pData);


#endif /* __DAL_RT_RTL8277C_CLS_H__ */
