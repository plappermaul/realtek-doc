#include <cross_env.h>
#include <nor_spi/nor_spif_core.h>

norsf_erase_cmd_t mt25ql512ab_erase_cmds_4b_cmd[] SECTION_NOR_SPIF_GEN2_PARAM =
	{{.a.cmd       = 0xdc,
	  .a.write_en  = 1,
	  .a.pdir      = norsf_ptx,
	  .a.cmode     = norsf_sio,
	  .a.amode     = norsf_sio,
	  .a.pmode     = norsf_sio,
	  .sz_b        = 64*1024,
	  .offset_lmt  = 0,
	  .to_us       = 1*1000*1000},
	 {.a.cmd       = 0x21,
	  .a.write_en  = 1,
	  .a.pdir      = norsf_ptx,
	  .a.cmode     = norsf_sio,
	  .a.amode     = norsf_sio,
	  .a.pmode     = norsf_sio,
	  .sz_b        = 4*1024,
	  .offset_lmt  = 0,
	  .to_us       = 400*1000}};

norsf_read_cmd_t mt25ql512ab_read_cmd_4b_cmd SECTION_NOR_SPIF_GEN2_PARAM = {
#if (NORSF_XREAD_EN == 1)
	.a.cmd      = 0xEC,
	.a.write_en = 0,
	.a.dummy_ck = 10,
	.a.pdir     = norsf_prx,
	.a.cmode    = norsf_sio,
	.a.amode    = norsf_qio,
	.a.pmode    = norsf_qio,

	.xread_en   = VZERO,
	.xread_ex   = VZERO,
#else
	.a.cmd      = 0x13,
	.a.write_en = 0,
	.a.dummy_ck = 0,
	.a.pdir     = norsf_prx,
	.a.cmode    = norsf_sio,
	.a.amode    = norsf_sio,
	.a.pmode    = norsf_sio,

	.xread_en   = VZERO,
	.xread_ex   = VZERO,
#endif
};

norsf_cmd_info_t mt25ql512ab_cmd_info_4b_cmd SECTION_NOR_SPIF_GEN2_PARAM = {
	.cerase = mt25ql512ab_erase_cmds_4b_cmd,

	.cerase_cmd_num = sizeof(mt25ql512ab_erase_cmds_4b_cmd)/sizeof(norsf_erase_cmd_t),

	.cprog_attr.cmd      = 0x12,
	.cprog_attr.pdir     = norsf_ptx,
	.cprog_attr.cmode    = norsf_sio,
	.cprog_attr.amode    = norsf_sio,
	.cprog_attr.pmode    = norsf_sio,
	.cprog_attr.write_en = 1,
	.cprog_attr.dummy_ck = 0,
	.cprog_to_us         = 5*1000,
	.cprog_lim_b         = 256,

	.cread = &mt25ql512ab_read_cmd_4b_cmd,
};

norsf_g2_info_t norsf_mt25ql512ab_info_4b_cmd = {
	.num_chips      = 1,
	.sec_sz_b       = 4*1024,   /* Set to the sector size for UBoot or Linux */
	.size_per_chip_b= 64*1024*1024, /* Set to 0 for auto-size detection. */
	.addr_mode      = norsf_4b_cmd,
	.arch_wip       = norsf_cmn_arch_wip,

	.arch_erase     = norsf_cmn_arch_erase,
	.arch_read      = norsf_cmn_arch_read,
	.arch_prog      = norsf_cmn_arch_prog,

	.cmd_info       = &mt25ql512ab_cmd_info_4b_cmd,
};

SECTION_NOR_SPIF_GEN2_MISC
norsf_g2_info_t *norsf_micron_probe(void) {
	u32_t norsf_id = norsf_rdid(0);
	norsf_g2_info_t *res = VZERO;

	switch (norsf_id) {
	case 0x20ba20:
		res = &norsf_mt25ql512ab_info_4b_cmd;
		break;
	default:
		break;
	}

	if (res) {
		puts("MICRON:");
	}
	return res;
}

REG_NOR_SPIF_PROBE_FUNC(norsf_micron_probe);
