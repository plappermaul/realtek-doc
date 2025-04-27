	/* basic info. */
	.dram_type    = 2,
	.bankcnt      = 4,
	.buswidth     = 16,
	.max_row_addr = 12,
	.max_col_addr = 9,

	/* Affected by size */
	.min_trfc_ns  = 150,  //105

	/* reference: grade (E.g., DDR3-800E, JEDEC79-3F/p.157 ~.) */
	.min_tck_ps   = 2500, /* Min. clock length in ps. E.g., DDR3-800D needs 2.5 ~ 3.3ns. */
	.max_tck_ps   = 25000, /* Max. clock length. */
	.min_trcd_ns  = 25,  //14
	.min_trp_ns   = 25,  //14
	.min_tras_ns  = 75,  //45
	.cl_ck        = 5,
	.cwl_ck       = 4,

	/* reference: speed (E.g., DDR3-800, JEDEC79-3F/p.169 ~.) */
	.min_twr_ns   = 50,  //15
	.min_trtp_ns  = 25,  //8
	.min_twtr_ns  = 50,  //8
	.min_trrd_ns  = 25,  //10
	.min_tfaw_ns  = 75,  //45
	.max_trefi_ns = 7800,
