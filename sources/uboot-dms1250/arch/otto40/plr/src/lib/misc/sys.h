#ifndef SYS_H
#define SYS_H

typedef void (udelay_t)(u32_t us, u32_t cpu_mhz);
typedef void (mdelay_t)(u32_t ms, u32_t cpu_mhz);
typedef u32_t (get_timer_t)(u32_t base, u32_t *ms_accumulator, u32_t cpu_mhz);

#ifndef NO_SYS_FUNC
extern void otto_sys_udelay(u32_t us, u32_t cpu_mhz);
extern void otto_sys_mdelay(u32_t ms, u32_t cpu_mhz);
extern u32_t otto_sys_get_timer(u32_t base, u32_t *ms_accumulator, u32_t cpu_mhz);
#endif

#endif

