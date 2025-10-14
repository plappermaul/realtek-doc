/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : RTL8367/RTL8367D switch high-level API
 *
 * Feature : The file includes ACL module high-layer API defination
 *
 */

#ifndef __DAL_RTL8367D_ACL_H__
#define __DAL_RTL8367D_ACL_H__

#include <acl.h>

#define RTL8367D_ACLRULENO                  64

#define RTL8367D_ACLRULEMAX                 (RTL8367D_ACLRULENO-1)
#define RTL8367D_ACLRULEFIELDNO             8
#define RTL8367D_ACLTEMPLATENO              5
#define RTL8367D_ACLTYPEMAX                 (RTL8367D_ACLTEMPLATENO-1)

#define RTL8367D_ACLRULETBLEN               9
#define RTL8367D_ACLACTTBLEN                4
#define RTL8367D_ACLRULETBADDR(type, rule)  ((type << 6) | rule)
#define RTL8367D_ACLRULETBADDR2(type, rule) ((type << 5) | (rule + 64))

#define ACL_ACT_CVLAN_ENABLE_MASK           0x1
#define ACL_ACT_SVLAN_ENABLE_MASK           0x2
#define ACL_ACT_PRIORITY_ENABLE_MASK        0x4
#define ACL_ACT_POLICING_ENABLE_MASK        0x8
#define ACL_ACT_FWD_ENABLE_MASK             0x10
#define ACL_ACT_INTGPIO_ENABLE_MASK         0x20

#define RTL8367D_ACLRULETAGBITS             5

#define RTL8367D_ACLRANGENO                 8

#define RTL8367D_ACLRANGEMAX                (RTL8367D_ACLRANGENO-1)

#define RTL8367D_ACL_PORTRANGEMAX           (0xFFFF)
#define RTL8367D_ACL_ACT_TABLE_LEN          (4)

#define RTL8367D_FIELDSEL_FORMAT_NUMBER     (8)
#define RTL8367D_FIELDSEL_MAX_OFFSET        (255)

#define RTL8367D_MAX_LOG_CNT_NUM            (16)

#define RTL8367D_RTK_IPV6_ADDR_WORD_LENGTH           2UL

#define RTL8367D_ACLGPIOPINNO               61


enum RTL8367D_FIELDSEL_FORMAT_FORMAT
{
    RTL8367D_FIELDSEL_FORMAT_DEFAULT = 0,
    RTL8367D_FIELDSEL_FORMAT_RAW,
    RTL8367D_FIELDSEL_FORMAT_LLC,
    RTL8367D_FIELDSEL_FORMAT_IPV4,
    RTL8367D_FIELDSEL_FORMAT_ARP,
    RTL8367D_FIELDSEL_FORMAT_IPV6,
    RTL8367D_FIELDSEL_FORMAT_IPPAYLOAD,
    RTL8367D_FIELDSEL_FORMAT_L4PAYLOAD,
    RTL8367D_FIELDSEL_FORMAT_END
};

enum RTL8367D_ACLFIELDTYPES
{
    RTL8367D_ACL_UNUSED,
    RTL8367D_ACL_DMAC0,
    RTL8367D_ACL_DMAC1,
    RTL8367D_ACL_DMAC2,
    RTL8367D_ACL_SMAC0,
    RTL8367D_ACL_SMAC1,
    RTL8367D_ACL_SMAC2,
    RTL8367D_ACL_ETHERTYPE,
    RTL8367D_ACL_STAG,
    RTL8367D_ACL_CTAG,
    RTL8367D_ACL_IP4SIP0 = 0x10,
    RTL8367D_ACL_IP4SIP1,
    RTL8367D_ACL_IP4DIP0,
    RTL8367D_ACL_IP4DIP1,
    RTL8367D_ACL_IP6SIP0WITHIPV4 = 0x20,
    RTL8367D_ACL_IP6SIP1WITHIPV4,
    RTL8367D_ACL_IP6DIP0WITHIPV4 = 0x28,
    RTL8367D_ACL_IP6DIP1WITHIPV4,
    RTL8367D_ACL_L4DPORT = 0x2a,
    RTL8367D_ACL_L4SPORT = 0x2b,
    RTL8367D_ACL_IPRANGE = 0x31,
    RTL8367D_ACL_FIELD_VALID = 0x33,
    RTL8367D_ACL_FIELD_SELECT00 = 0x40,
    RTL8367D_ACL_FIELD_SELECT01,
    RTL8367D_ACL_FIELD_SELECT02,
    RTL8367D_ACL_FIELD_SELECT03,
    RTL8367D_ACL_FIELD_SELECT04,
    RTL8367D_ACL_FIELD_SELECT05,
    RTL8367D_ACL_FIELD_SELECT06,
    RTL8367D_ACL_FIELD_SELECT07,
    RTL8367D_ACL_TYPE_END
};

