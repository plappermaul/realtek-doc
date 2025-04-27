#ifdef __LUNA_KERNEL__
#include <linux/module.h>
#include "nor_spif_core.h"
#include "register_map.h"
#define puts(...) printk(__VA_ARGS__)
#else
#include <nor_spi/nor_spif_core.h>
#include <register_map.h>
#include <util.h>
#endif

#define NORSF_CMN_SZ_MB      (16)
#define NORSF_CMN_ERASE_SZ_B (4096)

// MICRON Commands Table
#define MICRON_RDSR       (0x05)
#define MICRON_WRSR       (0x01)
#define MICRON_RDID       (0x9F)

#define MICRON_READ       (0x03)
#define MICRON_2READ      (0xBB)
#define MICRON_4READ      (0xEB)
#define MICRON_SE         (0x20)  // SECTOR ERASE
#define MICRON_PP         (0x02)  // 1-2-2
#define MICRON_4PP        (0x38)  // 1-4-4
#define MICRON_QPP        (0x32)  // 1-1-4

#define MICRON_READ_4B    (0x13)
#define MICRON_2READ_4B   (0xBC)
#define MICRON_4READ_4B   (0xEC)
#define MICRON_SE_4B      (0x21)  // SECTOR ERASE
#define MICRON_PP_4B      (0x12)  // 1-1-1
#define MICRON_4PP_4B     (0x3E)  // 1-4-4
#define MICRON_QPP_4B     (0x34)  // 1-1-4

#define MICRON_MANU_ID    (0x20)

#define _4BYTE_ADDR_EN  (4==micron_nor_spif_info.addr_len_b)

SECTION_NOR_SPIF_GEN2_CORE
s32_t micron_nor_spif_unit_erase(const u32_t, const u32_t);
SECTION_NOR_SPIF_GEN2_CORE
s32_t micron_nor_spif_unit_write(const u32_t, u32_t, u32_t, const void *);
SECTION_NOR_SPIF_GEN2_MISC
s32_t micron_nor_spif_read(const u32_t, u32_t, const u32_t, void *);

plr_nor_spi_info_g2_t micron_nor_spif_info = {
	.id             = 0xffffff,
	.num_chips      = 1,
	.wr_boundary_b  = 256,
	.erase_unit_b   = 4096,
	.size_per_chip_b= 0, //16 * 1024 * 1024,
	.addr_len_b     = 3,
	._nor_spi_erase = micron_nor_spif_unit_erase,
	._nor_spi_read  = micron_nor_spif_read,
	._nor_spi_write = micron_nor_spif_unit_write,
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
	u8_t payload = 0x03;

	while ((payload & 0x03) != 0) {
		spif_trx(cs, &wp, &payload, attr, VZERO);
	}

	return;
}

SECTION_NOR_SPIF_GEN2_CORE
s32_t micron_nor_spif_unit_erase(const u32_t cid, const u32_t offset) {
	const nsf_write_phase_t wp = {
		.cmd  = _4BYTE_ADDR_EN?MICRON_SE_4B:MICRON_SE,
		.addr = offset & (~(NORSF_CMN_ERASE_SZ_B - 1)),
	};
	nsf_trx_attr_t attr = {
		.plen_b = 0,
		.write_en = 1,
		.wlen_b = 1 + micron_nor_spif_info.addr_len_b,
		.dummy_ck = 0,
		.pdir  = norsf_ptx,
		.wmode = norsf_sio,
		.pmode = norsf_sio,
	};

	spif_trx(cid, &wp, VZERO, attr, spif_wip_block);

	return 0;
}

