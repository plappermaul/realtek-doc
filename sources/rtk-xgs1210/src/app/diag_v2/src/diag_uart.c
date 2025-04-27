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
 * Purpose : Definition those uart1 command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) uart1
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <drv/uart/uart.h>

#ifdef CMD_UART1_GET_CHARACTER_NUMBER_TIMEOUT_TIMEOUT
/*
 * uart1 get character <UINT:number> timeout <UINT:timeout>
 */
cparser_result_t cparser_cmd_uart1_get_character_number_timeout_timeout(cparser_context_t *context,
    uint32_t *number_ptr,
    uint32_t *timeout_ptr)
{
    uint32  unit = 0, number, timeout = 0;
    int32   ret = RT_ERR_FAILED;
    uint8   data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    number = (*number_ptr);
    timeout = (*timeout_ptr);
    diag_util_printf("\n\rGet %u character(s) from UART-1 is: ", number);
    while (number)
    {
        ret = drv_uart_getc(unit, &data, timeout);
        if (ret == RT_ERR_OK)
            diag_util_printf("%c", data);
        else
            break;
        number--;
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_UART1_PUT_CHARACTER_STRING
/*
 * uart1 put character <STRING:string>
 */
cparser_result_t cparser_cmd_uart1_put_character_string(cparser_context_t *context,
    char **string_ptr)
{
    uint32  unit = 0, number, i;
    uint8   data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    number = strlen(TOKEN_STR(3));

    for (i = 0; i < number; i++)
    {
        data = TOKEN_CHAR(3, i);
        drv_uart_putc(unit, data);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_UART1_GET_BAUDRATE
/*
 * uart1 get baudrate
 */
cparser_result_t cparser_cmd_uart1_get_baudrate(cparser_context_t *context)
{
    uint32  unit = 0, baudrate;
    int32   ret = RT_ERR_FAILED;
    uint8   *baudrate_str[5] = {(uint8 *)"9600", (uint8 *)"19200", (uint8 *)"38400",
                                (uint8 *)"57600", (uint8 *)"115200"};

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(drv_uart_baudrate_get(unit, &baudrate), ret);
    diag_util_mprintf("Uart-1 baudrate : %s\n", baudrate_str[baudrate]);

    return CPARSER_OK;
}
#endif

#ifdef CMD_UART1_SET_BAUDRATE_9600_19200_38400_57600_115200
/*
 * uart1 set baudrate ( 9600 | 19200 | 38400 | 57600 | 115200 )
 */
cparser_result_t cparser_cmd_uart1_set_baudrate_9600_19200_38400_57600_115200(cparser_context_t *context)
{
    uint32  unit = 0, baudrate;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    switch (TOKEN_CHAR(3, 0))
    {
        case '9':
            baudrate = UART_BAUDRATE_9600;
            break;
        case '1':
            if (TOKEN_CHAR(3, 1) == '9')
                baudrate = UART_BAUDRATE_19200;
            else
                baudrate = UART_BAUDRATE_115200;
            break;
        case '3':
            baudrate = UART_BAUDRATE_38400;
            break;
        case '5':
            baudrate = UART_BAUDRATE_57600;
            break;
        default:
            baudrate = UART_BAUDRATE_END;
            break;
    }

    DIAG_UTIL_ERR_CHK(drv_uart_baudrate_set(unit, baudrate), ret);

    return CPARSER_OK;
}
#endif
