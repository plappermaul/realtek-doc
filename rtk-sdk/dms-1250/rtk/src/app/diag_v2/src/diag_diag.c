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
 * Purpose : Definition those Diagnostic command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Locol/Remote Loopback
 *           2) RTCT
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/util/rt_util_serdes.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <rtk/diag.h>
#include <rtk/port.h>
#include <rtk/switch.h>
#include <osal/memory.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF)
  #include <hal/mac/rtl8295.h>
  #include <hal/phy/phy_rtl8295.h>
  #include <hal/phy/phy_rtl8295_patch.h>
#endif
#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_diag.h>
  #include <rtrpc/rtrpc_port.h>
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_debug.h>
  #include <rtrpc/rtrpc_util.h>
#endif


#define PORT_NUM_IN_8218B   8

#ifdef CMD_DIAG_GET_CABLE_DOCTOR_PORT_PORTS_ALL
static void _cparser_cmd_diag_get_cable_doctor_port_ports_all_channelStatus_display(char *channName, rtk_rtctChannelStatus_t *channelStatus);

/*
 * diag get cable-doctor port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_diag_get_cable_doctor_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_rtctResult_t    rtctResult;
    diag_portlist_t     portlist;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&rtctResult, 0, sizeof(rtk_rtctResult_t));
        if ((ret = rtk_diag_portRtctResult_get(unit, port, &rtctResult)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        if ((rtctResult.linkType == PORT_SPEED_1000M) || (rtctResult.linkType == PORT_SPEED_2_5G))
        {
            diag_util_mprintf("Port %2d (type GE):\n", port);
            diag_util_printf("  channel A: \n");
            diag_util_printf("    Status : ");
            if (rtctResult.un.ge_result.channelAShort == 1)
                diag_util_printf("[Short]");
            if (rtctResult.un.ge_result.channelAShort == 2)
                diag_util_printf("[Interpair Short]");
            if (rtctResult.un.ge_result.channelAOpen)
                diag_util_printf("[Open]");
            if (rtctResult.un.ge_result.channelAMismatch)
                diag_util_printf("[Mismatch]");
            if (rtctResult.un.ge_result.channelALinedriver)
                diag_util_printf("[Linedriver]");
            if (rtctResult.un.ge_result.channelAHiImpedance)
                diag_util_printf("[HiImpedance]");
            if (rtctResult.un.ge_result.channelACross)
                diag_util_printf("[Cross]");
            if (rtctResult.un.ge_result.channelAPartialCross)
                diag_util_printf("[PartialCross]");
            if (rtctResult.un.ge_result.channelAPairBusy)
                diag_util_printf("[PairBusy]");
            if (!(rtctResult.un.ge_result.channelAShort | rtctResult.un.ge_result.channelAOpen |
                rtctResult.un.ge_result.channelAMismatch | rtctResult.un.ge_result.channelALinedriver |
                rtctResult.un.ge_result.channelAHiImpedance | rtctResult.un.ge_result.channelACross |
                rtctResult.un.ge_result.channelAPartialCross | rtctResult.un.ge_result.channelAPairBusy))
                diag_util_printf("[Normal]");
            diag_util_printf("\n");

            diag_util_printf("    Cable Length : %d.%02d (m)\n", rtctResult.un.ge_result.channelALen/100, rtctResult.un.ge_result.channelALen%100);

            diag_util_printf("  channel B: \n");
            diag_util_printf("    Status : ");
            if (rtctResult.un.ge_result.channelBShort == 1)
                diag_util_printf("[Short]");
            if (rtctResult.un.ge_result.channelBShort == 2)
                diag_util_printf("[Interpair Short]");
            if (rtctResult.un.ge_result.channelBOpen)
                diag_util_printf("[Open]");
            if (rtctResult.un.ge_result.channelBMismatch)
                diag_util_printf("[Mismatch]");
            if (rtctResult.un.ge_result.channelBLinedriver)
                diag_util_printf("[Linedriver]");
            if (rtctResult.un.ge_result.channelBHiImpedance)
                diag_util_printf("[HiImpedance]");
            if (rtctResult.un.ge_result.channelBCross)
                diag_util_printf("[Cross]");
            if (rtctResult.un.ge_result.channelBPartialCross)
                diag_util_printf("[PartialCross]");
            if (rtctResult.un.ge_result.channelBPairBusy)
                diag_util_printf("[PairBusy]");
            if (!(rtctResult.un.ge_result.channelBShort | rtctResult.un.ge_result.channelBOpen |
                rtctResult.un.ge_result.channelBMismatch | rtctResult.un.ge_result.channelBLinedriver |
                rtctResult.un.ge_result.channelBHiImpedance | rtctResult.un.ge_result.channelBCross |
                rtctResult.un.ge_result.channelBPartialCross | rtctResult.un.ge_result.channelBPairBusy))
                diag_util_printf("[Normal]");
            diag_util_printf("\n");

            diag_util_printf("    Cable Length : %d.%02d (m)\n", rtctResult.un.ge_result.channelBLen/100, rtctResult.un.ge_result.channelBLen%100);

            diag_util_printf("  channel C: \n");
            diag_util_printf("    Status : ");
            if (rtctResult.un.ge_result.channelCShort == 1)
                diag_util_printf("[Short]");
            if (rtctResult.un.ge_result.channelCShort == 2)
                diag_util_printf("[Interpair Short]");
            if (rtctResult.un.ge_result.channelCOpen)
                diag_util_printf("[Open]");
            if (rtctResult.un.ge_result.channelCMismatch)
                diag_util_printf("[Mismatch]");
            if (rtctResult.un.ge_result.channelCLinedriver)
                diag_util_printf("[Linedriver]");
            if (rtctResult.un.ge_result.channelCHiImpedance)
                diag_util_printf("[HiImpedance]");
            if (rtctResult.un.ge_result.channelCCross)
                diag_util_printf("[Cross]");
            if (rtctResult.un.ge_result.channelCPartialCross)
                diag_util_printf("[PartialCross]");
            if (rtctResult.un.ge_result.channelCPairBusy)
                diag_util_printf("[PairBusy]");
            if (!(rtctResult.un.ge_result.channelCShort | rtctResult.un.ge_result.channelCOpen |
                rtctResult.un.ge_result.channelCMismatch | rtctResult.un.ge_result.channelCLinedriver |
                rtctResult.un.ge_result.channelCHiImpedance | rtctResult.un.ge_result.channelCCross |
                rtctResult.un.ge_result.channelCPartialCross | rtctResult.un.ge_result.channelCPairBusy))
                diag_util_printf("[Normal]");
            diag_util_printf("\n");

            diag_util_printf("    Cable Length : %d.%02d (m)\n", rtctResult.un.ge_result.channelCLen/100, rtctResult.un.ge_result.channelCLen%100);

            diag_util_printf("  channel D: \n");
            diag_util_printf("    Status : ");
            if (rtctResult.un.ge_result.channelDShort == 1)
                diag_util_printf("[Short]");
            if (rtctResult.un.ge_result.channelDShort == 2)
                diag_util_printf("[Interpair Short]");
            if (rtctResult.un.ge_result.channelDOpen)
                diag_util_printf("[Open]");
            if (rtctResult.un.ge_result.channelDMismatch)
                diag_util_printf("[Mismatch]");
            if (rtctResult.un.ge_result.channelDLinedriver)
                diag_util_printf("[Linedriver]");
            if (rtctResult.un.ge_result.channelDHiImpedance)
                diag_util_printf("[HiImpedance]");
            if (rtctResult.un.ge_result.channelDCross)
                diag_util_printf("[Cross]");
            if (rtctResult.un.ge_result.channelDPartialCross)
                diag_util_printf("[PartialCross]");
            if (rtctResult.un.ge_result.channelDPairBusy)
                diag_util_printf("[PairBusy]");
            if (!(rtctResult.un.ge_result.channelDShort | rtctResult.un.ge_result.channelDOpen |
                rtctResult.un.ge_result.channelDMismatch | rtctResult.un.ge_result.channelDLinedriver |
                rtctResult.un.ge_result.channelDHiImpedance | rtctResult.un.ge_result.channelDCross |
                rtctResult.un.ge_result.channelDPartialCross | rtctResult.un.ge_result.channelDPairBusy))
                diag_util_printf("[Normal]");
            diag_util_printf("\n");

            diag_util_printf("    Cable Length : %d.%02d (m)\n", rtctResult.un.ge_result.channelDLen/100, rtctResult.un.ge_result.channelDLen%100);

        }
        else if (rtctResult.linkType == PORT_SPEED_100M)
        {
            diag_util_mprintf("Port %2d (type FE):\n", port);

            diag_util_printf("  Rx channel : \n");
            diag_util_printf("    Status : ");
            if (rtctResult.un.fe_result.isRxShort)
                diag_util_printf("[Short]");
            if (rtctResult.un.fe_result.isRxOpen)
                diag_util_printf("[Open]");
            if (rtctResult.un.fe_result.isRxMismatch)
                diag_util_printf("[Mismatch]");
            if (rtctResult.un.fe_result.isRxLinedriver)
                diag_util_printf("[Linedriver]");
            if (!(rtctResult.un.fe_result.isRxShort | rtctResult.un.fe_result.isRxOpen |
                rtctResult.un.fe_result.isRxMismatch | rtctResult.un.fe_result.isRxLinedriver))
                diag_util_printf("[Normal]");
            diag_util_printf("\n");

            diag_util_printf("    Cable Length : %d.%02d (m)\n", rtctResult.un.fe_result.rxLen/100, rtctResult.un.fe_result.rxLen%100);

            diag_util_printf("  Tx channel : \n");
            diag_util_printf("    Status : ");
            if (rtctResult.un.fe_result.isTxShort)
                diag_util_printf("[Short]");
            if (rtctResult.un.fe_result.isTxOpen)
                diag_util_printf("[Open]");
            if (rtctResult.un.fe_result.isTxMismatch)
                diag_util_printf("[Mismatch]");
            if (rtctResult.un.fe_result.isTxLinedriver)
                diag_util_printf("[Linedriver]");
            if (!(rtctResult.un.fe_result.isTxShort | rtctResult.un.fe_result.isTxOpen |
                rtctResult.un.fe_result.isTxMismatch | rtctResult.un.fe_result.isTxLinedriver))
                diag_util_printf("[Normal]");
            diag_util_printf("\n");

            diag_util_printf("    Cable Length : %d.%02d (m)\n", rtctResult.un.fe_result.txLen/100, rtctResult.un.fe_result.txLen%100);
        }
        else
        {
            diag_util_mprintf("Port %2d (type XGE):\n", port);
            _cparser_cmd_diag_get_cable_doctor_port_ports_all_channelStatus_display("A", &rtctResult.un.channels_result.a);
            _cparser_cmd_diag_get_cable_doctor_port_ports_all_channelStatus_display("B", &rtctResult.un.channels_result.b);
            _cparser_cmd_diag_get_cable_doctor_port_ports_all_channelStatus_display("C", &rtctResult.un.channels_result.c);
            _cparser_cmd_diag_get_cable_doctor_port_ports_all_channelStatus_display("D", &rtctResult.un.channels_result.d);
        }
   }

    return CPARSER_OK;
}

/*
 * Display RTCT channel status
 */
