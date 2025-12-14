#ifndef __MMC_SD_H__
#define __MMC_SD_H__

#ifndef SDCARD_BLOCK_SIZE
#define SDCARD_BLOCK_SIZE         512
#endif // from ./emmc/include/sysdefs.h:18

#define DCACHE_FLUSH(start, size) sihnah_flush_dcache_range(start, size)

//#define CR_DEBUG
//#define sync()

#ifndef REG8
#define REG8(addr)  (*((volatile uint8_t *)(addr)))
#define REG16(addr) (*((volatile uint16_t *)(addr)))
#define REG32(addr) (*((volatile uint32_t *)(addr)))
#define REG64(addr) (*((volatile uint64_t *)(addr))
#endif

/* realtek definition */
#ifdef CR_DEBUG
#define cr_writel(value,addr)   ({ REG32(addr) = value; printf("%llx=%llx\n", (uint64_t)(addr), (uint64_t)(value)); })
#define cr_readl(addr)          REG32(addr)
#define cr_writeb(value,addr)   ({ REG8(addr) = value; printf("%llx=%llx\n", (uint64_t)(addr), (uint64_t)(value)); })
#define cr_readb(addr)          REG8(addr)
#else
#define cr_writel(value,addr)   REG32(addr) = value
#define cr_readl(addr)          REG32(addr)
#define cr_writeb(value,addr)   REG8(addr) = value
#define cr_readb(addr)          REG8(addr)
#endif

#if 0
#define SIG_LEN			    4
typedef struct _IMG_HEADER_TYPE_
{
	unsigned char signature[SIG_LEN];
	unsigned long startAddr;
	unsigned long burnAddr;
	unsigned long len;
} IMG_HEADER_TYPE, *PIMG_HEADER_TYPE;

#define REG_SYS_EMMC_SD_1       (BIT_ACTIVE_LX1ARB|BIT_ACTIVE_LX1)
#define REG_SYS_EMMC_SD_2       (BIT_ACTIVE_SD30|BIT_ACTIVE_SD30_PLL5M)
#define ENABLE_EMMC_SD  \
    do { \
        REG32(REG_CLK_MANAGE) = REG32(REG_CLK_MANAGE) | REG_SYS_EMMC_SD_1;      \
        REG32(REG_CLK_MANAGE2) = REG32(REG_CLK_MANAGE2) | REG_SYS_EMMC_SD_2;    \
    } while(0);

#define ON                      0
#define OFF                     1

#define DMA_CMD 3
#define R_W_CMD 2   //read/write command
#define INN_CMD 1   //command work chip inside
#define UIN_CMD 0   //no interrupt rtk command

#define RTK_TRANS_FAIL 6
#define RTK_CPU_TOUT 5 /* cpu mode timeout */
#define RTK_TOUT1 4  /* time out 2nd time */
#define RTK_FAIL 3  /* DMA error & cmd parser error */
#define RTK_RMOV 2  /* card removed */
#define RTK_TOUT 1  /* time out include DMA finish & cmd parser finish */
#define RTK_SUCC 0

#define BYTE_CNT            0x200
#define DMA_BUFFER_SIZE     128*512
#define RTK_NORM_SPEED      0x00
#define RTK_HIGH_SPEED      0x01
#define RTK_1_BITS          0x00
#define RTK_4_BITS          0x10
//#define RTK_8_BITS          0x11
#define RTK_BITS_MASK       0x30
#define RTK_SPEED_MASK      0x01
#define RTK_PHASE_MASK      0x06

/* send status event */
#define STATE_IDLE  0
#define STATE_READY 1
#define STATE_IDENT 2
#define STATE_STBY  3
#define STATE_TRAN  4
#define STATE_DATA  5
#define STATE_RCV   6
#define STATE_PRG   7
#define STATE_DIS   8

#define SDCARD_VDD_30_31  0x00040000  /* VDD voltage 3.0 ~ 3.1 */
#define SDCARD_VDD_31_32  0x00080000  /* VDD voltage 3.1 ~ 3.2 */
#define SDCARD_VDD_32_33  0x00100000  /* VDD voltage 3.2 ~ 3.3 */
#define SDCARD_VDD_33_34  0x00200000  /* VDD voltage 3.3 ~ 3.4 */
#define SDCARD_VDD_34_35  0x00400000  /* VDD voltage 3.4 ~ 3.5 */
#define SDCARD_VDD_35_36  0x00800000  /* VDD voltage 3.5 ~ 3.6 */
#define SDCARD_CARD_BUSY  0x80000000  /* Card Power up status bit */

#define ROM_SDCARD_VDD    (SDCARD_VDD_33_34|SDCARD_VDD_32_33|SDCARD_VDD_31_32|SDCARD_VDD_30_31)
#define SDCARD_SCT_ACC    0x40000000  /* host support sector mode access */

