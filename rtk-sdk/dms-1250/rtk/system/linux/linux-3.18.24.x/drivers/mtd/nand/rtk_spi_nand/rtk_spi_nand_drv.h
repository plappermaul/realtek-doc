#include <common/rt_autoconf.h>
#include "spi_nand_struct.h"


/* Following APIs are listed in mtdcore.h */
extern int add_mtd_device(struct mtd_info *mtd);
extern int add_mtd_partitions(struct mtd_info *, const struct mtd_partition *, int);
extern int del_mtd_partitions(struct mtd_info *);
extern int parse_mtd_partitions(struct mtd_info *master, const char * const *types,struct mtd_partition **pparts,struct mtd_part_parser_data *data);
/* Above APIs are listed in mtdcore.h */

#define PAD_PSR0                    0xb8000100
#define PAD_PSR0_CS1_MUX_SEL_OFF    13
#define PAD_PSR0_CS1_MUX_SEL_MASK   ((0x3)<<PAD_PSR0_CS1_MUX_SEL_OFF)

#ifndef REGISTER_REG32
#define REGISTER_REG32(reg)		(*(volatile unsigned int   *)(reg))
#endif

extern spi_nand_flash_info_t * probe_gd_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_mxic_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_micron_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_toshiba_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_winbond_spi_nand_chip(void);
#ifdef CONFIG_SDK_BSP_MTD_SPI_NAND_G3
extern spi_nand_flash_info_t * probe_micron_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_ato_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_dosilicon_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_esmt_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_etron_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_fmsh_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_heyangtek_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_longsys_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_xtx_spi_nand_chip(void);
extern spi_nand_flash_info_t * probe_zentel_spi_nand_chip(void);
#endif


extern int gd_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int mxic_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int toshiba_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int winbond_init_rest(spi_nand_flash_info_t * _spi_nand_info);
#ifdef CONFIG_SDK_BSP_MTD_SPI_NAND_G3
extern int micron_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int ato_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int dosilicon_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int esmt_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int etron_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int fmsh_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int heyangtek_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int longsys_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int xtx_init_rest(spi_nand_flash_info_t * _spi_nand_info);
extern int zentel_init_rest(spi_nand_flash_info_t * _spi_nand_info);
#endif

void rtk_luna_cs_pin_set(void);
spi_nand_flash_info_t * probe_spi_nand_chip_func(void);
int probe_spi_nand_chip_init_rest(spi_nand_flash_info_t * _spi_nand_info);

extern int g_spiNand_chipNum;
extern struct spi_nand_flash_info_s spi_nand_flash_info;

