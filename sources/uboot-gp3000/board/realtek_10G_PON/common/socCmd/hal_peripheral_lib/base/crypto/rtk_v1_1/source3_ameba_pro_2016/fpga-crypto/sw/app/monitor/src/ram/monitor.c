/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"
#include "monitor_lib.h"
#ifdef CONFIG_SOC_PS_MODULE
#include "hal_soc_ps_monitor.h"
#endif
#include "rtl_consol.h"

#define I2SDEMO_EN          0



u32
CmdRamHelp(
    IN  u16 argc,
    IN  u8  *argv[]
);

_LONG_CALL_
extern u32
CmdDumpByte(
    IN  u16 argc,
    IN  u8  *argv[]
);

_LONG_CALL_
extern u32
CmdDumpHelfWord(
    IN  u16 argc,
    IN  u8  *argv[]
);


_LONG_CALL_
extern u32
CmdDumpWord(
    IN  u16 argc,
    IN  u8  *argv[]
);

_LONG_CALL_
extern u32
CmdWriteByte(
    IN  u16 argc,
    IN  u8  *argv[]
);

_LONG_CALL_
extern u32
CmdWriteWord(
    IN  u16 argc,
    IN  u8  *argv[]
);


#ifdef CONFIG_GDMA_VERIFY
u32
CmdGdmaTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_SDIO_HOST_VERIFY
u32
CmdSdHostTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_SDIO_DEVICE_EN
#ifdef CONFIG_SDIO_DEVICE_VERIFY
u32
CmdSdioDeviceTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#elif SDIO_MP_MODE
u32
CmdSdioDeviceMPTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif
#endif  // #ifdef CONFIG_SDIO_DEVICE_EN

