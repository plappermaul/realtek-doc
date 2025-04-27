/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 80855 $
 * $Date: 2017-07-27 13:45:32 +0800 (Thu, 27 Jul 2017) $
 *
 * Purpose : Realtek Switch SDK UART1 Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) SDK UART1 Module
 *
 */

/*
 * Include Files
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <common/rt_autoconf.h>
#include <drv/uart/uart.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <dal/rtrpc/rtrpc_uart.h>
#include <dal/rtrpc/rtrpc_msg.h>

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
int32 drv_uart_getc(uint32 unit, uint8 *pData, uint32 timeout)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    unit_cfg.unit = unit;
    unit_cfg.data = timeout;
    GETSOCKOPT(RTDRV_UART1_GETC, &unit_cfg, rtdrv_unitCfg_t, 1);
    (*pData) = unit_cfg.data8;

    return RT_ERR_OK;
}

int32 drv_uart_putc(uint32 unit, uint8 data)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    unit_cfg.unit = unit;
    unit_cfg.data8 = data;
    SETSOCKOPT(RTDRV_UART1_PUTC, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 drv_uart_baudrate_get(uint32 unit, drv_uart_baudrate_t *pBaudarte)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_UART1_BAUDRATE_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    (*pBaudarte) = unit_cfg.data;

    return RT_ERR_OK;
}

int32 drv_uart_baudrate_set(uint32 unit, drv_uart_baudrate_t baudarte)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    unit_cfg.unit = unit;
    unit_cfg.data = baudarte;
    SETSOCKOPT(RTDRV_UART1_BAUDRATE_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 drv_uart_interface_set(uint32 unit, drv_uart_interface_t baudarte)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    unit_cfg.unit = unit;
    unit_cfg.data = baudarte;
    SETSOCKOPT(RTDRV_UART1_INTERFACE_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

