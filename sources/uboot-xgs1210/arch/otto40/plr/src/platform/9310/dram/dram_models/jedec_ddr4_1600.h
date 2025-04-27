    /* DCR */
    .dram_type      = DDR_TYPE_DDR4, 
    /* MISC */
    .bankcnt        = 8,        // 8 banks
    .rowcnt         = 0,        // auto size detect
    .pagesize       = 2048,     // 256B << 3 = 2KB
    .burst          = 8,        // burst = 4 or 8
    /* DRR */
    .ref_num        = 1,
    .min_tref_ns    = 7800,     //tREF,  unit is "ns"
    .min_trfc_ns    = 160,      //tRFC,  unit is "ns"
    /* TPR0 */
    .min_tzqcs_tck  = 128,      //tZQCS, unit is "tCK"
    .min_tcke_ns    = 30,       //tCKE,  unit is "ns", max(3nck, 5ns), need 30ns
    .min_trtp_ns    =  8,       //tRTP,  unit is "ns", max (4nCK, 7.5ns), follow twr/2
    .min_twr_ns     = 16,       //tWR,   unit is "ns", DDR4, MR0, twr v.s trtp
    .min_tras_ns    = 40,       //tRAS,  unit is "ns"
    .min_trp_ns     = 15,       //tRP,   unit is "ns"
    /* TPR1 */
    .min_tfaw_ns    = 40,       //tFAW,  unit is "ns"
    .min_trtw_tck   = 8,        //tRTW,  unit is "tCK", trtw=rd_lat+6-wr_lat
    .min_twtr_ns    = 40,       //tWTR,  unit is "ns", twtr=4nck+wr_lat(8ncK)+twtr(spec)
    .min_tccd_tck   = 4,        //tCCD,  unit is "tCK", tCCD_S?
    .min_trcd_ns    = 15,       //tRCD,  unit is "ns"
    .min_trc_ns     = 50,       //tRC,   unit is "ns"
    .min_trrd_ns    = 10,       //tRRD,  unit is "ns"
    /* TPR2 */
    .min_tmrd_tck   = 8,        //tMRD,  unit is "tCK"
    /* TPR3 (for DDR4) */
    .min_tccd_r_tck = 5,        //tCCD_R, unit is "tCK", tCCD_L?
    .min_twtr_s_tck = 6,        //tWTR_S, unit is "tCK"
    .min_tccd_s_tck = 4,        //tCCD_S, unit is "tCK"
    .min_dpin_cmd_lat = 0,      //DPIN_CMD_LAT, tCAL?, unit is "tCK"
    /* MRINFO */
    .add_lat        = 0,
    .rd_lat         = 12,       // cas latency
    .wr_lat         = 10,       // cas write latency 9~12, 14, 16, 18
    /* MISC */
    .dq32_en        = 2,        // half DQ, 0: DQ16, 1: DQ32, 2:  from strapped pin
    .zqc_en         = 0,
    .tphy_rdata     = 3,
    .tphy_wlat      = 0,
    .tphy_wdata     = 3,        // cwl = 2*tphy_wdata + 2*data_pre + afifo
    .stc_odt_en     = 0,
    .stc_cke_en     = 0,
    .ref_dis        = 0,
    .rttnom         = 120,      //34
    .rttwr          = 80,       // 0: disabled, 1: Hi-Z, 80, 120, 240ohm
    .odic           = 34,       // 34ohm or 48ohm
    .twpre          = 0,
    .trpre          = 0,        
