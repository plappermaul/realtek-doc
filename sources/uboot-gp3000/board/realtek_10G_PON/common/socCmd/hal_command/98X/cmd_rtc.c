

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
#include "cmd_rtc.h"

extern  VOID    RTCTestApp(VOID *Data);


u32
CmdRTCTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u8  InPutData[10];
    u8	index = 0;

    if (argc<1)
    {
	printf("Wrong argument number!\n");
        printf("0 Test Item,\n1 Loop,\n2 year,\n3 month,\n4 week,\n5 day,\n6 hour,\n7 minute,\n8 second,\n9 alarm type\n");
        return _FALSE;
    }

    for(index = 0; index < 10; index++){
        InPutData[index] = Strtoul((const u8*)(argv[index]), (u8 **)NULL, 10);
	/*printf("InPutData[%d] = 0x%X\n",index,InPutData[index]);*/
    }

    RTCTestApp(InPutData);

    return _TRUE;
}