#ifdef CONFIG_WIFI_VERIFY
u32
CmdWlanTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_WIFI_NORMAL
u32 CmdWlanNormal(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_I2S_VERIFY
u32
CmdI2sTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_I2S_NORMAL
u32
CmdI2sTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_PCM_VERIFY
u32
CmdPcmTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_I2C_VERIFY
u32
CmdI2CTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_SDR_VERIFY
u32
CmdSdrTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_SPIC_VERIFY
u32
CmdSpiFlashTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_DEBUG_LOG
u32
CmdDbgMsgCfg(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_SPI_COM_VERIFY
u32
CmdSsiTest(
    IN u16 argc,
    IN u8  *argv[]

);

u32
CmdSsiMain(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_UART_VERIFY
u32
CmdRuartTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_MII_VERIFY
u32
CmdMiiTest(
    IN u16 artc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_NFC_VERIFY
u32
CmdNFCTest(
    IN u16 artc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_SOC_PS_VERIFY
u32
CmdSoCPSTest(
    IN u16 artc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_PWM_VERIFY
u32
CmdPWMTest(
    IN u16 artc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_EFUSE_VERIFY
u32
CmdEFUSETest(
    IN u16 artc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_CRYPTO_VERIFY
u32 CmdCryptoTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_GPIO_VERIFY
u32
CmdGPIOTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_RTLIB_VERIFY
u32 CmdRTLIBTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif
#ifdef CONFIG_DAC_VERIFY
u32
CmdDACTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_ADC_VERIFY
u32
CmdADCTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_USB_VERIFY
#ifdef DWC_DEVICE_ONLY
u32
CmdOTGTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif
#endif

#if CONFIG_BTBX_MODULE
#if CONFIG_BTBX_TEST
u32
CmdBTBXTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif  //CONFIG_BTBX_TEST
#endif  //CONFIG_BTBX_MODULE


#if CONFIG_PERI_UPDATE_IMG
_LONG_CALL_
extern VOID SpicWaitBusyDoneRtl8195A(VOID);

extern void OTU_FW_Update(u8 uart_idx, u8 pin_mux, u32 baud_rate);
extern void OTU_Img_Download(u8 uart_idx, u8 pin_mux, u32 baud_rate,
                             u32 start_offset, u32 start_addr, u32 max_size);

u32
CmdOTUFwUpdate(
    IN  u16 argc,
    IN  u8  *argv[]
);

u32
CmdOTUImgDwnld(
    IN  u16 argc,
    IN  u8  *argv[]
);

u32
FlashDump(
    IN  u16 argc,
    IN  u8  *argv[]
);

u32
FlashErase(
    IN  u16 argc,
    IN  u8  *argv[]
);

u32
FlashWriteWord(
    IN  u16 argc,
    IN  u8  *argv[]
);

#endif

//MON_RAM_DATA_SECTION
COMMAND_TABLE   UartLogRamCmdTable[] = {
    {(const u8*)"?",        0, CmdRamHelp,        (const u8*)"\tHELP (?)   : Print this RAM help messag\n"},
    {
        (const u8*)"DB",    2, CmdDumpByte,      (const u8*)"\tDB <Address, Hex> <Len, Dec>: \n"
        "\t\t\t\t Dump memory byte or Read Hw byte register"
    },
    {
        (const u8*)"DHW",   2, CmdDumpHelfWord,  (const u8*)"\tDHW <Address, Hex> <Len, Dec>: \n"
        "\t\t\t\t Dump memory helf-word or Read Hw helf-word register;\n"
        "\t\t\t\t Unit: 4Bytes"
    },
    {
        (const u8*)"DW",    2, CmdDumpWord,      (const u8*)"\tDW <Address, Hex> <Len, Dec>: \n"
        "\t\t\t\t Dump memory word or Read Hw word register; \n"
        "\t\t\t\t Unit: 4Bytes"
    },
    {
        (const u8*)"EB",    2, CmdWriteByte,     (const u8*)"\tEB <Address, Hex> <Value, Hex>: \n"
        "\t\t\t\t Write memory byte or Write Hw byte register \n"
        "\t\t\t\t Can write more word at the same time \n"
        "\t\t\t\t Ex: EB Address Value0 Value1"
    },
    {
        (const u8*)"EW",    2, CmdWriteWord,     (const u8*)"\tEW <Address, Hex> <Value, Hex>: \n"
        "\t\t\t\t Write memory word or Write Hw word register \n"
        "\t\t\t\t Can write more word at the same time \n"
        "\t\t\t\t Ex: EW Address Value0 Value1"
    },
#ifdef CONFIG_GDMA_VERIFY
    {
        (const u8*)"GDMA",  7, CmdGdmaTest,      (const u8*)"\tGdma \n"
        "\t\t <Loop, Dec> <Length, Dec> <Gdam Type, Dec> \n"
        "\t\t <Memory Type, Dec> <Gdma Number, Dec> <Pattern, Dec> <Auto, Dec> :\n"
        "\t\t\t\t Loop: The number of the test\n"
        "\t\t\t\t Length: The transfer length of the single test\n"
        "\t\t\t\t Gdma Type: Single/Multi-Bock/Autoreload/LLP\n"
        "\t\t\t\t Memory Type: Src:SD SRAM/SF SRAM; Dst:SD SRAM/SF SRAM;\n"
        "\t\t\t\t Gdma Number: Gdma0/Gdma1\n"
        "\t\t\t\t Pattern: 0x5A/Incremental\n"
        "\t\t\t\t Auto: Auto-generate the below parameter"
    },
#endif

#ifdef CONFIG_SDIO_HOST_VERIFY
    {
        (const u8*)"SD_HOST",  7, CmdSdHostTest,      (const u8*)"\tSdHost \n"
        "\t\t <Test Item, Dec> \n"
    },
#endif

#ifdef CONFIG_SDIO_DEVICE_EN
#ifdef CONFIG_SDIO_DEVICE_VERIFY
    {
        (const u8*)"SDIO_DEVICE",  7, CmdSdioDeviceTest,      (const u8*)"\SDIO_DEVICE \n"
        "\t\t <Test Item, Dec> \n"
    },
#elif SDIO_MP_MODE
    {
        (const u8*)"SDIO",	2, CmdSdioDeviceMPTest,   (const u8*)"\tSDIO\n"
        "\t\t <MP Command> <Para 1> <Para 2> ... \n"
    },
#endif
#endif	// end of "#ifdef CONFIG_SDIO_DEVICE_EN"

#ifdef CONFIG_WIFI_VERIFY
    {
        (const u8*)"WLAN", 1, CmdWlanTest,     (const u8*)"\tWlan \n"
        "\t\t <Test Item, Dec> <Loop, Dec> <Pattern, Dec> \n"
        "\t\t <Length, Dec> <AC Queue, Dec> <Rate ID, Dec>:\n"
        "\t\t\t\t Test Item: 1: Initial; 2: TRX Loopback\n"
        "\t\t\t\t Loop: The number of the test\n"
        "\t\t\t\t Pattern: 0xFF/0x5A/Ramdon\n"
        "\t\t\t\t Length: 0: Ramdon; \n"
        "\t\t\t\t AC Queue: 0-7 \n"
        "\t\t\t\t Rate ID\n"
    },
#endif

#if defined(CONFIG_WIFI_NORMAL) && defined(CONFIG_NETWORK)
    {
        (const u8*)"WLAN", 1, CmdWlanNormal,     (const u8*)"\tWlan \n"
        "\t\t WLAN Driver Test\n"
    },
#endif

#ifdef CONFIG_SDR_VERIFY
    {
        (const u8*)"SDR",    2, CmdSdrTest,     (const u8*)"\tSDR \n"
        "\t\t <Mode, Dec> <Loop, Dec>: \n"
        "\t\t\t\t Mode: Initial = 1; Write and Read compare = 2 \n"
        "\t\t\t\t Loop: The number of Write and Read compare \n"
    },
#endif
#ifdef CONFIG_SPIC_VERIFY
    {
        (const u8*)"SPIC",   2, CmdSpiFlashTest,(const u8*)"\tSPIC \n"
        "\t\t <Mode, Dec> <Loop, Dec>: \n"
        "\t\t\t\t Mode: Initial = 1; Write and Read compare = 2 \n"
        "\t\t\t\t Loop: The number of Write and Read compare \n"
    },
#endif
#ifdef CONFIG_I2S_VERIFY
    {
        (const u8*)"I2S",  7, CmdI2sTest,      (const u8*)"\tI2S \n"
        "\t\t <mode, Dec>"
    },
#endif
#if I2SDEMO_EN
    {
        (const u8*)"I2SDEMO",  7, CmdI2sTest,      (const u8*)"\tI2SDEMO \n"
        "\t\t <mode, Dec>"
    },
#endif
#ifdef CONFIG_PCM_VERIFY
    {(const u8*)"PCM",  7, CmdPcmTest,      (const u8*)"\tPCM \n"},
#endif
#ifdef CONFIG_DEBUG_LOG
    {
        (const u8*)"DEBUG",	2, CmdDbgMsgCfg,		(const u8*)"\tDEBUG \n"
        "\tConfig Debugging Message \n"
        "\t\t <DbgLevel: error,warn,trace,info> <ConfigValue, Hex>"
    },
#endif
#ifdef CONFIG_SPI_COM_VERIFY
    {
        (const u8*)"SSI", 10, CmdSsiTest,       (const u8*)"\tSSI \n"
        "\t\t DesignWare SSI Verification Program \n"
    },
    {
        (const u8*)"SSIMAIN", 10, CmdSsiMain,       (const u8*)"\tSSIMAIN \n"
        "\t\t New SSI API \n"
    },
#endif
#ifdef CONFIG_UART_VERIFY
    {
        (const u8*)"UART", 7, CmdRuartTest,    (const u8*)"\tUART \n"
        "\t\t UART IP verify \n"
    },
#endif
#ifdef CONFIG_MII_VERIFY
    {
        (const u8*)"MII", 10, CmdMiiTest,       (const u8*)"\tMII \n"
        "\t\t MII Verification Program \n"
    },
#endif
#ifdef CONFIG_I2C_VERIFY
    {
        (const u8*)"I2C",  7, CmdI2CTest,      (const u8*)"\tI2C \n"
        "\t\t <Test Item, Dec> <Loop, Dec> <I2C RW Cmd, Dec> \n"
        "\t\t\t\t Test Item: "
        "\t\t\t\t 1: I2C single RW without interrupt and DMA \n"
        "\t\t\t\t 2: I2C single RW with interrupt but without DMA \n"
        "\t\t\t\t 2: I2C RW with interrupt and DMA \n"
        "\t\t\t\t \n"
    },
#endif
#ifdef CONFIG_NFC_VERIFY
    {
        (const u8*)"NFC",  4, CmdNFCTest,      (const u8*)"\tNFC \n"
        "\t\t <Test Item, Dec> <Parameter1, Dec> <Parameter2, Dec> <Parameter3, Dec>\n"
        "\t\t Test Item: \n"
        "\t\t  0: NFC Init : <NFC 0 Addr>\n"
        "\t\t  1: NFC AON Read32 : <NFC 1 Addr>\n"
        "\t\t  2: NFC AON Write32 : <NFC 2 Addr Value>\n"
        "\t\t  3: NFC General Read32 : <NFC 3 Addr>\n"
        "\t\t  4: NFC General Write32: <NFC 4 Addr Value>\n"
        "\t\t  5: NFC FW Download and Reboot : <NFC 5>\n"
        "\t\t  6: NFC IMEM Download : <NFC 6>\n"
        "\t\t  7: NFC DMEM Download : <NFC 7>\n"
        "\t\t  8: NFC A2N2A message box Rand LP : <NFC 8 Initialvalue LB_num>\n"
        "\t\t  9: NFC A2N message box Multi LP : <NFC 9 Initialvalue LB_num>\n"
        "\t\t 10: NFC N2A message box Multi LP : <NFC 10 Initialvalue LB_num>\n"
        "\t\t 11: NFC A2N message box signal LP : <NFC 11 Initialvalue>\n"
        "\t\t 12: NFC N2A message box signal LP : <NFC 12 Initialvalue>\n"
        "\t\t 13: NFC A2N2A message Rand LP : <NFC 13 Initialvalue LB_num>\n"
        "\t\t \n"
    },
#endif
#ifdef CONFIG_SOC_PS_VERIFY
    {
        (const u8*)"PS",  7, CmdSoCPSTest,      (const u8*)"\tPS <Test Item, Dec> <Loop, Dec> :\n"
        "\t\t Test Item : \n"
        "\t\t 1: SoC PS single RW without interrupt and DMA \n"
        "\t\t 2: SoC PS single RW with interrupt but without DMA \n"
        "\t\t 2: SoC PS RW with interrupt and DMA \n"
        "\t\t \n"
    },

#endif
#ifdef CONFIG_PWM_VERIFY
    {
        (const u8*)"PWM",  4, CmdPWMTest,      (const u8*)"\tPWM \n"
        "\t\t <Test Item, Dec> <Parameter1, Dec> <Parameter2, Dec> <Parameter3, Dec>\n"
        "\t\t Test Item: \n"
        "\t\t  0: Timer Init : <PWM 0 [1]: timerID, [2]: timerMode, [3]: IrqDIS, [4]:period>\n"
        "\t\t  1: PWM PinCtrl : <PWM 1 [1]: function, [2]: location, [3]:operation>\n"
        "\t\t  2: PWM Setting : <PWM 2 [1]: num, [2]:Gtimer, [3]:on_duty, [4]: period>\n"
        "\t\t  3: ETE PinCtrl : <ETE 3 [1]: function, [2]: location, [3]:operation>\n"
        "\t\t  4: ETE Setting : <ETE 4 [1]: num, [2]:Gtimer, [3]:pulse_dur>\n"
        "\t\t  5: EGTIM Setting : <EGTIM 5 [1]: BYTE0, [2]:BYTE1>\n"
        "\t\t \n"
    },
#endif
#ifdef CONFIG_EFUSE_VERIFY
    {
        (const u8*)"EFUSE",  4, CmdEFUSETest,      (const u8*)"\tEFUSE \n"
        "\t <Test Item, Dec> <Parameter1, Dec> <Parameter2, Dec> <Parameter3, Dec>\n"
        "\t\t Test Item: \n"
        "\t\t  0: Timer Init : <EFUSE 0 [1]: timerID, [2]: timerMode, [3]: IrqDIS, [4]:period>\n"
        "\t\t  1: EFUSE WRITE : <EFUSE 1 [1]: function, [2]: location, [3]:operation>\n"
        "\t\t  2: EFUSE READ : <EFUSE 2 [1]: num, [2]:Gtimer, [3]:on_duty, [4]: period>\n"
        "\t\t \n"
    },
#endif
#ifdef CONFIG_CRYPTO_VERIFY
    {
        (const u8*)"CRYPTO", 20, CmdCryptoTest,    (const u8*)"\tCRYPTO \n"
        "\t\t CRYPTO IP verify \n"
        "\t\t CRYPTO ? for more usage \n"
    },
#endif

#ifdef CONFIG_GPIO_VERIFY
    {
        (const u8*)"GPIO", 7, CmdGPIOTest,    (const u8*)"\tGPIO \n"
        "\t\t GPIO IP verify \n"
    },
#endif

#ifdef CONFIG_RTLIB_VERIFY
    {
        (const u8*)"RTLIB", 7, CmdRTLIBTest,    (const u8*)"\tRTLIB \n"
        "\t\t RTLIB verify \n"
    },
#endif
#ifdef CONFIG_DAC_VERIFY
    {
        (const u8*)"DAC",  7, CmdDACTest,      (const u8*)"\tDAC \n"
        "\t\t\t\t \n"
    },
#endif

#ifdef CONFIG_ADC_VERIFY
    {
        (const u8*)"ADC",  7, CmdADCTest,      (const u8*)"\tADC \n"
        "\t\t\t\t \n"
    },
#endif
#ifdef CONFIG_USB_VERIFY
#ifdef DWC_DEVICE_ONLY
    {
        (const u8*)"OTG",      4, CmdOTGTest,    (const u8*)"\tOTG <Test Item, Dec> <RegAddr/dummy parameter, Hex> \n"
        "\t <RegValue/dummy parameter, Hex> \n"
        "\t\t\t\t Test Item: \n"
        "\t\t\t\t 1: To read OTG register \n"
        "\t\t\t\t 2: To write OTG register \n"
        "\t\t\t\t 3: To initialize USB OTG device mode \n"
        "\t\t\t\t \n"
    },
#endif
#endif

#if CONFIG_PERI_UPDATE_IMG
    {
        (const u8*)"FUOTU",      3, CmdOTUFwUpdate,    (const u8*)"\tFUOTU <UART Index, 0/1/2> <UART PinMux, 0/1/2> <Baud Rate, Dec> \n"
        "\t\t\t\t FW Update Over the UART \n"
        "\t\t\t\t \n"
    },
    {
        (const u8*)"IDOTU",      6, CmdOTUImgDwnld,    (const u8*)"\tIDOTU <UART Index, 0/1/2> <UART PinMux, 0/1/2> <Baud Rate, Dec> \n"
        "\t\t\t <StartOffset, Dec> <StartAddr, Hex> <MaxSize, Dec>\n"
        "\t\t\t\t Image Download Over the UART \n"
        "\t\t\t\t \n"
    },
    {
        (const u8*)"FD",      2, FlashDump,    (const u8*)"\tFD <Flash Address, Hex> <Length, Dec> \n"
        "\t\t\t\t Dump Flash Memory \n"
        "\t\t\t\t \n"
    },
    {
        (const u8*)"FE",      2, FlashErase,    (const u8*)"\tFE <Flash Address, Hex> <Size(KB), Dec> \n"
        "\t\t\t\t Erase Flash Sectors \n"
        "\t\t\t\t \n"
    },
    {
        (const u8*)"FW",    2, FlashWriteWord,     (const u8*)"\tFW <Address, Hex> <Value, Hex>: \n"
        "\t\t\t\t Write SPI Flash Memory Word\n"
        "\t\t\t\t Can write more word at the same time \n"
        "\t\t\t\t Ex: FW <Addr> <Value0> <Value1>"
    },
#endif  // #if CONFIG_PERI_UPDATE_IMG

#if CONFIG_BTBX_TEST
    {
        (const u8*)"MAIL",  4, CmdBTBXTest,      (const u8*)"\tMAILBOX \n"
        "\t <Test Item, Dec> <Parameter1, Dec> <Parameter2, Dec> <Parameter3, Dec>\n"
        "\t\t Test Item: \n"
        "\t\t  0: BOX Init : <MAIL 0>\n"
        "\t\t  1: BOX WRITE : <MAIL 1 [1]: DATA0, [2]: DATA1>\n"
        "\t\t  2: BOX XX : <MAIL 2 [1]: num, [2]:Gtimer, [3]:on_duty, [4]: period>\n"
        "\t\t \n"
    },
#endif  //#if CONFIG_BTBX_TEST
};


#ifdef CONFIG_SPIC_MODULE
u32
CmdSpiFlashTool(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif


MON_RAM_TEXT_SECTION
u32
GetRamCmdNum(
    VOID
)
{
    return (sizeof(UartLogRamCmdTable)/sizeof(COMMAND_TABLE));
}

//======================================================
//<Function>:  CmdTest
//<Usage   >:  This function is a demo test function.
//<Argus    >:  argc --> number of argus
//                   argv --> pointer to a cmd parameter array
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================




MON_RAM_TEXT_SECTION
u32
CmdRamHelp(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32	LoopINdex ;

    DBG_8195A("----------------- COMMAND MODE HELP ------------------\n");
    for( LoopINdex=0  ; LoopINdex < (sizeof(UartLogRamCmdTable) / sizeof(COMMAND_TABLE)) ; LoopINdex++ ) {
        if( UartLogRamCmdTable[LoopINdex].msg ) {
            DBG_8195A( "%s\n",UartLogRamCmdTable[LoopINdex].msg );
        }
    }
    DBG_8195A("----------------- COMMAND MODE END  ------------------\n");

    return _TRUE ;
}



#ifdef CONFIG_GDMA_VERIFY
extern VOID GdmaTestApp(VOID *Data);

u32
CmdGdmaTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u8  InPutData[16];
    u32 *pTestLen, *pTestLoop;

    if(argc<2) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    pTestLen = (u32*)&InPutData[8];
    pTestLoop = (u32*)&InPutData[12];

    //Verify Loop
    *pTestLoop = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);

    //Test Length
    *pTestLen = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

    //Gdma Type; Single/Block/LLP...
    InPutData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
    if (argc == 3) {
        //Memory Test Type; SD to SD/SD to SF/SF to SF/ SF to SD
        InPutData[1] = 0;
        //Gdma Index; Gdma0/Gdma1
        InPutData[3] = 0;
        //Pattern
        InPutData[4] = 0;
        //AutoTest Enable
        InPutData[0] = 0;
    } else {
        //Memory Test Type; SD to SD (0)/SD to SF (1)/SF to SF (2)/ SF to SD (3) /Flash to SD (4)
        InPutData[1] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);
        //Gdma Index; Gdma0/Gdma1
        InPutData[3] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);
        //Pattern
        InPutData[4] = Strtoul((const u8*)(argv[5]), (u8 **)NULL, 10);
        //AutoTest Enable
        InPutData[0] = Strtoul((const u8*)(argv[6]), (u8 **)NULL, 10);

    }

//    for (i=0;i<16;i++) {
//        DBG_8195A("0x%02x\n",InPutData[i]);
//    }

    GdmaTestApp(InPutData);

    return _TRUE;
}
#endif

#ifdef CONFIG_SDIO_HOST_VERIFY
extern VOID
SdHostTestApp(
    IN  u8      *argv[]
);

u32
CmdSdHostTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 InPutData;

    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    SdHostTestApp(argv);

    return _TRUE;
}
#endif

#ifdef CONFIG_SDIO_DEVICE_EN
#ifdef CONFIG_SDIO_DEVICE_VERIFY
extern VOID
SdioDeviceTestApp(
    IN  u32      Data
);

u32
CmdSdioDeviceTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 InPutData;

    if(argc<1) {
        DBG_SDIO_ERR("Wrong argument number!\r\n");
        return _FALSE;
    }

    InPutData = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);


    SdioDeviceTestApp(InPutData);

    return _TRUE;
}

#elif SDIO_MP_MODE

u32
CmdSdioDeviceMPTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    HAL_SDIO_OP *pHalSdioOp;

    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }
#if SDIO_MP_MODE
    pHalSdioOp = (HAL_SDIO_OP *)(pgSDIODev->pHalOp);
    pHalSdioOp->HalSdioDevMPApp(pgSDIODev, argc, (&argv[0]));
#else
    DBG_8195A("Please enable SDIO_MP_MODE!\r\n");
#endif
    return _TRUE;
}

