/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <stdio_dev.h>
#include <version.h>
#include <net.h>
#include <environment.h>
#include <nand.h>
#include <soc.h>
#include <asm/otto_pll.h>
#include <osc.h>
#include <asm/symb_define.h>
#include "rtk_flash_common.h"


DECLARE_GLOBAL_DATA_PTR;

extern int timer_init(void);
extern void spi_flash_init(void);
extern int incaip_set_cpuclk(void);
extern void set_exception_base(u32_t);
extern void spi_nand_init(void);


extern ulong uboot_end_data;
extern ulong uboot_end;
extern ulong load_addr;

/* software patch */
typedef void (sw_patch_t) (void);
/* Defined in preloader.lds, points to SW-patch section. */

extern sw_patch_t *LS_bootm_stack;
extern sw_patch_t *LS_sw_patch_start;


ulong monitor_flash_len;

static inline void soc_post_init(void);

static char *failed = "*** failed ***\n";

/*
 * mips_io_port_base is the begin of the address space to which x86 style
 * I/O ports are mapped.
 */
unsigned long mips_io_port_base = -1;

int __board_early_init_f(void)
{
	/*
	 * Nothing to do in this dummy implementation
	 */
	return 0;
}
int board_early_init_f(void)
	__attribute__((weak, alias("__board_early_init_f")));

static int init_func_ram(void)
{
#ifdef	CONFIG_BOARD_TYPES
	int board_type = gd->board_type;
#else
	int board_type = 0;	/* use dummy arg */
#endif

	gd->ram_size = initdram(board_type);
	if (gd->ram_size > 0) {
		puts("DRAM:  ");
		print_size(gd->ram_size, "\n");
		return 0;
	}
	puts(failed);
	return 1;
}

static int display_banner(void)
{
	printf("\n\n%s\n\n", version_string);
	return 0;
}

#ifndef CONFIG_SYS_NO_FLASH
static void display_flash_config(ulong size)
{
	puts("Flash: ");
	print_size(size, "\n");
}
#endif

#if defined(OTTO21_PORT_TEMP)
static int init_baudrate(void)
{
#define	N_BAUDRATES (sizeof(baudrate_table) / sizeof(baudrate_table[0]))
	const unsigned int baudrate_table[] = CONFIG_SYS_BAUDRATE_TABLE;
	unsigned int br_from_divisor;
	unsigned int diff, prev_diff = 0xFFFFFFFF;
	int i;

	if (parameters.soc.peri_info.baudrate_divisor == 0x0) {
#ifdef CONFIG_FOLLOW_PLR_TO_DIS_UART
		gd->baudrate = 0;
#else
		gd->baudrate = getenv_ulong("baudrate", 10, CONFIG_BAUDRATE);
		parameters._uart_init(0); /* _uart_init is assign_uart_fp() to recover _uart function pointers */
#endif /* #ifdef FOLLOW_PLR_TO_DIS_UART */
	} else {
		br_from_divisor = (CONFIG_SYS_HZ /
		                   ((16 * parameters.soc.peri_info.baudrate_divisor) -
		                    (16 * CONFIG_UART_DIVISOR_MOD) -
		                    8));

		for (i=0; i<N_BAUDRATES; i++) {
			if (baudrate_table[i] > br_from_divisor) {
				diff = baudrate_table[i] - br_from_divisor;
			} else {
				diff = br_from_divisor - baudrate_table[i];
			}

			if (diff > prev_diff) {
				break;
			} else {
				prev_diff = diff;
			}
		}
		br_from_divisor = baudrate_table[i-1];
		gd->baudrate = getenv_ulong("baudrate", 10, br_from_divisor);
	}

	return 0;
}
#endif //defined(OTTO21_PORT_TEMP)

/*
 * Breath some life into the board...
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependend #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t)(void);

init_fnc_t *init_sequence[] = {
	board_early_init_f,
	timer_init,
	env_init,		/* initialize environment */
	//init_baudrate,	/* initialize baudrate settings */
	//serial_init,		/* serial communications setup */
	//console_init_f,
	display_banner,		/* say that we are here */
	checkboard,
	init_func_ram,
	NULL,
};

otto_soc_context_t otto_sc;