static void
_cparser_cmd_diag_get_cable_doctor_port_ports_all_channelStatus_display(char *channName, rtk_rtctChannelStatus_t *channelStatus)
{
    uint32  is_normal = TRUE;

    diag_util_printf("  channel %s: \n", channName);
    diag_util_printf("    Status : ");
    if (channelStatus->channelShort)
    {
        diag_util_printf("[Short]");
        is_normal = FALSE;
    }
    if (channelStatus->channelOpen)
    {
        diag_util_printf("[Open]");
        is_normal = FALSE;
    }
    if (channelStatus->channelLowMismatch)
    {
        diag_util_printf("[Low-Mismatch]");
        is_normal = FALSE;
    }
    if (channelStatus->channelHighMismatch)
    {
        diag_util_printf("[High-Mismatch]");
        is_normal = FALSE;
    }
    if (channelStatus->channelCrossoverA)
    {
        diag_util_printf("[Crossover-A]");
        is_normal = FALSE;
    }
    if (channelStatus->channelCrossoverB)
    {
        diag_util_printf("[Crossover-B]");
        is_normal = FALSE;
    }
    if (channelStatus->channelCrossoverC)
    {
        diag_util_printf("[Crossover-C]");
        is_normal = FALSE;
    }
    if (channelStatus->channelCrossoverD)
    {
        diag_util_printf("[Crossover-D]");
        is_normal = FALSE;
    }

    if (is_normal == TRUE)
        diag_util_printf("[Normal]");
    diag_util_printf("\n");

    diag_util_printf("    Cable Length : %d.%02d (m)\n", channelStatus->channelLen/100, channelStatus->channelLen%100);
}
#endif

#ifdef CMD_DIAG_SET_CABLE_DOCTOR_PORT_PORTS_ALL_START
/*
 * diag set cable-doctor port ( <PORT_LIST:ports> | all ) start
 */
cparser_result_t cparser_cmd_diag_set_cable_doctor_port_ports_all_start(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    diag_portlist_t               portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_ERR_CHK(rtk_diag_rtctEnable_set(unit, &portlist.portmask), ret);

    return CPARSER_OK;
}
#endif

/*
 * diag dump table <UINT:index>
 */
