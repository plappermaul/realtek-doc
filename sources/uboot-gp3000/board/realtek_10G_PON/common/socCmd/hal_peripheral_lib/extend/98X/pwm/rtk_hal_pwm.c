/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtk_hal_pwm.h"

VOID HalPWMOpInit(
    IN  VOID *Data
)
{

    PHAL_PWM_OP pHalPWMOp = (PHAL_PWM_OP) Data;

    pHalPWMOp->HalPWMEn = HalPWMEnCommon;
    pHalPWMOp->HalPWMInit = HalPWMInitCommon;
	
}

VOID HalGTEventOpInit(
    IN  VOID *Data
)
{

    PHAL_GTEVENT_OP pHalGTEOp = (PHAL_GTEVENT_OP) Data;

	pHalGTEOp->HalGTEventEn = HalGTEventEnCommon;
	pHalGTEOp->HalGTEventInit = HalGTEventInitCommon;
	
}

