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


#define SPI_CTRL_BASE 0x1FFEF000
#define SPI_DLY_CTRL_ADDR 0x40000300	// [7:0]
#define MIN_BAUDRATE  0x01
#define MAX_BAUDRATE  0x04
#define MAX_AUTOLEN   0x14
#define MAX_DLYLINE   99
#define GOLD_ID_NO_RAM    0xC220

#define WR_DATA(addr, data)    (*((volatile u32*)(addr)) = (data))
#define RD_DATA(addr)          (*((volatile u32*)(addr)))

BOOLEAN SpicFlashInitRtl8195A(u8 SpicBitMode);
u32 SpicCalibrationRtl8195A(u8 SpicBitMode, u32 DefRdDummyCycle);          // spi-flash calibration
VOID SpicSetFlashStatusRefinedRtl8195A(u32 data, SPIC_INIT_PARA SpicInitPara);
VOID SpicConfigAutoModeRtl8195A(u8 SpicBitMode);      // config spi-flash controller to auto mode
VOID SpicReadIDRtl8195A(VOID);
u8 SpicGetFlashFlagRtl8195A(SPIC_INIT_PARA SpicInitPara);

_LONG_CALL_
extern VOID SpicWaitBusyDoneRtl8195A(VOID);    // wait sr[0] = 0, wait transmission done

_LONG_CALL_
extern u32 SpicCmpDataForCalibrationRtl8195A(void);            // compare read_data and golden_data
#ifdef CONFIG_FPGA
_LONG_CALL_
extern VOID SpicProgFlashForCalibrationRtl8195A(SPIC_INIT_PARA SpicInitPara);         // program spi-flash
#endif
_LONG_CALL_
extern VOID SpicLoadInitParaFromClockRtl8195A(u8 CpuClkMode, u8 BaudRate, PSPIC_INIT_PARA pSpicInitPara);

struct ava_window {
    u16 baud_rate;
    u16 auto_length;
    u32 dly_line_sp;
    u32 dly_line_ep;
};

#ifdef CONFIG_FPGA
HAL_ROM_DATA_SECTION
SPIC_INIT_PARA FPGASpicInitPara = {1,1,0,0};
#endif

#if 0
HAL_FLASH_DATA_
SECTION SPIC_INIT_PARA SpicInitParaAllClk[CPU_CLK_TYPE_NO] =    {{0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
};
#else
HAL_FLASH_DATA_SECTION
SPIC_INIT_PARA SpicInitParaAllClk[3][CPU_CLK_TYPE_NO];
#endif

extern SPIC_INIT_PARA SpicInitCPUCLK[4];

/* Send Flash Instruction with Data Phase */
HAL_FLASH_TEXT_SECTION
VOID
SpicTxCmdWithDataRtl8195A
(
    IN  u8 cmd,
    IN  u8 DataPhaseLen,
    IN  u8* pData,
    IN  SPIC_INIT_PARA SpicInitPara
)
{
    u8 i;

    DBG_SPIF_INFO("%s(0x%x, 0x%x, 0x%x, 0x%x)\n",__func__, cmd, DataPhaseLen, pData, SpicInitPara);

    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    if (DataPhaseLen > 15) {
        DBG_SPIF_WARN("SpicTxInstRtl8195A: Data Phase Leng too Big(%d)\n",DataPhaseLen);
        DataPhaseLen = 15;
    }

    HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, DataPhaseLen);

    // set ctrlr0: TX mode
    HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                    (HAL_SPI_READ32(REG_SPIC_CTRLR0)& 0xFFF0FCFF));

    // set flash_cmd: wren to fifo
    HAL_SPI_WRITE8(REG_SPIC_DR0, cmd);

    //fill addr
    for (i=0; i<DataPhaseLen; i++) {
        HAL_SPI_WRITE8(REG_SPIC_DR0, (u8)*(pData+i));
    }

    // Enable SPI_FLASH  User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

    // wait spic busy done
    SpicWaitBusyDoneRtl8195A();

    // Disable SPI_FLASH User Mode
//    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    // wait flash busy done (wip=0)
    //SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    if((SpicInitParaAllClk[0][0].flashtype) == FLASH_MICRON)
        SpicWaitOperationDoneRtl8195A(SpicInitPara);
    else
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);

}

HAL_FLASH_TEXT_SECTION
VOID
SpicTxFlashInstRtl8195A(
    IN  u8 cmd,
    IN  u8 DataPhaseLen,
    IN  u8* pData
)
{
    SPIC_INIT_PARA SpicInitPara;

    // Wait for flash busy done
    SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    DBG_8195A();
    while((SpicGetFlashStatusRefinedRtl8195A(SpicInitPara) & 0x02)==0) {
        // Set flash_cmd: WREN to FIFO
        SpicTxCmdWithDataRtl8195A(FLASH_CMD_WREN, 0, 0, SpicInitPara);
    }

    // Set flash_cmd to FIFO
    SpicTxCmdWithDataRtl8195A(cmd, DataPhaseLen, pData, SpicInitPara);

    if((SpicInitParaAllClk[0][0].flashtype) == FLASH_MICRON)
        SpicWaitOperationDoneRtl8195A(SpicInitPara);
    else if((SpicInitParaAllClk[0][0].flashtype) == FLASH_EON) {
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    } else {
        // polling WEL
        do {
        } while((SpicGetFlashStatusRefinedRtl8195A(SpicInitPara) & 0x02)!=0);
    }
}

/**
  * @brief  SpicRxCmdRefinedRtl8195A. To send flash RX command.
  *            Timing store/restore is implemented inside.
  *
  * @param  IN  u8 cmd: flash RX command
  *
  * @retval NA
  */
HAL_FLASH_TEXT_SECTION
VOID
SpicRxCmdRefinedRtl8195A(
    IN  u8 cmd,
    IN  SPIC_INIT_PARA SpicInitPara
)
{
    u32 RdDummyCycle;
    u32 BaudRate;
    u32 BaudRate12bit;
    u32 DelayLine;
    u32 DelayLine8bit;

    u32 AutoLength = 0;
    u8  CpuClk = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70)) >> 4);
    PSPIC_INIT_PARA PSpicInitParaLocal = NULL;
    SPIC_INIT_PARA TmpSpicInitPara;

#ifdef CONFIG_FPGA
    PSpicInitParaLocal = &TmpSpicInitPara;
    PSpicInitParaLocal->BaudRate = FPGASpicInitPara.BaudRate;
    PSpicInitParaLocal->RdDummyCyle = FPGASpicInitPara.RdDummyCyle;
    PSpicInitParaLocal->DelayLine = FPGASpicInitPara.DelayLine;
#else

    if (SpicInitParaAllClk[SpicOneBitMode][CpuClk].Valid) {
        PSpicInitParaLocal = &(SpicInitParaAllClk[SpicOneBitMode][CpuClk]);
    } else {
        PSpicInitParaLocal = &TmpSpicInitPara;
        SpicLoadInitParaFromClockRtl8195A(CpuClk, 1, PSpicInitParaLocal);
    }
#endif

#if 0
    DBG_8195A("!cpuclk:%x\n",CpuClk);
    DBG_8195A("!baud:%x\n",PSpicInitParaLocal->BaudRate);
    DBG_8195A("!delay:%x\n",PSpicInitParaLocal->DelayLine);
    DBG_8195A("!dummy:%x\n",PSpicInitParaLocal->RdDummyCyle);