#endif	// end of else of "#ifdef CONFIG_SDIO_DEVICE_VERIFY"
#endif	// end of "#ifdef CONFIG_SDIO_DEVICE_EN"

#ifdef CONFIG_WIFI_VERIFY
extern VOID WlanTestApp(VOID *Data);

u32
CmdWlanTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 InputData[8];


    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    //Verify Loop
    InputData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    InputData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
    InputData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
    InputData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);
    InputData[4] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);
    InputData[5] = Strtoul((const u8*)(argv[5]), (u8 **)NULL, 10);

    DBG_8195A("Wlan Test\n");

    WlanTestApp((VOID*)InputData);

    return _TRUE;
}
#endif

#ifdef CONFIG_WIFI_NORMAL
#include "FreeRTOS.h"
#include "semphr.h"
VOID WlanNormal( IN  u16 argc, IN  u8  *argv[]);
u32
CmdWlanNormal(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }


    DBG_8195A("Wlan Normal Mode\n");

    WlanNormal( argc, argv);

    return _TRUE;
}
#endif

#ifdef CONFIG_I2S_VERIFY
extern VOID I2sTestApp(VOID *Data);

#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
#define I2S_FREE_RTOS_TASK_LOOPBACK_TEST 0
#endif


#define I2S_TEST_PAGE_NUM
#define I2S_TEST_SAMPLERATE
#define I2S_TEST_PAGE_TIME
u32 I2sPageTime=100; //unit 0.1ms
s32 i2s_rate_tab1[]= {8000, 16000, 24000, 32000, 0, 48000, 96000, 44100};
u32
CmdI2sTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
#if I2S_FREE_RTOS_TASK_LOOPBACK_TEST
    u32  InPutData[16], i;
    //example i2s 0: start, i2s 2: stop, i2s 3: get count

    InPutData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    InPutData[1] = 0;
    InPutData[2] = 0;
    InPutData[3] = 0;
