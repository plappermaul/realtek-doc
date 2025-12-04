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
#include <irq_func.h>
#include <dwmmc.h>
#include <command.h>
#ifdef CONFIG_MTD_CORTINA_SERIAL_NOR
#include <spi_flash.h>
#endif
#ifndef CONFIG_LAST_STAGE_INIT
#error CONFIG_LAST_STAGE_INIT should be enabled on TAURUS platform
#endif

DECLARE_GLOBAL_DATA_PTR;

#define CMD_BUF_SZ (256)

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

#define XRAM_BASE 0xF4500000
#define XRAM_SIZE 0x2000
static void *xram_buf;
void board_preboot_os(void)
{
	if (xram_buf)
	{
		uint32_t val, port;
		unsigned long reg;

		#define NI_HV_PT_RXMAC_CFG_BASE		0xf430a5c8
		#define NI_HV_PT_RXMAC_CFG_STRIDE	0x90

		for (port = 0; port < 6; port++) {
			reg = NI_HV_PT_RXMAC_CFG_BASE + (port * NI_HV_PT_RXMAC_CFG_STRIDE);
			val = readl(reg) & ~0x1;
			writel(val, reg);
		}
		memcpy((void *)XRAM_BASE, xram_buf, XRAM_SIZE);
	}
}

int board_init(void) {
#ifdef CONFIG_CORTINA_LED
	int i;
#endif

	/* Enable timer */
	writel(1, CONFIG_SYS_TIMER_BASE);

	xram_buf = malloc(XRAM_SIZE);
	if (xram_buf)
	{
		memcpy(xram_buf, (void *)XRAM_BASE, XRAM_SIZE);
	}

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

#ifdef CONFIG_MTD_CORTINA_SERIAL_NOR
    struct spi_flash *sflash __attribute__((unused));
    sflash = spi_flash_probe(0, 0, 0, 0);
#endif

#if !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
extern void ca77xx_eth_change_default(void);
	ca77xx_eth_change_default();
#endif

	return 0;
}

int dram_init(void) {
	unsigned int ddr_size, i = 0;
	ddr_size = readl(0x111100c);

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

	/* segment #2 start from offset of 256M
     * Only 2GB physical mapping area is useable for u-boot booting
     */
	if (ddr_size > 256) {
		gd->bd->bi_dram[i].start = PHYS_SDRAM_1 + SZ_256M;
		gd->bd->bi_dram[i].size  = ((ddr_size <= 2048) ? (ddr_size - 256) : (2048 - 256)) * 0x100000;
		gd->ram_size += gd->bd->bi_dram[i].size;
		i++;
	}

	/* Segment #3:
	 * Only 2GB physical mapping area is useable for u-boot booting
	 */
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
    bit_width = readl(0x1111004)*8;
    dram_type = readl(0x1111008);
    ddr_size  = readl(0x111100c);
    ddr_freq  = readl(0x1111010);

    printf("\b\b\b\b\b\b\b%s%4d: %d-bit mode, ", (dram_type == 0) ? "DDR3-" : "DDR4-", ddr_freq * 2, bit_width);

    gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
    gd->bd->bi_dram[0].size = ((ddr_size > 2048) ? 2048 : ddr_size) * 0x100000;

    if ((ddr_size > 2048) && (ddr_size <= 4096)) {
        gd->bd->bi_dram[1].start = DDR_ALIAS_BASE + SZ_2G;
        gd->bd->bi_dram[1].size = (ddr_size - 2048) * 0x100000;
    }

    gd->bd->bi_dram[2].start = 0;
    gd->bd->bi_dram[2].size = 0;
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
#define DDR_WARM_RESET (1)
#define DDR_COLD_RESET (0)

void ddr_reset_mode(uint32_t mode)
{
    /* Restore to DDR cold reset mode */
    GLOBAL_DDR_RESET_MODE_t reg_v;
    reg_v.wrd = readl(GLOBAL_DDR_RESET_MODE);
    reg_v.bf.ddr_warm_rst_mode = mode;
    writel(reg_v.wrd, GLOBAL_DDR_RESET_MODE);
}

void system_reset(void)
{
    invoke_psci_fn_smc(PSCI_0_2_FN_SYSTEM_RESET, 0, 0, 0);
}

void reset_cpu(ulong addr)
{
    /* Restore to DDR cold reset mode */
    ddr_reset_mode(DDR_COLD_RESET);

    /* Doing system reset*/
	system_reset();
}

static int do_reset_warm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    puts ("Reboot with DDR warm reset ...\n");

    udelay (50000);             /* wait 50 ms */

    disable_interrupts();

    /* Doing DDR warm reset */
    ddr_reset_mode(DDR_WARM_RESET);
    system_reset();

    /*NOTREACHED*/
    return 0;
}

