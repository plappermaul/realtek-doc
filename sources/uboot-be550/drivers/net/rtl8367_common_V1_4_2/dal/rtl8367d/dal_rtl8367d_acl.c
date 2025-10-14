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
 * $Revision$
 * $Date$
 *
 * Purpose : RTK switch high-level API for RTL8367/RTL8367D
 * Feature : Here is a list of all functions and variables in ACL module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_acl.h>
#include <rate.h>
#include <linux/string.h>

#include <dal/rtl8367d/rtl8367d_asicdrv.h>

#if defined(CONFIG_RTL8367D_ASICDRV_TEST)
rtl8367d_aclrulesmi Rtl8370sVirtualAclRuleTable[RTL8367D_ACLRULENO];
rtk_uint16 Rtl8370sVirtualAclActTable[RTL8367D_ACLRULENO][RTL8367D_ACL_ACT_TABLE_LEN];
#endif


CONST_T rtk_uint8 rtl8367D_filter_templateField[RTL8367D_ACLTEMPLATENO][RTL8367D_ACLRULEFIELDNO] = {
    {RTL8367D_ACL_DMAC0,             RTL8367D_ACL_DMAC1,          RTL8367D_ACL_DMAC2,          RTL8367D_ACL_SMAC0,          RTL8367D_ACL_SMAC1,          RTL8367D_ACL_SMAC2,          RTL8367D_ACL_ETHERTYPE,      RTL8367D_ACL_FIELD_SELECT07},
    {RTL8367D_ACL_IP4SIP0,           RTL8367D_ACL_IP4SIP1,        RTL8367D_ACL_IP4DIP0,        RTL8367D_ACL_IP4DIP1,        RTL8367D_ACL_L4SPORT,        RTL8367D_ACL_L4DPORT,        RTL8367D_ACL_FIELD_SELECT02, RTL8367D_ACL_FIELD_SELECT07},
    {RTL8367D_ACL_IP6SIP0WITHIPV4,   RTL8367D_ACL_IP6SIP1WITHIPV4,RTL8367D_ACL_L4SPORT,        RTL8367D_ACL_L4DPORT,        RTL8367D_ACL_FIELD_SELECT05, RTL8367D_ACL_FIELD_SELECT06, RTL8367D_ACL_FIELD_SELECT00, RTL8367D_ACL_FIELD_SELECT01},
    {RTL8367D_ACL_IP6DIP0WITHIPV4,   RTL8367D_ACL_IP6DIP1WITHIPV4,RTL8367D_ACL_L4SPORT,        RTL8367D_ACL_L4DPORT,        RTL8367D_ACL_FIELD_SELECT00, RTL8367D_ACL_FIELD_SELECT03, RTL8367D_ACL_FIELD_SELECT04, RTL8367D_ACL_FIELD_SELECT07},
    {RTL8367D_ACL_FIELD_SELECT01,    RTL8367D_ACL_IPRANGE,        RTL8367D_ACL_FIELD_SELECT02, RTL8367D_ACL_CTAG,           RTL8367D_ACL_STAG,           RTL8367D_ACL_FIELD_SELECT04, RTL8367D_ACL_FIELD_SELECT03, RTL8367D_ACL_FIELD_SELECT07}
};

CONST_T rtk_uint8 rtl8367D_filter_advanceCaretagField[RTL8367D_ACLTEMPLATENO][2] = {
    {TRUE,      7},
    {TRUE,      7},
    {FALSE,     0},
    {TRUE,      7},
    {TRUE,      7},
};


CONST_T rtk_uint8 rtl8367D_filter_fieldTemplateIndex[FILTER_FIELD_END][RTK_FILTER_FIELD_USED_MAX] = {
    {0x00, 0x01,0x02},
    {0x03, 0x04,0x05},
    {0x06},
    {0x43},
    {0x44},
    {0x10, 0x11},
    {0x12, 0x13},
    {0x24},
    {0x25},
    {0x35},
    {0x35},
    {0x20, 0x21},
    {0x30, 0x31},
    {0x26},
    {0x27},
    {0x14},
    {0x15},
    {0x16},
    {0x14},
    {0x15},
    {0x14},
    {0x14},
    {0x14},

    {0},
    {0x41},
    {0},

    {0x26},
    {0x27},
    {0x16},
    {0x35},
    {0x36},
    {0x24},
    {0x25},
    {0x47},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},

    {0xFF} /* Pattern Match */
};

CONST_T rtk_uint8 rtl8367D_filter_fieldSize[FILTER_FIELD_END] = {
    3, 3, 1, 1, 1,
    2, 2, 1, 1, 1, 1, 2, 2, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    0,1,0,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    8
};

CONST_T rtk_uint16 rtl8367D_field_selector[RTL8367D_FIELDSEL_FORMAT_NUMBER][2] =
{
    {RTL8367D_FIELDSEL_FORMAT_IPV6, 0},    /* Field Selector 0 */
    {RTL8367D_FIELDSEL_FORMAT_IPV6, 6},    /* Field Selector 1 */
    {RTL8367D_FIELDSEL_FORMAT_IPPAYLOAD, 12}, /* Field Selector 2 */
    {RTL8367D_FIELDSEL_FORMAT_IPV4, 6},       /* Field Selector 3 */
    {RTL8367D_FIELDSEL_FORMAT_IPPAYLOAD, 0},  /* Field Selector 4 */
    {RTL8367D_FIELDSEL_FORMAT_IPV4, 0},       /* Field Selector 5 */
    {RTL8367D_FIELDSEL_FORMAT_IPV4, 8},       /* Field Selector 6 */
    {RTL8367D_FIELDSEL_FORMAT_DEFAULT, 0}     /* Field Selector 7 */
};

/*
    Exchange structure type define with MMI and SMI
*/
static void _rtl8367d_aclRuleStSmi2User( rtl8367d_aclrule *pAclUser, rtl8367d_aclrulesmi *pAclSmi)
{
    rtk_uint8 *care_ptr, *data_ptr;
    rtk_uint8 care_tmp, data_tmp;
    rtk_uint8 care_pmask, data_pmask;
    rtk_uint32 i;

    care_ptr = (rtk_uint8*)&pAclSmi->care_bits;
    data_ptr = (rtk_uint8*)&pAclSmi->data_bits;

    for ( i = 0; i < sizeof(struct rtl8367d_acl_rule_smi_st); i++)
    {
        care_tmp = *(care_ptr + i) ^ (*(data_ptr + i));
        data_tmp = ~(*(care_ptr + i)) & *(data_ptr + i);

        *(care_ptr + i) = care_tmp;
        *(data_ptr + i) = data_tmp;
    }

    pAclUser->data_bits.active_portmsk = ((pAclSmi->data_bits.rule_info >> 8) & 0x00FF);
    pAclUser->data_bits.type = (pAclSmi->data_bits.rule_info & 0x0007);
    pAclUser->data_bits.tag_exist = (pAclSmi->data_bits.rule_info & 0x00F8) >> 3;

    for(i = 0; i < RTL8367D_ACLRULEFIELDNO; i++)
        pAclUser->data_bits.field[i] = pAclSmi->data_bits.field[i];

    pAclUser->valid = pAclSmi->valid;

    pAclUser->care_bits.active_portmsk = ((pAclSmi->care_bits.rule_info >> 8) & 0x00FF);
    pAclUser->care_bits.type = (pAclSmi->care_bits.rule_info & 0x0007);
    pAclUser->care_bits.tag_exist = (pAclSmi->care_bits.rule_info & 0x00F8) >> 3;


    care_pmask = pAclUser->care_bits.active_portmsk & 0xff;
    data_pmask = pAclUser->data_bits.active_portmsk & 0xff;

    for (i = 0; i <= 7; i++)
    {
        if( ((care_pmask & (0x01 << i)) == 0 )&&( (data_pmask & (0x01 << i)) == 0) )
        {
            care_pmask |= (0x01 << i);
            data_pmask |= (0x01 << i);
        }
    }

    pAclUser->care_bits.active_portmsk = care_pmask;
    pAclUser->data_bits.active_portmsk = data_pmask;

    for(i = 0; i < RTL8367D_ACLRULEFIELDNO; i++)
        pAclUser->care_bits.field[i] = pAclSmi->care_bits.field[i];
}

/*
    Exchange structure type define with MMI and SMI
*/
static void _rtl8367d_aclRuleStUser2Smi(rtl8367d_aclrule *pAclUser, rtl8367d_aclrulesmi *pAclSmi)
{
    rtk_uint8 *care_ptr, *data_ptr;
    rtk_uint8 care_tmp, data_tmp;
    rtk_uint8 care_pmask, data_pmask;
    rtk_uint32 i;

    care_pmask = pAclUser->care_bits.active_portmsk & 0xff;
    data_pmask = pAclUser->data_bits.active_portmsk & 0xff;

    for (i = 0; i <= 7; i++)
    {
        if( (care_pmask & (0x01 << i)) && (data_pmask & (0x01 << i)) )
        {
            care_pmask &= ~(0x01 << i);
            data_pmask &= ~(0x01 << i);
        }
    }
    pAclSmi->data_bits.rule_info = (data_pmask << 8) | ((pAclUser->data_bits.tag_exist & 0x1F) << 3) | (pAclUser->data_bits.type & 0x07);

    for(i = 0;i < RTL8367D_ACLRULEFIELDNO; i++)
        pAclSmi->data_bits.field[i] = pAclUser->data_bits.field[i];

    pAclSmi->valid = pAclUser->valid;

    pAclSmi->care_bits.rule_info = (care_pmask << 8) | ((pAclUser->care_bits.tag_exist & 0x1F) << 3) | (pAclUser->care_bits.type & 0x07);

    for(i = 0; i < RTL8367D_ACLRULEFIELDNO; i++)
        pAclSmi->care_bits.field[i] = pAclUser->care_bits.field[i];

    care_ptr = (rtk_uint8*)&pAclSmi->care_bits;
    data_ptr = (rtk_uint8*)&pAclSmi->data_bits;

    for ( i = 0; i < sizeof(struct rtl8367d_acl_rule_smi_st); i++)
    {
        care_tmp = ~(*(care_ptr + i)) | ~(*(data_ptr + i));
        data_tmp = ~(*(care_ptr + i)) | *(data_ptr + i);

        *(care_ptr + i) = care_tmp;
        *(data_ptr + i) = data_tmp;
    }
}

