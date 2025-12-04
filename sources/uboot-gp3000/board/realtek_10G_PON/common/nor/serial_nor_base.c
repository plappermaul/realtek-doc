#ifndef __UBOOT__
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <soc/cortina/rtk_serial_nor.h>
#else
#include <common.h>
#include <asm/io.h>
#include "serial_nor.h"
#endif

#if !defined(SFLASH_USE_SIO) && !defined(SFLASH_USE_DIO) && !defined(SFLASH_USE_QIO)
#error "Error: Missing compile flag for access IO"
#endif

#if defined(SFLASH_USE_SIO) || defined(SFLASH_USE_DIO)
static int norsf_xread_dummy(void){return 0;}
#else
static int norsf_mxic_25L12833F_25645G_51245G_xread_en(void) ;
static int norsf_mxic_25L12833F_25645G_51245G_xread_ex(void) ;
static int norsf_winbond_25qxxxjv_xread_en(void);
static int norsf_winbond_25qxxxjv_xread_ex(void);
#endif


/* Manufacturer ID definition */
#define CFI_MFR_MACRONIX        0x00C2
#define CFI_MFR_WINBOND         0x00EF

#define JEDEC_MFR(_jedec_id)    ((_jedec_id) >> 16)


/* NOTE: double check command sets and memory organization when you add
 * more flash chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 */
struct spi_device_id m25p_ids[] = {
    /* Macronix */
    { "MX25L12833F", INFO(0xc22018, 0, 64 * 1024, 256,  NEED_QE) },
    { "MX25L25645G", INFO(0xc22019, 0, 64 * 1024, 512,  NEED_QE) },
    { "MX25L51245G", INFO(0xc2201a, 0, 64 * 1024, 1024, NEED_QE) },

    /* Winbond  */
    { "W25Q128JV",   INFO(0xef4018, 0, 64 * 1024, 256,  NEED_QE) },
    { "W25Q256JV",   INFO(0xef4019, 0, 64 * 1024, 512,  NEED_QE) },
    { "W25Q512JV",   INFO(0xef4020, 0, 64 * 1024, 1024, NEED_QE) },


    /* Catalyst / On Semiconductor -- non-JEDEC */
    { "Unknown ID",  INFO(0x000000, 0, 64 * 1024, 256,  0) },
};

norsf_read_cmd_t mxic_read_cmds_3b = {
#if defined(SFLASH_USE_QIO)
    .cmd        = 0xeb,
    .dummy_ck   = 6,
    .addr_byte  = 3,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_qio,
    .data_io    = sf_qio,
    .xread_en   = norsf_mxic_25L12833F_25645G_51245G_xread_en,
    .xread_ex   = norsf_mxic_25L12833F_25645G_51245G_xread_ex,
#elif defined(SFLASH_USE_DIO)
    .cmd        = 0xbb,
    .dummy_ck   = 4,
    .addr_byte  = 3,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_dio,
    .data_io    = sf_dio,
    .xread_en   = norsf_xread_dummy,
    .xread_ex   = norsf_xread_dummy,
#else
    .cmd        = 0x0b,
    .dummy_ck   = 8,
    .addr_byte  = 3,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_sio,
    .data_io    = sf_sio,
    .xread_en   = norsf_xread_dummy,
    .xread_ex   = norsf_xread_dummy,
#endif
};

norsf_read_cmd_t mxic_read_cmds_4b = {
#if defined(SFLASH_USE_QIO)
    .cmd        = 0xec,
    .dummy_ck   = 6,
    .addr_byte  = 4,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_qio,
    .data_io    = sf_qio,
    .xread_en   = norsf_mxic_25L12833F_25645G_51245G_xread_en,
    .xread_ex   = norsf_mxic_25L12833F_25645G_51245G_xread_ex,
    /*
    {
        .cmd        = 0xec,
        .dummy_ck   = 4,
        .addr_byte  = 5,
        .predictor  = 0x800000A5,
        .cmd_io     = sf_sio,
        .addr_io    = sf_qio,
        .data_io    = sf_qio,
        .xread_en   = norsf_mxic_25l512x_xread_en,
        .xread_ex   = norsf_mxic_25l512x_xread_ex,
    },
    */
#elif defined(SFLASH_USE_DIO)
    .cmd        = 0xbc,
    .dummy_ck   = 4,
    .addr_byte  = 3,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_dio,
    .data_io    = sf_dio,
    .xread_en   = norsf_xread_dummy,
    .xread_ex   = norsf_xread_dummy,
#else
    .cmd        = 0x0C,
    .dummy_ck   = 8,
    .addr_byte  = 4,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_sio,
    .data_io    = sf_sio,
    .xread_en   = norsf_xread_dummy,
    .xread_ex   = norsf_xread_dummy,
#endif
};

