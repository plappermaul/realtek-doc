
/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_monitor.c
	
Abstract:
	Define 98E test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-05-19 Eric            Create.	
--*/


#include <common.h>
//#include <linux/autoconf.h> // using George menuconfig
#include <command.h>
#include <basic_types.h>
#include "cmd_monitor.h"
#include <checkRule.h>

unsigned char currentItem[5];


//MON_RAM_DATA_SECTION
COMMAND_TABLE   UartLogRamCmdTable[] = {
   {(const s8*)"?",        0, CmdRamHelp,        (const u8*)"\tHELP (?)   : Print this RAM help messag\n"},
   {(const s8*)"DB",    2, CmdDumpByte,      (const u8*)"\tDB <Address, Hex> <Len, Dec>: \n"
                                                        "\t\t\t\t Dump memory byte or Read Hw byte register"},
   {(const s8*)"DHW",   2, CmdDumpHelfWord,  (const u8*)"\tDHW <Address, Hex> <Len, Dec>: \n"
                                                        "\t\t\t\t Dump memory helf-word or Read Hw helf-word register;\n"
                                                        "\t\t\t\t Unit: 4Bytes"},
   {(const s8*)"DW",    2, CmdDumpWord,      (const u8*)"\tDW <Address, Hex> <Len, Dec>: \n"
                                                        "\t\t\t\t Dump memory word or Read Hw word register; \n"
                                                        "\t\t\t\t Unit: 4Bytes"},
   {(const s8*)"EB",    2, CmdWriteByte,     (const u8*)"\tEB <Address, Hex> <Value, Hex>: \n"
                                                        "\t\t\t\t Write memory byte or Write Hw byte register \n"
                                                        "\t\t\t\t Can write more word at the same time \n"
                                                        "\t\t\t\t Ex: EB Address Value0 Value1"},
   {(const s8*)"EHW",   2, CmdWriteHword,     (const u8*)"\tEHW <Address, Hex> <Value, Hex>: \n"
                                                        "\t\t\t\t Write memory half word or Write Hw byte register \n"
                                                        "\t\t\t\t Can write more word at the same time \n"
                                                        "\t\t\t\t Ex: EHW Address Value0 Value1"},                                                        
   {(const s8*)"EW",    2, CmdWriteWord,     (const u8*)"\tEW <Address, Hex> <Value, Hex>: \n"
                                                        "\t\t\t\t Write memory word or Write Hw word register \n"
                                                        "\t\t\t\t Can write more word at the same time \n"
                                                        "\t\t\t\t Ex: EW Address Value0 Value1"},
#ifdef SOC_PLATFORM
         {(const s8*)"SOC_TEST",  7, CmdSOCTest,      (const u8*)"\t CmdSOCTest \n"
                                                            "\t\t\t\t \n"},
#endif  
                                                        
#ifdef CONFIG_CMD_RTK_GDMA
   {(const s8*)"GDMA",  7, CmdGdmaTest,      (const u8*)"\tGdma \n"
                                                        "\t\t <Loop, Dec> <Length, Dec> <Gdam Type, Dec> \n"
                                                        "\t\t <Memory Type, Dec> <Gdma Number, Dec> <Pattern, Dec> <Auto, Dec> :\n"
                                                        "\t\t\t\t Loop: The number of the test\n"
                                                        "\t\t\t\t Length: The transfer length of the single test\n"
                                                        "\t\t\t\t Gdma Type: Single/Multi-Bock/Autoreload/LLP\n"
                                                        "\t\t\t\t Memory Type: Src:SD SRAM/SF SRAM; Dst:SD SRAM/SF SRAM;\n"
                                                        "\t\t\t\t Gdma Number: Gdma0/Gdma1\n"
                                                        "\t\t\t\t Pattern: 0x5A/Incremental\n"
                                                        "\t\t\t\t Auto: Auto-generate the below parameter\n"},
#endif

#ifdef CONFIG_SDIO_DEVICE_EN
#ifdef CONFIG_SDIO_DEVICE_VERIFY            
   {(const s8*)"SDIO_DEVICE",  7, CmdSdioDeviceTest,      (const u8*)"\tSdiodevice \n"
                                                        			"\t\t <Test Item, Dec> \n"},
#else
	{(const s8*)"SDIO",	2, CmdSdioDeviceMPTest,   (const u8*)"\tSDIO\n"
															 "\t\t <MP Command> <Para 1> <Para 2> ... \n"},
#endif
#endif	// end of "#ifdef CONFIG_SDIO_DEVICE_EN"

#ifdef CONFIG_CMD_RTK_WLAN

  /*  {(const s8*)"WLAN", 1, CmdWlanTest,     (const u8*)"\tWlan \n"
                                                       "\t\t <Test Item, Dec> <Loop, Dec> <Pattern, Dec> \n"
                                                       "\t\t <Length, Dec> <AC Queue, Dec> <Rate ID, Dec>:\n"
                                                       "\t\t\t\t Test Item: 1: Initial; 2: TRX Loopback\n"
                                                       "\t\t\t\t Loop: The number of the test\n"
                                                       "\t\t\t\t Pattern: 0xFF/0x5A/Ramdon\n"
                                                       "\t\t\t\t Length: 0: Ramdon; \n"
                                                       "\t\t\t\t AC Queue: 0-7 \n"
                                                       "\t\t\t\t Rate ID\n"},*/
    {(const s8*) "WLCTRL",  2,  CmdWLOprater, (const u8*)"\t WLCTRL: mode 1:initial, mode 2:test mode \n"},   
    {(const s8*) "WLTX",   1,  CmdSendTest,(const u8*) "\t WLTX: Sent Packet \n"},    
    //{(const s8*) "WLAN", 1, CmdWlanTest,     (const u8*)"\t Wlan \n"},
//    {(const s8*) "WLRX",   1,  CmdPollingReceiveTest, (const u8*) "\t WLRX: Poll Receive Packet \n"},
    {(const s8*) "RSTWL",   0,  CmdRsetWL,(const u8*) "\t RSTWL: Reset WL \n"},
    {(const s8*) "IOTEST",   1,  CmdIoTest, (const u8*)"\t IOTEST: Test IO \n"},
    {(const s8*) "FWDL",   0,  CmdFWDL, (const u8*)"\t FWDL: Firmware Download \n"},
    {(const s8*) "FWVERI",   1,  CmdFWVERI, (const u8*)"\t FWVERI: Firmware verification \n"},
   // {(const s8*) "WLLP",   4,  CmdWLLoopBack, (const u8*)"\t WLLP:WL HCI Loopback Test \n"},
    {(const s8*) "WLTXC",   3,  CmdWLTxCont, (const u8*)"\t WLTXC:WLTX Continus Send Test \n"},
   // {(const s8*) "WLTXRF",   3,  CmdWLTx,(const u8*) "\t WLTXRF:WLTX Continus Send Test for RF \n"},
    {(const s8*) "WLDMABCN",  0,  CmdDmaBeacon, (const u8*)"\t WLDMABCN: WL MAC Dma packet to TX reserved pages \n"},
    {(const s8*) "WLPOWER",  0,  CmdWlPower, (const u8*)"\t WLPOWER: Test Wifi Power Sequence \n"},    
    {(const s8*) "WLALL",  0,  CmdWlAllTest, (const u8*)"\t WLALL: Test Wifi all test \n"},        
    {(const s8*) "WLDUMP",  0,  CmdWlDumpAll, (const u8*)"\t WLDUMP: dump all register \n"},    
    {(const s8*) "WLPKTOPT",  7,  CmdSetPktOpt, (const u8*)"\t WLPKTOPT: set pkt option \n"},
    {(const s8*) "WLSWPS",  3,  CmdSWPSTest, (const u8*)"\t WLSWPS: SWPS verifiaction \n"},
	{(const s8*) "WLMBIST",  3,  CmdMBIST, (const u8*)"\t WLMBIST: MBIST test, 1:BSSID CAM, 2:Security CAM\n"},
    {(const s8*) "WLCAMEX",  3,  CmdCAMExtend, (const u8*)"\t WLCAMEX: CAM extend test, 1:BSSID CAM, 2:Security CAM\n"},
    {(const s8*) "WLTestALL",  3,  CmdWLTestALL, (const u8*)"\t Test all the wifi items\n"},
#endif

#ifdef CONFIG_WIFI_NORMAL    
	{(const s8*)"WLAN", 1, CmdWlanNormal,     (const u8*)"\tWlan \n"                                                       
							"\t\t WLAN Driver Test\n"},
#endif

#ifdef CONFIG_SDR_VERIFY
    {(const s8*)"SDR",    2, CmdSdrTest,     (const u8*)"\tSDR \n"
                                                        "\t\t <Mode, Dec> <Loop, Dec>: \n"
                                                        "\t\t\t\t Mode: Initial = 1; Write and Read compare = 2 \n"
                                                        "\t\t\t\t Loop: The number of Write and Read compare \n"},
#endif
#ifdef CONFIG_SPIC_VERIFY
    {(const s8*)"SPIC",   2, CmdSpiFlashTest,(const u8*)"\tSPIC \n"
                                                        "\t\t <Mode, Dec> <Loop, Dec>: \n"
                                                        "\t\t\t\t Mode: Initial = 1; Write and Read compare = 2 \n"
                                                        "\t\t\t\t Loop: The number of Write and Read compare \n"},
#endif
#ifdef CONFIG_CMD_RTK_I2S
   {(const s8*)"I2S",  7, CmdI2STest,      (const u8*)"\tI2S \n"
                                                      "\t\t <mode, Dec>"},
#endif
#ifdef CONFIG_CMD_RTK_PARALLEL_NAND
   {(const s8*)"NAND",  7, CmdParallelNandTest,      (const u8*)"\tNAND \n"
                                                      "\t\t <mode, Dec>"},
  {(const s8*)"MONFLG",  7, CmdMonflg,      (const u8*)"\tMONFLG \n"
                                                      "\t\t <Monflg, Dec>"},                                                      
#endif
#ifdef CONFIG_CMD_RTK_I2S
   {(const s8*)"I2SCONFIG",  7, CmdI2SConfig,      (const u8*)"\tI2SCONFIG \n"
                                                      "\t\t <WordLength, Dec> <CHNum, Dec>"},
#endif
#ifdef CONFIG_I2S_NORMAL            
   {(const s8*)"I2SDEMO",  7, CmdI2sTest,      (const u8*)"\tI2SDEMO \n"
                                                      "\t\t <mode, Dec>"},
#endif
#if defined(CONFIG_PCM_VERIFY) || defined(CONFIG_CMD_RTK_PCM)
   {(const s8*)"PCM",  7, CmdPCMTest,      (const u8*)"\tPCM \n"},
#endif
#ifdef CONFIG_CMD_CA_RTC
        {(const s8*)"RTC",10, CmdRTCTest,                (const u8*)"\tRTC \n"
                                                                                                           "\t\t CA RTC Verification Program \n"
                                                                                                           "\t\t <Test Item, Dec> <Loop, Dec> <year, Dec> <month, Dec> <week, Dec> <day, Dec> <hour, Dec> <minute, Dec> <second, Dec> <alarm type, Dec>\n"
                                                                                                           "\n"},
#endif
#if 0
//#ifdef CONFIG_DEBUG_LOG
	{(const s8*)"DEBUG",	2, CmdDbgMsgCfg,		(const u8*)"\tDEBUG \n"
															"\tConfig Debugging Message \n"
															"\t\t <DbgLevel: error,warn,trace,info> <ConfigValue, Hex>"},
#endif
#ifdef CONFIG_CMD_RTK_SPI	//CONFIG_SPI_COM_VERIFY
    {(const s8*)"SSI", 10, CmdSsiTest,       (const u8*)"\tSSI \n"
                                                        "\t\t DesignWare SSI Verification Program \n"},
#endif
#ifdef CONFIG_CMD_RTK_UART
    {(const s8*)"UART", 7, CmdRuartTest,    (const u8*)"\tUART \n"
                                                        "\t\t UART IP verify \n"
                                                        "\t\t <Test Item, Dec> <Loop, Dec>\n"
                                                        "\t\t\t\t Test Item: \n"
                                                        "\t\t\t\t 0: Pulling\n"
                                                        "\t\t\t\t 1: Basic Interrupts\n"
                                                        "\t\t\t\t 2: Receive Line Status Interrupt (No need)\n"
                                                        "\t\t\t\t 3: Modem Status Interrupt\n"
                                                        "\t\t\t\t 4: DMA Tx\n"
                                                        "\t\t\t\t 5: DMA Rx\n"
                                                        "\t\t\t\t 6: DMA TxRx\n"
                                                        "\t\t\t\t 7: Loopback\n"
                                                        "\n"},
#endif
#ifdef CONFIG_CMD_RTK_I2C
    {(const s8*)"I2C",  7, CmdI2CTest,      (const u8*)"\tI2C \n"
                                                       "\t\t <Test Item, Dec> <Loop, Dec> <I2C RW Cmd, Dec> \n"
                                                       "\t\t\t\t Test Item: \n"
                                                       "\t\t\t\t 1: I2C single RW without interrupt and DMA \n"
                                                       "\t\t\t\t 2: I2C single RW with interrupt but without DMA \n"
                                                       "\t\t\t\t 3: I2C RW with interrupt and DMA \n"
                                                       //"\t\t\t\t 4: Tx+Rx DMA test by master read \n"
                                                       //"\t\t\t\t 5: Error Interrupts test\n"
                                                       //"\t\t\t\t 6: General call test\n"
                                                       //"\t\t\t\t 7: TxAbort Interrupt test\n"
                                                       "\t\t\t\t \n"},
	{(const s8*)"RTKI2C", 6, CmdRTKI2CTest,    (const u8*)"\tRTKI2C \n"
	                                                        "\t\t RTK I2C IP verify \n"},                                                           
  #ifdef CONFIG_I2C_CODEC_ALC5651
    {(const s8*)"I2CCODEC",  7, CmdI2CCodecTest,      (const u8*)"\tI2CCodec \n"
                                                       "\t\t\t\t \n"},
  #endif
#endif
#ifdef CONFIG_CMD_RTK_PCIE
    {(const s8*)"PCIE", 4, CmdPCIeTest,     (const u8*)"\tPCIE "
                                                       "<Test Item, Dec> <Test Sub_Item, Dec> <PCIe Index, Dec> <Loop Count, Dec>\n"
                                                       "\t\t\t\t Test Item: 0: Link Initial; 1: TRX Loopback; 2: Power Related Function; 3: Interrupt\n"
                                                       "\t\t\t\t Test Sub_Item:\n"
#if 0
                                                       "\t\t\t\t\t (Test Item 0): 0: PCIE Host Link Initial\n"
                                                       "\t\t\t\t\t (Test Item 1): 0: Data Loop back\n"
                                                       "\t\t\t\t\t (Test Item 2): 0: D0 State\n"
                                                       "\t\t\t\t\t                1: D3hot State in L1\n"
                                                       "\t\t\t\t\t                2: D3cold State in L2\n"
                                                       "\t\t\t\t\t                3: ASPM L0s\n"
                                                       "\t\t\t\t\t                4: ASPM L1\n"
                                                       "\t\t\t\t\t                5: L2/L3 Handshake Sequence\n"
                                                       "\t\t\t\t\t                6: Disable ASPM\n"
                                                       "\t\t\t\t\t                7: L0->L1->L0\n"
                                                       "\t\t\t\t\t                8: L0->L2->L0\n"
                                                       "\t\t\t\t\t (Test Item 3): 0: Interrrupt\n"
#endif
                                                       "\t\t\t\t PCIe Index: The selected PCIe Index\n"
                                                       "\t\t\t\t Loop Count: The test count\n"},
#endif

#ifdef CONFIG_CMD_RTK_TIMER
	{(const s8*)"GTimer",5, CmdTimerTest,	(const u8*)	"\tGTimer \n"
														"\t\t DesignWare GTimer Verification Program \n"
													  	"\t\t <timeout count, Dec> <Timer value Us, Dec> <loop count, Dec>: \n"},

	{(const s8*)"Watchdog",5, CmdWatchdogTest,	(const u8*) "\tWatchdog "
														"<Mode, Dec> <Timer Value, Dec> \n"
														"\t\t\t\t Mode: 0: Initialization; 1: Start & Reset; 2: Start & Clear WD counter repeatedly; 3: Stop\n"
														"\t\t\t\t Timer Value: 0: 2s; 1: 4s; 2: 8s; 3: 16s; ... ; 9: 1024s\n"},
	{(const s8*)"ChangeCpuFreq",5, CmdChangeCpuFreqTest,	(const u8*) "\tChangeCpuFreq "
														"<Mode> \n"
														"\t\t\t\t Mode: 1: Change CPU frequency; 2: Enable watchdog timer; 3: Change CPU frequency and enable watchdog timer\n"},
#endif

  #ifdef CONFIG_CMD_RTK_SD30
    {(const s8*)"SD30",  7, CmdSD30Test,      (const u8*)"\tSD30 \n"
                                                       "\t\t\t\t \n"},
  #endif

  #ifdef CONFIG_CMD_RTK_EMMC
    {(const s8*)"EMMC",  7, CmdEMMCTest,      (const u8*)"\tEMMC \n"
                                                       "\t\t\t\t \n"},
  #endif

  #ifdef CONFIG_CMD_RTK_CRYPTO
    {(const s8*)"CRYPTO",  4, CmdCRYPTOTest,      (const u8*)"\tCRYPTO <item> <mode> <algo> <time>\n"
                                                       "\t\t\t\t \n"},
  #endif
  #ifdef CONFIG_CMD_RTK_PWM
	{(const s8*)"PWMEVT",7, CmdPWMTest,		 (const u8*)"\tPWMEVT \n"
													   "\t\t RTK PWM Verification Program \n"
													   "\t\t <Test Item, Dec> <Loop, Dec> <TimerSelect, Dec> <CountDownValue(Us), Dec> <PWM_EVT select, Dec>\n"
													   "\t\t\t\t Test Item: \n"
													   "\t\t\t\t 1: PWM \n"
                                                       "\t\t\t\t 2: Timer Event \n"
													   "\n"},
  #endif

};


