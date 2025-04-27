#include <preloader.h>
#include <soc.h>
#include <pblr.h>
#include <plr_spare.h>

#if (OTTO_PLL_GEN == 1)
	#include <plr_pll_gen1.h>
	#define pll_set_from_mhz pll_gen1_set_from_mhz
	typedef pll_gen1_mhz_t   pll_mhz_t;
#elif (OTTO_PLL_GEN == 2)
	#include <plr_pll_gen2.h>
	#define pll_set_from_mhz pll_gen2_set_from_mhz
	typedef pll_gen2_mhz_t   pll_mhz_t;
#elif (OTTO_PLL_GEN == 3)
	#include <plr_pll_gen3.h>
	#define pll_set_from_mhz pll_gen3_set_from_mhz
	typedef pll_gen3_mhz_t   pll_mhz_t;
#else
	#define pll_set_from_mhz pll_unknown_set_from_mhz
	typedef struct {}        pll_mhz_t;
#endif

void console_init(void);
void dram_setup(void);
void pll_setup(void);
void board_init_f(void);

void writeback_invalidate_dcache_all(void);
void invalidate_icache_all(void);

extern const soc_t soc_in_flash;
extern pll_mhz_t pll_mhz;

static inline void parameters_setup(void) {
	memcpy((void *)(&parameters),
	       (void *)(&soc_in_flash),
	       sizeof(soc_t));

	parameters.soc_id  = *((volatile u32_t *)0xb80010fc) >> 12;
	parameters.bond_id = 0xDEADDEAD;

	parameters._nor_spi_erase = (void *)0xDEADDEAD;
	parameters._nor_spi_read  = (void *)0xDEADDEAD;
	parameters._nor_spi_write = (void *)0xDEADDEAD;

	parameters.dram_init_result  = INI_RES_UNINIT;
	parameters.flash_init_result = INI_RES_UNINIT;

	parameters._pblr_printf = (void *)0xDEADDEAD;
	parameters._udelay = (void *)0xDEADDEAD;

	parameters._uart_init = (void *)0xDEADDEAD;
	parameters._uart_putc = (void *)0xDEADDEAD;
	parameters._uart_getc = (void *)0xDEADDEAD;
	parameters._uart_tstc = (void *)0xDEADDEAD;
	parameters._uart_ctrl = (void *)0xDEADDEAD;

	parameters._dcache_writeback_invalidate_all = &writeback_invalidate_dcache_all;
	parameters._icache_invalidate_all           = &invalidate_icache_all;

	return;
}

extern void uboot_start;
extern void uboot_size;
extern void preloader_sram_start;
extern void preloader_sram_size;
extern void preloader_bss_start;
extern void preloader_bss_size;

const u32_t c_sp_pos = STACK1_BASE;

void c_start(void) {
	pll_result_t res;

	pblr_bzero(&preloader_bss_start, (u32_t)&preloader_bss_size);

	parameters_setup();

	console_init();

	printf("II: Copied Preloader data from %p(%dB) to %p.\n",
	       &preloader_sram_start,
	       (u32_t)&preloader_sram_size,
	       SRAM_BASE);
	printf("II: Cleared Preloader BSS section at %p(%dB).\n",
	       &preloader_bss_start,
	       (u32_t)&preloader_bss_size);

	res = pll_set_from_mhz((pll_info_t *)&parameters.soc.pll_info,
	                       &pll_mhz);

	if (res != PLL_RES_OK) {
		if (res == PLL_RES_FREQ_OUT_OF_RANGE) {
			printf("WW: Given PLL frequency is not supported and tolerated.\n");
		} else {
			printf("EE: pll_set_from_mhz fails: %d\n", res);
		}
	}

	pll_setup();

#if SOC_NUM_DRAM_SPARE > 0
#define DRAMI (parameters.soc.dram_info)
	u32_t sel, di_num;
	dram_info_t *di;

	sel = dram_model_select();
	di = get_dram_spare(&di_num);
	if ((sel > 0) && (sel <= di_num)) {
		di = get_dram_spare(&di_num);
		di += (sel - 1);
		memcpy((void *)&DRAMI, (void *)di, sizeof(dram_info_t));
	}
	dram_setup();

	if (sel > di_num) {
		printf("EE: Bad DRAM model #%d from dram_model_select()."
		       " Only 0 ~ %d are configured in soc.tcl.\n",
		       sel, di_num);
		while (1);
	}
	printf("II: Selected DRAM model #%d.\n", sel);
#else
	dram_setup();
#endif

	printf("II: Copying U-Boot from %p(%dB) to %p... ",
	       (void *)&uboot_start,
	       (u32_t)&uboot_size,
	       (void *)CONFIG_SYS_TEXT_BASE);
	memcpy((void *)(CONFIG_SYS_TEXT_BASE),
	       (void *)&uboot_start,
	       (u32_t)&uboot_size);
	printf("OK\n");

	/* Note: flash driver must be initialized in U-Boot flow for .BSS. */

	return;
}
