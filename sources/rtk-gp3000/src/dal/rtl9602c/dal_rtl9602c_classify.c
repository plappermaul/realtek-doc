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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of Classifyication API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) classfication rule add/delete/get
 */



/*
 * Include Files
 */
#include <rtk/port.h>
#include <rtk/classify.h>
#include <hal/mac/reg.h>
#include <hal/mac/drv.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <dal/rtl9602c/dal_rtl9602c_classify.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32    classify_init = INIT_NOT_COMPLETED;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
static int
_dal_rtl9602c_classsify_patternIdx_get(uint32 entryIdx, uint32 *patternIdx)
{
    int32 ret;
    uint32 p0EntryNum, p1EntryNum;

    if ((ret = reg_field_read(RTL9602C_CF_CFGr, RTL9602C_CF_PATTERN1_NUMf, &p1EntryNum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
        return ret;
    }

    p0EntryNum = RTL9602C_CF_PATTERN0_ENTRY_MAX - p1EntryNum;
    if(entryIdx >= p0EntryNum)
    {
        *patternIdx = RTL9602C_RAW_CF_PATTERN_1;
    }
    else
    {
        *patternIdx = RTL9602C_RAW_CF_PATTERN_0;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_cf_rule_set
 * Description:
 *      Set classification pattern content
 * Input:
 *      index   -index of the classification (0 - 511)
 *      pattern  - the pattern of the classification
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
int32 rtl9602c_raw_cf_rule_set(rtl9602c_raw_cf_RuleEntry_t *pattern)
{

    int32 ret;
    uint32 entryIdx;
    uint32 patternIdx;
    uint32 fieldData[RTL9602C_RAW_CF_ENTRYLEN];
    uint32 fieldMask[RTL9602C_RAW_CF_ENTRYLEN];
    uint32 tmpData;

    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(NULL == pattern, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pattern->idx >= RTL9602C_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->valid >= CF_ENTRY_VALID_END, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pattern->care_bits.etherType > 0xffff, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.ifCtag > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.ifStag > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.interPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.tagPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.tagVid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.innerVlan > 0xffff, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.direction >= CF_DIRECTION_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.uni > 0x7, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.aclHit > 0x7f, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.cfAclHit > 0x7f, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.wanIf >= RTL9602C_L34_NETIF_TABLE_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.igmp_mld > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.ipmc > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.dei > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.stpidSel > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.pppoe > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.ipv6 > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.ipv4 > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.intVidTagIf > 1, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pattern->data_bits.etherType > 0xffff, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.ifCtag > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.ifStag > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.interPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.tagPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.tagVid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.innerVlan > 0xffff, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.direction >= CF_DIRECTION_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.uni > 0x7, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.aclHit > 0x7f, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.cfAclHit > 0x7f, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.wanIf >= RTL9602C_L34_NETIF_TABLE_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.igmp_mld > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.ipmc > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.dei > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.stpidSel > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.pppoe > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.ipv6 > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.ipv4 > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.intVidTagIf > 1, RT_ERR_OUT_OF_RANGE);

    /*Downstream*/
    if(1 == pattern->data_bits.direction)
    {
        RT_PARAM_CHK(pattern->care_bits.tos_gemidx > 0x7f, RT_ERR_OUT_OF_RANGE);
        RT_PARAM_CHK(pattern->data_bits.tos_gemidx >= RTL9602C_CLASSIFY_SID_MAX, RT_ERR_OUT_OF_RANGE);
    }
    else
    {
        RT_PARAM_CHK(pattern->care_bits.tos_gemidx > 0xff, RT_ERR_OUT_OF_RANGE);
        RT_PARAM_CHK(pattern->data_bits.tos_gemidx > 0xff, RT_ERR_OUT_OF_RANGE);
    }

    /*Get patternIdx*/
    _dal_rtl9602c_classsify_patternIdx_get(pattern->idx, &patternIdx);

    osal_memset(fieldData, 0, sizeof(fieldData));
    osal_memset(fieldMask, 0, sizeof(fieldData));

    entryIdx = pattern->idx;

    switch (patternIdx)
    {
    case RTL9602C_RAW_CF_PATTERN_0:
        if ((ret = table_read(RTL9602C_CF_RULE_48_P0t, entryIdx, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
            return ret;
        }
        if ((ret = table_read(RTL9602C_CF_MASK_48_P0t, entryIdx, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
            return ret;
        }

        /*mask*/
        tmpData = pattern->care_bits.wanIf&(~pattern->data_bits.wanIf);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_EGR_WAN_IFtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.pppoe&(~pattern->data_bits.pppoe);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_PPPOEtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.ipv4&(~pattern->data_bits.ipv4);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IPV4tf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.ipv6&(~pattern->data_bits.ipv6);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IPV6tf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.ipmc&(~pattern->data_bits.ipmc);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IPMCtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.igmp_mld&(~pattern->data_bits.igmp_mld);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IGMP_MLDtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.aclHit&(~pattern->data_bits.aclHit);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_ACL_HIT_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }

        tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_U_Dtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.intVidTagIf&(~pattern->data_bits.intVidTagIf);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_INT_VID_TAG_IFtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.cfAclHit&(~pattern->data_bits.cfAclHit);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_CF_ACL_HITtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.tagVid&(~pattern->data_bits.tagVid);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_VID_CFGtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.tagPri&(~pattern->data_bits.tagPri);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_PRItf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.interPri&(~pattern->data_bits.interPri);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_PRI_CFGtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IF_STAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IF_CTAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
        if ((ret = table_field_set(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_UNItf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }

        /*data*/
        tmpData = pattern->data_bits.wanIf&pattern->care_bits.wanIf;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_EGR_WAN_IFtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.pppoe&pattern->care_bits.pppoe;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_PPPOEtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.ipv4&pattern->care_bits.ipv4;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IPV4tf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.ipv6&pattern->care_bits.ipv6;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IPV6tf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.ipmc&pattern->care_bits.ipmc;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IPMCtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.igmp_mld&pattern->care_bits.igmp_mld;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IGMP_MLDtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.aclHit&pattern->care_bits.aclHit;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_ACL_HIT_0tf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }

        tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_U_Dtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.intVidTagIf&pattern->care_bits.intVidTagIf;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_INT_VID_TAG_IFtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.cfAclHit&pattern->care_bits.cfAclHit;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_CF_ACL_HITtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.tagVid&pattern->care_bits.tagVid;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_VID_CFGtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.tagPri&pattern->care_bits.tagPri;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_PRItf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.interPri & pattern->care_bits.interPri;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_PRI_CFGtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IF_STAGtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.ifCtag & pattern->care_bits.ifCtag;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IF_CTAGtf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        tmpData = pattern->data_bits.uni & pattern->care_bits.uni;
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_UNItf, &tmpData, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }

        /* vailid bit */
        if ((ret = table_field_set(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_VALIDtf, &pattern->valid, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }

        if ((ret = table_write(RTL9602C_CF_RULE_48_P0t, entryIdx, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_write(RTL9602C_CF_MASK_48_P0t, entryIdx, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        break;

    case RTL9602C_RAW_CF_PATTERN_1:
    {
        uint32 cfTemplate;

        if((ret = reg_array_field_read(RTL9602C_CF_TEMPLATE_SELr, REG_ARRAY_INDEX_NONE, entryIdx, RTL9602C_SELf, &cfTemplate)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if(cfTemplate == 0)
        {
            if ((ret = table_read(RTL9602C_CF_RULE_48_P1_T0t, entryIdx, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
                return ret;
            }
            if ((ret = table_read(RTL9602C_CF_MASK_48_P1_T0t, entryIdx, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
                return ret;
            }

            /* mask */
            tmpData = pattern->care_bits.etherType&(~pattern->data_bits.etherType);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_ETH_TYPE_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_U_Dtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tos_gemidx&(~pattern->data_bits.tos_gemidx);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_TOS_GEMIDXtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagVid&(~pattern->data_bits.tagVid);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_VIDtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagPri&(~pattern->data_bits.tagPri);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_PRItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.interPri&(~pattern->data_bits.interPri);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_INTER_PRItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_IF_STAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_IF_CTAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_UNItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /*data*/
            tmpData = pattern->data_bits.etherType&pattern->care_bits.etherType;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_ETH_TYPE_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_U_Dtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tos_gemidx&pattern->care_bits.tos_gemidx;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_TOS_GEMIDXtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagVid&pattern->care_bits.tagVid;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_VIDtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagPri&pattern->care_bits.tagPri;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_PRItf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.interPri&pattern->care_bits.interPri;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_INTER_PRItf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_IF_STAGtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifCtag&pattern->care_bits.ifCtag;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_IF_CTAGtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.uni&pattern->care_bits.uni;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_UNItf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /* vailid bit */
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_VALIDtf, &pattern->valid, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            if ((ret = table_write(RTL9602C_CF_RULE_48_P1_T0t, entryIdx, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_write(RTL9602C_CF_MASK_48_P1_T0t, entryIdx, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
        }
        else
        {
            if ((ret = table_read(RTL9602C_CF_RULE_48_P1_T1t, entryIdx, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
                return ret;
            }
            if ((ret = table_read(RTL9602C_CF_MASK_48_P1_T1t, entryIdx, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
                return ret;
            }

            /* mask */
            tmpData = pattern->care_bits.innerVlan&(~pattern->data_bits.innerVlan);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_CTAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_U_Dtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tos_gemidx&(~pattern->data_bits.tos_gemidx);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_TOS_GEMIDXtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagVid&(~pattern->data_bits.tagVid);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_VIDtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagPri&(~pattern->data_bits.tagPri);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_PRItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.dei&(~pattern->data_bits.dei);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_DEItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.stpidSel&(~pattern->data_bits.stpidSel);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_STPIDtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_IF_STAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_IF_CTAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
            if ((ret = table_field_set(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_UNItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /*data*/
            tmpData = pattern->data_bits.innerVlan&pattern->care_bits.innerVlan;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_CTAGtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_U_Dtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tos_gemidx&pattern->care_bits.tos_gemidx;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_TOS_GEMIDXtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagVid&pattern->care_bits.tagVid;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_VIDtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagPri&pattern->care_bits.tagPri;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_PRItf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.dei&pattern->care_bits.dei;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_DEItf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.stpidSel&pattern->care_bits.stpidSel;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_STPIDtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_IF_STAGtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifCtag&pattern->care_bits.ifCtag;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_IF_CTAGtf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.uni&pattern->care_bits.uni;
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_UNItf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /* vailid bit */
            if ((ret = table_field_set(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_VALIDtf, &pattern->valid, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            if ((ret = table_write(RTL9602C_CF_RULE_48_P1_T1t, entryIdx, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_write(RTL9602C_CF_MASK_48_P1_T1t, entryIdx, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
        }
        break;
    }
    default:
        return RT_ERR_OUT_OF_RANGE;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9602c_raw_cf_rule_get
 * Description:
  *      Set classification pattern content
 * Input:
 *      index   -index of the classification
 * Output:
 *      pattern  - the pattern of the classification
 * Return:
 *      RT_ERR_OK             - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
int32 rtl9602c_raw_cf_rule_get(rtl9602c_raw_cf_RuleEntry_t *pattern)

{
    int32 ret;
    uint32 entryIdx;
    uint32 patternIdx;
    uint32 fieldData[RTL9602C_RAW_CF_ENTRYLEN];
    uint32 fieldMask[RTL9602C_RAW_CF_ENTRYLEN];
    uint32 tmpData;


    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(NULL == pattern, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pattern->idx >= RTL9602C_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);

    /*Get pattern and entry num*/
    _dal_rtl9602c_classsify_patternIdx_get(pattern->idx, &patternIdx);

    osal_memset(fieldData, 0, sizeof(fieldData));
    osal_memset(fieldMask, 0, sizeof(fieldMask));

    entryIdx = pattern->idx;

    switch (patternIdx)
    {

    case RTL9602C_RAW_CF_PATTERN_0:
        if ((ret = table_read(RTL9602C_CF_RULE_48_P0t, entryIdx, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_read(RTL9602C_CF_MASK_48_P0t, entryIdx, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        /*data*/
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_EGR_WAN_IFtf, &(pattern->data_bits.wanIf), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_PPPOEtf, &(pattern->data_bits.pppoe), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IPV4tf, &(pattern->data_bits.ipv4), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IPV6tf, &(pattern->data_bits.ipv6), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IPMCtf, &(pattern->data_bits.ipmc), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IGMP_MLDtf, &(pattern->data_bits.igmp_mld), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_ACL_HIT_0tf, &(pattern->data_bits.aclHit), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }

        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_U_Dtf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_INT_VID_TAG_IFtf, &(pattern->data_bits.intVidTagIf), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_CF_ACL_HITtf, &(pattern->data_bits.cfAclHit), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_VID_CFGtf, &(pattern->data_bits.tagVid), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_PRItf, &(pattern->data_bits.tagPri), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_PRI_CFGtf, &(pattern->data_bits.interPri), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IF_STAGtf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_IF_CTAGtf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_UNItf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }

        /* mask */
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_EGR_WAN_IFtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.wanIf = tmpData^pattern->data_bits.wanIf;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_PPPOEtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.pppoe = tmpData^pattern->data_bits.pppoe;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IPV4tf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.ipv4 = tmpData^pattern->data_bits.ipv4;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IPV6tf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.ipv6 = tmpData^pattern->data_bits.ipv6;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IPMCtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.ipmc = tmpData^pattern->data_bits.ipmc;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IGMP_MLDtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.igmp_mld = tmpData^pattern->data_bits.igmp_mld;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_ACL_HIT_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.aclHit = tmpData^pattern->data_bits.aclHit;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_U_Dtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.direction = tmpData^pattern->data_bits.direction;

        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_INT_VID_TAG_IFtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.intVidTagIf = tmpData^pattern->data_bits.intVidTagIf;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_CF_ACL_HITtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.cfAclHit = tmpData^pattern->data_bits.cfAclHit;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_VID_CFGtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.tagVid = tmpData^pattern->data_bits.tagVid;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_PRItf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.tagPri = tmpData^pattern->data_bits.tagPri;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_PRI_CFGtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.interPri = tmpData^pattern->data_bits.interPri;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IF_STAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_IF_CTAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
        if ((ret = table_field_get(RTL9602C_CF_MASK_48_P0t, RTL9602C_CF_MASK_48_P0_UNItf, &tmpData, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        pattern->care_bits.uni = tmpData^pattern->data_bits.uni;

        /* vailid bit */
        if ((ret = table_field_get(RTL9602C_CF_RULE_48_P0t, RTL9602C_CF_RULE_48_P0_VALIDtf, &pattern->valid, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        break;

    case RTL9602C_RAW_CF_PATTERN_1:
    {
        uint32 cfTemplate;

        if((ret = reg_array_field_read(RTL9602C_CF_TEMPLATE_SELr, REG_ARRAY_INDEX_NONE, entryIdx, RTL9602C_SELf, &cfTemplate)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if(cfTemplate == 0)
        {
            if ((ret = table_read(RTL9602C_CF_RULE_48_P1_T0t, entryIdx, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_read(RTL9602C_CF_MASK_48_P1_T0t, entryIdx, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            /*data*/
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_ETH_TYPE_0tf, &(pattern->data_bits.etherType), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_U_Dtf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_TOS_GEMIDXtf, &(pattern->data_bits.tos_gemidx), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_VIDtf, &(pattern->data_bits.tagVid), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_PRItf, &(pattern->data_bits.tagPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_INTER_PRItf, &(pattern->data_bits.interPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_IF_STAGtf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_IF_CTAGtf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_UNItf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /* mask */
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_ETH_TYPE_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.etherType = tmpData^pattern->data_bits.etherType;
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_U_Dtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.direction = tmpData^pattern->data_bits.direction;
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_TOS_GEMIDXtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tos_gemidx = tmpData^pattern->data_bits.tos_gemidx;
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_VIDtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagVid = tmpData^pattern->data_bits.tagVid;
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_PRItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagPri = tmpData^pattern->data_bits.tagPri;
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_INTER_PRItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.interPri = tmpData^pattern->data_bits.interPri;
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_IF_STAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_IF_CTAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_MASK_48_P1_T0_UNItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.uni = tmpData^pattern->data_bits.uni;

            /* vailid bit */
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T0t, RTL9602C_CF_RULE_48_P1_T0_VALIDtf, &pattern->valid, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
        }
        else
        {
            if ((ret = table_read(RTL9602C_CF_RULE_48_P1_T1t, entryIdx, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_read(RTL9602C_CF_MASK_48_P1_T1t, entryIdx, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            /*data*/
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_CTAGtf, &(pattern->data_bits.innerVlan), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_U_Dtf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_TOS_GEMIDXtf, &(pattern->data_bits.tos_gemidx), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_VIDtf, &(pattern->data_bits.tagVid), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_PRItf, &(pattern->data_bits.tagPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_DEItf, &(pattern->data_bits.dei), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_STPIDtf, &(pattern->data_bits.stpidSel), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_IF_STAGtf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_IF_CTAGtf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_UNItf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /* mask */
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_CTAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.innerVlan = tmpData^pattern->data_bits.innerVlan;
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_U_Dtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.direction = tmpData^pattern->data_bits.direction;
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_TOS_GEMIDXtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tos_gemidx = tmpData^pattern->data_bits.tos_gemidx;
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_VIDtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagVid = tmpData^pattern->data_bits.tagVid;
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_PRItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagPri = tmpData^pattern->data_bits.tagPri;
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_DEItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.dei = tmpData^pattern->data_bits.dei;
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_STPIDtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.stpidSel = tmpData^pattern->data_bits.stpidSel;
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_IF_STAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_IF_CTAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
            if ((ret = table_field_get(RTL9602C_CF_RULE_48_P1_T1t, RTL9602C_CF_MASK_48_P1_T1_UNItf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.uni = tmpData^pattern->data_bits.uni;

            /* vailid bit */
            if ((ret = table_field_get(RTL9602C_CF_MASK_48_P1_T1t, RTL9602C_CF_RULE_48_P1_T1_VALIDtf, &pattern->valid, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
        }
        break;
    }

    default:
        return RT_ERR_OUT_OF_RANGE;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      rtl9602c_raw_cf_dsAct_set
 * Description:
 *      Set classification downstrean action control
 * Input:
 *      index   - index of the classification (0 - 511)
 *      pCfDsAct    - CF down stream action stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */

int32 rtl9602c_raw_cf_dsAct_set(rtl9602c_raw_cf_dsAct_t *pCfDsAct)
{
    int32 ret;
    uint32 fieldData[RTL9602C_RAW_CF_DSACTTBLEN];

    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(NULL == pCfDsAct, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pCfDsAct->idx >= RTL9602C_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfDsAct->csact >= CF_DS_CSACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->svid_act > CF_DS_VID_ACT_TRANSLATION_SP2C, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->spri_act >= CF_DS_PRI_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->s_vid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->s_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfDsAct->cact >= CF_DS_CACT_END , RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->cvid_act >= CF_DS_VID_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->cpri_act >= CF_DS_PRI_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->c_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->c_vid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfDsAct->cfpri_act >= CF_CF_PRI_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->cfpri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfDsAct->uni_act >= CF_DS_UNI_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->uni_mask > 0x7f, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfDsAct->dscp_remark >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_OUT_OF_RANGE);

    osal_memset(fieldData, 0, sizeof(fieldData));

    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CSACTtf, &pCfDsAct->csact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CSPRI_ACTtf, &pCfDsAct->spri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CSVID_ACTtf, &pCfDsAct->svid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CS_PRItf, &pCfDsAct->s_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CS_VIDtf, &pCfDsAct->s_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CACTtf, &pCfDsAct->cact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CVID_ACTtf, &pCfDsAct->cvid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CPRI_ACTtf, &pCfDsAct->cpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_C_VIDtf, &pCfDsAct->c_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_C_PRItf, &pCfDsAct->c_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CFPRI_ACTtf, &pCfDsAct->cfpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CF_PRItf, &pCfDsAct->cfpri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_DSCP_REMARKtf, &pCfDsAct->dscp_remark, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    /*Modify later*/
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_DSCPtf, &pCfDsAct->dscp, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_UNI_ACTtf, &pCfDsAct->uni_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_UNI_PMSKtf, &pCfDsAct->uni_mask, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }


    if ((ret = table_write(RTL9602C_CF_ACTION_DSt, pCfDsAct->idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}
/* Function Name:
 *      rtl9602c_raw_cf_dsAct_get
 * Description:
 *      Get classification downstrean action control
 * Input:
 *      index   - index of the classification (0 - 511)
 * Output:
 *      pCfDsAct    - CF down stream action stucture for setting
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
int32 rtl9602c_raw_cf_dsAct_get(rtl9602c_raw_cf_dsAct_t *pCfDsAct)
{
    int32 ret;
    uint32 fieldData[RTL9602C_RAW_CF_DSACTTBLEN];

    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(NULL == pCfDsAct, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pCfDsAct->idx >= RTL9602C_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);

    osal_memset(fieldData, 0, sizeof(fieldData));
    if ((ret = table_read(RTL9602C_CF_ACTION_DSt, pCfDsAct->idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CSACTtf, &pCfDsAct->csact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CSVID_ACTtf, &pCfDsAct->svid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CSPRI_ACTtf, &pCfDsAct->spri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CS_VIDtf, &pCfDsAct->s_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CS_PRItf, &pCfDsAct->s_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CACTtf, &pCfDsAct->cact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CVID_ACTtf, &pCfDsAct->cvid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CPRI_ACTtf, &pCfDsAct->cpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_C_VIDtf, &pCfDsAct->c_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_C_PRItf, &pCfDsAct->c_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CFPRI_ACTtf, &pCfDsAct->cfpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_CF_PRItf, &pCfDsAct->cfpri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_DSCP_REMARKtf, &pCfDsAct->dscp_remark, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_DSCPtf, &pCfDsAct->dscp, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_UNI_ACTtf, &pCfDsAct->uni_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_DSt, RTL9602C_CF_ACTION_DS_UNI_PMSKtf, &pCfDsAct->uni_mask, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      rtl9602c_raw_cf_usAct_set
 * Description:
 *      Set classification downstrean action control
 * Input:
 *      index   - index of the classification (0 - 511)
 *      pCfUsAct    - CF up stream action stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */

int32 rtl9602c_raw_cf_usAct_set(rtl9602c_raw_cf_usAct_t *pCfUsAct)
{
    int32 ret;
    uint32 fieldData[RTL9602C_RAW_CF_UPACTTBLEN];
    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(pCfUsAct == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pCfUsAct->idx >= RTL9602C_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->csact >= CF_US_CSACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->svid_act > CF_US_VID_ACT_FROM_2ND_TAG, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->spri_act >= CF_US_PRI_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->s_vid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->s_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfUsAct->cact >= CF_US_CACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->cvid_act >= CF_US_VID_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->cpri_act >= CF_US_PRI_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->c_vid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->c_pri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfUsAct->sid_act >= CF_US_SQID_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->assign_idx >= RTL9602C_CLASSIFY_SID_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfUsAct->cfpri_act >= CF_CF_PRI_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->cfpri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfUsAct->dscp_remark >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->dscp > RTK_VALUE_OF_DSCP_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfUsAct->forward_act >= CF_US_FWD_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->sid_act >= CF_US_SQID_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->assign_idx >= RTL9602C_CLASSIFY_SID_MAX, RT_ERR_OUT_OF_RANGE);


    osal_memset(fieldData, 0, sizeof(fieldData));

    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CSACTtf, &pCfUsAct->csact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CSVID_ACTtf, &pCfUsAct->svid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CSPRI_ACTtf, &pCfUsAct->spri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CS_VIDtf, &pCfUsAct->s_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CS_PRItf, &pCfUsAct->s_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CACTtf, &pCfUsAct->cact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CVID_ACTtf, &pCfUsAct->cvid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CPRI_ACTtf, &pCfUsAct->cpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_C_VIDtf, &pCfUsAct->c_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_C_PRItf, &pCfUsAct->c_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CFPRI_ACTtf, &pCfUsAct->cfpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CF_PRItf, &pCfUsAct->cfpri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_DSCP_REMARKtf, &pCfUsAct->dscp_remark, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_DSCPtf, &pCfUsAct->dscp, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_DROP_TRAP_ACTtf, &pCfUsAct->forward_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_SID_ACTtf, &pCfUsAct->sid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_ASSIGN_IDXtf, &pCfUsAct->assign_idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_write(RTL9602C_CF_ACTION_USt, pCfUsAct->idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_raw_cf_usAct_get
 * Description:
 *      Get classification downstrean action control
 * Input:
 *      index   - index of the classification (0 - 511)
 * Output:
 *      pCfUsAct    - CF down stream action stucture for setting
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */

int32 rtl9602c_raw_cf_usAct_get(rtl9602c_raw_cf_usAct_t *pCfUsAct)
{
    int32 ret;
    uint32 fieldData[RTL9602C_RAW_CF_UPACTTBLEN];

    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(pCfUsAct->idx >= RTL9602C_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct == NULL, RT_ERR_NULL_POINTER);

    osal_memset(fieldData, 0, sizeof(fieldData));
    if ((ret = table_read(RTL9602C_CF_ACTION_USt, pCfUsAct->idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CSACTtf, &pCfUsAct->csact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CSVID_ACTtf, &pCfUsAct->svid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CSPRI_ACTtf, &pCfUsAct->spri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CS_VIDtf, &pCfUsAct->s_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CS_PRItf, &pCfUsAct->s_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CACTtf, &pCfUsAct->cact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CVID_ACTtf, &pCfUsAct->cvid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CPRI_ACTtf, &pCfUsAct->cpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_C_VIDtf, &pCfUsAct->c_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_C_PRItf, &pCfUsAct->c_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CFPRI_ACTtf, &pCfUsAct->cfpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_CF_PRItf, &pCfUsAct->cfpri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_DSCP_REMARKtf, &pCfUsAct->dscp_remark, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_DSCPtf, &pCfUsAct->dscp, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_DROP_TRAP_ACTtf, &pCfUsAct->forward_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_SID_ACTtf, &pCfUsAct->sid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(RTL9602C_CF_ACTION_USt, RTL9602C_CF_ACTION_US_ASSIGN_IDXtf, &pCfUsAct->assign_idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      dal_rtl9602c_classify_init
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
dal_rtl9602c_classify_init(void)
{
    rtl9602c_raw_cf_RuleEntry_t entry;
    rtl9602c_raw_cf_dsAct_t ds_act;
    rtl9602c_raw_cf_usAct_t us_act;
    rtk_classify_template_cfg_pattern0_t p0TemplateCfg;
    rtk_classify_default_wanIf_t wanIf;
    uint32  idx;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    classify_init = INIT_COMPLETED;

    /* clear table */
    osal_memset(&entry, 0x0, sizeof(rtl9602c_raw_cf_RuleEntry_t));
    osal_memset(&ds_act, 0x0, sizeof(rtl9602c_raw_cf_dsAct_t));
    osal_memset(&us_act, 0x0, sizeof(rtl9602c_raw_cf_usAct_t));

    /*clear all Pattern 0 entry*/
    if ((ret = dal_rtl9602c_classify_entryNumPattern1_set(RTL9602C_CF_PATTERN1_ENTRY_MIN)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CLASSIFY), "");
        classify_init = INIT_NOT_COMPLETED;
        return ret;
    }
    for(idx = 0; idx < RTL9602C_CF_PATTERN0_ENTRY_MAX; idx++)
    {
        entry.idx  = idx;
        entry.valid = CF_ENTRY_INVALID;
        if ((ret = rtl9602c_raw_cf_rule_set(&entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_CLASSIFY), "");
            classify_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    /*clear all Pattern 1 entry, and all DS/US action*/
    if ((ret = dal_rtl9602c_classify_entryNumPattern1_set(RTL9602C_CF_PATTERN1_ENTRY_MAX)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CLASSIFY), "");
        classify_init = INIT_NOT_COMPLETED;
        return ret;
    }
    for(idx = 0; idx < RTL9602C_CLASSIFY_ENTRY_MAX; idx++)
    {
        entry.idx  = idx;
        entry.valid = CF_ENTRY_INVALID;
        ds_act.idx = idx;
        us_act.idx = idx;

        RT_ERR_CHK(rtl9602c_raw_cf_rule_set(&entry), ret);
        RT_ERR_CHK(rtl9602c_raw_cf_dsAct_set(&ds_act), ret);
        RT_ERR_CHK(rtl9602c_raw_cf_usAct_set(&us_act), ret);
    }

    /*Clear all setting*/
    if ((ret = dal_rtl9602c_classify_entryNumPattern1_set(0x80)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CLASSIFY), "");
        classify_init = INIT_NOT_COMPLETED;
        return ret;
    }
    if ((ret = dal_rtl9602c_classify_cf_sel_set(2, CLASSIFY_CF_SEL_DISABLE)) != RT_ERR_OK) /*PON*/
    {
        RT_ERR(ret, (MOD_DAL|MOD_CLASSIFY), "");
        classify_init = INIT_NOT_COMPLETED;
        return ret;
    }
    if ((ret = dal_rtl9602c_classify_unmatchAction_set(CLASSIFY_UNMATCH_PERMIT)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CLASSIFY), "");
        classify_init = INIT_NOT_COMPLETED;
        return ret;
    }

    wanIf.l2WanIf = 0;
    wanIf.mcastWanIf = 0;
    if ((ret = dal_rtl9602c_classify_defaultWanIf_set(&wanIf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_CLASSIFY), "");
        classify_init = INIT_NOT_COMPLETED;
        return ret;
    }

    osal_memset(&p0TemplateCfg, 0x0, sizeof(rtk_classify_template_cfg_pattern0_t));
    for(idx = 0; idx < 2; idx++)
    {
        p0TemplateCfg.index = idx;
        if ((ret = dal_rtl9602c_classify_templateCfgPattern0_set(&p0TemplateCfg)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_CLASSIFY), "");
            classify_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    classify_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_rtl9603_classify_init */


static int
_dal_rtl9602c_classify_cfg2raw_us_csAct(rtk_classify_us_csact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_US_CSACT_ACTCTRL_DISABLE: /*For action control*/
    case CLASSIFY_US_CSACT_NOP:
        *pRawAct = CF_US_CSACT_NOP;
        break;
    case CLASSIFY_US_CSACT_ADD_TAG_VS_TPID:
        *pRawAct = CF_US_CSACT_ADD_TAG_VS_TPID;
        break;
    case CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2:
        *pRawAct = CF_US_CSACT_ADD_TAG_VS_TPID2;
        break;
    case CLASSIFY_US_CSACT_DEL_STAG:
        *pRawAct = CF_US_CSACT_DEL_STAG;
        break;
    case CLASSIFY_US_CSACT_TRANSPARENT:
        *pRawAct = CF_US_CSACT_TRANSPARENT;
        break;
    case CLASSIFY_US_CSACT_ADD_TAG_STAG_TPID:
        *pRawAct = CF_US_CSACT_ADD_TAG_STAG_TPID;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_cfg2raw_us_cAct(rtk_classify_us_cact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_US_CACT_ACTCTRL_DISABLE: /*For action control*/
    case CLASSIFY_US_CACT_NOP:
        *pRawAct = CF_US_CACT_NOP;
        break;
    case CLASSIFY_US_CACT_ADD_CTAG_8100:
        *pRawAct = CF_US_CACT_ADD_CTAG_8100;
        break;
    case CLASSIFY_US_CACT_DEL_CTAG:
        *pRawAct = CF_US_CACT_DEL_CTAG;
        break;
    case CLASSIFY_US_CACT_TRANSPARENT:
        *pRawAct = CF_US_CACT_TRANSPARENT;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_cfg2raw_us_vidAct(rtk_classify_us_vid_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_US_VID_ACT_NOP:
        *pRawAct = CF_US_VID_ACT_NOP;
        break;
    case CLASSIFY_US_VID_ACT_ASSIGN:
        *pRawAct = CF_US_VID_ACT_ASSIGN;
        break;
    case CLASSIFY_US_VID_ACT_FROM_1ST_TAG:
        *pRawAct = CF_US_VID_ACT_FROM_1ST_TAG;
        break;
    case CLASSIFY_US_VID_ACT_FROM_2ND_TAG:
        *pRawAct = CF_US_VID_ACT_FROM_2ND_TAG;
        break;
    case CLASSIFY_US_VID_ACT_FROM_INTERNAL:
        *pRawAct = CF_US_VID_ACT_FROM_INTERNAL;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_cfg2raw_us_priAct(rtk_classify_us_pri_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_US_PRI_ACT_NOP:
        *pRawAct = CF_US_PRI_ACT_NOP;
        break;
    case CLASSIFY_US_PRI_ACT_ASSIGN:
        *pRawAct = CF_US_PRI_ACT_ASSIGN;
        break;
    case CLASSIFY_US_PRI_ACT_FROM_1ST_TAG:
        *pRawAct = CF_US_PRI_ACT_FROM_1ST_TAG;
        break;
    case CLASSIFY_US_PRI_ACT_FROM_2ND_TAG:
        *pRawAct = CF_US_PRI_ACT_FROM_2ND_TAG;
        break;
    case CLASSIFY_US_PRI_ACT_FROM_INTERNAL:
        *pRawAct = CF_US_PRI_ACT_FROM_INTERNAL;
        break;
    case CLASSIFY_US_PRI_ACT_FROM_DSCP:
        *pRawAct = CF_US_PRI_ACT_FROM_DSCP;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_cfg2raw_ds_csAct(rtk_classify_ds_csact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_DS_CSACT_ACTCTRL_DISABLE: /*For action control*/
    case CLASSIFY_DS_CSACT_NOP:
        *pRawAct = CF_DS_CSACT_NOP;
        break;
    case CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID:
        *pRawAct = CF_DS_CSACT_ADD_TAG_VS_TPID;
        break;
    case CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2:
        *pRawAct = CF_DS_CSACT_ADD_TAG_VS_TPID2;
        break;
    case CLASSIFY_DS_CSACT_DEL_STAG:
        *pRawAct = CF_DS_CSACT_DEL_STAG;
        break;
    case CLASSIFY_DS_CSACT_TRANSPARENT:
        *pRawAct = CF_DS_CSACT_TRANSPARENT;
        break;
    case CLASSIFY_DS_CSACT_ADD_TAG_STAG_TPID:
        *pRawAct = CF_DS_CSACT_ADD_TAG_STAG_TPID;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_cfg2raw_ds_cAct(rtk_classify_ds_cact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_DS_CACT_ACTCTRL_DISABLE: /*For action control*/
    case CLASSIFY_DS_CACT_NOP:
        *pRawAct = CF_DS_CACT_NOP;
        break;
    case CLASSIFY_DS_CACT_ADD_CTAG_8100:
        *pRawAct = CF_DS_CACT_ADD_CTAG_8100;
        break;
    case CLASSIFY_DS_CACT_DEL_CTAG:
        *pRawAct = CF_DS_CACT_DEL_CTAG;
        break;
    case CLASSIFY_DS_CACT_TRANSPARENT:
        *pRawAct = CF_DS_CACT_TRANSPARENT;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_cfg2raw_ds_vidAct(rtk_classify_ds_vid_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_DS_VID_ACT_NOP:
        *pRawAct = CF_DS_VID_ACT_NOP;
        break;
    case CLASSIFY_DS_VID_ACT_ASSIGN:
        *pRawAct = CF_DS_VID_ACT_ASSIGN;
        break;
    case CLASSIFY_DS_VID_ACT_FROM_1ST_TAG:
        *pRawAct = CF_DS_VID_ACT_FROM_1ST_TAG;
        break;
    case CLASSIFY_DS_VID_ACT_FROM_2ND_TAG:
        *pRawAct = CF_DS_VID_ACT_FROM_2ND_TAG;
        break;
    case CLASSIFY_DS_VID_ACT_TRANSLATION_SP2C:
        *pRawAct = CF_DS_VID_ACT_TRANSLATION_SP2C;
        break;
    case CLASSIFY_DS_VID_ACT_FROM_LUT:
        *pRawAct = CF_DS_VID_ACT_FROM_LUT;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_cfg2raw_ds_priAct(rtk_classify_ds_pri_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_DS_PRI_ACT_NOP:
        *pRawAct = CF_DS_PRI_ACT_NOP;
        break;
    case CLASSIFY_DS_PRI_ACT_ASSIGN:
        *pRawAct = CF_DS_PRI_ACT_ASSIGN;
        break;
    case CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG:
        *pRawAct = CF_DS_PRI_ACT_FROM_1ST_TAG;
        break;
    case CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG:
        *pRawAct = CF_DS_PRI_ACT_FROM_2ND_TAG;
        break;
    case CLASSIFY_DS_PRI_ACT_TRANSLATION_SP2C:
        *pRawAct = CF_DS_PRI_ACT_TRANSLATION_SP2C;
        break;
    case CLASSIFY_DS_PRI_ACT_FROM_INTERNAL:
        *pRawAct = CF_DS_PRI_ACT_FROM_INTERNAL;
        break;
    case CLASSIFY_DS_PRI_ACT_FROM_DSCP:
        *pRawAct = CF_DS_PRI_ACT_FROM_DSCP;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_cfg2raw_cfPriAct(rtk_classify_cf_pri_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_CF_PRI_ACT_ACTCTRL_DISABLE: /*For action control*/
    case CLASSIFY_CF_PRI_ACT_NOP:
        *pRawAct = CF_CF_PRI_ACT_NOP;
        break;
    case CLASSIFY_CF_PRI_ACT_ASSIGN:
        *pRawAct = CF_CF_PRI_ACT_ASSIGN;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_cfg2raw_dscpAct(rtk_classify_dscp_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_DSCP_ACT_ACTCTRL_DISABLE: /*For action control*/
    case CLASSIFY_DSCP_ACT_DISABLE:
        *pRawAct = CF_DISABLE;
        break;
    case CLASSIFY_DSCP_ACT_ENABLE:
        *pRawAct = CF_ENABLE;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}


static int
_dal_rtl9602c_classify_cfg2raw_uniAct(rtk_classify_ds_uni_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct) {
    case CLASSIFY_DS_UNI_ACT_ACTCTRL_DISABLE: /*For action control*/
    case CLASSIFY_DS_UNI_ACT_NOP:
        *pRawAct  = CF_DS_UNI_ACT_NOP;
        break;
    case CLASSIFY_DS_UNI_ACT_FORCE_FORWARD:
        *pRawAct   = CF_DS_UNI_ACT_FORCE_FORWARD ;
        break;
    case CLASSIFY_DS_UNI_ACT_TRAP:
        *pRawAct   = CF_DS_UNI_ACT_TRAP;
        break;
    case CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK:
        *pRawAct   = CF_DS_UNI_ACT_MASK_BY_UNIMASK;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }
    return ret;
}


static int
_dal_rtl9602c_classify_cfg2raw_us_fwdAct(rtk_classify_drop_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
    case CLASSIFY_DROP_ACT_ACTCTRL_DISABLE: /*For action control*/
    case CLASSIFY_DROP_ACT_NONE:
        *pRawAct = CF_US_FWD_ACT_NOP;
        break;
    case CLASSIFY_DROP_ACT_ENABLE:
        *pRawAct = CF_US_FWD_ACT_DROP;
        break;
    case CLASSIFY_DROP_ACT_TRAP:
        *pRawAct = CF_US_FWD_ACT_TRAP;
        break;
    case CLASSIFY_DROP_ACT_DROP_PON:
        *pRawAct = CF_US_FWD_ACT_DROP_PON;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}


static int
_dal_rtl9602c_classify_cfg2raw_sidAct(rtk_classify_us_sqid_act_t sqidAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(sqidAct)
    {
    case CLASSIFY_US_SQID_ACT_ACTCTRL_DISABLE:      /*For action control*/
    case CLASSIFY_US_SQID_ACT_ASSIGN_NOP:
        *pRawAct   =CF_US_SQID_ACT_ASSIGN_NOP ;
        break;
    case CLASSIFY_US_SQID_ACT_ASSIGN_SID:
        *pRawAct  = CF_US_SQID_ACT_ASSIGN_SID;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }

    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_us_csAct(uint32 rawAct, rtk_classify_us_csact_t *pCcfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_US_CSACT_NOP:
        *pCcfgAct = CLASSIFY_US_CSACT_NOP;
        break;
    case CF_US_CSACT_ADD_TAG_VS_TPID:
        *pCcfgAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID;
        break;
    case CF_US_CSACT_ADD_TAG_VS_TPID2:
        *pCcfgAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2;
        break;
    case CF_US_CSACT_DEL_STAG:
        *pCcfgAct = CLASSIFY_US_CSACT_DEL_STAG;
        break;
    case CF_US_CSACT_TRANSPARENT:
        *pCcfgAct = CLASSIFY_US_CSACT_TRANSPARENT;
        break;
    case CF_US_CSACT_ADD_TAG_STAG_TPID:
        *pCcfgAct = CLASSIFY_US_CSACT_ADD_TAG_STAG_TPID;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_us_cAct(uint32 rawAct, rtk_classify_us_cact_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_US_CACT_NOP:
        *pCfgAct = CLASSIFY_US_CACT_NOP;
        break;
    case CF_US_CACT_ADD_CTAG_8100:
        *pCfgAct = CLASSIFY_US_CACT_ADD_CTAG_8100;
        break;
    case CF_US_CACT_DEL_CTAG:
        *pCfgAct = CLASSIFY_US_CACT_DEL_CTAG;
        break;
    case CF_US_CACT_TRANSPARENT:
        *pCfgAct = CLASSIFY_US_CACT_TRANSPARENT;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_us_vidAct(uint32 rawAct, rtk_classify_us_vid_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_US_VID_ACT_NOP:
        *pCfgAct = CLASSIFY_US_VID_ACT_NOP;
        break;
    case CF_US_VID_ACT_ASSIGN:
        *pCfgAct = CLASSIFY_US_VID_ACT_ASSIGN;
        break;
    case CF_US_VID_ACT_FROM_1ST_TAG:
        *pCfgAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
        break;
    case CF_US_VID_ACT_FROM_2ND_TAG:
        *pCfgAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
        break;
    case CF_US_VID_ACT_FROM_INTERNAL:
        *pCfgAct = CLASSIFY_US_VID_ACT_FROM_INTERNAL;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}


static int
_dal_rtl9602c_classify_raw2cfg_us_priAct(uint32 rawAct, rtk_classify_us_pri_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_US_PRI_ACT_NOP:
        *pCfgAct = CLASSIFY_US_PRI_ACT_NOP;
        break;
    case CF_US_PRI_ACT_ASSIGN:
        *pCfgAct = CLASSIFY_US_PRI_ACT_ASSIGN;
        break;
    case CF_US_PRI_ACT_FROM_1ST_TAG:
        *pCfgAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
        break;
    case CF_US_PRI_ACT_FROM_2ND_TAG:
        *pCfgAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
        break;
    case CF_US_PRI_ACT_FROM_INTERNAL:
        *pCfgAct = CLASSIFY_US_PRI_ACT_FROM_INTERNAL;
        break;
    case CF_US_PRI_ACT_FROM_DSCP:
        *pCfgAct = CLASSIFY_US_PRI_ACT_FROM_DSCP;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_ds_csAct(uint32 rawAct, rtk_classify_ds_csact_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_DS_CSACT_NOP:
        *pCfgAct = CLASSIFY_DS_CSACT_NOP;
        break;
    case CF_DS_CSACT_ADD_TAG_VS_TPID:
        *pCfgAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID;
        break;
    case CF_DS_CSACT_ADD_TAG_VS_TPID2:
        *pCfgAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2;
        break;
    case CF_DS_CSACT_DEL_STAG:
        *pCfgAct = CLASSIFY_DS_CSACT_DEL_STAG;
        break;
    case CF_DS_CSACT_TRANSPARENT:
        *pCfgAct = CLASSIFY_DS_CSACT_TRANSPARENT;
        break;
    case CF_DS_CSACT_ADD_TAG_STAG_TPID:
        *pCfgAct = CLASSIFY_DS_CSACT_ADD_TAG_STAG_TPID;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_ds_cAct(uint32 rawAct, rtk_classify_ds_cact_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_DS_CACT_NOP:
        *pCfgAct = CLASSIFY_DS_CACT_NOP;
        break;
    case CF_DS_CACT_ADD_CTAG_8100:
        *pCfgAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
        break;
    case CF_DS_CACT_DEL_CTAG:
        *pCfgAct = CLASSIFY_DS_CACT_DEL_CTAG;
        break;
    case CF_DS_CACT_TRANSPARENT:
        *pCfgAct = CLASSIFY_DS_CACT_TRANSPARENT;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_ds_vidAct(uint32 rawAct, rtk_classify_ds_vid_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_DS_VID_ACT_NOP:
        *pCfgAct = CLASSIFY_DS_VID_ACT_NOP;
        break;
    case CF_DS_VID_ACT_ASSIGN:
        *pCfgAct = CLASSIFY_DS_VID_ACT_ASSIGN;
        break;
    case CF_DS_VID_ACT_FROM_1ST_TAG:
        *pCfgAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
        break;
    case CF_DS_VID_ACT_FROM_2ND_TAG:
        *pCfgAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
        break;
    case CF_DS_VID_ACT_TRANSLATION_SP2C:
        *pCfgAct = CLASSIFY_DS_VID_ACT_TRANSLATION_SP2C;
        break;
    case CF_DS_VID_ACT_FROM_LUT:
        *pCfgAct = CLASSIFY_DS_VID_ACT_FROM_LUT;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_ds_priAct(uint32 rawAct, rtk_classify_ds_pri_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_DS_PRI_ACT_NOP:
        *pCfgAct = CLASSIFY_DS_PRI_ACT_NOP;
        break;
    case CF_DS_PRI_ACT_ASSIGN:
        *pCfgAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
        break;
    case CF_DS_PRI_ACT_FROM_1ST_TAG:
        *pCfgAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
        break;
    case CF_DS_PRI_ACT_FROM_2ND_TAG:
        *pCfgAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
        break;
    case CF_DS_PRI_ACT_TRANSLATION_SP2C:
        *pCfgAct = CLASSIFY_DS_PRI_ACT_TRANSLATION_SP2C;
        break;
    case CF_DS_PRI_ACT_FROM_INTERNAL:
        *pCfgAct = CLASSIFY_DS_PRI_ACT_FROM_INTERNAL;
        break;
    case CF_DS_PRI_ACT_FROM_DSCP:
        *pCfgAct = CLASSIFY_DS_PRI_ACT_FROM_DSCP;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_cfPriAct(uint32 rawAct, rtk_classify_cf_pri_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_CF_PRI_ACT_NOP:
        *pCfgAct = CLASSIFY_CF_PRI_ACT_NOP;
        break;
    case CF_CF_PRI_ACT_ASSIGN:
        *pCfgAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_dscpAct(uint32 rawAct, rtk_classify_dscp_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_DISABLE:
        *pCfgAct = CLASSIFY_DSCP_ACT_DISABLE;
        break;
    case CF_ENABLE:
        *pCfgAct = CLASSIFY_DSCP_ACT_ENABLE;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_us_fwdAct(uint32 rawAct, rtk_classify_drop_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_US_FWD_ACT_NOP:
        *pCfgAct = CLASSIFY_DROP_ACT_NONE;
        break;
    case CF_US_FWD_ACT_DROP:
        *pCfgAct = CLASSIFY_DROP_ACT_ENABLE;
        break;
    case CF_US_FWD_ACT_TRAP:
        *pCfgAct = CLASSIFY_DROP_ACT_TRAP;
        break;
    case CF_US_FWD_ACT_DROP_PON:
        *pCfgAct = CLASSIFY_DROP_ACT_DROP_PON;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}


static int
_dal_rtl9602c_classify_raw2cfg_sidAct(uint32 rawAct, rtk_classify_us_sqid_act_t *psidAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
    case CF_US_SQID_ACT_ASSIGN_NOP:
        *psidAct = CLASSIFY_US_SQID_ACT_ASSIGN_NOP;
        break;
    case CF_US_SQID_ACT_ASSIGN_SID:
        *psidAct = CLASSIFY_US_SQID_ACT_ASSIGN_SID;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }
    return ret;
}

static int
_dal_rtl9602c_classify_raw2cfg_uniAct(uint32 rawAct, rtk_classify_ds_uni_act_t *puniAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct) {
    case CF_DS_UNI_ACT_NOP:
        *puniAct = CLASSIFY_DS_UNI_ACT_NOP;
        break;
    case CF_DS_UNI_ACT_FORCE_FORWARD:
        *puniAct = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD;
        break;
    case CF_DS_UNI_ACT_TRAP:
        *puniAct = CLASSIFY_DS_UNI_ACT_TRAP;
        break;
    case CF_DS_UNI_ACT_MASK_BY_UNIMASK:
        *puniAct = CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK;
        break;
    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }
    return ret;
}

/* Function Name:
 *      rtl9602c_classify_ds_actCtrl_set
 * Description:
 *      Set classification downstrean action control
 * Input:
 *      pClassifyCfg     - The classification configuration that this function will add comparison rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
static int
_dal_rtl9602c_classify_ds_actCtrl_set(rtk_classify_cfg_t *pClassifyCfg)
{
    int32   ret;
    uint32  tmp_field_val, tmp_val;

    tmp_val = 0;

    if(CLASSIFY_DS_CSACT_ACTCTRL_DISABLE == pClassifyCfg->act.dsAct.csAct)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_CSACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(CLASSIFY_DS_CACT_ACTCTRL_DISABLE == pClassifyCfg->act.dsAct.cAct)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_CACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(CLASSIFY_CF_PRI_ACT_ACTCTRL_DISABLE == pClassifyCfg->act.dsAct.interPriAct)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_CFPRI_ACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(CLASSIFY_DSCP_ACT_ACTCTRL_DISABLE == pClassifyCfg->act.dsAct.dscp)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_DSCP_REMARKf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(CLASSIFY_DS_UNI_ACT_ACTCTRL_DISABLE == pClassifyCfg->act.dsAct.uniAct)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_FWD_ACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if((ret = reg_array_write(RTL9602C_CF_ACTIONr, REG_ARRAY_INDEX_NONE, pClassifyCfg->index, &tmp_val)) !=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_classify_ds_actCtrl_get
 * Description:
 *      Set classification downstrean action control
 * Output:
 *      pClassifyCfg->index - The classification configuration index
 * Input:
 *      pClassifyCfg     - The classification configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
static int
_dal_rtl9602c_classify_ds_actCtrl_get(rtk_classify_cfg_t *pClassifyCfg)
{
    int32   ret;
    uint32  tmp_field_val, tmp_val;

    if((ret = reg_array_read(RTL9602C_CF_ACTIONr, REG_ARRAY_INDEX_NONE, pClassifyCfg->index, &tmp_val)) !=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }


    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_FWD_ACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_ACTCTRL_DISABLE;
    }

    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_DSCP_REMARKf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.dsAct.dscp = CLASSIFY_DSCP_ACT_ACTCTRL_DISABLE;
    }

    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_CFPRI_ACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.dsAct.interPriAct = CLASSIFY_CF_PRI_ACT_ACTCTRL_DISABLE;
    }

    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_CACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.dsAct.cAct = CLASSIFY_DS_CACT_ACTCTRL_DISABLE;
    }

    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_CSACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.dsAct.csAct = CLASSIFY_DS_CSACT_ACTCTRL_DISABLE;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_classify_us_actCtrl_set
 * Description:
 *      Set classification downstrean action control
 * Input:
 *      pClassifyCfg     - The classification configuration that this function will add comparison rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
static int
_dal_rtl9602c_classify_us_actCtrl_set(rtk_classify_cfg_t *pClassifyCfg)
{
    int32   ret;
    uint32  tmp_field_val, tmp_val;

    tmp_val = 0;

    if(CLASSIFY_US_CSACT_ACTCTRL_DISABLE == pClassifyCfg->act.usAct.csAct)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_CSACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(CLASSIFY_US_CACT_ACTCTRL_DISABLE == pClassifyCfg->act.usAct.cAct)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_CACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(CLASSIFY_US_SQID_ACT_ACTCTRL_DISABLE == pClassifyCfg->act.usAct.sidQidAct)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_SID_ACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
        {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(CLASSIFY_CF_PRI_ACT_ACTCTRL_DISABLE == pClassifyCfg->act.usAct.interPriAct)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_CFPRI_ACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(CLASSIFY_DSCP_ACT_ACTCTRL_DISABLE == pClassifyCfg->act.usAct.dscp)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_DSCP_REMARKf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(CLASSIFY_DROP_ACT_ACTCTRL_DISABLE== pClassifyCfg->act.usAct.drop)
        tmp_field_val = DISABLED;
    else
        tmp_field_val = ENABLED;
    if ((ret = reg_field_set(RTL9602C_CF_ACTIONr, RTL9602C_FWD_ACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if((ret = reg_array_write(RTL9602C_CF_ACTIONr, REG_ARRAY_INDEX_NONE, pClassifyCfg->index, &tmp_val)) !=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_classify_us_actCtrl_get
 * Description:
 *      Set classification downstrean action control
 * Output:
 *      pClassifyCfg->index - The classification configuration index
 * Input:
 *      pClassifyCfg     - The classification configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
static int
_dal_rtl9602c_classify_us_actCtrl_get(rtk_classify_cfg_t *pClassifyCfg)
{
    int32   ret;
    uint32  tmp_field_val, tmp_val;

    if((ret = reg_array_read(RTL9602C_CF_ACTIONr, REG_ARRAY_INDEX_NONE, pClassifyCfg->index, &tmp_val)) !=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_FWD_ACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.usAct.drop = CLASSIFY_DROP_ACT_ACTCTRL_DISABLE;
    }

    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_DSCP_REMARKf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.usAct.dscp = CLASSIFY_DSCP_ACT_ACTCTRL_DISABLE;
    }

    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_CFPRI_ACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.usAct.interPriAct = CLASSIFY_CF_PRI_ACT_ACTCTRL_DISABLE;
    }

    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_CACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.usAct.cAct = CLASSIFY_US_CACT_ACTCTRL_DISABLE;
    }

    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_CSACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.usAct.csAct = CLASSIFY_US_CSACT_ACTCTRL_DISABLE;
    }

    if ((ret = reg_field_get(RTL9602C_CF_ACTIONr, RTL9602C_SID_ACTf, &tmp_field_val, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(DISABLED == tmp_field_val)
    {
        pClassifyCfg->act.usAct.sidQidAct = CLASSIFY_US_SQID_ACT_ACTCTRL_DISABLE;
    }


    return RT_ERR_OK;
}

static int
dal_rtl9602c_classify_cfg2entry(rtk_classify_cfg_t    *pClassifyCfg,
                                rtl9602c_raw_cf_RuleEntry_t *pRule,
                                rtl9602c_raw_cf_usAct_t     *pUsAction,
                                rtl9602c_raw_cf_dsAct_t     *pDsAction)
{
    rtk_classify_field_t *tmp_field;
    int32 ret=RT_ERR_OK;

    osal_memset(pRule, 0, sizeof(rtl9602c_raw_cf_RuleEntry_t));
    osal_memset(pUsAction, 0, sizeof(rtl9602c_raw_cf_usAct_t));
    osal_memset(pDsAction, 0, sizeof(rtl9602c_raw_cf_dsAct_t));

    /* rule */
    pRule->idx   = pClassifyCfg->index;
    pRule->valid = 1;
    pRule->data_bits.direction  = pClassifyCfg->direction;
    pRule->care_bits.direction  = 0x1;
    tmp_field = pClassifyCfg->field.pFieldHead;
    while(tmp_field != NULL)
    {
        switch(tmp_field->fieldType)
        {
        case CLASSIFY_FIELD_ETHERTYPE:
            pRule->data_bits.etherType = tmp_field->classify_pattern.etherType.value;
            pRule->care_bits.etherType = tmp_field->classify_pattern.etherType.mask;
            break;

        case CLASSIFY_FIELD_TOS_DSIDX:
            pRule->data_bits.tos_gemidx = tmp_field->classify_pattern.tosDsidx.value;
            pRule->care_bits.tos_gemidx = tmp_field->classify_pattern.tosDsidx.mask;
            break;

        case CLASSIFY_FIELD_TAG_VID:
            pRule->data_bits.tagVid = tmp_field->classify_pattern.tagVid.value;
            pRule->care_bits.tagVid = tmp_field->classify_pattern.tagVid.mask;
            break;

        case CLASSIFY_FIELD_TAG_PRI:
            pRule->data_bits.tagPri = tmp_field->classify_pattern.tagPri.value;
            pRule->care_bits.tagPri = tmp_field->classify_pattern.tagPri.mask;
            break;

        case CLASSIFY_FIELD_INTER_PRI:
            pRule->data_bits.interPri = tmp_field->classify_pattern.interPri.value;
            pRule->care_bits.interPri = tmp_field->classify_pattern.interPri.mask;
            break;

        case CLASSIFY_FIELD_IS_CTAG:
            pRule->data_bits.ifCtag = tmp_field->classify_pattern.isCtag.value;
            pRule->care_bits.ifCtag = tmp_field->classify_pattern.isCtag.mask;
            break;

        case CLASSIFY_FIELD_IS_STAG:
            pRule->data_bits.ifStag = tmp_field->classify_pattern.isStag.value;
            pRule->care_bits.ifStag = tmp_field->classify_pattern.isStag.mask;
            break;

        case CLASSIFY_FIELD_UNI:
            pRule->data_bits.uni = tmp_field->classify_pattern.uni.value;
            pRule->care_bits.uni = tmp_field->classify_pattern.uni.mask;
            break;

        case CLASSIFY_FIELD_ACL_HIT:
            pRule->data_bits.aclHit = tmp_field->classify_pattern.aclHit.value;
            pRule->care_bits.aclHit = tmp_field->classify_pattern.aclHit.mask;
            break;

        case CLASSIFY_FIELD_WAN_IF:
            pRule->data_bits.wanIf = tmp_field->classify_pattern.wanIf.value;
            pRule->care_bits.wanIf = tmp_field->classify_pattern.wanIf.mask;
            break;

        case CLASSIFY_FIELD_IGMP_MLD:
            pRule->data_bits.igmp_mld = tmp_field->classify_pattern.igmp_mld.value;
            pRule->care_bits.igmp_mld = tmp_field->classify_pattern.igmp_mld.mask;
            break;

        case CLASSIFY_FIELD_INNER_VLAN:
            pRule->data_bits.innerVlan = tmp_field->classify_pattern.innerVlan.value;
            pRule->care_bits.innerVlan = tmp_field->classify_pattern.innerVlan.mask;
            break;

        case CLASSIFY_FIELD_PPPOE:
            pRule->data_bits.pppoe = tmp_field->classify_pattern.pppoe.value;
            pRule->care_bits.pppoe = tmp_field->classify_pattern.pppoe.mask;
            break;

        case CLASSIFY_FIELD_IPV4:
            pRule->data_bits.ipv4 = tmp_field->classify_pattern.ipv4.value;
            pRule->care_bits.ipv4 = tmp_field->classify_pattern.ipv4.mask;
            break;

        case CLASSIFY_FIELD_IPV6:
            pRule->data_bits.ipv6 = tmp_field->classify_pattern.ipv6.value;
            pRule->care_bits.ipv6 = tmp_field->classify_pattern.ipv6.mask;
            break;

        case CLASSIFY_FIELD_IPMC:
            pRule->data_bits.ipmc = tmp_field->classify_pattern.ipmc.value;
            pRule->care_bits.ipmc = tmp_field->classify_pattern.ipmc.mask;
            break;

        case CLASSIFY_FIELD_CF_ACL_HIT:
            pRule->data_bits.cfAclHit = tmp_field->classify_pattern.cfAclHit.value;
            pRule->care_bits.cfAclHit = tmp_field->classify_pattern.cfAclHit.mask;
            break;

        case CLASSIFY_FIELD_DEI:
            pRule->data_bits.dei = tmp_field->classify_pattern.dei.value;
            pRule->care_bits.dei = tmp_field->classify_pattern.dei.mask;
            break;

        case CLASSIFY_FIELD_INTERNAL_VID_TAG_IF:
            pRule->data_bits.intVidTagIf = tmp_field->classify_pattern.intVidTagIf.value;
            pRule->care_bits.intVidTagIf = tmp_field->classify_pattern.intVidTagIf.mask;
            break;

        case CLASSIFY_FIELD_STPID_SEL:
            pRule->data_bits.stpidSel = tmp_field->classify_pattern.stpidSel.value;
            pRule->care_bits.stpidSel = tmp_field->classify_pattern.stpidSel.mask;
            break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
        }

        tmp_field = tmp_field->next;
    }

    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {   /* us action */
        pUsAction->idx = pClassifyCfg->index;
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_us_csAct(pClassifyCfg->act.usAct.csAct, &pUsAction->csact), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_us_vidAct(pClassifyCfg->act.usAct.csVidAct, &pUsAction->svid_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_us_priAct(pClassifyCfg->act.usAct.csPriAct, &pUsAction->spri_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_us_cAct(pClassifyCfg->act.usAct.cAct, &pUsAction->cact), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_us_vidAct(pClassifyCfg->act.usAct.cVidAct, &pUsAction->cvid_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_us_priAct(pClassifyCfg->act.usAct.cPriAct, &pUsAction->cpri_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_cfPriAct(pClassifyCfg->act.usAct.interPriAct, &pUsAction->cfpri_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_dscpAct(pClassifyCfg->act.usAct.dscp, &pUsAction->dscp_remark), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_us_fwdAct(pClassifyCfg->act.usAct.drop, &pUsAction->forward_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_sidAct(pClassifyCfg->act.usAct.sidQidAct, &pUsAction->sid_act), ret);

        pUsAction->s_vid        = pClassifyCfg->act.usAct.sTagVid;
        pUsAction->s_pri        = pClassifyCfg->act.usAct.sTagPri;
        pUsAction->c_vid        = pClassifyCfg->act.usAct.cTagVid;
        pUsAction->c_pri        = pClassifyCfg->act.usAct.cTagPri;
        pUsAction->cfpri        = pClassifyCfg->act.usAct.cfPri;
        pUsAction->dscp         = pClassifyCfg->act.usAct.dscpVal;
        pUsAction->assign_idx   = pClassifyCfg->act.usAct.sidQid;

    }
    else /* ds action */
    {
        pDsAction->idx = pClassifyCfg->index;
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_ds_csAct(pClassifyCfg->act.dsAct.csAct, &pDsAction->csact), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_ds_vidAct(pClassifyCfg->act.dsAct.csVidAct, &pDsAction->svid_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_ds_priAct(pClassifyCfg->act.dsAct.csPriAct, &pDsAction->spri_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_ds_cAct(pClassifyCfg->act.dsAct.cAct, &pDsAction->cact), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_ds_vidAct(pClassifyCfg->act.dsAct.cVidAct, &pDsAction->cvid_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_ds_priAct(pClassifyCfg->act.dsAct.cPriAct, &pDsAction->cpri_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_cfPriAct(pClassifyCfg->act.dsAct.interPriAct, &pDsAction->cfpri_act), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_dscpAct(pClassifyCfg->act.dsAct.dscp, &pDsAction->dscp_remark), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_cfg2raw_uniAct(pClassifyCfg->act.dsAct.uniAct, &pDsAction->uni_act), ret);

        pDsAction->s_vid        = pClassifyCfg->act.dsAct.sTagVid;
        pDsAction->s_pri        = pClassifyCfg->act.dsAct.sTagPri;
        pDsAction->c_vid        = pClassifyCfg->act.dsAct.cTagVid;
        pDsAction->c_pri        = pClassifyCfg->act.dsAct.cTagPri;
        pDsAction->cfpri        = pClassifyCfg->act.dsAct.cfPri;
        pDsAction->dscp         = pClassifyCfg->act.dsAct.dscpVal;
        pDsAction->uni_mask     = pClassifyCfg->act.dsAct.uniMask.bits[0];

    }

    return ret;
}


static int
dal_rtl9602c_classify_entry2cfg(rtl9602c_raw_cf_usAct_t     *pUsAction,
                                rtl9602c_raw_cf_dsAct_t     *pDsAction,
                                rtk_classify_cfg_t          *pClassifyCfg)
{
    int32 ret=RT_ERR_OK;

    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {   /* us action */
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_us_csAct(pUsAction->csact, &pClassifyCfg->act.usAct.csAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_us_vidAct(pUsAction->svid_act, &pClassifyCfg->act.usAct.csVidAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_us_priAct(pUsAction->spri_act, &pClassifyCfg->act.usAct.csPriAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_us_cAct(pUsAction->cact, &pClassifyCfg->act.usAct.cAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_us_vidAct(pUsAction->cvid_act, &pClassifyCfg->act.usAct.cVidAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_us_priAct(pUsAction->cpri_act, &pClassifyCfg->act.usAct.cPriAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_cfPriAct(pUsAction->cfpri_act, &pClassifyCfg->act.usAct.interPriAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_dscpAct(pUsAction->dscp_remark, &pClassifyCfg->act.usAct.dscp), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_us_fwdAct(pUsAction->forward_act, &pClassifyCfg->act.usAct.drop), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_sidAct(pUsAction->sid_act, &pClassifyCfg->act.usAct.sidQidAct), ret);

        pClassifyCfg->act.usAct.sTagVid     = pUsAction->s_vid;
        pClassifyCfg->act.usAct.sTagPri     = pUsAction->s_pri;
        pClassifyCfg->act.usAct.cTagVid     = pUsAction->c_vid;
        pClassifyCfg->act.usAct.cTagPri     = pUsAction->c_pri;
        pClassifyCfg->act.usAct.cfPri       = pUsAction->cfpri;
        pClassifyCfg->act.usAct.dscpVal     = pUsAction->dscp;
        pClassifyCfg->act.usAct.sidQid      = pUsAction->assign_idx;

    }
    else /* ds action */
    {
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_ds_csAct(pDsAction->csact, &pClassifyCfg->act.dsAct.csAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_ds_vidAct(pDsAction->svid_act, &pClassifyCfg->act.dsAct.csVidAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_ds_priAct(pDsAction->spri_act, &pClassifyCfg->act.dsAct.csPriAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_ds_cAct(pDsAction->cact, &pClassifyCfg->act.dsAct.cAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_ds_vidAct(pDsAction->cvid_act, &pClassifyCfg->act.dsAct.cVidAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_ds_priAct(pDsAction->cpri_act, &pClassifyCfg->act.dsAct.cPriAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_cfPriAct(pDsAction->cfpri_act, &pClassifyCfg->act.dsAct.interPriAct), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_dscpAct(pDsAction->dscp_remark, &pClassifyCfg->act.dsAct.dscp), ret);
        RT_ERR_CHK(_dal_rtl9602c_classify_raw2cfg_uniAct(pDsAction->uni_act, &pClassifyCfg->act.dsAct.uniAct), ret);


        pClassifyCfg->act.dsAct.sTagVid     = pDsAction->s_vid;
        pClassifyCfg->act.dsAct.sTagPri     = pDsAction->s_pri;
        pClassifyCfg->act.dsAct.cTagVid     = pDsAction->c_vid;
        pClassifyCfg->act.dsAct.cTagPri     = pDsAction->c_pri;
        pClassifyCfg->act.dsAct.cfPri       = pDsAction->cfpri;
        pClassifyCfg->act.dsAct.dscpVal     = pDsAction->dscp;
        pClassifyCfg->act.dsAct.uniMask.bits[0] = pDsAction->uni_mask;


    }

    return ret;
}


/* Function Name:
 *      dal_rtl9602c_classify_cfgEntry_add
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
dal_rtl9602c_classify_cfgEntry_add(rtk_classify_cfg_t *pClassifyCfg)
{
    rtl9602c_raw_cf_RuleEntry_t entry;
    rtl9602c_raw_cf_dsAct_t ds_act;
    rtl9602c_raw_cf_usAct_t us_act;
    uint32 patternIdx;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((1 < pClassifyCfg->templateIdx), RT_ERR_INPUT);


    /*Set entry template index*/
    if((ret = reg_array_field_write(RTL9602C_CF_TEMPLATE_SELr, REG_ARRAY_INDEX_NONE, pClassifyCfg->index, RTL9602C_SELf, &pClassifyCfg->templateIdx)) !=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    /* translate data structure */
    RT_ERR_CHK(dal_rtl9602c_classify_cfg2entry(pClassifyCfg, &entry, &us_act, &ds_act), ret);

    /* set entry rule */
    RT_ERR_CHK(rtl9602c_raw_cf_rule_set(&entry), ret);

    _dal_rtl9602c_classsify_patternIdx_get(pClassifyCfg->index, &patternIdx);

    /* set entry action*/
    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {
        RT_ERR_CHK(rtl9602c_raw_cf_usAct_set(&us_act), ret);

        /*Set pattern 0 action control bit*/
        if(RTL9602C_RAW_CF_PATTERN_0 == patternIdx)
        {
            RT_ERR_CHK(_dal_rtl9602c_classify_us_actCtrl_set(pClassifyCfg), ret);
        }
    }
    else
    {
        RT_ERR_CHK(rtl9602c_raw_cf_dsAct_set(&ds_act), ret);

        /*Set pattern 0 action control bit*/
        if(RTL9602C_RAW_CF_PATTERN_0 == patternIdx)
        {
            RT_ERR_CHK(_dal_rtl9602c_classify_ds_actCtrl_set(pClassifyCfg), ret);
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_cfgEntry_add */


/* Function Name:
 *      dal_rtl9602c_classify_cfgEntry_get
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
dal_rtl9602c_classify_cfgEntry_get(rtk_classify_cfg_t *pClassifyCfg)
{
    uint32 entryIdx;
    uint32 patternIdx;
    uint32 fieldData[RTL9602C_RAW_CF_ENTRYLEN];
    uint32 fieldMask[RTL9602C_RAW_CF_ENTRYLEN];
    rtl9602c_raw_cf_dsAct_t ds_act;
    rtl9602c_raw_cf_usAct_t us_act;
    int32 ret;
    uint32 idx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_CLASSIFY_ENTRY_MAX() <= pClassifyCfg->index), RT_ERR_NULL_POINTER);

    /*Get pattern and entry num*/
    _dal_rtl9602c_classsify_patternIdx_get(pClassifyCfg->index, &patternIdx);

    pClassifyCfg->patternIdx = patternIdx;

    entryIdx = pClassifyCfg->index;

    /*Get entry template index*/
    if((ret = reg_array_field_read(RTL9602C_CF_TEMPLATE_SELr, REG_ARRAY_INDEX_NONE, entryIdx, RTL9602C_SELf, &pClassifyCfg->templateIdx)) !=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    /* get entry rule raw data */
    switch(patternIdx)
    {
    case RTL9602C_RAW_CF_PATTERN_0:
        RT_ERR_CHK(table_read(RTL9602C_CF_RULE_48_P0t, entryIdx, fieldData), ret);
        RT_ERR_CHK(table_read(RTL9602C_CF_MASK_48_P0t, entryIdx, fieldMask), ret);
        break;
    case RTL9602C_RAW_CF_PATTERN_1:
    {
        uint32 cfTemplate;

        if((ret = reg_array_field_read(RTL9602C_CF_TEMPLATE_SELr, REG_ARRAY_INDEX_NONE, entryIdx, RTL9602C_SELf, &cfTemplate)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
        if(cfTemplate == 0)
        {
            RT_ERR_CHK(table_read(RTL9602C_CF_RULE_48_P1_T0t, entryIdx, fieldData), ret);
            RT_ERR_CHK(table_read(RTL9602C_CF_MASK_48_P1_T0t, entryIdx, fieldMask), ret);
        }
        else
        {
            RT_ERR_CHK(table_read(RTL9602C_CF_RULE_48_P1_T1t, entryIdx, fieldData), ret);
            RT_ERR_CHK(table_read(RTL9602C_CF_MASK_48_P1_T1t, entryIdx, fieldMask), ret);
        }
        break;
    }
    default:
        return RT_ERR_OUT_OF_RANGE;
    }

    for(idx = 0 ; idx < RTL9602C_RAW_CF_ENTRYLEN; idx++)
        fieldMask[idx] = fieldMask[idx]^fieldData[idx];

    osal_memset(&pClassifyCfg->field.readField, 0, sizeof(rtk_classify_raw_field_t));
    pClassifyCfg->field.readField.dataFieldRaw[0] = (fieldData[1]&0xFFFF);
    pClassifyCfg->field.readField.dataFieldRaw[1] = ((fieldData[1]>>16)&0xFFFF);
    pClassifyCfg->field.readField.careFieldRaw[0] = (fieldMask[1]&0xFFFF);
    pClassifyCfg->field.readField.careFieldRaw[1] = ((fieldMask[1]>>16)&0xFFFF);
    pClassifyCfg->field.readField.dataFieldRaw[2] = (fieldData[0]&0xFFFF);
    pClassifyCfg->field.readField.careFieldRaw[2] = (fieldMask[0]&0xFFFF);

    /* direction: bit 31 */
    if(((pClassifyCfg->field.readField.dataFieldRaw[1] & 0x8000) &
            (pClassifyCfg->field.readField.careFieldRaw[1] & 0x8000)) == 0)
        pClassifyCfg->direction = CLASSIFY_DIRECTION_US;
    else
        pClassifyCfg->direction = CLASSIFY_DIRECTION_DS;

    /*valid : Highest bit (for bit 48 only)*/
    pClassifyCfg->valid = ((fieldData[0]>>16)&0x1); /*bit 48*/

    /*Invert bit not use in RTL9602C, always set to disable*/
    pClassifyCfg->invert = CLASSIFY_INVERT_DISABLE;

    osal_memset(&ds_act, 0, sizeof(rtl9602c_raw_cf_dsAct_t));
    osal_memset(&us_act, 0, sizeof(rtl9602c_raw_cf_usAct_t));
    ds_act.idx = pClassifyCfg->index;
    us_act.idx = pClassifyCfg->index;
    /* get entry action */
    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {
        RT_ERR_CHK(rtl9602c_raw_cf_usAct_get(&us_act), ret);
    }
    else
    {
        RT_ERR_CHK(rtl9602c_raw_cf_dsAct_get(&ds_act), ret);
    }

    /* translate data structure */
    RT_ERR_CHK(dal_rtl9602c_classify_entry2cfg(&us_act, &ds_act, pClassifyCfg), ret);

    /*Get action control bit for pattern 0*/
    if(RTL9602C_RAW_CF_PATTERN_0 == patternIdx)
    {
        if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
        {
            RT_ERR_CHK(_dal_rtl9602c_classify_us_actCtrl_get(pClassifyCfg), ret);
        }
        else
        {
            RT_ERR_CHK(_dal_rtl9602c_classify_ds_actCtrl_get(pClassifyCfg), ret);
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_cfgEntry_get */


/* Function Name:
 *      dal_rtl9602c_classify_cfgEntry_del
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
dal_rtl9602c_classify_cfgEntry_del(uint32 entryIdx)
{
    rtl9602c_raw_cf_RuleEntry_t entry;
    rtl9602c_raw_cf_dsAct_t ds_act;
    rtl9602c_raw_cf_usAct_t us_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_ENTRY_MAX() <= entryIdx), RT_ERR_INPUT);

    osal_memset(&entry, 0x0, sizeof(rtl9602c_raw_cf_RuleEntry_t));
    osal_memset(&ds_act, 0x0, sizeof(rtl9602c_raw_cf_dsAct_t));
    osal_memset(&us_act, 0x0, sizeof(rtl9602c_raw_cf_usAct_t));

    entry.idx  = entryIdx;
    entry.valid = CF_ENTRY_INVALID;
    ds_act.idx = entryIdx;
    us_act.idx = entryIdx;

    RT_ERR_CHK(rtl9602c_raw_cf_rule_set(&entry), ret);
    RT_ERR_CHK(rtl9602c_raw_cf_dsAct_set(&ds_act), ret);
    RT_ERR_CHK(rtl9602c_raw_cf_usAct_set(&us_act), ret);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_cfgEntry_del */


/* Function Name:
 *      dal_rtl9602c_classify_field_add
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
 *      Pointer pFieldHead points to an Classification configuration structure, this structure keeps multiple
 *      comparison rules by means of linked list. Pointer pClassifyField will be added to linked
 *      list keeped by structure that pClassifyEntry points to.
 *      - caller should not free (*pClassifyField) before dal_rtl9602c_classify_cfgEntry_add is called
 */
int32
dal_rtl9602c_classify_field_add(rtk_classify_cfg_t *pClassifyEntry, rtk_classify_field_t *pClassifyField)
{
    rtk_classify_field_t *tmp_field;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pClassifyField), RT_ERR_NULL_POINTER);

    switch(pClassifyField->fieldType)
    {
    case CLASSIFY_FIELD_ETHERTYPE:
    case CLASSIFY_FIELD_TOS_DSIDX:
    case CLASSIFY_FIELD_TAG_VID:
    case CLASSIFY_FIELD_TAG_PRI:
    case CLASSIFY_FIELD_DEI:
    case CLASSIFY_FIELD_INTER_PRI:
    case CLASSIFY_FIELD_IS_CTAG:
    case CLASSIFY_FIELD_IS_STAG:
    case CLASSIFY_FIELD_UNI:

    case CLASSIFY_FIELD_ACL_HIT:
    case CLASSIFY_FIELD_WAN_IF:
    case CLASSIFY_FIELD_IGMP_MLD:
    case CLASSIFY_FIELD_INNER_VLAN:
    case CLASSIFY_FIELD_PPPOE:
    case CLASSIFY_FIELD_IPV4:
    case CLASSIFY_FIELD_IPV6:
    case CLASSIFY_FIELD_INTERNAL_VID_TAG_IF:
    case CLASSIFY_FIELD_CF_ACL_HIT:
    case CLASSIFY_FIELD_STPID_SEL:
    case CLASSIFY_FIELD_IPMC:
        break;

    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
        break;

    }

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
} /* end of dal_rtl9602c_classify_field_add */


/* Function Name:
 *      dal_rtl9602c_classify_unmatchAction_set
 * Description:
 *      Apply action to upstream packets when no classfication configuration match
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
dal_rtl9602c_classify_unmatchAction_set(rtk_classify_unmatch_action_t action)
{
    rtl9602c_raw_cf_usPermit_t raw_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((CLASSIFY_UNMATCH_END <= action), RT_ERR_INPUT);

    switch(action)
    {
    case CLASSIFY_UNMATCH_DROP:
        raw_act = RTL9602C_RAW_CF_US_PERMIT_DROP;
        break;
    case CLASSIFY_UNMATCH_PERMIT_WITHOUT_PON:
        raw_act = RTL9602C_RAW_CF_US_PERMIT_NOPON;
        break;
    case CLASSIFY_UNMATCH_PERMIT:
        raw_act = RTL9602C_RAW_CF_US_PERMIT_NORMAL;
        break;
    default:
        return RT_ERR_INPUT;
    }

    if ((ret = reg_field_write(RTL9602C_CF_CFGr, RTL9602C_CF_US_PERMITf, &raw_act)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_unmatchAction_set */


/* Function Name:
 *      dal_rtl9602c_classify_unmatchAction_get
 * Description:
 *      Get action to upstream packets when no classfication configuration match
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
dal_rtl9602c_classify_unmatchAction_get(rtk_classify_unmatch_action_t *pAction)
{
    rtl9602c_raw_cf_usPermit_t raw_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_CF_CFGr, RTL9602C_CF_US_PERMITf, &raw_act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    switch(raw_act)
    {
    case RTL9602C_RAW_CF_US_PERMIT_NORMAL:
        *pAction = CLASSIFY_UNMATCH_PERMIT;
        break;
    case RTL9602C_RAW_CF_US_PERMIT_NOPON:
        *pAction = CLASSIFY_UNMATCH_PERMIT_WITHOUT_PON;
        break;
    case RTL9602C_RAW_CF_US_PERMIT_DROP:
        *pAction = CLASSIFY_UNMATCH_DROP;
        break;
    default:
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_unmatchAction_get */

/* Function Name:
 *      dal_rtl9602c_classify_unmatchAction_ds_set
 * Description:
 *      Apply action to downstream packets when no classfication configuration match
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
dal_rtl9602c_classify_unmatchAction_ds_set(rtk_classify_unmatch_action_ds_t action)
{
    rtl9602c_raw_cf_dsPermit_t raw_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    switch(action)
    {
    case CLASSIFY_UNMATCH_DS_DROP:
        raw_act = RTL9602C_RAW_CF_DS_PERMIT_DROP;
        break;
    case CLASSIFY_UNMATCH_DS_PERMIT:
        raw_act = RTL9602C_RAW_CF_DS_PERMIT_NORMAL;
        break;
    default:
        return RT_ERR_INPUT;
    }

    if ((ret = reg_field_write(RTL9602C_CF_CFGr, RTL9602C_CF_DS_PERMITf, &raw_act)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_unmatchAction_ds_set */


/* Function Name:
 *      dal_rtl9602c_classify_unmatchAction_ds_get
 * Description:
 *      Get action to downstream packets when no classfication configuration match
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
dal_rtl9602c_classify_unmatchAction_ds_get(rtk_classify_unmatch_action_ds_t *pAction)
{
    rtl9602c_raw_cf_dsPermit_t raw_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_CF_CFGr, RTL9602C_CF_DS_PERMITf, &raw_act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    switch(raw_act)
    {
    case RTL9602C_RAW_CF_DS_PERMIT_NORMAL:
        *pAction = CLASSIFY_UNMATCH_DS_PERMIT;
        break;
    case RTL9602C_RAW_CF_DS_PERMIT_DROP:
        *pAction = CLASSIFY_UNMATCH_DS_DROP;
        break;
    default:
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_unmatchAction_ds_get */

/* Function Name:
 *      dal_rtl9602c_classify_cf_sel_set
 * Description:
 *      Set CF port selection, only pon port and RGMII port can be set
 * Input:
 *      port    - port id, only pon port and RGMII port can be set.
 *      cfSel   - CF port selection.
 * Output:
 *      N/A
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
dal_rtl9602c_classify_cf_sel_set(rtk_port_t port, rtk_classify_cf_sel_t cfSel)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    switch(cfSel)
    {
    case CLASSIFY_CF_SEL_DISABLE:
        val = CF_DISABLE;
        break;
    case CLASSIFY_CF_SEL_ENABLE:
        val = CF_ENABLE;
        break;
    default:
        return RT_ERR_OUT_OF_RANGE;
        break;
    }

    switch(port)
    {
    case 2:
        ret = reg_field_write(RTL9602C_CF_CFGr, RTL9602C_CF_SEL_PON_ENf, &val);
        break;
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }

    if(ret != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_cf_sel_set */

/* Function Name:
 *      dal_rtl9602c_classify_cf_sel_get
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
dal_rtl9602c_classify_cf_sel_get(rtk_port_t port, rtk_classify_cf_sel_t *pCfSel)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCfSel), RT_ERR_NULL_POINTER);

    switch(port)
    {
    case 2:
        ret = reg_field_read(RTL9602C_CF_CFGr, RTL9602C_CF_SEL_PON_ENf, &val);
        break;
    default:
        return RT_ERR_CHIP_NOT_SUPPORTED;
        break;
    }

    *pCfSel = (rtk_classify_cf_sel_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_cf_sel_get */


/* Function Name:
 *      dal_rtl9602c_classify_templateCfgPattern0_set
 * Description:
 *      Set classification pattern 0 template configuration
 * Input:
 *      P0TemplateCfg  -  pointer of pattern 0 template cfg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
int32
dal_rtl9602c_classify_templateCfgPattern0_set(rtk_classify_template_cfg_pattern0_t *pP0TemplateCfg)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pP0TemplateCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((1 < pP0TemplateCfg->index), RT_ERR_INPUT);
    RT_PARAM_CHK((CLASSIFY_DIRECTION_END <= pP0TemplateCfg->direction), RT_ERR_INPUT);


    switch(pP0TemplateCfg->vidCfg)
    {
    case CLASSIFY_TEMPLATE_VID_CFG_INGRESS_CVID:
        val = RTL9602C_RAW_CF_TEMPLATE_VID_CFG_INGRESS_CVID;
        break;
    case CLASSIFY_TEMPLATE_VID_CFG_INGRESS_OUTERTAG_VID:
        val = RTL9602C_RAW_CF_TEMPLATE_VID_CFG_INGRESS_OUTERTAG_VID;
        break;
    case CLASSIFY_TEMPLATE_VID_CFG_INTERNAL_CVID:
        val = RTL9602C_RAW_CF_TEMPLATE_VID_CFG_INTERNAL_CVID;
        break;
    default:
        return RT_ERR_OUT_OF_RANGE;
        break;
    }

    if(pP0TemplateCfg->direction == CLASSIFY_DIRECTION_US)
    {
        if((ret = reg_array_field_write(RTL9602C_CF_PATTERN0_TEMPLATE_CFGr, REG_ARRAY_INDEX_NONE, pP0TemplateCfg->index, RTL9602C_US_VID_CFGf, &val)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
    } else {
        if((ret = reg_array_field_write(RTL9602C_CF_PATTERN0_TEMPLATE_CFGr, REG_ARRAY_INDEX_NONE, pP0TemplateCfg->index, RTL9602C_DS_VID_CFGf, &val)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
    }

    switch(pP0TemplateCfg->priCfg)
    {
    case CLASSIFY_TEMPLATE_PRI_CFG_INGRESS_CPRI:
        val = RTL9602C_RAW_CF_TEMPLATE_PRI_CFG_INGRESS_CPRI;
        break;
    case CLASSIFY_TEMPLATE_PRI_CFG_INTERNAL_PRI:
        val = RTL9602C_RAW_CF_TEMPLATE_PRI_CFG_INTERNAL_PRI;
        break;
    case CLASSIFY_TEMPLATE_PRI_CFG_REMARK_CPRI:
        val = RTL9602C_RAW_CF_TEMPLATE_PRI_CFG_REMARK_CPRI;
        break;
    default:
        return RT_ERR_OUT_OF_RANGE;
        break;
    }

    if(pP0TemplateCfg->direction == CLASSIFY_DIRECTION_US)
    {
        if((ret = reg_array_field_write(RTL9602C_CF_PATTERN0_TEMPLATE_CFGr, REG_ARRAY_INDEX_NONE, pP0TemplateCfg->index, RTL9602C_US_PRI_CFGf, &val)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
    } else {
        if((ret = reg_array_field_write(RTL9602C_CF_PATTERN0_TEMPLATE_CFGr, REG_ARRAY_INDEX_NONE, pP0TemplateCfg->index, RTL9602C_DS_PRI_CFGf, &val)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }

    }

    return RT_ERR_OK;
}/* end of dal_rtl9602c_classify_templateCfgPattern0_set */

/* Function Name:
 *      dal_rtl9602c_classify_templateCfgPattern0_get
 * Description:
 *      Get classification pattern 0 template configuration
 * Input:
 *      P0TemplateCfg  -  pointer of pattern 0 template cfg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
int32
dal_rtl9602c_classify_templateCfgPattern0_get( rtk_classify_template_cfg_pattern0_t *pP0TemplateCfg)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);
    RT_PARAM_CHK((NULL == pP0TemplateCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((1 < pP0TemplateCfg->index), RT_ERR_INPUT);
    RT_PARAM_CHK((CLASSIFY_DIRECTION_END <= pP0TemplateCfg->direction), RT_ERR_INPUT);

    if(pP0TemplateCfg->direction == CLASSIFY_DIRECTION_US)
    {
        if((ret = reg_array_field_read(RTL9602C_CF_PATTERN0_TEMPLATE_CFGr, REG_ARRAY_INDEX_NONE, pP0TemplateCfg->index, RTL9602C_US_VID_CFGf, &val)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
    } else {
        if((ret = reg_array_field_read(RTL9602C_CF_PATTERN0_TEMPLATE_CFGr, REG_ARRAY_INDEX_NONE, pP0TemplateCfg->index, RTL9602C_DS_VID_CFGf, &val)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
    }

    switch(val)
    {
    case RTL9602C_RAW_CF_TEMPLATE_VID_CFG_INGRESS_CVID:
        pP0TemplateCfg->vidCfg = CLASSIFY_TEMPLATE_VID_CFG_INGRESS_CVID;
        break;
    case RTL9602C_RAW_CF_TEMPLATE_VID_CFG_INGRESS_OUTERTAG_VID:
        pP0TemplateCfg->vidCfg = CLASSIFY_TEMPLATE_VID_CFG_INGRESS_OUTERTAG_VID;
        break;
    case RTL9602C_RAW_CF_TEMPLATE_VID_CFG_INTERNAL_CVID:
        pP0TemplateCfg->vidCfg = CLASSIFY_TEMPLATE_VID_CFG_INTERNAL_CVID;
        break;
    default:
        return RT_ERR_OUT_OF_RANGE;
        break;
    }

    if(pP0TemplateCfg->direction == CLASSIFY_DIRECTION_US)
    {
        if((ret = reg_array_field_read(RTL9602C_CF_PATTERN0_TEMPLATE_CFGr, REG_ARRAY_INDEX_NONE, pP0TemplateCfg->index, RTL9602C_US_PRI_CFGf, &val)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }
    } else {
        if((ret = reg_array_field_read(RTL9602C_CF_PATTERN0_TEMPLATE_CFGr, REG_ARRAY_INDEX_NONE, pP0TemplateCfg->index, RTL9602C_DS_PRI_CFGf, &val)) !=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
            return ret;
        }

    }

    switch(val)
    {
    case RTL9602C_RAW_CF_TEMPLATE_PRI_CFG_INGRESS_CPRI:
        pP0TemplateCfg->priCfg = CLASSIFY_TEMPLATE_PRI_CFG_INGRESS_CPRI;
        break;
    case RTL9602C_RAW_CF_TEMPLATE_PRI_CFG_INTERNAL_PRI:
        pP0TemplateCfg->priCfg = CLASSIFY_TEMPLATE_PRI_CFG_INTERNAL_PRI;
        break;
    case RTL9602C_RAW_CF_TEMPLATE_PRI_CFG_REMARK_CPRI:
        pP0TemplateCfg->priCfg = CLASSIFY_TEMPLATE_PRI_CFG_REMARK_CPRI;
        break;
    default:
        return RT_ERR_OUT_OF_RANGE;
        break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_templateCfgPattern0_get */



/* Function Name:
 *      dal_rtl9602c_classify_entryNumPattern1_set
 * Description:
 *      Set classification entry number of pattern1
 * Input:
 *      entryNum   - entry number of pattern1
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
int32
dal_rtl9602c_classify_entryNumPattern1_set( uint32 entryNum)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);
    /* parameter check */
    RT_PARAM_CHK((RTL9602C_CF_PATTERN1_ENTRY_MAX < entryNum), RT_ERR_INPUT);

    if((ret = reg_field_write(RTL9602C_CF_CFGr, RTL9602C_CF_PATTERN1_NUMf, &entryNum)) !=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_entryNumPattern1_set */


/* Function Name:
 *      dal_rtl9602c_classify_entryNumPattern1_get
 * Description:
 *      Get classification entry number of pattern1
 * Input:
 *      None
 * Output:
 *      entryNum   - entry number of pattern1
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *
 */
int32
dal_rtl9602c_classify_entryNumPattern1_get( uint32 *pEntryNum)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);
    RT_PARAM_CHK((NULL == pEntryNum), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(RTL9602C_CF_CFGr, RTL9602C_CF_PATTERN1_NUMf, pEntryNum)) !=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_entryNumPattern1_get */

/* Function Name:
 *      dal_rtl9602c_classify_defaultWanIf_set
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
dal_rtl9602c_classify_defaultWanIf_set( rtk_classify_default_wanIf_t *pDefaultWanIf)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);
    RT_PARAM_CHK((RTL9602C_L34_NETIF_TABLE_MAX <= pDefaultWanIf->l2WanIf), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9602C_L34_NETIF_TABLE_MAX <= pDefaultWanIf->mcastWanIf), RT_ERR_INPUT);

    if((ret = reg_field_write(RTL9602C_CF_CFGr, RTL9602C_WANIF_DEFAULT_L2f, &pDefaultWanIf->l2WanIf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if((ret = reg_field_write(RTL9602C_CF_CFGr, RTL9602C_WANIF_DEFAULT_MULTICASTf, &pDefaultWanIf->mcastWanIf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_entryNumPattern1_get */

/* Function Name:
 *      dal_rtl9602c_classify_defaultWanIf_get
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
dal_rtl9602c_classify_defaultWanIf_get( rtk_classify_default_wanIf_t *pDefaultWanIf)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);
    RT_PARAM_CHK((NULL == pDefaultWanIf), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(RTL9602C_CF_CFGr, RTL9602C_WANIF_DEFAULT_L2f, &pDefaultWanIf->l2WanIf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if((ret = reg_field_read(RTL9602C_CF_CFGr, RTL9602C_WANIF_DEFAULT_MULTICASTf, &pDefaultWanIf->mcastWanIf)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_classify_entryNumPattern1_get */