#ifdef CMD_DIAG_DUMP_TABLE_INDEX
cparser_result_t cparser_cmd_diag_dump_table_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    int32 return_value;
    uint32 unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (('t' == TOKEN_CHAR(2,0))&&(4 == TOKEN_NUM))
    {
        DIAG_UTIL_ERR_CHK(rtk_diag_table_whole_read(unit, *index_ptr), return_value);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_DIAG_DUMP_TABLE_INDEX_NAME
/*
 * diag dump table name ( <UINT:index> | all )
 */
cparser_result_t cparser_cmd_diag_dump_table_name_index_all(cparser_context_t *context,
    uint32_t *index_ptr)
{
    int32 return_value;
    uint32 unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (('t' == TOKEN_CHAR(2,0))&&(5 == TOKEN_NUM))
    {
        if('a' == TOKEN_CHAR(4,0))
            DIAG_UTIL_ERR_CHK(rtk_diag_table_index_name(unit, 0xff), return_value);
        else
            DIAG_UTIL_ERR_CHK(rtk_diag_table_index_name(unit, *index_ptr), return_value);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_DIAG_DUMP_TABLE_INDEX_START_INDEX_END_INDEX_DETAIL

/*
 * diag dump table <UINT:index> <UINT:start_index> <UINT:end_index> { detail }
 */
cparser_result_t cparser_cmd_diag_dump_table_index_start_index_end_index_detail(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *start_index_ptr,
    uint32_t *end_index_ptr)
{
    uint32      is_detail = FALSE;
    uint32      unit, i;
    int32       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (TOKEN_NUM > 6)
    {
        for (i = 6; i < TOKEN_NUM; i++)
        {
            if (strncmp(TOKEN_STR(i), "d", 1) == 0) /* detail */
            {
                is_detail = TRUE;
            }
        }/* end for */
    }

    DIAG_UTIL_ERR_CHK(rtk_diag_tableEntry_read(unit, *index_ptr, *start_index_ptr, *end_index_ptr, is_detail), ret);

    return CPARSER_OK;
}
#endif

#define BITS2WORD(b) ((b+31)/32)
#define DIAG_PRINT_TBL_VAL(i, tbl_words, value)  \
    diag_util_printf("0x");              \
    for (i= 0; i < tbl_words; i++)        \
    {                                     \
        if(i > 0)                        \
        {                                 \
            diag_util_printf("-");        \
        }                                 \
        diag_util_printf("%08x", value[i]);  \
    }

#define DIAG_PRINT_TBL_FIELD_NAME(info)  \
    diag_util_printf("%s", info.name);

#define DIAG_PRINT_TBL_FIELD_VAL(i, bits, pValue)  \
    if (bits == 1)                             \
    {                                          \
        diag_util_printf("%d", *pValue);         \
    }                                          \
    else                                       \
    {                                          \
        diag_util_mprintf("0x");               \
        for (i = BITS2WORD(bits)-1;i >= 0; i--)  \
        {                                      \
            if(i != BITS2WORD(bits)-1)                          \
            {                                  \
                diag_util_mprintf("-");        \
            }                                  \
            diag_util_mprintf("%x", pValue[i]); \
        }                                      \
    }

#ifdef CMD_DIAG_DUMP_TABLE_NAME_START_INDEX_END_INDEX_RAW
/*
 * diag dump table <STRING:name> <UINT:start_index> <UINT:end_index> { raw }
 */
cparser_result_t cparser_cmd_diag_dump_table_name_start_index_end_index_raw(cparser_context_t *context,
    char **name_ptr,
    uint32_t *start_index_ptr,
    uint32_t *end_index_ptr)
{

#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    uint32 unit, print_fld = TRUE;
    int32 ret,i;
    uint32 field_idx, entry_idx, eValue[RTK_DIAG_TBL_DATAREG_NUM_MAX];
    uint32 fValue[RTK_DIAG_TBL_FIELD_WORDS_MAX];
    uint32 entry_start, entry_end, datareg_idx ;
    char *tbl_name = *name_ptr;
    rtk_diag_tblInfo_t tbl_info;
    rtk_diag_tblFieldInfo_t *pField_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (TOKEN_NUM > 6)
    {
        for (i = 6; i < TOKEN_NUM; i++)
        {
            if (strncmp(TOKEN_STR(i), "r", 1) == 0) /* raw */
            {
                print_fld = FALSE;
            }
        }/* end for */
    }

    /* Name to upper case */
    diag_util_str2upper(*name_ptr);

    /* Get table info - index, name etc... */
    ret = rtk_diag_tableInfoByStr_get(unit, tbl_name, &tbl_info);
    if(ret != RT_ERR_OK)
    {
        diag_util_mprintf("Table not found\n");
        return CPARSER_NOT_OK;
    }

    // Malloc buf for field info storage
    pField_info = osal_alloc(tbl_info.field_num * sizeof(rtk_diag_tblFieldInfo_t));
    if(NULL == pField_info)
    {
        return RT_ERR_MEM_ALLOC;
    }

    /* Get table Field info - name, len, lsp etc... */
    for (field_idx = 0; field_idx < tbl_info.field_num; field_idx++)
    {
        ret = rtk_diag_tableFieldInfo_get(unit, tbl_info.index, field_idx, &pField_info[field_idx]);
        if(ret != RT_ERR_OK)
        {
            goto exit;
        }
    }

    entry_start = DIAG_MIN(*start_index_ptr, tbl_info.size -1);
    entry_end = DIAG_MIN(*end_index_ptr, tbl_info.size -1);

    // Print TBL info
    diag_util_mprintf("\n");
    diag_util_mprintf("Table Type(Phyical ID)  = %u", tbl_info.type);
    diag_util_mprintf(" [%s]\n", tbl_name);
    diag_util_mprintf("Table size              = %u\n", tbl_info.size);
    diag_util_mprintf("Table datareg_num       = %u\n", tbl_info.datareg_num);
    diag_util_mprintf("Table field_num         = %u\n", tbl_info.field_num);
    diag_util_mprintf("Table Index             = %u\n", tbl_info.index);

    for (entry_idx = entry_start; entry_idx <= entry_end ; entry_idx++)
    {
        osal_memset(eValue, 0, sizeof(eValue));
        /* Get table entry value */
        ret = rtk_diag_tableEntry_get(unit, tbl_info.index , entry_idx, eValue);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("Get table entry failed! (%d %d)\n", tbl_info.index, entry_idx);
            goto exit;
        }
        diag_util_mprintf(" %u|", entry_idx);
        diag_util_mprintf("");

        DIAG_PRINT_TBL_VAL(datareg_idx, tbl_info.datareg_num, eValue);

        diag_util_mprintf("\n");

        if(print_fld)
        {   /* Display Field */
            diag_util_mprintf("  <");
            for (field_idx = 0; field_idx < tbl_info.field_num; field_idx++)
            {
                osal_memset(fValue, 0, sizeof(fValue));
                ret = rt_util_tblEntry2Field(unit, tbl_info.index,
                                                  field_idx ,fValue,
                                                  eValue);

                if(ret != RT_ERR_OK)
                {
                    goto exit;
                }
                if(field_idx > 0)
                {
                    diag_util_mprintf(",");
                }
                DIAG_PRINT_TBL_FIELD_NAME(pField_info[field_idx]);
                diag_util_mprintf("=");
                DIAG_PRINT_TBL_FIELD_VAL(i, pField_info[field_idx].len,fValue);

            }
            diag_util_mprintf(">\n");
        }
    }

exit:
    osal_free(pField_info);
    if(ret != RT_ERR_OK)
    {
        diag_util_mprintf("Dump Table error\n");
        return CPARSER_NOT_OK;
    }
#else
    diag_util_mprintf("Not support; Please enable CONFIG_SDK_DUMP_TABLE_WITH_NAME\n");
#endif
    return CPARSER_OK;
}
#endif

#ifdef CMD_DIAG_LIST_TABLE_KEYWORD
cparser_result_t cparser_cmd_diag_list_table_keyword(
    cparser_context_t *context,
    char **keyword_ptr)
{
#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    uint32 unit, field_idx;
    int32 ret, tbl ;
    rtk_diag_tblInfo_t tbl_info;
    rtk_diag_tblFieldInfo_t *pField_info;
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* Keyword to upper case */
    diag_util_str2upper(*keyword_ptr);

    /* Get REG information - name, offset, array start/end etc...*/
    ret = rtk_diag_tableInfoByStr_get(unit, *keyword_ptr, &tbl_info);
    if(ret != RT_ERR_OK)
    {
        goto do_partial_match;
    }
    /* Get Field information - name, lsp etc...*/
    pField_info = osal_alloc(tbl_info.field_num * sizeof(rtk_diag_tblFieldInfo_t));
    if(NULL == pField_info)
    {
        return RT_ERR_MEM_ALLOC;
    }
    osal_memset(pField_info, 0, sizeof(tbl_info.field_num * sizeof(rtk_diag_tblFieldInfo_t)));
    for (field_idx = 0; field_idx < tbl_info.field_num; field_idx++)
    {
        ret = rtk_diag_tableFieldInfo_get(unit,
                                          tbl_info.index,
                                          field_idx,
                                          &pField_info[field_idx]);

        if(ret != RT_ERR_OK)
        {
            //Should not enter here
            diag_util_mprintf("Get Field info failed!\n");
            osal_free(pField_info);
            return CPARSER_NOT_OK;
        }
        if(pField_info[field_idx].len == 1)
        {
            diag_util_mprintf("%s<%d>\n", pField_info[field_idx].name, pField_info[field_idx].lsp );
        }
        else
        {
            diag_util_mprintf("%s<%d,%d>\n", pField_info[field_idx].name,
                                           pField_info[field_idx].lsp+pField_info[field_idx].len - 1,
                                           pField_info[field_idx].lsp);
        }
    }
    osal_free(pField_info);
    return CPARSER_OK;

do_partial_match:
    /* list all partially match TBL name */
    tbl = -1;
    while((ret = rtk_diag_tableInfoByStr_match(unit, *keyword_ptr, tbl, &tbl_info)) == RT_ERR_OK)
    {
        tbl = tbl_info.index;
        diag_util_printf("%s\n", tbl_info.name);
    }

    if(tbl == -1)
    {
        diag_util_printf("NOT Found\n");
    }
#else
        diag_util_mprintf("Not support; Please enable CONFIG_SDK_DUMP_TABLE_WITH_NAME\n");
#endif
    return CPARSER_OK;
}
#endif

#ifdef CMD_DIAG_MODIFY_TABLE_TBL_NAME_START_INDEX_END_INDEX_FIELD_VAL_PAIRS
/*
 *  diag modify table <STRING:tbl_name> <UINT:start_index> <UINT:end_index> <STRING:field_val_pairs>
 */
cparser_result_t cparser_cmd_diag_modify_table_tbl_name_start_index_end_index_field_val_pairs(cparser_context_t *context,
    char **tbl_name_ptr,
    uint32_t *start_index_ptr,
    uint32_t *end_index_ptr,
    char **field_val_pairs_ptr)
{
#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)
#define DIAG_CMD_FIELD_VALUE_PAIR_MAX 16
    char *pField_str, *pValue_str, *pStr=NULL, *pSave=NULL;
    uint32 fldIdx[DIAG_CMD_FIELD_VALUE_PAIR_MAX];
    uint32 fldVal[DIAG_CMD_FIELD_VALUE_PAIR_MAX][RTK_DIAG_TBL_FIELD_WORDS_MAX];
    uint32 eValue[RTK_DIAG_TBL_DATAREG_NUM_MAX];
    uint32 pair_cnt = 0, pair_idx, unit, field_idx, entry_idx, cnt, word_idx, tmp;
    int32 ret = RT_ERR_OK, found;
    rtk_diag_tblInfo_t tbl_info;
    rtk_diag_tblFieldInfo_t *pField_info=NULL;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    /* TBL name to upper case */
    diag_util_str2upper(*tbl_name_ptr);

    /* Get TBL information - name, offset, array start/end etc...*/
    ret = rtk_diag_tableInfoByStr_get(unit, *tbl_name_ptr, &tbl_info);
    if(ret != RT_ERR_OK)
    {
        diag_util_mprintf("Table not Found\n");
        return CPARSER_NOT_OK;
    }

    if((*start_index_ptr >= tbl_info.size) || (*end_index_ptr >= tbl_info.size))
    {
        diag_util_mprintf("Index (%d, %d) is over table size %d\n",
                          *start_index_ptr, *end_index_ptr, tbl_info.size);
        return CPARSER_NOT_OK;
    }

    /* Get Field information - name, lsp etc...*/
    pField_info = osal_alloc(tbl_info.field_num * sizeof(rtk_diag_tblFieldInfo_t));
    if(NULL == pField_info)
    {
        return RT_ERR_MEM_ALLOC;
    }
    osal_memset(pField_info, 0, sizeof(tbl_info.field_num * sizeof(rtk_diag_tblFieldInfo_t)));

    for (field_idx = 0; field_idx < tbl_info.field_num; field_idx++)
    {
        ret = rtk_diag_tableFieldInfo_get(unit, tbl_info.index, field_idx, &pField_info[field_idx]);
        if(ret != RT_ERR_OK)
        {
            goto exit;
        }
    }

    /* Parse fields and values */
    pStr = *field_val_pairs_ptr;
    while (((pField_str = osal_strtok_r(pStr, "=", &pSave)) != NULL) &&
           ((pValue_str = osal_strtok_r(NULL, ",", &pSave)) != NULL) &&
            (pair_cnt < DIAG_CMD_FIELD_VALUE_PAIR_MAX))
    {
        pStr = NULL;

        /* Convert string to uint32 array */
        cnt = RTK_DIAG_TBL_FIELD_WORDS_MAX;
        osal_memset(fldVal[pair_cnt], 0, RTK_DIAG_TBL_FIELD_WORDS_MAX * sizeof(uint32));
        ret = diag_util_convert_mword_string_2_int32_array(
                        pValue_str, "-", fldVal[pair_cnt], &cnt);
        if(ret != RT_ERR_OK)
        {
            diag_util_mprintf("Input format error!\n");
            goto exit;
        }

        /* Field name to upper case */
        diag_util_str2upper(pField_str);

        /* Compare name to find field index */
        found = 0;
        for (field_idx = 0; field_idx < tbl_info.field_num; field_idx++)
        {
            if(osal_strcmp(pField_info[field_idx].name, pField_str) == 0)
            {
                fldIdx[pair_cnt] = field_idx;

                /* Check the length of value */
                /*if((cnt == 1) && (fldVal[pair_cnt][0] == 0))
                {   // 0,0x0 can also represent 0x0-0, 0x0-0-0, etc...
                    cnt = (BITS2WORD(pField_info[field_idx].len));
                }*/
                if((BITS2WORD(pField_info[field_idx].len)) != cnt)
                {
                    diag_util_mprintf("field %s value should have %d bits\n",
                                      pField_str, pField_info[field_idx].len);
                    continue;
                }
                /* reverse the input field word order */
                for (word_idx = 0; word_idx < cnt/2; word_idx++)
                {
                    tmp = fldVal[pair_cnt][word_idx];
                    fldVal[pair_cnt][word_idx] = fldVal[pair_cnt][cnt - word_idx -1];
                    fldVal[pair_cnt][cnt - word_idx -1] = tmp;
                }
                pair_cnt++;
                found = 1;
            }
        }
        if(found == 0)
        {
            diag_util_mprintf("Unknown field %s\n", pField_str);
        }
    }

    if((pField_str != NULL) && (pair_cnt == DIAG_CMD_FIELD_VALUE_PAIR_MAX))
    {
        diag_util_mprintf("Over the Maximum Field number %d\n",DIAG_CMD_FIELD_VALUE_PAIR_MAX);
    }

    if(pair_cnt == 0)
    {
        diag_util_mprintf("Field-value pair input format error\n");
        goto exit;
    }

    /* Call RTK API to set value of fields */
    for (entry_idx=*start_index_ptr; entry_idx <=*end_index_ptr; entry_idx++ )
    {
        osal_memset(eValue, 0, sizeof(eValue));
        // Get table entry value
        ret = rtk_diag_tableEntry_get(unit, tbl_info.index , entry_idx, eValue);
        if (ret != RT_ERR_OK)
        {
            goto exit;
        }

        // modify all needed field values in sram
        for (pair_idx = 0; pair_idx < pair_cnt; pair_idx++)
        {
            ret = rt_util_field2TblEntry(unit, tbl_info.index,
                                          fldIdx[pair_idx], fldVal[pair_idx], eValue);

            if(ret != RT_ERR_OK)
            {
                goto exit;
            }
        }
        // Write to table
        ret = rtk_diag_tableEntry_set(unit, tbl_info.index,
                                      entry_idx, eValue);
    }

exit:
    osal_free(pField_info);
    if(ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_mprintf("Not support; Please enable CONFIG_SDK_DUMP_TABLE_WITH_NAME\n");
#endif

    return CPARSER_OK;
}
#endif

/*
 *  diag register dump
 */
/*cparser_result_t cparser_cmd_diag_register_dump(cparser_context_t *context)
{
    uint32      unit;
    //int32       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    //DIAG_UTIL_ERR_CHK(rtk_diag_reg_dump(unit, NULL, 0), ret);

    return CPARSER_OK;
}*/



#ifdef CMD_DIAG_WRITE_TABLE_DATAREG_INDEX_ENTRY_INDEX_DATAREG_INDEX_VALUE
/*
 * diag write table datareg <UINT:index> <UINT:entry_index> <UINT:datareg_index> <UINT:value>
 */
cparser_result_t cparser_cmd_diag_write_table_datareg_index_entry_index_datareg_index_value(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *entry_index_ptr,
    uint32_t *datareg_index_ptr,
    uint32_t *value_ptr)
{
    uint32      unit;
    int32       ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_diag_tableEntryDatareg_write(unit, *index_ptr, *entry_index_ptr, *datareg_index_ptr, value_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_DIAG_WRITE_TABLE_DATAREG_NAME_ENTRY_INDEX_DATAREG_INDEX_VALUE
/*
 * diag write table datareg <STRING:name> <UINT:entry_index> <UINT:datareg_index> <UINT:value>
 */
cparser_result_t cparser_cmd_diag_write_table_datareg_name_entry_index_datareg_index_value(cparser_context_t *context,
    char **name_ptr,
    uint32_t *entry_index_ptr,
    uint32_t *datareg_index_ptr,
    uint32_t *value_ptr)
{
#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    uint32 unit;
    int32 ret;
    rtk_diag_tblInfo_t tbl_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* Name to upper case */
    diag_util_str2upper(*name_ptr);

    /* Get table info - index, name etc... */
    ret = rtk_diag_tableInfoByStr_get(unit, *name_ptr, &tbl_info);
    if(ret != RT_ERR_OK)
    {
        diag_util_mprintf("Table not found\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_diag_tableEntryDatareg_write(unit, tbl_info.index, *entry_index_ptr, *datareg_index_ptr, value_ptr), ret);
#else
    diag_util_mprintf("Not support; Please enable CONFIG_SDK_DUMP_TABLE_WITH_NAME\n");
#endif
    return CPARSER_OK;
}
#endif

#ifdef CMD_DIAG_WRITE_TABLE_ENTRY_INDEX_ENTRY_INDEX_VALUE
/*
 * diag write table entry <UINT:index> <UINT:entry_index> <STRING:value>
 */
cparser_result_t cparser_cmd_diag_write_table_entry_index_entry_index_value(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *entry_index_ptr,
    char **value_ptr)
{
    uint32 unit, cnt = RTK_DIAG_TABLE_DATAREG_NUM_MAX;
    int32  ret;
    uint32 value[RTK_DIAG_TABLE_DATAREG_NUM_MAX] = {0};
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* Convert the data string to uint32 array */
    if((ret = diag_util_convert_mword_string_2_int32_array(*value_ptr, "-", value, &cnt)) != RT_ERR_OK)
    {
        diag_util_mprintf("Input format error!\n");
        return ret;
    }
    /*{
        int32 i;
        diag_util_mprintf("cparser ");
        for(i = 0; i< cnt; i++)
            diag_util_mprintf("0x%x ",value[i]);
        diag_util_mprintf("\n");
    }*/
    /* Write to table */
    DIAG_UTIL_ERR_CHK(rtk_diag_tableEntry_write(unit, *index_ptr, *entry_index_ptr, value, cnt), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_DIAG_WRITE_TABLE_ENTRY_NAME_ENTRY_INDEX_VALUE
/*
 * diag write table entry <STRING:name> <UINT:entry_index> <STRING:value>
 */
cparser_result_t cparser_cmd_diag_write_table_entry_name_entry_index_value(cparser_context_t *context,
    char **name_ptr,
    uint32_t *entry_index_ptr,
    char **value_ptr)
{
#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    uint32      unit, cnt = RTK_DIAG_TABLE_DATAREG_NUM_MAX;
    int32       ret;
    uint32      value[RTK_DIAG_TABLE_DATAREG_NUM_MAX] = {0};
    rtk_diag_tblInfo_t tbl_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* Name to upper case */
    diag_util_str2upper(*name_ptr);

    /* Get table info - index, name etc... */
    ret = rtk_diag_tableInfoByStr_get(unit, *name_ptr, &tbl_info);
    if(ret != RT_ERR_OK)
    {
        diag_util_mprintf("Table not found\n");
        return CPARSER_NOT_OK;
    }

    /* Convert the data string to uint32 array */
    cnt = tbl_info.datareg_num;
    if((ret = diag_util_convert_mword_string_2_int32_array(*value_ptr, "-", value, &cnt)) != RT_ERR_OK)
    {
        diag_util_mprintf("Input format error!\n");
        return ret;
    }

    if(cnt != (tbl_info.datareg_num))
    {
        diag_util_mprintf("Value length is too short!\n");
        return ret;
    }

    /* Write to table */
    DIAG_UTIL_ERR_CHK(rtk_diag_tableEntry_write(unit, tbl_info.index, *entry_index_ptr, value, cnt), ret);
#else
    diag_util_mprintf("Not support; Please enable CONFIG_SDK_DUMP_TABLE_WITH_NAME\n");
#endif
    return CPARSER_OK;
}
#endif


#ifdef CMD_DIAG_WHOLEDUMP_MAC_REG_PHY_REG_SOC_REG_TABLE_ALL
cparser_result_t cparser_cmd_diag_wholedump_mac_reg_phy_reg_soc_reg_table_all(cparser_context_t *context)
{
    int32 return_value;
    uint32 unit;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('s' == TOKEN_CHAR(2,0))        /*Dump SoC registers*/
    {
            DIAG_UTIL_ERR_CHK(rtk_diag_peripheral_register_dump(unit), return_value);
    }
    else if('m' == TOKEN_CHAR(2,0))    /*Dump MAC registers*/
    {
          DIAG_UTIL_ERR_CHK(rtk_diag_reg_whole_read(unit), return_value);
    }
    else if('t'== TOKEN_CHAR(2,0))    /*Dump tables*/
    {
          DIAG_UTIL_ERR_CHK(rtk_diag_table_whole_read(unit, 0xff), return_value);
    }
    else if('p'== TOKEN_CHAR(2,0)) /*Dump PHY registers*/
    {
          DIAG_UTIL_ERR_CHK(rtk_diag_phy_reg_whole_read(unit), return_value);
    }
    else if('a'== TOKEN_CHAR(2,0)) /*Dump All*/
    {
          DIAG_UTIL_ERR_CHK(rtk_diag_peripheral_register_dump(unit), return_value);
          DIAG_UTIL_ERR_CHK(rtk_diag_reg_whole_read(unit), return_value);
          DIAG_UTIL_ERR_CHK(rtk_diag_phy_reg_whole_read(unit), return_value);
          DIAG_UTIL_ERR_CHK(rtk_diag_table_whole_read(unit, 0xff), return_value);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_DIAG_GET_PHY_PHYID_SERDES_LINK_STATUS
/*
 * diag get phy <UINT:phyId> serdes link-status
 */
cparser_result_t
cparser_cmd_diag_get_phy_phyId_serdes_link_status(
    cparser_context_t *context,
    uint32_t *phyId_ptr)
{
    rtk_switch_devInfo_t    devInfo;
    rtk_port_t              port;
    uint32                  unit;
    uint32                  maxPage = 0x1fff, regVal, oriReg;
    uint32                  sdsPage[] = {0x40f, 0x42f};
    int32                   ret;
    uint8                   i;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    port = (*phyId_ptr) * PORT_NUM_IN_8218B;

    if (RTK_PORTMASK_IS_PORT_SET(devInfo.serdes.portmask, port))
        return CPARSER_OK;

    if (!RTK_PORTMASK_IS_PORT_SET(devInfo.ether.portmask, port))
        return CPARSER_OK;

    DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, 0, 3, &regVal), ret);
    if (regVal == 0xC981)
    {
        diag_util_mprintf("PHY Port ID: %d\n", port);
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, maxPage, 30, &oriReg), ret);
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, 8), ret);

        for (i = 0; i < sizeof(sdsPage)/sizeof(uint32); ++i)
        {
            if ((ret = rtk_port_phyReg_get(unit, port, sdsPage[i], 0x16, &regVal)) != RT_ERR_OK)
            {
                rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg);
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
            diag_util_mprintf(" status %d: 0x%04x\n", i, regVal);
        }

        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_get_phy_phyId_serdes_link_status */
#endif

#ifdef CMD_DIAG_GET_PHY_PHYID_SERDES_RX_SYM_ERR
/*
 * diag get phy <UINT:phyId> serdes rx-sym-err
 */
cparser_result_t
cparser_cmd_diag_get_phy_phyId_serdes_rx_sym_err(
    cparser_context_t *context,
    uint32_t *phyId_ptr)
{
    rtk_switch_devInfo_t    devInfo;
    rtk_port_t              port;
    uint32                  unit;
    uint32                  maxPage = 0x1fff, regVal, oriReg;
    uint32                  sdsPage[] = {0x40f, 0x42f};
    int32                   ret;
    uint8                   i, j;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    port = (*phyId_ptr) * PORT_NUM_IN_8218B;

    if (RTK_PORTMASK_IS_PORT_SET(devInfo.serdes.portmask, port))
        return CPARSER_OK;

    if (!RTK_PORTMASK_IS_PORT_SET(devInfo.ether.portmask, port))
        return CPARSER_OK;

    DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, 0, 3, &regVal), ret);
    if (regVal == 0xC981)
    {
        diag_util_mprintf("PHY Port ID: %d\n", port);
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, maxPage, 30, &oriReg), ret);
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, 8), ret);

        for (i = 0; i < sizeof(sdsPage)/sizeof(uint32); ++i)
        {
            diag_util_mprintf(" Sds ID: %d\n", i);

            for (j = 0x10; j <= 0x13; ++j)
            {
                if ((ret = rtk_port_phyReg_set(unit, port, sdsPage[i], 0x10, j)) != RT_ERR_OK)
                {
                    rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg);
                    DIAG_ERR_PRINT(ret);
                    return CPARSER_NOT_OK;
                }

                if ((ret = rtk_port_phyReg_get(unit, port, sdsPage[i], 0x11, &regVal)) != RT_ERR_OK)
                {
                    rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg);
                    DIAG_ERR_PRINT(ret);
                    return CPARSER_NOT_OK;
                }
                diag_util_mprintf("  CH%d: 0x%04x\n", (j - 0x10), regVal);
            }
        }

        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_get_phy_phyId_serdes_rx_sym_err */
#endif

#ifdef CMD_DIAG_CLEAR_PHY_PHYID_SERDES_RX_SYM_ERR
/*
 * diag clear phy <UINT:phyId> serdes rx-sym-err
 */
cparser_result_t
cparser_cmd_diag_clear_phy_phyId_serdes_rx_sym_err(
    cparser_context_t *context,
    uint32_t *phyId_ptr)
{
    rtk_switch_devInfo_t    devInfo;
    rtk_port_t              port;
    uint32                  unit;
    uint32                  maxPage = 0x1fff, regVal, oriReg;
    uint32                  sdsPage[] = {0x40f, 0x42f};
    int32                   ret;
    uint8                   i, j;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    port = (*phyId_ptr) * PORT_NUM_IN_8218B;

    if (RTK_PORTMASK_IS_PORT_SET(devInfo.serdes.portmask, port))
        return CPARSER_OK;

    if (!RTK_PORTMASK_IS_PORT_SET(devInfo.ether.portmask, port))
        return CPARSER_OK;

    DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, 0, 3, &regVal), ret);
    if (regVal == 0xC981)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, maxPage, 30, &oriReg), ret);
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, 8), ret);

        for (i = 0; i < sizeof(sdsPage)/sizeof(uint32); ++i)
        {
            for (j = 0x10; j <= 0x13; ++j)
            {
                if ((ret = rtk_port_phyReg_set(unit, port, sdsPage[i], 0x10, j)) != RT_ERR_OK)
                {
                    rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg);
                    DIAG_ERR_PRINT(ret);
                    return CPARSER_NOT_OK;
                }

                if ((ret = rtk_port_phyReg_get(unit, port, sdsPage[i], 0x11, &regVal)) != RT_ERR_OK)
                {
                    rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg);
                    DIAG_ERR_PRINT(ret);
                    return CPARSER_NOT_OK;
                }
            }
        }

        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_clear_phy_phyId_serdes_rx_sym_err */
