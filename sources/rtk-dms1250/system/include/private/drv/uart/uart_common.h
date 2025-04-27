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
 *            1) uart init
 *            2) uart set & get
 */
#ifndef __UART_RTL8380_H__
#define __UART_RTL8380_H__

/*
 * Include Files
 */
#include <drv/uart/uart.h>
#include <soc/socdef/rtl8380_soc_reg.h>
#include <soc/socdef/rtl8390_soc_reg.h>
#include <soc/socdef/rtl9300_soc_reg.h>
#include <soc/socdef/rtl9310_soc_reg.h>
#include <common/rt_type.h>


/*
 * Symbol Definition
 */

/*
 * Data Type Definition
 */

/*
 * Data Declaration
 */
typedef struct uart_addr_s
{
    uint32 reg_addr;
} uart_addr_t;

typedef struct uart_reg_definition_s
{
    uart_addr_t uart1_rx_tx_div_lsb_addr;
    uart_addr_t uart1_intr_en_div_msb_addr;
    uart_addr_t uart1_line_sts_addr;
    uart_addr_t uart1_line_ctrl_addr;
    uart_addr_t uart1_intr_id_fifo_ctrl_addr;
    uart_addr_t uart1_modem_ctrl_addr;
} uart_reg_definition_t;

/*
 * Macro Definition
 */
extern uart_reg_definition_t uart_reg[];
#define UART_REG(unit)       uart_reg[uart_if[unit]]

/*
 * Function Declaration
 */

/* Function Name:
 *      uart_init
 * Description:
 *      Init the uart module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
uart_init(uint32 unit,drv_uart_rxcallback_t rxcallback,drv_uart_txcallback_t txcallback);

/* Function Name:
 *      uart_interface_set
 * Description:
 *      Init the common GPIO interface.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
uart_interface_set(uint32 unit, drv_uart_interface_t interface);


/* Function Name:
 *      uart_tstc
 * Description:
 *      test if serial data in
 * Input:
 *      unit  - unit id
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
uart_tstc(uint32 unit);

/* Function Name:
 *      uart_getc
 * Description:
 *      Get the character from uart interface
 * Input:
 *      unit    - unit id
 * Output:
 *      pData   - pointer buffer of character from uart interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
uart_getc(uint32 unit, uint8 u8len, uint8 *pData);

/* Function Name:
 *      uart_putc
 * Description:
 *      Output the character to uart interface in the specified device
 * Input:
 *      unit - unit id
 * Output:
 *      data - output the character to uart interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
uart_putc(uint32 unit, uint8 u8len, uint8 *data);

/* Function Name:
 *      uart_baudrate_get
 * Description:
 *      Get the baudrate of the uart interface in the specified device
 * Input:
 *      unit      - unit id
 * Output:
 *      pBaudrate - pointer buffer of baudrate value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
uart_baudrate_get(uint32 unit, drv_uart_baudrate_t *pBaudrate);

/* Function Name:
 *      uart_baudrate_set
 * Description:
 *      Configure the baudrate of the uart interface in the specified device
 * Input:
 *      unit     - unit id
 *      baudrate - baudrate value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
uart_baudrate_set(uint32 unit, drv_uart_baudrate_t baudrate);

/* Function Name:
 *      serial_starttx
 * Description:
 *      trigger hw to start tx.
 * Input:
 *      unit  - unit id
 * Output:
 *
 * Return:
 * Note:
 */
extern void
serial_starttx(uint32 unit);

/* Function Name:
 *      serial_clearfifo
 * Description:
 *      empty hw rx/tx fifo.
 * Input:
 *      unit  - unit id
 * Output:
 *
 * Return:
 * Note:
 *      for sync io, poe should call this function before exchange message
 */
extern void
serial_clearfifo(uint32 unit);
#endif /* __UART_RTL8380_H__ */
