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
 * $Revision: 85728 $
 * $Date: 2018-01-25 13:40:22 +0800 (Thu, 25 Jan 2018) $
 *
 */

/*
 * Include Files
 */
#include <linux/irq.h>
#include <linux/hardirq.h>
#include <asm/irq_cpu.h>
#include <prom.h>
#include <platform.h>
#include <int_setup.h>

#include "chip_probe.h"
#include "gcmp-gic.h"

#include <asm/bitops.h>
#include <linux/bitmap.h>


struct gic_intr_map {
    unsigned int cpunum;	/* Directed to this CPU */
#define GIC_UNUSED		0xdead			/* Dummy data */
    unsigned int pin;	/* Directed to this Pin */
    unsigned int polarity;	/* Polarity : +/-	*/
    unsigned int trigtype;	/* Trigger  : Edge/Levl */
    unsigned int flags;	/* Misc flags	*/
#define GIC_FLAG_IPI           0x01
#define GIC_FLAG_TRANSPARENT   0x02
};
#define GIC_FLAG_TRANSPARENT   0x02



/*
 * Symbol Definition
 */
#if defined(CONFIG_RTL8390_SERIES)
#define rtl8390_shutdown_irq      rtl8390_disable_irq
#define rtl8390_mask_and_ack_irq  rtl8390_disable_irq

static void rtl8390_enable_irq(unsigned int irq);
static void rtl8390_disable_irq(unsigned int irq);
static void rtl8390_end_irq(unsigned int irq);
static unsigned int rtl8390_startup_irq(unsigned int irq);
#endif

#if defined(CONFIG_RTL8380_SERIES)
#define rtl8380_shutdown_irq      rtl8380_disable_irq
#define rtl8380_mask_and_ack_irq  rtl8380_disable_irq

static void rtl8380_enable_irq(unsigned int irq);
static void rtl8380_disable_irq(unsigned int irq);
static void rtl8380_end_irq(unsigned int irq);
static unsigned int rtl8380_startup_irq(unsigned int irq);
#endif

#if defined(CONFIG_RTL9300_SERIES)
#define rtl9300_shutdown_irq      rtl9300_disable_irq
#define rtl9300_mask_and_ack_irq  rtl9300_disable_irq

static void rtl9300_enable_irq(unsigned int irq);
static void rtl9300_disable_irq(unsigned int irq);
static void rtl9300_end_irq(unsigned int irq);
static unsigned int rtl9300_startup_irq(unsigned int irq);
#endif


#if defined(CONFIG_RTL9310_SERIES)

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
  bitmap_zero(bsp_ints6, GIC_NUM_INTRS);
  set_bit(0, bsp_ints6);

  /* IP5 */
  bitmap_zero(bsp_ints5, GIC_NUM_INTRS);
  set_bit(8, bsp_ints5);
  set_bit(9, bsp_ints5);
  set_bit(16, bsp_ints5);
  set_bit(20, bsp_ints5);

  /* IP4 */
  bitmap_zero(bsp_ints4, GIC_NUM_INTRS);
  set_bit(15, bsp_ints4);

  /* IP3 */
  bitmap_zero(bsp_ints3, GIC_NUM_INTRS);
  set_bit(22, bsp_ints3);

  /* IP2 */
  bitmap_zero(bsp_ints2, GIC_NUM_INTRS);
  set_bit(1, bsp_ints2);
  set_bit(23, bsp_ints2);
  set_bit(36, bsp_ints2);
}

#endif /*End of CONFIG_RTL9310_SERIES*/


/*
 * Data Declaration
 */
spinlock_t irq_lock = SPIN_LOCK_UNLOCKED;

#if defined(CONFIG_RTL8390_SERIES)
static struct irq_chip irq_type_rtl8390 = {
   .typename = "RTL8390",
   .startup = rtl8390_startup_irq,
   .shutdown = rtl8390_shutdown_irq,
   .enable = rtl8390_enable_irq,
   .disable = rtl8390_disable_irq,
   .ack = rtl8390_mask_and_ack_irq,
   .end = rtl8390_end_irq,
};
#endif

