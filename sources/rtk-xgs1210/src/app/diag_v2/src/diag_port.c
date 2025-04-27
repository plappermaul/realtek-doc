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
 * $Revision: 98180 $
 * $Date: 2019-06-25 16:10:41 +0800 (Tue, 25 Jun 2019) $
 *
 * Purpose : Definition those Port command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Parameter settings for the port-based view
 *           2) RTCT
 *           3) UDLD
 *           4) RLDP
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
#include <soc/type.h>
#include <rtk/diag.h>
#include <rtk/port.h>
#include <rtk/switch.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_diag.h>
  #include <rtrpc/rtrpc_port.h>
  #include <rtrpc/rtrpc_switch.h>
#endif
#define UTIL_STRING_BUFFER_LENGTH   (30)
#define MAX_PHY_REGISTER  (31)

#ifdef CMD_PORT_DUMP_CPU_PORT
/*
 * port dump cpu-port
 */
cparser_result_t cparser_cmd_port_dump_cpu_port(cparser_context_t *context)
{
    uint32                  unit = 0;
    rtk_port_t             port = 0;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    /* show cpu port numner */
    DIAG_UTIL_ERR_CHK(rtk_port_cpuPortId_get(unit, &port), ret);
    diag_util_mprintf("CPU port: %d\n", port);

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_DUMP_ISOLATION
/*
 * port dump isolation
 */
cparser_result_t cparser_cmd_port_dump_isolation(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_portmask_t      portmask;
    char                port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    diag_portlist_t  portlist;
    rtk_switch_devInfo_t devInfo;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_enable_t enable;
#endif

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    diag_util_port_min_max_get(&(devInfo.ether.portmask), &portlist.min, &portlist.max);
    osal_memcpy(&portlist.portmask, &(devInfo.ether.portmask), sizeof(rtk_portmask_t));

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
        DIAG_UTIL_ERR_CHK(rtk_port_isolation_get(unit, port, &portmask), ret);
        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN * sizeof(uint8));
        diag_util_lPortMask2str(port_list, &portmask);
        diag_util_mprintf("\tPort %2d : Isolation Port list %s\n", port, port_list[0] == '\0' ? "is NULL":port_list);
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationRestrictRoute_get(unit, &enable), ret);

        diag_util_mprintf("\n Restrict Route:%s\n",(ENABLED == enable)? DIAG_STR_ENABLE:DIAG_STR_DISABLE);
    }
#endif

    return CPARSER_OK;
}

#endif


void cmd_print_blank(uint8 max, uint8 min){

    uint8 i = 0;
    uint8 len = max - min ;
    if(len > 0){
        for(i = 0 ; i < len ; i++){
            diag_util_printf(" ");
        }
        diag_util_printf("\t: ");

    }else if(len == 0){
        diag_util_printf("\t: ");
    }

}

#ifdef CMD_PORT_DUMP_ISOLATION_VLAN_BASED
/*
 * port dump isolation vlan-based
 */
cparser_result_t cparser_cmd_port_dump_isolation_vlan_based(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          index, maxIndex;
    uint32          total_entry = 0;
    rtk_port_vlanIsolationEntry_t entry;
    char            port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, maxIndex, max_num_of_vlan_port_iso_entry);
    osal_memset(&entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));

#if defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Index |VID  |Trust Port list       |State\n");
        diag_util_mprintf("------+-----+----------------------+---------\n");

        for (index = 0; index < maxIndex; index++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, index, &entry), ret);

            if (entry.vid != 0)
            {
                osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN * sizeof(uint8));
                diag_util_lPortMask2str(port_list, &entry.portmask);

                diag_util_mprintf("%4d  |%4d | %20s | %s   \n", index, entry.vid,
                        port_list, (entry.enable == ENABLED ? DIAG_STR_ENABLE: DIAG_STR_DISABLE));
                total_entry++;
            }
        }
        diag_util_mprintf("\nTotal Number Of Entries : %d\n",total_entry);

    }
#endif

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("Index |Lo-VID |Up-VID |Trust Port list       |State\n");
        diag_util_mprintf("------+-------+-------+----------------------+---------\n");

        for (index = 0; index < maxIndex; index++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, index, &entry), ret);

            if (entry.vid != 0)
            {
                osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN * sizeof(uint8));
                diag_util_lPortMask2str(port_list, &entry.portmask);

                diag_util_mprintf("%4d  |%6d |%6d | %20s | %s   \n", index, entry.vid, entry.vid_high,
                        port_list, (entry.enable == ENABLED ? DIAG_STR_ENABLE: DIAG_STR_DISABLE));
                total_entry++;
            }
        }
        diag_util_mprintf("\nTotal Number Of Entries : %d\n",total_entry);

    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("Index |Lo-VID |Up-VID |Trust Port list       |State\n");
        diag_util_mprintf("------+-------+-------+----------------------+--------\n");

        for (index = 0; index < maxIndex; index++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, index, &entry), ret);

            if (entry.vid != 0)
            {
                osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN * sizeof(uint8));
                diag_util_lPortMask2str(port_list, &entry.portmask);

                diag_util_mprintf("%4d  |%6d |%6d | %20s |%s \n", index, entry.vid,entry.vid_high,
                        port_list, (entry.enable == ENABLED ? DIAG_STR_ENABLE : DIAG_STR_DISABLE));
                total_entry++;
            }
        }
        diag_util_mprintf("\nTotal Number Of Entries : %d\n",total_entry);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_DUMP_PORT_PORTS_ALL
/*
 * port dump port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_dump_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    uint8                   max_len = 0;
    rtk_enable_t            enabled = DISABLED;
    rtk_port_phy_ability_t  ability;
    rtk_port_speed_t        speed = PORT_SPEED_10M;
    rtk_port_duplex_t       duplex = PORT_HALF_DUPLEX;
    rtk_port_linkStatus_t   link_status = PORT_LINKDOWN;
    rtk_port_media_t        media;
    diag_portlist_t               portlist;
    rtk_enable_t              autoNego;
    rtk_enable_t              txStatus, rxStatus;
    rtk_port_crossOver_mode_t   mode;
    rtk_switch_devInfo_t    devInfo;

    char media_cmd[32]= "\tMedia";
    char admin_cmd[32] = "\tAdmin";
    char macTx_cmd[32] = "\tMac Tx";
    char macRx_cmd[32] = "\tMac Rx";
    char pressure_cmd[32] = "\tBack Pressure";
    char link_cmd[32] = "\tLink";
    char autoNego_cmd[32] = "\tAutoNego";
    char autoNegoAbility_cmd[32] = "\tAutoNego Ability";
    char gigaLite_cmd[32] = "\tGiga-Lite";
    char flowCtrl_cmd[32] = "\tFlow Control (actual)";
    char forceMode_cmd[32] = "\tForce Mode Ability";
    char forceCtrlConf_cmd[32] = "\tFlow Control (config)";
    char crossOverMode_cmd[32] = "\tCross Over Mode";

    max_len = strlen(flowCtrl_cmd);

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    osal_memset(&ability, 0, sizeof(rtk_port_phy_ability_t));

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);

        diag_util_printf("%s",media_cmd);
        cmd_print_blank(max_len, strlen(media_cmd));

        DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortMedia_get(unit, port, &media), ret);
        if (PORT_MEDIA_COPPER == media)
        {
            diag_util_printf("Copper\n");
        }
        else if (PORT_MEDIA_FIBER == media)
        {
            diag_util_printf("Fiber\n");
        }

        diag_util_printf("%s",admin_cmd);
        cmd_print_blank(max_len,strlen(admin_cmd));

        DIAG_UTIL_ERR_CHK(rtk_port_adminEnable_get(unit, port, &enabled), ret);
        diag_util_printf("%s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

        diag_util_printf("%s",macTx_cmd);
        cmd_print_blank(max_len,strlen(macTx_cmd));
        DIAG_UTIL_ERR_CHK(rtk_port_txEnable_get(unit, port, &enabled), ret);
        diag_util_printf("%s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

        diag_util_printf("%s",macRx_cmd);
        cmd_print_blank(max_len,strlen(macRx_cmd));
        DIAG_UTIL_ERR_CHK(rtk_port_rxEnable_get(unit, port, &enabled), ret);
        diag_util_printf("%s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

        diag_util_printf("%s",pressure_cmd);
        cmd_print_blank(max_len,strlen(pressure_cmd));
        DIAG_UTIL_ERR_CHK(rtk_port_backpressureEnable_get(unit, port, &enabled), ret);
        diag_util_mprintf("%s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

        diag_util_printf("%s",link_cmd);
        cmd_print_blank(max_len,strlen(link_cmd));
        DIAG_UTIL_ERR_CHK(rtk_port_link_get(unit, port, &link_status), ret);
        if (PORT_LINKUP == link_status)
        {
            diag_util_printf("UP");

            diag_util_printf("\tSpeed : ");
            if ((ret = rtk_port_speedDuplex_get(unit, port, &speed, &duplex)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
            else
            {
                if (PORT_HALF_DUPLEX == duplex)
                {
                    if (PORT_SPEED_10M == speed)
                        diag_util_printf("10H ");
                    else if (PORT_SPEED_100M == speed)
                        diag_util_printf("100H ");
                    else if (PORT_SPEED_1000M == speed)
                        diag_util_printf("1000H ");
                    else
                    {
                        diag_util_printf("User config: Error!\n");
                        return CPARSER_NOT_OK;
                    }
                }
                else if (PORT_FULL_DUPLEX == duplex)
                {
                    if (PORT_SPEED_10M == speed)
                        diag_util_printf("10F ");
                    else if (PORT_SPEED_100M == speed)
                        diag_util_printf("100F ");
                    else if (PORT_SPEED_500M == speed)
                        diag_util_printf("500F ");
                    else if (PORT_SPEED_1000M == speed)
                        diag_util_printf("1000F ");
                    else if (PORT_SPEED_10G == speed)
                        diag_util_printf("10G ");
                    else if (PORT_SPEED_2_5G == speed)
                        diag_util_printf("2.5G ");
                    else if (PORT_SPEED_5G == speed)
                        diag_util_printf("5G ");
                    else
                    {
                        diag_util_printf("User config: Speed Error %d!\n", speed);
                        return CPARSER_NOT_OK;
                    }
                }
                else
                {
                    diag_util_printf("User config: Error!\n");
                    return CPARSER_NOT_OK;
                }
            }
            diag_util_printf("\n");
        }
        else
        {
            diag_util_mprintf("DOWN\n");
        }

        if (RTK_PORTMASK_IS_PORT_SET(devInfo.xge.portmask, port)
            && (PORT_MEDIA_FIBER == media))
        {
            if (PORT_LINKUP == link_status)
            {
                if (PORT_SPEED_10G == speed)
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }

        diag_util_printf("%s",autoNego_cmd);
        cmd_print_blank(max_len,strlen(autoNego_cmd));
        ret = rtk_port_phyAutoNegoEnable_get(unit, port, &autoNego);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("(%s)\n", rt_error_numToStr(ret));
        }
        else
        {
            diag_util_mprintf("%s\n", autoNego ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

            diag_util_printf("%s",autoNegoAbility_cmd);
            cmd_print_blank(max_len,strlen(autoNegoAbility_cmd));
            DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoAbility_get(unit, port, &ability), ret);
            if (ABILITY_BIT_ON == ability.Half_10)
                diag_util_printf("10H ");
            if (ABILITY_BIT_ON == ability.Full_10)
                diag_util_printf("10F ");
            if (ABILITY_BIT_ON == ability.Half_100)
                diag_util_printf("100H ");
            if (ABILITY_BIT_ON == ability.Full_100)
                diag_util_printf("100F ");
            if (ABILITY_BIT_ON == ability.Full_1000)
                diag_util_printf("1000F ");
            if (ABILITY_BIT_ON == ability.adv_2_5G)
               diag_util_printf("2.5G ");
            if (ABILITY_BIT_ON == ability.adv_5G)
               diag_util_printf("5G ");
            if (ABILITY_BIT_ON == ability.adv_10GBase_T)
               diag_util_printf("10GBase_T ");
            if (ABILITY_BIT_ON == ability.FC)
                diag_util_printf("Flow-Control ");
            if (ABILITY_BIT_ON == ability.AsyFC)
                diag_util_printf("Asy-Flow-Control ");
            diag_util_mprintf("\n");

            if(!autoNego)
            {
                diag_util_printf("%s",forceMode_cmd);
                cmd_print_blank(max_len,strlen(forceMode_cmd));
                //diag_util_mprintf("\tForce Mode Ability    : ");
                DIAG_UTIL_ERR_CHK(rtk_port_phyForceModeAbility_get(unit, port, &speed, &duplex, &enabled), ret);
                if (PORT_HALF_DUPLEX == duplex)
                {
                    if (PORT_SPEED_10M == speed)
                        diag_util_printf("10H ");
                    else if (PORT_SPEED_100M == speed)
                        diag_util_printf("100H ");
                    else if (PORT_SPEED_1000M == speed)
                        diag_util_printf("1000H ");
                    else
                    {
                        diag_util_printf("User config: Error!\n");
                        return CPARSER_NOT_OK;
                    }
                }
                else if (PORT_FULL_DUPLEX == duplex)
                {
                    if (PORT_SPEED_10M == speed)
                        diag_util_printf("10F ");
                    else if (PORT_SPEED_100M == speed)
                        diag_util_printf("100F ");
                    else if (PORT_SPEED_1000M == speed)
                        diag_util_printf("1000F ");
                    else if (PORT_SPEED_10G == speed)
                        diag_util_printf("10G");
                    else if (PORT_SPEED_2_5G == speed)
                        diag_util_printf("2.5G");
                    else if (PORT_SPEED_5G == speed)
                        diag_util_printf("5G ");
                    else
                    {
                        diag_util_printf("User config: Error!\n");
                        return CPARSER_NOT_OK;
                    }
                }
                else
                {
                    diag_util_printf("User config: Error!\n");
                    return CPARSER_NOT_OK;
                }
                diag_util_mprintf("\n");
                diag_util_printf("%s",forceCtrlConf_cmd);
                cmd_print_blank(max_len,strlen(forceCtrlConf_cmd));
                diag_util_mprintf("%s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            }
        }

        diag_util_printf("%s",gigaLite_cmd);
        cmd_print_blank(max_len,strlen(gigaLite_cmd));
        ret = rtk_port_gigaLiteEnable_get(unit, port, &enabled);
        if (ret != RT_ERR_OK)
            diag_util_mprintf("(%s)\n", rt_error_numToStr(ret));
        else
            diag_util_mprintf("%s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

        //DIAG_UTIL_ERR_CHK(rtk_port_flowctrl_get(unit, port, &txStatus, &rxStatus), ret);
        ret = rtk_port_flowctrl_get(unit, port, &txStatus, &rxStatus);
        //DIAG_ERR_PRINT(ret);
        if (ret == RT_ERR_OK)
        {
            diag_util_printf("%s",flowCtrl_cmd);
            cmd_print_blank(max_len,strlen(flowCtrl_cmd));
            diag_util_mprintf("%s\n", (txStatus && rxStatus) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }

        diag_util_printf("%s",crossOverMode_cmd);
        cmd_print_blank(max_len,strlen(crossOverMode_cmd));
        ret = rtk_port_phyCrossOverMode_get(unit, port, &mode);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("(%s)\n", rt_error_numToStr(ret));
        }
        else
        {
            if (PORT_CROSSOVER_MODE_AUTO == mode)
            {
                diag_util_mprintf("Auto MDI/MDIX\n");
            }
            else if (PORT_CROSSOVER_MODE_MDI == mode)
            {
                diag_util_mprintf("Force MDI\n");
            }
            else
            {
                diag_util_mprintf("Force MDIX\n");
            }
        }
    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_REG_PORT_PORTS_ALL
/*
 * port get phy-reg port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_get_phy_reg_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    uint32      reg_data = 0;
    uint32      reg_page = 0;
    uint32      reg_indx = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port : %d\n", port);

        for(reg_page = 0; reg_page < 32; reg_page++)
        {
            diag_util_mprintf("    Page %d : \n", reg_page);
            for(reg_indx = 0; reg_indx < 32; reg_indx++)
            {
                DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, reg_page, reg_indx, &reg_data), ret);
                if(0 == (reg_indx%4))
                {
                    diag_util_printf("0x%02X  ", reg_indx);
                }
                diag_util_printf("0x%04X  ", reg_data);
                if(3 == (reg_indx%4))
                {
                    diag_util_mprintf("\n");
                }
            }
            diag_util_mprintf("\n");
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE
/*
 * port get phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page>
 */
