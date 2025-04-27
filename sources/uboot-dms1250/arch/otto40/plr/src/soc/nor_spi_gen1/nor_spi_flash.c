/** Realtek SD6 SoC Team SPI NOR driver
  */
#define SECTION_NOR_SPIF_DRIVER __attribute__ ((section (".sram_text")))

#include <soc.h>
#include <cg.h>

s32_t flash_unit_erase(const u32_t, u32_t);
s32_t flash_unit_write(const u32_t, u32_t, const u32_t, const void*);
s32_t flash_read(const u32_t, u32_t, const u32_t, void*);

typedef u32_t sffunc_ptr(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf);
static sffunc_ptr sffunc_wren_wrdi_eqcmd, sffunc_rdid, sffunc_sfdp_cmd, sffunc_wrsr, sffunc_rdsr_rdsr2;
static sffunc_ptr sffunc_rdevcr, sffunc_wrevcr;
static sffunc_ptr sffunc_erase, sffunc_write, sffunc_read;

static sffunc_ptr *sffunc_rdbusy;
static sffunc_ptr sffunc_rdbusy_dummy, sffunc_rdbusy_cmd;

#ifndef NULL
    #define NULL       VZERO
#endif
#define pblr_puts puts

#define para_flash_info (*(((soc_t *)(0x9f000020))->flash_info.nor_spi_info))

#define SPIF_BASE 0xb8001200
#define SFCR  (*((volatile u32_t *)(SPIF_BASE + 0x0 )))
    #define SFCR_EnableRBO (1<<28)
    #define SFCR_EnableWBO (1<<27)
    #define SFCR_SPI_TCS   (0x1F<<22)
#define SFCR2 (*((volatile u32_t *)(SPIF_BASE + 0x4 )))
    #define SFCR2_SFCMD(v)        ((v)<<24)
    #define SFCR2_SIZE(v)         ((v)<<21)
//  #define SFCR2_RDOPT           (1<<20)
    #define SFCR2_CMDIO(v)        ((v)<<18)
    #define SFCR2_ADDRIO(v)       ((v)<<16)
    #define SFCR2_DUMMYCYCLE(v)   ((v)<<13)
    #define SFCR2_DATAIO(v)       ((v)<<11)
//  #define SFCR2_HOLD_TILL_SFDR2 (1<<10)
#define SFCSR (*((volatile u32_t *)(SPIF_BASE + 0x8 )))
    #define SFCSR_SPI_CSB0 (1<<31)
    #define SFCSR_SPI_CSB1 (1<<30)
    #define SFCSR_LEN(val) ((val-1)<<28)
    #define SFCSR_SPI_RDY  (1<<27)
    #define SFCSR_IOW(val) (val << 25)
//  #define SFCSR_CHIP_SEL (1<<24)
    #define SFCSR_SPI_CSB2 (1<<15)
    #define SFCSR_SPI_CSB3 (1<<14)
#define SFDR  (*((volatile u32_t *)(SPIF_BASE + 0xC )))
//#define SFDR2 (*((volatile u32_t *)(SPIF_BASE + 0x10)))

#define SF_DEACT_ALL (SFCSR_SPI_CSB0 | SFCSR_SPI_CSB1 | SFCSR_SPI_CSB2 | SFCSR_SPI_CSB3)
#define SF_CS_INIT   ((SF_DEACT_ALL) | SFCSR_LEN(1) | SFCSR_SPI_RDY)

/* 1. Make sure the signal is synchronized.
   2. Experiment showed that CHECK_CTRL_READY() function call
      conducts smaller code size than macro. */
SECTION_NOR_SPIF_DRIVER
static inline void CHECK_CTRL_READY(void) {
    u32_t i = 0, err = 0;
    for (i=0; i<100; i++) {
        while ((SFCSR & SFCSR_SPI_RDY) == 0) {
            i = 0;
            if ((err++) > 1000000) {
                pblr_puts("EE: NOR SPI flash controller does not respond.\n");
                while (1);
            }
        };
    }
    return;
}

