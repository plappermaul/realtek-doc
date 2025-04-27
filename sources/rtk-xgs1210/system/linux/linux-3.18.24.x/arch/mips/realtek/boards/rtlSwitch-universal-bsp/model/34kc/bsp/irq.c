/*
 * Realtek Semiconductor Corp.
 *
 * bsp/irq.c
 *     bsp interrupt initialization and handler code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel_stat.h>
#include <linux/kernel.h>
#include <linux/random.h>
#include <asm/bitops.h>
#include <asm/bootinfo.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/irq_cpu.h>
#include <asm/mipsregs.h>
#include <asm/gic.h>
#include "bspchip.h"
#include "int_setup.h"
#include "chip_probe.h"

#include <asm/mips-cm.h>
#include <asm/mips-cpc.h>
#include <asm/gic.h>

/* GIC is transparent, just bridge*/
struct gic_intr_map rtl9310_gic_intr_map[GIC_NUM_INTRS] = {
    /* EXT_INT 0~7 */
    { 0, GIC_CPU_INT4,	GIC_POL_POS,		GIC_TRIG_LEVEL,		GIC_FLAG_TRANSPARENT },   /* Timer 0 *//*Index 0*/
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 1 *//*Index 1*/
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 2 *//*Index 2*/
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 3 *//*Index 3*/
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 4 *//*Index 4*/
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 5 *//*Index 5*/
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 6 *//*Index 6*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT3,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* WDT_IP1_IRQ *//*Index 8*/
    { 0, GIC_CPU_INT4,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* WDT_IP2_IRQ *//*Index 9*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /*Index 10*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT2,	GIC_POL_POS,		GIC_TRIG_LEVEL,		GIC_FLAG_TRANSPARENT },	  /* Switch Core *//* Index 15 */
    { 0, GIC_CPU_INT3,	GIC_POL_POS,		GIC_TRIG_LEVEL,		GIC_FLAG_TRANSPARENT },	  /* NIC *//* Index 16*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT3,  GIC_POL_POS,        GIC_TRIG_LEVEL,     GIC_FLAG_TRANSPARENT },   /* GPIO *//* Index 20 */
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT1,	GIC_POL_POS,		GIC_TRIG_LEVEL,		GIC_FLAG_TRANSPARENT },	  /* UART0 *//*Index 22*/
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* UART1 *//*Index 23*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,  GIC_POL_POS,        GIC_TRIG_LEVEL,     0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Index 30 */
    { 0, GIC_CPU_INT5,  GIC_POL_POS,        GIC_TRIG_LEVEL,     0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,  GIC_POL_POS,        GIC_TRIG_LEVEL,     0 },                       /* USB H2 *//* Index 36*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                       /* SPI *//* Index 37*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                       /* Index 40 */
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,  GIC_POL_POS,        GIC_TRIG_LEVEL,     0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },	                 /* Index 47*/
};


interrupt_mapping_conf_t rtl9310_intr_mapping[] = {
    [0]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [1]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [2]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [3]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [4]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [5]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [6]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [7]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [8]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [9]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [10] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [11] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [12] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 6,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC6_IRQ },
    [13] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 5,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC5_IRQ },
    [14] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 4,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC4_IRQ },
    [15] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 3,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC3_IRQ },
    [16] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 2,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC2_IRQ },
    [17] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 36,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = USB_H2_IRQ },           /*Index = USB_H2_IRQ = 12*/
    [18] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 9,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = WDT_IP2_IRQ },
    [19] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 8,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = WDT_IP1_IRQ },
    [20] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT4, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 15,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = SWCORE_IRQ },
    [21] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = RTC_IRQ },
    [22] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = GPIO_EFGH_IRQ },
    [23] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 20,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = GPIO_ABCD_IRQ },
    [24] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 16,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = NIC_IRQ },
    [25] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = SLXTO_IRQ },
    [26] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = HLXTO_IRQ },
    [27] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = OCPTO_IRQ },
    [28] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 1,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC1_IRQ },
    [29] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT6, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 0,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC0_IRQ },
    [30] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 23,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = UART1_IRQ },
    [31] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT3, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 22,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = UART0_IRQ }             /*Index = UART0_IRQ = 31*/
};


