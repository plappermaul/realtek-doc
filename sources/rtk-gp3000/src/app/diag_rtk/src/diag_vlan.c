/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>


#include <rtk/vlan.h>


#ifdef CONFIG_SDK_APOLLOMP
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/raw/apollomp_raw_vlan.h>
#endif

/*
 * vlan init
 */
cparser_result_t
cparser_cmd_vlan_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_init */

/*
 * vlan create vlan-table vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_vlan_create_vlan_table_vid_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_t vid;

    DIAG_UTIL_PARAM_CHK();

    vid = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_create(vid), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_create_vlan_table_vid_vid */

/*
 * vlan destroy vlan-table all { restore-default-vlan }
 */
cparser_result_t
cparser_cmd_vlan_destroy_vlan_table_all_restore_default_vlan(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(4 == TOKEN_NUM())
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_destroyAll(1), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_destroyAll(0), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_destroy_vlan_table_all_restore_default_vlan */

/*
 * vlan destroy vlan-table vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_vlan_destroy_vlan_table_vid_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_destroy(*vid_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_destroy_vlan_table_vid_vid */


/*
 * vlan destroy vlan-table all untag
 */
cparser_result_t
cparser_cmd_vlan_destroy_vlan_table_all_untag(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 vid;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("0-6", DIAG_PORTTYPE_ALL, &portlist),ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlan4kentry_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlan4kentry_t));
            vlanCfg.untag.bits[0] = portlist.portmask.bits[0];

            for(vid = 0;vid <= RTK_VLAN_ID_MAX ;vid++)
            {
                vlanCfg.vid = vid;
                DIAG_UTIL_ERR_CHK(apollo_raw_vlan_4kEntry_set(&vlanCfg), ret);
            }
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP

        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlan4kentry_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlan4kentry_t));
            vlanCfg.untag.bits[0] = portlist.portmask.bits[0];

            for(vid = 0;vid <= RTK_VLAN_ID_MAX ;vid++)
            {
                vlanCfg.vid = vid;
                DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_4kEntry_set(&vlanCfg), ret);
            }
        }
        break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_destroy_vlan_table_all_untag */

/*
 * vlan destroy entry all
 */
cparser_result_t
cparser_cmd_vlan_destroy_entry_all(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 idx;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;
            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            for(idx = 0;idx <HAL_MAX_NUM_OF_VLAN_MBRCFG() ;idx++)
            {
                vlanCfg.index = idx;
                if((ret = apollo_raw_vlan_memberConfig_set(&vlanCfg)) != RT_ERR_OK)
                {
                    DIAG_ERR_PRINT(ret);
                    return CPARSER_NOT_OK;
                }
            }
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;
            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            for(idx = 0;idx <HAL_MAX_NUM_OF_VLAN_MBRCFG() ;idx++)
            {
                vlanCfg.index = idx;
                if((ret = apollomp_raw_vlan_memberConfig_set(&vlanCfg)) != RT_ERR_OK)
                {
                    DIAG_ERR_PRINT(ret);
                    return CPARSER_NOT_OK;
                }
            }
        }
        break;
#endif

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_destroy_entry_all */

/*
 * vlan destroy entry <UINT:index>
 */
cparser_result_t
cparser_cmd_vlan_destroy_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    //apollo_raw_vlanconfig_t vlanCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;
            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK((apollo_raw_vlan_memberConfig_set(&vlanCfg)), ret);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;
            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK((apollomp_raw_vlan_memberConfig_set(&vlanCfg)), ret);
        }
        break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_destroy_entry_index */

/*
 * vlan get state
 */
