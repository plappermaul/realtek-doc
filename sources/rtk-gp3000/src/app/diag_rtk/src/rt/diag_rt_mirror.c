//
// Copyright (C) 2019 Realtek Semiconductor Corp.
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
// Purpose : Define diag shell commands for mirror.
//
// Feature : The file have include the following module and sub-modules
//           1) port mirror commands.
//

#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_mirror.h>

/*
 * rt_mirror init
 */
cparser_result_t
cparser_cmd_rt_mirror_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_mirror_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mirror_init */

/*
 * rt_mirror dump
 */
cparser_result_t
cparser_cmd_rt_mirror_dump(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_enable_t state;
    rt_port_t port;
    rt_portmask_t tx_portmask;
    rt_portmask_t rx_portmask;
    rtk_portmask_t tmpPortmask;
    uint8 portStr[20];

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_mirror_portBased_get(&port, &rx_portmask, &tx_portmask), ret);
    diag_util_printf("\n Monitor port: %d", port);
    
    memcpy(tmpPortmask.bits,tx_portmask.bits,RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST*sizeof(uint32));
    DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &tmpPortmask), ret);
    diag_util_printf("\n Mirroring TX portmask: %s", portStr);

    memcpy(tmpPortmask.bits,rx_portmask.bits,RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST*sizeof(uint32));
    DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &tmpPortmask), ret);
    diag_util_printf("\n Mirroring RX portmask: %s\n", portStr);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mirror_dump */

/*
 * rt_mirror set mirroring-port <UINT:port> mirrored-port ( <PORT_LIST:ports> | none ) { rx-mirror } { tx-mirror }
 */
cparser_result_t
cparser_cmd_rt_mirror_set_mirroring_port_port_mirrored_port_ports_none_rx_mirror_tx_mirror(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_portmask_t tx_portmask;
    rt_portmask_t rx_portmask;

    DIAG_UTIL_PARAM_CHK();

    port = (rt_port_t)(*port_ptr);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if(TOKEN_NUM() == 6)
    {
        /* no TX / no RX */
        tx_portmask.bits[0] = 0;
        rx_portmask.bits[0] = 0;
    }
    else if(TOKEN_NUM() == 8)
    {
        /* both TX and RX*/
        tx_portmask.bits[0] = portlist.portmask.bits[0];
        rx_portmask.bits[0] = portlist.portmask.bits[0];
    }
    else if(TOKEN_NUM() == 7)
    {
        if('t'==TOKEN_CHAR(6,0))
        {
            /* TX Only */
            tx_portmask.bits[0] = portlist.portmask.bits[0];
            rx_portmask.bits[0] = 0;
        }
        else if('r'==TOKEN_CHAR(6,0))
        {
            /* RX Only */
            tx_portmask.bits[0] = 0;
            rx_portmask.bits[0] = portlist.portmask.bits[0];
        }
        else
            return CPARSER_NOT_OK;
    }
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rt_mirror_portBased_set(port, &rx_portmask, &tx_portmask), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mirror_set_mirroring_port_port_mirrored_port_ports_none_rx_mirror_tx_mirror */

