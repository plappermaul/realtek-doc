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
 * Purpose : switch asic-level ACL API 
 * Feature : ACL related functions
 *
 */
 
#include <dal/apollomp/raw/apollomp_raw_acl.h>
#include <osal/lib.h>


/* Function Name:
 *      apollomp_raw_acl_rule_set
 * Description:
 *      Set acl rule content
 * Input:
 *      pAclRule - ACL rule stucture for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_OK 				- Success
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL rule index (0-63)
 *      RT_ERR_NULL_POINTER     - input parameter is null pointer
 * Note:
 *		System supported 64 shared 289-bit ACL ingress rule. Index was available at range 0-63 only. 
 *		If software want to modify ACL rule, the ACL function should be disable at first or unspecify 
 *		acl action will be executed. 
 *		One ACL rule structure has three parts setting:
 *		Bit 0-143		Data Bits of this Rule
 *		Bit	144		Valid Bit
 *		Bit 145-288	Care Bits of this Rule
 *		There are four kinds of field in Data Bits and Care Bits: Active Portmask, Type, Tag Exist, and 8 fields
 */
 
int32 apollomp_raw_acl_rule_set(apollomp_raw_acl_ruleEntry_t *pAclRule)
{
	int32 ret = RT_ERR_FAILED;
	uint32 i;
    uint32 entryIdx;
    uint32 aclFieldNum = 4;
    uint32 modeField0 = APOLLOMP_ACL_DATA2_FIELD0_0tf;
    uint32 modeMask0 = APOLLOMP_ACL_MASK2_FIELD0_0tf;
	uint32 fieldData[APOLLOMP_ACLRULETBLEN];
    uint32 fieldMask[APOLLOMP_ACLRULETBLEN];
    uint32 fieldData2[APOLLOMP_ACLRULETBLEN];
    uint32 fieldMask2[APOLLOMP_ACLRULETBLEN];
	uint32 tmpData;
    
	RT_PARAM_CHK(NULL == pAclRule , RT_ERR_NULL_POINTER);	
    RT_PARAM_CHK(1 < pAclRule->valid, RT_ERR_OUT_OF_RANGE);

    if (APOLLOMP_ACL_MODE_64ENTRIES == pAclRule->mode)
    {
    
        RT_PARAM_CHK(HAL_MAX_NUM_OF_ACL_RULE_ENTRY() < pAclRule->idx, RT_ERR_OUT_OF_RANGE);   
        osal_memset(fieldData, 0, sizeof(fieldData));
        for (i=0; i<HAL_MAX_NUM_OF_ACL_RULE_FIELD(); i++)
        {   
            if ((ret = table_field_set(APOLLOMP_ACL_DATAt, APOLLOMP_ACL_DATA_FIELD0tf-i, &pAclRule->data_bits.field[i], fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
        }
        if ((ret = table_field_set(APOLLOMP_ACL_DATAt,APOLLOMP_ACL_DATA_PMSKtf, &(pAclRule->data_bits.active_portmsk), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_set(APOLLOMP_ACL_DATAt, APOLLOMP_ACL_DATA_TYPEtf, &(pAclRule->data_bits.type),fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_field_set(APOLLOMP_ACL_DATAt, APOLLOMP_ACL_DATA_TAGStf, &(pAclRule->data_bits.tag_exist),fieldData)) != RT_ERR_OK)
        { 
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
        osal_memset(fieldMask,0,sizeof(fieldMask));
        for (i=0; i<HAL_MAX_NUM_OF_ACL_RULE_FIELD(); i++)
        {
            if ((ret = table_field_set(APOLLOMP_ACL_MASKt, APOLLOMP_ACL_MASK_FIELD0tf-i, &pAclRule->care_bits.field[i], fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }       
        
        }
        if ((ret = table_field_set(APOLLOMP_ACL_MASKt, APOLLOMP_ACL_MASK_PMSKtf, &(pAclRule->care_bits.active_portmsk), fieldMask)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
             return ret;
        }
        if ((ret = table_field_set(APOLLOMP_ACL_MASKt, APOLLOMP_ACL_MASK_TYPEtf, &(pAclRule->care_bits.type),fieldMask)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
             return ret;
        }
        if ((ret = table_field_set(APOLLOMP_ACL_MASKt, APOLLOMP_ACL_MASK_TAGStf, &(pAclRule->care_bits.tag_exist), fieldMask)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
             return ret;
        }
        for (i = 0; i < APOLLOMP_ACLRULETBLEN; i++)
        {
            fieldData2[i] = fieldMask[i]&fieldData[i];
            fieldMask2[i] = fieldMask[i]&~fieldData[i];
        }

        if ((table_field_set(APOLLOMP_ACL_DATAt, APOLLOMP_ACL_DATA_VALIDtf, &pAclRule->valid, fieldData2)) != RT_ERR_OK)
        {   
             RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
             return ret;
        }
        if ((ret = table_write(APOLLOMP_ACL_DATAt, pAclRule->idx, fieldData2)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
        if ((ret = table_write(APOLLOMP_ACL_MASKt, pAclRule->idx, fieldMask2)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

    }
    else if (APOLLOMP_ACL_MODE_128ENTRIES == pAclRule->mode)
    {    
	    RT_PARAM_CHK(HAL_MAX_NUM_OF_ACL_ACTION() < pAclRule->idx, RT_ERR_OUT_OF_RANGE);	
        osal_memset(fieldData, 0, sizeof(fieldData));

        entryIdx = pAclRule->idx & (HAL_MAX_NUM_OF_ACL_RULE_ENTRY() - 1);

        if ((ret = table_read(APOLLOMP_ACL_DATAt, entryIdx, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
        osal_memset(fieldMask, 0, sizeof(fieldMask));
        if ((ret = table_read(APOLLOMP_ACL_MASKt, entryIdx, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
             return ret;
        }
        if (pAclRule->idx >= HAL_MAX_NUM_OF_ACL_RULE_ENTRY())
        {
            aclFieldNum = 3;
            modeField0 = APOLLOMP_ACL_DATA2_FIELD0_1tf;
            tmpData = pAclRule->data_bits.active_portmsk & pAclRule->care_bits.active_portmsk;
            if ((ret = table_field_set(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_PMSK_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            tmpData = pAclRule->data_bits.type & pAclRule->care_bits.type;
            if ((ret = table_field_set(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_TYPE_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            tmpData = pAclRule->data_bits.tag_exist & pAclRule->care_bits.tag_exist;
            if ((ret = table_field_set(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_TAGS_1tf, &tmpData, fieldData)) != RT_ERR_OK)
            { 
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
   
            modeMask0 = APOLLOMP_ACL_MASK2_FIELD0_1tf;
            tmpData = pAclRule->care_bits.active_portmsk&(~pAclRule->data_bits.active_portmsk);
            if ((ret = table_field_set(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_PMSK_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            tmpData = pAclRule->care_bits.type&(~pAclRule->data_bits.type);
            if ((ret = table_field_set(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_TYPE_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            tmpData = pAclRule->care_bits.tag_exist&(~pAclRule->data_bits.tag_exist);
            if ((ret = table_field_set(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_TAGS_1tf, &tmpData, fieldMask)) != RT_ERR_OK)
            { 
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }

            
        }
        else {
            tmpData = pAclRule->data_bits.active_portmsk & pAclRule->care_bits.active_portmsk;
            if ((ret = table_field_set(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_PMSK_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            tmpData = pAclRule->data_bits.type & pAclRule->care_bits.type;
            if ((ret = table_field_set(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_TYPE_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            tmpData = pAclRule->data_bits.tag_exist & pAclRule->care_bits.tag_exist;
            if ((ret = table_field_set(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_TAGS_0tf, &tmpData, fieldData)) != RT_ERR_OK)
            { 
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }

            tmpData = pAclRule->care_bits.active_portmsk&(~pAclRule->data_bits.active_portmsk);
            if ((ret = table_field_set(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_PMSK_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            tmpData = pAclRule->care_bits.type&(~pAclRule->data_bits.type);
            if ((ret = table_field_set(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_TYPE_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            tmpData = pAclRule->care_bits.tag_exist&(~pAclRule->data_bits.tag_exist);
            if ((ret = table_field_set(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_TAGS_0tf, &tmpData, fieldMask)) != RT_ERR_OK)
            { 
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
        }             
	    for (i = 0; i < aclFieldNum; i++)
        {   
            tmpData = pAclRule->care_bits.field[i]&pAclRule->data_bits.field[i];
            if ((ret = table_field_set(APOLLOMP_ACL_DATA2t, modeField0-i, &pAclRule->data_bits.field[i], fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            tmpData = pAclRule->care_bits.field[i]&(~pAclRule->data_bits.field[i]);
            if ((ret = table_field_set(APOLLOMP_ACL_MASK2t, modeMask0-i, &tmpData, fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
        }
        if ((table_field_set(APOLLOMP_ACL_DATAt, APOLLOMP_ACL_DATA_VALIDtf, &pAclRule->valid, fieldData)) != RT_ERR_OK)
       	{   
             RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
             return ret;
        }	
  
        if ((ret = table_write(APOLLOMP_ACL_DATAt, entryIdx, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        if ((ret = table_write(APOLLOMP_ACL_MASKt, entryIdx, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }     
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_acl_rule_get
 * Description:
 *      Get acl rule content
 * Input:
 *		None
 * Output:
 *      pAclRule - ACL rule stucture 
 * Return:
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_OK 				- Success
 *      RT_ERR_OUT_OF_RANGE  	- Invalid ACL rule index (0-63)
 *      RT_ERR_NULL_POINTER     - input parameter is null pointer
 * Note:
 *		None
 */
int32 apollomp_raw_acl_rule_get(apollomp_raw_acl_ruleEntry_t *pAclRule)
{
	int32 ret = RT_ERR_FAILED;
	uint32 i;
    uint32 entryIdx;
    uint32 aclFieldNum = 4;
    uint32 modeField0 = APOLLOMP_ACL_DATA2_FIELD0_0tf;
    uint32 modeMask0 = APOLLOMP_ACL_MASK2_FIELD0_0tf;
    uint32 fieldData[APOLLOMP_ACLRULETBLEN];
    uint32 fieldMask[APOLLOMP_ACLRULETBLEN];

	RT_PARAM_CHK(NULL == pAclRule, RT_ERR_NULL_POINTER);

    if (APOLLOMP_ACL_MODE_64ENTRIES == pAclRule->mode)
    {
	    RT_PARAM_CHK(pAclRule->idx > HAL_MAX_NUM_OF_ACL_RULE_ENTRY(), RT_ERR_OUT_OF_RANGE);	
	
	    osal_memset(fieldData, 0x00, sizeof(fieldData));
        if ((ret = table_read(APOLLOMP_ACL_DATAt, pAclRule->idx, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    
	    for (i=0; i<HAL_MAX_NUM_OF_ACL_RULE_FIELD() ;i++)
    	{
            if((ret = table_field_get(APOLLOMP_ACL_DATAt, APOLLOMP_ACL_DATA_FIELD0tf-i, &pAclRule->data_bits.field[i], fieldData)) != RT_ERR_OK)
      	    {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
           	}
	    }
    	if ((ret = table_field_get(APOLLOMP_ACL_DATAt, APOLLOMP_ACL_DATA_PMSKtf, &(pAclRule->data_bits.active_portmsk), fieldData)) != RT_ERR_OK)
    	{
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
    	}
        
        if ((ret = table_field_get(APOLLOMP_ACL_DATAt, APOLLOMP_ACL_DATA_TYPEtf, &(pAclRule->data_bits.type), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    	if ((ret = table_field_get(APOLLOMP_ACL_DATAt, APOLLOMP_ACL_DATA_TAGStf, &(pAclRule->data_bits.tag_exist), fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

        /*retrive valit bit from acl data bits*/
        if ((ret = table_field_get(APOLLOMP_ACL_DATAt, APOLLOMP_ACL_DATA_VALIDtf, &pAclRule->valid, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
        
	    osal_memset(fieldMask, 0x00, sizeof(fieldMask));
        if ((ret = table_read(APOLLOMP_ACL_MASKt, pAclRule->idx, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
        for (i = 0; i < APOLLOMP_ACLRULETBLEN; i++)
            fieldMask[i] = fieldMask[i]^fieldData[i];

      	for (i = 0; i < HAL_MAX_NUM_OF_ACL_RULE_FIELD(); i++)
        { 
            if ((ret = table_field_get(APOLLOMP_ACL_MASKt, APOLLOMP_ACL_MASK_FIELD0tf-i, &pAclRule->care_bits.field[i], fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
    
            }
        }
        if ((ret = table_field_get(APOLLOMP_ACL_MASKt, APOLLOMP_ACL_MASK_PMSKtf, &(pAclRule->care_bits.active_portmsk), fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;    
        }
        if ((ret = table_field_get(APOLLOMP_ACL_MASKt, APOLLOMP_ACL_MASK_TYPEtf, &(pAclRule->care_bits.type), fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
    	if ((ret = table_field_get(APOLLOMP_ACL_MASKt, APOLLOMP_ACL_MASK_TAGStf, &(pAclRule->care_bits.tag_exist), fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }

    }
    else if (APOLLOMP_ACL_MODE_128ENTRIES == pAclRule->mode)
    {
        
        RT_PARAM_CHK(pAclRule->idx > HAL_MAX_NUM_OF_ACL_ACTION(), RT_ERR_OUT_OF_RANGE);	
        
        osal_memset(fieldData, 0, sizeof(fieldData));
        entryIdx = pAclRule->idx & (HAL_MAX_NUM_OF_ACL_RULE_ENTRY()-1);
        if ((ret = table_read(APOLLOMP_ACL_DATAt, entryIdx, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
        osal_memset(fieldMask, 0, sizeof(fieldMask));
        if ((ret = table_read(APOLLOMP_ACL_MASKt, entryIdx, fieldMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
             return ret;
        }

        for (i = 0; i < APOLLOMP_ACLRULETBLEN; i++)
            fieldMask[i] =  fieldMask[i]^fieldData[i];
        
        if (pAclRule->idx >= HAL_MAX_NUM_OF_ACL_RULE_ENTRY())
        {
        
            aclFieldNum = 3;
            modeField0 = APOLLOMP_ACL_DATA2_FIELD0_1tf;
            if ((ret = table_field_get(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_PMSK_1tf, &(pAclRule->data_bits.active_portmsk), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_TYPE_1tf, &(pAclRule->data_bits.type), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_TAGS_1tf, &(pAclRule->data_bits.tag_exist), fieldData)) != RT_ERR_OK)
            { 
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            modeMask0 = APOLLOMP_ACL_MASK2_FIELD0_1tf;
            if ((ret = table_field_get(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_PMSK_1tf, &(pAclRule->care_bits.active_portmsk), fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_TYPE_1tf, &(pAclRule->care_bits.type), fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_TAGS_1tf, &(pAclRule->care_bits.tag_exist), fieldMask)) != RT_ERR_OK)
            { 
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }            
            
        }
        else {
            if ((ret = table_field_get(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_PMSK_0tf, &(pAclRule->data_bits.active_portmsk), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_TYPE_0tf, &(pAclRule->data_bits.type), fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_TAGS_0tf, &(pAclRule->data_bits.tag_exist), fieldData)) != RT_ERR_OK)
            { 
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            
            if ((ret = table_field_get(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_PMSK_0tf, &(pAclRule->care_bits.active_portmsk), fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_TYPE_0tf, &(pAclRule->care_bits.type), fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_ACL_MASK2t, APOLLOMP_ACL_MASK2_TAGS_0tf, &(pAclRule->care_bits.tag_exist), fieldMask)) != RT_ERR_OK)
            { 
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
         }             
        
	    for (i = 0; i < aclFieldNum; i++)
        {   
            if ((ret = table_field_get(APOLLOMP_ACL_DATA2t, modeField0-i, &pAclRule->data_bits.field[i], fieldData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
            if ((ret = table_field_get(APOLLOMP_ACL_MASK2t, modeMask0-i, &pAclRule->care_bits.field[i], fieldMask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
                return ret;
            }
        }
        if ((ret = table_field_get(APOLLOMP_ACL_DATA2t, APOLLOMP_ACL_DATA2_VALIDtf, &pAclRule->valid, fieldData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            return ret;
        }
        

    }
	return RT_ERR_OK;
}



