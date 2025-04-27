#ifndef _REG_MAP_SYS_H
#define _REG_MAP_SYS_H

/*-----------------------------------------------------
 Extraced from file_SYS_REG.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int chipid:32; //0xE0709
	} f;
	unsigned int v;
} CHIPID_T;
#define CHIPIDar (0xB8000000)
#define CHIPIDdv (0x000E0709)
#define CHIPIDrv RVAL(CHIPID)
#define RMOD_CHIPID(...) RMOD(CHIPID, __VA_ARGS__)
#define RIZS_CHIPID(...) RIZS(CHIPID, __VA_ARGS__)
#define RFLD_CHIPID(fld) RFLD(CHIPID, fld)

typedef union {
	struct {
		unsigned int rl_number:20; //0xE0709
		unsigned int verid:4; //0xF
		unsigned int mbz_0:8; //0x0
	} f;
	unsigned int v;
} RMR_T;
#define RMRar (0xB8000004)
#define RMRdv (0xE0709F00)
#define RMRrv RVAL(RMR)
#define RMOD_RMR(...) RMOD(RMR, __VA_ARGS__)
#define RIZS_RMR(...) RIZS(RMR, __VA_ARGS__)
#define RFLD_RMR(fld) RFLD(RMR, fld)

typedef union {
	struct {
		unsigned int mbz_0:9; //0x0
		unsigned int arb_lock_ok_lx1:1; //0x0
		unsigned int mbz_1:6; //0x0
		unsigned int arb_lock_bus_lx1:1; //0x0
		unsigned int mbz_2:11; //0x0
		unsigned int lx1_arbit_mode:2; //0x0
		unsigned int mbz_3:2; //0x0
	} f;
	unsigned int v;
} LBPCR_T;
#define LBPCRar (0xB800000C)
#define LBPCRdv (0x00000000)
#define LBPCRrv RVAL(LBPCR)
#define RMOD_LBPCR(...) RMOD(LBPCR, __VA_ARGS__)
#define RIZS_LBPCR(...) RIZS(LBPCR, __VA_ARGS__)
#define RFLD_LBPCR(fld) RFLD(LBPCR, fld)

typedef union {
	struct {
		unsigned int slot7_own:4; //0x2
		unsigned int slot6_own:4; //0x1
		unsigned int slot5_own:4; //0x0
		unsigned int slot4_own:4; //0x4
		unsigned int slot3_own:4; //0x3
		unsigned int slot2_own:4; //0x2
		unsigned int slot1_own:4; //0x1
		unsigned int slot0_own:4; //0x0
	} f;
	unsigned int v;
} LB1ACR0_T;
#define LB1ACR0ar (0xB8000020)
#define LB1ACR0dv (0x21043210)
#define LB1ACR0rv RVAL(LB1ACR0)
#define RMOD_LB1ACR0(...) RMOD(LB1ACR0, __VA_ARGS__)
#define RIZS_LB1ACR0(...) RIZS(LB1ACR0, __VA_ARGS__)
#define RFLD_LB1ACR0(fld) RFLD(LB1ACR0, fld)

typedef union {
	struct {
		unsigned int slot15_own:4; //0x0
		unsigned int slot14_own:4; //0x4
		unsigned int slot13_own:4; //0x3
		unsigned int slot12_own:4; //0x2
		unsigned int slot11_own:4; //0x1
		unsigned int slot10_own:4; //0x0
		unsigned int slot9_own:4; //0x4
		unsigned int slot8_own:4; //0x3
	} f;
	unsigned int v;
} LB1ACR1_T;
#define LB1ACR1ar (0xB8000024)
#define LB1ACR1dv (0x04321043)
#define LB1ACR1rv RVAL(LB1ACR1)
#define RMOD_LB1ACR1(...) RMOD(LB1ACR1, __VA_ARGS__)
#define RIZS_LB1ACR1(...) RIZS(LB1ACR1, __VA_ARGS__)
#define RFLD_LB1ACR1(fld) RFLD(LB1ACR1, fld)

typedef union {
	struct {
		unsigned int slot23_own:4; //0x3
		unsigned int slot22_own:4; //0x2
		unsigned int slot21_own:4; //0x1
		unsigned int slot20_own:4; //0x0
		unsigned int slot19_own:4; //0x4
		unsigned int slot18_own:4; //0x3
		unsigned int slot17_own:4; //0x2
		unsigned int slot16_own:4; //0x1
	} f;
	unsigned int v;
} LB1ACR2_T;
#define LB1ACR2ar (0xB8000028)
#define LB1ACR2dv (0x32104321)
#define LB1ACR2rv RVAL(LB1ACR2)
#define RMOD_LB1ACR2(...) RMOD(LB1ACR2, __VA_ARGS__)
#define RIZS_LB1ACR2(...) RIZS(LB1ACR2, __VA_ARGS__)
#define RFLD_LB1ACR2(fld) RFLD(LB1ACR2, fld)

typedef union {
	struct {
		unsigned int slot31_own:4; //0x1
		unsigned int slot30_own:4; //0x0
		unsigned int slot29_own:4; //0x4
		unsigned int slot28_own:4; //0x3
		unsigned int slot27_own:4; //0x2
		unsigned int slot26_own:4; //0x1
		unsigned int slot25_own:4; //0x0
		unsigned int slot24_own:4; //0x4
	} f;
	unsigned int v;
} LB1ACR3_T;
#define LB1ACR3ar (0xB800002C)
#define LB1ACR3dv (0x10432104)
#define LB1ACR3rv RVAL(LB1ACR3)
#define RMOD_LB1ACR3(...) RMOD(LB1ACR3, __VA_ARGS__)
#define RIZS_LB1ACR3(...) RIZS(LB1ACR3, __VA_ARGS__)
#define RFLD_LB1ACR3(fld) RFLD(LB1ACR3, fld)

typedef union {
	struct {
		unsigned int mbz_0:21; //0x0
		unsigned int gnt_dly:3; //0x0
		unsigned int mbz_1:4; //0x0
		unsigned int cksel_ocp1:1; //0x0
		unsigned int cksel_ocp0:1; //0x1
		unsigned int mbz_2:1; //0x0
		unsigned int soc_init_rdy:1; //0x0
	} f;
	unsigned int v;
} SSR_T;
#define SSRar (0xB8000044)
#define SSRdv (0x00000004)
#define SSRrv RVAL(SSR)
#define RMOD_SSR(...) RMOD(SSR, __VA_ARGS__)
#define RIZS_SSR(...) RIZS(SSR, __VA_ARGS__)
#define RFLD_SSR(fld) RFLD(SSR, fld)

typedef union {
	struct {
		unsigned int debug_mode:1; //0x0
		unsigned int mbz_0:15; //0x0
		unsigned int dbgo_sel:16; //0x0
	} f;
	unsigned int v;
} BDSEL_T;
#define BDSELar (0xB800004C)
#define BDSELdv (0x00000000)
#define BDSELrv RVAL(BDSEL)
#define RMOD_BDSEL(...) RMOD(BDSEL, __VA_ARGS__)
#define RIZS_BDSEL(...) RIZS(BDSEL, __VA_ARGS__)
#define RFLD_BDSEL(fld) RFLD(BDSEL, fld)

typedef union {
	struct {
		unsigned int mbz_0:10; //0x0
		unsigned int guess_len:6; //0x20
		unsigned int mbz_1:2; //0x0
		unsigned int chop_len:6; //0x4
		unsigned int mbz_2:7; //0x0
		unsigned int chop_en:1; //0x1
	} f;
	unsigned int v;
} LX0ADAP_T;
#define LX0ADAPar (0xB80000A0)
#define LX0ADAPdv (0x00200401)
#define LX0ADAPrv RVAL(LX0ADAP)
#define RMOD_LX0ADAP(...) RMOD(LX0ADAP, __VA_ARGS__)
#define RIZS_LX0ADAP(...) RIZS(LX0ADAP, __VA_ARGS__)
#define RFLD_LX0ADAP(fld) RFLD(LX0ADAP, fld)

typedef union {
	struct {
		unsigned int mbz_0:10; //0x0
		unsigned int guess_len:6; //0x20
		unsigned int mbz_1:2; //0x0
		unsigned int chop_len:6; //0x4
		unsigned int mbz_2:7; //0x0
		unsigned int chop_en:1; //0x1
	} f;
	unsigned int v;
} LX1ADAP_T;
#define LX1ADAPar (0xB80000A4)
#define LX1ADAPdv (0x00200401)
#define LX1ADAPrv RVAL(LX1ADAP)
#define RMOD_LX1ADAP(...) RMOD(LX1ADAP, __VA_ARGS__)
#define RIZS_LX1ADAP(...) RIZS(LX1ADAP, __VA_ARGS__)
#define RFLD_LX1ADAP(fld) RFLD(LX1ADAP, fld)

/*-----------------------------------------------------
 Extraced from file_PAD_CTRL.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:12; //0x0
		unsigned int pin_dramtype:2; //0x0
		unsigned int pin_btuptype:2; //0x0
		unsigned int pin_enctrlch:1; //0x0
		unsigned int cs1_mux_sel:2; //0x0
		unsigned int ddr_data_buswidth:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int flshcfg:1; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int pin_cksel:1; //0x0
		unsigned int testmode:1; //0x0
		unsigned int spdup:1; //0x0
		unsigned int cpmode:4; //0x0
	} f;
	unsigned int v;
} PSR0_T;
#define PSR0ar (0xB8000100)
#define PSR0dv (0x00000000)
#define PSR0rv RVAL(PSR0)
#define RMOD_PSR0(...) RMOD(PSR0, __VA_ARGS__)
#define RIZS_PSR0(...) RIZS(PSR0, __VA_ARGS__)
#define RFLD_PSR0(fld) RFLD(PSR0, fld)

typedef union {
	struct {
		unsigned int mbz_0:29; //0x0
		unsigned int pin_spifben:1; //0x0
		unsigned int pin_sdckg_div:2; //0x0
	} f;
	unsigned int v;
} PSR1_T;
#define PSR1ar (0xB8000104)
#define PSR1dv (0x00000000)
#define PSR1rv RVAL(PSR1)
#define RMOD_PSR1(...) RMOD(PSR1, __VA_ARGS__)
#define RIZS_PSR1(...) RIZS(PSR1, __VA_ARGS__)
#define RFLD_PSR1(fld) RFLD(PSR1, fld)

/*-----------------------------------------------------
 Extraced from file_MISC_CTRL.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int ifu_patch_enable:1; //0x1
		unsigned int mbz_0:31; //0x0
	} f;
	unsigned int v;
} CPUCNTR_T;
#define CPUCNTRar (0xB8000258)
#define CPUCNTRdv (0x80000000)
#define CPUCNTRrv RVAL(CPUCNTR)
#define RMOD_CPUCNTR(...) RMOD(CPUCNTR, __VA_ARGS__)
#define RIZS_CPUCNTR(...) RIZS(CPUCNTR, __VA_ARGS__)
#define RFLD_CPUCNTR(fld) RFLD(CPUCNTR, fld)

/*-----------------------------------------------------
 Extraced from file_CMU.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:22; //0x0
		unsigned int se_spif:1; //0x0
		unsigned int mbz_1:9; //0x0
	} f;
	unsigned int v;
} CMUCR_OC0_T;
#define CMUCR_OC0ar (0xB8000388)
#define CMUCR_OC0dv (0x00000000)
#define CMUCR_OC0rv RVAL(CMUCR_OC0)
#define RMOD_CMUCR_OC0(...) RMOD(CMUCR_OC0, __VA_ARGS__)
#define RIZS_CMUCR_OC0(...) RIZS(CMUCR_OC0, __VA_ARGS__)
#define RFLD_CMUCR_OC0(fld) RFLD(CMUCR_OC0, fld)

/*-----------------------------------------------------
 Extraced from file_MEM_BIST.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int l2_ws_bist_mode:1; //0x0
		unsigned int l2_ws_bist_rstn:1; //0x0
		unsigned int l2_ws_bist_done:1; //0x0
		unsigned int l2_ws_bist_fail:1; //0x0
		unsigned int l2_ws_drf_bist_mode:1; //0x0
		unsigned int l2_ws_drf_test_resume:1; //0x0
		unsigned int l2_ws_drf_bist_done:1; //0x0
		unsigned int l2_ws_drf_bist_fail:1; //0x0
		unsigned int l2_ws_drf_start_pause:1; //0x0
		unsigned int mbz_0:23; //0x0
	} f;
	unsigned int v;
} L2WSBIST_T;
#define L2WSBISTar (0xB8000430)
#define L2WSBISTdv (0x00000000)
#define L2WSBISTrv RVAL(L2WSBIST)
#define RMOD_L2WSBIST(...) RMOD(L2WSBIST, __VA_ARGS__)
#define RIZS_L2WSBIST(...) RIZS(L2WSBIST, __VA_ARGS__)
#define RFLD_L2WSBIST(fld) RFLD(L2WSBIST, fld)

typedef union {
	struct {
		unsigned int l2_tag_bist_mode:1; //0x0
		unsigned int l2_tag_bist_rstn:1; //0x0
		unsigned int l2_tag_bist_done:1; //0x0
		unsigned int l2_tag_bist_fail:8; //0x0
		unsigned int l2_tag_drf_bist_mode:1; //0x0
		unsigned int l2_tag_drf_test_resume:1; //0x0
		unsigned int l2_tag_drf_bist_done:1; //0x0
		unsigned int l2_tag_drf_bist_fail:8; //0x0
		unsigned int l2_tag_drf_start_pause:1; //0x0
		unsigned int mbz_0:9; //0x0
	} f;
	unsigned int v;
} L2TAGBIST_T;
#define L2TAGBISTar (0xB8000434)
#define L2TAGBISTdv (0x00000000)
#define L2TAGBISTrv RVAL(L2TAGBIST)
#define RMOD_L2TAGBIST(...) RMOD(L2TAGBIST, __VA_ARGS__)
#define RIZS_L2TAGBIST(...) RIZS(L2TAGBIST, __VA_ARGS__)
#define RFLD_L2TAGBIST(fld) RFLD(L2TAGBIST, fld)

typedef union {
	struct {
		unsigned int l2_data_bist_grp_en:2; //0x0
		unsigned int l2_data_bist_mode:1; //0x0
		unsigned int l2_data_bist_rstn:1; //0x0
		unsigned int l2_data_bist_done:1; //0x0
		unsigned int l2_data_bist_fail:16; //0x0
		unsigned int mbz_0:11; //0x0
	} f;
	unsigned int v;
} L2DATABIST_T;
#define L2DATABISTar (0xB8000438)
#define L2DATABISTdv (0x00000000)
#define L2DATABISTrv RVAL(L2DATABIST)
#define RMOD_L2DATABIST(...) RMOD(L2DATABIST, __VA_ARGS__)
#define RIZS_L2DATABIST(...) RIZS(L2DATABIST, __VA_ARGS__)
#define RFLD_L2DATABIST(fld) RFLD(L2DATABIST, fld)

typedef union {
	struct {
		unsigned int l2_data_drf_bist_mode:1; //0x0
		unsigned int l2_data_drf_test_resume:1; //0x0
		unsigned int l2_data_drf_bist_done:1; //0x0
		unsigned int l2_data_drf_bist_fail:16; //0x0
		unsigned int l2_data_drf_start_pause:1; //0x0
		unsigned int mbz_0:12; //0x0
	} f;
	unsigned int v;
} L2DATABIST2_T;
#define L2DATABIST2ar (0xB800043C)
#define L2DATABIST2dv (0x00000000)
#define L2DATABIST2rv RVAL(L2DATABIST2)
#define RMOD_L2DATABIST2(...) RMOD(L2DATABIST2, __VA_ARGS__)
#define RIZS_L2DATABIST2(...) RIZS(L2DATABIST2, __VA_ARGS__)
#define RFLD_L2DATABIST2(fld) RFLD(L2DATABIST2, fld)

typedef union {
	struct {
		unsigned int core0_icache_bist_grp_en:3; //0x0
		unsigned int core0_icache_bist_mode:1; //0x0
		unsigned int core0_icache_bist_rstn:1; //0x0
		unsigned int core0_icache_bist_done:1; //0x0
		unsigned int core0_icache_bist_fail:9; //0x0
		unsigned int core0_icache_drf_bist_mode:1; //0x0
		unsigned int core0_icache_drf_test_resume:1; //0x0
		unsigned int core0_icache_drf_bist_done:1; //0x0
		unsigned int core0_icache_drf_bist_fail:9; //0x0
		unsigned int core0_icache_drf_start_pause:1; //0x0
		unsigned int mbz_0:4; //0x0
	} f;
	unsigned int v;
} C0_ICBIST_T;
#define C0_ICBISTar (0xB8000440)
#define C0_ICBISTdv (0x00000000)
#define C0_ICBISTrv RVAL(C0_ICBIST)
#define RMOD_C0_ICBIST(...) RMOD(C0_ICBIST, __VA_ARGS__)
#define RIZS_C0_ICBIST(...) RIZS(C0_ICBIST, __VA_ARGS__)
#define RFLD_C0_ICBIST(fld) RFLD(C0_ICBIST, fld)

typedef union {
	struct {
		unsigned int core0_dcache_bist_grp_en:4; //0x0
		unsigned int core0_dcache_bist_mode:1; //0x0
		unsigned int core0_dcache_bist_rstn:1; //0x0
		unsigned int core0_dcache_bist_done:1; //0x0
		unsigned int core0_dcache_bist_fail:21; //0x0
		unsigned int mbz_0:4; //0x0
	} f;
	unsigned int v;
} C0_DCBIST0_T;
#define C0_DCBIST0ar (0xB8000444)
#define C0_DCBIST0dv (0x00000000)
#define C0_DCBIST0rv RVAL(C0_DCBIST0)
#define RMOD_C0_DCBIST0(...) RMOD(C0_DCBIST0, __VA_ARGS__)
#define RIZS_C0_DCBIST0(...) RIZS(C0_DCBIST0, __VA_ARGS__)
#define RFLD_C0_DCBIST0(fld) RFLD(C0_DCBIST0, fld)

typedef union {
	struct {
		unsigned int core0_dcache_drf_test_resume:1; //0x0
		unsigned int core0_dcache_drf_bist_mode:1; //0x0
		unsigned int core0_dcache_drf_bist_done:1; //0x0
		unsigned int core0_dcache_drf_bist_fail:21; //0x0
		unsigned int core0_dcache_drf_start_pause:1; //0x0
		unsigned int mbz_0:7; //0x0
	} f;
	unsigned int v;
} C0_DCBIST1_T;
#define C0_DCBIST1ar (0xB8000448)
#define C0_DCBIST1dv (0x00000000)
#define C0_DCBIST1rv RVAL(C0_DCBIST1)
#define RMOD_C0_DCBIST1(...) RMOD(C0_DCBIST1, __VA_ARGS__)
#define RIZS_C0_DCBIST1(...) RIZS(C0_DCBIST1, __VA_ARGS__)
#define RFLD_C0_DCBIST1(fld) RFLD(C0_DCBIST1, fld)

typedef union {
	struct {
		unsigned int core1_icache_bist_grp_en:3; //0x0
		unsigned int core1_icache_bist_mode:1; //0x0
		unsigned int core1_icache_bist_rstn:1; //0x0
		unsigned int core1_icache_bist_done:1; //0x0
		unsigned int core1_icache_bist_fail:9; //0x0
		unsigned int core1_icache_drf_bist_mode:1; //0x0
		unsigned int core1_icache_drf_test_resume:1; //0x0
		unsigned int core1_icache_drf_bist_done:1; //0x0
		unsigned int core1_icache_drf_bist_fail:9; //0x0
		unsigned int core1_icache_drf_start_pause:1; //0x0
		unsigned int mbz_0:4; //0x0
	} f;
	unsigned int v;
} C1_ICBIST_T;
#define C1_ICBISTar (0xB800044C)
#define C1_ICBISTdv (0x00000000)
#define C1_ICBISTrv RVAL(C1_ICBIST)
#define RMOD_C1_ICBIST(...) RMOD(C1_ICBIST, __VA_ARGS__)
#define RIZS_C1_ICBIST(...) RIZS(C1_ICBIST, __VA_ARGS__)
#define RFLD_C1_ICBIST(fld) RFLD(C1_ICBIST, fld)

typedef union {
	struct {
		unsigned int core1_dcache_bist_grp_en:4; //0x0
		unsigned int core1_dcache_bist_mode:1; //0x0
		unsigned int core1_dcache_bist_rstn:1; //0x0
		unsigned int core1_dcache_bist_done:1; //0x0
		unsigned int core1_dcache_bist_fail:21; //0x0
		unsigned int mbz_0:4; //0x0
	} f;
	unsigned int v;
} C1_DCBIST0_T;
#define C1_DCBIST0ar (0xB8000450)
#define C1_DCBIST0dv (0x00000000)
#define C1_DCBIST0rv RVAL(C1_DCBIST0)
#define RMOD_C1_DCBIST0(...) RMOD(C1_DCBIST0, __VA_ARGS__)
#define RIZS_C1_DCBIST0(...) RIZS(C1_DCBIST0, __VA_ARGS__)
#define RFLD_C1_DCBIST0(fld) RFLD(C1_DCBIST0, fld)

typedef union {
	struct {
		unsigned int core1_dcache_drf_test_resume:1; //0x0
		unsigned int core1_dcache_drf_bist_mode:1; //0x0
		unsigned int core1_dcache_drf_bist_done:1; //0x0
		unsigned int core1_dcache_drf_bist_fail:21; //0x0
		unsigned int core1_dcache_drf_start_pause:1; //0x0
		unsigned int mbz_0:7; //0x0
	} f;
	unsigned int v;
} C1_DCBIST1_T;
#define C1_DCBIST1ar (0xB8000454)
#define C1_DCBIST1dv (0x00000000)
#define C1_DCBIST1rv RVAL(C1_DCBIST1)
#define RMOD_C1_DCBIST1(...) RMOD(C1_DCBIST1, __VA_ARGS__)
#define RIZS_C1_DCBIST1(...) RIZS(C1_DCBIST1, __VA_ARGS__)
#define RFLD_C1_DCBIST1(fld) RFLD(C1_DCBIST1, fld)

typedef union {
	struct {
		unsigned int sheipa_sram_bist_mode:1; //0x0
		unsigned int sheipa_sram_bist_rstn:1; //0x0
		unsigned int sheipa_sram_bist_done:1; //0x0
		unsigned int sheipa_sram_bist_fail:1; //0x0
		unsigned int sheipa_sram_drf_bist_mode:1; //0x0
		unsigned int sheipa_sram_drf_test_resume:1; //0x0
		unsigned int sheipa_sram_drf_bist_done:1; //0x0
		unsigned int sheipa_sram_drf_bist_fail:1; //0x0
		unsigned int sheipa_sram_drf_start_pause:1; //0x0
		unsigned int mbz_0:4; //0x0
		unsigned int sheipa_rom_bist_mode:1; //0x0
		unsigned int sheipa_rom_bist_rstn:1; //0x0
		unsigned int sheipa_rom_bist_done:1; //0x0
		unsigned int sheipa_rom_bist_fail:1; //0x0
		unsigned int mbz_1:15; //0x0
	} f;
	unsigned int v;
} SPSRAMBIST_T;
#define SPSRAMBISTar (0xB8000458)
#define SPSRAMBISTdv (0x00000000)
#define SPSRAMBISTrv RVAL(SPSRAMBIST)
#define RMOD_SPSRAMBIST(...) RMOD(SPSRAMBIST, __VA_ARGS__)
#define RIZS_SPSRAMBIST(...) RIZS(SPSRAMBIST, __VA_ARGS__)
#define RFLD_SPSRAMBIST(fld) RFLD(SPSRAMBIST, fld)

typedef union {
	struct {
		unsigned int sheipa_cmd_sram_bist_mode:1; //0x0
		unsigned int sheipa_cmd_sram_bist_rstn:1; //0x0
		unsigned int sheipa_cmd_sram_bist_done:1; //0x0
		unsigned int sheipa_cmd_sram_bist_fail:1; //0x0
		unsigned int sheipa_cmd_sram_drf_bist_mode:1; //0x0
		unsigned int sheipa_cmd_sram_drf_test_resume:1; //0x0
		unsigned int sheipa_cmd_sram_drf_bist_done:1; //0x0
		unsigned int sheipa_cmd_sram_drf_bist_fail:1; //0x0
		unsigned int sheipa_cmd_sram_drf_start_pause:1; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int sheipa_msk_sram_bist_mode:1; //0x0
		unsigned int sheipa_msk_sram_bist_rstn:1; //0x0
		unsigned int sheipa_msk_sram_bist_done:1; //0x0
		unsigned int sheipa_msk_sram_bist_fail:1; //0x0
		unsigned int sheipa_msk_sram_drf_bist_mode:1; //0x0
		unsigned int sheipa_msk_sram_drf_test_resume:1; //0x0
		unsigned int sheipa_msk_sram_drf_bist_done:1; //0x0
		unsigned int sheipa_msk_sram_drf_bist_fail:1; //0x0
		unsigned int sheipa_msk_sram_drf_start_pause:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int sheipa_rd_sram_bist_mode:1; //0x0
		unsigned int sheipa_rd_sram_bist_rstn:1; //0x0
		unsigned int sheipa_rd_sram_bist_done:1; //0x0
		unsigned int sheipa_rd_sram_bist_fail:1; //0x0
		unsigned int sheipa_rd_sram_drf_bist_mode:1; //0x0
		unsigned int sheipa_rd_sram_drf_test_resume:1; //0x0
		unsigned int sheipa_rd_sram_drf_bist_done:1; //0x0
		unsigned int sheipa_rd_sram_drf_bist_fail:1; //0x0
		unsigned int sheipa_rd_sram_drf_start_pause:1; //0x0
		unsigned int mbz_2:3; //0x0
	} f;
	unsigned int v;
} SPCRRSRAMBIST_T;
#define SPCRRSRAMBISTar (0xB800045C)
#define SPCRRSRAMBISTdv (0x00000000)
#define SPCRRSRAMBISTrv RVAL(SPCRRSRAMBIST)
#define RMOD_SPCRRSRAMBIST(...) RMOD(SPCRRSRAMBIST, __VA_ARGS__)
#define RIZS_SPCRRSRAMBIST(...) RIZS(SPCRRSRAMBIST, __VA_ARGS__)
#define RFLD_SPCRRSRAMBIST(fld) RFLD(SPCRRSRAMBIST, fld)

typedef union {
	struct {
		unsigned int sheipa_wd_sram_bist_mode:1; //0x0
		unsigned int sheipa_wd_sram_bist_rstn:1; //0x0
		unsigned int sheipa_wd_sram_bist_done:1; //0x0
		unsigned int sheipa_wd_sram_bist_fail:1; //0x0
		unsigned int sheipa_wd_sram_drf_bist_mode:1; //0x0
		unsigned int sheipa_wd_sram_drf_test_resume:1; //0x0
		unsigned int sheipa_wd_sram_drf_bist_done:1; //0x0
		unsigned int sheipa_wd_sram_drf_bist_fail:1; //0x0
		unsigned int sheipa_wd_sram_drf_start_pause:1; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int sheipa_rg_sram_bist_mode:1; //0x0
		unsigned int sheipa_rg_sram_bist_rstn:1; //0x0
		unsigned int sheipa_rg_sram_bist_done:1; //0x0
		unsigned int sheipa_rg_sram_bist_fail:1; //0x0
		unsigned int sheipa_rg_sram_drf_bist_mode:1; //0x0
		unsigned int sheipa_rg_sram_drf_test_resume:1; //0x0
		unsigned int sheipa_rg_sram_drf_bist_done:1; //0x0
		unsigned int sheipa_rg_sram_drf_bist_fail:1; //0x0
		unsigned int sheipa_rg_sram_drf_start_pause:1; //0x0
		unsigned int mbz_1:13; //0x0
	} f;
	unsigned int v;
} SPWMSRAMBIST_T;
#define SPWMSRAMBISTar (0xB8000460)
#define SPWMSRAMBISTdv (0x00000000)
#define SPWMSRAMBISTrv RVAL(SPWMSRAMBIST)
#define RMOD_SPWMSRAMBIST(...) RMOD(SPWMSRAMBIST, __VA_ARGS__)
#define RIZS_SPWMSRAMBIST(...) RIZS(SPWMSRAMBIST, __VA_ARGS__)
#define RFLD_SPWMSRAMBIST(fld) RFLD(SPWMSRAMBIST, fld)

typedef union {
	struct {
		unsigned int misr_dataout_sheipa_rom:32; //0x0
	} f;
	unsigned int v;
} SPMD0_T;
#define SPMD0ar (0xB8000464)
#define SPMD0dv (0x00000000)
#define SPMD0rv RVAL(SPMD0)
#define RMOD_SPMD0(...) RMOD(SPMD0, __VA_ARGS__)
#define RIZS_SPMD0(...) RIZS(SPMD0, __VA_ARGS__)
#define RFLD_SPMD0(fld) RFLD(SPMD0, fld)

typedef union {
	struct {
		unsigned int misr_dataout_sheipa_rom:32; //0x0
	} f;
	unsigned int v;
} SPMD1_T;
#define SPMD1ar (0xB8000468)
#define SPMD1dv (0x00000000)
#define SPMD1rv RVAL(SPMD1)
#define RMOD_SPMD1(...) RMOD(SPMD1, __VA_ARGS__)
#define RIZS_SPMD1(...) RIZS(SPMD1, __VA_ARGS__)
#define RFLD_SPMD1(fld) RFLD(SPMD1, fld)

typedef union {
	struct {
		unsigned int bist_mode_emac1:1; //0x0
		unsigned int bist_mode_emac2:1; //0x0
		unsigned int bist_mode_gmac1:1; //0x0
		unsigned int bist_mode_gmac2:1; //0x0
		unsigned int bist_mode_gmac3:1; //0x0
		unsigned int bist_mode_gmac4:1; //0x0
		unsigned int bist_mode_gmac5:1; //0x0
		unsigned int bist_mode_rom1:1; //0x0
		unsigned int bist_mode_rom2:1; //0x0
		unsigned int bist_rstn_emac1_reg:1; //0x0
		unsigned int bist_rstn_emac2_reg:1; //0x0
		unsigned int bist_rstn_gmac1_reg:1; //0x0
		unsigned int bist_rstn_gmac2_reg:1; //0x0
		unsigned int bist_rstn_gmac3_reg:1; //0x0
		unsigned int bist_rstn_gmac4_reg:1; //0x0
		unsigned int bist_rstn_gmac5_reg:1; //0x0
		unsigned int bist_rstn_rom1_reg:1; //0x0
		unsigned int bist_rstn_rom2_reg:1; //0x0
		unsigned int drf_bist_mode_emac1:1; //0x0
		unsigned int drf_bist_mode_emac2:1; //0x0
		unsigned int drf_bist_mode_gmac1:1; //0x0
		unsigned int drf_bist_mode_gmac2:1; //0x0
		unsigned int drf_bist_mode_gmac3:1; //0x0
		unsigned int drf_bist_mode_gmac4:1; //0x0
		unsigned int drf_bist_mode_gmac5:1; //0x0
		unsigned int drf_test_resume_emac1:1; //0x0
		unsigned int drf_test_resume_emac2:1; //0x0
		unsigned int drf_test_resume_gmac1:1; //0x0
		unsigned int drf_test_resume_gmac2:1; //0x0
		unsigned int drf_test_resume_gmac3:1; //0x0
		unsigned int drf_test_resume_gmac4:1; //0x0
		unsigned int drf_test_resume_gmac5:1; //0x0
	} f;
	unsigned int v;
} EAGBIST_T;
#define EAGBISTar (0xB800046C)
#define EAGBISTdv (0x00000000)
#define EAGBISTrv RVAL(EAGBIST)
#define RMOD_EAGBIST(...) RMOD(EAGBIST, __VA_ARGS__)
#define RIZS_EAGBIST(...) RIZS(EAGBIST, __VA_ARGS__)
#define RFLD_EAGBIST(fld) RFLD(EAGBIST, fld)

typedef union {
	struct {
		unsigned int ptsel_debug_rom1:2; //0x1
		unsigned int ptsel_debug_rom2a:2; //0x1
		unsigned int ptsel_debug_rom2b:2; //0x1
		unsigned int rtsel_debug_rom1:2; //0x0
		unsigned int rtsel_debug_rom2a:2; //0x0
		unsigned int rtsel_debug_rom2b:2; //0x0
		unsigned int trb_debug_rom1:2; //0x1
		unsigned int trb_debug_rom2a:2; //0x1
		unsigned int trb_debug_rom2b:2; //0x1
		unsigned int rtsel_dbg_sheipa_brom:2; //0x1
		unsigned int ptsel_dbg_sheipa_brom:2; //0x1
		unsigned int trb_dbg_sheipa_brom:2; //0x1
		unsigned int mbz_0:8; //0x0
	} f;
	unsigned int v;
} PRDBG_T;
#define PRDBGar (0xB8000470)
#define PRDBGdv (0x54055500)
#define PRDBGrv RVAL(PRDBG)
#define RMOD_PRDBG(...) RMOD(PRDBG, __VA_ARGS__)
#define RIZS_PRDBG(...) RIZS(PRDBG, __VA_ARGS__)
#define RFLD_PRDBG(fld) RFLD(PRDBG, fld)

typedef union {
	struct {
		unsigned int bist_done_emac1:1; //0x0
		unsigned int bist_done_emac2:1; //0x0
		unsigned int bist_done_gmac1:1; //0x0
		unsigned int bist_done_gmac2:1; //0x0
		unsigned int bist_done_gmac3:1; //0x0
		unsigned int bist_done_gmac4:1; //0x0
		unsigned int bist_done_gmac5:1; //0x0
		unsigned int bist_done_rom1:1; //0x0
		unsigned int bist_done_rom2:1; //0x0
		unsigned int drf_bist_done_emac1:1; //0x0
		unsigned int drf_bist_done_emac2:1; //0x0
		unsigned int drf_bist_done_gmac1:1; //0x0
		unsigned int drf_bist_done_gmac2:1; //0x0
		unsigned int drf_bist_done_gmac3:1; //0x0
		unsigned int drf_bist_done_gmac4:1; //0x0
		unsigned int drf_bist_done_gmac5:1; //0x0
		unsigned int drf_start_pause_emac1:1; //0x0
		unsigned int drf_start_pause_emac2:1; //0x0
		unsigned int drf_start_pause_gmac1:1; //0x0
		unsigned int drf_start_pause_gmac2:1; //0x0
		unsigned int drf_start_pause_gmac3:1; //0x0
		unsigned int drf_start_pause_gmac4:1; //0x0
		unsigned int drf_start_pause_gmac5:1; //0x0
		unsigned int mbz_0:9; //0x0
	} f;
	unsigned int v;
} EGMBIST_STATUS_T;
#define EGMBIST_STATUSar (0xB8000474)
#define EGMBIST_STATUSdv (0x00000000)
#define EGMBIST_STATUSrv RVAL(EGMBIST_STATUS)
#define RMOD_EGMBIST_STATUS(...) RMOD(EGMBIST_STATUS, __VA_ARGS__)
#define RIZS_EGMBIST_STATUS(...) RIZS(EGMBIST_STATUS, __VA_ARGS__)
#define RFLD_EGMBIST_STATUS(fld) RFLD(EGMBIST_STATUS, fld)

typedef union {
	struct {
		unsigned int bist_fail_adm_twoprf_rx_gmac5:1; //0x0
		unsigned int bist_fail_adm_twoprf_tx_gmac3:1; //0x0
		unsigned int bist_fail_desc_fifo_gmac4:1; //0x0
		unsigned int bist_fail_isram_fifo_512x16_gmac2_a:1; //0x0
		unsigned int bist_fail_ram1p_fifo_0_gmac1_b:1; //0x0
		unsigned int bist_fail_ram1p_fifo_1_gmac1_c:1; //0x0
		unsigned int bist_fail_ram1p_linklist_gmac1_a:1; //0x0
		unsigned int bist_fail_ram1p_rbuf_emac1_a:1; //0x0
		unsigned int bist_fail_ram1p_sot_emac1_b:1; //0x0
		unsigned int bist_fail_ram2p_p_data_emac2_c:1; //0x0
		unsigned int bist_fail_ram2p_p_hdr_h_emac2_b:1; //0x0
		unsigned int bist_fail_ram2p_p_hdr_l_emac2_a:1; //0x0
		unsigned int bist_fail_rx_reg_fifo_1p_gmac2_b:1; //0x0
		unsigned int drf_bist_fail_adm_twoprf_rx_gmac5:1; //0x0
		unsigned int drf_bist_fail_adm_twoprf_tx_gmac3:1; //0x0
		unsigned int drf_bist_fail_desc_fifo_gmac4:1; //0x0
		unsigned int drf_bist_fail_isram_fifo_512x16_gmac2_a:1; //0x0
		unsigned int drf_bist_fail_ram1p_fifo_0_gmac1_b:1; //0x0
		unsigned int drf_bist_fail_ram1p_fifo_1_gmac1_c:1; //0x0
		unsigned int drf_bist_fail_ram1p_linklist_gmac1_a:1; //0x0
		unsigned int drf_bist_fail_ram1p_rbuf_emac1_a:1; //0x0
		unsigned int drf_bist_fail_ram1p_sot_emac1_b:1; //0x0
		unsigned int drf_bist_fail_ram2p_p_data_emac2_c:1; //0x0
		unsigned int drf_bist_fail_ram2p_p_hdr_h_emac2_b:1; //0x0
		unsigned int drf_bist_fail_ram2p_p_hdr_l_emac2_a:1; //0x0
		unsigned int drf_bist_fail_rx_reg_fifo_1p_gmac2_b:1; //0x0
		unsigned int drom_bist_fail:1; //0x0
		unsigned int ftr_rom_bist_fail:1; //0x0
		unsigned int irom_bist_fail:1; //0x0
		unsigned int mbz_0:3; //0x0
	} f;
	unsigned int v;
} EGMBIST_STATUS2_T;
#define EGMBIST_STATUS2ar (0xB8000478)
#define EGMBIST_STATUS2dv (0x00000000)
#define EGMBIST_STATUS2rv RVAL(EGMBIST_STATUS2)
#define RMOD_EGMBIST_STATUS2(...) RMOD(EGMBIST_STATUS2, __VA_ARGS__)
#define RIZS_EGMBIST_STATUS2(...) RIZS(EGMBIST_STATUS2, __VA_ARGS__)
#define RFLD_EGMBIST_STATUS2(fld) RFLD(EGMBIST_STATUS2, fld)

typedef union {
	struct {
		unsigned int misr_dataout_drom:32; //0x26D486E7
	} f;
	unsigned int v;
} MDO0_T;
#define MDO0ar (0xB800047C)
#define MDO0dv (0x26D486E7)
#define MDO0rv RVAL(MDO0)
#define RMOD_MDO0(...) RMOD(MDO0, __VA_ARGS__)
#define RIZS_MDO0(...) RIZS(MDO0, __VA_ARGS__)
#define RFLD_MDO0(fld) RFLD(MDO0, fld)

typedef union {
	struct {
		unsigned int misr_dataout_ftr_rom:32; //0x5CF70382
	} f;
	unsigned int v;
} MDO1_T;
#define MDO1ar (0xB8000480)
#define MDO1dv (0x5CF70382)
#define MDO1rv RVAL(MDO1)
#define RMOD_MDO1(...) RMOD(MDO1, __VA_ARGS__)
#define RIZS_MDO1(...) RIZS(MDO1, __VA_ARGS__)
#define RFLD_MDO1(fld) RFLD(MDO1, fld)

typedef union {
	struct {
		unsigned int misr_dataout_irom_fifo:32; //0x39F861E8
	} f;
	unsigned int v;
} MDO2_T;
#define MDO2ar (0xB8000484)
#define MDO2dv (0x39F861E8)
#define MDO2rv RVAL(MDO2)
#define RMOD_MDO2(...) RMOD(MDO2, __VA_ARGS__)
#define RIZS_MDO2(...) RIZS(MDO2, __VA_ARGS__)
#define RFLD_MDO2(fld) RFLD(MDO2, fld)

typedef union {
	struct {
		unsigned int usb2_host_bist_mode:1; //0x0
		unsigned int usb2_host_bist_rstn:1; //0x0
		unsigned int usb2_host_bist_done:1; //0x0
		unsigned int usb2_host_bist_fail:1; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int usb2_host_drf_bist_mode:1; //0x0
		unsigned int usb2_host_drf_resume:1; //0x0
		unsigned int usb2_host_drf_start_pause:1; //0x0
		unsigned int usb2_host_drf_bist_done:1; //0x0
		unsigned int usb2_host_drf_bist_fail:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int spi_nand_ecc_bist_mode:1; //0x0
		unsigned int spi_nand_ecc_bist_rstn:1; //0x0
		unsigned int spi_nand_ecc_bist_done:1; //0x0
		unsigned int spi_nand_ecc_bist_fail:1; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int spi_nand_ecc_drf_bist_mode:1; //0x0
		unsigned int spi_nand_ecc_drf_test_resume:1; //0x0
		unsigned int spi_nand_ecc_drf_start_pause:1; //0x0
		unsigned int spi_nand_ecc_drf_bist_done:1; //0x0
		unsigned int spi_nand_ecc_drf_bist_fail:1; //0x0
		unsigned int mbz_3:11; //0x0
	} f;
	unsigned int v;
} USB2ECCBIST_T;
#define USB2ECCBISTar (0xB8000488)
#define USB2ECCBISTdv (0x00000000)
#define USB2ECCBISTrv RVAL(USB2ECCBIST)
#define RMOD_USB2ECCBIST(...) RMOD(USB2ECCBIST, __VA_ARGS__)
#define RIZS_USB2ECCBIST(...) RIZS(USB2ECCBIST, __VA_ARGS__)
#define RFLD_USB2ECCBIST(fld) RFLD(USB2ECCBIST, fld)

/*-----------------------------------------------------
 Extraced from file_ANA_PHY.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int slb_hs:1; //0x1
		unsigned int ldo_pw:1; //0x0
		unsigned int lf_pd_r:1; //0x1
		unsigned int by_pass_on:1; //0x0
		unsigned int txbitstuff_en:1; //0x1
		unsigned int clktsten:1; //0x0
		unsigned int mbz_1:10; //0x0
		unsigned int uphy_sw_rstn:1; //0x1
		unsigned int mbz_2:1; //0x0
		unsigned int utmi_reset_0:1; //0x0
		unsigned int force_slb_0:1; //0x0
		unsigned int slb_done_0:1; //0x0
		unsigned int slb_fail_0:1; //0x0
		unsigned int ckusable_outp_0:1; //0x0
		unsigned int vstatus_in_0:8; //0x0
	} f;
	unsigned int v;
} USB_H_PHY_CTRL_T;
#define USB_H_PHY_CTRLar (0xB8000500)
#define USB_H_PHY_CTRLdv (0x54004000)
#define USB_H_PHY_CTRLrv RVAL(USB_H_PHY_CTRL)
#define RMOD_USB_H_PHY_CTRL(...) RMOD(USB_H_PHY_CTRL, __VA_ARGS__)
#define RIZS_USB_H_PHY_CTRL(...) RIZS(USB_H_PHY_CTRL, __VA_ARGS__)
#define RFLD_USB_H_PHY_CTRL(fld) RFLD(USB_H_PHY_CTRL, fld)

typedef union {
	struct {
		unsigned int pow_cphy:1; //0x1
		unsigned int phy_test_mode:1; //0x0
		unsigned int mbz_0:4; //0x0
		unsigned int cphy_isolate:1; //0x0
		unsigned int mbz_1:25; //0x0
	} f;
	unsigned int v;
} PCIE_PHY_CTRL_T;
#define PCIE_PHY_CTRLar (0xB8000504)
#define PCIE_PHY_CTRLdv (0x80000000)
#define PCIE_PHY_CTRLrv RVAL(PCIE_PHY_CTRL)
#define RMOD_PCIE_PHY_CTRL(...) RMOD(PCIE_PHY_CTRL, __VA_ARGS__)
#define RIZS_PCIE_PHY_CTRL(...) RIZS(PCIE_PHY_CTRL, __VA_ARGS__)
#define RFLD_PCIE_PHY_CTRL(fld) RFLD(PCIE_PHY_CTRL, fld)

/*-----------------------------------------------------
 Extraced from file_IP_ENABLE.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int en_ecc:1; //0x1
		unsigned int en_serial_nand:1; //0x1
		unsigned int en_pcie_dev:1; //0x1
		unsigned int mbz_0:17; //0x0
		unsigned int en_ipsec:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int en_gdma1:1; //0x1
		unsigned int en_gdma0:1; //0x1
		unsigned int en_p0usbphy:1; //0x1
		unsigned int mbz_2:3; //0x0
		unsigned int en_p0usbhost:1; //0x1
		unsigned int mbz_3:3; //0x0
	} f;
	unsigned int v;
} IP_EN_CTRL_T;
#define IP_EN_CTRLar (0xB8000600)
#define IP_EN_CTRLdv (0xE0000B88)
#define IP_EN_CTRLrv RVAL(IP_EN_CTRL)
#define RMOD_IP_EN_CTRL(...) RMOD(IP_EN_CTRL, __VA_ARGS__)
#define RIZS_IP_EN_CTRL(...) RIZS(IP_EN_CTRL, __VA_ARGS__)
#define RFLD_IP_EN_CTRL(fld) RFLD(IP_EN_CTRL, fld)

typedef union {
	struct {
		unsigned int pcie_phy_debug_en:1; //0x0
		unsigned int mbz_0:3; //0x0
		unsigned int dbgo_shift:4; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int dbg_grp_sel:5; //0x0
		unsigned int dbg_blk_sel:4; //0x0
		unsigned int dbg_subgrp_sel:4; //0x0
		unsigned int mbz_2:3; //0x0
		unsigned int cpu_cmu_div_sel:1; //0x0
		unsigned int cpu_cmu_sleep_test:1; //0x0
		unsigned int cpu_cmu_bypass:1; //0x1
		unsigned int cpu_sync_level_sel:1; //0x1
		unsigned int cpu_clk_sel:1; //0x1
	} f;
	unsigned int v;
} SYS_MISC_CTRL0_T;
#define SYS_MISC_CTRL0ar (0xB8000614)
#define SYS_MISC_CTRL0dv (0x00000007)
#define SYS_MISC_CTRL0rv RVAL(SYS_MISC_CTRL0)
#define RMOD_SYS_MISC_CTRL0(...) RMOD(SYS_MISC_CTRL0, __VA_ARGS__)
#define RIZS_SYS_MISC_CTRL0(...) RIZS(SYS_MISC_CTRL0, __VA_ARGS__)
#define RFLD_SYS_MISC_CTRL0(fld) RFLD(SYS_MISC_CTRL0, fld)

typedef union {
	struct {
		unsigned int mbz_0:26; //0x0
		unsigned int dss_rst_n:1; //0x0
		unsigned int wire_sel:1; //0x0
		unsigned int speed_en:1; //0x0
		unsigned int ro_sel:3; //0x0
	} f;
	unsigned int v;
} DSS0_CTRL_T;
#define DSS0_CTRLar (0xB8000630)
#define DSS0_CTRLdv (0x00000000)
#define DSS0_CTRLrv RVAL(DSS0_CTRL)
#define RMOD_DSS0_CTRL(...) RMOD(DSS0_CTRL, __VA_ARGS__)
#define RIZS_DSS0_CTRL(...) RIZS(DSS0_CTRL, __VA_ARGS__)
#define RFLD_DSS0_CTRL(fld) RFLD(DSS0_CTRL, fld)

typedef union {
	struct {
		unsigned int ready:1; //0x0
		unsigned int mbz_0:11; //0x0
		unsigned int count_out:20; //0x0
	} f;
	unsigned int v;
} DSS0_DATA_T;
#define DSS0_DATAar (0xB8000634)
#define DSS0_DATAdv (0x00000000)
#define DSS0_DATArv RVAL(DSS0_DATA)
#define RMOD_DSS0_DATA(...) RMOD(DSS0_DATA, __VA_ARGS__)
#define RIZS_DSS0_DATA(...) RIZS(DSS0_DATA, __VA_ARGS__)
#define RFLD_DSS0_DATA(fld) RFLD(DSS0_DATA, fld)

typedef union {
	struct {
		unsigned int mbz_0:26; //0x0
		unsigned int dss_rst_n:1; //0x0
		unsigned int wire_sel:1; //0x0
		unsigned int speed_en:1; //0x0
		unsigned int ro_sel:3; //0x0
	} f;
	unsigned int v;
} DSS1_CTRL_T;
#define DSS1_CTRLar (0xB8000638)
#define DSS1_CTRLdv (0x00000000)
#define DSS1_CTRLrv RVAL(DSS1_CTRL)
#define RMOD_DSS1_CTRL(...) RMOD(DSS1_CTRL, __VA_ARGS__)
#define RIZS_DSS1_CTRL(...) RIZS(DSS1_CTRL, __VA_ARGS__)
#define RFLD_DSS1_CTRL(fld) RFLD(DSS1_CTRL, fld)

typedef union {
	struct {
		unsigned int ready:1; //0x0
		unsigned int mbz_0:11; //0x0
		unsigned int count_out:20; //0x0
	} f;
	unsigned int v;
} DSS1_DATA_T;
#define DSS1_DATAar (0xB800063C)
#define DSS1_DATAdv (0x00000000)
#define DSS1_DATArv RVAL(DSS1_DATA)
#define RMOD_DSS1_DATA(...) RMOD(DSS1_DATA, __VA_ARGS__)
#define RIZS_DSS1_DATA(...) RIZS(DSS1_DATA, __VA_ARGS__)
#define RFLD_DSS1_DATA(fld) RFLD(DSS1_DATA, fld)

typedef union {
	struct {
		unsigned int core0_voltprobe_gnd_ctrl:1; //0x0
		unsigned int core0_voltprobe_pwr_ctrl:1; //0x0
		unsigned int mbz_0:2; //0x0
		unsigned int core1_voltprobe_gnd_ctrl:1; //0x0
		unsigned int core1_voltprobe_pwr_ctrl:1; //0x0
		unsigned int mbz_1:26; //0x0
	} f;
	unsigned int v;
} VOL_PROBE_T;
#define VOL_PROBEar (0xB8000650)
#define VOL_PROBEdv (0x00000000)
#define VOL_PROBErv RVAL(VOL_PROBE)
#define RMOD_VOL_PROBE(...) RMOD(VOL_PROBE, __VA_ARGS__)
#define RIZS_VOL_PROBE(...) RIZS(VOL_PROBE, __VA_ARGS__)
#define RFLD_VOL_PROBE(fld) RFLD(VOL_PROBE, fld)

typedef union {
	struct {
		unsigned int pll_stable:1; //0x0
		unsigned int reg_clken_dcphy:1; //0x0
		unsigned int reg_clken_pllreg:1; //0x0
		unsigned int ref_cmd_rst_n_rx:1; //0x1
		unsigned int ref_cmd_rst_n_tx:1; //0x1
		unsigned int reg_bypass_en_rx:1; //0x0
		unsigned int reg_bypass_en_tx:1; //0x0
		unsigned int reg_force_rst_n_rx:1; //0x1
		unsigned int reg_force_rst_n_tx:1; //0x1
		unsigned int reg_ref_rst_en_rx:1; //0x0
		unsigned int reg_ref_rst_en_tx:1; //0x0
		unsigned int dpi_dll_voltageprobe_pwr_en:1; //0x0
		unsigned int dpi_dll_voltageprobe_gnd_en:1; //0x0
		unsigned int mbz_0:19; //0x0
	} f;
	unsigned int v;
} DPI_DLL_T;
#define DPI_DLLar (0xB8000660)
#define DPI_DLLdv (0x19800000)
#define DPI_DLLrv RVAL(DPI_DLL)
#define RMOD_DPI_DLL(...) RMOD(DPI_DLL, __VA_ARGS__)
#define RIZS_DPI_DLL(...) RIZS(DPI_DLL, __VA_ARGS__)
#define RFLD_DPI_DLL(fld) RFLD(DPI_DLL, fld)

typedef union {
	struct {
		unsigned int debug_bus_sel:4; //0x0
		unsigned int debug_lxdata_sel:3; //0x0
		unsigned int debug_data_sel:2; //0x0
		unsigned int mbz_0:23; //0x0
	} f;
	unsigned int v;
} OCP_DBG_T;
#define OCP_DBGar (0xB8000670)
#define OCP_DBGdv (0x00000000)
#define OCP_DBGrv RVAL(OCP_DBG)
#define RMOD_OCP_DBG(...) RMOD(OCP_DBG, __VA_ARGS__)
#define RIZS_OCP_DBG(...) RIZS(OCP_DBG, __VA_ARGS__)
#define RFLD_OCP_DBG(fld) RFLD(OCP_DBG, fld)

#endif // _REG_MAP_SYS_H
