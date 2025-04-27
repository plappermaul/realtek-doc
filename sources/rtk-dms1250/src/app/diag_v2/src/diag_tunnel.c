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
 * Purpose : Definition those TUNNEL command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) tunnel configuration
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
#include <rtk/tunnel.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_tunnel.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
const char text_tunnel_encap1pPriSrc[RTK_TUNNEL_ENCAP_1P_PRI_SRC_END+1][32] =
{
    "Specified value",
    "Passenger",
    "Internal-priority",
    "(unavailable)"
};

const char text_tunnel_encapDscpSrc[RTK_TUNNEL_ENCAP_DSCP_SRC_END+1][32] =
{
    "Specified value",
    "Passenger",
    "Internal-priority remarking",
    "(unavailable)"
};

const char text_tunnel_type[RTK_TUNNEL_TYPE_END+1][32] =
{
    "None",
    "IPv4-in-IPv4",
    "IPv6-in-IPv4",
    "IPv6/4-in-IPv4",
    "IPv4-in-IPv6",
    "IPv6-in-IPv6",
    "IPv6/4-in-IPv6",
    "ISATAP",
    "6to4",
    "6rd",
    "GRE IPv4-in-IPv4",
    "GRE IPv6-in-IPv4",
    "GRE IPv6/4-in-IPv4",
    "GRE IPv4-in-IPv6",
    "GRE IPv6-in-IPv6",
    "GRE IPv6/4-in-IPv6",
    "VXLAN-in-IPv4",
    "VXLAN-in-IPv6",
    "VXLAN-GPE-in-IPv4",
    "VXLAN-GPE-in-IPv6",
    "(unavailable)"
};


/*
 * Macro Declaration
 */
#define DIAG_UTIL_MPRINTF   diag_util_mprintf



#ifdef CMD_TUNNEL_GET_INFO
/*
 * tunnel get info
 */
cparser_result_t
cparser_cmd_tunnel_get_info(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_info_t tunnelInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_tunnel_info_get(unit, &tunnelInfo), ret);

    DIAG_UTIL_MPRINTF("\n Tunnel Capability\n");
    DIAG_UTIL_MPRINTF("---------------------\n");
    DIAG_UTIL_MPRINTF("Decapsulation Interface Entries: %u / %u (Used/Max)\n", \
        tunnelInfo.tunnel_decap_used, tunnelInfo.tunnel_decap_max);
    DIAG_UTIL_MPRINTF("Encapsulation Interface Entries: %u / %u (Used/Max)\n", \
        tunnelInfo.tunnel_encap_used, tunnelInfo.tunnel_encap_max);
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_info */
#endif

#ifdef CMD_TUNNEL_CREATE_INTF_TYPE_IP_IN_IP_IP6_IN_IP_IPANY_IN_IP_IP_IN_IP6_IP6_IN_IP6_IPANY_IN_IP6_ISATAP_6TO4_6RD_GRE_IP_IN_IP_GRE_IP6_IN_IP_GRE_IPANY_IN_IP_GRE_IP_IN_IP6_GRE_IP6_IN_IP6_GRE_IPANY_IN_IP6_VXLAN_IN_IP_VXLAN_IN_IP6_VXLAN_GPE_IN_IP_VXLAN_GPE_IN_IP6
/*
 * tunnel create intf type ( ip-in-ip | ip6-in-ip | ipany-in-ip | ip-in-ip6 | ip6-in-ip6 | ipany-in-ip6 | isatap | 6to4 | 6rd | gre-ip-in-ip | gre-ip6-in-ip | gre-ipany-in-ip | gre-ip-in-ip6 | gre-ip6-in-ip6 | gre-ipany-in-ip6 | vxlan-in-ip | vxlan-in-ip6 | vxlan-gpe-in-ip | vxlan-gpe-in-ip6 )
 */
cparser_result_t
cparser_cmd_tunnel_create_intf_type_ip_in_ip_ip6_in_ip_ipany_in_ip_ip_in_ip6_ip6_in_ip6_ipany_in_ip6_isatap_6to4_6rd_gre_ip_in_ip_gre_ip6_in_ip_gre_ipany_in_ip_gre_ip_in_ip6_gre_ip6_in_ip6_gre_ipany_in_ip6_vxlan_in_ip_vxlan_in_ip6_vxlan_gpe_in_ip_vxlan_gpe_in_ip6(
    cparser_context_t *context)

{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);

    if (0 == osal_strcmp(TOKEN_STR(4), "ip-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IP_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "ip6-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IP6_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "ipany-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IPANY_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "ip-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IP_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "ip6-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IP6_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "ipany-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IPANY_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "isatap"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_ISATAP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "6to4"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_6TO4;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "6rd"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_6RD;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "gre-ip-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "gre-ip6-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP6_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "gre-ipany-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "gre-ip-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "gre-ip6-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "gre-ipany-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "vxlan-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_VXLAN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "vxlan-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_VXLAN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "vxlan-gpe-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_VXLAN_GPE_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(4), "vxlan-gpe-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_VXLAN_GPE_IP6;
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_create(unit, &tunnelIntf), ret);

    DIAG_UTIL_MPRINTF("interface ID: %u (0x%X)\n", tunnelIntf.intf_id, tunnelIntf.intf_id);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_create_intf_type_ip_in_ip_ip6_in_ip_ipany_in_ip_ip_in_ip6_ip6_in_ip6_ipany_in_ip6_isatap_6to4_6rd_gre_ip_in_ip_gre_ip6_in_ip_gre_ipany_in_ip_gre_ip_in_ip6_gre_ip6_in_ip6_gre_ipany_in_ip6_vxlan_in_ip_vxlan_in_ip6_vxlan_gpe_in_ip_vxlan_gpe_in_ip6 */
#endif


#ifdef CMD_TUNNEL_CREATE_INTF_INTF_ID_TYPE_IP_IN_IP_IP6_IN_IP_IPANY_IN_IP_IP_IN_IP6_IP6_IN_IP6_IPANY_IN_IP6_ISATAP_6TO4_6RD_GRE_IP_IN_IP_GRE_IP6_IN_IP_GRE_IPANY_IN_IP_GRE_IP_IN_IP6_GRE_IP6_IN_IP6_GRE_IPANY_IN_IP6
/*
 * tunnel create intf <UINT:intf_id> type ( ip-in-ip | ip6-in-ip | ipany-in-ip | ip-in-ip6 | ip6-in-ip6 | ipany-in-ip6 | isatap | 6to4 | 6rd | gre-ip-in-ip | gre-ip6-in-ip | gre-ipany-in-ip | gre-ip-in-ip6 | gre-ip6-in-ip6 | gre-ipany-in-ip6 )
 */
cparser_result_t
cparser_cmd_tunnel_create_intf_intf_id_type_ip_in_ip_ip6_in_ip_ipany_in_ip_ip_in_ip6_ip6_in_ip6_ipany_in_ip6_isatap_6to4_6rd_gre_ip_in_ip_gre_ip6_in_ip_gre_ipany_in_ip_gre_ip_in_ip6_gre_ip6_in_ip6_gre_ipany_in_ip6(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);

    if (0 == osal_strcmp(TOKEN_STR(5), "ip-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IP_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "ip6-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IP6_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "ipany-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IPANY_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "ip-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IP_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "ip6-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IP6_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "ipany-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_IPANY_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "isatap"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_ISATAP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "6to4"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_6TO4;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "6rd"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_6RD;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ip-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ip6-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP6_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ipany-in-ip"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ip-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ip6-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ipany-in-ip6"))
    {
        tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6;
    }

    /* to specify L3 interface ID */
    tunnelIntf.flags |= RTK_TUNNEL_FLAG_WITH_L3_INTF_ID;
    tunnelIntf.intf_id = *intf_id_ptr;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_create(unit, &tunnelIntf), ret);

    DIAG_UTIL_MPRINTF("interface ID: %u (0x%X)\n", tunnelIntf.intf_id, tunnelIntf.intf_id);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_create_intf_intf_id_type_ip_in_ip_ip6_in_ip_ipany_in_ip_ip_in_ip6_ip6_in_ip6_ipany_in_ip6_isatap_6to4_6rd_gre_ip_in_ip_gre_ip6_in_ip_gre_ipany_in_ip_gre_ip_in_ip6_gre_ip6_in_ip6_gre_ipany_in_ip6 */
#endif


#ifdef CMD_TUNNEL_DESTROY_INTF_INTF_ID_ALL
/*
 * tunnel destroy intf ( <UINT:intf_id> | all )
 */
