/**
* spi-nand-base.c
*
* Copyright (c) 2009-2015 Micron Technology, Inc.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/
#include <common.h>
#include <malloc.h>
#include <linux/mtd/mtd.h>
#include <errno.h>
#include <nand.h>
#include <watchdog.h>
#include <u-boot/crc.h>
#include "cortina_spi_nand.h"

/* Configuration for SPI NAND write/read IO mode
  *  #define CONFIG_RTK_SPI_NAND_USE_QIO
  *  #define CONFIG_RTK_SPI_NAND_USE_DIO
*/
#ifdef CONFIG_TARGET_VENUS
#define CONFIG_RTK_SPI_NAND_USE_QIO
#endif

static int spi_nand_write(struct spi_nand_info *info, loff_t to, size_t len, size_t *retlen, const u8 *buf);
static int spi_nand_read(struct spi_nand_info *info, loff_t from, size_t len, size_t *retlen, u8 *buf);
static int spi_nand_do_read_oob(struct spi_nand_info *info, loff_t from, struct mtd_oob_ops *ops);


static struct spi_nand_flash spi_nand_table[] = {
    /* ATO */
SPI_NAND_INFO("ATO25D1GA", 0x9B, 0x12, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 1, 0),
    SPI_NAND_INFO("ATO25D2GB",   0x9B, 0x03, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),

