/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_TIMER_EXTEND_BIT_H_
#define _DW_TIMER_EXTEND_BIT_H_

/* CDBR */
#define BIT_SHIFT_TIMER_DIV_FACTOR				16
#define BIT_TIMER_DIV_FACTOR					BIT(BIT_SHIFT_TIMER_DIV_FACTOR)
#define BIT_MASK_TIMER_DIV_FACTOR				0xffff
#define BIT_CTRL_TIMER_DIV_FACTOR(x)			(((x) & BIT_MASK_TIMER_DIV_FACTOR) << BIT_SHIFT_TIMER_DIV_FACTOR)

/* WDTCNR*/
#define BIT_SHIFT_TIMER_OVSEL_L					21
#define BIT_TIMER_OVSEL_L						BIT(BIT_SHIFT_TIMER_OVSEL_L)
#define BIT_MASK_TIMER_OVSEL_L					0x3
#define BIT_CTRL_TIMER_OVSEL_L(x)				(((x) & BIT_MASK_TIMER_OVSEL_L) << BIT_SHIFT_TIMER_OVSEL_L)

#define BIT_SHIFT_TIMER_OVSEL_H					17
#define BIT_TIMER_OVSEL_H						BIT(BIT_SHIFT_TIMER_OVSEL_H)
#define BIT_MASK_TIMER_OVSEL_H					0x3
#define BIT_CTRL_TIMER_OVSEL_H(x)				(((x) & BIT_MASK_TIMER_OVSEL_H) << BIT_SHIFT_TIMER_OVSEL_H)

#define BIT_CTRL_TIMER_OVSEL(x)					( BIT_CTRL_TIMER_OVSEL_H(x>>2)| BIT_CTRL_TIMER_OVSEL_L(x))


typedef enum _TIMER_OVSEL_ {
	TIMER_OVSEL_2_15			= 0,
	TIMER_OVSEL_2_16			= 1,
	TIMER_OVSEL_2_17			= 2,
	TIMER_OVSEL_2_18			= 3,
	TIMER_OVSEL_2_19			= 4,
	TIMER_OVSEL_2_20			= 5,
	TIMER_OVSEL_2_21			= 6,
	TIMER_OVSEL_2_22			= 7,
	TIMER_OVSEL_2_23			= 8,
	TIMER_OVSEL_2_24			= 9
}TIMER_OVSEL, *PTIMER_OVSEL;

#define BIT_SHIFT_TIMER_WDTCLR					23
#define BIT_TIMER_WDTCLR						BIT(BIT_SHIFT_TIMER_WDTCLR)
#define BIT_MASK_TIMER_WDTCLR					0x1
#define BIT_CTRL_TIMER_WDTCLR(x)				(((x) & BIT_MASK_TIMER_WDTCLR) << BIT_SHIFT_TIMER_WDTCLR)

#define BIT_SHIFT_TIMER_WDTE					24
#define BIT_TIMER_WDTE							BIT(BIT_SHIFT_TIMER_WDTE)
#define BIT_MASK_TIMER_WDTE						0xff
#define BIT_CTRL_TIMER_WDTE(x)					(((x) & BIT_MASK_TIMER_WDTE) << BIT_SHIFT_TIMER_WDTE)

#define TIMER_WDTE_DEFAULT						0xA5

#endif

