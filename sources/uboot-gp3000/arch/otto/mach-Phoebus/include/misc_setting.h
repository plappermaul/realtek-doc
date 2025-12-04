#ifndef _MISC_SETTING_H_
#define _MISC_SETTING_H_

#define OTTO_CID ((*(soc_t *)(0x9F000020)).cid)&0xFFFF
#define OTTO_SID ((*(soc_t *)(0x9F000020)).sid)

/* SID for APro, APro Gen2, RTL9603C-VD */
#define PLR_SID_APRO       0x7
#define PLR_SID_APRO_GEN2  0x1
#define PLR_SID_9603CVD    0x8


typedef enum {
    ST_RTL9603CT  = 0,
    ST_RTL9603C   = 1,
    ST_RTL9603CP  = 2,
    ST_RTL9603CW  = 3,
    ST_RTL9603CE  = 4,
    ST_RTL9606C   = 6,
    ST_RTL9607C   = 8,
    ST_RTL9607CP  = 9,
    ST_RTL9607E   = 10,
    ST_RTL9607EP  = 11,
    ST_RTL8198D   = 20,
    ST_RTL8198DE  = 21,
} apro_sub_chip_type_t;

typedef enum {
	ST_RTL9603CVD4CG = 1,
	ST_RTL9602CVD4CG = 2,
	ST_RTL9603CVD5CG = 4,
	ST_RTL9602CVD5CG = 5,
	ST_RTL9603CVD6CG = 8,
	ST_RTL9602CVD6CG = 9,
	ST_RTL8281CG     = 10,
	ST_RTL9603CEVDCG = 16,
	ST_RTL9601DVD3CG = 20,
	ST_RTL9601DVD4CG = 24,
	ST_RTL9601DVD5CG = 28,
	ST_RTL9601DWVD5CG = 29,
	ST_RTL8281SCG    = 30
} rtl9603cvd_sub_chip_type_t;



u32_t xlat_dram_size_num(void);

#endif
