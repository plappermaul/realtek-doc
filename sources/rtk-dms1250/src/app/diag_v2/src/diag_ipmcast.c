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
 * Purpose : Definition those IPMCAST command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/ipmcast.h>
#include <rtk/mcast.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <stdio.h>
#include <rtk/default.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_l2.h>
  #include <rtrpc/rtrpc_l3.h>
  #include <rtrpc/rtrpc_ipmcast.h>
  #include <rtrpc/rtrpc_mcast.h>
#endif
#define DIAG_IPMC_PARSE_IPMC_ACT(__idx,__act)       \
do {                                                \
    switch(TOKEN_CHAR(__idx, 2)) {                  \
        case 'r':                                   \
            if ('w' == TOKEN_CHAR(__idx, 3))        \
                __act = RTK_IPMC_ACT_FORWARD;       \
            else if ('d' == TOKEN_CHAR(__idx, 3))   \
                __act = RTK_IPMC_ACT_HARD_DROP;     \
            break;                                  \
        case 'o':                                   \
            if ('p' == TOKEN_CHAR(__idx, 3))        \
                __act = RTK_IPMC_ACT_DROP;          \
            break;                                  \
        case 'a':                                   \
            if ('c' == TOKEN_CHAR(__idx, 8))        \
                __act = RTK_IPMC_ACT_TRAP2CPU;      \
            else if ('m' == TOKEN_CHAR(__idx, 8))   \
                __act = RTK_IPMC_ACT_TRAP2MASTERCPU;\
            break;                                  \
        case 'p':                                   \
            if ('c' == TOKEN_CHAR(__idx, 8))        \
                __act = RTK_IPMC_ACT_COPY2CPU;      \
            else if ('m' == TOKEN_CHAR(__idx, 8))   \
                __act = RTK_IPMC_ACT_COPY2MASTERCPU;\
            break;                                  \
        default:                                    \
            break;                                  \
    }                                               \
}while(0)

#define DIAG_IPMC_PARSE_IPMC_ADDR_ACT(__idx,__act)  \
do {                                                \
    switch(TOKEN_CHAR(__idx, 0)) {                  \
        case 'f':                                   \
            __act = RTK_IPMC_ADDR_ACT_FORWARD;      \
            break;                                  \
        case 'd':                                   \
            __act = RTK_IPMC_ADDR_ACT_DROP;         \
            break;                                  \
        case 't':                                   \
            __act = RTK_IPMC_ADDR_ACT_TRAP2CPU;     \
            break;                                  \
        case 'c':                                   \
            __act = RTK_IPMC_ADDR_ACT_COPY2CPU;     \
            break;                                  \
        default:                                    \
            break;                                  \
    }                                               \
}while(0)

#define DIAG_IPMC_PARSE_IPMC_RPF_FAIL_ACT(__idx,__act)  \
do {                                                    \
    switch(TOKEN_CHAR(__idx, 0)) {                      \
        case 't':                                       \
            __act = RTK_IPMC_RPF_FAIL_ACT_TRAP;         \
            break;                                      \
        case 'd':                                       \
            __act = RTK_IPMC_RPF_FAIL_ACT_DROP;         \
            break;                                      \
        case 'c':                                       \
            __act = RTK_IPMC_RPF_FAIL_ACT_COPY;         \
            break;                                      \
        case 'a':                                       \
            __act = RTK_IPMC_RPF_FAIL_ACT_ASSERT_CHK;   \
            break;                                      \
        default:                                        \
            break;                                      \
    }                                                   \
}while(0)


const char text_ipmc_action[RTK_IPMC_ACT_END+1][64] =
{
    /*RTK_IPMC_ACT_FORWARD*/                            "Forward",
    /*RTK_IPMC_ACT_DROP*/                               "Drop",
    /*RTK_IPMC_ACT_TRAP2CPU*/                           "Trap to CPU",
    /*RTK_IPMC_ACT_COPY2CPU*/                           "Copy to CPU",
    /*RTK_IPMC_ACT_TRAP2MASTERCPU*/                     "Trap to Master CPU",
    /*RTK_IPMC_ACT_COPY2MASTERCPU*/                     "Copy to Master CPU",
    /*RTK_IPMC_ACT_HARD_DROP*/                          "Hard Drop",
    /*RTK_IPMC_ACT_END*/                                "RTK_IPMC_ACT_END, remove it",
};

const char text_ipmc_addr_action[RTK_IPMC_ADDR_ACT_END+1][64] =
{
    /*RTK_IPMC_ADDR_ACT_FORWARD*/                       "Forward",
    /*RTK_IPMC_ADDR_ACT_DROP*/                          "Drop",
    /*RTK_IPMC_ADDR_ACT_TRAP2CPU*/                      "Trap to CPU",
    /*RTK_IPMC_ADDR_ACT_COPY2CPU*/                      "Copy to CPU",
    /*RTK_IPMC_ADDR_ACT_END*/                           "RTK_IPMC_ADDR_ACT_END, remove it",
};

const char text_ipmc_rpf_fail_action[RTK_IPMC_RPF_FAIL_ACT_END+1][64] =
{
    /*RTK_IPMC_RPF_FAIL_ACT_TRAP*/                      "Trap",
    /*RTK_IPMC_RPF_FAIL_ACT_DROP*/                      "Drop",
    /*RTK_IPMC_RPF_FAIL_ACT_COPY*/                      "Copy to CPU",
    /*RTK_IPMC_RPF_FAIL_ACT_ASSERT_CHK*/                "Assert check",
    /*RTK_IPMC_RPF_FAIL_ACT_END*/                       "RTK_IPMC_RPF_FAIL_ACT_END, remove it",
};

const char text_ipmc_cputarget[RTK_IPMC_CPUTARGET_END+1][32] =
{
    "Local CPU",        /* RTK_IPMC_CPUTARGET_LOCAL */
    "Master CPU",       /* RTK_IPMC_CPUTARGET_MASTER */
    "RTK_IPMC_CPUTARGET_END",   // for model code check error only
};

//IPMC_FLAG_SIP & IPMC_FLAG_IPV6 & IPMC_FLAG_HIT_CLEAR & IPMC_FLAG_REPLACE do not need display
const char text_ipmc_flag[10][32] =
{
    "NONE",
    "REPLACE",
    "DISABLE",
    "IPV6",
    "WITH_SIP",
    "SRC_INTF_ID",
    "RPF_EN",
    "QOS_ASSIGN",
    "HIT",
    "",                     /*HIT_CLEAR*/
};

const char text_ipmc_stat_type[4][32] =
{
    "Invalid Type",
    "Byte",
    "Packet",
    "Byte and Packet",
};

#define IPMC_FLAG2STR(__f, __s) \
do{                 \
    uint32 __i = 0; \
    uint32 __l = 0; \
    for (__i = 0; __i < 16 ; __i++) {   \
        if (0 != (__f & (1<< __i)))     \
            __l += osal_sprintf(__s + __l, " %s |", text_ipmc_flag[__i+1]);     \
    }   \
}while(0);

#define DUMP_IPMC(__ipmc)                                                       \
do{                                                                             \
    char __fs[128];                                                             \
    char __is[128];                                                             \
    osal_memset(__fs, 0 , sizeof(__fs));                                        \
    osal_memset(__is, 0 , sizeof(__is));                                        \
    IPMC_FLAG2STR(__ipmc.ipmc_flags, __fs);                                     \
    if (osal_strlen(__fs) > 0) __fs[osal_strlen(__fs)-1] = 0;                   \
    DIAG_UTIL_MPRINTF("IPMCAST Entry : \n");                                    \
    DIAG_UTIL_MPRINTF("Flags         : 0x%x (%s)\n", __ipmc.ipmc_flags, __fs);  \
    DIAG_UTIL_MPRINTF("VRF ID        : %u\n", __ipmc.vrf_id);                   \
    if (__ipmc.ipmc_flags & RTK_IPMC_FLAG_IPV6) {                               \
        osal_strcpy(__is, "0::0");                                              \
        if (__ipmc.ipmc_flags & RTK_IPMC_FLAG_SIP) {                            \
        diag_util_ipv62str(__is, (uint8 *)__ipmc.src_ip_addr.ipv6.octet);}      \
        DIAG_UTIL_MPRINTF("SIP IPv6 Addr : %s\n", __is);                        \
        diag_util_ipv62str(__is, (uint8 *)__ipmc.dst_ip_addr.ipv6.octet);       \
        DIAG_UTIL_MPRINTF("DIP IPv6 Addr : %s\n", __is);                        \
    } else {                                                                    \
        osal_strcpy(__is, "0.0.0.0");                                           \
        if (__ipmc.ipmc_flags & RTK_IPMC_FLAG_SIP) {                            \
        diag_util_ip2str(__is, __ipmc.src_ip_addr.ipv4);}                       \
        DIAG_UTIL_MPRINTF("SIP IPv4 Addr : %s\n", __is);                        \
        diag_util_ip2str(__is, __ipmc.dst_ip_addr.ipv4);                        \
        DIAG_UTIL_MPRINTF("DIP IPv4 Addr : %s\n", __is);                        \
    }                                                                           \
    if (__ipmc.ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID) {                        \
        DIAG_UTIL_MPRINTF("Interface ID  : %u\n", __ipmc.src_intf.intf_id);     \
    } else {                                                                    \
        DIAG_UTIL_MPRINTF("VLAN ID       : %u\n", __ipmc.src_intf.vlan_id);     \
    }                                                                           \
    DIAG_UTIL_MPRINTF("Group ID      : 0x%x\n", __ipmc.group);                  \
    DIAG_UTIL_MPRINTF("L3 enable     : %s\n", __ipmc.l3_en ? "Enable" : "Disable");      \
    DIAG_UTIL_MPRINTF("L2 action     : %s\n", text_ipmc_addr_action[__ipmc.l2_fwd_act]); \
    DIAG_UTIL_MPRINTF("L3 action     : %s\n", text_ipmc_addr_action[__ipmc.l3_fwd_act]); \
    DIAG_UTIL_MPRINTF("RPF fail act. : %s\n", text_ipmc_rpf_fail_action[__ipmc.l3_rpf_fail_act]);   \
    DIAG_UTIL_MPRINTF("TTL min.      : %d\n", __ipmc.ttl_min);                  \
    DIAG_UTIL_MPRINTF("MTU max.      : %d\n", __ipmc.mtu_max);                  \
}while(0);


//line header
#define DUMP_IPMC_LH(__ipv6)  \
do{                     \
    if (__ipv6) {                         \
        DIAG_UTIL_MPRINTF(" Index  | VRF ID |            Group IP Address            |              SIP Address               |VID/INTF|  Group ID  | Pri | MTU  | TTL | Flags   \n");           \
        DIAG_UTIL_MPRINTF("--------+--------+----------------------------------------+----------------------------------------+--------+------------+-----+------+-----+--------------------------\n");           \
    } else {                                                    \
        DIAG_UTIL_MPRINTF(" Index  | VRF ID | Group IP Address |   SIP Address    |VID/INTF|  Group ID  | Pri | MTU  | TTL | Flags \n");           \
        DIAG_UTIL_MPRINTF("--------+--------+------------------+------------------+--------+------------+-----+------+-----+--------------------------------------------\n");           \
    }                                                           \
}while(0);


