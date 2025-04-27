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
 * Purpose : Definition those L3 command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
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
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <osal/memory.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_l2.h>
  #include <rtrpc/rtrpc_l3.h>
#endif
const char text_l3_cputarget[RTK_L3_CPUTARGET_END+1][32] =
{
    "Local CPU",        /* RTK_L3_CPUTARGET_LOCAL */
    "Master CPU",       /* RTK_L3_CPUTARGET_MASTER */
    "RTK_L3_CPUTARGET_END, remove it",  /* for model code check error only */
};

const char text_urpf_mode[RTK_L3_URPF_MODE_END+1][32] =
{
    "Loose",
    "Strict",
    "L3_URPF_MODE_END , remove it",
};

const char text_urpf_base[RTK_L3_URPF_BASE_END+1][32] =
{
    "Interface Based",
    "Port Based",
    "L3_URPF_BASE_END , remove it",
};

const char text_l3_flag[12][32] =
{
    "none",
    "IPv6",
    "mc_key_intf",
    "replace",
    "null_intf",
    "ttl_dec_ignore",
    "ttl_chk_ignore",
    "qos_assign",
    "hit",
    "hit_clear",
    "negate",
    "path_with_nexthop",
};

#define L3_FLAG2STR(__f, __s)                                               \
do {                                                                        \
    uint32 __i = 0;                                                         \
    uint32 __l = 0;                                                         \
    for (__i = 0; __i < 16 ; __i++) {                                       \
        if (0 != (__f & (1<< __i)))                                         \
            __l += osal_sprintf(__s + __l, " %s |", text_l3_flag[__i+1]);   \
    }                                                                       \
} while(0);


#define DUMP_HOST(__h)                                                      \
do {                                                                        \
    char __fs[128];                                                         \
    char __is[128];                                                         \
    char __ms[32];                                                          \
    osal_memset(__fs, 0 , sizeof(__fs));                                    \
    osal_memset(__is, 0 , sizeof(__is));                                    \
    L3_FLAG2STR(__h.flags, __fs);                                           \
    DIAG_UTIL_MPRINTF("Route Entry    : \n");                               \
    DIAG_UTIL_MPRINTF("Flag           : 0x%x (%s)\n", __h.flags, __fs);     \
    DIAG_UTIL_MPRINTF("VRF ID         : %u\n", __h.vrf_id);                 \
    if (__h.flags & RTK_L3_FLAG_IPV6) {                                     \
        diag_util_ipv62str(__is, (uint8 *)__h.ip_addr.ipv6.octet);          \
        DIAG_UTIL_MPRINTF("IPv6 Addr      : %s\n", __is);                   \
    } else {                                                                \
        diag_util_ip2str(__is, __h.ip_addr.ipv4);                           \
        DIAG_UTIL_MPRINTF("IPv4 Addr      : %s\n", __is);                   \
    }                                                                       \
    DIAG_UTIL_MPRINTF("Path ID        : %u\n", __h.path_id);                \
    diag_util_mac2str(__ms, __h.nexthop.mac_addr.octet);                    \
    DIAG_UTIL_MPRINTF("Intf ID        : %u\n", __h.nexthop.intf_id);        \
    DIAG_UTIL_MPRINTF("Mac            : %s\n", __ms);                       \
    DIAG_UTIL_MPRINTF("Forward Action : %s\n", text_action[__h.fwd_act]);   \
    DIAG_UTIL_MPRINTF("QoS Priority   : %u\n", __h.qos_pri);                \
} while(0);

#if defined(CONFIG_SDK_RTL9300)
#define DUMP_ROUTE(__r)                                                     \
do {                                                                        \
    char __fs[128];                                                         \
    char __is[128];                                                         \
    char __ms[32];                                                          \
    osal_memset(__fs, 0 , sizeof(__fs));                                    \
    osal_memset(__is, 0 , sizeof(__is));                                    \
    L3_FLAG2STR(__r.flags, __fs);                                           \
    DIAG_UTIL_MPRINTF("Route Entry    : \n");                               \
    DIAG_UTIL_MPRINTF("Flag           : 0x%x (%s)\n", __r.flags, __fs);     \
    DIAG_UTIL_MPRINTF("VRF ID         : %u\n", __r.vrf_id);                 \
    if (__r.flags & RTK_L3_FLAG_IPV6) {                                     \
        diag_util_ipv62str(__is, (uint8 *)__r.ip_addr.ipv6.octet);          \
        DIAG_UTIL_MPRINTF("IPv6 Addr      : %s\n", __is);                   \
        DIAG_UTIL_MPRINTF("IPv6 Prefix    : %u\n", __r.prefix_len);         \
        DIAG_UTIL_MPRINTF("IPv6 Suffix    : %u\n", __r.suffix_len);         \
    } else {                                                                \
        diag_util_ip2str(__is, __r.ip_addr.ipv4);                           \
        DIAG_UTIL_MPRINTF("IPv4 Addr      : %s\n", __is);                   \
        DIAG_UTIL_MPRINTF("IPv4 Prefix    : %u\n", __r.prefix_len);         \
        DIAG_UTIL_MPRINTF("IPv4 Suffix    : %u\n", __r.suffix_len);         \
    }                                                                       \
    DIAG_UTIL_MPRINTF("Path ID        : %u\n", __r.path_id);                \
    diag_util_mac2str(__ms, __r.nexthop.mac_addr.octet);                    \
    DIAG_UTIL_MPRINTF("Intf ID        : %u\n", __r.nexthop.intf_id);        \
    DIAG_UTIL_MPRINTF("Mac            : %s\n", __ms);                       \
    DIAG_UTIL_MPRINTF("Forward Action : %s\n", text_action[__r.fwd_act]);   \
    DIAG_UTIL_MPRINTF("QoS Priority   : %u\n", __r.qos_pri);                \
} while(0);

#define DUMP_L3_HL(__isHost, __isIpv6)                                                                                                                              \
do {                                                                                                                                                                \
    if (__isHost) {                                                                                                                                                 \
        if (__isIpv6) {                                                                                                                                             \
            DIAG_UTIL_MPRINTF("Index |VRF ID|              IPv6 Address               |Path|Pri |   Forward Action   | Flag  \n");                                  \
            DIAG_UTIL_MPRINTF("------+------+-----------------------------------------+----+----+--------------------+----------------------\n");                   \
        } else {                                                                                                                                                    \
            DIAG_UTIL_MPRINTF("Index |VRF ID| IPv4 Address   |Path|Pri |   Forward Action   | Flag  \n");                                                           \
            DIAG_UTIL_MPRINTF("------+------+----------------+----+----+--------------------+----------------------\n");                                            \
        }                                                                                                                                                           \
    } else {                                                                                                                                                        \
        if (__isIpv6) {                                                                                                                                             \
            DIAG_UTIL_MPRINTF("Index |VRF ID|              IPv6 Address               |Pre-len|Suf-len|Path|Pri |   Forward Action   | Flag  \n");                  \
            DIAG_UTIL_MPRINTF("------+------+-----------------------------------------+-------+-------+----+----+--------------------+----------------------\n");   \
        } else {                                                                                                                                                    \
            DIAG_UTIL_MPRINTF("Index |VRF ID| IPv4 Address   |Pre-len|Suf-len|Path|Pri |   Forward Action   | Flag  \n");                                           \
            DIAG_UTIL_MPRINTF("------+------+----------------+-------+-------+----+----+--------------------+----------------------\n");                            \
        }                                                                                                                                                           \
    }                                                                                                                                                               \
} while(0);

#define DUMP_L3_ROUTE_ENTRY(__idx, __isIpv6, __ent)                                                                                         \
do {                                                                                                                                        \
    char __fs[128];                                                                                                                         \
    char __is[128];                                                                                                                         \
    osal_memset(__fs, 0 , sizeof(__fs));                                                                                                    \
    osal_memset(__is, 0 , sizeof(__is));                                                                                                    \
    L3_FLAG2STR(__ent.flags, __fs);                                                                                                         \
    if (__isIpv6) {                                                                                                                         \
        diag_util_ipv62str(__is, (uint8 *)__ent.ip_addr.ipv6.octet);                                                                        \
        DIAG_UTIL_MPRINTF("%5d | %4d |%40s | %4d  | %4d  |%4d|%4d| %18s | %s \n",                                                           \
            __idx, __ent.vrf_id, __is,__ent.prefix_len,__ent.suffix_len, __ent.path_id,  __ent.qos_pri, text_action[__ent.fwd_act], __fs);  \
    } else {                                                                                                                                \
        diag_util_ip2str(__is, __ent.ip_addr.ipv4);                                                                                         \
        DIAG_UTIL_MPRINTF("%5d | %4d |%16s| %4d  | %4d  |%4d|%4d| %18s | %s \n",                                                            \
            __idx, __ent.vrf_id, __is,__ent.prefix_len,__ent.suffix_len, __ent.path_id,  __ent.qos_pri, text_action[__ent.fwd_act], __fs);  \
    }                                                                                                                                       \
} while(0);

#else  /* CONFIG_SDK_RTL9310 */
#define DUMP_ROUTE(__r)                                                     \
do {                                                                        \
    char __fs[128];                                                         \
    char __is[128];                                                         \
    osal_memset(__fs, 0 , sizeof(__fs));                                    \
    osal_memset(__is, 0 , sizeof(__is));                                    \
    L3_FLAG2STR(__r.flags, __fs);                                           \
    DIAG_UTIL_MPRINTF("Route Entry    : \n");                               \
    DIAG_UTIL_MPRINTF("Flag           : 0x%x (%s)\n", __r.flags, __fs);     \
    DIAG_UTIL_MPRINTF("VRF ID         : %u\n", __r.vrf_id);                 \
    if (__r.flags & RTK_L3_FLAG_IPV6) {                                     \
        diag_util_ipv62str(__is, (uint8 *)__r.ip_addr.ipv6.octet);          \
        DIAG_UTIL_MPRINTF("IPv6 Addr      : %s\n", __is);                   \
        DIAG_UTIL_MPRINTF("IPv6 Prefix    : %u\n", __r.prefix_len);         \
    } else {                                                                \
        diag_util_ip2str(__is, __r.ip_addr.ipv4);                           \
        DIAG_UTIL_MPRINTF("IPv4 Addr      : %s\n", __is);                   \
        DIAG_UTIL_MPRINTF("IPv4 Prefix    : %u\n", __r.prefix_len);         \
    }                                                                       \
    DIAG_UTIL_MPRINTF("Path ID        : %u\n", __r.path_id);                \
    DIAG_UTIL_MPRINTF("Forward Action : %s\n", text_action[__r.fwd_act]);   \
    DIAG_UTIL_MPRINTF("QoS Priority   : %u\n", __r.qos_pri);                \
} while(0);

#define DUMP_L3_HL(__isHost, __isIpv6)                                                                                                                      \
do {                                                                                                                                                        \
    if (__isHost) {                                                                                                                                         \
        if (__isIpv6) {                                                                                                                                     \
            DIAG_UTIL_MPRINTF("Index |VRF ID|              IPv6 Address               |Path|Pri |   Forward Action   | Flag  \n");                          \
            DIAG_UTIL_MPRINTF("------+------+-----------------------------------------+----+----+--------------------+----------------------\n");           \
        } else {                                                                                                                                            \
            DIAG_UTIL_MPRINTF("Index |VRF ID| IPv4 Address   |Path|Pri |   Forward Action   | Flag  \n");                                                   \
            DIAG_UTIL_MPRINTF("------+------+----------------+----+----+--------------------+----------------------\n");                                    \
        }                                                                                                                                                   \
    } else {                                                                                                                                                \
        if (__isIpv6) {                                                                                                                                     \
            DIAG_UTIL_MPRINTF("Index |VRF ID|              IPv6 Address               |Pre-len|Path|Pri|   Forward Action   | Flag  \n");                   \
            DIAG_UTIL_MPRINTF("------+------+-----------------------------------------+-------+----+----+--------------------+----------------------\n");   \
        } else {                                                                                                                                            \
            DIAG_UTIL_MPRINTF("Index |VRF ID| IPv4 Address   |Pre-len|Path|Pri |   Forward Action   | Flag  \n");                                           \
            DIAG_UTIL_MPRINTF("------+------+----------------+-------+----+----+--------------------+----------------------\n");                            \
        }                                                                                                                                                   \
    }                                                                                                                                                       \
} while(0);

#define DUMP_L3_ROUTE_ENTRY(__idx, __isIpv6, __ent)                                                                         \
do {                                                                                                                        \
    char __fs[128];                                                                                                         \
    char __is[128];                                                                                                         \
    osal_memset(__fs, 0 , sizeof(__fs));                                                                                    \
    osal_memset(__is, 0 , sizeof(__is));                                                                                    \
    L3_FLAG2STR(__ent.flags, __fs);                                                                                         \
    if (__isIpv6) {                                                                                                         \
        diag_util_ipv62str(__is, (uint8 *)__ent.ip_addr.ipv6.octet);                                                        \
        DIAG_UTIL_MPRINTF("%5d | %4d |%40s | %4d  |%4d|%4d| %18s | %s \n",                                                  \
            __idx, __ent.vrf_id, __is,__ent.prefix_len, __ent.path_id,  __ent.qos_pri, text_action[__ent.fwd_act], __fs);   \
    } else {                                                                                                                \
        diag_util_ip2str(__is, __ent.ip_addr.ipv4);                                                                         \
        DIAG_UTIL_MPRINTF("%5d | %4d |%16s| %4d  |%4d|%4d| %18s | %s \n",                                                   \
            __idx, __ent.vrf_id, __is,__ent.prefix_len, __ent.path_id,  __ent.qos_pri, text_action[__ent.fwd_act], __fs);   \
    }                                                                                                                       \
} while(0);
#endif

#define DUMP_L3_HOST_ENTRY(__idx, __isIpv6, __ent)                                                      \
do {                                                                                                    \
    char __fs[128];                                                                                     \
    char __is[128];                                                                                     \
    osal_memset(__fs, 0 , sizeof(__fs));                                                                \
    osal_memset(__is, 0 , sizeof(__is));                                                                \
    L3_FLAG2STR(__ent.flags, __fs);                                                                     \
    if (__isIpv6) {                                                                                     \
        diag_util_ipv62str(__is, (uint8 *)__ent.ip_addr.ipv6.octet);                                    \
        DIAG_UTIL_MPRINTF("%5d | %4d |%40s |%4d|%4d| %18s | %s \n",                                     \
            __idx, __ent.vrf_id, __is,__ent.path_id, __ent.qos_pri, text_action[__ent.fwd_act], __fs);  \
    } else {                                                                                            \
        diag_util_ip2str(__is, __ent.ip_addr.ipv4);                                                     \
        DIAG_UTIL_MPRINTF("%5d | %4d |%16s|%4d|%4d| %18s | %s \n",                                      \
            __idx, __ent.vrf_id, __is,__ent.path_id, __ent.qos_pri,  text_action[__ent.fwd_act], __fs); \
    }                                                                                                   \
} while(0);

#define DUMP_INTF(__i)                                      \
do {                                                        \
    char __ms[32];                                          \
    osal_memset(&__ms, 0, sizeof(__ms));                    \
    diag_util_mac2str(__ms, __i.mac_addr.octet);            \
    DIAG_UTIL_MPRINTF("Interface : %u \n", __i.intf_id);    \
    DIAG_UTIL_MPRINTF("vrf_id    : %u\n", __i.vrf_id);      \
    DIAG_UTIL_MPRINTF("vid       : %u\n", __i.vid);         \
    DIAG_UTIL_MPRINTF("mac       : %s\n", __ms);            \
    DIAG_UTIL_MPRINTF("mtu       : %u\n", __i.mtu);         \
    DIAG_UTIL_MPRINTF("ipv6_mtu  : %u\n", __i.ipv6_mtu);    \
    DIAG_UTIL_MPRINTF("ttl       : %u\n", __i.ttl);         \
} while(0);

#define DUMP_NH(__n, __id)  \
do {                                                        \
    char __ms[32];                                          \
    osal_memset(&__ms, 0, sizeof(__ms));                    \
    diag_util_mac2str(__ms, __n.mac_addr.octet);            \
    DIAG_UTIL_MPRINTF("   NextHop ID : %u\n", __id);        \
    DIAG_UTIL_MPRINTF(" Interface ID : %u\n", __n.intf_id); \
    DIAG_UTIL_MPRINTF("  MAC address : %s\n", __ms);        \
    DIAG_UTIL_MPRINTF("    L3 action : %s\n", text_l3_action[__n.l3_act]);  \
} while(0);

/* Line Header */
#define DUMP_INTF_LH()                                                                      \
do {                                                                                        \
    DIAG_UTIL_MPRINTF(" Intf ID | VRF ID |  VLAN  |       MAC        |IPv4 MTU|IPv6 MTU| TTL   \n");   \
    DIAG_UTIL_MPRINTF("---------+--------+--------+------------------+--------+--------+----\n");      \
} while(0);

#define DUMP_INTF_ENTRY(__intf)                                                     \
do {                                                                                \
    char __ms[32];                                                                  \
    osal_memset(&__ms, 0, sizeof(__ms));                                            \
    diag_util_mac2str(__ms, __intf.mac_addr.octet);                                 \
        DIAG_UTIL_MPRINTF("  %4d   |  %4d  | %4d   |%s |  %4d  |  %4d  | %d  \n",             \
            __intf.intf_id,__intf.vrf_id,__intf.vid,__ms,__intf.mtu,__intf.ipv6_mtu,__intf.ttl);    \
} while(0);

/* Line Header */
#define DUMP_NH_LH()                                                        \
do {                                                                        \
    DIAG_UTIL_MPRINTF(" NextHop ID | Intf ID |       MAC         | Action \n");    \
    DIAG_UTIL_MPRINTF("------------+---------+-------------------+--------\n");     \
} while(0);

#define DUMP_NH_ENTRY(__nh, __id)                           \
do {                                                        \
    char __ms[32];                                          \
    osal_memset(&__ms, 0, sizeof(__ms));                    \
    diag_util_mac2str(__ms, __nh.mac_addr.octet);           \
        DIAG_UTIL_MPRINTF("  %4d      |  %4d   | %16s | %s  \n",  \
            __id,__nh.intf_id,__ms,text_l3_action[__nh.l3_act]);                        \
} while(0);

/* Line Header */
#define DUMP_ECMP_LH()                                                              \
do {                                                                                \
    DIAG_UTIL_MPRINTF(" No. | Nexthop ID |  Interface | MAC address       | action\n");   \
    DIAG_UTIL_MPRINTF("-----+------------+------------+-------------------+------------------\n");   \
} while (0)

#define DUMP_ECMP_ENTRY(__no, __nhId, __intfId, __macAddr, __l3_act)    \
do {                                                        \
    char __ms[32];                                          \
    osal_memset(&__ms, 0, sizeof(__ms));                    \
    diag_util_mac2str(__ms, __macAddr.octet);               \
    DIAG_UTIL_MPRINTF(" %3d | 0x%08X | 0x%08X | %s | %s\n", \
        __no, __nhId, __intfId, __ms, text_l3_action[__l3_act]);  \
} while (0)

#define L3_ECMP_MAX             (8)
#define L3_VRID_MAX             (256)


#ifdef CMD_L3_DUMP_ROUTE_SWITCH_MAC
/*
 * l3 dump route switch-mac
 */