interrupt_mapping_conf_t rtl9300_intr_mapping[] = {
    [0]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [1]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [2]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [3]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [4]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [5]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [6]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [7]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [8]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [9]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [10] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [11] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [12] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [13] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [14] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 11,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC4_IRQ },
    [15] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 10,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC3_IRQ },
    [16] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 9,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC2_IRQ },
    [17] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 28,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = USB_H2_IRQ },           /*Index = USB_H2_IRQ = 12*/
    [18] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = WDT_IP2_IRQ },
    [19] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 5,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = WDT_IP1_IRQ },
    [20] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT4, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 23,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = SWCORE_IRQ },
    [21] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = RTC_IRQ },
    [22] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = GPIO_EFGH_IRQ },
    [23] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 13,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = GPIO_ABCD_IRQ },
    [24] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 24,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = NIC_IRQ },
    [25] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = SLXTO_IRQ },
    [26] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = HLXTO_IRQ },
    [27] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 17,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = OCPTO_IRQ },
    [28] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 8,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC1_IRQ },
    [29] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT6, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 7,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC0_IRQ },
    [30] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 31,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = UART1_IRQ },
    [31] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT3, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 30,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = UART0_IRQ }             /*Index = UART0_IRQ = 31*/
};

interrupt_mapping_conf_t rtl839X8X_intr_mapping[] = {
    [0]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [1]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [2]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [3]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [4]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [5]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [6]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [7]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [8]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [9]  = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [10] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [11] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [12] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [13] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [14] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [15] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 15,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC4_IRQ },
    [16] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 16,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC3_IRQ },
    [17] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 17,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC2_IRQ },           /*Index = TC2_IRQ = 17*/
    [18] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = WDT_IP2_IRQ },
    [19] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 19,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = WDT_IP1_IRQ },
    [20] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT4, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 20,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = SWCORE_IRQ },
    [21] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = RTC_IRQ },
    [22] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = GPIO_EFGH_IRQ },
    [23] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 23,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = GPIO_ABCD_IRQ },
    [24] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT5, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 24,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = NIC_IRQ },
    [25] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 25,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = SLXTO_IRQ },
    [26] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 26,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = HLXTO_IRQ },
    [27] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 27,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = OCPTO_IRQ },
    [28] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 28,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC1_IRQ },
    [29] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT6, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 29,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC0_IRQ },
    [30] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 30,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = UART1_IRQ },
    [31] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT3, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 31,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = UART0_IRQ }             /*Index = UART0_IRQ = 31*/
};

DEFINE_SPINLOCK(irq_lock);

#define CONFIG_IRQ_FUNC_HOOK        1
#define HOOK_UNUSED                 0xFFFFFFFF


typedef struct interrupt_hook_ctrl_func_s
{
    void            (* hook_int_enable_func)(void);
    void            (* hook_int_disable_func)(void);
    unsigned int    cpu_ip;
} interrupt_hook_ctrl_func_t;

typedef struct interrupt_hook_dispatch_func_s
{
    int             (* hook_dispatcher_func)(void);
    unsigned int    system_irq;
    unsigned int    handle_pending;     /* 0:Not/1:Yes */

} interrupt_hook_dispatch_func_t;

interrupt_hook_ctrl_func_t hook_ctrl_func_t[BSP_SYS_IRQ_TOTAL_NUMBER];
interrupt_hook_dispatch_func_t hook_dispatch_func_t[MIPS_CPU_IP_NUMER];

extern unsigned int bsp_Interrupt_IRQ_Mapping_IP[ICTL_MAX_IP];
extern unsigned int bsp_Interrupt_srcID_Mapping_IRQ[GIC_NUM_INTRS];

/*
 * PIC 0 for Core0
 */
#define bsp_ictl_gic_shutdown_irq      bsp_ictl_gic_disable_irq
#define bsp_ictl_gic_mask_and_ack_irq  bsp_ictl_gic_disable_irq


#define bsp_ictl_shutdown_irq      bsp_ictl_disable_irq
#define bsp_ictl_mask_and_ack_irq  bsp_ictl_disable_irq


#define rtl8390_shutdown_irq      rtl8390_disable_irq
#define rtl8390_mask_and_ack_irq  rtl8390_disable_irq

static void rtl8390_enable_irq(struct irq_data *d);
static void rtl8390_disable_irq(struct irq_data *d);
static unsigned int rtl8390_startup_irq(struct irq_data *d);


#define rtl8380_shutdown_irq      rtl8380_disable_irq
#define rtl8380_mask_and_ack_irq  rtl8380_disable_irq

static void rtl8380_enable_irq(struct irq_data *d);
static void rtl8380_disable_irq(struct irq_data *d);
static unsigned int rtl8380_startup_irq(struct irq_data *d);


/* #### CHIP ####*/

void rtk_intr_func_init(void)
{
    unsigned long flags;
    unsigned int  loop_idx;

    spin_lock_irqsave(&irq_lock, flags);
    for(loop_idx = 0; loop_idx < BSP_SYS_IRQ_TOTAL_NUMBER; loop_idx++)
    {
        hook_ctrl_func_t[loop_idx].cpu_ip = HOOK_UNUSED;
        hook_ctrl_func_t[loop_idx].hook_int_disable_func = NULL;
        hook_ctrl_func_t[loop_idx].hook_int_enable_func = NULL;
    }

    for(loop_idx = 0; loop_idx < MIPS_CPU_IP_NUMER; loop_idx++)
    {
        hook_dispatch_func_t[loop_idx].hook_dispatcher_func = NULL;
        hook_dispatch_func_t[loop_idx].system_irq = HOOK_UNUSED;
        hook_dispatch_func_t[loop_idx].handle_pending = HOOK_UNUSED;
    }

    spin_unlock_irqrestore(&irq_lock, flags);
    return;
}