#if defined(CONFIG_RTL8380_SERIES)
static struct irq_chip irq_type_rtl8380 = {
   .typename = "RTL8380",
   .startup = rtl8380_startup_irq,
   .shutdown = rtl8380_shutdown_irq,
   .enable = rtl8380_enable_irq,
   .disable = rtl8380_disable_irq,
   .ack = rtl8380_mask_and_ack_irq,
   .end = rtl8380_end_irq,
};
#endif

#if defined(CONFIG_RTL9300_SERIES)
static struct irq_chip irq_type_rtl9300 = {
   .typename = "RTL9300",
   .startup = rtl9300_startup_irq,
   .shutdown = rtl9300_shutdown_irq,
   .enable = rtl9300_enable_irq,
   .disable = rtl9300_disable_irq,
   .ack = rtl9300_mask_and_ack_irq,
   .end = rtl9300_end_irq,
};
#endif

/* GIC is transparent, just bridge*/
struct gic_intr_map rtl9310_gic_intr_map[GIC_NUM_INTRS] = {
    /* EXT_INT 0~7 */
    { 0, GIC_CPU_INT4,	GIC_POL_POS,		GIC_TRIG_LEVEL,		GIC_FLAG_TRANSPARENT },   /* Timer 0 *//*Index 0*/
    { 0, GIC_CPU_INT1,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 1 *//*Index 1*/
    { 0, GIC_CPU_INT2,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT3,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT4,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },
    { 0, GIC_CPU_INT5,	GIC_POL_POS,		GIC_TRIG_LEVEL,		0 },                      /* Timer 6 *//*Index 6*/
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
    { 0, GIC_CPU_INT0,  GIC_POL_POS,        GIC_TRIG_LEVEL,     GIC_FLAG_TRANSPARENT },    /* USB H2 *//* Index 36*/
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
    [12] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [13] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [14] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [15] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [16] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [17] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 36,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = USB_H2_IRQ },           /*Index = USB_H2_IRQ = 17*/
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
    [14] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [15] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [16] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [17] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = 28,                 .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = USB_H2_IRQ },           /*Index = USB_H2_IRQ = 17*/
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
    [15] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [16] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = BSP_UNUSED_FIELD },
    [17] = { .target_cpu_id = TARGET_CPU_ID0, .cpu_ip_id = CPU_INT2, .intr_type = INTR_TYPE_DEFAULT,      .intr_src_id = BSP_UNUSED_FIELD,   .intr_defalut = INTR_DEFAULT_DISABLE,   .mapped_system_irq = USB_H2_IRQ },           /*Index = USB_H2_IRQ = 17*/
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

extern unsigned int bsp_chip_id, bsp_chip_rev_id;
extern unsigned int bsp_chip_family_id, bsp_chip_type;
extern unsigned int bsp_Interrupt_IRQ_Mapping_IP[ICTL_MAX_IP];
extern unsigned int bsp_Interrupt_srcID_Mapping_IRQ[GIC_NUM_INTRS];

extern unsigned int gic_get_int(void);
extern void gic_get_int_mask(unsigned long *dst, const unsigned long *src);

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */
#if defined(CONFIG_RTL8390_SERIES)
static void rtl8390_enable_irq(unsigned int irq)
{
   unsigned long flags;

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) | (1 << irq);
   spin_unlock_irqrestore(&irq_lock, flags);
}

static unsigned int rtl8390_startup_irq(unsigned int irq)
{
   rtl8390_enable_irq(irq);

   return 0;
}

static void rtl8390_disable_irq(unsigned int irq)
{
   unsigned long flags;

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) & (~(1 << irq));
   spin_unlock_irqrestore(&irq_lock, flags);
}

static void rtl8390_end_irq(unsigned int irq)
{
   unsigned long flags;

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) | (1 << irq);
   spin_unlock_irqrestore(&irq_lock, flags);
}
#endif /*End of CONFIG_RTL8390_SERIES*/

#if defined(CONFIG_RTL8380_SERIES)
static void rtl8380_enable_irq(unsigned int irq)
{
   unsigned long flags;

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) | (1 << irq);
   spin_unlock_irqrestore(&irq_lock, flags);
}

static unsigned int rtl8380_startup_irq(unsigned int irq)
{
   rtl8380_enable_irq(irq);

   return 0;
}