cparser_result_t cparser_cmd_l3_dump_route_switch_mac(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      maxIndex;
    uint32      index;
    char        macStr[24];
    rtk_mac_t   mac;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_OM_GET_CHIP_CAPACITY(unit, maxIndex, max_num_of_route_switch_addr);

    for (index = 0; index < maxIndex; index++)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_routeSwitchMacAddr_get(unit, index, &mac), ret);

        if ((mac.octet[0] != 0) || (mac.octet[1] != 0) || (mac.octet[2] != 0) ||
            (mac.octet[3] != 0) || (mac.octet[4] != 0) || (mac.octet[5] != 0))
        {
            DIAG_UTIL_ERR_CHK(diag_util_mac2str(macStr, mac.octet), ret);
            DIAG_UTIL_MPRINTF("Index : %2d, ", index);
            DIAG_UTIL_MPRINTF("Routing Switch MAC Address : %s\n", macStr);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_GET_ROUTE_SWITCH_MAC_INDEX
/*
 * l3 get route switch-mac <UINT:index>
 */
cparser_result_t cparser_cmd_l3_get_route_switch_mac_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    char    macStr[32];
    rtk_mac_t mac;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_routeSwitchMacAddr_get(unit, *index_ptr, &mac), ret);
    DIAG_UTIL_ERR_CHK(diag_util_mac2str(macStr, mac.octet), ret);
    diag_util_printf("Routing Switch MAC Address : %s\n", macStr);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_SET_ROUTE_SWITCH_MAC_INDEX_MAC
/*
 * l3 set route switch-mac <UINT:index> <MACADDR:mac>
 */
cparser_result_t cparser_cmd_l3_set_route_switch_mac_index_mac(cparser_context_t *context,
    uint32_t *index_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_routeSwitchMacAddr_set(unit, *index_ptr, (rtk_mac_t *)mac_ptr), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_DUMP_ROUTE_ROUTE_TABLE
/*
 * l3 dump route route-table
 */
cparser_result_t cparser_cmd_l3_dump_route_route_table(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    uint32 scan_idx = 0;
    uint32 total_entry = 0;
    rtk_l3_routeEntry_t entry;
    char   macStr[32];
    uint32 routeEntries;
    uint8  zero_mac[ETHER_ADDR_LEN]= {0};

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_MPRINTF("Index | Host MAC Address | Switch MAC Index\n");
        DIAG_UTIL_MPRINTF("------+------------------+------------------\n");
    }
    else
    {
        DIAG_UTIL_MPRINTF("Index | Host MAC Address \n");
        DIAG_UTIL_MPRINTF("------+------------------\n");
    }

    DIAG_OM_GET_CHIP_CAPACITY(unit, routeEntries, max_num_of_route_host_addr);
    for (scan_idx=0; scan_idx < routeEntries; scan_idx++)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_routeEntry_get(unit, scan_idx, &entry), ret);

        if (memcmp(entry.hostMac.octet, zero_mac, ETHER_ADDR_LEN) != 0)
        {
            DIAG_UTIL_ERR_CHK(diag_util_mac2str(macStr, entry.hostMac.octet), ret);

            if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
            {
#if defined(CONFIG_SDK_RTL8390)
                DIAG_UTIL_MPRINTF("%5d | %16s | %16d \n", scan_idx, macStr, entry.swMac_idx);
#endif
            }
            else
                DIAG_UTIL_MPRINTF("%5d | %16s \n", scan_idx, macStr);

            total_entry++;
        }
    }

    DIAG_UTIL_MPRINTF("\nTotal Number Of Entries :%d\n", total_entry);
    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_GET_ROUTE_ROUTE_TABLE_INDEX
/*
 * l3 get route route-table <UINT:index>
 */
