/*
 * Copyright (c) 2016 Cortina Access. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 * Modification History:
 *    #000 2016-01-01 Cortina_Owner   create file
 *    #001 2016-11-14 Flysky_Hung     proting to RTL8198F
 *    #002
 *    #003
 *    #004
 ******************************************************************************
 */
/*#define DEBUG*/
#include <asm/io.h>
#include <common.h>
#include <malloc.h>
#include <spi_flash.h>
#include "sf_internal.h"
#include <g3lite_registers.h>

#define CFI_MFR_AMD             0x0001
#define CFI_MFR_AMIC            0x0037
#define CFI_MFR_ATMEL           0x001F
#define CFI_MFR_EON             0x001C
#define CFI_MFR_FUJITSU         0x0004
#define CFI_MFR_HYUNDAI         0x00AD
#define CFI_MFR_INTEL           0x0089
#define CFI_MFR_MACRONIX        0x00C2
#define CFI_MFR_NEC             0x0010
#define CFI_MFR_PMC             0x009D
#define CFI_MFR_SAMSUNG         0x00EC
#define CFI_MFR_SHARP           0x00B0
#define CFI_MFR_SST             0x00BF
#define CFI_MFR_ST              0x0020 /* STMicroelectronics */
#define CFI_MFR_TOSHIBA         0x0098
#define CFI_MFR_WINBOND         0x00DA

#define SNOR_AC_OPCODE                	0x0000
#define SNOR_AC_OPCODE_1_DATA           	0x0100
#define SNOR_AC_OPCODE_2_DATA           	0x0200
#define SNOR_AC_OPCODE_3_DATA           	0x0300
#define SNOR_AC_OPCODE_4_DATA           	0x0400
#define SNOR_AC_OPCODE_4_ADDR         	0x0500
#define SNOR_AC_OPCODE_4_ADDR_1_DATA    	0x0600
#define SNOR_AC_OPCODE_4_ADDR_2_DATA    	0x0700
#define SNOR_AC_OPCODE_4_ADDR_3_DATA    	0x0800
#define SNOR_AC_OPCODE_4_ADDR_4_DATA    	0x0900
#define SNOR_AC_OPCODE_4_ADDR_X_1_DATA   	0x0A00
#define SNOR_AC_OPCODE_4_ADDR_X_2_DATA   	0x0B00
#define SNOR_AC_OPCODE_4_ADDR_X_3_DATA   	0x0C00
#define SNOR_AC_OPCODE_4_ADDR_X_4_DATA   	0x0D00
#define SNOR_AC_OPCODE_4_ADDR_4X_1_DATA  	0x0E00

#define SF_START_BIT_ENABLE             0x0002

#define SFLASH_FORCEBURST               0x2000
#define SFLASH_FORCETERM                0x1000

#define SF_AC_OPCODE                	0x00
#define SF_AC_OPCODE_1_DATA           	0x01
#define SF_AC_OPCODE_2_DATA           	0x02
#define SF_AC_OPCODE_3_DATA           	0x03
#define SF_AC_OPCODE_4_DATA           	0x04
#define SF_AC_OPCODE_3_ADDR         	0x05
#define SF_AC_OPCODE_3_ADDR_1_DATA    	0x06
#define SF_AC_OPCODE_3_ADDR_2_DATA    	0x07
#define SF_AC_OPCODE_3_ADDR_3_DATA    	0x08
#define SF_AC_OPCODE_3_ADDR_4_DATA    	0x09
#define SF_AC_OPCODE_3_ADDR_X_1_DATA   	0x0A
#define SF_AC_OPCODE_3_ADDR_X_2_DATA   	0x0B
#define SF_AC_OPCODE_3_ADDR_X_3_DATA   	0x0C
#define SF_AC_OPCODE_3_ADDR_X_4_DATA   	0x0D
#define SF_AC_OPCODE_3_ADDR_4X_1_DATA  	0x0E
#define SF_AC_OPCODE_EXTEND		0xF

#define SF_ACCESS_MIO_SINGLE		0
#define SF_ACCESS_MIO_DUAL		1
#define SF_ACCESS_MIO_QUARD		2

/* Chip select */
#define SF_ACCESS_CE_ALT		0

/* flashAddrCount*/
//#define SF_ADDR_COUNT			2	/* 3 Byte address */
//#define SF_ADDR_COUNT			3	/* 4 Byte address */

#define SF_START_DIRECT_ENABLE		0x00000001
#define SF_START_INDIRECT_ENABLE	0x00000002
#define SF_START_DMA_ENABLE		0x00000008
//#define SF_START_BIT_WRITE		0x200
#define SFLASH_FORCEBURST		0x2000
#define SFLASH_FORCETERM		0x1000

/* Command for SPI NOR */
#define CMD_READ_SISO			0x03
#define CMD_FAST_READ			0x0B
#define CMD_READ_SIDO			0x3B
#define CMD_READ_SIQO			0x6B
#define CMD_READ_DUAL_IO		0xBB
#define CMD_READ_QUAD_IO		0xEB
#define CMD_PROG_PAGE_X1		0x02
#define CMD_PROG_PAGE_X4		0x38
#define CMD_PROG_PAGE_EXC		0x10
#define CMD_ERASE_SEC			0x20
#define CMD_ERASE_BLK_32KB		0x52
#define CMD_ERASE_BLK_64KB		0xD8
#define CMD_ERASE_CHIP			0xC7
#define CMD_ENTER_4BYTE			0xB7
#define CMD_EXIT_4BYTE			0xE9
#define CMD_WR_ENABLE			0x06
#define CMD_WR_DISABLE			0x04
#define CMD_READ_ID			0x9F
#define CMD_RESET			0xFF
#define CMD_RESET_NOP			0x00
#define CMD_RESET_ENABLE		0x66
#define CMD_RESET_MEMORY		0x99
#define CMD_READ_STATUS_REG		0x05	/* Read Status Register */
#define CMD_READ_CONF_REG               0x15    /* Read Configuration Register */
#define CMD_WRITE_STATUS_CONF_REG	0x01	/* Write Status/Configuration Register */
#define CMD_READ_SCUR_REG		0x2B	/* Read Security Register (Read Block protect) */
#define CMD_WRITE_SCUR_REG		0x2F	/* Read Security Register (Read Block protect) */
#define CMD_GET_FEATURE_REG		0x05
#define CMD_SET_FEATURE_REG		0x01

/* feature / status reg */
#define REG_BLOCK_LOCK			0xa0
#define REG_OTP				0xb0
#define REG_STATUS			0xc0	/* timing */

/* status */
#define STATUS_OIP_MASK			0x01
#define STATUS_READY			(0 << 0)  /* Not in write operation */
#define STATUS_BUSY			(1 << 0)  /* In write operation */

#define STATUS_WIP_MASK			0x01
#define STATUS_WIP			(0 << 0)

/* QE */
#define OTP_QE_MASK			0x40 /* Bit6 QE (Quad Enable) */
#define OTP_QE_EN			1
#define OTP_QE_OFF			0

/* BURST MODE */
#define BURST_MODE_OFF			0

/* block lock */
#define BL_ALL_LOCKED			0x38
#define BL_1_2_LOCKED			0x30
#define BL_1_4_LOCKED			0x28
#define BL_1_8_LOCKED			0x20
#define BL_1_16_LOCKED			0x18
#define BL_1_32_LOCKED			0x10
#define BL_1_64_LOCKED			0x08
#define BL_ALL_UNLOCKED			0

#define PLANE_SELECT			0x40
#define PLANE_SELECT_BIT		0x1000

#define SFLASH_STATUS_WEL             	0x02
#define SFLASH_STATUS_WIP             	0x01
#define SFLASH_STATUS_BP		0x1C
#define SFLASH_STATUS_SRWD            	0x80

#define SFLASH_WEL_TIMEOUT		8000
#define SFLASH_PROGRAM_TIMEOUT		8000
#define SFLASH_ERASE_TIMEOUT		400000
#define SFLASH_CMD_TIMEOUT		40

/* DMA */
#define FLASH_RXDESC_ADDR_BASE		0x00002000
#define FLASH_TXDESC_ADDR_BASE		0x00003000

typedef volatile union {
  struct {
    ca_uint32   buffer_addr        :  8 ; /* bits 7:0 */
    ca_uint32   buffer_len         : 16 ; /* bits 23:8 */
    ca_uint32   rervd              :  7 ; /* bits 30:24 */
    ca_uint32   own                :  1 ; /* bits 31:31 */
  } bf ;
  ca_uint32       wrd ;
} FLASH_RXDESC_ADDR_BASE_t;

typedef volatile union {
  struct {
    ca_uint32   buffer_addr        :  8 ; /* bits 7:0 */
    ca_uint32   buffer_len         : 16 ; /* bits 23:8 */
    ca_uint32   sgm                :  1 ; /* bits 24:24 */
    ca_uint32   rervd              :  6 ; /* bits 30:25 */
    ca_uint32   own                :  1 ; /* bits 31:31 */
  } bf ;
  ca_uint32       wrd ;
} FLASH_TXDESC_ADDR_BASE_t;

/* FIFO */
#define FIFO_ADDR			0xE0000000
#define FIFO_ADDR_MAX			0xEFFFFFFF

/* FLASH BASS ADDR */
#define FLASH_BASE_ADDR			0xE0000000