static void rtl8380_disable_irq(unsigned int irq)
{
   unsigned long flags;

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) & (~(1 << irq));
   spin_unlock_irqrestore(&irq_lock, flags);
}

static void rtl8380_end_irq(unsigned int irq)
{
   unsigned long flags;

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) | (1 << irq);
   spin_unlock_irqrestore(&irq_lock, flags);
}

static void rtl8380_wdt_phase1(void)
{
    uint32 tmp = 0;

    REG32(0xb8003154) = 0x80000000; /*WDT PH1 IP clear*/
    OTTO838x_FLASH_DISABLE_4BYTE_ADDR_MODE();
}

#endif /*End of CONFIG_RTL8380_SERIES*/


#if defined(CONFIG_RTL9300_SERIES)
static void rtl9300_enable_irq(unsigned int irq)
{
   unsigned long flags;
   unsigned int ip_bit;

   ip_bit =	bsp_Interrupt_IRQ_Mapping_IP[irq];

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) | (1 << ip_bit);
   spin_unlock_irqrestore(&irq_lock, flags);
}

static unsigned int rtl9300_startup_irq(unsigned int irq)
{
   rtl9300_enable_irq(irq);

   return 0;
}

static void rtl9300_disable_irq(unsigned int irq)
{
   unsigned long flags;
   unsigned int ip_bit;

   ip_bit =	bsp_Interrupt_IRQ_Mapping_IP[irq];

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) & (~(1 << ip_bit));
   spin_unlock_irqrestore(&irq_lock, flags);
}
static void rtl9300_end_irq(unsigned int irq)
{
   unsigned long flags;
   unsigned int ip_bit;

   ip_bit =	bsp_Interrupt_IRQ_Mapping_IP[irq];

   spin_lock_irqsave(&irq_lock, flags);
   REG32(GIMR) = REG32(GIMR) | (1 << ip_bit);
   spin_unlock_irqrestore(&irq_lock, flags);
}

#endif /*End of CONFIG_RTL9300_SERIES*/

#if defined(CONFIG_RTL9310_SERIES)
static inline void bsp_get_gic_int_mask(unsigned long *map){
    unsigned long idx;
    unsigned int system_irq;
    DECLARE_BITMAP(pending, GIC_NUM_INTRS);
    gic_get_int_mask(pending, map);
    idx = find_first_bit(pending, GIC_NUM_INTRS);
    while (idx < GIC_NUM_INTRS) {
        system_irq = bsp_Interrupt_srcID_Mapping_IRQ[idx];
        do_IRQ(system_irq);
        idx = find_next_bit(pending, GIC_NUM_INTRS, idx + 1);
    }
}
#endif /*End of CONFIG_RTL9310_SERIES*/

