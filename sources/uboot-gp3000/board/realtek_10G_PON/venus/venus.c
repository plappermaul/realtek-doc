/*
 * (C) Copyright 2018 -
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <linux/compiler.h>
#include "psci.h"
#include <fdt_support.h>
#include <dwmmc.h>

DECLARE_GLOBAL_DATA_PTR;

/* BLOCK Reset Definitions */
#define RESET_OTPROM (1 << 29)
#define RESET_SD (1 << 28)
#define RESET_EAXI (1 << 27)
#define RESET_FBM (1 << 26)
#define RESET_LDMA (1 << 25)
#define RESET_RCRYPTO (1 << 24)
#define RESET_SADB (1 << 22)
#define RESET_RCPU1 (1 << 21)
#define RESET_RCPU0 (1 << 20)
#define RESET_PE1 (1 << 19)
#define RESET_PE0 (1 << 18)
#define RESET_RTC (1 << 17)
#define RESET_DMA (1 << 16)
#define RESET_PER (1 << 15)
#define RESET_FLASH (1 << 14)
#define RESET_GIC400 (1 << 10)
#define RESET_SATA (1 << 9)
#define RESET_PCIE2 (1 << 8)
#define RESET_PCIE1 (1 << 7)
#define RESET_PCIE0 (1 << 6)
#define RESET_TQM (1 << 5)
#define RESET_SDRAM (1 << 4)
#define RESET_L3FE (1 << 3)
#define RESET_L2TM (1 << 2)
#define RESET_L2FE (1 << 1)
#define RESET_NI (1 << 0)

int ca77xx_eth_initialize(bd_t *bis);

#ifdef CONFIG_RTK_LED_STYLE_GPIO
	void rtk_gpio_leds_init(void);
#endif

#ifdef CONFIG_CORTINA_LED
#	ifdef CONFIG_RTK_LED_STYLE_1
#		define RTK_POWER_LED 15
/* defined in Linux kernel dts file */
uint16_t led_blink_rate1 = 256;
uint16_t led_blink_rate2 = 512;

uint8_t led_cfg[CORTINA_LED_NUM][6] = {
  /* active_low/off_event/blink_event/on_event/port/blink_rate */
  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},

  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},
  // 8~11, LAN0~3
  {1, TRIGGER_NONE, TRIGGER_HW_TX | TRIGGER_HW_RX, TRIGGER_SW, 0, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_TX | TRIGGER_HW_RX, TRIGGER_SW, 1, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_TX | TRIGGER_HW_RX, TRIGGER_SW, 2, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_TX | TRIGGER_HW_RX, TRIGGER_SW, 3, BLINK_RATE1},

  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_NONE, TRIGGER_NONE, 6, BLINK_RATE1},
  // Power
  {1, TRIGGER_NONE, TRIGGER_SW, TRIGGER_NONE, 6, BLINK_RATE2},
};

uint8_t port2led[8][1] = {
  /* Rx/Tx */
  {8}, {9}, {10}, {11}, {0}, {0}, {0}, {0}};
#	else
/* defined in Linux kernel dts file */
uint16_t led_blink_rate1 = 256;
uint16_t led_blink_rate2 = 512;

uint8_t led_cfg[CORTINA_LED_NUM][6] = {
  /* active_low/off_event/blink_event/on_event/port/blink_rate */
  {1, TRIGGER_NONE, TRIGGER_HW_RX, TRIGGER_NONE, 4, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_RX, TRIGGER_NONE, 5, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_RX, TRIGGER_NONE, 6, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_RX, TRIGGER_NONE, 7, BLINK_RATE1},

  {1, TRIGGER_NONE, TRIGGER_HW_TX, TRIGGER_SW, 7, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_TX, TRIGGER_SW, 6, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_TX, TRIGGER_SW, 5, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_TX, TRIGGER_SW, 4, BLINK_RATE1},

  {1, TRIGGER_NONE, TRIGGER_HW_RX, TRIGGER_NONE, 0, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_RX, TRIGGER_NONE, 1, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_RX, TRIGGER_NONE, 2, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_RX, TRIGGER_NONE, 3, BLINK_RATE1},

  {1, TRIGGER_NONE, TRIGGER_HW_TX, TRIGGER_SW, 3, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_TX, TRIGGER_SW, 2, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_TX, TRIGGER_SW, 1, BLINK_RATE1},
  {1, TRIGGER_NONE, TRIGGER_HW_TX, TRIGGER_SW, 0, BLINK_RATE1},
};

uint8_t port2led[8][2] = {
  /* Rx/Tx */
  {8, 15}, {9, 14}, {10, 13}, {11, 12}, {0, 7}, {1, 6}, {2, 5}, {3, 4}};
#	endif // CONFIG_RTK_LED_STYLE_1
#endif

