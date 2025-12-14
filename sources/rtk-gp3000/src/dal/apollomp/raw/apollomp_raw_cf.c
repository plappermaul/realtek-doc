/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : switch asic-level classification API
 * Feature : Classification related functions
 *
 */

#include <hal/chipdef/apollomp/apollomp_def.h>
#include <dal/apollomp/raw/apollomp_raw_cf.h>

/* Function Name:
 *      apollomp_raw_cf_vlaid_set
 * Description:
 *      Set classification action valid control
 * Input:
 *      index 	       - index of the classification (0 - 511)
 *      enabled 	- Enable or disable  the rule of index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK             - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *	    None
 */

int32 apollomp_raw_cf_valid_set(uint32 index, rtk_enable_t enabled)
{
	int32 ret;
	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(index >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(enabled >= RTK_ENABLE_END, RT_ERR_OUT_OF_RANGE);
    if ((ret = reg_array_field_write(APOLLOMP_CF_VALIDr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VALIDf, &enabled)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
	return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_cf_valid_get
 * Description:
 *      Get classification downstrean action control
 * Input:
 *      index 	- index of the classification (0 - 511)
 * Output:
  *      pEnabled 	- The valid status the rule with  index
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *	    None
 */

int32 apollomp_raw_cf_valid_get(uint32 index, rtk_enable_t *pEnabled)

{
    int32 ret;
	ret = RT_ERR_FAILED;
    RT_PARAM_CHK(pEnabled == NULL,RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(index >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);

    if ((ret = reg_array_field_read(APOLLOMP_CF_VALIDr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VALIDf, pEnabled)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_cf_rule_set
 * Description:
 *      Set classification pattern content
 * Input:
 *      index 	-index of the classification (0 - 511)
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
int32 apollomp_raw_cf_rule_set(apollomp_raw_cf_RuleEntry_t *pattern)
{

    int32 ret;
    uint32 entryIdx; /*use which entry*/
    uint32 patternIdx; /*use which entry*/
    uint32 fieldData[APOLLOMP_RAW_CF_ENTRYLEN];
    uint32 fieldMask[APOLLOMP_RAW_CF_ENTRYLEN];
    uint32 tmpData;


    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(NULL == pattern, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pattern->idx >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->valid >= CF_ENTRY_VALID_END, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pattern->care_bits.etherType > 0xffff, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.ifCtag > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.ifStag > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.interPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.tagPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.tagVid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.direction >= CF_DIRECTION_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.uni > 0x7, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.ipRange > (0x8|(APOLLOMP_CLASSIFY_IP_RANGE_NUM-1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.portRange > (0x8|(APOLLOMP_CLASSIFY_L4PORT_RANGE_NUM-1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.aclHit > (0x80|(APOLLOMP_MAX_NUM_OF_ACL_ACTION-1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.wanIf >= APOLLOMP_L34_NETIF_TABLE_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.ip6Mc > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.ip4Mc > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.igmp > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.mld > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->care_bits.dei > 1, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pattern->data_bits.etherType > 0xffff, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.ifCtag > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.ifStag > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.interPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.tagPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.tagVid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.direction >= CF_DIRECTION_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.uni > 0x7, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.ipRange > (0x8|(APOLLOMP_CLASSIFY_IP_RANGE_NUM-1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.portRange > (0x8|(APOLLOMP_CLASSIFY_L4PORT_RANGE_NUM-1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.aclHit > (0x80|(APOLLOMP_MAX_NUM_OF_ACL_ACTION-1)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.wanIf >= APOLLOMP_L34_NETIF_TABLE_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.ip6Mc > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.ip4Mc > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.igmp > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.mld > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pattern->data_bits.dei > 1, RT_ERR_OUT_OF_RANGE);


    if(1 == pattern->data_bits.direction)
    {
        RT_PARAM_CHK(pattern->care_bits.tos_gemidx >= APOLLOMP_CLASSIFY_SID_MAX, RT_ERR_OUT_OF_RANGE);
        RT_PARAM_CHK(pattern->data_bits.tos_gemidx >= APOLLOMP_CLASSIFY_SID_MAX, RT_ERR_OUT_OF_RANGE);
    }
    else
    {
        RT_PARAM_CHK(pattern->care_bits.tos_gemidx > 0xff, RT_ERR_OUT_OF_RANGE);
        RT_PARAM_CHK(pattern->data_bits.tos_gemidx > 0xff, RT_ERR_OUT_OF_RANGE);
    }

    entryIdx = pattern->idx&0x7f;
    patternIdx = pattern->idx>>7;

    osal_memset(fieldData, 0, sizeof(fieldData));
    osal_memset(fieldMask, 0, sizeof(fieldData));

        if ((ret = table_read(APOLLOMP_CF_RULEt, entryIdx, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
            return ret;
        }
        if ((ret = table_read(APOLLOMP_CF_MASKt, entryIdx, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
            return ret;
        }

    switch (patternIdx)
    {
        case APOLLOMP_RAW_CF_PATTERN_0:
            /*mask*/
            if(pattern->idx < 64)
            {
                tmpData = pattern->care_bits.portRange&(~pattern->data_bits.portRange);
                if ((ret = table_field_set(APOLLOMP_CF_MASK_L34t, APOLLOMP_CF_MASK_L34_PORT_RNG_CHK_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->care_bits.ipRange&(~pattern->data_bits.ipRange);
                if ((ret = table_field_set(APOLLOMP_CF_MASK_L34t, APOLLOMP_CF_MASK_L34_IP_RNG_CHK_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->care_bits.aclHit&(~pattern->data_bits.aclHit);
                if ((ret = table_field_set(APOLLOMP_CF_MASK_L34t, APOLLOMP_CF_MASK_L34_ACL_HIT_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->care_bits.wanIf&(~pattern->data_bits.wanIf);
                if ((ret = table_field_set(APOLLOMP_CF_MASK_L34t, APOLLOMP_CF_MASK_L34_WAN_IF_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->care_bits.ip6Mc&(~pattern->data_bits.ip6Mc);
                if ((ret = table_field_set(APOLLOMP_CF_MASK_L34t, APOLLOMP_CF_MASK_L34_IP6_MCtf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->care_bits.ip4Mc&(~pattern->data_bits.ip4Mc);
                if ((ret = table_field_set(APOLLOMP_CF_MASK_L34t, APOLLOMP_CF_MASK_L34_IP4_MCtf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->care_bits.igmp&(~pattern->data_bits.igmp);
                if ((ret = table_field_set(APOLLOMP_CF_MASK_L34t, APOLLOMP_CF_MASK_L34_IGMPtf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->care_bits.mld&(~pattern->data_bits.mld);
                if ((ret = table_field_set(APOLLOMP_CF_MASK_L34t, APOLLOMP_CF_MASK_L34_MLDtf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->care_bits.dei&(~pattern->data_bits.dei);
                if ((ret = table_field_set(APOLLOMP_CF_MASK_L34t, APOLLOMP_CF_MASK_L34_DEItf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
            }
            else
            {
                tmpData = pattern->care_bits.etherType&(~pattern->data_bits.etherType);
                if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_ETH_TYPE_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->care_bits.tos_gemidx&(~pattern->data_bits.tos_gemidx);
                if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_TOS_GEMIDX_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
            }
            tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_U_D_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagVid&(~pattern->data_bits.tagVid);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_VID_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagPri&(~pattern->data_bits.tagPri);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_PRI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.interPri&(~pattern->data_bits.interPri);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_INTER_PRI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_STAG_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_CTAG_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_UNI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            /*data*/
            if(pattern->idx < 64)
            {
                tmpData = pattern->data_bits.portRange&pattern->care_bits.portRange;
                if ((ret = table_field_set(APOLLOMP_CF_RULE_L34t, APOLLOMP_CF_RULE_L34_PORT_RNG_CHK_0tf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->data_bits.ipRange&pattern->care_bits.ipRange;
                if ((ret = table_field_set(APOLLOMP_CF_RULE_L34t, APOLLOMP_CF_RULE_L34_IP_RNG_CHK_0tf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->data_bits.aclHit&pattern->care_bits.aclHit;
                if ((ret = table_field_set(APOLLOMP_CF_RULE_L34t, APOLLOMP_CF_RULE_L34_ACL_HIT_0tf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->data_bits.wanIf&pattern->care_bits.wanIf;
                if ((ret = table_field_set(APOLLOMP_CF_RULE_L34t, APOLLOMP_CF_RULE_L34_WAN_IF_0tf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->data_bits.ip6Mc&pattern->care_bits.ip6Mc;
                if ((ret = table_field_set(APOLLOMP_CF_RULE_L34t, APOLLOMP_CF_RULE_L34_IP6_MCtf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->data_bits.ip4Mc&pattern->care_bits.ip4Mc;
                if ((ret = table_field_set(APOLLOMP_CF_RULE_L34t, APOLLOMP_CF_RULE_L34_IP4_MCtf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->data_bits.igmp&pattern->care_bits.igmp;
                if ((ret = table_field_set(APOLLOMP_CF_RULE_L34t, APOLLOMP_CF_RULE_L34_IGMPtf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->data_bits.mld&pattern->care_bits.mld;
                if ((ret = table_field_set(APOLLOMP_CF_RULE_L34t, APOLLOMP_CF_RULE_L34_MLDtf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->data_bits.dei&pattern->care_bits.dei;
                if ((ret = table_field_set(APOLLOMP_CF_RULE_L34t, APOLLOMP_CF_RULE_L34_DEItf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
            }
            else
            {
                tmpData = pattern->data_bits.etherType&pattern->care_bits.etherType;
                if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_ETH_TYPE_0tf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
                tmpData = pattern->data_bits.tos_gemidx&pattern->care_bits.tos_gemidx;
                if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_TOS_GEMIDX_0tf, &tmpData, fieldData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }
            }
            tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_U_D_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagVid&pattern->care_bits.tagVid;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_VID_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagPri&pattern->care_bits.tagPri;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_PRI_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.interPri & pattern->care_bits.interPri;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_INTER_PRI_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_STAG_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifCtag & pattern->care_bits.ifCtag;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_CTAG_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.uni & pattern->care_bits.uni;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_UNI_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            break;

        case APOLLOMP_RAW_CF_PATTERN_1:
            /* mask */
            tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_U_D_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tos_gemidx&(~pattern->data_bits.tos_gemidx);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_TOS_GEMIDX_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagVid&(~pattern->data_bits.tagVid);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_VID_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagPri&(~pattern->data_bits.tagPri);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_PRI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.interPri&(~pattern->data_bits.interPri);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_INTER_PRI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_STAG_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_CTAG_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_UNI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /*data*/
            tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_U_D_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tos_gemidx&pattern->care_bits.tos_gemidx;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_TOS_GEMIDX_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagVid&pattern->care_bits.tagVid;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_VID_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagPri&pattern->care_bits.tagPri;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_PRI_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.interPri&pattern->care_bits.interPri;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_INTER_PRI_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_STAG_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifCtag&pattern->care_bits.ifCtag;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_CTAG_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.uni&pattern->care_bits.uni;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_UNI_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_RAW_CF_PATTERN_2:
            /* mask */
            tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_U_D_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tos_gemidx&(~pattern->data_bits.tos_gemidx);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_TOS_GEMIDX_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagVid&(~pattern->data_bits.tagVid);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_VID_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagPri&(~pattern->data_bits.tagPri);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_PRI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.interPri&(~pattern->data_bits.interPri);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_INTER_PRI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_STAG_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_CTAG_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_UNI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /*data*/
            tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_U_D_2tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tos_gemidx&pattern->care_bits.tos_gemidx;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_TOS_GEMIDX_2tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagVid&pattern->care_bits.tagVid;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_VID_2tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagPri&pattern->care_bits.tagPri;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_PRI_2tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.interPri&pattern->care_bits.interPri;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_INTER_PRI_2tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_STAG_2tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifCtag&pattern->care_bits.ifCtag;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_CTAG_2tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.uni&pattern->care_bits.uni;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_UNI_2tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            break;
        case APOLLOMP_RAW_CF_PATTERN_3:
            /* mask */
            tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_U_D_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tos_gemidx&(~pattern->data_bits.tos_gemidx);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_TOS_GEMIDX_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagVid&(~pattern->data_bits.tagVid);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_VID_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.tagPri&(~pattern->data_bits.tagPri);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_PRI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.interPri&(~pattern->data_bits.interPri);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_INTER_PRI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_STAG_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_CTAG_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
            if ((ret = table_field_set(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_UNI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /*data*/
            tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_U_D_3tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tos_gemidx&pattern->care_bits.tos_gemidx;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_TOS_GEMIDX_3tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagVid&pattern->care_bits.tagVid;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_VID_3tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.tagPri&pattern->care_bits.tagPri;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_PRI_3tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.interPri&pattern->care_bits.interPri;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_INTER_PRI_3tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_STAG_3tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.ifCtag&pattern->care_bits.ifCtag;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_CTAG_3tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            tmpData = pattern->data_bits.uni&pattern->care_bits.uni;
            if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_UNI_3tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            break;

        default:
            return RT_ERR_OUT_OF_RANGE;
    }
    /* vailid bit */
    if ((ret = table_field_set(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_VALIDtf, &pattern->valid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_write(APOLLOMP_CF_RULEt, entryIdx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_write(APOLLOMP_CF_MASKt, entryIdx, fieldMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_cf_rule_get
 * Description:
  *      Set classification pattern content
 * Input:
 *      index 	-index of the classification
 * Output:
 *      pattern  - the pattern of the classification
 * Return:
 *      RT_ERR_OK             - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
int32 apollomp_raw_cf_rule_get(apollomp_raw_cf_RuleEntry_t *pattern)

{
    int32 ret;
    uint32 entryIdx; /*use which entry*/
    uint32 patternIdx; /*use which entry*/
    uint32 fieldData[APOLLOMP_RAW_CF_ENTRYLEN];
    uint32 fieldMask[APOLLOMP_RAW_CF_ENTRYLEN];
    uint32 tmpData;


    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(NULL == pattern, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pattern->idx >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
    entryIdx = pattern->idx&0x7f;
    patternIdx = pattern->idx>>7;
    osal_memset(fieldData, 0, sizeof(fieldData));
    osal_memset(fieldData, 0, sizeof(fieldMask));

    if ((ret = table_read(APOLLOMP_CF_RULEt, entryIdx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_read(APOLLOMP_CF_MASKt, entryIdx, fieldMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    switch (patternIdx)
    {

        case APOLLOMP_RAW_CF_PATTERN_0:
            /*data*/
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_ETH_TYPE_0tf, &(pattern->data_bits.etherType), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_U_D_0tf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_TOS_GEMIDX_0tf, &(pattern->data_bits.tos_gemidx), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_VID_0tf, &(pattern->data_bits.tagVid), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_PRI_0tf, &(pattern->data_bits.tagPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_INTER_PRI_0tf, &(pattern->data_bits.interPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_STAG_0tf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_CTAG_0tf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_UNI_0tf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            /* mask */
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_ETH_TYPE_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.etherType = tmpData^pattern->data_bits.etherType;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_U_D_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.direction = tmpData^pattern->data_bits.direction;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_TOS_GEMIDX_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tos_gemidx = tmpData^pattern->data_bits.tos_gemidx;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_VID_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagVid = tmpData^pattern->data_bits.tagVid;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_PRI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagPri = tmpData^pattern->data_bits.tagPri;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_INTER_PRI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.interPri = tmpData^pattern->data_bits.interPri;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_STAG_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_CTAG_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_UNI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.uni = tmpData^pattern->data_bits.uni;
            break;

        case APOLLOMP_RAW_CF_PATTERN_1:
            /*data*/
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_U_D_1tf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_TOS_GEMIDX_1tf, &(pattern->data_bits.tos_gemidx), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_VID_1tf, &(pattern->data_bits.tagVid), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_PRI_1tf, &(pattern->data_bits.tagPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_INTER_PRI_1tf, &(pattern->data_bits.interPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_STAG_1tf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_CTAG_1tf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_UNI_1tf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /* mask */
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_U_D_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.direction = tmpData^pattern->data_bits.direction;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_TOS_GEMIDX_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tos_gemidx = tmpData^pattern->data_bits.tos_gemidx;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_VID_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagVid = tmpData^pattern->data_bits.tagVid;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_PRI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagPri = tmpData^pattern->data_bits.tagPri;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_INTER_PRI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.interPri = tmpData^pattern->data_bits.interPri;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_STAG_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_CTAG_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_UNI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.uni = tmpData^pattern->data_bits.uni;
            break;
        case APOLLOMP_RAW_CF_PATTERN_2:
            /*data*/
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_U_D_2tf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_TOS_GEMIDX_2tf, &(pattern->data_bits.tos_gemidx), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_VID_2tf, &(pattern->data_bits.tagVid), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_PRI_2tf, &(pattern->data_bits.tagPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_INTER_PRI_2tf, &(pattern->data_bits.interPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_STAG_2tf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_CTAG_2tf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_UNI_2tf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }

            /* mask */
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_U_D_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.direction = tmpData^pattern->data_bits.direction;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_TOS_GEMIDX_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tos_gemidx = tmpData^pattern->data_bits.tos_gemidx;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_VID_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagVid = tmpData^pattern->data_bits.tagVid;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_PRI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagPri = tmpData^pattern->data_bits.tagPri;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_INTER_PRI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.interPri = tmpData^pattern->data_bits.interPri;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_STAG_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_CTAG_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_UNI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.uni = tmpData^pattern->data_bits.uni;
            break;

        case APOLLOMP_RAW_CF_PATTERN_3:
            /* data */
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_U_D_3tf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_TOS_GEMIDX_3tf, &(pattern->data_bits.tos_gemidx), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_VID_3tf, &(pattern->data_bits.tagVid), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_PRI_3tf, &(pattern->data_bits.tagPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_INTER_PRI_3tf, &(pattern->data_bits.interPri), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_STAG_3tf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_IF_CTAG_3tf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_UNI_3tf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            /* mask */
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_U_D_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.direction = tmpData^pattern->data_bits.direction;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_TOS_GEMIDX_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tos_gemidx = tmpData^pattern->data_bits.tos_gemidx;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_VID_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagVid = tmpData^pattern->data_bits.tagVid;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_PRI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.tagPri = tmpData^pattern->data_bits.tagPri;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_INTER_PRI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.interPri = tmpData^pattern->data_bits.interPri;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_STAG_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_IF_CTAG_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
            if ((ret = table_field_get(APOLLOMP_CF_MASKt, APOLLOMP_CF_MASK_UNI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                return ret;
            }
            pattern->care_bits.uni = tmpData^pattern->data_bits.uni;
            break;

        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    /* vailid bit */
    if ((ret = table_field_get(APOLLOMP_CF_RULEt, APOLLOMP_CF_RULE_VALIDtf, &pattern->valid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_cf_dsAct_set
 * Description:
 *      Set classification downstrean action control
 * Input:
 *      index 	- index of the classification (0 - 511)
 *      pCfDsAct 	- CF down stream action stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *	    None
 */

int32 apollomp_raw_cf_dsAct_set(apollomp_raw_cf_dsAct_t *pCfDsAct)
{
	int32 ret;
	uint32 fieldData[APOLLOMP_RAW_CF_DSACTTBLEN];

    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(NULL == pCfDsAct, RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(pCfDsAct->idx >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfDsAct->csact >= CF_DS_CSACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->svid_act > CF_DS_VID_ACT_FROM_2ND_TAG, RT_ERR_OUT_OF_RANGE);
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

    osal_memset(fieldData, 0, sizeof(fieldData));

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CSACTtf, &pCfDsAct->csact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CSPRI_ACTtf, &pCfDsAct->spri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CSVID_ACTtf, &pCfDsAct->svid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CS_PRItf, &pCfDsAct->s_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CS_VIDtf, &pCfDsAct->s_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CACTtf, &pCfDsAct->cact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CVID_ACTtf, &pCfDsAct->cvid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CPRI_ACTtf, &pCfDsAct->cpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_C_VIDtf, &pCfDsAct->c_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_C_PRItf, &pCfDsAct->c_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CFPRI_ACTtf, &pCfDsAct->cfpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CF_PRItf, &pCfDsAct->cfpri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_UNI_ACTtf, &pCfDsAct->uni_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_UNI_PMSKtf, &pCfDsAct->uni_mask, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_DSCP_REMARKtf, &pCfDsAct->dscp_remark, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_write(APOLLOMP_CF_ACTION_DSt, pCfDsAct->idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
	return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_cf_dsAct_get
 * Description:
 *      Get classification downstrean action control
 * Input:
 *      index 	- index of the classification (0 - 511)
 * Output:
 *      pCfDsAct 	- CF down stream action stucture for setting
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *	    None
 */
int32 apollomp_raw_cf_dsAct_get(apollomp_raw_cf_dsAct_t *pCfDsAct)
{
	int32 ret;
	uint32 fieldData[APOLLOMP_RAW_CF_DSACTTBLEN];

    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(NULL == pCfDsAct, RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(pCfDsAct->idx >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);



    osal_memset(fieldData, 0, sizeof(fieldData));
    if ((ret = table_read(APOLLOMP_CF_ACTION_DSt, pCfDsAct->idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CSACTtf, &pCfDsAct->csact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CSVID_ACTtf, &pCfDsAct->svid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CSPRI_ACTtf, &pCfDsAct->spri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CS_VIDtf, &pCfDsAct->s_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CS_PRItf, &pCfDsAct->s_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CACTtf, &pCfDsAct->cact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CVID_ACTtf, &pCfDsAct->cvid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CPRI_ACTtf, &pCfDsAct->cpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_C_VIDtf, &pCfDsAct->c_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_C_PRItf, &pCfDsAct->c_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CFPRI_ACTtf, &pCfDsAct->cfpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_CF_PRItf, &pCfDsAct->cfpri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_UNI_ACTtf, &pCfDsAct->uni_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_UNI_PMSKtf, &pCfDsAct->uni_mask, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_CF_ACTION_DSt, APOLLOMP_CF_ACTION_DS_DSCP_REMARKtf, &pCfDsAct->dscp_remark, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_cf_usAct_set
 * Description:
 *      Set classification downstrean action control
 * Input:
 *      index 	- index of the classification (0 - 511)
 *      pCfUsAct 	- CF up stream action stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *	    None
 */

int32 apollomp_raw_cf_usAct_set(apollomp_raw_cf_usAct_t *pCfUsAct)
{
	int32 ret;
	uint32 fieldData[APOLLOMP_RAW_CF_UPACTTBLEN];
    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(pCfUsAct == NULL, RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(pCfUsAct->idx >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
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
    RT_PARAM_CHK(pCfUsAct->assign_idx >= APOLLOMP_CLASSIFY_SID_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfUsAct->cfpri_act >= CF_CF_PRI_ACT_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->cfpri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfUsAct->log_act >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->log_idx >= APOLLOMP_CLASSIFY_SID_MAX, RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK(pCfUsAct->dscp_remark >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfUsAct->drop_act >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);

    osal_memset(fieldData, 0, sizeof(fieldData));

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CSACTtf, &pCfUsAct->csact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CSVID_ACTtf, &pCfUsAct->svid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CSPRI_ACTtf, &pCfUsAct->spri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CS_VIDtf, &pCfUsAct->s_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CS_PRItf, &pCfUsAct->s_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CACTtf, &pCfUsAct->cact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CVID_ACTtf, &pCfUsAct->cvid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CPRI_ACTtf, &pCfUsAct->cpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_C_VIDtf, &pCfUsAct->c_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_C_PRItf, &pCfUsAct->c_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_SID_ACTtf, &pCfUsAct->sid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_ASSIGN_IDXtf, &pCfUsAct->assign_idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CFPRI_ACTtf, &pCfUsAct->cfpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CF_PRItf, &pCfUsAct->cfpri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_LOGtf, &pCfUsAct->log_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_COUNTER_IDXtf, &pCfUsAct->log_idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_DSCP_REMARKtf, &pCfUsAct->dscp_remark, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_set(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_DROPtf, &pCfUsAct->drop_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_write(APOLLOMP_CF_ACTION_USt, pCfUsAct->idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
	return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_cf_usAct_get
 * Description:
 *      Get classification downstrean action control
 * Input:
 *      index 	- index of the classification (0 - 511)
 * Output:
 *      pCfUsAct 	- CF down stream action stucture for setting
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *	    None
 */

int32 apollomp_raw_cf_usAct_get(apollomp_raw_cf_usAct_t *pCfUsAct)
{
	int32 ret;
	uint32 fieldData[APOLLOMP_RAW_CF_UPACTTBLEN];

    ret = RT_ERR_FAILED;
	RT_PARAM_CHK(pCfUsAct->idx >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pCfUsAct == NULL, RT_ERR_NULL_POINTER);

	osal_memset(fieldData, 0, sizeof(fieldData));
    if ((ret = table_read(APOLLOMP_CF_ACTION_USt, pCfUsAct->idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CSACTtf, &pCfUsAct->csact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CSVID_ACTtf, &pCfUsAct->svid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CSPRI_ACTtf, &pCfUsAct->spri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CS_VIDtf, &pCfUsAct->s_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CS_PRItf, &pCfUsAct->s_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CACTtf, &pCfUsAct->cact, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CVID_ACTtf, &pCfUsAct->cvid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CPRI_ACTtf, &pCfUsAct->cpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_C_VIDtf, &pCfUsAct->c_vid, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_C_PRItf, &pCfUsAct->c_pri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_SID_ACTtf, &pCfUsAct->sid_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_ASSIGN_IDXtf, &pCfUsAct->assign_idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CFPRI_ACTtf, &pCfUsAct->cfpri_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_CF_PRItf, &pCfUsAct->cfpri, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_LOGtf, &pCfUsAct->log_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_COUNTER_IDXtf, &pCfUsAct->log_idx, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_DSCP_REMARKtf, &pCfUsAct->dscp_remark, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    if ((ret = table_field_get(APOLLOMP_CF_ACTION_USt, APOLLOMP_CF_ACTION_US_DROPtf, &pCfUsAct->drop_act, fieldData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

	return RT_ERR_OK;


}

/* Function Name:
 *      apollomp_raw_cf_dsOp_set
 * Description:
 *      Set not operation config for downstream classification rule
 * Input:
 *      index         - index of the classification (0 - 511)
 *      notDs        - ENABLED: not opreation
 *                      - DISABLED: normal operation
*
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32 apollomp_raw_cf_dsOp_set(uint32 index, rtk_enable_t notDs)
{
    int32 ret;
    uint32 tmpData;

    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(index >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(notDs >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);

    tmpData = notDs;

    if ((ret = reg_array_field_write(APOLLOMP_CF_OP_DSr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_NOT_DSf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_cf_dsOp_get
 * Description:
 *      Get not operation config for downstream classification rule
  * Input:
  *      index         - index of the classification (0 - 511)
  *
  * Output:
  *      notDs        - ENABLED: not opreation
  *                      - DISABLED: normal operation
  * Return:
  *      RT_ERR_OK           - OK
  *      RT_ERR_FAILED       - Failed
  *      RT_ERR_OUT_OF_RANGE - input parameter out of range
  *      RT_ERR_NULL_POINTER - input parameter is null pointer
  * Note:

 *      None
 */
 int32 apollomp_raw_cf_dsOp_get(uint32 index, rtk_enable_t *notDs)
{
    int32 ret;
    uint32 tmpData;

    ret = RT_ERR_FAILED;
    RT_PARAM_CHK((notDs == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(index >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
    if ((ret = reg_array_field_read(APOLLOMP_CF_OP_DSr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_NOT_DSf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    *notDs = tmpData;
    return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_cf_usOp_set
 * Description:
 *      Set not operation config for upstream classification rule
 * Input:
 *      index         - index of the classification (0 - 511)
 *      notDs        - ENABLED: not opreation
 *                      - DISABLED: normal operation
*
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32 apollomp_raw_cf_usOp_set(uint32 index, rtk_enable_t notUs)
{
    int32 ret;
    uint32 tmpData;

    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(index >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(notUs >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);

    tmpData = notUs;

    if ((ret = reg_array_field_write(APOLLOMP_CF_OP_USr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_NOT_USf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_cf_usOp_get
 * Description:
 *      Get not operation config for upstream classification rule
  * Input:
  *      index         - index of the classification (0 - 511)
  *
  * Output:
  *      notDs        - ENABLED: not opreation
  *                      - DISABLED: normal operation
  * Return:
  *      RT_ERR_OK           - OK
  *      RT_ERR_FAILED       - Failed
  *      RT_ERR_OUT_OF_RANGE - input parameter out of range
  *      RT_ERR_NULL_POINTER - input parameter is null pointer
  * Note:

 *      None
 */
int32 apollomp_raw_cf_usOp_get(uint32 index, rtk_enable_t *notUs)
{
    int32 ret;
    uint32 tmpData;

    ret = RT_ERR_FAILED;

    RT_PARAM_CHK((notUs == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(index >= APOLLOMP_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
    if ((ret = reg_array_field_read(APOLLOMP_CF_OP_USr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_NOT_USf, &tmpData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    *notUs = tmpData;
    return RT_ERR_OK;

}

/* Function Name:
  *      apollomp_raw_cf_usPermit_set
  * Description:
  *     Set Permit packet which unmatch upstream classification rules
  * Input:
  *      rule           - the rule which unmath upstream classification
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK             - OK
  *      RT_ERR_FAILED       - Failed
  *      RT_ERR_OUT_OF_RANGE - input parameter out of range
  * Note:
  *      None
  */

int32 apollomp_raw_cf_usPermit_set(apollomp_raw_cf_usPermit_t rule)
{
    int32 ret;
    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(rule >= CF_US_PERMIT_END, RT_ERR_OUT_OF_RANGE);
    if ((ret = reg_field_write(APOLLOMP_CF_CFGr, APOLLOMP_CF_US_PERMITf, &rule)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}
/* Function Name:
  *      apollomp_raw_cf_usPermit_get
  * Description:
  *     Get Permit packet which unmatch upstream classification rules
  * Input:
  *      index   - None
  * Output:
  *      pRule     - the rule which unmath upstream classification
  * Return:
  *      RT_ERR_OK           - OK
  *      RT_ERR_FAILED       - Failed
  *      RT_ERR_NULL_POINTER - input parameter is null pointer
  * Note:
  *      None
  */

int32 apollomp_raw_cf_usPermit_get(apollomp_raw_cf_usPermit_t *pRule)
{
    int32 ret;
    ret = RT_ERR_FAILED;
    RT_PARAM_CHK(pRule == NULL,RT_ERR_NULL_POINTER);
    if ((ret = reg_field_read(APOLLOMP_CF_CFGr, APOLLOMP_CF_US_PERMITf, pRule)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}



