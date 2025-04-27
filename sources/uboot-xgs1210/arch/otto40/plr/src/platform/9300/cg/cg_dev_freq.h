#ifndef __CG_DEV_FREQ_H__
#define __CG_DEV_FREQ_H__

typedef struct {
    s16_t cpu_mhz;
    s16_t mem_mhz;
    s16_t lx_mhz;
    s16_t spif_mhz;
    s16_t snaf_mhz;
    s16_t sram_mhz;
} cg_dev_freq_t;

#define CG_QUERY_FREQUENCY(dev_type, dev_freq_sel) ({ \
    u32_t freq_ret; \
    switch(dev_type){ \
        case CG_DEV_OCP: \
            freq_ret = dev_freq_sel->cpu_mhz; break;\
        case CG_DEV_MEM: \
            freq_ret = dev_freq_sel->mem_mhz; break;\
        case CG_DEV_LX: \
            freq_ret = dev_freq_sel->lx_mhz; break;\
        case CG_DEV_SPIF: \
            freq_ret = dev_freq_sel->spif_mhz; break;\
        default: \
            freq_ret = (u32_t)(-1); \
    } \
    freq_ret; \
})

#endif //__CG_DEV_FREQ_H__