//static u32 spinor_type;
//struct spi_flash *snorflash;
static u8 sflash_addr = 2;			// 3 byte address
//static u32 sflash_size = 0x20000000;		// 512M bit
static u32 spinor_page_size = 0x100;		// 256B page size
static u32 spinor_page_sz_mask = 0xff;
static u32 spinor_sector_size = 0x1000;		// 4k sector size
static u32 spinor_blk_size_32 = 32 * 1024;	// 32k block size
//static u32 spinor_blk_size_64 = 64 * 1024;	// 64k block size
//static u32 spinor_dev_size = 256 * 0x100000;

#define READ_FROM_CACHE(x,y,z)		spinor_read_cache_x1(x,y,z)
#define WRITE_TO_CACHE(x,y,z)		spinor_write_cache_x1(x,y,z)
#define DMA_READ_FROM_CACHE(x,y,z)	spinor_dmassp_read_x1(x,y,z)
#define DMA_WRITE_TO_CACHE(x,y,z)	spinor_dmassp_write_x1(x,y,z)

#define g_chipen			0x0
//#define SFLASH_FLASH_TYPE		0x0
#define SFLASH_4BYTE_ADDR		0x400


/* Flash opcodes. */
#define SFLASH_WRITE_STATUS_OPCODE	( 0x01 | SNOR_AC_OPCODE_1_DATA  )
#define SFLASH_PROGRAM_OPCODE         	( 0x02 | SNOR_AC_OPCODE_4_ADDR_1_DATA )
#define SFLASH_READ_OPCODE            	( 0x03 | SNOR_AC_OPCODE_4_ADDR_1_DATA )
#define SFLASH_FASTREAD_OPCODE          ( 0x0B | SNOR_AC_OPCODE_4_ADDR_1_DATA )
#define SFLASH_READ_STATUS_OPCODE	( 0x05 | SNOR_AC_OPCODE_1_DATA  )
#define SFLASH_WRITE_ENABLE_OPCODE	( 0x06 | SNOR_AC_OPCODE )
#define SFLASH_BE_4K_OPCODE           	( 0x20 | SNOR_AC_OPCODE_4_ADDR )
#define SFLASH_BE_32K_OPCODE           	( 0x52 | SNOR_AC_OPCODE_4_ADDR )
#define SFLASH_CHIP_ERASE_OPCODE      	( 0xc7 | SNOR_AC_OPCODE_4_ADDR )
#define SFLASH_ERASE_OPCODE           	( 0xD8 | SNOR_AC_OPCODE_4_ADDR )
#define SFLASH_READID_OPCODE           	( 0x9F | SNOR_AC_OPCODE_4_DATA )


/* Used for SST flashes only. */
//#define OPCODE_BP               0x02    /* Byte program */
//#define OPCODE_WRDI             0x04    /* Write disable */
//#define OPCODE_AAI_WP           0xad    /* Auto address increment word program */


/* Used for Macronix flashes only. */
#define SFLASH_OPCODE_EN4B             ( 0xb7 | SNOR_AC_OPCODE  )    /* Enter 4-byte mode */
#define SFLASH_OPCODE_EX4B             ( 0xe9 | SNOR_AC_OPCODE  )    /* Exit 4-byte mode */

/* Used for Spansion flashes only. */
#define SFLASH_OPCODE_BRWR             ( 0x17 | SNOR_AC_OPCODE_1_DATA  )   /* Bank register write */

#define SFLASH_STATUS_WEL             	0x02
#define SFLASH_STATUS_WIP             	0x01
#define SFLASH_STATUS_BP		0x1C
#define SFLASH_STATUS_SRWD            	0x80

#define SFLASH_WEL_TIMEOUT		8000
#define SFLASH_PROGRAM_TIMEOUT		8000
#define SFLASH_ERASE_TIMEOUT		400000
#define SFLASH_CMD_TIMEOUT		40


//#define S25FL128P_SECTOR_SIZE         0x00040000	//0x00010000
//#define S25FL128P_PAGE_SIZE           0x00000100
//#define S25FL128P_SIZE		      0x01000000
static u32 sflash_sector_size = 0x00040000;	//0x00010000
static u32 sflash_page_size = 0x00000100;
static u32 sflash_size = 0x01000000;
static u32 sflash_type = 0x0;
/****************************************************************************/

/*
 * SPI device driver setup and teardown
 */
struct spi_device_id {
        char name[32];
        unsigned long driver_data      /* Data private to the driver */
                        __attribute__((aligned(sizeof(unsigned long))));
};


#define JEDEC_MFR(_jedec_id)    ((_jedec_id) >> 16)

struct flash_info {
        /* JEDEC id zero means "no ID" (most older chips); otherwise it has
         * a high byte of zero plus three data bytes: the manufacturer id,
         * then a two byte device id.
         */
        u32             jedec_id;
        u16             ext_id;

        /* The size listed here is what works with OPCODE_SE, which isn't
         * necessarily called a "sector" by the vendor.
         */
        unsigned        sector_size;
        u16             n_sectors;

        u16             page_size;
        u16             addr_width;

        u16             flags;
#define SECT_4K         0x01            /* OPCODE_BE_4K works uniformly */
#define M25P_NO_ERASE   0x02            /* No erase command needed */
};

#define INFO(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags)      \
        ((unsigned long)&(struct flash_info) {                         \
                .jedec_id = (_jedec_id),                                \
                .ext_id = (_ext_id),                                    \
                .sector_size = (_sector_size),                          \
                .n_sectors = (_n_sectors),                              \
                .page_size = 256,                                       \
                .flags = (_flags),                                      \
        })

/* NOTE: double check command sets and memory organization when you add
 * more flash chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 */
static struct spi_device_id m25p_ids[] = {
        /* Atmel -- some are (confusingly) marketed as "DataFlash" */
        { "at25fs010",  INFO(0x1f6601, 0, 32 * 1024,   4, SECT_4K) },
        { "at25fs040",  INFO(0x1f6604, 0, 64 * 1024,   8, SECT_4K) },

        { "at25df041a", INFO(0x1f4401, 0, 64 * 1024,   8, SECT_4K) },
        { "at25df321a", INFO(0x1f4701, 0, 64 * 1024,  64, SECT_4K) },
        { "at25df641",  INFO(0x1f4800, 0, 64 * 1024, 128, SECT_4K) },

        { "at26f004",   INFO(0x1f0400, 0, 64 * 1024,  8, SECT_4K) },
        { "at26df081a", INFO(0x1f4501, 0, 64 * 1024, 16, SECT_4K) },
        { "at26df161a", INFO(0x1f4601, 0, 64 * 1024, 32, SECT_4K) },
        { "at26df321",  INFO(0x1f4700, 0, 64 * 1024, 64, SECT_4K) },

        /* EON -- en25xxx */
        { "en25f32", INFO(0x1c3116, 0, 64 * 1024,  64, SECT_4K) },
        { "en25p32", INFO(0x1c2016, 0, 64 * 1024,  64, 0) },
        { "en25q32b", INFO(0x1c3016, 0, 64 * 1024,  64, 0) },
        { "en25p64", INFO(0x1c2017, 0, 64 * 1024, 128, 0) },
        { "en25qh128a", INFO(0x1c7018, 0, 64 * 1024, 256, 0) },

        /* Intel/Numonyx -- xxxs33b */
        { "160s33b",  INFO(0x898911, 0, 64 * 1024,  32, 0) },
        { "320s33b",  INFO(0x898912, 0, 64 * 1024,  64, 0) },
        { "640s33b",  INFO(0x898913, 0, 64 * 1024, 128, 0) },

        /* Macronix */
        { "mx25l4005a",  INFO(0xc22013, 0, 64 * 1024,   8, SECT_4K) },
        { "mx25l8005",   INFO(0xc22014, 0, 64 * 1024,  16, 0) },
        { "mx25l1606e",  INFO(0xc22015, 0, 64 * 1024,  32, SECT_4K) },
        { "mx25l3205d",  INFO(0xc22016, 0, 64 * 1024,  64, 0) },
        { "mx25l6405d",  INFO(0xc22017, 0, 64 * 1024, 128, 0) },
	/*        { "mx25l12805d", INFO(0xc22018, 0, 64 * 1024, 256, 0) }, */
        { "mx25l12845e", INFO(0xc22018, 0, 64 * 1024, 256, 0) },
        { "mx25l12855e", INFO(0xc22618, 0, 64 * 1024, 256, 0) },
        /* 257 4 bytes address */
        { "mx25l25735e", INFO(0xc22019, 0, 64 * 1024, 512, 0) },
        /* 256 3 bytes address */
        { "mx25l25635e", INFO(0xc22019, 0, 64 * 1024, 512, 0) },
        { "mx25l25655e", INFO(0xc22619, 0, 64 * 1024, 512, 0) },
        { "mx66l51235f", INFO(0xc2201a, 0, 64 * 1024, 1024, 0) },

        /* Spansion -- single (large) sector size only, at least
         * for the chips listed here (without boot sectors).
         */
        { "s25sl004a",  INFO(0x010212,      0,  64 * 1024,   8, 0) },
        { "s25sl008a",  INFO(0x010213,      0,  64 * 1024,  16, 0) },
        { "s25sl016a",  INFO(0x010214,      0,  64 * 1024,  32, 0) },
        { "s25sl032a",  INFO(0x010215,      0,  64 * 1024,  64, 0) },
        { "s25sl032p",  INFO(0x010215, 0x4d00,  64 * 1024,  64, SECT_4K) },
        { "s25sl064a",  INFO(0x010216,      0,  64 * 1024, 128, 0) },
        { "s25fl256s0", INFO(0x010219, 0x4d00, 256 * 1024, 128, 0) },
        { "s25fl256s1", INFO(0x010219, 0x4d01,  64 * 1024, 512, 0) },
        { "s25fl512s",  INFO(0x010220, 0x4d00, 256 * 1024, 256, 0) },
        { "s70fl01gs",  INFO(0x010221, 0x4d00, 256 * 1024, 256, 0) },
        { "s25sl12800", INFO(0x012018, 0x0300, 256 * 1024,  64, 0) },
        /* { "s25sl12801", INFO(0x012018, 0x0301,  64 * 1024, 256, 0) }, */
        { "s25fl129p0", INFO(0x012018, 0x4d00, 256 * 1024,  64, 0) },
        { "s25fl129p1", INFO(0x012018, 0x4d01,  64 * 1024, 256, 0) },
        { "s25fl016k",  INFO(0xef4015,      0,  64 * 1024,  32, SECT_4K) },
        { "s25fl064k",  INFO(0xef4017,      0,  64 * 1024, 128, SECT_4K) },

