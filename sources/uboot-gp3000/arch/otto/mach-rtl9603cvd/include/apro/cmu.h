#ifndef __APRO_CMU_H__
#define __APRO_CMU_H__

#define DISABLE_CMU     0
#define ENABLE_CMU_FIX_MODE      1
#define ENABLE_CMU_DYNAMIC_MODE  2


typedef union {
	struct {
		unsigned int busy:1;       // 1
        unsigned int mbz_30_7:24;  // 0
		unsigned int freq_div:3;   // 7
		unsigned int mbz_3_2:2;    // 0
		unsigned int cmu_mode:2;   // 0
	} f;
	unsigned int v;
} OC0_CMUGCR_T;
#define OC0_CMUGCRrv (*((regval)0xb8000380))
#define OC0_CMUGCRdv (0x80000070) 	/* Set all divisor to 1 and CMU disabled by default. */
#define RMOD_OC0_CMUGCR(...) rset(OC0_CMUGCR, OC0_CMUGCRrv, __VA_ARGS__)
#define RIZS_OC0_CMUGCR(...) rset(OC0_CMUGCR, 0, __VA_ARGS__)
#define RFLD_OC0_CMUGCR(fld) (*((const volatile OC0_CMUGCR_T *)0xb8000380)).f.fld


typedef union {
	struct {
		unsigned int mbz_31_3:29; // 0
		unsigned int dly_base:3;  // 0
	} f;
	unsigned int v;
} OC0_CMUSDCR_T;
#define OC0_CMUSDCRrv (*((regval)0xb8000384))
#define OC0_CMUSDCRdv (0x00000000)
#define RMOD_OC0_CMUSDCR(...) rset(OC0_CMUSDCR, OC0_CMUSDCRrv, __VA_ARGS__)
#define RFLD_OC0_CMUSDCR(fld) (*((const volatile OC0_CMUSDCR_T *)0xb8000384)).f.fld

typedef union {
	struct {
		unsigned int auto_bz:1;     //0
		unsigned int mbz_30_29:2;  //0
		unsigned int int_cxn_type:1;     //0
		unsigned int mbz_27_20:8;  //0
		unsigned int se_spif_wk:1;  //0
		unsigned int se_spif_slp:1; //0
		unsigned int se_spif:1;     //0
		unsigned int se_spif_hs:1;  //0
		unsigned int se_sram_rom_wk:1;//0
		unsigned int se_sram_rom_slp:1;//0
		unsigned int se_sram_rom:1;//0
		unsigned int se_sram_rom_hs:1;//0
		unsigned int se_dram_wk:1;  //0
		unsigned int se_dram_slp:1; //0
		unsigned int se_dram:1;     //0
		unsigned int se_dram_hs:1;  //0
		unsigned int mbz_7_4:4;     //0
		unsigned int dly_mul:4;     //0
	} f;
	unsigned int v;
} OC0_CMUCR_T;
#define OC0_CMUCRrv (*((regval)0xb8000388))
#define OC0_CMUCRdv (0x00000000)
#define RMOD_OC0_CMUCR(...) rset(OC0_CMUCR, OC0_CMUCRrv, __VA_ARGS__)
#define RFLD_OC0_CMUCR(fld) (*((const volatile OC0_CMUCR_T *)0xb8000388)).f.fld


typedef union {
	struct {
		unsigned int bus_slp_cnt:32;     //0
	} f;
	unsigned int v;
} OC0_CMUSCR_T;
#define OC0_CMUSCRrv (*((regval)0xb800038C))
#define OC0_CMUSCRdv (0x00000000)
#define RMOD_OC0_CMUSCR(...) rset(OC0_CMUSCR, OC0_CMUSCRrv, __VA_ARGS__)
#define RIZS_OC0_CMUSCR(...) rset(OC0_CMUSCR, 0, __VA_ARGS__)
#define RFLD_OC0_CMUSCR(fld) (*((const volatile OC0_CMUSCR_T *)0xb800038C)).f.fld


//=================================================
typedef union {
	struct {
		unsigned int busy:1;       // 1
        unsigned int mbz_30_7:24;  // 0
		unsigned int freq_div:3;   // 7
		unsigned int mbz_3_2:2;    // 0
		unsigned int cmu_mode:2;   // 0
	} f;
	unsigned int v;
} OC1_CMUGCR_T;
#define OC1_CMUGCRrv (*((regval)0xb8000390))
#define OC1_CMUGCRdv (0x80000070) 	/* Set all divisor to 1 and CMU disabled by default. */
#define RMOD_OC1_CMUGCR(...) rset(OC1_CMUGCR, OC1_CMUGCRrv, __VA_ARGS__)
#define RIZS_OC1_CMUGCR(...) rset(OC1_CMUGCR, 0, __VA_ARGS__)
#define RFLD_OC1_CMUGCR(fld) (*((const volatile OC1_CMUGCR_T *)0xb8000390)).f.fld


