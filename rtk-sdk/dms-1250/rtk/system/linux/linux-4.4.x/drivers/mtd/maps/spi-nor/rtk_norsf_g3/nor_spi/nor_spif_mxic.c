#include <common/rt_type.h>
#include <cross_env.h>
#include <nor_spi/nor_spif_core.h>

static int norsf_mxic_xread_en(const norsf_g2_info_t *ni, const u32_t cs);
static int norsf_mxic_xread_ex(const norsf_g2_info_t *ni, const u32_t cs);

norsf_erase_cmd_t mxic_erase_cmds_3b[] SECTION_NOR_SPIF_GEN2_PARAM = {
    {.a.cmd       = 0xD8,
      .a.write_en  = 1,
      .a.pdir      = norsf_ptx,
      .a.cmode     = norsf_sio,
      .a.amode     = norsf_sio,
      .a.pmode     = norsf_sio,
      .sz_b        = 64*1024,
      .offset_lmt  = 0,
      .to_us       = 2*1000*1000},
     {.a.cmd       = 0x52,
      .a.write_en  = 1,
      .a.pdir      = norsf_ptx,
      .a.cmode     = norsf_sio,
      .a.amode     = norsf_sio,
      .a.pmode     = norsf_sio,
      .sz_b        = 32*1024,
      .offset_lmt  = 0,
      .to_us       = 1*1000*1000},
     {.a.cmd       = 0x20,
      .a.write_en  = 1,
      .a.pdir      = norsf_ptx,
      .a.cmode     = norsf_sio,
      .a.amode     = norsf_sio,
      .a.pmode     = norsf_sio,
      .sz_b        = 4*1024,
      .offset_lmt  = 0,
      .to_us       = 200*1000}};

/* 4B CMD SET */
norsf_erase_cmd_t mxic_erase_cmds_4b_cmd[] SECTION_NOR_SPIF_GEN2_PARAM ={
    { .a.cmd       = 0xDC,
      .a.write_en  = 1,
      .a.pdir      = norsf_ptx,
      .a.cmode     = norsf_sio,
      .a.amode     = norsf_sio,
      .a.pmode     = norsf_sio,
      .sz_b        = 64*1024,
      .offset_lmt  = 0,
      .to_us       = 2000*1000    },
    { .a.cmd       = 0x5C,
      .a.write_en  = 1,
      .a.pdir      = norsf_ptx,
      .a.cmode     = norsf_sio,
      .a.amode     = norsf_sio,
      .a.pmode     = norsf_sio,
      .sz_b        = 32*1024,
      .offset_lmt  = 0,
      .to_us       = 1000*1000   },
    { .a.cmd       = 0x21,
      .a.write_en  = 1,
      .a.pdir      = norsf_ptx,
      .a.cmode     = norsf_sio,
      .a.amode     = norsf_sio,
      .a.pmode     = norsf_sio,
      .sz_b        = 4*1024,
      .offset_lmt  = 0,
     .to_us       = 400*1000}};

norsf_read_cmd_t mxic_read_cmd_3b SECTION_NOR_SPIF_GEN2_PARAM = {
#if (NORSF_XREAD_EN == 1)
    .a.cmd      = 0xEB,
    .a.write_en = 0,
    .a.dummy_ck = 6,
    .a.pdir     = norsf_prx,
    .a.cmode    = norsf_sio,
    .a.amode    = norsf_qio,
    .a.pmode    = norsf_qio,

    .xread_en   = norsf_mxic_xread_en,
    .xread_ex   = norsf_mxic_xread_ex,
#else
    .a.cmd      = 0xBB,
    .a.write_en = 0,
    .a.dummy_ck = 4,
    .a.pdir     = norsf_prx,
    .a.cmode    = norsf_sio,
    .a.amode    = norsf_dio,
    .a.pmode    = norsf_dio,

    .xread_en   = VZERO,
    .xread_ex   = VZERO,
#endif
};

