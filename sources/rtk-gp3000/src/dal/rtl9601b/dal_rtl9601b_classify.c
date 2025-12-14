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
#include <rtk/port.h>
#include <rtk/classify.h>
#include <hal/mac/reg.h>
#include <hal/mac/drv.h>
#include <dal/rtl9601b/dal_rtl9601b_classify.h>

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
/* Function Name:
 *		rtl9601b_raw_cf_vlaid_set
 * Description:
 *		Set classification action valid control
 * Input:
 *		index		   - index of the classification (0 - 255)
 *		enabled 	- Enable or disable  the rule of index
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK			  - OK
 *		RT_ERR_FAILED		- Failed
 *		RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *		None
 */

int32 rtl9601b_raw_cf_valid_set(uint32 index, rtk_enable_t enabled)
{
	int32 ret;
	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(index >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(enabled >= RTK_ENABLE_END, RT_ERR_OUT_OF_RANGE);
	if ((ret = reg_array_field_write(RTL9601B_CF_VALIDr, REG_ARRAY_INDEX_NONE, index, RTL9601B_VALIDf, &enabled)) != RT_ERR_OK )
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_cf_valid_get
 * Description:
 *		Get classification downstrean action control
 * Input:
 *		index	- index of the classification (0 - 255)
 * Output:
  * 	 pEnabled	- The valid status the rule with  index
 * Return:
 *		RT_ERR_OK			- OK
 *		RT_ERR_FAILED		- Failed
 *		RT_ERR_OUT_OF_RANGE - input parameter out of range
 *		RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *		None
 */

int32 rtl9601b_raw_cf_valid_get(uint32 index, rtk_enable_t *pEnabled)

{
	int32 ret;
	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(pEnabled == NULL,RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(index >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);

	if ((ret = reg_array_field_read(RTL9601B_CF_VALIDr, REG_ARRAY_INDEX_NONE, index, RTL9601B_VALIDf, pEnabled)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;

}

/* Function Name:
 *		rtl9601b_raw_cf_rule_set
 * Description:
 *		Set classification pattern content
 * Input:
 *		index	-index of the classification (0 - 255)
 *		pattern  - the pattern of the classification
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK			- OK
 *		RT_ERR_FAILED		- Failed
 *		RT_ERR_OUT_OF_RANGE - input parameter out of range
 *		RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *		None
 */
int32 rtl9601b_raw_cf_rule_set(rtl9601b_raw_cf_RuleEntry_t *pattern)
{

	int32 ret;
	uint32 entryIdx; /*use which entry*/
	uint32 patternIdx; /*use which entry*/
	uint32 fieldData[RTL9601B_RAW_CF_ENTRYLEN];
	uint32 fieldMask[RTL9601B_RAW_CF_ENTRYLEN];
	uint32 tmpData;


	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(NULL == pattern, RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(pattern->idx >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->valid >= CF_ENTRY_VALID_END, RT_ERR_OUT_OF_RANGE);

	RT_PARAM_CHK(pattern->care_bits.etherType > 0xffff, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->care_bits.ifCtag > 1, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->care_bits.ifStag > 1, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->care_bits.interPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->care_bits.outerPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->care_bits.outerVid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->care_bits.direction >= CF_DIRECTION_END, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->care_bits.uni > 0x7, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->care_bits.aclHit > (0x40|(RTL9601B_MAX_NUM_OF_ACL_ACTION-1)), RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->care_bits.outerDei > 1, RT_ERR_OUT_OF_RANGE);

	RT_PARAM_CHK(pattern->data_bits.etherType > 0xffff, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->data_bits.ifCtag > 1, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->data_bits.ifStag > 1, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->data_bits.interPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->data_bits.outerPri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->data_bits.outerVid > RTK_VLAN_ID_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->data_bits.direction >= CF_DIRECTION_END, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->data_bits.uni > 0x7, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->data_bits.aclHit > (0x40|(RTL9601B_MAX_NUM_OF_ACL_ACTION-1)), RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->data_bits.outerDei > 1, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->care_bits.innerVlan > 0xffff, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pattern->data_bits.innerVlan > 0xffff, RT_ERR_OUT_OF_RANGE);


	if(1 == pattern->data_bits.direction)
	{
		RT_PARAM_CHK(pattern->care_bits.tos_gemidx > 0x3f, RT_ERR_OUT_OF_RANGE);
		RT_PARAM_CHK(pattern->data_bits.tos_gemidx >= RTL9601B_CLASSIFY_SID_MAX, RT_ERR_OUT_OF_RANGE);
	}
	else
	{
		RT_PARAM_CHK(pattern->care_bits.tos_gemidx > 0xff, RT_ERR_OUT_OF_RANGE);
		RT_PARAM_CHK(pattern->data_bits.tos_gemidx > 0xff, RT_ERR_OUT_OF_RANGE);
	}

	entryIdx = pattern->idx&0x3f;
	patternIdx = pattern->idx>>6;

	osal_memset(fieldData, 0, sizeof(fieldData));
	osal_memset(fieldMask, 0, sizeof(fieldData));

	if ((ret = table_read(RTL9601B_CF_RULEt, entryIdx, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
		return ret;
	}
	if ((ret = table_read(RTL9601B_CF_MASKt, entryIdx, fieldMask)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "table read fail");
		return ret;
	}

	switch (patternIdx)
	{
		case RTL9601B_RAW_CF_PATTERN_0:
			/*mask*/
			if(pattern->idx < 32)
			{
				tmpData = pattern->care_bits.aclHit&(~pattern->data_bits.aclHit);
                if ((ret = table_field_set(RTL9601B_CF_MASK_L34t, RTL9601B_CF_MASK_L34_ACL_HITtf, &tmpData, fieldMask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
                    return ret;
                }

				tmpData = pattern->care_bits.outerDei&(~pattern->data_bits.outerDei);
				if ((ret = table_field_set(RTL9601B_CF_MASK_L34t, RTL9601B_CF_MASK_L34_DEItf, &tmpData, fieldMask)) != RT_ERR_OK)
				{
					RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
					return ret;
				}

				tmpData = pattern->care_bits.innerVlan & (~pattern->data_bits.innerVlan);
				if ((ret = table_field_set(RTL9601B_CF_MASK_L34t, RTL9601B_CF_MASK_L34_INNER_TAGtf, &tmpData, fieldMask)) != RT_ERR_OK)
				{
					RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
					return ret;
				}
			}
			else
			{
				tmpData = pattern->care_bits.etherType&(~pattern->data_bits.etherType);
				if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_ETH_TYPE_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
				{
					RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
					return ret;
				}
				tmpData = pattern->care_bits.tos_gemidx&(~pattern->data_bits.tos_gemidx);
				if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_TOS_GEMIDX_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
				{
					RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
					return ret;
				}
			}
			tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_U_D_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.outerVid&(~pattern->data_bits.outerVid);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_VID_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.outerPri&(~pattern->data_bits.outerPri);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_PRI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.interPri&(~pattern->data_bits.interPri);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_INTER_PRI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_STAG_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_CTAG_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_UNI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			/*data*/
			if(pattern->idx < 32)
			{

				tmpData = pattern->data_bits.aclHit&pattern->care_bits.aclHit;
				if ((ret = table_field_set(RTL9601B_CF_RULE_ENHANCEDt, RTL9601B_CF_RULE_ENHANCED_ACL_HITtf, &tmpData, fieldData)) != RT_ERR_OK)
				{
					RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
					return ret;
				}

				tmpData = pattern->data_bits.outerDei&pattern->care_bits.outerDei;
				if ((ret = table_field_set(RTL9601B_CF_RULE_ENHANCEDt, RTL9601B_CF_RULE_ENHANCED_DEItf, &tmpData, fieldData)) != RT_ERR_OK)
				{
					RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
					return ret;
				}
				tmpData = pattern->data_bits.innerVlan & pattern->care_bits.innerVlan;
				if ((ret = table_field_set(RTL9601B_CF_RULE_ENHANCEDt, RTL9601B_CF_RULE_ENHANCED_INNER_TAGtf, &tmpData, fieldData)) != RT_ERR_OK)
				{
					RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
					return ret;
				}
			}
			else
			{
				tmpData = pattern->data_bits.etherType&pattern->care_bits.etherType;
				if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_ETH_TYPE_0tf, &tmpData, fieldData)) != RT_ERR_OK)
				{
					RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
					return ret;
				}
				tmpData = pattern->data_bits.tos_gemidx&pattern->care_bits.tos_gemidx;
				if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_TOS_GEMIDX_0tf, &tmpData, fieldData)) != RT_ERR_OK)
				{
					RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
					return ret;
				}
			}
			tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_U_D_0tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.outerVid&pattern->care_bits.outerVid;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_VID_0tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.outerPri&pattern->care_bits.outerPri;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_PRI_0tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.interPri & pattern->care_bits.interPri;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_INTER_PRI_0tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_STAG_0tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.ifCtag & pattern->care_bits.ifCtag;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_CTAG_0tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.uni & pattern->care_bits.uni;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_UNI_0tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			break;

		case RTL9601B_RAW_CF_PATTERN_1:
			/* mask */
			tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_U_D_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.tos_gemidx&(~pattern->data_bits.tos_gemidx);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_TOS_GEMIDX_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.outerVid&(~pattern->data_bits.outerVid);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_VID_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.outerPri&(~pattern->data_bits.outerPri);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_PRI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.interPri&(~pattern->data_bits.interPri);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_INTER_PRI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_STAG_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_CTAG_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_UNI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}

			/*data*/
			tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_U_D_1tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.tos_gemidx&pattern->care_bits.tos_gemidx;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_TOS_GEMIDX_1tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.outerVid&pattern->care_bits.outerVid;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_VID_1tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.outerPri&pattern->care_bits.outerPri;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_PRI_1tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.interPri&pattern->care_bits.interPri;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_INTER_PRI_1tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_STAG_1tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.ifCtag&pattern->care_bits.ifCtag;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_CTAG_1tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.uni&pattern->care_bits.uni;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_UNI_1tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			break;
		case RTL9601B_RAW_CF_PATTERN_2:
			/* mask */
			tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_U_D_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.tos_gemidx&(~pattern->data_bits.tos_gemidx);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_TOS_GEMIDX_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.outerVid&(~pattern->data_bits.outerVid);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_VID_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.outerPri&(~pattern->data_bits.outerPri);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_PRI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.interPri&(~pattern->data_bits.interPri);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_INTER_PRI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_STAG_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_CTAG_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_UNI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}

			/*data*/
			tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_U_D_2tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.tos_gemidx&pattern->care_bits.tos_gemidx;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_TOS_GEMIDX_2tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.outerVid&pattern->care_bits.outerVid;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_VID_2tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.outerPri&pattern->care_bits.outerPri;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_PRI_2tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.interPri&pattern->care_bits.interPri;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_INTER_PRI_2tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_STAG_2tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.ifCtag&pattern->care_bits.ifCtag;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_CTAG_2tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.uni&pattern->care_bits.uni;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_UNI_2tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			break;
		case RTL9601B_RAW_CF_PATTERN_3:
			/* mask */
			tmpData = pattern->care_bits.direction&(~pattern->data_bits.direction);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_U_D_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.tos_gemidx&(~pattern->data_bits.tos_gemidx);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_TOS_GEMIDX_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.outerVid&(~pattern->data_bits.outerVid);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_VID_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.outerPri&(~pattern->data_bits.outerPri);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_PRI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.interPri&(~pattern->data_bits.interPri);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_INTER_PRI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.ifStag&(~pattern->data_bits.ifStag);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_STAG_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.ifCtag&(~pattern->data_bits.ifCtag);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_CTAG_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->care_bits.uni&(~pattern->data_bits.uni);
			if ((ret = table_field_set(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_UNI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}

			/*data*/
			tmpData = pattern->data_bits.direction&pattern->care_bits.direction;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_U_D_3tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.tos_gemidx&pattern->care_bits.tos_gemidx;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_TOS_GEMIDX_3tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.outerVid&pattern->care_bits.outerVid;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_VID_3tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.outerPri&pattern->care_bits.outerPri;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_PRI_3tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.interPri&pattern->care_bits.interPri;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_INTER_PRI_3tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.ifStag&pattern->care_bits.ifStag;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_STAG_3tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.ifCtag&pattern->care_bits.ifCtag;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_CTAG_3tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			tmpData = pattern->data_bits.uni&pattern->care_bits.uni;
			if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_UNI_3tf, &tmpData, fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			break;

		default:
			return RT_ERR_OUT_OF_RANGE;
	}
	/* vailid bit */
	if ((ret = table_field_set(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_VALIDtf, &pattern->valid, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_write(RTL9601B_CF_RULEt, entryIdx, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_write(RTL9601B_CF_MASKt, entryIdx, fieldMask)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}



/* Function Name:
 *		rtl9601b_raw_cf_rule_get
 * Description:
  * 	 Set classification pattern content
 * Input:
 *		index	-index of the classification
 * Output:
 *		pattern  - the pattern of the classification
 * Return:
 *		RT_ERR_OK			  - OK
 *		RT_ERR_FAILED		- Failed
 *		RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *		None
 */
int32 rtl9601b_raw_cf_rule_get(rtl9601b_raw_cf_RuleEntry_t *pattern)

{
	int32 ret;
	uint32 entryIdx; /*use which entry*/
	uint32 patternIdx; /*use which entry*/
	uint32 fieldData[RTL9601B_RAW_CF_ENTRYLEN];
	uint32 fieldMask[RTL9601B_RAW_CF_ENTRYLEN];
	uint32 tmpData;


	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(NULL == pattern, RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(pattern->idx >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
	entryIdx = pattern->idx&0x3f;
	patternIdx = pattern->idx>>6;
	osal_memset(fieldData, 0, sizeof(fieldData));
	osal_memset(fieldData, 0, sizeof(fieldMask));

	if ((ret = table_read(RTL9601B_CF_RULEt, entryIdx, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_read(RTL9601B_CF_MASKt, entryIdx, fieldMask)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	switch (patternIdx)
	{

		case RTL9601B_RAW_CF_PATTERN_0:
			/*data*/
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_ETH_TYPE_0tf, &(pattern->data_bits.etherType), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_U_D_0tf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_TOS_GEMIDX_0tf, &(pattern->data_bits.tos_gemidx), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_VID_0tf, &(pattern->data_bits.outerVid), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_PRI_0tf, &(pattern->data_bits.outerPri), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_INTER_PRI_0tf, &(pattern->data_bits.interPri), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_STAG_0tf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_CTAG_0tf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_UNI_0tf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			/* mask */
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_ETH_TYPE_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.etherType = tmpData^pattern->data_bits.etherType;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_U_D_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.direction = tmpData^pattern->data_bits.direction;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_TOS_GEMIDX_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.tos_gemidx = tmpData^pattern->data_bits.tos_gemidx;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_VID_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.outerVid = tmpData^pattern->data_bits.outerVid;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_PRI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.outerPri = tmpData^pattern->data_bits.outerPri;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_INTER_PRI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.interPri = tmpData^pattern->data_bits.interPri;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_STAG_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_CTAG_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_UNI_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.uni = tmpData^pattern->data_bits.uni;
			break;

		case RTL9601B_RAW_CF_PATTERN_1:
			/*data*/
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_U_D_1tf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_TOS_GEMIDX_1tf, &(pattern->data_bits.tos_gemidx), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_VID_1tf, &(pattern->data_bits.outerVid), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_PRI_1tf, &(pattern->data_bits.outerPri), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_INTER_PRI_1tf, &(pattern->data_bits.interPri), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_STAG_1tf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_CTAG_1tf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_UNI_1tf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}

			/* mask */
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_U_D_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.direction = tmpData^pattern->data_bits.direction;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_TOS_GEMIDX_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.tos_gemidx = tmpData^pattern->data_bits.tos_gemidx;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_VID_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.outerVid = tmpData^pattern->data_bits.outerVid;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_PRI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.outerPri = tmpData^pattern->data_bits.outerPri;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_INTER_PRI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.interPri = tmpData^pattern->data_bits.interPri;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_STAG_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_CTAG_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_UNI_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.uni = tmpData^pattern->data_bits.uni;
			break;
		case RTL9601B_RAW_CF_PATTERN_2:
			/*data*/
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_U_D_2tf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_TOS_GEMIDX_2tf, &(pattern->data_bits.tos_gemidx), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_VID_2tf, &(pattern->data_bits.outerVid), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_PRI_2tf, &(pattern->data_bits.outerPri), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_INTER_PRI_2tf, &(pattern->data_bits.interPri), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_STAG_2tf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_CTAG_2tf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_UNI_2tf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}

			/* mask */
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_U_D_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.direction = tmpData^pattern->data_bits.direction;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_TOS_GEMIDX_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.tos_gemidx = tmpData^pattern->data_bits.tos_gemidx;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_VID_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.outerVid = tmpData^pattern->data_bits.outerVid;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_PRI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.outerPri = tmpData^pattern->data_bits.outerPri;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_INTER_PRI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.interPri = tmpData^pattern->data_bits.interPri;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_STAG_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_CTAG_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_UNI_2tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.uni = tmpData^pattern->data_bits.uni;
			break;

		case RTL9601B_RAW_CF_PATTERN_3:
			/* data */
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_U_D_3tf, &(pattern->data_bits.direction), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_TOS_GEMIDX_3tf, &(pattern->data_bits.tos_gemidx), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_VID_3tf, &(pattern->data_bits.outerVid), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_PRI_3tf, &(pattern->data_bits.outerPri), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_INTER_PRI_3tf, &(pattern->data_bits.interPri), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_STAG_3tf, &(pattern->data_bits.ifStag), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_IF_CTAG_3tf, &(pattern->data_bits.ifCtag), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_UNI_3tf, &(pattern->data_bits.uni), fieldData)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			/* mask */
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_U_D_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.direction = tmpData^pattern->data_bits.direction;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_TOS_GEMIDX_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.tos_gemidx = tmpData^pattern->data_bits.tos_gemidx;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_VID_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.outerVid = tmpData^pattern->data_bits.outerVid;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_PRI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.outerPri = tmpData^pattern->data_bits.outerPri;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_INTER_PRI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.interPri = tmpData^pattern->data_bits.interPri;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_STAG_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.ifStag = tmpData^pattern->data_bits.ifStag;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_IF_CTAG_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
				return ret;
			}
			pattern->care_bits.ifCtag = tmpData^pattern->data_bits.ifCtag;
			if ((ret = table_field_get(RTL9601B_CF_MASKt, RTL9601B_CF_MASK_UNI_3tf, &tmpData, fieldMask)) != RT_ERR_OK)
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
	if ((ret = table_field_get(RTL9601B_CF_RULEt, RTL9601B_CF_RULE_VALIDtf, &pattern->valid, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;

}


/* Function Name:
 *		rtl9601b_raw_cf_dsAct_set
 * Description:
 *		Set classification downstrean action control
 * Input:
 *		index	- index of the classification (0 - 255)
 *		pCfDsAct	- CF down stream action stucture for setting
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK			- OK
 *		RT_ERR_FAILED		- Failed
 *		RT_ERR_OUT_OF_RANGE - input parameter out of range
 *		RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *		None
 */

int32 rtl9601b_raw_cf_dsAct_set(rtl9601b_raw_cf_dsAct_t *pCfDsAct)
{
	int32 ret;
	uint32 fieldData[RTL9601B_RAW_CF_DSACTTBLEN];

	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(NULL == pCfDsAct, RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(pCfDsAct->idx >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);

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
	/*Not support in rtl9601 List*/
    RT_PARAM_CHK(pCfDsAct->csact == CF_DS_CSACT_SP2C, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pCfDsAct->cact == CF_DS_CACT_TRANSLATION_SP2C, RT_ERR_OUT_OF_RANGE);


	osal_memset(fieldData, 0, sizeof(fieldData));

	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CSACTtf, &pCfDsAct->csact, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CSPRI_ACTtf, &pCfDsAct->spri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CSVID_ACTtf, &pCfDsAct->svid_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CS_PRItf, &pCfDsAct->s_pri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CS_VIDtf, &pCfDsAct->s_vid, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CACTtf, &pCfDsAct->cact, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CVID_ACTtf, &pCfDsAct->cvid_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CPRI_ACTtf, &pCfDsAct->cpri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_C_VIDtf, &pCfDsAct->c_vid, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_C_PRItf, &pCfDsAct->c_pri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CFPRI_ACTtf, &pCfDsAct->cfpri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CF_PRItf, &pCfDsAct->cfpri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_UNI_ACTtf, &pCfDsAct->uni_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_UNI_PMSKtf, &pCfDsAct->uni_mask, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_set(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_DSCP_REMARKtf, &pCfDsAct->dscp_remark, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_write(RTL9601B_CF_ACTION_DSt, pCfDsAct->idx, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_cf_dsAct_get
 * Description:
 *		Get classification downstrean action control
 * Input:
 *		index	- index of the classification (0 - 255)
 * Output:
 *		pCfDsAct	- CF down stream action stucture for setting
 * Return:
 *		RT_ERR_OK			- OK
 *		RT_ERR_FAILED		- Failed
 *		RT_ERR_OUT_OF_RANGE - input parameter out of range
 *		RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *		None
 */
int32 rtl9601b_raw_cf_dsAct_get(rtl9601b_raw_cf_dsAct_t *pCfDsAct)
{
	int32 ret;
	uint32 fieldData[RTL9601B_RAW_CF_DSACTTBLEN];

	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(NULL == pCfDsAct, RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(pCfDsAct->idx >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);



	osal_memset(fieldData, 0, sizeof(fieldData));
	if ((ret = table_read(RTL9601B_CF_ACTION_DSt, pCfDsAct->idx, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CSACTtf, &pCfDsAct->csact, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CSVID_ACTtf, &pCfDsAct->svid_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CSPRI_ACTtf, &pCfDsAct->spri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CS_VIDtf, &pCfDsAct->s_vid, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CS_PRItf, &pCfDsAct->s_pri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CACTtf, &pCfDsAct->cact, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CVID_ACTtf, &pCfDsAct->cvid_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CPRI_ACTtf, &pCfDsAct->cpri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_C_VIDtf, &pCfDsAct->c_vid, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_C_PRItf, &pCfDsAct->c_pri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CFPRI_ACTtf, &pCfDsAct->cfpri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_CF_PRItf, &pCfDsAct->cfpri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_UNI_ACTtf, &pCfDsAct->uni_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_UNI_PMSKtf, &pCfDsAct->uni_mask, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_get(RTL9601B_CF_ACTION_DSt, RTL9601B_CF_ACTION_DS_DSCP_REMARKtf, &pCfDsAct->dscp_remark, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;

}

/* Function Name:
 *		rtl9601b_raw_cf_usAct_set
 * Description:
 *		Set classification downstrean action control
 * Input:
 *		index	- index of the classification (0 - 255)
 *		pCfUsAct	- CF up stream action stucture for setting
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK			- OK
 *		RT_ERR_FAILED		- Failed
 *		RT_ERR_OUT_OF_RANGE - input parameter out of range
 *		RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *		None
 */

int32 rtl9601b_raw_cf_usAct_set(rtl9601b_raw_cf_usAct_t *pCfUsAct)
{
	int32 ret;
	uint32 fieldData[RTL9601B_RAW_CF_UPACTTBLEN];
	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(pCfUsAct == NULL, RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(pCfUsAct->idx >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
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
	RT_PARAM_CHK(pCfUsAct->assign_idx >= (RTL9601B_CLASSIFY_SID_MAX-1), RT_ERR_OUT_OF_RANGE);

	RT_PARAM_CHK(pCfUsAct->cfpri_act >= CF_CF_PRI_ACT_END, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pCfUsAct->cfpri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_OUT_OF_RANGE);

	RT_PARAM_CHK(pCfUsAct->dscp_remark >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pCfUsAct->drop_act >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);

	/*Not support in this chip*/
	RT_PARAM_CHK(pCfUsAct->cact == CF_US_CACT_TRANSLATION_C2S, RT_ERR_OUT_OF_RANGE);

	osal_memset(fieldData, 0, sizeof(fieldData));

	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CSACTtf, &pCfUsAct->csact, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CSVID_ACTtf, &pCfUsAct->svid_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CSPRI_ACTtf, &pCfUsAct->spri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CS_VIDtf, &pCfUsAct->s_vid, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CS_PRItf, &pCfUsAct->s_pri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CACTtf, &pCfUsAct->cact, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CVID_ACTtf, &pCfUsAct->cvid_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CPRI_ACTtf, &pCfUsAct->cpri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_C_VIDtf, &pCfUsAct->c_vid, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_C_PRItf, &pCfUsAct->c_pri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_SID_ACTtf, &pCfUsAct->sid_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_ASSIGN_IDXtf, &pCfUsAct->assign_idx, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CFPRI_ACTtf, &pCfUsAct->cfpri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CF_PRItf, &pCfUsAct->cfpri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_DSCP_REMARKtf, &pCfUsAct->dscp_remark, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_set(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_DROPtf, &pCfUsAct->drop_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_write(RTL9601B_CF_ACTION_USt, pCfUsAct->idx, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}
/* Function Name:
 *		rtl9601b_raw_cf_usAct_get
 * Description:
 *		Get classification downstrean action control
 * Input:
 *		index	- index of the classification (0 - 255)
 * Output:
 *		pCfUsAct	- CF down stream action stucture for setting
 * Return:
 *		RT_ERR_OK			- OK
 *		RT_ERR_FAILED		- Failed
 *		RT_ERR_OUT_OF_RANGE - input parameter out of range
 *		RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *		None
 */

int32 rtl9601b_raw_cf_usAct_get(rtl9601b_raw_cf_usAct_t *pCfUsAct)
{
	int32 ret;
	uint32 fieldData[RTL9601B_RAW_CF_UPACTTBLEN];

	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(pCfUsAct->idx >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(pCfUsAct == NULL, RT_ERR_NULL_POINTER);

	osal_memset(fieldData, 0, sizeof(fieldData));
	if ((ret = table_read(RTL9601B_CF_ACTION_USt, pCfUsAct->idx, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CSACTtf, &pCfUsAct->csact, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CSVID_ACTtf, &pCfUsAct->svid_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CSPRI_ACTtf, &pCfUsAct->spri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CS_VIDtf, &pCfUsAct->s_vid, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CS_PRItf, &pCfUsAct->s_pri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CACTtf, &pCfUsAct->cact, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CVID_ACTtf, &pCfUsAct->cvid_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CPRI_ACTtf, &pCfUsAct->cpri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_C_VIDtf, &pCfUsAct->c_vid, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_C_PRItf, &pCfUsAct->c_pri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_SID_ACTtf, &pCfUsAct->sid_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_ASSIGN_IDXtf, &pCfUsAct->assign_idx, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CFPRI_ACTtf, &pCfUsAct->cfpri_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_CF_PRItf, &pCfUsAct->cfpri, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_DSCP_REMARKtf, &pCfUsAct->dscp_remark, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	if ((ret = table_field_get(RTL9601B_CF_ACTION_USt, RTL9601B_CF_ACTION_US_DROPtf, &pCfUsAct->drop_act, fieldData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;


}

/* Function Name:
 *		rtl9601b_raw_cf_dsOp_set
 * Description:
 *		Set not operation config for downstream classification rule
 * Input:
 *		index		  - index of the classification (0 - 255)
 *		notDs		 - ENABLED: not opreation
 *						- DISABLED: normal operation
*
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK			- OK
 *		RT_ERR_FAILED		- Failed
 *		RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *		None
 */
int32 rtl9601b_raw_cf_dsOp_set(uint32 index, rtk_enable_t notDs)
{
	int32 ret;
	uint32 tmpData;

	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(index >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(notDs >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);

	tmpData = notDs;

	if ((ret = reg_array_field_write(RTL9601B_CF_OP_DSr, REG_ARRAY_INDEX_NONE, index, RTL9601B_NOT_DSf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}

/* Function Name:
 *		rtl9601b_raw_cf_dsOp_get
 * Description:
 *		Get not operation config for downstream classification rule
  * Input:
  * 	 index		   - index of the classification (0 - 255)
  *
  * Output:
  * 	 notDs		  - ENABLED: not opreation
  * 					 - DISABLED: normal operation
  * Return:
  * 	 RT_ERR_OK			 - OK
  * 	 RT_ERR_FAILED		 - Failed
  * 	 RT_ERR_OUT_OF_RANGE - input parameter out of range
  * 	 RT_ERR_NULL_POINTER - input parameter is null pointer
  * Note:

 *		None
 */
 int32 rtl9601b_raw_cf_dsOp_get(uint32 index, rtk_enable_t *notDs)
{
	int32 ret;
	uint32 tmpData;

	ret = RT_ERR_FAILED;
	RT_PARAM_CHK((notDs == NULL), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(index >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
	if ((ret = reg_array_field_read(RTL9601B_CF_OP_DSr, REG_ARRAY_INDEX_NONE, index, RTL9601B_NOT_DSf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	*notDs = tmpData;
	return RT_ERR_OK;

}

/* Function Name:
 *		rtl9601b_raw_cf_usOp_set
 * Description:
 *		Set not operation config for upstream classification rule
 * Input:
 *		index		  - index of the classification (0 - 255)
 *		notDs		 - ENABLED: not opreation
 *						- DISABLED: normal operation
*
 * Output:
 *		None
 * Return:
 *		RT_ERR_OK			- OK
 *		RT_ERR_FAILED		- Failed
 *		RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *		None
 */
int32 rtl9601b_raw_cf_usOp_set(uint32 index, rtk_enable_t notUs)
{
	int32 ret;
	uint32 tmpData;

	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(index >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
	RT_PARAM_CHK(notUs >= CF_ENABLE_END, RT_ERR_OUT_OF_RANGE);

	tmpData = notUs;

	if ((ret = reg_array_field_write(RTL9601B_CF_OP_USr, REG_ARRAY_INDEX_NONE, index, RTL9601B_NOT_USf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}

/* Function Name:
 *		rtl9601b_raw_cf_usOp_get
 * Description:
 *		Get not operation config for upstream classification rule
  * Input:
  * 	 index		   - index of the classification (0 - 255)
  *
  * Output:
  * 	 notDs		  - ENABLED: not opreation
  * 					 - DISABLED: normal operation
  * Return:
  * 	 RT_ERR_OK			 - OK
  * 	 RT_ERR_FAILED		 - Failed
  * 	 RT_ERR_OUT_OF_RANGE - input parameter out of range
  * 	 RT_ERR_NULL_POINTER - input parameter is null pointer
  * Note:

 *		None
 */
int32 rtl9601b_raw_cf_usOp_get(uint32 index, rtk_enable_t *notUs)
{
	int32 ret;
	uint32 tmpData;

	ret = RT_ERR_FAILED;

	RT_PARAM_CHK((notUs == NULL), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(index >= RTL9601B_CLASSIFY_ENTRY_MAX, RT_ERR_OUT_OF_RANGE);
	if ((ret = reg_array_field_read(RTL9601B_CF_OP_USr, REG_ARRAY_INDEX_NONE, index, RTL9601B_NOT_USf, &tmpData)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	*notUs = tmpData;
	return RT_ERR_OK;

}

/* Function Name:
  * 	 rtl9601b_raw_cf_usPermit_set
  * Description:
  * 	Set Permit packet which unmatch upstream classification rules
  * Input:
  * 	 rule			- the rule which unmath upstream classification
  * Output:
  * 	 None
  * Return:
  * 	 RT_ERR_OK			   - OK
  * 	 RT_ERR_FAILED		 - Failed
  * 	 RT_ERR_OUT_OF_RANGE - input parameter out of range
  * Note:
  * 	 None
  */

int32 rtl9601b_raw_cf_usPermit_set(rtl9601b_raw_cf_usPermit_t rule)
{
	int32 ret;
	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(rule >= CF_US_PERMIT_END, RT_ERR_OUT_OF_RANGE);
	if ((ret = reg_field_write(RTL9601B_CF_CFGr, RTL9601B_CF_US_PERMITf, &rule)) != RT_ERR_OK )
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}
/* Function Name:
  * 	 rtl9601b_raw_cf_usPermit_get
  * Description:
  * 	Get Permit packet which unmatch upstream classification rules
  * Input:
  * 	 index	 - None
  * Output:
  * 	 pRule	   - the rule which unmath upstream classification
  * Return:
  * 	 RT_ERR_OK			 - OK
  * 	 RT_ERR_FAILED		 - Failed
  * 	 RT_ERR_NULL_POINTER - input parameter is null pointer
  * Note:
  * 	 None
  */

int32 rtl9601b_raw_cf_usPermit_get(rtl9601b_raw_cf_usPermit_t *pRule)
{
	int32 ret;
	ret = RT_ERR_FAILED;
	RT_PARAM_CHK(pRule == NULL,RT_ERR_NULL_POINTER);
	if ((ret = reg_field_read(RTL9601B_CF_CFGr, RTL9601B_CF_US_PERMITf, pRule)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}



/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl9601b_classify_init
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
dal_rtl9601b_classify_init(void)
{
    rtl9601b_raw_cf_RuleEntry_t entry;
    rtl9601b_raw_cf_dsAct_t ds_act;
    rtl9601b_raw_cf_usAct_t us_act;
    uint32  idx;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* clear table */
    osal_memset(&entry, 0x0, sizeof(rtl9601b_raw_cf_RuleEntry_t));
    osal_memset(&ds_act, 0x0, sizeof(rtl9601b_raw_cf_dsAct_t));
    osal_memset(&us_act, 0x0, sizeof(rtl9601b_raw_cf_usAct_t));

    for(idx = 0; idx < RTL9601B_CLASSIFY_ENTRY_MAX; idx++)
    {
        entry.idx  = idx;
        entry.valid = CF_ENTRY_VALID; /* always set to 1 for Tcam valid bit */
        ds_act.idx = idx;
        us_act.idx = idx;

        RT_ERR_CHK(rtl9601b_raw_cf_valid_set(entry.idx, CF_ENTRY_INVALID), ret);
        RT_ERR_CHK(rtl9601b_raw_cf_rule_set(&entry), ret);
        RT_ERR_CHK(rtl9601b_raw_cf_dsAct_set(&ds_act), ret);
        RT_ERR_CHK(rtl9601b_raw_cf_usAct_set(&us_act), ret);
        RT_ERR_CHK(rtl9601b_raw_cf_dsOp_set(entry.idx, CLASSIFY_INVERT_DISABLE), ret);
        RT_ERR_CHK(rtl9601b_raw_cf_usOp_set(entry.idx, CLASSIFY_INVERT_DISABLE), ret);
    }

    classify_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_classify_init */

static int
_dal_rtl9601b_classify_cfg2raw_us_csAct(rtk_classify_us_csact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_US_CSACT_NOP:
            *pRawAct = CF_US_CSACT_NOP;
            break;
        case CLASSIFY_US_CSACT_ADD_TAG_VS_TPID:
            *pRawAct = CF_US_CSACT_ADD_TAG_VS_TPID;
            break;
        case CLASSIFY_US_CSACT_ADD_TAG_8100:
            *pRawAct = CF_US_CSACT_ADD_TAG_8100;
            break;
        case CLASSIFY_US_CSACT_DEL_STAG:
            *pRawAct = CF_US_CSACT_DEL_STAG;
            break;
        case CLASSIFY_US_CSACT_TRANSPARENT:
            *pRawAct = CF_US_CSACT_TRANSPARENT;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9601b_classify_cfg2raw_us_cAct(rtk_classify_us_cact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_US_CACT_NOP:
            *pRawAct = CF_US_CACT_NOP;
            break;
        case CLASSIFY_US_CACT_DEL_CTAG:
            *pRawAct = CF_US_CACT_DEL_CTAG;
            break;
        case CLASSIFY_US_CACT_TRANSPARENT:
            *pRawAct = CF_US_CACT_TRANSPARENT;
            break;
        case CLASSIFY_US_CACT_ADD_CTAG_8100:
            *pRawAct = CF_US_CACT_ADD_CTAG_8100;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9601b_classify_cfg2raw_us_vidAct(rtk_classify_us_vid_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
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
_dal_rtl9601b_classify_cfg2raw_us_priAct(rtk_classify_us_pri_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_US_PRI_ACT_ASSIGN:
            *pRawAct = CF_US_PRI_ACT_ASSIGN;
            break;
        case CLASSIFY_US_PRI_ACT_FROM_1ST_TAG:
            *pRawAct = CF_US_PRI_ACT_FROM_1ST_TAG;
            break;
        case CLASSIFY_US_PRI_ACT_FROM_INTERNAL:
            *pRawAct = CF_US_PRI_ACT_FROM_INTERNAL;
            break;
        case CLASSIFY_US_PRI_ACT_FROM_2ND_TAG:
            *pRawAct = CF_US_PRI_ACT_FROM_2ND_TAG;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9601b_classify_cfg2raw_ds_csAct(rtk_classify_ds_csact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DS_CSACT_NOP:
            *pRawAct = CF_DS_CSACT_NOP;
            break;
        case CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID:
            *pRawAct = CF_DS_CSACT_ADD_TAG_VS_TPID;
            break;
        case CLASSIFY_DS_CSACT_ADD_TAG_8100:
            *pRawAct = CF_DS_CSACT_ADD_TAG_8100;
            break;
        case CLASSIFY_DS_CSACT_DEL_STAG:
            *pRawAct = CF_DS_CSACT_DEL_STAG;
            break;
        case CLASSIFY_DS_CSACT_TRANSPARENT:
            *pRawAct = CF_DS_CSACT_TRANSPARENT;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9601b_classify_cfg2raw_ds_cAct(rtk_classify_ds_cact_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DS_CACT_NOP:
            *pRawAct = CF_DS_CACT_NOP;
            break;
        case CLASSIFY_DS_CACT_ADD_CTAG_8100:
            *pRawAct = CF_DS_CACT_ADD_CTAG_8100;
            break;
        case CLASSIFY_DS_CACT_TRANSPARENT:
            *pRawAct = CF_DS_CACT_TRANSPARENT;
            break;
        case CLASSIFY_DS_CACT_DEL_CTAG:
            *pRawAct = CF_DS_CACT_DEL_CTAG;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9601b_classify_cfg2raw_ds_vidAct(rtk_classify_ds_vid_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DS_VID_ACT_ASSIGN:
            *pRawAct = CF_DS_VID_ACT_ASSIGN;
            break;
        case CLASSIFY_DS_VID_ACT_FROM_1ST_TAG:
            *pRawAct = CF_DS_VID_ACT_FROM_1ST_TAG;
            break;
        case CLASSIFY_DS_VID_ACT_FROM_2ND_TAG:
            *pRawAct = CF_DS_VID_ACT_FROM_2ND_TAG;
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
_dal_rtl9601b_classify_cfg2raw_ds_priAct(rtk_classify_ds_pri_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DS_PRI_ACT_ASSIGN:
            *pRawAct = CF_DS_PRI_ACT_ASSIGN;
            break;
        case CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG:
            *pRawAct = CF_DS_PRI_ACT_FROM_1ST_TAG;
            break;
        case CLASSIFY_DS_PRI_ACT_FROM_INTERNAL:
            *pRawAct = CF_DS_PRI_ACT_FROM_INTERNAL;
            break;
        case CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG:
            *pRawAct = CF_DS_PRI_ACT_FROM_2ND_TAG;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9601b_classify_cfg2raw_cfPriAct(rtk_classify_cf_pri_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
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
_dal_rtl9601b_classify_cfg2raw_dscpAct(rtk_classify_dscp_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
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
_dal_rtl9601b_classify_cfg2raw_uniAct(rtk_classify_ds_uni_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    /*assign uni act, since new register designed, 2012.05.28*/
	 switch(cfgAct){
	 case CLASSIFY_DS_UNI_ACT_NOP:
	 	 *pRawAct  = CF_DS_UNI_ACT_NOP;
	 break;
	 case CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK:
		 *pRawAct   = CF_DS_UNI_ACT_MASK_BY_UNIMASK;
 	 break;
	 case CLASSIFY_DS_UNI_ACT_FORCE_FORWARD:
	 	*pRawAct   = CF_DS_UNI_ACT_FORCE_FORWARD ;
	 break;
	 default:
	 	ret = RT_ERR_CHIP_NOT_SUPPORTED;
	 break;
	 }
	 return ret;
}


static int
_dal_rtl9601b_classify_cfg2raw_dropAct(rtk_classify_drop_act_t cfgAct, uint32 *pRawAct)
{
    int32 ret=RT_ERR_OK;

    switch(cfgAct)
    {
        case CLASSIFY_DROP_ACT_NONE:
            *pRawAct = CF_DISABLE;
            break;
        case CLASSIFY_DROP_ACT_ENABLE:
            *pRawAct = CF_ENABLE;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}


static int
_dal_rtl9601b_classify_cfg2raw_sidAct(rtk_classify_us_sqid_act_t sqidAct, uint32 *pRawAct)
{
       int32 ret=RT_ERR_OK;

	 /*assign sid act, since new register designed, 2012.05.28*/
	 switch(sqidAct){
	 case CLASSIFY_US_SQID_ACT_ASSIGN_SID:
	 	*pRawAct  = CF_US_SQID_ACT_ASSIGN_SID;
	 break;
	 case CLASSIFY_US_SQID_ACT_ASSIGN_NOP:
	 	*pRawAct   =CF_US_SQID_ACT_ASSIGN_NOP ;
	 break;
	 case CLASSIFY_US_SQID_ACT_ASSIGN_QID:
	 default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
	 break;
	 }

	 return ret;
}

static int
_dal_rtl9601b_classify_raw2cfg_us_csAct(uint32 rawAct, rtk_classify_us_csact_t *pCcfgAct)
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
        case CF_US_CSACT_ADD_TAG_8100:
            *pCcfgAct = CLASSIFY_US_CSACT_ADD_TAG_8100;
            break;
        case CF_US_CSACT_DEL_STAG:
            *pCcfgAct = CLASSIFY_US_CSACT_DEL_STAG;
            break;
        case CF_US_CSACT_TRANSPARENT:
            *pCcfgAct = CLASSIFY_US_CSACT_TRANSPARENT;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9601b_classify_raw2cfg_us_cAct(uint32 rawAct, rtk_classify_us_cact_t *pCfgAct)
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
_dal_rtl9601b_classify_raw2cfg_us_vidAct(uint32 rawAct, rtk_classify_us_vid_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
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
_dal_rtl9601b_classify_raw2cfg_us_priAct(uint32 rawAct, rtk_classify_us_pri_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
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
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9601b_classify_raw2cfg_ds_csAct(uint32 rawAct, rtk_classify_ds_csact_t *pCfgAct)
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
        case CF_DS_CSACT_ADD_TAG_8100:
            *pCfgAct = CLASSIFY_DS_CSACT_ADD_TAG_8100;
            break;
        case CF_DS_CSACT_DEL_STAG:
            *pCfgAct = CLASSIFY_DS_CSACT_DEL_STAG;
            break;
        case CF_DS_CSACT_TRANSPARENT:
            *pCfgAct = CLASSIFY_DS_CSACT_TRANSPARENT;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9601b_classify_raw2cfg_ds_cAct(uint32 rawAct, rtk_classify_ds_cact_t *pCfgAct)
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
_dal_rtl9601b_classify_raw2cfg_ds_vidAct(uint32 rawAct, rtk_classify_ds_vid_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_DS_VID_ACT_ASSIGN:
            *pCfgAct = CLASSIFY_DS_VID_ACT_ASSIGN;
            break;
        case CF_DS_VID_ACT_FROM_1ST_TAG:
            *pCfgAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
            break;
        case CF_DS_VID_ACT_FROM_2ND_TAG:
            *pCfgAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
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
_dal_rtl9601b_classify_raw2cfg_ds_priAct(uint32 rawAct, rtk_classify_ds_pri_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_DS_PRI_ACT_ASSIGN:
            *pCfgAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
            break;
        case CF_DS_PRI_ACT_FROM_1ST_TAG:
            *pCfgAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
            break;
        case CF_DS_PRI_ACT_FROM_2ND_TAG:
            *pCfgAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
            break;
        case CF_DS_PRI_ACT_FROM_INTERNAL:
            *pCfgAct = CLASSIFY_DS_PRI_ACT_FROM_INTERNAL;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}

static int
_dal_rtl9601b_classify_raw2cfg_cfPriAct(uint32 rawAct, rtk_classify_cf_pri_act_t *pCfgAct)
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
_dal_rtl9601b_classify_raw2cfg_dscpAct(uint32 rawAct, rtk_classify_dscp_act_t *pCfgAct)
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
_dal_rtl9601b_classify_raw2cfg_dropAct(uint32 rawAct, rtk_classify_drop_act_t *pCfgAct)
{
    int32 ret=RT_ERR_OK;

    switch(rawAct)
    {
        case CF_DISABLE:
            *pCfgAct = CLASSIFY_DROP_ACT_NONE;
            break;
        case CF_ENABLE:
            *pCfgAct = CLASSIFY_DROP_ACT_ENABLE;
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }
    return ret;
}



static int
_dal_rtl9601b_classify_raw2cfg_sidAct(uint32 rawAct, rtk_classify_us_sqid_act_t *psidAct)
{
	int32 ret=RT_ERR_OK;

	/*assign sid act, since new register designed, 2012.05.28*/
	switch(rawAct){
	case CF_US_SQID_ACT_ASSIGN_SID:
		*psidAct = CLASSIFY_US_SQID_ACT_ASSIGN_SID;
	break;
	case CF_US_SQID_ACT_ASSIGN_NOP :
		*psidAct    =CLASSIFY_US_SQID_ACT_ASSIGN_NOP ;
	break;
	default:
		ret = RT_ERR_CHIP_NOT_SUPPORTED;
	break;
	}
	return ret;
}

static int
_dal_rtl9601b_classify_raw2cfg_uniAct(uint32 rawAct, rtk_classify_ds_uni_act_t *puniAct)
{
	int32 ret=RT_ERR_OK;

	/*assign uni act, since new register designed, 2012.05.28*/
	switch(rawAct) {
	case CF_DS_UNI_ACT_NOP:
		*puniAct   = CLASSIFY_DS_UNI_ACT_NOP;
	break;
	case CF_DS_UNI_ACT_MASK_BY_UNIMASK:
		*puniAct  = CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK;
	break;
	case CF_DS_UNI_ACT_FORCE_FORWARD:
		*puniAct   = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD ;
	break;
	default:
		ret = RT_ERR_CHIP_NOT_SUPPORTED;
	break;
	}
      return ret;
}


static int
dal_rtl9601b_classify_cfg2entry(rtk_classify_cfg_t          *pClassifyCfg,
                                rtl9601b_raw_cf_RuleEntry_t *pRule,
                                rtl9601b_raw_cf_usAct_t     *pUsAction,
                                rtl9601b_raw_cf_dsAct_t     *pDsAction)
{
    rtk_classify_field_t *tmp_field;
    int32 ret=RT_ERR_OK;

    osal_memset(pRule, 0, sizeof(rtl9601b_raw_cf_RuleEntry_t));
    osal_memset(pUsAction, 0, sizeof(rtl9601b_raw_cf_usAct_t));
    osal_memset(pDsAction, 0, sizeof(rtl9601b_raw_cf_dsAct_t));

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
                pRule->data_bits.outerVid = tmp_field->classify_pattern.tagVid.value;
                pRule->care_bits.outerVid = tmp_field->classify_pattern.tagVid.mask;
                break;

            case CLASSIFY_FIELD_TAG_PRI:
                pRule->data_bits.outerPri = tmp_field->classify_pattern.tagPri.value;
                pRule->care_bits.outerPri = tmp_field->classify_pattern.tagPri.mask;
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

            case CLASSIFY_FIELD_DEI:
                pRule->data_bits.outerDei = tmp_field->classify_pattern.dei.value;
                pRule->care_bits.outerDei = tmp_field->classify_pattern.dei.mask;
                break;
			case CLASSIFY_FIELD_INNER_VLAN:
				pRule->data_bits.innerVlan= tmp_field->classify_pattern.innerVlan.value;
                pRule->care_bits.innerVlan = tmp_field->classify_pattern.innerVlan.mask;
				break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }

        tmp_field = tmp_field->next;
    }

    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {   /* us action */
        pUsAction->idx = pClassifyCfg->index;
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_us_csAct(pClassifyCfg->act.usAct.csAct, &pUsAction->csact), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_us_vidAct(pClassifyCfg->act.usAct.csVidAct, &pUsAction->svid_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_us_priAct(pClassifyCfg->act.usAct.csPriAct, &pUsAction->spri_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_us_cAct(pClassifyCfg->act.usAct.cAct, &pUsAction->cact), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_us_vidAct(pClassifyCfg->act.usAct.cVidAct, &pUsAction->cvid_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_us_priAct(pClassifyCfg->act.usAct.cPriAct, &pUsAction->cpri_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_cfPriAct(pClassifyCfg->act.usAct.interPriAct, &pUsAction->cfpri_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_dscpAct(pClassifyCfg->act.usAct.dscp, &pUsAction->dscp_remark), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_dropAct(pClassifyCfg->act.usAct.drop, &pUsAction->drop_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_sidAct(pClassifyCfg->act.usAct.sidQidAct, &pUsAction->sid_act), ret);

	  pUsAction->s_vid        = pClassifyCfg->act.usAct.sTagVid;
        pUsAction->s_pri        = pClassifyCfg->act.usAct.sTagPri;
        pUsAction->c_vid        = pClassifyCfg->act.usAct.cTagVid;
        pUsAction->c_pri        = pClassifyCfg->act.usAct.cTagPri;
        pUsAction->assign_idx   = pClassifyCfg->act.usAct.sidQid;
        pUsAction->cfpri        = pClassifyCfg->act.usAct.cfPri;



    }
    else /* ds action */
    {
        pDsAction->idx = pClassifyCfg->index;
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_ds_csAct(pClassifyCfg->act.dsAct.csAct, &pDsAction->csact), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_ds_vidAct(pClassifyCfg->act.dsAct.csVidAct, &pDsAction->svid_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_ds_priAct(pClassifyCfg->act.dsAct.csPriAct, &pDsAction->spri_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_ds_cAct(pClassifyCfg->act.dsAct.cAct, &pDsAction->cact), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_ds_vidAct(pClassifyCfg->act.dsAct.cVidAct, &pDsAction->cvid_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_ds_priAct(pClassifyCfg->act.dsAct.cPriAct, &pDsAction->cpri_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_cfPriAct(pClassifyCfg->act.dsAct.interPriAct, &pDsAction->cfpri_act), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_dscpAct(pClassifyCfg->act.dsAct.dscp, &pDsAction->dscp_remark), ret);
	  	RT_ERR_CHK(_dal_rtl9601b_classify_cfg2raw_uniAct(pClassifyCfg->act.dsAct.uniAct, &pDsAction->uni_act), ret);
	  	pDsAction->s_vid        = pClassifyCfg->act.dsAct.sTagVid;
        pDsAction->s_pri        = pClassifyCfg->act.dsAct.sTagPri;
        pDsAction->c_vid        = pClassifyCfg->act.dsAct.cTagVid;
        pDsAction->c_pri        = pClassifyCfg->act.dsAct.cTagPri;
        pDsAction->cfpri        = pClassifyCfg->act.dsAct.cfPri;
        pDsAction->uni_mask     = pClassifyCfg->act.dsAct.uniMask.bits[0];

    }
    return ret;
}

static int
dal_rtl9601b_classify_entry2cfg(rtl9601b_raw_cf_usAct_t     *pUsAction,
                                rtl9601b_raw_cf_dsAct_t     *pDsAction,
                                rtk_classify_cfg_t          *pClassifyCfg)
{
    int32 ret=RT_ERR_OK;

    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {   /* us action */
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_us_csAct(pUsAction->csact, &pClassifyCfg->act.usAct.csAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_us_vidAct(pUsAction->svid_act, &pClassifyCfg->act.usAct.csVidAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_us_priAct(pUsAction->spri_act, &pClassifyCfg->act.usAct.csPriAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_us_cAct(pUsAction->cact, &pClassifyCfg->act.usAct.cAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_us_vidAct(pUsAction->cvid_act, &pClassifyCfg->act.usAct.cVidAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_us_priAct(pUsAction->cpri_act, &pClassifyCfg->act.usAct.cPriAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_cfPriAct(pUsAction->cfpri_act, &pClassifyCfg->act.usAct.interPriAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_dscpAct(pUsAction->dscp_remark, &pClassifyCfg->act.usAct.dscp), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_dropAct(pUsAction->drop_act, &pClassifyCfg->act.usAct.drop), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_sidAct(pUsAction->sid_act, &pClassifyCfg->act.usAct.sidQidAct), ret);

	  	pClassifyCfg->act.usAct.sTagVid     = pUsAction->s_vid;
        pClassifyCfg->act.usAct.sTagPri     = pUsAction->s_pri;
        pClassifyCfg->act.usAct.cTagVid     = pUsAction->c_vid;
        pClassifyCfg->act.usAct.cTagPri     = pUsAction->c_pri;
        pClassifyCfg->act.usAct.sidQid      = pUsAction->assign_idx;
        pClassifyCfg->act.usAct.cfPri       = pUsAction->cfpri;

    }
    else /* ds action */
    {
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_ds_csAct(pDsAction->csact, &pClassifyCfg->act.dsAct.csAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_ds_vidAct(pDsAction->svid_act, &pClassifyCfg->act.dsAct.csVidAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_ds_priAct(pDsAction->spri_act, &pClassifyCfg->act.dsAct.csPriAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_ds_cAct(pDsAction->cact, &pClassifyCfg->act.dsAct.cAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_ds_vidAct(pDsAction->cvid_act, &pClassifyCfg->act.dsAct.cVidAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_ds_priAct(pDsAction->cpri_act, &pClassifyCfg->act.dsAct.cPriAct), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_dscpAct(pDsAction->dscp_remark, &pClassifyCfg->act.dsAct.dscp), ret);
        RT_ERR_CHK(_dal_rtl9601b_classify_raw2cfg_uniAct(pDsAction->uni_act, &pClassifyCfg->act.dsAct.uniAct), ret);
        pClassifyCfg->act.dsAct.sTagVid     = pDsAction->s_vid;
        pClassifyCfg->act.dsAct.sTagPri     = pDsAction->s_pri;
        pClassifyCfg->act.dsAct.cTagVid     = pDsAction->c_vid;
        pClassifyCfg->act.dsAct.cTagPri     = pDsAction->c_pri;
        pClassifyCfg->act.dsAct.interPriAct = pDsAction->cfpri_act;
        pClassifyCfg->act.dsAct.cfPri       = pDsAction->cfpri;
        pClassifyCfg->act.dsAct.uniMask.bits[0] = pDsAction->uni_mask;


    }
    return ret;
}

/* Function Name:
 *      dal_rtl9601b_classify_cfgEntry_add
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
dal_rtl9601b_classify_cfgEntry_add(rtk_classify_cfg_t *pClassifyCfg)
{
    rtl9601b_raw_cf_RuleEntry_t entry;
    rtl9601b_raw_cf_dsAct_t ds_act;
    rtl9601b_raw_cf_usAct_t us_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyCfg), RT_ERR_NULL_POINTER);

    /* translate data structure */
    RT_ERR_CHK(dal_rtl9601b_classify_cfg2entry(pClassifyCfg, &entry, &us_act, &ds_act), ret);

    /* set entry invalid */
    RT_ERR_CHK(rtl9601b_raw_cf_valid_set(entry.idx, CF_ENTRY_INVALID), ret);

    /* set entry rule */
    RT_ERR_CHK(rtl9601b_raw_cf_rule_set(&entry), ret);

    /* set entry action and set not operation */
    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {
        RT_ERR_CHK(rtl9601b_raw_cf_usAct_set(&us_act), ret);
        RT_ERR_CHK(rtl9601b_raw_cf_usOp_set(pClassifyCfg->index, pClassifyCfg->invert), ret);
    }
    else
    {
        RT_ERR_CHK(rtl9601b_raw_cf_dsAct_set(&ds_act), ret);
        RT_ERR_CHK(rtl9601b_raw_cf_dsOp_set(pClassifyCfg->index, pClassifyCfg->invert), ret);
    }

    /* set entry valid */
    RT_ERR_CHK(rtl9601b_raw_cf_valid_set(entry.idx, CF_ENTRY_VALID), ret);

    return RT_ERR_OK;
} /* end of dal_rtl9601b_classify_cfgEntry_add */


/* Function Name:
 *      dal_rtl9601b_classify_cfgEntry_get
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
dal_rtl9601b_classify_cfgEntry_get(rtk_classify_cfg_t *pClassifyCfg)
{
    uint32 entryIdx;
    uint32 patternIdx;
    uint32 fieldData[RTL9601B_RAW_CF_ENTRYLEN];
    uint32 fieldMask[RTL9601B_RAW_CF_ENTRYLEN];
    rtl9601b_raw_cf_dsAct_t ds_act;
    rtl9601b_raw_cf_usAct_t us_act;
    rtk_enable_t not_op;
    int32 ret;
    uint32 idx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_CLASSIFY_ENTRY_MAX() <= pClassifyCfg->index), RT_ERR_NULL_POINTER);

    /* get entry valid */
    RT_ERR_CHK(rtl9601b_raw_cf_valid_get(pClassifyCfg->index, &pClassifyCfg->valid), ret);

    /* get entry rule raw data */
    entryIdx = pClassifyCfg->index&0x3f;
    patternIdx = pClassifyCfg->index>>6;
    RT_ERR_CHK(table_read(RTL9601B_CF_RULEt, entryIdx, fieldData), ret);
    RT_ERR_CHK(table_read(RTL9601B_CF_MASKt, entryIdx, fieldMask), ret);
    for(idx=0; idx<RTL9601B_RAW_CF_ENTRYLEN; idx++)
        fieldMask[idx] = fieldMask[idx]^fieldData[idx];

    osal_memset(&pClassifyCfg->field.readField, 0, sizeof(rtk_classify_raw_field_t));
    pClassifyCfg->field.readField.dataFieldRaw[0] = (fieldData[patternIdx+1]&0xFFFF);
    pClassifyCfg->field.readField.dataFieldRaw[1] = ((fieldData[patternIdx+1]>>16)&0xFFFF);
    pClassifyCfg->field.readField.careFieldRaw[0] = (fieldMask[patternIdx+1]&0xFFFF);
    pClassifyCfg->field.readField.careFieldRaw[1] = ((fieldMask[patternIdx+1]>>16)&0xFFFF);
    if(0 == patternIdx)
    {
        pClassifyCfg->field.readField.dataFieldRaw[2] = (fieldData[patternIdx]&0xFFFF);
        pClassifyCfg->field.readField.careFieldRaw[2] = (fieldMask[patternIdx]&0xFFFF);
    }

    /* direction: bit 31 */
     if(((pClassifyCfg->field.readField.dataFieldRaw[1] & 0x8000) &
         (pClassifyCfg->field.readField.careFieldRaw[1] & 0x8000)) == 0)
         pClassifyCfg->direction = CLASSIFY_DIRECTION_US;
     else
         pClassifyCfg->direction = CLASSIFY_DIRECTION_DS;

    osal_memset(&ds_act, 0, sizeof(rtl9601b_raw_cf_dsAct_t));
    osal_memset(&us_act, 0, sizeof(rtl9601b_raw_cf_usAct_t));
    ds_act.idx = pClassifyCfg->index;
    us_act.idx = pClassifyCfg->index;
    /* get entry action and get not operation */
    if(CLASSIFY_DIRECTION_US == pClassifyCfg->direction)
    {
        RT_ERR_CHK(rtl9601b_raw_cf_usAct_get(&us_act), ret);
        RT_ERR_CHK(rtl9601b_raw_cf_usOp_get(pClassifyCfg->index, &not_op), ret);
    }
    else
    {
        RT_ERR_CHK(rtl9601b_raw_cf_dsAct_get(&ds_act), ret);
        RT_ERR_CHK(rtl9601b_raw_cf_dsOp_get(pClassifyCfg->index, &not_op), ret);
    }

    /* translate data structure */
    RT_ERR_CHK(dal_rtl9601b_classify_entry2cfg(&us_act, &ds_act, pClassifyCfg), ret);
    if(DISABLED == not_op)
        pClassifyCfg->invert = CLASSIFY_INVERT_DISABLE;
    else
        pClassifyCfg->invert = CLASSIFY_INVERT_ENABLE;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_classify_cfgEntry_get */


/* Function Name:
 *      dal_rtl9601b_classify_cfgEntry_del
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
dal_rtl9601b_classify_cfgEntry_del(uint32 entryIdx)
{
    rtl9601b_raw_cf_RuleEntry_t entry;
    rtl9601b_raw_cf_dsAct_t ds_act;
    rtl9601b_raw_cf_usAct_t us_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_ENTRY_MAX() <= entryIdx), RT_ERR_INPUT);

    osal_memset(&entry, 0x0, sizeof(rtl9601b_raw_cf_RuleEntry_t));
    osal_memset(&ds_act, 0x0, sizeof(rtl9601b_raw_cf_dsAct_t));
    osal_memset(&us_act, 0x0, sizeof(rtl9601b_raw_cf_usAct_t));

    entry.idx  = entryIdx;
    entry.valid = 1; /* always set to 1 for tcam valid bit */
    ds_act.idx = entryIdx;
    us_act.idx = entryIdx;

    RT_ERR_CHK(rtl9601b_raw_cf_valid_set(entryIdx, CF_ENTRY_INVALID), ret);

    RT_ERR_CHK(rtl9601b_raw_cf_rule_set(&entry), ret);
    RT_ERR_CHK(rtl9601b_raw_cf_dsAct_set(&ds_act), ret);
    RT_ERR_CHK(rtl9601b_raw_cf_usAct_set(&us_act), ret);

    RT_ERR_CHK(rtl9601b_raw_cf_dsOp_set(entryIdx, CLASSIFY_INVERT_DISABLE), ret);
    RT_ERR_CHK(rtl9601b_raw_cf_usOp_set(entryIdx, CLASSIFY_INVERT_DISABLE), ret);

    return RT_ERR_OK;
} /* end of dal_rtl9601b_classify_cfgEntry_del */


/* Function Name:
 *      dal_rtl9601b_classify_field_add
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
 *      Pointer pFieldHead points to an Classification configuration structure, this structure keeps multiple
 *      comparison rules by means of linked list. Pointer pClassifyField will be added to linked
 *      list keeped by structure that pClassifyEntry points to.
 *      - caller should not free (*pClassifyField) before dal_rtl9601b_classify_cfgEntry_add is called
 */
int32
dal_rtl9601b_classify_field_add(rtk_classify_cfg_t *pClassifyEntry, rtk_classify_field_t *pClassifyField)
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
        case CLASSIFY_FIELD_INTER_PRI:
        case CLASSIFY_FIELD_IS_CTAG:
        case CLASSIFY_FIELD_IS_STAG:
        case CLASSIFY_FIELD_UNI:
        case CLASSIFY_FIELD_ACL_HIT:
        case CLASSIFY_FIELD_DEI:
		case CLASSIFY_FIELD_INNER_VLAN:
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
} /* end of dal_rtl9601b_classify_field_add */


/* Function Name:
 *      dal_rtl9601b_classify_unmatchAction_set
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
dal_rtl9601b_classify_unmatchAction_set(rtk_classify_unmatch_action_t action)
{
    rtl9601b_raw_cf_usPermit_t raw_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((CLASSIFY_UNMATCH_END <= action), RT_ERR_INPUT);

    switch(action)
    {
        case CLASSIFY_UNMATCH_DROP:
            raw_act = CF_US_PERMIT_DROP;
            break;
        case CLASSIFY_UNMATCH_PERMIT:
            raw_act = CF_US_PERMIT_NORMAL;
            break;
        case CLASSIFY_UNMATCH_PERMIT_WITHOUT_PON:
            raw_act = CF_US_PERMIT_NOPON;
            break;
        default:
            return RT_ERR_INPUT;
    }
    RT_ERR_CHK(rtl9601b_raw_cf_usPermit_set(raw_act), ret);

    return RT_ERR_OK;
} /* end of dal_rtl9601b_classify_unmatchAction_set */


/* Function Name:
 *      dal_rtl9601b_classify_unmatchAction_get
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
dal_rtl9601b_classify_unmatchAction_get(rtk_classify_unmatch_action_t *pAction)
{
    rtl9601b_raw_cf_usPermit_t raw_act;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    RT_ERR_CHK(rtl9601b_raw_cf_usPermit_get(&raw_act), ret);
    switch(raw_act)
    {
        case CF_US_PERMIT_NORMAL:
            *pAction = CLASSIFY_UNMATCH_PERMIT;
            break;
        case CF_US_PERMIT_DROP:
            *pAction = CLASSIFY_UNMATCH_DROP;
            break;
        case CF_US_PERMIT_NOPON:
            *pAction = CLASSIFY_UNMATCH_PERMIT_WITHOUT_PON;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_classify_unmatchAction_get */


/* Function Name:
 *      dal_rtl9601b_classify_cfPri2Dscp_set
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
dal_rtl9601b_classify_cfPri2Dscp_set(rtk_pri_t pri, rtk_dscp_t dscp)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pri), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_VALUE_OF_DSCP_MAX < dscp), RT_ERR_INPUT);

    if((ret = reg_array_field_write(RTL9601B_RMK_DSCP_CF_PRI_CTRLr, REG_ARRAY_INDEX_NONE, pri, RTL9601B_CFPRI_DSCPf, &dscp)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9601b_classify_cfPri2Dscp_get
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
dal_rtl9601b_classify_cfPri2Dscp_get(rtk_pri_t pri, rtk_dscp_t *pDscp)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_CLASSIFY),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(classify_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pri), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pDscp), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(RTL9601B_RMK_DSCP_CF_PRI_CTRLr, REG_ARRAY_INDEX_NONE, pri, RTL9601B_CFPRI_DSCPf, pDscp)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_CLASSIFY|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

