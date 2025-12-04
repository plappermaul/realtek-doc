#ifndef _CROSS_ENV_H_
#define _CROSS_ENV_H_

#include <soc.h>
#include <asm/mipsregs.h>
#include <cg_dev_freq.h>
#ifdef CONFIG_CMD_NAND
#	include <spi_nand/spi_nand_struct.h>
#endif

#ifdef noinline
#	undef noinline
#endif

/* this is cpu-specific: for mips, leave them empty;
   for 5281, refer src/soc/cpu/rlx5281/cpu.h */
#define RETURN_STACK_SAVE(_bpctl_backup)
#define RETURN_STACK_RESTORE(_bpctl_backup)

#define DCACHE_LINE_SZ_B CONFIG_SYS_DCACHE_LINE_SIZE

#define NORSF_XREAD_EN 0

#define IDCACHE_FLUSH() do { \
		writeback_invalidate_dcache_all(); \
		invalidate_icache_all(); \
	} while (0)

#define inline_memcpy(x, y, l) memcpy(x, y, l)

#endif
