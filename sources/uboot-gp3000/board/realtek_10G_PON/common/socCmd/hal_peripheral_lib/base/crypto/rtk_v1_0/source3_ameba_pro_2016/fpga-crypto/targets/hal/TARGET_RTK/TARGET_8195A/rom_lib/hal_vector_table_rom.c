/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"


#define MAX_VECTOR_TABLE_NUM                64
#define MAX_DEDECATED_IRQ_NUM               32
#define MAX_PERIPHERAL_IRQ_NUM              32

RAM_DEDECATED_VECTOR_TABLE_SECTION
HAL_VECTOR_FUN  NewVectorTable[MAX_VECTOR_TABLE_NUM];

RAM_USER_IRQ_FUN_TABLE_SECTION
IRQ_FUN         UserIrqFunTable[64];//MAX_DEDECATED_IRQ_NUM+MAX_PERIPHERAL_IRQ_NUM];

RAM_USER_IRQ_DATA_TABLE_SECTION
u32             UserIrqDataTable[64];//MAX_DEDECATED_IRQ_NUM+MAX_PERIPHERAL_IRQ_NUM];



//STEM_ON_IRQ                 = 0
HAL_ROM_TEXT_SECTION
_LONG_CALL_ VOID
HalSysOnIntrHandle(VOID)
{
    if (UserIrqFunTable[SYSTEM_ON_IRQ] != NULL) {
        UserIrqFunTable[SYSTEM_ON_IRQ]((VOID *)(UserIrqDataTable[SYSTEM_ON_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", SYSTEM_ON_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(SYSTEM_ON_IRQ);

}

//WDG_IRQ                       = 1
HAL_ROM_TEXT_SECTION
_LONG_CALL_ VOID
HalWdgIntrHandle(VOID)
{
    if (UserIrqFunTable[WDG_IRQ] != NULL) {
        UserIrqFunTable[WDG_IRQ]((VOID *)(UserIrqDataTable[WDG_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", WDG_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(WDG_IRQ);
}

//TIMER0_IRQ                    = 2
HAL_ROM_TEXT_SECTION
_LONG_CALL_ VOID
HalTimer0IntrHandle(VOID)
{
    if (UserIrqFunTable[TIMER0_IRQ] != NULL) {
        UserIrqFunTable[TIMER0_IRQ]((VOID *)(UserIrqDataTable[TIMER0_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", TIMER0_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(TIMER0_IRQ);

}

//TIMER1_IRQ                      = 3
HAL_ROM_TEXT_SECTION
_LONG_CALL_ VOID
HalTimer1IntrHandle(VOID)
{
    if (UserIrqFunTable[TIMER1_IRQ] != NULL) {
        UserIrqFunTable[TIMER1_IRQ]((VOID *)(UserIrqDataTable[TIMER1_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", TIMER1_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(TIMER1_IRQ);

}

//I2C3_IRQ                          = 4
HAL_ROM_TEXT_SECTION
_LONG_CALL_ VOID
HalI2C3IntrHandle(VOID)
{
    if (UserIrqFunTable[I2C3_IRQ] != NULL) {
        UserIrqFunTable[I2C3_IRQ]((VOID *)(UserIrqDataTable[I2C3_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", I2C3_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(I2C3_IRQ);
}

//TIMER2_7_IRQ                    =  5,
HAL_ROM_TEXT_SECTION
_LONG_CALL_ VOID
HalTimer2To7IntrHandle(VOID)
{
    if (UserIrqFunTable[TIMER2_7_IRQ] != NULL) {
        UserIrqFunTable[TIMER2_7_IRQ]((VOID *)(UserIrqDataTable[TIMER2_7_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", TIMER2_7_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(TIMER2_7_IRQ);

}

//SPI0_IRQ                        =  6,
HAL_ROM_TEXT_SECTION
_LONG_CALL_ VOID
HalSpi0IntrHandle(VOID)
{
    if (UserIrqFunTable[SPI0_IRQ] != NULL) {
        UserIrqFunTable[SPI0_IRQ]((VOID *)(UserIrqDataTable[SPI0_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", SPI0_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(SPI0_IRQ);
}

//GPIO_IRQ                        =  7,
HAL_ROM_TEXT_SECTION
_LONG_CALL_ VOID
HalGpioIntrHandle(VOID)
{
    if (UserIrqFunTable[GPIO_IRQ] != NULL) {
        UserIrqFunTable[GPIO_IRQ]((VOID *)(UserIrqDataTable[GPIO_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GPIO_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GPIO_IRQ);

}

//UART0_IRQ                       =  8,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalUart0IntrHandle(VOID)
{
    if (UserIrqFunTable[UART0_IRQ] != NULL) {
        UserIrqFunTable[UART0_IRQ]((VOID *)(UserIrqDataTable[UART0_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", UART0_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(UART0_IRQ);

}

//SPI_FLASH_IRQ                   =  9,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalSpiFlashIntrHandle(VOID)
{
    if (UserIrqFunTable[SPI_FLASH_IRQ] != NULL) {
        UserIrqFunTable[SPI_FLASH_IRQ]((VOID *)(UserIrqDataTable[SPI_FLASH_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", SPI_FLASH_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(SPI_FLASH_IRQ);

}

//USB_OTG_IRQ                     =  10,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalUsbOtgIntrHandle(VOID)
{
    if (UserIrqFunTable[USB_OTG_IRQ] != NULL) {
        UserIrqFunTable[USB_OTG_IRQ]((VOID *)(UserIrqDataTable[USB_OTG_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", USB_OTG_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(USB_OTG_IRQ);

}

//SDIO_HOST_IRQ                   =  11,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalSdioHostIntrHandle(VOID)
{
    if (UserIrqFunTable[SDIO_HOST_IRQ] != NULL) {
        UserIrqFunTable[SDIO_HOST_IRQ]((VOID *)(UserIrqDataTable[SDIO_HOST_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", SDIO_HOST_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(SDIO_HOST_IRQ);

}

//SDIO_DEVICE_IRQ                 =  12,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalSdioDeviceIntrHandle(VOID)
{
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(SDIO_DEVICE_IRQ);
    if (UserIrqFunTable[SDIO_DEVICE_IRQ] != NULL) {
        UserIrqFunTable[SDIO_DEVICE_IRQ]((VOID *)(UserIrqDataTable[SDIO_DEVICE_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", SDIO_DEVICE_IRQ);
    }

}


//I2S0_PCM0_IRQ                   =  13,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalI2s0OrPcm0IntrHandle(VOID)
{
    if (UserIrqFunTable[I2S0_PCM0_IRQ] != NULL) {
        UserIrqFunTable[I2S0_PCM0_IRQ]((VOID *)(UserIrqDataTable[I2S0_PCM0_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", I2S0_PCM0_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(I2S0_PCM0_IRQ);
}

//I2S1_PCM1_IRQ                   =  14,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalI2s1OrPcm1IntrHandle(VOID)
{
    if (UserIrqFunTable[I2S1_PCM1_IRQ] != NULL) {
        UserIrqFunTable[I2S1_PCM1_IRQ]((VOID *)(UserIrqDataTable[I2S1_PCM1_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", I2S1_PCM1_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(I2S1_PCM1_IRQ);
}

//WL_DMA_IRQ                      =  15,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalWlDmaIntrHandle(VOID)
{
//    DBG_8195A("wl dma isr\n");
    if (UserIrqFunTable[WL_DMA_IRQ] != NULL) {
        UserIrqFunTable[WL_DMA_IRQ]((VOID *)(UserIrqDataTable[WL_DMA_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", WL_DMA_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(WL_DMA_IRQ);
}


//WL_PROTOCOL_IRQ               =  16,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalWlProtocolIntrHandle(VOID)
{
    if (UserIrqFunTable[WL_PROTOCOL_IRQ] != NULL) {
        UserIrqFunTable[WL_PROTOCOL_IRQ]((VOID *)(UserIrqDataTable[WL_PROTOCOL_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", WL_PROTOCOL_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(WL_PROTOCOL_IRQ);
}

//CRYPTO_IRQ                          =  17,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalCryptoIntrHandle(VOID)
{
    if (UserIrqFunTable[CRYPTO_IRQ] != NULL) {
        UserIrqFunTable[CRYPTO_IRQ]((VOID *)(UserIrqDataTable[CRYPTO_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", CRYPTO_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(CRYPTO_IRQ);
}

//GMAC_IRQ                        =  18,
HAL_ROM_TEXT_SECTION
VOID
HalGmacIntrHandle(VOID)
{
    if (UserIrqFunTable[GMAC_IRQ] != NULL) {
        UserIrqFunTable[GMAC_IRQ]((VOID *)(UserIrqDataTable[GMAC_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GMAC_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GMAC_IRQ);
}

//PERIPHERAL_IRQ                  =  19,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalPeripheralIntrHandle(VOID)
{
    u32 PeriIrqStatus, CheckIndex, ExactIrqStatus, TableIndex;

    //DBG_8195A("Enter ISR\n");

    PeriIrqStatus = HAL_READ32(VENDOR_REG_BASE, PERIPHERAL_IRQ_STATUS);

    //Save exact IRQ status
    ExactIrqStatus = PeriIrqStatus &
                     HAL_READ32(VENDOR_REG_BASE, PERIPHERAL_IRQ_EN);

    //Check exact IRQ function
    for(CheckIndex = 0; CheckIndex<32; CheckIndex++) {
        if (ExactIrqStatus & BIT_(CheckIndex)) {
            TableIndex = CheckIndex + MAX_DEDECATED_IRQ_NUM;
            UserIrqFunTable[TableIndex]((VOID *)(UserIrqDataTable[TableIndex]));
        }
    }

    //Clear sub-rout IRQ
    HAL_WRITE32(VENDOR_REG_BASE, PERIPHERAL_IRQ_STATUS, PeriIrqStatus);

    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(PERIPHERAL_IRQ);
}

//GDMA0_CHANNEL0_IRQ              =  20,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma0Ch0IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA0_CHANNEL0_IRQ] != NULL) {
        UserIrqFunTable[GDMA0_CHANNEL0_IRQ]((VOID *)(UserIrqDataTable[GDMA0_CHANNEL0_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA0_CHANNEL0_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA0_CHANNEL0_IRQ);
}

//GDMA0_CHANNEL1_IRQ              =  21,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma0Ch1IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA0_CHANNEL1_IRQ] != NULL) {
        UserIrqFunTable[GDMA0_CHANNEL1_IRQ]((VOID *)(UserIrqDataTable[GDMA0_CHANNEL1_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA0_CHANNEL1_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA0_CHANNEL1_IRQ);
}

//GDMA0_CHANNEL2_IRQ              =  22,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma0Ch2IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA0_CHANNEL2_IRQ] != NULL) {
        UserIrqFunTable[GDMA0_CHANNEL2_IRQ]((VOID *)(UserIrqDataTable[GDMA0_CHANNEL2_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA0_CHANNEL2_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA0_CHANNEL2_IRQ);
}

//GDMA0_CHANNEL3_IRQ              =  23,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma0Ch3IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA0_CHANNEL3_IRQ] != NULL) {
        UserIrqFunTable[GDMA0_CHANNEL3_IRQ]((VOID *)(UserIrqDataTable[GDMA0_CHANNEL3_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA0_CHANNEL3_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA0_CHANNEL3_IRQ);
}

//GDMA0_CHANNEL4_IRQ              =  24,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma0Ch4IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA0_CHANNEL4_IRQ] != NULL) {
        UserIrqFunTable[GDMA0_CHANNEL4_IRQ]((VOID *)(UserIrqDataTable[GDMA0_CHANNEL4_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA0_CHANNEL4_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA0_CHANNEL4_IRQ);
}

//GDMA0_CHANNEL5_IRQ              =  25,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma0Ch5IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA0_CHANNEL5_IRQ] != NULL) {
        UserIrqFunTable[GDMA0_CHANNEL5_IRQ]((VOID *)(UserIrqDataTable[GDMA0_CHANNEL5_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA0_CHANNEL5_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA0_CHANNEL5_IRQ);
}

//GDMA1_CHANNEL0_IRQ              =  26,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma1Ch0IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA1_CHANNEL0_IRQ] != NULL) {
        UserIrqFunTable[GDMA1_CHANNEL0_IRQ]((VOID *)(UserIrqDataTable[GDMA1_CHANNEL0_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA1_CHANNEL0_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA1_CHANNEL0_IRQ);
}

//GDMA1_CHANNEL1_IRQ              =  27,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma1Ch1IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA1_CHANNEL1_IRQ] != NULL) {
        UserIrqFunTable[GDMA1_CHANNEL1_IRQ]((VOID *)(UserIrqDataTable[GDMA1_CHANNEL1_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA1_CHANNEL1_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA1_CHANNEL1_IRQ);
}

//GDMA1_CHANNEL2_IRQ              =  28,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma1Ch2IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA1_CHANNEL2_IRQ] != NULL) {
        UserIrqFunTable[GDMA1_CHANNEL2_IRQ]((VOID *)(UserIrqDataTable[GDMA1_CHANNEL2_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA1_CHANNEL2_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA1_CHANNEL2_IRQ);
}

//GDMA1_CHANNEL3_IRQ              =  29,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma1Ch3IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA1_CHANNEL3_IRQ] != NULL) {
        UserIrqFunTable[GDMA1_CHANNEL3_IRQ]((VOID *)(UserIrqDataTable[GDMA1_CHANNEL3_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA1_CHANNEL3_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA1_CHANNEL3_IRQ);
}


//GDMA1_CHANNEL4_IRQ              =  30,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma1Ch4IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA1_CHANNEL4_IRQ] != NULL) {
        UserIrqFunTable[GDMA1_CHANNEL4_IRQ]((VOID *)(UserIrqDataTable[GDMA1_CHANNEL4_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA1_CHANNEL4_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA1_CHANNEL4_IRQ);
}

//GDMA1_CHANNEL5_IRQ              =  31,
HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
HalGdma1Ch5IntrHandle(VOID)
{
    if (UserIrqFunTable[GDMA1_CHANNEL5_IRQ] != NULL) {
        UserIrqFunTable[GDMA1_CHANNEL5_IRQ]((VOID *)(UserIrqDataTable[GDMA1_CHANNEL5_IRQ]));
    } else {
        DBG_8195A("User Irq %d Fun Not Assign!!!!!", GDMA1_CHANNEL5_IRQ);
    }
    //Clear ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(GDMA1_CHANNEL5_IRQ);
}
#if 0
//#ifdef CONFIG_KERNEL
extern void vPortSVCHandler( void );
extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
#endif

HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
VectorTableInitRtl8195A(
    IN  u32 StackP
)
{
    u32 ArrayIndex = 0;

    //2 1) Initial Dedecated Vector Table

    //4 Initial new stack point
    NewVectorTable[0] = (HAL_VECTOR_FUN)StackP;

    //4 Initial new reset function
    NewVectorTable[1] =
        (HAL_VECTOR_FUN)HAL_READ32(0, 0x4);

    //4 Initial NMI
    NewVectorTable[2] =
        (HAL_VECTOR_FUN)HAL_READ32(0, 0x8);

    //4 Initial Hard Fault
    NewVectorTable[3] =
        (HAL_VECTOR_FUN)HAL_READ32(0, 0xC);
#if 0
    //4 Initial Memory Manage
    NewVectorTable[4] =
        (HAL_VECTOR_FUN)HAL_READ32(0, 0x10);

    //4 Initial Bus Fault
    NewVectorTable[5] =
        (HAL_VECTOR_FUN)HAL_READ32(0, 0x14);

    //4 Initial Usage Fault
    NewVectorTable[6] =
        (HAL_VECTOR_FUN)HAL_READ32(0, 0x18);
#endif
    //1 TODO
#if 0
//#ifdef CONFIG_KERNEL
    //4 Initial SVC
    NewVectorTable[11] =
        (HAL_VECTOR_FUN)vPortSVCHandler;

    //4 Initial Pend SVC
    NewVectorTable[14] =
        (HAL_VECTOR_FUN)xPortPendSVHandler;

    //4 Initial System Tick fun
    NewVectorTable[15] =
        (HAL_VECTOR_FUN)xPortSysTickHandler;
#endif

    NewVectorTable[16] = (HAL_VECTOR_FUN) HalSysOnIntrHandle;
    NewVectorTable[17] = (HAL_VECTOR_FUN) HalWdgIntrHandle;
    NewVectorTable[18] = (HAL_VECTOR_FUN) HalTimer0IntrHandle;
    NewVectorTable[19] = (HAL_VECTOR_FUN) HalTimer1IntrHandle;
    NewVectorTable[20] = (HAL_VECTOR_FUN) HalI2C3IntrHandle;
    NewVectorTable[21] = (HAL_VECTOR_FUN) HalTimer2To7IntrHandle;
    NewVectorTable[22] = (HAL_VECTOR_FUN) HalSpi0IntrHandle;
    NewVectorTable[23] = (HAL_VECTOR_FUN) HalGpioIntrHandle;
    NewVectorTable[24] = (HAL_VECTOR_FUN) HalUart0IntrHandle;
    NewVectorTable[25] = (HAL_VECTOR_FUN) HalSpiFlashIntrHandle;
    NewVectorTable[26] = (HAL_VECTOR_FUN) HalUsbOtgIntrHandle;
    NewVectorTable[27] = (HAL_VECTOR_FUN) HalSdioHostIntrHandle;
    NewVectorTable[28] = (HAL_VECTOR_FUN) HalSdioDeviceIntrHandle;
    NewVectorTable[29] = (HAL_VECTOR_FUN) HalI2s0OrPcm0IntrHandle;
    NewVectorTable[30] = (HAL_VECTOR_FUN) HalI2s1OrPcm1IntrHandle;
    NewVectorTable[31] = (HAL_VECTOR_FUN) HalWlDmaIntrHandle;
    NewVectorTable[32] = (HAL_VECTOR_FUN) HalWlProtocolIntrHandle;
    NewVectorTable[33] = (HAL_VECTOR_FUN) HalCryptoIntrHandle;
    NewVectorTable[34] = (HAL_VECTOR_FUN) HalGmacIntrHandle;
    NewVectorTable[35] = (HAL_VECTOR_FUN) HalPeripheralIntrHandle;
    NewVectorTable[36] = (HAL_VECTOR_FUN) HalGdma0Ch0IntrHandle;
    NewVectorTable[37] = (HAL_VECTOR_FUN) HalGdma0Ch1IntrHandle;
    NewVectorTable[38] = (HAL_VECTOR_FUN) HalGdma0Ch2IntrHandle;
    NewVectorTable[39] = (HAL_VECTOR_FUN) HalGdma0Ch3IntrHandle;
    NewVectorTable[40] = (HAL_VECTOR_FUN) HalGdma0Ch4IntrHandle;
    NewVectorTable[41] = (HAL_VECTOR_FUN) HalGdma0Ch5IntrHandle;
    NewVectorTable[42] = (HAL_VECTOR_FUN) HalGdma1Ch0IntrHandle;
    NewVectorTable[43] = (HAL_VECTOR_FUN) HalGdma1Ch1IntrHandle;
    NewVectorTable[44] = (HAL_VECTOR_FUN) HalGdma1Ch2IntrHandle;
    NewVectorTable[45] = (HAL_VECTOR_FUN) HalGdma1Ch3IntrHandle;
    NewVectorTable[46] = (HAL_VECTOR_FUN) HalGdma1Ch4IntrHandle;
    NewVectorTable[47] = (HAL_VECTOR_FUN) HalGdma1Ch5IntrHandle;

    for (ArrayIndex=0; ArrayIndex< 64; ArrayIndex++) {
        UserIrqFunTable[ArrayIndex] = NULL;
        UserIrqDataTable[ArrayIndex] = NULL;
    }

    //2 3) Set Vector Table Offset Register
    NVIC_SetVectorTable(0x10000000);

    //2 4) Set Peripheral Interrupt to level trigger
    HAL_WRITE32(VENDOR_REG_BASE, PERIPHERAL_IRQ_MODE, PERIPHERAL_IRQ_ALL_LEVEL);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
VectorTableInitForOSRtl8195A(
    IN  VOID *PortSVC,
    IN  VOID *PortPendSVH,
    IN  VOID *PortSysTick
)
{
    //4 Initial SVC
    NewVectorTable[11] =
        (HAL_VECTOR_FUN)PortSVC;

    //4 Initial Pend SVC
    NewVectorTable[14] =
        (HAL_VECTOR_FUN)PortPendSVH;

    //4 Initial System Tick fun
    NewVectorTable[15] =
        (HAL_VECTOR_FUN)PortSysTick;
}


HAL_ROM_TEXT_SECTION
_LONG_CALL_ BOOL
VectorIrqRegisterRtl8195A(
    IN  PIRQ_HANDLE pIrqHandle
)
{
    u32 IrqNum;

    if (pIrqHandle->IrqNum > (PERIPHERAL_IRQ_BASE_NUM - 1)) {
        IrqNum = (pIrqHandle->IrqNum - (PERIPHERAL_IRQ_BASE_NUM - 1)) +
                 ((u32) GDMA1_CHANNEL5_IRQ);
    } else {
        IrqNum = pIrqHandle->IrqNum;
    }

    if (UserIrqFunTable[IrqNum]) {
        DBG_8195A_HAL("ISR %d had been allocated!!!\n",pIrqHandle->IrqNum);
        return _FALSE;
    } else {
        UserIrqFunTable[IrqNum] = (IRQ_FUN)((u32)pIrqHandle->IrqFun | 0x1);
        UserIrqDataTable[IrqNum] = (u32)(pIrqHandle->Data);
    }
    return _TRUE;

}

HAL_ROM_TEXT_SECTION
_LONG_CALL_ BOOL
VectorIrqUnRegisterRtl8195A(
    IN  PIRQ_HANDLE pIrqHandle
)
{
    u32 IrqNum;

    if (pIrqHandle->IrqNum > (PERIPHERAL_IRQ_BASE_NUM - 1)) {
        IrqNum = (pIrqHandle->IrqNum - (PERIPHERAL_IRQ_BASE_NUM - 1)) +
                 ((u32) GDMA1_CHANNEL5_IRQ);
    } else {
        IrqNum = pIrqHandle->IrqNum;
    }

    if (UserIrqFunTable[IrqNum]) {
        UserIrqFunTable[IrqNum] = (IRQ_FUN)NULL;
        UserIrqDataTable[IrqNum] = (u32)NULL;
        return _TRUE;
    } else {
        DBG_8195A_HAL("ISR %d didb't bee register!!!\n",pIrqHandle->IrqNum);
        return _FALSE;
    }

}


HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
VectorIrqEnRtl8195A(
    IN  PIRQ_HANDLE pIrqHandle
)
{
    u32 IrqNum;

    // Enable ARM Cortex-M3 IRQ
    if (pIrqHandle->IrqNum > (PERIPHERAL_IRQ_BASE_NUM - 1)) {
        //Clear the status of peripheral interrupt
        HAL_WRITE32(VENDOR_REG_BASE, PERIPHERAL_IRQ_STATUS,
                    HAL_READ32(VENDOR_REG_BASE, PERIPHERAL_IRQ_STATUS));

        //Enable Peripheral IRQ mask
        HAL_WRITE32(VENDOR_REG_BASE, PERIPHERAL_IRQ_EN,
                    (HAL_READ32(VENDOR_REG_BASE, PERIPHERAL_IRQ_EN) |
                     BIT_(pIrqHandle->IrqNum - PERIPHERAL_IRQ_BASE_NUM)));

        IrqNum = PERIPHERAL_IRQ;
    } else {
        IrqNum = pIrqHandle->IrqNum;
    }
    //Clear the status of ARM Cortex-M3 IRQ
    NVIC_ClearPendingIRQ(IrqNum);

    NVIC_SetPriority(IrqNum,
                     pIrqHandle->Priority);
    NVIC_EnableIRQ(IrqNum);

}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  VOID
VectorIrqDisRtl8195A(
    IN  PIRQ_HANDLE pIrqHandle
)
{
    u32 IrqNum;
    u32 i;

    // Enable ARM Cortex-M3 IRQ
    if (pIrqHandle->IrqNum > (PERIPHERAL_IRQ_BASE_NUM - 1)) {

        //Disable Peripheral IRQ mask
        HAL_WRITE32(VENDOR_REG_BASE, PERIPHERAL_IRQ_EN,
                    (HAL_READ32(VENDOR_REG_BASE, PERIPHERAL_IRQ_EN) &
                     (~ BIT_(pIrqHandle->IrqNum - PERIPHERAL_IRQ_BASE_NUM))));
        for (i=0; i<64; i++) {
            if (UserIrqFunTable[i] != NULL) {
                // Atleast 1 peripheral device still using the peripheral interrupt, so cannot disable it
                return;
            }
        }
        IrqNum = PERIPHERAL_IRQ;
    } else {
        IrqNum = pIrqHandle->IrqNum;
    }

    NVIC_SetPriority(IrqNum, 0);

    NVIC_DisableIRQ(IrqNum);

}


