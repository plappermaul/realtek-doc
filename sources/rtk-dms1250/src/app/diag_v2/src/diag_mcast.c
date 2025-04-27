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
 * Purpose : Definition those MCAST command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/ipmcast.h>
#include <rtk/mcast.h>
#include <rtk/stack.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <stdio.h>
#include <osal/memory.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_l2.h>
  #include <rtrpc/rtrpc_l3.h>
  #include <rtrpc/rtrpc_ipmcast.h>
  #include <rtrpc/rtrpc_mcast.h>
  #include <rtrpc/rtrpc_stack.h>
#endif

#define MCAST_GROUP_ID_GET(_type, _index)            (((_type) << 24) | (_index))
#define MCAST_GROUP_TYPE_GET(_group_id)              (((_group_id) & 0xFF000000) >> 24)
#define MCAST_GROUP_INDEX_GET(_group_id)             ((_group_id) & 0x00FFFFFF)

const char text_mcast_type[RTK_MCAST_TYPE_END + 1][32] =
{
    "Invalid value",
    "MAC",
    "IP",
    "RTK_MCAST_TYPE_END , remove it",
};

const char text_egrif_type[RTK_MCAST_EGRIF_TYPE_END + 1][32] =
{
    "L2 Bridging",
    "L3 Routing",
    "IP Tunnel",
    "Bridge Port Extension",
    "VXLAN",
    "Stacking",
    "RTK_MCAST_EGRIF_TYPE_END",
};


#define DUMP_GROUP(__grpId)  \
do{                     \
    rtk_mcast_type_t __type = MCAST_GROUP_TYPE_GET(__grpId); \
    rtk_mcast_group_t __groupIdx = MCAST_GROUP_INDEX_GET(__grpId); \
    DIAG_UTIL_MPRINTF("multicast group type : %s, index : %d, groupID : 0x%x\n", text_mcast_type[__type], __groupIdx, __grpId); \
}while(0);

#define DUMP_NEXT_HOP(__nh)  \
do{                     \
    char __portStr[DIAG_UTIL_PORT_MASK_STRING_LEN]; \
    DIAG_UTIL_MPRINTF(" Egr-I/f type       : %s \n", text_egrif_type[__nh.type]);   \
    if (RTK_MCAST_EGRIF_TYPE_L3 == __nh.type) { \
    DIAG_UTIL_MPRINTF(" L3 type for bridge  : %s \n", (__nh.flags & RTK_MCAST_EGRIF_FLAG_BRIDGE) ? "True" : "False");   \
    }  \
    if (RTK_MCAST_EGRIF_TYPE_L2 == __nh.type) {                         \
        osal_memset(__portStr, 0, sizeof(__portStr));  \
        diag_util_lPortMask2str(__portStr, &__nh.l2.portmask);        \
        DIAG_UTIL_MPRINTF(" Egr-I/f flood ports: %s \n", __portStr);   \
        DIAG_UTIL_MPRINTF(" Egr-I/f fwd table index: %d \n", __nh.l2.fwdIndex); \
    }else if (RTK_MCAST_EGRIF_TYPE_L3 == __nh.type){                                                    \
        DIAG_UTIL_MPRINTF(" Egr-I/f intf ID    : %d \n", __nh.l3.intf_id);   \
        osal_memset(__portStr, 0, sizeof(__portStr));  \
        diag_util_lPortMask2str(__portStr, &__nh.l3.portmask);        \
        DIAG_UTIL_MPRINTF(" Egr-I/f flood ports: %s \n", __portStr);   \
        DIAG_UTIL_MPRINTF(" Egr-I/f fwd table index: %d \n", __nh.l3.fwdIndex); \
    } else if (RTK_MCAST_EGRIF_TYPE_TUNNEL == __nh.type){         \
        DIAG_UTIL_MPRINTF(" Egr-I/f intf ID    : %d \n", __nh.tunnel.intf_id);   \
    } else if (RTK_MCAST_EGRIF_TYPE_BPE == __nh.type){                                                    \
        DIAG_UTIL_MPRINTF(" Egr-I/f ECID: 0x%x \n", __nh.bpe.ecid);   \
        osal_memset(__portStr, 0, sizeof(__portStr));  \
        diag_util_lPortMask2str(__portStr, &__nh.bpe.portmask);        \
        DIAG_UTIL_MPRINTF(" Egr-I/f flood ports: %s \n", __portStr);   \
        DIAG_UTIL_MPRINTF(" Egr-I/f fwd table index: %d \n", __nh.bpe.fwdIndex); \
    } else if (RTK_MCAST_EGRIF_TYPE_VXLAN == __nh.type){                                                    \
        DIAG_UTIL_MPRINTF(" Egr-I/f VXLAN entry: %d \n", __nh.vxlan.entry_idx);   \
    }else if (RTK_MCAST_EGRIF_TYPE_STK == __nh.type){                                                    \
        osal_memset(__portStr, 0, sizeof(__portStr));  \
        diag_util_lPortMask2str(__portStr, &__nh.stk.portmask);        \
        DIAG_UTIL_MPRINTF(" Egr-I/f stack ports: %s \n", __portStr);   \
    } \
    DIAG_UTIL_MPRINTF("\n");     \
}while(0);


