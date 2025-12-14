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
#include <rtk/mirror.h>


/*
 * mirror init
 */
cparser_result_t
cparser_cmd_mirror_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_mirror_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mirror_init */

/*
 * mirror dump
 */
cparser_result_t
cparser_cmd_mirror_dump(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t state;
    rtk_port_t port;
    rtk_portmask_t tx_portmask;
    rtk_portmask_t rx_portmask;
    uint8 portStr[20];

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_mirror_portBased_get(&port, &rx_portmask, &tx_portmask), ret);
    diag_util_printf("\n Monitor port: %d", port);
    DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &tx_portmask), ret);
    diag_util_printf("\n Mirroring TX portmask: %s", portStr);
    DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &rx_portmask), ret);
    diag_util_printf("\n Mirroring RX portmask: %s\n", portStr);

#ifdef CONFIG_LUNA_G3_SERIES
    /*no support mirror iso*/
#else
    DIAG_UTIL_ERR_CHK(rtk_mirror_portIso_get(&state), ret);
    diag_util_printf("\n Mirror Egress Mode: %s\n\n", diagStr_mirrorEgressMode[state]);
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_mirror_dump */

/*
 * mirror set egress-mode ( all-pkt | mirrored-only )
 */
cparser_result_t
cparser_cmd_mirror_set_egress_mode_all_pkt_mirrored_only(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if('a'==TOKEN_CHAR(3,0))
        state = DISABLED;
    else
        state = ENABLED;

    DIAG_UTIL_ERR_CHK(rtk_mirror_portIso_set(state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mirror_set_egress_mode_all_pkt_mirrored_only */

/*
 * mirror get egress-mode
 */
cparser_result_t
cparser_cmd_mirror_get_egress_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_mirror_portIso_get(&state), ret);

    diag_util_printf("\n Mirror Egress Mode: %s\n\n", diagStr_mirrorEgressMode[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_mirror_get_egress_mode */

/*
 * mirror set mirroring-port <UINT:port> mirrored-port ( <PORT_LIST:ports> | none ) { rx-mirror } { tx-mirror }
 */
cparser_result_t
cparser_cmd_mirror_set_mirroring_port_port_mirrored_port_ports_none_rx_mirror_tx_mirror(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_portmask_t tx_portmask;
    rtk_portmask_t rx_portmask;

    DIAG_UTIL_PARAM_CHK();

    port = (rtk_port_t)(*port_ptr);
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

    DIAG_UTIL_ERR_CHK(rtk_mirror_portBased_set(port, &rx_portmask, &tx_portmask), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_mirror_set_mirroring_port_port_mirrored_port_ports_none_rx_mirror_tx_mirror */

