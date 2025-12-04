/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
 
#include "platform_autoconf.h"
#include "hal_vector_table.h"
#include "diag.h"
#include "section_config.h"


#define MAX_VECTOR_TABLE_NUM                64
#define MAX_DEDECATED_IRQ_NUM               32
#define MAX_PERIPHERAL_IRQ_NUM              32 

extern HAL_VECTOR_FUN  NewVectorTable[MAX_VECTOR_TABLE_NUM];

extern IRQ_FUN         UserIrqFunTable[64];

extern u32             UserIrqDataTable[64];

_LONG_CALL_
extern u32 HalDelayUs(u32 us);

VOID
En32KCalibration(
    VOID
)
{
    u32 Rtemp;
    u32 Ttemp = 0;

    DiagPrintf("32K clock source calibration\n");

    while(1) {

        //set parameter
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);
        Rtemp = 0x80f880;
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);


        //Polling LOCK
        Rtemp = 0x110000;
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
//        DiagPrintf("Polling lock\n");
        HalDelayUs(30);

        Rtemp = HAL_READ32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL1);
        if ((Rtemp & 0x3000) != 0x0){
            DiagPrintf("Calibration Success\n", Ttemp);
            break;
        }
        else {
            Ttemp++;
            HalDelayUs(60);
//            DiagPrintf("Check lock: %d\n", Ttemp);
            if (Ttemp > 100000) { /*Delay 100ms*/            
                DiagPrintf("Calibration Fail\n", Ttemp);
                break;
            }
        }
    }
        
    Rtemp = 0x884000;
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
}

#ifdef CONFIG_CODE_IN_THE_MOST_INTERNAL_SRAM
VOID
DownloadImageForSpecialMode(
    VOID
)
{
    u32 Image1Len, Image2Len, Image2Addr,Image3Len, Image3Addr,Image4Len, Image4Addr, ImageIndex, SpicImageIndex;
    Image1Len = HAL_READ32(SPI_FLASH_BASE, 0x10)+0x20;
    Image2Len = HAL_READ32(SPI_FLASH_BASE, Image1Len);
    Image2Addr = HAL_READ32(SPI_FLASH_BASE, (Image1Len+0x4));
    Image3Len = HAL_READ32(SPI_FLASH_BASE, (Image1Len + Image2Len + 0x10));
    Image3Addr = HAL_READ32(SPI_FLASH_BASE, (Image1Len + Image2Len + 0x10 + 0x4));
    Image4Len = HAL_READ32(SPI_FLASH_BASE, (Image1Len + Image2Len + Image3Len + 0x20));
    Image4Addr = HAL_READ32(SPI_FLASH_BASE, (Image1Len + Image2Len + Image3Len + 0x20 + 0x4));

    DBG_8195A("Image3 length: 0x%x, Image3 Addr: 0x%x\n",Image3Len, Image3Addr);
    DBG_8195A("Image4 length: 0x%x, Image4 Addr: 0x%x\n",Image4Len, Image4Addr);

    SpicImageIndex = 0;

    for (ImageIndex = 0x20 + Image1Len + Image2Len; 
            ImageIndex < (Image3Len + Image2Len + Image1Len + 0x20);
            ImageIndex = ImageIndex + 4) {
        HAL_WRITE32(Image3Addr, SpicImageIndex,
                    HAL_READ32(SPI_FLASH_BASE, ImageIndex));

        SpicImageIndex += 4;
    }

    SpicImageIndex = 0;

    for (ImageIndex = 0x30 + Image1Len + Image2Len + Image3Len; 
            ImageIndex < (Image4Len + Image3Len + Image2Len + Image1Len + 0x30);
            ImageIndex = ImageIndex + 4) {
        HAL_WRITE32(Image4Addr, SpicImageIndex,
                    HAL_READ32(SPI_FLASH_BASE, ImageIndex));

        SpicImageIndex += 4;
    }

}
#endif


//WLXXX1_IRQ                      =  16,
VOID
PatchHalWLXXX1IntrHandle(VOID)
{
    DBG_8195A("%s\n",__FUNCTION__);
    if (UserIrqFunTable[WLXXX1_IRQ] != NULL) {
        UserIrqFunTable[WLXXX1_IRQ]((VOID *)(UserIrqDataTable[WLXXX1_IRQ]));
    }
    else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", WLXXX1_IRQ);
    }     
    //Clear ARM Cortex-M3 IRQ        
    NVIC_ClearPendingIRQ(WLXXX1_IRQ);
}

//WLXXX2_IRQ                      =  17,
VOID
PatchHalWLXXX2IntrHandle(VOID)
{
//    DBG_8195A("%s\n",__FUNCTION__);
    if (UserIrqFunTable[WL_PROTOCOL_IRQ] != NULL) {
        UserIrqFunTable[WL_PROTOCOL_IRQ]((VOID *)(UserIrqDataTable[WL_PROTOCOL_IRQ]));
    }
    else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", WL_PROTOCOL_IRQ);
    }     
    //Clear ARM Cortex-M3 IRQ        
    NVIC_ClearPendingIRQ(WL_PROTOCOL_IRQ);    
}

//WLXXX3_IRQ                      =  18,
VOID
PatchHalWLXXX3IntrHandle(VOID)
{
    DBG_8195A("%s\n",__FUNCTION__);
    if (UserIrqFunTable[WLXXX3_IRQ] != NULL) {
        UserIrqFunTable[WLXXX3_IRQ]((VOID *)(UserIrqDataTable[WLXXX3_IRQ]));
    }
    else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", WLXXX3_IRQ);
    }     
    //Clear ARM Cortex-M3 IRQ        
    NVIC_ClearPendingIRQ(WLXXX3_IRQ);    
}


VOID
PatchVectorTableInitRtl8195A(
    VOID
)
{
    
    //2 1) Initial Dedecated Vector Table
    

    NewVectorTable[32] = (HAL_VECTOR_FUN) PatchHalWLXXX1IntrHandle;
    NewVectorTable[33] = (HAL_VECTOR_FUN) PatchHalWLXXX2IntrHandle;
    NewVectorTable[34] = (HAL_VECTOR_FUN) PatchHalWLXXX3IntrHandle;

    //2 4) Set Peripheral Interrupt to level trigger     
    HAL_WRITE32(VENDOR_REG_BASE, PERIPHERAL_IRQ_MODE, PERIPHERAL_IRQ_ALL_LEVEL);

#ifdef CONFIG_CODE_IN_THE_MOST_INTERNAL_SRAM
    DownloadImageForSpecialMode();
#endif

    En32KCalibration();

    HalDelayUs(1000);
}