        /* SST -- large erase sizes are "overlays", "sectors" are 4K */
        { "sst25vf040b", INFO(0xbf258d, 0, 64 * 1024,  8, SECT_4K) },
        { "sst25vf080b", INFO(0xbf258e, 0, 64 * 1024, 16, SECT_4K) },
        { "sst25vf016b", INFO(0xbf2541, 0, 64 * 1024, 32, SECT_4K) },
        { "sst25vf032b", INFO(0xbf254a, 0, 64 * 1024, 64, SECT_4K) },
        { "sst25wf512",  INFO(0xbf2501, 0, 64 * 1024,  1, SECT_4K) },
        { "sst25wf010",  INFO(0xbf2502, 0, 64 * 1024,  2, SECT_4K) },
        { "sst25wf020",  INFO(0xbf2503, 0, 64 * 1024,  4, SECT_4K) },
        { "sst25wf040",  INFO(0xbf2504, 0, 64 * 1024,  8, SECT_4K) },

        /* ST Microelectronics -- newer production may have feature updates */
        { "m25p05",  INFO(0x202010,  0,  32 * 1024,   2, 0) },
        { "m25p10",  INFO(0x202011,  0,  32 * 1024,   4, 0) },
        { "m25p20",  INFO(0x202012,  0,  64 * 1024,   4, 0) },
        { "m25p40",  INFO(0x202013,  0,  64 * 1024,   8, 0) },
        { "m25p80",  INFO(0x202014,  0,  64 * 1024,  16, 0) },
        { "m25p16",  INFO(0x202015,  0,  64 * 1024,  32, 0) },
        { "m25p32",  INFO(0x202016,  0,  64 * 1024,  64, 0) },
        { "m25p64",  INFO(0x202017,  0,  64 * 1024, 128, 0) },
        { "m25p128", INFO(0x202018,  0, 256 * 1024,  64, 0) },

        { "m25p05-nonjedec",  INFO(0, 0,  32 * 1024,   2, 0) },
        { "m25p10-nonjedec",  INFO(0, 0,  32 * 1024,   4, 0) },
        { "m25p20-nonjedec",  INFO(0, 0,  64 * 1024,   4, 0) },
        { "m25p40-nonjedec",  INFO(0, 0,  64 * 1024,   8, 0) },
        { "m25p80-nonjedec",  INFO(0, 0,  64 * 1024,  16, 0) },
        { "m25p16-nonjedec",  INFO(0, 0,  64 * 1024,  32, 0) },
        { "m25p32-nonjedec",  INFO(0, 0,  64 * 1024,  64, 0) },
        { "m25p64-nonjedec",  INFO(0, 0,  64 * 1024, 128, 0) },
        { "m25p128-nonjedec", INFO(0, 0, 256 * 1024,  64, 0) },

        { "m45pe10", INFO(0x204011,  0, 64 * 1024,    2, 0) },
        { "m45pe80", INFO(0x204014,  0, 64 * 1024,   16, 0) },
        { "m45pe16", INFO(0x204015,  0, 64 * 1024,   32, 0) },

        { "m25pe80", INFO(0x208014,  0, 64 * 1024, 16,       0) },
        { "m25pe16", INFO(0x208015,  0, 64 * 1024, 32, SECT_4K) },

        { "m25px32",    INFO(0x207116,  0, 64 * 1024, 64, SECT_4K) },
        { "m25px32-s0", INFO(0x207316,  0, 64 * 1024, 64, SECT_4K) },
        { "m25px32-s1", INFO(0x206316,  0, 64 * 1024, 64, SECT_4K) },
        { "m25px64",    INFO(0x207117,  0, 64 * 1024, 128, 0) },

        /* Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB */
        { "w25x10", INFO(0xef3011, 0, 64 * 1024,  2,  SECT_4K) },
        { "w25x20", INFO(0xef3012, 0, 64 * 1024,  4,  SECT_4K) },
        { "w25x40", INFO(0xef3013, 0, 64 * 1024,  8,  SECT_4K) },
        { "w25x80", INFO(0xef3014, 0, 64 * 1024,  16, SECT_4K) },
        { "w25x16", INFO(0xef3015, 0, 64 * 1024,  32, SECT_4K) },
        { "w25x32", INFO(0xef3016, 0, 64 * 1024,  64, SECT_4K) },
        { "w25q32", INFO(0xef4016, 0, 64 * 1024,  64, SECT_4K) },
        { "w25x64", INFO(0xef3017, 0, 64 * 1024, 128, SECT_4K) },
        { "w25q64", INFO(0xef4017, 0, 64 * 1024, 128, SECT_4K) },
        { "w25q128", INFO(0xef4018, 0, 64 * 1024, 256, SECT_4K) },

        /* Catalyst / On Semiconductor -- non-JEDEC */
        { },
};

static void write_flash_ctrl_reg(unsigned int ofs, unsigned int data)
{
	//printf("ew %X %X\n",(unsigned long *)ofs, data);
	writel(data, (unsigned long *)ofs);
}

static unsigned int read_flash_ctrl_reg(unsigned int ofs)
{
	//printf("dw %X %X\n", (unsigned long *)ofs, readl((unsigned long *)ofs));
	return readl((unsigned long *)ofs);
}

static int ca77xx_spansion_cmd(u32 opcode, u32 addr, u32 data)
{
	unsigned long tmp;
	unsigned long timebase;

	write_flash_ctrl_reg(FLASH_SF_ACCESS, opcode | g_chipen);
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, addr);
	write_flash_ctrl_reg(FLASH_SF_DATA, data);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

	timebase = get_timer( 0 );
	do {
		tmp = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		if ((tmp & SF_START_BIT_ENABLE) == 0) {
			return 0;
			/*FLASH_FLASH_ACCESS_START bit 1, sflashRegReq = 0,
			Cleared by hardware when the action is completed.*/
		}
	} while (get_timer(timebase) < SFLASH_CMD_TIMEOUT);

	return -1;
}

static int wait_flash_status(u32 flag, u32 value, u32 wait_msec)
{
	unsigned long status;
	unsigned long timebase;

	timebase = get_timer(0);
	do {
		if (ca77xx_spansion_cmd(SFLASH_READ_STATUS_OPCODE, 0, 0)) {
			return -1;
		}

		status = read_flash_ctrl_reg(FLASH_SF_DATA);
		if ((status & flag) == value) {
			return 0;
		}

	} while (get_timer(timebase) < wait_msec);

	return -1;
}

static int wait_sflash_cmd_rdy(int timeout)
{
	int tmp;
	int cnt;
	cnt = 0;
	do {
		cnt ++;
		udelay(1);
		tmp = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		if (((tmp >> 1) & 0x1) == 0) {
			udelay(15);
			return 0;
		}
	} while ( cnt < (timeout * 1000) );
	return -1;
}

int
spinor_mem_cmp(u32 dst1, u32 src1, u32 length1)
{
    u32    i, length;
    u32    dst, src;
    u32    dst_value, src_value;
    u8     error;

    // printf("\nSPIcmp().");
    printf("\ndst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

    // dst = dst1+0xbd000000;
    dst = dst1;
    src = src1;
    length = length1;

    // printf("\ndst=%x,src=%x,length=%x\n",dst,src,length);
    error = 0;
    for (i = 0; i < length; i += 4) {
        dst_value = *(volatile unsigned int *) (dst + i);
        src_value = *(volatile unsigned int *) (src + i);
        if (dst_value != src_value) {
            printf("\n->%dth data(dst=%x ,src=%x) error\n", i, dst_value,
                   src_value);
            error = 1;
            // return 1;
        }
    }
    if (!error) {
        printf("\n->Compare OK\n");
    }
    return error;
}

/*
 * spinor_read_status- send command 0xf to the SPI Nor status register
 * Description:
 *    After read, write, or erase, the Nor device is expected to set the
 *    busy status.
 *    This function is to allow reading the status of the command: read,
 *    write, and erase.
 *    Once the status turns to be ready, the other status bits also are
 *    valid status bits.
 */
static int spinor_read_status(uint8_t * status)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

	/* Get feature use 1 byte address as sub-command.
	 * We must use extend access code to meet this protocol.
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_READ_STATUS_REG;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_STATUS_REG;
	/* No dummy cnt */
	sf_ext_access.bf.sflashDummyCount = -1;
	/* 1 byte address */
	sf_ext_access.bf.sflashAddrCount = 0x7;
	/* 1 byte data */
	sf_ext_access.bf.sflashDataCount = 0;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Command to get status */
	//write_flash_ctrl_reg(FLASH_SF_ADDRESS, REG_STATUS);

	/* Issue command */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	/* Read out status */
	reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	*status = reg_data & 0xFF;

	return 0;
}


