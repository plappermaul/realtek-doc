#include <util.h>
#include <uart/uart.h>
#include <cg/cg.h>
#include <lib/lzma/tlzma.h>

#define STACK_GUIDE     0xcafebeef

proto_printf_t *_proto_printf SECTION_SDATA =(void*)ALWAYS_RETURN_ZERO;
udelay_t *_udelay SECTION_SDATA =(void *)ALWAYS_RETURN_ZERO;
mdelay_t *_mdelay SECTION_SDATA =(void *)ALWAYS_RETURN_ZERO;
get_timer_t *_get_timer SECTION_SDATA =(get_timer_t *)ALWAYS_RETURN_ZERO;
unsigned int (*_atoi)(const char *v) SECTION_SDATA=(void*)ALWAYS_RETURN_ZERO;
char (*_strcpy)(char *dst, const char *src) SECTION_SDATA=(void*)ALWAYS_RETURN_ZERO;
u32_t (*_strlen)(const char *s) SECTION_SDATA=(void*)ALWAYS_RETURN_ZERO;
int (*_strcmp)(const char *s1, const char *s2) SECTION_SDATA=(void*)ALWAYS_RETURN_ZERO;
char (*_memcpy)(void *dst, const void *src, unsigned int len) SECTION_SDATA=(void*)ALWAYS_RETURN_ZERO;
char (*_mass_copy)(void *dst, const void *src, unsigned int len) SECTION_SDATA=(void*)ALWAYS_RETURN_ZERO;
char (*_memset)(void *dst, char value, unsigned int len) SECTION_SDATA=(void*)ALWAYS_RETURN_ZERO;


u32_t util_cpu_mhz SECTION_SDATA = 0;
u32_t util_ms_accumulator SECTION_SDATA =0;

UTIL_FAR SECTION_UNS_TEXT void 
puts(const char *s) {
    inline_puts(s);
}

UTIL_FAR SECTION_UNS_TEXT void
__sprintf_putc(const char c, void *user) {
    //if (c=='\r') return;
    char **p=(char**)user;
    **p=c;
    ++(*p);
}


#define DUMMY_FUNC      always_return_zero

// message
const char _banner_msg[] SECTION_RECYCLE_DATA = {"\n\nPRELOADER.%u.%u.%u.%x.%x.LPLR.%08x.%08x\n"};
const char _dis_bus_to_mon_msg[] SECTION_RECYCLE_DATA = {"Disable Bus Timeout Monitor\n"};
const char _dis_pcie_msg[] SECTION_RECYCLE_DATA = {"Disable PCIe\n"};

const symb_retrive_entry_t plr_init_utility_retrive_list[] SECTION_RECYCLE_DATA = {
    {SF_PROTO_PRINTF, &_proto_printf},
    {SF_SYS_UDELAY, &_udelay},   
    {SF_SYS_MDELAY, &_mdelay},
    {SF_SYS_GET_TIMER, &_get_timer},
    {SCID_STR_ATOI, &_atoi},
    {SCID_STR_STRCPY, &_strcpy},
    {SCID_STR_STRLEN, &_strlen},
    {SCID_STR_STRCMP, &_strcmp},
    {SCID_STR_MEMCPY, &_memcpy},
    {SCID_STR_MEMSET, &_memset},
    {SCID_STR_MASSCPY, &_mass_copy},
    {ENDING_SYMB_ID, VZERO}
};

void plr_init_delay_wakeup(u32_t);

SECTION_RECYCLE void 
plr_init_utility(void) {
    // 0. put stack guide words
    extern u32_t farthest_stack_position;
    u32_t *cur_sp;
    __asm__ __volatile__  ("addiu %0, $29, -4": "=r"(cur_sp));
    u32_t *sp_end=&farthest_stack_position;
    while (cur_sp != sp_end) 
        *(cur_sp--)=STACK_GUIDE;

    u32_t count_down=1000000;
    while(count_down>0) {
        count_down--;
    }
    // 1. binding
    symb_retrive_list(plr_init_utility_retrive_list, lplr_symb_list_range);

#ifdef HAS_LIB_LZMA
    symb_retrive_and_set(lplr, SF_LIB_LZMA_DECODE, _lzma_decode);
#endif

    // 2. init uart
    if(0==uart_baud_rate) {
        _bios.uart_putc=(fpv_s8_t *)DUMMY_FUNC;
        _bios.uart_getc=(fps32_t *)DUMMY_FUNC;
        _bios.uart_tstc=(fps32_t *)DUMMY_FUNC;
    } else {
#ifdef PROJECT_ON_FPGA
        uart_init(uart_baud_rate, cg_info_proj.dev_freq.lx_mhz);
#else
        uart_init(uart_baud_rate, OTTO_LX_DEF_FREQ); 
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
           VCS_VER,
           _lplr_header.version,
           _lplr_soc_t.cid);
    // 4. disable timeout monitor
    puts(_dis_bus_to_mon_msg);
    RMOD_LB0MTCR(lx_tcen, 0);
    RMOD_LB0STCR(lx_tcen, 0);
    RMOD_LB1MTCR(lx_tcen, 0);
    RMOD_LB1STCR(lx_tcen, 0);
    puts(_dis_pcie_msg);
    REG32(0xBB0013F8) = 0x2;
    REG32(0xB8000504) = 0x0;

    // init delayed wakeup
    plr_init_delay_wakeup(15000); // 15us
}

REG_INIT_FUNC(plr_init_utility, 1);

const char _init_dwakeup_msg[] SECTION_RECYCLE_DATA = {"II: init delayed wakeup %d(ns) ..."};
const char _done_msg[] SECTION_RECYCLE_DATA = {"done\n"};

SECTION_RECYCLE
void plr_init_delay_wakeup(u32_t delay_ns) {
    printf(_init_dwakeup_msg, delay_ns);
    //u32_t cnt=0;
    // toggle and lauch timer once
    TC6DATArv = delay_ns/10;    // unit is 10ns

    RMOD_TC6INTR(tc6ie, 1);
    TC6CTRLrv = 0;
    TC6CTRL_T tc6 = { .v=TC6CTRLrv };
    tc6.f.tc6en = 1;
    tc6.f.tc6mode = 1; // timer mode
    tc6.f.tc6divfactor = 2; // 200MHz/2 = 10ns 
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
    puts(_done_msg);
}