static noinline int invoke_psci_fn_smc(u64 function_id, u64 arg0, u64 arg1,
                                       u64 arg2) {
#ifdef CONFIG_ARM64
	asm volatile(
	  "mov x0, %0\n"
	  "mov x1, %1\n"
	  "mov x2, %2\n"
	  "mov x3, %3\n"
	  "smc	#0\n"
	  : "+r"(function_id)
	  : "r"(arg0), "r"(arg1), "r"(arg2));
#else
	asm volatile(
	  ".arch_extension sec\n"
	  "mov r0, %0\n"
	  "mov r1, %1\n"
	  "mov r2, %2\n"
	  "mov r3, %3\n"
	  "smc	#0\n"
	  : "+r"(function_id)
	  : "r"(arg0), "r"(arg1), "r"(arg2));

#endif

	return function_id;
}

void ca_img_wtd_enable(void) {
	/*Reset by Watchdog*/
	return;
}

int board_init(void) {
#ifdef CONFIG_CORTINA_LED
	int i;
#endif

	/* Enable timer */
	writel(1, CONFIG_SYS_TIMER_BASE);


#ifdef CONFIG_CORTINA_LED
	ca77xx_leds_init(GLOBAL_LED_CONTROL, led_blink_rate1, led_blink_rate2);

	for (i = 0; i < CORTINA_LED_NUM; i++)
		ca77xx_led_config(i, led_cfg[i][0], led_cfg[i][1], led_cfg[i][2],
		                  led_cfg[i][3], led_cfg[i][4], led_cfg[i][5]);

#	ifdef CONFIG_RTK_LED_STYLE_1
	__led_set(1 << RTK_POWER_LED, STATUS_LED_ON);
#	endif
#endif // CONFIG_CORTINA_LED
#ifdef CONFIG_RTK_LED_STYLE_GPIO
	rtk_gpio_leds_init();
#endif

	return 0;
}

int dram_init(void) {
	unsigned int ddr_size, tddr_addr, i = 0;
	ddr_size = readl(0x111100c);
    tddr_addr = readl(0x1111014);

    /* Special configuration for 128MB dram chip */
    if ((tddr_addr/0x100000) < 240) {
        gd->bd->bi_dram[i].start = PHYS_SDRAM_1;
        gd->bd->bi_dram[i].size = (tddr_addr/0x100000) * 0x100000;
        gd->ram_size = (tddr_addr/0x100000) * 0x100000;
        i++;
        gd->bd->bi_dram[i].start = 0;
        gd->bd->bi_dram[i].size = 0;

        return 0;
    }



	/* DRAM size should be filled by DDR init code(BL2).\
	 * Hard code ddr_size here if no BL2!
	 */
	if (ddr_size >
	    240) { /* tail of 256M are reserved for secure firmware(BL31/BL32) */
		gd->bd->bi_dram[i].start = PHYS_SDRAM_1;
		gd->bd->bi_dram[i].size = 240 * 0x100000;
		gd->ram_size = 240 * 0x100000;
		i++;
	}

	/* segment #2 start from offset of 256M */
	if (ddr_size > 256) {
		gd->bd->bi_dram[i].start = PHYS_SDRAM_1 + SZ_256M;
		gd->bd->bi_dram[i].size =
		  ((ddr_size <= 2048) ? (ddr_size - 256) : (2048 - 256)) * 0x100000;
		gd->ram_size +=
		  ((ddr_size <= 2048) ? (ddr_size - 256) : (2048 - 256)) * 0x100000;
		i++;
	}

	/* Segment #3:
	 * Tail of base address 0 are for PCI and IO port.
	 * So we must use DDR alias base to add DDR bank.
	 */
	if (ddr_size > 2048) {
		gd->bd->bi_dram[i].start = DDR_ALIAS_BASE + SZ_2G;
		gd->bd->bi_dram[i].size = (ddr_size - 2048) * 0x100000;
		gd->ram_size += (ddr_size - 2048) * 0x100000;
		i++;
	}

	gd->bd->bi_dram[i].start = 0;
	gd->bd->bi_dram[i].size = 0;

	return 0;
}

