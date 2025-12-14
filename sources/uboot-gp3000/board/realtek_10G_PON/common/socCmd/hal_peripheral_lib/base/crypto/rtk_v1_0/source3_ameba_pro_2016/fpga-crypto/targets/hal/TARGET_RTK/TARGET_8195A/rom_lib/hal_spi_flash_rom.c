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

//3 One Bit Mode
#if 1
#ifdef CONFIG_RTL_SIM
HAL_ROM_DATA_SECTION
SPIC_INIT_PARA SpicInitCPUCLK[4] = {
    {0x1,0x1,0x5E,0},
    {0x1,0x0,0x0,0},
    {0x1,0x0,0x0,0},
    {0x1,0x0,0x0,0},
};
#else
HAL_ROM_DATA_SECTION
SPIC_INIT_PARA SpicInitCPUCLK[4] = {
    {0x1,0x1,0x5E,0},  //default cpu 41, baud 1
    {0x1,0x1,0x0,0},   //cpu 20.8 , baud 1
    {0x1,0x2,0x23,0},   //cpu 83.3, baud 1
    {0x1,0x5,0x5,0},
};
#endif
#else
#ifdef CONFIG_RTL_SIM
HAL_ROM_DATA_SECTION
SPIC_INIT_PARA SpicInit200MCLK[4] = {
    {0x1,0x0,0x1,0},
    {0x2,0x0,0x5,0},
    {0x3,0x0,0x5,0},
    {0x4,0x0,0x5,0},
};
HAL_ROM_DATA_SECTION
SPIC_INIT_PARA SpicInit100MCLK[2] = {
    {0x1,0x0,0x16,0},
    {0x2,0x0,0,0},
};
HAL_ROM_DATA_SECTION
SPIC_INIT_PARA SpicInit50MCLK[1] = {
    {0x1,0x0,0x24,0},
};

#else
HAL_ROM_DATA_SECTION
SPIC_INIT_PARA SpicInit200MCLK[4] = {
    {0x1,0x5,0x1,0},
    {0x2,0x4,0x5,0},
    {0x3,0x3,0x5,0},
    {0x4,0x2,0x5,0},
};
HAL_ROM_DATA_SECTION
SPIC_INIT_PARA SpicInit100MCLK[2] = {
    {0x1,0x2,0x16,0},
    {0x2,0x2,0,0},
};
HAL_ROM_DATA_SECTION
SPIC_INIT_PARA SpicInit50MCLK[1] = {
    {0x1,0x1,0x24,0},
};
#endif
#endif

HAL_ROM_DATA_SECTION
u32 SpicCalibrationPattern[4] = {
    0x96969999,
    0xFC66CC3F,
    0x03CC33C0,
    0x6231DCE5
};

#define SPI_CTRL_BASE 0x1FFEF000
#define SPI_DLY_CTRL_ADDR 0x40000300	// [7:0]
#define MIN_BAUDRATE  0x01
#define MAX_BAUDRATE  0x04
#define MAX_AUTOLEN   0x04
#define MAX_DLYLINE   99
#define GOLD_ID_NO    0xC220
#ifdef CONFIG_FPGA
#define SRAM_STR_BASE 0x1006FF00	// SRAM space to store golden value
#define SPI_TEST_ADDR 0x980FFC00
#define TEST_SIZE     0x16
#endif


#define WR_DATA(addr, data)    (*((volatile u32*)(addr)) = (data))
#define RD_DATA(addr)          (*((volatile u32*)(addr)))


