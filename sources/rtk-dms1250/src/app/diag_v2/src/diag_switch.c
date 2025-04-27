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
 * Purpose : Define diag shell commands for switch
 *
 * Feature : The file includes the following module and sub-modules
 *           1) switch commands.
 *
 */

#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_version.h>
#include <ioal/mem32.h>
#include <rtk/flowctrl.h>
#include <rtk/switch.h>
#include <rtk/port.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#ifdef CONFIG_SDK_DRIVER_WATCHDOG
  #include <drv/watchdog/watchdog.h>
#endif
#ifdef CONFIG_SDK_TC_DRV
  #include <drv/tc/tc.h>
#endif
#include <parser/cparser_priv.h>
#include <common/rt_autoconf.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_flowctrl.h>
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_port.h>
#endif

const char text_switch_flexTblFmt[RTK_SWITCH_FLEX_TBL_FMT_END+1][32] =
{
    "L2 Tunnel",
    "MPLS",
    "IP-MAC binding",
    "ECID",
    "RTK_SWITCH_FLEX_TBL_FMT_END",
};

#ifdef CMD_SWITCH_GET_CHKSUM_ERR_TYPE_L2_PORT_PORTS_ALL_ACTION
/*
 * switch get chksum-err-type l2 port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t cparser_cmd_switch_get_chksum_err_type_l2_port_ports_all_action(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    uint8  *actStr[2] = {(uint8 *)"FORWARD", (uint8 *)"DROP"};
    rtk_switch_chksum_fail_t type = LAYER2_CHKSUM_FAIL;
    rtk_action_t action = ACTION_DROP;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(3,1))
    {
        case '2':
            type = LAYER2_CHKSUM_FAIL;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Layer %c Checksum Error Action\n", TOKEN_CHAR(3,1));

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_chksumFailAction_get(unit, port, type, &action), ret);
        diag_util_mprintf("\tPort %d : %s\n", port, actStr[action]);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_CRC_RECAL_PORT_PORTS_ALL_STATE
/*
 * switch get crc-recal port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_switch_get_crc_recal_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("CRC Re-Calculation Enable Status\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_recalcCRCEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("\tPort %d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_DEV_CAPABILITIES
/*
 * switch get dev-capabilities
 */
