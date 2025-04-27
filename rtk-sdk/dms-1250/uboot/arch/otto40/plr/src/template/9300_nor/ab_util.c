#include <init_define.h>
#include <uart/uart.h>
#include <util.h>

#define STACK_GUIDE     0xcafecafe
//#define DUMMY_FUNC	always_return_zero
u32_t util_ms_accumulator SECTION_SDATA =0;

// message
//static char _banner_msg[] SECTION_RECYCLE_DATA = {"PRELOADER.%x.%x\n"};
static char _banner_msg[] SECTION_RECYCLE_DATA = {"\n\nPRELOADER.%u.%u.%u.%x.%x\n"};

UTIL_FAR SECTION_UNS_TEXT void
plr_puts(const char *s) {
	inline_puts(s);
}

static int dummy_all(void) {return 0;}
#define DUMMY_FUNC	dummy_all

SECTION_RECYCLE void
plr_init_utility(void) {
//	const u32_t chip_ver = 0;

	/* This is to enable UART IP. */
	//*((volatile u32_t *)0xbb023004) = 0x00000070;

	// 2. init uart
	if(0==uart_baud_rate) {
		_bios.uart_putc=(fpv_s8_t *)DUMMY_FUNC;
		_bios.uart_getc=(fps32_t *)DUMMY_FUNC;
		_bios.uart_tstc=(fps32_t *)DUMMY_FUNC;
        } else {
#ifdef PROJECT_ON_FPGA
	uart_init(uart_baud_rate, cg_info_proj.dev_freq.lx_mhz);
#else
		uart_init(uart_baud_rate, OTTO_LX_DEF_FREQ);    // default ?
#endif
	_bios.uart_putc=uart_putc;
	_bios.uart_getc=uart_getc;
	_bios.uart_tstc=uart_tstc;
	}

	// 3. using printf showing the banner
	//printf(_banner_msg, _soc_header.version, chip_ver);
        printf(_banner_msg,
               (_soc_header.version >> 24),
               (_soc_header.version >> 8) & 0xffff,
               _soc_header.version & 0xff,
               MAKE_DATE,
               VCS_VER);

	// 4. init timer
#ifdef PROJECT_ON_FPGA
	otto_lx_timer_init(cg_info_proj.dev_freq.lx_mhz);
#else
	otto_lx_timer_init(OTTO_LX_DEF_FREQ);
#endif
	// 5. put stack guide words
	/* extern u32_t farthest_stack_position; */
	/* u32_t *cur_sp; */
	/* __asm__ __volatile__  ("addiu %0, $29, -4": "=r"(cur_sp)); */
	/* u32_t *sp_end=(u32_t *)(0x9f000000 + 16*1024); */
	/* while (cur_sp != sp_end)  */
	/*     *(cur_sp--)=STACK_GUIDE; */

       // disable bus timeout monitor
	puts("II: Disable Bus Timeout Monitor\n");
       RMOD_OBTCR(to_ctrl_en, 0);
       RMOD_LBPSTCR(lx_tcen, 0);
       RMOD_LB0MTCR(lx_tcen, 0);
       RMOD_LB0STCR(lx_tcen, 0);
       RMOD_LB1MTCR(lx_tcen, 0);
       RMOD_LB1STCR(lx_tcen, 0);
       RMOD_LB2MTCR(lx_tcen, 0);
       RMOD_LB2STCR(lx_tcen, 0);
}

REG_INIT_FUNC(plr_init_utility, 1);
