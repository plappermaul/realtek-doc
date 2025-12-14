// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

/*
 * Misc boot support
 */
#include <common.h>
#include <command.h>
#include <osc.h>

extern otto_soc_context_t otto_sc;
extern void flush_dcache_all(void);
extern void invalidate_icache_all(void);
extern void dcache_disable(void);

unsigned long do_boot_b2_exec(ulong (*entry)(unsigned long))
{
	printf ("otto_sc = 0x%lx\n", (unsigned long)&otto_sc);
	flush_dcache_all();
	invalidate_icache_all();
//	dcache_disable();
	return entry ((unsigned long)&otto_sc);
}

static int do_boot_b2(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong	addr, rc;
	int     rcode = 0;

	if (argc < 2)
		return CMD_RET_USAGE;

	addr = simple_strtoul(argv[1], NULL, 16);

	printf ("## Starting second bootloader at 0x%08lX ...\n", addr);

	rc = do_boot_b2_exec ((void *)addr);
	if (rc != 0) rcode = 1;

	printf ("## Starting second bootloader  terminated, rc = 0x%lX\n", rc);
	return rcode;
}

/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	boot_b2, 2, 1,	do_boot_b2,
	"boot second u-boot at address 'addr'",
	"addr \n    - start second u-boot at address 'addr'\n"
);