#endif

#ifdef CMD_DIAG_GET_PHY_PORT_PORTS_ALL_RX_CNT
/*
 * diag get phy port ( <PORT_LIST:ports> | all ) rx-cnt
 */
cparser_result_t
cparser_cmd_diag_get_phy_port_ports_all_rx_cnt(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtk_switch_devInfo_t    devInfo;
    diag_portlist_t         portlist;
    rtk_port_t              port;
    uint32                  unit;
    uint32                  maxPage = 0x1fff, regVal, oriReg;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if (RTK_PORTMASK_IS_PORT_SET(devInfo.serdes.portmask, port))
            continue;

        if (!RTK_PORTMASK_IS_PORT_SET(devInfo.ether.portmask, port))
            continue;

        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, 0, 3, &regVal), ret);
        if (regVal == 0xC981)
        {
            diag_util_mprintf("Port ID: %d\n", port);

            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, maxPage, 30, &oriReg), ret);
            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, 0), ret);

            if ((ret = rtk_port_phyReg_get(unit, port, 0xc81, 0x10, &regVal)) != RT_ERR_OK)
            {
                rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg);
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
            diag_util_mprintf(" good1: 0x%x\n", regVal);

            if ((ret = rtk_port_phyReg_get(unit, port, 0xc81, 0x11, &regVal)) != RT_ERR_OK)
            {
                rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg);
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
            diag_util_mprintf(" good2: 0x%x\n", regVal);

            if ((ret = rtk_port_phyReg_get(unit, port, 0xc81, 0x12, &regVal)) != RT_ERR_OK)
            {
                rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg);
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
            diag_util_mprintf("    err: 0x%x\n", regVal);

            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg), ret);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_get_phy_port_ports_all_rx_cnt */