/* spinor_write_status */
static int spinor_write_status(uint8_t addr, uint8_t reg_v)
{
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

	/* Set feature use 1 byte address as sub-command.
	 * We must use extend access code to meet this protocol.
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_WRITE_STATUS_CONF_REG;
	/* sflashAcCode = 0xf */
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.wrd = 0;
	/* sflashOpCode = 0x01 */
	sf_ext_access.bf.sflashOpCode = CMD_WRITE_STATUS_CONF_REG;
	/* 1 byte data */
	sf_ext_access.bf.sflashDataCount = 0;
	/* 1 byte address */
	sf_ext_access.bf.sflashAddrCount = 0x7;
	/* No dummy cnt */
	sf_ext_access.bf.sflashDummyCount = -1;
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Command to get status */
	//write_flash_ctrl_reg(FLASH_SF_ADDRESS, addr);

	/* Data register */
	write_flash_ctrl_reg(FLASH_SF_DATA, reg_v);

	/* Issue command */
	/* sflashRegReq = 1 */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
	wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

	return 0;
}

/*
 * spinor_read_bp_reg- send command 0x05 with address 0xa0 to get Block Protect register
 * Description:
 */
static int spinor_read_bp_reg(uint8_t * status)
{

	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

	/* Get feature use 1 byte address as sub-command.
	 * We must use extend access code to meet this protocol.
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_READ_STATUS_REG;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_STATUS_REG;
	/* No dummy cnt */
	sf_ext_access.bf.sflashDummyCount = -1;
	/* 1 byte address */
	sf_ext_access.bf.sflashAddrCount = 0x7;
	/* 1 byte data */
	sf_ext_access.bf.sflashDataCount = 0;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Command to get status */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, REG_BLOCK_LOCK);

	/* Issue command */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	/* Read out status */
	reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	*status = reg_data & 0xFF;

	return 0;
}

/*
 * spinor_read_status- send command 0xf to the SPI Nor status register
 * Description:
 *    After read, write, or erase, the Nor device is expected to set the
 *    busy status.
 *    This function is to allow reading the status of the command: read,
 *    write, and erase.
 *    Once the status turns to be ready, the other status bits also are
 *    valid status bits.
 */
static int spinor_read_otp_status(uint8_t * status)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

	/* Get feature use 1 byte address as sub-command.
	 * We must use extend access code to meet this protocol.
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_CONF_REG;
	/* No dummy cnt */
	sf_ext_access.bf.sflashDummyCount = -1;
	/* 1 byte address */
	sf_ext_access.bf.sflashAddrCount = 7;
	/* 1 byte data */
	sf_ext_access.bf.sflashDataCount = 0;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Command to get status */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, REG_OTP);

	/* Issue command */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
	wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

	/* Read out status */
	reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	*status = reg_data & 0xFF;

	return 0;
}

/*
 * spinor_Get_feature_reg- send command 0x0f with address 0xa0 to Set Feature register
 * Description:
 */
void spinor_get_feature_reg(void)
{
	u8 tmp_var;
	tmp_var = 0;
	spinor_read_status(&tmp_var);
	printf("Get Feature Status:0x%x\n", tmp_var);
}

/*
 * spinor_set_feature_reg- send command 0x1f with address 0x to Set Feature register
 * Description:
 */
void spinor_set_feature_reg(uint8_t addr, uint8_t reg_v)
{
	spinor_write_status(addr, reg_v);
	printf("Set Feature: addr = 0x%x, reg_v = 0x%x\n", addr, reg_v);
}

/* wait_till_ready- Check device status
 * Description:
 *    Read device status register until timeout or OIP cleared.
 *
 */
#define MAX_WAIT_MS  			(10)               /* erase might take 10ms */
#define STATUS_E_FAIL_MASK              0x04
#define STATUS_E_FAIL                   (1 << 2)
#define STATUS_P_FAIL_MASK              0x08
#define STATUS_P_FAIL                   (1 << 3)
static int wait_till_ready(void)
{
        int retval;
        u8 stat = 0;
        unsigned long timebase;

        timebase = 0;// get_timer(0);
        do
        {
                retval = spinor_read_status(&stat);
                if (retval < 0)
                        return -1;
                else if ((stat & (STATUS_E_FAIL_MASK | STATUS_P_FAIL_MASK)))
                        break;
                else if (!(stat & STATUS_BUSY))
                        break;
                mdelay(1);
                timebase++;
        }
        while (timebase < MAX_WAIT_MS);

        if (stat & (STATUS_E_FAIL_MASK | STATUS_P_FAIL_MASK))
        {
		printf("Staus[0x%X] error!\n", stat);
                return -1;
        }

        if ((stat & STATUS_BUSY) == 0)
                return 0;

        return -1;
}

/*
 * spinor_write_enable- Enable write operation(erase,program)
 * Description:
 *    Enable write operation
 */
static int spinor_write_enable(void)
{
	FLASH_SF_ACCESS_t sf_access;

	/* Write enable
	 * Opcode only protocol
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_WR_ENABLE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	/* Wait command ready */
	wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

	return 0;
}

/*
 * spinor_write_disable- Enable write operation(erase,program)
 * Description:
 *    Disable write operation
 */
static int spinor_write_disable(void)
{
	FLASH_SF_ACCESS_t sf_access;

	/* Write disable
	 * Opcode only protocol
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_WR_DISABLE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);
	return 0;
}


/*
 * spinor_reset- Reset no operation
 * Description:
 *    Reset: reset no operation
 */
static int spinor_reset_nop(void)
{
	FLASH_SF_ACCESS_t sf_access;

	/* Reset SPI-NOR ENABLE
	 * Opcode only protocol
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_RESET_NOP;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	udelay(15);

	return 0;
}

/*
 * spinor_reset- Reset enable
 * Description:
 *    Reset: reset enable
 */
static int spinor_reset_enable(void)
{
	FLASH_SF_ACCESS_t sf_access;

	/* Reset SPI-NOR ENABLE
	 * Opcode only protocol
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_RESET_ENABLE;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	udelay(15);

	return 0;
}

/*
 * spinor_reset- Reset SPI Nor
 * Description:
 *    Reset: reset SPI Nor memory
 */
static int spinor_reset_memory(void)
{
	FLASH_SF_ACCESS_t sf_access;
	sf_access.wrd = 0;

	/* Reset SPI-NOR
	 * Opcode only protocol
	 */
	sf_access.bf.sflashOpCode = CMD_RESET_MEMORY;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	udelay(15);

	return 0;
}

/*
 * spinor_reset- Reset SPI Nor device
 * Description:
 *    Reset: reset SPI Nor device
 */
static int spinor_reset(void)
{
	FLASH_SF_ACCESS_t sf_access;

	/* Reset SPI-NOR FLASH
	 * Opcode only protocol
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_RESET;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	mdelay(500);

	return 0;
}

static int spinor_read_direct(struct spi_flash *flash,
			     u32 offset, size_t len, void *buf)
{
	unsigned long old_type;
	unsigned char *src;

	old_type = read_flash_ctrl_reg( FLASH_TYPE);
	write_flash_ctrl_reg( FLASH_TYPE, sflash_type );

	//debug("%s: old_type=0x%x sflash_type=0x%x\n", __func__, old_type, sflash_type);
	write_flash_ctrl_reg( FLASH_SF_ACCESS, g_chipen);
	src= (unsigned char *)((unsigned int )(CONFIG_SYS_FLASH_BASE) + offset);
	memcpy( buf,  src, len);

	write_flash_ctrl_reg( FLASH_TYPE, old_type);

	printf("%s Direct Mode Read finish!!!\n", __func__);
	return 0;
}

static int spinor_write_direct(struct spi_flash *flash,
			  u32 offset, size_t len, const void *buf)
{
	unsigned long old_type;
	unsigned long opcode,size;
	unsigned char *src, *dst;

	old_type = read_flash_ctrl_reg(FLASH_TYPE);
	write_flash_ctrl_reg(FLASH_TYPE, sflash_type);
	//debug("%s: old_type=0x%x sflash_type=0x%x\n", __func__, old_type, sflash_type);
	//if ((offset % sflash_page_size) || (len % sflash_page_size)) {
	//	write_flash_ctrl_reg(FLASH_TYPE, old_type);
	//	return -1;
	//}

	src = (unsigned char *)buf;
	dst = (unsigned char *)(CONFIG_SYS_FLASH_BASE + offset);
	while (len > 0) {

		size = sflash_page_size - ((u32)dst % sflash_page_size);
		if (size > len)
			size = len;

		if (ca77xx_spansion_cmd(SFLASH_WRITE_ENABLE_OPCODE, 0, 0)) {
			break;
		}

		if (wait_flash_status (SFLASH_STATUS_WEL, SFLASH_STATUS_WEL, SFLASH_WEL_TIMEOUT)) {
			break;
		}

		opcode = SFLASH_PROGRAM_OPCODE | g_chipen | SFLASH_FORCEBURST;
		write_flash_ctrl_reg(FLASH_SF_ACCESS, opcode);


		read_flash_ctrl_reg(FLASH_SF_ACCESS);	/* dummy read */
		//wmb();
		memcpy( dst, src, size);

		opcode = g_chipen | SFLASH_FORCETERM;
		write_flash_ctrl_reg(FLASH_SF_ACCESS, opcode);

		wait_flash_status(SFLASH_STATUS_WIP, 0, SFLASH_PROGRAM_OPCODE);


		len -= size;
		dst += size;
		src += size;

	}

	write_flash_ctrl_reg(FLASH_TYPE, old_type);

	printf("%s Direct Mode write finish!!!\n", __func__);
	return len > 0 ? -1 : 0;
}

int spinor_dmassp_read_x1(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data, tmp_timer;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	FLASH_FIFO_CONTROL_t sf_fifo_control;
	DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH_t dma_ssp_txq_base_depth;
	DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH_t dma_ssp_rxq_base_depth;
	FLASH_RXDESC_ADDR_BASE_t flash_rxdesc_addr_base;
	FLASH_TXDESC_ADDR_BASE_t flash_txdesc_addr_base;
	reg_data = 0;
	tmp_timer = 0;

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must use extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_READ_SISO;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	sf_access.bf.sflashForceBurst = 0;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_SISO;
	sf_ext_access.bf.sflashDummyCount = -1;
	sf_ext_access.bf.sflashAddrCount = sflash_addr;
	sf_ext_access.bf.sflashDataCount = (len - 1);
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	sf_fifo_control.wrd = 0;
	sf_fifo_control.bf.fifoWRTH = 3;
	sf_fifo_control.bf.fifoRDTH = 3;
	sf_fifo_control.bf.fifoClear = 1;
	sf_fifo_control.bf.fifoDbgSel = 0;
	sf_fifo_control.bf.fifoCmd = 2;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, sf_fifo_control.wrd);

	dma_ssp_txq_base_depth.wrd = 0;
	dma_ssp_txq_base_depth.bf.depth = 1;
	dma_ssp_txq_base_depth.bf.base = (FLASH_TXDESC_ADDR_BASE >> 4);

	dma_ssp_rxq_base_depth.wrd = 0;
	dma_ssp_rxq_base_depth.bf.depth = 1;
	dma_ssp_rxq_base_depth.bf.base = (FLASH_RXDESC_ADDR_BASE >> 4);

	flash_txdesc_addr_base.wrd = 0;
	flash_txdesc_addr_base.bf.buffer_addr = 0;
	flash_txdesc_addr_base.bf.buffer_len = len;
	flash_txdesc_addr_base.bf.sgm = 1;
	flash_txdesc_addr_base.bf.own = 0;

	flash_rxdesc_addr_base.wrd = 0;
	flash_rxdesc_addr_base.bf.buffer_addr = 0;
	flash_rxdesc_addr_base.bf.buffer_len = len;
	flash_rxdesc_addr_base.bf.own = 0;

	/* Start access */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_DMA_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
	}

	/* DMA begin */
	/* Set TX RX register */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL, 0); /* Disable TX */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL, 0); /* Disable RX */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, 2); /* flush TXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_CONTROL, 2); /* flush TXQ5 */
	/* enable dma tx+rx */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, 9); /* en TXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH, dma_ssp_txq_base_depth.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_CONTROL, 8); /* en RXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH, dma_ssp_rxq_base_depth.wrd);
	/* prepare TXQ descriptor */
	write_flash_ctrl_reg(FLASH_TXDESC_ADDR_BASE, (FIFO_ADDR + offset));
	write_flash_ctrl_reg((FLASH_TXDESC_ADDR_BASE + 4), flash_txdesc_addr_base.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, 1);
	/* prepare RXQ descriptor */
	write_flash_ctrl_reg(FLASH_RXDESC_ADDR_BASE, buf);
	write_flash_ctrl_reg((FLASH_RXDESC_ADDR_BASE + 4), flash_rxdesc_addr_base.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, 0);
	/* dma tx + rx */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL, 1); /* enable TX */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL, 1); /* enable RX */

	/* get interrupt event and clear interrupt */
	/* register irq handler for DMA */
	do
	{
		reg_data = read_flash_ctrl_reg(FLASH_FLASH_INTERRUPT);
		udelay(5);
		tmp_timer += 5;
		if (tmp_timer >= 2000)
		{
			write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, 1);
			printf("%s DMA wait timeout!!!\n", __func__);

			return -1;
		}
	} while (((reg_data >> 1) & 1) != 1);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, 1);

	//spinor_flash_register_irq_handler();
	/* DMA end */

	return 0;
}

