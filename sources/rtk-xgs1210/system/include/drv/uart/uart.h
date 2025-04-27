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
 * $Revision: 81064 $
 * $Date: 2017-08-07 19:21:35 +0800 (Mon, 07 Aug 2017) $
 *
 * Purpose : DRV APIs definition.
 *
 * Feature : Uart relative API
 *
 */

#ifndef __DRV_UART_H__
#define __DRV_UART_H__


/*
 * Include Files
 */
#include <common/error.h>

/*
 * Symbol Definition
 */

#define LCR_BKSE             0x80                /* Bank select enable */
#define LCRVAL               LCR_8N1             /* 8 data, 1 stop, no parity */
#define MCRVAL               (MCR_DTR | MCR_RTS) /* RTS/DTR */
#define FCRVAL               0xC1                /* Clear & enable FIFOs */
#define LCR_8N1              0x03
#define MCR_DTR              0x01               /* DTR complement */
#define MCR_RTS              0x02               /* RTS complement */
#define MCR_TCRTLR           0x40               /* Access TCR/TLR (TI16C752, EFR[4]=1) */
#define UART_IER_MSI         0x08               /* Enable Modem status interrupt */
#define UART_IER_RLSI        0x04               /* Enable receiver line status interrupt */
#define UART_IER_THRI        0x02               /* Enable Transmitter holding register int. */
#define UART_IER_RDI         0x01               /* Enable receiver data interrupt */
#define UART_IIR_NO_INT      0x01               /* No interrupts pending */
#define UART_LSR_DR          0x01               /* Receiver data ready */
#define UART_LSR_THRE        0x20               /* Transmit-hold-register empty */
#define UART_LCR_WLEN8       0x03               /* Wordlength: 8 bits */
#define UART_LCR_DLAB        0x80               /* Divisor latch access bit */
#define UART_FCR_ENABLE_FIFO 0x01               /* Enable the FIFO */
#define UART_FCR_CLEAR_RCVR  0x02               /* Clear the RCVR FIFO */
#define UART_FCR_CLEAR_XMIT  0x04               /* Clear the XMIT FIFO */


#define UART_RESERVED_FILED 24

/* Divisor Latch Value for different baudrate when system clock is 200MHz */
#define BAUDRTAE_9600_DIVISOR_LATCH_VALUE_IN_SYS_200MHZ     0x0515
#define BAUDRTAE_19200_DIVISOR_LATCH_VALUE_IN_SYS_200MHZ    0x028A
#define BAUDRTAE_38400_DIVISOR_LATCH_VALUE_IN_SYS_200MHZ    0x0145
#define BAUDRTAE_57600_DIVISOR_LATCH_VALUE_IN_SYS_200MHZ    0x00D8
#define BAUDRTAE_115200_DIVISOR_LATCH_VALUE_IN_SYS_200MHZ   0x006C

/*
 * Data Declaration
 */

typedef int32 (*drv_uart_txcallback_t)(uint32 unit, uint8 *inch);
typedef int32 (*drv_uart_rxcallback_t)(uint32 unit, uint8 data);

typedef enum drv_uart_baudrate_e
{
    UART_BAUDRATE_9600,
    UART_BAUDRATE_19200,
    UART_BAUDRATE_38400,
    UART_BAUDRATE_57600,
    UART_BAUDRATE_115200,
    UART_BAUDRATE_END,
} drv_uart_baudrate_t;

typedef enum drv_uart_interface_e
{
    JTAG_INTERFACE,
    UART1_INTERFACE,
    GPIO_INTERFACE,
} drv_uart_interface_t;

struct serial_info_s {
    drv_uart_baudrate_t baudrate;
    drv_uart_txcallback_t  getTxChar;
    drv_uart_rxcallback_t  putRxChar;
};

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */


/* Function Name:
 *      drv_uart_clearfifo
 * Description:
 *      empty hw rx/tx fifo and reset queue information to default value
 * Input:
 *      que - queue pointer
 *      inch - serial out data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_uart_clearfifo(uint32 unit);

/* Function Name:
 *      drv_uart_getc
 * Description:
 *      Get the character from uart interface with timeout value in the specified device
 * Input:
 *      unit    - unit id
 *      timeout - timeout value (unit: milli-second), 0 mean no timeout
 * Output:
 *      pData   - pointer buffer of character from uart interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_BUSYWAIT_TIMEOUT - timeout and no get the character
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_uart_getc(uint32 unit, uint8 *pData, uint32 timeout);

/* Function Name:
 *      drv_uart_gets
 * Description:
 *      Get the character(s) from uart queue buffer with timeout value in the specified device
 * Input:
 *      unit    - unit id
 *      timeout - timeout value (unit: milli-second), 0 mean no timeout
 * Output:
 *      pData   - pointer buffer of character from uart interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_BUSYWAIT_TIMEOUT - timeout and no get the character
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_uart_gets(uint32 unit, uint8 *pData, uint32 expect_size,uint32 *rev_cnt,uint32 timeout);

/* Function Name:
 *      drv_uart_putc
 * Description:
 *      Output the character to uart interface in the specified device
 * Input:
 *      unit - unit id
 * Output:
 *      data - output the character to uart interface
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
extern int32
drv_uart_putc(uint32 unit, uint8 data);

/* Function Name:
 *      drv_uart_puts
 * Description:
 *      Output the character(s) to uart queue buffer in the specified device
 * Input:
 *      unit - unit id
 * Output:
 *      data - output the character to uart interface
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
extern int32
drv_uart_puts(uint32 unit, uint8 *data,uint32 size);

/* Function Name:
 *      drv_uart_baudrate_get
 * Description:
 *      Get the baudrate of the uart interface in the specified device
 * Input:
 *      unit      - unit id
 * Output:
 *      pBaudrate - pointer buffer of baudrate value
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
extern int32
drv_uart_baudrate_get(uint32 unit, drv_uart_baudrate_t *pBaudrate);

/* Function Name:
 *      drv_uart_baudrate_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_uart_baudrate_set(uint32 unit, drv_uart_baudrate_t baudrate);


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
extern int32
uart_probe(uint32 unit);


/* Function Name:
 *      drv_uart_init
 * Description:
 *      Init the uart module of the specified device.
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
extern int32
drv_uart_init(uint32 unit);

/* Function Name:
 *      drv_uart_interface_set
 * Description:
 *      Init the common GPIO interface.
 * Input:
 *      unit - unit id
 *      interface - common pin interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
drv_uart_interface_set(uint32 unit, drv_uart_interface_t interface);


#endif /* __DRV_UART_H__ */

