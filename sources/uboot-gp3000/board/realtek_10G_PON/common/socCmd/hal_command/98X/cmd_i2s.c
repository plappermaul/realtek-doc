

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
#include "cmd_i2s.h"
#include "diag.h"

extern  VOID    I2STestApp(VOID *Data);


#define I2S_TEST_PAGE_NUM
#define I2S_TEST_SAMPLERATE
#define I2S_TEST_PAGE_TIME
u32 I2sPageTime=100; //unit 0.1ms
s32 i2s_rate_tab1[]={8000, 12000, 16000, 24000, 32000, 48000, 64000, 96000, 
                   192000, 384000, 0, 0, 0, 0, 0, 0,
                     7350, 11025, 14700, 22050, 29400, 44100, 58800, 88200,
                   175400, 0,      0, 0, 0, 0, 0, 0};
s32 i2s_ch_channelnum_tab[]={1, 2, 6};

u32 config_I2sWordLength=0; //(2:32bit, 1:24bit, 0:16bit)
u32 config_I2sCHNum=1; //(1:stereo, 0:mono)

extern u32 i2s_test_time_sec;

u32
CmdI2STest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
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

    if(argc < 2){
        printf("argc is not enough!!!\n");
        printf("Usage:\n");
        printf("socTest I2S [page_num] [sample_rate] [page_time]\n");
        return _TRUE;
    }

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
#if 1
    for (i=0;i<5;i++) {
        printf("%d-0x%02x\n", i, InPutData[i]);
    }
#endif
    I2STestApp(InPutData);


    return _TRUE;
}



u32
CmdI2SConfig(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    if(argc < 3)
    {
        printf("argc is not enough!!!\n");
        printf("Usage:\n");
        printf("socTest I2SCONFIG [WordLength] [CHNum] [Testtime]\n");
    }

    config_I2sWordLength = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    config_I2sCHNum = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);
	i2s_test_time_sec = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
    printf("i2s config WordLength=%d, CHNum=%d, testtime=%d\n", config_I2sWordLength, config_I2sCHNum, i2s_test_time_sec);

    return _TRUE;
}