static int spinor_read_dma(u32 offset, size_t len, void *buf)
{
	u32 page_id;
	u32 plane_select;

	if (len < 0) {
		return -1;
	}

	while (len >= spinor_page_size) {
		page_id = offset / spinor_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Read pages from flash's cache (page head) to ddr*/
		DMA_READ_FROM_CACHE(offset, spinor_page_size, buf);
		udelay(15);
		/* Wait until program finish */
		if (wait_till_ready()) {
			printf("%s 0 wait timeout 0x%x!\n", __func__, page_id);
			return -1;
		}

		buf += spinor_page_size;
		offset += spinor_page_size;
		len -= spinor_page_size;
	}

	/* Handle tail non-align part */
	if (len > 0) {
		page_id = offset / spinor_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Write data to cache
		 * Could be x1(03h) or x2(3bh) or x4(6bh) or fastread(0bh)
		 */

		/* Read tail data from flash's cache(page head) to ddr */
		DMA_READ_FROM_CACHE(offset, len, buf);
		udelay(15);
		/* Wait until program finish */
		if (wait_till_ready()) {
			printf("%s 1 wait timeout 0x%x!\n", __func__, page_id);
			return -1;
		}
	}

	printf("%s DMA Read from cache finish!!!\n", __func__);
	return 0;
}

int spinor_dmassp_write_x1(u32 offset, size_t len, unsigned char *buf)
{
	unsigned int reg_data, tmp_timer;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	FLASH_FIFO_CONTROL_t sf_fifo_control;
	DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH_t dma_ssp_txq_base_depth;
	DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH_t dma_ssp_rxq_base_depth;
	FLASH_RXDESC_ADDR_BASE_t flash_rxdesc_addr_base;
	FLASH_TXDESC_ADDR_BASE_t flash_txdesc_addr_base;
	reg_data = 0;
	tmp_timer = 0;

	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_PROG_PAGE_X1;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_PROG_PAGE_X1;
	sf_ext_access.bf.sflashDummyCount = -1;
	sf_ext_access.bf.sflashAddrCount = sflash_addr;
	sf_ext_access.bf.sflashDataCount = (len - 1);
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	sf_fifo_control.wrd = 0;
	sf_fifo_control.bf.fifoWRTH = 3;
	sf_fifo_control.bf.fifoRDTH = 3;
	sf_fifo_control.bf.fifoClear = 1;
	sf_fifo_control.bf.fifoDbgSel = 0;
	sf_fifo_control.bf.fifoCmd = 3;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, sf_fifo_control.wrd);

	dma_ssp_txq_base_depth.wrd = 0;
	dma_ssp_txq_base_depth.bf.depth = 1;
	dma_ssp_txq_base_depth.bf.base = (FLASH_TXDESC_ADDR_BASE >> 4);

	dma_ssp_rxq_base_depth.wrd = 0;
	dma_ssp_rxq_base_depth.bf.depth = 1;
	dma_ssp_rxq_base_depth.bf.base = (FLASH_RXDESC_ADDR_BASE >> 4);

	flash_txdesc_addr_base.wrd = 0;
	flash_txdesc_addr_base.bf.buffer_addr = 0;
	flash_txdesc_addr_base.bf.buffer_len = len;
	flash_txdesc_addr_base.bf.sgm = 1;
	flash_txdesc_addr_base.bf.own = 0;

	flash_rxdesc_addr_base.wrd = 0;
	flash_rxdesc_addr_base.bf.buffer_addr = 0;
	flash_rxdesc_addr_base.bf.buffer_len = len;
	flash_rxdesc_addr_base.bf.own = 0;

	/* Start access */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_DMA_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
	}

	/* DMA begin */
	/* Set TX RX register */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL, 0); /* Disable TX */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL, 0); /* Disable RX */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, 2); /* flush TXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_CONTROL, 2); /* flush TXQ5 */
	/* enable dma tx+rx */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, 9); /* en TXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH, dma_ssp_txq_base_depth.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_CONTROL, 8); /* en RXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH, dma_ssp_rxq_base_depth.wrd);
	/* prepare TXQ descriptor */
	write_flash_ctrl_reg(FLASH_TXDESC_ADDR_BASE, buf);
	write_flash_ctrl_reg((FLASH_TXDESC_ADDR_BASE + 4), flash_txdesc_addr_base.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, 1);
	/* prepare RXQ descriptor */
	write_flash_ctrl_reg(FLASH_RXDESC_ADDR_BASE, (FIFO_ADDR + offset));
	write_flash_ctrl_reg((FLASH_RXDESC_ADDR_BASE + 4), flash_rxdesc_addr_base.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, 0);
	/* dma tx + rx */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL, 1); /* enable TX */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL, 1); /* enable RX */

	/* get interrupt event and clear interrupt */
	/* register irq handler for DMA */
	do
	{
		reg_data = read_flash_ctrl_reg(FLASH_FLASH_INTERRUPT);
		udelay(5);
		tmp_timer += 5;
		if (tmp_timer >= 2000)
		{
			write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, 1);
			printf("%s DMA wait timeout!!!\n", __func__);

			return -1;
		}
	} while (((reg_data >> 1) & 1) != 1);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, 1);

	//spinor_flash_register_irq_handler();
	/* DMA end */

	return 0;
}