cparser_result_t cparser_cmd_l3_get_route_route_table_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_l3_routeEntry_t entry;
    char   macStr[32];

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_routeEntry_get(unit, *index_ptr, &entry), ret);
    DIAG_UTIL_ERR_CHK(diag_util_mac2str(macStr, entry.hostMac.octet), ret);

    diag_util_printf("Route Entry [%d]\n", *index_ptr);
    diag_util_printf("Host MAC Address : %s\n", macStr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_printf("Switch MAC Index : %u\n", entry.swMac_idx);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_SET_ROUTE_ROUTE_TABLE_INDEX_HOST_MAC_MAC
/*
 * l3 set route route-table <UINT:index> host-mac <MACADDR:mac>
 */
cparser_result_t cparser_cmd_l3_set_route_route_table_index_host_mac_mac(cparser_context_t *context,
    uint32_t *index_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_l3_routeEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_routeEntry_get(unit, *index_ptr, &entry), ret);
    osal_memcpy((void*)&entry.hostMac, (void*)mac_ptr, sizeof(cparser_macaddr_t));
    DIAG_UTIL_ERR_CHK(rtk_l3_routeEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_SET_ROUTE_ROUTE_TABLE_INDEX_SWITCH_MAC_INDEX_SWITCH_MAC_INDEX
/*
 * l3 set route route-table <UINT:index> switch-mac-index <UINT:switch_mac_index>
 */
cparser_result_t cparser_cmd_l3_set_route_route_table_index_switch_mac_index_switch_mac_index(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *switch_mac_index_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_l3_routeEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_routeEntry_get(unit, *index_ptr, &entry), ret);
    entry.swMac_idx = *switch_mac_index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_routeEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_L3_GET_INFO
/*
 * l3 get info
 */
cparser_result_t
cparser_cmd_l3_get_info(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_info_t l3Info;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_info_get(unit, &l3Info), ret);

    DIAG_UTIL_MPRINTF("\n L3 information\n");
    DIAG_UTIL_MPRINTF("--------------------------------------\n");
    DIAG_UTIL_MPRINTF("       MAC : %5u / %5u (used/max)\n", l3Info.mac_used, l3Info.mac_max);
    DIAG_UTIL_MPRINTF("    VRF ID : %5u / %5u (used/max)\n", l3Info.vrf_used, l3Info.vrf_max);
    DIAG_UTIL_MPRINTF(" Interface : %5u / %5u (used/max)\n", l3Info.intf_used, l3Info.intf_max);
    DIAG_UTIL_MPRINTF("  I/f MTUs : %5u / %5u (used/max)\n", l3Info.intf_mtu_used, l3Info.intf_mtu_max);
    DIAG_UTIL_MPRINTF(" IPv6 MTUs : %5u / %5u (used/max)\n", l3Info.intf_ipv6_mtu_used, l3Info.intf_ipv6_mtu_max);
    DIAG_UTIL_MPRINTF("      ECMP : %5u / %5u (used/max)\n", l3Info.ecmp_used, l3Info.ecmp_max);
    DIAG_UTIL_MPRINTF("   Nexthop : %5u / %5u (used/max)\n", l3Info.nexthop_used, l3Info.nexthop_max);
    DIAG_UTIL_MPRINTF(" IPv4 Host : %5u / %5u (used/max)\n", l3Info.host_ipv4_used, l3Info.host_ipv4_max);
    DIAG_UTIL_MPRINTF("IPv4 Route : %5u / %5u (used/max)\n", l3Info.route_ipv4_used, l3Info.route_ipv4_max);
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_info */
#endif

#ifdef CMD_L3_GET_ROUTER_MAC_ENTRY_INDEX
/*
 * l3 get router-mac entry <UINT:index>
 */
cparser_result_t
cparser_cmd_l3_get_router_mac_entry_index(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    uint32  index = *index_ptr;
    rtk_l3_routerMacEntry_t entry;
    int32   ret = RT_ERR_FAILED;
    char    macStr[24], macMskStr[24];

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_get(unit, index, &entry), ret);

    DIAG_UTIL_MPRINTF("\n Router MAC entry information\n");
    DIAG_UTIL_MPRINTF("------------------------------------------------------------\n");
    DIAG_UTIL_MPRINTF("   entry index : %d\n", index);
    DIAG_UTIL_MPRINTF("        enable : %d\n", entry.enable);
    DIAG_UTIL_MPRINTF("     port type : %d / 0x%01X\n", entry.port_type, entry.port_type_mask);
    DIAG_UTIL_MPRINTF("                 0 - individual\n");
    DIAG_UTIL_MPRINTF("                 1 - trunk\n");
    DIAG_UTIL_MPRINTF(" port trunk ID : %d / 0x%X\n", entry.port_trunk_id, entry.port_trunk_id_mask);
    #if defined(CONFIG_SDK_RTL9300)
    DIAG_UTIL_MPRINTF("       VLAN ID : %d / 0x%X\n", entry.vid, entry.vid_mask);
    #endif
    #if defined(CONFIG_SDK_RTL9310)
    DIAG_UTIL_MPRINTF("  interface ID : %d / 0x%X\n", entry.intf_id, entry.intf_id_mask);
    #endif
    DIAG_UTIL_ERR_CHK(diag_util_mac2str(macStr, entry.mac.octet), ret);
    DIAG_UTIL_ERR_CHK(diag_util_mac2str(macMskStr, entry.mac_mask.octet), ret);
    DIAG_UTIL_MPRINTF("   MAC Address : %s / %s\n", macStr, macMskStr);
    DIAG_UTIL_MPRINTF("     L3 action : %s\n", text_l3_action[entry.l3_act]);
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_router_mac_entry_index */
#endif

#ifdef CMD_L3_SET_ROUTER_MAC_ENTRY_INDEX_STATE_DISABLE_ENABLE
/*
 * l3 set router-mac entry <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l3_set_router_mac_entry_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    uint32  index = *index_ptr;
    rtk_l3_routerMacEntry_t entry;
    rtk_enable_t state = RTK_ENABLE_END;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(6, state);

    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_get(unit, index, &entry), ret);
    entry.enable = state;
    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_set(unit, index, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_router_mac_entry_index_state_disable_enable */
#endif

#ifdef CMD_L3_SET_ROUTER_MAC_ENTRY_INDEX_KEY_PORT_PORT_STATE_DISABLE_ENABLE
/*
 * l3 set router-mac entry <UINT:index> key port <UINT:port> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l3_set_router_mac_entry_index_key_port_port_state_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *port_ptr)
{
    uint32  unit;
    uint32  index = *index_ptr;
    rtk_l3_routerMacEntry_t entry;
    rtk_enable_t state = RTK_ENABLE_END;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(9, state);

    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_get(unit, index, &entry), ret);
    entry.port_type = 0;
    entry.port_trunk_id = *port_ptr;
    if (state == ENABLED)
    {
        entry.port_type_mask = -1;      /* care all bits */
        entry.port_trunk_id_mask = -1;  /* care all bits */
    } else {
        entry.port_type_mask = 0x0;     /* clear all bits */
        entry.port_trunk_id_mask = 0x0; /* clear all bits */
    }
    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_set(unit, index, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_router_mac_entry_index_key_port_port_state_disable_enable */
#endif

#ifdef CMD_L3_SET_ROUTER_MAC_ENTRY_INDEX_KEY_TRUNK_TRUNK_ID_STATE_DISABLE_ENABLE
/*
 * l3 set router-mac entry <UINT:index> key trunk <UINT:trunk_id> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l3_set_router_mac_entry_index_key_trunk_trunk_id_state_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *trunk_id_ptr)
{
    uint32  unit;
    uint32  index = *index_ptr;
    rtk_l3_routerMacEntry_t entry;
    rtk_enable_t state = RTK_ENABLE_END;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(9, state);

    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_get(unit, index, &entry), ret);
    entry.port_type = 1;
    entry.port_trunk_id = *trunk_id_ptr;
    if (state == ENABLED)
    {
        entry.port_type_mask = -1;      /* care all bits */
        entry.port_trunk_id_mask = -1;  /* care all bits */
    } else {
        entry.port_type_mask = 0x0;     /* clear all bits */
        entry.port_trunk_id_mask = 0x0; /* clear all bits */
    }
    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_set(unit, index, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_router_mac_entry_index_key_trunk_trunk_id_state_disable_enable */
#endif

#ifdef CMD_L3_SET_ROUTER_MAC_ENTRY_INDEX_KEY_VID_VID_VID_MASK_VID_MASK
/*
 * l3 set router-mac entry <UINT:index> key vid <UINT:vid> vid_mask <UINT:vid_mask>
 */
cparser_result_t
cparser_cmd_l3_set_router_mac_entry_index_key_vid_vid_vid_mask_vid_mask(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr,
    uint32_t *vid_mask_ptr)
{
    uint32  unit;
    uint32  index = *index_ptr;
    rtk_l3_routerMacEntry_t entry;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_get(unit, index, &entry), ret);
    entry.vid = *vid_ptr;
    entry.vid_mask = *vid_mask_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_set(unit, index, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_router_mac_entry_index_key_vid_vid_vid_mask_vid_mask */
#endif

#ifdef CMD_L3_SET_ROUTER_MAC_ENTRY_INDEX_KEY_INTF_INTF_ID_INTF_MASK_INTF_ID_MASK
/*
 * l3 set router-mac entry <UINT:index> key intf <UINT:intf_id> intf_mask <UINT:intf_id_mask>
 */
cparser_result_t
cparser_cmd_l3_set_router_mac_entry_index_key_intf_intf_id_intf_mask_intf_id_mask(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *intf_id_ptr,
    uint32_t *intf_id_mask_ptr)
{
    uint32  unit;
    uint32  index = *index_ptr;
    rtk_l3_routerMacEntry_t entry;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_get(unit, index, &entry), ret);
    entry.intf_id = *intf_id_ptr;
    entry.intf_id_mask = *intf_id_mask_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_set(unit, index, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_router_mac_entry_index_key_intf_intf_id_intf_mask_intf_id_mask */
#endif

#ifdef CMD_L3_SET_ROUTER_MAC_ENTRY_INDEX_KEY_MAC_ADDR_MAC_MAC_ADDR_MASK_MAC_MASK
/*
 * l3 set router-mac entry <UINT:index> key mac-addr <MACADDR:mac> mac-addr-mask <MACADDR:mac_mask>
 */
cparser_result_t
cparser_cmd_l3_set_router_mac_entry_index_key_mac_addr_mac_mac_addr_mask_mac_mask(
    cparser_context_t *context,
    uint32_t *index_ptr,
    cparser_macaddr_t *mac_ptr,
    cparser_macaddr_t *mac_mask_ptr)
{
    uint32  unit;
    uint32  index = *index_ptr;
    rtk_l3_routerMacEntry_t entry;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_get(unit, index, &entry), ret);
    osal_memcpy((void*)&entry.mac, (void*)mac_ptr, sizeof(rtk_mac_t));
    osal_memcpy((void*)&entry.mac_mask, (void*)mac_mask_ptr, sizeof(rtk_mac_t));
    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_set(unit, index, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_router_mac_entry_index_key_mac_addr_mac_mac_addr_mask_mac_mask */
#endif

#ifdef CMD_L3_SET_ROUTER_MAC_ENTRY_INDEX_ACTION_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set router-mac entry <UINT:index> action ( forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_router_mac_entry_index_action_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    uint32  index = *index_ptr;
    rtk_l3_routerMacEntry_t entry;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_get(unit, index, &entry), ret);
    DIAG_UTIL_PARSE_L3_ACT(6, entry.l3_act);
    DIAG_UTIL_ERR_CHK(rtk_l3_routerMacEntry_set(unit, index, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_router_mac_entry_index_action_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_CREATE_INTF_VID_VID_MAC_ADDR_MAC_ADDR
/*
 * l3 create intf vid <UINT:vid> mac-addr <MACADDR:mac_addr>
 */
cparser_result_t
cparser_cmd_l3_create_intf_vid_vid_mac_addr_mac_addr(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_addr_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.vid = *vid_ptr;
    osal_memcpy(&intf.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_create(unit, &intf), ret);
    DUMP_INTF(intf);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_create_intf_vid_vid_mac_addr_mac_addr */
#endif

#ifdef CMD_L3_CREATE_INTF_VID_VID_MAC_ADDR_MAC_ADDR_FLAG_FLAG
/*
 * l3 create intf vid <UINT:vid> mac-addr <MACADDR:mac_addr> flag <UINT:flag>
 */
cparser_result_t
cparser_cmd_l3_create_intf_vid_vid_mac_addr_mac_addr_flag_flag(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_addr_ptr,
    uint32_t *flag_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.vid = *vid_ptr;
    intf.flags = *flag_ptr;
    osal_memcpy(&intf.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_create(unit, &intf), ret);
    DUMP_INTF(intf);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_create_intf_vid_vid_mac_addr_mac_addr_flag_flag */
#endif

#ifdef CMD_L3_CREATE_INTF_INTF_ID_VID_VID_MAC_ADDR_MAC_ADDR
/*
 * l3 create intf <UINT:intf_id> vid <UINT:vid> mac-addr <MACADDR:mac_addr>
 */
cparser_result_t
cparser_cmd_l3_create_intf_intf_id_vid_vid_mac_addr_mac_addr(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_addr_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.flags |= RTK_L3_INTF_FLAG_WITH_ID;
    intf.intf_id = *intf_id_ptr;
    intf.vid = *vid_ptr;
    osal_memcpy(&intf.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_create(unit, &intf), ret);
    DUMP_INTF(intf);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_create_intf_intf_id_vid_vid_mac_addr_mac_addr */
#endif

#ifdef CMD_L3_CREATE_INTF_INTF_ID_VID_VID_MAC_ADDR_MAC_ADDR_FLAG_FLAG
/*
 * l3 create intf <UINT:intf_id> vid <UINT:vid> mac-addr <MACADDR:mac_addr> flag <UINT:flag>
 */
cparser_result_t
cparser_cmd_l3_create_intf_intf_id_vid_vid_mac_addr_mac_addr_flag_flag(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_addr_ptr,
    uint32_t *flag_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.flags = *flag_ptr;
    intf.flags |= RTK_L3_INTF_FLAG_WITH_ID;
    intf.intf_id = *intf_id_ptr;
    intf.vid = *vid_ptr;
    osal_memcpy(&intf.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_create(unit, &intf), ret);
    DUMP_INTF(intf);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_create_intf_intf_id_vid_vid_mac_addr_mac_addr_flag_flag */
#endif

#ifdef CMD_L3_DESTROY_INTF_INTF_ID_ALL
/*
 * l3 destroy intf ( <UINT:intf_id> | all )
 */
cparser_result_t
cparser_cmd_l3_destroy_intf_intf_id_all(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('a' == TOKEN_CHAR(3,0))
        DIAG_UTIL_ERR_CHK(rtk_l3_intf_destroyAll(unit), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_l3_intf_destroy(unit, *intf_id_ptr), ret);

    /*DIAG_UTIL_MPRINTF("destroy interface %s\n", TOKEN_STR(3));*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_destroy_intf_intf_id_all */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_VID_VID
/*
 * l3 set intf <UINT:intf_id> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_vid_vid(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vid_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);

    intf.vid = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_set(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_vid_vid */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_VRF_ID_VRF_ID
/*
 * l3 set intf <UINT:intf_id> vrf-id <UINT:vrf_id>
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_vrf_id_vrf_id(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vrf_id_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);

    intf.vrf_id = *vrf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_set(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_vrf_id_vrf_id */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_MTU_MTU
/*
 * l3 set intf <UINT:intf_id> mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_mtu_mtu(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *mtu_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);

    intf.mtu = *mtu_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_set(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_mtu_mtu */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IPV6_MTU_MTU
/*
 * l3 set intf <UINT:intf_id> ipv6-mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ipv6_mtu_mtu(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *mtu_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);
    intf.ipv6_mtu = *mtu_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_set(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ipv6_mtu_mtu */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_TTL_TTL
/*
 * l3 set intf <UINT:intf_id> ttl <UINT:ttl>
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ttl_ttl(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *ttl_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);

    intf.ttl = *ttl_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_set(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ttl_ttl */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IPMCAST_KEY_VID_INTF
/*
 * l3 set intf <UINT:intf_id> ipmcast-key ( vid | intf )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ipmcast_key_vid_intf(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    int32   value;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('v' == TOKEN_CHAR(5, 0))
        value = RTK_L3_IPMC_KEY_SEL_VID;
    else
        value = RTK_L3_IPMC_KEY_SEL_INTF;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, RTK_L3_ICT_MC_KEY_SEL, value), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ipmcast_key_vid_intf */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IPMCAST_KEY
/*
 * l3 get intf <UINT:intf_id> ipmcast-key
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ipmcast_key(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    int32   value;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, RTK_L3_ICT_MC_KEY_SEL, &value), ret);
    DIAG_UTIL_MPRINTF("IPMC key  : %s\n", (RTK_L3_IPMC_KEY_SEL_VID == value)? "VID" : "INTF");

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ipmcast_key */
#endif

#ifdef CMD_L3_SET_INTF_VID_VID_VRF_ID_VRF_ID
/*
 * l3 set intf vid <UINT:vid> vrf-id <UINT:vrf_id>
 */
cparser_result_t
cparser_cmd_l3_set_intf_vid_vid_vrf_id_vrf_id(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *vrf_id_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.vid = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_VID, &intf), ret);

    intf.vrf_id = *vrf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_set(unit, RTK_L3_INTFKEYTYPE_VID, &intf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_vid_vid_vrf_id_vrf_id */
#endif

#ifdef CMD_L3_SET_INTF_VID_VID_MTU_MTU
/*
 * l3 set intf vid <UINT:vid> mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_l3_set_intf_vid_vid_mtu_mtu(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *mtu_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.vid = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_VID, &intf), ret);
    intf.mtu = *mtu_ptr;

    DIAG_UTIL_MPRINTF("set interface vid %u mtu %u\n", intf.vid, intf.mtu);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_set(unit, RTK_L3_INTFKEYTYPE_VID, &intf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_vid_vid_mtu_mtu */
#endif

#ifdef CMD_L3_SET_INTF_VID_VID_TTL_TTL
/*
 * l3 set intf vid <UINT:vid> ttl <UINT:ttl>
 */
cparser_result_t
cparser_cmd_l3_set_intf_vid_vid_ttl_ttl(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *ttl_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.vid = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_VID, &intf), ret);
    intf.ttl = *ttl_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_intf_set(unit, RTK_L3_INTFKEYTYPE_VID, &intf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_vid_vid_ttl_ttl */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID
/*
 * l3 get intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf), ret);
    DUMP_INTF(intf);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id */
#endif

#ifdef CMD_L3_GET_INTF_VID_VID
/*
 * l3 get intf vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l3_get_intf_vid_vid(
    cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    intf.vid = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_VID, &intf), ret);
    DUMP_INTF(intf);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_vid_vid */
#endif

#ifdef CMD_L3_DUMP_INTF_FROM_BEGIN_INDEX_BEGIN_TO_END_INDEX_END
/*
 * l3 dump intf from ( <UINT:begin_index> | begin ) to ( <UINT:end_index> | end )
 */
cparser_result_t
cparser_cmd_l3_dump_intf_from_begin_index_begin_to_end_index_end(
    cparser_context_t *context,
    uint32_t *begin_index_ptr,
    uint32_t *end_index_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;
    uint32 intfIdx;
    int32 base,end;
    rtk_switch_devInfo_t devInfo;
    uint32 totCnt = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&devInfo,0,sizeof(rtk_switch_devInfo_t));
    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if ('b' == TOKEN_CHAR(4, 0))
    {
        base = 0;  /* from beginning */
    }
    else
    {
        base = ((*begin_index_ptr) > devInfo.capacityInfo.max_num_of_intf  - 1) ? \
               (devInfo.capacityInfo.max_num_of_intf  - 1): (*begin_index_ptr);
    }

    if ('e' == TOKEN_CHAR(6, 0))
    {
        end = devInfo.capacityInfo.max_num_of_intf - 1;
    }
    else
    {
        /* including the start number */
        end = ((*end_index_ptr) > devInfo.capacityInfo.max_num_of_intf  - 1) ? \
              (devInfo.capacityInfo.max_num_of_intf  - 1): (*end_index_ptr);
    }

    if (base > end)
        return CPARSER_ERR_INVALID_PARAMS;

    DUMP_INTF_LH();
    for (intfIdx = base; intfIdx <= end; intfIdx++)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
        intf.intf_id = intfIdx;
        if (RT_ERR_OK == (ret = rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &intf)))
        {
            DUMP_INTF_ENTRY(intf);
            totCnt++;
         }
    }

    DIAG_UTIL_MPRINTF("\n Total entry number : %d \n", totCnt);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_dump_intf_from_begin_index_begin_to_end_index_end */
#endif

#ifdef CMD_L3_GET_INTF_VID_VID_MAC_ADDR_MAC_ADDR
/*
 * l3 get intf vid <UINT:vid> mac-addr <MACADDR:mac_addr>
 */
cparser_result_t
cparser_cmd_l3_get_intf_vid_vid_mac_addr_mac_addr(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_addr_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_t intf;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_t_init(&intf), ret);
    osal_memcpy(&intf.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    intf.vid = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_MAC_AND_VID, &intf), ret);
    DUMP_INTF(intf);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_vid_vid_mac_addr_mac_addr */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_STATS
/*
 * l3 get intf <UINT:intf_id> stats
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_stats(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    rtk_l3_intf_stats_t stats;
    uint32 unit;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intfStats_get(unit, *intf_id_ptr, &stats), ret);

    DIAG_UTIL_MPRINTF("         inOctets : %llu \n", stats.rx.octets);
    DIAG_UTIL_MPRINTF("      inUcastPkts : %llu \n", stats.rx.unicast_pkts);
    DIAG_UTIL_MPRINTF("  inMulticastPkts : %llu \n", stats.rx.multicast_pkts);
    DIAG_UTIL_MPRINTF("  inBroadcastPkts : %llu \n", stats.rx.broadcast_pkts);
    DIAG_UTIL_MPRINTF("       inDiscards : %llu \n", stats.rx.discards);
    DIAG_UTIL_MPRINTF("        outOctets : %llu \n", stats.tx.octets);
    DIAG_UTIL_MPRINTF("     outUcastPkts : %llu \n", stats.tx.unicast_pkts);
    DIAG_UTIL_MPRINTF(" outMulticastPkts : %llu \n", stats.tx.multicast_pkts);
    DIAG_UTIL_MPRINTF(" outBroadcastPkts : %llu \n", stats.tx.broadcast_pkts);
    DIAG_UTIL_MPRINTF("      outDiscards : %llu \n", stats.tx.discards);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_stats */
#endif

#ifdef CMD_L3_RESET_INTF_INTF_ID_STATS
/*
 * l3 reset intf <UINT:intf_id> stats
 */
cparser_result_t
cparser_cmd_l3_reset_intf_intf_id_stats(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intfStats_reset(unit, *intf_id_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_reset_intf_intf_id_stats */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_STATS_IN_OCTETS_IN_UCAST_PKTS_IN_MULTICAST_PKTS_IN_BROADCAST_PKTS_IN_DISCARDS_OUT_OCTETS_OUT_UCAST_PKTS_OUT_MULTICAST_PKTS_OUT_BROADCAST_PKTS_OUT_DISCARDS
/*
 * l3 get intf <UINT:intf_id> stats ( in-octets | in-ucast-pkts | in-multicast-pkts | in-broadcast-pkts | in-discards | out-octets | out-ucast-pkts | out-multicast-pkts | out-broadcast-pkts | out-discards )
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_stats_in_octets_in_ucast_pkts_in_multicast_pkts_in_broadcast_pkts_in_discards_out_octets_out_ucast_pkts_out_multicast_pkts_out_broadcast_pkts_out_discards(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint64 val = 0;
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_intf_stats_t stats;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intfStats_get(unit, *intf_id_ptr, &stats), ret);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        if ('o' == TOKEN_CHAR(5, 3))
        {
            val = stats.rx.octets;
        }
        else if ('u' == TOKEN_CHAR(5, 3))
        {
            val = stats.rx.unicast_pkts;
        }
        else if ('m' == TOKEN_CHAR(5, 3))
        {
            val = stats.rx.multicast_pkts;
        }
        else if ('b' == TOKEN_CHAR(5, 3))
        {
            val = stats.rx.broadcast_pkts;
        }
        else if ('d' == TOKEN_CHAR(5, 3))
        {
            val = stats.rx.discards;
        }
    }
    else if ('o' == TOKEN_CHAR(5, 0))
    {
        if ('o' == TOKEN_CHAR(5, 4))
        {
            val = stats.tx.octets;
        }
        else if ('u' == TOKEN_CHAR(5, 4))
        {
            val = stats.tx.unicast_pkts;
        }
        else if ('m' == TOKEN_CHAR(5, 4))
        {
            val = stats.tx.multicast_pkts;
        }
        else if ('b' == TOKEN_CHAR(5, 4))
        {
            val = stats.tx.broadcast_pkts;
        }
        else if ('d' == TOKEN_CHAR(5, 4))
        {
            val = stats.tx.discards;
        }
    }

    DIAG_UTIL_MPRINTF("Interface counter %s: %llu \n", TOKEN_STR(5), val);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_stats_in_octets_in_ucast_pkts_in_multicast_pkts_in_broadcast_pkts_in_discards_out_octets_out_ucast_pkts_out_ucast_pkts_out_multicast_pkts_out_broadcast_pkts_out_discards */
#endif

#ifdef CMD_L3_ADD_VRRP_FLAGS_IPV4_IPV6_VID_VID_VRID_VRID
/*
 * l3 add vrrp flags ( ipv4 | ipv6 ) vid <UINT:vid> vrid <UINT:vrid>
 */
cparser_result_t
cparser_cmd_l3_add_vrrp_flags_ipv4_ipv6_vid_vid_vrid_vrid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *vrid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    if ('4' == TOKEN_CHAR(4, 3))
        DIAG_UTIL_ERR_CHK(rtk_l3_vrrp_add(unit, RTK_L3_VRRP_FLAG_IPV4, *vid_ptr, *vrid_ptr), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_l3_vrrp_add(unit, RTK_L3_VRRP_FLAG_IPV6, *vid_ptr, *vrid_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_add_vrrp_vid_vid_vrid_vrid */
#endif

#ifdef CMD_L3_DEL_VRRP_FLAGS_IPV4_IPV6_VID_VID_VRID_VRID
/*
 * l3 del vrrp flags ( ipv4 | ipv6 ) vid <UINT:vid> vrid <UINT:vrid>
 */
cparser_result_t
cparser_cmd_l3_del_vrrp_flags_ipv4_ipv6_vid_vid_vrid_vrid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *vrid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    if ('4' == TOKEN_CHAR(4, 3))
        DIAG_UTIL_ERR_CHK(rtk_l3_vrrp_del(unit, RTK_L3_VRRP_FLAG_IPV4, *vid_ptr, *vrid_ptr), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_l3_vrrp_del(unit, RTK_L3_VRRP_FLAG_IPV6, *vid_ptr, *vrid_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_vrrp_vid_vid_vrid_vrid */
#endif

#ifdef CMD_L3_DEL_VRRP_FLAGS_IPV4_IPV6_VID_VID_VRID_ALL
/*
 * l3 del vrrp flags ( ipv4 | ipv6 ) vid <UINT:vid> vrid all
 */
cparser_result_t
cparser_cmd_l3_del_vrrp_flags_ipv4_ipv6_vid_vid_vrid_all(
    cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    if ('4' == TOKEN_CHAR(4, 3))
        DIAG_UTIL_ERR_CHK(rtk_l3_vrrp_delAll(unit, RTK_L3_VRRP_FLAG_IPV4, *vid_ptr), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_l3_vrrp_delAll(unit, RTK_L3_VRRP_FLAG_IPV6, *vid_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_vrrp_vid_vid_vrid_all */
#endif

#ifdef CMD_L3_GET_VRRP_FLAGS_IPV4_IPV6_VID_VID_VRID
/*
 * l3 get vrrp flags ( ipv4 | ipv6 ) vid <UINT:vid> vrid
 */
cparser_result_t
cparser_cmd_l3_get_vrrp_flags_ipv4_ipv6_vid_vid_vrid(
    cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32  unit;
    uint32  i = 0;
    uint32  maxIdRet = 0;
    uint32  vrIds[L3_VRID_MAX];
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&vrIds, 0, (sizeof(uint32) * L3_VRID_MAX));

    if ('4' == TOKEN_CHAR(4, 3))
        DIAG_UTIL_ERR_CHK(rtk_l3_vrrp_get(unit, RTK_L3_VRRP_FLAG_IPV4, *vid_ptr, L3_VRID_MAX, vrIds, &maxIdRet), ret);
    else
        DIAG_UTIL_ERR_CHK(rtk_l3_vrrp_get(unit, RTK_L3_VRRP_FLAG_IPV6, *vid_ptr, L3_VRID_MAX, vrIds, &maxIdRet), ret);

    for (i=0; i<maxIdRet; i++)
    {
        DIAG_UTIL_MPRINTF(" VRID[%03d]: %d\n", i, vrIds[i]);
    }
    DIAG_UTIL_MPRINTF("\nTotal Number of VRIDs: %d\n", maxIdRet);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_vrrp_vid_vid_vrid */
#endif

#ifdef CMD_L3_CREATE_NEXT_HOP_INTF_INTF_ID_MAC_ADDR_MAC_ADDR
/*
 * l3 create next-hop intf <UINT:intf_id> mac-addr <MACADDR:mac_addr>
 */
cparser_result_t
cparser_cmd_l3_create_next_hop_intf_intf_id_mac_addr_mac_addr(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    cparser_macaddr_t *mac_addr_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_nextHop_t nh;
    rtk_l3_pathId_t nhId;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_t_init(&nh), ret);
    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    nh.intf_id = *intf_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_create(unit, RTK_L3_FLAG_NONE, &nh, &nhId), ret);
    DUMP_NH(nh, nhId);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_create_next_hop_intf_intf_id_mac_addr_mac_addr */
#endif

#ifdef CMD_L3_CREATE_NEXT_HOP_INTF_INTF_ID_MAC_ADDR_MAC_ADDR_ACTION_FORWARD_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * l3 create next-hop intf <UINT:intf_id> mac-addr <MACADDR:mac_addr> action ( forward | drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_l3_create_next_hop_intf_intf_id_mac_addr_mac_addr_action_forward_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    cparser_macaddr_t *mac_addr_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_nextHop_t nh;
    rtk_l3_pathId_t nhId;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_t_init(&nh), ret);
    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    nh.intf_id = *intf_id_ptr;
    DIAG_UTIL_PARSE_L3_ACT(8, nh.l3_act);

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_create(unit, RTK_L3_FLAG_NONE, &nh, &nhId), ret);
    DUMP_NH(nh, nhId);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_create_next_hop_intf_intf_id_mac_addr_mac_addr_action_forward_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_L3_CREATE_NEXT_HOP_NH_ID_NH_ID_INTF_INTF_ID_MAC_ADDR_MAC_ADDR
/*
 * l3 create next-hop nh-id <UINT:nh_id> intf <UINT:intf_id> mac-addr <MACADDR:mac_addr>
 */
cparser_result_t
cparser_cmd_l3_create_next_hop_nh_id_nh_id_intf_intf_id_mac_addr_mac_addr(
    cparser_context_t *context,
    uint32_t *nh_id_ptr,
    uint32_t *intf_id_ptr,
    cparser_macaddr_t *mac_addr_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_nextHop_t nh;
    rtk_l3_pathId_t nhId = *nh_id_ptr;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_t_init(&nh), ret);
    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    nh.intf_id = *intf_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_create(unit, RTK_L3_FLAG_WITH_ID, &nh, &nhId), ret);
    DUMP_NH(nh, nhId);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_create_next_hop_nh_id_nh_id_intf_intf_id_mac_addr_mac_addr */
#endif

#ifdef CMD_L3_CREATE_NEXT_HOP_NH_ID_NH_ID_INTF_INTF_ID_MAC_ADDR_MAC_ADDR_ACTION_FORWARD_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * l3 create next-hop nh-id <UINT:nh_id> intf <UINT:intf_id> mac-addr <MACADDR:mac_addr> action ( forward | drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_l3_create_next_hop_nh_id_nh_id_intf_intf_id_mac_addr_mac_addr_action_forward_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context,
    uint32_t *nh_id_ptr,
    uint32_t *intf_id_ptr,
    cparser_macaddr_t *mac_addr_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_nextHop_t nh;
    rtk_l3_pathId_t nhId = *nh_id_ptr;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_t_init(&nh), ret);
    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    nh.intf_id = *intf_id_ptr;
    DIAG_UTIL_PARSE_L3_ACT(10, nh.l3_act);

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_create(unit, RTK_L3_FLAG_WITH_ID, &nh, &nhId), ret);
    DUMP_NH(nh, nhId);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_create_next_hop_nh_id_nh_id_intf_intf_id_mac_addr_mac_addr_action_forward_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_L3_DESTROY_NEXT_HOP_NH_ID_NH_ID
/*
 * l3 destroy next-hop nh-id <UINT:nh_id>
 */
cparser_result_t
cparser_cmd_l3_destroy_next_hop_nh_id_nh_id(
    cparser_context_t *context,
    uint32_t *nh_id_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_destroy(unit, *nh_id_ptr), ret);
    /*DIAG_UTIL_MPRINTF("destroy next hop %u\n", *nh_id_ptr);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_destroy_next_hop_nh_id_nh_id */
#endif

#ifdef CMD_L3_SET_NEXT_HOP_NH_ID_NH_ID_INTF_INTF_ID
/*
 * l3 set next-hop nh-id <UINT:nh_id> intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_l3_set_next_hop_nh_id_nh_id_intf_intf_id(
    cparser_context_t *context,
    uint32_t *nh_id_ptr,
    uint32_t *intf_id_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_nextHop_t nh;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&nh, 0, sizeof(nh));
    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_get(unit, *nh_id_ptr, &nh), ret);
    nh.intf_id = *intf_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_create(unit, RTK_L3_FLAG_REPLACE, &nh, nh_id_ptr), ret);
    /*DUMP_NH(nh, *nh_id_ptr);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_next_hop_nh_id_nh_id_intf_intf_id */
#endif

#ifdef CMD_L3_SET_NEXT_HOP_NH_ID_NH_ID_MAC_ADDR_MAC_ADDR
/*
 * l3 set next-hop nh-id <UINT:nh_id> mac-addr <MACADDR:mac_addr>
 */
cparser_result_t
cparser_cmd_l3_set_next_hop_nh_id_nh_id_mac_addr_mac_addr(
    cparser_context_t *context,
    uint32_t *nh_id_ptr,
    cparser_macaddr_t *mac_addr_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_nextHop_t nh;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&nh, 0, sizeof(nh));
    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_get(unit, *nh_id_ptr, &nh), ret);
    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_create(unit, RTK_L3_FLAG_REPLACE, &nh, nh_id_ptr), ret);
    /*DUMP_NH(nh, *nh_id_ptr);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_next_hop_nh_id_nh_id_mac_addr_mac_addr */
#endif

#ifdef CMD_L3_SET_NEXT_HOP_NH_ID_NH_ID_ACTION_FORWARD_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * l3 set next-hop nh-id <UINT:nh_id> action ( forward | drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_next_hop_nh_id_nh_id_action_forward_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context,
    uint32_t *nh_id_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_nextHop_t nh;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&nh, 0, sizeof(nh));
    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_get(unit, *nh_id_ptr, &nh), ret);
    DIAG_UTIL_PARSE_L3_ACT(6, nh.l3_act);

    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_create(unit, RTK_L3_FLAG_REPLACE, &nh, nh_id_ptr), ret);
    /*DUMP_NH(nh, *nh_id_ptr);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_next_hop_nh_id_nh_id_action_forward_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_L3_GET_NEXT_HOP_NH_ID_NH_ID
/*
 * l3 get next-hop nh-id <UINT:nh_id>
 */
cparser_result_t
cparser_cmd_l3_get_next_hop_nh_id_nh_id(
    cparser_context_t *context,
    uint32_t *nh_id_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_nextHop_t nh;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&nh, 0, sizeof(nh));
    DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_get(unit, *nh_id_ptr, &nh), ret);
    DUMP_NH(nh, *nh_id_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_next_hop_nh_id_nh_id */
#endif

#ifdef CMD_L3_GET_NEXT_HOP_INTF_INTF_ID_MAC_ADDR_MAC_ADDR
/*
 * l3 get next-hop intf <UINT:intf_id> mac-addr <MACADDR:mac_addr>
 */
cparser_result_t
cparser_cmd_l3_get_next_hop_intf_intf_id_mac_addr_mac_addr(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    cparser_macaddr_t *mac_addr_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_nextHop_t nh;
    rtk_l3_pathId_t nhId = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&nh, 0, sizeof(nh));
    osal_memcpy(&nh.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));
    nh.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l3_nextHopPath_find(unit, &nh, &nhId), ret);
    DUMP_NH(nh, nhId);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_next_hop_intf_intf_id_mac_addr_mac_addr */
#endif

#ifdef CMD_L3_DUMP_NEXT_HOP_FROM_BEGIN_INDEX_BEGIN_TO_END_INDEX_END
/*
 * l3 dump next-hop from ( <UINT:begin_index> | begin ) to ( <UINT:end_index> | end )
 */
cparser_result_t
cparser_cmd_l3_dump_next_hop_from_begin_index_begin_to_end_index_end(
    cparser_context_t *context,
    uint32_t *begin_index_ptr,
    uint32_t *end_index_ptr)
{
    uint32 unit;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_nextHop_t nh;
    uint32 nhId;
    int32 base,end;
    rtk_switch_devInfo_t devInfo;
    uint32 totCnt = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&devInfo,0,sizeof(rtk_switch_devInfo_t));
    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if ('b' == TOKEN_CHAR(4, 0))
    {
        base = 0;  /* from beginning */
    }
    else
    {
        /* including the start number */
        base = ((*begin_index_ptr) > 0)? \
               ((*begin_index_ptr)) : (0);
    }

    if ('e' == TOKEN_CHAR(6, 0))
    {
        end = devInfo.capacityInfo.max_num_of_l3_nexthop;
    }
    else
    {
        /* including the start number */
        end = ((*end_index_ptr) > 0)? \
               ((*end_index_ptr)) : (devInfo.capacityInfo.max_num_of_l3_nexthop);

        end = (end > devInfo.capacityInfo.max_num_of_l3_nexthop) ? (devInfo.capacityInfo.max_num_of_l3_nexthop): end;
    }

    if (base > end)
        return CPARSER_ERR_INVALID_PARAMS;

    DUMP_NH_LH();
    for (nhId = base; nhId <= end; nhId++)
    {
        osal_memset(&nh, 0, sizeof(nh));
        if (RT_ERR_OK == (ret = rtk_l3_nextHop_get(unit, nhId, &nh)))
        {
            DUMP_NH_ENTRY(nh, nhId);
            totCnt++;
         }
    }

    DIAG_UTIL_MPRINTF("\n Total entry number : %d \n", totCnt);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_dump_next_hop_from_begin_index_begin_to_end_index_end */
#endif

#ifdef CMD_L3_CREATE_ECMP_NH_ID_1_NH_ID_2_NH_ID_3_NH_ID_4_NH_ID_5_NH_ID_6_NH_ID_7_NH_ID_8
/*
 * l3 create ecmp { <UINT:nh_id_1> } { <UINT:nh_id_2> } { <UINT:nh_id_3> } { <UINT:nh_id_4> } { <UINT:nh_id_5> } { <UINT:nh_id_6> } { <UINT:nh_id_7> } { <UINT:nh_id_8> }
 */
cparser_result_t
cparser_cmd_l3_create_ecmp_nh_id_1_nh_id_2_nh_id_3_nh_id_4_nh_id_5_nh_id_6_nh_id_7_nh_id_8(
    cparser_context_t *context,
    uint32_t *nh_id_1_ptr,
    uint32_t *nh_id_2_ptr,
    uint32_t *nh_id_3_ptr,
    uint32_t *nh_id_4_ptr,
    uint32_t *nh_id_5_ptr,
    uint32_t *nh_id_6_ptr,
    uint32_t *nh_id_7_ptr,
    uint32_t *nh_id_8_ptr)
{
    uint32 unit;
    uint32 i = 0;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_pathId_t nhIds[L3_ECMP_MAX];
    rtk_l3_pathId_t ecmpId = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&nhIds, 0, sizeof(nhIds));

    if (NULL != nh_id_1_ptr) nhIds[i++] = *nh_id_1_ptr;
    if (NULL != nh_id_2_ptr) nhIds[i++] = *nh_id_2_ptr;
    if (NULL != nh_id_3_ptr) nhIds[i++] = *nh_id_3_ptr;
    if (NULL != nh_id_4_ptr) nhIds[i++] = *nh_id_4_ptr;
    if (NULL != nh_id_5_ptr) nhIds[i++] = *nh_id_5_ptr;
    if (NULL != nh_id_6_ptr) nhIds[i++] = *nh_id_6_ptr;
    if (NULL != nh_id_7_ptr) nhIds[i++] = *nh_id_7_ptr;
    if (NULL != nh_id_8_ptr) nhIds[i++] = *nh_id_8_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_ecmp_create(unit, RTK_L3_FLAG_NONE, i, nhIds, &ecmpId), ret);
    DIAG_UTIL_MPRINTF("ECMP ID: %u (0x%X)\n", ecmpId, ecmpId);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_create_ecmp_nh_id_1_nh_id_2_nh_id_3_nh_id_4_nh_id_5_nh_id_6_nh_id_7_nh_id_8 */
#endif

#ifdef CMD_L3_CREATE_ECMP_ECMP_ID_ECMP_ID_NH_ID_1_NH_ID_2_NH_ID_3_NH_ID_4_NH_ID_5_NH_ID_6_NH_ID_7_NH_ID_8
/*
 * l3 create ecmp ecmp-id <UINT:ecmp_id> { <UINT:nh_id_1> } { <UINT:nh_id_2> } { <UINT:nh_id_3> } { <UINT:nh_id_4> } { <UINT:nh_id_5> } { <UINT:nh_id_6> } { <UINT:nh_id_7> } { <UINT:nh_id_8> }
 */
cparser_result_t
cparser_cmd_l3_create_ecmp_ecmp_id_ecmp_id_nh_id_1_nh_id_2_nh_id_3_nh_id_4_nh_id_5_nh_id_6_nh_id_7_nh_id_8(
    cparser_context_t *context,
    uint32_t *ecmp_id_ptr,
    uint32_t *nh_id_1_ptr,
    uint32_t *nh_id_2_ptr,
    uint32_t *nh_id_3_ptr,
    uint32_t *nh_id_4_ptr,
    uint32_t *nh_id_5_ptr,
    uint32_t *nh_id_6_ptr,
    uint32_t *nh_id_7_ptr,
    uint32_t *nh_id_8_ptr)
{
    uint32 unit;
    uint32 i = 0;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_pathId_t nhIds[L3_ECMP_MAX];
    rtk_l3_pathId_t ecmpId = *ecmp_id_ptr;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&nhIds, 0, sizeof(nhIds));

    if (NULL != nh_id_1_ptr) nhIds[i++] = *nh_id_1_ptr;
    if (NULL != nh_id_2_ptr) nhIds[i++] = *nh_id_2_ptr;
    if (NULL != nh_id_3_ptr) nhIds[i++] = *nh_id_3_ptr;
    if (NULL != nh_id_4_ptr) nhIds[i++] = *nh_id_4_ptr;
    if (NULL != nh_id_5_ptr) nhIds[i++] = *nh_id_5_ptr;
    if (NULL != nh_id_6_ptr) nhIds[i++] = *nh_id_6_ptr;
    if (NULL != nh_id_7_ptr) nhIds[i++] = *nh_id_7_ptr;
    if (NULL != nh_id_8_ptr) nhIds[i++] = *nh_id_8_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_ecmp_create(unit, RTK_L3_FLAG_WITH_ID, i, nhIds, &ecmpId), ret);
    DIAG_UTIL_MPRINTF("ECMP ID: %u (0x%X)\n", ecmpId, ecmpId);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_create_ecmp_ecmp_id_ecmp_id_nh_id_1_nh_id_2_nh_id_3_nh_id_4_nh_id_5_nh_id_6_nh_id_7_nh_id_8 */
#endif

#ifdef CMD_L3_DESTROY_ECMP_ECMP_ID_ECMP_ID
/*
 * l3 destroy ecmp ecmp-id <UINT:ecmp_id>
 */
cparser_result_t
cparser_cmd_l3_destroy_ecmp_ecmp_id_ecmp_id(
    cparser_context_t *context,
    uint32_t *ecmp_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_ecmp_destroy(unit, *ecmp_id_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_destroy_ecmp_ecmp_id_ecmp_id */
#endif

#ifdef CMD_L3_SET_ECMP_ECMP_ID_ECMP_ID_NH_ID_1_NH_ID_2_NH_ID_3_NH_ID_4_NH_ID_5_NH_ID_6_NH_ID_7_NH_ID_8
/*
 * l3 set ecmp ecmp-id <UINT:ecmp_id> { <UINT:nh_id_1> } { <UINT:nh_id_2> } { <UINT:nh_id_3> } { <UINT:nh_id_4> } { <UINT:nh_id_5> } { <UINT:nh_id_6> } { <UINT:nh_id_7> } { <UINT:nh_id_8> }
 */
cparser_result_t
cparser_cmd_l3_set_ecmp_ecmp_id_ecmp_id_nh_id_1_nh_id_2_nh_id_3_nh_id_4_nh_id_5_nh_id_6_nh_id_7_nh_id_8(
    cparser_context_t *context,
    uint32_t *ecmp_id_ptr,
    uint32_t *nh_id_1_ptr,
    uint32_t *nh_id_2_ptr,
    uint32_t *nh_id_3_ptr,
    uint32_t *nh_id_4_ptr,
    uint32_t *nh_id_5_ptr,
    uint32_t *nh_id_6_ptr,
    uint32_t *nh_id_7_ptr,
    uint32_t *nh_id_8_ptr)
{
    uint32 unit;
    uint32 i = 0;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_pathId_t nhIds[L3_ECMP_MAX];

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&nhIds, 0, sizeof(nhIds));

    if (NULL != nh_id_1_ptr) nhIds[i++] = *nh_id_1_ptr;
    if (NULL != nh_id_2_ptr) nhIds[i++] = *nh_id_2_ptr;
    if (NULL != nh_id_3_ptr) nhIds[i++] = *nh_id_3_ptr;
    if (NULL != nh_id_4_ptr) nhIds[i++] = *nh_id_4_ptr;
    if (NULL != nh_id_5_ptr) nhIds[i++] = *nh_id_5_ptr;
    if (NULL != nh_id_6_ptr) nhIds[i++] = *nh_id_6_ptr;
    if (NULL != nh_id_7_ptr) nhIds[i++] = *nh_id_7_ptr;
    if (NULL != nh_id_8_ptr) nhIds[i++] = *nh_id_8_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_ecmp_create(unit, RTK_L3_FLAG_REPLACE, i, nhIds, ecmp_id_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_ecmp_ecmp_id_ecmp_id_nh_id_1_nh_id_2_nh_id_3_nh_id_4_nh_id_5_nh_id_6_nh_id_7_nh_id_8 */
#endif

#ifdef CMD_L3_GET_ECMP_NH_ID_1_NH_ID_2_NH_ID_3_NH_ID_4_NH_ID_5_NH_ID_6_NH_ID_7_NH_ID_8
/*
 * l3 get ecmp { <UINT:nh_id_1> } { <UINT:nh_id_2> } { <UINT:nh_id_3> } { <UINT:nh_id_4> } { <UINT:nh_id_5> } { <UINT:nh_id_6> } { <UINT:nh_id_7> } { <UINT:nh_id_8> }
 */
cparser_result_t
cparser_cmd_l3_get_ecmp_nh_id_1_nh_id_2_nh_id_3_nh_id_4_nh_id_5_nh_id_6_nh_id_7_nh_id_8(
    cparser_context_t *context,
    uint32_t *nh_id_1_ptr,
    uint32_t *nh_id_2_ptr,
    uint32_t *nh_id_3_ptr,
    uint32_t *nh_id_4_ptr,
    uint32_t *nh_id_5_ptr,
    uint32_t *nh_id_6_ptr,
    uint32_t *nh_id_7_ptr,
    uint32_t *nh_id_8_ptr)
{
    uint32 unit;
    uint32 i = 0;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_pathId_t nhIds[L3_ECMP_MAX];
    rtk_l3_pathId_t ecmpId = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&nhIds, 0, sizeof(nhIds));

    if (NULL != nh_id_1_ptr) nhIds[i++] = *nh_id_1_ptr;
    if (NULL != nh_id_2_ptr) nhIds[i++] = *nh_id_2_ptr;
    if (NULL != nh_id_3_ptr) nhIds[i++] = *nh_id_3_ptr;
    if (NULL != nh_id_4_ptr) nhIds[i++] = *nh_id_4_ptr;
    if (NULL != nh_id_5_ptr) nhIds[i++] = *nh_id_5_ptr;
    if (NULL != nh_id_6_ptr) nhIds[i++] = *nh_id_6_ptr;
    if (NULL != nh_id_7_ptr) nhIds[i++] = *nh_id_7_ptr;
    if (NULL != nh_id_8_ptr) nhIds[i++] = *nh_id_8_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_ecmp_find(unit, i, nhIds, &ecmpId), ret);
    DIAG_UTIL_MPRINTF("ECMP ID: %u (0x%X)\n", ecmpId, ecmpId);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_ecmp_nh_id_1_nh_id_2_nh_id_3_nh_id_4_nh_id_5_nh_id_6_nh_id_7_nh_id_8 */
#endif

#ifdef CMD_L3_GET_ECMP_ECMP_ID_ECMP_ID_COUNT
/*
 * l3 get ecmp ecmp-id <UINT:ecmp_id> { <UINT:count> }
 */
cparser_result_t
cparser_cmd_l3_get_ecmp_ecmp_id_ecmp_id_count(
    cparser_context_t *context,
    uint32_t *ecmp_id_ptr,
    uint32_t *count_ptr)
{
    uint32 unit;
    uint32 i = 0;
    uint32 maxIdRet = 0;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_pathId_t nhIds[L3_ECMP_MAX];
    rtk_l3_nextHop_t nh;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&nhIds, 0, sizeof(nhIds));

    if (5 == TOKEN_NUM)
        i = L3_ECMP_MAX;
    else if (*count_ptr > L3_ECMP_MAX)
        i = L3_ECMP_MAX;
    else
        i = *count_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_ecmp_get(unit, *ecmp_id_ptr, i, nhIds, &maxIdRet), ret);

    DUMP_ECMP_LH();
    for (i=0; i<maxIdRet; i++)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_nextHop_get(unit, nhIds[i], &nh), ret);
        DUMP_ECMP_ENTRY((i+1), nhIds[i], nh.intf_id, nh.mac_addr, nh.l3_act);
    }
    DIAG_UTIL_MPRINTF("\nTotal Number of Nexthops: %d\n", maxIdRet);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_ecmp_ecmp_id_ecmp_id_count */
#endif

#ifdef CMD_L3_ADD_ECMP_ECMP_ID_ECMP_ID_NH_ID_NH_ID
/*
 * l3 add ecmp ecmp-id <UINT:ecmp_id> nh-id <UINT:nh_id>
 */
cparser_result_t
cparser_cmd_l3_add_ecmp_ecmp_id_ecmp_id_nh_id_nh_id(
    cparser_context_t *context,
    uint32_t *ecmp_id_ptr,
    uint32_t *nh_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_MPRINTF("add ecmpid %u nh-id %u\n", *ecmp_id_ptr, *nh_id_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l3_ecmp_add(unit, *ecmp_id_ptr, *nh_id_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_add_ecmp_ecmp_id_ecmp_id_nh_id_nh_id */
#endif

#ifdef CMD_L3_DEL_ECMP_ECMP_ID_ECMP_ID_NH_ID_NH_ID
/*
 * l3 del ecmp ecmp-id <UINT:ecmp_id> nh-id <UINT:nh_id>
 */
cparser_result_t
cparser_cmd_l3_del_ecmp_ecmp_id_ecmp_id_nh_id_nh_id(
    cparser_context_t *context,
    uint32_t *ecmp_id_ptr,
    uint32_t *nh_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_MPRINTF("del ecmpid %u nh-id %u\n", *ecmp_id_ptr, *nh_id_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l3_ecmp_del(unit, *ecmp_id_ptr, *nh_id_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_ecmp_ecmp_id_ecmp_id_nh_id_nh_id */
#endif

#ifdef CMD_L3_SET_ECMP_HASH_KEY_PORT_ID_TRUNK_ID_SIP_DIP_IP_DSCP_IP_PROTO_IP6_FLOW_LABEL_SPORT_DPORT_STATE_ENABLE_DISABLE
/*
 * l3 set ecmp hash key ( port-id | trunk-id | sip | dip | ip-dscp | ip-proto | ip6-flow-label | sport | dport ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l3_set_ecmp_hash_key_port_id_trunk_id_sip_dip_ip_dscp_ip_proto_ip6_flow_label_sport_dport_state_enable_disable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    int32   hash_key;
    rtk_l3_ecmpHashKey_t hashKey;

    if ('p' == TOKEN_CHAR(5, 0))
    {
        hashKey = RTK_L3_ECMP_HASHKEY_PORT_ID;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        hashKey = RTK_L3_ECMP_HASHKEY_TRUNK_ID;
    }
    else if ('s' == TOKEN_CHAR(5, 0))
    {
        if ('i' == TOKEN_CHAR(5, 1))
        {
            hashKey = RTK_L3_ECMP_HASHKEY_SIP;
        }
        else if ('p' == TOKEN_CHAR(5, 1))
        {
            hashKey = RTK_L3_ECMP_HASHKEY_SPORT;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        if ('i' == TOKEN_CHAR(5, 1))
        {
            hashKey = RTK_L3_ECMP_HASHKEY_DIP;
        }
        else if ('p' == TOKEN_CHAR(5, 1))
        {
            hashKey = RTK_L3_ECMP_HASHKEY_DPORT;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
    }
    else if ('i' == TOKEN_CHAR(5, 0))
    {
        if ('d' == TOKEN_CHAR(5, 3))
        {
            hashKey = RTK_L3_ECMP_HASHKEY_IP_DSCP;
        }
        else if ('p' == TOKEN_CHAR(5, 3))
        {
            hashKey = RTK_L3_ECMP_HASHKEY_IP_PROTO;
        }
        else if ('f' == TOKEN_CHAR(5, 4))
        {
            hashKey = RTK_L3_ECMP_HASHKEY_IP6_LABEL;
        }
        else
        {
            return CPARSER_NOT_OK;
        }

    }
    else
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_ECMP_HASH_KEY, &hash_key), ret);

    if ('e' == TOKEN_CHAR(7, 0))
    {
        hash_key |= hashKey;
    }
    else
    {
        hash_key &= ~(hashKey);
    }

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, RTK_L3_GCT_ECMP_HASH_KEY, hash_key), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_ecmp_hash_key_port_id_trunk_id_sip_dip_ip_dscp_ip_proto_ip6_flow_label_sport_dport_state_enable_disable */
#endif

#ifdef CMD_L3_SET_ECMP_HASH_KEY_UNIVERSAL_ID_SIP_SHIFT_DIP_SHIFT_SPORT_SHIFT_DPORT_SHIFT_VALUE
/*
 * l3 set ecmp hash key ( universal-id | sip-shift | dip-shift | sport-shift | dport-shift ) <UINT:value>
 */
cparser_result_t
cparser_cmd_l3_set_ecmp_hash_key_universal_id_sip_shift_dip_shift_sport_shift_dport_shift_value(
    cparser_context_t *context,
    uint32_t *value_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_globalCtrlType_t type;
    int32   value;

    if ('u' == TOKEN_CHAR(5, 0))
    {
        type = RTK_L3_GCT_ECMP_HASH_UNIVERSAL_ID;
    }
    else if ('s' == TOKEN_CHAR(5, 0))
    {
        if ('i' == TOKEN_CHAR(5, 1))
        {
            type = RTK_L3_GCT_ECMP_HASH_SIP_BIT_OFFSET;
        }
        else if ('p' == TOKEN_CHAR(5, 1))
        {
            type = RTK_L3_GCT_ECMP_HASH_SPORT_BIT_OFFSET;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        if ('i' == TOKEN_CHAR(5, 1))
        {
            type = RTK_L3_GCT_ECMP_HASH_DIP_BIT_OFFSET;
        }
        else if ('p' == TOKEN_CHAR(5, 1))
        {
            type = RTK_L3_GCT_ECMP_HASH_DPORT_BIT_OFFSET;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    value = *value_ptr;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, value), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_ecmp_hash_key_universal_id_sip_shift_dip_shift_sport_shift_dport_shift_value */
#endif

#ifdef CMD_L3_GET_ECMP_HASH_KEY
/*
 * l3 get ecmp hash key
 */
cparser_result_t
cparser_cmd_l3_get_ecmp_hash_key(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    int32   hash_key;
    int32   universal_id;
    int32   sip_shift;
    int32   dip_shift;
    int32   sport_shift;
    int32   dport_shift;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_ECMP_HASH_KEY, &hash_key), ret);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_ECMP_HASH_UNIVERSAL_ID, &universal_id), ret);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_ECMP_HASH_SIP_BIT_OFFSET, &sip_shift), ret);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_ECMP_HASH_DIP_BIT_OFFSET, &dip_shift), ret);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_ECMP_HASH_SPORT_BIT_OFFSET, &sport_shift), ret);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_ECMP_HASH_DPORT_BIT_OFFSET, &dport_shift), ret);

    DIAG_UTIL_MPRINTF("\tECMP Hash configuration\n");
    DIAG_UTIL_MPRINTF("----------------------------------------\n");
    DIAG_UTIL_MPRINTF("          Port ID : %s\n", text_state[(hash_key & RTK_L3_ECMP_HASHKEY_PORT_ID)? ENABLED : DISABLED]);
    DIAG_UTIL_MPRINTF("         Trunk ID : %s\n", text_state[(hash_key & RTK_L3_ECMP_HASHKEY_TRUNK_ID)? ENABLED : DISABLED]);
    DIAG_UTIL_MPRINTF("        Source IP : %s\n", text_state[(hash_key & RTK_L3_ECMP_HASHKEY_SIP)? ENABLED : DISABLED]);
    DIAG_UTIL_MPRINTF("   Destination IP : %s\n", text_state[(hash_key & RTK_L3_ECMP_HASHKEY_DIP)? ENABLED : DISABLED]);
    DIAG_UTIL_MPRINTF("          IP DSCP : %s\n", text_state[(hash_key & RTK_L3_ECMP_HASHKEY_IP_DSCP)? ENABLED : DISABLED]);
    DIAG_UTIL_MPRINTF("      IP protocol : %s\n", text_state[(hash_key & RTK_L3_ECMP_HASHKEY_IP_PROTO)? ENABLED : DISABLED]);
    DIAG_UTIL_MPRINTF("  IPv6 flow label : %s\n", text_state[(hash_key & RTK_L3_ECMP_HASHKEY_IP6_LABEL)? ENABLED : DISABLED]);
    DIAG_UTIL_MPRINTF("      Source port : %s\n", text_state[(hash_key & RTK_L3_ECMP_HASHKEY_SPORT)? ENABLED : DISABLED]);
    DIAG_UTIL_MPRINTF(" Destination port : %s\n", text_state[(hash_key & RTK_L3_ECMP_HASHKEY_DPORT)? ENABLED : DISABLED]);

    DIAG_UTIL_MPRINTF("     Universal ID : %d\n", universal_id);
    DIAG_UTIL_MPRINTF("    SIP bit-shift : %d\n", sip_shift);
    DIAG_UTIL_MPRINTF("    DIP bit-shift : %d\n", dip_shift);
    DIAG_UTIL_MPRINTF(" S-Port bit-shift : %d\n", sport_shift);
    DIAG_UTIL_MPRINTF(" D-Port bit-shift : %d\n", dport_shift);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_ecmp_hash_key */
