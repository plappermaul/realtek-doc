

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
#include "cmd_i2c.h"

extern  VOID    I2CTestApp(VOID *Data);


u32
CmdI2CTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32  I2CTestData[6];
	u8	index = 0;
	
    for(index = 0; index < 6; index++){
		I2CTestData[index] = 0;
	}
    if (argc<3)
    {
        printf("Wrong argument number!\r\n");
        return _FALSE;
    }
#if 0	
		I2CTestData[0] = 0;
		I2CTestData[1] = 1;
		I2CTestData[2] = 1;
		I2CTestData[3] = 0;
		I2CTestData[4] = 1;
		I2CTestData[5] = 0;
#endif	
#if 1
		I2CTestData[0] = 0 ; //Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);//VeriProcessCmd
		I2CTestData[1] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);//VeriItem
		I2CTestData[2] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);//VeriLoop
		I2CTestData[3] = 0; //Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);//VeriMaster
		I2CTestData[4] = 1; //Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);//VeriSlave
		I2CTestData[5] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);//MtrRW
#endif

    I2CTestApp(I2CTestData);

    return _TRUE;
}
u32
CmdRTKI2CTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32  I2CTestData[6];
	u8	index = 0;
		
	for(index = 0; index < 6; index++){
		I2CTestData[index] = 0;
	}
	
    if (argc<2)
    {
        printf("Wrong argument number!\r\n");
        return _FALSE;
    }
	
	printf("argc=%d\n",argc);
	

#if 1
    
    I2CTestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);//VeriItem
    I2CTestData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);//VeriLoop
    I2CTestData[2] = 0; //Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10);//VeriMaster
    I2CTestData[3] = 1; //Strtoul((const u8*)(argv[4]), (u8 **)NULL, 10);//VeriSlave
    I2CTestData[4] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10);//MtrRW
#endif
    //RTKI2CTestProc(I2CTestData);

    return _TRUE;
}


