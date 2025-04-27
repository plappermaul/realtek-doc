/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : Definition those register command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) register
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <osal/memory.h>
#include <hal/chipdef/allreg.h>
#include <rtk/diag.h>

#include <hal/mac/reg.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_debug.h>
  #include <rtrpc/rtrpc_diag.h>
#endif


#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)

/* negative array index represents 'show all' */
#define DIAG_RTK_REG_ARRAY_IDX(array_idx)   \
    ((array_idx>=0)? array_idx : RTK_DIAG_REG_IDX_NO_FILTER)

/* REG ARRAY 1-dimension indexed by index2 */
#define DIAG_IS_REG_ARRAY_1D_IDX2(info)  \
    (((info.harray - info.larray) > 0) && ((info.hport - info.lport) == 0))
/* REG ARRAY 1-dimension indexed by index1 */
#define DIAG_IS_REG_ARRAY_1D_IDX1(info)  \
    (((info.harray - info.larray) == 0)&&((info.hport - info.lport) > 0) )
/* REG ARRAY 2-dimension */
#define DIAG_IS_REG_ARRAY_2D(info)  \
    (((info.harray - info.larray) > 0) && ((info.hport - info.lport) > 0) )
/* Check if a REG ARRAY */
#define DIAG_IS_REG_ARRAY(info)  \
    (((info.harray - info.larray) > 0) || ((info.hport - info.lport) > 0))

#define DIAG_RTK_REG_FIELD_IDX(pReg, field_idx) \
        pReg->pFields[field_idx].name

#define DIAG_RTK_REG_FIELD_NAME(pReg, field_idx) \
        pReg->pFields[field_idx].field_name


int32
_cparser_register_name_parse(char *reg_str, rtk_diag_regArrFilter_t *usr_input)
{
    char *token;
    int32 array_idx;
    int32 reg_array_idx[2];

    if((NULL == reg_str) || (NULL == usr_input))
    {
        return CPARSER_NOT_OK;
    }

    if((token = osal_strtok(reg_str, ".")) == NULL)
    {
        return CPARSER_NOT_OK;
    }
    osal_strncpy(usr_input->name, token, sizeof(usr_input->name));

    // Parse the REG ARRAY index
    for(array_idx=0; array_idx < RTK_DIAG_REG_ARRAY_IDX_MAX; array_idx++)
    {
        token = osal_strtok(NULL, ".");
        if(token)
        {
            if( 1 != osal_sscanf(token, "%d", &reg_array_idx[array_idx]))
            {
                return CPARSER_NOT_OK;
            }
        }
        else
        {
            reg_array_idx[array_idx] = -1;
        }
    }
    usr_input->filter_1 = reg_array_idx[0];
    usr_input->filter_2 = reg_array_idx[1];

    /* Register name to upper case */
    diag_util_str2upper(usr_input->name);
    return CPARSER_OK;
}


#define DIAG_PRINT_REG_NAME(info, idx1, idx2)  \
    diag_util_printf("%s", info.name);  \
    if(DIAG_IS_REG_ARRAY_1D_IDX1(info))   \
    {                                                \
        diag_util_printf(".%d", idx1);    \
    }                                                \
    else if(DIAG_IS_REG_ARRAY_1D_IDX2(info)) \
    {                                                   \
        diag_util_printf(".%d", idx2);     \
    }                                                 \
    else if(DIAG_IS_REG_ARRAY_2D(info))    \
    {                                                 \
        diag_util_printf(".%d.%d", idx1, idx2);  \
    } \
    else  \
    {   \
        diag_util_printf("[0x%x]", info.offset);  \
    }


#define DIAG_PRINT_REG_VAL(i, reg_words, value)  \
    diag_util_printf("0x");              \
    for (i= 0; i < reg_words; i++)        \
    {                                     \
        if(i != 0)                        \
        {                                 \
            diag_util_printf("-");        \
        }                                 \
        diag_util_printf("%08x", value[i]);  \
    }

#define DIAG_PRINT_FIELD_NAME(info)  \
    diag_util_printf("%s", info.field_name);  \

