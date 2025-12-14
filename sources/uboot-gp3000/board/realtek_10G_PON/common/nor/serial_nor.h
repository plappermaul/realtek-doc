#ifndef _SERIAL_NOR_H
#define _SERIAL_NOR_H

#include <linux/types.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/flashchip.h>

/*
 * Configurations for SPI NOR flash driver
 *
 * Multi-IO mode selection:
 * #define SFLASH_USE_QIO
 * #define SFLASH_USE_DIO
 * #define SFLASH_USE_SIO
 *
 * Direct or in-direct Read mode selection:
 * #define SPI_NOR_USE_MMIO
 * #define SPI_NOR_USE_PIO
 */
#define SFLASH_USE_SIO
#define SPI_NOR_USE_PIO


#ifdef __UBOOT__
#define sfprint printf
#else
#define sfprint printk
#endif


/* register FLASH_SF_ACCESS definitions */
#define SF_ACCESS_AC_OP_XLAT(OpCode, AcCode) (OpCode|(AcCode<<8))
#define SF_ACCODE                	0x0
#define SF_ACCODE_1_DATA           	0x1
#define SF_ACCODE_2_DATA           	0x2
#define SF_ACCODE_3_DATA           	0x3
#define SF_ACCODE_4_DATA           	0x4
#define SF_ACCODE_4_ADDR         	0x5
#define SF_ACCODE_4_ADDR_1_DATA    	0x6
#define SF_ACCODE_4_ADDR_2_DATA    	0x7
#define SF_ACCODE_4_ADDR_3_DATA    	0x8
#define SF_ACCODE_4_ADDR_4_DATA    	0x9
#define SF_ACCODE_4_ADDR_X_1_DATA   0xA
#define SF_ACCODE_4_ADDR_X_2_DATA   0xB
#define SF_ACCODE_4_ADDR_X_3_DATA   0xC
#define SF_ACCODE_4_ADDR_X_4_DATA   0xD
#define SF_ACCODE_4_ADDR_4X_1_DATA  0xE
#define SF_ACCODE_EXTEND_MODE       0xF

typedef enum {
	sf_sio = 0x0,
	sf_dio = 0x1,
	sf_qio = 0x2,
} sflashMIO_mode_t;


/* register FLASH_SF_EXT_ACCESS definitions */



/* Command info definitions */
typedef struct {
	uint8_t cmd;
	uint8_t addr_byte;
	uint32_t to_us;
	uint32_t sz_b;
	s32 offset_lmt; /* For non-uniform sector devices,
	                     +: from the begining; -: from the tail; 0: no limit. I.e.,
	                     first 128KB = +0x20000; the last 128KB: = -0x20000. */
} norsf_erase_cmd_t;

typedef struct {
    uint8_t cmd;
    uint8_t addr_byte;
    uint8_t cmd_io;
    uint8_t addr_io;
    uint8_t data_io;
    uint32_t cprog_to_us;
    uint32_t cprog_lim_b;
} norsf_write_cmd_t;


typedef int32_t (fpint_t)(void);
typedef int32_t (fpint_uint32_t)(uint32_t);


typedef struct {
    uint8_t  cmd;
    uint8_t  addr_byte;
    uint8_t  dummy_ck;
    uint32_t predictor;
    uint8_t  cmd_io;
    uint8_t  addr_io;
    uint8_t  data_io;
	fpint_t *xread_en;
	fpint_t *xread_ex;
} norsf_read_cmd_t;

typedef struct {
	norsf_erase_cmd_t *cerase;
	norsf_write_cmd_t *cprog;
	norsf_read_cmd_t *cread;
    int (*wait_wip)(uint32_t wait_msec);
    int (*wait_wel)(void);
} norsf_cmd_info_t;



/* Command for SPI NOR PIO functions */
typedef enum {
	attr_tx = 0,
    attr_rx = 1,
    attr_op_only = 2,
} attr_op_dir_t;

typedef struct {
	uint8_t		cmd;
	uint8_t		n_addr;		/* Number of address */
	uint8_t		addr[4];	/* Reg Offset */
	uint8_t		n_dummy;	/* Number of dummy cycles */
	uint32_t	n_data;		/* Number of tx bytes */
	uint8_t	    *data_buf;	/* data buf */
	uint8_t     op_dir;     /* write or read data, refer to attr_op_dir_t s*/
} norsf_cmd_attr_t;

#ifdef __UBOOT__
/*
 * SPI device driver setup and teardown
 */
struct spi_device_id {
    char name[32];
    unsigned long driver_data      /* Data private to the driver */
    __attribute__((aligned(sizeof(unsigned long))));
};
#endif


typedef enum {
    NEED_QE       = (1<<4),
} info_flag_t;

struct flash_info {
    /* JEDEC id zero means "no ID" (most older chips); otherwise it has
     * a high byte of zero plus three data bytes: the manufacturer id,
     * then a two byte device id.
     */
    uint32_t             jedec_id;
    uint16_t             ext_id;

    /* The size listed here is what works with OPCODE_SE, which isn't
         * necessarily called a "sector" by the vendor.
         */
    uint32_t             sector_size;
    uint16_t             n_sectors;

    uint16_t             page_size;
    uint16_t             addr_width;

    uint16_t             flags;
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


/*
  * Flash opcodes.
  */
#define MXIC_OP_READ_STATUS        (0x05)
#define MXIC_OP_READ_CONFIGURATION (0x15)
#define MXIC_OP_WRITE_STATUS       (0x01)

#define WINBOND_OP_READ_STATUS2    (0x35)
#define WINBOND_OP_WRITE_STATUS2   (0x31)

#define SPINOR_OP_READ_STATUS      (0x05)

/* NOR Flash status register state */
#define SFLASH_STATUS_WEL      	0x02
#define SFLASH_STATUS_WIP       0x01
#define SFLASH_STATUS_BP		0x1C
#define SFLASH_STATUS_SRWD      0x80

#define SFLASH_WEL_TIMEOUT		8000
#define SFLASH_PROGRAM_TIMEOUT	8000
#define SFLASH_ERASE_TIMEOUT	400000
#define SFLASH_CMD_TIMEOUT		40

#undef SFLASH_QIO_PROHIBIT

/*
  * Functions for extern
  */
int serial_nor_compound_cmd(norsf_cmd_attr_t *cmd_attr);
int serial_nor_write_status_reg(uint8_t wdsr_cmd, uint16_t status, uint8_t data_cnt);
int serial_nor_read_status_reg(uint8_t rdsr_cmd);
void serial_nor_configure_cmd_info(norsf_cmd_info_t *cmd, struct flash_info *info);
struct spi_device_id *serial_nor_scan_id_table(uint32_t rdid);

#endif /* #ifndef _SERIAL_NOR_H */