#define DUMP_IPMC_ADDR_LH(__ipmc, __idx)                                                                \
do{                                                                                                     \
    char __fs[128];                                                                                     \
    char __sip[128];                                                                                    \
    char __dip[128];                                                                                    \
    osal_memset(__fs, 0 , sizeof(__fs));                                                                \
    osal_memset(__sip, 0 , sizeof(__sip));                                                              \
    osal_memset(__dip, 0 , sizeof(__dip));                                                              \
    IPMC_FLAG2STR(__ipmc.ipmc_flags, __fs);                                                             \
    if (osal_strlen(__fs) > 0) __fs[osal_strlen(__fs)-1] = 0;                                           \
    if (__ipmc.ipmc_flags & RTK_IPMC_FLAG_IPV6) {                                                       \
        osal_strcpy(__sip, "0::0");                                                                     \
        if (__ipmc.ipmc_flags & RTK_IPMC_FLAG_SIP){                                                     \
        diag_util_ipv62str(__sip, (uint8 *)__ipmc.src_ip_addr.ipv6.octet);}                             \
        diag_util_ipv62str(__dip, (uint8 *)__ipmc.dst_ip_addr.ipv6.octet);                              \
        DIAG_UTIL_MPRINTF(" %5d  | %4d   | %38s | %38s | %4d   | %#10x | %d   | %5d| %3d | %s \n",                    \
            __idx, __ipmc.vrf_id, __dip, __sip,__ipmc.src_intf.vlan_id, __ipmc.group, __ipmc.qos_pri,  __ipmc.mtu_max, __ipmc.ttl_min,__fs);    \
    }else {                                                                                             \
        osal_strcpy(__sip, "0.0.0.0");                                                                  \
        if (__ipmc.ipmc_flags & RTK_IPMC_FLAG_SIP){                                                     \
        diag_util_ip2str(__sip, __ipmc.src_ip_addr.ipv4);}                                              \
        diag_util_ip2str(__dip, __ipmc.dst_ip_addr.ipv4);                                               \
        DIAG_UTIL_MPRINTF(" %5d  | %4d   | %16s | %16s | %4d   | %#10x | %d   | %5d| %3d | %s \n",                    \
            __idx, __ipmc.vrf_id, __dip, __sip,__ipmc.src_intf.vlan_id, __ipmc.group, __ipmc.qos_pri, __ipmc.mtu_max, __ipmc.ttl_min, __fs);    \
    }                                                                                                   \
}while(0);

#define DUMP_IPMC_STAT(__ips, __isFlag)                                                     \
do{                                                                                         \
    char __dip[128];                                                                        \
    char __sip[128];                                                                        \
    osal_memset(__sip, 0x00, sizeof(__sip));                                                \
    osal_memset(__dip, 0x00, sizeof(__dip));                                                \
    DIAG_UTIL_MPRINTF("IPMCAST Stat monitor Entry : \n");                                   \
    DIAG_UTIL_MPRINTF("VRF ID        : %u\n", __ips.key.vrf_id);                            \
    DIAG_UTIL_MPRINTF("VLAN ID       : %u\n",__ips.key.src_intf.vlan_id);                   \
    if (__ips.key.flags & RTK_IPMC_FLAG_IPV6) {                                             \
        osal_strcpy(__sip, "0::0");                                                         \
        diag_util_ipv62str(__dip, (uint8 *)__ips.key.dst_ip_addr.ipv6.octet);               \
        DIAG_UTIL_MPRINTF("DIP IPv6 Addr : %s\n", __dip);                                   \
        if (__ips.key.flags & RTK_IPMC_FLAG_SIP) {                                          \
        diag_util_ipv62str(__sip, (uint8 *)__ips.key.src_ip_addr.ipv6.octet);}              \
        DIAG_UTIL_MPRINTF("SIP IPv6 Addr : %s\n", __sip);                                   \
    } else {                                                                                \
        osal_strcpy(__sip, "0.0.0.0");                                                      \
        diag_util_ip2str(__dip, __ips.key.dst_ip_addr.ipv4);                                \
        DIAG_UTIL_MPRINTF("DIP IPv4 Addr : %s\n", __dip);                                   \
        if (__ips.key.flags & RTK_IPMC_FLAG_SIP) {                                          \
        diag_util_ip2str(__sip, __ips.key.src_ip_addr.ipv4);}                               \
        DIAG_UTIL_MPRINTF("SIP IPv4 Addr : %s\n", __sip);                                   \
    }                                                                                       \
    if (__isFlag) {                                                                         \
        DIAG_UTIL_MPRINTF("Monitor Type  : %s\n", text_ipmc_stat_type[__ips.mont_flags]);   \
    }                                                                                       \
}while(0);

#ifdef CMD_IPMCAST_ADD_ADDR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_VLAN_VID_GROUP_GROUP_ID_L3_ENABLE_DISABLE_ENABLE_PRIORITY
/*
 * ipmcast add addr vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> vlan <UINT:vid> group <UINT:group_id> l3-enable ( disable | enable ) { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip_sip_dip_dip_vlan_vid_group_group_id_l3_enable_disable_enable_priority(
cparser_context_t *context,
uint32_t *vrf_id_ptr,
uint32_t *sip_ptr,
uint32_t *dip_ptr,
uint32_t *vid_ptr,
uint32_t *group_id_ptr,
uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t     ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0, sizeof(ipmcAddr));
    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.src_ip_addr.ipv4 = 0xFFFFFFFF;
    if (0 != *sip_ptr)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
        ipmcAddr.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcAddr.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcAddr.src_intf.vlan_id = *vid_ptr;
    ipmcAddr.group = *group_id_ptr;
    if ('e' == TOKEN_STR(14)[0])
    {
        ipmcAddr.l3_en = ENABLED;
    }
    else
    {
        ipmcAddr.l3_en = DISABLED;
    }

    if (16 == TOKEN_NUM)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN;
        ipmcAddr.qos_pri = *priority_ptr;
    }
    ipmcAddr.mtu_max = RTK_DEFAULT_L3_INTF_MTU;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip_sip_dip_dip_vlan_vid_group_group_id_priority */
#endif

#ifdef CMD_IPMCAST_ADD_ADDR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_VLAN_VID_GROUP_GROUP_ID_L3_ENABLE_DISABLE_ENABLE_L2_ACTION_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_DROP_L3_ACTION_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_DROP_RPF_FAIL_ACTION_TRAP_TO_CPU_DROP_COPY_TO_CPU_ASSERT_CHECK_TTL_MIN_TTL_MIN_MTU_MAX_MTU_MAX_PRIORITY
/*
 * ipmcast add addr vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> vlan <UINT:vid> group <UINT:group_id> l3-enable ( disable | enable ) l2-action ( forward | trap-to-cpu | copy-to-cpu | drop ) l3-action ( forward | trap-to-cpu | copy-to-cpu | drop ) rpf-fail-action ( trap-to-cpu | drop | copy-to-cpu | assert-check ) ttl-min <UINT:ttl_min> mtu-max <UINT:mtu_max> { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip_sip_dip_dip_vlan_vid_group_group_id_l3_enable_disable_enable_l2_action_forward_trap_to_cpu_copy_to_cpu_drop_l3_action_forward_trap_to_cpu_copy_to_cpu_drop_rpf_fail_action_trap_to_cpu_drop_copy_to_cpu_assert_check_ttl_min_ttl_min_mtu_max_mtu_max_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vid_ptr,
    uint32_t *group_id_ptr,
    uint32_t *ttl_min_ptr,
    uint32_t *mtu_max_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0x00, sizeof(ipmcAddr));

    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.src_ip_addr.ipv4 = 0xFFFFFFFF;
    if (0 != *sip_ptr)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
        ipmcAddr.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcAddr.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcAddr.src_intf.vlan_id = *vid_ptr;
    ipmcAddr.group = *group_id_ptr;
    if ('e' == TOKEN_STR(14)[0])
    {
        ipmcAddr.l3_en = ENABLED;
    }
    else
    {
        ipmcAddr.l3_en = DISABLED;
    }
    DIAG_IPMC_PARSE_IPMC_ADDR_ACT(16, ipmcAddr.l2_fwd_act);
    DIAG_IPMC_PARSE_IPMC_ADDR_ACT(18, ipmcAddr.l3_fwd_act);
    DIAG_IPMC_PARSE_IPMC_RPF_FAIL_ACT(20, ipmcAddr.l3_rpf_fail_act);
    ipmcAddr.ttl_min = *ttl_min_ptr;
    ipmcAddr.mtu_max = *mtu_max_ptr;
    if (26 == TOKEN_NUM)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN;
        ipmcAddr.qos_pri = *priority_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip_sip_dip_dip_vlan_vid_group_group_id_l2_action_forward_trap_to_cpu_copy_to_cpu_drop_l3_action_forward_trap_to_cpu_copy_to_cpu_drop_rpf_fail_action_trap_to_cpu_drop_copy_to_cpu_assert_check_ttl_min_ttl_min_mtu_max_mtu_max_priority */
#endif

#ifdef CMD_IPMCAST_DEL_ADDR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_VLAN_VID
/*
 * ipmcast del addr vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> vlan <UINT:vid>
 */