#endif

#ifdef CMD_DIAG_CLEAR_PHY_PORT_PORTS_ALL_RX_CNT
/*
 * diag clear phy port ( <PORT_LIST:ports> | all ) rx-cnt
 */
cparser_result_t
cparser_cmd_diag_clear_phy_port_ports_all_rx_cnt(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtk_switch_devInfo_t    devInfo;
    diag_portlist_t         portlist;
    rtk_port_t              port;
    uint32                  unit;
    uint32                  maxPage = 0x1fff, regVal, oriReg;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if (RTK_PORTMASK_IS_PORT_SET(devInfo.serdes.portmask, port))
            continue;

        if (!RTK_PORTMASK_IS_PORT_SET(devInfo.ether.portmask, port))
            continue;

        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, 0, 3, &regVal), ret);
        if (regVal == 0xC981)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, maxPage, 30, &oriReg), ret);
            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, 0), ret);

            if ((ret = rtk_port_phyReg_set(unit, port, 0xc80, 0x11, 0x73)) != RT_ERR_OK)
            {
                rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg);
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }

            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg), ret);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_clear_phy_port_ports_all_rx_cnt */
#endif

#ifdef CMD_DIAG_SET_PHY_PORT_PORTS_ALL_RX_CNT_MAC_TX_PHY_RX
/*
 * diag set phy port ( <PORT_LIST:ports> | all ) rx-cnt ( mac-tx | phy-rx )
 */