void board_init_f(ulong bootflag)
{
	gd_t gd_data, *id;
	bd_t *bd;
	init_fnc_t **init_fnc_ptr;

	/* set .bss to zero */
	memset(&uboot_end_data, 0, ((void *)&uboot_end) - ((void *)&uboot_end_data));

    /* get otto soc context from argument, this must be done after bss init */
    memcpy(&otto_sc, (otto_soc_context_t *)bootflag, sizeof(otto_soc_context_t));

	/* Pointer is writable since we allocated a register for it.
	 */
	gd = &gd_data;
	/* compiler optimization barrier needed for GCC >= 3.4 */
	__asm__ __volatile__("" : : : "memory");

	memset((void *)gd, 0, sizeof(gd_t));

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0)
			hang();
	}

	mem_malloc_init((ulong)__builtin_alloca(TOTAL_MALLOC_LEN), TOTAL_MALLOC_LEN);

	bd = (bd_t *)__builtin_alloca(sizeof(bd_t));
	gd->bd = bd;

	id = (gd_t *)__builtin_alloca(sizeof(gd_t));

	bd->bi_boot_params = (unsigned long)__builtin_alloca(CONFIG_SYS_BOOTPARAMS_LEN);

	/*
	 * Save local variables to board info struct
	 */
	bd->bi_memstart	= CONFIG_SYS_SDRAM_BASE;	/* start of DRAM */
	bd->bi_memsize	= gd->ram_size;		/* size of DRAM in bytes */
	bd->bi_baudrate	= gd->baudrate;		/* Console Baudrate */

	memcpy(id, (void *)gd, sizeof(gd_t));

	/* Obtain kernel address from preloader. */
#ifdef CONFIG_CMD_SF
	load_addr = 0x80000000;//parameters.soc.layout.kernel1_addr; FIXME
	load_addr += CONFIG_SYS_FLASH_BASE;
#endif
	__asm__ __volatile__("" : : : "memory");

	board_init_r(id, CONFIG_SYS_TEXT_BASE);

	/* NOTREACHED - relocate_code() does not return */
}

/*
 * This is the next part if the initialization sequence: we are now
 * running from RAM and have a "normal" C environment, i. e. global
 * data can be written, BSS has been cleared, the stack size in not
 * that critical any more, etc.
 */

void board_init_r(gd_t *id, ulong dest_addr)
{
#ifndef CONFIG_SYS_NO_FLASH
	ulong size;
#endif
	extern void malloc_bin_reloc(void);
#ifndef CONFIG_ENV_IS_NOWHERE
	extern char *env_name_spec;
#endif

	gd = id;
	gd->flags |= GD_FLG_RELOC;	/* tell others: relocation done */

	debug("Now running in RAM - U-Boot at: %08lx\n", dest_addr);

	gd->reloc_off = 0;

	monitor_flash_len = (ulong)&uboot_end_data - dest_addr;

#if defined(CONFIG_NEEDS_MANUAL_RELOC)
	/*
	 * We have to relocate the command table manually
	 */
	fixup_cmdtable(&__u_boot_cmd_start,
		(ulong)(&__u_boot_cmd_end - &__u_boot_cmd_start));
#endif /* defined(CONFIG_NEEDS_MANUAL_RELOC) */

	/* there are some other pointer constants we must deal with */
#ifndef CONFIG_ENV_IS_NOWHERE
	env_name_spec += gd->reloc_off;
#endif

	/* Issue software patch: level = 1~n , 0 for bootm start */
//	printf("start software patch\n");
	
	sw_patch_t **sw_patch = &LS_sw_patch_start;
	while (sw_patch!=&LS_bootm_stack) {
	    (*sw_patch)();
	    ++sw_patch;
	}

	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;)
		main_loop();

	/* NOTREACHED - no way out of command loop except booting */
}

static inline void soc_post_init(void) {
	/* The `baudrate' env was originally set by
	   CONFIG_BAUDRATE. Since we removed CONFIG_BAUDRATE, we
	   set baudrate env manually. */
	if (getenv_ulong("baudrate", 10, 0xFFFFFFFF) == 0xFFFFFFFF) {
		setenv_ulong("baudrate", gd->baudrate);
	}

#ifdef CONFIG_CMD_SF
	const char sf_probe_cmd[] = {"sf probe 0"};

	/* Issue `sf probe 0' before entering CLI. Since our dirver
	   automatically switches to next CS when access across CS
	   boundary, it is lousy to do `sf probe 0' before other
	   `sf' commands can be used. */
	run_command(sf_probe_cmd, 0);
#endif
	return;
}

void hang(void)
{
	puts("### ERROR ### Please RESET the board ###\n");
	for (;;)
		;
}


/*flash init , level 1*/
void swp_flash_init(void){
#ifndef CONFIG_SYS_NO_FLASH
	ulong size;
#endif
	bd_t *bd;

	bd = gd->bd;

#ifndef CONFIG_SYS_NO_FLASH
	size = flash_init();
	display_flash_config(size);
	bd->bi_flashsize = size;
#endif

#ifdef CONFIG_CMD_SF
	puts("SPI-F: ");
	spi_flash_init();
	bd->bi_flashstart = CONFIG_SYS_FLASH_BASE;
#endif

#if CONFIG_SYS_MONITOR_BASE == CONFIG_SYS_FLASH_BASE
	bd->bi_flashoffset = monitor_flash_len;	/* reserved area for U-Boot */
#else
	bd->bi_flashoffset = 0;
#endif
#if defined (CONFIG_CMD_SPI_NAND)
	spi_nand_init();
#endif

}

