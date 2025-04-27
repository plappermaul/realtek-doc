#include <cross_env.h>

#if defined(__LUNA_KERNEL__) || defined(CONFIG_UNDER_UBOOT)
#include "nor_spif_core.h"
#else
#include <nor_spi/nor_spif_core.h>
#include <cpu/cpu.h>
#endif

int norsf_mxic_4b_mode_en(const norsf_g2_info_t *ni, const u32_t cs);
int norsf_mxic_4b_mode_ex(const norsf_g2_info_t *ni, const u32_t cs);
int norsf_mxic_4b_ear_set(const norsf_g2_info_t *ni, const u32_t cs, const u8_t bank);

/* 4B CMD SET */
norsf_erase_cmd_t mx25l25635f_erase_cmds_4b_cmd[] SECTION_NOR_SPIF_GEN2_PARAM =
	{{.a.cmd       = 0xdc,
	  .a.write_en  = 1,
	  .a.pdir      = norsf_ptx,
	  .a.cmode     = norsf_sio,
	  .a.amode     = norsf_sio,
	  .a.pmode     = norsf_sio,
	  .sz_b        = 64*1024,
	  .offset_lmt  = 0,
	  .to_us       = 650*1000},
	 {.a.cmd       = 0x5c,
	  .a.write_en  = 1,
	  .a.pdir      = norsf_ptx,
	  .a.cmode     = norsf_sio,
	  .a.amode     = norsf_sio,
	  .a.pmode     = norsf_sio,
	  .sz_b        = 32*1024,
	  .offset_lmt  = 0,
	  .to_us       = 650*1000},
	 {.a.cmd       = 0x21,
	  .a.write_en  = 1,
	  .a.pdir      = norsf_ptx,
	  .a.cmode     = norsf_sio,
	  .a.amode     = norsf_sio,
	  .a.pmode     = norsf_sio,
	  .sz_b        = 4*1024,
	  .offset_lmt  = 0,
	  .to_us       = 120*1000}};

norsf_cmd_info_t mx25l25635f_cmd_info_4b_cmd SECTION_NOR_SPIF_GEN2_PARAM = {
	.cerase = mx25l25635f_erase_cmds_4b_cmd,

	.cerase_cmd_num = sizeof(mx25l25635f_erase_cmds_4b_cmd)/sizeof(norsf_erase_cmd_t),

	.cprog_attr.cmd      = 0x12,
	.cprog_attr.pdir     = norsf_ptx,
	.cprog_attr.cmode    = norsf_sio,
	.cprog_attr.amode    = norsf_sio,
	.cprog_attr.pmode    = norsf_sio,
	.cprog_attr.write_en = 1,
	.cprog_attr.dummy_ck = 0,
	.cprog_to_us         = 6*1000,
	.cprog_lim_b         = 256,

	.cread_attr.cmd      = 0x13,
	/* .cread_attr.cmd      = 0x0c, */
	.cread_attr.write_en = 0,
	.cread_attr.dummy_ck = 0,
	/* .cread_attr.dummy_ck = 8, */
	.cread_attr.pdir     = norsf_prx,
	.cread_attr.cmode    = norsf_sio,
	.cread_attr.amode    = norsf_sio,
	.cread_attr.pmode    = norsf_sio,
};

/* 3B ADDR MODE */
norsf_erase_cmd_t mx25l25635f_erase_cmds_3b_addr[] SECTION_NOR_SPIF_GEN2_PARAM =
	{{.a.cmd       = 0xd8,
	  .a.write_en  = 1,
	  .a.pdir      = norsf_ptx,
	  .a.cmode     = norsf_sio,
	  .a.amode     = norsf_sio,
	  .a.pmode     = norsf_sio,
	  .sz_b        = 64*1024,
	  .offset_lmt  = 0,
	  .to_us       = 650*1000},
	 {.a.cmd       = 0x52,
	  .a.write_en  = 1,
	  .a.pdir      = norsf_ptx,
	  .a.cmode     = norsf_sio,
	  .a.amode     = norsf_sio,
	  .a.pmode     = norsf_sio,
	  .sz_b        = 32*1024,
	  .offset_lmt  = 0,
	  .to_us       = 650*1000},
	 {.a.cmd       = 0x20,
	  .a.write_en  = 1,
	  .a.pdir      = norsf_ptx,
	  .a.cmode     = norsf_sio,
	  .a.amode     = norsf_sio,
	  .a.pmode     = norsf_sio,
	  .sz_b        = 4*1024,
	  .offset_lmt  = 0,
	  .to_us       = 120*1000}};