#endif

#ifdef CMD_L3_ADD_HOST_VRF_ID_VRF_ID_IP_IP_ADDR_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 add host vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_add_host_vrf_id_vrf_id_ip_ip_addr_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_host_t host;

    osal_memset(&host, 0, sizeof(host));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_HOST_ACT(10, host.fwd_act);

    host.vrf_id = *vrf_id_ptr;
    host.ip_addr.ipv4 = *ip_addr_ptr;
    host.path_id = *path_id_ptr;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        host.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        host.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        host.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        host.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        host.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DUMP_HOST(host);
    DIAG_UTIL_ERR_CHK(rtk_l3_host_add(unit, &host), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_ADD_HOST_VRF_ID_VRF_ID_IP_IP_ADDR_NEXTHOP_INTF_ID_MAC_ADDR_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 add host vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> nexthop <UINT:intf_id> <MACADDR:mac_addr> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_add_host_vrf_id_vrf_id_ip_ip_addr_nexthop_intf_id_mac_addr_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *intf_id_ptr,
    cparser_macaddr_t *mac_addr_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_host_t host;

    osal_memset(&host, 0, sizeof(host));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_HOST_ACT(11, host.fwd_act);

    host.vrf_id = *vrf_id_ptr;
    host.ip_addr.ipv4 = *ip_addr_ptr;
    host.flags |= RTK_L3_FLAG_WITH_NEXTHOP;
    host.nexthop.intf_id = *intf_id_ptr;
    osal_memcpy(&host.nexthop.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        host.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        host.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        host.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        host.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        host.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DUMP_HOST(host);
    DIAG_UTIL_ERR_CHK(rtk_l3_host_add(unit, &host), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_add_host_vrf_id_vrf_id_ip_ip_addr_nexthop_intf_id_mac_addr_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority */
#endif

#ifdef CMD_L3_SET_HOST_VRF_ID_VRF_ID_IP_IP_ADDR_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 set host vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_set_host_vrf_id_vrf_id_ip_ip_addr_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_host_t host;

    osal_memset(&host, 0, sizeof(host));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_HOST_ACT(10, host.fwd_act);

    host.vrf_id = *vrf_id_ptr;
    host.ip_addr.ipv4 = *ip_addr_ptr;
    host.path_id = *path_id_ptr;
    host.flags |= RTK_L3_FLAG_REPLACE;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        host.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        host.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        host.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        host.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        host.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_add(unit, &host), ret);
    /*DUMP_HOST(host);*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_DEL_HOST_VRF_ID_VRF_ID_IP_IP_ADDR
/*
 * l3 del host vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr>
 */
cparser_result_t
cparser_cmd_l3_del_host_vrf_id_vrf_id_ip_ip_addr(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_host_t host;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&host, 0, sizeof(host));
    host.ip_addr.ipv4 = *ip_addr_ptr;
    host.vrf_id = *vrf_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_del(unit, &host), ret);
    /*DUMP_HOST(host);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_host_vrf_id_vrf_id_ip_ip_addr */
#endif

#ifdef CMD_L3_DEL_HOST_VRF_ID_VRF_ID_IP_IP_ADDR_PREFIX_LENGTH_PREFIX_LENGTH
/*
 * l3 del host vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> prefix-length <UINT:prefix_length>
 */
cparser_result_t
cparser_cmd_l3_del_host_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *prefix_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&route, 0, sizeof(route));
    route.ip_addr.ipv4 = *ip_addr_ptr;
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_del_byNetwork(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_host_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length */
#endif

#ifdef CMD_L3_ADD_HOST_VRF_ID_VRF_ID_IP6_IP6_ADDR_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 add host vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_add_host_vrf_id_vrf_id_ip6_ip6_addr_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_host_t host;

    osal_memset(&host, 0, sizeof(host));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_HOST_ACT(10, host.fwd_act);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(host.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    host.vrf_id = *vrf_id_ptr;
    host.path_id = *path_id_ptr;
    host.flags |= RTK_L3_FLAG_IPV6;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        host.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        host.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        host.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        host.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        host.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_add(unit, &host), ret);
    /*DUMP_HOST(host);*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_ADD_HOST_VRF_ID_VRF_ID_IP6_IP6_ADDR_NEXTHOP_INTF_ID_MAC_ADDR_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 add host vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> nexthop <UINT:intf_id> <MACADDR:mac_addr> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_add_host_vrf_id_vrf_id_ip6_ip6_addr_nexthop_intf_id_mac_addr_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *intf_id_ptr,
    cparser_macaddr_t *mac_addr_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_host_t host;

    osal_memset(&host, 0, sizeof(host));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_HOST_ACT(11, host.fwd_act);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(host.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    host.vrf_id = *vrf_id_ptr;
    host.flags |= RTK_L3_FLAG_IPV6;
    host.flags |= RTK_L3_FLAG_WITH_NEXTHOP;
    host.nexthop.intf_id = *intf_id_ptr;
    osal_memcpy(&host.nexthop.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        host.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        host.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        host.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        host.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        host.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_add(unit, &host), ret);
    /*DUMP_HOST(host);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_add_host_vrf_id_vrf_id_ip6_ip6_addr_nexthop_intf_id_mac_addr_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority */
#endif


#ifdef CMD_L3_SET_HOST_VRF_ID_VRF_ID_IP6_IP6_ADDR_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 set host vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_set_host_vrf_id_vrf_id_ip6_ip6_addr_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_host_t host;

    osal_memset(&host, 0, sizeof(host));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_HOST_ACT(10, host.fwd_act);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(host.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    host.vrf_id = *vrf_id_ptr;
    host.path_id = *path_id_ptr;
    host.flags |= RTK_L3_FLAG_IPV6;
    host.flags |= RTK_L3_FLAG_REPLACE;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        host.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        host.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        host.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        host.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        host.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_add(unit, &host), ret);
    /*DUMP_HOST(host);*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_DEL_HOST_VRF_ID_VRF_ID_IP6_IP6_ADDR
/*
 * l3 del host vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr>
 */
cparser_result_t
cparser_cmd_l3_del_host_vrf_id_vrf_id_ip6_ip6_addr(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_host_t host;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&host, 0, sizeof(host));
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(host.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    host.vrf_id = *vrf_id_ptr;
    host.flags |= RTK_L3_FLAG_IPV6;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_del(unit, &host), ret);
    /*DUMP_HOST(host);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_host_vrf_id_vrf_id_ip6_ip6_addr */
#endif

#ifdef CMD_L3_DEL_HOST_VRF_ID_VRF_ID_IP6_IP6_ADDR_PREFIX_LENGTH_PREFIX_LENGTH
/*
 * l3 del host vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> prefix-length <UINT:prefix_length>
 */
cparser_result_t
cparser_cmd_l3_del_host_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *prefix_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(route.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.flags |= RTK_L3_FLAG_IPV6;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_del_byNetwork(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_host_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length */
#endif

#ifdef CMD_L3_DEL_HOST_INTF_ID_INTF_ID_NEGATE_IP_IP6
/*
 * l3 del host intf-id <UINT:intf_id> { negate } ( ip | ip6 )
 */
cparser_result_t
cparser_cmd_l3_del_host_intf_id_intf_id_negate_ip_ip6(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_flag_t flag = RTK_L3_FLAG_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(TOKEN_NUM-1)))
        flag |= RTK_L3_FLAG_IPV6;

    if (7 == TOKEN_NUM)
        flag |= RTK_L3_FLAG_NEGATE;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_del_byIntfId(unit, *intf_id_ptr, flag), ret);
    /*DIAG_UTIL_MPRINTF("delete %s host %s %s\n", TOKEN_STR(TOKEN_NUM-1), (7 == TOKEN_NUM) ? "with negate": "");*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_host_intf_id_intf_id_negate_ip_ip6 */
#endif

#ifdef CMD_L3_DEL_HOST_ALL_IP_IP6
/*
 * l3 del host all ( ip | ip6 )
 */
cparser_result_t
cparser_cmd_l3_del_host_all_ip_ip6(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_flag_t flag = RTK_L3_FLAG_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(4)))
        flag |= RTK_L3_FLAG_IPV6;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_delAll(unit, flag), ret);
    /*DIAG_UTIL_MPRINTF("delete %s host all\n", TOKEN_STR(4));*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_host_all_ip_ip6 */
