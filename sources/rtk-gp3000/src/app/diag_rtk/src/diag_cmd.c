/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * Purpose : Define diag shell functions for system usage.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) 
 */

#include <stdio.h>
#if defined(__linux__) /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
#include <stdint.h>
#endif  /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
#include <string.h>
#include <unistd.h>
#include <diag_util.h>
#include <diag_om.h>
#include <parser/cparser.h>
#include <parser/cparser_token.h>

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

cparser_result_t cparser_cmd_chip_id(cparser_context_t *context,
    uint32_t *id_ptr)
{

    if (NULL == context)
    {
        diag_util_printf("*** [RT_ERR] %s:%d: In function '%s'\n", __FILE__, __LINE__, __FUNCTION__);
        return CPARSER_NOT_OK;
    }
    
    if (*id_ptr > DIAG_OM_CHIP_ID_MAX)
    {
        diag_util_printf("chip id out of range!\n");
        return CPARSER_NOT_OK;
    }
    
    snprintf(context->parser->prompt[context->parser->root_level], 
             sizeof(context->parser->prompt[context->parser->root_level]), "RTK.%d> ", *id_ptr);

    if (diag_om_set_deviceInfo() != RT_ERR_OK)
    {
        diag_util_printf("set device info error!\n");
        return CPARSER_NOT_OK;
    }    
    diag_om_set_chip_id(*id_ptr);
    
    return CPARSER_OK;
}
