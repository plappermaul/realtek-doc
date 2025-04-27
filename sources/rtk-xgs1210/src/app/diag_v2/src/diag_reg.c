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
 * $Revision: 91355 $
 * $Date: 2018-08-13 21:54:45 +0800 (Mon, 13 Aug 2018) $
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
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <hal/chipdef/allreg.h>
#include <hal/mac/reg.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_debug.h>
  #include <rtrpc/rtrpc_diag.h>
#endif

#ifdef CMD_REGISTER_SET_ADDRESS_VALUE
/*
 * register set <UINT:address> <UINT:value>
 */
cparser_result_t cparser_cmd_register_set_address_value(cparser_context_t *context,
    uint32_t *address_ptr, uint32_t *value_ptr)
{
    uint32  unit = 0;
    uint32  reg = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    reg = *address_ptr;
    value = *value_ptr;

    if (0 != (reg % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

    if (reg >= SWCORE_MEM_SIZE)
    {
        DIAG_ERR_PRINT(RT_ERR_OUT_OF_RANGE);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(ioal_mem32_write(unit, reg, value), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_register_set_address_value */
#endif

#ifdef CMD_REGISTER_GET_ADDRESS_WORDS
/*
 * register get <UINT:address> { <UINT:words> }
 */
cparser_result_t cparser_cmd_register_get_address_words(cparser_context_t *context,
    uint32_t *address_ptr, uint32_t *words_ptr)
{
    uint32  unit = 0;
    uint32  reg = 0;
    uint32  value = 0;
    uint32  reg_words = 0;
    uint32  index = 0;
    int32   ret = RT_ERR_FAILED;

    /* Don't check the (NULL == words_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    reg = *address_ptr;
    if (0 != (reg % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

    if (reg >= SWCORE_MEM_SIZE)
    {
        DIAG_ERR_PRINT(RT_ERR_OUT_OF_RANGE);
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
} /* end of cparser_cmd_register_get_address_words */
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

