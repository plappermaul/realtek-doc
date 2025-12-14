

/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_emmc.c
	
Abstract:
	Define 96F test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2015-01-12 Xu Junwei       Create.	
--*/

#include <common.h>
#include <command.h>
#include <basic_types.h>
#include "cmd_common.h"
#include "cmd_emmc.h"

extern  VOID    setup_mmu_emmc(VOID);
extern  VOID Start_emmc_int1(VOID);
extern  VOID Start_emmc_int2(VOID);
extern  VOID Start_emmc_int4(VOID);

#define REG32(reg)      (*(volatile u32 *)(reg))

u32
CmdEMMCTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32  EMMCTestData[6];
	u8	index = 0;
	
    for(index = 0; index < 6; index++){
		EMMCTestData[index] = 0;
	}
	EMMCTestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
//		printf(".idddidid..\n");
//int i,s; 8196f bus idle test 20150602
//	for (i=0;i<100;i++) {
//		s=REG32(0xb8015400);	
//		printf("%x", REG32(0xb8015400));
//		s=REG32(0xb8015404);	
//		printf("-%x\n", REG32(0xb8015404));
//	}


	if (EMMCTestData[0] == 0) {
		printf("setup mmu/tlb...\n");

		setup_mmu_emmc();
	} else if (EMMCTestData[0] == 1) {
		//printf("Start_sd_carddetect...\n");
		//Start_sd_carddetect();
		printf("not support card detect in emmc...\n");
	} else if (EMMCTestData[0] == 2) {
		printf("Start_emmc_int1...\n");
		Start_emmc_int1();
	} else if (EMMCTestData[0] == 3) {
		printf("Start_emmc_int2...\n");
		Start_emmc_int2();
	} else if (EMMCTestData[0] == 4) {
		printf("Start_emmc_int4...\n");
		Start_emmc_int4();
	}

    return _TRUE;
}