VOID SpicInitRtl8195A(u8 InitBaudRate, u8 SpicBitMode);			// spi-flash controller initialization
VOID SpicRxCmdRtl8195A(u8);			// recieve command
VOID SpicTxCmdRtl8195A(u8 cmd, SPIC_INIT_PARA SpicInitPara);			// transfer command
u8 SpicGetFlashStatusRtl8195A(SPIC_INIT_PARA SpicInitPara);		// RDSR, read spi-flash status register
VOID SpicSetFlashStatusRtl8195A(u32 data, SPIC_INIT_PARA SpicInitPara);		// WRSR, write spi-flash status register
VOID SpicWaitBusyDoneRtl8195A(VOID);	// wait sr[0] = 0, wait transmission done
VOID SpicWaitWipDoneRtl8195A(SPIC_INIT_PARA SpicInitPara);		// wait spi-flash status register[0] = 0
VOID SpicEraseFlashRtl8195A(VOID);		// CE, flash chip erase
u32 SpicCmpDataForCalibrationRtl8195A(VOID);			// compare read_data and golden_data
VOID SpicLoadInitParaFromClockRtl8195A(u8 CpuClkMode, u8 BaudRate, PSPIC_INIT_PARA pSpicInitPara);
u32 Rand (VOID);

#ifdef CONFIG_FPGA
extern SPIC_INIT_PARA FPGASpicInitPara;
#endif

#ifdef CONFIG_SPIC_MODULE

HAL_ROM_TEXT_SECTION
VOID
SpiFlashApp(
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
    u8  Value8, InitBaudRate;

    InitBaudRate = 1;

    CpuClk = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70)) >> 4);
    SpicLoadInitParaFromClockRtl8195A(CpuClk, InitBaudRate, &SpicInitPara);
#endif

    switch (Cmd[0]) {
        case 1:
            SpicBitMode = Cmd[1];
            DBG_8195A("Initial Spi Flash Controller\n");
            FLASH_FCTRL(ON);
            ACTCK_FLASH_CCTRL(ON);
            SLPCK_FLASH_CCTRL(ON);
            PinCtrl(SPI_FLASH,S0,ON);

            switch(SpicBitMode) {
                case SpicOneBitMode:
                    DBG_8195A("Initial Spic One bit mode\n");
                    SpicInitRtl8195A(SpicInitPara.BaudRate, SpicOneBitMode);
                    break;
                case SpicDualBitMode:
                    DBG_8195A("Initial Spic Two bit mode\n");
                    SpicInitRtl8195A(SpicInitPara.BaudRate, SpicDualBitMode);
                    break;
                case SpicQuadBitMode:
                    DBG_8195A("Initial Spic Four bit mode\n");
                    SpicInitRtl8195A(SpicInitPara.BaudRate, SpicQuadBitMode);
                    break;
            }
#if 0
            if (!SpicFlashInitRtl8195A(SpicBitMode))
                DBG_8195A("SPI Init Fail!!!!!!\n");
            else
                DBG_8195A("SPI Init SUCCESS\n");
#endif
            break;
        case 2:
            DBG_8195A("Erase Falsh Start\n");
            SpicEraseFlashRtl8195A();
            DBG_8195A("Erase Falsh End\n");

            break;

        default:
            break;
    }

}
#endif



HAL_ROM_TEXT_SECTION
VOID
SpicInitRtl8195A
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
    SpicLoadInitParaFromClockRtl8195A(CpuClk, InitBaudRate, &SpicInitPara);
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


HAL_ROM_TEXT_SECTION
VOID
SpicRxCmdRtl8195A
(
    IN  u8 cmd
)
{

    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    // set ctrlr0: RX_mode
    HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                    ((HAL_SPI_READ32(REG_SPIC_CTRLR0)&0xFFF0FFFF) | BIT_TMOD(3)));

    // set flash_cmd: write cmd to fifo
    HAL_SPI_WRITE8(REG_SPIC_DR0, cmd);

    // Enable SPI_FLASH  User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

    // Wait spic busy done
    SpicWaitBusyDoneRtl8195A();

    // Disable SPI_FLASH User Mode
//    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

}


HAL_ROM_TEXT_SECTION
VOID
SpicWaitBusyDoneRtl8195A()
{

    u32 SrBsuyCheck = 0;

    do {
        SrBsuyCheck = ((HAL_SPI_READ32(REG_SPIC_SR)) & BIT_BUSY);
    } while(SrBsuyCheck == 1);


}

