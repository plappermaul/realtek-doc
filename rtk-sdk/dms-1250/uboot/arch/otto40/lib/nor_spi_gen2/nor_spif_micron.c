#ifdef __LUNA_KERNEL__
#include <linux/module.h>
#include "nor_spif_core.h"
#include "register_map.h"
#define puts(...) printk(__VA_ARGS__)
#else
#include <nor_spi/nor_spif_core.h>
#include <register_map.h>
#endif

#define NORSF_CMN_SZ_MB      (16)
#define NORSF_CMN_ERASE_SZ_B (4096)

#define MICRON_MT25QL512AB_ID (0x20BA20)

// MICRON Commands Table
#define MICRON_RDSR       (0x05)
#define MICRON_WRSR       (0x01)
#define MICRON_RDID       (0x9F)




#define MICRON_USE_DIO_MMIO
//#define MICRON_USE_QIO_MMIO


void __attribute__ ((weak)) enable_4BYTE_strappin(void) {};



SECTION_NOR_SPIF_GEN2_CORE s32_t micron_unit_erase_g2(const u32_t, const u32_t);
SECTION_NOR_SPIF_GEN2_CORE s32_t micron_unit_write_g2(const u32_t, u32_t, u32_t, const void *);
SECTION_NOR_SPIF_GEN2_MISC s32_t micron_read_g2(const u32_t, u32_t, const u32_t, void *);

SECTION_NOR_SPIF_GEN2_CORE s32_t micron_4B_unit_erase_g2(const u32_t, const u32_t);
SECTION_NOR_SPIF_GEN2_CORE s32_t micron_4B_unit_write_g2(const u32_t, u32_t, u32_t, const void *);
SECTION_NOR_SPIF_GEN2_MISC s32_t micron_4B_sio_read_g2(const u32_t, u32_t, const u32_t, void *);
SECTION_NOR_SPIF_GEN2_MISC s32_t micron_4B_dio_read_g2(const u32_t, u32_t, const u32_t, void *);
SECTION_NOR_SPIF_GEN2_MISC s32_t micron_4B_qio_read_g2(const u32_t, u32_t, const u32_t, void *);


plr_nor_spi_info_g2_t micron_MT25QL512AB_4B_info = {
	.id             = MICRON_MT25QL512AB_ID,
	.num_chips      = 1,
	.wr_boundary_b  = 256,
	.erase_unit_b   = 4096,
	.size_per_chip_b= 64 * 1024 * 1024,
	.addr_len_b     = 4,
	._nor_spi_erase = micron_4B_unit_erase_g2,
#ifdef MICRON_USE_QIO_MMIO
	._nor_spi_read  = micron_4B_qio_read_g2,
#elif defined MICRON_USE_DIO_MMIO
	._nor_spi_read  = micron_4B_dio_read_g2,
#else
    ._nor_spi_read  = micron_4B_sio_read_g2,
#endif
	._nor_spi_write = micron_4B_unit_write_g2,
};

plr_nor_spi_info_g2_t micron_MT25QL512AB_3B_info = {
	.id             = MICRON_MT25QL512AB_ID,
	.num_chips      = 1,
	.wr_boundary_b  = 256,
	.erase_unit_b   = 4096,
	.size_per_chip_b= 16 * 1024 * 1024,
	.addr_len_b     = 3,
	._nor_spi_erase = micron_unit_erase_g2,
	._nor_spi_read  = micron_read_g2,
	._nor_spi_write = micron_unit_write_g2,
};

/* !!!!This function be allocated on SRAM!!!! */
SECTION_NOR_SPIF_GEN2_CORE
static void spif_wip_block(u32_t cs) {
	nsf_write_phase_t wp = {
		.cmd  = MICRON_RDSR,
		.addr = 0,
	};
	nsf_trx_attr_t attr = {
		.wmode  = norsf_sio,
		.wlen_b = 1,
		.dummy_ck = 0,
		.pdir   = norsf_prx,
		.pmode  = norsf_sio,
		.plen_b = 1,
	};
	u8_t payload = 0x3;

	while ((payload & 0x3) != 0) {
		spif_trx(cs, &wp, &payload, attr, VZERO);
	}

	return;
}