cparser_result_t
cparser_cmd_tunnel_destroy_intf_intf_id_all(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('a' == TOKEN_CHAR(3, 0))
    {
        /* delete all tunnel interfaces */
        DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_destroyAll(unit), ret);
    }
    else
    {
        /* delete a specified tunnel interface */
        DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_destroy(unit, *intf_id_ptr), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_destroy_intf_intf_id_all */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_TYPE_IP_IN_IP_IP6_IN_IP_IPANY_IN_IP_IP_IN_IP6_IP6_IN_IP6_IPANY_IN_IP6_ISATAP_6TO4_6RD_GRE_IP_IN_IP_GRE_IP6_IN_IP_GRE_IPANY_IN_IP_GRE_IP_IN_IP6_GRE_IP6_IN_IP6_GRE_IPANY_IN_IP6_VXLAN_IN_IP_VXLAN_IN_IP6_VXLAN_GPE_IN_IP_VXLAN_GPE_IN_IP6
/*
 * tunnel set intf <UINT:intf_id> type ( ip-in-ip | ip6-in-ip | ipany-in-ip | ip-in-ip6 | ip6-in-ip6 | ipany-in-ip6 | isatap | 6to4 | 6rd | gre-ip-in-ip | gre-ip6-in-ip | gre-ipany-in-ip | gre-ip-in-ip6 | gre-ip6-in-ip6 | gre-ipany-in-ip6 | vxlan-in-ip | vxlan-in-ip6 | vxlan-gpe-in-ip | vxlan-gpe-in-ip6 )
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_type_ip_in_ip_ip6_in_ip_ipany_in_ip_ip_in_ip6_ip6_in_ip6_ipany_in_ip6_isatap_6to4_6rd_gre_ip_in_ip_gre_ip6_in_ip_gre_ipany_in_ip_gre_ip_in_ip6_gre_ip6_in_ip6_gre_ipany_in_ip6_vxlan_in_ip_vxlan_in_ip6_vxlan_gpe_in_ip_vxlan_gpe_in_ip6(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (0 == osal_strcmp(TOKEN_STR(5), "ip-in-ip"))
    {
        if (RTK_TUNNEL_TYPE_IP_IN_IP != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_IP_IN_IP;
            osal_memset(&tunnelIntf.ip_in_ip.ip.local, 0x00, sizeof(rtk_ip_addr_t));
            osal_memset(&tunnelIntf.ip_in_ip.ip.remote, 0x00, sizeof(rtk_ip_addr_t));
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "ip6-in-ip"))
    {
        if (RTK_TUNNEL_TYPE_IP6_IN_IP != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_IP6_IN_IP;
            osal_memset(&tunnelIntf.ip6_in_ip.ip.local, 0x00, sizeof(rtk_ip_addr_t));
            osal_memset(&tunnelIntf.ip6_in_ip.ip.remote, 0x00, sizeof(rtk_ip_addr_t));
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "ipany-in-ip"))
    {
        if (RTK_TUNNEL_TYPE_IPANY_IN_IP != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_IPANY_IN_IP;
            osal_memset(&tunnelIntf.ipany_in_ip.ip.local, 0x00, sizeof(rtk_ip_addr_t));
            osal_memset(&tunnelIntf.ipany_in_ip.ip.remote, 0x00, sizeof(rtk_ip_addr_t));
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "ip-in-ip6"))
    {
        if (RTK_TUNNEL_TYPE_IP_IN_IP6 != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_IP_IN_IP6;
            osal_memset(&tunnelIntf.ip_in_ip6.ip6.local, 0x00, sizeof(rtk_ipv6_addr_t));
            osal_memset(&tunnelIntf.ip_in_ip6.ip6.remote, 0x00, sizeof(rtk_ipv6_addr_t));
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "ip6-in-ip6"))
    {
        if (RTK_TUNNEL_TYPE_IP6_IN_IP6 != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_IP6_IN_IP6;
            osal_memset(&tunnelIntf.ip6_in_ip6.ip6.local, 0x00, sizeof(rtk_ipv6_addr_t));
            osal_memset(&tunnelIntf.ip6_in_ip6.ip6.remote, 0x00, sizeof(rtk_ipv6_addr_t));
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "ipany-in-ip6"))
    {
        if (RTK_TUNNEL_TYPE_IPANY_IN_IP6 != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_IPANY_IN_IP6;
            osal_memset(&tunnelIntf.ipany_in_ip6.ip6.local, 0x00, sizeof(rtk_ipv6_addr_t));
            osal_memset(&tunnelIntf.ipany_in_ip6.ip6.remote, 0x00, sizeof(rtk_ipv6_addr_t));
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "isatap"))
    {
        if (RTK_TUNNEL_TYPE_ISATAP != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_ISATAP;
            osal_memset(&tunnelIntf.isatap.ip.local, 0x00, sizeof(rtk_ip_addr_t));
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "6to4"))
    {
        if (RTK_TUNNEL_TYPE_6TO4 != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_6TO4;
            osal_memset(&tunnelIntf.ip_6to4.ip.local, 0x00, sizeof(rtk_ip_addr_t));
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "6rd"))
    {
        if (RTK_TUNNEL_TYPE_6RD != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_6RD;
            osal_memset(&tunnelIntf.ip_6rd.ip.local, 0x00, sizeof(rtk_ip_addr_t));
            tunnelIntf.ip_6rd.ip.prefix_len = 0;
            osal_memset(&tunnelIntf.ip_6rd.ip6.prefix, 0x00, sizeof(rtk_ipv6_addr_t));
            tunnelIntf.ip_6rd.ip6.prefix_len = 0;
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ip-in-ip"))
    {
        if (RTK_TUNNEL_TYPE_GRE_IP_IN_IP != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP_IN_IP;
            osal_memset(&tunnelIntf.gre_ip_in_ip.ip.local, 0x00, sizeof(rtk_ip_addr_t));
            osal_memset(&tunnelIntf.gre_ip_in_ip.ip.remote, 0x00, sizeof(rtk_ip_addr_t));
            tunnelIntf.gre_ip_in_ip.key = 0;
            tunnelIntf.gre_ip_in_ip.key_en = DISABLED;
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ip6-in-ip"))
    {
        if (RTK_TUNNEL_TYPE_GRE_IP6_IN_IP != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP6_IN_IP;
            osal_memset(&tunnelIntf.gre_ip6_in_ip.ip.local, 0x00, sizeof(rtk_ip_addr_t));
            osal_memset(&tunnelIntf.gre_ip6_in_ip.ip.remote, 0x00, sizeof(rtk_ip_addr_t));
            tunnelIntf.gre_ip_in_ip.key = 0;
            tunnelIntf.gre_ip_in_ip.key_en = DISABLED;
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ipany-in-ip"))
    {
        if (RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP;
            osal_memset(&tunnelIntf.gre_ipany_in_ip.ip.local, 0x00, sizeof(rtk_ip_addr_t));
            osal_memset(&tunnelIntf.gre_ipany_in_ip.ip.remote, 0x00, sizeof(rtk_ip_addr_t));
            tunnelIntf.gre_ip_in_ip.key = 0;
            tunnelIntf.gre_ip_in_ip.key_en = DISABLED;
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ip-in-ip6"))
    {
        if (RTK_TUNNEL_TYPE_GRE_IP_IN_IP6 != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP_IN_IP6;
            osal_memset(&tunnelIntf.gre_ip_in_ip6.ip6.local, 0x00, sizeof(rtk_ipv6_addr_t));
            osal_memset(&tunnelIntf.gre_ip_in_ip6.ip6.remote, 0x00, sizeof(rtk_ipv6_addr_t));
            tunnelIntf.gre_ip_in_ip.key = 0;
            tunnelIntf.gre_ip_in_ip.key_en = DISABLED;
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ip6-in-ip6"))
    {
        if (RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6 != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6;
            osal_memset(&tunnelIntf.gre_ip6_in_ip6.ip6.local, 0x00, sizeof(rtk_ipv6_addr_t));
            osal_memset(&tunnelIntf.gre_ip6_in_ip6.ip6.remote, 0x00, sizeof(rtk_ipv6_addr_t));
            tunnelIntf.gre_ip_in_ip.key = 0;
            tunnelIntf.gre_ip_in_ip.key_en = DISABLED;
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "gre-ipany-in-ip6"))
    {
        if (RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6 != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6;
            osal_memset(&tunnelIntf.gre_ipany_in_ip6.ip6.local, 0x00, sizeof(rtk_ipv6_addr_t));
            osal_memset(&tunnelIntf.gre_ipany_in_ip6.ip6.remote, 0x00, sizeof(rtk_ipv6_addr_t));
            tunnelIntf.gre_ip_in_ip.key = 0;
            tunnelIntf.gre_ip_in_ip.key_en = DISABLED;
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "vxlan-in-ip"))
    {
        if (RTK_TUNNEL_TYPE_VXLAN_IP != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_VXLAN_IP;
            osal_memset(&tunnelIntf.vxlan_ip.ip.local, 0x00, sizeof(rtk_ip_addr_t));
            osal_memset(&tunnelIntf.vxlan_ip.ip.remote, 0x00, sizeof(rtk_ip_addr_t));
            tunnelIntf.vxlan_ip.l4.remote_port = 0;
            tunnelIntf.vxlan_ip.vni = 0;
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "vxlan-in-ip6"))
    {
        if (RTK_TUNNEL_TYPE_VXLAN_IP6 != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_VXLAN_IP6;
            osal_memset(&tunnelIntf.vxlan_ip6.ip6.local, 0x00, sizeof(rtk_ipv6_addr_t));
            osal_memset(&tunnelIntf.vxlan_ip6.ip6.remote, 0x00, sizeof(rtk_ipv6_addr_t));
            tunnelIntf.vxlan_ip6.l4.remote_port = 0;
            tunnelIntf.vxlan_ip6.vni = 0;
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "vxlan-gpe-in-ip"))
    {
        if (RTK_TUNNEL_TYPE_VXLAN_IP != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_VXLAN_GPE_IP;
            osal_memset(&tunnelIntf.vxlan_ip.ip.local, 0x00, sizeof(rtk_ip_addr_t));
            osal_memset(&tunnelIntf.vxlan_ip.ip.remote, 0x00, sizeof(rtk_ip_addr_t));
            tunnelIntf.vxlan_ip.l4.remote_port = 0;
            tunnelIntf.vxlan_ip.vni = 0;
        }
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "vxlan-gpe-in-ip6"))
    {
        if (RTK_TUNNEL_TYPE_VXLAN_IP6 != tunnelIntf.type)
        {
            tunnelIntf.type = RTK_TUNNEL_TYPE_VXLAN_GPE_IP6;
            osal_memset(&tunnelIntf.vxlan_ip6.ip6.local, 0x00, sizeof(rtk_ipv6_addr_t));
            osal_memset(&tunnelIntf.vxlan_ip6.ip6.remote, 0x00, sizeof(rtk_ipv6_addr_t));
            tunnelIntf.vxlan_ip6.l4.remote_port = 0;
            tunnelIntf.vxlan_ip6.vni = 0;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_type_ip_in_ip_ip6_in_ip_ipany_in_ip_ip_in_ip6_ip6_in_ip6_ipany_in_ip6_isatap_6to4_6rd_gre_ip_in_ip_gre_ip6_in_ip_gre_ipany_in_ip_gre_ip_in_ip6_gre_ip6_in_ip6_gre_ipany_in_ip6_vxlan_in_ip_vxlan_in_ip6_vxlan_gpe_in_ip_vxlan_gpe_in_ip6 */
#endif


#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_L3_VRF_ID_VRF
/*
 * tunnel set intf <UINT:intf_id> l3-vrf-id <UINT:vrf>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_l3_vrf_id_vrf(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vrf_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    tunnelIntf.vrf_id = *vrf_ptr;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_l3_vrf_id_vrf */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_DECAP_CONFIG_PRIORITY_SELECT_GROUP_INDEX_INDEX
/*
 * tunnel set intf <UINT:intf_id> decap-config priority-select-group-index <UINT:index>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_decap_config_priority_select_group_index_index(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    tunnelIntf.decap.priSelGrp_idx = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_decap_config_priority_select_group_index_index */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_DECAP_CONFIG_INTERNAL_PRIORITY_PRIORITY
/*
 * tunnel set intf <UINT:intf_id> decap-config internal-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_decap_config_internal_priority_priority(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    tunnelIntf.decap.int_pri = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_decap_config_internal_priority_priority */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_DECAP_CONFIG_SA_LEARNING_STATE_ENABLE_DISABLE
/*
 * tunnel set intf <UINT:intf_id> decap-config sa-learning state ( enable | disable )
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_decap_config_sa_learning_state_enable_disable(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    //int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_MPRINTF("debug\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_decap_config_sa_learning_state_enable_disable */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_ENCAP_CONFIG_TTL_TTL
/*
 * tunnel set intf <UINT:intf_id> encap-config ttl <UINT:ttl>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_encap_config_ttl_ttl(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *ttl_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    tunnelIntf.encap.ttl = *ttl_ptr;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_encap_config_ttl_ttl */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_ENCAP_CONFIG_QOS_PROFILE_INDEX_INDEX
/*
 * tunnel set intf <UINT:intf_id> encap-config qos-profile-index <UINT:index>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_encap_config_qos_profile_index_index(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    tunnelIntf.encap.qosProfile_idx = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_encap_config_qos_profile_index_index */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_ENCAP_CONFIG_IP4_DONT_FRAG_BIT_CLEAR_SET
/*
 * tunnel set intf <UINT:intf_id> encap-config ip4 dont-frag-bit ( clear | set )
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_encap_config_ip4_dont_frag_bit_clear_set(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);
    tunnelIntf.encap.dontFrag_en = ('c' == TOKEN_CHAR(7, 0))? DISABLED : ENABLED;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_encap_config_ip4_dont_frag_bit_clear_set */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_ENCAP_CONFIG_IP6_FLOW_LABEL_FLOW_LABEL
/*
 * tunnel set intf <UINT:intf_id> encap-config ip6 flow-label <UINT:flow_label>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_encap_config_ip6_flow_label_flow_label(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *flow_label_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);
    tunnelIntf.encap.flow_label = *flow_label_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_encap_config_ip6_flow_label_flow_label */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_ENCAP_CONFIG_FORWARD_VLAN_SELECT_INNER_OUTER
/*
 * tunnel set intf <UINT:intf_id> encap-config forward-vlan-select ( inner | outer )
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_encap_config_forward_vlan_select_inner_outer(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);
    tunnelIntf.encap.fvid_select =  ('i' == TOKEN_CHAR(6, 0))? 0 : 1;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_encap_config_forward_vlan_select_inner_outer */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_ENCAP_CONFIG_DOUBLE_TAG_VID_VID
/*
 * tunnel set intf <UINT:intf_id> encap-config double-tag-vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_encap_config_double_tag_vid_vid(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vid_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);
    tunnelIntf.encap.dbl_tag_vid = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_encap_config_double_tag_vid_vid */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_ENCAP_CONFIG_VLAN_TAG_INNER_OUTER_TPID_IDX_TPID_IDX
/*
 * tunnel set intf <UINT:intf_id> encap-config vlan-tag ( inner | outer ) tpid-idx <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_encap_config_vlan_tag_inner_outer_tpid_idx_tpid_idx(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);
    if ('i' == TOKEN_CHAR(6, 0))
        tunnelIntf.encap.vlan_inner_tag_tpid_idx = *tpid_idx_ptr;
    else /* outer */
        tunnelIntf.encap.vlan_outer_tag_tpid_idx = *tpid_idx_ptr;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_encap_config_vlan_tag_inner_outer_tpid_idx_tpid_idx */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_ENCAP_CONFIG_PE_TAG_ECID_ECID_EXT_ECID_BASE
/*
 * tunnel set intf <UINT:intf_id> encap-config pe-tag ecid <UINT:ecid_ext> <UINT:ecid_base>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_encap_config_pe_tag_ecid_ecid_ext_ecid_base(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *ecid_ext_ptr,
    uint32_t *ecid_base_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);
    tunnelIntf.encap.ecid = ((*ecid_ext_ptr & 0xFF) << 12) | ((*ecid_base_ptr & 0xFFF) << 0);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_encap_config_pe_tag_ecid_ecid_ext_ecid_base */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_FLAG_DECAP_DISABLE_DECAP_USE_CARRIER_TTL_DECAP_USE_CARRIER_DSCP_DECAP_KEEP_PASSENGER_DSCP_STATE_ENABLE_DISABLE
/*
 * tunnel set intf <UINT:intf_id> flag ( decap-disable | decap-use-carrier-ttl | decap-use-carrier-dscp | decap-keep-passenger-dscp ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_flag_decap_disable_decap_use_carrier_ttl_decap_use_carrier_dscp_decap_keep_passenger_dscp_state_enable_disable(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;
    rtk_enable_t state = RTK_ENABLE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(7, state);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (ENABLED == state)
    {
        if ('d' == TOKEN_CHAR(5, 6))
        {
            tunnelIntf.flags |= RTK_TUNNEL_FLAG_DECAP_DISABLE;
        }
        else if ('k' == TOKEN_CHAR(5, 6))
        {
            tunnelIntf.flags |= RTK_TUNNEL_FLAG_DECAP_KEEP_PASSENGER_DSCP;
        }
        else if ('t' == TOKEN_CHAR(5, 18))
        {
            tunnelIntf.flags |= RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_TTL;
        }
        else if ('d' == TOKEN_CHAR(5, 18))
        {
            tunnelIntf.flags |= RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_DSCP;
        }
    }
    else
    {
        if ('d' == TOKEN_CHAR(5, 6))
        {
            tunnelIntf.flags &= ~(RTK_TUNNEL_FLAG_DECAP_DISABLE);
        }
        else if ('k' == TOKEN_CHAR(5, 6))
        {
            tunnelIntf.flags &= ~(RTK_TUNNEL_FLAG_DECAP_KEEP_PASSENGER_DSCP);
        }
        else if ('t' == TOKEN_CHAR(5, 18))
        {
            tunnelIntf.flags &= ~(RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_TTL);
        }
        else if ('d' == TOKEN_CHAR(5, 18))
        {
            tunnelIntf.flags &= ~(RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_DSCP);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_flag_decap_disable_decap_use_carrier_ttl_decap_use_carrier_dscp_decap_keep_passenger_dscp_state_enable_disable */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_FLAG_ENCAP_DISABLE_ENCAP_TTL_DEC_IGNORE_ENCAP_TTL_ASSGIN_ENCAP_DONT_FRAG_INHERIT_STATE_ENABLE_DISABLE
/*
 * tunnel set intf <UINT:intf_id> flag ( encap-disable | encap-ttl-dec-ignore | encap-ttl-assgin | encap-dont-frag-inherit ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_flag_encap_disable_encap_ttl_dec_ignore_encap_ttl_assgin_encap_dont_frag_inherit_state_enable_disable(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;
    rtk_enable_t state = RTK_ENABLE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(7, state);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (ENABLED == state)
    {
        if ('i' == TOKEN_CHAR(5, 7))
        {
            tunnelIntf.flags |= RTK_TUNNEL_FLAG_ENCAP_DISABLE;
        }
        else if ('d' == TOKEN_CHAR(5, 10))
        {
            tunnelIntf.flags |= RTK_TUNNEL_FLAG_ENCAP_TTL_DEC_IGNORE;
        }
        else if ('a' == TOKEN_CHAR(5, 10))
        {
            tunnelIntf.flags |= RTK_TUNNEL_FLAG_ENCAP_TTL_ASSIGN;
        }
        else if ('f' == TOKEN_CHAR(5, 11))
        {
            tunnelIntf.flags |= RTK_TUNNEL_FLAG_ENCAP_DONT_FRAG_INHERIT;
        }
    }
    else
    {
        if ('i' == TOKEN_CHAR(5, 7))
        {
            tunnelIntf.flags &= ~(RTK_TUNNEL_FLAG_ENCAP_DISABLE);
        }
        else if ('d' == TOKEN_CHAR(5, 10))
        {
            tunnelIntf.flags &= ~(RTK_TUNNEL_FLAG_ENCAP_TTL_DEC_IGNORE);
        }
        else if ('a' == TOKEN_CHAR(5, 10))
        {
            tunnelIntf.flags &= ~(RTK_TUNNEL_FLAG_ENCAP_TTL_ASSIGN);
        }
        else if ('f' == TOKEN_CHAR(5, 11))
        {
            tunnelIntf.flags &= ~(RTK_TUNNEL_FLAG_ENCAP_DONT_FRAG_INHERIT);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_flag_encap_disable_encap_ttl_dec_ignore_encap_ttl_assgin_encap_dont_frag_inherit_state_enable_disable */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_LOCAL_IP_IP
/*
 * tunnel set intf <UINT:intf_id> local-ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_local_ip_ip(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *ip_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (RTK_TUNNEL_TYPE_IP_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.ip_in_ip.ip.local = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_IP6_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.ip6_in_ip.ip.local = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_IPANY_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.ipany_in_ip.ip.local = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_ISATAP == tunnelIntf.type)
    {
        tunnelIntf.isatap.ip.local = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_6TO4 == tunnelIntf.type)
    {
        tunnelIntf.ip_6to4.ip.local = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_6RD == tunnelIntf.type)
    {
        tunnelIntf.ip_6rd.ip.local = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.gre_ip_in_ip.ip.local = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP6_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.gre_ip6_in_ip.ip.local = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.gre_ipany_in_ip.ip.local = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_IP == tunnelIntf.type)
    {
        tunnelIntf.vxlan_ip.ip.local = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_GPE_IP == tunnelIntf.type)
    {
        tunnelIntf.vxlan_gpe_ip.ip.local = *ip_ptr;
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support for %s tunnel\n", text_tunnel_type[tunnelIntf.type]);
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_local_ip_ip */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_REMOTE_IP_IP
/*
 * tunnel set intf <UINT:intf_id> remote-ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_remote_ip_ip(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *ip_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (RTK_TUNNEL_TYPE_IP_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.ip_in_ip.ip.remote = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_IP6_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.ip6_in_ip.ip.remote = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_IPANY_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.ipany_in_ip.ip.remote = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_6RD == tunnelIntf.type)
    {
        tunnelIntf.ip_6rd.ip.remote = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.gre_ip_in_ip.ip.remote = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP6_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.gre_ip6_in_ip.ip.remote = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.gre_ipany_in_ip.ip.remote = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_IP == tunnelIntf.type)
    {
        tunnelIntf.vxlan_ip.ip.remote = *ip_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_GPE_IP == tunnelIntf.type)
    {
        tunnelIntf.vxlan_gpe_ip.ip.remote = *ip_ptr;
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support for %s tunnel\n", text_tunnel_type[tunnelIntf.type]);
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_remote_ip_ip */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_6RD_IP_PREFIX_LENGTH_IP_LENGTH_IP6_PREFIX_IP6_PREFIX_IP6_PREFIX_LENGTH_IP6_LENGTH
/*
 * tunnel set intf <UINT:intf_id> 6rd ip-prefix-length <UINT:ip_length> ip6-prefix <IPV6ADDR:ip6_prefix> ip6-prefix-length <UINT:ip6_length>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_6rd_ip_prefix_length_ip_length_ip6_prefix_ip6_prefix_ip6_prefix_length_ip6_length(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *ip_length_ptr,
    char **ip6_prefix_ptr,
    uint32_t *ip6_length_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (RTK_TUNNEL_TYPE_6RD == tunnelIntf.type)
    {
        tunnelIntf.ip_6rd.ip.prefix_len = *ip_length_ptr;
        diag_util_str2ipv6(tunnelIntf.ip_6rd.ip6.prefix.octet, *ip6_prefix_ptr);
        tunnelIntf.ip_6rd.ip6.prefix_len = *ip6_length_ptr;
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support for %s tunnel\n", text_tunnel_type[tunnelIntf.type]);
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_6rd_ip_prefix_length_ip_length_ip6_prefix_ip6_prefix_ip6_prefix_length_ip6_length */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_LOCAL_IP6_IP6
/*
 * tunnel set intf <UINT:intf_id> local-ip6 <IPV6ADDR:ip6>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_local_ip6_ip6(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    char **ip6_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (RTK_TUNNEL_TYPE_IP_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.ip_in_ip6.ip6.local.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_IP6_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.ip6_in_ip6.ip6.local.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_IPANY_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.ipany_in_ip6.ip6.local.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.gre_ip_in_ip6.ip6.local.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.gre_ip6_in_ip6.ip6.local.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.gre_ipany_in_ip6.ip6.local.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.vxlan_ip6.ip6.local.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_GPE_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.vxlan_gpe_ip6.ip6.local.octet, *ip6_ptr);
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support for %s tunnel\n", text_tunnel_type[tunnelIntf.type]);
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_local_ip6_ip6 */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_REMOTE_IP6_IP6
/*
 * tunnel set intf <UINT:intf_id> remote-ip6 <IPV6ADDR:ip6>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_remote_ip6_ip6(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    char **ip6_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (RTK_TUNNEL_TYPE_IP_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.ip_in_ip6.ip6.remote.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_IP6_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.ip6_in_ip6.ip6.remote.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_IPANY_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.ipany_in_ip6.ip6.remote.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.gre_ip_in_ip6.ip6.remote.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.gre_ip6_in_ip6.ip6.remote.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.gre_ipany_in_ip6.ip6.remote.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.vxlan_ip6.ip6.remote.octet, *ip6_ptr);
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_GPE_IP6 == tunnelIntf.type)
    {
        diag_util_str2ipv6(tunnelIntf.vxlan_gpe_ip6.ip6.remote.octet, *ip6_ptr);
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support for %s tunnel\n", text_tunnel_type[tunnelIntf.type]);
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_remote_ip6_ip6 */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_LOCAL_PORT_PORT
/*
 * tunnel set intf <UINT:intf_id> local-port <UINT:port>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_local_port_port(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *port_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (RTK_TUNNEL_TYPE_VXLAN_IP == tunnelIntf.type)
    {
        tunnelIntf.vxlan_ip.l4.local_port = *port_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_IP6 == tunnelIntf.type)
    {
        tunnelIntf.vxlan_ip6.l4.local_port = *port_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_GPE_IP == tunnelIntf.type)
    {
        tunnelIntf.vxlan_gpe_ip.l4.local_port = *port_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_GPE_IP6 == tunnelIntf.type)
    {
        tunnelIntf.vxlan_gpe_ip6.l4.local_port = *port_ptr;
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support for %s tunnel\n", text_tunnel_type[tunnelIntf.type]);
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_src_port_port */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_REMOTE_PORT_PORT
/*
 * tunnel set intf <UINT:intf_id> remote-port <UINT:port>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_remote_port_port(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *port_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (RTK_TUNNEL_TYPE_VXLAN_IP == tunnelIntf.type)
    {
        tunnelIntf.vxlan_ip.l4.remote_port = *port_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_IP6 == tunnelIntf.type)
    {
        tunnelIntf.vxlan_ip6.l4.remote_port = *port_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_GPE_IP == tunnelIntf.type)
    {
        tunnelIntf.vxlan_gpe_ip.l4.remote_port = *port_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_GPE_IP6 == tunnelIntf.type)
    {
        tunnelIntf.vxlan_gpe_ip6.l4.remote_port = *port_ptr;
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support for %s tunnel\n", text_tunnel_type[tunnelIntf.type]);
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_remote_port_port */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_PATH_ID_PATH_ID
/*
 * tunnel set intf <UINT:intf_id> path-id <UINT:path_id>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_path_id_path_id(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *path_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_intf_id_t   intfId = *intf_id_ptr;
    rtk_l3_pathId_t pathId = *path_id_ptr;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intfPathId_set(unit, intfId, pathId), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_path_id_path_id */
#endif

#ifdef CMD_TUNNEL_GET_INTF_INTF_ID_PATH_ID
/*
 * tunnel get intf <UINT:intf_id> path-id
 */
cparser_result_t
cparser_cmd_tunnel_get_intf_intf_id_path_id(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_intf_id_t   intfId = *intf_id_ptr;
    rtk_l3_pathId_t pathId;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intfPathId_get(unit, intfId, &pathId), ret);
    if (0 == pathId)
    {
        /* Unavailable */
        DIAG_UTIL_MPRINTF("tunnel path ID: Unavailable\n");
    } else {
        DIAG_UTIL_MPRINTF("tunnel path ID: %d (0x%X)\n", pathId, pathId);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_intf_intf_id_path_id */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_PATH_NH_DMAC_IDX_L3_EGR_INTF_IDX
/*
 * tunnel set intf <UINT:intf_id> path <UINT:nh_dmac_idx> <UINT:l3_egr_intf_idx>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_path_nh_dmac_idx_l3_egr_intf_idx(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *nh_dmac_idx_ptr,
    uint32_t *l3_egr_intf_idx)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_intf_id_t   intfId = *intf_id_ptr;
    uint32  nhDmacIdx = *nh_dmac_idx_ptr;
    uint32  l3EgrIntfIdx = *l3_egr_intf_idx;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intfPath_set(unit, intfId, nhDmacIdx, l3EgrIntfIdx), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_path_nh_dmac_idx_l3_egr_intf_idx */
#endif

#ifdef CMD_TUNNEL_GET_INTF_INTF_ID_PATH
/*
 * tunnel get intf <UINT:intf_id> path
 */
cparser_result_t
cparser_cmd_tunnel_get_intf_intf_id_path(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_intf_id_t   intfId = *intf_id_ptr;
    uint32  nhDmacIdx;
    uint32  l3EgrIntfIdx;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intfPath_get(unit, intfId, &nhDmacIdx, &l3EgrIntfIdx), ret);
    DIAG_UTIL_MPRINTF("      NH_DMAC_IDX : %d\n", nhDmacIdx);
    DIAG_UTIL_MPRINTF("  L3_EGR_INTF_IDX : %d\n", l3EgrIntfIdx);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_intf_intf_id_path */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_GRE_KEY_KEY_STATE_ENABLE_DISABLE
/*
 * tunnel set intf <UINT:intf_id> gre key <UINT:key> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_gre_key_key_state_enable_disable(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *key_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;
    rtk_enable_t state = RTK_ENABLE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(8, state);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (RTK_TUNNEL_TYPE_GRE_IP_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.gre_ip_in_ip.key = *key_ptr;
        tunnelIntf.gre_ip_in_ip.key_en = state;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP6_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.gre_ip6_in_ip.key = *key_ptr;
        tunnelIntf.gre_ip6_in_ip.key_en = state;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP == tunnelIntf.type)
    {
        tunnelIntf.gre_ipany_in_ip.key = *key_ptr;
        tunnelIntf.gre_ipany_in_ip.key_en = state;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP_IN_IP6 == tunnelIntf.type)
    {
        tunnelIntf.gre_ip_in_ip6.key = *key_ptr;
        tunnelIntf.gre_ip_in_ip6.key_en = state;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6 == tunnelIntf.type)
    {
        tunnelIntf.gre_ip6_in_ip6.key = *key_ptr;
        tunnelIntf.gre_ip6_in_ip6.key_en = state;
    }
    else if (RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6 == tunnelIntf.type)
    {
        tunnelIntf.gre_ipany_in_ip6.key = *key_ptr;
        tunnelIntf.gre_ipany_in_ip6.key_en = state;
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support for %s tunnel\n", text_tunnel_type[tunnelIntf.type]);
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_gre_key_key_state_enable_disable */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_VXLAN_VNI_VNI
/*
 * tunnel set intf <UINT:intf_id> vxlan vni <UINT:vni>
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_vxlan_vni_vni(
    cparser_context_t *context,
    uint32_t *intf_id_ptr,
    uint32_t *vni_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    if (RTK_TUNNEL_TYPE_VXLAN_IP == tunnelIntf.type)
    {
        tunnelIntf.vxlan_ip.vni = *vni_ptr;
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_IP6 == tunnelIntf.type)
    {
        tunnelIntf.vxlan_ip6.vni = *vni_ptr;
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support for %s tunnel\n", text_tunnel_type[tunnelIntf.type]);
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_vxlan_vni_vni */
#endif

#ifdef CMD_TUNNEL_SET_INTF_INTF_ID_VXLAN_VLAN_INNER_TAG_OUTER_TAG_STATUS_TAGGED_UNTAGGED
/*
 * tunnel set intf <UINT:intf_id> vxlan vlan ( inner-tag | outer-tag ) status ( tagged | untagged )
 */
cparser_result_t
cparser_cmd_tunnel_set_intf_intf_id_vxlan_vlan_inner_tag_outer_tag_status_tagged_untagged(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;
    uint32  value;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    value = ('t' == TOKEN_CHAR(8, 0))? 1 : 0;

    if (RTK_TUNNEL_TYPE_VXLAN_IP == tunnelIntf.type)
    {
        if ('i' == TOKEN_CHAR(6, 0))
        {
            tunnelIntf.vxlan_ip.vlan.inner_tag_status = value;
        }
        else if ('o' == TOKEN_CHAR(6, 0))
        {
            tunnelIntf.vxlan_ip.vlan.outer_tag_status = value;
        }
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_IP6 == tunnelIntf.type)
    {
        if ('i' == TOKEN_CHAR(6, 0))
        {
            tunnelIntf.vxlan_ip6.vlan.inner_tag_status = value;
        }
        else if ('o' == TOKEN_CHAR(6, 0))
        {
            tunnelIntf.vxlan_ip6.vlan.outer_tag_status = value;
        }
    }
    else
    {
        DIAG_UTIL_MPRINTF("Not support for %s tunnel\n", text_tunnel_type[tunnelIntf.type]);
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_set(unit, &tunnelIntf), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_intf_intf_id_vxlan_vlan_inner_tag_outer_tag_status_tagged_untagged */
#endif

#ifdef CMD_TUNNEL_GET_INTF_INTF_ID
/*
 * tunnel get intf <UINT:intf_id>
 */
cparser_result_t
cparser_cmd_tunnel_get_intf_intf_id(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_t tunnelIntf;
    char strSIP[128], strDIP[128];

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_t_init(&tunnelIntf), ret);
    tunnelIntf.intf_id = *intf_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intf_get(unit, &tunnelIntf), ret);

    /* display */
    DIAG_UTIL_MPRINTF(" Interface ID : %u (0x%X)\n", tunnelIntf.intf_id, tunnelIntf.intf_id);
    DIAG_UTIL_MPRINTF("       VRF-ID : %u\n", tunnelIntf.vrf_id);
    DIAG_UTIL_MPRINTF("        Flags : 0x%08X\n", tunnelIntf.flags);
    if (tunnelIntf.flags & RTK_TUNNEL_FLAG_REPLACE)
        DIAG_UTIL_MPRINTF("               (0x%08X) REPLACE\n", RTK_TUNNEL_FLAG_REPLACE);
    if (tunnelIntf.flags & RTK_TUNNEL_FLAG_DECAP_DISABLE)
        DIAG_UTIL_MPRINTF("               (0x%08X) DECAP_DISABLE\n", RTK_TUNNEL_FLAG_DECAP_DISABLE);
    if (tunnelIntf.flags & RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_TTL)
        DIAG_UTIL_MPRINTF("               (0x%08X) DECAP_USE_CARRIER_TTL\n", RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_TTL);
    if (tunnelIntf.flags & RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_DSCP)
        DIAG_UTIL_MPRINTF("               (0x%08X) DECAP_USE_CARRIER_DSCP\n", RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_DSCP);
    if (tunnelIntf.flags & RTK_TUNNEL_FLAG_DECAP_KEEP_PASSENGER_DSCP)
        DIAG_UTIL_MPRINTF("               (0x%08X) DECAP_KEEP_PASSENGER_DSCP\n", RTK_TUNNEL_FLAG_DECAP_KEEP_PASSENGER_DSCP);
    if (tunnelIntf.flags & RTK_TUNNEL_FLAG_ENCAP_DISABLE)
        DIAG_UTIL_MPRINTF("               (0x%08X) ENCAP_DISABLE\n", RTK_TUNNEL_FLAG_ENCAP_DISABLE);
    if (tunnelIntf.flags & RTK_TUNNEL_FLAG_ENCAP_TTL_DEC_IGNORE)
        DIAG_UTIL_MPRINTF("               (0x%08X) ENCAP_TTL_DEC_IGNORE\n", RTK_TUNNEL_FLAG_ENCAP_TTL_DEC_IGNORE);
    if (tunnelIntf.flags & RTK_TUNNEL_FLAG_ENCAP_TTL_ASSIGN)
        DIAG_UTIL_MPRINTF("               (0x%08X) ENCAP_TTL_ASSIGN\n", RTK_TUNNEL_FLAG_ENCAP_TTL_ASSIGN);
    if (tunnelIntf.flags & RTK_TUNNEL_FLAG_ENCAP_DONT_FRAG_INHERIT)
        DIAG_UTIL_MPRINTF("               (0x%08X) ENCAP_DONT_FRAG_INHERIT\n", RTK_TUNNEL_FLAG_ENCAP_DONT_FRAG_INHERIT);

    DIAG_UTIL_MPRINTF("  Tunnel Type : %u (%s)\n", tunnelIntf.type, text_tunnel_type[tunnelIntf.type]);
    DIAG_UTIL_MPRINTF("       TTL/HL : %u\n", tunnelIntf.encap.ttl);
    DIAG_UTIL_MPRINTF(" PriSelGrpIdx : %u (Index of Prioirty Selection Group)\n", tunnelIntf.decap.priSelGrp_idx);
    DIAG_UTIL_MPRINTF(" Internal-Pri : %u (Internal Prioirty)\n", tunnelIntf.decap.int_pri);
    DIAG_UTIL_MPRINTF("  NH. Path ID : %u (0x%X)\n", tunnelIntf.encap.path_id, tunnelIntf.encap.path_id);
#if defined(CONFIG_SDK_RTL9310)
    DIAG_UTIL_MPRINTF("    Path Info : NH_DMAC_IDX = %u, L3_EGR_INTF_IDX = %u\n", tunnelIntf.encap.path.nh_dmac_idx, tunnelIntf.encap.path.l3_egr_intf_idx);
#endif
    DIAG_UTIL_MPRINTF("  Don't frag. : %u\n", tunnelIntf.encap.dontFrag_en);
    DIAG_UTIL_MPRINTF("  QoS P/f Idx : %u (Index of QoS Profile)\n", tunnelIntf.encap.qosProfile_idx);

    if (RTK_TUNNEL_TYPE_IP_IN_IP == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.ip_in_ip.ip.local), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strDIP, tunnelIntf.ip_in_ip.ip.remote), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
    }
    else if (RTK_TUNNEL_TYPE_IP6_IN_IP == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.ip6_in_ip.ip.local), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strDIP, tunnelIntf.ip6_in_ip.ip.remote), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
    }
    else if (RTK_TUNNEL_TYPE_IPANY_IN_IP == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.ipany_in_ip.ip.local), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strDIP, tunnelIntf.ipany_in_ip.ip.remote), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
    }
    else if (RTK_TUNNEL_TYPE_IP_IN_IP6 == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strSIP, tunnelIntf.ipany_in_ip6.ip6.local.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strDIP, tunnelIntf.ipany_in_ip6.ip6.remote.octet), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
    }
    else if (RTK_TUNNEL_TYPE_IP6_IN_IP6 == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strSIP, tunnelIntf.ipany_in_ip6.ip6.local.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strDIP, tunnelIntf.ipany_in_ip6.ip6.remote.octet), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
    }
    else if (RTK_TUNNEL_TYPE_IPANY_IN_IP6 == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strSIP, tunnelIntf.ipany_in_ip6.ip6.local.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strDIP, tunnelIntf.ipany_in_ip6.ip6.remote.octet), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
    }
    else if (RTK_TUNNEL_TYPE_ISATAP == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.isatap.ip.local), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
    }
    else if (RTK_TUNNEL_TYPE_6TO4 == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.ip_6to4.ip.local), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
    }
    else if (RTK_TUNNEL_TYPE_6RD == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.ip_6rd.ip.local), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strDIP, tunnelIntf.ip_6rd.ip.remote), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);

        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.ip_6rd.ip.local), ret);
        DIAG_UTIL_MPRINTF("  IPv4 Prefix : %s/%u\n", strSIP, tunnelIntf.ip_6rd.ip.prefix_len);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strSIP, tunnelIntf.ip_6rd.ip6.prefix.octet), ret);
        DIAG_UTIL_MPRINTF("  IPv6 Prefix : %s/%u\n", strSIP, tunnelIntf.ip_6rd.ip6.prefix_len);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP_IN_IP == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.gre_ip_in_ip.ip.local), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strDIP, tunnelIntf.gre_ip_in_ip.ip.remote), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
        DIAG_UTIL_MPRINTF("GRE Key State : %s\n", text_state[tunnelIntf.gre_ip_in_ip.key_en]);
        DIAG_UTIL_MPRINTF("GRE Key Value : %u (0x%08X)\n", \
            tunnelIntf.gre_ip_in_ip.key, tunnelIntf.gre_ip_in_ip.key);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP6_IN_IP == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.gre_ip6_in_ip.ip.local), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strDIP, tunnelIntf.gre_ip6_in_ip.ip.remote), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
        DIAG_UTIL_MPRINTF("GRE Key State : %s\n", text_state[tunnelIntf.gre_ip_in_ip.key_en]);
        DIAG_UTIL_MPRINTF("GRE Key Value : %u (0x%08X)\n", \
            tunnelIntf.gre_ip_in_ip.key, tunnelIntf.gre_ip_in_ip.key);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.gre_ipany_in_ip.ip.local), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strDIP, tunnelIntf.gre_ipany_in_ip.ip.remote), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
        DIAG_UTIL_MPRINTF("GRE Key State : %s\n", text_state[tunnelIntf.gre_ip_in_ip.key_en]);
        DIAG_UTIL_MPRINTF("GRE Key Value : %u (0x%08X)\n", \
            tunnelIntf.gre_ip_in_ip.key, tunnelIntf.gre_ip_in_ip.key);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP_IN_IP6 == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strSIP, tunnelIntf.gre_ip_in_ip6.ip6.local.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strDIP, tunnelIntf.gre_ip_in_ip6.ip6.remote.octet), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
        DIAG_UTIL_MPRINTF("GRE Key State : %s\n", text_state[tunnelIntf.gre_ip_in_ip.key_en]);
        DIAG_UTIL_MPRINTF("GRE Key Value : %u (0x%08X)\n", \
            tunnelIntf.gre_ip_in_ip.key, tunnelIntf.gre_ip_in_ip.key);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6 == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strSIP, tunnelIntf.gre_ip6_in_ip6.ip6.local.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strDIP, tunnelIntf.gre_ip6_in_ip6.ip6.remote.octet), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
        DIAG_UTIL_MPRINTF("GRE Key State : %s\n", text_state[tunnelIntf.gre_ip_in_ip.key_en]);
        DIAG_UTIL_MPRINTF("GRE Key Value : %u (0x%08X)\n", \
            tunnelIntf.gre_ip_in_ip.key, tunnelIntf.gre_ip_in_ip.key);
    }
    else if (RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6 == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strSIP, tunnelIntf.gre_ipany_in_ip6.ip6.local.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strDIP, tunnelIntf.gre_ipany_in_ip6.ip6.remote.octet), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
        DIAG_UTIL_MPRINTF("GRE Key State : %s\n", text_state[tunnelIntf.gre_ip_in_ip.key_en]);
        DIAG_UTIL_MPRINTF("GRE Key Value : %u (0x%08X)\n", \
            tunnelIntf.gre_ip_in_ip.key, tunnelIntf.gre_ip_in_ip.key);
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_IP == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.vxlan_ip.ip.local), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strDIP, tunnelIntf.vxlan_ip.ip.remote), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
        DIAG_UTIL_MPRINTF("   Local-Port : %u (0x%04X)\n", \
            tunnelIntf.vxlan_ip.l4.local_port, \
            tunnelIntf.vxlan_ip.l4.local_port);
        DIAG_UTIL_MPRINTF("  Remote-Port : %u (0x%04X)\n", \
            tunnelIntf.vxlan_ip.l4.remote_port, \
            tunnelIntf.vxlan_ip.l4.remote_port);
        DIAG_UTIL_MPRINTF("   Header/VNI : %u (0x%06X) [Flag: 0x%02X, Next-Protocol: 0x%02X]\n", \
            tunnelIntf.vxlan_ip.vni, tunnelIntf.vxlan_ip.vni, \
            (tunnelIntf.vxlan_ip.vni & (0xFF << 8)) >> 8, \
            (tunnelIntf.vxlan_ip.vni & (0xFF << 0)) >> 0);
        DIAG_UTIL_MPRINTF(" I-Tag Status : %s\n", \
            (tunnelIntf.vxlan_ip.vlan.inner_tag_status)? "Tagged" : "Untagged");
        DIAG_UTIL_MPRINTF(" O-Tag Status : %s\n", \
            (tunnelIntf.vxlan_ip.vlan.outer_tag_status)? "Tagged" : "Untagged");
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_IP6 == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strSIP, tunnelIntf.vxlan_ip6.ip6.local.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strDIP, tunnelIntf.vxlan_ip6.ip6.remote.octet), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
        DIAG_UTIL_MPRINTF("   Local-Port : %u (0x%04X)\n", \
            tunnelIntf.vxlan_ip6.l4.local_port, \
            tunnelIntf.vxlan_ip6.l4.local_port);
        DIAG_UTIL_MPRINTF("  Remote-Port : %u (0x%04X)\n", \
            tunnelIntf.vxlan_ip6.l4.remote_port, \
            tunnelIntf.vxlan_ip6.l4.remote_port);
        DIAG_UTIL_MPRINTF("   Header/VNI : %u (0x%06X) [Flag: 0x%02X, Next-Protocol: 0x%02X]\n", \
            tunnelIntf.vxlan_ip.vni, tunnelIntf.vxlan_ip.vni, \
            (tunnelIntf.vxlan_ip.vni & (0xFF << 8)) >> 8, \
            (tunnelIntf.vxlan_ip.vni & (0xFF << 0)) >> 0);
        DIAG_UTIL_MPRINTF(" I-Tag Status : %s\n", \
            (tunnelIntf.vxlan_ip6.vlan.inner_tag_status)? "Tagged" : "Untagged");
        DIAG_UTIL_MPRINTF(" O-Tag Status : %s\n", \
            (tunnelIntf.vxlan_ip6.vlan.outer_tag_status)? "Tagged" : "Untagged");
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_GPE_IP == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strSIP, tunnelIntf.vxlan_gpe_ip.ip.local), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ip2str(strDIP, tunnelIntf.vxlan_gpe_ip.ip.remote), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
        DIAG_UTIL_MPRINTF("   Local-Port : %u (0x%04X)\n", \
            tunnelIntf.vxlan_gpe_ip.l4.local_port, \
            tunnelIntf.vxlan_gpe_ip.l4.local_port);
        DIAG_UTIL_MPRINTF("  Remote-Port : %u (0x%04X)\n", \
            tunnelIntf.vxlan_gpe_ip.l4.remote_port, \
            tunnelIntf.vxlan_gpe_ip.l4.remote_port);
        DIAG_UTIL_MPRINTF("   Header/VNI : %u (0x%06X) [Flag: 0x%02X, Next-Protocol: 0x%02X]\n", \
            tunnelIntf.vxlan_gpe_ip.vni, tunnelIntf.vxlan_ip.vni, \
            (tunnelIntf.vxlan_gpe_ip.vni & (0xFF << 8)) >> 8, \
            (tunnelIntf.vxlan_gpe_ip.vni & (0xFF << 0)) >> 0);
        DIAG_UTIL_MPRINTF(" I-Tag Status : %s\n", \
            (tunnelIntf.vxlan_gpe_ip.vlan.inner_tag_status)? "Tagged" : "Untagged");
        DIAG_UTIL_MPRINTF(" O-Tag Status : %s\n", \
            (tunnelIntf.vxlan_gpe_ip.vlan.outer_tag_status)? "Tagged" : "Untagged");
    }
    else if (RTK_TUNNEL_TYPE_VXLAN_GPE_IP6 == tunnelIntf.type)
    {
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strSIP, tunnelIntf.vxlan_gpe_ip6.ip6.local.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strDIP, tunnelIntf.vxlan_gpe_ip6.ip6.remote.octet), ret);
        DIAG_UTIL_MPRINTF("     Local-IP : %s\n", strSIP);
        DIAG_UTIL_MPRINTF("    Remote-IP : %s\n", strDIP);
        DIAG_UTIL_MPRINTF("   Local-Port : %u (0x%04X)\n", \
            tunnelIntf.vxlan_gpe_ip6.l4.local_port, \
            tunnelIntf.vxlan_gpe_ip6.l4.local_port);
        DIAG_UTIL_MPRINTF("  Remote-Port : %u (0x%04X)\n", \
            tunnelIntf.vxlan_gpe_ip6.l4.remote_port, \
            tunnelIntf.vxlan_gpe_ip6.l4.remote_port);
        DIAG_UTIL_MPRINTF("   Header/VNI : %u (0x%06X) [Flag: 0x%02X, Next-Protocol: 0x%02X]\n", \
            tunnelIntf.vxlan_gpe_ip6.vni, tunnelIntf.vxlan_ip.vni, \
            (tunnelIntf.vxlan_gpe_ip6.vni & (0xFF << 8)) >> 8, \
            (tunnelIntf.vxlan_gpe_ip6.vni & (0xFF << 0)) >> 0);
        DIAG_UTIL_MPRINTF(" I-Tag Status : %s\n", \
            (tunnelIntf.vxlan_gpe_ip6.vlan.inner_tag_status)? "Tagged" : "Untagged");
        DIAG_UTIL_MPRINTF(" O-Tag Status : %s\n", \
            (tunnelIntf.vxlan_gpe_ip6.vlan.outer_tag_status)? "Tagged" : "Untagged");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_intf_intf_id */