HAL_ROM_TEXT_SECTION
VOID
SpicTxCmdRtl8195A
(
    IN  u8 cmd,
    IN  SPIC_INIT_PARA SpicInitPara
)
{

    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    // set ctrlr0: TX mode
    HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                    (HAL_SPI_READ32(REG_SPIC_CTRLR0)& 0xFFF0FCFF));

    // set flash_cmd: wren to fifo
    HAL_SPI_WRITE8(REG_SPIC_DR0, cmd);

    // Enable SPI_FLASH  User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

    // wait spic busy done
    SpicWaitBusyDoneRtl8195A();

    // Disable SPI_FLASH User Mode
//    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    // wait flash busy done (wip=0)
    SpicWaitWipDoneRtl8195A(SpicInitPara);

}

HAL_ROM_TEXT_SECTION
VOID
SpicWaitWipDoneRtl8195A
(
    IN  SPIC_INIT_PARA SpicInitPara
)
{
    do {
    } while((SpicGetFlashStatusRtl8195A(SpicInitPara) & 0x01));
}



HAL_ROM_TEXT_SECTION
u8
SpicGetFlashStatusRtl8195A
(
    IN  SPIC_INIT_PARA SpicInitPara
)
{

    u32 RdData;
    u32 RdDummyCycle;
    u32 Value32 = 0;

    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    // Set Ctrlr1; 1 byte data frames
    HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(1));

    // Store rd_dummy_cycle
    Value32 = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
    RdDummyCycle = Value32 & 0x0000FFFF;
    HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, ((Value32 & 0xFFFF0000)|SpicInitPara.RdDummyCyle));

    SpicRxCmdRtl8195A(0x05);
    RdData = HAL_SPI_READ8(REG_SPIC_DR0);

    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    // Recover rd_dummy_cycle
    Value32 = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
    Value32 = Value32 & 0xFFFF0000;
    HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, (Value32 | RdDummyCycle));

    return RdData;
}


HAL_ROM_TEXT_SECTION
VOID
SpicEraseFlashRtl8195A()
{

    SPIC_INIT_PARA SpicInitPara;

#ifdef CONFIG_FPGA
    SpicInitPara.BaudRate = FPGASpicInitPara.BaudRate;
    SpicInitPara.RdDummyCyle = FPGASpicInitPara.RdDummyCyle;
    SpicInitPara.DelayLine = FPGASpicInitPara.DelayLine;
#else


    u8 CpuClk;
    u8  Value8, InitBaudRate;

    InitBaudRate = 1;

    CpuClk = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70)) >> 4);
    SpicLoadInitParaFromClockRtl8195A(CpuClk, InitBaudRate, &SpicInitPara);
#endif
    DBG_8195A("Init Baud Rate: 0x%x\n",SpicInitPara.BaudRate);
    DBG_8195A("Init RD Dummy: 0x%x\n",SpicInitPara.RdDummyCyle);
    DBG_8195A("Init Delay Line: 0x%x\n",SpicInitPara.DelayLine);

    // Wait for flash busy done
    SpicWaitWipDoneRtl8195A(SpicInitPara);

    while((SpicGetFlashStatusRtl8195A(SpicInitPara) & 0x02)==0) {
        // Set flash_cmd: WREN to FIFO
        SpicTxCmdRtl8195A(0x06, SpicInitPara);
    }

    DBG_8195A("Erase Cmd Set\n");
    // Set flash_cmd: Chip_erase to FIFO
    SpicTxCmdRtl8195A(0xC7, SpicInitPara);

    // polling WEL
    do {
    } while((SpicGetFlashStatusRtl8195A(SpicInitPara) & 0x02)!=0);

    // Enable SPI_FLASH
    //HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

}


