/*  Routines to access hardware
*
*  Copyright (c) 2018 Realtek Semiconductor Corp.
*
*  This module is a confidential and proprietary property of RealTek and
*  possession or use of this module requires written permission of RealTek.
*/


#include "basic_types.h"
#include "diag.h"
#include <asm/cache.h>
#include "peripheral.h"
#include "section_config.h"

//#define WATCHDOG_CLEAR_VERIFY 

#ifndef REG32
#define REG32(reg)      (*(volatile unsigned long *)(reg))
#endif
#if defined(CONFIG_BSP_RTL_MY_ASIC)
#define WATCHDOG_BASE_CLOCK 200000000
#elif defined(CONFIG_BSP_RTL_MY_FPGA)
#define WATCHDOG_BASE_CLOCK 20000000
#else
#define WATCHDOG_BASE_CLOCK 20000000
#endif
#define WTCR 0xb800311c
#define CDBR 0xb8003118
#define TC0DATA 0xb8003100
#define TC0CNT 0xb8003108
#define TC1DATA 0xb8003104
#define TC1CNT 0xb800310c
#define TC2DATA 0xb8003120
#define TC2CNT 0xb8003128
#define TC3DATA 0xb8003124
#define TC3CNT 0xb800312c
#define TCCNR 0xb8003110
#define TCIR 0xb8003114

#define TIMER_ISR_0 7
#define TIMER_ISR_1 32+0
#define TIMER_ISR_2 32+1
#define TIMER_ISR_3 10 


#define TC0_EN 31
#define TC0_MODE 30
#define TC1_EN 29
#define TC1_MODE 28
#define TC2_EN 27
#define TC2_MODE 26 
#define TC3_EN 25
#define TC3_MODE 24

#define TC0_IE 31
#define TC1_IE 30
#define TC0_IP 29
#define TC1_IP 28

#define TC2_IE 27
#define TC3_IE 26
#define TC2_IP 25
#define TC3_IP 24


static inline void watchdog_disable(void) {REG32(WTCR) = ((REG32(WTCR)&0x00ffffff) | 0xa5000000);}
static inline void watchdog_enable(void) {REG32(WTCR) = ((REG32(WTCR)&0x00ffffff));}

void watchdog_set_reset_period(unsigned int sec)
{
        unsigned long long ovsel, divfactor_64;
        unsigned long long temp;
        unsigned int divfactor;

        //disable first, & change ovsel
        REG32(WTCR) = (REG32(WTCR)&0x00ffffff) | 0xa5220000;        
        ovsel = (((REG32(WTCR)>>17) & 0x3) << 2) | ((REG32(WTCR)>>21) & 0x3);
        ovsel = 1<<(15 + ovsel);
#if 1
        if(sec == 0)
        {
                //disable
                REG32(WTCR) = (REG32(WTCR)&0x00ffffff) | 0xa5000000;
                printf("disable watchdog \n");
        }
        else
        {
                temp =  sec*WATCHDOG_BASE_CLOCK;
                divfactor_64 = (temp / ovsel);
                if( ((divfactor_64 >> 32) & 0xFFFFFFFF) || ((divfactor_64 ) & 0xFFFF0000) )
                {
                        printf("watchdog period overflow, please set smaller period or set OVSEL for large period \n");
                }
                else if((divfactor_64 == 0x0) || (divfactor_64 == 0x1))
                {
                        printf("watchdog period underflow, please set larger period or set OVSEL for smalle period \n");
                }
                else
                {
                        divfactor = (divfactor_64 & 0x0000FFFF);
                        REG32(CDBR) = divfactor << 16;
                        printf("CDBR = 0x%x \n", REG32(CDBR));
                        //REG32(WTCR) = (REG32(WTCR)&0x00ffffff);
                }
        }
#else
         REG32(CDBR) = 0xffff << 16;
#endif
        return;
}

