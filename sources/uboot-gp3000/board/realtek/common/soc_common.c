#include <soc.h>

DECLARE_GLOBAL_DATA_PTR;

static uint32_t
dram_psize_b(void) {
	const uint8_t BNKCNTv[] = {1, 2, 3};
	const uint8_t BUSWIDv[] = {0, 1, 2};
	const uint8_t ROWCNTv[] = {11, 12, 13, 14, 15, 16};
	const uint8_t COLCNTv[] = {8, 9, 10, 11, 12};

	const uint32_t dcr = REG32(0xb8001004);
	uint32_t size = 1 << (BNKCNTv[(dcr >> 28) & 0x3] +
	                      BUSWIDv[(dcr >> 24) & 0x3] +
	                      ROWCNTv[(dcr >> 20) & 0xF] +
	                      COLCNTv[(dcr >> 16) & 0xF]);
	return size;
}

static uint32_t
dram_vsize_b(void) {
	const uint32_t max_dram_vsize_b = 256*1024*1024;
    printf("Physical %d MiB, uboot useable size ",dram_psize_b()/(1024*1024));
	return (dram_psize_b() > max_dram_vsize_b) ? max_dram_vsize_b : dram_psize_b();
}

__weak int dram_init(void) {
	/* DRAM is setup by Preloader. */
	gd->ram_size = dram_vsize_b();

	return 0;
}

void _machine_restart(void) {
	REG32(0xb8003268) = 0x80000000;
	fprintf(stderr, "*** triggering WDT reset ***\n");

	while (1);
	return;
}

__weak int dram_init_banksize(void)
{
#if defined(CONFIG_NR_DRAM_BANKS) && defined(CONFIG_SYS_SDRAM_BASE)
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size  = get_effective_memsize();

	if (gd->ram_size > CONFIG_MAX_MEM_MAPPED &&
	    CONFIG_NR_DRAM_BANKS > 1) {
		gd->bd->bi_dram[1].start = CONFIG_SYS_SDRAM_BASE+gd->bd->bi_dram[0].size;
		gd->bd->bi_dram[1].size  = gd->ram_size - gd->bd->bi_dram[0].size;
	}
#endif
        return 0;
}
