/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
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
 * Purpose : Definition those Layer2 command and APIs in the SDK diagnostic shell.
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
#include <rtk/vlan.h>
#include <rtk/bpe.h>
#include <rtk/stack.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <stdio.h>
#include <ioal/mem32.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_l2.h>
  #include <rtrpc/rtrpc_vlan.h>
  #include <rtrpc/rtrpc_bpe.h>
  #include <rtrpc/rtrpc_stack.h>
  #include <rtrpc/rtrpc_diag.h>
#endif

#ifdef CMD_L2_TABLE_ADD_SET_MAC_UCAST_VID_MAC_PORT_TRUNK_ID_SA_BLOCK_DA_BLOCK_STATIC_NEXTHOP_SUSPEND
int32 _l2_nexthop_set(uint32 unit, uint32 index)
{
    int32   ret;
    uint32  data;

    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(ioal_mem32_write(unit, 0x8500, 0x100000 | index), ret);
        DIAG_UTIL_ERR_CHK(ioal_mem32_read(unit, 0x8510, &data), ret);

        data |= 0x40000000;
        DIAG_UTIL_ERR_CHK(ioal_mem32_write(unit, 0x8510, data), ret);
        DIAG_UTIL_ERR_CHK(ioal_mem32_write(unit, 0x8500, 0x180000 | index), ret);
    }
    else if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(ioal_mem32_write(unit, 0xB320, 0x80000 | index), ret);
        DIAG_UTIL_ERR_CHK(ioal_mem32_read(unit, 0xB33c, &data), ret);

        data |= 0x1000;
        DIAG_UTIL_ERR_CHK(ioal_mem32_write(unit, 0xB33c, data), ret);
        DIAG_UTIL_ERR_CHK(ioal_mem32_write(unit, 0xB320, 0xC0000 | index), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_IP6_MCAST_SIP_DIP_VID_VLAN_ID_INDEX_INDEX
/*
 * l2-table add ip6-mcast <IPV6ADDR:sip> <IPV6ADDR:dip> vid <UINT:vlan_id> index <UINT:index>
 */
cparser_result_t cparser_cmd_l2_table_add_ip6_mcast_sip_dip_vid_vlan_id_index_index(cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vlan_id_ptr,
    uint32_t *index_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&ip6_mcast_data, 0, sizeof(rtk_l2_ip6McastAddr_t));

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.sip.octet, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.dip.octet, TOKEN_STR(4)), ret);

    ip6_mcast_data.rvid     = *vlan_id_ptr;
    ip6_mcast_data.fwdIndex = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6McastAddr_addByIndex(unit, &ip6_mcast_data), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_SET_IP6_MCAST_SIP_DIP_VID_VLAN_ID_PORT_ALL
/*
  * l2-table ( add | set ) ip6-mcast <IPV6ADDR:sip> <IPV6ADDR:dip> vid <UINT:vlan_id> ( <PORT_LIST:port> | all )
  */
cparser_result_t cparser_cmd_l2_table_add_set_ip6_mcast_sip_dip_vid_vlan_id_port_all(cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vlan_id_ptr,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&ip6_mcast_data, 0, sizeof(rtk_l2_ip6McastAddr_t));

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.sip.octet, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.dip.octet, TOKEN_STR(4)), ret);
    ip6_mcast_data.rvid = *vlan_id_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7), ret);
    ip6_mcast_data.portmask = portlist.portmask;

    if ('a' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_ip6McastAddr_add(unit, &ip6_mcast_data), ret);
    }
    else if ('s' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_ip6McastAddr_set(unit, &ip6_mcast_data), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_IP6_MCAST_SIP_DIP_VID_VLAN_ID_FWD_INDEX_INDEX
/*
  * l2-table set ip6-mcast <IPV6ADDR:sip> <IPV6ADDR:dip> vid <UINT:vlan_id> fwd_index <UINT:index>
  */
cparser_result_t cparser_cmd_l2_table_set_ip6_mcast_sip_dip_vid_vlan_id_fwd_index_index(cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vlan_id_ptr,
    uint32_t *index_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&ip6_mcast_data, 0, sizeof(rtk_l2_ip6McastAddr_t));

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.sip.octet, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.dip.octet, TOKEN_STR(4)), ret);
    ip6_mcast_data.rvid = *vlan_id_ptr;
    ip6_mcast_data.fwdIndex = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6McastAddr_setByIndex(unit, &ip6_mcast_data), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_L2_TABLE_SET_SECURE_MAC_MODE_DISABLE_ENABLE
/*
  * l2-table set secure-mac-mode ( disable | enable )
  */