void rtk_intr_func_hook(unsigned int ip, unsigned int irq, unsigned int handle_pending, int (*dispatch)(void), void (*enable)(void), void (*disable)(void))
{
    unsigned long flags;

    spin_lock_irqsave(&irq_lock, flags);

    hook_ctrl_func_t[irq].cpu_ip = ip;
    hook_ctrl_func_t[irq].hook_int_disable_func = disable;
    hook_ctrl_func_t[irq].hook_int_enable_func = enable;

    hook_dispatch_func_t[ip].hook_dispatcher_func = dispatch;
    hook_dispatch_func_t[ip].system_irq = irq;
    hook_dispatch_func_t[ip].handle_pending = handle_pending;

    spin_unlock_irqrestore(&irq_lock, flags);
    return;
}

void rtk_intr_func_unhook(unsigned int ip, unsigned int irq)
{
    unsigned long flags;

    spin_lock_irqsave(&irq_lock, flags);

    hook_ctrl_func_t[irq].cpu_ip = HOOK_UNUSED;
    hook_ctrl_func_t[irq].hook_int_disable_func = NULL;
    hook_ctrl_func_t[irq].hook_int_enable_func = NULL;

    hook_dispatch_func_t[ip].hook_dispatcher_func = NULL;
    hook_dispatch_func_t[ip].system_irq = HOOK_UNUSED;
    hook_dispatch_func_t[ip].handle_pending = HOOK_UNUSED;

    spin_unlock_irqrestore(&irq_lock, flags);
    return;
}


static void bsp_ictl_gic_enable_irq(struct irq_data *d)
{
    unsigned long flags;

    spin_lock_irqsave(&irq_lock, flags);
    GIC_SET_INTR_MASK(bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(d->irq)]);
    spin_unlock_irqrestore(&irq_lock, flags);
}

static unsigned int bsp_ictl_gic_startup_irq(struct irq_data *d)
{
      bsp_ictl_gic_enable_irq(d);
      return 0;
}

static void bsp_ictl_gic_disable_irq(struct irq_data *d)
{
    unsigned long flags;

    spin_lock_irqsave(&irq_lock, flags);
    GIC_CLR_INTR_MASK(bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(d->irq)]);
    spin_unlock_irqrestore(&irq_lock, flags);
}

static struct irq_chip bsp_ictl_gic_irq = {
        .name = "RTL9310 ICTL",
        .irq_startup = bsp_ictl_gic_startup_irq,
        .irq_shutdown = bsp_ictl_gic_shutdown_irq,
        .irq_enable = bsp_ictl_gic_enable_irq,
        .irq_disable = bsp_ictl_gic_disable_irq,
        .irq_ack = bsp_ictl_gic_mask_and_ack_irq,
        .irq_mask = bsp_ictl_gic_mask_and_ack_irq,
        .irq_mask_ack = bsp_ictl_gic_mask_and_ack_irq,
        .irq_unmask = bsp_ictl_gic_enable_irq,
        .irq_eoi = bsp_ictl_gic_enable_irq,
};

static void bsp_ictl_enable_irq(struct irq_data *d)
{
    unsigned long flags;

    spin_lock_irqsave(&irq_lock, flags);
    REG32(GIMR) = REG32(GIMR) | (1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(d->irq)]);
#ifdef CONFIG_IRQ_FUNC_HOOK
    if(hook_ctrl_func_t[d->irq].hook_int_enable_func != NULL)
        hook_ctrl_func_t[d->irq].hook_int_enable_func();
#endif
    spin_unlock_irqrestore(&irq_lock, flags);
}

static unsigned int bsp_ictl_startup_irq(struct irq_data *d)
{
      bsp_ictl_enable_irq(d);
      return 0;
}

static void bsp_ictl_disable_irq(struct irq_data *d)
{
    unsigned long flags;

    spin_lock_irqsave(&irq_lock, flags);
    REG32(GIMR) = REG32(GIMR) & (~(1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(d->irq)]));
#ifdef CONFIG_IRQ_FUNC_HOOK
    if(hook_ctrl_func_t[d->irq].hook_int_disable_func != NULL)
         hook_ctrl_func_t[d->irq].hook_int_disable_func();
#endif
    spin_unlock_irqrestore(&irq_lock, flags);
}