/*
    Exchange structure type define with MMI and SMI
*/
static void _rtl8367d_aclActStSmi2User(rtl8367d_acl_act_t *pAclUser, rtk_uint16 *pAclSmi)
{

    pAclUser->cact = (pAclSmi[0] & 0x3000) >> 12;
    pAclUser->cvidx_cact = (pAclSmi[0] & 0x0FFF);

    pAclUser->sact = (pAclSmi[1] & 0x0C00) >> 10;
    pAclUser->svidx_sact = ((pAclSmi[0] & 0xC000) >> 14) | ((pAclSmi[1] & 0x03FF) << 2);

    pAclUser->aclmeteridx = ((pAclSmi[1] & 0xF000) >> 12) | ((pAclSmi[2] & 0x0003) << 4);

    pAclUser->fwdact = (pAclSmi[2] & 0x0C00) >> 10;
    pAclUser->fwdpmask = ((pAclSmi[2] & 0x03FC) >> 2);

    pAclUser->priact = (pAclSmi[3] & 0x000C) >> 2;
    pAclUser->pridx = ((pAclSmi[2] & 0xF000) >> 12) | ((pAclSmi[3] & 0x0003) << 4);

    pAclUser->aclint = (pAclSmi[3] & 0x0400) >> 10;
    pAclUser->gpio_pin = (pAclSmi[3] & 0x03F0) >> 4;
    if(pAclUser->gpio_pin == 0)
        pAclUser->gpio_en = DISABLED;
    else
        pAclUser->gpio_en = ENABLED;

    pAclUser->cact_ext = (pAclSmi[3] & 0x1800) >> 11;
    pAclUser->tag_fmt = (pAclSmi[3] & 0x6000) >> 13;
    pAclUser->fwdact_ext = (pAclSmi[3] & 0x8000) >> 15;
}

/*
    Exchange structure type define with MMI and SMI
*/
static void _rtl8367d_aclActStUser2Smi(rtl8367d_acl_act_t *pAclUser, rtk_uint16 *pAclSmi)
{
    if(pAclUser->gpio_en == DISABLED)
        pAclUser->gpio_pin = 0;
    pAclSmi[0] |= (pAclUser->cvidx_cact & 0x0FFF);
    pAclSmi[0] |= (pAclUser->cact & 0x0003) << 12;
    pAclSmi[0] |= (pAclUser->svidx_sact & 0x0003) << 14;

    pAclSmi[1] |= (pAclUser->svidx_sact & 0x0FFC) >> 2;
    pAclSmi[1] |= (pAclUser->sact & 0x0003) << 10;
    pAclSmi[1] |= (pAclUser->aclmeteridx & 0x000F) << 12;
    pAclSmi[2] |= (pAclUser->aclmeteridx & 0x0030) >> 4;
    pAclSmi[2] |= (pAclUser->fwdpmask & 0x00FF) << 2;
    pAclSmi[2] |= (pAclUser->fwdact & 0x0003) << 10;
    pAclSmi[2] |= (pAclUser->pridx & 0x000F) << 12;

    pAclSmi[3] |= (pAclUser->pridx & 0x0030) >> 4;
    pAclSmi[3] |= (pAclUser->priact & 0x0003) << 2;
    pAclSmi[3] |= (pAclUser->gpio_pin & 0x003F) << 4;
    pAclSmi[3] |= (pAclUser->aclint & 0x0001) << 10;
    pAclSmi[3] |= (pAclUser->cact_ext & 0x0003) << 11;
    pAclSmi[3] |= (pAclUser->tag_fmt & 0x0003) << 13;
    pAclSmi[3] |= (pAclUser->fwdact_ext & 0x0001) << 15;
}

