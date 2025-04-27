#include <common/rt_type.h>
#include <cross_env.h>
#include <nor_spi/nor_spif_core.h>
#include <nor_spi/nor_spif_register.h>

#if defined(__LUNA_KERNEL__) || defined(CONFIG_UNDER_UBOOT)
    #define NORSFG2_PROLOGUE_HINT()
    #define NORSFG2_EPILOGUE_HINT()
    #define NORSFG2_ACTION_PREFIX
    #define NORSFG2_PP_VERIFY_EN (1)
#if defined(CONFIG_UNDER_UBOOT)
    #define GET_CPU_CYCLE() read_c0_count()
#else
    #define GET_CPU_CYCLE() jiffies
#endif
#else
    #define NORSFG2_PROLOGUE_HINT() puts("II: NOR SPI-F... ")
    #define NORSFG2_EPILOGUE_HINT() puts("done\n");
    #define NORSFG2_ACTION_PREFIX   "II: "
    #define NORSFG2_PP_VERIFY_EN (0)
    #define GET_CPU_CYCLE() CPU_GET_CP0_CYCLE_COUNT()
    REG_INIT_FUNC(norsf_detect, 20);
#endif

#if defined(__LUNA_KERNEL__)
void schedule(void);
#if (defined(CONFIG_DEFAULTS_KERNEL_3_18) && (CONFIG_DEFAULTS_KERNEL_3_18 == 1))
    DEFINE_SPINLOCK(spi_lock);
#elif (defined(CONFIG_KERNEL_2_6_30) && (CONFIG_KERNEL_2_6_30 == 1))
    spinlock_t spi_lock = SPIN_LOCK_UNLOCKED;
#else
    #error EE: UNKNOWN KERNEL version
#endif
unsigned long spi_lock_flags;
#define INTERRUPT_DISABLE() do { \
        spin_lock_irqsave(&spi_lock, spi_lock_flags); \
    } while (0)
#define INTERRUPT_ENABLE() do { \
        spin_unlock_irqrestore(&spi_lock, spi_lock_flags); \
    } while (0)
#define OS_SCHEDULE_OUT() schedule()
    #undef RETURN_STACK_SAVE
    #undef RETURN_STACK_RESTORE
    #define RETURN_STACK_SAVE(_bpctl_backup)
    #define RETURN_STACK_RESTORE(_bpctl_backup)
#else
#define INTERRUPT_ENABLE()
#define INTERRUPT_DISABLE()
#define OS_SCHEDULE_OUT()
#endif

#ifndef GET_CPU_MHZ
#error EE: Missing CPU freq. query function.
#endif

#ifndef GET_CPU_CYCLE
#error EE: Missing CPU cycle count retriever.
#endif

#ifndef NORSF_WBUF_LIM_B
    #define NORSF_WBUF_LIM_B 256
#endif

#if (NORSFG2_PP_VERIFY_EN == 1)
    #define NORSFG2_PP_VERIFY(src, dst, len) do { \
        norsf_int_memcmp(src, dst, len); \
    } while (0)
#else
    #define NORSFG2_PP_VERIFY(src, dst, len)
#endif

#define alloca(sz) __builtin_alloca(sz)
#define NORSF_MIN(x, y) (y ^ ((x ^ y) & -(x < y)))
#define MEGABYTE(n) (n * 1024 * 1024)

#define WAIT_FOR_CS() do { \
        SFCSR_T sfcsr_cs_mask = { .f.spi_csb0_sts = 1, \
                                  .f.spi_csb1_sts = 1 }; \
        SFCSR_T sfcsr_cur; \
        reg_soc_read(NOR_G2_SFCSRr, &sfcsr_cur.v); \
        while ((sfcsr_cur.v & sfcsr_cs_mask.v) == sfcsr_cs_mask.v) { \
            reg_soc_read(NOR_G2_SFCSRr, &sfcsr_cur.v); \
        } \
    } while (0)

#define POS_ADVANCE(cs, offset, flash_sz_b, accum_b, res) do { \
        if (res < 0) goto t4_exception; \
        offset  += res; \
        accum_b += res; \
        if (offset >= flash_sz_b) { \
            cs++; \
            offset = 0; \
        } \
    } while (0)

#define NOP_DELAY(num) do { \
        u32_t _num = (num); \
        while (_num--) { \
            asm volatile ("nop;\n"); \
        } \
    } while (0)

#ifdef SNOFG3_HAS_EXT_4B_ADDR_EN_FUNC
extern void norsf_proj_4b_addr_en(u32_t enable);
#else
SECTION_NOR_SPIF_GEN2_CORE
void norsf_proj_4b_addr_en(u32_t enable) {
    RMOD_SFCR2(mmio_4b_addr_en, enable);
    return;
}
#endif

