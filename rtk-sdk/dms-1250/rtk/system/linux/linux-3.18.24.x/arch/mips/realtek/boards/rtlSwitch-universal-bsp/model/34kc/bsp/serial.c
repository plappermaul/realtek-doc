/*
 * Realtek Semiconductor Corp.
 *
 * bsp/serial.c
 *     BSP serial port initialization
 *
 * Copyright 2006-2012 Tony Wu (tonywu@realtek.com)
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>
#include <linux/tty.h>
#include <linux/irq.h>

#include <asm/serial.h>
#include "bspchip.h"
#include <linux/serial_8250.h>
#include <linux/serial_core.h>
#include "chip_probe.h"
#include "bsp_ioal.h"

void bsp_console_init(void)
{
    unsigned int value = 0;
    /* 8 bits, 1 stop bit, no parity. */
    BSP_REG32(UART0_LCR) = ((CHAR_LEN_8 | ONE_STOP | PARITY_DISABLE) << 24);
    /* Reset/Enable the FIFO */
    BSP_REG32(UART0_FCR) = ((FCR_EN | RXRST | TXRST | CHAR_TRIGGER_14) << 24);
    /* Disable All Interrupts */
    BSP_REG32(UART0_IER) = 0x00000000;
    /* Enable Divisor Latch */
    BSP_REG32(UART0_LCR) |= (DLAB << 24);
    /* Set Divisor */
   if((bsp_chip_family_id == RTL9310_FAMILY_ID) || (bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID)  || (bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
   {
        value = (SYSCLK / (BAUDRATE * 16) - 1) & 0x00FF;
        BSP_REG32(UART0_DLL) = (value << 24);

        value = (((SYSCLK / (BAUDRATE * 16) - 1) & 0xFF00) >> 8);
        BSP_REG32(UART0_DLM) = (value << 24);
   }else{

        value = (SYSCLK_9300_MP / (BAUDRATE * 16) - 1) & 0x00FF;
        BSP_REG32(UART0_DLL) = (value << 24);

        value = (((SYSCLK_9300_MP / (BAUDRATE * 16) - 1) & 0xFF00) >> 8);
        BSP_REG32(UART0_DLM) = (value << 24);
   }

    /* Disable Divisor Latch */
    BSP_REG32(UART0_LCR) &= ((~DLAB) << 24);

}

int __init bsp_serial_init(void)
{
#ifdef CONFIG_SERIAL_8250
        int ret;
        struct uart_port s;

        memset(&s, 0, sizeof(s));

        s.type = PORT_16550A;
        s.membase = (unsigned char *) UART0_BASE;
// 		s.irq = BSP_EXT_UART0_IRQ;

        s.irq = BSP_UART0_EXT_IRQ;

        if((bsp_chip_family_id == RTL9310_FAMILY_ID) || (bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID)  || (bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
            s.uartclk = SYSCLK - BAUDRATE * 24;
        else
            s.uartclk = SYSCLK_9300_MP - BAUDRATE * 24;

        s.flags = UPF_SKIP_TEST | UPF_LOW_LATENCY | UPF_SPD_CUST;
        s.iotype = UPIO_MEM;
        s.regshift = 2;
        s.fifosize = 1;

        if((bsp_chip_family_id == RTL9310_FAMILY_ID) || (bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID)  || (bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
            s.custom_divisor = SYSCLK / (BAUDRATE * 16) - 1;
        else
            s.custom_divisor = SYSCLK_9300_MP / (BAUDRATE * 16) - 1;

        /* Call early_serial_setup() here, to set up 8250 console driver */
        if (early_serial_setup(&s) != 0) {
            ret = 1;
        }
#endif
    return 0;
}