enum RTL8367D_ACLTCAMTYPES
{
    RTL8367D_CAREBITS= 0,
    RTL8367D_DATABITS
};

typedef enum rtl8367d_aclFwd
{
    RTL8367D_ACL_FWD_MIRROR = 0,
    RTL8367D_ACL_FWD_REDIRECT,
    RTL8367D_ACL_FWD_MIRRORFUNTION,
    RTL8367D_ACL_FWD_TRAP,
} rtl8367d_aclFwd_t;


struct rtl8367d_acl_rule_smi_st{
    rtk_uint16 rule_info;
    rtk_uint16 field[RTL8367D_ACLRULEFIELDNO];
};

struct rtl8367d_acl_rule_smi_ext_st{
    rtk_uint16 rule_info;
};

typedef struct RTL8367D_ACLRULESMI{
    struct rtl8367d_acl_rule_smi_st  care_bits;
    rtk_uint32      valid;
    struct rtl8367d_acl_rule_smi_st  data_bits;

}rtl8367d_aclrulesmi;

struct rtl8367d_acl_rule_st{
    rtk_uint32 active_portmsk;
    rtk_uint32 type;
    rtk_uint32 tag_exist;
    rtk_uint16 field[RTL8367D_ACLRULEFIELDNO];
};

typedef struct RTL8367D_ACLRULE{
    struct rtl8367d_acl_rule_st  data_bits;
    rtk_uint32      valid;
    struct rtl8367d_acl_rule_st  care_bits;
}rtl8367d_aclrule;


typedef struct rtl8367d_acltemplate_s{
    rtk_uint8 field[8];
}rtl8367d_acltemplate_t;


typedef struct rtl8367d_acl_act_s{
    rtk_uint32 cvidx_cact;
    rtk_uint32 cact;
    rtk_uint32 svidx_sact;
    rtk_uint32 sact;


    rtk_uint32 aclmeteridx;
    rtk_uint32 fwdpmask;
    rtk_uint32 fwdact;

    rtk_uint32 pridx;
    rtk_uint32 priact;
    rtk_uint32 gpio_pin;
    rtk_uint32 gpio_en;
    rtk_uint32 aclint;

    rtk_uint32 cact_ext;
    rtk_uint32 fwdact_ext;
    rtk_uint32 tag_fmt;
}rtl8367d_acl_act_t;

typedef struct rtl8367d_acl_rule_union_s
{
    rtl8367d_aclrule aclRule;
    rtl8367d_acl_act_t aclAct;
    rtk_uint32 aclActCtrl;
    rtk_uint32 aclNot;
}rtl8367d_acl_rule_union_t;