#define DIAG_PRINT_FIELD_VAL(info, value)  \
    if (info.len == 1)                     \
    {                                      \
        diag_util_printf("%d", value);     \
    }                                      \
    else                                   \
    {                                      \
        diag_util_printf("0x%x",value);   \
    }


/* Input: reg, usr_input
 * Output: idx1_start,idx1_end, idx2_start,idx2_end
 * Determine the array dim1 and dim2 start and end
 */
int32
_cparser_reg_array_dim_loop(rtk_diag_regInfo_t *pReg_info,
                                  rtk_diag_regArrFilter_t usr_input,
                                  int32 *idx1_start, int32 *idx1_end,
                                  int32 *idx2_start, int32 *idx2_end)
{
    rtk_diag_regInfo_t reg_info;

    if((NULL == pReg_info) || (NULL == idx1_start) ||
       (NULL == idx1_end) || (NULL == idx2_start) || (NULL == idx2_end))
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    reg_info = *pReg_info;
    if (DIAG_IS_REG_ARRAY_1D_IDX1(reg_info))
    {
        if(usr_input.filter_1 >= 0 )
        {  //filter array index
            *idx1_start = *idx1_end = usr_input.filter_1;
        }
        else
        {  //no filter
            *idx1_start = reg_info.lport;
            *idx1_end = reg_info.hport;
        }
        *idx2_start = *idx2_end = REG_ARRAY_INDEX_NONE;
    }
    else if (DIAG_IS_REG_ARRAY_1D_IDX2(reg_info))
    {   // Notice: 1 dimension but use REG idx2, use filter_1
        if(usr_input.filter_1 >= 0 )
        {   //filter array index
            *idx2_start = *idx2_end = usr_input.filter_1;
        }
        else
        {   // No filter
            *idx2_start = reg_info.larray;
            *idx2_end = reg_info.harray;
        }
        *idx1_start = *idx1_end = REG_ARRAY_INDEX_NONE;
    }
    else if (DIAG_IS_REG_ARRAY_2D(reg_info))
    {
        if(usr_input.filter_1 >= 0 )
        {  //filter array index
            *idx1_start = *idx1_end = usr_input.filter_1;
        }
        else
        {  // No filter
            *idx1_start = reg_info.lport;
            *idx1_end = reg_info.hport;
        }
        if(usr_input.filter_2 >= 0 )
        {   //filter array index
            *idx2_start = *idx2_end = usr_input.filter_2;
        }
        else
        {   // No filter
            *idx2_start = reg_info.larray;
            *idx2_end = reg_info.harray;
        }
    }
    else
    {
        //Should not enter here
        return CPARSER_ERR_INVALID_PARAMS;
    }
    return CPARSER_OK;
}
#endif


