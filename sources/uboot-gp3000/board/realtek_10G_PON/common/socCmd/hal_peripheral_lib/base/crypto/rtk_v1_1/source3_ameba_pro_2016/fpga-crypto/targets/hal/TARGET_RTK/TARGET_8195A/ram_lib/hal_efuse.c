/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"
#include "hal_efuse.h"

#ifdef CONFIG_EFUSE_EN
#define EFUSELEN    127
#define WEFUSELEN   127
#define REFUSELEN   160
#define RFSECTION   0xD4
#define USERSECTION 44
#define OTPSECTION  0x80
#define EOTPSECTION 32

#define USER1  0
#define USER2  1
#define USER3  2


VOID
ReadEfuseContant(
    IN  u8 UserCode,
    OUT u8 *pContant
)
{
    u8 Idx = 0;
    u8 DataTemp0, DataTemp1, Addr, WordEn, Offset;

    while(Idx < EFUSELEN) {

        EFUSERead8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, &DataTemp0, L25EOUTVOLTAGE);

        if (DataTemp0 != 0xff) {

            if((DataTemp0&0x0f) == 0xf) {

                Idx++;
                EFUSERead8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, &DataTemp1, L25EOUTVOLTAGE);
                Addr = (((DataTemp1&0xf0)|((DataTemp0)>>4))>>1);
                WordEn = ((~DataTemp1)&0x0f);

                if ((Addr > (USERSECTION+UserCode*4 -1))&&(Addr < (USERSECTION+UserCode*4 +4))) {
                    Offset = 0;
                    while(WordEn!=0) {
                        if ((WordEn & BIT0)) {
                            Idx++;
                            EFUSERead8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, (pContant+(Addr-(USERSECTION+UserCode*4))*8+Offset), L25EOUTVOLTAGE);
                            Idx++;
                            EFUSERead8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, (pContant+(Addr-(USERSECTION+UserCode*4))*8+Offset+1), L25EOUTVOLTAGE);
                        }
                        WordEn = WordEn>>1;
                        Offset = Offset+2;
                    }
                } else {
                    while(WordEn!=0) {
                        if (WordEn & BIT0) {
                            Idx = Idx + 2;
                        }
                        WordEn = WordEn>>1;
                    }
                }
            } else {
                WordEn = ((~DataTemp0)&0x0f);
                while(WordEn!=0) {
                    if (WordEn & BIT0) {
                        Idx = Idx + 2;
                    }
                    WordEn = WordEn>>1;
                }
            }
        } else {
            break;
        }

        Idx++;
    }
}


VOID
ReadEfuseContant1(
    OUT u8 *pContant
)
{
    ReadEfuseContant(USER1, pContant);
}

VOID
ReadEfuseContant2(
    OUT u8 *pContant
)
{
    ReadEfuseContant(USER2, pContant);
}

VOID
ReadEfuseContant3(
    OUT u8 *pContant
)
{
    ReadEfuseContant(USER3, pContant);
}


u8
GetRemainingEfuseLength(
    void
)
{
    u8 Idx = 0;
    u8 DataTemp0, WordEn;

    //find start add
    while(Idx < EFUSELEN) {

        EFUSERead8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, &DataTemp0, L25EOUTVOLTAGE);

        if (DataTemp0 != 0xff) {

            if((DataTemp0&0x0f) == 0xf) {

                Idx++;
                EFUSERead8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, &DataTemp0, L25EOUTVOLTAGE);
                WordEn = ((~DataTemp0)&0x0f);

                while(WordEn!=0) {
                    if (WordEn & BIT0) {
                        Idx = Idx + 2;
                    }
                    WordEn = WordEn>>1;
                }
            } else {
                WordEn = ((~DataTemp0)&0x0f);
                while(WordEn!=0) {
                    if (WordEn & BIT0) {
                        Idx = Idx + 2;
                    }
                    WordEn = WordEn>>1;
                }
            }
        } else {
            break;
        }

        Idx++;
    }

    return (EFUSELEN - Idx);
}


