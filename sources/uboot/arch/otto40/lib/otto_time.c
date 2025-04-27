#include <common.h>
#include <command.h>
#include <asm/otto_pll.h>
#include <asm/otto_timer.h>

#ifndef SECTION_TIME
#define SECTION_TIME
#endif

static uint _accumulate_time_ms;

#if (TIMER_STEP_PER_US && (TIMER_STEP_PER_US & (TIMER_STEP_PER_US - 1)))
	#error TIMER_STEP_PER_US is not power of 2
#endif

u32_t _otto_lx_timer_init(const u32_t lx_freq_mhz) {
	TCCTRL_T tc_buffer;
	u32_t res = 0;

	/* divisor to make desired steps */
	u32_t divisor = ((lx_freq_mhz + TIMER_STEP_PER_US - 1) / TIMER_STEP_PER_US);

	if (lx_freq_mhz % TIMER_STEP_PER_US) {
		/* printf("WW: TIMER_FREQ_MHZ (%d) is not divisible by TIMER_STEP_PER_US (%d)\n", */
		/*        TIMER_FREQ_MHZ, */
		/*        TIMER_STEP_PER_US); */
		res |= TI_RES_BAD_PRECISION;
	}

	if (divisor > (1 << 16)) {
		/* printf("WW: Timer divisor overflow\n"); */
		divisor = (1 << 16) - 1;
		res |= TI_RES_BAD_DIVISOR;
}

	tc_buffer.f.divfactor = divisor;
	tc_buffer.f.mode      = 1; /* timer mode */
	tc_buffer.f.en        = 0; /* disable timer/counter */
	TIMER_CTRL = tc_buffer.v;

	tc_buffer.f.en        = 1; /* enable timer/counter */
	TIMER_CTRL = tc_buffer.v;

	TIMER_DATA = 0x0fffffff;

	return res;
}

SECTION_TIME
void _otto_lx_timer_udelay(us_t us) {
	const u32_t max_step = us * TIMER_STEP_PER_US;
	u32_t start, now;

	now = start = TIMER_CNT;

	while (((now - start) & 0x0fffffff) < max_step) {
		now = TIMER_CNT;
	}
	return;
}

void _otto_lx_timer_mdelay(u32_t ms) {
	while (ms >= 1000) {
		_otto_lx_timer_udelay(1000000);
		ms -= 1000;
		}
	if (ms > 0) _otto_lx_timer_udelay(ms*1000);
	return;
}

u32_t _otto_lx_timer_get_timer(u32_t base, u32_t *ms_accumulator) {
	const u32_t now = TIMER_CNT;
	const u32_t step_per_ms = 1000 * TIMER_STEP_PER_US;
	u32_t _accumulate_time_ms=*ms_accumulator;
	u32_t d = (now - ((_accumulate_time_ms * step_per_ms) & 0x0fffffff)) & 0x0fffffff;
	u32_t c = d / step_per_ms;

	_accumulate_time_ms += c;
	*ms_accumulator = _accumulate_time_ms;
	return _accumulate_time_ms - base;
	}



int timer_init(void)
{
    u32_t res = _otto_lx_timer_init(TIMER_FREQ_MHZ);

//    if(res & TI_RES_BAD_PRECISION)
//        printf("WW: TIMER_FREQ_MHZ (%d) is not divisible by TIMER_STEP_PER_US (%d)\n",TIMER_FREQ_MHZ,TIMER_STEP_PER_US);
    if(res & TI_RES_BAD_DIVISOR)
        printf("WW: Timer divisor overflow\n");

    _accumulate_time_ms = 0;

    return 0;
}

ulong get_timer(ulong base) {
    return _otto_lx_timer_get_timer(base, &_accumulate_time_ms);
}


/* `sleep' implementation to override the one in U-Boot. */
int do_sleep (cmd_tbl_t *cmdtp,
              int flag __attribute__((unused)),
              int argc, char * const argv[]) {
	uint32_t delay;

	if (argc != 2) return cmd_usage(cmdtp);

	delay = simple_strtoul(argv[1], NULL, 10);

	while (delay--) {
		__udelay(1000000);
	}

	return 0;
}

U_BOOT_CMD(sleep ,    2,    1,     do_sleep,
	"delay execution for some time",
	"N\n"
	"    - delay execution for N seconds (N is _decimal_ !!!)"
);
