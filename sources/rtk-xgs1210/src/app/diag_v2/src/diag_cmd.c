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
 * $Revision: 85850 $
 * $Date: 2018-01-30 10:25:11 +0800 (Tue, 30 Jan 2018) $
 *
 * Purpose : Define diag shell functions for system usage.
 *
 * Feature : The file have include the following module and sub-modules
 *           1)
 */

#include <stdio.h>
#if defined(__linux__)
#include <stdint.h>
#endif
#include <string.h>
#include <unistd.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser.h>
#include <parser/cparser_token.h>
#include <hwp/hw_profile.h>

/**
 * Exit the parser test program.
 */
cparser_result_t
cparser_cmd_exit(cparser_context_t *context)
{
    if (NULL == context)
    {
        diag_util_printf("*** [RT_ERR] %s:%d: In function '%s'\n", __FILE__, __LINE__, __FUNCTION__);
        return CPARSER_NOT_OK;
    }

    return cparser_quit(context->parser);
}


/* unit <UINT:id> */
cparser_result_t cparser_cmd_unit_id(cparser_context_t *context, uint32_t *id_ptr)
{
    if (NULL == context)
    {
        diag_util_printf("*** [RT_ERR] %s:%d: In function '%s'\n", __FILE__, __LINE__, __FUNCTION__);
        return CPARSER_NOT_OK;
    }

    if (*id_ptr > DIAG_OM_UNIT_ID_MAX)
    {
        diag_util_printf("Unit id out of range!\n");
        return CPARSER_NOT_OK;
    }

    if (diag_om_set_deviceInfo(*id_ptr) != RT_ERR_OK)
    {
        diag_util_printf("Obtain unit %d device information fail!\n", *id_ptr);
        return CPARSER_NOT_OK;
    }

    diag_om_set_unitId(*id_ptr);

    diag_om_get_promptString((uint8 *)context->parser->prompt[context->parser->root_level],
                              sizeof(context->parser->prompt[context->parser->root_level]),
                              *id_ptr);
    return CPARSER_OK;
}


/*
 * terminal set pager length <UINT:num>
 */
cparser_result_t cparser_cmd_terminal_set_pager_length_num(cparser_context_t *context, uint32_t *num_ptr)
{
    diag_util_printf("Configure pager line number as %d\n", *num_ptr);
    diag_util_mprintf_paging_lines (*num_ptr);

    return CPARSER_OK;
}

/*
 * sdk set rtk-api-backward-compatible state ( enable | disable )
 */
cparser_result_t cparser_cmd_sdk_set_rtk_api_backward_compatible_state_enable_disable(cparser_context_t *context)
{
    if ('e' == TOKEN_CHAR(4, 0))
    {
        diag_om_set_backwardCompatible(DIAG_BACKWARD_COMPATIBLE);
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        diag_om_set_backwardCompatible(DIAG_NOT_BACKWARD_COMPATIBLE);
    }
    diag_util_printf("backward Compatible_flag %s\n", (backwardCompatible_flag ? DIAG_STR_ENABLE : DIAG_STR_DISABLE));

    return CPARSER_OK;
}