#endif

#ifdef CMD_TUNNEL_GET_INTF_INTF_ID_STATS
/*
 * tunnel get intf <UINT:intf_id> stats
 */
cparser_result_t
cparser_cmd_tunnel_get_intf_intf_id_stats(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_stats_t stats;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intfStats_get(unit, *intf_id_ptr, &stats), ret);

    DIAG_UTIL_MPRINTF("  IN_OCTETS : %llu\n", stats.rx.octets);
    DIAG_UTIL_MPRINTF("    IN_PKTS : %llu\n", stats.rx.pkts);
    DIAG_UTIL_MPRINTF("   IN_DROPS : %llu\n", stats.rx.drops);
    DIAG_UTIL_MPRINTF(" OUT_OCTETS : %llu\n", stats.tx.octets);
    DIAG_UTIL_MPRINTF("   OUT_PKTS : %llu\n", stats.tx.pkts);
    DIAG_UTIL_MPRINTF("  OUT_DROPS : %llu\n", stats.tx.drops);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_intf_intf_id_stats */
#endif

#ifdef CMD_TUNNEL_GET_INTF_INTF_ID_STATS_RX_BYTES_RX_PKTS_RX_DROPS_TX_BYTES_TX_PKTS_TX_DROPS
/*
 * tunnel get intf <UINT:intf_id> stats ( rx-bytes | rx-pkts | rx-drops | tx-bytes | tx-pkts | tx-drops )
 */