/* static UINT8    addr_mode */
#define ADDR_MODE_BIT   1
#define ADDR_MODE_SEC   0

/* log display option */
#define SHOW_CID
#define SHOW_CSD
#define SHOW_EXT_CSD

#ifndef MMC_RSP_136
#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136     (1 << 1)        /* 136 bit response */
#define MMC_RSP_CRC     (1 << 2)        /* expect valid crc */
#define MMC_RSP_BUSY    (1 << 3)        /* card may send busy */
#define MMC_RSP_OPCODE  (1 << 4)        /* response contains opcode */

#define mmc_cmd_type(cmd)   ((cmd)->flags & MMC_CMD_MASK)
#define MMC_CMD_MASK    (3 << 5)        /* non-SPI command type */
#define MMC_CMD_AC      (0 << 5)
#define MMC_CMD_ADTC    (1 << 5)
#define MMC_CMD_BC      (2 << 5)
#define MMC_CMD_BCR     (3 << 5)

#define MMC_RSP_SPI_S1  (1 << 7)        /* one status byte */
#define MMC_RSP_SPI_S2  (1 << 8)        /* second byte */
#define MMC_RSP_SPI_B4  (1 << 9)        /* four data bytes */
#define MMC_RSP_SPI_BUSY (1 << 10)      /* card may send busy */
/*
 * These are the native response types, and correspond to valid bit
 * patterns of the above flags.  One additional valid pattern
 * is all zeros, which means we don't expect a response.
 */
#define MMC_RSP_NONE    (0)
#define MMC_RSP_R1      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1B     (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)
#define MMC_RSP_R2      (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3      (MMC_RSP_PRESENT)
#define MMC_RSP_R4      (MMC_RSP_PRESENT)
#define MMC_RSP_R5      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R6      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

#define mmc_resp_type(cmd)  ((cmd)->flags & (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC|MMC_RSP_BUSY|MMC_RSP_OPCODE))
#endif

#endif
#if 0
/*
 * Standard errno values are used for errors, but some have specific
 * meaning in the MMC layer:
 *
 * ETIMEDOUT    Card took too long to respond
 * EILSEQ       Basic format problem with the received or sent data
 *              (e.g. CRC check failed, incorrect opcode in response
 *              or bad end bit)
 * EINVAL       Request cannot be performed because of restrictions
 *              in hardware and/or the driver
 * ENOMEDIUM    Host can determine that the slot is empty and is
 *              actively failing requests
 */
/* liao ******************************************************************** */
#define MMC_ERR_NONE        0
#define MMC_ERR_TIMEOUT     1
#define MMC_ERR_BADCRC      2
#define MMC_ERR_RMOVE       3
#define MMC_ERR_FAILED      4
#define MMC_ERR_INVALID     5
#endif

#define MMCORE_ERR_XFER_FAIL     (1)
#define MMCORE_ERR_UNSUP_CMD     (2)
#define MMCORE_ERR_BAD_CRC       (3)
#define MMCORE_ERR_UNALIGNED_DMA (4)    // unalinged DMA buffer

/************************************************************************
 *  Structure
 ************************************************************************/
struct mmcore_rsp_type {
    uint32_t cmdidx:8;
    uint32_t type:8;
    uint32_t resp:8;
    uint32_t grp:8;
};

struct mmcore_cmd_para {
    uint32_t resptype;
    uint8_t  rsp_para1;
    uint8_t  rsp_para2;
    uint8_t  rsp_para3;
    uint8_t  cmdcode;
    uint8_t  rsp_len;
};

struct mmcore_cmd_info {
    uint32_t idx;
    uint32_t arg;
    struct mmcore_cmd_para para;
    uint32_t *resp;
    uint32_t timeout;
    uint8_t  err_case;
};

struct mmcore_data_info {
    uint32_t dir;
    uint32_t block_count;
    uint32_t block_size;
    uint8_t *buffer;
};

void mmcore_init(void);
void mmcore_set_mode(uint32_t mode);
void mmcore_set_hs_mode(uint32_t hsmode);
void mmcore_set_bits(uint32_t bits);
void mmcore_set_div(uint32_t freq);
int mmcore_send_cmd(struct mmcore_cmd_info *cmd,
                    struct mmcore_data_info *data);