cparser_result_t
cparser_cmd_ipmcast_del_addr_vrf_id_vrf_id_sip_sip_dip_dip_vlan_vid(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t     ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0, sizeof(ipmcAddr));
    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.src_ip_addr.ipv4 = 0xFFFFFFFF;
    if (0 != *sip_ptr)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
        ipmcAddr.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcAddr.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcAddr.src_intf.vlan_id = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_del(unit, &ipmcAddr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_del_addr_vrf_id_vrf_id_sip_sip_dip_dipvlan_vid */
#endif

#ifdef CMD_IPMCAST_ADD_ADDR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_VLAN_VID_GROUP_GROUP_ID_L3_ENABLE_DISABLE_ENABLE_PRIORITY
/*
 * ipmcast add addr vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> vlan <UINT:vid> group <UINT:group_id> l3-enable ( disable | enable ) { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_vlan_vid_group_group_id_l3_enable_disable_enable_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vid_ptr,
    uint32_t *group_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t     ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0, sizeof(ipmcAddr));
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_IPV6;
    ipmcAddr.vrf_id = *vrf_id_ptr;

    diag_util_str2ipv6(ipmcAddr.src_ip_addr.ipv6.octet,*sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
    else
        osal_memset(ipmcAddr.src_ip_addr.ipv6.octet, 0xFF, IPV6_ADDR_LEN);
    diag_util_str2ipv6(ipmcAddr.dst_ip_addr.ipv6.octet,*dip_ptr);
    ipmcAddr.src_intf.vlan_id = *vid_ptr;
    ipmcAddr.group = *group_id_ptr;
    if ('e' == TOKEN_STR(14)[0])
    {
        ipmcAddr.l3_en = ENABLED;
    }
    else
    {
        ipmcAddr.l3_en = DISABLED;
    }
    if (16 == TOKEN_NUM)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN;
        ipmcAddr.qos_pri = *priority_ptr;
    }
    ipmcAddr.mtu_max = RTK_DEFAULT_L3_INTF_MTU;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_vlan_vid_group_group_id_priority */
#endif

#ifdef CMD_IPMCAST_ADD_ADDR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_VLAN_VID_GROUP_GROUP_ID_L3_ENABLE_DISABLE_ENABLE_L2_ACTION_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_DROP_L3_ACTION_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_DROP_RPF_FAIL_ACTION_TRAP_TO_CPU_DROP_COPY_TO_CPU_ASSERT_CHECK_TTL_MIN_TTL_MIN_MTU_MAX_MTU_MAX_PRIORITY
/*
 * ipmcast add addr vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> vlan <UINT:vid> group <UINT:group_id> l3-enable ( disable | enable ) l2-action ( forward | trap-to-cpu | copy-to-cpu | drop ) l3-action ( forward | trap-to-cpu | copy-to-cpu | drop ) rpf-fail-action ( trap-to-cpu | drop | copy-to-cpu | assert-check ) ttl-min <UINT:ttl_min> mtu-max <UINT:mtu_max> { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_vlan_vid_group_group_id_l3_enable_disable_enable_l2_action_forward_trap_to_cpu_copy_to_cpu_drop_l3_action_forward_trap_to_cpu_copy_to_cpu_drop_rpf_fail_action_trap_to_cpu_drop_copy_to_cpu_assert_check_ttl_min_ttl_min_mtu_max_mtu_max_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vid_ptr,
    uint32_t *group_id_ptr,
    uint32_t *ttl_min_ptr,
    uint32_t *mtu_max_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0x00, sizeof(ipmcAddr));

    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_IPV6;
    diag_util_str2ipv6(ipmcAddr.src_ip_addr.ipv6.octet, *sip_ptr);
    if (TRUE != diag_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
    }
    else
        osal_memset(ipmcAddr.src_ip_addr.ipv6.octet, 0xFF, IPV6_ADDR_LEN);

    diag_util_str2ipv6(ipmcAddr.dst_ip_addr.ipv6.octet, *dip_ptr);
    ipmcAddr.src_intf.vlan_id = *vid_ptr;
    ipmcAddr.group = *group_id_ptr;
    if ('e' == TOKEN_STR(14)[0])
    {
        ipmcAddr.l3_en = ENABLED;
    }
    else
    {
        ipmcAddr.l3_en = DISABLED;
    }
    DIAG_IPMC_PARSE_IPMC_ADDR_ACT(16, ipmcAddr.l2_fwd_act);
    DIAG_IPMC_PARSE_IPMC_ADDR_ACT(18, ipmcAddr.l3_fwd_act);
    DIAG_IPMC_PARSE_IPMC_RPF_FAIL_ACT(20, ipmcAddr.l3_rpf_fail_act);
    ipmcAddr.ttl_min = *ttl_min_ptr;
    ipmcAddr.mtu_max = *mtu_max_ptr;
    if (26 == TOKEN_NUM)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN;
        ipmcAddr.qos_pri = *priority_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_vlan_vid_group_group_id_l2_action_forward_trap_to_cpu_copy_to_cpu_drop_l3_action_forward_trap_to_cpu_copy_to_cpu_drop_rpf_fail_action_trap_to_cpu_drop_copy_to_cpu_assert_check_ttl_min_ttl_min_mtu_max_mtu_max_priority */
#endif

#ifdef CMD_IPMCAST_DEL_ADDR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_VLAN_VID
/*
 * ipmcast del addr vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> vlan <UINT:vid>
 */
cparser_result_t
cparser_cmd_ipmcast_del_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_vlan_vid(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t     ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0, sizeof(ipmcAddr));
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_IPV6;
    ipmcAddr.vrf_id = *vrf_id_ptr;

    diag_util_str2ipv6(ipmcAddr.src_ip_addr.ipv6.octet,*sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
    else
        osal_memset(ipmcAddr.src_ip_addr.ipv6.octet, 0xFF, IPV6_ADDR_LEN);

    diag_util_str2ipv6(ipmcAddr.dst_ip_addr.ipv6.octet,*dip_ptr);
    ipmcAddr.src_intf.vlan_id = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_del(unit, &ipmcAddr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_del_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_vlan_vid */
#endif

#ifdef CMD_IPMCAST_ADD_ADDR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_INTF_INTF_ID_GROUP_GROUP_ID_L3_ENABLE_DISABLE_ENABLE_PRIORITY
/*
 * ipmcast add addr vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> intf <UINT:intf_id> group <UINT:group_id> l3-enable ( disable | enable ) { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip_sip_dip_dip_intf_intf_id_group_group_id_l3_enable_disable_enable_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *intf_id_ptr,
    uint32_t *group_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0x00, sizeof(ipmcAddr));

    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.src_ip_addr.ipv4 = 0xFFFFFFFF;
    if (0 != *sip_ptr)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
        ipmcAddr.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcAddr.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcAddr.src_intf.intf_id = *intf_id_ptr;
    ipmcAddr.group = *group_id_ptr;
    if ('e' == TOKEN_STR(14)[0])
    {
        ipmcAddr.l3_en = ENABLED;
    }
    else
    {
        ipmcAddr.l3_en = DISABLED;
    }
    if (16 == TOKEN_NUM)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN;
        ipmcAddr.qos_pri = *priority_ptr;
    }
    ipmcAddr.mtu_max = RTK_DEFAULT_L3_INTF_MTU;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip_sip_dip_dip_intf_intf_id_group_group_id_priority */
#endif

#ifdef CMD_IPMCAST_ADD_ADDR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_INTF_INTF_ID_GROUP_GROUP_ID_L3_ENABLE_DISABLE_ENABLE_L2_ACTION_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_DROP_L3_ACTION_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_DROP_RPF_FAIL_ACTION_TRAP_TO_CPU_DROP_COPY_TO_CPU_ASSERT_CHECK_TTL_MIN_TTL_MIN_MTU_MAX_MTU_MAX_PRIORITY
/*
 * ipmcast add addr vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> intf <UINT:intf_id> group <UINT:group_id> l3-enable ( disable | enable ) l2-action ( forward | trap-to-cpu | copy-to-cpu | drop ) l3-action ( forward | trap-to-cpu | copy-to-cpu | drop ) rpf-fail-action ( trap-to-cpu | drop | copy-to-cpu | assert-check ) ttl-min <UINT:ttl_min> mtu-max <UINT:mtu_max> { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip_sip_dip_dip_intf_intf_id_group_group_id_l3_enable_disable_enable_l2_action_forward_trap_to_cpu_copy_to_cpu_drop_l3_action_forward_trap_to_cpu_copy_to_cpu_drop_rpf_fail_action_trap_to_cpu_drop_copy_to_cpu_assert_check_ttl_min_ttl_min_mtu_max_mtu_max_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *intf_id_ptr,
    uint32_t *group_id_ptr,
    uint32_t *ttl_min_ptr,
    uint32_t *mtu_max_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0x00, sizeof(ipmcAddr));

    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.src_ip_addr.ipv4 = 0xFFFFFFFF;
    if (0 != *sip_ptr)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
        ipmcAddr.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcAddr.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcAddr.src_intf.intf_id = *intf_id_ptr;
    ipmcAddr.group = *group_id_ptr;
    DIAG_IPMC_PARSE_IPMC_ADDR_ACT(16, ipmcAddr.l2_fwd_act);
    DIAG_IPMC_PARSE_IPMC_ADDR_ACT(18, ipmcAddr.l3_fwd_act);
    DIAG_IPMC_PARSE_IPMC_RPF_FAIL_ACT(20, ipmcAddr.l3_rpf_fail_act);
    ipmcAddr.ttl_min = *ttl_min_ptr;
    ipmcAddr.mtu_max = *mtu_max_ptr;
    if ('e' == TOKEN_STR(14)[0])
    {
        ipmcAddr.l3_en = ENABLED;
    }
    else
    {
        ipmcAddr.l3_en = DISABLED;
    }
    if (26 == TOKEN_NUM)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN;
        ipmcAddr.qos_pri = *priority_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip_sip_dip_dip_intf_intf_id_group_group_id_l2_action_forward_trap_to_cpu_copy_to_cpu_drop_l3_action_forward_trap_to_cpu_copy_to_cpu_drop_rpf_fail_action_trap_to_cpu_drop_copy_to_cpu_assert_check_ttl_min_ttl_min_mtu_max_mtu_max_priority */
#endif

#ifdef CMD_IPMCAST_DEL_ADDR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_INTF_INTF_ID
/*
 * ipmcast del addr vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_ipmcast_del_addr_vrf_id_vrf_id_sip_sip_dip_dip_intf_intf_id(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0x00, sizeof(ipmcAddr));

    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.src_ip_addr.ipv4 = 0xFFFFFFFF;
    if (0 != *sip_ptr)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
        ipmcAddr.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcAddr.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcAddr.src_intf.intf_id = *intf_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_del(unit, &ipmcAddr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_del_addr_vrf_id_vrf_id_sip_sip_dip_dip_intf_intf_id */
#endif

#ifdef CMD_IPMCAST_ADD_ADDR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_INTF_INTF_ID_GROUP_GROUP_ID_L3_ENABLE_DISABLE_ENABLE_PRIORITY
/*
 * ipmcast add addr vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> intf <UINT:intf_id> group <UINT:group_id> l3-enable ( disable | enable ) { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_intf_intf_id_group_group_id_l3_enable_disable_enable_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *intf_id_ptr,
    uint32_t *group_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0x00, sizeof(ipmcAddr));

    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_IPV6;
    diag_util_str2ipv6(ipmcAddr.src_ip_addr.ipv6.octet, *sip_ptr);
    if (TRUE != diag_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
    }
    else
        osal_memset(ipmcAddr.src_ip_addr.ipv6.octet, 0xFF, IPV6_ADDR_LEN);

    diag_util_str2ipv6(ipmcAddr.dst_ip_addr.ipv6.octet, *dip_ptr);
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcAddr.src_intf.intf_id = *intf_id_ptr;
    ipmcAddr.group = *group_id_ptr;
    if ('e' == TOKEN_STR(14)[0])
    {
        ipmcAddr.l3_en = ENABLED;
    }
    else
    {
        ipmcAddr.l3_en = DISABLED;
    }
    if (16 == TOKEN_NUM)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN;
        ipmcAddr.qos_pri = *priority_ptr;
    }
    ipmcAddr.mtu_max = RTK_DEFAULT_L3_INTF_MTU;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_intf_intf_id_group_group_id_priority */
#endif

#ifdef CMD_IPMCAST_ADD_ADDR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_INTF_INTF_ID_GROUP_GROUP_ID_L3_ENABLE_DISABLE_ENABLE_L2_ACTION_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_DROP_L3_ACTION_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_DROP_RPF_FAIL_ACTION_TRAP_TO_CPU_DROP_COPY_TO_CPU_ASSERT_CHECK_TTL_MIN_TTL_MIN_MTU_MAX_MTU_MAX_PRIORITY
/*
 * ipmcast add addr vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> intf <UINT:intf_id> group <UINT:group_id> l3-enable ( disable | enable ) l2-action ( forward | trap-to-cpu | copy-to-cpu | drop ) l3-action ( forward | trap-to-cpu | copy-to-cpu | drop ) rpf-fail-action ( trap-to-cpu | drop | copy-to-cpu | assert-check ) ttl-min <UINT:ttl_min> mtu-max <UINT:mtu_max> { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_intf_intf_id_group_group_id_l3_enable_disable_enable_l2_action_forward_trap_to_cpu_copy_to_cpu_drop_l3_action_forward_trap_to_cpu_copy_to_cpu_drop_rpf_fail_action_trap_to_cpu_drop_copy_to_cpu_assert_check_ttl_min_ttl_min_mtu_max_mtu_max_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *intf_id_ptr,
    uint32_t *group_id_ptr,
    uint32_t *ttl_min_ptr,
    uint32_t *mtu_max_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0x00, sizeof(ipmcAddr));

    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_IPV6;
    diag_util_str2ipv6(ipmcAddr.src_ip_addr.ipv6.octet, *sip_ptr);
    if (TRUE != diag_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
    }
    else
        osal_memset(ipmcAddr.src_ip_addr.ipv6.octet, 0xFF, IPV6_ADDR_LEN);

    diag_util_str2ipv6(ipmcAddr.dst_ip_addr.ipv6.octet, *dip_ptr);
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcAddr.src_intf.intf_id = *intf_id_ptr;
    ipmcAddr.group = *group_id_ptr;
    if ('e' == TOKEN_STR(14)[0])
    {
        ipmcAddr.l3_en = ENABLED;
    }
    else
    {
        ipmcAddr.l3_en = DISABLED;
    }
    DIAG_IPMC_PARSE_IPMC_ADDR_ACT(16, ipmcAddr.l2_fwd_act);
    DIAG_IPMC_PARSE_IPMC_ADDR_ACT(18, ipmcAddr.l3_fwd_act);
    DIAG_IPMC_PARSE_IPMC_RPF_FAIL_ACT(20, ipmcAddr.l3_rpf_fail_act);
    ipmcAddr.ttl_min = *ttl_min_ptr;
    ipmcAddr.mtu_max = *mtu_max_ptr;
    if (26 == TOKEN_NUM)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN;
        ipmcAddr.qos_pri = *priority_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_intf_intf_id_group_group_id_l2_action_forward_trap_to_cpu_copy_to_cpu_drop_l3_action_forward_trap_to_cpu_copy_to_cpu_drop_rpf_fail_action_trap_to_cpu_drop_copy_to_cpu_assert_check_ttl_min_ttl_min_mtu_max_mtu_max_priority */
#endif

#ifdef CMD_IPMCAST_DEL_ADDR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_INTF_INTF_ID
/*
 * ipmcast del addr vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_ipmcast_del_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_intf_intf_id(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0x00, sizeof(ipmcAddr));

    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_IPV6;
    diag_util_str2ipv6(ipmcAddr.src_ip_addr.ipv6.octet, *sip_ptr);
    if (TRUE != diag_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
    }
    else
        osal_memset(ipmcAddr.src_ip_addr.ipv6.octet, 0xFF, IPV6_ADDR_LEN);

    diag_util_str2ipv6(ipmcAddr.dst_ip_addr.ipv6.octet, *dip_ptr);
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcAddr.src_intf.intf_id = *intf_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_del(unit, &ipmcAddr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_del_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_intf_intf_id */
#endif

#ifdef CMD_IPMCAST_DEL_ADDR_ALL_IP_IP6
/*
 * ipmcast del addr all ( ip | ip6 )
 */
cparser_result_t
cparser_cmd_ipmcast_del_addr_all_ip_ip6(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_flag_t flag = RTK_IPMC_FLAG_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(4)))
        flag |= RTK_IPMC_FLAG_IPV6;

    DIAG_UTIL_MPRINTF("delete %s ipmcast address all\n", TOKEN_STR(4));
    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_delAll(unit, flag), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_del_addr_all_ip_ip6 */
#endif

#ifdef CMD_IPMCAST_SET_ADDR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_INTF_ID_RPF_STATE_ENABLE_DISABLE
/*
 * ipmcast set addr vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> intf <UINT:id> ( rpf ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_ipmcast_set_addr_vrf_id_vrf_id_sip_sip_dip_dip_intf_id_rpf_state_enable_disable(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_addr_t     ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_PARSE_STATE(13, state);

    osal_memset(&ipmcAddr, 0, sizeof(ipmcAddr));
    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.src_ip_addr.ipv4 = 0xFFFFFFFF;
    if (0 != *sip_ptr)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
        ipmcAddr.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcAddr.dst_ip_addr.ipv4 = *dip_ptr;

    /* interface ID */
    ipmcAddr.src_intf.intf_id = *id_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_find(unit, &ipmcAddr), ret);

    /* update (replace it) */
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_REPLACE;

    if ('r' == TOKEN_CHAR(11, 0))
    {
        if (ENABLED == state)
        {
            ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_RPF_EN;
        }
        else
        {
            ipmcAddr.ipmc_flags &= ~(RTK_IPMC_FLAG_RPF_EN);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_addr_vrf_id_vrf_id_sip_sip_dip_dip_intf_id_rpf_state_enable_disable */
#endif

#ifdef CMD_IPMCAST_SET_ADDR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_VLAN_ID_RPF_STATE_ENABLE_DISABLE
/*
 * ipmcast set addr vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> vlan <UINT:id> rpf state ( enable | disable )
 */
cparser_result_t
cparser_cmd_ipmcast_set_addr_vrf_id_vrf_id_sip_sip_dip_dip_vlan_id_rpf_state_enable_disable(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_addr_t     ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_PARSE_STATE(13, state);

    osal_memset(&ipmcAddr, 0, sizeof(ipmcAddr));
    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.src_ip_addr.ipv4 = 0xFFFFFFFF;
    if (0 != *sip_ptr)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
        ipmcAddr.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcAddr.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcAddr.src_intf.vlan_id = *id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_find(unit, &ipmcAddr), ret);

    /* update (replace it) */
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_REPLACE;

    if ('r' == TOKEN_CHAR(11, 0))
    {
        if (ENABLED == state)
        {
            ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_RPF_EN;
        }
        else
        {
            ipmcAddr.ipmc_flags &= ~(RTK_IPMC_FLAG_RPF_EN);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_addr_vrf_id_vrf_id_sip_sip_dip_dip_vlan_id_rpf_state_enable_disable */
#endif

#ifdef CMD_IPMCAST_GET_ADDR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_VLAN_VID_HIT_CLEAR
/*
 * ipmcast get addr vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> vlan <UINT:vid> { hit-clear }
 */
cparser_result_t
cparser_cmd_ipmcast_get_addr_vrf_id_vrf_id_sip_sip_dip_dip_vlan_vid_hit_clear(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t     ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ipmcAddr, 0, sizeof(ipmcAddr));
    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.src_ip_addr.ipv4 = 0xFFFFFFFF;
    if (0 != *sip_ptr)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
        ipmcAddr.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcAddr.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcAddr.src_intf.vlan_id = *vid_ptr;
    ipmcAddr.ipmc_flags |= (12 == TOKEN_NUM) ? RTK_IPMC_FLAG_HIT_CLEAR : RTK_IPMC_FLAG_NONE;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_find(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_addr_vrf_id_vrf_id_sip_sip_dip_dip_vlan_vid_hit_clear */
#endif

#ifdef CMD_IPMCAST_SET_ADDR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_INTF_ID_RPF_STATE_ENABLE_DISABLE
/*
 * ipmcast set addr vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> intf <UINT:id> rpf state ( enable | disable )
 */
cparser_result_t
cparser_cmd_ipmcast_set_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_intf_id_rpf_state_enable_disable(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_addr_t     ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_PARSE_STATE(13, state);

    osal_memset(&ipmcAddr, 0, sizeof(rtk_ipmc_addr_t));
    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_IPV6;

    diag_util_str2ipv6(ipmcAddr.src_ip_addr.ipv6.octet, *sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
    else
        osal_memset(ipmcAddr.src_ip_addr.ipv6.octet, 0xFF, IPV6_ADDR_LEN);

    diag_util_str2ipv6(ipmcAddr.dst_ip_addr.ipv6.octet, *dip_ptr);

    ipmcAddr.src_intf.intf_id = *id_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_find(unit, &ipmcAddr), ret);

    /* update (replace it) */
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_REPLACE;

    if ('r' == TOKEN_CHAR(11, 0))
    {
        if (ENABLED == state)
        {
            ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_RPF_EN;
        }
        else
        {
            ipmcAddr.ipmc_flags &= ~(RTK_IPMC_FLAG_RPF_EN);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_intf_vlan_id_rpf_state_enable_disable */
#endif

#ifdef CMD_IPMCAST_SET_ADDR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_VLAN_ID_RPF_STATE_ENABLE_DISABLE
/*
 * ipmcast set addr vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> vlan <UINT:id> rpf state ( enable | disable )
 */
cparser_result_t
cparser_cmd_ipmcast_set_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_vlan_id_rpf_state_enable_disable(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_addr_t     ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_PARSE_STATE(13, state);

    osal_memset(&ipmcAddr, 0, sizeof(rtk_ipmc_addr_t));
    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_IPV6;

    diag_util_str2ipv6(ipmcAddr.src_ip_addr.ipv6.octet, *sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
    else
        osal_memset(ipmcAddr.src_ip_addr.ipv6.octet, 0xFF, IPV6_ADDR_LEN);

    diag_util_str2ipv6(ipmcAddr.dst_ip_addr.ipv6.octet, *dip_ptr);

    ipmcAddr.src_intf.vlan_id = *id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_find(unit, &ipmcAddr), ret);

    /* update (replace it) */
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_REPLACE;

    if ('r' == TOKEN_CHAR(11, 0))
    {
        if (ENABLED == state)
        {
            ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_RPF_EN;
        }
        else
        {
            ipmcAddr.ipmc_flags &= ~(RTK_IPMC_FLAG_RPF_EN);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_add(unit, &ipmcAddr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_vlan_id_rpf_state_enable_disable */
#endif

#ifdef CMD_IPMCAST_GET_ADDR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_VLAN_VID_HIT_CLEAR
/*
 * ipmcast get addr vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> vlan <UINT:vid> { hit-clear }
 */
cparser_result_t
cparser_cmd_ipmcast_get_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_vlan_vid_hit_clear(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t     ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ipmcAddr, 0, sizeof(rtk_ipmc_addr_t));
    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_IPV6;

    diag_util_str2ipv6(ipmcAddr.src_ip_addr.ipv6.octet,*sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
    else
        osal_memset(ipmcAddr.src_ip_addr.ipv6.octet, 0xFF, IPV6_ADDR_LEN);

    diag_util_str2ipv6(ipmcAddr.dst_ip_addr.ipv6.octet,*dip_ptr);
    ipmcAddr.src_intf.vlan_id = *vid_ptr;
    ipmcAddr.ipmc_flags |= (12 == TOKEN_NUM) ? RTK_IPMC_FLAG_HIT_CLEAR : RTK_IPMC_FLAG_NONE;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_find(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_vlan_vid_hit_clear */
#endif

#ifdef CMD_IPMCAST_GET_ADDR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_INTF_INTF_ID_HIT_CLEAR
/*
 * ipmcast get addr vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> intf <UINT:intf_id> { hit-clear }
 */
cparser_result_t
cparser_cmd_ipmcast_get_addr_vrf_id_vrf_id_sip_sip_dip_dip_intf_intf_id_hit_clear(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0, sizeof(ipmcAddr));

    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.src_ip_addr.ipv4 = 0xFFFFFFFF;
    if (0 != *sip_ptr)
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
        ipmcAddr.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcAddr.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcAddr.src_intf.intf_id = *intf_id_ptr;
    ipmcAddr.ipmc_flags |= (12 == TOKEN_NUM) ? RTK_IPMC_FLAG_HIT_CLEAR : RTK_IPMC_FLAG_NONE;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_find(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_addr_vrf_id_vrf_id_sip_sip_dip_dip_intf_intf_id_hit_clear */
#endif

#ifdef CMD_IPMCAST_GET_ADDR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_INTF_INTF_ID_HIT_CLEAR
/*
 * ipmcast get addr vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> intf <UINT:intf_id> { hit-clear }
 */
cparser_result_t
cparser_cmd_ipmcast_get_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_intf_intf_id_hit_clear(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t ipmcAddr;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcAddr, 0, sizeof(ipmcAddr));

    ipmcAddr.vrf_id = *vrf_id_ptr;
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_IPV6;
    diag_util_str2ipv6(ipmcAddr.src_ip_addr.ipv6.octet, *sip_ptr);
    if (TRUE != diag_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
    {
        ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SIP;
    }
    else
        osal_memset(ipmcAddr.src_ip_addr.ipv6.octet, 0xFF, IPV6_ADDR_LEN);

    diag_util_str2ipv6(ipmcAddr.dst_ip_addr.ipv6.octet, *dip_ptr);
    ipmcAddr.ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcAddr.src_intf.intf_id = *intf_id_ptr;
    ipmcAddr.ipmc_flags |= (12 == TOKEN_NUM) ? RTK_IPMC_FLAG_HIT_CLEAR : RTK_IPMC_FLAG_NONE;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_addr_find(unit, &ipmcAddr), ret);
    DUMP_IPMC(ipmcAddr);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_addr_vrf_id_vrf_id_sip6_sip_dip6_dip_intf_intf_id_hit_clear */
#endif

#ifdef CMD_IPMCAST_DUMP_ADDR_IP_IP6_FROM_BEGIN_INDEX_BEGIN_TO_END_INDEX_END
/*
 * ipmcast dump addr ( ip | ip6 ) from ( <UINT:begin_index> | begin ) to ( <UINT:end_index> | end )
 */
cparser_result_t
cparser_cmd_ipmcast_dump_addr_ip_ip6_from_begin_index_begin_to_end_index_end(
    cparser_context_t *context,
    uint32_t *begin_index_ptr,
    uint32_t *end_index_ptr)
{
    uint32  unit;
    int32 base = 0;
    int32 ret = RT_ERR_FAILED;
    rtk_ipmc_addr_t     ipmcAddr;
    rtk_ipmc_flag_t flag = RTK_IPMC_FLAG_NONE;
    int32 ipv6 = FALSE;
    uint32  entryNum = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(3)))
    {
        flag = RTK_IPMC_FLAG_IPV6;
        ipv6 = TRUE;
    }

    if ('b' == TOKEN_CHAR(5, 0))
    {
        base = -1;  /* from beginning */
    }
    else
    {
        /* including the start number */
        base = ((*begin_index_ptr) > 0) ? ((*begin_index_ptr) - 1) : (-1);
    }

    DUMP_IPMC_LH(ipv6);
    do
    {
        DIAG_UTIL_ERR_CHK(rtk_ipmc_nextValidAddr_get(unit, flag, &base, &ipmcAddr), ret);
        if (('e' != TOKEN_CHAR(7, 0)) && (base > (*end_index_ptr)))
        {
            break;
        }
        else if (base >= 0)
        {
            entryNum++;
            DUMP_IPMC_ADDR_LH(ipmcAddr, ipmcAddr.hw_entry_idx);
        }
    } while (base >= 0);

    DIAG_UTIL_MPRINTF("\n Total %s multicast entry number : %d \n", TOKEN_STR(3), entryNum);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_dump_addr_ip_ip6_from_begin_index_begin_to_end_index_end */
#endif

#ifdef CMD_IPMCAST_SET_L3_LU_HASH_WT_INTF_STATE_ENABLE_DISABLE
/*
 * ipmcast set l3-lu-hash-wt-intf state ( enable | disable )
 */
cparser_result_t
cparser_cmd_ipmcast_set_l3_lu_hash_wt_intf_state_enable_disable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_LU_HASH_WT_INTF_EN;
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(4, state);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, state), ret);
    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_l3_lu_hash_wt_intf_state_enable_disable */
#endif

#ifdef CMD_IPMCAST_GET_L3_LU_HASH_WT_INTF_STATE
/*
 * ipmcast get l3-lu-hash-wt-intf state
 */
cparser_result_t
cparser_cmd_ipmcast_get_l3_lu_hash_wt_intf_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_LU_HASH_WT_INTF_EN;
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&state), ret);
    DIAG_UTIL_MPRINTF("L3 lookup with VID/INTF state: %s\n", text_state[state]);
    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_l3_lu_hash_wt_intf_state */
#endif


#ifdef CMD_IPMCAST_SET_IPMC_IP6MC_PKT_TO_CPU_TARGET_LOCAL_MASTER
/*
 * ipmcast set ( ipmc | ip6mc ) pkt-to-cpu-target ( local | master )
 */
cparser_result_t
cparser_cmd_ipmcast_set_ipmc_ip6mc_pkt_to_cpu_target_local_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;
    rtk_ipmc_cpuTarget_t target = RTK_IPMC_CPUTARGET_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('m' == TOKEN_CHAR(2,2))
        type = RTK_IPMC_GCT_IPMC_PKT_TO_CPU_TARGET;
    else if ('6' == TOKEN_CHAR(2,2))
        type = RTK_IPMC_GCT_IP6MC_PKT_TO_CPU_TARGET;

    if ('l' == TOKEN_CHAR(4,0))
        target = RTK_IPMC_CPUTARGET_LOCAL;
    else if ('m' == TOKEN_CHAR(4,0))
        target = RTK_IPMC_CPUTARGET_MASTER;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, target), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_ipmc_ip6mc_pkt_to_cpu_target_local_master */
#endif

#ifdef CMD_IPMCAST_GET_IPMC_IP6MC_PKT_TO_CPU_TARGET
/*
 * ipmcast get ( ipmc | ip6mc ) pkt-to-cpu-target
 */
cparser_result_t
cparser_cmd_ipmcast_get_ipmc_ip6mc_pkt_to_cpu_target(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;
    rtk_ipmc_cpuTarget_t target = RTK_IPMC_CPUTARGET_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('m' == TOKEN_CHAR(2,2))
        type = RTK_IPMC_GCT_IPMC_PKT_TO_CPU_TARGET;
    else if ('6' == TOKEN_CHAR(2,2))
        type = RTK_IPMC_GCT_IP6MC_PKT_TO_CPU_TARGET;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&target), ret);
    DIAG_UTIL_MPRINTF("%s pkt-to-cpu target: %s\n", TOKEN_STR(2), text_ipmc_cputarget[target]);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_ipmc_ip6mc_pkt_to_cpu_target */
#endif

#ifdef CMD_IPMCAST_SET_IPMC_IP6MC_ROUTING_STATE_ENABLE_DISABLE
/*
 * ipmcast set ( ipmc | ip6mc ) routing state ( enable | disable )
 */
cparser_result_t
cparser_cmd_ipmcast_set_ipmc_ip6mc_routing_state_enable_disable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_PARSE_STATE(5, state);

    if ('m' == TOKEN_CHAR(2,2))
        type = RTK_IPMC_GCT_IPMC_GLB_EN;
    else if ('6' == TOKEN_CHAR(2,2))
        type = RTK_IPMC_GCT_IP6MC_GLB_EN;

    DIAG_UTIL_MPRINTF("%s routing state: %s\n", TOKEN_STR(2), text_state[state]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, state), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_ipmc_ip6mc_routing_state_enable_disable */
#endif

#ifdef CMD_IPMCAST_GET_IPMC_IP6MC_ROUTING_STATE
/*
 * ipmcast get ( ipmc | ip6mc ) routing state
 */
cparser_result_t
cparser_cmd_ipmcast_get_ipmc_ip6mc_routing_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('m' == TOKEN_CHAR(2,2))
        type = RTK_IPMC_GCT_IPMC_GLB_EN;
    else if ('6' == TOKEN_CHAR(2,2))
        type = RTK_IPMC_GCT_IP6MC_GLB_EN;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&state), ret);
    DIAG_UTIL_MPRINTF("%s routing state: %s\n", TOKEN_STR(2), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_ipmc_ip6mc_routing_state */
#endif

#ifdef CMD_IPMCAST_SET_ROUTING_EXCEPTION_IPMC_IP6MC_ROUTING_LOOKUP_MISS_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * ipmcast set routing-exception ( ipmc | ip6mc ) routing lookup-miss-action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_ipmcast_set_routing_exception_ipmc_ip6mc_routing_lookup_miss_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_IPMC_PARSE_IPMC_ACT(6, action);

    if ('m' == TOKEN_CHAR(3, 2))
        type = RTK_IPMC_GCT_IPMC_LU_MIS_ACT;
    else if ('6' == TOKEN_CHAR(3, 2))
        type = RTK_IPMC_GCT_IP6MC_LU_MIS_ACT;

    DIAG_UTIL_MPRINTF("%s lookup-miss-action: %s\n", TOKEN_STR(3), text_ipmc_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_routing_exception_ipmc_ip6mc_routing_lookup_miss_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_IPMCAST_GET_ROUTING_EXCEPTION_IPMC_IP6MC_ROUTING_LOOKUP_MISS_ACTION
/*
 * ipmcast get routing-exception ( ipmc | ip6mc ) routing lookup-miss-action
 */
cparser_result_t
cparser_cmd_ipmcast_get_routing_exception_ipmc_ip6mc_routing_lookup_miss_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('m' == TOKEN_CHAR(3, 2))
        type = RTK_IPMC_GCT_IPMC_LU_MIS_ACT;
    else if ('6' == TOKEN_CHAR(3, 2))
        type = RTK_IPMC_GCT_IP6MC_LU_MIS_ACT;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&action), ret);
    DIAG_UTIL_MPRINTF("%s lookup-miss-action: %s\n", TOKEN_STR(3), text_ipmc_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_routing_exception_ipmc_ip6mc_routing_lookup_miss_action */
#endif

#ifdef CMD_IPMCAST_SET_ROUTING_EXCEPTION_IPMC_IP6MC_SRC_VLAN_FILTER_STATE_ENABLE_DISABLE
/*
 * ipmcast set routing-exception ( ipmc | ip6mc ) src-vlan-filter state ( enable | disable )
 */
cparser_result_t
cparser_cmd_ipmcast_set_routing_exception_ipmc_ip6mc_src_vlan_filter_state_enable_disable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_PARSE_STATE(6, state);

    if ('m' == TOKEN_CHAR(3,2))
        type = RTK_IPMC_GCT_IPMC_SRC_INTF_FLTR_EN;
    else if ('6' == TOKEN_CHAR(3,2))
        type = RTK_IPMC_GCT_IP6MC_SRC_INTF_FLTR_EN;

    DIAG_UTIL_MPRINTF("%s routing source vlan filter state: %s\n", TOKEN_STR(3), text_state[state]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, state), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_routing_exception_ipmc_ip6mc_src_vlan_filter_state_enable_disable */
#endif

#ifdef CMD_IPMCAST_GET_ROUTING_EXCEPTION_IPMC_IP6MC_SRC_VLAN_FILTER_STATE
/*
 * ipmcast get routing-exception ( ipmc | ip6mc ) src-vlan-filter state
 */
cparser_result_t
cparser_cmd_ipmcast_get_routing_exception_ipmc_ip6mc_src_vlan_filter_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('m' == TOKEN_CHAR(3,2))
        type = RTK_IPMC_GCT_IPMC_SRC_INTF_FLTR_EN;
    else if ('6' == TOKEN_CHAR(3,2))
        type = RTK_IPMC_GCT_IP6MC_SRC_INTF_FLTR_EN;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&state), ret);
    DIAG_UTIL_MPRINTF("%s routing source vlan filter state: %s\n", TOKEN_STR(3), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_routing_exception_ipmc_ip6mc_src_vlan_filter_state */
#endif

#ifdef CMD_IPMCAST_SET_ROUTING_EXCEPTION_IPMC_IP6MC_SRC_INTF_FILTER_STATE_ENABLE_DISABLE
/*
 * ipmcast set routing-exception ( ipmc | ip6mc ) src-intf-filter state ( enable | disable )
 */
cparser_result_t
cparser_cmd_ipmcast_set_routing_exception_ipmc_ip6mc_src_intf_filter_state_enable_disable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_PARSE_STATE(6, state);

    if ('m' == TOKEN_CHAR(3,2))
        type = RTK_IPMC_GCT_IPMC_SRC_INTF_FLTR_EN;
    else if ('6' == TOKEN_CHAR(3,2))
        type = RTK_IPMC_GCT_IP6MC_SRC_INTF_FLTR_EN;

    DIAG_UTIL_MPRINTF("%s routing source interface filter state: %s\n", TOKEN_STR(3), text_state[state]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, state), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_routing_exception_ipmc_ip6mc_src_intf_filter_state_enable_disable */
#endif

#ifdef CMD_IPMCAST_GET_ROUTING_EXCEPTION_IPMC_IP6MC_SRC_INTF_FILTER_STATE
/*
 * ipmcast get routing-exception ( ipmc | ip6mc ) src-intf-filter state
 */
cparser_result_t
cparser_cmd_ipmcast_get_routing_exception_ipmc_ip6mc_src_intf_filter_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('m' == TOKEN_CHAR(3,2))
        type = RTK_IPMC_GCT_IPMC_SRC_INTF_FLTR_EN;
    else if ('6' == TOKEN_CHAR(3,2))
        type = RTK_IPMC_GCT_IP6MC_SRC_INTF_FLTR_EN;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&state), ret);
    DIAG_UTIL_MPRINTF("%s routing source interface filter state: %s\n", TOKEN_STR(3), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_routing_exception_ipmc_ip6mc_src_intf_filter_state */
#endif

#ifdef CMD_IPMCAST_SET_ROUTING_EXCEPTION_IPMC_TTL_FAIL_MTU_FAIL_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * ipmcast set routing-exception ipmc ( ttl-fail | mtu-fail ) action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_ipmcast_set_routing_exception_ipmc_ttl_fail_mtu_fail_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_IPMC_PARSE_IPMC_ACT(6, action);

    if ('t' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IPMC_TTL_FAIL_ACT;
    else if ('m' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IPMC_MTU_FAIL_ACT;

    DIAG_UTIL_MPRINTF("ipmc %s action: %s\n", TOKEN_STR(4), text_ipmc_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_routing_exception_ipmc_ttl_fail_mtu_fail_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_IPMCAST_SET_ROUTING_EXCEPTION_IP6MC_HL_FAIL_MTU_FAIL_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * ipmcast set routing-exception ip6mc ( hl-fail | mtu-fail ) action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_ipmcast_set_routing_exception_ip6mc_hl_fail_mtu_fail_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_IPMC_PARSE_IPMC_ACT(6, action);

    if ('h' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IP6MC_HL_FAIL_ACT;
    else if ('m' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IP6MC_MTU_FAIL_ACT;

    DIAG_UTIL_MPRINTF("ip6mc %s action: %s\n", TOKEN_STR(4), text_ipmc_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_routing_exception_ip6mc_hl_fail_mtu_fail_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_IPMCAST_SET_ROUTING_EXCEPTION_IPMC_ZERO_SIP_DMAC_MISMATCH_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER_HARD_DROP
/*
 * ipmcast set routing-exception ipmc ( zero-sip | dmac-mismatch ) action ( drop | trap-to-cpu | trap-to-master | hard-drop )
 */
cparser_result_t
cparser_cmd_ipmcast_set_routing_exception_ipmc_zero_sip_dmac_mismatch_action_drop_trap_to_cpu_trap_to_master_hard_drop(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_IPMC_PARSE_IPMC_ACT(6, action);

    if ('z' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IPMC_ZERO_SIP_ACT;
    else if ('d' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IPMC_DMAC_MISMATCH_ACT;

    DIAG_UTIL_MPRINTF("ipmc %s action: %s\n", TOKEN_STR(4), text_ipmc_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_routing_exception_ipmc_zero_sip_dmac_mismatch_action_drop_trap_to_cpu_trap_to_master_hard_drop */
#endif

#ifdef CMD_IPMCAST_GET_ROUTING_EXCEPTION_IPMC_HEADER_OPTION_ACTION
/*
 * ipmcast get routing-exception ipmc header-option action
 */
cparser_result_t
cparser_cmd_ipmcast_get_routing_exception_ipmc_header_option_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    type = RTK_IPMC_GCT_IPMC_HDR_OPT_ACT;
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&action), ret);
    DIAG_UTIL_MPRINTF("ipmc %s action: %s\n", TOKEN_STR(4), text_ipmc_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_routing_exception_ipmc_header_option_action */
#endif

#ifdef CMD_IPMCAST_SET_ROUTING_EXCEPTION_IP6MC_ZERO_SIP_DMAC_MISMATCH_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER_HARD_DROP
/*
 * ipmcast set routing-exception ip6mc ( zero-sip | dmac-mismatch ) action ( drop | trap-to-cpu | trap-to-master | hard-drop )
 */
cparser_result_t
cparser_cmd_ipmcast_set_routing_exception_ip6mc_zero_sip_dmac_mismatch_action_drop_trap_to_cpu_trap_to_master_hard_drop(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_IPMC_PARSE_IPMC_ACT(6, action);

    if ('z' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IP6MC_ZERO_SIP_ACT;
    else if ('d' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IP6MC_DMAC_MISMATCH_ACT;

    DIAG_UTIL_MPRINTF("ip6mc %s action: %s\n", TOKEN_STR(4), text_ipmc_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_routing_exception_ip6mc_zero_sip_dmac_mismatch_action_drop_trap_to_cpu_trap_to_master_hard_drop */
#endif

#ifdef CMD_IPMCAST_GET_ROUTING_EXCEPTION_IP6MC_HEADER_ROUTER_HBH_ERROR_HBH_ACTION
/*
 * ipmcast get routing-exception ip6mc ( header-router | hbh-error | hbh ) action
 */
cparser_result_t
cparser_cmd_ipmcast_get_routing_exception_ip6mc_header_router_hbh_error_hbh_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(4)) && ('h' == TOKEN_CHAR(4, 0)))
        type = RTK_IPMC_GCT_IP6MC_HBH_ACT;
    else if ('e' == TOKEN_CHAR(4, 1))
        type = RTK_IPMC_GCT_IP6MC_HDR_ROUTE_ACT;
    else if ('b' == TOKEN_CHAR(4, 1))
        type = RTK_IPMC_GCT_IP6MC_HBH_ERR_ACT;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&action), ret);
    DIAG_UTIL_MPRINTF("ip6mc %s action: %s\n", TOKEN_STR(4), text_ipmc_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_routing_exception_ip6mc_header_router_hbh_error_hbh_action */
#endif

#ifdef CMD_IPMCAST_GET_ROUTING_EXCEPTION_IPMC_TTL_FAIL_MTU_FAIL_ACTION
/*
 * ipmcast get routing-exception ipmc ( ttl-fail | mtu-fail ) action
 */
cparser_result_t
cparser_cmd_ipmcast_get_routing_exception_ipmc_ttl_fail_mtu_fail_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('t' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IPMC_TTL_FAIL_ACT;
    else if ('m' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IPMC_MTU_FAIL_ACT;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&action), ret);
    DIAG_UTIL_MPRINTF("ipmc %s action: %s\n", TOKEN_STR(4), text_ipmc_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_routing_exception_ipmc_ttl_fail_mtu_fail_action */
#endif


#ifdef CMD_IPMCAST_GET_ROUTING_EXCEPTION_IP6MC_HL_FAIL_MTU_FAIL_ACTION
/*
 * ipmcast get routing-exception ip6mc ( hl-fail | mtu-fail ) action
 */
cparser_result_t
cparser_cmd_ipmcast_get_routing_exception_ip6mc_hl_fail_mtu_fail_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('h' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IP6MC_HL_FAIL_ACT;
    else if ('m' == TOKEN_CHAR(4, 0))
        type = RTK_IPMC_GCT_IP6MC_MTU_FAIL_ACT;

    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&action), ret);
    DIAG_UTIL_MPRINTF("ip6mc %s action: %s\n", TOKEN_STR(4), text_ipmc_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_routing_exception_ip6mc_hl_fail_mtu_fail_action */
#endif

#ifdef CMD_IPMCAST_SET_ROUTING_EXCEPTION_IPMC_IP6MC_BAD_SIP_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER_HARD_DROP
/*
 * ipmcast set routing-exception ( ipmc | ip6mc ) bad-sip action ( drop | trap-to-cpu | trap-to-master | hard-drop )
 */
cparser_result_t
cparser_cmd_ipmcast_set_routing_exception_ipmc_ip6mc_bad_sip_action_drop_trap_to_cpu_trap_to_master_hard_drop(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_IPMC_BAD_SIP_ACT;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('6' == TOKEN_CHAR(3,2))
    {
        type = RTK_IPMC_GCT_IP6MC_BAD_SIP_ACT;
    }
    else
    {
        type = RTK_IPMC_GCT_IPMC_BAD_SIP_ACT;
    }

    DIAG_IPMC_PARSE_IPMC_ACT(6, action);

    DIAG_UTIL_MPRINTF("%s %s action: %s\n", TOKEN_STR(3), TOKEN_STR(4), text_ipmc_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_routing_exception_ipmc_ip6mc_bad_sip_action_drop_trap_to_cpu_trap_to_master_hard_drop */
#endif


#ifdef CMD_IPMCAST_GET_ROUTING_EXCEPTION_IPMC_IP6MC_ZERO_SIP_BAD_SIP_DMAC_MISMATCH_ACTION
/*
 * ipmcast get routing-exception ( ipmc | ip6mc ) ( zero-sip | bad-sip | dmac-mismatch ) action
 */
cparser_result_t
cparser_cmd_ipmcast_get_routing_exception_ipmc_ip6mc_zero_sip_bad_sip_dmac_mismatch_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('m' == TOKEN_CHAR(3,2))
    {
        if ('z' == TOKEN_CHAR(4, 0))
            type = RTK_IPMC_GCT_IPMC_ZERO_SIP_ACT;
        else if ('b' == TOKEN_CHAR(4, 0))
            type = RTK_IPMC_GCT_IPMC_BAD_SIP_ACT;
        else if ('d' == TOKEN_CHAR(4, 0))
            type = RTK_IPMC_GCT_IPMC_DMAC_MISMATCH_ACT;
    }
    else if ('6' == TOKEN_CHAR(3,2))
    {
        if ('z' == TOKEN_CHAR(4, 0))
            type = RTK_IPMC_GCT_IP6MC_ZERO_SIP_ACT;
        else if ('b' == TOKEN_CHAR(4, 0))
            type = RTK_IPMC_GCT_IP6MC_BAD_SIP_ACT;
        else if ('d' == TOKEN_CHAR(4, 0))
            type = RTK_IPMC_GCT_IP6MC_DMAC_MISMATCH_ACT;
    }

    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_get(unit, type, (int32 *)&action), ret);
    DIAG_UTIL_MPRINTF("%s %s action: %s\n", TOKEN_STR(3), TOKEN_STR(4), text_ipmc_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_routing_exception_ipmc_ip6mc_zero_sip_bad_sip_dmac_mismatch_action */
#endif

#ifdef CMD_IPMCAST_SET_ROUTING_EXCEPTION_IPMC_HEADER_OPTION_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER_HARD_DROP
/*
 * ipmcast set routing-exception ipmc header-option action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master | hard-drop )
 */
cparser_result_t
cparser_cmd_ipmcast_set_routing_exception_ipmc_header_option_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master_hard_drop(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_IPMC_HDR_OPT_ACT;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_IPMC_PARSE_IPMC_ACT(6, action);

    DIAG_UTIL_MPRINTF("ipmc %s action: %s\n", TOKEN_STR(4), text_ipmc_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_routing_exception_ipmc_header_option_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master_hard_drop */
#endif

#ifdef CMD_IPMCAST_SET_ROUTING_EXCEPTION_IP6MC_HEADER_ROUTER_HBH_ERROR_HBH_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER_HARD_DROP
/*
 * ipmcast set routing-exception ip6mc ( header-router | hbh-error | hbh ) action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master | hard-drop )
 */
cparser_result_t
cparser_cmd_ipmcast_set_routing_exception_ip6mc_header_router_hbh_error_hbh_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master_hard_drop(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_act_t action = RTK_IPMC_ACT_END;
    rtk_ipmc_globalCtrlType_t type = RTK_IPMC_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_IPMC_PARSE_IPMC_ACT(6, action);

    if (3 == osal_strlen(TOKEN_STR(4)))
        type = RTK_IPMC_GCT_IP6MC_HBH_ACT;
    else if ('e' == TOKEN_CHAR(4, 1))
        type = RTK_IPMC_GCT_IP6MC_HDR_ROUTE_ACT;
    else if ('b' == TOKEN_CHAR(4, 1))
        type = RTK_IPMC_GCT_IP6MC_HBH_ERR_ACT;

    DIAG_UTIL_MPRINTF("ip6mc %s action: %s\n", TOKEN_STR(4), text_ipmc_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_globalCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_set_routing_exception_ip6mc_header_router_hbh_error_hbh_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master_hard_drop */
#endif


#ifdef CMD_IPMCAST_ADD_STAT_MONITOR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_SRC_VLAN_VID_MONITOR_TYPE_BYTE_PACKET_BYTE_AND_PACKET
/*
 * ipmcast add stat-monitor vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> src-vlan <UINT:vid> monitor-type ( byte | packet | byte-and-packet )
 */
cparser_result_t
cparser_cmd_ipmcast_add_stat_monitor_vrf_id_vrf_id_sip_sip_dip_dip_src_vlan_vid_monitor_type_byte_packet_byte_and_packet(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    if (4 == osal_strlen(TOKEN_STR(12)) && ('b' == TOKEN_CHAR(12, 0)))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE;
    else if ('b' == TOKEN_CHAR(12, 0))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET;
    else if ('p' == TOKEN_CHAR(12, 0))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_PACKET;

    ipmcStat.key.vrf_id = *vrf_id_ptr;
    if (0 != *sip_ptr)
    {
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;
        ipmcStat.key.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcStat.key.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcStat.key.src_intf.vlan_id = *vid_ptr;

    DUMP_IPMC_STAT(ipmcStat, TRUE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statMont_create(unit, &ipmcStat), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_stat_monitor_vrf_id_vrf_id_sip_sip_dip_dip_src_vlan_vid_monitor_type_byte_packet_byte_and_packet */
#endif

#ifdef CMD_IPMCAST_ADD_STAT_MONITOR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_SRC_VLAN_VID_MONITOR_TYPE_BYTE_PACKET_BYTE_AND_PACKET
/*
 * ipmcast add stat-monitor vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> src-vlan <UINT:vid> monitor-type ( byte | packet | byte-and-packet )
 */
cparser_result_t
cparser_cmd_ipmcast_add_stat_monitor_vrf_id_vrf_id_sip6_sip_dip6_dip_src_vlan_vid_monitor_type_byte_packet_byte_and_packet(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));

    if (4 == osal_strlen(TOKEN_STR(12)) && ('b' == TOKEN_CHAR(12, 0)))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE;
    else if ('b' == TOKEN_CHAR(12, 0))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET;
    else if ('p' == TOKEN_CHAR(12, 0))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_PACKET;

    ipmcStat.key.flags |= RTK_IPMC_FLAG_IPV6;
    ipmcStat.key.vrf_id = *vrf_id_ptr;
    ipmcStat.key.src_intf.vlan_id = *vid_ptr;

    diag_util_str2ipv6(ipmcStat.key.src_ip_addr.ipv6.octet,*sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcStat.key.src_ip_addr.ipv6))
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;

    diag_util_str2ipv6(ipmcStat.key.dst_ip_addr.ipv6.octet,*dip_ptr);

    DUMP_IPMC_STAT(ipmcStat, TRUE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statMont_create(unit, &ipmcStat), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_stat_monitor_vrf_id_vrf_id_sip6_sip_dip6_dip_src_vlan_vid_monitor_type_byte_packet_byte_and_packet */
#endif

#ifdef CMD_IPMCAST_DEL_STAT_MONITOR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_SRC_VLAN_VID
/*
 * ipmcast del stat-monitor vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> src-vlan <UINT:vid>
 */
cparser_result_t
cparser_cmd_ipmcast_del_stat_monitor_vrf_id_vrf_id_sip_sip_dip_dip_src_vlan_vid(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET;

    ipmcStat.key.vrf_id = *vrf_id_ptr;
    if (0 != *sip_ptr)
    {
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;
        ipmcStat.key.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcStat.key.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcStat.key.src_intf.vlan_id = *vid_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statMont_destroy(unit, &ipmcStat), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_del_stat_monitor_vrf_id_vrf_id_sip_sip_dip_dipsrc_vlan_vid */
#endif

#ifdef CMD_IPMCAST_DEL_STAT_MONITOR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_SRC_VLAN_VID
/*
 * ipmcast del stat-monitor vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> src-vlan <UINT:vid>
 */
cparser_result_t
cparser_cmd_ipmcast_del_stat_monitor_vrf_id_vrf_id_sip6_sip_dip6_dip_src_vlan_vid(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET;

    ipmcStat.key.flags |= RTK_IPMC_FLAG_IPV6;
    ipmcStat.key.vrf_id = *vrf_id_ptr;
    ipmcStat.key.src_intf.vlan_id = *vid_ptr;

    diag_util_str2ipv6(ipmcStat.key.src_ip_addr.ipv6.octet,*sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcStat.key.src_ip_addr.ipv6))
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;

    diag_util_str2ipv6(ipmcStat.key.dst_ip_addr.ipv6.octet,*dip_ptr);

    DUMP_IPMC_STAT(ipmcStat,FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statMont_destroy(unit, &ipmcStat), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_del_stat_monitor_vrf_id_vrf_id_sip6_sip6_dip6_dip_src_vlan_vid */
#endif

#ifdef CMD_IPMCAST_ADD_STAT_MONITOR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_SRC_INTF_INTF_ID_MONITOR_TYPE_BYTE_PACKET_BYTE_AND_PACKET
/*
 * ipmcast add stat-monitor vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> src-intf <UINT:intf_id> monitor-type ( byte | packet | byte-and-packet )
 */
cparser_result_t
cparser_cmd_ipmcast_add_stat_monitor_vrf_id_vrf_id_sip_sip_dip_dip_src_intf_intf_id_monitor_type_byte_packet_byte_and_packet(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0x00, sizeof(ipmcStat));

    if (15 <= osal_strlen(TOKEN_STR(12)))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET;
    else if ('b' == TOKEN_CHAR(12, 0))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE;
    else if ('p' == TOKEN_CHAR(12, 0))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_PACKET;

    ipmcStat.key.vrf_id = *vrf_id_ptr;
    if (0 != *sip_ptr)
    {
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;
        ipmcStat.key.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcStat.key.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcStat.key.flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcStat.key.src_intf.intf_id = *intf_id_ptr;

    DUMP_IPMC_STAT(ipmcStat, TRUE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statMont_create(unit, &ipmcStat), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_stat_monitor_vrf_id_vrf_id_sip_sip_dip_dip_src_intf_intf_id_monitor_type_byte_packet_byte_and_packet */
#endif

#ifdef CMD_IPMCAST_ADD_STAT_MONITOR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_SRC_INTF_INTF_ID_MONITOR_TYPE_BYTE_PACKET_BYTE_AND_PACKET
/*
 * ipmcast add stat-monitor vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> src-intf <UINT:intf_id> monitor-type ( byte | packet | byte-and-packet )
 */
cparser_result_t
cparser_cmd_ipmcast_add_stat_monitor_vrf_id_vrf_id_sip6_sip_dip6_dip_src_intf_intf_id_monitor_type_byte_packet_byte_and_packet(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0x00, sizeof(ipmcStat));

    if (15 <= osal_strlen(TOKEN_STR(12)))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET;
    else if ('b' == TOKEN_CHAR(12, 0))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE;
    else if ('p' == TOKEN_CHAR(12, 0))
        ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_PACKET;

    ipmcStat.key.vrf_id = *vrf_id_ptr;
    ipmcStat.key.flags |= RTK_IPMC_FLAG_IPV6;
    diag_util_str2ipv6(ipmcStat.key.src_ip_addr.ipv6.octet, *sip_ptr);
    if (TRUE != diag_util_ipv6IsZero_ret(&ipmcStat.key.src_ip_addr.ipv6))
    {
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;
    }

    diag_util_str2ipv6(ipmcStat.key.dst_ip_addr.ipv6.octet, *dip_ptr);

    ipmcStat.key.flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcStat.key.src_intf.intf_id = *intf_id_ptr;

    DUMP_IPMC_STAT(ipmcStat, TRUE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statMont_create(unit, &ipmcStat), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_add_stat_monitor_vrf_id_vrf_id_sip6_sip_dip6_dip_src_intf_intf_id_monitor_type_byte_packet_byte_and_packet */
#endif

#ifdef CMD_IPMCAST_DEL_STAT_MONITOR_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_SRC_INTF_INTF_ID
/*
 * ipmcast del stat-monitor vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> src-intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_ipmcast_del_stat_monitor_vrf_id_vrf_id_sip_sip_dip_dip_src_intf_intf_id(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0x00, sizeof(ipmcStat));

    ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET;

    ipmcStat.key.vrf_id = *vrf_id_ptr;
    if (0 != *sip_ptr)
    {
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;
        ipmcStat.key.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcStat.key.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcStat.key.flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcStat.key.src_intf.intf_id = *intf_id_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statMont_destroy(unit, &ipmcStat), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_del_stat_monitor_vrf_id_vrf_id_sip_sip_dip_dip_src_intf_intf_id */
#endif

#ifdef CMD_IPMCAST_DEL_STAT_MONITOR_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_SRC_INTF_INTF_ID
/*
 * ipmcast del stat-monitor vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> src-intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_ipmcast_del_stat_monitor_vrf_id_vrf_id_sip6_sip_dip6_dip_src_intf_intf_id(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0x00, sizeof(ipmcStat));

    ipmcStat.mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET;

    ipmcStat.key.vrf_id = *vrf_id_ptr;
    ipmcStat.key.flags |= RTK_IPMC_FLAG_IPV6;
    diag_util_str2ipv6(ipmcStat.key.src_ip_addr.ipv6.octet, *sip_ptr);
    if (TRUE != diag_util_ipv6IsZero_ret(&ipmcStat.key.src_ip_addr.ipv6))
    {
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;
    }

    diag_util_str2ipv6(ipmcStat.key.dst_ip_addr.ipv6.octet, *dip_ptr);

    ipmcStat.key.flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcStat.key.src_intf.intf_id = *intf_id_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statMont_destroy(unit, &ipmcStat), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_del_stat_monitor_vrf_id_vrf_id_sip6_sip_dip6_dip_src_intf_intf_id */
#endif

#ifdef CMD_IPMCAST_RESET_STAT_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_SRC_VLAN_VID
/*
 * ipmcast reset stat vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> src-vlan <UINT:vid>
 */
cparser_result_t
cparser_cmd_ipmcast_reset_stat_vrf_id_vrf_id_sip_sip_dip_dip_src_vlan_vid(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    ipmcStat.key.vrf_id = *vrf_id_ptr;
    if (0 != *sip_ptr)
    {
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;
        ipmcStat.key.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcStat.key.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcStat.key.src_intf.vlan_id = *vid_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statCntr_reset(unit, &ipmcStat.key), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_reset_stat_vrf_id_vrf_id_sip_sip_dip_dip_src_vlan_vid */
#endif

#ifdef CMD_IPMCAST_RESET_STAT_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_SRC_VLAN_VID
/*
 * ipmcast reset stat vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> src-vlan <UINT:vid>
 */
cparser_result_t
cparser_cmd_ipmcast_reset_stat_vrf_id_vrf_id_sip6_sip_dip6_dip_src_vlan_vid(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    ipmcStat.key.flags |= RTK_IPMC_FLAG_IPV6;
    ipmcStat.key.vrf_id = *vrf_id_ptr;
    diag_util_str2ipv6(ipmcStat.key.src_ip_addr.ipv6.octet,*sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcStat.key.src_ip_addr.ipv6))
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;

    diag_util_str2ipv6(ipmcStat.key.dst_ip_addr.ipv6.octet,*dip_ptr);
    ipmcStat.key.src_intf.vlan_id = *vid_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statCntr_reset(unit, &ipmcStat.key), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_reset_stat_vrf_id_vrf_id_sip6_sip_dip6_dip_src_vlan_vid */
#endif

#ifdef CMD_IPMCAST_GET_STAT_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_SRC_VLAN_VID
/*
 * ipmcast get stat vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> src-vlan <UINT:vid>
 */
cparser_result_t
cparser_cmd_ipmcast_get_stat_vrf_id_vrf_id_sip_sip_dip_dip_src_vlan_vid(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;
    rtk_ipmc_statCntr_t     statCnt;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    osal_memset(&statCnt, 0, sizeof(statCnt));
    ipmcStat.key.vrf_id = *vrf_id_ptr;
    if (0 != *sip_ptr)
    {
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;
        ipmcStat.key.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcStat.key.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcStat.key.src_intf.vlan_id = *vid_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statCntr_get(unit, &ipmcStat.key,&statCnt), ret);
    DIAG_UTIL_MPRINTF("Bytes         : %llu\n", statCnt.rxByte);
    DIAG_UTIL_MPRINTF("Packets       : %llu\n", statCnt.rxPacket);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_stat_vrf_id_vrf_id_sip_sip_dip_dip_src_vlan_vid */
#endif

#ifdef CMD_IPMCAST_GET_STAT_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_SRC_VLAN_VID
/*
 * ipmcast get stat vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> src-vlan <UINT:vid>
 */
cparser_result_t
cparser_cmd_ipmcast_get_stat_vrf_id_vrf_id_sip6_sip_dip6_dip_src_vlan_vid(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;
    rtk_ipmc_statCntr_t     statCnt;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    ipmcStat.key.vrf_id = *vrf_id_ptr;
    ipmcStat.key.flags |= RTK_IPMC_FLAG_IPV6;
    diag_util_str2ipv6(ipmcStat.key.src_ip_addr.ipv6.octet,*sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcStat.key.src_ip_addr.ipv6))
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;

    diag_util_str2ipv6(ipmcStat.key.dst_ip_addr.ipv6.octet,*dip_ptr);
    ipmcStat.key.src_intf.vlan_id = *vid_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statCntr_get(unit, &ipmcStat.key,&statCnt), ret);
    DIAG_UTIL_MPRINTF("Bytes         : %llu\n", statCnt.rxByte);
    DIAG_UTIL_MPRINTF("Packets       : %llu\n", statCnt.rxPacket);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_stat_vrf_id_vrf_id_sip6_sip_dip6_dip_src_vlan_vid */
#endif

#ifdef CMD_IPMCAST_RESET_STAT_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_SRC_INTF_INTF_ID
/*
 * ipmcast reset stat vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> src-intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_ipmcast_reset_stat_vrf_id_vrf_id_sip_sip_dip_dip_src_intf_intf_id(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    ipmcStat.key.vrf_id = *vrf_id_ptr;
    ipmcStat.key.flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    if (0 != *sip_ptr)
    {
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;
        ipmcStat.key.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcStat.key.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcStat.key.src_intf.intf_id = *intf_id_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statCntr_reset(unit, &ipmcStat.key), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_reset_stat_vrf_id_vrf_id_sip_sip_dip_dip_src_intf_intf_id */
#endif

#ifdef CMD_IPMCAST_RESET_STAT_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_SRC_INTF_INTF_ID
/*
 * ipmcast reset stat vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> src-intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_ipmcast_reset_stat_vrf_id_vrf_id_sip6_sip_dip6_dip_src_intf_intf_id(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    ipmcStat.key.flags |= RTK_IPMC_FLAG_IPV6;
    ipmcStat.key.flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    ipmcStat.key.vrf_id = *vrf_id_ptr;
    diag_util_str2ipv6(ipmcStat.key.src_ip_addr.ipv6.octet,*sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcStat.key.src_ip_addr.ipv6))
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;

    diag_util_str2ipv6(ipmcStat.key.dst_ip_addr.ipv6.octet,*dip_ptr);
    ipmcStat.key.src_intf.intf_id = *intf_id_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statCntr_reset(unit, &ipmcStat.key), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_reset_stat_vrf_id_vrf_id_sip6_sip_dip6_dip_src_intf_intf_id */
#endif

#ifdef CMD_IPMCAST_GET_STAT_VRF_ID_VRF_ID_SIP_SIP_DIP_DIP_SRC_INTF_INTF_ID
/*
 * ipmcast get stat vrf-id <UINT:vrf_id> sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> src-intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_ipmcast_get_stat_vrf_id_vrf_id_sip_sip_dip_dip_src_intf_intf_id(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;
    rtk_ipmc_statCntr_t     statCnt;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    osal_memset(&statCnt, 0, sizeof(statCnt));
    ipmcStat.key.vrf_id = *vrf_id_ptr;
    ipmcStat.key.flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    if (0 != *sip_ptr)
    {
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;
        ipmcStat.key.src_ip_addr.ipv4 = *sip_ptr;
    }
    ipmcStat.key.dst_ip_addr.ipv4 = *dip_ptr;
    ipmcStat.key.src_intf.intf_id = *intf_id_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statCntr_get(unit, &ipmcStat.key,&statCnt), ret);
    DIAG_UTIL_MPRINTF("Bytes         : %llu\n", statCnt.rxByte);
    DIAG_UTIL_MPRINTF("Packets       : %llu\n", statCnt.rxPacket);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_stat_vrf_id_vrf_id_sip_sip_dip_dip_src_intf_intf_id */
#endif

#ifdef CMD_IPMCAST_GET_STAT_VRF_ID_VRF_ID_SIP6_SIP_DIP6_DIP_SRC_INTF_INTF_ID
/*
 * ipmcast get stat vrf-id <UINT:vrf_id> sip6 <IPV6ADDR:sip> dip6 <IPV6ADDR:dip> src-intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_ipmcast_get_stat_vrf_id_vrf_id_sip6_sip_dip6_dip_src_intf_intf_id(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_ipmc_statMont_t     ipmcStat;
    rtk_ipmc_statCntr_t     statCnt;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&ipmcStat, 0, sizeof(ipmcStat));
    ipmcStat.key.vrf_id = *vrf_id_ptr;
    ipmcStat.key.flags |= RTK_IPMC_FLAG_IPV6;
    ipmcStat.key.flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
    diag_util_str2ipv6(ipmcStat.key.src_ip_addr.ipv6.octet,*sip_ptr);
    if (!diag_util_ipv6IsZero_ret(&ipmcStat.key.src_ip_addr.ipv6))
        ipmcStat.key.flags |= RTK_IPMC_FLAG_SIP;

    diag_util_str2ipv6(ipmcStat.key.dst_ip_addr.ipv6.octet,*dip_ptr);
    ipmcStat.key.src_intf.intf_id = *intf_id_ptr;

    DUMP_IPMC_STAT(ipmcStat, FALSE);
    DIAG_UTIL_ERR_CHK(rtk_ipmc_statCntr_get(unit, &ipmcStat.key,&statCnt), ret);
    DIAG_UTIL_MPRINTF("Bytes         : %llu\n", statCnt.rxByte);
    DIAG_UTIL_MPRINTF("Packets       : %llu\n", statCnt.rxPacket);

    return CPARSER_OK;
}   /* end of cparser_cmd_ipmcast_get_stat_vrf_id_vrf_id_sip6_sip_dip6_dip_src_intf_intf_id */
#endif

