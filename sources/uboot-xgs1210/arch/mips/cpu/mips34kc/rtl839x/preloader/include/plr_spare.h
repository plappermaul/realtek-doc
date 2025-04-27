#ifndef PLR_SPARE_H
#include <preloader.h>

#ifdef USE_SOC_SPARE

#if (OTTO_NAND_FLASH == 1)
    #define SOC_SPARE_HEADER_SECTION __attribute__ ((section ("nand_spare_header")))
#elif (OTTO_NOR_SPI_FLASH == 1)
    #define SOC_SPARE_HEADER_SECTION __attribute__ ((section ("nor_spare_header")))
#else
    #define SOC_SPARE_HEADER_SECTION 
#endif

extern spare_header_t spare_headers[];
extern void *search_spare_by_type(u32_t type, u32_t *num_instances);
static inline dram_info_t *get_dram_spare(u32_t *num_instances) {
    return (dram_info_t *)search_spare_by_type(SST_DRAM, num_instances);}
static inline plr_flash_info_t *get_flash_spare(u32_t *num_instances) {
    return (plr_flash_info_t *)search_spare_by_type(SST_FLASH, num_instances);}

u32_t dram_model_select(void);

#endif //USE_SOC_SPARE

#endif //PLR_SPARE_H
