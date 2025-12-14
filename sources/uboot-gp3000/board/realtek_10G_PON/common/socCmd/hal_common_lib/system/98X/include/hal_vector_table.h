/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _HAL_VECTOR_TABLE_H_
#define _HAL_VECTOR_TABLE_H_
#include "basic_types.h"
#include "hal_platform.h"




VOID
VectorTableInitRtl8195A(
    IN  u32 StackP
);

VOID
VectorTableInitForOSRtlCommon(
    IN  VOID *PortSVC,
    IN  VOID *PortPendSVH,
    IN  VOID *PortSysTick    
);

BOOL
VectorIrqRegisterRtlCommon(
    IN  PIRQ_HANDLE pIrqHandle
);

BOOL
VectorIrqUnRegisterRtlCommon(
    IN  PIRQ_HANDLE pIrqHandle
);


VOID
VectorIrqEnRtlCommon(
    IN  PIRQ_HANDLE pIrqHandle
);

VOID
VectorIrqDisRtlCommon(
    IN  PIRQ_HANDLE pIrqHandle
);

 
VOID
HalPeripheralIntrHandle(VOID);
#endif //_HAL_VECTOR_TABLE_H_
