#ifndef _MISC_SETTING_H_
#define _MISC_SETTING_H_

#include <apro/misc_setting.h>

/* 19/03/14, wiki, package_bonding_efuse_info */
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

__attribute__ ((far)) int otto_is_apro(void);
void rtl9603cvd_stall_setup(void);
u32_t xlat_dram_size_num(void);

#endif
