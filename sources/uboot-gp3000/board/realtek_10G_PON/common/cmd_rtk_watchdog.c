/*
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Misc functions
 */
#include <common.h>
#include <command.h>
#include <watchdog.h>
extern void  hw_watchdog_settimeout(u32 timeout);
extern void hw_watchdog_init(void);
static int do_hw_watchdog_init(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 timeout;

	if (argc > 2)
		return CMD_RET_USAGE;
	if (argc == 2){
		timeout = simple_strtoul(argv[1], NULL, 10);
		hw_watchdog_settimeout(timeout);	
	}
	hw_watchdog_init();

	return 0;
}

U_BOOT_CMD(
	watchdog ,    2,    1,     do_hw_watchdog_init,
	"watchdog: reboot system after some time(default value: 30 Sec)",
	"N\n"
	"    - Reboot system after N seconds (N is _decimal_ !!!), if nobody clear it."
);

