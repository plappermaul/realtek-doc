/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_PWM_TEST_H_
#define _RTK_PWM_TEST_H_

typedef struct _PWM_VERI_PARA_ {
        u32         VeriItem;
        u32         VeriLoop;
		u32			TimerSel; //select timer source
		u32			TimerValueUs; //timer count down value
		u32			PWM_GTE_Sel;
}PWM_VERI_PARA,*PPWM_VERI_PARA;


typedef enum _PWM_VERI_ITEM_ {
        PWM_TEST     = 1,    
		GTEvent_TEST = 2
}PWM_VERI_ITEM, *PPWM_VERI_ITEM;

#endif