VOID watchdog_isr(VOID *DATA)
{

        
        printf("\ntime out TC0_CNT = 0x%x\n", REG32(TC0CNT)>>4);
        printf("\ntime out TC1_CNT = 0x%x\n", REG32(TC1CNT)>>4);
        printf("\ntime out TC2_CNT = 0x%x\n", REG32(TC2CNT)>>4);
        printf("\ntime out TC3_CNT = 0x%x\n", REG32(TC3CNT)>>4);
        printf("TCIR = 0x%x\n", REG32(TCIR));

        //clear irq
        REG32(TCIR) = (0x1<<TC0_IP) | (0x1<<TC1_IP) | (0x1<<TC2_IP) | (0x1<<TC3_IP);
        
        //disable timer counter mode
        REG32(TCCNR) = 0;//(0<<31) | (0<<30);

        return;
}

void watchdog_timer0_counter_mode(unsigned int reset_period_sec)
{
        unsigned int divfactor;
        double counter_per_sec;
        unsigned int tc_data;
        
        divfactor = REG32(CDBR)>>16;
        counter_per_sec = WATCHDOG_BASE_CLOCK/divfactor;
        tc_data = (unsigned int)((double)counter_per_sec*reset_period_sec);
        tc_data = tc_data<<4;
        REG32(TC0DATA) = tc_data & 0xFFFFFFF0;

        irq_install_handler(TIMER_ISR_0,(IRQ_FUN)watchdog_isr, NULL);
        //enable interrupt
        REG32(TCIR) = 0x1<<31;
        
        //enable timer counter mode
        REG32(TCCNR) = (1<<31) | (0<<30);

        return;
        
}

void watchdog_timer1_counter_mode(unsigned int reset_period_sec)
{
        unsigned int divfactor;
        double counter_per_sec;
        unsigned int tc_data;
        
        divfactor = REG32(CDBR)>>16;
        counter_per_sec = WATCHDOG_BASE_CLOCK/divfactor;
        tc_data = (unsigned int)((double)counter_per_sec*reset_period_sec);
        tc_data = tc_data<<4;
        REG32(TC1DATA) = tc_data & 0xFFFFFFF0;

        irq_install_handler(TIMER_ISR_1,(IRQ_FUN)watchdog_isr, NULL);
        //enable interrupt
        REG32(TCIR) = 0x1<<TC1_IE;
        
        //enable timer counter mode
        REG32(TCCNR) = (1<<TC1_EN) | (0<<TC1_MODE);

        return;
        
}

void watchdog_timer2_counter_mode(unsigned int reset_period_sec)
{
        unsigned int divfactor;
        double counter_per_sec;
        unsigned int tc_data;
        printf("Test Timer2 counter mode \n");
        divfactor = REG32(CDBR)>>16;
        counter_per_sec = WATCHDOG_BASE_CLOCK/divfactor;
        tc_data = (unsigned int)((double)counter_per_sec*reset_period_sec);
        tc_data = tc_data<<4;
        REG32(TC2DATA) = tc_data & 0xFFFFFFF0;

        irq_install_handler(TIMER_ISR_2,(IRQ_FUN)watchdog_isr, NULL);
        //enable interrupt
        REG32(TCIR) = 0x1<<TC2_IE;
        
        //enable timer counter mode
        REG32(TCCNR) = (1<<TC2_EN) | (0<<TC2_MODE);

        return;
        
}

void watchdog_timer3_counter_mode(unsigned int reset_period_sec)
{
        unsigned int divfactor;
        double counter_per_sec;
        unsigned int tc_data;
        
        divfactor = REG32(CDBR)>>16;
        counter_per_sec = WATCHDOG_BASE_CLOCK/divfactor;
        tc_data = (unsigned int)((double)counter_per_sec*reset_period_sec);
        tc_data = tc_data<<4;
        REG32(TC3DATA) = tc_data & 0xFFFFFFF0;

        irq_install_handler(TIMER_ISR_3,(IRQ_FUN)watchdog_isr, NULL);
        //enable interrupt
        REG32(TCIR) = (0x1<<26);
        
        //enable timer counter mode
        REG32(TCCNR) = (1<<25) | (0<<24);

        return;
        
}

