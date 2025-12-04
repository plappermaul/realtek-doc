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
 * Purpose : Definition those command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) switch commands.
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
#include <rtk/stp.h>
#include <hal/common/halctrl.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */


/*
 * stp init
 */
cparser_result_t
cparser_cmd_stp_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_stp_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_stp_init */

/*
 * stp get stp-table instance <UINT:instance> port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_stp_get_stp_table_instance_instance_port_ports_all_state(
    cparser_context_t *context,
    uint32_t  *instance_ptr,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;

    rtk_port_t         port = 0;
    rtk_stp_state_t stp_state = 0;
    diag_portlist_t               portlist;

    DIAG_UTIL_PARAM_CHK();

    RT_PARAM_CHK((NULL == instance_ptr), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("MSTI %d Status:\n", *instance_ptr);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_stp_mstpState_get(*instance_ptr, port, &stp_state), ret);
        diag_util_mprintf("\tPort %2d: ", port);

        if (STP_STATE_DISABLED == stp_state)
        {
            diag_util_mprintf("DISABLED\n");
        }
        else if (STP_STATE_BLOCKING == stp_state)
        {
            diag_util_mprintf("BLOCKING\n");
        }
        else if (STP_STATE_LEARNING == stp_state)
        {
            diag_util_mprintf("LEARNING\n");
        }
        else if (STP_STATE_FORWARDING == stp_state)
        {
            diag_util_mprintf("FORWARDING\n");
        }
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    diag_util_mprintf("\n");


    return CPARSER_OK;
}    /* end of cparser_cmd_stp_get_stp_table_instance_instance_port_ports_all_state */

/*
 * stp set stp-table instance <UINT:instance> port ( <PORT_LIST:ports> | all ) state ( blocking | disable | forwarding | learning )
 */
cparser_result_t
cparser_cmd_stp_set_stp_table_instance_instance_port_ports_all_state_blocking_disable_forwarding_learning(
    cparser_context_t *context,
    uint32_t  *instance_ptr,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t         port = 0;
    rtk_stp_state_t stp_state = 0;
    diag_portlist_t               portlist;

    DIAG_UTIL_PARAM_CHK();

    RT_PARAM_CHK((NULL == instance_ptr), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    if ('d' == TOKEN_CHAR(8, 0))
    {
        stp_state = STP_STATE_DISABLED;
    }
    else if ('b' == TOKEN_CHAR(8, 0))
    {
        stp_state = STP_STATE_BLOCKING;
    }
    else if ('l' == TOKEN_CHAR(8, 0))
    {
        stp_state = STP_STATE_LEARNING;
    }
    else if ('f' == TOKEN_CHAR(8, 0))
    {
        stp_state = STP_STATE_FORWARDING;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_stp_mstpState_set(*instance_ptr, port, stp_state), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_stp_set_stp_table_instance_instance_port_ports_all_state_blocking_disable_forwarding_learning */

