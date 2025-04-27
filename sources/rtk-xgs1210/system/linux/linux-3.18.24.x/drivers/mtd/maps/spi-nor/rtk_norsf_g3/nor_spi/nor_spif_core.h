#ifndef NOR_SPIF_CORE_H
#define NOR_SPIF_CORE_H

#include <soc.h>

#ifndef NORSF_MMIO_4B_EN
#define NORSF_MMIO_4B_EN (0)
#endif

#define NORSFG2_T2_WIP_TIMEOUT   -12
#define NORSFG2_E4_OFFSET_NALIGN -31
#define NORSFG2_E4_LEN_NALIGN    -32
#define NORSFG2_E4_BAD_CMD_AND_OFFSET -33
#define NORSFG2_E4_UNKNOWN       -34
#define NORSFG2_T4_LEN_OVERFLOW  -35

struct norsf_g2_info_s;
typedef struct norsf_g2_info_s norsf_g2_info_t;

typedef enum {
    norsf_sio = 0x0,
    norsf_dio = 0x1,
    norsf_qio = 0x2,
} spif_io_mode_t;

typedef enum {
    norsf_ptx = 0x0,
    norsf_prx = 0x1,
} spif_payload_dir_t;

typedef enum {
    norsf_3b_addr = 0x1,
    norsf_4b_mode = 0x2,
    norsf_4b_cmd  = 0x4,
    norsf_4b_ear  = 0x8,
} spif_addr_mode_t;

typedef struct {
    u32_t cs:2;
    u32_t cmd:8;
    u32_t plen_b:10;
    u32_t write_en:1;
    u32_t dummy_ck:4;
    u32_t cmode:2;  // cmd io
    u32_t amode:2;  // address io
    u32_t pmode:2;  // data io
    u32_t pdir:1;
} nsf_trx_attr_t;

typedef struct {
    nsf_trx_attr_t a;
    u32_t to_us;
    u32_t sz_b;
    s32_t offset_lmt; /* For non-uniform sector devices,
                         +: from the begining; -: from the tail; 0: no limit. I.e.,
                         first 128KB = +0x20000; the last 128KB: = -0x20000. */
} norsf_erase_cmd_t;

typedef int (norsf_wip_t)(const u32_t);

typedef struct {
    u32_t to_c;
    norsf_wip_t *wip;
} norsf_wip_info_t;

typedef int (norsf_erase_t)(const norsf_erase_cmd_t *, const u32_t, const u32_t, const norsf_wip_info_t *);
typedef int (norsf_prog_t) (const norsf_g2_info_t *,   const u32_t, const u32_t, const u32_t, const void *, const norsf_wip_info_t *);
typedef int (norsf_read_t) (const norsf_g2_info_t *,   const u32_t, const u32_t, const u32_t, void *);
typedef int (norsf_4b_mode_en_t)(const norsf_g2_info_t *, const u32_t);
typedef int (norsf_4b_mode_ex_t)(const norsf_g2_info_t *, const u32_t);
typedef int (norsf_4b_ear_set_t)(const norsf_g2_info_t *, const u32_t, const u8_t);

typedef int (norsf_xread_en_t)(const norsf_g2_info_t *, const u32_t);
typedef int (norsf_xread_ex_t)(const norsf_g2_info_t *, const u32_t);

typedef struct {
    nsf_trx_attr_t a;
    norsf_xread_en_t *xread_en;
    norsf_xread_ex_t *xread_ex;
} norsf_read_cmd_t;

typedef struct {
    norsf_erase_cmd_t *cerase;

    nsf_trx_attr_t cprog_attr;
    u32_t          cprog_to_us;
    u16_t          cprog_lim_b;

    u16_t          cerase_cmd_num;

    norsf_read_cmd_t *cread;
} norsf_cmd_info_t;

typedef struct {
    norsf_4b_mode_en_t *arch_en4b;
    norsf_4b_mode_ex_t *arch_ex4b;
} norsf_4b_mode_func_t;

typedef struct {
    norsf_4b_ear_set_t *arch_wrear;
} norsf_4b_ear_func_t;

