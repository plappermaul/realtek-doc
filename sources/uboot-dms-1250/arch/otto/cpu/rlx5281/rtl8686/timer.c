#include <common.h>
#include <asm/arch/bspchip.h>

extern unsigned int board_LX_freq_hz(void);

static uint timestamp;

#define MHZ  1000000
#define read_t0() (REG32(BSP_TC0CNT) >> BSP_TCD_OFFSET)


uint rtl8676_get_busfreq(void) 
{
#if 0
	uint sys_clock;
	unsigned short chip_id, chip_ver;
		
	chip_id  = REG32(CHIP_ID_REG) >> CHIP_ID_offset;
	chip_ver = REG32(CHIP_ID_REG) & 0xFFFF;
	switch(chip_id) {
	case 0x0412:
	case 0x6239:
		sys_clock = ((((REG32(BSP_SCCR) & 0x01F00)>>8)+2)*10) * MHZ;
		break;
	default:
		while (1);
	}
	return sys_clock;
#endif
	return board_LX_freq_hz();


}

#if 0 /* Use the one in otto_time.c */
int timer_init(void)
{	
	/* disable timer */
    REG32(BSP_TCCNR) = 0; /* disable timer before setting CDBR */

    /* initialize timer registers */
    REG32(BSP_CDBR)=(rtl8676_get_busfreq()/MHZ) << BSP_DIVF_OFFSET;
    REG32(BSP_TC0DATA) = (0xFFFFFFF) << BSP_TCD_OFFSET;
		
    REG32(BSP_TCCNR) = BSP_TC0EN | BSP_TC0MODE_TIMER;
	
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
	
	static int print_cnt = 10;
		
	now = read_t0();	
	tmo = now + usec;
	
	REG32(BSP_TCIR) |= BSP_TC0IP; /* clear ISR */
		
	do {
		diff = tmo - (read_t0() + ((REG32(BSP_TCIR) & BSP_TC0IP) ? 0xfffffff : 0));
	} while (diff >= 0);	
}
#endif