#ifdef CMD_MCAST_CREATE_GROUP_GROUP_TYPE_MAC
/*
 * mcast create group group-type mac
 */
cparser_result_t
cparser_cmd_mcast_create_group_group_type_mac(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_type_t type = RTK_MCAST_TYPE_MAC;
    rtk_mcast_group_t groupIdx = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_mcast_group_create(unit, 0, type, &groupIdx), ret);
    DUMP_GROUP(MCAST_GROUP_ID_GET(type, groupIdx));

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_create_group_group_type_mac */
#endif

#ifdef CMD_MCAST_CREATE_GROUP_GROUP_TYPE_IP
/*
 * mcast create group group-type ip
 */
cparser_result_t
cparser_cmd_mcast_create_group_group_type_ip(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_type_t type = RTK_MCAST_TYPE_IP;
    rtk_mcast_group_t groupIdx = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_mcast_group_create(unit, 0, type, &groupIdx), ret);
    DUMP_GROUP(MCAST_GROUP_ID_GET(type, groupIdx));

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_create_group_group_type_ip */
#endif

#ifdef CMD_MCAST_CREATE_GROUP_GROUP_TYPE_MAC_GROUP_ID_GROUP_ID
/*
 * mcast create group group-type mac group-id <UINT:group_id>
 */