static rtk_api_ret_t _rtl8367d_getAsicAclTemplate(rtk_uint32 index, rtl8367d_acltemplate_t *pAclType)
{
    ret_t retVal;
    rtk_uint32 i;
    rtk_uint32 regData, regAddr;

    if(index >= RTL8367D_ACLTEMPLATENO)
        return RT_ERR_OUT_OF_RANGE;

    regAddr = (RTL8367D_REG_ACL_RULE_TEMPLATE0_CTRL0 + index * 0x4);

    for(i = 0; i < (RTL8367D_ACLRULEFIELDNO/2); i++)
    {
        retVal = rtl8367d_getAsicReg(regAddr + i,&regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        pAclType->field[i*2] = regData & 0xFF;
        pAclType->field[i*2 + 1] = (regData >> 8) & 0xFF;
    }

    return RT_ERR_OK;
}


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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_cfg_delAll(void)
{
    rtk_uint32            i;
    rtk_api_ret_t     ret;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    for(i = 0; i < RTL8367D_ACLRULENO; i++)
    {
        if((ret = rtl8367d_setAsicRegBits(RTL8367D_REG_ACL_ACTION_CTRL0 + (i >> 1), (0x3F << ((i & 0x1) << 3)), FILTER_ENACT_INIT_MASK))!= RT_ERR_OK)
            return ret;

        if((ret = rtl8367d_setAsicRegBit(RTL8367D_REG_ACL_ACTION_CTRL0 + (i >> 1), (6 + ((i & 0x1) << 3)), DISABLED)) != RT_ERR_OK )
            return ret;
    }

    return rtl8367d_setAsicRegBit(RTL8367D_REG_ACL_RESET_CFG, RTL8367D_ACL_RESET_CFG_OFFSET, TRUE);;
}

static rtk_api_ret_t _rtl8367d_setAclTemplate(rtk_uint32 index, rtl8367d_acltemplate_t* pAclType)
{
    ret_t retVal;
    rtk_uint32 i;
    rtk_uint32 regAddr, regData;

    if(index >= RTL8367D_ACLTEMPLATENO)
        return RT_ERR_OUT_OF_RANGE;

    regAddr = (RTL8367D_REG_ACL_RULE_TEMPLATE0_CTRL0 + index * 0x4);

    for(i = 0; i < (RTL8367D_ACLRULEFIELDNO/2); i++)
    {
        regData = pAclType->field[i*2+1];
        regData = regData << 8 | pAclType->field[i*2];

        retVal = rtl8367d_setAsicReg(regAddr + i, regData);

        if(retVal != RT_ERR_OK)
            return retVal;
    }

    return retVal;
}


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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_init(void)
{
    rtl8367d_acltemplate_t       aclTemp;
    rtk_uint32                 i, j;
    rtk_api_ret_t          ret;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if ((ret = dal_rtl8367d_filter_igrAcl_cfg_delAll()) != RT_ERR_OK)
        return ret;

    for(i = 0; i < RTL8367D_ACLTEMPLATENO; i++)
    {
        for(j = 0; j < RTL8367D_ACLRULEFIELDNO;j++)
            aclTemp.field[j] = rtl8367D_filter_templateField[i][j];

        if ((ret = _rtl8367d_setAclTemplate(i, &aclTemp)) != RT_ERR_OK)
            return ret;
    }

    for(i = 0; i < RTL8367D_FIELDSEL_FORMAT_NUMBER; i++)
    {
        regData = (((rtl8367D_field_selector[i][0] << RTL8367D_FIELD_SELECTOR0_FORMAT_OFFSET) & RTL8367D_FIELD_SELECTOR0_FORMAT_MASK ) |
                   ((rtl8367D_field_selector[i][1] << RTL8367D_FIELD_SELECTOR0_OFFSET_OFFSET) & RTL8367D_FIELD_SELECTOR0_OFFSET_MASK ));

        if ((ret = rtl8367d_setAsicReg((RTL8367D_REG_FIELD_SELECTOR0 + i), regData)) != RT_ERR_OK)
            return ret;
    }

    RTK_SCAN_ALL_PHY_PORTMASK(i)
    {

        if ((ret = rtl8367d_setAsicRegBit(RTL8367D_REG_ACL_ENABLE, i, TRUE)) != RT_ERR_OK)
            return ret;

        if ((ret = rtl8367d_setAsicRegBit(RTL8367D_REG_ACL_UNMATCH_PERMIT, i, TRUE)) != RT_ERR_OK)
            return ret;
    }

#ifdef CONFIG_RTL8367D_ASICDRV_TEST
    for(i=0;i<RTL8367D_ACLRULENO;i++)
        memset(&Rtl8370sVirtualAclRuleTable[i],0x00, sizeof(rtl8367d_aclrulesmi));
#endif
    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_field_add(rtk_filter_cfg_t* pFilter_cfg, rtk_filter_field_t* pFilter_field)
{
    rtk_uint32 i;
    rtk_filter_field_t *tailPtr;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pFilter_cfg || NULL == pFilter_field)
        return RT_ERR_NULL_POINTER;

    if(pFilter_field->fieldType >= FILTER_FIELD_END)
        return RT_ERR_ENTRY_INDEX;


    if(0 == pFilter_field->fieldTemplateNo)
    {
        pFilter_field->fieldTemplateNo = rtl8367D_filter_fieldSize[pFilter_field->fieldType];

        for(i = 0; i < pFilter_field->fieldTemplateNo; i++)
        {
            pFilter_field->fieldTemplateIdx[i] = rtl8367D_filter_fieldTemplateIndex[pFilter_field->fieldType][i];
        }
    }

    if(NULL == pFilter_cfg->fieldHead)
    {
        pFilter_cfg->fieldHead = pFilter_field;
    }
    else
    {
        if (pFilter_cfg->fieldHead->next == NULL)
        {
            pFilter_cfg->fieldHead->next = pFilter_field;
        }
        else
        {
            tailPtr = pFilter_cfg->fieldHead->next;
            while( tailPtr->next != NULL)
            {
                tailPtr = tailPtr->next;
            }
            tailPtr->next = pFilter_field;
        }
    }

    return RT_ERR_OK;
}

static rtk_api_ret_t _rtk_filter_igrAcl_writeDataField(rtl8367d_aclrule *aclRule, rtk_filter_field_t *fieldPtr)
{
    rtk_uint32 i, tempIdx,fieldIdx, ipValue, ipMask;
    rtk_uint32 ip6addr[RTL8367D_RTK_IPV6_ADDR_WORD_LENGTH];
    rtk_uint32 ip6mask[RTL8367D_RTK_IPV6_ADDR_WORD_LENGTH];

    for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
    {
        tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;

        aclRule[tempIdx].valid = TRUE;
    }

    switch (fieldPtr->fieldType)
    {
    /* use DMAC structure as representative for mac structure */
    case FILTER_FIELD_DMAC:
    case FILTER_FIELD_SMAC:

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.mac.value.octet[5 - i*2] | (fieldPtr->filter_pattern_union.mac.value.octet[5 - (i*2 + 1)] << 8);
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.mac.mask.octet[5 - i*2] | (fieldPtr->filter_pattern_union.mac.mask.octet[5 - (i*2 + 1)] << 8);
        }
        break;
    case FILTER_FIELD_ETHERTYPE:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.etherType.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.etherType.mask;
        }
        break;
    case FILTER_FIELD_IPV4_SIP:
    case FILTER_FIELD_IPV4_DIP:

        ipValue = fieldPtr->filter_pattern_union.sip.value;
        ipMask = fieldPtr->filter_pattern_union.sip.mask;

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = (0xFFFF & (ipValue >> (i*16)));
            aclRule[tempIdx].care_bits.field[fieldIdx] = (0xFFFF & (ipMask >> (i*16)));
        }
        break;
    case FILTER_FIELD_IPV4_TOS:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.ipTos.value & 0xFF;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.ipTos.mask  & 0xFF;
        }
        break;
    case FILTER_FIELD_IPV4_PROTOCOL:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.protocol.value & 0xFF;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.protocol.mask  & 0xFF;
        }
        break;
    case FILTER_FIELD_IPV6_SIPV6:
    case FILTER_FIELD_IPV6_DIPV6:
        for(i = 0; i < RTL8367D_RTK_IPV6_ADDR_WORD_LENGTH; i++)
        {
            ip6addr[i] = fieldPtr->filter_pattern_union.sipv6.value.addr[i];
            ip6mask[i] = fieldPtr->filter_pattern_union.sipv6.mask.addr[i];
        }

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            if(i < 2)
            {
                aclRule[tempIdx].data_bits.field[fieldIdx] = ((ip6addr[0] & (0xFFFF << (i * 16))) >> (i * 16));
                aclRule[tempIdx].care_bits.field[fieldIdx] = ((ip6mask[0] & (0xFFFF << (i * 16))) >> (i * 16));
            }
        }

        break;
    case FILTER_FIELD_CTAG:
    case FILTER_FIELD_STAG:

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.l2tag.pri.value << 13) | (fieldPtr->filter_pattern_union.l2tag.cfi.value << 12) | fieldPtr->filter_pattern_union.l2tag.vid.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.l2tag.pri.mask << 13) | (fieldPtr->filter_pattern_union.l2tag.cfi.mask << 12) | fieldPtr->filter_pattern_union.l2tag.vid.mask;
        }
        break;
    case FILTER_FIELD_IPV4_FLAG:

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] &= 0x1FFF;
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.xf.value << 15);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.df.value << 14);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.mf.value << 13);

            aclRule[tempIdx].care_bits.field[fieldIdx] &= 0x1FFF;
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.xf.mask << 15);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.df.mask << 14);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.mf.mask << 13);
        }

        break;
    case FILTER_FIELD_IPV4_OFFSET:

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] &= 0xE000;
            aclRule[tempIdx].data_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.inData.value;

            aclRule[tempIdx].care_bits.field[fieldIdx] &= 0xE000;
            aclRule[tempIdx].care_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.inData.mask;
        }

        break;

    case FILTER_FIELD_IPV6_TRAFFIC_CLASS:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;


            aclRule[tempIdx].data_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.inData.value << 4)&0x0FF0;
            aclRule[tempIdx].care_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.inData.mask << 4)&0x0FF0;
        }
        break;
    case FILTER_FIELD_IPV6_NEXT_HEADER:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.inData.value << 8;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.inData.mask << 8;
        }
        break;
    case FILTER_FIELD_TCP_SPORT:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.tcpSrcPort.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.tcpSrcPort.mask;
        }
        break;
    case FILTER_FIELD_TCP_DPORT:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.tcpDstPort.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.tcpDstPort.mask;
        }
        break;
    case FILTER_FIELD_TCP_FLAG:

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.cwr.value << 7);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.ece.value << 6);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.urg.value << 5);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.ack.value << 4);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.psh.value << 3);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.rst.value << 2);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.syn.value << 1);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.tcpFlag.fin.value;

            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.cwr.mask << 7);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.ece.mask << 6);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.urg.mask << 5);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.ack.mask << 4);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.psh.mask << 3);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.rst.mask << 2);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.syn.mask << 1);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.tcpFlag.fin.mask;
        }
        break;
    case FILTER_FIELD_UDP_SPORT:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.udpSrcPort.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.udpSrcPort.mask;
        }
        break;
    case FILTER_FIELD_UDP_DPORT:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.udpDstPort.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.udpDstPort.mask;
        }
        break;
    case FILTER_FIELD_ICMP_CODE:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] &= 0xFF00;
            aclRule[tempIdx].data_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.icmpCode.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] &= 0xFF00;
            aclRule[tempIdx].care_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.icmpCode.mask;
        }
        break;
    case FILTER_FIELD_ICMP_TYPE:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] &= 0x00FF;
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.icmpType.value << 8);
            aclRule[tempIdx].care_bits.field[fieldIdx] &= 0x00FF;
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.icmpType.mask << 8);
        }
        break;
    case FILTER_FIELD_IGMP_TYPE:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.igmpType.value << 8);
            aclRule[tempIdx].care_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.igmpType.mask << 8);
        }
        break;
    case FILTER_FIELD_PATTERN_MATCH:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = ((fieldPtr->filter_pattern_union.pattern.value[i/2] >> (16 * (i%2))) & 0x0000FFFF );
            aclRule[tempIdx].care_bits.field[fieldIdx] = ((fieldPtr->filter_pattern_union.pattern.mask[i/2] >> (16 * (i%2))) & 0x0000FFFF );
        }
        break;
    case FILTER_FIELD_IP_RANGE:
    default:
        tempIdx = (fieldPtr->fieldTemplateIdx[0] & 0xF0) >> 4;
        fieldIdx = fieldPtr->fieldTemplateIdx[0] & 0x0F;

        aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.inData.value;
        aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.inData.mask;
        break;
    }

    return RT_ERR_OK;
}