SECTION_NOR_SPIF_GEN2_CORE
s32_t micron_unit_erase_g2(const u32_t cid, const u32_t offset) {
	const nsf_write_phase_t wp = {
		.cmd  = 0x20,
		.addr = offset & (~(NORSF_CMN_ERASE_SZ_B - 1)),
	};
	nsf_trx_attr_t attr = {
		.plen_b = 0,
		.write_en = 1,
		.wlen_b = 1 + micron_MT25QL512AB_3B_info.addr_len_b,
		.dummy_ck = 0,
		.pdir  = norsf_ptx,
		.wmode = norsf_sio,
		.pmode = norsf_sio,
	};

	spif_trx(cid, &wp, VZERO, attr, spif_wip_block);

	return 0;
}


SECTION_NOR_SPIF_GEN2_CORE
s32_t micron_4B_unit_erase_g2(const u32_t cid, const u32_t offset) {
	const nsf_write_phase_t wp = {
		.cmd  = 0x21,
		.addr = offset & (~(NORSF_CMN_ERASE_SZ_B - 1)),
	};
	nsf_trx_attr_t attr = {
		.plen_b = 0,
		.write_en = 1,
		.wlen_b = 1 + micron_MT25QL512AB_4B_info.addr_len_b,
		.dummy_ck = 0,
		.pdir  = norsf_ptx,
		.wmode = norsf_sio,
		.pmode = norsf_sio,
	};

	spif_trx(cid, &wp, VZERO, attr, spif_wip_block);

	return 0;
}

SECTION_NOR_SPIF_GEN2_CORE
s32_t micron_unit_write_g2(const u32_t cid, u32_t offset, u32_t leng, const void *buf) {
	nsf_write_phase_t wp = {
		.cmd  = 0x02,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 1,
		.wlen_b = 1 + micron_MT25QL512AB_3B_info.addr_len_b,
		.dummy_ck = 0,
		.pdir  = norsf_ptx,
		.wmode = norsf_sio,
		.pmode = norsf_sio,
	};

	spif_trx(cid, &wp, (void *)buf, attr, spif_wip_block);

	return 0;
}

