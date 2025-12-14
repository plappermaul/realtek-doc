

/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_gdma.c

Abstract:
	Define 98E test command

Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-08-19 Carl            Create.
--*/

#include <common.h>
#include <command.h>
#include <basic_types.h>
#include "cmd_gdma.h"

extern  VOID    GdmaTestApp(VOID *Data);


u32
CmdGdmaTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u8  InPutData[16];
    u32 *pTestLen, *pTestLoop;

	if(argc<2)
	{	printf("Wrong argument number!\r\n");
		printf("Usage :\n");
                printf("[0:Loop]\n[1:Length]\n[2:GdmaType]\n[3:MemoryType]\n[4:GdmaIndex]\n[5:Pattern]\n[6:AutoTest Enable]\n");
		return _FALSE;
	}

    memset(InPutData, 0x00, sizeof(InPutData));    // 20151123

    pTestLen = (u32*)&InPutData[8];
    pTestLoop = (u32*)&InPutData[12];

    //Verify Loop
    *pTestLoop = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);

    //Test Length
    *pTestLen = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

    //Gdma Type; Single/Block/LLP...
    InPutData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);
    if (argc == 3) {
        //Memory Test Type; SD to SD/SD to SF/SF to SF/ SF to SD
        InPutData[1] = 0;
        //Gdma Index; Gdma0/Gdma1
        InPutData[3] = 0;
        //Pattern
        InPutData[4] = 0;
        //AutoTest Enable
        InPutData[0] = 0;
    }
    else {
        //Memory Test Type; SD to SD (0)/SD to SF (1)/SF to SF (2)/ SF to SD (3) /Flash to SD (4)
        InPutData[1] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);
        //Gdma Index; Gdma0/Gdma1
        InPutData[3] = Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);
        //Pattern
        InPutData[4] = Strtoul((const u8*)(argv[5]), (u8 **)NULL, 10);
        //AutoTest Enable
        InPutData[0] = Strtoul((const u8*)(argv[6]), (u8 **)NULL, 10);

    }
#if 0
printf("pTestLoop = 0x%x\n", *pTestLoop);
printf("pTestLen = 0x%x\n", *pTestLen);
printf("GdmaType; Single/Block/LLP = 0x%x\n", InPutData[2] );
printf("GdmaIndex\t\t= 0x%x\n", InPutData[3] );
printf("MEMTYPE\t\t\t= 0x%x\n", InPutData[1] );
printf("Pattern\t\t\t= 0x%x\n", InPutData[4] );
printf("AutoTest\t\t= 0x%x\n", InPutData[0] );
#endif


#if 0	// Carl
    u32  i;
    for (i=0;i<16;i++) {
        printf("0x%02x\n",InPutData[i]);
    }
    //printf("\nCarl Test - skip GdmaTestApp!\n");
    printf("Flysky - coding...\n");
    //return _TRUE;
#endif

    GdmaTestApp(InPutData);

    return _TRUE;
}