static int spinor_write_dma(u32 offset, size_t len, void *buf)
{
	u32 page_id;
	u32 plane_select;

	if (len < 0) {
		return -1;
	}

	while (len >= spinor_page_size) {
		page_id = offset / spinor_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Enable write */
		spinor_write_enable();

		/* Write data to cache
		 * Could be x1(02h) or x4(32h)
		 */
		DMA_WRITE_TO_CACHE(offset, spinor_page_size, buf);
		udelay(15);
		/* Wait until program finish */
		if (wait_till_ready()) {
			printf("%s wait timeout 0x%x!\n", __func__, page_id);
			return -1;
		}

		buf += spinor_page_size;
		offset += spinor_page_size;
		len -= spinor_page_size;
	}

	/* Handle tail non-align part */
	if (len > 0) {
		page_id = offset / spinor_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Enable write */
		spinor_write_enable();

		/* Write data to cache
		 * Could be x1(02h) or x4(32h)
		 */
		DMA_WRITE_TO_CACHE(offset, len, buf);
		udelay(15);
		/* Wait until program finish */
		if (wait_till_ready()) {
			printf("%s wait timeout 0x%x!\n", __func__, page_id);
			return -1;
		}
	}

	printf("%s DMA Write to cache finish!!!\n", __func__);
	return 0;
}

/* Fast Read from Cache */
int spinor_fast_read_cache(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	reg_data = 0;

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must use extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_FAST_READ;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	if(!BURST_MODE_OFF) {
		sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	} else {
		sf_access.bf.sflashForceBurst = 0;
	}
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_FAST_READ;
	sf_ext_access.bf.sflashDummyCount = 7;
	sf_ext_access.bf.sflashAddrCount = sflash_addr;
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length %d!\n", __func__, len);
	}

	if(!BURST_MODE_OFF) {
		/* Finish burst read  */
		sf_access.bf.sflashForceBurst = 0;
		sf_access.bf.sflashForceTerm = 1;
		write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	}
	return 0;
}

int spinor_read_cache_x1(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	printf(".");
	reg_data = 0;

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must use extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_READ_SISO;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	if(!BURST_MODE_OFF) {
		sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	} else {
		sf_access.bf.sflashForceBurst = 0;
	}
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_SISO;
	sf_ext_access.bf.sflashDummyCount = 0x3f;
	sf_ext_access.bf.sflashAddrCount = sflash_addr;
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length %d!\n", __func__, len);
	}

	if(!BURST_MODE_OFF) {
		/* Finish burst read  */
		sf_access.bf.sflashForceBurst = 0;
		sf_access.bf.sflashForceTerm = 1;
		write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	}
	return 0;
}

int spinor_read_cache_x2(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	reg_data = 0;

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	if(!BURST_MODE_OFF) {
		sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	} else {
		sf_access.bf.sflashForceBurst = 0;
	}
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_DUAL;
	sf_access.bf.sflashMData = 1;
	sf_access.bf.sflashOpCode = CMD_READ_SIDO;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_SIDO;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = 7;
	sf_ext_access.bf.sflashAddrCount = sflash_addr;
	/* 4 bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length %d!\n", __func__, len);
	}

	if(!BURST_MODE_OFF) {
		/* Finish burst read  */
		sf_access.bf.sflashForceBurst = 0;
		sf_access.bf.sflashForceTerm = 1;
		write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	}

	return 0;
}

int spinor_read_cache_x4(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	reg_data = 0;

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_QUARD;
	sf_access.bf.sflashMData = 1;
	if(!BURST_MODE_OFF) {
		sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	} else {
		sf_access.bf.sflashForceBurst = 0;
	}
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashOpCode = CMD_READ_SIQO;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_SIQO;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = 7;
	sf_ext_access.bf.sflashAddrCount = sflash_addr;
	/* (len) bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length!\n", __func__);
	}

	if(!BURST_MODE_OFF) {
		/* Finish burst read  */
		sf_access.bf.sflashForceBurst = 0;
		sf_access.bf.sflashForceTerm = 1;
		write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	}
	return 0;
}

int spinor_read_cache_Dual(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	reg_data = 0;

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_DUAL;
	sf_access.bf.sflashMAddr = 1;
	sf_access.bf.sflashMData = 1;
	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashOpCode = CMD_READ_DUAL_IO;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_DUAL_IO;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = 3;
	sf_ext_access.bf.sflashAddrCount = sflash_addr;
	/* (len) bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length!\n", __func__);
	}

	return 0;
}

int spinor_read_cache_Quad(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	reg_data = 0;

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_QUARD;
	sf_access.bf.sflashMAddr = 1;
	sf_access.bf.sflashMData = 1;
	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashOpCode = CMD_READ_QUAD_IO;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_QUAD_IO;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = 5;
	sf_ext_access.bf.sflashAddrCount = sflash_addr;
	/* 4 bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length!\n", __func__);
	}

	return 0;
}
#if 0
static int spinor_read(struct spi_flash *snorflash,
						u32 offset, size_t len, void *buf)
{
	unsigned int page_id = offset / spinor_page_size;
	unsigned int plane_select;

	/* Handle head non-align part */
	if (offset & spinor_page_sz_mask) {
		printf("%s non-alignment read :0x%x\n", __func__, page_id);
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Read first page from flash */
		READ_FROM_CACHE((offset & spinor_page_sz_mask) | plane_select,
				spinor_page_size - (offset & spinor_page_sz_mask),
				buf);

		len -= spinor_page_size - (offset & spinor_page_sz_mask);
		buf += spinor_page_size - (offset & spinor_page_sz_mask);
		offset += spinor_page_size - (offset & spinor_page_sz_mask);
	}

	while (len >= spinor_page_size) {
		page_id = offset / spinor_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Read continuous pages from flash (page head) */
		READ_FROM_CACHE(0 | plane_select, spinor_page_size, buf);
		buf += spinor_page_size;
		offset += spinor_page_size;
		len -= spinor_page_size;
	}

	/* Handle tail non-align part */
	if (len > 0) {
		page_id = offset / spinor_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Read tail data from flash(page head) */
		READ_FROM_CACHE(0|plane_select, len, buf);
	}

	return 0;
}
#endif
static int spinor_read_indirect(u32 offset, size_t len, void *buf)
{
	u32 page_id;
	u32 plane_select;

	if (len < 0) {
		return -1;
	}

	while (len >= spinor_page_size) {
		page_id = offset / spinor_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Read pages from flash's cache (page head) to ddr*/
		READ_FROM_CACHE(offset, spinor_page_size, buf);
		udelay(15);
		/* Wait until program finish */
		if (wait_till_ready()) {
			printf("%s wait timeout 0x%x!\n", __func__, page_id);
			return -1;
		}

		buf += spinor_page_size;
		offset += spinor_page_size;
		len -= spinor_page_size;
	}

	/* Handle tail non-align part */
	if (len > 0) {
		page_id = offset / spinor_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Read tail data from flash's cache(page head) to ddr */
		READ_FROM_CACHE(offset, len, buf);
		udelay(15);
		/* Wait until program finish */
		if (wait_till_ready()) {
			printf("%s wait timeout 0x%x!\n", __func__, page_id);
			return -1;
		}
	}
	printf("\n");
	printf("%s Indirect Mode Read from cache finish!!!\n", __func__);
	return 0;
}

int spinor_write_cache_x1(u32 offset, size_t len, unsigned char *buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	FLASH_FLASH_ACCESS_START_t access_start;
	printf(".");

	/* Write to cache
	 * Opcode -> 16bits offset addr -> data ....
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	if(!BURST_MODE_OFF) {
		sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	} else {
		sf_access.bf.sflashForceBurst = 0;
	}
	sf_access.bf.sflashOpCode = CMD_PROG_PAGE_X1;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_PROG_PAGE_X1;
	/* 0 dummy bits */
	sf_ext_access.bf.sflashDummyCount = -1;
	sf_ext_access.bf.sflashAddrCount = sflash_addr;
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	access_start.wrd = 0;
	access_start.bf.sflashRegReq = 1;
	access_start.bf.sflashRegCmd = 1;
	/* Burst mode */
	while (len > 0) {
		reg_data = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
		write_flash_ctrl_reg(FLASH_SF_DATA, reg_data);
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		len -= 4;
		buf += 4;
	}

	/* elapse 15us before issuing any other command */
	/* Please do NOT remove!! */
	udelay(15);

	if (wait_till_ready()) {
		printf("%s wait timeout!\n", __func__);
		return -1;
	}

	if(!BURST_MODE_OFF) {
		/* Finish burst write  */
		sf_access.bf.sflashForceBurst = 0;
		sf_access.bf.sflashForceTerm = 1;	/* Terminate burst mode */
		write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	}
	return 0;
}

int spinor_write_cache_x4(u32 offset, size_t len, unsigned char *buf)
{
	unsigned int reg_data, reg_access_start;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	FLASH_FLASH_ACCESS_START_t access_start;

	/* Write to cache
	 * Opcode -> 16bits offset addr -> data ....
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashMAddr = 1;
	sf_access.bf.sflashMData = 1;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_QUARD;
	if(!BURST_MODE_OFF) {
		sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	} else {
		sf_access.bf.sflashForceBurst = 0;
	}
	sf_access.bf.sflashOpCode = CMD_PROG_PAGE_X4;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_PROG_PAGE_X4;
	/* 0 dummy bits */
	sf_ext_access.bf.sflashDummyCount = -1;
	sf_ext_access.bf.sflashAddrCount = sflash_addr;
	/* 4 bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	access_start.wrd = 0;
	access_start.bf.sflashRegReq = 1;
	access_start.bf.sflashRegCmd = 1;
	reg_access_start = access_start.wrd;
	/* Burst mode */
	while (len > 0) {
		reg_data = buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
		write_flash_ctrl_reg(FLASH_SF_DATA, reg_data);
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, reg_access_start);
		wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

		len -= 4;
		buf += 4;
	}

	/* elapse 15us before issuing any other command */
	/* Please do NOT remove!! */
	udelay(15);

	if (wait_till_ready()) {
		printf("%s wait timeout!\n", __func__);
		return -1;
	}

	if(!BURST_MODE_OFF) {
		/* Finish burst write  */
		sf_access.bf.sflashForceBurst = 0;
		sf_access.bf.sflashForceTerm = 1;	/* Terminate burst mode */
		write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	}

	return 0;
}