norsf_write_cmd_t mxic_write_cmds_3b = {
#if defined(SFLASH_USE_QIO)
    .cmd       = 0x38,
    .addr_byte = 3,
    .cmd_io    = sf_sio,
    .addr_io   = sf_qio,
    .data_io   = sf_qio,
    .cprog_to_us = 3000,
    .cprog_lim_b = 64,
#else
    .cmd       = 0x02,
    .addr_byte = 3,
    .cmd_io    = sf_sio,
    .addr_io   = sf_sio,
    .data_io   = sf_sio,
    .cprog_to_us = 3000,
    .cprog_lim_b = 64,
#endif
};

norsf_write_cmd_t mxic_write_cmds_4b = {
#if defined(SFLASH_USE_QIO)
	.cmd       = 0x3e,
	.addr_byte = 4,
	.cmd_io    = sf_sio,
	.addr_io   = sf_qio,
	.data_io   = sf_qio,
	.cprog_to_us = 3000,
    .cprog_lim_b = 64,
#else
    .cmd       = 0x12,
    .addr_byte = 4,
    .cmd_io    = sf_sio,
    .addr_io   = sf_sio,
    .data_io   = sf_sio,
    .cprog_to_us = 3000,
    .cprog_lim_b = 64,
#endif
};


norsf_erase_cmd_t mxic_erase_cmds_3b[] = {
    {
        .cmd        = 0xD8,
        .sz_b       = 64*1024,
        .offset_lmt = 0,
        .to_us      = 2*1000*1000,
    },
    {
        .cmd        = 0x52,
        .sz_b       = 32*1024,
        .offset_lmt = 0,
        .to_us      = 1*1000*1000,
    },
    {
        .cmd        = 0x20,
        .sz_b       = 4*1024,
        .offset_lmt = 0,
        .to_us      = 400*1000,
    },
};

norsf_erase_cmd_t mxic_erase_cmds_4b[] = {
    {
        .cmd        = 0xDC,
        .sz_b       = 64*1024,
        .offset_lmt = 0,
        .to_us      = 2*1000*1000
    },
    {
        .cmd        = 0x5C,
        .sz_b       = 32*1024,
        .offset_lmt = 0,
        .to_us      = 1*1000*1000
    },
    {
        .cmd        = 0x21,
        .sz_b       = 4*1024,
        .offset_lmt = 0,
        .to_us      = 400*1000
    },
};


norsf_read_cmd_t winbond_read_cmds_3b = {
#if defined(SFLASH_USE_QIO)
    .cmd        = 0xeb,
    .dummy_ck   = 6,
    .addr_byte  = 3,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_qio,
    .data_io    = sf_qio,
    .xread_en   = norsf_winbond_25qxxxjv_xread_en,
    .xread_ex   = norsf_winbond_25qxxxjv_xread_ex,
#elif defined(SFLASH_USE_DIO)
    .cmd        = 0xbb,
    .dummy_ck   = 4,
    .addr_byte  = 3,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_dio,
    .data_io    = sf_dio,
    .xread_en   = norsf_xread_dummy,
    .xread_ex   = norsf_xread_dummy,
#else
    .cmd        = 0x0B,
    .dummy_ck   = 8,
    .addr_byte  = 3,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_sio,
    .data_io    = sf_sio,
    .xread_en   = norsf_xread_dummy,
    .xread_ex   = norsf_xread_dummy,
#endif

};

norsf_read_cmd_t winbond_read_cmds_4b = {
#if defined(SFLASH_USE_QIO)
    .cmd        = 0xec,
    .dummy_ck   = 6,
    .addr_byte  = 4,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_qio,
    .data_io    = sf_qio,
    .xread_en   = norsf_winbond_25qxxxjv_xread_en,
    .xread_ex   = norsf_winbond_25qxxxjv_xread_ex,
#elif defined(SFLASH_USE_DIO)
    .cmd        = 0xbc,
    .dummy_ck   = 4,
    .addr_byte  = 3,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_dio,
    .data_io    = sf_dio,
    .xread_en   = norsf_xread_dummy,
    .xread_ex   = norsf_xread_dummy,
#else
    .cmd        = 0x0C,
    .dummy_ck   = 8,
    .addr_byte  = 4,
    .predictor  = 0,
    .cmd_io     = sf_sio,
    .addr_io    = sf_sio,
    .data_io    = sf_sio,
    .xread_en   = norsf_xread_dummy,
    .xread_ex   = norsf_xread_dummy,
#endif
};

