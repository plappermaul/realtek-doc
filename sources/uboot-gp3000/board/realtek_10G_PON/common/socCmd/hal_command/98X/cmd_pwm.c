
/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_pwm.c
	
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
#include "cmd_pwm.h"

extern  VOID    PWMTestApp(VOID *Data);

u32
CmdPWMTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
	
    u32  TestData[5];
	u8	index = 0;
		
	for(index = 0; index < 5; index++){
		TestData[index] = 0;
	}
	
    if (argc<5)
    {
        printf("Wrong argument number!\r\n");
        return _FALSE;
    }
	TestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10); //test item
    TestData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);//loop count
    TestData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);//timer select
	TestData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);//count down value
	TestData[4] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);//pwm select
	
	printf("%d,%d,%d,%d,%d\n",TestData[0],TestData[1],TestData[2],TestData[3],TestData[4]);
	printf("%s,%s,%s,%s,%s\n",argv[0],argv[1],argv[2],argv[3],argv[4]);
	
	PWMTestApp(TestData);

    return _TRUE;
	
}

