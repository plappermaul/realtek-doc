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
#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
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
#include <math.h>

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
#ifndef PHY_ONLY
    rtk_enable_t              txStatus, rxStatus;
#endif
    rtk_port_crossOver_mode_t   mode;
    rtk_switch_devInfo_t    devInfo;
    int32                   prt_lite = 0;
    uint32                  anDone;
    char media_cmd[32]= "\tMedia";
    char admin_cmd[32] = "\tAdmin";
#ifndef PHY_ONLY
    char macTx_cmd[32] = "\tMac Tx";
    char macRx_cmd[32] = "\tMac Rx";
    char pressure_cmd[32] = "\tBack Pressure";
#endif
    char link_cmd[32] = "\tLink";
    char autoNego_cmd[32] = "\tAutoNego";
    char autoNegoAbility_cmd[32] = "\tAutoNego Ability";
    char liteAbility_cmd[32] = "\tAutoNego Lite-Ability";
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

#ifndef PHY_ONLY
        DIAG_UTIL_ERR_CHK(rtk_port_adminEnable_get(unit, port, &enabled), ret);
        diag_util_printf("%s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
#endif

#ifndef PHY_ONLY
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
#endif

        diag_util_printf("%s",link_cmd);
        cmd_print_blank(max_len,strlen(link_cmd));
#ifndef PHY_ONLY
        DIAG_UTIL_ERR_CHK(rtk_port_link_get(unit, port, &link_status), ret);
#else
        DIAG_UTIL_ERR_CHK(rtk_port_phyLinkStatus_get(unit, port, &link_status), ret);
#endif
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
                        diag_util_printf("1G Lite ");
                    else if (PORT_SPEED_1000M == speed)
                        diag_util_printf("1000F ");
                    else if (PORT_SPEED_2_5G == speed)
                        diag_util_printf("2.5G ");
                    else if (PORT_SPEED_5G == speed)
                        diag_util_printf("5G ");
                    else if (PORT_SPEED_10G == speed)
                        diag_util_printf("10G ");
                    else if (PORT_SPEED_2_5G_LITE == speed)
                        diag_util_printf("2.5G Lite ");
                    else if (PORT_SPEED_5G_LITE == speed)
                        diag_util_printf("5G Lite ");
                    else if (PORT_SPEED_10G_LITE == speed)
                        diag_util_printf("10G Lite ");
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


            prt_lite = rtk_port_phyLiteEnable_get(unit, port, PORT_LITE_1G, &enabled);
            if (RT_ERR_OK == prt_lite)
            {
                diag_util_printf("%s", liteAbility_cmd);
                cmd_print_blank(max_len,strlen(liteAbility_cmd));
                if (enabled == ENABLED)
                    diag_util_printf("1G-Lite ");
                ret = rtk_port_phyLiteEnable_get(unit, port, PORT_LITE_2P5G, &enabled);
                if (RT_ERR_OK == ret && enabled == ENABLED)
                    diag_util_printf("2.5G-Lite ");
                ret = rtk_port_phyLiteEnable_get(unit, port, PORT_LITE_5G, &enabled);
                if (RT_ERR_OK == ret && enabled == ENABLED)
                    diag_util_printf("5G-Lite ");
                ret = rtk_port_phyLiteEnable_get(unit, port, PORT_LITE_10G, &enabled);
                if (RT_ERR_OK == ret && enabled == ENABLED)
                    diag_util_printf("10G-Lite ");
                diag_util_mprintf("\n");
            }

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
                    else if (PORT_SPEED_2_5G == speed)
                        diag_util_printf("2.5G");
                    else if (PORT_SPEED_5G == speed)
                        diag_util_printf("5G ");
                    else if (PORT_SPEED_10G == speed)
                        diag_util_printf("10G");
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
            else
            {
                if (rtk_port_phyCtrl_get(unit, port, RTK_PHY_CTRL_AN_COMPLETE, &anDone) == RT_ERR_OK)
                {
                    diag_util_mprintf("\tAutoNego Complete       : %s\n", (anDone == 1)?("TRUE"):("FALSE"));
                }
            }
        }

        diag_util_printf("%s",gigaLite_cmd);
        cmd_print_blank(max_len,strlen(gigaLite_cmd));
        ret = rtk_port_gigaLiteEnable_get(unit, port, &enabled);
        if (ret != RT_ERR_OK)
            diag_util_mprintf("(%s)\n", rt_error_numToStr(ret));
        else
            diag_util_mprintf("%s\n", enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

#ifndef PHY_ONLY
        //DIAG_UTIL_ERR_CHK(rtk_port_flowctrl_get(unit, port, &txStatus, &rxStatus), ret);
        ret = rtk_port_flowctrl_get(unit, port, &txStatus, &rxStatus);
        //DIAG_ERR_PRINT(ret);
        if (ret == RT_ERR_OK)
        {
            diag_util_printf("%s",flowCtrl_cmd);
            cmd_print_blank(max_len,strlen(flowCtrl_cmd));
            diag_util_mprintf("%s\n", (txStatus && rxStatus) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }
#endif

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

/*
 * port get phy-mmd-reg port ( <PORT_LIST:ports> | all ) mmd-addr <UINT:mmd_addr> mmd-reg <UINT:mmd_reg> brief
 */
cparser_result_t
cparser_cmd_port_get_phy_mmd_reg_port_ports_all_mmd_addr_mmd_addr_mmd_reg_mmd_reg_brief(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *mmd_addr_ptr,
    uint32_t *mmd_reg_ptr)
{
    uint32      unit = 0;
    uint32      reg_data = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((mmd_addr_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((mmd_reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyMmdReg_get(unit, port, *mmd_addr_ptr, *mmd_reg_ptr, &reg_data), ret);
        diag_util_printf("0x%04X\n", reg_data);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_phy_mmd_reg_port_ports_all_mmd_addr_mmd_addr_mmd_reg_mmd_reg_brief */

/*
 * port get phy-mmd-reg port ( <PORT_LIST:ports> | all ) mmd-addr <UINT:mmd_addr> mmd-reg <UINT:mmd_reg> <UINT:msb> <UINT:lsb> { brief }
 */
cparser_result_t
cparser_cmd_port_get_phy_mmd_reg_port_ports_all_mmd_addr_mmd_addr_mmd_reg_mmd_reg_msb_lsb_brief(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *mmd_addr_ptr,
    uint32_t *mmd_reg_ptr,
    uint32_t *msb_ptr,
    uint32_t *lsb_ptr)
{
    uint32           unit = 0;
    uint32           reg_data = 0, brief = 0;
    uint32           mask = 0, i = 0;
    int32            ret = RT_ERR_FAILED;
    rtk_port_t       port = 0;
    diag_portlist_t  portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((mmd_addr_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((mmd_reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((msb_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((lsb_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((*msb_ptr > 15), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((*lsb_ptr > *msb_ptr), CPARSER_ERR_INVALID_PARAMS);

    if (TOKEN_NUM == 12) //brief
    {
        brief = 1;
    }

    for (i = *lsb_ptr; i <= *msb_ptr; i++)
    {
        mask |= (1 << i);
    }

    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyMmdReg_get(unit, port, *mmd_addr_ptr, *mmd_reg_ptr, &reg_data), ret);
        if (brief == 1)
        {
            diag_util_printf("0x%04X\n", ((reg_data & mask) >> *lsb_ptr));
        }
        else
        {
            diag_util_mprintf("\n");
            diag_util_mprintf("Port : %d\n", port);
            diag_util_mprintf("    MMD Addr %d : \n", *mmd_addr_ptr);
            diag_util_printf("        0x%02X[%u:%u]", *mmd_reg_ptr, *msb_ptr, *lsb_ptr);
            diag_util_printf("        0x%04X\n", ((reg_data & mask) >> *lsb_ptr));
        }
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

#ifdef CMD_PORT_GET_AUTO_NEGO_PORT_PORTS_ALL_LOCAL_ABILITY
/*
 * port get auto-nego port ( <PORT_LIST:ports> | all ) local-ability
 */
cparser_result_t cparser_cmd_port_get_auto_nego_port_ports_all_local_ability(cparser_context_t *context,
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

        diag_util_printf("\tLocal Ability      : ");
        DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoAbilityLocal_get(unit, port, &ability), ret);

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
    rtk_enable_t            enabled;


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

        ret = rtk_port_phyLiteEnable_get(unit, port, PORT_LITE_1G, &enabled);
        if (RT_ERR_OK == ret && enabled == ENABLED)
            diag_util_printf("1G-Lite ");
        ret = rtk_port_phyLiteEnable_get(unit, port, PORT_LITE_2P5G, &enabled);
        if (RT_ERR_OK == ret && enabled == ENABLED)
            diag_util_printf("2.5G-Lite ");
        ret = rtk_port_phyLiteEnable_get(unit, port, PORT_LITE_5G, &enabled);
        if (RT_ERR_OK == ret && enabled == ENABLED)
            diag_util_printf("5G-Lite ");
        ret = rtk_port_phyLiteEnable_get(unit, port, PORT_LITE_10G, &enabled);
        if (RT_ERR_OK == ret && enabled == ENABLED)
            diag_util_printf("10G-Lite ");

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
           else if (PORT_SPEED_2_5G == speed)
               diag_util_printf("2500F");
           else if (PORT_SPEED_5G == speed)
               diag_util_printf("5000F ");
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
 * port get cross-over port ( <PORT_LIST:ports> | all ) status
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
        if((ret != RT_ERR_OK) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        else if (ret == RT_ERR_PORT_NOT_SUPPORTED)
            diag_util_mprintf("%s\n", "No supported");
        else if (ret == RT_ERR_PHY_FIBER_LINKUP)
            diag_util_mprintf("%s\n", "Fiber is Link Up, Can not get cross over status");
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
    rtk_port_t              port = 0;
    rtk_port_phy_ability_t  ability;
    diag_portlist_t         portlist;
    int                     len;
    char                    *str;

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

#ifdef CMD_PORT_SET_AUTO_NEGO_PORT_PORTS_ALL_LITE_ABILITY
/*
 * port set auto-nego port ( <PORT_LIST:ports> | all ) lite-ability { 1g } { 2_5g } { 5g } { 10g }
 */
cparser_result_t cparser_cmd_port_set_auto_nego_port_ports_all_lite_ability_1g_2_5g_5g_10g(cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    int32                   option_num = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
    int                     len;
    char                    *str;
    char                    litemode[PORT_LITE_END] = {0};
    uint32                  i = 0;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    for (option_num = 6; option_num < TOKEN_NUM; option_num++)
    {
        str = TOKEN_STR(option_num);
        len = strlen(str);
        if (!strncmp("1g", str, len))
        {
            litemode[PORT_LITE_1G] = 1;
        }
        else if (!strncmp("2_5g", str, len))
        {
            litemode[PORT_LITE_2P5G] = 1;
        }
        else if (!strncmp("5g", str, len))
        {
            litemode[PORT_LITE_5G] = 1;
        }
        else if (!strncmp("10g", str, len))
        {
            litemode[PORT_LITE_10G] = 1;
        }
        else
        {
            diag_util_printf("User config: Error! [%s]\n", str);
            return CPARSER_NOT_OK;
        }
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        for (i = 0; i < PORT_LITE_END; i++)
        {
            if (litemode[i] == 0)
            {
                DIAG_UTIL_ERR_CHK(rtk_port_phyLiteEnable_set(unit, port, i, DISABLED), ret);
            }
            else
            {
                DIAG_UTIL_ERR_CHK(rtk_port_phyLiteEnable_set(unit, port, i, ENABLED), ret);
            }
        }
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
 * port set phy-force port ( <PORT_LIST:ports> | all ) ability ( 10h | 10f | 100h | 100f | 1000f | 2_5g | 5g | 10G )
 */
cparser_result_t cparser_cmd_port_set_phy_force_port_ports_all_ability_10h_10f_100h_100f_1000f_2_5g_5g_10000f(cparser_context_t *context,
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

    if (0 == osal_strcmp(TOKEN_STR(6), "10h"))
    {
        speed = PORT_SPEED_10M;
        duplex = PORT_HALF_DUPLEX;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "10f"))
    {
        speed = PORT_SPEED_10M;
        duplex = PORT_FULL_DUPLEX;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "100h"))
    {
        speed = PORT_SPEED_100M;
        duplex = PORT_HALF_DUPLEX;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "100f"))
    {
        speed = PORT_SPEED_100M;
        duplex = PORT_FULL_DUPLEX;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "1000f"))
    {
        speed = PORT_SPEED_1000M;
        duplex = PORT_FULL_DUPLEX;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "2_5g"))
    {
        speed = PORT_SPEED_2_5G;
        duplex = PORT_FULL_DUPLEX;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "5g"))
    {
        speed = PORT_SPEED_5G;
        duplex = PORT_FULL_DUPLEX;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "10000f"))
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

/*
 * port set phy-mmd-reg port ( <PORT_LIST:ports> | all ) mmd-addr <UINT:mmd_addr> mmd-reg <UINT:mmd_reg> <UINT:msb> <UINT:lsb> data <UINT:data>
 */
cparser_result_t
cparser_cmd_port_set_phy_mmd_reg_port_ports_all_mmd_addr_mmd_addr_mmd_reg_mmd_reg_msb_lsb_data_data(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *mmd_addr_ptr,
    uint32_t *mmd_reg_ptr,
    uint32_t *msb_ptr,
    uint32_t *lsb_ptr,
    uint32_t *data_ptr)
{
    uint32          unit = 0;
    uint32          reg_data = 0;
    uint32          mask = 0, i = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((mmd_addr_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((mmd_reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((data_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((msb_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((lsb_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((*msb_ptr > 15), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((*lsb_ptr > *msb_ptr), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    for (i = *lsb_ptr; i <= *msb_ptr; i++)
    {
        mask |= (1 << i);
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        /* set port phy register */
        if ((*msb_ptr != 15) || (*lsb_ptr != 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyMmdReg_get(unit, port, *mmd_addr_ptr, *mmd_reg_ptr, &reg_data), ret);
            reg_data = REG32_FIELD_SET(reg_data, *data_ptr, *lsb_ptr, mask);
            DIAG_UTIL_ERR_CHK(rtk_port_phyMmdReg_set(unit, port, *mmd_addr_ptr, *mmd_reg_ptr, reg_data), ret);

        }
        else
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyMmdReg_set(unit, port, *mmd_addr_ptr, *mmd_reg_ptr, reg_data), ret);
        }
    }

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


#ifdef CMD_PORT_GET_PORT_PORTS_ALL_DOWN_SPEED_STATUS
/*
 * port get port ( <PORT_LIST:ports> | all ) down-speed status
 */
cparser_result_t cparser_cmd_port_get_port_ports_all_down_speed_status(cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    uint32          status;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %d: ", port);
        DIAG_UTIL_ERR_CHK(rtk_port_downSpeedStatus_get(unit, port, &status), ret);

        if (TRUE == status)
            diag_util_mprintf("True\n");
        else
            diag_util_mprintf("False\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_port_ports_all_down_speed_status */
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

#ifdef CMD_PORT_SET_10G_MEDIA_PORT_PORTS_ALL_FIBER10G_FIBER1G_DAC50CM_DAC100CM_DAC300CM_DAC500CM_NONE
/*
 * port set 10g-media port ( <PORT_LIST:ports> | all ) ( fiber10g | fiber1g | dac50cm | dac100cm | dac300cm | dac500cm | none )
 */
cparser_result_t
cparser_cmd_port_set_10g_media_port_ports_all_fiber10g_fiber1g_dac50cm_dac100cm_dac300cm_dac500cm_none(
    cparser_context_t *context,
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
}   /* end of cparser_cmd_port_set_10g_media_port_ports_all_fiber10g_fiber1g_dac50cm_dac100cm_dac300cm_none */
#endif

#ifdef CMD_PORT_SET_10G_MEDIA_PORT_PORTS_ALL_FIBER100M_FIBER2_5G
/*
 * port set 10g-media port ( <PORT_LIST:ports> | all ) ( fiber100m | fiber2.5g )
 */
cparser_result_t
cparser_cmd_port_set_10g_media_port_ports_all_fiber100m_fiber2_5g(
    cparser_context_t *context,
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

    if('2' == TOKEN_CHAR(5, 5))
        media = PORT_10GMEDIA_FIBER_2_5G;
    else
        media = PORT_10GMEDIA_FIBER_100M;

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
}   /* end of cparser_cmd_port_set_10g_media_port_ports_all_fiber100m */
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
            case PORT_10GMEDIA_FIBER_100M:
                diag_util_mprintf("fiber 100M\n");
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
 * port set phy-test-mode port <UINT:port> 1g-mode ( 1 | 2 | 3 | 4 | none )
 */
cparser_result_t
cparser_cmd_port_set_phy_test_mode_port_port_1g_mode_1_2_3_4_none(cparser_context_t *context,
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
        testMode.mode = RTK_PORT_PHY_1G_TEST_MODE1;
    else if (!strncmp("2", mode_str, len))
        testMode.mode = RTK_PORT_PHY_1G_TEST_MODE2;
    else if (!strncmp("3", mode_str, len))
        testMode.mode = RTK_PORT_PHY_1G_TEST_MODE3;
    else if (!strncmp("4", mode_str, len))
        testMode.mode = RTK_PORT_PHY_1G_TEST_MODE4;
    else if (!strncmp("none", mode_str, len))
        testMode.mode = RTK_PORT_PHY_1G_TEST_MODE_NONE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_port_phyIeeeTestMode_set(unit, *port_ptr, &testMode), ret);

    return CPARSER_OK;
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


#ifdef CMD_PORT_SET_PHY_SERDES_TEST_MODE_PORT_PORTS_ALL_SERDES_ID_SDSID_PATTERN
/*
 * port set phy-serdes-test-mode port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> pattern ( disable | prbs7 | prbs9 | prbs10 | prbs11 | prbs15 | prbs20 | prbs23 | prbs31 | 8180 )
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_test_mode_port_ports_all_serdes_id_sdsId_pattern_disable_prbs7_prbs9_prbs10_prbs11_prbs15_prbs20_prbs23_prbs31_8180(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr)
{
    int32               ret = CPARSER_OK;
    uint32              unit = 0;
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    int                 len = 0;
    char                *ptrnStr = NULL;
    rtk_sds_testMode_t  testMode = RTK_SDS_TESTMODE_DISABLE;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ptrnStr = TOKEN_STR(8);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("disable", ptrnStr, len))
        testMode = RTK_SDS_TESTMODE_DISABLE;
    else if (!osal_strncmp("prbs7", ptrnStr, len))
        testMode = RTK_SDS_TESTMODE_PRBS7;
    else if (!osal_strncmp("prbs9", ptrnStr, len))
        testMode = RTK_SDS_TESTMODE_PRBS9;
    else if (!osal_strncmp("prbs10", ptrnStr, len))
        testMode = RTK_SDS_TESTMODE_PRBS10;
    else if (!osal_strncmp("prbs11", ptrnStr, len))
        testMode = RTK_SDS_TESTMODE_PRBS11;
    else if (!osal_strncmp("prbs15", ptrnStr, len))
        testMode = RTK_SDS_TESTMODE_PRBS15;
    else if (!osal_strncmp("prbs20", ptrnStr, len))
        testMode = RTK_SDS_TESTMODE_PRBS20;
    else if (!osal_strncmp("prbs23", ptrnStr, len))
        testMode = RTK_SDS_TESTMODE_PRBS23;
    else if (!osal_strncmp("prbs31", ptrnStr, len))
        testMode = RTK_SDS_TESTMODE_PRBS31;
    else if (!osal_strncmp("8180", ptrnStr, len))
        testMode = RTK_SDS_TESTMODE_SQUARE8;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_port_phySdsTestMode_set(unit, port, *sdsId_ptr, testMode), ret) != RT_ERR_OK)
        {
            diag_util_mprintf("unit %d port %d:", unit, port);
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_SERDES_TEST_MODE_PORT_PORTS_ALL_SERDES_ID_SDSID_CNT
/*
 * port get phy-serdes-test-mode port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> cnt
 */
cparser_result_t
cparser_cmd_port_get_phy_serdes_test_mode_port_ports_all_serdes_id_sdsId_cnt(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr)
{
    int32           ret = CPARSER_OK;
    uint32          unit = 0;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    uint32          cnt = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phySdsTestModeCnt_get(unit, port, *sdsId_ptr, &cnt), ret);

        diag_util_mprintf("Port %d : \n", port);
        diag_util_mprintf("Test pattern error counter: 0x%08x\n", cnt);
    }

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

#ifdef CMD_PORT_GET_PHY_EYE_MONITOR_INFO_PORT_SDSID_FRAMENUM
/*
 * port get phy-eye-monitor info <UINT:port> <UINT:sdsId> <UINT:frameNum>
 */
cparser_result_t
cparser_cmd_port_get_phy_eye_monitor_info_port_sdsId_frameNum(cparser_context_t *context,
    uint32_t *port_ptr,
    uint32_t *sdsId_ptr,
    uint32_t *frameNum_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_sds_eyeMonInfo_t    eyeInfo;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_port_phyEyeMonitorInfo_get(unit, *port_ptr, *sdsId_ptr, *frameNum_ptr, &eyeInfo), ret);

    diag_util_mprintf("max height: %u\n", eyeInfo.height);
    diag_util_mprintf("max width: %u\n", eyeInfo.width);

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

            diag_util_mprintf(" Pre-AMP State  : %s\n", (eyeParam.pre_en == ENABLED) ? DIAG_STR_ENABLE: DIAG_STR_DISABLE);
            diag_util_mprintf(" Pre-AMP        : %u\n", eyeParam.pre_amp);
            diag_util_mprintf(" Main-AMP State : %s\n", (eyeParam.main_en == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            diag_util_mprintf(" Main-AMP       : %u\n", eyeParam.main_amp);
            diag_util_mprintf(" Post1-AMP State: %s\n", (eyeParam.post_en == ENABLED) ? DIAG_STR_ENABLE: DIAG_STR_DISABLE);
            diag_util_mprintf(" Post1-AMP      : %u\n", eyeParam.post_amp);
            diag_util_mprintf(" Post2-AMP State: %s\n", (eyeParam.post2_en == ENABLED) ? DIAG_STR_ENABLE: DIAG_STR_DISABLE);
            diag_util_mprintf(" Post2-AMP      : %u\n", eyeParam.post2_amp);
            diag_util_mprintf(" Impedance      : %u\n", eyeParam.impedance);
        }
    } /* end DIAG_UTIL_PORTMASK_SCAN */

    return CPARSER_OK;
}

/*
 * port set phy-serdes eye-param port ( <PORT_LIST:ports> | all ) target-type ( default | 5gr | 5gx | 2p5gx | 1gx )
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_eye_param_port_ports_all_target_type_default_5gr_5gx_2p5gx_1gx(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE;

    if (0 == osal_strcmp(TOKEN_STR(7), "default"))
    {
        value = RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_DEFAULT;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "5gr"))
    {
        value = RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_5GR;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "5gx"))
    {
        value = RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_5GX;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "2p5gx"))
    {
        value = RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_2P5GX;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "1gx"))
    {
        value = RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_1GX;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}

/*
 * port get phy-serdes eye-param port ( <PORT_LIST:ports> | all ) target-type
 */
cparser_result_t
cparser_cmd_port_get_phy_serdes_eye_param_port_ports_all_target_type(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            switch (value)
            {
                case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_5GR:
                    diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d (5gr)\n", type, value);
                    break;
                case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_5GX:
                    diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d (5gx)\n", type, value);
                    break;
                case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_2P5GX:
                    diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d (2p5gx)\n", type, value);
                    break;
                case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_1GX:
                    diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d (1gx)\n", type, value);
                    break;
                case RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_DEFAULT:
                    diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d (default)\n", type, value);
                    break;
                default:
                    diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d (unknown)\n", type, value);
                    break;

            }
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_MAIN_STATE_DISABLE_ENABLE
/*
 * port set phy-serdes eye-param port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> main-state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_eye_param_port_ports_all_serdes_id_sdsId_main_state_disable_enable(cparser_context_t *context,
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

        eyeParam.main_en = en_cfg;
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

#ifdef CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_POST2_STATE_DISABLE_ENABLE
/*
 * port set phy-serdes eye-param port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> post2-state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_eye_param_port_ports_all_serdes_id_sdsId_post2_state_disable_enable(cparser_context_t *context,
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

        eyeParam.post2_en = en_cfg;
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

#ifdef CMD_PORT_SET_PHY_SERDES_EYE_PARAM_PORT_PORTS_ALL_SERDES_ID_SDSID_POST2_AMP_POST_AMP
/*
 * port set phy-serdes eye-param port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> post2-amp <UINT:post_amp>
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_eye_param_port_ports_all_serdes_id_sdsId_post2_amp_post_amp(cparser_context_t *context,
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

        eyeParam.post2_amp = *post_amp_ptr;
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


#ifdef CMD_PORT_SET_PHY_INTR_PORT_PORTS_ALL_INTR_INIT
/*
 * port set phy-intr port ( <PORT_LIST:ports> | all ) intr ( common | rlfd ) init
 */
cparser_result_t
cparser_cmd_port_set_phy_intr_port_ports_all_intr_common_rlfd_init(cparser_context_t *context,
    char **ports_ptr)
{
    int32           ret = CPARSER_OK;
    uint32          unit = 0;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    int             len = 0;
    char            *str = NULL;
    rtk_phy_intr_t  phyIntr = RTK_PHY_INTR_END;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* PHY interrupt type. */
    str = TOKEN_STR(6);
    len = osal_strlen(str);
    if (!osal_strncmp("common", str, len))
        phyIntr = RTK_PHY_INTR_COMMON;
    else if (!osal_strncmp("rlfd", str, len))
        phyIntr = RTK_PHY_INTR_RLFD;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_port_phyIntr_init(unit, port, phyIntr), ret) != RT_ERR_OK)
        {
            diag_util_mprintf("unit %d port %d: ", unit, port);
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_INTR_PORT_PORTS_ALL_INTR_STATE
/*
 * port get phy-intr port ( <PORT_LIST:ports> | all ) intr ( nway-next-page-recv | an-complete | link-change | aldps-state-change | rlfd | tm-low | tm-high | fatal-err | macsec) state
 */
cparser_result_t
cparser_cmd_port_get_phy_intr_port_ports_all_intr_nway_next_page_recv_an_complete_link_change_aldps_state_change_rlfd_tm_low_tm_high_fatal_err_macsec_state(cparser_context_t *context,
    char **ports_ptr)
{
    int32           ret = CPARSER_OK;
    uint32          unit = 0;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    int             len = 0;
    char            *str = NULL;
    rtk_phy_intr_status_t  phyIntr = RTK_PHY_INTR_STATUS_END;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* PHY interrupt type. */
    str = TOKEN_STR(6);
    len = osal_strlen(str);
    if (!osal_strncmp("nway-next-page-recv", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV;
    else if (!osal_strncmp("an-complete", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE;
    else if (!osal_strncmp("link-change", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE;
    else if (!osal_strncmp("aldps-state-change", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE;
    else if (!osal_strncmp("rlfd", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_RLFD;
    else if (!osal_strncmp("tm-low", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_TM_LOW;
    else if (!osal_strncmp("tm-high", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_TM_HIGH;
    else if (!osal_strncmp("fatal-err", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_FATAL_ERROR;
    else if (!osal_strncmp("macsec", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_MACSEC;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyIntrEnable_get(unit, port, phyIntr, &enable), ret);

        diag_util_mprintf("Port %d : \n", port);
        diag_util_mprintf("\tPHY-Intr State : %s\n", (ENABLED == enable) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_INTR_PORT_PORTS_ALL_INTR_STATE_DISABLE_ENABLE
/*
 * port set phy-intr port ( <PORT_LIST:ports> | all ) intr ( nway-next-page-recv | an-complete | link-change | aldps-state-change | rlfd | tm-low | tm-high | fatal-err | macsec ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_phy_intr_port_ports_all_intr_nway_next_page_recv_an_complete_link_change_aldps_state_change_rlfd_tm_low_tm_high_fatal_err_macsec_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    int32           ret = CPARSER_OK;
    uint32          unit = 0;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    int             len = 0;
    char            *str = NULL;
    rtk_phy_intr_status_t  phyIntr = RTK_PHY_INTR_STATUS_END;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* PHY interrupt type. */
    str = TOKEN_STR(6);
    len = osal_strlen(str);
    if (!osal_strncmp("nway-next-page-recv", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV;
    else if (!osal_strncmp("an-complete", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE;
    else if (!osal_strncmp("link-change", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE;
    else if (!osal_strncmp("aldps-state-change", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE;
    else if (!osal_strncmp("rlfd", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_RLFD;
    else if (!osal_strncmp("tm-low", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_TM_LOW;
    else if (!osal_strncmp("tm-high", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_TM_HIGH;
    else if (!osal_strncmp("fatal-err", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_FATAL_ERROR;
    else if (!osal_strncmp("macsec", str, len))
        phyIntr = RTK_PHY_INTR_STATUS_MACSEC;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if ('e' == TOKEN_CHAR(8, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_port_phyIntrEnable_set(unit, port, phyIntr, enable), ret) != RT_ERR_OK)
        {
            diag_util_mprintf("unit %d port %d: ", unit, port);
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_INTR_PORT_PORTS_ALL_INTR_STATUS
/*
 * port get phy-intr port ( <PORT_LIST:ports> | all ) intr ( common | rlfd ) status
 */
cparser_result_t
cparser_cmd_port_get_phy_intr_port_ports_all_intr_common_rlfd_status(cparser_context_t *context,
    char **ports_ptr)
{
    int32                   ret = CPARSER_OK;
    uint32                  unit = 0;
    diag_portlist_t         portlist;
    rtk_port_t              port = 0;
    int                     len = 0;
    char                    *str = NULL;
    rtk_phy_intr_t          phyIntr = RTK_PHY_INTR_END;
    rtk_phy_intrStatusVal_t    status;
    int32                   i;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* PHY interrupt type. */
    str = TOKEN_STR(6);
    len = osal_strlen(str);
    if (!osal_strncmp("common", str, len))
        phyIntr = RTK_PHY_INTR_COMMON;
    else if (!osal_strncmp("rlfd", str, len))
        phyIntr = RTK_PHY_INTR_RLFD;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyIntrStatus_get(unit, port, phyIntr, &status), ret);

        diag_util_mprintf("Port %d : \n", port);
        switch (phyIntr)
        {
            /*
             * Common types of interrupts, supporting following types,
             * a bit represents a interrupt type status of a port.
             */
            case RTK_PHY_INTR_COMMON:
                if (status.statusType == RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP_PORT_ARRAY)
                {
                    diag_util_mprintf("\tPHY-Intr Common Status array : (port 1st 2nd ...)\n");
                    diag_util_mprintf("\tNway Next Page Recv Status  : ");
                    for (i = 0; i < RTK_MAX_PORT_PER_UNIT; i++)
                        diag_util_mprintf("0x%01X ", (status.statusValueArray[i] & (0x1 << RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV)) ? (1) : (0));
                    diag_util_mprintf("\n");
                    diag_util_mprintf("\tAN Complete Status          : ");
                    for (i = 0; i < RTK_MAX_PORT_PER_UNIT; i++)
                        diag_util_mprintf("0x%01X ", (status.statusValueArray[i] & (0x1 << RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE)) ? (1) : (0));
                    diag_util_mprintf("\n");
                    diag_util_mprintf("\tLink Change Status          : ");
                    for (i = 0; i < RTK_MAX_PORT_PER_UNIT; i++)
                        diag_util_mprintf("0x%01X ", (status.statusValueArray[i] & (0x1 << RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE)) ? (1) : (0));
                    diag_util_mprintf("\n");
                    diag_util_mprintf("\tALDPS State Change Status   : ");
                    for (i = 0; i < RTK_MAX_PORT_PER_UNIT; i++)
                        diag_util_mprintf("0x%01X ", (status.statusValueArray[i] & (0x1 << RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE)) ? (1) : (0));
                    diag_util_mprintf("\n");
                    diag_util_mprintf("\tRLFD Status                 : ");
                    for (i = 0; i < RTK_MAX_PORT_PER_UNIT; i++)
                        diag_util_mprintf("0x%01X ", (status.statusValueArray[i] & (0x1 << RTK_PHY_INTR_STATUS_RLFD)) ? (1) : (0));
                    diag_util_mprintf("\n");
                    diag_util_mprintf("\tTemperature over level low  : ");
                    for (i = 0; i < RTK_MAX_PORT_PER_UNIT; i++)
                        diag_util_mprintf("0x%01X ", (status.statusValueArray[i] & (0x1 << RTK_PHY_INTR_STATUS_TM_LOW)) ? (1) : (0));
                    diag_util_mprintf("\n");
                    diag_util_mprintf("\tTemperature over level high : ");
                    for (i = 0; i < RTK_MAX_PORT_PER_UNIT; i++)
                        diag_util_mprintf("0x%01X ", (status.statusValueArray[i] & (0x1 << RTK_PHY_INTR_STATUS_TM_HIGH)) ? (1) : (0));
                    diag_util_mprintf("\n");
                    diag_util_mprintf("\tFatal Error : ");
                    for (i = 0; i < RTK_MAX_PORT_PER_UNIT; i++)
                        diag_util_mprintf("0x%01X ", (status.statusValueArray[i] & (0x1 << RTK_PHY_INTR_STATUS_FATAL_ERROR)) ? (1) : (0));
                    diag_util_mprintf("\n");
                    diag_util_mprintf("\tMACsec : ");
                    for (i = 0; i < RTK_MAX_PORT_PER_UNIT; i++)
                        diag_util_mprintf("0x%01X ", (status.statusValueArray[i] & (0x1 << RTK_PHY_INTR_STATUS_MACSEC)) ? (1) : (0));
                    diag_util_mprintf("\n");
                }
                else
                {
                    diag_util_mprintf("\tPHY-Intr Common Status : \n");
                    diag_util_mprintf("\tNway Next Page Recv Status  : 0x%01X\n", (status.statusValue & (0x1 << RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV)) ? (1) : (0));
                    diag_util_mprintf("\tAN Complete Status          : 0x%01X\n", (status.statusValue & (0x1 << RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE)) ? (1) : (0));
                    diag_util_mprintf("\tLink Change Status          : 0x%01X\n", (status.statusValue & (0x1 << RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE)) ? (1) : (0));
                    diag_util_mprintf("\tALDPS State Change Status   : 0x%01X\n", (status.statusValue & (0x1 << RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE)) ? (1) : (0));
                    diag_util_mprintf("\tRLFD Status                 : 0x%01X\n", (status.statusValue & (0x1 << RTK_PHY_INTR_STATUS_RLFD)) ? (1) : (0));
                    diag_util_mprintf("\tTemperature over level low  : 0x%01X\n", (status.statusValue & (0x1 << RTK_PHY_INTR_STATUS_TM_LOW)) ? (1) : (0));
                    diag_util_mprintf("\tTemperature over level high : 0x%01X\n", (status.statusValue & (0x1 << RTK_PHY_INTR_STATUS_TM_HIGH)) ? (1) : (0));
                    diag_util_mprintf("\tFatal Error                 : 0x%01X\n", (status.statusValue & (0x1 << RTK_PHY_INTR_STATUS_FATAL_ERROR)) ? (1) : (0));
                    diag_util_mprintf("\tMACsec                      : 0x%01X\n", (status.statusValue & (0x1 << RTK_PHY_INTR_STATUS_MACSEC)) ? (1) : (0));
                }
                break;
            case RTK_PHY_INTR_RLFD:
                if (status.statusType == RTK_PHY_INTR_STATUS_TYPE_PORT_BITMAP)
                    diag_util_mprintf("\tRLFD Status : 0x%01X\n", status.statusValue);
                else
                    diag_util_mprintf("\tRLFD Status : 0x%01X\n", (status.statusValue & (0x1 << RTK_PHY_INTR_STATUS_RLFD)) ? (1) : (0));
                break;
            default:
                return CPARSER_ERR_INVALID_PARAMS;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_INTR_PORT_PORTS_ALL_INTR_MASK
/*
 * port get phy-intr port ( <PORT_LIST:ports> | all ) intr ( common | rlfd | tm-low | tm-high ) mask
 */
cparser_result_t
cparser_cmd_port_get_phy_intr_port_ports_all_intr_common_rlfd_tm_low_tm_high_macsec_mask(cparser_context_t *context,
    char **ports_ptr)
{
    int32           ret = CPARSER_OK;
    uint32          unit = 0;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    int             len = 0;
    char            *str = NULL;
    rtk_phy_intr_t  phyIntr = RTK_PHY_INTR_END;
    uint32          mask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* PHY interrupt type. */
    str = TOKEN_STR(6);
    len = osal_strlen(str);
    if (!osal_strncmp("common", str, len))
        phyIntr = RTK_PHY_INTR_COMMON;
    else if (!osal_strncmp("rlfd", str, len))
        phyIntr = RTK_PHY_INTR_RLFD;
    else if (!osal_strncmp("tm-low", str, len))
        phyIntr = RTK_PHY_INTR_TM_LOW;
    else if (!osal_strncmp("tm-high", str, len))
        phyIntr = RTK_PHY_INTR_TM_HIGH;
    else if (!osal_strncmp("macsec", str, len))
        phyIntr = RTK_PHY_INTR_MACSEC;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyIntrMask_get(unit, port, phyIntr, &mask), ret);

        diag_util_mprintf("Port %d : \n", port);
        diag_util_mprintf("\tPHY-Intr Mask : 0x%02X\n", mask);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_INTR_PORT_PORTS_ALL_INTR_MASK_MASK
/*
 * port set phy-intr port ( <PORT_LIST:ports> | all ) intr ( common | rlfd | tm-low | tm-high | macsec) mask <UINT:value>
 */
cparser_result_t
cparser_cmd_port_set_phy_intr_port_ports_all_intr_common_rlfd_tm_low_tm_high_macsec_mask_value(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *value_ptr)
{
    int32           ret = CPARSER_OK;
    uint32          unit = 0;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    int             len = 0;
    char            *str = NULL;
    rtk_phy_intr_t  phyIntr = RTK_PHY_INTR_END;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    /* PHY interrupt type. */
    str = TOKEN_STR(6);
    len = osal_strlen(str);
    if (!osal_strncmp("common", str, len))
        phyIntr = RTK_PHY_INTR_COMMON;
    else if (!osal_strncmp("rlfd", str, len))
        phyIntr = RTK_PHY_INTR_RLFD;
    else if (!osal_strncmp("tm-low", str, len))
        phyIntr = RTK_PHY_INTR_TM_LOW;
    else if (!osal_strncmp("tm-high", str, len))
        phyIntr = RTK_PHY_INTR_TM_HIGH;
    else if (!osal_strncmp("macsec", str, len))
        phyIntr = RTK_PHY_INTR_MACSEC;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_port_phyIntrMask_set(unit, port, phyIntr, *value_ptr), ret) != RT_ERR_OK)
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

#ifdef CMD_PORT_SET_PHY_SERDES_LEQ_PORT_PORTS_ALL_SERDES_ID_SDSID_MANUAL_ENABLE_DISABLE_MANUAL_LEQ
/*
 * port get phy-serdes leq port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId>
 */
cparser_result_t
cparser_cmd_port_get_phy_serdes_leq_port_ports_all_serdes_id_sdsId(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t    enable;
    uint32          leq;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("unit %d port %d sdsId %u: ", unit, port, *sdsId_ptr);
        if ((ret = rtk_port_phySdsLeq_get(unit, port, *sdsId_ptr, &enable, &leq)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\n");
            diag_util_mprintf("  Manual LEQ: %s\n", (ENABLED == enable)? DIAG_STR_ENABLE:DIAG_STR_DISABLE);
            diag_util_mprintf("  LEQ value: %u\n", leq);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_SERDES_LEQ_PORT_PORTS_ALL_SERDES_ID_SDSID
/*
 * port set phy-serdes leq port ( <PORT_LIST:ports> | all ) serdes-id <UINT:sdsId> manual ( enable | disable ) { <UINT:manual_leq> }
 */
cparser_result_t
cparser_cmd_port_set_phy_serdes_leq_port_ports_all_serdes_id_sdsId_manual_enable_disable_manual_leq(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sdsId_ptr,
    uint32_t *manual_leq_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t    enable;
    uint32          leq;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(9, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    if (manual_leq_ptr == NULL)
        leq = 0;
    else
        leq = *manual_leq_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_port_phySdsLeq_set(unit, port, *sdsId_ptr, enable, leq)) != RT_ERR_OK)
        {
            diag_util_mprintf("unit %d port %d sdsId %u: ", unit, port, *sdsId_ptr);
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_MAC_FORCE_PORT_PORTS_ALL_FLOW_CONTROL_STATE_DISABLE_ENABLE_TX_PAUSE_DISABLE_ENABLE_RX_PAUSE_DISABLE_ENABLE
/*
 * port set mac-force port ( <PORT_LIST:ports> | all ) flow-control state ( disable | enable ) tx-pause ( disable | enable ) rx-pause ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_mac_force_port_ports_all_flow_control_state_disable_enable_tx_pause_disable_enable_rx_pause_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_enable_t        flowControl = DISABLED, txPause = DISABLED, rxPause = DISABLED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;

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

    if ('e' == TOKEN_CHAR(9, 0))
    {
        txPause = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(9, 0))
    {
        txPause = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('e' == TOKEN_CHAR(11, 0))
    {
        rxPause = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(11, 0))
    {
        rxPause = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_macForceFc_set(unit, port, flowControl, txPause, rxPause), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_mac_force_port_ports_all_flow_control_state_disable_enable_tx_pause_disable_enable_rx_pause_disable_enable */
#endif

#ifdef CMD_PORT_GET_MAC_FORCE_PORT_PORTS_ALL
/*
 * port get mac-force port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_mac_force_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32                  unit = 0;
    rtk_port_t              port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_enable_t            fcMode = DISABLED;
    rtk_enable_t            txPause = DISABLED;
    rtk_enable_t            rxPause = DISABLED;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
       diag_util_printf("Port %2u :\n", port);

        ret = rtk_port_macForceFc_get(unit, port, &fcMode, &txPause, &rxPause);
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("\tFlow Control (config) : %s\n", rt_error_numToStr(ret));
            diag_util_mprintf("\tTX Pause (config) : %s\n", rt_error_numToStr(ret));
            diag_util_mprintf("\tRX Pause (config) : %s\n", rt_error_numToStr(ret));
        }
        else
        {
            diag_util_mprintf("\tFlow Control (config) : %s\n", fcMode ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            diag_util_mprintf("\tTX Pause (config) : %s\n", txPause ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            diag_util_mprintf("\tRX Pause (config) : %s\n", rxPause ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

        }
    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phy_force_port_ports_all */
#endif

#ifdef CMD_PORT_GET_PHY_LITE_PORT_PORTS_ALL_STATE
/*
 * port get lite ( 1g | 2_5g | 5g | 10g ) port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_lite_1g_2_5g_5g_10g_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_port_lite_mode_t mode;
    rtk_enable_t         enabled;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if (0 == osal_strcmp(TOKEN_STR(3), "1g"))
    {
        mode = PORT_LITE_1G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "2_5g"))
    {
        mode = PORT_LITE_2P5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "5g"))
    {
        mode = PORT_LITE_5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "10g"))
    {
        mode = PORT_LITE_10G;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyLiteEnable_get(unit, port, mode, &enabled);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\t%s-Lite : %s\n", TOKEN_STR(3), enabled ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }

    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_LITE_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * port set lite  ( 1g | 2_5g | 5g | 10g ) port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_lite_1g_2_5g_5g_10g_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_port_lite_mode_t mode;
    rtk_enable_t         enable;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if (0 == osal_strcmp(TOKEN_STR(3), "1g"))
    {
        mode = PORT_LITE_1G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "2_5g"))
    {
        mode = PORT_LITE_2P5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "5g"))
    {
        mode = PORT_LITE_5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(3), "10g"))
    {
        mode = PORT_LITE_10G;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(7, 0))
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
        ret = rtk_port_phyLiteEnable_set(unit, port, mode, enable);
        DIAG_ERR_PRINT(ret);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL
/*
 * port get phyctrl <UINT:type> port ( <PORT_LIST:ports> | all ) value
 */
cparser_result_t cparser_cmd_port_get_phyctrl_type_port_ports_all_value(cparser_context_t *context,
    uint32_t *type_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = *type_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL
/*
 * port set phyctrl <UINT:type> port ( <PORT_LIST:ports> | all ) value <UINT:val>
 */
cparser_result_t cparser_cmd_port_set_phyctrl_type_port_ports_all_value_val(cparser_context_t *context,
    uint32_t *type_ptr,
    char **ports_ptr,
    uint32_t *val_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = *type_ptr;
    value = *val_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_MII_BROADCAST
/*
 * port get phyctrl mii-broadcast port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_phyctrl_mii_broadcast_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_MII_BROADCAST;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_MII_BROADCAST
/*
 * port set phyctrl mii-broadcast port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */

cparser_result_t cparser_cmd_port_set_phyctrl_mii_broadcast_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_MII_BROADCAST;
    value = ('e' == TOKEN_CHAR(7, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_MII_BROADCAST_PHYAD
/*
 * port get phyctrl mii-broadcast port ( <PORT_LIST:ports> | all ) phyad
 */
cparser_result_t cparser_cmd_port_get_phyctrl_mii_broadcast_port_ports_all_phyad(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_MII_BROADCAST_PHYAD;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_MII_BROADCAST_PHYAD
/*
 * port set phyctrl mii-broadcast port ( <PORT_LIST:ports> | all ) phyad <UINT:phyadress>
 */
cparser_result_t cparser_cmd_port_set_phyctrl_mii_broadcast_port_ports_all_phyad_phyadress(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *val_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_MII_BROADCAST_PHYAD;
    value = *val_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_NBASE_T
/*
 * port get phyctrl nbase-t port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_phyctrl_nbase_t_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_NBASET;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_NBASE_T
/*
 * port set phyctrl nbase-t port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */

cparser_result_t cparser_cmd_port_set_phyctrl_nbase_t_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_NBASET;
    value = ('e' == TOKEN_CHAR(7, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_NBASE_T_802P3BZ_MASK
/*
 * port get phyctrl nbase-t port ( <PORT_LIST:ports> | all ) mask_ieee
 */
cparser_result_t cparser_cmd_port_get_phyctrl_nbase_t_port_ports_all_mask_ieee(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_NBASET_802P3BZ_MASK;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_NBASE_T_802P3BZ_MASK
/*
 * port set phyctrl nbase-t port ( <PORT_LIST:ports> | all ) mask_ieee ( disable | enable )
 */

cparser_result_t cparser_cmd_port_set_phyctrl_nbase_t_port_ports_all_mask_ieee_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_NBASET_802P3BZ_MASK;
    value = ('e' == TOKEN_CHAR(7, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_NBASE_T_STATUS
/*
 * port get phyctrl nbase-t port ( <PORT_LIST:ports> | all ) status
 */
cparser_result_t cparser_cmd_port_get_phyctrl_nbase_t_port_ports_all_status(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_NBASET_STATUS;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_LOOPBACK_STATE
/*
 * port set phyctrl loopback ( pma | remote | serdes_remote ) port ( <PORT_LIST:ports> | all )  state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_phyctrl_loopback_pma_remote_serdes_remote_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    if (0 == osal_strcmp(TOKEN_STR(4), "pma"))
    {
        type = RTK_PHY_CTRL_LOOPBACK_INTERNAL_PMA;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "remote"))
    {
        type = RTK_PHY_CTRL_LOOPBACK_REMOTE;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "serdes_remote"))
    {
        type = RTK_PHY_CTRL_SERDES_LOOPBACK_REMOTE;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('e' == TOKEN_CHAR(8, 0))
    {
        value = 1;
    }
    else if ('d' == TOKEN_CHAR(8, 0))
    {
        value = 0;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_LOOPBACK_STATE
/*
 * port get phyctrl loopback port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_phyctrl_loopback_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);

        ret = rtk_port_phyCtrl_get(unit, port, RTK_PHY_CTRL_LOOPBACK_INTERNAL_PMA, &value);
        if ((ret == RT_ERR_CHIP_NOT_SUPPORTED) || (ret == RT_ERR_PORT_NOT_SUPPORTED))
        {
            diag_util_mprintf("\tPMA           : Not support\n");
        }
        else if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPMA           : %d\n", value);
        }

        ret = rtk_port_phyCtrl_get(unit, port, RTK_PHY_CTRL_LOOPBACK_REMOTE, &value);
        if ((ret == RT_ERR_CHIP_NOT_SUPPORTED) || (ret == RT_ERR_PORT_NOT_SUPPORTED))
        {
            diag_util_mprintf("\tremote        : Not support\n");
        }
        else if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tremote        : %d\n", value);
        }

        ret = rtk_port_phyCtrl_get(unit, port, RTK_PHY_CTRL_SERDES_LOOPBACK_REMOTE, &value);
        if ((ret == RT_ERR_CHIP_NOT_SUPPORTED) || (ret == RT_ERR_PORT_NOT_SUPPORTED))
        {
            diag_util_mprintf("\tserdes remote : Not support\n");
        }
        else if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tserdes remote : %d\n", value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_PREAMBLE_RECOVERY
/*
 * port get phyctrl preamble-recovery port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_phyctrl_preamble_recovery_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PREAMBLE_RECOVERY;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_PREAMBLE_RECOVERY
/*
 * port set phyctrl preamble-recovery port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */

cparser_result_t cparser_cmd_port_set_phyctrl_preamble_recovery_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PREAMBLE_RECOVERY;
    value = ('e' == TOKEN_CHAR(7, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_GET_PHY_CTRL_LED_MODE
/*
 * port get phyctrl led <UINT:ledid> port ( <PORT_LIST:ports> | all ) mode
 */
cparser_result_t  cparser_cmd_port_get_phyctrl_led_ledid_port_ports_all_mode(cparser_context_t *context,
    uint32_t *ledid_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               ledid;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr > 6), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr < 1), CPARSER_ERR_INVALID_PARAMS);

    ledid = *ledid_ptr;
    type = RTK_PHY_CTRL_LED_1_MODE + (ledid-1);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : 0x%X\n", type, value);
            diag_util_mprintf("\tGet LED %d Mode : \n", ledid);
            diag_util_mprintf("\t[ 0]10G             :%d\n", (value & RTK_PHY_CTRL_LED_MODE_10G             )? 1:0);
            diag_util_mprintf("\t[ 1]10G_LITE        :%d\n", (value & RTK_PHY_CTRL_LED_MODE_10G_LITE        )? 1:0);
            diag_util_mprintf("\t[ 2]5G              :%d\n", (value & RTK_PHY_CTRL_LED_MODE_5G              )? 1:0);
            diag_util_mprintf("\t[ 3]5G_LITE         :%d\n", (value & RTK_PHY_CTRL_LED_MODE_5G_LITE         )? 1:0);
            diag_util_mprintf("\t[ 4]2P5G            :%d\n", (value & RTK_PHY_CTRL_LED_MODE_2P5G            )? 1:0);
            diag_util_mprintf("\t[ 5]2P5G_LITE       :%d\n", (value & RTK_PHY_CTRL_LED_MODE_2P5G_LITE       )? 1:0);
            diag_util_mprintf("\t[ 6]1G              :%d\n", (value & RTK_PHY_CTRL_LED_MODE_1G              )? 1:0);
            diag_util_mprintf("\t[ 7]500M            :%d\n", (value & RTK_PHY_CTRL_LED_MODE_500M            )? 1:0);
            diag_util_mprintf("\t[ 8]100M            :%d\n", (value & RTK_PHY_CTRL_LED_MODE_100M            )? 1:0);
            diag_util_mprintf("\t[ 9]10G_FLASH       :%d\n", (value & RTK_PHY_CTRL_LED_MODE_10G_FLASH       )? 1:0);
            diag_util_mprintf("\t[10]10G_LITE_FLASH  :%d\n", (value & RTK_PHY_CTRL_LED_MODE_10G_LITE_FLASH  )? 1:0);
            diag_util_mprintf("\t[11]5G_FLASH        :%d\n", (value & RTK_PHY_CTRL_LED_MODE_5G_FLASH        )? 1:0);
            diag_util_mprintf("\t[12]5G_LITE_FLASH   :%d\n", (value & RTK_PHY_CTRL_LED_MODE_5G_LITE_FLASH   )? 1:0);
            diag_util_mprintf("\t[13]2P5G_FLASH      :%d\n", (value & RTK_PHY_CTRL_LED_MODE_2P5G_FLASH      )? 1:0);
            diag_util_mprintf("\t[14]2P5G_LITE_FLASH :%d\n", (value & RTK_PHY_CTRL_LED_MODE_2P5G_LITE_FLASH )? 1:0);
            diag_util_mprintf("\t[15]1G_FLASH        :%d\n", (value & RTK_PHY_CTRL_LED_MODE_1G_FLASH        )? 1:0);
            diag_util_mprintf("\t[16]500M_FLASH      :%d\n", (value & RTK_PHY_CTRL_LED_MODE_500M_FLASH      )? 1:0);
            diag_util_mprintf("\t[17]100M_FLASH      :%d\n", (value & RTK_PHY_CTRL_LED_MODE_100M_FLASH      )? 1:0);
            diag_util_mprintf("\t[18]RX_ACT          :%d\n", (value & RTK_PHY_CTRL_LED_MODE_RX_ACT          )? 1:0);
            diag_util_mprintf("\t[19]TX_ACT          :%d\n", (value & RTK_PHY_CTRL_LED_MODE_TX_ACT          )? 1:0);
            diag_util_mprintf("\t[20]LITE_FLASH      :%d\n", (value & RTK_PHY_CTRL_LED_MODE_LITE_FLASH      )? 1:0);
            diag_util_mprintf("\t[21]LITE            :%d\n", (value & RTK_PHY_CTRL_LED_MODE_LITE            )? 1:0);
            diag_util_mprintf("\t[22]DUPLEX          :%d\n", (value & RTK_PHY_CTRL_LED_MODE_DUPLEX          )? 1:0);
            diag_util_mprintf("\t[23]MASTER          :%d\n", (value & RTK_PHY_CTRL_LED_MODE_MASTER          )? 1:0);
            diag_util_mprintf("\t[24]TRAINING        :%d\n", (value & RTK_PHY_CTRL_LED_MODE_TRAINING        )? 1:0);
            diag_util_mprintf("\t[25]LINK_EN         :%d\n", (value & RTK_PHY_CTRL_LED_MODE_LINK_EN         )? 1:0);
        }
    }
    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_SET_PHY_CTRL_LED_MODE
/*
 * port set phyctrl led <UINT:ledid> port ( <PORT_LIST:ports> | all ) mode <UINT:modebitmap>
 */
cparser_result_t  cparser_cmd_port_set_phyctrl_led_ledid_port_ports_all_mode_modebitmap(cparser_context_t *context,
    uint32_t *ledid_ptr, char **ports_ptr, uint32_t *modebitmap_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               ledid;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr > 6), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr < 1), CPARSER_ERR_INVALID_PARAMS);

    ledid = *ledid_ptr;
    type = RTK_PHY_CTRL_LED_1_MODE + (ledid-1);
    value = *modebitmap_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : 0x%X\n", type, value);
            diag_util_mprintf("\tSet LED %d Mode : \n", ledid);
            diag_util_mprintf("\t[ 0]10G             :%d\n", (value & RTK_PHY_CTRL_LED_MODE_10G             )? 1:0);
            diag_util_mprintf("\t[ 1]10G_LITE        :%d\n", (value & RTK_PHY_CTRL_LED_MODE_10G_LITE        )? 1:0);
            diag_util_mprintf("\t[ 2]5G              :%d\n", (value & RTK_PHY_CTRL_LED_MODE_5G              )? 1:0);
            diag_util_mprintf("\t[ 3]5G_LITE         :%d\n", (value & RTK_PHY_CTRL_LED_MODE_5G_LITE         )? 1:0);
            diag_util_mprintf("\t[ 4]2P5G            :%d\n", (value & RTK_PHY_CTRL_LED_MODE_2P5G            )? 1:0);
            diag_util_mprintf("\t[ 5]2P5G_LITE       :%d\n", (value & RTK_PHY_CTRL_LED_MODE_2P5G_LITE       )? 1:0);
            diag_util_mprintf("\t[ 6]1G              :%d\n", (value & RTK_PHY_CTRL_LED_MODE_1G              )? 1:0);
            diag_util_mprintf("\t[ 7]500M            :%d\n", (value & RTK_PHY_CTRL_LED_MODE_500M            )? 1:0);
            diag_util_mprintf("\t[ 8]100M            :%d\n", (value & RTK_PHY_CTRL_LED_MODE_100M            )? 1:0);
            diag_util_mprintf("\t[ 9]10G_FLASH       :%d\n", (value & RTK_PHY_CTRL_LED_MODE_10G_FLASH       )? 1:0);
            diag_util_mprintf("\t[10]10G_LITE_FLASH  :%d\n", (value & RTK_PHY_CTRL_LED_MODE_10G_LITE_FLASH  )? 1:0);
            diag_util_mprintf("\t[11]5G_FLASH        :%d\n", (value & RTK_PHY_CTRL_LED_MODE_5G_FLASH        )? 1:0);
            diag_util_mprintf("\t[12]5G_LITE_FLASH   :%d\n", (value & RTK_PHY_CTRL_LED_MODE_5G_LITE_FLASH   )? 1:0);
            diag_util_mprintf("\t[13]2P5G_FLASH      :%d\n", (value & RTK_PHY_CTRL_LED_MODE_2P5G_FLASH      )? 1:0);
            diag_util_mprintf("\t[14]2P5G_LITE_FLASH :%d\n", (value & RTK_PHY_CTRL_LED_MODE_2P5G_LITE_FLASH )? 1:0);
            diag_util_mprintf("\t[15]1G_FLASH        :%d\n", (value & RTK_PHY_CTRL_LED_MODE_1G_FLASH        )? 1:0);
            diag_util_mprintf("\t[16]500M_FLASH      :%d\n", (value & RTK_PHY_CTRL_LED_MODE_500M_FLASH      )? 1:0);
            diag_util_mprintf("\t[17]100M_FLASH      :%d\n", (value & RTK_PHY_CTRL_LED_MODE_100M_FLASH      )? 1:0);
            diag_util_mprintf("\t[18]RX_ACT          :%d\n", (value & RTK_PHY_CTRL_LED_MODE_RX_ACT          )? 1:0);
            diag_util_mprintf("\t[19]TX_ACT          :%d\n", (value & RTK_PHY_CTRL_LED_MODE_TX_ACT          )? 1:0);
            diag_util_mprintf("\t[20]LITE_FLASH      :%d\n", (value & RTK_PHY_CTRL_LED_MODE_LITE_FLASH      )? 1:0);
            diag_util_mprintf("\t[21]LITE            :%d\n", (value & RTK_PHY_CTRL_LED_MODE_LITE            )? 1:0);
            diag_util_mprintf("\t[22]DUPLEX          :%d\n", (value & RTK_PHY_CTRL_LED_MODE_DUPLEX          )? 1:0);
            diag_util_mprintf("\t[23]MASTER          :%d\n", (value & RTK_PHY_CTRL_LED_MODE_MASTER          )? 1:0);
            diag_util_mprintf("\t[24]TRAINING        :%d\n", (value & RTK_PHY_CTRL_LED_MODE_TRAINING        )? 1:0);
            diag_util_mprintf("\t[25]LINK_EN         :%d\n", (value & RTK_PHY_CTRL_LED_MODE_LINK_EN         )? 1:0);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_LED_ACTIVE_LOW
/*
 * port get phyctrl led <UINT:ledid> port ( <PORT_LIST:ports> | all ) active-low
 */
cparser_result_t  cparser_cmd_port_get_phyctrl_led_ledid_port_ports_all_active_low(cparser_context_t *context,
    uint32_t *ledid_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               ledid;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr > 6), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr < 1), CPARSER_ERR_INVALID_PARAMS);

    ledid = *ledid_ptr;
    type = RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW + (ledid-1);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif


#ifdef CMD_PORT_SET_PHY_CTRL_LED_ACTIVE_LOW
/*
 * port set phyctrl led <UINT:ledid> port ( <PORT_LIST:ports> | all ) active-low ( disable | enable )
 */
cparser_result_t  cparser_cmd_port_set_phyctrl_led_ledid_port_ports_all_active_low_disable_enable(cparser_context_t *context,
    uint32_t *ledid_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               ledid;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr > 6), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr < 1), CPARSER_ERR_INVALID_PARAMS);

    ledid = *ledid_ptr;
    type = RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW + (ledid-1);
    value = ('e' == TOKEN_CHAR(8, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_LED_FORCE
/*
 * port get phyctrl led <UINT:ledid> port ( <PORT_LIST:ports> | all ) force
 */
cparser_result_t  cparser_cmd_port_get_phyctrl_led_ledid_port_ports_all_force(cparser_context_t *context,
    uint32_t *ledid_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               ledid;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr > 6), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr < 1), CPARSER_ERR_INVALID_PARAMS);

    ledid = *ledid_ptr;
    type = RTK_PHY_CTRL_LED_1_CFG_FORCE + (ledid-1);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d", type, value);
            switch(value)
            {
                case RTK_PHY_CTRL_LED_CFG_FORCE_DISABLE:
                    diag_util_mprintf("(disable force mode)");
                    break;
                case  RTK_PHY_CTRL_LED_CFG_FORCE_ON:
                    diag_util_mprintf("(force on)");
                    break;
                case  RTK_PHY_CTRL_LED_CFG_FORCE_BLINK:
                    diag_util_mprintf("(force blink)");
                    break;
                case  RTK_PHY_CTRL_LED_CFG_FORCE_FLASH:
                    diag_util_mprintf("(force flash)");
                    break;
                case  RTK_PHY_CTRL_LED_CFG_FORCE_OFF:
                    diag_util_mprintf("(force off)");
                    break;
                default:
                    diag_util_mprintf("(unknown)");
                    break;
            }
            diag_util_mprintf("\n");
        }
    }
    return CPARSER_OK;
}

#endif


#ifdef CMD_PORT_SET_PHY_CTRL_LED_FORCE
/*
 * port set phyctrl led <UINT:ledid> port ( <PORT_LIST:ports> | all ) force ( disable | off | on | blink | flash )
 */
cparser_result_t  cparser_cmd_port_set_phyctrl_led_ledid_port_ports_all_force_disable_off_on_blink_flash(cparser_context_t *context,
    uint32_t *ledid_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               ledid;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr > 6), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*ledid_ptr < 1), CPARSER_ERR_INVALID_PARAMS);

    if (0 == osal_strcmp(TOKEN_STR(8), "disable"))
    {
        value = RTK_PHY_CTRL_LED_CFG_FORCE_DISABLE;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "off"))
    {
        value = RTK_PHY_CTRL_LED_CFG_FORCE_OFF;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "on"))
    {
        value = RTK_PHY_CTRL_LED_CFG_FORCE_ON;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "blink"))
    {
        value = RTK_PHY_CTRL_LED_CFG_FORCE_BLINK;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "flash"))
    {
        value = RTK_PHY_CTRL_LED_CFG_FORCE_FLASH;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    ledid = *ledid_ptr;
    type = RTK_PHY_CTRL_LED_1_CFG_FORCE + (ledid-1);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_LED_FLASH_RATE
/*
 * port get phyctrl led port ( <PORT_LIST:ports> | all ) flash-rate
 */
cparser_result_t cparser_cmd_port_get_phyctrl_led_port_ports_all_flash_rate(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_LED_CFG_FLASH_RATE;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d", type, value);
            switch(value)
            {
                case RTK_PHY_CTRL_LED_CFG_FLASH_RATE_128MS:
                    diag_util_mprintf("(128 ms)");
                    break;
                case  RTK_PHY_CTRL_LED_CFG_FLASH_RATE_256MS:
                    diag_util_mprintf("(256 ms)");
                    break;
                case  RTK_PHY_CTRL_LED_CFG_FLASH_RATE_512MS:
                    diag_util_mprintf("(512 ms)");
                    break;
                case  RTK_PHY_CTRL_LED_CFG_FLASH_RATE_1024MS:
                    diag_util_mprintf("(1024 ms)");
                    break;
                default:
                    diag_util_mprintf("(unknown)");
                    break;
            }
            diag_util_mprintf("\n");
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_LED_FLASH_RATE
/*
 * port set phyctrl led port ( <PORT_LIST:ports> | all ) flash-rate ( 128ms | 256ms | 512ms | 1024ms )
 */
cparser_result_t  cparser_cmd_port_set_phyctrl_led_port_ports_all_flash_rate_128ms_256ms_512ms_1024ms(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if (0 == osal_strcmp(TOKEN_STR(7), "128ms"))
    {
        value = RTK_PHY_CTRL_LED_CFG_FLASH_RATE_128MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "256ms"))
    {
        value = RTK_PHY_CTRL_LED_CFG_FLASH_RATE_256MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "512ms"))
    {
        value = RTK_PHY_CTRL_LED_CFG_FLASH_RATE_512MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "1024ms"))
    {
        value = RTK_PHY_CTRL_LED_CFG_FLASH_RATE_1024MS;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    type = RTK_PHY_CTRL_LED_CFG_FLASH_RATE;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_LED_BLINK_RATE
/*
 * port get phyctrl led port ( <PORT_LIST:ports> | all ) blink-rate
 */
cparser_result_t cparser_cmd_port_get_phyctrl_led_port_ports_all_blink_rate(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_LED_CFG_BLINK_RATE;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d", type, value);
            switch(value)
            {
                case RTK_PHY_CTRL_LED_CFG_BLINK_RATE_32MS:
                    diag_util_mprintf("(32 ms)");
                    break;
                case  RTK_PHY_CTRL_LED_CFG_BLINK_RATE_64MS:
                    diag_util_mprintf("(64 ms)");
                    break;
                case  RTK_PHY_CTRL_LED_CFG_BLINK_RATE_128MS:
                    diag_util_mprintf("(128 ms)");
                    break;
                case  RTK_PHY_CTRL_LED_CFG_BLINK_RATE_256MS:
                    diag_util_mprintf("(256 ms)");
                    break;
                default:
                    diag_util_mprintf("(unknown)");
                    break;
            }
            diag_util_mprintf("\n");
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_LED_BLINK_RATE
/*
 * port set phyctrl led port ( <PORT_LIST:ports> | all ) blink-rate ( 32ms | 64ms | 128ms | 256ms )
 */
cparser_result_t  cparser_cmd_port_set_phyctrl_led_port_ports_all_blink_rate_32ms_64ms_128ms_256ms(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if (0 == osal_strcmp(TOKEN_STR(7), "32ms"))
    {
        value = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_32MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "64ms"))
    {
        value = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_64MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "128ms"))
    {
        value = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_128MS;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "256ms"))
    {
        value = RTK_PHY_CTRL_LED_CFG_BLINK_RATE_256MS;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    type = RTK_PHY_CTRL_LED_CFG_BLINK_RATE;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_RLFD
/*
 * port get phyctrl rlfd port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_phyctrl_rlfd_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_RAPID_LINK_FAULT_DETECT;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_RLFD
/*
 * port set phyctrl rlfd port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */

cparser_result_t cparser_cmd_port_set_phyctrl_rlfd_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_RAPID_LINK_FAULT_DETECT;
    value = ('e' == TOKEN_CHAR(7, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_SYNCE
/*
 *port set phyctrl synce port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_phyctrl_synce_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_SYNCE;
    value = ('e' == TOKEN_CHAR(7, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}

/*
 *port set phyctrl synce port ( <PORT_LIST:ports> | all ) source_pll ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_phyctrl_synce_port_ports_all_source_pll_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_SYNCE_PLL;
    value = ('e' == TOKEN_CHAR(7, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_SYNCE
/*
 *port get phyctrl synce port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_phyctrl_synce_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_SYNCE;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_SYNCE_CLOCK_FREQ
/*
 *port set phyctrl synce <UINT:synceid> port ( <PORT_LIST:ports> | all ) clock-freq ( 50mhz | 25mhz | 8khz )
 */
cparser_result_t cparser_cmd_port_set_phyctrl_synce_synceid_port_ports_all_clock_freq_50mhz_25mhz_8khz(cparser_context_t *context,
    uint32_t *id_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               id;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*id_ptr > 2), CPARSER_ERR_INVALID_PARAMS);

    id = *id_ptr;
    type = RTK_PHY_CTRL_SYNCE_0_CLOCK_FREQ + id;

    if (0 == osal_strcmp(TOKEN_STR(8), "50mhz"))
    {
        value = RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_50MHZ;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "25mhz"))
    {
        value = RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_25MHZ;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "8khz"))
    {
        value = RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_8KHZ;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_SYNCE_CLOCK_FREQ
/*
 *port get phyctrl synce <UINT:synceid> port ( <PORT_LIST:ports> | all ) clock-freq
 */
cparser_result_t cparser_cmd_port_get_phyctrl_synce_synceid_port_ports_all_clock_freq(cparser_context_t *context,
    uint32_t *id_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               id;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*id_ptr > 2), CPARSER_ERR_INVALID_PARAMS);

    id = *id_ptr;
    type = RTK_PHY_CTRL_SYNCE_0_CLOCK_FREQ + id;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d", type, value);
            switch(value)
            {
                case RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_50MHZ:
                    diag_util_mprintf("(50mHz)");
                    break;
                case  RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_25MHZ:
                    diag_util_mprintf("(25mHz)");
                    break;
                case  RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_8KHZ:
                    diag_util_mprintf("(8kmHz)");
                    break;
                default:
                    diag_util_mprintf("(unknown)");
                    break;
            }
            diag_util_mprintf("\n");
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_SYNCE_RECOVERY_PHY
/*
 *port set phyctrl synce <UINT:synceid> port ( <PORT_LIST:ports> | all ) recover-phy ( 0 | 1 | 2 | 3 )
 */
cparser_result_t cparser_cmd_port_set_phyctrl_synce_synceid_port_ports_all_recover_phy_0_1_2_3(cparser_context_t *context,
    uint32_t *id_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               id;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*id_ptr > 2), CPARSER_ERR_INVALID_PARAMS);

    id = *id_ptr;
    type = RTK_PHY_CTRL_SYNCE_0_RECOVERY_PHY + id;

    if (0 == osal_strcmp(TOKEN_STR(8), "0"))
    {
        value = 0;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "1"))
    {
        value = 1;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "2"))
    {
        value = 2;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "3"))
    {
        value = 3;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_SYNCE_RECOVERY_PHY
/*
 *port get phyctrl synce <UINT:synceid> port ( <PORT_LIST:ports> | all ) recover-phy
 */
cparser_result_t cparser_cmd_port_get_phyctrl_synce_synceid_port_ports_all_recover_phy(cparser_context_t *context,
    uint32_t *id_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               id;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*id_ptr > 2), CPARSER_ERR_INVALID_PARAMS);

    id = *id_ptr;
    type = RTK_PHY_CTRL_SYNCE_0_RECOVERY_PHY + id;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_SYNCE_IDLE_MODE
/*
 *port set phyctrl synce <UINT:synceid> port ( <PORT_LIST:ports> | all ) idle-mode ( local-free-run | high | low )
 */
cparser_result_t cparser_cmd_port_set_phyctrl_synce_synceid_port_ports_all_idle_mode_local_free_run_high_low(cparser_context_t *context,
    uint32_t *id_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               id;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*id_ptr > 2), CPARSER_ERR_INVALID_PARAMS);

    id = *id_ptr;
    type = RTK_PHY_CTRL_SYNCE_0_IDLE_MODE + id;

    if (0 == osal_strcmp(TOKEN_STR(8), "local-free-run"))
    {
        value = RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOCAL_FREERUN;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "high"))
    {
        value = RTK_PHY_CTRL_SYNCE_IDLE_MODE_HIGH;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "low"))
    {
        value = RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOW;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_SYNCE_IDLE_MODE
/*
 *port get phyctrl synce <UINT:synceid> port ( <PORT_LIST:ports> | all ) idle-mode
 */
cparser_result_t cparser_cmd_port_get_phyctrl_synce_synceid_port_ports_all_idle_mode(cparser_context_t *context,
    uint32_t *id_ptr,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               id;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*id_ptr > 2), CPARSER_ERR_INVALID_PARAMS);

    id = *id_ptr;
    type = RTK_PHY_CTRL_SYNCE_0_IDLE_MODE + id;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d", type, value);
            switch(value)
            {
                case RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOCAL_FREERUN:
                    diag_util_mprintf("(local-free-run)");
                    break;
                case  RTK_PHY_CTRL_SYNCE_IDLE_MODE_HIGH:
                    diag_util_mprintf("(high)");
                    break;
                case  RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOW:
                    diag_util_mprintf("(low)");
                    break;
                default:
                    diag_util_mprintf("(unknown)");
                    break;
            }
            diag_util_mprintf("\n");
        }
    }
    return CPARSER_OK;
}
#endif

void
_parse_temp(uint32 data, double *val)
{
    double temp = 0;
    if (data & (1 << 18))
    {
        temp = (524288 - (double)data) / 1024;
        temp = temp * (-1);
    }
    else
    {
        temp = (double)data / 1024;
    }

    *val = temp;
}

#ifdef CMD_PORT_GET_PHY_CTRL_TEMP
/*
 * port get phyctrl temperature port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_get_phyctrl_temperature_port_ports_all(cparser_context_t *context,
                                                                         char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;
    double               temp = 0;
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_TEMP;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        _parse_temp(value, &temp);

        diag_util_mprintf("Port %2d :\n", port);
        diag_util_mprintf("\tPhyctrl Type(%d) - Value : 0x%X(%lf ¢J) \n", type, value, temp);
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_TEMP_TH

/*
 * port get phyctrl temperature port ( <PORT_LIST:ports> | all ) threshold
 */
cparser_result_t cparser_cmd_port_get_phyctrl_temperature_port_ports_all_threshold(cparser_context_t *context,
                                                                                   char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;
    double               temp = 0;
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        type = RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER;
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        _parse_temp(value, &temp);
        diag_util_mprintf("\tPhyctrl Type(%d) - high-higher threshold : 0x%X(%lf ¢J) \n", type, value, temp);

        type = RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_LOWER;
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        _parse_temp(value, &temp);
        diag_util_mprintf("\tPhyctrl Type(%d) - high-lower threshold  : 0x%X(%lf ¢J) \n", type, value, temp);

        type = RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_HIGHER;
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        _parse_temp(value, &temp);
        diag_util_mprintf("\tPhyctrl Type(%d) - low-higher threshold  : 0x%X(%lf ¢J) \n", type, value, temp);

        type = RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER;
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        _parse_temp(value, &temp);
        diag_util_mprintf("\tPhyctrl Type(%d) - low-lower threshold   : 0x%X(%lf ¢J) \n", type, value, temp);

    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_TEMP_TH
/*
 * port set phyctrl temperature port ( <PORT_LIST:ports> | all ) threshold ( high | low ) ( higher | lower ) <INT:degree>
 */
cparser_result_t cparser_cmd_port_set_phyctrl_temperature_port_ports_all_threshold_high_low_higher_lower_degree(cparser_context_t *context,
    char **ports_ptr, int *temp)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value = 0;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*temp > 255), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*temp < -254), CPARSER_ERR_INVALID_PARAMS);

    if (0 == osal_strcmp(TOKEN_STR(7), "high"))
    {
        if (0 == osal_strcmp(TOKEN_STR(8), "higher"))
            type = RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER;
        else
            type = RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_LOWER;
    }
    else
    {
        if (0 == osal_strcmp(TOKEN_STR(8), "higher"))
            type = RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_HIGHER;
        else
            type = RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER;
    }

    if ( *temp < 0)
    {

        value = 524288 - (abs(*temp) * 1024);
    }
    else
    {
        value = (*temp * 1024);
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}

#endif


#ifdef CMD_PORT_GET_PHYCTRL_AM_PERIOD_SDS
/*
 * port get phyctrl serdes port ( <PORT_LIST:ports> | all ) am-period usxgmii
 */
cparser_result_t cparser_cmd_port_get_phyctrl_serdes_port_ports_all_am_period_usxgmii(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        if('u' == TOKEN_CHAR(7, 0))
            type = RTK_PHY_CTRL_SERDES_USXGMII_AM_PERIOD;
        else
            DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);

        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("\tAM Period : 0x%X\n", value);
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHYCTRL_AM_PERIOD_SDS
/*
 * port set phyctrl serdes port ( <PORT_LIST:ports> | all ) am-period usxgmii <UINT:value>
 */
cparser_result_t cparser_cmd_port_set_phyctrl_serdes_port_ports_all_am_period_usxgmii_value(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *value_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value = 0;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    value = *value_ptr;
    if('u' == TOKEN_CHAR(7, 0))
        type = RTK_PHY_CTRL_SERDES_USXGMII_AM_PERIOD;
    else
        DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}

#endif
#ifdef CMD_PORT_GET_PHY_COUNTER
/*
 * port get port ( <PORT_LIST:ports> | all ) phy-cnt
 */
cparser_result_t cparser_cmd_port_get_port_ports_all_phy_cnt(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;

    uint64               cnt;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        if ((ret = rtk_port_phyDbgCounter_get(unit, port, PHY_DBG_CNT_RX, &cnt))!= RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("\tRX            : %llu\n", cnt);
        if ((ret = rtk_port_phyDbgCounter_get(unit, port, PHY_DBG_CNT_RX_ERR, &cnt))!= RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("\tRX Error      : %llu\n", cnt);
        if ((ret = rtk_port_phyDbgCounter_get(unit, port, PHY_DBG_CNT_RX_CRCERR, &cnt))!= RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("\tRX CRC Error  : %llu\n", cnt);
        if ((ret = rtk_port_phyDbgCounter_get(unit, port, PHY_DBG_CNT_LDPC_ERR, &cnt))!= RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("\tLDPC Error    : %llu\n", cnt);
    }
    return CPARSER_OK;
}

#ifdef CMD_PORT_GET_PHY_COUNTER_CLEAR
/*
 * port set port ( <PORT_LIST:ports> | all ) phy-cnt clear
 */
cparser_result_t cparser_cmd_port_set_port_ports_all_phy_cnt_clear(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    type = RTK_PHY_CTRL_COUNTER_CLEAR;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, 1);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif
#endif



#ifdef CMD_PORT_GET_PHY_FAST_RETRAIN
/*
 * port get phyctrl fast-retrain port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_phyctrl_fast_retrain_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_FAST_RETRAIN;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - State : %s\n", type,
                              (value == RTK_PHY_CTRL_FAST_RETRAIN_ENABLE) ? ("Enable") : ("Disable"));
        }
    }
    return CPARSER_OK;
}

/*
 * port get phyctrl fast-retrain port ( <PORT_LIST:ports> | all ) nfr_state
 */
cparser_result_t cparser_cmd_port_get_phyctrl_fast_retrain_port_ports_all_nfr_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_FAST_RETRAIN_NFR;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, RTK_PHY_CTRL_FAST_RETRAIN_NFR, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - State : %s\n", type,
                              (value == RTK_PHY_CTRL_FAST_RETRAIN_NFR_ENABLE) ? ("Enable") : ("Disable"));
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_FAST_RETRAIN
/*
 * port set phyctrl fast-retrain port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */

cparser_result_t cparser_cmd_port_set_phyctrl_fast_retrain_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_FAST_RETRAIN;

    if (0 == osal_strcmp(TOKEN_STR(7), "enable"))
    {
        value = RTK_PHY_CTRL_FAST_RETRAIN_ENABLE;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "disable"))
    {
        value = RTK_PHY_CTRL_FAST_RETRAIN_DISABLE;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}

/*
 * port set phyctrl fast-retrain port ( <PORT_LIST:ports> | all ) nfr_state ( disable | enable )
 */

cparser_result_t cparser_cmd_port_set_phyctrl_fast_retrain_port_ports_all_nfr_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_FAST_RETRAIN_NFR;

    if (0 == osal_strcmp(TOKEN_STR(7), "enable"))
    {
        value = RTK_PHY_CTRL_FAST_RETRAIN_NFR_ENABLE;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "disable"))
    {
        value = RTK_PHY_CTRL_FAST_RETRAIN_NFR_DISABLE;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_FAST_RETRAIN_STATUS
/*
 * port get phyctrl fast-retrain port ( <PORT_LIST:ports> | all ) status
 */
cparser_result_t cparser_cmd_port_get_phyctrl_fast_retrain_port_ports_all_status(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);

        type = RTK_PHY_CTRL_FAST_RETRAIN_STATUS;
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("\tPhyctrl Type(%d) - IEEE FR  : %d\n", type, value);

        type = RTK_PHY_CTRL_FAST_RETRAIN_NFR_STATUS;
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("\tPhyctrl Type(%d) - NFR      : %d\n", type, value);

        type = RTK_PHY_CTRL_FAST_RETRAIN_LPCOUNT;
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("\tPhyctrl Type(%d) - LP count : %d\n", type, value);

        type = RTK_PHY_CTRL_FAST_RETRAIN_LDCOUNT;
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("\tPhyctrl Type(%d) - LD count : %d\n", type, value);

    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_PHY_CTRL_SERDES

/*
 * port get phyctrl serdes port ( <PORT_LIST:ports> | all ) mode
 */
cparser_result_t cparser_cmd_port_get_phyctrl_serdes_port_ports_all_mode(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_SERDES_MODE;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d (0x%04X)\n", type, value, value);
            switch (value)
            {
                case RTK_PHY_CTRL_SERDES_MODE_USXGMII:
                    diag_util_mprintf("\tSerDes Mode : USXGMII\n");
                    break;
                case RTK_PHY_CTRL_SERDES_MODE_10GR_5GX_2P5GX_SGMII:
                    diag_util_mprintf("\tSerDes Mode : MC Mode(10G-R/5G-X/2.5G-X/SGMII)\n");
                    break;
                case RTK_PHY_CTRL_SERDES_MODE_10GR_XFI5GADAPT_XFI2P5GADAPT_SGMII:
                    diag_util_mprintf("\tSerDes Mode : MC Mode(10G-R/XFI-5G-ADAPT/XFI-2.5G-ADAPT/SGMII)\n");
                    break;
                case RTK_PHY_CTRL_SERDES_MODE_10GR_5GR_2P5GX_SGMII:
                    diag_util_mprintf("\tSerDes Mode : MC Mode(10G-R/5G-R/2.5G-X/SGMII)\n");
                    break;
                case RTK_PHY_CTRL_SERDES_MODE_10GR_XFI5GCPRI_2P5GX_SGMII:
                    diag_util_mprintf("\tSerDes Mode : MC Mode(10G-R/XFI-5G-CPRI/2.5G-X/SGMII)\n");
                    break;
                case RTK_PHY_CTRL_SERDES_MODE_OFF:
                    diag_util_mprintf("\tSerDes Mode : Power Off\n");
                    break;
                default:
                    diag_util_mprintf("\tSerDes Mode : Unknown\n");
                    break;
            }
        }
    }
    return CPARSER_OK;
}

/*
 * port set phyctrl serdes port ( <PORT_LIST:ports> | all ) mode <UINT:modeVal>
 */
cparser_result_t cparser_cmd_port_set_phyctrl_serdes_port_ports_all_mode_modeVal(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *modeVal_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_SERDES_MODE;
    value = *modeVal_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : 0x%04X\n", type, value);

            switch (value)
            {
                case RTK_PHY_CTRL_SERDES_MODE_USXGMII:
                    diag_util_mprintf("\tSerDes Mode : USXGMII\n");
                    break;
                case RTK_PHY_CTRL_SERDES_MODE_10GR_5GX_2P5GX_SGMII:
                    diag_util_mprintf("\tSerDes Mode : MC Mode(10G-R/5G-X/2.5G-X/SGMII)\n");
                    break;
                case RTK_PHY_CTRL_SERDES_MODE_10GR_XFI5GADAPT_XFI2P5GADAPT_SGMII:
                    diag_util_mprintf("\tSerDes Mode : MC Mode(10G-R/XFI-5G-ADAPT/XFI-2.5G-ADAPT/SGMII)\n");
                    break;
                case RTK_PHY_CTRL_SERDES_MODE_10GR_5GR_2P5GX_SGMII:
                    diag_util_mprintf("\tSerDes Mode : MC Mode(10G-R/5G-R/2.5G-X/SGMII)\n");
                    break;
                case RTK_PHY_CTRL_SERDES_MODE_10GR_XFI5GCPRI_2P5GX_SGMII:
                    diag_util_mprintf("\tSerDes Mode : MC Mode(10G-R/XFI-5G-CPRI/2.5G-X/SGMII)\n");
                    break;
                case RTK_PHY_CTRL_SERDES_MODE_OFF:
                    diag_util_mprintf("\tSerDes Mode : Power Off\n");
                    break;
                default:
                    diag_util_mprintf("\tSerDes Mode : unknown\n");
                    break;
            }
        }
    }
    return CPARSER_OK;
}

/*
 * port set phyctrl serdes port ( <PORT_LIST:ports> | all ) update
 */
cparser_result_t  cparser_cmd_port_set_phyctrl_serdes_port_ports_all_update(cparser_context_t *context,
     char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_SERDES_UPDTAE;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, 1);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        if (ret != RT_ERR_OK)
        {
            diag_util_mprintf("Port %2d :\n", port);
            DIAG_ERR_PRINT(ret);
        }
    }
    return CPARSER_OK;
}

/*
 * port set phyctrl serdes port ( <PORT_LIST:ports> | all ) auto-nego ( sgmii | usxgmii | base-x | xsgmii ) ( disable | enable )
 */
cparser_result_t  cparser_cmd_port_set_phyctrl_serdes_port_ports_all_auto_nego_sgmii_usxgmii_base_x_xsgmii_disable_enable(cparser_context_t *context,
     char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);


    if (0 == osal_strcmp(TOKEN_STR(7), "sgmii"))
    {
        type = RTK_PHY_CTRL_SERDES_SGMII_AN;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "usxgmii"))
    {
        type = RTK_PHY_CTRL_SERDES_USXGMII_AN;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "base-x"))
    {
        type = RTK_PHY_CTRL_SERDES_BASEX_AN;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "xsgmii"))
    {
        type = RTK_PHY_CTRL_SERDES_XSGMII_AN;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    value = ('e' == TOKEN_CHAR(8, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
/*
 * port get phyctrl serdes port ( <PORT_LIST:ports> | all ) auto-nego ( sgmii | usxgmii | base-x | xsgmii )
 */
cparser_result_t  cparser_cmd_port_get_phyctrl_serdes_port_ports_all_auto_nego_sgmii_usxgmii_base_x_xsgmii(cparser_context_t *context,
     char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);


    if (0 == osal_strcmp(TOKEN_STR(7), "sgmii"))
    {
        type = RTK_PHY_CTRL_SERDES_SGMII_AN;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "usxgmii"))
    {
        type = RTK_PHY_CTRL_SERDES_USXGMII_AN;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "base-x"))
    {
        type = RTK_PHY_CTRL_SERDES_BASEX_AN;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "xsgmii"))
    {
        type = RTK_PHY_CTRL_SERDES_XSGMII_AN;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }

    return CPARSER_OK;
}

/*
 * port set phyctrl serdes port ( <PORT_LIST:ports> | all ) polarity ( tx | rx ) ( normal | inverse )
 */
cparser_result_t  cparser_cmd_port_set_phyctrl_serdes_port_ports_all_polarity_tx_rx_normal_inverse(cparser_context_t *context,
     char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);


    if (0 == osal_strcmp(TOKEN_STR(7), "tx"))
    {
        type = RTK_PHY_CTRL_SERDES_TX_POLARITY;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "rx"))
    {
        type = RTK_PHY_CTRL_SERDES_RX_POLARITY;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    value = ('i' == TOKEN_CHAR(8, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
/*
 * port get phyctrl serdes port ( <PORT_LIST:ports> | all ) polarity
 */
cparser_result_t  cparser_cmd_port_get_phyctrl_serdes_port_ports_all_polarity(cparser_context_t *context,
     char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        type = RTK_PHY_CTRL_SERDES_TX_POLARITY;
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - SerDes TX polarity : %s\n", type, (value == 0) ? "normal" : "inverse");
        }
        type = RTK_PHY_CTRL_SERDES_RX_POLARITY;
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - SerDes RX polarity : %s\n", type, (value == 0) ? "normal" : "inverse");
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_PHY_CTRL_SNR

double _diag_port_snr_reg_to_db(rtk_port_speed_t speed, uint32 data)
{
    int32 i = 0;
    int32 len = 0;
    uint32 integer = 0;
    uint32 decimal = 0;
    double mse = 0;
    switch (speed)
    {
        case PORT_SPEED_10G:
        case PORT_SPEED_5G:
        case PORT_SPEED_2_5G:
            decimal = (data & 0x7FF);
            integer = (data & 0xFFFF) >> 11;
            len = 11;
            break;

        case PORT_SPEED_1000M:
        case PORT_SPEED_100M:
            decimal = (data & 0x1FFF);
            integer = 0;
            len = 17;
            break;

        default:
            return 0;
    }

    for (i = 1 ; i <= len ; i ++)
    {
        if( decimal & (1UL << (len - i)))
        {
            mse = mse + (pow(2.0, (i * (-1))));
        }
    }
    mse = mse + (double)integer;

    switch (speed)
    {
        case PORT_SPEED_10G:
        case PORT_SPEED_5G:
        case PORT_SPEED_2_5G:
            return (10 * log10(81/mse));

        case PORT_SPEED_1000M:
        case PORT_SPEED_100M:
            return (10 * log10(2/mse));

        default:
            return 0;
    }
    return 0;
}

uint32 _diag_port_snr_db_to_reg(rtk_port_speed_t speed, double snr)
{
    uint32 phyData = 0;
    int32 i = 0;
    int32 len = 0;
    double mse = 0;
    double tmp = 0;

    switch (speed)
    {
        case PORT_SPEED_10G:
        case PORT_SPEED_5G:
        case PORT_SPEED_2_5G:
            mse =  81 / (pow (10, (snr / 10)));
            len = 11;
            break;

        case PORT_SPEED_1000M:
        case PORT_SPEED_100M:
            mse =  2 / (pow (10, (snr / 10)));
            len = 17;
            break;
        default:
            return 0;
    }

    for (i = 1; i <= len; i++)
    {
        tmp = pow(2.0, (i * (-1)));
        if (mse > tmp)
        {
            phyData |= (1UL << (len - i));
            mse = mse - tmp;
        }
        else if (fabs(mse - tmp) < 0.0000001)
        {
            phyData |= (1UL << (len - i));
            mse = 0;
            break;
        }

    }
    return (phyData & 0xFFFF);
}

/*
 * port get phyctrl snr-threshold port ( <PORT_LIST:ports> | all ) ( 10g-master | 10g-slave | 5g-master | 5g-slave | 2p5g-master | 2p5g-slave | 1g )
 */
cparser_result_t cparser_cmd_port_get_phyctrl_snr_threshold_port_ports_all_10g_master_10g_slave_5g_master_5g_slave_2p5g_master_2p5g_slave_1g(cparser_context_t *context,
                                                                                                                                   char **ports_ptr)
{
    uint32                 unit = 0;
    rtk_port_t             port = 0;
    int32                  ret = RT_ERR_FAILED;
    diag_portlist_t        portlist;
    rtk_phy_ctrl_t         type;
    uint32                 value;
    rtk_port_speed_t       speed;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if (0 == osal_strcmp(TOKEN_STR(6), "10g-master"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_10G_MASTER;
        speed = PORT_SPEED_10G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "10g-slave"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_10G_SLAVE;
        speed = PORT_SPEED_10G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "5g-master"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_5G_MASTER;
        speed = PORT_SPEED_5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "5g-slave"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_5G_SLAVE;
        speed = PORT_SPEED_5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "2p5g-master"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_2P5G_MASTER;
        speed = PORT_SPEED_2_5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "2p5g-slave"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_2P5G_SLAVE;
        speed = PORT_SPEED_2_5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "1g"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_1G;
        speed = PORT_SPEED_1000M;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tSNR threshold - Value : %lf (db) reg:[0x%04X]\n", _diag_port_snr_reg_to_db(speed, value), value);
        }
    }

    return CPARSER_OK;
}

/*
 * port set phyctrl snr-threshold port ( <PORT_LIST:ports> | all ) ( 10g-master | 10g-slave | 5g-master | 5g-slave | 2p5g-master | 2p5g-slave | 1g ) <FLOAT:snr_value>
 */
cparser_result_t cparser_cmd_port_set_phyctrl_snr_threshold_port_ports_all_10g_master_10g_slave_5g_master_5g_slave_2p5g_master_2p5g_slave_1g_snr_value(cparser_context_t *context,
                                                                                                                                             char **ports_ptr,
                                                                                                                                             double *snr_value_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;
    rtk_port_speed_t     speed;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);


    if (0 == osal_strcmp(TOKEN_STR(6), "10g-master"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_10G_MASTER;
        speed = PORT_SPEED_10G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "10g-slave"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_10G_SLAVE;
        speed = PORT_SPEED_10G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "5g-master"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_5G_MASTER;
        speed = PORT_SPEED_5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "5g-slave"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_5G_SLAVE;
        speed = PORT_SPEED_5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "2p5g-master"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_2P5G_MASTER;
        speed = PORT_SPEED_2_5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "2p5g-slave"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_2P5G_SLAVE;
        speed = PORT_SPEED_2_5G;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "1g"))
    {
        type = RTK_PHY_CTRL_SNR_THRESHOLD_1G;
        speed = PORT_SPEED_1000M;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }
    value = _diag_port_snr_db_to_reg(speed, *snr_value_ptr);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        diag_util_mprintf("\tSet SNR threshold - Value : %lf (db) reg:[0x%04X]\n", *snr_value_ptr, value);
    }
    return CPARSER_OK;
}

/*
 * port get phyctrl snr port ( <PORT_LIST:ports> | all ) ( ch0 | ch1 | ch2 | ch3 )
 */
cparser_result_t cparser_cmd_port_get_phyctrl_snr_port_ports_all_ch0_ch1_ch2_ch3(cparser_context_t *context, char **ports_ptr)
{
    uint32                     unit = 0;
    rtk_port_t                 port = 0;
    int32                      ret = RT_ERR_FAILED;
    diag_portlist_t            portlist;
    rtk_phy_ctrl_t             type;
    uint32                     value;
    rtk_port_speed_t           speed;
    rtk_port_duplex_t          duplex;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if (0 == osal_strcmp(TOKEN_STR(6), "ch0"))
    {
        type = RTK_PHY_CTRL_SNR_CH0;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "ch1"))
    {
        type = RTK_PHY_CTRL_SNR_CH1;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "ch2"))
    {
        type = RTK_PHY_CTRL_SNR_CH2;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "ch3"))
    {
        type = RTK_PHY_CTRL_SNR_CH3;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }



    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_speedDuplex_get(unit, port, &speed, &duplex);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }

        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tSNR - Value : %lf (db) reg:[0x%04X]\n", _diag_port_snr_reg_to_db(speed, value), value);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_REINIT
/*
 * port set phyctrl reinit port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_set_phyctrl_reinit_port_ports_all(cparser_context_t *context, char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_REINIT;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, 1);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_SKEW
/*
 * port get phyctrl skew port ( <PORT_LIST:ports> | all ) ( b | c | d )
 */
cparser_result_t cparser_cmd_port_get_phyctrl_skew_port_ports_all_b_c_d(cparser_context_t *context, char **ports_ptr)
{
    uint32                     unit = 0;
    rtk_port_t                 port = 0;
    int32                      ret = RT_ERR_FAILED;
    diag_portlist_t            portlist;
    rtk_phy_ctrl_t             type;
    uint32                     value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if (0 == osal_strcmp(TOKEN_STR(6), "b"))
    {
        type = RTK_PHY_CTRL_SKEW_PAIR_B;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "c"))
    {
        type = RTK_PHY_CTRL_SKEW_PAIR_C;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "d"))
    {
        type = RTK_PHY_CTRL_SKEW_PAIR_D;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_IPG_SHRINK_PORT_PORTS_ALL_STATE
/*
 * port get phy-ipg-shrink port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_port_get_phy_ipg_shrink_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    rtk_port_t      port = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          value;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        ret = rtk_port_phyCtrl_get(unit, port, RTK_PHY_CTRL_IPG_SHRINK, &value);
        if ((ret == RT_ERR_CHIP_NOT_SUPPORTED) || (ret == RT_ERR_PORT_NOT_SUPPORTED))
        {
            diag_util_mprintf("\tIPG shrink : Not support\n");
        }
        else if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tIPG shrink : %u\n", value);
        }
    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_IPG_SHRINK_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * port set phy-ipg-shrink port ( <PORT_LIST:ports> | all )  state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_phy_ipg_shrink_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    uint32          value;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('e' == TOKEN_CHAR(6, 0))
        {
            value = 1;
        }
        else if ('d' == TOKEN_CHAR(6, 0))
        {
            value = 0;
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        ret = rtk_port_phyCtrl_set(unit, port, RTK_PHY_CTRL_IPG_SHRINK, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_RTCT_CABLE_TYPE
/*
 * port set phyctrl rtct-cable port ( <PORT_LIST:ports> | all ) ( cat5e | cat6a | cat6a-uu )
 */
cparser_result_t cparser_cmd_port_set_phyctrl_rtct_cable_port_ports_all_cat5e_cat6a_cat6a_uu(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_RTCT_CABLE_TYPE;

    if (0 == osal_strcmp(TOKEN_STR(6), "cat5e"))
    {
        value = RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT5E;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "cat6a"))
    {
        value = RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT6A;
    }
    else if (0 == osal_strcmp(TOKEN_STR(6), "cat6a-uu"))
    {
        value = RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT6A_UU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_IPG_STK_MODE_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * port set ipg-stk-mode port ( <PORT_LIST:ports> | all )  state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_ipg_stk_mode_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    uint32          value;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ('e' == TOKEN_CHAR(6, 0))
        {
            value = 1;
        }
        else if ('d' == TOKEN_CHAR(6, 0))
        {
            value = 0;
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        ret = rtk_port_miscCtrl_set(unit, port, RTK_PORT_MISC_CTRL_IPG_STK_MODE, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_ipg_stk_mode_port_ports_all_state_disable_enable */
#endif

#ifdef CMD_PORT_GET_IPG_STK_MODE_PORT_PORTS_ALL_STATE
/*
 * port get ipg-stk-mode port ( <PORT_LIST:ports> | all ) state */
cparser_result_t
cparser_cmd_port_get_ipg_stk_mode_port_ports_all_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    rtk_port_t      port = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          value;
    diag_portlist_t portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d :\n", port);
        ret = rtk_port_miscCtrl_get(unit, port, RTK_PORT_MISC_CTRL_IPG_STK_MODE, &value);
        if ((ret == RT_ERR_CHIP_NOT_SUPPORTED) || (ret == RT_ERR_PORT_NOT_SUPPORTED))
        {
            diag_util_mprintf("\tIPG Stack Mode : Not support\n");
        }
        else if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tIPG Stack Mode : %u\n", value);
        }
    }  /*end of DIAG_UTIL_PORTMASK_SCAN(portlist, port)*/

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_ipg_stk_mode_port_ports_all_state */
#endif

#ifdef CMD_PORT_PHY_CTRL_MDI_POLARITY_SWAP
/*
 * port get phyctrl mdi port ( <PORT_LIST:ports> | all ) swap
 */
cparser_result_t cparser_cmd_port_get_phyctrl_mdi_port_ports_all_swap(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_MDI_POLARITY_SWAP;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d (0x%04X)\n", type, value, value);
            diag_util_mprintf("\tpair A: %s\n", (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_A) ? "swap" : "normal");
            diag_util_mprintf("\tpair B: %s\n", (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_B) ? "swap" : "normal");
            diag_util_mprintf("\tpair C: %s\n", (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_C) ? "swap" : "normal");
            diag_util_mprintf("\tpair D: %s\n", (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_D) ? "swap" : "normal");

        }
    }
    return CPARSER_OK;
}

/*
 * port set phyctrl mdi port ( <PORT_LIST:ports> | all ) swap <UINT:pairbitmap>
 */
cparser_result_t cparser_cmd_port_set_phyctrl_mdi_port_ports_all_swap_pairbitmap(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *pairbitmap_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_MDI_POLARITY_SWAP;
    value = *pairbitmap_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d (0x%04X)\n", type, value, value);
        diag_util_mprintf("\tpair A: %s\n", (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_A) ? "swap" : "normal");
        diag_util_mprintf("\tpair B: %s\n", (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_B) ? "swap" : "normal");
        diag_util_mprintf("\tpair C: %s\n", (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_C) ? "swap" : "normal");
        diag_util_mprintf("\tpair D: %s\n", (value & RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_D) ? "swap" : "normal");
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_PHY_CTRL_MDI_INVERSE
/*
 * port get phyctrl mdi port ( <PORT_LIST:ports> | all ) inverse
 */
cparser_result_t cparser_cmd_port_get_phyctrl_mdi_port_ports_all_inverse(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_MDI_INVERSE;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - State : %s\n", type,
                              (value == 1) ? ("Enable") : ("Disable"));
        }
    }
    return CPARSER_OK;
}

/*
 * port set phyctrl mdi port ( <PORT_LIST:ports> | all ) inverse ( disable | enable )
 */

cparser_result_t cparser_cmd_port_set_phyctrl_mdi_port_ports_all_inverse_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_MDI_INVERSE;

    if (0 == osal_strcmp(TOKEN_STR(7), "enable"))
    {
        value = 1;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "disable"))
    {
        value = 0;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}

#endif

#ifdef CMD_PORT_GET_PHYCTRL_PTP_PORT_PORTS_ALL_ROLE
/*
 * port get phyctrl ptp port ( <PORT_LIST:ports> | all ) role
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_ptp_port_ports_all_role(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_PORT_ROLE;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            switch (value)
            {
                case RTK_PHY_CTRL_PTP_PORT_ROLE_NONE:
                    diag_util_mprintf("\tPhyctrl Type(%d) - port role : NONE\n", type);
                    break;
                case RTK_PHY_CTRL_PTP_PORT_ROLE_BC_OC:
                    diag_util_mprintf("\tPhyctrl Type(%d) - port role : BC_OC\n", type);
                    break;
                case RTK_PHY_CTRL_PTP_PORT_ROLE_E2E_TC:
                    diag_util_mprintf("\tPhyctrl Type(%d) - port role : E2E_TC\n", type);
                    break;
                case RTK_PHY_CTRL_PTP_PORT_ROLE_P2P_TC:
                    diag_util_mprintf("\tPhyctrl Type(%d) - port role : P2P_TC\n", type);
                    break;
                default:
                    diag_util_mprintf("\tPhyctrl Type(%d) - port role : Unknow(%d)\n", type, value);
                    break;
            }
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_ptp_port_ports_all_role */
#endif

#ifdef CMD_PORT_SET_PHYCTRL_PTP_PORT_PORTS_ALL_ROLE_NONE_BC_TC_E2E_TC_P2P_TC
/*
 * port set phyctrl ptp port ( <PORT_LIST:ports> | all ) role ( none | bc_tc | e2e_tc | p2p_tc )
 */
cparser_result_t
cparser_cmd_port_set_phyctrl_ptp_port_ports_all_role_none_bc_tc_e2e_tc_p2p_tc(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_PORT_ROLE;
    if (0 == osal_strcmp(TOKEN_STR(7), "none"))
    {
        value = RTK_PHY_CTRL_PTP_PORT_ROLE_NONE;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "bc_tc"))
    {
        value = RTK_PHY_CTRL_PTP_PORT_ROLE_BC_OC;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "e2e_tc"))
    {
        value = RTK_PHY_CTRL_PTP_PORT_ROLE_E2E_TC;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "p2p_tc"))
    {
        value = RTK_PHY_CTRL_PTP_PORT_ROLE_P2P_TC;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phyctrl_ptp_port_ports_all_role_none_bc_tc_e2e_tc_p2p_tc */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_PTP_PORT_PORTS_ALL_TX_IMBAL
/*
 * port get phyctrl ptp port ( <PORT_LIST:ports> | all ) tx-imbal
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_ptp_port_ports_all_tx_imbal(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_TX_IMBAL;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_ptp_port_ports_all_tx_imbal */
#endif

#ifdef CMD_PORT_SET_PHYCTRL_PTP_PORT_PORTS_ALL_TX_IMBAL_IMBAL
/*
 * port set phyctrl ptp port ( <PORT_LIST:ports> | all ) tx-imbal <UINT:imbal>
 */
cparser_result_t
cparser_cmd_port_set_phyctrl_ptp_port_ports_all_tx_imbal_imbal(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *imbal_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_TX_IMBAL;
    value = *imbal_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phyctrl_ptp_port_ports_all_tx_imbal_imbal */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_PTP_PORT_PORTS_ALL_RX_IMBAL
/*
 * port get phyctrl ptp port ( <PORT_LIST:ports> | all ) rx-imbal
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_ptp_port_ports_all_rx_imbal(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_RX_IMBAL;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_ptp_port_ports_all_rx_imbal */
#endif

#ifdef CMD_PORT_SET_PHYCTRL_PTP_PORT_PORTS_ALL_RX_IMBAL_IMBAL
/*
 * port set phyctrl ptp port ( <PORT_LIST:ports> | all ) rx-imbal <UINT:imbal>
 */
cparser_result_t
cparser_cmd_port_set_phyctrl_ptp_port_ports_all_rx_imbal_imbal(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *imbal_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_RX_IMBAL;
    value = *imbal_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phyctrl_ptp_port_ports_all_rx_imbal_imbal */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_PTP_PORT_PORTS_ALL_TOD_DELAY
/*
 * port get phyctrl ptp port ( <PORT_LIST:ports> | all ) tod-delay
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_ptp_port_ports_all_tod_delay(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_REFTIME_TOD_DELAY;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : 0x%X\n", type, value);
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_ptp_port_ports_all_tod_delay */
#endif

#ifdef CMD_PORT_SET_PHYCTRL_PTP_PORT_PORTS_ALL_TOD_DELAY_DELAY
/*
 * port set phyctrl ptp port ( <PORT_LIST:ports> | all ) tod-delay <UINT:delay>
 */
cparser_result_t
cparser_cmd_port_set_phyctrl_ptp_port_ports_all_tod_delay_delay(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *delay_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_REFTIME_TOD_DELAY;
    value = *delay_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phyctrl_ptp_port_ports_all_tod_delay_delay */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_PTP_PORT_PORTS_ALL_DURATION_TH
/*
 * port get phyctrl ptp port ( <PORT_LIST:ports> | all ) duration-th
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_ptp_port_ports_all_duration_th(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_DURATION_THRESHOLD;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_ptp_port_ports_all_duration_th */
#endif

#ifdef CMD_PORT_SET_PHYCTRL_PTP_PORT_PORTS_ALL_DURATION_TH_THRESHOLD
/*
 * port set phyctrl ptp port ( <PORT_LIST:ports> | all ) duration-th <UINT:threshold>
 */
cparser_result_t
cparser_cmd_port_set_phyctrl_ptp_port_ports_all_duration_th_threshold(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *threshold_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_DURATION_THRESHOLD;
    value = *threshold_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phyctrl_ptp_port_ports_all_duration_th_threshold */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_PTP_PORT_PORTS_ALL_CLOCK_SRC
/*
 * port get phyctrl ptp port ( <PORT_LIST:ports> | all ) clock-src
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_ptp_port_ports_all_clock_src(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_CLOCK_SRC;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Clock Source : %s\n", type,
                              (value == RTK_PHY_CTRL_PTP_CLOCK_SRC_INT) ? ("Internal Clock") : ("External Clock"));
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_ptp_port_ports_all_clock_src */
#endif

#ifdef CMD_PORT_SET_PHYCTRL_PTP_PORT_PORTS_ALL_CLOCK_SRC_EXTERNAL_INTERNAL
/*
 * port set phyctrl ptp port ( <PORT_LIST:ports> | all ) clock-src ( external | internal )
 */
cparser_result_t
cparser_cmd_port_set_phyctrl_ptp_port_ports_all_clock_src_external_internal(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_CLOCK_SRC;
    if (0 == osal_strcmp(TOKEN_STR(7), "external"))
    {
        value = RTK_PHY_CTRL_PTP_CLOCK_SRC_EXT;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "internal"))
    {
        value = RTK_PHY_CTRL_PTP_CLOCK_SRC_INT;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phyctrl_ptp_port_ports_all_clock_src_external_internal */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_PTP_PORT_PORTS_ALL_STATE
/*
 * port get phyctrl ptp port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_ptp_port_ports_all_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - State : %s\n", type,
                              (value == 0) ? ("Disable") : ("Enable"));
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_ptp_port_ports_all_state */
#endif

#ifdef CMD_PORT_SET_PHYCTRL_PTP_PORT_PORTS_ALL_STATE_ENABLE_DISABLE
/*
 * port set phyctrl ptp port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_port_set_phyctrl_ptp_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP;

    if (0 == osal_strcmp(TOKEN_STR(7), "enable"))
    {
        value = 1;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "disable"))
    {
        value = 0;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phyctrl_ptp_port_ports_all_state_enable_disable */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_DEBUG_PORT_PORTS_ALL_MDI_PLUG
/*
 * port get phyctrl debug port ( <PORT_LIST:ports> | all ) mdi_plug
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_debug_port_ports_all_mdi_plug(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_DEBUG_MDI_PLUG;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_debug_port_ports_all_mdi_plug */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_DEBUG_PORT_PORTS_ALL_MDIO_PARITY_CHK
/*
 * port get phyctrl debug port ( <PORT_LIST:ports> | all ) mdio_parity_chk
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_debug_port_ports_all_mdio_parity_chk(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_DEBUG_MDIO_PARITY_CHK;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_debug_port_ports_all_mdio_parity_chk */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_LINKDOWN_CNT_PORT_PORTS_ALL
/*
 * port get phyctrl linkdown_cnt port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_linkdown_cnt_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_LINKDOWN_CNT;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_linkdown_cnt_port_ports_all */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_FATAL_STATUS_PORT_PORTS_ALL
/*
 * port get phyctrl fatal_status port ( <PORT_LIST:ports> | all ) { clear }
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_fatal_status_port_ports_all_clear(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ((TOKEN_NUM == 7) && (!strncmp("clear", TOKEN_STR(6), strlen(TOKEN_STR(6)))))
    {
        type = RTK_PHY_CTRL_FATAL_STATUS_READ_CLEAR;
    }
    else
    {
        type = RTK_PHY_CTRL_FATAL_STATUS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : 0x%X\n", type, value);
            if (value != 0)
            {

                if (value & RTK_PHY_CTRL_FATAL_STATUS_UC_PC_OUT_OF_RANGE)
                    diag_util_mprintf("\t\tUC/UC2 PC is out of range.\n");
                if (value & RTK_PHY_CTRL_FATAL_STATUS_UC_RESPONSE_FAILED)
                    diag_util_mprintf("\t\tUC/UC2 saftey response is failed.\n");
                if (value & RTK_PHY_CTRL_FATAL_STATUS_NCTL_PC_OUT_OF_RANGE)
                    diag_util_mprintf("\t\tany NCTL PC of all threds is out of range\n");
                if (value & RTK_PHY_CTRL_FATAL_STATUS_NCTL_PC_STUCK)
                    diag_util_mprintf("\t\tNCTL PC of MAIN thread is stuck\n");
                if (value & RTK_PHY_CTRL_FATAL_STATUS_SYSCLK_DETECT_FAILED)
                    diag_util_mprintf("\t\tsysclk source is detected to be failed\n");
                if (value & RTK_PHY_CTRL_FATAL_STATUS_MAC_CK250M_FAILED)
                    diag_util_mprintf("\t\tmac_ck250m source is detected to be failed\n");
                if (value & RTK_PHY_CTRL_FATAL_STATUS_SRAM_ECC_1BITS_ERROR)
                    diag_util_mprintf("\t\tUC/NCTL SRAM is detected with 1 bits error by ECC\n");
                if (value & RTK_PHY_CTRL_FATAL_STATUS_SRAM_ECC_2BITS_ERROR)
                    diag_util_mprintf("\t\tUC/NCTL SRAM is detected with 2 bits error by ECC\n");
                if (value & RTK_PHY_CTRL_FATAL_STATUS_UC_STACK_FAILED)
                    diag_util_mprintf("\t\tUC/UC2 stack point (push/pop) is failed\n");
            }
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_fatal_status_port_ports_all */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_PTP_PORT_PORTS_ALL_PLL_STATE
/*
 * port get phyctrl ptp port ( <PORT_LIST:ports> | all ) pll state
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_ptp_port_ports_all_pll_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_PLL;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - State : %s\n", type,
                              (value == 0) ? ("Disable") : ("Enable"));
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_ptp_port_ports_all_pll_state */
#endif

#ifdef CMD_PORT_SET_PHYCTRL_PTP_PORT_PORTS_ALL_PLL_STATE_ENABLE_DISABLE
/*
 * port set phyctrl ptp port ( <PORT_LIST:ports> | all ) pll state ( enable | disable )
 */
cparser_result_t
cparser_cmd_port_set_phyctrl_ptp_port_ports_all_pll_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_PLL;

    if (0 == osal_strcmp(TOKEN_STR(8), "enable"))
    {
        value = 1;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "disable"))
    {
        value = 0;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phyctrl_ptp_port_ports_all_pll_state_enable_disable */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_PTP_PORT_PORTS_ALL_PLL_POWER_SRC
/*
 * port get phyctrl ptp port ( <PORT_LIST:ports> | all ) pll power-src
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_ptp_port_ports_all_pll_power_src(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_PLL_POW_SRC;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            switch (value)
            {
                case RTK_PHY_CTRL_PTP_PLL_POW_SRC_LDO:
                    diag_util_mprintf("\tPhyctrl Type(%d) - pll power src : LDO\n", type);
                    break;
                case RTK_PHY_CTRL_PTP_PLL_POW_SRC_EXT:
                    diag_util_mprintf("\tPhyctrl Type(%d) - pll power src : External\n", type);
                    break;
                default:
                    diag_util_mprintf("\tPhyctrl Type(%d) - pll power src : Unknow(%d)\n", type, value);
                    break;
            }
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_ptp_port_ports_all_pll_power_src */
#endif

#ifdef CMD_PORT_SET_PHYCTRL_PTP_PORT_PORTS_ALL_PLL_POWER_SRC_LDO_EXTERNAL
/*
 * port set phyctrl ptp port ( <PORT_LIST:ports> | all ) pll power-src ( ldo | external )
 */
cparser_result_t
cparser_cmd_port_set_phyctrl_ptp_port_ports_all_pll_power_src_ldo_external(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_PLL_POW_SRC;
    if (0 == osal_strcmp(TOKEN_STR(8), "ldo"))
    {
        value = RTK_PHY_CTRL_PTP_PLL_POW_SRC_LDO;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "external"))
    {
        value = RTK_PHY_CTRL_PTP_PLL_POW_SRC_EXT;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phyctrl_ptp_port_ports_all_pll_power_src_ldo_external */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_PTP_PORT_PORTS_ALL_PLL_CLK_FREQ
/*
 * port get phyctrl ptp port ( <PORT_LIST:ports> | all ) pll clk-freq
 */
cparser_result_t
cparser_cmd_port_get_phyctrl_ptp_port_ports_all_pll_clk_freq(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_PLL_CLK;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            switch (value)
            {
                case RTK_PHY_CTRL_PTP_PLL_CLK_25MHZ:
                    diag_util_mprintf("\tPhyctrl Type(%d) - pll clk freq : 25Mhz\n", type);
                    break;
                case RTK_PHY_CTRL_PTP_PLL_CLK_50MHZ:
                    diag_util_mprintf("\tPhyctrl Type(%d) - pll clk freq : 50Mhz\n", type);
                    break;
                case RTK_PHY_CTRL_PTP_PLL_CLK_125MHZ:
                    diag_util_mprintf("\tPhyctrl Type(%d) - pll clk freq : 125Mhz\n", type);
                    break;
                case RTK_PHY_CTRL_PTP_PLL_CLK_NONE:
                    diag_util_mprintf("\tPhyctrl Type(%d) - pll clk freq : None\n", type);
                    break;
                default:
                    diag_util_mprintf("\tPhyctrl Type(%d) - pll clk freq : Unknow(%d)\n", type, value);
                    break;
            }
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_phyctrl_ptp_port_ports_all_pll_clk_freq */
#endif

#ifdef CMD_PORT_SET_PHYCTRL_PTP_PORT_PORTS_ALL_PLL_CLK_FREQ_25MHZ_50MHZ_125MHZ
/*
 * port set phyctrl ptp port ( <PORT_LIST:ports> | all ) pll clk-freq ( 25mhz | 50mhz | 125mhz )
 */
cparser_result_t
cparser_cmd_port_set_phyctrl_ptp_port_ports_all_pll_clk_freq_25mhz_50mhz_125mhz(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_PTP_PLL_CLK;
    if (0 == osal_strcmp(TOKEN_STR(8), "25mhz"))
    {
        value = RTK_PHY_CTRL_PTP_PLL_CLK_25MHZ;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "50mhz"))
    {
        value = RTK_PHY_CTRL_PTP_PLL_CLK_50MHZ;
    }
    else if (0 == osal_strcmp(TOKEN_STR(8), "125mhz"))
    {
        value = RTK_PHY_CTRL_PTP_PLL_CLK_125MHZ;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_phyctrl_ptp_port_ports_all_pll_clk_freq_25mhz_50mhz_125mhz */
#endif

#ifdef CMD_PORT_GET_MACSEC_PORT_PORTS_ALL_REG_TX_RX_REG
/*
 * port get macsec port ( <PORT_LIST:ports> | all ) reg ( tx | rx ) <UINT:reg>
 */
cparser_result_t
cparser_cmd_port_get_macsec_port_ports_all_reg_tx_rx_reg(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *reg_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    uint32          reg_data = 0;
    rtk_macsec_dir_t dir = ('t' == TOKEN_CHAR(6, 0)) ? (RTK_MACSEC_DIR_EGRESS) : (RTK_MACSEC_DIR_INGRESS);

    DIAG_UTIL_FUNC_INIT(unit);
    RT_PARAM_CHK((reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port : %d, MACsec.%s: 0x%04X = ", port, (dir == RTK_MACSEC_DIR_EGRESS) ? "TX" : "RX", *reg_ptr);

        DIAG_UTIL_ERR_CHK(rtk_port_macsecReg_get(unit, port, dir, *reg_ptr, &reg_data), ret);
        diag_util_printf("0x%08X\n", reg_data);
    }
    diag_util_mprintf("\n");
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_macsec_port_ports_all_reg_tx_rx_reg */
#endif

#ifdef CMD_PORT_SET_MACSEC_PORT_PORTS_ALL_REG_TX_RX_REG_DATA
/*
 * port set macsec port ( <PORT_LIST:ports> | all ) reg ( tx | rx ) <UINT:reg> <UINT:data>
 */
cparser_result_t
cparser_cmd_port_set_macsec_port_ports_all_reg_tx_rx_reg_data(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *reg_ptr,
    uint32_t *data_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_dir_t dir = ('t' == TOKEN_CHAR(6, 0)) ? (RTK_MACSEC_DIR_EGRESS) : (RTK_MACSEC_DIR_INGRESS);

    DIAG_UTIL_FUNC_INIT(unit);
    RT_PARAM_CHK((reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((data_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_macsecReg_set(unit, port, dir, *reg_ptr, *data_ptr), ret);
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_macsec_port_ports_all_reg_tx_rx_reg_data */
#endif

#ifdef CMD_PORT_GET_PHYCTRL_MACSEC
/*
 * port get phyctrl macsec port ( <PORT_LIST:ports> | all ) bypass
 */
cparser_result_t cparser_cmd_port_get_phyctrl_macsec_port_ports_all_bypass(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_MACSEC_BYPASS;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - Value : %d\n", type, value);
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHYCTRL_MACSEC
/*
 * port set phyctrl macsec port ( <PORT_LIST:ports> | all ) bypass ( enable | disable )
 */
cparser_result_t cparser_cmd_port_set_phyctrl_macsec_port_ports_all_bypass_enable_disable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32               unit = 0;
    rtk_port_t           port = 0;
    int32                ret = RT_ERR_FAILED;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type;
    uint32               value;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_MACSEC_BYPASS;
    value = ('e' == TOKEN_CHAR(7, 0)) ? (1) : (0);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_MACSEC_PORT_PORTS_ALL_STATE_ENABLE_DISABLE
/*
 * port set macsec port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_port_set_macsec_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    rtk_macsec_port_cfg_t pPortcfg;
    diag_portlist_t      portlist;
    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_get(unit, port, &pPortcfg), ret);
        if ('e' == TOKEN_CHAR(6, 0))
        {
            pPortcfg.flags |= (RTK_MACSEC_PORT_F_ENABLE);
        }
        else if ('d' == TOKEN_CHAR(6, 0))
        {
            pPortcfg.flags &= (~RTK_MACSEC_PORT_F_ENABLE);
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_set(unit, port, &pPortcfg), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_macsec_port_ports_all_state_enable_disable */
#endif

#ifdef CMD_PORT_SET_MACSEC_PORT_PORTS_ALL_DROP_KAY_ENABLE_DISABLE
/*
 * port set macsec port ( <PORT_LIST:ports> | all ) drop_kay ( enable | disable )
 */
cparser_result_t
cparser_cmd_port_set_macsec_port_ports_all_drop_kay_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    rtk_macsec_port_cfg_t pPortcfg;
    diag_portlist_t      portlist;
    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_get(unit, port, &pPortcfg), ret);

        pPortcfg.flags |= (RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY);

        if ('e' == TOKEN_CHAR(6, 0))
        {
            pPortcfg.flags |= (RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY);
        }
        else if ('d' == TOKEN_CHAR(6, 0))
        {
            pPortcfg.flags &= (~RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY);
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_set(unit, port, &pPortcfg), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_macsec_port_ports_all_drop_kay_enable_disable */
#endif

#ifdef CMD_PORT_SET_MACSEC_PORT_PORTS_ALL_XPN_INTR_THRESHOLD
/*
 * port set macsec port ( <PORT_LIST:ports> | all ) xpn_intr <UINT64:threshold>
 */
cparser_result_t
cparser_cmd_port_set_macsec_port_ports_all_xpn_intr_threshold(
    cparser_context_t *context,
    char **ports_ptr,
    uint64_t *threshold_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    rtk_macsec_port_cfg_t pPortcfg;
    diag_portlist_t      portlist;
    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_get(unit, port, &pPortcfg), ret);
        pPortcfg.xpn_intr_threshold = *threshold_ptr;
        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_set(unit, port, &pPortcfg), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_macsec_port_ports_all_xpn_intr_threshold */
#endif

#ifdef CMD_PORT_SET_MACSEC_PORT_PORTS_ALL_PN_INTR_THRESHOLD
/*
 * port set macsec port ( <PORT_LIST:ports> | all ) pn_intr <UINT:threshold>
 */
cparser_result_t
cparser_cmd_port_set_macsec_port_ports_all_pn_intr_threshold(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *threshold_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    rtk_macsec_port_cfg_t pPortcfg;
    diag_portlist_t      portlist;
    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_get(unit, port, &pPortcfg), ret);
        pPortcfg.pn_intr_threshold = *threshold_ptr;
        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_set(unit, port, &pPortcfg), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_macsec_port_ports_all_pn_intr_threshold */
#endif

#ifdef CMD_PORT_GET_MACSEC_PORT_PORTS_ALL_CFG
/*
 * port get macsec port ( <PORT_LIST:ports> | all ) cfg
 */
cparser_result_t
cparser_cmd_port_get_macsec_port_ports_all_cfg(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    rtk_macsec_port_cfg_t pPortcfg;
    diag_portlist_t      portlist;

    const char* name_validate[3] = {
        "RTK_MACSEC_VALIDATE_STRICT",
        "RTK_MACSEC_VALIDATE_CHECK",
        "RTK_MACSEC_VALIDATE_DISABLE",
    };

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_get(unit, port, &pPortcfg), ret);
        diag_util_mprintf("Port %2d :\n", port);
        diag_util_mprintf("    Flags              :\n");
        diag_util_mprintf("%s",  (pPortcfg.flags & RTK_MACSEC_PORT_F_ENABLE) ?           "        RTK_MACSEC_PORT_F_ENABLE\n" : "");
        diag_util_mprintf("%s",  (pPortcfg.flags & RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY) ? "        RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY\n" : "");
        diag_util_mprintf("    PN threshold       : %u\n",  pPortcfg.pn_intr_threshold);
        diag_util_mprintf("    XPN threshold      : %llu\n", pPortcfg.xpn_intr_threshold);
        diag_util_mprintf("    non-match validate : %s (%u)\n", name_validate[pPortcfg.nm_validate_frames], pPortcfg.nm_validate_frames);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_macsec_port_ports_all_cfg */
#endif

#ifdef CMD_PORT_ADD_MACSEC_PORT_PORTS_ALL_TXSC_SCI_AES_PN128_PN256_XPN128_XPN256_PROTECT_FRAME_INCLUDE_SCI_USE_ES_USE_SCB_CONF_PROTECT
/*
 * port add macsec port ( <PORT_LIST:ports> | all ) txsc <UINT64:sci> aes ( pn128 | pn256 | xpn128 | xpn256 ) <UINT:protect_frame> <UINT:include_sci> <UINT:use_es> <UINT:use_scb> <UINT:conf_protect>
 */
cparser_result_t
cparser_cmd_port_add_macsec_port_ports_all_txsc_sci_aes_pn128_pn256_xpn128_xpn256_protect_frame_include_sci_use_es_use_scb_conf_protect(
    cparser_context_t *context,
    char **ports_ptr,
    uint64_t *sci_ptr,
    uint32_t *protect_frame_ptr,
    uint32_t *include_sci_ptr,
    uint32_t *use_es_ptr,
    uint32_t *use_scb_ptr,
    uint32_t *conf_protect_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_sc_t sc;
    char            *ptrnStr = NULL;
    uint32          len = 0, sc_id = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PARAM_RANGE_CHK((*protect_frame_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*include_sci_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*use_es_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*use_scb_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*conf_protect_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&sc, 0, sizeof(rtk_macsec_sc_t));

    #ifdef DIAG_UTIL_VAL_TO_BYTE_ARRAY
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*sci_ptr), 8, sc.tx.sci, 0, 8, len);
    #else
    sc.tx.sci[0] = (uint8)(((*sci_ptr) & 0xFF00000000000000) >> (7 * 8));
    sc.tx.sci[1] = (uint8)(((*sci_ptr) & 0x00FF000000000000) >> (6 * 8));
    sc.tx.sci[2] = (uint8)(((*sci_ptr) & 0x0000FF0000000000) >> (5 * 8));
    sc.tx.sci[3] = (uint8)(((*sci_ptr) & 0x000000FF00000000) >> (4 * 8));
    sc.tx.sci[4] = (uint8)(((*sci_ptr) & 0x00000000FF000000) >> (3 * 8));
    sc.tx.sci[5] = (uint8)(((*sci_ptr) & 0x0000000000FF0000) >> (2 * 8));
    sc.tx.sci[6] = (uint8)(((*sci_ptr) & 0x000000000000FF00) >> (1 * 8));
    sc.tx.sci[7] = (uint8)(((*sci_ptr) & 0x00000000000000FF));
    #endif

    ptrnStr = TOKEN_STR(8);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("pn128", ptrnStr, len))
        sc.tx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_128;
    else if (!osal_strncmp("pn256", ptrnStr, len))
        sc.tx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_256;
    else if (!osal_strncmp("xpn128", ptrnStr, len))
        sc.tx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_XPN_128;
    else if (!osal_strncmp("xpn256", ptrnStr, len))
        sc.tx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_XPN_256;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    sc.tx.protect_frame = (*protect_frame_ptr == 0) ? 0 : 1;
    sc.tx.include_sci = (*include_sci_ptr == 0) ? 0 : 1;
    sc.tx.use_es = (*use_es_ptr == 0) ? 0 : 1;
    sc.tx.use_scb = (*use_scb_ptr == 0) ? 0 : 1;
    sc.tx.conf_protect = (*conf_protect_ptr == 0) ? 0 : 1;

    sc.tx.flow_match = RTK_MACSEC_MATCH_NON_CTRL;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_macsec_sc_create(unit, port, RTK_MACSEC_DIR_EGRESS, &sc, &sc_id)) != RT_ERR_OK)
        {
            diag_util_mprintf("Port %2d: ", port);
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("Port %2d: create TX SC : %u\n", port, sc_id);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_add_macsec_port_ports_all_txsc_sci_aes_pn128_pn256_xpn128_xpn256_protect_frame_include_sci_use_es_use_scb_conf_protect */
#endif

#ifdef CMD_PORT_ADD_MACSEC_PORT_PORTS_ALL_TXSC_SCI_AES_PN128_PN256_XPN128_XPN256_PROTECT_FRAME_INCLUDE_SCI_USE_ES_USE_SCB_CONF_PROTECT_MATCH_DA_ADDR
/*
 * port add macsec port ( <PORT_LIST:ports> | all ) txsc <UINT64:sci> aes ( pn128 | pn256 | xpn128 | xpn256 ) <UINT:protect_frame> <UINT:include_sci> <UINT:use_es> <UINT:use_scb> <UINT:conf_protect> match_da <MACADDR:addr>
 */
cparser_result_t
cparser_cmd_port_add_macsec_port_ports_all_txsc_sci_aes_pn128_pn256_xpn128_xpn256_protect_frame_include_sci_use_es_use_scb_conf_protect_match_da_addr(
    cparser_context_t *context,
    char **ports_ptr,
    uint64_t *sci_ptr,
    uint32_t *protect_frame_ptr,
    uint32_t *include_sci_ptr,
    uint32_t *use_es_ptr,
    uint32_t *use_scb_ptr,
    uint32_t *conf_protect_ptr,
    cparser_macaddr_t *addr_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_sc_t sc;
    char            *ptrnStr = NULL;
    uint32          len = 0, sc_id = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PARAM_RANGE_CHK((*protect_frame_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*include_sci_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*use_es_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*use_scb_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*conf_protect_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&sc, 0, sizeof(rtk_macsec_sc_t));

    #ifdef DIAG_UTIL_VAL_TO_BYTE_ARRAY
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*sci_ptr), 8, sc.tx.sci, 0, 8, len);
    #else
    sc.tx.sci[0] = (uint8)(((*sci_ptr) & 0xFF00000000000000) >> (7 * 8));
    sc.tx.sci[1] = (uint8)(((*sci_ptr) & 0x00FF000000000000) >> (6 * 8));
    sc.tx.sci[2] = (uint8)(((*sci_ptr) & 0x0000FF0000000000) >> (5 * 8));
    sc.tx.sci[3] = (uint8)(((*sci_ptr) & 0x000000FF00000000) >> (4 * 8));
    sc.tx.sci[4] = (uint8)(((*sci_ptr) & 0x00000000FF000000) >> (3 * 8));
    sc.tx.sci[5] = (uint8)(((*sci_ptr) & 0x0000000000FF0000) >> (2 * 8));
    sc.tx.sci[6] = (uint8)(((*sci_ptr) & 0x000000000000FF00) >> (1 * 8));
    sc.tx.sci[7] = (uint8)(((*sci_ptr) & 0x00000000000000FF));
    #endif

    ptrnStr = TOKEN_STR(8);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("pn128", ptrnStr, len))
        sc.tx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_128;
    else if (!osal_strncmp("pn256", ptrnStr, len))
        sc.tx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_256;
    else if (!osal_strncmp("xpn128", ptrnStr, len))
        sc.tx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_XPN_128;
    else if (!osal_strncmp("xpn256", ptrnStr, len))
        sc.tx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_XPN_256;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    sc.tx.protect_frame = (*protect_frame_ptr == 0) ? 0 : 1;
    sc.tx.include_sci = (*include_sci_ptr == 0) ? 0 : 1;
    sc.tx.use_es = (*use_es_ptr == 0) ? 0 : 1;
    sc.tx.use_scb = (*use_scb_ptr == 0) ? 0 : 1;
    sc.tx.conf_protect = (*conf_protect_ptr == 0) ? 0 : 1;

    sc.tx.flow_match = RTK_MACSEC_MATCH_MAC_DA;
    osal_memcpy(sc.tx.mac_da.octet, addr_ptr->octet, sizeof(uint8) * 6);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_macsec_sc_create(unit, port, RTK_MACSEC_DIR_EGRESS, &sc, &sc_id)) != RT_ERR_OK)
        {
            diag_util_mprintf("Port %2d: ", port);
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("Port %2d: create TX SC : %u\n", port, sc_id);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_add_macsec_port_ports_all_txsc_sci_aes_pn128_pn256_xpn128_xpn256_protect_frame_include_sci_use_es_use_scb_conf_protect_match_da_addr */
#endif

#ifdef CMD_PORT_GET_MACSEC_PORT_PORTS_ALL_TXSC_SC_ID
/*
 * port get macsec port ( <PORT_LIST:ports> | all ) txsc <UINT:sc_id>
 */
cparser_result_t
cparser_cmd_port_get_macsec_port_ports_all_txsc_sc_id(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_sc_t sc;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_macsec_sc_get(unit, port, RTK_MACSEC_DIR_EGRESS, (*sc_id_ptr), &sc)) != RT_ERR_OK)
        {
            diag_util_mprintf("Port %2d SC %u :", port, (*sc_id_ptr));
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("Port %2d SC %u :\n", port, (*sc_id_ptr));
            diag_util_mprintf("    SCI           : %02X:%02X:%02X:%02X:%02X:%02X %02X%02X\n",
                              sc.tx.sci[0], sc.tx.sci[1], sc.tx.sci[2], sc.tx.sci[3],
                              sc.tx.sci[4], sc.tx.sci[5], sc.tx.sci[6], sc.tx.sci[7]);

            diag_util_mprintf("    cipher_suite  : ");
            {
                switch (sc.tx.cipher_suite)
                {
                    case RTK_MACSEC_CIPHER_GCM_ASE_128:
                        diag_util_mprintf("GCM_ASE_128\n");
                        break;
                    case RTK_MACSEC_CIPHER_GCM_ASE_256:
                        diag_util_mprintf("GCM_ASE_256\n");
                        break;
                    case RTK_MACSEC_CIPHER_GCM_ASE_XPN_128:
                        diag_util_mprintf("GCM_ASE_XPN_128\n");
                        break;
                    case RTK_MACSEC_CIPHER_GCM_ASE_XPN_256:
                        diag_util_mprintf("GCM_ASE_XPN_256\n");
                        break;
                    default:
                        diag_util_mprintf("Unknown(%u)\n", sc.tx.cipher_suite);
                        break;
                }
            }
            diag_util_mprintf("    protect_frame : %u\n", sc.tx.protect_frame);
            diag_util_mprintf("    include_sci   : %u\n", sc.tx.include_sci);
            diag_util_mprintf("    use_es        : %u\n", sc.tx.use_es);
            diag_util_mprintf("    use_scb       : %u\n", sc.tx.use_scb );
            diag_util_mprintf("    conf_protect  : %u\n", sc.tx.conf_protect);
            diag_util_mprintf("    flow_match    : (%u)", sc.tx.flow_match);
            {
                switch (sc.tx.flow_match)
                {
                    case RTK_MACSEC_MATCH_NON_CTRL:
                        diag_util_mprintf("MATCH_NON_CTRL\n");
                        break;
                    case RTK_MACSEC_MATCH_MAC_DA:
                        diag_util_mprintf("MATCH_MAC_DA [%02X:%02X:%02X:%02X:%02X:%02X]\n",
                                          sc.tx.mac_da.octet[0], sc.tx.mac_da.octet[1], sc.tx.mac_da.octet[2],
                                          sc.tx.mac_da.octet[3], sc.tx.mac_da.octet[4], sc.tx.mac_da.octet[5]);
                        break;
                }
            }
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_macsec_port_ports_all_txsc_sc_id */
#endif

#ifdef CMD_PORT_DEL_MACSEC_PORT_PORTS_ALL_TXSC_SC_ID
/*
 * port del macsec port ( <PORT_LIST:ports> | all ) txsc <UINT:sc_id>
 */
cparser_result_t
cparser_cmd_port_del_macsec_port_ports_all_txsc_sc_id(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
         DIAG_UTIL_ERR_CHK(rtk_macsec_sc_del(unit, port, RTK_MACSEC_DIR_EGRESS, (*sc_id_ptr)), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_del_macsec_port_ports_all_txsc_sc_id */
#endif

#ifdef CMD_PORT_ADD_MACSEC_PORT_PORTS_ALL_RXSC_SCI_AES_PN128_PN256_XPN128_XPN256_VALIDATE_STRICT_CHECK_OFF_REPLAY_PROTECT_REPLAY_WINDOW
/*
 * port add macsec port ( <PORT_LIST:ports> | all ) rxsc <UINT64:sci> aes ( pn128 | pn256 | xpn128 | xpn256 ) validate ( strict | check | off ) <UINT:replay_protect> <UINT:replay_window>
 */
cparser_result_t
cparser_cmd_port_add_macsec_port_ports_all_rxsc_sci_aes_pn128_pn256_xpn128_xpn256_validate_strict_check_off_replay_protect_replay_window(
    cparser_context_t *context,
    char **ports_ptr,
    uint64_t *sci_ptr,
    uint32_t *replay_protect_ptr,
    uint32_t *replay_window_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_sc_t sc;
    char            *ptrnStr = NULL;
    uint32          len = 0, sc_id = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*replay_protect_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*replay_window_ptr > 0xFFFFFFFF), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&sc, 0, sizeof(rtk_macsec_sc_t));

    #ifdef DIAG_UTIL_VAL_TO_BYTE_ARRAY
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*sci_ptr), 8, sc.rx.sci, 0, 8, len);
    #else
    sc.rx.sci[0] = (uint8)(((*sci_ptr) & 0xFF00000000000000) >> (7 * 8));
    sc.rx.sci[1] = (uint8)(((*sci_ptr) & 0x00FF000000000000) >> (6 * 8));
    sc.rx.sci[2] = (uint8)(((*sci_ptr) & 0x0000FF0000000000) >> (5 * 8));
    sc.rx.sci[3] = (uint8)(((*sci_ptr) & 0x000000FF00000000) >> (4 * 8));
    sc.rx.sci[4] = (uint8)(((*sci_ptr) & 0x00000000FF000000) >> (3 * 8));
    sc.rx.sci[5] = (uint8)(((*sci_ptr) & 0x0000000000FF0000) >> (2 * 8));
    sc.rx.sci[6] = (uint8)(((*sci_ptr) & 0x000000000000FF00) >> (1 * 8));
    sc.rx.sci[7] = (uint8)(((*sci_ptr) & 0x00000000000000FF));
    #endif

    ptrnStr = TOKEN_STR(8);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("pn128", ptrnStr, len))
        sc.rx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_128;
    else if (!osal_strncmp("pn256", ptrnStr, len))
        sc.rx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_256;
    else if (!osal_strncmp("xpn128", ptrnStr, len))
        sc.rx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_XPN_128;
    else if (!osal_strncmp("xpn256", ptrnStr, len))
        sc.rx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_XPN_256;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    ptrnStr = TOKEN_STR(10);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("strict", ptrnStr, len))
        sc.rx.validate_frames = RTK_MACSEC_VALIDATE_STRICT;
    else if (!osal_strncmp("check", ptrnStr, len))
        sc.rx.validate_frames = RTK_MACSEC_VALIDATE_CHECK;
    else if (!osal_strncmp("off", ptrnStr, len))
        sc.rx.validate_frames = RTK_MACSEC_VALIDATE_DISABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    sc.rx.replay_protect = (*replay_protect_ptr == 0) ? 0 : 1;
    sc.rx.replay_window = (uint32)(*replay_window_ptr);

    sc.rx.flow_match = RTK_MACSEC_MATCH_SCI;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_macsec_sc_create(unit, port, RTK_MACSEC_DIR_INGRESS, &sc, &sc_id)) != RT_ERR_OK)
        {
            diag_util_mprintf("Port %2d: ", port);
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("Port %2d: create RX SC : %u\n", port, sc_id);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_add_macsec_port_ports_all_rxsc_sci_aes_pn128_pn256_xpn128_xpn256_validate_strict_check_off_replay_protect_replay_window */
#endif

#ifdef CMD_PORT_ADD_MACSEC_PORT_PORTS_ALL_RXSC_SCI_AES_PN128_PN256_XPN128_XPN256_VALIDATE_STRICT_CHECK_OFF_REPLAY_PROTECT_REPLAY_WINDOW_MATCH_SA_ADDR
/*
 * port add macsec port ( <PORT_LIST:ports> | all ) rxsc <UINT64:sci> aes ( pn128 | pn256 | xpn128 | xpn256 ) validate ( strict | check | off ) <UINT:replay_protect> <UINT:replay_window> match_sa <MACADDR:addr>
 */
cparser_result_t
cparser_cmd_port_add_macsec_port_ports_all_rxsc_sci_aes_pn128_pn256_xpn128_xpn256_validate_strict_check_off_replay_protect_replay_window_match_sa_addr(
    cparser_context_t *context,
    char **ports_ptr,
    uint64_t *sci_ptr,
    uint32_t *replay_protect_ptr,
    uint32_t *replay_window_ptr,
    cparser_macaddr_t *addr_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_sc_t sc;
    char            *ptrnStr = NULL;
    uint32          len = 0, sc_id = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((*replay_protect_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*replay_window_ptr > 0xFFFFFFFF), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&sc, 0, sizeof(rtk_macsec_sc_t));

    #ifdef DIAG_UTIL_VAL_TO_BYTE_ARRAY
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*sci_ptr), 8, sc.rx.sci, 0, 8, len);
    #else
    sc.rx.sci[0] = (uint8)(((*sci_ptr) & 0xFF00000000000000) >> (7 * 8));
    sc.rx.sci[1] = (uint8)(((*sci_ptr) & 0x00FF000000000000) >> (6 * 8));
    sc.rx.sci[2] = (uint8)(((*sci_ptr) & 0x0000FF0000000000) >> (5 * 8));
    sc.rx.sci[3] = (uint8)(((*sci_ptr) & 0x000000FF00000000) >> (4 * 8));
    sc.rx.sci[4] = (uint8)(((*sci_ptr) & 0x00000000FF000000) >> (3 * 8));
    sc.rx.sci[5] = (uint8)(((*sci_ptr) & 0x0000000000FF0000) >> (2 * 8));
    sc.rx.sci[6] = (uint8)(((*sci_ptr) & 0x000000000000FF00) >> (1 * 8));
    sc.rx.sci[7] = (uint8)(((*sci_ptr) & 0x00000000000000FF));
    #endif

    ptrnStr = TOKEN_STR(8);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("pn128", ptrnStr, len))
        sc.rx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_128;
    else if (!osal_strncmp("pn256", ptrnStr, len))
        sc.rx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_256;
    else if (!osal_strncmp("xpn128", ptrnStr, len))
        sc.rx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_XPN_128;
    else if (!osal_strncmp("xpn256", ptrnStr, len))
        sc.rx.cipher_suite = RTK_MACSEC_CIPHER_GCM_ASE_XPN_256;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    ptrnStr = TOKEN_STR(10);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("strict", ptrnStr, len))
        sc.rx.validate_frames = RTK_MACSEC_VALIDATE_STRICT;
    else if (!osal_strncmp("check", ptrnStr, len))
        sc.rx.validate_frames = RTK_MACSEC_VALIDATE_CHECK;
    else if (!osal_strncmp("off", ptrnStr, len))
        sc.rx.validate_frames = RTK_MACSEC_VALIDATE_DISABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    sc.rx.replay_protect = (*replay_protect_ptr == 0) ? 0 : 1;
    sc.rx.replay_window = (uint32)(*replay_window_ptr);

    sc.rx.flow_match = RTK_MACSEC_MATCH_MAC_SA;
    osal_memcpy(sc.rx.mac_sa.octet, addr_ptr->octet, sizeof(uint8) * 6);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_macsec_sc_create(unit, port, RTK_MACSEC_DIR_INGRESS, &sc, &sc_id)) != RT_ERR_OK)
        {
            diag_util_mprintf("Port %2d: ", port);
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("Port %2d: create RX SC : %u\n", port, sc_id);
        }
    }

    diag_util_mprintf("");

    return CPARSER_OK;
}   /* end of cparser_cmd_port_add_macsec_port_ports_all_rxsc_sci_aes_pn128_pn256_xpn128_xpn256_validate_strict_check_off_replay_protect_replay_window_match_sa_addr */
#endif

#ifdef CMD_PORT_GET_MACSEC_PORT_PORTS_ALL_RXSC_SC_ID
/*
 * port get macsec port ( <PORT_LIST:ports> | all ) rxsc <UINT:sc_id>
 */
cparser_result_t
cparser_cmd_port_get_macsec_port_ports_all_rxsc_sc_id(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_sc_t sc;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_macsec_sc_get(unit, port, RTK_MACSEC_DIR_INGRESS, (*sc_id_ptr), &sc)) != RT_ERR_OK)
        {
            diag_util_mprintf("Port %2d SC %u :", port, (*sc_id_ptr));
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("Port %2d SC %u :\n", port, (*sc_id_ptr));
            diag_util_mprintf("    SCI            : %02X:%02X:%02X:%02X:%02X:%02X %02X%02X\n",
                              sc.rx.sci[0], sc.rx.sci[1], sc.rx.sci[2], sc.rx.sci[3],
                              sc.rx.sci[4], sc.rx.sci[5], sc.rx.sci[6], sc.rx.sci[7]);

            diag_util_mprintf("    cipher_suite   : ");
            {
                switch (sc.rx.cipher_suite)
                {
                    case RTK_MACSEC_CIPHER_GCM_ASE_128:
                        diag_util_mprintf("GCM_ASE_128\n");
                        break;
                    case RTK_MACSEC_CIPHER_GCM_ASE_256:
                        diag_util_mprintf("GCM_ASE_256\n");
                        break;
                    case RTK_MACSEC_CIPHER_GCM_ASE_XPN_128:
                        diag_util_mprintf("GCM_ASE_XPN_128\n");
                        break;
                    case RTK_MACSEC_CIPHER_GCM_ASE_XPN_256:
                        diag_util_mprintf("GCM_ASE_XPN_256\n");
                        break;
                    default:
                        diag_util_mprintf("Unknown(%u)\n", sc.tx.cipher_suite);
                        break;
                }
            }
            diag_util_mprintf("    replay_protect : %u\n", sc.rx.replay_protect);
            diag_util_mprintf("    replay_window  : %u\n", sc.rx.replay_window);
            diag_util_mprintf("    flow_match     : (%u)", sc.rx.flow_match);
            {
                switch (sc.tx.flow_match)
                {
                    case RTK_MACSEC_MATCH_SCI:
                        diag_util_mprintf("MATCH_SCI\n");
                        break;
                    case RTK_MACSEC_MATCH_MAC_SA:
                        diag_util_mprintf("MATCH_MAC_SA [%02X:%02X:%02X:%02X:%02X:%02X]\n",
                                          sc.rx.mac_sa.octet[0], sc.rx.mac_sa.octet[1], sc.rx.mac_sa.octet[2],
                                          sc.rx.mac_sa.octet[3], sc.rx.mac_sa.octet[4], sc.rx.mac_sa.octet[5]);
                        break;
                }
            }
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_macsec_port_ports_all_rxsc_sc_id */
#endif

#ifdef CMD_PORT_DEL_MACSEC_PORT_PORTS_ALL_RXSC_SC_ID
/*
 * port del macsec port ( <PORT_LIST:ports> | all ) rxsc <UINT:sc_id>
 */
cparser_result_t
cparser_cmd_port_del_macsec_port_ports_all_rxsc_sc_id(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
         DIAG_UTIL_ERR_CHK(rtk_macsec_sc_del(unit, port, RTK_MACSEC_DIR_INGRESS, (*sc_id_ptr)), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_del_macsec_port_ports_all_rxsc_sc_id */
#endif

#ifdef CMD_PORT_GET_MACSEC_PORT_PORTS_ALL_RXSC_TXSC_SC_ID_HWSTATUS
/*
 * port get macsec port ( <PORT_LIST:ports> | all ) ( rxsc | txsc ) <UINT:sc_id> hwstatus
 */
cparser_result_t
cparser_cmd_port_get_macsec_port_ports_all_rxsc_txsc_sc_id_hwstatus(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_dir_t dir;
    rtk_macsec_sc_status_t sc_status;
    uint32 i = 0, len = 0;
    char *ptrnStr = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    ptrnStr = TOKEN_STR(5);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("txsc", ptrnStr, len))
        dir = RTK_MACSEC_DIR_EGRESS;
    else if (!osal_strncmp("rxsc", ptrnStr, len))
        dir = RTK_MACSEC_DIR_INGRESS;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_macsec_sc_status_get(unit, port, dir, (*sc_id_ptr), &sc_status)) != RT_ERR_OK)
        {
            diag_util_mprintf("Port %2d SC %u :", port, (*sc_id_ptr));
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            if (dir == RTK_MACSEC_DIR_EGRESS)
            {
                diag_util_mprintf("Port %2d SC %u :\n", port, (*sc_id_ptr));
                diag_util_mprintf("    Current AN            : %u\n", sc_status.tx.running_an);
                diag_util_mprintf("    HW flow entry index   : %u\n", sc_status.tx.hw_flow_index);
                diag_util_mprintf("    HW flow data          : 0x%08X\n", sc_status.tx.hw_flow_data);
                diag_util_mprintf("    HW flow status        : %u\n", sc_status.tx.hw_sc_flow_status);
                diag_util_mprintf("    HW SA entry index     : %u\n", sc_status.tx.hw_sa_index);
                diag_util_mprintf("    SA inUse              : %u\n", sc_status.tx.sa_inUse);

            }
            else
            {
                diag_util_mprintf("Port %2d SC %u :\n", port, (*sc_id_ptr));
                for (i = 0; i < 4; i++)
                {
                    diag_util_mprintf("  AN : %u\n", i);
                    diag_util_mprintf("    HW flow entry index   : %u\n", sc_status.rx.hw_flow_base + i);
                    diag_util_mprintf("    HW flow data          : 0x%08X\n", sc_status.rx.hw_flow_data[i]);
                    diag_util_mprintf("    HW flow status        : %u\n", sc_status.rx.hw_sc_flow_status[i]);
                    diag_util_mprintf("    HW SA entry index     : %u\n", sc_status.rx.hw_sa_index[i]);
                    diag_util_mprintf("    SA inUse              : %u\n", sc_status.rx.sa_inUse[i]);
                }
            }
        }

    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_macsec_port_ports_all_rxsc_txsc_sc_id_hwstatus */
#endif

#ifdef CMD_PORT_ADD_MACSEC_PORT_PORTS_ALL_RXSA_TXSA_SC_SC_ID_AN_AN_AES128_KEY_KEY_P0_KEY_P1_PN_PN
/*
 * port add macsec port ( <PORT_LIST:ports> | all ) ( rxsa | txsa ) sc <UINT:sc_id> an <UINT:an> aes128 key <UINT64:key_p0> <UINT64:key_p1> pn <UINT:pn>
 */
cparser_result_t
cparser_cmd_port_add_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an_aes128_key_key_p0_key_p1_pn_pn(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr,
    uint32_t *an_ptr,
    uint64_t *key_p0_ptr,
    uint64_t *key_p1_ptr,
    uint32_t *pn_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_dir_t dir;
    char *ptrnStr = NULL;
    rtk_macsec_sa_t sa;
    uint32 i = 0, len = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    ptrnStr = TOKEN_STR(5);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("txsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_EGRESS;

    else if (!osal_strncmp("rxsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_INGRESS;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    sa.key_bytes = 16;
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p0_ptr), 8, sa.key, 0, 8, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p1_ptr), 8, sa.key, 8, 8, i);
    sa.pn = (*pn_ptr);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_sa_create(unit, port, dir, (*sc_id_ptr), (*an_ptr), &sa), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_add_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an_aes128_key_key_p0_key_p1_pn_pn */
#endif

#ifdef CMD_PORT_ADD_MACSEC_PORT_PORTS_ALL_RXSA_TXSA_SC_SC_ID_AN_AN_AES256_KEY_KEY_P0_KEY_P1_KEY_P2_KEY_P3_PN_PN
/*
 * port add macsec port ( <PORT_LIST:ports> | all ) ( rxsa | txsa ) sc <UINT:sc_id> an <UINT:an> aes256 key <UINT64:key_p0> <UINT64:key_p1> <UINT64:key_p2> <UINT64:key_p3> pn <UINT:pn>
 */
cparser_result_t
cparser_cmd_port_add_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an_aes256_key_key_p0_key_p1_key_p2_key_p3_pn_pn(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr,
    uint32_t *an_ptr,
    uint64_t *key_p0_ptr,
    uint64_t *key_p1_ptr,
    uint64_t *key_p2_ptr,
    uint64_t *key_p3_ptr,
    uint32_t *pn_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_dir_t dir;
    char *ptrnStr = NULL;
    rtk_macsec_sa_t sa;
    uint32 i = 0, len = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    ptrnStr = TOKEN_STR(5);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("txsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_EGRESS;

    else if (!osal_strncmp("rxsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_INGRESS;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    sa.key_bytes = 32;
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p0_ptr), 8, sa.key, 0, 8, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p1_ptr), 8, sa.key, 8, 8, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p2_ptr), 8, sa.key, 16, 8, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p3_ptr), 8, sa.key, 24, 8, i);
    sa.pn = (*pn_ptr);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_sa_create(unit, port, dir, (*sc_id_ptr), (*an_ptr), &sa), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_add_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an_aes256_key_key_p0_key_p1_key_p2_key_p3_pn_pn */
#endif

#ifdef CMD_PORT_ADD_MACSEC_PORT_PORTS_ALL_RXSA_TXSA_SC_SC_ID_AN_AN_AES128_KEY_KEY_P0_KEY_P1_XPN_XPN_SALT_SALT_P0_SALT_P1_SALT_P2_SSCI_SSCI
/*
 * port add macsec port ( <PORT_LIST:ports> | all ) ( rxsa | txsa ) sc <UINT:sc_id> an <UINT:an> aes128 key <UINT64:key_p0> <UINT64:key_p1> xpn <UINT64:xpn> salt <HEX:salt_p0> <HEX:salt_p1> <HEX:salt_p2> ssci <HEX:ssci>
 */
cparser_result_t
cparser_cmd_port_add_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an_aes128_key_key_p0_key_p1_xpn_xpn_salt_salt_p0_salt_p1_salt_p2_ssci_ssci(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr,
    uint32_t *an_ptr,
    uint64_t *key_p0_ptr,
    uint64_t *key_p1_ptr,
    uint64_t *xpn_ptr,
    uint32_t *salt_p0_ptr,
    uint32_t *salt_p1_ptr,
    uint32_t *salt_p2_ptr,
    uint32_t *ssci_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_dir_t dir;
    char *ptrnStr = NULL;
    rtk_macsec_sa_t sa;
    uint32 i = 0, len = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    ptrnStr = TOKEN_STR(5);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("txsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_EGRESS;

    else if (!osal_strncmp("rxsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_INGRESS;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    sa.key_bytes = 16;
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p0_ptr), 8, sa.key, 0, 8, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p1_ptr), 8, sa.key, 8, 8, i);

    sa.pn = (uint32)((*xpn_ptr) & 0xFFFFFFFF);
    sa.pn_h = (uint32)(((*xpn_ptr) / 0x100000000));

    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*salt_p0_ptr), 4, sa.salt, 0, 4, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*salt_p1_ptr), 4, sa.salt, 4, 4, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*salt_p2_ptr), 4, sa.salt, 8, 4, i);

    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*ssci_ptr), 4, sa.ssci, 0, 4, i);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_sa_create(unit, port, dir, (*sc_id_ptr), (*an_ptr), &sa), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_add_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an_aes128_key_key_p0_key_p1_xpn_xpn_salt_salt_p0_salt_p1_salt_p2_ssci_ssci */
#endif

#ifdef CMD_PORT_ADD_MACSEC_PORT_PORTS_ALL_RXSA_TXSA_SC_SC_ID_AN_AN_AES256_KEY_KEY_P0_KEY_P1_KEY_P2_KEY_P3_XPN_XPN_SALT_SALT_P0_SALT_P1_SALT_P2_SSCI_SSCI
/*
 * port add macsec port ( <PORT_LIST:ports> | all ) ( rxsa | txsa ) sc <UINT:sc_id> an <UINT:an> aes256 key <UINT64:key_p0> <UINT64:key_p1> <UINT64:key_p2> <UINT64:key_p3> xpn <UINT64:xpn> salt <HEX:salt_p0> <HEX:salt_p1> <HEX:salt_p2> ssci <HEX:ssci>
 */
cparser_result_t
cparser_cmd_port_add_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an_aes256_key_key_p0_key_p1_key_p2_key_p3_xpn_xpn_salt_salt_p0_salt_p1_salt_p2_ssci_ssci(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr,
    uint32_t *an_ptr,
    uint64_t *key_p0_ptr,
    uint64_t *key_p1_ptr,
    uint64_t *key_p2_ptr,
    uint64_t *key_p3_ptr,
    uint64_t *xpn_ptr,
    uint32_t *salt_p0_ptr,
    uint32_t *salt_p1_ptr,
    uint32_t *salt_p2_ptr,
    uint32_t *ssci_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_dir_t dir;
    char *ptrnStr = NULL;
    rtk_macsec_sa_t sa;
    uint32 i = 0, len = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    ptrnStr = TOKEN_STR(5);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("txsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_EGRESS;

    else if (!osal_strncmp("rxsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_INGRESS;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    sa.key_bytes = 32;
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p0_ptr), 8, sa.key, 0, 8, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p1_ptr), 8, sa.key, 8, 8, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p2_ptr), 8, sa.key, 16, 8, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*key_p3_ptr), 8, sa.key, 24, 8, i);

    sa.pn = (uint32)((*xpn_ptr) & 0xFFFFFFFF);
    sa.pn_h = (uint32)(((*xpn_ptr) / 0x100000000));

    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*salt_p0_ptr), 4, sa.salt, 0, 4, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*salt_p1_ptr), 4, sa.salt, 4, 4, i);
    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*salt_p2_ptr), 4, sa.salt, 8, 4, i);

    DIAG_UTIL_VAL_TO_BYTE_ARRAY((*ssci_ptr), 4, sa.ssci, 0, 4, i);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_sa_create(unit, port, dir, (*sc_id_ptr), (*an_ptr), &sa), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_add_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an_aes256_key_key_p0_key_p1_key_p2_key_p3_xpn_xpn_salt_salt_p0_salt_p1_salt_p2_ssci_ssci */
#endif

#ifdef CMD_PORT_GET_MACSEC_PORT_PORTS_ALL_RXSA_TXSA_SC_SC_ID_AN_AN
/*
 * port get macsec port ( <PORT_LIST:ports> | all ) ( rxsa | txsa ) sc <UINT:sc_id> an <UINT:an>
 */
cparser_result_t
cparser_cmd_port_get_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr,
    uint32_t *an_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_dir_t dir;
    char *ptrnStr = NULL;
    rtk_macsec_sa_t sa;
    uint32 i = 0, len = 0;
    uint64 xpn = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    ptrnStr = TOKEN_STR(5);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("txsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_EGRESS;
    else if (!osal_strncmp("rxsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_INGRESS;
    else
        return CPARSER_ERR_INVALID_PARAMS;


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_macsec_sa_get(unit, port, dir, (*sc_id_ptr), (*an_ptr), &sa)) != RT_ERR_OK)
        {
            diag_util_mprintf("Port %2d SA(SC %u, AN %u) :", port, (*sc_id_ptr), (*an_ptr));
            DIAG_ERR_PRINT(ret);
        }
        else
        {
                /*
                uint8 key[RTK_MACSEC_MAX_KEY_LEN];  // MACsec Key.
                uint32 key_bytes; // Size of the MACsec key in bytes (16 for AES128, 32 for AES256).

                uint32 pn;      // PN (32-bit) or lower 32-bit of XPN (64-bit)
                uint32 pn_h;    // higher 32-bit of XPN (64-bit)
                uint8 salt[12]; // 12-byte salt (for XPN).
                uint8 ssci[4];  // 4-byte SSCI value (for XPN).
                */
            diag_util_mprintf("Port %2d SA(SC %u, AN %u) :\n", port, (*sc_id_ptr), (*an_ptr));
            diag_util_mprintf("    Key bytes            : %u\n", sa.key_bytes);
            diag_util_mprintf("    Key                  : 0x");
            {
                for (i = 0; i < 16; i++)
                {
                    diag_util_mprintf("%02X", sa.key[i]);
                }
                if (sa.key_bytes == 16)
                {
                    diag_util_mprintf("\n");
                }
                else
                {
                    for (i = 16; i < 32; i++)
                    {
                        diag_util_mprintf("%02X", sa.key[i]);
                    }
                    diag_util_mprintf("\n");
                }
            }
            diag_util_mprintf("    PN                   : %u\n", sa.pn);

            xpn = ((uint64) sa.pn) | (((uint64) sa.pn_h) << 32);
            diag_util_mprintf("    XPN                  : %llu (0x%08X 0x%08X)\n", xpn, sa.pn_h, sa.pn);
            diag_util_mprintf("      Salt               : 0x");
            {
                for (i = 0; i < 12; i++)
                {
                    diag_util_mprintf("%02X", sa.salt[i]);
                }
                diag_util_mprintf("\n");
            }
            diag_util_mprintf("      SSCI               : 0x");
            {
                for (i = 0; i < 4; i++)
                {
                    diag_util_mprintf("%02X", sa.ssci[i]);
                }
                diag_util_mprintf("\n");
            }

        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an */
#endif

#ifdef CMD_PORT_DEL_MACSEC_PORT_PORTS_ALL_RXSA_TXSA_SC_SC_ID_AN_AN
/*
 * port del macsec port ( <PORT_LIST:ports> | all ) ( rxsa | txsa ) sc <UINT:sc_id> an <UINT:an>
 */
cparser_result_t
cparser_cmd_port_del_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr,
    uint32_t *an_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_dir_t dir;
    char *ptrnStr = NULL;
    uint32 len = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    ptrnStr = TOKEN_STR(5);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("txsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_EGRESS;
    else if (!osal_strncmp("rxsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_INGRESS;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_sa_del(unit, port, dir, (*sc_id_ptr),  (*an_ptr)), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_del_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an */
#endif

#ifdef CMD_PORT_SET_MACSEC_PORT_PORTS_ALL_RXSA_TXSA_SC_SC_ID_AN_AN_ACTIVATE
/*
 * port set macsec port ( <PORT_LIST:ports> | all )  ( rxsa | txsa )  sc <UINT:sc_id> an <UINT:an> activate
 */
cparser_result_t
cparser_cmd_port_set_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an_activate(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr,
    uint32_t *an_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_dir_t dir;
    char *ptrnStr = NULL;
    uint32 len = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    ptrnStr = TOKEN_STR(5);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("txsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_EGRESS;
    else if (!osal_strncmp("rxsa", ptrnStr, len))
        dir = RTK_MACSEC_DIR_INGRESS;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_sa_activate(unit, port, dir, (*sc_id_ptr),  (*an_ptr)), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_macsec_port_ports_all_rxsa_txsa_sc_sc_id_an_an_activate */
#endif

#ifdef CMD_PORT_SET_MACSEC_PORT_PORTS_ALL_RXSA_SC_SC_ID_AN_AN_DISABLE
/*
 * port set macsec port ( <PORT_LIST:ports> | all )  rxsa sc <UINT:sc_id> an <UINT:an> disable
 */
cparser_result_t
cparser_cmd_port_set_macsec_port_ports_all_rxsa_sc_sc_id_an_an_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr,
    uint32_t *an_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_rxsa_disable(unit, port, (*sc_id_ptr),  (*an_ptr)), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_macsec_port_ports_all_rxsa_sc_sc_id_an_an_disable */
#endif

#ifdef CMD_PORT_SET_MACSEC_PORT_PORTS_ALL_TXSA_SC_SC_ID_DISABLE
/*
 * port set macsec port ( <PORT_LIST:ports> | all )  txsa sc <UINT:sc_id> disable
 */
cparser_result_t
cparser_cmd_port_set_macsec_port_ports_all_txsa_sc_sc_id_disable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_txsa_disable(unit, port, (*sc_id_ptr)), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_macsec_port_ports_all_txsa_sc_sc_id_disable */
#endif

#ifdef CMD_PORT_GET_MACSEC_PORT_PORTS_ALL_COUNTER
/*
 * port get macsec port ( <PORT_LIST:ports> | all ) counter
 */
cparser_result_t
cparser_cmd_port_get_macsec_port_ports_all_counter(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    uint64 cnt = 0;
    rtk_macsec_stat_t stat = 0;

    const char* name[RTK_MACSEC_STAT_MAX] = {
        "InPktsUntagged",
        "InPktsNoTag",
        "InPktsBadTag",
        "InPktsUnknownSCI",
        "InPktsNoSCI",
        "OutPktsUntagged"
    };


    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %2d:\n", port);
        for (stat = 0; stat < RTK_MACSEC_STAT_MAX; stat++)
        {
            DIAG_UTIL_ERR_CHK(rtk_macsec_stat_port_get(unit, port, stat, &cnt), ret);
            diag_util_mprintf("    %-16s : %llu\n", name[stat], cnt);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_macsec_port_ports_all_counter */
#endif

#ifdef CMD_PORT_GET_MACSEC_PORT_PORTS_ALL_COUNTER_RXSA_TXSA_SC_SC_ID_AN_AN
/*
 * port get macsec port ( <PORT_LIST:ports> | all ) counter ( rxsa | txsa ) sc <UINT:sc_id> an <UINT:an>
 */
cparser_result_t
cparser_cmd_port_get_macsec_port_ports_all_counter_rxsa_txsa_sc_sc_id_an_an(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sc_id_ptr,
    uint32_t *an_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    uint64 cnt = 0;
    rtk_macsec_rxsa_stat_t rx_stat = 0;
    rtk_macsec_txsa_stat_t tx_stat = 0;
    char *ptrnStr = NULL;
    uint32 len = 0;

    const char* name_rx[RTK_MACSEC_RXSA_STAT_MAX] = {
        "InPktsUnusedSA",
        "InPktsNotUsingSA",
        "InPktsUnchecked",
        "InPktsDelayed",
        "InPktsLate",
        "InPktsOK",
        "InPktsInvalid",
        "InPktsNotValid",
        "InOctetsDecryptedValidated"
    };

    const char* name_tx[RTK_MACSEC_TXSA_STAT_MAX] = {
        "OutPktsTooLong",
        "OutOctetsProtectedEncrypted",
        "OutPktsProtectedEncrypted",
    };

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    ptrnStr = TOKEN_STR(6);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("txsa", ptrnStr, len))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            diag_util_mprintf("Port %2d:\n", port);
            for (tx_stat = 0; tx_stat < RTK_MACSEC_TXSA_STAT_MAX; tx_stat++)
            {
                DIAG_UTIL_ERR_CHK(rtk_macsec_stat_txsa_get(unit, port, (*sc_id_ptr), (*an_ptr), tx_stat, &cnt), ret);
                diag_util_mprintf("    %-27s : %llu\n", name_tx[tx_stat], cnt);
            }
        }
    }
    else if (!osal_strncmp("rxsa", ptrnStr, len))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            diag_util_mprintf("Port %2d:\n", port);
            for (rx_stat = 0; rx_stat < RTK_MACSEC_RXSA_STAT_MAX; rx_stat++)
            {
                DIAG_UTIL_ERR_CHK(rtk_macsec_stat_rxsa_get(unit, port, (*sc_id_ptr), (*an_ptr), rx_stat, &cnt), ret);
                diag_util_mprintf("    %-26s : %llu\n", name_rx[rx_stat], cnt);
            }
        }
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_macsec_port_ports_all_counter_rxsa_txsa_sc_sc_id_an_an */
#endif

#ifdef CMD_PORT_SET_MACSEC_PORT_PORTS_ALL_COUNTER_RESET
/*
 * port set macsec port ( <PORT_LIST:ports> | all ) counter reset
 */
cparser_result_t
cparser_cmd_port_set_macsec_port_ports_all_counter_reset(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_stat_clear(unit, port), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_macsec_port_ports_all_counter_reset */
#endif

#ifdef CMD_PORT_GET_MACSEC_PORT_PORTS_ALL_INTR_STATUS
/*
 * port get macsec port ( <PORT_LIST:ports> | all ) intr status
 */
cparser_result_t
cparser_cmd_port_get_macsec_port_ports_all_intr_status(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    rtk_macsec_intr_status_t intr_status;
    uint sc = 0;


    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_macsec_intr_status_get(unit, port, &intr_status)) != RT_ERR_OK)
        {
            diag_util_mprintf("Port %2d:", port);
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("Port %2d:\n", port);
            diag_util_mprintf("    PN over threshold :%u\n", (intr_status.status & RTK_MACSEC_INTR_EGRESS_PN_THRESHOLD) ? (1) : (0));
            if (intr_status.status & RTK_MACSEC_INTR_EGRESS_PN_THRESHOLD)
            {
                diag_util_mprintf("      ");
                for (sc = 0; sc < RTK_MAX_MACSEC_SC_PER_PORT; sc++)
                {
                    if (intr_status.egress_pn_thr_an_bmap[sc] != 0)
                    {
                        diag_util_mprintf("SC%u: [", sc);
                        if (intr_status.egress_pn_thr_an_bmap[sc] & 0x1)
                            diag_util_mprintf("AN%u ", 0);
                        if (intr_status.egress_pn_thr_an_bmap[sc] & 0x2)
                            diag_util_mprintf("AN%u ", 1);
                        if (intr_status.egress_pn_thr_an_bmap[sc] & 0x4)
                            diag_util_mprintf("AN%u ", 2);
                        if (intr_status.egress_pn_thr_an_bmap[sc] & 0x8)
                            diag_util_mprintf("AN%u ", 3);
                        diag_util_mprintf("]\n");
                    }
                }
            }

            diag_util_mprintf("    PN rollover       :%u\n", (intr_status.status & RTK_MACSEC_INTR_EGRESS_PN_ROLLOVER) ? (1) : (0));
            if (intr_status.status & RTK_MACSEC_INTR_EGRESS_PN_ROLLOVER)
            {
                diag_util_mprintf("      ");
                for (sc = 0; sc < RTK_MAX_MACSEC_SC_PER_PORT; sc++)
                {
                    if (intr_status.egress_pn_exp_an_bmap[sc] != 0)
                    {
                        diag_util_mprintf("SC%u: [", sc);
                        if (intr_status.egress_pn_exp_an_bmap[sc] & 0x1)
                            diag_util_mprintf("AN%u ", 0);
                        if (intr_status.egress_pn_exp_an_bmap[sc] & 0x2)
                            diag_util_mprintf("AN%u ", 1);
                        if (intr_status.egress_pn_exp_an_bmap[sc] & 0x4)
                            diag_util_mprintf("AN%u ", 2);
                        if (intr_status.egress_pn_exp_an_bmap[sc] & 0x8)
                            diag_util_mprintf("AN%u ", 3);
                        diag_util_mprintf("]\n");
                    }
                }
            }
        }
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_port_get_macsec_port_ports_all_intr_status */
#endif

#ifdef CMD_PORT_SET_MACSEC_PORT_PORTS_ALL_NM_VALIDATE_STRICT_CHECK_OFF
/*
 * port set macsec port ( <PORT_LIST:ports> | all ) nm_validate ( strict | check | off )
 */
cparser_result_t
cparser_cmd_port_set_macsec_port_ports_all_nm_validate_strict_check_off(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;
    char            *ptrnStr = NULL;
    uint32          len = 0;
    rtk_macsec_port_cfg_t portcfg;
    rtk_macsec_validate_t nm_validate_frames;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    ptrnStr = TOKEN_STR(6);
    len = osal_strlen(ptrnStr);
    if (!osal_strncmp("strict", ptrnStr, len))
        nm_validate_frames = RTK_MACSEC_VALIDATE_STRICT;
    else if (!osal_strncmp("check", ptrnStr, len))
        nm_validate_frames = RTK_MACSEC_VALIDATE_CHECK;
    else if (!osal_strncmp("off", ptrnStr, len))
        nm_validate_frames = RTK_MACSEC_VALIDATE_DISABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_get(unit, port, &portcfg), ret);
        portcfg.nm_validate_frames = nm_validate_frames;
        DIAG_UTIL_ERR_CHK(rtk_macsec_port_cfg_set(unit, port, &portcfg), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_port_set_macsec_port_ports_all_nm_validate_strict_check_off */
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_EEE_PHYMODE
/*
 * port get phyctrl eee-phymode port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_port_get_phyctrl_eee_phymode_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    int32                ret = RT_ERR_FAILED;
    uint32               unit = 0;
    rtk_port_t           port = 0;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type = 0;
    uint32               value = 0;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_EEE_PHYMODE;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - State : %s\n", type,
                              (value == 1) ? ("Enable") : ("Disable"));
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_CTRL_EEE_PHYMODE
/*
 * port set phyctrl eee-phymode port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_port_set_phyctrl_eee_phymode_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    int32               ret = RT_ERR_FAILED;
    uint32              unit = 0;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    rtk_phy_ctrl_t      type = 0;
    uint32              value = 0;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_EEE_PHYMODE;

    if (0 == osal_strcmp(TOKEN_STR(7), "enable"))
    {
        value = 1;
    }
    else if (0 == osal_strcmp(TOKEN_STR(7), "disable"))
    {
        value = 0;
    }
    else
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_set(unit, port, type, value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_GET_PHY_SDS_REG_PORT_PORTS_ALL_SDS_PAGE_SES_REG
/*
 * port get phy-sds-reg port ( <PORT_LIST:ports> | all ) sds-page <UINT:sds_page> sds-reg <UINT:sds_reg>
 */
cparser_result_t cparser_cmd_port_get_phy_sds_reg_port_ports_all_sds_page_sds_page_sds_reg_sds_reg(cparser_context_t *context,
        char **ports_ptr,
        uint32_t *sds_page_ptr,
        uint32_t *sds_reg_ptr)
{
    uint32      unit = 0;
    uint32      reg_data = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((sds_page_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((sds_reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\n");
        diag_util_mprintf("Port : %d\n", port);
        diag_util_mprintf("    SDS Page %d : \n", *sds_page_ptr);
        diag_util_mprintf("    SDS Reg %d : \n", *sds_reg_ptr);

        DIAG_UTIL_ERR_CHK(rtk_port_phySdsReg_get(unit, port, *sds_page_ptr, *sds_reg_ptr, &reg_data), ret);

        diag_util_printf("Reg Data :  0x%04X  \n", reg_data);
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_PORT_SET_PHY_SDS_REG_PORT_PORTS_ALL_SDS_PAGE_SDS_REG_DATA_DATA
/*
 * port set phy-sds-reg port ( <PORT_LIST:ports> | all ) sds-page <UINT:sds_addr> sds-reg <UINT:sds_reg> data <UINT:data>
 */
cparser_result_t cparser_cmd_port_set_phy_sds_reg_port_ports_all_sds_page_sds_page_sds_reg_sds_reg_data_data(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *sds_page_ptr,
    uint32_t *sds_reg_ptr,
    uint32_t *data_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((sds_page_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((sds_reg_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((data_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\n");
        diag_util_mprintf("Port : %d\n", port);
        diag_util_mprintf("    SDS Page %d : \n", *sds_page_ptr);
        diag_util_mprintf("    SDS Reg %d : \n", *sds_reg_ptr);
        diag_util_mprintf("    Reg Date %d : \n", *data_ptr);

        DIAG_UTIL_ERR_CHK(rtk_port_phySdsReg_set(unit, port, *sds_page_ptr, *sds_reg_ptr, *data_ptr), ret);
    }
    return CPARSER_OK;   
}
#endif

#ifdef CMD_PORT_GET_PHY_CTRL_DEBUG_CABLE_LENGTH
/*
 * port get phyctrl cable-length port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_port_get_phyctrl_cable_length_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    int32                ret = RT_ERR_FAILED;
    uint32               unit = 0;
    rtk_port_t           port = 0;
    diag_portlist_t      portlist;
    rtk_phy_ctrl_t       type = 0;
    uint32               value = 0;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    type = RTK_PHY_CTRL_DEBUG_CABLE_LENGTH;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_port_phyCtrl_get(unit, port, type, &value);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        diag_util_mprintf("Port %2d :\n", port);
        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
        else
        {
            diag_util_mprintf("\tPhyctrl Type(%d) - length : %u\n", type, value);
        }
    }

    return CPARSER_OK;
}
#endif