norsf_cmd_info_t mx25l25635f_cmd_info_3b_addr SECTION_NOR_SPIF_GEN2_PARAM = {
	.cerase = mx25l25635f_erase_cmds_3b_addr,

	.cerase_cmd_num = sizeof(mx25l25635f_erase_cmds_3b_addr)/sizeof(norsf_erase_cmd_t),

	.cprog_attr.cmd      = 0x02,
	.cprog_attr.pdir     = norsf_ptx,
	.cprog_attr.cmode    = norsf_sio,
	.cprog_attr.amode    = norsf_sio,
	.cprog_attr.pmode    = norsf_sio,
	.cprog_attr.write_en = 1,
	.cprog_attr.dummy_ck = 0,
	.cprog_to_us         = 6*1000,
	.cprog_lim_b         = 256,

	.cread_attr.cmd      = 0x03,
	.cread_attr.write_en = 0,
	.cread_attr.dummy_ck = 0,
	.cread_attr.pdir     = norsf_prx,
	.cread_attr.cmode    = norsf_sio,
	.cread_attr.amode    = norsf_sio,
	.cread_attr.pmode    = norsf_sio,
};

norsf_g2_info_t norsf_mx25l25635f_info_3b_addr = {
	.num_chips      = 1,
	.sec_sz_b       = 4*1024,   /* Set to the sector size for UBoot or Linux */
	.size_per_chip_b= 33554432, /* Set to 0 for auto-size detection. */
	.addr_mode      = norsf_3b_addr,
	.arch_wip       = norsf_cmn_arch_wip,
	.arch_erase     = norsf_cmn_arch_erase,
	.arch_read      = norsf_cmn_arch_read,
	.arch_prog      = norsf_cmn_arch_prog,
	.cmd_info       = &mx25l25635f_cmd_info_3b_addr,
};

/* Enter 4B ADDR MODE */
norsf_g2_info_t norsf_mx25l25635f_info_4b_mode = {
	.num_chips      = 1,
	.sec_sz_b       = 4*1024,   /* Set to the sector size for UBoot or Linux */
	.size_per_chip_b= 33554432, /* Set to 0 for auto-size detection. */
	.addr_mode      = norsf_4b_mode,
	.arch_wip       = norsf_cmn_arch_wip,
	.arch_erase     = norsf_cmn_arch_erase,
	.arch_read      = norsf_cmn_arch_read,
	.arch_prog      = norsf_cmn_arch_prog,
	.am.mode.arch_en4b = norsf_mxic_4b_mode_en,
	.am.mode.arch_ex4b = norsf_mxic_4b_mode_ex,
	.cmd_info       = &mx25l25635f_cmd_info_3b_addr,
};

/* Enter 4B ADDR CMD */
norsf_g2_info_t norsf_mx25l25635f_info_4b_cmd = {
	.num_chips      = 1,
	.sec_sz_b       = 4*1024,   /* Set to the sector size for UBoot or Linux */
	.size_per_chip_b= 33554432, /* Set to 0 for auto-size detection. */
	.addr_mode      = norsf_4b_cmd,
	.arch_wip       = norsf_cmn_arch_wip,
	.arch_erase     = norsf_cmn_arch_erase,
	.arch_read      = norsf_cmn_arch_read,
	.arch_prog      = norsf_cmn_arch_prog,
	.cmd_info       = &mx25l25635f_cmd_info_4b_cmd,
};