static struct irq_chip bsp_ictl_irq = {
        .name = "RTL9300 ICTL",
        .irq_startup = bsp_ictl_startup_irq,
        .irq_shutdown = bsp_ictl_shutdown_irq,
        .irq_enable = bsp_ictl_enable_irq,
        .irq_disable = bsp_ictl_disable_irq,
        .irq_ack = bsp_ictl_mask_and_ack_irq,
        .irq_mask = bsp_ictl_mask_and_ack_irq,
        .irq_mask_ack = bsp_ictl_mask_and_ack_irq,
        .irq_unmask = bsp_ictl_enable_irq,
        .irq_eoi = bsp_ictl_enable_irq,
};

static void rtl8390_enable_irq(struct irq_data *d)
{
   unsigned long flags;
#ifndef CONFIG_IRQ_FUNC_HOOK
   unsigned long gpio_shared_ip;
#endif
   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR)  | (1 << ICTL_OFFSET(d->irq));
#ifdef CONFIG_IRQ_FUNC_HOOK
   if(hook_ctrl_func_t[d->irq].hook_int_enable_func != NULL)
       hook_ctrl_func_t[d->irq].hook_int_enable_func();
#endif
   spin_unlock_irqrestore(&irq_lock, flags);
}

static unsigned int rtl8390_startup_irq(struct irq_data *d)
{
   rtl8390_enable_irq(d);

   return 0;
}

static void rtl8390_disable_irq(struct irq_data *d)
{
   unsigned long flags;
#ifndef CONFIG_IRQ_FUNC_HOOK
   unsigned long gpio_shared_ip;
#endif
   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) & (~(1 << ICTL_OFFSET(d->irq)));
#ifdef CONFIG_IRQ_FUNC_HOOK
   if(hook_ctrl_func_t[d->irq].hook_int_disable_func != NULL)
        hook_ctrl_func_t[d->irq].hook_int_disable_func();
#endif

   spin_unlock_irqrestore(&irq_lock, flags);
}



static struct irq_chip bsp_ictl_irq_rtl8390 = {
        .name = "RTL8390 ICTL",
        .irq_startup = rtl8390_startup_irq,
        .irq_shutdown = rtl8390_shutdown_irq,
        .irq_enable = rtl8390_enable_irq,
        .irq_disable = rtl8390_disable_irq,
        .irq_ack = rtl8390_mask_and_ack_irq,
        .irq_mask = rtl8390_mask_and_ack_irq,
        .irq_mask_ack = rtl8390_mask_and_ack_irq,
        .irq_unmask = rtl8390_enable_irq,
        .irq_eoi = rtl8390_enable_irq,
};


static void rtl8380_enable_irq(struct irq_data *d)
{
   unsigned long flags;

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR)  | (1 << ICTL_OFFSET(d->irq));
#ifdef CONFIG_IRQ_FUNC_HOOK
   if(hook_ctrl_func_t[d->irq].hook_int_enable_func != NULL)
       hook_ctrl_func_t[d->irq].hook_int_enable_func();
#endif
   spin_unlock_irqrestore(&irq_lock, flags);
}

static unsigned int rtl8380_startup_irq(struct irq_data *d)
{
   rtl8380_enable_irq(d);

   return 0;
}

static void rtl8380_disable_irq(struct irq_data *d)
{
   unsigned long flags;

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) & (~(1 << ICTL_OFFSET(d->irq)));
#ifdef CONFIG_IRQ_FUNC_HOOK
   if(hook_ctrl_func_t[d->irq].hook_int_disable_func != NULL)
        hook_ctrl_func_t[d->irq].hook_int_disable_func();
#endif
   spin_unlock_irqrestore(&irq_lock, flags);
}



static struct irq_chip bsp_ictl_irq_rtl8380 = {
        .name = "RTL8380 ICTL",
        .irq_startup = rtl8380_startup_irq,
        .irq_shutdown = rtl8380_shutdown_irq,
        .irq_enable = rtl8380_enable_irq,
        .irq_disable = rtl8380_disable_irq,
        .irq_ack = rtl8380_mask_and_ack_irq,
        .irq_mask = rtl8380_mask_and_ack_irq,
        .irq_mask_ack = rtl8380_mask_and_ack_irq,
        .irq_unmask = rtl8380_enable_irq,
        .irq_eoi = rtl8380_enable_irq,
};


#if NR_CPUS >1
/*
 * PIC 1 for Core1
 */
