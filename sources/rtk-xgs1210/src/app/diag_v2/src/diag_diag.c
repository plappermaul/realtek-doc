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
 * $Revision: 97277 $
 * $Date: 2019-05-29 17:51:38 +0800 (Wed, 29 May 2019) $
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
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <rtk/diag.h>
#include <rtk/port.h>
#include <rtk/switch.h>
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

        if (rtctResult.linkType == PORT_SPEED_1000M)
        {
            diag_util_mprintf("Port %2d (type GE):\n", port);
            diag_util_printf("  channel A: \n");
            diag_util_printf("    Status : ");
            if (rtctResult.un.ge_result.channelAShort)
                diag_util_printf("[Short]");
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
            if (rtctResult.un.ge_result.channelBShort)
                diag_util_printf("[Short]");
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
            if (rtctResult.un.ge_result.channelCShort)
                diag_util_printf("[Short]");
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
            if (rtctResult.un.ge_result.channelDShort)
                diag_util_printf("[Short]");
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
cparser_result_t
cparser_cmd_diag_dump_table_index_start_index_end_index_detail(cparser_context_t *context,
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
