#include <soc.h>
#include <config.h>
#include <plr_pll_gen3.h>

pll_gen3_mhz_t pll_mhz = {
	.mode = PLL_MODE_BY_SW,
	.cpu = 500,
	.lx  = 200,
	.mem = 300,
};

const dram_info_t dram_info_in_spare[SOC_NUM_DRAM_SPARE] = {
	{
		#include <DRAM_GEN2_NANYA_NT5TU128M8GE-AC_838x_DEMO_300MHZ.h>
	}, {
		#include <DRAM_GEN2_NANYA_NT5TU128M8GE-AC_ZQ-STATIC_838x_DEMO_300MHZ.h>
	}
};

const plr_flash_info_t flash_info_in_spare[SOC_NUM_FLASH_SPARE] = {
	{
		#include <NOR_SPI_F_MXIC_MX25L25635F_32MB_4BCMD_DIO.h>
	}, {
		#include <NOR_SPI_F_MXIC_MX25L25635F_32MB_4BCMD_DIO.h>
	}
};

const spare_header_t spare_headers_in_flash[] = {
	{
		SST_DRAM,
		SOC_NUM_DRAM_SPARE,
		(void *)dram_info_in_spare
	},
	{
		SST_FLASH,
		SOC_NUM_FLASH_SPARE,
		(void *)flash_info_in_spare
	},
	{
		SST_END,
		0,
		0
	}
};

/* spare_headers comes form plr_spare.c. */
const soc_t soc_in_flash = {
	.header_ver  = SOC_HEADER_VERSION,
	.header_type = (FIHT_NORMAL |
	                ((OTTO_DRAM_GEN) << (FIHT_DRAMCTRL_SHIFT)) |
	                ((OTTO_PLL_GEN)  << (FIHT_PLL_SHIFT)) |
	                (OTTO_NAND_FLASH ? (FIHT_NAND|FIHT_PARALLEL) : 0) |
	                (OTTO_NOR_SPI_FLASH ? (FIHT_NOR|FIHT_SPI) : 0)),

	.spare_headers = (spare_header_p)spare_headers_in_flash,

	.flash_info = {
		#include <NOR_SPI_F_MXIC_MX25L12835F_DIO.h>
	},

	.layout = {
		.bootloader1_addr = 0x00009000,
		.bootloader2_addr = 0x00000000,
		.kernel1_addr = 0x00100000,
		.kernel2_addr = 0x00000000,
		.rootfs1_addr = 0x00500000,
		.rootfs2_addr = 0x00000000,
		.env_addr = SOC_CONFIG_ENV_OFFSET,
		.env_size = 0x00004000,
		.opt1_addr = 0x00000000,
		.opt2_addr = 0x00000000,
		.opt3_addr = 0x00000000,
		.opt4_addr = 0x00000000,
		.end_addr = 0x00a00000,
	},

	.dram_info = {
	#ifndef CONFIG_CUSTOMER_BOARD
		/* Realtek's default parameters of DRAM. */
		#include <DRAM_GEN2_SAMSUNG_K4B1G0846G_838x_DEMO_300MHZ.h>
	#else
		/* Customer's parameters of DRAM. */
		#include <DRAM_CUSTOM_MODEL.h>
	#endif
	},

	.peri_info = {
		.baudrate_divisor = 0x0 /*Disable preloader message*/
	},

	.mac_info = {
		.address = {0, 0, 0, 0, 0, 0}
	}
};

u32_t dram_model_select(void) {
	return 0;
}
