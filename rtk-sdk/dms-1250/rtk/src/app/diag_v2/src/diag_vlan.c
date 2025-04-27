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
 * Purpose : Define diag shell functions for vlan.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) vlan diag shell.
 */


#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/util/rt_util.h>
#include <rtk/vlan.h>
#include <rtk/l3.h>
#include <rtk/bpe.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_vlan.h>
  #include <rtrpc/rtrpc_l3.h>
  #include <rtrpc/rtrpc_bpe.h>
#endif

const rtk_text_t text_accept_frame_type[] =
{
    { /* ACCEPT_FRAME_TYPE_ALL */          "all packet" },
    { /* ACCEPT_FRAME_TYPE_TAG_ONLY */     "tagged" },
    { /* ACCEPT_FRAME_TYPE_UNTAG_ONLY */   "untagged and priority-tagged" },
};

const rtk_text_t text_cnvt_frame_type[] =
{
    { /* VLAN_CNVT_TAG_STS_UNTAGGED */   "untagged" },
    { /* VLAN_CNVT_TAG_STS_TAGGED */     "tagged" },
    { /* VLAN_CNVT_TAG_STS_ALL */        "all packet" },
};

const rtk_text_t text_forward_action[] =
{
    { /* ACTION_FORWARD */          "forward" },
    { /* ACTION_DROP */             "drop" },
    { /* ACTION_TRAP2CPU */         "trap to cpu" },
    { /* ACTION_COPY2CPU */         "copy to cpu" },
    { /* ACTION_TRAP2MASTERCPU */   "trap to master-cpu" },
    { /* ACTION_COPY2MASTERCPU */   "copy to master-cpu" },
};

const rtk_text_t text_cnvt_vid_act[] =
{
    { /* VLAN_TAG_VID_ACT_NONE */                  "none" },
    { /* VLAN_TAG_VID_ACT_ASSIGN */                "force" },
    { /* VLAN_TAG_VID_ACT_SHIFT*/                  "shift" },
    { /* VLAN_TAG_VID_ACT_COPY_FROM_INNER */       "copy from inner" },
    { /* VLAN_TAG_PRIORITY_ACT_COPY_FROM_OUTER */  "copy from outer" },
};

const rtk_text_t text_cnvt_pri_act[] =
{
    { /* VLAN_TAG_PRIORITY_ACT_NONE */   "none" },
    { /* VLAN_TAG_PRIORITY_ACT_ASSIGN */ "force" },
    { /* VLAN_TAG_PRIORITY_ACT_COPY_FROM_INNER */   "copy from inner" },
    { /* VLAN_TAG_PRIORITY_ACT_COPY_FROM_OUTER */   "copy from outer" },
};

const rtk_text_t text_tag_status[] =
{
    { /* VLAN_TAG_STATUS_ACT_UNTAGGED */   "untag" },
    { /* VLAN_TAG_STATUS_ACT_TAGGED */     "tagged" },
    { /* VLAN_TAG_STATUS_ACT_NONE */       "not touch" },
};

const rtk_text_t text_l3urpf_mode[] =
{
    {/*RTK_L3_URPF_MODE_LOOSE*/     "Loose"},
    {/*RTK_L3_URPF_MODE_STRICT*/    "Strict"},
    {/*RTK_L3_URPF_MODE_END*/  "L3_URPF_MODE_END , remove it"},
};

const rtk_text_t text_bridge_mode[] =
{
    {/*VLAN_BRIDGE_MODE_MAC_BASED*/ "MAC-based" },
    {/*VLAN_BRIDGE_MODE_IP_BASED*/  "IP-based" },
    {/*VLAN_BRIDGE_MODE_END*/       "VLAN_BRIDGE_MODE_END , remove it" },
};



#ifdef CMD_VLAN_CREATE_VLAN_TABLE_VID_VID
/*
 * vlan create vlan-table vid  <UINT:vid>
 */
