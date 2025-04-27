#ifndef NOR_SPIF_CORE_H
#define NOR_SPIF_CORE_H

#include <soc.h>

typedef enum {
	norsf_sio = 0b00,
	norsf_dio = 0b01,
	norsf_qio = 0b10,
} spif_io_mode_t;

typedef enum {
	norsf_ptx = 0b0,
	norsf_prx = 0b1,
} spif_payload_dir_t;

typedef struct {
	u8_t cmd;
	u32_t addr;
} nsf_write_phase_t;

typedef struct {
	u32_t plen_b:16;
	u32_t write_en:1;
	u32_t wmode:2;  // address io
	u32_t wlen_b:3;
	u32_t dummy_ck:4;
	u32_t pmode:2;  // data io
	u32_t pdir:1;
} nsf_trx_attr_t;

typedef struct {
	u8_t  num_chips;
	u8_t  addr_len_b;
	u16_t wr_boundary_b;
	u16_t erase_unit_b;
	u32_t size_per_chip_b;
	u32_t id;
	nor_spi_erase *_nor_spi_erase;
	nor_spi_read  *_nor_spi_read;
	nor_spi_write *_nor_spi_write;
} plr_nor_spi_info_g2_t;

typedef void (nor_spi_wip_block)(u32_t cid);
typedef plr_nor_spi_info_g2_t *(nor_spi_probe_t)(void);

u32_t spif_trx(const u32_t cs,
               const nsf_write_phase_t *wp,
               u8_t *payload,
               nsf_trx_attr_t attr,
               nor_spi_wip_block *wip);

void nor_spif_detect(void);

u32_t auto_sz_factor(u32_t);

#ifndef SECTION_NOR_SPIF_PROBE_FUNC
#define SECTION_NOR_SPIF_PROBE_FUNC
#define REG_NOR_SPIF_PROBE_FUNC(fn)
#else
#define REG_NOR_SPIF_PROBE_FUNC(fn) nor_spi_probe_t* __nspf_ ## ## fn ## _ \
	SECTION_NOR_SPIF_PROBE_FUNC = (nor_spi_probe_t*) fn
#endif

#ifndef SECTION_NOR_SPIF_GEN2_CORE
#define SECTION_NOR_SPIF_GEN2_CORE
#endif

#ifndef SECTION_NOR_SPIF_GEN2_COREDATA
#define SECTION_NOR_SPIF_GEN2_COREDATA
#endif

#ifndef SECTION_NOR_SPIF_GEN2_MISC
#define SECTION_NOR_SPIF_GEN2_MISC
#endif

#endif