norsf_erase_cmd_t cmn_erase_cmds[] SECTION_NOR_SPIF_GEN2_PARAM =
    {{.a.cmd       = 0xd8,
      .a.write_en  = 1,
      .a.pdir      = norsf_ptx,
      .a.cmode     = norsf_sio,
      .a.amode     = norsf_sio,
      .a.pmode     = norsf_sio,
      .sz_b        = 64*1024,
      .offset_lmt  = 0,
      .to_us       = -1},
     {.a.cmd       = 0x52,
      .a.write_en  = 1,
      .a.pdir      = norsf_ptx,
      .a.cmode     = norsf_sio,
      .a.amode     = norsf_sio,
      .a.pmode     = norsf_sio,
      .sz_b        = 32*1024,
      .offset_lmt  = 0,
      .to_us       = -1},
     {.a.cmd       = 0x20,
      .a.write_en  = 1,
      .a.pdir      = norsf_ptx,
      .a.cmode     = norsf_sio,
      .a.amode     = norsf_sio,
      .a.pmode     = norsf_sio,
      .sz_b        = 4*1024,
      .offset_lmt  = 0,
      .to_us       = -1}};

norsf_read_cmd_t cmn_read_cmd SECTION_NOR_SPIF_GEN2_PARAM = {
    .a.cmd      = 0x03,
    .a.write_en = 0,
    .a.dummy_ck = 0,
    .a.pdir     = norsf_prx,
    .a.cmode    = norsf_sio,
    .a.amode    = norsf_sio,
    .a.pmode    = norsf_sio,
    .xread_en   = VZERO,
    .xread_ex   = VZERO,
};

norsf_cmd_info_t cmn_cmd_info SECTION_NOR_SPIF_GEN2_PARAM = {
    .cerase = cmn_erase_cmds,

    .cerase_cmd_num = sizeof(cmn_erase_cmds)/sizeof(norsf_erase_cmd_t),

    .cprog_attr.cmd      = 0x02,
    .cprog_attr.pdir     = norsf_ptx,
    .cprog_attr.cmode    = norsf_sio,
    .cprog_attr.amode    = norsf_sio,
    .cprog_attr.pmode    = norsf_sio,
    .cprog_attr.write_en = 1,
    .cprog_attr.dummy_ck = 0,
    .cprog_to_us         = -1,
    .cprog_lim_b         = 256,

    .cread = &cmn_read_cmd,
};

norsf_g2_info_t norsf_info SECTION_NOR_SPIF_GEN2_COREDATA = {
    .num_chips      = 1,
    .addr_mode      = norsf_3b_addr,
    .sec_sz_b       = 4*1024, /* Set to the sector size for UBoot or Linux */
    .size_per_chip_b= 0,      /* Set to 0 for auto-size detection. */

    .arch_erase     = norsf_cmn_arch_erase,
    .arch_prog      = norsf_cmn_arch_prog,
    .arch_read      = norsf_cmn_arch_read,
    .arch_wip       = norsf_cmn_arch_wip,

    .cmd_info       = &cmn_cmd_info,
};

#if (NORSFG2_PP_VERIFY_EN == 1)
SECTION_NOR_SPIF_GEN2_MISC
static void norsf_int_memcmp(u8_t *debug_src, u8_t *debug_dst, u32_t debug_len) {
    while (debug_len--) {
        if (*debug_src != *debug_dst) {
            printf("EE: debug_src: %p: %02x %02x %02x %02x *%02x* %02x %02x %02x %02x\n",
                   debug_src,
                   *(u8_t *)(((u32_t)debug_src - 4)),
                   *(u8_t *)(((u32_t)debug_src - 3)),
                   *(u8_t *)(((u32_t)debug_src - 2)),
                   *(u8_t *)(((u32_t)debug_src - 1)),
                   *(u8_t *)(((u32_t)debug_src)),
                   *(u8_t *)(((u32_t)debug_src + 1)),
                   *(u8_t *)(((u32_t)debug_src + 2)),
                   *(u8_t *)(((u32_t)debug_src + 3)),
                   *(u8_t *)(((u32_t)debug_src + 4)));
            printf("EE: debug_dst: %p: %02x %02x %02x %02x *%02x* %02x %02x %02x %02x\n",
                   debug_dst,
                   *(u8_t *)(((u32_t)debug_dst - 4)),
                   *(u8_t *)(((u32_t)debug_dst - 3)),
                   *(u8_t *)(((u32_t)debug_dst - 2)),
                   *(u8_t *)(((u32_t)debug_dst - 1)),
                   *(u8_t *)(((u32_t)debug_dst)),
                   *(u8_t *)(((u32_t)debug_dst + 1)),
                   *(u8_t *)(((u32_t)debug_dst + 2)),
                   *(u8_t *)(((u32_t)debug_dst + 3)),
                   *(u8_t *)(((u32_t)debug_dst + 4)));
            break;
        }
        debug_src++;
        debug_dst++;
    }
    return;
}
#endif