U_BOOT_CMD(
    reset_warm, 1, 0,    do_reset_warm,
    "Perform RESET of the CPU with DDR warm reset",
    ""
);

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

#if 0 //#ifdef CONFIG_MMC
int board_mmc_init(bd_t *bis) {
	GLOBAL_PIN_MUX_t reg_v;
	GLOBAL_SD_DLL_CTRL_t g3_global_sd_dll_ctrl;
	struct dwmci_host *host = NULL;

	host = malloc(sizeof(struct dwmci_host));
	if (!host) {
		printf("dwmci_host malloc fail!\n");
		return 1;
	}

	g3_global_sd_dll_ctrl.wrd = readl(GLOBAL_SD_DLL_CTRL);

	/* turn on DLL0 and DLL1 bypass per HW documentation */
	g3_global_sd_dll_ctrl.bf.sd_dll_shiftClk0_bypass = 1;
	g3_global_sd_dll_ctrl.bf.sd_dll_shiftClk1_bypass = 1;

	/* clear non-functional bits per HW documentation */
	g3_global_sd_dll_ctrl.bf.sd_dll_reset = 0;
	g3_global_sd_dll_ctrl.bf.sd_dll_shiftClk0_phaseSel = 0;
	g3_global_sd_dll_ctrl.bf.sd_dll_shiftClk1_phaseSel = 0;
	g3_global_sd_dll_ctrl.bf.sd_phase_reset_override = 0;
	/* 50MH */
	g3_global_sd_dll_ctrl.bf.sd_clk_sel = 0;
	writel(g3_global_sd_dll_ctrl.wrd, GLOBAL_SD_DLL_CTRL);

	reg_v.wrd = readl(GLOBAL_PIN_MUX);
	reg_v.bf.iomux_sd_mmc_resetn = 1;
	reg_v.bf.iomux_sd_volt_reg = 1;
	writel(reg_v.wrd, GLOBAL_PIN_MUX);

	memset(host, 0, sizeof(struct dwmci_host));
	host->name = "Cortina Access G3 SOC";
	host->ioaddr = (void *)CA_DWMMC_BASE;
	host->buswidth = 4;
	host->dev_index = 0;
	host->bus_hz = 50000000;

	add_dwmci(host, host->bus_hz, 400000);

	return 0;
}
#endif

#ifdef CONFIG_LAST_STAGE_INIT
int last_stage_init(void) {
	char cmd_buf[CMD_BUF_SZ] = {0};

    /* BL2 in FIP0 will enable WDOG timeout to 23 seconds and FORCE_BOOT_STANDBY_BL3_FLAG
     * When boots up to here successfully, clear FORCE_BOOT_STANDBY_BL3_FLAG and disable WDOG rest
     * This is developed for [run upf] command
     */
	writel(0, GLOBAL_SCRATCH);
	if((readl(PER_WDT_DIV)==1000) && (readl(PER_WDT_LD)==23)) {
        writel(0, PER_WDT_CTRL);
    }

	/* set extSClkSel to use SCLK from input SCLK for no SCLK */
	writel(0x00200000, DMA_SEC_SSP_FRAME_CTRL0);

	/* Run env default setting */
	if (env_get_yesno("default_setting") != -1) {
		sprintf(cmd_buf, "run default_setting");
		run_command(cmd_buf, 0);
	}

	return 0;
}
#endif