cparser_result_t
cparser_cmd_vlan_get_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK((rtk_vlan_vlanFunctionEnable_get(&enable)), ret);

    diag_util_mprintf("vlan state: %s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_state */

/*
 * vlan set state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_state_enable_disable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(3,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK((rtk_vlan_vlanFunctionEnable_set(enable)), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_state_enable_disable */

/*
 * vlan get vlan-treat vid ( 0 | 4095 ) type
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_treat_vid_0_4095_type(
    cparser_context_t *context)
{
    rtk_vlan_resVidAction_t actionVid0;
    rtk_vlan_resVidAction_t actionVid4095;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK((rtk_vlan_reservedVidAction_get(&actionVid0,&actionVid4095)), ret);

    if('0'==TOKEN_CHAR(4,0))
        diag_util_mprintf("vlan 0 treat %s\n",diagStr_vlanTagType[actionVid0]);
    else
        diag_util_mprintf("vlan 4095 treat %s\n",diagStr_vlanTagType[actionVid4095]);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_vlan_treat_vid_0_4095_type */


/*
 * vlan set vlan-treat vid ( 0 | 4095 ) type ( tagging | un-tagging )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_treat_vid_0_4095_type_tagging_un_tagging(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_resVidAction_t actionVid0;
    rtk_vlan_resVidAction_t actionVid4095;
    rtk_vlan_resVidAction_t setAct;

    DIAG_UTIL_PARAM_CHK();

    if('u'==TOKEN_CHAR(6,0))
        setAct = RESVID_ACTION_UNTAG;
    else
        setAct = RESVID_ACTION_TAG;

    DIAG_UTIL_ERR_CHK((rtk_vlan_reservedVidAction_get(&actionVid0,&actionVid4095)), ret);

    if('0'==TOKEN_CHAR(4,0))
        actionVid0 = setAct;
    else
        actionVid4095 = setAct;

    DIAG_UTIL_ERR_CHK((rtk_vlan_reservedVidAction_set(actionVid0,actionVid4095)), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_treat_vid_0_4095_type_tagging_un_tagging */

/*
 * vlan get accept-frame-type port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_accept_frame_type_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_acceptFrameType_t  acceptFrameType;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port Accept Frame Type\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        diag_util_mprintf("%-4d ",port);
        DIAG_UTIL_ERR_CHK(rtk_vlan_portAcceptFrameType_get(port, &acceptFrameType), ret);

        switch(acceptFrameType)
        {
            case ACCEPT_FRAME_TYPE_ALL:
                diag_util_mprintf("%s\n","all");
                break;
            case ACCEPT_FRAME_TYPE_TAG_ONLY:
                diag_util_mprintf("%s\n","tagged-only");
                break;
            case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
                diag_util_mprintf("%s\n","untagged-only");
                break;
            case ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY:
                diag_util_mprintf("%s\n","tagged-1p-1q-only");
                break;
            default:
                diag_util_mprintf("%s\n",DIAG_STR_INVALID);
                break;
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_accept_frame_type_port_ports_all */

/*
 * vlan set accept-frame-type port ( <PORT_LIST:ports> | all ) ( all | tag-only | untag-only | priority-tag-and-tag )
 */
cparser_result_t
cparser_cmd_vlan_set_accept_frame_type_port_ports_all_all_tag_only_untag_only_priority_tag_and_tag(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_acceptFrameType_t  type;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    switch(TOKEN_CHAR(5,0))
    {
        case 'a':
            type = ACCEPT_FRAME_TYPE_ALL;
            break;
        case 'u':
            type = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
            break;
        case 't':
            type = ACCEPT_FRAME_TYPE_TAG_ONLY;
            break;
        case 'p':
            type = ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY;
            break;
        default:
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
    	DIAG_UTIL_ERR_CHK(rtk_vlan_portAcceptFrameType_set(port, type), ret);
   	}
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_accept_frame_type_port_ports_all_all_tag_only_untag_only_priority_tag_and_tag */

/*
 * vlan get ingress-filter port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_vlan_get_ingress_filter_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port Ingress-filter\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        /*call API*/
        DIAG_UTIL_ERR_CHK(rtk_vlan_portIgrFilterEnable_get(port, &enable), ret);
        diag_util_mprintf("%-4d %s\n",port,diagStr_enable[enable]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_ingress_filter_port_ports_all_state */

/*
 * vlan set ingress-filter port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_ingress_filter_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6,0))
        enable = ENABLED;
    else
        enable = DISABLED;


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        /*call API*/
        DIAG_UTIL_ERR_CHK(rtk_vlan_portIgrFilterEnable_set(port, enable), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_ingress_filter_port_ports_all_state_enable_disable */

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * vlan get egress port ( <PORT_LIST:egr_ports> | all ) keep-tag ingress-port ( <PORT_LIST:igr_ports> | all ) state
 */
cparser_result_t
cparser_cmd_vlan_get_egress_port_egr_ports_all_keep_tag_ingress_port_igr_ports_all_state(
    cparser_context_t *context,
    char * *egr_ports_ptr,
    char * *igr_ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    rtk_port_t igrPort;
    diag_portlist_t igrPortlist;
    diag_portlist_t egrPortlist;
    uint32 portMbr;
    diag_portlist_t asicPortMbr;
    rtk_portmask_t pmsk;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(egrPortlist, 4), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(igrPortlist, 7), ret);
	diag_util_mprintf("Port P0      P1      P2      P3      P4      P5       P6\n");
    DIAG_UTIL_PORTMASK_SCAN(egrPortlist, port)
    {
        /*call API*/
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_vlan_egrKeepPmsk_get(port, &(asicPortMbr.portmask)), ret);
                pmsk.bits[0] = asicPortMbr.portmask.bits[0];
                
                diag_util_printf("%-4d ",port);
                for ((igrPort) = igrPortlist.min; (igrPort) <= igrPortlist.max; (igrPort)++)
                {
                    if (RTK_PORTMASK_IS_PORT_SET((igrPortlist).portmask, (igrPort)))
                    {
                        if(RTK_PORTMASK_IS_PORT_SET(pmsk, igrPort))
                            diag_util_printf("%-7s ",DIAG_STR_ENABLE);
                        else
                            diag_util_printf("%-7s ",DIAG_STR_DISABLE);
                
                    }
                    else
                        diag_util_printf("        ");
                }
                diag_util_mprintf("\n");
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_egrKeepPmsk_get(port, &(asicPortMbr.portmask)), ret);
                pmsk.bits[0] = asicPortMbr.portmask.bits[0];
                
                diag_util_printf("%-4d ",port);
                for ((igrPort) = igrPortlist.min; (igrPort) <= igrPortlist.max; (igrPort)++)
                {
                    if (RTK_PORTMASK_IS_PORT_SET((igrPortlist).portmask, (igrPort)))
                    {
                        if(RTK_PORTMASK_IS_PORT_SET(pmsk, igrPort))
                            diag_util_printf("%-7s ",DIAG_STR_ENABLE);
                        else
                            diag_util_printf("%-7s ",DIAG_STR_DISABLE);
                
                    }
                    else
                        diag_util_printf("        ");
                }
                diag_util_mprintf("\n");
            break;
#endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
            break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_egress_port_egr_ports_all_keep_tag_ingress_port_igr_ports_all_state */

/*
 * vlan set egress port ( <PORT_LIST:egr_ports> | all ) keep-tag ingress-port ( <PORT_LIST:igr_ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_egress_port_egr_ports_all_keep_tag_ingress_port_igr_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *egr_ports_ptr,
    char * *igr_ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    rtk_port_t port;
    diag_portlist_t setPortlist;
    diag_portlist_t egrPortlist;
    uint32 portMbr;
    diag_portlist_t asicPortMbr;

    DIAG_UTIL_PARAM_CHK();

    if('e'==(TOKEN_CHAR(9,0)))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(setPortlist, 4), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(egrPortlist, 7), ret);
    portMbr = egrPortlist.portmask.bits[0];

    DIAG_UTIL_PORTMASK_SCAN(setPortlist, port)
    {
        /*call API*/
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_vlan_egrKeepPmsk_get(port, &(asicPortMbr.portmask)), ret);
                if(enable == ENABLED)
                    asicPortMbr.portmask.bits[0] = (asicPortMbr.portmask.bits[0] | portMbr);
                else
                    asicPortMbr.portmask.bits[0] = (asicPortMbr.portmask.bits[0] & (~portMbr));
                DIAG_UTIL_ERR_CHK(apollo_raw_vlan_egrKeepPmsk_set(port, asicPortMbr.portmask), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_egrKeepPmsk_get(port, &(asicPortMbr.portmask)), ret);
                if(enable == ENABLED)
                    asicPortMbr.portmask.bits[0] = (asicPortMbr.portmask.bits[0] | portMbr);
                else
                    asicPortMbr.portmask.bits[0] = (asicPortMbr.portmask.bits[0] & (~portMbr));
                DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_egrKeepPmsk_set(port, asicPortMbr.portmask), ret);
            break;
#endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
            break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_egress_port_egr_ports_all_keep_tag_ingress_port_igr_ports_all_state_enable_disable */

/*
 * vlan get transparent state
 */
cparser_result_t
cparser_cmd_vlan_get_transparent_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK((rtk_vlan_transparentEnable_get(&enable)), ret);

    diag_util_mprintf("vlan transparent state: %s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_transparent_state */

/*
 * vlan set transparent state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_transparent_state_enable_disable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK((rtk_vlan_transparentEnable_set(enable)), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_transparent_state_enable_disable */


/*
 * vlan get entry <UINT:index>
 */
cparser_result_t
cparser_cmd_vlan_get_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();



    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_get(&vlanCfg), ret);

            diag_util_mprintf("vlan id:%d\n",vlanCfg.index);
            diag_util_mprintf("%4s %5s %6s %5s %3s %3s\n",
                                "Evid",
                                "PlyEn",
                                "MtrIdx",
                                "PriEn",
                                "pri",
                                "Fid");

            diag_util_mprintf("%4d %5s %6d %5s %3d %3d\n",
                                vlanCfg.evid,
                                diagStr_enDisplay[vlanCfg.envlanpol],
                                vlanCfg.meteridx,
                                diagStr_enDisplay[vlanCfg.vbpen],
                                vlanCfg.vbpri,
                                vlanCfg.fid_msti);

            diag_util_lPortMask2str(buf,&vlanCfg.mbr);
            diag_util_mprintf("member port          : %s\n",buf);
            diag_util_lPortMask2str(buf,&vlanCfg.exMbr);
            diag_util_mprintf("extention member port: %s\n",buf);
            diag_util_lPortMask2str(buf,&vlanCfg.dslMbr);
            diag_util_mprintf("dsl member port      : %s\n\n",buf);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_get(&vlanCfg), ret);

            diag_util_mprintf("vlan id:%d\n",vlanCfg.index);
            diag_util_mprintf("%4s %5s %6s %5s %3s %3s\n",
                                "Evid",
                                "PlyEn",
                                "MtrIdx",
                                "PriEn",
                                "pri",
                                "Fid");

            diag_util_mprintf("%4d %5s %6d %5s %3d %3d\n",
                                vlanCfg.evid,
                                diagStr_enDisplay[vlanCfg.envlanpol],
                                vlanCfg.meteridx,
                                diagStr_enDisplay[vlanCfg.vbpen],
                                vlanCfg.vbpri,
                                vlanCfg.fid_msti);

            diag_util_lPortMask2str(buf,&vlanCfg.mbr);
            diag_util_mprintf("member port          : %s\n",buf);
            diag_util_lPortMask2str(buf,&vlanCfg.exMbr);
            diag_util_mprintf("extention member port: %s\n",buf);
        }
        break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_entry_index */