/* Enter 4B ADDR MODE */
norsf_g2_info_t norsf_mx25l25635f_info_4b_ear = {
	.num_chips      = 1,
	.sec_sz_b       = 4*1024,   /* Set to the sector size for UBoot or Linux */
	.size_per_chip_b= 33554432, /* Set to 0 for auto-size detection. */
	.addr_mode      = norsf_4b_ear,
	.arch_wip       = norsf_cmn_arch_wip,
	.arch_erase     = norsf_cmn_arch_erase,
	.arch_read      = norsf_cmn_arch_read,
	.arch_prog      = norsf_cmn_arch_prog,
	.am.ear.arch_wrear = norsf_mxic_4b_ear_set,
	.cmd_info       = &mx25l25635f_cmd_info_3b_addr,
};

SECTION_NOR_SPIF_GEN2_CORE
int norsf_mxic_4b_mode_en(const norsf_g2_info_t *ni, const u32_t cs) {
	u32_t res;
	nsf_trx_attr_t attr = {
		.cs       = cs,
		.cmd      = 0xb7,
		.write_en = 0,
		.dummy_ck = 0,
		.pdir     = norsf_ptx,
		.cmode    = norsf_sio,
		.amode    = norsf_sio,
		.pmode    = norsf_sio,
	};

	res = norsf_compound_cmd(-1, VZERO, attr, VZERO);

	return res;
}

SECTION_NOR_SPIF_GEN2_CORE
int norsf_mxic_4b_mode_ex(const norsf_g2_info_t *ni, const u32_t cs) {
	u32_t res;
	nsf_trx_attr_t attr = {
		.cs       = cs,
		.cmd      = 0xe9,
		.write_en = 0,
		.dummy_ck = 0,
		.pdir     = norsf_ptx,
		.cmode    = norsf_sio,
		.amode    = norsf_sio,
		.pmode    = norsf_sio,
	};

	res = norsf_compound_cmd(-1, VZERO, attr, VZERO);

	return res;
}

SECTION_NOR_SPIF_GEN2_MISC
int norsf_mxic_4b_ear_set(const norsf_g2_info_t *ni, const u32_t cs, u8_t bank) {
	u32_t res;
	nsf_trx_attr_t attr = {
		.cs       = cs,
		.cmd      = 0xc5,
		.plen_b   = 1,
		.write_en = 1,
		.dummy_ck = 0,
		.pdir     = norsf_ptx,
		.cmode    = norsf_sio,
		.amode    = norsf_sio,
		.pmode    = norsf_sio,
	};

	res = norsf_compound_cmd(-1, &bank, attr, VZERO);

	return res;
}

#ifdef NORSF_4B_MODE_DEV
u32_t exp_norsf_sel_addr_type = 0;

SECTION_NOR_SPIF_GEN2_CORE
int norsf_mxic_sw_reset(void) {
	nsf_trx_attr_t attr = {
		.cs       = 0,
		.write_en = 0,
		.dummy_ck = 0,
		.pdir     = norsf_ptx,
		.cmode    = norsf_sio,
		.amode    = norsf_sio,
		.pmode    = norsf_sio,
	};

	attr.cmd = 0x66;
	norsf_compound_cmd(-1, VZERO, attr, VZERO);
	attr.cmd = 0x99;
	norsf_compound_cmd(-1, VZERO, attr, VZERO);

	return 0;
}
#endif

SECTION_NOR_SPIF_GEN2_MISC
norsf_g2_info_t *norsf_mxic_probe(void) {
	u32_t norsf_id = norsf_rdid(0);
	norsf_g2_info_t *res = VZERO;

	switch (norsf_id) {
	case 0xc22019:
#ifdef NORSF_4B_MODE_DEV
		norsf_mxic_sw_reset();
		switch (exp_norsf_sel_addr_type) {
		case 0:
			res = &norsf_mx25l25635f_info_3b_addr;
			break;
		case 1:
			res = &norsf_mx25l25635f_info_4b_mode;
			break;
		case 2:
			res = &norsf_mx25l25635f_info_4b_cmd;
			break;
		case 3:
			res = &norsf_mx25l25635f_info_4b_ear;
			break;
		default:
			res = &norsf_mx25l25635f_info_4b_cmd;
		}
#else
			res = &norsf_mx25l25635f_info_4b_cmd;
#endif
		break;
	default:
		break;
	}

	if (res) puts("MXIC:");
	return res;
}
REG_NOR_SPIF_PROBE_FUNC(norsf_mxic_probe);
