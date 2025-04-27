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
 * Purpose : Definition those trap command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) trap
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
#include <rtk/switch.h>
#include <rtk/trap.h>
#include <diag_util.h>
#include <diag_str.h>
#include <diag_om.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_trap.h>
#endif


const rtk_text_t text_userdefine_cmptype[] =
{
    { /* RMA_CMP_TYPE_MAC */            "MAC Only" },
    { /* RMA_CMP_TYPE_ETHERTYPE */      "Ethertype Only" },
    { /* RMA_CMP_TYPE_MAC_ETHERTYPE */  "MAC and Ethertype" },
};

const rtk_text_t text_trap_act[] =
{
    { /* MGMT_ACTION_FORWARD */                "Forward"  },
    { /* MGMT_ACTION_DROP */                   "Drop"},
    { /* MGMT_ACTION_TRAP2CPU */               "Trap to Cpu"},
    { /* MGMT_ACTION_COPY2CPU */               "Copy to Cpu"},
    { /* MGMT_ACTION_TRAP2MASTERCPU */         "Trap to Master Cpu" },
    { /* MGMT_ACTION_COPY2MASTERCPU */         "Copy to Master Cpu" },
    { /* MGMT_ACTION_FLOOD_TO_ALL_PORT */      "Flood to all port" },
    { /* MGMT_ACTION_END */                    "Error: MGMT_ACTION_END" } 
};

#define DIAG_TRAP_PARSE_MGNT_ACTION(__idx,__act)  \
do{                                 \
    switch(TOKEN_CHAR(__idx, 0)) {  \
        case 'd':                   \
            __act = MGMT_ACTION_DROP;            \
            break;                  \
        case 'f':                   \
            if (8 < osal_strlen(TOKEN_STR(__idx)))   \
                __act = MGMT_ACTION_FLOOD_TO_ALL_PORT; \
            else                                    \
                __act = MGMT_ACTION_FORWARD; \
            break;                  \
        case 't':                   \
            if ('c' == TOKEN_CHAR(__idx, 8))    \
                __act = MGMT_ACTION_TRAP2CPU;        \
            else if ('m' == TOKEN_CHAR(__idx, 8))   \
                __act = MGMT_ACTION_TRAP2MASTERCPU;  \
            else                                    \
                __act = MGMT_ACTION_END;            \
            break;                  \
        case 'c':                               \
            if ('c' == TOKEN_CHAR(__idx, 8))    \
                __act = MGMT_ACTION_COPY2CPU;        \
            else if ('m' == TOKEN_CHAR(__idx, 8))   \
                __act = MGMT_ACTION_COPY2MASTERCPU;  \
            else                                    \
                __act = MGMT_ACTION_END;            \
            break;                              \
        default:                                \
                __act = MGMT_ACTION_END;  \
            break;                              \
    }                                           \
}while(0);

#ifdef CMD_TRAP_GET_CFM_PRIORITY
/*
 * trap get cfm priority
 */
cparser_result_t
cparser_cmd_trap_get_cfm_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_cfmFrameTrapPri_get(unit, &pri), ret);
    diag_util_mprintf("CFM Trap Priority       : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_cfm_priority */
#endif

#ifdef CMD_TRAP_GET_OAMPDU_INFO
/*
 * trap get oampdu info
 */
cparser_result_t
cparser_cmd_trap_get_oampdu_info(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;
    #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    rtk_pri_t       pri = 0;
    #endif  /* defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390) */

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_oamPDUAction_get(unit, &action), ret);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("OAM PDU:\n");
    diag_util_mprintf("\tAction: ");
    if (ACTION_FORWARD == action)
        diag_util_mprintf("Forward");
    else if (ACTION_DROP == action)
        diag_util_mprintf("Drop");
    else if (ACTION_TRAP2CPU == action)
        diag_util_mprintf("Trap");
    else
        diag_util_mprintf("%d", action);
    diag_util_mprintf("\n");

    #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_oamPDUPri_get(unit, &pri), ret);
        diag_util_mprintf("\tPriority: %d\n", pri);
    }
    #endif  /* defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390) */

    return CPARSER_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
int32 cmd_dump_maple_cypress_rma_l2_user_define(uint32 unit)
{
    uint32                  rma_index = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = MGMT_ACTION_FORWARD;
    rtk_trap_userDefinedRma_t   userDefinedRma;
    rtk_enable_t            enable;
    uint32                  bypassStpFieldIdx[] = {BYPASS_STP_TYPE_USER_DEF_0,
                                                   BYPASS_STP_TYPE_USER_DEF_1};

    diag_util_mprintf("  Index | Mac address      | Mac Address Mask  | Action     | Learn     | Stp Block\n");
    diag_util_mprintf("--------+------------------+-------------------+------------+-----------+---------------\n");

    for (rma_index = 0; rma_index < 2; rma_index++)
    {
        osal_memset(&userDefinedRma, 0, sizeof(rtk_trap_userDefinedRma_t));
        DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRma_get(unit, rma_index, &userDefinedRma), ret);
        diag_util_printf("  %u  ", rma_index);
        diag_util_printf("    %02X:%02X:%02X:%02X:%02X:%02X ",userDefinedRma.mac.octet[0],userDefinedRma.mac.octet[1],
            userDefinedRma.mac.octet[2], userDefinedRma.mac.octet[3],userDefinedRma.mac.octet[4],userDefinedRma.mac.octet[5]);
        diag_util_printf("   %02X:%02X:%02X:%02X:%02X:%02X  ",userDefinedRma.macMask.octet[0],userDefinedRma.macMask.octet[1],
            userDefinedRma.macMask.octet[2], userDefinedRma.macMask.octet[3],userDefinedRma.macMask.octet[4],
            userDefinedRma.macMask.octet[5]);

        DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRmaAction_get(unit, rma_index, &rma_action), ret);
        if (MGMT_ACTION_FORWARD == rma_action)
        {
            diag_util_printf(" forward     ");
        }
        else if (MGMT_ACTION_DROP == rma_action)
        {
            diag_util_printf(" drop        ");
        }
        else if (MGMT_ACTION_TRAP2CPU == rma_action)
        {
            diag_util_printf(" trap-to-cpu ");
        }
        else if (MGMT_ACTION_FLOOD_TO_ALL_PORT == rma_action)
        {
            diag_util_printf(" forward-and-flood ");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRmaLearningEnable_get(unit,
                rma_index, &enable), ret);
        if(ENABLED == enable)
            diag_util_printf(" enabled ");
        else if(DISABLED == enable)
            diag_util_printf(" disabled ");
        else
            diag_util_printf(" disabled ");

        DIAG_UTIL_ERR_CHK(rtk_trap_bypassStp_get(unit,
                bypassStpFieldIdx[rma_index], &enable), ret);
        if(ENABLED == enable)
            diag_util_printf(" enabled\n");
        else if(DISABLED == enable)
            diag_util_printf(" disabled\n");
        else
            diag_util_printf(" disabled\n");
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
int32 cmd_dump_longan_mango_rma_l2_user_define(uint32 unit)
{
    uint32                  rma_index = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = MGMT_ACTION_FORWARD;
    rtk_trap_userDefinedRma_t   userDefinedRma;
    rtk_enable_t            enable;

    for (rma_index = 0; rma_index < 4; rma_index++)
    {
        osal_memset(&userDefinedRma, 0, sizeof(rtk_trap_userDefinedRma_t));

        DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRma_get(unit, rma_index, &userDefinedRma), ret);
        DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRmaEnable_get(unit, rma_index, &enable), ret);

        diag_util_printf("\n");
        diag_util_printf("Index        :  %u\n", rma_index);
        diag_util_printf("State        :  %s\n", (ENABLED == enable)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        diag_util_printf("Compare Type :  %s\n", text_userdefine_cmptype[userDefinedRma.cmpType].text);
        diag_util_printf("Min MAC      :  %02X:%02X:%02X:%02X:%02X:%02X\n",userDefinedRma.mac_min.octet[0],userDefinedRma.mac_min.octet[1],
            userDefinedRma.mac_min.octet[2], userDefinedRma.mac_min.octet[3], userDefinedRma.mac_min.octet[4], userDefinedRma.mac_min.octet[5]);
        diag_util_printf("Max MAC      :  %02X:%02X:%02X:%02X:%02X:%02X\n",userDefinedRma.mac_max.octet[0],userDefinedRma.mac_max.octet[1],
            userDefinedRma.mac_max.octet[2], userDefinedRma.mac_max.octet[3], userDefinedRma.mac_max.octet[4], userDefinedRma.mac_max.octet[5]);
        diag_util_printf("Ethertype    :  %x\n", userDefinedRma.frameValue);
        diag_util_printf("Action       :  ");

        DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRmaAction_get(unit, rma_index, &rma_action), ret);
        if (MGMT_ACTION_FORWARD == rma_action)
        {
            diag_util_printf("forward     ");
        }
        else if (MGMT_ACTION_DROP == rma_action)
        {
            diag_util_printf("drop        ");
        }
        else if (MGMT_ACTION_TRAP2CPU == rma_action)
        {
            diag_util_printf("trap-to-cpu ");
        }
        else if (MGMT_ACTION_FLOOD_TO_ALL_PORT == rma_action)
        {
            diag_util_printf("forward-and-flood");
        }
        else if (MGMT_ACTION_TRAP2MASTERCPU == rma_action)
        {
            diag_util_printf("trap-to-master");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
        diag_util_printf("\n");

        DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRmaLearningEnable_get(unit,rma_index, &enable), ret);
        diag_util_printf("Learn        :  %s\n",(ENABLED == enable)? DIAG_STR_ENABLE:DIAG_STR_DISABLE);

        DIAG_UTIL_ERR_CHK(rtk_trap_bypassStp_get(unit, rma_index, &enable), ret);
        diag_util_printf("Bypass STP   :  %s\n",(ENABLED == enable)? DIAG_STR_ENABLE:DIAG_STR_DISABLE);

        DIAG_UTIL_ERR_CHK(rtk_trap_bypassVlan_get(unit, rma_index, &enable), ret);
        diag_util_printf("Bypass VLAN  :  %s\n",(ENABLED == enable)? DIAG_STR_ENABLE:DIAG_STR_DISABLE);
    }

    return RT_ERR_OK;
}
#endif

#ifdef CMD_TRAP_DUMP_RMA_L2_USER_DEFINE
/*
 * trap dump rma l2-user-define
 */
cparser_result_t
cparser_cmd_trap_dump_rma_l2_user_define(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
             DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(cmd_dump_maple_cypress_rma_l2_user_define(unit),ret);
        }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
        if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(cmd_dump_longan_mango_rma_l2_user_define(unit),ret);
        }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_DUMP_RMA_LAYER2
/*
 * trap dump rma layer2
 */
cparser_result_t
cparser_cmd_trap_dump_rma_layer2(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = MGMT_ACTION_FORWARD;
    rtk_enable_t            enable;

#if defined(CONFIG_SDK_RTL8380)
    rtk_trap_rmaGroup_frameType_t rmaGroup_frameType;
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32                  rma_index = 0;
    rtk_mac_t               rma_frame;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Multicast Address frame | Action      | Learn\n");
        diag_util_mprintf("------------------------+-------------+-----------\n");
        for (rmaGroup_frameType = RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM;
                rmaGroup_frameType < RMA_GROUP_TYPE_END; rmaGroup_frameType++)
        {
            ret = rtk_trap_rmaGroupAction_get(unit, rmaGroup_frameType, &rma_action);
            if (ret == RT_ERR_CHIP_NOT_SUPPORTED)
                continue;

            if (ret != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }

            /*RMA Type*/
            if(rmaGroup_frameType == RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM)
            {
                diag_util_printf("   OAM   ");
            }
            else if(rmaGroup_frameType == RMA_GROUP_TYPE_SLOW_PROTOCOL_OTHER)
            {
                diag_util_printf("   rma-02");
            }
            else if(rmaGroup_frameType == RMA_GROUP_TYPE_03)
            {
                diag_util_printf("   rma-03");
            }
            else if(rmaGroup_frameType == RMA_GROUP_TYPE_0E_EXCEPT_PTP_LLDP)
            {
                diag_util_printf("   rma-0E");
            }
            else if(rmaGroup_frameType == RMA_GROUP_TYPE_10)
            {
                diag_util_printf("   rma-10");
            }
            else if(rmaGroup_frameType == RMA_GROUP_TYPE_GMRP)
            {
                diag_util_printf("   rma-20");
            }
            else if(rmaGroup_frameType == RMA_GROUP_TYPE_GVRP)
            {
                diag_util_printf("   rma-21");
            }
            else if(rmaGroup_frameType == RMA_GROUP_TYPE_MSRP)
            {
                diag_util_printf("   rma-22");
            }
            else if(rmaGroup_frameType == RMA_GROUP_TYPE_0X)
            {
                diag_util_printf("   rma-0X");
            }
            else if(rmaGroup_frameType == RMA_GROUP_TYPE_1X)
            {
                diag_util_printf("   rma-1X");
            }
            else if(rmaGroup_frameType == RMA_GROUP_TYPE_2X)
            {
                diag_util_printf("   rma-2X");
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }

            /*RMA Action*/
            if (MGMT_ACTION_FORWARD == rma_action)
            {
                diag_util_printf(" forward     ");
            }
            else if (MGMT_ACTION_DROP == rma_action)
            {
                diag_util_printf(" drop        ");
            }
            else if (MGMT_ACTION_TRAP2CPU == rma_action)
            {
                diag_util_printf(" trap-to-cpu ");
            }
            else if (MGMT_ACTION_FLOOD_TO_ALL_PORT == rma_action)
            {
                diag_util_printf(" forward-and-flood ");
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
            DIAG_UTIL_ERR_CHK(rtk_trap_rmaGroupLearningEnable_get(unit, rmaGroup_frameType,
                   &enable), ret);
            if(ENABLED == enable)
                diag_util_printf(" enabled\n");
            else if(DISABLED == enable)
                diag_util_printf(" disabled\n");
            else
                diag_util_printf(" disabled\n");
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        rma_frame.octet[0] = 0x01;
        rma_frame.octet[1] = 0x80;
        rma_frame.octet[2] = 0xc2;
        rma_frame.octet[3] = 0x00;
        rma_frame.octet[4] = 0x00;

        diag_util_mprintf("Multicast Address frame | Action         | Learn\n");
        diag_util_mprintf("------------------------+----------------+-----------\n");
        for (rma_index = 1; rma_index <= 0x2f; rma_index++)
        {
            rma_frame.octet[5] = rma_index;
            ret = rtk_trap_rmaAction_get(unit, &rma_frame, &rma_action);
            if (ret == RT_ERR_CHIP_NOT_SUPPORTED)
                continue;

            if (ret != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
            diag_util_printf("   %02X:%02X:%02X:%02X:%02X:%02X     ",rma_frame.octet[0],rma_frame.octet[1],rma_frame.octet[2],
                                                           rma_frame.octet[3],rma_frame.octet[4],rma_frame.octet[5]);
            if (MGMT_ACTION_FORWARD == rma_action)
            {
                diag_util_printf(" forward        ");
            }
            else if (MGMT_ACTION_DROP == rma_action)
            {
                diag_util_printf(" drop           ");
            }
            else if (MGMT_ACTION_TRAP2CPU == rma_action)
            {
                diag_util_printf(" trap-to-cpu    ");
            }
            else if (MGMT_ACTION_COPY2CPU == rma_action)
            {
                diag_util_printf(" copy-to-cpu    ");
            }
            else if (MGMT_ACTION_TRAP2MASTERCPU == rma_action)
            {
                diag_util_printf(" trap-to-master ");
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }

            DIAG_UTIL_ERR_CHK(rtk_trap_rmaLearningEnable_get(unit, &rma_frame, &enable), ret);

            if(ENABLED == enable)
                diag_util_printf(" enabled\n");
            else
                diag_util_printf(" disabled\n");
        }
    }
#endif
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_EAPOL
/*
 * trap get eapol
 */
cparser_result_t cparser_cmd_trap_get_eapol(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action = MGMT_ACTION_END;
    rtk_enable_t        enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_get(unit, MGMT_TYPE_EAPOL, &action), ret);
    diag_util_printf("EAPOL Action :");
    if (MGMT_ACTION_FORWARD == action)
    {
        diag_util_printf(" forward      \n");
    }
    else if (MGMT_ACTION_TRAP2CPU == action)
    {
        diag_util_printf(" trap-to-cpu  \n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassStp_get(unit, BYPASS_STP_TYPE_EAPOL, &enable), ret);
    diag_util_printf("EAPOL Bypass STP Status : %s \n", (ENABLED == enable) ? "Enabled " : DIAG_STR_DISABLE);
    DIAG_UTIL_ERR_CHK(rtk_trap_bypassVlan_get(unit, BYPASS_VLAN_TYPE_EAPOL, &enable), ret);
    diag_util_printf("EAPOL Bypass VLAN Status : %s \n", (ENABLED == enable) ? "Enabled " : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_ARP_REQUEST
/*
 * trap get arp-request
 */
cparser_result_t cparser_cmd_trap_get_arp_request(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_get(unit, MGMT_TYPE_ARP, &action), ret);

    diag_util_printf("ARP Request Action :");
    if (MGMT_ACTION_FORWARD == action)
    {
        diag_util_printf(" forward      \n");
    }
    else if (MGMT_ACTION_COPY2CPU == action)
    {
        diag_util_printf("  copy-to-cpu  \n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_NEIGHBOR_DISCOVERY
/*
 * trap get neighbor-discovery
 */
cparser_result_t cparser_cmd_trap_get_neighbor_discovery(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_get(unit, MGMT_TYPE_IPV6ND, &action), ret);

    diag_util_printf("IPv6 Neighbor Discovery Action :");
    if (MGMT_ACTION_FORWARD == action)
    {
        diag_util_printf(" forward      \n");
    }
    else if (MGMT_ACTION_TRAP2CPU == action)
    {
        diag_util_printf(" trap-to-cpu  \n");
    }
#if defined(CONFIG_SDK_RTL8380)
    else if (MGMT_ACTION_COPY2CPU == action)
    {
        diag_util_printf("  copy-to-cpu  ");
    }
#endif
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_HOP_BY_HOP_POSITION_ACTION
/*
 * trap get hop-by-hop-position action
 */
cparser_result_t cparser_cmd_trap_get_hop_by_hop_position_action(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_get(unit, MGMT_TYPE_IPV6_HOP_POS_ERR, &action), ret);

    diag_util_printf("IPv6 Hop-By-Hop NexHeader Position Error Action :");
    if (MGMT_ACTION_FORWARD == action)
    {
        diag_util_printf(" forward      \n");
    }
    else if (MGMT_ACTION_TRAP2CPU == action)
    {
        diag_util_printf(" trap-to-cpu  \n");
    }
    else
    {
        diag_util_printf("Error action is returned!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_IP6_HOP_BY_HOP_POSITION_PRIORITY
/*
 * trap get ip6 hop-by-hop-position priority
 */
cparser_result_t cparser_cmd_trap_get_ip6_hop_by_hop_position_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_IPV6_HOP_POS_ERR, &pri), ret);
    diag_util_mprintf("IPv6 Hop-by-Hop Position Error Trap Priority       : %u\n", pri);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_IP6_UNKNOWN_HEADER_ACTION
/*
 * trap get ip6-unknown-header action
 */
cparser_result_t cparser_cmd_trap_get_ip6_unknown_header_action(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_get(unit, MGMT_TYPE_IPV6_HDR_UNKWN, &action), ret);

    diag_util_printf("IPv6 Unknown Extension Header Action :");
    if (MGMT_ACTION_FORWARD == action)
    {
        diag_util_printf(" forward      \n");
    }
    else if (MGMT_ACTION_TRAP2CPU == action)
    {
        diag_util_printf(" trap-to-cpu  \n");
    }
    else
    {
        diag_util_printf("Error action is returned!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_IP6_IP6_UNKNOWN_HEADER_PRIORITY
/*
 * trap get ip6 ip6-unknown-header priority
 */
cparser_result_t cparser_cmd_trap_get_ip6_ip6_unknown_header_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_IPV6_HDR_UNKWN, &pri), ret);
    diag_util_mprintf("IPv6 Unknown Extension Header Trap Priority       : %u\n", pri);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_INVALID_SA_PRIORITY
/*
 * trap get invalid-sa priority
 */
cparser_result_t cparser_cmd_trap_get_invalid_sa_priority(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_INVALID_SA, &pri), ret);
    diag_util_mprintf("Invalid(Broadcast,Multicast,All Zero) SA Trap Priority       : %u\n", pri);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_CRC_ERROR_ACTION
/*
 * trap get crc-error action
 */
cparser_result_t cparser_cmd_trap_get_crc_error_action(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_get(unit, MGMT_TYPE_L2_CRC_ERR, &action), ret);

    diag_util_printf("L2 CRC Error Action :");
    if (MGMT_ACTION_DROP == action)
    {
        diag_util_printf(" drop      \n");
    }
    else if (MGMT_ACTION_FORWARD == action)
    {
        diag_util_printf(" forward      \n");
    }
    else if (MGMT_ACTION_TRAP2CPU == action)
    {
        diag_util_printf(" trap-to-cpu  \n");
    }
    else
    {
        diag_util_printf("Error action is returned!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_IP4_CHECKSUM_ERROR_ACTION
/*
 * trap get ip4-checksum-error action
 */
cparser_result_t cparser_cmd_trap_get_ip4_checksum_error_action(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_get(unit, MGMT_TYPE_IP4_CHKSUM_ERR, &action), ret);

    diag_util_printf("IPv4 Checksum Error Action :");
    if (MGMT_ACTION_DROP == action)
    {
        diag_util_printf(" drop      \n");
    }
    else if (MGMT_ACTION_FORWARD == action)
    {
        diag_util_printf(" forward      \n");
    }
    else if (MGMT_ACTION_TRAP2CPU == action)
    {
        diag_util_printf(" trap-to-cpu  \n");
    }
    else
    {
        diag_util_printf("Error action is returned!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_REASON_1X_EAPOL_PRIORITY
/*
 * trap get reason 1x-eapol priority
 */
cparser_result_t cparser_cmd_trap_get_reason_1x_eapol_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_EAPOL, &pri), ret);
    diag_util_mprintf("dot1x EAPOL Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_1x_eapol_priority */
#endif

#ifdef CMD_TRAP_GET_REASON_ARP_IPV6_ND_PRIORITY
/*
 * trap get reason ( arp | ipv6-nd ) priority
 */
cparser_result_t cparser_cmd_trap_get_reason_arp_ipv6_nd_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('a' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_ARP, &pri), ret);
        diag_util_mprintf("ARP Trap Priority : %u\n", pri);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_IPV6ND, &pri), ret);
        diag_util_mprintf("IPv6 Neighbor Discovery Trap Priority : %u\n", pri);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_arp_ipv6_nd_priority */
#endif

#ifdef CMD_TRAP_GET_REASON_BPDU_LACP_LLDP_PRIORITY
/*
 * trap get reason ( bpdu | lacp | lldp ) priority
 */
cparser_result_t cparser_cmd_trap_get_reason_bpdu_lacp_lldp_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('b' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_BPDU, &pri), ret);
        diag_util_mprintf("BPDU Trap Priority : %u\n", pri);
    }
    else if ('a' == TOKEN_CHAR(3,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_LACP, &pri), ret);
        diag_util_mprintf("LACP Trap Priority : %u\n", pri);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_LLDP, &pri), ret);
        diag_util_mprintf("LLDP Trap Priority : %u\n", pri);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_bpdu_lacp_lldp_priority */
#endif

#ifdef CMD_TRAP_GET_REASON_INGRESS_VLAN_FILTER_PRIORITY
/*
 * trap get reason ingress-vlan-filter priority
 */
cparser_result_t cparser_cmd_trap_get_reason_ingress_vlan_filter_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_IGR_VLAN_FLTR, &pri), ret);
    diag_util_mprintf("Ingress-VLAN-Filter Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_ingress_vlan_filter_priority */
#endif

#ifdef CMD_TRAP_GET_REASON_IPV4_IGMP_IPV6_MLD_PRIORITY
/*
 * trap get reason ( ipv4-igmp | ipv6-mld ) priority
 */
cparser_result_t cparser_cmd_trap_get_reason_ipv4_igmp_ipv6_mld_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('4' == TOKEN_CHAR(3,3))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_IGMP, &pri), ret);
        diag_util_mprintf("IPv4-IGMP Trap Priority : %u\n", pri);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_MLD, &pri), ret);
        diag_util_mprintf("IPv6-MLD Trap Priority : %u\n", pri);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_ipv4_igmp_ipv6_mld_priority */
