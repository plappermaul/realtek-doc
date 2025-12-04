
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
#include <command.h>
#include <basic_types.h>
#include "cmd_uart.h"

extern  VOID    RuartTestApp(VOID *Data);

u32
CmdRuartTest(
    IN u16 argc,
    IN u8  *argv[]
)
{


    u8  index = 0;
    u8  TEST_LENGTH = 6;
    u32 TestData[TEST_LENGTH];
	
	for (index = 0; index < TEST_LENGTH; ++index) {
		 TestData[index] = 0;
	}
		
#if 1
    for (index = 0; index < argc; ++index) {
        TestData[index] = Strtoul((const u8*)(argv[index]), (u8 **)NULL, 10);
    }

    for (index = 0; index < argc; ++index) {
        printf("RUART TestData[%d] = %d\n", index, TestData[index]);
    }
#endif

	//TestData[0]=0; //test item
	//TestData[1]=1; //loopcount
    RuartTestApp(TestData);
    return _TRUE;
}