norsf_write_cmd_t winbond_write_cmds_3b = {
#if defined(SFLASH_USE_QIO)
    .cmd       = 0x32,
    .addr_byte = 3,
    .cmd_io    = sf_sio,
    .addr_io   = sf_sio,
    .data_io   = sf_qio,
    .cprog_to_us = 3000,
    .cprog_lim_b = 64,
#else
    .cmd       = 0x02,
    .addr_byte = 3,
    .cmd_io    = sf_sio,
    .addr_io   = sf_sio,
    .data_io   = sf_sio,
    .cprog_to_us = 3000,
    .cprog_lim_b = 64,
#endif
};

norsf_write_cmd_t winbond_write_cmds_4b = {
#if defined(SFLASH_USE_QIO)
	.cmd       = 0x34,
	.addr_byte = 4,
	.cmd_io    = sf_sio,
	.addr_io   = sf_sio,
	.data_io   = sf_qio,
	.cprog_to_us = 3000,
    .cprog_lim_b = 64,
#else
    .cmd       = 0x12,
    .addr_byte = 4,
    .cmd_io    = sf_sio,
    .addr_io   = sf_sio,
    .data_io   = sf_sio,
    .cprog_to_us = 3000,
    .cprog_lim_b = 64,
#endif
};

norsf_erase_cmd_t winbond_erase_cmds_3b[] = {
    {
        .cmd        = 0xD8,
        .sz_b       = 64*1024,
        .offset_lmt = 0,
        .to_us      = 2000*1000,
    },
    {
        .cmd        = 0x52,
        .sz_b       = 32*1024,
        .offset_lmt = 0,
        .to_us      = 1600*1000,
    },
    {
        .cmd        = 0x20,
        .sz_b       = 4*1024,
        .offset_lmt = 0,
        .to_us      = 400*1000,
    },
};

norsf_erase_cmd_t winbond_erase_cmds_4b[] = {
    {
        .cmd        = 0xDC,
        .sz_b       = 64*1024,
        .offset_lmt = 0,
        .to_us      = 2000*1000
    },
    {
        .cmd        = 0x21,
        .sz_b       = 4*1024,
        .offset_lmt = 0,
        .to_us      = 400*1000
    },
};

static int norsf_gen_wait_wel(void)
{
#ifndef __UBOOT__
#define MIN_TRY_COUNT 1000
    unsigned int count;
	unsigned long status;
	unsigned long timeout_jiffies;

	timeout_jiffies = jiffies + msecs_to_jiffies(SFLASH_WEL_TIMEOUT);

	do {
		status = serial_nor_read_status_reg(SPINOR_OP_READ_STATUS);
		if ((status & SFLASH_STATUS_WEL) == SFLASH_STATUS_WEL) {
			return 0;
		}

        count++;
    } while (time_before(jiffies, timeout_jiffies) || count < MIN_TRY_COUNT);

	return -1;
#else
	unsigned long status;
	unsigned long timebase;

	timebase = get_timer(0);
	do {
		status = serial_nor_read_status_reg(SPINOR_OP_READ_STATUS);
		if ((status & SFLASH_STATUS_WEL) == SFLASH_STATUS_WEL) {
			return 0;
		}

	} while (get_timer(timebase) < SFLASH_WEL_TIMEOUT);

	return -1;
#endif
}

static int norsf_gen_wait_wip(uint32_t wait_msec)
{
#ifndef __UBOOT__
#define MIN_TRY_COUNT 1000
    unsigned int count;
	unsigned long status;
	unsigned long timeout_jiffies;

	timeout_jiffies = jiffies + msecs_to_jiffies(wait_msec);
	do {
		status = serial_nor_read_status_reg(SPINOR_OP_READ_STATUS);
		if ((status & SFLASH_STATUS_WIP) == 0) {
			return 0;
		}

        count++;
    } while (time_before(jiffies, timeout_jiffies) || count < MIN_TRY_COUNT);

	return -1;
#else
	unsigned long status;
	unsigned long timebase;

	timebase = get_timer(0);
	do {
		status = serial_nor_read_status_reg(SPINOR_OP_READ_STATUS);
		if ((status & SFLASH_STATUS_WIP) == 0) {
			return 0;
		}

	} while (get_timer(timebase) < wait_msec);

	return -1;
#endif
}