#else
    //example i2s 4 0 100, page_num=4 sample_rate=8khz, page_time=10ms
    u8  InPutData[16], i;
    u32 mode, mode2, mode3;
    u32 I2sWordLength=0; //0:16bit, 1:24bit
    u32 I2sLRSwap=0;
    u32 I2sIndex=0;
    u32 I2sTestMode=0;
    u32 I2sCHNum=1;//0:mono, 1:stereo channel
    u32 I2sPageSize=40-1;
    u32 I2sPageNum=4-1;
    u32 I2sSampleRate=0;

    I2sPageSize=i2s_rate_tab1[I2sSampleRate]/1000*I2sPageTime*(I2sCHNum+1)*(1+I2sWordLength)/20-1;

//InPutData[0]bit2 : I2sWordLength (1:24bit, 0:16bit)
//InPutData[0]bit5 : I2sLRSwap (1:lrswap, 0:normal)
//InPutData[0]bit6 : I2sIndex
//InPutData[1]bit2-[0]bit7 : I2sTestMode
//InPutData[1]bit4-3 : I2sCHNum
//InPutData[3]bit2-[1]bit7 : I2sPageSize
//InputData[3]bit4-3 : I2sPageNum
//InputData[3]bit7-5 : I2sSampleRate
    InPutData[0]=((0x1&I2sTestMode)<<7)|(I2sIndex<<6)|(I2sLRSwap<<5)|(I2sWordLength<<2);
    InPutData[1]=((0x1&I2sPageSize)<<7)|(I2sCHNum<<3)|(I2sTestMode>>1);
    InPutData[2]=0xff&(I2sPageSize>>1);
    InPutData[3]=(I2sSampleRate<<5)|(I2sPageNum<<3)|(I2sPageSize>>9);

