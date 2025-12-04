
/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_timer.c
	
Abstract:
	Define 98E test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-05-19 Eric            Create.	
--*/

#include <common.h>
#include <command.h>
#include <basic_types.h>
#include "cmd_timer.h"

extern  VOID    TimerTestApp(VOID *Data);
extern  VOID    WatchdogTestApp(VOID *Data);

u32
CmdTimerTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	
    u32  TestData[3];
	u8	index = 0;
		
	for(index = 0; index < 3; index++){
		TestData[index] = 0;
	}
	
    if (argc<2)
    {
        printf("Wrong argument number!\r\n");
        return _FALSE;
    }
	TestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10); //timeout count
    TestData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);//Timer value Us
    TestData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);//loop count

	printf("%d,%d,%d\n",TestData[0],TestData[1],TestData[2]);
	printf("%s,%s,%s\n",argv[0],argv[1],argv[2]);
	TimerTestApp(TestData);

    return _TRUE;
	
}


u32
CmdWatchdogTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32  WDTestData[2];
    
    if (argc<2)
    {
        printf("Wrong argument number!\r\n");
        return _FALSE;
    }

	WDTestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10); // Mode
    WDTestData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10); // Timer value

    WatchdogTestApp(WDTestData);

    return _TRUE;
}

u32
CmdChangeCpuFreqTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32  TestData[1];

    printf("%s(%d)\n", __FUNCTION__, __LINE__);
    
    if (argc<1)
    {
        printf("Wrong argument number!\r\n");
        return _FALSE;
    }

	TestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10); // Mode
    printf("%s(%d): 0x%x\n", __FUNCTION__, __LINE__, TestData[0]);

    ChangeCpuFreqTestApp(TestData);

    return _TRUE;
}