#ifdef SFLASH_USE_QIO
static int norsf_mxic_25L12833F_25645G_51245G_xread_en(void)
{
	uint8_t status, configuration;
	status = serial_nor_read_status_reg(MXIC_OP_READ_STATUS);
	configuration = serial_nor_read_status_reg(MXIC_OP_READ_CONFIGURATION);

	/* enable quad-enable bit. */
	status |= 0x40;
	serial_nor_write_status_reg(MXIC_OP_WRITE_STATUS, (configuration<<8)|status, 2);

	return 0;
}

static int norsf_mxic_25L12833F_25645G_51245G_xread_ex(void)
{
	uint8_t status, configuration;

	status = serial_nor_read_status_reg(MXIC_OP_READ_STATUS);
	configuration = serial_nor_read_status_reg(MXIC_OP_READ_CONFIGURATION);

	/* disable quad-enable bit. */
	status &= (~0x40);
	serial_nor_write_status_reg(MXIC_OP_WRITE_STATUS, (configuration<<8)|status, 2);
	return 0;
}

static int norsf_winbond_25qxxxjv_xread_en(void)
{
    uint8_t status;
    status = serial_nor_read_status_reg(WINBOND_OP_READ_STATUS2);

    /* enable quad-enable bit. */
    status |= (1<<1);
    serial_nor_write_status_reg(WINBOND_OP_WRITE_STATUS2, status, 1);
    return 0;
}

static int norsf_winbond_25qxxxjv_xread_ex(void)
{
    uint8_t status;
    status = serial_nor_read_status_reg(WINBOND_OP_READ_STATUS2);

    /* enable quad-enable bit. */
    status &= ~(1<<1);
    serial_nor_write_status_reg(WINBOND_OP_WRITE_STATUS2, status, 1);
    return 0;
}
#endif

void serial_nor_configure_cmd_info(norsf_cmd_info_t *cmd, struct flash_info *info)
{
    uint32_t chip_size = info->sector_size * info->n_sectors;
    cmd->wait_wel = norsf_gen_wait_wel;
    cmd->wait_wip = norsf_gen_wait_wip;


    switch (info->jedec_id) {
        case 0xC2201A:
        case 0xC22019:
            cmd->cerase= mxic_erase_cmds_4b;
            cmd->cprog = &mxic_write_cmds_4b;
            cmd->cread = &mxic_read_cmds_4b;
            break;
        case 0xC22018:
            cmd->cerase= mxic_erase_cmds_3b;
            cmd->cprog = &mxic_write_cmds_3b;
            cmd->cread = &mxic_read_cmds_3b;
            break;
        case 0xEF4019:
        case 0xEF4020:
            cmd->cerase= winbond_erase_cmds_4b;
            cmd->cprog = &winbond_write_cmds_4b;
            cmd->cread = &winbond_read_cmds_4b;
            break;
        case 0xEF4018:
            cmd->cerase= winbond_erase_cmds_3b;
            cmd->cprog = &winbond_write_cmds_3b;
            cmd->cread = &winbond_read_cmds_3b;
            break;
        default:
            if (chip_size > 0x1000000) {
                cmd->cerase= mxic_erase_cmds_4b;
                cmd->cprog = &mxic_write_cmds_4b;
                cmd->cread = &mxic_read_cmds_4b;
            } else {
                cmd->cerase= mxic_erase_cmds_3b;
                cmd->cprog = &mxic_write_cmds_3b;
                cmd->cread = &mxic_read_cmds_3b;
            }
            break;
    }

    sfprint("0x%02x(1%d%d) / ",cmd->cread->cmd,(1<<cmd->cread->addr_io), (1<<cmd->cread->data_io));
    sfprint("0x%02x(1%d%d)\n",cmd->cprog->cmd,(1<<cmd->cprog->addr_io), (1<<cmd->cprog->data_io));
}


struct spi_device_id *serial_nor_scan_id_table(uint32_t rdid)
{
    uint32_t tmp;
    struct flash_info *info;

	sfprint("\nSPI NOR: ID is 0x%x", rdid);

    for (tmp = 0; tmp < ARRAY_SIZE(m25p_ids); tmp++) {
        info = (void *)m25p_ids[tmp].driver_data;
        if (info->jedec_id == rdid) {
            sfprint("\rSPI NOR: %s (0x%X)\n",  m25p_ids[tmp].name, rdid);
            return &m25p_ids[tmp];
        }
    }

    sfprint("\rSPI NOR: Unrecognized JEDEC id 0x%06X, configurated as SIO/16MB\n", rdid);
    return &m25p_ids[sizeof(m25p_ids)/sizeof(struct spi_device_id) - 1];
}