void __init arch_init_irq(void)
{
   int i;
#if (defined(CONFIG_RTL8390_SERIES) || defined(CONFIG_RTL8380_SERIES) || defined(CONFIG_RTL9300_SERIES) || defined(CONFIG_RTL9310_SERIES))
   struct irq_chip * irq_callback_ptr = NULL;
#endif

#if defined(CONFIG_RTL8390_SERIES)
    if((bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID))
        irq_callback_ptr = &irq_type_rtl8390;
#endif
#if defined(CONFIG_RTL8380_SERIES)
    if((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
        irq_callback_ptr = &irq_type_rtl8380;
#endif

#if defined(CONFIG_RTL9300_SERIES)
    if((bsp_chip_family_id == RTL9300_FAMILY_ID))
        irq_callback_ptr = &irq_type_rtl9300;
#endif

    /* Initialize for IRQ: 0~31 */
    /*For MIPS IA, the IRQ call back functions are setup in gic_basic_init()*/
    if((bsp_chip_family_id != RTL9310_FAMILY_ID))
    {
        for (i = 0; i < 32; i++) {
            irq_desc[i].chip = irq_callback_ptr;
        }
    }
   /* Disable internal Count register */
   write_c0_cause(read_c0_cause() | (1 << 27));

   /* Clear internal timer interrupt */
   write_c0_compare(0);

   /* Enable all interrupt mask of CPU */
   write_c0_status(read_c0_status() | ST0_IM);


#if defined(CONFIG_RTL8390_SERIES)
    if((bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID))
       {
           /* Set GIMR, IRR */
           REG32(GIMR) = TC0_IE | (1UL<<30) | UART0_IE;

           REG32(IRR0) = IRR0_SETTING;
           REG32(IRR1) = IRR1_SETTING;
           REG32(IRR2) = IRR2_SETTING;
           REG32(IRR3) = IRR3_SETTING;
       }
#endif

#if defined(CONFIG_RTL8380_SERIES)
    if((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
       {
           /* Set GIMR, IRR */
           REG32(GIMR) = TC0_IE | (1UL<<30) | UART0_IE;

           REG32(IRR0) = IRR0_SETTING;
           REG32(IRR1) = IRR1_SETTING;
           REG32(IRR2) = IRR2_SETTING;
           REG32(IRR3) = IRR3_SETTING;
       }
#endif


#if defined(CONFIG_RTL9300_SERIES)
    if((bsp_chip_family_id == RTL9300_FAMILY_ID))
       {
           /* Set GIMR, IRR */
           REG32(GIMR) = TC0_IE_RTL9300 | UART1_IE_RTL9300 | UART0_IE_RTL9300;
           REG32(GIMR) = TC0_IE_RTL9300  | UART0_IE_RTL9300;

           REG32(IRR0) = IRR0_RTL9300_SETTING;
           REG32(IRR1) = IRR1_RTL9300_SETTING;
           REG32(IRR2) = IRR2_RTL9300_SETTING;
           REG32(IRR3) = IRR3_RTL9300_SETTING;

       }
#endif

#if defined(CONFIG_RTL9310_SERIES)
    if((bsp_chip_family_id == RTL9310_FAMILY_ID))
       {
            fill_bsp_int_map();
            init_gic();
       }
#endif

}

asmlinkage void plat_irq_dispatch(void)
{
   unsigned int cpuint_ip = read_c0_cause() & read_c0_status() & ST0_IM;
   unsigned int extint_ip;

   if (cpuint_ip & CAUSEF_IP6)
   {
      /* Timer 0 */
      do_IRQ(TC0_IRQ);
   }
   else if (cpuint_ip & CAUSEF_IP5)
   {
      if((bsp_chip_family_id == RTL9310_FAMILY_ID))
      {
        bsp_get_gic_int_mask(bsp_ints5);
      }
      else
      {
        extint_ip = REG32(GIMR) & REG32(GISR);  /* For shared interrupts */

        if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[NIC_IRQ]))
        {
            /* NIC */
            do_IRQ(NIC_IRQ);
        }
        else if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[GPIO_ABCD_IRQ]))
        {
            /* GPIO ABCD */
            do_IRQ(GPIO_ABCD_IRQ);
        }
#if defined(CONFIG_RTL8380_SERIES)
        if((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
        {
            if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[WDT_IP1_IRQ]))
            {
                rtl8380_wdt_phase1();
                /* WDT Phase-1 */
                do_IRQ(WDT_IP1_IRQ);
            }
        }
#endif
     }
   }
   else if (cpuint_ip & CAUSEF_IP4)
   {
      /* SWCORE */
      do_IRQ(SWCORE_IRQ);
   }
   else if (cpuint_ip & CAUSEF_IP3)
   {
      /* UART 0 */
      do_IRQ(UART0_IRQ);
   }
   else if (cpuint_ip & CAUSEF_IP2)
   {
      if((bsp_chip_family_id == RTL9310_FAMILY_ID))
      {
         bsp_get_gic_int_mask(bsp_ints2);
      }
      else
      {
         extint_ip = REG32(GIMR) & REG32(GISR);  /* For shared interrupts */

            /* This code also should be modified because of 9300 & 93100 difference from 80/90 */
          if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[TC1_IRQ]))
          {
             do_IRQ(TC1_IRQ);
          }
          else if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[UART1_IRQ]))
          {
             do_IRQ(UART1_IRQ);
          }
          else if (extint_ip & (0x1 << bsp_Interrupt_IRQ_Mapping_IP[USB_H2_IRQ]))
          {
             do_IRQ(USB_H2_IRQ);
        }
      }
   }
   else
   {
      prom_printf("Unknown_IRQ!\n");
      prom_printf("Current IP: 0x%08X!\n", cpuint_ip);
   }
}
