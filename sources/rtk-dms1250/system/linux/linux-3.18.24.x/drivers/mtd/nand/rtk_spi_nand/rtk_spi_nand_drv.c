/******************************************************************************
 * $Id: luan_spi_nand.c,v 1.0 2016/06/13   Exp $
 * drivers/mtd/nand/rtk_spi_nand_drv.c
 * Overview: Realtek NAND Flash Controller Driver
 * Copyright (c) 2016 Realtek Semiconductor Corp. All Rights Reserved.
 * Modification History:
 *
 *******************************************************************************/
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/nand.h>
#include <mtd/mtd-abi.h>
#include "rtk_spi_nand_drv.h"
#include <soc.h>
#include <linux/delay.h>

//id (* hook_restart_func)(void) = NULL;

//id rtk_hook_restart_function(void (*func)(void))
typedef spi_nand_flash_info_t * (*pf_t)(void);
typedef int (*ir_t)(spi_nand_flash_info_t *);


pf_t spi_nand_chip_probe_func[] =
{   probe_gd_spi_nand_chip,
    probe_mxic_spi_nand_chip,
    probe_toshiba_spi_nand_chip,
    probe_winbond_spi_nand_chip,
#ifdef CONFIG_SDK_BSP_MTD_SPI_NAND_G3
    probe_micron_spi_nand_chip,
    probe_ato_spi_nand_chip,
    probe_dosilicon_spi_nand_chip,
    probe_esmt_spi_nand_chip,
    probe_etron_spi_nand_chip,
    probe_fmsh_spi_nand_chip,
    probe_heyangtek_spi_nand_chip,
    probe_longsys_spi_nand_chip,
    probe_xtx_spi_nand_chip,
    probe_zentel_spi_nand_chip,
#endif
};

ir_t spi_nand_chip_init_rest_func[] =
{   gd_init_rest,
    mxic_init_rest,
    toshiba_init_rest,
    winbond_init_rest,
#ifdef CONFIG_SDK_BSP_MTD_SPI_NAND_G3
    micron_init_rest,
    ato_init_rest,
    dosilicon_init_rest,
    esmt_init_rest,
    etron_init_rest,
    fmsh_init_rest,
    heyangtek_init_rest,
    longsys_init_rest,
    xtx_init_rest,
    zentel_init_rest,
#endif
};

/* Add by Managemented Switch Project for CS1 MUX setting */
/* Set NAND controller CS0 to PIN CS1 */
void
rtk_luna_cs_pin_set(void)
{
    unsigned int reg_v;

    reg_v = REGISTER_REG32(PAD_PSR0);
    reg_v &= ~(PAD_PSR0_CS1_MUX_SEL_MASK);
    reg_v |= ((0x2)<<PAD_PSR0_CS1_MUX_SEL_OFF);
    REGISTER_REG32(PAD_PSR0) = reg_v;
    mdelay(500);
}

spi_nand_flash_info_t * probe_spi_nand_chip_func(void)
{
    unsigned int    loop_idx;
    void *flash_info = VZERO;
    unsigned int table_size = (sizeof(spi_nand_chip_probe_func)/sizeof(pf_t));
    for(loop_idx = 0; loop_idx < table_size; loop_idx++)
    {
        flash_info = (void *)spi_nand_chip_probe_func[loop_idx]();
        if(flash_info != VZERO)
            return flash_info;
    }
    return flash_info;
}

int probe_spi_nand_chip_init_rest(spi_nand_flash_info_t * spi_nand_info)
{
    unsigned int    loop_idx;
    int             result;
    unsigned int    table_size = (sizeof(spi_nand_chip_init_rest_func)/sizeof(ir_t));

    for(loop_idx = 0; loop_idx < table_size; loop_idx++)
    {
        result = spi_nand_chip_init_rest_func[loop_idx](spi_nand_info);
        if(result == 1)
            return result;
    }
    return result;
}


uint32_t spi_nand_chip_size(void) {
    uint32_t chip_size;
    chip_size = ((SNAF_PAGE_SIZE((&spi_nand_flash_info)))*(SNAF_NUM_OF_PAGE_PER_BLK((&spi_nand_flash_info)))*(SNAF_NUM_OF_BLOCK((&spi_nand_flash_info))));
    return chip_size;
}

uint32_t spi_nand_block_size(void) {
    uint32_t block_size;
    block_size = ((SNAF_PAGE_SIZE((&spi_nand_flash_info)))*(SNAF_NUM_OF_PAGE_PER_BLK((&spi_nand_flash_info))));
    return block_size;
}

uint32_t spi_nand_page_size(void) {
    return SNAF_PAGE_SIZE((&spi_nand_flash_info));
}

uint32_t spi_nand_spare_size(void) {
    return SNAF_SPARE_SIZE((&spi_nand_flash_info));
}

uint32_t spi_nand_oob_size(void) {
    return SNAF_OOB_SIZE((&spi_nand_flash_info));
}

uint32_t spi_nand_cs_count(void) {
    return g_spiNand_chipNum;
}


extern u32_t nsc_get_feature_register(u32_t cs, u32_t feature_addr);

u32_t spi_nand_get_feature(u32_t cs, u32_t feature_addr) {
    u32_t v = 0xFF;
    v = nsc_get_feature_register(cs, feature_addr);
    return v;
}


