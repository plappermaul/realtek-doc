/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _DW_TIMER_BASE_BIT_H_
#define _DW_TIMER_BASE_BIT_H_

#define BIT_TIMER_CTL_INT_MASK           	BIT(2)
#define BIT_SHIFT_TIMER_CTL_INT_MASK   		2
#define BIT_MASK_TIMER_CTL_INT_MASK      	0x1
#define BIT_CTRL_TIMER_CTL_INT_MASK(x)   	(((x) & BIT_MASK_TIMER_CTL_INT_MASK) << BIT_SHIFT_TIMER_CTL_INT_MASK)

#define BIT_TIMER_CTL_TIMER_MODE              	BIT(1)
#define BIT_SHIFT_TIMER_CTL_TIMER_MODE        	1
#define BIT_MASK_TIMER_CTL_TIMER_MODE         	0x1
#define BIT_CTRL_TIMER_CTL_TIMER_MODE(x)      	(((x) & BIT_MASK_TIMER_CTL_TIMER_MODE) << BIT_SHIFT_TIMER_CTL_TIMER_MODE)

#define BIT_TIMER_CTL_TIMER_ENABLE        	BIT(0)
#define BIT_SHIFT_TIMER_CTL_TIMER_ENABLE  	0
#define BIT_MASK_TIMER_CTL_TIMER_ENABLE   	0x1
#define BIT_CTRL_TIMER_CTL_TIMER_ENABLE(x)	(((x) & BIT_MASK_TIMER_CTL_TIMER_ENABLE) << BIT_SHIFT_TIMER_CTL_TIMER_ENABLE)

#endif //_DW_TIMER_BASE_BIT_H_
