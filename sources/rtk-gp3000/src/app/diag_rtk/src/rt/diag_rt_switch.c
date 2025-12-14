//
// Copyright (C) 2018 Realtek Semiconductor Corp.
// All Rights Reserved.
//
// This program is the proprietary software of Realtek Semiconductor
// Corporation and/or its licensors, and only be used, duplicated,
// modified or distributed under the authorized license from Realtek.
//
// ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
// THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
//
// $Revision:  $
// $Date:  $
//
// Purpose : Define diag shell commands for trunk.
//
// Feature : The file have include the following module and sub-modules
//           1) switch commands.
//
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_switch.h>

/*
 * rt_switch init
 */
cparser_result_t
cparser_cmd_rt_switch_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_switch_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_switch_init */

/*
 * rt_switch get phy-port-id portName <UINT:name>
 */
cparser_result_t
cparser_cmd_rt_switch_get_phy_port_id_portName_name(
    cparser_context_t *context,
    uint32_t  *name_ptr)
{
    int32 ret = RT_ERR_FAILED;
    int32 portId;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_switch_phyPortId_get(*name_ptr, &portId), ret);

    diag_util_mprintf("Port Name ID(%d) Port ID(%d)\n",*name_ptr,portId);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_switch_get_phy_port_id_portname_name */

/*
 * rt_switch get version
 */
cparser_result_t
cparser_cmd_rt_switch_get_version(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 chipId, rev,subtype;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_switch_version_get(&chipId,&rev,&subtype), ret);
    
    diag_util_mprintf("Chid ID(0x%x) Revision(0x%x) Subtype(0x%x)\n",chipId,rev,subtype);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_switch_get_version */

/*
 * rt_switch get max-pkt-len port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_rt_switch_get_max_pkt_len_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rt_port_t port;
    int32 ret = RT_ERR_FAILED;
    uint32 value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port Max Len\n");  
    diag_util_mprintf("----------\n");  
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        diag_util_mprintf("%-4d ", port);

        DIAG_UTIL_ERR_CHK(rt_switch_maxPktLenByPort_get(port, &value), ret);   
        diag_util_mprintf("%-8d \n", value);             
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_switch_get_max_pkt_len_port_ports_all */

/*
 * rt_switch set max-pkt-len port ( <PORT_LIST:ports> | all ) length <UINT:len> */
cparser_result_t
cparser_cmd_rt_switch_set_max_pkt_len_port_ports_all_length_len(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *len_ptr)
{
    diag_portlist_t portlist;
    rt_port_t port;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rt_switch_maxPktLenByPort_set(port, *len_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_switch_set_max_pkt_len_port_ports_all_length_len */


/*
 * rt_switch set mode ( sfu | hgu | hybrid )
 */
cparser_result_t
cparser_cmd_rt_switch_set_mode_sfu_hgu_hybrid(
    cparser_context_t *context)
{
    rt_switch_mode_t mode;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('s' == TOKEN_CHAR(3, 0))
        mode = RT_SWITCH_MODE_SFU;
    else if('g' == TOKEN_CHAR(3, 1))
        mode = RT_SWITCH_MODE_HGU;
    else if('y' == TOKEN_CHAR(3, 1))
        mode = RT_SWITCH_MODE_HYBRID;
    else
        return CPARSER_NOT_OK;
    DIAG_UTIL_ERR_CHK(rt_switch_mode_set(mode), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_switch_set_mode_sfu_hgu_hybrid */

/*
 * rt_switch get mode */
cparser_result_t
cparser_cmd_rt_switch_get_mode(
    cparser_context_t *context)
{
    rt_switch_mode_t mode;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_switch_mode_get(&mode), ret);

    if(mode == RT_SWITCH_MODE_SFU)
        diag_util_mprintf("\n  SFU mode\n");
    else if(mode == RT_SWITCH_MODE_HGU)
        diag_util_mprintf("\n  HGU mode\n");
    else
        diag_util_mprintf("\n  HYBRID mode\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_switch_get_mode */