u8
WriteEfuseContant(
    IN u8 UserCode,
    IN u8 CodeWordNum,
    IN u8 WordEnable,
    IN u8 *pContant
)
{
    u8 Idx = 0;
    u8 DataTemp0, DataTemp1, Addr, WordEn, Offset, IdxTemp, Laddr;
    u8 Result = _FAIL;
    u8 Len = 0;

    if ((WordEnable&0xf) == 0) {
        return Result;
    }

    //find start add
    while(Idx < EFUSELEN) {

        EFUSERead8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, &DataTemp0, L25EOUTVOLTAGE);

        if (DataTemp0 != 0xff) {

            if((DataTemp0&0x0f) == 0xf) {

                Idx++;
                EFUSERead8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, &DataTemp1, L25EOUTVOLTAGE);
                Addr = (((DataTemp1&0xf0)|((DataTemp0)>>4))>>1);
                WordEn = ((~DataTemp1)&0x0f);

                while(WordEn!=0) {
                    if (WordEn & BIT0) {
                        Idx = Idx + 2;
                    }
                    WordEn = WordEn>>1;
                }
            } else {
                WordEn = ((~DataTemp0)&0x0f);
                while(WordEn!=0) {
                    if (WordEn & BIT0) {
                        Idx = Idx + 2;
                    }
                    WordEn = WordEn>>1;
                }
            }
        } else {
            break;
        }

        Idx++;
    }

    //CODE WORD
    CodeWordNum = CodeWordNum & 0x3;
    WordEnable = WordEnable & 0xf;
    WordEn = ((~WordEnable) & 0xf);

    while(WordEnable != 0) {
        if (WordEnable & BIT0) {
            Len = Len + 2;
        }
        WordEnable = WordEnable >> 1;
    }

    if ((Idx + Len + 2) < (EFUSELEN)) {

        Laddr = USERSECTION+UserCode*4;

        EFUSEWrite8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, (((Laddr<<5)|0x0f)+(CodeWordNum << 5)), L25EOUTVOLTAGE);
        Idx++;
        EFUSEWrite8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, (((Laddr<<1)&0xf0)|WordEn), L25EOUTVOLTAGE);
        Idx++;

        for(IdxTemp = 0; IdxTemp < Len; IdxTemp++) {
            EFUSEWrite8(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), Idx, *(pContant+IdxTemp), L25EOUTVOLTAGE);
            Idx++;
        }

        Result = _SUCCESS;
    }

    return Result;
}

u8
WriteEfuseContant1(
    IN u8 CodeWordNum,
    IN u8 WordEnable,
    IN u8 *pContant
)
{
    return WriteEfuseContant(USER1, CodeWordNum, WordEnable, pContant);
}

u8
WriteEfuseContant2(
    IN u8 CodeWordNum,
    IN u8 WordEnable,
    IN u8 *pContant
)
{
    return WriteEfuseContant(USER2, CodeWordNum, WordEnable, pContant);
}

u8
WriteEfuseContant3(
    IN u8 CodeWordNum,
    IN u8 WordEnable,
    IN u8 *pContant
)
{
    return WriteEfuseContant(USER3, CodeWordNum, WordEnable, pContant);
}


_LONG_CALL_ extern u32 HALEFUSEOneByteReadROM(IN	u32 CtrlSetting, IN	u16 Addr, OUT u8 *Data, IN u8 L25OutVoltage);
_LONG_CALL_ extern u32 HALEFUSEOneByteWriteROM(IN	u32 CtrlSetting, IN	u16 Addr, IN u8 Data, IN u8 L25OutVoltage);