    /* Dosilicon */
    SPI_NAND_INFO("DS35Q1GA-IB", 0xE5, 0x71, PAGE_2KB,  OOB_64B, PAGES_PER_BLK, 1024, 1, 4, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("DS35Q2GA-IB", 0xE5, 0x72, PAGE_2KB,  OOB_64B, PAGES_PER_BLK, 2048, 1, 4, SPI_NAND_NEED_PLANE_SELECT|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("DS35Q1GB-IB", 0xE5, 0xF1, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 1024, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("DS35Q2GB-IB", 0xE5, 0xF2, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_PLANE_SELECT|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("DS35Q4GM-IB", 0xE5, 0xF4, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 4096, 1, 8, SPI_NAND_NEED_PLANE_SELECT|SPI_NAND_NEED_QE_ENABLE),

    /* ESMT */
    SPI_NAND_INFO("F50L1G41LB",  0xC8, 0x01, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 1024, 1, 1, SPI_NAND_MFR_IS_ESMT),
    SPI_NAND_INFO("F50L2G41LB",  0xC8, 0x0A, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 1024, 2, 1, SPI_NAND_MFR_IS_ESMT|SPI_NAND_NEED_DIE_SELECT),
    SPI_NAND_INFO("F50L2G41KA",  0xC8, 0x41, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_MFR_IS_ESMT),

    /* Etron */
    SPI_NAND_INFO("EM73C044VCC_H", 0xD5, 0x22, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 1, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("EM73D044VCE_H", 0xD5, 0x20, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 1, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("EM73C044VCF_H", 0xD5, 0x25, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 4, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("EM73D044VCL_H", 0xD5, 0x2E, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("EM73E044VCB_H", 0xD5, 0x2F, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 4096, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("EM73F044VCA_H", 0xD5, 0x2D, PAGE_4KB, OOB_256B, PAGES_PER_BLK, 4096, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("EM73D044VCO_H", 0xD5, 0x3A, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("EM73E044VCE_H", 0xD5, 0x3B, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 4096, 1, 8, SPI_NAND_NEED_QE_ENABLE),

    /* FMSH */
    SPI_NAND_INFO("FM25S01",    0xA1, 0xA1, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 1024, 1, 1, 0),
    SPI_NAND_INFO("FM25S01A",   0xA1, 0xE4, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 1, 0),
    SPI_NAND_INFO("FM25S02A",   0xA1, 0xE5, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 1, SPI_NAND_NEED_QE_ENABLE),

    /* GD */
    SPI_NAND_INFO("GD5F1GQ4UxYIG", 0xC8, 0xD1, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 1024, 1, 8, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F2GQ4UxxIG", 0xC8, 0xD2, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F4GQ4UBYIG", 0xC8, 0xD4, PAGE_4KB, OOB_256B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F1GQ4UEYIH", 0xC8, 0xD9, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 8, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F1GQ5UEYIH", 0xC8, 0x31, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 4, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F2GQ5UEYIH", 0xC8, 0x32, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 4, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F4GQ6UEYIH", 0xC8, 0x35, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 4096, 1, 4, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F1GQ5UEYIG", 0xC8, 0x51, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 1024, 1, 4, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F2GQ5UEYIG", 0xC8, 0x52, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 4, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F4GQ6UEYIG", 0xC8, 0x55, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 4096, 1, 4, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F1GM7UEYIG", 0xC8, 0x91, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 8, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F2GM7UEYIG", 0xC8, 0x92, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("GD5F4GM8UEYIG", 0xC8, 0x95, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 4096, 1, 8, SPI_NAND_MFR_IS_GD|SPI_NAND_NEED_QE_ENABLE),

    /* HeYangTek */
    SPI_NAND_INFO("HYF1GQ4UDACAE", 0xC9, 0x21, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 4,  SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("HYF1GQ4UPACAE", 0xC9, 0xA1, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 1024, 1, 1,  0),
    SPI_NAND_INFO("HYF2GQ4UHCCAE", 0xC9, 0x5A, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 4,  SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("HYF2GQ4UAACAE", 0xC9, 0x52, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 4,  SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("HYF4GQ4UAACBE", 0xC9, 0xD4, PAGE_4KB, OOB_256B, PAGES_PER_BLK, 2048, 1, 14, SPI_NAND_NEED_QE_ENABLE),

    /* Longsys */
    SPI_NAND_INFO("FS35ND01G_D1F1QWHI100", 0xCD, 0xA1, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 1024, 1, 4, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("FS35ND01G_S1F1QWFI000", 0xCD, 0xB1, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 1024, 1, 4, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("FS35ND02G_S2F1QWFI000", 0xCD, 0xA2, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 2048, 1, 4, 0),
    SPI_NAND_INFO("FS35ND02G_D1F1QWFI000", 0xCD, 0xB2, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 2048, 1, 4, 0),
    SPI_NAND_INFO("FS35ND04G_S2F1QWFI000", 0xCD, 0xA4, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 4096, 1, 4, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("FS35ND01G-S1Y2", 0xCD, 0xEA, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 2048, 1, 4, 0),
    SPI_NAND_INFO("FS35ND02G-S3Y2", 0xCD, 0xEB, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 2048, 1, 4, 0),
    SPI_NAND_INFO("FS35ND04G-S2Y2", 0xCD, 0xEC, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 4096, 1, 4, 0),
    SPI_NAND_INFO("F35SQA002G",     0xCD, 0x72, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 2048, 1, 1, 0),

    /* Macronix */
    SPI_NAND_INFO("MX35LF1GE4AB", 0xC2, 0x12, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 4, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("MX35LF2GE4AD", 0xC2, 0x26, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("MX35LF4GE4AD", 0xC2, 0x37, PAGE_4KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),

    /* Micron */
    SPI_NAND_INFO("MT29F1G01ABAFDWB-IT:F", 0x2C, 0x14, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 1024, 1, 8, 0),
    SPI_NAND_INFO("MT29F2G01ABAGDWB/F50L2G41XA/XT26G02ELGIG/WSFVC32GBID", 0x2C, 0x24, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_PLANE_SELECT),
    SPI_NAND_INFO("MT29F4G01ABAFDWB/F50L4G41XB", 0x2C, 0x34, PAGE_4KB, OOB_256B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_DIE_SELECT),

    /* Toshiba */
    SPI_NAND_INFO("TC58CVG0S3HRAIG", 0x98, 0xC2, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 8, 0),
    SPI_NAND_INFO("TC58CVG1S3HRAIG", 0x98, 0xCB, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 8, 0),
    SPI_NAND_INFO("TC58CVG2S0HRAIG", 0x98, 0xCD, PAGE_4KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 5, 0),
    SPI_NAND_INFO("TC58CVG0S3HRAIJ", 0x98, 0xE2, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("TC58CVG1S3HRAIJ", 0x98, 0xEB, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("TC58CVG2S0HRAIJ", 0x98, 0xED, PAGE_4KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 5, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("TC58CVG3S0HRAIJ", 0x98, 0xE4, PAGE_4KB, OOB_128B, PAGES_PER_BLK, 4096, 1, 5, SPI_NAND_NEED_QE_ENABLE),

    /* United Memory */
    SPI_NAND_INFO("GSS01GSAK1", 0x52, 0xBA13, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 1024, 1, 4, 0),
    SPI_NAND_INFO("GSS02GSAK1", 0x52, 0xBA23, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 4, 0),

    /* WINBOND */
    SPI_NAND_INFO("W25N512GVEIG", 0xEF, 0xAA20, PAGE_2KB, OOB_64B,  PAGES_PER_BLK,  512, 1, 1, 0),
    SPI_NAND_INFO("W25N01GVZEIG", 0xEF, 0xAA21, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 1, 0),
    SPI_NAND_INFO("W25M02GVZEIG", 0xEF, 0xAB21, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 2, 1, SPI_NAND_NEED_DIE_SELECT),
    SPI_NAND_INFO("W25N01KVZEIR", 0xEF, 0xAE21, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 4, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("W25N02KVZEIE", 0xEF, 0xAA22, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("W25N04KVZEIE", 0xEF, 0xAA23, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 4096, 1, 8, SPI_NAND_NEED_QE_ENABLE),

    /* Xin Cun */
    SPI_NAND_INFO("XCSP2AAPK-IT", 0x8C, 0xA1, PAGE_2KB, OOB_128B,  PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),

    /* XTX */
    SPI_NAND_INFO("XT26G01AWSEGA", 0x0B, 0xE1, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("XT26G02AWSEGA", 0x0B, 0xE2, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("XT26G01BWSEGA", 0x0B, 0xF1, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 1024, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("XT26G02BWSEGA", 0x0B, 0xF2, PAGE_2KB, OOB_64B,  PAGES_PER_BLK, 2048, 1, 4, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("XT26G01CWSIGA", 0x0B, 0x11, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 1024, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("XT26G02CWSIGA", 0x0B, 0x12, PAGE_2KB, OOB_256B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("XT26G04CWSIGA", 0x0B, 0x13, PAGE_4KB, OOB_256B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),
    SPI_NAND_INFO("XT26G12DWSIGA", 0x0B, 0x35, PAGE_2KB, OOB_128B, PAGES_PER_BLK, 2048, 1, 8, SPI_NAND_NEED_QE_ENABLE),

    /* Zentel */
    SPI_NAND_INFO("A5U12A21ASC_AWS", 0xC8, 0x20, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 512,  1, 1, SPI_NAND_MFR_IS_ZENTEL),
    SPI_NAND_INFO("A5U1GA21BSC_BWS", 0xC8, 0x21, PAGE_2KB, OOB_64B, PAGES_PER_BLK, 1024, 1, 1, SPI_NAND_MFR_IS_ZENTEL),
};

#ifdef CONFIG_RTK_SPI_NAND_USE_QIO
static struct spi_nand_info spi_nand_opcode_overwrite_table[] = {
    OPCODE_OW(SPI_NAND_MFR_ATO,            0x12,   104, SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_ATO,            0x03,   40,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(SPI_NAND_MFR_DOSILICON,      0xFFFF, 50, SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(SPI_NAND_MFR_ETRON,          0xFFFF, 120, SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(SPI_NAND_MFR_FMSH,           0x92,   104, SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(SPI_NAND_MFR_ESMT_GD_ZENTEL, 0xD1,   52,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_ESMT_GD_ZENTEL, 0xD2,   52,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_ESMT_GD_ZENTEL, 0xD9,   52,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_ESMT_GD_ZENTEL, 0x31,   66,  SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_ESMT_GD_ZENTEL, 0x32,   52,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_ESMT_GD_ZENTEL, 0x35,   52,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_ESMT_GD_ZENTEL, 0x51,   66,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 4, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_ESMT_GD_ZENTEL, 0x52,   52,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(SPI_NAND_MFR_HEYANGTEK,      0xA1,   80,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 4, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_HEYANGTEK,      0xFFFF, 80,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(SPI_NAND_MFR_LONGSYS,        0xA1,   54,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_LONGSYS,        0xB1,   100, SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_LONGSYS,        0xA4,   54,  SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(SPI_NAND_MFR_MACRONIX,       0x12,   100, SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA_X4),

    OPCODE_OW(SPI_NAND_MFR_TOSHIBA,        0xC2,   104, SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD,    SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_TOSHIBA,        0xCB,   104, SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD,    SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_TOSHIBA,        0xCD,   104, SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD,    SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_TOSHIBA,        0xE2,   133, SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA_X4),
    OPCODE_OW(SPI_NAND_MFR_TOSHIBA,        0xEB,   133, SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA_X4),
    OPCODE_OW(SPI_NAND_MFR_TOSHIBA,        0xED,   133, SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA_X4),
    OPCODE_OW(SPI_NAND_MFR_TOSHIBA,        0xE4,   133, SPI_NAND_CMD_READ_FROM_CACHE_X4,      8, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA_X4),

    OPCODE_OW(SPI_NAND_MFR_XTX,            0xFFFF, 133, SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO, 2, SPI_NAND_CMD_PROG_LOAD_X4, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0),
};
#endif /* CONFIG_RTK_SPI_NAND_USE_DIO */

#ifdef CONFIG_RTK_SPI_NAND_USE_DIO
static struct spi_nand_info spi_nand_opcode_overwrite_table_dual[] = {
    OPCODE_OW(SPI_NAND_MFR_DOSILICON,      0xFFFF, 100, SPI_NAND_CMD_READ_FROM_CACHE_X2,      8, SPI_NAND_CMD_PROG_LOAD, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(SPI_NAND_MFR_ESMT_GD_ZENTEL, 0x31,   100, SPI_NAND_CMD_READ_FROM_CACHE_X2,      8, SPI_NAND_CMD_PROG_LOAD, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_ESMT_GD_ZENTEL, 0xFFFF, 100, SPI_NAND_CMD_READ_FROM_CACHE_DUAL_IO, 8, SPI_NAND_CMD_PROG_LOAD, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(SPI_NAND_MFR_MACRONIX,       0x12,   100, SPI_NAND_CMD_READ_FROM_CACHE_X2,      8, SPI_NAND_CMD_PROG_LOAD, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_MACRONIX,       0x22,   100, SPI_NAND_CMD_READ_FROM_CACHE_X2,      8, SPI_NAND_CMD_PROG_LOAD, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_MACRONIX,       0x26,   100, SPI_NAND_CMD_READ_FROM_CACHE_DUAL_IO, 4, SPI_NAND_CMD_PROG_LOAD, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),
    OPCODE_OW(SPI_NAND_MFR_MACRONIX,       0x37,   100, SPI_NAND_CMD_READ_FROM_CACHE_DUAL_IO, 4, SPI_NAND_CMD_PROG_LOAD, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(SPI_NAND_MFR_TOSHIBA,        0xFFFF, 100, SPI_NAND_CMD_READ_FROM_CACHE_X2,      8, SPI_NAND_CMD_PROG_LOAD, SPI_NAND_CMD_PROG_LOAD_RDM_DATA),

    OPCODE_OW(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0),
};
#endif /* CONFIG_RTK_SPI_NAND_USE_DIO */

/* OOB layout */
static struct nand_ecclayout micron_ecc_layout_64 = {
	.eccbytes = 32,
	.eccpos = {
		8, 9, 10, 11, 12, 13, 14, 15,
		24, 25, 26, 27, 28, 29, 30, 21,
		40, 41, 42, 43, 44, 45, 46, 47,
		56, 57, 58, 59, 60, 61, 62, 63},
	.oobavail = 30,
	.oobfree = {
		{.offset = 2,
		 .length = 6},
		{.offset = 16,
		 .length = 8},
		{.offset = 32,
		 .length = 8},
		{.offset = 48,
		 .length = 8}, }
};

static struct nand_ecclayout micron_ecc_layout_128 = {
	.eccbytes = 64,
	.eccpos = {
		64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 87,
		88, 89, 90, 91, 92, 93, 94, 95,
		96, 97, 98, 99, 100, 101, 102, 103,
		104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127},
	.oobavail = 62,
	.oobfree = {
		{.offset = 2,
		 .length = 62}, }
};

static struct nand_ecclayout gd5f_ecc_layout_64 = {
	.eccbytes = 64,
	.eccpos = {
		64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 87,
		88, 89, 90, 91, 92, 93, 94, 95,
		96, 97, 98, 99, 100, 101, 102, 103,
		104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127},
	.oobavail = 63,
	.oobfree = { {1, 63} }
};

static struct nand_ecclayout gd5f_ecc_layout_256 = {
	.eccbytes = 128,
	.eccpos = {
		128, 129, 130, 131, 132, 133, 134, 135,
		136, 137, 138, 139, 140, 141, 142, 143,
		144, 145, 146, 147, 148, 149, 150, 151,
		152, 153, 154, 155, 156, 157, 158, 159,
		160, 161, 162, 163, 164, 165, 166, 167,
		168, 169, 170, 171, 172, 173, 174, 175,
		176, 177, 178, 179, 180, 181, 182, 183,
		184, 185, 186, 187, 188, 189, 190, 191,
		192, 193, 194, 195, 196, 197, 198, 199,
		200, 201, 202, 203, 204, 205, 206, 207,
		208, 209, 210, 211, 212, 213, 214, 215,
		216, 217, 218, 219, 220, 221, 222, 223,
		224, 225, 226, 227, 228, 229, 230, 231,
		232, 233, 234, 235, 236, 237, 238, 239,
		240, 241, 242, 243, 244, 245, 246, 247,
		248, 249, 250, 251, 252, 253, 254, 255
	},
	.oobavail = 127,
	.oobfree = { {1, 127} }
};

static struct nand_ecclayout dummy_ecc_layout_64 = {
	.eccbytes = 64,
	.eccpos = {
		64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 87,
		88, 89, 90, 91, 92, 93, 94, 95,
		96, 97, 98, 99, 100, 101, 102, 103,
		104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127},
	.oobavail = 63,
	.oobfree = { {1, 63} }
};


/**
 * spi_nand_get_feature_register - send command 0Fh to read register
 * @chip: SPI-NAND device structure
 * @reg; register to read
 * @buf: buffer to store value
 */
static int spi_nand_get_feature_register(struct spi_nand_info *info,
			uint8_t reg, uint8_t *buf)
{
	struct spi_nand_cmd cmd;
	int ret;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = SPI_NAND_CMD_GET_FEATURE;
	cmd.n_addr = 1;
	cmd.addr[0] = reg;
	cmd.n_rx = 1;
	cmd.rx_buf = buf;

	ret = info->spinand_command_fn(&cmd);
	if (ret < 0)
		spinand_err("err: %d read register %d\n", ret, reg);

	return ret;
}

/**
 * spi_nand_set_feature_register - send command 1Fh to write register
 * @chip: SPI-NAND device structure
 * @reg; register to write
 * @buf: buffer stored value
 */
static int spi_nand_set_feature_register(struct spi_nand_info *info,
			uint8_t reg, uint8_t *buf)
{
	struct spi_nand_cmd cmd;
	int ret;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = SPI_NAND_CMD_SET_FEATURE;
	cmd.n_addr = 1;
	cmd.addr[0] = reg;
	cmd.n_tx = 1,
	cmd.tx_buf = buf,

	ret = info->spinand_command_fn(&cmd);
	if (ret < 0)
		spinand_err("err: %d write register %d\n", ret, reg);

	return ret;
}

/**
 * spi_nand_read_status - get status register value
 * @chip: SPI-NAND device structure
 * @status: buffer to store value
 * Description:
 *   After read, write, or erase, the Nand device is expected to set the
 *   busy status.
 *   This function is to allow reading the status of the command: read,
 *   write, and erase.
 *   Once the status turns to be ready, the other status bits also are
 *   valid status bits.
 */
static int spi_nand_read_status(struct spi_nand_info *info, uint8_t *status)
{
	return spi_nand_get_feature_register(info, REG_STATUS, status);
}

/**
 * spi_nand_get_cfg - get configuration register value
 * @chip: SPI-NAND device structure
 * @cfg: buffer to store value
 * Description:
 *   Configuration register includes OTP config, Lock Tight enable/disable
 *   and Internal ECC enable/disable.
 */
static int spi_nand_get_cfg(struct spi_nand_info *info, u8 *cfg)
{
	return spi_nand_get_feature_register(info, REG_CFG, cfg);
}

/**
 * spi_nand_set_cfg - set value to configuration register
 * @chip: SPI-NAND device structure
 * @cfg: buffer stored value
 * Description:
 *   Configuration register includes OTP config, Lock Tight enable/disable
 *   and Internal ECC enable/disable.
 */
static int spi_nand_set_cfg(struct spi_nand_info *info, u8 *cfg)
{
	return spi_nand_set_feature_register(info, REG_CFG, cfg);
}

/**
 * spi_nand_set_ds - set value to die select register
 * @chip: SPI-NAND device structure
 * @cfg: buffer stored value
 * Description:
 */
static int spi_nand_set_ds(struct spi_nand_info *info, u8 lun)
{
    struct spi_nand_cmd cmd;
    memset(&cmd, 0, sizeof(struct spi_nand_cmd));
    cmd.cmd = info->die_sel_op.cmd;
    cmd.n_addr = info->die_sel_op.n_addr;
    cmd.n_tx   = info->die_sel_op.n_data;

    if((info->die_sel_op.n_data==0)&&(info->die_sel_op.addr==0xFF)){
        cmd.addr[0] = info->die_sel_op.die[lun];
    }else if((info->die_sel_op.n_data==1)&&(info->die_sel_op.addr!=0xFF)){
        cmd.addr[0]= info->die_sel_op.addr;
        cmd.tx_buf = &info->die_sel_op.die[lun];
    }

    int ret = info->spinand_command_fn(&cmd);
	if (ret < 0){
        spinand_err("EE: %s cmd:0x%x die:%d\n", __func__, cmd.cmd, info->die_sel_op.die[lun]);
    }
    info->lun = lun;
    return ret;
}

/**
 * spi_nand_lun_select - send die select command if needed
 * @chip: SPI-NAND device structure
 * @lun: lun need to access
 */
static int spi_nand_lun_select(struct spi_nand_info *info, u8 lun)
{
	int ret = 0;

	if (info->lun != lun) {
		ret = spi_nand_set_ds(info, lun);
	}

	return ret;
}

/**
 * spi_nand_enable_ecc - enable internal ECC
 * @chip: SPI-NAND device structure
 * Description:
 *   There is one bit( bit 0x10 ) to set or to clear the internal ECC.
 *   Enable chip internal ECC, set the bit to 1
 *   Disable chip internal ECC, clear the bit to 0
 */
static int spi_nand_enable_ecc(struct spi_nand_info *info)
{
	u8 cfg = 0;
    if((info->mfr_id==SPI_NAND_MFR_FMSH)&&(info->dev_id==0x92)){
        spi_nand_get_feature_register(info, 0x90, &cfg);
        cfg |= CFG_ECC_ENABLE;
        return spi_nand_set_feature_register(info, 0x90, &cfg);
    }

	spi_nand_get_cfg(info, &cfg);
	if ((cfg & CFG_ECC_MASK) == CFG_ECC_ENABLE)
		return 0;
	cfg |= CFG_ECC_ENABLE;

	return spi_nand_set_cfg(info, &cfg);
}

/**
 * spi_nand_disable_ecc - disable internal ECC
 * @chip: SPI-NAND device structure
 * Description:
 *   There is one bit( bit 0x10 ) to set or to clear the internal ECC.
 *   Enable chip internal ECC, set the bit to 1
 *   Disable chip internal ECC, clear the bit to 0
 */
static int spi_nand_disable_ecc(struct spi_nand_info *info)
{
	u8 cfg = 0;
    if((info->mfr_id==SPI_NAND_MFR_FMSH)&&(info->dev_id==0x92)){
        spi_nand_get_feature_register(info, 0x90, &cfg);
        cfg &= ~CFG_ECC_ENABLE;
        return spi_nand_set_feature_register(info, 0x90, &cfg);
    }

	spi_nand_get_cfg(info, &cfg);
	if ((cfg & CFG_ECC_MASK) == CFG_ECC_ENABLE) {
		cfg &= ~CFG_ECC_ENABLE;
		return spi_nand_set_cfg(info, &cfg);
	}
	return 0;
}

/**
 * spi_nand_enable_quad_io - enable quad IO
 * @chip: SPI-NAND device structure
 * Description:
 *   There is one bit( bit 0x01 ) to set or to clear the quad IO.
 *   Macronix Nand flash need to set this bit to enable Quad IO.
 *   Enable chip quad IO, set the bit to 1
 *   Disable chip quad IO, clear the bit to 0
 */
static int spi_nand_enable_quad_io(struct spi_nand_info *info)
{
	u8 cfg = 0;
	spi_nand_get_cfg(info, &cfg);
	if ((cfg & CFG_QE_MASK) == CFG_QE_ENABLE)
		return 0;
	cfg |= CFG_QE_ENABLE;
	return spi_nand_set_cfg(info, &cfg);
}

#ifdef CONFIG_NAND_HIDE_BAD
#define BAD_BLOCK_LIST_ELEMENT 128
#define BBL_INDICATORS	       4
#define BBL0_NAME 0x42626c30 //'Bbl0'
#define BBL1_NAME 0x316C6242 //'1lbB'

enum {
	FACTORY_1ST_SCAN = 1,
	FACTORY_FROM_BBT0= 2,
    FACTORY_FROM_BBT1= 3,
    FACTORY_UPDATED  = 4, //For debug use.
};
uint8_t acc_phy_offset=0;
static uint8_t bbl_source=0;
static uint8_t bad_block_cnt=0;
static uint32_t bad_block_list[BAD_BLOCK_LIST_ELEMENT];

/*******************************************************
     After the first the factory bad block scanning
     Program the list to spi nand flash for later use
     The goal is to keep the original flash status and speed up booting

     This function will program two copy to flash in case.
     Each copy will be placed as:
         Page 0: Indicators for BBL
         Page 1: BBL content
*******************************************************/
static void store_bad_block_list_to_flash(struct spi_nand_info *info)
{
    size_t retlen;
    uint32_t crc16=crc16_ccitt(0, (uint8_t *)bad_block_list, sizeof(bad_block_list));
    uint32_t indicator[BBL_INDICATORS]={BBL0_NAME, bad_block_cnt, crc16, crc16};

    nand_acc_phy_offs_addr(0);

    spi_nand_erase(info, NAND_BBL_OFFSET, info->block_size);
    spi_nand_write(info, NAND_BBL_OFFSET, sizeof(indicator), &retlen, (const u8 *)indicator);
    spi_nand_write(info, NAND_BBL_OFFSET+info->page_size, sizeof(bad_block_list), &retlen, (const u8 *)bad_block_list);

    indicator[0] = BBL1_NAME;
    spi_nand_erase(info, NAND_BBL_OFFSET_REDUND, info->block_size);
    spi_nand_write(info, NAND_BBL_OFFSET_REDUND, sizeof(indicator), &retlen, (const u8 *)indicator);
    spi_nand_write(info, NAND_BBL_OFFSET_REDUND+info->page_size, sizeof(bad_block_list), &retlen, (const u8 *)bad_block_list);
}


static int xlat_offset_to_skip_factory_bad(struct spi_nand_info *info, uint32_t *page_addr)
{
	/* Just return original offset:
	     1. if no bad block found
	     2. Physical flash offset access is assigned */
	if((bad_block_cnt==0) || (acc_phy_offset)){
		return 0;
    }

	uint32_t i=0;
    uint32_t offset = *page_addr<<info->page_shift;
	for(i=0; i<bad_block_cnt; i++){
		if(bad_block_list[i] <= offset){
			offset += info->block_size;
		}else{
			break;
        }
	}

    if(offset >= info->size){
        return -EINVAL;
    }
    *page_addr = offset>>info->page_shift;
	return 0;
}


static void scan_spi_nand_factory_bad_blocks(struct spi_nand_info *info)
{
    /* First, check if the factory bad list is in flash chip?
         * 1) Read Bbt0 if the factory bad had already been scanned
         * 2) If Bbt0 is bad. Read Bbt1 for the back up copy
         */
    loff_t offset, start;
    size_t retlen, size;
    uint32_t ret=0;
    uint32_t indicator[BBL_INDICATORS];
    uint32_t tempread[BAD_BLOCK_LIST_ELEMENT];
    memset(indicator, 0xFF, sizeof(indicator));
    memset(tempread, 0xFF, sizeof(tempread));
    memset(bad_block_list, 0xFF, sizeof(bad_block_list));

    nand_acc_phy_offs_addr(1);
    for(offset=NAND_BBL_OFFSET ; offset<info->size ; offset+= info->block_size){
        spi_nand_read(info, offset, sizeof(indicator), &retlen, (u8 *)indicator);
        if(((indicator[0]==BBL0_NAME) || (indicator[0]==BBL1_NAME)) && (indicator[2]==indicator[3])){
            spi_nand_read(info, offset+info->page_size, sizeof(tempread), &retlen, (u8 *)tempread);
            if(indicator[2] == crc16_ccitt(0, (u8 *)tempread, sizeof(tempread))){
                bad_block_cnt = indicator[1];
                bbl_source = (indicator[0]==BBL0_NAME)?FACTORY_FROM_BBT0:FACTORY_FROM_BBT1;
                memcpy(bad_block_list, tempread, sizeof(bad_block_list));
                info->mtd->size = info->size - bad_block_cnt*info->block_size;
                goto found_list;
            }
        }
    }

    offset = 0;
    start = 0;
    size = info->size;
    bbl_source = FACTORY_1ST_SCAN;
    bad_block_cnt = 0;
    while(((offset - start) < size) && (bad_block_cnt<BAD_BLOCK_LIST_ELEMENT)) {
        ret = spi_nand_block_isbad(info, offset);
        if(ret == 1){
            bad_block_list[bad_block_cnt++] = offset;
        }
        offset += info->block_size;
    }
    info->mtd->size = info->size - bad_block_cnt*info->block_size;

    /* Fitst scan, store it into flash chip */
    store_bad_block_list_to_flash(info);

found_list:
    nand_acc_phy_offs_addr(0);
    return;
}

void update_factory_bad_block_list(/*uint32_t offset, uint32_t is_add, */struct spi_nand_info *info)
{
    scan_spi_nand_factory_bad_blocks(info);
    #if 0
    uint32_t i=0;
    uint32_t local_buf[BAD_BLOCK_LIST_ELEMENT];
    memcpy(local_buf, bad_block_list, sizeof(local_buf));

    if(is_add){
        if(!bad_block_cnt){
            bad_block_list[bad_block_cnt++] = offset;
            goto end;
        }
        while(1){
            if(offset < bad_block_list[i]){
                bad_block_list[i] = offset;
                memcpy(&bad_block_list[i+1], &local_buf[i], sizeof(uint32_t)*(bad_block_cnt-i+1));
                bad_block_cnt++;
                goto end;
            }else if(offset == bad_block_list[i]){
                return;
            }else if((i+1)==bad_block_cnt){
                bad_block_list[i+1] = offset;
                bad_block_cnt++;
                goto end;
            }else{
                i++;
            }
        }
    }else{
        while(i<bad_block_cnt){
            if(offset != bad_block_list[i]){
                i++;
            }else{
                memcpy(&bad_block_list[i], &local_buf[i+1], sizeof(uint32_t)*(bad_block_cnt-i-1));
                bad_block_cnt--;
                bad_block_list[bad_block_cnt] = 0xFFFFFFFF;
                goto end;
            }
        }
    }

end:
    bbl_source = FACTORY_UPDATED;
    info->mtd->size = info->size - bad_block_cnt*info->block_size;

    /* Update the latest one into flash chip */
    store_bad_block_list_to_flash(info);
#endif
    return;
}

uint32_t spi_nand_get_phy_offset(struct mtd_info *mtd, uint32_t viraddr)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct spi_nand_info *info = (struct spi_nand_info *)chip->priv;
    uint32_t page_addr = viraddr>>info->page_shift;
    xlat_offset_to_skip_factory_bad(info, &page_addr);

    return page_addr<<info->page_shift;
}

void spi_nand_print_bad_block_hidden_list(void)
{
    if(bbl_source==FACTORY_1ST_SCAN)
        puts("      BBL: Factory bad first scan\n");
    else if(bbl_source==FACTORY_UPDATED)
        printf("      BBL: Updated during debugging\n");
    else
        printf("      BBL: %s from flash\n",(bbl_source==FACTORY_FROM_BBT0)?"Bbl0":"Bbl1");

    puts("   Hidden: ");
    uint32_t i;
    for(i=0 ; i<bad_block_cnt ; i++){
        printf("0x%x ", bad_block_list[i]);
    }
    puts("\n");
    return;
}
#endif /* #ifdef CONFIG_NAND_HIDE_BAD */

/**
 * spi_nand_write_enable - send command 06h to enable write or erase the
 * Nand cells
 * @chip: SPI-NAND device structure
 * Description:
 *   Before write and erase the Nand cells, the write enable has to be set.
 *   After the write or erase, the write enable bit is automatically
 *   cleared (status register bit 2)
 *   Set the bit 2 of the status register has the same effect
 */
static int spi_nand_write_enable(struct spi_nand_info *info)
{
	struct spi_nand_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = SPI_NAND_CMD_WR_ENABLE;

	return info->spinand_command_fn(&cmd);
}

/**
 * spi_nand_read_page_to_cache - send command 13h to read data from Nand to cache
 * @chip: SPI-NAND device structure
 * @page_addr: page to read
 */
static int spi_nand_read_page_to_cache(struct spi_nand_info *info,
					uint32_t page_addr)
{
	struct spi_nand_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = SPI_NAND_CMD_PAGE_READ;
	cmd.n_addr = 3;
	cmd.addr[0] = (u8)(page_addr >> 16);
	cmd.addr[1] = (u8)(page_addr >> 8);
	cmd.addr[2] = (u8)page_addr;

	return info->spinand_command_fn(&cmd);
}

/**
 * spi_nand_read_from_cache - read data out from cache register
 * @chip: SPI-NAND device structure
 * @page_addr: page to read
 * @column: the location to read from the cache
 * @len: number of bytes to read
 * @rbuf: buffer held @len bytes
 * Description:
 *   Command can be 03h, 0Bh, 3Bh, 6Bh, BBh, EBh
 *   The read can specify 1 to (page size + spare size) bytes of data read at
 *   the corresponding locations.
 *   No tRd delay.
 */
static int spi_nand_read_from_cache(struct spi_nand_info *info, uint32_t page_addr,
		uint32_t column, size_t len, u8 *rbuf)
{
	struct spi_nand_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = info->read_cache_op;
	cmd.n_addr = 2;
	cmd.addr[0] = (u8)(column >> 8);
	if (info->options & SPI_NAND_NEED_PLANE_SELECT){
		cmd.addr[0] |= (u8)(((page_addr >>(info->block_shift - info->page_shift)) & 0x1) << 4);
	}
	cmd.addr[1] = (u8)column;
    cmd.n_dummy_cycle = info->dummy_cycle;
	cmd.n_rx = len;
	cmd.rx_buf = rbuf;

    cmd.io_addr = 1;
    cmd.io_data = 1;
    if(cmd.cmd == SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO){
        cmd.io_addr =4;
        cmd.io_data = 4;
    }else if(cmd.cmd == SPI_NAND_CMD_READ_FROM_CACHE_DUAL_IO){
        cmd.io_addr = 2;
        cmd.io_data = 2;
    }else if(cmd.cmd == SPI_NAND_CMD_READ_FROM_CACHE_X4){
        cmd.io_data = 4;
    }else if(cmd.cmd == SPI_NAND_CMD_READ_FROM_CACHE_X2){
        cmd.io_data = 2;
    }
	return info->spinand_command_fn(&cmd);
}

/**
 * spi_nand_program_data_to_cache - write data to cache register
 * @chip: SPI-NAND device structure
 * @page_addr: page to write
 * @column: the location to write to the cache
 * @len: number of bytes to write
 * @wrbuf: buffer held @len bytes
 * @clr_cache: clear cache register or not
 * Description:
 *   Command can be 02h, 32h, 84h, 34h
 *   02h and 32h will clear the cache with 0xff value first
 *   Since it is writing the data to cache, there is no tPROG time.
 */
static int spi_nand_program_data_to_cache(struct spi_nand_info *info,
		uint32_t page_addr, uint32_t column, size_t len, const u8 *wbuf, bool clr_cache)
{
	struct spi_nand_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	if (clr_cache)
		cmd.cmd = info->write_cache_op;
	else
		cmd.cmd = info->write_cache_rdm_op;
	cmd.n_addr = 2;
	cmd.addr[0] = (u8)(column >> 8);
	if (info->options & SPI_NAND_NEED_PLANE_SELECT)
		cmd.addr[0] |= (u8)(((page_addr >>
			(info->block_shift - info->page_shift)) & 0x1) << 4);
	cmd.addr[1] = (u8)column;
	cmd.n_tx = len;
	cmd.tx_buf = wbuf;

	return info->spinand_command_fn(&cmd);
}


/**
 * spi_nand_program_execute - send command 10h to write a page from
 * cache to the Nand array
 * @chip: SPI-NAND device structure
 * @page_addr: the physical page location to write the page.
 * Description:
 *   Need to wait for tPROG time to finish the transaction.
 */
static int spi_nand_program_execute(struct spi_nand_info *info, uint32_t page_addr)
{
	struct spi_nand_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = SPI_NAND_CMD_PROG_EXC;
	cmd.n_addr = 3;
	cmd.addr[0] = (u8)(page_addr >> 16);
	cmd.addr[1] = (u8)(page_addr >> 8);
	cmd.addr[2] = (u8)page_addr;

	return info->spinand_command_fn(&cmd);
}


/**
 * spi_nand_erase_one_block - send command D8h to erase a block
 * @chip: SPI-NAND device structure
 * @page_addr: the page to erase.
 * Description:
 *   Need to wait for tERS.
 */
static int spi_nand_erase_one_block(struct spi_nand_info *info, uint32_t page_addr)
{
	struct spi_nand_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = SPI_NAND_CMD_BLK_ERASE;
	cmd.n_addr = 3;
	cmd.addr[0] = (u8)(page_addr >> 16);
	cmd.addr[1] = (u8)(page_addr >> 8);
	cmd.addr[2] = (u8)page_addr;

	return info->spinand_command_fn(&cmd);
}

/**
 * spi_nand_read_page_cache_random - send command 30h for data read
 * @chip: SPI-NAND device structure
 * @page_addr: the page to read to data register.
 * Description:
 *   Transfer data from data register to cache register and kick off the other
 *   page data transferring from array to data register.
 */
static int spi_nand_read_page_cache_random(struct spi_nand_info *info,
					uint32_t page_addr)
{
	struct spi_nand_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = SPI_NAND_CMD_READ_PAGE_CACHE_RDM;
	cmd.addr[0] = (u8)(page_addr >> 16);
	cmd.addr[1] = (u8)(page_addr >> 8);
	cmd.addr[2] = (u8)page_addr;
	cmd.n_addr = 3;

	return info->spinand_command_fn(&cmd);
}

/**
 * spi_nand_read_page_cache_last - send command 3Fh to end
 * READ PAGE CACHE RANDOM(30h) sequence
 * @chip: SPI-NAND device structure
 * Description:
 *   End the READ PAGE CACHE RANDOM sequence and copies a page from
 *   the data register to the cache register.
 */
static int spi_nand_read_page_cache_last(struct spi_nand_info *info)
{
	struct spi_nand_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = SPI_NAND_CMD_READ_PAGE_CACHE_LAST;

	return info->spinand_command_fn(&cmd);
}

/**
 * spi_nand_wait - wait until the command is done
 * @chip: SPI-NAND device structure
 * @s: buffer to store status register(can be NULL)
 */
static int spi_nand_wait(struct spi_nand_info *info, u8 *s)
{
	unsigned long long start = get_timer(0);;
	u8 status;
	/* set timeout to 1 second */
	int timeout = start + CONFIG_SYS_HZ;
	unsigned long ret = -ETIMEDOUT;
	int count = 0;

	while (get_timer(start) < timeout || count < MIN_TRY_COUNT) {
		spi_nand_read_status(info, &status);
		if ((status & STATUS_OIP_MASK) == STATUS_READY) {
			ret = 0;
			goto out;
		}
		count++;
	}
out:
	if (s)
		*s = status;

	return ret;
}

/**
 * spi_nand_wait_crbusy - wait until CRBSY is clear
 * @chip: SPI-NAND device structure
 * Description:
 *   Used in READ PAGE CACHE RANDOM(30h) sequence, CRBSY bit clear
 *   means data is transferd from data register to cache register.
 */
static int spi_nand_wait_crbusy(struct spi_nand_info *info)
{
	unsigned long long start = get_timer(0);;
	u8 status;
	/* set timeout to 1 second */
	int timeout = start + CONFIG_SYS_HZ;
	unsigned long ret = -ETIMEDOUT;
	int count = 0;

	while (get_timer(start) < timeout || count < MIN_TRY_COUNT) {
		spi_nand_read_status(info, &status);
		if ((status & STATUS_CRBSY_MASK) == STATUS_READY) {
			ret = 0;
			goto out;
		}
		count++;
	}
out:
	return ret;
}

/**
 * spi_nand_read_id - send 9Fh command to get ID
 * @chip: SPI-NAND device structure
 * @buf: buffer to store id
 */
static int spi_nand_read_id(struct spi_nand_info *info, u8 *buf)
{
	struct spi_nand_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = SPI_NAND_CMD_READ_ID;
	cmd.n_addr = 1;
	cmd.n_dummy_cycle = 0;
	cmd.addr[0]= 0;
	cmd.n_rx = 3;
	cmd.rx_buf = buf;

	return info->spinand_command_fn(&cmd);
}

/**
 * spi_nand_reset - send command FFh to reset chip.
 * @chip: SPI-NAND device structure
 */
static int spi_nand_reset(struct spi_nand_info *info)
{
	struct spi_nand_cmd cmd;

	memset(&cmd, 0, sizeof(struct spi_nand_cmd));
	cmd.cmd = SPI_NAND_CMD_RESET;

	if (info->spinand_command_fn(&cmd) < 0)
		spinand_err("spi_nand reset failed!\n");

	/* elapse 2ms before issuing any other command */
	udelay(2000);

	return 0;
}

/**
 * spi_nand_lock_block - write block lock register to
 * lock/unlock device
 * @spi: spi device structure
 * @lock: value to set to block lock register
 * Description:
 *   After power up, all the Nand blocks are locked.  This function allows
 *   one to unlock the blocks, and so it can be written or erased.
 */
static int spi_nand_lock_block(struct spi_nand_info *info, u8 lock)
{
	int ret = spi_nand_set_feature_register(info, REG_BLOCK_LOCK, &lock);
    u8 read_val = 0;
    spi_nand_get_feature_register(info, REG_BLOCK_LOCK, &read_val);
    if(read_val != lock){
        printf("EE: #Die_%d block %s Fail!! (=0x%x)\n",info->lun, (lock==0?"Unlock":"Lock"),read_val);
        return -1;
    }
	return ret;
}

/**
 * spi_nand_change_mode - switch chip to OTP/OTP protect/Normal mode
 * @chip: SPI-NAND device structure
 * @mode: mode to enter
 */
static int spi_nand_change_mode(struct spi_nand_info *info, u8 mode, u8 *cur_cfg)
{
	u8 cfg;

	spi_nand_get_cfg(info, &cfg);
	switch (mode) {
	case OTP_MODE:
	    *cur_cfg = cfg;
		cfg = CFG_OTP_ENTER;
		break;
	case NORMAL_MODE:
		cfg = *cur_cfg;
		break;
	}
	spi_nand_set_cfg(info, &cfg);

	return 0;
}

/**
 * spi_nand_do_read_page - read page from flash to buffer
 * @chip: spi nand chip structure
 * @page_addr: page address/raw address
 * @column: column address
 * @ecc_off: without ecc or not
 * @corrected: how many bit error corrected
 * @buf: data buffer
 * @len: data length to read
 * Description:
 *   Return -EBADMSG when internal ecc can not correct bitflips.
 *   The command sequence to transfer data from NAND array to output is
 *   follows:
 *      13h (PAGE READ to cache register)
 *      0Fh (GET FEATURES command to read the status)
 *      0Bh/03h/3Bh/6Bh (Read from Cache Xn); or BBh/EBh (Read From
 *      Cache Dual/Quad IO)
 */
static int spi_nand_do_read_page(struct spi_nand_info *info, uint32_t page_addr,
				uint32_t column, bool ecc_off, unsigned int *corrected, u_char *buf, size_t len)
{
	int ret;
	unsigned int ecc_error = 0;
	u8 status;
	int lun_num=0, from;

#ifdef CONFIG_NAND_HIDE_BAD
    uint32_t pa_be4xlat = page_addr;
    ret = xlat_offset_to_skip_factory_bad(info, &page_addr);
    if(ret == -EINVAL){
        spinand_err("%s: offset xlat exceeds chip size\n",__FUNCTION__);
        return ret;
    }
    if(pa_be4xlat != page_addr){
        spinand_debug("read xlat: 0x%x => 0x%x\n",pa_be4xlat,page_addr);
    }
#endif

	if (info->options & SPI_NAND_NEED_DIE_SELECT){
        from = (page_addr<<info->page_shift);
        lun_num = from >> info->lun_shift;
		spi_nand_lun_select(info, lun_num);
        if(lun_num){
            page_addr = (from&info->lun_mask)>>info->page_shift;
        }
    }
	if (ecc_off)
		info->disable_ecc(info);
	spi_nand_read_page_to_cache(info, page_addr);
	ret = spi_nand_wait(info, &status);
	if (ret < 0) {
#ifdef CONFIG_NAND_HIDE_BAD
        spinand_err("waiting Die#%d page#0x%x timeout (0x%x): spi_nand_read_page_to_cache\n", lun_num, pa_be4xlat, status);
#else
		spinand_err("waiting Die#%d page#0x%x timeout (0x%x): spi_nand_read_page_to_cache\n", lun_num, page_addr, status);
#endif
		return ret;
	}
	if (!ecc_off) {
		info->get_ecc_status(info, status, corrected, &ecc_error);
		if (ecc_error) {
#ifdef CONFIG_NAND_HIDE_BAD
            spinand_err("internal ECC error reading page 0x%x (status=0x%x)\n", pa_be4xlat, status);
#else
            spinand_err("internal ECC error reading page 0x%x\n", page_addr);
#endif
			ret = -EBADMSG;
		} else if (*corrected) {
            if (*corrected == 0xFF) {
                spinand_info("SPI_NAND: Unknown ECC criteria, status=0x%x, pageid=%d\n",status, page_addr);
            } else {
#ifdef CONFIG_NAND_HIDE_BAD
                spinand_info("internal ECC corrected %d bits, status=0x%x, pageid=%d\n",*corrected, status, pa_be4xlat);
#else
                spinand_info("internal ECC corrected %d bits, status=0x%x, pageid=%d\n",*corrected, status, page_addr);
#endif
            }
		}
	}else
		info->enable_ecc(info);
	spi_nand_read_from_cache(info, page_addr, column, len, buf);
	return ret;
}

/**
 * spi_nand_do_write_page - write data from buffer to flash
 * @chip: spi nand chip structure
 * @page_addr: page address/raw address
 * @column: column address
 * @buf: data buffer
 * @len: data length to write
 * @clr_cache: clear cache register with 0xFF or not
 * Description:
 *   Page program sequence is as follows:
 *       06h (WRITE ENABLE)
 *       02h/32h/84h/34h (PROGRAM LOAD (RAMDOM_DATA) Xn)
 *       10h (PROGRAM EXECUTE)
 *       0Fh (GET FEATURE command to read the status)
 *   PROGRAM LOAD Xn instruction will reset the cache resigter with 0xFF,
 *   while PROGRAM LOAD RANDOM DATA Xn instruction will only update the
 *   data bytes that are specified by the command input sequence and the rest
 *   of data in the cache buffer will remain unchanged.
 */
static int spi_nand_do_write_page(struct spi_nand_info *info, uint32_t page_addr,
				uint32_t column, const u_char *buf, size_t len, bool ecc_off, bool clr_cache)
{
	u8 status;
	bool p_fail = false;
	int ret = 0;
    int lun_num=0, from;
#ifdef CONFIG_NAND_HIDE_BAD
    uint32_t pa_be4xlat = page_addr;
    ret = xlat_offset_to_skip_factory_bad(info, &page_addr);

    if(ret == -EINVAL){
        spinand_err("%s: offset xlat exceeds chip size\n",__FUNCTION__);
        return ret;
    }
    if(pa_be4xlat != page_addr){
        spinand_debug("write xlat: 0x%x => 0x%x\n",pa_be4xlat,page_addr);
    }
#endif

	if (info->options & SPI_NAND_NEED_DIE_SELECT){
        from = (page_addr<<info->page_shift);
        lun_num = from >> info->lun_shift;
		spi_nand_lun_select(info, lun_num);
        if(lun_num){
            page_addr = (from&info->lun_mask)>>info->page_shift;
        }
    }
	if (ecc_off)
		info->disable_ecc(info);

	spi_nand_write_enable(info);
	spi_nand_program_data_to_cache(info, page_addr,
					column, len, buf, clr_cache);
	spi_nand_program_execute(info, page_addr);
	ret = spi_nand_wait(info, &status);
	if (ret < 0) {
#ifdef CONFIG_NAND_HIDE_BAD
		spinand_err("waiting Die#%d page#0x%x timeout (0x%x): spi_nand_program_execute\n", lun_num, pa_be4xlat, status);
#else
        spinand_err("waiting Die#%d page#0x%x timeout (0x%x): spi_nand_program_execute\n", lun_num, page_addr, status);
#endif
		return ret;
	}
	if ((status & STATUS_P_FAIL_MASK) == STATUS_P_FAIL) {
#ifdef CONFIG_NAND_HIDE_BAD
		spinand_err("program Die#%d page#0x%x failed\n", lun_num, pa_be4xlat);
#else
        spinand_err("program Die#%d page#0x%x failed\n", lun_num, page_addr);
#endif
		p_fail = true;
	}
	if (p_fail)
		ret = -EIO;

	if (ecc_off)
		info->enable_ecc(info);

	return ret;
}

/**
 * spi_nand_transfer_oob - transfer oob to client buffer
 * @chip: SPI-NAND device structure
 * @oob: oob destination address
 * @ops: oob ops structure
 * @len: size of oob to transfer
 */
static void spi_nand_transfer_oob(struct spi_nand_info *info, u8 *oob,
				  struct mtd_oob_ops *ops, size_t len)
{
	switch (ops->mode) {

	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_RAW:
		memcpy(oob, info->oobbuf + ops->ooboffs, len);
		return;

	case MTD_OPS_AUTO_OOB: {
		struct nand_oobfree *free = info->ecclayout->oobfree;
		uint32_t boffs = 0, roffs = ops->ooboffs;
		size_t bytes = 0;

		for (; free->length && len; free++, len -= bytes) {
			/* Read request not from offset 0? */
			if (unlikely(roffs)) {
				if (roffs >= free->length) {
					roffs -= free->length;
					continue;
				}
				boffs = free->offset + roffs;
				bytes = min_t(size_t, len,
					      (free->length - roffs));
				roffs = 0;
			} else {
				bytes = min_t(size_t, len, free->length);
				boffs = free->offset;
			}
			memcpy(oob, info->oobbuf + boffs, bytes);
			oob += bytes;
		}
		return;
	}
	default:
		BUG();
	}
}

/**
 * spi_nand_fill_oob - transfer client buffer to oob
 * @chip: SPI-NAND device structure
 * @oob: oob data buffer
 * @len: oob data write length
 * @ops: oob ops structure
 */
static void spi_nand_fill_oob(struct spi_nand_info *info, uint8_t *oob,
				size_t len, struct mtd_oob_ops *ops)
{
	memset(info->oobbuf, 0xff, info->oob_size);

	switch (ops->mode) {

	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_RAW:
		memcpy(info->oobbuf + ops->ooboffs, oob, len);
		return;

	case MTD_OPS_AUTO_OOB: {
		struct nand_oobfree *free = info->ecclayout->oobfree;
		uint32_t boffs = 0, woffs = ops->ooboffs;
		size_t bytes = 0;

		for (; free->length && len; free++, len -= bytes) {
			/* Write request not from offset 0? */
			if (unlikely(woffs)) {
				if (woffs >= free->length) {
					woffs -= free->length;
					continue;
				}
				boffs = free->offset + woffs;
				bytes = min_t(size_t, len,
					      (free->length - woffs));
				woffs = 0;
			} else {
				bytes = min_t(size_t, len, free->length);
				boffs = free->offset;
			}
			memcpy(info->oobbuf + boffs, oob, bytes);
			oob += bytes;
		}
		return;
	}
	default:
		BUG();
	}
}

/**
 * spi_nand_read_pages - read data from flash to buffer
 * @chip: spi nand chip structure
 * @from: offset to read from
 * @ops: oob operations description structure
 * Description:
 *   Normal read function, read one page to buffer before issue
 *   another. Return -EUCLEAN when bitflip is over threshold.
 *   Return -EBADMSG when internal ecc can not correct bitflips.
 */
static int spi_nand_read_pages(struct spi_nand_info *info,
			loff_t from, struct mtd_oob_ops *ops)
{
	int page_addr, page_offset, size;
	int ret;
	unsigned int corrected = 0;
	unsigned int max_bitflip = 0;
	int readlen = ops->len;
	/* int oobreadlen = ops->ooblen; */// 2019/10/15
	bool ecc_off = ops->mode == MTD_OPS_RAW;
	/*int ooblen = (ops->mode == MTD_OPS_AUTO_OOB) ? info->ecclayout->oobavail : info->oob_size; */// 2019/10/15
	unsigned int failed = 0;

	spinand_debug("%s: from = 0x%012llx, len = %i\n",
			 __func__, from, readlen);

	page_addr = from >> info->page_shift;
	page_offset = from & info->page_mask;
	ops->retlen = 0;

	while (1) {
		WATCHDOG_RESET();
		size = min(readlen, info->page_size - page_offset);
		ret = spi_nand_do_read_page(info, page_addr, page_offset,
					ecc_off, &corrected, ops->datbuf + ops->retlen, size);
		if (ret == -EBADMSG)
			failed++;
		else if (ret) {
			spinand_err("error %d reading page 0x%x\n",
				ret, page_addr);
			goto out;
		}

		max_bitflip = max(corrected, max_bitflip);

		ops->retlen += size;
		readlen -= size;
		page_offset = 0;

		if (unlikely(ops->oobbuf)) {
            spi_nand_do_read_oob(info, from, ops);

            /* 2019/10/15: Read to Cache and Read from cache are combo set for some vendors.
			size = min(oobreadlen, ooblen);
			spi_nand_read_from_cache(info, page_addr,
				info->page_size, info->oob_size, info->oobbuf);
			spi_nand_transfer_oob(info,
				ops->oobbuf + ops->oobretlen, ops, size);
			ops->oobretlen += size;
			oobreadlen -= size;
                      */
		}
		if (!readlen)
			break;

		page_addr++;
	}
out:
	if (max_bitflip >= info->refresh_threshold)
		ret = -EUCLEAN;
	if (failed)
		ret = -EBADMSG;

	return ret;
}

/**
 * spi_nand_read_pages_fast - read data from flash to buffer
 * @chip: spi nand chip structure
 * @from: offset to read from
 * @ops: oob operations description structure
 * Description:
 *   Advanced read function, use READ PAGE CACHE RANDOM to
 *   speed up read. Return -EUCLEAN when bitflip is over threshold.
 *   Return -EBADMSG when internal ecc can not correct bitflips.
 */
static int spi_nand_read_pages_fast(struct spi_nand_info *info,
			loff_t from, struct mtd_oob_ops *ops)
{
	int page_addr, page_offset, size;
	int ret;
	unsigned int corrected = 0;
	unsigned int max_bitflip = 0;
	int readlen = ops->len;
	int oobreadlen = ops->ooblen;
	bool ecc_off = ops->mode == MTD_OPS_RAW, cross_lun = false;
	bool read_ramdon_issued = false;
	int ooblen = (ops->mode == MTD_OPS_AUTO_OOB) ? info->ecclayout->oobavail : info->oob_size;
	u8 status;
	unsigned int ecc_error;
	unsigned int failed = 0;
	int lun_num;

	page_addr = from >> info->page_shift;
	page_offset = from & info->page_mask;
	ops->retlen = 0;
	lun_num = from >> info->lun_shift;
again:
	spi_nand_read_page_to_cache(info, page_addr);
	ret = spi_nand_wait(info, &status);
	if (ret < 0) {
		spinand_err("waiting page 0x%x timeout (0x%x): spi_nand_read_page_to_cache\n", status, page_addr);
		return ret;
	}
	while ((page_offset + readlen > info->page_size) && !cross_lun) {
		WATCHDOG_RESET();
		if (!(info->options & SPI_NAND_NEED_DIE_SELECT) ||
		(page_addr + 1) & ((1 << (info->lun_shift - info->page_shift)) - 1)) {
			read_ramdon_issued = true;
			spi_nand_read_page_cache_random(info, page_addr + 1);
			ret = spi_nand_wait(info, &status);
			if (ret < 0) {
                spinand_err("waiting page 0x%x timeout (0x%x): spi_nand_read_page_cache_random\n", status, page_addr);
				return ret;
			}
		} else {
			cross_lun = true;
			break;
		}
		if (!ecc_off) {
			info->get_ecc_status(info, status, &corrected, &ecc_error);
			if (ecc_error) {
                spinand_err("internal ECC error reading page 0x%x\n", page_addr);
				failed++;
			}
		}
		max_bitflip = max(corrected, max_bitflip);
		size = min(readlen, info->page_size - page_offset);
		spi_nand_read_from_cache(info, page_addr, page_offset, size,
				ops->datbuf + ops->retlen);
		page_offset = 0;
		ops->retlen += size;
		readlen -= size;
		if (unlikely(ops->oobbuf)) {
			size = min(oobreadlen, ooblen);
			spi_nand_read_from_cache(info, page_addr,
				info->page_size, info->oob_size, info->oobbuf);
			spi_nand_transfer_oob(info,
				ops->oobbuf + ops->oobretlen, ops, size);
			ops->oobretlen += size;
			oobreadlen -= size;
		}
		if (!cross_lun) {
			ret = spi_nand_wait_crbusy(info);
			if (ret < 0) {
				spinand_err("error %d waiting page 0x%x to cache\n",
					ret, page_addr + 1);
				return ret;
			}
		}
		page_addr++;
	}
	if (read_ramdon_issued) {
		read_ramdon_issued = false;
		spi_nand_read_page_cache_last(info);
		/*
		* Already check ecc status in loop, no need to check again
		*/
		ret = spi_nand_wait(info, &status);
		if (ret < 0) {
            spinand_err("waiting page 0x%x timeout (0x%x): spi_nand_read_page_cache_last\n", status, page_addr);
			return ret;
		}
	}
	if (!ecc_off) {
		info->get_ecc_status(info, status, &corrected, &ecc_error);
		if (ecc_error) {
			spinand_err("internal ECC error reading page 0x%x\n", page_addr);
			failed++;
		}
	}
	max_bitflip = max(corrected, max_bitflip);
	size = min(readlen, info->page_size - page_offset);
	spi_nand_read_from_cache(info, page_addr, page_offset, size,
			ops->datbuf + ops->retlen);
	ops->retlen += size;
	readlen -= size;
	if (unlikely(ops->oobbuf)) {
		size = min(oobreadlen, ooblen);
		spi_nand_read_from_cache(info, page_addr,
			info->page_size, info->oob_size, info->oobbuf);
		spi_nand_transfer_oob(info,
			ops->oobbuf + ops->oobretlen, ops, size);
		ops->oobretlen += size;
		oobreadlen -= size;
	}
	if (cross_lun) {
		cross_lun = false;
		page_addr++;
		page_offset = 0;
		lun_num++;
		goto again;
	}
	if (max_bitflip >= info->refresh_threshold)
		ret = -EUCLEAN;
	if (failed)
		ret = -EBADMSG;

	return ret;
}

static inline bool is_read_page_fast_benefit(struct spi_nand_info *info,
			loff_t from, size_t len)
{
	return false;
	/* Alway return false!!! */

	if (len < info->page_size << 2)
		return false;
	if (from >> info->lun_shift == (from + len) >> info->lun_shift)
		return true;
	if (((1 << info->lun_shift) - from) >= (info->page_size << 2) ||
		(from + len - (1 << info->lun_shift)) >= (info->page_size << 2))
		return true;
	return false;
}

/**
 * spi_nand_do_read_ops - read data from flash to buffer
 * @chip: spi nand device structure
 * @from: offset to read from
 * @ops: oob ops structure
 * Description:
 *   Disable internal ECC before reading when MTD_OPS_RAW set.
 */
static int spi_nand_do_read_ops(struct spi_nand_info *info, loff_t from,
			  struct mtd_oob_ops *ops)
{
	int ret;
	int oobreadlen = ops->ooblen;
	int ooblen = (ops->mode == MTD_OPS_AUTO_OOB) ?
		info->ecclayout->oobavail : info->oob_size;

	spinand_debug("%s: from = 0x%012llx, len = %i\n",
			 __func__, from, ops->len);
	/* Do not allow reads past end of device */
	if (unlikely(from >= info->size)) {
		spinand_err("%s: attempt to read beyond end of device\n",
				__func__);
		return -EINVAL;
	}

	/* for oob */
	if (oobreadlen > 0) {
		if (unlikely(ops->ooboffs >= ooblen)) {
			spinand_err("%s: attempt to start read outside oob\n",
					__func__);
			return -EINVAL;
		}

		if (unlikely(ops->ooboffs + oobreadlen >
		((info->size >> info->page_shift) - (from >> info->page_shift))
		* ooblen)) {
			spinand_err("%s: attempt to read beyond end of device\n",
					__func__);
			return -EINVAL;
		}
		ooblen -= ops->ooboffs;
		ops->oobretlen = 0;
	}

	if (is_read_page_fast_benefit(info, from, ops->len))
		ret = spi_nand_read_pages_fast(info, from, ops);
	else
		ret = spi_nand_read_pages(info, from, ops);

	return ret;
}

/**
 * spi_nand_do_write_ops - write data from buffer to flash
 * @chip: spi nand device structure
 * @to: offset to write to
 * @ops: oob operations description structure
 * Description:
 *   Disable internal ECC before writing when MTD_OPS_RAW set.
 */
static int spi_nand_do_write_ops(struct spi_nand_info *info, loff_t to,
			 struct mtd_oob_ops *ops)
{
	int page_addr, page_offset, size, size_d, size_o;
	int writelen = ops->len;
	int oobwritelen = ops->ooblen;
	int ret = 0;
	int ooblen = (ops->mode == MTD_OPS_AUTO_OOB) ?
		info->ecclayout->oobavail : info->oob_size;
	bool ecc_off = ops->mode == MTD_OPS_RAW;
	bool clr_cache = true;
    u8 local_buf[info->page_size+info->oob_size];

	spinand_debug("%s: to = 0x%012llx, len = %i\n",
			 __func__, to, writelen);
	/* Do not allow reads past end of device */
	if (unlikely(to >= info->size)) {
		spinand_err("%s: attempt to write beyond end of device\n",
				__func__);
		return -EINVAL;
	}

	page_addr = to >> info->page_shift;
	page_offset = to & info->page_mask;
	ops->retlen = 0;

	/* for oob */
	if (oobwritelen > 0) {
		if (unlikely(ops->ooboffs >= ooblen)) {
			spinand_err("%s: attempt to start write outside oob\n",
					__func__);
			return -EINVAL;
		}
		if (unlikely(ops->ooboffs + oobwritelen >
		((info->size >> info->page_shift) - (to >> info->page_shift))
			* ooblen)) {
			spinand_err("%s: attempt to write beyond end of device\n",
					__func__);
			return -EINVAL;
		}
		ooblen -= ops->ooboffs;
		ops->oobretlen = 0;
	}

	while (1) {
		WATCHDOG_RESET();
        if ((unlikely(ops->oobbuf))&&(unlikely(ops->datbuf))){
            memset(local_buf, 0xff, sizeof(local_buf));

            size_d = min(writelen, info->page_size - page_offset);
            memcpy(local_buf, ops->datbuf + ops->retlen, size_d);
            ops->retlen += size_d;

            size_o = min(oobwritelen, ooblen);;
            memcpy(local_buf+info->page_size, ops->oobbuf + ops->oobretlen, size_o);
            ops->oobretlen += size_o;

            size = size_d + size_o;
            ret = spi_nand_do_write_page(info, page_addr, page_offset,
                    local_buf, size, ecc_off, clr_cache);
            if (ret) {
                spinand_err("error %d writing page 0x%x\n",
                    ret, page_addr);
                goto out;
            }
            writelen -= size_d;
            oobwritelen -= size_o;
            page_offset = 0;
            if (!writelen)
                break;
            page_addr++;
        }else{
    		size = min(writelen, info->page_size - page_offset);
    		ret = spi_nand_do_write_page(info, page_addr, page_offset,
    				ops->datbuf + ops->retlen, size, ecc_off, clr_cache);
    		if (ret) {
    			spinand_err("error %d writing page 0x%x\n",
    				ret, page_addr);
    			goto out;
    		}
    		ops->retlen += size;
    		writelen -= size;
    		page_offset = 0;
    		if (!writelen)
    			break;
    		page_addr++;
	    }
	}
out:
	return ret;
}

/**
 * spi_nand_read - [Interface] SPI-NAND read
 * @chip: spi nand device structure
 * @from: offset to read from
 * @len: number of bytes to read
 * @retlen: pointer to variable to store the number of read bytes
 * @buf: the databuffer to put data
 */
static int spi_nand_read(struct spi_nand_info *info, loff_t from, size_t len,
	size_t *retlen, u8 *buf)
{
	struct mtd_oob_ops ops;
	int ret;
	memset(&ops, 0, sizeof(ops));
	ops.len = len;
	ops.datbuf = buf;
	ops.mode = MTD_OPS_PLACE_OOB;
	ret = spi_nand_do_read_ops(info, from, &ops);

	*retlen = ops.retlen;
	return ret;
}

/**
 * spi_nand_write - [Interface] SPI-NAND write
 * @chip: spi nand device structure
 * @to: offset to write to
 * @len: number of bytes to write
 * @retlen: pointer to variable to store the number of written bytes
 * @buf: the data to write
 */
static int spi_nand_write(struct spi_nand_info *info, loff_t to, size_t len,
	size_t *retlen, const u8 *buf)
{
	struct mtd_oob_ops ops;
	int ret;

	memset(&ops, 0, sizeof(ops));
	ops.len = len;
	ops.datbuf = (uint8_t *)buf;
	ops.mode = MTD_OPS_PLACE_OOB;
	ret =  spi_nand_do_write_ops(info, to, &ops);

	*retlen = ops.retlen;

	return ret;
}

/**
 * spi_nand_do_read_oob - read out-of-band
 * @chip: spi nand device structure
 * @from: offset to read from
 * @ops: oob operations description structure
 * Description:
 *   Disable internal ECC before reading when MTD_OPS_RAW set.
 */
static int spi_nand_do_read_oob(struct spi_nand_info *info, loff_t from, struct mtd_oob_ops *ops)
{
	int page_addr;
	unsigned int corrected = 0;
	int readlen = ops->ooblen;
	int max_len = (ops->mode == MTD_OPS_AUTO_OOB) ?
		info->ecclayout->oobavail : info->oob_size;
	int ooboffs = ops->ooboffs;
	bool ecc_off = ops->mode == MTD_OPS_RAW;
	unsigned int failed = 0;
	int ret = 0;

	spinand_debug("%s: from = 0x%012llx, len = %i\n",
			 __func__, from, readlen);
	if (unlikely(ooboffs >= max_len)) {
		spinand_err("%s: attempt to read outside oob\n",
				__func__);
		return -EINVAL;
	}
	if (unlikely(from >= info->size ||
		ooboffs + readlen > ((info->size >> info->page_shift) -
							(from >> info->page_shift)) * max_len)) {
		spinand_err("%s: attempt to read beyond end of device\n",
				__func__);
		return -EINVAL;
	}

	/* Shift to get page */
	page_addr = (from >> info->page_shift);
	max_len -= ooboffs;
	ops->oobretlen = 0;

	while (1) {
		WATCHDOG_RESET();
		/*read data from chip*/
		ret = spi_nand_do_read_page(info, page_addr, info->page_size,
				ecc_off, &corrected, info->oobbuf, info->oob_size);
		if (ret == -EBADMSG)
			failed++;
		else if (ret) {
			spinand_err("error %d reading page 0x%x\n",
				ret, page_addr);
			goto out;
		}

		max_len = min(max_len, readlen);
		spi_nand_transfer_oob(info, ops->oobbuf + ops->oobretlen,
					ops, max_len);

		readlen -= max_len;
		ops->oobretlen += max_len;
		if (!readlen)
			break;

		page_addr++;
	}
out:
	if (failed)
		ret = -EBADMSG;

	return ret;
}

/**
 * spi_nand_do_write_oob - write out-of-band
 * @chip: spi nand device structure
 * @to: offset to write to
 * @ops: oob operation description structure
 * Description:
 *   Disable internal ECC before writing when MTD_OPS_RAW set.
 */
static int spi_nand_do_write_oob(struct spi_nand_info *info, loff_t to, struct mtd_oob_ops *ops)
{
	int ret = 0;
	int page_addr, max_len = (ops->mode == MTD_OPS_AUTO_OOB) ?
		info->ecclayout->oobavail : info->oob_size;
	int ooboffs = ops->ooboffs;
	bool ecc_off = ops->mode == MTD_OPS_RAW;
	int writelen = ops->ooblen;

	spinand_debug("%s: to = 0x%012llx, len = %i\n",
			 __func__, to, writelen);

	/* Do not allow write past end of page */
	if (unlikely(ooboffs > max_len)) {
		spinand_err("%s: attempt to write outside oob\n",
				__func__);
		return -EINVAL;
	}
	if (unlikely(to >= info->size ||
		ooboffs + writelen > ((info->size >> info->page_shift) -
							(to >> info->page_shift)) * max_len)) {
		spinand_err("%s: attempt to write beyond end of device\n",
				__func__);
		return -EINVAL;
	}

	/* Shift to get page */
	page_addr = to >> info->page_shift;
	max_len -= ooboffs;
	ops->oobretlen = 0;

	while (1) {
		WATCHDOG_RESET();
		max_len = min(max_len, writelen);
		spi_nand_fill_oob(info, ops->oobbuf + ops->oobretlen, max_len, ops);

		ret = spi_nand_do_write_page(info, page_addr, info->page_size,
					info->oobbuf, info->oob_size, ecc_off, true);
		if (ret) {
			spinand_err("error %d writing page 0x%x\n",
				ret, page_addr);
			goto out;
		}

		writelen -= max_len;
		ops->oobretlen += max_len;
		if (!writelen)
			break;

		page_addr++;
	}
out:
	return ret;
}

/**
 * spi_nand_read_oob - [MTD Interface] SPI-NAND read data and/or out-of-band
 * @chip: spi nand device structure
 * @from: offset to read from
 * @ops: oob operation description structure
 */
int spi_nand_read_oob(struct spi_nand_info *info, loff_t from, struct mtd_oob_ops *ops)
{
	int ret = -ENOTSUPP;

	ops->retlen = 0;

	/* Do not allow reads past end of device */
	if (ops->datbuf && (from + ops->len) > info->size) {
		spinand_err("%s: attempt to read beyond end of device\n",
				__func__);
		return -EINVAL;
	}

	switch (ops->mode) {
	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_AUTO_OOB:
	case MTD_OPS_RAW:
		break;

	default:
		goto out;
	}

	if (!ops->datbuf)
		ret = spi_nand_do_read_oob(info, from, ops);
	else
		ret = spi_nand_do_read_ops(info, from, ops);

out:
	return ret;
}

/**
 * spi_nand_write_oob - [MTD Interface] SPI-NAND write data and/or out-of-band
 * @chip: spi nand device structure
 * @to: offset to write to
 * @ops: oob operation description structure
 */
int spi_nand_write_oob(struct spi_nand_info *info, loff_t to, struct mtd_oob_ops *ops)
{
	int ret = -ENOTSUPP;

	ops->retlen = 0;

	/* Do not allow writes past end of device */
	if (ops->datbuf && (to + ops->len) > info->size) {
		spinand_err("%s: attempt to write beyond end of device\n",
				__func__);
		return -EINVAL;
	}

	switch (ops->mode) {
	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_AUTO_OOB:
	case MTD_OPS_RAW:
		break;

	default:
		goto out;
	}

	if (!ops->datbuf)
		ret = spi_nand_do_write_oob(info, to, ops);
	else
		ret = spi_nand_do_write_ops(info, to, ops);

out:
	return ret;
}

/**
 * spi_nand_block_isbad - [Interface] check block is bad or not via read
 * bad block mark(the first two byte in oob area of the first page in the block)
 * @chip: spi nand device structure
 * @offs: offset from device start
 * Description:
 *   For a block, read the first page's first two byte of oob data, if data is all
 *   0xFF, the block is a good block, otherwise it a bad block.
 */
int spi_nand_block_isbad(struct spi_nand_info *info, loff_t offs)
{
	struct mtd_oob_ops ops = {0};
	uint32_t block_addr;
	u8 bad[2] = {0, 0};
	u8 ret = 0;

	block_addr = offs >> info->block_shift;
	ops.mode = MTD_OPS_RAW;
	ops.ooblen = 2;
	ops.oobbuf = bad;

	spi_nand_do_read_oob(info, block_addr << info->block_shift, &ops);
	if (bad[0] != 0xFF || bad[1] != 0xFF)
		ret =  1;

	return ret;
}

/**
 * spi_nand_erase - [MTD Interface] erase block(s)
 * @chip: spi nand device structure
 * @addr: address that erase start with, should be blocksize aligned
 * @len: length that want to be erased, should be blocksize aligned
 * Description:
 *   Erase one ore more blocks
 *   The command sequence for the BLOCK ERASE operation is as follows:
 *       06h (WRITE ENBALE command)
 *       D8h (BLOCK ERASE command)
 *       0Fh (GET FEATURES command to read the status register)
 */
int spi_nand_erase(struct spi_nand_info *info, uint64_t addr, uint64_t len)
{
	int page_addr, pages_per_block;
	int lun_num=0, from=0;
	u8 status;
	int ret = 0;
    uint32_t pa_be4xlat __attribute((unused));
    uint32_t pa_in_out __attribute((unused));

	spinand_debug("%s: address = 0x%012llx, len = %llu\n",
			 __func__, addr, len);
	/* check address align on block boundary */
	if (addr & (info->block_size - 1)) {
		spinand_err("%s: Unaligned address\n", __func__);
		return -EINVAL;
	}

	if (len & (info->block_size - 1)) {
		spinand_err("%s: Length not block aligned\n", __func__);
		return -EINVAL;
	}

	/* Do not allow erase past end of device */
	if ((len + addr) > info->size) {
		spinand_err("%s: Erase past end of device\n", __func__);
		return -EINVAL;
	}
	pages_per_block = 1 << (info->block_shift - info->page_shift);
	page_addr = addr >> info->page_shift;
    pa_be4xlat = page_addr;

	while (len) {
		WATCHDOG_RESET();
		/* Check if we have a bad block, we do not erase bad blocks! */
		if (spi_nand_block_isbad(info, ((loff_t) page_addr) <<
					info->page_shift)) {
			spinand_err("%s: attempt to erase a bad block at 0x%012llx\n",
			__func__, ((loff_t) page_addr) << info->page_shift);
            ret = 1;
			goto erase_exit;
		}
#ifdef CONFIG_NAND_HIDE_BAD
        pa_in_out = pa_be4xlat;
        ret = xlat_offset_to_skip_factory_bad(info, &pa_in_out);
        if(ret == -EINVAL){
            spinand_err("%s: offset xlat exceeds chip size\n",__FUNCTION__);
            return ret;
        }
        if(pa_be4xlat != page_addr){
            spinand_debug("erase xlat: 0x%x => 0x%x\n",pa_be4xlat,page_addr);
        }
        page_addr = pa_in_out;
        pa_be4xlat += pages_per_block;
#endif

		spi_nand_write_enable(info);

        if (info->options & SPI_NAND_NEED_DIE_SELECT){
            from = (page_addr<<info->page_shift);
            lun_num = from >> info->lun_shift;
            spi_nand_lun_select(info, lun_num);
        }
		spi_nand_erase_one_block(info, (lun_num?((from&info->lun_mask)>>info->page_shift):page_addr));
		ret = spi_nand_wait(info, &status);
		if (ret < 0) {
#ifdef CONFIG_NAND_HIDE_BAD
            spinand_err("waiting page 0x%x timeout (0x%x): spi_nand_erase_one_block\n", status, (pa_be4xlat-pages_per_block));
#else
            spinand_err("waiting page 0x%x timeout (0x%x): spi_nand_erase_one_block\n", status, page_addr);
#endif
			goto erase_exit;
		}
		if ((status & STATUS_E_FAIL_MASK) == STATUS_E_FAIL) {
#ifdef CONFIG_NAND_HIDE_BAD
            spinand_err("erase block 0x%012llx failed\n",((loff_t) (pa_be4xlat-pages_per_block)) << info->page_shift);
#else
			spinand_err("erase block 0x%012llx failed\n",((loff_t) page_addr) << info->page_shift);
#endif
			ret = -EIO;
			goto erase_exit;
		}

		/* Increment page address and decrement length */
		len -= (1ULL << info->block_shift);
		page_addr += pages_per_block;
#ifdef CONFIG_NAND_HIDE_BAD
        page_addr = pa_be4xlat;
#endif
	}

erase_exit:
	return ret;
}

/**
 * spi_nand_erase_with_badblock - [Interface] erase block(s)
 * @chip: spi nand device structure
 * @addr: address that erase start with, should be blocksize aligned
 * @len: length that want to be erased, should be blocksize aligned
 * Description:
 *   Erase one ore more blocks
 *   The command sequence for the BLOCK ERASE operation is as follows:
 *       06h (WRITE ENBALE command)
 *       D8h (BLOCK ERASE command)
 *       0Fh (GET FEATURES command to read the status register)
 */
static int spi_nand_erase_with_badblock(struct spi_nand_info *info, uint64_t addr, uint64_t len)
{
	int page_addr, pages_per_block;
	u8 status;
	int ret = 0;
    int lun_num=0, from=0;
    uint32_t pa_be4xlat __attribute((unused));
    uint32_t pa_in_out __attribute((unused));

	spinand_debug("%s: address = 0x%012llx, len = %llu\n",
			 __func__, addr, len);
	/* check address align on block boundary */
	if (addr & (info->block_size - 1)) {
		spinand_err("%s: Unaligned address\n", __func__);
		return -EINVAL;
	}

	if (len & (info->block_size - 1)) {
		spinand_err("%s: Length not block aligned\n", __func__);
		return -EINVAL;
	}

	/* Do not allow erase past end of device */
	if ((len + addr) > info->size) {
		spinand_err("%s: Erase past end of device\n", __func__);
		return -EINVAL;
	}

	pages_per_block = 1 << (info->block_shift - info->page_shift);
	page_addr = addr >> info->page_shift;
    pa_be4xlat = page_addr;


	while (len) {
		WATCHDOG_RESET();
		spi_nand_write_enable(info);
#ifdef CONFIG_NAND_HIDE_BAD
        pa_in_out = pa_be4xlat;
        ret = xlat_offset_to_skip_factory_bad(info, &pa_in_out);
        if(ret == -EINVAL){
            spinand_err("%s: offset xlat exceeds chip size\n",__FUNCTION__);
            return ret;
        }
        if(pa_be4xlat != page_addr){
            spinand_debug("erase xlat: 0x%x => 0x%x\n",pa_be4xlat,page_addr);
        }
        page_addr = pa_in_out;
        pa_be4xlat += pages_per_block;
#endif

        if (info->options & SPI_NAND_NEED_DIE_SELECT){
            from = (page_addr<<info->page_shift);
            lun_num = from >> info->lun_shift;
            spi_nand_lun_select(info, lun_num);
        }
		spi_nand_erase_one_block(info, (lun_num?((from&info->lun_mask)>>info->page_shift):page_addr));
		ret = spi_nand_wait(info, &status);
		if (ret < 0) {
#ifdef CONFIG_NAND_HIDE_BAD
            spinand_err("waiting page 0x%x timeout (0x%x): spi_nand_erase_one_block\n", status, (pa_be4xlat-pages_per_block));
#else
            spinand_err("waiting page 0x%x timeout (0x%x): spi_nand_erase_one_block\n", status, page_addr);
#endif
			goto erase_exit;
		}
		if ((status & STATUS_E_FAIL_MASK) == STATUS_E_FAIL) {
#ifdef CONFIG_NAND_HIDE_BAD
			spinand_err("erase block 0x%012llx failed\n",((loff_t) (pa_be4xlat-pages_per_block)) << info->page_shift);
#else
            spinand_err("erase block 0x%012llx failed\n",((loff_t) page_addr) << info->page_shift);
#endif
			ret = -EIO;
			goto erase_exit;
		}

		/* Increment page address and decrement length */
		len -= (1ULL << info->block_shift);
		page_addr += pages_per_block;
	}

erase_exit:
	return ret;
}


/**
 * spi_nand_block_markbad - [Interface] Mark a block as bad block
 * @chip: spi nand device structure
 * @offs: offset from device start
 * Description:
 *   For a block, bad block mark is the first page's first two byte of oob data,
 *   Write 0x0 to the area if we want to mark bad block.
 *   It is unnecessary to write the mark if the block has already marked as bad
 *   block.
 */
int spi_nand_block_markbad(struct spi_nand_info *info, loff_t offs)
{
	int ret;
	uint32_t block_addr;
	struct mtd_oob_ops ops = {0};
	u8 buf[2] = {0, 0};

	ret = spi_nand_block_isbad(info, offs);
	if (ret) {
		if (ret > 0)
			return 0;
		return ret;
	}
	block_addr = offs >> info->block_shift;
	spi_nand_erase(info, block_addr << info->block_shift, info->block_size);
	ops.mode = MTD_OPS_PLACE_OOB;
	ops.ooblen = 2;
	ops.oobbuf = buf;

	ret = spi_nand_do_write_oob(info, block_addr << info->block_shift, &ops);

	return ret;
}

/**
 * spi_nand_block_markunbad - [Interface] Mark a block as unbad block
 * @chip: spi nand device structure
 * @offs: offset from device start
 * Description:
 *   For a block, bad block mark is the first page's first two byte of oob data,
 *   Write 0xff to the area if we want to mark as unbad(good) block.
 */

int spi_nand_block_markunbad(struct spi_nand_info *info, loff_t offs)
{
	uint32_t block_addr;

    block_addr = offs >> info->block_shift;
    return spi_nand_erase_with_badblock(info, block_addr << info->block_shift, info->block_size);
}

/**
 * spi_nand_scan_id_table - scan chip info in id table
 * @chip: SPI-NAND device structure
 * @id: point to manufacture id and device id
 * Description:
 *   If found in id table, config chip with table information.
 */
static bool spi_nand_scan_id_table(struct spi_nand_info *info, u8 *id)
{
	struct spi_nand_flash *type = spi_nand_table;

	for (; type->name; type++) {
		if ((id[0] == type->mfr_id && id[1] == type->dev_id)||
		    (id[0] == type->mfr_id && ((id[1]<<8)|id[2]) == type->dev_id)) {
			info->name = type->name;
            info->mfr_id = type->mfr_id;
            info->dev_id = type->dev_id;
			info->size = type->page_size * type->pages_per_blk * type->blks_per_lun * type->luns_per_chip;
			info->block_size = type->page_size * type->pages_per_blk;
			info->page_size = type->page_size;
			info->oob_size = type->oob_size;
			info->lun_shift = ilog2(info->block_size * type->blks_per_lun);
			info->ecc_strength = type->ecc_strength;
			info->options = type->options;

			return true;
		}
	}

	return false;
}

static uint16_t onfi_crc16(uint16_t crc, u8 const *p, size_t len)
{
	int i;

	while (len--) {
		crc ^= *p++ << 8;
		for (i = 0; i < 8; i++)
			crc = (crc << 1) ^ ((crc & 0x8000) ? 0x8005 : 0);
	}

	return crc;
}

/* Sanitize ONFI strings so we can safely print them */
static void sanitize_string(char *s, size_t len)
{
	int i = len - 1;
	int j = 0;

	/* Null terminate */
	s[i--] = 0;

	/* Remove unnecessary space */
	while (i >= 0 && (s[i] <= ' ' || s[i] > 127)) {
		s[i--] = 0;
	}
	/* Remove non printable chars */
	for (j = 0; j <= i; j++) {
		if (s[j] < ' ' || s[j] > 127)
			s[j] = '?';
	}
}

/**
 * spi_nand_detect_onfi - config chip with parameter page
 * @chip: SPI-NAND device structure
 * Description:
 *   This function is called when we can not get info from id table.
 */
bool spi_nand_detect_onfi(struct spi_nand_info *info, u8 printout)
{
	struct spi_nand_onfi_params *p;
	u8 *buffer;
	u8 read_cache_op, read_dummy, cur_cfg=0;
	bool ret = true;
	u32 i, pp, wc_in_line = 0;
    u32 page_addr[3]={0x0, 0x1, 0x4};

	buffer = malloc(256 * 3);

    for(pp=0 ; pp<sizeof(page_addr)/sizeof(u32) ; pp++){
        spi_nand_change_mode(info, OTP_MODE, &cur_cfg);
        spi_nand_read_page_to_cache(info, page_addr[pp]);
        spi_nand_wait(info, NULL);
        /*
                * read parameter page can only ues 1-1-1 mode
                */
        read_cache_op = info->read_cache_op;
        read_dummy = info->dummy_cycle;
        info->read_cache_op = SPI_NAND_CMD_READ_FROM_CACHE;
        info->dummy_cycle = 8;
        spi_nand_read_from_cache(info, page_addr[pp], 0, 256 * 3, buffer);
        info->read_cache_op = read_cache_op;
        info->dummy_cycle = read_dummy;
        spi_nand_change_mode(info, NORMAL_MODE, &cur_cfg);

        p = (struct spi_nand_onfi_params *)buffer;
        for (i = 0; i < 3; i++, p++) {
            if (onfi_crc16(ONFI_CRC_BASE, (uint8_t *)p, 254) ==
                    le16_to_cpu(p->crc)){
                ret = true;
                goto xlat_page_data;
            }
        }
    }

    if(ret == false){
        spinand_err("Could not find valid ONFI parameter page; aborting\n");
        ret = false;
        goto out;
    }


xlat_page_data:
    info->mfr_id = p->mfr_id;
	memcpy(&info->onfi_params, p, sizeof(*p));

	p = &info->onfi_params;

	sanitize_string(p->manufacturer, sizeof(p->manufacturer));
	sanitize_string(p->model, sizeof(p->model));

	info->name = p->model;
	info->size = le32_to_cpu(p->byte_per_page) *
			le32_to_cpu(p->pages_per_block) *
			le32_to_cpu(p->blocks_per_lun) * p->lun_count;
	info->block_size = le32_to_cpu(p->byte_per_page) *
			le32_to_cpu(p->pages_per_block);
	info->page_size = le32_to_cpu(p->byte_per_page);
	info->oob_size = le16_to_cpu(p->spare_bytes_per_page);
	info->lun_shift = ilog2(info->block_size * le32_to_cpu(p->blocks_per_lun));

    if(info->mfr_id == SPI_NAND_MFR_MICRON){
    	if (p->vendor.micron_sepcific.two_plane_page_read)
    		info->options |= SPI_NAND_NEED_PLANE_SELECT;
    	if (p->vendor.micron_sepcific.die_selection)
    		info->options |= SPI_NAND_NEED_DIE_SELECT;
    	info->ecc_strength = p->vendor.micron_sepcific.ecc_ability;
    }

out:
    if(printout){
        printf(" 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
        printf("-----------------------------------------------\n");
        for (i = 0; i < 256; i++) {
            printf("%02x ", buffer[i]);
            wc_in_line++;
            if (wc_in_line >= 16) {
                printf("\n");
                wc_in_line = 0;
            }
        }
        if (wc_in_line != 0) {
            printf("\n");
        }
    }
	free(buffer);
	return ret;
}

/**
 * spi_nand_set_rd_wr_op - Chose the best read write command
 * @chip: SPI-NAND device structure
 * Description:
 *   Chose the fastest r/w command according to spi controller's ability.
 * Note:
 *   If 03h/0Bh follows SPI NAND protocol, there is no difference,
 *   while if follows SPI NOR protocol, 03h command is working under
 *   <=20Mhz@3.3V,<=5MHz@1.8V; 0Bh command is working under
 *   133Mhz@3.3v, 83Mhz@1.8V.
 */
static void spi_nand_set_rd_wr_op(struct spi_nand_info *info)
{
#if defined(CONFIG_TARGET_VENUS)
    u8  mid = info->mfr_id;
    uint16_t did = info->dev_id;
    struct spi_nand_info *type;

    /* For Cortina serial nand interface, the default IO mode is Quad */
    info->sclk               = 40;
    info->read_cache_op      = SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO;
    info->dummy_cycle        = 4;
    info->write_cache_op     = SPI_NAND_CMD_PROG_LOAD_X4;
    info->write_cache_rdm_op = SPI_NAND_CMD_PROG_LOAD_RDM_DATA_X4;
    type = spi_nand_opcode_overwrite_table;

    for(; type->mfr_id; type++){
        if((mid == type->mfr_id) && ((type->dev_id == 0xFFFF)||(type->dev_id == did))){
            info->sclk               = type->sclk;
            info->read_cache_op      = type->read_cache_op;
            info->dummy_cycle        = type->dummy_cycle;
            info->write_cache_op     = type->write_cache_op;
            info->write_cache_rdm_op = type->write_cache_rdm_op;

            if((type->dev_id == did)||((type->mfr_id == 0x0)&&(type->dev_id == 0x0))){
                break;
            }
        }
    }
	return;
#elif defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
    info->sclk               = 100;
    info->read_cache_op      = SPI_NAND_CMD_READ_FROM_CACHE_FAST;
    info->dummy_cycle        = 8;
    info->write_cache_op     = SPI_NAND_CMD_PROG_LOAD;
    info->write_cache_rdm_op = SPI_NAND_CMD_PROG_LOAD_RDM_DATA_X4;
    return;
#endif
}

/**
 * spi_nand_set_die_selection_op -
 * @chip: SPI-NAND device structure
 * Description:
 */
static void spi_nand_set_die_selection_op(struct spi_nand_info *info)
{
    if((info->options&SPI_NAND_NEED_DIE_SELECT)){
        if((info->mfr_id == SPI_NAND_MFR_WINBOND) || ((info->mfr_id == SPI_NAND_MFR_ESMT_GD_ZENTEL)&&(info->options&SPI_NAND_MFR_IS_ESMT))){
            info->die_sel_op.cmd  = SPI_NAND_CMD_DIE_SEL_ESMT_WINBOND;
            info->die_sel_op.addr= 0xFF;
            info->die_sel_op.die[0] = 0;
            info->die_sel_op.die[1] = 1;
            info->die_sel_op.n_addr = 1;
            info->die_sel_op.n_data = 0;
        }else if(info->mfr_id == SPI_NAND_MFR_MICRON){
            info->die_sel_op.cmd = SPI_NAND_CMD_SET_FEATURE;
            info->die_sel_op.addr= 0xD0;
            info->die_sel_op.die[0] = 0;
            info->die_sel_op.die[1] = 0x40;
            info->die_sel_op.n_addr = 1;
            info->die_sel_op.n_data = 1;
        }
    }
}

/*
 * spinand_xxx_ecc_status- Get flash on die ecc decode status
 * Description:
 *    Every flash model has its own ecc status definition rule.
 */
static void spi_nand_bypass_ecc_status_check(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    *corrected = 0xFF;
    *ecc_error = 0;
}

static void spi_nand_general_1bit_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;

    switch (ecc_status) {
        case 0:
            *corrected = 0;
            break;

        case 1:
            *corrected = 1;
            break;

        case 2:
        default:
            *ecc_error = 1;
            break;
    }
    return;
}

static void spi_nand_ato_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;

    switch (ecc_status) {
        case 0:
            *corrected = 0;
            break;

        case 1:
            *corrected = 7;
            break;

        case 2:
            *ecc_error = 1;
            break;

        case 3:
            *corrected = 8;
            break;
    }
    return;
}

static void spi_nand_etron_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;

    switch (ecc_status) {
        case 2:
            *ecc_error = 1;
            break;

        case 0:
            *corrected = 0;
            break;

        case 1:
            *corrected = info->ecc_strength-1;
            break;

        case 3:
            *corrected = info->ecc_strength;
            break;
    }
    return;
}

static void spi_nand_gd5fq4xx_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint32_t ecc_status = (status & 0x70) >> SPI_NAND_STATUS_ECC_SHIFT;

    switch(ecc_status) {
        case 0:
            *corrected = 0;
            break;

        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            *corrected = ecc_status + 2;
            break;

        case 7:
            *ecc_error = 1;
            break;
    }
    return;
}

static void spi_nand_gd5f_4bit_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t s1;
    uint32_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;

    switch(ecc_status) {
        case 2:
            *ecc_error = 1;
            break;

        case 0:
            *corrected = 0;
            break;

        default:
            spi_nand_get_feature_register(info, 0xF0, &s1);
            *corrected = ((s1 >> 4) & 0x3) + 1;
            break;
    }
    return;
}

static void spi_nand_gd5f_8bit_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t s1;
    uint32_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;

    switch (ecc_status) {
        case 2:
            *ecc_error = 1;
            break;

        case 0:
            *corrected = 0;
            break;

        case 3:
            *corrected = 8;
            break;

        default:
            spi_nand_get_feature_register(info, 0xF0, &s1);
            *corrected = ((s1 >> 4) & 0x3) + 4;
            break;
    }
    return;
}

static void spi_nand_gd_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    switch(info->dev_id) {
        case 0xD1: //GD5F1GQ4UxYIG
        case 0xD2: //GD5F2GQ4UxxIG
        case 0xD4: //GD5F4GQ4UBYIG
        case 0xD9: //GD5F1GQ4UEYIH
            spi_nand_gd5fq4xx_ecc_status(info, status, corrected, ecc_error);
            break;

        case 0x31: //GD5F1GQ5UEYIH
        case 0x32: //GD5F2GQ5UEYIH
        case 0x35: //GD5F4GQ6UEYIH
        case 0x51: //GD5F1GQ5UEYIG
        case 0x52: //GD5F2GQ5UEYIG
        case 0x55: //GD5F4GQ6UEYIG
            spi_nand_gd5f_4bit_ecc_status(info, status, corrected, ecc_error);
            break;

        case 0x91: //GD5F1GM7UEYIG
        case 0x92: //GD5F2GM7UEYIG
        case 0x95: //GD5F4GM8UEYIG
            spi_nand_gd5f_8bit_ecc_status(info, status, corrected, ecc_error);
            break;

        default:
            spi_nand_bypass_ecc_status_check(info, status, corrected, ecc_error);
            break;
    }
    return;
}

static void spi_nand_heyangtek_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint32_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;

    switch (ecc_status) {
        case 2:
            *ecc_error = 1;
            break;

        case 0:
            *corrected = 0;
            break;

        case 1:
            *corrected = info->ecc_strength - 1;
            break;

        case 3:
            *corrected = info->ecc_strength;
            break;
    }
    return;
}

static void spi_nand_longsys_xxF1Q_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & 0x70) >> SPI_NAND_STATUS_ECC_SHIFT;

    switch(ecc_status) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            *corrected = ecc_status;
            break;

        case 7:
        default:
            *ecc_error = 1;
            break;
    }
    return;
}

static void spi_nand_longsys_4bit_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;

    switch(ecc_status) {
        case 2:
            *ecc_error = 1;
            break;

        case 0:
            *corrected = 0;
            break;

        default:
            *corrected = 4;
            break;
    }
    return;
}

static void spi_nand_longsys_1bit_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;

    switch(ecc_status) {
        case 2:
        case 3:
            *ecc_error = 1;
            break;

        case 0:
            *corrected = 0;
            break;

        case 1:
            *corrected = 1;
            break;
    }
    return;
}

static void spi_nand_longsys_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    switch(info->dev_id) {
        case 0xA1: //FS35ND01G_D1F1QWHI100
        case 0xB1: //FS35ND01G_S1F1QWFI000
        case 0xA2: //FS35ND02G_S2F1QWFI000
        case 0xB2: //FS35ND02G_D1F1QWFI000
        case 0xA4: //FS35ND04G_S2F1QWFI000
            spi_nand_longsys_xxF1Q_ecc_status(info, status, corrected, ecc_error);
            break;

        case 0x72: //F35SQA002G
            spi_nand_longsys_1bit_ecc_status(info, status, corrected, ecc_error);
            break;

        case 0xEA: //FS35ND01G-S1Y2
        case 0xEB: //FS35ND02G-S3Y2
        case 0xEC: //FS35ND04G-S2Y2
            spi_nand_longsys_4bit_ecc_status(info, status, corrected, ecc_error);
            break;

        default:
            spi_nand_bypass_ecc_status_check(info, status, corrected, ecc_error);
            break;
    }
    return;
}

static void spi_nand_mt29f_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint32_t ecc_status = status & SPI_NAND_MT29F_ECC_MASK;

    switch (ecc_status) {
        case SPI_NAND_MT29F_ECC_UNCORR:
            *ecc_error = 1;
            break;
        case SPI_NAND_MT29F_ECC_0_BIT:
            *corrected = 0;
            break;
        case SPI_NAND_MT29F_ECC_1_3_BIT:
            *corrected = 3;
            break;
        case SPI_NAND_MT29F_ECC_4_6_BIT:
            *corrected = 6;
            break;
        case SPI_NAND_MT29F_ECC_7_8_BIT:
        default:
            *corrected = 8;
            break;
    }
    return;
}

static void spi_nand_dosilicon_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    switch(info->dev_id) {
        case 0x71: //DS35Q1GA-IB
        case 0x72: //DS35Q2GA-IB
            spi_nand_longsys_4bit_ecc_status(info, status, corrected, ecc_error);
            break;

        case 0xF1: //DS35Q1GB-IB
        case 0xF2: //DS35Q2GB-IB
        case 0xF4: //DS35Q4GM-IB
            spi_nand_mt29f_ecc_status(info, status, corrected, ecc_error);
            break;

        default:
            spi_nand_bypass_ecc_status_check(info, status, corrected, ecc_error);
            break;
    }
    return;
}

static void spi_nand_esmt_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    switch(info->dev_id) {
        case 0x01:
        case 0x0A:
            spi_nand_general_1bit_ecc_status(info, status, corrected, ecc_error);
            break;

        case 0x41:
            spi_nand_mt29f_ecc_status(info, status, corrected, ecc_error);
            break;

        default:
            spi_nand_bypass_ecc_status_check(info, status, corrected, ecc_error);
            break;
    }
    return;
}

static void spi_nand_fmsh_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    switch(info->dev_id) {
        case 0xA1: //FM25S01"
        case 0xE4: //FM25S01A
        case 0xE5: //FM25S02A
            spi_nand_general_1bit_ecc_status(info, status, corrected, ecc_error);
            break;

        case 0xD4: //FM25S01BI3
        case 0xD6: //FM25S02BI3
            spi_nand_mt29f_ecc_status(info, status, corrected, ecc_error);
            break;

        default:
            spi_nand_bypass_ecc_status_check(info, status, corrected, ecc_error);
            break;
    }
    return;
}

#define MXIC_READ_ECCSR_OPcode     0x7C
static void spi_nand_mxic_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;
    struct spi_nand_cmd cmd;

    if (ecc_status == 2) {
        *ecc_error = 1;
    } else {
        memset(&cmd, 0, sizeof(struct spi_nand_cmd));
        cmd.cmd = SPI_NAND_CMD_MXIC_ECC_STS_READ;
        cmd.n_rx = 1;
        cmd.rx_buf = (u8 *)corrected;
        info->spinand_command_fn(&cmd);
        *corrected &= 0xF;
    }
    return;
}

static void spi_nand_toshiba_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t s1;
    uint8_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;
    uint8_t mbf_addr = 0x30;

    if (ecc_status == 2) {
        *ecc_error = 1;
    } else {
        spi_nand_get_feature_register(info, mbf_addr, &s1);
        *corrected = (s1 >> 4) & 0xF;
    }
    return;
}

static void spi_nand_winbond_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;
    uint8_t mbf_addr = 0x30;
    uint8_t s1 = 0;

    if (ecc_status == 2) {
        *ecc_error = 1;
        return;
    }

    switch(info->dev_id) {
        case 0xAA20: //W25N512GVEIG
        case 0xAA21: //W25N01GVZEIG
        case 0xAB21: //W25M02GVZEIG
            *corrected = ecc_status;
            break;

        case 0xAE21: //W25N01KVZEIR
            spi_nand_get_feature_register(info, mbf_addr, &s1);
            *corrected = (s1 >> 4) & 0x7;
            break;

        case 0xAA22: //W25N02KVZEIE
        case 0xAA23: //W25N04KVZEIE
            spi_nand_get_feature_register(info, mbf_addr, &s1);
            *corrected = (s1 >> 4) & 0xF;
            break;

        default:
            spi_nand_bypass_ecc_status_check(info, status, corrected, ecc_error);
            break;
    }
    return;
}

static void spi_nand_united_memory_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    switch(info->dev_id) {
        case 0xBA13: //GSS01GSAK1
        case 0xBA23: //GSS02GSAK1
            spi_nand_longsys_4bit_ecc_status(info, status, corrected, ecc_error);
            break;

        default:
            spi_nand_bypass_ecc_status_check(info, status, corrected, ecc_error);
            break;
    }
    return;
}

static void spi_nand_xin_cun_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint32_t ecc_status = (status & SPI_NAND_STATUS_ECC_MASK) >> SPI_NAND_STATUS_ECC_SHIFT;


    switch(info->dev_id) {
        case 0xA1: //XCSP2AAPK-IT
            switch(ecc_status) {
                case 0:
                    *corrected = 0;
                    break;
                case 1:
                    *corrected = 4;
                    break;
                case 2:
                    *ecc_error = 1;
                    break;
                case 3:
                    *corrected = 8;
                    break;
            }
            break;

        default:
            spi_nand_bypass_ecc_status_check(info, status, corrected, ecc_error);
            break;
    }
    return;
}

static void spi_nand_xtx26g0xa_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & 0x3C) >> 2;

    switch(ecc_status) {
        case 8:
            *ecc_error = 1;
            break;

        case 12:
            *corrected = 8;
            break;

        default:
            *corrected = ecc_status;
            break;
    }
    return;
}

static void spi_nand_xtx26g02b_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & 0x70) >> 4;

    switch(ecc_status) {
        case 7:
            *ecc_error = 1;
            break;

        case 0:
        case 1:
        case 2:
        case 3:
        default:
            *corrected = ecc_status;
            break;
    }
    return;
}

static void spi_nand_xtx26g0xc_8bit_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & 0xF0) >> 4;

    if (ecc_status == 0xF) {
        *ecc_error = 1;
    } else {
        *corrected = ecc_status;
    }
    return;
}

static void spi_nand_xtx26g1xd_8bit_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    uint8_t ecc_status = (status & 0xF0) >> 4;
    uint8_t eccs10 = (ecc_status & 0x3);
    uint8_t eccs32 = (ecc_status >> 2) & 0x3;

    switch(eccs10) {
        case 2:
            *ecc_error = 1;
            break;

        case 3:
            *corrected = 8;
            break;

        case 0:
            *corrected = 0;
            break;

        default:
            *corrected = eccs32 + 4;
            break;
    }
    return;
}

static void spi_nand_xtx_ecc_status(struct spi_nand_info *info, uint32_t status, uint32_t *corrected, uint32_t *ecc_error)
{
    switch(info->dev_id){
        case 0xE1: //XT26G01AWSEGA
        case 0xE2: //XT26G02AWSEGA
        case 0xF1: //XT26G01BWSEGA
            spi_nand_xtx26g0xa_ecc_status(info, status, corrected, ecc_error);
            break;

        case 0xF2: //XT26G02BWSEGA
            spi_nand_xtx26g02b_ecc_status(info, status, corrected, ecc_error);
            break;

        case 0x11: //XT26G01CWSEGA
        case 0x12: //XT26G02CWSEGA
        case 0x13: //XT26G04CWSEGA
            spi_nand_xtx26g0xc_8bit_ecc_status(info, status, corrected, ecc_error);
            break;

        case 0x31: //XT26G01DWSIGA 
        case 0x32: //XT26G02DWSIGA
        case 0x33: //XT26G04DWSIGA
        case 0x34: //XT26G11DWSIGA
        case 0x35: //XT26G12DWSIGA
            spi_nand_xtx26g1xd_8bit_ecc_status(info, status, corrected, ecc_error);
            break;

        default:
            spi_nand_bypass_ecc_status_check(info, status, corrected, ecc_error);
            break;
    }
    return;
}

static int spi_nand_set_ecc_criteria(struct spi_nand_info *info)
{
    if (!info->enable_ecc)
        info->enable_ecc = spi_nand_enable_ecc;
    if (!info->disable_ecc)
        info->disable_ecc = spi_nand_disable_ecc;

    info->ecclayout = &dummy_ecc_layout_64;
    info->get_ecc_status = spi_nand_bypass_ecc_status_check;

    switch (info->mfr_id) {
        case SPI_NAND_MFR_ATO:
            info->get_ecc_status = spi_nand_ato_ecc_status;
            break;

        case SPI_NAND_MFR_DOSILICON:
            info->get_ecc_status = spi_nand_dosilicon_ecc_status;
            break;

        case SPI_NAND_MFR_ESMT_GD_ZENTEL:
            if (SPI_NAND_MFR_IS_ESMT == (info->options&SPI_NAND_MFR_IS_ESMT)) {
                info->get_ecc_status = spi_nand_esmt_ecc_status;
            } else if (SPI_NAND_MFR_IS_GD == (info->options&SPI_NAND_MFR_IS_GD)) {
                info->get_ecc_status = spi_nand_gd_ecc_status;
            } else if (SPI_NAND_MFR_IS_ZENTEL == (info->options&SPI_NAND_MFR_IS_ZENTEL)) {
                info->get_ecc_status = spi_nand_general_1bit_ecc_status;
            }

            if (info->oob_size == 64) {
                info->ecclayout = &gd5f_ecc_layout_64;
            }else if (info->oob_size == 256) {
                info->ecclayout = &gd5f_ecc_layout_256;
            }
            break;

        case SPI_NAND_MFR_ETRON:
            info->get_ecc_status = spi_nand_etron_ecc_status;
            break;

        case SPI_NAND_MFR_FMSH:
            info->get_ecc_status = spi_nand_fmsh_ecc_status;
            break;

        case SPI_NAND_MFR_HEYANGTEK:
            info->get_ecc_status = spi_nand_heyangtek_ecc_status;
            break;

        case SPI_NAND_MFR_LONGSYS:
            info->get_ecc_status = spi_nand_longsys_ecc_status;
            break;

        case SPI_NAND_MFR_MACRONIX:
            info->get_ecc_status = spi_nand_mxic_ecc_status;
            break;

        case SPI_NAND_MFR_MICRON:
            info->get_ecc_status = spi_nand_mt29f_ecc_status;

            if (info->oob_size == 64)
                info->ecclayout = &micron_ecc_layout_64;
            else if (info->oob_size == 128)
                info->ecclayout = &micron_ecc_layout_128;
            break;

        case SPI_NAND_MFR_TOSHIBA:
            info->get_ecc_status = spi_nand_toshiba_ecc_status;
            break;

        case SPI_NAND_MFR_UNITED_MEMORY:
            info->get_ecc_status = spi_nand_united_memory_ecc_status;
            break;

        case SPI_NAND_MFR_WINBOND:
            info->get_ecc_status = spi_nand_winbond_ecc_status;
            break;

        case SPI_NAND_MFR_XIN_CUN:
            info->get_ecc_status = spi_nand_xin_cun_ecc_status;
            break;

        case SPI_NAND_MFR_XTX:
            info->get_ecc_status = spi_nand_xtx_ecc_status;
            break;
    }

    info->oobbuf = malloc(info->oob_size);
    if (!info->oobbuf)
        return -ENOMEM;

    info->refresh_threshold = info->ecc_strength;
    return 0;
}


u8 is_info_printed = 0;
void spi_nand_show_info(struct spi_nand_info *info)
{
    printf("\rSPI  NAND: %s (0x%02x%02x)\n",  info->name, info->mfr_id, info->dev_id);
	printf("    spare: 0x%x      (%dB)\n",  info->oob_size, info->oob_size);
	printf("     page: 0x%x     (%dKB)\n", info->page_size, info->page_size/1024);
	printf("    block: 0x%x   (%dKB)\n", info->block_size, info->block_size/1024);
	printf("     chip: 0x%8x(%dMB)\n",(uint32_t)info->size, (uint32_t)info->size/(1024*1024));
	printf("available: 0x%8x(%dMB)\n",(uint32_t)(info->mtd->size), (uint32_t)(info->mtd->size)/(1024*1024));

    printf("     mode: ");
    switch(info->read_cache_op){
        case SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO:
            puts("Q/");
            break;
        case SPI_NAND_CMD_READ_FROM_CACHE_X4:
            puts("4/");
            break;
        case SPI_NAND_CMD_READ_FROM_CACHE_DUAL_IO:
            puts("D/");
            break;
        case SPI_NAND_CMD_READ_FROM_CACHE_X2:
            puts("2/");
            break;
        case SPI_NAND_CMD_READ_FROM_CACHE:
        case SPI_NAND_CMD_READ_FROM_CACHE_FAST:
            puts("S/");
            break;
    }
    switch(info->write_cache_op){
        case SPI_NAND_CMD_PROG_LOAD_X4:
            puts("4\n");
            break;
        case SPI_NAND_CMD_PROG_LOAD:
            puts("S\n");
            break;
    }
    printf("    clock: %d MHz\n",spi_nand_get_clock());

#ifdef CONFIG_NAND_HIDE_BAD
    spi_nand_print_bad_block_hidden_list();
#endif
    puts("NAND:  ");
}


/**
 * spi_nand_scan_ident - [Interface] Init SPI-NAND device driver
 * @spi: spi device structure
 * @chip_ptr: pointer point to spi nand device structure pointer
 */
int spi_nand_scan_ident(struct mtd_info *mtd, int maxchips)
{
    struct nand_chip *chip = mtd_to_nand(mtd);
    struct spi_nand_info *info = (struct spi_nand_info *) chip->priv;
	u8 id[SPI_NAND_MAX_ID_LEN] = {0};

    /* Sanity Check */
	if(!chip || !info)
		return -ENOMEM;

    /* Initialize CA flash controller for SPI NAND type */
    ca_spinand_ctrlr_init();


    /* Set spi-nand-command function first  */
    info->spinand_command_fn = ca_spinand_cmd_fn;

	spi_nand_reset(info);
	spi_nand_read_id(info, id);


	if (spi_nand_scan_id_table(info, id)){
		spi_nand_set_die_selection_op(info);
		goto ident_done;
	}
	puts("\n");
	spinand_info("SPI-NAND type mfr_id: %x, dev_id: %x is not in id table.\n", id[0], id[1]);

	if (spi_nand_detect_onfi(info, 0)){
		goto ident_done;
    }
	return -ENODEV;

ident_done:
    spi_nand_set_rd_wr_op(info);
    spi_nand_set_clock(info);

	info->block_shift = ilog2(info->block_size);
	info->page_shift = ilog2(info->page_size);
	info->page_mask = info->page_size - 1;
	info->lun_mask = (1<<info->lun_shift)-1;
	info->lun = 0;
    info->numchips = 1;

    if(spi_nand_set_ecc_criteria(info))
        return -ENOMEM;

    uint32_t need_die_sel =(info->options&SPI_NAND_NEED_DIE_SELECT)?1:0;
    do{
        if(info->options&SPI_NAND_NEED_QE_ENABLE){
            spi_nand_enable_quad_io(info);
       }

        spi_nand_lock_block(info, BL_ALL_UNLOCKED);
        info->enable_ecc(info);

        if(need_die_sel){
            spi_nand_lun_select(info, 1);
       }
   }while(need_die_sel--);



#ifdef CONFIG_NAND_HIDE_BAD
    scan_spi_nand_factory_bad_blocks(info);
#endif

    if(!is_info_printed++){
        spi_nand_show_info(info);
    }
	return 0;
}