void watchdog_timer0_timer_mode(unsigned int reset_period_sec)
{
        unsigned int divfactor,duration_times;
        unsigned int  counter_per_sec;
        //unsigned int tc_data;
        unsigned int i;
        
        divfactor = REG32(CDBR)>>16;
        counter_per_sec = WATCHDOG_BASE_CLOCK/divfactor;
        //tc_data = (unsigned int)((double)counter_per_sec*reset_period_sec);
        //tc_data = tc_data<<4;
        //REG32(TC0DATA) = tc_data & 0xFFFFFFF0;

        //irq_install_handler(TIMER_ISR_0,(IRQ_FUN)watchdog_isr, NULL);
        //enable interrupt
        //REG32(TCIR) = 0x1<<31;
        irq_install_handler(TIMER_ISR_0,(IRQ_FUN)watchdog_isr, NULL);
        REG32(TCIR) = 0x1<<31;
        
        //enable timer counter mode
        REG32(TCCNR) = (1<<31) | (1<<30);
        for(i = 0; i < reset_period_sec; i++)
        {
                mdelay(1000);
        }
        
        //disable timer
        REG32(TCCNR) = 0;
        duration_times = REG32(TC0CNT) >> 4;
        duration_times = (duration_times/counter_per_sec);
        printf("REG32(CDBR) = 0x%x\n", REG32(CDBR));
        printf("duration_times = %d\n", duration_times );
        printf("counter_per_sec = 0x%x, TC0CNT = 0x%x\n", counter_per_sec,(REG32(TC0CNT)>>4));
        //clean counter
        REG32(TC0DATA) = 0;        
        return;
        
}

void watchdog_timer1_timer_mode(unsigned int reset_period_sec)
{
        unsigned int divfactor,duration_times;
        unsigned int  counter_per_sec;
        //unsigned int tc_data;
        unsigned int i;
        printf("Test timer1 timer mode\n");
        divfactor = REG32(CDBR)>>16;
        counter_per_sec = WATCHDOG_BASE_CLOCK/divfactor;
        //tc_data = (unsigned int)((double)counter_per_sec*reset_period_sec);
        //tc_data = tc_data<<4;
        //REG32(TC0DATA) = tc_data & 0xFFFFFFF0;

        //irq_install_handler(TIMER_ISR_0,(IRQ_FUN)watchdog_isr, NULL);
        //enable interrupt
        //REG32(TCIR) = 0x1<<31;
        irq_install_handler(TIMER_ISR_1,(IRQ_FUN)watchdog_isr, NULL);
        REG32(TCIR) = 0x1<<TC1_IE;
        
        //enable timer counter mode
        REG32(TCCNR) = (1<<TC1_EN) | (1<<TC1_MODE);
        for(i = 0; i < reset_period_sec; i++)
        {
                mdelay(1000);
        }
        
        //disable timer
        REG32(TCCNR) = 0;
        duration_times = REG32(TC1CNT) >> 4;
        duration_times = (duration_times/counter_per_sec);
        printf("duration_times = %d\n", duration_times );
        printf("counter_per_sec = 0x%x, TC1CNT = 0x%x\n", counter_per_sec,REG32(TC1CNT)>>4);
          //clean counter
        REG32(TC1DATA) = 0;  
        return;
        
}

