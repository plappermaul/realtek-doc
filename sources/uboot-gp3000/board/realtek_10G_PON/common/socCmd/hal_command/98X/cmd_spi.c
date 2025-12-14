/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_ssi.c
	
Abstract:
	Define 98E test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-11-10 Carl            Create.	
--*/

#include <common.h>
#include <command.h>
#include <basic_types.h>
#include "cmd_spi.h"

extern VOID SsiTestApp(VOID *Data);
//extern int SsiMain(VOID *Data);

u32 CmdSsiTest(
        IN u16 argc,
        IN u8  *argv[]
)
{
    u8 index;
    u8 TEST_LENGTH = 9;
    u8 TestData[TEST_LENGTH];

    if(argc < 2){
	printf("argc is not enough!!!\n");
        printf("Usage:\n");
        printf("socTest SSI\n");
        printf("argc0         0   TestFrameFormat [0-2]\n");
        printf("                  0   SPI        Motorola Serial Peripheral Interface\n");
        printf("                  1   SSP        Texas Instruments Serial Protocol\n");
        printf("                  2   Micorwire  National Semiconductor Microwire\n");
        printf("argc1         0   SSI0Index\n");
        printf("argc2         1   SSI1Index\n");
        printf("argc3         0   TestType [0-3]\n");
        printf("                  0   SSI_TT_BASIC_IO\n");
        printf("                  1   SSI_TT_INTERRUPT\n");
        printf("                  2   SSI_TT_DMA_TO_TX\n");
        printf("                  3   SSI_TT_RX_TO_DMA\n");
        printf("argc4         x   BaudRateBase (BAUDR = x * 100)\n");
        printf("argc5         0   R/W DR (2 bytes) commands [1-10]\n");
        printf("                  1   SPI0: SOC_HAL_R16\n");
        printf("                  2   SPI1: SOC_HAL_R16\n");
        printf("                  3   SPI0: SOC_HAL_W16\n");
        printf("                  4   SPI1: SOC_HAL_W16\n");
        printf("                  5   dump whole SPI0 DR data\n");
        printf("                  6   dump whole SPI1 DR data\n");
        printf("argc6         0   Switch command [1-2]\n");
        printf("                  1   Disable SSI\n");
        printf("                  2   Enable SSI\n");
        printf("argc7         0   Write Data\n");
        printf("argc8        16   TestTime\n");
        printf("argc9         0   Wire Mode\n");
        printf("                  0 4-wire-Mode\n");
        printf("                  1 3-wire-Mode\n");
        return _TRUE;
    }
	
    for(index = 0; index < TEST_LENGTH; ++index) {
        TestData[index] = Strtoul((const u8*)(argv[index]), (u8 **)NULL, 10);
        /*printf("TestData[%d] = %d\n",index,TestData[index]);*/
    }

    SsiTestApp(TestData);
    //SsiMain(TestData);

    return _TRUE;
}

#if 0
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