HAL_ROM_TEXT_SECTION
VOID
SpicSetFlashStatusRtl8195A
(
    IN  u32 data,
    IN  SPIC_INIT_PARA SpicInitPara
)
{

    u32 Info;

    Info = HAL_SPI_READ32(REG_SPIC_ADDR_LENGTH);

    // Set flash_cmd: WREN to FIFO
    SpicTxCmdRtl8195A(0x06, SpicInitPara);

    // Disable SPI_FLASH
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    // set ctrlr0: TX mode
    HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                    (HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~ BIT_TMOD(3))));

    HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, BIT_ADDR_PHASE_LENGTH(1));

    // Set flash_cmd: WRSR to FIFO
    HAL_SPI_WRITE8(REG_SPIC_DR0, BIT_DR0(1));

    // Set data FIFO
    HAL_SPI_WRITE8(REG_SPIC_DR0, BIT_DR0(data));

    // Enable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

    // wait spic busy done
    SpicWaitBusyDoneRtl8195A();

    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, Info);

    // wait flash busy done (wip=0)
    SpicWaitWipDoneRtl8195A(SpicInitPara);

}




HAL_ROM_TEXT_SECTION
u32
SpicCmpDataForCalibrationRtl8195A()
{

    BOOLEAN Pass = _TRUE;
    u32 AddrIndex = 0;
    u32 RdData;
#ifdef CONFIG_FPGA
    u32 GoldData;
#endif

#ifdef CONFIG_FPGA
    for(AddrIndex=0; AddrIndex<TEST_SIZE; AddrIndex=AddrIndex+4) {

        // read golden_data
        GoldData = HAL_READ32(SRAM_STR_BASE, AddrIndex);
        // read spi-flash data
        RdData = HAL_READ32(SPI_TEST_ADDR, AddrIndex);
//        DBG_8195A("%d: SRAM: 0x%x; SPI: 0x%x\n",i, gold_data, rd_data);

        // compare data
        if(RdData!=GoldData) {
            Pass = 0;
//            DBG_8195A("Fail\n");
            break;
        } else {
//            DBG_8195A("Pass\n");
        }
    }
#else
    for(AddrIndex=0; AddrIndex<4; AddrIndex++) {
        RdData = HAL_READ32(SPI_FLASH_BASE, AddrIndex*4);

        // compare data
        if(RdData!=SpicCalibrationPattern[AddrIndex]) {
            Pass = 0;
//            DBG_8195A("Fail\n");
            break;
        }
    }
#endif



    return Pass;

}
#ifdef CONFIG_FPGA

HAL_ROM_TEXT_SECTION
VOID
SpicProgFlashForCalibrationRtl8195A
(
    IN  SPIC_INIT_PARA SpicInitPara
)
{

    u32 AddrIndex;
    u32 WrData;
    u16 WrDataH, WrDataL;

    for(AddrIndex=0; AddrIndex<TEST_SIZE; AddrIndex=AddrIndex+4) {
        WrDataH = Rand();
        WrDataL = Rand();
        WrData = ((WrDataH) << 16) | WrDataL ;
        HAL_WRITE32(SRAM_STR_BASE,AddrIndex, WrData);
        HAL_WRITE32(SPI_TEST_ADDR,AddrIndex, WrData);

        // wait spic busy done
        SpicWaitBusyDoneRtl8195A();
        // wait flash busy done (wip=0)
        SpicWaitWipDoneRtl8195A(SpicInitPara);
    }
}
#endif
HAL_ROM_TEXT_SECTION
VOID
SpicLoadInitParaFromClockRtl8195A
(
    IN  u8 CpuClkMode,
    IN  u8 BaudRate,
    IN  PSPIC_INIT_PARA pSpicInitPara
)
{
    PSPIC_INIT_PARA pTempSpinInitPara = NULL;
    u8  Value8, BootCpuClk;
    u8  DelayCycle = 0;
    u8  DelayLine = 0;

    // Load Efuse Setting
    Value8 = ((u8)((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & 0xFF000000)
                   >> 24));
    BootCpuClk = ((Value8 & 0xC) >> 2);

    pTempSpinInitPara = (PSPIC_INIT_PARA) &SpicInitCPUCLK[BootCpuClk];

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

    pSpicInitPara->BaudRate = pTempSpinInitPara->BaudRate;
    pSpicInitPara->DelayLine = DelayLine;
    pSpicInitPara->RdDummyCyle = (pTempSpinInitPara->RdDummyCyle + DelayCycle);

}