SECTION_NOR_SPIF_GEN2_CORE
s32_t micron_nor_spif_unit_write(const u32_t cid, u32_t offset, u32_t leng, const void *buf) {
	nsf_write_phase_t wp = {
		.cmd  = _4BYTE_ADDR_EN?MICRON_PP_4B:MICRON_PP,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 1,
		.wlen_b = 1 + micron_nor_spif_info.addr_len_b,
		.dummy_ck = 0,
		.pdir  = norsf_ptx,
		.wmode = norsf_sio,
		.pmode = norsf_sio,
	};

	spif_trx(cid, &wp, (void *)buf, attr, spif_wip_block);

	return 0;
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t micron_nor_spif_read(const u32_t cid, u32_t offset, const u32_t leng, void *buf) {
	u32_t left = leng;
	nsf_write_phase_t wp = {
		.cmd  = _4BYTE_ADDR_EN?MICRON_READ_4B:MICRON_READ,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 0,
		.wlen_b = 1 + micron_nor_spif_info.addr_len_b,
		.dummy_ck = 0,
		.pdir  = norsf_prx,
		.wmode = norsf_sio,
		.pmode = norsf_sio,
	};

	while (left > attr.plen_b) {
		spif_trx(cid, &wp, buf, attr, VZERO);
		wp.addr += attr.plen_b;
		left -= attr.plen_b;
	}

	if (left) {
		attr.plen_b = left;
		spif_trx(cid, &wp, buf, attr, VZERO);
	}

	return 0;
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t micron_nor_spif_2read(const u32_t cid, u32_t offset, const u32_t leng, void *buf) {
	nsf_write_phase_t wp = {
		.cmd  = _4BYTE_ADDR_EN?MICRON_2READ_4B:MICRON_2READ,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 0,
		.wlen_b = 1 + micron_nor_spif_info.addr_len_b,
		.dummy_ck = 8,
		.pdir  = norsf_prx,
		.wmode = norsf_dio,
		.pmode = norsf_dio,
	};

	spif_trx(cid, &wp, buf, attr, VZERO);

	return 0;
}

SECTION_NOR_SPIF_GEN2_CORE
s32_t micron_nor_spif_unit_4pp(const u32_t cid, u32_t offset, u32_t leng, const void *buf) {
	nsf_write_phase_t wp = {
		.cmd  = _4BYTE_ADDR_EN?MICRON_4PP_4B:MICRON_4PP,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 1,
		.wlen_b = 1 + micron_nor_spif_info.addr_len_b,
		.dummy_ck = 0,
		.pdir  = norsf_ptx,
		.wmode = norsf_qio,
		.pmode = norsf_qio,
	};

	spif_trx(cid, &wp, (void *)buf, attr, spif_wip_block);

	return 0;
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t micron_nor_spif_4read(const u32_t cid, u32_t offset, const u32_t leng, void *buf) {
	nsf_write_phase_t wp = {
		.cmd  = _4BYTE_ADDR_EN?MICRON_4READ_4B:MICRON_4READ,
		.addr = offset,
	};
	nsf_trx_attr_t attr = {
		.plen_b = leng,
		.write_en = 0,
		.wlen_b = 1 + micron_nor_spif_info.addr_len_b,
		.dummy_ck = 10,
		.pdir  = norsf_prx,
		.wmode = norsf_qio,
		.pmode = norsf_qio,
	};

	spif_trx(cid, &wp, buf, attr, VZERO);

	return 0;
}

SECTION_NOR_SPIF_GEN2_CORE
u32_t micron_nor_spif_read_id(void)
{
    u32_t cs = 0;
    u8_t buf[4];
    u32_t *id_val = (u32_t *)buf;
    nsf_write_phase_t wp_rdid = {
        .cmd  = MICRON_RDID,
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
    return ((*id_val)>>8);
}

SECTION_NOR_SPIF_GEN2_CORE
void micron_nor_ctrl_dio_enable(void)
{
    /* controller switch to dual mode */
    SFCR2_T reg;
    reg.v = SFCR2rv;
    reg.f.sfcmd = _4BYTE_ADDR_EN?MICRON_2READ_4B:MICRON_2READ;
    reg.f.cmd_io = norsf_sio;
	reg.f.addr_io = norsf_dio;
	reg.f.dummy_cycles = 0x4;   // 8 dummy cycles
	reg.f.data_io = norsf_dio;
    SFCR2rv=reg.v;

    micron_nor_spif_info._nor_spi_read = micron_nor_spif_2read;
}

SECTION_NOR_SPIF_GEN2_CORE
void micron_nor_ctrl_qio_enable(void)
{
    /* controller switch to quad mode */
    SFCR2_T reg;
    reg.v = SFCR2rv;
    reg.f.sfcmd = _4BYTE_ADDR_EN?MICRON_4READ_4B:MICRON_4READ;
    reg.f.cmd_io = norsf_sio;
	reg.f.addr_io = norsf_qio;
	reg.f.dummy_cycles = 0x5;   // 10 dummy cycles
	reg.f.data_io = norsf_qio;
    printf("reg=> 0x%08x\n", reg.v);
    SFCR2rv=reg.v;

    micron_nor_spif_info._nor_spi_read = micron_nor_spif_4read;
    micron_nor_spif_info._nor_spi_write= micron_nor_spif_unit_4pp;
}

SECTION_NOR_SPIF_GEN2_CORE
void micron_nor_spif_4B_enable(void)
{
    /* controller switch to 4Byte Address mode */
    SWITCH_4B_ADDR_MODE();
    SFCR2_T reg;
    reg.v = SFCR2rv;
    reg.f.sfcmd = MICRON_READ_4B;
    reg.f.cmd_io = norsf_sio;
    reg.f.addr_io = norsf_sio;
    reg.f.dummy_cycles = 0;
	reg.f.data_io = norsf_sio;
	reg.f.mmio_4b_addr_en = _4BYTE_ADDR_EN;
    SFCR2rv=reg.v;
}

SECTION_NOR_SPIF_GEN2_MISC
plr_nor_spi_info_g2_t * micron_nor_spif_probe(void) {
	u32_t sz_mb = micron_nor_spif_info.size_per_chip_b / 1024 / 1024;
	u32_t sz_factor = 0;
    u32_t id = micron_nor_spif_read_id();
    u32_t density = id&0xFF;

    /* check manufactory id */
    if(MICRON_MANU_ID!=(id>>16))  return VZERO;

    puts("MICRON... ");

    /* probe and set density */
    if (0 == sz_mb) {
        if (density >= 0x19) { // 2^25 = 32MB
            puts("4Byte... ");
            if (0x20 == density) density=0x1A;    // ???
            
            sz_factor = density - 19;
            if (sz_factor > 7) puts("EE: NOR SPI-F has wrong size !!");
            micron_nor_spif_info.addr_len_b=4;
            micron_nor_spif_4B_enable();
        } else {
            puts("3Byte... ");
            sz_factor = density - 17;
            micron_nor_spif_info.addr_len_b=3;
        }
        micron_nor_spif_info.size_per_chip_b = 1<<density;
    } else {
        while (sz_mb) {
		    sz_factor++;
		    sz_mb >>= 1;
	    }
    }
    RMOD_SFCR2(sfsize, sz_factor);
    

    /* FIXME: dual or quad mode should be configured somewhere */
    /* enable dual mode */
    //micron_nor_ctrl_dio_enable();
        
    /* enable quad mode */
//    micron_nor_ctrl_qio_enable();
    
    return &micron_nor_spif_info;
}
//REG_NOR_SPIF_PROBE_FUNC(micron_nor_spif_probe);
