#include <cpu/cpu.h>

#define GET_CPU_HZ()  (cg_query_freq(0)*1000000)

#ifndef __ASSEMBLER__
#define SECTION_CMD_NODE __attribute__ ((section (".cli_node")))
#define SECTION_CMD_MP_NODE __attribute__ ((section (".cli_mp_node")))
#define SECTION_CMD_TAIL __attribute__ ((section (".cli_tail")))

#include <lplr_sections.h>
#include <lplr.h>
#include <inline_util.h>

#define _soc (*(soc_t *)(OTTO_SRAM_START+OTTO_HEADER_OFFSET))
#include <bios_io.h>

#define LPLR_VERSION 0x00000001

#include <plr_sections.h>
#include <util.h>

#define SECTION_CG SECTION_SRAM_TEXT
#define SECTION_CG_INFO         SECTION_SDATA
#define SECTION_CG_MISC_DATA	SECTION_RO
#define SECTION_UART SECTION_SRAM_TEXT

/* NOR SPI-F driver uses udelay. Putting it on flash causes MMIO/PIO conflicts. */
#define SECTION_SYS __attribute__ ((section (".sram_text")))

/* For SPI-F driver. */
#define SECTION_NOR_SPIF_PROBE_FUNC    __attribute__ ((section (".nor_spif_probe_func")))
#define SECTION_NOR_SPIF_GEN2_CORE     __attribute__ ((section (".sram_text"), noinline))
#define SECTION_NOR_SPIF_GEN2_COREDATA __attribute__ ((section (".data")))
#define SECTION_NOR_SPIF_GEN2_MISC     __attribute__ ((section (".text")))

extern u32_t lx_bus_freq_mhz SECTION_SDATA;
extern u32_t uart_baud_rate SECTION_SDATA;
#include <cg/cg.h>
extern const cg_info_t cg_info_proj SECTION_PARAMETERS;
#endif

/*  #define SYSTEM_RESET() do { \
        } while(0) */

#define NORSF_CHIP_NUM     (2)
#define NORSF_MMIO_4B_EN   (0)
#define NORSF_XREAD_EN     (0)
#define NORSF_WBUF_LIM_B   (128)
#define NORSF_CFLASH_BASE  (OTTO_SPI_NOR_START)
#define NORSF_UCFLASH_BASE (NORSF_CFLASH_BASE | 0x20000000)

#define TIMER_FREQ_MHZ     (cg_query_freq(CG_DEV_LX))
#define TIMER_STEP_PER_US  (4)
#define TIMER1_BASE        (0xb8003210)
#define UDELAY_TIMER_BASE  TIMER1_BASE

// for cache operation function
/* LFUNC - declare local function */
#define LFUNC(symbol)     \
        .text;       \
        .align 4;    \
        .ent symbol; \
symbol:

/* FUNC - declare global function */
#define GFUNC(symbol)     \
        .text;         \
        .globl symbol; \
        .align 4;      \
        .ent symbol;   \
symbol:

/* FUNC - declare global function in SRAM*/
#define GSFUNC(symbol)     \
        .section .sram_text, "ax", @progbits;   \
        .globl symbol; \
        .align 4;      \
        .ent symbol;   \
symbol:

/* END - mark end of function */
#define END(symbol)       \
        .end symbol

