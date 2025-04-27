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
 * Purpose : Definition those debug command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) debug
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/util/rt_util_serdes.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <hal/mac/mac_debug.h>
#include <hal/mac/mem.h>
#include <rtk/switch.h>
#include <rtk/sds.h>
#include <rtk/flowctrl.h>
#include <dal/dal_phy.h>
#include <dal/dal_linkMon.h>
#ifdef CMD_DEBUG_FLASHTEST_MTD_MTD_IDX
#include <mtd/mtd-user.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <osal/memory.h>
#include <sys/ioctl.h>
#endif
#include <drv/nic/nic.h>
#include <rtk/diag.h>
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
  #include <hal/phy/phydef.h>
  #include <hal/mac/rtl8295.h>
  #include <hal/phy/phy_rtl8295.h>
  #include <hal/phy/phy_rtl8295_patch.h>
#endif
#if (defined CONFIG_SDK_RTL9300)
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/chipdef/longan/rtk_longan_regField_list.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/chipdef/longan/rtk_longan_tableField_list.h>
#endif
#ifdef CMD_DEBUG_GET_VERSION
  #ifdef VCS_REVISION
    #include <common/vcs_rev.h>     /* For RT_VCS_REVISION */
  #endif /* VCS_REVISION */
#endif /* CMD_DEBUG_GET_VERSION */
#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_diag.h>
  #include <rtrpc/rtrpc_sds.h>
  #include <rtrpc/rtrpc_debug.h>
  #include <rtrpc/rtrpc_flowctrl.h>
  #include <rtrpc/rtrpc_port.h>
  #include <rtrpc/rtrpc_util.h>
#endif


const rtk_text_t text_sds_rxcali_status[] =
{
    { /* PHY_SDS_RXCALI_STATUS_NOINFO */ "NoT rx-Calied" },
    { /* PHY_SDS_RXCALI_STATUS_OK */         "OK" },
    { /* PHY_SDS_RXCALI_STATUS_FAILED */  "Failed" },
};

#ifdef CMD_DEBUG_GET_VERSION
/*
 * debug get version
 */
cparser_result_t
cparser_cmd_debug_get_version(cparser_context_t *context)
{
    uint32  unit = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#ifdef VCS_REVISION
    diag_util_mprintf("VCS revision number: %s\n", RT_VCS_REVISION);
#else
    diag_util_mprintf("VCS revision number: Not available\n");
#endif

    return CPARSER_OK;
}
#endif /* CMD_DEBUG_GET_VERSION */


#ifdef CMD_DEBUG_GET_LOG
/*
 * debug get log
 */
