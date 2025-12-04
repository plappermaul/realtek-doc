/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_PWM_H_
#define _RTK_PWM_H_

#include "hal_api.h"
#include "rtk_pwm_base_bit.h"
#include "rtk_pwm_base_reg.h"

VOID
HalPWMWrite32(
    IN  u8      PWMIdx,
    IN  u32     PWMVal
);


u32
HalPWMRead32(
    IN  u8      PWMIdx
);

VOID
HalGTEventWrite8(
    IN  u8     PWMIdx,
    IN  u8     PWMVal
);


u8
HalGTEventRead8(
    IN  u8      PWMIdx
);


#define HAL_PWM_WRITE32(PWMIdx, value)    HalPWMWrite32(PWMIdx,value)
#define HAL_PWM_READ32(PWMIdx)            HalPWMRead32(PWMIdx)

#define HAL_GTEVENT_WRITE8(PWMIdx, value)    HalGTEventWrite8(PWMIdx,value)
#define HAL_GTEVENT_READ8(PWMIdx)            HalGTEventRead8(PWMIdx)



//4 PWM Selection
typedef enum _PWM_SEL_ {
        PWM0_SEL    =   0x0,
        PWM1_SEL    =   0x1,
        PWM2_SEL    =   0x2,
        PWM3_SEL    =   0x3,
}PWM_SEL,*PPWM_SEL;

//4 GT Event Selection
typedef enum _GTE_SEL_ {
        GTE0_SEL    =   0x0,
        GTE1_SEL    =   0x1,
        GTE2_SEL    =   0x2,
        GTE3_SEL    =   0x3,
}GTE_SEL,*PGTE_SEL;


BOOL
HalPWMEnCommon(
    IN  VOID    *Data
);
BOOL
HalPWMInitCommon(
	IN	VOID	*Data
);
BOOL
HalGTEventEnCommon(
	IN	VOID	*Data
);
BOOL
HalGTEventInitCommon(
	IN	VOID	*Data
);

#endif