cparser_result_t cparser_cmd_switch_get_dev_capabilities(cparser_context_t *context)
{
    uint32 unit = 0;
    uint32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceCapability_print(unit), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_IPV4_ADDRESS
/*
 * switch get ipv4-address
 */
cparser_result_t cparser_cmd_switch_get_ipv4_address(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    char   ipv4Str[16];
    ipaddr_t ip;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(rtk_switch_IPv4Addr_get(unit, &ip), ret);
    DIAG_UTIL_ERR_CHK(diag_util_ip2str(ipv4Str, ip), ret);
    diag_util_printf("IPv4 address : %s\n", ipv4Str);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_MAC_ADDRESS
/*
 * switch get mac-address
 */
cparser_result_t cparser_cmd_switch_get_mac_address(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    char   macStr[18];
    rtk_mac_t mac;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(rtk_switch_mgmtMacAddr_get(unit, &mac), ret);
    DIAG_UTIL_ERR_CHK(diag_util_mac2str(macStr, mac.octet), ret);
    diag_util_printf("MAC address : %s\n", macStr);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_OUI_00_00_00_STATE
/*
 * switch get oui-00-00-00 state
 */
cparser_result_t cparser_cmd_switch_get_oui_00_00_00_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_snapMode_t snapMode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_switch_snapMode_get(unit, &snapMode), ret);
    diag_util_printf("OUI(00-00-00) Comparing State: %s\n", (snapMode == SNAP_MODE_AAAA03000000) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_CPU_MAX_PKT_LEN_RX_DIR_TX_DIR_LENGTH
/*
 * switch get cpu-max-pkt-len ( rx-dir | tx-dir ) length
 */
cparser_result_t cparser_cmd_switch_get_cpu_max_pkt_len_rx_dir_tx_dir_length(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    uint32 len;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('r' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_cpuMaxPktLen_get(unit, PKTDIR_RX, &len), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_cpuMaxPktLen_get(unit, PKTDIR_TX, &len), ret);
    }

    diag_util_printf("CPU Port Max Packet Length (%s) : %u\n", ('r' == TOKEN_CHAR(3,0))? "Rx" : "Tx", len);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_MAX_PKT_LEN_FE_GE_LENGTH
/*
 * switch get max-pkt-len ( fe | ge ) length
 */
cparser_result_t cparser_cmd_switch_get_max_pkt_len_fe_ge_length(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    uint32 len;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(3,0))
    {
        case 'f':
            DIAG_UTIL_ERR_CHK(rtk_switch_maxPktLenLinkSpeed_get(unit, MAXPKTLEN_LINK_SPEED_FE, &len), ret);
            diag_util_printf("System Max Packet Length in 10M/100M Speed : %u\n", len);
            break;

        case 'g':
            DIAG_UTIL_ERR_CHK(rtk_switch_maxPktLenLinkSpeed_get(unit, MAXPKTLEN_LINK_SPEED_GE, &len), ret);
            diag_util_printf("System Max Packet Length in Giga Speed : %u\n", len);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_MAX_PKT_LEN_TAG_LENGTH_STATE
/*
 * switch get max-pkt-len tag-length-state
 */
cparser_result_t cparser_cmd_switch_get_max_pkt_len_tag_length_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_switch_maxPktLenTagLenCntIncEnable_get(unit, &enable), ret);
    diag_util_printf("Max Packet Length Tag Length Configuration : %s\n", (enable == ENABLED) ? "Include" : "Exclude");

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_PROBE_INFORMATION
void _diag_switch_print_portType_info(char *name, rt_portType_info_t *p)
{
    uint32      max = -1, min = -1;
    char portListStr[DIAG_UTIL_PORT_MASK_STRING_LEN];

    if (p->portNum)
    {
        diag_util_lPortMask2str(portListStr, &p->portmask);
        diag_util_port_min_max_get(&p->portmask, &min, &max);
        diag_util_mprintf("%s Port Number: %2d, Minimum: %2d, Maximum: %2d, Ports: %s\n", name, p->portNum, min, max, portListStr);
    }
}

typedef struct {
    uint32          chip_id;
    char            *chipNameStr;
}diag_chipName_t;

/*
 * Get chip name string
 */
int32
_switch_get_chip_name(uint32 chip_id, char **ppChipNameStr)
{
    int     i;
    diag_chipName_t     chipNameDb[] =
        {
            {RTL9301_CHIP_ID,   "RTL9301"},
            {RTL9303_CHIP_ID,   "RTL9303"},
            {RTL9310_CHIP_ID,   "RTL9310"},
            {RTL9311_CHIP_ID,   "RTL9311"},
            {RTL9312_CHIP_ID,   "RTL9312"},
            {RTL9313_CHIP_ID,   "RTL9313"},
            {RTL8351M_CHIP_ID,  "RTL8351"},
            {RTL8352M_CHIP_ID,  "RTL8352"},
            {RTL8353M_CHIP_ID,  "RTL8353"},
            {RTL8391M_CHIP_ID,  "RTL8391"},
            {RTL8392M_CHIP_ID,  "RTL8392"},
            {RTL8393M_CHIP_ID,  "RTL8393"},
            {RTL8396M_CHIP_ID,  "RTL8396"},
            {RTL8330M_CHIP_ID,  "RTL8330"},
            {RTL8380M_CHIP_ID,  "RTL8380"},
            {RTL8332M_CHIP_ID,  "RTL8332"},
            {RTL8382M_CHIP_ID,  "RTL8382"},
            {RTL8352MES_CHIP_ID,  "RTL8352MES"},
            {RTL8353MES_CHIP_ID,  "RTL8353MES"},
            {RTL8392MES_CHIP_ID,  "RTL8392MES"},
            {RTL8393MES_CHIP_ID,  "RTL8393MES"},
            {RTL8396MES_CHIP_ID,  "RTL8396MES"},
        };

    for (i = 0; i < sizeof(chipNameDb)/sizeof(diag_chipName_t); i++)
    {
        if (chipNameDb[i].chip_id == chip_id)
        {
            *ppChipNameStr = chipNameDb[i].chipNameStr;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
}

/*
 * switch get probe-information
 */
cparser_result_t cparser_cmd_switch_get_probe_information(cparser_context_t *context)
{
    uint32                  unit = 0, port = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t    devInfo;
    char                    *chipNameStr;
    rtk_portmask_t          portmask;

    uint8 *pPhyName[] =
    {
        RTK_PHYTYPE_STRING
    };


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    chipNameStr = NULL;
    _switch_get_chip_name(devInfo.chipId, &chipNameStr);
    if (chipNameStr == NULL)
    {
        switch(devInfo.chipId&FAMILY_ID_MASK)
        {
          case RTL8390_FAMILY_ID:
          case RTL8350_FAMILY_ID:
          case RTL8380_FAMILY_ID:
          case RTL8330_FAMILY_ID:
            chipNameStr = "RTL83xx";
            break;
          case RTL9300_FAMILY_ID:
            chipNameStr = "RTL930x";
            break;
        }
    }

    diag_util_mprintf("Unit ID: %d\n", unit);
    diag_util_mprintf("Chip ID: %x (%s)\n", RT_CHIP_ID_DISPLAY(devInfo.chipId), chipNameStr?chipNameStr:"");
    diag_util_mprintf("Family ID: %x\n", RT_CHIP_ID_DISPLAY(devInfo.familyId));
    diag_util_mprintf("Port Number: %2d\n", devInfo.port_number);

    _diag_switch_print_portType_info("All", &devInfo.all);
    _diag_switch_print_portType_info("Ether", &devInfo.ether);

    /* Port information by Ether type */
    _diag_switch_print_portType_info("FE", &devInfo.fe);
    _diag_switch_print_portType_info("GE", &devInfo.ge);
    _diag_switch_print_portType_info("10GE", &devInfo.xge);

    /* Port information by Port medium */
    _diag_switch_print_portType_info("Copper", &devInfo.copper);
    _diag_switch_print_portType_info("Combo", &devInfo.combo);
    _diag_switch_print_portType_info("Fiber", &devInfo.fiber);
    _diag_switch_print_portType_info("Serdes", &devInfo.serdes);

    diag_util_mprintf("CPU Port : %d\n", devInfo.cpuPort);

    /* Display PHY chip information */
    diag_util_mprintf("\n");
    diag_util_mprintf("Port     PHY chip\n");
    diag_util_mprintf("======================\n");
    osal_memcpy(&portmask, &devInfo.ether.portmask, sizeof(rtk_portmask_t));

    for (port = 0; port < RTK_MAX_NUM_OF_PORTS; port++)
    {
        if (RTK_PORTMASK_IS_PORT_SET(devInfo.ether.portmask, port))
        {
            if (devInfo.phyType[port] > RTK_PHYTYPE_END)
            {
                diag_util_mprintf(" %2d      UNKNOW\n", port);
                continue;
            }
            diag_util_mprintf(" %2d      %s\n", port, pPhyName[devInfo.phyType[port]]);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_SDK_VERSION
/*
 * switch get sdk-version
 */
cparser_result_t cparser_cmd_switch_get_sdk_version(cparser_context_t *context)
{
    uint32  unit = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("SDK       version : %s\n", RT_VERSION_SDK);
    if (strlen(CONFIG_SDK_OS_VERSION) > 0)
        diag_util_mprintf("OS        version : %s\n", CONFIG_SDK_OS_VERSION);
    if (strlen(CONFIG_SDK_COMPILER_VERSION) > 0)
        diag_util_mprintf("Compiler  version : %s\n", CONFIG_SDK_COMPILER_VERSION);
    if (strlen(CONFIG_SDK_LIBC_VERSION) > 0)
        diag_util_mprintf("C Library version : %s\n", CONFIG_SDK_LIBC_VERSION);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_WATCHDOG_PHASE_1_PHASE_2_THRESHOLD_THRESHOLD

/*
 * switch set watchdog ( phase_1 | phase_2 ) threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_switch_set_watchdog_phase_1_phase_2_threshold_threshold(cparser_context_t *context,
    uint32_t *threshold_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    drv_watchdog_threshold_t threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(drv_watchdog_threshold_get(unit, &threshold), ret);

    switch(TOKEN_CHAR(3,6))
    {
        case '1':
            threshold.phase_1_threshold = *threshold_ptr;
            break;

        case '2':
            threshold.phase_2_threshold = *threshold_ptr;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_watchdog_threshold_set(unit, &threshold), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_WATCHDOG_PHASE_1_PHASE_2_THRESHOLD

/*
 * switch get watchdog ( phase_1 | phase_2 ) threshold
 */

cparser_result_t cparser_cmd_switch_get_watchdog_phase_1_phase_2_threshold(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    drv_watchdog_threshold_t threshold;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(drv_watchdog_threshold_get(unit, &threshold), ret);

    switch(TOKEN_CHAR(3,6))
    {
        case '1':
            diag_util_printf("Phase1 threshold : %d\n", threshold.phase_1_threshold);
            break;

        case '2':
            diag_util_printf("Phase2 threshold : %d\n", threshold.phase_2_threshold);
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_KICK_WATCHDOG
/*
 *switch kick watchdog
 */
cparser_result_t cparser_cmd_switch_kick_watchdog(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(drv_watchdog_kick(unit), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_SPECIAL_CONGEST_DRAIN_OUT_THRESHOLD
/*
 * switch get special-congest drain-out-threshold
 */
cparser_result_t cparser_cmd_switch_get_special_congest_drain_out_threshold(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_specialCongestThreshold_get(unit, &thresh), ret);

    diag_util_printf("Special Congest Drain Out Threshold : 0x%X (%u)\n", thresh.high, thresh.high);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_WATCHDOG_SCALE_BITS
/*
 * switch get watchdog scale-bits
 */
cparser_result_t cparser_cmd_switch_get_watchdog_scale_bits(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    drv_watchdog_scale_t scale;
    uint8 *scaleStr[4] = { (uint8 *)"25-bits", (uint8 *)"26-bits", (uint8 *)"27-bits", (uint8 *)"28-bits"};

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(drv_watchdog_scale_get(unit, &scale), ret);
    diag_util_mprintf("\tScale length: %s\n", scaleStr[scale]);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_WATCHDOG_STATE
/*
 * switch get watchdog state
 */
cparser_result_t cparser_cmd_switch_get_watchdog_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(drv_watchdog_enable_get(unit, &enable), ret);
    diag_util_mprintf("\tWatchdog State: %s\n", (enable == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_PPPOE_IP_PARSE_STATE
/*
 * switch get pppoe-ip-parse state
 */
cparser_result_t cparser_cmd_switch_get_pppoe_ip_parse_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_switch_pppoeIpParseEnable_get(unit, &state), ret);
    diag_util_mprintf("\tPPPoE Passthrough State: %s\n", (state == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_CHKSUM_ERR_TYPE_L2_PORT_PORTS_ALL_ACTION_DROP_FORWARD
/*
 * switch set chksum-err-type l2 port ( <PORT_LIST:ports> | all ) action ( drop | forward )
 */
cparser_result_t cparser_cmd_switch_set_chksum_err_type_l2_port_ports_all_action_drop_forward(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_switch_chksum_fail_t type = LAYER2_CHKSUM_FAIL;
    rtk_action_t action = ACTION_DROP;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,1))
    {
        case '2':
            type = LAYER2_CHKSUM_FAIL;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(7,0))
    {
        case 'd':
            action = ACTION_DROP;
            break;

        case 'f':
            action = ACTION_FORWARD;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_chksumFailAction_set(unit, port, type, action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_CRC_RECAL_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * switch set crc-recal port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_crc_recal_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_recalcCRCEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_IPV4_ADDRESS_IP
/*
 * switch set ipv4-address <IPV4ADDR:ip>
 */
cparser_result_t cparser_cmd_switch_set_ipv4_address_ip(cparser_context_t *context,
    uint32_t *ip_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    ipaddr_t ip;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(diag_util_str2ip(&ip, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(rtk_switch_IPv4Addr_set(unit, ip), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_MAC_ADDRESS_MAC
/*
 * switch set mac-address <MACADDR:mac>
 */
cparser_result_t cparser_cmd_switch_set_mac_address_mac(cparser_context_t *context,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_mac_t mac;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(diag_util_str2mac(mac.octet, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(rtk_switch_mgmtMacAddr_set(unit, &mac), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_CPU_MAX_PKT_LEN_RX_DIR_TX_DIR_LENGTH_LEN
/*
 * switch set cpu-max-pkt-len ( rx-dir | tx-dir ) length <UINT:len>
 */
cparser_result_t cparser_cmd_switch_set_cpu_max_pkt_len_rx_dir_tx_dir_length_len(cparser_context_t *context,
    uint32_t *len_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    uint32 len = *len_ptr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('r' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_cpuMaxPktLen_set(unit, PKTDIR_RX, len), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_cpuMaxPktLen_set(unit, PKTDIR_TX, len), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_MAX_PKT_LEN_FE_GE_LENGTH_LEN
/*
 * switch set max-pkt-len ( fe | ge ) length <UINT:len>
 */
cparser_result_t cparser_cmd_switch_set_max_pkt_len_fe_ge_length_len(cparser_context_t *context,
    uint32_t *len_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    uint32 len;
    rtk_switch_maxPktLen_linkSpeed_t speed;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'f':
            speed = MAXPKTLEN_LINK_SPEED_FE;
            break;

        case 'g':
            speed = MAXPKTLEN_LINK_SPEED_GE;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }
    len = *len_ptr;

    DIAG_UTIL_ERR_CHK(rtk_switch_maxPktLenLinkSpeed_set(unit, speed, len), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_MAX_PKT_LEN_TAG_LENGTH_STATE_EXCLUDE_INCLUDE
/*
 * switch set max-pkt-len tag-length-state ( exclude | include )
 */
cparser_result_t cparser_cmd_switch_set_max_pkt_len_tag_length_state_exclude_include(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'e':
            enable = DISABLED;
            break;

        case 'i':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_maxPktLenTagLenCntIncEnable_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_WATCHDOG_SCALE_BITS_25_26_27_28
/*
 * switch set watchdog scale-bits ( 25 | 26 | 27 | 28 )
 */
cparser_result_t cparser_cmd_switch_set_watchdog_scale_bits_25_26_27_28(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    drv_watchdog_scale_t scale;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,1))
    {
        case '5':
            scale = WATCHDOG_SCALE_1;
            break;

        case '6':
            scale = WATCHDOG_SCALE_2;
            break;

        case '7':
            scale = WATCHDOG_SCALE_3;
            break;

        case '8':
            scale = WATCHDOG_SCALE_4;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_watchdog_scale_set(unit, scale), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_WATCHDOG_STATE_DISABLE_ENABLE
/*
 * switch set watchdog state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_watchdog_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_watchdog_enable_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_PKT2CPU_FORWARD_TRAP_FORMAT
/*
 * switch get pkt2cpu ( forward | trap ) format
 */
cparser_result_t
cparser_cmd_switch_get_pkt2cpu_forward_trap_format(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_pktFormat_t             format;
    rtk_switch_pkt2CpuType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(3,0))
    {
        case 'f':
            type = PKT2CPU_TYPE_FORWARD;
            break;

        case 't':
            type = PKT2CPU_TYPE_TRAP;
            break;

        default:
            diag_util_printf("User config type: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_pkt2CpuTypeFormat_get(unit, type, &format), ret);

    diag_util_mprintf("\t%s packet to CPU: ", (type == PKT2CPU_TYPE_FORWARD)? "Forward": "Trap");
    diag_util_mprintf("%s\n", (format == MODIFIED_PACKET) ? "Modified" : "Original");

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_get_pkt2cpu_forward_trap_format */
#endif

#ifdef CMD_SWITCH_GET_PKT2CPU_VLAN_FORMAT
/*
 * switch get pkt2cpu vlan format
 */
cparser_result_t
cparser_cmd_switch_get_pkt2cpu_vlan_format(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_pktFormat_t             format;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_switch_pkt2CpuTypeFormat_get(unit, PKT2CPU_TYPE_VLAN, &format), ret);

    diag_util_mprintf("\tVLAN tag format of packets to CPU: %s\n", (format == MODIFIED_PACKET) ? "Modified" : "Original");

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_get_pkt2cpu_vlan_format */
#endif

#ifdef CMD_SWITCH_SET_SPECIAL_CONGEST_DRAIN_OUT_THRESHOLD_THRESHOLD
/*
 * switch set special-congest drain-out-threshold <UINT:threshold>
 */
cparser_result_t cparser_cmd_switch_set_special_congest_drain_out_threshold_threshold(cparser_context_t *context,
    uint32_t *threshold_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_flowctrl_drop_thresh_t thresh;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    thresh.high = *threshold_ptr;
    thresh.low = *threshold_ptr;

    DIAG_UTIL_ERR_CHK(rtk_flowctrl_specialCongestThreshold_set(unit, &thresh), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_PKT2CPU_FORWARD_TRAP_FORMAT_MODIFIED_ORIGINAL
/*
 * switch set pkt2cpu ( forward | trap ) format ( modified | original )
 */
cparser_result_t
cparser_cmd_switch_set_pkt2cpu_forward_trap_format_modified_original(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_pktFormat_t             format;
    rtk_switch_pkt2CpuType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'f':
            type = PKT2CPU_TYPE_FORWARD;
            break;

        case 't':
            type = PKT2CPU_TYPE_TRAP;
            break;

        default:
            diag_util_printf("User config type: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(5,0))
    {
        case 'm':
            format = MODIFIED_PACKET;
            break;

        case 'o':
            format = ORIGINAL_PACKET;
            break;

        default:
            diag_util_printf("User config format: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_pkt2CpuTypeFormat_set(unit, type, format), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_set_pkt2cpu_forward_trap_format_modified_original */
#endif

#ifdef CMD_SWITCH_SET_PKT2CPU_VLAN_FORMAT_MODIFIED_ORIGINAL
/*
 * switch set pkt2cpu vlan format ( modified | original )
 */
cparser_result_t
cparser_cmd_switch_set_pkt2cpu_vlan_format_modified_original(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_pktFormat_t             format;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch (TOKEN_CHAR(5,0))
    {
        case 'm':
            format = MODIFIED_PACKET;
            break;

        case 'o':
            format = ORIGINAL_PACKET;
            break;

        default:
            diag_util_printf("User config format: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_pkt2CpuTypeFormat_set(unit, PKT2CPU_TYPE_VLAN, format), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_set_pkt2cpu_vlan_format_modified_original */
#endif

#ifdef CMD_SWITCH_GET_ETAGPKT2CPU_FORWARD_TRAP_FORMAT
/*
 * switch get etagpkt2cpu ( forward | trap ) format
 */
cparser_result_t
cparser_cmd_switch_get_etagpkt2cpu_forward_trap_format(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_pktFormat_t             format;
    rtk_switch_pkt2CpuType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(3,0))
    {
        case 'f':
            type = PKT2CPU_ETAG_TYPE_FORWARD;
            break;

        case 't':
            type = PKT2CPU_ETAG_TYPE_TRAP;
            break;

        default:
            diag_util_printf("User config type: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_pkt2CpuTypeFormat_get(unit, type, &format), ret);

    diag_util_mprintf("\t%s E-TAGed packet to CPU: ", (type == PKT2CPU_ETAG_TYPE_FORWARD)? "Forward": "Trap");
    diag_util_mprintf("%s\n", (format == MODIFIED_PACKET) ? "Modified" : "Original");

    return CPARSER_OK;

}   /* end of cparser_cmd_switch_get_etagpkt2cpu_forward_trap_format */
#endif

#ifdef CMD_SWITCH_SET_ETAGPKT2CPU_FORWARD_TRAP_FORMAT_MODIFIED_ORIGINAL
/*
 * switch set etagpkt2cpu ( forward | trap ) format ( modified | original )
 */
cparser_result_t
cparser_cmd_switch_set_etagpkt2cpu_forward_trap_format_modified_original(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_pktFormat_t             format;
    rtk_switch_pkt2CpuType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(3,0))
    {
        case 'f':
            type = PKT2CPU_ETAG_TYPE_FORWARD;
            break;

        case 't':
            type = PKT2CPU_ETAG_TYPE_TRAP;
            break;

        default:
            diag_util_printf("User config type: Error!\n");
            return CPARSER_NOT_OK;
    }

    switch(TOKEN_CHAR(5,0))
    {
        case 'm':
            format = MODIFIED_PACKET;
            break;

        case 'o':
            format = ORIGINAL_PACKET;
            break;

        default:
            diag_util_printf("User config format: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_pkt2CpuTypeFormat_set(unit, type, format), ret);

    return CPARSER_OK;

}   /* end of cparser_cmd_switch_set_etagpkt2cpu_forward_trap_format_modified_original */
#endif


#ifdef CMD_SWITCH_SET_PPPOE_IP_PARSE_STATE_DISABLE_ENABLE
/*
 * switch set pppoe-ip-parse state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_pppoe_ip_parse_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            state = DISABLED;
            break;

        case 'e':
            state = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_pppoeIpParseEnable_set(unit, state), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_CPU_PACKET_TRUNCATE_STATE
/*
 * switch get cpu-packet-truncate state
 */
cparser_result_t cparser_cmd_switch_get_cpu_packet_truncate_state(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_switch_cpuPktTruncateEnable_get(unit, &state), ret);
    diag_util_mprintf("\tCPU Packet Truncation State: %s\n", (state == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_CPU_PACKET_TRUNCATE_STATE_DISABLE_ENABLE
/*
 * switch set cpu-packet-truncate state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_cpu_packet_truncate_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            state = DISABLED;
            break;

        case 'e':
            state = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_cpuPktTruncateEnable_set(unit, state), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_CPU_PACKET_TRUNCATE_LEN
/*
 * switch get cpu-packet-truncate len
 */
cparser_result_t cparser_cmd_switch_get_cpu_packet_truncate_len(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    uint32 len = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_switch_cpuPktTruncateLen_get(unit, &len), ret);
    diag_util_mprintf("\tCPU Packet Truncation Length: %d\n", len);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_CPU_PACKET_TRUNCATE_LEN_LEN
/*
 * switch set cpu-packet-truncate len <UINT:len>
 */
cparser_result_t cparser_cmd_switch_set_cpu_packet_truncate_len_len(cparser_context_t *context,
    uint32_t *len_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_switch_cpuPktTruncateLen_set(unit, *len_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_MAX_PKT_LEN_PORT_PORTS_ALL_FE_GE_LENGTH
/*
 * switch get max-pkt-len port ( <PORT_LIST:ports> | all ) ( fe | ge ) length
 */
cparser_result_t
cparser_cmd_switch_get_max_pkt_len_port_ports_all_fe_ge_length(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t                     portlist;
    rtk_switch_maxPktLen_linkSpeed_t    speed;
    uint32                              unit, port;
    uint32                              len;
    int32                               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(5,0))
    {
        case 'f':
            speed = MAXPKTLEN_LINK_SPEED_FE;
            diag_util_printf("Max Packet Length in 10M/100M Speed\n");
            break;

        case 'g':
            speed = MAXPKTLEN_LINK_SPEED_GE;
            diag_util_printf("Max Packet Length in Giga Speed\n");
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_portMaxPktLenLinkSpeed_get(unit, port, speed, &len), ret);

        diag_util_printf("Port %u : %u\n", port, len);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_get_max_pkt_len_port_ports_all_fe_ge_length */
#endif

#ifdef CMD_SWITCH_SET_OUI_00_00_00_STATE_DISABLE_ENABLE
/*
 * switch set oui-00-00-00 state ( disable | enable )
 */
cparser_result_t cparser_cmd_switch_set_oui_00_00_00_state_disable_enable(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_snapMode_t snapMode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(4,0))
    {
        case 'd':
            snapMode = SNAP_MODE_AAAA03;
            break;

        case 'e':
            snapMode = SNAP_MODE_AAAA03000000;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_snapMode_set(unit, snapMode), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_MAX_PKT_LEN_PORT_PORTS_ALL_FE_GE_LENGTH_LEN
/*
 * switch set max-pkt-len port ( <PORT_LIST:ports> | all ) ( fe | ge ) length <UINT:len>
 */
cparser_result_t
cparser_cmd_switch_set_max_pkt_len_port_ports_all_fe_ge_length_len(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *len_ptr)
{
    diag_portlist_t                     portlist;
    rtk_switch_maxPktLen_linkSpeed_t    speed;
    uint32                              unit, port;
    uint32                              len;
    int32                               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(5,0))
    {
        case 'f':
            speed = MAXPKTLEN_LINK_SPEED_FE;
            break;
        case 'g':
            speed = MAXPKTLEN_LINK_SPEED_GE;
            break;
        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }
    len = *len_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_portMaxPktLenLinkSpeed_set(unit, port, speed, len), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_set_max_pkt_len_port_ports_all_fe_ge_length_len */
#endif

#ifdef CMD_SWITCH_GET_MAX_PKT_LEN_PORT_PORTS_ALL_TAG_LENGTH_STATE
/*
 * switch get max-pkt-len port ( <PORT_LIST:ports> | all ) tag-length-state
 */
cparser_result_t
cparser_cmd_switch_get_max_pkt_len_port_ports_all_tag_length_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_enable_t    enable;
    uint32          unit, port;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("Max Packet Length Tag Length Configuration :\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_portMaxPktLenTagLenCntIncEnable_get(unit, port, &enable), ret);
        diag_util_printf("Port %u : %s\n", port, (enable == ENABLED) ? "Include" : "Exclude");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_get_max_pkt_len_port_ports_all_tag_length_state */
#endif

#ifdef CMD_SWITCH_SET_MAX_PKT_LEN_PORT_PORTS_ALL_TAG_LENGTH_STATE_EXCLUDE_INCLUDE
/*
 * switch set max-pkt-len port ( <PORT_LIST:ports> | all ) tag-length-state ( exclude | include )
 */
cparser_result_t
cparser_cmd_switch_set_max_pkt_len_port_ports_all_tag_length_state_exclude_include(
    cparser_context_t *context,
    char **ports_ptr)
{
    diag_portlist_t portlist;
    rtk_enable_t    enable;
    uint32          unit, port;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'e':
            enable = DISABLED;
            break;

        case 'i':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_switch_portMaxPktLenTagLenCntIncEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_switch_set_max_pkt_len_port_ports_all_tag_length_state_exclude_include */
#endif

#ifdef CMD_SWITCH_GET_FLEXIBLE_TABLE_FORMAT
/*
 * switch get flexible-table format
 */
cparser_result_t
cparser_cmd_switch_get_flexible_table_format(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_switch_flexTblFmt_t  fmt = RTK_SWITCH_FLEX_TBL_FMT_END;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_switch_flexTblFmt_get(unit, &fmt), ret);
    diag_util_mprintf("\tGet flexible table format: %s\n", text_switch_flexTblFmt[fmt]);


    return CPARSER_OK;
}   /* end of cparser_cmd_switch_get_flexible_table_format */
#endif

#ifdef CMD_SWITCH_SET_FLEXIBLE_TABLE_FORMAT_L2_TUNNEL_MPLS_IP_MAC_BINDING_ECID
/*
 * switch set flexible-table format ( l2-tunnel | mpls | ip-mac-binding | ecid )
 */
cparser_result_t
cparser_cmd_switch_set_flexible_table_format_l2_tunnel_mpls_ip_mac_binding_ecid(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_switch_flexTblFmt_t  fmt = RTK_SWITCH_FLEX_TBL_FMT_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('l' == TOKEN_CHAR(4, 0))
    {
        fmt = RTK_SWITCH_FLEX_TBL_FMT_L2_TUNNEL;
    }
    else if ('m' == TOKEN_CHAR(4, 0))
    {
        fmt = RTK_SWITCH_FLEX_TBL_FMT_MPLS;
    }
    else if ('i' == TOKEN_CHAR(4, 0))
    {
        fmt = RTK_SWITCH_FLEX_TBL_FMT_IP_MAC_BINDING;
    }
    else if ('e' == TOKEN_CHAR(4, 0))
    {
        fmt = RTK_SWITCH_FLEX_TBL_FMT_PE_ECID;
    }

    DIAG_UTIL_ERR_CHK(rtk_switch_flexTblFmt_set(unit, fmt), ret);
    diag_util_mprintf("\tSet flexible table format: %s\n", text_switch_flexTblFmt[fmt]);

    return CPARSER_OK;
}/* end of cparser_cmd_switch_set_flexible_table_format_l2_tunnel_mpls_ip_mac_binding_ecid */
#endif

#ifdef CMD_SWITCH_SET_TC_ID_ID_STATE_ENABLE_DISABLE
cparser_result_t cparser_cmd_switch_set_tc_id_id_state_enable_disable(cparser_context_t *context,
    uint32_t *id_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable = ENABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'd':
            enable = DISABLED;
            break;

        case 'e':
            enable = ENABLED;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_tc_enable_set(unit, *id_ptr, enable), ret);

    return CPARSER_OK;

}
#endif

#ifdef CMD_SWITCH_SET_TC_ID_ID_MODE_TIMER_COUNTER
cparser_result_t cparser_cmd_switch_set_tc_id_id_mode_timer_counter(cparser_context_t *context,
    uint32_t *id_ptr)
{
    uint32          unit;
    int32           ret;
    drv_tc_mode_t   mode;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);

    switch(TOKEN_CHAR(6,0))
    {
        case 'c':
            mode = TC_MODE_COUNTER;
            break;

        case 't':
            mode = TC_MODE_TIMER;
            break;

        default:
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(drv_tc_mode_set(unit, *id_ptr, mode), ret);

    return CPARSER_OK;

}
#endif

#ifdef CMD_SWITCH_SET_TC_ID_ID_DIV_FACTOR_FACTOR
cparser_result_t cparser_cmd_switch_set_tc_id_id_div_factor_factor(cparser_context_t *context,
    uint32_t *id_ptr,
    uint32_t *factor_ptr)
{
    uint32          unit;
    int32           ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_tc_divFactor_set(unit, *id_ptr, *factor_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_SET_TC_ID_ID_INIT_VALUE_VALUE
cparser_result_t cparser_cmd_switch_set_tc_id_id_init_value_value(cparser_context_t *context,
    uint32_t *id_ptr,
    uint32_t *value_ptr)
{
    uint32          unit;
    int32           ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_tc_dataInitValue_set(unit, *id_ptr, *value_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SWITCH_GET_TC_ID_ID_COUNTER
cparser_result_t cparser_cmd_switch_get_tc_id_id_counter(cparser_context_t *context,
    uint32_t *id_ptr)
{
    uint32 unit;
    uint32 val;
    int32  ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(drv_tc_counterValue_get(unit, *id_ptr, &val), ret);
    diag_util_printf("usec:%d tv.nsec:%d\n", val/100, (val%100)*10);

    return CPARSER_OK;

}
#endif