SECTION_NOR_SPIF_GEN2_CORE
static void norsf_int_check_ctrl_ready(void) {
    SFCSR_T sfcsr_rdy_n_idle_mask = {
        .f.spi_rdy  = 1,
        .f.spi_idle = 1,
    };
    SFCSR_T sfcsr_cur;

    u32_t err = 0;

    NOP_DELAY(6);

    reg_soc_read(NOR_G2_SFCSRr, &sfcsr_cur.v);
    while ((sfcsr_rdy_n_idle_mask.v & sfcsr_cur.v) != sfcsr_rdy_n_idle_mask.v) {
        /* Say each iteration of this loop takes 10 cycles,
           on a 500MHz CPU, it polls for about 20ms. */
        if ((err++) > (1024*1024)) {
            puts("EE: NOR SPI flash controller does not respond.\n");
            while (1);
        }
        reg_soc_read(NOR_G2_SFCSRr, &sfcsr_cur.v);
    }

    return;
}

SECTION_NOR_SPIF_GEN2_CORE
static void norsf_int_cs_deact(void) {
    SFCSR_T sfcsr_deact_mask = {
        .f.spi_csb0_sts = 1,
        .f.spi_csb1_sts = 1,
        .f.spi_rdy      = 1,
        .f.spi_idle     = 1,
    };
    SFCSR_T sfcsr_cur;

    /* Activate all in case RD_OPT = 1 and MMIO already sets CS low */
    reg_soc_read(NOR_G2_SFCSRr, &sfcsr_cur.v);
    sfcsr_cur.f.spi_csb0 = 0;
    sfcsr_cur.f.spi_csb1 = 0;
    reg_soc_write(NOR_G2_SFCSRr, &sfcsr_cur.v);
    NOP_DELAY(32);

    /* Deactivate all */
    reg_soc_read(NOR_G2_SFCSRr, &sfcsr_cur.v);
    sfcsr_cur.f.spi_csb0 = 1;
    sfcsr_cur.f.spi_csb1 = 1;
    reg_soc_write(NOR_G2_SFCSRr, &sfcsr_cur.v);

    /* Following delay period is necessary to prevent controller from *NOT* ready. */
    NOP_DELAY(32);

    reg_soc_read(NOR_G2_SFCSRr, &sfcsr_cur.v);
    while ((sfcsr_deact_mask.v & sfcsr_cur.v) != sfcsr_deact_mask.v) {
        reg_soc_read(NOR_G2_SFCSRr, &sfcsr_cur.v);
    }

    return;
}

/* Tier-I function */
SECTION_NOR_SPIF_GEN2_CORE
static s32_t norsf_single_cmd(u32_t addr, u8_t *payload, nsf_trx_attr_t attr) {
    SFCSR_T sfcsr_wk;
    u32_t sfdr_buf;
    u32_t bpctl_backup __attribute__((unused));

    u32_t i;
    volatile u8_t pf_dummy __attribute__((unused));

    INTERRUPT_DISABLE();
    RETURN_STACK_SAVE(bpctl_backup);

    /* Pre-fetch payload to cache to reduce uncertain DRAM traffic. */
    i = 0;
    if (payload != VZERO) {
        while (i < attr.plen_b) {
            pf_dummy = payload[i];
        i += DCACHE_LINE_SZ_B;
    }
        pf_dummy = payload[attr.plen_b - 1];
    }

    /* Reset phase */
    norsf_int_check_ctrl_ready();
    norsf_int_cs_deact();

    reg_soc_read(NOR_G2_SFCSRr, &sfcsr_wk.v);
    if (attr.cs == 0) {
        sfcsr_wk.f.spi_csb0 = 0;
    } else if (attr.cs == 1) {
        sfcsr_wk.f.spi_csb1 = 0;
    }

    /* Write phase: cmd */
    sfcsr_wk.f.len      = 0;
    sfcsr_wk.f.io_width = attr.cmode;

    reg_soc_write(NOR_G2_SFCSRr, &sfcsr_wk.v);

    WAIT_FOR_CS();

    sfdr_buf = attr.cmd << 24;
    reg_soc_write(NOR_G2_SFDRr, &sfdr_buf);

    norsf_int_check_ctrl_ready();

    /* Write phase: addr */
    if (addr != -1) {
        sfcsr_wk.f.len      = norsf_info.pio_addr_len;
        sfcsr_wk.f.io_width = attr.amode;
        reg_soc_write(NOR_G2_SFCSRr, &sfcsr_wk.v);

        sfdr_buf = addr << ((3 - sfcsr_wk.f.len)*8);
        reg_soc_write(NOR_G2_SFDRr, &sfdr_buf);

        norsf_int_check_ctrl_ready();
    }

    /* Payload phase */
    if (payload != VZERO) {
        sfcsr_wk.f.io_width = attr.pmode;

        /* dummy cycle phase, if any. */
        if (attr.dummy_ck) {
            const int dummy_byte = (attr.dummy_ck * ( 1 << attr.pmode) / 8);
            for (i=0; i<dummy_byte; i++) {
                sfcsr_wk.f.len = 0;
            reg_soc_write(NOR_G2_SFCSRr, &sfcsr_wk.v);
            sfdr_buf = 0;
            reg_soc_write(NOR_G2_SFDRr, &sfdr_buf);
            norsf_int_check_ctrl_ready();
        }
        }

        /* continue demanded access. */
        sfcsr_wk.f.len = 0;

        reg_soc_write(NOR_G2_SFCSRr, &sfcsr_wk.v);

        while (attr.plen_b--) {
            if (attr.pdir == norsf_prx) {
                reg_soc_read(NOR_G2_SFDRr, &sfdr_buf);
                *(payload++) = sfdr_buf >> 24;
            } else {
                sfdr_buf = *(payload++) << 24;
                reg_soc_write(NOR_G2_SFDRr, &sfdr_buf);
            }
            norsf_int_check_ctrl_ready();
        }
    }

    /* Reset phase */
    norsf_int_check_ctrl_ready();
    norsf_int_cs_deact();

    RETURN_STACK_RESTORE(bpctl_backup);
    INTERRUPT_ENABLE();

    return 0;
}