cparser_result_t
cparser_cmd_tunnel_get_intf_intf_id_stats_rx_bytes_rx_pkts_rx_drops_tx_bytes_tx_pkts_tx_drops(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_intf_stats_t stats;
    char    *pText = "NULL";
    uint64  value64 = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intfStats_get(unit, *intf_id_ptr, &stats), ret);

    if ('r' == TOKEN_CHAR(5, 0))
    {
        if ('b' == TOKEN_CHAR(5, 3))
        {
            pText = "Rx-Bytes";
            value64 = stats.rx.octets;
        }
        else if ('p' == TOKEN_CHAR(5, 3))
        {
            pText = "Rx-Pkts";
            value64 = stats.rx.pkts;
        }
        else if ('d' == TOKEN_CHAR(5, 3))
        {
            pText = "Rx-Drops";
            value64 = stats.rx.drops;
        }
    } else if ('t' == TOKEN_CHAR(5, 0))
    {
        if ('b' == TOKEN_CHAR(5, 3))
        {
            pText = "Tx-Bytes";
            value64 = stats.tx.octets;
        }
        else if ('p' == TOKEN_CHAR(5, 3))
        {
            pText = "Tx-Pkts";
            value64 = stats.tx.pkts;
        }
        else if ('d' == TOKEN_CHAR(5, 3))
        {
            pText = "Tx-Drops";
            value64 = stats.tx.drops;
        }
    }

    DIAG_UTIL_MPRINTF(" %s : %llu\n", pText, value64);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_intf_intf_id_stats_rx_bytes_rx_pkts_rx_drops_tx_bytes_tx_pkts_tx_drops */