SECTION_NOR_SPIF_GEN2_CORE
s32_t micron_4B_unit_write_g2(const u32_t cid, u32_t offset, u32_t leng, const void *buf) {
	nsf_write_phase_t wp = {
		.cmd  = 0x12,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 1,
		.wlen_b = 1 + micron_MT25QL512AB_4B_info.addr_len_b,
		.dummy_ck = 0,
		.pdir  = norsf_ptx,
		.wmode = norsf_sio,
		.pmode = norsf_sio,
	};

	spif_trx(cid, &wp, (void *)buf, attr, spif_wip_block);

	return 0;
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t micron_4B_qio_read_g2(const u32_t cid, u32_t offset, const u32_t leng, void *buf) {
	nsf_write_phase_t wp = {
		.cmd  = 0xEC,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 0,
		.wlen_b = 1 + micron_MT25QL512AB_4B_info.addr_len_b,
		.dummy_ck = 8,
		.pdir  = norsf_prx,
		.wmode = norsf_qio,
		.pmode = norsf_qio,
	};
	spif_trx(cid, &wp, buf, attr, VZERO);

	return 0;
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t micron_4B_dio_read_g2(const u32_t cid, u32_t offset, const u32_t leng, void *buf) {
	nsf_write_phase_t wp = {
		.cmd  = 0xBc,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 0,
		.wlen_b = 1 + micron_MT25QL512AB_4B_info.addr_len_b,
		.dummy_ck = 8,
		.pdir  = norsf_prx,
		.wmode = norsf_dio,
		.pmode = norsf_dio,
	};
	spif_trx(cid, &wp, buf, attr, VZERO);

	return 0;
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t micron_4B_sio_read_g2(const u32_t cid, u32_t offset, const u32_t leng, void *buf) {
	nsf_write_phase_t wp = {
		.cmd  = 0x13,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 0,
		.wlen_b = 1 + micron_MT25QL512AB_4B_info.addr_len_b,
		.dummy_ck = 0,
		.pdir  = norsf_prx,
		.wmode = norsf_sio,
		.pmode = norsf_sio,
	};
	spif_trx(cid, &wp, buf, attr, VZERO);

	return 0;
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t micron_read_g2(const u32_t cid, u32_t offset, const u32_t leng, void *buf) {
	nsf_write_phase_t wp = {
		.cmd  = 0x0B,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 0,
		.wlen_b = 1 + micron_MT25QL512AB_3B_info.addr_len_b,
		.dummy_ck = 0,
		.pdir  = norsf_prx,
		.wmode = norsf_sio,
		.pmode = norsf_sio,
	};
	spif_trx(cid, &wp, buf, attr, VZERO);

	return 0;
}

SECTION_NOR_SPIF_GEN2_CORE 
u32_t mircon_read_id(void)
{
    u32_t cs = 0;
    u8_t buf[4];
    u32_t *id_val = (u32_t *)buf;
    nsf_write_phase_t wp_rdid = {
        .cmd  = 0x9F,
        .addr = 0,
    };
    nsf_trx_attr_t attr_rdid = {
        .plen_b = 3,
        .write_en = 0,
        .wlen_b = 1,
        .dummy_ck = 0,
        .pdir  = norsf_prx,
        .wmode = norsf_sio,
        .pmode = norsf_sio,
    };

    spif_trx(cs, &wp_rdid, buf, attr_rdid, VZERO);
    return ((*id_val)>>8)&0x00FFFFFF;
}

SECTION_NOR_SPIF_GEN2_CORE
plr_nor_spi_info_g2_t * nor_spif_micron_probe(void) {
    u32_t sz_mb;
    u32_t sz_factor = 0;  

    if(MICRON_MT25QL512AB_ID == mircon_read_id()){
        printf("MT25QL512AB, 64MB, 0x%x, ",MICRON_MT25QL512AB_ID);

        enable_4BYTE_strappin();
        if(RFLD_SFCR2(mmio_4b_addr_en) == 1){
            #ifdef MICRON_USE_QIO_MMIO
            RMOD_SFCR2(sfcmd, 0xEC);
            RMOD_SFCR2(sfsize, 7);
            RMOD_SFCR2(cmd_io, 0);
            RMOD_SFCR2(addr_io, 2);
            RMOD_SFCR2(dummy_cycles, 5);
            RMOD_SFCR2(data_io, 2);
            puts("QIO MMIO supported, ");
            #elif defined (MICRON_USE_DIO_MMIO)
            RMOD_SFCR2(sfcmd, 0xBC);
            RMOD_SFCR2(sfsize, 7);
            RMOD_SFCR2(cmd_io, 0);
            RMOD_SFCR2(addr_io, 1);
            RMOD_SFCR2(dummy_cycles, 4);
            RMOD_SFCR2(data_io, 1);
            puts("DIO MMIO supported, ");
            #else
            RMOD_SFCR2(sfcmd, 0x13);
            RMOD_SFCR2(sfsize, 7);
            RMOD_SFCR2(cmd_io, 0);
            RMOD_SFCR2(addr_io, 0);
            RMOD_SFCR2(dummy_cycles, 0);
            RMOD_SFCR2(data_io, 0);
            puts("SIO MMIO supported, ");
            #endif

            sz_mb = micron_MT25QL512AB_4B_info.size_per_chip_b / 1024 / 1024;
            while (sz_mb) {
                sz_factor++;
                sz_mb >>= 1;
            }
            puts("4BYTE supported... ");
            return &micron_MT25QL512AB_4B_info;
        }else{		
			sz_mb = micron_MT25QL512AB_3B_info.size_per_chip_b / 1024 / 1024;
            while (sz_mb) {
                sz_factor++;
                sz_mb >>= 1;
            }
            puts("Only 3BYTE supported... ");
            return &micron_MT25QL512AB_3B_info;
        }
    }
	return VZERO;
}
REG_NOR_SPIF_PROBE_FUNC(nor_spif_micron_probe);