norsf_read_cmd_t mxic_read_cmd_4b_cmd SECTION_NOR_SPIF_GEN2_PARAM = {
#if (NORSF_XREAD_EN == 1)
    .a.cmd      = 0xEC,
    .a.write_en = 0,
    .a.dummy_ck = 6,
    .a.pdir     = norsf_prx,
    .a.cmode    = norsf_sio,
    .a.amode    = norsf_qio,
    .a.pmode    = norsf_qio,

    .xread_en   = norsf_mxic_xread_en,
    .xread_ex   = norsf_mxic_xread_ex,
#else
    .a.cmd      = 0xBC,
    .a.write_en = 0,
    .a.dummy_ck = 4,
    .a.pdir     = norsf_prx,
    .a.cmode    = norsf_sio,
    .a.amode    = norsf_dio,
    .a.pmode    = norsf_dio,

    .xread_en   = VZERO,
    .xread_ex   = VZERO,
#endif
};

norsf_cmd_info_t mxic_cmd_info_3b SECTION_NOR_SPIF_GEN2_PARAM = {
    .cerase = mxic_erase_cmds_3b,

    .cerase_cmd_num = sizeof(mxic_erase_cmds_3b)/sizeof(norsf_erase_cmd_t),

    .cprog_attr.cmd      = 0x02,
    .cprog_attr.pdir     = norsf_ptx,
    .cprog_attr.cmode    = norsf_sio,
    .cprog_attr.amode    = norsf_sio,
    .cprog_attr.pmode    = norsf_sio,
    .cprog_attr.write_en = 1,
    .cprog_attr.dummy_ck = 0,
    .cprog_to_us         = 6*1000,
    .cprog_lim_b         = 256,

    .cread = &mxic_read_cmd_3b,
};

norsf_cmd_info_t mxic_cmd_info_4b_cmd SECTION_NOR_SPIF_GEN2_PARAM = {
    .cerase = mxic_erase_cmds_4b_cmd,

    .cerase_cmd_num = sizeof(mxic_erase_cmds_4b_cmd)/sizeof(norsf_erase_cmd_t),

    .cprog_attr.cmd      = 0x12,
    .cprog_attr.pdir     = norsf_ptx,
    .cprog_attr.cmode    = norsf_sio,
    .cprog_attr.amode    = norsf_sio,
    .cprog_attr.pmode    = norsf_sio,
    .cprog_attr.write_en = 1,
    .cprog_attr.dummy_ck = 0,
    .cprog_to_us         = 6*1000,
    .cprog_lim_b         = 256,

    .cread = &mxic_read_cmd_4b_cmd,
};

norsf_g2_info_t norsf_mx25l6435e_info SECTION_NOR_SPIF_GEN2_PARAM = {
    .num_chips       = 1,
    .sec_sz_b        = 4*1024,   /* Set to the sector size for UBoot or Linux */
    .size_per_chip_b = 8*1024*1024, /* Set to 0 for auto-size detection. */
    .addr_mode       = norsf_3b_addr,
    .arch_wip        = norsf_cmn_arch_wip,
    .arch_erase      = norsf_cmn_arch_erase,
    .arch_read       = norsf_cmn_arch_read,
    .arch_prog       = norsf_cmn_arch_prog,
    .cmd_info        = &mxic_cmd_info_3b,
};

norsf_g2_info_t norsf_mx25l12835f_info SECTION_NOR_SPIF_GEN2_PARAM = {
    .num_chips       = 1,
    .sec_sz_b        = 4*1024,   /* Set to the sector size for UBoot or Linux */
    .size_per_chip_b = 16*1024*1024, /* Set to 0 for auto-size detection. */
    .addr_mode       = norsf_3b_addr,
    .arch_wip        = norsf_cmn_arch_wip,
    .arch_erase      = norsf_cmn_arch_erase,
    .arch_read       = norsf_cmn_arch_read,
    .arch_prog       = norsf_cmn_arch_prog,
    .cmd_info        = &mxic_cmd_info_3b,
};