/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_init
 * Description:
 *      ACL initialization function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - Pointer pFilter_field or pFilter_cfg point to NULL.
 * Note:
 *      This function enable and intialize ACL function
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_init(void);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_field_add
 * Description:
 *      Add comparison rule to an ACL configuration
 * Input:
 *      pFilter_cfg     - The ACL configuration that this function will add comparison rule
 *      pFilter_field   - The comparison rule that will be added.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_NULL_POINTER     - Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This function add a comparison rule (*pFilter_field) to an ACL configuration (*pFilter_cfg).
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL
 *      comparison rules by means of linked list. Pointer pFilter_field will be added to linked
 *      list keeped by structure that pFilter_cfg points to.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_field_add(rtk_filter_cfg_t *pFilter_cfg, rtk_filter_field_t *pFilter_field);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_cfg_add
 * Description:
 *      Add an ACL configuration to ASIC
 * Input:
 *      filter_id       - Start index of ACL configuration.
 *      pFilter_cfg     - The ACL configuration that this function will add comparison rule
 *      pFilter_action  - Action(s) of ACL configuration.
 * Output:
 *      ruleNum - number of rules written in acl table
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_SMI                              - SMI access error
 *      RT_ERR_NULL_POINTER                     - Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 *      RT_ERR_ENTRY_INDEX                      - Invalid filter_id .
 *      RT_ERR_NULL_POINTER                     - Pointer pFilter_action or pFilter_cfg point to NULL.
 *      RT_ERR_FILTER_INACL_ACT_NOT_SUPPORT     - Action is not supported in this chip.
 *      RT_ERR_FILTER_INACL_RULE_NOT_SUPPORT    - Rule is not supported.
 * Note:
 *      This function store pFilter_cfg, pFilter_action into ASIC. The starting
 *      index(es) is filter_id.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_cfg_add(rtk_filter_id_t filter_id, rtk_filter_cfg_t *pFilter_cfg, rtk_filter_action_t *pAction, rtk_filter_number_t *ruleNum);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_cfg_del
 * Description:
 *      Delete an ACL configuration from ASIC
 * Input:
 *      filter_id   - Start index of ACL configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_FILTER_ENTRYIDX  - Invalid filter_id.
 * Note:
 *      This function delete a group of ACL rules starting from filter_id.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_cfg_del(rtk_filter_id_t filter_id);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_cfg_delAll
 * Description:
 *      Delete all ACL entries from ASIC
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This function delete all ACL configuration from ASIC.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_cfg_delAll(void);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_cfg_get
 * Description:
 *      Get one ingress acl configuration from ASIC.
 * Input:
 *      filter_id       - Start index of ACL configuration.
 * Output:
 *      pFilter_cfg     - buffer pointer of ingress acl data
 *      pFilter_action  - buffer pointer of ingress acl action
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_NULL_POINTER     - Pointer pFilter_action or pFilter_cfg point to NULL.
 *      RT_ERR_FILTER_ENTRYIDX  - Invalid entry index.
 * Note:
 *      This function delete all ACL configuration from ASIC.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_cfg_get(rtk_filter_id_t filter_id, rtk_filter_cfg_raw_t *pFilter_cfg, rtk_filter_action_t *pAction);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_unmatchAction_set
 * Description:
 *      Set action to packets when no ACL configuration match
 * Input:
 *      port    - Port id.
 *      action  - Action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function sets action of packets when no ACL configruation matches.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_unmatchAction_set(rtk_port_t port, rtk_filter_unmatch_action_t action);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_unmatchAction_get
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
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_unmatchAction_get(rtk_port_t port, rtk_filter_unmatch_action_t* action);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_state_set
 * Description:
 *      Set state of ingress ACL.
 * Input:
 *      port    - Port id.
 *      state   - Ingress ACL state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_state_set(rtk_port_t port, rtk_filter_state_t state);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_state_get
 * Description:
 *      Get state of ingress ACL.
 * Input:
 *      port    - Port id.
 * Output:
 *      pState  - Ingress ACL state.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_state_get(rtk_port_t port, rtk_filter_state_t* state);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_template_set
 * Description:
 *      Set template of ingress ACL.
 * Input:
 *      template - Ingress ACL template
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function set ACL template.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_template_set(rtk_filter_template_t *aclTemplate);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_template_get
 * Description:
 *      Get template of ingress ACL.
 * Input:
 *      template - Ingress ACL template
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This function gets template of ACL.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_template_get(rtk_filter_template_t *aclTemplate);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_field_sel_set
 * Description:
 *      Set user defined field selectors in HSB
 * Input:
 *      index       - index of field selector 0-15
 *      format      - Format of field selector
 *      offset      - Retrieving data offset
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      System support 16 user defined field selctors.
 *      Each selector can be enabled or disable.
 *      User can defined retrieving 16-bits in many predefiend
 *      standard l2/l3/l4 payload.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_field_sel_set(rtk_uint32 index, rtk_field_sel_t format, rtk_uint32 offset);

/* Function Name:
 *      dal_rtl8367d_filter_igrAcl_field_sel_get
 * Description:
 *      Get user defined field selectors in HSB
 * Input:
 *      index       - index of field selector 0-15
 * Output:
 *      pFormat     - Format of field selector
 *      pOffset     - Retrieving data offset
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      None.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_igrAcl_field_sel_get(rtk_uint32 index, rtk_field_sel_t *pFormat, rtk_uint32 *pOffset);

/* Function Name:
 *      dal_rtl8367d_filter_iprange_set
 * Description:
 *      Set IP Range check
 * Input:
 *      index       - index of IP Range 0-15
 *      type        - IP Range check type, 0:Delete a entry, 1: IPv4_SIP, 2: IPv4_DIP, 3:IPv6_SIP, 4:IPv6_DIP
 *      upperIp     - The upper bound of IP range
 *      lowerIp     - The lower Bound of IP range
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
extern rtk_api_ret_t dal_rtl8367d_filter_iprange_set(rtk_uint32 index, rtk_filter_iprange_t type, ipaddr_t upperIp, ipaddr_t lowerIp);

/* Function Name:
 *      dal_rtl8367d_filter_iprange_get
 * Description:
 *      Set IP Range check
 * Input:
 *      index       - index of IP Range 0-15
 * Output:
 *      pType        - IP Range check type, 0:Delete a entry, 1: IPv4_SIP, 2: IPv4_DIP, 3:IPv6_SIP, 4:IPv6_DIP
 *      pUpperIp     - The upper bound of IP range
 *      pLowerIp     - The lower Bound of IP range
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      upperIp must be larger or equal than lowerIp.
 */
extern rtk_api_ret_t dal_rtl8367d_filter_iprange_get(rtk_uint32 index, rtk_filter_iprange_t *pType, ipaddr_t *pUpperIp, ipaddr_t *pLowerIp);

#endif /* __DAL_RTL8367D_ACL_H__ */
