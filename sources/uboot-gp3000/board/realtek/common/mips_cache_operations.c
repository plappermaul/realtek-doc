#include <common.h>
#include <cpu_func.h>
#include <asm/cacheops.h>

#include <asm/io.h>
#include <asm/mipsregs.h>
#include <asm/system.h>


int icache_status(void)
{
	unsigned int cca = read_c0_config() & CONF_CM_CMASK;
	return cca != CONF_CM_UNCACHED;
}

void icache_enable(void)
{
		sync();
# ifdef CONFIG_MIPS_CM
		change_c0_config(CONF_CM_CMASK, CONF_CM_CACHABLE_COW);
# else
		change_c0_config(CONF_CM_CMASK, CONF_CM_CACHABLE_NONCOHERENT);
# endif
		instruction_hazard_barrier();
}

void icache_disable(void)
{
	/* change CCA to uncached */
	change_c0_config(CONF_CM_CMASK, CONF_CM_UNCACHED);

	/* ensure the pipeline doesn't contain now-invalid instructions */
	instruction_hazard_barrier();
}

void flush_dcache_all(void){

	writeback_invalidate_dcache_all();
	sync();

}