#endif

#ifdef CMD_TUNNEL_RESET_INTF_INTF_ID_STATS
/*
 * tunnel reset intf <UINT:intf_id> stats
 */
cparser_result_t
cparser_cmd_tunnel_reset_intf_intf_id_stats(
    cparser_context_t *context,
    uint32_t *intf_id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_tunnel_intfStats_reset(unit, *intf_id_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_reset_intf_intf_id_stats */
#endif

#ifdef CMD_TUNNEL_SET_QOS_PROFILE_INDEX_INNER_PRIORITY_SOURCE_FORCE_FROM_PASSENGER_FROM_INTERNAL_PRIORITY
/*
 * tunnel set qos-profile <UINT:index> inner-priority-source ( force | from-passenger | from-internal-priority )
 */
cparser_result_t
cparser_cmd_tunnel_set_qos_profile_index_inner_priority_source_force_from_passenger_from_internal_priority(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_qosProfile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_get(unit, *index_ptr, &profile), ret);

    if ('o' == TOKEN_CHAR(5, 1))
        profile.inner_pri_src = RTK_TUNNEL_ENCAP_1P_PRI_SRC_FORCE;
    else if ('p' == TOKEN_CHAR(5, 5))
        profile.inner_pri_src = RTK_TUNNEL_ENCAP_1P_PRI_SRC_PASSENGER;
    else if ('i' == TOKEN_CHAR(5, 5))
        profile.inner_pri_src = RTK_TUNNEL_ENCAP_1P_PRI_SRC_INT_PRI;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_set(unit, *index_ptr, profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_qos_profile_index_inner_priority_source_force_from_passenger_from_internal_priority */
#endif

#ifdef CMD_TUNNEL_SET_QOS_PROFILE_INDEX_INNER_PRIORITY_PRIORITY
/*
 * tunnel set qos-profile <UINT:index> inner-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_tunnel_set_qos_profile_index_inner_priority_priority(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_qosProfile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_get(unit, *index_ptr, &profile), ret);

    profile.inner_pri = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_set(unit, *index_ptr, profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_qos_profile_index_inner_priority_priority */
#endif

#ifdef CMD_TUNNEL_SET_QOS_PROFILE_INDEX_OUTER_PRIORITY_SOURCE_FORCE_FROM_PASSENGER_FROM_INTERNAL_PRIORITY
/*
 * tunnel set qos-profile <UINT:index> outer-priority-source ( force | from-passenger | from-internal-priority )
 */
cparser_result_t
cparser_cmd_tunnel_set_qos_profile_index_outer_priority_source_force_from_passenger_from_internal_priority(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_qosProfile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_get(unit, *index_ptr, &profile), ret);

    if ('o' == TOKEN_CHAR(5, 1))
        profile.outer_pri_src = RTK_TUNNEL_ENCAP_1P_PRI_SRC_FORCE;
    else if ('p' == TOKEN_CHAR(5, 5))
        profile.outer_pri_src = RTK_TUNNEL_ENCAP_1P_PRI_SRC_PASSENGER;
    else if ('i' == TOKEN_CHAR(5, 5))
        profile.outer_pri_src = RTK_TUNNEL_ENCAP_1P_PRI_SRC_INT_PRI;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_set(unit, *index_ptr, profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_qos_profile_index_outer_priority_source_force_from_passenger_from_internal_priority */
#endif

#ifdef CMD_TUNNEL_SET_QOS_PROFILE_INDEX_OUTER_PRIORITY_PRIORITY
/*
 * tunnel set qos-profile <UINT:index> outer-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_tunnel_set_qos_profile_index_outer_priority_priority(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *priority_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_qosProfile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_get(unit, *index_ptr, &profile), ret);

    profile.outer_pri = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_set(unit, *index_ptr, profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_qos_profile_index_outer_priority_priority */
#endif

#ifdef CMD_TUNNEL_SET_QOS_PROFILE_INDEX_DSCP_SOURCE_FORCE_FROM_PASSENGER_FROM_INTERNAL_PRIORITY
/*
 * tunnel set qos-profile <UINT:index> dscp-source ( force | from-passenger | from-internal-priority )
 */
cparser_result_t
cparser_cmd_tunnel_set_qos_profile_index_dscp_source_force_from_passenger_from_internal_priority(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_qosProfile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_get(unit, *index_ptr, &profile), ret);

    if ('o' == TOKEN_CHAR(5, 1))
        profile.dscp_src = RTK_TUNNEL_ENCAP_DSCP_SRC_FORCE;
    else if ('p' == TOKEN_CHAR(5, 5))
        profile.dscp_src = RTK_TUNNEL_ENCAP_DSCP_SRC_PASSENGER;
    else if ('i' == TOKEN_CHAR(5, 5))
        profile.dscp_src = RTK_TUNNEL_ENCAP_DSCP_SRC_INT_PRI_REMARKING;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_set(unit, *index_ptr, profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_qos_profile_index_dscp_source_force_from_passenger_from_internal_priority */
#endif

#ifdef CMD_TUNNEL_SET_QOS_PROFILE_INDEX_DSCP_DSCP
/*
 * tunnel set qos-profile <UINT:index> dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_tunnel_set_qos_profile_index_dscp_dscp(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *dscp_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_qosProfile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_get(unit, *index_ptr, &profile), ret);

    profile.dscp = *dscp_ptr;

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_set(unit, *index_ptr, profile), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_qos_profile_index_dscp_dscp */
#endif

#ifdef CMD_TUNNEL_GET_QOS_PROFILE_INDEX
/*
 * tunnel get qos-profile <UINT:index>
 */
cparser_result_t
cparser_cmd_tunnel_get_qos_profile_index(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_tunnel_qosProfile_t profile;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_tunnel_qosProfile_get(unit, *index_ptr, &profile), ret);

    if (RTK_TUNNEL_ENCAP_1P_PRI_SRC_FORCE == profile.inner_pri_src)
    {
        DIAG_UTIL_MPRINTF("Inner Priority Source: %s (%u)\n", \
            text_tunnel_encap1pPriSrc[profile.inner_pri_src], \
            profile.inner_pri);
    }
    else
    {
        DIAG_UTIL_MPRINTF("Inner Priority Source: %s\n", \
            text_tunnel_encap1pPriSrc[profile.inner_pri_src]);
    }

    if (RTK_TUNNEL_ENCAP_1P_PRI_SRC_FORCE == profile.outer_pri_src)
    {
        DIAG_UTIL_MPRINTF("Outer Priority Source: %s (%u)\n", \
            text_tunnel_encap1pPriSrc[profile.outer_pri_src], \
            profile.outer_pri);
    }
    else
    {
        DIAG_UTIL_MPRINTF("Outer Priority Source: %s\n", \
            text_tunnel_encap1pPriSrc[profile.outer_pri_src]);
    }

    if (RTK_TUNNEL_ENCAP_DSCP_SRC_FORCE == profile.dscp_src)
    {
        DIAG_UTIL_MPRINTF("DSCP Source: %s (%u)\n", \
            text_tunnel_encapDscpSrc[profile.dscp_src], \
            profile.dscp);
    }
    else
    {
        DIAG_UTIL_MPRINTF("DSCP Source: %s\n", \
            text_tunnel_encapDscpSrc[profile.dscp_src]);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_qos_profile_index */
#endif

#ifdef CMD_TUNNEL_SET_DECAP_IP_SIP_FAIL_IP6_SIP_FAIL_ISATAP_SIP_FAIL_6TO4_SIP_FAIL_6TO4_DIP_FAIL_6RD_DIP_FAIL_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * tunnel set decap ( ip-sip-fail | ip6-sip-fail | isatap-sip-fail | 6to4-sip-fail | 6to4-dip-fail | 6rd-dip-fail ) action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_tunnel_set_decap_ip_sip_fail_ip6_sip_fail_isatap_sip_fail_6to4_sip_fail_6to4_dip_fail_6rd_dip_fail_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_tunnel_globalCtrlType_t type = RTK_TUNNEL_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(5, action);

    if ('i' == TOKEN_CHAR(3, 0))
    {
        if ('-' == TOKEN_CHAR(3, 2))
            type = RTK_TUNNEL_GCT_DECAP_IP4_SIP_FAIL_ACT;
        else if ('6' == TOKEN_CHAR(3, 2))
            type = RTK_TUNNEL_GCT_DECAP_IP6_SIP_FAIL_ACT;
        else if ('a' == TOKEN_CHAR(3, 2))
            type = RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_FAIL_ACT;
    }
    else if ('6' == TOKEN_CHAR(3, 0))
    {
        if ('s' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6TO4_SIP_FAIL_ACT;
        else if ('d' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6TO4_DIP_FAIL_ACT;
        else if ('i' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6RD_DIP_FAIL_ACT;
    }

    DIAG_UTIL_MPRINTF("tunnel decap %s action: %s\n", TOKEN_STR(3), text_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_tunnel_globalCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_decap_ip_sip_fail_ip6_sip_fail_isatap_sip_fail_6to4_sip_fail_6to4_dip_fail_6rd_dip_fail_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_TUNNEL_GET_DECAP_IP_SIP_FAIL_IP6_SIP_FAIL_ISATAP_SIP_FAIL_6TO4_SIP_FAIL_6TO4_DIP_FAIL_6RD_DIP_FAIL_ACTION
/*
 * tunnel get decap ( ip-sip-fail | ip6-sip-fail | isatap-sip-fail | 6to4-sip-fail | 6to4-dip-fail | 6rd-dip-fail ) action
 */
cparser_result_t
cparser_cmd_tunnel_get_decap_ip_sip_fail_ip6_sip_fail_isatap_sip_fail_6to4_sip_fail_6to4_dip_fail_6rd_dip_fail_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_tunnel_globalCtrlType_t type = RTK_TUNNEL_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('i' == TOKEN_CHAR(3, 0))
    {
        if ('-' == TOKEN_CHAR(3, 2))
            type = RTK_TUNNEL_GCT_DECAP_IP4_SIP_FAIL_ACT;
        else if ('6' == TOKEN_CHAR(3, 2))
            type = RTK_TUNNEL_GCT_DECAP_IP6_SIP_FAIL_ACT;
        else if ('a' == TOKEN_CHAR(3, 2))
            type = RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_FAIL_ACT;
    }
    else if ('6' == TOKEN_CHAR(3, 0))
    {
        if ('s' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6TO4_SIP_FAIL_ACT;
        else if ('d' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6TO4_DIP_FAIL_ACT;
        else if ('i' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6RD_DIP_FAIL_ACT;
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_globalCtrl_get(unit, type, (int32 *)&action), ret);
    DIAG_UTIL_MPRINTF("tunnel decap %s action: %s\n", TOKEN_STR(3), text_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_decap_ip_sip_fail_ip6_sip_fail_isatap_sip_fail_6to4_sip_fail_6to4_dip_fail_6rd_dip_fail_action */
#endif

#ifdef CMD_TUNNEL_SET_ENCAP_MTU_FAIL_TTL_FAIL_ROUTE_TO_TUNNEL_ACTION_DROP_TRAP_TO_CPU_TRAP_TO_MASTER
/*
 * tunnel set encap ( mtu-fail | ttl-fail | route-to-tunnel ) action ( drop | trap-to-cpu | trap-to-master )
 */
cparser_result_t
cparser_cmd_tunnel_set_encap_mtu_fail_ttl_fail_route_to_tunnel_action_drop_trap_to_cpu_trap_to_master(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_tunnel_globalCtrlType_t type = RTK_TUNNEL_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_L3_ACT(5, action);

    if ('m' == TOKEN_CHAR(3, 0))
    {
        type = RTK_TUNNEL_GCT_ENCAP_MTU_FAIL_ACT;
    }
    else if ('t' == TOKEN_CHAR(3, 0))
    {
        type = RTK_TUNNEL_GCT_ENCAP_TTL_FAIL_ACT;
    }
    else if ('r' == TOKEN_CHAR(3, 0))
    {
        type = RTK_TUNNEL_GCT_ROUTE_TO_TUNNEL_ACT;
    }

    DIAG_UTIL_MPRINTF("tunnel encap %s action: %s\n", TOKEN_STR(3), text_action[action]);
    DIAG_UTIL_ERR_CHK(rtk_tunnel_globalCtrl_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_encap_mtu_fail_ttl_fail_route_to_tunnel_action_drop_trap_to_cpu_trap_to_master */
#endif

#ifdef CMD_TUNNEL_GET_ENCAP_MTU_FAIL_TTL_FAIL_ROUTE_TO_TUNNEL_ACTION
/*
 * tunnel get encap ( mtu-fail | ttl-fail | route-to-tunnel ) action
 */
cparser_result_t
cparser_cmd_tunnel_get_encap_mtu_fail_ttl_fail_route_to_tunnel_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t action = ACTION_END;
    rtk_tunnel_globalCtrlType_t type = RTK_TUNNEL_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('m' == TOKEN_CHAR(3, 0))
    {
        type = RTK_TUNNEL_GCT_ENCAP_MTU_FAIL_ACT;
    }
    else if ('t' == TOKEN_CHAR(3, 0))
    {
        type = RTK_TUNNEL_GCT_ENCAP_TTL_FAIL_ACT;
    }
    else if ('r' == TOKEN_CHAR(3, 0))
    {
        type = RTK_TUNNEL_GCT_ROUTE_TO_TUNNEL_ACT;
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_globalCtrl_get(unit, type, (int32 *)&action), ret);
    DIAG_UTIL_MPRINTF("tunnel encap %s action: %s\n", TOKEN_STR(3), text_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_encap_mtu_fail_ttl_fail_route_to_tunnel_action */
#endif

#ifdef CMD_TUNNEL_SET_DECAP_IP6_SIP_IP4COMPATIBLE_CHECK_IP6_SIP_IP4MAPPED_CHECK_ISATAP_SIP_TYPE_CHECK_ISATAP_SIP_MAPPING_CHECK_6TO4_SIP_CHECK_6TO4_DIP_CHECK_6RD_DIP_CHECK_STATE_ENABLE_DISABLE
/*
 * tunnel set decap ( ip6-sip-ip4compatible-check | ip6-sip-ip4mapped-check | isatap-sip-type-check | isatap-sip-mapping-check | 6to4-sip-check | 6to4-dip-check | 6rd-dip-check ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_tunnel_set_decap_ip6_sip_ip4compatible_check_ip6_sip_ip4mapped_check_isatap_sip_type_check_isatap_sip_mapping_check_6to4_sip_check_6to4_dip_check_6rd_dip_check_state_enable_disable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_tunnel_globalCtrlType_t type = RTK_TUNNEL_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_PARSE_STATE(5, state);

    if ('p' == TOKEN_CHAR(3, 1))
    {
        if ('c' == TOKEN_CHAR(3, 11))
            type = RTK_TUNNEL_GCT_DECAP_IP6_SIP_IP4CMPT_CHK;
        else if ('m' == TOKEN_CHAR(3, 11))
            type = RTK_TUNNEL_GCT_DECAP_IP6_SIP_IP4MAP_CHK;
    }
    else if ('s' == TOKEN_CHAR(3, 1))
    {
        if ('t' == TOKEN_CHAR(3, 11))
            type = RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_TYPE_CHK;
        else if ('m' == TOKEN_CHAR(3, 11))
            type = RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_MAP_CHK;
    }
    else if ('6' == TOKEN_CHAR(3, 0))
    {
        if ('s' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6TO4_SIP_CHK;
        else if ('d' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6TO4_DIP_CHK;
        else if ('i' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6RD_DIP_CHK;
    }

    DIAG_UTIL_MPRINTF("tunnel decap %s state: %s\n", TOKEN_STR(3), text_state[state]);
    DIAG_UTIL_ERR_CHK(rtk_tunnel_globalCtrl_set(unit, type, state), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_decap_ip6_sip_ip4compatible_check_ip6_sip_ip4mapped_check_isatap_sip_type_check_isatap_sip_mapping_check_6to4_sip_check_6to4_dip_check_6rd_dip_check_state_enable_disable */
#endif

#ifdef CMD_TUNNEL_GET_DECAP_IP6_SIP_IP4COMPATIBLE_CHECK_IP6_SIP_IP4MAPPED_CHECK_ISATAP_SIP_TYPE_CHECK_ISATAP_SIP_MAPPING_CHECK_6TO4_SIP_CHECK_6TO4_DIP_CHECK_6RD_DIP_CHECK_STATE
/*
 * tunnel get decap ( ip6-sip-ip4compatible-check | ip6-sip-ip4mapped-check | isatap-sip-type-check | isatap-sip-mapping-check | 6to4-sip-check | 6to4-dip-check | 6rd-dip-check ) state
 */
cparser_result_t
cparser_cmd_tunnel_get_decap_ip6_sip_ip4compatible_check_ip6_sip_ip4mapped_check_isatap_sip_type_check_isatap_sip_mapping_check_6to4_sip_check_6to4_dip_check_6rd_dip_check_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t state = RTK_ENABLE_END;
    rtk_tunnel_globalCtrlType_t type = RTK_TUNNEL_GCT_NONE;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('p' == TOKEN_CHAR(3, 1))
    {
        if ('c' == TOKEN_CHAR(3, 11))
            type = RTK_TUNNEL_GCT_DECAP_IP6_SIP_IP4CMPT_CHK;
        else if ('m' == TOKEN_CHAR(3, 11))
            type = RTK_TUNNEL_GCT_DECAP_IP6_SIP_IP4MAP_CHK;
    }
    else if ('s' == TOKEN_CHAR(3, 1))
    {
        if ('t' == TOKEN_CHAR(3, 11))
            type = RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_TYPE_CHK;
        else if ('m' == TOKEN_CHAR(3, 11))
            type = RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_MAP_CHK;
    }
    else if ('6' == TOKEN_CHAR(3, 0))
    {
        if ('s' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6TO4_SIP_CHK;
        else if ('d' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6TO4_DIP_CHK;
        else if ('i' == TOKEN_CHAR(3, 5))
            type = RTK_TUNNEL_GCT_DECAP_6RD_DIP_CHK;
    }

    DIAG_UTIL_ERR_CHK(rtk_tunnel_globalCtrl_get(unit, type, (int32 *)&state), ret);
    DIAG_UTIL_MPRINTF("tunnel decap %s state: %s\n", TOKEN_STR(3), text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_decap_ip6_sip_ip4compatible_check_ip6_sip_ip4mapped_check_isatap_sip_type_check_isatap_sip_mapping_check_6to4_sip_check_6to4_dip_check_6rd_dip_check_state */
#endif

#ifdef CMD_TUNNEL_SET_ENCAP_IP_HDR_IDENTIFICATION_ID
/*
 * tunnel set encap ip-hdr-identification <UINT:id>
 */
cparser_result_t
cparser_cmd_tunnel_set_encap_ip_hdr_identification_id(
    cparser_context_t *context,
    uint32_t *id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_tunnel_globalCtrl_set(unit, RTK_TUNNEL_GCT_ENCAP_IP_HDR_IDENTIFICATION, *id_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_set_encap_ip_hdr_identification_id */
#endif

#ifdef CMD_TUNNEL_GET_ENCAP_IP_HDR_IDENTIFICATION
/*
 * tunnel get encap ip-hdr-identification
 */
cparser_result_t
cparser_cmd_tunnel_get_encap_ip_hdr_identification(
    cparser_context_t *context)
{
    uint32  unit;
    int32   arg;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_tunnel_globalCtrl_get(unit, RTK_TUNNEL_GCT_ENCAP_IP_HDR_IDENTIFICATION, &arg), ret);
    DIAG_UTIL_MPRINTF("Tunnel Encap IP-Identication: %u (0x%04X)\n", arg, arg);

    return CPARSER_OK;
}   /* end of cparser_cmd_tunnel_get_encap_ip_hdr_identification */
#endif