norsf_g2_info_t norsf_mx25l25635f_info_4b_cmd SECTION_NOR_SPIF_GEN2_PARAM = {
    .num_chips       = 1,
    .sec_sz_b        = 4*1024,   /* Set to the sector size for UBoot or Linux */
    .size_per_chip_b = 32*1024*1024, /* Set to 0 for auto-size detection. */
    .addr_mode       = norsf_4b_cmd,
    .arch_wip        = norsf_cmn_arch_wip,
    .arch_erase      = norsf_cmn_arch_erase,
    .arch_read       = norsf_cmn_arch_read,
    .arch_prog       = norsf_cmn_arch_prog,
    .cmd_info        = &mxic_cmd_info_4b_cmd,
};

norsf_g2_info_t norsf_mx25l51245f_info_4b_cmd SECTION_NOR_SPIF_GEN2_PARAM = {
    .num_chips       = 1,
    .sec_sz_b        = 4*1024,   /* Set to the sector size for UBoot or Linux */
    .size_per_chip_b = 64*1024*1024, /* Set to 0 for auto-size detection. */
    .addr_mode       = norsf_4b_cmd,
    .arch_wip        = norsf_cmn_arch_wip,
    .arch_erase      = norsf_cmn_arch_erase,
    .arch_read       = norsf_cmn_arch_read,
    .arch_prog       = norsf_cmn_arch_prog,
    .cmd_info        = &mxic_cmd_info_4b_cmd,
};

SECTION_NOR_SPIF_GEN2_CORE
static u8_t norsf_mxic_rdsr(u32_t chip_sel) {
    u8_t _status;
    nsf_trx_attr_t attr = {
        .cmd      = 0x05,
        .cs       = chip_sel,
        .plen_b   = 1,
        .write_en = 0,
        .dummy_ck = 0,
        .pdir     = norsf_prx,
        .cmode    = norsf_sio,
        .amode    = norsf_sio,
        .pmode    = norsf_sio,
    };
    norsf_compound_cmd(-1, &_status, attr, VZERO);
    return _status;
}

SECTION_NOR_SPIF_GEN2_CORE
static void norsf_mxic_wrsr(u32_t chip_sel, u8_t status) {
    nsf_trx_attr_t attr = {
        .cmd      = 0x01,
        .cs       = chip_sel,
        .plen_b   = 1,
        .write_en = 1,
        .dummy_ck = 0,
        .pdir     = norsf_ptx,
        .cmode    = norsf_sio,
        .amode    = norsf_sio,
        .pmode    = norsf_sio,
    };
    norsf_wip_info_t wi = {
        .to_c = -1,
        .wip = norsf_cmn_arch_wip,
    };
    norsf_compound_cmd(-1, &status, attr, &wi);
    return;
}

SECTION_NOR_SPIF_GEN2_CORE
__attribute__ ((unused))
static int norsf_mxic_xread_en(const norsf_g2_info_t *ni,
                               const u32_t cs) {
    u8_t status;

    status = norsf_mxic_rdsr(cs);

    /* enable quad-enable bit. */
    status |= 0x40;

    norsf_mxic_wrsr(cs, status);

    return 0;
}

SECTION_NOR_SPIF_GEN2_CORE
__attribute__ ((unused))
static int norsf_mxic_xread_ex(const norsf_g2_info_t *ni,
                               const u32_t cs) {
    u8_t status;

    status = norsf_mxic_rdsr(cs);

    /* disable quad-enable bit. */
    status &= (~0x40);

    norsf_mxic_wrsr(cs, status);

    return 0;
}

SECTION_NOR_SPIF_GEN2_MISC
norsf_g2_info_t *norsf_mxic_probe(void) {
    u32_t norsf_id = norsf_rdid(0);
    norsf_g2_info_t *res = VZERO;

    switch (norsf_id) {
    case 0xC22017:
    res = &norsf_mx25l6435e_info;
    break;
    case 0xC22018:
        res = &norsf_mx25l12835f_info;
        break;
    case 0xC22019:
        res = &norsf_mx25l25635f_info_4b_cmd;
        break;
    case 0xC2201A:
        res = &norsf_mx25l51245f_info_4b_cmd;
        break;
    default:
        break;
    }

    if (res) {
        puts("MXIC/");
    }
    return res;
}

REG_NOR_SPIF_PROBE_FUNC(norsf_mxic_probe);
