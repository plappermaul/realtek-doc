/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "platform_autoconf.h" 
#include "diag.h"
#include "hal_api.h"
//#include "rtk_pwm_base.h"
//#include "rtk_hal_pwm.h"
#include "peripheral.h"

VOID
HalPWMWrite32(
    IN  u8      PWMIdx,
    IN  u32     PWMVal
){
    switch(PWMIdx)
    {
        case PWM0_SEL:
            HAL_WRITE32(PWM0_REG_BASE,0,PWMVal);
            break;
            
        case PWM1_SEL:
            HAL_WRITE32(PWM1_REG_BASE,0,PWMVal);
            break;
            
        case PWM2_SEL:
            HAL_WRITE32(PWM2_REG_BASE,0,PWMVal);
            break;
            
        case PWM3_SEL:
            HAL_WRITE32(PWM3_REG_BASE,0,PWMVal);
            break;

        default:
            break;
            
    }
    
}

u32
HalPWMRead32(
    IN  u8      PWMIdx
){
    u32 PWMDatRd = 0;
    
    switch(PWMIdx)
    {
        case PWM0_SEL:
            PWMDatRd = HAL_READ32(PWM0_REG_BASE,0);
            break;
            
        case PWM1_SEL:
            PWMDatRd = HAL_READ32(PWM1_REG_BASE,0);
            break;
            
        case PWM2_SEL:
            PWMDatRd = HAL_READ32(PWM2_REG_BASE,0);
            break;
            
        case PWM3_SEL:
            PWMDatRd = HAL_READ32(PWM3_REG_BASE,0);
            break;

        default:
            break;
            
    }

    return PWMDatRd;
}

BOOL
HalPWMEnCommon(
    IN  VOID    *Data
)
{
    PHAL_PWM_DAT_ADAPTER pHalPWMDatAdapter = (PHAL_PWM_DAT_ADAPTER) Data;
    u8  PWMIdx;
    u8  PWMICEn;

    PWMIdx = pHalPWMDatAdapter->Idx;
    PWMICEn = pHalPWMDatAdapter->En;

    
   HAL_PWM_WRITE32(PWMIdx, (HAL_PWM_READ32(PWMIdx) & ~BIT_PWM_EN) | BIT_CTRL_PWM_EN(PWMICEn));
   //HalPWMWrite32(PWMIdx, HAL_PWM_READ32(PWMIdx) | BIT_CTRL_PWM_EN(PWMICEn));

    return _TRUE;
}

BOOL
HalPWMInitCommon(
	IN	VOID	*Data
)
{
	PHAL_PWM_DAT_ADAPTER pHalPWMDatAdapter = (PHAL_PWM_DAT_ADAPTER) Data;
	u8  PWMIdx;
	u8 PWMGT; 
	u8 PWMDuty;
	u8 PWMPeriod;

	PWMIdx = pHalPWMDatAdapter->Idx;
	PWMGT = pHalPWMDatAdapter->GT_SEL;
	PWMDuty = pHalPWMDatAdapter->Duty;
	PWMPeriod = pHalPWMDatAdapter->Period;

	HAL_PWM_WRITE32(PWMIdx, BIT_CTRL_PWM_GT_SEL(PWMGT) | BIT_CTRL_PWM_DUTY(PWMDuty) | BIT_CTRL_PWM_PERIOD(PWMPeriod));
	//HalPWMWrite32(PWMIdx, BIT_CTRL_PWM_GT_SEL(PWMGT) | BIT_CTRL_PWM_DUTY(PWMDuty) | BIT_CTRL_PWM_PERIOD(PWMPeriod));
	return _TRUE;
	
}

VOID
HalGTEventWrite8(
    IN  u8      GTEIdx,
    IN  u8     GTEVal
){

	u32 GTEmask = 0;
	u32 GTEshift = 0;

    switch(GTEIdx)
    {
        case GTE0_SEL:
            GTEmask = 0xFF;
			GTEshift = 0;
            break;
            
        case GTE1_SEL:
            GTEmask = 0xFF00;
			GTEshift = 8;
            break;
            
        case GTE2_SEL:
            GTEmask = 0xFF0000;
			GTEshift = 16;
            break;
            
        case GTE3_SEL:
            GTEmask = 0xFF000000;
			GTEshift = 24;
            break;

        default:
            break;
            
    }
	
	HAL_WRITE32(GTEVENT_REG_BASE,REG_TIM_EVENT_CTRL,(HAL_READ32(GTEVENT_REG_BASE, REG_TIM_EVENT_CTRL) & ~GTEmask) | (GTEVal << GTEshift));
	
}

u8
HalGTEvevntRead8(
    IN  u8      GTEIdx
){
   u32 GTEmask = 0;
   u8 GTEDatRd = 0;
   u32 GTEshift;
   
    switch(GTEIdx)
    {
        case GTE0_SEL:
            GTEmask = 0xFF;
			GTEshift = 0;
            break;
			
		case GTE1_SEL:
		   	GTEmask = 0xFF00;
			GTEshift = 8;
		   	break;
		   
	   	case GTE2_SEL:
		   	GTEmask = 0xFF0000;
			GTEshift = 16;
		   	break;
		   
	   	case GTE3_SEL:
		   	GTEmask = 0xFF000000;
			GTEshift = 24;
		   	break;
        
        default:
            break;
            
    }

	GTEDatRd = (HAL_READ32(GTEVENT_REG_BASE, REG_TIM_EVENT_CTRL) & GTEmask) >> GTEshift;

    return GTEDatRd;
}


BOOL
HalGTEventEnCommon(
	IN	VOID	*Data
)
{
	PHAL_GTEVENT_DAT_ADAPTER pHalGTEventDatAdapter = (PHAL_GTEVENT_DAT_ADAPTER) Data;
	
	u8	GTEventIdx;
   	u8  GTEventEn;

   	GTEventIdx = pHalGTEventDatAdapter->Idx;
   	GTEventEn = pHalGTEventDatAdapter->En;

	
	//HalGTEventWrite8(GTEventIdx, HalGTEvevntRead8(GTEventIdx) | BIT_CTRL_GT_EVT_EN(GTEventEn));
	HalGTEventWrite8(GTEventIdx, ((HalGTEvevntRead8(GTEventIdx)& ~BIT_GT_EVT_EN) | BIT_CTRL_GT_EVT_EN(GTEventEn)));

	return _TRUE;
}

BOOL
HalGTEventInitCommon(
	IN	VOID	*Data
)
{
	PHAL_GTEVENT_DAT_ADAPTER pHalGTEventDatAdapter = (PHAL_GTEVENT_DAT_ADAPTER) Data;
	u8	GTEventIdx;
	u8	GTESrcSel;
	u8	GTEPulseDur;
	
	GTEventIdx = pHalGTEventDatAdapter->Idx;
	GTESrcSel = pHalGTEventDatAdapter->Src_Sel;
	GTEPulseDur = pHalGTEventDatAdapter->Pulse_dur;
	
	HalGTEventWrite8(GTEventIdx, BIT_CTRL_GT_EVT_SRC_SEL(GTESrcSel) | BIT_CTRL_GT_EVT_PULSE_DUR(GTEPulseDur));
	
	return _TRUE;
	
}

