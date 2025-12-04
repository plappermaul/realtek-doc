

/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_sd30.c
	
Abstract:
	Define 96F test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-12-01 Xu Junwei       Create.	
--*/

#include <common.h>
#include <command.h>
#include <basic_types.h>
#include "cmd_common.h"
#include "cmd_sd30.h"

#include <asm/aarch64_sysreg_a55_tools.h>

extern  VOID    setup_mmu_sd30(VOID);
extern  VOID Start_sd_carddetect(VOID);
extern  VOID Start_sd_int1(VOID);
extern  VOID Start_sd_int2(VOID);
extern  VOID Start_sd_int4(VOID);

u32
CmdSD30Test(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32  SD30TestData[6];
	u8	index = 0;
	u64 i;
	u32 start_count, current_count;
	u32 i32;
	
    for(index = 0; index < 6; index++){
		SD30TestData[index] = 0;
	}
	SD30TestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);

	if (SD30TestData[0] == 0) {
		printf("setup mmu/tlb...\n");

		setup_mmu_sd30();
	} else if (SD30TestData[0] == 1) {
		printf("Start_sd_carddetect...\n");
		Start_sd_carddetect();
	} else if (SD30TestData[0] == 2) {
		printf("Start_sd_int1...\n");
		Start_sd_int1();
	} else if (SD30TestData[0] == 3) {
		printf("Start_sd_int2...\n");
		Start_sd_int2();
	} else if (SD30TestData[0] == 4) {
		printf("Start_sd_int4...\n");
		Start_sd_int4();
	} else if (SD30TestData[0] == 5) {
		printf("a55 aarch64 pmu test\n");
		i = read_sysreg(PMCR_EL0) | PMCR_EL0_P|PMCR_EL0_C;
		write_sysreg(i, PMCR_EL0);
		i32 = read_sysreg(PMCR_EL0);
		printf("a55 aarch64 pmu test 33, pmcr_el0=%x\n", i32);
		i = PMCR_EL0_E|read_sysreg(PMCR_EL0);
		write_sysreg(i, PMCR_EL0);
		start_count = read_sysreg(PMCCNTR_EL0);
        write_sysreg(PMCNTENSET_EL0_C, PMCNTENSET_EL0);
		i32 = read_sysreg(PMCR_EL0);
		printf("a55 aarch64 pmu test 55, PMCR_EL0=%x\n", i32);
		i32 = read_sysreg(PMCNTENSET_EL0);
		printf("a55 aarch64 pmu test 66, PMCNTENSET_EL0=%x\n", i32);
		current_count = read_sysreg(PMCCNTR_EL0);
       	printf("pmu(%x, %x)\n", start_count, current_count);
		//asic use 2000000000 cycle, 0x7fffffff=2147483647
		i = read_sysreg(PMCCNTR_EL0);
		while ( i < 500000000 ) {
			i = read_sysreg(PMCCNTR_EL0);
		}
		printf("pmu delay 100000000 end\n");
	}

    return _TRUE;
}