#define CHECK_WRITE_READY(cid) do { \
        while (SPI_CMD_SIMPLE(cid, SFCMD_PARA_RDBUSY) & (0x1 << para_flash_info.rdbusy_loc)) { \
            udelay(1 << para_flash_info.rdbusy_polling_period); \
        } \
    } while(0)

#define SPI_CMD_SIMPLE(cid, cmd) spi_cmd(cid, cmd, 0, 0, NULL)

#define SFCMD_WRDI_OP (0x04)
#define SFCMD_WREN_OP (0x06)
#define SFCMD_RDID_OP (0x9F)
#define SFCMD_SFDP_CMD_OP (0x5A)
#define SFCMD_EX4B_OP (0xE9)
#define SFCMD_RDEVCR_OP (0x65)
#define SFCMD_WREVCR_OP (0x61)
#define SFCMD_FAST_READ_4BCMD_OP (0x0C)
#define SFCMD_PARA_READ_OP   (para_flash_info.prefer_rd_cmd)
#define SFCMD_PARA_WRITE_OP  (para_flash_info.wr_cmd)
#define SFCMD_PARA_ERASE_OP  (para_flash_info.erase_cmd)
#define SFCMD_PARA_RDSR_OP   (para_flash_info.pm_rdsr_cmd)
#define SFCMD_PARA_RDSR2_OP  (para_flash_info.pm_rdsr2_cmd)
#define SFCMD_PARA_WRSR_OP   (para_flash_info.pm_wrsr_cmd)
#define SFCMD_PARA_EQCMD_OP  (para_flash_info.pm_enable_cmd)
#define SFCMD_PARA_RDBUSY_OP (para_flash_info.rdbusy_cmd)

#define SFCMD_WRDI_FUNC (sffunc_wren_wrdi_eqcmd)
#define SFCMD_WREN_FUNC (sffunc_wren_wrdi_eqcmd)
#define SFCMD_RDID_FUNC (sffunc_rdid)
#define SFCMD_SFDP_CMD_FUNC (sffunc_sfdp_cmd)
#define SFCMD_RDEVCR_FUNC   (sffunc_rdevcr)
#define SFCMD_WREVCR_FUNC   (sffunc_wrevcr)
#define SFCMD_PARA_READ_FUNC   (sffunc_read)
#define SFCMD_PARA_WRITE_FUNC  (sffunc_write)
#define SFCMD_PARA_ERASE_FUNC  (sffunc_erase)
#define SFCMD_PARA_RDSR_FUNC   (sffunc_rdsr_rdsr2)
#define SFCMD_PARA_RDSR2_FUNC  (sffunc_rdsr_rdsr2)
#define SFCMD_PARA_WRSR_FUNC   (sffunc_wrsr)
#define SFCMD_PARA_EQCMD_FUNC  (sffunc_wren_wrdi_eqcmd)
#define SFCMD_PARA_RDBUSY_FUNC (sffunc_rdbusy)

static u32_t wr_dummy_byte = 0, rd_dummy_byte = 0;
static u8_t sr_len_offset = 0, rdbusy_len_offset = 0;
u32_t wr_boundary = 0, erase_unit = 0;
static u32_t addr_modifier = 0;

