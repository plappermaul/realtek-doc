/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#if 0
#include "platform_autoconf.h"
#include "hal_peri_on.h"
#include "diag.h"


typedef enum _SRAM_ACCESS_TYEP_{
    ONE_BYTE_ACCESS,
    TWO_BYTE_ACCESS,
    FOUR_BYTE_ACCESS
}SRAM_ACCESS_TYEP, *PSRAM_ACCESS_TYEP;

static BOOL
SramReadWriteTest(
    IN u32                StartAddr,
    IN u32                SramSize
);


static BOOL
SramReadWriteCpy(
    IN SRAM_ACCESS_TYEP   AccessType,
    IN u32                StartAddr,
    IN u32                SramSize,
    IN u32                Pattern
);

#define MAX_SRAM_TYPE 3

BOOL 
SramTest(
    IN u8 SramType
)
{
    //Set Sram type  
    if (SramType < MAX_SRAM_TYPE) {
        HAL_PERI_ON_WRITE32(0x4, 
            ((HAL_PERI_ON_READ32(0x4)&(~(BIT17|BIT16)))|(SramType<<16)));
        DBG_8195A_HAL("Peri On Reg 0x4: 0x%x\n", HAL_PERI_ON_READ32(0x4));
    }
    switch(SramType) {
        case 0: {
                //Test BD Sram
                if (!SramReadWriteTest(0x20000000, 0x10000)) {                    
                    DBG_8195A_HAL("Sram Type: 0x0; BD Sram Test Fail\n");
                    goto test_fail;
                }

                //Test Buffer Sram
                if (!SramReadWriteTest(0x20080000, 0x60000)) {                    
                    DBG_8195A_HAL("Sram Type: 0x0; Buffer Sram Test Fail\n");
                    goto test_fail;
                }
            
            }
            break;
        case 1: {
                //Test BD Sram
                if (!SramReadWriteTest(0x20000000, 0x20000)) {
                    DBG_8195A_HAL("Sram Type: 0x1; BD Sram Test Fail\n");
                    goto test_fail;
                }

                //Test Buffer Sram
                if (!SramReadWriteTest(0x20080000, 0x50000)) {                    
                    DBG_8195A_HAL("Sram Type: 0x1; Buffer Sram Test Fail\n");
                    goto test_fail;
                }
                
            }
            break;
        case 2: {
                //Test BD Sram
                if (!SramReadWriteTest(0x20000000, 0x30000)) {
                    DBG_8195A_HAL("Sram Type: 0x2; BD Sram Test Fail\n");
                    goto test_fail;
                }
               
                //Test Buffer Sram
                if (!SramReadWriteTest(0x20080000, 0x40000)) {                    
                    DBG_8195A_HAL("Sram Type: 0x2; Buffer Sram Test Fail\n");
                    goto test_fail;
                }
               
            }
            break;

        case 3: {
                if (!SramReadWriteTest(0x1FFF0000, 0x10000)) {                    
                    DBG_8195A_HAL("TCM Test Fail\n");
                    goto test_fail;
                }
                
            }
            break;
        default:
            break;
    }

    return _TRUE;

test_fail:
    return _FALSE;
}



static BOOL
SramReadWriteTest(
    IN u32                StartAddr,
    IN u32                SramSize
)
{
                        
    if (SramReadWriteCpy(ONE_BYTE_ACCESS, StartAddr,
                          SramSize, 0x5A5A5A5A)) {
        DBG_8195A_HAL("One Bytes Access Success\n");                    
    }
    else {
        DBG_8195A_HAL("One Bytes Access Fail\n");
        return _FALSE;
    }


    if (SramReadWriteCpy(TWO_BYTE_ACCESS, StartAddr,
                          SramSize, 0xFFFFFFFF)) {
        DBG_8195A_HAL("Two Bytes Access Success\n");                    
    }
    else {
        DBG_8195A_HAL("Two Bytes Access Fail\n");
        return _FALSE;
    }

    if (SramReadWriteCpy(FOUR_BYTE_ACCESS, StartAddr,
                          SramSize, 0xA5A5A5A5)) {
        DBG_8195A_HAL("Four Bytes Access Success\n");                    
    }    
    else {
        DBG_8195A_HAL("Four Bytes Access Fail\n");
        return _FALSE;
    }

    return _TRUE;
}



static BOOL
SramReadWriteCpy(
    IN SRAM_ACCESS_TYEP   AccessType,
    IN u32                StartAddr,
    IN u32                SramSize,
    IN u32                Pattern
)
{
    switch(AccessType) {
        case ONE_BYTE_ACCESS:
            {
                u8  OneBytePattern = (u8) Pattern;
                u32 RwLoop = SramSize, RwIndex = 0;
                for (RwIndex = 0; RwIndex<RwLoop; RwIndex++) {
                    
                    HAL_WRITE8(StartAddr, RwIndex, OneBytePattern);
                    if (HAL_READ8(StartAddr, RwIndex) != OneBytePattern) {
                        DBG_8195A_HAL("Address: 0x%x; Write value %02x != Read Value 0x%02x\n",
                                    (StartAddr + RwIndex),
                                    OneBytePattern,
                                    HAL_READ8(StartAddr, RwIndex));
                        return _FALSE;
                    }
                    
                }
            }
            break;
            
        case TWO_BYTE_ACCESS:
            {
                u16 TwoBytePattern = (u16) Pattern;
                u32 RwLoop = SramSize>>1, RwIndex = 0;

                //Check two bytes alignment
                if (SramSize & 0x1) {                    
                    DBG_8195A_HAL("Addres not 2 bytes alignemnt: Stop Verify!!!!\n");
                    return _FALSE;
                }
                
                for (RwIndex = 0; RwIndex<RwLoop; RwIndex++) {
                    
                    HAL_WRITE16(StartAddr, (RwIndex<<1), TwoBytePattern);
                    if (HAL_READ16(StartAddr, (RwIndex<<1)) != TwoBytePattern) {
                        DBG_8195A_HAL("Address: 0x%x; Write value 0x%04x != Read Value 0x%04x\n",
                                    (StartAddr + (RwIndex<<1)),
                                    TwoBytePattern,
                                    HAL_READ16(StartAddr, (RwIndex<<1)));
                        return _FALSE;
                    }
                    
                }
            }
            break;

        case FOUR_BYTE_ACCESS:
            {
                u32 FourBytePattern = Pattern;
                u32 RwLoop = SramSize>>2, RwIndex = 0;

                //Check two bytes alignment
                if (SramSize & 0x3) {                    
                    DBG_8195A_HAL("Addres not 2 bytes alignemnt: Stop Verify!!!!\n");
                    return _FALSE;
                }
                
                for (RwIndex = 0; RwIndex<RwLoop; RwIndex++) {
                    
                    HAL_WRITE32(StartAddr, (RwIndex<<2), FourBytePattern);
                    if (HAL_READ32(StartAddr, (RwIndex<<2)) != FourBytePattern) {
                        DBG_8195A_HAL("Address: 0x%x; Write value 0x%x != Read Value 0x%x\n",
                                    (StartAddr + (RwIndex<<2)),
                                    FourBytePattern,
                                    HAL_READ32(StartAddr, (RwIndex<<1)));
                        return _FALSE;
                    }
                    
                }
            }

            break;

        default:
            {
                DBG_8195A_HAL("No Support Access Type!!!!!\n");
                return _FALSE;
            }
            break;
    }

    return _TRUE;
}

#endif