/*
 * vlan get entry all
 */
cparser_result_t
cparser_cmd_vlan_get_entry_all(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];
    int32 index;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        for(index = 0;index <= HAL_MAX_VLAN_MBRCFG_IDX();index++)
        {
            apollo_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = index;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_get(&vlanCfg), ret);

            diag_util_mprintf("vlan id:%d\n",vlanCfg.index);
            diag_util_mprintf("%4s %5s %6s %5s %3s %3s\n",
                                "Evid",
                                "PlyEn",
                                "MtrIdx",
                                "PriEn",
                                "pri",
                                "Fid");

            diag_util_mprintf("%4d %5s %6d %5s %3d %3d\n",
                                vlanCfg.evid,
                                diagStr_enDisplay[vlanCfg.envlanpol],
                                vlanCfg.meteridx,
                                diagStr_enDisplay[vlanCfg.vbpen],
                                vlanCfg.vbpri,
                                vlanCfg.fid_msti);

            diag_util_lPortMask2str(buf,&vlanCfg.mbr);
            diag_util_mprintf("member port          : %s\n",buf);
            diag_util_lPortMask2str(buf,&vlanCfg.exMbr);
            diag_util_mprintf("extention member port: %s\n",buf);
            diag_util_lPortMask2str(buf,&vlanCfg.dslMbr);
            diag_util_mprintf("dsl member port      : %s\n\n",buf);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        for(index = 0;index <= HAL_MAX_VLAN_MBRCFG_IDX();index++)
        {
            apollomp_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = index;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_get(&vlanCfg), ret);

            diag_util_mprintf("vlan id:%d\n",vlanCfg.index);
            diag_util_mprintf("%4s %5s %6s %5s %3s %3s\n",
                                "Evid",
                                "PlyEn",
                                "MtrIdx",
                                "PriEn",
                                "pri",
                                "Fid");

            diag_util_mprintf("%4d %5s %6d %5s %3d %3d\n",
                                vlanCfg.evid,
                                diagStr_enDisplay[vlanCfg.envlanpol],
                                vlanCfg.meteridx,
                                diagStr_enDisplay[vlanCfg.vbpen],
                                vlanCfg.vbpri,
                                vlanCfg.fid_msti);

            diag_util_lPortMask2str(buf,&vlanCfg.mbr);
            diag_util_mprintf("member port          : %s\n",buf);
            diag_util_lPortMask2str(buf,&vlanCfg.exMbr);
            diag_util_mprintf("extention member port: %s\n",buf);
        }
        break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_entry_all */

/*
 * vlan set entry <UINT:index> enhanced-vid <UINT:evid>
 */
cparser_result_t
cparser_cmd_vlan_set_entry_index_enhanced_vid_evid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *evid_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.evid = *evid_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.evid = *evid_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_entry_index_enhanced_vid_evid */

/*
 * vlan set entry <UINT:index> fid-msti <UINT:fid_msti>
 */
