#include <preloader.h>
#include <pblr.h>

u32_t endian_swap(const u8_t *addr) {
	return ((*(addr+3) << 24) + (*(addr+2) << 16) +
	        (*(addr+1) <<  8) + (*(addr+0) <<  0));
}

extern u32_t pll_query_freq(u32_t);
void pblr_udelay(u32_t usecs) {
	u32_t current, start, bound;

	start = OTTO_CPU_CYCLE();

	/* Limite usecs to less or equal to 1 sec. */
	usecs = ((usecs > (1000*1000)) ? (1000*1000) : usecs);
	bound = usecs * pll_query_freq(PLL_DEV_CPU);

	do {
		current = OTTO_CPU_CYCLE();
		current -= start;
	} while(current < bound);

	return;
}

void bzero(void *buf, u32_t nbyte) {
    pblr_bzero(buf, nbyte);
}

__attribute__((weak)) void *
pblr_memcpy(u8_t *dest, const u8_t *src, u32_t n) {
	if (n == 0) return dest;
	do {
		*dest++ = *src++;
	} while (--n != 0);
	return dest;
}

