/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "rtl8195a.h"
#include "hal_spi_flash.h"
#include "rtl8195a_spi_flash.h"

/***************** C-Cut Patch functions *********************/
#if 1//CONFIG_CHIP_C_CUT
VOID SpicInitRtl8195A(u8 InitBaudRate, u8 SpicBitMode);			// spi-flash controller initialization
VOID SpicRxCmdRtl8195A(u8);			// recieve command
VOID SpicTxCmdRtl8195A(u8 cmd, SPIC_INIT_PARA SpicInitPara);			// transfer command
u8 SpicGetFlashStatusRtl8195A(SPIC_INIT_PARA SpicInitPara);		// RDSR, read spi-flash status register
VOID SpicSetFlashStatusRtl8195A(u32 data, SPIC_INIT_PARA SpicInitPara);		// WRSR, write spi-flash status register
VOID SpicWaitBusyDoneRtl8195A(VOID);	// wait sr[0] = 0, wait transmission done
VOID SpicWaitWipDoneRtl8195A(SPIC_INIT_PARA SpicInitPara);		// wait spi-flash status register[0] = 0
VOID SpicEraseFlashRtl8195A(VOID);		// CE, flash chip erase
u32 SpicCmpDataForCalibrationRtl8195A(VOID);			// compare read_data and golden_data
VOID SpicLoadInitParaFromClockRtl8195AV02(IN  u8 CpuClkMode,IN  u8 BaudRate,IN  PSPIC_INIT_PARA pSpicInitPara);

u32 Rand (VOID);
extern SPIC_INIT_PARA SpicInitCPUCLK[4];

#ifdef CONFIG_FPGA
extern SPIC_INIT_PARA FPGASpicInitPara;
#endif

#ifdef CONFIG_SPIC_MODULE
C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
VOID
SpiFlashAppV02(
    IN  VOID *Data
)
{
    u32 *Cmd =(u32*)Data;
    u32 SpicBitMode;
    SPIC_INIT_PARA SpicInitPara;

#ifdef CONFIG_FPGA
    SpicInitPara.BaudRate = FPGASpicInitPara.BaudRate;
    SpicInitPara.RdDummyCyle = FPGASpicInitPara.RdDummyCyle;
    SpicInitPara.DelayLine = FPGASpicInitPara.DelayLine;
#else
    u8 CpuClk;
    //u8  Value8;
    u8 InitBaudRate;

    InitBaudRate = 1;

    CpuClk = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70)) >> 4);
    SpicLoadInitParaFromClockRtl8195AV02(CpuClk, InitBaudRate, &SpicInitPara);
#endif

    switch (Cmd[0]) {
        case 1:
            SpicBitMode = Cmd[1];
            DBG_SPIF_INFO("Initial Spi Flash Controller\n");
            FLASH_FCTRL(ON);
            ACTCK_FLASH_CCTRL(ON);
            SLPCK_FLASH_CCTRL(ON);
            PinCtrl(SPI_FLASH,S0,ON);

            switch(SpicBitMode) {
                case SpicOneBitMode:
                    DBG_SPIF_INFO("Initial Spic One bit mode\n");
                    SpicInitRtl8195A(SpicInitPara.BaudRate, SpicOneBitMode);
                    break;
                case SpicDualBitMode:
                    DBG_SPIF_INFO("Initial Spic Two bit mode\n");
                    SpicInitRtl8195A(SpicInitPara.BaudRate, SpicDualBitMode);
                    break;
                case SpicQuadBitMode:
                    DBG_SPIF_INFO("Initial Spic Four bit mode\n");
                    SpicInitRtl8195A(SpicInitPara.BaudRate, SpicQuadBitMode);
                    break;
            }
#if 0
            if (!SpicFlashInitRtl8195A(SpicBitMode))
                DBG_SPIF_INFO("SPI Init Fail!!!!!!\n");
            else
                DBG_SPIF_INFO("SPI Init SUCCESS\n");
#endif
            break;
        case 2:
            DBG_SPIF_INFO("Erase Falsh Start\n");
            SpicEraseFlashRtl8195A();
            DBG_SPIF_INFO("Erase Falsh End\n");

            break;

        default:
            break;
    }

}
#endif



C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
VOID
SpicInitRtl8195AV02
(
    IN  u8 InitBaudRate,
    IN  u8 SpicBitMode
)
{

    u32 Value32;
    SPIC_INIT_PARA SpicInitPara;

#ifdef CONFIG_FPGA
    SpicInitPara.BaudRate = FPGASpicInitPara.BaudRate;
    SpicInitPara.RdDummyCyle = FPGASpicInitPara.RdDummyCyle;
    SpicInitPara.DelayLine = FPGASpicInitPara.DelayLine;
#else
    u8 CpuClk;

    CpuClk = (((u8)(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70))) >> 4);
    SpicLoadInitParaFromClockRtl8195AV02(CpuClk, InitBaudRate, &SpicInitPara);