#define bsp_ictl01_shutdown_irq     bsp_ictl01_disable_irq
#define bsp_ictl01_mask_and_ack_irq  bsp_ictl01_disable_irq
DEFINE_SPINLOCK(irq_lock01);
static void bsp_ictl01_enable_irq(struct irq_data *d)
{
        unsigned long flags;
    spin_lock_irqsave(&irq_lock01, flags);
    REG32(GIMR1) = REG32(GIMR1) | (1 << ICTL_OFFSET(d->irq));
        spin_unlock_irqrestore(&irq_lock01, flags);
}
static unsigned int bsp_ictl01_startup_irq(struct irq_data *d)
{
      bsp_ictl01_enable_irq(d);
      return 0;
}
static void bsp_ictl01_disable_irq(struct irq_data *d)
{
        unsigned long flags;
    spin_lock_irqsave(&irq_lock01, flags);
    REG32(GIMR1) = REG32(GIMR1) & (~(1 << ICTL_OFFSET(d->irq)));
        spin_unlock_irqrestore(&irq_lock01, flags);
}
static struct irq_chip bsp_ictl01_irq = {
        .name = "RTL9300 ICTL01",
        .irq_startup = bsp_ictl01_startup_irq,
        .irq_shutdown = bsp_ictl01_shutdown_irq,
        .irq_enable = bsp_ictl01_enable_irq,
        .irq_disable = bsp_ictl01_disable_irq,
        .irq_ack = bsp_ictl01_mask_and_ack_irq,
        .irq_mask = bsp_ictl01_mask_and_ack_irq,
        .irq_mask_ack = bsp_ictl01_mask_and_ack_irq,
        .irq_unmask = bsp_ictl01_enable_irq,
        .irq_eoi = bsp_ictl01_enable_irq,
};
#endif

/* IP2 IRQ handle map */
DECLARE_BITMAP(bsp_ints2, GIC_NUM_INTRS);
/* IP3 IRQ handle map */
DECLARE_BITMAP(bsp_ints3, GIC_NUM_INTRS);
/* IP4 IRQ handle map */
DECLARE_BITMAP(bsp_ints4, GIC_NUM_INTRS);
/* IP3 IRQ handle map */
DECLARE_BITMAP(bsp_ints5, GIC_NUM_INTRS);
/* IP4 IRQ handle map */
DECLARE_BITMAP(bsp_ints6, GIC_NUM_INTRS);

/* For Share IRQ*/
static void __init fill_bsp_int_map(void){

  /* IP6 */
  bitmap_set(bsp_ints6, 0, 1);

  /* IP5 */
  bitmap_set(bsp_ints5, 8, 2);
  bitmap_set(bsp_ints5, 16, 1);
  bitmap_set(bsp_ints5, 20, 1);

  /* IP4 */
  bitmap_set(bsp_ints4, 15, 1);

  /* IP3 */
  bitmap_set(bsp_ints3, 22, 1);

  /* IP2 */
  bitmap_set(bsp_ints2, 1, 1);
  bitmap_set(bsp_ints2, 2, 1);
  bitmap_set(bsp_ints2, 3, 1);
  bitmap_set(bsp_ints2, 4, 1);
  bitmap_set(bsp_ints2, 5, 1);
  bitmap_set(bsp_ints2, 6, 1);
  bitmap_set(bsp_ints2, 23, 1);
  bitmap_set(bsp_ints2, 36, 1);
}

static inline void bsp_get_gic_int_mask(unsigned long *map){
    unsigned long idx;
    unsigned int system_irq;
    DECLARE_BITMAP(pending, GIC_NUM_INTRS);
    gic_get_int_mask(pending, map);
    idx = find_first_bit(pending, GIC_NUM_INTRS);
    while (idx < GIC_NUM_INTRS) {
        system_irq = IRQ_CONV_ICTL(bsp_Interrupt_srcID_Mapping_IRQ[idx]);
        do_IRQ(system_irq);
        idx = find_next_bit(pending, GIC_NUM_INTRS, idx + 1);
    }
}


#ifdef CONFIG_SMP
#define BSP_IRQ_IPI_RESCHED		0	/* SW int 0 for resched */
#define BSP_IRQ_IPI_CALL		1	/* SW int 1 for call */
static irqreturn_t ipi_resched_interrupt(int irq, void *dev_id)
{
    /*From linux-3.x add this function */
    scheduler_ipi();
    return IRQ_HANDLED;
}

static irqreturn_t ipi_call_interrupt(int irq, void *dev_id)
{
    smp_call_function_interrupt();
    return IRQ_HANDLED;
}

static struct irqaction irq_resched = {
    .handler	= ipi_resched_interrupt,
    .flags		= IRQF_PERCPU,
    .name		= "IPI_resched"
};

static struct irqaction irq_call = {
    .handler	= ipi_call_interrupt,
    .flags		= IRQF_PERCPU,
    .name		= "IPI_call"
};
static void __init arch_init_ipiirq(int irq, struct irqaction *action)
{
    setup_irq(irq, action);
    irq_set_handler(irq, handle_percpu_irq);
}


#endif

#ifdef CONFIG_CPU_MIPSR2_IRQ_VI
/************************************
 * Vector0 & vector1 are for SW0/W1
************************************/
#ifdef CONFIG_SMP
void bsp_ictl_irq_dispatch_v0(void)
{
  do_IRQ(BSP_IRQ_IPI_RESCHED);
}