cparser_result_t cparser_cmd_debug_get_log(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  data = 0;
    uint64  data64 = 0;
    uint32  i = 0;
    uint32  log_type = LOG_TYPE_DEFAULT;
    int32   ret = RT_ERR_FAILED;

    char *pLevelName[] = {
        "fatal", "major", "minor", "warning", "event", "info",
        "func", "debug", "trace", ""
    };

    uint8 *pModName[] =
    {
        RT_LOG_MODULE_STRING
    };

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(rt_log_enable_get(&data), ret);
    if (data < RTK_ENABLE_END)
        diag_util_printf("    status      : %s \n", data ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    else
        diag_util_printf("    status      : ERROR \n");

    DIAG_UTIL_ERR_CHK(rt_log_type_get(&log_type), ret);
    if (log_type < LOG_TYPE_END)
        diag_util_printf("    type        : %s \n", log_type ? "LEVEL-MASK" : "LEVEL");
    else
        diag_util_printf("    type        : ERROR \n");

    data = 0;
    DIAG_UTIL_ERR_CHK(rt_log_level_get(&data), ret);
    if (data < LOG_LV_END)
    {
        if (LOG_MSG_OFF == data)
        {
            diag_util_printf("    level       : Message off ");
        }
        else
        {
            diag_util_printf("    level       : %u ", data);
        }
        if (LOG_TYPE_LEVEL == log_type)
            diag_util_printf("(*)");
        diag_util_printf("\n");
    }
    else
        diag_util_printf("    level       : ERROR \n");

    data = 0;
    DIAG_UTIL_ERR_CHK(rt_log_mask_get(&data), ret);
    if (data <= LOG_MASK_ALL)
    {
        diag_util_printf("    level-mask  : ");
        if (data)
        {
            for (i = 0; i < LOG_MSG_OFF; i++)
            {
                if ((data >> i) & 0x1)
                    diag_util_printf("%s ", *(pLevelName + i));
            }
        }
        else
            diag_util_printf("ALL_MSG_OFF");

        if (LOG_TYPE_MASK == log_type)
            diag_util_printf("(*)");
        diag_util_printf("\n");
    }
    else
        diag_util_printf("    level-mask  : ERROR \n");

    data = 0;
    DIAG_UTIL_ERR_CHK(rt_log_format_get(&data), ret);
    if (data < LOG_FORMAT_END)
        diag_util_printf("    format      : %s \n", data ? "DETAILED" : "NORMAL");
    else
        diag_util_printf("    format      : ERROR \n");

    data64 = 0;
    DIAG_UTIL_ERR_CHK(rt_log_moduleMask_get(&data64), ret);

        diag_util_printf("    module-mask : ");
        if (data64)
        {
            for (i = 0; i < SDK_MOD_END; i++)
            {
                if ((data64 >> i) & 0x1)
                    diag_util_printf("%s ", *(pModName + i));
            }
        }
        else
            diag_util_printf("ALL_MODULE_OFF");
        diag_util_printf("\n\n");

#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_get_log */
#endif

#ifdef CMD_DEBUG_GET_MEMORY_ADDRESS_WORDS
/*
 * debug get memory <UINT:address> { <UINT:words> }
 */
cparser_result_t cparser_cmd_debug_get_memory_address_words(cparser_context_t *context,
    uint32_t *address_ptr, uint32_t *words_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  mem = 0;
    uint32  value = 0;
    uint32  mem_words = 0;
    uint32  index = 0;
    int32   ret = RT_ERR_FAILED;

    /* Don't check the (NULL == words_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    mem = *address_ptr;
    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

    if ('\0' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(debug_mem_read(unit, mem, &value), ret);
        diag_util_mprintf("Memory 0x%x : 0x%08x\n", mem, value);
    }
    else
    {
        mem_words = *words_ptr;
        for (index = 0; index < mem_words; index++)
        {
            DIAG_UTIL_ERR_CHK(debug_mem_read(unit, mem, &value), ret);
            if (0 == (index % 4))
            {
                diag_util_mprintf("\n");
                diag_util_printf("0x%08x ", mem);
            }
            diag_util_printf("0x%08x ", value);
            mem = mem + 4;
        }
        diag_util_mprintf("\n");
    }

#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_get_memory_address_words */
#endif

#ifdef CMD_DEBUG_SET_LOG_STATE_DISABLE_ENABLE
/*
 * debug set log state ( disable | enable )
 */
cparser_result_t cparser_cmd_debug_set_log_state_disable_enable(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(rt_log_enable_set(ENABLED), ret);
    }
    else if ('d' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(rt_log_enable_set(DISABLED), ret);
    }
    else {}
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_state_disable_enable */
#endif

#ifdef CMD_DEBUG_SET_LOG_LEVEL_VALUE
/*
 * debug set log level <UINT:value>
 */
cparser_result_t cparser_cmd_debug_set_log_level_value(cparser_context_t *context,
    uint32_t *value_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  log_level = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    log_level = *value_ptr;
    DIAG_UTIL_ERR_CHK(rt_log_level_set(log_level), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_level_value */
#endif

#ifdef CMD_DEBUG_SET_LOG_LEVEL_MASK_BITMASK
/*
 * debug set log level-mask <UINT:bitmask>
 */
cparser_result_t cparser_cmd_debug_set_log_level_mask_bitmask(cparser_context_t *context,
    uint32_t *bitmask_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  log_level_mask = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    log_level_mask = *bitmask_ptr;
    DIAG_UTIL_ERR_CHK(rt_log_mask_set(log_level_mask), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_level_mask_bitmask */
#endif

#ifdef CMD_DEBUG_SET_LOG_LEVEL_TYPE_LEVEL_LEVEL_MASK
/*
 * debug set log level-type ( level | level-mask )
 */
cparser_result_t cparser_cmd_debug_set_log_level_type_level_level_mask(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (strlen(TOKEN_STR(4)) == strlen("level"))
    {
        DIAG_UTIL_ERR_CHK(rt_log_type_set(LOG_TYPE_LEVEL), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rt_log_type_set(LOG_TYPE_MASK), ret);
    }
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_level_type_level_level_mask */
#endif

#ifdef CMD_DEBUG_SET_LOG_FORMAT_NORMAL_DETAIL
/*
 * debug set log format ( normal | detail )
 */
cparser_result_t cparser_cmd_debug_set_log_format_normal_detail(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(rt_log_format_set(LOG_FORMAT_DETAILED), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rt_log_format_set(LOG_FORMAT_NORMAL), ret);
    }
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_format_normal_detail */
#endif

#ifdef CMD_DEBUG_SET_LOG_MODULE_BITMASK
/*
 * debug set log module <UINT64:bitmask>
 */
cparser_result_t cparser_cmd_debug_set_log_module_bitmask(cparser_context_t *context,
    uint64_t *bitmask_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint64  log_module_mask = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    log_module_mask = *bitmask_ptr;
    DIAG_UTIL_ERR_CHK(rt_log_moduleMask_set(log_module_mask), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_module_bitmask */
#endif

#ifdef CMD_DEBUG_SET_MEMORY_ADDRESS_VALUE
/*
 * debug set memory <UINT:address> <UINT:value>
 */
cparser_result_t cparser_cmd_debug_set_memory_address_value(cparser_context_t *context,
    uint32_t *address_ptr, uint32_t *value_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  mem = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    mem = *address_ptr;
    value = *value_ptr;

    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(debug_mem_write(unit, mem, value), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_memory_address_value */
#endif

#ifdef CMD_DEBUG_DUMP_HSA
/*
 * debug dump hsa
 */
cparser_result_t cparser_cmd_debug_dump_hsa(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(hal_dumpHsa(unit), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_hsa */
#endif

#ifdef CMD_DEBUG_DUMP_HSA_OPENFLOW
/*
 * debug dump hsa openflow
 */
cparser_result_t cparser_cmd_debug_dump_hsa_openflow(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(hal_dumpHsa_openflow(unit), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_hsa_openflow */
#endif

#ifdef CMD_DEBUG_DUMP_HSB
/*
 * debug dump hsb
 */
cparser_result_t cparser_cmd_debug_dump_hsb(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(hal_dumpHsb(unit), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_hsb */
#endif

#ifdef CMD_DEBUG_DUMP_HSB_OPENFLOW
/*
 * debug dump hsb openflow
 */
cparser_result_t cparser_cmd_debug_dump_hsb_openflow(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(hal_dumpHsb_openflow(unit), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_hsb_openflow */
#endif

#ifdef CMD_DEBUG_DUMP_HSM
/*
 * debug dump hsm
 */
cparser_result_t cparser_cmd_debug_dump_hsm(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(hal_dumpHsm(unit), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_hsm */
#endif

#ifdef CMD_DEBUG_DUMP_HSM_OPENFLOW
/*
 * debug dump hsm openflow
 */
cparser_result_t cparser_cmd_debug_dump_hsm_openflow(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(hal_dumpHsm_openflow(unit), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_hsm_openflow */
#endif

#ifdef CMD_DEBUG_DUMP_PMI
/*
 * debug dump pmi
 */
cparser_result_t cparser_cmd_debug_dump_pmi(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(hal_dumpPmi(unit), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_pmi */
#endif


#ifdef CMD_DEBUG_DUMP_HSM_INDEX
/*
 * debug dump hsm <UINT:index>
 */
cparser_result_t cparser_cmd_debug_dump_hsm_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if(*index_ptr > 3)
    {
        return CPARSER_NOT_OK;
    }

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(hal_dumpHsmIdx(unit, *index_ptr), ret);
    }
#endif

#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_ppi_index */
#endif


#ifdef CMD_DEBUG_DUMP_PPI_INDEX
/*
 * debug dump ppi <UINT:index>
 */
cparser_result_t cparser_cmd_debug_dump_ppi_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if(*index_ptr > 6)
    {
        return CPARSER_NOT_OK;
    }
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_ppi_index */
#endif

#ifdef CMD_DEBUG_GET_TABLE_TABLE_IDX_ADDRESS
/*
 * debug get table <UINT:table_idx> <UINT:address>
 */
cparser_result_t cparser_cmd_debug_get_table_table_idx_address(cparser_context_t *context,
    uint32_t *table_idx_ptr,
    uint32_t *address_ptr)

{
    uint32      unit = 0;
    uint32      loop;
    int32       ret = RT_ERR_FAILED;
    uint32      value[20];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ((ret = table_read(unit, *table_idx_ptr, *address_ptr, value)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Table %u, address %u\n", *table_idx_ptr, *address_ptr);

#if defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        uint32  table_size;

        switch(*table_idx_ptr)
        {
            case 0:
              table_size = 18;  /*ACL*/
              break;
            case 11:
            case 13:
            case 14:
            case 16:
              table_size = 2;
              break;
            case 12:
            case 15:
              table_size = 1;
              break;
            case 17:
              table_size = 6;
              break;
            default:
              table_size = 3;  /*L2*/
              break;
        }

        for (loop = 0; loop < table_size; loop++)
        {
            diag_util_printf("%x", value[loop]);
            if(loop < table_size-1)
                diag_util_printf("-");
        }

        diag_util_mprintf("\n\n");

        if(0 == *table_idx_ptr)
        {
            for (loop = 0; loop < 7; loop++)
            {
                if(6 != loop)
                    diag_util_printf("Field %d-%d:\t", 11-loop*2, 10-loop*2);
                else
                    diag_util_printf("Fixed Field:\t");

                diag_util_printf("data 0x%08x  ", value[loop]);
                diag_util_printf("mask 0x%08x", value[loop+7]);
                diag_util_mprintf("\n");

            }

            diag_util_mprintf("\n");
            diag_util_printf("valid:%d\t\tnot:%d\t\tand1:%d\t\tand2:%d\n",
                            (value[14]&0x80000000) >> 31,
                            (value[14]&0x40000000) >> 30,
                            (value[14]&0x20000000) >> 29,
                            (value[14]&0x10000000) >> 28);
            diag_util_printf("shap:%d\t\titpid:%d\t\totpid:%d\t\tcpu_pri:%d\tnor_pri:%d\n",
                            (value[17]&0x1) >> 0,
                            (value[17]&0x2) >> 1,
                            (value[17]&0x4) >> 2,
                            (value[17]&0x8) >> 3,
                            (value[17]&0x10) >> 4);
            diag_util_printf("mir:%d\t\ttagst:%d\t\tmeter:%d\t\trmk:%d\t\tlog:%d\n",
                            (value[17]&0x20) >> 5,
                            (value[17]&0x40) >> 6,
                            (value[17]&0x80) >> 7,
                            (value[17]&0x100) >> 8,
                            (value[17]&0x200) >> 9);
            diag_util_printf("flt:%d\t\tivid:%d\t\tovid:%d\t\tfwd:%d\t\tdrop:%d\n",
                            (value[17]&0x400) >> 10,
                            (value[17]&0x800) >> 11,
                            (value[17]&0x1000) >> 12,
                            (value[17]&0x2000) >> 13,
                            (value[17]&0xc000) >> 14);
            diag_util_printf("AIF0:0x%4x\tAIF1:0x%4x\tAIF2:0x%4x\tAIF3:0x%4x\tAIF4:0x%4x\n",
                            (value[16]&0xffff) >> 0,
                            (value[16]&0x3fff0000) >> 16,
                            (value[15]&0x3fff) >> 0,
                            (value[15]&0x3ff0000) >> 16,
                            (value[14]&0x1ff) >> 0);
        }

        diag_util_mprintf("\n");
    }
#endif
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        for (loop = 0; loop < 20; loop++)
        {
            diag_util_printf("%x-", value[loop]);
        }

        diag_util_mprintf("\n");
    }
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        uint32  table_size = 0;

        switch(*table_idx_ptr)
        {
            case LONGAN_VACLt:
            case LONGAN_IACLt:
              table_size = 19;
              break;
            default:
              break;
        }

        for (loop = 0; loop < table_size; loop++)
        {
            diag_util_printf("%x", value[loop]);
            if(loop < table_size-1)
                diag_util_printf("-");
        }

        diag_util_mprintf("\n\n\n");
        if(LONGAN_VACLt == *table_idx_ptr || LONGAN_IACLt == *table_idx_ptr)
        {
            diag_util_printf("Field 11-10: data 0x%8x   mask 0x%8x\t\tField 9-8: data 0x%8x  mask 0x%8x\n",
                value[0], ((value[6]&0xff)<<24 | value[7]>>8), value[1], ((value[7]&0xff)<<24 | value[8]>>8));
            diag_util_printf("Field 7-6:   data 0x%8x   mask 0x%8x\t\tField 5-4: data 0x%8x  mask 0x%8x\n",
                value[2], ((value[8]&0xff)<<24 | value[9]>>8), value[3], ((value[9]&0xff)<<24 | value[10]>>8));
            diag_util_printf("Field 3-2:   data 0x%8x   mask 0x%8x\t\tField 1-0: data 0x%8x  mask 0x%8x\n",
                value[4], ((value[10]&0xff)<<24 | value[11]>>8), value[5], ((value[11]&0xff)<<24 | value[12]>>8));
            diag_util_printf("Fixed Field: data 0x%8x   mask 0x%8x\n",
                value[6]>>8, ((value[12]&0xff)<<16 | value[13]>>16));

            diag_util_mprintf("\n");
            diag_util_printf("valid:%d\t\t\tnot:%d\t\tand1:%d\t\tand2:%d\n",
                            (value[13]&0x8000) >> 15,
                            (value[13]&0x4000) >> 14,
                            (value[13]&0x2000) >> 13,
                            (value[13]&0x1000) >> 12);

            diag_util_printf("invert_ip_rsvd:%d\tred_rmk:%d\tyellow_rmk:%d\trmk:%d\t\tqid:%d\n",
                            (value[13]&0x800) >> 11,
                            (value[13]&0x400) >> 10,
                            (value[13]&0x200) >> 9,
                            (value[13]&0x100) >> 8,
                            (value[13]&0x80) >> 7);
            diag_util_printf("metadata:%d\tbypass:%d\tpriority:%d\ttagsts:%d\topri:%d\n",
                            (value[13]&0x40) >> 6,
                            (value[13]&0x20) >> 5,
                            (value[13]&0x10) >> 4,
                            (value[13]&0x8) >> 3,
                            (value[13]&0x4) >> 2);
            diag_util_printf("ovlan:%d\t\t\t\tipri:%d\t\tivlan:%d\t\tmeter:%d\t\tmir:%d\n",
                            (value[13]&0x2) >> 1,
                            (value[13]&0x1),
                            (value[14]&0x80000000) >> 31,
                            (value[14]&0x40000000) >> 30,
                            (value[14]&0x20000000) >> 29);
            diag_util_printf("log:%d\t\t\tfwd:%d\t\tred_drop:%d\tyellow_drop:%d\tdrop:%d\n\n",
                            (value[14]&0x10000000) >> 28,
                            (value[14]&0x8000000) >> 27,
                            (value[14]&0x4000000) >> 26,
                            (value[14]&0x2000000) >> 25,
                            (value[14]&0x1000000) >> 24);

            diag_util_printf("drop:%d\t\t\tyellow_drop:%d\t\tred_drop:%d\tfwd_act:%d\n",
                            (value[14]&0x800000) >> 23,
                            (value[14]&0x400000) >> 22,
                            (value[14]&0x200000) >> 21,
                            (value[14]&0x1c0000) >> 18);

            diag_util_printf("cpu_fmt:%d\t\tsa_lrn:%d\t\tfwd_sel:%d\tfwd_port_info:0x%4x\n",
                            (value[14]&0x30000) >> 16,
                            (value[14]&0x8000) >> 15,
                            (value[14]&0x4000) >> 14,
                            (value[14]&0x3ff8) >> 3);

            diag_util_printf("log:%d\t\t\tmeter_idx:%3d\t\tivid_act:%d\tivid:0x%4x\n",
                            (value[15]&0x80000000) >> 31,
                            (value[15]&0x7f800000) >> 23,
                            (value[15]&0x600000) >> 21,
                            (value[15]&0x1ffe00) >> 9);

            diag_util_printf("ipri_act:%d\t\tipri:%d\t\t\tovid_act:%d\tovid:0x%4x\n",
                            (value[15]&18) >> 3,
                            (value[15]&0x7),
                            (value[16]&0xc0000000) >> 30,
                            (value[16]&0x3ffc0000) >> 18);

            diag_util_printf("opri_act:%d\t\topri:%d\t\t\tmir:%d\t\tmir_idx:%d\n",
                            (value[16]&0x6000) >> 13,
                            (value[16]&0x1c00) >> 10,
                            (value[16]&0x200) >> 9,
                            (value[16]&0xc0) >> 6);

            diag_util_printf("tagsts_inner:%d\t\ttagsts_outer:%d\t\tint_pri:%d\tqid:%d\n",
                            (value[17]&0xc0000000) >> 30,
                            (value[17]&0x30000000) >> 28,
                            (value[17]&0xe000000) >> 25,
                            (value[17]&0x1f00000) >> 20);

            diag_util_printf("bypass:0x%4x\t\tmetadata:0x%4x\t\trmk_act:%d\trmk_val:0x%4x\n",
                            (value[17]&0x70000) >> 16,
                            (value[17]&0xff00) >> 8,
                            (value[18]&0xc0000000) >> 30,
                            (value[18]&0x3fc00000) >> 22);

            diag_util_printf("yellow_rmk_act:%d\tyellow_rmk_val:0x%4x\tred_rmk_act:%d\tred_rmk_val:0x%4x\n",
                            (value[18]&0x300000) >> 20,
                            (value[18]&0xff000) >> 12,
                            (value[18]&0xc00) >> 10,
                            (value[18]&0x3fc) >> 2);

        }

        diag_util_mprintf("\n");
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_GET_SW_WD
/*
 * * debug get sw_wd
 */
cparser_result_t cparser_cmd_debug_get_sw_wd(cparser_context_t *context)
{
    uint32      value;
    uint32      unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    hal_getWatchdogCnt(unit, 0, &value);
    diag_util_printf("PktBuf         WatchDog CNT = %u\n", value);
    hal_getWatchdogCnt(unit, 1, &value);
    diag_util_printf("SerDes         WatchDog CNT = %u\n", value);
    hal_getWatchdogCnt(unit, 2, &value);
    diag_util_printf("PHY            WatchDog CNT = %u\n", value);
    hal_getWatchdogCnt(unit, 3, &value);
    diag_util_printf("Fiber Rx       WatchDog CNT = %u\n", value);
    hal_getWatchdogCnt(unit, 4, &value);
    diag_util_printf("PHY Serdes     WatchDog CNT = %u\n", value);
    hal_getWatchdogCnt(unit, 5, &value);
    diag_util_printf("95R CLK        WatchDog CNT = %u\n", value);
    hal_getWatchdogCnt(unit, 6, &value);
    diag_util_printf("95R CLK OK     WatchDog CNT = %u\n", value);
    hal_getWatchdogCnt(unit, 7, &value);
    diag_util_printf("DrainTX        WatchDog CNT = %u\n", value);
    hal_getWatchdogCnt(unit, 8, &value);
    diag_util_printf("  TXHasPkt     WatchDog CNT = %u\n", value);
    hal_getWatchdogCnt(unit, 9, &value);
    diag_util_printf("    TXQHasPkt  WatchDog CNT = %u\n", value);
    hal_getWatchdogCnt(unit, 10, &value);
    diag_util_printf("    SyncPhyERR WatchDog CNT = %u\n", value);
    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_SET_SW_WD_STATE_DISABLE_ENABLE
/*
 * debug set sw_wd state ( disable | enable )
 */
cparser_result_t cparser_cmd_debug_set_sw_wd_state_disable_enable(cparser_context_t *context)
{
    int32   ret;
    uint32  state;

    if ('e' == TOKEN_CHAR(4,0))
    {
        state = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(4,0))
    {
        state = DISABLED;
    }
    else
    {
        diag_util_printf("Wrong input: %s\n", TOKEN_STR(4));
        return CPARSER_OK;
    }

    DIAG_UTIL_ERR_CHK(hal_setWatchdogMonitorEnable(state), ret);
    return CPARSER_OK;

}
#endif


#ifdef CMD_DEBUG_GET_CHIP
cparser_result_t cparser_cmd_debug_get_chip(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t    devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);


    diag_util_mprintf("Chip ID   : %x\n", RT_CHIP_ID_DISPLAY(devInfo.chipId));
    diag_util_mprintf("Family ID : %x\n", RT_CHIP_ID_DISPLAY(devInfo.familyId));

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_DUMP_MIB_COUNTER_DEBUG_INDEX
/*
 * debug dump mib counter debug <UINT:index>
 */
cparser_result_t cparser_cmd_debug_dump_mib_counter_debug_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0, cntr = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_getDbgCntr(unit, *index_ptr, &cntr), ret);

#if defined(CONFIG_SDK_RTL8390)
    switch (*index_ptr)
    {
        case RTL8390_DBG_MIB_ALE_TX_GOOD_PKTS:
            diag_util_mprintf("Debug Counter %d - The number of ALE TX good packets : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_ERROR_PKTS:
            diag_util_mprintf("Debug Counter %d - Error packet : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_EGR_ACL_DROP:
            diag_util_mprintf("Debug Counter %d - Egress ACL drop : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_EGR_METER_DROP:
            diag_util_mprintf("Debug Counter %d - Egress meter drop : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_OAM:
            diag_util_mprintf("Debug Counter %d - OAM : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_CFM:
            diag_util_mprintf("Debug Counter %d - CFM : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_VLAN_IGR_FLTR:
            diag_util_mprintf("Debug Counter %d - VLAN ingress filter : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_VLAN_ERR:
            diag_util_mprintf("Debug Counter %d - VLAN Error(VID=4095 or MBR=0) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_INNER_OUTER_CFI_EQUAL_1:
            diag_util_mprintf("Debug Counter %d - inner/outer CFI=1 : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_VLAN_TAG_FORMAT:
            diag_util_mprintf("Debug Counter %d - VLAN tag format : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_SRC_PORT_SPENDING_TREE:
            diag_util_mprintf("Debug Counter %d - Source port spending tree : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_INBW:
            diag_util_mprintf("Debug Counter %d - Input bandwidth : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_RMA:
            diag_util_mprintf("Debug Counter %d - RMA : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_HW_ATTACK_PREVENTION:
            diag_util_mprintf("Debug Counter %d - hardware attack prevention : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_PROTO_STORM:
            diag_util_mprintf("Debug Counter %d - protocol storm : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_MCAST_SA:
            diag_util_mprintf("Debug Counter %d - multicast SA : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_IGR_ACL_DROP:
            diag_util_mprintf("Debug Counter %d - Ingress ACL drop : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_IGR_METER_DROP:
            diag_util_mprintf("Debug Counter %d - Ingress Meter drop : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_DFLT_ACTION_FOR_MISS_ACL_AND_C2SC:
            diag_util_mprintf("Debug Counter %d - Default action for miss ACL &C2SC : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_NEW_SA:
            diag_util_mprintf("Debug Counter %d - New SA : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_PORT_MOVE:
            diag_util_mprintf("Debug Counter %d - Port move : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_SA_BLOCKING:
            diag_util_mprintf("Debug Counter %d - SA blocking : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_ROUTING_EXCEPTION:
            diag_util_mprintf("Debug Counter %d - routing exception : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_SRC_PORT_SPENDING_TREE_NON_FWDING:
            diag_util_mprintf("Debug Counter %d - Source port spending tree(non forwarding) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_MAC_LIMIT:
            diag_util_mprintf("Debug Counter %d - MAC limit : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_UNKNOW_STORM:
            diag_util_mprintf("Debug Counter %d - Unknow storm : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_MISS_DROP:
            diag_util_mprintf("Debug Counter %d - Miss Drop : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_CPU_MAC_DROP:
            diag_util_mprintf("Debug Counter %d - CPU MAC drop : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_DA_BLOCKING:
            diag_util_mprintf("Debug Counter %d - DA blocking : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_SRC_PORT_FILTER_BEFORE_EGR_ACL:
            diag_util_mprintf("Debug Counter %d (Egress) - Source port filter(before Egress ACL) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_VLAN_EGR_FILTER:
            diag_util_mprintf("Debug Counter %d (Egress) - VLAN egress filter : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_SPANNING_TRE:
            diag_util_mprintf("Debug Counter %d (Egress) - Spanning tree : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_PORT_ISOLATION:
            diag_util_mprintf("Debug Counter %d (Egress) - Port isolation : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_OAM_EGRESS_DROP:
            diag_util_mprintf("Debug Counter %d (Egress) - OAM : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_MIRROR_ISOLATION:
            diag_util_mprintf("Debug Counter %d (Egress) - Mirror isolation : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_MAX_LEN_BEFORE_EGR_ACL:
            diag_util_mprintf("Debug Counter %d (Egress) - Max length(before Egress ACL) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_SRC_PORT_FILTER_BEFORE_MIRROR:
            diag_util_mprintf("Debug Counter %d (Egress) - Source port filter(before Mirror) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_MAX_LEN_BEFORE_MIRROR:
            diag_util_mprintf("Debug Counter %d (Egress) - Max length(before Mirror) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_SPECIAL_CONGEST_BEFORE_MIRROR:
            diag_util_mprintf("Debug Counter %d (Egress) - Special congest(before Mirror) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_LINK_STATUS_BEFORE_MIRROR:
            diag_util_mprintf("Debug Counter %d (Egress) - Link status(before Mirror) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_WRED_BEFORE_MIRROR:
            diag_util_mprintf("Debug Counter %d (Egress) - WRED(before Mirror) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_MAX_LEN_AFTER_MIRROR:
            diag_util_mprintf("Debug Counter %d (Egress) - Max length(after Mirror) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_SPECIAL_CONGEST_AFTER_MIRROR:
            diag_util_mprintf("Debug Counter %d (Egress) - Special congest(after Mirror) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_LINK_STATUS_AFTER_MIRROR:
            diag_util_mprintf("Debug Counter %d (Egress) - Link status(after Mirror) : %u \n", *index_ptr, cntr);
            break;
        case RTL8390_DBG_MIB_WRED_AFTER_MIRROR:
            diag_util_mprintf("Debug Counter %d (Egress) - WRED(after Mirror) : %u \n", *index_ptr, cntr);
            break;
        default:
            diag_util_printf("User config: Error!\n");
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    switch (*index_ptr)
    {
        case RTL9310_DBG_CNTR_ALE_RX_GOOD_PKTS:
            diag_util_mprintf("Debug Counter %2d - The nummber of ALE receive packet : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_RX_MAX_FRAME_SIZE:
            diag_util_mprintf("Debug Counter %2d - Reserved\n", *index_ptr);
            break;
        case RTL9310_DBG_CNTR_MAC_RX_DROP:
            diag_util_mprintf("Debug Counter %2d - MAC Rx Drop : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_OPENFLOW_IP_MPLS_TTL:
            diag_util_mprintf("Debug Counter %2d - OpenFlow IP/MPLS TTL : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_OPENFLOW_TBL_MISS:
            diag_util_mprintf("Debug Counter %2d - OpenFlow Table Miss : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IGR_BW:
            diag_util_mprintf("Debug Counter %2d - Ingress Bandwidth (except Backdoor, IACL, EACL Bypass) : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_SPECIAL_CONGEST:
            diag_util_mprintf("Debug Counter %2d - Special Congest : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_EGR_QUEUE:
            diag_util_mprintf("Debug Counter %2d - Egress Queue/S-WRED : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_RESERVED:
            diag_util_mprintf("Debug Counter %2d - Reserved\n", *index_ptr);
            break;
        case RTL9310_DBG_CNTR_EGR_LINK_STATUS:
            diag_util_mprintf("Debug Counter %2d - Egress Link Status : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_STACK_UCAST_NONUCAST_TTL:
            diag_util_mprintf("Debug Counter %2d - Reserved\n", *index_ptr);
            break;
        case RTL9310_DBG_CNTR_STACK_NONUC_BLOCKING_PMSK:
            diag_util_mprintf("Debug Counter %2d - Stacking NonUnicast Blocking PortMask : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_L2_CRC:
            diag_util_mprintf("Debug Counter %2d - L2 CRC : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_SRC_PORT_FILTER:
            diag_util_mprintf("Debug Counter %2d - Source Port Filter or MyUnit Drop : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_PARSER_PACKET_TOO_LONG:
            diag_util_mprintf("Debug Counter %2d - Parser Packet Too Long : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_PARSER_MALFORM_PACKET:
            diag_util_mprintf("Debug Counter %2d - Parser Malform Packet : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_MPLS_OVER_2_LBL:
            diag_util_mprintf("Debug Counter %2d - MPLS Over 2 Label : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_EACL_METER:
            diag_util_mprintf("Debug Counter %2d - IACL Meter (learn) : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IACL_METER:
            diag_util_mprintf("Debug Counter %2d - VACL Meter : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_PROTO_STORM:
            diag_util_mprintf("Debug Counter %2d - Protocol Storm : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_INVALID_CAPWAP_HEADER:
            diag_util_mprintf("Debug Counter %2d - Invalid CAPWAP Header or BPE Filter : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_MAC_IP_SUBNET_BASED_VLAN:
            diag_util_mprintf("Debug Counter %2d - MAC/IP-Subnet Based VLAN : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_OAM_PARSER:
            diag_util_mprintf("Debug Counter %2d - OAM Parser : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_UC_MC_RPF:
            diag_util_mprintf("Debug Counter %2d - Unicast/Multicast RPF : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IP_MAC_BINDING_MATCH_MISMATCH:
            diag_util_mprintf("Debug Counter %2d - IP-MAC Binding Match/Mismatch : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_SA_BLOCK:
            diag_util_mprintf("Debug Counter %2d - SA Block : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_TUNNEL_IP_ADDRESS_CHECK:
            diag_util_mprintf("Debug Counter %2d - Tunnel IP Address Check : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_EACL_DROP:
            diag_util_mprintf("Debug Counter %2d - IACL Drop (learn) : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IACL_DROP:
            diag_util_mprintf("Debug Counter %2d - VACL Drop : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_ATTACK_PREVENT:
            diag_util_mprintf("Debug Counter %2d - Attack Prevent : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_SYSTEM_PORT_LIMIT_LEARN:
            diag_util_mprintf("Debug Counter %2d - System/Port-based MAC Limit : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_OAMPDU:
            diag_util_mprintf("Debug Counter %2d - OAMPDU (OAM disabled, option is drop) : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_CCM_RX:
            diag_util_mprintf("Debug Counter %2d - CCM RX : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_CFM_UNKNOWN_TYPE:
            diag_util_mprintf("Debug Counter %2d - CFM Unknown Type : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_LBM_LBR_LTM_LTR:
            diag_util_mprintf("Debug Counter %2d - LBM/LBR, LTM/LTR : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_Y_1731:
            diag_util_mprintf("Debug Counter %2d - Y.1731 (ETHDM) : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_VLAN_LIMIT_LEARN:
            diag_util_mprintf("Debug Counter %2d - VLAN Limit Learn : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_VLAN_ACCEPT_FRAME_TYPE:
            diag_util_mprintf("Debug Counter %2d - VLAN Accept Frame Type : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_CFI_1:
            diag_util_mprintf("Debug Counter %2d - VLAN CFI=1 : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_STATIC_DYNAMIC_PORT_MOVING:
            diag_util_mprintf("Debug Counter %2d - Static/Dynamic Port Moving : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_PORT_MOVE_FORBID:
            diag_util_mprintf("Debug Counter %2d - Port Move Forbidden : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_L3_CRC:
            diag_util_mprintf("Debug Counter %2d - IP Checksum Error : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_BPDU_PTP_LLDP_EAPOL_RMA:
            diag_util_mprintf("Debug Counter %2d - BPDU/PTP/LLDP/EAPOL/RMA : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_MSTP_SRC_DROP_DISABLED_BLOCKING:
            diag_util_mprintf("Debug Counter %2d - Ingress Spanning Tree Filter (Disabled/Blocking State) : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_INVALID_SA:
            diag_util_mprintf("Debug Counter %2d - Invalid SA (Zero, Bcast, Mcast) : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_NEW_SA:
            diag_util_mprintf("Debug Counter %2d - New SA : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_VLAN_IGR_FILTER:
            diag_util_mprintf("Debug Counter %2d - VLAN Ingress Filter : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IGR_VLAN_CONVERT:
            diag_util_mprintf("Debug Counter %2d - Reserved\n", *index_ptr);
            break;
        case RTL9310_DBG_CNTR_GRATUITOUS_ARP:
            diag_util_mprintf("Debug Counter %2d - Gratuitous ARP : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_MSTP_SRC_DROP:
            diag_util_mprintf("Debug Counter %2d - Ingress Spanning Tree Filter (Learning State) : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_L2_HASH_FULL:
            diag_util_mprintf("Debug Counter %2d - L2 Hash Full : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_MPLS_UNKNOWN_LBL:
            diag_util_mprintf("Debug Counter %2d - MPLS Unknown Label : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_L3_IPUC_NON_IP:
            diag_util_mprintf("Debug Counter %2d - My Packet Drop : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_TTL:
            diag_util_mprintf("Debug Counter %2d - TTL : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_MTU:
            diag_util_mprintf("Debug Counter %2d - MTU/OIL RPF : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_ICMP_REDIRECT:
            diag_util_mprintf("Debug Counter %2d - ICMP Redirect : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_STORM_CONTROL:
            diag_util_mprintf("Debug Counter %2d - Storm Control : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_L3_DIP_DMAC_MISMATCH:
            diag_util_mprintf("Debug Counter %2d - IPMC DIP/DMAC Mismatch : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IP4_IP_OPTION:
            diag_util_mprintf("Debug Counter %2d - IPv4 IP Option/IPv6 Hop-by-Hop Extension Header Existed : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IP6_HBH_EXT_HEADER:
            diag_util_mprintf("Debug Counter %2d - Reserved IPv4/IPv6 Multicast : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IP4_IP6_HEADER_ERROR:
            diag_util_mprintf("Debug Counter %2d - IPv4/IPv6 Header Error : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_ROUTING_IP_ADDR_CHECK:
            diag_util_mprintf("Debug Counter %2d - Routing IP Addr Check : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_ROUTING_EXCEPTION:
            diag_util_mprintf("Debug Counter %2d - Routing Exception : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_DA_BLOCK:
            diag_util_mprintf("Debug Counter %2d - DA Block : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_OAM_MUX:
            diag_util_mprintf("Debug Counter %2d - OAM Multiplexer Drop : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_PORT_ISOLATION:
            diag_util_mprintf("Debug Counter %2d - Port Isolation : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_VLAN_EGR_FILTER:
            diag_util_mprintf("Debug Counter %2d - VLAN Egress Filter : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_MIRROR_ISOLATE:
            diag_util_mprintf("Debug Counter %2d - Mirror Isolation : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_MSTP_DESTINATION_DROP:
            diag_util_mprintf("Debug Counter %2d - Egress Spanning Tree Filter (Disabled/Blocking/Learning State) : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_L2_MC_BRIDGE:
            diag_util_mprintf("Debug Counter %2d - L2 Multicast Bridge : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IP_UC_MC_ROUTING_LOOK_UP_MISS:
            diag_util_mprintf("Debug Counter %2d - IP Unicast/Multicast Routing Look-up Miss : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_L2_UC:
            diag_util_mprintf("Debug Counter %2d - Unknown L2 Unicast : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_L2_MC:
            diag_util_mprintf("Debug Counter %2d - Unknown L2 Multicast : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IP4_MC:
            diag_util_mprintf("Debug Counter %2d - Unknown IPv4 Multicast : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_IP6_MC:
            diag_util_mprintf("Debug Counter %2d - Unknown IPv6 Multicast : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_L3_UC_MC_ROUTE:
            diag_util_mprintf("Debug Counter %2d - L3 Unicast/Multicast Route : %u \n", *index_ptr, cntr);
            break;
        case RTL9310_DBG_CNTR_UNKNOWN_L2_UC_FLPM:
            diag_util_mprintf("Debug Counter %2d - Reserved\n", *index_ptr);
            break;
        case RTL9310_DBG_CNTR_BC_FLPM:
            diag_util_mprintf("Debug Counter %2d - Reserved\n", *index_ptr);
            break;
        case RTL9310_DBG_CNTR_VLAN_PRO_UNKNOWN_L2_MC_FLPM:
            diag_util_mprintf("Debug Counter %2d - Reserved\n", *index_ptr);
            break;
        case RTL9310_DBG_CNTR_VLAN_PRO_UNKNOWN_IP4_MC_FLPM:
            diag_util_mprintf("Debug Counter %2d - Reserved\n", *index_ptr);
            break;
        case RTL9310_DBG_CNTR_VLAN_PROFILE_UNKNOWN_IP6_MC_FLPM:
            diag_util_mprintf("Debug Counter %2d - Reserved\n", *index_ptr);
            break;
        default:
            diag_util_printf("User config: Error!\n");
    }
#endif
#if defined(CONFIG_SDK_RTL9300)
    switch (*index_ptr)
    {
        case RTL9300_DBG_CNTR_OAM_PARSER:
            diag_util_mprintf("Debug Counter %2d - OAM Parser : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_UC_RPF:
            diag_util_mprintf("Debug Counter %2d - Unicast RPF (learn) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_DEI_CFI:
            diag_util_mprintf("Debug Counter %2d - DEI=1 or CFI=1 : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_MAC_IP_SUBNET_BASED_VLAN:
            diag_util_mprintf("Debug Counter %2d - MAC/IP-Subnet Based VLAN : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_VLAN_IGR_FILTER:
            diag_util_mprintf("Debug Counter %2d - VLAN Ingress Filter : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L2_UC_MC_BRIDGE:
            diag_util_mprintf("Debug Counter %2d - L2 Unicast/Multicast Bridge Look-up Miss : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IP4_IP6_MC_BRIDGE:
            diag_util_mprintf("Debug Counter %2d - IPv4/IPv6 Multicast Bridge Look-up Miss : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_PTP:
            diag_util_mprintf("Debug Counter %2d - PTP : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_USER_DEF_0_3:
            diag_util_mprintf("Debug Counter %2d - User Define RMA : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_RESERVED:
            diag_util_mprintf("Debug Counter %2d - N/A\n", *index_ptr);
            break;
        case RTL9300_DBG_CNTR_RESERVED1:
             diag_util_mprintf("Debug Counter %2d - N/A \n", *index_ptr);
            break;
        case RTL9300_DBG_CNTR_RESERVED2:
             diag_util_mprintf("Debug Counter %2d - N/A \n", *index_ptr);
            break;
        case RTL9300_DBG_CNTR_BPDU_RMA:
            diag_util_mprintf("Debug Counter %2d - BPDU : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_LACP:
            diag_util_mprintf("Debug Counter %2d - LACP : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_LLDP:
            diag_util_mprintf("Debug Counter %2d - LLDP : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_EAPOL:
            diag_util_mprintf("Debug Counter %2d - EAPOL : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_XX_RMA:
            diag_util_mprintf("Debug Counter %2d - RMA with MAC 01-80-C2-00-00-xx : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_IPUC_NON_IP:
            diag_util_mprintf("Debug Counter %2d - L3 IPUC Non-IP : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IP4_IP6_HEADER_ERROR:
            diag_util_mprintf("Debug Counter %2d - L3 IP Header error : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_BAD_IP:
            diag_util_mprintf("Debug Counter %2d - Invalid IP Adderss (bad sip, bad dip, zero sip) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_DIP_DMAC_MISMATCH:
            diag_util_mprintf("Debug Counter %2d - L3 DIP/DMAC Mismatch : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IP4_IP_OPTION:
            diag_util_mprintf("Debug Counter %2d - L3 IPv4 option header : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IP_UC_MC_ROUTING_LOOK_UP_MISS:
            diag_util_mprintf("Debug Counter %2d - IP Unicast/Multicast Routing Look-up Miss : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_DST_NULL_INTF:
            diag_util_mprintf("Debug Counter %2d - L3 Destination NULL interface : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_PBR_NULL_INTF:
            diag_util_mprintf("Debug Counter %2d - L3 PBR Destination NULL interface : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_HOST_NULL_INTF:
            diag_util_mprintf("Debug Counter %2d - L3 Host entry with DST_NULL_INTF : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_ROUTE_NULL_INTF:
            diag_util_mprintf("Debug Counter %2d - L3 Route entry with DST_NULL_INTF : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_BRIDGING_ACTION:
            diag_util_mprintf("Debug Counter %2d - IP multicast bridge action drop : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_ROUTING_ACTION:
            diag_util_mprintf("Debug Counter %2d - IP multicast routing action drop : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IPMC_RPF:
            diag_util_mprintf("Debug Counter %2d - Multicast RPF : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L2_NEXTHOP_AGE_OUT:
            diag_util_mprintf("Debug Counter %2d - L2 Nexthop entry age out : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_UC_TTL_FAIL:
            diag_util_mprintf("Debug Counter %2d - IPUC TTL : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_MC_TTL_FAIL:
            diag_util_mprintf("Debug Counter %2d - IPMC TTL : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_UC_MTU_FAIL:
            diag_util_mprintf("Debug Counter %2d - IPUC MTU : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_MC_MTU_FAIL:
            diag_util_mprintf("Debug Counter %2d - IPMC TTL : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_UC_ICMP_REDIR:
            diag_util_mprintf("Debug Counter %2d - ICMP Redirect : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IP6_MLD_OTHER_ACT:
            diag_util_mprintf("Debug Counter %2d - IPv6 MLD_ACT_0_X_X and MLD_ACT_DB8_X_X : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_ND:
            diag_util_mprintf("Debug Counter %2d - IPv6 Neighbor Discovery : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IP_MC_RESERVED:
            diag_util_mprintf("Debug Counter %2d - IPv4/IPv6  reserved multicast address : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IP6_HBH:
            diag_util_mprintf("Debug Counter %2d - IPv6 Hop-by-Hop or Hop-by-Hop error Header : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_INVALID_SA:
            diag_util_mprintf("Debug Counter %2d - Invalid SA (Bcast, Mcast) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L2_HASH_FULL:
            diag_util_mprintf("Debug Counter %2d - L2 Hash Full : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_NEW_SA:
            diag_util_mprintf("Debug Counter %2d - New SA : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_PORT_MOVE_FORBID:
            diag_util_mprintf("Debug Counter %2d - Port move Forbid : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_STATIC_PORT_MOVING:
            diag_util_mprintf("Debug Counter %2d - Static Port moving : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_DYNMIC_PORT_MOVING:
            diag_util_mprintf("Debug Counter %2d - Dynamic Port moving : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_CRC:
            diag_util_mprintf("Debug Counter %2d - L3 CRC (learn) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_MAC_LIMIT:
            diag_util_mprintf("Debug Counter %2d - Mac Limit Learn : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_ATTACK_PREVENT:
            diag_util_mprintf("Debug Counter %2d - Attack Prevent : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_ACL_FWD_ACTION:
            diag_util_mprintf("Debug Counter %2d - ACL forwarding action : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_OAMPDU:
            diag_util_mprintf("Debug Counter %2d - OAMPDU (OAM disabled, option is drop) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_OAM_MUX:
            diag_util_mprintf("Debug Counter %2d - OAM Mux (NON_OAM TX MUT/LB dorp, OAM enable) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_TRUNK_FILTER:
            diag_util_mprintf("Debug Counter %2d - Trunk filter : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_ACL_DROP:
            diag_util_mprintf("Debug Counter %2d - ACL Drop (learn) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IGR_BW:
            diag_util_mprintf("Debug Counter %2d - Ingress Bandwidth (except Backdoor, IACL, EACL Bypass) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_ACL_METER:
            diag_util_mprintf("Debug Counter %2d - ACL Meter : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_VLAN_ACCEPT_FRAME_TYPE:
            diag_util_mprintf("Debug Counter %2d - VLAN Accept Frame Type : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_MSTP_SRC_DROP_DISABLED_BLOCKING:
            diag_util_mprintf("Debug Counter %2d - MSTP SRC Drop (Disabled, Blocking) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_SA_BLOCK:
            diag_util_mprintf("Debug Counter %2d - SA Block (learn but not update learning counter) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_DA_BLOCK:
            diag_util_mprintf("Debug Counter %2d - DA Block : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_STORM_CONTROL:
            diag_util_mprintf("Debug Counter %2d - Storm Control : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_VLAN_EGR_FILTER:
            diag_util_mprintf("Debug Counter %2d - VLAN Egress Filter : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_MSTP_DESTINATION_DROP:
            diag_util_mprintf("Debug Counter %2d - MSTP Destination Drop (disabled,blocking, learning) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_SRC_PORT_FILTER:
            diag_util_mprintf("Debug Counter %2d - Source Port Filter (Exclude Routed Packet) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_PORT_ISOLATION:
            diag_util_mprintf("Debug Counter %2d - Port Isolation : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_TX_MAX_FRAME_SIZE:
            diag_util_mprintf("Debug Counter %2d - TX Max Frame Size : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_EGR_LINK_STATUS:
            diag_util_mprintf("Debug Counter %2d - Egress Link Status : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_MAC_TX_DISABLE:
            diag_util_mprintf("Debug Counter %2d - MAC Tx Disable : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_MAC_PAUSE_FRAME:
            diag_util_mprintf("Debug Counter %2d - MAC Frame (Pause frame and unknown opcode) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_MAC_RX_DROP:
            diag_util_mprintf("Debug Counter %2d - MAC Rx Drop (LLC length check fail, 48 pass 1, dsc runout, L2 CRC, oversize, undersize, symbol error) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_MIRROR_ISOLATE:
            diag_util_mprintf("Debug Counter %2d - Mirror Isolate : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_RX_FC:
            diag_util_mprintf("Debug Counter %2d - Rx Flow control : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_EGR_QUEUE:
            diag_util_mprintf("Debug Counter %2d - Egress Queue Drop Tail Drop/SWRED Drop : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_HSM_RUNOUT:
            diag_util_mprintf("Debug Counter %2d - HSM Runout (replication) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_ROUTING_DISABLE:
            diag_util_mprintf("Debug Counter %2d - L3 Routing ability is disable (global or per-interface) : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_INVALID_L2_NEXTHOP_ENTRY:
            diag_util_mprintf("Debug Counter %2d - L3 Routing invalid l2 nexthop entry : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_L3_MC_SRC_FLT:
            diag_util_mprintf("Debug Counter %2d - L3 IPMC Routing Source VLAN filter : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_CPUTAG_FLT:
            diag_util_mprintf("Debug Counter %2d - CPU Tag filter PortId : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_FWD_PMSK_NULL:
            diag_util_mprintf("Debug Counter %2d - Foward Table Entry Port Mask is Null : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IPUC_ROUTING_LOOKUP_MISS:
            diag_util_mprintf("Debug Counter %2d - IP Unicast Routing Look-up Miss : %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_MY_DEV_DROP:
            diag_util_mprintf("Debug Counter %2d - SRC unit is equal to my unit: %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_STACK_NONUC_BLOCKING_PMSK:
            diag_util_mprintf("Debug Counter %2d - Stacking NonUnicast Blocking PortMask (after non-uc-block-port filter): %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_STACK_PORT_NOT_FOUND:
            diag_util_mprintf("Debug Counter %2d - Stacking not found for special unit: %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_ACL_LOOPBACK_DROP:
            diag_util_mprintf("Debug Counter %2d - ACL loopback drop while exceeding loopback times: %u \n", *index_ptr, cntr);
            break;
        case RTL9300_DBG_CNTR_IP6_ROUTING_EXT_HEADER:
            diag_util_mprintf("Debug Counter %2d - IPv6 UC/MC Extension Header : %u \n", *index_ptr, cntr);
            break;
        default:
            diag_util_printf("User config: Error!\n");
    }
#endif


    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_DUMP_MIB_COUNTER_DEBUG_ALL
/*
 * debug dump mib counter debug all
 */
cparser_result_t cparser_cmd_debug_dump_mib_counter_debug_all(cparser_context_t *context)
{
    uint32  unit = 0;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    uint32  idx=0, cntr = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  idx2 = 0;
#endif
#if defined(CONFIG_SDK_RTL9310)
        rtk_dbg_encap_cntr_t encapCntr;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        for (idx2 = 0; idx2 < RTK_DBG_MIB_DBG_TYPE_END; idx2++)
        {
            ret = hal_getDbgCntr(unit, idx2, &cntr);
         if(ret != RT_ERR_OK)
             continue;

            switch (idx2)
            {
                case RTL8390_DBG_MIB_ALE_TX_GOOD_PKTS:
                    diag_util_mprintf("Debug Counter %d - The number of ALE TX good packets : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_ERROR_PKTS:
                    diag_util_mprintf("Debug Counter %d - Error packet : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_EGR_ACL_DROP:
                    diag_util_mprintf("Debug Counter %d - Egress ACL drop : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_EGR_METER_DROP:
                    diag_util_mprintf("Debug Counter %d - Egress meter drop : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_OAM:
                    diag_util_mprintf("Debug Counter %d - OAM : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_CFM:
                    diag_util_mprintf("Debug Counter %d - CFM : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_VLAN_IGR_FLTR:
                    diag_util_mprintf("Debug Counter %d - VLAN ingress filter : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_VLAN_ERR:
                    diag_util_mprintf("Debug Counter %d - VLAN Error(VID=4095 or MBR=0) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_INNER_OUTER_CFI_EQUAL_1:
                    diag_util_mprintf("Debug Counter %d - inner/outer CFI=1 : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_VLAN_TAG_FORMAT:
                    diag_util_mprintf("Debug Counter %d - VLAN tag format : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_SRC_PORT_SPENDING_TREE:
                    diag_util_mprintf("Debug Counter %d - Source port spending tree : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_INBW:
                    diag_util_mprintf("Debug Counter %d - Input bandwidth : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_RMA:
                    diag_util_mprintf("Debug Counter %d - RMA : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_HW_ATTACK_PREVENTION:
                    diag_util_mprintf("Debug Counter %d - hardware attack prevention : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_PROTO_STORM:
                    diag_util_mprintf("Debug Counter %d - protocol storm : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_MCAST_SA:
                    diag_util_mprintf("Debug Counter %d - multicast SA : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_IGR_ACL_DROP:
                    diag_util_mprintf("Debug Counter %d - Ingress ACL drop : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_IGR_METER_DROP:
                    diag_util_mprintf("Debug Counter %d - Ingress Meter drop : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_DFLT_ACTION_FOR_MISS_ACL_AND_C2SC:
                    diag_util_mprintf("Debug Counter %d - Default action for miss ACL &C2SC : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_NEW_SA:
                    diag_util_mprintf("Debug Counter %d - New SA : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_PORT_MOVE:
                    diag_util_mprintf("Debug Counter %d - Port move : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_SA_BLOCKING:
                    diag_util_mprintf("Debug Counter %d - SA blocking : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_ROUTING_EXCEPTION:
                    diag_util_mprintf("Debug Counter %d - routing exception : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_SRC_PORT_SPENDING_TREE_NON_FWDING:
                    diag_util_mprintf("Debug Counter %d - Source port spending tree(non forwarding) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_MAC_LIMIT:
                    diag_util_mprintf("Debug Counter %d - MAC limit : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_UNKNOW_STORM:
                    diag_util_mprintf("Debug Counter %d - Unknow storm : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_MISS_DROP:
                    diag_util_mprintf("Debug Counter %d - Miss Drop : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_CPU_MAC_DROP:
                    diag_util_mprintf("Debug Counter %d - CPU MAC drop : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_DA_BLOCKING:
                    diag_util_mprintf("Debug Counter %d - DA blocking : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_SRC_PORT_FILTER_BEFORE_EGR_ACL:
                    diag_util_mprintf("Debug Counter %d (Egress)- Source port filter(before Egress ACL) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_VLAN_EGR_FILTER:
                    diag_util_mprintf("Debug Counter %d (Egress) - VLAN egress filter : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_SPANNING_TRE:
                    diag_util_mprintf("Debug Counter %d (Egress) - Spanning tree : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_PORT_ISOLATION:
                    diag_util_mprintf("Debug Counter %d (Egress) - Port isolation : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_OAM_EGRESS_DROP:
                    diag_util_mprintf("Debug Counter %d (Egress) - OAM : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_MIRROR_ISOLATION:
                    diag_util_mprintf("Debug Counter %d (Egress) - Mirror isolation : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_MAX_LEN_BEFORE_EGR_ACL:
                    diag_util_mprintf("Debug Counter %d (Egress) - Max length(before Egress ACL) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_SRC_PORT_FILTER_BEFORE_MIRROR:
                    diag_util_mprintf("Debug Counter %d (Egress) - Source port filter(before Mirror) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_MAX_LEN_BEFORE_MIRROR:
                    diag_util_mprintf("Debug Counter %d (Egress) - Max length(before Mirror) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_SPECIAL_CONGEST_BEFORE_MIRROR:
                    diag_util_mprintf("Debug Counter %d (Egress) - Special congest(before Mirror) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_LINK_STATUS_BEFORE_MIRROR:
                    diag_util_mprintf("Debug Counter %d (Egress) - Link status(before Mirror) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_WRED_BEFORE_MIRROR:
                    diag_util_mprintf("Debug Counter %d (Egress) - WRED(before Mirror) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_MAX_LEN_AFTER_MIRROR:
                    diag_util_mprintf("Debug Counter %d (Egress) - Max length(after Mirror) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_SPECIAL_CONGEST_AFTER_MIRROR:
                    diag_util_mprintf("Debug Counter %d (Egress) - Special congest(after Mirror) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_LINK_STATUS_AFTER_MIRROR:
                    diag_util_mprintf("Debug Counter %d (Egress) - Link status(after Mirror) : %u \n", idx, cntr);
                    break;
                case RTL8390_DBG_MIB_WRED_AFTER_MIRROR:
                    diag_util_mprintf("Debug Counter %d (Egress) - WRED(after Mirror) : %u \n", idx, cntr);
                    break;
                default:
                    diag_util_mprintf("Debug Counter %d Usr Config Error !\n", idx);
                    break;
            }
         idx++;

        }
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        for (idx2 = 0; idx2 < RTK_DBG_MIB_DBG_TYPE_END; idx2++)
        {
            ret = hal_getDbgCntr(unit, idx2, &cntr);

         if(ret != RT_ERR_OK)
             continue;

            switch (idx2)
            {
                case RTL8380_DBG_MIB_ALE_TX_GOOD_PKTS:
                    diag_util_mprintf("Debug Counter %d - The number of ALE TX good packets : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_MAC_RX_DROP:
                    diag_util_mprintf("Debug Counter %d - MAC RX Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_ACL_FWD_DROP:
                    diag_util_mprintf("Debug Counter %d - ACL Forward drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_HW_ATTACK_PREVENTION_DROP:
                    diag_util_mprintf("Debug Counter %d - Attack prevention drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_RMA_DROP:
                    diag_util_mprintf("Debug Counter %d - RMA Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_VLAN_IGR_FLTR_DROP:
                    diag_util_mprintf("Debug Counter %d - VLAN IGR Filter Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_INNER_OUTER_CFI_EQUAL_1_DROP:
                    diag_util_mprintf("Debug Counter %d - CFI=1 Drop: %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_PORT_MOVE_DROP:
                    diag_util_mprintf("Debug Counter %d - Port Move Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_NEW_SA_DROP:
                    diag_util_mprintf("Debug Counter %d - NEW SA Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_MAC_LIMIT_SYS_DROP:
                    diag_util_mprintf("Debug Counter %d - MAC Limit SYS Drop: %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_MAC_LIMIT_VLAN_DROP:
                    diag_util_mprintf("Debug Counter %d - MAC Limit VLAN Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_MAC_LIMIT_PORT_DROP:
                    diag_util_mprintf("Debug Counter %d - MAC Limit Port Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_SWITCH_MAC_DROP:
                    diag_util_mprintf("Debug Counter %d - Switch MAC Drop: %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_ROUTING_EXCEPTION_DROP:
                    diag_util_mprintf("Debug Counter %d - Routing Exception Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_DA_LKMISS_DROP:
                    diag_util_mprintf("Debug Counter %d - DA Lookup Missed Drop: %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_RSPAN_DROP:
                    diag_util_mprintf("Debug Counter %d - RSPAN Drop: %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_ACL_LKMISS_DROP:
                    diag_util_mprintf("Debug Counter %d - Ingress ACL Lookup missed drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_ACL_DROP:
                    diag_util_mprintf("Debug Counter %d - Ingress ACL  drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_INBW_DROP:
                    diag_util_mprintf("Debug Counter %d - Input Bandwidth Control Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_IGR_METER_DROP:
                    diag_util_mprintf("Debug Counter %d - Ingress Meter Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_ACCEPT_FRAME_TYPE_DROP:
                    diag_util_mprintf("Debug Counter %d - Accept Frame Type Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_STP_IGR_DROP:
                    diag_util_mprintf("Debug Counter %d - STP Ingress Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_INVALID_SA_DROP:
                    diag_util_mprintf("Debug Counter %d - Invalid SA Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_SA_BLOCKING_DROP:
                    diag_util_mprintf("Debug Counter %d - SA Blocking Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_DA_BLOCKING_DROP:
                    diag_util_mprintf("Debug Counter %d - DA Blocking Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_L2_INVALID_DPM_DROP:
                    diag_util_mprintf("Debug Counter %d - L2 invalid DPM Drop  : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_MCST_INVALID_DPM_DROP:
                    diag_util_mprintf("Debug Counter %d - MAC Constraint Invalid DPM Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_RX_FLOW_CONTROL_DROP:
                    diag_util_mprintf("Debug Counter %d - RX Flow Control Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_STORM_SPPRS_DROP:
                    diag_util_mprintf("Debug Counter %d - Storm Susspresion Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_LALS_DROP:
                    diag_util_mprintf("Debug Counter %d - Link Aggregation Load sharing Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_VLAN_EGR_FILTER_DROP:
                    diag_util_mprintf("Debug Counter %d - VLAN egress filter Drop: %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_STP_EGR_DROP:
                    diag_util_mprintf("Debug Counter %d - Egress Spanning tree Drop: %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_SRC_PORT_FILTER_DROP:
                    diag_util_mprintf("Debug Counter %d - Source Port Filter  Drop: %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_PORT_ISOLATION_DROP:
                    diag_util_mprintf("Debug Counter %d - Port Isolation Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_ACL_FLTR_DROP:
                    diag_util_mprintf("Debug Counter %d - ACL Filter Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_MIRROR_FLTR_DROP:
                    diag_util_mprintf("Debug Counter %d - Mirror Filter Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_TX_MAX_DROP:
                    diag_util_mprintf("Debug Counter %d - TX MAX Length Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_LINK_DOWN_DROP:
                    diag_util_mprintf("Debug Counter %d - Link down Drop : %u \n", idx, cntr);
                    break;
                case RTL8380_DBG_MIB_FLOW_CONTROL_DROP:
                    diag_util_mprintf("Debug Counter %d - Flow Control Drop: %u \n", idx, cntr);
                    break;
                default:
                    diag_util_mprintf("Debug Counter %d Usr Config Error !\n", idx);
                    break;
            }

        idx++;

        }
    }
#endif

#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        for (idx2 = 0; idx2 < RTK_DBG_MIB_DBG_TYPE_END; idx2++)
        {
            ret = hal_getDbgCntr(unit, idx2, &cntr);
            if (ret != RT_ERR_OK)
                continue;

            switch (idx2)
            {
                case RTL9310_DBG_CNTR_ALE_RX_GOOD_PKTS:
                    diag_util_mprintf("Debug Counter %2d - The nummber of ALE receive packet : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_RX_MAX_FRAME_SIZE:
                    diag_util_mprintf("Debug Counter %2d - Reserved\n", idx);
                    break;
                case RTL9310_DBG_CNTR_MAC_RX_DROP:
                    diag_util_mprintf("Debug Counter %2d - MAC Rx Drop : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_OPENFLOW_IP_MPLS_TTL:
                    diag_util_mprintf("Debug Counter %2d - OpenFlow IP/MPLS TTL : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_OPENFLOW_TBL_MISS:
                    diag_util_mprintf("Debug Counter %2d - OpenFlow Table Miss : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IGR_BW:
                    diag_util_mprintf("Debug Counter %2d - Ingress Bandwidth (except Backdoor, IACL, EACL Bypass) : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_SPECIAL_CONGEST:
                    diag_util_mprintf("Debug Counter %2d - Special Congest : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_EGR_QUEUE:
                    diag_util_mprintf("Debug Counter %2d - Egress Queue/S-WRED : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_RESERVED:
                    diag_util_mprintf("Debug Counter %2d - Reserved\n", idx);
                    break;
                case RTL9310_DBG_CNTR_EGR_LINK_STATUS:
                    diag_util_mprintf("Debug Counter %2d - Egress Link Status : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_STACK_UCAST_NONUCAST_TTL:
                    diag_util_mprintf("Debug Counter %2d - Reserved\n", idx);
                    break;
                case RTL9310_DBG_CNTR_STACK_NONUC_BLOCKING_PMSK:
                    diag_util_mprintf("Debug Counter %2d - Stacking NonUnicast Blocking PortMask : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_L2_CRC:
                    diag_util_mprintf("Debug Counter %2d - L2 CRC : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_SRC_PORT_FILTER:
                    diag_util_mprintf("Debug Counter %2d - Source Port Filter (Exclude Routed Packet) : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_PARSER_PACKET_TOO_LONG:
                    diag_util_mprintf("Debug Counter %2d - Parser Packet Too Long : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_PARSER_MALFORM_PACKET:
                    diag_util_mprintf("Debug Counter %2d - Parser Malform Packet : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_MPLS_OVER_2_LBL:
                    diag_util_mprintf("Debug Counter %2d - MPLS Over 2 Label : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_EACL_METER:
                    diag_util_mprintf("Debug Counter %2d - IACL Meter : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IACL_METER:
                    diag_util_mprintf("Debug Counter %2d - VACL Meter : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_PROTO_STORM:
                    diag_util_mprintf("Debug Counter %2d - Protocol Storm : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_INVALID_CAPWAP_HEADER:
                    diag_util_mprintf("Debug Counter %2d - Invalid CAPWAP Header or BPE Filter : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_MAC_IP_SUBNET_BASED_VLAN:
                    diag_util_mprintf("Debug Counter %2d - MAC/IP-Subnet Based VLAN : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_OAM_PARSER:
                    diag_util_mprintf("Debug Counter %2d - OAM Parser : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_UC_MC_RPF:
                    diag_util_mprintf("Debug Counter %2d - Unicast/Multicast RPF : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IP_MAC_BINDING_MATCH_MISMATCH:
                    diag_util_mprintf("Debug Counter %2d - IP-MAC Binding Match/Mismatch : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_SA_BLOCK:
                    diag_util_mprintf("Debug Counter %2d - SA Block : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_TUNNEL_IP_ADDRESS_CHECK:
                    diag_util_mprintf("Debug Counter %2d - Tunnel IP Address Check : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_EACL_DROP:
                    diag_util_mprintf("Debug Counter %2d - IACL Drop : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IACL_DROP:
                    diag_util_mprintf("Debug Counter %2d - VACL Drop : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_ATTACK_PREVENT:
                    diag_util_mprintf("Debug Counter %2d - Attack Prevent : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_SYSTEM_PORT_LIMIT_LEARN:
                    diag_util_mprintf("Debug Counter %2d - System/Port-based MAC Limit : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_OAMPDU:
                    diag_util_mprintf("Debug Counter %2d - OAMPDU (OAM disabled, option is drop) : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_CCM_RX:
                    diag_util_mprintf("Debug Counter %2d - CCM RX : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_CFM_UNKNOWN_TYPE:
                    diag_util_mprintf("Debug Counter %2d - CFM Unknown Type : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_LBM_LBR_LTM_LTR:
                    diag_util_mprintf("Debug Counter %2d - LBM/LBR, LTM/LTR : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_Y_1731:
                    diag_util_mprintf("Debug Counter %2d - Y.1731 (ETHDM) : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_VLAN_LIMIT_LEARN:
                    diag_util_mprintf("Debug Counter %2d - VLAN Limit Learn : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_VLAN_ACCEPT_FRAME_TYPE:
                    diag_util_mprintf("Debug Counter %2d - VLAN Accept Frame Type : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_CFI_1:
                    diag_util_mprintf("Debug Counter %2d - VLAN CFI=1 : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_STATIC_DYNAMIC_PORT_MOVING:
                    diag_util_mprintf("Debug Counter %2d - Static/Dynamic Port Moving : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_PORT_MOVE_FORBID:
                    diag_util_mprintf("Debug Counter %2d - Port Move Forbidden : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_L3_CRC:
                    diag_util_mprintf("Debug Counter %2d - IP Checksum Error : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_BPDU_PTP_LLDP_EAPOL_RMA:
                    diag_util_mprintf("Debug Counter %2d - BPDU/PTP/LLDP/EAPOL/RMA : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_MSTP_SRC_DROP_DISABLED_BLOCKING:
                    diag_util_mprintf("Debug Counter %2d - Ingress Spanning Tree Filter (Disabled/Blocking State) : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_INVALID_SA:
                    diag_util_mprintf("Debug Counter %2d - Invalid SA (Zero, Bcast, Mcast) : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_NEW_SA:
                    diag_util_mprintf("Debug Counter %2d - New SA : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_VLAN_IGR_FILTER:
                    diag_util_mprintf("Debug Counter %2d - VLAN Ingress Filter : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IGR_VLAN_CONVERT:
                    diag_util_mprintf("Debug Counter %2d - Reserved\n", idx);
                    break;
                case RTL9310_DBG_CNTR_GRATUITOUS_ARP:
                    diag_util_mprintf("Debug Counter %2d - Gratuitous ARP : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_MSTP_SRC_DROP:
                    diag_util_mprintf("Debug Counter %2d - Ingress Spanning Tree Filter (Learning State) : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_L2_HASH_FULL:
                    diag_util_mprintf("Debug Counter %2d - L2 Hash Full : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_MPLS_UNKNOWN_LBL:
                    diag_util_mprintf("Debug Counter %2d - MPLS Unknown Label : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_L3_IPUC_NON_IP:
                    diag_util_mprintf("Debug Counter %2d - My Packet Drop : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_TTL:
                    diag_util_mprintf("Debug Counter %2d - TTL : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_MTU:
                    diag_util_mprintf("Debug Counter %2d - MTU/OIL RPF : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_ICMP_REDIRECT:
                    diag_util_mprintf("Debug Counter %2d - ICMP Redirect : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_STORM_CONTROL:
                    diag_util_mprintf("Debug Counter %2d - Storm Control : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_L3_DIP_DMAC_MISMATCH:
                    diag_util_mprintf("Debug Counter %2d - IPMC DIP/DMAC Mismatch : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IP4_IP_OPTION:
                    diag_util_mprintf("Debug Counter %2d - IPv4 IP Option/IPv6 Hop-by-Hop Extension Header Existed : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IP6_HBH_EXT_HEADER:
                    diag_util_mprintf("Debug Counter %2d - Reserved IPv4/IPv6 Multicast : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IP4_IP6_HEADER_ERROR:
                    diag_util_mprintf("Debug Counter %2d - IPv4/IPv6 Header Error : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_ROUTING_IP_ADDR_CHECK:
                    diag_util_mprintf("Debug Counter %2d - Routing IP Addr Check : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_ROUTING_EXCEPTION:
                    diag_util_mprintf("Debug Counter %2d - Routing Exception : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_DA_BLOCK:
                    diag_util_mprintf("Debug Counter %2d - DA Block : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_OAM_MUX:
                    diag_util_mprintf("Debug Counter %2d - OAM Multiplexer Drop : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_PORT_ISOLATION:
                    diag_util_mprintf("Debug Counter %2d - Port Isolation : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_VLAN_EGR_FILTER:
                    diag_util_mprintf("Debug Counter %2d - VLAN Egress Filter : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_MIRROR_ISOLATE:
                    diag_util_mprintf("Debug Counter %2d - Mirror Isolation : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_MSTP_DESTINATION_DROP:
                    diag_util_mprintf("Debug Counter %2d - Egress Spanning Tree Filter (Disabled/Blocking/Learning State) : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_L2_MC_BRIDGE:
                    diag_util_mprintf("Debug Counter %2d - L2 Multicast Bridge : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IP_UC_MC_ROUTING_LOOK_UP_MISS:
                    diag_util_mprintf("Debug Counter %2d - IP Unicast/Multicast Routing Look-up Miss : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_L2_UC:
                    diag_util_mprintf("Debug Counter %2d - Unknown L2 Unicast : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_L2_MC:
                    diag_util_mprintf("Debug Counter %2d - Unknown L2 Multicast : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IP4_MC:
                    diag_util_mprintf("Debug Counter %2d - Unknown IPv4 Multicast : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_IP6_MC:
                    diag_util_mprintf("Debug Counter %2d - Unknown IPv6 Multicast : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_L3_UC_MC_ROUTE:
                    diag_util_mprintf("Debug Counter %2d - L3 Unicast/Multicast Route : %u \n", idx, cntr);
                    break;
                case RTL9310_DBG_CNTR_UNKNOWN_L2_UC_FLPM:
                    diag_util_mprintf("Debug Counter %2d - Reserved\n", idx);
                    break;
                case RTL9310_DBG_CNTR_BC_FLPM:
                    diag_util_mprintf("Debug Counter %2d - Reserved\n", idx);
                    break;
                case RTL9310_DBG_CNTR_VLAN_PRO_UNKNOWN_L2_MC_FLPM:
                    diag_util_mprintf("Debug Counter %2d - Reserved\n", idx);
                    break;
                case RTL9310_DBG_CNTR_VLAN_PRO_UNKNOWN_IP4_MC_FLPM:
                    diag_util_mprintf("Debug Counter %2d - Reserved\n", idx);
                    break;
                case RTL9310_DBG_CNTR_VLAN_PROFILE_UNKNOWN_IP6_MC_FLPM:
                    diag_util_mprintf("Debug Counter %2d - Reserved\n", idx);
                    break;
                default:
                    diag_util_printf("User config: Error!\n");
            }
            idx++;
        }

        osal_memset(&encapCntr, 0, sizeof(rtk_dbg_encap_cntr_t));
        DIAG_UTIL_ERR_CHK(hal_getDbgEncapCntr(unit, &encapCntr), ret);
        diag_util_mprintf("Encap Debug Counter - OpenFlow Drop : %u \n", encapCntr.encapOpenFlow);
        diag_util_mprintf("Encap Debug Counter - EACL Drop : %u \n", encapCntr.encapEACL);
        diag_util_mprintf("Encap Debug Counter - Lookup Miss Drop : %u \n", encapCntr.encapEVC);
        diag_util_mprintf("Encap Debug Counter - IP TTL=0 / MPLS TTL=0 : %u \n", encapCntr.encapTTL);
    }
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        for (idx2 = 0; idx2 < RTK_DBG_MIB_DBG_TYPE_END; idx2++)
        {
            ret = hal_getDbgCntr(unit, idx2, &cntr);
            if (ret != RT_ERR_OK)
                continue;

            switch (idx2)
            {
                case RTL9300_DBG_CNTR_OAM_PARSER:
                    diag_util_mprintf("Debug Counter %2d - OAM Parser : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_UC_RPF:
                    diag_util_mprintf("Debug Counter %2d - Unicast RPF : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_DEI_CFI:
                    diag_util_mprintf("Debug Counter %2d - DEI=1 or CFI=1 : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_MAC_IP_SUBNET_BASED_VLAN:
                    diag_util_mprintf("Debug Counter %2d - MAC/IP-Subnet Based VLAN : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_VLAN_IGR_FILTER:
                    diag_util_mprintf("Debug Counter %2d - VLAN Ingress Filter : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L2_UC_MC_BRIDGE:
                    diag_util_mprintf("Debug Counter %2d - L2 Unicast/Multicast Bridge Look-up Miss : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IP4_IP6_MC_BRIDGE:
                    diag_util_mprintf("Debug Counter %2d - IPv4/IPv6 Multicast Bridge Look-up Miss : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_PTP:
                    diag_util_mprintf("Debug Counter %2d - PTP : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_USER_DEF_0_3:
                    diag_util_mprintf("Debug Counter %2d - User Define RMA : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_RESERVED:
                    diag_util_mprintf("Debug Counter %2d - N/A\n", idx);
                    break;
                case RTL9300_DBG_CNTR_RESERVED1:
                    diag_util_mprintf("Debug Counter %2d - N/A \n", idx);
                    break;
                case RTL9300_DBG_CNTR_RESERVED2:
                    diag_util_mprintf("Debug Counter %2d - N/A \n", idx);
                    break;
                case RTL9300_DBG_CNTR_BPDU_RMA:
                    diag_util_mprintf("Debug Counter %2d - BPDU : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_LACP:
                    diag_util_mprintf("Debug Counter %2d - LACP : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_LLDP:
                    diag_util_mprintf("Debug Counter %2d - LLDP : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_EAPOL:
                    diag_util_mprintf("Debug Counter %2d - EAPOL : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_XX_RMA:
                    diag_util_mprintf("Debug Counter %2d - RMA with MAC 01-80-C2-00-00-xx : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_IPUC_NON_IP:
                    diag_util_mprintf("Debug Counter %2d - L3 IPUC Non-IP : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IP4_IP6_HEADER_ERROR:
                    diag_util_mprintf("Debug Counter %2d - L3 IP Header error : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_BAD_IP:
                    diag_util_mprintf("Debug Counter %2d - Invalid IP Adderss (bad sip, bad dip, zero sip) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_DIP_DMAC_MISMATCH:
                    diag_util_mprintf("Debug Counter %2d - L3 DIP/DMAC Mismatch : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IP4_IP_OPTION:
                    diag_util_mprintf("Debug Counter %2d - L3 IPv4 option header : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IP_UC_MC_ROUTING_LOOK_UP_MISS:
                    diag_util_mprintf("Debug Counter %2d - IP Unicast/Multicast Routing Look-up Miss : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_DST_NULL_INTF:
                    diag_util_mprintf("Debug Counter %2d - L3 Destination NULL interface : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_PBR_NULL_INTF:
                    diag_util_mprintf("Debug Counter %2d - L3 PBR Destination NULL interface : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_HOST_NULL_INTF:
                    diag_util_mprintf("Debug Counter %2d - L3 Host entry with DST_NULL_INTF : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_ROUTE_NULL_INTF:
                    diag_util_mprintf("Debug Counter %2d - L3 Route entry with DST_NULL_INTF : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_BRIDGING_ACTION:
                    diag_util_mprintf("Debug Counter %2d - IP multicast bridge action drop : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_ROUTING_ACTION:
                    diag_util_mprintf("Debug Counter %2d - IP multicast routing action drop : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IPMC_RPF:
                    diag_util_mprintf("Debug Counter %2d - Multicast RPF : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L2_NEXTHOP_AGE_OUT:
                    diag_util_mprintf("Debug Counter %2d - L2 Nexthop entry age out : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_UC_TTL_FAIL:
                    diag_util_mprintf("Debug Counter %2d - IPUC TTL : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_MC_TTL_FAIL:
                    diag_util_mprintf("Debug Counter %2d - IPMC TTL : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_UC_MTU_FAIL:
                    diag_util_mprintf("Debug Counter %2d - IPUC MTU : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_MC_MTU_FAIL:
                    diag_util_mprintf("Debug Counter %2d - IPMC MTU : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_UC_ICMP_REDIR:
                    diag_util_mprintf("Debug Counter %2d - ICMP Redirect : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IP6_MLD_OTHER_ACT:
                    diag_util_mprintf("Debug Counter %2d - IPv6 MLD_ACT_0_X_X and MLD_ACT_DB8_X_X : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_ND:
                    diag_util_mprintf("Debug Counter %2d - IPv6 Neighbor Discovery : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IP_MC_RESERVED:
                    diag_util_mprintf("Debug Counter %2d - IPv4/IPv6  reserved multicast address : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IP6_HBH:
                    diag_util_mprintf("Debug Counter %2d - IPv6 Hop-by-Hop or Hop-by-Hop error Header : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_INVALID_SA:
                    diag_util_mprintf("Debug Counter %2d - Invalid SA (Bcast, Mcast) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L2_HASH_FULL:
                    diag_util_mprintf("Debug Counter %2d - L2 Hash Full : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_NEW_SA:
                    diag_util_mprintf("Debug Counter %2d - New SA : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_PORT_MOVE_FORBID:
                    diag_util_mprintf("Debug Counter %2d - Port move Forbid : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_STATIC_PORT_MOVING:
                    diag_util_mprintf("Debug Counter %2d - Static Port moving : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_DYNMIC_PORT_MOVING:
                    diag_util_mprintf("Debug Counter %2d - Dynamic Port moving : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_CRC:
                    diag_util_mprintf("Debug Counter %2d - L3 CRC : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_MAC_LIMIT:
                    diag_util_mprintf("Debug Counter %2d - Mac Limit : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_ATTACK_PREVENT:
                    diag_util_mprintf("Debug Counter %2d - Attack Prevent : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_ACL_FWD_ACTION:
                    diag_util_mprintf("Debug Counter %2d - ACL forwarding action : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_OAMPDU:
                    diag_util_mprintf("Debug Counter %2d - OAMPDU (OAM disabled, option is drop) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_OAM_MUX:
                    diag_util_mprintf("Debug Counter %2d - OAM Mux (NON_OAM TX MUT/LB dorp, OAM enable) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_TRUNK_FILTER:
                    diag_util_mprintf("Debug Counter %2d - Trunk filter : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_ACL_DROP:
                    diag_util_mprintf("Debug Counter %2d - ACL Drop : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IGR_BW:
                    diag_util_mprintf("Debug Counter %2d - Ingress Bandwidth (except Backdoor, IACL, EACL Bypass) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_ACL_METER:
                    diag_util_mprintf("Debug Counter %2d - ACL Meter : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_VLAN_ACCEPT_FRAME_TYPE:
                    diag_util_mprintf("Debug Counter %2d - VLAN Accept Frame Type : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_MSTP_SRC_DROP_DISABLED_BLOCKING:
                    diag_util_mprintf("Debug Counter %2d - MSTP SRC Drop (Disabled, Blocking) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_SA_BLOCK:
                    diag_util_mprintf("Debug Counter %2d - SA Block : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_DA_BLOCK:
                    diag_util_mprintf("Debug Counter %2d - DA Block : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_STORM_CONTROL:
                    diag_util_mprintf("Debug Counter %2d - Storm Control : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_VLAN_EGR_FILTER:
                    diag_util_mprintf("Debug Counter %2d - VLAN Egress Filter : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_MSTP_DESTINATION_DROP:
                    diag_util_mprintf("Debug Counter %2d - MSTP Destination Drop (disabled,blocking) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_SRC_PORT_FILTER:
                    diag_util_mprintf("Debug Counter %2d - Source Port Filter (Exclude Routed Packet) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_PORT_ISOLATION:
                    diag_util_mprintf("Debug Counter %2d - Port Isolation : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_TX_MAX_FRAME_SIZE:
                    diag_util_mprintf("Debug Counter %2d - TX Max Frame Size : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_EGR_LINK_STATUS:
                    diag_util_mprintf("Debug Counter %2d - Egress Link Status : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_MAC_TX_DISABLE:
                    diag_util_mprintf("Debug Counter %2d - MAC Tx Disable : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_MAC_PAUSE_FRAME:
                    diag_util_mprintf("Debug Counter %2d - MAC Frame (Pause frame and unknown opcode) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_MAC_RX_DROP:
                    diag_util_mprintf("Debug Counter %2d - MAC Rx Drop (LLC length check fail, 48 pass 1, dsc runout, L2 CRC, oversize, undersize, symbol error) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_MIRROR_ISOLATE:
                    diag_util_mprintf("Debug Counter %2d - Mirror Isolate : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_RX_FC:
                    diag_util_mprintf("Debug Counter %2d - Rx Flow control : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_EGR_QUEUE:
                    diag_util_mprintf("Debug Counter %2d - Egress Queue Drop Tail Drop/SWRED Drop : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_HSM_RUNOUT:
                    diag_util_mprintf("Debug Counter %2d - HSM Runout (replication) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_ROUTING_DISABLE:
                    diag_util_mprintf("Debug Counter %2d - L3 Routing ability is disable (global or per-interface) : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_INVALID_L2_NEXTHOP_ENTRY:
                    diag_util_mprintf("Debug Counter %2d - L3 Routing invalid l2 nexthop entry : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_L3_MC_SRC_FLT:
                    diag_util_mprintf("Debug Counter %2d - L3 IPMC Routing Source VLAN filter : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_CPUTAG_FLT:
                    diag_util_mprintf("Debug Counter %2d - CPU Tag filter PortId : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_FWD_PMSK_NULL:
                    diag_util_mprintf("Debug Counter %2d - Foward Table Entry Port Mask is Null : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IPUC_ROUTING_LOOKUP_MISS:
                    diag_util_mprintf("Debug Counter %2d - IP Unicast Routing Look-up Miss : %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_MY_DEV_DROP:
                    diag_util_mprintf("Debug Counter %2d - SRC unit is equal to my unit: %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_STACK_NONUC_BLOCKING_PMSK:
                    diag_util_mprintf("Debug Counter %2d - Stacking NonUnicast Blocking PortMask (after non-uc-block-port filter): %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_STACK_PORT_NOT_FOUND:
                    diag_util_mprintf("Debug Counter %2d - Stacking not found for special unit: %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_ACL_LOOPBACK_DROP:
                    diag_util_mprintf("Debug Counter %2d - ACL loopback drop while exceeding loopback times: %u \n", idx, cntr);
                    break;
                case RTL9300_DBG_CNTR_IP6_ROUTING_EXT_HEADER:
                    diag_util_mprintf("Debug Counter %2d - IPv6 UC/MC Extension Header : %u \n", idx, cntr);
                    break;
                default:
                    diag_util_printf("User config: Error!\n");
            }
            idx++;
        }
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_GET_FLOWCTRL_USED_PAGE_CNT_EGRESS_INGRESS_PORT_PORTS_ALL
/*
 * debug get flowctrl used-page-cnt ( ingress | egress ) port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_debug_get_flowctrl_used_page_cnt_egress_ingress_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit=0, port, cntr, maxCntr;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    if ('i' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(hal_getFlowCtrlIgrPortUsedPageCnt(unit, port, &cntr, &maxCntr), ret);
            diag_util_mprintf("Port %2d\n", port);
            diag_util_mprintf("\tIngress Used Page Count : %d\n", cntr);
            diag_util_mprintf("\tIngress Max Used Page Count : %d\n", maxCntr);
        }
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(hal_getFlowCtrlEgrPortUsedPageCnt(unit, port, &cntr, &maxCntr), ret);
            diag_util_mprintf("Port %2d\n", port);
            diag_util_mprintf("\tEgress Used Page Count : %d\n", cntr);
            diag_util_mprintf("\tEgress Max Used Page Count : %d\n", maxCntr);
        }
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_GET_FLOWCTRL_USED_PAGE_CNT_SYSTEM
/*
 * debug get flowctrl used-page-cnt system
 */
cparser_result_t cparser_cmd_debug_get_flowctrl_used_page_cnt_system(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      cntr, maxCntr;
    int32       ret = RT_ERR_FAILED;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    rtk_flowctrl_drop_thresh_t thresh;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_flowctrl_egrSystemDropThresh_get(unit, &thresh), ret);
        diag_util_printf("System Drop Threshold : %u\n", thresh.high);
    }
#endif

    DIAG_UTIL_ERR_CHK(hal_getFlowCtrlSystemUsedPageCnt(unit, &cntr, &maxCntr), ret);

    diag_util_mprintf("System Used Page Count : %d\n", cntr);
    diag_util_mprintf("System Max Used Page Count : %d\n", maxCntr);

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(hal_getFlowCtrlSystemPublicResource_get(unit, &cntr), ret);
        diag_util_mprintf("System Total Public Page : %d\n", cntr);

        DIAG_UTIL_ERR_CHK(hal_getFlowCtrlSystemPublicUsedPageCnt(unit, &cntr, &maxCntr), ret);

        diag_util_mprintf("System Public Used Page Count : %d\n", cntr);
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            diag_util_mprintf("System Max Public Used Page Count : %d\n", maxCntr);
        }
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_GET_FLOWCTRL_USED_PAGE_CNT_SYSTEM_INGRESS_QUEUE
/*
 * debug get flowctrl used-page-cnt system ingress-queue
 */
cparser_result_t cparser_cmd_debug_get_flowctrl_used_page_cnt_system_ingress_queue(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_dbg_queue_usedPageCnt_t qCntr, qMaxCntr;

    osal_memset(&qCntr, 0, sizeof(qCntr));
    osal_memset(&qMaxCntr, 0, sizeof(qMaxCntr));

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_getFlowCtrlSystemIgrQueueUsedPageCnt(unit, &qCntr, &qMaxCntr), ret);
    diag_util_mprintf("System Admit Queue Used Page Count : %d\n", qCntr.cntr[0]);
    diag_util_mprintf("System Admit Queue Max Used Page Count : %d\n", qMaxCntr.cntr[0]);
    diag_util_mprintf("System Drop Queue Used Page Count : %d\n", qCntr.cntr[1]);
    diag_util_mprintf("System Drop Queue Max Used Page Count : %d\n", qMaxCntr.cntr[1]);

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_GET_FLOWCTRL_USED_PAGE_CNT_QUEUE_BASED_PORT_PORTS_ALL
/*
 * debug get flowctrl used-page-cnt queue-based port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_debug_get_flowctrl_used_page_cnt_queue_based_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit=0, port = 0;
    rtk_qid_t   queue, qid_max;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    rtk_qid_t   uplinkQid_max = 0;
    rtk_qid_t   cpuQid_max = 0;
    rtk_switch_devInfo_t devInfo;
#endif
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_dbg_queue_usedPageCnt_t qCntr, qMaxCntr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&qCntr, 0, sizeof(qCntr));
    osal_memset(&qMaxCntr, 0, sizeof(qMaxCntr));

    DIAG_OM_GET_CHIP_CAPACITY(unit, qid_max, max_num_of_queue);

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
        if (rtk_switch_deviceInfo_get(unit, &devInfo) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }

        DIAG_OM_GET_CHIP_CAPACITY(unit, uplinkQid_max, max_num_of_stackQueue);
        DIAG_OM_GET_CHIP_CAPACITY(unit, cpuQid_max, max_num_of_cpuQueue);
    }
#endif

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(hal_getFlowCtrlPortQueueUsedPageCnt(unit, port, &qCntr, &qMaxCntr), ret);
        diag_util_mprintf("Port %2d\n", port);

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            if (RTK_PORTMASK_IS_PORT_SET(devInfo.uplink.portmask, port))
            {
                qid_max = uplinkQid_max;
            }
            if (devInfo.cpuPort==port)
            {
                qid_max = cpuQid_max;
            }
        }
#endif
        for (queue = 0; queue < qid_max; queue++)
        {
            diag_util_mprintf("\tQueue %d Used Page Count : %d\n", queue, qCntr.cntr[queue]);
            diag_util_mprintf("\tQueue %d Max Used Page Count : %d\n", queue, qMaxCntr.cntr[queue]);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_GET_FLOWCTRL_USED_PAGE_CNT_EGRESS_INGRESS_PORT_PORTS_ALL_QUEUE
/*
 * debug get flowctrl used-page-cnt ( egress | ingress ) port ( <PORT_LIST:ports> | all ) queue
 */
cparser_result_t cparser_cmd_debug_get_flowctrl_used_page_cnt_egress_ingress_port_ports_all_queue(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit=0, port = 0;
    rtk_qid_t   queue, qid_max, igrQid_max;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_dbg_queue_usedPageCnt_t qCntr, qMaxCntr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&qCntr, 0, sizeof(qCntr));
    osal_memset(&qMaxCntr, 0, sizeof(qMaxCntr));

    DIAG_OM_GET_CHIP_CAPACITY(unit, qid_max, max_num_of_queue);
    DIAG_OM_GET_CHIP_CAPACITY(unit, igrQid_max, max_num_of_igrQueue);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    if ('e' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(hal_getFlowCtrlPortQueueUsedPageCnt(unit, port, &qCntr, &qMaxCntr), ret);
            diag_util_mprintf("Port %2d\n", port);
            for (queue = 0; queue < qid_max; queue++)
            {
                diag_util_mprintf("\tQueue %d Used Page Count : %d\n", queue, qCntr.cntr[queue]);
                diag_util_mprintf("\tQueue %d Max Used Page Count : %d\n", queue, qMaxCntr.cntr[queue]);
            }
        }
    }
    else if ('i' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(hal_getFlowCtrlPortIgrQueueUsedPageCnt(unit, port, &qCntr, &qMaxCntr), ret);
            diag_util_mprintf("Port %2d\n", port);
            for (queue = 0; queue < igrQid_max; queue++)
            {
                diag_util_mprintf("\tIngress Queue %d Used Page Count : %d\n", queue, qCntr.cntr[queue]);
                diag_util_mprintf("\tIngress Queue %d Max Used Page Count : %d\n", queue, qMaxCntr.cntr[queue]);
            }
        }
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_GET_FLOWCTRL_USED_PAGE_CNT_REPLICATION_QUEUE
/*
 * debug get flowctrl used-page-cnt replication-queue
 */
cparser_result_t cparser_cmd_debug_get_flowctrl_used_page_cnt_replication_queue(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_dbg_repctQ_CntrInfo_t repctCntr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_getFlowCtrlRepctQueueCntrInfo(unit, &repctCntr), ret);

    diag_util_mprintf("Non-Replication-Queue Used Page Count : %d\n", repctCntr.nonRepctCntr);
    diag_util_mprintf("Non-Replication-Queue Max Used Page Count : %d\n", repctCntr.nonRepctCntrMax);
    diag_util_mprintf("Replication-Queue Used Page Count : %d\n", repctCntr.repctCntr);
    diag_util_mprintf("Replication-Queue Max Used Page Count : %d\n\n", repctCntr.repctCntrMax);

    diag_util_mprintf("Flow-Control ON Used Page Count : %d\n", repctCntr.repctFCOnCntr);
    diag_util_mprintf("Flow-Control ON Max Used Page Count : %d\n", repctCntr.repctFCOnCntrMax);
    diag_util_mprintf("Flow-Control OFF Used Page Count : %d\n", repctCntr.repctFCOffCntr);
    diag_util_mprintf("Flow-Control OFF Max Used Page Count : %d\n", repctCntr.repctFCOffCntrMax);
    diag_util_mprintf("Flow-Control OFF Drop Packet Count : %d\n\n", repctCntr.repctFCOffDropPktCntr);

    diag_util_mprintf("Extra-HSA Packet Count : %d\n", repctCntr.extraHsaPktCntr);
    diag_util_mprintf("Extra-HSA Max Packet Count : %d\n", repctCntr.extraHsaPktCntrMax);

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_GET_REPLICATION_QUEUE_STATUS
/*
 * debug get replication-queue status
 */
cparser_result_t cparser_cmd_debug_get_replication_queue_status(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_repctQueueEmptyStatus_get(unit, &enable), ret);
    diag_util_mprintf("Replication Queue Status : %s\n", enable ? "Empty" : "Non-Empty");

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_GET_REPLICATION_STICK_FETCH_STATE
/*
 * debug get replication-queue ( stick | fetch ) state
 */
cparser_result_t cparser_cmd_debug_get_replication_queue_stick_fetch_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(hal_repctQueueStickEnable_get(unit, &enable), ret);
        diag_util_mprintf("Replication Queue Stick Function Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(hal_repctQueueFetchEnable_get(unit, &enable), ret);
        diag_util_mprintf("Replication Queue Fetch Function Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_RESET_FLOWCTRL_USED_PAGE_CNT_EGRESS_INGRESS_PORT_PORTS_ALL
/*
 * debug reset flowctrl used-page-cnt ( egress | ingress ) port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_debug_reset_flowctrl_used_page_cnt_egress_ingress_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit=0, port;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    if ('i' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(hal_resetFlowCtrlIgrPortUsedPageCnt(unit, port), ret);
        }
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(hal_resetFlowCtrlEgrPortUsedPageCnt(unit, port), ret);
        }
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_RESET_FLOWCTRL_USED_PAGE_CNT_SYSTEM
/*
 * debug reset flowctrl used-page-cnt system
 */
cparser_result_t cparser_cmd_debug_reset_flowctrl_used_page_cnt_system(cparser_context_t *context)
{
    uint32      unit;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_resetFlowCtrlSystemUsedPageCnt(unit), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_RESET_FLOWCTRL_USED_PAGE_CNT_REPLICATION_QUEUE
/*
 * debug reset flowctrl used-page-cnt replication-queue
 */
cparser_result_t cparser_cmd_debug_reset_flowctrl_used_page_cnt_replication_queue(cparser_context_t *context)
{
    uint32      unit;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_resetFlowCtrlRepctQueueUsedPageCnt(unit), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_SET_REPLICATION_STICK_FETCH_STATE_DISABLE_ENABLE
/*
 * debug set replication-queue ( stick | fetch ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_debug_set_replication_queue_stick_fetch_state_disable_enable(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('s' == TOKEN_CHAR(3,0))
    {
        if ('e' == TOKEN_CHAR(5,0))
        {
            DIAG_UTIL_ERR_CHK(hal_repctQueueStickEnable_set(unit, ENABLED), ret);
        }
        else if ('d' == TOKEN_CHAR(5,0))
        {
            DIAG_UTIL_ERR_CHK(hal_repctQueueStickEnable_set(unit, DISABLED), ret);
        }
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        if ('e' == TOKEN_CHAR(5,0))
        {
            DIAG_UTIL_ERR_CHK(hal_repctQueueFetchEnable_set(unit, ENABLED), ret);
        }
        else if ('d' == TOKEN_CHAR(5,0))
        {
            DIAG_UTIL_ERR_CHK(hal_repctQueueFetchEnable_set(unit, DISABLED), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_FLASHTEST_MTD_MTD_IDX
#define TEST_PATTERN_SIZE   16
char flash_test_pattern[TEST_PATTERN_SIZE+1] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x8, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x0};
char flash_erase_verify_pattern[TEST_PATTERN_SIZE+1] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0};

void pattern_display(char * buf)
{
    int i = 0;
    diag_util_mprintf_init();

    for(i = 0; i < TEST_PATTERN_SIZE; i++)
    {
        if((i%8) == 0)
            diag_util_mprintf("\n");
        diag_util_mprintf(" 0x%02x", (uint8)*(buf+i));
    }
    diag_util_mprintf("\n");
    diag_util_mprintf_init();
}

cparser_result_t cparser_cmd_debug_flashtest_mtd_mtd_idx(cparser_context_t *context, uint32_t *mtd_idx_ptr)
{
    uint32      unit;
    char        test_mtd[32];
    char *buf_begin = NULL;
    char *buf_end = NULL;
    int32 fd = -1, mtd_size = 0, test_result = 0;
    mtd_info_t mtd_info;
    erase_info_t erase_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(test_mtd, 0, 32);

    if((*mtd_idx_ptr) == 0)
    {
        diag_util_mprintf("MTD %d is Loader; Cannot use then command\n", (*mtd_idx_ptr));
        return CPARSER_OK;
    }

    sprintf(test_mtd,"/dev/mtdchar%d",(*mtd_idx_ptr)*2);
    if ((fd = open(test_mtd, O_RDONLY)) < 0)
    {
        diag_util_mprintf("Cannot open MTD device : %s\n", test_mtd);
        return RT_ERR_FAILED;
    }

    /* Get MTD partition size information */
    if (ioctl(fd, MEMGETINFO, &mtd_info) < 0)
    {
        goto fail_ret;
    }
    diag_util_mprintf("Target MTD is %s(mtdchar ID should %d * 2), size is 0x%x\n\n", test_mtd, *mtd_idx_ptr, mtd_info.size);

    mtd_size = mtd_info.size;

    buf_begin = osal_alloc(TEST_PATTERN_SIZE+1);
    if (buf_begin == NULL)
    {
        goto fail_ret;
    }
    osal_memset(buf_begin, 0, TEST_PATTERN_SIZE+1);
    buf_end = osal_alloc(TEST_PATTERN_SIZE+1);
    if (buf_end == NULL)
    {
        goto fail_ret;
    }
    osal_memset(buf_end, 0, TEST_PATTERN_SIZE+1);

    close(fd);

    /* ========== 1st DUMMY READ TEST PROCESS ========== */
    if ((fd = open(test_mtd, O_RDWR)) < 0)
    {
        diag_util_mprintf("Cannot open MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Read flash begin*/
    if (read(fd, buf_begin, TEST_PATTERN_SIZE) != TEST_PATTERN_SIZE)
    {
        diag_util_mprintf("Cannot read MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Seek fd point to MTD partition last TEST_PATTERN_SIZE position */
    if (lseek(fd, (mtd_size - TEST_PATTERN_SIZE), SEEK_SET) < 0)
    {
        diag_util_mprintf("Cannot write MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Read flash end*/
    if (read(fd, buf_end, TEST_PATTERN_SIZE) != TEST_PATTERN_SIZE)
    {
        diag_util_mprintf("Cannot read MTD device : %s\n", test_mtd);
        goto fail_ret;
    }
    close(fd);

    diag_util_mprintf_init();
    diag_util_mprintf("\n\n[1] DUMMY Read: ");
    diag_util_mprintf("\n[1] Read back pattern (BEGIN): ");
    pattern_display(buf_begin);
    diag_util_mprintf("[1] Read back pattern (END): ");
    pattern_display(buf_end);

    /* ========== 2nd ERASE TEST PROCESS========== */

    if ((fd = open(test_mtd, O_RDWR)) < 0)
    {
        diag_util_mprintf("Cannot open MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Erase flash */
     erase_info.start = 0x0;
     erase_info.length = mtd_info.size;
     if (ioctl(fd, MEMERASE, &erase_info) < 0)
     {
         diag_util_mprintf("Cannot erase MTD device : %s\n", test_mtd);
         goto fail_ret;
     }
     /* Read flash begin*/
     if (read(fd, buf_begin, TEST_PATTERN_SIZE) != TEST_PATTERN_SIZE)
     {
         diag_util_mprintf("Cannot read MTD device : %s\n", test_mtd);
         goto fail_ret;
     }

     /* Seek fd point to MTD partition last TEST_PATTERN_SIZE position */
     if (lseek(fd, (mtd_size - TEST_PATTERN_SIZE), SEEK_SET) < 0)
     {
         diag_util_mprintf("Cannot write MTD device : %s\n", test_mtd);
         goto fail_ret;
     }

     /* Read flash end*/
     if (read(fd, buf_end, TEST_PATTERN_SIZE) != TEST_PATTERN_SIZE)
     {
         diag_util_mprintf("Cannot read MTD device : %s\n", test_mtd);
         goto fail_ret;
     }

    close(fd);

    diag_util_mprintf("\n[2] ERASE TEST:   RESULT :");
    if(osal_memcmp(buf_begin, flash_erase_verify_pattern, TEST_PATTERN_SIZE) != 0)
    {
        test_result = 1;
        diag_util_mprintf("(BEGIN Failed!!!)");
    }else{
        diag_util_mprintf("(BEGIN Success)");
    }

    if(osal_memcmp(buf_end, flash_erase_verify_pattern, TEST_PATTERN_SIZE) != 0)
    {
        test_result = 1;
        diag_util_mprintf("(END Failed!!!)\n");
    }else{
        diag_util_mprintf("(END Success)\n");
    }

    diag_util_mprintf_init();
    diag_util_mprintf("[2] Read back pattern (BEGIN): ");
    pattern_display(buf_begin);
    diag_util_mprintf("[2] Read back pattern (END): ");
    pattern_display(buf_end);

    /* ========== 3rd WRITE TEST PROCESS========== */

    /* Re-open*/
    if ((fd = open(test_mtd, O_RDWR)) < 0)
    {
        diag_util_mprintf("Cannot open MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Write test pattern into MTD partition in BEGIN, make sure write function */
    if (write(fd, flash_test_pattern, TEST_PATTERN_SIZE) < 0)
    {
        diag_util_mprintf("Cannot write MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Seek fd point to MTD partition last TEST_PATTERN_SIZE position */
    if (lseek(fd, (mtd_size - TEST_PATTERN_SIZE), SEEK_SET) < 0)
    {
        diag_util_mprintf("Cannot write MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Write test pattern into MTD partition in END, make sure write function */
    if (write(fd, flash_test_pattern, TEST_PATTERN_SIZE) < 0)
    {
        diag_util_mprintf("Cannot write MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    close(fd);

    /* Re-open*/
    if ((fd = open(test_mtd, O_RDWR)) < 0)
    {
        diag_util_mprintf("Cannot open MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

     /* Read flash begin*/
     if (read(fd, buf_begin, TEST_PATTERN_SIZE) != TEST_PATTERN_SIZE)
     {
         diag_util_mprintf("Cannot read MTD device : %s\n", test_mtd);
         goto fail_ret;
     }

     /* Seek fd point to MTD partition last TEST_PATTERN_SIZE position */
     if (lseek(fd, (mtd_size - TEST_PATTERN_SIZE), SEEK_SET) < 0)
     {
         diag_util_mprintf("Cannot write MTD device : %s\n", test_mtd);
         goto fail_ret;
     }

     /* Read flash end*/
     if (read(fd, buf_end, TEST_PATTERN_SIZE) != TEST_PATTERN_SIZE)
     {
         diag_util_mprintf("Cannot read MTD device : %s\n", test_mtd);
         goto fail_ret;
     }
    close(fd);

    diag_util_mprintf("\n[3] WRITE TEST:   RESULT :");
    if(osal_memcmp(buf_begin, flash_test_pattern, TEST_PATTERN_SIZE) != 0)
    {
        test_result = 1;
        diag_util_mprintf("(BEGIN Failed!!!)");
    }else{
        diag_util_mprintf("(BEGIN Success)");
    }

    if(osal_memcmp(buf_end, flash_test_pattern, TEST_PATTERN_SIZE) != 0)
    {
        test_result = 1;
        diag_util_mprintf("(END Failed!!!)\n");
    }else{
        diag_util_mprintf("(END Success)\n");
    }

    diag_util_mprintf_init();
    diag_util_mprintf("[3] Read back pattern (BEGIN): ");
    pattern_display(buf_begin);
    diag_util_mprintf("[3] Read back pattern (END): ");
    pattern_display(buf_end);

    /* ========== 4rd ERASE TEST PROCESS========== */

    /* Re-open*/
    if ((fd = open(test_mtd, O_RDWR)) < 0)
    {
        diag_util_mprintf("Cannot open MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Erase Flash, and make sure Erase function*/
    if (ioctl(fd, MEMERASE, &erase_info) < 0)
    {
         diag_util_mprintf("Cannot erase MTD device : %s\n", test_mtd);
         goto fail_ret;
    }

    close(fd);

    if ((fd = open(test_mtd, O_RDWR)) < 0)
    {
        diag_util_mprintf("Cannot open MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Read flash begin*/
    if (read(fd, buf_begin, TEST_PATTERN_SIZE) != TEST_PATTERN_SIZE)
    {
        diag_util_mprintf("Cannot read MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Seek fd point to MTD partition last TEST_PATTERN_SIZE position */
    if (lseek(fd, (mtd_size - TEST_PATTERN_SIZE), SEEK_SET) < 0)
    {
        diag_util_mprintf("Cannot write MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    /* Read flash end*/
    if (read(fd, buf_end, TEST_PATTERN_SIZE) != TEST_PATTERN_SIZE)
    {
        diag_util_mprintf("Cannot read MTD device : %s\n", test_mtd);
        goto fail_ret;
    }

    close(fd);

    diag_util_mprintf("\n[4] ERASE TEST:   RESULT :");
    if(osal_memcmp(buf_begin, flash_erase_verify_pattern, TEST_PATTERN_SIZE) != 0)
    {
        test_result = 1;
        diag_util_mprintf("(BEGIN Failed!!!)");
    }else{
        diag_util_mprintf("(BEGIN Success)");
    }

    if(osal_memcmp(buf_end, flash_erase_verify_pattern, TEST_PATTERN_SIZE) != 0)
    {
        test_result = 1;
        diag_util_mprintf("(END Failed!!!)\n");
    }else{
        diag_util_mprintf("(END Success)\n");
    }

    diag_util_mprintf_init();
    diag_util_mprintf("[4] Read back pattern (BEGIN): ");
    pattern_display(buf_begin);
    diag_util_mprintf("[4] Read back pattern (END): ");
    pattern_display(buf_end);

    if(test_result == 1)
        goto fail_test;

    osal_free(buf_begin);
    osal_free(buf_end);

    diag_util_mprintf_init();
    diag_util_mprintf("\nFlash driver test SUCCESSED !!!\n");

    return CPARSER_OK;


  fail_ret:
    if (fd >= 0)
    {
        close(fd);
    }

  fail_test:
    if (buf_begin)
        osal_free(buf_begin);

    if (buf_end)
        osal_free(buf_end);

    diag_util_mprintf_init();
    diag_util_mprintf("\nFlash driver test FAILED !!!\n");

    return RT_ERR_FAILED;

}

#endif


int32 _diag_flowCtrl_dump(void)
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    uint32      unit = 0, cntr, maxCntr;
    rtk_qid_t   queue, qid_max;
    rtk_dbg_queue_usedPageCnt_t qCntr, qMaxCntr;
    rtk_port_t  port;
    int32       ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t devInfo;

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    if (rtk_switch_deviceInfo_get(unit, &devInfo) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    DIAG_UTIL_ERR_CHK(hal_getFlowCtrlSystemUsedPageCnt(unit, &cntr, &maxCntr), ret);
    diag_util_mprintf("\n===================== Flow Control Used Page Info =============================\n");
    diag_util_mprintf("\n");
    diag_util_mprintf("System Used Page Count : %d\n", cntr);
    diag_util_mprintf("System Max Used Page Count : %d\n\n", maxCntr);

    osal_memset(&qCntr, 0, sizeof(qCntr));
    osal_memset(&qMaxCntr, 0, sizeof(qMaxCntr));
    qid_max = devInfo.capacityInfo.max_num_of_queue;

    for (port = 0; port < RTK_MAX_NUM_OF_PORTS; port++)
    {
        if (RTK_PORTMASK_IS_PORT_SET((devInfo.all.portmask), port))
        {
            hal_getFlowCtrlPortQueueUsedPageCnt(unit, port, &qCntr, &qMaxCntr);
            diag_util_mprintf("Port %2d\n", port);
            for (queue = 0; queue < qid_max; queue++)
            {
                diag_util_mprintf("\tQueue %d Used Page Count : %d\n", queue, qCntr.cntr[queue]);
                diag_util_mprintf("\tQueue %d Max Used Page Count : %d\n", queue, qMaxCntr.cntr[queue]);
            }
        }
    }
    diag_util_mprintf("===============================================================================\n");
#endif
    return RT_ERR_OK;
}

/* print all chip debug informations */
int32 diag_debug_dump(void)
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    _diag_flowCtrl_dump();
#endif

    return RT_ERR_OK;
}

#ifdef CMD_DEBUG_GET_MEMORY_ALLOCATED
cparser_result_t cparser_cmd_debug_get_memory_allocated(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(debug_mem_show(unit), ret);
    return CPARSER_OK;
}
#endif


#ifdef CMD_DEBUG_GET_MEMORY_AVAILABLE
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <sys/stat.h>
#include <fcntl.h>
int32 _cparser_cmd_debug_get_memory_available_print(char *buffer, char *subStr, char *prefix)
{
    const char *str;

    diag_util_printf("%s", prefix);
    if ((str = strstr(buffer, subStr)) != NULL)
    {
        while(*str != '\n')
        {
            diag_util_printf("%c", *str);
            str++;
        }
        diag_util_printf("\n\n");
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}
#endif
#endif /* CMD_DEBUG_GET_MEMORY_AVAILABLE */

#ifdef CMD_DEBUG_GET_MEMORY_AVAILABLE
/*
 * debug get memory available
 */
cparser_result_t cparser_cmd_debug_get_memory_available(cparser_context_t *context)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    #define BUF_SIZE    2048
    char buffer[BUF_SIZE];
    int fd, ret;

    if ((fd = open("/tmp/mem_log_rtsdk_start", O_RDONLY)) >= 0)
    {
        if ((ret = read (fd, &buffer, BUF_SIZE)) > 0)
        {
            if (_cparser_cmd_debug_get_memory_available_print(buffer, "MemTotal", "") != RT_ERR_OK)
            {
                diag_util_printf("  unable to get MemTotal info\n");
            }

            diag_util_printf("Before SDK init:\n");
            if (_cparser_cmd_debug_get_memory_available_print(buffer, "MemAvailable", "    ") != RT_ERR_OK)
            {
                diag_util_printf("  unable to get MemAvailable info\n");
            }
        }
        close(fd);
    }
    else
    {
        diag_util_printf("unable to open /tmp/mem_log_rtsdk_start file\n\n");
    }

    if ((fd = open("/tmp/mem_log_rtsdk_finish", O_RDONLY)) >= 0)
    {
        if ((ret = read (fd, &buffer, BUF_SIZE)) > 0)
        {
            diag_util_printf("After SDK init:\n");
            if (_cparser_cmd_debug_get_memory_available_print(buffer, "MemAvailable", "    ") != RT_ERR_OK)
            {
                diag_util_printf("unable to get MemAvailable info\n");
            }
        }
        close(fd);
    }
    else
    {
        diag_util_printf("unable to open /tmp/mem_log_rtsdk_finish file\n\n");
    }

    if ((fd = open("/tmp/mem_log_rtsdk_diag", O_RDONLY)) >= 0)
    {
        if ((ret = read (fd, &buffer, BUF_SIZE)) > 0)
        {
            diag_util_printf("After start diag:\n");
            if (_cparser_cmd_debug_get_memory_available_print(buffer, "MemAvailable", "    ") != RT_ERR_OK)
            {
                diag_util_printf("unable to get MemAvailable info\n");
            }
        }
        close(fd);
    }
    else
    {
        diag_util_printf("unable to open /tmp/mem_log_rtsdk_diag file\n\n");
    }

    if(system("cat /proc/meminfo > /tmp/mem_log_cur")==-1)
    {
        diag_util_printf("cat /proc/meminfo failed\n");
    }

    if ((fd = open("/tmp/mem_log_cur", O_RDONLY)) >= 0)
    {
        if ((ret = read (fd, &buffer, BUF_SIZE)) > 0)
        {
            diag_util_printf("Current:\n");
            if (_cparser_cmd_debug_get_memory_available_print(buffer, "MemAvailable", "    ") != RT_ERR_OK)
            {
                diag_util_printf("unable to get MemAvailable info\n");
            }
        }
        close(fd);
    }
    else
    {
        diag_util_printf("unable to open /tmp/mem_log_cur file\n\n");
    }
    return CPARSER_OK;
#else
    diag_util_printf("not support\n");
    return CPARSER_OK;
#endif
}
#endif /* CMD_DEBUG_GET_MEMORY_AVAILABLE */

/*
 * diag set sc reg <UINT:mdx_macId> <UINT:reg_addr> <UINT:reg_data>
 */
cparser_result_t
cparser_cmd_diag_set_sc_reg_mdx_macId_reg_addr_reg_data(cparser_context_t *context,
    uint32_t *mdx_macId_ptr,
    uint32_t *reg_addr_ptr,
    uint32_t *reg_data_ptr)
{
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((mdx_macId_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((reg_addr_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((reg_data_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_rtl8295_reg_write(unit, *mdx_macId_ptr, *reg_addr_ptr, *reg_data_ptr), ret);

    diag_util_printf("    Write macId: %u Register: 0x%04X VALUE: 0x%08X", *mdx_macId_ptr, *reg_addr_ptr, *reg_data_ptr);
    diag_util_printf("\n");
    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_OK;
#endif
}

/*
 * diag get sc reg <UINT:mdx_macId> <UINT:reg_addr>
 */
cparser_result_t
cparser_cmd_diag_get_sc_reg_mdx_macId_reg_addr(cparser_context_t *context,
    uint32_t *mdx_macId_ptr,
    uint32_t *reg_addr_ptr)
{
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;
    uint32  data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((mdx_macId_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((reg_addr_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_rtl8295_reg_read(unit, *mdx_macId_ptr, *reg_addr_ptr, &data), ret);

    diag_util_printf("    Read macId: %u Register: 0x%04X VALUE: 0x%08X", *mdx_macId_ptr, *reg_addr_ptr, data);
    diag_util_printf("\n");
    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_OK;
#endif
}

/*
 * diag set sc serdes <UINT:mdx_macId> <UINT:sds_idx> <UINT:page> <UINT:reg> <UINT:data>
 */
cparser_result_t
cparser_cmd_diag_set_sc_serdes_mdx_macId_sds_idx_page_reg_data(cparser_context_t *context,
    uint32_t *mdx_macId_ptr,
    uint32_t *sds_idx_ptr,
    uint32_t *page_ptr,
    uint32_t *reg_ptr,
    uint32_t *data_ptr)
{
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((mdx_macId_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((sds_idx_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((page_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((data_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, *mdx_macId_ptr, *sds_idx_ptr, *page_ptr, *reg_ptr, *data_ptr), ret);

    diag_util_printf("    Write macId: %u Serdes: %u Page: 0x%04X Reg: 0x%02X VALUE: 0x%08X",
            *mdx_macId_ptr, *sds_idx_ptr, *page_ptr, *reg_ptr, *data_ptr);
    diag_util_printf("\n");
    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_OK;
#endif
}

/*
 * diag get sc serdes <UINT:mdx_macId> <UINT:sds_idx> <UINT:page> <UINT:reg>
 */
cparser_result_t
cparser_cmd_diag_get_sc_serdes_mdx_macId_sds_idx_page_reg(cparser_context_t *context,
    uint32_t *mdx_macId_ptr,
    uint32_t *sds_idx_ptr,
    uint32_t *page_ptr,
    uint32_t *reg_ptr)
{
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;
    uint32  data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((mdx_macId_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((sds_idx_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((page_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_read(unit, *mdx_macId_ptr, *sds_idx_ptr, *page_ptr, *reg_ptr, &data), ret);

    diag_util_printf("    Read macId: %u Serdes: %u Page: 0x%04X Reg: 0x%02X VALUE: 0x%08X",
            *mdx_macId_ptr, *sds_idx_ptr, *page_ptr, *reg_ptr, data);
    diag_util_printf("\n");
    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_OK;
#endif
}

/*
 * debug dump 8295r eye-param reg <UINT:mdx_macId> ( 8431 | daclong | <STRING:dac_type> )
 */
cparser_result_t
cparser_cmd_debug_dump_8295r_eye_param_reg_mdx_macId_8431_daclong_dac_type(cparser_context_t *context,
    uint32_t *mdx_macId_ptr)
{
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;
    uint32  data;
    int32   familyId = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    familyId = (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID)) ? RTL8390_FAMILY_ID : \
                (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID)) ? RTL8380_FAMILY_ID :\
                DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ? RTL9300_FAMILY_ID :\
                DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) ? RTL9310_FAMILY_ID : RT_ERR_FAILED;
    if (familyId == RT_ERR_FAILED)
    {
        DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
        return CPARSER_OK;
    }
    else
    {
        familyId = (familyId >> 16) & 0xFFFF;
    }

    osal_printf("phy_8295_pageRegVal_t rtl8295r_%x_s1%s_myParam[] = {\n", familyId, TOKEN_STR(6));

    //DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, *mdx_macId_ptr, 957, 23, &data), ret);
    DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_read(unit, *mdx_macId_ptr, 1, 0x2F, 0xF, &data), ret);
    osal_printf("    {957, 23, 0x%04X}, /* S1, 0x2F, 0xF. [10]pre_en; [8]post_en; */\n", data);

    //DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, *mdx_macId_ptr, 958, 16, &data), ret);
    DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_read(unit, *mdx_macId_ptr, 1, 0x2F, 0x10, &data), ret);
    osal_printf("    {958, 16, 0x%04X}, /* S1, 0x2F, 0x10. [14:10]pre_amp; [9:5]main_amp; [4:0]post_amp */\n", data);

    DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_read(unit, *mdx_macId_ptr, 0, 0x21, 0x5, &data), ret);
    osal_printf("    {388, 21, 0x%04X}, /* S0, 0x21, 0x5. [9:6]impedance */\n", data);


    osal_printf("    {PHY_8295_PAGEREGVAL_END, },\n");

    osal_printf("};\n");

    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_OK;
#endif

}

/*
 * diag set sc patch <UINT:port> <UINT:mdx_macId> <UINT:serdes> <STRING:name>
 */
cparser_result_t
cparser_cmd_diag_set_sc_patch_port_mdx_macId_serdes_name(cparser_context_t *context,
        uint32_t *port_ptr,
        uint32_t *mdx_macId_ptr,
        uint32_t *serdes_ptr,
        char **name_ptr)
{
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(phy_8295_diag_set(unit, *port_ptr, *mdx_macId_ptr, *serdes_ptr, (uint8 *)*name_ptr), ret);
    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_OK;
#endif
}

/*
 * diag set sc patch debug ( enable | disable )
 */
cparser_result_t
cparser_cmd_diag_set_sc_patch_debug_enable_disable(cparser_context_t *context)
{
#if defined(CONFIG_SDK_RTL8295) || defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
    int32           ret = RT_ERR_FAILED;
    uint32          enable = 2;
    //uint32          unit = 0;

    DIAG_UTIL_PARAM_CHK();
    //DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (TOKEN_CHAR(5, 0) == 'e')
        enable = 1;
    else if (TOKEN_CHAR(5, 0) == 'd')
        enable = 0;

    DIAG_UTIL_ERR_CHK(phy_8295_patch_debugEnable_set(enable), ret);

    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_OK;
#endif
}


extern int32 phy_8390_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);
/*
 *  diag set 8390 10g-media port ( <PORT_LIST:ports> | all ) ( fiber10g | fiber1g | none )
 */
cparser_result_t
cparser_cmd_diag_set_8390_10g_media_port_ports_all_fiber10g_fiber1g_none(cparser_context_t *context,
        char **ports_ptr)
{
#if defined(CONFIG_SDK_RTL8390)
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    rtk_port_10gMedia_t media;
    uint32              unit;
    int32               ret;
    rtk_switch_devInfo_t    devInfo;
    int32               tIdx;

    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);


    tIdx = 6; /* media mode */
    if (!strcmp(TOKEN_STR(tIdx), "fiber10g"))
    {
        media = PORT_10GMEDIA_FIBER_10G;
    }
    else if (!strcmp(TOKEN_STR(tIdx), "fiber1g"))
    {
        media = PORT_10GMEDIA_FIBER_1G;
    }
    else if (!strcmp(TOKEN_STR(tIdx), "none"))
    {
        media = PORT_10GMEDIA_NONE;
    }
    else
    {
        media = PORT_10GMEDIA_NONE;
    }

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if (!RTK_PORTMASK_IS_PORT_SET(devInfo.xge.portmask, port))
            continue;

        ret = phy_8390_10gMedia_set(unit, port, media);
        if (ret != RT_ERR_OK)
        {
            diag_util_printf("port %u error (0x%x): %s\n", port, ret, rt_error_numToStr(ret));
        }
    }

    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_OK;
#endif
}

#ifdef CMD_RTK_EYE_MONITOR_PORT_FRAMENUM
/*
 * rtk eye-monitor <UINT:port> <UINT:frameNum>
 */
cparser_result_t
cparser_cmd_rtk_eye_monitor_port_frameNum(
    cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *frameNum_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_port_phyEyeMonitor_start(unit, *port_ptr, *frameNum_ptr), ret);

    diag_util_printf("Done\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_rtk_eye_monitor_port_frameNum */
#endif


#ifdef CMD_SERDES_GET_SDSID_RXCALI_PARAM_CFG
/*
 * serdes get  <UINT:sdsId> rx-cali param-cfg
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_rx_cali_param_cfg(cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_sds_rxCaliConf_t conf;
    uint32  unit;
    int32   ret = RT_ERR_OK;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_sds_rxCaliConf_get(unit, *sdsId_ptr, &conf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    diag_util_printf("SDS %u:\n", *sdsId_ptr);
    #if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        diag_util_printf(" %-20s : %u,\n", "dacLongCableOffset", conf.dacLongCableOffset);
        diag_util_printf(" %-20s : 0x%X,\n", "tap0InitVal", conf.tap0_init_val);
        diag_util_printf(" %-20s : 0x%x,\n", "vthMinThr", conf.vth_min);
        diag_util_printf(" %-20s : %s,\n", "eqHoldEnable", (conf.eqHoldEnable==ENABLED)?"ENABLED":"DISABLED");
    }
    #endif  /* CONFIG_SDK_RTL9300 */

    diag_util_printf(" %-20s : %s,\n", "dfeTap1_4Enable", (conf.dfeTap1_4Enable==ENABLED)?"ENABLED":"DISABLED");

    #if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_printf(" %-20s : %s,\n", "dfe_auto", (conf.dfeAuto==ENABLED)?"ENABLED":"DISABLED");
        diag_util_printf(" %-20s : %u,\n", "offset", conf.ofst);
        diag_util_printf(" %-20s : %s,\n", "leq_auto", (conf.leqAuto==ENABLED)?"ENABLED":"DISABLED");
    }
    #endif  /* CONFIG_SDK_RTL9310 */

    return CPARSER_OK;

}
#endif

#ifdef CMD_SERDES_SET_SDSID_RXCALI_PARAM_CFG_DAC_LONG_CABLE_OFFSET_OFFSET
/*
 * serdes set  <UINT:sdsId> rx-cali param-cfg dac-long-cable-offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_dac_long_cable_offset_offset(cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *offset_ptr)
{
    rtk_sds_rxCaliConf_t conf;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_sds_rxCaliConf_get(unit, *sdsId_ptr, &conf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    conf.dacLongCableOffset = *offset_ptr;
    if ((ret = rtk_sds_rxCaliConf_set(unit, *sdsId_ptr, conf)) != RT_ERR_OK)
    {
        diag_util_printf("Set Serdes %u RX-Cali. config failed: 0x%x\n", *sdsId_ptr, ret);
    }

    return CPARSER_OK;

}
#endif

#ifdef CMD_SERDES_SET_SDSID_RXCALI_PARAM_CFG_OFFSET_OFFSET
/*
 * serdes set  <UINT:sdsId> rx-cali param-cfg offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_offset_offset(cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *offset_ptr)
{
    rtk_sds_rxCaliConf_t conf;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_sds_rxCaliConf_get(unit, *sdsId_ptr, &conf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    conf.ofst = *offset_ptr;
    if ((ret = rtk_sds_rxCaliConf_set(unit, *sdsId_ptr, conf)) != RT_ERR_OK)
    {
        diag_util_printf("Set Serdes %u RX-Cali. config failed: 0x%x\n", *sdsId_ptr, ret);
    }

    return CPARSER_OK;

}   /* end of cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_offset_offset */
#endif

#ifdef CMD_SERDES_SET_SDSID_RXCALI_PARAM_CFG_VTH_MIN_THRESHOLD_TAP0_INIT_VALUE
/*
 * serdes set <UINT:sdsId> rx-cali param-cfg vth-min <UINT:threshold> tap0-init <UINT:value>
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_vth_min_threshold_tap0_init_value(cparser_context_t *context,
    uint32_t *sdsId_ptr,
    uint32_t *threshold_ptr,
    uint32_t *value_ptr)
{
    rtk_sds_rxCaliConf_t conf;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_sds_rxCaliConf_get(unit, *sdsId_ptr, &conf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    conf.vth_min = *threshold_ptr;
    conf.tap0_init_val = *value_ptr;
    if ((ret = rtk_sds_rxCaliConf_set(unit, *sdsId_ptr, conf)) != RT_ERR_OK)
    {
        diag_util_printf("Set Serdes %u RX-Cali. config failed: 0x%x\n", *sdsId_ptr, ret);
    }

    return CPARSER_OK;

}
#endif

#ifdef CMD_SERDES_SET_SDSID_RXCALI_PARAM_CFG_EQ_HOLD_ENABLE_DISABLE_DFE_TAP1_4_ENABLE_DISABLE
/*
 * serdes set  <UINT:sdsId> rx-cali param-cfg eq-hold ( enable | disable ) dfe-tap1-4 ( enable | disable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_eq_hold_enable_disable_dfe_tap1_4_enable_disable(cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_sds_rxCaliConf_t conf;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_sds_rxCaliConf_get(unit, *sdsId_ptr, &conf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    if ('e' == TOKEN_CHAR(6, 0))
        conf.eqHoldEnable = ENABLED;
    else
        conf.eqHoldEnable = DISABLED;

    if ('e' == TOKEN_CHAR(8, 0))
        conf.dfeTap1_4Enable= ENABLED;
    else
        conf.dfeTap1_4Enable = DISABLED;

    if ((ret = rtk_sds_rxCaliConf_set(unit, *sdsId_ptr, conf)) != RT_ERR_OK)
    {
        diag_util_printf("Set Serdes %u RX-Cali. config failed: 0x%x\n", *sdsId_ptr, ret);
    }

    return CPARSER_OK;

}
#endif

#ifdef CMD_SERDES_SET_SDSID_RXCALI_PARAM_CFG_DFE_TAP1_4_ENABLE_DISABLE
/*
 * serdes set <UINT:sdsId> rx-cali param-cfg dfe-tap1-4 ( enable | disable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_dfe_tap1_4_enable_disable(cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_sds_rxCaliConf_t conf;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_sds_rxCaliConf_get(unit, *sdsId_ptr, &conf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    if ('e' == TOKEN_CHAR(6, 0))
        conf.dfeTap1_4Enable= ENABLED;
    else
        conf.dfeTap1_4Enable = DISABLED;

    if ((ret = rtk_sds_rxCaliConf_set(unit, *sdsId_ptr, conf)) != RT_ERR_OK)
    {
        diag_util_printf("Set Serdes %u RX-Cali. config failed: 0x%x\n", *sdsId_ptr, ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_dfe_tap1_4_enable_disable */
#endif

#ifdef CMD_SERDES_SET_SDSID_RXCALI_PARAM_CFG_DFE_AUTO_ENABLE_DISABLE
/*
 * serdes set <UINT:sdsId> rx-cali param-cfg dfe-auto ( enable | disable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_dfe_auto_enable_disable(cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_sds_rxCaliConf_t conf;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_sds_rxCaliConf_get(unit, *sdsId_ptr, &conf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    if ('e' == TOKEN_CHAR(6, 0))
        conf.dfeAuto= ENABLED;
    else
        conf.dfeAuto = DISABLED;

    if ((ret = rtk_sds_rxCaliConf_set(unit, *sdsId_ptr, conf)) != RT_ERR_OK)
    {
        diag_util_printf("Set Serdes %u RX-Cali. config failed: 0x%x\n", *sdsId_ptr, ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_dfe_auto_enable_disable */
#endif

#ifdef CMD_SERDES_SET_SDSID_RXCALI_PARAM_CFG_LEQ_AUTO_ENABLE_DISABLE
/*
 * serdes set <UINT:sdsId> rx-cali param-cfg leq-auto ( enable | disable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_leq_auto_enable_disable(cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_sds_rxCaliConf_t conf;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_sds_rxCaliConf_get(unit, *sdsId_ptr, &conf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    if ('e' == TOKEN_CHAR(6, 0))
        conf.leqAuto = ENABLED;
    else
        conf.leqAuto = DISABLED;

    if ((ret = rtk_sds_rxCaliConf_set(unit, *sdsId_ptr, conf)) != RT_ERR_OK)
    {
        diag_util_printf("Set Serdes %u RX-Cali. config failed: 0x%x\n", *sdsId_ptr, ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_serdes_set_sdsId_rx_cali_param_cfg_leq_auto_enable_disable */
#endif

#ifdef CMD_SERDES_GET_SDSID_RXCALI_STATE
/*
 * serdes get <UINT:sdsId> rx-cali  state
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_rx_cali_state(cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_enable_t enable;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(mac_debug_sds_rxCaliEnable_get(unit, *sdsId_ptr, &enable), ret);
    diag_util_printf("Serdes %u rxCali State: %s\n", *sdsId_ptr, (enable == ENABLED) ? "enable" : "disabled");

    return CPARSER_OK;
}
#endif

#ifdef CMD_SERDES_SET_SDSID_RXCALI_STATE_DISABLE_ENABLE
/*
 * serdes set <UINT:sdsId> rx-cali  state ( disable | enable )
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_rx_cali_state_disable_enable(cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(5, 0))
        DIAG_UTIL_ERR_CHK(mac_debug_sds_rxCaliEnable_set(unit, *sdsId_ptr, ENABLED),ret);
    else
        DIAG_UTIL_ERR_CHK(mac_debug_sds_rxCaliEnable_set(unit, *sdsId_ptr, DISABLED), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SERDES_GET_SDSID_RXCALI_STATUS
/*
 * serdes get <UINT:sdsId> rx-cali  status
 */
cparser_result_t
cparser_cmd_serdes_get_sdsId_rx_cali_status(cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    rtk_port_phySdsRxCaliStatus_t status;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(mac_debug_sds_rxCaliStatus_get(unit, *sdsId_ptr, &status), ret);
    diag_util_printf("Serdes %u rxCali Status: %s\n", *sdsId_ptr, text_sds_rxcali_status[status].text);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SERDES_SET_SDSID_RXCALI_START
/*
 * serdes set <UINT:sdsId> rx-cali  start
 */
cparser_result_t
cparser_cmd_serdes_set_sdsId_rx_cali_start(cparser_context_t *context,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(mac_debug_sds_rxCali(unit,  *sdsId_ptr, 0), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_SERDES_SET_RX_CALI_DEBUG_ENABLE_DISABLE
/*
 * serdes set rx-cali debug ( enable | disable )
 */
cparser_result_t
cparser_cmd_serdes_set_rx_cali_debug_enable_disable(cparser_context_t *context)
{
    rtk_enable_t enable;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (TOKEN_CHAR(4, 0) == 'e')
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(mac_debug_sds_rxCali_debugEnable_set(enable), ret);
    return CPARSER_OK;
}
#endif


#ifdef CMD_DEBUG_GET_IMAGE_INFO
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
void
_cparser_cmd_debug_get_image_info_uboot(cparser_context_t *context)
{
    #define RTK_FLSH_LOADER_MTDDEV          "/dev/mtdchar0"
    #define LOADER_TAIL "#-TAIL-#" /* LOADER_TAIL */
    typedef struct loader_tail_s {
        char     lt_key[8];  /* key string #-TAIL-# */
        uint32_t lt_chip;    /* chip family */
        uint32_t lt_crc;     /* loader image crc */
        uint32_t lt_size;    /* loader image size */
        uint32_t lt_tcrc;    /* loader tail crc */
    } loader_tail_t;

    char *pFree = NULL;
    char *tmp_buf = NULL;
    const char *str;
    int32 fd, i;
    uint32 size = 0;
    mtd_info_t mtd_info;
    loader_tail_t   *loader_tail;

    diag_util_printf("U-Boot:\n");
    if ((fd = open(RTK_FLSH_LOADER_MTDDEV, O_RDONLY)) < 0)
    {
        diag_util_printf("  unable to open %s \n", RTK_FLSH_LOADER_MTDDEV);
        return;
    }

    /* Get MTD partition size information */
    if (ioctl(fd, MEMGETINFO, &mtd_info) < 0)
    {
        close(fd);
        diag_util_printf("  unable to read MTD info of %s \n", RTK_FLSH_LOADER_MTDDEV);
        return;
    }

    if ((size = mtd_info.size) <= 0)
    {
        close(fd);
        diag_util_printf("  unable to read. mtd_info.size is 0 \n");
        return;
    }
    //diag_util_printf("  mtd_info.size=%u\n", mtd_info.size);

    tmp_buf = malloc(size);
    if (tmp_buf == NULL)
    {
        close(fd);
        diag_util_printf("  no memory\n");
        return;
    }
    pFree = tmp_buf;

    if (read(fd, tmp_buf, size) < 0)
    {
        close(fd);
        free(pFree);
        diag_util_printf("  read fail \n");
        return;
    }
    close(fd);

    str = NULL;
    for (i = 0; i < size; i++)
    {
        //"#-TAIL-#"
        if (tmp_buf[i] == '#')
        {
            if (!strncmp(&tmp_buf[i], LOADER_TAIL, strlen(LOADER_TAIL)))
            {
                str = &tmp_buf[i];
            }
        }
    }

    //str = strstr(tmp_buf, LOADER_TAIL);
    if (str == NULL)
    {
        diag_util_printf("  unable to fine loader tail: %s\n", LOADER_TAIL);
        goto EXIT;
    }

    loader_tail = (loader_tail_t *)str;
    diag_util_printf("  size       : %u\n", loader_tail->lt_size);
    diag_util_printf("  tail size  : %x\n", sizeof(loader_tail_t));
    diag_util_printf("  chip family: %x\n", loader_tail->lt_chip);
    diag_util_printf("\n");

  EXIT:
    if (pFree)
    {
        free(pFree);
    }

    return;
}

void
_cparser_cmd_debug_get_image_info_runtime(cparser_context_t *context)
{
    #define RTK_FLSH_RUNTIME1_MTDDEV            "/dev/mtdchar10"
    #define IH_NMLEN                            32
    typedef struct image_header {
        uint32_t    ih_magic;   /* Image Header Magic Number    */
        uint32_t    ih_hcrc;    /* Image Header CRC Checksum    */
        uint32_t    ih_time;    /* Image Creation Timestamp */
        uint32_t    ih_size;    /* Image Data Size      */
        uint32_t    ih_load;    /* Data  Load  Address      */
        uint32_t    ih_ep;      /* Entry Point Address      */
        uint32_t    ih_dcrc;    /* Image Data CRC Checksum  */
        uint8_t     ih_os;      /* Operating System     */
        uint8_t     ih_arch;    /* CPU architecture     */
        uint8_t     ih_type;    /* Image Type           */
        uint8_t     ih_comp;    /* Compression Type     */
        uint8_t     ih_name[IH_NMLEN];  /* Image Name       */
    } image_header_t;

    int fd;
    image_header_t ih;

    diag_util_printf("Runtime1:\n");

    if ((fd = open(RTK_FLSH_RUNTIME1_MTDDEV, O_RDONLY)) < 0)
    {
        diag_util_printf("  unable to open %s\n\n", RTK_FLSH_RUNTIME1_MTDDEV);
        return;
    }
    if (read(fd, &ih, sizeof(image_header_t)) < 0)
    {
        close(fd);
        diag_util_printf("  unable to read header info\n\n");
        return;
    }
    close(fd);

    diag_util_printf("  size       : %u byte \n", ih.ih_size);
    diag_util_printf("  header size: %u \n", sizeof(image_header_t));

    diag_util_printf("\n");
    return;
}
#endif

/*
 * debug get image info
 */
cparser_result_t cparser_cmd_debug_get_image_info(cparser_context_t *context)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    _cparser_cmd_debug_get_image_info_uboot(context);
    _cparser_cmd_debug_get_image_info_runtime(context);
#else
    diag_util_printf("not support\n");
#endif
    return CPARSER_OK;
}
#endif /* CMD_DEBUG_GET_IMAGE_INFO */


/*
 * debug phy <STRING:cmd_name> port ( <PORT_LIST:ports> | none ) { <UINT:param1> } { <UINT:param2> } { <UINT:param3> } { <UINT:param4> } { <UINT:param5> }
 */
cparser_result_t
cparser_cmd_debug_phy_cmd_name_port_ports_none_param1_param2_param3_param4_param5(cparser_context_t *context,
    char **cmd_name_ptr,
    char **ports_ptr,
    uint32_t *param1_ptr,
    uint32_t *param2_ptr,
    uint32_t *param3_ptr,
    uint32_t *param4_ptr,
    uint32_t *param5_ptr)
{
    uint32_t    param1 = 0, param2 = 0, param3 = 0, param4 = 0, param5 = 0;
    diag_portlist_t portlist;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if (param1_ptr != NULL)
    {
        param1 = *param1_ptr;
    }

    if (param2_ptr != NULL)
    {
        param2 = *param2_ptr;
    }

    if (param3_ptr != NULL)
    {
        param3 = *param3_ptr;
    }

    if (param4_ptr != NULL)
    {
        param4 = *param4_ptr;
    }

    if (param5_ptr != NULL)
    {
        param5 = *param5_ptr;
    }

    DIAG_UTIL_ERR_CHK(_dal_phy_debug_cmd(unit, *cmd_name_ptr, &portlist.portmask, param1, param2, param3, param4, param5), ret);
    return CPARSER_OK;
}


/*
 * debug set phy port <UINT:port> serdes-id <UINT:sdsId> rx-cali start
 */
cparser_result_t
cparser_cmd_debug_set_phy_port_port_serdes_id_sdsId_rx_cali_start(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr)
{
    rtk_portmask_t portmask;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    RTK_PORTMASK_RESET(portmask);
    RTK_PORTMASK_PORT_SET(portmask, *port_ptr);


    DIAG_UTIL_ERR_CHK(_dal_phy_debug_cmd(unit, "rx_cali_start", &portmask, *sdsId_ptr, 0, 0, 0, 0), ret);

    return CPARSER_OK;
}

#ifdef CMD_DEBUG_TX_SCAN_TESTPORT_PEERPORT
#define RTK_UTIL_PRINT_AMP_RANGE \
    diag_util_printf("Param: Pre AMP(%d-%d,step=%d) Main AMP(%d-%d,step=%d) Post AMP(%d-%d,step=%d)\n", \
                    preAmp.start, preAmp.end, preAmp.step, \
                    mainAmp.start,mainAmp.end, mainAmp.step,\
                    postAmp.start,postAmp.end, postAmp.step);


#define RTK_DIAG_PRINT_TXSCAN_OPTION \
    if(txScanOption & RT_SDS_TX_SCAN_OPT_MANUAL_LEQ) \
    { \
        diag_util_printf("Option: pair-scan(%s) rx-cali(%s) manual-leq(TRUE,val=%d)\n\n",  \
                         (txScanOption & RT_SDS_TX_SCAN_OPT_PAIR_TST)? "TRUE":"FALSE", \
                         (txScanOption & RT_SDS_TX_SCAN_OPT_RX_CALI)? "TRUE":"FALSE", leqVal); \
    } \
    else \
    { \
        diag_util_printf("Option: pair-scan(%s) rx-cali(%s) manual-leq(FALSE)\n\n", \
                         (txScanOption & RT_SDS_TX_SCAN_OPT_PAIR_TST)? "TRUE":"FALSE", \
                         (txScanOption & RT_SDS_TX_SCAN_OPT_RX_CALI)? "TRUE":"FALSE"); \
    }
#define RTK_DIAG_PRINT_TXSCAN_CHART_HELP \
    diag_util_printf("Chart: PASS(%c), FAIL(%c), Link Down FAIL(%c), Cali FAIL(%c)\n", \
                    RT_SDS_TXSCAN_PASS, RT_SDS_TXSCAN_FAIL, \
                    RT_SDS_TXSCAN_FAIL_LINK, RT_SDS_TXSCAN_FAIL_CALI); \
    diag_util_printf("       Manual LEQ FAIL(%c)\n", RT_SDS_TXSCAN_FAIL_LEQ);

/* Print the result map */
#define RTK_UTIL_PRINT_TXSCAN_MAIN_AMP_CHART(preAmpFmt, colWidth, prnt_data_expr) \
        diag_util_printf("[Main AMP %d, Horizontal axis: Pre AMP, Vertical axis: Post AMP]\n", mainAmpNum); \
        diag_util_printf("  | "); \
        for(preAmpIdx = preAmp.start; preAmpIdx <= preAmp.end; preAmpIdx += preAmp.step) \
        { \
            diag_util_printf(preAmpFmt, preAmpIdx); \
        } \
        diag_util_printf("\n---"); \
        for(preAmpIdx = preAmp.start; preAmpIdx <= preAmp.end; preAmpIdx += preAmp.step) \
        { \
            diag_util_printf(colWidth); \
        } \
        diag_util_printf("\n"); \
        for(postAmpIdx = postAmp.start; postAmpIdx <= postAmp.end; postAmpIdx += postAmp.step)\
        { \
            diag_util_printf("%02d|", postAmpIdx); \
            for(preAmpIdx = preAmp.start; preAmpIdx <= preAmp.end; preAmpIdx += preAmp.step) \
            { \
                prnt_data_expr; \
            }\
            diag_util_printf("\n"); \
        }\
        diag_util_printf("\n");
#endif


#ifdef CMD_DEBUG_TX_SCAN_TESTPORT_PEERPORT
/* Parse AMP range */
int32 _cparser_cmd_debug_tx_scan_amp_range_parsing(
    char *preAmp_ptr, rt_valRangeStep_t *preAmp,
    char *mainAmp_ptr, rt_valRangeStep_t *mainAmp,
    char *postAmp_ptr, rt_valRangeStep_t *postAmp)
{
    int32 ret;
    if( (ret = diag_util_str2rangeStep(preAmp_ptr, preAmp))!= RT_ERR_OK)
    {
        diag_util_printf("preAmp rang format error\n");
        return CPARSER_NOT_OK;
    }
    if( (ret = diag_util_str2rangeStep(mainAmp_ptr, mainAmp))!= RT_ERR_OK)
    {
        diag_util_printf("mainAmp rang format error\n");
        return CPARSER_NOT_OK;
    }
    if( (ret = diag_util_str2rangeStep(postAmp_ptr, postAmp))!= RT_ERR_OK)
    {
        diag_util_printf("postAmp rang format error\n");
        return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_TX_SCAN_TESTPORT_PEERPORT
int32 _cparser_cmd_debug_tx_scan_option_parsing(
    cparser_context_t *context,
    uint32 optionStart,
    uint32 *txScanOption,
    uint32 *leqVal)
{
    uint32 len;
    uint32 optionNum;
    int32 ret;

    *txScanOption = 0;
    *leqVal = 0;
    for (optionNum = optionStart; optionNum < TOKEN_NUM; optionNum++)
    {
        len = osal_strlen(TOKEN_STR(optionNum));
        if (!strncmp("verbose", TOKEN_STR(optionNum), len))
        {
            *txScanOption |= RT_SDS_TX_SCAN_OPT_VERBOSE;
        }
        else if (!strncmp("pair-scan", TOKEN_STR(optionNum), len))
        {
            *txScanOption |= RT_SDS_TX_SCAN_OPT_PAIR_TST;
        }
        else if (!strncmp("rx-cali", TOKEN_STR(optionNum), len))
        {
            *txScanOption |= RT_SDS_TX_SCAN_OPT_RX_CALI;
            if ( optionNum < TOKEN_NUM -1)
            {
                *txScanOption |= RT_SDS_TX_SCAN_OPT_MANUAL_LEQ;
                ret = diag_util_str2ul(leqVal, TOKEN_STR(optionNum+1));
                if(ret != RT_ERR_OK)
                {
                    return CPARSER_NOT_OK;
                }
            }
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_TX_SCAN_TESTPORT_PEERPORT
/*
 * debug tx-scan <UINT:testPort> <UINT:peerPort> pre-amp <STRING:preAmp> main-amp <STRING:mainAmp>
 * post-amp <STRING:postAmp> cpu-tx <UINT:pktn> <UINT:pktl> { verbose } { pair-scan } { rx-cali } { <UINT:leqValue> }
 */
cparser_result_t cparser_cmd_debug_tx_scan_testPort_peerPort_pre_amp_preAmp_main_amp_mainAmp_post_amp_postAmp_cpu_tx_pktn_pktl_verbose_pair_scan_rx_cali_leqValue(cparser_context_t *context,
    uint32_t *testPort_ptr,
    uint32_t *peerPort_ptr,
    char **preAmp_ptr,
    char **mainAmp_ptr,
    char **postAmp_ptr,
    uint32_t *pktn_ptr,
    uint32_t *pktl_ptr,
    uint32_t *leqValue_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit;
    uint32  mainAmpNum, preAmpIdx, postAmpIdx;
    rt_valRangeStep_t mainAmp, preAmp, postAmp;
    uint32 txScanOption=0, leqVal = 0;
    uint32 pktl=64;
    rt_sdsTxScanParam_t param;
    rt_sdsTxScanChart_t txScanResult;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    /* Option parsing */
    if((ret = _cparser_cmd_debug_tx_scan_option_parsing(context, 13, &txScanOption, &leqVal)) != CPARSER_OK)
    {
        return ret;
    }

    pktl = *pktl_ptr;
    /* Packet length check */
    if (pktl < 64 || pktl > JUMBO_FRAME_SIZE_MAX)
    {
        diag_util_printf("Length:%d not supported\n",pktl);
        return CPARSER_NOT_OK;
    }

    if((ret = _cparser_cmd_debug_tx_scan_amp_range_parsing(
        *preAmp_ptr, &preAmp,
        *mainAmp_ptr, &mainAmp,
        *postAmp_ptr, &postAmp)) != CPARSER_OK)
    {
        return ret;
    }
    RTK_UTIL_PRINT_AMP_RANGE;

    diag_util_printf("       pkt num(%d) pkt len(%u)\n", *pktn_ptr, pktl);

    RTK_DIAG_PRINT_TXSCAN_OPTION;
    RTK_DIAG_PRINT_TXSCAN_CHART_HELP;

    osal_memset(&txScanResult, 0, sizeof(txScanResult));

    for(mainAmpNum = mainAmp.start; mainAmpNum <= mainAmp.end; mainAmpNum += mainAmp.step)
    {
        osal_memset(&param, 0, sizeof(param));
        param.mainAmp = mainAmpNum;
        osal_memcpy(&param.preAmpRangeStep, &preAmp, sizeof(rt_valRangeStep_t));
        osal_memcpy(&param.postAmpRangeStep, &postAmp, sizeof(rt_valRangeStep_t));
        param.testPort = *testPort_ptr;
        param.peerPort = *peerPort_ptr;
        param.testSerdes.type = param.peerSerdes.type = SERDES_IN_MAC;

        param.option = txScanOption;
        if(param.option & RT_SDS_TX_SCAN_OPT_MANUAL_LEQ)
        {
            param.leqValue.manual = ENABLED;
            param.leqValue.val = leqVal;
        }

        param.txScanMethod = RT_UTIL_TXSCAN_NIC_TX;
        param.pktCnt = *pktn_ptr;
        param.pktLen = *pktl_ptr;

        /* Do pre/post AMP variation and get a 2-dimensional result map */
        ret = rt_util_serdesTxEyeParam_scan(unit, &param, &txScanResult);
        if( ret != RT_ERR_OK)
        {
            diag_util_printf("SERDES TX Scan program stop\n");
        }
        if(param.option & RT_SDS_TX_SCAN_OPT_PAIR_TST)
        {
            diag_util_printf("serdes ID(%d <=> %d)\n", txScanResult.testSerdes.mac_sdsId,
                                                         txScanResult.peerSerdes.mac_sdsId);
        }
        else
        {
            diag_util_printf("serdes ID(%d => %d)\n", txScanResult.testSerdes.mac_sdsId,
                                                      txScanResult.peerSerdes.mac_sdsId);
        }
        RTK_UTIL_PRINT_TXSCAN_MAIN_AMP_CHART("%02d ", "---", diag_util_printf("  %c", txScanResult.rMap[preAmpIdx][postAmpIdx]));
        if( ret != RT_ERR_OK)
        {
            return CPARSER_OK;
        }
    } // end of mainAmp loop

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_TX_SCAN_TESTPORT_PEERPORT
/*
 * debug tx-scan <UINT:testPort> <UINT:peerPort> pre-amp <STRING:preAmp> main-amp <STRING:mainAmp>
 * post-amp <STRING:postAmp> sds-test ( square8180 | prbs9 | prbs31 ) <UINT:sec> { verbose } { pair-scan } { rx-cali } { <UINT:leqValue> }
 */
cparser_result_t cparser_cmd_debug_tx_scan_testPort_peerPort_pre_amp_preAmp_main_amp_mainAmp_post_amp_postAmp_sds_test_square8180_prbs9_prbs31_sec_verbose_pair_scan_rx_cali_leqValue(cparser_context_t *context,
    uint32_t *testPort_ptr,
    uint32_t *peerPort_ptr,
    char **preAmp_ptr,
    char **mainAmp_ptr,
    char **postAmp_ptr,
    uint32_t *sec_ptr,
    uint32_t *leqValue_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit;
    uint32  mainAmpNum, preAmpIdx, postAmpIdx;
    char *sdsModeStr = NULL;
    rt_valRangeStep_t mainAmp, preAmp, postAmp;
    uint32 txScanOption = 0,leqVal = 0;
    rtk_sds_testMode_t sdsTestMode;
    rt_sdsTxScanParam_t param;
    rt_sdsTxScanChart_t txScanResult;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    /* Serdes test mode selection */
    if('s' == TOKEN_CHAR(11, 0))
    {
        sdsTestMode = RTK_SDS_TESTMODE_SQUARE8;
        sdsModeStr = "square8";
    }
    else if(0 == osal_strcmp(TOKEN_STR(11), "prbs9"))
    {
        sdsTestMode = RTK_SDS_TESTMODE_PRBS9;
        sdsModeStr = "prbs9";
    }
    else if(0 == osal_strcmp(TOKEN_STR(11), "prbs31"))
    {
        sdsTestMode = RTK_SDS_TESTMODE_PRBS31;
        sdsModeStr = "prbs31";
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    /* Option parsing */
    if(_cparser_cmd_debug_tx_scan_option_parsing(context, 13, &txScanOption, &leqVal) != CPARSER_OK)
    {
        return CPARSER_NOT_OK;
    }

    if((ret = _cparser_cmd_debug_tx_scan_amp_range_parsing(
            *preAmp_ptr, &preAmp,
            *mainAmp_ptr, &mainAmp,
            *postAmp_ptr, &postAmp)) != CPARSER_OK)
    {
        return ret;
    }
    RTK_UTIL_PRINT_AMP_RANGE;

    diag_util_printf("       serdes test mode(%s) test time(%d second)\n",
                     sdsModeStr, *sec_ptr);

    RTK_DIAG_PRINT_TXSCAN_OPTION;
    RTK_DIAG_PRINT_TXSCAN_CHART_HELP;

    osal_memset(&txScanResult, 0, sizeof(txScanResult));

    for(mainAmpNum = mainAmp.start; mainAmpNum <= mainAmp.end; mainAmpNum += mainAmp.step)
    {
        osal_memset(&param, 0, sizeof(param));
        param.mainAmp = mainAmpNum;
        osal_memcpy(&param.preAmpRangeStep, &preAmp, sizeof(rt_valRangeStep_t));
        osal_memcpy(&param.postAmpRangeStep, &postAmp, sizeof(rt_valRangeStep_t));
        param.testPort = *testPort_ptr;
        param.peerPort = *peerPort_ptr;
        param.testSerdes.type = param.peerSerdes.type = SERDES_IN_MAC;
        param.option = txScanOption;
        if(param.option & RT_SDS_TX_SCAN_OPT_MANUAL_LEQ)
        {
            param.leqValue.manual = ENABLED;
            param.leqValue.val = leqVal;
        }

        param.txScanMethod = RT_UTIL_TXSCAN_SERDES_TEST;
        param.sdsTestMode = sdsTestMode;
        param.sdsTestTime = *sec_ptr;

        /* Do pre/post AMP variation and get a 2-dimensional result map */
        ret = rt_util_serdesTxEyeParam_scan(unit, &param, &txScanResult);
        if( ret != RT_ERR_OK)
        {
            diag_util_printf("SERDES TX Scan program stop\n");
        }
        if(param.option & RT_SDS_TX_SCAN_OPT_PAIR_TST)
        {
            diag_util_printf("serdes ID(%d <=> %d)\n", txScanResult.testSerdes.mac_sdsId,
                                                         txScanResult.peerSerdes.mac_sdsId);
        }
        else
        {
            diag_util_printf("serdes ID(%d => %d)\n", txScanResult.testSerdes.mac_sdsId,
                                                      txScanResult.peerSerdes.mac_sdsId);
        }
        RTK_UTIL_PRINT_TXSCAN_MAIN_AMP_CHART("%02d ", "---", diag_util_printf("  %c", txScanResult.rMap[preAmpIdx][postAmpIdx]));
        if( ret != RT_ERR_OK)
        {
            return CPARSER_OK;
        }
    } // end of mainAmp loop

    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_TX_SCAN_TESTPORT_PEERPORT
/*
 * debug tx-scan <UINT:testPort> <UINT:peerPort> pre-amp <STRING:preAmp> main-amp <STRING:mainAmp>
 * post-amp <STRING:postAmp> eye-monitor <UINT:frameNum> { verbose } { pair-scan } { rx-cali } { <UINT:leqValue> }
 */
cparser_result_t cparser_cmd_debug_tx_scan_testPort_peerPort_pre_amp_preAmp_main_amp_mainAmp_post_amp_postAmp_eye_monitor_frameNum_verbose_pair_scan_rx_cali_leqValue(cparser_context_t *context,
    uint32_t *testPort_ptr,
    uint32_t *peerPort_ptr,
    char **preAmp_ptr,
    char **mainAmp_ptr,
    char **postAmp_ptr,
    uint32_t *frameNum_ptr,
    uint32_t *leqValue_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit;
    uint32  mainAmpNum, preAmpIdx, postAmpIdx;
    rt_valRangeStep_t mainAmp, preAmp, postAmp;
    uint32 txScanOption = 0, leqVal=0;
    int32 pairRunCnt=0;
    rt_sdsTxScanParam_t param;
    rt_sdsTxScanChart_t txScanResult;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    /* Option parsing */
    if(_cparser_cmd_debug_tx_scan_option_parsing(context, 12, &txScanOption, &leqVal) != CPARSER_OK)
    {
        return CPARSER_NOT_OK;
    }

    if((ret = _cparser_cmd_debug_tx_scan_amp_range_parsing(
        *preAmp_ptr, &preAmp,
        *mainAmp_ptr, &mainAmp,
        *postAmp_ptr, &postAmp)) != CPARSER_OK)
    {
        return ret;
    }
    RTK_UTIL_PRINT_AMP_RANGE;
    diag_util_printf("       eye info frame number(%d)\n", *frameNum_ptr);
    RTK_DIAG_PRINT_TXSCAN_OPTION;
    diag_util_printf("Chart: (Height/Width/LEQ)\n");

    osal_memset(&txScanResult, 0, sizeof(txScanResult));

    for(mainAmpNum = mainAmp.start; mainAmpNum <= mainAmp.end; mainAmpNum += mainAmp.step)
    {
        osal_memset(&param, 0, sizeof(param));
        param.mainAmp = mainAmpNum;
        osal_memcpy(&param.preAmpRangeStep, &preAmp, sizeof(rt_valRangeStep_t));
        osal_memcpy(&param.postAmpRangeStep, &postAmp, sizeof(rt_valRangeStep_t));
        param.testPort = *testPort_ptr;
        param.peerPort = *peerPort_ptr;
        param.testSerdes.type = param.peerSerdes.type = SERDES_IN_MAC;
        param.option = txScanOption;
        if(param.option & RT_SDS_TX_SCAN_OPT_MANUAL_LEQ)
        {
            param.leqValue.manual = ENABLED;
            param.leqValue.val = leqVal;
        }

        param.txScanMethod = RT_UTIL_TXSCAN_EYE_MON;
        param.frameNum= *frameNum_ptr;

        if(param.option & RT_SDS_TX_SCAN_OPT_PAIR_TST)
        {
            pairRunCnt = 2;
        }
        else
        {
            pairRunCnt = 1;
        }

        do{
            /* Do pre/post AMP variation and get a 2-dimensional result map */
            ret = rt_util_serdesTxEyeParam_scan(unit, &param, &txScanResult);
            if( ret != RT_ERR_OK)
            {
                diag_util_printf("SERDES TX Scan program stop\n");
            }

            diag_util_printf("serdes ID(%d)\n", txScanResult.peerSerdes.mac_sdsId);

            RTK_UTIL_PRINT_TXSCAN_MAIN_AMP_CHART("%9d ", "----------",
                diag_util_printf("%3d/%2d/%2d  ", txScanResult.eyeHeight[preAmpIdx][postAmpIdx],
                                                  txScanResult.eyeWidth[preAmpIdx][postAmpIdx],
                                                  txScanResult.rMap[preAmpIdx][postAmpIdx]);
              );
            pairRunCnt--;

            if(param.option & RT_SDS_TX_SCAN_OPT_PAIR_TST)
            {   //swap tx/rx
                param.testPort = *peerPort_ptr;
                param.peerPort = *testPort_ptr;
                param.testSerdes.type = SERDES_IN_MAC;
                param.peerSerdes.type = SERDES_IN_MAC;
            }

        }while(pairRunCnt > 0);
        if( ret != RT_ERR_OK)
        {
            return CPARSER_OK;
        }
    } // end of mainAmp loop

    return CPARSER_OK;
}
#endif


#ifdef CMD_DEBUG_LINKMON_POLLING_STOP
/*
 * debug linkmon polling stop
 */
cparser_result_t
cparser_cmd_debug_linkmon_polling_stop(cparser_context_t *context)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(dal_linkMonPolling_stop(unit, TRUE), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_LINKMON_POLLING_RESUME
/*
 * debug linkmon polling resume
 */
cparser_result_t
cparser_cmd_debug_linkmon_polling_resume(cparser_context_t *context)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(dal_linkMonPolling_stop(unit, FALSE), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_PHY_DUMP_DSP
/*
 * debug dump phy port ( <PORT_LIST:ports> | all ) dsp cnt <UINT:count> mask <UINT64:bitmask>
 */
cparser_result_t
cparser_cmd_debug_dump_phy_port_ports_all_dsp_cnt_count_mask_bitmask(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *count_ptr,
    uint64_t *bitmask_ptr)
{
    uint32               unit = 0;
    //rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_debug_t      dbg;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

#if 1 //use port mask
    dbg.phyDbgType = RTK_PHY_DEBUG_DSP_MULTIPORT;
    osal_memcpy(&dbg.parameter.dsp_multiport.portmask, &portlist.portmask, sizeof(rtk_portmask_t));
    dbg.parameter.dsp_multiport.cnt = *count_ptr;
    dbg.parameter.dsp_multiport.itemBitmap = *bitmask_ptr;

    DIAG_UTIL_ERR_CHK(rtk_phy_debug_get(unit, &dbg), ret);
#else
    dbg.phyDbgType = RTK_PHY_DEBUG_DSP;
    dbg.parameter.dsp.cnt = *count_ptr;
    dbg.parameter.dsp.itemBitmap = *bitmask_ptr;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        dbg.port = port;
        DIAG_UTIL_ERR_CHK(rtk_phy_debug_get(unit, &dbg), ret);
    }
#endif
    return CPARSER_OK;
}
#endif


#ifdef CMD_DEBUG_PHY_DUMP_COUPLING
/*
 * debug dump phy port ( <PORT_LIST:ports> | all ) coupling { <UINT:channel> }
 */
cparser_result_t
cparser_cmd_debug_dump_phy_port_ports_all_coupling_channel(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *channel_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_debug_t      dbg;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);


    dbg.phyDbgType = RTK_PHY_DEBUG_COUPLING;
    if (channel_ptr != NULL)
    {
        dbg.parameter.coupling.channel_bitmap = (1UL << *channel_ptr);

    }
    else
    {
        dbg.parameter.coupling.channel_bitmap = 0;
    }



    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        dbg.port = port;
        DIAG_UTIL_ERR_CHK(rtk_phy_debug_get(unit, &dbg), ret);
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_DEBUG_BATCH
/*
 * debug batch port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_debug_batch_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit=0;
    int32           ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_ERR_CHK(rtk_phy_debug_batch_port_set(unit, &portlist.portmask), ret);
    return CPARSER_OK;
}

/*
 * debug batch <STRING:cmd> { <UINT:param1> { <UINT:param2> { <UINT:param3> { <UINT:param4> { <UINT:param5> } } } } }
 */
cparser_result_t
_diag_debug_batch_cmd(cparser_context_t *context,
    char **cmd_ptr,
    uint32_t *param1_ptr,
    uint32_t *param2_ptr,
    uint32_t *param3_ptr,
    uint32_t *param4_ptr,
    uint32_t *param5_ptr)
{
    rtk_phy_batch_para_t batch_op;
    uint32               unit=0;
    uint32               cnt = 0;
    int32                ret;
    char                 *str;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&batch_op, 0x0, sizeof(rtk_phy_batch_para_t));
    str = *cmd_ptr;

    if (!strncmp("help", str, 4))
    {
        diag_util_printf("\nQuick start steps:\n");
        diag_util_printf("-------------------------------------------------------------------\n");
        diag_util_printf("1. Initial the batch profile by command: debug batch init \n");
        diag_util_printf("2. Write debug batch profile by batch profile commands \n");
        diag_util_printf("3. Set target port(s) by command: debug batch port <portlist>\n");
        diag_util_printf("4. Run the batch profile by command: debug batch run\n");

        diag_util_printf("\nDebug batch command:\n");
        diag_util_printf("-------------------------------------------------------------------\n");
        diag_util_printf("debug batch port <portlist> \n");
        diag_util_printf("        set target ports to run batch\n");
        diag_util_printf("debug batch init \n");
        diag_util_printf("        init/reset batch profile\n");
        diag_util_printf("debug batch init <num> \n");
        diag_util_printf("        init/reset batch profile to specific entry size\n");
        diag_util_printf("debug batch dump \n");
        diag_util_printf("        dump batch profile\n");
        diag_util_printf("debug batch run \n");
        diag_util_printf("debug batch run <display>\n");
        diag_util_printf("        run batch profile, <display>=1 to show debug info\n");

        diag_util_printf("\nDebug batch profile command:\n");
        diag_util_printf("-------------------------------------------------------------------\n");
        diag_util_printf("debug batch r <mmd> <reg>\n");
        diag_util_printf("debug batch r <mmd> <reg> <msb> <lsb>\n");
        diag_util_printf("        read value from Clause 45 register\n");
        diag_util_printf("debug batch w <mmd> <reg> <val>\n");
        diag_util_printf("debug batch w <mmd> <reg> <msb> <lsb> <val>\n");
        diag_util_printf("        write value to Clause 45 register\n");
        diag_util_printf("debug batch c <mmd> <reg> <val>\n");
        diag_util_printf("debug batch c <mmd> <reg> <msb> <lsb> <val>\n");
        diag_util_printf("        read value from Clause 45 register and compare to expected value\n");
        diag_util_printf("debug batch weq <mmd> <reg> <val>\n");
        diag_util_printf("debug batch weq <mmd> <reg> <msb> <lsb> <val>\n");
        diag_util_printf("        wait Clause 45 register value is equal to <val>\n");
        diag_util_printf("debug batch wneq <mmd> <reg> <val>\n");
        diag_util_printf("debug batch wneq <mmd> <reg> <msb> <lsb> <val>\n");
        diag_util_printf("        wait Clause 45 register value is not equal to <val>\n");

        diag_util_printf("debug batch 22r <page> <reg>\n");
        diag_util_printf("debug batch 22r <page> <reg> <msb> <lsb>\n");
        diag_util_printf("        read value from Clause 22 register\n");
        diag_util_printf("debug batch 22w <page> <reg> <val>\n");
        diag_util_printf("debug batch 22w <page> <reg> <msb> <lsb> <val>\n");
        diag_util_printf("        write value to Clause 22 register\n");
        diag_util_printf("debug batch 22c <page> <reg> <val>\n");
        diag_util_printf("debug batch 22c <page> <reg> <msb> <lsb> <val>\n");
        diag_util_printf("        read value from Clause 22 register and compare to expected value\n");

        diag_util_printf("debug batch loopstart <num>\n");
        diag_util_printf("        start entry for loop, loop <num> times\n");
        diag_util_printf("debug batch loopend\n");
        diag_util_printf("        end entry for loop\n");
        return CPARSER_OK;
    }
    else if (!strncmp("init", str, 4))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_INIT;
    }
    else if (!strncmp("dump", str, 4))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_DUMP;
    }
    else if (!strncmp("run", str, 3))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_RUN;
    }
    else if (!strncmp("loopstart", str, 9))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_LOOPSTART;
    }
    else if (!strncmp("loopend", str, 7))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_LOOPEND;
    }
    else if (!strncmp("22r", str, 3))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_R_C22;
    }
    else if (!strncmp("22w", str, 3))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_W_C22;
    }
    else if (!strncmp("22c", str, 3))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_C_C22;
    }
    else if (!strncmp("weq", str, 3))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_WEQ_C45;
    }
    else if (!strncmp("wneq", str, 4))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_WNEQ_C45;
    }
    else if (!strncmp("r", str, 1))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_R_C45;
    }
    else if (!strncmp("w", str, 1))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_W_C45;
    }
    else if (!strncmp("c", str, 1))
    {
        batch_op.batch_op = RTK_PHY_BATCH_OP_C_C45;
    }
    else
    {
        diag_util_printf("Error: unknown op %s\n", str);
        return CPARSER_ERR_INVALID_PARAMS;
    }

    if (param1_ptr != NULL)
    {
        batch_op.para1 = *param1_ptr;
        cnt++;
    }

    if (param2_ptr != NULL)
    {
        batch_op.para2 = *param2_ptr;
        cnt++;
    }

    if (param3_ptr != NULL)
    {
        batch_op.para3 = *param3_ptr;
        cnt++;
    }

    if (param4_ptr != NULL)
    {
        batch_op.para4 = *param4_ptr;
        cnt++;
    }

    if (param5_ptr != NULL)
    {
        batch_op.para5 = *param5_ptr;
        cnt++;
    }

    batch_op.para_num = cnt;
    DIAG_UTIL_ERR_CHK(rtk_phy_debug_batch_op_set(unit, &batch_op), ret);
    return CPARSER_OK;
}

cparser_result_t
cparser_cmd_debug_batch_cmd_param1_param2_param3_param4_param5(cparser_context_t *context,
    char **cmd_ptr,
    uint32_t *param1_ptr,
    uint32_t *param2_ptr,
    uint32_t *param3_ptr,
    uint32_t *param4_ptr,
    uint32_t *param5_ptr)
{
    return _diag_debug_batch_cmd(context, cmd_ptr, param1_ptr, param2_ptr, param3_ptr, param4_ptr, param5_ptr);
}

cparser_result_t
cparser_cmd_debug_batch_cmd_param1_param2_param3_param4(cparser_context_t *context,
    char **cmd_ptr,
    uint32_t *param1_ptr,
    uint32_t *param2_ptr,
    uint32_t *param3_ptr,
    uint32_t *param4_ptr)
{
    return _diag_debug_batch_cmd(context, cmd_ptr, param1_ptr, param2_ptr, param3_ptr, param4_ptr, NULL);
}

cparser_result_t
cparser_cmd_debug_batch_cmd_param1_param2_param3(cparser_context_t *context,
    char **cmd_ptr,
    uint32_t *param1_ptr,
    uint32_t *param2_ptr,
    uint32_t *param3_ptr)
{
    return _diag_debug_batch_cmd(context, cmd_ptr, param1_ptr, param2_ptr, param3_ptr, NULL, NULL);
}

cparser_result_t
cparser_cmd_debug_batch_cmd_param1_param2(cparser_context_t *context,
    char **cmd_ptr,
    uint32_t *param1_ptr,
    uint32_t *param2_ptr)
{
    return _diag_debug_batch_cmd(context, cmd_ptr, param1_ptr, param2_ptr, NULL, NULL, NULL);
}

cparser_result_t
cparser_cmd_debug_batch_cmd_param1(cparser_context_t *context,
    char **cmd_ptr,
    uint32_t *param1_ptr)
{
    return _diag_debug_batch_cmd(context, cmd_ptr, param1_ptr, NULL, NULL, NULL, NULL);
}

cparser_result_t
cparser_cmd_debug_batch_cmd(cparser_context_t *context,
    char **cmd_ptr)
{
    return _diag_debug_batch_cmd(context, cmd_ptr, NULL, NULL, NULL, NULL, NULL);
}
#endif