void watchdog_timer2_timer_mode(unsigned int reset_period_sec)
{
        unsigned int divfactor,duration_times;
        unsigned int  counter_per_sec;
        //unsigned int tc_data;
        unsigned int i;
        printf("Test timer2 timer mode\n");
        divfactor = REG32(CDBR)>>16;
        counter_per_sec = WATCHDOG_BASE_CLOCK/divfactor;
        //tc_data = (unsigned int)((double)counter_per_sec*reset_period_sec);
        //tc_data = tc_data<<4;
        //REG32(TC0DATA) = tc_data & 0xFFFFFFF0;

        //irq_install_handler(TIMER_ISR_0,(IRQ_FUN)watchdog_isr, NULL);
        //enable interrupt
        //REG32(TCIR) = 0x1<<31;
        irq_install_handler(TIMER_ISR_2,(IRQ_FUN)watchdog_isr, NULL);
        REG32(TCIR) = 0x1<<TC2_IE;
        
        //enable timer counter mode
        REG32(TCCNR) = (1<<TC2_EN) | (1<<TC2_MODE);
        for(i = 0; i < reset_period_sec; i++)
        {
                mdelay(1000);
        }
        
        //disable timer
        REG32(TCCNR) = 0;
        duration_times = REG32(TC2CNT) >> 4;
        duration_times = (duration_times/counter_per_sec);
        printf("duration_times = %d\n", duration_times );
        printf("counter_per_sec = 0x%x, TC1CNT = 0x%x\n", counter_per_sec,REG32(TC2CNT)>>4);
          //clean counter
        REG32(TC2DATA) = 0;  
        return;
        
}

void watchdog_timer3_timer_mode(unsigned int reset_period_sec)
{
        unsigned int divfactor,duration_times;
        unsigned int  counter_per_sec;
        //unsigned int tc_data;
        unsigned int i;
        printf("Test timer3 timer mode\n");
        divfactor = REG32(CDBR)>>16;
        counter_per_sec = WATCHDOG_BASE_CLOCK/divfactor;
        //tc_data = (unsigned int)((double)counter_per_sec*reset_period_sec);
        //tc_data = tc_data<<4;
        //REG32(TC0DATA) = tc_data & 0xFFFFFFF0;

        //irq_install_handler(TIMER_ISR_0,(IRQ_FUN)watchdog_isr, NULL);
        //enable interrupt
        //REG32(TCIR) = 0x1<<31;
        irq_install_handler(TIMER_ISR_3,(IRQ_FUN)watchdog_isr, NULL);
        REG32(TCIR) = 0x1<<TC3_IE;
        
        //enable timer counter mode
        REG32(TCCNR) = (1<<TC3_EN) | (1<<TC3_MODE);
        for(i = 0; i < reset_period_sec; i++)
        {
                mdelay(1000);
        }
        
        //disable timer
        REG32(TCCNR) = 0;
        duration_times = REG32(TC3CNT) >> 4;
        duration_times = (duration_times/counter_per_sec);
        printf("REG32(CDBR) = 0x%x\n", REG32(CDBR));
        printf("duration_times = %d\n", duration_times );
        printf("counter_per_sec = 0x%x, TC3CNT = 0x%x\n", counter_per_sec,REG32(TC3CNT)>>4);
        //clean counter
        REG32(TC3DATA) = 0;  
        return;
        
}

void watchdog_test_entry(unsigned int reset_period_sec)
{
        #if 1
        watchdog_disable();
        watchdog_set_reset_period(reset_period_sec);
        watchdog_enable();
        #ifdef WATCHDOG_CLEAR_VERIFY
        while(1)
        {
                //Watchdog Clear.
                REG32(WTCR) = REG32(WTCR) | 0x800000;
                mdelay(1);
        }
        #endif        
        #else
        //set divisor
        REG32(CDBR) = 20 << 16;
        //watchdog_timer0_counter_mode(reset_period_sec);
        //watchdog_timer1_counter_mode(reset_period_sec);
        //watchdog_timer2_counter_mode(reset_period_sec);
        //watchdog_timer3_counter_mode(reset_period_sec);
        //watchdog_timer0_timer_mode(reset_period_sec);
        //watchdog_timer1_timer_mode(reset_period_sec);
        //watchdog_timer2_timer_mode(reset_period_sec);
        //watchdog_timer3_timer_mode(reset_period_sec);
        #endif
    
        return;
}