void bsp_ictl_irq_dispatch_v1(void)
{
  do_IRQ(BSP_IRQ_IPI_CALL);
}
#endif


static inline void bsp_irq_hook_func(unsigned int ip)
{
    if(hook_dispatch_func_t[ip].hook_dispatcher_func != NULL)
      if(hook_dispatch_func_t[ip].handle_pending == 0)
          if(hook_dispatch_func_t[ip].hook_dispatcher_func() == 1)
              do_IRQ(hook_dispatch_func_t[ip].system_irq);
}

/* Vector2 IPI */
void bsp_ictl_irq_dispatch_v2(void)
{
  /* For shared interrupts */
  unsigned int extint_ip = REG32(GIMR) & REG32(GISR);

#ifdef CONFIG_IRQ_FUNC_HOOK
  bsp_irq_hook_func(2);
#endif

  if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(BSP_TC1_EXT_IRQ)]))
  {
     do_IRQ(BSP_TC1_EXT_IRQ);
  }
  else if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(BSP_UART1_EXT_IRQ)]))
  {
     do_IRQ(BSP_UART1_EXT_IRQ);
  }
  else if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(BSP_USB_H2_IRQ)]))
  {
     do_IRQ(BSP_USB_H2_IRQ);
  }
  else if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(BSP_TC2_EXT_IRQ)]))
  {
     do_IRQ(BSP_TC2_EXT_IRQ);
  }
  else if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(BSP_TC3_EXT_IRQ)]))
  {
     do_IRQ(BSP_TC3_EXT_IRQ);
  }
  else if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(BSP_TC4_EXT_IRQ)]))
  {
     do_IRQ(BSP_TC4_EXT_IRQ);
  }

#ifdef CONFIG_IRQ_FUNC_HOOK
  bsp_irq_hook_func(2);
#endif
}
/* Vector3 UART0 */
void bsp_ictl_irq_dispatch_v3(void)
{
#ifdef CONFIG_IRQ_FUNC_HOOK
  bsp_irq_hook_func(3);
#endif
  //spurious_interrupt();
  do_IRQ(BSP_UART0_EXT_IRQ);
#ifdef CONFIG_IRQ_FUNC_HOOK
  bsp_irq_hook_func(3);
#endif
}
/* Vector4 */
/* Uninstall */
void bsp_ictl_irq_dispatch_v4(void)
{
#ifdef CONFIG_IRQ_FUNC_HOOK
  bsp_irq_hook_func(4);
#endif
//  spurious_interrupt();
  /* SWCORE */
  do_IRQ(BSP_SWCORE_EXT_IRQ);
#ifdef CONFIG_IRQ_FUNC_HOOK
  bsp_irq_hook_func(4);
#endif

}
/* Vector5 */
/* Uninstall */
void bsp_ictl_irq_dispatch_v5(void)
{
  //spurious_interrupt();
  /* For shared interrupts */
  unsigned int extint_ip = REG32(GIMR) & REG32(GISR);

#ifdef CONFIG_IRQ_FUNC_HOOK
  bsp_irq_hook_func(5);
#endif

  if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(BSP_NIC_EXT_IRQ)]))
  {
      /* NIC */
      do_IRQ(BSP_NIC_EXT_IRQ);
  }
  else if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(BSP_GPIO_ABCD_EXT_IRQ)]))
  {
#if 0
#ifndef CONFIG_IRQ_FUNC_HOOK /* For Netgear Device*/
      gpio_shared_ip = REG32(PABC_ISR);
      if((gpio_shared_ip & MAX3421_IP) != 0)
      {
          do_IRQ(BSP_MAX3421_USB_IRQ);
      }
#endif
#endif
      /* GPIO ABCD */
      do_IRQ(BSP_GPIO_ABCD_EXT_IRQ);
  }

#ifdef CONFIG_IRQ_FUNC_HOOK
  bsp_irq_hook_func(5);
#endif



}
/* Vector6 */
/* Uninstall */
void bsp_ictl_irq_dispatch_v6(void)
{
  //spurious_interrupt();
#ifdef CONFIG_IRQ_FUNC_HOOK
  bsp_irq_hook_func(6);
#endif
  /* Timer 0 */
  do_IRQ(BSP_TC0_EXT_IRQ);
#ifdef CONFIG_IRQ_FUNC_HOOK
  bsp_irq_hook_func(6);
#endif

}


