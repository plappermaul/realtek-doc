

/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	cmd_pcm.c
	
Abstract:
	 PCM test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-08-13 Jwsyu             Create.	
--*/

#include <common.h>
#include <command.h>
#include <basic_types.h>
#include "cmd_pcm.h"
#include "diag.h"

extern  VOID    PCMTestApp(VOID *Data);


u32
CmdPCMTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
#ifdef PCM_FREE_RTOS_TASK_LOOPBACK_TEST
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

    for (i=0;i<5;i++) {
        DBG_8195A("%d-0x%02x\n", i, InPutData[i]);
    }

    PCMTestApp(InPutData);


    return _TRUE;
}


