

/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	cmd_i2s.c
	
Abstract:
	 I2S test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-07-28 Jwsyu             Create.	
--*/

#include <common.h>
#include <command.h>
#include <basic_types.h>
#include "cmd_para_nand.h"
#include "diag.h"

extern VOID  rtk_nand_test (VOID *Data);
extern VOID  rtk_nand_set_monflg(VOID *Data);


u32
CmdParallelNandTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32  InPutData[16];

    if((argv[0]) && (argv[1]))
    {

    }
    else
    {
        printf("Wrong argument number\n");
        printf("socTest nand <test_item> <page_number>\n");
	return _FALSE;
    }
    InPutData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    InPutData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
    InPutData[2] = 0;
    InPutData[3] = 0;	
    rtk_nand_test(InPutData);
#if 0
#ifdef I2S_FREE_RTOS_TASK_LOOPBACK_TEST
    u32  InPutData[16], i;


    InPutData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    InPutData[1] = 0;
    InPutData[2] = 0;
    InPutData[3] = 0;
#else
    u8  InPutData[16], i;
    u32 mode, mode2, mode3;
    u32 I2sWordLength=config_I2sWordLength; //(2:32bit, 1:24bit, 0:16bit)
    u32 I2sLRSwap=0;
    u32 I2sIndex=0;
    u32 I2sTestMode=0;
    u32 I2sCHNum=config_I2sCHNum; //(1:stereo, 0:mono)
    u32 I2sPageSize=40-1;
    u32 I2sPageNum=4-1;
    u32 I2sSampleRate=0;

    I2sPageSize=i2s_rate_tab1[I2sSampleRate]/1000*I2sPageTime*(i2s_ch_channelnum_tab[I2sCHNum])*(1+(I2sWordLength>0))/20-1;
//InPutData[0]bit2-3 : I2sWordLength (2:32bit, 1:24bit, 0:16bit)
//InPutData[0]bit6 : I2sLRSwap (1:lrswap, 0:normal)
//InPutData[0]bit7 : I2sIndex
//InPutData[1]bit3-0 : I2sTestMode
//InPutData[1]bit5-4 : I2sCHNum
//InPutData[3]bit3-[2]bit0 : I2sPageSize
//InputData[3]bit5-4 : I2sPageNum
//InputData[4]bit4-0 : I2sSampleRate
    InPutData[0]=((0)<<7)|(I2sIndex<<7)|(I2sLRSwap<<6)|(I2sWordLength<<2);
    InPutData[1]=((0)<<7)|(I2sCHNum<<4)|(I2sTestMode);
    InPutData[2]=0xff&(I2sPageSize);
    InPutData[3]=(0<<6)|(I2sPageNum<<4)|(I2sPageSize>>8);
    InPutData[4]=(I2sSampleRate);
#ifdef I2S_TEST_PAGE_NUM //change page num from 2to4
    mode= Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    I2sPageNum= mode-1;
    InPutData[3]=(0<<6)|(I2sPageNum<<4)|(I2sPageSize>>8);
#endif

#ifdef I2S_TEST_SAMPLERATE //change sample rate from 0, 1, 2, 3, 4, 5, 6 7, 8, 9
                           //                       16, 17, 18, 19, 20, 21, 22, 23, 24
    mode2= Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
    I2sSampleRate= mode2;
    I2sPageSize=i2s_rate_tab1[I2sSampleRate]/1000*I2sPageTime*(i2s_ch_channelnum_tab[I2sCHNum])*(1+(I2sWordLength>0))/20-1;
    InPutData[2]=0xff&(I2sPageSize);
    InPutData[3]=(0<<6)|(I2sPageNum<<4)|(I2sPageSize>>8);
    InPutData[4]=(I2sSampleRate);
#endif

#ifdef I2S_TEST_PAGE_TIME //change page time from 10, 25, 50, 100, 200, 400
    mode3= Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
    I2sPageTime= mode3;
    I2sPageSize=i2s_rate_tab1[I2sSampleRate]/1000*I2sPageTime*(i2s_ch_channelnum_tab[I2sCHNum])*(1+(I2sWordLength>0))/20-1;
    InPutData[2]=0xff&(I2sPageSize);
    InPutData[3]=(0<<6)|(I2sPageNum<<4)|(I2sPageSize>>8);
#endif
#endif
    for (i=0;i<5;i++) {
        DBG_8195A("%d-0x%02x\n", i, InPutData[i]);
    }

    I2STestApp(InPutData);
#endif

    return _TRUE;
}

u32
CmdMonflg(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	if((argv[0]) && (argv[1]))
	{

	}
	else
	{
	    printf("Wrong argument number\n");
	    printf("socTest monflg <monflg> <ecc_select>\n");
	    return _FALSE;
	}

	u32  InPutData[16];
	InPutData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
	InPutData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
	
	rtk_nand_set_monflg(InPutData);
}


