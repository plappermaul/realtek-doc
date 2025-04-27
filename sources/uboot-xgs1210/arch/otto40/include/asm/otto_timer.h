#ifndef __TIMER_H__
#define __TIMER_H__ 1

#include <soc.h>

#if defined(CONFIG_UNDER_UBOOT)
	#define _otto_lx_timer_udelay __udelay
	typedef unsigned long us_t;
#else
	typedef u32_t us_t;
#endif

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int divfactor:16; //0
	} f;
	unsigned int v;
} TCCTRL_T;

#define TIMER_DATA REG32(UDELAY_TIMER_BASE+0x0)
#define TIMER_CNT  REG32(UDELAY_TIMER_BASE+0x4)
#define TIMER_CTRL REG32(UDELAY_TIMER_BASE+0x8)

#define TI_RES_BAD_PRECISION (1)
#define TI_RES_BAD_DIVISOR   (2)

typedef void (udelay_t)(us_t us);
typedef void (mdelay_t)(u32_t ms);
typedef u32_t (get_timer_t)(u32_t base, u32_t *ms_accumulator);

extern udelay_t *_udelay;
extern mdelay_t *_mdelay;
extern get_timer_t *_get_timer;

u32_t _otto_lx_timer_init(const u32_t lx_freq_mhz);
void _otto_lx_timer_udelay(us_t us);
void _otto_lx_timer_mdelay(u32_t ms);
u32_t _otto_lx_timer_get_timer(u32_t base, u32_t *ms_accumulator);

extern u32_t util_ms_accumulator;

#endif