cparser_result_t cparser_cmd_vlan_create_vlan_table_vid_vid(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_create(unit, *vid_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_DESTROY_VLAN_TABLE_VID_VID
/*
 * vlan destroy vlan-table vid <UINT:vid>
 */
cparser_result_t cparser_cmd_vlan_destroy_vlan_table_vid_vid(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_destroy(unit, *vid_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_DESTROY_ALL_RESTORE_DEFAULT_VLAN
/*
 * vlan destroy all { restore-default-vlan }
 */
cparser_result_t cparser_cmd_vlan_destroy_all_restore_default_vlan(cparser_context_t *context)
{
    uint32      unit = 0;
    uint32      restore = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('r' == TOKEN_STR(3)[0])
    {
        restore = 1;
    }

    if ((ret = rtk_vlan_destroyAll(unit, restore)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_SVL_MODE_MAC_TYPE_VLAN
/*
 * vlan set svl-mode ( mac-type | vlan )
 */
cparser_result_t
cparser_cmd_vlan_set_svl_mode_mac_type_vlan(
    cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_svlMode_t  mode = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('m' == TOKEN_CHAR(3, 0))
        mode = VLAN_SVL_MODE_FID_MAC_TYPE;
    else if ('v' == TOKEN_CHAR(3, 0))
        mode = VLAN_SVL_MODE_FID_VLAN;

    if ((ret = rtk_vlan_svlMode_set(unit, mode)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_svl_mode_mac_type_vlan */
#endif

#ifdef CMD_VLAN_GET_SVL_MODE
/*
 * vlan get svl-mode
 */
cparser_result_t
cparser_cmd_vlan_get_svl_mode(
    cparser_context_t *context)
{
    uint32              unit = 0;
    rtk_vlan_svlMode_t  mode;
    int32               ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF(" VLAN SVL mode : ");
    if ((ret = rtk_vlan_svlMode_get(unit, &mode)) == RT_ERR_OK)
    {
        if (VLAN_SVL_MODE_FID_MAC_TYPE == mode)
            DIAG_UTIL_MPRINTF("MAC type (unicast/multicast)\n");
        else if (VLAN_SVL_MODE_FID_VLAN == mode)
            DIAG_UTIL_MPRINTF("VLAN\n");
    }
    else
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_svl_mode */
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_VID_VID_SVL_FID_FID
/*
 * vlan set vlan-table vid <UINT:vid> svl-fid <UINT:fid>
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_svl_fid_fid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *fid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_svlFid_set(unit, *vid_ptr, *fid_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_svl_fid_fid */
#endif

#ifdef CMD_VLAN_GET_VLAN_TABLE_VID_VID_SVL_FID
/*
 * vlan get vlan-table vid <UINT:vid> svl-fid
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_table_vid_vid_svl_fid(
    cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_fid_t   fid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_svlFid_get(unit, *vid_ptr, &fid)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("Vlan %u \n", *vid_ptr);
    DIAG_UTIL_MPRINTF("  SVL-FID\t: %u \n", fid);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_table_vid_vid_svl_fid */
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_VID_VID_MSTI_MSTI
/*
 *  vlan set vlan-table vid <UINT:vid> msti <UINT:msti>
 */
cparser_result_t cparser_cmd_vlan_set_vlan_table_vid_vid_msti_msti(cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *msti_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_stg_set(unit, *vid_ptr, *msti_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_VID_VID_FID_MSTI_FID_MSTI
/*
 *  vlan set vlan-table vid <UINT:vid> fid-msti <UINT:fid_msti>
 */
cparser_result_t cparser_cmd_vlan_set_vlan_table_vid_vid_fid_msti_fid_msti(cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *fid_msti_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_stg_set(unit, *vid_ptr, *fid_msti_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_VID_VID_MEMBER_PORTS_ALL_NONE
/*
 *  vlan set vlan-table vid <UINT:vid> member ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_table_vid_vid_member_ports_all_none(cparser_context_t *context,
    uint32_t *vid_ptr,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t member_portmask;
    diag_portlist_t untag_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_port_get(unit, *vid_ptr, &member_portmask.portmask, &untag_portmask.portmask)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(member_portmask, 6), ret);

    if ((ret = rtk_vlan_port_set(unit, *vid_ptr, &member_portmask.portmask, &untag_portmask.portmask)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_VID_VID_GROUP_GROUP_ID_NONE
/*
 * vlan set vlan-table vid <UINT:vid> group ( <UINT:group_id> | none )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_table_vid_vid_group_group_id_none(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *group_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_mcast_group_t group;

    group = ('n' == TOKEN_CHAR(6,0))? 0 : *group_id_ptr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_mcastGroup_set(unit, *vid_ptr, group)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_table_vid_vid_group_group_id_none */
#endif

#ifdef CMD_VLAN_ADD_VLAN_TABLE_VID_VID_MEMBER_PORT_PORT_UNTAG_TAG
/*
 * vlan add vlan-table vid <UINT:vid> member port <UINT:port> ( untag | tag )
 */
cparser_result_t
cparser_cmd_vlan_add_vlan_table_vid_vid_member_port_port_untag_tag(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *port_ptr)
{
    uint32      unit;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_t  vid = (rtk_vlan_t)*vid_ptr;
    uint32      untag = ('u' == TOKEN_CHAR(8,0))? 1 : 0;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_port_add(unit, vid, *port_ptr, untag), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_add_vlan_table_vid_vid_member_port_port_untag_tag */
#endif

#ifdef CMD_VLAN_DEL_VLAN_TABLE_VID_VID_MEMBER_PORT_PORT
/*
 * vlan del vlan-table vid <UINT:vid> member port <UINT:port>
 */
cparser_result_t
cparser_cmd_vlan_del_vlan_table_vid_vid_member_port_port(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *port_ptr)
{
    uint32      unit;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_t  vid = (rtk_vlan_t)*vid_ptr;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_port_del(unit, vid, *port_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_del_vlan_table_vid_vid_member_port_port */
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_VID_VID_UNTAG_PORT_PORTS_ALL_NONE
/*
 *  vlan set vlan-table vid <UINT:vid> untag-port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_table_vid_vid_untag_port_ports_all_none(cparser_context_t *context,
    uint32_t *vid_ptr,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    diag_portlist_t member_portmask;
    diag_portlist_t untag_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_port_get(unit, *vid_ptr, &member_portmask.portmask, &untag_portmask.portmask)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(untag_portmask, 6), ret);

    if ((ret = rtk_vlan_port_set(unit, *vid_ptr, &member_portmask.portmask, &untag_portmask.portmask)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_VID_VID_GROUPMASK_GROUP_MSK
/*
vlan set vlan-table vid <UINT:vid>  groupmask <HEX:group_msk>
*/
cparser_result_t cparser_cmd_vlan_set_vlan_table_vid_vid_groupmask_group_msk(cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *groupMask_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_groupMask_t groupMask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memcpy(&groupMask.bits[0], groupMask_ptr, sizeof(uint32));

    if ((ret = rtk_vlan_groupMask_set(unit, *vid_ptr, &groupMask)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_ACCEPT_FRAME_TYPE_INNER_PORT_PORTS_ALL_ALL_TAG_ONLY_UNTAG_ONLY
/*
 * vlan set accept-frame-type inner port ( <PORT_LIST:ports> | all ) ( all | tag-only | untag-only )
 */
cparser_result_t cparser_cmd_vlan_set_accept_frame_type_inner_port_ports_all_all_tag_only_untag_only(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_acceptFrameType_t      acceptFrame_type;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    switch (TOKEN_STR(6)[0])
    {
        case 'a':
            acceptFrame_type = ACCEPT_FRAME_TYPE_ALL;
            break;

        case 't':
            acceptFrame_type = ACCEPT_FRAME_TYPE_TAG_ONLY;
            break;

        case 'u':
            acceptFrame_type = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
            break;

        default:
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_vlan_portInnerAcceptFrameType_set(unit, port, acceptFrame_type)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portAcceptFrameType_set(unit, port, INNER_VLAN, acceptFrame_type)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_ACCEPT_FRAME_TYPE_OUTER_PORT_PORTS_ALL_ALL_TAG_ONLY_UNTAG_ONLY
/*
 * vlan set accept-frame-type outer port ( <PORT_LIST:ports> | all ) ( all | tag-only | untag-only )
 */
cparser_result_t cparser_cmd_vlan_set_accept_frame_type_outer_port_ports_all_all_tag_only_untag_only(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_acceptFrameType_t      acceptFrame_type;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    switch (TOKEN_STR(6)[0])
    {
        case 'a':
            acceptFrame_type = ACCEPT_FRAME_TYPE_ALL;
            break;

        case 't':
            acceptFrame_type = ACCEPT_FRAME_TYPE_TAG_ONLY;
            break;

        case 'u':
            acceptFrame_type = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
            break;

        default:
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_vlan_portOuterAcceptFrameType_set(unit, port, acceptFrame_type)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portAcceptFrameType_set(unit, port, OUTER_VLAN, acceptFrame_type)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_EGRESS_FILTER_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * vlan set egress-filter port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_egress_filter_port_ports_all_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_STR(6)[0])
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_vlan_portEgrFilterEnable_set(unit, port, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_LEAKY_MULTICAST_STATE_DISABLE_ENABLE
/*
 * vlan set leaky multicast state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_leaky_multicast_state_disable_enable(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    int32       (*fp_sys)(uint32, rtk_enable_t);
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_STR(5)[0])
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    fp_sys = rtk_vlan_mcastLeakyEnable_set;
    if ((ret = fp_sys(unit, enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_PVID_INNER_PORT_PORTS_ALL_PVID
/*
 * vlan set pvid inner port ( <PORT_LIST:ports> | all ) <UINT:pvid>
 */
cparser_result_t cparser_cmd_vlan_set_pvid_inner_port_ports_all_pvid(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *pvid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_vlan_portInnerPvid_set(unit, port, *pvid_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portPvid_set(unit, port, INNER_VLAN, *pvid_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_PVID_OUTER_PORT_PORTS_ALL_PVID
/*
 * vlan set pvid outer port ( <PORT_LIST:ports> | all ) <UINT:pvid>
 */
cparser_result_t cparser_cmd_vlan_set_pvid_outer_port_ports_all_pvid(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *pvid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_vlan_portOuterPvid_set(unit, port, *pvid_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portPvid_set(unit, port, OUTER_VLAN, *pvid_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_PROTOCOL_VLAN_GROUP_INDEX_FRAME_TYPE_UNKNOWN_ETHERNET_SNAP_LLC_OTHER_FRAME_VALUE_VALUE
/*
 * vlan set protocol-vlan group <UINT:index> frame-type ( unknown | ethernet | snap | llc-other ) frame-value <UINT:value>
 */
cparser_result_t cparser_cmd_vlan_set_protocol_vlan_group_index_frame_type_unknown_ethernet_snap_llc_other_frame_value_value(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *value_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_protoGroup_t   protoGroup;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch (TOKEN_STR(6)[0])
    {
        case 'u':
            protoGroup.frametype = FRAME_TYPE_UNKNOWN;
            break;

        case 'e':
            protoGroup.frametype = FRAME_TYPE_ETHERNET;
            break;

        case 's':
            protoGroup.frametype = FRAME_TYPE_RFC1042;
            break;

        case 'l':
            protoGroup.frametype = FRAME_TYPE_LLCOTHER;
            break;

        default:
            return CPARSER_NOT_OK;
    }

    protoGroup.framevalue = *value_ptr;

    if ((ret = rtk_vlan_protoGroup_set(unit, *index_ptr, &protoGroup)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_PROTOCOL_VLAN_INNER_PORT_PORTS_ALL_GROUP_INDEX_VID_VID_PRIORITY_PRIORITY
/*
 * vlan set protocol-vlan inner port ( <PORT_LIST:ports> | all ) group <UINT:index> vid <UINT:vid> priority <UINT:priority>
 */
cparser_result_t cparser_cmd_vlan_set_protocol_vlan_inner_port_ports_all_group_index_vid_vid_priority_priority(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *index_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlan_protoVlanCfg_t     protoVlan;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        ret = rtk_vlan_portProtoVlan_get(unit, port, *index_ptr, &protoVlan);
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        protoVlan.vid = *vid_ptr;
        protoVlan.pri = *priority_ptr;
        if ((ret = rtk_vlan_portProtoVlan_set(unit, port, *index_ptr, &protoVlan)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_PROTOCOL_VLAN_PORT_PORTS_ALL_GROUP_INDEX_VLAN_TYPE_INNER_OUTER_VID_VID_STATE_DISABLE_ENABLE_PRIORITY_PRIORITY_STATE_DISABLE_ENABLE
/*
 * vlan set protocol-vlan port ( <PORT_LIST:ports> | all ) group <UINT:index> vlan-type ( inner | outer ) vid <UINT:vid> state (disable | enable) priority <UINT:priority> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_protocol_vlan_port_ports_all_group_index_vlan_type_inner_outer_vid_vid_state_disable_enable_priority_priority_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *index_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlan_protoVlanCfg_t     protoVlan;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&protoVlan, 0, sizeof(rtk_vlan_protoVlanCfg_t));

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if('i' == TOKEN_CHAR(8, 0))
        protoVlan.vlan_type = INNER_VLAN;
    else
        protoVlan.vlan_type = OUTER_VLAN;

    if('e' == TOKEN_CHAR(12, 0))
    {
        protoVlan.vid_assign = ENABLED;
        protoVlan.vid = *vid_ptr;
    }
    else
    {
        protoVlan.vid_assign = DISABLED;
    }

    if('e' == TOKEN_CHAR(16, 0))
    {
        protoVlan.pri_assign = ENABLED;
        protoVlan.pri = *priority_ptr;
    }
    else
    {
        protoVlan.pri_assign = DISABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_vlan_portProtoVlan_set(unit, port, *index_ptr, &protoVlan)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_PROTOCOL_VLAN_PORT_PORTS_ALL_GROUP_INDEX_STATE_DISABLE_ENABLE
/*
 * vlan set protocol-vlan port ( <PORT_LIST:ports> | all ) group <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_protocol_vlan_port_ports_all_group_index_state_disable_enable(
        cparser_context_t *context,
        char **ports_ptr,
        uint32_t *index_ptr)
{
    uint32                  unit = 0;
    uint32                  valid;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_t              port;
    diag_portlist_t         portmask;
    rtk_vlan_protoVlanCfg_t protoVlan;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if('e' == TOKEN_CHAR(8, 0))
    {
        valid = 1;
    }
    else
    {
        valid = 0;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        ret = rtk_vlan_portProtoVlan_get(unit, port, *index_ptr, &protoVlan);
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        protoVlan.valid = valid;

        ret = rtk_vlan_portProtoVlan_set(unit, port, *index_ptr, &protoVlan);
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_TPID_INNER_OUTER_EXTRA_ENTRY_TPID_IDX_TPID_TPID
/*
 * vlan set tpid ( inner | outer | extra ) entry <UINT:tpid_idx> tpid <UINT:tpid>
 */
cparser_result_t cparser_cmd_vlan_set_tpid_inner_outer_extra_entry_tpid_idx_tpid_tpid(cparser_context_t *context,
    uint32_t *tpid_idx_ptr,
    uint32_t *tpid_ptr)
{
    uint32  unit = 0;
    rtk_vlan_tagType_t tagType;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('i' == TOKEN_CHAR(3, 0))
    {
        tagType = VLAN_TAG_TYPE_INNER;
    }
    else if('o' == TOKEN_CHAR(3, 0))
    {
        tagType = VLAN_TAG_TYPE_OUTER;
    }
    else
    {
        tagType = VLAN_TAG_TYPE_EXTRA;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if (tagType == VLAN_TAG_TYPE_INNER)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_innerTpidEntry_set(unit, *tpid_idx_ptr, *tpid_ptr) , ret);
        }
        else if (tagType == VLAN_TAG_TYPE_OUTER)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_outerTpidEntry_set(unit, *tpid_idx_ptr, *tpid_ptr) , ret);
        }
        else if (tagType == VLAN_TAG_TYPE_EXTRA)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_extraTpidEntry_set(unit, *tpid_idx_ptr, *tpid_ptr) , ret);
        }
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_tpidEntry_set(unit, tagType, *tpid_idx_ptr, *tpid_ptr) , ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_INGRESS_PORT_PORTS_ALL_INNER_OUTER_TPID_TPID_IDX_MASK
/*
 * vlan set ingress port ( <PORT_LIST:ports> | all ) ( inner | outer ) tpid <HEX:tpid_idx_mask>
 */
cparser_result_t cparser_cmd_vlan_set_ingress_port_ports_all_inner_outer_tpid_tpid_idx_mask(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *tpid_idx_mask_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlanType_t type;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    switch (TOKEN_STR(5)[0])
    {
        case 'i':
            type = INNER_VLAN;
            break;

        case 'o':
            type = OUTER_VLAN;
            break;

        default:
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if (type == INNER_VLAN)
            {
                if ((ret = rtk_vlan_portIgrInnerTpid_set(unit, port, *tpid_idx_mask_ptr)) != RT_ERR_OK)
                {
                    DIAG_ERR_PRINT(ret);
                    return CPARSER_NOT_OK;
                }
            }
            else if (type == OUTER_VLAN)
            {
                if ((ret = rtk_vlan_portIgrOuterTpid_set(unit, port, *tpid_idx_mask_ptr)) != RT_ERR_OK)
                {
                    DIAG_ERR_PRINT(ret);
                    return CPARSER_NOT_OK;
                }
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portIgrTpid_set(unit, port, type, *tpid_idx_mask_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_INGRESS_PORT_PORTS_ALL_EXTRA_STATE_DISABLE_ENABLE
/*
 * vlan set ingress port ( <PORT_LIST:ports> | all ) extra state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_ingress_port_ports_all_extra_state_disable_enable(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t      enable;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if ('e' == TOKEN_STR(7)[0])
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_vlan_portIgrExtraTagEnable_set(unit, port, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_EGRESS_PORT_PORTS_ALL_INNER_OUTER_TPID_TPID_IDX
/*
 * vlan set egress port ( <PORT_LIST:ports> | all ) ( inner | outer ) tpid <UINT:tpid_idx>
 */
cparser_result_t cparser_cmd_vlan_set_egress_port_ports_all_inner_outer_tpid_tpid_idx(cparser_context_t *context,
    char **ports_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlanType_t type;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('i' == TOKEN_STR(5)[0])
    {
        type = INNER_VLAN;
    }
    else
    {
        type = OUTER_VLAN;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);


    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if (type == INNER_VLAN)
            {
                if ((ret = rtk_vlan_portEgrInnerTpid_set(unit, port, *tpid_idx_ptr)) != RT_ERR_OK)
                {
                    DIAG_ERR_PRINT(ret);
                    return CPARSER_NOT_OK;
                }
            }
            else if (type == OUTER_VLAN)
            {
                if ((ret = rtk_vlan_portEgrOuterTpid_set(unit, port, *tpid_idx_ptr)) != RT_ERR_OK)
                {
                    DIAG_ERR_PRINT(ret);
                    return CPARSER_NOT_OK;
                }
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portEgrTpid_set(unit, port, type, *tpid_idx_ptr)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_INGRESS_PORT_PORTS_ALL_INNER_OUTER_KEEP_TAG_STATE_DISABLE_ENABLE
/*
 * vlan set ingress port ( <PORT_LIST:ports> | all ) ( inner | outer ) keep-tag state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_ingress_port_ports_all_inner_outer_keep_tag_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_vlanType_t  type;
    rtk_enable_t    enable;
    rtk_port_t      port;
    diag_portlist_t portmask;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    rtk_enable_t    keepOuter, keepInner;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        type = INNER_VLAN;
    }
    else
    {
        type = OUTER_VLAN;
    }

    if ('e' == TOKEN_CHAR(8, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_vlan_portIgrTagKeepEnable_get(unit, port, &keepOuter, &keepInner)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        if(INNER_VLAN == type)
        {
            if ((ret = rtk_vlan_portIgrTagKeepEnable_set(unit, port, keepOuter, enable)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
        {
            if ((ret = rtk_vlan_portIgrTagKeepEnable_set(unit, port, enable, keepInner)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }
    else
#endif
    {
        if ((ret = rtk_vlan_portIgrVlanTransparentEnable_set(unit, port, type, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_EGRESS_PORT_PORTS_ALL_INNER_OUTER_KEEP_TAG_STATE_DISABLE_ENABLE
/*
 * vlan set egress port ( <PORT_LIST:ports> | all ) ( inner | outer ) keep-tag state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_egress_port_ports_all_inner_outer_keep_tag_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_vlanType_t  type;
    rtk_enable_t    enable;
    rtk_port_t      port;
    diag_portlist_t portmask;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    rtk_enable_t    keepOuter, keepInner;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        type = INNER_VLAN;
    }
    else
    {
        type = OUTER_VLAN;
    }

    if ('e' == TOKEN_CHAR(8, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_vlan_portEgrTagKeepEnable_get(unit, port, &keepOuter, &keepInner)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        if(INNER_VLAN == type)
        {
            if ((ret = rtk_vlan_portEgrTagKeepEnable_set(unit, port, keepOuter, enable)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
        {
            if ((ret = rtk_vlan_portEgrTagKeepEnable_set(unit, port, enable, keepInner)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }
    else
#endif
    {
        if ((ret = rtk_vlan_portEgrVlanTransparentEnable_set(unit, port, type,enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_EGRESS_PORT_PORTS_ALL_INNER_OUTER_KEEP_TPID_STATE_DISABLE_ENABLE
/*
 * vlan set egress port ( <PORT_LIST:ports> | all ) ( inner | outer ) keep-tpid state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_egress_port_ports_all_inner_outer_keep_tpid_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_vlanType_t  type;
    rtk_vlan_egrTpidSrc_t tpidSrc;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        type = INNER_VLAN;
    }
    else
    {
        type = OUTER_VLAN;
    }

    if ('e' == TOKEN_CHAR(8, 0))
    {
        tpidSrc = VLAN_TPID_SRC_ORIG;
    }
    else
    {
        tpidSrc = VLAN_TPID_SRC_EGR;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_vlan_portEgrTpidSrc_set(unit, port, type, tpidSrc)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_VID_VID_PROFILE_INDEX_INDEX
/*
 * vlan set vlan-table vid <UINT:vid> profile-index <UINT:index>
 */
cparser_result_t cparser_cmd_vlan_set_vlan_table_vid_vid_profile_index_index(cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_profileIdx_set(unit, *vid_ptr, *index_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_SA_LEARNING_STATE_ASIC_LEARN_SOFTWARE_LEARN_NOT_LEARN
/*
 * vlan set profile entry <UINT:index> sa-learning state ( asic-learn | software-learn | not-learn )
 */
cparser_result_t cparser_cmd_vlan_set_profile_entry_index_sa_learning_state_asic_learn_software_learn_not_learn(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_profile_t profile;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    if('e' == TOKEN_CHAR(7, 0))
    {
        profile.learn = ENABLED;
    }
    else
    {
        profile.learn = DISABLED;
    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if ('a' == TOKEN_CHAR(7, 0))
    {
        profile.lrnMode = HARDWARE_LEARNING;
    }
    else if ('s' == TOKEN_CHAR(7, 0))
    {
        profile.lrnMode = SOFTWARE_LEARNING;
    }
    else
    {
        profile.lrnMode = NOT_LEARNING;
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_SRC_MAC_ACTION_FORWARD_DROP_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * vlan set profile entry <UINT:index> src-mac action ( forward | drop | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_src_mac_action_forward_drop_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARSE_ACTION(7, action);

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);
    profile.l2_newMacAct = action;
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);
#endif
    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_src_mac_action_forward_drop_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_SRC_MAC_LEARN_MODE_ASIC_LEARN_SOFTWARE_LEARN_NOT_LEARN_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
 * vlan set profile entry <UINT:index> src-mac-learn-mode ( asic-learn | software-learn | not-learn ) action ( copy-to-cpu | drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_src_mac_learn_mode_asic_learn_software_learn_not_learn_action_copy_to_cpu_drop_forward_trap_to_cpu(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARSE_ACTION(8, action);

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_vlan_profile_get(unit, *index_ptr, &profile);

    if ('a' == TOKEN_CHAR(6, 0))
    {
        profile.lrnMode = HARDWARE_LEARNING;
    }
    else if ('s' == TOKEN_CHAR(6, 0))
    {
        profile.lrnMode = SOFTWARE_LEARNING;
    }
    else
    {
        profile.lrnMode = NOT_LEARNING;
    }

    profile.l2_newMacAct = action;
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);
#endif

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_src_mac_learn_mode_asic_learn_software_learn_not_learn_action_copy_to_cpu_drop_forward_trap_to_cpu */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_SRC_MAC_LEARN_MODE_ASIC_LEARN_SOFTWARE_LEARN_NOT_LEARN_ACTION_COPY_TO_MASTER_TRAP_TO_MASTER
/*
 * vlan set profile entry <UINT:index> src-mac-learn-mode ( asic-learn | software-learn | not-learn ) action ( copy-to-master | trap-to-master )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_src_mac_learn_mode_asic_learn_software_learn_not_learn_action_copy_to_master_trap_to_master(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARSE_ACTION(8, action);

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_vlan_profile_get(unit, *index_ptr, &profile);

    if ('a' == TOKEN_CHAR(6, 0))
    {
        profile.lrnMode = HARDWARE_LEARNING;
    }
    else if ('s' == TOKEN_CHAR(6, 0))
    {
        profile.lrnMode = SOFTWARE_LEARNING;
    }
    else
    {
        profile.lrnMode = NOT_LEARNING;
    }

    profile.l2_newMacAct = action;
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);
#endif

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_src_mac_learn_mode_asic_learn_software_learn_not_learn_action_copy_to_master_trap_to_master */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_IPUC_IP6UC_IPMC_IP6MC_ROUTING_STATE_ENABLE_DISABLE
/*
 * vlan set profile entry <UINT:index> ( ipuc | ip6uc | ipmc | ip6mc ) routing state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_ipuc_ip6uc_ipmc_ip6mc_routing_state_enable_disable(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);
    DIAG_UTIL_PARSE_STATE(8, state);

    if ('6' == TOKEN_CHAR(5, 2))
    {
        if ('u' == TOKEN_CHAR(5, 3))
            profile.ip6_uRoute = state;
        else
            profile.ip6_mRoute = state;
    }
    else
    {
        if ('u' == TOKEN_CHAR(5, 2))
            profile.ip4_uRoute = state;
        else
            profile.ip4_mRoute = state;
    }

    DIAG_UTIL_MPRINTF("%s routing state: %s\n", TOKEN_STR(5), text_state[state]);
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_ipuc_ip6uc_ipmc_ip6mc_routing_state_enable_disable */
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX_IPUC_IP6UC_IPMC_IP6MC_ROUTING_STATE
/*
 * vlan get profile entry <UINT:index> ( ipuc | ip6uc | ipmc | ip6mc ) routing state
 */
cparser_result_t
cparser_cmd_vlan_get_profile_entry_index_ipuc_ip6uc_ipmc_ip6mc_routing_state(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('6' == TOKEN_CHAR(5, 2))
    {
        if ('u' == TOKEN_CHAR(5, 3))
            state = profile.ip6_uRoute;
        else
            state = profile.ip6_mRoute;
    }
    else
    {
        if ('u' == TOKEN_CHAR(5, 2))
            state = profile.ip4_uRoute;
        else
            state = profile.ip4_mRoute;
    }

    DIAG_UTIL_MPRINTF("%s routing state: %s\n", TOKEN_STR(5), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_profile_entry_index_ipuc_ip6uc_ipmc_ip6mc_routing_state */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_IPUC_IP6UC_URPF_STATE_ENABLE_DISABLE
/*
 * vlan set profile entry <UINT:index> ( ipuc | ip6uc ) urpf state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_ipuc_ip6uc_urpf_state_enable_disable(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    DIAG_UTIL_PARSE_STATE(8, state);
    if ('u' == TOKEN_CHAR(5, 2))
        profile.ip4_uRPF_chk = state;
    else if ('6' == TOKEN_CHAR(5, 2))
        profile.ip6_uRPF_chk = state;

    DIAG_UTIL_MPRINTF("%s urpf state: %s\n", TOKEN_STR(5), text_state[state]);
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_ipuc_ip6uc_urpf_state_enable_disable */
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX_IPUC_IP6UC_URPF_STATE
/*
 * vlan get profile entry <UINT:index> ( ipuc | ip6uc ) urpf state
 */
cparser_result_t
cparser_cmd_vlan_get_profile_entry_index_ipuc_ip6uc_urpf_state(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('u' == TOKEN_CHAR(5, 2))
        state = profile.ip4_uRPF_chk;
    else if ('6' == TOKEN_CHAR(5, 2))
        state = profile.ip6_uRPF_chk;

    DIAG_UTIL_MPRINTF("%s urpf state: %s\n", TOKEN_STR(5), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_profile_entry_index_ipuc_ip6uc_urpf_state */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_IPUC_IP6UC_URPF_DEFAULT_ROUTE_STATE_ENABLE_DISABLE
/*
 * vlan set profile entry <UINT:index> ( ipuc | ip6uc ) urpf default-route state ( enable | disable )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_ipuc_ip6uc_urpf_default_route_state_enable_disable(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    DIAG_UTIL_PARSE_STATE(9, state);
    if ('u' == TOKEN_CHAR(5, 2))
        profile.ip4_uRPF_dfltRoute = state;
    else if ('6' == TOKEN_CHAR(5, 2))
        profile.ip6_uRPF_dfltRoute = state;

    DIAG_UTIL_MPRINTF("%s urpf default-route state: %s\n", TOKEN_STR(5), text_state[state]);
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_ipuc_ip6uc_urpf_default_route_state_enable_disable */
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX_IPUC_IP6UC_URPF_DEFAULT_ROUTE_STATE
/*
 * vlan get profile entry <UINT:index> ( ipuc | ip6uc ) urpf default-route state
 */
cparser_result_t
cparser_cmd_vlan_get_profile_entry_index_ipuc_ip6uc_urpf_default_route_state(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('u' == TOKEN_CHAR(5, 2))
        state =  profile.ip4_uRPF_dfltRoute;
    else if ('6' == TOKEN_CHAR(5, 2))
        state =  profile.ip6_uRPF_dfltRoute;

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);
    DIAG_UTIL_MPRINTF("%s urpf default-route state: %s\n", TOKEN_STR(5), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_profile_entry_index_ipuc_ip6uc_urpf_default_route_state */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_IPUC_IP6UC_URPF_MODE_LOOSE_STRICT
/*
 * vlan set profile entry <UINT:index> ( ipuc | ip6uc ) urpf mode ( loose | strict )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_ipuc_ip6uc_urpf_mode_loose_strict(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    uint32 mode = RTK_L3_URPF_MODE_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('l' == TOKEN_CHAR(8, 0))
        mode = RTK_L3_URPF_MODE_LOOSE;
    else if ('s' == TOKEN_CHAR(8, 0))
        mode = RTK_L3_URPF_MODE_STRICT;

    if ('u' == TOKEN_CHAR(5, 2))
        profile.ip4_uRPF_chkMode = mode;
    else if ('6' == TOKEN_CHAR(5, 2))
        profile.ip6_uRPF_chkMode = mode;

    DIAG_UTIL_MPRINTF("%s urpf mode: %s\n", TOKEN_STR(5), text_l3urpf_mode[mode].text);
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_ipuc_ip6uc_urpf_mode_loose_strict */
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX_IPUC_IP6UC_URPF_MODE
/*
 * vlan get profile entry <UINT:index> ( ipuc | ip6uc ) urpf mode
 */
cparser_result_t
cparser_cmd_vlan_get_profile_entry_index_ipuc_ip6uc_urpf_mode(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    uint32 mode = RTK_L3_URPF_MODE_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('u' == TOKEN_CHAR(5, 2))
        mode = profile.ip4_uRPF_chkMode;
    else if ('6' == TOKEN_CHAR(5, 2))
        mode = profile.ip6_uRPF_chkMode;

    DIAG_UTIL_MPRINTF("%s urpf mode: %s\n", TOKEN_STR(5), text_l3urpf_mode[mode].text);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_profile_entry_index_ipuc_ip6uc_urpf_mode */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_IPUC_IP6UC_URPF_FAIL_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * vlan set profile entry <UINT:index> ( ipuc | ip6uc ) urpf fail-action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_ipuc_ip6uc_urpf_fail_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_ACTION(8, action);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('u' == TOKEN_CHAR(5, 2))
        profile.ip4_uRPF_failAct = action;
    else if ('6' == TOKEN_CHAR(5, 2))
        profile.ip6_uRPF_failAct = action;

    DIAG_UTIL_MPRINTF("%s urpf fail-action: %s\n", TOKEN_STR(5), text_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of
cparser_cmd_vlan_set_profile_entry_index_ipuc_ip6uc_urpf_fail_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX_IPUC_IP6UC_URPF_FAIL_ACTION
/*
 * vlan get profile entry <UINT:index> ( ipuc | ip6uc ) urpf fail-action
 */
cparser_result_t
cparser_cmd_vlan_get_profile_entry_index_ipuc_ip6uc_urpf_fail_action(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('u' == TOKEN_CHAR(5, 2))
        action = profile.ip4_uRPF_failAct;
    else if ('6' == TOKEN_CHAR(5, 2))
        action = profile.ip6_uRPF_failAct;

    DIAG_UTIL_MPRINTF("%s urpf fail-action: %s\n", TOKEN_STR(5), text_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_profile_entry_index_ipuc_ip6uc_urpf_fail_action */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_IPMC_IP6MC_MC_BRIDGE_MODE_MAC_BASE_IP_BASE
/*
 * vlan set profile entry <UINT:index> ( ipmc | ip6mc ) mc-bridge mode ( mac-base | ip-base )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_ipmc_ip6mc_mc_bridge_mode_mac_base_ip_base(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_bridgeMode_t  mode = VLAN_BRIDGE_MODE_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('m' == TOKEN_CHAR(8, 0))
        mode = VLAN_BRIDGE_MODE_MAC_BASED;
    else if ('i' == TOKEN_CHAR(8, 0))
        mode = VLAN_BRIDGE_MODE_IP_BASED;

    if ('m' == TOKEN_CHAR(5, 2))
        profile.ipmcBdgMode = mode;
    else if ('6' == TOKEN_CHAR(5, 2))
        profile.ip6mcBdgMode = mode;

    DIAG_UTIL_MPRINTF("%s mc-bridge mode : %s\n", TOKEN_STR(5), text_bridge_mode[mode].text);
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_ipmc_ip6mc_mc_bridge_mode_mac_base_ip_base */
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX_IPMC_IP6MC_MC_BRIDGE_MODE
/*
 * vlan get profile entry <UINT:index> ( ipmc | ip6mc ) mc-bridge mode
 */
cparser_result_t
cparser_cmd_vlan_get_profile_entry_index_ipmc_ip6mc_mc_bridge_mode(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_bridgeMode_t  mode = VLAN_BRIDGE_MODE_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('m' == TOKEN_CHAR(5, 2))
        mode = profile.ipmcBdgMode;
    else if ('6' == TOKEN_CHAR(5, 2))
        mode = profile.ip6mcBdgMode;

    DIAG_UTIL_MPRINTF("%s mc-bridge mode : %s\n", TOKEN_STR(5), text_bridge_mode[mode].text);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_profile_entry_index_ipmc_ip6mc_mc_bridge_mode */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_IPMC_DIP_224_0_0_X_224_0_1_X_239_X_X_X_ACTION_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * vlan set profile entry <UINT:index> ipmc dip ( 224-0-0-x | 224-0-1-x | 239-x-x-x ) action ( forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_ipmc_dip_224_0_0_x_224_0_1_x_239_x_x_x_action_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_ACTION(9, action);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('0' == TOKEN_CHAR(7, 6))
        profile.ipmc_224_0_act = action;
    else if ('1' == TOKEN_CHAR(7, 6))
        profile.ipmc_224_1_act = action;
    else if ('x' == TOKEN_CHAR(7, 6))
        profile.ipmc_239_0_act = action;

    DIAG_UTIL_MPRINTF("ipmc %s: %s\n", TOKEN_STR(7), text_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of
cparser_cmd_vlan_set_profile_entry_index_ipmc_dip_224_0_0_x_224_0_1_x_239_x_x_x_action_forward_trap_to_cpu_copy_to_cpu_trap_to_master_c
opy_to_master */
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX_IPMC_DIP_224_0_0_X_224_0_1_X_239_X_X_X_ACTION
/*
 * vlan get profile entry <UINT:index> ipmc dip ( 224-0-0-x | 224-0-1-x | 239-x-x-x ) action
 */
cparser_result_t
cparser_cmd_vlan_get_profile_entry_index_ipmc_dip_224_0_0_x_224_0_1_x_239_x_x_x_action(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('0' == TOKEN_CHAR(7, 6))
        action = profile.ipmc_224_0_act;
    else if ('1' == TOKEN_CHAR(7, 6))
        action = profile.ipmc_224_1_act;
    else if ('x' == TOKEN_CHAR(7, 6))
        action = profile.ipmc_239_0_act;

    DIAG_UTIL_MPRINTF("ipmc %s: %s\n", TOKEN_STR(7), text_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_profile_entry_index_ipmc_dip_224_0_0_x_224_0_1_x_239_x_x_x_action */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_IP6MC_DIP_XXXX_0000_00XX_FF0X_XXXX_XXXX_FF0X_DB8_0_0_ACTION_FORWARD_FLOOD_IN_VLAN_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * vlan set profile entry <UINT:index> ip6mc dip ( xxxx-0000-00xx | ff0x-xxxx-xxxx | ff0x-db8-0-0 ) action ( forward | flood-in-vlan | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_ip6mc_dip_xxxx_0000_00xx_ff0x_xxxx_xxxx_ff0x_db8_0_0_action_forward_flood_in_vlan_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_ACTION(9, action);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('0' == TOKEN_CHAR(7, 5))
        profile.ip6mc_00xx_act = action;
    else if ('x' == TOKEN_CHAR(7, 5))
        profile.ip6mc_mld_0x_act = action;
    else if ('d' == TOKEN_CHAR(7, 5))
        profile.ip6mc_mld_db8_act = action;

    DIAG_UTIL_MPRINTF("ip6mc %s: %s\n", TOKEN_STR(7), text_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of
cparser_cmd_vlan_set_profile_entry_index_ip6mc_dip_xxxx_0000_00xx_ff0x_xxxx_xxxx_ff0x_db8_0_0_action_forward_flood_in_vlan_trap_to_cpu_copy_to_cpu_tr
ap_to_master_copy_to_master */
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX_IP6MC_DIP_XXXX_0000_00XX_FF0X_XXXX_XXXX_FF0X_DB8_0_0_ACTION
/*
 * vlan get profile entry <UINT:index> ip6mc dip ( xxxx-0000-00xx | ff0x-xxxx-xxxx | ff0x-db8-0-0 ) action
 */
cparser_result_t
cparser_cmd_vlan_get_profile_entry_index_ip6mc_dip_xxxx_0000_00xx_ff0x_xxxx_xxxx_ff0x_db8_0_0_action(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if ('0' == TOKEN_CHAR(7, 5))
        action = profile.ip6mc_00xx_act;
    else if ('x' == TOKEN_CHAR(7, 5))
        action = profile.ip6mc_mld_0x_act;
    else if ('d' == TOKEN_CHAR(7, 5))
        action = profile.ip6mc_mld_db8_act;

    DIAG_UTIL_MPRINTF("ip6mc %s: %s\n", TOKEN_STR(7), text_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_profile_entry_index_ip6mc_dip_xxxx_0000_00xx_ff0x_xxxx_xxxx_ff0x_db8_0_0_action */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_LOOKUP_MISS_TYPE_L2_MCAST_IP4_MCAST_IP6_MCAST_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * vlan set profile entry <UINT:index> lookup-miss-type ( l2-mcast | ip4-mcast | ip6-mcast ) action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_lookup_miss_type_l2_mcast_ip4_mcast_ip6_mcast_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_ACTION(8, action);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);
    if('l' == TOKEN_CHAR(6, 0))
    {
        profile.l2mc_bdgLuMisAct = action;
    }
    else if('i' == TOKEN_CHAR(6, 0))
    {
        if('4' == TOKEN_CHAR(6, 2))
            profile.ipmc_bdgLuMisAct = action;
        else if ('6' == TOKEN_CHAR(6, 2))
            profile.ip6mc_bdgLuMisAct = action;
    }

    DIAG_UTIL_MPRINTF("%s bridge lookup miss action: %s\n", TOKEN_STR(6), text_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_lookup_miss_type_l2_mcast_ip4_mcast_ip6_mcast_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX_LOOKUP_MISS_TYPE_L2_MCAST_IP4_MCAST_IP6_MCAST_ACTION
/*
 * vlan get profile entry <UINT:index> lookup-miss-type ( l2-mcast | ip4-mcast | ip6-mcast ) action
 */
cparser_result_t
cparser_cmd_vlan_get_profile_entry_index_lookup_miss_type_l2_mcast_ip4_mcast_ip6_mcast_action(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_vlan_profile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if('l' == TOKEN_CHAR(6, 0))
    {
        action = profile.l2mc_bdgLuMisAct;
    }
    else if('i' == TOKEN_CHAR(6, 0))
    {
        if('4' == TOKEN_CHAR(6, 2))
            action = profile.ipmc_bdgLuMisAct;
        else if ('6' == TOKEN_CHAR(6, 2))
            action = profile.ip6mc_bdgLuMisAct;
    }

    DIAG_UTIL_MPRINTF("%s bridge lookup miss action: %s\n", TOKEN_STR(6), text_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_profile_entry_index_lookup_miss_type_l2_mcast_ip4_mcast_ip6_mcast_action */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_LOOKUP_MISS_TYPE_L2_MCAST_IP4_MCAST_IP6_MCAST_PORT_PORTS_ALL
/*
 * vlan set profile entry <UINT:index> lookup-miss-type ( l2-mcast | ip4-mcast | ip6-mcast ) port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_lookup_miss_type_l2_mcast_ip4_mcast_ip6_mcast_port_ports_all(
    cparser_context_t *context,
    uint32_t *index_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    diag_portlist_t fldPmsk;
    rtk_vlan_profile_t profile;
    char portStr[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(fldPmsk, 8), ret);

    if('l' == TOKEN_CHAR(6, 0))
    {
        profile.l2mc_unknFloodPm = fldPmsk.portmask;
    }
    else if('i' == TOKEN_CHAR(6, 0))
    {
        if('4' == TOKEN_CHAR(6, 2))
            profile.ipmc_unknFloodPm = fldPmsk.portmask;
        else if ('6' == TOKEN_CHAR(6, 2))
            profile.ip6mc_unknFloodPm = fldPmsk.portmask;
    }

    osal_memset(&portStr, 0, sizeof(portStr));
    diag_util_lPortMask2str(portStr, &fldPmsk.portmask);
    DIAG_UTIL_MPRINTF("%s bridge lookup miss flood portmask: %s\n", TOKEN_STR(6), portStr);
    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_lookup_miss_type_l2_mcast_ip4_mcast_ip6_mcast_port_ports_all */
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX_LOOKUP_MISS_TYPE_L2_MCAST_IP4_MCAST_IP6_MCAST_PORT
/*
 * vlan get profile entry <UINT:index> lookup-miss-type ( l2-mcast | ip4-mcast | ip6-mcast ) port
 */
cparser_result_t
cparser_cmd_vlan_get_profile_entry_index_lookup_miss_type_l2_mcast_ip4_mcast_ip6_mcast_port(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    diag_portlist_t fldPmsk;
    rtk_vlan_profile_t profile;
    char portStr[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if('l' == TOKEN_CHAR(6, 0))
    {
        fldPmsk.portmask = profile.l2mc_unknFloodPm;
    }
    else if('i' == TOKEN_CHAR(6, 0))
    {
        if('4' == TOKEN_CHAR(6, 2))
            fldPmsk.portmask = profile.ipmc_unknFloodPm;
        else if ('6' == TOKEN_CHAR(6, 2))
            fldPmsk.portmask = profile.ip6mc_unknFloodPm;
    }

    osal_memset(&portStr, 0, sizeof(portStr));
    diag_util_lPortMask2str(portStr, &fldPmsk.portmask);
    DIAG_UTIL_MPRINTF("%s bridge lookup miss flood portmask: %s\n", TOKEN_STR(6), portStr);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_profile_entry_index_lookup_miss_type_l2_mcast_ip4_mcast_ip6_mcast_port */
#endif

#ifdef CMD_VLAN_SET_PROFILE_ENTRY_INDEX_LOOKUP_MISS_TYPE_L2_MCAST_IP4_MCAST_IP6_MCAST_MCAST_TABLE_TABLE_INDEX
/*
 * vlan set profile entry <UINT:index> lookup-miss-type ( l2-mcast | ip4-mcast | ip6-mcast ) mcast-table <UINT:table_index>
 */
cparser_result_t
cparser_cmd_vlan_set_profile_entry_index_lookup_miss_type_l2_mcast_ip4_mcast_ip6_mcast_mcast_table_table_index(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *table_index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_profile_t profile;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

    if('l' == TOKEN_CHAR(6, 0))
    {
        profile.l2_mcast_dlf_pm_idx = *table_index_ptr;
    }
    else if('i' == TOKEN_CHAR(6, 0))
    {
        if('4' == TOKEN_CHAR(6, 2))
            profile.ip4_mcast_dlf_pm_idx = *table_index_ptr;
        else
            profile.ip6_mcast_dlf_pm_idx = *table_index_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_set(unit, *index_ptr, &profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_profile_entry_index_lookup_miss_type_l2_mcast_ip4_mcast_ip6_mcast_mcast_table_table_index */
#endif

#ifdef CMD_VLAN_SET_INGRESS_FILTER_PORT_PORTS_ALL_ACTION_FORWARD_DROP_TRAP
/*
 * vlan set ingress-filter port ( <PORT_LIST:ports> | all ) action ( forward | drop | trap )
 */
cparser_result_t cparser_cmd_vlan_set_ingress_filter_port_ports_all_action_forward_drop_trap(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlan_ifilter_t igr_filter;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if('f' == TOKEN_CHAR(6, 0))
        igr_filter = INGRESS_FILTER_FWD;
    else if('d' == TOKEN_CHAR(6, 0))
        igr_filter = INGRESS_FILTER_DROP;
    else
        igr_filter = INGRESS_FILTER_TRAP;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portIgrFilter_set(unit, port, igr_filter), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_PVID_MODE_INNER_OUTER_PORT_PORTS_ALL_ALL_UNTAG_ONLY_UNTAG_AND_PRIORITY_TAG
/*
 * vlan set pvid-mode ( inner | outer ) port ( <PORT_LIST:ports> | all ) ( all | untag-only | untag-and-priority-tag )
 */
cparser_result_t cparser_cmd_vlan_set_pvid_mode_inner_outer_port_ports_all_all_untag_only_untag_and_priority_tag(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlanType_t type;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlan_pbVlan_mode_t mode;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    if('i' == TOKEN_CHAR(3, 0))
        type = INNER_VLAN;
    else
        type = OUTER_VLAN;

    if('u' == TOKEN_CHAR(6, 0))
    {
        if('o' == TOKEN_CHAR(6, 6))
            mode = PBVLAN_MODE_UNTAG_ONLY;
        else
            mode = PBVLAN_MODE_UNTAG_AND_PRITAG;
    }
    else
        mode = PBVLAN_MODE_ALL_PKT;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if (type == INNER_VLAN)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portInnerPvidMode_set(unit, port, mode), ret);
            }
            else
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portOuterPvidMode_set(unit, port, mode), ret);
            }
        }
        else
#endif
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portPvidMode_set(unit, port, type, mode), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_MAC_ADDRESS_MAC_VID_VID_PRIORITY_PRIORITY
/*
 * vlan set mac-based-vlan entry <UINT:index> mac-address <MACADDR:mac> vid <UINT:vid> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_vlan_set_mac_based_vlan_entry_index_mac_address_mac_vid_vid_priority_priority(
    cparser_context_t *context,
    uint32_t *index_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  valid;
    rtk_mac_t smac;
    rtk_vlan_t vid;
    rtk_pri_t   priority;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlan_get(unit, *index_ptr, &valid, &smac, &vid, &priority) , ret);
    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlan_set(unit, *index_ptr, valid, (rtk_mac_t *)mac_ptr, *vid_ptr, *priority_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_MAC_ADDRESS_MAC_MAC_MASK_MSK_VID_VID_PRIORITY_PRIORITY
/*
 * vlan set mac-based-vlan entry <UINT:index> mac-address <MACADDR:mac> mac-mask <MACADDR:msk> vid <UINT:vid> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_vlan_set_mac_based_vlan_entry_index_mac_address_mac_mac_mask_msk_vid_vid_priority_priority(
    cparser_context_t *context,
    uint32_t *index_ptr,
    cparser_macaddr_t *mac_ptr,
    cparser_macaddr_t *msk_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  valid;
    rtk_mac_t smac;
    rtk_mac_t msk;
    rtk_vlan_t vid;
    rtk_pri_t   priority;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanWithMsk_get(unit, *index_ptr, &valid, &smac, &msk, &vid, &priority) , ret);
    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanWithMsk_set(unit, *index_ptr, valid, (rtk_mac_t *)mac_ptr, (rtk_mac_t *)msk_ptr, *vid_ptr, *priority_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_MAC_ADDRESS_MAC_MAC_MASK_MSK_PORT_PORT_PORT_MASK_PORT_MSK_VID_VID_PRIORITY_PRIORITY
/*
 * vlan set mac-based-vlan entry <UINT:index> mac-address <MACADDR:mac> mac-mask <MACADDR:msk> { port <UINT:port> port-mask <UINT:port_msk> } vid <UINT:vid> priority <UINT:priority>
 */
cparser_result_t cparser_cmd_vlan_set_mac_based_vlan_entry_index_mac_address_mac_mac_mask_msk_port_port_port_mask_port_msk_vid_vid_priority_priority(cparser_context_t *context,
    uint32_t *index_ptr,
    cparser_macaddr_t *mac_ptr,
    cparser_macaddr_t *msk_ptr,
    uint32_t *port_ptr,
    uint32_t *port_msk_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  valid;
    rtk_mac_t smac;
    rtk_mac_t msk;
    rtk_vlan_t vid;
    rtk_pri_t   priority;
    rtk_port_t port;
    rtk_port_t port_mask;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanWithPort_get(unit, *index_ptr, &valid, &smac, &msk, &port, &port_mask, &vid, &priority) , ret);
    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanWithPort_set(unit, *index_ptr, valid, (rtk_mac_t *)mac_ptr, (rtk_mac_t *)msk_ptr, *port_ptr, *port_msk_ptr, *vid_ptr, *priority_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_STATE_DISABLE_ENABLE
/*
 * vlan set mac-based-vlan entry <UINT:index> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_mac_based_vlan_entry_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  val;
#if defined(CONFIG_SDK_RTL8390)
    uint32  valid;
    rtk_mac_t smac;
    rtk_vlan_t vid;
    rtk_pri_t   priority;
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_vlan_macVlanEntry_t macVlanEntry;
#endif

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6, 0))
        val = TRUE;
    else
        val = FALSE;

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlan_get(unit, *index_ptr, &valid, &smac, &vid, &priority) , ret);

        valid = val;
        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlan_set(unit, *index_ptr, valid, &smac, vid, priority), ret);
    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_get(unit, *index_ptr, &macVlanEntry) , ret);

        macVlanEntry.valid = val;
        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_set(unit, *index_ptr, &macVlanEntry) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * vlan set mac-based-vlan port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_mac_based_vlan_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_enable_t enable;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portMacBasedVlanEnable_set(unit, port, enable) , ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_KEY_PORT_PORT_STATE_DISABLE_ENABLE
/*
 * vlan set mac-based-vlan entry <UINT:index> key port <UINT:port> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_mac_based_vlan_entry_index_key_port_port_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *port_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_macVlanEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(9, 0))
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_get(unit, *index_ptr, &data) , ret);
        data.port_type = 0; /* individual port */
        data.port = *port_ptr;
        data.port_care = value;
        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_KEY_TRUNK_TRUNK_ID_STATE_DISABLE_ENABLE
/*
 * vlan set mac-based-vlan entry <UINT:index> key trunk <UINT:trunk_id> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_mac_based_vlan_entry_index_key_trunk_trunk_id_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *trunk_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_macVlanEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(9, 0))
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_get(unit, *index_ptr, &data) , ret);
        data.port_type = 1; /* trunk port */
        data.port = *trunk_id_ptr;
        data.port_care = value;
        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_KEY_INNER_OUTER_FRAME_TYPE_ALL_TAGGED_UNTAGGED
/*
 * vlan set mac-based-vlan entry <UINT:index> key ( inner | outer ) frame-type ( all |tagged |untagged )
 */
cparser_result_t cparser_cmd_vlan_set_mac_based_vlan_entry_index_key_inner_outer_frame_type_all_tagged_untagged(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_macVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_get(unit,*index_ptr, &entry),ret);

    if('i' == TOKEN_CHAR(6,0))
    {
        switch(TOKEN_CHAR(8,0))
        {
            case 'a':
                entry.ivid_aft = ACCEPT_FRAME_TYPE_ALL;
                break;
            case 't':
                entry.ivid_aft = ACCEPT_FRAME_TYPE_TAG_ONLY;
                break;
            case 'u':
                entry.ivid_aft = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
    else
    {
        switch(TOKEN_CHAR(8,0))
        {
            case 'a':
                entry.ovid_aft = ACCEPT_FRAME_TYPE_ALL;
                break;
            case 't':
                entry.ovid_aft = ACCEPT_FRAME_TYPE_TAG_ONLY;
                break;
            case 'u':
                entry.ovid_aft = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_set(unit, *index_ptr, &entry) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_KEY_MAC_ADDRESS_MAC_MAC_MASK_MSK
/*
 * vlan set mac-based-vlan entry <UINT:index> key mac-address <MACADDR:mac> mac-mask <MACADDR:msk>
 */
cparser_result_t cparser_cmd_vlan_set_mac_based_vlan_entry_index_key_mac_address_mac_mac_mask_msk(cparser_context_t *context,
    uint32_t *index_ptr,
    cparser_macaddr_t *mac_ptr,
    cparser_macaddr_t *msk_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_macVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_get(unit,*index_ptr, &entry),ret);

    osal_memcpy(&entry.mac,(rtk_mac_t *)mac_ptr,sizeof(rtk_mac_t));
    osal_memcpy(&entry.mac_care,(rtk_mac_t *)msk_ptr,sizeof(rtk_mac_t));

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_set(unit,*index_ptr, &entry),ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_DATA_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_BYPASS_INGRESS_VLAN_FILTER
/*
 * vlan set mac-based-vlan entry <UINT:index> data fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { bypass-ingress-vlan-filter }
 */
cparser_result_t cparser_cmd_vlan_set_mac_based_vlan_entry_index_data_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_bypass_ingress_vlan_filter(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_macVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_get(unit, *index_ptr, &entry), ret);

    switch(TOKEN_CHAR(7,0))
    {
        case 'd':
            entry.fwd_action = ACTION_DROP;
            break;
        case 'f':
            entry.fwd_action = ACTION_FORWARD;
            break;
        case 't':
            entry.fwd_action = ACTION_TRAP2CPU;
            break;
        case 'c':
            entry.fwd_action = ACTION_COPY2CPU;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if('b' == TOKEN_CHAR(8,0))
        entry.igrvlanfilter_ignore = ENABLED;
    else
        entry.igrvlanfilter_ignore = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_DATA_FWD_ACTION_TRAP_TO_MASTER_COPY_TO_MASTER_BYPASS_INGRESS_VLAN_FILTER
/*
 * vlan set mac-based-vlan entry <UINT:index> data fwd-action ( trap-to-master | copy-to-master ) { bypass-ingress-vlan-filter }
 */
cparser_result_t cparser_cmd_vlan_set_mac_based_vlan_entry_index_data_fwd_action_trap_to_master_copy_to_master_bypass_ingress_vlan_filter(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_macVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_get(unit, *index_ptr, &entry), ret);

    switch(TOKEN_CHAR(7,0))
    {
        case 't':
            entry.fwd_action = ACTION_TRAP2MASTERCPU;
            break;
        case 'c':
            entry.fwd_action = ACTION_COPY2MASTERCPU;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if('b' == TOKEN_CHAR(8,0))
        entry.igrvlanfilter_ignore = ENABLED;
    else
        entry.igrvlanfilter_ignore = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_DATA_VLAN_TYPE_INNER_OUTER_VID_VID_STATE_DISABLE_ENABLE_PRIORITY_PRIORITY_STATE_DISABLE_ENABLE_TPID_TPID_INDEX_STATE_DISABLE_ENABLE
/*
 * vlan set mac-based-vlan entry <UINT:index> data vlan-type ( inner | outer ) vid <UINT:vid> state (disable | enable ) priority <UINT:priority> state (disable | enable) tpid <UINT:tpid_index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_mac_based_vlan_entry_index_data_vlan_type_inner_outer_vid_vid_state_disable_enable_priority_priority_state_disable_enable_tpid_tpid_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr,
    uint32_t *tpid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_macVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_get(unit, *index_ptr, &entry), ret);

    if('i' == TOKEN_CHAR(7,0))
        entry.vlan_type = INNER_VLAN;
    else
        entry.vlan_type = OUTER_VLAN;

    if('e' == TOKEN_CHAR(11,0))
        entry.vid_assign = ENABLED;
    else
        entry.vid_assign = DISABLED;

    if('e' == TOKEN_CHAR(15,0))
        entry.pri_assign = ENABLED;
    else
        entry.pri_assign = DISABLED;

    if('e' == TOKEN_CHAR(19,0))
        entry.tpid_action = VLAN_TAG_TPID_ACT_ASSIGN;
    else
        entry.tpid_action = VLAN_TAG_TPID_ACT_NONE;

    entry.vid_new = *vid_ptr;
    entry.pri_new = *priority_ptr;
    entry.tpid_idx = *tpid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_MAC_BASED_VLAN_ENTRY_INDEX_DATA_VLAN_TAG_STATUS_UNTAG_TAG_NONE
/*
 * vlan set mac-based-vlan entry <UINT:index> data vlan-tag-status ( untag | tag | none )
 */
cparser_result_t
cparser_cmd_vlan_set_mac_based_vlan_entry_index_data_vlan_tag_status_untag_tag_none(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_macVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_get(unit, *index_ptr, &entry), ret);

    if ('u' == TOKEN_CHAR(7,0))
        entry.sts_action = VLAN_TAG_STATUS_ACT_UNTAGGED;
    else if ('t' == TOKEN_CHAR(7,0))
        entry.sts_action = VLAN_TAG_STATUS_ACT_TAGGED;
    else
        entry.sts_action = VLAN_TAG_STATUS_ACT_NONE;

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_ADD_MAC_BASED_VLAN_ENTRY_MAC_ADDRESS_MAC_VID_VID_PRIORITY_PRIORITY
/*
 * vlan add mac-based-vlan entry mac-address <MACADDR:mac> vid <UINT:vid> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_vlan_add_mac_based_vlan_entry_mac_address_mac_vid_vid_priority_priority(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_macBasedEntry_t entry;

    osal_memset(&entry, 0, sizeof(rtk_vlan_macBasedEntry_t));

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARAM_CHK();

    osal_memcpy(&entry.mac,(rtk_mac_t *)mac_ptr,sizeof(rtk_mac_t));
    entry.vid_new = *vid_ptr;
    entry.pri_new = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_add(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_add_mac_based_vlan_entry_mac_address_mac_vid_vid_priority_priority */
#endif

#ifdef CMD_VLAN_DEL_MAC_BASED_VLAN_ENTRY_MAC_ADDRESS_MAC
/*
 * vlan del mac-based-vlan entry mac-address <MACADDR:mac> */
cparser_result_t
cparser_cmd_vlan_del_mac_based_vlan_entry_mac_address_mac(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_macBasedEntry_t entry;

    osal_memset(&entry, 0, sizeof(rtk_vlan_macBasedEntry_t));

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARAM_CHK();

    osal_memcpy(&entry.mac,(rtk_mac_t *)mac_ptr,sizeof(rtk_mac_t));

    DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_del(unit, &entry), ret);

    return CPARSER_OK;

}   /* end of cparser_cmd_vlan_del_mac_based_vlan_entry_mac_address_mac */
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_SRC_IP_SIP_SRC_IP_MASK_SIPMASK_VID_VID_PRIORITY_PRIORITY
/*
 * vlan set ip-subnet-based-vlan entry <UINT:index> src-ip <IPV4ADDR:sip> src-ip-mask <IPV4ADDR:sipmask> vid <UINT:vid> priority <UINT:priority>
 */
cparser_result_t cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_src_ip_sip_src_ip_mask_sipmask_vid_vid_priority_priority(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *sip_ptr,
    uint32_t *sipmask_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  valid;
    ipaddr_t sip, sip_msk;
    rtk_vlan_t vid;
    rtk_pri_t  priority;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlan_get(unit, *index_ptr, &valid, &sip, &sip_msk, &vid, &priority) , ret);
    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlan_set(unit, *index_ptr, valid, *sip_ptr, *sipmask_ptr, *vid_ptr, *priority_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * vlan set ip-subnet-based-vlan port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_ip_subnet_based_vlan_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_enable_t enable;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portIpSubnetBasedVlanEnable_set(unit, port, enable) , ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_SRC_IP_SIP_SRC_IP_MASK_SIPMASK_PORT_PORT_PORT_MASK_PORT_MSK_VID_VID_PRIORITY_PRIORITY
/*
 * vlan set ip-subnet-based-vlan entry <UINT:index> src-ip <IPV4ADDR:sip> src-ip-mask <IPV4ADDR:sipmask> port <UINT:port> port-mask <UINT:port_msk> vid <UINT:vid> priority <UINT:priority>
 */
cparser_result_t cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_src_ip_sip_src_ip_mask_sipmask_port_port_port_mask_port_msk_vid_vid_priority_priority(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *sip_ptr,
    uint32_t *sipmask_ptr,
    uint32_t *port_ptr,
    uint32_t *port_msk_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  valid;
    ipaddr_t sip, sip_msk;
    rtk_vlan_t vid;
    rtk_pri_t  priority;
    rtk_port_t port, port_msk;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanWithPort_get(unit, *index_ptr, &valid, &sip, &sip_msk, &port, &port_msk, &vid, &priority) , ret);
    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanWithPort_set(unit, *index_ptr, valid, *sip_ptr, *sipmask_ptr, *port_ptr, *port_msk_ptr, *vid_ptr, *priority_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_STATE_DISABLE_ENABLE
/*
 * vlan set ip-subnet-based-vlan entry <UINT:index> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  val;
#if defined (CONFIG_SDK_RTL8390)
    uint32 valid;
    ipaddr_t sip, sip_msk;
    rtk_vlan_t vid;
    rtk_pri_t  priority;
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_vlan_ipSubnetVlanEntry_t ipSubnetVlanEntry;
#endif

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6, 0))
        val = TRUE;
    else
        val = FALSE;

#if defined (CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlan_get(unit, *index_ptr, &valid, &sip, &sip_msk, &vid, &priority) , ret);
        valid = val;
        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlan_set(unit, *index_ptr, valid, sip, sip_msk, vid, priority), ret);
    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_get(unit, *index_ptr, &ipSubnetVlanEntry) , ret);
        ipSubnetVlanEntry.valid = val;
        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_set(unit, *index_ptr, &ipSubnetVlanEntry) , ret);
    }
#endif

   return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_KEY_PORT_PORT_STATE_DISABLE_ENABLE
/*
 * vlan set ip-subnet-based-vlan entry <UINT:index> key port <UINT:port> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_key_port_port_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *port_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_ipSubnetVlanEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(9, 0))
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_get(unit, *index_ptr, &data) , ret);
        data.port_type = 0; /* individual port */
        data.port = *port_ptr;
        data.port_care = value;
        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_KEY_TRUNK_TRUNK_ID_STATE_DISABLE_ENABLE
/*
 * vlan set ip-subnet-based-vlan entry <UINT:index> key trunk <UINT:trunk_id> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_key_trunk_trunk_id_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *trunk_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_ipSubnetVlanEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(9, 0))
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_get(unit, *index_ptr, &data) , ret);
        data.port_type = 1; /* trunk port */
        data.port = *trunk_id_ptr;
        data.port_care = value;
        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_KEY_SRC_IP_SIP_SRC_IP_MASK_SIPMSK
/*
 * vlan set ip-subnet-based-vlan entry <UINT:index> key src-ip <IPV4ADDR:sip> src-ip-mask <IPV4ADDR:sipmsk>
 */
cparser_result_t cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_key_src_ip_sip_src_ip_mask_sipmsk(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *sip_ptr,
    uint32_t *sipmsk_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_ipSubnetVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_get(unit, *index_ptr, &entry),ret);

    entry.sip = *sip_ptr;
    entry.sip_care = *sipmsk_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_set(unit, *index_ptr, &entry),ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_KEY_INNER_OUTER_FRAME_TYPE_ALL_TAGGED_UNTAGGED
/*
 * vlan set ip-subnet-based-vlan entry  <UINT:index> key ( inner | outer ) frame-type ( all | tagged | untagged )
 */
cparser_result_t cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_key_inner_outer_frame_type_all_tagged_untagged(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_ipSubnetVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_get(unit, *index_ptr, &entry),ret);

    if('i' == TOKEN_CHAR(6,0))
    {
        switch(TOKEN_CHAR(8,0))
        {
            case 'a':
                entry.ivid_aft = ACCEPT_FRAME_TYPE_ALL;
                break;
            case 't':
                entry.ivid_aft = ACCEPT_FRAME_TYPE_TAG_ONLY;
                break;
            case 'u':
                entry.ivid_aft = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
    else
    {
        switch(TOKEN_CHAR(8,0))
        {
            case 'a':
                entry.ovid_aft = ACCEPT_FRAME_TYPE_ALL;
                break;
            case 't':
                entry.ovid_aft = ACCEPT_FRAME_TYPE_TAG_ONLY;
                break;
            case 'u':
                entry.ovid_aft = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
                break;
            default:
                return RT_ERR_FAILED;
        }

    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_set(unit, *index_ptr, &entry) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_DATA_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_BYPASS_INGRESS_VLAN_FILTER
/*
 * vlan set ip-subnet-based-vlan entry <UINT:index> data fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { bypass-ingress-vlan-filter }
 */
cparser_result_t cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_data_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_bypass_ingress_vlan_filter(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_ipSubnetVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_get(unit, *index_ptr, &entry), ret);

    switch(TOKEN_CHAR(7,0))
    {
        case 'd':
            entry.fwd_action = ACTION_DROP;
            break;
        case 'f':
            entry.fwd_action = ACTION_FORWARD;
            break;
        case 't':
            entry.fwd_action = ACTION_TRAP2CPU;
            break;
        case 'c':
            entry.fwd_action = ACTION_COPY2CPU;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if('b' == TOKEN_CHAR(8,0))
        entry.igrvlanfilter_ignore = ENABLED;
    else
        entry.igrvlanfilter_ignore = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_DATA_FWD_ACTION_TRAP_TO_MASTER_COPY_TO_MASTER_BYPASS_INGRESS_VLAN_FILTER
/*
 * vlan set ip-subnet-based-vlan entry <UINT:index> data fwd-action ( trap-to-master | copy-to-master ) { bypass-ingress-vlan-filter }
 */
cparser_result_t cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_data_fwd_action_trap_to_master_copy_to_master_bypass_ingress_vlan_filter(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_ipSubnetVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_get(unit, *index_ptr, &entry), ret);

    switch(TOKEN_CHAR(7,0))
    {
        case 't':
            entry.fwd_action = ACTION_TRAP2MASTERCPU;
            break;
        case 'c':
            entry.fwd_action = ACTION_COPY2MASTERCPU;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if('b' == TOKEN_CHAR(8,0))
        entry.igrvlanfilter_ignore = ENABLED;
    else
        entry.igrvlanfilter_ignore = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_DATA_VLAN_TYPE_INNER_OUTER_VID_VID_STATE_DISABLE_ENABLE_PRIORITY_PRIORITY_STATE_DISABLE_ENABLE_TPID_TPID_INDEX_STATE_DISABLE_ENABLE
/*
 * vlan set ip-subnet-based-vlan entry <UINT:index> data vlan-type ( inner | outer ) vid <UINT:vid> state ( disable | enable ) priority <UINT:priority> state ( disable | enable ) tpid <UINT:tpid_index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_data_vlan_type_inner_outer_vid_vid_state_disable_enable_priority_priority_state_disable_enable_tpid_tpid_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr,
    uint32_t *tpid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_ipSubnetVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_get(unit, *index_ptr, &entry), ret);

    if('i' == TOKEN_CHAR(7,0))
        entry.vlan_type = INNER_VLAN;
    else
        entry.vlan_type = OUTER_VLAN;

    if('e' == TOKEN_CHAR(11,0))
        entry.vid_assign = ENABLED;
    else
        entry.vid_assign = DISABLED;

    if('e' == TOKEN_CHAR(15,0))
        entry.pri_assign = ENABLED;
    else
        entry.pri_assign = DISABLED;

    if('e' == TOKEN_CHAR(19,0))
        entry.tpid_action = VLAN_TAG_TPID_ACT_ASSIGN;
    else
        entry.tpid_action = VLAN_TAG_TPID_ACT_NONE;

    entry.vid_new = *vid_ptr;
    entry.pri_new = *priority_ptr;
    entry.tpid_idx = *tpid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX_DATA_VLAN_TAG_STATUS_UNTAG_TAG_NONE
/*
 * vlan set ip-subnet-based-vlan entry <UINT:index> data vlan-tag-status ( untag | tag | none )
 */
cparser_result_t
cparser_cmd_vlan_set_ip_subnet_based_vlan_entry_index_data_vlan_tag_status_untag_tag_none(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_ipSubnetVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_get(unit, *index_ptr, &entry), ret);

    if ('u' == TOKEN_CHAR(7,0))
        entry.sts_action = VLAN_TAG_STATUS_ACT_UNTAGGED;
    else if ('t' == TOKEN_CHAR(7,0))
        entry.sts_action = VLAN_TAG_STATUS_ACT_TAGGED;
    else
        entry.sts_action = VLAN_TAG_STATUS_ACT_NONE;

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_ADD_IP_SUBNET_BASED_VLAN_ENTRY_SRC_IP_SIP_SRC_IP_MASK_SIPMASK_VID_VID_PRIORITY_PRIORITY
/*
 * vlan add ip-subnet-based-vlan entry src-ip <IPV4ADDR:sip> src-ip-mask <IPV4ADDR:sipmask> vid <UINT:vid> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_vlan_add_ip_subnet_based_vlan_entry_src_ip_sip_src_ip_mask_sipmask_vid_vid_priority_priority(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *sipmask_ptr,
    uint32_t *vid_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_ipSubnetVlanEntry_t entry;

    osal_memset(&entry, 0, sizeof(rtk_vlan_ipSubnetVlanEntry_t));

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    entry.sip = *sip_ptr;
    entry.sip_care = *sipmask_ptr;
    entry.vid_assign = ENABLED;
    entry.pri_assign = ENABLED;
    entry.vid_new = *vid_ptr;
    entry.pri_new = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_add(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_add_ip_subnet_based_vlan_entry_src_ip_sip_src_ip_mask_sipmask_vid_vid_priority_priority */
#endif

#ifdef CMD_VLAN_DEL_IP_SUBNET_BASED_VLAN_ENTRY_SRC_IP_SIP_SRC_IP_MASK_SIPMASK
/*
 * vlan del ip-subnet-based-vlan entry src-ip <IPV4ADDR:sip> src-ip-mask <IPV4ADDR:sipmask>
 */
cparser_result_t
cparser_cmd_vlan_del_ip_subnet_based_vlan_entry_src_ip_sip_src_ip_mask_sipmask(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *sipmask_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_ipSubnetVlanEntry_t entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    entry.sip = *sip_ptr;
    entry.sip_care = *sipmask_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_del_ip_subnet_based_vlan_entry_src_ip_sip_src_ip_mask_sipmask */
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_BLOCK_MODE_INDEX_CONVERSION_MAC_BASED_IP_SUBNET_BASED
/*
 * vlan set vlan-conversion ingress block-mode <UINT:index> ( conversion | mac-based | ip-subnet-based )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_block_mode_index_conversion_mac_based_ip_subnet_based(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtBlk_mode_t mode;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('c' == TOKEN_CHAR(6, 0))
    {
        mode = CONVERSION_MODE_C2SC;
    }
    else if('m' == TOKEN_CHAR(6, 0))
    {
        mode = CONVERSION_MODE_MAC_BASED;
    }
    else
    {
        mode = CONVERSION_MODE_IP_SUBNET_BASED;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtBlkMode_set(unit, *index_ptr, mode) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  valid;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(7, 0))
    {
        valid = 1;
    }
    else
    {
        valid = 0;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.valid = valid;
    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_KEY_VID_VID_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> key vid <UINT:vid> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_key_vid_vid_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(10, 0))
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    data.vid = *vid_ptr;
    data.vid_ignore = value;

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_KEY_INNER_OUTER_VID_VID_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> key ( inner | outer ) vid <UINT:vid> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_key_inner_outer_vid_vid_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('i' == TOKEN_CHAR(7, 0))
    {
        if('e' == TOKEN_CHAR(11, 0))
        {
            data.match_key.inner_vid = *vid_ptr;
            data.match_key.inner_vid_ignore = DISABLED;
        }
        else
        {
            data.match_key.inner_vid = *vid_ptr;
            data.match_key.inner_vid_ignore = ENABLED;
        }
    }
    else
    {
        if('e' == TOKEN_CHAR(11, 0))
        {
            data.match_key.outer_vid = *vid_ptr;
            data.match_key.outer_vid_ignore = DISABLED;
        }
        else
        {
            data.match_key.outer_vid = *vid_ptr;
            data.match_key.outer_vid_ignore = ENABLED;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_KEY_RANGE_CHECK_RANGE_CHECK_BITMASK_MASK_RANGE_CHECK_MASK
/*
 * vlan set vlan-conversion ingress entry <UINT:index> key range-check <HEX:range_check_bitmask> mask <HEX:range_check_mask>
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_key_range_check_range_check_bitmask_mask_range_check_mask(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *range_check_bitmask_ptr,
    uint32_t *range_check_mask_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

        data.rngchk_result = *range_check_bitmask_ptr;
        data.rngchk_result_mask = *range_check_mask_ptr;
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

        data.match_key.rngchk_result = *range_check_bitmask_ptr;
        data.match_key.rngchk_result_mask = *range_check_mask_ptr;
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_KEY_PRIORITY_PRIORITY_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> key priority <UINT:priority> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_key_priority_priority_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(10, 0))
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.priority = *vid_ptr;
    data.priority_ignore = value;
    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_KEY_INNER_OUTER_PRIORITY_PRIORITY_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> key (inner | outer)  priority <UINT:priority> state (disable | enable)
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_key_inner_outer_priority_priority_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *pri_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('i' == TOKEN_CHAR(7, 0))
    {
        if('e' == TOKEN_CHAR(11, 0))
        {
            data.match_key.inner_priority = *pri_ptr;
            data.match_key.inner_priority_ignore = DISABLED;
        }
        else
        {
            data.match_key.inner_priority = *pri_ptr;
            data.match_key.inner_priority_ignore = ENABLED;
        }
    }
    else
    {
        if('e' == TOKEN_CHAR(11, 0))
        {
            data.match_key.outer_priority = *pri_ptr;
            data.match_key.outer_priority_ignore = DISABLED;
        }
        else
        {
            data.match_key.outer_priority = *pri_ptr;
            data.match_key.outer_priority_ignore = ENABLED;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_KEY_INNER_OUTER_TAG_STATUS_ALL_TAGGED_UNTAGGED
/*
 * vlan set vlan-conversion ingress entry <UINT:index> key ( inner | outer ) tag-status ( all | tagged | untagged )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_key_inner_outer_tag_status_all_tagged_untagged(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('i' == TOKEN_CHAR(7, 0))
    {
        switch(TOKEN_CHAR(9, 0))
        {
            case 'a':
                data.match_key.inner_tag_sts = VLAN_CNVT_TAG_STS_ALL;
                break;
            case 't':
                data.match_key.inner_tag_sts = VLAN_CNVT_TAG_STS_TAGGED;
                break;
            case 'u':
                data.match_key.inner_tag_sts = VLAN_CNVT_TAG_STS_UNTAGGED;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
    else
    {
        switch(TOKEN_CHAR(9, 0))
        {
            case 'a':
                data.match_key.outer_tag_sts = VLAN_CNVT_TAG_STS_ALL;
                break;
            case 't':
                data.match_key.outer_tag_sts = VLAN_CNVT_TAG_STS_TAGGED;
                break;
            case 'u':
                data.match_key.outer_tag_sts = VLAN_CNVT_TAG_STS_UNTAGGED;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_KEY_PORT_PORT_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> key port <UINT:port> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_key_port_port_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *port_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(10, 0))
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
        data.port = *port_ptr;
        data.port_ignore = value;
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
        data.match_key.port_type = 0;   /* individual port */
        data.match_key.port = *port_ptr;
        data.match_key.port_ignore = value;
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_KEY_TRUNK_TRUNK_ID_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> key trunk <UINT:trunk_id> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_key_trunk_trunk_id_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *trunk_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(10, 0))
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
        data.match_key.port_type = 1;   /* trunk port */
        data.match_key.port = *trunk_id_ptr;
        data.match_key.port_ignore = value;
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_VID_SELECT_INNER_OUTER
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data vid-select ( inner | outer )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_vid_select_inner_outer(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(8, 0))
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.vid_cnvt_sel = value;
    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_VID_VID_FORCE_SHIFT_POSITIVE_SHIFT_NEGATIVE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data vid <UINT:vid> ( force | shift-positive | shift-negative )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_vid_vid_force_shift_positive_shift_negative(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    uint32  shift_sel = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('f' == TOKEN_CHAR(9, 0))
    {
        value = 0;
    }
    else if (0 == osal_strcmp(TOKEN_STR(9), "shift-positive"))
    {
        value = 1;
    }
    else
    {
        value = 1;
        shift_sel = 1;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.vid_new  = *vid_ptr;
    data.vid_shift_sel = shift_sel;
    data.vid_shift = value;
    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_INNER_VID_VID_FORCE_SHIFT_COPY_FROM_OUTER_NONE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data inner_vid  <UINT:vid> ( force | shift | copy-from-outer | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_inner_vid_vid_force_shift_copy_from_outer_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    switch(TOKEN_CHAR(9, 0))
    {
        case 'f':
            data.action.inner_vid_action = VLAN_TAG_VID_ACT_ASSIGN;
            break;
        case 's':
            data.action.inner_vid_action = VLAN_TAG_VID_ACT_SHIFT;
            break;
        case 'c':
            data.action.inner_vid_action = VLAN_TAG_VID_ACT_COPY_FROM_OUTER;
            break;
        case 'n':
            data.action.inner_vid_action = VLAN_TAG_VID_ACT_NONE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    data.action.inner_vid_value = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_OUTER_VID_VID_FORCE_SHIFT_COPY_FROM_INNER_NONE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data outer_vid <UINT:vid> ( force | shift | copy-from-inner | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_outer_vid_vid_force_shift_copy_from_inner_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    switch(TOKEN_CHAR(9, 0))
    {
        case 'f':
            data.action.outer_vid_action = VLAN_TAG_VID_ACT_ASSIGN;
            break;
        case 's':
            data.action.outer_vid_action = VLAN_TAG_VID_ACT_SHIFT;
            break;
        case 'c':
            data.action.outer_vid_action = VLAN_TAG_VID_ACT_COPY_FROM_INNER;
            break;
        case 'n':
            data.action.outer_vid_action = VLAN_TAG_VID_ACT_NONE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    data.action.outer_vid_value = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_PRIORITY_PRIORITY_FORCE_NONE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data priority <UINT:priority> ( force | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_priority_priority_force_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('f' == TOKEN_CHAR(9, 0))
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.pri_new = *priority_ptr;
    data.pri_assign = value;
    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_INNER_PRIORITY_PRIORITY_FORCE_NONE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data inner-priority <UINT:priority> ( force | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_inner_priority_priority_force_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);


    if('f' == TOKEN_CHAR(9, 0))
        data.action.inner_priority_action = VLAN_TAG_PRIORITY_ACT_ASSIGN;
    else
        data.action.inner_priority_action = VLAN_TAG_PRIORITY_ACT_NONE;

    data.action.inner_priority = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_OUTER_PRIORITY_PRIORITY_FORCE_COPY_FROM_INNER_NONE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data outer-priority <UINT:priority> ( force | copy-from-inner | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_outer_priority_priority_force_copy_from_inner_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    switch(TOKEN_CHAR(9, 0))
    {
        case 'f':
            data.action.outer_priority_action = VLAN_TAG_PRIORITY_ACT_ASSIGN;
            break;
        case 'c':
            data.action.outer_priority_action = VLAN_TAG_PRIORITY_ACT_COPY_FROM_INNER;
            break;
        case 'n':
            data.action.outer_priority_action = VLAN_TAG_PRIORITY_ACT_NONE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    data.action.outer_priority = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_STATUS_INNER_OUTER_UNTAG_TAG_NONE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data status ( inner | outer ) ( untag | tag | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_status_inner_outer_untag_tag_none(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  status;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('u' == TOKEN_CHAR(9, 0))
    {
        status = 0;
    }
    else if('t' == TOKEN_CHAR(9, 0))
    {
        status = 1;
    }
    else
    {
        status = 2;
    }

#if defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if('i' == TOKEN_CHAR(8, 0))
        {
            data.inner_tag_sts = status;
        }
        else
        {
            data.outer_tag_sts = status;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if('i' == TOKEN_CHAR(8, 0))
        {
            data.action.inner_tag_sts = status;
        }
        else
        {
            data.action.outer_tag_sts = status;
        }
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_TPID_TPID_INDEX_FORCE_NONE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data tpid <UINT:tpid_index> ( force | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_tpid_tpid_index_force_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tpid_index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('f' == TOKEN_CHAR(9, 0))
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    data.tpid_assign = value;
    data.tpid_idx = *tpid_index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_INNER_OUTER_TPID_TPID_INDEX_FORCE_NONE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data ( inner | outer ) tpid <UINT:tpid_index> ( force | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_inner_outer_tpid_tpid_index_force_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tpid_index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('f' == TOKEN_CHAR(10, 0))
    {
        if('i' == TOKEN_CHAR(7, 0))
        {
            data.action.inner_tpid_action = VLAN_TAG_TPID_ACT_ASSIGN;
            data.action.inner_tpid_idx = *tpid_index_ptr;
        }
        else
        {
            data.action.outer_tpid_action = VLAN_TAG_TPID_ACT_ASSIGN;
            data.action.outer_tpid_idx = *tpid_index_ptr;
        }

    }
    else
    {
        if('i' == TOKEN_CHAR(7, 0))
        {
            data.action.inner_tpid_action = VLAN_TAG_TPID_ACT_NONE;
            data.action.inner_tpid_idx = *tpid_index_ptr;
        }
        else
        {
            data.action.outer_tpid_action = VLAN_TAG_TPID_ACT_NONE;
            data.action.outer_tpid_idx = *tpid_index_ptr;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_DATA_INTF_INTF_ID_FORCE_NONE
/*
 * vlan set vlan-conversion ingress entry <UINT:index> data intf <UINT:intf_id> ( force | none )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_conversion_ingress_entry_index_data_intf_intf_id_force_none(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('f' == TOKEN_CHAR(9, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.action.intf_id_assign = enable;
    data.action.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion ingress port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_conversion_ingress_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    diag_portlist_t  portmask;
    rtk_enable_t enable;
    int32   port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portIgrVlanCnvtEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_conversion_ingress_port_ports_all_state_disable_enable */
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_INGRESS_PORT_PORTS_ALL
/*
 * vlan get vlan-conversion ingress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_conversion_ingress_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    diag_portlist_t  portmask;
    rtk_enable_t enable;
    int32   port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("Port configuration of VLAN ingress conversion:\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_MPRINTF("Port %u status\t: ", port);
        if((ret = rtk_vlan_portIgrVlanCnvtEnable_get(unit, port, &enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        DIAG_UTIL_MPRINTF("%s\n", ((enable == ENABLED) ? DIAG_STR_ENABLE:DIAG_STR_DISABLE));
    }


    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_conversion_ingress_port_ports_all */
#endif

#ifdef CMD_VLAN_SET_EGRESS_PORT_PORTS_ALL_INNER_OUTER_STATUS_UNTAG_PRIORITY_TAG_TAG
/*
 * vlan set egress port ( <PORT_LIST:ports> | all ) ( inner | outer ) status ( untag | priority-tag | tag )
 */
cparser_result_t cparser_cmd_vlan_set_egress_port_ports_all_inner_outer_status_untag_priority_tag_tag(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlanType_t type;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlan_tagSts_t sts;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        type = INNER_VLAN;
    }
    else
    {
        type = OUTER_VLAN;
    }

    if ('u' == TOKEN_CHAR(7, 0))
    {
        sts = TAG_STATUS_UNTAG;
    }
    else if('p' == TOKEN_CHAR(7, 0))
    {
        sts = TAG_STATUS_PRIORITY_TAGGED;
    }
    else
    {
        sts = TAG_STATUS_TAGGED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if (type == INNER_VLAN)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portEgrInnerTagSts_set(unit, port, sts), ret);
            }
            else if (type == OUTER_VLAN)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portEgrOuterTagSts_set(unit, port, sts), ret);
            }
        }
        else
#endif
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portEgrTagSts_set(unit, port, type, sts), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_EGRESS_PORT_PORTS_ALL_INNER_OUTER_STATUS_INTERNAL
/*
 * vlan set egress port ( <PORT_LIST:ports> | all ) ( inner | outer ) status internal
 */
cparser_result_t cparser_cmd_vlan_set_egress_port_ports_all_inner_outer_status_internal(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlanType_t type;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        type = INNER_VLAN;
    }
    else
    {
        type = OUTER_VLAN;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if (type == INNER_VLAN)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portEgrInnerTagSts_set(unit, port, TAG_STATUS_INTERNAL), ret);
            }
            else if (type == OUTER_VLAN)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portEgrOuterTagSts_set(unit, port, TAG_STATUS_INTERNAL), ret);
            }
        }
        else
#endif
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portEgrTagSts_set(unit, port, type, TAG_STATUS_INTERNAL), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_DOUBLE_TAG_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion egress double-tag state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_double_tag_state_disable_enable(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(6, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    if ((ret = rtk_vlan_egrVlanCnvtDblTagEnable_set(unit, enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_VID_SELECT_INNER_OUTER
/*
 * vlan set vlan-conversion egress vid-select ( inner | outer )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_vid_select_inner_outer(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_l2_vlanMode_t src;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('i' == TOKEN_CHAR(5, 0))
    {
        src = BASED_ON_INNER_VLAN;
    }
    else
    {
        src = BASED_ON_OUTER_VLAN;
    }

    if ((ret = rtk_vlan_egrVlanCnvtVidSource_set(unit, src)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion egress entry <UINT:index> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  valid;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(7, 0))
    {
        valid = 1;
    }
    else
    {
        valid = 0;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.valid = valid;
    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_KEY_VID_VID_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion egress entry <UINT:index> key vid <UINT:vid> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_key_vid_vid_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(10, 0))
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.vid = *vid_ptr;
    data.vid_ignore = value;
    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_KEY_INNER_OUTER_VID_VID_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion egress entry <UINT:index> key ( inner | outer ) vid <UINT:vid> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_key_inner_outer_vid_vid_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('i' == TOKEN_CHAR(7, 0))
    {
        if('e' == TOKEN_CHAR(11, 0))
        {
            data.match_key.inner_vid = *vid_ptr;
            data.match_key.inner_vid_ignore = DISABLED;
        }
        else
        {
            data.match_key.inner_vid = *vid_ptr;
            data.match_key.inner_vid_ignore = ENABLED;
        }

    }
    else
    {
        if('e' == TOKEN_CHAR(11, 0))
        {
            data.match_key.outer_vid = *vid_ptr;
            data.match_key.outer_vid_ignore = DISABLED;
        }
        else
        {
            data.match_key.outer_vid = *vid_ptr;
            data.match_key.outer_vid_ignore = ENABLED;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_KEY_RANGE_CHECK_RANGE_CHECK_BITMASK_MASK_RANGE_CHECK_MASK
/*
 * vlan set vlan-conversion egress entry <UINT:index> key range-check <HEX:range_check_bitmask> mask <HEX:range_check_mask>
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_key_range_check_range_check_bitmask_mask_range_check_mask(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *range_check_bitmask_ptr,
    uint32_t *range_check_mask_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_SDK_RTL8380)|| defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
          DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

        data.rngchk_result = *range_check_bitmask_ptr;
        data.rngchk_result_mask = *range_check_mask_ptr;
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

        data.match_key.rngchk_result = *range_check_bitmask_ptr;
        data.match_key.rngchk_result_mask = *range_check_mask_ptr;
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_KEY_PRIORITY_PRIORITY_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion egress entry <UINT:index> key priority <UINT:priority> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_key_priority_priority_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(10, 0))
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.orgpri = *priority_ptr;
    data.orgpri_ignore = value;
    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_KEY_INNER_OUTER_PRIORITY_PRIORITY_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion egress entry <UINT:index> key (inner | outer)  priority <UINT:priority> state (disable | enable)
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_key_inner_outer_priority_priority_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *pri_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('i' == TOKEN_CHAR(7, 0))
    {
        if('e' == TOKEN_CHAR(11, 0))
        {
            data.match_key.inner_priority = *pri_ptr;
            data.match_key.inner_priority_ignore = DISABLED;
        }
        else
        {
            data.match_key.inner_priority = *pri_ptr;
            data.match_key.inner_priority_ignore = ENABLED;
        }
    }
    else
    {
        if('e' == TOKEN_CHAR(11, 0))
        {
            data.match_key.outer_priority = *pri_ptr;
            data.match_key.outer_priority_ignore = DISABLED;
        }
        else
        {
            data.match_key.outer_priority = *pri_ptr;
            data.match_key.outer_priority_ignore = ENABLED;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_KEY_INNER_OUTER_TAG_STATUS_ALL_TAGGED_UNTAGGED
/*
 * vlan set vlan-conversion egress entry <UINT:index> key ( inner | outer ) tag-status ( all | tagged | untagged )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_key_inner_outer_tag_status_all_tagged_untagged(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('i' == TOKEN_CHAR(7, 0))
    {
        switch(TOKEN_CHAR(9, 0))
        {
            case 'a':
                data.match_key.inner_tag_sts = VLAN_CNVT_TAG_STS_ALL;
                break;
            case 't':
                data.match_key.inner_tag_sts = VLAN_CNVT_TAG_STS_TAGGED;
                break;
            case 'u':
                data.match_key.inner_tag_sts = VLAN_CNVT_TAG_STS_UNTAGGED;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
    else
    {
        switch(TOKEN_CHAR(9, 0))
        {
            case 'a':
                data.match_key.outer_tag_sts = VLAN_CNVT_TAG_STS_ALL;
                break;
            case 't':
                data.match_key.outer_tag_sts = VLAN_CNVT_TAG_STS_TAGGED;
                break;
            case 'u':
                data.match_key.outer_tag_sts = VLAN_CNVT_TAG_STS_UNTAGGED;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_KEY_PORT_PORT_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion egress entry <UINT:index> key port <UINT:port> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_key_port_port_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *port_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(10, 0))
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

#if defined(CONFIG_SDK_RTL8380)|| defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
          DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
        data.port = *port_ptr;
        data.port_ignore = value;
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
        data.match_key.port_type = 0;   /* individual port */
        data.match_key.port = *port_ptr;
        data.match_key.port_ignore = value;
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_KEY_TRUNK_TRUNK_ID_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion egress entry <UINT:index> key trunk <UINT:trunk_id> state ( disable | enable )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_key_trunk_trunk_id_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *trunk_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(10, 0))
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
        data.match_key.port_type = 1;   /* trunk port */
        data.match_key.port = *trunk_id_ptr;
        data.match_key.port_ignore = value;
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_DATA_VID_VID_FORCE_SHIFT_POSITIVE_SHIFT_NEGATIVE
/*
 * vlan set vlan-conversion egress entry <UINT:index> data vid <UINT:vid> ( force | shift-positive | shift-negative )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_data_vid_vid_force_shift_positive_shift_negative(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    uint32  shift_sel = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('f' == TOKEN_CHAR(9, 0))
    {
        value = 0;
    }
    else if (0 == osal_strcmp(TOKEN_STR(9), "shift-positive"))
    {
        value = 1;
    }
    else
    {
        value = 1;
        shift_sel = 1;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.vid_new  = *vid_ptr;
    data.vid_shift_sel = shift_sel;
    data.vid_shift = value;
    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_DATA_INNER_VID_VID_FORCE_SHIFT_COPY_FROM_OUTER_NONE
/*
 * vlan set vlan-conversion egress entry <UINT:index> data inner_vid <UINT:vid> ( force | shift | copy-from-outer | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_data_inner_vid_vid_force_shift_copy_from_outer_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    switch(TOKEN_CHAR(9, 0))
    {
        case 'f':
            data.action.inner_vid_action = VLAN_TAG_VID_ACT_ASSIGN;
            break;
        case 's':
            data.action.inner_vid_action = VLAN_TAG_VID_ACT_SHIFT;
            break;
        case 'c':
            data.action.inner_vid_action = VLAN_TAG_VID_ACT_COPY_FROM_OUTER;
            break;
        case 'n':
            data.action.inner_vid_action = VLAN_TAG_VID_ACT_NONE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    data.action.inner_vid_value = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_DATA_OUTER_VID_VID_FORCE_SHIFT_COPY_FROM_INNER_NONE
/*
 * vlan set vlan-conversion egress entry <UINT:index> data outer_vid <UINT:vid> ( force | shift | copy-from-inner | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_data_outer_vid_vid_force_shift_copy_from_inner_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    switch(TOKEN_CHAR(9, 0))
    {
        case 'f':
            data.action.outer_vid_action = VLAN_TAG_VID_ACT_ASSIGN;
            break;
        case 's':
            data.action.outer_vid_action = VLAN_TAG_VID_ACT_SHIFT;
            break;
        case 'c':
            data.action.outer_vid_action = VLAN_TAG_VID_ACT_COPY_FROM_INNER;
            break;
        case 'n':
            data.action.outer_vid_action = VLAN_TAG_VID_ACT_NONE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    data.action.outer_vid_value = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_DATA_PRIORITY_PRIORITY_FORCE_NONE
/*
 * vlan set vlan-conversion egress entry <UINT:index> data priority <UINT:priority> ( force | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_data_priority_priority_force_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('f' == TOKEN_CHAR(9, 0))
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    data.pri_new = *priority_ptr;
    data.pri_assign = value;
    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_DATA_INNER_PRIORITY_PRIORITY_FORCE_COPY_FROM_OUTER_NONE
/*
 * vlan set vlan-conversion egress entry <UINT:index> data inner-priority <UINT:priority> ( force | copy-from-outer | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_data_inner_priority_priority_force_copy_from_outer_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    switch(TOKEN_CHAR(9, 0))
    {
        case 'f':
            data.action.inner_priority_action = VLAN_TAG_PRIORITY_ACT_ASSIGN;
            break;
        case 'c':
            data.action.inner_priority_action = VLAN_TAG_PRIORITY_ACT_COPY_FROM_OUTER;
            break;
        case 'n':
            data.action.inner_priority_action = VLAN_TAG_PRIORITY_ACT_NONE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    data.action.inner_priority = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_DATA_OUTER_PRIORITY_PRIORITY_FORCE_NONE
/*
 * vlan set vlan-conversion egress entry <UINT:index> data outer-priority <UINT:priority> ( force | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_data_outer_priority_priority_force_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('f' == TOKEN_CHAR(9, 0))
        data.action.outer_priority_action = VLAN_TAG_PRIORITY_ACT_ASSIGN;
    else
        data.action.outer_priority_action = VLAN_TAG_PRIORITY_ACT_NONE;

    data.action.outer_priority = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_DATA_STATUS_INNER_OUTER_UNTAG_TAG_NONE
/*
 * vlan set vlan-conversion egress entry <UINT:index> data status ( inner | outer ) ( untag | tag | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_data_status_inner_outer_untag_tag_none(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_tagStatusAct_t status;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('u' == TOKEN_CHAR(9, 0))
    {
        status = 0;
    }
    else if('t' == TOKEN_CHAR(9, 0))
    {
        status = 1;
    }
    else
    {
        status = 2;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('i' == TOKEN_CHAR(8, 0))
    {
        data.action.inner_tag_sts = status;
    }
    else
    {
        data.action.outer_tag_sts = status;
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_DATA_TPID_TPID_INDEX_FORCE_NONE
/*
 * vlan set vlan-conversion egress entry <UINT:index> data tpid <UINT:tpid_index> ( force | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_data_tpid_tpid_index_force_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tpid_index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('f' == TOKEN_CHAR(9, 0))
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    data.itpid_assign = value;
    data.itpid_idx = *tpid_index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_DATA_INNER_OUTER_TPID_TPID_INDEX_FORCE_NONE
/*
 * vlan set vlan-conversion egress entry <UINT:index> data ( inner | outer ) tpid <UINT:tpid_index> ( force | none )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_entry_index_data_inner_outer_tpid_tpid_index_force_none(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tpid_index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);

    if('f' == TOKEN_CHAR(10, 0))
    {
        if('i' == TOKEN_CHAR(7, 0))
        {
            data.action.inner_tpid_action = VLAN_TAG_TPID_ACT_ASSIGN;
            data.action.inner_tpid_idx = *tpid_index_ptr;
        }
        else
        {
            data.action.outer_tpid_action = VLAN_TAG_TPID_ACT_ASSIGN;
            data.action.outer_tpid_idx = *tpid_index_ptr;
        }

    }
    else
    {
        if('i' == TOKEN_CHAR(7, 0))
        {
            data.action.inner_tpid_action = VLAN_TAG_TPID_ACT_NONE;
            data.action.inner_tpid_idx = *tpid_index_ptr;
        }
        else
        {
            data.action.outer_tpid_action = VLAN_TAG_TPID_ACT_NONE;
            data.action.outer_tpid_idx = *tpid_index_ptr;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &data) , ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion egress port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_conversion_egress_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t port;
    rtk_enable_t enable;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portEgrVlanCnvtEnable_set(unit, port, enable), ret);
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_conversion_egress_port_ports_all_state_disable_enable */
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_EGRESS_PORT_PORTS_ALL
/*
 * vlan get vlan-conversion egress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_conversion_egress_port_ports_all(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t port;
    rtk_enable_t enable;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("Port status of VLAN egress conversion:\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_MPRINTF("Port %d status: ",port);
        if((ret = rtk_vlan_portEgrVlanCnvtEnable_get(unit, port, &enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        DIAG_UTIL_MPRINTF("%s\n",((enable == ENABLED)? DIAG_STR_ENABLE:DIAG_STR_DISABLE));
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_conversion_egress_port_ports_all */
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_HIT_INDICATION_HIT_CLEAR
/*
 * vlan get vlan-conversion ingress entry <UINT:index> hit-indication { hit-clear }
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_conversion_ingress_entry_index_hit_indication_hit_clear(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    uint32  hit_clear = (TOKEN_NUM >= 8)? 1 : 0;
    uint32  hit_state;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtHitIndication_get(unit, *index_ptr, hit_clear, &hit_state), ret);
    DIAG_UTIL_MPRINTF(" Hit state : %d\n", hit_state);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_conversion_ingress_entry_index_hit_indication_hit_clear */
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_HIT_INDICATION_HIT_CLEAR
/*
 * vlan get vlan-conversion egress entry <UINT:index> hit-indication { hit-clear }
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_conversion_egress_entry_index_hit_indication_hit_clear(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    uint32  hit_clear = (TOKEN_NUM >= 8)? 1 : 0;
    uint32  hit_state;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtHitIndication_get(unit, *index_ptr, hit_clear, &hit_state), ret);
    DIAG_UTIL_MPRINTF(" Hit state : %d\n", hit_state);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_conversion_egress_entry_index_hit_indication_hit_clear */
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_INGRESS_EGRESS_PORT_PORTS_ALL_RANGE_CHECK
/*
 * vlan get vlan-conversion ( ingress | egress ) port ( <PORT_LIST:ports> | all ) range-check
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_conversion_ingress_egress_port_ports_all_range_check(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    uint32  index;
    rtk_port_t  port;
    diag_portlist_t portlist;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    if ('i' == TOKEN_CHAR(3,0))
    {
        /* ingress */
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portIgrVlanCnvtRangeCheckSet_get(unit, port, &index), ret);
            DIAG_UTIL_MPRINTF(" Port %d\t: set %d\n", port, index);
        }
    }
    else
    {
        /* egress */
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portEgrVlanCnvtRangeCheckSet_get(unit, port, &index), ret);
            DIAG_UTIL_MPRINTF(" Port %d\t: set %d\n", port, index);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_conversion_ingress_egress_port_ports_all_range_check */
#endif

#ifdef CMD_VLAN_GET_VLAN_AGGREGATION_STATE
/*
 * vlan get vlan-aggregation state
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_aggregation_state(
    cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_aggrEnable_get(unit, &enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    DIAG_UTIL_MPRINTF("VLAN Aggregation Status: %s\n", \
        ((enable == ENABLED)? DIAG_STR_ENABLE : DIAG_STR_DISABLE));

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_aggregation_state */
#endif

#ifdef CMD_VLAN_SET_VLAN_AGGREGATION_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-aggregation state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_aggregation_state_disable_enable(
    cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(4, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    if ((ret = rtk_vlan_aggrEnable_set(unit, enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_aggregation_state_disable_enable */
#endif

#ifdef CMD_VLAN_SET_VLAN_AGGREGATION_PORT_PORTS_ALL_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-aggregation port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_aggregation_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_port_t      port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('e' == TOKEN_CHAR(6, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_vlan_portVlanAggrEnable_set(unit, port, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_aggregation_port_ports_all_state_disable_enable */
#endif

#ifdef CMD_VLAN_SET_VLAN_AGGREGATION_TRUNK_TRUNKS_ALL_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-aggregation trunk ( <MASK_LIST:trunks> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_aggregation_trunk_trunks_all_state_disable_enable(
    cparser_context_t *context,
    char **trunks_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_trk_t       trunk;
    diag_trklist_t  trklist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('e' == TOKEN_CHAR(6, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

    DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
    {
        if ((ret = rtk_vlan_trkVlanAggrEnable_set(unit, trunk, enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_aggregation_trunk_trunks_all_state_disable_enable */
#endif

#ifdef CMD_VLAN_SET_VLAN_AGGREGATION_PORT_PORTS_ALL_PRIORITY_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-aggregation port ( <PORT_LIST:ports> | all ) priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_aggregation_port_ports_all_priority_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_port_t      port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrPriEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_aggregation_port_ports_all_priority_state_disable_enable */
#endif

#ifdef CMD_VLAN_SET_VLAN_AGGREGATION_TRUNK_TRUNKS_ALL_PRIORITY_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-aggregation trunk ( <MASK_LIST:trunks> | all ) priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_aggregation_trunk_trunks_all_priority_state_disable_enable(
    cparser_context_t *context,
    char **trunks_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_enable_t    enable;
    rtk_trk_t       trunk;
    diag_trklist_t  trklist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

    DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_trkVlanAggrPriEnable_set(unit, trunk, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_aggregation_trunk_trunks_all_priority_state_disable_enable */
#endif

#ifdef CMD_VLAN_GET_VLAN_AGGREGATION_PORT_PORTS_ALL_PRIORITY_STATE
/*
 * vlan get vlan-aggregation port ( <PORT_LIST:ports> | all ) priority state
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_aggregation_port_ports_all_priority_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    uint32          unit = 0;
    rtk_enable_t    enable;
    rtk_port_t      port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrPriEnable_get(unit, port, &enable), ret);
        DIAG_UTIL_MPRINTF("\tPort %d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_aggregation_port_ports_all_priority_state */
#endif

#ifdef CMD_VLAN_GET_VLAN_AGGREGATION_TRUNK_TRUNKS_ALL_PRIORITY_STATE
/*
 * vlan get vlan-aggregation trunk ( <MASK_LIST:trunks> | all ) priority state
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_aggregation_trunk_trunks_all_priority_state(
    cparser_context_t *context,
    char **trunks_ptr)
{
    int32           ret = RT_ERR_FAILED;
    uint32          unit = 0;
    rtk_enable_t    enable;
    rtk_trk_t       trunk;
    diag_trklist_t  trklist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

    DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_trkVlanAggrPriEnable_get(unit, trunk, &enable), ret);
        DIAG_UTIL_MPRINTF("\tTrunk %d : %s\n", trunk, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_aggregation_trunk_trunks_all_priority_state */
#endif

#ifdef CMD_VLAN_GET_LEAKY_STP_FILTER_STATE
/*
 * vlan get leaky stp-filter state
 */
cparser_result_t
cparser_cmd_vlan_get_leaky_stp_filter_state(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_leakyStpFilter_get(unit, &enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("Vlan leaky STP filter: ");
    if (ENABLED == enable)
    {
        DIAG_UTIL_MPRINTF("%s", DIAG_STR_ENABLE);
    }
    else
    {
        DIAG_UTIL_MPRINTF("%s", DIAG_STR_DISABLE);
    }

    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_LEAKY_STP_FILTER_STATE_DISABLE_ENABLE
/*
 * vlan set leaky stp-filter state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_leaky_stp_filter_state_disable_enable(cparser_context_t *context)
{

    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('e' == TOKEN_CHAR(5, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    if ((ret = rtk_vlan_leakyStpFilter_set(unit, enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_EXCEPTION_ACTION
/*
 * vlan get exception action
 */
cparser_result_t
cparser_cmd_vlan_get_exception_action(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_except_get(unit, &action)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("Vlan except action: ");
    if (ACTION_DROP == action)
    {
        DIAG_UTIL_MPRINTF("%s", "Drop");
    }
    else if (ACTION_FORWARD == action)
    {
        DIAG_UTIL_MPRINTF("%s", "Forward");
    }
    else
    {
        DIAG_UTIL_MPRINTF("%s", "Trap");
    }

    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_EXCEPTION_ACTION_DROP_FORWARD_TRAP
/*
 * vlan set exception action ( drop | forward | trap )
 */
cparser_result_t
cparser_cmd_vlan_set_exception_action_drop_forward_trap(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('d' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_DROP;
    }
    else if('f' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_FORWARD;
    }
    else
    {
        action = ACTION_TRAP2CPU;
    }

    if ((ret = rtk_vlan_except_set(unit, action)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_INGRESS_DEFAULT_ACTION_PORTS_ALL
/*
 * vlan get vlan-conversion ingress default-action ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_conversion_ingress_default_action_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("VLAN conversion default action:\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : ", port);

        if ((ret = rtk_vlan_portIgrCnvtDfltAct_get(unit, port, &action)) == RT_ERR_OK)
        {
            if (ACTION_DROP == action)
                DIAG_UTIL_MPRINTF("%s\n", "Drop");
            else
                DIAG_UTIL_MPRINTF("%s\n", "Forward");
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_DEFAULT_ACTION_PORTS_ALL_FORWARD_DROP
/*
 * vlan set vlan-conversion ingress default-action ( <PORT_LIST:ports> | all ) ( forward | drop )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_conversion_ingress_default_action_ports_all_forward_drop(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    if('d' == TOKEN_CHAR(6, 0))
    {
        action = ACTION_DROP;
    }
    else
    {
        action = ACTION_FORWARD;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_vlan_portIgrCnvtDfltAct_set(unit, port, action)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;

}
#endif

#ifdef CMD_VLAN_CLEAR_VLAN_CONVERSION_INGRESS_ENTRY_INDEX_ALL
/*
 * vlan clear vlan-conversion ingress entry ( <UINT:index> | all )
 */
cparser_result_t
cparser_cmd_vlan_clear_vlan_conversion_ingress_entry_index_all(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_vlan_igrVlanCnvtEntry_t entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('a' == TOKEN_CHAR(5, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_delAll(unit) , ret);
    }
    else
    {
        osal_memset(&entry, 0x00, sizeof(rtk_vlan_igrVlanCnvtEntry_t));
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_set(unit, *index_ptr, &entry), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_CLEAR_VLAN_CONVERSION_EGRESS_ENTRY_INDEX_ALL
/*
 * vlan clear vlan-conversion egress entry ( <UINT:index> | all )
 */
cparser_result_t
cparser_cmd_vlan_clear_vlan_conversion_egress_entry_index_all(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_vlan_egrVlanCnvtEntry_t entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('a' == TOKEN_CHAR(5, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_delAll(unit) , ret);
    }
    else
    {
        osal_memset(&entry, 0x00, sizeof(rtk_vlan_egrVlanCnvtEntry_t));
        DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_set(unit, *index_ptr, &entry), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_DUMP_IP_SUBNET_BASED_VLAN
/*
 * vlan dump ip-subnet-based-vlan
 */
cparser_result_t cparser_cmd_vlan_dump_ip_subnet_based_vlan(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  valid;
    ipaddr_t sip, sip_msk;
    rtk_vlan_t vid;
    rtk_pri_t priority;
    uint32  i, cnvt_entry_total, cnvt_entry_per_block;
    rtk_vlan_igrVlanCnvtBlk_mode_t mode;
    rtk_port_t port, port_msk;
    char    ipv4Str[16], ipv4MskStr[16];

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF("Index | State   | Source IP Address |   Source IP Mask  | Port ID | Port ID Mask | VID | Priority\n");
    DIAG_UTIL_MPRINTF("------+---------+-------------------+-------------------+---------+--------------+-----+----------\n");
    DIAG_OM_GET_CHIP_CAPACITY(unit, cnvt_entry_total, max_num_of_c2sc_entry);
    DIAG_OM_GET_CHIP_CAPACITY(unit, cnvt_entry_per_block, max_num_of_c2sc_blk_entry);

    for(i=0; i<cnvt_entry_total; i++)
    {
        if((i%cnvt_entry_per_block) == 0)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtBlkMode_get(unit, i/cnvt_entry_per_block, &mode) , ret);

            if(mode != CONVERSION_MODE_IP_SUBNET_BASED)
            {
                i += (cnvt_entry_per_block-1);
                continue;
            }
        }

        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanWithPort_get(unit, i, &valid, &sip, &sip_msk, &port, &port_msk, &vid, &priority) , ret);

        if(vid != 0)
        {
            diag_util_ip2str(ipv4Str, sip);
            diag_util_ip2str(ipv4MskStr, sip_msk);
            DIAG_UTIL_MPRINTF("%6d| %8s|  %15s  |  %15s  |%9d|%12s%02X|%5d|%5d\n", i, valid ? DIAG_STR_ENABLE : DIAG_STR_DISABLE,\
                ipv4Str, ipv4MskStr, port, "0x", port_msk, vid, priority);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_TABLE_VID_VID
/*
 * vlan get vlan-table vid <UINT:vid>
 */
cparser_result_t cparser_cmd_vlan_get_vlan_table_vid_vid(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_stg_t   stg;
    rtk_portmask_t  member_portmask, untag_portmask;
    char        port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    uint32      profile_idx;
    rtk_vlan_l2LookupMode_t mode;
    rtk_mcast_group_t group;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_vlan_groupMask_t groupMask;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_port_get(unit, *vid_ptr, &member_portmask, &untag_portmask)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("Vlan %u \n", *vid_ptr);
    diag_util_lPortMask2str(port_list, &member_portmask);
    DIAG_UTIL_MPRINTF("  Member Ports\t: %s \n", port_list);

    diag_util_lPortMask2str(port_list, &untag_portmask);
    DIAG_UTIL_MPRINTF("  Untag Ports\t: %s \n", port_list);

    RTK_PORTMASK_REMOVE(member_portmask,untag_portmask);
    diag_util_lPortMask2str(port_list, &member_portmask);
    DIAG_UTIL_MPRINTF("  Tag Ports\t: %s \n", port_list);

    diag_util_printf("  mcast group\t: ");
    if (RT_ERR_OK == (ret = rtk_vlan_mcastGroup_get(unit, *vid_ptr, &group)) )
    {
        DIAG_UTIL_MPRINTF("0x%X\n", group);
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support\n");
    }

    diag_util_printf("  Stg\t\t: ");
    if (RT_ERR_OK == (ret = rtk_vlan_stg_get(unit, *vid_ptr, &stg)) )
    {
        DIAG_UTIL_MPRINTF("%u\n", stg);
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support\n");
    }

    diag_util_printf("  UBCAST hkey\t: ");

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if (RT_ERR_OK == (ret = rtk_vlan_l2UcastLookupMode_get(unit, *vid_ptr, &mode)))
        {
            DIAG_UTIL_MPRINTF("%s \n", (mode == VLAN_L2_LOOKUP_MODE_VID) ? "UC_LOOKUP_ON_VID" : "UC_LOOKUP_ON_FID");
        }
        else
        {
            DIAG_UTIL_MPRINTF("Not support\n");
        }
    }
    else
#endif
    {
        if (RT_ERR_OK == (ret = rtk_vlan_l2LookupMode_get(unit, *vid_ptr,VLAN_L2_MAC_TYPE_UC, &mode)))
        {
            DIAG_UTIL_MPRINTF("%s \n", (mode == VLAN_L2_LOOKUP_MODE_VID) ? "UC_LOOKUP_ON_VID" : "UC_LOOKUP_ON_FID");
        }
        else
        {
            DIAG_UTIL_MPRINTF("Not support\n");
        }
    }

    diag_util_printf("  MCAST hkey\t: ");

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if (RT_ERR_OK == (ret = rtk_vlan_l2McastLookupMode_get(unit, *vid_ptr, &mode)))
        {
            DIAG_UTIL_MPRINTF("%s \n", (mode == VLAN_L2_LOOKUP_MODE_VID) ? "MC_LOOKUP_ON_VID" : "MC_LOOKUP_ON_FID");
        }
        else
        {
            DIAG_UTIL_MPRINTF("Not support\n");
        }
    }
    else
#endif
    {
        if (RT_ERR_OK == (ret = rtk_vlan_l2LookupMode_get(unit, *vid_ptr, VLAN_L2_MAC_TYPE_MC, &mode)))
        {
            DIAG_UTIL_MPRINTF("%s \n", (mode == VLAN_L2_LOOKUP_MODE_VID) ? "MC_LOOKUP_ON_VID" : "MC_LOOKUP_ON_FID");
        }
        else
        {
            DIAG_UTIL_MPRINTF("Not support\n");
        }
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_printf("  VLAN GroupMask\t: ");
        if (RT_ERR_OK == (ret = rtk_vlan_groupMask_get(unit, *vid_ptr, &groupMask)) )
        {
            DIAG_UTIL_MPRINTF("%x\n", groupMask.bits[0]);
        }
        else
        {
            DIAG_UTIL_MPRINTF("Not support\n");
        }
    }
#endif

    diag_util_printf("  Profile index\t: ");
    if (RT_ERR_OK == (ret = rtk_vlan_profileIdx_get(unit, *vid_ptr, &profile_idx)) )
    {
        DIAG_UTIL_MPRINTF("%u\n", profile_idx);
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_ACCEPT_FRAME_TYPE_INNER_PORT_PORTS_ALL
/*
 * vlan get accept-frame-type inner port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_vlan_get_accept_frame_type_inner_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_acceptFrameType_t  accept_frame_type;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();


    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("Accept frame type of ports\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : ", port);

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_vlan_portInnerAcceptFrameType_get(unit, port, &accept_frame_type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        if ((ret = rtk_vlan_portAcceptFrameType_get(unit, port, INNER_VLAN, &accept_frame_type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

        switch (accept_frame_type)
        {
            case ACCEPT_FRAME_TYPE_ALL:
                DIAG_UTIL_MPRINTF("accept all frame\n");
                break;

            case ACCEPT_FRAME_TYPE_TAG_ONLY:
                DIAG_UTIL_MPRINTF("accept tag frame only\n");
                break;

            case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
                DIAG_UTIL_MPRINTF("accept untag frame only\n");
                break;

            default:
                return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_ACCEPT_FRAME_TYPE_OUTER_PORT_PORTS_ALL
/*
 * vlan get accept-frame-type outer port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_vlan_get_accept_frame_type_outer_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_acceptFrameType_t  accept_frame_type;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();


    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("Accept frame type of outer tag\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : ", port);

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_vlan_portOuterAcceptFrameType_get(unit, port, &accept_frame_type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        if ((ret = rtk_vlan_portAcceptFrameType_get(unit, port, OUTER_VLAN, &accept_frame_type)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

        switch (accept_frame_type)
        {
            case ACCEPT_FRAME_TYPE_ALL:
                DIAG_UTIL_MPRINTF("accept all frame\n");
                break;

            case ACCEPT_FRAME_TYPE_TAG_ONLY:
                DIAG_UTIL_MPRINTF("accept tag frame only\n");
                break;

            case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
                DIAG_UTIL_MPRINTF("accept untag frame only\n");
                break;

            default:
                return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_EGRESS_FILTER_PORT_PORTS_ALL_STATE
/*
 * vlan get egress-filter port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t cparser_cmd_vlan_get_egress_filter_port_ports_all_state(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    int32       (*fp_port)(uint32, rtk_port_t, rtk_enable_t *);
    rtk_enable_t    enable;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    fp_port = rtk_vlan_portEgrFilterEnable_get;
    DIAG_UTIL_MPRINTF("Egress filter status of ports\n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : ", port);
        if ((ret = fp_port(unit, port, &enable)) == RT_ERR_OK)
        {
            if (ENABLED == enable)
            {
                DIAG_UTIL_MPRINTF("ENABLE\n");
            }
            else
            {
                DIAG_UTIL_MPRINTF("DISABLE\n");
            }
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_LEAKY_MULTICAST_STATE
/*
 * vlan get leaky multicast state
 */
cparser_result_t cparser_cmd_vlan_get_leaky_multicast_state(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    int32       (*fp_sys)(uint32, rtk_enable_t *);
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    fp_sys = rtk_vlan_mcastLeakyEnable_get;
    diag_util_printf("Multicast leaky system status : ");
    if ((ret = fp_sys(unit, &enable)) == RT_ERR_OK)
    {
        if (ENABLED == enable)
        {
            DIAG_UTIL_MPRINTF("ENABLE\n");
        }
        else
        {
            DIAG_UTIL_MPRINTF("DISABLE\n");
        }
    }
    else
    {
        DIAG_ERR_PRINT(ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_PVID_INNER_PORT_PORTS_ALL
/*
 * vlan get pvid inner port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_vlan_get_pvid_inner_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_t  pvid;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("Port based vlan configuration \n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : ", port);

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_vlan_portInnerPvid_get(unit, port, &pvid)) == RT_ERR_OK)
            {
                DIAG_UTIL_MPRINTF("%u\n", pvid);
            }
            else
            {
                DIAG_ERR_PRINT(ret);
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portPvid_get(unit, port, INNER_VLAN, &pvid)) == RT_ERR_OK)
            {
                DIAG_UTIL_MPRINTF("%u\n", pvid);
            }
            else
            {
                DIAG_ERR_PRINT(ret);
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_PVID_OUTER_PORT_PORTS_ALL
/*
 * vlan get pvid outer port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_vlan_get_pvid_outer_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_t  pvid;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("Outer tag port based vlan configuration \n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : ", port);

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_vlan_portOuterPvid_get(unit, port, &pvid)) == RT_ERR_OK)
            {
                DIAG_UTIL_MPRINTF("%u\n", pvid);
            }
            else
            {
                DIAG_ERR_PRINT(ret);
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portPvid_get(unit, port, OUTER_VLAN, &pvid)) == RT_ERR_OK)
            {
                DIAG_UTIL_MPRINTF("%u\n", pvid);
            }
            else
            {
                DIAG_ERR_PRINT(ret);
            }
        }

    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_PROTOCOL_VLAN_GROUP_INDEX
/*
 * vlan get protocol-vlan group <UINT:index>
 */
cparser_result_t cparser_cmd_vlan_get_protocol_vlan_group_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_protoGroup_t   protoGroup;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_protoGroup_get(unit, *index_ptr, &protoGroup)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("Protocol Group %u\n", *index_ptr);

    diag_util_printf("  Protocol type\t: ");
    switch (protoGroup.frametype)
    {
        case FRAME_TYPE_UNKNOWN:
            DIAG_UTIL_MPRINTF("Unknown\n");
            break;

        case FRAME_TYPE_ETHERNET:
            DIAG_UTIL_MPRINTF("Ethernet\n");
            break;

        case FRAME_TYPE_RFC1042:
            DIAG_UTIL_MPRINTF("SNAP\n");
            break;

        case FRAME_TYPE_LLCOTHER:
            DIAG_UTIL_MPRINTF("LLC other\n");
            break;

        default:
            DIAG_UTIL_MPRINTF("Not support\n");
            break;
    }

    DIAG_UTIL_MPRINTF("  Frame value\t: 0x%x\n", protoGroup.framevalue);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_PROTOCOL_VLAN_PORT_PORTS_ALL
/*
 * vlan get protocol-vlan port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_vlan_get_protocol_vlan_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      group_id;
    uint32      group_id_max;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlan_protoVlanCfg_t protoVlan_cfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    DIAG_UTIL_MPRINTF("Port configuration of protocol vlan \n");
    DIAG_OM_GET_CHIP_CAPACITY(unit, group_id_max, protocol_vlan_idx_max);
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_MPRINTF("Port %u configuration : \n", port);

        for (group_id = 0; group_id <= group_id_max; group_id++)
        {
            DIAG_UTIL_MPRINTF("  Group %u \n", group_id);

            if ((ret = rtk_vlan_portProtoVlan_get(unit, port, group_id, &protoVlan_cfg)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                continue;
            }

            DIAG_UTIL_MPRINTF("    VLAN Type   : %s\n", protoVlan_cfg.vlan_type ? "OUTER":"INNER");
            DIAG_UTIL_MPRINTF("    VID Assign  : %s\n", protoVlan_cfg.vid_assign ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            DIAG_UTIL_MPRINTF("    Vlan Id     : %u\n", protoVlan_cfg.vid);
            DIAG_UTIL_MPRINTF("    Priority Assign  : %s\n", protoVlan_cfg.pri_assign ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            DIAG_UTIL_MPRINTF("    Priority    : %u\n", protoVlan_cfg.pri);
        }

        DIAG_UTIL_MPRINTF("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_PROTOCOL_VLAN_INNER_PORT_PORTS_ALL
/*
 * vlan get protocol-vlan inner port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_vlan_get_protocol_vlan_inner_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      group_id;
    uint32      group_id_max;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlan_protoVlanCfg_t protoVlan_cfg;
    rtk_switch_devInfo_t devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("Port configuration of protocol vlan \n");
    DIAG_OM_GET_CHIP_CAPACITY(unit, group_id_max, protocol_vlan_idx_max);
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_MPRINTF("Port %u configuration : \n", port);

        for (group_id = 0; group_id <= group_id_max; group_id++)
        {

            DIAG_UTIL_MPRINTF("  Group %u \n", group_id);

            if ((ret = rtk_vlan_portProtoVlan_get(unit, port, group_id, &protoVlan_cfg)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                continue;
            }

            if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
                DIAG_UTIL_MPRINTF("    State    : %s\n", protoVlan_cfg.valid ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
            DIAG_UTIL_MPRINTF("    Vlan Id  : %u\n", protoVlan_cfg.vid);
            DIAG_UTIL_MPRINTF("    Priority : %u\n", protoVlan_cfg.pri);
        }

        DIAG_UTIL_MPRINTF("\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_DUMP_INGRESS_EGRESS_PORT_PORTS_ALL
/*
 * vlan dump ( ingress | egress ) port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_vlan_dump_ingress_egress_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    rtk_enable_t enable;
    rtk_port_t  port;
    diag_portlist_t  portmask;
#if defined(CONFIG_SDK_RTL8380)
    rtk_vlan_tagKeepType_t inner_keeptype, outer_keeptype;
#endif
    rtk_vlanType_t vlanType;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_vlan_egrTpidSrc_t tpidSrc;
#endif
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    rtk_enable_t keepOuter, keepInner;
#endif
    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if ('i' == TOKEN_STR(2)[0])
    {
        DIAG_UTIL_MPRINTF("Port ingress configuration\n");
        DIAG_UTIL_PORTMASK_SCAN(portmask, port)
        {
            DIAG_UTIL_MPRINTF("Port %u configuration : \n", port);

            diag_util_printf("Inner TPID entry mask\t: ");

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_vlan_portIgrInnerTpid_get(unit, port, &value)) == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("0x%x\n", value);
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }
    else
#endif
    {
        if ((ret = rtk_vlan_portIgrTpid_get(unit, port, INNER_VLAN, &value)) == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("0x%x\n", value);
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }

            diag_util_printf("Outer TPID entry mask\t: ");

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_vlan_portIgrOuterTpid_get(unit, port, &value)) == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("0x%x\n", value);
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }
    else
#endif
    {
        if ((ret = rtk_vlan_portIgrTpid_get(unit, port, OUTER_VLAN, &value)) == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("0x%x\n", value);
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }
            diag_util_printf("Extra tag status\t: ");
            if (RT_ERR_OK == rtk_vlan_portIgrExtraTagEnable_get(unit, port, &value))
            {
                if (ENABLED == value)
                {
                    DIAG_UTIL_MPRINTF("ENABLE\n");
                }
                else
                {
                    DIAG_UTIL_MPRINTF("DISABLE\n");
                }
            }
            else
            {
                DIAG_ERR_PRINT(ret);
            }


#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_vlan_portIgrTagKeepEnable_get(unit, port, &keepOuter, &keepInner)) == RT_ERR_OK)
        {
            diag_util_printf("Keep inner tag format\t: ");
            if (ENABLED == keepInner)
            {
                DIAG_UTIL_MPRINTF("ENABLE\n");
            }
            else
            {
                DIAG_UTIL_MPRINTF("DISABLE\n");
            }
            diag_util_printf("Keep outer tag format\t: ");
            if (ENABLED == keepOuter)
            {
                DIAG_UTIL_MPRINTF("ENABLE\n");
            }
            else
            {
                DIAG_UTIL_MPRINTF("DISABLE\n");
            }
        }
        else
        {
            diag_util_printf("Keep inner tag format\t: ");
            DIAG_ERR_PRINT(ret);
            diag_util_printf("Keep outer tag format\t: ");
            DIAG_ERR_PRINT(ret);
        }
    }
    else
#endif
    {
        diag_util_printf("Keep inner tag format\t: ");
        if (RT_ERR_OK == rtk_vlan_portIgrVlanTransparentEnable_get(unit, port,INNER_VLAN, &enable))
        {
            if (ENABLED == enable)
            {
                DIAG_UTIL_MPRINTF("ENABLE\n");
            }
            else
            {
                DIAG_UTIL_MPRINTF("DISABLE\n");
            }
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }

        diag_util_printf("Keep outer tag format\t: ");
        if ((ret = rtk_vlan_portIgrVlanTransparentEnable_get(unit, port,OUTER_VLAN, &enable)) == RT_ERR_OK)
        {
            if (ENABLED == enable)
            {
                DIAG_UTIL_MPRINTF("ENABLE\n");
            }
            else
            {
                DIAG_UTIL_MPRINTF("DISABLE\n");
            }
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }

#if defined(CONFIG_SDK_RTL8380)
            if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            {
                if ((ret = rtk_vlan_portIgrTagKeepType_get(unit, port, &outer_keeptype, &inner_keeptype)) == RT_ERR_OK)
                {
                    diag_util_printf("Keep inner tag type\t: ");
                    if (TAG_KEEP_TYPE_NOKEEP == inner_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("NO KEEP\n");
                    }
                    else if (TAG_KEEP_TYPE_FORMAT == inner_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("KEEP FORMAT\n");
                    }
                    else if (TAG_KEEP_TYPE_CONTENT == inner_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("KEEP CONTENT\n");
                    }

                    diag_util_printf("Keep outer tag type\t: ");
                    if (TAG_KEEP_TYPE_NOKEEP == outer_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("NO KEEP\n");
                    }
                    else if (TAG_KEEP_TYPE_FORMAT == outer_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("KEEP FORMAT\n");
                    }
                    else if (TAG_KEEP_TYPE_CONTENT == outer_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("KEEP CONTENT\n");
                    }
                }
                else
                {
                    diag_util_printf("Keep inner tag type\t: ");
                    DIAG_ERR_PRINT(ret);
                    diag_util_printf("Keep outer tag type\t: ");
                    DIAG_ERR_PRINT(ret);
                }
            }
#endif
#if defined(CONFIG_SDK_RTL8380)|| defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
          DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portFwdVlan_get(unit, port, VLAN_PKT_TAG_MODE_ALL, &vlanType), ret);
                diag_util_printf("Fwd-VLAN           \t: ");
                if (OUTER_VLAN == vlanType)
                    DIAG_UTIL_MPRINTF("Outer VLAN\n");
                else
                    DIAG_UTIL_MPRINTF("Inner VLAN\n");
            }
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
            if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portFwdVlan_get(unit, port, VLAN_PKT_TAG_MODE_UNTAG, &vlanType), ret);
                diag_util_printf("Fwd-VLAN (untagged)\t: ");
                if (OUTER_VLAN == vlanType)
                    DIAG_UTIL_MPRINTF("Outer VLAN\n");
                else
                    DIAG_UTIL_MPRINTF("Inner VLAN\n");

                DIAG_UTIL_ERR_CHK(rtk_vlan_portFwdVlan_get(unit, port, VLAN_PKT_TAG_MODE_INNERTAG, &vlanType), ret);
                diag_util_printf("Fwd-VLAN (inner-tagged)\t: ");
                if (OUTER_VLAN == vlanType)
                    DIAG_UTIL_MPRINTF("Outer VLAN\n");
                else
                    DIAG_UTIL_MPRINTF("Inner VLAN\n");

                DIAG_UTIL_ERR_CHK(rtk_vlan_portFwdVlan_get(unit, port, VLAN_PKT_TAG_MODE_OUTERTAG, &vlanType), ret);
                diag_util_printf("Fwd-VLAN (outer-tagged)\t: ");
                if (OUTER_VLAN == vlanType)
                    DIAG_UTIL_MPRINTF("Outer VLAN\n");
                else
                    DIAG_UTIL_MPRINTF("Inner VLAN\n");

                DIAG_UTIL_ERR_CHK(rtk_vlan_portFwdVlan_get(unit, port, VLAN_PKT_TAG_MODE_DBLTAG, &vlanType), ret);
                diag_util_printf("Fwd-VLAN (double-tag)\t: ");
                if (OUTER_VLAN == vlanType)
                    DIAG_UTIL_MPRINTF("Outer VLAN\n");
                else
                    DIAG_UTIL_MPRINTF("Inner VLAN\n");
            }
#endif

#if defined(CONFIG_SDK_RTL9310)
            diag_util_printf("Private VLAN\t: ");
            if (RT_ERR_OK == rtk_vlan_portPrivateVlanEnable_get(unit, port, &enable))
            {
                if (ENABLED == enable)
                {
                    DIAG_UTIL_MPRINTF("ENABLE\n");
                }
                else
                {
                    DIAG_UTIL_MPRINTF("DISABLE\n");
                }
            }
            else
            {
                DIAG_ERR_PRINT(ret);
            }
#endif

            DIAG_UTIL_MPRINTF("\n");
        }
    }
    else
    {
        DIAG_UTIL_MPRINTF("Port egress configuration\n");
        DIAG_UTIL_PORTMASK_SCAN(portmask, port)
        {
            DIAG_UTIL_MPRINTF("Port %u configuration : \n", port);
            diag_util_printf("Inner:\n");

            diag_util_printf("TPID Entry Index\t: ");

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_vlan_portEgrInnerTpid_get(unit, port, &value)) == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("%u\n", value);
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }
    else
#endif
    {
        if ((ret = rtk_vlan_portEgrTpid_get(unit, port, INNER_VLAN, &value)) == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("%u\n", value);
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }

            diag_util_printf("Tag Status\t\t: ");

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_vlan_portEgrInnerTagSts_get(unit, port, &value)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
    }
    else
#endif
    {
        if ((ret = rtk_vlan_portEgrTagSts_get(unit, port, INNER_VLAN, &value)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
        }
    }

    if (TAG_STATUS_UNTAG == value)
    {
        DIAG_UTIL_MPRINTF("Untag\n");
    }
    else if (TAG_STATUS_TAGGED == value)
    {
        DIAG_UTIL_MPRINTF("Tag\n");
    }
    else if (TAG_STATUS_PRIORITY_TAGGED == value)
    {
        DIAG_UTIL_MPRINTF("Priority Tag\n");
    }
    else
    {
        DIAG_UTIL_MPRINTF("Internal\n");
    }


    diag_util_printf("Outer:\n");

    diag_util_printf("TPID Entry Index\t: ");

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_vlan_portEgrOuterTpid_get(unit, port, &value)) == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("%u\n", value);
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }
    else
#endif
    {
        if ((ret = rtk_vlan_portEgrTpid_get(unit, port, OUTER_VLAN, &value)) == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("%u\n", value);
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }

    diag_util_printf("Tag Status\t\t: ");

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if (RT_ERR_OK != rtk_vlan_portEgrOuterTagSts_get(unit, port, &value))
        {
            DIAG_ERR_PRINT(ret);
        }
    }
    else
#endif
    {
        if (RT_ERR_OK != rtk_vlan_portEgrTagSts_get(unit, port, OUTER_VLAN, &value))
        {
            DIAG_ERR_PRINT(ret);
        }
    }

    if (TAG_STATUS_UNTAG == value)
    {
        DIAG_UTIL_MPRINTF("Untag\n");
    }
    else if (TAG_STATUS_TAGGED == value)
    {
        DIAG_UTIL_MPRINTF("Tag\n");
    }
    else if (TAG_STATUS_PRIORITY_TAGGED == value)
    {
        DIAG_UTIL_MPRINTF("Priority Tag\n");
    }
    else
    {
        DIAG_UTIL_MPRINTF("Internal\n");
    }

    diag_util_printf("Extra:\n");

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if (RT_ERR_OK == rtk_vlan_portEgrTagKeepEnable_get(unit, port, &keepOuter, &keepInner))
        {
            diag_util_printf("Keep inner tag format\t: ");
            if (ENABLED == keepInner)
            {
                DIAG_UTIL_MPRINTF("ENABLE\n");
            }
            else
            {
                DIAG_UTIL_MPRINTF("DISABLE\n");
            }
            diag_util_printf("Keep outer tag format\t: ");
            if (ENABLED == keepOuter)
            {
                DIAG_UTIL_MPRINTF("ENABLE\n");
            }
            else
            {
                DIAG_UTIL_MPRINTF("DISABLE\n");
            }
        }
        else
        {
            diag_util_printf("Keep inner tag format\t: ");
            DIAG_ERR_PRINT(ret);
            diag_util_printf("Keep outer tag format\t: ");
            DIAG_ERR_PRINT(ret);
        }
    }
    else
#endif
    {
        diag_util_printf("Keep inner tag format\t: ");
        if (RT_ERR_OK == rtk_vlan_portEgrVlanTransparentEnable_get(unit, port, INNER_VLAN, &enable))
        {
            if (ENABLED == enable)
            {
                DIAG_UTIL_MPRINTF("ENABLE\n");
            }
            else
            {
                DIAG_UTIL_MPRINTF("DISABLE\n");
            }
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }

        diag_util_printf("Keep outer tag format\t: ");
        if (RT_ERR_OK == rtk_vlan_portEgrVlanTransparentEnable_get(unit, port, OUTER_VLAN, &enable))
        {
            if (ENABLED == enable)
            {
                DIAG_UTIL_MPRINTF("ENABLE\n");
            }
            else
            {
                DIAG_UTIL_MPRINTF("DISABLE\n");
            }
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
            diag_util_printf("Keep inner TPID value\t: ");
            if (RT_ERR_OK == rtk_vlan_portEgrTpidSrc_get(unit, port, INNER_VLAN, &tpidSrc))
            {
                if (VLAN_TPID_SRC_ORIG == tpidSrc)
                {
                    DIAG_UTIL_MPRINTF("ENABLE\n");
                }
                else
                {
                    DIAG_UTIL_MPRINTF("DISABLE\n");
                }
            }
            else
            {
                DIAG_ERR_PRINT(ret);
            }

            diag_util_printf("Keep outer TPID value\t: ");
            if (RT_ERR_OK == rtk_vlan_portEgrTpidSrc_get(unit, port, OUTER_VLAN, &tpidSrc))
            {
                if (VLAN_TPID_SRC_ORIG == tpidSrc)
                {
                    DIAG_UTIL_MPRINTF("ENABLE\n");
                }
                else
                {
                    DIAG_UTIL_MPRINTF("DISABLE\n");
                }
            }
            else
            {
                DIAG_ERR_PRINT(ret);
            }
#endif

#if defined(CONFIG_SDK_RTL8380)
            if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            {
                if (RT_ERR_OK == rtk_vlan_portEgrTagKeepType_get(unit, port, &outer_keeptype, &inner_keeptype))
                {
                    diag_util_printf("Keep inner tag type\t: ");
                    if (TAG_KEEP_TYPE_NOKEEP == inner_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("NO KEEP\n");
                    }
                    else if (TAG_KEEP_TYPE_FORMAT == inner_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("KEEP FORMAT\n");
                    }
                    else if (TAG_KEEP_TYPE_CONTENT == inner_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("KEEP CONTENT\n");
                    }

                    diag_util_printf("Keep outer tag type\t: ");
                    if (TAG_KEEP_TYPE_NOKEEP == outer_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("NO KEEP\n");
                    }
                    else if (TAG_KEEP_TYPE_FORMAT == outer_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("KEEP FORMAT\n");
                    }
                    else if (TAG_KEEP_TYPE_CONTENT == outer_keeptype)
                    {
                        DIAG_UTIL_MPRINTF("KEEP CONTENT\n");
                    }
                }
                else
                {
                    diag_util_printf("Keep inner tag type\t: ");
                    DIAG_ERR_PRINT(ret);
                    diag_util_printf("Keep outer tag type\t: ");
                    DIAG_ERR_PRINT(ret);
                }
            }
#endif

            DIAG_UTIL_MPRINTF("\n");
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_DUMP_MAC_BASED_VLAN
/*
 * vlan dump mac-based-vlan
 */
cparser_result_t cparser_cmd_vlan_dump_mac_based_vlan(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  valid;
    rtk_mac_t smac;
    rtk_mac_t smsk;
    rtk_vlan_t vid;
    uint32  i, cnvt_entry_total, cnvt_entry_per_block;
    rtk_vlan_igrVlanCnvtBlk_mode_t mode;
    rtk_pri_t   priority;
    rtk_port_t port, pmsk;
    char macStr[20], macMskStr[20];

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF("Index | State   | Source MAC Address | Source MAC Mask   | Port ID | Port ID Mask | VID | Priority\n");
    DIAG_UTIL_MPRINTF("------+---------+--------------------+-------------------+---------+--------------+-----+----------\n");
    DIAG_OM_GET_CHIP_CAPACITY(unit, cnvt_entry_total, max_num_of_c2sc_entry);
    DIAG_OM_GET_CHIP_CAPACITY(unit, cnvt_entry_per_block, max_num_of_c2sc_blk_entry);

    for(i=0; i<cnvt_entry_total; i++)
    {
        if((i%cnvt_entry_per_block) == 0)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtBlkMode_get(unit, i/cnvt_entry_per_block, &mode) , ret);

            if(mode != CONVERSION_MODE_MAC_BASED)
            {
                i += (cnvt_entry_per_block-1);
                continue;
            }
        }

        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanWithPort_get(unit, i, &valid, &smac, &smsk, &port, &pmsk, &vid, &priority) , ret);

        if ((vid != 0))
        {
            DIAG_UTIL_MAC2STR(macStr, smac.octet);
            DIAG_UTIL_MAC2STR(macMskStr, smsk.octet);

            DIAG_UTIL_MPRINTF("%6d| %8s|  %17s | %17s |%9d|%12s%02X|%5d|%5d\n", i, valid ? DIAG_STR_ENABLE : DIAG_STR_DISABLE,\
                macStr, macMskStr, port, "0x", pmsk, vid, priority);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_PROFILE_ENTRY_INDEX
/*
 * vlan get profile entry <UINT:index>
 */
cparser_result_t cparser_cmd_vlan_get_profile_entry_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_profile_t profile;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    char portList[DIAG_UTIL_PORT_MASK_STRING_LEN];
#endif

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_vlan_profile_get(unit, *index_ptr, &profile), ret);

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    DIAG_UTIL_MPRINTF("VLAN profile configuration\n");

    DIAG_UTIL_MPRINTF(" New Src Mac Operation: ");
    if (profile.lrnMode == HARDWARE_LEARNING)
    {
       DIAG_UTIL_MPRINTF("Asic-AUTO-Learn\n");
    }
    else if(profile.lrnMode == SOFTWARE_LEARNING)
    {
       DIAG_UTIL_MPRINTF("Learn-As-SUSPEND\n");
    }
    else if(profile.lrnMode == NOT_LEARNING)
    {
       DIAG_UTIL_MPRINTF("Not-Learn\n");
    }
    else
    {
        DIAG_UTIL_MPRINTF("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF(" L2 new source mac action: %s\n", text_action[profile.l2_newMacAct]);

    DIAG_UTIL_MPRINTF(" IPv4 bridge mode: %s\n", text_bridge_mode[profile.ipmcBdgMode].text);
    DIAG_UTIL_MPRINTF(" IPv6 bridge mode: %s\n", text_bridge_mode[profile.ip6mcBdgMode].text);

    DIAG_UTIL_MPRINTF(" L2 bridge look-up-miss action: %s\n", text_action[profile.l2mc_bdgLuMisAct]);
    diag_util_lPortMask2str(portList, &profile.l2mc_unknFloodPm);
    DIAG_UTIL_MPRINTF(" L2 bridge look-up-miss flooding ports: %s\n", portList);

    DIAG_UTIL_MPRINTF(" IPv4 bridge look-up-miss action: %s\n", text_action[profile.ipmc_bdgLuMisAct]);
    diag_util_lPortMask2str(portList, &profile.ipmc_unknFloodPm);
    DIAG_UTIL_MPRINTF(" IPv4 bridge look-up-miss flooding ports: %s\n", portList);

    DIAG_UTIL_MPRINTF(" IPv6 bridge look up miss action: %s\n", text_action[profile.ip6mc_bdgLuMisAct]);
    diag_util_lPortMask2str(portList, &profile.ip6mc_unknFloodPm);
    DIAG_UTIL_MPRINTF(" IPv6 bridge look-up-miss flooding ports: %s\n", portList);
#endif

#if defined(CONFIG_SDK_RTL8380)|| defined(CONFIG_SDK_RTL8390)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
          DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_MPRINTF(" portmask table index for L2 unknown multicast traffic : %d\n", profile.l2_mcast_dlf_pm_idx);
        DIAG_UTIL_MPRINTF(" portmask table index for IP4 unknown multicast traffic: %d\n", profile.ip4_mcast_dlf_pm_idx);
        DIAG_UTIL_MPRINTF(" portmask table index for IP6 unknown multicast traffic: %d\n", profile.ip6_mcast_dlf_pm_idx);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_INGRESS_FILTER_PORT_PORTS_ALL_ACTION
/*
 * vlan get ingress-filter port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t cparser_cmd_vlan_get_ingress_filter_port_ports_all_action(cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlan_ifilter_t igr_filter;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    DIAG_UTIL_MPRINTF("Ingress filter configuration\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portIgrFilter_get(unit, port, &igr_filter), ret);
        DIAG_UTIL_MPRINTF("Port %u : ", port);
        if (igr_filter == INGRESS_FILTER_FWD)
            DIAG_UTIL_MPRINTF("Forward\n", port);
        else if (igr_filter == INGRESS_FILTER_DROP)
            DIAG_UTIL_MPRINTF("Drop\n", port);
        else
            DIAG_UTIL_MPRINTF("Trap\n", port);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_PVID_MODE_INNER_OUTER_PORT_PORTS_ALL
/*
 * vlan get pvid-mode ( inner | outer ) port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_vlan_get_pvid_mode_inner_outer_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlanType_t type;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_vlan_pbVlan_mode_t mode;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    if ('i' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_MPRINTF("Inner port-based VLAN mode configuration\n");
        type = INNER_VLAN;
    }
    else
    {
        DIAG_UTIL_MPRINTF("Outer port-based VLAN mode configuration\n");
        type = OUTER_VLAN;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if (type == INNER_VLAN)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portInnerPvidMode_get(unit, port, &mode), ret);
            }
            else if (type == OUTER_VLAN)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portOuterPvidMode_get(unit, port, &mode), ret);
            }
        }
        else
#endif
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portPvidMode_get(unit, port, type, &mode), ret);
        }

        DIAG_UTIL_MPRINTF("Port %u : ", port);
        if (mode == PBVLAN_MODE_UNTAG_AND_PRITAG)
            DIAG_UTIL_MPRINTF("Untag and priority tag\n", port);
        else if (mode == PBVLAN_MODE_UNTAG_ONLY)
            DIAG_UTIL_MPRINTF("Untag only\n", port);
        else
            DIAG_UTIL_MPRINTF("All\n", port);

    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_MAC_BASED_VLAN_ENTRY_INDEX
/*
 * vlan get mac-based-vlan entry <UINT:index>
 */
cparser_result_t cparser_cmd_vlan_get_mac_based_vlan_entry_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    char    macStr[16];
#if defined(CONFIG_SDK_RTL8390)
    uint32  valid;
    rtk_mac_t smac, smsk;
    rtk_vlan_t vid;
    rtk_pri_t priority;
    rtk_port_t port, pmsk;
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_vlan_macVlanEntry_t entry;
#endif

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF("MAC-based VLAN configuration\n");

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanWithPort_get(unit, *index_ptr, &valid, &smac, &smsk, &port, &pmsk, &vid, &priority) , ret);

        DIAG_UTIL_MPRINTF("Entry index          : %u\n", *index_ptr);
        DIAG_UTIL_MPRINTF("State                : %s\n", valid ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MAC2STR(macStr, smac.octet);
        DIAG_UTIL_MPRINTF("Source MAC address   : %s\n", macStr);
        DIAG_UTIL_MAC2STR(macStr, smsk.octet);
        DIAG_UTIL_MPRINTF("Source MAC Mask      : %s\n", macStr);
        DIAG_UTIL_MPRINTF("Port ID              : %d\n", port);
        DIAG_UTIL_MPRINTF("Port ID Mask         : 0x%02X\n", pmsk);
        DIAG_UTIL_MPRINTF("VID                  : %u \n", vid);
        DIAG_UTIL_MPRINTF("Priority             : %u\n", priority);
    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_macBasedVlanEntry_get(unit, *index_ptr, &entry) , ret);

        DIAG_UTIL_MPRINTF("Entry index          : %u\n", *index_ptr);
        DIAG_UTIL_MPRINTF("State                : %s\n", entry.valid? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("\nKey:\n");
        DIAG_UTIL_MPRINTF("Inner vlan frame type: %s\n", text_accept_frame_type[entry.ivid_aft].text);
        DIAG_UTIL_MPRINTF("Outer vlan frame type: %s\n", text_accept_frame_type[entry.ovid_aft].text);
        DIAG_UTIL_MAC2STR(macStr, entry.mac.octet);
        DIAG_UTIL_MPRINTF("Source MAC address   : %s\n", macStr);
        DIAG_UTIL_MAC2STR(macStr, entry.mac_care.octet);
        DIAG_UTIL_MPRINTF("Source MAC Mask      : %s\n", macStr);
        DIAG_UTIL_MPRINTF("Port comparing state : %s\n", (entry.port_care == 1)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("%s \t: %d\n", (0 == entry.port_type)? "Port" : "Trunk", entry.port);

        DIAG_UTIL_MPRINTF("\nAction:\n");
        DIAG_UTIL_MPRINTF("Forward action            : %s\n", text_forward_action[entry.fwd_action].text);
        DIAG_UTIL_MPRINTF("Bypass ingress vlan filter: %s\n", entry.igrvlanfilter_ignore ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("Assign VLAN Type          : %s\n", (entry.vlan_type == INNER_VLAN) ? "INNER VLAN" : "OUTER VLAN");
        DIAG_UTIL_MPRINTF("Status assign             : %s\n", text_tag_status[entry.sts_action].text);
        DIAG_UTIL_MPRINTF("Tpid assign               : %s\n", entry.tpid_action? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("Tpid index                : %u\n", entry.tpid_idx);
        DIAG_UTIL_MPRINTF("VID assign                : %s\n", entry.vid_assign ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("VID                       : %u\n", entry.vid_new);
        DIAG_UTIL_MPRINTF("Priority assign           : %s\n", entry.pri_assign ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("Priority                  : %u\n", entry.pri_new);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_MAC_BASED_VLAN_PORT_PORTS_ALL
/*
 * vlan get mac-based-vlan  port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_vlan_get_mac_based_vlan_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_enable_t macBased;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    DIAG_UTIL_MPRINTF("Port configuration of MAC-based VLAN \n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_MPRINTF("Port %u status\t: ", port);

        if ((ret = rtk_vlan_portMacBasedVlanEnable_get(unit, port, &macBased)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        DIAG_UTIL_MPRINTF("%s\n", (macBased == ENABLED) ? DIAG_STR_ENABLE:DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_IP_SUBNET_BASED_VLAN_ENTRY_INDEX
/*
 * vlan get ip-subnet-based-vlan entry <UINT:index>
 */
cparser_result_t cparser_cmd_vlan_get_ip_subnet_based_vlan_entry_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    char    ipv4Str[16];
#if defined(CONFIG_SDK_RTL8390)
    uint32  valid;
    ipaddr_t sip, sip_msk;
    rtk_vlan_t vid;
    rtk_pri_t priority;
    rtk_port_t port, port_msk;
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_vlan_ipSubnetVlanEntry_t entry;
#endif

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF("IP-Subnet-based VLAN configuration\n");

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanWithPort_get(unit, *index_ptr, &valid, &sip, &sip_msk, &port, &port_msk, &vid, &priority) , ret);

        DIAG_UTIL_MPRINTF("Entry index          : %u\n", *index_ptr);
        DIAG_UTIL_MPRINTF("State                : %s\n", valid ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        diag_util_ip2str(ipv4Str, sip);
        DIAG_UTIL_MPRINTF("Source IP address    : %s\n", ipv4Str);
        diag_util_ip2str(ipv4Str, sip_msk);
        DIAG_UTIL_MPRINTF("Source IP Mask       : %s\n", ipv4Str);
        DIAG_UTIL_MPRINTF("Port ID              : %d\n", port);
        DIAG_UTIL_MPRINTF("Port ID Mask         : 0x%02X\n", port_msk);
        DIAG_UTIL_MPRINTF("VID                  : %u \n", vid);
        DIAG_UTIL_MPRINTF("Priority             : %u\n", priority);
    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_ipSubnetBasedVlanEntry_get(unit, *index_ptr, &entry) , ret);

        DIAG_UTIL_MPRINTF("Entry index          : %u\n", *index_ptr);
        DIAG_UTIL_MPRINTF("State                : %s\n", entry.valid? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("\nKey:\n");
        DIAG_UTIL_MPRINTF("Inner vlan frame type: %s\n", text_accept_frame_type[entry.ivid_aft].text);
        DIAG_UTIL_MPRINTF("Outer vlan frame type: %s\n", text_accept_frame_type[entry.ovid_aft].text);
        diag_util_ip2str(ipv4Str, entry.sip);
        DIAG_UTIL_MPRINTF("source IP address    : %s\n", ipv4Str);
        diag_util_ip2str(ipv4Str, entry.sip_care);
        DIAG_UTIL_MPRINTF("Source IP Mask       : %s\n", ipv4Str);
        DIAG_UTIL_MPRINTF("Port comparing state : %s\n", (entry.port_care == 1)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("%s \t: %d\n", (0 == entry.port_type)? "Port" : "Trunk", entry.port);

        DIAG_UTIL_MPRINTF("\nAction:\n");
        DIAG_UTIL_MPRINTF("Forward action            : %s\n", text_forward_action[entry.fwd_action].text);
        DIAG_UTIL_MPRINTF("Bypass ingress vlan filter: %s\n", entry.igrvlanfilter_ignore ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("Assign VLAN Type          : %s\n", (entry.vlan_type == INNER_VLAN) ? "INNER VLAN" : "OUTER VLAN");
        DIAG_UTIL_MPRINTF("Status assign             : %s\n", text_tag_status[entry.sts_action].text);
        DIAG_UTIL_MPRINTF("Tpid assign               : %s\n", entry.tpid_action? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("Tpid index                : %u\n", entry.tpid_idx);
        DIAG_UTIL_MPRINTF("VID assign                : %s\n", entry.vid_assign ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("VID                       : %u\n", entry.vid_new);
        DIAG_UTIL_MPRINTF("Priority assign           : %s\n", entry.pri_assign ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF("Priority                  : %u\n", entry.pri_new);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_IP_SUBNET_BASED_VLAN_PORT_PORTS_ALL

/*
 * vlan get ip-subnet-based-vlan port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_vlan_get_ip_subnet_based_vlan_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port;
    diag_portlist_t  portmask;
    rtk_enable_t ipSubnet;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    DIAG_UTIL_MPRINTF("Port configuration of IP-Subnet-based \n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_MPRINTF("Port %u status\t: ", port);

        if ((ret = rtk_vlan_portIpSubnetBasedVlanEnable_get(unit, port, &ipSubnet)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            continue;
        }
        DIAG_UTIL_MPRINTF("%s\n", (ipSubnet == ENABLED) ? DIAG_STR_ENABLE:DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_INGRESS_BLOCK_MODE
/*
 * vlan get vlan-conversion ingress block-mode
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_conversion_ingress_block_mode(
    cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  i, block_num;
    rtk_vlan_igrVlanCnvtBlk_mode_t  blk_mode;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, block_num, max_num_of_c2sc_blk);
    DIAG_UTIL_MPRINTF("VLAN conversion block configuration\n\n");
    DIAG_UTIL_MPRINTF(" Index | Mode \n");
    DIAG_UTIL_MPRINTF("-------+-----------------------\n");
    for(i=0; i<block_num; i++)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtBlkMode_get(unit, i, &blk_mode) , ret);
        DIAG_UTIL_MPRINTF("%4d   |", i);

        if (blk_mode == CONVERSION_MODE_C2SC)
            DIAG_UTIL_MPRINTF("%s", "Ingress vlan conversion\n");
        else if(blk_mode == CONVERSION_MODE_MAC_BASED)
            DIAG_UTIL_MPRINTF("%s", "MAC-based VLAN\n");
        else if(blk_mode == CONVERSION_MODE_IP_SUBNET_BASED)
            DIAG_UTIL_MPRINTF("%s", "IP-Subnet-based VLAN\n");
        else
            DIAG_UTIL_MPRINTF("%s", "Disabled\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_TPID_INNER_OUTER_EXTRA
/*
 * vlan get tpid ( inner | outer | extra )
 */
cparser_result_t cparser_cmd_vlan_get_tpid_inner_outer_extra(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  i, entry_num;
    uint32  tpid;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if('i' == TOKEN_CHAR(3, 0))
    {
        DIAG_OM_GET_CHIP_CAPACITY(unit, entry_num, max_num_of_cvlan_tpid);
        DIAG_UTIL_MPRINTF("Inner TPID configuration\n");
        for(i=0; i<entry_num; i++)
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_innerTpidEntry_get(unit, i, &tpid) , ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_tpidEntry_get(unit,VLAN_TAG_TYPE_INNER, i, &tpid) , ret);
            }
            DIAG_UTIL_MPRINTF(" TPID %d: 0x%x\n", i, tpid);
        }
    }
    else if('o' == TOKEN_CHAR(3, 0))
    {
        DIAG_OM_GET_CHIP_CAPACITY(unit, entry_num, max_num_of_svlan_tpid);
        DIAG_UTIL_MPRINTF("Outer TPID configuration\n");
        for(i=0; i<entry_num; i++)
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_outerTpidEntry_get(unit, i, &tpid) , ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_tpidEntry_get(unit, VLAN_TAG_TYPE_OUTER, i, &tpid) , ret);
            }
            DIAG_UTIL_MPRINTF(" TPID %d: 0x%x\n", i, tpid);
        }
    }
    else
    {
        DIAG_OM_GET_CHIP_CAPACITY(unit, entry_num, max_num_of_evlan_tpid);
        DIAG_UTIL_MPRINTF("Extra TPID configuration\n");
        for(i=0; i<entry_num; i++)
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_extraTpidEntry_get(unit, i, &tpid) , ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_tpidEntry_get(unit, VLAN_TAG_TYPE_EXTRA, i, &tpid) , ret);
            }

            DIAG_UTIL_MPRINTF(" TPID %d: 0x%x\n", i, tpid);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_INGRESS_ENTRY_INDEX
/*
 * vlan get vlan-conversion ingress entry <UINT:index>
 */
cparser_result_t cparser_cmd_vlan_get_vlan_conversion_ingress_entry_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_vlan_igrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    DIAG_UTIL_MPRINTF("Ingress VLAN conversion entry configuration\n");
    DIAG_UTIL_MPRINTF(" Index : %d\n", *index_ptr);
    DIAG_UTIL_MPRINTF(" Enable state : %s\n", (data.valid == 1)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_MPRINTF(" ------ KEY SECTION ------\n");
        DIAG_UTIL_MPRINTF(" VID comparing state : %s\n", (data.vid_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" VID : %d\n", data.vid);
        DIAG_UTIL_MPRINTF(" Range check result : 0x%x\n", data.rngchk_result);
        DIAG_UTIL_MPRINTF(" Range check result mask : 0x%x\n", data.rngchk_result_mask);
        DIAG_UTIL_MPRINTF(" Priority comparing state : %s\n", (data.priority_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Priority : %d\n", data.priority);
        DIAG_UTIL_MPRINTF(" Port comparing state : %s\n", (data.port_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Port : %d\n", data.port);

        DIAG_UTIL_MPRINTF(" ------ DATA SECTION ------\n");
        DIAG_UTIL_MPRINTF(" VID selection : %s\n", (data.vid_cnvt_sel == 0)? "inner" : "outer");
        DIAG_UTIL_MPRINTF(" VID operation : %s\n", (data.vid_shift == 0)? "force" : "shift");
        DIAG_UTIL_MPRINTF(" New VID : %d\n", data.vid_new);
        DIAG_UTIL_MPRINTF(" Priority operation : %s\n", (data.pri_assign == 1)? "force" : "none");
        DIAG_UTIL_MPRINTF(" New priority : %d\n", data.pri_new);
        DIAG_UTIL_MPRINTF(" Egress inner tag status: %s\n", text_tag_status[data.inner_tag_sts].text);
        DIAG_UTIL_MPRINTF(" Egress outer tag status: %s\n", text_tag_status[data.outer_tag_sts].text);
        DIAG_UTIL_MPRINTF(" TPID operation : %s\n", (data.tpid_assign == 1)? "force" : "none");
        DIAG_UTIL_MPRINTF(" TPID index : %d\n", data.tpid_idx);

    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_MPRINTF(" ------ KEY SECTION ------\n");

        DIAG_UTIL_MPRINTF(" Accept inner vlan frame type : %s\n", text_cnvt_frame_type[data.match_key.inner_tag_sts].text);
        DIAG_UTIL_MPRINTF(" Accept outer vlan frame type : %s\n", text_cnvt_frame_type[data.match_key.outer_tag_sts].text);
        DIAG_UTIL_MPRINTF(" Inner VID comparing state : %s\n", (data.match_key.inner_vid_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Inner VID : %d\n", data.match_key.inner_vid);
        DIAG_UTIL_MPRINTF(" Outer VID comparing state : %s\n", (data.match_key.outer_vid_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Outer VID : %d\n", data.match_key.outer_vid);
        DIAG_UTIL_MPRINTF(" Range check result : 0x%x\n", data.match_key.rngchk_result);
        DIAG_UTIL_MPRINTF(" Range check result mask : 0x%x\n", data.match_key.rngchk_result_mask);
        DIAG_UTIL_MPRINTF(" Inner Priority comparing state : %s\n", (data.match_key.inner_priority_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Inner Priority : %d\n", data.match_key.inner_priority);
        DIAG_UTIL_MPRINTF(" Outer Priority comparing state : %s\n", (data.match_key.outer_priority_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Outer Priority : %d\n", data.match_key.outer_priority);
        DIAG_UTIL_MPRINTF(" Port comparing state : %s\n", (data.match_key.port_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" %s \t: %d\n", (0 == data.match_key.port_type)? "Port" : "Trunk", data.match_key.port);

        DIAG_UTIL_MPRINTF(" ------ DATA SECTION ------\n");
        DIAG_UTIL_MPRINTF(" Inner VID operation : %s\n", text_cnvt_vid_act[data.action.inner_vid_action].text);
        DIAG_UTIL_MPRINTF(" New Inner VID : %d\n", data.action.inner_vid_value);
        DIAG_UTIL_MPRINTF(" Outer VID operation : %s\n", text_cnvt_vid_act[data.action.outer_vid_action].text);
        DIAG_UTIL_MPRINTF(" New Outer VID : %d\n", data.action.outer_vid_value);

        DIAG_UTIL_MPRINTF(" Inner Priority operation : %s\n", text_cnvt_pri_act[data.action.inner_priority_action].text);
        DIAG_UTIL_MPRINTF(" New Inner priority : %d\n", data.action.inner_priority);
        DIAG_UTIL_MPRINTF(" Outer Priority operation : %s\n", text_cnvt_pri_act[data.action.outer_priority_action].text);
        DIAG_UTIL_MPRINTF(" New Outer priority : %d\n", data.action.outer_priority);

        DIAG_UTIL_MPRINTF(" Egress inner tag status: %s\n", text_tag_status[data.action.inner_tag_sts].text);
        DIAG_UTIL_MPRINTF(" Egress outer tag status: %s\n", text_tag_status[data.action.outer_tag_sts].text);

        DIAG_UTIL_MPRINTF(" Inner TPID operation : %s\n", (data.action.inner_tpid_action == 1)? "force" : "none");
        DIAG_UTIL_MPRINTF(" Inner TPID index : %d\n", data.action.inner_tpid_idx);
        DIAG_UTIL_MPRINTF(" Outer TPID operation : %s\n", (data.action.outer_tpid_action == 1)? "force" : "none");
        DIAG_UTIL_MPRINTF(" Outer TPID index : %d\n", data.action.outer_tpid_idx);

#if defined(CONFIG_SDK_RTL9310)
        DIAG_UTIL_MPRINTF(" Interface assign : %s\n", (data.action.intf_id_assign)? "enable" : "disable");
        DIAG_UTIL_MPRINTF(" Interface ID     : %d\n", data.action.intf_id);
#endif
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_EGRESS_DOUBLE_TAG_STATE
/*
 * vlan get vlan-conversion egress double-tag state
 */
cparser_result_t cparser_cmd_vlan_get_vlan_conversion_egress_double_tag_state(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_egrVlanCnvtDblTagEnable_get(unit, &enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("Egress VLAN conversion double tag state : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_EGRESS_VID_SELECT
/*
 * vlan get vlan-conversion egress vid-select
 */
cparser_result_t cparser_cmd_vlan_get_vlan_conversion_egress_vid_select(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_l2_vlanMode_t src;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ((ret = rtk_vlan_egrVlanCnvtVidSource_get(unit, &src)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("Egress VLAN conversion source VLAN : %s\n", (src==BASED_ON_INNER_VLAN) ? "inner" : "outer");

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_EGRESS_ENTRY_INDEX
/*
 * vlan get vlan-conversion egress entry <UINT:index>
 */
cparser_result_t cparser_cmd_vlan_get_vlan_conversion_egress_entry_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_egrVlanCnvtEntry_t data;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_vlan_egrVlanCnvtEntry_get(unit, *index_ptr, &data) , ret);
    DIAG_UTIL_MPRINTF("Egress VLAN conversion entry configuration\n");
    DIAG_UTIL_MPRINTF(" Index : %d\n", *index_ptr);
    DIAG_UTIL_MPRINTF(" Enable state : %s\n", (data.valid == 1)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID)||
            DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_MPRINTF(" ------ KEY SECTION ------\n");
        DIAG_UTIL_MPRINTF(" VID comparing state : %s\n", (data.vid_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" VID : %d\n", data.vid);
        DIAG_UTIL_MPRINTF(" Range check result : 0x%x\n", data.rngchk_result);
        DIAG_UTIL_MPRINTF(" Range check result mask : 0x%x\n", data.rngchk_result_mask);
        DIAG_UTIL_MPRINTF(" Priority comparing state : %s\n", (data.orgpri_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Priority : %d\n", data.orgpri);
        DIAG_UTIL_MPRINTF(" Port comparing state : %s\n", (data.port_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Port : %d\n", data.port);

        DIAG_UTIL_MPRINTF(" ------ DATA SECTION ------\n");
        DIAG_UTIL_MPRINTF(" VID operation : %s\n", (data.vid_shift == 0)? "force" : "shift");
        DIAG_UTIL_MPRINTF(" New VID : %d\n", data.vid_new);
        DIAG_UTIL_MPRINTF(" Priority operation : %s\n", (data.pri_assign == 1)? "force" : "none");
        DIAG_UTIL_MPRINTF(" New priority : %d\n", data.pri_new);

        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            DIAG_UTIL_MPRINTF(" TPID operation : %s\n", (data.itpid_assign == 1)? "force" : "none");
            DIAG_UTIL_MPRINTF(" TPID index : %d\n", data.itpid_idx);
        }
    }
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_MPRINTF(" ------ KEY SECTION ------\n");

        DIAG_UTIL_MPRINTF(" Accept inner vlan frame type : %s\n", text_cnvt_frame_type[data.match_key.inner_tag_sts].text);
        DIAG_UTIL_MPRINTF(" Accept outer vlan frame type : %s\n", text_cnvt_frame_type[data.match_key.outer_tag_sts].text);
        DIAG_UTIL_MPRINTF(" Inner VID comparing state : %s\n", (data.match_key.inner_vid_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Inner VID : %d\n", data.match_key.inner_vid);
        DIAG_UTIL_MPRINTF(" Outer VID comparing state : %s\n", (data.match_key.outer_vid_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Outer VID : %d\n", data.match_key.outer_vid);
        DIAG_UTIL_MPRINTF(" Range check result : 0x%x\n", data.match_key.rngchk_result);
        DIAG_UTIL_MPRINTF(" Range check result mask : 0x%x\n", data.match_key.rngchk_result_mask);
        DIAG_UTIL_MPRINTF(" Inner Priority comparing state : %s\n", (data.match_key.inner_priority_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Inner Priority : %d\n", data.match_key.inner_priority);
        DIAG_UTIL_MPRINTF(" Outer Priority comparing state : %s\n", (data.match_key.outer_priority_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" Outer Priority : %d\n", data.match_key.outer_priority);
        DIAG_UTIL_MPRINTF(" Port comparing state : %s\n", (data.match_key.port_ignore == 0)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        DIAG_UTIL_MPRINTF(" %s \t: %d\n", (0 == data.match_key.port_type)? "Port" : "Trunk", data.match_key.port);

        DIAG_UTIL_MPRINTF(" ------ DATA SECTION ------\n");

        DIAG_UTIL_MPRINTF(" Inner VID operation : %s\n", text_cnvt_vid_act[data.action.inner_vid_action].text);
        DIAG_UTIL_MPRINTF(" New Inner VID : %d\n", data.action.inner_vid_value);
        DIAG_UTIL_MPRINTF(" Outer VID operation : %s\n", text_cnvt_vid_act[data.action.outer_vid_action].text);
        DIAG_UTIL_MPRINTF(" New Outer VID : %d\n", data.action.outer_vid_value);

        DIAG_UTIL_MPRINTF(" Inner Priority operation : %s\n", text_cnvt_pri_act[data.action.inner_priority_action].text);
        DIAG_UTIL_MPRINTF(" New Inner priority : %d\n", data.action.inner_priority);
        DIAG_UTIL_MPRINTF(" Outer Priority operation : %s\n", text_cnvt_pri_act[data.action.outer_priority_action].text);
        DIAG_UTIL_MPRINTF(" New Outer priority : %d\n", data.action.outer_priority);

        DIAG_UTIL_MPRINTF(" Egress inner tag status: %s\n", text_tag_status[data.action.inner_tag_sts].text);
        DIAG_UTIL_MPRINTF(" Egress outer tag status: %s\n", text_tag_status[data.action.outer_tag_sts].text);

        DIAG_UTIL_MPRINTF(" Inner TPID operation : %s\n", (data.action.inner_tpid_action == 1)? "force" : "none");
        DIAG_UTIL_MPRINTF(" Inner TPID index : %d\n", data.action.inner_tpid_idx);
        DIAG_UTIL_MPRINTF(" Outer TPID operation : %s\n", (data.action.outer_tpid_action == 1)? "force" : "none");
        DIAG_UTIL_MPRINTF(" Outer TPID index : %d\n", data.action.outer_tpid_idx);

    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_AGGREGATION_PORT_PORTS_ALL_STATE
/*
 * vlan get vlan-aggregation port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_aggregation_port_ports_all_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    uint32          unit = 0;
    rtk_enable_t    enable;
    rtk_port_t      port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF("VLAN aggregation configuration \n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrEnable_get(unit, port, &enable), ret);
        DIAG_UTIL_MPRINTF("\tPort %d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_aggregation_port_ports_all_state */
#endif

#ifdef CMD_VLAN_GET_VLAN_AGGREGATION_TRUNK_TRUNKS_ALL_STATE
/*
 * vlan get vlan-aggregation trunk ( <MASK_LIST:trunks> | all ) state
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_aggregation_trunk_trunks_all_state(
    cparser_context_t *context,
    char **trunks_ptr)
{
    int32           ret = RT_ERR_FAILED;
    uint32          unit = 0;
    rtk_enable_t    enable;
    rtk_trk_t       trunk;
    diag_trklist_t  trklist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF("VLAN aggregation configuration \n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

    DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_trkVlanAggrEnable_get(unit, trunk, &enable), ret);
        DIAG_UTIL_MPRINTF("\tTrunk %d : %s\n", trunk, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_aggregation_trunk_trunks_all_state */
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_PORT_PORTS_ALL_RANGE_CHECK_SET_ID
/*
 * vlan set vlan-conversion ingress port ( <PORT_LIST:ports> | all ) range-check <UINT:set_id>
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_conversion_ingress_port_ports_all_range_check_set_id(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *set_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    diag_portlist_t  portmask;
    int32   port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portIgrVlanCnvtRangeCheckSet_set(unit, port, *set_id_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_INGRESS_RANGE_CHECK_SET_ID_ENTRY_INDEX_VID
/*
 * vlan get vlan-conversion ingress range-check <UINT:set_id> <UINT:entry_index> vid
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_conversion_ingress_range_check_set_id_entry_index_vid(
    cparser_context_t *context,
    uint32_t *set_id_ptr,
    uint32_t *entry_index_ptr)
{
    uint32                                  unit;
    int32                                   ret;
    rtk_vlan_vlanCnvtRangeCheck_vid_t       vid_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&vid_range, 0x00, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));

    ret = rtk_vlan_igrVlanCnvtRangeCheckEntry_get(unit, *set_id_ptr, *entry_index_ptr, &vid_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("VLAN ingress conversion vid range check set %d index %d:\n",
        *set_id_ptr,
        *entry_index_ptr);

    DIAG_UTIL_MPRINTF("\tType: ");
    if (INNER_VLAN == vid_range.vid_type)
    {
        DIAG_UTIL_MPRINTF("Inner\n");
    }
    else
    {
        DIAG_UTIL_MPRINTF("Outer\n");
    }

    DIAG_UTIL_MPRINTF("\tLower: %d\n", vid_range.vid_lower_bound);
    DIAG_UTIL_MPRINTF("\tUpper: %d\n", vid_range.vid_upper_bound);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_RANGE_CHECK_SET_ID_ENTRY_INDEX_VID_INNER_OUTER_LOWER_UPPER
/*
 * vlan set vlan-conversion ingress range-check <UINT:set_id> <UINT:entry_index> vid ( inner | outer ) <UINT:lower> <UINT:upper>
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_conversion_ingress_range_check_set_id_entry_index_vid_inner_outer_lower_upper(
    cparser_context_t *context,
    uint32_t *set_id_ptr,
    uint32_t *entry_index_ptr,
    uint32_t *lower_ptr,
    uint32_t *upper_ptr)
{
    uint32                                  unit;
    int32                                   ret;
    rtk_vlanType_t                          type;
    rtk_vlan_vlanCnvtRangeCheck_vid_t       vid_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(8, 0))
    {
        type = INNER_VLAN;
    }
    else
    {
        type = OUTER_VLAN;
    }

    osal_memset(&vid_range, 0x00, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));
    vid_range.vid_upper_bound   = *upper_ptr;
    vid_range.vid_lower_bound   = *lower_ptr;
    vid_range.vid_type          = type;
    ret = rtk_vlan_igrVlanCnvtRangeCheckEntry_set(unit, *set_id_ptr, *entry_index_ptr, &vid_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_PORT_PORTS_ALL_RANGE_CHECK_SET_ID
/*
 * vlan set vlan-conversion egress port ( <PORT_LIST:ports> | all ) range-check <UINT:set_id>
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_conversion_egress_port_ports_all_range_check_set_id(
    cparser_context_t *context,
    char **ports_ptr,
    uint32_t *set_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    diag_portlist_t  portmask;
    int32   port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portEgrVlanCnvtRangeCheckSet_set(unit, port, *set_id_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_EGRESS_RANGE_CHECK_SET_ID_ENTRY_INDEX_VID
/*
 * vlan get vlan-conversion egress range-check <UINT:set_id> <UINT:entry_index> vid
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_conversion_egress_range_check_set_id_entry_index_vid(
    cparser_context_t *context,
    uint32_t *set_id_ptr,
    uint32_t *entry_index_ptr)
{
    uint32                                  unit;
    int32                                   ret;
    rtk_vlan_vlanCnvtRangeCheck_vid_t       vid_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&vid_range, 0x00, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));

    ret = rtk_vlan_egrVlanCnvtRangeCheckEntry_get(unit, *set_id_ptr, *entry_index_ptr, &vid_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("VLAN egress conversion vid range check set %d index %d:\n",
        *set_id_ptr,
        *entry_index_ptr);

    DIAG_UTIL_MPRINTF("\tType: ");
    if (INNER_VLAN == vid_range.vid_type)
    {
        DIAG_UTIL_MPRINTF("Inner\n");
    }
    else
    {
        DIAG_UTIL_MPRINTF("Outer\n");
    }

    DIAG_UTIL_MPRINTF("\tLower: %d\n", vid_range.vid_lower_bound);
    DIAG_UTIL_MPRINTF("\tUpper: %d\n", vid_range.vid_upper_bound);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_RANGE_CHECK_SET_ID_ENTRY_INDEX_VID_INNER_OUTER_LOWER_UPPER
/*
 * vlan set vlan-conversion egress range-check <UINT:set_id> <UINT:entry_index> vid ( inner | outer ) <UINT:lower> <UINT:upper>
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_conversion_egress_range_check_set_id_entry_index_vid_inner_outer_lower_upper(
    cparser_context_t *context,
    uint32_t *set_id_ptr,
    uint32_t *entry_index_ptr,
    uint32_t *lower_ptr,
    uint32_t *upper_ptr)
{
    uint32                                  unit;
    int32                                   ret;
    rtk_vlanType_t                          type;
    rtk_vlan_vlanCnvtRangeCheck_vid_t       vid_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(8, 0))
    {
        type = INNER_VLAN;
    }
    else
    {
        type = OUTER_VLAN;
    }

    osal_memset(&vid_range, 0x00, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));

    ret = rtk_vlan_egrVlanCnvtRangeCheckEntry_get(unit, *set_id_ptr, *entry_index_ptr, &vid_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    vid_range.vid_upper_bound   = *upper_ptr;
    vid_range.vid_lower_bound   = *lower_ptr;
    vid_range.vid_type          = type;
    ret = rtk_vlan_egrVlanCnvtRangeCheckEntry_set(unit, *set_id_ptr, *entry_index_ptr, &vid_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_EGRESS_RANGE_CHECK_ENTRY_ENTRY_INDEX_VID_REVERSE_STATE
/*
 * vlan get vlan-conversion egress range-check entry <UINT:entry_index> vid reverse state
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_conversion_egress_range_check_entry_entry_index_vid_reverse_state(
    cparser_context_t *context,
    uint32_t *entry_index_ptr)
{
    uint32                                  unit;
    int32                                   ret;
    rtk_vlan_vlanCnvtRangeCheck_vid_t       vid_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&vid_range, 0x00, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));

    ret = rtk_vlan_egrVlanCnvtRangeCheckVid_get(unit, *entry_index_ptr, &vid_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF("VLAN egress conversion vid range check index %d:\n",
            *entry_index_ptr);

    DIAG_UTIL_MPRINTF("\tReverse: ");
    if (0 == vid_range.reverse)
    {
        DIAG_UTIL_MPRINTF("%s\n", DIAG_STR_DISABLE);
    }
    else
    {
        DIAG_UTIL_MPRINTF("%s\n",DIAG_STR_ENABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_RANGE_CHECK_ENTRY_ENTRY_INDEX_VID_REVERSE_STATE_DISABLE_ENABLE
/*
 * vlan set vlan-conversion egress range-check entry <UINT:entry_index> vid reverse state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_conversion_egress_range_check_entry_entry_index_vid_reverse_state_disable_enable(
    cparser_context_t *context,
    uint32_t *entry_index_ptr)
{
    uint32                                  unit;
    int32                                   ret;
    rtk_vlan_vlanCnvtRangeCheck_vid_t       vid_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&vid_range, 0, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));

    ret = rtk_vlan_egrVlanCnvtRangeCheckVid_get(unit, *entry_index_ptr, &vid_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        vid_range.reverse = 1;
    }
    else
    {
        vid_range.reverse = 0;
    }

    ret = rtk_vlan_egrVlanCnvtRangeCheckVid_set(unit, *entry_index_ptr, &vid_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_TABLE_HASH_MODE_UNICAST_MULTICAST_VID_VID
/*
 * vlan get vlan-table hash-mode ( unicast | multicast ) vid <UINT:vid>
 */
cparser_result_t cparser_cmd_vlan_get_vlan_table_hash_mode_unicast_multicast_vid_vid(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_l2mactype_t type;
    rtk_vlan_l2LookupMode_t mode;
#if defined(CONFIG_SDK_RTL9310)
    rtk_vlan_svlMode_t svlMode;
    rtk_fid_t svlFid;
#endif /* CONFIG_SDK_RTL9310 */
    uint32 val = 0;
    mode = VLAN_L2_LOOKUP_MODE_VID;
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if('u' == TOKEN_CHAR(4, 0))
        type = VLAN_L2_MAC_TYPE_UC;
    else
        type = VLAN_L2_MAC_TYPE_MC;

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if (type == VLAN_L2_MAC_TYPE_UC)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_l2UcastLookupMode_get(unit, *vid_ptr, &mode), ret);
        }
        else if (type == VLAN_L2_MAC_TYPE_MC)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_l2McastLookupMode_get(unit, *vid_ptr, &mode), ret);
        }
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_l2LookupMode_get(unit, *vid_ptr, type, &mode), ret);
    }

    DIAG_UTIL_MPRINTF("VLAN learning method: ");

    if (VLAN_L2_LOOKUP_MODE_VID == mode)
        DIAG_UTIL_MPRINTF("IVL\n");
    else
        DIAG_UTIL_MPRINTF("SVL, ");

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_stg_get(unit, *vid_ptr, &val), ret);
        DIAG_UTIL_MPRINTF("FID/MSTI: %d\n", val);
    }
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        if (VLAN_L2_LOOKUP_MODE_FID == mode)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_l2LookupSvlFid_get(unit, type, &val), ret);
            DIAG_UTIL_MPRINTF("FID: %d\n", val);
        }
    }
#endif

#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_svlMode_get(unit, &svlMode), ret);

        if (VLAN_SVL_MODE_FID_MAC_TYPE == svlMode)
        {
            if (VLAN_L2_LOOKUP_MODE_FID == mode)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_l2LookupSvlFid_get(unit, type, &val), ret);
                DIAG_UTIL_MPRINTF("FID: %d (MAC-type mode)\n", val);
            }
        }
        else if (VLAN_SVL_MODE_FID_VLAN == svlMode)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_svlFid_get(unit, *vid_ptr, &svlFid), ret);
            DIAG_UTIL_MPRINTF("FID: %d (VLAN mode)\n", svlFid);
        }
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_HASH_MODE_IVL_UNICAST_MULTICAST_VID_VID
/*
 * vlan set vlan-table hash-mode ivl ( unicast | multicast ) vid <UINT:vid>
 */
cparser_result_t cparser_cmd_vlan_set_vlan_table_hash_mode_ivl_unicast_multicast_vid_vid(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_l2mactype_t macType;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('u' == TOKEN_CHAR(5, 0))
        macType = VLAN_L2_MAC_TYPE_UC;
    else
        macType = VLAN_L2_MAC_TYPE_MC;

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if (macType == VLAN_L2_MAC_TYPE_UC)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_l2UcastLookupMode_set(unit, *vid_ptr,VLAN_L2_LOOKUP_MODE_VID),ret);
        }
        else if (macType == VLAN_L2_MAC_TYPE_MC)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_l2McastLookupMode_set(unit, *vid_ptr, VLAN_L2_LOOKUP_MODE_VID),ret);
        }
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_l2LookupMode_set(unit, *vid_ptr, macType, VLAN_L2_LOOKUP_MODE_VID),ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_HASH_MODE_SVL_UNICAST_MULTICAST_FID_FID
/*
 * vlan set vlan-table hash-mode svl ( unicast | multicast ) fid <UINT:fid>
 */
cparser_result_t cparser_cmd_vlan_set_vlan_table_hash_mode_svl_unicast_multicast_fid_fid(cparser_context_t *context,
    uint32_t *fid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_l2mactype_t macType;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('u' == TOKEN_CHAR(5, 0))
        macType = VLAN_L2_MAC_TYPE_UC;
    else
        macType = VLAN_L2_MAC_TYPE_MC;


    DIAG_UTIL_ERR_CHK(rtk_vlan_l2LookupSvlFid_set(unit, macType, *fid_ptr),ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_HASH_MODE_SVL_UNICAST_MULTICAST_VID_VID
/*
 * vlan set vlan-table hash-mode svl ( unicast | multicast ) vid <UINT:vid>
 */
cparser_result_t cparser_cmd_vlan_set_vlan_table_hash_mode_svl_unicast_multicast_vid_vid(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_l2mactype_t macType;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('u' == TOKEN_CHAR(5, 0))
        macType = VLAN_L2_MAC_TYPE_UC;
    else
        macType = VLAN_L2_MAC_TYPE_MC;

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if (macType == VLAN_L2_MAC_TYPE_UC)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_l2UcastLookupMode_set(unit, *vid_ptr, VLAN_L2_LOOKUP_MODE_FID), ret);
        }
        else if (macType == VLAN_L2_MAC_TYPE_MC)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_l2McastLookupMode_set(unit, *vid_ptr, VLAN_L2_LOOKUP_MODE_FID), ret);
        }
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_l2LookupMode_set(unit, *vid_ptr, macType, VLAN_L2_LOOKUP_MODE_FID), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_TABLE_HASH_MODE_SVL_UNICAST_MULTICAST_VID_VID_FID_MSTI_FID_MSTI
/*
 * vlan set vlan-table hash-mode svl ( unicast | multicast ) vid <UINT:vid> fid_msti <UINT:fid_msti>
 */
cparser_result_t cparser_cmd_vlan_set_vlan_table_hash_mode_svl_unicast_multicast_vid_vid_fid_msti_fid_msti(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *fid_msti_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_l2mactype_t macType;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if('u' == TOKEN_CHAR(5, 0))
        macType = VLAN_L2_MAC_TYPE_UC;
    else
        macType = VLAN_L2_MAC_TYPE_MC;

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if (macType == VLAN_L2_MAC_TYPE_UC)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_l2UcastLookupMode_set(unit, *vid_ptr, VLAN_L2_LOOKUP_MODE_FID), ret);
        }
        else if (macType == VLAN_L2_MAC_TYPE_MC)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_l2McastLookupMode_set(unit, *vid_ptr, VLAN_L2_LOOKUP_MODE_FID), ret);
        }
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_l2LookupMode_set(unit, *vid_ptr, macType, VLAN_L2_LOOKUP_MODE_FID),ret);
    }

    DIAG_UTIL_ERR_CHK(rtk_vlan_stg_set(unit, *vid_ptr, *fid_msti_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_INGRESS_PORT_PORTS_ALL_INNER_OUTER_KEEP_TAG_KEEPTYPE_NOKEEP_FORMAT_CONTENT
/*
 * vlan set ingress port ( <PORT_LIST:ports> | all ) ( inner | outer ) keep-tag keeptype ( nokeep | format | content )
 */
cparser_result_t cparser_cmd_vlan_set_ingress_port_ports_all_inner_outer_keep_tag_keeptype_nokeep_format_content(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    int32           dir;    /* 0: inner, 1: outer */
    rtk_vlan_tagKeepType_t  inner_keeptype, outer_keeptype, new_keeptype;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        dir = 0;
    }
    else
    {
        dir = 1;
    }

    if ('n' == TOKEN_CHAR(8, 0))
    {
        new_keeptype = TAG_KEEP_TYPE_NOKEEP;
    }
    else if('f' == TOKEN_CHAR(8,0))
    {
        new_keeptype = TAG_KEEP_TYPE_FORMAT;
    }
    else
    {
        new_keeptype = TAG_KEEP_TYPE_CONTENT;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        ret = rtk_vlan_portIgrTagKeepType_get(unit, port, &outer_keeptype,
                &inner_keeptype);
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        if (0 == dir)
            inner_keeptype = new_keeptype;
        else
            outer_keeptype = new_keeptype;

        ret = rtk_vlan_portIgrTagKeepType_set(unit, port, outer_keeptype,
                inner_keeptype);
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_EGRESS_PORT_PORTS_ALL_INNER_OUTER_KEEP_TAG_KEEPTYPE_NOKEEP_FORMAT_CONTENT
/*
 * vlan set egress port ( <PORT_LIST:ports> | all ) ( inner | outer ) keep-tag keeptype ( nokeep | format | content )
 */
cparser_result_t cparser_cmd_vlan_set_egress_port_ports_all_inner_outer_keep_tag_keeptype_nokeep_format_content(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    int32           dir;    /* 0: inner, 1: outer */
    rtk_vlan_tagKeepType_t  inner_keeptype, outer_keeptype, new_keeptype;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        dir = 0;
    }
    else
    {
        dir = 1;
    }

    if ('n' == TOKEN_CHAR(8, 0))
    {
        new_keeptype = TAG_KEEP_TYPE_NOKEEP;
    }
    else if('f' == TOKEN_CHAR(8,0))
    {
        new_keeptype = TAG_KEEP_TYPE_FORMAT;
    }
    else
    {
        new_keeptype = TAG_KEEP_TYPE_CONTENT;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        ret = rtk_vlan_portEgrTagKeepType_get(unit, port, &outer_keeptype,
                &inner_keeptype);
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        if (0 == dir)
            inner_keeptype = new_keeptype;
        else
            outer_keeptype = new_keeptype;

        ret = rtk_vlan_portEgrTagKeepType_set(unit, port, outer_keeptype,
                inner_keeptype);
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_INGRESS_PORT_PORTS_ALL_FWD_VLAN_TAG_MODE_UNTAG_INNER_TAG_OUTER_TAG_DOUBLE_TAG_ALL_BY_INNER_OUTER
/*
 * vlan set ingress port ( <PORT_LIST:ports> | all ) fwd-vlan tag-mode ( untag | inner-tag | outer-tag | double-tag | all ) by ( inner | outer )
 */
cparser_result_t
cparser_cmd_vlan_set_ingress_port_ports_all_fwd_vlan_tag_mode_untag_inner_tag_outer_tag_double_tag_all_by_inner_outer(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_pktTagMode_t tagMode;
    rtk_vlanType_t vlanType;
    diag_portlist_t portmask;
    rtk_port_t      port;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    if ('u' == TOKEN_CHAR(7, 0))
        tagMode = VLAN_PKT_TAG_MODE_UNTAG;
    else if ('i' == TOKEN_CHAR(7, 0))
        tagMode = VLAN_PKT_TAG_MODE_INNERTAG;
    else if ('o' == TOKEN_CHAR(7, 0))
        tagMode = VLAN_PKT_TAG_MODE_OUTERTAG;
    else if ('d' == TOKEN_CHAR(7, 0))
        tagMode = VLAN_PKT_TAG_MODE_DBLTAG;
    else
        tagMode = VLAN_PKT_TAG_MODE_ALL;

    vlanType = ('i' == TOKEN_CHAR(9, 0))? INNER_VLAN : OUTER_VLAN;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portFwdVlan_set(unit, port, tagMode, vlanType), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_ingress_port_ports_all_fwd_vlan_tag_mode_untag_inner_tag_outer_tag_double_tag_all_by_inner_outer */
#endif

#ifdef CMD_VLAN_SET_INGRESS_PORT_PORTS_ALL_PRIVATE_VLAN_STATE_DISABLE_ENABLE
/*
 * vlan set ingress port ( <PORT_LIST:ports> | all ) private-vlan state ( disable | enable )
 */
cparser_result_t
cparser_cmd_vlan_set_ingress_port_ports_all_private_vlan_state_disable_enable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    diag_portlist_t portmask;
    rtk_port_t      port;
    rtk_enable_t    enable;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 4), ret);

    enable = ('e' == TOKEN_CHAR(7, 0))? ENABLED : DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_portPrivateVlanEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_ingress_port_ports_all_private_vlan_state_disable_enable */
#endif

#ifdef CMD_VLAN_SET_VLAN_AGGREGATION_PORT_PORTS_ALL_VID_SOURCE_INNER_OUTER
/*
 * vlan set vlan-aggregation port ( <PORT_LIST:ports> | all ) vid-source ( inner | outer )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_aggregation_port_ports_all_vid_source_inner_outer(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlanType_t    src;
    rtk_vlan_aggrCtrl_t vlanAggrCtrl;
    rtk_port_t      port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('i' == TOKEN_CHAR(6, 0))
    {
        src = BASED_ON_INNER_VLAN;
    }
    else
    {
        src = BASED_ON_OUTER_VLAN;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrVidSource_set(unit, port, src) , ret);
        }
        else
#endif
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrCtrl_get(unit, port, &vlanAggrCtrl), ret);
            vlanAggrCtrl.vid_src = src;
            DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrCtrl_set(unit, port, vlanAggrCtrl), ret);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_aggregation_port_ports_all_vid_source_inner_outer */
#endif

#ifdef CMD_VLAN_SET_VLAN_AGGREGATION_TRUNK_TRUNKS_ALL_VID_SOURCE_INNER_OUTER
/*
 * vlan set vlan-aggregation trunk ( <MASK_LIST:trunks> | all ) vid-source ( inner | outer )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_aggregation_trunk_trunks_all_vid_source_inner_outer(
    cparser_context_t *context,
    char **trunks_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlanType_t    src;
    rtk_vlan_aggrCtrl_t vlanAggrCtrl;
    rtk_trk_t       trunk;
    diag_trklist_t  trklist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('i' == TOKEN_CHAR(6, 0))
    {
        src = BASED_ON_INNER_VLAN;
    }
    else
    {
        src = BASED_ON_OUTER_VLAN;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

    DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_trkVlanAggrCtrl_get(unit, trunk, &vlanAggrCtrl), ret);
        vlanAggrCtrl.vid_src = src;
        DIAG_UTIL_ERR_CHK(rtk_vlan_trkVlanAggrCtrl_set(unit, trunk, vlanAggrCtrl), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_aggregation_trunk_trunks_all_vid_source_inner_outer */
#endif

#ifdef CMD_VLAN_GET_VLAN_AGGREGATION_PORT_PORTS_ALL_VID_SOURCE
/*
 * vlan get vlan-aggregation port ( <PORT_LIST:ports> | all ) vid-source
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_aggregation_port_ports_all_vid_source(
    cparser_context_t *context,
    char **ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    uint32          unit = 0;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    rtk_vlanType_t  src;
#endif
    rtk_vlan_aggrCtrl_t vlanAggrCtrl;
    rtk_port_t      port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF("VLAN aggregation configuration \n");

        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrVidSource_get(unit, port, &src), ret);
                DIAG_UTIL_MPRINTF("\tPort %d : Source Selection is %s\n", port, src ? "Outer" : "Inner");
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrCtrl_get(unit, port, &vlanAggrCtrl), ret);
                DIAG_UTIL_MPRINTF("\tPort %d : Source Selection is %s\n", port, vlanAggrCtrl.vid_src ? "Outer" : "Inner");
            }
        }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_aggregation_port_ports_all_vid_source */
#endif

#ifdef CMD_VLAN_GET_VLAN_AGGREGATION_TRUNK_TRUNKS_ALL_VID_SOURCE
/*
 * vlan get vlan-aggregation trunk ( <MASK_LIST:trunks> | all ) vid-source
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_aggregation_trunk_trunks_all_vid_source(
    cparser_context_t *context,
    char **trunks_ptr)
{
    int32           ret = RT_ERR_FAILED;
    uint32          unit = 0;
    rtk_vlan_aggrCtrl_t vlanAggrCtrl;
    rtk_trk_t       trunk;
    diag_trklist_t  trklist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF("VLAN aggregation configuration \n");

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

    DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_trkVlanAggrCtrl_get(unit, trunk, &vlanAggrCtrl), ret);
        DIAG_UTIL_MPRINTF("\tTrunk %d : Source Selection is %s\n", trunk, vlanAggrCtrl.vid_src ? "Outer" : "Inner");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_aggregation_trunk_trunks_all_vid_source */
#endif

#ifdef CMD_VLAN_SET_VLAN_AGGREGATION_PORT_PORTS_ALL_PRI_TAG_VID_SOURCE_PRIORITY_VID_PORT_BASED_VID
/*
 * vlan set vlan-aggregation port ( <PORT_LIST:ports> | all ) pri-tag vid-source ( priority-vid | port-based-vid )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_aggregation_port_ports_all_pri_tag_vid_source_priority_vid_port_based_vid(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_priTagVidSrc_t    src;
    rtk_vlan_aggrCtrl_t vlanAggrCtrl;
    rtk_port_t      port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('r' == TOKEN_CHAR(7,1))
    {
        src = LEARNING_VID_PRI;
    }
    else if ('o' == TOKEN_CHAR(7,1))
    {
        src = LEARNING_VID_PBASED;
    }
    else
    {
        src = CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrPriTagVidSource_set(unit, port, src) , ret);
        }
        else
#endif
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrCtrl_get(unit, port, &vlanAggrCtrl), ret);
            vlanAggrCtrl.pri_src = src;
            DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrCtrl_set(unit, port, vlanAggrCtrl), ret);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_aggregation_port_ports_all_pri_tag_vid_source_priority_vid_port_based_vid */
#endif

#ifdef CMD_VLAN_SET_VLAN_AGGREGATION_TRUNK_TRUNKS_ALL_PRI_TAG_VID_SOURCE_PRIORITY_VID_PORT_BASED_VID
/*
 * vlan set vlan-aggregation trunk ( <MASK_LIST:trunks> | all ) pri-tag vid-source ( priority-vid | port-based-vid )
 */
cparser_result_t
cparser_cmd_vlan_set_vlan_aggregation_trunk_trunks_all_pri_tag_vid_source_priority_vid_port_based_vid(
    cparser_context_t *context,
    char **trunks_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_vlan_priTagVidSrc_t    src;
    rtk_vlan_aggrCtrl_t vlanAggrCtrl;
    rtk_trk_t       trunk;
    diag_trklist_t  trklist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('r' == TOKEN_CHAR(7,1))
    {
        src = LEARNING_VID_PRI;
    }
    else if ('o' == TOKEN_CHAR(7,1))
    {
        src = LEARNING_VID_PBASED;
    }
    else
    {
        src = CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

    DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_trkVlanAggrCtrl_get(unit, trunk, &vlanAggrCtrl), ret);
        vlanAggrCtrl.pri_src = src;
        DIAG_UTIL_ERR_CHK(rtk_vlan_trkVlanAggrCtrl_set(unit, trunk, vlanAggrCtrl), ret);

    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vlan_aggregation_trunk_trunks_all_pri_tag_vid_source_priority_vid_port_based_vid */
#endif

#ifdef CMD_VLAN_GET_VLAN_AGGREGATION_PORT_PORTS_ALL_PRI_TAG_VID_SOURCE
/*
 * vlan get vlan-aggregation port ( <PORT_LIST:ports> | all ) pri-tag vid-source
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_aggregation_port_ports_all_pri_tag_vid_source(
    cparser_context_t *context,
    char **ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    uint32          unit = 0;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    rtk_vlan_priTagVidSrc_t  src;
#endif
    rtk_vlan_aggrCtrl_t vlanAggrCtrl;
    rtk_port_t      port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrPriTagVidSource_get(unit, port, &src), ret);
            DIAG_UTIL_MPRINTF("\tPort %d : Priority VID Source Slection is %s\n", port, src ? "port-based VID" : "priority VID");
        }
        else
#endif
        {
            DIAG_UTIL_ERR_CHK(rtk_vlan_portVlanAggrCtrl_get(unit, port, &vlanAggrCtrl), ret);
            DIAG_UTIL_MPRINTF("\tPort %d : Priority VID Source Slection is %s\n", port, vlanAggrCtrl.pri_src ? "port-based VID" : "priority VID");
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_aggregation_port_ports_all_pri_tag_vid_source */
#endif

#ifdef CMD_VLAN_GET_VLAN_AGGREGATION_TRUNK_TRUNKS_ALL_PRI_TAG_VID_SOURCE
/*
 * vlan get vlan-aggregation trunk ( <MASK_LIST:trunks> | all ) pri-tag vid-source
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_aggregation_trunk_trunks_all_pri_tag_vid_source(
    cparser_context_t *context,
    char **trunks_ptr)
{
    int32           ret = RT_ERR_FAILED;
    uint32          unit = 0;
    rtk_vlan_aggrCtrl_t vlanAggrCtrl;
    rtk_trk_t       trunk;
    diag_trklist_t  trklist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

    DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
    {
        DIAG_UTIL_ERR_CHK(rtk_vlan_trkVlanAggrCtrl_get(unit, trunk, &vlanAggrCtrl), ret);
        DIAG_UTIL_MPRINTF("\tTrunk %d : Priority VID Source Slection is %s\n", trunk, vlanAggrCtrl.pri_src ? "port-based VID" : "priority VID");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_aggregation_trunk_trunks_all_pri_tag_vid_source */
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_PORT_PORTS_ALL_VID_SOURCE_INNER_OUTER
/*
 * vlan set vlan-conversion egress port ( <PORT_LIST:ports> | all ) vid-source ( inner | outer )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_port_ports_all_vid_source_inner_outer(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_vlanMode_t   src;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    if('i' == TOKEN_CHAR(7, 0))
    {
        src = BASED_ON_INNER_VLAN;
    }
    else
    {
        src = BASED_ON_OUTER_VLAN;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_vlan_portEgrVlanCnvtVidSource_set(unit, port, src)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_EGRESS_PORT_PORTS_ALL_VID_SOURCE
/*
 * vlan get vlan-conversion egress port ( <PORT_LIST:ports> | all ) vid-source
 */
cparser_result_t cparser_cmd_vlan_get_vlan_conversion_egress_port_ports_all_vid_source(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_vlanMode_t   src;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("VLAN Egress VLAN Translation configuration \n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : ", port);

        if ((ret = rtk_vlan_portEgrVlanCnvtVidSource_get(unit, port, &src)) == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("Source Slection is %s\n", src ? "Outer" : "Inner");
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_PORT_PORTS_ALL_VID_TARGET_INNER_OUTER
/*
 * vlan set vlan-conversion egress port ( <PORT_LIST:ports> | all ) vid-target ( inner | outer )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_port_ports_all_vid_target_inner_outer(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_vlanMode_t   tgt;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    if('i' == TOKEN_CHAR(7, 0))
    {
        tgt = BASED_ON_INNER_VLAN;
    }
    else
    {
        tgt = BASED_ON_OUTER_VLAN;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_vlan_portEgrVlanCnvtVidTarget_set(unit, port, tgt)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_EGRESS_PORT_PORTS_ALL_VID_TARGET
/*
 * vlan get vlan-conversion egress port ( <PORT_LIST:ports> | all ) vid-target
 */
cparser_result_t cparser_cmd_vlan_get_vlan_conversion_egress_port_ports_all_vid_target(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_vlanMode_t   tgt;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("VLAN Egress VLAN Translation configuration \n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : ", port);

        if ((ret = rtk_vlan_portEgrVlanCnvtVidTarget_get(unit, port, &tgt)) == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("Target Slection is %s\n", tgt ? "Outer" : "Inner");
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_INGRESS_PORT_PORTS_ALL_INNER_OUTER_LOOKUP_MISS_ACTION_FORWARD_DROP
/*
 * vlan set vlan-conversion ingress port ( <PORT_LIST:ports> | all ) ( inner | outer ) lookup-miss-action ( forward | drop )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_ingress_port_ports_all_inner_outer_lookup_miss_action_forward_drop(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;
    rtk_vlanType_t  type;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    if('i' == TOKEN_CHAR(6, 0))
        type = INNER_VLAN;
    else
        type = OUTER_VLAN;

    if('d' == TOKEN_CHAR(8, 0))
        action = LOOKUPMISS_DROP;
    else
        action = LOOKUPMISS_FWD;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        if ((ret = rtk_vlan_portIgrVlanCnvtLuMisAct_set(unit, port, type, action)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_INGRESS_PORT_PORTS_ALL_LOOKUP_MISS_ACTION
/*
 * vlan get vlan-conversion ingress port ( <PORT_LIST:ports> | all ) lookup-miss-action
 */
cparser_result_t cparser_cmd_vlan_get_vlan_conversion_ingress_port_ports_all_lookup_miss_action(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_vlan_lookupMissAct_t action;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("Ingress VLAN conversion Lookup Miss action:\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : \n", port);

        if ((ret = rtk_vlan_portIgrVlanCnvtLuMisAct_get(unit, port, INNER_VLAN, &action)) == RT_ERR_OK)
        {
            if (LOOKUPMISS_DROP == action)
                DIAG_UTIL_MPRINTF("\tInner Tag Packet:%s\n", "Drop");
            else
                DIAG_UTIL_MPRINTF("\tInner Tag Packet:%s\n", "Forward");
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }

        if ((ret = rtk_vlan_portIgrVlanCnvtLuMisAct_get(unit, port, OUTER_VLAN, &action)) == RT_ERR_OK)
        {
            if (LOOKUPMISS_DROP == action)
                DIAG_UTIL_MPRINTF("\tOuter Tag Packet:%s\n", "Drop");
            else
                DIAG_UTIL_MPRINTF("\tOuter Tag Packet:%s\n", "Forward");
        }
        else
        {
            DIAG_ERR_PRINT(ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_PORT_PORTS_ALL_INNER_OUTER_LOOKUP_MISS_ACTION_FORWARD_DROP
/*
 * vlan set vlan-conversion egress port ( <PORT_LIST:ports> | all ) ( inner | outer ) lookup-miss-action ( forward | drop )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_port_ports_all_inner_outer_lookup_miss_action_forward_drop(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;
    rtk_vlanType_t  type;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    if('i' == TOKEN_CHAR(6, 0))
        type = INNER_VLAN;
    else
        type = OUTER_VLAN;

    if('d' == TOKEN_CHAR(8, 0))
        action = LOOKUPMISS_DROP;
    else
        action = LOOKUPMISS_FWD;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_set(unit, port, action)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portEgrVlanCnvtLuMisAct_set(unit, port, type, action)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;

}
#endif

#ifdef CMD_VLAN_SET_VLAN_CONVERSION_EGRESS_PORT_PORTS_ALL_LOOKUP_MISS_ACTION_FORWARD_DROP
/*
 * vlan set vlan-conversion egress port ( <PORT_LIST:ports> | all ) lookup-miss-action ( forward | drop )
 */
cparser_result_t cparser_cmd_vlan_set_vlan_conversion_egress_port_ports_all_lookup_miss_action_forward_drop(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t    action;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    if('d' == TOKEN_CHAR(7, 0))
    {
        action = LOOKUPMISS_DROP;
    }
    else
    {
        action = LOOKUPMISS_FWD;
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_set(unit, port, action)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portEgrVlanCnvtLuMisAct_set(unit, port, INNER_VLAN, action)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }

            if ((ret = rtk_vlan_portEgrVlanCnvtLuMisAct_set(unit, port, OUTER_VLAN, action)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_GET_VLAN_CONVERSION_EGRESS_PORT_PORTS_ALL_LOOKUP_MISS_ACTION
/*
 * vlan get vlan-conversion egress port ( <PORT_LIST:ports> | all ) lookup-miss-action
 */
cparser_result_t cparser_cmd_vlan_get_vlan_conversion_egress_port_ports_all_lookup_miss_action(cparser_context_t *context,
    char **ports_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_vlan_lookupMissAct_t action;
    rtk_port_t      port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portmask, 5), ret);

    DIAG_UTIL_MPRINTF("Egress VLAN conversion Lookup Miss action:\n");
    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : \n", port);

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_get(unit, port, &action)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portEgrVlanCnvtLuMisAct_get(unit, port, INNER_VLAN, &action)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
            }
        }

        if (LOOKUPMISS_DROP == action)
            DIAG_UTIL_MPRINTF("\tInner Tag Packet:%s\n", "Drop");
        else
            DIAG_UTIL_MPRINTF("\tInner Tag Packet:%s\n", "Forward");


#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if (IS_BACKWARD_COMPATIBLE)
        {
            if ((ret = rtk_vlan_portEgrVlanCnvtLookupMissAct_get(unit, port, &action)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
            }
        }
        else
#endif
        {
            if ((ret = rtk_vlan_portEgrVlanCnvtLuMisAct_get(unit, port, OUTER_VLAN, &action)) != RT_ERR_OK)
            {
                DIAG_ERR_PRINT(ret);
            }
        }

        if (LOOKUPMISS_DROP == action)
            DIAG_UTIL_MPRINTF("\tOuter Tag Packet:%s\n", "Drop");
        else
            DIAG_UTIL_MPRINTF("\tOuter Tag Packet:%s\n", "Forward");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_VLAN_ADD_VLAN_TABLE_VID_VID_ECID_ECID_PORT_PORTS_ALL
/*
 * vlan add vlan-table vid <UINT:vid> ecid <UINT:ecid> port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_vlan_add_vlan_table_vid_vid_ecid_ecid_port_ports_all(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *ecid_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_t      vid = (rtk_vlan_t)*vid_ptr;
    rtk_bpe_pmskEntry_t entry;
    diag_portlist_t diagPmsk;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(diagPmsk, 8), ret);

    osal_memset(&entry, 0x00, sizeof(rtk_bpe_pmskEntry_t));
    entry.ecid = (rtk_bpe_ecid_t)*ecid_ptr;
    entry.port_type = 0;
    entry.portmask = diagPmsk.portmask;
    entry.fwdIndex = -1;

    DIAG_UTIL_ERR_CHK(rtk_vlan_ecidPmsk_add(unit, vid, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_add_vlan_table_vid_vid_ecid_ecid_port_ports_all */
#endif

#ifdef CMD_VLAN_DEL_VLAN_TABLE_VID_VID_ECID_ECID
/*
 * vlan del vlan-table vid <UINT:vid> ecid <UINT:ecid>
 */
cparser_result_t
cparser_cmd_vlan_del_vlan_table_vid_vid_ecid_ecid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *ecid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_t      vid = (rtk_vlan_t)*vid_ptr;
    rtk_bpe_pmskEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&entry, 0x00, sizeof(rtk_bpe_pmskEntry_t));
    entry.ecid = (rtk_bpe_ecid_t)*ecid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_vlan_ecidPmsk_del(unit, vid, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_del_vlan_table_vid_vid_ecid_ecid */
#endif

#ifdef CMD_VLAN_GET_VLAN_TABLE_VID_VID_ECID_ECID
/*
 * vlan get vlan-table vid <UINT:vid> ecid <UINT:ecid>
 */
cparser_result_t
cparser_cmd_vlan_get_vlan_table_vid_vid_ecid_ecid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *ecid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_t      vid = (rtk_vlan_t)*vid_ptr;
    rtk_bpe_pmskEntry_t entry;
    char            port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&entry, 0x00, sizeof(rtk_bpe_pmskEntry_t));
    entry.ecid = (rtk_bpe_ecid_t)*ecid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_vlan_ecidPmsk_get(unit, vid, &entry), ret);

    diag_util_lPortMask2str(port_list, &entry.portmask);
    DIAG_UTIL_MPRINTF("  VLAN\t: %u\n", vid);
    DIAG_UTIL_MPRINTF("  ECID\t: 0x%06X (GRP: %1u, EXT: %3u, BASE: %4u)\n", \
        entry.ecid, BPE_ECID_GRP(entry.ecid), BPE_ECID_EXT(entry.ecid), BPE_ECID_BASE(entry.ecid));
    DIAG_UTIL_MPRINTF(" Ports\t: %s \n", port_list);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vlan_table_vid_vid_ecid_ecid */
#endif

#ifdef CMD_VLAN_DUMP_VLAN_TABLE_VID_VID_ECID
/*
 * vlan dump vlan-table vid <UINT:vid> ecid
 */
cparser_result_t
cparser_cmd_vlan_dump_vlan_table_vid_vid_ecid(
    cparser_context_t *context,
    uint32_t *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 unit;
    uint32 totalCnt = 0;
    rtk_vlan_t vid = *vid_ptr;
    rtk_bpe_pmskEntry_t entry;
    char    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_MPRINTF(" VLAN : %d\n", vid);
    DIAG_UTIL_MPRINTF("--------------------------------------------------\n");

    osal_memset(&entry, 0x00, sizeof(rtk_bpe_pmskEntry_t));

    while (1)
    {
        if ((ret = rtk_vlan_ecidPmskNextValid_get(unit, vid, &entry)) != RT_ERR_OK)
        {
            break;
        }

        osal_memset(port_list, 0, sizeof(port_list));
        diag_util_lPortMask2str(port_list, &entry.portmask);

        DIAG_UTIL_MPRINTF("  ECID\t: 0x%06X (GRP: %1u, EXT: %3u, BASE: %4u)\tPorts : %s\n", \
            entry.ecid, BPE_ECID_GRP(entry.ecid), BPE_ECID_EXT(entry.ecid), BPE_ECID_BASE(entry.ecid), port_list);

        totalCnt++;
    }


    DIAG_UTIL_MPRINTF("\n Total entry number : %d \n", totalCnt);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_dump_vlan_table_vid_vid_ecid */
#endif


#ifdef CMD_VLAN_GET_VID_VID_INTF
/*
 * vlan get vid <UINT:vid> intf
 */
cparser_result_t
cparser_cmd_vlan_get_vid_vid_intf(
    cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_vlan_t vid = *vid_ptr;
    rtk_intf_id_t intfId;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_intfId_get(unit, vid, &intfId), ret);

    DIAG_UTIL_MPRINTF("interface ID: %d\n", intfId);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_get_vid_vid_intf */
#endif

#ifdef CMD_VLAN_SET_VID_VID_INTF_INTF_ID
/*
 * vlan set vid <UINT:vid> intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_vlan_set_vid_vid_intf_intf_id(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_vlan_t vid = *vid_ptr;
    rtk_intf_id_t intfId = *intf_id_ptr;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_vlan_intfId_set(unit, vid, intfId), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_vlan_set_vid_vid_intf_intf_id */
#endif

