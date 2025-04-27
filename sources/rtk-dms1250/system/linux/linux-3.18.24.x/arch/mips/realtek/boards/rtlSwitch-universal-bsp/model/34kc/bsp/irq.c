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
#include "bsp_ioal.h"

/* GIC is transparent, just bridge*/
struct gic_intr_map rtl9310_gic_intr_map[GIC_NUM_INTRS] = {
    /* EXT_INT 0~7 */
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		GIC_FLAG_TRANSPARENT },   /* Timer 0 *//*Index 0*/
    { 0, GIC_CPU_INT5,  GIC_POL_POS,        GIC_TRIG_LEVEL,     GIC_FLAG_TRANSPARENT },   /* Timer 1 *//*Index 1*/
    { 0, GIC_CPU_INT5,  GIC_POL_POS,        GIC_TRIG_LEVEL,     GIC_FLAG_TRANSPARENT },   /* Timer 2 *//*Index 2*/
    { 0, GIC_CPU_INT5,  GIC_POL_POS,        GIC_TRIG_LEVEL,     GIC_FLAG_TRANSPARENT },   /* Timer 3 *//*Index 3*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 4 *//*Index 4*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 5 *//*Index 5*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 6 *//*Index 6*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT2,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* WDT_IP1_IRQ *//*Index 8*/
    { 0, GIC_CPU_INT2,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* WDT_IP2_IRQ *//*Index 9*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /*Index 10*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT2,	GIC_POL_POS,		GIC_TRIG_LEVEL,		GIC_FLAG_TRANSPARENT },	  /* Switch Core *//* Index 15 */
    { 0, GIC_CPU_INT2,	GIC_POL_POS,		GIC_TRIG_LEVEL,		GIC_FLAG_TRANSPARENT },	  /* NIC *//* Index 16*/
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT1,  GIC_POL_POS,        GIC_TRIG_LEVEL,     GIC_FLAG_TRANSPARENT },   /* GPIO *//* Index 20 */
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT1,	GIC_POL_POS,		GIC_TRIG_LEVEL,		GIC_FLAG_TRANSPARENT },	  /* UART0 *//*Index 22*/
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* UART1 *//*Index 23*/
    { 0, GIC_CPU_INT2,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* NIC   *//*Index 24*/
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,  GIC_POL_POS,        GIC_TRIG_LEVEL,     0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Index 30 */
    { 0, GIC_CPU_INT0,  GIC_POL_POS,        GIC_TRIG_LEVEL,     0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT1,  GIC_POL_POS,        GIC_TRIG_LEVEL,     0 },                       /* USB H2 *//* Index 36*/
    { 0, GIC_CPU_INT1,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                       /* SPI *//* Index 37*/
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                       /* Index 40 */
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,  GIC_POL_POS,        GIC_TRIG_LEVEL,     0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT0,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },	                 /* Index 47*/
};