#endif

#ifdef CMD_L3_GET_HOST_VRF_ID_VRF_ID_IP_IP_ADDR_HIT_CLEAR
/*
 * l3 get host vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> { hit-clear }
 */
cparser_result_t
cparser_cmd_l3_get_host_vrf_id_vrf_id_ip_ip_addr_hit_clear(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_host_t host;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&host, 0, sizeof(host));
    host.ip_addr.ipv4 = *ip_addr_ptr;
    host.vrf_id = *vrf_id_ptr;
    host.flags |= (8 == TOKEN_NUM) ? RTK_L3_FLAG_HIT_CLEAR : RTK_L3_FLAG_NONE;
    DIAG_UTIL_ERR_CHK(rtk_l3_host_find(unit, &host), ret);

    DUMP_HOST(host);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_host_vrf_id_vrf_id_ip_ip_addr_hit_clear */
#endif

#ifdef CMD_L3_GET_HOST_VRF_ID_VRF_ID_IP6_IP6_ADDR_HIT_CLEAR
/*
 * l3 get host vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> { hit-clear }
 */
cparser_result_t
cparser_cmd_l3_get_host_vrf_id_vrf_id_ip6_ip6_addr_hit_clear(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_host_t host;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&host, 0, sizeof(host));
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(host.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    host.vrf_id = *vrf_id_ptr;
    host.flags |= RTK_L3_FLAG_IPV6;
    host.flags |= (8 == TOKEN_NUM) ? RTK_L3_FLAG_HIT_CLEAR : RTK_L3_FLAG_NONE;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_find(unit, &host), ret);

    DUMP_HOST(host);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_host_vrf_id_vrf_id_ip6_ip6_addr_hit_clear */
#endif

#ifdef CMD_L3_GET_HOST_CONFLICT_LIST_VRF_ID_VRF_ID_IP_IP_ADDR_MAX_HOST_MAX_HOST
/*
 * l3 get host conflict-list vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> max-host <UINT:max_host>
 */
cparser_result_t
cparser_cmd_l3_get_host_conflict_list_vrf_id_vrf_id_ip_ip_addr_max_host_max_host(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *max_host_ptr)
{
    uint32 unit;
    uint32 i = 0;
    int32 maxHostRet = 0;
    int32 ret = RT_ERR_FAILED;
    uint32 maxConflictHostNum, allocHostNum;
    rtk_switch_devInfo_t devInfo;
    rtk_l3_key_t key;
    rtk_l3_host_t *pHosts;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&devInfo, 0x00, sizeof(rtk_switch_devInfo_t));
    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    maxConflictHostNum = devInfo.capacityInfo.max_num_of_l3_conflict_host;
    allocHostNum = (*max_host_ptr > maxConflictHostNum) ? maxConflictHostNum : *max_host_ptr;
    pHosts = osal_alloc(sizeof(rtk_l3_host_t) * allocHostNum);
    DIAG_UTIL_ERR_CHK((NULL == pHosts)? RT_ERR_NULL_POINTER : RT_ERR_OK, ret);
    osal_memset(pHosts, 0x00, sizeof(rtk_l3_host_t) * allocHostNum);

    osal_memset(&key, 0x00, sizeof(key));
    key.ip_addr.ipv4 = *ip_addr_ptr;
    key.vrf_id = *vrf_id_ptr;

    if (RT_ERR_OK != (ret = rtk_l3_hostConflict_get(unit, &key, pHosts, allocHostNum, &maxHostRet)))
    {
        osal_free(pHosts);
        DIAG_UTIL_ERR_CHK(ret, ret);
    }

    for (i = 0; i < maxHostRet; i++)
    {
        DUMP_HOST(pHosts[i]);
    }

    osal_free(pHosts);
    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_host_conflict_list_vrf_id_vrf_id_ip_ip_addr_max_host_max_host */
#endif

#ifdef CMD_L3_GET_HOST_CONFLICT_LIST_VRF_ID_VRF_ID_IP6_IP6_ADDR_MAX_HOST_MAX_HOST
/*
 * l3 get host conflict-list vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> max-host <UINT:max_host>
 */
cparser_result_t
cparser_cmd_l3_get_host_conflict_list_vrf_id_vrf_id_ip6_ip6_addr_max_host_max_host(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *max_host_ptr)
{
    uint32 unit;
    uint32 i = 0;
    int32 maxHostRet = 0;
    int32 ret = RT_ERR_FAILED;
    uint32 maxConflictHostNum, allocHostNum;
    rtk_switch_devInfo_t devInfo;
    rtk_l3_key_t key;
    rtk_l3_host_t *pHosts;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&devInfo, 0x00, sizeof(rtk_switch_devInfo_t));
    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    maxConflictHostNum = devInfo.capacityInfo.max_num_of_l3_conflict_host;
    allocHostNum = (*max_host_ptr > maxConflictHostNum) ? maxConflictHostNum : *max_host_ptr;
    pHosts = osal_alloc(sizeof(rtk_l3_host_t) * allocHostNum);
    DIAG_UTIL_ERR_CHK((NULL == pHosts)? RT_ERR_NULL_POINTER : RT_ERR_OK, ret);
    osal_memset(pHosts, 0x00, sizeof(rtk_l3_host_t) * allocHostNum);

    osal_memset(&key, 0x00, sizeof(key));
    if (RT_ERR_OK != (ret = diag_util_str2ipv6(key.ip_addr.ipv6.octet, *ip6_addr_ptr)))
    {
        osal_free(pHosts);
        return CPARSER_NOT_OK;
    }

    key.vrf_id = *vrf_id_ptr;
    key.flags |= RTK_L3_FLAG_IPV6;

    if (RT_ERR_OK != (ret = rtk_l3_hostConflict_get(unit, &key, pHosts, allocHostNum, &maxHostRet)))
    {
        osal_free(pHosts);
        DIAG_UTIL_ERR_CHK(ret, ret);
    }

    for (i = 0; i < maxHostRet; i++)
    {
        DUMP_HOST(pHosts[i]);
    }

    osal_free(pHosts);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_host_conflict_list_vrf_id_vrf_id_ip6_ip6_addr_max_host_max_host */
#endif

#ifdef CMD_L3_SET_HOST_AGE_IP_IP6_CLEAR_HIT_BIT_REMOVE_UNUSED_ENTRY
/*
 * l3 set host age ( ip | ip6 ) ( clear-hit-bit | remove-unused-entry )
 */
cparser_result_t
cparser_cmd_l3_set_host_age_ip_ip6_clear_hit_bit_remove_unused_entry(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_flag_t flag = RTK_L3_FLAG_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(4)))
        flag |= RTK_L3_FLAG_IPV6;

    if ('c' == TOKEN_CHAR(5, 0))
        flag |= RTK_L3_FLAG_HIT;

    DIAG_UTIL_ERR_CHK(rtk_l3_host_age(unit, flag, NULL, NULL), ret);
    /*DIAG_UTIL_MPRINTF("%s host age: %s\n", TOKEN_STR(4), TOKEN_STR(5));*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_host_age_ip_ip6_clear_hit_bit_remove_unused_entry */
#endif

#ifdef CMD_L3_DUMP_HOST_IP_IP6_FROM_BEGIN_INDEX_BEGIN_TO_END_INDEX_END
/*
 * l3 dump host ( ip | ip6 ) from ( <UINT:begin_index> | begin ) to ( <UINT:end_index> | end )
 */
cparser_result_t
cparser_cmd_l3_dump_host_ip_ip6_from_begin_index_begin_to_end_index_end(
    cparser_context_t *context,
    uint32_t *begin_index_ptr,
    uint32_t *end_index_ptr)
{
    uint32 unit;
    int32 base = 0;
    int32 ret = RT_ERR_FAILED;
    rtk_l3_host_t host;
    rtk_l3_flag_t flag = RTK_L3_FLAG_NONE;
    uint32 ipv6 = FALSE;
    uint32 totCnt = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(3)))
    {
        flag = RTK_L3_FLAG_IPV6;
        ipv6 = TRUE;
    }

    if ('b' == TOKEN_CHAR(5, 0))
    {
        base = -1;  /* from beginning */
    }
    else
    {
        /* including the start number */
        base = ((*begin_index_ptr) > 0)? \
               ((*begin_index_ptr) - 1) : (-1);
    }

    DUMP_L3_HL(TRUE, ipv6);
    do
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_host_getNext(unit, flag, &base, &host), ret);

        if (('e' != TOKEN_CHAR(7, 0)) && (base > (*end_index_ptr)))
        {
            break;
        }
        else if (base >= 0)
        {
            totCnt++;
            DUMP_L3_HOST_ENTRY(base, ipv6, host);
        }
    } while (base >= 0);

    DIAG_UTIL_MPRINTF("\n Total entry number : %d \n", totCnt);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_dump_host_ip_ip6_from_begin_index_begin_to_end_index_end */