#endif

#ifdef CMD_TRAP_GET_REASON_OTHER_PRIORITY
/*
 * trap get reason other priority
 */
cparser_result_t cparser_cmd_trap_get_reason_other_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_OTHER, &pri), ret);
    diag_util_mprintf("Other Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_other_priority */
#endif

#ifdef CMD_TRAP_GET_REASON_PTP_PRIORITY
/*
 * trap get reason ptp priority
 */
cparser_result_t cparser_cmd_trap_get_reason_ptp_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_PTP, &pri), ret);
    diag_util_mprintf("PTP Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_ptp_priority */
#endif

#ifdef CMD_TRAP_GET_REASON_SWITCH_MAC_PRIORITY
/*
 * trap get reason switch-mac priority
 */
cparser_result_t cparser_cmd_trap_get_reason_switch_mac_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_SELFMAC, &pri), ret);
    diag_util_mprintf("Switch-MAC Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_switch_mac_priority */
#endif

#ifdef CMD_TRAP_GET_REASON_UNKNOWN_DA_PRIORITY
/*
 * trap get reason unknown-da priority
 */
cparser_result_t cparser_cmd_trap_get_reason_unknown_da_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_UNKNOWN_DA, &pri), ret);
    diag_util_mprintf("Unknown-DA Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_unknown_da_priority */
#endif

#ifdef CMD_TRAP_GET_REASON_VLAN_ERROR_PRIORITY
/*
 * trap get reason vlan-error priority
 */
cparser_result_t cparser_cmd_trap_get_reason_vlan_error_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_VLAN_ERR, &pri), ret);
    diag_util_mprintf("VLAN-Error Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_vlan_error_priority */
#endif

#ifdef CMD_TRAP_GET_RMA_PRIORITY
/*
 * trap get rma priority
 */
cparser_result_t cparser_cmd_trap_get_rma_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_RMA, &pri), ret);
    diag_util_mprintf("RMA Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_rma_priority */
#endif

#ifdef CMD_TRAP_GET_SWITCH_MAC
/*
 * trap get switch-mac
 */
cparser_result_t cparser_cmd_trap_get_switch_mac(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_get(unit, MGMT_TYPE_SELFMAC, &action), ret);

    diag_util_printf("Switch Self Mac Action :");
    if (MGMT_ACTION_FORWARD == action)
    {
        diag_util_printf(" forward      \n");
    }
    else if (MGMT_ACTION_DROP == action)
    {
        diag_util_printf(" drop         \n");
    }
    else if (MGMT_ACTION_TRAP2CPU == action)
    {
        diag_util_printf(" trap-to-cpu  \n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_CFM_PRIORITY_PRIORITY
/*
 * trap set cfm priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_cfm_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_cfmFrameTrapPri_set(unit, pri), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_cfm_priority_priority */
#endif

#ifdef CMD_TRAP_SET_OAMPDU_DROP_FORWARD_TRAP_TO_CPU
/*
 * trap set oampdu ( drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_oampdu_drop_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action = ACTION_TRAP2CPU;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('f' == TOKEN_CHAR(3,0))
    {
        action = ACTION_FORWARD;
    }
    else if ('t' == TOKEN_CHAR(3,0))
    {
        action = ACTION_TRAP2CPU;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        action = ACTION_DROP;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_oamPDUAction_set(unit, action), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_oampdu_drop_forward_trap_to_cpu */
#endif

#ifdef CMD_TRAP_SET_OAMPDU_PRIORITY_PRIORITY
/*
 * trap set oampdu priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_oampdu_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_oamPDUPri_set(unit, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_oampdu_port_all_priority_priority */
#endif

#ifdef CMD_TRAP_GET_CFI
/*
  * trap get cfi
  */
cparser_result_t cparser_cmd_trap_get_cfi(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t op1,op2;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_pktWithCFIAction_get(unit, &op1) , ret);
    DIAG_UTIL_ERR_CHK(rtk_trap_pktWithOuterCFIAction_get(unit, &op2) , ret);

    diag_util_mprintf("CFI configuration\n");
    diag_util_mprintf("  Inner CFI operation : ");
    if(op1 == ACTION_FORWARD)
        diag_util_mprintf("Forward\n");
    else if(op1 == ACTION_DROP)
        diag_util_mprintf("Drop\n");
    else if(op1 == ACTION_TRAP2CPU)
        diag_util_mprintf("Trap to CPU\n");
    else
        diag_util_mprintf("Trap to Master\n");

    diag_util_mprintf("  Outer CFI operation : ");
    if(op2 == ACTION_FORWARD)
        diag_util_mprintf("Forward\n");
    else if(op2 == ACTION_DROP)
        diag_util_mprintf("Drop\n");
    else if(op2 == ACTION_TRAP2CPU)
        diag_util_mprintf("Trap to CPU\n");
    else
        diag_util_mprintf("Trap to Master\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_CFI_INNER_OUTER_FORWARD_DROP_TRAP
/*
 * trap set cfi ( inner | outer ) ( forward | drop | trap )
 */
cparser_result_t
cparser_cmd_trap_set_cfi_inner_outer_forward_drop_trap(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t op;
    int32   (*fp)(uint32, rtk_action_t);

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(3, 0))
    {
        fp = rtk_trap_pktWithCFIAction_set;
    }
    else
    {
        fp = rtk_trap_pktWithOuterCFIAction_set;
    }

    if('f' == TOKEN_CHAR(4, 0))
        {
        op = ACTION_FORWARD;
    }
    else if('d' == TOKEN_CHAR(4, 0))
    {
        op = ACTION_DROP;
    }
    else
    {
        op = ACTION_TRAP2CPU;
    }

    DIAG_UTIL_ERR_CHK(fp(unit, op) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_CFI_INNER_OUTER_TRAP_TO_MASTER
/*
 * trap set cfi ( inner | outer ) trap-to-master
 */
cparser_result_t
cparser_cmd_trap_set_cfi_inner_outer_trap_to_master(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    int32   (*fp)(uint32, rtk_action_t);

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(3, 0))
    {
        fp = rtk_trap_pktWithCFIAction_set;
    }
    else
    {
        fp = rtk_trap_pktWithOuterCFIAction_set;
    }

    DIAG_UTIL_ERR_CHK(fp(unit, ACTION_TRAP2MASTERCPU) , ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_TRAP_GET_CFI_PRIORITY
/*
 * trap get cfi priority
 */
cparser_result_t
cparser_cmd_trap_get_cfi_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_pktWithCFIPri_get(unit, &pri), ret);
    diag_util_mprintf("Packet with CFI Trap Priority       : %u\n", pri);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_CFI_PRIORITY_PRIORITY
/*
 * trap set cfi priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_trap_set_cfi_priority_priority(cparser_context_t *context,
        uint32_t *priority_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trap_pktWithCFIPri_set(unit, pri), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_cfi_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_1X_EAPOL_PRIORITY_PRIORITY
/*
 * trap set reason 1x-eapol priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_reason_1x_eapol_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_EAPOL, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_1x_eapol_priority_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_ARP_IPV6_ND_PRIORITY_PRIORITY
/*
 * trap set reason ( arp | ipv6-nd ) priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_reason_arp_ipv6_nd_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;

    if ('a' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_ARP, pri), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_IPV6ND, pri), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_arp_ipv6_nd_priority_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_BPDU_LACP_LLDP_PRIORITY_PRIORITY
/*
 * trap set reason ( bpdu | lacp | lldp ) priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_reason_bpdu_lacp_lldp_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;

    if ('b' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_BPDU, pri), ret);
    }
    else if ('a' == TOKEN_CHAR(3,1))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_LACP, pri), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_LLDP, pri), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_bpdu_lacp_lldp_priority_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_INGRESS_VLAN_FILTER_PRIORITY_PRIORITY
/*
 * trap set reason ingress-vlan-filter priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_reason_ingress_vlan_filter_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_IGR_VLAN_FLTR, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_ingress_vlan_filter_priority_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_IPV4_IGMP_IPV6_MLD_PRIORITY_PRIORITY
/*
 * trap set reason ( ipv4-igmp | ipv6-mld ) priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_reason_ipv4_igmp_ipv6_mld_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;

    if ('4' == TOKEN_CHAR(3,3))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_IGMP, pri), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_MLD, pri), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_ipv4_igmp_ipv6_mld_priority_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_OTHER_PRIORITY_PRIORITY
/*
 * trap set reason other priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_reason_other_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_OTHER, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_other_priority_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_PTP_PRIORITY_PRIORITY
/*
 * trap set reason ptp priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_reason_ptp_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_PTP, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_ptp_priority_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_SWITCH_MAC_PRIORITY_PRIORITY
/*
 * trap set reason switch-mac priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_reason_switch_mac_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_SELFMAC, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_switch_mac_priority_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_UNKNOWN_DA_PRIORITY_PRIORITY
/*
 * trap set reason unknown-da priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_reason_unknown_da_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_UNKNOWN_DA, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_unknown_da_priority_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_VLAN_ERROR_PRIORITY_PRIORITY
/*
 * trap set reason vlan-error priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_reason_vlan_error_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_VLAN_ERR, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_vlan_error_priority_priority */
#endif

#ifdef CMD_TRAP_SET_RMA_PRIORITY_PRIORITY
/*
 * trap set rma priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_rma_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_RMA, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_rma_priority_priority */
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_INDEX_ACTION_DROP_FORWARD_TRAP_TO_CPU_FORWARD_AND_FLOOD
/*
 * trap set rma l2-user-define <UINT:index> action ( drop | forward | trap-to-cpu | forward-and-flood )
 */
cparser_result_t
cparser_cmd_trap_set_rma_l2_user_define_index_action_drop_forward_trap_to_cpu_forward_and_flood(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                  unit = 0;
    uint32                  index = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = MGMT_ACTION_FORWARD;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    index = *index_ptr;

    DIAG_TRAP_PARSE_MGNT_ACTION(6, rma_action);

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRmaAction_set(unit, index, rma_action), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_INDEX_ACTION_TRAP_TO_MASTER
/*
 * trap set rma l2-user-define <UINT:index> action trap-to-master
 */
cparser_result_t
cparser_cmd_trap_set_rma_l2_user_define_index_action_trap_to_master(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                  unit = 0;
    uint32                  index = 0;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRmaAction_set(unit, index, MGMT_ACTION_TRAP2MASTERCPU), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_RMA_L2_USER_DEFINE_FLOOD_PORTMASK
/*
 * trap get rma l2-user-define flood-portmask
 */
cparser_result_t
cparser_cmd_trap_get_rma_l2_user_define_flood_portmask(cparser_context_t *context)
{
    uint32            unit = 0;
    int32             ret = RT_ERR_FAILED;
    char              port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_portmask_t    flood_portmask;

    osal_memset(&flood_portmask, 0, sizeof(rtk_portmask_t));
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineFloodPortmask_get(unit, &flood_portmask), ret);

    diag_util_lPortMask2str(port_list, &flood_portmask);
    diag_util_mprintf("  User defined flood portmask\t: %s \n", port_list);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_FLOOD_PORTMASK_PORT_ALL
/*
 * trap set rma l2-user-define flood-portmask ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_set_rma_l2_user_define_flood_portmask_port_all(
    cparser_context_t *context,
    char **port_ptr)
{
    uint32            unit = 0;
    int32             ret = RT_ERR_FAILED;
    diag_portlist_t   portlist;
    rtk_portmask_t    flood_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    osal_memset(&flood_portmask, 0 , sizeof(rtk_portmask_t));
    osal_memcpy(&flood_portmask, &portlist.portmask, sizeof(rtk_portmask_t));
    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineFloodPortmask_set(unit, &flood_portmask), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_INDEX_STATE_DISABLE_ENABLE
/*
 * trap set rma l2-user-define <UINT:index> state ( disable | enable )
 */
cparser_result_t cparser_cmd_trap_set_rma_l2_user_define_index_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    uint32  index;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    index = *index_ptr;

    if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else
    {
        enable = ENABLED;
    }
    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRmaEnable_set(unit, index, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_RMA_L2_USER_DEFINE_INDEX_STATE
/*
 * trap get rma l2-user-define <UINT:index> state
 */
cparser_result_t cparser_cmd_trap_get_rma_l2_user_define_index_state(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    uint32  index;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRmaEnable_get(unit, index, &enable), ret);
    diag_util_mprintf("User-Defined RMA %d : %s\n", index, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_INDEX_MAC
/*
 * trap set rma l2-user-define <UINT:index> <MACADDR:mac>
 */
cparser_result_t cparser_cmd_trap_set_rma_l2_user_define_index_mac(cparser_context_t *context,
    uint32_t *index_ptr, cparser_macaddr_t *mac_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_trap_userDefinedRma_t   userDefinedRma;
    uint32  index;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&userDefinedRma, 0, sizeof(rtk_trap_userDefinedRma_t));

    index = atoi(TOKEN_STR(4));
    osal_memcpy(&userDefinedRma.mac.octet[0], &mac_ptr->octet[0], sizeof(cparser_macaddr_t));

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRma_set(unit, index, &userDefinedRma), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_INDEX_MAC_MAC_MIN_MAC_MAX
/*
 * trap set rma l2-user-define <UINT:index> mac <MACADDR:mac_min> <MACADDR:mac_max>
 */
cparser_result_t cparser_cmd_trap_set_rma_l2_user_define_index_mac_mac_min_mac_max(cparser_context_t *context,
    uint32_t *index_ptr,
    cparser_macaddr_t *mac_min_ptr,
    cparser_macaddr_t *mac_max_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_trap_userDefinedRma_t   userDefinedRma;
    uint64 mac_min, mac_max;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    mac_min = ((uint64)mac_min_ptr->octet[0] << 40) | ((uint64)mac_min_ptr->octet[1] << 32) | ((uint64)mac_min_ptr->octet[2] << 24) | ((uint64)mac_min_ptr->octet[3] << 16) | ((uint64)mac_min_ptr->octet[4] << 8) | (uint64)mac_min_ptr->octet[5];
    mac_max = ((uint64)mac_max_ptr->octet[0] << 40) | ((uint64)mac_max_ptr->octet[1] << 32) | ((uint64)mac_max_ptr->octet[2] << 24) | ((uint64)mac_max_ptr->octet[3] << 16) | ((uint64)mac_max_ptr->octet[4] << 8) | (uint64)mac_max_ptr->octet[5];

    if (mac_min > mac_max)
    {
        diag_util_printf("Input error: the value of upper should be bigger than htat of lower\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&userDefinedRma, 0, sizeof(rtk_trap_userDefinedRma_t));

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRma_get(unit, *index_ptr, &userDefinedRma), ret);

    osal_memcpy(&userDefinedRma.mac_min, (rtk_mac_t *)mac_min_ptr, sizeof(rtk_mac_t));
    osal_memcpy(&userDefinedRma.mac_max, (rtk_mac_t *)mac_max_ptr, sizeof(rtk_mac_t));

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRma_set(unit, *index_ptr, &userDefinedRma), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_INDEX_ETHER_TYPE_VALUE
/*
 * trap set rma l2-user-define <UINT:index> ether-type <UINT:value>
 */
cparser_result_t cparser_cmd_trap_set_rma_l2_user_define_index_ether_type_value(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *ethertype_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_trap_userDefinedRma_t   userDefinedRma;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&userDefinedRma, 0, sizeof(rtk_trap_userDefinedRma_t));

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRma_get(unit, *index_ptr, &userDefinedRma), ret);

    userDefinedRma.frameValue = *ethertype_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRma_set(unit, *index_ptr, &userDefinedRma), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_INDEX_COMPARE_TYPE_MAC_ETHERTYPE_BOTH
/*
 * trap set rma l2-user-define <UINT:index> compare-type ( mac | ethertype | both )
 */
cparser_result_t cparser_cmd_trap_set_rma_l2_user_define_index_compare_type_mac_ethertype_both(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_trap_userDefinedRma_t   userDefinedRma;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&userDefinedRma, 0, sizeof(rtk_trap_userDefinedRma_t));

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRma_get(unit, *index_ptr, &userDefinedRma), ret);

    if('m' == TOKEN_CHAR(6, 0))
        userDefinedRma.cmpType = RMA_CMP_TYPE_MAC;
    else if ('e' == TOKEN_CHAR(6, 0))
        userDefinedRma.cmpType = RMA_CMP_TYPE_ETHERTYPE;
    else if ('b' == TOKEN_CHAR(6, 0))
        userDefinedRma.cmpType = RMA_CMP_TYPE_MAC_ETHERTYPE;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRma_set(unit, *index_ptr, &userDefinedRma), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_LAYER2_OAM_RMA_02_RMA_03_RMA_0E_RMA_10_RMA_20_RMA_21_RMA_22_RMA_0X_RMA_1X_RMA_2X_ACTION_DROP_FORWARD_FORWARD_AND_FLOOD_TRAP_TO_CPU
/*
* trap set rma layer2 ( oam | rma-02 | rma-03 | rma-0E | rma-10 | rma-20 | rma-21 | rma-22 | rma-0X | rma-1X | rma-2X ) action ( drop | forward | forward-and-flood | trap-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_rma_layer2_oam_rma_02_rma_03_rma_0E_rma_10_rma_20_rma_21_rma_22_rma_0X_rma_1X_rma_2X_action_drop_forward_forward_and_flood_trap_to_cpu(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_trap_rmaGroup_frameType_t    rmaGroup_frameType;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    /*RMA Type*/
    if ('o' == TOKEN_CHAR(4,0))
    {
        rmaGroup_frameType = RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM;
    }
    else if ('r' == TOKEN_CHAR(4,0))
    {
        if (('0' == TOKEN_CHAR(4,4)) && ('2' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_SLOW_PROTOCOL_OTHER;
        }
        else if (('0' == TOKEN_CHAR(4,4)) && ('3' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_03;
        }
        else if (('0' == TOKEN_CHAR(4,4)) && ('E' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_0E_EXCEPT_PTP_LLDP;
        }
        else if (('1' == TOKEN_CHAR(4,4)) && ('0' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_10;
        }
        else if (('2' == TOKEN_CHAR(4,4)) && ('0' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_GMRP;
        }
        else if (('2' == TOKEN_CHAR(4,4)) && ('1' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_GVRP;
        }
        else if (('2' == TOKEN_CHAR(4,4)) && ('2' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_MSRP;
        }
        else if (('0' == TOKEN_CHAR(4,4)) && ('X' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_0X;
        }
        else if (('1' == TOKEN_CHAR(4,4)) && ('X' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_1X;
        }
        else if (('2' == TOKEN_CHAR(4,4)) && ('X' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_2X;
        }
        else
        {
            diag_util_printf("User config: type Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    /*RMA Action*/
    DIAG_TRAP_PARSE_MGNT_ACTION(6, rma_action);

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaGroupAction_set(unit, rmaGroup_frameType, rma_action), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_rma_layer2_oam_rma_02_rma_03_rma_0E_rma_10_rma_20_rma_21_rma_22_rma_0X_rma_1X_rma_2X_action_forward_trap_to_cpu_drop_flood_to_all */
#endif


#ifdef CMD_TRAP_SET_RMA_LAYER2_RMA_TAIL_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * trap set rma layer2 <UINT:rma_tail> action ( drop | forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_rma_layer2_rma_tail_action_drop_forward_trap_to_cpu(cparser_context_t *context,
    uint32_t *rma_tail_ptr)
{
    uint32                  unit = 0;
    uint32                  ramTail = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = MGMT_ACTION_FORWARD;
    rtk_mac_t               rma_frame;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    rma_frame.octet[0] = 0x01;
    rma_frame.octet[1] = 0x80;
    rma_frame.octet[2] = 0xc2;
    rma_frame.octet[3] = 0x00;
    rma_frame.octet[4] = 0x00;
    rma_frame.octet[5] = 0x00;

    ramTail = *rma_tail_ptr;
    if (ramTail > 0x2f)
    {
        diag_util_printf("Input error: the RMA address is out of range\n");
        return CPARSER_NOT_OK;
    }
    else
    {
        rma_frame.octet[5] = ramTail;
    }

    DIAG_TRAP_PARSE_MGNT_ACTION(6, rma_action);

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_set(unit, &rma_frame, rma_action), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_LAYER2_RMA_TAIL_ACTION_TRAP_TO_MASTER
/*
 * trap set rma layer2 <UINT:rma_tail> action trap-to-master
 */
cparser_result_t cparser_cmd_trap_set_rma_layer2_rma_tail_action_trap_to_master(cparser_context_t *context,
    uint32_t *rma_tail_ptr)
{
    uint32                  unit = 0;
    uint32                  ramTail = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mac_t               rma_frame;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    rma_frame.octet[0] = 0x01;
    rma_frame.octet[1] = 0x80;
    rma_frame.octet[2] = 0xc2;
    rma_frame.octet[3] = 0x00;
    rma_frame.octet[4] = 0x00;
    rma_frame.octet[5] = 0x00;

    ramTail = *rma_tail_ptr;
    if (ramTail > 0x2f)
    {
        diag_util_printf("Input error: the RMA address is out of range\n");
        return CPARSER_NOT_OK;
    }
    else
    {
        rma_frame.octet[5] = ramTail;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_set(unit, &rma_frame, MGMT_ACTION_TRAP2MASTERCPU), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_LAYER2_RMA_TAIL_RMA_TAIL_END_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * trap set rma layer2 <UINT:rma_tail> <UINT:rma_tail_end> action ( drop | forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_rma_layer2_rma_tail_rma_tail_end_action_drop_forward_trap_to_cpu(cparser_context_t *context,
    uint32_t *rma_tail_ptr, uint32_t *rma_tail_end_ptr)
{
    uint32                  unit = 0;
    uint32                  ramTail = 0, ramTailEnd, i;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = MGMT_ACTION_FORWARD;
    rtk_mac_t               rma_frame;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    rma_frame.octet[0] = 0x01;
    rma_frame.octet[1] = 0x80;
    rma_frame.octet[2] = 0xc2;
    rma_frame.octet[3] = 0x00;
    rma_frame.octet[4] = 0x00;
    rma_frame.octet[5] = 0x00;

    ramTail = *rma_tail_ptr;
    ramTailEnd = *rma_tail_end_ptr;
    if (ramTail > 0x2f || ramTailEnd > 0x2f)
    {
        diag_util_printf("Input error: the RMA address is out of range\n");
        return CPARSER_NOT_OK;
    }
    if (ramTail > ramTailEnd)
    {
        diag_util_printf("Input error: the value of rma_tail_end should be bigger than htat of rma_tail\n");
        return CPARSER_NOT_OK;
    }

    DIAG_TRAP_PARSE_MGNT_ACTION(7, rma_action);

    for (i = ramTail; i <= ramTailEnd; i++)
    {
        rma_frame.octet[5] = i;
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_set(unit, &rma_frame, rma_action), ret);
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_LAYER2_RMA_TAIL_RMA_TAIL_END_ACTION_TRAP_TO_MASTER
/*
 * trap set rma layer2 <UINT:rma_tail> <UINT:rma_tail_end> action trap-to-master
 */
cparser_result_t cparser_cmd_trap_set_rma_layer2_rma_tail_rma_tail_end_action_trap_to_master(cparser_context_t *context,
    uint32_t *rma_tail_ptr, uint32_t *rma_tail_end_ptr)
{
    uint32                  unit = 0;
    uint32                  ramTail = 0, ramTailEnd, i;
    int32                   ret = RT_ERR_FAILED;
    rtk_mac_t               rma_frame;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    rma_frame.octet[0] = 0x01;
    rma_frame.octet[1] = 0x80;
    rma_frame.octet[2] = 0xc2;
    rma_frame.octet[3] = 0x00;
    rma_frame.octet[4] = 0x00;
    rma_frame.octet[5] = 0x00;

    ramTail = *rma_tail_ptr;
    ramTailEnd = *rma_tail_end_ptr;
    if (ramTail > 0x2f || ramTailEnd > 0x2f)
    {
        diag_util_printf("Input error: the RMA address is out of range\n");
        return CPARSER_NOT_OK;
    }
    if (ramTail > ramTailEnd)
    {
        diag_util_printf("Input error: the value of rma_tail_end should be bigger than htat of rma_tail\n");
        return CPARSER_NOT_OK;
    }

    for (i = ramTail; i <= ramTailEnd; i++)
    {
        rma_frame.octet[5] = i;
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_set(unit, &rma_frame, MGMT_ACTION_TRAP2MASTERCPU), ret);
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_BPDU_PORT_PORT_ALL_ACTION_DROP_FORWARD_FORWARD_AND_FLOOD_TRAP_TO_CPU
/*
 * trap set rma port-rma bpdu port ( <PORT_LIST:port> | all ) action ( drop | forward | forward-and-flood | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_bpdu_port_port_all_action_drop_forward_forward_and_flood_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_BPDU;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_TRAP_PARSE_MGNT_ACTION(8, rma_action);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, rma_action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_BPDU_PORT_PORT_ALL_ACTION_TRAP_TO_MASTER
/*
 * trap set rma port-rma bpdu port ( <PORT_LIST:port> | all ) action trap-to-master
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_bpdu_port_port_all_action_trap_to_master(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_BPDU;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, ACTION_TRAP2MASTERCPU), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_RMA_BPDU_FLOOD_PORTMASK
/*
 * trap get rma bpdu flood-portmask
 */
cparser_result_t
cparser_cmd_trap_get_rma_bpdu_flood_portmask(cparser_context_t *context)
{
    uint32            unit = 0;
    int32             ret = RT_ERR_FAILED;
    char              port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_portmask_t    flood_portmask;

    osal_memset(&flood_portmask, 0, sizeof(rtk_portmask_t));
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trap_bpduFloodPortmask_get(unit, &flood_portmask), ret);

    diag_util_lPortMask2str(port_list, &flood_portmask);
    diag_util_mprintf("  BPDU flood portmask\t: %s \n", port_list);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_BPDU_FLOOD_PORTMASK_PORT_ALL
/*
 * trap set rma bpdu flood-portmask ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_set_rma_bpdu_flood_portmask_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32            unit = 0;
    int32             ret = RT_ERR_FAILED;
    diag_portlist_t   portlist;
    rtk_portmask_t    flood_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    osal_memset(&flood_portmask, 0 , sizeof(rtk_portmask_t));
    osal_memcpy(&flood_portmask, &portlist.portmask, sizeof(rtk_portmask_t));
    DIAG_UTIL_ERR_CHK(rtk_trap_bpduFloodPortmask_set(unit, &flood_portmask), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_LLDP_PORT_PORT_ALL_ACTION_DROP_FORWARD_FORWARD_AND_FLOOD_TRAP_TO_CPU
/*
 * trap set rma port-rma lldp port ( <PORT_LIST:port> | all ) action ( drop | forward | forward-and-flood | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_lldp_port_port_all_action_drop_forward_forward_and_flood_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_LLDP;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_TRAP_PARSE_MGNT_ACTION(8, rma_action);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, rma_action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_LLDP_PORT_PORT_ALL_ACTION_TRAP_TO_MASTER
/*
 * trap set rma port-rma lldp port ( <PORT_LIST:port> | all ) action trap-to-master
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_lldp_port_port_all_action_trap_to_master(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_LLDP;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, ACTION_TRAP2MASTERCPU), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_PTP_PORT_PORT_ALL_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * trap set rma port-rma ptp port ( <PORT_LIST:port> | all ) action ( drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_ptp_port_port_all_action_drop_forward_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_PTP;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_TRAP_PARSE_MGNT_ACTION(8, rma_action);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, rma_action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_PTP_PORT_PORT_ALL_ACTION_TRAP_TO_MASTER
/*
 * trap set rma port-rma ptp port ( <PORT_LIST:port> | all ) action trap-to-master
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_ptp_port_port_all_action_trap_to_master(
    cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_PTP;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, ACTION_TRAP2MASTERCPU), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_rma_port_rma_ptp_port_port_all_action_trap_to_master */
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_PTP_ETH_PORT_PORT_ALL_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * trap set rma port-rma ptp-eth port ( <PORT_LIST:port> | all ) action ( drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_ptp_eth_port_port_all_action_drop_forward_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_PTP_ETH2;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_TRAP_PARSE_MGNT_ACTION(8, rma_action);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, rma_action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_PTP_ETH_PORT_PORT_ALL_ACTION_TRAP_TO_MASTER
/*
 * trap set rma port-rma ptp-eth port ( <PORT_LIST:port> | all ) action trap-to-master
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_ptp_eth_port_port_all_action_trap_to_master(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_PTP_ETH2;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, ACTION_TRAP2MASTERCPU), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_PTP_UDP_PORT_PORT_ALL_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * trap set rma port-rma ptp-udp port ( <PORT_LIST:port> | all ) action ( drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_ptp_udp_port_port_all_action_drop_forward_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_PTP_UDP;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_TRAP_PARSE_MGNT_ACTION(8, rma_action);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, rma_action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_PTP_UDP_PORT_PORT_ALL_ACTION_TRAP_TO_MASTER
/*
 * trap set rma port-rma ptp-udp port ( <PORT_LIST:port> | all ) action trap-to-master
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_ptp_udp_port_port_all_action_trap_to_master(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_PTP_UDP;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, ACTION_TRAP2MASTERCPU), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_EAPOL_PORT_PORT_ALL_ACTION_DROP_FORWARD_FORWARD_AND_FLOOD_TRAP_TO_CPU
/*
 * trap set rma port-rma eapol port ( <PORT_LIST:port> | all ) action ( drop | forward | forward-and-flood | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_eapol_port_port_all_action_drop_forward_forward_and_flood_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = MGMT_ACTION_END;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_EAPOL;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_TRAP_PARSE_MGNT_ACTION(8, rma_action);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, rma_action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_EAPOL_PORT_PORT_ALL_ACTION_TRAP_TO_MASTER
/*
 * trap set rma port-rma eapol port ( <PORT_LIST:port> | all ) action trap-to-master
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_eapol_port_port_all_action_trap_to_master(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t     frameType;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_TYPE_EAPOL;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, MGMT_ACTION_TRAP2MASTERCPU), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_RMA_EAPOL_FLOOD_PORTMASK
/*
 * trap get rma eapol flood-portmask
 */
cparser_result_t
cparser_cmd_trap_get_rma_eapol_flood_portmask(cparser_context_t *context)
{
    uint32            unit = 0;
    int32             ret = RT_ERR_FAILED;
    char              port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_portmask_t    flood_portmask;

    osal_memset(&flood_portmask, 0, sizeof(rtk_portmask_t));
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trap_eapolFloodPortmask_get(unit, &flood_portmask), ret);

    diag_util_lPortMask2str(port_list, &flood_portmask);
    diag_util_mprintf("  EAPOL flood portmask\t: %s \n", port_list);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_EAPOL_FLOOD_PORTMASK_PORT_ALL
/*
 * trap set rma eapol flood-portmask ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_set_rma_eapol_flood_portmask_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32            unit = 0;
    int32             ret = RT_ERR_FAILED;
    diag_portlist_t   portlist;
    rtk_portmask_t    flood_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    osal_memset(&flood_portmask, 0 , sizeof(rtk_portmask_t));
    osal_memcpy(&flood_portmask, &portlist.portmask, sizeof(rtk_portmask_t));
    DIAG_UTIL_ERR_CHK(rtk_trap_eapolFloodPortmask_set(unit, &flood_portmask), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_RMA_LLDP_FLOOD_PORTMASK
/*
 * trap get rma lldp flood-portmask
 */
cparser_result_t
cparser_cmd_trap_get_rma_lldp_flood_portmask(cparser_context_t *context)
{
    uint32            unit = 0;
    int32             ret = RT_ERR_FAILED;
    char              port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_portmask_t    flood_portmask;

    osal_memset(&flood_portmask, 0, sizeof(rtk_portmask_t));
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trap_lldpFloodPortmask_get(unit, &flood_portmask), ret);

    diag_util_lPortMask2str(port_list, &flood_portmask);
    diag_util_mprintf("  LLDP flood portmask\t: %s \n", port_list);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_LLDP_FLOOD_PORTMASK_PORT_ALL
/*
 * trap set rma lldp flood-portmask ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_set_rma_lldp_flood_portmask_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32            unit = 0;
    int32             ret = RT_ERR_FAILED;
    diag_portlist_t   portlist;
    rtk_portmask_t    flood_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    osal_memset(&flood_portmask, 0 , sizeof(rtk_portmask_t));
    osal_memcpy(&flood_portmask, &portlist.portmask, sizeof(rtk_portmask_t));
    DIAG_UTIL_ERR_CHK(rtk_trap_lldpFloodPortmask_set(unit, &flood_portmask), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_TRAP_SET_EAPOL_FORWARD_TRAP_TO_CPU
/*
 * trap set eapol ( forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_eapol_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action = MGMT_ACTION_FORWARD;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_TRAP_PARSE_MGNT_ACTION(3, action);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, MGMT_TYPE_EAPOL, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_RMA_FLOOD_PORTMASK
/*
 * trap get rma flood-portmask
 */
cparser_result_t
cparser_cmd_trap_get_rma_flood_portmask(cparser_context_t *context)
{
    uint32            unit = 0;
    int32             ret = RT_ERR_FAILED;
    char              port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_portmask_t    flood_portmask;

    osal_memset(&flood_portmask, 0, sizeof(rtk_portmask_t));
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaFloodPortmask_get(unit, &flood_portmask), ret);

    diag_util_lPortMask2str(port_list, &flood_portmask);
    diag_util_mprintf("  RMA flood portmask\t: %s \n", port_list);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_FLOOD_PORTMASK_PORT_ALL
/*
 * trap set rma flood-portmask ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_set_rma_flood_portmask_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32            unit = 0;
    int32             ret = RT_ERR_FAILED;
    diag_portlist_t   portlist;
    rtk_portmask_t    flood_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    osal_memset(&flood_portmask, 0, sizeof(rtk_portmask_t));
    osal_memcpy(&flood_portmask, &portlist.portmask, sizeof(rtk_portmask_t));
    DIAG_UTIL_ERR_CHK(rtk_trap_rmaFloodPortmask_set(unit, &flood_portmask), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_EAPOL_BYPASS_STP_STATE_DISABLE_ENABLE
/*
 * trap set eapol bypass-stp state ( disable | enable )
 */
cparser_result_t cparser_cmd_trap_set_eapol_bypass_stp_state_disable_enable(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }
    else if('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassStp_set(unit, BYPASS_STP_TYPE_EAPOL, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_ARP_REQUEST_FORWARD_COPY_TO_CPU
/*
 * trap set arp-request ( forward | copy-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_arp_request_forward_copy_to_cpu(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action = MGMT_ACTION_FORWARD;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('f' == TOKEN_CHAR(3, 0))
    {
        action = MGMT_ACTION_FORWARD;
    }
    else
    {
        action = MGMT_ACTION_COPY2CPU;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, MGMT_TYPE_ARP, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_NEIGHBOR_DISCOVERY_FORWARD_COPY_TO_CPU
/*
 * trap set neighbor-discovery ( forward | copy-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_neighbor_discovery_forward_copy_to_cpu(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action = MGMT_ACTION_FORWARD;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('f' == TOKEN_CHAR(3, 0))
    {
        action = MGMT_ACTION_FORWARD;
    }
    else
    {
        action = MGMT_ACTION_COPY2CPU;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, MGMT_TYPE_IPV6ND, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_HOP_BY_HOP_POSITION_ACTION_FORWARD_TRAP_TO_CPU
/*
 * trap set hop-by-hop-position action ( forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_hop_by_hop_position_action_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action = MGMT_ACTION_FORWARD;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_TRAP_PARSE_MGNT_ACTION(4, action);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, MGMT_TYPE_IPV6_HOP_POS_ERR, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_IP6_HOP_BY_HOP_POSITION_PRIORITY_PRIORITY
/*
 * trap set ip6 hop-by-hop-position priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_ip6_hop_by_hop_position_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_IPV6_HOP_POS_ERR, *priority_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_IP6_UNKNOWN_HEADER_ACTION_FORWARD_TRAP_TO_CPU
/*
 * trap set ip6-unknown-header action ( forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_ip6_unknown_header_action_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action = MGMT_ACTION_FORWARD;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_TRAP_PARSE_MGNT_ACTION(4, action);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, MGMT_TYPE_IPV6_HDR_UNKWN, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_IP6_IP6_UNKNOWN_HEADER_PRIORITY_PRIORITY
/*
 * trap set ip6 ip6-unknown-header priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_ip6_ip6_unknown_header_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_IPV6_HDR_UNKWN, *priority_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_INVALID_SA_PRIORITY_PRIORITY
/*
 * trap set invalid-sa priority <UINT:priority>
 */
cparser_result_t cparser_cmd_trap_set_invalid_sa_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_INVALID_SA, *priority_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_CRC_ERROR_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * trap set crc-error action ( drop | forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_crc_error_action_drop_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action = MGMT_ACTION_FORWARD;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_TRAP_PARSE_MGNT_ACTION(4, action);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, MGMT_TYPE_L2_CRC_ERR, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_IP4_CHECKSUM_ERROR_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * trap set ip4-checksum-error action ( drop | forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_ip4_checksum_error_action_drop_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action = MGMT_ACTION_FORWARD;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_TRAP_PARSE_MGNT_ACTION(4, action);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, MGMT_TYPE_IP4_CHKSUM_ERR, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_SWITCH_MAC_DROP_FORWARD_TRAP_TO_CPU
/*
 * trap set switch-mac ( drop | forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_trap_set_switch_mac_drop_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_mgmt_action_t   action = MGMT_ACTION_FORWARD;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_TRAP_PARSE_MGNT_ACTION(3, action);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, MGMT_TYPE_SELFMAC, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_CFM_UNKNOWN_FRAME_ACTION
/* trap get cfm unknown-frame action */
cparser_result_t
cparser_cmd_trap_get_cfm_unknown_frame_action(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        ret = rtk_trap_cfmUnknownFrameAct_get(unit, &action);
    }
    else
#endif
    {
        ret = rtk_trap_cfmAct_get(unit, TRAP_CFM_TYPE_UNKN, 0, &action);
    }

    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACTION_DROP == action)
        diag_util_mprintf("\tCFM unknown type frame action: Drop\n");
    else if (ACTION_TRAP2CPU == action)
        diag_util_mprintf("\tCFM unknown type frame action: Trap\n");
    else if (ACTION_FORWARD == action)
        diag_util_mprintf("\tCFM unknown type frame action: Forward\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_cfm_unknown_frame_action */
#endif

#ifdef CMD_TRAP_SET_CFM_UNKNOWN_FRAME_DROP_TRAP_FORWARD
/* trap set cfm unknown-frame ( drop | trap | forward ) */
cparser_result_t
cparser_cmd_trap_set_cfm_unknown_frame_drop_trap_forward(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    switch (TOKEN_CHAR(4, 0))
    {
        case 'd':
            action = ACTION_DROP;
            break;
        case 't':
            action = ACTION_TRAP2CPU;
            break;
        case 'f':
            action = ACTION_FORWARD;
            break;
        default:
            diag_util_printf("User config: action Error!\n");
            return CPARSER_NOT_OK;
    }   /* switch (TOKEN_CHAR(4, 0)) */

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmUnknownFrameAct_set(unit, action), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmAct_set(unit, TRAP_CFM_TYPE_UNKN, 0, action), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_cfm_unknown_frame_drop_trap_forward */
#endif  /* CMD_TRAP_SET_CFM_UNKNOWN_FRAME_DROP_TRAP_FORWARD */

#ifdef CMD_TRAP_GET_CFM_LOOPBACK_LEVEL_ACTION
/* trap get cfm loopback <UINT:level> action */
cparser_result_t
cparser_cmd_trap_get_cfm_loopback_level_action(cparser_context_t *context,
                                              uint32_t *level_ptr)
{
    uint32          unit = 0;
    int32           ret;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((level_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        ret = rtk_trap_cfmLoopbackLinkTraceAct_get(unit, *level_ptr, &action);
    }
    else
#endif
    {
        ret = rtk_trap_cfmAct_get(unit, TRAP_CFM_TYPE_LB, *level_ptr, &action);
    }

    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM Loopback frame with MD level %d action: ",
                      *level_ptr);
    if (ACTION_DROP == action)
        diag_util_mprintf("Drop\n");
    else if (ACTION_TRAP2CPU == action)
        diag_util_mprintf("Trap\n");
    else if (ACTION_FORWARD == action)
        diag_util_mprintf("Forward\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_cfm_loopback_level_action */
#endif  /* CMD_TRAP_GET_CFM_LOOPBACK_LEVEL_ACTION */

#ifdef CMD_TRAP_SET_CFM_LOOPBACK_LEVEL_DROP_TRAP_FORWARD
/* trap set cfm loopback <UINT:level> ( drop | trap | forward ) */
cparser_result_t
cparser_cmd_trap_set_cfm_loopback_level_drop_trap_forward(cparser_context_t *context,
                                                         uint32_t *level_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((level_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    switch (TOKEN_CHAR(5, 0))
    {
        case 'd':
            action = ACTION_DROP;
            break;
        case 't':
            action = ACTION_TRAP2CPU;
            break;
        case 'f':
            action = ACTION_FORWARD;
            break;
        default:
            diag_util_printf("User config: action Error!\n");
            return CPARSER_NOT_OK;
    }   /* switch (TOKEN_CHAR(5, 0)) */

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmLoopbackLinkTraceAct_set(unit, *level_ptr, action), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmAct_set(unit, TRAP_CFM_TYPE_LB, *level_ptr, action), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_cfm_loopback_level_drop_trap_forward */
#endif  /* CMD_TRAP_SET_CFM_LOOPBACK_LEVEL_DROP_TRAP_FORWARD */

#ifdef CMD_TRAP_GET_CFM_LINKTRACE_LEVEL_ACTION
/* trap get cfm linktrace <UINT:level> action */
cparser_result_t
cparser_cmd_trap_get_cfm_linktrace_level_action(cparser_context_t *context,
                                              uint32_t *level_ptr)
{
    uint32          unit = 0;
    int32           ret;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((level_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        ret = rtk_trap_cfmLoopbackLinkTraceAct_get(unit, *level_ptr, &action);
    }
    else
#endif
    {
        ret = rtk_trap_cfmAct_get(unit, TRAP_CFM_TYPE_LT, *level_ptr, &action);
    }

    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM Linktrace frame with MD level %d action: ",
                      *level_ptr);
    if (ACTION_DROP == action)
        diag_util_mprintf("Drop\n");
    else if (ACTION_TRAP2CPU == action)
        diag_util_mprintf("Trap\n");
    else if (ACTION_FORWARD == action)
        diag_util_mprintf("Forward\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_cfm_linktrace_level_action */
#endif  /* CMD_TRAP_GET_CFM_LINKTRACE_LEVEL_ACTION */

#ifdef CMD_TRAP_SET_CFM_LINKTRACE_LEVEL_DROP_TRAP_FORWARD
/* trap set cfm linktrace <UINT:level> ( drop | trap | forward ) */
cparser_result_t
cparser_cmd_trap_set_cfm_linktrace_level_drop_trap_forward(cparser_context_t *context,
                                                         uint32_t *level_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((level_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    switch (TOKEN_CHAR(5, 0))
    {
        case 'd':
            action = ACTION_DROP;
            break;
        case 't':
            action = ACTION_TRAP2CPU;
            break;
        case 'f':
            action = ACTION_FORWARD;
            break;
        default:
            diag_util_printf("User config: action Error!\n");
            return CPARSER_NOT_OK;
    }   /* switch (TOKEN_CHAR(5, 0)) */

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmLoopbackLinkTraceAct_set(unit, *level_ptr, action), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmAct_set(unit, TRAP_CFM_TYPE_LT, *level_ptr, action), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_cfm_linktrace_level_drop_trap_forward */
#endif  /* CMD_TRAP_SET_CFM_LINKTRACE_LEVEL_DROP_TRAP_FORWARD */

#ifdef CMD_TRAP_GET_CFM_CCM_LEVEL_ACTION
/* trap get cfm ccm <UINT:level> action */
cparser_result_t
cparser_cmd_trap_get_cfm_ccm_level_action(cparser_context_t *context,
                                          uint32_t *level_ptr)
{
    uint32          unit = 0;
    int32           ret;
    rtk_trap_oam_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((level_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        ret = rtk_trap_cfmCcmAct_get(unit, *level_ptr, &action);
    }
    else
#endif
    {
        ret = rtk_trap_cfmAct_get(unit, TRAP_CFM_TYPE_CCM, *level_ptr, (rtk_action_t *)&action);
    }

    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM CCM frame with MD level %d action: ", *level_ptr);
    if (TRAP_OAM_ACTION_DROP == action)
        diag_util_mprintf("Drop\n");
    else if (TRAP_OAM_ACTION_TRAP2CPU == action)
        diag_util_mprintf("Trap\n");
    else if (TRAP_OAM_ACTION_FORWARD == action)
        diag_util_mprintf("Forward\n");
    else if (TRAP_OAM_ACTION_LINK_FAULT_DETECT == action)
        diag_util_mprintf("Link fault detection\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_cfm_ccm_level_action */
#endif  /* CMD_TRAP_GET_CFM_CCM_LEVEL_ACTION */

#ifdef CMD_TRAP_SET_CFM_CCM_LEVEL_DROP_TRAP_FORWARD_LINK_FAULT_DETECTION
/* trap set cfm ccm <UINT:level> ( drop | trap | forward | link-fault-detection ) */
cparser_result_t
cparser_cmd_trap_set_cfm_ccm_level_drop_trap_forward_link_fault_detection(
        cparser_context_t *context, uint32_t *level_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((level_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    switch (TOKEN_CHAR(5, 0))
    {
        case 'd':
            action = ACTION_DROP;
            break;
        case 't':
            action = ACTION_TRAP2CPU;
            break;
        case 'f':
            action = ACTION_FORWARD;
            break;
        case 'l':
            action = TRAP_OAM_ACTION_LINK_FAULT_DETECT;
            break;
        default:
            diag_util_printf("User config: action Error!\n");
            return CPARSER_NOT_OK;
    }   /* switch (TOKEN_CHAR(5, 0)) */

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmCcmAct_set(unit, *level_ptr, action), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmAct_set(unit, TRAP_CFM_TYPE_CCM, *level_ptr, action), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_cfm_ccm_level_drop_trap_forward_link_fault_detection */
#endif  /* CMD_TRAP_SET_CFM_CCM_LEVEL_DROP_TRAP_FORWARD_LINK_FAULT_DETECTION */

#ifdef CMD_TRAP_GET_CFM_ETH_DM_LEVEL_ACTION
/* trap get cfm eth-dm <UINT:level> action */
cparser_result_t cparser_cmd_trap_get_cfm_eth_dm_level_action(cparser_context_t *context,
    uint32_t *level_ptr)
{
    uint32          unit = 0;
    int32           ret;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((level_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmEthDmAct_get(unit, *level_ptr, &action), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmAct_get(unit, TRAP_CFM_TYPE_ETHDM, *level_ptr, &action), ret);
    }

    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tCFM ETH-DM frame with MD level %d action: ",
                      *level_ptr);
    if (ACTION_DROP == action)
        diag_util_mprintf("Drop\n");
    else if (ACTION_TRAP2CPU == action)
        diag_util_mprintf("Trap\n");
    else if (ACTION_FORWARD == action)
        diag_util_mprintf("Forward\n");
    else if (ACTION_TRAP2MASTERCPU == action)
        diag_util_mprintf("Trap to Master Cpu\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_cfm_eth_dm_level_action */
#endif  /* CMD_TRAP_GET_CFM_ETH_DM_LEVEL_ACTION */

#ifdef CMD_TRAP_SET_CFM_ETH_DM_LEVEL_ACTION_DROP_TRAP_FORWARD
/* trap set cfm eth-dm <UINT:level> action ( drop | trap | forward ) */
cparser_result_t cparser_cmd_trap_set_cfm_eth_dm_level_action_drop_trap_forward(cparser_context_t *context,
    uint32_t *level_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((level_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    switch (TOKEN_CHAR(6, 0))
    {
        case 'd':
            action = ACTION_DROP;
            break;
        case 't':
            action = ACTION_TRAP2CPU;
            break;
        case 'f':
            action = ACTION_FORWARD;
            break;
        default:
            diag_util_printf("User config: action Error!\n");
            return CPARSER_NOT_OK;
    }   /* switch (TOKEN_CHAR(5, 0)) */

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmEthDmAct_set(unit, *level_ptr, action), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmAct_set(unit, TRAP_CFM_TYPE_ETHDM, *level_ptr, action), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_cfm_eth_dm_level_action_drop_trap_forward */
#endif  /* CMD_TRAP_SET_CFM_ETH_DM_LEVEL_ACTION_DROP_TRAP_FORWARD */

#ifdef CMD_TRAP_SET_CFM_ETH_DM_LEVEL_ACTION_TRAP_MASTER
/* trap set cfm eth-dm <UINT:level> action trap-master */
cparser_result_t cparser_cmd_trap_set_cfm_eth_dm_level_action_trap_master(cparser_context_t *context,
    uint32_t *level_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((level_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    action = ACTION_TRAP2MASTERCPU;

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmEthDmAct_set(unit, *level_ptr, action), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cfmAct_set(unit, TRAP_CFM_TYPE_ETHDM, *level_ptr, action), ret);
    }

    return CPARSER_OK;
}
#endif /*CMD_TRAP_SET_CFM_ETH_DM_LEVEL_ACTION_TRAP_MASTER*/


#ifdef CMD_TRAP_GET_OAM_LOOPBACK_CTRL_PORT_ALL_PAR
/*
 * trap get oam-loopback-ctrl ( <PORT_LIST:port> | all ) par
 */
cparser_result_t cparser_cmd_trap_get_oam_loopback_ctrl_port_all_par(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    rtk_trap_oam_action_t    action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_trap_portOamLoopbackParAction_get(unit, port, &action);
        if (ret!= RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        diag_util_mprintf("\tPort %u:\n", port);
        if( TRAP_OAM_ACTION_DROP == action )
            diag_util_mprintf("\t\tOAM parser action: Drop\n");
        else if( TRAP_OAM_ACTION_FORWARD == action )
            diag_util_mprintf("\t\tOAM parser action: Forward\n");
        else if( TRAP_OAM_ACTION_LOOPBACK == action )
            diag_util_mprintf("\t\tOAM parser action: Loopback\n");
        else if( TRAP_OAM_ACTION_TRAP2CPU == action )
            diag_util_mprintf("\t\tOAM parser action: Trap\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_oam_loopback_ctrl_port_all_par */
#endif  /* CMD_TRAP_GET_OAM_LOOPBACK_CTRL_PORT_ALL_PAR */

#ifdef CMD_TRAP_SET_OAM_LOOPBACK_CTRL_PORT_ALL_PAR_DROP_FORWARD_LOOPBACK_TRAP
/*
 * trap set oam-loopback-ctrl ( <PORT_LIST:port> | all ) par ( drop | forward | loopback | trap )
 */
cparser_result_t
cparser_cmd_trap_set_oam_loopback_ctrl_port_all_par_drop_forward_loopback_trap(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_trap_oam_action_t   parAction;
    diag_portlist_t         portlist;
    rtk_port_t              port = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('d' == TOKEN_CHAR(5, 0))
        parAction = TRAP_OAM_ACTION_DROP;
    else if('f' == TOKEN_CHAR(5, 0))
        parAction = TRAP_OAM_ACTION_FORWARD;
    else if('l' == TOKEN_CHAR(5, 0))
        parAction = TRAP_OAM_ACTION_LOOPBACK;
    else if('t' == TOKEN_CHAR(5, 0))
        parAction = TRAP_OAM_ACTION_TRAP2CPU;
    else
    {
        diag_util_printf("User config: parAction Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portOamLoopbackParAction_set(unit, port, parAction), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_oam_loopback_ctrl_port_all_par_drop_forward_loopback_trap */
#endif

#ifdef CMD_TRAP_GET_IP6_HOP_LIMIT_EXCEED_HDR_ERR_HOP_BY_HOP_ACTION
/*
 * trap get ip6 ( hop-limit-exceed | hdr-err | hop-by-hop ) action
 */
cparser_result_t
cparser_cmd_trap_get_ip6_hop_limit_exceed_hdr_err_hop_by_hop_action(
        cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if (0 == osal_strcmp(TOKEN_STR(3), "hop-limit-exceed"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HL_EXCEED;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-err"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hop-by-hop"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HOP_BY_HOP;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    ret = rtk_trap_routeExceptionAction_get(unit, type, &action);
    if (ret!= RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tIPv6 %s action: ", TOKEN_STR(3));
    if (ACTION_DROP == action)
        diag_util_mprintf("Drop\n");
    else if (ACTION_FORWARD == action)
        diag_util_mprintf("L2 forward\n");
    else if (ACTION_TRAP2CPU == action)
        diag_util_mprintf("Trap\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_ip6_hop_limit_exceed_hdr_err_hop_by_hop_action */
#endif

#ifdef CMD_TRAP_SET_IP6_HOP_LIMIT_EXCEED_HDR_ERR_HOP_BY_HOP_DROP_L2_FORWARD_TRAP
/*
 * trap set ip6 ( hop-limit-exceed | hdr-err | hop-by-hop ) ( drop | l2-forward | trap )
 */
cparser_result_t
cparser_cmd_trap_set_ip6_hop_limit_exceed_hdr_err_hop_by_hop_drop_l2_forward_trap(
        cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    act;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if (0 == osal_strcmp(TOKEN_STR(3), "hop-limit-exceed"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HL_EXCEED;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-err"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hop-by-hop"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HOP_BY_HOP;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    if('d' == TOKEN_CHAR(4, 0))
        act = ACTION_DROP;
    else if('l' == TOKEN_CHAR(4, 0))
        act = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(4, 0))
        act = ACTION_TRAP2CPU;
    else
    {
        diag_util_printf("User config: trap action Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_routeExceptionAction_set(unit, type, act), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_ip6_hop_limit_exceed_hdr_err_hop_by_hop_drop_l2_forward_trap */
#endif

#ifdef CMD_TRAP_GET_IP6_HOP_LIMIT_EXCEED_HDR_ERR_HOP_BY_HOP_PRIORITY
/*
 * trap get ip6 ( hop-limit-exceed | hdr-err | hop-by-hop ) priority
 */
cparser_result_t
cparser_cmd_trap_get_ip6_hop_limit_exceed_hdr_err_hop_by_hop_priority(
        cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   pri;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if (0 == osal_strcmp(TOKEN_STR(3), "hop-limit-exceed"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HL_EXCEED;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-err"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hop-by-hop"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HOP_BY_HOP;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    ret = rtk_trap_routeExceptionPri_get(unit, type, &pri);
    if (ret!= RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tIPv6 %s priority: %d", TOKEN_STR(3), pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_ip6_hop_limit_exceed_hdr_err_hop_by_hop_priority */
#endif

#ifdef CMD_TRAP_SET_IP6_HOP_LIMIT_EXCEED_HDR_ERR_HOP_BY_HOP_PRIORITY_PRIORITY
/*
 * trap set ip6 ( hop-limit-exceed | hdr-err | hop-by-hop ) priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_trap_set_ip6_hop_limit_exceed_hdr_err_hop_by_hop_priority_priority(
        cparser_context_t *context, uint32_t *priority_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if (0 == osal_strcmp(TOKEN_STR(3), "hop-limit-exceed"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HL_EXCEED;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-err"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hop-by-hop"))
        type = ROUTE_EXCEPTION_TYPE_IP6_HOP_BY_HOP;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_routeExceptionPri_set(unit, type, *priority_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_ip6_hop_limit_exceed_hdr_err_hop_by_hop_priority_priority */
#endif

#ifdef CMD_TRAP_GET_IP_TTL_EXCEED_HDR_ERR_HDR_WITH_OPTION_ACTION
/*
 * trap get ip ( ttl-exceed | hdr-err | hdr-with-option ) action
 */
cparser_result_t
cparser_cmd_trap_get_ip_ttl_exceed_hdr_err_hdr_with_option_action(
        cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if (0 == osal_strcmp(TOKEN_STR(3), "ttl-exceed"))
        type = ROUTE_EXCEPTION_TYPE_TTL_EXCEED;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-err"))
        type = ROUTE_EXCEPTION_TYPE_HDR_ERR;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-with-option"))
        type = ROUTE_EXCEPTION_TYPE_WITH_OPT;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    ret = rtk_trap_routeExceptionAction_get(unit, type, &action);
    if (ret!= RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tIPv4 %s action: ", TOKEN_STR(3));
    if (ACTION_DROP == action)
        diag_util_mprintf("Drop\n");
    else if (ACTION_FORWARD == action)
        diag_util_mprintf("L2 forward\n");
    else if (ACTION_TRAP2CPU == action)
        diag_util_mprintf("Trap\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_ip_ttl_exceed_hdr_err_hdr_with_option_action */
#endif

#ifdef CMD_TRAP_SET_IP_TTL_EXCEED_HDR_ERR_HDR_WITH_OPTION_DROP_L2_FORWARD_TRAP
/*
 * trap set ip ( ttl-exceed | hdr-err | hdr-with-option ) ( drop | l2-forward | trap )
 */
cparser_result_t
cparser_cmd_trap_set_ip_ttl_exceed_hdr_err_hdr_with_option_drop_l2_forward_trap(
        cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    act;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if (0 == osal_strcmp(TOKEN_STR(3), "ttl-exceed"))
        type = ROUTE_EXCEPTION_TYPE_TTL_EXCEED;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-err"))
        type = ROUTE_EXCEPTION_TYPE_HDR_ERR;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-with-option"))
        type = ROUTE_EXCEPTION_TYPE_WITH_OPT;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    if('d' == TOKEN_CHAR(4, 0))
        act = ACTION_DROP;
    else if('l' == TOKEN_CHAR(4, 0))
        act = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(4, 0))
        act = ACTION_TRAP2CPU;
    else
    {
        diag_util_printf("User config: trap action Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_routeExceptionAction_set(unit, type, act), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_ip_ttl_exceed_hdr_err_hdr_with_option_drop_l2_forward_trap */
#endif

#ifdef CMD_TRAP_GET_IP_TTL_EXCEED_HDR_ERR_HDR_WITH_OPTION_PRIORITY
/*
 * trap get ip ( ttl-exceed | hdr-err | hdr-with-option ) priority
 */
cparser_result_t
cparser_cmd_trap_get_ip_ttl_exceed_hdr_err_hdr_with_option_priority(
        cparser_context_t *context)
{

    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   pri;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if (0 == osal_strcmp(TOKEN_STR(3), "ttl-exceed"))
        type = ROUTE_EXCEPTION_TYPE_TTL_EXCEED;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-err"))
        type = ROUTE_EXCEPTION_TYPE_HDR_ERR;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-with-option"))
        type = ROUTE_EXCEPTION_TYPE_WITH_OPT;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    ret = rtk_trap_routeExceptionPri_get(unit, type, &pri);
    if (ret!= RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tIPv4 %s priority: %d", TOKEN_STR(3), pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_ip_ttl_exceed_hdr_err_hdr_with_option_priority */
#endif

#ifdef CMD_TRAP_SET_IP_TTL_EXCEED_HDR_ERR_HDR_WITH_OPTION_PRIORITY_PRIORITY
/*
 * trap set ip ( ttl-exceed | hdr-err | hdr-with-option ) priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_trap_set_ip_ttl_exceed_hdr_err_hdr_with_option_priority_priority(
        cparser_context_t *context, uint32_t *priority_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if (0 == osal_strcmp(TOKEN_STR(3), "ttl-exceed"))
        type = ROUTE_EXCEPTION_TYPE_TTL_EXCEED;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-err"))
        type = ROUTE_EXCEPTION_TYPE_HDR_ERR;
    else if (0 == osal_strcmp(TOKEN_STR(3), "hdr-with-option"))
        type = ROUTE_EXCEPTION_TYPE_WITH_OPT;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_routeExceptionPri_set(unit, type, *priority_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_ip_ttl_exceed_hdr_err_hdr_with_option_priority_priority */
#endif

#ifdef CMD_TRAP_GET_GW_MAC_ERR_ACTION
/*
 * trap get gw-mac-err action
 */
cparser_result_t
cparser_cmd_trap_get_gw_mac_err_action(
        cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    type = ROUTE_EXCEPTION_TYPE_GW_MAC_ERR;

    ret = rtk_trap_routeExceptionAction_get(unit, type, &action);
    if (ret!= RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tIPv4 %s action: ", TOKEN_STR(3));
    if (ACTION_DROP == action)
        diag_util_mprintf("Drop\n");
    else if (ACTION_FORWARD == action)
        diag_util_mprintf("L2 forward\n");
    else if (ACTION_TRAP2CPU == action)
        diag_util_mprintf("Trap\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_gw_mac_err_action */
#endif

#ifdef CMD_TRAP_SET_GW_MAC_ERR_DROP_L2_FORWARD_TRAP
/*
 * trap set gw-mac-err ( drop | l2-forward | trap )
 */
cparser_result_t
cparser_cmd_trap_set_gw_mac_err_drop_l2_forward_trap(
        cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    act;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    type = ROUTE_EXCEPTION_TYPE_GW_MAC_ERR;

    if('d' == TOKEN_CHAR(3, 0))
        act = ACTION_DROP;
    else if('l' == TOKEN_CHAR(3, 0))
        act = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(3, 0))
        act = ACTION_TRAP2CPU;
    else
    {
        diag_util_printf("User config: trap action Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_routeExceptionAction_set(unit, type, act), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_gw_mac_err_drop_l2_forward_trap */
#endif

#ifdef CMD_TRAP_GET_GW_MAC_ERR_PRIORITY
/*
 * trap get gw-mac-err priority
 */
cparser_result_t
cparser_cmd_trap_get_gw_mac_err_priority(
        cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_pri_t   pri;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    type = ROUTE_EXCEPTION_TYPE_GW_MAC_ERR;

    ret = rtk_trap_routeExceptionPri_get(unit, type, &pri);
    if (ret!= RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\tGateway MAC error priority: %d", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_gw_mac_err_priority */
#endif

#ifdef CMD_TRAP_SET_GW_MAC_ERR_PRIORITY_PRIORITY
/*
 * trap set gw-mac-err priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_trap_set_gw_mac_err_priority_priority(
        cparser_context_t *context, uint32_t *priority_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_trap_routeExceptionType_t type;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    type = ROUTE_EXCEPTION_TYPE_GW_MAC_ERR;

    DIAG_UTIL_ERR_CHK(rtk_trap_routeExceptionPri_set(unit, type, *priority_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_gw_mac_err_priority_priority */
#endif

#ifdef CMD_TRAP_GET_ARP_REQUEST_ARP_REPLY
/*
 * trap get ( arp-request | arp-reply )
 */
cparser_result_t cparser_cmd_trap_get_arp_request_arp_reply(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_mgmt_action_t   act = MGMT_ACTION_END;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('q' == TOKEN_CHAR(2, 6))
        type = MGMT_TYPE_ARP_REQ;
    else if('p' == TOKEN_CHAR(2, 6))
        type = MGMT_TYPE_ARP_REP;
    else
    {
        diag_util_printf("User config: trap type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_get(unit, type, &act), ret);

    if('q' == TOKEN_CHAR(2, 6))
        diag_util_mprintf("\tARP Request: %s\n",text_trap_act[act].text);
    else
        diag_util_mprintf("\tARP Reply: %s\n",text_trap_act[act].text);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_ARP_REQUEST_ARP_REPLY_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * trap set ( arp-request | arp-reply ) ( forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_trap_set_arp_request_arp_reply_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_mgmt_action_t   act = MGMT_ACTION_END;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('q' == TOKEN_CHAR(2, 6))
        type = MGMT_TYPE_ARP_REQ;
    else if('p' == TOKEN_CHAR(2, 6))
        type = MGMT_TYPE_ARP_REP;
    else
    {
        diag_util_printf("User config: trap type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_TRAP_PARSE_MGNT_ACTION(3, act);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, type, act), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_arp_request_arp_reply_forward_trap_to_cpu_copy_to_cpu */
#endif


#ifdef CMD_TRAP_GET_IGMP_MLD
/*
 * trap get ( igmp | mld )
 */
cparser_result_t
cparser_cmd_trap_get_igmp_mld(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_mgmt_action_t   act;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if('i' == TOKEN_CHAR(2, 0))
        type = MGMT_TYPE_IGMP;
    else if('m' == TOKEN_CHAR(2, 0))
        type = MGMT_TYPE_MLD;
    else
    {
        diag_util_printf("User config: trap type Error!\n");
        return CPARSER_NOT_OK;
    }

    ret = rtk_trap_mgmtFrameAction_get(unit, type, &act);
    if (ret!= RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('i' == TOKEN_CHAR(2, 0))
        diag_util_mprintf("\tIGMP: %s\n",text_trap_act[act].text);
    else
        diag_util_mprintf("\tMLD: %s\n",text_trap_act[act].text);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_igmp_mld */
#endif

#ifdef CMD_TRAP_SET_IGMP_MLD_FORWARD_TRAP_TO_CPU
/*
 * trap set ( igmp | mld ) ( forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_igmp_mld_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_mgmt_action_t   action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(2, 0))
        type = MGMT_TYPE_IGMP;
    else if('m' == TOKEN_CHAR(2, 0))
        type = MGMT_TYPE_MLD;
    else
    {
        diag_util_printf("User config: trap type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_TRAP_PARSE_MGNT_ACTION(3, action);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_igmp_mld */
#endif

#ifdef CMD_TRAP_SET_IGMP_MLD_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * trap set ( igmp | mld ) ( copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_trap_set_igmp_mld_copy_to_cpu_trap_to_master_copy_to_master(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_mgmt_action_t   act = MGMT_ACTION_END;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(2, 0))
        type = MGMT_TYPE_IGMP;
    else if('m' == TOKEN_CHAR(2, 0))
        type = MGMT_TYPE_MLD;
    else
    {
        diag_util_printf("User config: trap type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_TRAP_PARSE_MGNT_ACTION(3, act);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, type, act), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_igmp_mld_forward_trap_to_cpu_copy_to_cpu */
#endif

#ifdef CMD_TRAP_GET_DHCP_DHCPV6
/*
 * trap get ( dhcp | dhcpv6 )
 */
cparser_result_t
cparser_cmd_trap_get_dhcp_dhcpv6(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_mgmt_action_t   act;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if('v' == TOKEN_CHAR(2, 4))
        type = MGMT_TYPE_DHCP6;
    else if('d' == TOKEN_CHAR(2, 0))
        type = MGMT_TYPE_DHCP;
    else
    {
        diag_util_printf("User config: trap type Error!\n");
        return CPARSER_NOT_OK;
    }

    ret = rtk_trap_mgmtFrameAction_get(unit, type, &act);
    if (ret!= RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('v' == TOKEN_CHAR(2, 4))
        diag_util_mprintf("\tDHCPV6: %s\n",text_trap_act[act].text);
    else
        diag_util_mprintf("\tDHCP: %s\n",text_trap_act[act].text);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_dhcp_dhcpv6 */
#endif

#ifdef CMD_TRAP_SET_DHCP_DHCPV6_FORWARD_TRAP_TO_CPU_COPY_TO_CPU
/*
 * trap set ( dhcp | dhcpv6 ) ( forward | trap-to-cpu | copy-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_dhcp_dhcpv6_forward_trap_to_cpu_copy_to_cpu(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_mgmt_action_t   action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('v' == TOKEN_CHAR(2, 4))
        type = MGMT_TYPE_DHCP6;
    else if('d' == TOKEN_CHAR(2, 0))
        type = MGMT_TYPE_DHCP;
    else
    {
        diag_util_printf("User config: trap type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_TRAP_PARSE_MGNT_ACTION(3, action);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_dhcp_dhcpv6_forward_trap_to_cpu_copy_to_cpu */
#endif

#ifdef CMD_TRAP_SET_DHCP_DHCPV6_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * trap set ( dhcp | dhcpv6 ) ( trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_trap_set_dhcp_dhcpv6_trap_to_master_copy_to_master(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_mgmt_action_t   action;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('v' == TOKEN_CHAR(2, 4))
        type = MGMT_TYPE_DHCP6;
    else if('d' == TOKEN_CHAR(2, 0))
        type = MGMT_TYPE_DHCP;
    else
    {
        diag_util_printf("User config: trap type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_TRAP_PARSE_MGNT_ACTION(3, action);

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameAction_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_dhcp_dhcpv6_trap_to_master_copy_to_master */
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_INDEX_BYPASS_STP_DISABLE_ENABLE
/*
 * trap set rma l2-user-define <UINT:index> bypass-stp ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_l2_user_define_index_bypass_stp_disable_enable(
        cparser_context_t *context, uint32_t *index_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    en;
    uint32          bypassStpFieldIdx[] = {BYPASS_STP_TYPE_USER_DEF_0,
                                           BYPASS_STP_TYPE_USER_DEF_1,
                                           BYPASS_STP_TYPE_USER_DEF_2,
                                           BYPASS_STP_TYPE_USER_DEF_3};

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6, 0))
        en = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        en = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassStp_set(unit, bypassStpFieldIdx[*index_ptr], en), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_INDEX_LEARN_DISABLE_ENABLE
/*
 * trap set rma l2-user-define <UINT:index> learn ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_l2_user_define_index_learn_disable_enable(
        cparser_context_t *context, uint32_t *index_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    en;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6, 0))
        en = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        en = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_userDefineRmaLearningEnable_set(unit, *index_ptr, en), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_LAYER2_RMA_TAIL_0X_SLOW_PROTO_BYPASS_STP_DISABLE_ENABLE
/*
 * trap set rma layer2 ( rma-tail-0X | slow-proto ) bypass-stp ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_layer2_rma_tail_0X_slow_proto_bypass_stp_disable_enable(
        cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_trap_bypassStpType_t    type;
    rtk_enable_t    en;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('r' == TOKEN_CHAR(4, 0))
        type = BYPASS_STP_TYPE_RMA_0X;
    else if('s' == TOKEN_CHAR(4, 0))
        type = BYPASS_STP_TYPE_SLOW_PROTO;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6, 0))
        en = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        en = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassStp_set(unit, type, en), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_LAYER2_PTP_LLDP_BYPASS_STP_DISABLE_ENABLE
/*
 * trap set rma layer2 ( ptp | lldp ) bypass-stp ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_layer2_ptp_lldp_bypass_stp_disable_enable(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_trap_bypassStpType_t    type;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('p' == TOKEN_CHAR(4, 0))
        type = BYPASS_STP_TYPE_PTP;
    else if('l' == TOKEN_CHAR(4, 0))
        type = BYPASS_STP_TYPE_LLDP;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassStp_set(unit, type, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_rma_layer2_ptp_lldp_bypass_stp_disable_enable */
#endif

#ifdef CMD_TRAP_SET_RMA_LAYER2_OAM_RMA_02_RMA_03_RMA_0E_RMA_10_RMA_20_RMA_21_RMA_22_RMA_0X_RMA_1X_RMA_2X_LEARN_DISABLE_ENABLE
/*
 *trap set rma layer2 ( oam | rma-02 | rma-03 | rma-0E | rma-10 | rma-20 | rma-21 | rma-22 | rma-0X | rma-1X | rma-2X) learn ( disable |enable)
 */
cparser_result_t cparser_cmd_trap_set_rma_layer2_oam_rma_02_rma_03_rma_0E_rma_10_rma_20_rma_21_rma_22_rma_0X_rma_1X_rma_2X_learn_disable_enable(
cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    rtk_trap_rmaGroup_frameType_t    rmaGroup_frameType;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    /*RMA Type*/
    if ('o' == TOKEN_CHAR(4,0))
    {
        rmaGroup_frameType = RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM;
    }
    else if ('r' == TOKEN_CHAR(4,0))
    {
        if (('0' == TOKEN_CHAR(4,4)) && ('2' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_SLOW_PROTOCOL_OTHER;
        }
        else if (('0' == TOKEN_CHAR(4,4)) && ('3' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_03;
        }
        else if (('0' == TOKEN_CHAR(4,4)) && ('E' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_0E_EXCEPT_PTP_LLDP;
        }
        else if (('1' == TOKEN_CHAR(4,4)) && ('0' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_10;
        }
        else if (('2' == TOKEN_CHAR(4,4)) && ('0' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_GMRP;
        }
        else if (('2' == TOKEN_CHAR(4,4)) && ('1' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_GVRP;
        }
        else if (('2' == TOKEN_CHAR(4,4)) && ('2' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_MSRP;
        }
        else if (('0' == TOKEN_CHAR(4,4)) && ('X' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_0X;
        }
        else if (('1' == TOKEN_CHAR(4,4)) && ('X' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_1X;
        }
        else if (('2' == TOKEN_CHAR(4,4)) && ('X' == TOKEN_CHAR(4,5)))
        {
            rmaGroup_frameType = RMA_GROUP_TYPE_2X;
        }
        else
        {
            diag_util_printf("User config: type Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    /*RMA learn or not*/
    if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaGroupLearningEnable_set(unit, rmaGroup_frameType, enable), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_rma_layer2_rma_tail_action_copy_to_cpu_drop_forward_trap_to_cpu */
#endif



#ifdef CMD_TRAP_SET_RMA_LAYER2_RMA_TAIL_LEARN_DISABLE_ENABLE
/*
 * trap set rma layer2 <UINT:rma_tail> learn ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_layer2_rma_tail_learn_disable_enable(
        cparser_context_t *context, uint32_t *rma_tail_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_mac_t       mac;
    rtk_enable_t    en;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    mac.octet[0] = 0x01;
    mac.octet[1] = 0x80;
    mac.octet[2] = 0xc2;
    mac.octet[3] = 0x00;
    mac.octet[4] = 0x00;
    mac.octet[5] = *rma_tail_ptr;

    if (mac.octet[5] > 0x2f)
    {
        diag_util_printf("Input error: the RMA address is out of range\n");
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6, 0))
        en = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        en = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaLearningEnable_set(unit, &mac, en), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_LAYER2_RMA_TAIL_RMA_TAIL_END_LEARN_DISABLE_ENABLE
/*
 * trap set rma layer2 <UINT:rma_tail> <UINT:rma_tail_end> learn ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_layer2_rma_tail_rma_tail_end_learn_disable_enable(
        cparser_context_t *context, uint32_t *rma_tail_ptr, uint32_t *rma_tail_end_ptr)
{
    uint32          unit = 0;
    uint32          ramTail = 0, ramTailEnd, i;
    int32           ret = RT_ERR_FAILED;
    rtk_mac_t       mac;
    rtk_enable_t    en;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    mac.octet[0] = 0x01;
    mac.octet[1] = 0x80;
    mac.octet[2] = 0xc2;
    mac.octet[3] = 0x00;
    mac.octet[4] = 0x00;
    mac.octet[5] = 0x00;

    ramTail = *rma_tail_ptr;
    ramTailEnd = *rma_tail_end_ptr;

    if (ramTail > 0x2f || ramTailEnd > 0x2f)
    {
        diag_util_printf("Input error: the RMA address is out of range\n");
        return CPARSER_NOT_OK;
    }
    if (ramTail > ramTailEnd)
    {
        diag_util_printf("Input error: the value of rma_tail_end should be bigger than htat of rma_tail\n");
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(7, 0))
        en = ENABLED;
    else if('d' == TOKEN_CHAR(7, 0))
        en = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    for (i = ramTail; i <= ramTailEnd; i++)
    {
        mac.octet[5] = i;
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaLearningEnable_set(unit, &mac, en), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_rma_layer2_rma_tail_rma_tail_end_learn_disable_enable */
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_LLDP_PTP_LEARN_DISABLE_ENABLE
/*
 * trap set rma port-rma ( lldp | ptp ) learn ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_lldp_ptp_learn_disable_enable(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_enable_t    en;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('l' == TOKEN_CHAR(4, 0))
        type = MGMT_TYPE_LLDP;
    else if('p' == TOKEN_CHAR(4, 0))
        type = MGMT_TYPE_PTP;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6, 0))
        en = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        en = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameLearningEnable_set(unit, type, en), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_EAPOL_LEARN_DISABLE_ENABLE
/*
 * trap set rma port-rma eapol learn ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_eapol_learn_disable_enable(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_enable_t    en;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6, 0))
        en = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        en = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    type = MGMT_TYPE_EAPOL;

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameLearningEnable_set(unit, type, en), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_RMA_PORT_RMA_BPDU_LEARN_DISABLE_ENABLE
/*
 * trap set rma port-rma bpdu learn ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_port_rma_bpdu_learn_disable_enable(
    cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_trap_mgmtType_t type;
    rtk_enable_t    en;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6, 0))
        en = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        en = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    type = MGMT_TYPE_BPDU;

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameLearningEnable_set(unit, type, en), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_rma_port_rma_bpdu_learn_disable_enable */
#endif

#ifdef CMD_TRAP_DUMP_RMA_PORT_RMA_BPDU_PORT_PORT_ALL
/*
 * trap dump rma port-rma bpdu port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_dump_rma_port_rma_bpdu_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    rtk_trap_mgmtType_t     type = MGMT_TYPE_BPDU;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;
#if defined(CONFIG_SDK_RTL8380)
    rtk_enable_t        enable;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

#if defined(CONFIG_SDK_RTL8380)
    diag_util_mprintf("Port | Action            | Learn    |\n");
    diag_util_mprintf("-----+-------------------+----------+\n");
#else
    diag_util_mprintf("Port | Action            |\n");
    diag_util_mprintf("-----+-------------------+\n");
#endif

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_printf("%5u", port);

        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_get(unit, port, type, &rma_action), ret);
        if (MGMT_ACTION_FORWARD == rma_action)
        {
            diag_util_printf("  forward      ");
        }
        else if (MGMT_ACTION_DROP == rma_action)
        {
            diag_util_printf("  drop         ");
        }
        else if (MGMT_ACTION_TRAP2CPU == rma_action)
        {
            diag_util_printf("  trap-to-cpu  ");
        }
        else if (MGMT_ACTION_COPY2CPU == rma_action)
        {
            diag_util_printf("  copy-to-cpu  ");
        }
        else if (MGMT_ACTION_FLOOD_TO_ALL_PORT == rma_action)
        {
            diag_util_printf(" forward-and-flood ");
        }
        else if (MGMT_ACTION_TRAP2MASTERCPU == rma_action)
        {
            diag_util_printf(" trap-to-master ");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

#if defined(CONFIG_SDK_RTL8380)
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameLearningEnable_get(unit, type, &enable), ret);

        if(ENABLED == enable)
            diag_util_printf("      enabled\n");
        else
            diag_util_printf("      disabled\n");
#else
        diag_util_mprintf("\n");
#endif
    }
        diag_util_mprintf("\n");


    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_DUMP_RMA_PORT_RMA_LLDP_PORT_PORT_ALL
/*
 * trap dump rma port-rma lldp port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_dump_rma_port_rma_lldp_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    rtk_trap_mgmtType_t     type = MGMT_TYPE_LLDP;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_enable_t            enable;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    diag_util_mprintf("Port | Action            | Learn    |\n");
    diag_util_mprintf("-----+-------------------+----------+\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_printf("%5u", port);

        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_get(unit, port, type, &rma_action), ret);
        if (MGMT_ACTION_FORWARD == rma_action)
        {
            diag_util_printf("  forward          ");
        }
        else if (MGMT_ACTION_DROP == rma_action)
        {
            diag_util_printf("  drop             ");
        }
        else if (MGMT_ACTION_TRAP2CPU == rma_action)
        {
            diag_util_printf("  trap-to-cpu      ");
        }
        else if (MGMT_ACTION_FLOOD_TO_ALL_PORT == rma_action)
        {
            diag_util_printf(" forward-and-flood ");
        }
        else if (MGMT_ACTION_TRAP2MASTERCPU == rma_action)
        {
            diag_util_printf(" trap-to-master    ");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameLearningEnable_get(unit, type, &enable), ret);

        if(ENABLED == enable)
            diag_util_printf("  enabled\n");
        else
            diag_util_printf("  disabled\n");
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_DUMP_RMA_PORT_RMA_PTP_PORT_PORT_ALL
/*
 * trap dump rma port-rma ptp port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_dump_rma_port_rma_ptp_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    rtk_trap_mgmtType_t     type = MGMT_TYPE_PTP;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_enable_t            enable;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    diag_util_mprintf("Port | Action            | Learn    \n");
    diag_util_mprintf("-----+-------------------+----------\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_printf("%5u", port);

        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_get(unit, port, type, &rma_action), ret);
        if (MGMT_ACTION_FORWARD == rma_action)
        {
            diag_util_printf("  forward          ");
        }
        else if (MGMT_ACTION_DROP == rma_action)
        {
            diag_util_printf("  drop             ");
        }
        else if (MGMT_ACTION_TRAP2CPU == rma_action)
        {
            diag_util_printf("  trap-to-cpu      ");
        }
        else if (MGMT_ACTION_COPY2CPU == rma_action)
        {
            diag_util_printf("  copy-to-cpu      ");
        }
        else if (MGMT_ACTION_FLOOD_TO_ALL_PORT == rma_action)
        {
            diag_util_printf(" forward-and-flood ");
        }
        else if (MGMT_ACTION_TRAP2MASTERCPU == rma_action)
        {
            diag_util_printf(" trap-to-master    ");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameLearningEnable_get(unit, type, &enable), ret);

        if(ENABLED == enable)
            diag_util_printf("  enabled\n");
        else
            diag_util_printf("  disabled\n");
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_DUMP_RMA_PORT_RMA_PTP_ETH_PORT_PORT_ALL
/*
 * trap dump rma port-rma ptp-eth port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_dump_rma_port_rma_ptp_eth_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    rtk_trap_mgmtType_t     type = MGMT_TYPE_PTP_ETH2;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_enable_t            enable;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    diag_util_mprintf("Port | Action            | Learn    \n");
    diag_util_mprintf("-----+-------------------+----------\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_printf("%5u", port);

        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_get(unit, port, type, &rma_action), ret);
        if (MGMT_ACTION_FORWARD == rma_action)
        {
            diag_util_printf("  forward          ");
        }
        else if (MGMT_ACTION_DROP == rma_action)
        {
            diag_util_printf("  drop             ");
        }
        else if (MGMT_ACTION_TRAP2CPU == rma_action)
        {
            diag_util_printf("  trap-to-cpu      ");
        }
        else if (MGMT_ACTION_COPY2CPU == rma_action)
        {
            diag_util_printf("  copy-to-cpu      ");
        }
        else if (MGMT_ACTION_FLOOD_TO_ALL_PORT == rma_action)
        {
            diag_util_printf(" forward-and-flood ");
        }
        else if (MGMT_ACTION_TRAP2MASTERCPU == rma_action)
        {
            diag_util_printf(" trap-to-master    ");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        type = MGMT_TYPE_PTP;
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameLearningEnable_get(unit, type, &enable), ret);

        if(ENABLED == enable)
            diag_util_printf("  enabled\n");
        else
            diag_util_printf("  disabled\n");
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_DUMP_RMA_PORT_RMA_PTP_UDP_PORT_PORT_ALL
/*
 * trap dump rma port-rma ptp-udp port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_dump_rma_port_rma_ptp_udp_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    rtk_trap_mgmtType_t     type = MGMT_TYPE_PTP_UDP;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = ACTION_FORWARD;
    rtk_enable_t            enable;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    diag_util_mprintf("Port | Action            | Learn    \n");
    diag_util_mprintf("-----+-------------------+----------\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_printf("%5u", port);

        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_get(unit, port, type, &rma_action), ret);
        if (MGMT_ACTION_FORWARD == rma_action)
        {
            diag_util_printf("  forward          ");
        }
        else if (MGMT_ACTION_DROP == rma_action)
        {
            diag_util_printf("  drop             ");
        }
        else if (MGMT_ACTION_TRAP2CPU == rma_action)
        {
            diag_util_printf("  trap-to-cpu      ");
        }
        else if (MGMT_ACTION_COPY2CPU == rma_action)
        {
            diag_util_printf("  copy-to-cpu      ");
        }
        else if (MGMT_ACTION_FLOOD_TO_ALL_PORT == rma_action)
        {
            diag_util_printf(" forward-and-flood ");
        }
        else if (MGMT_ACTION_TRAP2MASTERCPU == rma_action)
        {
            diag_util_printf(" trap-to-master    ");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        type = MGMT_TYPE_PTP;
        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameLearningEnable_get(unit, type, &enable), ret);

        if(ENABLED == enable)
            diag_util_printf("  enabled\n");
        else
            diag_util_printf("  disabled\n");
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_DUMP_RMA_PORT_RMA_EAPOL_PORT_PORT_ALL
/*
 * trap dump rma port-rma eapol port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_dump_rma_port_rma_eapol_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    rtk_trap_mgmtType_t     type = MGMT_TYPE_EAPOL;
    int32                   ret = RT_ERR_FAILED;
    rtk_mgmt_action_t       rma_action = MGMT_ACTION_FORWARD;
    rtk_enable_t            enable;
    rtk_port_t              port = 0;
    diag_portlist_t         portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    diag_util_mprintf("Port | Action            | Learn    |\n");
    diag_util_mprintf("-----+-------------------+----------+\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_printf("%5u", port);

        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_get(unit, port, type, &rma_action), ret);
        if (MGMT_ACTION_FORWARD == rma_action)
        {
            diag_util_printf("  forward          ");
        }
        else if (MGMT_ACTION_DROP == rma_action)
        {
            diag_util_printf("  drop             ");
        }
        else if (MGMT_ACTION_TRAP2CPU == rma_action)
        {
            diag_util_printf("  trap-to-cpu      ");
        }
        else if (MGMT_ACTION_COPY2CPU == rma_action)
        {
            diag_util_printf("  copy-to-cpu      ");
        }
        else if (MGMT_ACTION_FLOOD_TO_ALL_PORT == rma_action)
        {
            diag_util_printf(" forward-and-flood ");
        }
        else if (MGMT_ACTION_TRAP2MASTERCPU == rma_action)
        {
            diag_util_printf(" trap-to-master    ");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameLearningEnable_get(unit, type, &enable), ret);

        if(ENABLED == enable)
            diag_util_printf("  enabled\n");
        else
            diag_util_printf("  disabled\n");
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_MANAGEMENT_VLAN_STATE
/*
 * trap get management-vlan state
 */
cparser_result_t
cparser_cmd_trap_get_management_vlan_state(
        cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_enable_t        enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameMgmtVlanEnable_get(unit, &enable), ret);
    diag_util_printf("Management VLAN state: ");
    if(ENABLED == enable)
        diag_util_printf("%s\n",DIAG_STR_ENABLE);
    else if(DISABLED == enable)
        diag_util_printf("%s\n", DIAG_STR_DISABLE);
    else
        diag_util_printf("%s\n", DIAG_STR_DISABLE);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_get_management_vlan_state */
#endif

#ifdef CMD_TRAP_SET_MANAGEMENT_VLAN_STATE_DISABLE_ENABLE
/*
 * trap set management-vlan state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_management_vlan_state_disable_enable(
        cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    en;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4, 0))
        en = ENABLED;
    else if('d' == TOKEN_CHAR(4, 0))
        en = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameMgmtVlanEnable_set(unit, en), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_management_vlan_state_disable_enable */
#endif

#ifdef CMD_TRAP_GET_SELF_ARP_STATE
/*
 * trap get self-arp state
 */
cparser_result_t
cparser_cmd_trap_get_self_arp_state(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_enable_t        enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameSelfARPEnable_get(unit, &enable), ret);
    diag_util_printf("Self-ARP state: ");
    if(ENABLED == enable)
        diag_util_printf("%s\n",DIAG_STR_ENABLE);
    else if(DISABLED == enable)
        diag_util_printf("%s\n", DIAG_STR_DISABLE);
    else
        diag_util_printf("%s\n", DIAG_STR_DISABLE);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_get_management_vlan_state */
#endif

#ifdef CMD_TRAP_SET_SELF_ARP_STATE_DISABLE_ENABLE
/*
 * trap set self-arp state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_self_arp_state_disable_enable(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    en;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4, 0))
        en = ENABLED;
    else if('d' == TOKEN_CHAR(4, 0))
        en = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameSelfARPEnable_set(unit, en), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_management_vlan_state_disable_enable */
#endif

#ifdef CMD_TRAP_GET_RMA_LAYER2_PTP_LLDP_BYPASS_VLAN
/*
 * trap get rma layer2 ( ptp | lldp ) bypass-vlan
 */
cparser_result_t
cparser_cmd_trap_get_rma_layer2_ptp_lldp_bypass_vlan(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_trap_bypassVlanType_t   type;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('p' == TOKEN_CHAR(4, 0))
        type = BYPASS_VLAN_TYPE_PTP;
    else if('l' == TOKEN_CHAR(4, 0))
        type = BYPASS_VLAN_TYPE_LLDP;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassVlan_get(unit, type, &enable), ret);

    diag_util_mprintf("RMA layer 2 %s bypass VLAN: %s\n",
        TOKEN_STR(4), (ENABLED == enable)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_rma_layer2_ptp_lldp_bypass_vlan */
#endif

#ifdef CMD_TRAP_SET_RMA_LAYER2_PTP_LLDP_BYPASS_VLAN_DISABLE_ENABLE
/*
 * trap set rma layer2 ( ptp | lldp ) bypass-vlan ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_layer2_ptp_lldp_bypass_vlan_disable_enable(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_trap_bypassVlanType_t   type;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('p' == TOKEN_CHAR(4, 0))
        type = BYPASS_VLAN_TYPE_PTP;
    else if('l' == TOKEN_CHAR(4, 0))
        type = BYPASS_VLAN_TYPE_LLDP;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassVlan_set(unit, type, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_rma_layer2_ptp_lldp_bypass_vlan_disable_enable */
#endif


#ifdef CMD_TRAP_SET_RMA_LAYER2_BPDU_OAM_RMA_02_RMA_03_RMA_0E_RMA_0X_RMA_20_RMA_21_BYPASS_VLAN_DISABLE_ENABLE
/*
 * trap set rma layer2 ( bpdu | oam | rma-02 | rma-03 | rma-0E | rma-0X | rma-20 | rma-21 ) bypass-vlan ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_layer2_bpdu_oam_rma_02_rma_03_rma_0E_rma_0X_rma_20_rma_21_bypass_vlan_disable_enable(
cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_trap_bypassVlanType_t   type;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if( 'b' == (TOKEN_CHAR(4, 0)))
    {
         type = BYPASS_VLAN_TYPE_RMA_00;
    }
    else if( 'o' == (TOKEN_CHAR(4, 0)))
    {
         type = BYPASS_VLAN_TYPE_OAM;
    }
    else if( 'r' == (TOKEN_CHAR(4, 0)))
    {
        switch (TOKEN_CHAR(4, 5))
        {
            case '2':
                type = BYPASS_VLAN_TYPE_RMA_02;
                break;
            case '3':
                type = BYPASS_VLAN_TYPE_RMA_03;
                break;
            case 'E':
                type = BYPASS_VLAN_TYPE_RMA_0E;
                break;
              case '0':
                type = BYPASS_VLAN_TYPE_GMRP;
                break;
              case '1':
                type = BYPASS_VLAN_TYPE_GVRP;
                break;
            case 'X':
                type = BYPASS_VLAN_TYPE_RMA_0X;
                break;
            default:
                diag_util_printf("User config: type Error!\n");
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassVlan_set(unit, type, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_rma_layer2_bpdu_oam_rma_02_rma_03_rma_0E_rma_0X_rma_20_rma_21_bypass_vlan_disable_enable */
#endif

#ifdef CMD_TRAP_GET_RMA_LAYER2_BPDU_RMA_02_RMA_0E_RMA_0X_BYPASS_VLAN
/*
 * trap get rma layer2 ( bpdu | rma-02 | rma-0E | rma-0X ) bypass-vlan
 */
cparser_result_t
cparser_cmd_trap_get_rma_layer2_bpdu_rma_02_rma_0E_rma_0X_bypass_vlan(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_trap_bypassVlanType_t   type;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if( 'b' == (TOKEN_CHAR(4, 0)))
    {
         type = BYPASS_VLAN_TYPE_RMA_00;
    }
    else if( 'r' == (TOKEN_CHAR(4, 0)))
    {
        switch (TOKEN_CHAR(4, 5))
        {
            case '2':
                type = BYPASS_VLAN_TYPE_RMA_02;
                break;
            case 'E':
                type = BYPASS_VLAN_TYPE_RMA_0E;
                break;
            case 'X':
                type = BYPASS_VLAN_TYPE_RMA_0X;
                break;
            default:
                diag_util_printf("User config: type Error!\n");
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassVlan_get(unit, type, &enable), ret);

    diag_util_mprintf("RMA layer 2 %s bypass VLAN: %s\n",
        TOKEN_STR(4), (ENABLED == enable)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_rma_layer2_bpdu_rma_02_rma_0e_rma_0x_bypass_vlan */
#endif

#ifdef CMD_TRAP_GET_RMA_LAYER2_BPDU_OAM_RMA_02_RMA_03_RMA_0E_RMA_20_RMA_21_RMA_0X_BYPASS_VLAN
/*
 * trap get rma layer2 ( bpdu | oam | rma-02 | rma-03 |rma-0E |rma-20 |rma-21 |rma-0X ) bypass-vlan
 */
cparser_result_t
cparser_cmd_trap_get_rma_layer2_bpdu_oam_rma_02_rma_03_rma_0E_rma_20_rma_21_rma_0X_bypass_vlan(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_trap_bypassVlanType_t   type;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if( 'b' == (TOKEN_CHAR(4, 0)))
    {
         type = BYPASS_VLAN_TYPE_RMA_00;
    }
    else if( 'o' == (TOKEN_CHAR(4, 0)))
    {
         type = BYPASS_VLAN_TYPE_OAM;
    }
    else if( 'r' == (TOKEN_CHAR(4, 0)))
    {
        switch (TOKEN_CHAR(4, 5))
        {
            case '2':
                type = BYPASS_VLAN_TYPE_RMA_02;
                break;
            case '3':
                type = BYPASS_VLAN_TYPE_RMA_03;
                break;
            case 'E':
                type = BYPASS_VLAN_TYPE_RMA_0E;
                break;
              case '0':
                type = BYPASS_VLAN_TYPE_GMRP;
                break;
              case '1':
                type = BYPASS_VLAN_TYPE_GVRP;
                break;
            case 'X':
                type = BYPASS_VLAN_TYPE_RMA_0X;
                break;
            default:
                diag_util_printf("User config: type Error!\n");
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassVlan_get(unit, type, &enable), ret);

    diag_util_mprintf("RMA layer 2 %s bypass VLAN: %s\n",
        TOKEN_STR(4), (ENABLED == enable)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;

}   /* end of cparser_cmd_trap_get_rma_layer2_bpdu_oam_rma_02_rma_03_rma_0E_rma_20_rma_21_rma_0X_bypass_vlan */
#endif


#ifdef CMD_TRAP_SET_RMA_LAYER2_BPDU_RMA_02_RMA_0E_RMA_0X_BYPASS_VLAN_DISABLE_ENABLE
/*
 * trap set rma layer2 ( bpdu | rma-02 | rma-0E | rma-0X ) bypass-vlan ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_layer2_bpdu_rma_02_rma_0E_rma_0X_bypass_vlan_disable_enable(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_trap_bypassVlanType_t   type;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if( 'b' == (TOKEN_CHAR(4, 0)))
    {
         type = BYPASS_VLAN_TYPE_RMA_00;
    }
    else if( 'r' == (TOKEN_CHAR(4, 0)))
    {
        switch (TOKEN_CHAR(4, 5))
        {
            case '2':
                type = BYPASS_VLAN_TYPE_RMA_02;
                break;
            case 'E':
                type = BYPASS_VLAN_TYPE_RMA_0E;
                break;
            case 'X':
                type = BYPASS_VLAN_TYPE_RMA_0X;
                break;
            default:
                diag_util_printf("User config: type Error!\n");
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassVlan_set(unit, type, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_rma_layer2_bpdu_rma_02_rma_0E_rma_0X_bypass_vlan_disable_enable */
#endif

#ifdef CMD_TRAP_GET_RMA_L2_USER_DEFINE_INDEX_BYPASS_VLAN
/*
 * trap get rma l2-user-define <UINT:index> bypass-vlan
 */
cparser_result_t
cparser_cmd_trap_get_rma_l2_user_define_index_bypass_vlan(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_enable_t                enable;
    uint32                      bypassVlanList[] = {
                                    BYPASS_VLAN_TYPE_USER_DEF_0,
                                    BYPASS_VLAN_TYPE_USER_DEF_1,
                                    BYPASS_VLAN_TYPE_USER_DEF_2,
                                    BYPASS_VLAN_TYPE_USER_DEF_3,
                                    BYPASS_VLAN_TYPE_USER_DEF_4,
                                    BYPASS_VLAN_TYPE_USER_DEF_5};

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassVlan_get(unit, bypassVlanList[*index_ptr], &enable), ret);

    diag_util_mprintf("RMA layer 2 user defined %d bypass VLAN: %s\n",
        *index_ptr, (ENABLED == enable)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_rma_l2_user_define_bypass_vlan */
#endif

#ifdef CMD_TRAP_SET_RMA_L2_USER_DEFINE_INDEX_BYPASS_VLAN_DISABLE_ENABLE
/*
 * trap set rma l2-user-define <UINT:index> bypass-vlan ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_l2_user_define_index_bypass_vlan_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_trap_bypassVlanType_t   type;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch (*index_ptr)
    {
        case 0:
            type = BYPASS_VLAN_TYPE_USER_DEF_0;
            break;
        case 1:
            type = BYPASS_VLAN_TYPE_USER_DEF_1;
            break;
        case 2:
            type = BYPASS_VLAN_TYPE_USER_DEF_2;
            break;
        case 3:
            type = BYPASS_VLAN_TYPE_USER_DEF_3;
            break;

        default:
            diag_util_printf("User config: index Error!\n");
            return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassVlan_set(unit, type, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_rma_l2_user_define_index_bypass_vlan_disable_enable */
#endif

#ifdef CMD_TRAP_GET_RMA_LAYER2_PTP_LLDP_BYPASS_STP
/*
 * trap get rma layer2 ( ptp | lldp ) bypass-stp
 */
cparser_result_t
cparser_cmd_trap_get_rma_layer2_ptp_lldp_bypass_stp(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_trap_bypassVlanType_t   type;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('p' == TOKEN_CHAR(4, 0))
        type = BYPASS_STP_TYPE_PTP;
    else if('l' == TOKEN_CHAR(4, 0))
        type = BYPASS_STP_TYPE_LLDP;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassStp_get(unit, type, &enable), ret);

    diag_util_mprintf("RMA layer 2 %s bypass STP: %s\n",
        TOKEN_STR(4), (ENABLED == enable)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_rma_layer2_ptp_lldp_bypass_stp */
#endif

#ifdef CMD_TRAP_GET_RMA_LAYER2_RMA_TAIL_0X_SLOW_PROTO_BYPASS_STP
/*
 * trap get rma layer2 ( rma-tail-0X | slow-proto ) bypass-stp
 */
cparser_result_t
cparser_cmd_trap_get_rma_layer2_rma_tail_0X_slow_proto_bypass_stp(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_trap_bypassVlanType_t   type;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('r' == TOKEN_CHAR(4, 0))
        type = BYPASS_STP_TYPE_RMA_0X;
    else if('s' == TOKEN_CHAR(4, 0))
        type = BYPASS_STP_TYPE_SLOW_PROTO;
    else
    {
        diag_util_printf("User config: type Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassStp_get(unit, type, &enable), ret);

    diag_util_mprintf("RMA layer 2 %s bypass STP: %s\n",
        TOKEN_STR(4), (ENABLED == enable)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_rma_layer2_rma_tail_0x_slow_proto_bypass_stp */
#endif

#ifdef CMD_TRAP_GET_RMA_L2_USER_DEFINE_INDEX_BYPASS_STP
/*
 * trap get rma l2-user-define <UINT:index> bypass-stp
 */
cparser_result_t
cparser_cmd_trap_get_rma_l2_user_define_index_bypass_stp(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_enable_t                enable;
    uint32                      bypassStpList[] = {
                                    BYPASS_STP_TYPE_USER_DEF_0,
                                    BYPASS_STP_TYPE_USER_DEF_1,
                                    BYPASS_STP_TYPE_USER_DEF_2,
                                    BYPASS_STP_TYPE_USER_DEF_3,
                                    BYPASS_STP_TYPE_USER_DEF_4,
                                    BYPASS_STP_TYPE_USER_DEF_5};

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_bypassStp_get(unit, bypassStpList[*index_ptr], &enable), ret);

    diag_util_mprintf("RMA layer 2 user defined %d bypass STP: %s\n",
        bypassStpList[*index_ptr], (ENABLED == enable)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_RMA_CANCEL_MIRROR
/*
 * trap get rma cancel-mirror
 */
cparser_result_t
cparser_cmd_trap_get_rma_cancel_mirror(cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_enable_t                enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaCancelMirror_get(unit, &enable), ret);

    diag_util_mprintf("RMA cancel mirror state: %s\n",(ENABLED == enable) ? DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_rma_cancel_mirror */
#endif

#ifdef CMD_TRAP_SET_RMA_CANCEL_MIRROR_DISABLE_ENABLE
/*
 * trap set rma cancel-mirror ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_cancel_mirror_disable_enable(cparser_context_t *context)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(4,0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(4,0))
        enable = DISABLED;
    else
    {
        diag_util_printf("User config: state Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaCancelMirror_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_GET_REASON_CFI_PRIORITY
/*
*trap get reason cfi priority
*/
cparser_result_t cparser_cmd_trap_get_reason_cfi_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_CFI, &pri), ret);
    diag_util_mprintf("Cfi=1 Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_cfi_priority */
#endif

#ifdef CMD_TRAP_GET_REASON_RLPP_RLDP_PRIORITY
/*
*trap get reason rlpp-rldp priority
*/
cparser_result_t cparser_cmd_trap_get_reason_rlpp_rldp_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_RLDP_RLPP, &pri), ret);
    diag_util_mprintf("rldp&rlpp Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_rlpp_rldp_priority */
#endif


#ifdef CMD_TRAP_GET_REASON_SYSTEM_MAC_CONSTRAIN_PRIORITY
/*
*trap get reason system-mac-constrain priority
*/
cparser_result_t cparser_cmd_trap_get_reason_system_mac_constrain_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_MAC_CST_SYS, &pri), ret);
    diag_util_mprintf("System mac cosntrain Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_system_mac_constrain_priority */
#endif


#ifdef CMD_TRAP_GET_REASON_PORT_MAC_CONSTRAIN_PRIORITY
/*
*trap get reason port-mac-constrain priority
*/
cparser_result_t cparser_cmd_trap_get_reason_port_mac_constrain_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_MAC_CST_PORT, &pri), ret);
    diag_util_mprintf("Port mac cosntrain Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_port_mac_constrain_priority */
#endif


#ifdef CMD_TRAP_GET_REASON_VLAN_MAC_CONSTRAIN_PRIORITY
/*
*trap get reason vlan-mac-constrain priority
*/
cparser_result_t cparser_cmd_trap_get_reason_vlan_mac_constrain_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_MAC_CST_VLAN, &pri), ret);
    diag_util_mprintf("Vlan mac cosntrain Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_vlan_mac_constrain_priority */
#endif


#ifdef CMD_TRAP_GET_REASON_RMA_PRIORITY
/*
*trap get reason rma priority
*/
cparser_result_t cparser_cmd_trap_get_reason_rma_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_RMA, &pri), ret);
    diag_util_mprintf("RMA Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_rma_priority */
#endif


#ifdef CMD_TRAP_GET_REASON_SPECIAL_COPY_PRIORITY
/*
*trap get reason special-copy priority
*/
cparser_result_t cparser_cmd_trap_get_reason_special_copy_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_SPECIAL_COPY, &pri), ret);
    diag_util_mprintf("Special Copy Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_special_copy_priority */
#endif


#ifdef CMD_TRAP_GET_REASON_SPECIAL_TRAP_PRIORITY
/*
*trap get reason special-trap priority
*/
cparser_result_t cparser_cmd_trap_get_reason_special_trap_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_SPECIAL_TRAP, &pri), ret);
    diag_util_mprintf("Special Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_special_trap_priority */
#endif


#ifdef CMD_TRAP_GET_REASON_ROUTING_EXCEPTION_PRIORITY
/*
*trap get reason routing-exception priority
*/
cparser_result_t cparser_cmd_trap_get_reason_routing_exception_priority(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_pri_t       pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_get(unit, MGMT_TYPE_ROUT_EXCEPT, &pri), ret);
    diag_util_mprintf("Routing exception Trap Priority : %u\n", pri);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_reason_routing_exception_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_CFI_PRIORITY_PRIORITY
/*
*trap set reason cfi priority <UINT:priority>
*/
cparser_result_t cparser_cmd_trap_set_reason_cfi_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_CFI, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_cfi_priority_priority */
#endif


#ifdef CMD_TRAP_SET_REASON_RLPP_RLDP_PRIORITY_PRIORITY
/*
*trap set reason rlpp-rldp priority <UINT:priority>
*/
cparser_result_t cparser_cmd_trap_set_reason_rlpp_rldp_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_RLDP_RLPP, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_rlpp_rpdp_priority_priority */
#endif


#ifdef CMD_TRAP_SET_REASON_SYSTEM_MAC_CONSTRAIN_PRIORITY_PRIORITY
/*
*trap set reason system-mac-constrain priority <UINT:priority>
*/
cparser_result_t cparser_cmd_trap_set_reason_system_mac_constrain_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_MAC_CST_SYS, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_system_mac_constrain_priority_priority */
#endif


#ifdef CMD_TRAP_SET_REASON_PORT_MAC_CONSTRAIN_PRIORITY_PRIORITY
/*
*trap set reason port-mac-constrain priority <UINT:priority>
*/
cparser_result_t cparser_cmd_trap_set_reason_port_mac_constrain_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_MAC_CST_PORT, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_port_mac_constrain_priority_priority */
#endif


#ifdef CMD_TRAP_SET_REASON_VLAN_MAC_CONSTRAIN_PRIORITY_PRIORITY
/*
*trap set reason vlan-mac-constrain priority <UINT:priority>
*/
cparser_result_t cparser_cmd_trap_set_reason_vlan_mac_constrain_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_MAC_CST_VLAN, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_vlan_mac_constrain_priority_priority */
#endif

#ifdef CMD_TRAP_SET_REASON_RMA_PRIORITY_PRIORITY
/*
*trap set reason rma priority <UINT:priority>
*/
cparser_result_t cparser_cmd_trap_set_reason_rma_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_RMA, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_rma_priority_priority */
#endif


#ifdef CMD_TRAP_SET_REASON_SPECIAL_COPY_PRIORITY_PRIORITY
/*
*trap set reason special-copy priority <UINT:priority>
*/
cparser_result_t cparser_cmd_trap_set_reason_special_copy_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_SPECIAL_COPY, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_special_copy_priority_priority */
#endif


#ifdef CMD_TRAP_SET_REASON_SPECIAL_TRAP_PRIORITY_PRIORITY
/*
*trap set reason special-trap priority <UINT:priority>
*/
cparser_result_t cparser_cmd_trap_set_reason_special_trap_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_SPECIAL_TRAP, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_special_trap_priority_priority */
#endif


#ifdef CMD_TRAP_SET_REASON_ROUTING_EXCEPTION_PRIORITY_PRIORITY
/*
*trap set reason routing-exception priority <UINT:priority>
*/
cparser_result_t cparser_cmd_trap_set_reason_routing_exception_priority_priority(cparser_context_t *context,
    uint32_t *priority_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_pri_t   pri = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    pri = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFramePri_set(unit, MGMT_TYPE_ROUT_EXCEPT, pri), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_trap_set_reason_routing_exception_priority_priority */
#endif

#ifdef CMD_TRAP_GET_RMA_LOOKUP_MISS_ACTION_STATE
/*
 * trap get rma lookup-miss-action state
 */
cparser_result_t
cparser_cmd_trap_get_rma_lookup_miss_action_state(
    cparser_context_t *context)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaLookupMissActionEnable_get(unit, &enable), ret);
    diag_util_mprintf("RMA care lookup miss action : ");
    if (ENABLED == enable)
        diag_util_mprintf("enable\n");
    else
        diag_util_mprintf("disable\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_rma_lookup_miss_action_state */
#endif

#ifdef CMD_TRAP_SET_RMA_LOOKUP_MISS_ACTION_STATE_DISABLE_ENABLE
/*
 * trap set rma lookup-miss-action state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_rma_lookup_miss_action_state_disable_enable(
    cparser_context_t *context)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(5,0))
        enable = DISABLED;
    else
        enable = ENABLED;

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaLookupMissActionEnable_set(unit, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_rma_lookup_miss_action_state_disable_enable */
#endif

#ifdef CMD_TRAP_GET_NEXTHOP_AGE_OUT_ACTION
/*
 * trap get nexthop-age-out action
 */
cparser_result_t
cparser_cmd_trap_get_nexthop_age_out_action(
    cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_action_t                    action;
    rtk_trap_routeExceptionType_t   type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    type = ROUTE_EXCEPTION_TYPE_ENTRY_AGE_OUT;

    ret = rtk_trap_routeExceptionAction_get(unit, type, &action);
    if (ret!= RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Route next-hop age out action: ");
    if (ACTION_DROP == action)
        diag_util_mprintf("Drop\n");
    else if (ACTION_FORWARD == action)
        diag_util_mprintf("L2 forward\n");
    else if (ACTION_TRAP2CPU == action)
        diag_util_mprintf("Trap\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_nexthop_age_out_action */
#endif

#ifdef CMD_TRAP_SET_NEXTHOP_AGE_OUT_ACTION_DROP_L2_FORWARD_TRAP
/*
 * trap set nexthop-age-out action ( drop | l2-forward | trap )
 */
cparser_result_t
cparser_cmd_trap_set_nexthop_age_out_action_drop_l2_forward_trap(
    cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_action_t                    act;
    rtk_trap_routeExceptionType_t   type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    type = ROUTE_EXCEPTION_TYPE_ENTRY_AGE_OUT;

    if('d' == TOKEN_CHAR(4, 0))
        act = ACTION_DROP;
    else if('l' == TOKEN_CHAR(4, 0))
        act = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(4, 0))
        act = ACTION_TRAP2CPU;
    else
    {
        diag_util_printf("User config: trap action Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_routeExceptionAction_set(unit, type, act), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_nexthop_age_out_action_drop_l2_forward_trap */
#endif

#ifdef CMD_TRAP_GET_CFM_TRAP_TARGET
/*
 * trap get cfm trap-target
 */
cparser_result_t
cparser_cmd_trap_get_cfm_trap_target(
    cparser_context_t *context)
{
    rtk_trapTarget_t    target;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_cfmTarget_get(unit, &target), ret);

    diag_util_mprintf("CFM trap target: ");
    if (RTK_TRAP_LOCAL == target)
        diag_util_mprintf("Local\n");
    else
        diag_util_mprintf("Master\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_cfm_trap_target */
#endif

#ifdef CMD_TRAP_SET_CFM_TRAP_TARGET_LOCAL_MASTER
/*
 * trap set cfm trap-target ( local | master )
 */
cparser_result_t
cparser_cmd_trap_set_cfm_trap_target_local_master(
    cparser_context_t *context)
{
    rtk_trapTarget_t    target;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('l' == TOKEN_CHAR(4, 0))
        target = RTK_TRAP_LOCAL;
    else
        target = RTK_TRAP_MASTER;

    DIAG_UTIL_ERR_CHK(rtk_trap_cfmTarget_set(unit, target), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_cfm_trap_target_local_master */
#endif

#ifdef CMD_TRAP_GET_OAM_TRAP_TARGET
/*
 * trap get oam trap-target
 */
cparser_result_t
cparser_cmd_trap_get_oam_trap_target(
    cparser_context_t *context)
{
    rtk_trapTarget_t    target;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_oamTarget_get(unit, &target), ret);

    diag_util_mprintf("OAM PDU trap target: ");
    if (RTK_TRAP_LOCAL == target)
        diag_util_mprintf("Local\n");
    else
        diag_util_mprintf("Master\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_oam_trap_target */
#endif

#ifdef CMD_TRAP_SET_OAM_TRAP_TARGET_LOCAL_MASTER
/*
 * trap set oam trap-target ( local | master )
 */
cparser_result_t
cparser_cmd_trap_set_oam_trap_target_local_master(
    cparser_context_t *context)
{
    rtk_trapTarget_t    target;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('l' == TOKEN_CHAR(4, 0))
        target = RTK_TRAP_LOCAL;
    else
        target = RTK_TRAP_MASTER;

    DIAG_UTIL_ERR_CHK(rtk_trap_oamTarget_set(unit, target), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_oam_trap_target_local_master */
#endif

#ifdef CMD_TRAP_GET_MGMT_FRAME_TRAP_TARGET
/*
 * trap get mgmt-frame trap-target
 */
cparser_result_t
cparser_cmd_trap_get_mgmt_frame_trap_target(
    cparser_context_t *context)
{
    rtk_trapTarget_t    target;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameTarget_get(unit, &target), ret);

    diag_util_mprintf("Management frame trap target: ");
    if (RTK_TRAP_LOCAL == target)
        diag_util_mprintf("Local\n");
    else
        diag_util_mprintf("Master\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_get_mgmt_frame_trap_target */
#endif

#ifdef CMD_TRAP_SET_MGMT_FRAME_TRAP_TARGET_LOCAL_MASTER
/*
 * trap set mgmt-frame trap-target ( local | master )
 */
cparser_result_t
cparser_cmd_trap_set_mgmt_frame_trap_target_local_master(
    cparser_context_t *context)
{
    rtk_trapTarget_t    target;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('l' == TOKEN_CHAR(4, 0))
        target = RTK_TRAP_LOCAL;
    else
        target = RTK_TRAP_MASTER;

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameTarget_set(unit, target), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_mgmt_frame_trap_target_local_master */
#endif

#ifdef CMD_TRAP_DUMP_GRATUITOUS_ARP_PORT_ALL
/*
 * trap dump gratuitous-arp ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_trap_dump_gratuitous_arp_port_all(
    cparser_context_t *context,
    char **port_ptr)
{
    rtk_port_t              port;
    diag_portlist_t         portlist;
    rtk_trap_mgmtType_t     frameType;
    rtk_mgmt_action_t       action;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Port | Action      |\n");
    diag_util_mprintf("-----+-------------+\n");

    frameType = MGMT_GRATUITOUS_ARP;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_printf("%5u", port);

        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_get(unit, port, frameType, &action), ret);

        if (MGMT_ACTION_FORWARD == action)
        {
            diag_util_printf("  forward      ");
        }
        else if (MGMT_ACTION_DROP == action)
        {
            diag_util_printf("  drop         ");
        }
        else if (MGMT_ACTION_TRAP2CPU == action)
        {
            diag_util_printf("  trap-to-cpu  ");
        }
        else if (MGMT_ACTION_COPY2CPU == action)
        {
            diag_util_printf("  copy-to-cpu  ");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_dump_gratuitous_arp_port_all */
#endif

#ifdef CMD_TRAP_SET_GRATUITOUS_ARP_PORT_ALL_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU
/*
 * trap set gratuitous-arp ( <PORT_LIST:port> | all ) ( drop | forward | trap-to-cpu | copy-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_gratuitous_arp_port_all_drop_forward_trap_to_cpu_copy_to_cpu(
    cparser_context_t *context,
    char **port_ptr)
{
    rtk_port_t              port;
    diag_portlist_t         portlist;
    rtk_trap_mgmtType_t     frameType;
    rtk_mgmt_action_t       action;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    frameType = MGMT_GRATUITOUS_ARP;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_TRAP_PARSE_MGNT_ACTION(4, action);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portMgmtFrameAction_set(unit, port, frameType, action), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trap_set_gratuitous_arp_port_all_drop_forward_trap_to_cpu_copy_to_cpu */
#endif

#ifdef CMD_TRAP_SET_REASON_ARP_DHCP_IGMP_MLD_BPDU_PTP_LLDP_EAPOL_OAM_LACP_QUEUE_ID_QUEUE_ID
/*
 * trap set reason ( arp | dhcp | igmp-mld | bpdu | ptp | lldp | eapol | oam | lacp ) queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_arp_dhcp_igmp_mld_bpdu_ptp_lldp_eapol_oam_lacp_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t   qId = 0;
    rtk_trap_qType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    if ('a' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_ARP_REQ_REP_GRTS;
    else if ('d' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_DHCP_DHCP6;
    else if ('i' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_IGMP_MLD;
    else if ('b' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_BPDU;
    else if ('p' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_PTP;
    else if ('l' == TOKEN_CHAR(3, 0))
    {
        if ('l' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_LLDP;
        else if ('a' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_LACP;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else if ('e' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_EAPOL;
    else if ('o' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_OAM;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_USR_DEF_RMA_RMA_IP_HDR_ERR_CRC_ERR_CHECKSUM_ERR_QUEUE_ID_QUEUE_ID
/*
 * trap set reason ( usr-def-rma | rma | ip-hdr-err | crc-err | checksum-err ) queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_usr_def_rma_rma_ip_hdr_err_crc_err_checksum_err_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t   qId = 0;
    rtk_trap_qType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    if ('u' == TOKEN_CHAR(3, 0))
    {
        type = TRAP_Q_RMA_USR_DEF;
    }
    else if ('r' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_RMA;
    else if ('i' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_IP4_IP6_HDR_ERR;
    else if ('c' == TOKEN_CHAR(3, 0))
    {
        if ('r' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_L2_CRC_ERR;
        else if ('h' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_IP4_CHKSUM_ERR;
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

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_IP_RSVD_ADDR_INGR_VLAN_FILTER_CFI_IVC_INVALID_SA_MAC_CONSTRAINT_NEW_SA_PMV_FORBID_STTC_PMV_DYNM_PMV_HASH_FULL_ATTACK_ACL_MIRROR_QUEUE_ID_QUEUE_ID
/*
 * trap set reason ( ip-rsvd-addr | ingr-vlan-filter | cfi | ivc | invalid-sa | mac-constraint | new-sa | pmv-forbid | sttc-pmv | dynm-pmv | hash-full | attack | acl | mirror ) queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_ip_rsvd_addr_ingr_vlan_filter_cfi_ivc_invalid_sa_mac_constraint_new_sa_pmv_forbid_sttc_pmv_dynm_pmv_hash_full_attack_acl_mirror_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t   qId = 0;
    rtk_trap_qType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    if ('i' == TOKEN_CHAR(3, 0))
    {
        if ('-' == TOKEN_CHAR(3, 2))
            type = TRAP_Q_IP4_IP6_RSVD_ADDR;
        else if ('g' == TOKEN_CHAR(3, 2))
            type = TRAP_Q_IGR_VLAN_FLTR;
        else if ('c' == TOKEN_CHAR(3, 2))
            type = TRAP_Q_IVC;
        else if ('v' == TOKEN_CHAR(3, 2))
            type = TRAP_Q_INVALID_SA;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else if ('c' == TOKEN_CHAR(3, 0))
    {
        if ('f' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_CFI;
        else if ('a' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_CAPWAP;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else if ('m' == TOKEN_CHAR(3, 0))
    {
        if ('a' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_MAC_CST;
        else if ('i' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_MIR_HIT;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else if ('n' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_NEW_SA;
    else if ('p' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_PMV_FORBID;
    else if ('s' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_L2_STTC_PMV;
    else if ('d' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_L2_DYN_PMV;
    else if ('h' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_HASH_FULL;
    else if ('a' == TOKEN_CHAR(3, 0))
    {
        if ('t' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_ATK;
        else if ('c' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_ACL_HIT;
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

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_IPUC_RPF_IPMC_RPF_L2_LOOKUP_MIS_L3_BDG_LOOKUP_MIS_ROUTER_MAC_INTF_ROUTER_MAC_NON_IP_EXCEPT_ROUTING_IP_ROUTING_DIP_DMAC_MIS_MATCH_QUEUE_ID_QUEUE_ID
/*
 * trap set reason ( ipuc-rpf | ipmc-rpf | l2-lookup-mis | l3-bdg-lookup-mis | router-mac-intf | router-mac-non-ip | except-routing-ip | routing-dip-dmac-mis-match ) queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_ipuc_rpf_ipmc_rpf_l2_lookup_mis_l3_bdg_lookup_mis_router_mac_intf_router_mac_non_ip_except_routing_ip_routing_dip_dmac_mis_match_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t   qId = 0;
    rtk_trap_qType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    if ('i' == TOKEN_CHAR(3, 0))
    {
        if ('u' == TOKEN_CHAR(3, 2))
            type = TRAP_Q_L3_IPUC_RPF;
        else if ('m' == TOKEN_CHAR(3, 2))
            type = TRAP_Q_L3_IPMC_RPF;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else if ('l' == TOKEN_CHAR(3, 0))
    {
        if ('2' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_L2_UC_MC_BDG_LUMIS;
        else if ('3' == TOKEN_CHAR(3, 1))
            type = TRAP_Q_IP4_IP6_BDG_LUMIS;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else if ('r' == TOKEN_CHAR(3, 0))
    {
        if ('i' == TOKEN_CHAR(3, 11))
            type = TRAP_Q_ROUTER_MAC_IF;
        else if ('n' == TOKEN_CHAR(3, 11))
            type = TRAP_Q_L3_IPUC_NON_IP_PKT;
        else if ('-' == TOKEN_CHAR(3, 11))
            type = TRAP_Q_L3_ROUTE_DIP_DMAC_MISMATCH;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else if ('e' == TOKEN_CHAR(3, 0))
        type = TRAP_Q_ROUTE_IP_CHK;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_IP6UC_HOP_BY_HOP_IP6MC_ROUTE_HEADER_IP4_OPTION_IPMC_ROUTING_LOOKUP_MISS_IPUC_NULL_ROUTE_IPUC_PBR_NULL_ROUTE_IPUC_HOST_ROUTE_IPUC_NET_ROUTE_QUEUE_ID_QUEUE_ID
/*
 * trap set reason ( ip6uc-hop-by-hop | ip6mc-route-header | ip4-option | ipmc-routing-lookup-miss | ipuc-null-route | ipuc-pbr-null-route | ipuc-host-route | ipuc-net-route ) queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_ip6uc_hop_by_hop_ip6mc_route_header_ip4_option_ipmc_routing_lookup_miss_ipuc_null_route_ipuc_pbr_null_route_ipuc_host_route_ipuc_net_route_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t   qId = 0;
    rtk_trap_qType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    if ('h' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_IP6_UC_MC_HOP_BY_HOP;
    else if ('r' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_IP6_UC_MC_ROUTE_HDR;
    else if ('t' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_IP4_IP_OPT;
    else if ('o' == TOKEN_CHAR(3, 6))
    {
        if ('m' == TOKEN_CHAR(3, 2))
            type = TRAP_Q_IP4_IP6_MC_ROUTE_LUMIS;
        else if ('u' == TOKEN_CHAR(3, 2))
            type = TRAP_Q_L3_UC_HOST_ROUTE;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else if ('u' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_L3_IPUC_NULL_ROUTE;
    else if ('b' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_L3_IPUC_PBR_NULL_ROUTE;
    else if ('e' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_L3_UC_NET_ROUTE;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_IPMC_BDG_ENTRY_IPMC_ROUTE_ENTRY_ROUTE_NH_AGE_OUT_ICMP_REDIR_IPUC_MTU_IPMC_MTU_IPUC_TTL_IPMC_TTL_NORMAL_FWD_QUEUE_ID_QUEUE_ID
/*
 * trap set reason ( ipmc-bdg-entry | ipmc-route-entry | route-nh-age-out | icmp-redir | ipuc-mtu | ipmc-mtu | ipuc-ttl | ipmc-ttl | normal-fwd ) queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_ipmc_bdg_entry_ipmc_route_entry_route_nh_age_out_icmp_redir_ipuc_mtu_ipmc_mtu_ipuc_ttl_ipmc_ttl_normal_fwd_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t   qId = 0;
    rtk_trap_qType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    if ('d' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_L3_MC_BDG_ENTRY;
    else if ('o' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_L3_MC_ROUTE_ENTRY;
    else if ('n' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_ROUTE_EXCPT_NH_AGE_OUT_ACT;
    else if ('e' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_IP4_IP6_ICMP_REDRT;
    else if ('-' == TOKEN_CHAR(3, 6))
        type = TRAP_Q_NORMAL_FWD;
    else if ('t' == TOKEN_CHAR(3, 6))
    {
        if ('u' == TOKEN_CHAR(3, 2))
        {
            if ('m' == TOKEN_CHAR(3, 5))
                type = TRAP_Q_IPUC_MTU;
            else if ('t' == TOKEN_CHAR(3, 5))
                type = TRAP_Q_IPUC_TTL;
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
        }
        else if ('m' == TOKEN_CHAR(3, 2))
        {
            if ('m' == TOKEN_CHAR(3, 5))
                type = TRAP_Q_IPMC_MTU;
            else if ('t' == TOKEN_CHAR(3, 5))
                type = TRAP_Q_IPMC_TTL;
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
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_RLDP_RLPP_QUEUE_ID_QUEUE_ID
/*
 * trap set reason rldp-rlpp queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_rldp_rlpp_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t   qId = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, TRAP_Q_RLDP_RLPP, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_CFM_ETHDM_QUEUE_ID_QUEUE_ID
/*
 * trap set reason ( cfm | ethdm ) queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_cfm_ethdm_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t   qId = 0;
    rtk_trap_qType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    if ('c' == TOKEN_CHAR(3, 0))
    {
        type = TRAP_Q_CFM;
    }
    else if ('e' == TOKEN_CHAR(3, 0))
    {
        type = TRAP_Q_CFM_ETHDM;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_IP6_ND_QUEUE_ID_QUEUE_ID
/*
 * trap set reason ip6-nd queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_ip6_nd_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t	qId = 0;
    rtk_trap_qType_t	type = TRAP_Q_IPV6ND;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_MPLS_EXCEPT_QUEUE_ID_QUEUE_ID
/*
 * trap set reason mpls-except queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_mpls_except_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t	qId = 0;
    rtk_trap_qType_t	type = TRAP_Q_MPLS_EXCPT;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_OF_HIT_OF_LOOKUP_MISS_QUEUE_ID_QUEUE_ID
/*
 * trap set reason ( of-hit | of-lookup-miss ) queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_of_hit_of_lookup_miss_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t	qId = 0;
    rtk_trap_qType_t	type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    if ('h' == TOKEN_CHAR(3, 3))
        type = TRAP_Q_OF_HIT;
    else if ('l' == TOKEN_CHAR(3, 3))
        type = TRAP_Q_OF_TBL_LUMIS;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_SET_REASON_TT_HIT_QUEUE_ID_QUEUE_ID
/*
 * trap set reason tt-hit queue-id <UINT:queue_id>
 */
cparser_result_t cparser_cmd_trap_set_reason_tt_hit_queue_id_queue_id(cparser_context_t *context,
    uint32_t *queue_id_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_qid_t	qId = 0;
    rtk_trap_qType_t	type = TRAP_Q_TT_HIT;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    qId = *queue_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trap_mgmtFrameQueue_set(unit, type, qId), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRAP_DUMP_REASON_QUEUE_ID
/*
 * trap dump reason queue-id
 */
cparser_result_t cparser_cmd_trap_dump_reason_queue_id(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_qid_t   qId = 0;
    rtk_trap_qType_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    for (type = TRAP_Q_ARP_REQ_REP_GRTS; type < TRAP_Q_END; type++)
    {
        ret = rtk_trap_mgmtFrameQueue_get(unit, type, &qId);
        if (ret)
            continue;

        switch (type)
        {
            case TRAP_Q_ARP_REQ_REP_GRTS:
                diag_util_mprintf("ARP Req/Rep Trap Queue ID                : %u\n", qId);
                break;
            case TRAP_Q_DHCP_DHCP6:
                diag_util_mprintf("DHCP Trap Queue ID                       : %u\n", qId);
                break;
            case TRAP_Q_IGMP_MLD:
                diag_util_mprintf("IGMP/MLD Trap Queue ID                   : %u\n", qId);
                break;
            case TRAP_Q_BPDU:
                diag_util_mprintf("BPDU Trap Queue ID                       : %u\n", qId);
                break;
            case TRAP_Q_PTP:
                diag_util_mprintf("PTP Trap Queue ID                        : %u\n", qId);
                break;
            case TRAP_Q_LLDP:
                diag_util_mprintf("LLDP Trap Queue ID                       : %u\n", qId);
                break;
            case TRAP_Q_EAPOL:
                diag_util_mprintf("EAPOL Trap Queue ID                      : %u\n", qId);
                break;
            case TRAP_Q_OAM:
                diag_util_mprintf("OAM Trap Queue ID                        : %u\n", qId);
                break;
            case TRAP_Q_LACP:
                diag_util_mprintf("LACP Trap Queue ID                       : %u\n", qId);
                break;
            case TRAP_Q_RLDP_RLPP:
                diag_util_mprintf("RLDP/RLPP Trap Queue ID                  : %u\n", qId);
                break;
            case TRAP_Q_CFM:
                diag_util_mprintf("CFM Trap Queue ID                        : %u\n", qId);
                break;
            case TRAP_Q_CFM_ETHDM:
                diag_util_mprintf("ETHDM Trap Queue ID                      : %u\n", qId);
                break;
            case TRAP_Q_RMA_USR_DEF:
                diag_util_mprintf("User-defined RMA Trap Queue ID           : %u\n", qId);
                break;
            case TRAP_Q_RMA:
                diag_util_mprintf("RMA Trap Queue ID                        : %u\n", qId);
                break;
            case TRAP_Q_IPV6ND:
                diag_util_mprintf("IP6 Neighbor Discover Trap Queue ID      : %u\n", qId);
                break;
            case TRAP_Q_IP4_IP6_HDR_ERR:
                diag_util_mprintf("IP4/6 Header Error Trap Queue ID         : %u\n", qId);
                break;
            case TRAP_Q_L2_CRC_ERR:
                diag_util_mprintf("L2 CRC Error Trap Queue ID               : %u\n", qId);
                break;
            case TRAP_Q_IP4_CHKSUM_ERR:
                diag_util_mprintf("IP4 Checksum Error Trap Queue ID         : %u\n", qId);
                break;
            case TRAP_Q_IP4_IP6_RSVD_ADDR:
                diag_util_mprintf("IP4/6 Reserved Address Trap Queue ID     : %u\n", qId);
                break;
            case TRAP_Q_IGR_VLAN_FLTR:
                diag_util_mprintf("Ingress VLAN Filter Trap Queue ID        : %u\n", qId);
                break;
            case TRAP_Q_CFI:
                diag_util_mprintf("CFI=1 Trap Queue ID                      : %u\n", qId);
                break;
            case TRAP_Q_IVC:
                diag_util_mprintf("Ingress VLAN Conversion Trap Queue ID    : %u\n", qId);
                break;
            case TRAP_Q_INVALID_SA:
                diag_util_mprintf("Invalid SA Trap Queue ID                 : %u\n", qId);
                break;
            case TRAP_Q_MAC_CST:
                diag_util_mprintf("MAC Constraint Trap Queue ID             : %u\n", qId);
                break;
            case TRAP_Q_NEW_SA:
                diag_util_mprintf("New SA Trap Queue ID                     : %u\n", qId);
                break;
            case TRAP_Q_PMV_FORBID:
                diag_util_mprintf("Port Move Forbid Trap Queue ID           : %u\n", qId);
                break;
            case TRAP_Q_L2_STTC_PMV:
                diag_util_mprintf("Static Port Move Trap Queue ID           : %u\n", qId);
                break;
            case TRAP_Q_L2_DYN_PMV:
                diag_util_mprintf("Dynamic Port Move Trap Queue ID          : %u\n", qId);
                break;
            case TRAP_Q_HASH_FULL:
                diag_util_mprintf("Hash Full Trap Queue ID                  : %u\n", qId);
                break;
            case TRAP_Q_ATK:
                diag_util_mprintf("Attack Trap Queue ID                     : %u\n", qId);
                break;
            case TRAP_Q_ACL_HIT:
                diag_util_mprintf("ACL Hit Trap Queue ID                    : %u\n", qId);
                break;
            case TRAP_Q_MIR_HIT:
                diag_util_mprintf("Mirror Hit Trap Queue ID                 : %u\n", qId);
                break;
            case TRAP_Q_CAPWAP:
                //diag_util_mprintf("CAPWAP Packet Trap Queue ID              : %u\n", qId);
                break;
            case TRAP_Q_MPLS_EXCPT:
                diag_util_mprintf("MPLS Exception Trap Queue ID             : %u\n", qId);
                break;
            case TRAP_Q_OF_HIT:
                diag_util_mprintf("OpenFlow Hit Trap Queue ID               : %u\n", qId);
                break;
            case TRAP_Q_OF_TBL_LUMIS:
                diag_util_mprintf("OpenFlow Table Lookup Miss Trap Queue ID : %u\n", qId);
                break;
            case TRAP_Q_TT_HIT:
                diag_util_mprintf("Tunnel Terminate Hit Trap Queue ID       : %u\n", qId);
                break;
            case TRAP_Q_SFLOW:
                diag_util_mprintf("SFlow Trap Queue ID                      : %u\n", qId);
                break;
            case TRAP_Q_PASR_EXCPT:
                diag_util_mprintf("Parser Exception Trap Queue ID           : %u\n", qId);
                break;
            case TRAP_Q_MALFORM_PKT:
                diag_util_mprintf("Malformed Packet Trap Queue ID           : %u\n", qId);
                break;
            case TRAP_Q_CPU2CPU_TALK:
                diag_util_mprintf("CPU to CPU talk Queue ID                 : %u\n", qId);
                break;
            case TRAP_Q_L3_IPUC_RPF:
                diag_util_mprintf("IP Unicast RPF Fail Trap Queue ID        : %u\n", qId);
                break;
            case TRAP_Q_L3_IPMC_RPF:
                diag_util_mprintf("IP Multicast RPF Fail Trap Queue ID      : %u\n", qId);
                break;
            case TRAP_Q_L2_UC_MC_BDG_LUMIS:
                diag_util_mprintf("Unknown DA Trap Queue ID                 : %u\n", qId);
                break;
            case TRAP_Q_IP4_IP6_BDG_LUMIS:
                diag_util_mprintf("L3 Multicast Lookup-miss Trap Queue ID   : %u\n", qId);
                break;
            case TRAP_Q_ROUTER_MAC_IF:
                diag_util_mprintf("Router MAC Interface Trap Queue ID       : %u\n", qId);
                break;
            case TRAP_Q_L3_IPUC_NON_IP_PKT:
                diag_util_mprintf("Router MAC Non-IP Packet Trap Queue ID   : %u\n", qId);
                break;
            case TRAP_Q_ROUTE_IP_CHK:
                diag_util_mprintf("Routing IP Addr Check Fail Trap Queue ID : %u\n", qId);
                break;
            case TRAP_Q_L3_ROUTE_DIP_DMAC_MISMATCH:
                diag_util_mprintf("L3 Routing DIP/DMAC Mismatch Trap Queue ID : %u\n", qId);
                break;
            case TRAP_Q_IP6_UC_MC_HOP_BY_HOP:
                diag_util_mprintf("IP6 UC/MC Hop by Hop Option Trap Queue ID: %u\n", qId);
                break;
            case TRAP_Q_IP6_UC_MC_ROUTE_HDR:
                diag_util_mprintf("IP6 UC/MC Routing Header Trap Queue ID   : %u\n", qId);
                break;
            case TRAP_Q_IP4_IP_OPT:
                diag_util_mprintf("IP4 IP Option Trap Queue ID              : %u\n", qId);
                break;
            case TRAP_Q_IP4_IP6_MC_ROUTE_LUMIS:
                diag_util_mprintf("IP4/6 MC Routing Lookup Miss Trap Queue ID : %u\n", qId);
                break;
            case TRAP_Q_L3_IPUC_NULL_ROUTE:
                diag_util_mprintf("IP Unicast Null Route Trap Queue ID      : %u\n", qId);
                break;
            case TRAP_Q_L3_IPUC_PBR_NULL_ROUTE:
                diag_util_mprintf("IP Unicast PBR Null Route Trap Queue ID  : %u\n", qId);
                break;
            case TRAP_Q_L3_UC_HOST_ROUTE:
                diag_util_mprintf("IP Unicast Host Route Trap Queue ID      : %u\n", qId);
                break;
            case TRAP_Q_L3_UC_NET_ROUTE:
                diag_util_mprintf("IP Unicast Net Route Trap Queue ID       : %u\n", qId);
                break;
            case TRAP_Q_L3_MC_BDG_ENTRY:
                diag_util_mprintf("L3 MC Bridge Entry Trap Queue ID         : %u\n", qId);
                break;
            case TRAP_Q_L3_MC_ROUTE_ENTRY:
                diag_util_mprintf("L3 MC Route Entry Trap Queue ID          : %u\n", qId);
                break;
            case TRAP_Q_TTL_EXCPT:
                diag_util_mprintf("TTL Exception Trap Queue ID              : %u\n", qId);
                break;
            case TRAP_Q_ROUTE_EXCPT_NH_AGE_OUT_ACT:
                diag_util_mprintf("Next-Hop Entry Age-out Trap Queue ID     : %u\n", qId);
                break;
            case TRAP_Q_IP4_IP6_ICMP_REDRT:
                diag_util_mprintf("IP4/6 ICMP Redirect Trap Queue ID        : %u\n", qId);
                break;
            case TRAP_Q_IPUC_MTU:
                diag_util_mprintf("IP Unicast MTU Trap Queue ID             : %u\n", qId);
                break;
            case TRAP_Q_IPMC_MTU:
                diag_util_mprintf("IP Multicast MTU Trap Queue ID           : %u\n", qId);
                break;
            case TRAP_Q_IPUC_TTL:
                diag_util_mprintf("IP Unicast TTL Trap Queue ID             : %u\n", qId);
                break;
            case TRAP_Q_IPMC_TTL:
                diag_util_mprintf("IP Multicast TTL Trap Queue ID           : %u\n", qId);
                break;
            case TRAP_Q_IP_MAC_BINDING:
                diag_util_mprintf("IP MAC Binding Trap Queue ID             : %u\n", qId);
                break;
            case TRAP_Q_TUNL_MAC_IF:
                diag_util_mprintf("Tunnel MAC Interface Trap Queue ID       : %u\n", qId);
                break;
            case TRAP_Q_TUNL_IP_CHK:
                diag_util_mprintf("Tunnel IP Check Trap Queue ID            : %u\n", qId);
                break;
            case TRAP_Q_ROUTE_EXCPT_NH_ERR_ACT:
                diag_util_mprintf("NOT a Next-Hop Entry Trap Queue ID       : %u\n", qId);
                break;
            case TRAP_Q_ROUTE_EXCPT_ROUTE_TO_TUNL_ACT:
                diag_util_mprintf("NH is a tunnel interface while Port Routing Trap Queue ID : %u\n", qId);
                break;
            case TRAP_Q_NORMAL_FWD:
                diag_util_mprintf("Normal Forward Trap Queue ID             : %u\n", qId);
                break;

            default:
                diag_util_printf("User config: index Error!\n");
                return CPARSER_NOT_OK;
                break;
        }
    }


    return CPARSER_OK;
}
#endif