#define spi_cmd(cid, cmd, data, wr_leng, wr_buf) \
    _spi_cmd(cid, (cmd##_OP << 24), cmd##_FUNC, data, wr_leng, wr_buf)

SECTION_NOR_SPIF_DRIVER
static u32_t _spi_cmd(const u8_t cid, const u32_t opcode, sffunc_ptr sffunc,
                      const u32_t data, u32_t wr_leng, void *wr_buf) {
    const u32_t sfcsr_act_cs[4] = {0x7FFFFFFF, 0xBFFFFFFF, 0xFFFF7FFF, 0xFFFFBFFF};
    u32_t sfcsr_val, ret = 0;

    CHECK_CTRL_READY();
    SFCSR = SF_CS_INIT; /* deactivate CS0 and CS1 */
    CHECK_CTRL_READY();
    SFCSR = 0;          /* activate CS0 and CS1 */
    udelay(3);
    SFCSR = SF_CS_INIT; /* deactivate CS0 and CS1 */

    sfcsr_val = (SFCSR & sfcsr_act_cs[cid]);

    CHECK_CTRL_READY();

    ret = sffunc(cid, sfcsr_val, opcode, data, wr_leng, wr_buf);

    CHECK_CTRL_READY();
    SFCSR = SF_CS_INIT; /* deactivate CS0 and CS1 */
    CHECK_CTRL_READY();
    SFCSR = 0;          /* activate CS0 and CS1 */
    udelay(3);
    SFCSR = SF_CS_INIT; /* deactivate CS0 and CS1 */
    CHECK_CTRL_READY();

    return ret;
}

#define GENERIC_FLASH_ID (0xFFFFFF)

SECTION_NOR_SPIF_DRIVER
void flash_init(void) {
    /* SFSIZE of 3B-addr mode starts from 2^17;
       SFSIZE of 4B-addr mode starts from 2^19.
       Use sf_size_mod[4 - addr_mode] to get sf_size_mode[]. */
    const u8_t sf_size_mod[] = {19, 17};

    /* Detect address mode. */
    para_flash_info.addr_mode += (SFCR2 >> 9) & 0x1;

    if (para_flash_info.rdbusy_cmd == 0x0) {
        sffunc_rdbusy = sffunc_rdbusy_dummy;
    } else {
        sffunc_rdbusy = sffunc_rdbusy_cmd;
    }

    wr_dummy_byte = para_flash_info.wr_dummy_c        * (1 << para_flash_info.wr_data_io)        / 8;
    rd_dummy_byte = para_flash_info.prefer_rd_dummy_c * (1 << para_flash_info.prefer_rd_data_io) / 8;

    wr_boundary = (para_flash_info.wr_boundary == 0 ? 0xFFFFFFFF : (0x1UL << para_flash_info.wr_boundary));
    erase_unit = 1 << para_flash_info.erase_unit;

    /* offset to shift data when read/write status and read busy bit. */
    sr_len_offset = (32 - (para_flash_info.pm_status_len * 8));
    rdbusy_len_offset = (32 - (para_flash_info.rdbusy_len * 8));

    SFCR = (((para_flash_info.prefer_divisor / 2 - 1) << 29) |
            SFCR_EnableWBO | SFCR_EnableRBO | SFCR_SPI_TCS);

    /* for 4B addr mode, address of read/write/erase requires 32-bit and needs no shift.
       for 3B mode, address needs to shift 8 bits left to make a 24-bit address. */
    addr_modifier = (4 - para_flash_info.addr_mode) * 8;

    /* Set NOR SPI flash CTRL with parameters in flash_info. */
    SFCR2 = (SFCR2_SFCMD(para_flash_info.prefer_rd_cmd)     |
             SFCR2_SIZE((para_flash_info.size_per_chip - sf_size_mod[4-para_flash_info.addr_mode])) |
             SFCR2_ADDRIO(para_flash_info.prefer_rd_addr_io)|
             SFCR2_DATAIO(para_flash_info.prefer_rd_data_io)|
             SFCR2_CMDIO(para_flash_info.prefer_rd_cmd_io)  |
             SFCR2_DUMMYCYCLE((para_flash_info.prefer_rd_dummy_c >> 1)));

    para_flash_info._nor_spi_erase = flash_unit_erase;
    para_flash_info._nor_spi_read  = flash_read;
    para_flash_info._nor_spi_write = flash_unit_write;

    return;
}
REG_INIT_FUNC(flash_init, 20);

SECTION_NOR_SPIF_DRIVER
s32_t flash_unit_erase(const u32_t cid, const u32_t offset) {
    SPI_CMD_SIMPLE(cid, SFCMD_WREN);
    spi_cmd(cid, SFCMD_PARA_ERASE, offset, 0, NULL);
    SPI_CMD_SIMPLE(cid, SFCMD_WRDI);
    return 0;
}

SECTION_NOR_SPIF_DRIVER
s32_t flash_unit_write(const u32_t cid, u32_t offset, u32_t leng, const void *buf) {
    SPI_CMD_SIMPLE(cid, SFCMD_WREN);
    spi_cmd(cid, SFCMD_PARA_WRITE, offset, leng, (void *)buf);
    SPI_CMD_SIMPLE(cid, SFCMD_WRDI);
    return 0;
}

SECTION_NOR_SPIF_DRIVER
s32_t flash_read(const u32_t cid, u32_t offset, const u32_t leng, void *buf) {
    spi_cmd(cid, SFCMD_PARA_READ, offset, leng, buf);
    return 0;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_wren_wrdi_eqcmd(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    SFCSR = sfcsr_val;
    CHECK_CTRL_READY();
    SFDR = opcode;
    return 0;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_rdid(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    SFCSR = sfcsr_val;
    CHECK_CTRL_READY();
    SFDR = opcode;
    CHECK_CTRL_READY();
    SFCSR = sfcsr_val | SFCSR_LEN(3);
    CHECK_CTRL_READY();
    return SFDR;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_sfdp_cmd(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    SFCSR = sfcsr_val | SFCSR_LEN(1);
    CHECK_CTRL_READY();
    SFDR = opcode;
    CHECK_CTRL_READY();

    /* 3-Byte Address + 1-Byte Dummy*/
    SFCSR = sfcsr_val | SFCSR_LEN(4);
    CHECK_CTRL_READY();
    SFDR = data;
    CHECK_CTRL_READY();

    /* Read SFDP Data */
    SFCSR = sfcsr_val | SFCSR_LEN(1);
    CHECK_CTRL_READY();
    return SFDR;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_rdevcr(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    /* command */
    SFCSR = sfcsr_val;
    CHECK_CTRL_READY();
    SFDR = opcode;
    /* data */
    CHECK_CTRL_READY();
    SFCSR = sfcsr_val | SFCSR_LEN(1);
    CHECK_CTRL_READY();
    return SFDR >> 24;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_wrevcr(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    /* command */
    SFCSR = sfcsr_val;
    CHECK_CTRL_READY();
    SFDR = opcode;
    /* data */
    CHECK_CTRL_READY();
    SFCSR = sfcsr_val | SFCSR_LEN(1);
    CHECK_CTRL_READY();
    SFDR = data << 24;
    CHECK_WRITE_READY(cid);
    return 0;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_wrsr(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    /* command */
    SFCSR = sfcsr_val;
    CHECK_CTRL_READY();
    SFDR = opcode;
    /* data */
    CHECK_CTRL_READY();
    SFCSR = sfcsr_val | SFCSR_LEN(para_flash_info.pm_status_len);
    CHECK_CTRL_READY();
    SFDR = data << sr_len_offset;
    CHECK_WRITE_READY(cid);
    return 0;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_rdsr_rdsr2(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    /* command */
    SFCSR = sfcsr_val;
    CHECK_CTRL_READY();
    SFDR = opcode;
    /* data */
    CHECK_CTRL_READY();
    SFCSR = sfcsr_val | SFCSR_LEN(para_flash_info.pm_status_len);
    CHECK_CTRL_READY();
    return SFDR >> sr_len_offset;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_rdbusy_dummy(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    return 0;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_rdbusy_cmd(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    /* command */
    SFCSR = sfcsr_val;
    CHECK_CTRL_READY();
    SFDR = opcode;
    /* data */
    CHECK_CTRL_READY();
    SFCSR = sfcsr_val | SFCSR_LEN(para_flash_info.rdbusy_len);
    CHECK_CTRL_READY();
    return (SFDR >> rdbusy_len_offset);
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_erase(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    /* command */
    SFCSR = sfcsr_val;
    CHECK_CTRL_READY();
    SFDR = opcode;
    /* address */
    CHECK_CTRL_READY();
    SFCSR = sfcsr_val | SFCSR_LEN(para_flash_info.addr_mode);
    CHECK_CTRL_READY();
    SFDR = data << addr_modifier;
    CHECK_WRITE_READY(cid);
    return 0;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_write(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    u32_t i;
    u8_t tmp[4];

    /* command */
    SFCSR = sfcsr_val | SFCSR_IOW(para_flash_info.wr_cmd_io);
    CHECK_CTRL_READY();
    SFDR = opcode;
    /* address */
    CHECK_CTRL_READY();
    SFCSR = sfcsr_val | SFCSR_LEN(para_flash_info.addr_mode) | SFCSR_IOW(para_flash_info.wr_addr_io);
    CHECK_CTRL_READY();
    SFDR = data << addr_modifier;
    /* data */
    CHECK_CTRL_READY();
    sfcsr_val |= SFCSR_IOW(para_flash_info.wr_data_io);

    /* Walk though dummy cycles */
    if (wr_dummy_byte > 0) {
        SFCSR = sfcsr_val;
        for (i=0; i<wr_dummy_byte; i++) {
            CHECK_CTRL_READY();
            SFDR = 0;
        }
    }

    while (wr_leng > 0) {
        CHECK_CTRL_READY();
        if (wr_leng < 4) {
            SFCSR = sfcsr_val | SFCSR_LEN(wr_leng);
            wr_leng = 0;
        } else {
            SFCSR = sfcsr_val | SFCSR_LEN(4);
            wr_leng -= 4;
        }
        /* The last few bytes may be garbage but won't be written
           to flash, since its length is explicitly given */
        tmp[0] = *((u8_t *)wr_buf++);
        tmp[1] = *((u8_t *)wr_buf++);
        tmp[2] = *((u8_t *)wr_buf++);
        tmp[3] = *((u8_t *)wr_buf++);
        CHECK_CTRL_READY();
        SFDR = *((u32_t *)tmp);
    }

    CHECK_WRITE_READY(cid);
    return 0;
}

SECTION_NOR_SPIF_DRIVER
static u32_t sffunc_read(u8_t cid, u32_t sfcsr_val, u32_t opcode, const u32_t data, u32_t wr_leng, void *wr_buf) {
    u32_t wr_bound, i;
    u8_t tmp[4];

    /* command */
    SFCSR = sfcsr_val | SFCSR_IOW(para_flash_info.prefer_rd_cmd_io);
    CHECK_CTRL_READY();
    SFDR = opcode;
    /* address */
    CHECK_CTRL_READY();
    SFCSR = sfcsr_val | SFCSR_LEN(para_flash_info.addr_mode) | SFCSR_IOW(para_flash_info.prefer_rd_addr_io);
    CHECK_CTRL_READY();
    SFDR = data << addr_modifier;
    /* data */
    CHECK_CTRL_READY();
    wr_bound = (u32_t)wr_buf + wr_leng;
    sfcsr_val |= SFCSR_IOW(para_flash_info.prefer_rd_data_io);

    /* Walk though dummy cycles */
    if (rd_dummy_byte > 0) {
        SFCSR = sfcsr_val;
        for (i=0; i<rd_dummy_byte; i++) {
            CHECK_CTRL_READY();
            SFDR = 0;
        }
    }

    if (wr_leng >= 4) {
        CHECK_CTRL_READY();
        SFCSR = sfcsr_val | SFCSR_LEN(4);
        while ((u32_t)wr_buf < (wr_bound & 0xFFFFFFFC)) {
            CHECK_CTRL_READY();
            *((u32_t *)tmp) = SFDR;
            *((u8_t *)wr_buf++) = tmp[0];
            *((u8_t *)wr_buf++) = tmp[1];
            *((u8_t *)wr_buf++) = tmp[2];
            *((u8_t *)wr_buf++) = tmp[3];
        }
    }

    CHECK_CTRL_READY();
    SFCSR = sfcsr_val;
    for (i=0; i<(wr_leng & 0x3); i++) {
        CHECK_CTRL_READY();
        *((u8_t *)wr_buf++) = SFDR >> 24;
    }
    return 0;
}