typedef union {
	struct {
		unsigned int mbz_31_3:29; // 0
		unsigned int dly_base:3;  // 0
	} f;
	unsigned int v;
} OC1_CMUSDCR_T;
#define OC1_CMUSDCRrv (*((regval)0xb8000394))
#define OC1_CMUSDCRdv (0x00000000)
#define RMOD_OC1_CMUSDCR(...) rset(OC1_CMUSDCR, OC1_CMUSDCRrv, __VA_ARGS__)
#define RIZS_OC1_CMUSDCR(...) rset(OC1_CMUSDCR, 0, __VA_ARGS__)
#define RFLD_OC1_CMUSDCR(fld) (*((const volatile OC1_CMUSDCR_T *)0xb8000394)).f.fld

typedef union {
	struct {
		unsigned int auto_bz:1;     //0
		unsigned int mbz_30_29:2;  //0
		unsigned int int_cxn_type:1;     //0
		unsigned int mbz_27_20:8;  //0
		unsigned int se_spif_wk:1;  //0
		unsigned int se_spif_slp:1; //0
		unsigned int se_spif:1;     //0
		unsigned int se_spif_hs:1;  //0
		unsigned int se_sram_rom_wk:1;//0
		unsigned int se_sram_rom_slp:1;//0
		unsigned int se_sram_rom:1;//0
		unsigned int se_sram_rom_hs:1;//0
		unsigned int se_dram_wk:1;  //0
		unsigned int se_dram_slp:1; //0
		unsigned int se_dram:1;     //0
		unsigned int se_dram_hs:1;  //0
		unsigned int mbz_7_4:4;     //0
		unsigned int dly_mul:4;     //0
	} f;
	unsigned int v;
} OC1_CMUCR_T;
#define OC1_CMUCRrv (*((regval)0xb8000398))
#define OC1_CMUCRdv (0x00000000)
#define RMOD_OC1_CMUCR(...) rset(OC1_CMUCR, OC1_CMUCRrv, __VA_ARGS__)
#define RIZS_OC1_CMUCR(...) rset(OC1_CMUCR, 0, __VA_ARGS__)
#define RFLD_OC1_CMUCR(fld) (*((const volatile OC1_CMUCR_T *)0xb8000398)).f.fld


typedef union {
    struct {
		unsigned int bus_slp_cnt:32;     //0
	} f;
	unsigned int v;
} OC1_CMUSCR_T;
#define OC1_CMUSCRrv (*((regval)0xb800039C))
#define OC1_CMUSCRdv (0x00000000)
#define RMOD_OC1_CMUSCR(...) rset(OC1_CMUSCR, OC1_CMUSCRrv, __VA_ARGS__)
#define RIZS_OC1_CMUSCR(...) rset(OC1_CMUSCR, 0, __VA_ARGS__)
#define RFLD_OC1_CMUSCR(fld) (*((const volatile OC1_CMUSCR_T *)0xb800039C)).f.fld


//=================================================

typedef union {
	struct {
    unsigned int mbz_31_21:11;
		unsigned int pbo_egw_lx_frq_slower:1;     //0
		unsigned int pbo_usr_lx_frq_slower:1;     //0
		unsigned int pbo_usw_lx_frq_slower:1;     //0
		unsigned int pbo_dsr_lx_frq_slower:1;     //0
		unsigned int pbo_dsw_lx_frq_slower:1;     //0
		unsigned int mbz_15_9:7;
		unsigned int lxp_frq_slower:1;     //0
		unsigned int mbz_7_4:4;
		unsigned int oc2_frq_slower:1;     //0
		unsigned int lx2_frq_slower:1;     //0
		unsigned int lx1_frq_slower:1;     //0
		unsigned int lx0_frq_slower:1;     //0
	} f;
	unsigned int v;
} LBSBCR_apro_T;
#define LBSBCR_apro_rv (*((regval)0xb80003A0))
#define LBSBCR_apro_dv (0x00000000)
#define RMOD_LBSBCR_apro(...) rset(LBSBCR_apro, LBSBCR_apro_rv, __VA_ARGS__)
#define RFLD_LBSBCR_apro(fld) (*((const volatile LBSBCR_apro_T *)0xb80003A0)).f.fld

#endif //__CMU_H__