#ifdef I2S_TEST_PAGE_NUM //change page num from 2to4
    mode= Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    I2sPageNum= mode-1;
    InPutData[3]=(I2sSampleRate<<5)|(I2sPageNum<<3)|(I2sPageSize>>9);
#endif

#ifdef I2S_TEST_SAMPLERATE //change sample rate from 0, 1, 2, 3, 5, 6
    mode2= Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
    I2sSampleRate= mode2;
    I2sPageSize=i2s_rate_tab1[I2sSampleRate]/1000*I2sPageTime*(I2sCHNum+1)*(1+I2sWordLength)/20-1;
    InPutData[1]=((0x1&I2sPageSize)<<7)|(I2sCHNum<<3)|(I2sTestMode>>1);
    InPutData[2]=0xff&(I2sPageSize>>1);
    InPutData[3]=(I2sSampleRate<<5)|(I2sPageNum<<3)|(I2sPageSize>>9);
#endif

#ifdef I2S_TEST_PAGE_TIME //change page time from 10, 25, 50, 100, 200, 400
    mode3= Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
    I2sPageTime= mode3;
    I2sPageSize=i2s_rate_tab1[I2sSampleRate]/1000*I2sPageTime*(I2sCHNum+1)*(1+I2sWordLength)/20-1;
    InPutData[1]=((0x1&I2sPageSize)<<7)|(I2sCHNum<<3)|(I2sTestMode>>1);
    InPutData[2]=0xff&(I2sPageSize>>1);
    InPutData[3]=(I2sSampleRate<<5)|(I2sPageNum<<3)|(I2sPageSize>>9);
#endif
#endif
    for (i=0; i<5; i++) {
        DBG_8195A("%d-0x%02x\n", i, InPutData[i]);
    }

    I2sTestApp(InPutData);

    return _TRUE;
}
#endif

#if I2SDEMO_EN
extern VOID I2SDemoStart();
extern VOID I2SDemoStop();

u32
CmdI2sTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 i;
    i = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);

    if (i == 0) {
        I2SDemoStop();
    } else if (i == 1) {
        I2SDemoStart();
    }
}
#endif

#ifdef CONFIG_PCM_VERIFY
extern VOID PcmTestApp(VOID *Data);

#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
#define PCM_FREE_RTOS_TASK_LOOPBACK_TEST 0
#endif

u32
CmdPcmTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
#if PCM_FREE_RTOS_TASK_LOOPBACK_TEST
    u32  InPutData[16], i;

    InPutData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    InPutData[1] = 0;
    InPutData[2] = 0;
    InPutData[3] = 0;
#else
    u8  InPutData[16], i;
    u32 mode1, mode2, mode3, mode4;
    u32 PcmTestMode=0;
    u32 PcmIndex=0;
    u32 PcmLinearUA=0;
    u32 PcmPageSize=39;
    u32 PcmWideband=0;
    u32 PcmTimeSlot=0;

//InPutData[0]bit2 : PcmWideband
//InPutData[0]bit4-3 : PcmLinearUA
//InPutData[3]bit2-[1]bit7 : PcmPageSize
//InPutData[3]bit7-3 : PcmTimeSlot

    mode1 = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    PcmWideband = mode1;

    mode2 = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
    PcmLinearUA = mode2;

    mode3 = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
    PcmPageSize = mode3;

    mode4 = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);
    PcmTimeSlot = mode4;

    InPutData[0]=((0x1&PcmTestMode)<<7)|(PcmIndex<<6)|(PcmLinearUA<<3)|(PcmWideband<<2);
    InPutData[1]=((0x1&PcmPageSize)<<7)|(PcmTestMode>>1);
    InPutData[2]=0xff&(PcmPageSize>>1);
    InPutData[3]=(PcmTimeSlot<<3)|(PcmPageSize>>9);