cpu_timer_source_mapping_conf_t rtl9310_irq_sysTick_mapping[NR_CPUS] = {
    { CPU_TIMER0, CPU_INT7, INTR_SRC_IDX_0, INTR_TYPE_DEFAULT, INTR_DEFAULT_DISABLE,  TC0_IRQ, GIC_CPU_INT5, GIC_FLAG_TRANSPARENT},   /* CPU0 Timer source */
#ifdef CONFIG_SMP
    { CPU_TIMER1, CPU_INT7, INTR_SRC_IDX_1, INTR_TYPE_DEFAULT, INTR_DEFAULT_DISABLE,  TC1_IRQ, GIC_CPU_INT5, GIC_FLAG_TRANSPARENT},   /* CPU1 Timer source */
    { CPU_TIMER2, CPU_INT7, INTR_SRC_IDX_2, INTR_TYPE_DEFAULT, INTR_DEFAULT_DISABLE,  TC2_IRQ, GIC_CPU_INT5, GIC_FLAG_TRANSPARENT},   /* CPU2 Timer source */
    { CPU_TIMER3, CPU_INT7, INTR_SRC_IDX_3, INTR_TYPE_DEFAULT, INTR_DEFAULT_DISABLE,  TC3_IRQ, GIC_CPU_INT5, GIC_FLAG_TRANSPARENT},   /* CPU3 Timer source */
#endif
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
    [12] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT3, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 36,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = USB_H2_IRQ},
    [13] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT7, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 6,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC6_IRQ },
    [14] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT7, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 5,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC5_IRQ },
    [15] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT7, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 4,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC4_IRQ },
    [16] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT7, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 3,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC3_IRQ },
    [17] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT7, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 2,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC2_IRQ },           /*Index = TC2_IRQ = 17*/
    [18] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT4, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 9,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = WDT_IP2_IRQ },
    [19] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT4, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 8,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = WDT_IP1_IRQ },
    [20] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT4, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 15,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = SWCORE_IRQ },
    [21] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT3, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = RTC_IRQ },
    [22] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT3, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = GPIO_EFGH_IRQ },
    [23] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT3, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 20,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = GPIO_ABCD_IRQ },
    [24] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT4, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 16,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = NIC_IRQ },
    [25] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = SLXTO_IRQ },
    [26] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = HLXTO_IRQ },
    [27] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = OCPTO_IRQ },
    [28] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT7, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 1,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC1_IRQ },
    [29] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT7, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 0,                  .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = TC0_IRQ },
    [30] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 23,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = UART1_IRQ },
    [31] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT3, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 22,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = UART0_IRQ }             /*Index = UART0_IRQ = 31*/
};

