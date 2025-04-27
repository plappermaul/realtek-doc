#ifndef UTIL_H
#define UTIL_H
#include <soc.h>
#include <init_define.h>

// section and symbols
#define NSU_BIOS            (_soc.bios)
#define SECTION_SPI_NAND    SECTION_UNS_TEXT

#ifndef UTIL_MIPS16
    #ifdef NO_UTIL_MIPS16
        #define UTIL_MIPS16
    #else
        #define UTIL_MIPS16  __attribute__((mips16))
    #endif
#endif

#ifndef UTIL_FAR
    #ifndef NO_UTIL_FAR
        #define UTIL_FAR __attribute__ ((far))
    #else
        #define UTIL_FAR
    #endif
#endif

#include <symb_define.h>
//#define symb_retrive_lplr(key) symb_retrive(key, _lplr_header.export_symb_list, _lplr_header.end_of_export_symb_list);
//#define symb_retrive_plr(key) symb_retrive(key, _soc_header.export_symb_list, _soc_header.end_of_export_symb_list);
#define lplr_symb_list_range _lplr_header.export_symb_list, _lplr_header.end_of_export_symb_list
#define plr_symb_list_range _soc_header.export_symb_list, _soc_header.end_of_export_symb_list
#define symb_retrive_lplr(key) symb_retrive(key, lplr_symb_list_range)
#define symb_retrive_plr(key) symb_retrive(key, plr_symb_list_range)
#define symb_retrive_and_set(lplr_or_plr, symb_id, local_symb) ({\
    const symbol_table_entry_t *s=symb_retrive_ ## lplr_or_plr(symb_id);\
    if (s) local_symb=s->v.pvalue; })


// Timer & Delay function pointer defined for 9602C only
typedef void (udelay_t)(u32_t us, u32_t cpu_mhz);
typedef void (mdelay_t)(u32_t ms, u32_t cpu_mhz);
typedef u32_t (get_timer_t)(u32_t base, u32_t *ms_accumulator, u32_t cpu_mhz);
extern u32_t util_cpu_mhz SECTION_SDATA;
extern u32_t util_ms_accumulator SECTION_SDATA;

// functions defined in LPLR
#define NO_PROTO_PRINTF
#include <proto_printf.h>
extern proto_printf_t *_proto_printf;
extern void puts(const char *s) UTIL_FAR;
extern void __sprintf_putc(const char c, void *user) UTIL_FAR;
extern unsigned int (*_atoi)(const char *v) SECTION_SDATA;
extern char (*_strcpy)(char *dst, const char *src) SECTION_SDATA;
extern u32_t (*_strlen)(const char *s) SECTION_SDATA;
extern int (*_strcmp)(const char *s1, const char *s2) SECTION_SDATA;
extern char (*_memcpy)(void *dst, const void *src, unsigned int len) SECTION_SDATA;
extern char (*_mass_copy)(void *dst, const void *src, unsigned int len) SECTION_SDATA;
extern char (*_memset)(void *dst, char value, unsigned int len) SECTION_SDATA;
extern udelay_t *_udelay SECTION_SDATA;
extern mdelay_t *_mdelay SECTION_SDATA;
extern get_timer_t *_get_timer SECTION_SDATA;

#define printf(...) (*_proto_printf)((proto_putc_t*)_bios.uart_putc, VZERO, __VA_ARGS__)
#define sprintf(dst_buf, ...) ({char *___dst_buf=dst_buf; \
    unsigned int ___res=(*_proto_printf)(__sprintf_putc, (void*)&___dst_buf, __VA_ARGS__); \
    *___dst_buf='\0'; ___res})

#define atoi (*_atoi)
#define strcpy (*_strcpy)
#define strlen (*_strlen)
#define strcmp (*_strcmp)
#define memcpy (*_memcpy)
#define mass_copy (*_mass_copy)
#define memset (*_memset)
#define bzero(dst, len) (*_memset)(dst, 0, len)
#define udelay(us) (*_udelay)(us, util_cpu_mhz)
#define mdelay(ms) (*_mdelay)(ms, util_cpu_mhz)
#define get_timer(base) (*_get_timer)(base, &util_ms_accumulator, util_cpu_mhz)

#define SWBREAK() do {               \
		__asm__ __volatile__ ("sdbbp;"); \
	} while(0)

// dummy function
extern int always_return_zero(void);
#define ALWAYS_RETURN_ZERO always_return_zero

#endif // UTIL_H