#endif

    DBG_SPIF_INFO("%s(0x%x, 0x%x)\n", __func__, cmd, PSpicInitParaLocal);

    /* Store rd_dummy_cycle */
    AutoLength = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
    RdDummyCycle = AutoLength & BIT_MASK_RD_DUMMY_LENGTH;
    HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, ((AutoLength & (~BIT_MASK_RD_DUMMY_LENGTH))|(PSpicInitParaLocal->RdDummyCyle)));

    /* Store baud rate */
    BaudRate        = HAL_SPI_READ32(REG_SPIC_BAUDR);
    BaudRate12bit   = (BaudRate & BIT_MASK_SCKDV);
    HAL_SPI_WRITE32(REG_SPIC_BAUDR,((BaudRate & (~BIT_MASK_SCKDV))|(PSpicInitParaLocal->BaudRate)));

    /* Store delay line */
    DelayLine       = HAL_READ32(SYSTEM_CTRL_BASE, REG_PESOC_MEM_CTRL);
    DelayLine8bit   = (DelayLine & BIT_MASK_PESOC_FLASH_DDL_CTRL);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_PESOC_MEM_CTRL, ((DelayLine & (~BIT_MASK_PESOC_FLASH_DDL_CTRL))|(PSpicInitParaLocal->DelayLine)));
    //HAL_WRITE32(SYSTEM_CTRL_BASE, REG_PESOC_MEM_CTRL, ((DelayLine & (~BIT_MASK_PESOC_FLASH_DDL_CTRL))|DelayLine8bit));

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    /* set ctrlr0: RX_mode */
    HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                    ((HAL_SPI_READ32(REG_SPIC_CTRLR0)&0xFFF0FFFF) | BIT_TMOD(3)));

    /* set flash_cmd: write cmd to fifo */
    HAL_SPI_WRITE8(REG_SPIC_DR0, cmd);

    /* Enable SPI_FLASH  User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

    /* Wait spic busy done */
    SpicWaitBusyDoneRtl8195A();

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    /* Recover rd_dummy_cycle */
    AutoLength = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
    AutoLength = AutoLength & 0xFFFF0000;
    HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, (AutoLength | RdDummyCycle));

    /* Recover baud rate */
    BaudRate        = HAL_SPI_READ32(REG_SPIC_BAUDR);
    BaudRate        = (BaudRate & (~BIT_MASK_SCKDV));
    HAL_SPI_WRITE32(REG_SPIC_BAUDR, (BaudRate|BaudRate12bit));

    /* Recover delay line */
    DelayLine       = HAL_READ32(SYSTEM_CTRL_BASE, REG_PESOC_MEM_CTRL);
    DelayLine       = (DelayLine & (~BIT_MASK_PESOC_FLASH_DDL_CTRL));
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_PESOC_MEM_CTRL, (DelayLine|DelayLine8bit));
}

HAL_FLASH_TEXT_SECTION
VOID
SpicDeepPowerDownFlashRtl8195A(
    VOID
)
{
    SPIC_INIT_PARA SpicInitPara;

    if ((HAL_READ32(SYSTEM_CTRL_BASE,REG_SYS_EFUSE_SYSCFG6) & BIT(25))&&
            ((((u8)HAL_READ32(SYSTEM_CTRL_BASE,REG_SYS_SYSTEM_CFG0))>>4)>= 2)) {

        SPI_FLASH_PIN_FCTRL(ON);

        // Wait for flash busy done
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);


        DBG_8195A("Deep power down\n");

        // Disable SPI_FLASH User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

        // set ctrlr0: TX mode
        HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                        (HAL_SPI_READ32(REG_SPIC_CTRLR0)& 0xFFF0FCFF));

        // set flash_cmd: wren to fifo
        HAL_SPI_WRITE8(REG_SPIC_DR0, FLASH_CMD_DP);

        // Enable SPI_FLASH  User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

        HalDelayUs(20);
    }
}

/**
  * @brief  SpicEraseFlashRefinedRtl8195A.
  *
  * @param    NA
  *
  * @retval NA
  */
HAL_FLASH_TEXT_SECTION
VOID
SpicEraseFlashRefinedRtl8195A(VOID)
{
    SpicTxFlashInstRtl8195A(FLASH_CMD_CE,0, 0);
}


//This funciton is only valid for Micron Flash
HAL_FLASH_TEXT_SECTION
VOID
SpicDieEraseFlashRtl8195A(
    IN u32 Address
)
{
    u8 Addr[3];

    Addr[0] = (Address >> 16) & 0xFF;
    Addr[1] = (Address >> 8) & 0xFF;
    Addr[2] = Address & 0xFF;
    SpicTxFlashInstRtl8195A(FLASH_CMD_DE, 3, Addr);
}

HAL_FLASH_TEXT_SECTION
VOID
SpicBlockEraseFlashRtl8195A(
    IN u32 Address
)
{
    u8 Addr[3];

    //DBG_8195A("Erase Cmd Set\n");
    // Set flash_cmd: Chip_erase to FIFO
    Addr[0] = (Address >> 16) & 0xFF;
    Addr[1] = (Address >> 8) & 0xFF;
    Addr[2] = Address & 0xFF;
    SpicTxFlashInstRtl8195A(FLASH_CMD_BE, 3, Addr);
}


HAL_FLASH_TEXT_SECTION
VOID
SpicSectorEraseFlashRtl8195A(
    IN u32 Address
)
{
    u8 Addr[3];

    Addr[0] = (Address >> 16) & 0xFF;
    Addr[1] = (Address >> 8) & 0xFF;
    Addr[2] = Address & 0xFF;
    SpicTxFlashInstRtl8195A(FLASH_CMD_SE, 3, Addr);
}

HAL_FLASH_TEXT_SECTION
VOID
SpicWriteProtectFlashRtl8195A(
    IN u32 Protect
)
{

    SPIC_INIT_PARA SpicInitPara;
    u8 Status;

    Status = SpicGetFlashStatusRefinedRtl8195A(SpicInitPara);
    if (Protect) {
        Status |= 0x1c; // protect whole chip
    } else {
        Status &= ~0x1c; // Protect none
    }
    SpicSetFlashStatusRefinedRtl8195A(Status,SpicInitPara);

}


HAL_FLASH_TEXT_SECTION
BOOLEAN
SpicFlashInitRtl8195A(
    IN  u8 SpicBitMode
)
{
    u32 DefRdDummyCycle = 0;
    SPIC_INIT_PARA SpicInitPara;

    if(SpicInitParaAllClk[0][0].id[0] == 0x00)
        SpicReadIDRtl8195A();

#ifdef CONFIG_FPGA
    SpicInitPara.BaudRate = FPGASpicInitPara.BaudRate;
    SpicInitPara.RdDummyCyle = FPGASpicInitPara.RdDummyCyle;
    SpicInitPara.DelayLine = FPGASpicInitPara.DelayLine;
#endif

    SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);

    switch (SpicBitMode) {
        case SpicOneBitMode:
//            DBG_8195A("Initial Spic One bit mode\n");

            // set auto mode
            SpicConfigAutoModeRtl8195A(SpicBitMode);

            /* MXIC spec */
            DefRdDummyCycle = 0;
            break;
        case SpicDualBitMode:
//            DBG_8195A("Initial Spic Two bit mode\n");
#ifdef CONFIG_FPGA
            // program golden_data to golden_address and store golden_data in sram
            SpicProgFlashForCalibrationRtl8195A(SpicInitPara);
#endif
            // set auto mode
            SpicConfigAutoModeRtl8195A(SpicBitMode);

            /* MXIC spec */
#if 0
#if FLASH_RD_2IO_EN
            DefRdDummyCycle = FLASH_DM_CYCLE_2IO;
#endif
#if FLASH_RD_2O_EN
            DefRdDummyCycle = FLASH_DM_CYCLE_2O;
#endif
#endif
            DefRdDummyCycle = FLASH_DM_CYCLE_2O;
            break;
        case SpicQuadBitMode:
            //DBG_8195A("Initial Spic Four bit mode\n");
#ifdef CONFIG_FPGA
            // program golden_data to golden_address and store golden_data in sram
            SpicProgFlashForCalibrationRtl8195A(SpicInitPara);
#endif
            // set auto mode
            SpicConfigAutoModeRtl8195A(SpicBitMode);

            /* MXIC spec */
#if 0
#if FLASH_RD_4IO_EN
            DefRdDummyCycle = FLASH_DM_CYCLE_4IO;
#endif
#if FLASH_RD_4O_EN
            DefRdDummyCycle = FLASH_DM_CYCLE_4O;
#endif
#endif
            DefRdDummyCycle = FLASH_DM_CYCLE_4O;
            if(SpicInitParaAllClk[0][0].flashtype == FLASH_EON)
                DefRdDummyCycle = 6;
            break;
        default:
            DBG_8195A("No Support SPI Mode!!!!!!!!\n");
            break;

    }

    if (!SpicCalibrationRtl8195A(SpicBitMode, DefRdDummyCycle)) {

        DBG_8195A("SPI calibration fail and recover one bit mode\n");
        SpicLoadInitParaFromClockRtl8195A(0, 0, &SpicInitPara);
        SpicInitRefinedRtl8195A(SpicInitPara.BaudRate, SpicOneBitMode);
        SpicConfigAutoModeRtl8195A(SpicOneBitMode);

        return _FALSE;
    }

    return _TRUE;
}