cparser_result_t
cparser_cmd_vlan_set_entry_index_fid_msti_fid_msti(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *fid_msti_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.fid_msti = *fid_msti_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.fid_msti = *fid_msti_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_entry_index_fid_msti_fid_msti */

/*
 * vlan set entry <UINT:index> member ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_vlan_set_entry_index_member_ports_all_none(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_get(&vlanCfg), ret);
            RTK_PORTMASK_ASSIGN(vlanCfg.mbr,portlist.portmask);
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP

        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_get(&vlanCfg), ret);
            RTK_PORTMASK_ASSIGN(vlanCfg.mbr,portlist.portmask);
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_entry_index_member_ports_all_none */

/*
 * vlan set entry <UINT:index> ext-member ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_vlan_set_entry_index_ext_member_ext_all_none(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ext_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_get(&vlanCfg), ret);
            RTK_PORTMASK_ASSIGN(vlanCfg.exMbr,portlist.portmask);
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_get(&vlanCfg), ret);
            RTK_PORTMASK_ASSIGN(vlanCfg.exMbr,portlist.portmask);
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_entry_index_ext_member_ext_all_none */

/*
 * vlan set entry <UINT:index> vlan-based-policing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_entry_index_vlan_based_policing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(6,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.envlanpol = enable;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP

        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.envlanpol = enable;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_entry_index_vlan_based_policing_state_disable_enable */

/*
 * vlan set entry <UINT:index> meter <UINT:meter>
 */
cparser_result_t
cparser_cmd_vlan_set_entry_index_meter_meter(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *meter_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.meteridx = *meter_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.meteridx = *meter_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_entry_index_meter_meter */

/*
 * vlan set entry <UINT:index> vlan-based-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_entry_index_vlan_based_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(6,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.vbpen = enable;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP

        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.vbpen = enable;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_entry_index_vlan_based_priority_state_disable_enable */

/*
 * vlan set entry <UINT:index> vlan-based-priority priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_vlan_set_entry_index_vlan_based_priority_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            apollo_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollo_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.vbpri = *priority_ptr;
            DIAG_UTIL_ERR_CHK(apollo_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif
#ifdef CONFIG_SDK_APOLLOMP

        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_vlanconfig_t vlanCfg;

            memset(&vlanCfg,0x0,sizeof(apollomp_raw_vlanconfig_t));
            vlanCfg.index = *index_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_get(&vlanCfg), ret);
            vlanCfg.vbpri = *priority_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_memberConfig_set(&vlanCfg), ret);
        }
        break;
#endif

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_entry_index_vlan_based_priority_priority_priority */
#endif
/*
 * vlan get protocol-vlan group <UINT:index>
 */
cparser_result_t
cparser_cmd_vlan_get_protocol_vlan_group_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_protoGroup_t  cfg;

    memset(&cfg,0x0,sizeof(rtk_vlan_protoGroup_t));


    DIAG_UTIL_ERR_CHK(rtk_vlan_protoGroup_get(*index_ptr,&cfg), ret);

    diag_util_mprintf("Group  FRAME-TYPE  ETHER-TYPE\n");
    switch(cfg.frametype)
    {
        case FRAME_TYPE_ETHERNET:
            diag_util_mprintf("%-5d  %-10s  0x%-4.4x\n",*index_ptr,DIAG_STR_ETHERNET,cfg.framevalue);
            break;
        case FRAME_TYPE_RFC1042:
            diag_util_mprintf("%-5d  %-10s  0x%-4.4x\n",*index_ptr,DIAG_STR_SNAP,cfg.framevalue);
            break;
        case FRAME_TYPE_LLCOTHER:
            diag_util_mprintf("%-5d  %-10s  0x%-4.4x\n",*index_ptr,DIAG_STR_LLC_OTHER,cfg.framevalue);
            break;
        default:
            break;

    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_protocol_vlan_group_index */

/*
 * vlan set protocol-vlan group <UINT:index> frame-type ( ethernet | snap | llc-other ) <UINT:frame_type>
 */
cparser_result_t
cparser_cmd_vlan_set_protocol_vlan_group_index_frame_type_ethernet_snap_llc_other_frame_type(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *frame_type_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_protoGroup_t  cfg;

    memset(&cfg,0x0,sizeof(rtk_vlan_protoGroup_t));

    switch(TOKEN_CHAR(6,0))
    {
        case 'e':
            cfg.frametype = FRAME_TYPE_ETHERNET;
            break;

        case 's':
            cfg.frametype = FRAME_TYPE_RFC1042;
            break;

        case 'l':
            cfg.frametype = FRAME_TYPE_LLCOTHER;
            break;

        default:
            return CPARSER_NOT_OK;
    }

    cfg.framevalue = *frame_type_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_protoGroup_set(*index_ptr,&cfg), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_protocol_vlan_group_index_frame_type_ethernet_snap_llc_other_frame_type */

/*
 * vlan get protocol-vlan port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_protocol_vlan_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    int32 protoId;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_vlan_protoVlanCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port  ProtoIdx    Valid  Vlan  Pri\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        memset(&cfg,0x0,sizeof(rtk_vlan_protoVlanCfg_t));

        for(protoId = 0;protoId <= HAL_PROTOCOL_VLAN_IDX_MAX();protoId++)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portProtoVlan_get(port,protoId,&cfg), ret);
            diag_util_mprintf("%-4d  %-8d  %-7s  %-6d  %-3d\n",port,protoId,diagStr_valid[cfg.valid],cfg.vid,cfg.pri);

        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_protocol_vlan_port_ports_all */

/*
 * vlan set protocol-vlan port ( <PORT_LIST:ports> | all ) group <UINT:index> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_protocol_vlan_port_ports_all_group_index_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    int32 protoId;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_vlan_protoVlanCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        memset(&cfg,0x0,sizeof(rtk_vlan_protoVlanCfg_t));

        DIAG_UTIL_ERR_CHK(rtk_vlan_portProtoVlan_get(port,*index_ptr,&cfg), ret);

        if('e' == TOKEN_CHAR(8,0))
            cfg.valid = 1;
        else
            cfg.valid = 0;

        DIAG_UTIL_ERR_CHK(rtk_vlan_portProtoVlan_set(port,*index_ptr,&cfg), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_protocol_vlan_port_ports_all_group_index_state_enable_disable */

/*
 * vlan set protocol-vlan port ( <PORT_LIST:ports> | all ) group <UINT:index> vid <UINT:vid> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_vlan_set_protocol_vlan_port_ports_all_group_index_vid_vid_priority_priority(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_vlan_protoVlanCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        memset(&cfg,0x0,sizeof(rtk_vlan_protoVlanCfg_t));

        DIAG_UTIL_ERR_CHK(rtk_vlan_portProtoVlan_get(port,*index_ptr,&cfg), ret);

        cfg.vid = *vid_ptr;
        cfg.pri = *priority_ptr;
        DIAG_UTIL_ERR_CHK(rtk_vlan_portProtoVlan_set(port,*index_ptr,&cfg), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_protocol_vlan_port_ports_all_group_index_vid_vid_priority_priority */
#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * vlan set protocol-vlan ext <PORT_LIST:ext> group <UINT:index> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_vlan_set_protocol_vlan_ext_ext_group_index_vid_vid(
    cparser_context_t *context,
    char * *ext_ptr,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_vlan_protoVlanCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        memset(&cfg,0x0,sizeof(rtk_vlan_protoVlanCfg_t));

        DIAG_UTIL_ERR_CHK(rtk_vlan_extPortProtoVlan_get(port,*index_ptr,&cfg), ret);

        cfg.vid = *vid_ptr;
        DIAG_UTIL_ERR_CHK(rtk_vlan_extPortProtoVlan_set(port,*index_ptr,&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_protocol_vlan_ext_ext_group_index_vid_vid */

/*
 * vlan set protocol-vlan ext <PORT_LIST:ext> group <UINT:index> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_protocol_vlan_ext_ext_group_index_state_enable_disable(
    cparser_context_t *context,
    char * *ext_ptr,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_vlan_protoVlanCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        memset(&cfg,0x0,sizeof(rtk_vlan_protoVlanCfg_t));

        DIAG_UTIL_ERR_CHK(rtk_vlan_extPortProtoVlan_get(port,*index_ptr,&cfg), ret);

        if('e' == TOKEN_CHAR(8,0))
            cfg.valid = 1;
        else
            cfg.valid = 0;

        DIAG_UTIL_ERR_CHK(rtk_vlan_extPortProtoVlan_set(port,*index_ptr,&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_protocol_vlan_ext_ext_group_index_state_enable_disable */


/*
 * vlan get protocol-vlan ext ( <PORT_LIST:ext> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_protocol_vlan_ext_ext_all(
    cparser_context_t *context,
    char * *ext_ptr)
{
    int32 ret = RT_ERR_FAILED;
    int32 protoId;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_vlan_protoVlanCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 4), ret);

    diag_util_mprintf("Port ProtoIdx Vlan\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        memset(&cfg, 0x0, sizeof(rtk_vlan_protoVlanCfg_t));

        for(protoId = 0;protoId <= HAL_PROTOCOL_VLAN_IDX_MAX();protoId++)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_extPortProtoVlan_get(port,protoId,&cfg), ret);
            diag_util_mprintf("%-4d %-8d %-3d\n",port,protoId,cfg.vid);
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_protocol_vlan_ext_ext */

/*
 * vlan get pvid port ( <PORT_LIST:ports> | all ) vlan-index
 */
cparser_result_t
cparser_cmd_vlan_get_pvid_port_ports_all_vlan_index(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 vidx;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    diag_util_mprintf("Port  Vidx\n");
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_vlan_portToMbrCfgIdx_get(port,&vidx), ret);
                diag_util_mprintf("%-4d  %-4d\n",port,vidx);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_portToMbrCfgIdx_get(port,&vidx), ret);
                diag_util_mprintf("%-4d  %-4d\n",port,vidx);
            break;
#endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
            break;
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_pvid_port_ports_all_vlan_index */

/*
 * vlan set pvid port ( <PORT_LIST:ports> | all ) vlan-index <UINT:index>
 */
cparser_result_t
cparser_cmd_vlan_set_pvid_port_ports_all_vlan_index_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_vlan_portToMbrCfgIdx_set(port - 1,*index_ptr), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP

            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_portToMbrCfgIdx_set(port - 1,*index_ptr), ret);
            break;
#endif

            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
            break;
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_pvid_port_ports_all_vlan_index_index */

/*
 * vlan get ext-pvid port <PORT_LIST:ports> vlan-index
 */
cparser_result_t
cparser_cmd_vlan_get_ext_pvid_port_ports_vlan_index(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 vidx;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    diag_util_mprintf("Port  Vidx\n");
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                if(HAL_GET_EXT_CPU_PORT() == port)
                {
                    DIAG_UTIL_ERR_CHK(apollo_raw_vlan_portToMbrCfgIdx_get(HAL_GET_CPU_PORT(),&vidx), ret);
                }
                else
                {
                    DIAG_UTIL_ERR_CHK(apollo_raw_vlan_extPortToMbrCfgIdx_get(port - 1,&vidx), ret);
                }
                diag_util_mprintf("%-4d  %-4d\n",port,vidx);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                if(HAL_GET_EXT_CPU_PORT() == port)
                {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_portToMbrCfgIdx_get(HAL_GET_CPU_PORT(),&vidx), ret);
                }
                else
                {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_extPortToMbrCfgIdx_get(port - 1,&vidx), ret);
                }
                diag_util_mprintf("%-4d  %-4d\n",port,vidx);
            break;
#endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
            break;
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_ext_pvid_port_ports_vlan_index */

/*
 * vlan get ext-pvid port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_ext_pvid_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_vlan_t pvid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_extPortPvid_get(port, &pvid), ret);
        diag_util_printf("\n EXT Port %d PVID: %d", port, pvid);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_ext_pvid_port_ports_all */

/*
 * vlan set ext-pvid port ( <PORT_LIST:ports> | all ) <UINT:vid>
 */
cparser_result_t
cparser_cmd_vlan_set_ext_pvid_port_ports_all_vid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_extPortPvid_set(port, (rtk_vlan_t)*vid_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_ext_pvid_port_ports_all_vid */

/*
 * vlan set ext-pvid port <PORT_LIST:ports> vlan-index <UINT:index>
 */
cparser_result_t
cparser_cmd_vlan_set_ext_pvid_port_ports_vlan_index_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 vidx;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                if(HAL_GET_EXT_CPU_PORT() == port)
                {
                    DIAG_UTIL_ERR_CHK(apollo_raw_vlan_portToMbrCfgIdx_set(HAL_GET_CPU_PORT(),*index_ptr), ret);
                }
                else
                {
                    DIAG_UTIL_ERR_CHK(apollo_raw_vlan_extPortToMbrCfgIdx_set(port - 1,*index_ptr), ret);
                }
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP

            case APOLLOMP_CHIP_ID:
                if(HAL_GET_EXT_CPU_PORT() == port)
                {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_portToMbrCfgIdx_set(HAL_GET_CPU_PORT(),*index_ptr), ret);
                }
                else
                {
                    DIAG_UTIL_ERR_CHK(apollomp_raw_vlan_extPortToMbrCfgIdx_set(port - 1, *index_ptr), ret);
                }
            break;
#endif

            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
            break;
        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_ext_pvid_port_ports_vlan_index_index */
#endif

/*
 * vlan get pvid port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_pvid_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_vlan_t pvid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portPvid_get(port, &pvid), ret);
        diag_util_printf("\n Port %d PVID: %d", port, pvid);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_pvid_port_ports_all */

/*
 * vlan set pvid port ( <PORT_LIST:ports> | all ) <UINT:vid>
 */
cparser_result_t
cparser_cmd_vlan_set_pvid_port_ports_all_vid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portPvid_set(port, (rtk_vlan_t)*vid_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_pvid_port_ports_all_vid */

/*
 * vlan get tag-mode port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_tag_mode_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_tagMode_t mode;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port Mode\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        /*call API*/
        DIAG_UTIL_ERR_CHK(rtk_vlan_tagMode_get(port,&mode), ret);

        diag_util_mprintf("%-4d ",port);

        switch(mode)
        {
            case VLAN_TAG_MODE_ORIGINAL:
                diag_util_mprintf("%s\n","original");
                break;
            case VLAN_TAG_MODE_KEEP_FORMAT:
                diag_util_mprintf("%s\n","keep-format");
                break;
            case VLAN_TAG_MODE_PRI:
                diag_util_mprintf("%s\n","priority-tag");
                break;
            default:
                diag_util_mprintf("%s\n",DIAG_STR_INVALID);
                break;

        }
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_tag_mode_port_ports_all */

/*
 * vlan set tag-mode port ( <PORT_LIST:ports> | all ) ( original | keep-format | priority-tag )
 */
cparser_result_t
cparser_cmd_vlan_set_tag_mode_port_ports_all_original_keep_format_priority_tag(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_tagMode_t mode;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(TOKEN_CHAR(5,0))
    {
        case 'o':
            mode = VLAN_TAG_MODE_ORIGINAL;
            break;

        case 'k':
            mode = VLAN_TAG_MODE_KEEP_FORMAT;
            break;

        case 'p':
            mode = VLAN_TAG_MODE_PRI;
            break;

        default:
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        /*call API*/
        DIAG_UTIL_ERR_CHK(rtk_vlan_tagMode_set(port,mode), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_tag_mode_port_ports_all_original_keep_format_priority_tag */

/*
 * vlan get vlan-table vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_table_vid_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_portmask_t tagPlst;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];
    uint32 meterIdx;
    rtk_enable_t policingState;
    rtk_enable_t priorityState;
    rtk_enable_t svlanHashState;
    rtk_pri_t priority;
    rtk_fid_t fid;
    rtk_fidMode_t fidMode;
    rtk_portmask_t member_portmask;
    rtk_portmask_t untag_portmask;
    rtk_portmask_t ext_portmask;
    uint32 extpmskidx;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

#if defined(CONFIG_LUNA_G3_SERIES)
    {
        rtk_portmask_t allPlst;

        DIAG_UTIL_ERR_CHK(rtk_vlan_port_get(*vid_ptr, &member_portmask, &untag_portmask), ret);
        DIAG_UTIL_ERR_CHK(rtk_vlan_fidMode_get(*vid_ptr, &fidMode), ret);
        
        diag_util_mprintf("Vid:%d\n", *vid_ptr);
        diag_util_mprintf("Hash\n");
        diag_util_mprintf("%-4s\n",
                            diagStr_l2HashMethodStr[(VLAN_FID_IVL == fidMode) ? 1 : 0 ]);
        diag_util_lPortMask2str(buf,&member_portmask);
        diag_util_mprintf("member port          : %s\n",buf);
        
        diag_util_reverse_portlist(DIAG_PORTTYPE_ALL, &untag_portmask, &tagPlst);

        HAL_GET_ALL_PORTMASK(allPlst);
        RTK_PORTMASK_AND(tagPlst,allPlst);

        diag_util_lPortMask2str(buf,&tagPlst);
        diag_util_mprintf("tag member port      : %s\n",buf);
    }
#else
    DIAG_UTIL_ERR_CHK(rtk_vlan_port_get(*vid_ptr, &member_portmask, &untag_portmask), ret);

	ret=rtk_vlan_extPort_get(*vid_ptr, &ext_portmask);
	if(RT_ERR_OK!=ret)
	{
	    if(ret!=RT_ERR_DRIVER_NOT_FOUND)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    memset(&ext_portmask,0x0,sizeof(rtk_portmask_t));
	}

	ret=rtk_vlan_extPortmaskIndex_get(*vid_ptr, &extpmskidx);
	if(RT_ERR_OK!=ret)
	{
	    if(ret!=RT_ERR_DRIVER_NOT_FOUND)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    extpmskidx = 0;
	}


    ret=rtk_vlan_fid_get(*vid_ptr, &fid);
    if(RT_ERR_OK!=ret)
    {
        if(ret!=RT_ERR_DRIVER_NOT_FOUND)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        fid = 0;
    }

    ret=rtk_vlan_fidMode_get(*vid_ptr, &fidMode);

    ret=rtk_vlan_priorityEnable_get(*vid_ptr, &priorityState);
    if(RT_ERR_OK!=ret)
    {
        if(ret!=RT_ERR_DRIVER_NOT_FOUND)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        priorityState = DISABLED;
    }

    ret=rtk_vlan_priority_get(*vid_ptr, &priority);
    if(RT_ERR_OK!=ret)
    {
        if(ret!=RT_ERR_DRIVER_NOT_FOUND)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        priority = 0;
    }
    ret=rtk_vlan_policingEnable_get(*vid_ptr, &policingState);
    if(RT_ERR_OK!=ret)
    {
        if(ret!=RT_ERR_DRIVER_NOT_FOUND)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        policingState = DISABLED;
    }
    ret=rtk_vlan_policingMeterIdx_get(*vid_ptr, &meterIdx);
    if(RT_ERR_OK!=ret)
    {
        if(ret!=RT_ERR_DRIVER_NOT_FOUND)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        meterIdx = 0;
    }

    ret=rtk_vlan_lutSvlanHashState_get(*vid_ptr, &svlanHashState);
    if(RT_ERR_OK!=ret)
    {
        if(ret!=RT_ERR_DRIVER_NOT_FOUND)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        svlanHashState = DISABLED;
    }

    diag_util_reverse_portlist(DIAG_PORTTYPE_ALL, &untag_portmask, &tagPlst);

    diag_util_mprintf("Vid:%d\n", *vid_ptr);

    diag_util_mprintf("Hash Policing Meter VlanPriEn Priority FID Svlanhash\n");
    diag_util_mprintf("%-4s %-8s %-5d %-9s %-8d %-3d %-9s\n",
                        diagStr_l2HashMethodStr[(VLAN_FID_IVL == fidMode) ? 1 : 0 ],
                        diagStr_enDisplay[policingState],
                        meterIdx,
                        diagStr_enDisplay[priorityState],
                        priority,
                        fid,
                        diagStr_enDisplay[svlanHashState]);

    diag_util_lPortMask2str(buf,&member_portmask);
    diag_util_mprintf("member port          : %s\n",buf);
    diag_util_lPortMask2str(buf,&tagPlst);
    diag_util_mprintf("tag member port      : %s\n",buf);
    diag_util_lPortMask2str(buf,&ext_portmask);
    diag_util_mprintf("extention member port: %s index:%d\n",buf, extpmskidx);
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_vlan_table_vid_vid */


/*
 * vlan set vlan-table vid <UINT:vid> hash-mode ( ivl | svl )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_hash_mode_ivl_svl(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_fidMode_t fidMode;

    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(6,0))
        fidMode = VLAN_FID_IVL;
    else if('s' == TOKEN_CHAR(6,0))
        fidMode = VLAN_FID_SVL;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_vlan_fidMode_set(*vid_ptr, fidMode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_hash_mode_ivl_svl */

/*
 * vlan set vlan-table vid <UINT:vid> member ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_member_ports_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_portmask_t member_portmask;
    rtk_portmask_t untag_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_ERR_CHK(rtk_vlan_port_get(*vid_ptr, &member_portmask, &untag_portmask), ret);

    RTK_PORTMASK_ASSIGN(member_portmask, portlist.portmask);

    DIAG_UTIL_ERR_CHK(rtk_vlan_port_set(*vid_ptr, &member_portmask, &untag_portmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_member_ports_all_none */

/*
 * vlan set vlan-table vid <UINT:vid> tag-member ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_tag_member_ports_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_portmask_t member_portmask;
    rtk_portmask_t untag_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_ERR_CHK(rtk_vlan_port_get(*vid_ptr, &member_portmask, &untag_portmask), ret);

    diag_util_reverse_portlist(DIAG_PORTTYPE_ALL,&(portlist.portmask),&untag_portmask);

    DIAG_UTIL_ERR_CHK(rtk_vlan_port_set(*vid_ptr, &member_portmask, &untag_portmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_tag_member_ports_all_none */

/*
 * vlan set vlan-table vid <UINT:vid> untag-member ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_untag_member_ports_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_portmask_t member_portmask;
    rtk_portmask_t untag_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_ERR_CHK(rtk_vlan_port_get(*vid_ptr, &member_portmask, &untag_portmask), ret);

    RTK_PORTMASK_ASSIGN(untag_portmask, portlist.portmask);

    DIAG_UTIL_ERR_CHK(rtk_vlan_port_set(*vid_ptr, &member_portmask, &untag_portmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_untag_member_ports_all_none */
/*
 * vlan set vlan-table vid <UINT:vid> svlan-hash  state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_svlan_hash_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(7, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(7, 0))
        enable = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_vlan_lutSvlanHashState_set(*vid_ptr, enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_svlan_hash_state_disable_enable */

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * vlan set vlan-table vid <UINT:vid> fid-msti <UINT:fid_msti>
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_fid_msti_fid_msti(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *fid_msti_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_fid_set(*vid_ptr, *fid_msti_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_fid_msti_fid_msti */

/*
 * vlan set vlan-table vid <UINT:vid> ext-member ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_ext_member_ext_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    char * *ext_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_portmask_t ext_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 6), ret);
    RTK_PORTMASK_ASSIGN(ext_portmask, portlist.portmask);

    DIAG_UTIL_ERR_CHK(rtk_vlan_extPort_set(*vid_ptr, &ext_portmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_ext_member_ext_all_none */

/*
 * vlan set vlan-table vid <UINT:vid> vlan-based-policing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_vlan_based_policing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(7,0))
        state = ENABLED;
    else if('d'==TOKEN_CHAR(7,0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_vlan_policingEnable_set(*vid_ptr, state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_vlan_based_policing_state_disable_enable */

/*
 * vlan set vlan-table vid <UINT:vid> meter <UINT:meter>
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_meter_meter(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *meter_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_policingMeterIdx_set(*vid_ptr, *meter_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_meter_meter */

/*
 * vlan set vlan-table vid <UINT:vid> vlan-based-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_vlan_based_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(7,0))
        state = ENABLED;
    else if('d'==TOKEN_CHAR(7,0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_vlan_priorityEnable_set(*vid_ptr, state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_vlan_based_priority_state_disable_enable */

/*
 * vlan set vlan-table vid <UINT:vid> vlan-based-priority priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_vlan_based_priority_priority_priority(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_priority_set(*vid_ptr, *priority_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_vlan_based_priority_priority_priority */

/*
 * vlan set vlan-table vid <UINT:vid> ext-member-index <UINT:index>
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_ext_member_index_index(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_extPortmaskIndex_set(*vid_ptr, *index_ptr), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_ext_member_index_index */

/*
 * vlan set ext-member entry <UINT:index> member ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_vlan_set_ext_member_entry_index_member_ext_all_none(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ext_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_portmask_t ext_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 6), ret);
    RTK_PORTMASK_ASSIGN(ext_portmask, portlist.portmask);

    DIAG_UTIL_ERR_CHK(rtk_vlan_extPortmaskCfg_set(*index_ptr, &ext_portmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_ext_member_entry_index_member_ext_all_none */

/*
 * vlan get ext-member entry <UINT:index>
 */
cparser_result_t
cparser_cmd_vlan_get_ext_member_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];
    rtk_portmask_t ext_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(rtk_vlan_extPortmaskCfg_get(*index_ptr, &ext_portmask), ret);

    diag_util_lPortMask2str(buf,&ext_portmask);
    diag_util_mprintf("index: %d, extention member port: %s\n",*index_ptr, buf);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_ext_member_entry_index */


#endif
/*
 * vlan get cfi-keep
 */
cparser_result_t
cparser_cmd_vlan_get_cfi_keep(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    /*call API*/
    DIAG_UTIL_ERR_CHK(rtk_vlan_cfiKeepEnable_get(&enable), ret);

    diag_util_printf("cfi-keep :%s \n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_cfi_keep */

/*
 * vlan set cfi-keep ( cfi-to-0 | keep-cfi )
 */
cparser_result_t
cparser_cmd_vlan_set_cfi_keep_cfi_to_0_keep_cfi(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    DIAG_UTIL_PARAM_CHK();

    if('c' == TOKEN_CHAR(3,0))
        enable = DISABLED;
    else
        enable = ENABLED;
    /*call API*/
    DIAG_UTIL_ERR_CHK(rtk_vlan_cfiKeepEnable_set(enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_cfi_keep_cfi_to_0_keep_cfi */

/*
 * vlan set keep-format ( cdp | csstp ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_keep_format_cdp_csstp_state_enable_disable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_keep_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(3, 1))
        type = KEEP_CDP;
    else if('s' == TOKEN_CHAR(3, 1))
        type = KEEP_SSTP;
    else
        return CPARSER_NOT_OK;

    if('e' == TOKEN_CHAR(5, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(5, 0))
        enable = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_vlan_keepType_set(type, enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_keep_format_cdp_csstp_state_enable_disable */

/*
 * vlan get keep-format ( cdp | csstp ) state
 */
cparser_result_t
cparser_cmd_vlan_get_keep_format_cdp_csstp_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_keep_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(3, 1))
        type = KEEP_CDP;
    else if('s' == TOKEN_CHAR(3, 1))
        type = KEEP_SSTP;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_vlan_keepType_get(type, &enable), ret);
    diag_util_mprintf("\n VLAN Keep format state of %s: %s \n", diagStr_vlanKeepType[type], diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_keep_format_cdp_csstp_state */

/*
 * vlan set keep-format rma <UINT:rma_tail> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_keep_format_rma_rma_tail_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *rma_tail_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_keep_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
        return CPARSER_NOT_OK;

    type = (rtk_vlan_keep_type_t)*rma_tail_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vlan_keepType_set(type, enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_keep_format_rma_rma_tail_state_disable_enable */

/*
 * vlan get keep-format rma <UINT:rma_tail> state
 */
cparser_result_t
cparser_cmd_vlan_get_keep_format_rma_rma_tail_state(
    cparser_context_t *context,
    uint32_t  *rma_tail_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_keep_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    type = (rtk_vlan_keep_type_t)*rma_tail_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vlan_keepType_get(type, &enable), ret);
    diag_util_mprintf("\n VLAN Keep format state of %s: %s \n", diagStr_vlanKeepType[type], diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_keep_format_rma_rma_tail_state */

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * vlan set port-based-fid port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid>
 */
cparser_result_t
cparser_cmd_vlan_set_port_based_fid_port_ports_all_filter_id_fid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *fid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_enable_t enable;
    uint32 fid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portFid_get(port,&enable,&fid), ret);

        DIAG_UTIL_ERR_CHK(rtk_vlan_portFid_set(port,enable,*fid_ptr), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_port_based_fid_port_ports_all_filter_id_fid */

/*
 * vlan set port-based-fid port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_port_based_fid_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    rtk_enable_t enable,setEnable;
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 fid;

    DIAG_UTIL_PARAM_CHK();
    if ('d' == TOKEN_CHAR(6,0))
        setEnable = DISABLED;
    else
        setEnable = ENABLED;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portFid_get(port,&enable,&fid), ret);

        DIAG_UTIL_ERR_CHK(rtk_vlan_portFid_set(port,setEnable,fid), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_port_based_fid_port_ports_all_state_disable_enable */

/*
 * vlan get port-based-fid port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_port_based_fid_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 fid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Port  FID  State\n");
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portFid_get(port,&enable,&fid), ret);
        diag_util_mprintf("%-4d  %-3d  %s\n",port,fid,diagStr_enable[enable]);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_port_based_fid_port_ports_all */

/*
 * vlan get port-based-pri port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_port_based_pri_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_pri_t priority;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portPriority_get(port, &priority), ret);
        diag_util_printf("\n Port %d VLAN based priority: %d", port, priority);
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_port_based_pri_port_ports_all */

/*
 * vlan set port-based-pri port ( <PORT_LIST:ports> | all ) priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_vlan_set_port_based_pri_port_ports_all_priority_priority(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portPriority_set(port, (rtk_pri_t)*priority_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_port_based_pri_port_ports_all_priority_priority */

/*
 * vlan set leaky ip-mcast port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_leaky_ip_mcast_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('d' == TOKEN_CHAR(7,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(7,0))
    {
        enable = ENABLED;
    }
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portLeaky_set(port, LEAKY_IPMULTICAST, enable), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_leaky_ip_mcast_port_ports_all_state_enable_disable */

/*
 * vlan get leaky ip-mcast port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_vlan_get_leaky_ip_mcast_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_util_mprintf("Port State\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portLeaky_get(port, LEAKY_IPMULTICAST, &enable), ret);
        diag_util_mprintf("%-4d %s\n", port, diagStr_enable[enable]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_leaky_ip_mcast_port_ports_all_state */

/*
 * vlan set leaky ( cdp | csstp | igmp-mld ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_leaky_cdp_csstp_igmp_mld_state_enable_disable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_leaky_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(3, 1))
        type = LEAKY_CDP;
    else if('s' == TOKEN_CHAR(3, 1))
        type = LEAKY_SSTP;
    else if('i' == TOKEN_CHAR(3, 0))
        type = LEAKY_IGMP;
    else
        return CPARSER_NOT_OK;

    if('e' == TOKEN_CHAR(5, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(5, 0))
        enable = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_vlan_leaky_set(type, enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_leaky_cdp_csstp_igmp_mld_state_enable_disable */

/*
 * vlan get leaky ( cdp | csstp | igmp-mld ) state
 */
cparser_result_t
cparser_cmd_vlan_get_leaky_cdp_csstp_igmp_mld_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_leaky_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(3, 1))
        type = LEAKY_CDP;
    else if('s' == TOKEN_CHAR(3, 1))
        type = LEAKY_SSTP;
    else if('i' == TOKEN_CHAR(3, 0))
        type = LEAKY_IGMP;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_vlan_leaky_get(type, &enable), ret);
    diag_util_mprintf("\n Leaky state of %s: %s \n", diagStr_vlanLeakyType[type], diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_leaky_cdp_csstp_igmp_mld_state */

/*
 * vlan set leaky rma <UINT:rma_tail> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_leaky_rma_rma_tail_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *rma_tail_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_leaky_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    type = (rtk_leaky_type_t)*rma_tail_ptr;

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        enable = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_vlan_leaky_set(type, enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_leaky_rma_rma_tail_state_disable_enable */

/*
 * vlan get leaky rma <UINT:rma_tail> state
 */
cparser_result_t
cparser_cmd_vlan_get_leaky_rma_rma_tail_state(
    cparser_context_t *context,
    uint32_t  *rma_tail_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_leaky_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    type = (rtk_leaky_type_t)*rma_tail_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vlan_leaky_get(type, &enable), ret);
    diag_util_mprintf("\n Leaky state of %s: %s \n", diagStr_vlanLeakyType[type], diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_leaky_rma_rma_tail_state */

/*
 * vlan get tag-mode ( ip4mc | ip6mc ) port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_tag_mode_ip4mc_ip6mc_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_vlan_tagModeIpmc_t tagMode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if('4' == TOKEN_CHAR(3, 2))
    {
        diag_util_mprintf("Port Mode\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_tagModeIp4mc_get(port, &tagMode), ret);

            diag_util_mprintf("%-4d ",port);

            switch(tagMode)
            {
                case VLAN_TAG_MODE_IPMC_ORIGINAL:
                    diag_util_mprintf("%s\n","original");
                    break;
                case VLAN_TAG_MODE_IPMC_KEEP_FORMAT:
                    diag_util_mprintf("%s\n","keep-format");
                    break;
                case VLAN_TAG_MODE_IPMC_PRI:
                    diag_util_mprintf("%s\n","priority-tag");
                    break;
                case VLAN_TAG_MODE_IPMC_DEFAULT:
                    diag_util_mprintf("%s\n","default");
                    break;
            }
        }
    }
    else if('6' == TOKEN_CHAR(3, 2))
    {
        diag_util_mprintf("Port Mode\n");
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_tagModeIp6mc_get(port, &tagMode), ret);

            diag_util_mprintf("%-4d ",port);

            switch(tagMode)
            {
                case VLAN_TAG_MODE_IPMC_ORIGINAL:
                    diag_util_mprintf("%s\n","original");
                    break;
                case VLAN_TAG_MODE_IPMC_KEEP_FORMAT:
                    diag_util_mprintf("%s\n","keep-format");
                    break;
                case VLAN_TAG_MODE_IPMC_PRI:
                    diag_util_mprintf("%s\n","priority-tag");
                    break;
                case VLAN_TAG_MODE_IPMC_DEFAULT:
                    diag_util_mprintf("%s\n","default");
                    break;
            }
        }
    }
    else
        return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_get_tag_mode_ip4mc_ip6mc_port_ports_all */

/*
 * vlan set tag-mode ( ip4mc | ip6mc ) port ( <PORT_LIST:ports> | all ) ( original | keep-format | priority-tag | default )
 */
cparser_result_t
cparser_cmd_vlan_set_tag_mode_ip4mc_ip6mc_port_ports_all_original_keep_format_priority_tag_default(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_vlan_tagModeIpmc_t tagMode;

    DIAG_UTIL_PARAM_CHK();

    if('o' == TOKEN_CHAR(6, 0))
        tagMode = VLAN_TAG_MODE_IPMC_ORIGINAL;
    else if('k' == TOKEN_CHAR(6, 0))
        tagMode = VLAN_TAG_MODE_IPMC_KEEP_FORMAT;
    else if('p' == TOKEN_CHAR(6, 0))
        tagMode = VLAN_TAG_MODE_IPMC_PRI;
    else if('d' == TOKEN_CHAR(6, 0))
        tagMode = VLAN_TAG_MODE_IPMC_DEFAULT;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);


    if('4' == TOKEN_CHAR(3, 2))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_tagModeIp4mc_set(port, tagMode), ret);
        }
    }
    else if('6' == TOKEN_CHAR(3, 2))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_tagModeIp6mc_set(port, tagMode), ret);
        }
    }
    else
        return CPARSER_NOT_OK;


    return CPARSER_OK;
}    /* end of cparser_cmd_vlan_set_tag_mode_ip4mc_ip6mc_port_ports_all_original_keep_format_priority_tag_default */
#endif