cparser_result_t cparser_cmd_port_get_phy_reg_port_ports_all_page_page(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *page_ptr)
{
    uint32      unit = 0;
    uint32      reg_data = 0;
    uint32      reg_indx = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((page_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    //DIAG_UTIL_PARAM_RANGE_CHK((*page_ptr > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\n");

        diag_util_mprintf("Port : %d\n", port);

        diag_util_mprintf("    Page %d : \n", *page_ptr);
        for(reg_indx = 0; reg_indx < 32; reg_indx++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, *page_ptr, reg_indx, &reg_data), ret);
            if(0 == (reg_indx%4))
            {
                diag_util_printf("0x%02X  ", reg_indx);
            }
            diag_util_printf("0x%04X  ", reg_data);
            if(3 == (reg_indx%4))
            {
                diag_util_mprintf("\n");
            }
        }

        diag_util_mprintf("\n");
    }


    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE_REGISTER_REGISTER
/*
 * port get phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register>
 */
cparser_result_t cparser_cmd_port_get_phy_reg_port_ports_all_page_page_register_register(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *page_ptr,
    uint32_t *register_ptr)
{
    uint32      unit = 0;
    uint32      reg_indx, display_no;
    uint32      reg_data = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;
    uint32 index = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((page_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((register_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*register_ptr > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);

    display_no = 1;

    DIAG_UTIL_PARAM_RANGE_CHK((display_no > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\n");

        diag_util_mprintf("Port : %d\n", port);

        diag_util_mprintf("    Page %d : \n", *page_ptr);
        index = 0;
        for(reg_indx = *register_ptr; reg_indx < ((((*register_ptr) + (display_no)) < 32) ? ((*register_ptr) + (display_no)) : 32); reg_indx++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, *page_ptr, reg_indx, &reg_data), ret);
            if(0 == (index%4))
            {
                diag_util_printf("        0x%02X  ", reg_indx);
            }
            diag_util_printf("        0x%04X  ", reg_data);
            if(3 == (index%4))
            {
                diag_util_mprintf("\n");
            }

            index++;
        }

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE_REGISTER_REGISTER_NUMBER_NUMBER
/*
 * port get phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register> number <UINT:number>
 */
cparser_result_t cparser_cmd_port_get_phy_reg_port_ports_all_page_page_register_register_number_number(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *page_ptr,
    uint32_t *register_ptr,
    uint32_t *number_ptr)
{
    uint32      unit = 0;
    uint32      reg_indx, display_no;
    uint32      reg_data = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;
    uint32 index = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((page_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((register_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*register_ptr > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);

    display_no = *number_ptr;

    DIAG_UTIL_PARAM_RANGE_CHK((display_no > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\n");

        diag_util_mprintf("Port : %d\n", port);

        diag_util_mprintf("    Page %d : \n", *page_ptr);
        index = 0;
        for(reg_indx = *register_ptr; reg_indx < ((((*register_ptr) + (display_no)) < 32) ? ((*register_ptr) + (display_no)) : 32); reg_indx++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, *page_ptr, reg_indx, &reg_data), ret);
            if(0 == (index%4))
            {
                diag_util_printf("        0x%02X  ", reg_indx);
            }
            diag_util_printf("        0x%04X  ", reg_data);
            if(3 == (index%4))
            {
                diag_util_mprintf("\n");
            }

            index++;
        }

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_EXT_REG_PORT_PORTS_ALL_MAIN_PAGE_MAINPAGE_EXT_PAGE_EXTPAGE_PARK_PAGE_PARKPAGE_REGISTER_REGISTER
/*
 * port get phy-ext-reg port ( <PORT_LIST:ports> | all ) main-page <UINT:mainPage> ext-page <UINT:extPage> park-page <UINT:parkPage> register <UINT:register>
 */
cparser_result_t cparser_cmd_port_get_phy_ext_reg_port_ports_all_main_page_mainPage_ext_page_extPage_park_page_parkPage_register_register(cparser_context_t *context,
    char **port_ptr,
    uint32_t *mainPage_ptr,
    uint32_t *extPage_ptr,
    uint32_t *parkPage_ptr,
    uint32_t *register_ptr)
{
    uint32      unit = 0;
    uint32      reg_indx, display_no;
    uint32      reg_data = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;
    uint32 index = 0;;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((mainPage_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((extPage_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((parkPage_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((register_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((*register_ptr > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);

    display_no = 1;

    RT_PARAM_CHK((display_no > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\n");

        diag_util_mprintf("Port : %d\n", port);

        diag_util_mprintf("    Main Page %d : \n", *mainPage_ptr);
        diag_util_mprintf("    Ext Page %d : \n", *extPage_ptr);
        diag_util_mprintf("    Park Page %d : \n", *parkPage_ptr);
        index = 0;
        for(reg_indx = *register_ptr; reg_indx < ((((*register_ptr) + (display_no)) < 32) ? ((*register_ptr) + (display_no)) : 32); reg_indx++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyExtParkPageReg_get(unit, port, *mainPage_ptr, *extPage_ptr, *parkPage_ptr, reg_indx, &reg_data), ret);
            if(0 == (index%4))
            {
                diag_util_printf("        0x%02X  ", reg_indx);
            }
            diag_util_printf("        0x%04X  ", reg_data);
            if(3 == (index%4))
            {
                diag_util_mprintf("\n");
            }

            index++;
        }

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_phy_ext_reg_port_ports_all_main_page_mainPage_ext_page_extPage_park_page_parkPage_register_register */
#endif

#ifdef CMD_PORT_GET_PHY_EXT_REG_PORT_PORTS_ALL_MAIN_PAGE_MAINPAGE_EXT_PAGE_EXTPAGE_PARK_PAGE_PARKPAGE_REGISTER_REGISTER_NUMBER_NUMBER
/*
 * port get phy-ext-reg port ( <PORT_LIST:ports> | all ) main-page <UINT:mainPage> ext-page <UINT:extPage> park-page <UINT:parkPage> register <UINT:register> { number <UINT:number> }
 */
cparser_result_t cparser_cmd_port_get_phy_ext_reg_port_ports_all_main_page_mainPage_ext_page_extPage_park_page_parkPage_register_register_number_number(cparser_context_t *context,
    char **port_ptr,
    uint32_t *mainPage_ptr,
    uint32_t *extPage_ptr,
    uint32_t *parkPage_ptr,
    uint32_t *register_ptr,
    uint32_t *number_ptr)
{
    uint32      unit = 0;
    uint32      reg_indx, display_no;
    uint32      reg_data = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;
    uint32 index = 0;;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((mainPage_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((extPage_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((parkPage_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((register_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((*register_ptr > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);


    display_no = *number_ptr;

    RT_PARAM_CHK((display_no > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);


    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\n");

        diag_util_mprintf("Port : %d\n", port);

        diag_util_mprintf("    Main Page %d : \n", *mainPage_ptr);
        diag_util_mprintf("    Ext Page %d : \n", *extPage_ptr);
        diag_util_mprintf("    Park Page %d : \n", *parkPage_ptr);
        index = 0;
        for(reg_indx = *register_ptr; reg_indx < ((((*register_ptr) + (display_no)) < 32) ? ((*register_ptr) + (display_no)) : 32); reg_indx++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyExtParkPageReg_get(unit, port, *mainPage_ptr, *extPage_ptr, *parkPage_ptr, reg_indx, &reg_data), ret);
            if(0 == (index%4))
            {
                diag_util_printf("        0x%02X  ", reg_indx);
            }
            diag_util_printf("        0x%04X  ", reg_data);
            if(3 == (index%4))
            {
                diag_util_mprintf("\n");
            }

            index++;
        }

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_phy_ext_reg_port_all_mainpage_extpage_parkpage_register_number */
#endif

#ifdef CMD_PORT_GET_PHY_MMD_REG_PORT_PORTS_ALL_MMD_ADDR_MMD_ADDR_MMD_REG_MMD_REG
/*
 * port get phy-mmd-reg port ( <PORT_LIST:ports> | all ) mmd-addr <UINT:mmd_addr> mmd-reg <UINT:mmd_reg>
 */
cparser_result_t
cparser_cmd_port_get_phy_mmd_reg_port_ports_all_mmd_addr_mmd_addr_mmd_reg_mmd_reg(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *mmd_addr_ptr,
    uint32_t *mmd_reg_ptr)
{
    uint32      unit = 0;
    uint32      reg_indx, display_no;
    uint32      reg_data = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;
    uint32 index = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((mmd_addr_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((mmd_reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    display_no = 1;

    RT_PARAM_CHK((display_no > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\n");

        diag_util_mprintf("Port : %d\n", port);

        diag_util_mprintf("    MMD Addr %d : \n", *mmd_addr_ptr);
        index = 0;
        for(reg_indx = *mmd_reg_ptr; reg_indx < ((((*mmd_reg_ptr) + (display_no)) < (1 << 16)) ? ((*mmd_reg_ptr) + (display_no)) : (1 << 16)); reg_indx++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyMmdReg_get(unit, port, *mmd_addr_ptr, reg_indx, &reg_data), ret);
            if(0 == (index%4))
            {
                diag_util_printf("        0x%02X  ", reg_indx);
            }
            diag_util_printf("        0x%04X  ", reg_data);
            if(3 == (index%4))
            {
                diag_util_mprintf("\n");
            }

            index++;
        }

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_phy_mmd_reg_port_ports_all_mmd_addr_mmd_addr_mmd_reg_mmd_reg */
#endif

#ifdef CMD_PORT_GET_PHY_MMD_REG_PORT_PORTS_ALL_MMD_ADDR_MMD_ADDR_MMD_REG_MMD_REG_NUMBER_NUMBER
/*
 * port get phy-mmd-reg port ( <PORT_LIST:ports> | all ) mmd-addr <UINT:mmd_addr> mmd-reg <UINT:mmd_reg> { number <UINT:number> }
 */
cparser_result_t
cparser_cmd_port_get_phy_mmd_reg_port_ports_all_mmd_addr_mmd_addr_mmd_reg_mmd_reg_number_number(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *mmd_addr_ptr,
    uint32_t *mmd_reg_ptr,
    uint32_t *number_ptr)
{
    uint32      unit = 0;
    uint32      reg_indx, display_no;
    uint32      reg_data = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;
    uint32 index = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((mmd_addr_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((mmd_reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    display_no = *number_ptr;

    RT_PARAM_CHK((display_no > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\n");

        diag_util_mprintf("Port : %d\n", port);

        diag_util_mprintf("    MMD Addr %d : \n", *mmd_addr_ptr);
        index = 0;
        for(reg_indx = *mmd_reg_ptr; reg_indx < ((((*mmd_reg_ptr) + (display_no)) < (1 << 16)) ? ((*mmd_reg_ptr) + (display_no)) : (1 << 16)); reg_indx++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyMmdReg_get(unit, port, *mmd_addr_ptr, reg_indx, &reg_data), ret);
            if(0 == (index%4))
            {
                diag_util_printf("        0x%02X  ", reg_indx);
            }
            diag_util_printf("        0x%04X  ", reg_data);
            if(3 == (index%4))
            {
                diag_util_mprintf("\n");
            }

            index++;
        }

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_phy_mmd_reg_port_all_mmd_addr_mmd_reg_number */
#endif

#ifdef CMD_PORT_GET_BACK_PRESSURE_PORT_PORTS_ALL_STATE
/*
 * port get back-pressure port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_back_pressure_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                   unit = 0;
    rtk_port_t               port = 0;
    int32                    ret = RT_ERR_FAILED;
    rtk_enable_t             enabled = DISABLED;
    diag_portlist_t          portlist;


    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();


    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
       diag_util_mprintf("Port %2d :\n", port);

       DIAG_UTIL_ERR_CHK(rtk_port_backpressureEnable_get(unit, port, &enabled), ret);
       diag_util_mprintf("\tBack Pressure : %s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;

}
#endif

#ifdef CMD_PORT_GET_AUTO_NEGO_PORT_PORTS_ALL_STATE
/*
 * port get auto-nego port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_auto_nego_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_phy_ability_t  ability;
    diag_portlist_t               portlist;
    rtk_enable_t              autoNego;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ability, 0, sizeof(rtk_port_phy_ability_t));

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
       diag_util_mprintf("Port %2d :\n", port);

       DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoEnable_get(unit, port, &autoNego), ret);
       diag_util_mprintf("\tAutoNego              : %s\n", autoNego ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_GREEN_PORT_PORTS_ALL_STATE
/*
 * port get green port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_green_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                   unit = 0;
    rtk_port_t               port = 0;
    int32                    ret = RT_ERR_FAILED;
    rtk_enable_t             enabled = DISABLED;
    diag_portlist_t          portlist;


    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
       diag_util_mprintf("Port %2d :\n", port);

       DIAG_UTIL_ERR_CHK(rtk_port_greenEnable_get(unit, port, &enabled), ret);

       diag_util_mprintf("\tGreen : %s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_GIGA_LITE_PORT_PORTS_ALL_STATE
/*
 * port get giga-lite port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_giga_lite_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    rtk_port_t      port = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enabled = DISABLED;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        DIAG_UTIL_ERR_CHK(rtk_port_gigaLiteEnable_get(unit, port, &enabled),ret);
        diag_util_mprintf("\tGiga-Lite : %s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_LINK_DOWN_POWER_SAVING_PORT_PORT_ALL_STATE
/*
 * port get link-down-power-saving port ( <PORT_LIST:port> | all ) state
 */
cparser_result_t cparser_cmd_port_get_link_down_power_saving_port_port_all_state(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    rtk_port_t      port = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enabled = DISABLED;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);

        ret = rtk_port_linkDownPowerSavingEnable_get(unit, port, &enabled);
        if((ret != RT_ERR_OK))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        else
            diag_util_mprintf("\tLink-Down Power-Saving : %s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_AUTO_NEGO_PORT_PORTS_ALL_ABILITY
/*
 * port get auto-nego port ( <PORT_LIST:ports> | all ) ability
 */
cparser_result_t cparser_cmd_port_get_auto_nego_port_ports_all_ability(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_phy_ability_t  ability;
    diag_portlist_t         portlist;


    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ability, 0, sizeof(rtk_port_phy_ability_t));

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);

        diag_util_printf("\tAutoNego Ability      : ");
        DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoAbility_get(unit, port, &ability), ret);
        if (ABILITY_BIT_ON == ability.Half_10)
           diag_util_printf("10H ");
        if (ABILITY_BIT_ON == ability.Full_10)
           diag_util_printf("10F ");
        if (ABILITY_BIT_ON == ability.Half_100)
           diag_util_printf("100H ");
        if (ABILITY_BIT_ON == ability.Full_100)
           diag_util_printf("100F ");
        if (ABILITY_BIT_ON == ability.Full_1000)
           diag_util_printf("1000F ");
        if (ABILITY_BIT_ON == ability.adv_2_5G)
           diag_util_printf("2.5G ");
        if (ABILITY_BIT_ON == ability.adv_5G)
           diag_util_printf("5G ");
        if (ABILITY_BIT_ON == ability.adv_10GBase_T)
           diag_util_printf("10GBase_T ");
        if (ABILITY_BIT_ON == ability.FC)
           diag_util_printf("Flow-Control ");
        if (ABILITY_BIT_ON == ability.AsyFC)
           diag_util_printf("Asy-Flow-Control ");
        diag_util_mprintf("\n");

    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;

}
#endif

#ifdef CMD_PORT_GET_ISOLATION_SRC_PORT_SRC_PORTS
/*
 *  port get isolation src-port <PORT_LIST:src_ports>
 */
cparser_result_t cparser_cmd_port_get_isolation_src_port_src_ports(cparser_context_t *context,
    char **src_ports_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_portmask_t      portmask;
    char                port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    diag_portlist_t  portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*src_ports_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    /* show specific port isolation info */
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
        DIAG_UTIL_ERR_CHK(rtk_port_isolation_get(unit, port, &portmask), ret);
        osal_memset(port_list, '\0', DIAG_UTIL_PORT_MASK_STRING_LEN * sizeof(uint8));
        diag_util_lPortMask2str(port_list, &portmask);
        diag_util_mprintf("\tPort %2d : Isolation Port list %s\n", port, port_list[0] == '\0' ? "is NULL":port_list);
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_ISOLATION_DEV_ID_DEV_ID_SRC_PORT_SRC_PORTS
/*
 * port get isolation dev-id <UINT:dev_id> src-port <PORT_LIST:src_ports>
 */
cparser_result_t cparser_cmd_port_get_isolation_dev_id_dev_id_src_port_src_ports(cparser_context_t *context,
    uint32_t *dev_id_ptr,
    char **src_ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    rtk_portmask_t  portmask;
    char            port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*src_ports_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
    /* show specific port isolation info */
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
        DIAG_UTIL_ERR_CHK(rtk_port_isolationExt_get(unit, *dev_id_ptr, port, &portmask), ret);
        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN * sizeof(uint8));
        diag_util_lPortMask2str(port_list, &portmask);
        diag_util_mprintf("Source Device %2d  Port %2d : Isolation Port list %s\n", *dev_id_ptr, port, port_list[0] == '\0' ? "is NULL":port_list);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_ISOLATION_VLAN_BASED_VID
/*
 * port get isolation vlan-based <UINT:vid>
 */
cparser_result_t cparser_cmd_port_get_isolation_vlan_based_vid(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          index, maxIndex;
    rtk_port_vlanIsolationEntry_t entry;
    char            port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, maxIndex, max_num_of_vlan_port_iso_entry);

    osal_memset(&entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("Index |VID  |Trust Port list       |State\n");
        diag_util_mprintf("------+-----+----------------------+---------\n");

        for (index = 0; index < maxIndex; index++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, index, &entry), ret);

            if (entry.vid == *vid_ptr)
            {
                osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN * sizeof(uint8));
                diag_util_lPortMask2str(port_list, &entry.portmask);

                diag_util_mprintf("%4d  |%4d | %20s | %s   \n", index, entry.vid,
                        port_list, (entry.enable == ENABLED ? DIAG_STR_ENABLE : DIAG_STR_DISABLE));
            }
        }
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("Index |Lo-VID |Up-VID |Trust Port list       | State\n");
        diag_util_mprintf("------+-------+-------+----------------------+--------\n");

        for (index = 0; index < maxIndex; index++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, index, &entry), ret);

            if (entry.vid == *vid_ptr)
            {
                osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN * sizeof(uint8));
                diag_util_lPortMask2str(port_list, &entry.portmask);

                diag_util_mprintf("%4d  |%6d |%6d | %20s | %s \n", index, entry.vid,entry.vid_high,
                        port_list, (entry.enable == ENABLED ? DIAG_STR_ENABLE : DIAG_STR_DISABLE));
            }
        }
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_ISOLATION_VLAN_BASED_VLAN_SOURCE
/*
 * port get isolation vlan-based vlan-source
 */
cparser_result_t cparser_cmd_port_get_isolation_vlan_based_vlan_source(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_vlanIsolationSrc_t vlanSrc;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolation_vlanSource_get(unit, &vlanSrc), ret);

    diag_util_printf("VLAN-based Port Isolation\n");
    if (VLAN_ISOLATION_SRC_INNER == vlanSrc)
    {
        diag_util_printf("VLAN ID Source : %s\n", "Inner-tag VID");
    }
    else if (VLAN_ISOLATION_SRC_OUTER == vlanSrc)
    {
        diag_util_printf("VLAN ID Source : %s\n", "Outer-tag VID");
    }
    else
    {
        diag_util_printf("VLAN ID Source : %s\n", "Forwarding VID");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_FORCE_PORT_PORTS_ALL
/*
 *  port get phy-force port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_get_phy_force_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_enable_t            enabled = DISABLED;
    rtk_port_speed_t        speed = PORT_SPEED_10M;
    rtk_port_duplex_t       duplex = PORT_HALF_DUPLEX;
    diag_portlist_t               portlist;
    rtk_port_flowctrl_mode_t    fcMode;


    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
       diag_util_printf("Port %2u :\n", port);

       diag_util_printf("\tForce Mode Ability    : ");
       DIAG_UTIL_ERR_CHK(rtk_port_phyForceModeAbility_get(unit, port, &speed, &duplex, &enabled), ret);
       if (PORT_HALF_DUPLEX == duplex)
       {
           if (PORT_SPEED_10M == speed)
               diag_util_printf("10H ");
           else if (PORT_SPEED_100M == speed)
               diag_util_printf("100H ");
           else
           {
               diag_util_printf("User config: Error!\n");
               return CPARSER_NOT_OK;
           }
       }
       else if (PORT_FULL_DUPLEX == duplex)
       {
           if (PORT_SPEED_10M == speed)
               diag_util_printf("10F ");
           else if (PORT_SPEED_100M == speed)
               diag_util_printf("100F ");
           else if (PORT_SPEED_1000M == speed)
               diag_util_printf("1000F ");
           else if (PORT_SPEED_10G == speed)
               diag_util_printf("10000F ");
           else
           {
               diag_util_printf("User config: Error!\n");
               return CPARSER_NOT_OK;
           }
       }
       else
       {
           diag_util_printf("User config: Error!\n");
           return CPARSER_NOT_OK;
       }
       diag_util_mprintf("\n");
       diag_util_mprintf("\tFlow Control (config) : %s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

        ret = rtk_port_phyForceFlowctrlMode_get(unit, port, &fcMode);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("\tTX Pause (config) : %s\n", rt_error_numToStr(ret));
            diag_util_mprintf("\tRX Pause (config) : %s\n", rt_error_numToStr(ret));
        }
        else
        {
            diag_util_mprintf("\tTX Pause (config) : %s\n", fcMode.tx_pause ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            diag_util_mprintf("\tRX Pause (config) : %s\n", fcMode.rx_pause ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }

    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;

}
#endif

#ifdef CMD_PORT_GET_RX_TX_PORT_PORTS_ALL_STATE
/*
 *  port get ( rx | tx ) port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_rx_tx_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_enable_t            enabled = DISABLED;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('e' == TOKEN_CHAR(6, 0))
        enabled = ENABLED;
    else
        enabled = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        if ('r' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_rxEnable_get(unit, port, &enabled), ret);
            diag_util_mprintf("\tPacket RX: %s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }
        else if ('t' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_txEnable_get(unit, port, &enabled), ret);
            diag_util_mprintf("\tPacket TX: %s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;

}
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_STATE
/*
 *  port get port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_enable_t            enabled = DISABLED;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
       diag_util_mprintf("Port %2d :\n", port);

       DIAG_UTIL_ERR_CHK(rtk_port_adminEnable_get(unit, port, &enabled), ret);
       diag_util_mprintf("\tAdmin : %s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;

}
#endif

#ifdef CMD_PORT_GET_COMBO_MODE_PORT_PORTS_ALL
/*
 * port get combo-mode port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_get_combo_mode_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_media_t        media;
    diag_portlist_t         portlist;


    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        diag_util_printf("\tMedia                 : ");
        DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortMedia_get(unit, port, &media), ret);
        if (PORT_MEDIA_COPPER == media)
        {
            diag_util_mprintf("Copper\n");
        }
        else if (PORT_MEDIA_FIBER == media)
        {
            diag_util_mprintf("Fiber\n");
        }

    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_CROSS_OVER_PORT_PORTS_ALL_MODE
/*
 * port get cross-over port ( <PORT_LIST:ports> | all ) mode
 */
cparser_result_t cparser_cmd_port_get_cross_over_port_ports_all_mode(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_crossOver_mode_t   mode;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        diag_util_printf("\tCross Over Mode: ");
        ret = rtk_port_phyCrossOverMode_get(unit, port, &mode);
        if((ret != RT_ERR_OK) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        else if (ret == RT_ERR_PORT_NOT_SUPPORTED)
            diag_util_mprintf("%s\n", "No supported");
        else
        {
            if (PORT_CROSSOVER_MODE_AUTO == mode)
            {
                diag_util_mprintf("Auto MDI/MDIX\n");
            }
            else if (PORT_CROSSOVER_MODE_MDI == mode)
            {
                diag_util_mprintf("Force MDI\n");
            }
            else
            {
                diag_util_mprintf("Force MDIX\n");
            }
        }
    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
} /* end of cparser_cmd_port_get_cross_over_port_all_mode */
#endif

cparser_result_t cparser_cmd_port_get_cross_over_port_ports_all_status(cparser_context_t *context,
    char **ports_ptr);

#ifdef CMD_PORT_GET_CROSS_OVER_PORT_PORTS_ALL_STATUS
/*
 * port get cross-over port ( <PORT_LIST:ports> | all ) mode
 */
cparser_result_t cparser_cmd_port_get_cross_over_port_ports_all_status(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_crossOver_status_t   status;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        diag_util_printf("\tCross Over Status: ");
        ret = rtk_port_phyCrossOverStatus_get(unit, port, &status);
#if 0
        if((ret != RT_ERR_OK) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        else if (ret == RT_ERR_PORT_NOT_SUPPORTED)
            diag_util_mprintf("%s\n", "No supported");
        else if (ret == RT_ERR_PHY_FIBER_LINKUP)
            diag_util_mprintf("%s\n", "Fiber is Link Up, Can not get cross over status");
#else
        if (ret != RT_ERR_OK)
        {
            if (ret == RT_ERR_PORT_NOT_SUPPORTED)
                diag_util_mprintf("%s\n", "No supported");
            else if (ret == RT_ERR_PHY_FIBER_LINKUP)
                diag_util_mprintf("%s\n", "Fiber is Link Up, Can not get cross over status");
            else
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
#endif
        else
        {
            if (PORT_CROSSOVER_STATUS_MDI == status)
            {
                diag_util_mprintf("MDI\n");
            }
            else if (PORT_CROSSOVER_STATUS_MDIX == status)
            {
                diag_util_mprintf("MDIX\n");
            }
            else
            {
                diag_util_mprintf("Unknown\n");
            }
        }
    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
} /* end of cparser_cmd_port_get_cross_over_port_all_status */
#endif


#ifdef CMD_PORT_SET_AUTO_NEGO_PORT_PORTS_ALL_ABILITY_10H_10F_100H_100F_1000F_FLOW_CONTROL_ASY_FLOW_CONTROL
/*
 * port set auto-nego port ( <PORT_LIST:ports> | all ) ability { 10h } { 10f } { 100h } { 100f } { 1000f } { 2_5g } { 5g } { 10gbase-t } { flow-control } { asy-flow-control }
 */
cparser_result_t cparser_cmd_port_set_auto_nego_port_ports_all_ability_10h_10f_100h_100f_1000f_2_5g_5g_10gbase_t_flow_control_asy_flow_control(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    int32                   option_num = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    rtk_port_phy_ability_t  ability;
    diag_portlist_t               portlist;
    int     len;
    char    *str;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ability, 0, sizeof(rtk_port_phy_ability_t));

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    for (option_num = 6; option_num < TOKEN_NUM; option_num++)
    {
        str = TOKEN_STR(option_num);
        len = strlen(str);
        if (!strncmp("flow-control", str, len))
        {
            ability.FC = ABILITY_BIT_ON;
        }
        else if (!strncmp("asy-flow-control", str, len))
        {
            ability.AsyFC = ABILITY_BIT_ON;
        }
        else if (!strncmp("10h", str, len))
        {
            ability.Half_10 = ABILITY_BIT_ON;
        }
        else if (!strncmp("10f", str, len))
        {
            ability.Full_10 = ABILITY_BIT_ON;
        }
        else if (!strncmp("100h", str, len))
        {
            ability.Half_100 = ABILITY_BIT_ON;
        }
        else if (!strncmp("100f", str, len))
        {
            ability.Full_100 = ABILITY_BIT_ON;
        }
        else if (!strncmp("1000f", str, len))
        {
            ability.Full_1000 = ABILITY_BIT_ON;
        }
        else if (!strncmp("2_5g", str, len))
        {
            ability.adv_2_5G = ABILITY_BIT_ON;
        }
        else if (!strncmp("5g", str, len))
        {
            ability.adv_5G = ABILITY_BIT_ON;
        }
        else if (!strncmp("10gbase-t", str, len))
        {
            ability.adv_10GBase_T = ABILITY_BIT_ON;
        }
        else
        {
            diag_util_printf("User config: Error! [%s]\n", str);
            return CPARSER_NOT_OK;
        }
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoAbility_set(unit, port, &ability), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_AUTO_NEGO_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * port set auto-nego port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_auto_nego_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    /* set port auto nego */
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('e' == TOKEN_CHAR(6, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoEnable_set(unit, port, ENABLED), ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoEnable_set(unit, port, DISABLED), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_BACK_PRESSURE_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * port set back-pressure port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_back_pressure_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('e' == TOKEN_CHAR(6, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_backpressureEnable_set(unit, port, ENABLED), ret);
        }
        else if ('d' == TOKEN_CHAR(6, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_backpressureEnable_set(unit, port, DISABLED), ret);
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_COMBO_MODE_PORT_PORTS_ALL_COPPER_FORCE_FIBER_FORCE
/*
 * port set combo-mode port ( <PORT_LIST:ports> | all ) ( copper-force | fiber-force )
 */
cparser_result_t cparser_cmd_port_set_combo_mode_port_ports_all_copper_force_fiber_force(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    rtk_port_media_t media = PORT_MEDIA_COPPER;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('c' == TOKEN_CHAR(5, 0))
    {
        media = PORT_MEDIA_COPPER;
    }
    else if ('f' == TOKEN_CHAR(5, 0))
    {
        media = PORT_MEDIA_FIBER;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        /* set port media type */
        DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortMedia_set(unit, port, media), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_FORCE_PORT_PORTS_ALL_ABILITY_10H_10F_100H_100F_1000F
/*
 * port set phy-force port ( <PORT_LIST:ports> | all ) ability ( 10h | 10f | 100h | 100f | 1000f | 2_5g | 10000f )
 */
cparser_result_t cparser_cmd_port_set_phy_force_port_ports_all_ability_10h_10f_100h_100f_1000f_2_5g_10000f(cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_speed_t    speed = PORT_SPEED_10M, oldSpeed;
    rtk_port_duplex_t   duplex = PORT_HALF_DUPLEX, oldDuplex;
    rtk_enable_t        oldFlowControl = DISABLED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('2' == TOKEN_CHAR(6, 0))
    {
        speed = PORT_SPEED_2_5G;
        duplex = PORT_FULL_DUPLEX;
    }
    else if ('h' == TOKEN_CHAR(6, 2))
    {
        speed = PORT_SPEED_10M;
        duplex = PORT_HALF_DUPLEX;
    }
    else if ('f' == TOKEN_CHAR(6, 2))
    {
        speed = PORT_SPEED_10M;
        duplex = PORT_FULL_DUPLEX;
    }
    else if ('h' == TOKEN_CHAR(6, 3))
    {
        speed = PORT_SPEED_100M;
        duplex = PORT_HALF_DUPLEX;
    }
    else if ('f' == TOKEN_CHAR(6, 3))
    {
        speed = PORT_SPEED_100M;
        duplex = PORT_FULL_DUPLEX;
    }
    else if ('f' == TOKEN_CHAR(6, 4))
    {
        speed = PORT_SPEED_1000M;
        duplex = PORT_FULL_DUPLEX;
    }
    else if ('f' == TOKEN_CHAR(6, 5))
    {
        speed = PORT_SPEED_10G;
        duplex = PORT_FULL_DUPLEX;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyForceModeAbility_get(unit, port, &oldSpeed, &oldDuplex, &oldFlowControl), ret);
        DIAG_UTIL_ERR_CHK(rtk_port_phyForceModeAbility_set(unit, port, speed, duplex, oldFlowControl), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_FORCE_PORT_PORTS_ALL_FLOW_CONTROL_STATE_DISABLE_ENABLE
/*
 * port set phy-force port ( <PORT_LIST:ports> | all ) flow-control state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_phy_force_port_ports_all_flow_control_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_speed_t    oldSpeed;
    rtk_port_duplex_t   oldDuplex;
    rtk_enable_t        oldFlowControl, flowControl = DISABLED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('e' == TOKEN_CHAR(7, 0))
    {
        flowControl = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(7, 0))
    {
        flowControl = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyForceModeAbility_get(unit, port, &oldSpeed, &oldDuplex, &oldFlowControl), ret);
        DIAG_UTIL_ERR_CHK(rtk_port_phyForceModeAbility_set(unit, port, oldSpeed, oldDuplex, flowControl), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_GREEN_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * port set green port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_green_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('e' == TOKEN_CHAR(6, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_greenEnable_set(unit, port, ENABLED), ret);
        }
        else if ('d' == TOKEN_CHAR(6, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_greenEnable_set(unit, port, DISABLED), ret);
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_GIGA_LITE_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * port set giga-lite port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_giga_lite_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('e' == TOKEN_CHAR(6, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_gigaLiteEnable_set(unit, port, ENABLED), ret);
        }
        else if ('d' == TOKEN_CHAR(6, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_gigaLiteEnable_set(unit, port, DISABLED), ret);
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_LINK_DOWN_POWER_SAVING_PORT_PORT_ALL_STATE_DISABLE_ENABLE
/*
 * port set link-down-power-saving port ( <PORT_LIST:port> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_link_down_power_saving_port_port_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('e' == TOKEN_CHAR(6, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_linkDownPowerSavingEnable_set(unit, port, ENABLED), ret);
        }
        else if ('d' == TOKEN_CHAR(6, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_linkDownPowerSavingEnable_set(unit, port, DISABLED), ret);
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_GET_2PT5G_LITE_PORT_PORTS_ALL_STATE
/*
 * port get 2pt5g-lite port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_2pt5g_lite_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    rtk_port_t      port = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enabled = DISABLED;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_2pt5gLiteEnable_get(unit, port, &enabled);
        if ((ret == RT_ERR_CHIP_NOT_SUPPORTED) || (ret == RT_ERR_PORT_NOT_SUPPORTED))
            continue;
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\t2.5G-Lite : %s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }

    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_2PT5G_LITE_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * port set 2pt5g-lite port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_2pt5g_lite_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_enable_t    enable;
    char            *portStr = TOKEN_STR(4);

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('e' == TOKEN_CHAR(6, 0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(6, 0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_2pt5gLiteEnable_set(unit, port, enable);
        if ('a' == portStr[0])
        {
            if ((ret == RT_ERR_CHIP_NOT_SUPPORTED) || (ret == RT_ERR_PORT_NOT_SUPPORTED))
                continue;
        }

        DIAG_ERR_PRINT(ret);
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_SET_ISOLATION_SRC_PORT_SRC_PORTS_DST_PORT_DST_PORTS_ALL
/*
 * port set isolation src-port <PORT_LIST:src_ports> dst-port ( <PORT_LIST:dst_ports> | all )
 */
cparser_result_t cparser_cmd_port_set_isolation_src_port_src_ports_dst_port_dst_ports_all(cparser_context_t *context,
    char **src_ports_ptr,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t    portlist;
    diag_portlist_t    targetPortlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*src_ports_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(targetPortlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolation_set(unit, port, &targetPortlist.portmask), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_ADD_ISOLATION_SRC_PORT_SRC_PORT_DST_PORT_DST_PORT
/*
 * port add isolation src-port <UINT:src_port> dst-port <UINT:dst_port>
 */
cparser_result_t
cparser_cmd_port_add_isolation_src_port_src_port_dst_port_dst_port(
    cparser_context_t *context,
    uint32_t *src_port_ptr,
    uint32_t *dst_port_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_port_isolation_add(unit, *src_port_ptr, *dst_port_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_port_add_isolation_src_port_src_port_dst_port_dst_port */
#endif

#ifdef CMD_PORT_DEL_ISOLATION_SRC_PORT_SRC_PORT_DST_PORT_DST_PORT
/*
 * port del isolation src-port <UINT:src_port> dst-port <UINT:dst_port> */
cparser_result_t
cparser_cmd_port_del_isolation_src_port_src_port_dst_port_dst_port(
    cparser_context_t *context,
    uint32_t *src_port_ptr,
    uint32_t *dst_port_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_port_isolation_del(unit, *src_port_ptr, *dst_port_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_port_del_isolation_src_port_src_port_dst_port_dst_port */
#endif



#ifdef CMD_PORT_SET_ISOLATION_DEV_ID_DEV_ID_SRC_PORT_SRC_PORTS_DST_PORT_DST_PORTS_ALL
/*
 * port set isolation dev-id <UINT:dev_id> src-port <PORT_LIST:src_ports> dst-port ( <PORT_LIST:dst_ports> | all )
 */
cparser_result_t cparser_cmd_port_set_isolation_dev_id_dev_id_src_port_src_ports_dst_port_dst_ports_all(cparser_context_t *context,
    uint32_t *dev_id_ptr,
    char **src_ports_ptr,
    char **dst_ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t    portlist;
    diag_portlist_t    targetPortlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*src_ports_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(targetPortlist, 8), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationExt_set(unit, *dev_id_ptr, port, &targetPortlist.portmask), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_ISOLATION_PORT_BASED_RESTRICT_ROUTE_STATE_DISABLE_ENABLE
/*
 * port set isolation port-based restrict-route state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_isolation_port_based_restrict_route_state_disable_enable(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_port_isolationRestrictRoute_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_ISOLATION_VLAN_BASED_VID_TRUST_PORT_PORTS_NONE
/*
 * port set isolation vlan-based <UINT:vid> trust-port ( <PORT_LIST:ports> | none )
 */
cparser_result_t cparser_cmd_port_set_isolation_vlan_based_vid_trust_port_ports_none(cparser_context_t *context,
    uint32_t *vid_ptr,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          index, maxIndex, doDel = FALSE, found = FALSE;
    rtk_port_vlanIsolationEntry_t entry, setEntry;
    rtk_port_t      port = 0;
    diag_portlist_t    portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_OM_GET_CHIP_CAPACITY(unit, maxIndex, max_num_of_vlan_port_iso_entry);

    osal_memset(&entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));
    osal_memset(&setEntry, 0, sizeof(rtk_port_vlanIsolationEntry_t));

    if ('n' == TOKEN_CHAR(6, 0))
    {
        setEntry.vid = *vid_ptr;
        #if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        setEntry.vid_high = *vid_ptr;
        #endif
        doDel = TRUE;
    }
    else
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            RTK_PORTMASK_PORT_SET(setEntry.portmask, port);
        }


        setEntry.enable = ENABLED;
        setEntry.vid = *vid_ptr;
        #if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        setEntry.vid_high = *vid_ptr;
        #endif
    }

    /* search exist entry */
    for (index = 0; index < maxIndex; index++)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, index, &entry), ret);

        if (entry.vid == *vid_ptr)
        {
            if(TRUE == doDel)
            {
                setEntry.vid = 0; /*set vid = 0 if action is to delete the entry*/
                #if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
                setEntry.vid_high = 0;
                #endif
            }
            DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_set(unit, index, &setEntry), ret);
            found = TRUE;
        }
    }
    if(FALSE == found && TRUE == doDel)
        return RT_ERR_PORT_VLAN_ISO_VID_NOT_FOUND;

    /* search empty entry */
    if (FALSE == found && FALSE == doDel)
    {
        for (index = 0; index < maxIndex; index++)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, index, &entry), ret);

            if (entry.vid == 0)
            {
                DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_set(unit, index, &setEntry), ret);
                found = TRUE;
                break;
            }
        }
        if(FALSE == found)
            return RT_ERR_PORT_VLAN_ISO_NO_EMPTY_ENTRY;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_ISOLATION_VLAN_BASED_ENTRY_INDEX_TRUST_PORT_PORTS_NONE
/*
 * port set isolation vlan-based entry <UINT:index> trust-port ( <PORT_LIST:ports> | none )
 */
cparser_result_t cparser_cmd_port_set_isolation_vlan_based_entry_index_trust_port_ports_none(cparser_context_t *context,
    uint32_t *index_ptr,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    diag_portlist_t   portlist;
    rtk_port_vlanIsolationEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    osal_memset(&entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));

    DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, *index_ptr, &entry), ret);

    if ('n' == TOKEN_CHAR(7, 0))
    {
        RTK_PORTMASK_RESET(entry.portmask);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7), ret);
        osal_memcpy(&entry.portmask, &portlist.portmask, sizeof(rtk_portmask_t));
    }

    DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_ISOLATION_VLAN_BASED_ENTRY_INDEX_VID_LOWER_UPPER_STATE_DISABLE_ENABLE
/*
 * port set isolation vlan-based entry <UINT:index> vid <UINT:lower> <UINT:upper> state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_isolation_vlan_based_entry_index_vid_lower_upper_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *low_vid_ptr,
    uint32_t *up_vid_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_vlanIsolationEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    osal_memset(&entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));

    DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, *index_ptr, &entry), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        entry.enable = ENABLED;
    }
    else
    {
        entry.enable = DISABLED;
    }

    entry.vid = *low_vid_ptr;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    entry.vid_high = *up_vid_ptr;
    if (entry.vid > entry.vid_high)
    {
        diag_util_printf("Input error: the value of upper should be bigger than htat of lower\n");
        return CPARSER_NOT_OK;
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_ISOLATION_VLAN_BASED_VID_STATE_DISABLE_ENABLE
/*
 * port set isolation vlan-based <UINT:vid> state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_isolation_vlan_based_vid_state_disable_enable(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          index, maxIndex;
    rtk_port_vlanIsolationEntry_t entry;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_OM_GET_CHIP_CAPACITY(unit, maxIndex, max_num_of_vlan_port_iso_entry);

    osal_memset(&entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));
    if ('e' == TOKEN_CHAR(6, 0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(6, 0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    for (index = 0; index < maxIndex; index++)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, index, &entry), ret);

        if (entry.vid == *vid_ptr)
        {
            entry.enable = enable;
            DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_set(unit, index, &entry), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_ISOLATION_VLAN_BASED_VLAN_SOURCE_INNER_OUTER_FORWARDING
/*
 * port set isolation vlan-based vlan-source ( inner | outer | forwarding )
 */
cparser_result_t cparser_cmd_port_set_isolation_vlan_based_vlan_source_inner_outer_forwarding(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_vlanIsolationSrc_t vlanSrc;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        vlanSrc = VLAN_ISOLATION_SRC_INNER;
    }
    else if ('o' == TOKEN_CHAR(5, 0))
    {
        vlanSrc = VLAN_ISOLATION_SRC_OUTER;
    }
    else if ('f' == TOKEN_CHAR(5, 0))
    {
        vlanSrc = VLAN_ISOLATION_SRC_FORWARD;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolation_vlanSource_set(unit, vlanSrc), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_ISOLATION_PORT_BASED_RESTRICT_ROUTE_STATE
/*
 * port get isolation port-based restrict-route state
 */
cparser_result_t cparser_cmd_port_get_isolation_port_based_restrict_route_state(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_port_isolationRestrictRoute_get(unit, &enable), ret);
    diag_util_mprintf("Restrict Route:%s\n",(ENABLED == enable)? DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_ISOLATION_VLAN_BASED_EGRESS_PORT_PORTS_ALL_BYPASS_STATE
/*
 * port get isolation vlan-based egress port ( <PORT_LIST:ports> | all ) bypass state
 */
cparser_result_t cparser_cmd_port_get_isolation_vlan_based_egress_port_ports_all_bypass_state(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_enable_t        enable;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    diag_util_mprintf("Port | Bypass State\n");
    diag_util_mprintf("-----+---------------\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEgrBypass_get(unit, port, &enable), ret);
        diag_util_printf("%4u  %12s\n", port, (ENABLED == enable)? DIAG_STR_ENABLE:DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_ISOLATION_VLAN_BASED_EGRESS_PORT_PORTS_ALL_BYPASS_STATE_DISABLE_ENABLE
/*
 * port set isolation vlan-based egress port ( <PORT_LIST:ports> | all ) bypass state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_isolation_vlan_based_egress_port_ports_all_bypass_state_disable_enable(cparser_context_t *context,
    char **ports_str)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_port_t       port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 6) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(9, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_port_vlanBasedIsolationEgrBypass_set(unit, port, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_ISOLATION_VLAN_BASED_ENTRY_INDEX
/*
 * port get isolation vlan-based entry <UINT:index>
 */
cparser_result_t cparser_cmd_port_get_isolation_vlan_based_entry_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_vlanIsolationEntry_t entry;
    char    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&entry, 0, sizeof(rtk_port_vlanIsolationEntry_t));
    diag_util_mprintf("VLAN-Based port isolation configuration\n");

    DIAG_UTIL_ERR_CHK(rtk_port_vlanBasedIsolationEntry_get(unit, *index_ptr, &entry), ret);

    osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN * sizeof(uint8));

    diag_util_lPortMask2str(port_list, &entry.portmask);

    diag_util_mprintf("Entry index          : %u\n", *index_ptr);
    diag_util_mprintf("State                : %s\n", entry.enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    diag_util_mprintf("Trust Port           : %s\n", port_list);
    diag_util_mprintf("Low-bound VID        : %u\n", entry.vid);
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID)
         || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID))
        diag_util_mprintf("Up-bound VID         : %u\n", entry.vid_high);

#endif
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE_REGISTER_REGISTER_DATA_DATA
/*
 * port set phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register> data <UINT:data>
 */
cparser_result_t cparser_cmd_port_set_phy_reg_port_ports_all_page_page_register_register_data_data(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *page_ptr,
    uint32_t *register_ptr,
    uint32_t *data_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((page_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    //DIAG_UTIL_PARAM_RANGE_CHK((*page_ptr > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((register_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*register_ptr > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((data_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        /* set port phy register */
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, *page_ptr, *register_ptr, *data_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_EXT_REG_PORT_PORTS_ALL_MAIN_PAGE_MAINPAGE_EXT_PAGE_EXTPAGE_PARK_PAGE_PARKPAGE_REGISTER_REGISTER_DATA_DATA
/*
 * port set phy-ext-reg port ( <PORT_LIST:ports> | all ) main-page <UINT:mainPage> ext-page <UINT:extPage> park-page <UINT:parkPage> register <UINT:register> data <UINT:data>
 */
cparser_result_t cparser_cmd_port_set_phy_ext_reg_port_ports_all_main_page_mainPage_ext_page_extPage_park_page_parkPage_register_register_data_data(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *mainPage_ptr,
    uint32_t *extPage_ptr,
    uint32_t *parkPage_ptr,
    uint32_t *register_ptr,
    uint32_t *data_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((mainPage_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((extPage_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((parkPage_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((register_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((*register_ptr > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((data_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    /* set port phy register */
    DIAG_UTIL_ERR_CHK(rtk_port_phymaskExtParkPageReg_set(unit, &portlist.portmask,
        *mainPage_ptr, *extPage_ptr, *parkPage_ptr, *register_ptr, *data_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_phy_ext_reg_port_all_mainpage_extpage_parkpage_register_data */
#endif

#ifdef CMD_PORT_SET_PHY_MMD_REG_PORT_PORTS_ALL_MMD_ADDR_MMD_ADDR_MMD_REG_MMD_REG_DATA_DATA
/*
 * port set phy-mmd-reg port ( <PORT_LIST:ports> | all ) mmd-addr <UINT:mmd_addr> mmd-reg <UINT:mmd_reg> data <UINT:data>
 */
cparser_result_t
cparser_cmd_port_set_phy_mmd_reg_port_ports_all_mmd_addr_mmd_addr_mmd_reg_mmd_reg_data_data(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *mmd_addr_ptr,
    uint32_t *mmd_reg_ptr,
    uint32_t *data_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((mmd_addr_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((mmd_reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((data_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    /* set port phy register */
    DIAG_UTIL_ERR_CHK(rtk_port_phymaskMmdReg_set(unit, &portlist.portmask,
        *mmd_addr_ptr, *mmd_reg_ptr, *data_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_phy_mmd_reg_port_all_mmd_addr_mmd_reg_data */
#endif

#ifdef CMD_PORT_SET_RX_TX_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * port set ( rx | tx ) port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_rx_tx_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_enable_t enable;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('r' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_rxEnable_set(unit, port, enable), ret);
        }
        else if ('t' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_txEnable_set(unit, port, enable), ret);
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * port set port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */

cparser_result_t cparser_cmd_port_set_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('e' == TOKEN_CHAR(5, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_adminEnable_set(unit, port, ENABLED), ret);
        }
        else if ('d' == TOKEN_CHAR(5, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_adminEnable_set(unit, port, DISABLED), ret);
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_CROSS_OVER_PORT_PORTS_ALL_MODE_AUTO_MDI_MDIX
/*
 * port set cross-over port ( <PORT_LIST:ports> | all ) mode ( auto | mdi | mdix )
 */
cparser_result_t cparser_cmd_port_set_cross_over_port_ports_all_mode_auto_mdi_mdix(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if('a' == TOKEN_CHAR(6, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyCrossOverMode_set(unit, port, PORT_CROSSOVER_MODE_AUTO), ret);
        }
        else if ('x' == TOKEN_CHAR(6, 3))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyCrossOverMode_set(unit, port, PORT_CROSSOVER_MODE_MDIX), ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyCrossOverMode_set(unit, port, PORT_CROSSOVER_MODE_MDI), ret);
        }
    }

    return CPARSER_OK;
} /* end of cparser_cmd_port_set_cross_over_port_all_mode_auto_mdi_mdix */
#endif

#ifdef CMD_PORT_GET_COMBO_FIBER_MODE_PORT_PORTS_ALL
/*
 * port get combo-fiber-mode port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_get_combo_fiber_mode_port_ports_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_fiber_media_t  media;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        diag_util_printf("\tFiber Media           : ");
        DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortFiberMedia_get(unit, port, &media), ret);
        if (PORT_FIBER_MEDIA_1000 == media)
        {
            diag_util_mprintf("Fiber-1000Base-X\n");
        }
        else if (PORT_FIBER_MEDIA_100 == media)
        {
            diag_util_mprintf("Fiber-100Base-FX\n");
        }
        else if (PORT_FIBER_MEDIA_AUTO == media)
        {
            diag_util_mprintf("Fiber-Auto\n");
        }
    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
} /* end of cparser_cmd_port_get_combo_fiber_mode_port_ports_all */
#endif

#ifdef CMD_PORT_SET_COMBO_FIBER_MODE_PORT_PORTS_ALL_FIBER_1000_FIBER_100_FIBER_AUTO
/*
 * port set combo-fiber-mode port ( <PORT_LIST:ports> | all ) ( fiber-1000 | fiber-100 | fiber-auto )
 */
cparser_result_t cparser_cmd_port_set_combo_fiber_mode_port_ports_all_fiber_1000_fiber_100_fiber_auto(cparser_context_t *context,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    rtk_port_fiber_media_t media = PORT_FIBER_MEDIA_AUTO;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('a' == TOKEN_CHAR(5, 6))
    {
        media = PORT_FIBER_MEDIA_AUTO;
    }
    else if ('0' == TOKEN_CHAR(5, 9))
    {
        media = PORT_FIBER_MEDIA_1000;
    }
    else
    {
        media = PORT_FIBER_MEDIA_100;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        /* set port fiber media type */
        DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortFiberMedia_set(unit, port, media), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_port_set_combo_fiber_mode_port_ports_all_fiber_1000_fiber_100_fiber_auto */
#endif

#ifdef CMD_PORT_GET_MASTER_SLAVE_PORT_PORTS_ALL
/*
 * port get master-slave port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_get_master_slave_port_ports_all(cparser_context_t *context,    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_masterSlave_t  masterSlaveCfg = PORT_AUTO_MODE, masterSlaveActual;
    diag_portlist_t           portlist;


    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);


        DIAG_UTIL_ERR_CHK(rtk_port_phyMasterSlave_get(unit, port, &masterSlaveCfg, &masterSlaveActual), ret);
        diag_util_printf("\tMaster/Slave(Config) : ");

        switch(masterSlaveCfg)
        {
            case PORT_AUTO_MODE:
                diag_util_mprintf("Auto\n");
                break;
            case PORT_SLAVE_MODE:
                diag_util_mprintf("Slave\n");
                break;
            case PORT_MASTER_MODE:
                diag_util_mprintf("Master\n");
                break;
            default:
                break;
        }

        diag_util_printf("\tMaster/Slave(Actual) : ");

        switch(masterSlaveActual)
        {
            case PORT_AUTO_MODE:
                diag_util_mprintf("Auto\n");
                break;
            case PORT_SLAVE_MODE:
                diag_util_mprintf("Slave\n");
                break;
            case PORT_MASTER_MODE:
                diag_util_mprintf("Master\n");
                break;
            default:
                break;
        }
    }/*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}/* end of cparser_cmd_port_get_master_slave_port_all */
#endif

#ifdef CMD_PORT_SET_MASTER_SLAVE_PORT_PORTS_ALL_AUTO_MASTER_SLAVE
/*
 * port set master-slave port ( <PORT_LIST:ports> | all ) ( auto | master | slave )
 */
cparser_result_t cparser_cmd_port_set_master_slave_port_ports_all_auto_master_slave(cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_masterSlave_t  masterSlave = PORT_AUTO_MODE;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('a' == TOKEN_CHAR(5, 0))
    {
        masterSlave = PORT_AUTO_MODE;
    }
    else if('m' == TOKEN_CHAR(5, 0))
    {
        masterSlave = PORT_MASTER_MODE;
    }
    else if('s' == TOKEN_CHAR(5, 0))
    {
        masterSlave = PORT_SLAVE_MODE;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyMasterSlave_set(unit, port, masterSlave), ret);
    }

    return CPARSER_NOT_OK;
}/* end of cparser_cmd_port_set_master_slave_port_all_auto_master_slave */
#endif

#ifdef CMD_PORT_SET_PORT_PORTS_ALL_FIBER_LOOPBACK_ENABLE_DISABLE
/*
 * port set port ( <PORT_LIST:ports> | all ) fiber loopback (enable | disable)
 */
cparser_result_t
cparser_cmd_port_set_port_ports_all_fiber_loopback_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    uint32          unit, regData;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortMedia_set(unit, port, PORT_MEDIA_FIBER), ret);

        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, 0, 0, &regData), ret);

        if('e' == TOKEN_CHAR(6, 0))
            regData |= (1 << 14);
        else
            regData &= ~(1 << 14);
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, 0, 0, regData), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_port_ports_all_fiber_loopback_enable_disable */
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_FIBER_LOOPBACK
/*
 * port get port ( <PORT_LIST:ports> | all ) fiber loopback
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_fiber_loopback(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    uint32          unit, regData;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortMedia_set(unit, port, PORT_MEDIA_FIBER), ret);

        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, 0, 0, &regData), ret);

        diag_util_mprintf("Port %d: ", port);
        if ((regData >> 14) & 0x1)
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_fiber_loopback */
#endif

#ifdef CMD_PORT_SET_PORT_PORTS_ALL_DOWN_SPEED_ENABLE_DISABLE
/*
 * port set port ( <PORT_LIST:ports> | all ) down-speed (enable | disable)
 */
cparser_result_t cparser_cmd_port_set_port_ports_all_down_speed_enable_disable(cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(5, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_downSpeedEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_port_ports_all_down_speed_enable_disable */
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_DOWN_SPEED
/*
 * port get port ( <PORT_LIST:ports> | all ) down-speed
 */
cparser_result_t cparser_cmd_port_get_port_ports_all_down_speed(cparser_context_t *context,
    char **ports_ptr)

{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %d: ", port);
        DIAG_UTIL_ERR_CHK(rtk_port_downSpeedEnable_get(unit, port, &enable), ret);

        if (ENABLED == enable)
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_down_speed */
#endif


#ifdef CMD_PORT_SET_PORT_PORTS_ALL_FIBER_DOWN_SPEED_ENABLE_DISABLE
/*
 * port set port ( <PORT_LIST:ports> | all ) fiber down-speed (enable | disable)
 */
cparser_result_t
cparser_cmd_port_set_port_ports_all_fiber_down_speed_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;
    rtk_port_media_t    media;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        rtk_port_phyComboPortMedia_get(unit, port, &media);
        if (media != PORT_MEDIA_FIBER)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortMedia_set(unit, port, PORT_MEDIA_FIBER), ret);
        }
        DIAG_UTIL_ERR_CHK(rtk_port_fiberDownSpeedEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_port_ports_all_fiber_down_speed_enable_disable */
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_FIBER_DOWN_SPEED
/*
 * port get port ( <PORT_LIST:ports> | all ) fiber down-speed
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_fiber_down_speed(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;
    rtk_port_media_t    media;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        rtk_port_phyComboPortMedia_get(unit, port, &media);
        if (media != PORT_MEDIA_FIBER)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortMedia_set(unit, port, PORT_MEDIA_FIBER), ret);
        }

        diag_util_mprintf("Port %d: ", port);
        DIAG_UTIL_ERR_CHK(rtk_port_fiberDownSpeedEnable_get(unit, port, &enable), ret);

        if (ENABLED == enable)
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_fiber_down_speed */
#endif

#ifdef CMD_PORT_SET_PORT_PORTS_ALL_FIBER_NWAY_FORCE_LINK_ENABLE_DISABLE
/*
 * port set port ( <PORT_LIST:ports> | all ) fiber nway-force-link (enable | disable)
 */
cparser_result_t
cparser_cmd_port_set_port_ports_all_fiber_nway_force_link_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;
    rtk_port_media_t    media;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        rtk_port_phyComboPortMedia_get(unit, port, &media);
        if (media != PORT_MEDIA_FIBER)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortMedia_set(unit, port, PORT_MEDIA_FIBER), ret);
        }
        DIAG_UTIL_ERR_CHK(rtk_port_fiberNwayForceLinkEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_port_ports_all_fiber_nway_force_link_enable_disable */
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_FIBER_NWAY_FORCE_LINK
/*
 * port get port ( <PORT_LIST:ports> | all ) fiber nway-force-link
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_fiber_nway_force_link(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;
    rtk_port_media_t    media;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        rtk_port_phyComboPortMedia_get(unit, port, &media);
        if (media != PORT_MEDIA_FIBER)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyComboPortMedia_set(unit, port, PORT_MEDIA_FIBER), ret);
        }
        diag_util_mprintf("Port %d: ", port);
        DIAG_UTIL_ERR_CHK(rtk_port_fiberNwayForceLinkEnable_get(unit, port, &enable), ret);

        if (ENABLED == enable)
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_fiber_nway_force_link */
#endif

#ifdef CMD_PORT_SET_PORT_PORTS_ALL_FIBER_UNIDIR_ENABLE_DISABLE
/*
 * port set port ( <PORT_LIST:ports> | all ) fiber unidir ( enable | disable )
 */
cparser_result_t
cparser_cmd_port_set_port_ports_all_fiber_unidir_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_fiberUnidirEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_port_ports_all_fiber_unidir_enable_disable */
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_FIBER_UNIDIR_STATE
/*
 * port get port ( <PORT_LIST:ports> | all ) fiber unidir state
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_fiber_unidir_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_fiberUnidirEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("Port %d: ", port);

        if (ENABLED == enable)
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_fiber_unidir_state */
#endif

#ifdef CMD_PORT_SET_PORT_PORTS_ALL_FIBER_OAM_LOOPBACK_ENABLE_DISABLE
/*
 * port set port ( <PORT_LIST:ports> | all ) fiber oam-loopback (enable | disable)
 */
cparser_result_t
cparser_cmd_port_set_port_ports_all_fiber_oam_loopback_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_fiberOAMLoopBackEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_port_ports_all_fiber_oam_loopback_enable_disable */
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_PHY_LOOPBACK
/*
 * port get port ( <PORT_LIST:ports> | all ) phy loopback
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_phy_loopback(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyLoopBackEnable_get(unit, port, &enable), ret);

        diag_util_mprintf("Port %d : \n", port);
        diag_util_mprintf("Loopback : %s\n", (ENABLED ==enable) ? "Enable" : "Disable");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_phy_loopback */
#endif

#ifdef CMD_PORT_SET_PORT_PORTS_ALL_PHY_LOOPBACK_ENABLE_DISABLE
/*
 *  port set port ( <PORT_LIST:ports> | all ) phy loopback ( enable | disable )
 */
cparser_result_t cparser_cmd_port_set_port_ports_all_phy_loopback_enable_disable(cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_port_phyLoopBackEnable_set(unit, port, enable), ret) != RT_ERR_OK)
        {
            diag_util_mprintf("unit %d port %d: ", unit, port);
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_10G_MEDIA_PORT_PORTS_ALL_FIBER10G_FIBER1G_DAC50CM_DAC100CM_DAC300CM_NONE
/*
 * port set 10g-media port ( <PORT_LIST:ports> | all ) ( fiber10g | fiber1g | dac50cm | dac100cm | dac300cm | dac500cm  | none )
 */
cparser_result_t
cparser_cmd_port_set_10g_media_port_ports_all_fiber10g_fiber1g_dac50cm_dac100cm_dac300cm_dac500cm_none(cparser_context_t *context,
        char **ports_ptr)
{
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    rtk_port_10gMedia_t media;
    uint32              unit;
    int32               ret;
    rtk_switch_devInfo_t    devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('f' == TOKEN_CHAR(5, 0))
    {
        if ('g' == TOKEN_CHAR(5, 6))
            media = PORT_10GMEDIA_FIBER_1G;
        else
            media = PORT_10GMEDIA_FIBER_10G;
    }
    else if ('n' == TOKEN_CHAR(5, 0))
    {
        media = PORT_10GMEDIA_NONE;
    }
    else
    {
        if (0 == osal_strcmp(TOKEN_STR(5), "dac500cm"))
            media = PORT_10GMEDIA_DAC_500CM;
        else if ('5' == TOKEN_CHAR(5, 3))
            media = PORT_10GMEDIA_DAC_50CM;
        else if ('1' == TOKEN_CHAR(5, 3))
            media = PORT_10GMEDIA_DAC_100CM;
        else
            media = PORT_10GMEDIA_DAC_300CM;
    }

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
      if('a' == TOKEN_CHAR(4, 0))
      {
        if (!RTK_PORTMASK_IS_PORT_SET(devInfo.xge.portmask, port))
            continue;
      }

        DIAG_UTIL_ERR_CHK(rtk_port_10gMedia_set(unit, port, media), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_10g_media_port_ports_all_fiber10g_fiber1g_fiber100m_dac50cm_dac100cm_dac300cm_none */
#endif

#ifdef CMD_PORT_GET_10G_MEDIA_PORT_PORTS_ALL
/*
 * port get 10g-media port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_10g_media_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    rtk_port_10gMedia_t media;
    uint32              unit;
    int32               ret;
    uint32              is_all = 0;
    rtk_switch_devInfo_t    devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('a' == TOKEN_CHAR(4, 0))
    {
        is_all = 1;
        osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
        DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(is_all)
        {
            if (!RTK_PORTMASK_IS_PORT_SET(devInfo.xge.portmask, port))
                continue;
        }

        DIAG_UTIL_ERR_CHK(rtk_port_10gMedia_get(unit, port, &media), ret);
        diag_util_mprintf("Port %d: ", port);

        switch (media)
        {
            case PORT_10GMEDIA_NONE:
                diag_util_mprintf("None\n");
                break;
            case PORT_10GMEDIA_FIBER_10G:
                diag_util_mprintf("fiber 10G\n");
                break;
            case PORT_10GMEDIA_FIBER_1G:
                diag_util_mprintf("fiber 1G\n");
                break;
            case PORT_10GMEDIA_DAC_50CM:
                diag_util_mprintf("DAC 50cm\n");
                break;
            case PORT_10GMEDIA_DAC_100CM:
                diag_util_mprintf("DAC 100cm\n");
                break;
            case PORT_10GMEDIA_DAC_300CM:
                diag_util_mprintf("DAC 300cm\n");
                break;
            case PORT_10GMEDIA_DAC_500CM:
                diag_util_mprintf("DAC 500cm\n");
                break;
            default:
                DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_10g_media_port_ports_all */
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_FIBER_RX
/*
 * port get port ( <PORT_LIST:ports> | all ) fiber rx
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_fiber_rx(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    rtk_enable_t        enable;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Fiber Rx Status:\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_fiberRxEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("  Port %d: ", port);

        if (ENABLED == enable)
            diag_util_mprintf("%s\n", "ENABLE");
        else
            diag_util_mprintf("%s\n", "DISABLE");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_fiber_rx */
#endif

#ifdef CMD_PORT_SET_PORT_PORTS_ALL_FIBER_RX_ENABLE_DISABLE
/*
 * port set port ( <PORT_LIST:ports> | all ) fiber rx ( enable | disable )
 */
cparser_result_t
cparser_cmd_port_set_port_ports_all_fiber_rx_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    rtk_enable_t        enable;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_fiberRxEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_port_ports_all_fiber_rx_enable_disable */
#endif

#ifdef CMD_PORT_SET_PORT_PORTS_ALL_FIBER_TX_ENABLE_DISABLE_OUTPUT_DATA
/*
 * port set port ( <PORT_LIST:ports> | all ) fiber tx ( enable | disable ) output <UINT:data>
 */
cparser_result_t cparser_cmd_port_set_port_ports_all_fiber_tx_enable_disable_output_data(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *data_ptr)
{
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    rtk_enable_t        enable;
    uint32              unit, data;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyFiberTxDis_set(unit, port, enable), ret);
    }

    if(enable == ENABLED)
    {
        if('1' == TOKEN_CHAR(8, 0))
            data = 1;
        else
            data = 0;
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyFiberTxDisPin_set(unit, port, data), ret);
        }
    }

    return CPARSER_OK;

}
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_LINK_MEDIA
/*
 * port get port ( <PORT_LIST:ports> | all ) link-media
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_link_media(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t         portlist;
    rtk_port_t              port;
    rtk_port_linkStatus_t   linkSts;
    rtk_port_media_t        media;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_linkMedia_get(unit, port, &linkSts, &media), ret);
        diag_util_mprintf("Port %d: ", port);
        if (PORT_LINKDOWN == linkSts)
        {
            diag_util_mprintf("Link down\n");
        }
        else
        {
            diag_util_mprintf("Link in ");
            if (PORT_MEDIA_COPPER == media)
            {
                diag_util_mprintf("Copper\n");
            }
            else
            {
                diag_util_mprintf("Fiber\n");
            }
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_link_media */
#endif


#ifdef CMD_PORT_SET_PHY_TEST_MODE_PORT_PORT_MODE_MODE_CHANNEL_A_B_C_D_NONE_ALL_PHY_PORTS
/*
 * port set phy-test-mode port <UINT:port> mode <UINT:mode>  channel ( a | b | c | d | none )  { all_phy_ports }
 */
cparser_result_t
cparser_cmd_port_set_phy_test_mode_port_port_mode_mode_channel_a_b_c_d_none_all_phy_ports(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *mode_ptr)
{
    uint32      unit;
    int32       ret;
    int         i, len;
    rtk_port_phyTestMode_t  testMode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&testMode, 0, sizeof(rtk_port_phyTestMode_t));
    testMode.mode = *mode_ptr;
    testMode.channel = PHY_TESTMODE_CHANNEL_NONE;
    for (i = 7; i < TOKEN_NUM; i++)
    {
        len = strlen(TOKEN_STR(i));
        if (!strncmp("channel", TOKEN_STR(i), len))
        {
            i++;
            if (TOKEN_CHAR(i, 0) == 'a')
                testMode.channel = PHY_TESTMODE_CHANNEL_A;
            else if (TOKEN_CHAR(i, 0) == 'b')
                testMode.channel = PHY_TESTMODE_CHANNEL_B;
            else if (TOKEN_CHAR(i, 0) == 'c')
                testMode.channel = PHY_TESTMODE_CHANNEL_C;
            else if (TOKEN_CHAR(i, 0) == 'd')
                testMode.channel = PHY_TESTMODE_CHANNEL_D;
            else if (TOKEN_CHAR(i, 0) == 'n')
                testMode.channel = PHY_TESTMODE_CHANNEL_NONE;
        }
        else if (!strncmp("all_phy_ports", TOKEN_STR(i), len))
        {
             testMode.flags |= RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_port_phyIeeeTestMode_set(unit, *port_ptr, &testMode), ret);

    return CPARSER_OK;
}



/*
 * port set phy-test-mode port <UINT:port> 10g-mode ( 1 | 2 | 3 | 4-1 | 4-2 | 4-4 | 4-5 | 4-6 | 5 | 6 | 7 | none )  channel ( a | b | c | d | none )
 */
cparser_result_t
cparser_cmd_port_set_phy_test_mode_port_port_10g_mode_1_2_3_4_1_4_2_4_4_4_5_4_6_5_6_7_none_channel_a_b_c_d_none(cparser_context_t *context,
    uint32_t *port_ptr)
{
    uint32      unit;
    int32       ret;
    int         len;
    char        *mode_str;
    rtk_port_phyTestMode_t  testMode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&testMode, 0, sizeof(rtk_port_phyTestMode_t));
    mode_str = TOKEN_STR(6);
    len = strlen(mode_str);
    testMode.channel = PHY_TESTMODE_CHANNEL_NONE;
    if (!strncmp("1", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE1;
    else if (!strncmp("2", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE2;
    else if (!strncmp("3", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE3;
    else if (!strncmp("4-1", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE4_1;
    else if (!strncmp("4-2", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE4_2;
    else if (!strncmp("4-4", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE4_4;
    else if (!strncmp("4-5", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE4_5;
    else if (!strncmp("4-6", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE4_6;
    else if (!strncmp("5", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE5;
    else if (!strncmp("6", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE6;
    else if (!strncmp("7", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE7;
    else if (!strncmp("none", mode_str, len))
        testMode.mode = RTK_PORT_PHY_10G_TEST_MODE_NONE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if (TOKEN_NUM >= 9)
    {
        if (TOKEN_CHAR(8, 0) == 'a')
            testMode.channel = PHY_TESTMODE_CHANNEL_A;
        else if (TOKEN_CHAR(8, 0) == 'b')
            testMode.channel = PHY_TESTMODE_CHANNEL_B;
        else if (TOKEN_CHAR(8, 0) == 'c')
            testMode.channel = PHY_TESTMODE_CHANNEL_C;
        else if (TOKEN_CHAR(8, 0) == 'd')
            testMode.channel = PHY_TESTMODE_CHANNEL_D;
        else if (TOKEN_CHAR(8, 0) == 'n')
            testMode.channel = PHY_TESTMODE_CHANNEL_NONE;
    }

    DIAG_UTIL_ERR_CHK(rtk_port_phyIeeeTestMode_set(unit, *port_ptr, &testMode), ret);

    return CPARSER_OK;
}

/*
 * port set phy-test-mode port <UINT:port> 10g-mode ( 1 | 2 | 3 | 4-1 | 4-2 | 4-4 | 4-5 | 4-6 | 5 | 6 | 7 | none )
 */
cparser_result_t
cparser_cmd_port_set_phy_test_mode_port_port_10g_mode_1_2_3_4_1_4_2_4_4_4_5_4_6_5_6_7_none(cparser_context_t *context,
    uint32_t *port_ptr)
{
    return cparser_cmd_port_set_phy_test_mode_port_port_10g_mode_1_2_3_4_1_4_2_4_4_4_5_4_6_5_6_7_none_channel_a_b_c_d_none(context, port_ptr);
}

/*
 * port set phy-test-mode port <UINT:port> 100m-mode ( ieee | ansi-jitter | ansi-droop | none )
 */
cparser_result_t
cparser_cmd_port_set_phy_test_mode_port_port_100m_mode_ieee_ansi_jitter_ansi_droop_none(cparser_context_t *context,
    uint32_t *port_ptr)
{
    uint32      unit;
    int32       ret;
    int         len;
    char        *mode_str;
    rtk_port_phyTestMode_t  testMode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&testMode, 0, sizeof(rtk_port_phyTestMode_t));
    mode_str = TOKEN_STR(6);
    len = strlen(mode_str);
    testMode.channel = PHY_TESTMODE_CHANNEL_NONE;
    if (!strncmp("ieee", mode_str, len))
        testMode.mode = RTK_PORT_PHY_100M_TEST_MODE_IEEE;
    else if (!strncmp("ansi-jitter", mode_str, len))
        testMode.mode = RTK_PORT_PHY_100M_TEST_MODE_ANSI_JITTER;
    else if (!strncmp("ansi-droop", mode_str, len))
        testMode.mode = RTK_PORT_PHY_100M_TEST_MODE_ANSI_DROOP;
    else if (!strncmp("none", mode_str, len))
        testMode.mode = RTK_PORT_PHY_100M_TEST_MODE_NONE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_port_phyIeeeTestMode_set(unit, *port_ptr, &testMode), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_GET_PHY_POLARITY_PORT_PORT
char *
_phy_polarity_string_get(rtk_phy_polarity_t polar)
{
    switch (polar)
    {
      case PHY_POLARITY_NORMAL:
        return "normal";
      case PHY_POLARITY_INVERSE:
        return "inversed";
      default:
        return "unknown";
    }
}
#endif

#ifdef CMD_PORT_GET_PHY_POLARITY_PORT_PORT
/*
 * port get phy-polarity port <UINT:port>
 */
cparser_result_t
cparser_cmd_port_get_phy_polarity_port_port(cparser_context_t *context,
    uint32_t *port_ptr)
{
    uint32      unit;
    int32       ret;
    rtk_port_phyPolarCtrl_t     polarCtrl;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&polarCtrl, 0, sizeof(rtk_port_phyPolarCtrl_t));
    DIAG_UTIL_ERR_CHK(rtk_port_phyPolar_get(unit, *port_ptr, &polarCtrl), ret);
    diag_util_mprintf("Port %d: tx %s, rx %s \n", *port_ptr, _phy_polarity_string_get(polarCtrl.phy_polar_tx), _phy_polarity_string_get(polarCtrl.phy_polar_rx));

    return CPARSER_OK;
}
#endif


#if defined(CMD_PORT_SET_PHY_POLARITY_PORT_PORT_TX_NORMAL_INVERSE) || defined(CMD_PORT_SET_PHY_POLARITY_PORT_PORT_RX_NORMAL_INVERSE)
rtk_phy_polarity_t
_phy_polarity_enum_get(char *polar_str)
{
    int     len;

    if ((len = strlen(polar_str)) <= 0)
    {
        return PHY_POLARITY_END;
    }

    if (!strncmp("normal", polar_str, len))
    {
        return PHY_POLARITY_NORMAL;
    }
    else if (!strncmp("inverse", polar_str, len))
    {
        return PHY_POLARITY_INVERSE;
    }
    else
    {
        return PHY_POLARITY_END;
    }
}
#endif

#ifdef CMD_PORT_SET_PHY_POLARITY_PORT_PORT_TX_NORMAL_INVERSE
/*
 * port set phy-polarity port <UINT:port> tx ( normal | inverse )
 */
cparser_result_t
cparser_cmd_port_set_phy_polarity_port_port_tx_normal_inverse(cparser_context_t *context,
    uint32_t *port_ptr)
{
    uint32      unit;
    int32       ret;
    rtk_port_phyPolarCtrl_t     polarCtrl;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&polarCtrl, 0, sizeof(rtk_port_phyPolarCtrl_t));
    DIAG_UTIL_ERR_CHK(rtk_port_phyPolar_get(unit, *port_ptr, &polarCtrl), ret);

    polarCtrl.phy_polar_tx = _phy_polarity_enum_get(TOKEN_STR(6));

    DIAG_UTIL_ERR_CHK(rtk_port_phyPolar_set(unit, *port_ptr, &polarCtrl), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_POLARITY_PORT_PORT_RX_NORMAL_INVERSE
/*
 * port set phy-polarity port <UINT:port> rx ( normal | inverse )
 */
cparser_result_t
cparser_cmd_port_set_phy_polarity_port_port_rx_normal_inverse(cparser_context_t *context,
    uint32_t *port_ptr)
{
    uint32      unit;
    int32       ret;
    rtk_port_phyPolarCtrl_t     polarCtrl;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&polarCtrl, 0, sizeof(rtk_port_phyPolarCtrl_t));
    DIAG_UTIL_ERR_CHK(rtk_port_phyPolar_get(unit, *port_ptr, &polarCtrl), ret);

    polarCtrl.phy_polar_rx = _phy_polarity_enum_get(TOKEN_STR(6));

    DIAG_UTIL_ERR_CHK(rtk_port_phyPolar_set(unit, *port_ptr, &polarCtrl), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_SERDES
/*
 * port get port ( <PORT_LIST:ports> | all ) serdes
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_serdes(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    rtk_sdsCfg_t        sdsCfg;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("SerDes information:\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phySds_get(unit, port, &sdsCfg), ret);
        diag_util_mprintf("  Port %d: ", port);

        switch (sdsCfg.sdsMode)
        {
            case RTK_MII_SGMII:
                diag_util_mprintf("SGMII\n");
                break;
            case RTK_MII_1000BX_FIBER:
                diag_util_mprintf("1000 base-X\n");
                break;
            default:
                diag_util_mprintf("Unknown\n");
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_serdes */
#endif

#ifdef CMD_PORT_SET_PORT_PORTS_ALL_SERDES_SGMII_1000BX
/*
 * port set port ( <PORT_LIST:ports> | all ) serdes ( sgmii | 1000bx )
 */
cparser_result_t
cparser_cmd_port_set_port_ports_all_serdes_sgmii_1000bx(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    rtk_sdsCfg_t        sdsCfg;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('s' == TOKEN_CHAR(5, 0))
        sdsCfg.sdsMode = RTK_MII_SGMII;
    else
        sdsCfg.sdsMode = RTK_MII_1000BX_FIBER;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phySds_set(unit, port, &sdsCfg), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_port_ports_all_serdes_sgmii_1000bx */
#endif


#ifdef CMD_PORT_GET_PORT_PORTS_ALL_PHY_RX_CALI_STATUS_SERDES_ID_SDSID
/*
 * port get port ( <PORT_LIST:ports> | all ) phy rx-cali-status serdes-id <UINT:sdsId>
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_phy_rx_cali_status_serdes_id_sdsId(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr)
{
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    rtk_port_phySdsRxCaliStatus_t        status;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("SerDes information:\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phySdsRxCaliStatus_get(unit, port, *sdsId_ptr, &status), ret);
        diag_util_mprintf("  Port %d: ", port);

        switch (status)
        {
            case PHY_SDS_RXCALI_STATUS_NOINFO:
                diag_util_mprintf("NOINFO\n");
                break;
            case PHY_SDS_RXCALI_STATUS_OK:
                diag_util_mprintf("OK\n");
                break;
            case PHY_SDS_RXCALI_STATUS_FAILED:
                diag_util_mprintf("FAILED\n");
                break;
            default:
                diag_util_mprintf("Unknown (%u)\n", status);
                break;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_EYE_MONITOR_PORT_SDSID_FRAMENUM
/*
 * port set phy-eye-monitor <UINT:port> <UINT:sdsId> <UINT:frameNum>
 */
cparser_result_t
cparser_cmd_port_set_phy_eye_monitor_port_sdsId_frameNum(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr,
    uint32_t *frameNum_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_port_phyEyeMonitor_start(unit, *port_ptr, *sdsId_ptr, *frameNum_ptr), ret);

    diag_util_printf("Done\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_FORCE_PORT_PORTS_ALL_FLOW_CONTROL_TX_PAUSE_DISABLE_ENABLE_RX_PAUSE_DISABLE_ENABLE
/*
 * port set phy-force port ( <PORT_LIST:ports> | all ) flow-control tx-pause ( disable | enable ) rx-pause ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_phy_force_port_ports_all_flow_control_tx_pause_disable_enable_rx_pause_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    char                *token;
    int32               len;
    rtk_port_flowctrl_mode_t    fcMode;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    token = TOKEN_STR(7);
    len = osal_strlen(token);
    if (!strncmp("enable", token, len))
    {
        fcMode.tx_pause = ENABLED;
    }
    else if (!strncmp("disable", token, len))
    {
        fcMode.tx_pause = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    token = TOKEN_STR(9);
    len = osal_strlen(token);
    if (!strncmp("enable", token, len))
    {
        fcMode.rx_pause = ENABLED;
    }
    else if (!strncmp("disable", token, len))
    {
        fcMode.rx_pause = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyForceFlowctrlMode_set(unit, port, &fcMode), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE_REGISTER_REGISTER_BITS_MSB_LSB
/*
 * port get phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register> bits <UINT:msb> <UINT:lsb>
 */
cparser_result_t cparser_cmd_port_get_phy_reg_port_ports_all_page_page_register_register_bits_msb_lsb(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *page_ptr,
    uint32_t *register_ptr,
    uint32_t *msb_ptr,
    uint32_t *lsb_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    uint32              org_data = 0;
    uint32              data = 0;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_RANGE_CHK((page_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((register_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((msb_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((lsb_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*register_ptr > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK(*lsb_ptr > *msb_ptr, CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, *page_ptr, *register_ptr, &org_data), ret);

        data = REG32_FIELD_GET(org_data, *lsb_ptr, UINT32_BITS_MASK(*msb_ptr, *lsb_ptr));
        diag_util_mprintf("  Port %d: \n", port);
        diag_util_mprintf("     Page %d: \n", *page_ptr);
        diag_util_mprintf("     0x%02X    0x%04X, [%d:%d] 0x%04X \n", *page_ptr, org_data, *msb_ptr, *lsb_ptr, data);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_REG_PORT_PORTS_ALL_PAGE_PAGE_REGISTER_REGISTER_BITS_MSB_LSB_VALUE
/*
 * port set phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register> bits <UINT:msb> <UINT:lsb> <UINT:value>
 */
cparser_result_t cparser_cmd_port_set_phy_reg_port_ports_all_page_page_register_register_bits_msb_lsb_value(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *page_ptr,
    uint32_t *register_ptr,
    uint32_t *msb_ptr,
    uint32_t *lsb_ptr,
    uint32_t *value_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    uint32              data = 0;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PARAM_RANGE_CHK((page_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((register_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((msb_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((lsb_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*register_ptr > MAX_PHY_REGISTER), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK(*lsb_ptr > *msb_ptr, CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(unit, port, *page_ptr, *register_ptr, &data), ret);

        data = REG32_FIELD_SET(data, *value_ptr, *lsb_ptr, UINT32_BITS_MASK(*msb_ptr, *lsb_ptr));
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(unit, port, *page_ptr, *register_ptr, data), ret);
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_MAC_INTF_SERDES_MODE_PORT_PORTS_ALL
/*
 *  port get phy-mac-intf serdes-mode port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_phy_mac_intf_serdes_mode_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    rt_serdesMode_t     sdsMode;
    uint32              is_all = FALSE;
    char                *port_str;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyMacIntfSerdesMode_get(unit, port, &sdsMode);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("PHY of Port %u : ", port);
                DIAG_ERR_PRINT(ret);
            }
        }
        else
        {
            diag_util_mprintf("PHY of Port %u : %s\n", port, diag_util_serdesMode2str(sdsMode));
        }
    } /* end DIAG_UTIL_PORTMASK_SCAN */

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PORT_PORTS_ALL_PHY_LINK_STATUS
/*
 * port get port ( <PORT_LIST:ports> | all ) phy-link-status
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_phy_link_status(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    rtk_port_linkStatus_t   linkSts;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port_str = TOKEN_STR(3);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("PHY interface Link Status:\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyLinkStatus_get(unit, port, &linkSts);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("  PHY of Port %u : ", port);
                DIAG_ERR_PRINT(ret);
            }
        }
        else
        {
            diag_util_mprintf("  PHY of Port %u : %s\n", port, (linkSts == PORT_LINKDOWN ? "LINK DOWN" : "LINK UP"));
        }
    } /* end DIAG_UTIL_PORTMASK_SCAN */


    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_phy_link_status */
#endif

#ifdef CMD_PORT_GET_PEER_AUTO_NEGO_PORT_PORTS_ALL_ABILITY
/*
 * port get peer auto-nego port ( <PORT_LIST:ports> | all ) ability
 */
cparser_result_t
cparser_cmd_port_get_peer_auto_nego_port_ports_all_ability(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_phy_ability_t  ability;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str;


    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_util_mprintf("PHY interface Port Peer AN ability:\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("PHY interface Port %d: ", port);
        ret = rtk_port_phyPeerAutoNegoAbility_get(unit, port, &ability);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("  PHY of Port %u: ", port);
                DIAG_ERR_PRINT(ret);
            }
        }
        else
        {
            if (ABILITY_BIT_ON == ability.Half_10)
                diag_util_printf("10H ");
            if (ABILITY_BIT_ON == ability.Full_10)
                diag_util_printf("10F ");
            if (ABILITY_BIT_ON == ability.Half_100)
                diag_util_printf("100H ");
            if (ABILITY_BIT_ON == ability.Full_100)
                diag_util_printf("100F ");
            if (ABILITY_BIT_ON == ability.Full_1000)
                diag_util_printf("1000F ");
            if (ABILITY_BIT_ON == ability.adv_2_5G)
               diag_util_printf("2.5G ");
            if (ABILITY_BIT_ON == ability.adv_5G)
               diag_util_printf("5G ");
            if (ABILITY_BIT_ON == ability.adv_10GBase_T)
               diag_util_printf("10GBase_T ");
            if (ABILITY_BIT_ON == ability.FC)
                diag_util_printf("Flow-Control ");
            if (ABILITY_BIT_ON == ability.AsyFC)
                diag_util_printf("Asy-Flow-Control ");
            diag_util_mprintf("\n");
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_peer_auto_nego_port_ports_all_ability */
#endif

#ifdef CMD_PORT_RESET_PORT_PORTS_ALL
/*
 * port reset port ( <PORT_LIST:ports> | all ) */
cparser_result_t
cparser_cmd_port_reset_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port_str = TOKEN_STR(3);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Reset PHY interface :\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyReset_set(unit, port);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("  PHY of Port %u : ", port);
                DIAG_ERR_PRINT(ret);
            }
        }
        else
        {
            diag_util_mprintf("  Reset PHY of Port %u\n", port);
        }
    } /* end DIAG_UTIL_PORTMASK_SCAN */

    return CPARSER_OK;
}   /* end of cparser_cmd_port_reset_port_ports_all */
#endif


#ifdef CMD_PORT_SET_PHY_LED_MODE_PORT_PORTS_ALL_LED_ID_MDI_MDI_LED_INDICATOR_1000M_SPEED_100M_SPEED_10M_SPEED_1000M_ACT_100M_ACT_10M_ACT_DUPLEX_COLLISION_TX_ACT_RX_ACT
/*
 * port set phy-led mode port ( <PORT_LIST:ports> | all ) led <UINT:id> mdi <UINT:mdi> led-indicator { 1000M-speed } { 100M-speed } { 10M-speed } { 1000M-act } { 100M-act } { 10M-act } { duplex } { collision } { tx-act } { rx-act }
 */
cparser_result_t
cparser_cmd_port_set_phy_led_mode_port_ports_all_led_id_mdi_mdi_led_indicator_1000M_speed_100M_speed_10M_speed_1000M_act_100M_act_10M_act_duplex_collision_tx_act_rx_act(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *id_ptr,
    uint32_t *mdi_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    rtk_phy_ledMode_t   phyLedMode;
    char                *str;
    int32               i;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    osal_memset(&phyLedMode, 0, sizeof(rtk_phy_ledMode_t));
    phyLedMode.led_id = *id_ptr;
    phyLedMode.mdi = *mdi_ptr;
    for (i = 11; i < TOKEN_NUM; i++)
    {
        str = TOKEN_STR(i);
        if (!strncmp("1000M-speed", str, strlen(str)))
        {
            phyLedMode.led_ind_status_sel |= RTK_PHY_LED_IND_STATUS_SEL_1000M_SPEED;
        }
        if (!strncmp("100M-speed", str, strlen(str)))
        {
            phyLedMode.led_ind_status_sel |= RTK_PHY_LED_IND_STATUS_SEL_100M_SPEED;
        }
        if (!strncmp("10M-speed", str, strlen(str)))
        {
            phyLedMode.led_ind_status_sel |= RTK_PHY_LED_IND_STATUS_SEL_10M_SPEED;
        }
        if (!strncmp("1000M-act", str, strlen(str)))
        {
            phyLedMode.led_ind_status_sel |= RTK_PHY_LED_IND_STATUS_SEL_1000M_ACTIVITY;
        }
        if (!strncmp("100M-act", str, strlen(str)))
        {
            phyLedMode.led_ind_status_sel |= RTK_PHY_LED_IND_STATUS_SEL_100M_ACTIVITY;
        }
        if (!strncmp("10M-act", str, strlen(str)))
        {
            phyLedMode.led_ind_status_sel |= RTK_PHY_LED_IND_STATUS_SEL_10M_ACTIVITY;
        }
        if (!strncmp("duplex", str, strlen(str)))
        {
            phyLedMode.led_ind_status_sel |= RTK_PHY_LED_IND_STATUS_SEL_DUPLEX;
        }
        if (!strncmp("collision", str, strlen(str)))
        {
            phyLedMode.led_ind_status_sel |= RTK_PHY_LED_IND_STATUS_SEL_COLLISION;
        }
        if (!strncmp("tx-act", str, strlen(str)))
        {
            phyLedMode.led_ind_status_sel |= RTK_PHY_LED_IND_STATUS_SEL_TX_ACTIVITY;
        }
        if (!strncmp("rx-act", str, strlen(str)))
        {
            phyLedMode.led_ind_status_sel |= RTK_PHY_LED_IND_STATUS_SEL_RX_ACTIVITY;
        }
    }/* end for (i) */

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyLedMode_set(unit, port, &phyLedMode);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of Port %u \n", port);
            DIAG_ERR_PRINT(ret);
        }
    } /* end DIAG_UTIL_PORTMASK_SCAN */

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_LED_CONTROL_PORT_PORTS_ALL_BLINK_RATE_32MS_48MS_64MS_96MS_128MS_256MS_512MS_1024MS
/*
 * port set phy-led control port ( <PORT_LIST:ports> | all ) blink-rate ( 32ms | 48ms | 64ms | 96ms | 128ms | 256ms | 512ms | 1024ms )
 */
cparser_result_t
cparser_cmd_port_set_phy_led_control_port_ports_all_blink_rate_32ms_48ms_64ms_96ms_128ms_256ms_512ms_1024ms(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_port_t          port;
    diag_portlist_t     portlist;
    rtk_phy_ledCtrl_t   phyLedCtrl;
    char                *str;
    rtk_phy_ledBlinkRate_t  blink_rate;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    str = TOKEN_STR(7);
    if (!strncmp("32ms", str, strlen(str)))
    {
        blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_32MS;
    }
    else if (!strncmp("48ms", str, strlen(str)))
    {
        blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_48MS;
    }
    else if (!strncmp("64ms", str, strlen(str)))
    {
        blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_64MS;
    }
    else if (!strncmp("96ms", str, strlen(str)))
    {
        blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_96MS;
    }
    else if (!strncmp("128ms", str, strlen(str)))
    {
        blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_128MS;
    }
    else if (!strncmp("256ms", str, strlen(str)))
    {
        blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_256MS;
    }
    else if (!strncmp("512ms", str, strlen(str)))
    {
        blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_512MS;
    }
    else if (!strncmp("1024ms", str, strlen(str)))
    {
        blink_rate = RTK_PHY_LED_CTRL_BLINK_RATE_1024MS;
    }
    else
    {
        diag_util_mprintf("Error! Unreconized input: %s \n", str);
        return CPARSER_OK;
    }


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&phyLedCtrl, 0, sizeof(rtk_phy_ledCtrl_t));
        ret = rtk_port_phyLedCtrl_get(unit, port, &phyLedCtrl);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of Port %u \n", port);
            DIAG_ERR_PRINT(ret);
            continue;
        }

        phyLedCtrl.blink_rate = blink_rate;
        ret = rtk_port_phyLedCtrl_set(unit, port, &phyLedCtrl);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of Port %u \n", port);
            DIAG_ERR_PRINT(ret);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_LED_CONTROL_PORT_PORTS_ALL_BURST_CYCLE_8MS_16MS_32MS_64MS
/*
 * port set phy-led control port ( <PORT_LIST:ports> | all ) burst-cycle ( 8ms | 16ms | 32ms | 64ms )
 */
cparser_result_t
cparser_cmd_port_set_phy_led_control_port_ports_all_burst_cycle_8ms_16ms_32ms_64ms(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_port_t          port;
    diag_portlist_t     portlist;
    rtk_phy_ledCtrl_t   phyLedCtrl;
    char                *str;
    rtk_phy_ledBurstCycle_t burst_cycle;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    str = TOKEN_STR(7);
    if (!strncmp("8ms", str, strlen(str)))
    {
        burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_8MS;
    }
    else if (!strncmp("16ms", str, strlen(str)))
    {
        burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_16MS;
    }
    else if (!strncmp("32ms", str, strlen(str)))
    {
        burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_32MS;
    }
    else if (!strncmp("64ms", str, strlen(str)))
    {
        burst_cycle = RTK_PHY_LED_CTRL_BURST_CYCLE_64MS;
    }
    else
    {
        diag_util_mprintf("Error! Unreconized input: %s \n", str);
        return CPARSER_OK;
    }


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&phyLedCtrl, 0, sizeof(rtk_phy_ledCtrl_t));
        ret = rtk_port_phyLedCtrl_get(unit, port, &phyLedCtrl);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of Port %u \n", port);
            DIAG_ERR_PRINT(ret);
            continue;
        }

        phyLedCtrl.burst_cycle = burst_cycle;
        ret = rtk_port_phyLedCtrl_set(unit, port, &phyLedCtrl);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of Port %u \n", port);
            DIAG_ERR_PRINT(ret);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_LED_CONTROL_PORT_PORTS_ALL_CLOCK_CYCLE_32NS_64NS_96NS_192NS
/*
 * port set phy-led control port ( <PORT_LIST:ports> | all ) clock-cycle ( 32ns | 64ns | 96ns | 192ns )
 */
cparser_result_t
cparser_cmd_port_set_phy_led_control_port_ports_all_clock_cycle_32ns_64ns_96ns_192ns(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_port_t          port;
    diag_portlist_t     portlist;
    rtk_phy_ledCtrl_t   phyLedCtrl;
    char                *str;
    rtk_phy_ledClockCycle_t clock_cycle;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    str = TOKEN_STR(7);
    if (!strncmp("32ns", str, strlen(str)))
    {
        clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_32NS;
    }
    else if (!strncmp("64ns", str, strlen(str)))
    {
        clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_64NS;
    }
    else if (!strncmp("96ns", str, strlen(str)))
    {
        clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_96NS;
    }
    else if (!strncmp("192ns", str, strlen(str)))
    {
        clock_cycle = RTK_PHY_LED_CTRL_CLOCK_CYCLE_192NS;
    }
    else
    {
        diag_util_mprintf("Error! Unreconized input: %s \n", str);
        return CPARSER_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&phyLedCtrl, 0, sizeof(rtk_phy_ledCtrl_t));
        ret = rtk_port_phyLedCtrl_get(unit, port, &phyLedCtrl);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of Port %u \n", port);
            DIAG_ERR_PRINT(ret);
            continue;
        }

        phyLedCtrl.clock_cycle = clock_cycle;
        ret = rtk_port_phyLedCtrl_set(unit, port, &phyLedCtrl);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of Port %u \n", port);
            DIAG_ERR_PRINT(ret);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_LED_CONTROL_PORT_PORTS_ALL_ACTIVE_HIGH_LOW
/*
 * port set phy-led control port ( <PORT_LIST:ports> | all ) active ( high | low )
 */
cparser_result_t
cparser_cmd_port_set_phy_led_control_port_ports_all_active_high_low(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_port_t          port;
    diag_portlist_t     portlist;
    rtk_phy_ledCtrl_t   phyLedCtrl;
    char                *str;
    rtk_phy_ledActiveLow_t active_low;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    str = TOKEN_STR(7);
    if (!strncmp("high", str, strlen(str)))
    {
        active_low = RTK_PHY_LED_CTRL_ACTIVE_HIGH;
    }
    else if (!strncmp("low", str, strlen(str)))
    {
        active_low = RTK_PHY_LED_CTRL_ACTIVE_LOW;
    }
    else
    {
        diag_util_mprintf("Error! Unreconized input: %s \n", str);
        return CPARSER_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&phyLedCtrl, 0, sizeof(rtk_phy_ledCtrl_t));
        ret = rtk_port_phyLedCtrl_get(unit, port, &phyLedCtrl);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of Port %u \n", port);
            DIAG_ERR_PRINT(ret);
            continue;
        }

        phyLedCtrl.active_low = active_low;
        ret = rtk_port_phyLedCtrl_set(unit, port, &phyLedCtrl);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of Port %u \n", port);
            DIAG_ERR_PRINT(ret);
        }
    }
    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_GET_PHY_LED_CONTROL_PORT_PORTS_ALL
/*
 * port get phy-led control port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_phy_led_control_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_port_t          port;
    diag_portlist_t     portlist;
    rtk_phy_ledCtrl_t   phyLedCtrl;
    uint32              is_all = FALSE;
    char                *port_str;
    char                *str_blink_rate, *str_burst_cycle, *str_clock_cycle, *str_active_low;

    DIAG_UTIL_FUNC_INIT(unit);

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&phyLedCtrl, 0, sizeof(rtk_phy_ledCtrl_t));
        ret = rtk_port_phyLedCtrl_get(unit, port, &phyLedCtrl);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("PHY of Port %u: \n", port);
                DIAG_ERR_PRINT(ret);
            }
        }
        else
        {
            if (phyLedCtrl.blink_rate == RTK_PHY_LED_CTRL_BLINK_RATE_32MS)
                str_blink_rate = "32ms";
            else if (phyLedCtrl.blink_rate == RTK_PHY_LED_CTRL_BLINK_RATE_48MS)
                str_blink_rate = "48ms";
            else if (phyLedCtrl.blink_rate == RTK_PHY_LED_CTRL_BLINK_RATE_64MS)
                str_blink_rate = "64ms";
            else if (phyLedCtrl.blink_rate == RTK_PHY_LED_CTRL_BLINK_RATE_96MS)
                str_blink_rate = "96ms";
            else if (phyLedCtrl.blink_rate == RTK_PHY_LED_CTRL_BLINK_RATE_128MS)
                str_blink_rate = "128ms";
            else if (phyLedCtrl.blink_rate == RTK_PHY_LED_CTRL_BLINK_RATE_256MS)
                str_blink_rate = "256ms";
            else if (phyLedCtrl.blink_rate == RTK_PHY_LED_CTRL_BLINK_RATE_512MS)
                str_blink_rate = "512ms";
            else if (phyLedCtrl.blink_rate == RTK_PHY_LED_CTRL_BLINK_RATE_1024MS)
                str_blink_rate = "1024ms";
            else
                str_blink_rate = "Unknown";

            if (phyLedCtrl.burst_cycle == RTK_PHY_LED_CTRL_BURST_CYCLE_8MS)
                str_burst_cycle = "8ms";
            else if (phyLedCtrl.burst_cycle == RTK_PHY_LED_CTRL_BURST_CYCLE_16MS)
                str_burst_cycle = "16ms";
            else if (phyLedCtrl.burst_cycle == RTK_PHY_LED_CTRL_BURST_CYCLE_32MS)
                str_burst_cycle = "32ms";
            else if (phyLedCtrl.burst_cycle == RTK_PHY_LED_CTRL_BURST_CYCLE_64MS)
                str_burst_cycle = "64ms";
            else
                str_burst_cycle = "Unknown";

            if (phyLedCtrl.clock_cycle == RTK_PHY_LED_CTRL_CLOCK_CYCLE_32NS)
                str_clock_cycle = "32ns";
            else if (phyLedCtrl.clock_cycle == RTK_PHY_LED_CTRL_CLOCK_CYCLE_64NS)
                str_clock_cycle = "64ns";
            else if (phyLedCtrl.clock_cycle == RTK_PHY_LED_CTRL_CLOCK_CYCLE_96NS)
                str_clock_cycle = "96ns";
            else if (phyLedCtrl.clock_cycle == RTK_PHY_LED_CTRL_CLOCK_CYCLE_192NS)
                str_clock_cycle = "192ns";
            else
                str_clock_cycle = "Unknown";

            if (phyLedCtrl.active_low == RTK_PHY_LED_CTRL_ACTIVE_HIGH)
                str_active_low = "high";
            else if (phyLedCtrl.active_low == RTK_PHY_LED_CTRL_ACTIVE_LOW)
                str_active_low = "low";
            else
                str_active_low = "Unknown";

            diag_util_mprintf("PHY of Port %u \n", port);
            diag_util_mprintf("  blink rate  :  %s \n", str_blink_rate);
            diag_util_mprintf("  burst cycle :  %s \n", str_burst_cycle);
            diag_util_mprintf("  clock cycle :  %s \n", str_clock_cycle);
            diag_util_mprintf("  active low  :  %s \n\n", str_active_low);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_MAC_INTF_SERDES_LINK_STATUS_PORT_PORTS_ALL
/*
 * port get phy-mac-intf serdes-link-status port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_phy_mac_intf_serdes_link_status_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    rtk_phy_macIntfSdsLinkStatus_t   linkSts;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str;
    int32   ret;
    int32   serdes_id;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_util_mprintf("SERDES interface Link Status:\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyMacIntfSerdesLinkStatus_get(unit, port, &linkSts);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("PHY of port %d ", port);
                DIAG_ERR_PRINT(ret);
            }
        }
        else
        {
            for(serdes_id = 0; serdes_id < linkSts.sds_num; serdes_id++)
            {
                diag_util_mprintf("PHY of port %d SERDES[%d] : ", port, serdes_id);
                diag_util_mprintf(" %s\n", (linkSts.link_status[serdes_id] == PORT_LINKDOWN ? "LINK DOWN" : "LINK UP"));
            }
        }
    } /* end DIAG_UTIL_PORTMASK_SCAN */


    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phy_serdes_link_status_port_ports_all */
#endif


#ifdef CMD_PORT_GET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID
/*
 * port get phy-serdes eye-param port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId>
 */
cparser_result_t
cparser_cmd_port_get_phy_serdes_eye_param_port_ports_all_serdes_id_sdsId(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    rtk_sds_eyeParam_t      eyeParam;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&eyeParam, 0, sizeof(rtk_sds_eyeParam_t));
        ret = rtk_port_phySdsEyeParam_get(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
                DIAG_ERR_PRINT(ret);
            }
        }
        else
        {
            diag_util_mprintf("PHY of port %u SERDES %u\n", port, *sdsId_ptr);

            diag_util_mprintf(" Pre-AMP State : %s\n", (eyeParam.pre_en == ENABLED) ? DIAG_STR_ENABLE: DIAG_STR_DISABLE);
            diag_util_mprintf(" Pre-AMP       : %u\n", eyeParam.pre_amp);
            diag_util_mprintf(" Main-AMP      : %u\n", eyeParam.main_amp);
            diag_util_mprintf(" Post-AMP State: %s\n", (eyeParam.post_en == ENABLED) ? DIAG_STR_ENABLE: DIAG_STR_DISABLE);
            diag_util_mprintf(" Post-AMP      : %u\n", eyeParam.post_amp);
            diag_util_mprintf(" Impedance     : %u\n", eyeParam.impedance);
        }
    } /* end DIAG_UTIL_PORTMASK_SCAN */

    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_PRE_STATE_DISABLE_ENABLE
/*
 * port set phy-serdes eye-param port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> pre-state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_eye_param_port_ports_all_serdes_id_sdsId_pre_state_disable_enable(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    rtk_sds_eyeParam_t      eyeParam;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str, *en_str;
    rtk_enable_t            en_cfg;
    int32   ret, len;


    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    en_str = TOKEN_STR(9);
    len = strlen(en_str);
    if (!strncmp("enable", en_str, len))
    {
        en_cfg = ENABLED;
    }
    else if (!strncmp("disable", en_str, len))
    {
        en_cfg = DISABLED;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&eyeParam, 0, sizeof(rtk_sds_eyeParam_t));
        ret = rtk_port_phySdsEyeParam_get(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
                DIAG_ERR_PRINT(ret);
            }
            continue;
        }

        eyeParam.pre_en = en_cfg;
        ret = rtk_port_phySdsEyeParam_set(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
            DIAG_ERR_PRINT(ret);
        }
    }/* DIAG_UTIL_PORTMASK_SCAN */
    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_POST_STATE_DISABLE_ENABLE
/*
 * port set phy-serdes eye-param port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> post-state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_eye_param_port_ports_all_serdes_id_sdsId_post_state_disable_enable(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    rtk_sds_eyeParam_t      eyeParam;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str, *en_str;
    rtk_enable_t            en_cfg;
    int32   ret, len;


    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    en_str = TOKEN_STR(9);
    len = strlen(en_str);
    if (!strncmp("enable", en_str, len))
    {
        en_cfg = ENABLED;
    }
    else if (!strncmp("disable", en_str, len))
    {
        en_cfg = DISABLED;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&eyeParam, 0, sizeof(rtk_sds_eyeParam_t));
        ret = rtk_port_phySdsEyeParam_get(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
                DIAG_ERR_PRINT(ret);
            }
            continue;
        }

        eyeParam.post_en = en_cfg;
        ret = rtk_port_phySdsEyeParam_set(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
            DIAG_ERR_PRINT(ret);
        }
    }/* DIAG_UTIL_PORTMASK_SCAN */
    return CPARSER_OK;

}
#endif

#ifdef CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_PRE_AMP_PRE_AMP
/*
 * port set phy-serdes eye-param port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> pre-amp <UINT:pre_amp>
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_eye_param_port_ports_all_serdes_id_sdsId_pre_amp_pre_amp(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr,
    uint32_t *pre_amp_ptr)
{
    uint32  unit;
    rtk_sds_eyeParam_t      eyeParam;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str;
    int32   ret;


    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&eyeParam, 0, sizeof(rtk_sds_eyeParam_t));
        ret = rtk_port_phySdsEyeParam_get(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
                DIAG_ERR_PRINT(ret);
            }
            continue;
        }

        eyeParam.pre_amp = *pre_amp_ptr;
        ret = rtk_port_phySdsEyeParam_set(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
            DIAG_ERR_PRINT(ret);
        }
    }/* DIAG_UTIL_PORTMASK_SCAN */
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_MAIN_AMP_MAIN_AMP
/*
 * port set phy-serdes eye-param port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> main-amp <UINT:main_amp>
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_eye_param_port_ports_all_serdes_id_sdsId_main_amp_main_amp(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr,
    uint32_t *main_amp_ptr)
{
    uint32  unit;
    rtk_sds_eyeParam_t      eyeParam;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str;
    int32   ret;


    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&eyeParam, 0, sizeof(rtk_sds_eyeParam_t));
        ret = rtk_port_phySdsEyeParam_get(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
                DIAG_ERR_PRINT(ret);
            }
            continue;
        }

        eyeParam.main_amp = *main_amp_ptr;
        ret = rtk_port_phySdsEyeParam_set(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
            DIAG_ERR_PRINT(ret);
        }
    }/* DIAG_UTIL_PORTMASK_SCAN */
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_POST_AMP_POST_AMP
/*
 * port set phy-serdes eye-param port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> post-amp <UINT:post_amp>
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_eye_param_port_ports_all_serdes_id_sdsId_post_amp_post_amp(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr,
    uint32_t *post_amp_ptr)
{
    uint32  unit;
    rtk_sds_eyeParam_t      eyeParam;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str;
    int32   ret;


    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&eyeParam, 0, sizeof(rtk_sds_eyeParam_t));
        ret = rtk_port_phySdsEyeParam_get(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
                DIAG_ERR_PRINT(ret);
            }
            continue;
        }

        eyeParam.post_amp = *post_amp_ptr;
        ret = rtk_port_phySdsEyeParam_set(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
            DIAG_ERR_PRINT(ret);
        }
    }/* DIAG_UTIL_PORTMASK_SCAN */
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_IMPEDANCE_IMPEDANCE
/*
 * port set phy-serdes eye-param port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> impedance <UINT:impedance>
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_eye_param_port_ports_all_serdes_id_sdsId_impedance_impedance(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr,
    uint32_t *impedance_ptr)
{
    uint32  unit;
    rtk_sds_eyeParam_t      eyeParam;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    uint32                  is_all = FALSE;
    char                    *port_str;
    int32   ret;


    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port_str = TOKEN_STR(5);
    if (!strncmp(port_str, "all", strlen(port_str)))
    {
        is_all = TRUE;
    }
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&eyeParam, 0, sizeof(rtk_sds_eyeParam_t));
        ret = rtk_port_phySdsEyeParam_get(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            if (is_all == FALSE)
            {
                diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
                DIAG_ERR_PRINT(ret);
            }
            continue;
        }

        eyeParam.impedance = *impedance_ptr;
        ret = rtk_port_phySdsEyeParam_set(unit, port, *sdsId_ptr, &eyeParam);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("PHY of port %u SERDES %u", port, *sdsId_ptr);
            DIAG_ERR_PRINT(ret);
        }
    }/* DIAG_UTIL_PORTMASK_SCAN */
    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_GET_PORT_PORTS_ALL_PHY_MDI_LOOPBACK
/*
 * port get port ( <PORT_LIST:ports> | all ) phy mdi-loopback
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_phy_mdi_loopback(cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyMdiLoopbackEnable_get(unit, port, &enable), ret);

        diag_util_mprintf("Port %d : \n", port);
        diag_util_mprintf("MDI-Loopback : %s\n", (ENABLED ==enable) ? "Enable" : "Disable");
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_SET_PORT_PORTS_ALL_PHY_MDI_LOOPBACK_ENABLE_DISABLE
/*
 * port set port ( <PORT_LIST:ports> | all ) phy mdi-loopback ( enable | disable )
 */
cparser_result_t
cparser_cmd_port_set_port_ports_all_phy_mdi_loopback_enable_disable(cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_port_phyMdiLoopbackEnable_set(unit, port, enable), ret) != RT_ERR_OK)
        {
            diag_util_mprintf("unit %d port %d: ", unit, port);
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_CONGEST_TIME_PORT_PORTS_ALL
/*
 * port get congest-time port ( <PORT_LIST:ports> | all ) */
cparser_result_t
cparser_cmd_port_get_congest_time_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret;
    uint32  time;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_port_specialCongest_get(unit, port, &time), ret) != RT_ERR_OK)
        {
            diag_util_mprintf("unit %d port %d: ", unit, port);
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %d : Congest time: %d \n", port, time);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_congest_time_port_ports_all */
#endif

#ifdef CMD_PORT_SET_CONGEST_TIME_PORT_PORTS_ALL_TIME_TIME
/*
 * port set congest-time port ( <PORT_LIST:ports> | all ) time <UINT:time> */
cparser_result_t
cparser_cmd_port_set_congest_time_port_ports_all_time_time(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *time_ptr)
{
    uint32  unit;
    int32   ret;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_port_specialCongest_set(unit, port, *time_ptr), ret) != RT_ERR_OK)
        {
            diag_util_mprintf("unit %d port %d: ", unit, port);
            DIAG_ERR_PRINT(ret);
        }

    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_congest_time_port_ports_all_time_time */
#endif

