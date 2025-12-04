
/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_monitor.h
	
Abstract:
	Define 98E test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-05-19 Eric            Create.	
--*/


#ifndef _CMD_MONITOR_H_
#define _CMD_MONITOR_H_

#include "cmd_common.h"
#include "cmd_i2c.h"
#include "cmd_spi.h"
#include "cmd_pcie.h"
#include "cmd_uart.h"
#include "cmd_wlan.h"
#include "cmd_timer.h"
#include "cmd_rtc.h"

#ifdef CONFIG_CMD_RTK_GDMA     //CONFIG_GDMA_VERIFY
u32 
CmdGdmaTest(
    IN  u16 argc, 
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_SDIO_DEVICE_VERIFY
u32 
CmdSdioDeviceTest(
    IN  u16 argc, 
    IN  u8  *argv[]
);
#else
u32 
CmdSdioDeviceMPTest(
    IN  u16 argc, 
    IN  u8  *argv[]
);
#endif

#ifdef CONFIG_CMD_RTK_WLAN
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

#ifdef CONFIG_CMD_RTK_I2S
u32 
CmdI2STest(
    IN  u16 argc, 
    IN  u8  *argv[]
);
u32 
CmdI2SConfig(
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

#ifdef CONFIG_CMD_RTK_PARALLEL_NAND
u32 
CmdParallelNandTest(
    IN  u16 argc, 
    IN  u8  *argv[]
);
u32 
CmdMonflg(
    IN  u16 argc, 
    IN  u8  *argv[]
);
#endif

#if defined(CONFIG_PCM_VERIFY) || defined(CONFIG_CMD_RTK_PCM)
u32 
CmdPCMTest(
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
  #ifdef CONFIG_I2C_CODEC_ALC5651
u32
CmdI2CCodecTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
  #endif
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

#ifdef CONFIG_CMD_RTK_SPI   //CONFIG_SPI_COM_VERIFY
u32
CmdSsiTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_CMD_CA_RTC
u32
CmdRTCTest(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif


#ifdef CONFIG_UART_VERIFY
u32
CmdRuartTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_CMD_RTK_SD30
u32
CmdSD30Test(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_CMD_RTK_EMMC
u32
CmdEMMCTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_CMD_RTK_CRYPTO
u32
CmdCRYPTOTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

#ifdef CONFIG_CMD_RTK_PWM
u32
CmdPWMTest(
    IN u16 argc,
    IN u8  *argv[]
);
#endif

typedef struct _COMMAND_TABLE_ {
    const   s8* cmd;
    u16     ArgvCnt;
    u32     (*func)(u16 argc, u8* argv[]);
    const   u8* msg;
}COMMAND_TABLE, *PCOMMAND_TABLE;

#endif //_CMD_MONITOR_H_