/**
  * @brief  SpicInitRefinedRtl8195A.
  *
  * @param    IN  u8 InitBaudRate,
  *                IN  u8 SpicBitMode
  *
  * @retval NA
  */
HAL_FLASH_TEXT_SECTION
VOID
SpicInitRefinedRtl8195A(
    IN  u8 InitBaudRate,
    IN  u8 SpicBitMode
)
{

    u32 Value32;
    SPIC_INIT_PARA SpicInitPara;
    PSPIC_INIT_PARA PSpicInitParaLocal;

    PSpicInitParaLocal = &SpicInitPara;
#ifdef CONFIG_FPGA
    PSpicInitParaLocal->BaudRate = FPGASpicInitPara.BaudRate;
    PSpicInitParaLocal->RdDummyCyle = FPGASpicInitPara.RdDummyCyle;
    PSpicInitParaLocal->DelayLine = FPGASpicInitPara.DelayLine;
#else
    u8 CpuClk;

    CpuClk = (((u8)(HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70))) >> 4);

    if (SpicInitParaAllClk[SpicBitMode][CpuClk].Valid) {
        PSpicInitParaLocal = &(SpicInitParaAllClk[SpicBitMode][CpuClk]);
    } else {
        SpicLoadInitParaFromClockRtl8195A(CpuClk, 1, PSpicInitParaLocal);
    }

#endif

    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    HAL_SPI_WRITE32(REG_SPIC_BAUDR, BIT_SCKDV(InitBaudRate));

    HAL_SPI_WRITE32(REG_SPIC_SER, BIT_SER);

    Value32 = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
    HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH,
                    ((Value32 & 0xFFFF0000) | BIT_RD_DUMMY_LENGTH(PSpicInitParaLocal->RdDummyCyle)));

    HAL_WRITE32(PERI_ON_BASE, REG_PESOC_MEM_CTRL,
                ((HAL_READ32(PERI_ON_BASE, REG_PESOC_MEM_CTRL)&0xFFFFFF00)|
                 PSpicInitParaLocal->DelayLine));

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
}

HAL_FLASH_TEXT_SECTION
VOID
SpicReadIDRtl8195A(
    VOID
)
{
    u32 RdData;
    u32 RetryNum;
    SPIC_INIT_PARA SpicInitPara;// = *PSpicInitPara;
    u8 i,j;

    DBG_SPIF_INFO("%s(0x%x)\n", __func__, SpicInitPara);

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    /* Set Ctrlr1; 1 byte data frames */
    HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(3));

    /* Send flash RX command and read the data */
    SpicRxCmdRefinedRtl8195A(FLASH_CMD_RDID, SpicInitPara);
    RdData = HAL_SPI_READ32(REG_SPIC_DR0);

    SpicInitPara.id[0] = RdData & 0xFF;
    SpicInitPara.id[1] = (RdData>> 8) & 0xFF;
    SpicInitPara.id[2] = (RdData>>16) & 0xFF;
    for(RetryNum =0; RetryNum < 3; RetryNum++) {
        if((SpicInitPara.id[0] != 0) && (SpicInitPara.id[0] != 0xFF)) {
            if(SpicInitPara.id[0] == 0x20)
                SpicInitPara.flashtype = FLASH_MICRON;
            else if(SpicInitPara.id[0] == 0xC2) {
                if(SpicInitPara.id[2] == 0x18)
                    SpicInitPara.flashtype = FLASH_MXIC_4IO;
                else
                    SpicInitPara.flashtype = FLASH_MXIC;
            } else if(SpicInitPara.id[0] == 0xEF)
                SpicInitPara.flashtype = FLASH_WINBOND;
            else if(SpicInitPara.id[0] == 0x1C)
                SpicInitPara.flashtype = FLASH_EON;
            else
                SpicInitPara.flashtype = FLASH_OTHERS;
            break;
        } else {
            if(RetryNum == 2)
                DBG_8195A("Invalid ID\n");
        }
    }
    for(i=0; i<3; i++) {
        for (j=0; j<CPU_CLK_TYPE_NO; j++) {
            SpicInitParaAllClk[i][j].id[0] = SpicInitPara.id[0];
            SpicInitParaAllClk[i][j].id[1] = SpicInitPara.id[1];
            SpicInitParaAllClk[i][j].id[2] = SpicInitPara.id[2];
            SpicInitParaAllClk[i][j].flashtype= SpicInitPara.flashtype;
        }
    }

    //DBG_8195A("Flash type = %x\n",SpicInitParaAllClk[0][0].flashtype);
    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

}

HAL_FLASH_TEXT_SECTION
u32
SpicCalibrationRtl8195A
(
    IN  u8 SpicBitMode,
    IN  u32 DefRdDummyCycle
)
{

    u32 rd_data, /*id_no,*/ baudr, autolen, dly_line;
    u32 total_ava_wds=0;
    u32 tmp_str_pt, tmp_end_pt, pass, last_pass;
    struct ava_window max_wd;
    u32  CpuType;
    SPIC_INIT_PARA SpicInitPara;

    CpuType = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1) & (0x70)) >> 4);

#if SPIC_CALIBRATION_IN_NVM
    if (!SpicInitParaAllClk[SpicBitMode][CpuType].Valid) {
        SpicNVMCalLoad(SpicBitMode, CpuType);
    }
