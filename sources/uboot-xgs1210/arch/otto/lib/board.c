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
#include <pblr.h>
#include <asm/otto_pll.h>

#ifdef CONFIG_INTERRUPT_SUPPORT
#include <asm/mipsregs.h>
#include <interrupt.h>
#endif

#if defined(CONFIG_POST)
#include <post.h>
#endif
#include "rtk_flash_common.h"

DECLARE_GLOBAL_DATA_PTR;

extern int timer_init(void);
extern void spi_flash_init(void);
extern int incaip_set_cpuclk(void);
extern void set_exception_base(u32_t);

extern ulong uboot_end_data;
extern ulong uboot_end;
extern ulong load_addr;

ulong monitor_flash_len;

static inline void soc_post_init(void);

static char *failed = "*** failed ***\n";

/*
 * mips_io_port_base is the begin of the address space to which x86 style
 * I/O ports are mapped.
 */
unsigned const long mips_io_port_base = -1;

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
#ifdef    CONFIG_BOARD_TYPES
    int board_type = gd->board_type;
#else
    int board_type = 0;    /* use dummy arg */
#endif
    puts("DRAM:  ");

    gd->ram_size = initdram(board_type);
    if (gd->ram_size > 0) {
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

static int init_baudrate(void)
{
#define    N_BAUDRATES (sizeof(baudrate_table) / sizeof(baudrate_table[0]))
    const unsigned int baudrate_table[] = CONFIG_SYS_BAUDRATE_TABLE;
    unsigned int br_from_divisor;
    unsigned int diff, prev_diff = 0xFFFFFFFF;
    int i;

    if (parameters.soc.peri_info.baudrate_divisor == 0x0) {
#ifdef CONFIG_FOLLOW_PLR_TO_DIS_UART
        gd->baudrate = 0;
#else
        gd->baudrate = getenv_ulong("baudrate", 10, CONFIG_BAUDRATE);
        parameters._uart_init(CONSOLE_CH_INDEX,0); /* _uart_init is assign_uart_fp() to recover _uart function pointers */
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
    env_init,        /* initialize environment */
    //init_baudrate,        /* initialize baudrate settings */
    //serial_init,        /* serial communications setup */
    //console_init_f,
    display_banner,        /* say that we are here */
    checkboard,
    init_func_ram,
    NULL,
};


void board_init_f(ulong bootflag)
{
    gd_t gd_data, *id;
    bd_t *bd;
    init_fnc_t **init_fnc_ptr;

    /* set .bss to zero */
    memset(&uboot_end_data, 0, ((void *)&uboot_end) - ((void *)&uboot_end_data));

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
    bd->bi_memstart    = CONFIG_SYS_SDRAM_BASE;    /* start of DRAM */
    bd->bi_memsize    = gd->ram_size;        /* size of DRAM in bytes */
    bd->bi_baudrate    = gd->baudrate;        /* Console Baudrate */

    memcpy(id, (void *)gd, sizeof(gd_t));

    /* Obtain kernel address from preloader. */
    load_addr = parameters.soc.layout.kernel1_addr;
    load_addr += CONFIG_SYS_FLASH_BASE;

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
#ifdef CONFIG_INTERRUPT_SUPPORT
    unsigned int ebase_org;
    unsigned int uboot_ebase;
#endif
#ifndef CONFIG_SYS_NO_FLASH
    ulong size;
#endif
    extern void malloc_bin_reloc(void);
#ifndef CONFIG_ENV_IS_NOWHERE
    extern char *env_name_spec;
#endif
    bd_t *bd;

    gd = id;
    gd->flags |= GD_FLG_RELOC;    /* tell others: relocation done */
#ifdef CONFIG_INTERRUPT_SUPPORT
    ebase_org = read_c0_ebase();
    uboot_ebase = UBOOT_EBASE;
    write_c0_ebase(uboot_ebase);

    /* Inform CPU the base address of exception handler. */
    set_exception_base(CONFIG_EXCEPTION_BASE);

#endif /*CONFIG_INTERRUPT_SUPPORT*/

    debug("Now running in RAM - U-Boot at: %08lx\n", dest_addr);

    gd->reloc_off = 0;

    monitor_flash_len = (ulong)&uboot_end_data - dest_addr;

#ifdef CONFIG_POST
    post_init_f();
#endif

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

    bd = gd->bd;

#ifndef CONFIG_SYS_NO_FLASH
    size = flash_init();
    display_flash_config(size);
    bd->bi_flashsize = size;
#endif

#ifdef CONFIG_CMD_SF
    puts("SPI-F: ");
    spi_flash_init();
#endif

    bd->bi_flashstart = CONFIG_SYS_FLASH_BASE;
#if CONFIG_SYS_MONITOR_BASE == CONFIG_SYS_FLASH_BASE
    bd->bi_flashoffset = monitor_flash_len;    /* reserved area for U-Boot */
#else
    bd->bi_flashoffset = 0;
#endif
#ifdef CONFIG_CMD_NAND
    nand_init();        /* go init the NAND */
#endif
    /* relocate environment function pointers etc. */
    printf("Loading %dB env. variables from offset 0x%x\n",
           CONFIG_ENV_SIZE, CONFIG_ENV_OFFSET);
    env_relocate();
    init_baudrate();    /* initialize baudrate settings */
    serial_init(CONSOLE_CH_INDEX,gd->baudrate);        /* serial communications setup */
    console_init_f();

#ifdef CONFIG_POST
    post_bootmode_init();
    //post_run(NULL, POST_ROM | post_bootmode_get(0));
#endif

    /* IP Address */
    bd->bi_ip_addr = getenv_IPaddr("ipaddr");

#if defined(CONFIG_PCI)
    /*
     * Do pci configuration
     */
    pci_init();
#endif

/** leave this here (after malloc(), environment and PCI are working) **/
    /* Initialize stdio devices */
    stdio_init();

    jumptable_init();

    /* Initialize the console (after the relocation and devices init) */
    console_init_r();
/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

    /* Initialize from environment */
    load_addr = getenv_ulong("loadaddr", 16, load_addr);
#if defined(CONFIG_CMD_NET)
    {
        char *s = getenv("bootfile");

        if (s != NULL)
            copy_filename(BootFile, s, sizeof(BootFile));
    }
#endif

#ifdef CONFIG_CMD_SPI
    puts("SPI: ");
    spi_init();        /* go init the SPI */
    puts("ready\n");
#endif

#ifdef CONFIG_INTERRUPT_SUPPORT
#if defined(CONFIG_RTL8390)
    common_irq_init();
#endif
#endif


#if defined(CONFIG_MISC_INIT_R)
    /* miscellaneous platform dependent initialisations */
    misc_init_r();
#endif

    soc_post_init();

#if defined(CONFIG_CMD_NET)
    puts("Net:   ");
    eth_initialize(gd->bd);
#endif

#ifdef CONFIG_POST
    {
        post_run(NULL, POST_RAM | post_bootmode_get(0));
    }
#endif

    /* main_loop() can return to retry autoboot, if so just run it again. */
    for (;;)
        main_loop();

    /* NOTREACHED - no way out of command loop except booting */
}

static inline void soc_post_init(void) {
    const char sf_probe_cmd[] = {"sf probe 0"};

    /* The `baudrate' env was originally set by
       CONFIG_BAUDRATE. Since we removed CONFIG_BAUDRATE, we
       set baudrate env manually. */
    if (getenv_ulong("baudrate", 10, 0xFFFFFFFF) == 0xFFFFFFFF) {
        setenv_ulong("baudrate", gd->baudrate);
    }

    /* Issue `sf probe 0' before entering CLI. Since our dirver
       automatically switches to next CS when access across CS
       boundary, it is lousy to do `sf probe 0' before other
       `sf' commands can be used. */
    run_command(sf_probe_cmd, 0);

    return;
}

void hang(void)
{
    puts("### ERROR ### Please RESET the board ###\n");
    for (;;)
        ;
}
