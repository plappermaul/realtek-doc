#include <common.h>

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
#if 0
        void (*f)(void) = (void *) 0xbfc00000;
        f();
#else
	    SYSTEM_RESET();
#endif
        fprintf(stderr, "*** reset failed ***\n");
        return 0;
}

#if 0
void flush_cache_all(void)
{
}
#endif
void flush_cache(unsigned long a, unsigned long b)
{
	__BIOS.dcache_writeback_invalidate_all();
}
void enable_interrupts(void)
{
}

int disable_interrupts(void)
{
        return 0;
}