#endif
    PcmTestApp(InPutData);

    return _TRUE;
}
#endif

#ifdef CONFIG_DEBUG_LOG

//MON_ROM_DATA_SECTION
const DBG_CFG_CMD Dbg_CfgCmdTable[] = {
    {"error", DBG_CFG_ERR},
    {"warn", DBG_CFG_WARN},
    {"info", DBG_CFG_INFO}
};


extern _LONG_CALL_ u8
prvStrCmp(
    IN  const   u8  *string1,
    IN  const   u8  *string2);


//MON_ROM_TEXT_SECTION
u32
CmdDbgMsgCfg(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u16 i;
    u16 entry_num;
    u8 cfg_type=0xff;
    u32 cfg_value=0;


    entry_num = sizeof(Dbg_CfgCmdTable)/sizeof(DBG_CFG_CMD);

    for (i=0; i<entry_num; i++) {
        if ((prvStrCmp(argv[0], Dbg_CfgCmdTable[i].cmd_name))==0) {
            cfg_type = Dbg_CfgCmdTable[i].cmd_type;
            break;
        }
    }

    cfg_value = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);

    switch (cfg_type) {
        case DBG_CFG_ERR:
            DiagPrintf("Debug Error Msg Cfg: 0x%08x -> 0x%08x\n", ConfigDebugErr, cfg_value);
            ConfigDebugErr = cfg_value;
            break;

        case DBG_CFG_WARN:
            DiagPrintf("Debug Warn Msg Cfg: 0x%08x -> 0x%08x\n", ConfigDebugWarn, cfg_value);
            ConfigDebugWarn = cfg_value;
            break;

        case DBG_CFG_INFO:
            DiagPrintf("Debug Info Msg Cfg: 0x%08x -> 0x%08x\n", ConfigDebugInfo, cfg_value);
            ConfigDebugInfo = cfg_value;
            break;

        default:
            DiagPrintf("Command Format: DEBUG <error/warn/info> <Value>\n");
            DiagPrintf("Debug Error Msg Cfg: 0x%08x\n", ConfigDebugErr);
            DiagPrintf("Debug Warn Msg Cfg: 0x%08x\n", ConfigDebugWarn);
            DiagPrintf("Debug Info Msg Cfg: 0x%08x\n", ConfigDebugInfo);
            break;
    }

    return _TRUE;
}
#endif

#ifdef CONFIG_SDR_VERIFY
extern VOID SdrTestApp(VOID* Data);

u32
CmdSdrTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 Cmd[4];

    if(argc<2) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }


    Cmd[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    Cmd[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    Cmd[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
    Cmd[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 16);

    SdrTestApp((VOID*)Cmd);

    return _TRUE;
}
#endif

#ifdef CONFIG_SPIC_VERIFY
extern VOID SpiFlashTestApp(VOID* Data);

u32
CmdSpiFlashTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 Cmd[2];

    if(argc<2) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }


    Cmd[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    Cmd[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

    SpiFlashTestApp((VOID*)Cmd);

    return _TRUE;
}
#endif

#ifdef CONFIG_UART_VERIFY
extern VOID RuartTestApp(VOID *Data);

u32
CmdRuartTest(
    IN u16 argc,
    IN u8  *argv[]
)
{
    u8  index;
    u8  TEST_LENGTH = 6;
    u32 TestData[TEST_LENGTH];

    for (index = 0; index < TEST_LENGTH; ++index) {
        TestData[index] = Strtoul((const u8*)(argv[index]), (u8 **)NULL, 10);
    }

#if 0
    for (index = 0; index < TEST_LENGTH; ++index) {
        DBG_8195A("RUART TestData[%d] = %d\n", index, TestData[index]);
    }
#endif

    RuartTestApp(TestData);
    return _TRUE;
}
#endif

#ifdef CONFIG_SPI_COM_VERIFY
extern VOID SsiTestApp(VOID *Data);

u32 CmdSsiTest(
    IN u16 argc,
    IN u8  *argv[]
)
{
    u8 index;
    u8 TEST_LENGTH = 9;
    u8 TestData[TEST_LENGTH];

    for(index = 0; index < TEST_LENGTH; ++index) {
        TestData[index] = Strtoul((const u8*)(argv[index]), (u8 **)NULL, 10);
    }

    SsiTestApp(TestData);
    //SsiMain(TestData);

    return _TRUE;
}

extern int SsiMain(VOID *Data);

u32 CmdSsiMain(
    IN u16 argc,
    IN u8  *argv[]
)
{
    u8 index;
    u8 TEST_LENGTH = 9;
    u8 TestData[TEST_LENGTH];

    for(index = 0; index < TEST_LENGTH; ++index) {
        TestData[index] = Strtoul((const u8*)(argv[index]), (u8 **)NULL, 10);
    }

    SsiMain(TestData);
    return _TRUE;
}
#endif

#ifdef CONFIG_MII_VERIFY
extern VOID MiiTestApp(IN u8  *argv[]);

u32 CmdMiiTest(
    IN u16 argc,
    IN u8  *argv[]
)
{
    if(argc < 1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    MiiTestApp(argv);
    return _TRUE;
}
#endif

#ifdef CONFIG_I2C_EN
#ifdef CONFIG_I2C_VERIFY
extern  VOID    I2CTestApp(VOID *Data);

u32
CmdI2CTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32  I2CTestData[6];

    if (argc<2) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    I2CTestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);//VeriProcessCmd
    I2CTestData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);//VeriItem
    I2CTestData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);//VeriLoop
    I2CTestData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);//VeriMaster
    I2CTestData[4] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);//VeriSlave
    I2CTestData[5] = Strtoul((const u8*)(argv[5]), (u8 **)NULL, 10);//MtrRW

    I2CTestApp(I2CTestData);

    return _TRUE;
}
#endif
#endif

#ifdef CONFIG_NFC_EN
#ifdef CONFIG_NFC_VERIFY
extern VOID NFCTestApp(IN  VOID      *Data);

u32
CmdNFCTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 InPutData[5];

    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    InPutData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    InPutData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    InPutData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
    InPutData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 16);
    InPutData[4] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);

    NFCTestApp((VOID*)InPutData);

    return _TRUE;
}
#endif
#endif
#ifdef CONFIG_SOC_PS_MODULE
#ifdef CONFIG_SOC_PS_VERIFY
extern VOID SOCPSTestApp(VOID *Data);

