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
 * Purpose : Definition those TRUNK command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) trunk configuration
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
#include <rtk/port.h>
#include <rtk/trunk.h>
#include <rtk/stack.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_port.h>
  #include <rtrpc/rtrpc_trunk.h>
  #include <rtrpc/rtrpc_stack.h>
#endif
#ifdef CMD_TRUNK_GET_MODE
/*
 * trunk get mode
 */
cparser_result_t
cparser_cmd_trunk_get_mode(
    cparser_context_t *context)
{
    uint32                      unit = 0;
    int32                        ret = RT_ERR_FAILED;
    rtk_trunk_mode_t      mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_mode_get(unit,  &mode), ret);

    diag_util_mprintf("Trunk Mode : %s\n", (TRUNK_MODE_STACKING == mode) ? "Stacking": "Stand Alone");

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_mode */
#endif

#ifdef CMD_TRUNK_SET_MODE_STAND_ALONE_STACKING
/*
 * trunk set mode ( stand-alone | stacking )
 */
cparser_result_t
cparser_cmd_trunk_set_mode_stand_alone_stacking(
    cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('c' == TOKEN_CHAR(3, 3))
    {
        DIAG_UTIL_ERR_CHK(rtk_trunk_mode_set(unit,  TRUNK_MODE_STACKING), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_trunk_mode_set(unit,  TRUNK_MODE_STANDALONE), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_mode_stand_alone_stacking */
#endif



#ifdef CMD_TRUNK_DUMP_TRUNK_GROUP_ALL
/*
 * trunk dump trunk-group all
 */
cparser_result_t cparser_cmd_trunk_dump_trunk_group_all(cparser_context_t *context)
{
    uint32                      unit = 0;
    uint32                      min_trk_gid = 0;
    uint32                      max_trk_gid = 0;
    uint32                      trunk_index = 0;
    int32                       ret = RT_ERR_FAILED;
    rtk_portmask_t              trunk_member_portmask;
    char                        memPortList[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_switch_devInfo_t        devInfo;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    char                        *pBuf;
    rtk_trunk_mode_t      mode;
    int32                       len, bufSz, i;
    rtk_trk_egrPort_t           trk_egr_ports;
    uint8                       isFirst = TRUE;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();


    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    /* show all trunk groups inforamtion */
    min_trk_gid = 0; /* PORT_MIN_TRUNK */
    max_trk_gid = devInfo.capacityInfo.max_num_of_trunk - 1; /* PORT_MAX_TRUNK */

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    DIAG_UTIL_ERR_CHK(rtk_trunk_mode_get(unit, &mode), ret);

    if(TRUNK_MODE_STACKING == mode)
        max_trk_gid = devInfo.capacityInfo.max_num_of_trunk_stacking_mode - 1;
    else
        max_trk_gid = devInfo.capacityInfo.max_num_of_trunk - 1;
#endif

    for (trunk_index = min_trk_gid; trunk_index <= max_trk_gid; trunk_index++)
    {
        diag_util_mprintf("\nTrunk %d:\n", trunk_index);

      #if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        DIAG_UTIL_ERR_CHK(rtk_trunk_localPort_get(unit, trunk_index, &trunk_member_portmask), ret);
        osal_memset(memPortList, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(memPortList, &trunk_member_portmask);
        diag_util_mprintf("Local member : %s\n", memPortList);

        DIAG_UTIL_ERR_CHK(rtk_trunk_egrPort_get(unit, trunk_index, &trk_egr_ports), ret);
        osal_memset(memPortList, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);

        pBuf = memPortList;
        bufSz = DIAG_UTIL_PORT_MASK_STRING_LEN;
        for(i = 0; i < trk_egr_ports.num_ports; i ++)
        {
            len = snprintf((char *)pBuf, bufSz, (TRUE == isFirst)? "%d:%d": ", %d:%d", trk_egr_ports.egr_port[i].devID, trk_egr_ports.egr_port[i].port);
            isFirst = FALSE;

            if ((len < 0) || (len >= bufSz))
            {
                diag_util_mprintf("Insufficient string buffer, information may be incomplete!\n\n");
                break;
            }

            pBuf += len;
            bufSz -= len;
        }/* end for */

        diag_util_mprintf("Egress member (%d ports): %s\n", trk_egr_ports.num_ports, memPortList);

      #else
        osal_memset(&trunk_member_portmask, 0, sizeof(rtk_portmask_t));
        DIAG_UTIL_ERR_CHK(rtk_trunk_port_get(unit, trunk_index, &trunk_member_portmask), ret);
        osal_memset(memPortList, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(memPortList, &trunk_member_portmask);
        diag_util_mprintf("Member : %s\n", memPortList);
      #endif

    } /* end of for (trunk_index = min_trk_gid; trunk_index <= max_trk_gid; trunk_index++) */



    return CPARSER_OK;
} /* end of cparser_cmd_trunk_dump_trunk_id */
#endif

#ifdef CMD_TRUNK_DUMP_TRUNK_GROUP_TRUNK_ID
/*
 * trunk dump trunk-group <UINT:trunk_id>
 */
cparser_result_t cparser_cmd_trunk_dump_trunk_group_trunk_id(cparser_context_t *context,
    uint32_t *trunk_id_ptr)
{
    uint32                      unit = 0;
    uint32                      min_trk_gid = 0;
    uint32                      max_trk_gid = 0;
    uint32                      trunk_index = 0;
    int32                       ret = RT_ERR_FAILED;
    rtk_portmask_t              trunk_member_portmask;
    char                        memPortList[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_switch_devInfo_t        devInfo;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    char                        *pBuf;
    int32                       len, bufSz, i;
    rtk_trk_egrPort_t           trk_egr_ports;
    uint8                       isFirst = TRUE;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    /* show specific trunk group inforamtion */
    min_trk_gid = *trunk_id_ptr;
    max_trk_gid = *trunk_id_ptr;

    for (trunk_index = min_trk_gid; trunk_index <= max_trk_gid; trunk_index++)
    {
        diag_util_mprintf("\nTrunk %d:\n", trunk_index);

      #if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        DIAG_UTIL_ERR_CHK(rtk_trunk_localPort_get(unit, trunk_index, &trunk_member_portmask), ret);
        osal_memset(memPortList, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(memPortList, &trunk_member_portmask);
        diag_util_mprintf("Local member : %s\n", memPortList);

        DIAG_UTIL_ERR_CHK(rtk_trunk_egrPort_get(unit, trunk_index, &trk_egr_ports), ret);
        osal_memset(memPortList, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);

        pBuf = memPortList;
        bufSz = DIAG_UTIL_PORT_MASK_STRING_LEN;
        for(i = 0; i < trk_egr_ports.num_ports; i ++)
        {
            len = snprintf((char *)pBuf, bufSz, (TRUE == isFirst)? "%d:%d": ", %d:%d", trk_egr_ports.egr_port[i].devID, trk_egr_ports.egr_port[i].port);
            isFirst = FALSE;

            if ((len < 0) || (len >= bufSz))
            {
                diag_util_mprintf("Insufficient string buffer, information may be incomplete!\n\n");
                break;
            }

            pBuf += len;
            bufSz -= len;
        }/* end for */

        diag_util_mprintf("Egress member (%d ports): %s\n", trk_egr_ports.num_ports, memPortList);

      #else

        osal_memset(&trunk_member_portmask, 0, sizeof(rtk_portmask_t));
        DIAG_UTIL_ERR_CHK(rtk_trunk_port_get(unit, trunk_index, &trunk_member_portmask), ret);
        osal_memset(memPortList, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
        diag_util_lPortMask2str(memPortList, &trunk_member_portmask);
        diag_util_mprintf("Member : %s\n", memPortList);
      #endif
    } /* end of for (trunk_index = min_trk_gid; trunk_index <= max_trk_gid; trunk_index++) */

    return CPARSER_OK;
} /* end of cparser_cmd_trunk_dump_trunk_id */
#endif

#ifdef CMD_TRUNK_GET_MEMBER_PORT_TRUNK_GROUP_TRUNK_ID
/*
 * trunk get member-port trunk-group <UINT:trunk_id>
 */
cparser_result_t cparser_cmd_trunk_get_member_port_trunk_group_trunk_id(cparser_context_t *context,
    uint32_t *trunk_id_ptr)
{
    uint32                      unit = 0;
    int32                       ret = RT_ERR_FAILED;
    char                        memPortList[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_portmask_t              trunk_member_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&trunk_member_portmask, 0, sizeof(rtk_portmask_t));
    DIAG_UTIL_ERR_CHK(rtk_trunk_port_get(unit, *trunk_id_ptr, &trunk_member_portmask), ret);
    osal_memset(memPortList, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
    diag_util_lPortMask2str(memPortList, &trunk_member_portmask);
    diag_util_mprintf("Member : %s\n", memPortList);

    return CPARSER_OK;
} /* end of cparser_cmd_trunk_get_member_port_trunk_id */
#endif

#ifdef CMD_TRUNK_SET_MEMBER_PORT_TRUNK_GROUP_TRUNK_ID_PORTS_NONE
/*
 * trunk set member-port trunk-group <UINT:trunk_id> ( <PORT_LIST:ports> | none )
 */
cparser_result_t cparser_cmd_trunk_set_member_port_trunk_group_trunk_id_ports_none(cparser_context_t *context,
    uint32_t *trunk_id_ptr,
    char **ports_ptr)
{
    uint32          unit = 0;
    uint32          trk_gid = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_portmask_t  trunk_member_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    trk_gid = *trunk_id_ptr;
    osal_memset(&trunk_member_portmask, 0, sizeof(rtk_portmask_t));

    if ('n' != TOKEN_CHAR(5,0))
        diag_util_str2LPortMask(*ports_ptr, &trunk_member_portmask);

    DIAG_UTIL_ERR_CHK(rtk_trunk_port_set(unit, trk_gid, &trunk_member_portmask), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_trunk_set_member_port_trunk_id_port_all */
#endif

#ifdef CMD_TRUNK_GET_LOCAL_PORT_TRUNK_GROUP_TRUNK_ID
/*
 * trunk get local-port trunk-group <UINT:trunk_id>
 */
cparser_result_t
cparser_cmd_trunk_get_local_port_trunk_group_trunk_id(
    cparser_context_t *context,
    uint32_t *trunk_id_ptr)
{
    uint32                      unit = 0;
    int32                       ret = RT_ERR_FAILED;
    char                        memPortList[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_portmask_t              trunk_member_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&trunk_member_portmask, 0, sizeof(rtk_portmask_t));

    DIAG_UTIL_ERR_CHK(rtk_trunk_localPort_get(unit, *trunk_id_ptr, &trunk_member_portmask), ret);
    osal_memset(memPortList, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);
    diag_util_lPortMask2str(memPortList, &trunk_member_portmask);
    diag_util_mprintf("Local member : %s\n", memPortList);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_local_port_trunk_group_trunk_id */
#endif

#ifdef CMD_TRUNK_SET_LOCAL_PORT_TRUNK_GROUP_TRUNK_ID_MEMBER_PORTS_NONE
/*
 * trunk set local-port trunk-group <UINT:trunk_id> member ( <PORT_LIST:ports> | none )
 */
cparser_result_t
cparser_cmd_trunk_set_local_port_trunk_group_trunk_id_member_ports_none(
    cparser_context_t *context,
    uint32_t *trunk_id_ptr,
    char **ports_ptr)
{
    uint32          unit = 0, myUnit;
    uint32          trk_gid = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t    port;
    rtk_dev_port_t unitPort;
    rtk_portmask_t  trunk_member_portmask, preMeberMask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    trk_gid = *trunk_id_ptr;
    osal_memset(&trunk_member_portmask, 0, sizeof(rtk_portmask_t));

    if ('n' != TOKEN_CHAR(6,0))
        diag_util_str2LPortMask(*ports_ptr, &trunk_member_portmask);

    DIAG_UTIL_ERR_CHK(rtk_trunk_localPort_get(unit, trk_gid, &preMeberMask), ret);

    DIAG_UTIL_ERR_CHK(rtk_stack_devId_get(unit, &myUnit), ret);
    unitPort.devID = myUnit;

    for (port = 0; port <= RTK_MAX_NUM_OF_PORTS; ++port)
    {
        if (RTK_PORTMASK_IS_PORT_SET(preMeberMask,port))
        {
            unitPort.port = port;
            DIAG_UTIL_ERR_CHK(rtk_trunk_srcPortMap_set(unit, unitPort, 0, 0), ret);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_localPort_set(unit, trk_gid, &trunk_member_portmask), ret);

    for (port = 0; port <= RTK_MAX_NUM_OF_PORTS; ++port)
    {
        if (RTK_PORTMASK_IS_PORT_SET(trunk_member_portmask,port))
        {
            unitPort.port = port;
            DIAG_UTIL_ERR_CHK(rtk_trunk_srcPortMap_set(unit, unitPort, 1, *trunk_id_ptr), ret);
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_local_port_trunk_group_trunk_id_member_ports_none */
#endif

#ifdef CMD_TRUNK_GET_EGRESS_PORT_TRUNK_GROUP_TRUNK_ID
/*
 * trunk get egress-port trunk-group <UINT:trunk_id>
 */
cparser_result_t
cparser_cmd_trunk_get_egress_port_trunk_group_trunk_id(
    cparser_context_t *context,
    uint32_t *trunk_id_ptr)
{
    uint32                      i, unit = 0;
    int32                       ret = RT_ERR_FAILED;
    uint8                       memPortList[DIAG_UTIL_PORT_MASK_STRING_LEN], *pBuf;
    int32                       len, bufSz;
    rtk_trk_egrPort_t           trk_egr_ports;
    uint8                       isFirst = TRUE;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&trk_egr_ports, 0, sizeof(trk_egr_ports));

    DIAG_UTIL_ERR_CHK(rtk_trunk_egrPort_get(unit, *trunk_id_ptr, &trk_egr_ports), ret);
    osal_memset(memPortList, 0, DIAG_UTIL_PORT_MASK_STRING_LEN);

    pBuf = memPortList;
    bufSz = DIAG_UTIL_PORT_MASK_STRING_LEN;
    for(i = 0; i < trk_egr_ports.num_ports; i ++)
    {
        len = snprintf((char *)pBuf, bufSz, (TRUE == isFirst)? "%d:%d": ", %d:%d", trk_egr_ports.egr_port[i].devID, trk_egr_ports.egr_port[i].port);
        isFirst = FALSE;

        if ((len < 0) || (len >= bufSz))
        {
            diag_util_mprintf("Insufficient string buffer, information may be incomplete!\n\n");
            break;
        }

        pBuf += len;
        bufSz -= len;
    }/* end for */

    diag_util_mprintf("Egress member (%d ports): %s\n", trk_egr_ports.num_ports, memPortList);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_egress_port_trunk_group_trunk_id */
#endif


#ifdef CMD_TRUNK_SET_EGRESS_PORT_TRUNK_GROUP_TRUNK_ID_PORT_DEV_PORTS_NONE
/*
 * trunk set egress-port trunk-group <UINT:trunk_id> port ( <DEV_PORT_LIST:devID_ports> | none )
 */
cparser_result_t
cparser_cmd_trunk_set_egress_port_trunk_group_trunk_id_port_devID_ports_none(
    cparser_context_t *context,
    uint32_t *trunk_id_ptr,
    char **dev_ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    uint32          unit = 0;
    uint32          trk_gid = 0;
    rtk_trk_egrPort_t trk_egr_ports;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    trk_gid = *trunk_id_ptr;
    osal_memset(&trk_egr_ports, 0, sizeof(trk_egr_ports));

    if ('n' != TOKEN_CHAR(6,0))
    {
        diag_util_str2devPorts(*dev_ports_ptr, &trk_egr_ports);
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_egrPort_set(unit, trk_gid, &trk_egr_ports), ret);
    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_egress_port_trunk_group_trunk_id_num_num_ports_port_dev_ports_none */
#endif



#ifdef CMD_TRUNK_GET_DISTRIBUTE_ALGORITHM_BIND_TRUNK_GROUP_TRUNK_ID
/*
 * trunk get distribute-algorithm bind trunk-group <UINT:trunk_id>
 */
cparser_result_t cparser_cmd_trunk_get_distribute_algorithm_bind_trunk_group_trunk_id(cparser_context_t *context,
    uint32_t *trunk_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  algo_idx ;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmBind_get(unit, *trunk_id_ptr, &algo_idx), ret);
    diag_util_mprintf("Trunk %d binds to distribution algorithm %d\n", *trunk_id_ptr, algo_idx);


    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_GET_DISTRIBUTE_ALGORITHM_PARAMETER_ALGORITHM_ID_ALGO_ID
/*
 * trunk get distribute-algorithm parameter algorithm-id <UINT:algo_id>
 */
cparser_result_t cparser_cmd_trunk_get_distribute_algorithm_parameter_algorithm_id_algo_id(cparser_context_t *context,
    uint32_t *algo_id_ptr)
{
    uint32  unit = 0;
    uint32  algo_bitmask = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("\nIndex %d ", *algo_id_ptr);
    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmParam_get(unit, *algo_id_ptr, &algo_bitmask), ret);
    diag_util_mprintf("distribution algorithm: ");
    if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_SPA_BIT)
    {
        diag_util_mprintf("src-port|");
    }
    if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_SMAC_BIT)
    {
        diag_util_mprintf("src-mac|");
    }
    if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_DMAC_BIT)
    {
        diag_util_mprintf("dst-mac|");
    }
    if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_SIP_BIT)
    {
        diag_util_mprintf("src-ip|");
    }
    if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_DIP_BIT)
    {
        diag_util_mprintf("dst-ip|");
    }
    if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_SRC_L4PORT_BIT)
    {
        diag_util_mprintf("src-l4-port|");
    }
    if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_DST_L4PORT_BIT)
    {
        diag_util_mprintf("dst-l4-port|");
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_GET_DISTRIBUTE_ALGORITHM_BIND_TRUNK_GROUP_TRUNK_ID_TYPE_L2_IPV4_IPV6
/*
 * trunk get distribute-algorithm bind trunk-group <UINT:trunk_id> type ( l2 | ipv4 | ipv6 )
 */
cparser_result_t
cparser_cmd_trunk_get_distribute_algorithm_bind_trunk_group_trunk_id_type_l2_ipv4_ipv6(
    cparser_context_t *context,
    uint32_t *trunk_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  algo_idx ;
    rtk_trunk_bindType_t type;
    char typeStr[32];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('l' == TOKEN_CHAR(7, 0))
    {
        type = BIND_TYPE_L2;
        osal_strcpy(typeStr, "L2");
    }
    else if ('4' == TOKEN_CHAR(7, 3))
    {
        type = BIND_TYPE_IPV4;
        osal_strcpy(typeStr, "IPV4");
    }
    else if ('6' == TOKEN_CHAR(7, 3))
    {
        type = BIND_TYPE_IPV6;
        osal_strcpy(typeStr, "IPV6");
    }
    else
    {
        diag_util_mprintf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmTypeBind_get(unit, *trunk_id_ptr, type, &algo_idx), ret);
    diag_util_mprintf("Trunk %d Type %s binds to distribution algorithm %d\n", *trunk_id_ptr, typeStr, algo_idx);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_distribute_algorithm_bind_trunk_group_trunk_id_type_l2_ipv4_ipv6 */
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_BIND_TRUNK_GROUP_TRUNK_ID_TYPE_L2_IPV4_IPV6_ALGO_ID_ALGO_ID
/*
 * trunk set distribute-algorithm bind trunk-group <UINT:trunk_id> type ( l2 | ipv4 | ipv6 ) algo-id <UINT:algo_id>
 */
cparser_result_t
cparser_cmd_trunk_set_distribute_algorithm_bind_trunk_group_trunk_id_type_l2_ipv4_ipv6_algo_id_algo_id(
    cparser_context_t *context,
    uint32_t *trunk_id_ptr,
    uint32_t *algo_id_ptr)
{
    int32           ret = RT_ERR_FAILED;
    uint32          unit = 0;
    rtk_trunk_bindType_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('l' == TOKEN_CHAR(7, 0))
    {
        type = BIND_TYPE_L2;
    }
    else if ('4' == TOKEN_CHAR(7, 3))
    {
        type = BIND_TYPE_IPV4;
    }
    else if ('6' == TOKEN_CHAR(7, 3))
    {
        type = BIND_TYPE_IPV6;
    }
    else
    {
        diag_util_mprintf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmTypeBind_set(unit, *trunk_id_ptr, type, *algo_id_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_distribute_algorithm_bind_trunk_group_trunk_id_type_l2_ipv4_ipv6_algo_id_algo_id */
#endif


#ifdef CMD_TRUNK_GET_DISTRIBUTE_ALGORITHM_SHIFT_ALGORITHM_ID_ALGO_ID
/*
 * trunk get distribute-algorithm shift algorithm-id <UINT:algo_id>
 */
cparser_result_t cparser_cmd_trunk_get_distribute_algorithm_shift_algorithm_id_algo_id(cparser_context_t *context,
    uint32_t *algo_id_ptr)
{
    uint32  unit = 0;
    rtk_trunk_distAlgoShift_t shift;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_get(unit, *algo_id_ptr, &shift), ret);

    diag_util_mprintf("Shift of algorithm %d parameters:\n", *algo_id_ptr);
    diag_util_mprintf("SPA: %d bits\n", shift.spa_shift);
    diag_util_mprintf("SMAC: %d bits\n", shift.smac_shift);
    diag_util_mprintf("DMAC: %d bits\n", shift.dmac_shift);
    diag_util_mprintf("SIP: %d bits\n", shift.sip_shift);
    diag_util_mprintf("DIP: %d bits\n", shift.dip_shift);
    diag_util_mprintf("SPORT: %d bits\n", shift.sport_shift);
    diag_util_mprintf("DPORT: %d bits\n", shift.dport_shift);

    return CPARSER_OK;
}
#endif


#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_BIND_TRUNK_GROUP_TRUNK_ID_ALGO_ID_ALGO_ID
/*
 * trunk set distribute-algorithm bind trunk-group <UINT:trunk_id> algo-id <UINT:algo_id>
 */
cparser_result_t cparser_cmd_trunk_set_distribute_algorithm_bind_trunk_group_trunk_id_algo_id_algo_id(cparser_context_t *context,
    uint32_t *trunk_id_ptr,
    uint32_t *algo_id_ptr)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmBind_set(unit, *trunk_id_ptr, *algo_id_ptr), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_PARAMETER_ALGORITHM_ID_ALGO_ID_DST_IP_DST_L4_PORT_DST_MAC_SRC_IP_SRC_L4_PORT_SRC_MAC_SRC_PORT
/*
 * trunk set distribute-algorithm parameter algorithm-id <UINT:algo_id> { dst-ip } { dst-l4-port } { dst-mac } { src-ip } { src-l4-port } { src-mac } { src-port }
 */
cparser_result_t cparser_cmd_trunk_set_distribute_algorithm_parameter_algorithm_id_algo_id_dst_ip_dst_l4_port_dst_mac_src_ip_src_l4_port_src_mac_src_port(cparser_context_t *context,
    uint32_t *algo_id_ptr)
{
    uint32  unit = 0;
    uint32  algo_bitmask = 0;
    int32   option_num = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (TOKEN_NUM < 7)
    {
        algo_bitmask = 0;
    }
    else
    {
        for (option_num = 6; option_num < TOKEN_NUM; option_num++)
        {
            if ('s' == TOKEN_CHAR(option_num, 0))
            {
                if ('p' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_SPA_BIT;
                }
                else if ('m' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_SMAC_BIT;
                }
                else if ('i' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_SIP_BIT;
                }
                else if ('l' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_SRC_L4PORT_BIT;
                }
                else
                {
                    diag_util_mprintf("User config: Error!\n");
                    return CPARSER_NOT_OK;
                }
            }
            else if ('d' == TOKEN_CHAR(option_num, 0))
            {
                if ('m' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_DMAC_BIT;
                }
                else if ('i' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_DIP_BIT;
                }
                else if ('l' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_DST_L4PORT_BIT;
                }
                else
                {
                    diag_util_mprintf("User config: Error!\n");
                    return CPARSER_NOT_OK;
                }
            }
            else
            {
                diag_util_mprintf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
        } /* end of for (option_num = 6; option_num < TOKEN_NUM; option_num++) */
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmParam_set(unit, *algo_id_ptr, algo_bitmask), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_GET_DISTRIBUTE_ALGORITHM_PARAMETER_TYPE_L2_L3_ALGORITHM_ID_ALGO_ID
/*
 * trunk get distribute-algorithm parameter type ( l2 | l3 ) algorithm-id <UINT:algo_id>
 */
cparser_result_t
cparser_cmd_trunk_get_distribute_algorithm_parameter_type_l2_l3_algorithm_id_algo_id(
    cparser_context_t *context,
    uint32_t *algo_id_ptr)
{
    uint32  unit = 0;
    uint32  algo_bitmask = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_trunk_hashParamType_t type;
    char typeStr[16], paraStr[128];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('2' == TOKEN_CHAR(5, 1))
    {
        type = PARAM_TYPE_L2;
        osal_strcpy(typeStr, "L2");
    }
    else if ('3' == TOKEN_CHAR(5, 1))
    {
        type = PARAM_TYPE_L3;
        osal_strcpy(typeStr, "L3");
    }
    else
    {
        diag_util_mprintf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(paraStr, 0, sizeof(paraStr));

    diag_util_mprintf("\nType: %s Index %d ", typeStr, *algo_id_ptr);
    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmTypeParam_get(unit, type, *algo_id_ptr, &algo_bitmask), ret);
    diag_util_mprintf("distribution algorithm: ");
    switch(type)
    {
        case PARAM_TYPE_L2:

            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L2_SPA_BIT)
            {
                osal_strcat(paraStr, "| src-port");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L2_SMAC_BIT)
            {
                osal_strcat(paraStr, "| src-mac");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L2_DMAC_BIT)
            {
                osal_strcat(paraStr, "| dst-mac");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L2_VLAN_BIT)
            {
                osal_strcat(paraStr, "| vlan");
            }
            osal_strcat(paraStr, "\n");
            break;
        case PARAM_TYPE_L3:
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L3_SPA_BIT)
            {
                osal_strcat(paraStr, "| src-port");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L3_SMAC_BIT)
            {
                osal_strcat(paraStr, "| src-mac");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L3_DMAC_BIT)
            {
                osal_strcat(paraStr, "| dst-mac");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L3_VLAN_BIT)
            {
                osal_strcat(paraStr, "| vlan");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L3_SIP_BIT)
            {
                osal_strcat(paraStr, "| src-ip");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L3_DIP_BIT)
            {
                osal_strcat(paraStr, "| dst-ip");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L3_SRC_L4PORT_BIT)
            {
                osal_strcat(paraStr, "| src-l4-port");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L3_DST_L4PORT_BIT)
            {
                osal_strcat(paraStr, "| dst-l4-port");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L3_PROTO_BIT)
            {
                osal_strcat(paraStr, "| proto");
            }
            if (algo_bitmask & TRUNK_DISTRIBUTION_ALGO_L3_FLOW_LABEL_BIT)
            {
                osal_strcat(paraStr, "| flow-label");
            }
            osal_strcat(paraStr, "\n");
            break;
        default:
            break;
    }

    diag_util_mprintf(&paraStr[2]);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_distribute_algorithm_parameter_type_l2_l3_algorithm_id_algo_id */
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_PARAMETER_TYPE_L2_ALGORITHM_ID_ALGO_ID_DST_MAC_SRC_MAC_SRC_PORT_VLAN
/*
 * trunk set distribute-algorithm parameter type l2 algorithm-id <UINT:algo_id> { dst-mac } { src-mac } { src-port } { vlan }
 */
cparser_result_t
cparser_cmd_trunk_set_distribute_algorithm_parameter_type_l2_algorithm_id_algo_id_dst_mac_src_mac_src_port_vlan(
    cparser_context_t *context,
    uint32_t *algo_id_ptr)
{
    uint32  unit = 0;
    uint32  algo_bitmask = 0;
    int32   option_num = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (TOKEN_NUM < 9)
    {
        algo_bitmask = 0;
    }
    else
    {
        for (option_num = 8; option_num < TOKEN_NUM; option_num++)
        {
            if ('s' == TOKEN_CHAR(option_num, 0))
            {
                if ('p' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L2_SPA_BIT;
                }
                else if ('m' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L2_SMAC_BIT;
                }
                else
                {
                    diag_util_mprintf("User config: Error!\n");
                    return CPARSER_NOT_OK;
                }
            }
            else if ('d' == TOKEN_CHAR(option_num, 0))
            {
                algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L2_DMAC_BIT;
            }
            else if ('v' == TOKEN_CHAR(option_num, 0))
            {
                algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L2_VLAN_BIT;
            }
            else
            {
                diag_util_mprintf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmTypeParam_set(unit, PARAM_TYPE_L2, *algo_id_ptr, algo_bitmask), ret);
    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_distribute_algorithm_parameter_type_l2_algorithm_id_algo_id_dst_mac_src_mac_src_port_vlan */
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_PARAMETER_TYPE_L3_ALGORITHM_ID_ALGO_ID_DST_IP_DST_L4_PORT_DST_MAC_FLOW_LABEL_PROTOCOL_SRC_IP_SRC_L4_PORT_SRC_MAC_SRC_PORT_VLAN
/*
 * trunk set distribute-algorithm parameter type l3 algorithm-id <UINT:algo_id> { dst-ip } { dst-l4-port } { dst-mac } { flow-label } { protocol } { src-ip } { src-l4-port } { src-mac } { src-port } { vlan }
 */
cparser_result_t
cparser_cmd_trunk_set_distribute_algorithm_parameter_type_l3_algorithm_id_algo_id_dst_ip_dst_l4_port_dst_mac_flow_label_protocol_src_ip_src_l4_port_src_mac_src_port_vlan(
    cparser_context_t *context,
    uint32_t *algo_id_ptr)
{
    uint32  unit = 0;
    uint32  algo_bitmask = 0;
    int32   option_num = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (TOKEN_NUM < 9)
    {
        algo_bitmask = 0;
    }
    else
    {
        for (option_num = 8; option_num < TOKEN_NUM; option_num++)
        {
             if ('s' == TOKEN_CHAR(option_num, 0))
            {
                if ('p' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L3_SPA_BIT;
                }
                else if ('m' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L3_SMAC_BIT;
                }
                else if ('i' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L3_SIP_BIT;
                }
                else if ('l' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L3_SRC_L4PORT_BIT;
                }
                else
                {
                    diag_util_mprintf("User config: Error!\n");
                    return CPARSER_NOT_OK;
                }
            }
            else if ('d' == TOKEN_CHAR(option_num, 0))
            {
                if ('m' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L3_DMAC_BIT;
                }
                else if ('i' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L3_DIP_BIT;
                }
                else if ('l' == TOKEN_CHAR(option_num, 4))
                {
                    algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L3_DST_L4PORT_BIT;
                }
                else
                {
                    diag_util_mprintf("User config: Error!\n");
                    return CPARSER_NOT_OK;
                }
            }
            else if ('v' == TOKEN_CHAR(option_num, 0))
            {
                algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L3_VLAN_BIT;
            }
            else if ('f' == TOKEN_CHAR(option_num, 0))
            {
                algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L3_FLOW_LABEL_BIT;
            }
            else if ('p' == TOKEN_CHAR(option_num, 0))
            {
                algo_bitmask |= TRUNK_DISTRIBUTION_ALGO_L3_PROTO_BIT;
            }
            else
            {
                diag_util_mprintf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmTypeParam_set(unit, PARAM_TYPE_L3, *algo_id_ptr, algo_bitmask), ret);
    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_distribute_algorithm_parameter_type_l3_algorithm_id_algo_id_dst_ip_dst_l4_port_dst_mac_flow_label_protocol_src_ip_src_l4_port_src_mac_src_port_vlan */
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_SHIFT_ALGORITHM_ID_ALGO_ID_SRC_PORT_SHIFT
/*
 * trunk set distribute-algorithm shift algorithm-id <UINT:algo_id> src-port <UINT:shift>
 */
cparser_result_t cparser_cmd_trunk_set_distribute_algorithm_shift_algorithm_id_algo_id_src_port_shift(cparser_context_t *context,
    uint32_t *algo_id_ptr,
    uint32_t *shift_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_trunk_distAlgoShift_t shift;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_get(unit, *algo_id_ptr, &shift), ret);

    shift.spa_shift = *shift_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_set(unit, *algo_id_ptr, &shift), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_SHIFT_ALGORITHM_ID_ALGO_ID_SRC_MAC_SHIFT
/*
 * trunk set distribute-algorithm shift algorithm-id <UINT:algo_id> src-mac <UINT:shift>
 */
cparser_result_t cparser_cmd_trunk_set_distribute_algorithm_shift_algorithm_id_algo_id_src_mac_shift(cparser_context_t *context,
    uint32_t *algo_id_ptr,
    uint32_t *shift_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_trunk_distAlgoShift_t shift;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_get(unit, *algo_id_ptr, &shift), ret);

    shift.smac_shift = *shift_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_set(unit, *algo_id_ptr, &shift), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_SHIFT_ALGORITHM_ID_ALGO_ID_DST_MAC_SHIFT
/*
 * trunk set distribute-algorithm shift algorithm-id <UINT:algo_id> dst-mac <UINT:shift>
 */
cparser_result_t cparser_cmd_trunk_set_distribute_algorithm_shift_algorithm_id_algo_id_dst_mac_shift(cparser_context_t *context,
    uint32_t *algo_id_ptr,
    uint32_t *shift_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_trunk_distAlgoShift_t shift;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_get(unit, *algo_id_ptr, &shift), ret);

    shift.dmac_shift = *shift_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_set(unit, *algo_id_ptr, &shift), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_SHIFT_ALGORITHM_ID_ALGO_ID_SRC_IP_SHIFT
/*
 * trunk set distribute-algorithm shift algorithm-id <UINT:algo_id> src-ip <UINT:shift>
 */
cparser_result_t cparser_cmd_trunk_set_distribute_algorithm_shift_algorithm_id_algo_id_src_ip_shift(cparser_context_t *context,
    uint32_t *algo_id_ptr,
    uint32_t *shift_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_trunk_distAlgoShift_t shift;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_get(unit, *algo_id_ptr, &shift), ret);

    shift.sip_shift = *shift_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_set(unit, *algo_id_ptr, &shift), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_SHIFT_ALGORITHM_ID_ALGO_ID_DST_IP_SHIFT
/*
 * trunk set distribute-algorithm shift algorithm-id <UINT:algo_id> dst-ip <UINT:shift>
 */
cparser_result_t cparser_cmd_trunk_set_distribute_algorithm_shift_algorithm_id_algo_id_dst_ip_shift(cparser_context_t *context,
    uint32_t *algo_id_ptr,
    uint32_t *shift_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_trunk_distAlgoShift_t shift;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_get(unit, *algo_id_ptr, &shift), ret);

    shift.dip_shift = *shift_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_set(unit, *algo_id_ptr, &shift), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_SHIFT_ALGORITHM_ID_ALGO_ID_SRC_L4_PORT_SHIFT
/*
 * trunk set distribute-algorithm shift algorithm-id <UINT:algo_id> src-l4-port <UINT:shift>
 */
cparser_result_t cparser_cmd_trunk_set_distribute_algorithm_shift_algorithm_id_algo_id_src_l4_port_shift(cparser_context_t *context,
    uint32_t *algo_id_ptr,
    uint32_t *shift_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_trunk_distAlgoShift_t shift;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_get(unit, *algo_id_ptr, &shift), ret);

    shift.sport_shift = *shift_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_set(unit, *algo_id_ptr, &shift), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_SHIFT_ALGORITHM_ID_ALGO_ID_DST_L4_PORT_SHIFT
/*
 * trunk set distribute-algorithm shift algorithm-id <UINT:algo_id> dst-l4-port <UINT:shift>
 */
cparser_result_t cparser_cmd_trunk_set_distribute_algorithm_shift_algorithm_id_algo_id_dst_l4_port_shift(cparser_context_t *context,
    uint32_t *algo_id_ptr,
    uint32_t *shift_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_trunk_distAlgoShift_t shift;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_get(unit, *algo_id_ptr, &shift), ret);

    shift.dport_shift = *shift_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShift_set(unit, *algo_id_ptr, &shift), ret);
    return CPARSER_OK;
}
#endif


#ifdef CMD_TRUNK_GET_DISTRIBUTE_ALGORITHM_SHIFT
/*
 * trunk get distribute-algorithm shift
 */
cparser_result_t
cparser_cmd_trunk_get_distribute_algorithm_shift(
    cparser_context_t *context)
{
    uint32  unit = 0;
    rtk_trunk_distAlgoShift_t shift;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShiftGbl_get(unit, &shift), ret);

    diag_util_mprintf("Shift of algorithm parameters:\n");
    diag_util_mprintf("SPA: %d bits\n", shift.spa_shift);
    diag_util_mprintf("SMAC: %d bits\n", shift.smac_shift);
    diag_util_mprintf("DMAC: %d bits\n", shift.dmac_shift);
    diag_util_mprintf("VLAN: %d bits\n", shift.vlan_shift);
    diag_util_mprintf("SIP: %d bits\n", shift.sip_shift);
    diag_util_mprintf("DIP: %d bits\n", shift.dip_shift);
    diag_util_mprintf("SPORT: %d bits\n", shift.sport_shift);
    diag_util_mprintf("DPORT: %d bits\n", shift.dport_shift);
    diag_util_mprintf("PROTOCOL: %d bits\n", shift.proto_shift);
    diag_util_mprintf("FLOW LABEL: %d bits\n", shift.flowLabel_shift);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_distribute_algorithm_shift */
#endif


#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_SHIFT_SRC_PORT_SRC_MAC_DST_MAC_VLAN_SRC_IP_DST_IP_SRC_L4_PORT_DST_L4_PORT_PROTO_FLOW_LABEL_SHIFT
/*
 * trunk set distribute-algorithm shift ( src-port | src-mac | dst-mac | vlan | src-ip | dst-ip | src-l4-port | dst-l4-port | proto | flow-label ) <UINT:shift>
 */
cparser_result_t
cparser_cmd_trunk_set_distribute_algorithm_shift_src_port_src_mac_dst_mac_vlan_src_ip_dst_ip_src_l4_port_dst_l4_port_proto_flow_label_shift(
    cparser_context_t *context,
    uint32_t *shift_ptr)
{
    uint32  unit = 0;
    rtk_trunk_distAlgoShift_t shift;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShiftGbl_get(unit, &shift), ret);

    if ('s' == TOKEN_CHAR(4, 0))
    {
        if ('p' == TOKEN_CHAR(4, 4))
        {
            shift.spa_shift = *shift_ptr;
        }
        else if ('m' == TOKEN_CHAR(4, 4))
        {
            shift.smac_shift = *shift_ptr;
        }
        else if ('i' == TOKEN_CHAR(4, 4))
        {
            shift.sip_shift = *shift_ptr;
        }
        else if ('l' == TOKEN_CHAR(4, 4))
        {
            shift.sport_shift = *shift_ptr;
        }
        else
        {
            diag_util_mprintf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('m' == TOKEN_CHAR(4, 4))
        {
            shift.dmac_shift = *shift_ptr;
        }
        else if ('i' == TOKEN_CHAR(4, 4))
        {
            shift.dip_shift = *shift_ptr;
        }
        else if ('l' == TOKEN_CHAR(4, 4))
        {
            shift.dport_shift = *shift_ptr;
        }
        else
        {
            diag_util_mprintf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    else if ('v' == TOKEN_CHAR(4, 0))
    {
        shift.vlan_shift = *shift_ptr;
    }
    else if ('p' == TOKEN_CHAR(4, 0))
    {
        shift.proto_shift = *shift_ptr;
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        shift.flowLabel_shift = *shift_ptr;
    }
    else
    {
        diag_util_mprintf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithmShiftGbl_set(unit, &shift), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_distribute_algorithm_shift_src_port_src_mac_dst_mac_vlan_src_ip_dst_ip_src_l4_port_dst_l4_port_proto_flow_label_shift */
#endif

#ifdef CMD_TRUNK_GET_TRAFFIC_SEPARATION_TRUNK_GROUP_TRUNK_ID_KNOWN_MCAST_STATE
/*
 * trunk get traffic-separation trunk-group <UINT:trunk_id> known-mcast state
 */
cparser_result_t cparser_cmd_trunk_get_traffic_separation_trunk_group_trunk_id_known_mcast_state(cparser_context_t *context,
    uint32_t *trunk_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
  #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    rtk_trunk_separateType_t separate;
  #endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

  #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparate_get(unit, *trunk_id_ptr, &separate), ret);

        if (separate == SEPARATE_KNOWN_MULTI || separate == SEPARATE_KNOWN_MULTI_AND_FLOOD)
            enable = ENABLED;
        else
            enable = DISABLED;
    }
  #endif

  #if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparateEnable_get(unit, *trunk_id_ptr, SEPARATE_KNOWN_MULTI, &enable), ret);
    }
  #endif

    diag_util_mprintf("Traffic separate of trunk %d:\n", *trunk_id_ptr);
    if(enable == DISABLED)
    {
        diag_util_mprintf("Separate Known Multicast: Disable\n");
    }
    else
    {
        diag_util_mprintf("Separate Known Multicast: Enable\n");
    }

    return CPARSER_OK;

}
#endif



#ifdef CMD_TRUNK_SET_TRAFFIC_SEPARATION_TRUNK_GROUP_TRUNK_ID_KNOWN_MCAST_STATE_DISABLE_ENABLE
/*
 * trunk set traffic-separation trunk-group <UINT:trunk_id> known-mcast state ( disable | enable )
 */
cparser_result_t cparser_cmd_trunk_set_traffic_separation_trunk_group_trunk_id_known_mcast_state_disable_enable(cparser_context_t *context,
    uint32_t *trunk_id_ptr)
{
    uint32  unit = 0;
    int32    ret = RT_ERR_FAILED;
    rtk_enable_t enable;
  #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    rtk_trunk_separateType_t separate;
  #endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(7, 0))
        enable = DISABLED;
    else
        enable = ENABLED;

  #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparate_get(unit, *trunk_id_ptr, &separate), ret);

        switch(separate)
        {
            case SEPARATE_NONE:
                if(ENABLED == enable)
                    separate = SEPARATE_KNOWN_MULTI;
                break;
            case SEPARATE_KNOWN_MULTI:
                if(DISABLED == enable)
                    separate = SEPARATE_NONE;
                break;
            case SEPARATE_FLOOD:
                if(ENABLED == enable)
                    separate = SEPARATE_KNOWN_MULTI_AND_FLOOD;
                break;
            case SEPARATE_KNOWN_MULTI_AND_FLOOD:
                if(DISABLED == enable)
                    separate = SEPARATE_FLOOD;
                break;
            default:
                break;
        }

        DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparate_set(unit, *trunk_id_ptr, separate), ret);
    }
  #endif

  #if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparateEnable_set(unit, *trunk_id_ptr, SEPARATE_KNOWN_MULTI, enable), ret);
    }
  #endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_GET_TRAFFIC_SEPARATION_TRUNK_GROUP_TRUNK_ID_FLOODING_STATE
/*
 * trunk get traffic-separation trunk-group <UINT:trunk_id> flooding state
 */
cparser_result_t cparser_cmd_trunk_get_traffic_separation_trunk_group_trunk_id_flooding_state(cparser_context_t *context,
    uint32_t *trunk_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
  #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    rtk_trunk_separateType_t separate;
  #endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

  #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparate_get(unit, *trunk_id_ptr, &separate), ret);

        if (separate == SEPARATE_FLOOD || separate == SEPARATE_KNOWN_MULTI_AND_FLOOD)
            enable = ENABLED;
        else
            enable = DISABLED;
    }
  #endif

  #if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparateEnable_get(unit, *trunk_id_ptr, SEPARATE_FLOOD, &enable), ret);
    }
  #endif

    diag_util_mprintf("Traffic separate of trunk %d:\n", *trunk_id_ptr);
    if(enable == DISABLED)
    {
        diag_util_mprintf("Separate Flooding Traffic: Disable\n");
    }
    else
    {
        diag_util_mprintf("Separate Flooding Traffic: Enable\n");
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_SET_TRAFFIC_SEPARATION_TRUNK_GROUP_TRUNK_ID_FLOODING_STATE_DISABLE_ENABLE
/*
 * trunk set traffic-separation trunk-group <UINT:trunk_id> flooding state ( disable | enable )
 */
cparser_result_t cparser_cmd_trunk_set_traffic_separation_trunk_group_trunk_id_flooding_state_disable_enable(cparser_context_t *context,
    uint32_t *trunk_id_ptr)
{
    uint32  unit = 0;
    int32    ret = RT_ERR_FAILED;
    rtk_enable_t enable;
  #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    rtk_trunk_separateType_t separate;
  #endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(7, 0))
        enable = DISABLED;
    else
        enable = ENABLED;

    #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparate_get(unit, *trunk_id_ptr, &separate), ret);

        switch(separate)
        {
            case SEPARATE_NONE:
                if(ENABLED == enable)
                    separate = SEPARATE_FLOOD;
                break;
            case SEPARATE_KNOWN_MULTI:
                if(ENABLED == enable)
                    separate = SEPARATE_KNOWN_MULTI_AND_FLOOD;
                break;
            case SEPARATE_FLOOD:
                if(DISABLED == enable)
                    separate = SEPARATE_NONE;
                break;
            case SEPARATE_KNOWN_MULTI_AND_FLOOD:
                if(DISABLED == enable)
                    separate = SEPARATE_KNOWN_MULTI;
                break;
            default:
                break;
        }

        DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparate_set(unit, *trunk_id_ptr, separate), ret);
    }
  #endif

  #if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparateEnable_set(unit, *trunk_id_ptr, SEPARATE_FLOOD, enable), ret);
    }
  #endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_TRUNK_GET_TRAFFIC_SEPARATION_DIVISION_STATE
/*
 * trunk get traffic-separation division state
 */
cparser_result_t
cparser_cmd_trunk_get_traffic_separation_division_state(
    cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();


    DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparateDivision_get(unit, &enable), ret);

    diag_util_mprintf("Divide knwon multicast traffic from other separated traffic: ");

    if(ENABLED == enable)
    {
        diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
    }
    else
    {
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_traffic_separation_division_state */
#endif

#ifdef CMD_TRUNK_SET_TRAFFIC_SEPARATION_DIVISION_STATE_DISABLE_ENABLE
/*
 * trunk set traffic-separation division state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trunk_set_traffic_separation_division_state_disable_enable(
    cparser_context_t *context)
{
    uint32  unit = 0;
    int32    ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(5, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparateDivision_set(unit, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_traffic_separation_division_state_disable_enable */
#endif

#ifdef CMD_TRUNK_GET_TUNNEL_HASH_SRC
/*
 * trunk get tunnel-hash-src
 */
cparser_result_t
cparser_cmd_trunk_get_tunnel_hash_src(
    cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_trunk_tunnelHashSrc_t tunnelHashSrc;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_tunnelHashSrc_get(unit, &tunnelHashSrc), ret);

    diag_util_mprintf("Source of tunnel packets hash: ");

    if(TUNNEL_HASH_SRC_OUTER_HEADER== tunnelHashSrc)
    {
        diag_util_mprintf("from outer layer of header\n");
    }
    else
    {
        diag_util_mprintf("from inner layer of header\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_tunnel_hash_src */
#endif

#ifdef CMD_TRUNK_SET_TUNNEL_HASH_SRC_INNER_OUTER
/*
 * trunk set tunnel-hash-src ( inner | outer )
 */
cparser_result_t
cparser_cmd_trunk_set_tunnel_hash_src_inner_outer(
    cparser_context_t *context)
{
    uint32  unit = 0;
    int32    ret = RT_ERR_FAILED;
    rtk_trunk_tunnelHashSrc_t tunnelHashSrc;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(3, 0))
        tunnelHashSrc = TUNNEL_HASH_SRC_INNER_HEADER;
    else
        tunnelHashSrc = TUNNEL_HASH_SRC_OUTER_HEADER;

    DIAG_UTIL_ERR_CHK(rtk_trunk_tunnelHashSrc_set(unit, tunnelHashSrc), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_tunnel_hash_src_inner_outer */
#endif

#ifdef CMD_TRUNK_GET_MEMBER_PORT_STACK_TRUNK_GROUP_STACK_TRUNK_ID
/*
 * trunk get member-port stack-trunk-group <UINT:stack_trunk_id>
 */
cparser_result_t
cparser_cmd_trunk_get_member_port_stack_trunk_group_stack_trunk_id(
    cparser_context_t *context,
    uint32_t *stack_trunk_id_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_portmask_t stk_portmask;
    char portList[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(portList, 0, sizeof(portList));
    osal_memset(&stk_portmask, 0, sizeof(stk_portmask));

    DIAG_UTIL_ERR_CHK(rtk_trunk_stkTrkPort_get(unit, *stack_trunk_id_ptr, &stk_portmask), ret);

    diag_util_mprintf("Stack trunk %d member port:  ", *stack_trunk_id_ptr);

    diag_util_lPortMask2str(portList, &stk_portmask);

    if(osal_strcmp(portList, "") == 0)
        osal_strcpy(portList, "None");

    diag_util_mprintf("%s\n", portList);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_member_port_stack_trunk_group_stack_trunk_id */
#endif

#ifdef CMD_TRUNK_SET_MEMBER_PORT_STACK_TRUNK_GROUP_STACK_TRUNK_ID_PORTS_NONE
/*
 * trunk set member-port stack-trunk-group <UINT:stack_trunk_id> ( <PORT_LIST:ports> | none )
 */
cparser_result_t
cparser_cmd_trunk_set_member_port_stack_trunk_group_stack_trunk_id_ports_none(
    cparser_context_t *context,
    uint32_t *stack_trunk_id_ptr,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32    ret = RT_ERR_FAILED;
    rtk_portmask_t trunk_member_portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&trunk_member_portmask, 0, sizeof(rtk_portmask_t));

    if ('n' != TOKEN_CHAR(5,0))
        DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(*ports_ptr, &trunk_member_portmask), ret);

    DIAG_UTIL_ERR_CHK(rtk_trunk_stkTrkPort_set(unit, *stack_trunk_id_ptr, &trunk_member_portmask), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_member_port_stack_trunk_group_stack_trunk_id_ports_none */
#endif


#ifdef CMD_TRUNK_GET_STACK_TRUNK_HASH
/*
 * trunk get stack-trunk-hash
 */
cparser_result_t
cparser_cmd_trunk_get_stack_trunk_hash(
    cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_trunk_stkTrkHash_t stkTrkHash;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_stkTrkHash_get(unit, &stkTrkHash), ret);

    diag_util_mprintf("Stacking trunk hash mechanism: ");

    if(STACK_TRK_HASH_KEEP == stkTrkHash)
    {
        diag_util_mprintf("follow the hash result in stacking header to decide outgoing port\n");
    }
    else
    {
        diag_util_mprintf("recalculate a hash result to decide outgoing port\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_stack_trunk_hash */
#endif

#ifdef CMD_TRUNK_SET_STACK_TRUNK_HASH_TRUST_RECALCULATE
/*
 * trunk set stack-trunk-hash ( trust | recalculate )
 */
cparser_result_t
cparser_cmd_trunk_set_stack_trunk_hash_trust_recalculate(
    cparser_context_t *context)
{
    uint32  unit = 0;
    int32    ret = RT_ERR_FAILED;
    rtk_trunk_stkTrkHash_t stkTrkHash;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('t' == TOKEN_CHAR(3, 0))
        stkTrkHash = STACK_TRK_HASH_KEEP;
    else
        stkTrkHash = STACK_TRK_HASH_RECALCULATE;

    DIAG_UTIL_ERR_CHK(rtk_trunk_stkTrkHash_set(unit, stkTrkHash), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_stack_trunk_hash_trust_recalculate */
#endif


#ifdef CMD_TRUNK_GET_DISTRIBUTE_ALGORITHM_BIND_STACK_TRUNK_GROUP_STACK_TRUNK_ID_TYPE_L2_IPV4_IPV6
/*
 * trunk get distribute-algorithm bind stack-trunk-group <UINT:stack_trunk_id> type ( l2 | ipv4 | ipv6 )
 */
cparser_result_t
cparser_cmd_trunk_get_distribute_algorithm_bind_stack_trunk_group_stack_trunk_id_type_l2_ipv4_ipv6(
    cparser_context_t *context,
    uint32_t *stack_trunk_id_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32 algo_idx;
    rtk_trunk_bindType_t type;
    char typeStr[16];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('l' == TOKEN_CHAR(7, 0))
    {
        type = BIND_TYPE_L2;
        osal_strcpy(typeStr, "L2");
    }
    else if ('4' == TOKEN_CHAR(7, 3))
    {
        type = BIND_TYPE_IPV4;
        osal_strcpy(typeStr, "IPV4");
    }
    else if ('6' == TOKEN_CHAR(7, 3))
    {
        type = BIND_TYPE_IPV6;
        osal_strcpy(typeStr, "IPV6");
    }
    else
    {
        diag_util_mprintf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_stkDistributionAlgorithmTypeBind_get(unit, *stack_trunk_id_ptr, type, &algo_idx), ret);

    diag_util_mprintf("Stack trunk %d type %s binds to distribution algorithm %d\n", *stack_trunk_id_ptr, typeStr, algo_idx);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_distribute_algorithm_bind_stack_trunk_group_stack_trunk_id_type_l2_ipv4_ipv6 */
#endif

#ifdef CMD_TRUNK_SET_DISTRIBUTE_ALGORITHM_BIND_STACK_TRUNK_GROUP_STACK_TRUNK_ID_TYPE_L2_IPV4_IPV6_ALGO_ID_ALGO_ID
/*
 * trunk set distribute-algorithm bind stack-trunk-group <UINT:stack_trunk_id> type ( l2 | ipv4 | ipv6 ) algo-id <UINT:algo_id>
 */
cparser_result_t
cparser_cmd_trunk_set_distribute_algorithm_bind_stack_trunk_group_stack_trunk_id_type_l2_ipv4_ipv6_algo_id_algo_id(
    cparser_context_t *context,
    uint32_t *stack_trunk_id_ptr,
    uint32_t *algo_id_ptr)
{
    uint32  unit = 0;
    int32    ret = RT_ERR_FAILED;
    rtk_trunk_bindType_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('l' == TOKEN_CHAR(7, 0))
    {
        type = BIND_TYPE_L2;
    }
    else if ('4' == TOKEN_CHAR(7, 3))
    {
        type = BIND_TYPE_IPV4;
    }
    else if ('6' == TOKEN_CHAR(7, 3))
    {
        type = BIND_TYPE_IPV6;
    }
    else
    {
        diag_util_mprintf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_stkDistributionAlgorithmTypeBind_set(unit, *stack_trunk_id_ptr, type, *algo_id_ptr), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_distribute_algorithm_bind_stack_trunk_group_stack_trunk_id_type_l2_ipv4_ipv6_algo_id_algo_id */
#endif

#ifdef CMD_TRUNK_GET_LOCAL_FIRST_STATE
/*
 * trunk get local-first state
 */
cparser_result_t
cparser_cmd_trunk_get_local_first_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t localFirst;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_localFirst_get(unit, &localFirst), ret);

    diag_util_mprintf("Local first: %s\n", (ENABLED == localFirst)?DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_local_first_state */
#endif

#ifdef CMD_TRUNK_GET_CONGST_AVOID_STATE
/*
 * trunk get congst-avoid state
 */
cparser_result_t
cparser_cmd_trunk_get_congst_avoid_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t congstAvoid;
    rtk_enable_t linkFailAvoid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_localFirstFailOver_get(unit, &congstAvoid, &linkFailAvoid), ret);

    diag_util_mprintf("Congest Avoid: %s\n", (ENABLED == congstAvoid)?DIAG_STR_ENABLE : DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_congst_avoid_state */
#endif

#ifdef CMD_TRUNK_GET_LINK_FAIL_AVOID_STATE
/*
 * trunk get link-fail-avoid state
 */
cparser_result_t
cparser_cmd_trunk_get_link_fail_avoid_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t congstAvoid;
    rtk_enable_t linkFailAvoid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_localFirstFailOver_get(unit, &congstAvoid, &linkFailAvoid), ret);

    diag_util_mprintf("Link Fail Avoid %s\n", (ENABLED == linkFailAvoid)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_link_fail_avoid_state */
#endif

#ifdef CMD_TRUNK_SET_LOCAL_FIRST_STATE_DISABLE_ENABLE
/*
 * trunk set local-first state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trunk_set_local_first_state_disable_enable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t localFirst;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(4, 0))
        localFirst = DISABLED;
    else
        localFirst = ENABLED;

    DIAG_UTIL_ERR_CHK(rtk_trunk_localFirst_set(unit, localFirst), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_local_first_state_disable_enable */
#endif

#ifdef CMD_TRUNK_SET_CONGST_AVOID_STATE_DISABLE_ENABLE
/*
 * trunk set congst-avoid state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trunk_set_congst_avoid_state_disable_enable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t congstAvoid = DISABLED;
    rtk_enable_t linkFailAvoid = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_localFirstFailOver_get(unit, &congstAvoid, &linkFailAvoid), ret);

    if ('d' == TOKEN_CHAR(4, 0))
        congstAvoid = DISABLED;
    else
        congstAvoid = ENABLED;

    DIAG_UTIL_ERR_CHK(rtk_trunk_localFirstFailOver_set(unit, congstAvoid, linkFailAvoid), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_congst_avoid_state_disable_enable */
#endif

#ifdef CMD_TRUNK_SET_LINK_FAIL_AVOID_STATE_DISABLE_ENABLE
/*
 * trunk set link-fail-avoid state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trunk_set_link_fail_avoid_state_disable_enable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t congstAvoid = DISABLED;
    rtk_enable_t linkFailAvoid = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_trunk_localFirstFailOver_get(unit, &congstAvoid, &linkFailAvoid), ret);

    if ('d' == TOKEN_CHAR(4, 0))
        linkFailAvoid = DISABLED;
    else
        linkFailAvoid = ENABLED;

    DIAG_UTIL_ERR_CHK(rtk_trunk_localFirstFailOver_set(unit, congstAvoid, linkFailAvoid), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_link_fail_avoid_state_disable_enable */
#endif


#ifdef CMD_TRUNK_GET_SRC_PORT_MAPPING_DEV_DEV_ID_PORT_PORTS_NONE
/*
 * trunk get src-port-mapping devID <UINT:devID> port ( <PORT_LIST:ports> | none )
 */
cparser_result_t
cparser_cmd_trunk_get_src_port_mapping_devID_devID_port_ports_none(
    cparser_context_t *context,
    uint32_t *unit_id_ptr,
    char **ports_ptr)
{
    uint32  unit;
    int32   ret;
    uint32 is_trk_mbr;
    rtk_dev_port_t dev_port;
    rtk_port_t port;
    rtk_trk_t trk_gid;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        dev_port.devID = *unit_id_ptr;
        dev_port.port = port;
        DIAG_UTIL_ERR_CHK(rtk_trunk_srcPortMap_get(unit, dev_port, &is_trk_mbr, &trk_gid), ret);
        diag_util_mprintf("port %d: %s %d\n", port, (0x1 == is_trk_mbr)?"trunk" : "not trunk", trk_gid);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_get_src_port_mapping_unit_unit_id_port_ports_none */
#endif

#ifdef CMD_TRUNK_SET_SRC_PORT_MAPPING_DEV_DEV_ID_PORT_PORTS_NONE_TRK_GROUP_TRUNK_ID_NONE
/*
 * trunk set src-port-mapping devID <UINT:devID> port ( <PORT_LIST:ports> | none ) trk_group ( <UINT:trunk_id> | none )
 */
cparser_result_t
cparser_cmd_trunk_set_src_port_mapping_devID_devID_port_ports_none_trk_group_trunk_id_none(
    cparser_context_t *context,
    uint32_t *unit_id_ptr,
    char **ports_ptr,
    uint32_t *trunk_id_ptr)
{
    uint32  unit;
    int32   ret;
    uint32 is_trk_mbr;
    rtk_dev_port_t dev_port;
    rtk_trk_t trk_gid;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        dev_port.devID = *unit_id_ptr;
        dev_port.port = port;

        if ('n' == TOKEN_CHAR(8,0))
        {
            is_trk_mbr = 0x0;
            trk_gid = 0;
        }
        else
        {
            is_trk_mbr = 0x1;
            trk_gid = *trunk_id_ptr;
        }

        DIAG_UTIL_ERR_CHK(rtk_trunk_srcPortMap_set(unit, dev_port, is_trk_mbr, trk_gid), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_trunk_set_src_port_mapping_unit_unit_id_port_ports_none_trk_group_trunk_id_none */
#endif