void bsp_ictl_gic_irq_dispatch_v2(void)
{
  bsp_get_gic_int_mask(bsp_ints2);
}
/* Vector3 UART0 */
void bsp_ictl_gic_irq_dispatch_v3(void)
{
  //spurious_interrupt();
  do_IRQ(BSP_UART0_EXT_IRQ);
}
/* Vector4 */
/* Uninstall */
void bsp_ictl_gic_irq_dispatch_v4(void)
{
//  spurious_interrupt();
  /* SWCORE */
  do_IRQ(BSP_SWCORE_EXT_IRQ);
}
/* Vector5 */
/* Uninstall */
void bsp_ictl_gic_irq_dispatch_v5(void)
{
  bsp_get_gic_int_mask(bsp_ints5);
}
/* Vector6 */
/* Uninstall */
void bsp_ictl_gic_irq_dispatch_v6(void)
{
  //spurious_interrupt();
  /* Timer 0 */
  do_IRQ(BSP_TC0_EXT_IRQ);
}


/* Vector7 for Timer */
#ifdef CONFIG_ARCH_CEVT_EXT
void bsp_ictl_irq_dispatch_timer(void)
{
#ifdef CONFIG_SMP
  int cpu = smp_processor_id();
  do_IRQ(BSP_TC0_EXT_IRQ + cpu );
#else
  do_IRQ(BSP_TC0_EXT_IRQ);
#endif
}
#endif

static void __init  bsp_setup_vi(void){

    /* CPU_MIPSR2_IRQ_VI only support 8 vectored interrupt corresponded  with IP0 ~ IP7
     * Vector 0 for IP0(SW0)
     * Vector 1 for IP1(SW1)
     */
#ifdef CONFIG_SMP
    /* This is for LINUX SMP Inter-Processor Interrupts
     * Don't change this section, if you don't sure.
     */
    set_vi_handler(0, bsp_ictl_irq_dispatch_v0);
    set_vi_handler(1, bsp_ictl_irq_dispatch_v1);
#endif
    if(bsp_chip_family_id != RTL9310_FAMILY_ID)
    {
        set_vi_handler(2, bsp_ictl_irq_dispatch_v2);            /*IP2,  UART */
        set_vi_handler(3, bsp_ictl_irq_dispatch_v3);            /*IP3,  UART */
        set_vi_handler(4, bsp_ictl_irq_dispatch_v4);            /*IP4,  Uninstall */
        set_vi_handler(5, bsp_ictl_irq_dispatch_v5);            /*IP5,  Uninstall */
        set_vi_handler(6, bsp_ictl_irq_dispatch_v6);            /*IP6,  Uninstall */
     }else{
        set_vi_handler(2, bsp_ictl_gic_irq_dispatch_v2);            /*IP2,  UART */
        set_vi_handler(3, bsp_ictl_gic_irq_dispatch_v3);            /*IP3,  UART */
        set_vi_handler(4, bsp_ictl_gic_irq_dispatch_v4);            /*IP4,  Uninstall */
        set_vi_handler(5, bsp_ictl_gic_irq_dispatch_v5);            /*IP5,  Uninstall */
        set_vi_handler(6, bsp_ictl_gic_irq_dispatch_v6);            /*IP6,  Uninstall */
     }

#ifdef CONFIG_ARCH_CEVT_EXT
//	set_vi_handler(7, bsp_ictl_irq_dispatch_timer);         /*IP7,  IPI, Vector 7 *//*Mask by Daniel*/
#endif


}
#else  /* Not define CONFIG_CPU_MIPSR2_IRQ_VI */
void bsp_irq_dispatch(void)
{
    unsigned int pending;
    pending = read_c0_cause() & read_c0_status() & ST0_IM;

    if (pending & CAUSEF_IP6)
        do_IRQ(6);
    else if (pending & CAUSEF_IP5)
        do_IRQ(5);
    else if (pending & CAUSEF_IP4)
        do_IRQ(4);
    else if (pending & CAUSEF_IP3)
        do_IRQ(3);
    else if (pending & CAUSEF_IP2)
        do_IRQ(2);	else
        spurious_interrupt();

}
#endif

static inline void __init bsp_ictl00_irq_set(int irq){

       irq_set_chip(irq, &bsp_ictl_irq);
       irq_set_handler(irq, handle_level_irq);
}

static inline void __init bsp_ictl01_irq_set(int irq){
#if NR_CPUS > 1
       irq_set_chip(irq, &bsp_ictl01_irq);
       irq_set_handler(irq, handle_level_irq);
#endif
}

static inline void __init bsp_ictl00_gic_irq_set(int irq){

       irq_set_chip(irq, &bsp_ictl_gic_irq);
       irq_set_handler(irq, handle_level_irq);
}