cparser_result_t
cparser_cmd_mcast_create_group_group_type_mac_group_id_group_id(
    cparser_context_t *context,
    uint32_t *group_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_type_t type = RTK_MCAST_TYPE_MAC;
    rtk_mcast_group_t groupIdx;

    DIAG_UTIL_FUNC_INIT(unit);

    groupIdx = MCAST_GROUP_ID_GET(type, *group_id_ptr);

    DUMP_GROUP(groupIdx);
    DIAG_UTIL_ERR_CHK(rtk_mcast_group_create(unit, RTK_MCAST_FLAG_WITH_ID, type, &groupIdx), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_create_group_group_type_mac_group_id_group_id */
#endif

#ifdef CMD_MCAST_CREATE_GROUP_GROUP_TYPE_IP_GROUP_ID_GROUP_ID
/*
 * mcast create group group-type ip group-id <UINT:group_id>
 */
cparser_result_t
cparser_cmd_mcast_create_group_group_type_ip_group_id_group_id(
    cparser_context_t *context,
    uint32_t *group_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_type_t type = RTK_MCAST_TYPE_IP;
    rtk_mcast_group_t groupIdx;

    DIAG_UTIL_FUNC_INIT(unit);

    groupIdx = MCAST_GROUP_ID_GET(type, *group_id_ptr);

    DUMP_GROUP(groupIdx);
    DIAG_UTIL_ERR_CHK(rtk_mcast_group_create(unit, RTK_MCAST_FLAG_WITH_ID, type, &groupIdx), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_create_group_group_type_ip_group_id_group_id */
#endif

#ifdef CMD_MCAST_DESTROY_GROUP_GROUP
/*
 * mcast destroy group <UINT:group>
 */
cparser_result_t
cparser_cmd_mcast_destroy_group_group(
    cparser_context_t *context,
    uint32_t *group_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);

    DUMP_GROUP(*group_ptr);
    DIAG_UTIL_ERR_CHK(rtk_mcast_group_destroy(unit, *group_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_destroy_group_group */
#endif

#ifdef CMD_MCAST_DUMP_GROUP
/*
 * mcast dump group
 */
cparser_result_t
cparser_cmd_mcast_dump_group(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    int32   nhNum, j;
    rtk_switch_devInfo_t devInfo;
    rtk_mcast_egrif_t *pNhEntry = NULL;
    rtk_mcast_group_t grpId;
    rtk_mcast_type_t type = 0;
    int32 base = -1;
    uint32 grpNum = 0;
    uint32 maxNhNum = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&devInfo,0,sizeof(rtk_switch_devInfo_t));
    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    maxNhNum = devInfo.capacityInfo.max_num_of_mcast_group_nexthop;
    pNhEntry = osal_alloc(sizeof(rtk_mcast_egrif_t) * maxNhNum);
    if (NULL == pNhEntry)
    {
        DIAG_UTIL_ERR_CHK(RT_ERR_NULL_POINTER,ret);
    }
    osal_memset(pNhEntry, 0, sizeof(rtk_mcast_egrif_t) * maxNhNum);

    while (1)
    {
        if (RT_ERR_OK != (ret = rtk_mcast_group_getNext(unit, type, &base, &grpId)))
        {
            DIAG_ERR_PRINT(ret);
            osal_free(pNhEntry);
            return CPARSER_NOT_OK;
        }
        if (base >= 0)
        {
            grpNum++;
            DUMP_GROUP(grpId);

            if (RT_ERR_OK != (ret = rtk_mcast_egrIf_get(unit, grpId, maxNhNum, pNhEntry, &nhNum)))
            {
                DIAG_ERR_PRINT(ret);
                osal_free(pNhEntry);
                return CPARSER_NOT_OK;
            }

            DIAG_UTIL_MPRINTF(" Total group egress-interface entry number : %d \n", nhNum);
            if (0 < nhNum)
            {
                for (j=0; j<nhNum; j++)
                {
                    DUMP_NEXT_HOP(pNhEntry[j]);
                }
            }
        }
        else
            break;
    }

    DIAG_UTIL_MPRINTF("\n Total group entry number : %d \n", grpNum);

    osal_free(pNhEntry);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_dump_group */
#endif

#ifdef CMD_MCAST_DUMP_GROUP_GROUP
/*
 * mcast dump group <UINT:group>
 */
cparser_result_t
cparser_cmd_mcast_dump_group_group(
    cparser_context_t *context,
    uint32_t *group_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    int32   nhNum, j;
    rtk_switch_devInfo_t devInfo;
    rtk_mcast_egrif_t *pNhEntry = NULL;
    rtk_mcast_group_t grpId = *group_ptr;
    uint32 maxNhNum = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&devInfo,0,sizeof(rtk_switch_devInfo_t));
    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    maxNhNum = devInfo.capacityInfo.max_num_of_mcast_group_nexthop;
    pNhEntry = osal_alloc(sizeof(rtk_mcast_egrif_t) * maxNhNum);
    if (NULL == pNhEntry)
    {
        DIAG_UTIL_ERR_CHK(RT_ERR_NULL_POINTER,ret);
    }
    osal_memset(pNhEntry, 0, sizeof(rtk_mcast_egrif_t) * maxNhNum);

    DUMP_GROUP(grpId);

    if (RT_ERR_OK != (ret = rtk_mcast_egrIf_get(unit, grpId, maxNhNum, pNhEntry, &nhNum)))
    {
        DIAG_ERR_PRINT(ret);
        osal_free(pNhEntry);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF(" Total group egress-interface entry number : %d \n", nhNum);
    if (0 < nhNum)
    {
        for (j=0; j<nhNum; j++)
        {
            DUMP_NEXT_HOP(pNhEntry[j]);
        }
    }

    osal_free(pNhEntry);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_dump_group_group */
#endif

#ifdef CMD_MCAST_GET_EGRESS_INTERFACE_GROUP_GROUP
/*
 * mcast get egress-interface group <UINT:group>
 */
cparser_result_t
cparser_cmd_mcast_get_egress_interface_group_group(
    cparser_context_t *context,
    uint32_t *group_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t *pNhEntry = NULL;
    rtk_switch_devInfo_t devInfo;
    int32 nhNum, j;
    uint32 maxNhNum = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&devInfo,0,sizeof(rtk_switch_devInfo_t));
    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    maxNhNum = devInfo.capacityInfo.max_num_of_mcast_group_nexthop;
    pNhEntry = osal_alloc(sizeof(rtk_mcast_egrif_t) * maxNhNum);
    if (NULL == pNhEntry)
    {
        DIAG_UTIL_ERR_CHK(RT_ERR_NULL_POINTER,ret);
    }
    osal_memset(pNhEntry, 0, sizeof(rtk_mcast_egrif_t) * maxNhNum);

    if ( RT_ERR_OK != (ret = rtk_mcast_egrIf_get(unit, *group_ptr, maxNhNum, pNhEntry, &nhNum)))
    {
        DIAG_ERR_PRINT(ret);
        osal_free(pNhEntry);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_MPRINTF(" Total group egress-interface entry number : %d \n", nhNum);
    if (0 < nhNum)
    {
        DUMP_GROUP(*group_ptr);
        for (j = 0; j < nhNum; j++)
        {
            DUMP_NEXT_HOP(pNhEntry[j]);
        }
    }
    osal_free(pNhEntry);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_get_egress_interface_group_group */
#endif

#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_L2_PORT_PORTS_ALL
/*
 * mcast add egress-interface group <UINT:group> l2 port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_l2_port_ports_all(
    cparser_context_t *context,
    uint32_t *group_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;
    diag_portlist_t pmsk;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(pmsk, 7), ret);

    osal_memset(&egrIf, 0x00, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_L2;
    egrIf.l2.fwdIndex = -1;
    RTK_PORTMASK_ASSIGN(egrIf.l2.portmask, pmsk.portmask);

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);

    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_l2_port_ports_all */
#endif

#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_BPE_ECID_ECID_PORT_PORTS_ALL
/*
 * mcast add egress-interface group <UINT:group> bpe ecid <UINT:ecid> port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_bpe_ecid_ecid_port_ports_all(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *ecid_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;
    diag_portlist_t pmsk;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(pmsk, 9), ret);

    osal_memset(&egrIf, 0x00, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_BPE;
    egrIf.bpe.ecid = *ecid_ptr;
    egrIf.bpe.fwdIndex = -1;
    RTK_PORTMASK_ASSIGN(egrIf.bpe.portmask, pmsk.portmask);

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);

    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_bpe_ecid_ecid_port_ports_all */
#endif

#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_STACK_PORT_PORTS_ALL
/*
 * mcast add egress-interface group <UINT:group> stack port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_stack_port_ports_all(
    cparser_context_t *context,
    uint32_t *group_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;
    diag_portlist_t pmsk;
    rtk_portmask_t  stkLpmsk;
    char    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_stack_port_get(unit, &stkLpmsk), ret);
    DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(port_list, &stkLpmsk), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(pmsk, 7), ret);
    if ('a' != TOKEN_CHAR(7, 0))
    {
        RTK_PORTMASK_REVERT(stkLpmsk);
        RTK_PORTMASK_AND(stkLpmsk,pmsk.portmask);
    }
    else
    {
        RTK_PORTMASK_ASSIGN(pmsk.portmask, stkLpmsk);
    }

    osal_memset(&egrIf, 0x00, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_STK;
    RTK_PORTMASK_ASSIGN(egrIf.stk.portmask, pmsk.portmask);

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);

    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_stack_port_ports_all */
#endif

#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_L3_INTF_INTF_ID_PORT_PORTS_ALL_REPLACE_BRIDGE
/*
 * mcast add egress-interface group <UINT:group> l3 intf <UINT:intf_id> port ( <PORT_LIST:ports> | all )  { replace } { bridge }
 */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_l3_intf_intf_id_port_ports_all_replace_bridge(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *intf_id_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;
    diag_portlist_t pmsk;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(pmsk, 9), ret);
    osal_memset(&egrIf, 0, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_L3;
    egrIf.l3.intf_id = *intf_id_ptr;
    egrIf.l3.fwdIndex = -1;
    RTK_PORTMASK_ASSIGN(egrIf.l3.portmask,pmsk.portmask);
    egrIf.l3.intf_id = *intf_id_ptr;

    if (11 == TOKEN_NUM)
    {
        if ('r' ==  TOKEN_CHAR(10, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE;
        else if ('b' ==  TOKEN_CHAR(10, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_BRIDGE;
    }
    else if (12 == TOKEN_NUM)
    {
        if ('r' ==  TOKEN_CHAR(10, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE;
        if ('b' ==  TOKEN_CHAR(11, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_BRIDGE;
    }

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_l3_intf_intf_id_port_ports_all_replace_bridge */
#endif

#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_L3_INTF_INTF_ID_ECID_ECID_PORT_PORTS_ALL_REPLACE_BRIDGE
/*
 * mcast add egress-interface group <UINT:group> l3 intf <UINT:intf_id> ecid <UINT:ecid> port ( <PORT_LIST:ports> | all )  { replace } { bridge }
 */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_l3_intf_intf_id_ecid_ecid_port_ports_all_replace_bridge(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *intf_id_ptr,
    uint32_t *ecid_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;
    diag_portlist_t pmsk;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(pmsk, 11), ret);
    osal_memset(&egrIf, 0, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_L3;
    egrIf.l3.intf_id = *intf_id_ptr;
    egrIf.l3.fwdIndex = -1;
    egrIf.l3.ecid = *ecid_ptr;
    RTK_PORTMASK_ASSIGN(egrIf.l3.portmask,pmsk.portmask);

    if (13 == TOKEN_NUM)
    {
        if ('r' ==  TOKEN_CHAR(12, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE;
        else if ('b' ==  TOKEN_CHAR(12, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_BRIDGE;
    }
    else if (14 == TOKEN_NUM)
    {
        if ('r' ==  TOKEN_CHAR(12, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE;
        if ('b' ==  TOKEN_CHAR(13, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_BRIDGE;
    }

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;

}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_l3_intf_intf_id_ecid_ecid_port_ports_all_replace_bridge */
#endif


#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_TUNNEL_INTF_INTF_ID
/*
 * mcast add egress-interface group <UINT:group> tunnel intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_tunnel_intf_intf_id(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&egrIf, 0, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_TUNNEL;
    egrIf.tunnel.intf_id = *intf_id_ptr;

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_tunnel_intf_intf_id */
#endif

#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_VXLAN_ENTRY_INDEX
/*
 * mcast add egress-interface group <UINT:group> vxlan entry <UINT:index>
 */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_vxlan_entry_index(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&egrIf, 0, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_VXLAN;
    egrIf.vxlan.entry_idx = *index_ptr;

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_vxlan_entry_index */
#endif

#ifdef CMD_MCAST_DEL_EGRESS_INTERFACE_GROUP_GROUP_L2_PORT_PORTS_ALL
/*
 * mcast del egress-interface group <UINT:group> l2 port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_mcast_del_egress_interface_group_group_l2_port_ports_all(
    cparser_context_t *context,
    uint32_t *group_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;
    diag_portlist_t pmsk;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(pmsk, 7), ret);

    osal_memset(&egrIf, 0x00, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_L2;
    egrIf.l2.fwdIndex = -1;
    RTK_PORTMASK_ASSIGN(egrIf.l2.portmask, pmsk.portmask);

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);

    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_del(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_del_egress_interface_group_group_l2_port_ports_all */
#endif

#ifdef CMD_MCAST_DEL_EGRESS_INTERFACE_GROUP_GROUP_BPE_ECID_ECID_PORT_PORTS_ALL
/*
 * mcast del egress-interface group <UINT:group> bpe ecid <UINT:ecid> port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_mcast_del_egress_interface_group_group_bpe_ecid_ecid_port_ports_all(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *ecid_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;
    diag_portlist_t pmsk;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(pmsk, 9), ret);

    osal_memset(&egrIf, 0x00, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_BPE;
    egrIf.bpe.ecid = *ecid_ptr;
    egrIf.bpe.fwdIndex = -1;
    RTK_PORTMASK_ASSIGN(egrIf.bpe.portmask, pmsk.portmask);

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);

    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_del(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_del_egress_interface_group_group_bpe_ecid_ecid_port_ports_all */
#endif

#ifdef CMD_MCAST_DEL_EGRESS_INTERFACE_GROUP_GROUP_STACK_PORT_PORTS_ALL
/*
 * mcast del egress-interface group <UINT:group> stack port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_mcast_del_egress_interface_group_group_stack_port_ports_all(
    cparser_context_t *context,
    uint32_t *group_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;
    diag_portlist_t pmsk;
    rtk_portmask_t  stkLpmsk;
    char    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_stack_port_get(unit, &stkLpmsk), ret);
    DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(port_list, &stkLpmsk), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(pmsk, 7), ret);
    if ('a' == TOKEN_CHAR(7, 0))
    {
        RTK_PORTMASK_ASSIGN(pmsk.portmask, stkLpmsk);
    }
    else
    {
        RTK_PORTMASK_AND(stkLpmsk,pmsk.portmask);
    }

    osal_memset(&egrIf, 0x00, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_STK;
    RTK_PORTMASK_ASSIGN(egrIf.stk.portmask, pmsk.portmask);

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);

    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_del(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_del_egress_interface_group_group_stack_port_ports_all */
#endif

#ifdef CMD_MCAST_DEL_EGRESS_INTERFACE_GROUP_GROUP_L3_INTF_INTF_ID_PORT_PORTS_ALL_BRIDGE
/*
 * mcast del egress-interface group <UINT:group> l3 intf <UINT:intf_id> port ( <PORT_LIST:ports> | all ) {bridge}
 */
cparser_result_t
cparser_cmd_mcast_del_egress_interface_group_group_l3_intf_intf_id_port_ports_all_bridge(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *intf_id_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;
    diag_portlist_t pmsk;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(pmsk, 9), ret);
    osal_memset(&egrIf, 0, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_L3;
    egrIf.l3.intf_id = *intf_id_ptr;
    egrIf.l3.fwdIndex = -1;
    egrIf.l3.intf_id = *intf_id_ptr;
    RTK_PORTMASK_ASSIGN(egrIf.l3.portmask,pmsk.portmask);
    if (11 == TOKEN_NUM)
    {
        egrIf.flags = RTK_MCAST_EGRIF_FLAG_BRIDGE;
    }

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_del(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_del_egress_interface_group_group_l3_intf_intf_id_port_ports_all_bridge */
#endif

#ifdef CMD_MCAST_DEL_EGRESS_INTERFACE_GROUP_GROUP_L3_INTF_INTF_ID_ECID_ECID_PORT_PORTS_ALL_BRIDGE
/*
 * mcast del egress-interface group <UINT:group> l3 intf <UINT:intf_id> ecid <UINT:ecid> port ( <PORT_LIST:ports> | all ) { bridge }
 */
cparser_result_t
cparser_cmd_mcast_del_egress_interface_group_group_l3_intf_intf_id_ecid_ecid_port_ports_all_bridge(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *intf_id_ptr,
    uint32_t *ecid_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;
    diag_portlist_t pmsk;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(pmsk, 11), ret);
    osal_memset(&egrIf, 0, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_L3;
    egrIf.l3.intf_id = *intf_id_ptr;
    egrIf.l3.fwdIndex = -1;
    egrIf.l3.ecid = *ecid_ptr;

    RTK_PORTMASK_ASSIGN(egrIf.l3.portmask,pmsk.portmask);
    if (13 == TOKEN_NUM)
    {
        egrIf.flags = RTK_MCAST_EGRIF_FLAG_BRIDGE;
    }

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_del(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;

}   /* end of cparser_cmd_mcast_del_egress_interface_group_group_l3_intf_intf_id_ecid_ecid_port_ports_all_bridge */
#endif


#ifdef CMD_MCAST_DEL_EGRESS_INTERFACE_GROUP_GROUP_TUNNEL_INTF_INTF_ID
/*
 * mcast del egress-interface group <UINT:group> tunnel intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_mcast_del_egress_interface_group_group_tunnel_intf_intf_id(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&egrIf, 0, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_TUNNEL;
    egrIf.tunnel.intf_id = *intf_id_ptr;

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_del(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_del_egress_interface_group_group_tunnel_intf_intf_id */
#endif

#ifdef CMD_MCAST_DEL_EGRESS_INTERFACE_GROUP_GROUP_VXLAN_ENTRY_INDEX
/*
 * mcast del egress-interface group <UINT:group> vxlan entry <UINT:index>
 */
cparser_result_t
cparser_cmd_mcast_del_egress_interface_group_group_vxlan_entry_index(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&egrIf, 0, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_VXLAN;
    egrIf.vxlan.entry_idx = *index_ptr;

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_del(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_del_egress_interface_group_group_vxlan_entry_index */
#endif

#ifdef CMD_MCAST_DEL_EGRESS_INTERFACE_GROUP_GROUP
/*
 * mcast del egress-interface group <UINT:group>
 */
cparser_result_t
cparser_cmd_mcast_del_egress_interface_group_group(
    cparser_context_t *context,
    uint32_t *group_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);

    DUMP_GROUP(*group_ptr);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_delAll(unit, *group_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_del_egress_interface_group_group */
#endif


#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_L2_FWD_TBL_IDX_INDEX
/*
 * mcast add egress-interface group <UINT:group> l2 fwd-tbl-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_l2_fwd_tbl_idx_index(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&egrIf, 0x00, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_L2;
    egrIf.l2.fwdIndex = *index_ptr;

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);

    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_l2_fwd_tbl_idx_index */
#endif

#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_BPE_ECID_ECID_FWD_TBL_IDX_INDEX
/*
 * mcast add egress-interface group <UINT:group> bpe ecid <UINT:ecid> fwd-tbl-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_bpe_ecid_ecid_fwd_tbl_idx_index(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *ecid_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&egrIf, 0x00, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_BPE;
    egrIf.bpe.ecid = *ecid_ptr;
    egrIf.bpe.fwdIndex = *index_ptr;

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);

    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_bpe_ecid_ecid_fwd_tbl_idx_index */
#endif

#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_L3_INTF_INTF_ID_FWD_TBL_IDX_INDEX_REPLACE_BRIDGE
/*
 * mcast add egress-interface group <UINT:group> l3 intf <UINT:intf_id> fwd-tbl-idx <UINT:index>  { replace } { bridge }
 */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_l3_intf_intf_id_fwd_tbl_idx_index_replace_bridge(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *intf_id_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&egrIf, 0, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_L3;
    egrIf.l3.intf_id = *intf_id_ptr;
    egrIf.l3.fwdIndex = *index_ptr;

    if (11 == TOKEN_NUM)
    {
        if ('r' ==  TOKEN_CHAR(10, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE;
        else if ('b' ==  TOKEN_CHAR(10, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_BRIDGE;
    }
    else if (12 == TOKEN_NUM)
    {
        if ('r' ==  TOKEN_CHAR(10, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE;
        if ('b' ==  TOKEN_CHAR(11, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_BRIDGE;
    }

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);


    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_l3_intf_intf_id_fwd_tbl_idx_index_replace_bridge */
#endif

#ifdef CMD_MCAST_ADD_EGRESS_INTERFACE_GROUP_GROUP_L3_INTF_INTF_ID_ECID_ECID_FWD_TBL_IDX_INDEX_REPLACE_BRIDGE
/*
 * mcast add egress-interface group <UINT:group> l3 intf <UINT:intf_id> ecid <UINT:ecid> fwd-tbl-idx <UINT:index> { replace } { bridge } */
cparser_result_t
cparser_cmd_mcast_add_egress_interface_group_group_l3_intf_intf_id_ecid_ecid_fwd_tbl_idx_index_replace_bridge(
    cparser_context_t *context,
    uint32_t *group_ptr,
    uint32_t *intf_id_ptr,
    uint32_t *ecid_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_mcast_egrif_t egrIf;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&egrIf, 0, sizeof(rtk_mcast_egrif_t));
    egrIf.type = RTK_MCAST_EGRIF_TYPE_L3;
    egrIf.l3.intf_id = *intf_id_ptr;
    egrIf.l3.fwdIndex = *index_ptr;
    egrIf.l3.ecid = *ecid_ptr;

    if (13 == TOKEN_NUM)
    {
        if ('r' ==  TOKEN_CHAR(12, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE;
        else if ('b' ==  TOKEN_CHAR(12, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_BRIDGE;
    }
    else if (14 == TOKEN_NUM)
    {
        if ('r' ==  TOKEN_CHAR(12, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE;
        if ('b' ==  TOKEN_CHAR(13, 0))
            egrIf.flags |= RTK_MCAST_EGRIF_FLAG_BRIDGE;
    }

    DUMP_GROUP(*group_ptr);
    DUMP_NEXT_HOP(egrIf);
    DIAG_UTIL_ERR_CHK(rtk_mcast_egrIf_add(unit, *group_ptr, &egrIf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mcast_add_egress_interface_group_group_l3_intf_intf_id_ecid_ecid_fwd_tbl_idx_index_replace_bridge */
#endif