cpu_timer_source_mapping_conf_t rtl9300_irq_sysTick_mapping[1] = {
    { CPU_TIMER0, CPU_INT6, INTR_SRC_IDX_7 , INTR_TYPE_DEFAULT, INTR_DEFAULT_DISABLE,  TC0_IRQ, GIC_UNUSED, GIC_UNUSED},   /* CPU0 Timer source */
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

cpu_timer_source_mapping_conf_t rtl839X8X_irq_sysTick_mapping[1] = {
    { CPU_TIMER0, CPU_INT6, INTR_SRC_IDX_29 , INTR_TYPE_DEFAULT, INTR_DEFAULT_DISABLE,  TC0_IRQ, GIC_UNUSED, GIC_UNUSED},   /* CPU0 Timer source */
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

cpu_timerId_irq_t bsp_cpu_timerId_irq[NR_CPUS];


void _bsp_irq_sysTick_mapping_setup(void)
{
    int cpu_idx = 0;
    int irq_idx = 0;
    int gic_src_idx = 0;

    if((bsp_chip_family_id == RTL8390_FAMILY_ID)||(bsp_chip_family_id == RTL8350_FAMILY_ID)||(bsp_chip_family_id == RTL8380_FAMILY_ID)||(bsp_chip_family_id == RTL8330_FAMILY_ID))
    {
        irq_idx = rtl839X8X_irq_sysTick_mapping[cpu_idx].mapped_system_irq;
        rtl839X8X_intr_mapping[irq_idx].target_cpu_id = cpu_idx;
        rtl839X8X_intr_mapping[irq_idx].cpu_ip_id = rtl839X8X_irq_sysTick_mapping[cpu_idx].cpu_ip_id;
        rtl839X8X_intr_mapping[irq_idx].intr_type = rtl839X8X_irq_sysTick_mapping[cpu_idx].intr_type;
        rtl839X8X_intr_mapping[irq_idx].intr_src_id = rtl839X8X_irq_sysTick_mapping[cpu_idx].intr_src_id;
        rtl839X8X_intr_mapping[irq_idx].intr_defalut = rtl839X8X_irq_sysTick_mapping[cpu_idx].intr_default;
        rtl839X8X_intr_mapping[irq_idx].mapped_system_irq = rtl839X8X_irq_sysTick_mapping[cpu_idx].mapped_system_irq;

        bsp_cpu_timerId_irq[cpu_idx].mapped_system_irq = rtl839X8X_irq_sysTick_mapping[cpu_idx].mapped_system_irq;
        bsp_cpu_timerId_irq[cpu_idx].timer_id = rtl839X8X_irq_sysTick_mapping[cpu_idx].timer_id;
    }
    else if((bsp_chip_family_id == RTL9310_FAMILY_ID))
    {
        for(cpu_idx = 0; cpu_idx < (NR_CPUS); cpu_idx++)
        {
            irq_idx = rtl9310_irq_sysTick_mapping[cpu_idx].mapped_system_irq;
            rtl9310_intr_mapping[irq_idx].target_cpu_id = cpu_idx;
            rtl9310_intr_mapping[irq_idx].cpu_ip_id = rtl9310_irq_sysTick_mapping[cpu_idx].cpu_ip_id;
            rtl9310_intr_mapping[irq_idx].intr_type = rtl9310_irq_sysTick_mapping[cpu_idx].intr_type;
            rtl9310_intr_mapping[irq_idx].intr_src_id = rtl9310_irq_sysTick_mapping[cpu_idx].intr_src_id;
            rtl9310_intr_mapping[irq_idx].intr_defalut = rtl9310_irq_sysTick_mapping[cpu_idx].intr_default;
            rtl9310_intr_mapping[irq_idx].mapped_system_irq = rtl9310_irq_sysTick_mapping[cpu_idx].mapped_system_irq;
            /* Mapping to GIC table*/
            gic_src_idx = rtl9310_irq_sysTick_mapping[cpu_idx].intr_src_id;
            rtl9310_gic_intr_map[gic_src_idx].cpunum = cpu_idx;
            rtl9310_gic_intr_map[gic_src_idx].pin = rtl9310_irq_sysTick_mapping[cpu_idx].gic_cpu_ip;
            rtl9310_gic_intr_map[gic_src_idx].flags = rtl9310_irq_sysTick_mapping[cpu_idx].intr_default;

            bsp_cpu_timerId_irq[cpu_idx].mapped_system_irq = rtl9310_irq_sysTick_mapping[cpu_idx].mapped_system_irq;
            bsp_cpu_timerId_irq[cpu_idx].timer_id = rtl9310_irq_sysTick_mapping[cpu_idx].timer_id;
        }
    }
    else if((bsp_chip_family_id == RTL9300_FAMILY_ID))
    {
        irq_idx = rtl9300_irq_sysTick_mapping[cpu_idx].mapped_system_irq;
        rtl9300_intr_mapping[irq_idx].target_cpu_id = cpu_idx;
        rtl9300_intr_mapping[irq_idx].cpu_ip_id = rtl9300_irq_sysTick_mapping[cpu_idx].cpu_ip_id;
        rtl9300_intr_mapping[irq_idx].intr_type = rtl9300_irq_sysTick_mapping[cpu_idx].intr_type;
        rtl9300_intr_mapping[irq_idx].intr_src_id = rtl9300_irq_sysTick_mapping[cpu_idx].intr_src_id;
        rtl9300_intr_mapping[irq_idx].intr_defalut = rtl9300_irq_sysTick_mapping[cpu_idx].intr_default;
        rtl9300_intr_mapping[irq_idx].mapped_system_irq = rtl9300_irq_sysTick_mapping[cpu_idx].mapped_system_irq;

        bsp_cpu_timerId_irq[cpu_idx].mapped_system_irq = rtl9300_irq_sysTick_mapping[cpu_idx].mapped_system_irq;
        bsp_cpu_timerId_irq[cpu_idx].timer_id = rtl9300_irq_sysTick_mapping[cpu_idx].timer_id;
    }else{
        printk("\n Interrupt Mapping failed !!! \n");
    }
    return;
}

void bsp_interrupt_irq_mapping_setup(void)
{
    int	index;
    int	irq_index;
    unsigned int	src_index;
    int system_irq;
    int int_type;
    interrupt_mapping_conf_t *intr_mapping_table;
    struct gic_intr_map *gic_mapping_table = NULL;

    rtk_intr_func_init();

    memset(bsp_cpu_timerId_irq, 0, (sizeof(cpu_timerId_irq_t)*NR_CPUS));
    _bsp_irq_sysTick_mapping_setup();

    if((bsp_chip_family_id == RTL8390_FAMILY_ID)||(bsp_chip_family_id == RTL8350_FAMILY_ID)||(bsp_chip_family_id == RTL8380_FAMILY_ID)||(bsp_chip_family_id == RTL8330_FAMILY_ID))
    {
        intr_mapping_table = rtl839X8X_intr_mapping;
    }
    else if((bsp_chip_family_id == RTL9300_FAMILY_ID))
    {
        intr_mapping_table = rtl9300_intr_mapping;
    }
    else if((bsp_chip_family_id == RTL9310_FAMILY_ID))
    {
        intr_mapping_table = rtl9310_intr_mapping;
        gic_mapping_table = rtl9310_gic_intr_map;
    }else{
        printk("\n Interrupt Mapping failed !!! \n");
        return;
    }

    for(index = 0; index < ICTL_MAX_IP; index++)
    {
        src_index = (unsigned int)((interrupt_mapping_conf_t *)(intr_mapping_table + index)->intr_src_id);
        system_irq = (unsigned int)((interrupt_mapping_conf_t *)(intr_mapping_table + index)->mapped_system_irq);
        bsp_Interrupt_IRQ_Mapping_IP[system_irq] = src_index;
        if((bsp_chip_family_id == RTL9310_FAMILY_ID) && (src_index < GIC_NUM_INTRS) && (gic_mapping_table != NULL))
        {
            ((struct gic_intr_map *)(gic_mapping_table + src_index))->cpunum = (unsigned int)((interrupt_mapping_conf_t *)(intr_mapping_table + index)->target_cpu_id);
            ((struct gic_intr_map *)(gic_mapping_table + src_index))->pin = (unsigned int)((unsigned int)((interrupt_mapping_conf_t *)(intr_mapping_table + index)->cpu_ip_id) - 2);
            int_type = (int)((interrupt_mapping_conf_t *)(intr_mapping_table + index)->intr_type);
            if(((interrupt_mapping_conf_t *)(intr_mapping_table + index)->intr_type) != INTR_TYPE_DEFAULT)
            {
                switch(int_type)
                {
                    case INTR_TYPE_HIGH_LEVEL:
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->polarity = GIC_POL_POS;
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->trigtype = GIC_TRIG_LEVEL;
                        break;
                    case INTR_TYPE_LOW_LEVEL:
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->polarity = GIC_POL_NEG;
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->trigtype = GIC_TRIG_LEVEL;
                        break;
                    case INTR_TYPE_FALLING_EDGE:
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->polarity = GIC_POL_NEG;
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->trigtype = GIC_TRIG_EDGE;
                        break;
                    case INTR_TYPE_RISING_EDGE:
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->polarity = GIC_POL_POS;
                        ((struct gic_intr_map *)(gic_mapping_table + src_index))->trigtype = GIC_TRIG_EDGE;
                        break;
                     default:
                        break;
                }
            }
        }
    }

    if((bsp_chip_family_id == RTL9310_FAMILY_ID))
    {
        for(src_index = 0; src_index < GIC_NUM_INTRS; src_index++)
        {
            for(irq_index = 0; irq_index < ICTL_MAX_IP; irq_index++){
                if(bsp_Interrupt_IRQ_Mapping_IP[irq_index] == src_index) /*interrupt src index*/
                    bsp_Interrupt_srcID_Mapping_IRQ[src_index] = irq_index;
            }
        }
    }
}







#ifdef CONFIG_SMP
DECLARE_BITMAP(ipi_ints, GIC_NUM_INTRS);
static inline void bsp_ipi_irqdispatch(void)
{
	unsigned long irq;
	DECLARE_BITMAP(pending, GIC_NUM_INTRS);

	gic_get_int_mask(pending, ipi_ints);

	irq = find_first_bit(pending, GIC_NUM_INTRS);

	while (irq < GIC_NUM_INTRS) {
		do_IRQ(MIPS_GIC_IRQ_BASE + irq);

		irq = find_next_bit(pending, GIC_NUM_INTRS, irq + 1);
	}

}

static void __init fill_ipi_map1(int baseintr, int cpu, int cpupin)
{
	int intr = baseintr + cpu;
	rtl9310_gic_intr_map[intr].cpunum = cpu;
	rtl9310_gic_intr_map[intr].pin = cpupin;
	rtl9310_gic_intr_map[intr].polarity = GIC_POL_POS;
	rtl9310_gic_intr_map[intr].trigtype = GIC_TRIG_EDGE;
	rtl9310_gic_intr_map[intr].flags = 0;
        //ipi_map[cpu] |= (1 << (cpupin + 2));
	bitmap_set(ipi_ints, intr, 1);
}

static void __init fill_ipi_map(void)
{
	int cpu;
	for (cpu = 0; cpu < nr_cpu_ids; cpu++) {
		fill_ipi_map1(GIC_IPI_RESCHED_BASE, cpu, GIC_CPU_INT3);
		fill_ipi_map1(GIC_IPI_CALL_BASE, cpu, GIC_CPU_INT4);
	}
}

void __init irq_validity_check(void) {
	int i;
	struct gic_intr_map *gic;
	for (i=0; i<ARRAY_SIZE(rtl9310_gic_intr_map); i++) {
		gic = &rtl9310_gic_intr_map[i];
		if (GIC_UNUSED == gic->cpunum)
			continue;
		if ((gic->cpunum+1) > CONFIG_NR_CPUS) {
			gic->cpunum = GIC_UNUSED;
			printk("Correct IRQ entries %d\n", i);
		}
	}
}
#endif

/******************************************************
 * GIC disable routing local timer and perf intterrupt
 * ****************************************************/
void __init vpe_local_disable(void)
{
	unsigned int vpe_ctl;

	/* Are Interrupts locally routable? */
	GICREAD(GIC_REG(VPE_LOCAL, GIC_VPE_CTL), vpe_ctl);
	if (vpe_ctl & GIC_VPE_CTL_TIMER_RTBL_MSK)
			GICWRITE(GIC_REG(VPE_LOCAL, GIC_VPE_TIMER_MAP), 0);

	if (vpe_ctl & GIC_VPE_CTL_PERFCNT_RTBL_MSK)
			GICWRITE(GIC_REG(VPE_LOCAL, GIC_VPE_PERFCTR_MAP), 0);

}


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
    BSP_REG32(GIMR) = BSP_REG32(GIMR) | (1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(d->irq)]);
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
    BSP_REG32(GIMR) = BSP_REG32(GIMR) & (~(1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(d->irq)]));
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
   BSP_REG32(GIMR) = BSP_REG32(GIMR)  | (1 << ICTL_OFFSET(d->irq));
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
   BSP_REG32(GIMR) = BSP_REG32(GIMR) & (~(1 << ICTL_OFFSET(d->irq)));
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
   BSP_REG32(GIMR) = BSP_REG32(GIMR)  | (1 << ICTL_OFFSET(d->irq));
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
   BSP_REG32(GIMR) = BSP_REG32(GIMR) & (~(1 << ICTL_OFFSET(d->irq)));
