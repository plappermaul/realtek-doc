#ifndef __BIFFEN_H__
#define __BIFFEN_H__ 1

#include <sihnah.h>
#include <reg_skc35.h>

#define BFN_MAGIC (0xba)

#define BFN_ODATA_SZ (512)
#define BFN_SYNDROME_SZ (20)
#define BFN_TAG_SZ (6)

#define BFN_DUMMY_SZ (BFN_SYNDROME_SZ + BFN_TAG_SZ)
#define BFN_IDATA_SZ (BFN_ODATA_SZ - BFN_DUMMY_SZ)

typedef struct __attribute__ ((packed)) {
	uint64_t fip_idx:4;
	uint64_t fip_num:4;
	uint64_t fip_sz_64kb:8;
	uint64_t customized_entry:24;
	uint64_t customized_sz_b:24;
} biffen_fip_flags_t;

/*
	type == 0: attr: the number of biffen that form a binary file
	this allows image (e.g., FIP) up to (2^16*512) = 32MByte
	type == 1: attr: index of chunk
		Since the 1st biffen must be type 0, type 1 attr. always starts from 1.
	type == 2: end of binary file; attr: num. of biffen - 1
 */
#define BFN_TAG_HEAD (0)
#define BFN_TAG_IDX (1)
#define BFN_TAG_TAIL (2)
typedef struct __attribute__ ((packed)) {
	uint8_t magic;

	uint8_t prev_crc:5;
	uint8_t type:3;

	uint16_t attr;

	uint16_t crc16;
} bfn_tag_t;
skc35_assert(sizeof(bfn_tag_t) == 6);

typedef struct { //512 + 6 = 518 bytes
	uint8_t data[BFN_IDATA_SZ];
	uint8_t dummy[BFN_DUMMY_SZ];
	bfn_tag_t tag;
} bfn_raw_t;

/* Considering in-band tag to convert image offset to physical offset. */
__attribute__ ((unused))
static uint32_t bfn_io_to_po(uint32_t _image_offset) {
	return ((_image_offset / BFN_IDATA_SZ) * BFN_ODATA_SZ) + (_image_offset % BFN_IDATA_SZ);
}

typedef struct { //512 bytes
	uint8_t data[BFN_IDATA_SZ];
	bfn_tag_t tag;
	uint8_t syndrome[BFN_SYNDROME_SZ];
} bfn_medium_t;

typedef struct {
	uint32_t total_chunks;
	uint32_t page_idx_start;
} bfn_info_t;

/* crc16_ccitt() is from: https://github.com/Thuffir/xmodem */
__attribute__ ((unused))
static unsigned short crc16_ccitt(
  /* Pointer to the byte buffer */
  const unsigned char *buffer,
  /* length of the byte buffer */
  int length)
{
	unsigned short crc16 = 0;

	while (length != 0) {
		crc16 = (unsigned char)(crc16 >> 8) | (crc16 << 8);
		crc16 ^= *buffer;
		crc16 ^= (unsigned char)(crc16 & 0xff) >> 4;
		crc16 ^= (crc16 << 8) << 4;
		crc16 ^= ((crc16 & 0xff) << 4) << 1;
		buffer++;
		length--;
	}

	return crc16;
}

//uint32_t *biffen_pnand_probe(pnand_info_t *info, bfn_info_t *binfo);

enum {
	BFN_RD_SEQ_CMD_ADDR_CMD = 0,
	BFN_RD_SEQ_CMD_ADDR,
	BFN_RD_SEQ_NUM,
};

static const char *rd_seq_msg[] __attribute__ ((unused)) = {
	"CMD-ADDR-CMD",
	"CMD-ADDR",
};

int biffen_verify_chunk(bfn_medium_t *medium, int seq);

#endif