#endif

#ifdef CMD_L3_ADD_ROUTE_VRF_ID_VRF_ID_IP_IP_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 add route vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> prefix-length <UINT:prefix_length> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_add_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ROUTE_ACT(12, route.fwd_act);

    route.vrf_id = *vrf_id_ptr;
    route.path_id = *path_id_ptr;
    route.ip_addr.ipv4 = *ip_addr_ptr;
    route.prefix_len = *prefix_length_ptr;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        route.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        route.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        route.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        route.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        route.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_add(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_ADD_ROUTE_VRF_ID_VRF_ID_IP_IP_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_NEXTHOP_INTF_ID_MAC_ADDR_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 add route vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> prefix-length <UINT:prefix_length> nexthop <UINT:intf_id> <MACADDR:mac_addr> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_add_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_nexthop_intf_id_mac_addr_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *intf_id_ptr,
    cparser_macaddr_t *mac_addr_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ROUTE_ACT(13, route.fwd_act);

    route.vrf_id = *vrf_id_ptr;
    route.ip_addr.ipv4 = *ip_addr_ptr;
    route.prefix_len = *prefix_length_ptr;

    route.flags |= RTK_L3_FLAG_WITH_NEXTHOP;
    route.nexthop.intf_id = *intf_id_ptr;
    osal_memcpy(&route.nexthop.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        route.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        route.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        route.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        route.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        route.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_add(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_add_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_nexthop_intf_id_mac_addr_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority */
#endif

#ifdef CMD_L3_SET_ROUTE_VRF_ID_VRF_ID_IP_IP_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 set route vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> prefix-length <UINT:prefix_length> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_set_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ROUTE_ACT(12, route.fwd_act);

    route.vrf_id = *vrf_id_ptr;
    route.path_id = *path_id_ptr;
    route.ip_addr.ipv4 = *ip_addr_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.flags |= RTK_L3_FLAG_REPLACE;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        route.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        route.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        route.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        route.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        route.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_add(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_DEL_ROUTE_VRF_ID_VRF_ID_IP_IP_ADDR_PREFIX_LENGTH_PREFIX_LENGTH
/*
 * l3 del route vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> prefix-length <UINT:prefix_length>
 */
cparser_result_t
cparser_cmd_l3_del_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *prefix_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&route, 0, sizeof(route));
    route.ip_addr.ipv4 = *ip_addr_ptr;
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_del(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length */
#endif

#ifdef CMD_L3_ADD_ROUTE_VRF_ID_VRF_ID_IP6_IP6_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 add route vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> prefix-length <UINT:prefix_length> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_add_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ROUTE_ACT(12, route.fwd_act);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(route.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    route.vrf_id = *vrf_id_ptr;
    route.path_id = *path_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.flags |= RTK_L3_FLAG_IPV6;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        route.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        route.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        route.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        route.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        route.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_add(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_ADD_ROUTE_VRF_ID_VRF_ID_IP6_IP6_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_NEXTHOP_INTF_ID_MAC_ADDR_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 add route vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> prefix-length <UINT:prefix_length> nexthop <UINT:intf_id> <MACADDR:mac_addr> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_add_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_nexthop_intf_id_mac_addr_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *intf_id_ptr,
    cparser_macaddr_t *mac_addr_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ROUTE_ACT(12, route.fwd_act);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(route.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.flags |= RTK_L3_FLAG_IPV6;

    route.flags |= RTK_L3_FLAG_WITH_NEXTHOP;
    route.nexthop.intf_id = *intf_id_ptr;
    osal_memcpy(&route.nexthop.mac_addr, mac_addr_ptr, sizeof(rtk_mac_t));

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        route.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        route.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        route.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        route.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        route.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_add(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_add_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_nexthop_intf_id_mac_addr_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority */
#endif

#ifdef CMD_L3_SET_ROUTE_VRF_ID_VRF_ID_IP6_IP6_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 set route vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> prefix-length <UINT:prefix_length> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_set_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ROUTE_ACT(12, route.fwd_act);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(route.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    route.vrf_id = *vrf_id_ptr;
    route.path_id = *path_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.flags |= RTK_L3_FLAG_IPV6;
    route.flags |= RTK_L3_FLAG_REPLACE;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        route.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        route.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        route.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        route.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        route.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_add(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_DEL_ROUTE_VRF_ID_VRF_ID_IP6_IP6_ADDR_PREFIX_LENGTH_PREFIX_LENGTH
/*
 * l3 del route vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> prefix-length <UINT:prefix_length>
 */
cparser_result_t
cparser_cmd_l3_del_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *prefix_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(route.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.flags |= RTK_L3_FLAG_IPV6;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_del(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length */
#endif

#ifdef CMD_L3_DEL_ROUTE_INTF_ID_INTF_ID_NEGATE_IP_IP6
/*
 * l3 del route intf-id <UINT:intf_id> { negate } ( ip | ip6 )
 */
cparser_result_t
cparser_cmd_l3_del_route_intf_id_intf_id_negate_ip_ip6(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_flag_t flag = RTK_L3_FLAG_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(TOKEN_NUM-1)))
        flag |= RTK_L3_FLAG_IPV6;

    if (7 == TOKEN_NUM)
        flag |= RTK_L3_FLAG_NEGATE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_del_byIntfId(unit, flag,*intf_id_ptr), ret);
    /*DIAG_UTIL_MPRINTF("delete %s route %s %s\n", TOKEN_STR(TOKEN_NUM-1), (7 == TOKEN_NUM) ? "with negate": "");*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_route_intf_id_intf_id_negate_ip_ip6 */
#endif

#ifdef CMD_L3_DEL_ROUTE_ALL_IP_IP6
/*
 * l3 del route all ( ip | ip6 )
 */
cparser_result_t
cparser_cmd_l3_del_route_all_ip_ip6(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_flag_t flag = RTK_L3_FLAG_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(4)))
        flag |= RTK_L3_FLAG_IPV6;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_delAll(unit, flag), ret);
    /*DIAG_UTIL_MPRINTF("delete %s route all\n", TOKEN_STR(4));*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_route_all_ip_ip6 */
#endif

#ifdef CMD_L3_GET_ROUTE_VRF_ID_VRF_ID_IP_IP_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_HIT_CLEAR
/*
 * l3 get route vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> prefix-length <UINT:prefix_length> { hit-clear }
 */
cparser_result_t
cparser_cmd_l3_get_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_hit_clear(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *prefix_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&route, 0, sizeof(route));
    route.ip_addr.ipv4 = *ip_addr_ptr;
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.flags |= (10 == TOKEN_NUM) ? RTK_L3_FLAG_HIT_CLEAR : RTK_L3_FLAG_NONE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_get(unit, &route), ret);

    DUMP_ROUTE(route);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_hit_clear */
#endif

#ifdef CMD_L3_GET_ROUTE_VRF_ID_VRF_ID_IP6_IP6_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_HIT_CLEAR
/*
 * l3 get route vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> prefix-length <UINT:prefix_length> { hit-clear }
 */
cparser_result_t
cparser_cmd_l3_get_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_hit_clear(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *prefix_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(route.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.flags |= RTK_L3_FLAG_IPV6;
    route.flags |= (10 == TOKEN_NUM) ? RTK_L3_FLAG_HIT_CLEAR : RTK_L3_FLAG_NONE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_get(unit, &route), ret);

    DUMP_ROUTE(route);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_hit_clear */
#endif

#ifdef CMD_L3_ADD_ROUTE_VRF_ID_VRF_ID_IP_IP_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_SUFFIX_LENGTH_SUFFIX_LENGTH_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 add route vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> prefix-length <UINT:prefix_length> suffix-length <UINT:suffix_length> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_add_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_suffix_length_suffix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *suffix_length_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ROUTE_ACT(14, route.fwd_act);

    route.vrf_id = *vrf_id_ptr;
    route.path_id = *path_id_ptr;
    route.ip_addr.ipv4 = *ip_addr_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.suffix_len = *suffix_length_ptr;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        route.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        route.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        route.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        route.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        route.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_add(unit, &route), ret);

    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_add_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_suffix_length_suffix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority */
#endif

#ifdef CMD_L3_SET_ROUTE_VRF_ID_VRF_ID_IP_IP_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_SUFFIX_LENGTH_SUFFIX_LENGTH_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 set route vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> prefix-length <UINT:prefix_length> suffix-length <UINT:suffix_length> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_set_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_suffix_length_suffix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *suffix_length_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ROUTE_ACT(14, route.fwd_act);

    route.vrf_id = *vrf_id_ptr;
    route.path_id = *path_id_ptr;
    route.ip_addr.ipv4 = *ip_addr_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.suffix_len = *suffix_length_ptr;
    route.flags |= RTK_L3_FLAG_REPLACE;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        route.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        route.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        route.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        route.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        route.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_add(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_suffix_length_suffix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority */
#endif

#ifdef CMD_L3_DEL_ROUTE_VRF_ID_VRF_ID_IP_IP_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_SUFFIX_LENGTH_SUFFIX_LENGTH
/*
 * l3 del route vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> prefix-length <UINT:prefix_length> suffix-length <UINT:suffix_length>
 */
cparser_result_t
cparser_cmd_l3_del_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_suffix_length_suffix_length(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *suffix_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&route, 0, sizeof(route));
    route.ip_addr.ipv4 = *ip_addr_ptr;
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.suffix_len = *suffix_length_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_del(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_suffix_length_suffix_length */
#endif

#ifdef CMD_L3_ADD_ROUTE_VRF_ID_VRF_ID_IP6_IP6_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_SUFFIX_LENGTH_SUFFIX_LENGTH_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 add route vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> prefix-length <UINT:prefix_length> suffix-length <UINT:suffix_length> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_add_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_suffix_length_suffix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *suffix_length_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ROUTE_ACT(14, route.fwd_act);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(route.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    route.vrf_id = *vrf_id_ptr;
    route.path_id = *path_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.suffix_len = *suffix_length_ptr;
    route.flags |= RTK_L3_FLAG_IPV6;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        route.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        route.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        route.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        route.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        route.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_add(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_add_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_suffix_length_suffix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority */
#endif

#ifdef CMD_L3_SET_ROUTE_VRF_ID_VRF_ID_IP6_IP6_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_SUFFIX_LENGTH_SUFFIX_LENGTH_PATH_ID_PATH_ID_FWD_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_NULL_INTF_TTL_DEC_IGNORE_TTL_CHK_IGNORE_PRIORITY
/*
 * l3 set route vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> prefix-length <UINT:prefix_length> suffix-length <UINT:suffix_length> path-id <UINT:path_id> fwd-action ( drop | forward | trap-to-cpu | copy-to-cpu ) { null-intf } { ttl-dec-ignore } { ttl-chk-ignore } { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_l3_set_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_suffix_length_suffix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *suffix_length_ptr,
    uint32_t *path_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ROUTE_ACT(14, route.fwd_act);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(route.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    route.vrf_id = *vrf_id_ptr;
    route.path_id = *path_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.suffix_len = *suffix_length_ptr;
    route.flags |= RTK_L3_FLAG_IPV6;
    route.flags |= RTK_L3_FLAG_REPLACE;

    if (isdigit(TOKEN_CHAR(TOKEN_NUM-1, 0)))
    {
        route.qos_pri = atoi(TOKEN_STR(TOKEN_NUM-1));
        route.flags |= RTK_L3_FLAG_QOS_ASSIGN;
    }

    if (NULL != strstr(context->parser->cur_lines_body.buf, "null"))
        route.flags |= RTK_L3_FLAG_NULL_INTF;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "dec"))
        route.flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;

    if (NULL != strstr(context->parser->cur_lines_body.buf, "chk"))
        route.flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_add(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_suffix_length_suffix_length_path_id_path_id_fwd_action_drop_forward_trap_to_cpu_copy_to_cpu_null_intf_ttl_dec_ignore_ttl_chk_ignore_priority */
#endif

#ifdef CMD_L3_DEL_ROUTE_VRF_ID_VRF_ID_IP6_IP6_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_SUFFIX_LENGTH_SUFFIX_LENGTH
/*
 * l3 del route vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> prefix-length <UINT:prefix_length> suffix-length <UINT:suffix_length>
 */
cparser_result_t
cparser_cmd_l3_del_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_suffix_length_suffix_length(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *suffix_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(route.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.suffix_len = *suffix_length_ptr;
    route.flags |= RTK_L3_FLAG_IPV6;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_del(unit, &route), ret);
    /*DUMP_ROUTE(route);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_del_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_suffix_length_suffix_length */
#endif

#ifdef CMD_L3_GET_ROUTE_VRF_ID_VRF_ID_IP_IP_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_SUFFIX_LENGTH_SUFFIX_LENGTH_HIT_CLEAR
/*
 * l3 get route vrf-id <UINT:vrf_id> ip <IPV4ADDR:ip_addr> prefix-length <UINT:prefix_length> suffix-length <UINT:suffix_length> { hit-clear }
 */
cparser_result_t
cparser_cmd_l3_get_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_suffix_length_suffix_length_hit_clear(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    uint32_t *ip_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *suffix_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&route, 0, sizeof(route));
    route.ip_addr.ipv4 = *ip_addr_ptr;
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.suffix_len = *suffix_length_ptr;
    route.flags |= (12 == TOKEN_NUM) ? RTK_L3_FLAG_HIT_CLEAR : RTK_L3_FLAG_NONE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_get(unit, &route), ret);

    DUMP_ROUTE(route);

    diag_util_mprintf("");

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_route_vrf_id_vrf_id_ip_ip_addr_prefix_length_prefix_length_suffix_length_suffix_length_hit_clear */
#endif

#ifdef CMD_L3_GET_ROUTE_VRF_ID_VRF_ID_IP6_IP6_ADDR_PREFIX_LENGTH_PREFIX_LENGTH_SUFFIX_LENGTH_SUFFIX_LENGTH_HIT_CLEAR
/*
 * l3 get route vrf-id <UINT:vrf_id> ip6 <IPV6ADDR:ip6_addr> prefix-length <UINT:prefix_length> suffix-length <UINT:suffix_length> { hit-clear }
 */
cparser_result_t
cparser_cmd_l3_get_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_suffix_length_suffix_length_hit_clear(
    cparser_context_t *context,
    uint32_t *vrf_id_ptr,
    char **ip6_addr_ptr,
    uint32_t *prefix_length_ptr,
    uint32_t *suffix_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_route_t route;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&route, 0, sizeof(route));
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(route.ip_addr.ipv6.octet, *ip6_addr_ptr), ret);
    route.vrf_id = *vrf_id_ptr;
    route.prefix_len = *prefix_length_ptr;
    route.flags |= RTK_L3_FLAG_IPV6;
    route.flags |= (12 == TOKEN_NUM) ? RTK_L3_FLAG_HIT_CLEAR : RTK_L3_FLAG_NONE;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_get(unit, &route), ret);

    DUMP_ROUTE(route);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_route_vrf_id_vrf_id_ip6_ip6_addr_prefix_length_prefix_length_suffix_length_suffix_length_hit_clear */
#endif


#ifdef CMD_L3_SET_ROUTE_AGE_IP_IP6_CLEAR_HIT_BIT_REMOVE_UNUSED_ENTRY
/*
 * l3 set route age ( ip | ip6 ) ( clear-hit-bit | remove-unused-entry )
 */
cparser_result_t
cparser_cmd_l3_set_route_age_ip_ip6_clear_hit_bit_remove_unused_entry(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_flag_t flag = RTK_L3_FLAG_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(4)))
        flag |= RTK_L3_FLAG_IPV6;

    if ('c' == TOKEN_CHAR(5, 0))
        flag |= RTK_L3_FLAG_HIT;

    DIAG_UTIL_ERR_CHK(rtk_l3_route_age(unit, flag, NULL, NULL), ret);
    /*DIAG_UTIL_MPRINTF("%s route age: %s\n", TOKEN_STR(4), TOKEN_STR(5));*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_route_age_ip_ip6_clear_hit_bit_remove_unused_entry */
#endif

#ifdef CMD_L3_DUMP_ROUTE_IP_IP6_FROM_BEGIN_INDEX_BEGIN_TO_END_INDEX_END
/*
 * l3 dump route ( ip | ip6 ) from ( <UINT:begin_index> | begin ) to ( <UINT:end_index> | end )
 */
cparser_result_t
cparser_cmd_l3_dump_route_ip_ip6_from_begin_index_begin_to_end_index_end(
    cparser_context_t *context,
    uint32_t *begin_index_ptr,
    uint32_t *end_index_ptr)
{
    uint32 unit;
    int32 base = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_l3_route_t route;
    rtk_l3_flag_t flags = RTK_L3_FLAG_NONE;
    uint32 ipv6 = FALSE;
    uint32 totCnt = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    if (3 == osal_strlen(TOKEN_STR(3)))
    {
        flags = RTK_L3_FLAG_IPV6;
        ipv6 = TRUE;
    }

    if ('b' == TOKEN_CHAR(5, 0))
    {
        base = -1;  /* from beginning */
    }
    else
    {
        /* including the start number */
        base = ((*begin_index_ptr) > 0)? \
               ((*begin_index_ptr) - 1) : (-1);
    }

    DUMP_L3_HL(FALSE, ipv6);

    do
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_route_getNext(unit, flags, &base, &route), ret);

        if (('e' != TOKEN_CHAR(7, 0)) && (base > (*end_index_ptr)))
        {
            break;
        }
        else if (base >= 0)
        {
            totCnt++;
            DUMP_L3_ROUTE_ENTRY(base, ipv6, route);
        }
    } while (base >= 0);

    DIAG_UTIL_MPRINTF("\n Total entry number : %d \n", totCnt);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_dump_route_ip_ip6_from_begin_index_begin_to_end_index_end */
#endif

#ifdef CMD_L3_SET_IPUC_IP6UC_PKT_TO_CPU_TARGET_LOCAL_MASTER
/*
 * l3 set ( ipuc | ip6uc ) pkt-to-cpu-target ( local | master )
 */
cparser_result_t
cparser_cmd_l3_set_ipuc_ip6uc_pkt_to_cpu_target_local_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;
    rtk_l3_cpuTarget_t target = RTK_L3_CPUTARGET_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('u' == TOKEN_CHAR(2,2))
        type = RTK_L3_GCT_IPUC_PKT_TO_CPU_TARGET;
    else if ('6' == TOKEN_CHAR(2,2))
        type = RTK_L3_GCT_IP6UC_PKT_TO_CPU_TARGET;

    if ('l' == TOKEN_CHAR(4,0))
        target = RTK_L3_CPUTARGET_LOCAL;
    else if ('m' == TOKEN_CHAR(4,0))
        target = RTK_L3_CPUTARGET_MASTER;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, target), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_ipuc_ip6uc_pkt_to_cpu_target_local_master */
#endif

#ifdef CMD_L3_GET_IPUC_IP6UC_PKT_TO_CPU_TARGET
/*
 * l3 get ( ipuc | ip6uc ) pkt-to-cpu-target
 */
cparser_result_t
cparser_cmd_l3_get_ipuc_ip6uc_pkt_to_cpu_target(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;
    rtk_l3_cpuTarget_t target = RTK_L3_CPUTARGET_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('u' == TOKEN_CHAR(2,2))
        type = RTK_L3_GCT_IPUC_PKT_TO_CPU_TARGET;
    else if ('6' == TOKEN_CHAR(2,2))
        type = RTK_L3_GCT_IP6UC_PKT_TO_CPU_TARGET;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, type, (int32 *)&target), ret);
    DIAG_UTIL_MPRINTF("%s pkt-to-CPU target: %s\n", TOKEN_STR(2), text_l3_cputarget[target]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_ipuc_ip6uc_pkt_to_cpu_target */
#endif

#ifdef CMD_L3_SET_IPUC_IP6UC_ROUTING_STATE_ENABLE_DISABLE
/*
 * l3 set ( ipuc | ip6uc ) routing state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l3_set_ipuc_ip6uc_routing_state_enable_disable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(5, state);

    if ('u' == TOKEN_CHAR(2,2))
        type = RTK_L3_GCT_IPUC_GLB_EN;
    else if ('6' == TOKEN_CHAR(2,2))
        type = RTK_L3_GCT_IP6UC_GLB_EN;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, state), ret);
    /*DIAG_UTIL_MPRINTF("%s routing state: %s\n", TOKEN_STR(2), text_state[state]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_ipuc_ip6uc_routing_state_enable_disable */
#endif

#ifdef CMD_L3_GET_IPUC_IP6UC_ROUTING_STATE
/*
 * l3 get ( ipuc | ip6uc ) routing state
 */
cparser_result_t
cparser_cmd_l3_get_ipuc_ip6uc_routing_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('u' == TOKEN_CHAR(2,2))
        type = RTK_L3_GCT_IPUC_GLB_EN;
    else if ('6' == TOKEN_CHAR(2,2))
        type = RTK_L3_GCT_IP6UC_GLB_EN;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, type, (int32 *)&state), ret);

    DIAG_UTIL_MPRINTF("%s routing state: %s\n", TOKEN_STR(2), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_ipuc_ip6uc_routing_state */
#endif

#ifdef CMD_L3_SET_URPF_BASE_SELECTION_INTF_BASED_PORT_BASED
/*
 * l3 set urpf base-selection ( intf-based | port-based )
 */
cparser_result_t
cparser_cmd_l3_set_urpf_base_selection_intf_based_port_based(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_urpf_base_t base = RTK_L3_URPF_BASE_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('i' == TOKEN_CHAR(4, 0))
        base = RTK_L3_URPF_BASE_INTF;
    else if ('p' == TOKEN_CHAR(4, 0))
        base = RTK_L3_URPF_BASE_PORT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, RTK_L3_GCT_URPF_BASE_SEL, base), ret);
    /*DIAG_UTIL_MPRINTF("urpf base-selection: %s\n", text_urpf_base[base]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_urpf_base_selection_intf_based_port_based */
#endif

#ifdef CMD_L3_GET_URPF_BASE_SELECTION
/*
 * l3 get urpf base-selection
 */
cparser_result_t
cparser_cmd_l3_get_urpf_base_selection(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_urpf_base_t base = RTK_L3_URPF_BASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_URPF_BASE_SEL, (int32 *)&base), ret);

    DIAG_UTIL_MPRINTF("urpf base-selection: %s\n", text_urpf_base[base]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_urpf_base_selection */
#endif

#ifdef CMD_L3_SET_ROUTING_EXCEPTION_NON_IP_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * l3 set routing-exception non-ip-action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_non_ip_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = ACTION_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(4, action);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, RTK_L3_GCT_NON_IP_ACT, action), ret);
    /*DIAG_UTIL_MPRINTF("routing-exception non-ip-action: %s\n", text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_non_ip_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_L3_GET_ROUTING_EXCEPTION_NON_IP_ACTION
/*
 * l3 get routing-exception non-ip-action
 */
cparser_result_t
cparser_cmd_l3_get_routing_exception_non_ip_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_NON_IP_ACT, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("routing-exception non-ip-action: %s\n", text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_routing_exception_non_ip_action */
#endif

#ifdef CMD_L3_SET_ROUTING_EXCEPTION_IP_IP6_HEADER_ERROR_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER_HARD_DROP
/*
 * l3 set routing-exception ( ip | ip6 ) header-error-action ( drop | trap-to-cpu | trap-to-master | hard-drop )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_ip_ip6_header_error_action_drop_trap_to_cpu_trap_to_master_hard_drop(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(5, action);  /*hard-drop is forward for unicast*/

    if (2 == osal_strlen(TOKEN_STR(3)))
        type = RTK_L3_GCT_IP_HDR_ERR_ACT;
    else if (3 == osal_strlen(TOKEN_STR(3)))
        type = RTK_L3_GCT_IP6_HDR_ERR_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, action), ret);
    /*DIAG_UTIL_MPRINTF("%s header-error-action: %s\n", TOKEN_STR(3), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_ip_ip6_header_error_action_drop_trap_to_cpu_trap_to_master_hard_drop */