#endif
    if (SpicInitParaAllClk[SpicBitMode][CpuType].Valid) {
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
        HAL_SPI_WRITE32(REG_SPIC_BAUDR, (SpicInitParaAllClk[SpicBitMode][CpuType].BaudRate & 0x00000FFF));
        rd_data = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
        rd_data = (rd_data & 0xFFFF0000) | (SpicInitParaAllClk[SpicBitMode][CpuType].RdDummyCyle & 0x0000FFFF);
        HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, rd_data);
        rd_data = SpicInitParaAllClk[SpicBitMode][CpuType].DelayLine;
        WR_DATA(SPI_DLY_CTRL_ADDR, ((RD_DATA(SPI_DLY_CTRL_ADDR) & 0xFFFFFF00) | (rd_data & 0x000000FF)));
        // Enable SPI_FLASH  User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
        pass = SpicCmpDataForCalibrationRtl8195A();
        if (pass) {
            // Check the Magic Pattern OK
            return 1;
        }
    }

    // calibration
    DBG_8195A("SPI calibration\n");

    max_wd.auto_length = 0;
    max_wd.baud_rate = 0;
    max_wd.dly_line_ep = 0;
    max_wd.dly_line_sp = 0;

    for(baudr=MIN_BAUDRATE; baudr < (MAX_BAUDRATE+1); baudr++) {
        // Disable SPI_FLASH User Mode
        if(baudr == MIN_BAUDRATE)
            if((SpicBitMode == SpicOneBitMode) &&(CpuType == 0))
                continue;
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
        HAL_SPI_WRITE32(REG_SPIC_BAUDR, BIT_SCKDV(baudr));
        // Enable SPI_FLASH  User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
        //DBG_8195A("(0x14)Baudr: 0x%x\n",HAL_SPI_READ32(REG_SPIC_BAUDR));

        for(autolen=(DefRdDummyCycle*2*baudr); autolen<(DefRdDummyCycle*2*baudr+MAX_AUTOLEN); autolen++) {
            // Disable SPI_FLASH User Mode
            HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
            rd_data = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
            rd_data = (rd_data & 0xFFFF0000) | (0x0000FFFF & autolen);
            HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, rd_data);
            // Enable SPI_FLASH  User Mode
            HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
            //DBG_8195A("Auto length: 0x%x\n",autolen);
            //DBG_8195A("(0x11C) Auto address length register: 0x%x\n",HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH));
            tmp_str_pt = MAX_DLYLINE;
            tmp_end_pt = 0;
            last_pass = 0;

            for(dly_line=0; dly_line<=MAX_DLYLINE; dly_line++) {
                rd_data = RD_DATA(SPI_DLY_CTRL_ADDR);
                rd_data = (rd_data & 0xFFFFFF00) | (dly_line & 0x000000FF);
                WR_DATA(SPI_DLY_CTRL_ADDR, rd_data);
                //DBG_8195A("SPI_DLY_CTRL_ADDR: 0x%x\n",RD_DATA(SPI_DLY_CTRL_ADDR));

                pass = SpicCmpDataForCalibrationRtl8195A();


                if(pass) {	// PASS
                    if(last_pass==0) {
                        tmp_str_pt = dly_line;
                        total_ava_wds++;
                    }

                    if(dly_line==MAX_DLYLINE) {

                        tmp_end_pt = dly_line;

                        if(total_ava_wds==1) {
                            max_wd.baud_rate = baudr;
                            max_wd.auto_length = autolen;
                            max_wd.dly_line_sp = tmp_str_pt;
                            max_wd.dly_line_ep = tmp_end_pt;
                        } else {
                            if((tmp_end_pt-tmp_str_pt)>(max_wd.dly_line_ep-max_wd.dly_line_sp)) {
                                max_wd.baud_rate = baudr;
                                max_wd.auto_length = autolen;
                                max_wd.dly_line_sp = tmp_str_pt;
                                max_wd.dly_line_ep = tmp_end_pt;
                            }
                        }
                    }
                    last_pass = 1;
                } else {			// FAIL
                    if(last_pass==1) {
                        tmp_end_pt = dly_line;
                        if(total_ava_wds == 1) {
                            max_wd.baud_rate = baudr;
                            max_wd.auto_length = autolen;
                            max_wd.dly_line_sp = tmp_str_pt;
                            max_wd.dly_line_ep = tmp_end_pt;
                        } else {
                            if((tmp_end_pt-tmp_str_pt)>(max_wd.dly_line_ep-max_wd.dly_line_sp)) {
                                max_wd.baud_rate = baudr;
                                max_wd.auto_length = autolen;
                                max_wd.dly_line_sp = tmp_str_pt;
                                max_wd.dly_line_ep = tmp_end_pt;
                            }
                        }
                    }
                    last_pass = 0;
                }
            }
            //DBG_8195A("total wds: %d\n",total_ava_wds);
            //DBG_8195A("Baud:%x; auto_length:%x; Delay start:%x; Delay end:%x\n",max_wd.baud_rate, max_wd.auto_length,max_wd.dly_line_sp, max_wd.dly_line_ep);
        }
        if (total_ava_wds) {
            DBG_8195A("Find the avaiable window\n");
            break;
        }
    }


    if(total_ava_wds==0) {
        return 0;
    } else {
        // set baudr, auto_length, and delay_line
        DBG_8195A("Baud:%x; auto_length:%x; Delay start:%x; Delay end:%x\n",max_wd.baud_rate, max_wd.auto_length,max_wd.dly_line_sp, max_wd.dly_line_ep);
        // Disable SPI_FLASH User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
        HAL_SPI_WRITE32(REG_SPIC_BAUDR, (max_wd.baud_rate & 0x00000FFF));
        SpicInitParaAllClk[SpicBitMode][CpuType].BaudRate = max_wd.baud_rate;
        rd_data = HAL_SPI_READ32(REG_SPIC_AUTO_LENGTH);
        rd_data = (rd_data & 0xFFFF0000) | (max_wd.auto_length & 0x0000FFFF);
        HAL_SPI_WRITE32(REG_SPIC_AUTO_LENGTH, rd_data);
        SpicInitParaAllClk[SpicBitMode][CpuType].RdDummyCyle = max_wd.auto_length;
        rd_data = ((max_wd.dly_line_sp + max_wd.dly_line_ep) >> 1);
        WR_DATA(SPI_DLY_CTRL_ADDR, ((RD_DATA(SPI_DLY_CTRL_ADDR) & 0xFFFFFF00) | (rd_data & 0x000000FF)));
        SpicInitParaAllClk[SpicBitMode][CpuType].DelayLine = rd_data;
        SpicInitParaAllClk[SpicBitMode][CpuType].Valid = 1;
        // Enable SPI_FLASH  User Mode
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
#if SPIC_CALIBRATION_IN_NVM
        SpicNVMCalStore(SpicBitMode, CpuType);
#endif
        return 1;
    }

}

HAL_FLASH_TEXT_SECTION
u32
SpicOneBitCalibrationRtl8195A(
    IN u8 SysCpuClk
)
{
    u32 DefRdDummyCycle = 0;


    // set auto mode
    SpicConfigAutoModeRtl8195A(SpicOneBitMode);

    /* MXIC spec */
    DefRdDummyCycle = 0;

    if (!SpicCalibrationRtl8195A(SpicOneBitMode, DefRdDummyCycle)) {
        return _FALSE;
    }

#if 0
    DBG_8195A("@baud:%x\n",SpicInitParaAllClk[0][SysCpuClk].BaudRate);
    DBG_8195A("@delay:%x\n",SpicInitParaAllClk[0][SysCpuClk].DelayLine);
    DBG_8195A("@dummy:%x\n\n",SpicInitParaAllClk[0][SysCpuClk].RdDummyCyle);
#endif
    return _TRUE;
}