u32
CmdRamHelp(
    IN  u16 argc, 
    IN  u8  *argv[] 
)
{
	u32	LoopINdex ;

    printf("----------------- COMMAND MODE HELP ------------------\n");
	for( LoopINdex=0  ; LoopINdex < (sizeof(UartLogRamCmdTable) / sizeof(COMMAND_TABLE)) ; LoopINdex++ )
	{
		if( UartLogRamCmdTable[LoopINdex].msg )
		{
			printf( "%s\n",UartLogRamCmdTable[LoopINdex].msg );
		}
	}
    printf("----------------- COMMAND MODE END  ------------------\n");

	return _TRUE ;
}

u32
CmdSOCTest(
    IN  u16 argc, 
    IN  u8  *argv[] 
)
{
    AUTOTEST_REVISION_PRINT();
    AUTOTEST_TEST_START_PRINT();
    AUTOTEST_TEST_END_PASS_PRINT();

	return _TRUE ;
}



static int do_soc_test(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
	int		i, retval ;
    char * tmp;

	if (argc < 2)
    {   
        printf("Unknow soc test command\n");    
		return CMD_RET_USAGE;
    }

   // StrUpr( argv[1] );

    // add for SOC auto test tool    
    tmp = strchr(argv[1],'#');
    if(tmp)
    {
        memcpy(currentItem,tmp+1,5);
        memset(tmp,0,2);        
        //argv[1] = (tmp+1);
        printf("argv[1]=%s \n",argv[1]);   
    }
    
            
    for( i=0 ; i < (sizeof(UartLogRamCmdTable) / sizeof(COMMAND_TABLE)) ; i++ )
    {
        
        if( ! strcasecmp( argv[1], (char *)UartLogRamCmdTable[i].cmd ) )
        {
            retval = UartLogRamCmdTable[i].func( argc - 2 , (u8**)(argv+2) );
            memset(argv[1],0,sizeof(argv[1]));
            break;
        }
    }
    if(i==sizeof(UartLogRamCmdTable) / sizeof(COMMAND_TABLE)) printf("Unknown command !\r\n");

	return CMD_RET_SUCCESS;
}

//jwsyu 20140924 change 5 to 6 (maxargs)
//Carl 20151116 change 9 to 11 (maxargs)
U_BOOT_CMD(socTest, 11, 0,	do_soc_test,
	    "Test98E socTest \n",
	    ""
);