SECTION_NOR_SPIF_GEN2_CORE
static void norsf_int_configure_mmio_read(norsf_g2_info_t *ni,
                                          u32_t sz_field,
                                          const nsf_trx_attr_t cread) {
    u32_t i;
    SFCR2_T sfcr2_buf;

    norsf_int_check_ctrl_ready();

    if (ni->addr_mode & norsf_4b_mode) {
        for (i=0; i<ni->num_chips; i++) {
            if (ni->is_mr4a) {
                ni->am.mode.arch_en4b(ni, i);
            } else {
                ni->am.mode.arch_ex4b(ni, i);
            }
        }
    }
    norsf_proj_4b_addr_en(ni->is_mr4a);

#if (NORSF_XREAD_EN == 1)
    if (ni->cmd_info->cread->xread_en) {
        for (i=0; i<ni->num_chips; i++) {
            ni->cmd_info->cread->xread_en(ni, i);
        }
    }
#endif

    reg_soc_read(NOR_G2_SFCR2r, &sfcr2_buf.v);
    sfcr2_buf.f.sfcmd        = cread.cmd;
    sfcr2_buf.f.sfsize       = sz_field;
    sfcr2_buf.f.dummy_cycles = cread.dummy_ck/2;
    sfcr2_buf.f.cmd_io       = cread.cmode;
    sfcr2_buf.f.addr_io      = cread.amode;
    sfcr2_buf.f.data_io      = cread.pmode;
    reg_soc_write(NOR_G2_SFCR2r, &sfcr2_buf.v);

    reg_soc_read(NOR_G2_SFCR2r, &sfcr2_buf.v);
    while (sfcr2_buf.f.mmio_4b_addr_en != ni->is_mr4a) {
        reg_soc_read(NOR_G2_SFCR2r, &sfcr2_buf.v);
    }

    norsf_int_check_ctrl_ready();

    return;
}

/* Tier-II function */
SECTION_NOR_SPIF_GEN2_CORE
s32_t norsf_compound_cmd(const u32_t addr,
                         u8_t *payload,
                         const nsf_trx_attr_t attr,
                         const norsf_wip_info_t *wi) {
    u32_t start_c, now_c;

    /* `write en' flow here assumes that all NOR SPI-F comes with the same style. */
    if (attr.write_en == 1) {
        nsf_trx_attr_t attr_wren = {
            .cs = attr.cs,
            .cmd  = 0x06,
            .plen_b = 0,
            .write_en = 0,
            .dummy_ck = 0,
            .pdir  = norsf_ptx,
            .cmode = norsf_sio,
            .amode = norsf_sio,
            .pmode = norsf_sio,
        };

        norsf_single_cmd(-1, VZERO, attr_wren);
    }

    norsf_single_cmd(addr, payload, attr);

    if (wi) {
        OS_SCHEDULE_OUT();
        now_c = start_c = GET_CPU_CYCLE();
        while (wi->wip(attr.cs)) {
            if ((now_c - start_c) > wi->to_c) {
                return NORSFG2_T2_WIP_TIMEOUT;
            }
            OS_SCHEDULE_OUT();
            now_c = GET_CPU_CYCLE();
        }
    }

    return 0;
}

