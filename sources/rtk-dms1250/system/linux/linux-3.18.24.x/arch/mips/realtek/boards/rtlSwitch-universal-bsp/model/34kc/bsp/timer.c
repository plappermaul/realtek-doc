/*
 * Realtek Semiconductor Corp.
 *
 * bsp/timer.c
 *     bsp timer initialization code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/timex.h>
#include <linux/delay.h>
#include <asm/setup.h>
#include <asm/irq.h>
#include <asm/time.h>
#include "bspchip.h"
#include "chip_probe.h"
#include <int_setup.h>
#include "bsp_ioal.h"

#ifdef CONFIG_CEVT_EXT
extern int ext_clockevent_init(void);
#endif


static void rtl8390_timer_ack(void);
static void __init rtl8390_timer_setup(void);

static void rtl8380_timer_ack(void);
static void __init rtl8380_timer_setup(void);

static int rtk_watchdog_default_func(void)
{
    return 0;
}

int (*rtk_watchdog_kick_func)(void) = rtk_watchdog_default_func;

EXPORT_SYMBOL(rtk_watchdog_kick_func);


/***************************************************
 * RTL not use MIPS R4K Clock event, this function
 * is for test or debug purpose.
 **************************************************/
#if defined(CONFIG_CEVT_R4K)
static void mips_timer_dispatch(void)
{
    do_IRQ(BSP_COMPARE_IRQ);
}

unsigned int __init get_c0_compare_int(void)
{
#ifdef CONFIG_CPU_MIPSR2_IRQ_VI
        if (cpu_has_vint)
          set_vi_handler(BSP_COMPARE_IRQ, mips_timer_dispatch);
#endif
    return BSP_COMPARE_IRQ;
}

#endif
static void __init rtl9300_bsp_ext_time_init_all(void){
        int i, offset;
        int TCDATA = 0;
    int TCCTL = 0;

        TCDATA = ( (MHZ_9300_MP * 1000000)/ ((int) DIVISOR_RTL9300 * HZ) );

    for(i=0; i<RTL9300_TC_MAX; i++){
        offset = i*0x10;
        if (BSP_REG32(RTL93XXMP_TC0INT + offset) & RTL93XXMP_TCIP){
            BSP_REG32(RTL93XXMP_TC0INT + offset) |= RTL93XXMP_TCIP;
        }
            /* disable timer before setting CDBR */
            BSP_REG32(RTL93XXMP_TC0CTL + offset) = 0; /* disable timer before setting CDBR */
        BSP_REG32(RTL93XXMP_TC0DATA + offset ) =  TCDATA;
    }
        /* Enable timer for all CPU at one time. Let the count of all timer is near */
    TCCTL = RTL93XXMP_TCEN | RTL93XXMP_TCMODE_TIMER | DIVISOR_RTL9300 ;
    for(i=0; i<NR_CPUS && i<RTL9300_TC_MAX; i++){
       offset = i*0x10;
       BSP_REG32(RTL93XXMP_TC0CTL+ offset) = TCCTL;
    }
}

static void __init rtl9310_bsp_ext_time_init_all(void){
        int i, offset;
        int TCDATA = 0;
    int TCCTL = 0;

        TCDATA = ( (MHZ * 1000000)/ ((int) DIVISOR_RTL9310 * HZ) );

    for(i=0; i<RTL9310_TC_MAX; i++){
        offset = i*0x10;
        if (BSP_REG32(RTL93XXMP_TC0INT + offset) & RTL93XXMP_TCIP){
            BSP_REG32(RTL93XXMP_TC0INT + offset) |= RTL93XXMP_TCIP;
        }
            /* disable timer before setting CDBR */
            BSP_REG32(RTL93XXMP_TC0CTL + offset) = 0; /* disable timer before setting CDBR */
        BSP_REG32(RTL93XXMP_TC0DATA + offset ) =  TCDATA;
    }
        /* Enable timer for all CPU at one time. Let the count of all timer is near */
    TCCTL = RTL93XXMP_TCEN | RTL93XXMP_TCMODE_TIMER | DIVISOR_RTL9310 ;
    for(i=0; i<RTL9310_TC_MAX; i++){
       offset = i*0x10;
       BSP_REG32(RTL93XXMP_TC0CTL+ offset) = TCCTL;
    }
}

