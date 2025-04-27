	/* basic info. */
	.dram_type    = 3,
	.bankcnt      = 8,
	.buswidth     = 16,
	.max_row_addr = 13,	// 14
	.max_col_addr = 9,

	/* Affected by size */
	.min_trfc_ns  = 260, //330, //110,

	/* reference: grade (E.g., DDR3-800E, JEDEC79-3F/p.157 ~.) */
	.min_tck_ps   = 1600, /* Min. clock length in ps. E.g., DDR3-800D needs 2.5 ~ 3.3ns. */
	.max_tck_ps   = 25000, /* Max. clock length. */
	.min_trcd_ns  = 14, 
	.min_trp_ns   = 14, 
	.min_tras_ns  = 40,
	.cl_ck        = 6,
	.cwl_ck       = 5,

	/* reference: speed (E.g., DDR3-800, JEDEC79-3F/p.169 ~.) */
	.min_twr_ns   = 15,	//15
	.min_trtp_ns  = 8,
	.min_twtr_ns  = 8,
	.min_trrd_ns  = 8,
	.min_tfaw_ns  = 45, //50,
	.max_trefi_ns = 7800,