#ifdef CONFIG_IRQ_FUNC_HOOK
   if(hook_ctrl_func_t[d->irq].hook_int_disable_func != NULL)
        hook_ctrl_func_t[d->irq].hook_int_disable_func();
#endif
   spin_unlock_irqrestore(&irq_lock, flags);
}

static void rtl8380_wdt_phase1(void)
{
    uint32 tmp = 0;

    BSP_REG32(0xb8003154) = 0x80000000; /*WDT PH1 IP clear*/
    OTTO838x_FLASH_DISABLE_4BYTE_ADDR_MODE();
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

/* IP2 IRQ handle map */
DECLARE_BITMAP(bsp_ints2, GIC_NUM_INTRS);
/* IP3 IRQ handle map */
DECLARE_BITMAP(bsp_ints3, GIC_NUM_INTRS);
/* IP4 IRQ handle map */
DECLARE_BITMAP(bsp_ints4, GIC_NUM_INTRS);
/* IP5 IRQ handle map */
DECLARE_BITMAP(bsp_ints5, GIC_NUM_INTRS);
/* IP6 IRQ handle map */
DECLARE_BITMAP(bsp_ints6, GIC_NUM_INTRS);
/* IP7 IRQ handle map */
DECLARE_BITMAP(bsp_ints7, GIC_NUM_INTRS);

/* For Share IRQ*/
static void __init fill_bsp_int_map(void){

  /* IP7 */
  bitmap_set(bsp_ints7, 0, 1);  /* TC0 */
  bitmap_set(bsp_ints7, 1, 1);  /* TC1 */
  bitmap_set(bsp_ints7, 2, 1);  /* TC2 */
  bitmap_set(bsp_ints7, 3, 1);  /* TC3 */
  bitmap_set(bsp_ints7, 4, 1);  /* TC4 */
  bitmap_set(bsp_ints7, 5, 1);  /* TC5 */
  bitmap_set(bsp_ints7, 6, 1);  /* TC6 */

  /* IP4 */
  bitmap_set(bsp_ints4, 16, 1); /* NIC */
  bitmap_set(bsp_ints4, 15, 1); /* SWCORE */
  bitmap_set(bsp_ints4, 8, 1);  /* WDT_IP1 */
  bitmap_set(bsp_ints4, 9, 1);  /* WDT_IP2 */

  /* IP3 */
  bitmap_set(bsp_ints3, 22, 1); /* UART0 */
  bitmap_set(bsp_ints3, 20, 1); /* GPIO */
  bitmap_set(bsp_ints3, 36, 1); /* USB H2 */
  bitmap_set(bsp_ints3, 37, 1); /* SPI */

  /* IP2 */
  bitmap_set(bsp_ints2, 23, 1); /* UART1 */
  bitmap_set(bsp_ints2, 34, 1); /* OCPTO */
  bitmap_set(bsp_ints2, 31, 1); /* HLXTO */
  bitmap_set(bsp_ints2, 32, 1); /* SLXTO */
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
  unsigned int extint_ip = BSP_REG32(GIMR) & BSP_REG32(GISR);

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
  unsigned int extint_ip = BSP_REG32(GIMR) & BSP_REG32(GISR);

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
      gpio_shared_ip = BSP_REG32(PABC_ISR);
      if((gpio_shared_ip & MAX3421_IP) != 0)
      {
          do_IRQ(BSP_MAX3421_USB_IRQ);
      }
#endif
#endif
      /* GPIO ABCD */
      do_IRQ(BSP_GPIO_ABCD_EXT_IRQ);
  }
  else if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[ICTL_OFFSET(BSP_WDT_IP1_IRQ)]))
  {
      if((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
      {
          rtl8380_wdt_phase1();
      }
      do_IRQ(BSP_WDT_IP1_IRQ);
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

/* Vector3 */
void bsp_ictl_gic_irq_dispatch_v3(void)
{
    bsp_get_gic_int_mask(bsp_ints3);
}

/* Vector4 */
void bsp_ictl_gic_irq_dispatch_v4(void)
{
    bsp_get_gic_int_mask(bsp_ints4);
}

#ifdef CONFIG_SMP
/* Vector5 */
void bsp_ictl_gic_irq_dispatch_v5(void)
{
    bsp_ipi_irqdispatch();
}

/* Vector6 */
void bsp_ictl_gic_irq_dispatch_v6(void)
{
   bsp_ipi_irqdispatch();
}
#endif

void bsp_ictl_gic_irq_dispatch_v7(void)
{
    bsp_get_gic_int_mask(bsp_ints7);
}

/**************************************************************************************
 * CPU_MIPSR2_IRQ_VI only support 8 vectored interrupt
 * corresponded  with IP0 ~ IP7
     * Vector 0 for IP0(SW0)
     * Vector 1 for IP1(SW1)
 *
 * Vector 5 for IP5  ---> IPI resched
 * Vector 6 for IP6  ---> IPI call
 * Vector 7 for IP7  ---> External Timer(CEVT_EXT),(MIPS Default, used by CEVT_R4K)
 *************************************************************************************/
static void __init  bsp_setup_vi(void){
    if(bsp_chip_family_id != RTL9310_FAMILY_ID)
    {
        set_vi_handler(2, bsp_ictl_irq_dispatch_v2);            /*IP2 */
        set_vi_handler(3, bsp_ictl_irq_dispatch_v3);            /*IP3 */
        set_vi_handler(4, bsp_ictl_irq_dispatch_v4);            /*IP4 */
        set_vi_handler(5, bsp_ictl_irq_dispatch_v5);            /*IP5 */
        set_vi_handler(6, bsp_ictl_irq_dispatch_v6);            /*IP6 */
     }else{
        set_vi_handler(2, bsp_ictl_gic_irq_dispatch_v2);        /*IP2 */
        set_vi_handler(3, bsp_ictl_gic_irq_dispatch_v3);        /*IP3 */
        set_vi_handler(4, bsp_ictl_gic_irq_dispatch_v4);        /*IP4 */
#ifdef CONFIG_SMP
        set_vi_handler(5, bsp_ictl_gic_irq_dispatch_v5);        /*IP5,  Reschule */
        set_vi_handler(6, bsp_ictl_gic_irq_dispatch_v6);        /*IP6,  Call */
#endif
#ifdef CONFIG_ARCH_CEVT_EXT
        set_vi_handler(7, bsp_ictl_gic_irq_dispatch_v7);        /*IP7,  CPU clock */
#endif
     }
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

static inline void __init bsp_ictl00_gic_irq_set(int irq){

       irq_set_chip(irq, &bsp_ictl_gic_irq);
       irq_set_handler(irq, handle_level_irq);
}

static void __init bsp_irq_init(void)
{
   int i;
   /* Initialize for IRQ: 0~31 */
#ifdef CONFIG_ARCH_CEVT_EXT
  if((bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID) || (bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID) || (bsp_chip_family_id == RTL9300_FAMILY_ID))
  {
      bsp_ictl00_irq_set(BSP_TC0_EXT_IRQ);
  }
  else if(bsp_chip_family_id == RTL9310_FAMILY_ID)
  {
      bsp_ictl00_gic_irq_set(BSP_TC0_EXT_IRQ);
  }
  else
  {
      printk("\n[%s] initial failed!!!\n",__FUNCTION__);
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
        BSP_REG32(GIMR) = UART0_IE_RTL9300 | TC0_IE_RTL9300;

        BSP_REG32(IRR0) = IRR0_RTL9300_SETTING;  /* UART 0 */
        BSP_REG32(IRR1) = IRR1_RTL9300_SETTING;
        BSP_REG32(IRR2) = IRR2_RTL9300_SETTING;
        BSP_REG32(IRR3) = IRR3_RTL9300_SETTING;
    }else if(bsp_chip_family_id != RTL9310_FAMILY_ID){
        /* Set GIMR, IRR *//* For RTL8390/80*/
        BSP_REG32(GIMR) = TC0_IE | UART0_IE;

        BSP_REG32(IRR0) = IRR0_SETTING;
        BSP_REG32(IRR1) = IRR1_SETTING;
        BSP_REG32(IRR2) = IRR2_SETTING;
        BSP_REG32(IRR3) = IRR3_SETTING;
    }
#ifdef CONFIG_SMP
/* GIC is mandatory */
   for (i = 0; i < nr_cpu_ids; i++) {
	arch_init_ipiirq(MIPS_GIC_IRQ_BASE +
					GIC_IPI_RESCHED(i), &irq_resched);
	arch_init_ipiirq(MIPS_GIC_IRQ_BASE +
					 GIC_IPI_CALL(i), &irq_call);
   }
#endif

}
static void plat_gic_init(void)
{
#ifdef CONFIG_SMP
    irq_validity_check();
    fill_ipi_map();
#endif
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