u32 HALEFUSEOneByteReadRAM(
    IN	u32 CtrlSetting,
    IN	u16 Addr,
    OUT u8 *Data,
    IN  u8 L25OutVoltage
)
{
    u32 bResult = _SUCCESS;

    if ((Addr > (REFUSELEN-1))&&(Addr < RFSECTION)) {
        *Data = 0xFF;
    } else {
        bResult = HALEFUSEOneByteReadROM(CtrlSetting, Addr, Data, L25OutVoltage);
    }

    return bResult;
}

u32 HALEFUSEOneByteWriteRAM(
    IN u32 CtrlSetting,
    IN u16 Addr,
    IN u8 Data,
    IN u8 L25OutVoltage
)
{
    u32 bResult = _SUCCESS;

    if ((Addr > (WEFUSELEN-1))&&(Addr < RFSECTION)) {
    } else {
        bResult = HALEFUSEOneByteWriteROM(CtrlSetting, Addr, Data, L25OutVoltage);
    }

    return bResult;
}


u32 HALOTPOneByteReadRAM(
    IN	u32 CtrlSetting,
    IN	u16 Addr,
    OUT u8 *Data,
    IN  u8 L25OutVoltage
)
{
    u32 bResult = _SUCCESS;

    if ((Addr >= (OTPSECTION))&&(Addr < (OTPSECTION+EOTPSECTION))) {

        bResult = HALEFUSEOneByteReadROM(CtrlSetting, Addr, Data, L25OutVoltage);
    }

    return bResult;
}

u32 HALOTPOneByteWriteRAM(
    IN u32 CtrlSetting,
    IN u16 Addr,
    IN u8 Data,
    IN u8 L25OutVoltage
)
{
    u32 bResult = _SUCCESS;

    if ((Addr >= (OTPSECTION))&&(Addr < (OTPSECTION+EOTPSECTION))) {

        bResult = HALEFUSEOneByteWriteROM(CtrlSetting, Addr, Data, L25OutVoltage);
    }

    return bResult;
}


VOID
ReadEOTPContant(
    OUT u8 *pContant
)
{
    u8 Idx = 0;

    for (Idx = 0; Idx < EOTPSECTION; Idx++) {

        HALOTPOneByteReadRAM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), (OTPSECTION+Idx), (pContant+Idx), L25EOUTVOLTAGE);
    }
}


VOID
WriteEOTPContant(
    IN u8 *pContant
)
{
    u8 Idx = 0;
    u8 DataTemp;
    for (Idx = 0; Idx < EOTPSECTION; Idx++) {

        HALOTPOneByteReadRAM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), (OTPSECTION+Idx), (&DataTemp), L25EOUTVOLTAGE);

        if (DataTemp == 0xFF) {

            HALOTPOneByteWriteRAM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), (OTPSECTION+Idx), (*(pContant+Idx)), L25EOUTVOLTAGE);
        }
    }
}

VOID
HALJtagOff(
    VOID
)
{
    HALEFUSEOneByteWriteROM(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_CTRL), 0xD3, 0xFE, L25EOUTVOLTAGE);
}

#ifdef CONFIG_EFUSE_VERIFY
VOID
EFUSETestApp
(
    IN  VOID *Data
)
{
    u32 *TestParameter;
    TestParameter = (u32*) Data;
    u32 Rtemp, i;

    switch (TestParameter[0]) {

        case 0: {
            u8 text[8] = {0xaa,0xcc,0xee,0x22,0x44,0xbb,0x77,0xdd};
            DBG_8195A("WriteEFUSE \n");

            WriteEfuseContant(TestParameter[1],TestParameter[2],TestParameter[3], text);
        }
        break;

        case 1: {
            u8 temp[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            u8 i;

            DBG_8195A("ReadEFUSE \n");
            ReadEfuseContant(TestParameter[1],temp);
            for (i=0; i<32; i++) {
                DBG_8195A("IDX : 0x%08x, 0x%08x\n", i, temp[i]);
            }
        }
        break;

        default:
            break;
    }
}
#endif
#endif  //CONFIG_EFUSE_EN
