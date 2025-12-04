/*
 * Copyright (C) 2019 Realtek
 */
#include <common.h>
#include <command.h>

__weak int dram_memory_test (int argc, char * const argv[]) {return 0;}

int do_dram_memory_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
    ret = dram_memory_test(argc, argv);
    if(ret == -2) return CMD_RET_USAGE;

	return ret;
}

U_BOOT_CMD(
    mdram_test,   9,  1,  do_dram_memory_test,
	"RTK DRAM Memory Test",
	"[-r start_address test_length] [-l loops] [-c case_num]\n"
    "  -r: Test the assigned range\n"
    "  -l: Totol test loops\n"
    "  -c: Assign the specific test case\n"
);