cparser_result_t
cparser_cmd_diag_set_phy_port_ports_all_rx_cnt_mac_tx_phy_rx(
    cparser_context_t *context,
    char **ports_ptr)
{
    rtk_switch_devInfo_t    devInfo;
    diag_portlist_t         portlist;
    rtk_port_t              port;
    uint32                  unit, val;
    uint32                  maxPage = 0x1fff, regVal, oriReg;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    if ('m' == TOKEN_CHAR(6, 0))
        val = 0x6;
    else
        val = 0x2;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if (RTK_PORTMASK_IS_PORT_SET(devInfo.serdes.portmask, port))
            continue;

        if (!RTK_PORTMASK_IS_PORT_SET(devInfo.ether.portmask, port))
            continue;

        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, 0, 3, &regVal), ret);
        if (regVal == 0xC981)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, maxPage, 30, &oriReg), ret);
            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, 0), ret);

            if ((ret = rtk_port_phyReg_set(unit, port, 0xc80, 0x10, val)) != RT_ERR_OK)
            {
                rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg);
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }

            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, maxPage, 30, oriReg), ret);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_diag_set_phy_port_ports_all_rx_cnt_mac_tx_phy_rx */
#endif

#ifdef CONFIG_SDK_RTL8295R
/*
 * diag set 8295r sff-8431-test port <UINT:port> serdes <UINT:sdsId> pattern ( init | square8180 | prbs9 | prbs31 | disable )
 */
cparser_result_t
cparser_cmd_diag_set_8295r_sff_8431_test_port_port_serdes_sdsId_pattern_init_square8180_prbs9_prbs31_disable(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit, mdxMacId;
    int32   ret, len;
    char    *pattern = TOKEN_STR(9);

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (*sdsId_ptr > 1)
    {
        diag_util_printf("Invalid serdes ID %u\n", *sdsId_ptr);
        return CPARSER_OK;
    }

    mdxMacId = *port_ptr;
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID))
    {
        if (*port_ptr == 24)
            mdxMacId = 25;
        else if (*port_ptr == 36)
            mdxMacId = 26;
    }

    len = strlen(pattern);

    if (!strncmp("disable", pattern, len))
    {
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x5, 0xA, 0x0000), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x6, 0xE, 0x0000), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x6, 0xF, 0x0000), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x6, 0x0, 0x0000), ret);
    }
    else if (!strncmp("init", pattern, len))
    {

        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, 0, 0x2F, 0x1B, 0x0), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, 0, 0x21, 0x5,  0x4A8D), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, 1, 0x2F, 0x1B, 0x0), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, 1, 0x21, 0x5,  0x4A8D), ret);
    }
    else if (!strncmp("square8180", pattern, len))
    {
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x5, 0xA, 0x0002), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x6, 0x0, 0x6280), ret);
    }
    else if (!strncmp("prbs9", pattern, len))
    {
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x5, 0xA, 0x00C0), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x6, 0xE, 0x0002), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x6, 0xF, 0x0002), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x6, 0x0, 0x6200), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_reg_write(unit, mdxMacId, 0x34, 0x6), ret);
    }
    else if (!strncmp("prbs31", pattern, len))
    {
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x5, 0xA, 0x0030), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x6, 0xE, 0x0002), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x6, 0xF, 0x0002), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_sds_write(unit, mdxMacId, *sdsId_ptr, 0x6, 0x0, 0x6200), ret);
        DIAG_UTIL_ERR_CHK(hal_rtl8295_reg_write(unit, mdxMacId, 0x34, 0x6), ret);
    }
    return CPARSER_OK;
}
#endif

#ifdef CONFIG_SDK_RTL8295R
/*
 * diag set 8295r rx-cali param-cfg port <UINT:port> dac-long-cable-offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_diag_set_8295r_rx_cali_param_cfg_port_port_dac_long_cable_offset_offset(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *offset_ptr)
{
    uint32  unit;
    int32   ret;
    phy_8295_rxCaliConf_t   rxCaliConf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    memset(&rxCaliConf, 0, sizeof(phy_8295_rxCaliConf_t));
    if ((ret = phy_8295r_rxCaliConfPort_get(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    rxCaliConf.dacLongCableOffset = *offset_ptr;

    if ((ret = phy_8295r_rxCaliConfPort_set(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Set RX-Cali. config failed: 0x%x\n", ret);
    }

    return CPARSER_OK;

}
#endif


#ifdef CONFIG_SDK_RTL8295R
/*
 * diag set 8295r rx-cali param-cfg port <UINT:port> <UINT:sdsId> vth-min <UINT:threshold> tap0_init <UINT:value>
 */