static void __init rtl9310_bsp_timer_all(void){
    rtl9310_bsp_ext_time_init_all();

    /* hook up timer interrupt handler on CPU0 for TC0
     * CPU1~CPU3 need do this by itself
     */
    ext_clockevent_init();
    /*Only enable interrrupt of TC0 ,
     *other timer interrupt will be enalbe by other CPU init
     */
    BSP_REG32(RTL93XXMP_TC0INT) = RTL93XXMP_TCIE;
}


static void __init rtl9300_bsp_timer_all(void){
    rtl9300_bsp_ext_time_init_all();

    /* hook up timer interrupt handler on CPU0 for TC0
     * CPU1~CPU3 need do this by itself
     */
    ext_clockevent_init();
    /*Only enable interrrupt of TC0 ,
     *other timer interrupt will be enalbe by other CPU init
     */
    BSP_REG32(RTL93XXMP_TC0INT) = RTL93XXMP_TCIE;
}

static void rtl8390_timer_ack(void)
{
    unsigned int model_info;
    unsigned int chip_info;
    static unsigned int is_probe = 0, is_tc = 0;

    if (is_probe == 0)
    {
        model_info = BSP_REG32(0xBB000FF0);
        BSP_REG32(0xBB000FF4) = 0xA0000000;
        chip_info = BSP_REG32(0xBB000FF4);
        BSP_REG32(0xBB000FF4) = 0;
        if ((chip_info & 0xFFFF) == 0x0399)
            is_tc = 1;
        else
            is_tc = 0;
        is_probe = 1;
    }

    BSP_REG32(RTL8390MP_TC0INT) |= RTL8390MP_TCIP;

    return;
}

static void __init rtl8390_timer_setup()
{
    unsigned int model_info;
    unsigned int chip_info;


    model_info = BSP_REG32(0xBB000FF0);
    BSP_REG32(0xBB000FF4) = 0xA0000000;
    chip_info = BSP_REG32(0xBB000FF4);
    BSP_REG32(0xBB000FF4) = 0;

    ext_clockevent_init();

    /* Setup Timer0 */

    /* Clear Timer IP status */
    if (BSP_REG32(RTL8390MP_TC0INT) & RTL8390MP_TCIP)
        BSP_REG32(RTL8390MP_TC0INT) |= RTL8390MP_TCIP;

    BSP_REG32(RTL8390MP_TC0CTL) = 0; /* disable timer before setting CDBR */
    BSP_REG32(TC09X8XDATA)= ((MHZ * 1000000)/(DIVISOR_RTL8390 * HZ));
    BSP_REG32(RTL8390MP_TC0CTL) = RTL8390MP_TCEN | RTL8390MP_TCMODE_TIMER | DIVISOR_RTL8390 ;
    BSP_REG32(RTL8390MP_TC0INT) = RTL8390MP_TCIE;

    return;
}

static void rtl8380_timer_ack(void)
{
    unsigned int original_data_intRd;
    unsigned int original_data_chipRd;
    unsigned int temp = 0;
    static unsigned int is_probe = 0;

    if (is_probe == 0)
    {
        original_data_intRd = BSP_REG32(0xBB000058);
        BSP_REG32(0xBB000058) = (original_data_intRd | 0x3);
        original_data_chipRd = BSP_REG32(0xBB0000D8);
        BSP_REG32(0xBB0000D8) = (original_data_chipRd | 0xA0000000);
        temp = BSP_REG32(0xBB0000D4);
        BSP_REG32(0xBB0000D8) = original_data_chipRd;
        BSP_REG32(0xBB000058) = original_data_intRd;
        is_probe = 1;
    }
    BSP_REG32(RTL8380MP_TC0INT) |= RTL8380MP_TCIP;

    return;
}