static rtk_api_ret_t _rtl8367d_getAclRule(rtk_uint32 index, rtl8367d_aclrule *pAclRule)
{
    rtl8367d_aclrulesmi aclRuleSmi;
    rtk_uint32 regAddr, regData;
    ret_t retVal;
    rtk_uint16* tableAddr;
    rtk_uint32 i;

    if(index > RTL8367D_ACLRULEMAX)
        return RT_ERR_OUT_OF_RANGE;

    memset(&aclRuleSmi, 0x00, sizeof(rtl8367d_aclrulesmi));

    /* Write ACS_ADR register for data bits */
    regAddr = RTL8367D_REG_TABLE_ACCESS_ADDR;

    regData = RTL8367D_ACLRULETBADDR(RTL8367D_DATABITS, index);

    retVal = rtl8367d_setAsicReg(regAddr, regData);
    if(retVal != RT_ERR_OK)
        return retVal;


    /* Write ACS_CMD register */
    regAddr = RTL8367D_REG_TABLE_ACCESS_CTRL;
    regData = RTL8367D_TABLE_ACCESS_REG_DATA(RTL8367D_TB_OP_READ, RTL8367D_TB_TARGET_ACLRULE);
    retVal = rtl8367d_setAsicRegBits(regAddr, RTL8367D_TABLE_TYPE_MASK | RTL8367D_COMMAND_TYPE_MASK, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Read Data Bits */
    regAddr = RTL8367D_REG_TABLE_READ_DATA0;
    tableAddr = (rtk_uint16*)&aclRuleSmi.data_bits;
    for(i = 0; i < RTL8367D_ACLRULETBLEN; i++)
    {
        retVal = rtl8367d_getAsicReg(regAddr, &regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        *tableAddr = regData;

        regAddr ++;
        tableAddr ++;
    }

    /* Read Valid Bit */
    retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_TABLE_READ_DATA0+RTL8367D_ACLRULETBLEN, 0, &regData);
    if(retVal != RT_ERR_OK)
        return retVal;
    aclRuleSmi.valid = regData & 0x1;

    /* Write ACS_ADR register for carebits*/
    regAddr = RTL8367D_REG_TABLE_ACCESS_ADDR;
    regData = RTL8367D_ACLRULETBADDR(RTL8367D_CAREBITS, index);
    retVal = rtl8367d_setAsicReg(regAddr, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Write ACS_CMD register */
    regAddr = RTL8367D_REG_TABLE_ACCESS_CTRL;
    regData = RTL8367D_TABLE_ACCESS_REG_DATA(RTL8367D_TB_OP_READ, RTL8367D_TB_TARGET_ACLRULE);
    retVal = rtl8367d_setAsicRegBits(regAddr, RTL8367D_TABLE_TYPE_MASK | RTL8367D_COMMAND_TYPE_MASK, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Read Care Bits */
    regAddr = RTL8367D_REG_TABLE_READ_DATA0;
    tableAddr = (rtk_uint16*)&aclRuleSmi.care_bits;
    for(i = 0; i < RTL8367D_ACLRULETBLEN; i++)
    {
        retVal = rtl8367d_getAsicReg(regAddr, &regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        *tableAddr = regData;

        regAddr ++;
        tableAddr ++;
    }

#ifdef CONFIG_RTL8367D_ASICDRV_TEST
    memcpy(&aclRuleSmi,&Rtl8370sVirtualAclRuleTable[index], sizeof(rtl8367d_aclrulesmi));
#endif

     _rtl8367d_aclRuleStSmi2User(pAclRule, &aclRuleSmi);

    return RT_ERR_OK;
}

static rtk_api_ret_t _rtl8367d_setAclAct(rtk_uint32 index, rtl8367d_acl_act_t* pAclAct)
{
    rtk_uint16 aclActSmi[RTL8367D_ACL_ACT_TABLE_LEN];
    ret_t retVal;
    rtk_uint32 regAddr, regData;
    rtk_uint16* tableAddr;
    rtk_uint32 i;

    if(index > RTL8367D_ACLRULEMAX)
        return RT_ERR_OUT_OF_RANGE;

    memset(aclActSmi, 0x00, sizeof(rtk_uint16) * RTL8367D_ACL_ACT_TABLE_LEN);
     _rtl8367d_aclActStUser2Smi(pAclAct, aclActSmi);

    /* Write ACS_ADR register for data bits */
    regAddr = RTL8367D_REG_TABLE_ACCESS_ADDR;
    regData = index;
    retVal = rtl8367d_setAsicReg(regAddr, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Write Data Bits to ACS_DATA registers */
     tableAddr = aclActSmi;
     regAddr = RTL8367D_REG_TABLE_WRITE_DATA0;

    for(i = 0; i < RTL8367D_ACLACTTBLEN; i++)
    {
        regData = *tableAddr;
        retVal = rtl8367d_setAsicReg(regAddr, regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        regAddr++;
        tableAddr++;
    }

    /* Write ACS_CMD register for care bits*/
    regAddr = RTL8367D_REG_TABLE_ACCESS_CTRL;
    regData = RTL8367D_TABLE_ACCESS_REG_DATA(RTL8367D_TB_OP_WRITE, RTL8367D_TB_TARGET_ACLACT);
    retVal = rtl8367d_setAsicRegBits(regAddr, RTL8367D_TABLE_TYPE_MASK | RTL8367D_COMMAND_TYPE_MASK, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

#ifdef CONFIG_RTL8367D_ASICDRV_TEST
    memcpy(&Rtl8370sVirtualAclActTable[index][0], aclActSmi, sizeof(rtk_uint16) * RTL8367D_ACL_ACT_TABLE_LEN);
#endif

    return RT_ERR_OK;
}

static rtk_api_ret_t _rtl8367d_setAclRule(rtk_uint32 index, rtl8367d_aclrule* pAclRule)
{
    rtl8367d_aclrulesmi aclRuleSmi;
    rtk_uint16* tableAddr;
    rtk_uint32 regAddr;
    rtk_uint32  regData;
    rtk_uint32 i;
    ret_t retVal;

    if(index > RTL8367D_ACLRULEMAX)
        return RT_ERR_OUT_OF_RANGE;

    memset(&aclRuleSmi, 0x00, sizeof(rtl8367d_aclrulesmi));

    _rtl8367d_aclRuleStUser2Smi(pAclRule, &aclRuleSmi);

    /* Write valid bit = 0 */
    regAddr = RTL8367D_REG_TABLE_ACCESS_ADDR;
    regData = RTL8367D_ACLRULETBADDR(RTL8367D_DATABITS, index);
    retVal = rtl8367d_setAsicReg(regAddr,regData);
    if(retVal !=RT_ERR_OK)
        return retVal;

    retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_TABLE_WRITE_DATA0+RTL8367D_ACLRULETBLEN, 0x1, 0);
    if(retVal !=RT_ERR_OK)
        return retVal;

    regAddr = RTL8367D_REG_TABLE_ACCESS_CTRL;
    regData = RTL8367D_TABLE_ACCESS_REG_DATA(RTL8367D_TB_OP_WRITE, RTL8367D_TB_TARGET_ACLRULE);
    retVal = rtl8367d_setAsicReg(regAddr, regData);
    if(retVal !=RT_ERR_OK)
        return retVal;



    /* Write ACS_ADR register */
    regAddr = RTL8367D_REG_TABLE_ACCESS_ADDR;
    regData = RTL8367D_ACLRULETBADDR(RTL8367D_CAREBITS, index);
    retVal = rtl8367d_setAsicReg(regAddr, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Write Care Bits to ACS_DATA registers */
     tableAddr = (rtk_uint16*)&aclRuleSmi.care_bits;
     regAddr = RTL8367D_REG_TABLE_WRITE_DATA0;

    for(i = 0; i < RTL8367D_ACLRULETBLEN; i++)
    {
        regData = *tableAddr;
        retVal = rtl8367d_setAsicReg(regAddr, regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        regAddr++;
        tableAddr++;
    }

    /* Write ACS_CMD register */
    regAddr = RTL8367D_REG_TABLE_ACCESS_CTRL;
    regData = RTL8367D_TABLE_ACCESS_REG_DATA(RTL8367D_TB_OP_WRITE, RTL8367D_TB_TARGET_ACLRULE);
    retVal = rtl8367d_setAsicRegBits(regAddr, RTL8367D_TABLE_TYPE_MASK | RTL8367D_COMMAND_TYPE_MASK,regData);
    if(retVal != RT_ERR_OK)
        return retVal;



    /* Write ACS_ADR register for data bits */
    regAddr = RTL8367D_REG_TABLE_ACCESS_ADDR;
    regData = RTL8367D_ACLRULETBADDR(RTL8367D_DATABITS, index);
    retVal = rtl8367d_setAsicReg(regAddr, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Write Data Bits to ACS_DATA registers */
     tableAddr = (rtk_uint16*)&aclRuleSmi.data_bits;
     regAddr = RTL8367D_REG_TABLE_WRITE_DATA0;

    for(i = 0; i < RTL8367D_ACLRULETBLEN; i++)
    {
        regData = *tableAddr;
        retVal = rtl8367d_setAsicReg(regAddr, regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        regAddr++;
        tableAddr++;
    }

    retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_TABLE_WRITE_DATA0+RTL8367D_ACLRULETBLEN, 0, aclRuleSmi.valid);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Write ACS_CMD register for care bits*/
    regAddr = RTL8367D_REG_TABLE_ACCESS_CTRL;
    regData = RTL8367D_TABLE_ACCESS_REG_DATA(RTL8367D_TB_OP_WRITE, RTL8367D_TB_TARGET_ACLRULE);
    retVal = rtl8367d_setAsicRegBits(regAddr, RTL8367D_TABLE_TYPE_MASK | RTL8367D_COMMAND_TYPE_MASK, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

#ifdef CONFIG_RTL8367D_ASICDRV_TEST
    memcpy(&Rtl8370sVirtualAclRuleTable[index], &aclRuleSmi, sizeof(rtl8367d_aclrulesmi));
#endif

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_cfg_add(rtk_filter_id_t filter_id, rtk_filter_cfg_t* pFilter_cfg, rtk_filter_action_t* pFilter_action, rtk_filter_number_t *ruleNum)
{
    rtk_api_ret_t               retVal;
    rtk_uint32                  careTagData, careTagMask;
    rtk_uint32                  i,actType, ruleId;
    rtk_uint32                  aclActCtrl;
    rtk_uint32                  cpuPort;
    rtk_filter_field_t*         fieldPtr;
    rtl8367d_aclrule            aclRule[RTL8367D_ACLTEMPLATENO];
    rtl8367d_aclrule            tempRule;
    rtl8367d_acl_act_t          aclAct;
    rtk_uint32                  noRulesAdd;
    rtk_uint32                  portmask;
    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(filter_id > RTL8367D_ACLRULEMAX )
        return RT_ERR_ENTRY_INDEX;

    if((NULL == pFilter_cfg) || (NULL == pFilter_action) || (NULL == ruleNum))
        return RT_ERR_NULL_POINTER;

    fieldPtr = pFilter_cfg->fieldHead;

    /* init RULE */
    for(i = 0; i < RTL8367D_ACLTEMPLATENO; i++)
    {
        memset(&aclRule[i], 0, sizeof(rtl8367d_aclrule));

        aclRule[i].data_bits.type= i;
        aclRule[i].care_bits.type= 0x7;
    }

    while(NULL != fieldPtr)
    {
        _rtk_filter_igrAcl_writeDataField(aclRule, fieldPtr);

        fieldPtr = fieldPtr->next;
    }

    /*set care tag mask in User Defined Field 15*/
    /*Follow care tag should not be used while ACL template and User defined fields are fully control by system designer*/
    /*those advanced packet type care tag is used in default template design structure only*/
    careTagData = 0;
    careTagMask = 0;

    for(i = CARE_TAG_TCP; i < CARE_TAG_END; i++)
    {
        if(pFilter_cfg->careTag.tagType[i].mask)
            careTagMask = careTagMask | (1 << (i-CARE_TAG_TCP));

        if(pFilter_cfg->careTag.tagType[i].value)
            careTagData = careTagData | (1 << (i-CARE_TAG_TCP));
    }

    if(careTagData || careTagMask)
    {
        i = 0;
        while(i < RTL8367D_ACLTEMPLATENO)
        {
            if(aclRule[i].valid == 1 && rtl8367D_filter_advanceCaretagField[i][0] == TRUE)
            {

                aclRule[i].data_bits.field[rtl8367D_filter_advanceCaretagField[i][1]] = careTagData & 0xFFFF;
                aclRule[i].care_bits.field[rtl8367D_filter_advanceCaretagField[i][1]] = careTagMask & 0xFFFF;
                break;
            }
            i++;
        }
        /*none of previous used template containing field 15*/
        if(i == RTL8367D_ACLTEMPLATENO)
        {
            i = 0;
            while(i < RTL8367D_ACLTEMPLATENO)
            {
                if(rtl8367D_filter_advanceCaretagField[i][0] == TRUE)
                {
                    aclRule[i].data_bits.field[rtl8367D_filter_advanceCaretagField[i][1]] = careTagData & 0xFFFF;
                    aclRule[i].care_bits.field[rtl8367D_filter_advanceCaretagField[i][1]] = careTagMask & 0xFFFF;
                    aclRule[i].valid = 1;
                    break;
                }
                i++;
            }
        }
    }

    /*Check rule number*/
    noRulesAdd = 0;
    for(i = 0; i < RTL8367D_ACLTEMPLATENO; i++)
    {
        if(1 == aclRule[i].valid)
        {
            noRulesAdd ++;
        }
    }

    *ruleNum = noRulesAdd;

    if((filter_id + noRulesAdd - 1) > RTL8367D_ACLRULEMAX)
    {
        return RT_ERR_ENTRY_INDEX;
    }

    /*set care tag mask in TAG Indicator*/
    careTagData = 0;
    careTagMask = 0;

    for(i = 0; i <= CARE_TAG_IPV6;i++)
    {
        if(0 == pFilter_cfg->careTag.tagType[i].mask )
        {
            careTagMask &=  ~(1 << i);
        }
        else
        {
            careTagMask |= (1 << i);
            if(0 == pFilter_cfg->careTag.tagType[i].value )
                careTagData &= ~(1 << i);
            else
                careTagData |= (1 << i);
        }
    }

    for(i = 0; i < RTL8367D_ACLTEMPLATENO; i++)
    {
        aclRule[i].data_bits.tag_exist = (careTagData) & ACL_RULE_CARETAG_MASK;
        aclRule[i].care_bits.tag_exist = (careTagMask) & ACL_RULE_CARETAG_MASK;
    }

    RTK_CHK_PORTMASK_VALID(&pFilter_cfg->activeport.value);
    RTK_CHK_PORTMASK_VALID(&pFilter_cfg->activeport.mask);

    for(i = 0; i < RTL8367D_ACLTEMPLATENO; i++)
    {
        if(TRUE == aclRule[i].valid)
        {
            if(rtk_switch_portmask_L2P_get(&pFilter_cfg->activeport.value, &portmask) != RT_ERR_OK)
                return RT_ERR_PORT_MASK;

            aclRule[i].data_bits.active_portmsk = portmask;

            if(rtk_switch_portmask_L2P_get(&pFilter_cfg->activeport.mask, &portmask) != RT_ERR_OK)
                return RT_ERR_PORT_MASK;

            aclRule[i].care_bits.active_portmsk = portmask;
        }
    }

    if(pFilter_cfg->invert >= FILTER_INVERT_END )
        return RT_ERR_INPUT;


    /*Last action gets high priority if actions are the same*/
    memset(&aclAct, 0, sizeof(rtl8367d_acl_act_t));
    aclActCtrl = 0;
    for(actType = 0; actType < FILTER_ENACT_END; actType ++)
    {
        if(pFilter_action->actEnable[actType])
        {
            switch (actType)
            {
            case FILTER_ENACT_CVLAN_INGRESS:
                if(pFilter_action->filterCvlanVid > RTL8367D_VIDMAX)
                    return RT_ERR_INPUT;
                aclAct.cact = FILTER_ENACT_CVLAN_TYPE(actType);
                aclAct.cvidx_cact = pFilter_action->filterCvlanVid;

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_TAGONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_VLANONLY;
                }

                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
            case FILTER_ENACT_CVLAN_EGRESS:
                if(pFilter_action->filterCvlanVid > RTL8367D_VIDMAX)
                    return RT_ERR_INPUT;

                aclAct.cact = FILTER_ENACT_CVLAN_TYPE(actType);
                aclAct.cvidx_cact = pFilter_action->filterCvlanVid;

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_TAGONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_VLANONLY;
                }

                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
             case FILTER_ENACT_CVLAN_SVID:

                aclAct.cact = FILTER_ENACT_CVLAN_TYPE(actType);

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_TAGONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_VLANONLY;
                }

                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
             case FILTER_ENACT_POLICING_1:
                if(pFilter_action->filterPolicingIdx[1] >= ((RTK_MAX_METER_ID + 1) + RTL8367D_MAX_LOG_CNT_NUM))
                    return RT_ERR_INPUT;

                aclAct.cact = FILTER_ENACT_CVLAN_TYPE(actType);
                aclAct.cvidx_cact = pFilter_action->filterPolicingIdx[1];

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_TAGONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_VLANONLY;
                }

                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;

            case FILTER_ENACT_SVLAN_INGRESS:
            case FILTER_ENACT_SVLAN_EGRESS:
                aclAct.sact = FILTER_ENACT_SVLAN_TYPE(actType);
                aclAct.svidx_sact = pFilter_action->filterSvlanVid;
                aclActCtrl |= FILTER_ENACT_SVLAN_MASK;
                break;
            case FILTER_ENACT_SVLAN_CVID:

                aclAct.sact = FILTER_ENACT_SVLAN_TYPE(actType);
                aclActCtrl |= FILTER_ENACT_SVLAN_MASK;
                break;
            case FILTER_ENACT_POLICING_2:
                if(pFilter_action->filterPolicingIdx[2] >= ((RTK_MAX_METER_ID + 1) + RTL8367D_MAX_LOG_CNT_NUM))
                    return RT_ERR_INPUT;

                aclAct.sact = FILTER_ENACT_SVLAN_TYPE(actType);
                aclAct.svidx_sact = pFilter_action->filterPolicingIdx[2];
                aclActCtrl |= FILTER_ENACT_SVLAN_MASK;
                break;
            case FILTER_ENACT_POLICING_0:
                if(pFilter_action->filterPolicingIdx[0] >= ((RTK_MAX_METER_ID + 1) + RTL8367D_MAX_LOG_CNT_NUM))
                    return RT_ERR_INPUT;

                aclAct.aclmeteridx = pFilter_action->filterPolicingIdx[0];
                aclActCtrl |= FILTER_ENACT_POLICING_MASK;
                break;
            case FILTER_ENACT_PRIORITY:
            case FILTER_ENACT_1P_REMARK:
                if(pFilter_action->filterPriority > RTL8367D_PRIMAX)
                    return RT_ERR_INPUT;

                aclAct.priact = FILTER_ENACT_PRI_TYPE(actType);
                aclAct.pridx = pFilter_action->filterPriority;
                aclActCtrl |= FILTER_ENACT_PRIORITY_MASK;
                break;
            case FILTER_ENACT_DSCP_REMARK:
                if(pFilter_action->filterPriority > RTL8367D_DSCPMAX)
                    return RT_ERR_INPUT;

                aclAct.priact = FILTER_ENACT_PRI_TYPE(actType);
                aclAct.pridx = pFilter_action->filterPriority;
                aclActCtrl |= FILTER_ENACT_PRIORITY_MASK;
                break;
            case FILTER_ENACT_POLICING_3:
                if(pFilter_action->filterPriority >= ((RTK_MAX_METER_ID + 1) + RTL8367D_MAX_LOG_CNT_NUM))
                    return RT_ERR_INPUT;

                aclAct.priact = FILTER_ENACT_PRI_TYPE(actType);
                aclAct.pridx = pFilter_action->filterPolicingIdx[3];
                aclActCtrl |= FILTER_ENACT_PRIORITY_MASK;
                break;
            case FILTER_ENACT_DROP:

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(FILTER_ENACT_REDIRECT);
                aclAct.fwdact_ext = FALSE;

                aclAct.fwdpmask = 0;
                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;
            case FILTER_ENACT_REDIRECT:
                RTK_CHK_PORTMASK_VALID(&pFilter_action->filterPortmask);

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(actType);
                aclAct.fwdact_ext = FALSE;

                if(rtk_switch_portmask_L2P_get(&pFilter_action->filterPortmask, &portmask) != RT_ERR_OK)
                    return RT_ERR_PORT_MASK;
                aclAct.fwdpmask = portmask;

                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;

            case FILTER_ENACT_ADD_DSTPORT:
                RTK_CHK_PORTMASK_VALID(&pFilter_action->filterPortmask);

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(actType);
                aclAct.fwdact_ext = FALSE;

                if(rtk_switch_portmask_L2P_get(&pFilter_action->filterPortmask, &portmask) != RT_ERR_OK)
                    return RT_ERR_PORT_MASK;
                aclAct.fwdpmask = portmask;

                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;
            case FILTER_ENACT_MIRROR:
                RTK_CHK_PORTMASK_VALID(&pFilter_action->filterPortmask);

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(actType);
                aclAct.cact_ext = FALSE;

                if(rtk_switch_portmask_L2P_get(&pFilter_action->filterPortmask, &portmask) != RT_ERR_OK)
                    return RT_ERR_PORT_MASK;
                aclAct.fwdpmask = portmask;

                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;
            case FILTER_ENACT_TRAP_CPU:

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(actType);
                aclAct.fwdact_ext = FALSE;

                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;
            case FILTER_ENACT_COPY_CPU:
                if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_CPU_CTRL, RTL8367D_CPU_TRAP_PORT_MASK, &cpuPort)) != RT_ERR_OK)
                    return retVal;

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(FILTER_ENACT_MIRROR);
                aclAct.fwdact_ext = FALSE;

                aclAct.fwdpmask = 1 << cpuPort;
                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;
            case FILTER_ENACT_ISOLATION:
                RTK_CHK_PORTMASK_VALID(&pFilter_action->filterPortmask);

                aclAct.fwdact_ext = TRUE;

                if(rtk_switch_portmask_L2P_get(&pFilter_action->filterPortmask, &portmask) != RT_ERR_OK)
                    return RT_ERR_PORT_MASK;
                aclAct.fwdpmask = portmask;

                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;

            case FILTER_ENACT_INTERRUPT:

                aclAct.aclint = TRUE;
                aclActCtrl |= FILTER_ENACT_INTGPIO_MASK;
                break;
            case FILTER_ENACT_GPO:
                if((pFilter_action->filterPin > RTL8367D_ACLGPIOPINNO) || ((pFilter_action->filterPin == 0)))
                    return RT_ERR_INPUT;
                aclAct.gpio_en = TRUE;
                aclAct.gpio_pin = pFilter_action->filterPin;
                aclActCtrl |= FILTER_ENACT_INTGPIO_MASK;
                break;
             case FILTER_ENACT_EGRESSCTAG_TAG:

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_VLANONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_TAGONLY;
                }
                aclAct.tag_fmt = FILTER_CTAGFMT_TAG;
                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
             case FILTER_ENACT_EGRESSCTAG_UNTAG:

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_VLANONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_TAGONLY;
                }
                aclAct.tag_fmt = FILTER_CTAGFMT_UNTAG;
                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
             case FILTER_ENACT_EGRESSCTAG_KEEP:

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_VLANONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_TAGONLY;
                }
                aclAct.tag_fmt = FILTER_CTAGFMT_KEEP;
                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
             case FILTER_ENACT_EGRESSCTAG_KEEPAND1PRMK:

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_VLANONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_TAGONLY;
                }
                aclAct.tag_fmt = FILTER_CTAGFMT_KEEP1PRMK;
                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
           default:
                return RT_ERR_FILTER_INACL_ACT_NOT_SUPPORT;
            }
        }
    }


    /*check if free ACL rules are enough*/
    for(i = filter_id; i < (filter_id + noRulesAdd); i++)
    {
        if((retVal = _rtl8367d_getAclRule(i, &tempRule)) != RT_ERR_OK )
            return retVal;

        if(tempRule.valid == TRUE)
        {
            return RT_ERR_TBL_FULL;
        }
    }

    ruleId = 0;
    for(i = 0; i < RTL8367D_ACLTEMPLATENO; i++)
    {
        if(aclRule[i].valid == TRUE)
        {
            /* write ACL action control */
            if((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_ACL_ACTION_CTRL0 + ((filter_id + ruleId) >> 1), (0x3F << (((filter_id + ruleId) & 0x1) << 3)), aclActCtrl)) != RT_ERR_OK )
                return retVal;
            /* write ACL action */
            if((retVal = _rtl8367d_setAclAct(filter_id + ruleId, &aclAct)) != RT_ERR_OK )
                return retVal;

            /* write ACL not */
            if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_ACL_ACTION_CTRL0 + ((filter_id + ruleId) >> 1), (6 + (((filter_id + ruleId) & 0x1) << 3)), pFilter_cfg->invert)) != RT_ERR_OK )
                return retVal;
            /* write ACL rule */
            if((retVal = _rtl8367d_setAclRule(filter_id + ruleId, &aclRule[i])) != RT_ERR_OK )
                return retVal;

            /* only the first rule will be written with input action control, aclActCtrl of other rules will be zero */
            aclActCtrl = 0;
            memset(&aclAct, 0, sizeof(rtl8367d_acl_act_t));

            ruleId ++;
        }
    }

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_cfg_del(rtk_filter_id_t filter_id)
{
    rtl8367d_aclrule initRule;
    rtl8367d_acl_act_t  initAct;
    rtk_api_ret_t ret;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(filter_id > RTL8367D_ACLRULEMAX )
        return RT_ERR_FILTER_ENTRYIDX;

    memset(&initRule, 0, sizeof(rtl8367d_aclrule));
    memset(&initAct, 0, sizeof(rtl8367d_acl_act_t));

    if((ret = _rtl8367d_setAclRule(filter_id, &initRule)) != RT_ERR_OK)
        return ret;


    if((ret = rtl8367d_setAsicRegBits(RTL8367D_REG_ACL_ACTION_CTRL0 + (filter_id >> 1), (0x3F << ((filter_id & 0x1) << 3)), FILTER_ENACT_INIT_MASK))!= RT_ERR_OK)
        return ret;
    if((ret = _rtl8367d_setAclAct(filter_id, &initAct)) != RT_ERR_OK)
        return ret;
    if((ret = rtl8367d_setAsicRegBit(RTL8367D_REG_ACL_ACTION_CTRL0 + (filter_id >> 1), (6 + ((filter_id & 0x1) << 3)), DISABLED)) != RT_ERR_OK )
        return ret;

    return RT_ERR_OK;
}