#endif

    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    HAL_SPI_WRITE32(REG_SPIC_BAUDR, BIT_SCKDV(InitBaudRate));

    HAL_SPI_WRITE32(REG_SPIC_SER, BIT_SER);

    Value32 = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
    HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
                    ((Value32 & 0xFFFF0000) | BIT_RD_DUMMY_LENGTH(SpicInitPara.RdDummyCyle)));

    HAL_WRITE32(PERI_ON_BASE, REG_PESOC_MEM_CTRL,
                ((HAL_READ32(PERI_ON_BASE, REG_PESOC_MEM_CTRL)&0xFFFFFF00)|
                 SpicInitPara.DelayLine));

    HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(4));


    switch (SpicBitMode) {
        case SpicOneBitMode:
            HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                            (HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_CMD_CH(3)|BIT_ADDR_CH(3)|BIT_DATA_CH(3)))));
            break;

        case SpicDualBitMode:
            HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                            ((HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_CMD_CH(3)|BIT_ADDR_CH(3)|BIT_DATA_CH(3)))) |
                             (BIT_ADDR_CH(1)|BIT_DATA_CH(1))));

            break;

        case SpicQuadBitMode:
            HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                            ((HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~(BIT_CMD_CH(3)|BIT_ADDR_CH(3)|BIT_DATA_CH(3)))) |
                             (BIT_ADDR_CH(2)|BIT_DATA_CH(2))));
            break;

    }

    // Enable SPI_FLASH User Mode
//    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
}


C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
VOID
SpicEraseFlashRtl8195AV02(VOID)
{

    SPIC_INIT_PARA SpicInitPara;

#ifdef CONFIG_FPGA
    SpicInitPara.BaudRate = FPGASpicInitPara.BaudRate;
    SpicInitPara.RdDummyCyle = FPGASpicInitPara.RdDummyCyle;
    SpicInitPara.DelayLine = FPGASpicInitPara.DelayLine;
#else


    u8 CpuClk;
    //u8  Value8;
    u8 InitBaudRate;

    InitBaudRate = 1;

    CpuClk = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70)) >> 4);
    SpicLoadInitParaFromClockRtl8195AV02(CpuClk, InitBaudRate, &SpicInitPara);
#endif
    DBG_SPIF_INFO("Init Baud Rate: 0x%x\n",SpicInitPara.BaudRate);
    DBG_SPIF_INFO("Init RD Dummy: 0x%x\n",SpicInitPara.RdDummyCyle);
    DBG_SPIF_INFO("Init Delay Line: 0x%x\n",SpicInitPara.DelayLine);

    // Wait for flash busy done
    SpicWaitWipDoneRtl8195A(SpicInitPara);

    while((SpicGetFlashStatusRtl8195A(SpicInitPara) & 0x02)==0) {
        // Set flash_cmd: WREN to FIFO
        SpicTxCmdRtl8195A(0x06, SpicInitPara);
    }

    DBG_SPIF_INFO("Erase Cmd Set\n");
    // Set flash_cmd: Chip_erase to FIFO
    SpicTxCmdRtl8195A(0xC7, SpicInitPara);

    // polling WEL
    do {
    } while((SpicGetFlashStatusRtl8195A(SpicInitPara) & 0x02)!=0);

    // Enable SPI_FLASH
    //HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

}

C_CUT_ROM_TEXT_SECTION
__weak _LONG_CALL_
VOID
SpicLoadInitParaFromClockRtl8195AV02
(
    IN  u8 CpuClkMode,
    IN  u8 BaudRate,
    IN  PSPIC_INIT_PARA pSpicInitPara
)
{
    PSPIC_INIT_PARA pTempSpinInitPara = NULL;
    u8  Value8;// BootCpuClk;
    u8  DelayCycle = 0;
    u8  DelayLine = 0;

    // Load Efuse Setting
#if 0
    Value8 = ((u8)((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & 0xFF000000)
                   >> 24));
    BootCpuClk = ((Value8 & 0xC) >> 2);

    pTempSpinInitPara = (PSPIC_INIT_PARA) &SpicInitCPUCLK[BootCpuClk];
#else
    pTempSpinInitPara = (PSPIC_INIT_PARA) &SpicInitCPUCLK[CpuClkMode];
#endif
    DelayLine = pTempSpinInitPara->DelayLine;

    //Adjustment from EFUSE
    if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSTEM_CFG1) & BIT_SYSCFG_ALDN_STS ) {

        if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EEPROM_CTRL0) & BIT_SYS_AUTOLOAD_SUS ) {

            if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & BIT(24) ) {

                Value8 = ((u8)((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & 0x00FF0000)
                               >> 16));
                //DDL
                DelayLine = ((Value8 & 0x1F) * 3);

                //Dummy cycle
                DelayCycle = ((Value8 & 0xE0) >> 5);
            }
        }
    }

#ifdef CONFIG_FPGA
    pSpicInitPara->BaudRate = FPGASpicInitPara.BaudRate;
    pSpicInitPara->DelayLine = FPGASpicInitPara.DelayLine;
    pSpicInitPara->RdDummyCyle = FPGASpicInitPara.RdDummyCyle;
#else
    pSpicInitPara->BaudRate = pTempSpinInitPara->BaudRate;
    pSpicInitPara->DelayLine = DelayLine;
    pSpicInitPara->RdDummyCyle = (pTempSpinInitPara->RdDummyCyle + DelayCycle);
#endif
}


#endif
