#include <init_define.h>
#include <uart/uart.h>
#include <util.h>

#define STACK_GUIDE     0xcafecafe

u32_t util_ms_accumulator SECTION_SDATA =0;

// message
//static char _banner_msg[] SECTION_RECYCLE_DATA = {"PRELOADER.%x.%x\n"};
static char _banner_msg[] SECTION_RECYCLE_DATA = {"\n\nPRELOADER.%u.%u.%u.%x.%x\n"};

static int dummy_all(void) {return 0;}
#define DUMMY_FUNC      dummy_all

UTIL_FAR SECTION_UNS_TEXT void
plr_puts(const char *s) {
	inline_puts(s);
}

extern void plr_init_delay_wakeup(u32_t);

SECTION_RECYCLE void
plr_init_utility(void) {
//	const u32_t chip_ver = 0;

	// 2. init uart
	if(0==uart_baud_rate) {
                _bios.uart_putc=(fpv_s8_t *)DUMMY_FUNC;
                _bios.uart_getc=(fps32_t *)DUMMY_FUNC;
                _bios.uart_tstc=(fps32_t *)DUMMY_FUNC;
        } else {
#ifdef PROJECT_ON_FPGA
        uart_init(uart_baud_rate, cg_info_proj.dev_freq.lx_mhz);
#else
        uart_init(uart_baud_rate, OTTO_LX_DEF_FREQ);	// default ?
#endif
	_bios.uart_putc=uart_putc;
	_bios.uart_getc=uart_getc;
	_bios.uart_tstc=uart_tstc;
        }

	// 3. using printf showing the banner
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

       // disable bus timeout monitor
        puts("II: disable bus timeout monitor\n");
        RMOD_LB0MTCR(lx_tcen, 0);
        RMOD_LB0STCR(lx_tcen, 0);
        RMOD_LB1MTCR(lx_tcen, 0);
        RMOD_LB1STCR(lx_tcen, 0);
        puts("II: disable PCIe\n");
        REG32(0xBB0013F8) = 0x2;
        REG32(0xB8000504) = 0x0;

        // init delayed wakeup
        plr_init_delay_wakeup(15000); // 15us
}

REG_INIT_FUNC(plr_init_utility, 1);


void plr_init_delay_wakeup(u32_t delay_ns) {
    printf("II: init delayed wakeup %d(ns) ...", delay_ns);
    //u32_t cnt=0;
    // toggle and lauch timer once
    TC6DATArv = delay_ns/10;	// unit is 10ns

    RMOD_TC6INTR(tc6ie, 1);
    TC6CTRLrv = 0;
    TC6CTRL_T tc6 = { .v=TC6CTRLrv };
    tc6.f.tc6en = 1;
    tc6.f.tc6mode = 1; // timer mode
    tc6.f.tc6divfactor = 2; // FIXME! as LX freq.
    TC6CTRLrv = tc6.v;

    while(0==RFLD_TC6INTR(tc6ip)) {
        /*cnt++;
        if(cnt>0x1000000) {
            puts("failed!\n");
            return;
        }*/
    }

    TC6_DLY_INTR_T tc6dint = { .v=TC6_DLY_INTRrv };
    tc6dint.f.iti_trig = 1;
    tc6dint.f.delayed_ip_sel = 2;
    TC6_DLY_INTRrv = tc6dint.v;

    RMOD_SYS_MISC_CTRL0(cpu_cmu_bypass, 0);

    puts("done\n");
}

#if 0
#include <cli/cli_access.h>

cli_cmd_ret_t
cli_dw_int(const void *user, u32_t argc, const char *argv[])
{
   u32_t delay_ns = 1000;
   if(3==argc) {
        delay_ns = atoi(argv[2]);
        if(delay_ns<10) return CCR_FAIL;
   }

   plr_init_delay_wakeup(delay_ns);
   return CCR_OK;
}


cli_add_node(dwakeup, call, (cli_cmd_func_t *)cli_dw_int);
cli_add_help(dwakeup, "call dwakeup: Delayed Wakeup Init");
#endif