/*swp_env, level 3*/
void swp_env(void){
	bd_t *bd;
	bd = gd->bd;

	/* relocate environment function pointers etc. */
	printf("Loading %dB env. variables from offset 0x%x\n",
       		CONFIG_ENV_SIZE, CONFIG_ENV_OFFSET);
#ifdef CONFIG_ENV_OFFSET_REDUND
	if(CONFIG_ENV_OFFSET_REDUND != 0) {
		printf("Loading %dB env. variables from offset 0x%x\n",
       			CONFIG_ENV_SIZE, CONFIG_ENV_OFFSET_REDUND);
	} else {
		puts("ERROR!!! CONFIG_ENV_OFFSET_REDUND must NOT be 0, please check it.\n");
		puts("Please re-built and re-burn your image.\n");
		puts("SYSTEM HALT!\n");
		return;
	}
#endif /* CONFIG_ENV_OFFSET_REDUND */

	env_relocate();
#ifdef CONFIG_ENV_OFFSET_REDUND
	if(gd->env_valid == 1) {
		/* relocate environment function pointers etc. */
		printf("Loaded %dB env. variables from offset 0x%x\n",
	       		CONFIG_ENV_SIZE, CONFIG_ENV_OFFSET);
	} else {
		printf("Loaded %dB env. variables from offset 0x%x\n",
	       		CONFIG_ENV_SIZE, CONFIG_ENV_OFFSET_REDUND);
	}
#endif /* CONFIG_ENV_OFFSET_REDUND */
	/* IP Address */
	bd->bi_ip_addr = getenv_IPaddr("ipaddr");	
}
/* swp_uart , level 5 */
void swp_uart(void){
#if defined(OTTO21_PORT_TEMP)
	init_baudrate();	/* initialize baudrate settings */
#else
	gd->baudrate = getenv_ulong("baudrate", 10, CONFIG_BAUDRATE);
#endif
	serial_init(CONSOLE_CH_INDEX,gd->baudrate);		/* serial communications setup */
	console_init_f();
}

#ifdef CONFIG_OTTO_FLASH_LAYOUT
/* swp_gen_fl , level 5 */
/* swp_gen_fl() needs to be called after env_relocate() as it creates environment variable */
extern void otto_flash_layout_init(void);
void swp_gen_fl(void){
	otto_flash_layout_init();	
}
#endif /* #ifdef CONFIG_OTTO_FLASH_LAYOUT */

/* swp_pci , level 7*/
void swp_pci(void){
#if defined(CONFIG_PCI)
	/*
	 * Do pci configuration
	 */
	pci_init();
#endif
}


/* swp_pci , level 9*/
void swp_studio(void){

/** leave this here (after malloc(), environment and PCI are working) **/
	/* Initialize stdio devices */
	stdio_init();
	jumptable_init();
}

/* swp_console_r , level 11 */
void swp_console_r(void){

	/* Initialize the console (after the relocation and devices init) */
	console_init_r();
}

/* swp_env_set , level 13 */
void swp_env_set(void){
	/* Initialize from environment */
	load_addr = getenv_ulong("loadaddr", 16, load_addr);
#if defined(CONFIG_CMD_NET)
	{
		char *s = getenv("bootfile");

		if (s != NULL)
			copy_filename(BootFile, s, sizeof(BootFile));
	}
#endif
}

/* swp_spi , level 15 */
void swp_spi(void){
#ifdef CONFIG_CMD_SPI
	puts("SPI: ");
	spi_init();		/* go init the SPI */
	puts("ready\n");
#endif
}

/* swp_misc , level 17 */

void swp_misc(void){
#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r();
#endif
}
/* swp_post , level 19 */

void swp_post(void){
	soc_post_init();
}
/* swp_eth , level 21 */

void swp_eth(void){

#if defined(CONFIG_CMD_NET)
	puts("Net:   ");
	eth_initialize(gd->bd);
#endif

}


PATCH_REG(swp_flash_init, 1);
PATCH_REG(swp_env, 3);
PATCH_REG(swp_uart, 5);
#ifdef CONFIG_OTTO_FLASH_LAYOUT
PATCH_REG(swp_gen_fl, 5);
#endif /* #ifdef CONFIG_OTTO_FLASH_LAYOUT */
PATCH_REG(swp_pci, 7);
PATCH_REG(swp_studio, 9);
PATCH_REG(swp_console_r, 11);
PATCH_REG(swp_env_set, 13);
PATCH_REG(swp_spi, 15);
PATCH_REG(swp_misc, 17);
PATCH_REG(swp_post, 19);
PATCH_REG(swp_eth, 21);

