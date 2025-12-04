#include <common.h>
#include <nand.h>
#include "cortina_spi_nand.h"

static struct spi_nand_info spinand_info;
static struct nand_chip spinand_chip;

static uint8_t dummy_read_byte(struct mtd_info *mtd) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return 0;
}

static u16 dummy_read_word(struct mtd_info *mtd) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return 0;
}

static void dummy_write_byte(struct mtd_info *mtd, uint8_t byte) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return;
}

static void dummy_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return;
}

static void dummy_read_buf(struct mtd_info *mtd, uint8_t *buf, int len) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return;
}

static void dummy_select_chip(struct mtd_info *mtd, int chip) {
    if(chip>0)
        spinand_debug("WW: %s chip=%d\n", __FUNCTION__,chip);
}

static void dummy_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return;
}

static int dummy_dev_ready(struct mtd_info *mtd) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return 1;
}

static void dummy_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return;
}

static int dummy_waitfunc(struct mtd_info *mtd, struct nand_chip *this) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return 0;
}

static int dummy_write_page(struct mtd_info *mtd, struct nand_chip *chip, uint32_t offset, int data_len,
    const uint8_t *buf, int oob_required, int page, int raw) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return 0;
}

static int dummy_set_features(struct mtd_info *mtd, struct nand_chip *chip, int feature_addr, uint8_t *subfeature_para) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return 0;
}

static int dummy_get_features(struct mtd_info *mtd, struct nand_chip *chip, int feature_addr, uint8_t *subfeature_para) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return 0;
}
static int dummy_setup_read_retry(struct mtd_info *mtd, int retry_mode) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return 0;
}

static int dummy_setup_data_interface(struct mtd_info *mtd, int chipnr, const struct nand_data_interface *conf) {
    spinand_debug("DD: %s\n", __FUNCTION__);
    return 0;
}

static int nandchip_block_bad(struct mtd_info *mtd, loff_t ofs)
{
    spinand_debug("DD: %s\n", __FUNCTION__);
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct spi_nand_info *info = (struct spi_nand_info *)chip->priv;
    return spi_nand_block_isbad(info, ofs);
}

static int nandchip_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
    spinand_debug("DD: %s\n", __FUNCTION__);

    struct nand_chip *chip = mtd_to_nand(mtd);
    struct spi_nand_info *info = (struct spi_nand_info *)chip->priv;
    return spi_nand_block_markbad(info, ofs);
}

static int nandchip_erase(struct mtd_info *mtd, int page)
{
    spinand_debug("DD: %s\n", __FUNCTION__);

    struct nand_chip *chip = mtd_to_nand(mtd);
    struct spi_nand_info *info = (struct spi_nand_info *)chip->priv;
    spi_nand_erase(info, page, mtd->erasesize);
    return 0;
}

int spi_nand_mtd_erase(struct mtd_info *mtd, struct erase_info *instr)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct spi_nand_info *info = (struct spi_nand_info *)chip->priv;
    if(instr->scrub)
        return spi_nand_block_markunbad(info, instr->addr);
    else
        return spi_nand_erase(info, instr->addr, instr->len);
}

int spi_nand_mtd_read_oob(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct spi_nand_info *info = (struct spi_nand_info *)chip->priv;
    return spi_nand_read_oob(info, from, ops);
}

int spi_nand_mtd_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct spi_nand_info *info = (struct spi_nand_info *)chip->priv;
    return spi_nand_write_oob(info, to, ops);
}