HAL_FLASH_TEXT_SECTION
VOID
SpicConfigAutoModeRtl8195A
(
    IN  u8 SpicBitMode
)
{


    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    HAL_SPI_WRITE32(REG_SPIC_VALID_CMD,
                    (HAL_SPI_READ32(REG_SPIC_VALID_CMD) & (~(
                                BIT_WR_QUAD_II |
                                BIT_WR_QUAD_I  |
                                BIT_WR_DUAL_II |
                                BIT_WR_DUAL_I  |
                                BIT_RD_QUAD_IO |
                                BIT_RD_QUAD_O  |
                                BIT_RD_DUAL_IO |
                                BIT_RD_DUAL_I))));//Disable all the four and two bit commands.


    if (SpicDualBitMode == SpicBitMode) {

        //#if FLASH_WR_2IO_EN
        //HAL_SPI_WRITE32(REG_SPIC_WRITE_DUAL_ADDR_DATA, FLASH_CMD_2PP);
        //#endif

        //#if FLASH_RD_2IO_EN
        //HAL_SPI_WRITE32(REG_SPIC_READ_DUAL_ADDR_DATA, FLASH_CMD_2READ);
        //#endif

        //#if FLASH_WR_2O_EN
        HAL_SPI_WRITE32(REG_SPIC_WRITE_DUAL_ADDR_DATA, FLASH_CMD_DPP);
        //#endif

        //#if FLASH_RD_2O_EN
        HAL_SPI_WRITE32(REG_SPIC_READ_DUAL_ADDR_DATA, FLASH_CMD_DREAD);
        //#endif
        HAL_SPI_WRITE32(REG_SPIC_VALID_CMD,
                        (HAL_SPI_READ32(REG_SPIC_VALID_CMD)|(FLASH_VLD_DUAL_CMDS)));

    }

    if (SpicQuadBitMode == SpicBitMode) {
        //#if FLASH_WR_4IO_EN
        HAL_SPI_WRITE32(REG_SPIC_WRITE_QUAD_ADDR_DATA, FLASH_CMD_4PP);
        //#endif

        //#if FLASH_RD_4IO_EN
        HAL_SPI_WRITE32(REG_SPIC_READ_QUAD_ADDR_DATA, FLASH_CMD_4READ);
        //#endif

        //#if FLASH_WR_4O_EN
        HAL_SPI_WRITE32(REG_SPIC_WRITE_QUAD_DATA, FLASH_CMD_QPP);
        //#endif
        //#if FLASH_RD_4O_EN
        HAL_SPI_WRITE32(REG_SPIC_READ_QUAD_DATA, FLASH_CMD_QREAD);
        //#endif
        if(SpicInitParaAllClk[0][0].flashtype == FLASH_MXIC_4IO) {
            HAL_SPI_WRITE32(REG_SPIC_VALID_CMD, (HAL_SPI_READ32(REG_SPIC_VALID_CMD) |FLASH_VLD_QUAD_CMDS2));
        } else { //Micron 4IO
            HAL_SPI_WRITE32(REG_SPIC_VALID_CMD, (HAL_SPI_READ32(REG_SPIC_VALID_CMD) |FLASH_VLD_QUAD_CMDS));
        }
    }


}

HAL_FLASH_TEXT_SECTION
VOID
SpicUserReadRtl8195A(
    IN  u32 Length,
    IN  u32 addr,
    IN  u8 *data
)
{

    u32 RdData;
    u32 instruction;
    u32 UnalignOffset;
    u8 addrbyte[3];
    u8 *ptr;
    u8 *buf;
    u8 i;
    u8 NDFLimit = 32;
    u8 TransferByte = 0;
    u32* temp;

    UnalignOffset = addr & 0x3;
    buf = data;
    temp = (u32*) buf;
    ptr = (u8*) &RdData;

    if(UnalignOffset) {
        addr-=UnalignOffset;
    }
    addrbyte[2] = (addr & 0xFF0000) >>16;
    addrbyte[1] = (addr & 0xFF00)>>8;
    addrbyte[0] = addr & 0xFF;
    instruction = FLASH_CMD_READ | (addrbyte[2] << 8)|(addrbyte[1] << 16)|(addrbyte[0] << 24);

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                    ((HAL_SPI_READ32(REG_SPIC_CTRLR0)&0xFFF0FCFF) | BIT_TMOD(3)));

    if(UnalignOffset) {
        if(Length <= NDFLimit) {
            HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(Length));
        } else {
            HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(NDFLimit - 4 + UnalignOffset));
        }
    } else {
        if(Length <= NDFLimit) {
            HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(Length));
        } else {
            HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(NDFLimit));
        }
    }

    HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, 3);

    HAL_SPI_WRITE32(REG_SPIC_SER, 1);

    HAL_SPI_WRITE32(REG_SPIC_DR0, instruction);

    // Enable SPI_FLASH  User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

    SpicWaitBusyDoneRtl8195A();

    if(UnalignOffset) {
        RdData = HAL_SPI_READ32(REG_SPIC_DR0);
        for(i = 0; i < (4-UnalignOffset); i++) {
            *buf = *(ptr + UnalignOffset + i);
            buf++;
        }
        Length -= (4-UnalignOffset);
        TransferByte += UnalignOffset;
    }
    while(Length > 0) {
        if(TransferByte > (NDFLimit - 4)) {
            addr+= TransferByte;
            TransferByte = 0;
            addrbyte[2] = (addr & 0xFF0000) >>16;
            addrbyte[1] = (addr & 0xFF00)>>8;
            addrbyte[0] = addr & 0xFF;
            instruction = FLASH_CMD_READ | (addrbyte[2] << 8)|(addrbyte[1] << 16)|(addrbyte[0] << 24);
            HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
            if(Length < NDFLimit) {
                HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(Length));
            } else {
                HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(NDFLimit));
            }

            HAL_SPI_WRITE32(REG_SPIC_DR0, instruction);
            HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

            SpicWaitBusyDoneRtl8195A();
        }
        RdData = HAL_SPI_READ32(REG_SPIC_DR0);
        if(Length >=4) {
            temp = (u32*) buf;
            *temp = RdData;
            temp++;
            buf+=4;
            Length -=4;
            TransferByte += 4;
        } else {
            buf = (u8*) temp;
            for(i = 0; i < Length; i++) {
                *buf = *(ptr+i);
                buf++;
            }
            Length = 0;
        }
    }

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
    HAL_SPI_WRITE32(REG_SPIC_FLUSH_FIFO, 1);

}


