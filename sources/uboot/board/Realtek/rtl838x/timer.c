#include <common.h>
#include <asm/arch/bspchip.h>

static uint timestamp;

#define MHZ  1000000
#define read_t0() (MEM32_READ(BSP_TC0CNT) >> BSP_TCD_OFFSET)

int timer_init(void)
{	
	/* disable timer */
    MEM32_WRITE(BSP_TCCNR, 0); /* disable timer before setting CDBR */

    /* initialize timer registers */
    MEM32_WRITE(BSP_CDBR, (board_LX_freq_mhz()) << BSP_DIVF_OFFSET);
    MEM32_WRITE(BSP_TC0DATA, (0xFFFFFFF) << BSP_TCD_OFFSET);

    MEM32_WRITE(BSP_TCCNR, BSP_TC0EN | BSP_TC0MODE_TIMER);

	return 0;
}


ulong get_timer(ulong base) 
{	
	static uint cnt = 0, last_t0 = 0;
	uint now;
	
	now = read_t0();
	if (last_t0 != now) {
		if (now > last_t0)
			cnt += (now - last_t0);
		else
			cnt += (0x1000000 - last_t0) + now;
	}
	last_t0 = now;
	
	if (cnt > 1000) {
		timestamp += (cnt / 1000);
		cnt = cnt % 1000;
	}
	
	return timestamp - base;
}

void __udelay(unsigned long usec)
{
	uint tmo, now;
	int diff;
       uint32 val;

	now = read_t0();
	tmo = now + usec;

	val = MEM32_READ(BSP_TCIR);
        val |= BSP_TC0IP;
        MEM32_WRITE(BSP_TCIR, val);

	do {
		diff = tmo - (read_t0() + ((MEM32_READ(BSP_TCIR) & BSP_TC0IP) ? 0xfffffff : 0));
	} while (diff >= 0);
}
