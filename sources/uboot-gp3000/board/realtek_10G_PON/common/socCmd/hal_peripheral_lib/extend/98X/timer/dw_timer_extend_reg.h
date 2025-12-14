
/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _DW_TIMER_EXTEND_REG_H_
#define _DW_TIMER_EXTEND_REG_H_

/* Timer control registers */
#define REG_TIMER_CDBR_REG       (0x18)       /* Clock division base */
#define REG_TIMER_WDTCNR_REG     (0x1C)       /* Watchdog timer control */

/* Change Cpu Frequency registers */
#define REG_INT_PENDING_STATUS              (0x4)
#define REG_HW_STRAP                        (0x8)

#define BIT_INT_CPUWAKE                     1
#define BIT_SHIFT_STRAP_PIN_CK_CPU_FREQ_SEL 15
#define BIT_MASK_STRAP_PIN_CK_CPU_FREQ_SEL  0xf
#define BIT_STRAP_PIN_CK_CPU_FREQ_SEL(x)    (((x) & BIT_MASK_STRAP_PIN_CK_CPU_FREQ_SEL) << BIT_SHIFT_STRAP_PIN_CK_CPU_FREQ_SEL)

#endif //#ifndef _DW_TIMER_EXTEND_REG_H_