u32
CmdSoCPSTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 InputData[8];


    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    //Verify Loop
    InputData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    InputData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    InputData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
    InputData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 16);
    InputData[4] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);
    InputData[5] = Strtoul((const u8*)(argv[5]), (u8 **)NULL, 16);

    DBG_8195A("SOC PS Test\n");

    SOCPSTestApp((VOID*)InputData);

    return _TRUE;
}
#endif
#endif  //CONFIG_SOC_PS_MODULE

#ifdef CONFIG_PWM_EN
#ifdef CONFIG_PWM_VERIFY
extern VOID PWMTestApp(IN  VOID      *Data);

u32
CmdPWMTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 InPutData[5];

    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    InPutData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    InPutData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    InPutData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
    InPutData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 16);
    InPutData[4] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);

    PWMTestApp((VOID*)InPutData);

    return _TRUE;
}
#endif
#endif

#ifdef CONFIG_EFUSE_EN
#ifdef CONFIG_EFUSE_VERIFY
extern VOID EFUSETestApp(IN  VOID      *Data);

u32
CmdEFUSETest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 InPutData[8];

    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    InPutData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    InPutData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    InPutData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
    InPutData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 16);
    InPutData[4] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);
    InPutData[5] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);
    InPutData[6] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);
    InPutData[7] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);

    EFUSETestApp((VOID*)InPutData);

    return _TRUE;
}
#endif
#endif

#ifdef CONFIG_CRYPTO_VERIFY
extern int rtl_crypto_test(IN u16 argc, IN u8 *argv[]);

u32
CmdCryptoTest(
    IN u16 argc,
    IN u8  *argv[]
)
{
    return rtl_crypto_test(argc, argv);
}
#endif

#ifdef CONFIG_GPIO_VERIFY
extern VOID GPIOVerifyApp(IN VOID *Data);

u32
CmdGPIOTest(
    IN u16 argc,
    IN u8  *argv[]
)
{
    u8  index;
    u8  TEST_LENGTH = 3;
    u32 TestData[TEST_LENGTH];

    if(argc<1) {
        for (index = 0; index < TEST_LENGTH; ++index) {
            TestData[index] = 0xff;
        }
    } else {
        for (index = 0; index < TEST_LENGTH; ++index) {
            TestData[index] = Strtoul((const u8*)(argv[index]), (u8 **)NULL, 10);
        }
    }

    GPIOVerifyApp(TestData);
    return _TRUE;
}
#endif

#ifdef CONFIG_RTLIB_VERIFY
extern int rtl_lib_test(IN u16 argc, IN u8 *argv[]);

u32
CmdRTLIBTest(
    IN u16 argc,
    IN u8  *argv[]
)
{
    return rtl_lib_test(argc, argv);
}
#endif

#ifdef CONFIG_DAC_EN
#ifdef CONFIG_DAC_VERIFY
VOID DACTestApp(IN  VOID    *DACTestData);


u32
CmdDACTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32  DACTestData[6];

    if (argc<2) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    DACTestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);//VeriProcessCmd
    DACTestData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);//VeriItem
    DACTestData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);//VeriLoop
    DACTestData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);//VeriIndex
    //DACTestData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);//VeriMaster
    //DACTestData[4] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);//VeriSlave
    //DACTestData[5] = Strtoul((const u8*)(argv[5]), (u8 **)NULL, 10);//MtrRW

    DACTestApp(DACTestData);

    return _TRUE;
}
#endif
#endif

#ifdef CONFIG_ADC_EN
#ifdef CONFIG_ADC_VERIFY
VOID ADCTestApp(IN  VOID    *ADCTestData);


u32
CmdADCTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32  ADCTestData[6];

    if (argc<2) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    ADCTestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);//VeriProcessCmd
    ADCTestData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);//VeriItem
    ADCTestData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);//VeriLoop
    ADCTestData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);//VeriIndex
    //DACTestData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);//VeriMaster
    //DACTestData[4] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);//VeriSlave
    //DACTestData[5] = Strtoul((const u8*)(argv[5]), (u8 **)NULL, 10);//MtrRW

    ADCTestApp(ADCTestData);

    return _TRUE;
}
#endif
#endif
#ifdef CONFIG_USB_VERIFY
#ifdef DWC_DEVICE_ONLY

extern void OTGTestApp
(
    IN  u8  TestItem,
    IN  u32 OtgTestAddr,
    IN  u32 OtgTestVal
);


u32
CmdOTGTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32 TestItem = 0;
    u32 OtgTestAddr = 0;
    u32 OtgTestVal = 0;



    if (argc > 0) {
        TestItem = Strtoul((const char*)(argv[0]), (char **)NULL, 16);
    }

    if (argc > 1) {
        OtgTestAddr = Strtoul((const char*)(argv[1]), (char **)NULL, 16);
    }

    if (argc > 2) {
        OtgTestVal = Strtoul((const char*)(argv[2]), (char **)NULL, 16);
    }

    OTGTestApp((u8)TestItem,OtgTestAddr,OtgTestVal);


}

#endif  //#ifdef DWC_DEVICE_ONLY
#endif  //#ifdef CONFIG_USB_VERIFY

#if CONFIG_PERI_UPDATE_IMG
u32
CmdOTUFwUpdate(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32 uart_idx=0;
    u32 pin_mux=0;
    u32 baud_rate;

    if (argc<2) {
        DBG_8195A("Wrong argument number!\r\n");
        DBG_8195A("Command Format: FUOTU <UART Index, 0/1/2> <UART PinMux, 0/1/2> <Baud Rate, Dec> \r\n");
        return _FALSE;
    }

    uart_idx = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    pin_mux = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
    baud_rate = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
    OTU_FW_Update(uart_idx, pin_mux, baud_rate);
    return _TRUE;
}

u32
CmdOTUImgDwnld(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32 uart_idx=0;
    u32 pin_mux=0;
    u32 baud_rate;
    u32 start_offset;
    u32 start_addr;
    u32 max_size;

    if (argc<5) {
        DBG_8195A("Wrong argument number!\r\n");
        DBG_8195A("Command Format: IDOTU <UART Index, 0/1/2> <UART PinMux, 0/1/2> <Baud Rate, Dec>\r\n");
        DBG_8195A("                      <StartOffset, Dec> <StartAddr, Hex> <MaxSize, Dec>\r\n");
        return _FALSE;
    }

    uart_idx = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    pin_mux = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
    baud_rate = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
    start_offset = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);
    start_addr = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);
    max_size = Strtoul((const u8*)(argv[5]), (u8 **)NULL, 10);
    OTU_Img_Download(uart_idx, pin_mux, baud_rate, start_offset, start_addr, max_size);
    return _TRUE;
}