cparser_result_t
cparser_cmd_diag_set_8295r_rx_cali_param_cfg_port_port_sdsId_vth_min_threshold_tap0_init_value(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr,
    uint32_t *threshold_ptr,
    uint32_t *value_ptr)
{
    uint32  unit;
    int32   ret;
    phy_8295_rxCaliConf_t   rxCaliConf;
    phy_8295_rxCaliSdsConf_t    *sdsCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    memset(&rxCaliConf, 0, sizeof(phy_8295_rxCaliConf_t));
    if ((ret = phy_8295r_rxCaliConfPort_get(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    if (*sdsId_ptr == 0)
    {
        sdsCfg = &rxCaliConf.s0;
    }
    else if (*sdsId_ptr == 1)
    {
        sdsCfg = &rxCaliConf.s1;
    }
    else
    {
        diag_util_printf("Invalid serdes ID: %u\n", *sdsId_ptr);
        return CPARSER_OK;
    }

    sdsCfg->vthMinThr   = *threshold_ptr;
    sdsCfg->tap0InitVal = *value_ptr;

    if ((ret = phy_8295r_rxCaliConfPort_set(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Set RX-Cali. config failed: 0x%x\n", ret);
    }

    return CPARSER_OK;
}
#endif


#ifdef CONFIG_SDK_RTL8295R
/*
 * diag set 8295r rx-cali param-cfg port <UINT:port> <UINT:sdsId> eq-hold ( enable | disable ) dfe-tap1-4 ( enable | disable ) dfe-auto ( enable | disable )
 */
cparser_result_t
cparser_cmd_diag_set_8295r_rx_cali_param_cfg_port_port_sdsId_eq_hold_enable_disable_dfe_tap1_4_enable_disable_dfe_auto_enable_disable(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret, len;
    phy_8295_rxCaliConf_t   rxCaliConf;
    phy_8295_rxCaliSdsConf_t    *sdsCfg;
    char *eq_hold_enStr, *defTap1_4_enStr, *defAuto_enStr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    memset(&rxCaliConf, 0, sizeof(phy_8295_rxCaliConf_t));
    if ((ret = phy_8295r_rxCaliConfPort_get(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    if (*sdsId_ptr == 0)
        sdsCfg = &rxCaliConf.s0;
    else if (*sdsId_ptr == 1)
        sdsCfg = &rxCaliConf.s1;
    else
    {
        diag_util_printf("Invalid serdes ID: %u\n", *sdsId_ptr);
        return CPARSER_OK;
    }

    eq_hold_enStr = TOKEN_STR(9);
    len = strlen(eq_hold_enStr);
    if (!strncmp("enable", eq_hold_enStr, len))
        sdsCfg->eqHoldEnable = ENABLED;
    else
        sdsCfg->eqHoldEnable = DISABLED;

    defTap1_4_enStr = TOKEN_STR(11);
    len = strlen(defTap1_4_enStr);
    if (!strncmp("enable", defTap1_4_enStr, len))
        sdsCfg->dfeTap1_4Enable = ENABLED;
    else
        sdsCfg->dfeTap1_4Enable = DISABLED;

    defAuto_enStr = TOKEN_STR(13);
    len = strlen(defAuto_enStr);
    if (!strncmp("enable", defAuto_enStr, len))
        sdsCfg->dfeAuto = ENABLED;
    else
        sdsCfg->dfeAuto = DISABLED;


    if ((ret = phy_8295r_rxCaliConfPort_set(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Set RX-Cali. config failed: 0x%x\n", ret);
    }

    return CPARSER_OK;

}
#endif

#ifdef CONFIG_SDK_RTL8295R
/*
 * diag set 8295r rx-cali param-cfg port <UINT:port> <UINT:sdsId> rx-cali ( enable | disable )
 */
cparser_result_t
cparser_cmd_diag_set_8295r_rx_cali_param_cfg_port_port_sdsId_rx_cali_enable_disable(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret, len;
    phy_8295_rxCaliConf_t   rxCaliConf;
    phy_8295_rxCaliSdsConf_t    *sdsCfg;
    char *enStr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    memset(&rxCaliConf, 0, sizeof(phy_8295_rxCaliConf_t));
    if ((ret = phy_8295r_rxCaliConfPort_get(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    if (*sdsId_ptr == 0)
        sdsCfg = &rxCaliConf.s0;
    else if (*sdsId_ptr == 1)
        sdsCfg = &rxCaliConf.s1;
    else
    {
        diag_util_printf("Invalid serdes ID: %u\n", *sdsId_ptr);
        return CPARSER_OK;
    }

    enStr = TOKEN_STR(9);
    len = strlen(enStr);
    if (!strncmp("enable", enStr, len))
        sdsCfg->rxCaliEnable = ENABLED;
    else
        sdsCfg->rxCaliEnable = DISABLED;

    if ((ret = phy_8295r_rxCaliConfPort_set(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Set RX-Cali. config failed: 0x%x\n", ret);
    }

    return CPARSER_OK;

}
#endif

#ifdef CONFIG_SDK_RTL8295R
/*
 * diag get 8295r rx-cali param-cfg port <UINT:port>
 */
cparser_result_t
cparser_cmd_diag_get_8295r_rx_cali_param_cfg_port_port(cparser_context_t *context,
    uint32_t *port_ptr)
{
    uint32  unit;
    int32   ret;
    phy_8295_rxCaliConf_t   rxCaliConf;
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
        familyId = 0xFFFF;
    }
    else
    {
        familyId = (familyId >> 16) & 0xFFFF;
    }

    memset(&rxCaliConf, 0, sizeof(phy_8295_rxCaliConf_t));
    if ((ret = phy_8295r_rxCaliConfPort_get(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    diag_util_printf("phy_8295_rxCaliConf_t rtl8295r_%x_rxCaliConf_myParam_unit%u_port%u =\n", familyId, unit, *port_ptr);
    diag_util_printf("        {\n");
    diag_util_printf("            .dacLongCableOffset             = %u,\n", rxCaliConf.dacLongCableOffset);
    diag_util_printf("\n");
    diag_util_printf("            .s1.rxCaliEnable                = %s,\n", (rxCaliConf.s1.rxCaliEnable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("            .s1.tap0InitVal                 = 0x%X,\n", rxCaliConf.s1.tap0InitVal);
    diag_util_printf("            .s1.vthMinThr                   = 0x%X,\n", rxCaliConf.s1.vthMinThr);
    diag_util_printf("            .s1.eqHoldEnable                = %s,\n", (rxCaliConf.s1.eqHoldEnable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("            .s1.dfeTap1_4Enable             = %s,\n", (rxCaliConf.s1.dfeTap1_4Enable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("            .s1.dfeAuto                     = %s,\n", (rxCaliConf.s1.dfeAuto==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("\n");
    diag_util_printf("            .s0.rxCaliEnable                = %s,\n", (rxCaliConf.s0.rxCaliEnable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("            .s0.tap0InitVal                 = 0x%X,\n", rxCaliConf.s0.tap0InitVal);
    diag_util_printf("            .s0.vthMinThr                   = 0x%X,\n", rxCaliConf.s0.vthMinThr);
    diag_util_printf("            .s0.eqHoldEnable                = %s,\n", (rxCaliConf.s0.eqHoldEnable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("            .s0.dfeTap1_4Enable             = %s,\n", (rxCaliConf.s0.dfeTap1_4Enable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("            .s0.dfeAuto                     = %s,\n", (rxCaliConf.s0.dfeAuto==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("        };\n");

    return CPARSER_OK;

}
#endif


#ifdef CONFIG_SDK_RTL8295R
/*
 * diag set 8295r rx-cali <UINT:port> <UINT:sdsId> ( enable | disable )
 */
cparser_result_t
cparser_cmd_diag_set_8295r_rx_cali_port_sdsId_enable_disable(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit, len, dummy = 0;
    int32   ret;
    char    *str;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    str = TOKEN_STR(6);
    len = strlen(str);
    if (!strncmp("enable", str, len))
    {
        DIAG_UTIL_ERR_CHK(phy_8295_diag_set(unit, *port_ptr, dummy, *sdsId_ptr, (uint8 *)"port_cali_enable"), ret);
    }
    else if (!strncmp("disable", str, len))
    {
        DIAG_UTIL_ERR_CHK(phy_8295_diag_set(unit, *port_ptr, dummy, *sdsId_ptr, (uint8 *)"port_cali_disable"), ret);
    }

    return CPARSER_OK;
}
#endif


#ifdef CONFIG_SDK_RTL8295R
/*
 * diag set 8295r rx-cali <UINT:port> <UINT:sdsId> start
 */
cparser_result_t
cparser_cmd_diag_set_8295r_rx_cali_port_sdsId_start(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit, dummy = 0;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(phy_8295_diag_set(unit, *port_ptr, dummy, *sdsId_ptr, (uint8 *)"rxCali"), ret);
    return CPARSER_OK;
}
#endif


#ifdef CONFIG_SDK_RTL8214QF
/*
 * diag set 8214qf rx-cali param-cfg port <UINT:port> <UINT:sdsId> vth-min <UINT:threshold> tap0_init <UINT:value>
 */
cparser_result_t
cparser_cmd_diag_set_8214qf_rx_cali_param_cfg_port_port_sdsId_vth_min_threshold_tap0_init_value(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr,
    uint32_t *threshold_ptr,
    uint32_t *value_ptr)
{
    uint32  unit;
    int32   ret;
    phy_8295_rxCaliConf_t   rxCaliConf;
    phy_8295_rxCaliSdsConf_t    *sdsCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    memset(&rxCaliConf, 0, sizeof(phy_8295_rxCaliConf_t));
    if ((ret = phy_8214qf_rxCaliConf_get(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    if (*sdsId_ptr == 0)
    {
        sdsCfg = &rxCaliConf.s0;
    }
    else if (*sdsId_ptr == 1)
    {
        sdsCfg = &rxCaliConf.s1;
    }
    else
    {
        diag_util_printf("Invalid serdes ID: %u\n", *sdsId_ptr);
        return CPARSER_OK;
    }

    sdsCfg->vthMinThr   = *threshold_ptr;
    sdsCfg->tap0InitVal = *value_ptr;

    if ((ret = phy_8214qf_rxCaliConf_set(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Set RX-Cali. config failed: 0x%x\n", ret);
    }

    return CPARSER_OK;
}
#endif


#ifdef CONFIG_SDK_RTL8214QF
/*
 * diag set 8214qf rx-cali param-cfg port <UINT:port> <UINT:sdsId> eq-hold ( enable | disable ) dfe-tap1-4 ( enable | disable ) dfe-auto ( enable | disable )
 */
cparser_result_t
cparser_cmd_diag_set_8214qf_rx_cali_param_cfg_port_port_sdsId_eq_hold_enable_disable_dfe_tap1_4_enable_disable_dfe_auto_enable_disable(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret, len;
    phy_8295_rxCaliConf_t   rxCaliConf;
    phy_8295_rxCaliSdsConf_t    *sdsCfg;
    char *eq_hold_enStr, *defTap1_4_enStr, *defAuto_enStr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    memset(&rxCaliConf, 0, sizeof(phy_8295_rxCaliConf_t));
    if ((ret = phy_8214qf_rxCaliConf_get(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    if (*sdsId_ptr == 0)
        sdsCfg = &rxCaliConf.s0;
    else if (*sdsId_ptr == 1)
        sdsCfg = &rxCaliConf.s1;
    else
    {
        diag_util_printf("Invalid serdes ID: %u\n", *sdsId_ptr);
        return CPARSER_OK;
    }

    eq_hold_enStr = TOKEN_STR(9);
    len = strlen(eq_hold_enStr);
    if (!strncmp("enable", eq_hold_enStr, len))
        sdsCfg->eqHoldEnable = ENABLED;
    else
        sdsCfg->eqHoldEnable = DISABLED;

    defTap1_4_enStr = TOKEN_STR(11);
    len = strlen(defTap1_4_enStr);
    if (!strncmp("enable", defTap1_4_enStr, len))
        sdsCfg->dfeTap1_4Enable = ENABLED;
    else
        sdsCfg->dfeTap1_4Enable = DISABLED;

    defAuto_enStr = TOKEN_STR(13);
    len = strlen(defAuto_enStr);
    if (!strncmp("enable", defAuto_enStr, len))
        sdsCfg->dfeAuto = ENABLED;
    else
        sdsCfg->dfeAuto = DISABLED;


    if ((ret = phy_8214qf_rxCaliConf_set(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Set RX-Cali. config failed: 0x%x\n", ret);
    }

    return CPARSER_OK;

}
#endif

#ifdef CONFIG_SDK_RTL8214QF
/*
 * diag set 8214qf rx-cali param-cfg port <UINT:port> <UINT:sdsId> rx-cali ( enable | disable )
 */
cparser_result_t
cparser_cmd_diag_set_8214qf_rx_cali_param_cfg_port_port_sdsId_rx_cali_enable_disable(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret, len;
    phy_8295_rxCaliConf_t   rxCaliConf;
    phy_8295_rxCaliSdsConf_t    *sdsCfg;
    char *enStr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    memset(&rxCaliConf, 0, sizeof(phy_8295_rxCaliConf_t));
    if ((ret = phy_8214qf_rxCaliConf_get(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    if (*sdsId_ptr == 0)
        sdsCfg = &rxCaliConf.s0;
    else if (*sdsId_ptr == 1)
        sdsCfg = &rxCaliConf.s1;
    else
    {
        diag_util_printf("Invalid serdes ID: %u\n", *sdsId_ptr);
        return CPARSER_OK;
    }

    enStr = TOKEN_STR(9);
    len = strlen(enStr);
    if (!strncmp("enable", enStr, len))
        sdsCfg->rxCaliEnable = ENABLED;
    else
        sdsCfg->rxCaliEnable = DISABLED;

    if ((ret = phy_8214qf_rxCaliConf_set(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Set RX-Cali. config failed: 0x%x\n", ret);
    }

    return CPARSER_OK;

}
#endif

#ifdef CONFIG_SDK_RTL8214QF
/*
 * diag get 8214qf rx-cali param-cfg port <UINT:port>
 */
cparser_result_t
cparser_cmd_diag_get_8214qf_rx_cali_param_cfg_port_port(cparser_context_t *context,
    uint32_t *port_ptr)
{
    uint32  unit;
    int32   ret;
    phy_8295_rxCaliConf_t   rxCaliConf;
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
        familyId = 0xFFFF;
    }
    else
    {
        familyId = (familyId >> 16) & 0xFFFF;
    }

    memset(&rxCaliConf, 0, sizeof(phy_8295_rxCaliConf_t));
    if ((ret = phy_8214qf_rxCaliConf_get(unit, *port_ptr, &rxCaliConf)) != RT_ERR_OK)
    {
        diag_util_printf("Get RX-Cali. config failed: 0x%x\n", ret);
        return CPARSER_OK;
    }

    diag_util_printf("phy_8295_rxCaliConf_t rtl8214qf_%x_rxCaliConf_myParam_unit%u_port%u =\n", familyId, unit, *port_ptr);
    diag_util_printf("        {\n");
    diag_util_printf("            .dacLongCableOffset             = %u,\n", rxCaliConf.dacLongCableOffset);
    diag_util_printf("\n");
    diag_util_printf("            .s1.rxCaliEnable                = %s,\n", (rxCaliConf.s1.rxCaliEnable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("            .s1.tap0InitVal                 = 0x%X,\n", rxCaliConf.s1.tap0InitVal);
    diag_util_printf("            .s1.vthMinThr                   = 0x%X,\n", rxCaliConf.s1.vthMinThr);
    diag_util_printf("            .s1.eqHoldEnable                = %s,\n", (rxCaliConf.s1.eqHoldEnable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("            .s1.dfeTap1_4Enable             = %s,\n", (rxCaliConf.s1.dfeTap1_4Enable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("\n");
    diag_util_printf("            .s0.rxCaliEnable                = %s,\n", (rxCaliConf.s0.rxCaliEnable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("            .s0.tap0InitVal                 = 0x%X,\n", rxCaliConf.s0.tap0InitVal);
    diag_util_printf("            .s0.vthMinThr                   = 0x%X,\n", rxCaliConf.s0.vthMinThr);
    diag_util_printf("            .s0.eqHoldEnable                = %s,\n", (rxCaliConf.s0.eqHoldEnable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("            .s0.dfeTap1_4Enable             = %s,\n", (rxCaliConf.s0.dfeTap1_4Enable==ENABLED)?"ENABLED":"DISABLED");
    diag_util_printf("        };\n");

    return CPARSER_OK;

}
#endif
