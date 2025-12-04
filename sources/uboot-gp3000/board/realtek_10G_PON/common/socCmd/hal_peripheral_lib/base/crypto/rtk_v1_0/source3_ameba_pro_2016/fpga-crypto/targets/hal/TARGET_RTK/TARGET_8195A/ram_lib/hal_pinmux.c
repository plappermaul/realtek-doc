/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"

VOID HalJtagPinOff(VOID)
{
    PinCtrl(JTAG, S0, OFF);
}

#if 0
VOID HalJtagPinOn(VOID)
{
    PinCtrl(JTAG, S0, ON);
}
#endif

extern u16 GPIOState[11];

u8
GpioIcFunChk(
    IN u32  chip_pin
)
{
    u8 chip_id;

    HALEFUSEOneByteReadRAM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), 0xF8, &chip_id, L25EOUTVOLTAGE);

    switch (chip_id) {
        case CHIP_ID_8710AF:
            if ((chip_pin == 0x1)||(chip_pin == 0x2)||(chip_pin == 0x3)||(chip_pin == 0x25)) {
                return _FALSE;
            } else {
                return _TRUE;
            }
            break;

        case CHIP_ID_8711AM:
        case CHIP_ID_8195AM:
        case CHIP_ID_8711AF:
        case CHIP_ID_8711AN:
        case CHIP_ID_8710AM:
            return _TRUE;
            break;

        default:
            return _FALSE;
            break;
    }
}

u8
GpioFunctionChk(
    IN u32  chip_pin,
    IN u8   Operation)
{
    u8   GPIOPortIdx = (((u8)chip_pin)>>4);
    u16  GPIOIdx = (BIT0 << (chip_pin & 0xf));
    u8 ret = GpioIcFunChk(chip_pin);

    if (ret == _FALSE) {
        return _FALSE;
    }

    if (Operation) {

        if (GPIOState[GPIOPortIdx] & GPIOIdx) {

            return _FALSE;
        } else {
            GPIOState[GPIOPortIdx] = (GPIOState[GPIOPortIdx] | GPIOIdx);
        }
    } else {
        GPIOState[GPIOPortIdx] = (GPIOState[GPIOPortIdx] & (~GPIOIdx));
    }

    return _TRUE;
}

u8
RTL8710afFunChk(
    IN u32  Function,
    IN u32  PinLocation
)
{
    switch (Function) {
        case JTAG:
            return _TRUE;
            break;

        case UART0:
            if (PinLocation == S0) {
                return _TRUE;
            } else {
                return _FALSE;
            }
            break;

        case LOG_UART:
            return _TRUE;
            break;

        case I2C3:
            if (PinLocation == S0) {
                return _TRUE;
            } else {
                return _FALSE;
            }
            break;

        case SPI0:
            if (PinLocation == S1) {
                return _TRUE;
            } else {
                return _FALSE;
            }
            break;

        case SPI0_MCS:
            if (PinLocation == S1) {
                return _TRUE;
            } else {
                return _FALSE;
            }
            break;

        case I2S1:
            if (PinLocation == S0) {
                return _TRUE;
            } else {
                return _FALSE;
            }
            break;

        default :
            return _FALSE;
            break;
    }
}

u8
FunctionChk(
    IN u32  Function,
    IN u32  PinLocation
)
{
    u8 chip_id;
    HALEFUSEOneByteReadRAM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), 0xF8, &chip_id, L25EOUTVOLTAGE);

    switch (chip_id) {
        case CHIP_ID_8710AF:
            return RTL8710afFunChk(Function, PinLocation);
            break;

        case CHIP_ID_8711AM:
        case CHIP_ID_8195AM:
        case CHIP_ID_8711AF:
        case CHIP_ID_8711AN:
        case CHIP_ID_8710AM:
            return _TRUE;
            break;

        default:
            return _FALSE;
            break;
    }
}