static void __init bsp_irq_init(void)
{
   int i;
   /* Initialize for IRQ: 0~31 */
#ifdef CONFIG_ARCH_CEVT_EXT
  if((bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID))
  {
      bsp_ictl00_irq_set(BSP_TC0_EXT_IRQ); //For CPU0
  }
  if((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
  {
      bsp_ictl00_irq_set(BSP_TC0_EXT_IRQ); //For CPU0
  }
  if(bsp_chip_family_id == RTL9300_FAMILY_ID)
  {
      bsp_ictl00_irq_set(BSP_TC0_EXT_IRQ); //For CPU0
  }
  if(bsp_chip_family_id == RTL9300_FAMILY_ID)
  {
      bsp_ictl01_irq_set(BSP_TC1_EXT_IRQ); //For CPU1
  }
  if(bsp_chip_family_id == RTL9310_FAMILY_ID)
  {
      bsp_ictl00_gic_irq_set(BSP_TC0_EXT_IRQ); //For CPU0
  }
#endif
   if((bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID))
   {
       for (i = (BSP_IRQ_ICTL_BASE); i < (BSP_IRQ_ICTL_BASE + UART0_IRQ + 1); i++) {
           irq_set_chip(i, &bsp_ictl_irq_rtl8390);
           irq_set_handler(i, handle_level_irq);
       }
   }
   if((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
   {
       for (i = (BSP_IRQ_ICTL_BASE); i < (BSP_IRQ_ICTL_BASE + UART0_IRQ + 1); i++) {
           irq_set_chip(i, &bsp_ictl_irq_rtl8380);
           irq_set_handler(i, handle_level_irq);
       }
   }
   if(bsp_chip_family_id == RTL9300_FAMILY_ID)
   {
       for (i = (BSP_IRQ_ICTL_BASE); i < (BSP_IRQ_ICTL_BASE + UART0_IRQ + 1); i++) {
           irq_set_chip(i, &bsp_ictl_irq);
           irq_set_handler(i, handle_level_irq);
       }
   }
   if(bsp_chip_family_id == RTL9310_FAMILY_ID)
   {
       for (i = (BSP_IRQ_ICTL_BASE); i < (BSP_IRQ_ICTL_BASE + UART0_IRQ + 1); i++) {
           irq_set_chip(i, &bsp_ictl_gic_irq);
           irq_set_handler(i, handle_level_irq);
       }
   }
#ifdef CONFIG_ARCH_CEVT_EXT
   /* Disable internal Count register */
   write_c0_cause(read_c0_cause() | (1 << 27));
   /* Clear internal timer interrupt */
   write_c0_compare(0);
#endif
   /* Enable all interrupt mask of CPU */
    change_c0_status(ST0_IM,  STATUSF_IP0| STATUSF_IP1| STATUSF_IP2 | STATUSF_IP3 |  STATUSF_IP4 |
                STATUSF_IP5 | STATUSF_IP6 | STATUSF_IP7 );

   if(bsp_chip_family_id == RTL9300_FAMILY_ID)
   {
        /* Set GIMR, IRR */
        /* Timer for Global Interrupt Mask Register*/
        REG32(GIMR) = UART0_IE_RTL9300 | TC0_IE_RTL9300;

        REG32(IRR0) = IRR0_RTL9300_SETTING;  /* UART 0 */
        REG32(IRR1) = IRR1_RTL9300_SETTING;
        REG32(IRR2) = IRR2_RTL9300_SETTING;
        REG32(IRR3) = IRR3_RTL9300_SETTING;
    }else if(bsp_chip_family_id != RTL9310_FAMILY_ID){
        /* Set GIMR, IRR *//* For RTL8390/80*/
        REG32(GIMR) = TC0_IE | UART0_IE;

        REG32(IRR0) = IRR0_SETTING;
        REG32(IRR1) = IRR1_SETTING;
        REG32(IRR2) = IRR2_SETTING;
        REG32(IRR3) = IRR3_SETTING;
    }
#ifdef CONFIG_SMP
    arch_init_ipiirq(BSP_IRQ_IPI_RESCHED, &irq_resched);
    arch_init_ipiirq(BSP_IRQ_IPI_CALL, &irq_call);
#endif

}
static void plat_gic_init(void)
{
    fill_bsp_int_map();

    if (mips_cm_present())
        write_gcr_gic_base(GIC_BASE_ADDR | CM_GCR_GIC_BASE_GICEN_MSK);

    gic_init(GIC_BASE_ADDR, GIC_BASE_SIZE, rtl9310_gic_intr_map);

    change_c0_status(ST0_IM, 0xff00);
}


void __init plat_irq_init(void)
{
    /*For IP0, IP1 */
    mips_cpu_irq_init();
#ifdef CONFIG_CPU_MIPSR2_IRQ_VI
    bsp_setup_vi();
#endif
    if(bsp_chip_family_id == RTL9310_FAMILY_ID)
    {
        plat_gic_init();
    }
    /* initialize IRQ action handlers */
    bsp_irq_init();
}

EXPORT_SYMBOL(rtk_intr_func_hook);
EXPORT_SYMBOL(rtk_intr_func_unhook);


