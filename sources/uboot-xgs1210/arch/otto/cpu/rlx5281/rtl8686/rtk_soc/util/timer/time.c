/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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

#include <common.h>
//#include "../../../drivers/rtl8652_asicregs.h"
#include "timer_reg.h"
void rtosTickIsr(void) { return; }
static inline void mips_compare_set(u32 v)
{
	return;
}


#if 0
static inline void mips_count_set(u32 v)
{
        REG32(TC0CNT) = v << 8;
}


static inline u32 mips_count_get(void)
{
        u32 tt;

        tt = REG32(TC0CNT) >> 8;
        return tt;

}
int timer_init(void)
{

        REG32(TCCNR) = 0; /*Reset timer or counter control register.*/
        REG32(CDBR) = (CFG_LXBUS_HZ/1000000) << DIVF_OFFSET;
        REG32(TC0DATA) = 0xFFFFFF << 8;
        REG32(TCCNR) = TC0EN | TC0MODE_TIMER; /*Enable timer 0 or counter 0 control register.*/
        /* Enable timer interrupt */
        REG32(TCIR) = TC0IE;
        return 0;
}
#else
static inline void mips_count_set(u32 v)
{
	REG32(TC0CNT) = v << TC_VALUE_OFFSET;
}


static inline u32 mips_count_get(void)
{
	u32 tt;

	tt = REG32(TC0CNT) >> TC_VALUE_OFFSET;
	return tt;
	
}

int timer_init(void)
{
	REG32(TCCNR) = 0; /*Reset timer or counter control register.*/
	REG32(CDBR) = (CFG_LXBUS_HZ/1000000) << DIVF_OFFSET;
	REG32(TC0DATA) = 0xFFFFFFF << TC_VALUE_OFFSET;
	REG32(TCCNR) = TC0EN | TC0MODE_TIMER; /*Enable timer 0 or counter 0 control register.*/
	/* Enable timer interrupt */
	REG32(TCIR) = TC0IE;
	return 0;
}
#endif

void reset_timer(void)
{
	timer_init();
}

ulong get_timer(ulong base)
{
	return mips_count_get() - base;
}

void udelay (unsigned long usec)
{
	ulong tmo;
	//ulong j;
	
	reset_timer();
	tmo = get_timer(0);
	while(usec > tmo){
		tmo = get_timer(0);
	}

}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On MIPS it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return mips_count_get();
}

/*
 *  * This function is derived from PowerPC code (timebase clock frequency).
 *   * On MIPS it returns the number of timer ticks per second.
 *    */
ulong get_tbclk(void)
{
        return CFG_LXBUS_HZ/CFG_LXBUS_MHZ;
}

#if 0

void tick_IntHandler(void)
{
    if ( REG32(TCIR) & TC0IP )
    {	
        REG32(TCIR) = TC0IP | TC0IE;
        SysUpTime++;
        rtosTickIsr();
    }
}

static void tick_testIntHandler()
{
    if ( REG32(TCIR) & TC0IP )
        timer_test |= 1;
    if ( REG32(TCIR) & TC1IP )
        timer_test |= 2;
    REG32(TCIR) = TC0IP | TC1IP;
}

uint64 tick_GetSysUpTime(uint64 *systime)
{
    uint32        imask;

    imask = setIlev(TICK_ILEV + 1);
        *systime = SysUpTime;
    setIlev(imask);

        return (*systime);
}
void tick_Delay10ms(uint32 ten_msec)
{
        int i;
        for(i=0;i<ten_msec;i++)
        {
                tick_pollStart();
                while(tick_pollGet10MS()==0);
        }
}
void tick_Delay100ms(uint32 hundred_msec)
{
        int i;
        for(i=0;i<(hundred_msec<<3)+(hundred_msec<<1);i++)
        {
                tick_pollStart();
                while(tick_pollGet10MS()==0);
        }
}

/* This function is used when interrupts must be disabled */
static uint32 pstime;
static uint32 p10ms;
static uint32 ptoggle;

void tick_pollStart(void)
{
    pstime = REG32(TC0CNT);
    p10ms = 0;
    ptoggle = 0;
}

uint32 tick_pollGet10MS(void)
{
    if (REG32(TC0CNT) > pstime)
        ptoggle = 1;
    else if (ptoggle == 1)
    {
        p10ms++;
        ptoggle = 0;
    }

    return p10ms;
}

#endif 
