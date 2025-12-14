#ifndef _APRO_MISC_SETTING_H_
#define _APRO_MISC_SETTING_H_

#define NA (0x4E41)
typedef enum {
    SIZE_128Mb = 0,
    SIZE_256Mb = 1,
    SIZE_512Mb = 2,
    SIZE_1Gb   = 3,
    SIZE_2Gb   = 4,
    SIZE_4Gb   = 5,
    SIZE_8Gb   = 6,
    SIZE_NA    = 0x4E41,
} memory_size_mapping_t;

typedef enum {
    ST_RTL9603CT  = 0,
    ST_RTL9603C   = 1,
    ST_RTL9603CP  = 2,
    ST_RTL9603CW  = 3,
    ST_RTL9603CE  = 4,
    ST_RTL9607C   = 8,
    ST_RTL9607CP  = 9,
    ST_RTL9607E   = 10,
    ST_RTL9607EP  = 11,
    ST_RTL8198D   = 20,
} sub_chip_type_t;

typedef struct{
    sub_chip_type_t st;;
    s32_t clk;
    memory_size_mapping_t size;
}series_mode_t;

extern u32_t apro_xlat_dram_size_num(void);
extern void apro_chip_pre_init(void);
extern u32_t apro_xlat_cpu_freq(void);
extern u32_t acquire_series(void);
extern __attribute__ ((far)) u32_t apro_cg_query_freq(u32_t);

#endif