static rtk_api_ret_t _rtl8367d_getAclAct(rtk_uint32 index, rtl8367d_acl_act_t *pAclAct)
{
    rtk_uint16 aclActSmi[RTL8367D_ACL_ACT_TABLE_LEN];
    ret_t retVal;
    rtk_uint32 regAddr, regData;
    rtk_uint16 *tableAddr;
    rtk_uint32 i;

    if(index > RTL8367D_ACLRULEMAX)
        return RT_ERR_OUT_OF_RANGE;

    memset(aclActSmi, 0x00, sizeof(rtk_uint16) * RTL8367D_ACL_ACT_TABLE_LEN);

    /* Write ACS_ADR register for data bits */
    regAddr = RTL8367D_REG_TABLE_ACCESS_ADDR;
    regData = index;
    retVal = rtl8367d_setAsicReg(regAddr, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Write ACS_CMD register */
    regAddr = RTL8367D_REG_TABLE_ACCESS_CTRL;
    regData = RTL8367D_TABLE_ACCESS_REG_DATA(RTL8367D_TB_OP_READ, RTL8367D_TB_TARGET_ACLACT);
    retVal = rtl8367d_setAsicRegBits(regAddr, RTL8367D_TABLE_TYPE_MASK | RTL8367D_COMMAND_TYPE_MASK, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Read Data Bits */
    regAddr = RTL8367D_REG_TABLE_READ_DATA0;
    tableAddr = aclActSmi;
    for(i = 0; i < RTL8367D_ACLACTTBLEN; i++)
    {
        retVal = rtl8367d_getAsicReg(regAddr, &regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        *tableAddr = regData;

        regAddr ++;
        tableAddr ++;
    }

#ifdef CONFIG_RTL8367D_ASICDRV_TEST
    memcpy(aclActSmi, &Rtl8370sVirtualAclActTable[index][0], sizeof(rtk_uint16) * RTL8367D_ACL_ACT_TABLE_LEN);
#endif

     _rtl8367d_aclActStSmi2User(pAclAct, aclActSmi);

    return RT_ERR_OK;
}

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
 *      This function get configuration from ASIC.
 */
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_cfg_get(rtk_filter_id_t filter_id, rtk_filter_cfg_raw_t *pFilter_cfg, rtk_filter_action_t *pAction)
{
    rtk_api_ret_t               retVal;
    rtk_uint32                  i, tmp;
    rtl8367d_aclrule            aclRule;
    rtl8367d_acl_act_t          aclAct;
    rtk_uint32                  cpuPort;
    rtl8367d_acltemplate_t      type;
    rtk_uint32                  phyPmask;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pFilter_cfg || NULL == pAction)
        return RT_ERR_NULL_POINTER;

    if(filter_id > RTL8367D_ACLRULEMAX)
        return RT_ERR_ENTRY_INDEX;

    if ((retVal = _rtl8367d_getAclRule(filter_id, &aclRule)) != RT_ERR_OK)
        return retVal;

    /* Check valid */
    if(aclRule.valid == 0)
    {
        pFilter_cfg->valid = DISABLED;
        return RT_ERR_OK;
    }

    phyPmask = aclRule.data_bits.active_portmsk;
    if(rtk_switch_portmask_P2L_get(phyPmask,&(pFilter_cfg->activeport.value)) != RT_ERR_OK)
        return RT_ERR_FAILED;

    phyPmask = aclRule.care_bits.active_portmsk;
    if(rtk_switch_portmask_P2L_get(phyPmask,&(pFilter_cfg->activeport.mask)) != RT_ERR_OK)
        return RT_ERR_FAILED;

    for(i = 0; i <= CARE_TAG_IPV6; i++)
    {
        if(aclRule.data_bits.tag_exist & (1 << i))
            pFilter_cfg->careTag.tagType[i].value = 1;
        else
            pFilter_cfg->careTag.tagType[i].value = 0;

        if (aclRule.care_bits.tag_exist & (1 << i))
            pFilter_cfg->careTag.tagType[i].mask = 1;
        else
            pFilter_cfg->careTag.tagType[i].mask = 0;
    }

    if(rtl8367D_filter_advanceCaretagField[aclRule.data_bits.type][0] == TRUE)
    {
        /* Advanced Care tag setting */
        for(i = CARE_TAG_TCP; i < CARE_TAG_END; i++)
        {
            if(aclRule.data_bits.field[rtl8367D_filter_advanceCaretagField[aclRule.data_bits.type][1]] & (0x0001 << (i-CARE_TAG_TCP)) )
                pFilter_cfg->careTag.tagType[i].value = 1;
            else
                pFilter_cfg->careTag.tagType[i].value = 0;

            if(aclRule.care_bits.field[rtl8367D_filter_advanceCaretagField[aclRule.care_bits.type][1]] & (0x0001 << (i-CARE_TAG_TCP)) )
                pFilter_cfg->careTag.tagType[i].mask = 1;
            else
                pFilter_cfg->careTag.tagType[i].mask = 0;
        }
    }

    for(i = 0; i < RTL8367D_ACLRULEFIELDNO; i++)
    {
        pFilter_cfg->careFieldRaw[i] = aclRule.care_bits.field[i];
        pFilter_cfg->dataFieldRaw[i] = aclRule.data_bits.field[i];
    }

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_ACL_ACTION_CTRL0 + (filter_id >> 1), (6 + ((filter_id & 0x1) << 3)), &tmp))!= RT_ERR_OK)
        return retVal;

    pFilter_cfg->invert = tmp;

    pFilter_cfg->valid = aclRule.valid;

    memset(pAction, 0, sizeof(rtk_filter_action_t));
    memset(&aclAct, 0, sizeof(rtl8367d_acl_act_t));

    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_ACL_ACTION_CTRL0 + (filter_id >> 1), (0x3F << ((filter_id & 0x1) << 3)), &tmp))!= RT_ERR_OK)
        return retVal;

    if ((retVal = _rtl8367d_getAclAct(filter_id, &aclAct)) != RT_ERR_OK)
        return retVal;

    if(tmp & FILTER_ENACT_FWD_MASK)
    {
        if(TRUE == aclAct.fwdact_ext)
        {
            pAction->actEnable[FILTER_ENACT_ISOLATION] = TRUE;

            phyPmask = aclAct.fwdpmask;
            if(rtk_switch_portmask_P2L_get(phyPmask,&(pAction->filterPortmask)) != RT_ERR_OK)
                return RT_ERR_FAILED;
        }
        else if(aclAct.fwdact == RTL8367D_ACL_FWD_TRAP)
        {
            pAction->actEnable[FILTER_ENACT_TRAP_CPU] = TRUE;
        }
        else if (aclAct.fwdact == RTL8367D_ACL_FWD_MIRRORFUNTION )
        {
            pAction->actEnable[FILTER_ENACT_MIRROR] = TRUE;

            phyPmask = aclAct.fwdpmask;
            if(rtk_switch_portmask_P2L_get(phyPmask,&(pAction->filterPortmask)) != RT_ERR_OK)
                return RT_ERR_FAILED;
        }
        else if (aclAct.fwdact == RTL8367D_ACL_FWD_REDIRECT)
        {
            if(aclAct.fwdpmask == 0 )
                pAction->actEnable[FILTER_ENACT_DROP] = TRUE;
            else
            {
                pAction->actEnable[FILTER_ENACT_REDIRECT] = TRUE;

                phyPmask = aclAct.fwdpmask;
                if(rtk_switch_portmask_P2L_get(phyPmask,&(pAction->filterPortmask)) != RT_ERR_OK)
                    return RT_ERR_FAILED;
            }
        }
        else if (aclAct.fwdact == RTL8367D_ACL_FWD_MIRROR)
        {
            if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_CPU_CTRL, RTL8367D_CPU_TRAP_PORT_MASK, &cpuPort)) != RT_ERR_OK)
                return retVal;
            if (aclAct.fwdpmask == (1UL << cpuPort))
            {
                pAction->actEnable[FILTER_ENACT_COPY_CPU] = TRUE;
            }
            else
            {
                pAction->actEnable[FILTER_ENACT_ADD_DSTPORT] = TRUE;

                phyPmask = aclAct.fwdpmask;
                if(rtk_switch_portmask_P2L_get(phyPmask,&(pAction->filterPortmask)) != RT_ERR_OK)
                    return RT_ERR_FAILED;
            }
        }
        else
        {
            return RT_ERR_FAILED;
        }
    }

    if(tmp & FILTER_ENACT_POLICING_MASK)
    {
        pAction->actEnable[FILTER_ENACT_POLICING_0] = TRUE;
        pAction->filterPolicingIdx[0] = aclAct.aclmeteridx;
    }

    if(tmp & FILTER_ENACT_PRIORITY_MASK)
    {
        if(aclAct.priact == FILTER_ENACT_PRI_TYPE(FILTER_ENACT_PRIORITY))
        {
            pAction->actEnable[FILTER_ENACT_PRIORITY] = TRUE;
            pAction->filterPriority = aclAct.pridx;
        }
        else if(aclAct.priact == FILTER_ENACT_PRI_TYPE(FILTER_ENACT_1P_REMARK))
        {
            pAction->actEnable[FILTER_ENACT_1P_REMARK] = TRUE;
            pAction->filterPriority = aclAct.pridx;
        }
        else if(aclAct.priact == FILTER_ENACT_PRI_TYPE(FILTER_ENACT_DSCP_REMARK))
        {
            pAction->actEnable[FILTER_ENACT_DSCP_REMARK] = TRUE;
            pAction->filterPriority = aclAct.pridx;
        }
        else if(aclAct.priact == FILTER_ENACT_PRI_TYPE(FILTER_ENACT_POLICING_3))
        {
            pAction->actEnable[FILTER_ENACT_POLICING_3] = TRUE;
            pAction->filterPolicingIdx[3]  = aclAct.pridx;
        }
    }

    if(tmp & FILTER_ENACT_SVLAN_MASK)
    {
        if(aclAct.sact == FILTER_ENACT_SVLAN_TYPE(FILTER_ENACT_SVLAN_INGRESS))
        {
            pAction->actEnable[FILTER_ENACT_SVLAN_INGRESS] = TRUE;
            pAction->filterSvlanIdx = aclAct.svidx_sact;
            pAction->filterSvlanVid = aclAct.svidx_sact;
        }
        else if(aclAct.sact == FILTER_ENACT_SVLAN_TYPE(FILTER_ENACT_SVLAN_EGRESS))
        {
            pAction->actEnable[FILTER_ENACT_SVLAN_EGRESS] = TRUE;
            pAction->filterSvlanIdx = aclAct.svidx_sact;
            pAction->filterSvlanVid = aclAct.svidx_sact;
        }
        else if(aclAct.sact == FILTER_ENACT_SVLAN_TYPE(FILTER_ENACT_SVLAN_CVID))
            pAction->actEnable[FILTER_ENACT_SVLAN_CVID] = TRUE;
        else if(aclAct.sact == FILTER_ENACT_SVLAN_TYPE(FILTER_ENACT_POLICING_2))
        {
            pAction->actEnable[FILTER_ENACT_POLICING_2] = TRUE;
            pAction->filterPolicingIdx[2]  = aclAct.svidx_sact;
        }
    }


    if(tmp & FILTER_ENACT_CVLAN_MASK)
    {
        if(FILTER_ENACT_CACTEXT_TAGONLY == aclAct.cact_ext ||
            FILTER_ENACT_CACTEXT_BOTHVLANTAG == aclAct.cact_ext )
        {
            if(FILTER_CTAGFMT_UNTAG == aclAct.tag_fmt)
            {
                pAction->actEnable[FILTER_ENACT_EGRESSCTAG_UNTAG] = TRUE;
            }
            else if(FILTER_CTAGFMT_TAG == aclAct.tag_fmt)
            {
                pAction->actEnable[FILTER_ENACT_EGRESSCTAG_TAG] = TRUE;
            }
            else if(FILTER_CTAGFMT_KEEP == aclAct.tag_fmt)
            {
                pAction->actEnable[FILTER_ENACT_EGRESSCTAG_KEEP] = TRUE;
            }
             else if(FILTER_CTAGFMT_KEEP1PRMK== aclAct.tag_fmt)
            {
                pAction->actEnable[FILTER_ENACT_EGRESSCTAG_KEEPAND1PRMK] = TRUE;
            }

        }

        if(FILTER_ENACT_CACTEXT_VLANONLY == aclAct.cact_ext ||
            FILTER_ENACT_CACTEXT_BOTHVLANTAG == aclAct.cact_ext )
        {
            if(aclAct.cact == FILTER_ENACT_CVLAN_TYPE(FILTER_ENACT_CVLAN_INGRESS))
            {
                pAction->actEnable[FILTER_ENACT_CVLAN_INGRESS] = TRUE;
                pAction->filterCvlanIdx  = aclAct.cvidx_cact;
                pAction->filterCvlanVid  = aclAct.cvidx_cact;
            }
            else if(aclAct.cact == FILTER_ENACT_CVLAN_TYPE(FILTER_ENACT_CVLAN_EGRESS))
            {
                pAction->actEnable[FILTER_ENACT_CVLAN_EGRESS] = TRUE;
                pAction->filterCvlanIdx  = aclAct.cvidx_cact;
                pAction->filterCvlanVid  = aclAct.cvidx_cact;
            }
            else if(aclAct.cact == FILTER_ENACT_CVLAN_TYPE(FILTER_ENACT_CVLAN_SVID))
            {
                pAction->actEnable[FILTER_ENACT_CVLAN_SVID] = TRUE;
            }
            else if(aclAct.cact == FILTER_ENACT_CVLAN_TYPE(FILTER_ENACT_POLICING_1))
            {
                pAction->actEnable[FILTER_ENACT_POLICING_1] = TRUE;
                pAction->filterPolicingIdx[1]  = aclAct.cvidx_cact;
            }
        }
    }

    if(tmp & FILTER_ENACT_INTGPIO_MASK)
    {
        if(TRUE == aclAct.aclint)
        {
            pAction->actEnable[FILTER_ENACT_INTERRUPT] = TRUE;
        }

        if(TRUE == aclAct.gpio_en)
        {
            pAction->actEnable[FILTER_ENACT_GPO] = TRUE;
            pAction->filterPin = aclAct.gpio_pin;
        }
    }

    /* Get field type of RAW data */
    if ((retVal = _rtl8367d_getAsicAclTemplate(aclRule.data_bits.type, &type))!= RT_ERR_OK)
        return retVal;

    for(i = 0; i < RTL8367D_ACLRULEFIELDNO; i++)
    {
        pFilter_cfg->fieldRawType[i] = type.field[i];
    }/* end of for(i...) */

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_unmatchAction_set(rtk_port_t port, rtk_filter_unmatch_action_t action)
{
    rtk_api_ret_t ret;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check port valid */
    RTK_CHK_PORT_VALID(port);

    if(action >= FILTER_UNMATCH_END)
        return RT_ERR_INPUT;

    if((ret = rtl8367d_setAsicRegBit(RTL8367D_REG_ACL_UNMATCH_PERMIT, rtk_switch_port_L2P_get(port), action)) != RT_ERR_OK)
       return ret;

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_unmatchAction_get(rtk_port_t port, rtk_filter_unmatch_action_t* pAction)
{
    rtk_api_ret_t ret;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pAction)
        return RT_ERR_NULL_POINTER;

    /* Check port valid */
    RTK_CHK_PORT_VALID(port);

    if((ret = rtl8367d_getAsicRegBit(RTL8367D_REG_ACL_UNMATCH_PERMIT, rtk_switch_port_L2P_get(port), pAction)) != RT_ERR_OK)
       return ret;

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_state_set(rtk_port_t port, rtk_filter_state_t state)
{
    rtk_api_ret_t ret;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check port valid */
    RTK_CHK_PORT_VALID(port);

    if(state >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if((ret = rtl8367d_setAsicRegBit(RTL8367D_REG_ACL_ENABLE, rtk_switch_port_L2P_get(port), state)) != RT_ERR_OK)
       return ret;

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_state_get(rtk_port_t port, rtk_filter_state_t* pState)
{
    rtk_api_ret_t ret;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pState)
        return RT_ERR_NULL_POINTER;

    /* Check port valid */
    RTK_CHK_PORT_VALID(port);

    if((ret = rtl8367d_getAsicRegBit(RTL8367D_REG_ACL_ENABLE, rtk_switch_port_L2P_get(port), pState)) != RT_ERR_OK)
       return ret;

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_template_set(rtk_filter_template_t *aclTemplate)
{
    rtk_api_ret_t retVal;
    rtk_uint32 idxField;
    rtl8367d_acltemplate_t aclType;
    rtk_uint32 i;
    rtk_uint32 regAddr, regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(aclTemplate->index >= RTK_MAX_NUM_OF_FILTER_TYPE)
        return RT_ERR_INPUT;

    for(idxField = 0; idxField < RTK_MAX_NUM_OF_FILTER_FIELD; idxField++)
    {
        if(aclTemplate->fieldType[idxField] < FILTER_FIELD_RAW_DMAC_15_0 ||
            (aclTemplate->fieldType[idxField] > FILTER_FIELD_RAW_CTAG && aclTemplate->fieldType[idxField] < FILTER_FIELD_RAW_IPV4_SIP_15_0 ) ||
            (aclTemplate->fieldType[idxField] > FILTER_FIELD_RAW_IPV4_DIP_31_16 && aclTemplate->fieldType[idxField] < FILTER_FIELD_RAW_IPV6_SIP_15_0 ) ||
            (aclTemplate->fieldType[idxField] > FILTER_FIELD_RAW_FIELD_VALID && aclTemplate->fieldType[idxField] < FILTER_FIELD_RAW_FIELD_SELECT00 ) ||
            aclTemplate->fieldType[idxField] >= FILTER_FIELD_RAW_END)
        {
            return RT_ERR_INPUT;
        }
    }

    for(idxField = 0; idxField < RTK_MAX_NUM_OF_FILTER_FIELD; idxField++)
    {
        aclType.field[idxField] = aclTemplate->fieldType[idxField];
    }

    regAddr = (RTL8367D_REG_ACL_RULE_TEMPLATE0_CTRL0 + aclTemplate->index * 0x4);

    for(i = 0; i < (RTL8367D_ACLRULEFIELDNO/2); i++)
    {
        regData = aclType.field[i*2+1];
        regData = regData << 8 | aclType.field[i*2];

        if((retVal = rtl8367d_setAsicReg(regAddr + i, regData)) != RT_ERR_OK)
           return retVal;
    }

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_template_get(rtk_filter_template_t *aclTemplate)
{
    rtk_api_ret_t ret;
    rtk_uint32 idxField;
    rtl8367d_acltemplate_t aclType;
    rtk_uint32 i;
    rtk_uint32 regData, regAddr;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == aclTemplate)
        return RT_ERR_NULL_POINTER;

    if(aclTemplate->index >= RTK_MAX_NUM_OF_FILTER_TYPE)
        return RT_ERR_INPUT;

    regAddr = (RTL8367D_REG_ACL_RULE_TEMPLATE0_CTRL0 + aclTemplate->index * 0x4);

    for(i = 0; i < (RTL8367D_ACLRULEFIELDNO/2); i++)
    {
        if((ret = rtl8367d_getAsicReg(regAddr + i,&regData)) != RT_ERR_OK)
           return ret;

        aclType.field[i*2] = regData & 0xFF;
        aclType.field[i*2 + 1] = (regData >> 8) & 0xFF;
    }

    for(idxField = 0; idxField < RTK_MAX_NUM_OF_FILTER_FIELD; idxField ++)
    {
        aclTemplate->fieldType[idxField] = aclType.field[idxField];
    }

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_field_sel_set(rtk_uint32 index, rtk_field_sel_t format, rtk_uint32 offset)
{
    rtk_api_ret_t ret;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(index >= RTL8367D_FIELDSEL_FORMAT_NUMBER)
        return RT_ERR_OUT_OF_RANGE;

    if(format >= FORMAT_END)
        return RT_ERR_OUT_OF_RANGE;

    if(offset > RTL8367D_FIELDSEL_MAX_OFFSET)
        return RT_ERR_OUT_OF_RANGE;

    regData = ((((rtk_uint32)format << RTL8367D_FIELD_SELECTOR0_FORMAT_OFFSET) & RTL8367D_FIELD_SELECTOR0_FORMAT_MASK ) |
               ((offset << RTL8367D_FIELD_SELECTOR0_OFFSET_OFFSET) & RTL8367D_FIELD_SELECTOR0_OFFSET_MASK ));

    if((ret = rtl8367d_setAsicReg((RTL8367D_REG_FIELD_SELECTOR0 + index), regData)) != RT_ERR_OK)
       return ret;

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_igrAcl_field_sel_get(rtk_uint32 index, rtk_field_sel_t *pFormat, rtk_uint32 *pOffset)
{
    rtk_api_ret_t ret;
    rtk_uint32 regData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pFormat || NULL == pOffset)
        return RT_ERR_NULL_POINTER;

    if(index >= RTL8367D_FIELDSEL_FORMAT_NUMBER)
        return RT_ERR_OUT_OF_RANGE;

    if((ret = rtl8367d_getAsicReg(RTL8367D_REG_FIELD_SELECTOR0 + index, &regData)) != RT_ERR_OK)
       return ret;

    *pFormat    = ((regData & RTL8367D_FIELD_SELECTOR0_FORMAT_MASK) >> RTL8367D_FIELD_SELECTOR0_FORMAT_OFFSET);
    *pOffset    = ((regData & RTL8367D_FIELD_SELECTOR0_OFFSET_MASK) >> RTL8367D_FIELD_SELECTOR0_OFFSET_OFFSET);

    return RT_ERR_OK;
}

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
rtk_api_ret_t dal_rtl8367d_filter_iprange_set(rtk_uint32 index, rtk_filter_iprange_t type, ipaddr_t upperIp, ipaddr_t lowerIp)
{
    rtk_api_ret_t ret;
    rtk_uint32 regData;
    ipaddr_t ipData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(index > RTL8367D_ACLRANGEMAX)
        return RT_ERR_OUT_OF_RANGE;

    if(type >= IPRANGE_END)
        return RT_ERR_OUT_OF_RANGE;

    if(lowerIp > upperIp)
        return RT_ERR_INPUT;

    if((ret = rtl8367d_setAsicRegBits(RTL8367D_REG_ACL_IP_RANGE_ENTRY0_CTRL4 + index*5, RTL8367D_ACL_IP_RANGE_ENTRY0_CTRL4_MASK, type)) != RT_ERR_OK)
       return ret;

    ipData = upperIp;

    regData = ipData & 0xFFFF;
    if((ret = rtl8367d_setAsicReg(RTL8367D_REG_ACL_IP_RANGE_ENTRY0_CTRL2 + index*5, regData)) != RT_ERR_OK)
       return ret;

    regData = (ipData>>16) & 0xFFFF;
    if((ret = rtl8367d_setAsicReg(RTL8367D_REG_ACL_IP_RANGE_ENTRY0_CTRL3 + index*5, regData)) != RT_ERR_OK)
       return ret;

    ipData = lowerIp;

    regData = ipData & 0xFFFF;
    if((ret = rtl8367d_setAsicReg(RTL8367D_REG_ACL_IP_RANGE_ENTRY0_CTRL0 + index*5, regData)) != RT_ERR_OK)
       return ret;

    regData = (ipData>>16) & 0xFFFF;
    if((ret = rtl8367d_setAsicReg(RTL8367D_REG_ACL_IP_RANGE_ENTRY0_CTRL1 + index*5, regData)) != RT_ERR_OK)
       return ret;

    return RT_ERR_OK;
}

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
 *      None.
 */
rtk_api_ret_t dal_rtl8367d_filter_iprange_get(rtk_uint32 index, rtk_filter_iprange_t *pType, ipaddr_t *pUpperIp, ipaddr_t *pLowerIp)
{
    rtk_api_ret_t ret;
    rtk_uint32 regData;
    ipaddr_t ipData;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if((NULL == pType) || (NULL == pUpperIp) || (NULL == pLowerIp))
        return RT_ERR_NULL_POINTER;

    if(index > RTL8367D_ACLRANGEMAX)
        return RT_ERR_OUT_OF_RANGE;

    if((ret = rtl8367d_getAsicRegBits(RTL8367D_REG_ACL_IP_RANGE_ENTRY0_CTRL4 + index*5, RTL8367D_ACL_IP_RANGE_ENTRY0_CTRL4_MASK, pType)) != RT_ERR_OK)
       return ret;

    if((ret = rtl8367d_getAsicReg(RTL8367D_REG_ACL_IP_RANGE_ENTRY0_CTRL2 + index*5, &regData)) != RT_ERR_OK)
       return ret;

    ipData = regData;

    if((ret = rtl8367d_getAsicReg(RTL8367D_REG_ACL_IP_RANGE_ENTRY0_CTRL3 + index*5, &regData)) != RT_ERR_OK)
       return ret;

    ipData = (regData <<16) | ipData;
    *pUpperIp = ipData;

    if((ret = rtl8367d_getAsicReg(RTL8367D_REG_ACL_IP_RANGE_ENTRY0_CTRL0 + index*5, &regData)) != RT_ERR_OK)
       return ret;

    ipData = regData;

    if((ret = rtl8367d_getAsicReg(RTL8367D_REG_ACL_IP_RANGE_ENTRY0_CTRL1 + index*5, &regData)) != RT_ERR_OK)
       return ret;

    ipData = (regData << 16) | ipData;
    *pLowerIp = ipData;

    return RT_ERR_OK;
}

