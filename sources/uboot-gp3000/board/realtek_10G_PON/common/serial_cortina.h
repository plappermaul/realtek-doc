/*
 *  u-boot/drivers/serial/serial_cortina.h
 *
 * Copyright (c) Cortina-Access Limited 2024.  All rights reserved.
 *                Jason Li <jason.li@cortina-access.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Cortina UART Module
 */
#define UART0_BASE_ADDR CONFIG_SYS_SERIAL0
#define UART1_BASE_ADDR CONFIG_SYS_SERIAL1

#define CONFIG_CONS_INDEX 0

/* Register definitions */
#define UCFG 0x00 /* UART config register */
#define UFC 0x04 /* Flow Control */
#define URX_SAMPLE 0x08 /* UART RX Sample register */
#define URT_TUNE 0x0C /* Fine tune of UART clk */
#define UTX_DATA 0x10 /* UART TX Character data */
#define URX_DATA 0x14 /* UART RX Character data */
#define UINFO 0x18 /* UART Info */
#define UINT_EN0 0x1C /* UART Interrupt enable 0*/
#define UINT_EN1 0x20 /* UART Interrupt enable 1*/
#define UINT0 0x24 /* UART Interrupt 0 setting/clearing */
#define UINT1 0x28 /* UART Interrupt 1 setting/clearing */
#define UINT_STAT 0x2C /* UART Interrupt Status */
/* UART Control Register Bit Fields.*/
#define UCFG_BAUD_COUNT (1 << 8)
#define UCFG_EN (1 << 7)
#define UCFG_RX_EN (1 << 6)
#define UCFG_TX_EN (1 << 5)
#define UCFG_PARITY_EN (1 << 4)
#define UCFG_PARITY_SEL (1 << 3)
#define UCFG_2STOP_BIT (1 << 2)
#define UCFG_CNT1 (1 << 1)
#define UCFG_CNT0 (1 << 0)
#define UCFG_CHAR_5 0
#define UCFG_CHAR_6 1
#define UCFG_CHAR_7 2
#define UCFG_CHAR_8 3

#define UINFO_TX_FIFO_EMPTY (1 << 3)
#define UINFO_TX_FIFO_FULL (1 << 2)
#define UINFO_RX_FIFO_EMPTY (1 << 1)
#define UINFO_RX_FIFO_FULL (1 << 0)

#define UINT_RX_NON_EMPTY (1 << 6)
#define UINT_TX_EMPTY (1 << 5)
#define UINT_RX_UNDERRUN (1 << 4)
#define UINT_RX_OVERRUN (1 << 3)
#define UINT_RX_PARITY_ERR (1 << 2)
#define UINT_RX_STOP_ERR (1 << 1)
#define UINT_TX_OVERRUN (1 << 0)
#define UINT_MASK_ALL 0x7F

// UART CONF bits
#define SHF_UCONF_WL 0
#define MSK_UCONF_WL (0x3 << SHF_UCONF_WL)
#define VAL_UCONF_WL_5 (0x0 << SHF_UCONF_WL)
#define VAL_UCONF_WL_6 (0x1 << SHF_UCONF_WL)
#define VAL_UCONF_WL_7 (0x2 << SHF_UCONF_WL)
#define VAL_UCONF_WL_8 (0x3 << SHF_UCONF_WL)

#define SHF_UCONF_SB 2
#define MSK_UCONF_SB (0x1 << SHF_UCONF_SB)
#define VAL_UCONF_SB_1 (0x0 << SHF_UCONF_SB)
#define VAL_UCONF_SB_2 (0x1 << SHF_UCONF_SB)

#define SHF_UCONF_PM 3
#define MSK_UCONF_PM (0x3 << SHF_UCONF_PM)
#define VAL_UCONF_PM_N (0x0 << SHF_UCONF_PM)
#define VAL_UCONF_PM_O (0x2 << SHF_UCONF_PM)
#define VAL_UCONF_PM_E (0x3 << SHF_UCONF_PM)