#endif

#ifdef CMD_L3_GET_ROUTING_EXCEPTION_IP_IP6_HEADER_ERROR_ACTION
/*
 * l3 get routing-exception ( ip | ip6 ) header-error-action
 */
cparser_result_t
cparser_cmd_l3_get_routing_exception_ip_ip6_header_error_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if (2 == osal_strlen(TOKEN_STR(3)))
        type = RTK_L3_GCT_IP_HDR_ERR_ACT;
    else if (3 == osal_strlen(TOKEN_STR(3)))
        type = RTK_L3_GCT_IP6_HDR_ERR_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, type, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("%s header-error-action: %s\n", TOKEN_STR(3), text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_routing_exception_ip_ip6_header_error_action */
#endif


#ifdef CMD_L3_SET_ROUTING_EXCEPTION_IPUC_BAD_SIP_BAD_DIP_ZERO_SIP_DMAC_BC_TTL_FAIL_MTU_FAIL_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * l3 set routing-exception ipuc ( bad-sip | bad-dip | zero-sip | dmac-bc | ttl-fail | mtu-fail ) action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_ipuc_bad_sip_bad_dip_zero_sip_dmac_bc_ttl_fail_mtu_fail_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(6, action);

    if ('b' == TOKEN_CHAR(4, 0))
    {
        if ('s' == TOKEN_CHAR(4, 4))
            type = RTK_L3_GCT_IPUC_BAD_SIP_ACT;
        else if ('d' == TOKEN_CHAR(4, 4))
            type = RTK_L3_GCT_IPUC_BAD_DIP_ACT;
    }
    else if ('z' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IPUC_ZERO_SIP_ACT;
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('b' == TOKEN_CHAR(4, 5))
            type = RTK_L3_GCT_IPUC_DMAC_BC_ACT;
    }
    else if ('t' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IPUC_TTL_FAIL_ACT;
    else if ('m' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IPUC_MTU_FAIL_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, action), ret);
    /*DIAG_UTIL_MPRINTF("ipuc %s action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_ipuc_bad_sip_bad_dip_zero_sip_dmac_bc_ttl_fail_mtu_fail_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_L3_SET_ROUTING_EXCEPTION_IPUC_DMAC_MC_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * l3 set routing-exception ipuc dmac-mc action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_ipuc_dmac_mc_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(6, action);

    type = RTK_L3_GCT_IPUC_DMAC_MC_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, action), ret);
    /*DIAG_UTIL_MPRINTF("ipuc %s action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_ipuc_dmac_mc_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_L3_SET_ROUTING_EXCEPTION_IPUC_DMAC_MC_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set routing-exception ipuc dmac-mc action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_ipuc_dmac_mc_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(6, action);

    type = RTK_L3_GCT_IPUC_DMAC_MC_ACT;
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, action), ret);
    /*DIAG_UTIL_MPRINTF("ipuc %s action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_ipuc_dmac_mc_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_SET_ROUTING_EXCEPTION_IPUC_HEADER_OPT_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set routing-exception ipuc header-opt action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_ipuc_header_opt_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(6, action);

    type = RTK_L3_GCT_IPUC_HDR_OPT_ACT;
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, action), ret);
    /*DIAG_UTIL_MPRINTF("ipuc %s action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_ipuc_header_opt_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_GET_ROUTING_EXCEPTION_IPUC_BAD_SIP_BAD_DIP_ZERO_SIP_DMAC_BC_DMAC_MC_TTL_FAIL_MTU_FAIL_HEADER_OPT_ACTION
/*
 * l3 get routing-exception ipuc ( bad-sip | bad-dip | zero-sip | dmac-bc | dmac-mc | ttl-fail | mtu-fail | header-opt ) action
 */
cparser_result_t
cparser_cmd_l3_get_routing_exception_ipuc_bad_sip_bad_dip_zero_sip_dmac_bc_dmac_mc_ttl_fail_mtu_fail_header_opt_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('b' == TOKEN_CHAR(4, 0))
    {
        if ('s' == TOKEN_CHAR(4, 4))
            type = RTK_L3_GCT_IPUC_BAD_SIP_ACT;
        else if ('d' == TOKEN_CHAR(4, 4))
            type = RTK_L3_GCT_IPUC_BAD_DIP_ACT;
    }
    else if ('z' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IPUC_ZERO_SIP_ACT;
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('b' == TOKEN_CHAR(4, 5))
            type = RTK_L3_GCT_IPUC_DMAC_BC_ACT;
        else if ('m' == TOKEN_CHAR(4, 5))
            type = RTK_L3_GCT_IPUC_DMAC_MC_ACT;
    }
    else if ('t' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IPUC_TTL_FAIL_ACT;
    else if ('m' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IPUC_MTU_FAIL_ACT;
    else if ('h' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IPUC_HDR_OPT_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, type, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("ipuc %s action: %s\n", TOKEN_STR(4), text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_routing_exception_ipuc_bad_sip_bad_dip_zero_sip_dmac_bc_dmac_mc_ttl_fail_mtu_fail_header_opt_action */
#endif

#ifdef CMD_L3_SET_ROUTING_EXCEPTION_IP6UC_HBH_ERR_HEADER_ROUTE_HBH_ACTION_FORWARD_COPY_TO_CPU_COPY_TO_MASTER
/*
 * l3 set routing-exception ip6uc ( hbh-err | header-route | hbh ) action ( forward | copy-to-cpu | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_ip6uc_hbh_err_header_route_hbh_action_forward_copy_to_cpu_copy_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(6, action);

    if (0 == osal_strcmp(TOKEN_STR(4), "hbh"))
        type = RTK_L3_GCT_IP6UC_HBH_ACT;
    else if ('b' == TOKEN_CHAR(4, 1))
        type = RTK_L3_GCT_IP6UC_HBH_ERR_ACT;
    else if ('e' == TOKEN_CHAR(4, 1))
        type = RTK_L3_GCT_IP6UC_HDR_ROUTE_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, action), ret);
    /*DIAG_UTIL_MPRINTF("ip6uc %s action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}
#endif

#ifdef CMD_L3_SET_ROUTING_EXCEPTION_IP6UC_BAD_SIP_BAD_DIP_ZERO_SIP_HBH_ERR_HEADER_ROUTE_HBH_HL_FAIL_MTU_FAIL_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * l3 set routing-exception ip6uc ( bad-sip | bad-dip | zero-sip | hbh-err | header-route | hbh | hl-fail | mtu-fail ) action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_ip6uc_bad_sip_bad_dip_zero_sip_hbh_err_header_route_hbh_hl_fail_mtu_fail_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(6, action);

    if ('b' == TOKEN_CHAR(4, 0))
    {
        if ('s' == TOKEN_CHAR(4, 4))
            type = RTK_L3_GCT_IP6UC_BAD_SIP_ACT;
        else if ('d' == TOKEN_CHAR(4, 4))
            type = RTK_L3_GCT_IP6UC_BAD_DIP_ACT;
    }
    else if ('z' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IP6UC_ZERO_SIP_ACT;
    else if ('d' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IP6UC_DMAC_MISMATCH_ACT;
    else if ('h' == TOKEN_CHAR(4, 0))
    {
        if (0 == osal_strcmp(TOKEN_STR(4), "hbh"))
            type = RTK_L3_GCT_IP6UC_HBH_ACT;
        else if (0 == osal_strcmp(TOKEN_STR(4), "hbh-err"))
            type = RTK_L3_GCT_IP6UC_HBH_ERR_ACT;
        else if ('l' == TOKEN_CHAR(4, 1))
            type = RTK_L3_GCT_IP6UC_HL_FAIL_ACT;
        else if ('e' == TOKEN_CHAR(4, 1))
            type = RTK_L3_GCT_IP6UC_HDR_ROUTE_ACT;
    }
    else if ('m' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IP6UC_MTU_FAIL_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, action), ret);
    /*DIAG_UTIL_MPRINTF("ip6uc %s action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_ip6uc_bad_sip_bad_dip_zero_sip_hbh_err_header_route_hbh_hl_fail_mtu_fail_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_L3_SET_ROUTING_EXCEPTION_IP6UC_DMAC_MISMATCH_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * l3 set routing-exception ip6uc dmac-mismatch action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_ip6uc_dmac_mismatch_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(6, action);

    type = RTK_L3_GCT_IP6UC_DMAC_MISMATCH_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, action), ret);
    /*DIAG_UTIL_MPRINTF("ip6uc %s action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_ip6uc_dmac_mismatch_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_L3_SET_ROUTING_EXCEPTION_IP6UC_DMAC_MISMATCH_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set routing-exception ip6uc dmac-mismatch action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_ip6uc_dmac_mismatch_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(6, action);

    type = RTK_L3_GCT_IP6UC_DMAC_MISMATCH_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, action), ret);
    /*DIAG_UTIL_MPRINTF("ip6uc %s action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_ip6uc_dmac_mismatch_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif


#ifdef CMD_L3_GET_ROUTING_EXCEPTION_IP6UC_BAD_SIP_BAD_DIP_ZERO_SIP_DMAC_MISMATCH_HBH_ERR_HEADER_ROUTE_HBH_HL_FAIL_MTU_FAIL_ACTION
/*
 * l3 get routing-exception ip6uc ( bad-sip | bad-dip | zero-sip | dmac-mismatch | hbh-err | header-route | hbh | hl-fail | mtu-fail ) action
 */
cparser_result_t
cparser_cmd_l3_get_routing_exception_ip6uc_bad_sip_bad_dip_zero_sip_dmac_mismatch_hbh_err_header_route_hbh_hl_fail_mtu_fail_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('b' == TOKEN_CHAR(4, 0))
    {
        if ('s' == TOKEN_CHAR(4, 4))
            type = RTK_L3_GCT_IP6UC_BAD_SIP_ACT;
        else if ('d' == TOKEN_CHAR(4, 4))
            type = RTK_L3_GCT_IP6UC_BAD_DIP_ACT;
    }
    else if ('z' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IP6UC_ZERO_SIP_ACT;
    else if ('d' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IP6UC_DMAC_MISMATCH_ACT;
    else if ('h' == TOKEN_CHAR(4, 0))
    {
        if (0 == osal_strcmp(TOKEN_STR(4), "hbh"))
            type = RTK_L3_GCT_IP6UC_HBH_ACT;
        else if (0 == osal_strcmp(TOKEN_STR(4), "hbh-err"))
            type = RTK_L3_GCT_IP6UC_HBH_ERR_ACT;
        else if ('l' == TOKEN_CHAR(4, 1))
            type = RTK_L3_GCT_IP6UC_HL_FAIL_ACT;
        else if ('e' == TOKEN_CHAR(4, 1))
            type = RTK_L3_GCT_IP6UC_HDR_ROUTE_ACT;
    }
    else if ('m' == TOKEN_CHAR(4, 0))
        type = RTK_L3_GCT_IP6UC_MTU_FAIL_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, type, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("ip6uc %s action: %s\n", TOKEN_STR(4), text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_routing_exception_ip6uc_bad_sip_bad_dip_zero_sip_dmac_mismatch_hbh_err_header_route_hbh_hl_fail_mtu_fail_action */
#endif

#ifdef CMD_L3_SET_ROUTING_EXCEPTION_NEXT_HOP_ERROR_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * l3 set routing-exception next-hop-error-action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_next_hop_error_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(4, action);

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, RTK_L3_GCT_NH_ERR_ACT, action), ret);
    /*DIAG_UTIL_MPRINTF("next-hop-error-action: %s\n", text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_next_hop_error_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_L3_GET_ROUTING_EXCEPTION_NEXT_HOP_ERROR_ACTION
/*
 * l3 get routing-exception next-hop-error-action
 */
cparser_result_t
cparser_cmd_l3_get_routing_exception_next_hop_error_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_NH_ERR_ACT, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("next-hop-error-action: %s\n", text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_routing_exception_next_hop_error_action */
#endif


#ifdef CMD_L3_SET_ROUTING_EXCEPTION_NEXT_HOP_AGE_OUT_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set routing-exception next-hop-age-out-action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_routing_exception_next_hop_age_out_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(4, action);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, RTK_L3_GCT_NH_AGE_OUT_ACT, action), ret);
    /*DIAG_UTIL_MPRINTF("next-hop-age-out-action: %s\n", text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_routing_exception_next_hop_age_out_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_GET_ROUTING_EXCEPTION_NEXT_HOP_AGE_OUT_ACTION
/*
 * l3 get routing-exception next-hop-age-out-action
 */
cparser_result_t
cparser_cmd_l3_get_routing_exception_next_hop_age_out_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_get(unit, RTK_L3_GCT_NH_AGE_OUT_ACT, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("next-hop-age-out-action: %s\n", text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_routing_exception_next_hop_age_out_action */
#endif

#ifdef CMD_L3_SET_REFERENCE_COUNT_CHECK_STATE_ENABLE_DISABLE
/*
 * l3 set reference-count-check state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l3_set_reference_count_check_state_enable_disable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_globalCtrlType_t type = RTK_L3_GCT_REFCNT_CHK;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(4, state);

    DIAG_UTIL_ERR_CHK(rtk_l3_globalCtrl_set(unit, type, state), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_reference_count_check_state_enable_disable */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IPUC_IP6UC_IPMC_IP6MC_ROUTING_STATE_ENABLE_DISABLE
/*
 * l3 set intf <UINT:intf_id> ( ipuc | ip6uc | ipmc | ip6mc ) routing state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_ipmc_ip6mc_routing_state_enable_disable(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(7, state);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_ROUTE_EN;
    else if ('m' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPMC_ROUTE_EN;
    else if ('6' == TOKEN_CHAR(4, 2))
    {
        if ('u' == TOKEN_CHAR(4, 3))
            type = RTK_L3_ICT_IP6UC_ROUTE_EN;
        else if ('m' == TOKEN_CHAR(4, 3))
            type = RTK_L3_ICT_IP6MC_ROUTE_EN;
    }

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, type, state), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_ipmc_ip6mc_routing_state_enable_disable */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IPUC_IP6UC_IPMC_IP6MC_ROUTING_STATE
/*
 * l3 get intf <UINT:intf_id> ( ipuc | ip6uc | ipmc | ip6mc ) routing state
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_ipmc_ip6mc_routing_state(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_ROUTE_EN;
    else if ('m' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPMC_ROUTE_EN;
    else if ('6' == TOKEN_CHAR(4, 2))
    {
        if ('u' == TOKEN_CHAR(4, 3))
            type = RTK_L3_ICT_IP6UC_ROUTE_EN;
        else if ('m' == TOKEN_CHAR(4, 3))
            type = RTK_L3_ICT_IP6MC_ROUTE_EN;
    }

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, type, (int32 *)&state), ret);

    DIAG_UTIL_MPRINTF("%s routing state: %s\n", TOKEN_STR(4), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_ipmc_ip6mc_routing_state */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IPUC_IP6UC_URPF_STATE_ENABLE_DISABLE
/*
 * l3 set intf <UINT:intf_id> ( ipuc | ip6uc ) urpf state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_urpf_state_enable_disable(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(7, state);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_URPF_CHK_EN;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6UC_URPF_CHK_EN;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, type, state), ret);
    /*DIAG_UTIL_MPRINTF("%s urpf state: %s\n", TOKEN_STR(4), text_state[state]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_urpf_state_enable_disable */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IPUC_IP6UC_URPF_STATE
/*
 * l3 get intf <UINT:intf_id> ( ipuc | ip6uc ) urpf state
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_urpf_state(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_URPF_CHK_EN;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6UC_URPF_CHK_EN;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, type, (int32 *)&state), ret);

    DIAG_UTIL_MPRINTF("%s urpf state: %s\n", TOKEN_STR(4), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_urpf_state */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IPUC_IP6UC_URPF_DEFAULT_ROUTE_STATE_ENABLE_DISABLE
/*
 * l3 set intf <UINT:intf_id> ( ipuc | ip6uc ) urpf default-route state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_urpf_default_route_state_enable_disable(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(8, state);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_URPF_DFLT_ROUTE_EN;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6UC_URPF_DFLT_ROUTE_EN;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, type, state), ret);
    /*DIAG_UTIL_MPRINTF("%s urpf default-route state: %s\n", TOKEN_STR(4), text_state[state]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_urpf_default_route_state_enable_disable */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IPUC_IP6UC_URPF_DEFAULT_ROUTE_STATE
/*
 * l3 get intf <UINT:intf_id> ( ipuc | ip6uc ) urpf default-route state
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_urpf_default_route_state(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_URPF_DFLT_ROUTE_EN;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6UC_URPF_DFLT_ROUTE_EN;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, type, (int32 *)&state), ret);

    DIAG_UTIL_MPRINTF("%s urpf default-route state: %s\n", TOKEN_STR(4), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_urpf_default_route_state */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IPUC_IP6UC_URPF_MODE_LOOSE_STRICT
/*
 * l3 set intf <UINT:intf_id> ( ipuc | ip6uc ) urpf mode ( loose | strict )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_urpf_mode_loose_strict(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32 unit;
    uint32 mode = RTK_L3_URPF_MODE_END;
    int32  ret = RT_ERR_FAILED;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('l' == TOKEN_CHAR(7, 0))
        mode = RTK_L3_URPF_MODE_LOOSE;
    else if ('s' == TOKEN_CHAR(7, 0))
        mode = RTK_L3_URPF_MODE_STRICT;

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_URPF_MODE;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6UC_URPF_MODE;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, type, mode), ret);
    /*DIAG_UTIL_MPRINTF("%s urpf mode: %s\n", TOKEN_STR(4), text_urpf_mode[mode]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_urpf_mode_loose_strict */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IPUC_IP6UC_URPF_MODE
/*
 * l3 get intf <UINT:intf_id> ( ipuc | ip6uc ) urpf mode
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_urpf_mode(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32 unit;
    int32 mode = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_URPF_MODE;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6UC_URPF_MODE;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, type, &mode), ret);

    DIAG_UTIL_MPRINTF("%s urpf mode: %s\n", TOKEN_STR(4), text_urpf_mode[mode]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_urpf_mode */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IPUC_IP6UC_URPF_FAIL_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set intf <UINT:intf_id> ( ipuc | ip6uc ) urpf fail-action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_urpf_fail_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(7, action);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_URPF_FAIL_ACT;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6UC_URPF_FAIL_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, type, action), ret);
    /*DIAG_UTIL_MPRINTF("%s urpf fail-action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_urpf_fail_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IPUC_IP6UC_URPF_FAIL_ACTION
/*
 * l3 get intf <UINT:intf_id> ( ipuc | ip6uc ) urpf fail-action
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_urpf_fail_action(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_URPF_FAIL_ACT;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6UC_URPF_FAIL_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, type, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("%s urpf fail-action: %s\n", TOKEN_STR(4), text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_urpf_fail_action */
#endif

#ifdef CMD_L3_SET_PORT_PORTS_ALL_IPUC_IP6UC_URPF_STATE_ENABLE_DISABLE
/*
 * l3 set port ( <PORT_LIST:ports> | all ) ( ipuc | ip6uc ) urpf state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l3_set_port_ports_all_ipuc_ip6uc_urpf_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_portCtrlType_t type = RTK_L3_PCT_NONE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(7, state);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IPUC_URPF_CHK_EN;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IP6UC_URPF_CHK_EN;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_portCtrl_set(unit, port, type, state), ret);
        /*DIAG_UTIL_MPRINTF("port %u: %s urpf state: %s\n", port, TOKEN_STR(4), text_state[state]);*/
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_port_ports_all_ipuc_ip6uc_urpf_state_enable_disable */
#endif

#ifdef CMD_L3_GET_PORT_PORTS_ALL_IPUC_IP6UC_URPF_STATE
/*
 * l3 get port ( <PORT_LIST:ports> | all ) ( ipuc | ip6uc ) urpf state
 */
cparser_result_t
cparser_cmd_l3_get_port_ports_all_ipuc_ip6uc_urpf_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_portCtrlType_t type = RTK_L3_PCT_NONE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IPUC_URPF_CHK_EN;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IP6UC_URPF_CHK_EN;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_portCtrl_get(unit, port, type, (int32 *)&state), ret);

        DIAG_UTIL_MPRINTF("port %u: %s urpf state: %s\n", port, TOKEN_STR(4), text_state[state]);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_port_ports_all_ipuc_ip6uc_urpf_state */
#endif

#ifdef CMD_L3_SET_PORT_PORTS_ALL_IPUC_IP6UC_URPF_DEFAULT_ROUTE_STATE_ENABLE_DISABLE
/*
 * l3 set port ( <PORT_LIST:ports> | all ) ( ipuc | ip6uc ) urpf default-route state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l3_set_port_ports_all_ipuc_ip6uc_urpf_default_route_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_portCtrlType_t type = RTK_L3_PCT_NONE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(8, state);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IPUC_URPF_DFLT_ROUTE_EN;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IP6UC_URPF_DFLT_ROUTE_EN;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_portCtrl_set(unit, port, type, state), ret);
        /*DIAG_UTIL_MPRINTF("port %u: %s urpf default-route state: %s\n", port, TOKEN_STR(4), text_state[state]);*/
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_port_ports_all_ipuc_ip6uc_urpf_default_route_state_enable_disable */
#endif

#ifdef CMD_L3_GET_PORT_PORTS_ALL_IPUC_IP6UC_URPF_DEFAULT_ROUTE_STATE
/*
 * l3 get port ( <PORT_LIST:ports> | all ) ( ipuc | ip6uc ) urpf default-route state
 */
cparser_result_t
cparser_cmd_l3_get_port_ports_all_ipuc_ip6uc_urpf_default_route_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_l3_portCtrlType_t type = RTK_L3_PCT_NONE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IPUC_URPF_DFLT_ROUTE_EN;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IP6UC_URPF_DFLT_ROUTE_EN;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_portCtrl_get(unit, port, type, (int32 *)&state), ret);

        DIAG_UTIL_MPRINTF("port %u: %s urpf default-route state: %s\n", port, TOKEN_STR(4), text_state[state]);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_port_ports_all_ipuc_ip6uc_urpf_default_route_state */
#endif

#ifdef CMD_L3_SET_PORT_PORTS_ALL_IPUC_IP6UC_URPF_MODE_LOOSE_STRICT
/*
 * l3 set port ( <PORT_LIST:ports> | all ) ( ipuc | ip6uc ) urpf mode ( loose | strict )
 */
cparser_result_t
cparser_cmd_l3_set_port_ports_all_ipuc_ip6uc_urpf_mode_loose_strict(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    uint32 mode = RTK_L3_URPF_MODE_END;
    rtk_l3_portCtrlType_t type = RTK_L3_PCT_NONE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('l' == TOKEN_CHAR(7, 0))
        mode = RTK_L3_URPF_MODE_LOOSE;
    else if ('s' == TOKEN_CHAR(7, 0))
        mode = RTK_L3_URPF_MODE_STRICT;

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IPUC_URPF_MODE;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IP6UC_URPF_MODE;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_portCtrl_set(unit, port, type, mode), ret);
        /*DIAG_UTIL_MPRINTF("port %u: %s urpf mode: %s\n", port, TOKEN_STR(4), text_urpf_mode[mode]);*/
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_port_ports_all_ipuc_ip6uc_urpf_mode_loose_strict */
#endif

#ifdef CMD_L3_GET_PORT_PORTS_ALL_IPUC_IP6UC_URPF_MODE
/*
 * l3 get port ( <PORT_LIST:ports> | all ) ( ipuc | ip6uc ) urpf mode
 */
cparser_result_t
cparser_cmd_l3_get_port_ports_all_ipuc_ip6uc_urpf_mode(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    int32   mode = 0;
    rtk_l3_portCtrlType_t type = RTK_L3_PCT_NONE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IPUC_URPF_MODE;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IP6UC_URPF_MODE;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_portCtrl_get(unit, port, type, (int32 *)&mode), ret);

        DIAG_UTIL_MPRINTF("port %u: %s urpf mode: %s\n", port, TOKEN_STR(4), text_urpf_mode[mode]);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_port_ports_all_ipuc_ip6uc_urpf_mode */
#endif

#ifdef CMD_L3_SET_PORT_PORTS_ALL_IPUC_IP6UC_URPF_FAIL_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set port ( <PORT_LIST:ports> | all ) ( ipuc | ip6uc ) urpf fail-action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_port_ports_all_ipuc_ip6uc_urpf_fail_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_portCtrlType_t type = RTK_L3_PCT_NONE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(7, action);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IPUC_URPF_FAIL_ACT;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IP6UC_URPF_FAIL_ACT;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_portCtrl_set(unit, port, type, action), ret);
        /*DIAG_UTIL_MPRINTF("port %u: %s urpf fail-action: %s\n", port, TOKEN_STR(4), text_l3_action[action]);*/
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_port_ports_all_ipuc_ip6uc_urpf_fail_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_GET_PORT_PORTS_ALL_IPUC_IP6UC_URPF_FAIL_ACTION
/*
 * l3 get port ( <PORT_LIST:ports> | all ) ( ipuc | ip6uc ) urpf fail-action
 */
cparser_result_t
cparser_cmd_l3_get_port_ports_all_ipuc_ip6uc_urpf_fail_action(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_portCtrlType_t type = RTK_L3_PCT_NONE;
    rtk_port_t  port;
    diag_portlist_t  portmask;

    DIAG_UTIL_FUNC_INIT(unit);

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IPUC_URPF_FAIL_ACT;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_PCT_IP6UC_URPF_FAIL_ACT;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l3_portCtrl_get(unit, port, type, (int32 *)&action), ret);

        DIAG_UTIL_MPRINTF("port %u: %s urpf fail-action: %s\n", port, TOKEN_STR(4), text_l3_action[action]);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_port_ports_all_ipuc_ip6uc_urpf_fail_action */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IPUC_IP6UC_ICMP_REDIRECT_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set intf <UINT:intf_id> ( ipuc | ip6uc ) icmp redirect-action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_icmp_redirect_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(7, action);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_ICMP_REDIR_ACT;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6UC_ICMP_REDIR_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, type, action), ret);
    /*DIAG_UTIL_MPRINTF("%s icmp redirect-action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ipuc_ip6uc_icmp_redirect_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IPUC_IP6UC_ICMP_REDIRECT_ACTION
/*
 * l3 get intf <UINT:intf_id> ( ipuc | ip6uc ) icmp redirect-action
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_icmp_redirect_action(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('u' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPUC_ICMP_REDIR_ACT;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6UC_ICMP_REDIR_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, type, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("%s icmp redirect-action: %s\n", TOKEN_STR(4), text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ipuc_ip6uc_icmp_redirect_action */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IPMC_IP6MC_ROUTING_LOOKUP_MISS_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * l3 set intf <UINT:intf_id> ( ipmc | ip6mc ) routing lookup-miss-action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ipmc_ip6mc_routing_lookup_miss_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(7, action);

    if ('m' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPMC_ROUTE_LU_MIS_ACT;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6MC_ROUTE_LU_MIS_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, type, action), ret);
    /*DIAG_UTIL_MPRINTF("%s lookup-miss-action: %s\n", TOKEN_STR(4), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ipmc_ip6mc_routing_lookup_miss_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IPMC_IP6MC_ROUTING_LOOKUP_MISS_ACTION
/*
 * l3 get intf <UINT:intf_id> ( ipmc | ip6mc ) routing lookup-miss-action
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ipmc_ip6mc_routing_lookup_miss_action(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('m' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IPMC_ROUTE_LU_MIS_ACT;
    else if ('6' == TOKEN_CHAR(4, 2))
        type = RTK_L3_ICT_IP6MC_ROUTE_LU_MIS_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, type, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("%s lookup-miss-action: %s\n", TOKEN_STR(4), text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ipmc_ip6mc_routing_lookup_miss_action */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IPMC_DIP_224_0_0_X_224_0_1_X_239_X_X_X_ACTION_FORWARD_DROP_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set intf <UINT:intf_id> ipmc dip ( 224-0-0-x | 224-0-1-x | 239-x-x-x ) action ( forward | drop | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ipmc_dip_224_0_0_x_224_0_1_x_239_x_x_x_action_forward_drop_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(8, action);

    if ('0' == TOKEN_CHAR(6, 6))
        type = RTK_L3_ICT_IPMC_DIP_224_0_0_X_ACT;
    else if ('1' == TOKEN_CHAR(6, 6))
        type = RTK_L3_ICT_IPMC_DIP_224_0_1_X_ACT;
    else if ('x' == TOKEN_CHAR(6, 6))
        type = RTK_L3_ICT_IPMC_DIP_239_X_X_X_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, type, action), ret);
    /*DIAG_UTIL_MPRINTF("ipmc %s: %s\n", TOKEN_STR(6), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ipmc_dip_224_0_0_x_224_0_1_x_239_x_x_x_action_forward_drop_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IPMC_DIP_224_0_0_X_224_0_1_X_239_X_X_X_ACTION
/*
 * l3 get intf <UINT:intf_id> ipmc dip ( 224-0-0-x | 224-0-1-x | 239-x-x-x ) action
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ipmc_dip_224_0_0_x_224_0_1_x_239_x_x_x_action(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('0' == TOKEN_CHAR(6, 6))
        type = RTK_L3_ICT_IPMC_DIP_224_0_0_X_ACT;
    else if ('1' == TOKEN_CHAR(6, 6))
        type = RTK_L3_ICT_IPMC_DIP_224_0_1_X_ACT;
    else if ('x' == TOKEN_CHAR(6, 6))
        type = RTK_L3_ICT_IPMC_DIP_239_X_X_X_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, type, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("ipmc %s: %s\n", TOKEN_STR(6), text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ipmc_dip_224_0_0_x_224_0_1_x_239_x_x_x_action */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IP6MC_DIP_FF0X_0000_XXXX_FF0X_XXXX_XXXX_FF0X_DB8_0_0_ACTION_FORWARD_DROP_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set intf <UINT:intf_id> ip6mc dip ( ff0x-0000-xxxx | ff0x-xxxx-xxxx | ff0x-db8-0-0 ) action ( forward | drop | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ip6mc_dip_ff0x_0000_xxxx_ff0x_xxxx_xxxx_ff0x_db8_0_0_action_forward_drop_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(8, action);

    if ('0' == TOKEN_CHAR(6, 5))
        type = RTK_L3_ICT_IP6MC_DIP_FF0X_0000_XXXX_ACT;
    else if ('x' == TOKEN_CHAR(6, 5))
        type = RTK_L3_ICT_IP6MC_DIP_FF0X_XXXX_XXXX_ACT;
    else if ('d' == TOKEN_CHAR(6, 5))
        type = RTK_L3_ICT_IP6MC_DIP_FF0X_DB8_0_0_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, type, action), ret);
    /*DIAG_UTIL_MPRINTF("ip6mc %s: %s\n", TOKEN_STR(6), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ip6mc_dip_ff0x_0000_xxxx_ff0x_xxxx_xxxx_ff0x_db8_0_0_action_forward_drop_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IP6MC_DIP_FF0X_0000_XXXX_FF0X_XXXX_XXXX_FF0X_DB8_0_0_ACTION
/*
 * l3 get intf <UINT:intf_id> ip6mc dip ( ff0x-0000-xxxx | ff0x-xxxx-xxxx | ff0x-db8-0-0 ) action
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ip6mc_dip_ff0x_0000_xxxx_ff0x_xxxx_xxxx_ff0x_db8_0_0_action(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('0' == TOKEN_CHAR(6, 5))
        type = RTK_L3_ICT_IP6MC_DIP_FF0X_0000_XXXX_ACT;
    else if ('x' == TOKEN_CHAR(6, 5))
        type = RTK_L3_ICT_IP6MC_DIP_FF0X_XXXX_XXXX_ACT;
    else if ('d' == TOKEN_CHAR(6, 5))
        type = RTK_L3_ICT_IP6MC_DIP_FF0X_DB8_0_0_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, type, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("ip6mc %s: %s\n", TOKEN_STR(6), text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ip6mc_dip_ff0x_0000_xxxx_ff0x_xxxx_xxxx_ff0x_db8_0_0_action */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_IP6_NEIGHBOR_DISCOVERY_ACTION_FORWARD_FLOOD_IN_VLAN_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set intf <UINT:intf_id> ip6 neighbor-discovery action ( forward | flood-in-vlan | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_ip6_neighbor_discovery_action_forward_flood_in_vlan_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(7, action);
    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, RTK_L3_ICT_IP6_ND_ACT, action), ret);
    /*DIAG_UTIL_MPRINTF("ip6 neighbor-discovery action: %s\n", text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_ip6_neighbor_discovery_action_forward_flood_in_vlan_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_IP6_NEIGHBOR_DISCOVERY_ACTION
/*
 * l3 get intf <UINT:intf_id> ip6 neighbor-discovery action
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_ip6_neighbor_discovery_action(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, RTK_L3_ICT_IP6_ND_ACT, (int32 *)(int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("ip6 neighbor-discovery action: %s\n", text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_ip6_neighbor_discovery_action */
#endif

