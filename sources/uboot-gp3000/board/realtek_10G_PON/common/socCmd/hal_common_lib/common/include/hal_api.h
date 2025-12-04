/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _HAL_API_H_
#define _HAL_API_H_

#include "basic_types.h"
#include "hal_platform.h"
#include "hal_vector_table.h"
//#include "hal_timer.h"
#include "hal_diag.h"

#include "hal_vector_table_ram.h"


#define HAL_READ32(base, addr)            \
        rtk_le32_to_cpu(*((volatile u32*)(base + addr)))

#define HAL_WRITE32(base, addr, value32)  \
        ((*((volatile u32*)(base + addr))) = rtk_cpu_to_le32(value32))


#define HAL_READ16(base, addr)            \
        rtk_le16_to_cpu(*((volatile u16*)(base + addr)))

#define HAL_WRITE16(base, addr, value)  \
        ((*((volatile u16*)(base + addr))) = rtk_cpu_to_le16(value))


#define HAL_READ8(base, addr)            \
        (*((volatile u8*)(base + addr)))

#define HAL_WRITE8(base, addr, value)  \
        ((*((volatile u8*)(base + addr))) = value)


// For system register R/W
#define HAL_READ32_SYS(addr)            \
        rtk_le32_to_cpu(*((volatile u32*)(SYSTEM_REG_BASE + addr)))

#define HAL_WRITE32_SYS(addr, value32)  \
        ((*((volatile u32*)(SYSTEM_REG_BASE + addr))) = rtk_cpu_to_le32(value32))


// For memory address directly R/W
#define HAL_READ32_MEM(addr)            \
        rtk_le32_to_cpu(*((volatile u32*)(addr)))

#define HAL_WRITE32_MEM(addr, value32)  \
        ((*((volatile u32*)(addr))) = rtk_cpu_to_le32(value32))

#define HAL_READ8_MEM(addr)            \
        (*((volatile u8*)(addr)))

#define HAL_WRITE8_MEM(addr, value)  \
        ((*((volatile u8*)(addr))) = value)


#define PinCtrl HalPinCtrlRtl8195A

#if 0 // 8195a ameba
#define DiagPutChar	HalSerialPutcRtl8195a
#else // 8198X APSOC
//#define DiagPutChar    dwc_serial_putc
#define DiagPutChar    cortina_serial_putc
#endif
#define DiagGetChar HalSerialGetcRtl8195a
#define DiagGetIsrEnReg HalSerialGetIsrEnRegRtl8195a
#define DiagSetIsrEnReg HalSerialSetIrqEnRegRtl8195a

#if 0
#define InterruptForOSInit VectorTableInitForOSRtl8195A
#define InterruptRegister VectorIrqRegisterRtl8195A
#define InterruptUnRegister  VectorIrqUnRegisterRtl8195A

#define InterruptEn VectorIrqEnRtl8195A
#define InterruptDis VectorIrqDisRtl8195A
#endif

#define InterruptForOSInit VectorTableInitForOSRtlCommon
#define InterruptRegister VectorIrqRegisterRtlCommon
#define InterruptUnRegister  VectorIrqUnRegisterRtlCommon

#define InterruptEn VectorIrqEnRtlCommon
#define InterruptDis VectorIrqDisRtlCommon


#define SpicFlashInit SpicFlashInitRtl8195A
#endif //_HAL_API_H_
