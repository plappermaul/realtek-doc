/*
 * Copyright (C) 2009-2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 97155 $
 * $Date: 2019-11-10 14:22:30 +0800 (Sun, 10 Nov 2019) $
 *
 * Purpose : RTL9310 INTR source mapping
 *
 * Feature : RTL9310 INTR source mapping
 *
 */

/* Interrupt source index*/
#define RTL9310_TC0_INTRSRC         0
#define RTL9310_TC1_INTRSRC         1
#define RTL9310_TC2_INTRSRC         2
#define RTL9310_TC3_INTRSRC         3
#define RTL9310_TC4_INTRSRC         4
#define RTL9310_TC5_INTRSRC         5
#define RTL9310_TC6_INTRSRC         6
#define RTL9310_WDT0_INTRSRC        8
#define RTL9310_WDT1_INTRSRC        9
#define RTL9310_RXI300_INTRSRC      11
#define RTL9310_SWCORE_INTRSRC      15
#define RTL9310_NIC_INTRSRC         16
#define RTL9310_GPIO_INTRSRC        20
#define RTL9310_UART0_INTRSRC       22
#define RTL9310_UART1_INTRSRC       23
#define RTL9310_TC4DEL_INTRSRC      28
#define RTL9310_TC5DEL_INTRSRC      29
#define RTL9310_TC6DEL_INTRSRC      30
#define RTL9310_LXSTO_INTRSRC       32
#define RTL9310_LXMTO_INTRSRC       33
#define RTL9310_BTG_INTRSRC         34
#define RTL9310_USBH2_INTRSRC       36
#define RTL9310_SPINAND_INTRSRC     37
#define RTL9310_ECC_INTRSRC         38
#define RTL9310_SEC_INTRSRC         39

/* Interrupt device tree node name */
#define RTL9310_TC0                 "rtk_tc0"
#define RTL9310_TC1                 "rtk_tc1"
#define RTL9310_TC2                 "rtk_tc2"
#define RTL9310_TC3                 "rtk_tc3"
#define RTL9310_TC4                 "rtk_tc4"
#define RTL9310_TC5                 "rtk_tc5"
#define RTL9310_TC6                 "rtk_tc6"
#define RTL9310_WDT0                "rtk_wdt0"
#define RTL9310_WDT1                "rtk_wdt1"
#define RTL9310_RXI300              "rtk_rxi300"
#define RTL9310_SWCORE              "rtk_swcore"
#define RTL9310_NIC                 "rtk_nic"
#define RTL9310_GPIO                "rtk_gpio"
#define RTL9310_UART0               "ns16550a"
#define RTL9310_UART1               "rtk_uart1"
#define RTL9310_DEL_TC4             "rtk_del_tc4"
#define RTL9310_DEL_TC5             "rtk_del_tc5"
#define RTL9310_DEL_TC6             "rtk_del_tc6"
#define RTL9310_LXSTO               "rtk_lxsto"
#define RTL9310_LXMTO               "rtk_lxmto"
#define RTL9310_BTG                 "rtk_btg"
#define RTL9310_USBH2               "rtk_usbh2"
#define RTL9310_SPI_NAND            "rtk_spinand"
#define RTL9310_ECC                 "rtk_ecc"
#define RTL9310_SEC                 "rtk_sec"

#define RTL9310_GET_INTR_ID0        0