int spinor_write_page(u32 page_id, size_t len, unsigned char *buf)
{
	u32 plane_select;

	plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

	/* 06h (WRITE ENABLE command)
	 * 02h (PROGRAM LOAD command) or 32h to load x4(multi-IO)
	 * 10h (PROGRAM EXECUTE command)
	 * 0Fh (GET FEATURES command to read the status)
	 */

	/* Enable write */
	spinor_write_enable();

	/* Write data to cache
	 * Could be x1(02h) or x4(32h)
	 */
	WRITE_TO_CACHE(0 | plane_select, len, buf);

	/* Wait until program finish */
	if (wait_till_ready()) {
		printf("%s wait timeout 0x%x!\n", __func__, page_id);
		return -1;
	}

	return 0;
}

static int spinor_write(struct spi_flash *snorflash,
						 u32 offset, size_t len, const void *buf)
{
	unsigned char *src = (unsigned char *)buf;

	/* Sanity check */
	if (offset & spinor_page_sz_mask) {
		printf("Non alignment write(ADDR) not supported!\nThe unit is 8hex!!!\n");
		return -1;
	}
#if 0
	if (len % spinor_page_size) {
		printf("Non alignment write(SIZE) not supported!\nThe unit is a PAGE!!!\n");
		return -1;
	}
#endif
	while (len > 0) {
		spinor_write_page(offset / spinor_page_size, spinor_page_size, src);
		offset += spinor_page_size;
		src += spinor_page_size;
		len -= spinor_page_size;
	}

	return len > 0 ? -1 : 0;
}

static int spinor_write_indirect(u32 offset, size_t len, void *buf)
{
	u32 page_id;
	u32 plane_select;

	if (len < 0) {
		return -1;
	}

	while (len >= spinor_page_size) {
		page_id = offset / spinor_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Enable write */
		spinor_write_enable();

		/* Write data to cache
		 * Could be x1(02h) or x4(32h)
		 */
		WRITE_TO_CACHE(offset, spinor_page_size, buf);
		udelay(15);
		/* Wait until program finish */
		if (wait_till_ready()) {
			printf("%s wait timeout 0x%x!\n", __func__, page_id);
			return -1;
		}

		buf += spinor_page_size;
		offset += spinor_page_size;
		len -= spinor_page_size;
	}

	/* Handle tail non-align part */
	if (len > 0) {
		page_id = offset / spinor_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Enable write */
		spinor_write_enable();

		/* Write data to cache
		 * Could be x1(02h) or x4(32h)
		 */
		WRITE_TO_CACHE(offset, len, buf);
		udelay(15);
		/* Wait until program finish */
		if (wait_till_ready()) {
			printf("%s wait timeout 0x%x!\n", __func__, page_id);
			return -1;
		}
	}
        printf("\n");
	printf("%s Indirect Mode Write to cache finish!!!\n", __func__);
	return 0;
}

int spinor_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	unsigned long old_type;

	old_type = read_flash_ctrl_reg(FLASH_TYPE);
	write_flash_ctrl_reg(FLASH_TYPE, sflash_type);
	//debug("%s: old_type=0x%x sflash_type=0x%x offset=0x%x len=0x%x sflash_sector_size=%d\n", __func__, old_type, sflash_type,offset, len, sflash_sector_size);
	if ((offset % sflash_sector_size) || (len % sflash_sector_size)) {
		write_flash_ctrl_reg(FLASH_TYPE, old_type);
		debug("SF: Erase length not sector alignment.\n");
		return -1;
	}

	while (len > 0) {

		if (ca77xx_spansion_cmd(SFLASH_WRITE_ENABLE_OPCODE, 0, 0)) {
			break;
		}

		if (wait_flash_status (SFLASH_STATUS_WEL, SFLASH_STATUS_WEL, SFLASH_WEL_TIMEOUT)) {
			break;
		}

		if (ca77xx_spansion_cmd(SFLASH_ERASE_OPCODE, offset, 0)) {
			break;
		}

		if (wait_flash_status (SFLASH_STATUS_WIP, 0, SFLASH_ERASE_TIMEOUT)) {
			break;
		}

		len -= sflash_sector_size;
		offset += sflash_sector_size;
	}

	write_flash_ctrl_reg(FLASH_TYPE, old_type);
	return len > 0 ? -1 : 0;
}

/*
 * spinor_erase_block- erase one block
 * Description:
 *    ERASE BLOCK: erase one block at once, each block is 32K byte(0x8000)
 */