HAL_FLASH_TEXT_SECTION
VOID
SpicUserReadFourByteRtl8195A(
    IN  u32 Length,
    IN  u32 addr,
    IN  u32 *data,
    IN  u8 BitMode
)
{

    u32 instruction;
    u32 TransferByte = 0;
    u8 addrbyte[3];
    u8 NDFLimit = 32;
    u8 ReadCMD = 0;

    addrbyte[2] = (addr & 0xFF0000) >>16;
    addrbyte[1] = (addr & 0xFF00)>>8;
    addrbyte[0] = addr & 0xFF;

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                    ((HAL_SPI_READ32(REG_SPIC_CTRLR0)&0xFFF0FCFF) | BIT_TMOD(3)));

    if(SpicDualBitMode == BitMode) {
        HAL_SPI_WRITE32(REG_SPIC_CTRLR0, HAL_SPI_READ32(REG_SPIC_CTRLR0) | BIT_DATA_CH(1) | BIT_ADDR_CH(0));
        ReadCMD = FLASH_CMD_DREAD;
    } else if(SpicOneBitMode == BitMode) {
        HAL_SPI_WRITE32(REG_SPIC_CTRLR0, HAL_SPI_READ32(REG_SPIC_CTRLR0) | BIT_DATA_CH(0) | BIT_ADDR_CH(0));
        ReadCMD = FLASH_CMD_READ;
    } else { // Quad bit mode
        if(SpicInitParaAllClk[0][0].flashtype == FLASH_MXIC_4IO) {
            HAL_SPI_WRITE32(REG_SPIC_CTRLR0, HAL_SPI_READ32(REG_SPIC_CTRLR0) | BIT_DATA_CH(2) | BIT_ADDR_CH(0));
            ReadCMD = FLASH_CMD_QREAD;
        } else { //Micron
            HAL_SPI_WRITE32(REG_SPIC_CTRLR0, HAL_SPI_READ32(REG_SPIC_CTRLR0) | BIT_DATA_CH(2) | BIT_ADDR_CH(2));
            ReadCMD = FLASH_CMD_4READ;
        }
    }

    instruction = ReadCMD | (addrbyte[2] << 8)|(addrbyte[1] << 16)|(addrbyte[0] << 24);

    if(Length < NDFLimit) {
        HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(Length));
    } else {
        HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(NDFLimit));
    }
    HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, 3);

    HAL_SPI_WRITE32(REG_SPIC_SER, 1);

    HAL_SPI_WRITE32(REG_SPIC_DR0, instruction);

    // Enable SPI_FLASH  User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

    SpicWaitBusyDoneRtl8195A();

    while(Length > 0) {
        if(TransferByte == NDFLimit) {
            addr+= TransferByte;
            TransferByte = 0;
            addrbyte[2] = (addr & 0xFF0000) >>16;
            addrbyte[1] = (addr & 0xFF00)>>8;
            addrbyte[0] = addr & 0xFF;
            instruction = ReadCMD | (addrbyte[2] << 8)|(addrbyte[1] << 16)|(addrbyte[0] << 24);
            HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
            if(Length < NDFLimit) {
                HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(Length));
            } else {
                HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(NDFLimit));
            }
            HAL_SPI_WRITE32(REG_SPIC_DR0, instruction);
            HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

            SpicWaitBusyDoneRtl8195A();
        }

        *data = HAL_SPI_READ32(REG_SPIC_DR0);
        TransferByte += 4;
        Length -= 4;
        data++;
    }
    HAL_SPI_WRITE32(REG_SPIC_FLUSH_FIFO, 1);

}

HAL_FLASH_TEXT_SECTION
VOID
SpicUserProgramRtl8195A
(
    //IN  flash_t *obj,
    IN  u8 *data,
    IN  SPIC_INIT_PARA SpicInitPara,
    IN  u32 addr,
    IN  u32* LengthInfo
)
{

    u32 Info;
    u32 Length = (u32) *LengthInfo;
    u32 OccuSize;
    u32 writeword;
    u32 lastwriteword;
    u32 ProgramLength;
    u32 instruction;
    u32 PageSize;
    u8 addrbyte[3];
    u8 UnalignOffset;
    u8 lastunalignoffset;
    u8 index;
    u8 *ptr;
    u8 *buff;


    UnalignOffset = 0;
    lastunalignoffset = 0;
    writeword = 0;
    lastwriteword = 0;
    ProgramLength = 0;
    buff = data;
    PageSize = 256;

    OccuSize = addr & 0xFF;
    if(OccuSize) {
        if((Length >= PageSize) ||((OccuSize + Length) >= PageSize))
            ProgramLength= PageSize - OccuSize;
        else
            ProgramLength = Length;
    } else { //program from the beginning of the page
        if(Length >= PageSize)
            ProgramLength = PageSize;
        else
            ProgramLength = Length;
    }

    *LengthInfo -= ProgramLength;

    if(addr & 0x03) {
        UnalignOffset = (addr & 0x03);
        addr -= UnalignOffset;
        writeword = HAL_READ32(SPI_FLASH_BASE, addr);
        ptr = (u8*) &writeword + UnalignOffset;
        UnalignOffset = 4 - UnalignOffset;
        for(index = 0; index < UnalignOffset ; index++) {
            *ptr = *buff;
            buff++;
            ptr++;
            ProgramLength--;
            if(ProgramLength == 0)
                break;
        }
    } else {
        if(ProgramLength >= 4) {
            writeword = (u32)(*buff) | (u32)((*(buff+1)) << 8)|(u32)((*(buff+2)) <<16)|(u32)((*(buff+3))<<24);
        }
    }
//address already align
    if(ProgramLength & 0x3) {
        lastunalignoffset = ProgramLength & 0x3;
        if(UnalignOffset)
            lastwriteword = HAL_READ32(SPI_FLASH_BASE, (addr + 4) + ProgramLength - lastunalignoffset);
        else
            lastwriteword = HAL_READ32(SPI_FLASH_BASE, addr + ProgramLength - lastunalignoffset);
        buff += (ProgramLength - lastunalignoffset);
        ptr = (u8*) &lastwriteword;
        for(index = 0; index < lastunalignoffset; index++) {
            *ptr = *buff;
            buff++;
            ptr++;
        }
        if(UnalignOffset == 0)
            if(ProgramLength < 4) {
                writeword = lastwriteword;
                ProgramLength = 0;
            }
    }



    addrbyte[2] = (addr & 0xFF0000) >>16;
    addrbyte[1] = (addr & 0xFF00)>>8;
    addrbyte[0] = addr & 0xFF;

    instruction = FLASH_CMD_PP | (addrbyte[2] << 8)|(addrbyte[1] << 16)|(addrbyte[0] << 24);
    Info = HAL_SPI_READ32(REG_SPIC_ADDR_LENGTH);
    //Store current setting of Address length
    // Set flash_cmd: WREN to FIFO
    SpicTxCmdWithDataRtl8195A(FLASH_CMD_WREN, 0, 0, SpicInitPara);

    // Disable SPI_FLASH
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    // set ctrlr0: TX mode
    HAL_SPI_WRITE32(REG_SPIC_CTRLR0,
                    (HAL_SPI_READ32(REG_SPIC_CTRLR0) & (~ BIT_TMOD(3))));

    HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, BIT_ADDR_PHASE_LENGTH(3));

    HAL_SPI_WRITE32(REG_SPIC_DR0, instruction);

    HAL_SPI_WRITE32(REG_SPIC_DR0, writeword);

    if(UnalignOffset == 0) {
        if(ProgramLength >= 4) {
            buff = data + 4;
            ProgramLength-=4;
        }
    } else
        buff = data + UnalignOffset;
    //Pre-load data before enabling
    index = 0;
    while(ProgramLength > 4) {
        if((u32)buff & 0x03) {
            //while(ProgramLength >= 4){
            writeword = (u32)(*buff) | ((u32)(*(buff+1)) << 8) | ((u32)(*(buff+2)) << 16) | ((u32)(*(buff+3)) << 24);
            HAL_SPI_WRITE32(REG_SPIC_DR0, writeword);
            ProgramLength -=4;
            buff+=4;
            //}
        } else {
            //while(ProgramLength >= 4){
            HAL_SPI_WRITE32(REG_SPIC_DR0, (u32)*((u32 *)buff));
            ProgramLength -=4;
            buff+=4;
            //}
        }
        index++;
        if(index >= 6)
            break;
    }

    // Enable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, BIT_SPIC_EN);

    if((u32)buff & 0x03) {
        while(ProgramLength >= 4) {
            writeword = (u32)(*buff) | ((u32)(*(buff+1)) << 8) | ((u32)(*(buff+2)) << 16) | ((u32)(*(buff+3)) << 24);
            HAL_SPI_WRITE32(REG_SPIC_DR0, writeword);
            ProgramLength -=4;
            buff+=4;
        }
    } else {
        while(ProgramLength >= 4) {
            HAL_SPI_WRITE32(REG_SPIC_DR0, (u32)*((u32 *)buff));
            ProgramLength -=4;
            buff+=4;
        }
    }

    if(ProgramLength > 0) {
        HAL_SPI_WRITE32(REG_SPIC_DR0, lastwriteword);
    }

    // wait spic busy done
    SpicWaitBusyDoneRtl8195A();
    // wait flash busy done (wip=0)
    if(SpicInitPara.flashtype == FLASH_MICRON) {
        SpicWaitOperationDoneRtl8195A(SpicInitPara);
    } else {
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    }
    // Disable SPI_FLASH User Mode
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
    //REG_SPIC_ADDR_LENGTH cannot be programmed if SSIENR is active
    //Here to restore the setting of address length
    HAL_SPI_WRITE32(REG_SPIC_ADDR_LENGTH, Info);
}