static void __init rtl8380_timer_setup( )
{
    unsigned int original_data_intRd;
    unsigned int original_data_chipRd;
    unsigned int temp = 0;
    unsigned int temp_chip_info = 0;

    //prom_printf("plat_timer_setup API\n");
    original_data_intRd = BSP_REG32(0xBB000058);
    BSP_REG32(0xBB000058) = (original_data_intRd | 0x3);
    original_data_chipRd = BSP_REG32(0xBB0000D8);
    BSP_REG32(0xBB0000D8) = (original_data_chipRd | 0xA0000000);
    temp = BSP_REG32(0xBB0000D4);
    temp_chip_info = BSP_REG32(0xBB0000D8);
    BSP_REG32(0xBB0000D8) = original_data_chipRd;
    BSP_REG32(0xBB000058) = original_data_intRd;

    ext_clockevent_init();


    /* Setup Timer0 */
    {
        //prom_printf("plat_timer_setup API: 8380 MP-chip\n");
        if (BSP_REG32(RTL8380MP_TC0INT) & RTL8380MP_TCIP)
            BSP_REG32(RTL8380MP_TC0INT) |= RTL8380MP_TCIP;

        BSP_REG32(RTL8380MP_TC0CTL) = 0; /* disable timer before setting CDBR */
        BSP_REG32(TC09X8XDATA)= ((MHZ * 1000000)/(DIVISOR * HZ));
        BSP_REG32(RTL8380MP_TC0CTL) = RTL8380MP_TCEN | RTL8380MP_TCMODE_TIMER | DIVISOR ;
        BSP_REG32(RTL8380MP_TC0INT) = RTL8380MP_TCIE;
    }

    return;

}

void inline bsp_timer_ack(void)
{
    unsigned int offset = smp_processor_id() * 0x10;
    int          cpu = smp_processor_id();
    if(bsp_chip_family_id == RTL9310_FAMILY_ID)
    {
        offset = (bsp_cpu_timerId_irq[cpu].timer_id) * TC_REG_OFFSET;
        BSP_REG32(RTL93XXMP_TC0INT + offset) |= RTL93XXMP_TCIP;
    }
    else if(bsp_chip_family_id == RTL9300_FAMILY_ID)
    {
        BSP_REG32(RTL93XXMP_TC0INT + offset) |= RTL93XXMP_TCIP;
    }
    else if((bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID))
    {
        rtl8390_timer_ack();
    }
    else if((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
    {
        rtl8380_timer_ack();
    }
    rtk_watchdog_kick_func();
}

void __init plat_time_init(void)
{
#if defined(CONFIG_CEVT_EXT)
    if(bsp_chip_family_id == RTL9310_FAMILY_ID)
    {
        rtl9310_bsp_timer_all();
    }else if(bsp_chip_family_id == RTL9300_FAMILY_ID)
    {
        rtl9300_bsp_timer_all();
    }else if((bsp_chip_family_id == RTL8390_FAMILY_ID) || (bsp_chip_family_id == RTL8350_FAMILY_ID))
    {
        rtl8390_timer_setup();
    }else if((bsp_chip_family_id == RTL8380_FAMILY_ID) || (bsp_chip_family_id == RTL8330_FAMILY_ID))
    {
        rtl8380_timer_setup();
    }
#elif defined(CONFIG_CEVT_R4K)
/* For R4K on FPGA  */
#define BSP_CPU0_FREQ           80000000     /* 80 MHz */

        /* set cp0_compare_irq and cp0_perfcount_irq */
        cp0_compare_irq = BSP_COMPARE_IRQ;
        cp0_perfcount_irq = BSP_PERFCOUNT_IRQ;

        mips_hpt_frequency = BSP_CPU0_FREQ / 2;

        write_c0_count(0);
#endif

    return;
}

