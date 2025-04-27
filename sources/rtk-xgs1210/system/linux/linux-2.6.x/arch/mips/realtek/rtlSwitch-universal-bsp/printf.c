/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
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
 * $Revision: 83626 $
 * $Date: 2017-11-21 16:06:39 +0800 (Tue, 21 Nov 2017) $
 *
 */

/*
 * Include Files
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <asm/io.h>
#include <asm/system.h>

#include <prom.h>
#include <platform.h>
#include "chip_probe.h"

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
extern unsigned int bsp_chip_id, bsp_chip_rev_id;
extern unsigned int bsp_chip_family_id, bsp_chip_type;

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */
void  __init prom_console_init(void)
{
   /* 8 bits, 1 stop bit, no parity. */
   REG8(UART0_LCR) = CHAR_LEN_8 | ONE_STOP | PARITY_DISABLE;

   /* Reset/Enable the FIFO */
   REG8(UART0_FCR) = FCR_EN | RXRST | TXRST | CHAR_TRIGGER_14;

   /* Disable All Interrupts */
   REG8(UART0_IER) = 0x00000000;

   /* Enable Divisor Latch */
   REG8(UART0_LCR) |= DLAB;

   /* Set Divisor */
   if((bsp_chip_family_id == RTL9310_FAMILY_ID) || (bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID)  || (bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
   {
       REG8(UART0_DLL) = (SYSCLK / (BAUDRATE * 16) - 1) & 0x00FF;
       REG8(UART0_DLM) = ((SYSCLK / (BAUDRATE * 16) - 1) & 0xFF00) >> 8;
   }else{
       REG8(UART0_DLL) = (SYSCLK_9300_MP / (BAUDRATE * 16) - 1) & 0x00FF;
       REG8(UART0_DLM) = ((SYSCLK_9300_MP / (BAUDRATE * 16) - 1) & 0xFF00) >> 8;
   }
   /* Disable Divisor Latch */
   REG8(UART0_LCR) &= (~DLAB);
}

int prom_putchar(char c)
{
   unsigned int busy_cnt = 0;

   do
   {
      /* Prevent Hanging */
      if (busy_cnt++ >= 30000)
      {
         /* Reset Tx FIFO */
         REG8(UART0_FCR) = TXRST | CHAR_TRIGGER_14;
         return 0;
      }
   } while ((REG8(UART0_LSR) & LSR_THRE) == TxCHAR_AVAIL);

   /* Send Character */
   REG8(UART0_THR) = c;

   return 1;
}