void spi_nand_update_chip_setting(struct nand_chip *chip)
{
    struct spi_nand_info *info = (struct spi_nand_info *)chip->priv;

    /* Not supported in CA SPI NAND cntlr  */
    chip->IO_ADDR_R = NULL;
    chip->IO_ADDR_W = NULL;

    /* Refchip-> to access flash node in device treechip-> We don't leverage device tree so leave it */
    //chip->flash_node = 0;

    /* For the functions used in general raw nand, assign [*_dummy_] to bypass it */
    chip->read_byte = dummy_read_byte;
    chip->read_word = dummy_read_word;
    chip->write_byte= dummy_write_byte;
    chip->write_buf = dummy_write_buf;
    chip->read_buf =  dummy_read_buf;
    chip->select_chip = dummy_select_chip;
    chip->block_bad = nandchip_block_bad;
    chip->block_markbad = nandchip_block_markbad;
    chip->cmd_ctrl = dummy_cmd_ctrl;
    chip->dev_ready = dummy_dev_ready;
    chip->cmdfunc = dummy_cmdfunc;
    chip->waitfunc = dummy_waitfunc;
    chip->erase = nandchip_erase;
#ifdef CONFIG_CORTINA_SPINAND_BBT
    chip->scan_bbt = spi_nand_default_bbt;
#else
	chip->scan_bbt = NULL;
#endif
    chip->write_page = dummy_write_page;
    chip->onfi_set_features = dummy_set_features;
    chip->onfi_get_features = dummy_get_features;
    chip->setup_read_retry = dummy_setup_read_retry;
    chip->setup_data_interface = dummy_setup_data_interface;

    /* chip dependent delay for transferring data from array to read regs (tR) */
    /* We don't need this, since the underlying driver already handle that */
    //chip->chip_delay = 0;
#ifdef CONFIG_CORTINA_SPINAND_BBT
    /* Set "0" for using BBTSCAN */
    chip->options = 0,

    /* to be set, check bbmchip->h for more info */
    chip->bbt_options = NAND_BBT_USE_FLASH | NAND_BBT_NO_OOB;
#else
    chip->options = NAND_SKIP_BBTSCAN,
    chip->bbt_options = 0;
#endif
    /* number of address bits a page, update at run-time */
    chip->page_shift = info->page_shift;

    /*  number of address bits in a physical eraseblock */
    chip->phys_erase_shift = info->block_shift;

    /* = phy_erase_shift */
    chip->bbt_erase_shift = chip->phys_erase_shift;

    /* flash size addrchip-> bits; update at run-time */
    chip->chip_shift = ilog2(info->size);

    chip->numchips = 1;

    /* flash size, update run-time */
    chip->chipsize = info->size;

    /* page numer mask, update run-time */
    chip->pagemask = info->page_mask;

    /* holds the pagenumber which is currently in data_buf */
    //chip->pagebuf = 0,

    /* bigflip count for the page which isb currently in data_buf */
    //chip->pagebuf_bitflips = USED_IN_RAW_NAND,

    /* minimal write size, size per ECC, run-time */
    //chip->subpagesize = USED_IN_RAW_NAND,

    /* SLC? MLC? etcchip-> */
    chip->bits_per_cell = 1;

    /* ECC ability, default use BCH16/1024Byte */
    chip->ecc_strength_ds = info->ecc_strength;

    /* For general SPI NAND model, it is 512-byte  */
    chip->ecc_step_ds = 512;

    /* SNAF doesn't support thischip-> */
    //chip->onfi_timing_mode_default = USED_IN_RAW_NAND;

    /* position of BBI in sparechip-> */
    chip->badblockpos = 0;

    /* minimum number of set bits in a good block's bad block marker position
       Echip->gchip->, 7 means at least 7 1s (ones) in BBI to present a good block */
    //chip->badblockbits = 8,

    //chip->onfi_version = IGNORED_FOR_NOW,
    //chip->jedec_version = IGNORED_FOR_NOW,

    /* ignore both until further requirement */
    //chip->onfi_params = {{IGNORED_FOR_NOW}},
    //chip->jedec_params = {{IGNORED_FOR_NOW}},

    /* ignore timing until further requirement */
    //chip->data_interface = USED_IN_RAW_NAND;

    /* the number of read retry modes supported */
    //chip->read_retries = USED_IN_RAW_NAND;

    /* the current state of the NAND device, ignored for now */
//    chip->state = IGNORED_FOR_NOW,

    /* laying out OOB data before writing */
    //chip->oob_poi = USED_IN_RAW_NAND;

//    chip->controller = IGNORED_FOR_NOW,
    chip->ecclayout = info->ecclayout;
    chip->ecc.layout = info->ecclayout;

    /*chip->ecc->mode =   USED_IN_RAW_NAND;
        chip->ecc->size =      USED_IN_RAW_NAND;
        chip->ecc->bytes=    USED_IN_RAW_NAND;
        chip->ecc->strength=USED_IN_RAW_NAND;*/

    //chip->buffers = IGNORED_FOR_NOW,

    /* minimum buffer alignment required by a platform */
    //chip->buf_align =USED_IN_RAW_NAND;
    /*chip->hwcontrol = {
        chip->lock = {},
        chip->active = IGNORED_FOR_NOW,
        },*/

    /* bad block table pointer, updated in chip->bbt_scan */
    chip->bbt = NULL;

    /* bad block table descriptor for flash loopup, updated in chip->bbt_scan */
    chip->bbt_td = NULL;

    /* bad block table mirror descriptor, updated in chip->bbt_scan */
    chip->bbt_md = NULL;

    /* bad block scan pattern used for initial bad block scan,  use default pattern */
    chip->badblock_pattern = 0;
}

void board_nand_init(void)
{
    memset(&spinand_chip, 0, sizeof(struct nand_chip));
    memset(&spinand_info, 0, sizeof(struct spi_nand_info));

    /* chip is struct nand_chip, and is now provided by the driver. */
    struct mtd_info *mtd = nand_to_mtd(&spinand_chip);
    uint32_t devnum=0;

    spinand_chip.priv = (void *)(&spinand_info);

    if (spi_nand_scan_ident(mtd, CONFIG_SYS_MAX_NAND_CHIPS)){
        printf("EE: %s, %d\n",__FUNCTION__, __LINE__);
        goto err_out;
    }

    /* Update the probed spec to nand_chip */
    spi_nand_update_chip_setting(&spinand_chip);

    /* Update the probed spec to mtd_info */
    mtd->size = spinand_info.mtd->size;
    mtd->erasesize = spinand_info.block_size;
    mtd->writesize = spinand_info.page_size;
    mtd->writebufsize = spinand_info.block_size;
    mtd->ecc_strength = spinand_info.ecc_strength ? spinand_info.ecc_strength : 1;
    mtd->ecclayout = spinand_info.ecclayout;
    mtd->oobsize = spinand_info.oob_size;
    mtd->oobavail = spinand_info.ecclayout->oobavail;

    if (nand_scan_tail(mtd)){
        printf("EE: %s, %d\n",__FUNCTION__, __LINE__);
        goto err_out;
    }

    /* Replace the MTD driver to project specific one*/
	mtd->_erase     = spi_nand_mtd_erase;
	mtd->_read_oob  = spi_nand_mtd_read_oob;
	mtd->_write_oob = spi_nand_mtd_write_oob;

    /* devnum is the device number to be used in nand commands
         * and in mtd->name.  Must be less than CONFIG_SYS_MAX_NAND_DEVICE.
         */
    if (nand_register(devnum, mtd)){
        printf("EE: %s, %d\n",__FUNCTION__, __LINE__);
        goto err_out;
    }
	return;

err_out:
    return;
}