int spinor_erase_block(u32 block_addr)
{
	FLASH_SF_ACCESS_t sf_access;

	/* Enable write */
	spinor_write_enable();

	/* Erase block
	 * Opcode -> 24bits Addr
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_ERASE_BLK_32KB;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_3_ADDR;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, block_addr);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
	wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

	wait_till_ready();

	printf("spinor_erase_block done!!!\n");
	return 0;
}

/*
 * spinor_reset- Reset controller
 * Description:
 *    Reset: reset SPI Nor controller
 */
void spinor_reset_controller(void)
{
	GLOBAL_BLOCK_RESET_SECURE_t global_block_reset_secure;

	global_block_reset_secure.wrd = read_flash_ctrl_reg(GLOBAL_BLOCK_RESET_SECURE);
	global_block_reset_secure.bf.reset_flash = 1;
	write_flash_ctrl_reg(GLOBAL_BLOCK_RESET_SECURE, global_block_reset_secure.wrd);

	udelay(10);

	global_block_reset_secure.wrd = 0;
	write_flash_ctrl_reg(GLOBAL_BLOCK_RESET_SECURE, global_block_reset_secure.wrd);
}

/*
 * spinor_enter_4byte_mode - enter 4 byte mode
 * Description:
 *    ENTER: 4 byte addressing
 */
int spinor_enter_4byte_mode(void)
{
	FLASH_SF_ACCESS_t sf_access;

	/*
	 * EN4B
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_ENTER_4BYTE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
	wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

	if (wait_till_ready()) {
		printf("%s wait timeout!\n", __func__);
		return -1;
	}

	sflash_addr = 3;
	printf("ENTER 4-Byte MODE done!!!\n");
        return 0;
}

/*
 * spinor_exit_4byte_mode - exit 4 byte mode
 * Description:
 *    EXIT: 4 byte addressing
 */
int spinor_exit_4byte_mode(void)
{
	FLASH_SF_ACCESS_t sf_access;

	/*
	 * EX4B
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_EXIT_4BYTE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
	wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

	if (wait_till_ready()) {
		debug("%s wait timeout!\n", __func__);
		return -1;
	}

	sflash_addr = 2;
	printf("EXIT 4-Byte MODE done!!!\n");
        return 0;
}

/*
 * Enable/disable 4-byte addressing mode.
 */
static inline int ca77xx_set_4byte(u32 jedec_id, int enable)
{

	u32 tmp;

        switch (JEDEC_MFR(jedec_id)) {
        case CFI_MFR_MACRONIX:
               // flash->command[0] = enable ? OPCODE_EN4B : OPCODE_EX4B;
               // return spi_write(flash->spi, flash->command, 1);

               	if (ca77xx_spansion_cmd(enable ? SFLASH_OPCODE_EN4B : SFLASH_OPCODE_EX4B, 0, 0)) {
			debug("SF: MACRONIX set 4 bytes error.\n");
		}
		break;
	case CFI_MFR_ST:
		/* Before issue EN4B or EX4B the WRITE ENABLE command must issued first. */
		if (ca77xx_spansion_cmd(SFLASH_WRITE_ENABLE_OPCODE, 0, 0)) {
			break;
		}

		if (wait_flash_status (SFLASH_STATUS_WEL, SFLASH_STATUS_WEL, SFLASH_WEL_TIMEOUT)) {
			break;
		}

		if (ca77xx_spansion_cmd(enable ? SFLASH_OPCODE_EN4B : SFLASH_OPCODE_EX4B, 0, 0)) {
			debug("SF: Micron flash set 4 bytes error.\n");
		}

		break;
        default:
                /* Spansion style */
                //flash->command[0] = OPCODE_BRWR;
                //flash->command[1] = enable << 7;
                //return spi_write(flash->spi, flash->command, 2);
                if (ca77xx_spansion_cmd(SFLASH_OPCODE_BRWR, 0, enable << 7)) {
			debug("SF: SFlash set 4 bytes error.\n");
		}
        }

        tmp = read_flash_ctrl_reg(FLASH_TYPE);

        if(enable)
        	sflash_type = (tmp | SFLASH_4BYTE_ADDR);
	else
		sflash_type = (tmp & ~SFLASH_4BYTE_ADDR);

	write_flash_ctrl_reg(FLASH_TYPE, sflash_type);

	return 0;
}

struct spi_device_id *spinor_read_id(void)
{
        u32                     jedec, tmp;
        /*u16                     ext_jedec;*/
        struct flash_info       *info;
	unsigned long status;

	if (ca77xx_spansion_cmd(SFLASH_READID_OPCODE, 0, 0)) {
		debug("Read SF_ID fail\n");
		return NULL;
	}

	status = read_flash_ctrl_reg(FLASH_SF_DATA);
	debug("SF_ID 0x%lX\n", status);
	jedec = ((status&0xff)<<16) | (status&0xff00) | ((status&0xff0000)>>16);
	printf("SPI_NOR ID: 0x%X\n", jedec);
	//For G2 just can read 4 bytesdata only, So without ext_jedec
        //ext_jedec = id[3] << 8 | id[4];

        for (tmp = 0; tmp < ARRAY_SIZE(m25p_ids) - 1; tmp++) {

                info = (void *)m25p_ids[tmp].driver_data;

                if (info->jedec_id == jedec) {
                        //if (info->ext_id != 0 && info->ext_id != ext_jedec)
                        //        continue;
                        return &m25p_ids[tmp];
                }
        }
        printf("SF: unrecognized SPI_NOR ID: %06X\n", jedec);

        return NULL;
}

void set_spinor_clk(void)
{
	FLASH_SF_TIMING_t sf_timing;
	sf_timing.wrd = 0;
	sf_timing.bf.sflashIdleTime = 0x37;    /* 20 clk */
	sf_timing.bf.sflashHoldTime = 0x37;    /* 20 clk */
	sf_timing.bf.sflashSetupTime = 0x37;   /* 20 clk */
	sf_timing.bf.sflashClkWidth = 07;       /* 40M Hz */
	/* sf_timing.wrd = 0x00141414 */
	write_flash_ctrl_reg(FLASH_SF_TIMING, sf_timing.wrd);
}

struct spinor_flash *spinor_flash_probe(unsigned int bus, unsigned int cs,
				  unsigned int max_hz, unsigned int spi_mode)
{
	struct spi_flash *flash;
	const struct spi_device_id *jid;
	struct flash_info               *info;
	u32 tmp;

	/* reset controller */
	/*
	writel((readl(0xf432000c) | (1<<14)), 0xf432000c);
	udelay(10);
	writel((readl(0xf432000c) & (~(1<<14))), 0xf432000c);
	*/

	/* Set FLASH_TYPE as serial flash */
	write_flash_ctrl_reg(FLASH_TYPE, 0);

	/* Set flash timing */
	set_spinor_clk();

	jid = spinor_read_id();

	if(!jid)
		return NULL;

	info = (void *)jid->driver_data;

	flash = malloc(sizeof(struct spi_flash));
	if (!flash) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	/*
         * Atmel, SST and Intel/Numonyx serial flash tend to power
         * up with the software protection bits set
         */
         debug("found %s.\n",jid->name);

        if (JEDEC_MFR(info->jedec_id) == CFI_MFR_ATMEL ||
            JEDEC_MFR(info->jedec_id) == CFI_MFR_INTEL ||
            JEDEC_MFR(info->jedec_id) == CFI_MFR_SST) {
              //  write_enable(flash);
              //  write_sr(flash, 0);
              debug("SF: Atmel, Intel or SST SFlash.\n");
        }

	flash->spi = NULL;
	flash->name = jid->name;

	/* For S25-FL512SAIFG1 page size is 512B, 64MB with RESET# pin*/
	if(info->jedec_id == 0x010220)
		info->page_size = 512;
	sflash_sector_size = info->sector_size;
	sflash_page_size = info->page_size;
	sflash_size = info->sector_size * info->n_sectors;
	/* prefer "small sector" erase if possible */
        if (info->flags & SECT_4K) {
        	debug("SF: Check OPCODE_BE_4K.\n");
                //flash->erase_opcode = OPCODE_BE_4K;
                //flash->mtd.erasesize = 4096;
        }

	flash->page_size = sflash_page_size;
	flash->sector_size = sflash_sector_size;
	flash->size = sflash_size;

	flash->write = spinor_write_direct;
	flash->erase = spinor_erase_block;
	flash->read = spinor_read_direct;

	/* enable 4-byte addressing if the device exceeds 16MiB */
        if (flash->size > 0x1000000) {
                debug("SF: 4 Bytes mode flash.\n");
		tmp = read_flash_ctrl_reg(FLASH_TYPE);
		write_flash_ctrl_reg(FLASH_TYPE, tmp | SFLASH_4BYTE_ADDR);
		sflash_type = (tmp | SFLASH_4BYTE_ADDR);

                ca77xx_set_4byte(info->jedec_id, 1);
        }
        else
        {	/* use strapping setting */
		sflash_type = read_flash_ctrl_reg(FLASH_TYPE);
              debug("SF: 3 Bytes mode flash.\n");
        }
	debug("sflash_type=0x%x\n", sflash_type);
	return flash;
}

void spinor_flash_free(struct spi_flash *snorflash)
{
	free(snorflash);
}

/* Flysky add for socCmd test item*/
void rtk_spi_nor_test(IN  void *Data)
{
	struct spi_flash *snorflash;
	u32*  InPutData;
	InPutData = (u32*)Data;

        //flash->temp_buf = temp_buf;

	switch(InPutData[0])
	{
		case 0:
			//CAUTION :  init memory and parameter , make sure this command have been excuted before any others
			//socCmd spinor 0
			//flash = spinor_flash_probe(InPutData[1], InPutData[2], InPutData[3], InPutData[4]);
			spinor_flash_probe(0, 0, 0, 0);
			break;
		case 1:
			//TODO: READ ID (Read device ID) RDID
			//socCmd spinor 1
			spinor_read_id();
			break;
		case 2:
			/**********************************************************************/
			/* RTL8198F SPI_NOR flash controller does NOT support DIRECT MODE yet */
			/**********************************************************************/
			/* DIRECT MODE(AUTO MODE) */
			switch(InPutData[1])
			{
				case 0:
					/* DIRECT MODE(AUTO MODE): READ */
					switch(InPutData[2])
					{
						case 0:
							spinor_reset_controller();
							/* DIRECT MODE: READ */
							spinor_read_direct(snorflash, InPutData[3], InPutData[4], InPutData[5]);
							break;
					}
					break;
				case 1:
					/* DIRECT MODE(AUTO MODE): WRITE */
					switch(InPutData[2])
					{
						case 0:
							spinor_reset_controller();
							/* DIRECT MODE: PAGE WRITE */
							spinor_write_direct(snorflash, InPutData[3], InPutData[4], InPutData[5]);
							break;
					}
					break;
			}
			break;
		case 3:
			/* INDIRECT MODE(USER MODE) */
			switch(InPutData[1])
			{
				case 0:
					/* INDIRECT MODE(USER MODE): READ */
					switch(InPutData[2])
					{
						case 0:
							/* INDIRECT MODE: PAGE READ (Array read), PRD */
							/* socCmd 3 0 0 offset len buf*/
							spinor_read_indirect(InPutData[3], InPutData[4], InPutData[5]);
							break;
					}
					break;
				case 1:
					/* INDIRECT MODE(USER MODE): WRITE */
					switch(InPutData[2])
					{
						case 0:
							/* INDIRECT MODE: PAGE WRITE (Array write) PW */
							/* socCmd 3 1 0 offset len buf */
							spinor_write_indirect(InPutData[3], InPutData[4], InPutData[5]);
							break;
					}
					break;
			}
			break;
		case 4:
			/* DMA MODE */
			switch(InPutData[1])
			{
				case 0:
					/* DMA MODE: READ */
					switch(InPutData[2])
					{
						case 0:
							/* DMA: PAGE READ (Array read) PRD */
							/* socCmd 4 0 0 offset len buf */
							spinor_read_dma(InPutData[3], InPutData[4], InPutData[5]);
							break;
					}
					break;
				case 1:
					/* DMA MODE: WRITE */
					switch(InPutData[2])
					{
						case 0:
							/* DMA: PAGE WRITE (Array write) PWT */
							spinor_write_dma(InPutData[3], InPutData[4], InPutData[5]);
							break;
					}
					break;
			}
			break;
		case 5:
			/* RESET FLASH */
			spinor_reset();
			break;
		case 6:
			/* ERASE CHIP */
			//spinor_erase_chip();
			break;
		case 7:
			/* ERASE */
			/* spinor_erase */
			/* socCmd 7 offset len */
			spinor_erase(snorflash, InPutData[1], InPutData[2]);
			break;
		case 8:
			/* SECTOR ERASE */
			/* socCmd 8 sector_addr */
			//spinor_erase_sector((InPutData[1] * spinor_sector_size));
			break;
		case 9:
			/* BLOCK ERASE */
			/* socCmd 9 blk_addr */
			spinor_erase_block(InPutData[1]);
			spinor_reset();
			break;
		case 10:
			/* GET FEATURES */
			spinor_get_feature_reg();
			break;
		case 11:
			/* SET FEATURES */
			/* spinor_set_feature_reg(addr, value) */
			spinor_set_feature_reg(InPutData[1], InPutData[2]);
			break;
		case 12:
			/* WRITE ENABLE */
			spinor_write_enable();
			break;
		case 13:
			/* WRITE DISABLE */
			spinor_write_disable();
			break;
		case 14:
			//TODO: Power Down: Enter Deep Power Down
			printf("Enter Deep Power Down not available\n");
			break;
		case 15:
			//TODO: Power Down: Release from Deep Power Down
			printf("Release from Deep Power Down not available\n");
			break;
		case 16:
			/* ENTER 4BYTE MODE */
			spinor_enter_4byte_mode();
			break;
		case 17:
			/* EXIT 4BYTE MODE */
			spinor_exit_4byte_mode();
			break;
		case 18:
			/* COMPARE MEM */
			/* socCmd 18 addr1 addr2 len */
			spinor_mem_cmp(InPutData[1], InPutData[2], InPutData[3]);
			break;
	}
}