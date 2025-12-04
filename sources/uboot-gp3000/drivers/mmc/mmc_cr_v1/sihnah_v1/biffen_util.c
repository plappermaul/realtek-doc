#include <sihnah.h>
#include <biffen.h>
#include <ecc.h>

static int _biffen_is_head_chunk(bfn_medium_t *med, int seq) {
	return !!((med->tag.type == BFN_TAG_HEAD) && (seq == 0));
}

static int _biffen_is_index_n_tail_chunk(bfn_medium_t *med, int seq) {
	if (
		(med->tag.attr == seq) &&
		((med->tag.type == BFN_TAG_IDX) || (med->tag.type == BFN_TAG_TAIL))) {
		return 1;
	} else {
		return 0;
	}
}

int biffen_verify_chunk(bfn_medium_t *medium, int seq) {
	int err;
	SIHNAH_DMA_ALLOC(_raw, sizeof(bfn_raw_t), SIHNAH_CACHE_LINE_SIZE);
	bfn_raw_t *raw = (bfn_raw_t *)_raw;
	short crc_res;
	const sihnah_ecc_t ecc_op = {
		.algo = ECC_BCH12, /* biffen accepts only BCH12 */
		.act = ECC_DECODE,
	};

	/* append dummy to data then apply ecc decode */
	sihnah_memset(raw, 0, sizeof(bfn_raw_t));
	sihnah_memcpy(raw->data, medium->data, BFN_IDATA_SZ);

	err = ecc_action(&ecc_op, raw->data, (uint8_t *)&medium->tag, medium->syndrome);
	if (err < 0) {
		VERBOSE("ECC decode: FUBAR\n");
		return 2;
	} else if (err) {
		VERBOSE("ECC decode: recovered from %d error(s)\n", err);
		sihnah_memcpy(medium->data, raw->data, BFN_IDATA_SZ);
	}

	/* check magic number */
	if (medium->tag.magic != 0xba) {
		VERBOSE("ECC decode: bad magic\n");
		return 3;
	}

	/* check CRC16 */
	crc_res = crc16_ccitt(medium->data, BFN_IDATA_SZ);
	if (*((short *)&medium->tag.crc16) != crc_res) {
		VERBOSE("ECC decode: bad crc16: %04x vs %04x\n", *((short *)&medium->tag.crc16), crc_res);
		return 3;
	}

	if (_biffen_is_head_chunk(medium, seq) || _biffen_is_index_n_tail_chunk(medium, seq)) {
		return 0;
	} else {
		return 1;
	}
}