int dram_init_banksize(void) {
#if defined(CONFIG_REALTEK_10G_PON) && defined(CONFIG_NR_DRAM_BANKS)
    unsigned int bit_width;
    unsigned int dram_type;
    unsigned int ddr_size;
    unsigned int ddr_freq;
    unsigned int tddr_addr;

    bit_width = readl(0x1111004)*8;
    dram_type = readl(0x1111008);
    ddr_size  = readl(0x111100c);
    ddr_freq  = readl(0x1111010);
    tddr_addr = readl(0x1111014);

    printf("\b\b\b\b\b\b\bDDR%d-%d, %dBit mode, tdram @%dMiB, total ", dram_type, ddr_freq * 2, bit_width, tddr_addr/0x100000);

	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = ddr_size * 0x100000;
	// clear bi_dram 1,2,
	gd->bd->bi_dram[1].start = 0;
	gd->bd->bi_dram[1].size = 0;
	gd->bd->bi_dram[2].start = 0;
	gd->bd->bi_dram[2].size = 0;
	//	printf("[dram_init_banksize]0:%d, 1:%d, 2:%d\n", gd->bd->bi_dram[0].size,
	// gd->bd->bi_dram[1].size,  gd->bd->bi_dram[2].size);

#else
	dram_init();
#endif
	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
extern uint64_t rtk_cortex_freq_get(void);

int show_board_info(void)
{
    printf("CortexA55: %d MHz\n",(uint32_t)(rtk_cortex_freq_get()/1000000));
    return 0;
}
#endif


/*
 * Board specific reset that is system reset.
 */
void reset_cpu(ulong addr) {
	invoke_psci_fn_smc(PSCI_0_2_FN_SYSTEM_RESET, 0, 0, 0);
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void) {
#	ifdef CONFIG_SEMIHOSTING
	/*
	 * Please refer to doc/README.semihosting for a more complete
	 * description.
	 *
	 * We require that the board include file defines these env variables:
	 * - kernel_name
	 * - kernel_addr_r
	 * - initrd_name
	 * - initrd_addr_r
	 * - fdt_name
	 * - fdt_addr_r
	 *
	 * For the "fdt chosen" startup macro, this code will then define:
	 * - initrd_end (based on initrd_addr_r plus actual initrd_size)
	 *
	 * We will then load the kernel, initrd, and fdt into the specified
	 * locations in memory in a similar way that the ATF fastmodel code
	 * uses semihosting calls to load other boot stages and u-boot itself.
	 */

	/* Env variable strings */
	char *kernel_name = getenv("kernel_name");
	char *kernel_addr_str = getenv("kernel_addr_r");
	char *initrd_name = getenv("initrd_name");
	char *initrd_addr_str = getenv("initrd_addr_r");
	char *fdt_name = getenv("fdt_name");
	char *fdt_addr_str = getenv("fdt_addr_r");
	char initrd_end_str[64];

	/* Actual addresses converted from env variables */
	void *kernel_addr_r;
	void *initrd_addr_r;
	void *fdt_addr_r;

	/* Actual initrd base and size */
	unsigned long initrd_base;
	unsigned long initrd_size;

	/* Space available */
	int avail;

	/* Make sure the environment variables needed are set */
	if (!(kernel_addr_str && initrd_addr_str && fdt_addr_str)) {
		printf("%s: Define {kernel/initrd/fdt}_addr_r\n", __func__);
		return -1;
	}
	if (!(kernel_name && initrd_name && fdt_name)) {
		printf("%s: Define {kernel/initrd/fdt}_name\n", __func__);
		return -1;
	}

	/* Get exact initrd_size */
	initrd_size = smh_len(initrd_name);
	if (initrd_size == -1) {
		printf("%s: Can't get file size for \'%s\'\n", __func__, initrd_name);
		return -1;
	}

	/* Set initrd_end */
	initrd_base = simple_strtoul(initrd_addr_str, NULL, 16);
	initrd_addr_r = (void *)initrd_base;
	sprintf(initrd_end_str, "0x%lx", initrd_base + initrd_size - 1);
	setenv("initrd_end", initrd_end_str);

	/* Load kernel to memory */
	fdt_addr_r = (void *)simple_strtoul(fdt_addr_str, NULL, 16);
	kernel_addr_r = (void *)simple_strtoul(kernel_addr_str, NULL, 16);

	/*
	 * The kernel must be lower in memory than fdt and loading the
	 * kernel must not trample the fdt or vice versa.
	 */
	avail = fdt_addr_r - kernel_addr_r;
	if (avail < 0) {
		printf("%s: fdt must be after kernel\n", __func__);
		return -1;
	}
	smh_load(kernel_name, kernel_addr_r, avail, 1);

	/* Load fdt to memory */
	smh_load(fdt_name, fdt_addr_r, 0x20000, 1);

	/* Load initrd to memory */
	smh_load(initrd_name, initrd_addr_r, initrd_size, 1);

#	endif /* CONFIG_SEMIHOSTING */
	return 0;
}
#endif /* CONFIG_BOARD_LATE_INIT */

/*
 * Board specific ethernet initialization routine.
 */
int board_eth_init(bd_t *bis) {
	int rc = 0;

	ca77xx_eth_initialize(bis);

	return rc;
}


#ifdef CONFIG_LAST_STAGE_INIT
int last_stage_init(void) {
	u32 val;

	val = readl(GLOBAL_BLOCK_RESET - GLOBAL_JTAG_ID + GLOBAL_BASE);
	val &= ~RESET_DMA;
	writel(val, GLOBAL_BLOCK_RESET - GLOBAL_JTAG_ID + GLOBAL_BASE);

	/* reduce output pclk ~3.7Hz to save power consumption */
	writel(0x000000FF, DMA_SEC_SSP_BAUDRATE_CTRL - DMA_SEC_SSP_ID + SSP_BASE);

	return 0;
}
#endif
