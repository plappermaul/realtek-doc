/* for freqency DDR-1333 ~ DDR-1600 */
   /* DCR */
    .dram_type      = DDR_TYPE_DDR3, 
    /* MISC */
    .bankcnt        = 8,        // 8 banks
    .rowcnt         = 0,        // 0: auto size detect
    .pagesize       = 2048,     // 256B << 3 = 2KB
    .burst          = 8,        // burst = 4 or 8
    /* DRR */
    .ref_num        = 1,
    .min_tref_ns    = 7800,     //tREF,  unit is "ns"
    .min_trfc_ns    = 90,      //tRFC,  unit is "ns"
    /* TPR0 */
    .min_tzqcs_tck  = 0,        //tZQCS, unit is "tCK"
    .min_tcke_ns    = 10,       //tCKE,  unit is "ns", max(3nCK, 7.5ns), need 30ns
    .min_trtp_ns    = 10,       //tRTP,  unit is "ns", max(4nCK, 7.5ns)
    .min_twr_ns     = 15,       //tWR,   unit is "ns"
    .min_tras_ns    = 50,	//tRAS,  unit is "ns"
    .min_trp_ns     = 15,       //tRP,   unit is "ns"
    /* TPR1 */
    .min_tfaw_ns    = 60,       //tFAW,  unit is "ns"
    .min_trtw_tck   = 9,        //tRTW,  unit is "tCK", trtw=rd_lat+6-wr_lat
    .min_twtr_ns    = 10,       //tWTR,  unit is "ns", twtr=4+wr_lat+twtr(spec)
    .min_tccd_tck   = 4,        //tCCD,  unit is "tCK"
    .min_trcd_ns    = 15,       //tRCD,  unit is "ns"
    .min_trc_ns     = 55,       //tRC,   unit is "ns"
    .min_trrd_ns    = 15,       //tRRD,  unit is "ns"
    /* TPR2 */
    .min_tmrd_tck   = 12,       //tMRD,  unit is "tCK"
    /* TPR3 (for DDR4) */
    .min_tccd_r_tck = 0,        //tCCD_R, unit is "tCK"
    .min_twtr_s_tck = 0,        //tWTR_S, unit is "tCK"
    .min_tccd_s_tck = 0,        //tCCD_S, unit is "tCK"
    .min_dpin_cmd_lat = 0,      //DPIN_CMD_LAT, tCAL?, unit is "tCK"
    /* MRINFO */
    .add_lat        = 0,
    .rd_lat         = 11,       // cas latency
    .wr_lat         = 8,
    /* MISC */
    .dq32_en        = 2,    // half DQ, 0: DQ16, 1: DQ32, 2:  from strapped pin
    .zqc_en         = 0,
    .tphy_rdata     = 2,    // smallest value is 1
    .tphy_wlat      = 0,
    .tphy_wdata     = 2,    // smallest value is 1
    .stc_odt_en     = 0,
    .stc_cke_en     = 0,
    .ref_dis        = 0,
    .rttnom         = 60,
    .rttwr          = 60,   // 0: disabled, 60ohm, 120ohm
    .odic           = 40,   // 40ohm or 34ohm
    .twpre          = 0,
    .trpre          = 0,