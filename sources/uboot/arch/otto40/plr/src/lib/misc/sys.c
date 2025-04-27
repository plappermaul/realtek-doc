#include <util.h>
#include <cpu/cpu.h>

#ifndef SECTION_SYS
    #define SECTION_SYS 
#endif

SECTION_SYS void
otto_sys_udelay(u32_t us, u32_t cpu_mhz) {
    u32_t base=CPU_GET_CP0_CYCLE_COUNT();
    u32_t w=us*cpu_mhz;
    while (1) {
        u32_t now=CPU_GET_CP0_CYCLE_COUNT();
        if (((now-base) << 1)>=w) return;
    }
}
SECTION_SYS void
otto_sys_mdelay(u32_t ms, u32_t cpu_mhz) {   
    while (ms>=1000) {
        otto_sys_udelay(1000000, cpu_mhz);
        ms-=1000;
    }
    if (ms>0) otto_sys_udelay(ms*1000, cpu_mhz);
}
SECTION_SYS u32_t
otto_sys_get_timer(u32_t base, u32_t *ms_accumulator, u32_t cpu_mhz) {
    u32_t now = CPU_GET_CP0_CYCLE_COUNT();
    u32_t cycle_count_per_ms = cpu_mhz*1000;
    u32_t _accumulate_time_ms=*ms_accumulator;
    u32_t d = now - _accumulate_time_ms * cycle_count_per_ms;
    u32_t c = d / cycle_count_per_ms;

    _accumulate_time_ms += c;
    *ms_accumulator = _accumulate_time_ms;
    return _accumulate_time_ms - base;
}