#ifdef CMD_REGISTER_SET_ADDRESS_VALUE
#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
int32 _cparser_register_set_by_name(uint32 unit, char **address_ptr, char **value_ptr)
{
    uint32  value[RTK_DIAG_REG_WORD_NUM_MAX], cnt;
    int32   ret = RT_ERR_FAILED, i, j;
    int32 idx1_start, idx1_end, idx2_start, idx2_end;
    rtk_diag_regArrFilter_t usr_input;
    rtk_diag_regInfo_t reg_info;
    /*
     * Get REG by string
     */
    _cparser_register_name_parse(*address_ptr, &usr_input);

    /* Get REG information - name, offset, array start/end etc...*/
    ret = rtk_diag_regInfoByStr_get(unit, usr_input.name, &reg_info);
    if(ret != RT_ERR_OK)
    {
        diag_util_mprintf("Register not found\n");
        return CPARSER_NOT_OK;
    }

    /* Convert the data string to uint32 array */
    cnt = reg_info.reg_words;
    if((ret = diag_util_convert_mword_string_2_int32_array(*value_ptr, "-", value, &cnt)) != RT_ERR_OK)
    {
        diag_util_mprintf("Input format error!\n");
        return ret;
    }

    if(cnt != (reg_info.reg_words))
    {
        diag_util_mprintf("Value length is too short!\n");
        goto exit;
    }

    /* Get value of REG or REG ARRAY and print */
   if (DIAG_IS_REG_ARRAY(reg_info))
   {
       ret = _cparser_reg_array_dim_loop(&reg_info, usr_input,
                                   &idx1_start, &idx1_end,
                                   &idx2_start, &idx2_end);
       if(ret != CPARSER_OK)
       {
           ret = RT_ERR_FAILED;
           goto exit;
       }
       for(i = idx1_start; i <= idx1_end; i++)
       {
           for(j = idx2_start; j <= idx2_end; j++)
           {
                ret = rtk_diag_regArray_set(unit, reg_info.index,
                                            i, j, value);
                if(ret != RT_ERR_OK)
                {
                    goto exit;
                }
           }
       }
   }
   else
   { //NOT REG ARRAY
        ret = rtk_diag_reg_set(unit, reg_info.index, value);
   }

exit:
    if(ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}
#endif

/*
 * register set <STRING:address> <STRING:value>
 * ex: register set 0x94E4 0x1
 * ex: register set L3_HW_LU_KEY_SIP_CTRL 0x00000010-00000010-00000010-bb003001
 */
cparser_result_t cparser_cmd_register_set_address_value(cparser_context_t *context,
    char **address_ptr, char **value_ptr)
{
    uint32  unit = 0,reg = 0,value  = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    /* If address begins with "0x",
     * get REG by address
     */
    if ((1 == osal_sscanf(*address_ptr, "0x%x", &reg)) ||
        (1 == osal_sscanf(*address_ptr, "%u", &reg)))
    {
        if (0 != (reg % 4))
        {
            diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
            return CPARSER_NOT_OK;
        }

        if((1 != osal_sscanf(*value_ptr, "0x%x", &value)) &&
           (1 != osal_sscanf(*value_ptr, "%x", &value)) &&
           (1 != osal_sscanf(*value_ptr, "%u", &value)))
        {
            diag_util_printf("\n\rValue format error!!\n\r\n\r");
            return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(ioal_mem32_write(unit, reg, value), ret);
        return CPARSER_OK;
    }
    else
    {
        #if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
        return _cparser_register_set_by_name(unit, address_ptr, value_ptr);
        #else
        diag_util_mprintf("Not support; Please enable CONFIG_SDK_DUMP_REG_WITH_NAME\n");
        #endif
    }

    return CPARSER_OK;
} /* end of cparser_cmd_register_set_address_value */
#endif

#ifdef CMD_REGISTER_GET_ADDRESS_WORDS
#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
int32 _cparser_register_get_by_name(uint32 unit, char **address_ptr)
{
    uint32 rvalue[RTK_DIAG_REG_WORD_NUM_MAX], fvalue;
    int32 ret = RT_ERR_OK,i ,j, k, field_idx;
    int32 idx1_start, idx1_end, idx2_start, idx2_end;
    rtk_diag_regArrFilter_t usr_input;
    rtk_diag_regInfo_t reg_info;
    rtk_diag_regFieldInfo_t *pField_info=NULL;

    /* If address does NOT begin with "0x",
     * get REG by string
     */
    _cparser_register_name_parse(*address_ptr, &usr_input);

    /* Get REG information - name, offset, array start/end etc...*/
    ret = rtk_diag_regInfoByStr_get(unit, usr_input.name, &reg_info);
    if(ret != RT_ERR_OK)
    {
        diag_util_mprintf("Register not found\n");
        return CPARSER_NOT_OK;
    }

    /* Check REG word number */
    if(reg_info.reg_words > RTK_DIAG_REG_WORD_NUM_MAX )
    {
        diag_util_mprintf("Register word number exceeds buffer length.\n");
        return CPARSER_NOT_OK;
    }

    /* Get Field information - name, lsp etc...*/
    pField_info = osal_alloc(reg_info.field_num * sizeof(rtk_diag_regFieldInfo_t));
    if(NULL == pField_info)
    {
        return RT_ERR_MEM_ALLOC;
    }
    for (field_idx = 0; field_idx < reg_info.field_num; field_idx++)
    {
        ret = rtk_diag_regFieldInfo_get(unit, reg_info.index, field_idx, &pField_info[field_idx]);

        if(ret != RT_ERR_OK)
        {
            //Should not enter here
            diag_util_mprintf("Get Field info failed!\n");
            osal_free(pField_info);
            return CPARSER_NOT_OK;
        }
    }

    /* Call RTK API to get value of REG or REG ARRAY and print */
    if (DIAG_IS_REG_ARRAY(reg_info))
    {
        ret = _cparser_reg_array_dim_loop(&reg_info, usr_input,
                                    &idx1_start, &idx1_end,
                                    &idx2_start, &idx2_end);
        if(ret != CPARSER_OK)
        {
            ret = RT_ERR_FAILED;
            goto exit;
        }
        for(i = idx1_start; i <= idx1_end; i++)
        {
            for(j = idx2_start; j <= idx2_end; j++)
            {
                ret = rtk_diag_regArray_get(unit, reg_info.index, i, j, rvalue);
                if(ret != RT_ERR_OK)
                {
                    goto exit;
                }

                DIAG_PRINT_REG_NAME(reg_info, i, j);
                diag_util_printf("=");
                DIAG_PRINT_REG_VAL(k, reg_info.reg_words, rvalue);

                diag_util_printf("   <");
                for (field_idx = 0; field_idx < reg_info.field_num; field_idx++)
                {
                    ret = rtk_diag_regArrayField_get(unit, reg_info.index, i, j,
                                                     pField_info[field_idx].index,
                                                     &fvalue);
                    if(ret != RT_ERR_OK)
                    {
                        goto exit;
                    }

                    if(field_idx > 0)
                    {
                        diag_util_printf(",");
                    }
                    DIAG_PRINT_FIELD_NAME(pField_info[field_idx]);
                    diag_util_printf("=");
                    DIAG_PRINT_FIELD_VAL(pField_info[field_idx], fvalue);
                }
                diag_util_printf(">\n");
            }
        }
    }
    else
    {  //NOT REG ARRAY
        ret = rtk_diag_reg_get(unit, reg_info.index, rvalue);
        if(ret != RT_ERR_OK)
        {
            osal_free(pField_info);
            return CPARSER_NOT_OK;
        }

        DIAG_PRINT_REG_NAME(reg_info, 0, 0);
        diag_util_printf("=");
        DIAG_PRINT_REG_VAL(k, reg_info.reg_words, rvalue);

        diag_util_printf("   <");
        for (field_idx = 0; field_idx < reg_info.field_num; field_idx++)
        {

            ret = rtk_diag_regField_get(unit, reg_info.index,
                                        pField_info[field_idx].index,
                                        &fvalue);
            if(ret != RT_ERR_OK)
            {
                goto exit;
            }

            if(field_idx > 0)
            {
                diag_util_printf(",");
            }
            DIAG_PRINT_FIELD_NAME(pField_info[field_idx]);
            diag_util_printf("=");
            DIAG_PRINT_FIELD_VAL(pField_info[field_idx], fvalue);
        }
        diag_util_printf(">\n");
    }

exit:
    osal_free(pField_info);
    if(ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}
#endif

/*
 * register get <STRING:address> { <UINT:words> }
 * ex: register get 0x94E4
 * ex: register get L3_HW_LU_KEY_SIP_CTRL
 */
cparser_result_t cparser_cmd_register_get_address_words(cparser_context_t *context,
    char **address_ptr, uint32_t *words_ptr)

{
    uint32  unit = 0, index;
    uint32  reg = 0, value, reg_words;
    int32 ret = RT_ERR_OK;

    /* Don't check the (NULL == words_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((1 == osal_sscanf(*address_ptr, "0x%x", &reg)) ||
        (1 == osal_sscanf(*address_ptr, "%u", &reg)))
    {
        if (0 != (reg % 4))
        {
            diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
            return CPARSER_NOT_OK;
        }

        if (3 == TOKEN_NUM)
        {
            DIAG_UTIL_ERR_CHK(ioal_mem32_read(unit, reg, &value), ret);
            diag_util_mprintf("Register 0x%x : 0x%08x\n", reg, value);
        }
        else
        {
            reg_words = *words_ptr;
            for (index = 0; index < reg_words; index++)
            {
                DIAG_UTIL_ERR_CHK(ioal_mem32_read(unit, reg, &value), ret);
                if (0 == (index % 4))
                {
                    diag_util_mprintf("\n");
                    diag_util_mprintf("0x%08x ", reg);
                }
                diag_util_mprintf("0x%08x ", value);
                reg = reg + 4;
            }
            diag_util_mprintf("\n");
        }
        return CPARSER_OK;
    }
    else
    {
        #if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
        _cparser_register_get_by_name(unit, address_ptr);
        #else
        diag_util_mprintf("Not support; Please enable CONFIG_SDK_DUMP_REG_WITH_NAME\n");
        #endif
    }

    return CPARSER_OK;
} /* end of cparser_cmd_register_get_address_words */
#endif


#ifdef CMD_REGISTER_LIST_KEYWORD
/*
 * register list <STRING:key_word>
 */
cparser_result_t cparser_cmd_register_list_key_word(cparser_context_t *context,
    char **key_word_ptr)
{
#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
    uint32 unit, field_idx;
    int32 ret, reg ;
    rtk_diag_regInfo_t reg_info;
    rtk_diag_regFieldInfo_t *pField_info;
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* Keyword to upper case */
    diag_util_str2upper(*key_word_ptr);

    /* Get REG information - name, offset, array start/end etc...*/
    ret = rtk_diag_regInfoByStr_get(unit, *key_word_ptr, &reg_info);
    if(ret == RT_ERR_OK)
    {
        /* Get Field information - name, lsp etc...*/
        pField_info = osal_alloc(reg_info.field_num * sizeof(rtk_diag_regFieldInfo_t));
        if(NULL == pField_info)
        {
            return RT_ERR_MEM_ALLOC;
        }
        for (field_idx = 0; field_idx < reg_info.field_num; field_idx++)
        {
            ret = rtk_diag_regFieldInfo_get(unit, reg_info.index, field_idx, &pField_info[field_idx]);

            if(ret != RT_ERR_OK)
            {
                //Should not enter here
                diag_util_mprintf("Get Field info failed!\n");
                osal_free(pField_info);
                return CPARSER_NOT_OK;
            }
            if(pField_info[field_idx].len == 1)
            {
                diag_util_mprintf("%s<%d>\n", pField_info[field_idx].field_name, pField_info[field_idx].lsp );
            }
            else
            {
                diag_util_mprintf("%s<%d,%d>\n", pField_info[field_idx].field_name,
                                               pField_info[field_idx].lsp+pField_info[field_idx].len - 1,
                                               pField_info[field_idx].lsp);
            }
        }
        osal_free(pField_info);
        return CPARSER_OK;
    }

    reg = -1;
    while((ret = rtk_diag_regInfoByStr_match(unit, *key_word_ptr, reg, &reg_info)) == RT_ERR_OK)
    {
        reg = reg_info.index;
        diag_util_printf("%s\n", reg_info.name);
    }

    if(reg == -1)
    {
        diag_util_printf("NOT Found\n");
    }
#else
    diag_util_mprintf("Not support; Please enable CONFIG_SDK_DUMP_REG_WITH_NAME\n");
#endif
    return CPARSER_OK;
}

#endif


#ifdef CMD_REGISTER_MODIFY_REG_NAME_FIELD_VAL_PAIRS
/*
 * register modify <STRING:reg_name> <STRING:field_val_pairs>
 */
cparser_result_t cparser_cmd_register_modify_reg_name_field_val_pairs(cparser_context_t *context,
    char **reg_name_ptr, char **field_val_pairs_ptr)
{
#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)
#define MAX_NUM_FIELD_VALUE_PAIR 12
    char *pField_str, *pValue_str, *pStr;
    uint32 input_fld_idx[MAX_NUM_FIELD_VALUE_PAIR];
    uint32 input_val[MAX_NUM_FIELD_VALUE_PAIR];
    uint32 pair_cnt = 0, unit, value, field_idx;
    int32 ret = RT_ERR_OK, i, j, k, found;
    int32 idx1_start, idx1_end, idx2_start, idx2_end;
    rtk_diag_regArrFilter_t usr_input;
    rtk_diag_regInfo_t reg_info;
    rtk_diag_regFieldInfo_t *pField_info=NULL;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    _cparser_register_name_parse(*reg_name_ptr, &usr_input);

    /* Get REG information - name, offset, array start/end etc...*/
    ret = rtk_diag_regInfoByStr_get(unit, usr_input.name, &reg_info);
    if(ret != RT_ERR_OK)
    {
        diag_util_mprintf("Register not found\n");
        return CPARSER_NOT_OK;
    }

    /* Get Field information - name, lsp etc...*/
    pField_info = osal_alloc(reg_info.field_num * sizeof(rtk_diag_regFieldInfo_t));
    if(NULL == pField_info)
    {
        return RT_ERR_MEM_ALLOC;
    }

    for (field_idx = 0; field_idx < reg_info.field_num; field_idx++)
    {
        ret = rtk_diag_regFieldInfo_get(unit, reg_info.index, field_idx, &pField_info[field_idx]);
        if(ret != RT_ERR_OK)
        {
            //Should not enter here
            diag_util_mprintf("Get Field info failed!\n");
            osal_free(pField_info);
            return CPARSER_NOT_OK;
        }
    }

    /* Parse fields and values */
    pStr = *field_val_pairs_ptr;
    while (((pField_str = osal_strtok(pStr, "=")) != NULL) &&
           ((pValue_str = osal_strtok(NULL, ",")) != NULL) &&
            (pair_cnt < MAX_NUM_FIELD_VALUE_PAIR))
    {
        pStr = NULL;
        if((1 != osal_sscanf(pValue_str, "0x%x", &value)) &&
           (1 != osal_sscanf(pValue_str, "%x", &value)))
        {
            diag_util_mprintf("Input format error!\n");
            goto exit;
        }

        diag_util_str2upper(pField_str);

        /* Compare name to find field index */
        found = 0;
        for (field_idx = 0; field_idx < reg_info.field_num; field_idx++)
        {
            if(strcmp(pField_info[field_idx].field_name, pField_str) == 0)
            {
                input_fld_idx[pair_cnt] = pField_info[field_idx].index;
                input_val[pair_cnt] = value;
                pair_cnt++;
                found = 1;
            }
        }
        if(found == 0)
        {
            diag_util_mprintf("Unknown field %s\n", pField_str);
        }
    }

        if((pField_str != NULL) && (pair_cnt == MAX_NUM_FIELD_VALUE_PAIR))
        {
        diag_util_mprintf("Over the Maximum Field number %d\n",MAX_NUM_FIELD_VALUE_PAIR);
        }

        if(pair_cnt == 0)
        {
        diag_util_mprintf("Field-value pair input format error\n");
        goto exit;
        }

    /* Call RTK API to set value of fields */
    if (DIAG_IS_REG_ARRAY(reg_info))
    {
        ret = _cparser_reg_array_dim_loop(&reg_info, usr_input,
                                    &idx1_start, &idx1_end,
                                    &idx2_start, &idx2_end);

        if(ret != CPARSER_OK)
        {
            ret = RT_ERR_FAILED;
            goto exit;
        }
        for(i = idx1_start; i <= idx1_end; i++)
        {
            for(j = idx2_start; j <= idx2_end; j++)
            {
                for (k = 0; k < pair_cnt; k++)
                {
                    ret = rtk_diag_regArrayField_set(unit,reg_info.index, i, j,
                                                     input_fld_idx[k], &input_val[k]);
                    if(ret != RT_ERR_OK)
                    {
                        goto exit;
                    }
                }
            }
        }
    }
    else
    {   //NOT REG ARRAY
        for (k = 0; k < pair_cnt; k++)
        {
            ret = rtk_diag_regField_set(unit,reg_info.index,
                                        input_fld_idx[k],&input_val[k]);
            if(ret != RT_ERR_OK)
            {
                goto exit;
            }
        }
    }

exit:
    osal_free(pField_info);
    if(ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_mprintf("Not support; Please enable CONFIG_SDK_DUMP_REG_WITH_NAME\n");
#endif
    return CPARSER_OK;
}
#endif

#ifdef CMD_REGISTER_GET_ALL
/*
 * register get all
 */
cparser_result_t cparser_cmd_register_get_all(cparser_context_t *context)
{
    uint32  unit = 0;
    uint32  reg_idx = 0;
    uint32  addr = 0;
    uint32  value = 0;
    uint32  reg_max = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_reg_info_t  reg_data;
    uint32  i, j, field_no_of_word, j_max, array_dim;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(reg_idxMax_get(unit, &reg_max), ret);

    for (reg_idx = 0; reg_idx < reg_max; reg_idx++)
    {
        DIAG_UTIL_ERR_CHK(reg_info_get(unit, reg_idx, &reg_data), ret);
        if (reg_data.offset >= SWCORE_MEM_SIZE)
            return CPARSER_NOT_OK;

        if (reg_data.bit_offset == 0)
        {   /* non register array case */
            DIAG_UTIL_ERR_CHK(ioal_mem32_read(unit, reg_data.offset, &value), ret);
            diag_util_printf("Register 0x%x : 0x%08x\n", reg_data.offset, value);
        }
        else if ((reg_data.bit_offset % 32) == 0)
        {   /* register array case (unit: word) */
            if (reg_data.bit_offset == 32)
            {
                for (i = reg_data.lport; i <= reg_data.hport; i++)
                {
                    for (j = reg_data.larray; j <= reg_data.harray; j++)
                    {
                        if (reg_data.is_PpBlock)
                            addr = reg_data.offset + (i - reg_data.lport) * 0x100 + (j - reg_data.larray) * 0x4;
                        else
                            addr = reg_data.offset + (i - reg_data.lport) * (reg_data.harray - reg_data.larray + 1) * 0x4 + (j - reg_data.larray) * 0x4;
                        DIAG_UTIL_ERR_CHK(ioal_mem32_read(unit, addr, &value), ret);
                        diag_util_printf("Register 0x%x : 0x%08x\n", addr, value);
                    }
                }
            }
            else
            {
                diag_util_printf("Register 0x%x : Unexpected Case\n", reg_data.offset);
            }
        }
        else
        {
            /* register array case (unit: bit) */
            field_no_of_word = 32/reg_data.bit_offset;
            if (reg_data.lport != reg_data.hport)
            {
                if (reg_data.larray != reg_data.harray)
                    array_dim = 2;
                else
                    array_dim = 1;
            }
            else
            {
                array_dim = 0;
            }

            if (array_dim == 2)
            {
                if ((reg_data.harray-reg_data.larray+1) % field_no_of_word)
                    j_max = (reg_data.harray-reg_data.larray+1)/field_no_of_word;
                else
                    j_max = (reg_data.harray-reg_data.larray+1)/field_no_of_word -1;
                for (i = reg_data.lport; i <= reg_data.hport; i++)
                {
                    for (j = 0; j <= j_max; j++)
                    {
                        if (reg_data.is_PpBlock)
                            addr = reg_data.offset + (i - reg_data.lport) * 0x100 + (j) * 0x4;
                        else
                            addr = reg_data.offset + (i - reg_data.lport) * (j_max + 1) * 0x4 + (j) * 0x4;
                        DIAG_UTIL_ERR_CHK(ioal_mem32_read(unit, addr, &value), ret);
                        diag_util_printf("Register 0x%x : 0x%08x\n", addr, value);
                    }
                }
            }
            else if (array_dim == 1)
            {
                if ((reg_data.hport-reg_data.lport+1) % field_no_of_word)
                    j_max = (reg_data.hport-reg_data.lport+1)/field_no_of_word;
                else
                    j_max = (reg_data.hport-reg_data.lport+1)/field_no_of_word -1;

                for (j = 0; j <= j_max; j++)
                {
                    addr = reg_data.offset + (j) * 0x4;
                    DIAG_UTIL_ERR_CHK(ioal_mem32_read(unit, addr, &value), ret);
                    diag_util_printf("Register 0x%x : 0x%08x\n", addr, value);
                }
            }
            else
            {
                diag_util_printf("Register 0x%x : Unexpected Case\n", reg_data.offset);
            }
        }
    }

    return CPARSER_OK;
} /* end of cparser_cmd_register_get_all */
#endif