#ifdef CMD_L3_SET_INTF_INTF_ID_PBR_IPUC_IP6UC_ICMP_REDIRECT_ACTION_DROP_FORWARD_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * l3 set intf <UINT:intf_id> pbr ( ipuc | ip6uc ) icmp redirect-action ( drop | forward | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_l3_set_intf_intf_id_pbr_ipuc_ip6uc_icmp_redirect_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(8, action);

    if ('u' == TOKEN_CHAR(5, 2))
        type = RTK_L3_ICT_IPUC_PBR_ICMP_REDIR_ACT;
    else if ('6' == TOKEN_CHAR(5, 2))
        type = RTK_L3_ICT_IP6UC_PBR_ICMP_REDIR_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_set(unit, *intf_id_ptr, type, action), ret);
    /*DIAG_UTIL_MPRINTF("%s icmp redirect-action: %s\n", TOKEN_STR(5), text_l3_action[action]);*/

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_set_intf_intf_id_pbr_ipuc_ip6uc_icmp_redirect_action_drop_forward_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_L3_GET_INTF_INTF_ID_PBR_IPUC_IP6UC_ICMP_REDIRECT_ACTION
/*
 * l3 get intf <UINT:intf_id> pbr ( ipuc | ip6uc ) icmp redirect-action
 */
cparser_result_t
cparser_cmd_l3_get_intf_intf_id_pbr_ipuc_ip6uc_icmp_redirect_action(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_l3_act_t action = RTK_L3_ACT_END;
    rtk_l3_intfCtrlType_t type = RTK_L3_ICT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('u' == TOKEN_CHAR(5, 2))
        type = RTK_L3_ICT_IPUC_PBR_ICMP_REDIR_ACT;
    else if ('6' == TOKEN_CHAR(5, 2))
        type = RTK_L3_ICT_IP6UC_PBR_ICMP_REDIR_ACT;

    DIAG_UTIL_ERR_CHK(rtk_l3_intfCtrl_get(unit, *intf_id_ptr, type, (int32 *)&action), ret);

    DIAG_UTIL_MPRINTF("%s icmp redirect-action: %s\n", TOKEN_STR(5), text_l3_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_l3_get_intf_intf_id_pbr_ipuc_ip6uc_icmp_redirect_action */
#endif


