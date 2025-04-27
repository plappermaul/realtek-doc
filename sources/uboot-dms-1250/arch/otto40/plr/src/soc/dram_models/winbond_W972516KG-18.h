	/* basic info. */
	.dram_type    = 2,
	.bankcnt      = 4,
	.buswidth     = 16,
	.max_row_addr = 12,
	.max_col_addr = 8,

	/* Affected by size */
	.min_trfc_ns  = 75,

	/* reference: grade (.e.g., DDR3-800E, JEDEC79-3F/p.157 ~.) */
	.min_tck_ps  = 1875, /* Min. clock length in ps. E.g., DDR3-800D needs 2.5 ~ 3.3ns. */
	.max_tck_ps  = 7500, /* Max. clock length. */
	.min_trcd_ns = 14,
	.min_trp_ns  = 14,
	.min_tras_ns = 45,
	.cl_ck       = 7,
	.cwl_ck      = 6,

	/* reference: speed (.e.g., DDR3-800, JEDEC79-3F/p.169 ~.) */
	.min_twr_ns   = 15,
	.min_trtp_ns  = 8,
	.min_twtr_ns  = 8,
	.min_trrd_ns  = 8,
	.min_tfaw_ns  = 35,
	.max_trefi_ns = 3900,
