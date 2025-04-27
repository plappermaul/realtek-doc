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
 * Purpose : Definition those public uart APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) uart probe
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <hwp/hw_profile.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <private/drv/uart/uart_mapper.h>
#include <hwp/hw_profile.h>
#include <private/drv/uart/uart_common.h>

/*
 * Symbol Definition
 */
#define UART_DB_SIZE (sizeof(uart_db)/sizeof(cid_prefix_group_t))

/*
 * Data Declaration
 */
const static cid_prefix_group_t uart_db[] =
{
#if defined(CONFIG_SDK_RTL9310)
    {RTL9310_FAMILY_ID, UART_R9310},
#endif
#if defined(CONFIG_SDK_RTL9300)
    {RTL9300_FAMILY_ID, UART_R9300},
#endif
#if defined(CONFIG_SDK_RTL8390)
    /* RTL8390 series chips */
    {RTL8390_FAMILY_ID, UART_R8390},
    {RTL8350_FAMILY_ID, UART_R8390},
#endif
#if defined(CONFIG_SDK_RTL8380)
    /* RTL8380 series chips */
    {RTL8380_FAMILY_ID, UART_R8380},
    {RTL8330_FAMILY_ID, UART_R8380},
#endif
};

uart_mapper_operation_t uart_ops[UART_CTRL_END] =
{
#if defined(CONFIG_SDK_RTL9310)
    {   /* UART_R9300 */
        .init = uart_init,
        .interface_set = uart_interface_set,
        .tstc = uart_tstc,
        .poll_getc = uart_getc,
        .poll_putc = uart_putc,
        .baudrate_get = uart_baudrate_get,
        .baudrate_set = uart_baudrate_set,
        .starttx = serial_starttx,
        .clearfifo = serial_clearfifo,
    },
#endif
#if defined(CONFIG_SDK_RTL9300)
    {   /* UART_R9300 */
        .init = uart_init,
        .interface_set = uart_interface_set,
        .tstc = uart_tstc,
        .poll_getc = uart_getc,
        .poll_putc = uart_putc,
        .baudrate_get = uart_baudrate_get,
        .baudrate_set = uart_baudrate_set,
        .starttx = serial_starttx,
        .clearfifo = serial_clearfifo,
    },
#endif
#if defined(CONFIG_SDK_RTL8390)
    {   /* UART_R8390 */
        .init = uart_init,
        .interface_set = uart_interface_set,
        .tstc = uart_tstc,
        .poll_getc = uart_getc,
        .poll_putc = uart_putc,
        .baudrate_get = uart_baudrate_get,
        .baudrate_set = uart_baudrate_set,
        .starttx = serial_starttx,
        .clearfifo = serial_clearfifo,
    },
#endif
#if defined(CONFIG_SDK_RTL8380)
    {   /* UART_R8380 */
        .init = uart_init,
        .interface_set = uart_interface_set,
        .tstc = uart_tstc,
        .poll_getc = uart_getc,
        .poll_putc = uart_putc,
        .baudrate_get = uart_baudrate_get,
        .baudrate_set = uart_baudrate_set,
        .starttx = serial_starttx,
        .clearfifo = serial_clearfifo,
    },
#endif
};

uint32 uart_if[RTK_MAX_NUM_OF_UNIT] = {CID_GROUP_NONE};


/*
 * Function Declaration
 */

/* Function Name:
 *      uart_probe
 * Description:
 *      Probe uart module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
uart_probe(uint32 unit)
{
    uint32      i;
    uint32      cid;

    RT_INIT_MSG("  UART probe (unit %d): ", unit);

    cid = HWP_CHIP_ID(unit);

    for (i = 0; i < UART_DB_SIZE; i++)
    {
        if (CID_PREFIX_MATCH(uart_db[i], cid))
        {
            uart_if[unit] = uart_db[i].gid;
            RT_INIT_MSG("(found)\n");
            return RT_ERR_OK;
        }
    }

    RT_INIT_MSG("(Not found)\n");
    return RT_ERR_FAILED;
} /* end of uart_probe */