#define MCMD0_GO_IDLE_STATE            (0)
#define MCMD1_SEND_OP_COND             (1)
#define MCMD2_ALL_SEND_CID             (2)
#define MCMD3_SET_RELATIVE_ADDR        (3)
#define MCMD4_SET_DSR                  (4)
#define MCMD5_SLEEP_AWAKE              (5)
#define MCMD6_SWITCH                   (6)
#define MCMD7_SELECT_CARD              (7)
#define MCMD8_SEND_EXT_CSD             (8)
#define MCMD9_SEND_CSD                 (9)
#define MCMD10_SEND_CID                (10)
#define MCMD12_STOP_TRANSMISSION       (12)
#define MCMD13_SEND_STATUS             (13)
#define MCMD14_BUSTEST_R               (14)
#define MCMD15_GO_INACTIVE_STATE       (15)
#define MCMD16_SET_BLOCKLEN            (16)
#define MCMD17_READ_SINGLE_BLOCK       (17)
#define MCMD18_READ_MULTIPLE_BLOCK     (18)
#define MCMD19_BUSTEST_W               (19)
#define MCMD21_SEND_TUNING_BLOCK       (21)
#define MCMD23_SET_BLOCK_COUNT         (23)
#define MCMD24_WRITE_BLOCK             (24)
#define MCMD25_WRITE_MULTIPLE_BLOCK    (25)
#define MCMD26_PROGRAM_CID             (26)
#define MCMD27_PROGRAM_CSD             (27)
#define MCMD28_SET_WRITE_PROT          (28)
#define MCMD29_CLR_WRITE_PROT          (29)
#define MCMD30_SEND_WRITE_PROT         (30)
#define MCMD31_SEND_WRITE_PROT_TYPE    (31)
#define MCMD35_ERASE_GROUP_START       (35)
#define MCMD36_ERASE_GROUP_END         (36)
#define MCMD38_ERASE                   (38)
#define MCMD39_FAST_IO                 (39)
#define MCMD40_GO_IRQ_STATE            (40)
#define MCMD42_LOCK_UNLOCK             (42)
#define MCMD49_SET_TIME                (49)
#define MCMD53_PROTOCOL_RD             (53)
#define MCMD54_PROTOCOL_WR             (54)
#define MCMD55_APP_CMD                 (55)
#define MCMD56_GEN_CMD                 (56)

#define MMCORE_MAX_DMA_BLOCKS          ((2^16)-1)
#define MMCORE_MAX_BLK_SIZE            (0x200)

#define MMCORE_DEF_BLK_SIZE            MMCORE_MAX_BLK_SIZE
#define MMCORE_DEF_BYTE_COUNT          MMCORE_MAX_BLK_SIZE
#define MMCORE_RSP_LEN_6               (6)
#define MMCORE_RSP_LEN_17              (16) // 1 is reserved

#define MMCORE_RSP_SEL                 (0x1)
#define MMCORE_DMA_XFER                (0x1)
#define MMCORE_DATA_WRITE              (0x0)
#define MMCORE_DATA_READ               (0x1)
#define MMCORE_DMA_WRITE               (MMCORE_DATA_WRITE<<1|MMCORE_DMA_XFER)
#define MMCORE_DMA_READ                (MMCORE_DATA_READ<<1|MMCORE_DMA_XFER)
#define MMCORE_DMA_READ_RSP17                (MMCORE_RSP_SEL<<4|MMCORE_DATA_READ<<1|MMCORE_DMA_XFER)

#define MMCORE_MODE_SD20               (0x0)
#define MMCORE_MODE_DDR                (0x1)
#define MMCORE_MODE_SD30               (0x2)

#define MMCORE_HSMODE_NORMAL           (0x0)
#define MMCORE_HSMODE_HS400            (0x1)
#define MMCORE_HSMODE_HS400ES          (0x2)

// For SD_CONFIGURE2
#define MTYPE_BC      (0)
#define MTYPE_BCR     (1)
#define MTYPE_AC      (2)
#define MTYPE_ADTC    (3)

#define MRSP_R0      (0)
#define MRSP_R1      (1)
#define MRSP_R1B     (11)
#define MRSP_R2      (2)
#define MRSP_R3      (3)
#define MRSP_R4      (4)
#define MRSP_R5      (5)

#define MCLASS(x)    (x)

// CMD CODE
#define MMCORE_NORMALWRITE          (0x0)
#define MMCORE_AUTOWRITE3           (0x1)
#define MMCORE_AUTOWRITE4           (0x2)
#define MMCORE_AUTOREAD3            (0x5)
#define MMCORE_AUTOREAD4            (0x6)
#define MMCORE_SENDCMDGETRSP        (0x8)
#define MMCORE_AUTOWRITE1           (0x9)
#define MMCORE_AUTOWRITE2           (0xA)
#define MMCORE_NORMALREAD           (0xC)
#define MMCORE_AUTOREAD1            (0xD)
#define MMCORE_AUTOREAD2            (0xE)

#endif // __MMC_SD_H__