extern SPIC_INIT_PARA SpicInitParaAllClk[3][CPU_CLK_TYPE_NO];

u32
FlashDump(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32 addr;
    u32 len;
    u32 i, section;
    u32 buf;
    SPIC_INIT_PARA SpicInitPara;

    if (argc<1) {
        DBG_8195A("Command Format: FD <Flash Address, Hex> <Length, Dec>\r\n");
        return _FALSE;
    }

    addr = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    section = ((addr & 0x03000000) >> 24);
    addr &= 0x00FFFFFF;
    len = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
    DBG_8195A("Dump Flash: Addr=0x%x Len=%d Section=%d\r\n", addr, len, section);

    SPI_FLASH_PIN_FCTRL(ON);
    if (!SpicFlashInitRtl8195A(SpicOneBitMode)) {
        DBG_8195A("FlashDump: SPI Init Fail!!\n");
        return _FALSE;
    }

    SpicInitPara.flashtype  = SpicInitParaAllClk[SpicOneBitMode][0].flashtype;

    if(SpicInitPara.flashtype == FLASH_MICRON) {
        SpicSetExtendAddrRtl8195A(section, SpicInitPara);
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    }

    addr &= (~0x03);    // make sure it's 4-bytes aligned
    DBG_8195A("\r\n");
    while (len > 0) {
        DiagPrintf("%08x: ", addr);
        for (i=0; i<4; i++) {
            buf = HAL_READ32(SPI_FLASH_BASE, addr);
            DiagPrintf("%02x %02x %02x %02x ", buf&0xff, (buf >> 8)&0xff, (buf >> 16)&0xff, (buf >> 24)&0xff);
            addr += 4;
            if (len >=4) {
                len -= 4;
            } else {
                len = 0;
            }

            if (len == 0) {
                break;
            }
        }
        DBG_8195A("\r\n");
    }
    if(SpicInitPara.flashtype == FLASH_MICRON) {
        SpicSetExtendAddrRtl8195A(0, SpicInitPara);
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    }
    SPI_FLASH_PIN_FCTRL(OFF);
    return _TRUE;
}

u32
FlashErase(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32 addr;
    u32 ksize;
    u32 i, section;
    SPIC_INIT_PARA SpicInitPara;

    if (argc<1) {
        DBG_8195A("Command Format: FE <Flash Address, Hex> <Page Num, Dec>\r\n");
        return _FALSE;
    }

    addr = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    section = ((addr & 0x03000000) >> 24);
    addr &= 0x00FFFFFF;
    ksize = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

    SPI_FLASH_PIN_FCTRL(ON);
    if (!SpicFlashInitRtl8195A(SpicOneBitMode)) {
        DBG_8195A("Flash Erase: SPI Init Fail!!\n");
        return _FALSE;
    }
    SpicInitPara.flashtype  = SpicInitParaAllClk[SpicOneBitMode][0].flashtype;

    if(SpicInitPara.flashtype == FLASH_MICRON) {
        SpicSetExtendAddrRtl8195A(section, SpicInitPara);
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    }
    addr &= (~0xfff);    // make sure it's 4K aligned
    DBG_8195A("Erase Flash Sector: Addr=0x%x Size=%dK Section=%d\r\n", addr, ksize, section);

    for (i=0; i<ksize; i+=4) {
        DBG_SPIF_INFO("Flash Erase: 0x%x\n", addr);
        SpicSectorEraseFlashRtl8195A(SPI_FLASH_BASE + addr);
        addr += 0x1000;  // 1 sector = 4k bytes
    }

    if(SpicInitPara.flashtype == FLASH_MICRON) {
        SpicSetExtendAddrRtl8195A(0, SpicInitPara);
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    }

    SPI_FLASH_PIN_FCTRL(OFF);

    return _TRUE;
}


u32
FlashWriteWord(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32 Src;
    u32 Value,Index, section;
    SPIC_INIT_PARA SpicInitPara;

    Src = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    while ( (Src) & 0x03) {
        Src++;
    }
    section = ((Src & 0x03000000) >> 24);
    Src &= 0x00FFFFFF;

    SPI_FLASH_PIN_FCTRL(ON);
    if (!SpicFlashInitRtl8195A(SpicOneBitMode)) {
        DBG_8195A("Flash Write: SPI Init Fail!!\n");
        return _FALSE;
    }

    SpicInitPara.flashtype  = SpicInitParaAllClk[SpicOneBitMode][0].flashtype;

    if(SpicInitPara.flashtype == FLASH_MICRON) {
        SpicSetExtendAddrRtl8195A(section, SpicInitPara);
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    }

    for(Index=0; Index<argc-1; Index++,Src+=4) {
        Value= Strtoul((const u8*)(argv[Index+1]), (u8 **)NULL, 16);
        DBG_8195A("Flash Write Word: 0x%x = 0x%x, section=%d\n", Src, Value, section);
        HAL_WRITE32(SPI_FLASH_BASE, Src, Value);

        SpicWaitBusyDoneRtl8195A();

        if(SpicInitPara.flashtype == FLASH_MICRON) {
            SpicWaitOperationDoneRtl8195A(SpicInitPara);
            DBG_8195A("Flash type = FLASH_MICRON\n", Src, Value);
        } else
            SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    }

    if(SpicInitPara.flashtype == FLASH_MICRON) {
        SpicSetExtendAddrRtl8195A(0, SpicInitPara);
        SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
    }

    SPI_FLASH_PIN_FCTRL(OFF);

    return _TRUE;
}

#endif  // #if CONFIG_PERI_UPDATE_IMG

#if CONFIG_BTBX_MODULE
#if CONFIG_BTBX_TEST
extern VOID BTBXTestApp(IN  VOID *Data);

u32
CmdBTBXTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 InPutData[5];

    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    InPutData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    InPutData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 16);
    InPutData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
    InPutData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 16);
    InPutData[4] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 16);

    BTBXTestApp((VOID*)InPutData);

    return _TRUE;
}
#endif  //CONFIG_BTBX_TEST
#endif  //CONFIG_BTBX_MODULE

