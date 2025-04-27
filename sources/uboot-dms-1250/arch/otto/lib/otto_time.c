#include <common.h>
#include <command.h>
#include <asm/otto_pll.h>
#include <soc.h>
#include <pblr.h>

#define CYCLES_PER_MS (board_CPU_freq_mhz() * 1000)

static uint _accumulate_time_ms;

int timer_init(void) {
	_accumulate_time_ms = 0;
	return 0;
}

ulong get_timer(ulong base) {
	ulong now = OTTO_CPU_CYCLE();
	ulong cycle_count_per_ms = CYCLES_PER_MS;
	ulong d = now - _accumulate_time_ms * cycle_count_per_ms;
	ulong c = d / cycle_count_per_ms;

	_accumulate_time_ms += c;

	return _accumulate_time_ms - base;
}

void __udelay(unsigned long usec) {
	parameters._udelay(usec);
}

/* `sleep' implementation to override the one in U-Boot. */
int do_sleep (cmd_tbl_t *cmdtp,
              int flag __attribute__((unused)),
              int argc, char * const argv[]) {
	uint32_t delay;

	if (argc != 2) return cmd_usage(cmdtp);

	delay = simple_strtoul(argv[1], NULL, 10);

	while (delay--) {
		parameters._udelay(1000000);
	}

	return 0;
}

U_BOOT_CMD(
	sleep ,    2,    1,     do_sleep,
	"delay execution for some time",
	"N\n"
	"    - delay execution for N seconds (N is _decimal_ !!!)"
);