typedef struct {
    u32_t start;
    u32_t end;
} norsf_usable_addr_t;

struct norsf_g2_info_s {
    u8_t  num_chips;

    u8_t  addr_mode:4;
    u8_t  is_mr4a:1;
    u8_t  dyn_4b_addr_switch:1;
    u8_t  pio_addr_len:2;

    u32_t sec_sz_b;
    u32_t size_per_chip_b;

    norsf_erase_t *arch_erase;
    norsf_prog_t  *arch_prog;
    norsf_read_t  *arch_read;
    norsf_wip_t   *arch_wip;
    union {
        norsf_4b_mode_func_t mode;
        norsf_4b_ear_func_t  ear;
    } am;

    norsf_cmd_info_t *cmd_info;
};

typedef norsf_g2_info_t *(norsf_probe_t)(void);

void norsf_detect(void);
s32_t norsf_rdid(const u32_t);

s32_t norsf_compound_cmd  (const u32_t addr, u8_t *payload, const nsf_trx_attr_t attr, const norsf_wip_info_t *wi);

void norsf_proj_4b_addr_en(u32_t en);

s32_t norsf_cmn_arch_wip  (const u32_t cs);
s32_t norsf_cmn_arch_erase(const norsf_erase_cmd_t *cmd, const u32_t cs, const u32_t offset, const norsf_wip_info_t *wi);
s32_t norsf_cmn_arch_prog (const norsf_g2_info_t *ni,    const u32_t cs, const u32_t offset, const u32_t len,           const void *buf, const norsf_wip_info_t *wi);
s32_t norsf_cmn_arch_read (const norsf_g2_info_t *ni,    const u32_t cs, const u32_t offset, const u32_t len,                 void *buf);
s32_t norsf_cmn_4b_mode_en(const norsf_g2_info_t *ni,    const u32_t cs);
s32_t norsf_cmn_4b_mode_ex(const norsf_g2_info_t *ni,    const u32_t cs);

int norsf_erase(const norsf_g2_info_t *ni, const u32_t offset, const u32_t len, const u32_t strict_len, const u32_t verbose);
int norsf_prog (const norsf_g2_info_t *ni, const u32_t offset, const u32_t len,        const void *buf, const u32_t verbose);
int norsf_read (const norsf_g2_info_t *ni, const u32_t offset, const u32_t len,              void *buf, const u32_t verbose);

#ifndef SECTION_NOR_SPIF_PROBE_FUNC
#define SECTION_NOR_SPIF_PROBE_FUNC __attribute__ ((section (".nor_spif_probe_func")))
#endif

#define REG_NOR_SPIF_PROBE_FUNC(fn) norsf_probe_t* __nspf_ ## fn ## _ \
    SECTION_NOR_SPIF_PROBE_FUNC = (norsf_probe_t*) fn

#ifndef SECTION_NOR_SPIF_GEN2_CORE
#define SECTION_NOR_SPIF_GEN2_CORE __attribute__ ((section (".text.cacheline_alignment"), noinline, aligned(32)))
#endif

#ifndef SECTION_NOR_SPIF_GEN2_COREDATA
#define SECTION_NOR_SPIF_GEN2_COREDATA
#endif

#ifndef SECTION_NOR_SPIF_GEN2_MISC
#define SECTION_NOR_SPIF_GEN2_MISC
#endif

#ifndef SECTION_NOR_SPIF_GEN2_PARAM
#define SECTION_NOR_SPIF_GEN2_PARAM
#endif

extern norsf_g2_info_t norsf_info;
extern norsf_cmd_info_t cmn_cmd_info;
extern norsf_erase_cmd_t cmn_erase_cmds[];

#define NORSF_READ(offset, len, buf, verbose) \
    norsf_read(&norsf_info, offset, len, buf, verbose)

#define NORSF_PROG(offset, len, buf, verbose) \
    norsf_prog(&norsf_info, offset, len, buf, verbose)

#define NORSF_ERASE(offset, len, verbose, is_strict_len) \
    norsf_erase(&norsf_info, offset, len, is_strict_len, verbose)

/* For development. */
extern u32_t exp_norsf_sel_addr_type;

#endif