cparser_result_t cparser_cmd_l2_table_set_secure_mac_mode_disable_enable(cparser_context_t *context)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(3, 0))
    {
        /* set invalid link down enable */
        DIAG_UTIL_ERR_CHK(rtk_l2_secureMacMode_set(unit, ENABLED), ret);
    }
    else if ('d' == TOKEN_CHAR(3, 0))
    {
        /* set invalid link down disable */
        DIAG_UTIL_ERR_CHK(rtk_l2_secureMacMode_set(unit, DISABLED), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_HASH_ALGORITHM_ALGO0_ALGO1
/*
  * l2-table set hash-algorithm ( algo0 | algo1 )
  */
cparser_result_t cparser_cmd_l2_table_set_hash_algorithm_algo0_algo1(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    uint32                          hashType = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('0' == TOKEN_CHAR(3, 4))
        hashType = 0;
    else if('1' == TOKEN_CHAR(3, 4))
        hashType = 1;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_hashAlgo_set(unit, hashType), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_bucketHashAlgo_set(unit, 0, hashType), ret);
        if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_bucketHashAlgo_set(unit, 1, hashType), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_IP_MCAST_DIP_CHECK_DISABLE_ENABLE
/*
  * l2-table set ip-mcast dip-check ( disable | enable )
  */
cparser_result_t cparser_cmd_l2_table_set_ip_mcast_dip_check_disable_enable(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('d' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddrChkEnable_set(unit, DISABLED), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddrChkEnable_set(unit, ENABLED), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_IP_MCAST_VLAN_COMPARE_DISABLE_ENABLE
/*
  * l2-table set ip-mcast vlan-compare ( disable | enable )
  */
cparser_result_t cparser_cmd_l2_table_set_ip_mcast_vlan_compare_disable_enable(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('d' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_ipMcstFidVidCompareEnable_set(unit, DISABLED), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_ipMcstFidVidCompareEnable_set(unit, ENABLED), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_VLAN_FWD_MODE_PORT_ALL_INNER_VID_BASED_OUTER_VID_BASED
/*
  * l2-table set vlan-fwd-mode ( <PORT_LIST:port> | all ) ( inner-vid-based | outer-vid-based )
  */
cparser_result_t cparser_cmd_l2_table_set_vlan_fwd_mode_port_all_inner_vid_based_outer_vid_based(cparser_context_t *context,
    char **port_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_port_t                      port = 0;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('i' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_vlanMode_set(unit, port, BASED_ON_INNER_VLAN), ret);
        }

    }
    else if('o' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_vlanMode_set(unit, port, BASED_ON_OUTER_VLAN), ret);
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_MAC_UCAST_PORT_PORT_ID_INCLUDE_STATIC
/*
  *  l2-table set flush mac-ucast port <UINT:port_id> { include-static }
  */
cparser_result_t cparser_cmd_l2_table_set_flush_mac_ucast_port_port_id_include_static(cparser_context_t *context,
    uint32_t *port_id_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t   config;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == port_id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    config.act = 0;
#endif
    config.flushByPort = TRUE;
    config.portOrTrunk = TRUE; /* port-based */
    config.port = *port_id_ptr;

    if('i' == TOKEN_CHAR(6, 0))
        config.flushStaticAddr = TRUE;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_MAC_UCAST_PORT_PORT_ID_VID_VLAN_ID_INCLUDE_STATIC
/*
  *  l2-table set flush mac-ucast port <UINT:port_id> vid <UINT:vlan_id> { include-static }
  */
cparser_result_t cparser_cmd_l2_table_set_flush_mac_ucast_port_port_id_vid_vlan_id_include_static(cparser_context_t *context,
    uint32_t *port_id_ptr,
    uint32_t *vlan_id_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t   config;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == port_id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*vlan_id_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    config.act = 0;
#endif
    config.flushByPort = TRUE;
    config.portOrTrunk = TRUE; /* port-based */
    config.port = *port_id_ptr;
    config.flushByVid = TRUE;
    config.vid = *vlan_id_ptr;

    if('i' == TOKEN_CHAR(8, 0))
        config.flushStaticAddr = TRUE;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_MAC_UCAST_VID_VLAN_ID_INCLUDE_STATIC
/*
  * l2-table set flush mac-ucast vid <UINT:vlan_id> { include-static }
  */
cparser_result_t cparser_cmd_l2_table_set_flush_mac_ucast_vid_vlan_id_include_static(cparser_context_t *context,
    uint32_t *vlan_id_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t   config;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*vlan_id_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    config.act = 0;
#endif
    config.flushByVid = TRUE;
    config.vid = *vlan_id_ptr;

    if('i' == TOKEN_CHAR(6, 0))
        config.flushStaticAddr = TRUE;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_MAC_UCAST_ALL_INCLUDE_STATIC
/*
  *  l2-table set flush mac-ucast all { include-static }
  */
cparser_result_t cparser_cmd_l2_table_set_flush_mac_ucast_all_include_static(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32         include_static = FALSE;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('i' == TOKEN_CHAR(5, 0))
        include_static = TRUE;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_delAll(unit, include_static), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FWD_TABLE_INDEX_PORT_ALL
/*
 * l2-table set fwd-table <UINT:index> ( <PORT_LIST:port> | all )
 */
cparser_result_t cparser_cmd_l2_table_set_fwd_table_index_port_all(cparser_context_t *context,
    uint32_t *index_ptr,
    char **port_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_portmask_t portmask;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == index_ptr), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    portmask = portlist.portmask;


#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        if ((ret = rtk_l2_mcastFwdPortmask_set(unit, *index_ptr, &portmask, 0)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    else
#endif
    {
        if ((ret = rtk_l2_mcastFwdPortmaskEntry_set(unit, *index_ptr, &portmask)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DUMP_IP_MCAST_SIP_DIP_VID
/*
  * l2-table dump ip-mcast <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:vid>
  */
cparser_result_t cparser_cmd_l2_table_dump_ip_mcast_sip_dip_vid(cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vid_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_l2_ipMcastAddr_t    ip_mcast_data;
    char                    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char                    strBuf1[20], strBuf2[20];
    rtk_switch_devInfo_t devInfo;
    uint32                  enable;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    osal_memset(&ip_mcast_data, 0, sizeof(rtk_l2_ipMcastAddr_t));

    diag_util_mprintf("SIP             | DIP             | VID  | Port   \n");
    diag_util_mprintf("----------------+-----------------+------+----------\n");

    /* show specific ip-ipmcast entry */
    ip_mcast_data.dip = *dip_ptr;
    ip_mcast_data.sip = *sip_ptr;
    ip_mcast_data.rvid = *vid_ptr;

    rtk_l2_ipMcstFidVidCompareEnable_get(unit, &enable);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcstFidVidCompareEnable_set(unit, ENABLED), ret);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(unit, &ip_mcast_data), ret);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcstFidVidCompareEnable_set(unit, enable), ret);     /* Restore original setting */
        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(port_list, &ip_mcast_data.portmask);

    diag_util_ip2str_format(strBuf1, ip_mcast_data.sip, 15);
    diag_util_ip2str_format(strBuf2, ip_mcast_data.dip, 15);
    diag_util_mprintf("%s | %s | %4d | %s   ", strBuf1, strBuf2, ip_mcast_data.rvid, port_list);
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_HASH_ALGORITHM
/*
  *  l2-table get hash-algorithm
  */
cparser_result_t cparser_cmd_l2_table_get_hash_algorithm (cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    uint32                           val = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();


#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        ret = rtk_l2_hashAlgo_get(unit, &val);
    }
    else
#endif
    {
        ret = rtk_l2_bucketHashAlgo_get(unit, 0, &val);
    }


    if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    else
    {
        diag_util_mprintf("\tL2 Table Hash Algorithm                        : ");
        if (val == 0)
        {
            diag_util_mprintf("ALGO-0\n");
        }
        else
        {
            diag_util_mprintf("ALGO-1\n");
        }
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_IP6_DIP_CARE_BYTE
/*
  * l2-table get ip6-dip-care-byte
  */
cparser_result_t cparser_cmd_l2_table_get_ip6_dip_care_byte(cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;
    uint32  val;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_l2_ip6CareByte_get(unit, L2_DIP_HASH_CARE_BYTE, &val);
    if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    else
    {
        diag_util_mprintf("\tL2 Hash DIP Care-Byte                        : %#x", val);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_IP6_SIP_CARE_BYTE
/*
  * l2-table get ip6-sip-care-byte
  */
cparser_result_t cparser_cmd_l2_table_get_ip6_sip_care_byte(cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;
    uint32  val;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_l2_ip6CareByte_get(unit, L2_SIP_HASH_CARE_BYTE, &val);
    if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    else
    {
        diag_util_mprintf("\tL2 Hash SIP Care-Byte                        : %#x", val);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_IP6MC_MODE
/*
 *   l2-table get ip6mc-mode
 */
cparser_result_t cparser_cmd_l2_table_get_ip6mc_mode(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    uint32                          val = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();


    diag_util_mprintf("Ip6 Multicast Configuration:\n");
    diag_util_mprintf("\tIp6 Multicast Asic Lookup                       : ");
    if ((ret = rtk_l2_ip6mcMode_get(unit, &val)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    if (val == LOOKUP_ON_FVID_AND_MAC)
    {
        diag_util_mprintf("VID + MAC\n");
    }
    else if (val == LOOKUP_ON_DIP_AND_SIP)
    {
        diag_util_mprintf("DIP + SIP\n");
    }
    else if (val == LOOKUP_ON_DIP_AND_FVID)
    {
        diag_util_mprintf("DIP + VID\n");
    }


    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_IPMC_MODE
/*
 *   l2-table get ipmc-mode
 */
cparser_result_t cparser_cmd_l2_table_get_ipmc_mode(cparser_context_t *context)
{
    int32   ret;
    uint32                          unit = 0;
    uint32                          val = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Ip Multicast Configuration:\n");
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("\tIp Multicast Asic Lookup                       : ");
        DIAG_UTIL_ERR_CHK(rtk_l2_ipmcMode_get(unit, &val), ret);
        if (val == LOOKUP_ON_FVID_AND_MAC)
        {
            diag_util_mprintf("VID + MAC\n");
        }
        else if (val == LOOKUP_ON_DIP_AND_SIP)
        {
            diag_util_mprintf("DIP + SIP\n");
        }
        else if (val == LOOKUP_ON_DIP_AND_FVID)
        {
            diag_util_mprintf("DIP + VID\n");
        }
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_LOOKUP_MISS_PORT_PORT_ALL
/*
  *  l2-table get lookup-miss port ( <PORT_LIST:port> | all )
  */
cparser_result_t cparser_cmd_l2_table_get_lookup_miss_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_action_t        action;
    diag_portlist_t     portlist;


    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_OUTPUT_INIT();
    diag_util_mprintf("Port Lookup Miss Information:\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %u :\n", port);

        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_get(unit, port, DLF_TYPE_UCAST, &action), ret);
        diag_util_mprintf("\tUnicast Lookup Action        : ");
        if (action == ACTION_FORWARD)
        {
            diag_util_mprintf("Forward\n");
        }
        else if (action == ACTION_DROP)
        {
            diag_util_mprintf("Drop\n");
        }
        else if (action == ACTION_TRAP2CPU)
        {
            diag_util_mprintf("Trap-To-Cpu\n");
        }
        else if (action == ACTION_COPY2CPU)
        {
            diag_util_mprintf("Copy-To-Cpu\n");
        }
        else
        {
            diag_util_mprintf("Unknown action\n");
        }

        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_get(unit, port, DLF_TYPE_MCAST, &action), ret);
        diag_util_mprintf("\tL2 Multicast Lookup Action   : ");
        if (action == ACTION_FORWARD)
        {
            diag_util_mprintf("Forward\n");
        }
        else if (action == ACTION_DROP)
        {
            diag_util_mprintf("Drop\n");
        }
        else if (action == ACTION_TRAP2CPU)
        {
            diag_util_mprintf("Trap-To-Cpu\n");
        }
        else if (action == ACTION_COPY2CPU)
        {
            diag_util_mprintf("Copy-To-Cpu\n");
        }
        else
        {
            diag_util_mprintf("Unknown action\n");
        }

        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_get(unit, port, DLF_TYPE_IPMC, &action), ret);
        diag_util_mprintf("\tIPv4 Multicast Lookup Action : ");
        if (action == ACTION_FORWARD)
        {
            diag_util_mprintf("Forward\n");
        }
        else if (action == ACTION_DROP)
        {
            diag_util_mprintf("Drop\n");
        }
        else if (action == ACTION_TRAP2CPU)
        {
            diag_util_mprintf("Trap-To-Cpu\n");
        }
        else if (action == ACTION_COPY2CPU)
        {
            diag_util_mprintf("Copy-To-Cpu\n");
        }
        else
        {
            diag_util_mprintf("Unknown action\n");
        }

        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_get(unit, port, DLF_TYPE_IP6MC, &action), ret);
        diag_util_mprintf("\tIPv6 Multicast Lookup Action : ");
        if (action == ACTION_FORWARD)
        {
            diag_util_mprintf("Forward\n");
        }
        else if (action == ACTION_DROP)
        {
            diag_util_mprintf("Drop\n");
        }
        else if (action == ACTION_TRAP2CPU)
        {
            diag_util_mprintf("Trap-To-Cpu\n");
        }
        else if (action == ACTION_COPY2CPU)
        {
            diag_util_mprintf("Copy-To-Cpu\n");
        }
        else
        {
            diag_util_mprintf("Unknown action\n");
        }
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_LOOKUP_MISS_BCAST_UNICAST_FWD_TBL_IDX
/*
  * l2-table get lookup-miss ( bcast | unicast ) fwd-tbl-idx
  */
cparser_result_t cparser_cmd_l2_table_get_lookup_miss_bcast_unicast_fwd_tbl_idx(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_lookupMissType_t     type = DLF_TYPE_UCAST;
    uint32                      index;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(3, 0))
    {
        type = DLF_TYPE_UCAST;
    }
    else if ('b' == TOKEN_CHAR(3, 0))
    {
        type = DLF_TYPE_BCAST;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMaskIdx_get(unit, type, &index), ret);
    if (DLF_TYPE_UCAST == type)
        diag_util_mprintf("\tUnicast lookup miss forwarding table index : %d\n", index);
    else
        diag_util_mprintf("\tBroadcast lookup miss forwarding table index : %d\n", index);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_IP_MCAST_DIP_CHECK
/*
  *  l2-table get ip-mcast dip-check
 */
cparser_result_t cparser_cmd_l2_table_get_ip_mcast_dip_check(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();


    ret = rtk_l2_ipMcastAddrChkEnable_get(unit, &enable);
    if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    else
    {
        diag_util_mprintf("    IP Multicast DIP check: %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_IP_MCAST_VLAN_COMPARE
/*
  *  l2-table get ip-mcast vlan-compare
  */
cparser_result_t cparser_cmd_l2_table_get_ip_mcast_vlan_compare(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();


    ret = rtk_l2_ipMcstFidVidCompareEnable_get(unit, &enable);
    if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    else
    {
        diag_util_mprintf("    IP Multicast VLAN Compare: %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_PORT_MOVE_STTC_PORT_MOVE_PORT_PORT_ALL
/*
 *   l2-table get port-move sttc-port-move port ( <PORT_LIST:port> | all )
 */
cparser_result_t cparser_cmd_l2_table_get_port_move_sttc_port_move_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_action_t        action;
    diag_portlist_t     portlist;


    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("\tPort %u : ", port);

        DIAG_UTIL_ERR_CHK(rtk_l2_staticPortMoveAction_get(unit, port, &action), ret);
        if (action == ACTION_FORWARD)
        {
            diag_util_mprintf("Forward\n");
        }
        else if (action == ACTION_DROP)
        {
            diag_util_mprintf("Drop\n");
        }
        else if (action == ACTION_TRAP2CPU)
        {
            diag_util_mprintf("Trap-To-Cpu\n");
        }
        else if (action == ACTION_COPY2CPU)
        {
            diag_util_mprintf("Copy-To-Cpu\n");
        }
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_SECURE_MAC_MODE
/*
 *   l2-table get secure-mac-mode
 */
cparser_result_t cparser_cmd_l2_table_get_secure_mac_mode(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_enable_t                    enable = DISABLED;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_secureMacMode_get(unit, &enable), ret);
    diag_util_mprintf("\tSecure MAC mode                           : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DUMP
/*
  *  l2-table dump
  */
cparser_result_t cparser_cmd_l2_table_dump(cparser_context_t *context)
{
    uint32                          unit = 0;
    uint32                          aging_time = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_enable_t                    enable = DISABLED;
    rtk_portmask_t                  portmask;
    char                            port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    uint32                          val = 0, val1 =0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    /* show configured register setting */
    diag_util_mprintf("L2 Lookup Table Information :\n");

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if(IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_aging_get(unit, &aging_time), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_agingTime_get(unit, L2_AGE_TIME_NORMAL, &aging_time), ret);
    }
    diag_util_mprintf("\tAging Time                                     : %d seconds.\n", aging_time);

    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_agingTime_get(unit, L2_AGE_TIME_SUSPEND, &aging_time), ret);
        diag_util_mprintf("\tAging Time for suspend entry                    : %d seconds.\n", aging_time);
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_flushLinkDownPortAddrEnable_get(unit, &enable), ret);
    diag_util_mprintf("\tInvalidate Link Down                           : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    ret = 0;

    DIAG_UTIL_ERR_CHK(rtk_l2_srcPortEgrFilterMask_get(unit, &portmask), ret);
    osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
    diag_util_lPortMask2str(port_list, &portmask);
    diag_util_mprintf("\tSource Port Egress Filter                      : %s\n", port_list);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if(IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_hashAlgo_get(unit, &val), ret);
        val1 = val;
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_bucketHashAlgo_get(unit, 0, &val), ret);

        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_bucketHashAlgo_get(unit, 1, &val1), ret);
        }

    }

    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_printf("\tL2 Table Hash Algorithm                        : ");
        if (val == 0)
        {
            diag_util_mprintf("ALGO-0\n");
        }
        else
        {
            diag_util_mprintf("ALGO-1\n");
        }
    }

    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_printf("\tL2 Table Hash Algorithm                        :  Block0 -- %s, Block1 -- %s\n", (val==0 ? "ALGO-0" : "ALGO-1"), (val1==0 ? "ALGO-0" : "ALGO-1"));
    }

    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Ip Multicast Configuration:\n");
        ret = rtk_l2_ipmcMode_get(unit, &val);
        if (ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        else
        {
            diag_util_printf("\tIp Multicast Lookup Mode                       : ");
            if (val == LOOKUP_ON_FVID_AND_MAC)
            {
                diag_util_mprintf("VID + MAC\n");
            }
            else if (val == LOOKUP_ON_DIP_AND_SIP)
            {
                diag_util_mprintf("DIP + SIP\n");
            }
            else
            {
                diag_util_mprintf("VID + DIP\n");
            }
        }
    }

    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Ipv6 Multicast Configuration:\n");

        ret = rtk_l2_ip6mcMode_get(unit, &val);
        if (ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        else
        {
            diag_util_printf("\tIpv6 Multicast Lookup Mode                     : ");
            if (val == LOOKUP_ON_FVID_AND_MAC)
            {
                diag_util_mprintf("VID + MAC\n");
            }
            else if (val == LOOKUP_ON_DIP_AND_SIP)
            {
                diag_util_mprintf("DIP + SIP\n");
            }
            else
            {
                diag_util_mprintf("VID + DIP\n");
            }
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_exceptionAddrAction_get(unit, SA_IS_BCAST_OR_MCAST, &val), ret);

    diag_util_printf("Exception Src Mac Configuration :\n");
    diag_util_printf("\tBroadcast/Multicast Src Mac Action             : ");
    if (val == ACTION_DROP)
    {
        diag_util_mprintf("Drop\n");
    }
    else if(val == ACTION_TRAP2CPU)
    {
        diag_util_mprintf("Trap-To-Cpu\n");
    }

    else if(val == ACTION_TRAP2MASTERCPU)
    {
        diag_util_mprintf("Trap-To Master Cpu\n");
    }

    else
    {
        diag_util_mprintf("Forward\n");
    }

    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_exceptionAddrAction_get(unit, SA_IS_ZERO, &val), ret);
        diag_util_printf("\tZero Src Mac Action             : ");
        if (val == ACTION_DROP)
        {
            diag_util_mprintf("Drop\n");
        }
        else if(val == ACTION_TRAP2CPU)
        {
            diag_util_mprintf("Trap-To-Cpu\n");
        }

        else if(val == ACTION_TRAP2MASTERCPU)
        {
            diag_util_mprintf("Trap-To Master Cpu\n");
        }

        else
        {
            diag_util_mprintf("Forward\n");
        }

    }


    ret = rtk_l2_zeroSALearningEnable_get(unit, &enable);
    if (ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    else
    {
        diag_util_mprintf("\tZero SA Learning                               : %s", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DUMP_PORT_ALL
/*
  *  l2-table dump  ( <PORT_LIST:port> | all )
  */
cparser_result_t cparser_cmd_l2_table_dump_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32                          unit = 0;
    uint32                          mac_cnt = 0, mac_cnt_disable;
    int32                           ret = RT_ERR_FAILED;
    rtk_enable_t                    enable = DISABLED;
    rtk_port_t                      port = 0;
    rtk_l2_limitLearnCntAction_t    action = LIMIT_LEARN_CNT_ACTION_DROP;
    rtk_l2_vlanMode_t         vlanMode = BASED_ON_INNER_VLAN;
    rtk_l2_newMacLrnMode_t  lrnMode;
    rtk_action_t                     fwdAction;
    diag_portlist_t               portlist;
    rtk_l2_macCnt_t                 limitCnt;
    rtk_l2_macLimitAction_t         limitAction;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 2), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %d :\n", port);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
        if(IS_BACKWARD_COMPATIBLE)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_portLearningCnt_get(unit, port, &mac_cnt), ret);
        }
        else
#endif
        {
            limitCnt.portTrkCnt.id = port;
            DIAG_UTIL_ERR_CHK(rtk_l2_macLearningCnt_get(unit, L2_MAC_LIMIT_PORT, &limitCnt), ret);
            mac_cnt = limitCnt.portTrkCnt.cnt;
        }
        diag_util_mprintf("\tLearning Count                    : %d\n", mac_cnt);

        if(ENABLED == enable)
        {
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if(IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCnt_get(unit, port, &mac_cnt), ret);
            }
            else
#endif
            {
               DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningNum_get(unit, L2_MAC_LIMIT_PORT, &limitCnt), ret);
               mac_cnt = limitCnt.portTrkCnt.cnt;
            }
            DIAG_OM_GET_CHIP_CAPACITY(unit, mac_cnt_disable, l2_learn_limit_cnt_disable);
            if (mac_cnt == mac_cnt_disable)
                diag_util_mprintf("\t\tLimit Learning Max Count  : Unlimited\n");
            else
                diag_util_mprintf("\t\tLimit Learning Max Count  : %u\n", mac_cnt);

            diag_util_mprintf("\t\tLimit Learning Action     : ");
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if(IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCntAction_get(unit, port, &action), ret);
            }
            else
#endif
            {
                limitAction.portTrkAct.id = port;
                DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_get(unit, L2_MAC_LIMIT_PORT, &limitAction), ret);
                action = limitAction.portTrkAct.act;
            }
            if (LIMIT_LEARN_CNT_ACTION_FORWARD == action)
            {
                diag_util_mprintf("Forward");
            }
            else if (LIMIT_LEARN_CNT_ACTION_DROP == action)
            {
                diag_util_mprintf("Drop");
            }
            else if (LIMIT_LEARN_CNT_ACTION_TO_CPU == action)
            {
                diag_util_mprintf("Trap-To-Cpu");
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
            diag_util_mprintf("\n");
        }

        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {

            ret = rtk_l2_vlanMode_get(unit, port, &vlanMode);
            if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
            else
            {
                diag_util_mprintf("\tVLAN Forward Mode                 : ");
                if (vlanMode == BASED_ON_INNER_VLAN)
                {
                    diag_util_mprintf("Base-On-INNER-Vid\n");
                }
                else
                {
                    diag_util_mprintf("Base-On-OUTER-Vid\n");
                }
            }
        }

        {
            ret = rtk_l2_portNewMacOp_get(unit, port, &lrnMode, &fwdAction);
            if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
            else
            {
                diag_util_mprintf("\tNew Src Mac Operation             : ");
                if (lrnMode == HARDWARE_LEARNING)
                {
                    diag_util_mprintf("Asic-AUTO-Learn\n");
                }
                else if(lrnMode == SOFTWARE_LEARNING)
                {
                    diag_util_mprintf("Learn-As-SUSPEND\n");
                }
                else
                {
                    diag_util_mprintf("Not-Learn\n");
                }

                diag_util_mprintf("\tPacket Action                     : ");
                if (fwdAction == ACTION_FORWARD)
                {
                    diag_util_mprintf("Forward\n");
                }
                else if(fwdAction == ACTION_COPY2CPU)
                {
                    diag_util_mprintf("Copy-To-Cpu\n ");
                }
                else if(fwdAction == ACTION_TRAP2CPU)
                {
                    diag_util_mprintf("Trap-To Cpu\n");
                }
                else if (fwdAction == ACTION_TRAP2MASTERCPU)
                {
                   diag_util_mprintf("Trap-To Master Cpu\n");
                }
                else if (fwdAction == ACTION_COPY2MASTERCPU)
                {
                   diag_util_mprintf("Copy-To Master Cpu\n");
                }
                else
                {
                    diag_util_printf("User config: Error!\n");
                }
            }
        }

        diag_util_mprintf("\n");
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DUMP_IP_MCAST
/*
  * l2-table dump ip-mcast
  */
cparser_result_t cparser_cmd_l2_table_dump_ip_mcast(cparser_context_t *context)
{
    int32                   ret;
    uint32                  unit = 0;
    uint32                  scan_idx = 0;
    uint32                  total_entry = 0;
    rtk_l2_ipMcastAddr_t    ip_mcast_data;
    char                    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char                    strBuf1[20], strBuf2[20];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ip_mcast_data, 0, sizeof(rtk_l2_ipMcastAddr_t));

    diag_util_mprintf("Index | SIP             | DIP             | VID  | Port                 \n");
    diag_util_mprintf("------+-----------------+-----------------+------+----------------------\n");

    /* show all ip-ipmcast */
    scan_idx = -1; /* get the first entry */
    while (1)
    {
        if ((ret = rtk_l2_nextValidIpMcastAddr_get(unit, (int32 *)&scan_idx, &ip_mcast_data)) != RT_ERR_OK)
        {
            if(ret == RT_ERR_L2_ENTRY_NOTFOUND || ret == RT_ERR_ENTRY_NOTFOUND)
            {
                if (total_entry == 0)
                    diag_util_printf("Entry is not exist\n");
                break;
            }
            else
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }

        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(port_list, &ip_mcast_data.portmask);
        diag_util_ip2str_format(strBuf1, ip_mcast_data.sip, 15);
        diag_util_ip2str_format(strBuf2, ip_mcast_data.dip, 15);
        diag_util_mprintf("%5d | %s | %s | %4d | %20s", scan_idx, strBuf1, strBuf2, ip_mcast_data.rvid, port_list);
        diag_util_mprintf("\n");

        total_entry++;
    }
    diag_util_mprintf("\nTotal Number Of Entries : %d\n",total_entry);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DUMP_IP_MCAST_SIP_DIP
/*
  * l2-table dump ip-mcast <IPV4ADDR:sip> <IPV4ADDR:dip>
  */
cparser_result_t cparser_cmd_l2_table_dump_ip_mcast_sip_dip(cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_l2_ipMcastAddr_t    ip_mcast_data;
    char                    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char                    strBuf1[20], strBuf2[20];
    rtk_switch_devInfo_t devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    osal_memset(&ip_mcast_data, 0, sizeof(rtk_l2_ipMcastAddr_t));

    diag_util_mprintf("SIP             | DIP             | VID  | Port                 \n");
    diag_util_mprintf("----------------+-----------------+------+----------------------\n");

    /* show specific ip-ipmcast entry */
    ip_mcast_data.dip = *dip_ptr;
    ip_mcast_data.sip = *sip_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(unit, &ip_mcast_data), ret);
        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
    diag_util_lPortMask2str(port_list, &ip_mcast_data.portmask);
    diag_util_ip2str_format(strBuf1, ip_mcast_data.sip, 15);
    diag_util_ip2str_format(strBuf2, ip_mcast_data.dip, 15);
    diag_util_mprintf("%s | %s | %4d | %20s", strBuf1, strBuf2, ip_mcast_data.rvid, port_list);
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_PORT_MOVE_DYNAMIC_PORT_MOVE_FLUSH_PORT_ALL_STATE
/*
  * l2-table get port-move dynamic-port-move-flush ( <PORT_LIST:port> | all ) state
  */
cparser_result_t cparser_cmd_l2_table_get_port_move_dynamic_port_move_flush_port_all_state(cparser_context_t *context,
    char **port_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t        enable = DISABLED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_legalPortMoveFlushAddrEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("\tPort %d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_PORT_MOVE_DYNAMIC_PORT_MOVE_FLUSH_PORT_ALL_STATE_DISABLE_ENABLE
/*
  * l2-table set port-move dynamic-port-move-flush ( <PORT_LIST:port> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_l2_table_set_port_move_dynamic_port_move_flush_port_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('e' == TOKEN_CHAR(5, 0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_legalPortMoveFlushAddrEnable_set(unit, port, ENABLED), ret);
        }
    }
    else if('d' == TOKEN_CHAR(5, 0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_legalPortMoveFlushAddrEnable_set(unit, port, DISABLED), ret);
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_IP6_DIP_CARE_BYTE_CARE_BYTE
/*
  * l2-table set ip6-dip-care-byte <HEX:care_byte>
  */
cparser_result_t cparser_cmd_l2_table_set_ip6_dip_care_byte_care_byte(cparser_context_t *context,
    uint32_t *pCareByte)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6CareByte_set(unit, L2_DIP_HASH_CARE_BYTE, *pCareByte), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_IP6_SIP_CARE_BYTE_CARE_BYTE
/*
  * l2-table set ip6-sip-care-byte <HEX:care_byte>
  */
cparser_result_t cparser_cmd_l2_table_set_ip6_sip_care_byte_care_byte(cparser_context_t *context,
    uint32_t *pCareByte)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6CareByte_set(unit, L2_SIP_HASH_CARE_BYTE, *pCareByte), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_IP6MC_MODE_DIP_AND_SIP_DIP_AND_VID_VID_AND_MAC
/*
  * l2-table set ip6mc-mode ( dip-and-sip | dip-and-vid | vid-and-mac)
  */
cparser_result_t cparser_cmd_l2_table_set_ip6mc_mode_dip_and_sip_dip_and_vid_vid_and_mac(cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;
    uint32  mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('s' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_DIP_AND_SIP;
    else if('v' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_DIP_AND_FVID;
    else if('m' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_FVID_AND_MAC;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6mcMode_set(unit, mode), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_IPMC_MODE_DIP_AND_SIP_DIP_AND_VID_VID_AND_MAC
/*
  * l2-table set ipmc-mode ( dip-and-sip | dip-and-vid | vid-and-mac)
  */
cparser_result_t cparser_cmd_l2_table_set_ipmc_mode_dip_and_sip_dip_and_vid_vid_and_mac(cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  unit = 0;
    uint32  mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('s' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_DIP_AND_SIP;
    else if('v' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_DIP_AND_FVID;
    else if('m' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_FVID_AND_MAC;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ipmcMode_set(unit, mode), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DUMP_IP6_MCAST
/*
  * l2-table dump ip6-mcast
  */
cparser_result_t cparser_cmd_l2_table_dump_ip6_mcast(cparser_context_t *context)
{
    uint32                  unit = 0;
    uint32                  scan_idx = 0;
    uint32                  total_entry = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;
    char                    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char                    ipv6DipStr[32], ipv6SipStr[32];
    rtk_switch_devInfo_t    devInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    osal_memset(&ip6_mcast_data, 0, sizeof(rtk_l2_ip6McastAddr_t));

    diag_util_mprintf("Index | SIP                  | DIP                  | VID  | Port                 \n");
    diag_util_mprintf("------+----------------------+----------------------+------+-----------------------\n");

    /* show all ip-ipmcast */
    scan_idx = -1; /* get the first entry */
    while (1)
    {
        if ((ret = rtk_l2_nextValidIp6McastAddr_get(unit, (int32 *)&scan_idx, &ip6_mcast_data)) != RT_ERR_OK)
        {
            /* diag_util_mprintf("Warning:%d \n", ret); */
            break;
        }
        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(port_list, &ip6_mcast_data.portmask);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(ipv6SipStr, ip6_mcast_data.sip.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(ipv6DipStr, ip6_mcast_data.dip.octet), ret);
        diag_util_mprintf("%5d | %20s | %20s | %4d | %20s \n", scan_idx, ipv6SipStr, ipv6DipStr, ip6_mcast_data.rvid, port_list);

        total_entry++;
    }
    diag_util_mprintf("\nTotal Number Of Entries : %d\n",total_entry);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DUMP_IP6_MCAST_SIP_DIP_VID
/*
  * l2-table dump ip6-mcast <IPV6ADDR:sip> <IPV6ADDR:dip> <UINT:vid>
  */
cparser_result_t cparser_cmd_l2_table_dump_ip6_mcast_sip_dip_vid(cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vid_ptr)
{
    char                    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char                    ipv6DipStr[32], ipv6SipStr[32];
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;
    rtk_switch_devInfo_t    devInfo;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    osal_memset(&ip6_mcast_data, 0, sizeof(rtk_l2_ip6McastAddr_t));

    diag_util_mprintf("SIP             | DIP             | VID  | Port     \n");
    diag_util_mprintf("-----------------+-----------------+------+-----------------------\n");

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.sip.octet, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.dip.octet, TOKEN_STR(4)), ret);

    /* show specific ip-ipmcast entry */
    ip6_mcast_data.rvid = *vid_ptr;

    rtk_l2_ipMcstFidVidCompareEnable_get(unit, &enable);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcstFidVidCompareEnable_set(unit, ENABLED), ret);
    DIAG_UTIL_ERR_CHK(rtk_l2_ip6McastAddr_get(unit, &ip6_mcast_data), ret);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcstFidVidCompareEnable_set(unit, enable), ret);     /* Restore original setting */

    osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
    diag_util_lPortMask2str(port_list, &ip6_mcast_data.portmask);
    DIAG_UTIL_ERR_CHK(diag_util_ipv62str(ipv6SipStr, ip6_mcast_data.sip.octet), ret);
    DIAG_UTIL_ERR_CHK(diag_util_ipv62str(ipv6DipStr, ip6_mcast_data.dip.octet), ret);
    diag_util_mprintf("%s | %s | %4d | %20s\n", ipv6SipStr, ipv6DipStr, ip6_mcast_data.rvid, port_list);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LOOKUP_MISS_BCAST_UNICAST_FWD_TBL_IDX_INDEX
/*
  * l2-table set lookup-miss ( bcast | unicast ) fwd-tbl-idx <UINT:index>
  */
cparser_result_t cparser_cmd_l2_table_set_lookup_miss_bcast_unicast_fwd_tbl_idx_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_lookupMissType_t     type = DLF_TYPE_UCAST;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(3, 0))
    {
        type = DLF_TYPE_UCAST;
    }
    else if ('b' == TOKEN_CHAR(3, 0))
    {
        type = DLF_TYPE_BCAST;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMaskIdx_set(unit, type, *index_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LOOKUP_MISS_PORT_PORT_ALL_L2MC_IPMC_IP6MC_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
  * l2-table set lookup-miss port ( <PORT_LIST:port> | all ) ( l2mc | ipmc | ip6mc ) action ( copy-to-cpu | drop | forward | trap-to-cpu )
  */
cparser_result_t cparser_cmd_l2_table_set_lookup_miss_port_port_all_l2mc_ipmc_ip6mc_action_copy_to_cpu_drop_forward_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    int32                           ret = RT_ERR_FAILED;
    uint32                          unit = 0;
    rtk_port_t                      port = 0;
    rtk_l2_lookupMissType_t         type = DLF_TYPE_UCAST;
    rtk_action_t                    action  = ACTION_FORWARD;
    diag_portlist_t                 portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('l' == TOKEN_CHAR(5, 0))
    {
        type = DLF_TYPE_MCAST;
    }
    else if ('i' == TOKEN_CHAR(5, 0))
    {
        if ('m' == TOKEN_CHAR(5, 2))
            type = DLF_TYPE_IPMC;
        else
            type = DLF_TYPE_IP6MC;

    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(7, 0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(7, 0))
    {
        action = ACTION_TRAP2CPU;
    }
    else if ('f' == TOKEN_CHAR(7, 0))
    {
        action = ACTION_FORWARD;
    }
    else if ('c' == TOKEN_CHAR(7, 0))
    {
        action = ACTION_COPY2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_set(unit, port, type, action), ret);
    }


    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_REPLACE_PORT_PORT_ID_REPLACING_PORT_ID
/*
  * l2-table set replace port <UINT:port_id> <UINT:replacing_port_id>
  */
cparser_result_t cparser_cmd_l2_table_set_replace_port_port_id_replacing_port_id(cparser_context_t *context,
uint32_t *port_id_ptr, uint32_t *replacing_port_id)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t   config;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));
    config.act = 1;
    config.flushByPort = 1;
    config.port = *port_id_ptr;
    config.replacingPort = *replacing_port_id;
    config.portOrTrunk = 1;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_REPLACE_PORT_PORT_ID_REPLACING_PORT_ID_INCLUDE_STATIC
/*
 * l2-table set replace port <UINT:port_id> <UINT:replacing_port_id> include-static
 */
cparser_result_t cparser_cmd_l2_table_set_replace_port_port_id_replacing_port_id_include_static(cparser_context_t *context,
    uint32_t *port_id_ptr,
    uint32_t *replacing_port_id_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t   config;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));
    config.act = 1;
    config.flushByPort = 1;
    config.flushByVid = 0;
    config.vid = 0;
    config.port = *port_id_ptr;
    config.replacingPort = *replacing_port_id_ptr;
    config.flushStaticAddr = TRUE;
    config.portOrTrunk = TRUE; /* port-based */

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_REPLACE_PORT_PORT_ID_REPLACING_PORT_ID_VID_VLAN_ID_INCLUDE_STATIC
/*
  * l2-table set replace port <UINT:port_id> <UINT:replacing_port_id> vid <UINT:vlan_id> { include-static }
  */
cparser_result_t cparser_cmd_l2_table_set_replace_port_port_id_replacing_port_id_vid_vlan_id_include_static(cparser_context_t *context,
uint32_t *port_id_ptr, uint32_t *replacing_port_id, uint32_t *vid_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t   config;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));
    config.act = 1;
    config.flushByPort = 1;
    config.flushByVid = 1;
    config.vid = *vid_ptr;
    config.port = *port_id_ptr;
    config.replacingPort = *replacing_port_id;
    config.portOrTrunk = TRUE; /* port-based */
    if (9 == TOKEN_NUM)
        config.flushStaticAddr = TRUE;
    else
        config.flushStaticAddr = FALSE;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_REPLACE_VID_VLAN_ID_REPLACING_PORT_ID_INCLUDE_STATIC
/*
  * l2-table set replace vid <UINT:vlan_id> <UINT:replacing_port_id> { include-static }
  */
cparser_result_t cparser_cmd_l2_table_set_replace_vid_vlan_id_replacing_port_id_include_static(cparser_context_t *context,
    uint32_t *vlan_id_ptr,
    uint32_t *replacing_port_id_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t   config;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));
    config.act = 1;
    config.flushByVid = 1;
    config.vid = *vlan_id_ptr;
    config.replacingPort = *replacing_port_id_ptr;

    if (7 == TOKEN_NUM)
        config.flushStaticAddr = TRUE;
    else
        config.flushStaticAddr = FALSE;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_IP_MCAST_SIP_DIP_VLAN_ID_INDEX_INDEX
/*
 * l2-table add ip-mcast <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:vlan_id> index <UINT:index>
 */
cparser_result_t cparser_cmd_l2_table_add_ip_mcast_sip_dip_vlan_id_index_index(cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vlan_id_ptr,
    uint32_t *index_ptr)
{
    uint32                  unit = 0, mode;
    int32                   ret = RT_ERR_FAILED;
    rtk_l2_ipMcastAddr_t    ip_mcast_data;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == index_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&ip_mcast_data, 0, sizeof(rtk_l2_ipMcastAddr_t));

    ip_mcast_data.rvid      = *vlan_id_ptr;
    ip_mcast_data.dip       = *dip_ptr;
    ip_mcast_data.sip       = *sip_ptr;
    ip_mcast_data.fwdIndex  = *index_ptr;
    ret = rtk_l2_ipMcastAddr_addByIndex(unit, &ip_mcast_data);

    if (ret == RT_ERR_CHIP_NOT_SUPPORTED)
    {
        rtk_l2_ipmcMode_get(unit, &mode);
        if (mode == LOOKUP_ON_FVID_AND_MAC)
        {
            diag_util_printf("Curent hash mode is VID+MAC. Not support ip-mcast addition in this mode.\n");
            return CPARSER_OK;
        }
    }
    else
        DIAG_UTIL_ERR_CHK(ret, ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_SET_IP_MCAST_SIP_DIP_VLAN_ID_PORT_ALL
/*
  * l2-table ( add | set ) ip-mcast <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:vlan_id> ( <PORT_LIST:port> | all )
  */
cparser_result_t cparser_cmd_l2_table_add_set_ip_mcast_sip_dip_vlan_id_port_all(cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vlan_id_ptr,
    char **port_ptr)
{
    uint32          unit = 0, mode;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_ipMcastAddr_t    ip_mcast_data;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&ip_mcast_data, 0, sizeof(rtk_l2_ipMcastAddr_t));

    ip_mcast_data.rvid = *vlan_id_ptr;
    ip_mcast_data.dip = *dip_ptr;
    ip_mcast_data.sip = *sip_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    ip_mcast_data.portmask = portlist.portmask;

    if ('a' == TOKEN_CHAR(1, 0))
    {
        ret = rtk_l2_ipMcastAddr_add(unit, &ip_mcast_data);
    }
    else if ('s' == TOKEN_CHAR(1, 0))
    {
        ret = rtk_l2_ipMcastAddr_set(unit, &ip_mcast_data);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if (ret == RT_ERR_CHIP_NOT_SUPPORTED)
    {
        rtk_l2_ipmcMode_get(unit, &mode);
        if (mode == LOOKUP_ON_FVID_AND_MAC)
        {
            diag_util_printf("Curent hash mode is VID+MAC. Not support ip-mcast addition in this mode.\n");
            return CPARSER_OK;
        }
    }
    else
        DIAG_UTIL_ERR_CHK(ret, ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_IP_MCAST_SIP_DIP_VLAN_ID_FWD_INDEX_INDEX
/*
 * l2-table set ip-mcast <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:vlan_id> fwd_index <UINT:index>
 */
 cparser_result_t cparser_cmd_l2_table_set_ip_mcast_sip_dip_vlan_id_fwd_index_index(cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vlan_id_ptr,
    uint32_t *index_ptr)
{
    uint32          unit = 0, mode;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_ipMcastAddr_t    ip_mcast_data;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&ip_mcast_data, 0, sizeof(rtk_l2_ipMcastAddr_t));

    ip_mcast_data.rvid = *vlan_id_ptr;
    ip_mcast_data.dip = *dip_ptr;
    ip_mcast_data.sip = *sip_ptr;
    ip_mcast_data.fwdIndex = *index_ptr;

    ret = rtk_l2_ipMcastAddr_setByIndex(unit, &ip_mcast_data);

    if (ret == RT_ERR_CHIP_NOT_SUPPORTED)
    {
        rtk_l2_ipmcMode_get(unit, &mode);
        if (mode == LOOKUP_ON_FVID_AND_MAC)
        {
            diag_util_printf("Curent hash mode is VID+MAC. Not support ip-mcast addition in this mode.\n");
            return CPARSER_OK;
        }
    }
    else
        DIAG_UTIL_ERR_CHK(ret, ret);

    return CPARSER_OK;
}

#endif

#ifdef CMD_L2_TABLE_SET_PORT_MOVE_STTC_PORT_MOVE_PORT_PORT_ALL_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
 *   l2-table set port-move sttc-port-move port ( <PORT_LIST:port> | all ) action ( copy-to-cpu | drop | forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_l2_table_set_port_move_sttc_port_move_port_port_all_action_copy_to_cpu_drop_forward_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_action_t        action;
    diag_portlist_t     portlist;


    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ('f' == TOKEN_CHAR(7, 0))
    {
        action = LIMIT_LEARN_CNT_ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(7, 0))
    {
        action = LIMIT_LEARN_CNT_ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(7, 0))
    {
        action = LIMIT_LEARN_CNT_ACTION_TO_CPU;
    }
    else if ('c' == TOKEN_CHAR(7, 0))
    {
        action = LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_staticPortMoveAction_set(unit, port, action), ret);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_ALL_INCLUDE_STATIC
/*
  * l2-table del all { include-static }
  */
cparser_result_t cparser_cmd_l2_table_del_all_include_static(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  include_static = TRUE;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if(4 == TOKEN_NUM)
        include_static = TRUE;
    else
        include_static = FALSE;

    /* del l2 table - all */
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_delAll(unit, include_static), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_IP_MCAST_SIP_DIP
/*
  * l2-table del ip-mcast <IPV4ADDR:sip> <IPV4ADDR:dip>
  */
cparser_result_t cparser_cmd_l2_table_del_ip_mcast_sip_dip(cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(unit, *sip_ptr, *dip_ptr, 0), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_IGNORE_INDEX_IP_MCAST_SIP_DIP
/*
 * l2-table del-ignore-index ip-mcast <IPV4ADDR:sip> <IPV4ADDR:dip>
 */
cparser_result_t cparser_cmd_l2_table_del_ignore_index_ip_mcast_sip_dip(cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_delIgnoreIndex(unit, *sip_ptr, *dip_ptr, 0), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_IP_MCAST_SIP_DIP_VLAN_ID
/*
  * l2-table del ip-mcast <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:vlan_id>
  */
cparser_result_t cparser_cmd_l2_table_del_ip_mcast_sip_dip_vlan_id(cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vlan_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*vlan_id_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(unit, *sip_ptr, *dip_ptr, *vlan_id_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_IGNORE_INDEX_IP_MCAST_SIP_DIP_VLAN_ID
/*
 * l2-table del-ignore-index ip-mcast <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:vlan_id>
 */
cparser_result_t cparser_cmd_l2_table_del_ignore_index_ip_mcast_sip_dip_vlan_id(cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *vlan_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*vlan_id_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_delIgnoreIndex(unit, *sip_ptr, *dip_ptr, *vlan_id_ptr), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_L2_TABLE_DEL_IP6_MCAST_SIP_DIP
/*
  * l2-table del ip6-mcast <IPV6ADDR:sip> <IPV6ADDR:dip>
  */
cparser_result_t cparser_cmd_l2_table_del_ip6_mcast_sip_dip(cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.sip.octet, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.dip.octet, TOKEN_STR(4)), ret);

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6McastAddr_del(unit, ip6_mcast_data.sip, ip6_mcast_data.dip, 0), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_IGNORE_INDEX_IP6_MCAST_SIP_DIP
/*
  * l2-table del-ignore-index ip6-mcast <IPV6ADDR:sip> <IPV6ADDR:dip>
  */
cparser_result_t cparser_cmd_l2_table_del_ignore_index_ip6_mcast_sip_dip(cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.sip.octet, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.dip.octet, TOKEN_STR(4)), ret);

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6McastAddr_delIgnoreIndex(unit, ip6_mcast_data.sip, ip6_mcast_data.dip, 0), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_IP6_MCAST_SIP_DIP_VLAN_ID
/*
  * l2-table del ip6-mcast <IPV6ADDR:sip> <IPV6ADDR:dip> <UINT:vlan_id>
  */
cparser_result_t cparser_cmd_l2_table_del_ip6_mcast_sip_dip_vlan_id(cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vlan_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.sip.octet, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.dip.octet, TOKEN_STR(4)), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*vlan_id_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6McastAddr_del(unit, ip6_mcast_data.sip, ip6_mcast_data.dip, *vlan_id_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_IGNORE_INDEX_IP6_MCAST_SIP_DIP_VLAN_ID
/*
  * l2-table del-ignore-index ip6-mcast <IPV6ADDR:sip> <IPV6ADDR:dip> <UINT:vlan_id>
  */
cparser_result_t cparser_cmd_l2_table_del_ignore_index_ip6_mcast_sip_dip_vlan_id(cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *vlan_id_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.sip.octet, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.dip.octet, TOKEN_STR(4)), ret);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vlan_id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*vlan_id_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6McastAddr_delIgnoreIndex(unit, ip6_mcast_data.sip, ip6_mcast_data.dip, *vlan_id_ptr), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_L2_TABLE_DUMP_IP6_MCAST_SIP_DIP
/*
 * l2-table dump ip6-mcast <IPV6ADDR:sip> <IPV6ADDR:dip>
 */
cparser_result_t cparser_cmd_l2_table_dump_ip6_mcast_sip_dip(cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr)
{
    char                    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char                    ipv6DipStr[32], ipv6SipStr[32];
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;
    rtk_switch_devInfo_t    devInfo;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == sip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == dip_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    osal_memset(&ip6_mcast_data, 0, sizeof(rtk_l2_ip6McastAddr_t));

    diag_util_mprintf("SIP             | DIP             | VID  | Port     \n");
    diag_util_mprintf("-----------------+-----------------+------+-----------------------\n");

    /* show specific ip-ipmcast entry */
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.sip.octet, TOKEN_STR(3)), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(ip6_mcast_data.dip.octet, TOKEN_STR(4)), ret);

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6McastAddr_get(unit, &ip6_mcast_data), ret);

    diag_util_mprintf("vid = %d\n", ip6_mcast_data.rvid);

    osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
    diag_util_lPortMask2str(port_list, &ip6_mcast_data.portmask);
    DIAG_UTIL_ERR_CHK(diag_util_ipv62str(ipv6SipStr, ip6_mcast_data.sip.octet), ret);
    DIAG_UTIL_ERR_CHK(diag_util_ipv62str(ipv6DipStr, ip6_mcast_data.dip.octet), ret);
    diag_util_mprintf("%s | %s | %4d | %20s\n", ipv6SipStr, ipv6DipStr, ip6_mcast_data.rvid, port_list);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_MAC_UCAST_VID_MAC_ROUTE_TARGET_VLAN_INNER_OUTER
/*
 * l2-table set mac-ucast <UINT:vid> <MACADDR:mac> route-target-vlan ( inner | outer )
 */
cparser_result_t cparser_cmd_l2_table_set_mac_ucast_vid_mac_route_target_vlan_inner_outer(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;
    uint32 vlan_target = 0;
    rtk_l2_ucastAddr_t  l2_ucast;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);

    if ((TRUE == diag_util_isBcastMacAddr(mac_ptr->octet)) || (TRUE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Broadcast and Multicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    if ('i' == TOKEN_CHAR(6, 0))
    {
        vlan_target = 0;
    }
    else
    {
        vlan_target = 1;
    }

    osal_memset(&l2_ucast, 0, sizeof(rtk_l2_ucastAddr_t));
    osal_memcpy(l2_ucast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    l2_ucast.vid = *vid_ptr;
    /* Add for test chip */
    l2_ucast.flags |= RTK_L2_UCAST_FLAG_NEXTHOP;

    if (rtk_l2_addr_get(unit, &l2_ucast) == RT_ERR_L2_ENTRY_NOTFOUND)
    {
        diag_util_printf("Entry is not exist.\n");
        return CPARSER_NOT_OK;
    }

    l2_ucast.flags |= RTK_L2_UCAST_FLAG_NEXTHOP;
    l2_ucast.vlan_target = vlan_target;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(unit, &l2_ucast), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_ENTRY_INDEX_INDEX
/*
 * l2-table get entry index <UINT:index>
 */
cparser_result_t cparser_cmd_l2_table_get_entry_index_index(cparser_context_t *context, uint32_t *index_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_l2_entry_t          entry;
    rtk_l2_ucastAddr_t      l2_data;
    rtk_l2_mcastAddr_t      mcast_data;
    rtk_l2_ipMcastAddr_t    ip_mcast_data;
    rtk_l2_ip6McastAddr_t   ip6_mcast_data;
    char                    port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char                    strSip[32], strDip[32];
#if (defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300))
    char                    buffer[6];
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&entry, 0, sizeof(rtk_l2_entry_t));
    osal_memset(&l2_data, 0, sizeof(rtk_l2_ucastAddr_t));
    osal_memset(&mcast_data, 0, sizeof(rtk_l2_mcastAddr_t));
    osal_memset(&ip_mcast_data, 0, sizeof(rtk_l2_ipMcastAddr_t));
    osal_memset(&ip6_mcast_data, 0, sizeof(rtk_l2_ip6McastAddr_t));


    if ((ret = rtk_l2_addrEntry_get(unit, *index_ptr, &entry)) != RT_ERR_OK)
    {
        if (ret == RT_ERR_L2_INVALID_FLOWTYPE)
            ret = RT_ERR_INPUT;
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (FALSE == entry.valid)
    {
        diag_util_printf("Entry is invalid\n");
        return CPARSER_NOT_OK;
    }

    if (FLOW_TYPE_UNICAST == entry.entry_type)
    {
        osal_memcpy(&l2_data, &entry.unicast, sizeof(rtk_l2_ucastAddr_t));
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID | MAC Address       | SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Agg VID | Age | vlanTarget | mac_idx |\n");
            diag_util_mprintf("-------+-----+-------------------+------+----------+----------+---------+----------+---------+---------+-----+---------+-------\n");
            diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %8d | %8d | %7d | %8d | %7d |   %4d  |  %2d| %10d | %8d \n",
                                *index_ptr,
                                l2_data.vid,
                                l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                         l2_data.port, (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                         (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                         (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0, l2_data.agg_vid, l2_data.age ,
                         ((l2_data.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? l2_data.vlan_target : 0),
                         ((l2_data.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? l2_data.route_idx : 0));
        }
#endif

#if defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            sprintf(buffer, "%d", l2_data.devID);
            diag_util_mprintf(" Index | VID | MAC Address       | TRK | DevID | SPA |SBlk|DBlk|Sttc| NH |Sus|    AVID    |  MAC Index | Age\n");
            diag_util_mprintf("-------+-----+-------------------+-----+------+-----+----+----+----+----+---+------------+------------+-----\n");
            if(l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %3d | %4s | %3d |%4d|%4d|%4d| %2d |%3d| %s| %11d|  %2d\n",
                            *index_ptr,
                            l2_data.vid,
                            l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                            (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                            "Not Support",
                            l2_data.route_idx,
                            l2_data.age);
            }
            else
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %3d | %4s | %3d |%4d|%4d|%4d| %2d |%3d| %10d | %s|  %2d\n",
                            *index_ptr,
                            l2_data.vid,
                            l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                         (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                         (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                         (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                         (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                         (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                         (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                         (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                         (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                         l2_data.agg_vid,
                         "Not Support",
                         l2_data.age);
            }
        }
#endif

#if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            sprintf(buffer, "%d", l2_data.devID);
            diag_util_mprintf(" Index | VID | MAC Address       |TRK|DevID|SPA|SBlk|DBlk|Sttc|NH|Sus|Tag|APri|AVID|Tnl|ECID/Idx| Age\n");
            diag_util_mprintf("-------+-----+-------------------+---+-----+---+----+----+----+--+---+---+----+----+---+--------+-----\n");
            diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %d |%5s|%3d|%4d|%4d|%4d|%2d|%3d|%3d|%4d|%4d| %d |0x%06X|  %2d\n",
                            *index_ptr,
                            l2_data.vid,
                            l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? l2_data.trk_gid : l2_data.port,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP) ? 1 : 0,
                            (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TAG_STS) ? 1 : 0,
                            l2_data.agg_pri,
                            l2_data.agg_vid,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? l2_data.l2_tunnel_idx : l2_data.ecid,
                            l2_data.age);
        }
#endif
    }
    else if (FLOW_TYPE_L2_MULTI == entry.entry_type)
    {
        osal_memcpy(&mcast_data, &entry.l2mcast, sizeof(rtk_l2_mcastAddr_t));
#if defined(CONFIG_SDK_RTL8380)
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID  | MAC address       | Port                 | Agg Vid\n");
            diag_util_mprintf("-------+------+-------------------+----------------------+-----------\n");
            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &mcast_data.portmask);

            diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d\n",
                        *index_ptr,
                        mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                        mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.agg_vid);
        }
#endif

        if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID  | MAC address       | Port                \n");
            diag_util_mprintf("-------+------+-------------------+----------------------\n");
            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &mcast_data.portmask);

            diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s \n",
                        *index_ptr,
                        mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                        mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list);
        }

#if defined(CONFIG_SDK_RTL9300)
        if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &mcast_data.portmask);
            diag_util_mprintf(" Index | VID  | MAC address       | Port                 | nexthop | MAC Index\n");
            diag_util_mprintf("-------+-----+-------------------+----------------------+---------+---------\n");
            diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d | %4d\n",
                            *index_ptr,
                            mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                            mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.nextHop, mcast_data.mac_idx);
        }
#endif

#if defined(CONFIG_SDK_RTL9310)
        if(DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &mcast_data.portmask);
            diag_util_mprintf(" Index | VID | MAC address       | Port                 | NH | Group ID \n");
            diag_util_mprintf("-------+-----+-------------------+----------------------+----+------------\n");
            diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %2d | 0x%X\n",
                        *index_ptr,
                        mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                        mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.nextHop, mcast_data.groupId);
        }
#endif
    }
    else if (FLOW_TYPE_IP4_MULTI == entry.entry_type)
    {
        osal_memcpy(&ip_mcast_data, &entry.ipmcast, sizeof(rtk_l2_ipMcastAddr_t));
        diag_util_lPortMask2str(port_list, &ip_mcast_data.portmask);
        diag_util_ip2str_format(strSip, ip_mcast_data.sip, 15);
        diag_util_ip2str_format(strDip, ip_mcast_data.dip, 15);

        diag_util_mprintf("Index | SIP             | DIP             | VID  | Port                 \n");
        diag_util_mprintf("------+-----------------+-----------------+------+----------------------\n");
        diag_util_mprintf("%5d | %s | %s | %4d | %20s\n", *index_ptr, strSip, strDip, ip_mcast_data.rvid, port_list);
    }
    else if (FLOW_TYPE_IP6_MULTI == entry.entry_type)
    {
        osal_memcpy(&ip6_mcast_data, &entry.ip6mcast, sizeof(rtk_l2_ip6McastAddr_t));
        diag_util_lPortMask2str(port_list, &ip6_mcast_data.portmask);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strSip, ip6_mcast_data.sip.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strDip, ip6_mcast_data.dip.octet), ret);

        diag_util_mprintf("Index | SIP                  | DIP                  | VID  | Port                 \n");
        diag_util_mprintf("------+----------------------+----------------------+------+-----------------------\n");
        diag_util_mprintf("%5d | %20s | %20s | %4d | %20s \n", *index_ptr, strSip, strDip, ip6_mcast_data.rvid, port_list);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_NEXT_VALID_ENTRY_TYPE_UC_DYNAMIC_UC_SOFTWARE_CFG_UC_NEXTHOP_UC_MC_NEXTHOP_MC_START_INDEX_INDEX
/*
 * l2-table get next-valid-entry type ( uc | dynamic-uc | software_cfg_uc | nexthop-uc | mc | nexthop-mc ) start-index <UINT:index>
 */
cparser_result_t cparser_cmd_l2_table_get_next_valid_entry_type_uc_dynamic_uc_software_cfg_uc_nexthop_uc_mc_nexthop_mc_start_index_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32             unit = 0;
    int32              ret = RT_ERR_FAILED;

    rtk_l2_entry_t l2_entry;
    rtk_l2_nextValidType_t method;
    int32 scanIdx;
    char                port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char                buffer[6];

    DIAG_OM_GET_UNIT_ID(unit);

    if ('u' == TOKEN_CHAR(4, 0) && 'c' == TOKEN_CHAR(4, 1))
    {
        method = L2_NEXT_VALID_TYPE_UC;
    }
    else if ('d' == TOKEN_CHAR(4, 0) && 'u' == TOKEN_CHAR(4, 8))
    {
        method = L2_NEXT_VALID_TYPE_AUTO_UC;
    }
    else if('s' == TOKEN_CHAR(4, 0) && 'o' == TOKEN_CHAR(4, 1))
    {
        method = L2_NEXT_VALID_TYPE_SOFTWARE_UC;
    }
    else if('n' == TOKEN_CHAR(4, 0) && 'u' == TOKEN_CHAR(4, 8))
    {
        method = L2_NEXT_VALID_TYPE_UC_NH;
    }
    else if('m' == TOKEN_CHAR(4, 0) && 'c' == TOKEN_CHAR(4, 1))
    {
        method = L2_NEXT_VALID_TYPE_MC;
    }
    else if('n' == TOKEN_CHAR(4, 0) && 'm' == TOKEN_CHAR(4, 8))
    {
        method = L2_NEXT_VALID_TYPE_MC_NH;
    }
    else
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    scanIdx = *index_ptr;

    diag_util_mprintf("scan from Index %d, method = %d\n", scanIdx, method);

    ret = rtk_l2_hwNextValidAddr_get(unit, &scanIdx, method, &l2_entry);
    if(ret!=RT_ERR_OK && ret!=RT_ERR_ENTRY_NOTFOUND)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if(ret==RT_ERR_ENTRY_NOTFOUND)
    {
        diag_util_mprintf("entry is not exist!\n");
        return CPARSER_OK;
    }


    if(l2_entry.entry_type  == FLOW_TYPE_UNICAST)
    {
#if defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID | MAC Address       | TRK | DevID | SPA |SBlk|DBlk|Sttc| NH |Sus|    AVID    |  MAC Index | Age\n");
            diag_util_mprintf("-------+-----+-------------------+-----+------+-----+----+----+----+----+---+------------+------------+-----\n");

            if(l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_NEXTHOP)
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %3d | %4s | %3d |%4d|%4d|%4d| %2d |%3d| %s| %11d|  %2d\n",
                            l2_entry.unicast.l2_idx,
                            l2_entry.unicast.vid,
                            l2_entry.unicast.mac.octet[0],l2_entry.unicast.mac.octet[1],l2_entry.unicast.mac.octet[2],l2_entry.unicast.mac.octet[3],l2_entry.unicast.mac.octet[4],l2_entry.unicast.mac.octet[5],
                            (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                            (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_entry.unicast.devID==1 ? "ntfy" : " - ") : buffer,
                            (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_entry.unicast.trk_gid:l2_entry.unicast.port,
                            (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                            (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                            (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                            (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                            (l2_entry.unicast.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                            "Not Support",
                            l2_entry.unicast.route_idx,
                            l2_entry.unicast.age);
            }
            else
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %3d | %4s | %3d |%4d|%4d|%4d| %2d |%3d| %10d | %s|  %2d\n",
                            l2_entry.unicast.l2_idx,
                            l2_entry.unicast.vid,
                            l2_entry.unicast.mac.octet[0],l2_entry.unicast.mac.octet[1],l2_entry.unicast.mac.octet[2],l2_entry.unicast.mac.octet[3],l2_entry.unicast.mac.octet[4],l2_entry.unicast.mac.octet[5],
                         (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                         (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_entry.unicast.devID==1 ? "ntfy" : " - ") : buffer,
                         (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_entry.unicast.trk_gid:l2_entry.unicast.port,
                         (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                         (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                         (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                         (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                         (l2_entry.unicast.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                         l2_entry.unicast.agg_vid,
                         "Not Support",
                         l2_entry.unicast.age);
            }

        }
#endif

#if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID | MAC Address       |TRK|DevID|SPA|SBlk|DBlk|Sttc|NH|Sus|Tag|APri|AVID|Tnl|ECID/Idx| Age\n");
            diag_util_mprintf("-------+-----+-------------------+---+-----+---+----+----+----+--+---+---+----+----+---+--------+-----\n");

            sprintf(buffer, "%d", l2_entry.unicast.devID);
            diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %d |%5s|%3d|%4d|%4d|%4d|%2d|%3d|%3d|%4d|%4d| %d |0x%06X|  %2d\n",
                l2_entry.unicast.l2_idx,
                l2_entry.unicast.vid,
                l2_entry.unicast.mac.octet[0],l2_entry.unicast.mac.octet[1],l2_entry.unicast.mac.octet[2],l2_entry.unicast.mac.octet[3],l2_entry.unicast.mac.octet[4],l2_entry.unicast.mac.octet[5],
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? 1 : 0,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? l2_entry.unicast.trk_gid : l2_entry.unicast.port,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_NEXTHOP) ? 1 : 0,
                (l2_entry.unicast.state&RTK_L2_UCAST_STATE_SUSPEND) ? 1 : 0,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_TAG_STS) ? 1 : 0,
                l2_entry.unicast.agg_pri,
                l2_entry.unicast.agg_vid,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? 1 : 0,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? l2_entry.unicast.l2_tunnel_idx : l2_entry.unicast.ecid,
                l2_entry.unicast.age);

        }
#endif
    }
    else
    {
#if defined(CONFIG_SDK_RTL9300)
        if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID  | MAC address       | Port                 | nexthop | MAC Index\n");
            diag_util_mprintf("-------+------+-------------------+----------------------+---------+---------\n");

            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &l2_entry.l2mcast.portmask);

            if(l2_entry.l2mcast.nextHop)
            {
                diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d | %4d\n",
                            l2_entry.l2mcast.l2_idx,
                            l2_entry.l2mcast.rvid, l2_entry.l2mcast.mac.octet[0], l2_entry.l2mcast.mac.octet[1], l2_entry.l2mcast.mac.octet[2],
                            l2_entry.l2mcast.mac.octet[3], l2_entry.l2mcast.mac.octet[4], l2_entry.l2mcast.mac.octet[5], port_list, l2_entry.l2mcast.nextHop, l2_entry.l2mcast.mac_idx);
            }
            else
            {
                diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d | %s\n",
                            l2_entry.l2mcast.l2_idx,
                            l2_entry.l2mcast.rvid, l2_entry.l2mcast.mac.octet[0], l2_entry.l2mcast.mac.octet[1], l2_entry.l2mcast.mac.octet[2],
                            l2_entry.l2mcast.mac.octet[3], l2_entry.l2mcast.mac.octet[4], l2_entry.l2mcast.mac.octet[5], port_list, l2_entry.l2mcast.nextHop, "Not Support");
            }
        }
#endif

#if defined(CONFIG_SDK_RTL9310)
        if(DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID  | MAC address       | Port                 | NH | Group ID \n");
            diag_util_mprintf("-------+------+-------------------+----------------------+----+------------\n");

            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &l2_entry.l2mcast.portmask);

            diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %2d | 0x%X\n",
                        l2_entry.l2mcast.l2_idx,
                        l2_entry.l2mcast.rvid, l2_entry.l2mcast.mac.octet[0], l2_entry.l2mcast.mac.octet[1], l2_entry.l2mcast.mac.octet[2],
                        l2_entry.l2mcast.mac.octet[3], l2_entry.l2mcast.mac.octet[4], l2_entry.l2mcast.mac.octet[5], port_list, l2_entry.l2mcast.nextHop, l2_entry.l2mcast.groupId);
        }
#endif
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_L2_TABLE_GET_NEXT_VALID_ENTRY_TYPE_CURRENT_ENTRY_DYNAMIC_UCAST_DYNAMIC_UCAST_AND_MCAST_UCAST_AND_MCAST_START_INDEX_INDEX
/*
*l2-table get next-valid-entry type ( current-entry | dynamic-ucast | dynamic-ucast-and-mcast | ucast-and-mcast ) start-index <UINT:index>
*/
cparser_result_t cparser_cmd_l2_table_get_next_valid_entry_type_current_entry_dynamic_ucast_dynamic_ucast_and_mcast_ucast_and_mcast_start_index_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32             unit = 0;
    int32              ret = RT_ERR_FAILED;

    rtk_l2_entry_t l2_entry;
    rtk_l2_nextValidType_t method;
    int32 scanIdx;

    char                ipv6DipStr[32], ipv6SipStr[32];
    char                strBuf1[20], strBuf2[20];
    char                port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];


    DIAG_OM_GET_UNIT_ID(unit);

    if ('c' == TOKEN_CHAR(4, 0) && 'u' == TOKEN_CHAR(4, 1))
    {
        method = L2_NEXT_VALID_TYPE_CURRENT;
    }
    else if ('u' == TOKEN_CHAR(4, 0) && 'c' == TOKEN_CHAR(4, 1))
    {
        method = L2_NEXT_VALID_TYPE_UC_AND_MC;
    }
    else if('d' == TOKEN_CHAR(4, 0) && 'a' == TOKEN_CHAR(4, 14))
    {
        method = L2_NEXT_VALID_TYPE_DYNAMIC_UC_AND_MC;
    }
    else
        method = L2_NEXT_VALID_TYPE_DYNAMIC_UC;

    scanIdx = *index_ptr;

    diag_util_mprintf("scan from Index %d, method = %d\n", scanIdx, method);

    ret = rtk_l2_hwNextValidAddr_get(unit, &scanIdx, method, &l2_entry);
    if(ret!=RT_ERR_OK && ret!=RT_ERR_ENTRY_NOTFOUND)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if(ret==RT_ERR_ENTRY_NOTFOUND)
    {
        diag_util_mprintf("entry is not exist!\n");
        return CPARSER_OK;
    }

    if(l2_entry.entry_type  == FLOW_TYPE_UNICAST)
    {
        if(l2_entry.unicast.flags & RTK_L2_UCAST_FLAG_NEXTHOP)
        {
            diag_util_mprintf("Index | MAC Address       | VID  | SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Vid_Sel |Rout_index |\n");
            diag_util_mprintf("------+-------------------+------+------+----------+----------+---------+----------+---------+-------+------\n");

            diag_util_mprintf("%5d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4d | %8d | %8d | %7d | %8d | %7d | %5d | %8d\n",
                scanIdx,
                l2_entry.unicast.mac.octet[0],l2_entry.unicast.mac.octet[1],l2_entry.unicast.mac.octet[2],l2_entry.unicast.mac.octet[3],l2_entry.unicast.mac.octet[4],l2_entry.unicast.mac.octet[5],
                l2_entry.unicast.vid, l2_entry.unicast.port, (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0, (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0, (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                (l2_entry.unicast.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0, l2_entry.unicast.vlan_target, l2_entry.unicast.route_idx);

        }
        else
        {
            diag_util_mprintf("Index | MAC Address       | VID  | SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Aggreate VID\n");
            diag_util_mprintf("------+-------------------+------+------+----------+----------+---------+----------+---------+-------------\n");

            diag_util_mprintf("%5d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4d | %8d | %8d | %7d | %8d | %7d | %4d\n",
                scanIdx,
                l2_entry.unicast.mac.octet[0],l2_entry.unicast.mac.octet[1],l2_entry.unicast.mac.octet[2],l2_entry.unicast.mac.octet[3],l2_entry.unicast.mac.octet[4],l2_entry.unicast.mac.octet[5],
                l2_entry.unicast.vid, l2_entry.unicast.port, (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0, (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0, (l2_entry.unicast.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                (l2_entry.unicast.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0, l2_entry.unicast.agg_vid);
        }
    }
    else if(l2_entry.entry_type  == FLOW_TYPE_L2_MULTI)
    {
        diag_util_mprintf("Index | VID  | MAC address       | Port                 | Agg Vid\n");
        diag_util_mprintf("------+------+-------------------+----------------------+-----------\n");
        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(port_list, &l2_entry.l2mcast.portmask);

        diag_util_mprintf("%5d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d\n",
                    scanIdx,
                    l2_entry.l2mcast.rvid, l2_entry.l2mcast.mac.octet[0], l2_entry.l2mcast.mac.octet[1], l2_entry.l2mcast.mac.octet[2],
                    l2_entry.l2mcast.mac.octet[3], l2_entry.l2mcast.mac.octet[4], l2_entry.l2mcast.mac.octet[5], port_list, l2_entry.l2mcast.agg_vid);
    }
    else if(l2_entry.entry_type  == FLOW_TYPE_IP4_MULTI)
    {

        diag_util_mprintf("Index | SIP             | DIP             | VID  | Port                 \n");
        diag_util_mprintf("------+-----------------+-----------------+------+----------------------\n");

        diag_util_ip2str_format(strBuf1, l2_entry.ipmcast.sip, 15);
        diag_util_ip2str_format(strBuf2, l2_entry.ipmcast.dip, 15);
        diag_util_mprintf("%5d | %s | %s | %4d | %20s\n", scanIdx, strBuf1, strBuf2, l2_entry.ipmcast.rvid, port_list);
    }
    else if(l2_entry.entry_type  == FLOW_TYPE_IP6_MULTI)
    {
        diag_util_mprintf("Index | SIP                  | DIP                  | VID  | Port                 \n");
        diag_util_mprintf("------+----------------------+----------------------+------+-----------------------\n");

        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(port_list, &l2_entry.ip6mcast.portmask);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(ipv6SipStr, l2_entry.ip6mcast.sip.octet), ret);
        DIAG_UTIL_ERR_CHK(diag_util_ipv62str(ipv6DipStr, l2_entry.ip6mcast.dip.octet), ret);
        diag_util_mprintf("%5d | %20s | %20s | %4d | %20s \n", scanIdx, ipv6SipStr, ipv6DipStr, l2_entry.ip6mcast.rvid, port_list);
    }
    else
        return CPARSER_NOT_OK;

    return CPARSER_OK;
}

#endif

#ifdef CMD_L2_TABLE_GET_LINK_DOWN_FLUSH_STATE
/*
 *   l2-table get link-down-flush state
 */
cparser_result_t cparser_cmd_l2_table_get_link_down_flush_state(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_enable_t                    enable = DISABLED;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_flushLinkDownPortAddrEnable_get(unit, &enable), ret);
    diag_util_mprintf("\tInvalidate Link Down                           : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LINK_DOWN_FLUSH_STATE_DISABLE_ENABLE
/*
  * l2-table set link-down-flush state ( disable | enable )
  */
cparser_result_t cparser_cmd_l2_table_set_link_down_flush_state_disable_enable(cparser_context_t *context)
{
    uint32 unit = 0;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(4, 0))
    {
        /* set invalid link down enable */
        DIAG_UTIL_ERR_CHK(rtk_l2_flushLinkDownPortAddrEnable_set(unit, ENABLED), ret);
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        /* set invalid link down disable */
        DIAG_UTIL_ERR_CHK(rtk_l2_flushLinkDownPortAddrEnable_set(unit, DISABLED), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_FLUSH_ENTRY_DYNAMIC_ONLY_INCLUDE_STATIC
/*
l2-table set flush flush-entry ( dynamic-only | include-static )
*/
cparser_result_t cparser_cmd_l2_table_set_flush_flush_entry_dynamic_only_include_static(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    if ('d' == TOKEN_CHAR(4, 0))
    {
        config.act = FLUSH_ACT_FLUSH_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(4, 0))
    {
        config.act = FLUSH_ACT_FLUSH_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_FLUSH_ENTRY_PORT_TRUNK_ID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
l2-table set flush flush-entry ( port | trunk ) <UINT:id> ( dynamic-only | include-static )
*/
cparser_result_t cparser_cmd_l2_table_set_flush_flush_entry_port_trunk_id_dynamic_only_include_static(cparser_context_t *context,
    uint32_t *id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;

    if ('p' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(6, 0))
    {
        config.act = FLUSH_ACT_FLUSH_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(6, 0))
    {
        config.act = FLUSH_ACT_FLUSH_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_FLUSH_ENTRY_VID_VID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
l2-table set flush flush-entry vid <UINT:vid> ( dynamic-only | include-static )
*/
cparser_result_t cparser_cmd_l2_table_set_flush_flush_entry_vid_vid_dynamic_only_include_static(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    if ('d' == TOKEN_CHAR(6, 0))
    {
        config.act = FLUSH_ACT_FLUSH_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(6, 0))
    {
        config.act = FLUSH_ACT_FLUSH_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_FLUSH_ENTRY_PORT_TRUNK_ID_VID_VID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
l2-table set flush flush-entry ( port | trunk ) <UINT:id> vid <UINT:vid> ( dynamic-only | include-static )
*/
cparser_result_t cparser_cmd_l2_table_set_flush_flush_entry_port_trunk_id_vid_vid_dynamic_only_include_static(cparser_context_t *context,
    uint32_t *id_ptr,
    uint32_t *vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.devID), ret);
#endif
    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    if ('d' == TOKEN_CHAR(8, 0))
    {
        config.act = FLUSH_ACT_FLUSH_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(8, 0))
    {
        config.act = FLUSH_ACT_FLUSH_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_REPLACE_ENTRY_REPLACE_PORT_REPLACE_TRUNK_REPLACE_ID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
l2-table set flush replace-entry ( replace-port | replace-trunk ) <UINT:replace_id> ( dynamic-only | include-static )
*/
cparser_result_t cparser_cmd_l2_table_set_flush_replace_entry_replace_port_replace_trunk_replace_id_dynamic_only_include_static(cparser_context_t *context,
    uint32_t *replace_id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    if ('p' == TOKEN_CHAR(4, 8))
    {
        config.replacePortOrTrunk = ENABLED;
        config.replacingPort = *replace_id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.replacing_devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(4, 8))
    {
        config.replacePortOrTrunk = DISABLED;
        config.replacingTrk = *replace_id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(6, 0))
    {
        config.act = FLUSH_ACT_REPLACE_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(6, 0))
    {
        config.act = FLUSH_ACT_REPLACE_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_REPLACE_ENTRY_PORT_TRUNK_ID_REPLACE_PORT_REPLACE_TRUNK_REPLACE_ID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
l2-table set flush replace-entry ( port | trunk ) <UINT:id> ( replace-port | replace-trunk ) <UINT:replace_id>  ( dynamic-only | include-static )
*/
cparser_result_t cparser_cmd_l2_table_set_flush_replace_entry_port_trunk_id_replace_port_replace_trunk_replace_id_dynamic_only_include_static(cparser_context_t *context,
    uint32_t *id_ptr,
    uint32_t *replace_id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('p' == TOKEN_CHAR(6, 8))
    {
        config.replacePortOrTrunk = ENABLED;
        config.replacingPort = *replace_id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.replacing_devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(6, 8))
    {
        config.replacePortOrTrunk = DISABLED;
        config.replacingTrk = *replace_id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(8, 0))
    {
        config.act = FLUSH_ACT_REPLACE_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(8, 0))
    {
        config.act = FLUSH_ACT_REPLACE_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_REPLACE_ENTRY_VID_VID_REPLACE_PORT_REPLACE_TRUNK_REPLACE_ID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
l2-table set flush replace-entry vid <UINT:vid> ( replace-port | replace-trunk ) <UINT:replace_id> ( dynamic-only | include-static )
*/
cparser_result_t cparser_cmd_l2_table_set_flush_replace_entry_vid_vid_replace_port_replace_trunk_replace_id_dynamic_only_include_static(cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *replace_id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    if ('p' == TOKEN_CHAR(6, 8))
    {
        config.replacePortOrTrunk = ENABLED;
        config.replacingPort = *replace_id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.replacing_devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(6, 8))
    {
        config.replacePortOrTrunk = DISABLED;
        config.replacingTrk = *replace_id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(8, 0))
    {
        config.act = FLUSH_ACT_REPLACE_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(8, 0))
    {
        config.act = FLUSH_ACT_REPLACE_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_REPLACE_ENTRY_PORT_TRUNK_ID_VID_VID_REPLACE_PORT_REPLACE_TRUNK_REPLACE_ID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
l2-table set flush replace-entry ( port | trunk ) <UINT:id> vid <UINT:vid> ( replace-port | replace-trunk ) <UINT:replace_id> ( dynamic-only | include-static )
*/
cparser_result_t cparser_cmd_l2_table_set_flush_replace_entry_port_trunk_id_vid_vid_replace_port_replace_trunk_replace_id_dynamic_only_include_static(cparser_context_t *context,
    uint32_t *id_ptr,
    uint32_t *vid_ptr,
    uint32_t *replace_id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    if ('p' == TOKEN_CHAR(8, 8))
    {
        config.replacePortOrTrunk = ENABLED;
        config.replacingPort = *replace_id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.replacing_devID), ret);
#endif
    }
    else if ('t' == TOKEN_CHAR(8, 8))
    {
        config.replacePortOrTrunk = DISABLED;
        config.replacingTrk = *replace_id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(10, 0))
    {
        config.act = FLUSH_ACT_REPLACE_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(10, 0))
    {
        config.act = FLUSH_ACT_REPLACE_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_NEXTHOP
/*
l2-table set flush clear-nexthop
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_nexthop(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.act = FLUSH_ACT_CLEAR_NEXTHOP;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_NEXTHOP_PORT_TRUNK_ID
/*
l2-table set flush clear-nexthop ( port | trunk ) <UINT:id>
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_nexthop_port_trunk_id(cparser_context_t *context,
    uint32_t *id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.devID), ret);
#endif
    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.act = FLUSH_ACT_CLEAR_NEXTHOP;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_NEXTHOP_VID_VID
/*
l2-table set flush clear-nexthop vid <UINT:vid>
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_nexthop_vid_vid(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    config.act = FLUSH_ACT_CLEAR_NEXTHOP;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_NEXTHOP_PORT_TRUNK_ID_VID_VID
/*
l2-table set flush clear-nexthop ( port | trunk ) <UINT:id> vid <UINT:vid>
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_nexthop_port_trunk_id_vid_vid(cparser_context_t *context,
    uint32_t *id_ptr,
    uint32_t *vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    config.act = FLUSH_ACT_CLEAR_NEXTHOP;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID
/*
l2-table set flush clear-aggVid
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_aggVid(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_PORT_TRUNK_ID
/*
l2-table set flush clear-aggVid ( port | trunk ) <UINT:id>
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_aggVid_port_trunk_id(cparser_context_t *context,
    uint32_t *id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_VID_VID
/*
l2-table set flush clear-aggVid vid <UINT:vid>
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_aggVid_vid_vid(cparser_context_t *context,
    uint32_t *vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_AGG_VID_AGG_VID
/*
l2-table set flush clear-aggVid agg-vid <UINT:agg_vid>
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_aggVid_agg_vid_agg_vid(cparser_context_t *context,
    uint32_t *agg_vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushFlag |= RTK_L2_FLUSH_BY_AGGVID;
    config.agg_vid = *agg_vid_ptr;

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_PORT_TRUNK_ID_VID_VID
/*
l2-table set flush clear-aggVid ( port | trunk ) <UINT:id> vid <UINT:vid>
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_aggVid_port_trunk_id_vid_vid(cparser_context_t *context,
    uint32_t *id_ptr,
    uint32_t *vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_PORT_TRUNK_ID_AGG_VID_AGG_VID
/*
l2-table set flush clear-aggVid ( port | trunk ) <UINT:id> agg-vid <UINT:agg_vid>
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_aggVid_port_trunk_id_agg_vid_agg_vid(cparser_context_t *context,
    uint32_t *id_ptr,
    uint32_t *agg_vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushFlag |= RTK_L2_FLUSH_BY_AGGVID;
    config.agg_vid = *agg_vid_ptr;

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_VID_VID_AGG_VID_AGG_VID
/*
l2-table set flush clear-aggVid vid <UINT:vid> agg-vid <UINT:agg_vid>
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_aggVid_vid_vid_agg_vid_agg_vid(cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *agg_vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    config.flushFlag |= RTK_L2_FLUSH_BY_AGGVID;
    config.agg_vid = *agg_vid_ptr;

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_PORT_TRUNK_ID_VID_VID_AGG_VID_AGG_VID
/*
l2-table set flush clear-aggVid ( port | trunk ) <UINT:id> vid <UINT:vid> agg-vid <UINT:agg_vid>
*/
cparser_result_t cparser_cmd_l2_table_set_flush_clear_aggVid_port_trunk_id_vid_vid_agg_vid_agg_vid(cparser_context_t *context,
    uint32_t *id_ptr,
    uint32_t *vid_ptr,
    uint32_t *agg_vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &config.devID), ret);
#endif

    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    config.flushFlag |= RTK_L2_FLUSH_BY_AGGVID;
    config.agg_vid = *agg_vid_ptr;

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_LIMIT_LEARNING_GLOBAL
/*
l2-table get limit-learning global
*/
cparser_result_t cparser_cmd_l2_table_get_limit_learning_global(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_macCnt_t limitCnt;
    rtk_l2_macCnt_t limitNum;
    rtk_l2_macLimitAction_t limitAct;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("System Limit Learning Information\n");

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        uint32 val = 0;

        DIAG_UTIL_ERR_CHK(rtk_l2_learningCnt_get(unit, &val), ret);
        limitCnt.glbCnt = val;
    }
    else
#endif
    {
    DIAG_UTIL_ERR_CHK(rtk_l2_macLearningCnt_get(unit, L2_MAC_LIMIT_GLOBAL, &limitCnt), ret);
    }
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCnt_get(unit, &limitNum.glbCnt), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningNum_get(unit, L2_MAC_LIMIT_GLOBAL, &limitNum), ret);
    }
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        rtk_l2_limitLearnCntAction_t act;
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCntAction_get(unit, &act), ret);
        limitAct.glbAct = (rtk_action_t) act;
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_get(unit, L2_MAC_LIMIT_GLOBAL, &limitAct), ret);
    }

    if (limitNum.glbCnt == L2_MAC_CST_DISABLE)
        diag_util_mprintf("\tMax Mac Count : Unlimited\n");
    else
        diag_util_mprintf("\tMax Mac Count : %u\n", limitNum.glbCnt);
    diag_util_mprintf("\tCurrent Mac Count : %u\n", limitCnt.glbCnt);
    diag_util_mprintf("\tExceed Max Count Action : ");

    if (limitAct.glbAct == ACTION_FORWARD)
    {
        diag_util_mprintf("Forward\n");
    }
    else if (limitAct.glbAct ==ACTION_DROP)
    {
        diag_util_mprintf("Drop\n");
    }
    else if (limitAct.glbAct == ACTION_TRAP2CPU)
    {
        diag_util_mprintf("Trap-To-Cpu\n");
    }
    else if (limitAct.glbAct == ACTION_COPY2CPU)
    {
        diag_util_mprintf("Copy-To-Cpu\n");
    }
    else if (limitAct.glbAct == ACTION_TRAP2MASTERCPU)
    {
       diag_util_mprintf("Trap-To Master Cpu\n");
    }
    else if (limitAct.glbAct == ACTION_COPY2MASTERCPU)
    {
       diag_util_mprintf("Copy-To Master Cpu\n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LIMIT_LEARNING_GLOBAL_MAX_COUNT
/*
l2-table set limit-learning global <UINT:max_count>
*/
cparser_result_t cparser_cmd_l2_table_set_limit_learning_global_max_count(cparser_context_t *context,
    uint32_t *max_count_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_l2_macCnt_t limitNum;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == max_count_ptr), CPARSER_ERR_INVALID_PARAMS);

    limitNum.glbCnt = *max_count_ptr;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCnt_set(unit, *max_count_ptr), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningNum_set(unit, L2_MAC_LIMIT_GLOBAL, &limitNum), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LIMIT_LEARNING_GLOBAL_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
l2-table set limit-learning global action ( copy-to-cpu | drop | forward | trap-to-cpu )
*/
cparser_result_t cparser_cmd_l2_table_set_limit_learning_global_action_copy_to_cpu_drop_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_l2_macLimitAction_t limitAct;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('f' == TOKEN_CHAR(5, 0))
    {
        limitAct.glbAct = ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        limitAct.glbAct = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        limitAct.glbAct = ACTION_TRAP2CPU;
    }
    else if ('c' == TOKEN_CHAR(5, 0))
    {
        limitAct.glbAct = ACTION_COPY2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        rtk_l2_limitLearnCntAction_t    act;
        act = (rtk_l2_limitLearnCntAction_t) limitAct.glbAct;
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCntAction_set(unit, act), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_set(unit, L2_MAC_LIMIT_GLOBAL, &limitAct), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LIMIT_LEARNING_GLOBAL_ACTION_COPY_TO_MASTER_TRAP_TO_MASTER
/*
l2-table set limit-learning global action ( copy-to-master | trap-to-master )
*/
cparser_result_t cparser_cmd_l2_table_set_limit_learning_global_action_copy_to_master_trap_to_master(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_l2_macLimitAction_t limitAct;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('c' == TOKEN_CHAR(5, 0))
    {
        limitAct.glbAct = ACTION_COPY2MASTERCPU;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        limitAct.glbAct = ACTION_TRAP2MASTERCPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        rtk_l2_limitLearnCntAction_t    act;
        act = (rtk_l2_limitLearnCntAction_t) limitAct.glbAct;
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCntAction_set(unit, act), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_set(unit, L2_MAC_LIMIT_GLOBAL, &limitAct), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_LIMIT_LEARNING_PORT_TRUNK_PORT_TRUNK_ALL
/*
l2-table get limit-learning ( port | trunk ) ( <MASK_LIST:port_trunk> | all )
*/
cparser_result_t cparser_cmd_l2_table_get_limit_learning_port_trunk_port_trunk_all(cparser_context_t *context,
    char **port_trunk_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    diag_portlist_t               portlist;
    rtk_port_t                     port = 0;
    diag_trklist_t                trklist;
    rtk_trk_t                      trunk = 0;

    rtk_l2_macCnt_t limitCnt;
    rtk_l2_macCnt_t limitNum;
    rtk_l2_macLimitAction_t limitAct;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == *port_trunk_ptr), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    if ('t' == TOKEN_CHAR(3, 0))
    {
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("trunk-based configuration is not supported in this chip\n");
            return CPARSER_OK;
        }
    }

    diag_util_mprintf("Port Limit Learning Information:\n");

    if ('p' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            diag_util_mprintf("    Port %2d :\n", port);
            limitCnt.portTrkCnt.id = port;
            limitNum.portTrkCnt.id = port;
            limitAct.portTrkAct.id = port;

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_portLearningCnt_get(unit, port, &limitCnt.portTrkCnt.cnt), ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_macLearningCnt_get(unit, L2_MAC_LIMIT_PORT, &limitCnt), ret);
            }
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCnt_get(unit, port, &limitNum.portTrkCnt.cnt), ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningNum_get(unit, L2_MAC_LIMIT_PORT, &limitNum), ret);
            }
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                rtk_l2_limitLearnCntAction_t act;
                DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCntAction_get(unit, port, &act), ret);
                limitAct.portTrkAct.act = (rtk_action_t)act;
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_get(unit, L2_MAC_LIMIT_PORT, &limitAct), ret);
            }

            if (limitNum.portTrkCnt.cnt == L2_MAC_CST_DISABLE)
                diag_util_mprintf("        Max Mac Count           : Unlimited\n");
            else
                diag_util_mprintf("        Max Mac Count           : %u\n", limitNum.portTrkCnt.cnt);

            diag_util_mprintf("        Current Mac Count       : %u\n", limitCnt.portTrkCnt.cnt);

            diag_util_mprintf("        Exceed Max Count Action : ");

            if (limitAct.portTrkAct.act == ACTION_FORWARD)
            {
                diag_util_mprintf("Forward\n");
            }
            else if (limitAct.portTrkAct.act == ACTION_DROP)
            {
                diag_util_mprintf("Drop\n");
            }
            else if (limitAct.portTrkAct.act == ACTION_TRAP2CPU)
            {
                diag_util_mprintf("Trap-To-Cpu\n");
            }
            else if (limitAct.portTrkAct.act == ACTION_COPY2CPU)
            {
                diag_util_mprintf("Copy-To-Cpu\n");
            }
            else if (limitAct.portTrkAct.act == ACTION_TRAP2MASTERCPU)
            {
               diag_util_mprintf("Trap-To Master Cpu\n");
            }
            else if (limitAct.portTrkAct.act == ACTION_COPY2MASTERCPU)
            {
               diag_util_mprintf("Copy-To Master Cpu\n");
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }

            diag_util_mprintf("\n");
        }
    }
    else if ('t' == TOKEN_CHAR(3, 0))
    {
        if (IS_BACKWARD_COMPATIBLE)
        {
            diag_util_mprintf("trunk-based configuration is not supported in backward compatible mode\n");
            return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

        DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
        {
            diag_util_mprintf("    trunk %2d :\n", trunk);
            limitCnt.portTrkCnt.id = trunk;
            limitNum.portTrkCnt.id = trunk;
            limitAct.portTrkAct.id = trunk;

            DIAG_UTIL_ERR_CHK(rtk_l2_macLearningCnt_get(unit, L2_MAC_LIMIT_TRUNK, &limitCnt), ret);
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningNum_get(unit, L2_MAC_LIMIT_TRUNK, &limitNum), ret);
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_get(unit, L2_MAC_LIMIT_TRUNK, &limitAct), ret);

            if (limitNum.portTrkCnt.cnt == L2_MAC_CST_DISABLE)
                diag_util_mprintf("        Max Mac Count           : Unlimited\n");
            else
                diag_util_mprintf("        Max Mac Count           : %u\n", limitNum.portTrkCnt.cnt);

            diag_util_mprintf("        Current Mac Count       : %u\n", limitCnt.portTrkCnt.cnt);

            diag_util_mprintf("        Exceed Max Count Action : ");

            if (limitAct.portTrkAct.act == ACTION_FORWARD)
            {
                diag_util_mprintf("Forward\n");
            }
            else if (limitAct.portTrkAct.act == ACTION_DROP)
            {
                diag_util_mprintf("Drop\n");
            }
            else if (limitAct.portTrkAct.act == ACTION_TRAP2CPU)
            {
                diag_util_mprintf("Trap-To-Cpu\n");
            }
            else if (limitAct.portTrkAct.act == ACTION_COPY2CPU)
            {
                diag_util_mprintf("Copy-To-Cpu\n");
            }
            else if (limitAct.portTrkAct.act == ACTION_TRAP2MASTERCPU)
            {
               diag_util_mprintf("Trap-To Master Cpu\n");
            }
            else if (limitAct.portTrkAct.act == ACTION_COPY2MASTERCPU)
            {
               diag_util_mprintf("Copy-To Master Cpu\n");
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }

            diag_util_mprintf("\n");
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LIMIT_LEARNING_PORT_TRUNK_PORT_TRUNK_ALL_MAX_COUNT
/*
l2-table set limit-learning ( port | trunk ) ( <MASK_LIST:port_trunk> | all ) <UINT:max_count>
*/
cparser_result_t cparser_cmd_l2_table_set_limit_learning_port_trunk_port_trunk_all_max_count(cparser_context_t *context,
    char **port_trunk_ptr,
    uint32_t *max_count_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    diag_portlist_t portlist;
    rtk_trk_t         trunk = 0;
    diag_trklist_t      trklist;
    rtk_l2_macCnt_t limitNum;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == max_count_ptr), CPARSER_ERR_INVALID_PARAMS);

    if ('t' == TOKEN_CHAR(3, 0))
    {
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("trunk is not supported in this chip\n");
            return CPARSER_NOT_OK;
        }
    }
    limitNum.portTrkCnt.cnt = *max_count_ptr;

    if ('p' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            limitNum.portTrkCnt.id = port;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCnt_set(unit, port, limitNum.portTrkCnt.cnt), ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningNum_set(unit, L2_MAC_LIMIT_PORT, &limitNum), ret);
            }
        }
    }
    else if ('t' == TOKEN_CHAR(3, 0))
    {
        if (IS_BACKWARD_COMPATIBLE)
        {
            diag_util_mprintf("trunk-based configuration is not supported in backward compatible mode\n");
            return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

        DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
        {
            limitNum.portTrkCnt.id = trunk;
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningNum_set(unit, L2_MAC_LIMIT_TRUNK, &limitNum), ret);
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LIMIT_LEARNING_PORT_TRUNK_PORT_TRUNK_ALL_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
l2-table set limit-learning ( port | trunk ) ( <MASK_LIST:port_trunk> | all ) action ( copy-to-cpu | drop | forward | trap-to-cpu )
*/
cparser_result_t cparser_cmd_l2_table_set_limit_learning_port_trunk_port_trunk_all_action_copy_to_cpu_drop_forward_trap_to_cpu(cparser_context_t *context,
    char **port_trunk_ptr)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_port_t                      port = 0;
    rtk_l2_macLimitAction_t limitAct;
    diag_portlist_t             portlist;
    rtk_trk_t         trunk = 0;
    diag_trklist_t      trklist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if ('t' == TOKEN_CHAR(3, 0))
    {
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("trunk is not supported in this chip\n");
            return CPARSER_NOT_OK;
        }
    }

    if ('f' == TOKEN_CHAR(6, 0))
    {
        limitAct.portTrkAct.act = ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(6, 0))
    {
        limitAct.portTrkAct.act = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        limitAct.portTrkAct.act = ACTION_TRAP2CPU;
    }
    else if ('c' == TOKEN_CHAR(6, 0))
    {
        limitAct.portTrkAct.act = ACTION_COPY2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('p' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            limitAct.portTrkAct.id = port;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                rtk_l2_limitLearnCntAction_t act;

                act = (rtk_l2_limitLearnCntAction_t)limitAct.portTrkAct.act;
                DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCntAction_set(unit, port, act), ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_set(unit, L2_MAC_LIMIT_PORT, &limitAct), ret);
            }
        }
    }
    else if ('t' == TOKEN_CHAR(3, 0))
    {
        if (IS_BACKWARD_COMPATIBLE)
        {
            diag_util_mprintf("trunk-based configuration is not supported in backward compatible mode\n");
            return CPARSER_NOT_OK;
        }

        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("trunk is not supported in this chip\n");
            return CPARSER_OK;
        }

        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

        DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
        {
            limitAct.portTrkAct.id = trunk;
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_set(unit, L2_MAC_LIMIT_TRUNK, &limitAct), ret);
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LIMIT_LEARNING_PORT_TRUNK_PORT_TRUNK_ALL_ACTION_COPY_TO_MASTER_TRAP_TO_MASTER
/*
l2-table set limit-learning ( port | trunk ) ( <MASK_LIST:port_trunk> | all ) action ( copy-to-master | trap-to-master )
*/
cparser_result_t cparser_cmd_l2_table_set_limit_learning_port_trunk_port_trunk_all_action_copy_to_master_trap_to_master(cparser_context_t *context,
    char **port_trunk_ptr)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_port_t                      port = 0;
    rtk_l2_macLimitAction_t limitAct;
    diag_portlist_t             portlist;
    rtk_trk_t         trunk = 0;
    diag_trklist_t      trklist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if ('t' == TOKEN_CHAR(3, 0))
    {
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("trunk is not supported in this chip\n");
            return CPARSER_NOT_OK;
        }
    }

    if ('c' == TOKEN_CHAR(6, 0))
    {
        limitAct.portTrkAct.act = ACTION_COPY2MASTERCPU;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        limitAct.portTrkAct.act = ACTION_TRAP2MASTERCPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('p' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            limitAct.portTrkAct.id = port;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
            if (IS_BACKWARD_COMPATIBLE)
            {
                rtk_l2_limitLearnCntAction_t act;

                act = (rtk_l2_limitLearnCntAction_t)limitAct.portTrkAct.act;
                DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCntAction_set(unit, port, act), ret);
            }
            else
#endif
            {
                DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_set(unit, L2_MAC_LIMIT_PORT, &limitAct), ret);
            }
        }
    }
    else if ('t' == TOKEN_CHAR(3, 0))
    {
        if (IS_BACKWARD_COMPATIBLE)
        {
            diag_util_mprintf("trunk-based configuration is not supported in backward compatible mode\n");
            return CPARSER_NOT_OK;
        }

        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);

        DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
        {
            limitAct.portTrkAct.id = trunk;
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_set(unit, L2_MAC_LIMIT_TRUNK, &limitAct), ret);
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_LIMIT_LEARNING_VLAN_BASED_INDEX
/*
l2-table get limit-learning vlan-based <UINT:index>
*/
cparser_result_t cparser_cmd_l2_table_get_limit_learning_vlan_based_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                      unit = 0;
    int32                       ret = RT_ERR_FAILED;
    rtk_l2_fidMacLimitEntry_t   limitEntry;
    rtk_l2_macCnt_t             limitCnt;
    rtk_l2_macLimitAction_t     limitAct;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == index_ptr), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("VLAN-based Limit Learning Information \n");

    diag_util_mprintf("\tIndex %2d :\n", *index_ptr);

    DIAG_UTIL_ERR_CHK(rtk_l2_fidLimitLearningEntry_get(unit, *index_ptr, &limitEntry), ret);

    diag_util_mprintf("\tConstraint VID : %4d\n", limitEntry.fid);

#if defined(CONFIG_SDK_RTL9300)  || defined(CONFIG_SDK_RTL9310)
    if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if(limitEntry.portOrTrunk == ENABLED)
        {
            if (
                    (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) && limitEntry.port == 0x1f) ||
                    (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) && limitEntry.port == 0x3f)
                )
                diag_util_mprintf("\tConstraint Port : %s\n", "all");
            else
                diag_util_mprintf("\tConstraint Port : %2d\n", limitEntry.port);
        }
        else
        {
            diag_util_mprintf("\tConstraint trunk : %2d\n", limitEntry.trunk);
        }
    }
#endif

    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
            if (
                    (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) && limitEntry.port == 0x1f) ||
                    (DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) && limitEntry.port == 0x1f) ||
                    (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) && limitEntry.port == 0x3f) ||
                    (DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) && limitEntry.port == 0x3f)
                )
                diag_util_mprintf("\tConstraint Port : %s\n", "all");
            else
                diag_util_mprintf("\tConstraint Port : %2d\n", limitEntry.port);
    }

    if (limitEntry.maxNum == L2_MAC_CST_DISABLE)
        diag_util_mprintf("\tMax Mac Count : Unlimited\n");
    else
        diag_util_mprintf("\tMax Mac Count : %u\n", limitEntry.maxNum);

    limitCnt.fidCnt.entryId = *index_ptr;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        uint32 val = 0;

        DIAG_UTIL_ERR_CHK(rtk_l2_fidLearningCnt_get(unit, *index_ptr, &val), ret);
        limitCnt.fidCnt.cnt = val;
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_macLearningCnt_get(unit, L2_MAC_LIMIT_FID, &limitCnt), ret);
    }
    diag_util_mprintf("\tCurrent Mac Count : %u\n", limitCnt.fidCnt.cnt);

    diag_util_mprintf("\tExceed Max Count Action : ");
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        rtk_l2_limitLearnCntAction_t   act;
        DIAG_UTIL_ERR_CHK(rtk_l2_fidLearningCntAction_get(unit, &act), ret);
        limitAct.fidAct.act = (rtk_action_t)act;
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_get(unit, L2_MAC_LIMIT_FID, &limitAct), ret);
    }

    if (limitAct.fidAct.act == ACTION_FORWARD)
    {
        diag_util_mprintf("Forward\n");
    }
    else if (limitAct.fidAct.act ==ACTION_DROP)
    {
        diag_util_mprintf("Drop\n");
    }
    else if (limitAct.fidAct.act == ACTION_TRAP2CPU)
    {
        diag_util_mprintf("Trap-To-Cpu\n");
    }
    else if (limitAct.fidAct.act == ACTION_COPY2CPU)
    {
        diag_util_mprintf("Copy-To-Cpu\n");
    }
    else if (limitAct.fidAct.act == ACTION_TRAP2MASTERCPU)
    {
       diag_util_mprintf("Trap-To Master Cpu\n");
    }
    else if (limitAct.fidAct.act == ACTION_COPY2MASTERCPU)
    {
       diag_util_mprintf("Copy-To Master Cpu\n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LIMIT_LEARNING_VLAN_BASED_INDEX_VID_VID_MAX_COUNT
/*
l2-table set limit-learning vlan-based <UINT:index> vid <UINT:vid> <UINT:max_count>
*/
cparser_result_t cparser_cmd_l2_table_set_limit_learning_vlan_based_index_vid_vid_max_count(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr,
    uint32_t *max_count_ptr)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_l2_fidMacLimitEntry_t       entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    entry.fid = *vid_ptr;
    entry.maxNum = *max_count_ptr;

#if defined(CONFIG_SDK_RTL9300)  || defined(CONFIG_SDK_RTL9310)
    if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        entry.portOrTrunk = ENABLED;
        entry.port = 0x3f;

    }
#endif

    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            entry.port = 0x1f;
        else
            entry.port = 0x3f;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_fidLimitLearningEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LIMIT_LEARNING_VLAN_BASED_INDEX_VID_VID_PORT_TRUNK_ID_MAX_COUNT
/*
l2-table set limit-learning vlan-based <UINT:index> vid <UINT:vid> ( port | trunk ) <UINT:id> <UINT:max_count>
*/
cparser_result_t cparser_cmd_l2_table_set_limit_learning_vlan_based_index_vid_vid_port_trunk_id_max_count(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *vid_ptr,
    uint32_t *id_ptr,
    uint32_t *max_count_ptr)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_l2_fidMacLimitEntry_t       entry;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('t' == TOKEN_CHAR(7, 0))
    {
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("trunk is not supported in this chip\n");
            return CPARSER_NOT_OK;
        }
    }

    entry.fid = *vid_ptr;
    entry.maxNum = *max_count_ptr;

#if defined(CONFIG_SDK_RTL9300)  || defined(CONFIG_SDK_RTL9310)
    if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if ('p' == TOKEN_CHAR(7, 0))
        {
            entry.portOrTrunk = ENABLED;
            entry.port = *id_ptr;
        }
        else if ('t' == TOKEN_CHAR(7, 0))
        {
            entry.portOrTrunk = DISABLED;
            entry.trunk = *id_ptr;
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        entry.port = *id_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_fidLimitLearningEntry_set(unit, *index_ptr, &entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LIMIT_LEARNING_VLAN_BASED_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
l2-table set limit-learning vlan-based action ( copy-to-cpu | drop | forward | trap-to-cpu )
*/
cparser_result_t cparser_cmd_l2_table_set_limit_learning_vlan_based_action_copy_to_cpu_drop_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_l2_macLimitAction_t limitAct;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('f' == TOKEN_CHAR(5, 0))
    {
        limitAct.fidAct.act = ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        limitAct.fidAct.act = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        limitAct.fidAct.act = ACTION_TRAP2CPU;
    }
    else if ('c' == TOKEN_CHAR(5, 0))
    {
        limitAct.fidAct.act = ACTION_COPY2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        rtk_l2_limitLearnCntAction_t   act;
        act = (rtk_l2_limitLearnCntAction_t)limitAct.fidAct.act;
        DIAG_UTIL_ERR_CHK(rtk_l2_fidLearningCntAction_set(unit, act), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_set(unit, L2_MAC_LIMIT_FID, &limitAct), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LIMIT_LEARNING_VLAN_BASED_ACTION_COPY_TO_MASTER_TRAP_TO_MASTER
/*
 * l2-table set limit-learning vlan-based action ( copy-to-master | trap-to-master )
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_vlan_based_action_copy_to_master_trap_to_master(
    cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_l2_macLimitAction_t limitAct;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('c' == TOKEN_CHAR(5, 0))
    {
        limitAct.fidAct.act = ACTION_COPY2MASTERCPU;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        limitAct.fidAct.act = ACTION_TRAP2MASTERCPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        rtk_l2_limitLearnCntAction_t   act;
        act = (rtk_l2_limitLearnCntAction_t)limitAct.fidAct.act;
        DIAG_UTIL_ERR_CHK(rtk_l2_fidLearningCntAction_set(unit, act), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningAction_set(unit, L2_MAC_LIMIT_FID, &limitAct), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_limit_learning_vlan_based_action_copy_to_master_trap_to_master */
#endif

#ifdef CMD_L2_TABLE_RESET_LIMIT_LEARNING_VLAN_BASED_INDEX
/*
l2-table reset limit-learning vlan-based <UINT:index>
*/
cparser_result_t cparser_cmd_l2_table_reset_limit_learning_vlan_based_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_l2_fidLearningCnt_reset(unit, *index_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_AGE_AGING_TIME_AUTO
/*
l2-table get age aging-time auto
*/
cparser_result_t cparser_cmd_l2_table_get_age_aging_time_auto(cparser_context_t *context)
{
    uint32   unit = 0;
    uint32   aging_time = 0;
    int32    ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("\tAuto-Learned entry Aging Time                                     : ");
    DIAG_UTIL_ERR_CHK(rtk_l2_agingTime_get(unit, L2_AGE_TIME_NORMAL, &aging_time), ret);

    diag_util_mprintf("%d seconds.\n", aging_time);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_AGE_AGING_TIME_AUTO_TIME
/*
l2-table set age aging-time auto <UINT:time>
*/
cparser_result_t cparser_cmd_l2_table_set_age_aging_time_auto_time(cparser_context_t *context,
    uint32_t *time_ptr)
{
    uint32   unit = 0;;
    int32    ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_aging_set(unit, *time_ptr), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_agingTime_set(unit, L2_AGE_TIME_NORMAL, *time_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_AGE_AGING_TIME_SUSPEND
/*
l2-table get age aging-time suspend
*/
cparser_result_t cparser_cmd_l2_table_get_age_aging_time_suspend(cparser_context_t *context)
{
    uint32   unit = 0;
    uint32   aging_time = 0;
    int32    ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("\tSuspend entry Aging Time                                     : ");
    DIAG_UTIL_ERR_CHK(rtk_l2_agingTime_get(unit, L2_AGE_TIME_SUSPEND, &aging_time), ret);

    diag_util_mprintf("%d seconds.\n", aging_time);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_AGE_AGING_TIME_SUSPEND_TIME
/*
l2-table set age aging-time suspend <UINT:time>
*/
cparser_result_t cparser_cmd_l2_table_set_age_aging_time_suspend_time(cparser_context_t *context,
    uint32_t *time_ptr)
{
    uint32   unit = 0;;
    int32    ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        diag_util_mprintf("aging-time configuration for suspend entry is not supported in backward compatible mode\n");
        return CPARSER_NOT_OK;
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_agingTime_set(unit, L2_AGE_TIME_SUSPEND, *time_ptr), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_AGE_AGING_OUT_PORT_TRUNK_PORT_TRUNK_ALL_STATE
/*
l2-table get age aging-out ( port | trunk ) ( <MASK_LIST:port_trunk> | all ) state
*/
cparser_result_t cparser_cmd_l2_table_get_age_aging_out_port_trunk_port_trunk_all_state(cparser_context_t *context,
    char **port_trunk_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t        enable = DISABLED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    rtk_trk_t         trunk = 0;
    diag_trklist_t      trklist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ('t' == TOKEN_CHAR(4, 0))
    {
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("trunk is not supported in this chip\n");
            return CPARSER_NOT_OK;
        }
    }

    if ('p' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_portAgingEnable_get(unit, port, &enable), ret);
            diag_util_mprintf("\tPort %d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }
    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 5), ret);

        DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_trkAgingEnable_get(unit, trunk, &enable), ret);
            diag_util_mprintf("\tTrunk %d : %s\n", trunk, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_AGE_AGING_OUT_PORT_TRUNK_PORT_TRUNK_ALL_STATE_DISABLE_ENABLE
/*
l2-table set age aging-out ( port | trunk ) ( <MASK_LIST:port_trunk> | all ) state ( disable | enable )
*/
cparser_result_t cparser_cmd_l2_table_set_age_aging_out_port_trunk_port_trunk_all_state_disable_enable(cparser_context_t *context,
    char **port_trunk_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    rtk_enable_t enable;
    rtk_trk_t         trunk = 0;
    diag_trklist_t      trklist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else if('d' == TOKEN_CHAR(7, 0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('p' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_portAgingEnable_set(unit, port, enable), ret);
        }
    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 5), ret);

        DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_trkAgingEnable_set(unit, trunk, enable), ret);
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_HASH_ALGORITHM_BLOCK_ID
/*
l2-table get hash-algorithm <UINT:block_id>
*/
cparser_result_t cparser_cmd_l2_table_get_hash_algorithm_block_id(cparser_context_t *context,
    uint32_t *block_id_ptr)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    uint32                           val = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_bucketHashAlgo_get(unit, *block_id_ptr, &val), ret);

    diag_util_mprintf("\tL2 Table Hash Algorithm blk%1d                      : ", *block_id_ptr);
    if (val == 0)
    {
        diag_util_mprintf("ALGO-0\n");
    }
    else
    {
        diag_util_mprintf("ALGO-1\n");
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_HASH_ALGORITHM_BLOCK_ID_ALGO0_ALGO1
/*
l2-table set hash-algorithm <UINT:block_id> ( algo0 | algo1 )
*/
cparser_result_t cparser_cmd_l2_table_set_hash_algorithm_block_id_algo0_algo1(cparser_context_t *context,
    uint32_t *block_id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    uint32                          hashType = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('0' == TOKEN_CHAR(4, 4))
        hashType = 0;
    else if('1' == TOKEN_CHAR(4, 4))
        hashType = 1;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_bucketHashAlgo_set(unit, *block_id_ptr, hashType), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_LEARNING_FULL_ACTION
/*
l2-table get learning-full action
*/
cparser_result_t cparser_cmd_l2_table_get_learning_full_action(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_action_t action;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_l2_learningFullAction_get(unit, &action), ret);

    diag_util_mprintf("\tLearn Full Action : ");

    if (action == ACTION_FORWARD)
    {
        diag_util_mprintf("Forward\n");
    }
    else if (action ==ACTION_DROP)
    {
        diag_util_mprintf("Drop\n");
    }
    else if (action == ACTION_TRAP2CPU)
    {
        diag_util_mprintf("Trap-To-Cpu\n");
    }
    else if (action == ACTION_COPY2CPU)
    {
        diag_util_mprintf("Copy-To-Cpu\n");
    }
    else if (action == ACTION_TRAP2MASTERCPU)
    {
       diag_util_mprintf("Trap-To Master Cpu\n");
    }
    else if (action == ACTION_COPY2MASTERCPU)
    {
       diag_util_mprintf("Copy-To Master Cpu\n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LEARNING_FULL_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
l2-table set learning-full action ( copy-to-cpu | drop | forward | trap-to-cpu )
*/
cparser_result_t cparser_cmd_l2_table_set_learning_full_action_copy_to_cpu_drop_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_action_t action;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('f' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_TRAP2CPU;
    }
    else if ('c' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_COPY2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_learningFullAction_set(unit, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LEARNING_FULL_ACTION_COPY_TO_MASTER_TRAP_TO_MASTER
/*
l2-table set learning-full action ( copy-to-master | trap-to-master )
*/
cparser_result_t cparser_cmd_l2_table_set_learning_full_action_copy_to_master_trap_to_master(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_action_t action;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('c' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_COPY2MASTERCPU;
    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        action = ACTION_TRAP2MASTERCPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_learningFullAction_set(unit, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_SRC_MAC_PORT_PORT_ALL
/*
l2-table get src-mac port ( <PORT_LIST:port> | all )
*/
cparser_result_t cparser_cmd_l2_table_get_src_mac_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_port_t                      port = 0;
    rtk_l2_newMacLrnMode_t          lrnMode;
    rtk_action_t                    fwdAction;
    diag_portlist_t                 portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %d :\n", port);

        ret = rtk_l2_portNewMacOp_get(unit, port, &lrnMode, &fwdAction);
        if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
           DIAG_ERR_PRINT(ret);
           return CPARSER_NOT_OK;
        }
        else
        {
            diag_util_mprintf("\tNew Src Mac Operation             : ");
            if (lrnMode == HARDWARE_LEARNING)
            {
               diag_util_mprintf("Asic-AUTO-Learn\n");
            }
            else if(lrnMode == SOFTWARE_LEARNING)
            {
               diag_util_mprintf("Learn-As-SUSPEND\n");
            }
            else if(lrnMode == NOT_LEARNING)
            {
               diag_util_mprintf("Not-Learn\n");
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }

            diag_util_mprintf("\tPacket Action                     : ");
            if (fwdAction == ACTION_FORWARD)
            {
               diag_util_mprintf("Forward\n");
            }
            else if(fwdAction == ACTION_COPY2CPU)
            {
               diag_util_mprintf("Copy-To-Cpu\n ");
            }
            else if(fwdAction == ACTION_TRAP2CPU)
            {
               diag_util_mprintf("Trap-To Cpu\n");
            }
            else if(fwdAction == ACTION_DROP)
            {
               diag_util_mprintf("Drop\n");
            }
            else if (fwdAction == ACTION_TRAP2MASTERCPU)
            {
               diag_util_mprintf("Trap-To Master Cpu\n");
            }
            else if (fwdAction == ACTION_COPY2MASTERCPU)
            {
               diag_util_mprintf("Copy-To Master Cpu\n");
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
       }
    }
     return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_SRC_MAC_PORT_PORT_ALL_LEARN_MODE_ASIC_LEARN_SOFTWARE_LEARN_NOT_LEARN_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
l2-table set src-mac port ( <PORT_LIST:port> | all ) learn-mode ( asic-learn | software-learn | not-learn ) action ( copy-to-cpu | drop | forward | trap-to-cpu )
*/
cparser_result_t cparser_cmd_l2_table_set_src_mac_port_port_all_learn_mode_asic_learn_software_learn_not_learn_action_copy_to_cpu_drop_forward_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_port_t                      port = 0;
    rtk_l2_newMacLrnMode_t  mode  = HARDWARE_LEARNING;
    rtk_action_t                  action = ACTION_FORWARD;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('a' == TOKEN_CHAR(6, 0))
        mode = HARDWARE_LEARNING;
    else if('s' == TOKEN_CHAR(6, 0))
        mode = SOFTWARE_LEARNING;
    else if('n' == TOKEN_CHAR(6, 0))
        mode = NOT_LEARNING;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('f' == TOKEN_CHAR(8, 0))
    {
        action = ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(8, 0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(8, 0))
    {
        action = ACTION_TRAP2CPU;
    }
    else if ('c' == TOKEN_CHAR(8, 0))
    {
        action = ACTION_COPY2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portNewMacOp_set(unit, port, mode, action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_SRC_MAC_PORT_PORT_ALL_LEARN_MODE_ASIC_LEARN_SOFTWARE_LEARN_NOT_LEARN_ACTION_COPY_TO_MASTER_TRAP_TO_MASTER
/*
l2-table set src-mac port ( <PORT_LIST:port> | all ) learn-mode ( asic-learn | software-learn | not-learn ) action ( copy-to-master | trap-to-master )
*/
cparser_result_t cparser_cmd_l2_table_set_src_mac_port_port_all_learn_mode_asic_learn_software_learn_not_learn_action_copy_to_master_trap_to_master(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_t              port = 0;
    rtk_l2_newMacLrnMode_t  mode  = HARDWARE_LEARNING;
    rtk_action_t            action = ACTION_FORWARD;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('a' == TOKEN_CHAR(6, 0))
        mode = HARDWARE_LEARNING;
    else if('s' == TOKEN_CHAR(6, 0))
        mode = SOFTWARE_LEARNING;
    else if('n' == TOKEN_CHAR(6, 0))
        mode = NOT_LEARNING;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('c' == TOKEN_CHAR(8, 0))
    {
        action = ACTION_COPY2MASTERCPU;
    }
    else if ('t' == TOKEN_CHAR(8, 0))
    {
        action = ACTION_TRAP2MASTERCPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portNewMacOp_set(unit, port, mode, action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_PORT_MOVE_DYNAMIC_PORT_MOVE_PORT_PORT_ALL
/*
l2-table get port-move dynamic-port-move port ( <PORT_LIST:port> | all )
*/
cparser_result_t cparser_cmd_l2_table_get_port_move_dynamic_port_move_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    rtk_l2_portMoveAct_t act;
    rtk_l2_portMoveLrn_t learn;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %u :          \n", port);

        act.dynAct.port = port;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_legalPortMoveAction_get(unit, port, &act.dynAct.act), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portMoveAction_get(unit, L2_PORT_MOVE_DYNAMIC, &act), ret);
    }

        diag_util_mprintf("\tAction:              ");
        if (act.dynAct.act == ACTION_FORWARD)
        {
            diag_util_mprintf("Forward\n");
        }
        else if (act.dynAct.act  == ACTION_DROP)
        {
            diag_util_mprintf("Drop\n");
        }
        else if (act.dynAct.act  == ACTION_TRAP2CPU)
        {
            diag_util_mprintf("Trap-To-Cpu\n");
        }
        else if (act.dynAct.act  == ACTION_COPY2CPU)
        {
            diag_util_mprintf("Copy-To-Cpu\n");
        }
        else if (act.dynAct.act == ACTION_TRAP2MASTERCPU)
        {
           diag_util_mprintf("Trap-To Master Cpu\n");
        }
        else if (act.dynAct.act == ACTION_COPY2MASTERCPU)
        {
           diag_util_mprintf("Copy-To Master Cpu\n");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            learn.dynLrn.port = port;
            DIAG_UTIL_ERR_CHK(rtk_l2_portMoveLearn_get(unit, L2_PORT_MOVE_DYNAMIC, &learn), ret);

            diag_util_mprintf("\tLearn:              ");
            if (learn.dynLrn.enable == ENABLED)
            {
                diag_util_mprintf("enable-learn\n");
            }
            else if (learn.dynLrn.enable == DISABLED)
            {
                diag_util_mprintf("disable-learn\n");
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
        }
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_PORT_MOVE_DYNAMIC_PORT_MOVE_PORT_PORT_ALL_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
l2-table set port-move dynamic-port-move port ( <PORT_LIST:port> | all ) action ( copy-to-cpu | drop | forward | trap-to-cpu )
*/
cparser_result_t cparser_cmd_l2_table_set_port_move_dynamic_port_move_port_port_all_action_copy_to_cpu_drop_forward_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_l2_portMoveAct_t act;
    diag_portlist_t     portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ('f' == TOKEN_CHAR(7, 0))
    {
        act.dynAct.act  = ACTION_FORWARD;
    }
    else if('d' == TOKEN_CHAR(7, 0))
    {
        act.dynAct.act  = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(7, 0))
    {
        act.dynAct.act  = ACTION_TRAP2CPU;
    }
    else if ('c' == TOKEN_CHAR(7, 0))
    {
        act.dynAct.act  = ACTION_COPY2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        act.dynAct.port = port;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_legalPortMoveAction_set(unit, port, act.dynAct.act), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portMoveAction_set(unit, L2_PORT_MOVE_DYNAMIC, &act), ret);
    }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_PORT_MOVE_DYNAMIC_PORT_MOVE_PORT_PORT_ALL_ACTION_COPY_TO_MASTER_TRAP_TO_MASTER
/*
l2-table set port-move dynamic-port-move port ( <PORT_LIST:port> | all ) action ( copy-to-master | trap-to-master )
*/
cparser_result_t cparser_cmd_l2_table_set_port_move_dynamic_port_move_port_port_all_action_copy_to_master_trap_to_master(cparser_context_t *context,
    char **port_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_port_t              port = 0;
    rtk_l2_portMoveAct_t    act;
    diag_portlist_t         portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ('c' == TOKEN_CHAR(7, 0))
    {
        act.dynAct.act  = ACTION_COPY2MASTERCPU;
    }
    else if ('t' == TOKEN_CHAR(7, 0))
    {
        act.dynAct.act  = ACTION_TRAP2MASTERCPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        act.dynAct.port = port;
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_legalPortMoveAction_set(unit, port, act.dynAct.act), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portMoveAction_set(unit, L2_PORT_MOVE_DYNAMIC, &act), ret);
    }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_PORT_MOVE_DYNAMIC_PORT_MOVE_PORT_PORT_ALL_LEARN_STATE_DISABLE_ENABLE
/*
l2-table set port-move dynamic-port-move port ( <PORT_LIST:port> | all ) learn state  ( disable | enable )
*/
cparser_result_t cparser_cmd_l2_table_set_port_move_dynamic_port_move_port_port_all_learn_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_l2_portMoveLrn_t learn;
    diag_portlist_t     portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(8, 0))
    {
        learn.dynLrn.enable = ENABLED;
    }
    else if('d' == TOKEN_CHAR(8, 0))
    {
        learn.dynLrn.enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        learn.dynLrn.port = port;
        DIAG_UTIL_ERR_CHK(rtk_l2_portMoveLearn_set(unit, L2_PORT_MOVE_DYNAMIC, &learn), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_PORT_MOVE_STTC_PORT_MOVE
/*
l2-table get port-move sttc-port-move
*/
cparser_result_t cparser_cmd_l2_table_get_port_move_sttc_port_move(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_portMoveAct_t act;
    rtk_l2_portMoveLrn_t learn;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        diag_util_mprintf("global sttc-port-move configuration is not supported in backward compatible mode\n");
        return CPARSER_NOT_OK;
    }
    else
#endif
    {
    DIAG_UTIL_ERR_CHK(rtk_l2_portMoveAction_get(unit, L2_PORT_MOVE_STATIC, &act), ret);
    }

    diag_util_mprintf("\tAction:              ");
    if (act.sttAct.act == ACTION_FORWARD)
    {
        diag_util_mprintf("Forward\n");
    }
    else if (act.sttAct.act  == ACTION_DROP)
    {
        diag_util_mprintf("Drop\n");
    }
    else if (act.sttAct.act  == ACTION_TRAP2CPU)
    {
        diag_util_mprintf("Trap-To-Cpu\n");
    }
    else if (act.sttAct.act  == ACTION_COPY2CPU)
    {
        diag_util_mprintf("Copy-To-Cpu\n");
    }
    else if (act.sttAct.act  == ACTION_TRAP2MASTERCPU)
    {
        diag_util_mprintf("Trap-To-Master Cpu\n");
    }
    else if (act.sttAct.act  == ACTION_COPY2MASTERCPU)
    {
        diag_util_mprintf("Copy-To-Master Cpu\n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portMoveLearn_get(unit, L2_PORT_MOVE_STATIC, &learn), ret);

        diag_util_mprintf("\tLearn:              ");
        if (learn.sttLrn.enable == ENABLED)
        {
            diag_util_mprintf("enable-learn\n");
        }
        else if (learn.sttLrn.enable == DISABLED)
        {
            diag_util_mprintf("disable-learn\n");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_PORT_MOVE_STTC_PORT_MOVE_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
l2-table set port-move sttc-port-move action ( copy-to-cpu | drop | forward | trap-to-cpu )
*/
cparser_result_t cparser_cmd_l2_table_set_port_move_sttc_port_move_action_copy_to_cpu_drop_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_portMoveAct_t    act;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ('f' == TOKEN_CHAR(5, 0))
    {
        act.sttAct.act  = ACTION_FORWARD;
    }
    else if('d' == TOKEN_CHAR(5, 0))
    {
        act.sttAct.act  = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        act.sttAct.act  = ACTION_TRAP2CPU;
    }
    else if ('c' == TOKEN_CHAR(5, 0))
    {
        act.sttAct.act  = ACTION_COPY2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        diag_util_mprintf("global sttc-port-move configuration is not supported in backward compatible mode\n");
        return CPARSER_NOT_OK;
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portMoveAction_set(unit, L2_PORT_MOVE_STATIC, &act), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_PORT_MOVE_STTC_PORT_MOVE_ACTION_COPY_TO_MASTER_TRAP_TO_MASTER
/*
l2-table set port-move sttc-port-move action ( copy-to-master | trap-to-master )
*/
cparser_result_t cparser_cmd_l2_table_set_port_move_sttc_port_move_action_copy_to_master_trap_to_master(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_portMoveAct_t    act;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ('c' == TOKEN_CHAR(5, 0))
    {
        act.sttAct.act  = ACTION_COPY2MASTERCPU;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        act.sttAct.act  = ACTION_TRAP2MASTERCPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        diag_util_mprintf("global sttc-port-move configuration is not supported in backward compatible mode\n");
        return CPARSER_NOT_OK;
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portMoveAction_set(unit, L2_PORT_MOVE_STATIC, &act), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_PORT_MOVE_STTC_PORT_MOVE_LEARN_STATE_DISABLE_ENABLE
/*
l2-table set port-move sttc-port-move learn state  ( disable | enable )
*/
cparser_result_t cparser_cmd_l2_table_set_port_move_sttc_port_move_learn_state_disable_enable(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_portMoveLrn_t learn;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(6, 0))
    {
        learn.sttLrn.enable = ENABLED;
    }
    else if('d' == TOKEN_CHAR(6, 0))
    {
        learn.sttLrn.enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_portMoveLearn_set(unit, L2_PORT_MOVE_STATIC, &learn), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_PORT_MOVE_DYNAMIC_PORT_MOVE_FORBID_PORT_TRUNK_PORT_TRUNK_ALL_STATE
/*
l2-table get port-move dynamic-port-move-forbid ( port | trunk ) ( <MASK_LIST:port_trunk> | all ) state
*/
cparser_result_t cparser_cmd_l2_table_get_port_move_dynamic_port_move_forbid_port_trunk_port_trunk_all_state(cparser_context_t *context,
    char **port_trunk_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t        enable = DISABLED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    rtk_trk_t         trunk = 0;
    diag_trklist_t      trklist;
    rtk_l2_portMoveAct_t    act;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ('t' == TOKEN_CHAR(4, 0))
    {
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("trunk is not supported in this chip\n");
            return CPARSER_NOT_OK;
        }
    }

    if ('p' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_portDynamicPortMoveForbidEnable_get(unit, port, &enable), ret);
            diag_util_mprintf("\tPort %d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }
    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 5), ret);

        DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_trkDynamicPortMoveForbidEnable_get(unit, trunk, &enable), ret);
            diag_util_mprintf("\tTrunk %d : %s\n", trunk, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_dynamicPortMoveForbidAction_get(unit, &act.forbidAct.act), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portMoveAction_get(unit, L2_PORT_MOVE_FORBID, &act), ret);
    }
    diag_util_mprintf("\tAction : ");
    if (act.forbidAct.act == ACTION_FORWARD)
    {
        diag_util_mprintf("Forward\n");
    }
    else if (act.forbidAct.act  == ACTION_DROP)
    {
        diag_util_mprintf("Drop\n");
    }
    else if (act.forbidAct.act  == ACTION_TRAP2CPU)
    {
        diag_util_mprintf("Trap-To-Cpu\n");
    }
    else if (act.forbidAct.act  == ACTION_COPY2CPU)
    {
        diag_util_mprintf("Copy-To-Cpu\n");
    }
    else if (act.forbidAct.act  == ACTION_TRAP2MASTERCPU)
    {
        diag_util_mprintf("Trap-To-Master Cpu\n");
    }
    else if (act.forbidAct.act  == ACTION_COPY2MASTERCPU)
    {
        diag_util_mprintf("Copy-To-Master Cpu\n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_PORT_MOVE_DYNAMIC_PORT_MOVE_FORBID_PORT_TRUNK_PORT_TRUNK_ALL_STATE_DISABLE_ENABLE
/*
l2-table set port-move dynamic-port-move-forbid ( port | trunk ) ( <MASK_LIST:port_trunk> | all ) state ( disable | enable )
*/
cparser_result_t cparser_cmd_l2_table_set_port_move_dynamic_port_move_forbid_port_trunk_port_trunk_all_state_disable_enable(cparser_context_t *context,
    char **port_trunk_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_enable_t        enable;
    diag_portlist_t     portlist;
    rtk_trk_t         trunk = 0;
    diag_trklist_t      trklist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('t' == TOKEN_CHAR(4, 0))
    {
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf("trunk is not supported in this chip\n");
            return CPARSER_NOT_OK;
        }
    }

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else if('d' == TOKEN_CHAR(7, 0))
    {
        enable = DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('p' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_portDynamicPortMoveForbidEnable_set(unit, port, enable), ret);
        }
    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 5), ret);

        DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_trkDynamicPortMoveForbidEnable_set(unit, trunk, enable), ret);
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_LOOKUP_MISS_BCAST_UNICAST_FWD_PORT
/*
l2-table get lookup-miss ( bcast | unicast ) fwd-port
*/
cparser_result_t cparser_cmd_l2_table_get_lookup_miss_bcast_unicast_fwd_port(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_lookupMissType_t     type = DLF_TYPE_UCAST;
    rtk_portmask_t  portmask;
    char            port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(3, 0))
    {
        type = DLF_TYPE_UCAST;
    }
    else if ('b' == TOKEN_CHAR(3, 0))
    {
        type = DLF_TYPE_BCAST;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
    DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_get(unit, type, &portmask), ret);
    osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
    diag_util_lPortMask2str(port_list, &portmask);

    if (DLF_TYPE_UCAST == type)
        diag_util_mprintf("\tUnicast lookup miss forwarding portmask : %s\n", port_list);
    else
        diag_util_mprintf("\tBroadcast lookup miss forwarding portmask : %s\n", port_list);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_DEL_SET_LOOKUP_MISS_BCAST_UNICAST_PORT_PORT_ALL
/*
l2-table ( add | del | set ) lookup-miss ( bcast | unicast ) port ( <PORT_LIST:port> | all )
*/
cparser_result_t cparser_cmd_l2_table_add_del_set_lookup_miss_bcast_unicast_port_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t      port = 0;
    uint32          index;
    rtk_l2_lookupMissType_t type = DLF_TYPE_UCAST;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(3, 0))
    {
        type = DLF_TYPE_UCAST;
    }
    else if ('b' == TOKEN_CHAR(3, 0))
    {
        type = DLF_TYPE_BCAST;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }


    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('a' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_add(unit, type, port), ret);
        }
    }
    else if ('d' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_del(unit, type, port), ret);
        }
    }
    else if ('s' == TOKEN_CHAR(1, 0))
    {
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMaskIdx_get(unit, type, &index), ret);
            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_setByIndex(unit, type, index, &portlist.portmask), ret);
        }

        if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_set(unit, type, &portlist.portmask), ret);
        }
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_LOOKUP_MISS_PORT_PORT_ALL_UNICAST_ACTION
/*
l2-table get lookup-miss port ( <PORT_LIST:port> | all ) unicast action
*/
cparser_result_t cparser_cmd_l2_table_get_lookup_miss_port_port_all_unicast_action(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_action_t        action;
    diag_portlist_t     portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port Lookup Miss Information:\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %u :\n", port);

        DIAG_UTIL_ERR_CHK(rtk_l2_portUcastLookupMissAction_get(unit, port, &action), ret);
        diag_util_mprintf("\tUnicast Lookup Action        : ");
        if (action == ACTION_FORWARD)
        {
            diag_util_mprintf("Forward\n");
        }
        else if (action == ACTION_DROP)
        {
            diag_util_mprintf("Drop\n");
        }
        else if (action == ACTION_TRAP2CPU)
        {
            diag_util_mprintf("Trap-To-Cpu\n");
        }
        else if (action == ACTION_COPY2CPU)
        {
            diag_util_mprintf("Copy-To-Cpu\n");
        }
        else if (action == ACTION_TRAP2MASTERCPU)
        {
            diag_util_mprintf("Trap-To Master Cpu\n");
        }
        else if (action == ACTION_COPY2MASTERCPU)
        {
            diag_util_mprintf("Copy-To Master Cpu\n");
        }
        else
        {
            diag_util_mprintf("Unknown action\n");
        }
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LOOKUP_MISS_PORT_PORT_ALL_UNICAST_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
l2-table set lookup-miss port ( <PORT_LIST:port> | all ) unicast action ( copy-to-cpu | drop | forward | trap-to-cpu )
*/
cparser_result_t cparser_cmd_l2_table_set_lookup_miss_port_port_all_unicast_action_copy_to_cpu_drop_forward_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_action_t        action;
    diag_portlist_t     portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('f' == TOKEN_CHAR(7, 0))
    {
        action = ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(7, 0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(7, 0))
    {
        action = ACTION_TRAP2CPU;
    }
    else if ('c' == TOKEN_CHAR(7, 0))
    {
        action = ACTION_COPY2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portUcastLookupMissAction_set(unit, port, action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_LOOKUP_MISS_PORT_PORT_ALL_UNICAST_ACTION_COPY_TO_MASTER_TRAP_TO_MASTER
/*
l2-table set lookup-miss port ( <PORT_LIST:port> | all ) unicast action ( copy-to-master | trap-to-master )
*/
cparser_result_t cparser_cmd_l2_table_set_lookup_miss_port_port_all_unicast_action_copy_to_master_trap_to_master(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_action_t        action;
    diag_portlist_t     portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('c' == TOKEN_CHAR(7, 0))
    {
        action = ACTION_COPY2MASTERCPU;
    }
    else if ('t' == TOKEN_CHAR(7, 0))
    {
        action = ACTION_TRAP2MASTERCPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portUcastLookupMissAction_set(unit, port, action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_SRC_PORT_EGRESS_FILTER_PORT_PORT_ALL_STATE
/*
l2-table get src-port-egress-filter port ( <PORT_LIST:port> | all ) state
*/
cparser_result_t cparser_cmd_l2_table_get_src_port_egress_filter_port_port_all_state(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    diag_portlist_t               portlist;
    rtk_port_t port;
    rtk_portmask_t srcFilterMask;
    rtk_switch_devInfo_t devInfo;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));

    if ((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_srcPortEgrFilterMask_get(unit, &srcFilterMask), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if (RTK_PORTMASK_IS_PORT_SET(devInfo.all.portmask, port))
        {
            diag_util_mprintf("Port %u      :", port);

            if (RTK_PORTMASK_IS_PORT_SET(srcFilterMask, port))
                diag_util_mprintf("enable\n");
            else
                diag_util_mprintf("disable\n");
        }
        else
            DIAG_ERR_PRINT(RT_ERR_PORT_ID);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_SRC_PORT_EGRESS_FILTER_PORT_PORT_ALL_STATE_DISABLE_ENABLE
/*
l2-table set src-port-egress-filter port ( <PORT_LIST:port> | all ) state ( disable | enable )
*/
cparser_result_t cparser_cmd_l2_table_set_src_port_egress_filter_port_port_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_portmask_t  ori_pmask, portmask;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    DIAG_UTIL_ERR_CHK(rtk_l2_srcPortEgrFilterMask_get(unit, &ori_pmask), ret);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    portmask = portlist.portmask;

    if('e' == TOKEN_CHAR(6, 0))
    {
        RTK_PORTMASK_OR(ori_pmask, portmask);
    }
    else if('d' == TOKEN_CHAR(6, 0))
    {
        RTK_PORTMASK_REMOVE(ori_pmask, portmask);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_srcPortEgrFilterMask_set(unit, &ori_pmask), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_EXCEPT_SMAC
/*
l2-table get except-smac
*/
cparser_result_t cparser_cmd_l2_table_get_except_smac(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_action_t          action = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_exceptionAddrAction_get(unit, SA_IS_BCAST_OR_MCAST, &action), ret);
    diag_util_mprintf("    Multicast or Broadcast Src Mac Action : ");
    if (action == ACTION_DROP)
    {
        diag_util_mprintf("Drop\n");
    }
    else if (action == ACTION_TRAP2CPU)
    {
        diag_util_mprintf("Trap to CPU\n");
    }
    else if (action == ACTION_FORWARD)
    {
        diag_util_mprintf("Forward\n");
    }
    else if (action == ACTION_TRAP2MASTERCPU)
    {
        diag_util_mprintf("Trap-To Master Cpu\n");
    }
    else if (action == ACTION_COPY2MASTERCPU)
    {
        diag_util_mprintf("Copy-To Master Cpu\n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_exceptionAddrAction_get(unit, SA_IS_ZERO, &action), ret);
    diag_util_mprintf("    Zero Src Mac Action : ");
    if (action == ACTION_DROP)
    {
        diag_util_mprintf("Drop\n");
    }
    else if (action == ACTION_TRAP2CPU)
    {
        diag_util_mprintf("Trap to CPU\n");
    }
    else if (action == ACTION_FORWARD)
    {
        diag_util_mprintf("Forward\n");
    }
    else if (action == ACTION_TRAP2MASTERCPU)
    {
        diag_util_mprintf("Trap-To Master Cpu\n");
    }
    else if (action == ACTION_COPY2MASTERCPU)
    {
        diag_util_mprintf("Copy-To Master Cpu\n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_zeroSALearningEnable_get(unit, &enable), ret);
    diag_util_mprintf("    Zero Src Mac Learn : ");
    if (enable == ENABLED)
    {
        diag_util_mprintf("enable\n");
    }
    else if (enable == DISABLED)
    {
        diag_util_mprintf("disable\n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_EXCEPT_SMAC_ZERO_SA_LEARN_STATE_DISABLE_ENABLE
/*
l2-table set except-smac zero-sa-learn state ( disable | enable )
*/
cparser_result_t cparser_cmd_l2_table_set_except_smac_zero_sa_learn_state_disable_enable(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('e' == TOKEN_CHAR(5, 0))
    {
        enable=ENABLED;
    }
    else if('d' == TOKEN_CHAR(5, 0))
    {
        enable=DISABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_zeroSALearningEnable_set(unit, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_EXCEPT_SMAC_MCAST_BCAST_SA_ZERO_SA_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
l2-table set except-smac ( mcast-bcast-sa | zero-sa ) action ( drop | forward | trap-to-cpu )
*/
cparser_result_t cparser_cmd_l2_table_set_except_smac_mcast_bcast_sa_zero_sa_action_drop_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_l2_exceptionAddrType_t type;
    rtk_action_t    action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('m' == TOKEN_CHAR(3, 0))
    {
        type = SA_IS_BCAST_OR_MCAST;
    }
    else if ('z' == TOKEN_CHAR(3, 0))
    {
        type = SA_IS_ZERO;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('f' == TOKEN_CHAR(5, 0))
    {
        action = ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        action = ACTION_TRAP2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_exceptionAddrAction_set(unit, type, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_EXCEPT_SMAC_MCAST_BCAST_SA_ZERO_SA_ACTION_TRAP_TO_MASTER
/*
l2-table set except-smac ( mcast-bcast-sa | zero-sa ) action ( trap-to-master )
*/
cparser_result_t cparser_cmd_l2_table_set_except_smac_mcast_bcast_sa_zero_sa_action_trap_to_master(cparser_context_t *context)
{
    uint32                      unit = 0;
    int32                       ret = RT_ERR_FAILED;
    rtk_l2_exceptionAddrType_t  type;
    rtk_action_t                action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('m' == TOKEN_CHAR(3, 0))
    {
        type = SA_IS_BCAST_OR_MCAST;
    }
    else if ('z' == TOKEN_CHAR(3, 0))
    {
        type = SA_IS_ZERO;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('t' == TOKEN_CHAR(5, 0))
    {
        action = ACTION_TRAP2MASTERCPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_exceptionAddrAction_set(unit, type, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_SA_BLOCK_DA_BLOCK_PORT_PORTS_ALL_STATE
/*
 * l2-table get ( sa-block | da-block ) port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_l2_table_get_sa_block_da_block_port_ports_all_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_l2_macFilterMode_t mode = MAC_FILTER_MODE_SA;
    rtk_enable_t        enable = DISABLED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ('s' == TOKEN_CHAR(2, 0))
    {
        mode = MAC_FILTER_MODE_SA;
        diag_util_mprintf("\tSA-BLOCK Configuration\n");
    }
    else if ('d' == TOKEN_CHAR(2, 0))
    {
        mode = MAC_FILTER_MODE_DA;
        diag_util_mprintf("\tDA-BLOCK Configuration\n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portMacFilterEnable_get(unit, port, mode, &enable), ret);
        diag_util_mprintf("\tPort %d : %s\n", port, enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_SA_BLOCK_DA_BLOCK_PORT_PORTS_ALL_STATE_ENABLE_DISABLE
/*
l2-table set ( sa-block | da-block ) port ( <PORT_LIST:ports> | all ) state ( enable | disable )
*/
cparser_result_t cparser_cmd_l2_table_set_sa_block_da_block_port_ports_all_state_enable_disable(cparser_context_t *context,
    char **ports_ptr)
{

    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_port_t  port = 0;
    diag_portlist_t portlist;
    rtk_l2_macFilterMode_t mode = MAC_FILTER_MODE_SA;
    rtk_enable_t enable = DISABLED;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('s' == TOKEN_CHAR(2, 0))
    {
        mode = MAC_FILTER_MODE_SA;
    }
    else if ('d' == TOKEN_CHAR(2, 0))
    {
        mode = MAC_FILTER_MODE_DA;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

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
        DIAG_UTIL_ERR_CHK(rtk_l2_portMacFilterEnable_set(unit, port, mode, enable), ret);
    }

    return CPARSER_OK;

}
#endif

#ifdef CMD_L2_TABLE_GET_FWD_TABLE_LOW_IDX_HIGH_IDX
/*
l2-table get fwd-table <UINT:low_idx> <UINT:high_idx>
*/
cparser_result_t cparser_cmd_l2_table_get_fwd_table_low_idx_high_idx(cparser_context_t *context,
    uint32_t *low_idx_ptr,
    uint32_t *high_idx_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      minIndex;
    uint32      maxIndex;
    int32       index;
    rtk_portmask_t portmask;
    char        port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == low_idx_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == high_idx_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    minIndex = *low_idx_ptr;
    maxIndex = *high_idx_ptr;

    for(index = minIndex; index <= maxIndex; index++)
    {
#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        uint32 dummy;
        DIAG_UTIL_ERR_CHK(rtk_l2_mcastFwdPortmask_get(unit, index, &portmask, &dummy), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_mcastFwdPortmaskEntry_get(unit, index, &portmask), ret);
    }

        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(port_list, &portmask);
        diag_util_mprintf("Index : %4u,  Port : %s\n", index, port_list);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_FWD_TABLE_FREE_COUNT
/*
 *   l2-table get fwd-table free-count
 */
cparser_result_t cparser_cmd_l2_table_get_fwd_table_free_count(cparser_context_t *context)
{
    int32   ret;
    uint32  unit = 0;
    uint32  val = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();


    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_mcastFwdIndexFreeCount_get(unit, &val), ret);
        diag_util_mprintf("Forwarding Table Free Count: %d\n", val);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_DEL_CPU_MAC_VID_MAC
/*
l2-table ( add | del ) cpu-mac <UINT:vid> <MACADDR:mac>
*/
cparser_result_t cparser_cmd_l2_table_add_del_cpu_mac_vid_mac(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    if ('a' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_cpuMacAddr_add(unit, *vid_ptr, (rtk_mac_t *)mac_ptr), ret);
    }
    else if ('d' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_cpuMacAddr_del(unit, *vid_ptr, (rtk_mac_t *)mac_ptr), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_SET_MAC_UCAST_VID_MAC_PORT_TRUNK_ID_SA_BLOCK_DA_BLOCK_STATIC_NEXTHOP_SUSPEND
/*
l2-table ( add | set ) mac-ucast <UINT:vid> <MACADDR:mac> ( port | trunk ) <UINT:id> { sa-block } { da-block } { static } { nexthop } { suspend }
*/
cparser_result_t cparser_cmd_l2_table_add_set_mac_ucast_vid_mac_port_trunk_id_sa_block_da_block_static_nexthop_suspend(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *id_ptr)
{
    int32           flag_num = 0;
    uint32          sa_block = FALSE;
    uint32          da_block = FALSE;
    uint32          is_static = FALSE;
    uint32          nexthop = FALSE;
    uint32          suspend = FALSE;
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t  l2_uAddr;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&l2_uAddr, 0, sizeof(rtk_l2_ucastAddr_t));

    if ((TRUE == diag_util_isBcastMacAddr(mac_ptr->octet)) || (TRUE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Broadcast and Multicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    /*from first optional token*/
    for(flag_num = 7; flag_num < TOKEN_NUM; flag_num++)
    {
        if ('s' == TOKEN_CHAR(flag_num, 0))
        {
            if ('a' == TOKEN_CHAR(flag_num, 1))
            {
                sa_block = TRUE;
            }
            else if ('t' == TOKEN_CHAR(flag_num, 1))
            {
                is_static = TRUE;
            }
            else if ('u' == TOKEN_CHAR(flag_num, 1))
            {
                suspend = TRUE;
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
        }
        else if ('d' == TOKEN_CHAR(flag_num, 0))
        {
            da_block = TRUE;
        }
        else if ('n' == TOKEN_CHAR(flag_num, 0))
        {
            nexthop = TRUE;
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    /* Fill structure */
    l2_uAddr.vid = *vid_ptr;
    osal_memcpy(l2_uAddr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    if ('p' == TOKEN_CHAR(5, 0))
    {
        if (((DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID)) && *id_ptr == 0x1f) ||
            ((DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID)) && *id_ptr == 0x3f))
            l2_uAddr.port = L2_PORT_DONT_CARE;
        else
            l2_uAddr.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
        l2_uAddr.trk_gid = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if(sa_block)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;

    if(da_block)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;

    if(is_static)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_STATIC;

    if(suspend)
        l2_uAddr.state |= RTK_L2_UCAST_STATE_SUSPEND;

    l2_uAddr.agg_vid = 0;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &l2_uAddr.devID), ret);
#endif

    if ('a' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(unit, &l2_uAddr), ret);
    }
    else if ('s' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_addr_set(unit, &l2_uAddr), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if (nexthop)
        _l2_nexthop_set(unit, l2_uAddr.l2_idx);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_SET_MAC_UCAST_VID_MAC_DEVID_DEVID_PORT_TRUNK_ID_SA_BLOCK_DA_BLOCK_STATIC_NEXTHOP_SUSPEND_AGED
/*
l2-table ( add | set ) mac-ucast <UINT:vid> <MACADDR:mac> devID <UINT:devID> ( port | trunk ) <UINT:id>  { sa-block } { da-block } { static } { nexthop } { suspend } { aged }
*/
cparser_result_t cparser_cmd_l2_table_add_set_mac_ucast_vid_mac_devID_devID_port_trunk_id_sa_block_da_block_static_nexthop_suspend_aged(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *devid_ptr,
    uint32_t *id_ptr)
{
    int32           flag_num = 0;
    uint32          sa_block = FALSE;
    uint32          da_block = FALSE;
    uint32          is_static = FALSE;
    uint32          nexthop = FALSE;
    uint32          suspend = FALSE;
    uint32          aged = FALSE;
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t  l2_uAddr;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == id_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&l2_uAddr, 0, sizeof(rtk_l2_ucastAddr_t));

    if ((TRUE == diag_util_isBcastMacAddr(mac_ptr->octet)) || (TRUE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Broadcast and Multicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    /*from first optional token*/
    for(flag_num = 9; flag_num < TOKEN_NUM; flag_num++)
    {
        if ('s' == TOKEN_CHAR(flag_num, 0))
        {
            if ('a' == TOKEN_CHAR(flag_num, 1))
            {
                sa_block = TRUE;
            }
            else if ('t' == TOKEN_CHAR(flag_num, 1))
            {
                is_static = TRUE;
            }
            else if ('u' == TOKEN_CHAR(flag_num, 1))
            {
                suspend = TRUE;
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
        }
        else if ('d' == TOKEN_CHAR(flag_num, 0))
        {
            da_block = TRUE;
        }
        else if ('n' == TOKEN_CHAR(flag_num, 0))
        {
            nexthop = TRUE;
        }
        else if ('a' == TOKEN_CHAR(flag_num, 0))
        {
            aged = TRUE;
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }


    /* Fill structure */
    l2_uAddr.vid        = *vid_ptr;
    l2_uAddr.devID    = *devid_ptr;
    osal_memcpy(l2_uAddr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    if ('p' == TOKEN_CHAR(7, 0))
    {
        l2_uAddr.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(7, 0))
    {
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
        l2_uAddr.trk_gid = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if(sa_block)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;

    if(da_block)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;

    if(is_static)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_STATIC;

    if(suspend)
        l2_uAddr.state |= RTK_L2_UCAST_STATE_SUSPEND;

    if(aged)
    {
        l2_uAddr.isAged=1;
    }

    l2_uAddr.agg_vid = 0;

    if ('a' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(unit, &l2_uAddr), ret);
    }
    else if ('s' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_addr_set(unit, &l2_uAddr), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if (nexthop)
        _l2_nexthop_set(unit, l2_uAddr.l2_idx);



    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_SET_MAC_UCAST_VID_MAC_VXLAN_ENTRY_INDEX_SA_BLOCK_DA_BLOCK_STATIC_NEXTHOP_SUSPEND_AGED
/*
 * l2-table ( add | set ) mac-ucast <UINT:vid> <MACADDR:mac> vxlan-entry <UINT:index> { sa-block } { da-block } { static } { nexthop } { suspend } { aged }
 */
cparser_result_t
cparser_cmd_l2_table_add_set_mac_ucast_vid_mac_vxlan_entry_index_sa_block_da_block_static_nexthop_suspend_aged(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *index_ptr)
{
    int32           flag_num = 0;
    uint32          sa_block = FALSE;
    uint32          da_block = FALSE;
    uint32          is_static = FALSE;
    uint32          nexthop = FALSE;
    uint32          suspend = FALSE;
    uint32          aged = FALSE;
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t  l2_uAddr;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == index_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&l2_uAddr, 0, sizeof(rtk_l2_ucastAddr_t));

    if ((TRUE == diag_util_isBcastMacAddr(mac_ptr->octet)) || (TRUE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Broadcast and Multicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    /*from first optional token*/
    for(flag_num = 7; flag_num < TOKEN_NUM; flag_num++)
    {
        if ('s' == TOKEN_CHAR(flag_num, 0))
        {
            if ('a' == TOKEN_CHAR(flag_num, 1))
            {
                sa_block = TRUE;
            }
            else if ('t' == TOKEN_CHAR(flag_num, 1))
            {
                is_static = TRUE;
            }
            else if ('u' == TOKEN_CHAR(flag_num, 1))
            {
                suspend = TRUE;
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
        }
        else if ('d' == TOKEN_CHAR(flag_num, 0))
        {
            da_block = TRUE;
        }
        else if ('n' == TOKEN_CHAR(flag_num, 0))
        {
            nexthop = TRUE;
        }
        else if ('a' == TOKEN_CHAR(flag_num, 0))
        {
            aged = TRUE;
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    /* Fill structure */
    l2_uAddr.vid  = *vid_ptr;
    osal_memcpy(l2_uAddr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    if(sa_block)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;

    if(da_block)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;

    if(is_static)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_STATIC;

    if(nexthop)
        l2_uAddr.flags |= RTK_L2_UCAST_FLAG_NEXTHOP;

    if(suspend)
        l2_uAddr.state |= RTK_L2_UCAST_STATE_SUSPEND;

    if(aged)
    {
        l2_uAddr.isAged = 1;
    }

    /* L2 tunnel info */
    l2_uAddr.flags |= RTK_L2_UCAST_FLAG_L2_TUNNEL;  /* L2-tunnel type entry */
    l2_uAddr.l2_tunnel_idx = *index_ptr;

    if ('a' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(unit, &l2_uAddr), ret);
    }
    else if ('s' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_addr_set(unit, &l2_uAddr), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_add_set_mac_ucast_vid_mac_vxlan_entry_index_sa_block_da_block_static_nexthop_suspend_aged */
#endif

#ifdef CMD_L2_TABLE_SET_MAC_UCAST_VID_MAC_NEXTHOP_MAC_IDX
/*
l2-table set mac-ucast <UINT:vid> <MACADDR:mac> nexthop <UINT:mac_idx>
*/
cparser_result_t cparser_cmd_l2_table_set_mac_ucast_vid_mac_nexthop_mac_idx(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *mac_idx_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t  l2_uAddr;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_idx_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&l2_uAddr, 0, sizeof(rtk_l2_ucastAddr_t));

    if ((TRUE == diag_util_isBcastMacAddr(mac_ptr->octet)) || (TRUE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Broadcast and Multicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    /* Fill structure */
    l2_uAddr.vid = *vid_ptr;
    osal_memcpy(l2_uAddr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_get(unit, &l2_uAddr), ret);

    l2_uAddr.flags |= RTK_L2_UCAST_FLAG_NEXTHOP;
    l2_uAddr.route_idx = *mac_idx_ptr;

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &l2_uAddr.devID), ret);
#endif
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_set(unit, &l2_uAddr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_MAC_UCAST_VID_MAC
/*
l2-table get mac-ucast <UINT:vid> <MACADDR:mac>
*/
cparser_result_t cparser_cmd_l2_table_get_mac_ucast_vid_mac(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t  l2_data;
#if (defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300))
    char                buffer[6];
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2_data, 0, sizeof(rtk_l2_ucastAddr_t));

    osal_memcpy(l2_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    l2_data.vid = *vid_ptr;

    if ((ret = rtk_l2_addr_get(unit, &l2_data)) != RT_ERR_OK)
    {
        if(ret==RT_ERR_L2_ENTRY_EXIST)
        {
            diag_util_printf("Entry is not exist\n");
            return CPARSER_OK;
        }
        else
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf(" Index | VID | MAC Address       | SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Agg VID | Age | vlanTarget | mac_idx |\n");
        diag_util_mprintf("-------+-----+-------------------+------+----------+----------+---------+----------+---------+---------+-----+---------+-------\n");
        diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %8d | %8d | %7d | %8d | %7d |   %4d  |  %2d | %10d | %8d \n",
                            l2_data.l2_idx,
                            l2_data.vid,
                            l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                     l2_data.port, (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                     (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                     (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0, l2_data.agg_vid, l2_data.age,
                     ((l2_data.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? l2_data.vlan_target : 0),
                     ((l2_data.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? l2_data.route_idx : 0));

    }
#endif

#if defined(CONFIG_SDK_RTL9300)
                if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
                {
                    sprintf(buffer, "%d", l2_data.devID);
                    diag_util_mprintf(" Index | VID | MAC Address       | TRK | DevID |  SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Aggreate VID | MAC Index |\n");
                    diag_util_mprintf("-------+------+------------------+------+------+------+----------+----------+---------+----------+---------+---------+-------------\n");
                    if(l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)
                    {
                        diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4s | %4d | %8d | %8d | %7d | %8d | %7d | %s | %7d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                    (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                    "Not Support",
                                    l2_data.route_idx);
                    }
                    else
                    {
                        diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4s | %4d | %8d | %8d | %7d | %8d | %7d | %7d | %s\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                 (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                 l2_data.agg_vid,
                                 "Not Support");
                    }
                }
#endif

#if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            sprintf(buffer, "%d", l2_data.devID);
            diag_util_mprintf(" Index | VID | MAC Address       |TRK|DevID|SPA|SBlk|DBlk|Sttc|NH|Sus|Tag|APri|AVID|Tnl|ECID/Idx| Age\n");
            diag_util_mprintf("-------+-----+-------------------+---+-----+---+----+----+----+--+---+---+----+----+---+--------+-----\n");
            diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %d | %4s|%3d|%4d|%4d|%4d|%2d|%3d|%3d|%4d|%4d| %d |0x%06X|  %2d\n",
                            l2_data.l2_idx,
                            l2_data.vid,
                            l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? l2_data.trk_gid : l2_data.port,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP) ? 1 : 0,
                            (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_TAG_STS) ? 1 : 0,
                            l2_data.agg_pri,
                            l2_data.agg_vid,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? 1 : 0,
                            (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? l2_data.l2_tunnel_idx : l2_data.ecid,
                            l2_data.age);
        }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_MAC_UCAST_VID_MAC
/*
l2-table del mac-ucast <UINT:vid> <MACADDR:mac>
*/
cparser_result_t cparser_cmd_l2_table_del_mac_ucast_vid_mac(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t  l2_data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2_data, 0, sizeof(rtk_l2_ucastAddr_t));

    osal_memcpy(l2_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    l2_data.vid = *vid_ptr;

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &l2_data.devID), ret);
#endif
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_del(unit, l2_data.vid, &l2_data.mac), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_MAC_MCAST_VID_MAC_INDEX_INDEX_NEXTHOP
/*
 * l2-table add mac-mcast <UINT:vid> <MACADDR:mac> index <UINT:index> { nexthop }
 */
cparser_result_t cparser_cmd_l2_table_add_mac_mcast_vid_mac_index_index_nexthop(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *index_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_mcastAddr_t  mcast_data;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == index_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&mcast_data, 0, sizeof(rtk_l2_mcastAddr_t));

    if ((FALSE == diag_util_isBcastMacAddr(mac_ptr->octet)) && (FALSE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Unicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    mcast_data.rvid = *vid_ptr;
    osal_memcpy(mcast_data.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    mcast_data.fwdIndex = *index_ptr;

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if(TOKEN_NUM==8 && ('n' == TOKEN_CHAR(7, 0)))
    {
        if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            mcast_data.nextHop = 1;
        }
        else
        {
            diag_util_mprintf("nexthop is not supported in this chip\n");
        }
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_addByIndex(unit, &mcast_data), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_ADD_SET_MAC_MCAST_VID_MAC_PORT_PORT_ALL_NEXTHOP
/*
 * l2-table ( add | set ) mac-mcast <UINT:vid> <MACADDR:mac> port ( <PORT_LIST:port> | all ) { nexthop }
 */
cparser_result_t cparser_cmd_l2_table_add_set_mac_mcast_vid_mac_port_port_all_nexthop(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    char **port_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_mcastAddr_t  mcast_data;
    diag_portlist_t               portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&mcast_data, 0, sizeof(rtk_l2_mcastAddr_t));

    if ((FALSE == diag_util_isBcastMacAddr(mac_ptr->octet)) && (FALSE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Unicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    mcast_data.portmask = portlist.portmask;

    mcast_data.rvid = *vid_ptr;
    osal_memcpy(mcast_data.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        mcast_data.agg_vid = 0;
#endif


    if ('a' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(unit, &mcast_data), ret);
    }
    else if ('s' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_set(unit, &mcast_data), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }


#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    /* rtk_l2_mcastAddr_add/set don't allowed setting nexthop bit */
    if(TOKEN_NUM==8 && ('n' == TOKEN_CHAR(7, 0)))
    {
        if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            _l2_nexthop_set(unit, mcast_data.l2_idx);
        }
        else
        {
            diag_util_mprintf("nexthop is not supported in this chip\n");
        }
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_MAC_MCAST_VID_MAC_FWD_INDEX_INDEX
/*
 * l2-table set mac-mcast <UINT:vid> <MACADDR:mac> fwd_index <UINT:index>
 */
 cparser_result_t cparser_cmd_l2_table_set_mac_mcast_vid_mac_fwd_index_index(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *index_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_mcastAddr_t  mcast_data;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&mcast_data, 0, sizeof(rtk_l2_mcastAddr_t));

    if ((FALSE == diag_util_isBcastMacAddr(mac_ptr->octet)) && (FALSE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Unicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    mcast_data.rvid = *vid_ptr;
    osal_memcpy(mcast_data.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    mcast_data.fwdIndex = *index_ptr;

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        mcast_data.agg_vid = 0;
#endif

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_setByIndex(unit, &mcast_data), ret);

    return CPARSER_OK;
}

#endif

#ifdef CMD_L2_TABLE_ADD_SET_MAC_MCAST_VID_MAC_GROUP_GROUP_ID_NEXTHOP
/*
 * l2-table ( add | set ) mac-mcast <UINT:vid> <MACADDR:mac> group <UINT:group_id> { nexthop }
 */
cparser_result_t
cparser_cmd_l2_table_add_set_mac_mcast_vid_mac_group_group_id_nexthop(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *group_id_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_mcastAddr_t  mcast_data;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((0 == *group_id_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&mcast_data, 0, sizeof(rtk_l2_mcastAddr_t));

    if ((FALSE == diag_util_isBcastMacAddr(mac_ptr->octet)) && (FALSE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Unicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    mcast_data.rvid = *vid_ptr;
    osal_memcpy(mcast_data.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (TOKEN_NUM==8 && ('n' == TOKEN_CHAR(7, 0)))
    {
        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            mcast_data.nextHop = 1;
        }
        else
        {
            diag_util_mprintf("nexthop is not supported in this chip\n");
        }
    }
#endif

    /* data */
    mcast_data.groupId = *group_id_ptr;

    if ('a' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(unit, &mcast_data), ret);
    }
    else if ('s' == TOKEN_CHAR(1, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_set(unit, &mcast_data), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_add_set_mac_mcast_vid_mac_group_group_id_nexthop */
#endif

#ifdef CMD_L2_TABLE_SET_MAC_MCAST_VID_MAC_NEXTHOP_MAC_IDX
/*
l2-table set mac-mcast <UINT:vid> <MACADDR:mac> nexthop <UINT:mac_idx>
*/
cparser_result_t cparser_cmd_l2_table_set_mac_mcast_vid_mac_nexthop_mac_idx(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *mac_idx_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_l2_mcastAddr_t  mcast_data;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);

    osal_memset(&mcast_data, 0, sizeof(rtk_l2_mcastAddr_t));

    if ((FALSE == diag_util_isBcastMacAddr(mac_ptr->octet)) && (FALSE == diag_util_isMcastMacAddr(mac_ptr->octet)))
    {
        diag_util_printf("Unicast MAC address is not allowed to configure.\n");
        return CPARSER_NOT_OK;
    }

    mcast_data.rvid = *vid_ptr;
    osal_memcpy(mcast_data.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_get(unit, &mcast_data), ret);

    mcast_data.nextHop = 1;
    mcast_data.mac_idx = *mac_idx_ptr;

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_set(unit, &mcast_data), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_MAC_MCAST_VID_MAC
/*
l2-table get mac-mcast <UINT:vid> <MACADDR:mac>
*/
cparser_result_t cparser_cmd_l2_table_get_mac_mcast_vid_mac(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_mcastAddr_t  mcast_data;
    char                port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&mcast_data, 0, sizeof(rtk_l2_mcastAddr_t));

    mcast_data.rvid = *vid_ptr;
    osal_memcpy(mcast_data.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    if ((ret = rtk_l2_mcastAddr_get(unit, &mcast_data)) != RT_ERR_OK)
    {
        if(ret==RT_ERR_L2_ENTRY_EXIST)
        {
            diag_util_printf("Entry is not exist\n");
            return CPARSER_OK;
        }
        else
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }


#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Index | VID  | MAC address       | Port                 | Agg Vid\n");
        diag_util_mprintf("------+------+-------------------+----------------------+-----------\n");

        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(port_list, &mcast_data.portmask);

        diag_util_mprintf("%5d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d\n",
                    mcast_data.l2_idx,
                    mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                    mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.agg_vid);
    }
#endif

    if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("Index | VID  | MAC address       | Port                \n");
        diag_util_mprintf("------+------+-------------------+----------------------\n");

        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(port_list, &mcast_data.portmask);

        diag_util_mprintf("%5d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s \n",
                    mcast_data.l2_idx,
                    mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                    mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list);
    }

#if defined(CONFIG_SDK_RTL9300)
    if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        diag_util_mprintf("Index | VID  | MAC address       | Port                 | nexthop | MAC Index\n");
        diag_util_mprintf("-----+-----+-------------------+----------------------+---------+---------\n");

        osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(port_list, &mcast_data.portmask);

        if(mcast_data.nextHop)
        {
            diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d | %4d\n",
                        mcast_data.l2_idx,
                        mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                        mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.nextHop, mcast_data.mac_idx);
        }
        else
        {
            diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d | %4s\n",
                        mcast_data.l2_idx,
                        mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                        mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.nextHop, "Not Support");
        }
    }
#endif

#if defined(CONFIG_SDK_RTL9310)
        if(DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID  | MAC address       | Port                 | NH | Group ID \n");
            diag_util_mprintf("-------+------+-------------------+----------------------+----+------------\n");

            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &mcast_data.portmask);

            diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %2d | 0x%X\n",
                        mcast_data.l2_idx,
                        mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                        mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.nextHop, mcast_data.groupId);
        }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_MAC_MCAST_VID_MAC
/*
l2-table del mac-mcast <UINT:vid> <MACADDR:mac>
*/
cparser_result_t cparser_cmd_l2_table_del_mac_mcast_vid_mac(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_mcastAddr_t  mcast_data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&mcast_data, 0, sizeof(rtk_l2_mcastAddr_t));

    mcast_data.rvid = *vid_ptr;
    osal_memcpy(mcast_data.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_del(unit, mcast_data.rvid, &mcast_data.mac), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_IGNORE_INDEX_MAC_MCAST_VID_MAC
/*
l2-table del-ignore-index mac-mcast <UINT:vid> <MACADDR:mac>
*/
cparser_result_t cparser_cmd_l2_table_del_ignore_index_mac_mcast_vid_mac(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_mcastAddr_t  mcast_data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&mcast_data, 0, sizeof(rtk_l2_mcastAddr_t));

    mcast_data.rvid = *vid_ptr;
    osal_memcpy(mcast_data.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_delIgnoreIndex(unit, mcast_data.rvid, &mcast_data.mac), ret);
    return CPARSER_OK;
}

#endif

#ifdef CMD_L2_TABLE_DUMP_MAC_UCAST_DYNAMIC_STATIC_NEXTHOP_ALL
/*
l2-table dump mac-ucast ( dynamic | static | nexthop | all )
*/
cparser_result_t cparser_cmd_l2_table_dump_mac_ucast_dynamic_static_nexthop_all(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t  l2_data;
    int32               scan_idx;
    uint32              total_entry;
#if (defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300))
    char                buffer[6];
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2_data, 0, sizeof(rtk_l2_ucastAddr_t));

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf(" Index | VID | MAC Address       | SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Agg VID | Age | vlanTarget | mac_idx |\n");
        diag_util_mprintf("-------+-----+-------------------+------+----------+----------+---------+----------+---------+---------+-----+---------+-------\n");
    }
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        diag_util_mprintf(" Index | VID | MAC Address       | TRK | DevID | SPA |SBlk|DBlk|Sttc| NH |Sus|    AVID    |  MAC Index | Age\n");
        diag_util_mprintf("-------+-----+-------------------+-----+------+-----+----+----+----+----+---+------------+------------+-----\n");
    }
#endif

#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf(" Index | VID | MAC Address       |TRK|DevID|SPA|SBlk|DBlk|Sttc|NH|Sus|Tag|APri|AVID|Tnl|ECID/Idx| Age\n");
        diag_util_mprintf("-------+-----+-------------------+---+-----+---+----+----+----+--+---+---+----+----+---+--------+-----\n");
    }
#endif

    scan_idx=-1;
    total_entry=0;
    while (1)
    {

        if ((ret = rtk_l2_nextValidAddr_get(unit, (int32 *)&scan_idx, 1, &l2_data)) != RT_ERR_OK)
        {
            if(ret == RT_ERR_L2_ENTRY_NOTFOUND || ret == RT_ERR_ENTRY_NOTFOUND)
            {
                if (total_entry == 0)
                    diag_util_printf("Entry is not exist\n");
                break;
            }
            else if ((scan_idx > 1) && (ret == RT_ERR_INPUT))
            {
                return CPARSER_OK;
            }
            else
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }

        if ( 'd' == TOKEN_CHAR(3, 0))
        {
            if((l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) || (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) || (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC))
                continue;

            if((l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP) || (l2_data.isAged))
                continue;

            if((l2_data.state&RTK_L2_UCAST_STATE_SUSPEND))
                continue;
        }
        else if ( 's' == TOKEN_CHAR(3, 0))
        {
            if(!(l2_data.flags&RTK_L2_UCAST_FLAG_STATIC))
                continue;
        }
        else if ( 'n' == TOKEN_CHAR(3, 0))
        {
            if(!(l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP))
                continue;
        }
        else if ( 'a' == TOKEN_CHAR(3, 0))
        {

        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }


#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %8d | %8d | %7d | %8d | %7d |   %4d  |  %2d | %10d | %8d \n",
                            l2_data.l2_idx,
                            l2_data.vid,
                            l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                     l2_data.port, (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                     (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                     (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0, l2_data.agg_vid, l2_data.age,
                     ((l2_data.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? l2_data.vlan_target : 0),
                     ((l2_data.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? l2_data.route_idx : 0));
        }
#endif

#if defined(CONFIG_SDK_RTL9300)
                if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
                {
                    sprintf(buffer, "%d", l2_data.devID);
                    if(l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)
                    {
                        diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %3d | %4s | %3d |%4d|%4d|%4d| %2d |%3d| %s| %11d|  %2d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                    (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                    "Not Support",
                                    l2_data.route_idx,
                                    l2_data.age);
                    }
                    else
                    {
                        diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %3d | %4s | %3d |%4d|%4d|%4d| %2d |%3d| %10d | %s|  %2d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                 (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                 l2_data.agg_vid,
                                 "Not Support",
                                 l2_data.age);
                    }
                }
#endif

#if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            sprintf(buffer, "%d", l2_data.devID);
            ret = diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %d |%5s|%3d|%4d|%4d|%4d|%2d|%3d|%3d|%4d|%4d| %d |0x%06X|  %2d\n",
                l2_data.l2_idx,
                l2_data.vid,
                l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? 1 : 0,
                (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? l2_data.trk_gid : l2_data.port,
                (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0,
                (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0,
                (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0,
                (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP) ? 1 : 0,
                (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND) ? 1 : 0,
                (l2_data.flags&RTK_L2_UCAST_FLAG_TAG_STS) ? 1 : 0,
                l2_data.agg_pri,
                l2_data.agg_vid,
                (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? 1 : 0,
                (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? l2_data.l2_tunnel_idx : l2_data.ecid,
                l2_data.age);
            if (ret)
                break;
        }
#endif

        total_entry++;
    }
    diag_util_mprintf("\nTotal Number Of Entries : %d\n",total_entry);
    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DUMP_MAC_UCAST_PORT_TRUNK_PORT_TRUNK_ALL_DYNAMIC_STATIC_NEXTHOP_ALL
/*
l2-table dump mac-ucast ( port | trunk ) ( <MASK_LIST:port_trunk> | all ) ( dynamic | static |  nexthop | all )
*/
cparser_result_t cparser_cmd_l2_table_dump_mac_ucast_port_trunk_port_trunk_all_dynamic_static_nexthop_all(cparser_context_t *context,
    char **port_trunk_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t  l2_data;
    int32 scan_idx;
    uint32 total_entry;

    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    diag_trklist_t      trklist;
    rtk_trk_t           trunk = 0;
#if defined(CONFIG_SDK_RTL9300) ||  defined(CONFIG_SDK_RTL9310)
    char                buffer[6];
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2_data, 0, sizeof(rtk_l2_ucastAddr_t));

    scan_idx=-1;
    total_entry=0;

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf(" Index | VID | MAC Address       | SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Agg VID | Age | vlanTarget | mac_idx |\n");
        diag_util_mprintf("-------+-----+-------------------+------+----------+----------+---------+----------+---------+---------+-----+---------+-------\n");
    }
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        diag_util_mprintf(" Index | VID | MAC Address       | TRK | SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Aggreate VID | MAC Index |\n");
        diag_util_mprintf("-------+------+------------------+------+------+----------+----------+---------+----------+---------+---------+-------------\n");
    }
#endif

#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf(" Index | VID | MAC Address       |TRK|DevID|SPA|SBlk|DBlk|Sttc|NH|Sus|Tag|APri|AVID|Tnl|ECID/Idx| Age\n");
        diag_util_mprintf("-------+-----+-------------------+---+-----+---+----+----+----+--+---+---+----+----+---+--------+-----\n");
    }
#endif

    if ('p' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    }
    else if ('t' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 4), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    while (1)
    {
        if ((ret = rtk_l2_nextValidAddr_get(unit, (int32 *)&scan_idx, 1, &l2_data)) != RT_ERR_OK)
        {
            break;
        }

        if ( 'd' == TOKEN_CHAR(5, 0))
        {
            if((l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) || (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) || (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC))
                continue;

            if((l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP) || (l2_data.isAged))
                continue;

            if((l2_data.state&RTK_L2_UCAST_STATE_SUSPEND))
                continue;
        }
        else if ( 's' == TOKEN_CHAR(5, 0))
        {
            if(!(l2_data.flags&RTK_L2_UCAST_FLAG_STATIC))
                continue;
        }
        else if ( 'n' == TOKEN_CHAR(5, 0))
        {
            if(!(l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP))
                continue;
        }
        else if ( 'a' == TOKEN_CHAR(5, 0))
        {

        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        if ('p' == TOKEN_CHAR(3, 0))
        {
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                if((l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) || (l2_data.port!= port))
                {
                    continue;
                }

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
                    DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
                {
                    diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %8d | %8d | %7d | %8d | %7d |   %4d  |  %2d | %10d | %8d \n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                             l2_data.port, (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                             (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                             (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0, l2_data.agg_vid, l2_data.age,
                             ((l2_data.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? l2_data.vlan_target : 0),
                             ((l2_data.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? l2_data.route_idx : 0));

                }
#endif

#if defined(CONFIG_SDK_RTL9300)
                if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
                {
                    sprintf(buffer, "%d", l2_data.devID);
                    if(l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)
                    {
                        diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4s | %4d | %8d | %8d | %7d | %8d | %7d | %s | %7d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                    (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                    "Not Support",
                                    l2_data.route_idx);
                    }
                    else
                    {
                        diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4s | %4d | %8d | %8d | %7d | %8d | %7d | %7d | %s\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                 (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                 l2_data.agg_vid,
                                 "Not Support");
                    }
                }
#endif

#if defined(CONFIG_SDK_RTL9310)
                if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
                {
                    sprintf(buffer, "%d", l2_data.devID);
                    diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %d |%5s|%3d|%4d|%4d|%4d|%2d|%3d|%3d|%4d|%4d| %d |0x%06X|  %2d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? l2_data.trk_gid : l2_data.port,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP) ? 1 : 0,
                                    (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TAG_STS) ? 1 : 0,
                                    l2_data.agg_pri,
                                    l2_data.agg_vid,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? l2_data.l2_tunnel_idx : l2_data.ecid,
                                    l2_data.age);
                }
#endif
                total_entry++;
            }
        }
        else if ('t' == TOKEN_CHAR(3, 0))
        {
            DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
            {
                if(!(l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) || (l2_data.trk_gid!= trunk))
                    continue;

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
                if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
                    DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
                {
                    diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %8d | %8d | %7d | %8d | %7d |   %4d  |  %2d | %10d | %8d \n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                             l2_data.port, (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                             (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0, (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                             (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0, l2_data.agg_vid, l2_data.age,
                             ((l2_data.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? l2_data.vlan_target : 0),
                             ((l2_data.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? l2_data.route_idx : 0));

                }
#endif

#if defined(CONFIG_SDK_RTL9300)
                if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
                {
                    sprintf(buffer, "%d", l2_data.devID);
                    if(l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)
                    {
                        diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4s | %4d | %8d | %8d | %7d | %8d | %7d | %s | %7d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                    (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                    "Not Support",
                                    l2_data.route_idx);
                    }
                    else
                    {
                        diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4s | %4d | %8d | %8d | %7d | %8d | %7d | %7d | %s\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                 (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                 l2_data.agg_vid,
                                 "Not Support");
                    }
                }
#endif

#if defined(CONFIG_SDK_RTL9310)
                if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
                {
                    sprintf(buffer, "%d", l2_data.devID);
                    diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %3d | %4s | %3d | %7d | %7d | %6d | %2d | %7d |   %4d  |   %4d  |  %2d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? l2_data.trk_gid : l2_data.port,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP) ? 1 : 0,
                                    (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND) ? 1 : 0,
                                    l2_data.agg_pri,
                                    l2_data.agg_vid,
                                    l2_data.age);
                }
#endif
                total_entry++;
            }
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    diag_util_mprintf("\nTotal Number Of Entries : %d\n", total_entry);
    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DUMP_MAC_MCAST_NEXTHOP
/*
l2-table dump mac-mcast { nexthop }
*/
cparser_result_t cparser_cmd_l2_table_dump_mac_mcast_nexthop(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_mcastAddr_t  mcast_data;
    char                port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    int32 scan_idx;
    uint32 total_entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&mcast_data, 0, sizeof(rtk_l2_mcastAddr_t));

#if defined(CONFIG_SDK_RTL8380)
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID  | MAC address       | Port                 | Agg Vid\n");
            diag_util_mprintf("-------+------+-------------------+----------------------+-----------\n");
        }
#endif

        if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID  | MAC address       | Port                \n");
            diag_util_mprintf("-------+------+-------------------+----------------------\n");
        }

#if defined(CONFIG_SDK_RTL9300)
        if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID  | MAC address       | Port                 | nexthop | MAC Index\n");
            diag_util_mprintf("-------+-----+-------------------+----------------------+---------+---------\n");
        }
#endif

#if defined(CONFIG_SDK_RTL9310)
        if(DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID  | MAC address       | Port                 | NH | Group ID \n");
            diag_util_mprintf("-------+------+-------------------+----------------------+----+------------\n");
        }
#endif

 /* show all l2-macmcast */
    scan_idx = -1; /* get the first entry */
    total_entry = 0;
    while (1)
    {
        if ((ret = rtk_l2_nextValidMcastAddr_get(unit, &scan_idx, &mcast_data)) != RT_ERR_OK)
        {
            if(ret == RT_ERR_L2_ENTRY_NOTFOUND || ret == RT_ERR_ENTRY_NOTFOUND)
            {
                if (total_entry == 0)
                    diag_util_printf("Entry is not exist\n");
                break;
            }
            else
            {
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
            }
        }

#if defined(CONFIG_SDK_RTL8380)
        if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &mcast_data.portmask);

            diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d\n",
                        mcast_data.l2_idx,
                        mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                        mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.agg_vid);
        }
#endif

        if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &mcast_data.portmask);

            diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s \n",
                        mcast_data.l2_idx,
                        mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                        mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list);
        }

#if defined(CONFIG_SDK_RTL9300)
        if(DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            if (TOKEN_NUM == 4 && 'n' == TOKEN_CHAR(3, 0))
            {
                if(mcast_data.nextHop== 0)
                    continue;
            }

            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &mcast_data.portmask);

            if(mcast_data.nextHop)
            {
                diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d | %4d\n",
                            mcast_data.l2_idx,
                            mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                            mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.nextHop, mcast_data.mac_idx);
            }
            else
            {
                diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d | %s\n",
                            mcast_data.l2_idx,
                            mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                            mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.nextHop, "Not Support");
            }
        }
#endif

#if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            if (TOKEN_NUM == 4 && 'n' == TOKEN_CHAR(3, 0))
            {
                if (mcast_data.nextHop == 0)
                    continue;
            }

            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &mcast_data.portmask);

            diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %2d | 0x%X\n",
                        mcast_data.l2_idx,
                        mcast_data.rvid, mcast_data.mac.octet[0], mcast_data.mac.octet[1], mcast_data.mac.octet[2],
                        mcast_data.mac.octet[3], mcast_data.mac.octet[4], mcast_data.mac.octet[5], port_list, mcast_data.nextHop, mcast_data.groupId);
        }
#endif

        total_entry++;
    }
    diag_util_mprintf("\nTotal Number Of Entries : %d\n",total_entry);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_STACK_AUTO_LEARN_STATE
/*
l2-table get stack auto-learn state
*/
cparser_result_t cparser_cmd_l2_table_get_stack_auto_learn_state(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    enable = 0;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();


    ret = rtk_l2_stkLearningEnable_get(unit, &enable);
    if ( (ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    else
    {
        diag_util_mprintf("\tStacking port auto-learn state        : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_STACK_AUTO_LEARN_STATE_DISABLE_ENABLE
/*
l2-table set stack auto-learn state ( disable | enable )
*/
cparser_result_t cparser_cmd_l2_table_set_stack_auto_learn_state_disable_enable(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(5, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_stkLearningEnable_set(unit, ENABLED), ret);
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_stkLearningEnable_set(unit, DISABLED), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_STACK_UCAST_AGE_OUT_VALID_STATE
/*
l2-table get stack ucast-age-out-valid state
*/
cparser_result_t cparser_cmd_l2_table_get_stack_ucast_age_out_valid_state(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_enable_t                    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_l2_stkKeepUcastEntryValid_get(unit, &enable), ret);
    diag_util_mprintf("L2 stack ucast age-out valid Status : %s\n", enable ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_SET_STACK_UCAST_AGE_OUT_VALID_STATE_DISABLE_ENABLE
/*
l2-table set stack ucast-age-out-valid state ( disable | enable )
*/
cparser_result_t cparser_cmd_l2_table_set_stack_ucast_age_out_valid_state_disable_enable(cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(5, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_stkKeepUcastEntryValid_set(unit, ENABLED), ret);
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_stkKeepUcastEntryValid_set(unit, DISABLED), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif


#ifdef CMD_L2_TABLE_SET_PORT_MOVE_DYNAMIC_PORT_MOVE_FORBID_ACTION_COPY_TO_CPU_DROP_FORWARD_TRAP_TO_CPU
/*
 * l2-table set port-move dynamic-port-move-forbid action ( copy-to-cpu | drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_port_move_dynamic_port_move_forbid_action_copy_to_cpu_drop_forward_trap_to_cpu(
    cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_portMoveAct_t    act;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    if ('f' == TOKEN_CHAR(5, 0))
    {
        act.forbidAct.act  = ACTION_FORWARD;
    }
    else if('d' == TOKEN_CHAR(5, 0))
    {
        act.forbidAct.act  = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        act.forbidAct.act  = ACTION_TRAP2CPU;
    }
    else if ('c' == TOKEN_CHAR(5, 0))
    {
        act.forbidAct.act  = ACTION_COPY2CPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_dynamicPortMoveForbidAction_set(unit, act.forbidAct.act), ret);
    }
    else
#endif
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portMoveAction_set(unit, L2_PORT_MOVE_FORBID, &act), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_port_move_dynamic_port_move_forbid_action_copy_to_cpu_drop_forward_trap_to_cpu */
#endif

#ifdef CMD_L2_TABLE_SET_PORT_MOVE_DYNAMIC_PORT_MOVE_FORBID_ACTION_COPY_TO_MASTER_TRAP_TO_MASTER
/*
 * l2-table set port-move dynamic-port-move-forbid action ( copy-to-master | trap-to-master )
 */
cparser_result_t
cparser_cmd_l2_table_set_port_move_dynamic_port_move_forbid_action_copy_to_master_trap_to_master(
    cparser_context_t *context)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_portMoveAct_t    act;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('c' == TOKEN_CHAR(5, 0))
    {
        act.forbidAct.act  = ACTION_COPY2MASTERCPU;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        act.forbidAct.act  = ACTION_TRAP2MASTERCPU;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_dynamicPortMoveForbidAction_set(unit, act.forbidAct.act), ret);
    }
    else
#endif
    {
    DIAG_UTIL_ERR_CHK(rtk_l2_portMoveAction_set(unit, L2_PORT_MOVE_FORBID, &act), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_port_move_dynamic_port_move_forbid_action_copy_to_master_trap_to_master */
#endif

#ifdef CMD_L2_TABLE_GET_PORT_MOVE_DYNAMIC_PORT_MOVE_FORBID_ACTION
/*
 * l2-table get port-move dynamic-port-move-forbid action
 */
cparser_result_t
cparser_cmd_l2_table_get_port_move_dynamic_port_move_forbid_action(
    cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_portMoveAct_t act;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

#if defined (CONFIG_SDK_DRIVER_RTK_LEGACY_API)
    if (IS_BACKWARD_COMPATIBLE)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_dynamicPortMoveForbidAction_get(unit, &act.forbidAct.act), ret);
    }
    else
#endif
    {
    DIAG_UTIL_ERR_CHK(rtk_l2_portMoveAction_get(unit, L2_PORT_MOVE_FORBID, &act), ret);
    }

    diag_util_mprintf("\tAction:              ");
    if (act.forbidAct.act == ACTION_FORWARD)
    {
        diag_util_mprintf("Forward\n");
    }
    else if (act.forbidAct.act  == ACTION_DROP)
    {
        diag_util_mprintf("Drop\n");
    }
    else if (act.forbidAct.act  == ACTION_TRAP2CPU)
    {
        diag_util_mprintf("Trap-To-Cpu\n");
    }
    else if (act.forbidAct.act  == ACTION_COPY2CPU)
    {
        diag_util_mprintf("Copy-To-Cpu\n");
    }
    else if (act.forbidAct.act  == ACTION_TRAP2MASTERCPU)
    {
        diag_util_mprintf("Trap-To-Master Cpu\n");
    }
    else if (act.forbidAct.act  == ACTION_COPY2MASTERCPU)
    {
        diag_util_mprintf("Copy-To-Master Cpu\n");
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_get_port_move_dynamic_port_move_forbid_action */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_FLUSH_ENTRY_DEVID_DEVID_PORT_TRUNK_ID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
 * l2-table set flush flush-entry devID <UINT:devID> ( port | trunk ) <UINT:id> ( dynamic-only | include-static )
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_flush_entry_devID_devID_port_trunk_id_dynamic_only_include_static(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    uint32_t *id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;

    if ('p' == TOKEN_CHAR(6, 0))
    {
        config.devID = *devid_ptr;
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(8, 0))
    {
        config.act = FLUSH_ACT_FLUSH_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(8, 0))
    {
        config.act = FLUSH_ACT_FLUSH_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_flush_entry_devID_devID_port_trunk_id_dynamic_only_include_static */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_FLUSH_ENTRY_DEVID_DEVID_PORT_TRUNK_ID_VID_VID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
 * l2-table set flush flush-entry devID <UINT:devID> ( port | trunk ) <UINT:id> vid <UINT:vid> ( dynamic-only | include-static )
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_flush_entry_devID_devID_port_trunk_id_vid_vid_dynamic_only_include_static(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    uint32_t *id_ptr,
    uint32_t *vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(6, 0))
    {
        config.devID = *devid_ptr;
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    if ('d' == TOKEN_CHAR(10, 0))
    {
        config.act = FLUSH_ACT_FLUSH_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(10, 0))
    {
        config.act = FLUSH_ACT_FLUSH_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_flush_entry_devID_devID_port_trunk_id_vid_vid_dynamic_only_include_static */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_REPLACE_ENTRY_REPLACE_DEV_REPLACE_DEVID_REPLACE_PORT_REPLACE_TRUNK_REPLACE_ID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
 * l2-table set flush replace-entry replace-dev <UINT:replace_devID> ( replace-port | replace-trunk ) <UINT:replace_id> ( dynamic-only | include-static )
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_replace_entry_replace_dev_replace_devID_replace_port_replace_trunk_replace_id_dynamic_only_include_static(
    cparser_context_t *context,
    uint32_t *replace_devid_ptr,
    uint32_t *replace_id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    if ('p' == TOKEN_CHAR(6, 8))
    {
        config.replacing_devID = *replace_devid_ptr;
        config.replacePortOrTrunk = ENABLED;
        config.replacingPort = *replace_id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 8))
    {
        config.replacePortOrTrunk = DISABLED;
        config.replacingTrk = *replace_id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(8, 0))
    {
        config.act = FLUSH_ACT_REPLACE_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(8, 0))
    {
        config.act = FLUSH_ACT_REPLACE_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_replace_entry_replace_dev_replace_devID_replace_port_replace_trunk_replace_id_dynamic_only_include_static */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_REPLACE_ENTRY_DEVID_DEVID_PORT_TRUNK_ID_REPLACE_DEV_REPLACE_DEVID_REPLACE_PORT_REPLACE_TRUNK_REPLACE_ID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
 * l2-table set flush replace-entry devID <UINT:devID> ( port | trunk ) <UINT:id> replace-dev <UINT:replace_devID> ( replace-port | replace-trunk ) <UINT:replace_id>  ( dynamic-only | include-static )
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_replace_entry_devID_devID_port_trunk_id_replace_dev_replace_devID_replace_port_replace_trunk_replace_id_dynamic_only_include_static(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    uint32_t *id_ptr,
    uint32_t *replace_devid_ptr,
    uint32_t *replace_id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(6, 0))
    {
        config.devID = *devid_ptr;
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('p' == TOKEN_CHAR(10, 8))
    {
        config.replacing_devID = *replace_devid_ptr;
        config.replacePortOrTrunk = ENABLED;
        config.replacingPort = *replace_id_ptr;
    }
    else if ('t' == TOKEN_CHAR(10, 8))
    {
        config.replacePortOrTrunk = DISABLED;
        config.replacingTrk = *replace_id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(12, 0))
    {
        config.act = FLUSH_ACT_REPLACE_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(12, 0))
    {
        config.act = FLUSH_ACT_REPLACE_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_replace_entry_devID_devID_port_trunk_id_replace_dev_replace_devID_replace_port_replace_trunk_replace_id_dynamic_only_include_static */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_REPLACE_ENTRY_VID_VID_REPLACE_DEV_REPLACE_DEVID_REPLACE_PORT_REPLACE_TRUNK_REPLACE_ID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
 * l2-table set flush replace-entry vid <UINT:vid> replace-dev <UINT:replace_devID> ( replace-port | replace-trunk ) <UINT:replace_id> ( dynamic-only | include-static )
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_replace_entry_vid_vid_replace_dev_replace_devID_replace_port_replace_trunk_replace_id_dynamic_only_include_static(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    uint32_t *replace_devid_ptr,
    uint32_t *replace_id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    if ('p' == TOKEN_CHAR(8, 8))
    {
        config.replacing_devID = *replace_devid_ptr;
        config.replacePortOrTrunk = ENABLED;
        config.replacingPort = *replace_id_ptr;
    }
    else if ('t' == TOKEN_CHAR(8, 8))
    {
        config.replacePortOrTrunk = DISABLED;
        config.replacingTrk = *replace_id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(10, 0))
    {
        config.act = FLUSH_ACT_REPLACE_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(10, 0))
    {
        config.act = FLUSH_ACT_REPLACE_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_replace_entry_vid_vid_replace_dev_replace_devID_replace_port_replace_trunk_replace_id_dynamic_only_include_static */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_REPLACE_ENTRY_DEVID_DEVID_PORT_TRUNK_ID_VID_VID_REPLACE_DEV_REPLACE_DEVID_REPLACE_PORT_REPLACE_TRUNK_REPLACE_ID_DYNAMIC_ONLY_INCLUDE_STATIC
/*
 * l2-table set flush replace-entry devID <UINT:devID> ( port | trunk ) <UINT:id> vid <UINT:vid> replace-dev <UINT:replace_devID> ( replace-port | replace-trunk ) <UINT:replace_id> ( dynamic-only | include-static )
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_replace_entry_devID_devID_port_trunk_id_vid_vid_replace_dev_replace_devID_replace_port_replace_trunk_replace_id_dynamic_only_include_static(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    uint32_t *id_ptr,
    uint32_t *vid_ptr,
    uint32_t *replace_devid_ptr,
    uint32_t *replace_id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(6, 0))
    {
        config.devID = *devid_ptr;
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    if ('p' == TOKEN_CHAR(12, 8))
    {
        config.replacing_devID = *replace_devid_ptr;
        config.replacePortOrTrunk = ENABLED;
        config.replacingPort = *replace_id_ptr;
    }
    else if ('t' == TOKEN_CHAR(12, 8))
    {
        config.replacePortOrTrunk = DISABLED;
        config.replacingTrk = *replace_id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(14, 0))
    {
        config.act = FLUSH_ACT_REPLACE_DYNM_ONLY;
    }
    else if ('i' == TOKEN_CHAR(14, 0))
    {
        config.act = FLUSH_ACT_REPLACE_ALL_UC;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_replace_entry_devID_devID_port_trunk_id_vid_vid_replace_dev_replace_devID_replace_port_replace_trunk_replace_id_dynamic_only_include_static */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_NEXTHOP_DEVID_DEVID_PORT_TRUNK_ID
/*
 * l2-table set flush clear-nexthop devID <UINT:devID> ( port | trunk ) <UINT:id>
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_clear_nexthop_devID_devID_port_trunk_id(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    uint32_t *id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(6, 0))
    {
        config.devID = *devid_ptr;
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.act = FLUSH_ACT_CLEAR_NEXTHOP;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_clear_nexthop_devID_devID_port_trunk_id */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_NEXTHOP_DEVID_DEVID_PORT_TRUNK_ID_VID_VID
/*
 * l2-table set flush clear-nexthop devID <UINT:devID> ( port | trunk ) <UINT:id> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_clear_nexthop_devID_devID_port_trunk_id_vid_vid(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    uint32_t *id_ptr,
    uint32_t *vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(6, 0))
    {
        config.devID = *devid_ptr;
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    config.act = FLUSH_ACT_CLEAR_NEXTHOP;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_clear_nexthop_devID_devID_port_trunk_id_vid_vid */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_DEVID_DEVID_PORT_TRUNK_ID
/*
 * l2-table set flush clear-aggVid devID <UINT:devID> ( port | trunk ) <UINT:id>
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_clear_aggVid_devID_devID_port_trunk_id(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    uint32_t *id_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(6, 0))
    {
        config.devID = *devid_ptr;
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_clear_aggVid_devID_devID_port_trunk_id */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_DEVID_DEVID_PORT_TRUNK_ID_VID_VID
/*
 * l2-table set flush clear-aggVid devID <UINT:devID> ( port | trunk ) <UINT:id> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_clear_aggVid_devID_devID_port_trunk_id_vid_vid(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    uint32_t *id_ptr,
    uint32_t *vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(6, 0))
    {
        config.devID = *devid_ptr;
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_clear_aggVid_devID_devID_port_trunk_id_vid_vid */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_DEVID_DEVID_PORT_TRUNK_ID_AGG_VID_AGG_VID
/*
 * l2-table set flush clear-aggVid devID <UINT:devID> ( port | trunk ) <UINT:id> agg-vid <UINT:agg_vid>
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_clear_aggVid_devID_devID_port_trunk_id_agg_vid_agg_vid(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    uint32_t *id_ptr,
    uint32_t *agg_vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(6, 0))
    {
        config.devID = *devid_ptr;
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushFlag |= RTK_L2_FLUSH_BY_AGGVID;
    config.agg_vid = *agg_vid_ptr;

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_clear_aggVid_devID_devID_port_trunk_id_agg_vid_agg_vid */
#endif

#ifdef CMD_L2_TABLE_SET_FLUSH_CLEAR_AGGVID_DEVID_DEVID_PORT_TRUNK_ID_VID_VID_AGG_VID_AGG_VID
/*
 * l2-table set flush clear-aggVid devID <UINT:devID> ( port | trunk ) <UINT:id> vid <UINT:vid> agg-vid <UINT:agg_vid>
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_clear_aggVid_devID_devID_port_trunk_id_vid_vid_agg_vid_agg_vid(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    uint32_t *id_ptr,
    uint32_t *vid_ptr,
    uint32_t *agg_vid_ptr)
{
    uint32                          unit = 0;
    int32                            ret = RT_ERR_FAILED;
    rtk_l2_flushCfg_t           config;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&config, 0, sizeof(rtk_l2_flushCfg_t));

    config.flushByPort = ENABLED;
    if ('p' == TOKEN_CHAR(6, 0))
    {
        config.devID = *devid_ptr;
        config.portOrTrunk = ENABLED;
        config.port = *id_ptr;
    }
    else if ('t' == TOKEN_CHAR(6, 0))
    {
        config.portOrTrunk = DISABLED;
        config.trunk = *id_ptr;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    config.flushByVid = ENABLED;
    config.vid = *vid_ptr;

    config.flushFlag |= RTK_L2_FLUSH_BY_AGGVID;
    config.agg_vid = *agg_vid_ptr;

    config.act = FLUSH_ACT_CLEAR_AGG_VID;

    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(unit, &config), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_set_flush_clear_aggVid_devID_devID_port_trunk_id_vid_vid_agg_vid_agg_vid */
#endif

#ifdef CMD_L2_TABLE_DUMP_MAC_UCAST_DEVID_DEVID_PORT_TRUNK_PORT_TRUNK_ALL_DYNAMIC_STATIC_NEXTHOP_ALL
/*
 * l2-table dump mac-ucast devID <UINT:devID> ( port | trunk ) ( <MASK_LIST:port_trunk> | all ) ( dynamic | static |  nexthop | all )
 */
cparser_result_t
cparser_cmd_l2_table_dump_mac_ucast_devID_devID_port_trunk_port_trunk_all_dynamic_static_nexthop_all(
    cparser_context_t *context,
    uint32_t *devid_ptr,
    char **port_trunk_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_l2_ucastAddr_t  l2_data;
    int32 scan_idx;
    uint32 total_entry;
    diag_portlist_t     portlist;
    rtk_port_t          port = 0;
    diag_trklist_t      trklist;
    rtk_trk_t           trunk = 0;
#if (defined(CONFIG_SDK_RTL9310) || (defined(CONFIG_SDK_RTL9300)))
    char                buffer[6];
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2_data, 0, sizeof(rtk_l2_ucastAddr_t));

    scan_idx=-1;
    total_entry=0;

#if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        diag_util_mprintf(" Index | VID  | MAC Address       | TRK | DevID | SPA  | SaBlock  | DaBlock  | Static  | Nexthop  | Suspend | Aggreate VID | MAC Index |\n");
        diag_util_mprintf("-------+------+-------------------+-----+-------+------+----------+----------+---------+----------+---------+--------------+-------------\n");
    }
#endif

#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf(" Index | VID | MAC Address       |TRK|DevID|SPA|SBlk|DBlk|Sttc|NH|Sus|Tag|APri|AVID|Tnl|ECID/Idx| Age\n");
        diag_util_mprintf("-------+-----+-------------------+---+-----+---+----+----+----+--+---+---+----+----+---+--------+-----\n");
    }
#endif

    if ('p' == TOKEN_CHAR(5, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_TRKLIST(trklist, 6), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    while (1)
    {
        if ((ret = rtk_l2_nextValidAddr_get(unit, (int32 *)&scan_idx, 1, &l2_data)) != RT_ERR_OK)
        {
            break;
        }

        if ( 'd' == TOKEN_CHAR(7, 0))
        {
            if((l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) || (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) || (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC))
                continue;

            if((l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP) || (l2_data.isAged))
                continue;

            if((l2_data.state&RTK_L2_UCAST_STATE_SUSPEND))
                continue;
        }
        else if ( 's' == TOKEN_CHAR(7, 0))
        {
            if(!(l2_data.flags&RTK_L2_UCAST_FLAG_STATIC))
                continue;
        }
        else if ( 'n' == TOKEN_CHAR(7, 0))
        {
            if(!(l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP))
                continue;
        }
        else if ( 'a' == TOKEN_CHAR(7, 0))
        {

        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }

        if ('p' == TOKEN_CHAR(5, 0))
        {
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                if((l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) || l2_data.devID != *devid_ptr || l2_data.port!= port)
                {
                    continue;
                }

#if defined(CONFIG_SDK_RTL9300)
                if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
                {
                    sprintf(buffer, "%d", l2_data.devID);
                    if(l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)
                    {
                        diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4s | %4d | %8d | %8d | %7d | %8d | %7d | %s | %7d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                    (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                    "Not Support",
                                    l2_data.route_idx);
                    }
                    else
                    {
                        diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4s | %4d | %8d | %8d | %7d | %8d | %7d | %7d | %s\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                 (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                 l2_data.agg_vid,
                                 "Not Support");
                    }
                }
#endif


#if defined(CONFIG_SDK_RTL9310)
                if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
                {
                    sprintf(buffer, "%d", l2_data.devID);
                    diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %d |%5s|%3d|%4d|%4d|%4d|%2d|%3d|%3d|%4d|%4d| %d |0x%06X|  %2d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? l2_data.trk_gid : l2_data.port,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP) ? 1 : 0,
                                    (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TAG_STS) ? 1 : 0,
                                    l2_data.agg_pri,
                                    l2_data.agg_vid,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? l2_data.l2_tunnel_idx : l2_data.ecid,
                                    l2_data.age);
                }
#endif
                total_entry++;
            }
        }
        else if ('t' == TOKEN_CHAR(5, 0))
        {
            DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)
            {
                if(!(l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) || (l2_data.trk_gid!= trunk))
                    continue;

#if defined(CONFIG_SDK_RTL9300)
                if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
                {
                    sprintf(buffer, "%d", l2_data.devID);
                    if(l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)
                    {
                        diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4s | %4d | %8d | %8d | %7d | %8d | %7d | %s | %7d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                    (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                    "Not Support",
                                    l2_data.route_idx);
                    }
                    else
                    {
                        diag_util_mprintf("%6d | %4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %4s | %4d | %8d | %8d | %7d | %8d | %7d | %7d | %s\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? (l2_data.devID==1 ? "ntfy" : " - ") : buffer,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?l2_data.trk_gid:l2_data.port,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                                 (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                                 (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                                 l2_data.agg_vid,
                                 "Not Support");
                    }
                }
#endif
#if defined(CONFIG_SDK_RTL9310)
                if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
                {
                    sprintf(buffer, "%d", l2_data.devID);
                    diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %d |%5s|%3d|%4d|%4d|%4d|%2d|%3d|%3d|%4d|%4d| %d |0x%06X|  %2d\n",
                                    l2_data.l2_idx,
                                    l2_data.vid,
                                    l2_data.mac.octet[0],l2_data.mac.octet[1],l2_data.mac.octet[2],l2_data.mac.octet[3],l2_data.mac.octet[4],l2_data.mac.octet[5],
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? l2_data.trk_gid : l2_data.port,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_NEXTHOP) ? 1 : 0,
                                    (l2_data.state&RTK_L2_UCAST_STATE_SUSPEND) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_TAG_STS) ? 1 : 0,
                                    l2_data.agg_pri,
                                    l2_data.agg_vid,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? 1 : 0,
                                    (l2_data.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? l2_data.l2_tunnel_idx : l2_data.ecid,
                                    l2_data.age);
                }
#endif
                total_entry++;
            }
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    diag_util_mprintf("\nTotal Number Of Entries : %d\n", total_entry);
    return CPARSER_OK;
}   /* end of cparser_cmd_l2_table_dump_mac_ucast_devID_devID_port_trunk_port_trunk_all_dynamic_static_nexthop_all */
#endif

#ifdef CMD_L2_TABLE_GET_CONFLICT_ENTRY_MAC_UCAST_VID_MAC
/*
 * l2-table get conflict-entry mac-ucast <UINT:vid> <MACADDR:mac>
 */
cparser_result_t cparser_cmd_l2_table_get_conflict_entry_mac_ucast_vid_mac(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr)
{
#define MAX_HASH_BUCKET_NUM 8
    uint32  unit = 0, ret_cnt = 0, i;
    uint8   contain_ucst = 0, contain_mcst = 0;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    uint8   contain_ipmcst = 0, contain_ip6mcst = 0;
#endif
    int32   ret = RT_ERR_FAILED;
    rtk_l2_entry_t  input_l2_entry;
    rtk_l2_entry_t  output_l2_entry[MAX_HASH_BUCKET_NUM];
    char        port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    char        strBuf1[32], strBuf2[32];
#endif
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    char                buffer[6];
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&input_l2_entry, 0, sizeof(input_l2_entry));
    osal_memset(&output_l2_entry, 0, sizeof(output_l2_entry));
    input_l2_entry.entry_type = FLOW_TYPE_UNICAST;
    input_l2_entry.unicast.vid = *vid_ptr;
    osal_memcpy(input_l2_entry.unicast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_conflictAddr_get(unit, &input_l2_entry, &output_l2_entry[0], MAX_HASH_BUCKET_NUM, &ret_cnt), ret);

    for (i = 0; i < ret_cnt; i++)
    {
        if (output_l2_entry[i].entry_type == FLOW_TYPE_UNICAST)
            contain_ucst = 1;
        if (output_l2_entry[i].entry_type == FLOW_TYPE_L2_MULTI)
            contain_mcst = 1;
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
        if (output_l2_entry[i].entry_type == FLOW_TYPE_IP4_MULTI)
            contain_ipmcst = 1;
        if (output_l2_entry[i].entry_type == FLOW_TYPE_IP6_MULTI)
            contain_ip6mcst = 1;
#endif
    }

    if (ret_cnt && contain_ucst)
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID | MAC Address       | SPA | SBlk | DBlk | Static | Nexthop | Suspend | Agg VID | Age\n");
            diag_util_mprintf("-------+-----+-------------------+-----+------+------+--------+---------+---------+---------+-----\n");
        }
        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID | MAC Address       | TRK | DevID | SPA |SBlk|DBlk|Sttc| NH |Sus| Agg VID | MAC Index \n");
            diag_util_mprintf("-------+-----+-------------------+-----+------+-----+----+----+----+----+---+---------+------------\n");
        }
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID | MAC Address       |TRK|DevID|SPA|SBlk|DBlk|Sttc|NH|Sus|Tag|APri|AVID|Tnl|ECID/Idx| Age\n");
            diag_util_mprintf("-------+-----+-------------------+---+-----+---+----+----+----+--+---+---+----+----+---|--------+-----\n");
        }
    }

    /* L2 unicast */
    for (i = 0; i < ret_cnt; i++)
    {
        if (output_l2_entry[i].entry_type == FLOW_TYPE_UNICAST)
        {
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
            if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
                DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %4d | %8d | %8d | %7d | %8d | %7d |   %4d  |  %2d | %10d | %8d \n",
                                output_l2_entry[i].unicast.l2_idx,
                                output_l2_entry[i].unicast.vid,
                                output_l2_entry[i].unicast.mac.octet[0],output_l2_entry[i].unicast.mac.octet[1],output_l2_entry[i].unicast.mac.octet[2],output_l2_entry[i].unicast.mac.octet[3],output_l2_entry[i].unicast.mac.octet[4],output_l2_entry[i].unicast.mac.octet[5],
                         output_l2_entry[i].unicast.port, (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0, (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                         (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0, (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                         (output_l2_entry[i].unicast.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0, output_l2_entry[i].unicast.agg_vid, output_l2_entry[i].unicast.age,
                         ((output_l2_entry[i].unicast.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? output_l2_entry[i].unicast.vlan_target : 0),
                         ((output_l2_entry[i].unicast.flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? output_l2_entry[i].unicast.route_idx : 0));

            }
#endif
#if defined(CONFIG_SDK_RTL9300)
            if(output_l2_entry[i].unicast.flags & RTK_L2_UCAST_FLAG_NEXTHOP)
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %3d | %4d | %3d |%4d|%4d|%4d| %2d |%3d| %s | %7d\n",
                            output_l2_entry[i].unicast.l2_idx,
                            output_l2_entry[i].unicast.vid,
                            output_l2_entry[i].unicast.mac.octet[0],output_l2_entry[i].unicast.mac.octet[1],output_l2_entry[i].unicast.mac.octet[2],output_l2_entry[i].unicast.mac.octet[3],output_l2_entry[i].unicast.mac.octet[4],output_l2_entry[i].unicast.mac.octet[5],
                            (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                            (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                            (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?output_l2_entry[i].unicast.trk_gid:output_l2_entry[i].unicast.port,
                            (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                            (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                            (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                            (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                            (output_l2_entry[i].unicast.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                            "Not Support",
                            output_l2_entry[i].unicast.route_idx);
            }
            else
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %3d | %4d | %3d |%4d|%4d|%4d| %2d |%3d| %7d | %s\n",
                            output_l2_entry[i].unicast.l2_idx,
                            output_l2_entry[i].unicast.vid,
                            output_l2_entry[i].unicast.mac.octet[0],output_l2_entry[i].unicast.mac.octet[1],output_l2_entry[i].unicast.mac.octet[2],output_l2_entry[i].unicast.mac.octet[3],output_l2_entry[i].unicast.mac.octet[4],output_l2_entry[i].unicast.mac.octet[5],
                             (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?1:0,
                             (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                             (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT)?output_l2_entry[i].unicast.trk_gid:output_l2_entry[i].unicast.port,
                             (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_SA_BLOCK)?1:0,
                             (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_DA_BLOCK)?1:0,
                             (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_STATIC)?1:0,
                             (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_NEXTHOP)?1:0,
                             (output_l2_entry[i].unicast.state&RTK_L2_UCAST_STATE_SUSPEND)?1:0,
                             output_l2_entry[i].unicast.agg_vid,
                             "Not Support");
            }
#endif
#if defined(CONFIG_SDK_RTL9310)
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                sprintf(buffer, "%d", output_l2_entry[i].unicast.devID);
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %d |%5s|%3d|%4d|%4d|%4d|%2d|%3d|%3d|%4d|%4d| %d |0x%06X|  %2d\n",
                                output_l2_entry[i].unicast.l2_idx,
                                output_l2_entry[i].unicast.vid,
                                output_l2_entry[i].unicast.mac.octet[0],output_l2_entry[i].unicast.mac.octet[1],output_l2_entry[i].unicast.mac.octet[2],output_l2_entry[i].unicast.mac.octet[3],output_l2_entry[i].unicast.mac.octet[4],output_l2_entry[i].unicast.mac.octet[5],
                                (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? 1 : 0,
                                (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? " - " : buffer,
                                (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_TRUNK_PORT) ? output_l2_entry[i].unicast.trk_gid : output_l2_entry[i].unicast.port,
                                (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0,
                                (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0,
                                (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0,
                                (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_NEXTHOP) ? 1 : 0,
                                (output_l2_entry[i].unicast.state&RTK_L2_UCAST_STATE_SUSPEND) ? 1 : 0,
                                (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_TAG_STS) ? 1 : 0,
                                output_l2_entry[i].unicast.agg_pri,
                                output_l2_entry[i].unicast.agg_vid,
                                (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? 1 : 0,
                                (output_l2_entry[i].unicast.flags&RTK_L2_UCAST_FLAG_L2_TUNNEL) ? output_l2_entry[i].unicast.l2_tunnel_idx : output_l2_entry[i].unicast.ecid,
                                output_l2_entry[i].unicast.age);
            }
#endif
        }
    }


    /* L2 multicast */
    if (ret_cnt && contain_mcst)
    {
        diag_util_mprintf("\n");
        if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID | MAC address       | Port                 | Agg Vid\n");
            diag_util_mprintf("-------+-----+-------------------+----------------------+-----------\n");
        }
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID | MAC address       | Port                \n");
            diag_util_mprintf("-------+-----+-------------------+----------------------\n");
        }
        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID | MAC address       | Port                 | nexthop | MAC Index\n");
            diag_util_mprintf("-------+-----+-------------------+----------------------+---------+---------\n");
        }
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            diag_util_mprintf(" Index | VID | MAC address       | Port                 | NH | Group ID \n");
            diag_util_mprintf("-------+-----+-------------------+----------------------+----+------------\n");
        }
    }

    for (i = 0; i < ret_cnt; i++)
    {
        if (output_l2_entry[i].entry_type == FLOW_TYPE_L2_MULTI)
        {
            osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
            diag_util_lPortMask2str(port_list, &output_l2_entry[i].l2mcast.portmask);
#if defined(CONFIG_SDK_RTL8380)
            if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d\n",
                            output_l2_entry[i].l2mcast.l2_idx,
                            output_l2_entry[i].l2mcast.rvid, output_l2_entry[i].l2mcast.mac.octet[0], output_l2_entry[i].l2mcast.mac.octet[1], output_l2_entry[i].l2mcast.mac.octet[2],
                            output_l2_entry[i].l2mcast.mac.octet[3], output_l2_entry[i].l2mcast.mac.octet[4], output_l2_entry[i].l2mcast.mac.octet[5], port_list, output_l2_entry[i].l2mcast.agg_vid);
            }
#endif
#if defined(CONFIG_SDK_RTL8390)
            if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s \n",
                            output_l2_entry[i].l2mcast.l2_idx,
                            output_l2_entry[i].l2mcast.rvid, output_l2_entry[i].l2mcast.mac.octet[0], output_l2_entry[i].l2mcast.mac.octet[1], output_l2_entry[i].l2mcast.mac.octet[2],
                            output_l2_entry[i].l2mcast.mac.octet[3], output_l2_entry[i].l2mcast.mac.octet[4], output_l2_entry[i].l2mcast.mac.octet[5], port_list);
            }
#endif
#if defined(CONFIG_SDK_RTL9300)
            if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %4d | %4d\n",
                            output_l2_entry[i].l2mcast.l2_idx,
                            output_l2_entry[i].l2mcast.rvid, output_l2_entry[i].l2mcast.mac.octet[0], output_l2_entry[i].l2mcast.mac.octet[1], output_l2_entry[i].l2mcast.mac.octet[2],
                            output_l2_entry[i].l2mcast.mac.octet[3], output_l2_entry[i].l2mcast.mac.octet[4], output_l2_entry[i].l2mcast.mac.octet[5], port_list, output_l2_entry[i].l2mcast.nextHop, output_l2_entry[i].l2mcast.mac_idx);
            }
#endif
#if defined(CONFIG_SDK_RTL9310)
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                diag_util_mprintf("%6d |%4d | %02X:%02X:%02X:%02X:%02X:%02X | %20s | %2d | 0x%X\n",
                        output_l2_entry[i].l2mcast.l2_idx,
                        output_l2_entry[i].l2mcast.rvid, output_l2_entry[i].l2mcast.mac.octet[0], output_l2_entry[i].l2mcast.mac.octet[1], output_l2_entry[i].l2mcast.mac.octet[2],
                        output_l2_entry[i].l2mcast.mac.octet[3], output_l2_entry[i].l2mcast.mac.octet[4], output_l2_entry[i].l2mcast.mac.octet[5], port_list, output_l2_entry[i].l2mcast.nextHop, output_l2_entry[i].l2mcast.groupId);
            }
#endif
        }
    }


#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    /* IP multicast */
    if (ret_cnt && contain_ipmcst)
    {
        diag_util_mprintf("\n");
        diag_util_mprintf(" Index | SIP             | DIP             | VID  | Port                 \n");
        diag_util_mprintf("-------+-----------------+-----------------+------+----------------------\n");

        for (i = 0; i < ret_cnt; i++)
        {
            if (output_l2_entry[i].entry_type == FLOW_TYPE_IP4_MULTI)
            {
                osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
                diag_util_lPortMask2str(port_list, &output_l2_entry[i].ipmcast.portmask);
                diag_util_ip2str_format(strBuf1, output_l2_entry[i].ipmcast.sip, 15);
                diag_util_ip2str_format(strBuf2, output_l2_entry[i].ipmcast.dip, 15);
                diag_util_mprintf("%6d | %s | %s | %4d | %20s\n", output_l2_entry[i].ipmcast.l2_idx, strBuf1, strBuf2, output_l2_entry[i].ipmcast.rvid, port_list);
            }
        }
    }


    /* IP6 multicast */
    if (ret_cnt && contain_ip6mcst)
    {
        diag_util_mprintf("\n");
        diag_util_mprintf(" Index | SIP                  | DIP                  | VID  | Port                 \n");
        diag_util_mprintf("-------+----------------------+----------------------+------+-----------------------\n");

        for (i = 0; i < ret_cnt; i++)
        {
            if (output_l2_entry[i].entry_type == FLOW_TYPE_IP6_MULTI)
            {
                osal_memset(port_list, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
                diag_util_lPortMask2str(port_list, &output_l2_entry[i].ip6mcast.portmask);
                DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strBuf1, output_l2_entry[i].ip6mcast.sip.octet), ret);
                DIAG_UTIL_ERR_CHK(diag_util_ipv62str(strBuf2, output_l2_entry[i].ip6mcast.dip.octet), ret);
                diag_util_mprintf("%6d | %20s | %20s | %4d | %20s \n", output_l2_entry[i].ip6mcast.l2_idx, strBuf1, strBuf2, output_l2_entry[i].ip6mcast.rvid, port_list);
            }
        }
    }
#endif

    if (ret_cnt)
    {
        diag_util_mprintf("\nTotal Number Of Conflicted Entries :%d\n", ret_cnt);
    }
    else
        diag_util_mprintf("No Confilcted Entry is found\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_GET_PORT_CTRL_PORT_PORT_ALL_SA_ACT_REF
/*
 * l2-table get port-ctrl port ( <PORT_LIST:port> | all ) sa-act ref
 */
cparser_result_t
cparser_cmd_l2_table_get_port_ctrl_port_port_all_sa_act_ref(cparser_context_t *context,
    char **port_ptr)

{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_port_t                      port = 0;
    int32 arg;

    diag_portlist_t                 portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port %d :\n", port);

        ret = rtk_l2_portCtrl_get(unit, port, RTK_L2_PCT_SA_ACT_REF, &arg);
        if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED))
        {
           DIAG_ERR_PRINT(ret);
           return CPARSER_NOT_OK;
        }
        else
        {
            diag_util_mprintf("\tSA-act ref             : ");
            if (arg == ENABLED)
            {
               diag_util_mprintf("VLAN-profile\n");
            }
            else if(arg == DISABLED)
            {
                diag_util_mprintf("Port\n");
            }
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
       }
    }
     return CPARSER_OK;
}

#endif

#ifdef CMD_L2_TABLE_SET_PORT_CTRL_PORT_PORT_ALL_SA_ACT_REF_PORT_VLAN_PROFILE
/*
 * l2-table set port-ctrl port ( <PORT_LIST:port> | all ) sa-act ref ( port | vlan-profile )
 */
cparser_result_t
cparser_cmd_l2_table_set_port_ctrl_port_port_all_sa_act_ref_port_vlan_profile(cparser_context_t *context,
    char **port_ptr)
{
    uint32                          unit = 0;
    int32                           ret = RT_ERR_FAILED;
    rtk_port_t                      port = 0;
    int32 arg;

    diag_portlist_t                 portlist;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);


    if ('p' == TOKEN_CHAR(7, 0))
    {
        arg = DISABLED;
    }
    else if ('v' == TOKEN_CHAR(7, 0))
    {
        arg = ENABLED;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portCtrl_set(unit, port, RTK_L2_PCT_SA_ACT_REF, arg), ret);
    }

    return CPARSER_OK;
}

#endif

#ifdef CMD_L2_TABLE_GET_HASH_INDEX_VID_MAC
/*
 * l2-table get hash-index <UINT:vid> <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_get_hash_index_vid_mac(cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr)

{
    uint32                          unit = 0, i, j;
    int32                           ret = RT_ERR_FAILED;
    rtk_l2_macHashIdx_t             macHashIdx;

    RT_PARAM_CHK((NULL == vid_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    macHashIdx.vid = *vid_ptr;
    osal_memcpy(macHashIdx.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    DIAG_UTIL_ERR_CHK(rtk_l2_hashIdx_get(unit, &macHashIdx), ret);

    diag_util_mprintf("MAC Hash Inde: \n");
    for (i = 0; i < macHashIdx.validBlk; i++)
    {
        for (j = 0; j < macHashIdx.validAlgo; j++)
            diag_util_mprintf("Block %d, Algo %d:  %d\n", i, j, macHashIdx.idx[i][j]);
    }
    return CPARSER_OK;
}

#endif

#ifdef CMD_L2_TABLE_GET_HASH_FULL_COUNTER
/*
 * l2-table get hash-full-counter
 */
cparser_result_t
cparser_cmd_l2_table_get_hash_full_counter(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  val;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_status_get(unit, RTK_L2_STS_HASHFULL_CNT, 0, &val), ret);

    diag_util_mprintf("MAC Hash full counter: %d\n", val);
    return CPARSER_OK;
}
#endif


#ifdef CMD_L2_TABLE_GET_MAC_COUNTER_ALL_SOFTWARE_CFG_MCAST_UCAST_SOFTWARE_CFG
/*
 * l2-table get mac-counter ( all | software_cfg | mcast | ucast-software_cfg )
*/
cparser_result_t cparser_cmd_l2_table_get_mac_counter_all_software_cfg_mcast_ucast_software_cfg(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  val;
    rtk_l2_entryType_t type;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('a' == TOKEN_CHAR(3, 0))
    {
        type = L2_ENTRY_ALL;
    }
    else if ('s' == TOKEN_CHAR(3, 0))
    {
        type = L2_ENTRY_ALL_SOFTWARE_CFG;
    }
    else if ('m' == TOKEN_CHAR(3, 0))
    {
        type = L2_ENTRY_MC;
    }
    else if ('u' == TOKEN_CHAR(3, 0))
    {
        type = L2_ENTRY_UC_SOFTWARE_CFG;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_entryCnt_get(unit, type, &val), ret);

    diag_util_mprintf("MAC counter: %d\n", val);
    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_MAC_BASE_ENTRY_MAC_MCAST_MAC
/*
 * l2-table del mac-base-entry mac-mcast <MACADDR:mac>
 */
    cparser_result_t cparser_cmd_l2_table_del_mac_base_entry_mac_mcast_mac(cparser_context_t *context,
    cparser_macaddr_t *mac_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_OUTPUT_INIT();

    if(!(mac_ptr->octet[0] & 0x1))
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_delByMac(unit, 1, (rtk_mac_t *)mac_ptr), ret);

    diag_util_mprintf("DELETE_OK!\n");
    return CPARSER_OK;
}
#endif

#ifdef CMD_L2_TABLE_DEL_MAC_BASE_ENTRY_MAC_UCAST_MAC_DYNAMIC_ONLY_INCLUDE_STATIC
/*
 * l2-table del mac-base-entry mac <MACADDR:mac> ( dynamic-only | include-static )
 */
    cparser_result_t cparser_cmd_l2_table_del_mac_base_entry_mac_ucast_mac_dynamic_only_include_static(cparser_context_t *context,
    cparser_macaddr_t *mac_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    uint32              include_static = 2;

    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_OUTPUT_INIT();

    if(mac_ptr->octet[0] & 0x1)
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('i' == TOKEN_CHAR(5, 0))
    {
        include_static = 1;
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        include_static = 0;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_delByMac(unit, include_static, (rtk_mac_t *)mac_ptr), ret);

    diag_util_mprintf("DELETE_OK!\n");
    return CPARSER_OK;
}
#endif

