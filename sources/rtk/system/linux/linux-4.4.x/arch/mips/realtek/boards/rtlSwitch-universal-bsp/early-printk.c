/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2015, Tony Wu <tonywu@realtek.com>
 */

#include <common/rt_type.h>
#include <linux/io.h>
#include <linux/serial_reg.h>
#include <asm/addrspace.h>

#include "rtk_bsp_driver.h"
#include "bspchip.h"

static __iomem void *uart_base;

static inline void uart_w32(u32 val, unsigned reg)
{
    __raw_writel(val, uart_base + (reg));
}

static inline u32 uart_r32(unsigned reg)
{
    return __raw_readl(uart_base + (reg));
}

void early_uart_init(void)
{

    uart_base = (__iomem void *) KSEG1ADDR(rtk_bspUart0_phyAddr_get());
}

void prom_putchar(unsigned char ch)
{
    while ((uart_r32(U_LSR) & (LSR_THRE << 24)) == 0)
        ;
    uart_w32((ch << 24), U_THR);
}