/**
  * @brief  SpicWaitWipDoneRefinedRtl8195A. Wait for flash ready.
  *
  * @param  IN  SPIC_INIT_PARA SpicInitPara: spic init parameters with timing setting
  *
  * @retval NA
  */
HAL_FLASH_TEXT_SECTION
VOID
SpicWaitWipDoneRefinedRtl8195A(
    IN  SPIC_INIT_PARA SpicInitPara
)
{
    DBG_SPIF_INFO("%s(0x%x)\n", __func__, SpicInitPara);

    do {
    } while((SpicGetFlashStatusRefinedRtl8195A(SpicInitPara) & 0x01));
}

/**
  * @brief  SpicWaitWipRtl8195A.
  *
  * @param    NA
  *
  * @retval NA
  */

HAL_FLASH_TEXT_SECTION
u32
SpicWaitWipRtl8195A(
    VOID
)
{

    SPIC_INIT_PARA SpicInitPara;

    /* Check for flash ready status */
    SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);

    return _TRUE;
}

HAL_FLASH_TEXT_SECTION
VOID
SpicWaitOperationDoneRtl8195A
(
    IN  SPIC_INIT_PARA SpicInitPara
)
{
    DBG_SPIF_INFO("%s(0x%x)\n", __func__, SpicInitPara);

    do {
    } while(!(SpicGetFlashFlagRtl8195A(SpicInitPara) & 0x80));
}

HAL_FLASH_TEXT_SECTION
u8
SpicGetFlashFlagRtl8195A
(
    IN  SPIC_INIT_PARA SpicInitPara
)
{

    u32 RdData;

    DBG_SPIF_INFO("%s(0x%x)\n", __func__, SpicInitPara);

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    /* Set Ctrlr1; 1 byte data frames */
    HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(1));

    /* Send flash RX command and read the data */
    SpicRxCmdRefinedRtl8195A(FLASH_CMD_RFSR, SpicInitPara);
    RdData = HAL_SPI_READ8(REG_SPIC_DR0);

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    if(RdData & 0x2) {
        if(RdData & 0x80)
            DBG_SPIF_WARN("Attempts to Program / Erase Protected Area.\n");
        SpicTxCmdWithDataRtl8195A(FLASH_CMD_CFSR, 0, 0, SpicInitPara);//Clear Error Bit & Write Enable of Flag Status Register
    }

    return RdData;

}

HAL_FLASH_TEXT_SECTION
u8
SpicGetExtendAddrRtl8195A(
    IN  SPIC_INIT_PARA SpicInitPara
)
{

    u32 RdData;

    DBG_SPIF_INFO("%s(0x%x)\n", __func__, SpicInitPara);

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    /* Set Ctrlr1; 1 byte data frames */
    HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(1));

    /* Send flash RX command and read the data */
    SpicRxCmdRefinedRtl8195A(FLASH_CMD_REAR, SpicInitPara);
    RdData = HAL_SPI_READ8(REG_SPIC_DR0);

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    return RdData;
}

HAL_FLASH_TEXT_SECTION
u8
SpicGetConfigRegRtl8195A(
    IN  SPIC_INIT_PARA SpicInitPara
)
{

    u32 RdData;

    DBG_SPIF_INFO("%s(0x%x)\n", __func__, SpicInitPara);

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    /* Set Ctrlr1; 1 byte data frames */
    HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(1));

    /* Send flash RX command and read the data */
    SpicRxCmdRefinedRtl8195A(FLASH_CMD_RDCR, SpicInitPara);
    RdData = HAL_SPI_READ8(REG_SPIC_DR0);

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    return RdData;
}
/**
  * @brief  SpicGetFlashStatusRefinedRtl8195A. For the exchange between one- and two-
  *            bit mode, the spic timing setting (baud, rd_dummy_cycle (ATUO_LENGTH)
  *            and delay line) should be changed according to the mode used.
  *
  * @param  IN  SPIC_INIT_PARA SpicInitPara: spic init parameters with timing setting
  *
  * @retval u8 flash status register value
  */
HAL_FLASH_TEXT_SECTION
u8
SpicGetFlashStatusRefinedRtl8195A(
    IN  SPIC_INIT_PARA SpicInitPara
)
{

    u32 RdData;

    DBG_SPIF_INFO("%s(0x%x)\n", __func__, SpicInitPara);

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    /* Set Ctrlr1; 1 byte data frames */
    HAL_SPI_WRITE32(REG_SPIC_CTRLR1, BIT_NDF(1));

    /* Send flash RX command and read the data */
    SpicRxCmdRefinedRtl8195A(FLASH_CMD_RDSR, SpicInitPara);
    RdData = HAL_SPI_READ8(REG_SPIC_DR0);

    /* Disable SPI_FLASH User Mode */
    HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);

    return RdData;
}

HAL_FLASH_TEXT_SECTION
VOID
SpicSetExtendAddrRtl8195A
(
    IN  u32 data,
    IN  SPIC_INIT_PARA SpicInitPara
)
{
    SpicTxFlashInstRtl8195A(FLASH_CMD_WEAR, 1, (u8 *) (&data));
}

/**
  * @brief  SpicSetFlashStatusRefinedRtl8195A.
  *
  * @param    NA
  *
  * @retval NA
  */
HAL_FLASH_TEXT_SECTION
VOID
SpicSetFlashStatusRefinedRtl8195A
(
    IN  u32 data,
    IN  SPIC_INIT_PARA SpicInitPara
)
{
    u8 Buf[2];

    Buf[0] = data & 0xFF;
    Buf[1] = (data >> 8) & 0xFF;

    if((SpicInitParaAllClk[0][0].flashtype == FLASH_WINBOND) || (SpicInitParaAllClk[0][0].flashtype == FLASH_MXIC_4IO))
        SpicTxFlashInstRtl8195A(FLASH_CMD_WRSR, 2, Buf);
    else
        SpicTxFlashInstRtl8195A(FLASH_CMD_WRSR, 1, Buf);
}

/**
  * @brief  SpicDisableRtl8195A.
  *     Disable SPI Flash memory controller.
  * @param    NA
  *
  * @retval NA
  */
HAL_FLASH_TEXT_SECTION
VOID
SpicDisableRtl8195A(VOID)
{
    SPI_FLASH_PIN_FCTRL(OFF);
}

#if SPIC_CALIBRATION_IN_NVM
/**
  * @brief  SpicNVMCalLoad.
  *     Load the SPI Flash Controller Calibration data from NVM
  * @param    NA
  *
  * @retval NA
  */