extern norsf_probe_t *LS_start_of_nor_spif_probe_func[];
extern norsf_probe_t *LS_end_of_nor_spif_probe_func;

SECTION_NOR_SPIF_GEN2_MISC
static u32_t norsf_log2(u32_t v) {
    u32_t _v, f;

    if (v == 0) return 0;

    _v = v;
    f = 0;

    while (_v) {
        f++;
        _v = _v >> 1;
    }

    return f - 1;
}

SECTION_NOR_SPIF_GEN2_MISC
static u32_t norsf_addr_on_flash(const void *addr) {
    const u32_t ucaddr = ((u32_t)addr) | 0x20000000;
    u32_t ret = (((ucaddr - 0xb4000000) ^ (ucaddr - 0xbf000000)) |
                 ((ucaddr - 0xbfc00000) ^ (ucaddr - 0xc0000000)));

    return (ret >> 31);
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t norsf_configure_mmio_read(norsf_g2_info_t *ni, u32_t mmio32_en) {
    u32_t id, sfsize_field;

    id = norsf_rdid(0);

    printf("%06X/MMIO", id);
    if (ni->size_per_chip_b == 0) {
        ni->size_per_chip_b = 1 << (id & 0xff);
    }

    sfsize_field = norsf_log2(ni->size_per_chip_b) - 17;

    if ((mmio32_en && (ni->addr_mode & norsf_4b_mode)) ||
        (ni->addr_mode & norsf_4b_cmd)) {
        sfsize_field -= 2;
        ni->is_mr4a = 1;
    } else {
        ni->is_mr4a = 0;
    }

    printf("%d-%x/Mode%c ", (16 << ni->is_mr4a), ni->addr_mode, NORSF_XREAD_EN?'X':'C');

    if (sfsize_field > 7) sfsize_field = 7;

    norsf_int_configure_mmio_read(ni, sfsize_field, ni->cmd_info->cread->a);
    IDCACHE_FLUSH();
    return 0;
}

SECTION_NOR_SPIF_GEN2_MISC
int norsf_chip_num_det(void) {
#if NORSF_CHIP_NUM > 1
    int cs0_id;

    cs0_id = norsf_rdid(0);

    if (cs0_id == norsf_rdid(1)) {
        return 2;
    } else {
        return 1;
    }
#else
    return 1;
#endif
}

SECTION_NOR_SPIF_GEN2_MISC
void norsf_detect(void) {
    norsf_probe_t **pf = LS_start_of_nor_spif_probe_func;
    void *flash_info = VZERO;

    NORSFG2_PROLOGUE_HINT();

    while (pf != &LS_end_of_nor_spif_probe_func) {
        flash_info = (void *)(*pf)();
        if (flash_info != VZERO) {
            inline_memcpy(&norsf_info, flash_info, sizeof(norsf_g2_info_t));
            break;
        }
        pf++;
    }

    norsf_info.num_chips = norsf_chip_num_det();

    if (norsf_addr_on_flash(norsf_info.arch_wip) ||

        norsf_addr_on_flash(norsf_proj_4b_addr_en) ||

        ((norsf_info.addr_mode & norsf_4b_mode) &&
         (norsf_addr_on_flash(norsf_info.am.mode.arch_en4b) ||
          norsf_addr_on_flash(norsf_info.am.mode.arch_ex4b))) ||

        (norsf_addr_on_flash(norsf_info.cmd_info->cread->xread_en) ||
         norsf_addr_on_flash(norsf_info.cmd_info->cread->xread_ex))) {
        puts("WW: arch_wip(), arch_en4b(), arch_ex4b(), xread_en(),"
             "and/or xread_ex() is on flash!\n");
    }

    if (((norsf_info.addr_mode & norsf_4b_mode) &&
         ((VZERO == norsf_info.am.mode.arch_en4b) ||
          (VZERO == norsf_info.am.mode.arch_ex4b))) ||

        ((norsf_info.addr_mode & norsf_4b_ear) &&
         (VZERO == norsf_info.am.ear.arch_wrear))) {
        puts("WW: 4B mode/ear selected but no arch_en/ex4b()/arch_wrear() functions defined!\n");
    }

    if (norsf_info.addr_mode & (norsf_4b_mode | norsf_4b_cmd)) {
        norsf_info.pio_addr_len = 3;
    } else {
        norsf_info.pio_addr_len = 2;
    }

    if ((norsf_info.addr_mode & norsf_4b_ear) ||
        ((NORSF_MMIO_4B_EN == 0) && (norsf_info.addr_mode & norsf_4b_mode))) {
        norsf_info.dyn_4b_addr_switch = 1;
    } else {
        norsf_info.dyn_4b_addr_switch = 0;
    }

    norsf_configure_mmio_read(&norsf_info, NORSF_MMIO_4B_EN);

    NORSFG2_EPILOGUE_HINT();

    return;
}

SECTION_NOR_SPIF_GEN2_MISC
static void show_progress(u32_t done, u32_t total, u32_t verbose) {
    if (verbose) {
        printf("%3d%%\b\b\b\b", done*100/total);
    }
    return;
}

/* Common Tier-III functions. */
/* !!!! norsf_cmn_arch_wip() MUST NOT run from flash!!!! */
SECTION_NOR_SPIF_GEN2_CORE
s32_t norsf_cmn_arch_wip(const u32_t cs) {
    const nsf_trx_attr_t attr = {
        .cs       = cs,
        .cmd      = 0x05,
        .plen_b   = 1,
        .write_en = 0,
        .dummy_ck = 0,
        .pdir     = norsf_prx,
        .cmode    = norsf_sio,
        .amode    = norsf_sio,
        .pmode    = norsf_sio,
    };
    const u8_t mask = 0x1;
    u8_t payload;

    norsf_compound_cmd(-1, &payload, attr, 0);

    return ((payload & mask) == mask);
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t norsf_cmn_arch_erase(const norsf_erase_cmd_t *cmd,
                           const u32_t cs,
                           const u32_t offset,
                           const norsf_wip_info_t *wi) {
    s32_t res;
    nsf_trx_attr_t attr = cmd->a;
    attr.cs = cs;

    res = norsf_compound_cmd(offset, VZERO, attr, wi);

    res = (res < 0)? res: cmd->sz_b;

    return res;
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t norsf_cmn_arch_prog(const norsf_g2_info_t *ni,
                          const u32_t cs,
                          const u32_t offset,
                          const u32_t len,
                          const void *buf,
                          const norsf_wip_info_t *wi) {
    s32_t res;
    nsf_trx_attr_t attr = ni->cmd_info->cprog_attr;
    attr.cs     = cs;
    attr.plen_b = len;

    res = norsf_compound_cmd(offset, (void *)buf, attr, wi);

    res = (res < 0)? res: len;

    return res;
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t norsf_cmn_arch_read(const norsf_g2_info_t *ni,
                          const u32_t cs,
                          const u32_t offset,
                          const u32_t len,
                          void *buf) {
    nsf_trx_attr_t attr = ni->cmd_info->cread->a;
    attr.cs = cs;
    attr.plen_b = len;

    norsf_compound_cmd(offset, buf, attr, VZERO);

    return len;
}

SECTION_NOR_SPIF_GEN2_MISC
s32_t norsf_cmn_4b_mode_en(const norsf_g2_info_t *ni, const u32_t cs) {
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
s32_t norsf_cmn_4b_mode_ex(const norsf_g2_info_t *ni, const u32_t cs) {
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
s32_t norsf_rdid(const u32_t cs) {
    u32_t id;
    nsf_trx_attr_t attr = {
        .cs       = cs,
        .cmd      = 0x9f,
        .plen_b   = 3,
        .write_en = 0,
        .dummy_ck = 0,
        .pdir     = norsf_prx,
        .cmode    = norsf_sio,
        .amode    = norsf_sio,
        .pmode    = norsf_sio,
    };

    norsf_compound_cmd(-1, (u8_t *)&id, attr, VZERO);
    return id >> ((4 - attr.plen_b) * 8);
}

/* Tier-IV functions. */
SECTION_NOR_SPIF_GEN2_MISC
static s32_t norsf_t4_epilogue(s32_t res,
                               const u32_t cur_offset,
                               const u32_t left_len,
                               const u32_t cmd_offset,
                               const u32_t accum_len,
                               const u32_t verbose) {
    if (res > 0) {
    if (verbose) {
            printf("100%% ~ %08x/%d B\n", accum_len + cmd_offset - 1, accum_len);
        }
        } else {
            puts("EE: ");
            switch (res) {
            case NORSFG2_T2_WIP_TIMEOUT:
                puts("WIP() timeout");
                break;
            case NORSFG2_E4_OFFSET_NALIGN:
                printf("offset %08x is not aligned", cur_offset);
                break;
            case NORSFG2_E4_LEN_NALIGN:
                printf("erase size: %d B is not aligned", left_len);
                break;
            case NORSFG2_E4_BAD_CMD_AND_OFFSET:
                printf("can't erase %d B due to non-uniform sector", left_len);
                break;
            case NORSFG2_T4_LEN_OVERFLOW:
                printf("destination %08x overflows", left_len + cmd_offset);
                break;
            default:
                printf("unknown error: %d", res);
                break;
            }
            puts("\n");
        }

    return (res < 0);
}

SECTION_NOR_SPIF_GEN2_MISC
static void norsf_addr_range_enable(const norsf_g2_info_t *ni,
                                    const u32_t cs,
                                    const u32_t next_offset,
                                    norsf_usable_addr_t *ua_range) {
    u32_t global_offset, bank_id;

    if (ni->dyn_4b_addr_switch) {
        global_offset = cs * ni->size_per_chip_b + next_offset;
        if ((global_offset >= ua_range->start) &&
            (global_offset < ua_range->end)) {
            return;
        }

        ua_range->start = cs * ni->size_per_chip_b;

        if (ni->addr_mode == norsf_4b_mode) {
            ni->am.mode.arch_en4b(ni, cs);

            ua_range->end   = ua_range->start + ni->size_per_chip_b;
        } else {
            /* norsf_4b_ear */
            bank_id = next_offset/MEGABYTE(16);

            ni->am.ear.arch_wrear(ni, cs, bank_id);

            ua_range->start += bank_id * MEGABYTE(16);
            ua_range->end   = ua_range->start + MEGABYTE(16);
        }
    }

    return;
}

SECTION_NOR_SPIF_GEN2_MISC
static void norsf_addr_range_restore(const norsf_g2_info_t *ni) {
    u32_t i;
    s32_t dyn_addr_mode = -1;

    if (ni->dyn_4b_addr_switch) {
        dyn_addr_mode = ni->addr_mode;
    }

    for (i=0; i<ni->num_chips; i++) {
        if (dyn_addr_mode == norsf_4b_mode) {
            ni->am.mode.arch_ex4b(ni, i);
        } else if (dyn_addr_mode == norsf_4b_ear) {
            ni->am.ear.arch_wrear(ni, i, 0);
        }
    }

    return;
}

SECTION_NOR_SPIF_GEN2_MISC
static s32_t norsf_erase_cmd_sel(const norsf_g2_info_t *ni,
                                 const u32_t offset,
                                 const u32_t len) {
    u32_t i = 0;
    int res = NORSFG2_E4_UNKNOWN, offset_lmt;
    const norsf_erase_cmd_t *cmds = ni->cmd_info->cerase;

    for (i=0; i<ni->cmd_info->cerase_cmd_num; i++) {
        offset_lmt = (cmds[i].offset_lmt == 0 ? ni->size_per_chip_b : cmds[i].offset_lmt);
        if ((((s32_t)offset) - offset_lmt) & ni->size_per_chip_b) {
            if ((offset % cmds[i].sz_b) == 0) {
                res = i;
                if (len >= cmds[i].sz_b) {
                    break;
                }
            } else {
                res = NORSFG2_E4_OFFSET_NALIGN;
            }
        } else {
            /* res = NORSFG2_E4_BAD_CMD_AND_OFFSET; */
        }
    }

    return res;
}

SECTION_NOR_SPIF_GEN2_MISC
int norsf_erase(const norsf_g2_info_t *ni,
                const u32_t offset,
                const u32_t len,
                const u32_t is_strict_len,
                const u32_t verbose) {
    const u32_t nsf_sz_b = ni->size_per_chip_b;
    const norsf_erase_cmd_t *cmds = ni->cmd_info->cerase;
    norsf_wip_info_t wi;
    s32_t done_b = 0, res = 0;
    u32_t next_offset = offset % nsf_sz_b;
    u32_t cs = offset / nsf_sz_b;
    norsf_usable_addr_t ua_handler = {0};

    if ((offset + len) > (nsf_sz_b * ni->num_chips)) {
        res = NORSFG2_T4_LEN_OVERFLOW;
        goto e4_end;
    }

    if (verbose) {
        printf(NORSFG2_ACTION_PREFIX "Erasing %d B from %08x... ", len, offset);
    }

    wi.wip = ni->arch_wip;

    while (done_b < len) {
        show_progress(done_b, len, verbose);
        res = norsf_erase_cmd_sel(ni, next_offset, len - done_b);
        if (res < 0) {
            break;
        } else {
            if ((cmds[res].sz_b > len - done_b) && (is_strict_len)) {
                res = NORSFG2_E4_LEN_NALIGN;
                break;
            } else {
                /* 0 makes timeout become -1, i.e., 0xffff_ffff. */
                wi.to_c = cmds[res].to_us * GET_CPU_MHZ() - 1;

                norsf_addr_range_enable(ni, cs, next_offset, &ua_handler);

                res = ni->arch_erase(&cmds[res],
                                     cs,
                                     next_offset,
                                     &wi);
            }
        }

        POS_ADVANCE(cs, next_offset, nsf_sz_b, done_b, res);
    }

 t4_exception:
    norsf_addr_range_restore(ni);

    IDCACHE_FLUSH_ADDR((SFNOR_BASE_ADDR+offset),len);
 e4_end:
    return norsf_t4_epilogue(res, next_offset, len - done_b, offset, done_b, verbose);
}

SECTION_NOR_SPIF_GEN2_MISC
int norsf_prog(const norsf_g2_info_t *ni,
               const u32_t offset,
               const u32_t len,
               const void *buf,
               const u32_t verbose) {
    const u32_t nsf_sz_b = ni->size_per_chip_b;
    const norsf_cmd_info_t *cmd = ni->cmd_info;
    norsf_wip_info_t wi;
    u16_t op_unit = ni->cmd_info->cprog_lim_b;
    s32_t done_b = 0, res = 0;
    u32_t next_offset = offset % nsf_sz_b;
    u32_t cs = offset / nsf_sz_b;
    u32_t op_len;
    void *temp_buf = VZERO;
    void **buf_ptr;
    norsf_usable_addr_t ua_handler = {0};

#if (NORSFG2_PP_VERIFY_EN == 1)
    u32_t debug_len = len;
    u8_t *debug_src = (u8_t *)buf;
    u8_t *debug_dst = (u8_t *)(NORSF_CFLASH_BASE + offset);
#endif

    if ((offset + len) > (nsf_sz_b * ni->num_chips)) {
        res = NORSFG2_T4_LEN_OVERFLOW;
        goto p4_end;
    }

    buf_ptr = (void **)&buf;

    /* 0 makes timeout become -1, i.e., 0xffff_ffff. */
    wi.to_c = cmd->cprog_to_us * GET_CPU_MHZ() - 1;
    wi.wip = ni->arch_wip;

    op_unit = NORSF_MIN(op_unit, NORSF_WBUF_LIM_B);

    if (norsf_addr_on_flash(buf)) {
        temp_buf = alloca(op_unit);
        buf_ptr = &temp_buf;
    }

    if (verbose) {
        printf(NORSFG2_ACTION_PREFIX "Writing %d B from %p to %08x... ", len, buf, offset);
    }

    while (done_b < len) {
        op_len = NORSF_MIN((len - done_b), (op_unit - (next_offset % op_unit)));
        if (temp_buf) {
            inline_memcpy(temp_buf, buf, op_len);
        }
        show_progress(done_b, len, verbose);

        norsf_addr_range_enable(ni, cs, next_offset, &ua_handler);

        res = ni->arch_prog(ni,
                            cs,
                            next_offset,
                            op_len,
                            *buf_ptr,
                            &wi);

        POS_ADVANCE(cs, next_offset, nsf_sz_b, done_b, res);
        buf += res;
    }

 t4_exception:
    norsf_addr_range_restore(ni);

    IDCACHE_FLUSH_ADDR((SFNOR_BASE_ADDR+offset),len);

#if (NORSFG2_PP_VERIFY_EN == 1)
    norsf_int_memcmp(debug_src, debug_dst, debug_len);
#endif

 p4_end:
    return norsf_t4_epilogue(res, next_offset, len - done_b, offset, done_b, verbose);
}

SECTION_NOR_SPIF_GEN2_MISC
int norsf_read(const norsf_g2_info_t *ni,
               const u32_t offset,
               const u32_t len,
               void *buf,
               const u32_t verbose) {
    const nsf_trx_attr_t dummy_attr = { .plen_b = -1 };
    const u32_t op_unit = dummy_attr.plen_b;
    const u32_t nsf_sz_b = ni->size_per_chip_b;
    u32_t cs = offset / nsf_sz_b;
    u32_t next_offset = offset % nsf_sz_b;
    u32_t op_len;
    s32_t done_b = 0, res = 0;
    norsf_usable_addr_t ua_handler = {0};

    if ((offset + len) > (nsf_sz_b * ni->num_chips)) {
        res = NORSFG2_T4_LEN_OVERFLOW;
        goto r4_end;
    }

    if (verbose) {
        printf(NORSFG2_ACTION_PREFIX "Reading %d B to %p from %08x... ", len, buf, offset);
    }

    if ((offset + len) <= MEGABYTE(16)) {
        inline_memcpy(buf, (void *)(NORSF_CFLASH_BASE + offset), len);
        res = len;
        done_b = len;
        goto r4_end;
    }

    while (done_b < len) {
        op_len = NORSF_MIN(NORSF_MIN((len - done_b), op_unit), (nsf_sz_b - next_offset));
        show_progress(done_b, len, verbose);

        norsf_addr_range_enable(ni, cs, next_offset, &ua_handler);

        res = ni->arch_read(ni, cs, next_offset, op_len, buf);

        POS_ADVANCE(cs, next_offset, nsf_sz_b, done_b, res);
        buf += res;
    }

 t4_exception:
    norsf_addr_range_restore(ni);

 r4_end:
    return norsf_t4_epilogue(res, next_offset, len - done_b, offset, done_b, verbose);
}