HAL_FLASH_TEXT_SECTION VOID
SpicNVMCalLoad(u8 BitMode, u8 CpuClk)
{
    SPIC_INIT_PARA *pspci_para;
    u32 spci_para;
    u32 spci_para_inv;
    u32 flash_offset;
    u32 validCMDback;
    validCMDback = HAL_SPI_READ32(REG_SPIC_VALID_CMD);

//    DBG_SPIF_INFO("SpicNVMCalLoad==> BitMode=%d CpuClk=%d\r\n", BitMode, CpuClk);

    /* each Calibration parameters use 8 bytes, first 4-bytes are the calibration data,
       2nd 4-bytes are the validate data: ~(calibration data) */
    flash_offset = (CpuClk * 8) + (BitMode * CPU_CLK_TYPE_NO * 8);
    /*Change the setting only if cpu is 166 MHz*/
    if((HAL_SPI_READ32(REG_SPIC_BAUDR) == 1) && (HalGetCpuClk() == 0x9ef21aa)) {
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
        HAL_SPI_WRITE32(REG_SPIC_VALID_CMD,
                        (HAL_SPI_READ32(REG_SPIC_VALID_CMD)|(FLASH_VLD_DUAL_CMDS)));
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 1);
        SpicWaitBusyDoneRtl8195A();
    }
    spci_para = HAL_READ32(SPI_FLASH_BASE, (FLASH_SPIC_PARA_BASE+flash_offset));
    if (spci_para != 0xFFFFFFFF) {
        spci_para_inv = HAL_READ32(SPI_FLASH_BASE, (FLASH_SPIC_PARA_BASE+flash_offset+4));
        if (0xFFFFFFFF == (spci_para ^ spci_para_inv)) {
            pspci_para = (SPIC_INIT_PARA*)&spci_para;
            SpicInitParaAllClk[BitMode][CpuClk].BaudRate = pspci_para->BaudRate;
            SpicInitParaAllClk[BitMode][CpuClk].RdDummyCyle = pspci_para->RdDummyCyle;
            SpicInitParaAllClk[BitMode][CpuClk].DelayLine = pspci_para->DelayLine;
            SpicInitParaAllClk[BitMode][CpuClk].Valid = pspci_para->Valid;
            DBG_SPIF_INFO("SpicNVMCalLoad: Calibration Loaded(BitMode %d, CPUClk %d): BaudRate=0x%x RdDummyCyle=0x%x DelayLine=0x%x\r\n",
                          BitMode, CpuClk,
                          SpicInitParaAllClk[BitMode][CpuClk].BaudRate,
                          SpicInitParaAllClk[BitMode][CpuClk].RdDummyCyle,
                          SpicInitParaAllClk[BitMode][CpuClk].DelayLine);
        } else {
            DBG_SPIF_WARN("SpicNVMCalLoad: Data in Flash(@ 0x%x = 0x%x 0x%x) is Invalid\r\n",
                          (FLASH_SPIC_PARA_BASE+flash_offset), spci_para, spci_para_inv);
        }

    } else {
//        DBG_SPIF_INFO("SpicNVMCalLoad: No Data in Flash(@ 0x%x)\r\n", flash_offset);
    }
    if((HAL_SPI_READ32(REG_SPIC_BAUDR) == 1) && (HalGetCpuClk() == 0x9ef21aa)) {
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 0);
        HAL_SPI_WRITE32(REG_SPIC_VALID_CMD, validCMDback);
        HAL_SPI_WRITE32(REG_SPIC_SSIENR, 1);
    }
}

/**
  * @brief  SpicNVMCalLoadAll.
  *     Load the SPI Flash Controller Calibration data from NVM
  * @param    NA
  *
  * @retval NA
  */
HAL_FLASH_TEXT_SECTION VOID
SpicNVMCalLoadAll(void)
{
    u8 i,j;

    for(i=0; i<3; i++) {
        for (j=0; j<CPU_CLK_TYPE_NO; j++) {
            SpicNVMCalLoad(i,j);
        }
    }
}


/**
  * @brief  SpicNVMCalStore.
  *     Write the SPI Flash Controller Calibration data to NVM
  * @param    NA
  *
  * @retval NA
  */
HAL_FLASH_TEXT_SECTION VOID
SpicNVMCalStore(u8 BitMode, u8 CpuClk)
{
    SPIC_INIT_PARA *pspci_para;
    u32 spci_para;
    u32 flash_offset;
    SPIC_INIT_PARA SpicInitPara;

    if(SpicInitParaAllClk[BitMode][CpuClk].id[0] == 0x00)
        SpicReadIDRtl8195A();

    DBG_SPIF_INFO("SpicNVMCalStore==> BitMode=%d CpuClk=%d\r\n", BitMode, CpuClk);
    //DBG_SPIF_INFO("Flash type in SPIC NVM is = %x\n",SpicInitParaAllClk[BitMode][CpuClk].flashtype);
    /* each Calibration parameters use 8 bytes, first 4-bytes are the calibration data,
       2nd 4-bytes are the validate data: ~(calibration data) */
    flash_offset = (CpuClk * 8) + (BitMode * CPU_CLK_TYPE_NO * 8);
    spci_para = HAL_READ32(SPI_FLASH_BASE, (FLASH_SPIC_PARA_BASE+flash_offset));
    if (spci_para == 0xFFFFFFFF) {
//    if (1) {
        pspci_para = (SPIC_INIT_PARA*)&spci_para;
        pspci_para->BaudRate = SpicInitParaAllClk[BitMode][CpuClk].BaudRate;
        pspci_para->RdDummyCyle = SpicInitParaAllClk[BitMode][CpuClk].RdDummyCyle;
        pspci_para->DelayLine = SpicInitParaAllClk[BitMode][CpuClk].DelayLine;
        pspci_para->Valid = SpicInitParaAllClk[BitMode][CpuClk].Valid;
        HAL_WRITE32(SPI_FLASH_BASE, (FLASH_SPIC_PARA_BASE+flash_offset), spci_para);

        if((SpicInitParaAllClk[BitMode][CpuClk].flashtype) == FLASH_MICRON)
            SpicWaitOperationDoneRtl8195A(SpicInitPara);
        else
            SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);

        HAL_WRITE32(SPI_FLASH_BASE, (FLASH_SPIC_PARA_BASE+flash_offset+4), ~spci_para);

        if((SpicInitParaAllClk[BitMode][CpuClk].flashtype) == FLASH_MICRON)
            SpicWaitOperationDoneRtl8195A(SpicInitPara);
        else
            SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);

        DBG_SPIF_INFO("SpicNVMCalStore(BitMode %d, CPUClk %d): Calibration Stored: BaudRate=0x%x RdDummyCyle=0x%x DelayLine=0x%x\r\n",
                      BitMode, CpuClk,
                      SpicInitParaAllClk[BitMode][CpuClk].BaudRate,
                      SpicInitParaAllClk[BitMode][CpuClk].RdDummyCyle,
                      SpicInitParaAllClk[BitMode][CpuClk].DelayLine);
        // Read back to check
        if (HAL_READ32(SPI_FLASH_BASE, (FLASH_SPIC_PARA_BASE+flash_offset)) != spci_para) {
            DBG_SPIF_ERR("SpicNVMCalStore Err(Offset=0x%x), Wr=0x%x Rd=0x%x \r\n",
                         flash_offset, spci_para, HAL_READ32(SPI_FLASH_BASE, (FLASH_SPIC_PARA_BASE+flash_offset)));
        }

        if (HAL_READ32(SPI_FLASH_BASE, (FLASH_SPIC_PARA_BASE+flash_offset+4)) != ~spci_para) {
            DBG_SPIF_ERR("SpicNVMCalStore Err(Offset=0x%x), Wr=0x%x Rd=0x%x \r\n",
                         flash_offset+4, ~spci_para, HAL_READ32(SPI_FLASH_BASE, (FLASH_SPIC_PARA_BASE+flash_offset+4)));
        }
    } else {
        // There is a parameter on the flash memory already
        DBG_SPIF_ERR("SpicNVMCalStore: The flash memory(@0x%x = 0x%x) is not able to be write, Erase it first!!\r\n",
                     (FLASH_SPIC_PARA_BASE+flash_offset), spci_para);
    }
}

#endif  // #if SPIC_CALIBRATION_IN_NVM
