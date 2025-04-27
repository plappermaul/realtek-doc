#ifndef _REGISTER_MAP_H_
#define _REGISTER_MAP_H_

#include <reg_map_util.h>

/*-----------------------------------------------------
 Extraced from file_LXB_CTRL.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int no_use31:32; //0
	} f;
	unsigned int v;
} LXB_CTRL_NO_USE_0000_T;
#define LXB_CTRL_NO_USE_0000rv (*((regval)0xb8000000))
#define LXB_CTRL_NO_USE_0000dv (0x00000000)
#define RMOD_LXB_CTRL_NO_USE_0000(...) rset(LXB_CTRL_NO_USE_0000, LXB_CTRL_NO_USE_0000rv, __VA_ARGS__)
#define RIZS_LXB_CTRL_NO_USE_0000(...) rset(LXB_CTRL_NO_USE_0000, 0, __VA_ARGS__)
#define RFLD_LXB_CTRL_NO_USE_0000(fld) (*((const volatile LXB_CTRL_NO_USE_0000_T *)0xb8000000)).f.fld

typedef union {
	struct {
		unsigned int no_use31:24; //0
		unsigned int mbz_0:8; //0
	} f;
	unsigned int v;
} LXB_CTRL_NO_USE_0004_T;
#define LXB_CTRL_NO_USE_0004rv (*((regval)0xb8000004))
#define LXB_CTRL_NO_USE_0004dv (0x00000000)
#define RMOD_LXB_CTRL_NO_USE_0004(...) rset(LXB_CTRL_NO_USE_0004, LXB_CTRL_NO_USE_0004rv, __VA_ARGS__)
#define RIZS_LXB_CTRL_NO_USE_0004(...) rset(LXB_CTRL_NO_USE_0004, 0, __VA_ARGS__)
#define RFLD_LXB_CTRL_NO_USE_0004(fld) (*((const volatile LXB_CTRL_NO_USE_0004_T *)0xb8000004)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int to_en:1; //1
		unsigned int to_lmt:3; //7
	} f;
	unsigned int v;
} BUS_TO_CTRL_T;
#define BUS_TO_CTRLrv (*((regval)0xb8000008))
#define BUS_TO_CTRLdv (0x0000000f)
#define RMOD_BUS_TO_CTRL(...) rset(BUS_TO_CTRL, BUS_TO_CTRLrv, __VA_ARGS__)
#define RIZS_BUS_TO_CTRL(...) rset(BUS_TO_CTRL, 0, __VA_ARGS__)
#define RFLD_BUS_TO_CTRL(fld) (*((const volatile BUS_TO_CTRL_T *)0xb8000008)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:21; //0
		unsigned int no_use10:3; //0
		unsigned int mbz_1:2; //0
		unsigned int lx2_arb_mode:2; //0
		unsigned int lx1_arb_mode:2; //0
		unsigned int lx0_arb_mode:2; //0
	} f;
	unsigned int v;
} LBPRICR_T;
#define LBPRICRrv (*((regval)0xb800000c))
#define LBPRICRdv (0x00000000)
#define RMOD_LBPRICR(...) rset(LBPRICR, LBPRICRrv, __VA_ARGS__)
#define RIZS_LBPRICR(...) rset(LBPRICR, 0, __VA_ARGS__)
#define RFLD_LBPRICR(fld) (*((const volatile LBPRICR_T *)0xb800000c)).f.fld

typedef union {
	struct {
		unsigned int slot7_own:4; //1
		unsigned int slot6_own:4; //0
		unsigned int slot5_own:4; //1
		unsigned int slot4_own:4; //0
		unsigned int slot3_own:4; //1
		unsigned int slot2_own:4; //0
		unsigned int slot1_own:4; //1
		unsigned int slot0_own:4; //0
	} f;
	unsigned int v;
} LX0_ARB_CFG0_T;
#define LX0_ARB_CFG0rv (*((regval)0xb8000010))
#define LX0_ARB_CFG0dv (0x10101010)
#define RMOD_LX0_ARB_CFG0(...) rset(LX0_ARB_CFG0, LX0_ARB_CFG0rv, __VA_ARGS__)
#define RIZS_LX0_ARB_CFG0(...) rset(LX0_ARB_CFG0, 0, __VA_ARGS__)
#define RFLD_LX0_ARB_CFG0(fld) (*((const volatile LX0_ARB_CFG0_T *)0xb8000010)).f.fld

typedef union {
	struct {
		unsigned int slot15_own:4; //1
		unsigned int slot14_own:4; //0
		unsigned int slot13_own:4; //1
		unsigned int slot12_own:4; //0
		unsigned int slot11_own:4; //1
		unsigned int slot10_own:4; //0
		unsigned int slot9_own:4; //1
		unsigned int slot8_own:4; //0
	} f;
	unsigned int v;
} LX0_ARB_CFG1_T;
#define LX0_ARB_CFG1rv (*((regval)0xb8000014))
#define LX0_ARB_CFG1dv (0x10101010)
#define RMOD_LX0_ARB_CFG1(...) rset(LX0_ARB_CFG1, LX0_ARB_CFG1rv, __VA_ARGS__)
#define RIZS_LX0_ARB_CFG1(...) rset(LX0_ARB_CFG1, 0, __VA_ARGS__)
#define RFLD_LX0_ARB_CFG1(fld) (*((const volatile LX0_ARB_CFG1_T *)0xb8000014)).f.fld

typedef union {
	struct {
		unsigned int slot23_own:4; //1
		unsigned int slot22_own:4; //0
		unsigned int slot21_own:4; //1
		unsigned int slot20_own:4; //0
		unsigned int slot19_own:4; //1
		unsigned int slot18_own:4; //0
		unsigned int slot17_own:4; //1
		unsigned int slot16_own:4; //0
	} f;
	unsigned int v;
} LX0_ARB_CFG2_T;
#define LX0_ARB_CFG2rv (*((regval)0xb8000018))
#define LX0_ARB_CFG2dv (0x10101010)
#define RMOD_LX0_ARB_CFG2(...) rset(LX0_ARB_CFG2, LX0_ARB_CFG2rv, __VA_ARGS__)
#define RIZS_LX0_ARB_CFG2(...) rset(LX0_ARB_CFG2, 0, __VA_ARGS__)
#define RFLD_LX0_ARB_CFG2(fld) (*((const volatile LX0_ARB_CFG2_T *)0xb8000018)).f.fld

typedef union {
	struct {
		unsigned int slot31_own:4; //1
		unsigned int slot30_own:4; //0
		unsigned int slot29_own:4; //1
		unsigned int slot28_own:4; //0
		unsigned int slot27_own:4; //1
		unsigned int slot26_own:4; //0
		unsigned int slot25_own:4; //1
		unsigned int slot24_own:4; //0
	} f;
	unsigned int v;
} LX0_ARB_CFG3_T;
#define LX0_ARB_CFG3rv (*((regval)0xb800001c))
#define LX0_ARB_CFG3dv (0x10101010)
#define RMOD_LX0_ARB_CFG3(...) rset(LX0_ARB_CFG3, LX0_ARB_CFG3rv, __VA_ARGS__)
#define RIZS_LX0_ARB_CFG3(...) rset(LX0_ARB_CFG3, 0, __VA_ARGS__)
#define RFLD_LX0_ARB_CFG3(fld) (*((const volatile LX0_ARB_CFG3_T *)0xb800001c)).f.fld

typedef union {
	struct {
		unsigned int slot7_own:4; //1
		unsigned int slot6_own:4; //0
		unsigned int slot5_own:4; //1
		unsigned int slot4_own:4; //0
		unsigned int slot3_own:4; //1
		unsigned int slot2_own:4; //0
		unsigned int slot1_own:4; //1
		unsigned int slot0_own:4; //0
	} f;
	unsigned int v;
} LX1_ARB_CFG0_T;
#define LX1_ARB_CFG0rv (*((regval)0xb8000020))
#define LX1_ARB_CFG0dv (0x10101010)
#define RMOD_LX1_ARB_CFG0(...) rset(LX1_ARB_CFG0, LX1_ARB_CFG0rv, __VA_ARGS__)
#define RIZS_LX1_ARB_CFG0(...) rset(LX1_ARB_CFG0, 0, __VA_ARGS__)
#define RFLD_LX1_ARB_CFG0(fld) (*((const volatile LX1_ARB_CFG0_T *)0xb8000020)).f.fld

typedef union {
	struct {
		unsigned int slot15_own:4; //1
		unsigned int slot14_own:4; //0
		unsigned int slot13_own:4; //1
		unsigned int slot12_own:4; //0
		unsigned int slot11_own:4; //1
		unsigned int slot10_own:4; //0
		unsigned int slot9_own:4; //1
		unsigned int slot8_own:4; //0
	} f;
	unsigned int v;
} LX1_ARB_CFG1_T;
#define LX1_ARB_CFG1rv (*((regval)0xb8000024))
#define LX1_ARB_CFG1dv (0x10101010)
#define RMOD_LX1_ARB_CFG1(...) rset(LX1_ARB_CFG1, LX1_ARB_CFG1rv, __VA_ARGS__)
#define RIZS_LX1_ARB_CFG1(...) rset(LX1_ARB_CFG1, 0, __VA_ARGS__)
#define RFLD_LX1_ARB_CFG1(fld) (*((const volatile LX1_ARB_CFG1_T *)0xb8000024)).f.fld

typedef union {
	struct {
		unsigned int slot23_own:4; //1
		unsigned int slot22_own:4; //0
		unsigned int slot21_own:4; //1
		unsigned int slot20_own:4; //0
		unsigned int slot19_own:4; //1
		unsigned int slot18_own:4; //0
		unsigned int slot17_own:4; //1
		unsigned int slot16_own:4; //0
	} f;
	unsigned int v;
} LX1_ARB_CFG2_T;
#define LX1_ARB_CFG2rv (*((regval)0xb8000028))
#define LX1_ARB_CFG2dv (0x10101010)
#define RMOD_LX1_ARB_CFG2(...) rset(LX1_ARB_CFG2, LX1_ARB_CFG2rv, __VA_ARGS__)
#define RIZS_LX1_ARB_CFG2(...) rset(LX1_ARB_CFG2, 0, __VA_ARGS__)
#define RFLD_LX1_ARB_CFG2(fld) (*((const volatile LX1_ARB_CFG2_T *)0xb8000028)).f.fld

typedef union {
	struct {
		unsigned int slot31_own:4; //1
		unsigned int slot30_own:4; //0
		unsigned int slot29_own:4; //1
		unsigned int slot28_own:4; //0
		unsigned int slot27_own:4; //1
		unsigned int slot26_own:4; //0
		unsigned int slot25_own:4; //1
		unsigned int slot24_own:4; //0
	} f;
	unsigned int v;
} LX1_ARB_CFG3_T;
#define LX1_ARB_CFG3rv (*((regval)0xb800002c))
#define LX1_ARB_CFG3dv (0x10101010)
#define RMOD_LX1_ARB_CFG3(...) rset(LX1_ARB_CFG3, LX1_ARB_CFG3rv, __VA_ARGS__)
#define RIZS_LX1_ARB_CFG3(...) rset(LX1_ARB_CFG3, 0, __VA_ARGS__)
#define RFLD_LX1_ARB_CFG3(fld) (*((const volatile LX1_ARB_CFG3_T *)0xb800002c)).f.fld

typedef union {
	struct {
		unsigned int slot7_own:4; //1
		unsigned int slot6_own:4; //0
		unsigned int slot5_own:4; //2
		unsigned int slot4_own:4; //1
		unsigned int slot3_own:4; //0
		unsigned int slot2_own:4; //2
		unsigned int slot1_own:4; //1
		unsigned int slot0_own:4; //0
	} f;
	unsigned int v;
} LX2_ARB_CFG0_T;
#define LX2_ARB_CFG0rv (*((regval)0xb8000030))
#define LX2_ARB_CFG0dv (0x10210210)
#define RMOD_LX2_ARB_CFG0(...) rset(LX2_ARB_CFG0, LX2_ARB_CFG0rv, __VA_ARGS__)
#define RIZS_LX2_ARB_CFG0(...) rset(LX2_ARB_CFG0, 0, __VA_ARGS__)
#define RFLD_LX2_ARB_CFG0(fld) (*((const volatile LX2_ARB_CFG0_T *)0xb8000030)).f.fld

typedef union {
	struct {
		unsigned int slot15_own:4; //1
		unsigned int slot14_own:4; //0
		unsigned int slot13_own:4; //2
		unsigned int slot12_own:4; //1
		unsigned int slot11_own:4; //0
		unsigned int slot10_own:4; //2
		unsigned int slot9_own:4; //1
		unsigned int slot8_own:4; //0
	} f;
	unsigned int v;
} LX2_ARB_CFG1_T;
#define LX2_ARB_CFG1rv (*((regval)0xb8000034))
#define LX2_ARB_CFG1dv (0x10210210)
#define RMOD_LX2_ARB_CFG1(...) rset(LX2_ARB_CFG1, LX2_ARB_CFG1rv, __VA_ARGS__)
#define RIZS_LX2_ARB_CFG1(...) rset(LX2_ARB_CFG1, 0, __VA_ARGS__)
#define RFLD_LX2_ARB_CFG1(fld) (*((const volatile LX2_ARB_CFG1_T *)0xb8000034)).f.fld

typedef union {
	struct {
		unsigned int slot23_own:4; //1
		unsigned int slot22_own:4; //0
		unsigned int slot21_own:4; //2
		unsigned int slot20_own:4; //1
		unsigned int slot19_own:4; //0
		unsigned int slot18_own:4; //2
		unsigned int slot17_own:4; //1
		unsigned int slot16_own:4; //0
	} f;
	unsigned int v;
} LX2_ARB_CFG2_T;
#define LX2_ARB_CFG2rv (*((regval)0xb8000038))
#define LX2_ARB_CFG2dv (0x10210210)
#define RMOD_LX2_ARB_CFG2(...) rset(LX2_ARB_CFG2, LX2_ARB_CFG2rv, __VA_ARGS__)
#define RIZS_LX2_ARB_CFG2(...) rset(LX2_ARB_CFG2, 0, __VA_ARGS__)
#define RFLD_LX2_ARB_CFG2(fld) (*((const volatile LX2_ARB_CFG2_T *)0xb8000038)).f.fld

typedef union {
	struct {
		unsigned int slot31_own:4; //1
		unsigned int slot30_own:4; //0
		unsigned int slot29_own:4; //2
		unsigned int slot28_own:4; //1
		unsigned int slot27_own:4; //0
		unsigned int slot26_own:4; //2
		unsigned int slot25_own:4; //1
		unsigned int slot24_own:4; //0
	} f;
	unsigned int v;
} LXBUS2_ARBIT_CFG_3_T;
#define LXBUS2_ARBIT_CFG_3rv (*((regval)0xb800003c))
#define LXBUS2_ARBIT_CFG_3dv (0x10210210)
#define RMOD_LXBUS2_ARBIT_CFG_3(...) rset(LXBUS2_ARBIT_CFG_3, LXBUS2_ARBIT_CFG_3rv, __VA_ARGS__)
#define RIZS_LXBUS2_ARBIT_CFG_3(...) rset(LXBUS2_ARBIT_CFG_3, 0, __VA_ARGS__)
#define RFLD_LXBUS2_ARBIT_CFG_3(fld) (*((const volatile LXBUS2_ARBIT_CFG_3_T *)0xb800003c)).f.fld

typedef union {
	struct {
		unsigned int bonding_magic_no:28; //0
		unsigned int mbz_0:2; //0
		unsigned int sw_bonding_mode:2; //0
	} f;
	unsigned int v;
} VOIP_BONDING_CTRL_T;
#define VOIP_BONDING_CTRLrv (*((regval)0xb8000040))
#define VOIP_BONDING_CTRLdv (0x00000000)
#define RMOD_VOIP_BONDING_CTRL(...) rset(VOIP_BONDING_CTRL, VOIP_BONDING_CTRLrv, __VA_ARGS__)
#define RIZS_VOIP_BONDING_CTRL(...) rset(VOIP_BONDING_CTRL, 0, __VA_ARGS__)
#define RFLD_VOIP_BONDING_CTRL(fld) (*((const volatile VOIP_BONDING_CTRL_T *)0xb8000040)).f.fld

typedef union {
	struct {
		unsigned int no_use31:21; //0
		unsigned int no_use10:1; //0
		unsigned int no_use09:1; //0
		unsigned int no_use08:1; //0
		unsigned int mbz_0:1; //0
		unsigned int no_use06:1; //0
		unsigned int no_use05:1; //0
		unsigned int cf_cksel_lx:1; //1
		unsigned int no_use03:1; //0
		unsigned int cf_ckse_ocp0:1; //1
		unsigned int rdy_for_pathch:1; //0
		unsigned int soc_init_rdy:1; //0
	} f;
	unsigned int v;
} SYS_STATUS_T;
#define SYS_STATUSrv (*((regval)0xb8000044))
#define SYS_STATUSdv (0x00000014)
#define RMOD_SYS_STATUS(...) rset(SYS_STATUS, SYS_STATUSrv, __VA_ARGS__)
#define RIZS_SYS_STATUS(...) rset(SYS_STATUS, 0, __VA_ARGS__)
#define RFLD_SYS_STATUS(fld) (*((const volatile SYS_STATUS_T *)0xb8000044)).f.fld

typedef union {
	struct {
		unsigned int ltoc:32; //0
	} f;
	unsigned int v;
} LXBUS_TO_CTRL_T;
#define LXBUS_TO_CTRLrv (*((regval)0xb8000048))
#define LXBUS_TO_CTRLdv (0x00000000)
#define RMOD_LXBUS_TO_CTRL(...) rset(LXBUS_TO_CTRL, LXBUS_TO_CTRLrv, __VA_ARGS__)
#define RIZS_LXBUS_TO_CTRL(...) rset(LXBUS_TO_CTRL, 0, __VA_ARGS__)
#define RFLD_LXBUS_TO_CTRL(fld) (*((const volatile LXBUS_TO_CTRL_T *)0xb8000048)).f.fld

/*-----------------------------------------------------
 Extraced from file_PAD_CTRL.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:12; //0
		unsigned int dram_type:2; //3
		unsigned int boot_up_type:2; //0
		unsigned int no_use15:1; //0
                unsigned int cs1_mux_sel:2; //0
		unsigned int mbz_1:1; //0
		unsigned int no_use11:4; //0
		unsigned int mbz_2:1; //0
		unsigned int spi_flash_4b_en:1; //0
		unsigned int mbz_3:1; //0
		unsigned int scan_mode:1; //0
		unsigned int speed_up:1; //0
		unsigned int mbz_4:3; //0
	} f;
	unsigned int v;
} PIN_STS_T;
#define PIN_STSrv (*((regval)0xb8000100))
#define PIN_STSdv (0x000c0000)
#define RMOD_PIN_STS(...) rset(PIN_STS, PIN_STSrv, __VA_ARGS__)
#define RIZS_PIN_STS(...) rset(PIN_STS, 0, __VA_ARGS__)
#define RFLD_PIN_STS(fld) (*((const volatile PIN_STS_T *)0xb8000100)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:22; //0
		unsigned int mbz_1:4; //0
		unsigned int no_use05:6; //0
	} f;
	unsigned int v;
} PAD_CTRL_NO_USE_0104_T;
#define PAD_CTRL_NO_USE_0104rv (*((regval)0xb8000104))
#define PAD_CTRL_NO_USE_0104dv (0x00000000)
#define RMOD_PAD_CTRL_NO_USE_0104(...) rset(PAD_CTRL_NO_USE_0104, PAD_CTRL_NO_USE_0104rv, __VA_ARGS__)
#define RIZS_PAD_CTRL_NO_USE_0104(...) rset(PAD_CTRL_NO_USE_0104, 0, __VA_ARGS__)
#define RFLD_PAD_CTRL_NO_USE_0104(fld) (*((const volatile PAD_CTRL_NO_USE_0104_T *)0xb8000104)).f.fld

typedef union {
	struct {
		unsigned int no_use31:31; //0
		unsigned int mbz_0:1; //0
	} f;
	unsigned int v;
} PAD_CTRL_NO_USE_0108_T;
#define PAD_CTRL_NO_USE_0108rv (*((regval)0xb8000108))
#define PAD_CTRL_NO_USE_0108dv (0x00000000)
#define RMOD_PAD_CTRL_NO_USE_0108(...) rset(PAD_CTRL_NO_USE_0108, PAD_CTRL_NO_USE_0108rv, __VA_ARGS__)
#define RIZS_PAD_CTRL_NO_USE_0108(...) rset(PAD_CTRL_NO_USE_0108, 0, __VA_ARGS__)
#define RFLD_PAD_CTRL_NO_USE_0108(fld) (*((const volatile PAD_CTRL_NO_USE_0108_T *)0xb8000108)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:2; //0
		unsigned int no_use29:30; //0
	} f;
	unsigned int v;
} PAD_CTRL_NO_USE_010C_T;
#define PAD_CTRL_NO_USE_010Crv (*((regval)0xb800010c))
#define PAD_CTRL_NO_USE_010Cdv (0x00000000)
#define RMOD_PAD_CTRL_NO_USE_010C(...) rset(PAD_CTRL_NO_USE_010C, PAD_CTRL_NO_USE_010Crv, __VA_ARGS__)
#define RIZS_PAD_CTRL_NO_USE_010C(...) rset(PAD_CTRL_NO_USE_010C, 0, __VA_ARGS__)
#define RFLD_PAD_CTRL_NO_USE_010C(fld) (*((const volatile PAD_CTRL_NO_USE_010C_T *)0xb800010c)).f.fld

typedef union {
	struct {
		unsigned int no_use31:5; //0
		unsigned int mbz_0:27; //0
	} f;
	unsigned int v;
} PAD_CTRL_NO_USE_0110_T;
#define PAD_CTRL_NO_USE_0110rv (*((regval)0xb8000110))
#define PAD_CTRL_NO_USE_0110dv (0x00000000)
#define RMOD_PAD_CTRL_NO_USE_0110(...) rset(PAD_CTRL_NO_USE_0110, PAD_CTRL_NO_USE_0110rv, __VA_ARGS__)
#define RIZS_PAD_CTRL_NO_USE_0110(...) rset(PAD_CTRL_NO_USE_0110, 0, __VA_ARGS__)
#define RFLD_PAD_CTRL_NO_USE_0110(fld) (*((const volatile PAD_CTRL_NO_USE_0110_T *)0xb8000110)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int no_use30:15; //0
		unsigned int mbz_1:3; //0
		unsigned int no_use12:13; //0
	} f;
	unsigned int v;
} PAD_CTRL_NO_USE_0114_T;
#define PAD_CTRL_NO_USE_0114rv (*((regval)0xb8000114))
#define PAD_CTRL_NO_USE_0114dv (0x00000000)
#define RMOD_PAD_CTRL_NO_USE_0114(...) rset(PAD_CTRL_NO_USE_0114, PAD_CTRL_NO_USE_0114rv, __VA_ARGS__)
#define RIZS_PAD_CTRL_NO_USE_0114(...) rset(PAD_CTRL_NO_USE_0114, 0, __VA_ARGS__)
#define RFLD_PAD_CTRL_NO_USE_0114(fld) (*((const volatile PAD_CTRL_NO_USE_0114_T *)0xb8000114)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int dtn:2; //0
		unsigned int dtp:2; //0
		unsigned int force_mode:1; //0
		unsigned int drv:1; //0
		unsigned int nt:3; //0
		unsigned int pt:3; //0
		unsigned int plsb:2; //0
		unsigned int ttcn:3; //0
		unsigned int ttcp:3; //0
		unsigned int ttfn:3; //0
		unsigned int ttfp:3; //0
	} f;
	unsigned int v;
} DDR_MD_IOPDCR0_T;
#define DDR_MD_IOPDCR0rv (*((regval)0xb8000118))
#define DDR_MD_IOPDCR0dv (0x00000000)
#define RMOD_DDR_MD_IOPDCR0(...) rset(DDR_MD_IOPDCR0, DDR_MD_IOPDCR0rv, __VA_ARGS__)
#define RIZS_DDR_MD_IOPDCR0(...) rset(DDR_MD_IOPDCR0, 0, __VA_ARGS__)
#define RFLD_DDR_MD_IOPDCR0(fld) (*((const volatile DDR_MD_IOPDCR0_T *)0xb8000118)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int dtn:2; //0
		unsigned int dtp:2; //0
		unsigned int force_mode:1; //0
		unsigned int drv:1; //0
		unsigned int nt:3; //0
		unsigned int pt:3; //0
		unsigned int plsb:2; //0
		unsigned int ttcn:3; //0
		unsigned int ttcp:3; //0
		unsigned int ttfn:3; //0
		unsigned int ttfp:3; //0
	} f;
	unsigned int v;
} DDR_MD_IOPDCR1_T;
#define DDR_MD_IOPDCR1rv (*((regval)0xb800011c))
#define DDR_MD_IOPDCR1dv (0x00000000)
#define RMOD_DDR_MD_IOPDCR1(...) rset(DDR_MD_IOPDCR1, DDR_MD_IOPDCR1rv, __VA_ARGS__)
#define RIZS_DDR_MD_IOPDCR1(...) rset(DDR_MD_IOPDCR1, 0, __VA_ARGS__)
#define RFLD_DDR_MD_IOPDCR1(fld) (*((const volatile DDR_MD_IOPDCR1_T *)0xb800011c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int dtn:2; //0
		unsigned int dtp:2; //0
		unsigned int force_mode:1; //0
		unsigned int drv:1; //0
		unsigned int nt:3; //0
		unsigned int pt:3; //0
		unsigned int plsb:2; //0
		unsigned int ttcn:3; //0
		unsigned int ttcp:3; //0
		unsigned int ttfn:3; //0
		unsigned int ttfp:3; //0
	} f;
	unsigned int v;
} DDR_MD_IOPDCR2_T;
#define DDR_MD_IOPDCR2rv (*((regval)0xb8000120))
#define DDR_MD_IOPDCR2dv (0x00000000)
#define RMOD_DDR_MD_IOPDCR2(...) rset(DDR_MD_IOPDCR2, DDR_MD_IOPDCR2rv, __VA_ARGS__)
#define RIZS_DDR_MD_IOPDCR2(...) rset(DDR_MD_IOPDCR2, 0, __VA_ARGS__)
#define RFLD_DDR_MD_IOPDCR2(fld) (*((const volatile DDR_MD_IOPDCR2_T *)0xb8000120)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int dtn:2; //0
		unsigned int dtp:2; //0
		unsigned int force_mode:1; //0
		unsigned int drv:1; //0
		unsigned int nt:3; //0
		unsigned int pt:3; //0
		unsigned int plsb:2; //0
		unsigned int ttcn:3; //0
		unsigned int ttcp:3; //0
		unsigned int ttfn:3; //0
		unsigned int ttfp:3; //0
	} f;
	unsigned int v;
} DDR_MD_IOPDCR3_T;
#define DDR_MD_IOPDCR3rv (*((regval)0xb8000124))
#define DDR_MD_IOPDCR3dv (0x00000000)
#define RMOD_DDR_MD_IOPDCR3(...) rset(DDR_MD_IOPDCR3, DDR_MD_IOPDCR3rv, __VA_ARGS__)
#define RIZS_DDR_MD_IOPDCR3(...) rset(DDR_MD_IOPDCR3, 0, __VA_ARGS__)
#define RFLD_DDR_MD_IOPDCR3(fld) (*((const volatile DDR_MD_IOPDCR3_T *)0xb8000124)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int hdqm_drv:1; //0
		unsigned int hdqm_nt:3; //0
		unsigned int hdqm_pt:3; //0
		unsigned int hdqm_plsb:2; //0
		unsigned int hdqm_force_mode:1; //0
		unsigned int mbz_1:6; //0
		unsigned int ldqm_drv:1; //0
		unsigned int ldqm_nt:3; //0
		unsigned int ldqm_pt:3; //0
		unsigned int ldqm_plsb:2; //0
		unsigned int ldqm_force_mode:1; //0
	} f;
	unsigned int v;
} DDR_DM_IOPDCR_T;
#define DDR_DM_IOPDCRrv (*((regval)0xb8000128))
#define DDR_DM_IOPDCRdv (0x00000000)
#define RMOD_DDR_DM_IOPDCR(...) rset(DDR_DM_IOPDCR, DDR_DM_IOPDCRrv, __VA_ARGS__)
#define RIZS_DDR_DM_IOPDCR(...) rset(DDR_DM_IOPDCR, 0, __VA_ARGS__)
#define RFLD_DDR_DM_IOPDCR(fld) (*((const volatile DDR_DM_IOPDCR_T *)0xb8000128)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int ckn_drv:1; //0
		unsigned int ckn_nt:3; //0
		unsigned int ckn_pt:3; //0
		unsigned int ckn_plsb:2; //0
		unsigned int ckn_force_mode:1; //0
		unsigned int mbz_1:6; //0
		unsigned int ck_drv:1; //0
		unsigned int ck_nt:3; //0
		unsigned int ck_pt:3; //0
		unsigned int ck_plsb:2; //0
		unsigned int ck_force_mode:1; //0
	} f;
	unsigned int v;
} DDR_CK_IOPDCR_T;
#define DDR_CK_IOPDCRrv (*((regval)0xb800012c))
#define DDR_CK_IOPDCRdv (0x00000000)
#define RMOD_DDR_CK_IOPDCR(...) rset(DDR_CK_IOPDCR, DDR_CK_IOPDCRrv, __VA_ARGS__)
#define RIZS_DDR_CK_IOPDCR(...) rset(DDR_CK_IOPDCR, 0, __VA_ARGS__)
#define RFLD_DDR_CK_IOPDCR(fld) (*((const volatile DDR_CK_IOPDCR_T *)0xb800012c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int dtn:2; //0
		unsigned int dtp:2; //0
		unsigned int force_mode:1; //0
		unsigned int drv:1; //0
		unsigned int nt:3; //0
		unsigned int pt:3; //0
		unsigned int plsb:2; //0
		unsigned int ttcn:3; //0
		unsigned int ttcp:3; //0
		unsigned int ttfn:3; //0
		unsigned int ttfp:3; //0
	} f;
	unsigned int v;
} DDR_DQS_IOPDCR0_T;
#define DDR_DQS_IOPDCR0rv (*((regval)0xb8000130))
#define DDR_DQS_IOPDCR0dv (0x00000000)
#define RMOD_DDR_DQS_IOPDCR0(...) rset(DDR_DQS_IOPDCR0, DDR_DQS_IOPDCR0rv, __VA_ARGS__)
#define RIZS_DDR_DQS_IOPDCR0(...) rset(DDR_DQS_IOPDCR0, 0, __VA_ARGS__)
#define RFLD_DDR_DQS_IOPDCR0(fld) (*((const volatile DDR_DQS_IOPDCR0_T *)0xb8000130)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int dtn:2; //0
		unsigned int dtp:2; //0
		unsigned int force_mode:1; //0
		unsigned int drv:1; //0
		unsigned int nt:3; //0
		unsigned int pt:3; //0
		unsigned int plsb:2; //0
		unsigned int ttcn:3; //0
		unsigned int ttcp:3; //0
		unsigned int ttfn:3; //0
		unsigned int ttfp:3; //0
	} f;
	unsigned int v;
} DDR_DQS_IOPDCR1_T;
#define DDR_DQS_IOPDCR1rv (*((regval)0xb8000134))
#define DDR_DQS_IOPDCR1dv (0x00000000)
#define RMOD_DDR_DQS_IOPDCR1(...) rset(DDR_DQS_IOPDCR1, DDR_DQS_IOPDCR1rv, __VA_ARGS__)
#define RIZS_DDR_DQS_IOPDCR1(...) rset(DDR_DQS_IOPDCR1, 0, __VA_ARGS__)
#define RFLD_DDR_DQS_IOPDCR1(fld) (*((const volatile DDR_DQS_IOPDCR1_T *)0xb8000134)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int dtn:2; //0
		unsigned int dtp:2; //0
		unsigned int force_mode:1; //0
		unsigned int drv:1; //0
		unsigned int nt:3; //0
		unsigned int pt:3; //0
		unsigned int plsb:2; //0
		unsigned int ttcn:3; //0
		unsigned int ttcp:3; //0
		unsigned int ttfn:3; //0
		unsigned int ttfp:3; //0
	} f;
	unsigned int v;
} DDR_DQS_IOPDCR2_T;
#define DDR_DQS_IOPDCR2rv (*((regval)0xb8000138))
#define DDR_DQS_IOPDCR2dv (0x00000000)
#define RMOD_DDR_DQS_IOPDCR2(...) rset(DDR_DQS_IOPDCR2, DDR_DQS_IOPDCR2rv, __VA_ARGS__)
#define RIZS_DDR_DQS_IOPDCR2(...) rset(DDR_DQS_IOPDCR2, 0, __VA_ARGS__)
#define RFLD_DDR_DQS_IOPDCR2(fld) (*((const volatile DDR_DQS_IOPDCR2_T *)0xb8000138)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int dtn:2; //0
		unsigned int dtp:2; //0
		unsigned int force_mode:1; //0
		unsigned int drv:1; //0
		unsigned int nt:3; //0
		unsigned int pt:3; //0
		unsigned int plsb:2; //0
		unsigned int ttcn:3; //0
		unsigned int ttcp:3; //0
		unsigned int ttfn:3; //0
		unsigned int ttfp:3; //0
	} f;
	unsigned int v;
} DDR_DQS_IOPDCR3_T;
#define DDR_DQS_IOPDCR3rv (*((regval)0xb800013c))
#define DDR_DQS_IOPDCR3dv (0x00000000)
#define RMOD_DDR_DQS_IOPDCR3(...) rset(DDR_DQS_IOPDCR3, DDR_DQS_IOPDCR3rv, __VA_ARGS__)
#define RIZS_DDR_DQS_IOPDCR3(...) rset(DDR_DQS_IOPDCR3, 0, __VA_ARGS__)
#define RFLD_DDR_DQS_IOPDCR3(fld) (*((const volatile DDR_DQS_IOPDCR3_T *)0xb800013c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int misc_drv:1; //0
		unsigned int misc_nt:3; //0
		unsigned int misc_pt:3; //0
		unsigned int misc_plsb:2; //0
		unsigned int misc_force_mode:1; //0
		unsigned int mbz_1:6; //0
		unsigned int mba_drv:1; //0
		unsigned int mba_nt:3; //0
		unsigned int mba_pt:3; //0
		unsigned int mba_plsb:2; //0
		unsigned int mba_force_mode:1; //0
	} f;
	unsigned int v;
} DDR_MA_IOPDCR_T;
#define DDR_MA_IOPDCRrv (*((regval)0xb8000140))
#define DDR_MA_IOPDCRdv (0x00000000)
#define RMOD_DDR_MA_IOPDCR(...) rset(DDR_MA_IOPDCR, DDR_MA_IOPDCRrv, __VA_ARGS__)
#define RIZS_DDR_MA_IOPDCR(...) rset(DDR_MA_IOPDCR, 0, __VA_ARGS__)
#define RFLD_DDR_MA_IOPDCR(fld) (*((const volatile DDR_MA_IOPDCR_T *)0xb8000140)).f.fld

typedef union {
	struct {
		unsigned int ddr3_md:1; //0
		unsigned int ddr_cal_srst:1; //1
		unsigned int ddr_zq_pwrdn:1; //0
		unsigned int force_mode:1; //0
		unsigned int plsb:1; //0
		unsigned int vpcalen:1; //0
		unsigned int podt:3; //0
		unsigned int pocd:5; //0
		unsigned int nodt:3; //0
		unsigned int nocd:5; //0
		unsigned int cal:2; //0
		unsigned int mbz_0:3; //0
		unsigned int ddr2_dqs_se:1; //0
		unsigned int ddr_io_speed:1; //1
		unsigned int ddr_io_pwdpad3:1; //0
		unsigned int ddr3_mode:1; //0
		unsigned int ddr2_mode:1; //1
	} f;
	unsigned int v;
} DDR123_MCR_T;
#define DDR123_MCRrv (*((regval)0xb8000144))
#define DDR123_MCRdv (0x40000009)
#define RMOD_DDR123_MCR(...) rset(DDR123_MCR, DDR123_MCRrv, __VA_ARGS__)
#define RIZS_DDR123_MCR(...) rset(DDR123_MCR, 0, __VA_ARGS__)
#define RFLD_DDR123_MCR(fld) (*((const volatile DDR123_MCR_T *)0xb8000144)).f.fld

/*-----------------------------------------------------
 Extraced from file_CLK_CTRL.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:10; //0
		unsigned int ocp0_pll_div:6; //14
		unsigned int mbz_1:10; //0
		unsigned int sdpll_div:6; //14
	} f;
	unsigned int v;
} SYSPLLCTR_T;
#define SYSPLLCTRrv (*((regval)0xb8000200))
#define SYSPLLCTRdv (0x000e000e)
#define RMOD_SYSPLLCTR(...) rset(SYSPLLCTR, SYSPLLCTRrv, __VA_ARGS__)
#define RIZS_SYSPLLCTR(...) rset(SYSPLLCTR, 0, __VA_ARGS__)
#define RFLD_SYSPLLCTR(fld) (*((const volatile SYSPLLCTR_T *)0xb8000200)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int ssc_rst_n:1; //1
		unsigned int ssc_test_mode:2; //0
		unsigned int ssc_offset:8; //0
		unsigned int ssc_step:6; //0
		unsigned int ssc_period:7; //0
		unsigned int ssc_en:1; //0
		unsigned int mbz_1:1; //0
		unsigned int pwrdn_pll_ddr:1; //0
		unsigned int frac_en:1; //0
		unsigned int pllddr_fupdn:1; //0
		unsigned int pllddr_psen:1; //1
		unsigned int pllddr_oeb:1; //0
	} f;
	unsigned int v;
} PLLCCR_T;
#define PLLCCRrv (*((regval)0xb8000204))
#define PLLCCRdv (0x40000002)
#define RMOD_PLLCCR(...) rset(PLLCCR, PLLCCRrv, __VA_ARGS__)
#define RIZS_PLLCCR(...) rset(PLLCCR, 0, __VA_ARGS__)
#define RFLD_PLLCCR(fld) (*((const volatile PLLCCR_T *)0xb8000204)).f.fld

typedef union {
	struct {
		unsigned int pll_ctrl:32; //118185
	} f;
	unsigned int v;
} PLL0_T;
#define PLL0rv (*((regval)0xb8000208))
#define PLL0dv (0x0001cda9)
#define RMOD_PLL0(...) rset(PLL0, PLL0rv, __VA_ARGS__)
#define RIZS_PLL0(...) rset(PLL0, 0, __VA_ARGS__)
#define RFLD_PLL0(fld) (*((const volatile PLL0_T *)0xb8000208)).f.fld

typedef union {
	struct {
		unsigned int pll_ctrl:32; //2123736121
	} f;
	unsigned int v;
} PLL1_T;
#define PLL1rv (*((regval)0xb800020c))
#define PLL1dv (0x7e95a439)
#define RMOD_PLL1(...) rset(PLL1, PLL1rv, __VA_ARGS__)
#define RIZS_PLL1(...) rset(PLL1, 0, __VA_ARGS__)
#define RFLD_PLL1(fld) (*((const volatile PLL1_T *)0xb800020c)).f.fld

typedef union {
	struct {
		unsigned int pll_ctrl:32; //15794152
	} f;
	unsigned int v;
} PLL2_T;
#define PLL2rv (*((regval)0xb8000210))
#define PLL2dv (0x00f0ffe8)
#define RMOD_PLL2(...) rset(PLL2, PLL2rv, __VA_ARGS__)
#define RIZS_PLL2(...) rset(PLL2, 0, __VA_ARGS__)
#define RFLD_PLL2(fld) (*((const volatile PLL2_T *)0xb8000210)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:14; //0
		unsigned int no_use15:18; //0
	} f;
	unsigned int v;
} CLK_CTRL_NO_USE_0218_T;
#define CLK_CTRL_NO_USE_0218rv (*((regval)0xb8000218))
#define CLK_CTRL_NO_USE_0218dv (0x00000000)
#define RMOD_CLK_CTRL_NO_USE_0218(...) rset(CLK_CTRL_NO_USE_0218, CLK_CTRL_NO_USE_0218rv, __VA_ARGS__)
#define RIZS_CLK_CTRL_NO_USE_0218(...) rset(CLK_CTRL_NO_USE_0218, 0, __VA_ARGS__)
#define RFLD_CLK_CTRL_NO_USE_0218(fld) (*((const volatile CLK_CTRL_NO_USE_0218_T *)0xb8000218)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int ddrckm90_tap:5; //0
		unsigned int mbz_1:3; //0
		unsigned int ddrckm_tap:5; //0
		unsigned int mbz_2:2; //0
		unsigned int no_use05:1; //0
		unsigned int ddrck_phs_sel:5; //0
	} f;
	unsigned int v;
} DRCKO_T;
#define DRCKOrv (*((regval)0xb800021c))
#define DRCKOdv (0x00000000)
#define RMOD_DRCKO(...) rset(DRCKO, DRCKOrv, __VA_ARGS__)
#define RIZS_DRCKO(...) rset(DRCKO, 0, __VA_ARGS__)
#define RFLD_DRCKO(fld) (*((const volatile DRCKO_T *)0xb800021c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:29; //0
		unsigned int mckg_phase_sel:3; //2
	} f;
	unsigned int v;
} MCKG_PHS_SEL_T;
#define MCKG_PHS_SELrv (*((regval)0xb8000220))
#define MCKG_PHS_SELdv (0x00000002)
#define RMOD_MCKG_PHS_SEL(...) rset(MCKG_PHS_SEL, MCKG_PHS_SELrv, __VA_ARGS__)
#define RIZS_MCKG_PHS_SEL(...) rset(MCKG_PHS_SEL, 0, __VA_ARGS__)
#define RFLD_MCKG_PHS_SEL(fld) (*((const volatile MCKG_PHS_SEL_T *)0xb8000220)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:30; //0
		unsigned int mckg_divl:2; //0
	} f;
	unsigned int v;
} MCKG_DIV_SEL_T;
#define MCKG_DIV_SELrv (*((regval)0xb8000224))
#define MCKG_DIV_SELdv (0x00000000)
#define RMOD_MCKG_DIV_SEL(...) rset(MCKG_DIV_SEL, MCKG_DIV_SELrv, __VA_ARGS__)
#define RIZS_MCKG_DIV_SEL(...) rset(MCKG_DIV_SEL, 0, __VA_ARGS__)
#define RFLD_MCKG_DIV_SEL(fld) (*((const volatile MCKG_DIV_SEL_T *)0xb8000224)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:20; //0
		unsigned int spi_flash_clk_div:4; //3
		unsigned int mbz_1:4; //0
		unsigned int lxpll_div:4; //3
	} f;
	unsigned int v;
} LX_CLK_PLL_T;
#define LX_CLK_PLLrv (*((regval)0xb8000228))
#define LX_CLK_PLLdv (0x00000303)
#define RMOD_LX_CLK_PLL(...) rset(LX_CLK_PLL, LX_CLK_PLLrv, __VA_ARGS__)
#define RIZS_LX_CLK_PLL(...) rset(LX_CLK_PLL, 0, __VA_ARGS__)
#define RFLD_LX_CLK_PLL(fld) (*((const volatile LX_CLK_PLL_T *)0xb8000228)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:31; //0
		unsigned int start_dram_clk_out:1; //1
	} f;
	unsigned int v;
} DRAM_CLK_EN_T;
#define DRAM_CLK_ENrv (*((regval)0xb800022c))
#define DRAM_CLK_ENdv (0x00000001)
#define RMOD_DRAM_CLK_EN(...) rset(DRAM_CLK_EN, DRAM_CLK_ENrv, __VA_ARGS__)
#define RIZS_DRAM_CLK_EN(...) rset(DRAM_CLK_EN, 0, __VA_ARGS__)
#define RFLD_DRAM_CLK_EN(fld) (*((const volatile DRAM_CLK_EN_T *)0xb800022c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:31; //0
		unsigned int dram_clk_dg_en:1; //0
	} f;
	unsigned int v;
} DRAM_CLK_CHG_T;
#define DRAM_CLK_CHGrv (*((regval)0xb8000230))
#define DRAM_CLK_CHGdv (0x00000000)
#define RMOD_DRAM_CLK_CHG(...) rset(DRAM_CLK_CHG, DRAM_CLK_CHGrv, __VA_ARGS__)
#define RIZS_DRAM_CLK_CHG(...) rset(DRAM_CLK_CHG, 0, __VA_ARGS__)
#define RFLD_DRAM_CLK_CHG(fld) (*((const volatile DRAM_CLK_CHG_T *)0xb8000230)).f.fld

/*-----------------------------------------------------
 Extraced from file_CMU.xml
-----------------------------------------------------*/
// CMU.h ??
typedef union {
	struct {
		unsigned int dont_care_0:14; //0
        unsigned int se_spif:1;     //0
        unsigned int dont_care_1:7; //0
		unsigned int se_dram:1;     //0
		unsigned int dont_care_2:9;  //0
	} f;
	unsigned int v;
} CMU_BC_OC0_T;
#define CMU_BC_OC0rv (*((regval)0xb8000388))
#define CMU_BC_OC0dv (0x00000000)
#define RMOD_CMU_BC_OC0(...) rset(CMU_BC_OC0, CMU_BC_OC0rv, __VA_ARGS__)
#define RIZS_CMU_BC_OC0(...) rset(CMU_BC_OC0, 0, __VA_ARGS__)
#define RFLD_CMU_BC_OC0(fld) (*((const volatile CMU_BC_OC0_T *)0xb8000388)).f.fld

typedef union {
	struct {
		unsigned int dont_care_0:22; //0
		unsigned int se_dram:1;     //0
		unsigned int dont_care_1:9;  //0
	} f;
	unsigned int v;
} CMU_BC_LX0_T;
#define CMU_BC_LX0rv (*((regval)0xb80003a8))
#define CMU_BC_LX0dv (0x00000000)
#define RMOD_CMU_BC_LX0(...) rset(CMU_BC_LX0, CMU_BC_LX0rv, __VA_ARGS__)
#define RIZS_CMU_BC_LX0(...) rset(CMU_BC_LX0, 0, __VA_ARGS__)
#define RFLD_CMU_BC_LX0(fld) (*((const volatile CMU_BC_LX0_T *)0xb80003a8)).f.fld

typedef union {
	struct {
		unsigned int dont_care_0:22; //0
		unsigned int se_dram:1;     //0
		unsigned int dont_care_1:9;  //0
	} f;
	unsigned int v;
} CMU_BC_LX1_T;
#define CMU_BC_LX1rv (*((regval)0xb80003b8))
#define CMU_BC_LX1dv (0x00000000)
#define RMOD_CMU_BC_LX1(...) rset(CMU_BC_LX1, CMU_BC_LX1rv, __VA_ARGS__)
#define RIZS_CMU_BC_LX1(...) rset(CMU_BC_LX1, 0, __VA_ARGS__)
#define RFLD_CMU_BC_LX1(fld) (*((const volatile CMU_BC_LX1_T *)0xb80003b8)).f.fld

typedef union {
	struct {
		unsigned int dont_care_0:22; //0
		unsigned int se_dram:1;     //0
		unsigned int dont_care_1:9;  //0
	} f;
	unsigned int v;
} CMU_BC_LX2_T;
#define CMU_BC_LX2rv (*((regval)0xb80003c8))
#define CMU_BC_LX2dv (0x00000000)
#define RMOD_CMU_BC_LX2(...) rset(CMU_BC_LX2, CMU_BC_LX2rv, __VA_ARGS__)
#define RIZS_CMU_BC_LX2(...) rset(CMU_BC_LX2, 0, __VA_ARGS__)
#define RFLD_CMU_BC_LX2(fld) (*((const volatile CMU_BC_LX2_T *)0xb80003c8)).f.fld

typedef union {
	struct {
		unsigned int dont_care_0:22; //0
		unsigned int se_dram:1;     //0
		unsigned int dont_care_1:9;  //0
	} f;
	unsigned int v;
} CMU_BC_LXP_T;
#define CMU_BC_LXPrv (*((regval)0xb80003d8))
#define CMU_BC_LXPdv (0x00000000)
#define RMOD_CMU_BC_LXP(...) rset(CMU_BC_LXP, CMU_BC_LXPrv, __VA_ARGS__)
#define RIZS_CMU_BC_LXP(...) rset(CMU_BC_LXP, 0, __VA_ARGS__)
#define RFLD_CMU_BC_LXP(fld) (*((const volatile CMU_BC_LXP_T *)0xb80003c8)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:2; //0
		unsigned int no_use29:7; //0
		unsigned int dll_dis:1; //0
		unsigned int dll_pd:1; //1
		unsigned int dll_dl:2; //2
		unsigned int dll_start_b:1; //1
		unsigned int dll_pfd:2; //0
		unsigned int dll_vr:4; //10
		unsigned int dll_md:3; //0
		unsigned int dll_lf:3; //4
		unsigned int dll_cp:6; //4
	} f;
	unsigned int v;
} DLL_CTRL_T;
#define DLL_CTRLrv (*((regval)0xb8000604))
#define DLL_CTRLdv (0x0034a104)
#define RMOD_DLL_CTRL(...) rset(DLL_CTRL, DLL_CTRLrv, __VA_ARGS__)
#define RIZS_DLL_CTRL(...) rset(DLL_CTRL, 0, __VA_ARGS__)
#define RFLD_DLL_CTRL(fld) (*((const volatile DLL_CTRL_T *)0xb8000604)).f.fld

typedef union {
	struct {
		unsigned int dll_mode:1; //0
		unsigned int cf_mvref0_pd_ref:1; //0
		unsigned int cf_mvref0_por_sel:2; //2
		unsigned int cf_mvref1_pd_ref:1; //0
		unsigned int cf_mvref1_por_sel:2; //2
		unsigned int cf_clk_oc1_slower_spif:1; //0
		unsigned int mbz_0:3; //0
		unsigned int pow:1;
		unsigned int bin_phs_sel:5;
		unsigned int pfd_div:6;
		unsigned int sc:3;
		unsigned int sel_ipump:4;
		unsigned int en_enewpump:1;
		unsigned int en_test:1;
	} f;
	unsigned int v;
} ANA_DLL0_T;
#define ANA_DLL0rv (*((regval)0xb8000608))
#define ANA_DLL0dv (0x24148a5e)
#define RMOD_ANA_DLL0(...) rset(ANA_DLL0, ANA_DLL0rv, __VA_ARGS__)
#define RIZS_ANA_DLL0(...) rset(ANA_DLL0, 0, __VA_ARGS__)
#define RFLD_ANA_DLL0(fld) (*((const volatile ANA_DLL0_T *)0xb8000608)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int pow:1;
		unsigned int bin_phs_sel:5;
		unsigned int pfd_div:6;
		unsigned int sc:3;
		unsigned int sel_ipump:4;
		unsigned int en_enewpump:1;
		unsigned int en_test:1;
	} f;
	unsigned int v;
} ANA_DLL1_T;
#define ANA_DLL1rv (*((regval)0xb800060c))
#define ANA_DLL1dv (0x00148a5e)
#define RMOD_ANA_DLL1(...) rset(ANA_DLL1, ANA_DLL1rv, __VA_ARGS__)
#define RIZS_ANA_DLL1(...) rset(ANA_DLL1, 0, __VA_ARGS__)
#define RFLD_ANA_DLL1(fld) (*((const volatile ANA_DLL1_T *)0xb800060c)).f.fld

typedef union {
	struct {
		unsigned int no_use_hw_rst_out_n:1; //1
		unsigned int no_use_pmd_done:1; //0
		unsigned int voip_cnt_start:1; //0
		unsigned int no_use28:2; //0
		unsigned int pcm_spd_mode:1; //0
		unsigned int mbz_0:3; //0
		unsigned int ocp_ext_dbg_sel:7; //0
		unsigned int voip_cnt:16; //0
	} f;
	unsigned int v;
} SMCR_T;
#define SMCRrv (*((regval)0xb8000610))
#define SMCRdv (0x80000000)
#define RMOD_SMCR(...) rset(SMCR, SMCRrv, __VA_ARGS__)
#define RIZS_SMCR(...) rset(SMCR, 0, __VA_ARGS__)
#define RFLD_SMCR(fld) (*((const volatile SMCR_T *)0xb8000610)).f.fld

typedef union {
	struct {
		unsigned int genr0_rst_n:1; //1
		unsigned int mini0_rst_n:1; //1
		unsigned int mini1_rst_n:1; //1
		unsigned int mini2_rst_n:1; //1
		unsigned int mbz_0:12; //0
		unsigned int genr0_wire_sel:1; //0
		unsigned int genr0_ro_sel:3; //0
		unsigned int mini0_wire_sel:1; //0
		unsigned int mini0_ro_sel:3; //0
		unsigned int mini1_wire_sel:1; //0
		unsigned int mini1_ro_sel:3; //0
		unsigned int mini2_wire_sel:1; //0
		unsigned int mini2_ro_sel:3; //0
	} f;
	unsigned int v;
} SPD_SENSOR0_T;
#define SPD_SENSOR0rv (*((regval)0xb8000618))
#define SPD_SENSOR0dv (0xf0000000)
#define RMOD_SPD_SENSOR0(...) rset(SPD_SENSOR0, SPD_SENSOR0rv, __VA_ARGS__)
#define RIZS_SPD_SENSOR0(...) rset(SPD_SENSOR0, 0, __VA_ARGS__)
#define RFLD_SPD_SENSOR0(fld) (*((const volatile SPD_SENSOR0_T *)0xb8000618)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int genr0_dss_ready:1; //0
		unsigned int genr0_wrost_go:1; //0
		unsigned int mbz_1:9; //0
		unsigned int genr0_dss_cnt_out:20; //0
	} f;
	unsigned int v;
} SPD_SENSOR1_T;
#define SPD_SENSOR1rv (*((regval)0xb800061c))
#define SPD_SENSOR1dv (0x00000000)
#define RMOD_SPD_SENSOR1(...) rset(SPD_SENSOR1, SPD_SENSOR1rv, __VA_ARGS__)
#define RIZS_SPD_SENSOR1(...) rset(SPD_SENSOR1, 0, __VA_ARGS__)
#define RFLD_SPD_SENSOR1(fld) (*((const volatile SPD_SENSOR1_T *)0xb800061c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int mini0_dss_ready:1; //0
		unsigned int mini0_wrost_go:1; //0
		unsigned int mbz_1:9; //0
		unsigned int mini0_dss_cnt_out:20; //0
	} f;
	unsigned int v;
} SPD_SENSOR2_T;
#define SPD_SENSOR2rv (*((regval)0xb8000620))
#define SPD_SENSOR2dv (0x00000000)
#define RMOD_SPD_SENSOR2(...) rset(SPD_SENSOR2, SPD_SENSOR2rv, __VA_ARGS__)
#define RIZS_SPD_SENSOR2(...) rset(SPD_SENSOR2, 0, __VA_ARGS__)
#define RFLD_SPD_SENSOR2(fld) (*((const volatile SPD_SENSOR2_T *)0xb8000620)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int mini1_dss_ready:1; //0
		unsigned int mini1_wrost_go:1; //0
		unsigned int mbz_1:9; //0
		unsigned int mini1_dss_cnt_out:20; //0
	} f;
	unsigned int v;
} SPD_SENSOR3_T;
#define SPD_SENSOR3rv (*((regval)0xb8000624))
#define SPD_SENSOR3dv (0x00000000)
#define RMOD_SPD_SENSOR3(...) rset(SPD_SENSOR3, SPD_SENSOR3rv, __VA_ARGS__)
#define RIZS_SPD_SENSOR3(...) rset(SPD_SENSOR3, 0, __VA_ARGS__)
#define RFLD_SPD_SENSOR3(fld) (*((const volatile SPD_SENSOR3_T *)0xb8000624)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int mini2_dss_ready:1; //0
		unsigned int mini2_wrost_go:1; //0
		unsigned int mbz_1:9; //0
		unsigned int mini2_dss_cnt_out:20; //0
	} f;
	unsigned int v;
} SPD_SENSOR4_T;
#define SPD_SENSOR4rv (*((regval)0xb8000628))
#define SPD_SENSOR4dv (0x00000000)
#define RMOD_SPD_SENSOR4(...) rset(SPD_SENSOR4, SPD_SENSOR4rv, __VA_ARGS__)
#define RIZS_SPD_SENSOR4(...) rset(SPD_SENSOR4, 0, __VA_ARGS__)
#define RFLD_SPD_SENSOR4(fld) (*((const volatile SPD_SENSOR4_T *)0xb8000628)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int genr_speed_en:1; //0
		unsigned int genr_dss_data:20; //0
	} f;
	unsigned int v;
} SPD_SENSOR5_T;
#define SPD_SENSOR5rv (*((regval)0xb800062c))
#define SPD_SENSOR5dv (0x00000000)
#define RMOD_SPD_SENSOR5(...) rset(SPD_SENSOR5, SPD_SENSOR5rv, __VA_ARGS__)
#define RIZS_SPD_SENSOR5(...) rset(SPD_SENSOR5, 0, __VA_ARGS__)
#define RFLD_SPD_SENSOR5(fld) (*((const volatile SPD_SENSOR5_T *)0xb800062c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int mini0_speed_en:1; //0
		unsigned int mini0_dss_data:20; //0
	} f;
	unsigned int v;
} SPD_SENSOR6_T;
#define SPD_SENSOR6rv (*((regval)0xb8000630))
#define SPD_SENSOR6dv (0x00000000)
#define RMOD_SPD_SENSOR6(...) rset(SPD_SENSOR6, SPD_SENSOR6rv, __VA_ARGS__)
#define RIZS_SPD_SENSOR6(...) rset(SPD_SENSOR6, 0, __VA_ARGS__)
#define RFLD_SPD_SENSOR6(fld) (*((const volatile SPD_SENSOR6_T *)0xb8000630)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int mini1_speed_en:1; //0
		unsigned int mini1_dss_data:20; //0
	} f;
	unsigned int v;
} SPD_SENSOR7_T;
#define SPD_SENSOR7rv (*((regval)0xb8000634))
#define SPD_SENSOR7dv (0x00000000)
#define RMOD_SPD_SENSOR7(...) rset(SPD_SENSOR7, SPD_SENSOR7rv, __VA_ARGS__)
#define RIZS_SPD_SENSOR7(...) rset(SPD_SENSOR7, 0, __VA_ARGS__)
#define RFLD_SPD_SENSOR7(fld) (*((const volatile SPD_SENSOR7_T *)0xb8000634)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int mini2_speed_en:1; //0
		unsigned int mini2_dss_data:20; //0
	} f;
	unsigned int v;
} SPD_SENSOR8_T;
#define SPD_SENSOR8rv (*((regval)0xb8000638))
#define SPD_SENSOR8dv (0x00000000)
#define RMOD_SPD_SENSOR8(...) rset(SPD_SENSOR8, SPD_SENSOR8rv, __VA_ARGS__)
#define RIZS_SPD_SENSOR8(...) rset(SPD_SENSOR8, 0, __VA_ARGS__)
#define RFLD_SPD_SENSOR8(fld) (*((const volatile SPD_SENSOR8_T *)0xb8000638)).f.fld

/*-----------------------------------------------------
 Extraced from file_UART.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int rbr_thr_dll:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART_PBR_THR_DLL_T;
#define UART_PBR_THR_DLLrv (*((regval)0xb8002000))
#define UART_PBR_THR_DLLdv (0x00000000)
#define RMOD_UART_PBR_THR_DLL(...) rset(UART_PBR_THR_DLL, UART_PBR_THR_DLLrv, __VA_ARGS__)
#define RIZS_UART_PBR_THR_DLL(...) rset(UART_PBR_THR_DLL, 0, __VA_ARGS__)
#define RFLD_UART_PBR_THR_DLL(fld) (*((const volatile UART_PBR_THR_DLL_T *)0xb8002000)).f.fld

typedef union {
	struct {
		unsigned int ier_dlm:8; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART_IER_DLM_T;
#define UART_IER_DLMrv (*((regval)0xb8002004))
#define UART_IER_DLMdv (0x00000000)
#define RMOD_UART_IER_DLM(...) rset(UART_IER_DLM, UART_IER_DLMrv, __VA_ARGS__)
#define RIZS_UART_IER_DLM(...) rset(UART_IER_DLM, 0, __VA_ARGS__)
#define RFLD_UART_IER_DLM(fld) (*((const volatile UART_IER_DLM_T *)0xb8002004)).f.fld

typedef union {
	struct {
		unsigned int iir_fcr_1:2; //3
		unsigned int mbz_0:2; //0
		unsigned int iir_fcr_0:4; //1
		unsigned int mbz_1:24; //0
	} f;
	unsigned int v;
} UART_IIR_FCR_T;
#define UART_IIR_FCRrv (*((regval)0xb8002008))
#define UART_IIR_FCRdv (0xc1000000)
#define RMOD_UART_IIR_FCR(...) rset(UART_IIR_FCR, UART_IIR_FCRrv, __VA_ARGS__)
#define RIZS_UART_IIR_FCR(...) rset(UART_IIR_FCR, 0, __VA_ARGS__)
#define RFLD_UART_IIR_FCR(fld) (*((const volatile UART_IIR_FCR_T *)0xb8002008)).f.fld

typedef union {
	struct {
		unsigned int dlab:1; //0
		unsigned int brk:1; //0
		unsigned int eps:2; //0
		unsigned int pen:1; //0
		unsigned int stb:1; //0
		unsigned int wls_1:1; //1
		unsigned int wls_0:1; //1
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART_LCR_T;
#define UART_LCRrv (*((regval)0xb800200c))
#define UART_LCRdv (0x03000000)
#define RMOD_UART_LCR(...) rset(UART_LCR, UART_LCRrv, __VA_ARGS__)
#define RIZS_UART_LCR(...) rset(UART_LCR, 0, __VA_ARGS__)
#define RFLD_UART_LCR(fld) (*((const volatile UART_LCR_T *)0xb800200c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int lxclk_sel:1; //0
		unsigned int afe:1; //0
		unsigned int loop:1; //0
		unsigned int out2:1; //0
		unsigned int out1:1; //0
		unsigned int rts:1; //0
		unsigned int dtr:1; //0
		unsigned int mbz_1:24; //0
	} f;
	unsigned int v;
} UART_MCR_T;
#define UART_MCRrv (*((regval)0xb8002010))
#define UART_MCRdv (0x00000000)
#define RMOD_UART_MCR(...) rset(UART_MCR, UART_MCRrv, __VA_ARGS__)
#define RIZS_UART_MCR(...) rset(UART_MCR, 0, __VA_ARGS__)
#define RFLD_UART_MCR(fld) (*((const volatile UART_MCR_T *)0xb8002010)).f.fld

typedef union {
	struct {
		unsigned int rfe:1; //0
		unsigned int temt:1; //1
		unsigned int thre:1; //1
		unsigned int bi:1; //0
		unsigned int fe:1; //0
		unsigned int pe:1; //0
		unsigned int oe:1; //0
		unsigned int dr:1; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART_LSR_T;
#define UART_LSRrv (*((regval)0xb8002014))
#define UART_LSRdv (0x60000000)
#define RMOD_UART_LSR(...) rset(UART_LSR, UART_LSRrv, __VA_ARGS__)
#define RIZS_UART_LSR(...) rset(UART_LSR, 0, __VA_ARGS__)
#define RFLD_UART_LSR(fld) (*((const volatile UART_LSR_T *)0xb8002014)).f.fld

typedef union {
	struct {
		unsigned int dcts:1; //0
		unsigned int ddsr:1; //0
		unsigned int teri:1; //0
		unsigned int ddcd:1; //1
		unsigned int cts:1; //0
		unsigned int dsr:1; //0
		unsigned int ri:1; //0
		unsigned int dcd:1; //0
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} UART_MSR_T;
#define UART_MSRrv (*((regval)0xb8002018))
#define UART_MSRdv (0x10000000)
#define RMOD_UART_MSR(...) rset(UART_MSR, UART_MSRrv, __VA_ARGS__)
#define RIZS_UART_MSR(...) rset(UART_MSR, 0, __VA_ARGS__)
#define RFLD_UART_MSR(fld) (*((const volatile UART_MSR_T *)0xb8002018)).f.fld

/*-----------------------------------------------------
 Extraced from file_INTR_CTRL.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int tmo_ie:1; //0
		unsigned int voipacc_ie:1; //0
		unsigned int spi_ie:1; //0
		unsigned int ptm_ie:1; //0
		unsigned int gmac1_ie:1; //0
		unsigned int gmac0_ie:1; //0
		unsigned int no_use25:3; //0
		unsigned int pon_ip:1; //0
		unsigned int gdma1_ie:1; //0
		unsigned int gdma0_ie:1; //0
		unsigned int no_use19:2; //0
		unsigned int pcm_ie:1; //0
		unsigned int no_use16:4; //0
		unsigned int peripheral_ie:1; //0
		unsigned int no_use11:3; //0
		unsigned int switch_ie:1; //0
		unsigned int no_use07:4; //0
		unsigned int fftacc_ie:1; //0
		unsigned int no_use02:3; //0
	} f;
	unsigned int v;
} GIMR0_T;
#define GIMR0rv (*((regval)0xb8003000))
#define GIMR0dv (0x00000000)
#define RMOD_GIMR0(...) rset(GIMR0, GIMR0rv, __VA_ARGS__)
#define RIZS_GIMR0(...) rset(GIMR0, 0, __VA_ARGS__)
#define RFLD_GIMR0(fld) (*((const volatile GIMR0_T *)0xb8003000)).f.fld

typedef union {
	struct {
		unsigned int lbctmom2_ie:1; //0
		unsigned int lbctmom1_ie:1; //0
		unsigned int lbctmom0_ie:1; //0
		unsigned int lbctmos3_ie:1; //0
		unsigned int lbctmos2_ie:1; //0
		unsigned int lbctmos1_ie:1; //0
		unsigned int lbctmos0_ie:1; //0
		unsigned int no_use24:1; //0
		unsigned int ocpto0_ie:1; //0
		unsigned int no_use22:1; //0
		unsigned int ocp0_cpu1_ila_ip:1; //0
		unsigned int uart3_ie:1; //0
		unsigned int uart2_ie:1; //0
		unsigned int uart1_ie:1; //0
		unsigned int uart0_ie:1; //0
		unsigned int tc5_ie:1; //0
		unsigned int tc4_ie:1; //0
		unsigned int tc3_ie:1; //0
		unsigned int tc2_ie:1; //0
		unsigned int tc1_ie:1; //0
		unsigned int tc0_ie:1; //0
		unsigned int gpio1_ie:1; //0
		unsigned int gpio0_ie:1; //0
		unsigned int wdt_ph2to_ie:1; //0
		unsigned int wdt_ph1to_ie:1; //0
		unsigned int tc5_dly_int_ie:1; //0
		unsigned int tc4_dly_int_ie:1; //0
		unsigned int tc3_dly_int_ie:1; //0
		unsigned int tc2_dly_int_ie:1; //0
		unsigned int tc1_dly_int_ie:1; //0
		unsigned int tc0_dly_int_ie:1; //0
		unsigned int no_use00:1; //0
	} f;
	unsigned int v;
} GIMR1_T;
#define GIMR1rv (*((regval)0xb8003004))
#define GIMR1dv (0x00000000)
#define RMOD_GIMR1(...) rset(GIMR1, GIMR1rv, __VA_ARGS__)
#define RIZS_GIMR1(...) rset(GIMR1, 0, __VA_ARGS__)
#define RFLD_GIMR1(fld) (*((const volatile GIMR1_T *)0xb8003004)).f.fld

typedef union {
	struct {
		unsigned int tmo_ip:1; //0
		unsigned int voipacc_ip:1; //0
		unsigned int spi_ip:1; //0
		unsigned int xsi_ip:1; //0
		unsigned int gmac1_ip:1; //0
		unsigned int gmac0_ip:1; //0
		unsigned int mbz_0:3; //0
		unsigned int pon_ip:1; //0
		unsigned int gdma1_ip:1; //0
		unsigned int gdma0_ip:1; //0
		unsigned int mbz_1:2; //0
		unsigned int pcm1_ip:1; //0
		unsigned int mbz_2:4; //0
		unsigned int peripheral_ip:1; //0
		unsigned int mbz_3:3; //0
		unsigned int switch_ip:1; //0
		unsigned int mbz_4:4; //0
		unsigned int fftacc_ip:1; //0
		unsigned int mbz_5:3; //0
	} f;
	unsigned int v;
} GISR0_T;
#define GISR0rv (*((regval)0xb8003008))
#define GISR0dv (0x00000000)
#define RMOD_GISR0(...) rset(GISR0, GISR0rv, __VA_ARGS__)
#define RIZS_GISR0(...) rset(GISR0, 0, __VA_ARGS__)
#define RFLD_GISR0(fld) (*((const volatile GISR0_T *)0xb8003008)).f.fld

typedef union {
	struct {
		unsigned int lbctmom2_ip:1; //0
		unsigned int lbctmom1_ip:1; //0
		unsigned int lbctmom0_ip:1; //0
		unsigned int lbctmos3_ip:1; //0
		unsigned int lbctmos2_ip:1; //0
		unsigned int lbctmos1_ip:1; //0
		unsigned int lbctmos0_ip:1; //0
		unsigned int ocpto1_ip:1; //0
		unsigned int ocpto0_ip:1; //0
		unsigned int ocp1_cpu0_ila_ip:1; //0
		unsigned int ocp0_cpu1_ila_ip:1; //0
		unsigned int uart3_ip:1; //0
		unsigned int uart2_ip:1; //0
		unsigned int uart1_ip:1; //0
		unsigned int uart0_ip:1; //0
		unsigned int tc5_ip:1; //0
		unsigned int tc4_ip:1; //0
		unsigned int tc3_ip:1; //0
		unsigned int tc2_ip:1; //0
		unsigned int tc1_ip:1; //0
		unsigned int tc0_ip:1; //0
		unsigned int gpio1_ip:1; //0
		unsigned int gpio0_ip:1; //0
		unsigned int wdt_ph2to_ip:1; //0
		unsigned int wdt_ph1to_ip:1; //0
		unsigned int tc5_dly_int_ip:1; //0
		unsigned int tc4_dly_int_ip:1; //0
		unsigned int tc3_dly_int_ip:1; //0
		unsigned int tc2_dly_int_ip:1; //0
		unsigned int tc1_dly_int_ip:1; //0
		unsigned int tc0_dly_int_ip:1; //0
		unsigned int mbz_0:1; //0
	} f;
	unsigned int v;
} GISR1_T;
#define GISR1rv (*((regval)0xb800300c))
#define GISR1dv (0x00000000)
#define RMOD_GISR1(...) rset(GISR1, GISR1rv, __VA_ARGS__)
#define RIZS_GISR1(...) rset(GISR1, 0, __VA_ARGS__)
#define RFLD_GISR1(fld) (*((const volatile GISR1_T *)0xb800300c)).f.fld

typedef union {
	struct {
		unsigned int lbctmom2_rs:4; //0
		unsigned int lbctmom1_rs:4; //0
		unsigned int lbctmom0_rs:4; //0
		unsigned int lbctmos3_rs:4; //0
		unsigned int lbctmos2_rs:4; //0
		unsigned int lbctmos1_rs:4; //0
		unsigned int lbctmos0_rs:4; //0
		unsigned int no_use03:4; //0
	} f;
	unsigned int v;
} IRR0_T;
#define IRR0rv (*((regval)0xb8003010))
#define IRR0dv (0x00000000)
#define RMOD_IRR0(...) rset(IRR0, IRR0rv, __VA_ARGS__)
#define RIZS_IRR0(...) rset(IRR0, 0, __VA_ARGS__)
#define RFLD_IRR0(fld) (*((const volatile IRR0_T *)0xb8003010)).f.fld

typedef union {
	struct {
		unsigned int ocpto0_rs:4; //0
		unsigned int no_use27:4; //0
		unsigned int ocp0_cpu1_ila_rs:4; //0
		unsigned int voipacc_rs:4; //0
		unsigned int spi_rs:4; //0
		unsigned int xsi_rs:4; //0
		unsigned int gmac1_rs:4; //0
		unsigned int gmac0_rs:4; //0
	} f;
	unsigned int v;
} IRR1_T;
#define IRR1rv (*((regval)0xb8003014))
#define IRR1dv (0x00000000)
#define RMOD_IRR1(...) rset(IRR1, IRR1rv, __VA_ARGS__)
#define RIZS_IRR1(...) rset(IRR1, 0, __VA_ARGS__)
#define RFLD_IRR1(fld) (*((const volatile IRR1_T *)0xb8003014)).f.fld

typedef union {
	struct {
		unsigned int no_use31:12; //0
		unsigned int pon_rs:4; //0
		unsigned int gdma1_rs:4; //0
		unsigned int gdma0_rs:4; //0
		unsigned int no_use07:8; //0
	} f;
	unsigned int v;
} IRR2_T;
#define IRR2rv (*((regval)0xb8003018))
#define IRR2dv (0x00000000)
#define RMOD_IRR2(...) rset(IRR2, IRR2rv, __VA_ARGS__)
#define RIZS_IRR2(...) rset(IRR2, 0, __VA_ARGS__)
#define RFLD_IRR2(fld) (*((const volatile IRR2_T *)0xb8003018)).f.fld

typedef union {
	struct {
		unsigned int pcm_rs:4; //0
		unsigned int no_use27:16; //0
		unsigned int uart3_rs:4; //0
		unsigned int uart2_rs:4; //0
		unsigned int uart1_rs:4; //0
	} f;
	unsigned int v;
} IRR3_T;
#define IRR3rv (*((regval)0xb800301c))
#define IRR3dv (0x00000000)
#define RMOD_IRR3(...) rset(IRR3, IRR3rv, __VA_ARGS__)
#define RIZS_IRR3(...) rset(IRR3, 0, __VA_ARGS__)
#define RFLD_IRR3(fld) (*((const volatile IRR3_T *)0xb800301c)).f.fld

typedef union {
	struct {
		unsigned int uart0_rs:4; //0
		unsigned int tc5_rs:4; //0
		unsigned int tc4_rs:4; //0
		unsigned int tc3_rs:4; //0
		unsigned int tc2_rs:4; //0
		unsigned int tc1_rs:4; //0
		unsigned int tc0_rs:4; //0
		unsigned int gpio1_rs:4; //0
	} f;
	unsigned int v;
} IRR4_T;
#define IRR4rv (*((regval)0xb8003020))
#define IRR4dv (0x00000000)
#define RMOD_IRR4(...) rset(IRR4, IRR4rv, __VA_ARGS__)
#define RIZS_IRR4(...) rset(IRR4, 0, __VA_ARGS__)
#define RFLD_IRR4(fld) (*((const volatile IRR4_T *)0xb8003020)).f.fld

typedef union {
	struct {
		unsigned int gpio0_rs:4; //0
		unsigned int switch_rs:4; //0
		unsigned int no_use23:4; //0
		unsigned int wdog_ph2to_rs:4; //0
		unsigned int wdog_ph1to_rs:4; //0
		unsigned int no_use11:4; //0
		unsigned int fftacc_rs:4; //0
		unsigned int no_use03:4; //0
	} f;
	unsigned int v;
} IRR5_T;
#define IRR5rv (*((regval)0xb8003024))
#define IRR5dv (0x00000000)
#define RMOD_IRR5(...) rset(IRR5, IRR5rv, __VA_ARGS__)
#define RIZS_IRR5(...) rset(IRR5, 0, __VA_ARGS__)
#define RFLD_IRR5(fld) (*((const volatile IRR5_T *)0xb8003024)).f.fld

/*-----------------------------------------------------
 Extraced from file_DLY_INTC.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int iti_trig:1; //0
		unsigned int no_use30:27; //0
		unsigned int delayed_ip_sel:4; //0
	} f;
	unsigned int v;
} TC0_DLY_INTR_T;
#define TC0_DLY_INTRrv (*((regval)0xb8003080))
#define TC0_DLY_INTRdv (0x00000000)
#define RMOD_TC0_DLY_INTR(...) rset(TC0_DLY_INTR, TC0_DLY_INTRrv, __VA_ARGS__)
#define RIZS_TC0_DLY_INTR(...) rset(TC0_DLY_INTR, 0, __VA_ARGS__)
#define RFLD_TC0_DLY_INTR(fld) (*((const volatile TC0_DLY_INTR_T *)0xb8003080)).f.fld

typedef union {
	struct {
		unsigned int iti_trig:1; //0
		unsigned int no_use30:27; //0
		unsigned int delayed_ip_sel:4; //0
	} f;
	unsigned int v;
} TC1_DLY_INTR_T;
#define TC1_DLY_INTRrv (*((regval)0xb8003084))
#define TC1_DLY_INTRdv (0x00000000)
#define RMOD_TC1_DLY_INTR(...) rset(TC1_DLY_INTR, TC1_DLY_INTRrv, __VA_ARGS__)
#define RIZS_TC1_DLY_INTR(...) rset(TC1_DLY_INTR, 0, __VA_ARGS__)
#define RFLD_TC1_DLY_INTR(fld) (*((const volatile TC1_DLY_INTR_T *)0xb8003084)).f.fld

typedef union {
	struct {
		unsigned int iti_trig:1; //0
		unsigned int no_use30:27; //0
		unsigned int delayed_ip_sel:4; //0
	} f;
	unsigned int v;
} TC2_DLY_INTR_T;
#define TC2_DLY_INTRrv (*((regval)0xb8003088))
#define TC2_DLY_INTRdv (0x00000000)
#define RMOD_TC2_DLY_INTR(...) rset(TC2_DLY_INTR, TC2_DLY_INTRrv, __VA_ARGS__)
#define RIZS_TC2_DLY_INTR(...) rset(TC2_DLY_INTR, 0, __VA_ARGS__)
#define RFLD_TC2_DLY_INTR(fld) (*((const volatile TC2_DLY_INTR_T *)0xb8003088)).f.fld

typedef union {
	struct {
		unsigned int iti_trig:1; //0
		unsigned int no_use30:27; //0
		unsigned int delayed_ip_sel:4; //0
	} f;
	unsigned int v;
} TC3_DLY_INTR_T;
#define TC3_DLY_INTRrv (*((regval)0xb800308c))
#define TC3_DLY_INTRdv (0x00000000)
#define RMOD_TC3_DLY_INTR(...) rset(TC3_DLY_INTR, TC3_DLY_INTRrv, __VA_ARGS__)
#define RIZS_TC3_DLY_INTR(...) rset(TC3_DLY_INTR, 0, __VA_ARGS__)
#define RFLD_TC3_DLY_INTR(fld) (*((const volatile TC3_DLY_INTR_T *)0xb800308c)).f.fld

typedef union {
	struct {
		unsigned int iti_trig:1; //0
		unsigned int no_use30:27; //0
		unsigned int delayed_ip_sel:4; //0
	} f;
	unsigned int v;
} TC4_DLY_INTR_T;
#define TC4_DLY_INTRrv (*((regval)0xb8003090))
#define TC4_DLY_INTRdv (0x00000000)
#define RMOD_TC4_DLY_INTR(...) rset(TC4_DLY_INTR, TC4_DLY_INTRrv, __VA_ARGS__)
#define RIZS_TC4_DLY_INTR(...) rset(TC4_DLY_INTR, 0, __VA_ARGS__)
#define RFLD_TC4_DLY_INTR(fld) (*((const volatile TC4_DLY_INTR_T *)0xb8003090)).f.fld

typedef union {
	struct {
		unsigned int iti_trig:1; //0
		unsigned int no_use30:27; //0
		unsigned int delayed_ip_sel:4; //0
	} f;
	unsigned int v;
} TC5_DLY_INTR_T;
#define TC5_DLY_INTRrv (*((regval)0xb8003094))
#define TC5_DLY_INTRdv (0x00000000)
#define RMOD_TC5_DLY_INTR(...) rset(TC5_DLY_INTR, TC5_DLY_INTRrv, __VA_ARGS__)
#define RIZS_TC5_DLY_INTR(...) rset(TC5_DLY_INTR, 0, __VA_ARGS__)
#define RFLD_TC5_DLY_INTR(fld) (*((const volatile TC5_DLY_INTR_T *)0xb8003094)).f.fld

/*-----------------------------------------------------
 Extraced from file_TIMER.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc0data:28; //0
	} f;
	unsigned int v;
} TC0DATA_T;
#define TC0DATArv (*((regval)0xb8003200))
#define TC0DATAdv (0x00000000)
#define RMOD_TC0DATA(...) rset(TC0DATA, TC0DATArv, __VA_ARGS__)
#define RIZS_TC0DATA(...) rset(TC0DATA, 0, __VA_ARGS__)
#define RFLD_TC0DATA(fld) (*((const volatile TC0DATA_T *)0xb8003200)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc0value:28; //0
	} f;
	unsigned int v;
} TC0CNT_T;
#define TC0CNTrv (*((regval)0xb8003204))
#define TC0CNTdv (0x00000000)
#define RMOD_TC0CNT(...) rset(TC0CNT, TC0CNTrv, __VA_ARGS__)
#define RIZS_TC0CNT(...) rset(TC0CNT, 0, __VA_ARGS__)
#define RFLD_TC0CNT(fld) (*((const volatile TC0CNT_T *)0xb8003204)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc0en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc0mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc0divfactor:16; //0
	} f;
	unsigned int v;
} TC0CTRL_T;
#define TC0CTRLrv (*((regval)0xb8003208))
#define TC0CTRLdv (0x00000000)
#define RMOD_TC0CTRL(...) rset(TC0CTRL, TC0CTRLrv, __VA_ARGS__)
#define RIZS_TC0CTRL(...) rset(TC0CTRL, 0, __VA_ARGS__)
#define RFLD_TC0CTRL(fld) (*((const volatile TC0CTRL_T *)0xb8003208)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc0ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc0ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TC0INTR_T;
#define TC0INTRrv (*((regval)0xb800320c))
#define TC0INTRdv (0x00000000)
#define RMOD_TC0INTR(...) rset(TC0INTR, TC0INTRrv, __VA_ARGS__)
#define RIZS_TC0INTR(...) rset(TC0INTR, 0, __VA_ARGS__)
#define RFLD_TC0INTR(fld) (*((const volatile TC0INTR_T *)0xb800320c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc1data:28; //0
	} f;
	unsigned int v;
} TC1DATA_T;
#define TC1DATArv (*((regval)0xb8003210))
#define TC1DATAdv (0x00000000)
#define RMOD_TC1DATA(...) rset(TC1DATA, TC1DATArv, __VA_ARGS__)
#define RIZS_TC1DATA(...) rset(TC1DATA, 0, __VA_ARGS__)
#define RFLD_TC1DATA(fld) (*((const volatile TC1DATA_T *)0xb8003210)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc1value:28; //0
	} f;
	unsigned int v;
} TC1CNTR_T;
#define TC1CNTRrv (*((regval)0xb8003214))
#define TC1CNTRdv (0x00000000)
#define RMOD_TC1CNTR(...) rset(TC1CNTR, TC1CNTRrv, __VA_ARGS__)
#define RIZS_TC1CNTR(...) rset(TC1CNTR, 0, __VA_ARGS__)
#define RFLD_TC1CNTR(fld) (*((const volatile TC1CNTR_T *)0xb8003214)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc1en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc1mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc1divfactor:16; //0
	} f;
	unsigned int v;
} TC1CTRL_T;
#define TC1CTRLrv (*((regval)0xb8003218))
#define TC1CTRLdv (0x00000000)
#define RMOD_TC1CTRL(...) rset(TC1CTRL, TC1CTRLrv, __VA_ARGS__)
#define RIZS_TC1CTRL(...) rset(TC1CTRL, 0, __VA_ARGS__)
#define RFLD_TC1CTRL(fld) (*((const volatile TC1CTRL_T *)0xb8003218)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc1ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc1ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TC1INTR_T;
#define TC1INTRrv (*((regval)0xb800321c))
#define TC1INTRdv (0x00000000)
#define RMOD_TC1INTR(...) rset(TC1INTR, TC1INTRrv, __VA_ARGS__)
#define RIZS_TC1INTR(...) rset(TC1INTR, 0, __VA_ARGS__)
#define RFLD_TC1INTR(fld) (*((const volatile TC1INTR_T *)0xb800321c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc1data:28; //0
	} f;
	unsigned int v;
} TC2DATA_T;
#define TC2DATArv (*((regval)0xb8003220))
#define TC2DATAdv (0x00000000)
#define RMOD_TC2DATA(...) rset(TC2DATA, TC2DATArv, __VA_ARGS__)
#define RIZS_TC2DATA(...) rset(TC2DATA, 0, __VA_ARGS__)
#define RFLD_TC2DATA(fld) (*((const volatile TC2DATA_T *)0xb8003220)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc2value:28; //0
	} f;
	unsigned int v;
} TC2CNTR_T;
#define TC2CNTRrv (*((regval)0xb8003224))
#define TC2CNTRdv (0x00000000)
#define RMOD_TC2CNTR(...) rset(TC2CNTR, TC2CNTRrv, __VA_ARGS__)
#define RIZS_TC2CNTR(...) rset(TC2CNTR, 0, __VA_ARGS__)
#define RFLD_TC2CNTR(fld) (*((const volatile TC2CNTR_T *)0xb8003224)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc2en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc2mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc2divfactor:16; //0
	} f;
	unsigned int v;
} TC2CTRL_T;
#define TC2CTRLrv (*((regval)0xb8003228))
#define TC2CTRLdv (0x00000000)
#define RMOD_TC2CTRL(...) rset(TC2CTRL, TC2CTRLrv, __VA_ARGS__)
#define RIZS_TC2CTRL(...) rset(TC2CTRL, 0, __VA_ARGS__)
#define RFLD_TC2CTRL(fld) (*((const volatile TC2CTRL_T *)0xb8003228)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc2ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc2ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TC2INTR_T;
#define TC2INTRrv (*((regval)0xb800322c))
#define TC2INTRdv (0x00000000)
#define RMOD_TC2INTR(...) rset(TC2INTR, TC2INTRrv, __VA_ARGS__)
#define RIZS_TC2INTR(...) rset(TC2INTR, 0, __VA_ARGS__)
#define RFLD_TC2INTR(fld) (*((const volatile TC2INTR_T *)0xb800322c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc1data:28; //0
	} f;
	unsigned int v;
} TC3DATA_T;
#define TC3DATArv (*((regval)0xb8003230))
#define TC3DATAdv (0x00000000)
#define RMOD_TC3DATA(...) rset(TC3DATA, TC3DATArv, __VA_ARGS__)
#define RIZS_TC3DATA(...) rset(TC3DATA, 0, __VA_ARGS__)
#define RFLD_TC3DATA(fld) (*((const volatile TC3DATA_T *)0xb8003230)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc3value:28; //0
	} f;
	unsigned int v;
} TC3CNTR_T;
#define TC3CNTRrv (*((regval)0xb8003234))
#define TC3CNTRdv (0x00000000)
#define RMOD_TC3CNTR(...) rset(TC3CNTR, TC3CNTRrv, __VA_ARGS__)
#define RIZS_TC3CNTR(...) rset(TC3CNTR, 0, __VA_ARGS__)
#define RFLD_TC3CNTR(fld) (*((const volatile TC3CNTR_T *)0xb8003234)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc3en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc3mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc3divfactor:16; //0
	} f;
	unsigned int v;
} TC3CTRL_T;
#define TC3CTRLrv (*((regval)0xb8003238))
#define TC3CTRLdv (0x00000000)
#define RMOD_TC3CTRL(...) rset(TC3CTRL, TC3CTRLrv, __VA_ARGS__)
#define RIZS_TC3CTRL(...) rset(TC3CTRL, 0, __VA_ARGS__)
#define RFLD_TC3CTRL(fld) (*((const volatile TC3CTRL_T *)0xb8003238)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc3ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc3ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TC3INTR_T;
#define TC3INTRrv (*((regval)0xb800323c))
#define TC3INTRdv (0x00000000)
#define RMOD_TC3INTR(...) rset(TC3INTR, TC3INTRrv, __VA_ARGS__)
#define RIZS_TC3INTR(...) rset(TC3INTR, 0, __VA_ARGS__)
#define RFLD_TC3INTR(fld) (*((const volatile TC3INTR_T *)0xb800323c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc1data:28; //0
	} f;
	unsigned int v;
} TC4DATA_T;
#define TC4DATArv (*((regval)0xb8003240))
#define TC4DATAdv (0x00000000)
#define RMOD_TC4DATA(...) rset(TC4DATA, TC4DATArv, __VA_ARGS__)
#define RIZS_TC4DATA(...) rset(TC4DATA, 0, __VA_ARGS__)
#define RFLD_TC4DATA(fld) (*((const volatile TC4DATA_T *)0xb8003240)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc4value:28; //0
	} f;
	unsigned int v;
} TC4CNTR_T;
#define TC4CNTRrv (*((regval)0xb8003244))
#define TC4CNTRdv (0x00000000)
#define RMOD_TC4CNTR(...) rset(TC4CNTR, TC4CNTRrv, __VA_ARGS__)
#define RIZS_TC4CNTR(...) rset(TC4CNTR, 0, __VA_ARGS__)
#define RFLD_TC4CNTR(fld) (*((const volatile TC4CNTR_T *)0xb8003244)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc4en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc4mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc4divfactor:16; //0
	} f;
	unsigned int v;
} TC4CTRL_T;
#define TC4CTRLrv (*((regval)0xb8003248))
#define TC4CTRLdv (0x00000000)
#define RMOD_TC4CTRL(...) rset(TC4CTRL, TC4CTRLrv, __VA_ARGS__)
#define RIZS_TC4CTRL(...) rset(TC4CTRL, 0, __VA_ARGS__)
#define RFLD_TC4CTRL(fld) (*((const volatile TC4CTRL_T *)0xb8003248)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc4ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc4ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TC4INTR_T;
#define TC4INTRrv (*((regval)0xb800324c))
#define TC4INTRdv (0x00000000)
#define RMOD_TC4INTR(...) rset(TC4INTR, TC4INTRrv, __VA_ARGS__)
#define RIZS_TC4INTR(...) rset(TC4INTR, 0, __VA_ARGS__)
#define RFLD_TC4INTR(fld) (*((const volatile TC4INTR_T *)0xb800324c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc1data:28; //0
	} f;
	unsigned int v;
} TC5DATA_T;
#define TC5DATArv (*((regval)0xb8003250))
#define TC5DATAdv (0x00000000)
#define RMOD_TC5DATA(...) rset(TC5DATA, TC5DATArv, __VA_ARGS__)
#define RIZS_TC5DATA(...) rset(TC5DATA, 0, __VA_ARGS__)
#define RFLD_TC5DATA(fld) (*((const volatile TC5DATA_T *)0xb8003250)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int tc5value:28; //0
	} f;
	unsigned int v;
} TC5CNTR_T;
#define TC5CNTRrv (*((regval)0xb8003254))
#define TC5CNTRdv (0x00000000)
#define RMOD_TC5CNTR(...) rset(TC5CNTR, TC5CNTRrv, __VA_ARGS__)
#define RIZS_TC5CNTR(...) rset(TC5CNTR, 0, __VA_ARGS__)
#define RFLD_TC5CNTR(fld) (*((const volatile TC5CNTR_T *)0xb8003254)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int tc5en:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc5mode:1; //0
		unsigned int mbz_2:8; //0
		unsigned int tc5divfactor:16; //0
	} f;
	unsigned int v;
} TC5CTRL_T;
#define TC5CTRLrv (*((regval)0xb8003258))
#define TC5CTRLdv (0x00000000)
#define RMOD_TC5CTRL(...) rset(TC5CTRL, TC5CTRLrv, __VA_ARGS__)
#define RIZS_TC5CTRL(...) rset(TC5CTRL, 0, __VA_ARGS__)
#define RFLD_TC5CTRL(fld) (*((const volatile TC5CTRL_T *)0xb8003258)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:11; //0
		unsigned int tc5ie:1; //0
		unsigned int mbz_1:3; //0
		unsigned int tc5ip:1; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} TC5INTR_T;
#define TC5INTRrv (*((regval)0xb800325c))
#define TC5INTRdv (0x00000000)
#define RMOD_TC5INTR(...) rset(TC5INTR, TC5INTRrv, __VA_ARGS__)
#define RIZS_TC5INTR(...) rset(TC5INTR, 0, __VA_ARGS__)
#define RFLD_TC5INTR(fld) (*((const volatile TC5INTR_T *)0xb800325c)).f.fld

typedef union {
	struct {
		unsigned int wdt_kick:1; //0
		unsigned int mbz_0:31; //0
	} f;
	unsigned int v;
} WDTCNTRR_T;
#define WDTCNTRRrv (*((regval)0xb8003260))
#define WDTCNTRRdv (0x00000000)
#define RMOD_WDTCNTRR(...) rset(WDTCNTRR, WDTCNTRRrv, __VA_ARGS__)
#define RIZS_WDTCNTRR(...) rset(WDTCNTRR, 0, __VA_ARGS__)
#define RFLD_WDTCNTRR(fld) (*((const volatile WDTCNTRR_T *)0xb8003260)).f.fld

typedef union {
	struct {
		unsigned int ph1_ip:1; //0
		unsigned int ph2_ip:1; //0
		unsigned int mbz_0:30; //0
	} f;
	unsigned int v;
} WDTINTRR_T;
#define WDTINTRRrv (*((regval)0xb8003264))
#define WDTINTRRdv (0x00000000)
#define RMOD_WDTINTRR(...) rset(WDTINTRR, WDTINTRRrv, __VA_ARGS__)
#define RIZS_WDTINTRR(...) rset(WDTINTRR, 0, __VA_ARGS__)
#define RFLD_WDTINTRR(fld) (*((const volatile WDTINTRR_T *)0xb8003264)).f.fld

typedef union {
	struct {
		unsigned int wdt_e:1; //0
		unsigned int wdt_clk_sc:2; //0
		unsigned int mbz_0:2; //0
		unsigned int ph1_to:5; //0
		unsigned int mbz_1:2; //0
		unsigned int ph2_to:5; //0
		unsigned int mbz_2:13; //0
		unsigned int wdt_reset_mode:2; //0
	} f;
	unsigned int v;
} WDT_CTRL_T;
#define WDT_CTRLrv (*((regval)0xb8003268))
#define WDT_CTRLdv (0x00000000)
#define RMOD_WDT_CTRL(...) rset(WDT_CTRL, WDT_CTRLrv, __VA_ARGS__)
#define RIZS_WDT_CTRL(...) rset(WDT_CTRL, 0, __VA_ARGS__)
#define RFLD_WDT_CTRL(fld) (*((const volatile WDT_CTRL_T *)0xb8003268)).f.fld

/*-----------------------------------------------------
 Extraced from file_GPIO.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int no_use31:32; //0
	} f;
	unsigned int v;
} GPIO_NO_USE_0004_T;
#define GPIO_NO_USE_0004rv (*((regval)0xb8003304))
#define GPIO_NO_USE_0004dv (0x00000000)
#define RMOD_GPIO_NO_USE_0004(...) rset(GPIO_NO_USE_0004, GPIO_NO_USE_0004rv, __VA_ARGS__)
#define RIZS_GPIO_NO_USE_0004(...) rset(GPIO_NO_USE_0004, 0, __VA_ARGS__)
#define RFLD_GPIO_NO_USE_0004(fld) (*((const volatile GPIO_NO_USE_0004_T *)0xb8003304)).f.fld

typedef union {
	struct {
		unsigned int drc_d:8; //0
		unsigned int drc_c:8; //0
		unsigned int drc_b:8; //0
		unsigned int drc_a:8; //0
	} f;
	unsigned int v;
} PABCD_DIR_T;
#define PABCD_DIRrv (*((regval)0xb8003308))
#define PABCD_DIRdv (0x00000000)
#define RMOD_PABCD_DIR(...) rset(PABCD_DIR, PABCD_DIRrv, __VA_ARGS__)
#define RIZS_PABCD_DIR(...) rset(PABCD_DIR, 0, __VA_ARGS__)
#define RFLD_PABCD_DIR(fld) (*((const volatile PABCD_DIR_T *)0xb8003308)).f.fld

typedef union {
	struct {
		unsigned int pd_d:8; //0
		unsigned int pd_c:8; //0
		unsigned int pd_b:8; //0
		unsigned int pd_a:8; //0
	} f;
	unsigned int v;
} PABCD_DAT_T;
#define PABCD_DATrv (*((regval)0xb800330c))
#define PABCD_DATdv (0x00000000)
#define RMOD_PABCD_DAT(...) rset(PABCD_DAT, PABCD_DATrv, __VA_ARGS__)
#define RIZS_PABCD_DAT(...) rset(PABCD_DAT, 0, __VA_ARGS__)
#define RFLD_PABCD_DAT(fld) (*((const volatile PABCD_DAT_T *)0xb800330c)).f.fld

typedef union {
	struct {
		unsigned int ips_d:8; //0
		unsigned int ips_c:8; //0
		unsigned int ips_b:8; //0
		unsigned int ips_a:8; //0
	} f;
	unsigned int v;
} PABCD_ISR_T;
#define PABCD_ISRrv (*((regval)0xb8003310))
#define PABCD_ISRdv (0x00000000)
#define RMOD_PABCD_ISR(...) rset(PABCD_ISR, PABCD_ISRrv, __VA_ARGS__)
#define RIZS_PABCD_ISR(...) rset(PABCD_ISR, 0, __VA_ARGS__)
#define RFLD_PABCD_ISR(fld) (*((const volatile PABCD_ISR_T *)0xb8003310)).f.fld

typedef union {
	struct {
		unsigned int pb7_im:2; //0
		unsigned int pb6_im:2; //0
		unsigned int pb5_im:2; //0
		unsigned int pb4_im:2; //0
		unsigned int pb3_im:2; //0
		unsigned int pb2_im:2; //0
		unsigned int pb1_im:2; //0
		unsigned int pb0_im:2; //0
		unsigned int pa7_im:2; //0
		unsigned int pa6_im:2; //0
		unsigned int pa5_im:2; //0
		unsigned int pa4_im:2; //0
		unsigned int pa3_im:2; //0
		unsigned int pa2_im:2; //0
		unsigned int pa1_im:2; //0
		unsigned int pa0_im:2; //0
	} f;
	unsigned int v;
} PAB_IMR_T;
#define PAB_IMRrv (*((regval)0xb8003314))
#define PAB_IMRdv (0x00000000)
#define RMOD_PAB_IMR(...) rset(PAB_IMR, PAB_IMRrv, __VA_ARGS__)
#define RIZS_PAB_IMR(...) rset(PAB_IMR, 0, __VA_ARGS__)
#define RFLD_PAB_IMR(fld) (*((const volatile PAB_IMR_T *)0xb8003314)).f.fld

typedef union {
	struct {
		unsigned int pd7_im:2; //0
		unsigned int pd6_im:2; //0
		unsigned int pd5_im:2; //0
		unsigned int pd4_im:2; //0
		unsigned int pd3_im:2; //0
		unsigned int pd2_im:2; //0
		unsigned int pd1_im:2; //0
		unsigned int pd0_im:2; //0
		unsigned int pc7_im:2; //0
		unsigned int pc6_im:2; //0
		unsigned int pc5_im:2; //0
		unsigned int pc4_im:2; //0
		unsigned int pc3_im:2; //0
		unsigned int pc2_im:2; //0
		unsigned int pc1_im:2; //0
		unsigned int pc0_im:2; //0
	} f;
	unsigned int v;
} PCD_IMR_T;
#define PCD_IMRrv (*((regval)0xb8003318))
#define PCD_IMRdv (0x00000000)
#define RMOD_PCD_IMR(...) rset(PCD_IMR, PCD_IMRrv, __VA_ARGS__)
#define RIZS_PCD_IMR(...) rset(PCD_IMR, 0, __VA_ARGS__)
#define RFLD_PCD_IMR(fld) (*((const volatile PCD_IMR_T *)0xb8003318)).f.fld

typedef union {
	struct {
		unsigned int no_use31:32; //0
	} f;
	unsigned int v;
} GPIO_NO_USE_0020_T;
#define GPIO_NO_USE_0020rv (*((regval)0xb8003320))
#define GPIO_NO_USE_0020dv (0x00000000)
#define RMOD_GPIO_NO_USE_0020(...) rset(GPIO_NO_USE_0020, GPIO_NO_USE_0020rv, __VA_ARGS__)
#define RIZS_GPIO_NO_USE_0020(...) rset(GPIO_NO_USE_0020, 0, __VA_ARGS__)
#define RFLD_GPIO_NO_USE_0020(fld) (*((const volatile GPIO_NO_USE_0020_T *)0xb8003320)).f.fld

typedef union {
	struct {
		unsigned int drc_h:8; //0
		unsigned int drc_g:8; //0
		unsigned int drc_f:8; //0
		unsigned int drc_e:8; //0
	} f;
	unsigned int v;
} PEFGH_DIR_T;
#define PEFGH_DIRrv (*((regval)0xb8003324))
#define PEFGH_DIRdv (0x00000000)
#define RMOD_PEFGH_DIR(...) rset(PEFGH_DIR, PEFGH_DIRrv, __VA_ARGS__)
#define RIZS_PEFGH_DIR(...) rset(PEFGH_DIR, 0, __VA_ARGS__)
#define RFLD_PEFGH_DIR(fld) (*((const volatile PEFGH_DIR_T *)0xb8003324)).f.fld

typedef union {
	struct {
		unsigned int ph_h:8; //0
		unsigned int ph_g:8; //0
		unsigned int ph_f:8; //0
		unsigned int ph_e:8; //0
	} f;
	unsigned int v;
} PEFGH_DAT_T;
#define PEFGH_DATrv (*((regval)0xb8003328))
#define PEFGH_DATdv (0x00000000)
#define RMOD_PEFGH_DAT(...) rset(PEFGH_DAT, PEFGH_DATrv, __VA_ARGS__)
#define RIZS_PEFGH_DAT(...) rset(PEFGH_DAT, 0, __VA_ARGS__)
#define RFLD_PEFGH_DAT(fld) (*((const volatile PEFGH_DAT_T *)0xb8003328)).f.fld

typedef union {
	struct {
		unsigned int ips_h:8; //0
		unsigned int ips_g:8; //0
		unsigned int ips_f:8; //0
		unsigned int ips_e:8; //0
	} f;
	unsigned int v;
} PEFGH_ISR_T;
#define PEFGH_ISRrv (*((regval)0xb800332c))
#define PEFGH_ISRdv (0x00000000)
#define RMOD_PEFGH_ISR(...) rset(PEFGH_ISR, PEFGH_ISRrv, __VA_ARGS__)
#define RIZS_PEFGH_ISR(...) rset(PEFGH_ISR, 0, __VA_ARGS__)
#define RFLD_PEFGH_ISR(fld) (*((const volatile PEFGH_ISR_T *)0xb800332c)).f.fld

typedef union {
	struct {
		unsigned int pf7_im:2; //0
		unsigned int pf6_im:2; //0
		unsigned int pf5_im:2; //0
		unsigned int pf4_im:2; //0
		unsigned int pf3_im:2; //0
		unsigned int pf2_im:2; //0
		unsigned int pf1_im:2; //0
		unsigned int pf0_im:2; //0
		unsigned int pe7_im:2; //0
		unsigned int pe6_im:2; //0
		unsigned int pe5_im:2; //0
		unsigned int pe4_im:2; //0
		unsigned int pe3_im:2; //0
		unsigned int pe2_im:2; //0
		unsigned int pe1_im:2; //0
		unsigned int pe0_im:2; //0
	} f;
	unsigned int v;
} PEF_IMR_T;
#define PEF_IMRrv (*((regval)0xb8003330))
#define PEF_IMRdv (0x00000000)
#define RMOD_PEF_IMR(...) rset(PEF_IMR, PEF_IMRrv, __VA_ARGS__)
#define RIZS_PEF_IMR(...) rset(PEF_IMR, 0, __VA_ARGS__)
#define RFLD_PEF_IMR(fld) (*((const volatile PEF_IMR_T *)0xb8003330)).f.fld

typedef union {
	struct {
		unsigned int ph7_im:2; //0
		unsigned int ph6_im:2; //0
		unsigned int ph5_im:2; //0
		unsigned int ph4_im:2; //0
		unsigned int ph3_im:2; //0
		unsigned int ph2_im:2; //0
		unsigned int ph1_im:2; //0
		unsigned int ph0_im:2; //0
		unsigned int pg7_im:2; //0
		unsigned int pg6_im:2; //0
		unsigned int pg5_im:2; //0
		unsigned int pg4_im:2; //0
		unsigned int pg3_im:2; //0
		unsigned int pg2_im:2; //0
		unsigned int pg1_im:2; //0
		unsigned int pg0_im:2; //0
	} f;
	unsigned int v;
} PGH_IMR_T;
#define PGH_IMRrv (*((regval)0xb8003334))
#define PGH_IMRdv (0x00000000)
#define RMOD_PGH_IMR(...) rset(PGH_IMR, PGH_IMRrv, __VA_ARGS__)
#define RIZS_PGH_IMR(...) rset(PGH_IMR, 0, __VA_ARGS__)
#define RFLD_PGH_IMR(fld) (*((const volatile PGH_IMR_T *)0xb8003334)).f.fld

/*-----------------------------------------------------
 Extraced from file_OB_TO_MONT.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int to_ctrl_en:1; //1
		unsigned int err_indcat:1; //0
		unsigned int to_ctrl_thr:4; //7
		unsigned int mbz_0:26; //0
	} f;
	unsigned int v;
} OBTCR_T;
#define OBTCRar             (0xB8005100)
#define OBTCRrv             (*((regval)OBTCRar))
#define OBTCRdv             (0x9c000000)
#define RMOD_OBTCR(...)     rset(OBTCR, OBTCRrv, __VA_ARGS__)
#define RIZS_OBTCR(...)     rset(OBTCR, 0, __VA_ARGS__)
#define RFLD_OBTCR(fld)     (*((const volatile OBTCR_T *)OBTCRar)).f.fld

typedef union {
	struct {
		unsigned int to_ip:2; //0
		unsigned int mbz_0:30; //0
	} f;
	unsigned int v;
} OBTIR_T;
#define OBTIRar             (0xB8005104)
#define OBTIRrv             (*((regval)OBTIRar))
#define OBTIRdv             (0x00000000)
#define RMOD_OBTIR(...)     rset(OBTIR, OBTIRrv, __VA_ARGS__)
#define RIZS_OBTIR(...)     rset(OBTIR, 0, __VA_ARGS__)
#define RFLD_OBTIR(fld)     (*((const volatile OBTIR_T *)OBTIRar)).f.fld

typedef union {
	struct {
		unsigned int to_addr:32; //0
	} f;
	unsigned int v;
} OBTMAR_T;
#define OBTMARar            (0xB8005108)
#define OBTMARrv            (*((regval)OBTMARar))
#define OBTMARdv            (0x00000000)
#define RMOD_OBTMAR(...)    rset(OBTMAR, OBTMARrv, __VA_ARGS__)
#define RIZS_OBTMAR(...)    rset(OBTMAR, 0, __VA_ARGS__)
#define RFLD_OBTMAR(fld)    (*((const volatile OBTMAR_T *)OBTMARar)).f.fld

typedef union {
	struct {
		unsigned int lx_tcen:1; //0
        unsigned int lx_tct:3; //0
        unsigned int mbz:28; //0
	} f;
	unsigned int v;
} LBPSTCR_T;
#define LBPSTCRar            (0xB8005200)
#define LBPSTCRrv            (*((regval)LBPSTCRar))
#define LBPSTCRdv            (0xF0000000)
#define RMOD_LBPSTCR(...)    rset(LBPSTCR, LBPSTCRrv, __VA_ARGS__)
#define RIZS_LBPSTCR(...)    rset(LBPSTCR, 0, __VA_ARGS__)
#define RFLD_LBPSTCR(fld)    (*((const volatile LBPSTCR_T *)LBPSTCRar)).f.fld

typedef union {
	struct {
		unsigned int lx_ip:1; //0
        unsigned int mbz:31; //0
	} f;
	unsigned int v;
} LBPSTIR_T;
#define LBPSTIRar            (0xB8005204)
#define LBPSTIRrv            (*((regval)LBPSTIRar))
#define LBPSTIRdv            (0x00000000)
#define RMOD_LBPSTIR(...)    rset(LBPSTIR, LBPSTIRrv, __VA_ARGS__)
#define RIZS_LBPSTIR(...)    rset(LBPSTIR, 0, __VA_ARGS__)
#define RFLD_LBPSTIR(fld)    (*((const volatile LBPSTIR_T *)LBPSTIRar)).f.fld

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0
	} f;
	unsigned int v;
} LBPSTMAR_T;
#define LBPSTMARar          (0xB8005208)
#define LBPSTMARrv          (*((regval)LBPSTMARar))
#define LBPSTMARdv          (0x00000000)
#define RMOD_LBPSTMAR(...)  rset(LBPSTMAR, LBPSTMARrv, __VA_ARGS__)
#define RIZS_LBPSTMAR(...)  rset(LBPSTMAR, 0, __VA_ARGS__)
#define RFLD_LBPSTMAR(fld)  (*((const volatile LBPSTMAR_T *)LBPSTMARar)).f.fld

typedef union {
	struct {
		unsigned int lx_tcen:1; //0
        unsigned int lx_tct:3; //0
        unsigned int mbz:28; //0
	} f;
	unsigned int v;
} LB0MTCR_T;
#define LB0MTCRar            (0xB8005210)
#define LB0MTCRrv            (*((regval)LB0MTCRar))
#define LB0MTCRdv            (0xF0000000)
#define RMOD_LB0MTCR(...)    rset(LB0MTCR, LB0MTCRrv, __VA_ARGS__)
#define RIZS_LB0MTCR(...)    rset(LB0MTCR, 0, __VA_ARGS__)
#define RFLD_LB0MTCR(fld)    (*((const volatile LB0MTCR_T *)LB0MTCRar)).f.fld

typedef union {
	struct {
		unsigned int lx_ip:1; //0
        unsigned int mbz:31; //0
	} f;
	unsigned int v;
} LB0MTIR_T;
#define LB0MTIRar            (0xB8005214)
#define LB0MTIRrv            (*((regval)LB0MTIRar))
#define LB0MTIRdv            (0x00000000)
#define RMOD_LB0MTIR(...)    rset(LB0MTIR, LB0MTIRrv, __VA_ARGS__)
#define RIZS_LB0MTIR(...)    rset(LB0MTIR, 0, __VA_ARGS__)
#define RFLD_LB0MTIR(fld)    (*((const volatile LB0MTIR_T *)LB0MTIRar)).f.fld

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0
	} f;
	unsigned int v;
} LB0MTMAR_T;
#define LB0MTMARar          (0xB8005218)
#define LB0MTMARrv          (*((regval)LB0MTMARar))
#define LB0MTMARdv          (0x00000000)
#define RMOD_LB0MTMAR(...)  rset(LB0MTMAR, LB0MTMARrv, __VA_ARGS__)
#define RIZS_LB0MTMAR(...)  rset(LB0MTMAR, 0, __VA_ARGS__)
#define RFLD_LB0MTMAR(fld)  (*((const volatile LB0MTMAR_T *)LB0MTMARar)).f.fld

typedef union {
	struct {
		unsigned int lx_tcen:1; //0
        unsigned int lx_tct:3; //0
        unsigned int mbz:28; //0
	} f;
	unsigned int v;
} LB0STCR_T;
#define LB0STCRar            (0xB8005220)
#define LB0STCRrv            (*((regval)LB0STCRar))
#define LB0STCRdv            (0xF0000000)
#define RMOD_LB0STCR(...)    rset(LB0STCR, LB0STCRrv, __VA_ARGS__)
#define RIZS_LB0STCR(...)    rset(LB0STCR, 0, __VA_ARGS__)
#define RFLD_LB0STCR(fld)    (*((const volatile LB0STCR_T *)LB0STCRar)).f.fld

typedef union {
	struct {
		unsigned int lx_ip:1; //0
        unsigned int mbz:31; //0
	} f;
	unsigned int v;
} LB0STIR_T;
#define LB0STIRar            (0xB8005224)
#define LB0STIRrv            (*((regval)LB0STIRar))
#define LB0STIRdv            (0x00000000)
#define RMOD_LB0STIR(...)    rset(LB0STIR, LB0STIRrv, __VA_ARGS__)
#define RIZS_LB0STIR(...)    rset(LB0STIR, 0, __VA_ARGS__)
#define RFLD_LB0STIR(fld)    (*((const volatile LB0STIR_T *)LB0STIRar)).f.fld

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0
	} f;
	unsigned int v;
} LB0STMAR_T;
#define LB0STMARar          (0xB8005228)
#define LB0STMARrv          (*((regval)LB0STMARar))
#define LB0STMARdv          (0x00000000)
#define RMOD_LB0STMAR(...)  rset(LB0STMAR, LB0STMARrv, __VA_ARGS__)
#define RIZS_LB0STMAR(...)  rset(LB0STMAR, 0, __VA_ARGS__)
#define RFLD_LB0STMAR(fld)  (*((const volatile LB0STMAR_T *)LB0STMARar)).f.fld

typedef union {
	struct {
		unsigned int lx_tcen:1; //0
        unsigned int lx_tct:3; //0
        unsigned int mbz:28; //0
	} f;
	unsigned int v;
} LB1MTCR_T;
#define LB1MTCRar            (0xB8005230)
#define LB1MTCRrv            (*((regval)LB1MTCRar))
#define LB1MTCRdv            (0xF0000000)
#define RMOD_LB1MTCR(...)    rset(LB1MTCR, LB1MTCRrv, __VA_ARGS__)
#define RIZS_LB1MTCR(...)    rset(LB1MTCR, 0, __VA_ARGS__)
#define RFLD_LB1MTCR(fld)    (*((const volatile LB1MTCR_T *)LB1MTCRar)).f.fld

typedef union {
	struct {
		unsigned int lx_ip:1; //0
        unsigned int mbz:31; //0
	} f;
	unsigned int v;
} LB1MTIR_T;
#define LB1MTIRar            (0xB8005234)
#define LB1MTIRrv            (*((regval)LB1MTIRar))
#define LB1MTIRdv            (0x00000000)
#define RMOD_LB1MTIR(...)    rset(LB1MTIR, LB1MTIRrv, __VA_ARGS__)
#define RIZS_LB1MTIR(...)    rset(LB1MTIR, 0, __VA_ARGS__)
#define RFLD_LB1MTIR(fld)    (*((const volatile LB1MTIR_T *)LB1MTIRar)).f.fld

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0
	} f;
	unsigned int v;
} LB1MTMAR_T;
#define LB1MTMARar          (0xB8005238)
#define LB1MTMARrv          (*((regval)LB1MTMARar))
#define LB1MTMARdv          (0x00000000)
#define RMOD_LB1MTMAR(...)  rset(LB1MTMAR, LB1MTMARrv, __VA_ARGS__)
#define RIZS_LB1MTMAR(...)  rset(LB1MTMAR, 0, __VA_ARGS__)
#define RFLD_LB1MTMAR(fld)  (*((const volatile LB1MTMAR_T *)LB1MTMARar)).f.fld

typedef union {
	struct {
		unsigned int lx_tcen:1; //0
        unsigned int lx_tct:3; //0
        unsigned int mbz:28; //0
	} f;
	unsigned int v;
} LB1STCR_T;
#define LB1STCRar            (0xB8005240)
#define LB1STCRrv            (*((regval)LB1STCRar))
#define LB1STCRdv            (0xF0000000)
#define RMOD_LB1STCR(...)    rset(LB1STCR, LB1STCRrv, __VA_ARGS__)
#define RIZS_LB1STCR(...)    rset(LB1STCR, 0, __VA_ARGS__)
#define RFLD_LB1STCR(fld)    (*((const volatile LB1STCR_T *)LB1STCRar)).f.fld

typedef union {
	struct {
		unsigned int lx_ip:1; //0
        unsigned int mbz:31; //0
	} f;
	unsigned int v;
} LB1STIR_T;
#define LB1STIRar            (0xB8005244)
#define LB1STIRrv            (*((regval)LB1STIRar))
#define LB1STIRdv            (0x00000000)
#define RMOD_LB1STIR(...)    rset(LB1STIR, LB1STIRrv, __VA_ARGS__)
#define RIZS_LB1STIR(...)    rset(LB1STIR, 0, __VA_ARGS__)
#define RFLD_LB1STIR(fld)    (*((const volatile LB1STIR_T *)LB1STIRar)).f.fld

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0
	} f;
	unsigned int v;
} LB1STMAR_T;
#define LB1STMARar          (0xB8005248)
#define LB1STMARrv          (*((regval)LB1STMARar))
#define LB1STMARdv          (0x00000000)
#define RMOD_LB1STMAR(...)  rset(LB1STMAR, LB1STMARrv, __VA_ARGS__)
#define RIZS_LB1STMAR(...)  rset(LB1STMAR, 0, __VA_ARGS__)
#define RFLD_LB1STMAR(fld)  (*((const volatile LB1STMAR_T *)LB1STMARar)).f.fld

typedef union {
	struct {
		unsigned int lx_tcen:1; //0
        unsigned int lx_tct:3; //0
        unsigned int mbz:28; //0
	} f;
	unsigned int v;
} LB2MTCR_T;
#define LB2MTCRar            (0xB8005250)
#define LB2MTCRrv            (*((regval)LB2MTCRar))
#define LB2MTCRdv            (0xF0000000)
#define RMOD_LB2MTCR(...)    rset(LB2MTCR, LB2MTCRrv, __VA_ARGS__)
#define RIZS_LB2MTCR(...)    rset(LB2MTCR, 0, __VA_ARGS__)
#define RFLD_LB2MTCR(fld)    (*((const volatile LB2MTCR_T *)LB2MTCRar)).f.fld

typedef union {
	struct {
		unsigned int lx_ip:1; //0
        unsigned int mbz:31; //0
	} f;
	unsigned int v;
} LB2MTIR_T;
#define LB2MTIRar            (0xB8005254)
#define LB2MTIRrv            (*((regval)LB2MTIRar))
#define LB2MTIRdv            (0x00000000)
#define RMOD_LB2MTIR(...)    rset(LB2MTIR, LB2MTIRrv, __VA_ARGS__)
#define RIZS_LB2MTIR(...)    rset(LB2MTIR, 0, __VA_ARGS__)
#define RFLD_LB2MTIR(fld)    (*((const volatile LB2MTIR_T *)LB2MTIRar)).f.fld

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0
	} f;
	unsigned int v;
} LB2MTMAR_T;
#define LB2MTMARar          (0xB8005258)
#define LB2MTMARrv          (*((regval)LB2MTMARar))
#define LB2MTMARdv          (0x00000000)
#define RMOD_LB2MTMAR(...)  rset(LB2MTMAR, LB2MTMARrv, __VA_ARGS__)
#define RIZS_LB2MTMAR(...)  rset(LB2MTMAR, 0, __VA_ARGS__)
#define RFLD_LB2MTMAR(fld)  (*((const volatile LB2MTMAR_T *)LB2MTMARar)).f.fld

typedef union {
	struct {
		unsigned int lx_tcen:1; //0
        unsigned int lx_tct:3; //0
        unsigned int mbz:28; //0
	} f;
	unsigned int v;
} LB2STCR_T;
#define LB2STCRar            (0xB8005260)
#define LB2STCRrv            (*((regval)LB2STCRar))
#define LB2STCRdv            (0xF0000000)
#define RMOD_LB2STCR(...)    rset(LB2STCR, LB2STCRrv, __VA_ARGS__)
#define RIZS_LB2STCR(...)    rset(LB2STCR, 0, __VA_ARGS__)
#define RFLD_LB2STCR(fld)    (*((const volatile LB2STCR_T *)LB2STCRar)).f.fld

typedef union {
	struct {
		unsigned int lx_ip:1; //0
        unsigned int mbz:31; //0
	} f;
	unsigned int v;
} LB2STIR_T;
#define LB2STIRar            (0xB8005264)
#define LB2STIRrv            (*((regval)LB2STIRar))
#define LB2STIRdv            (0x00000000)
#define RMOD_LB2STIR(...)    rset(LB2STIR, LB2STIRrv, __VA_ARGS__)
#define RIZS_LB2STIR(...)    rset(LB2STIR, 0, __VA_ARGS__)
#define RFLD_LB2STIR(fld)    (*((const volatile LB2STIR_T *)LB2STIRar)).f.fld

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0
	} f;
	unsigned int v;
} LB2STMAR_T;
#define LB2STMARar          (0xB8005268)
#define LB2STMARrv          (*((regval)LB2STMARar))
#define LB2STMARdv          (0x00000000)
#define RMOD_LB2STMAR(...)  rset(LB2STMAR, LB2STMARrv, __VA_ARGS__)
#define RIZS_LB2STMAR(...)  rset(LB2STMAR, 0, __VA_ARGS__)
#define RFLD_LB2STMAR(fld)  (*((const volatile LB2STMAR_T *)LB2STMARar)).f.fld

/*-----------------------------------------------------
 Extraced from file_BUS_TRAF_GEN.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int enable:1; //0
		unsigned int poll:1; //0
		unsigned int bus_traf_gen:2; //0
		unsigned int func:4; //0
		unsigned int mbz_0:1; //0
		unsigned int swap_type:1; //0
		unsigned int ent_size:2; //0
		unsigned int mbz_1:10; //0
		unsigned int dis_check_ddr:1; //0
		unsigned int debug_sel:1; //0
		unsigned int burst_size:2; //0
		unsigned int enough_thr:2; //0
		unsigned int dst_low_thr:2; //0
		unsigned int src_low_th:2; //0
	} f;
	unsigned int v;
} GDMA_CTRL_T;
#define GDMA_CTRLrv (*((regval)0xb800a000))
#define GDMA_CTRLdv (0x00000000)
#define RMOD_GDMA_CTRL(...) rset(GDMA_CTRL, GDMA_CTRLrv, __VA_ARGS__)
#define RIZS_GDMA_CTRL(...) rset(GDMA_CTRL, 0, __VA_ARGS__)
#define RFLD_GDMA_CTRL(fld) (*((const volatile GDMA_CTRL_T *)0xb800a000)).f.fld

typedef union {
	struct {
		unsigned int comp_ie:1; //0
		unsigned int mbz_0:2; //0
		unsigned int need_cpu_ie:1; //0
		unsigned int rd_pkt_ie:1; //0
		unsigned int err_rchk_ie:1; //0
		unsigned int err_rprd_ie:1; //0
		unsigned int err_rrp_ie:1; //0
		unsigned int wr_pkt_ie:1; //0
		unsigned int mbz_1:1; //0
		unsigned int err_wprd_ie:1; //0
		unsigned int err_wrp_ie:1; //0
		unsigned int mbz_2:20; //0
	} f;
	unsigned int v;
} GDMA_IM_T;
#define GDMA_IMrv (*((regval)0xb800a004))
#define GDMA_IMdv (0x00000000)
#define RMOD_GDMA_IM(...) rset(GDMA_IM, GDMA_IMrv, __VA_ARGS__)
#define RIZS_GDMA_IM(...) rset(GDMA_IM, 0, __VA_ARGS__)
#define RFLD_GDMA_IM(fld) (*((const volatile GDMA_IM_T *)0xb800a004)).f.fld

typedef union {
	struct {
		unsigned int comp_ip:1; //0
		unsigned int mbz_0:2; //0
		unsigned int need_cpu_ip:1; //0
		unsigned int rd_pkt_ip:1; //0
		unsigned int err_rchk_ip:1; //0
		unsigned int err_rprd_ip:1; //0
		unsigned int err_rrp_ip:1; //0
		unsigned int wr_pkt_ip:1; //0
		unsigned int mbz_1:1; //0
		unsigned int err_wprd_ip:1; //0
		unsigned int err_wrp_ip:1; //0
		unsigned int mbz_2:20; //0
	} f;
	unsigned int v;
} GDMA_IS_T;
#define GDMA_ISrv (*((regval)0xb800a008))
#define GDMA_ISdv (0x00000000)
#define RMOD_GDMA_IS(...) rset(GDMA_IS, GDMA_ISrv, __VA_ARGS__)
#define RIZS_GDMA_IS(...) rset(GDMA_IS, 0, __VA_ARGS__)
#define RFLD_GDMA_IS(fld) (*((const volatile GDMA_IS_T *)0xb800a008)).f.fld

typedef union {
	struct {
		unsigned int icvl:32; //0
	} f;
	unsigned int v;
} GDMA_ICVL_T;
#define GDMA_ICVLrv (*((regval)0xb800a00c))
#define GDMA_ICVLdv (0x00000000)
#define RMOD_GDMA_ICVL(...) rset(GDMA_ICVL, GDMA_ICVLrv, __VA_ARGS__)
#define RIZS_GDMA_ICVL(...) rset(GDMA_ICVL, 0, __VA_ARGS__)
#define RFLD_GDMA_ICVL(fld) (*((const volatile GDMA_ICVL_T *)0xb800a00c)).f.fld

typedef union {
	struct {
		unsigned int enable:1; //0
		unsigned int no_use30:8; //0
		unsigned int rec_err:1; //0
		unsigned int prec:1; //0
		unsigned int infinite:1; //0
		unsigned int no_use19:2; //0
		unsigned int burst_len:2; //0
		unsigned int no_use15:3; //0
		unsigned int pkt_len:8; //0
		unsigned int no_use04:5; //0
	} f;
	unsigned int v;
} BTG_WRITE_CTRL_T;
#define BTG_WRITE_CTRLrv (*((regval)0xb800a100))
#define BTG_WRITE_CTRLdv (0x00000000)
#define RMOD_BTG_WRITE_CTRL(...) rset(BTG_WRITE_CTRL, BTG_WRITE_CTRLrv, __VA_ARGS__)
#define RIZS_BTG_WRITE_CTRL(...) rset(BTG_WRITE_CTRL, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_CTRL(fld) (*((const volatile BTG_WRITE_CTRL_T *)0xb800a100)).f.fld

typedef union {
	struct {
		unsigned int iter:32; //0
	} f;
	unsigned int v;
} BTG_WRITE_ITER_T;
#define BTG_WRITE_ITERrv (*((regval)0xb800a104))
#define BTG_WRITE_ITERdv (0x00000000)
#define RMOD_BTG_WRITE_ITER(...) rset(BTG_WRITE_ITER, BTG_WRITE_ITERrv, __VA_ARGS__)
#define RIZS_BTG_WRITE_ITER(...) rset(BTG_WRITE_ITER, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_ITER(fld) (*((const volatile BTG_WRITE_ITER_T *)0xb800a104)).f.fld

typedef union {
	struct {
		unsigned int resp_time:32; //0
	} f;
	unsigned int v;
} BTG_WRITE_RESP_TIME_T;
#define BTG_WRITE_RESP_TIMErv (*((regval)0xb800a108))
#define BTG_WRITE_RESP_TIMEdv (0x00000000)
#define RMOD_BTG_WRITE_RESP_TIME(...) rset(BTG_WRITE_RESP_TIME, BTG_WRITE_RESP_TIMErv, __VA_ARGS__)
#define RIZS_BTG_WRITE_RESP_TIME(...) rset(BTG_WRITE_RESP_TIME, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_RESP_TIME(fld) (*((const volatile BTG_WRITE_RESP_TIME_T *)0xb800a108)).f.fld

typedef union {
	struct {
		unsigned int period:32; //0
	} f;
	unsigned int v;
} BTG_WRITE_PERI_T;
#define BTG_WRITE_PERIrv (*((regval)0xb800a10c))
#define BTG_WRITE_PERIdv (0x00000000)
#define RMOD_BTG_WRITE_PERI(...) rset(BTG_WRITE_PERI, BTG_WRITE_PERIrv, __VA_ARGS__)
#define RIZS_BTG_WRITE_PERI(...) rset(BTG_WRITE_PERI, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_PERI(fld) (*((const volatile BTG_WRITE_PERI_T *)0xb800a10c)).f.fld

typedef union {
	struct {
		unsigned int base_addr:32; //0
	} f;
	unsigned int v;
} BTG_WRITE_ADDR_T;
#define BTG_WRITE_ADDRrv (*((regval)0xb800a110))
#define BTG_WRITE_ADDRdv (0x00000000)
#define RMOD_BTG_WRITE_ADDR(...) rset(BTG_WRITE_ADDR, BTG_WRITE_ADDRrv, __VA_ARGS__)
#define RIZS_BTG_WRITE_ADDR(...) rset(BTG_WRITE_ADDR, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_ADDR(fld) (*((const volatile BTG_WRITE_ADDR_T *)0xb800a110)).f.fld

typedef union {
	struct {
		unsigned int addr_msk:32; //0
	} f;
	unsigned int v;
} BTG_WRITE_ADDR_MASK_T;
#define BTG_WRITE_ADDR_MASKrv (*((regval)0xb800a114))
#define BTG_WRITE_ADDR_MASKdv (0x00000000)
#define RMOD_BTG_WRITE_ADDR_MASK(...) rset(BTG_WRITE_ADDR_MASK, BTG_WRITE_ADDR_MASKrv, __VA_ARGS__)
#define RIZS_BTG_WRITE_ADDR_MASK(...) rset(BTG_WRITE_ADDR_MASK, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_ADDR_MASK(fld) (*((const volatile BTG_WRITE_ADDR_MASK_T *)0xb800a114)).f.fld

typedef union {
	struct {
		unsigned int dec_gap:1; //0
		unsigned int no_use30:15; //0
		unsigned int addr_gap:16; //0
	} f;
	unsigned int v;
} BTG_WRITE_GAP_T;
#define BTG_WRITE_GAPrv (*((regval)0xb800a118))
#define BTG_WRITE_GAPdv (0x00000000)
#define RMOD_BTG_WRITE_GAP(...) rset(BTG_WRITE_GAP, BTG_WRITE_GAPrv, __VA_ARGS__)
#define RIZS_BTG_WRITE_GAP(...) rset(BTG_WRITE_GAP, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_GAP(fld) (*((const volatile BTG_WRITE_GAP_T *)0xb800a118)).f.fld

typedef union {
	struct {
		unsigned int cur_addr:32; //0
	} f;
	unsigned int v;
} BTG_WRITE_CURR_ADDR_T;
#define BTG_WRITE_CURR_ADDRrv (*((regval)0xb800a11c))
#define BTG_WRITE_CURR_ADDRdv (0x00000000)
#define RMOD_BTG_WRITE_CURR_ADDR(...) rset(BTG_WRITE_CURR_ADDR, BTG_WRITE_CURR_ADDRrv, __VA_ARGS__)
#define RIZS_BTG_WRITE_CURR_ADDR(...) rset(BTG_WRITE_CURR_ADDR, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_CURR_ADDR(fld) (*((const volatile BTG_WRITE_CURR_ADDR_T *)0xb800a11c)).f.fld

typedef union {
	struct {
		unsigned int resp_addr:32; //0
	} f;
	unsigned int v;
} BTG_WRITE_RESP_FAIL_ADDR_T;
#define BTG_WRITE_RESP_FAIL_ADDRrv (*((regval)0xb800a120))
#define BTG_WRITE_RESP_FAIL_ADDRdv (0x00000000)
#define RMOD_BTG_WRITE_RESP_FAIL_ADDR(...) rset(BTG_WRITE_RESP_FAIL_ADDR, BTG_WRITE_RESP_FAIL_ADDRrv, __VA_ARGS__)
#define RIZS_BTG_WRITE_RESP_FAIL_ADDR(...) rset(BTG_WRITE_RESP_FAIL_ADDR, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_RESP_FAIL_ADDR(fld) (*((const volatile BTG_WRITE_RESP_FAIL_ADDR_T *)0xb800a120)).f.fld

typedef union {
	struct {
		unsigned int peri_addr:32; //0
	} f;
	unsigned int v;
} BTG_WRITE_PERI_FAIL_ADDR_T;
#define BTG_WRITE_PERI_FAIL_ADDRrv (*((regval)0xb800a124))
#define BTG_WRITE_PERI_FAIL_ADDRdv (0x00000000)
#define RMOD_BTG_WRITE_PERI_FAIL_ADDR(...) rset(BTG_WRITE_PERI_FAIL_ADDR, BTG_WRITE_PERI_FAIL_ADDRrv, __VA_ARGS__)
#define RIZS_BTG_WRITE_PERI_FAIL_ADDR(...) rset(BTG_WRITE_PERI_FAIL_ADDR, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_PERI_FAIL_ADDR(fld) (*((const volatile BTG_WRITE_PERI_FAIL_ADDR_T *)0xb800a124)).f.fld

typedef union {
	struct {
		unsigned int max_resp_time:32; //0
	} f;
	unsigned int v;
} BTG_WRITE_MAX_RESP_TIME_T;
#define BTG_WRITE_MAX_RESP_TIMErv (*((regval)0xb800a12c))
#define BTG_WRITE_MAX_RESP_TIMEdv (0x00000000)
#define RMOD_BTG_WRITE_MAX_RESP_TIME(...) rset(BTG_WRITE_MAX_RESP_TIME, BTG_WRITE_MAX_RESP_TIMErv, __VA_ARGS__)
#define RIZS_BTG_WRITE_MAX_RESP_TIME(...) rset(BTG_WRITE_MAX_RESP_TIME, 0, __VA_ARGS__)
#define RFLD_BTG_WRITE_MAX_RESP_TIME(fld) (*((const volatile BTG_WRITE_MAX_RESP_TIME_T *)0xb800a12c)).f.fld

typedef union {
	struct {
		unsigned int in_vec:32; //0
	} f;
	unsigned int v;
} BTC_WRITE_IN_VEC_T;
#define BTC_WRITE_IN_VECrv (*((regval)0xb800a130))
#define BTC_WRITE_IN_VECdv (0x00000000)
#define RMOD_BTC_WRITE_IN_VEC(...) rset(BTC_WRITE_IN_VEC, BTC_WRITE_IN_VECrv, __VA_ARGS__)
#define RIZS_BTC_WRITE_IN_VEC(...) rset(BTC_WRITE_IN_VEC, 0, __VA_ARGS__)
#define RFLD_BTC_WRITE_IN_VEC(fld) (*((const volatile BTC_WRITE_IN_VEC_T *)0xb800a130)).f.fld

typedef union {
	struct {
		unsigned int enable:1; //0
		unsigned int no_use30:5; //0
		unsigned int chk_mode:2; //0
		unsigned int no_use23:1; //0
		unsigned int rec_err:1; //0
		unsigned int prec:1; //0
		unsigned int infinite:1; //0
		unsigned int no_use19:2; //0
		unsigned int burst_len:2; //0
		unsigned int no_use15:3; //0
		unsigned int pkt_len:8; //0
		unsigned int no_use04:5; //0
	} f;
	unsigned int v;
} BTG_READ_CTRL_T;
#define BTG_READ_CTRLrv (*((regval)0xb800a200))
#define BTG_READ_CTRLdv (0x00000000)
#define RMOD_BTG_READ_CTRL(...) rset(BTG_READ_CTRL, BTG_READ_CTRLrv, __VA_ARGS__)
#define RIZS_BTG_READ_CTRL(...) rset(BTG_READ_CTRL, 0, __VA_ARGS__)
#define RFLD_BTG_READ_CTRL(fld) (*((const volatile BTG_READ_CTRL_T *)0xb800a200)).f.fld

typedef union {
	struct {
		unsigned int iter:32; //0
	} f;
	unsigned int v;
} BTG_READ_ITER_T;
#define BTG_READ_ITERrv (*((regval)0xb800a204))
#define BTG_READ_ITERdv (0x00000000)
#define RMOD_BTG_READ_ITER(...) rset(BTG_READ_ITER, BTG_READ_ITERrv, __VA_ARGS__)
#define RIZS_BTG_READ_ITER(...) rset(BTG_READ_ITER, 0, __VA_ARGS__)
#define RFLD_BTG_READ_ITER(fld) (*((const volatile BTG_READ_ITER_T *)0xb800a204)).f.fld

typedef union {
	struct {
		unsigned int resp_time:32; //0
	} f;
	unsigned int v;
} BTG_READ_RESP_TIME_T;
#define BTG_READ_RESP_TIMErv (*((regval)0xb800a208))
#define BTG_READ_RESP_TIMEdv (0x00000000)
#define RMOD_BTG_READ_RESP_TIME(...) rset(BTG_READ_RESP_TIME, BTG_READ_RESP_TIMErv, __VA_ARGS__)
#define RIZS_BTG_READ_RESP_TIME(...) rset(BTG_READ_RESP_TIME, 0, __VA_ARGS__)
#define RFLD_BTG_READ_RESP_TIME(fld) (*((const volatile BTG_READ_RESP_TIME_T *)0xb800a208)).f.fld

typedef union {
	struct {
		unsigned int period:32; //0
	} f;
	unsigned int v;
} BTG_READ_PERI_T;
#define BTG_READ_PERIrv (*((regval)0xb800a20c))
#define BTG_READ_PERIdv (0x00000000)
#define RMOD_BTG_READ_PERI(...) rset(BTG_READ_PERI, BTG_READ_PERIrv, __VA_ARGS__)
#define RIZS_BTG_READ_PERI(...) rset(BTG_READ_PERI, 0, __VA_ARGS__)
#define RFLD_BTG_READ_PERI(fld) (*((const volatile BTG_READ_PERI_T *)0xb800a20c)).f.fld

typedef union {
	struct {
		unsigned int base_addr:32; //0
	} f;
	unsigned int v;
} BTG_READ_ADDR_T;
#define BTG_READ_ADDRrv (*((regval)0xb800a210))
#define BTG_READ_ADDRdv (0x00000000)
#define RMOD_BTG_READ_ADDR(...) rset(BTG_READ_ADDR, BTG_READ_ADDRrv, __VA_ARGS__)
#define RIZS_BTG_READ_ADDR(...) rset(BTG_READ_ADDR, 0, __VA_ARGS__)
#define RFLD_BTG_READ_ADDR(fld) (*((const volatile BTG_READ_ADDR_T *)0xb800a210)).f.fld

typedef union {
	struct {
		unsigned int addr_msk:32; //0
	} f;
	unsigned int v;
} BTG_READ_ADDR_MASK_T;
#define BTG_READ_ADDR_MASKrv (*((regval)0xb800a214))
#define BTG_READ_ADDR_MASKdv (0x00000000)
#define RMOD_BTG_READ_ADDR_MASK(...) rset(BTG_READ_ADDR_MASK, BTG_READ_ADDR_MASKrv, __VA_ARGS__)
#define RIZS_BTG_READ_ADDR_MASK(...) rset(BTG_READ_ADDR_MASK, 0, __VA_ARGS__)
#define RFLD_BTG_READ_ADDR_MASK(fld) (*((const volatile BTG_READ_ADDR_MASK_T *)0xb800a214)).f.fld

typedef union {
	struct {
		unsigned int dec_gap:1; //0
		unsigned int no_use30:15; //0
		unsigned int addr_gap:16; //0
	} f;
	unsigned int v;
} BTG_READ_GAP_T;
#define BTG_READ_GAPrv (*((regval)0xb800a218))
#define BTG_READ_GAPdv (0x00000000)
#define RMOD_BTG_READ_GAP(...) rset(BTG_READ_GAP, BTG_READ_GAPrv, __VA_ARGS__)
#define RIZS_BTG_READ_GAP(...) rset(BTG_READ_GAP, 0, __VA_ARGS__)
#define RFLD_BTG_READ_GAP(fld) (*((const volatile BTG_READ_GAP_T *)0xb800a218)).f.fld

typedef union {
	struct {
		unsigned int cur_addr:32; //0
	} f;
	unsigned int v;
} BTG_READ_CURR_ADDR_T;
#define BTG_READ_CURR_ADDRrv (*((regval)0xb800a21c))
#define BTG_READ_CURR_ADDRdv (0x00000000)
#define RMOD_BTG_READ_CURR_ADDR(...) rset(BTG_READ_CURR_ADDR, BTG_READ_CURR_ADDRrv, __VA_ARGS__)
#define RIZS_BTG_READ_CURR_ADDR(...) rset(BTG_READ_CURR_ADDR, 0, __VA_ARGS__)
#define RFLD_BTG_READ_CURR_ADDR(fld) (*((const volatile BTG_READ_CURR_ADDR_T *)0xb800a21c)).f.fld

typedef union {
	struct {
		unsigned int resp_addr:32; //0
	} f;
	unsigned int v;
} BTG_READ_RESP_FAIL_ADDR_T;
#define BTG_READ_RESP_FAIL_ADDRrv (*((regval)0xb800a220))
#define BTG_READ_RESP_FAIL_ADDRdv (0x00000000)
#define RMOD_BTG_READ_RESP_FAIL_ADDR(...) rset(BTG_READ_RESP_FAIL_ADDR, BTG_READ_RESP_FAIL_ADDRrv, __VA_ARGS__)
#define RIZS_BTG_READ_RESP_FAIL_ADDR(...) rset(BTG_READ_RESP_FAIL_ADDR, 0, __VA_ARGS__)
#define RFLD_BTG_READ_RESP_FAIL_ADDR(fld) (*((const volatile BTG_READ_RESP_FAIL_ADDR_T *)0xb800a220)).f.fld

typedef union {
	struct {
		unsigned int peri_addr:32; //0
	} f;
	unsigned int v;
} BTG_READ_PERI_FAIL_ADDR_T;
#define BTG_READ_PERI_FAIL_ADDRrv (*((regval)0xb800a224))
#define BTG_READ_PERI_FAIL_ADDRdv (0x00000000)
#define RMOD_BTG_READ_PERI_FAIL_ADDR(...) rset(BTG_READ_PERI_FAIL_ADDR, BTG_READ_PERI_FAIL_ADDRrv, __VA_ARGS__)
#define RIZS_BTG_READ_PERI_FAIL_ADDR(...) rset(BTG_READ_PERI_FAIL_ADDR, 0, __VA_ARGS__)
#define RFLD_BTG_READ_PERI_FAIL_ADDR(fld) (*((const volatile BTG_READ_PERI_FAIL_ADDR_T *)0xb800a224)).f.fld

typedef union {
	struct {
		unsigned int chk_addr:32; //0
	} f;
	unsigned int v;
} BTG_READ_VERI_FAIL_ADDR_T;
#define BTG_READ_VERI_FAIL_ADDRrv (*((regval)0xb800a228))
#define BTG_READ_VERI_FAIL_ADDRdv (0x00000000)
#define RMOD_BTG_READ_VERI_FAIL_ADDR(...) rset(BTG_READ_VERI_FAIL_ADDR, BTG_READ_VERI_FAIL_ADDRrv, __VA_ARGS__)
#define RIZS_BTG_READ_VERI_FAIL_ADDR(...) rset(BTG_READ_VERI_FAIL_ADDR, 0, __VA_ARGS__)
#define RFLD_BTG_READ_VERI_FAIL_ADDR(fld) (*((const volatile BTG_READ_VERI_FAIL_ADDR_T *)0xb800a228)).f.fld

typedef union {
	struct {
		unsigned int max_resp_time:32; //0
	} f;
	unsigned int v;
} BTG_READ_MAX_RESP_TIME_T;
#define BTG_READ_MAX_RESP_TIMErv (*((regval)0xb800a22c))
#define BTG_READ_MAX_RESP_TIMEdv (0x00000000)
#define RMOD_BTG_READ_MAX_RESP_TIME(...) rset(BTG_READ_MAX_RESP_TIME, BTG_READ_MAX_RESP_TIMErv, __VA_ARGS__)
#define RIZS_BTG_READ_MAX_RESP_TIME(...) rset(BTG_READ_MAX_RESP_TIME, 0, __VA_ARGS__)
#define RFLD_BTG_READ_MAX_RESP_TIME(fld) (*((const volatile BTG_READ_MAX_RESP_TIME_T *)0xb800a22c)).f.fld

typedef union {
	struct {
		unsigned int in_vec:32; //0
	} f;
	unsigned int v;
} BTC_READ_IN_VEC0_T;
#define BTC_READ_IN_VEC0rv (*((regval)0xb800a230))
#define BTC_READ_IN_VEC0dv (0x00000000)
#define RMOD_BTC_READ_IN_VEC0(...) rset(BTC_READ_IN_VEC0, BTC_READ_IN_VEC0rv, __VA_ARGS__)
#define RIZS_BTC_READ_IN_VEC0(...) rset(BTC_READ_IN_VEC0, 0, __VA_ARGS__)
#define RFLD_BTC_READ_IN_VEC0(fld) (*((const volatile BTC_READ_IN_VEC0_T *)0xb800a230)).f.fld

typedef union {
	struct {
		unsigned int in_vec:32; //0
	} f;
	unsigned int v;
} BTC_READ_IN_VEC1_T;
#define BTC_READ_IN_VEC1rv (*((regval)0xb800a234))
#define BTC_READ_IN_VEC1dv (0x00000000)
#define RMOD_BTC_READ_IN_VEC1(...) rset(BTC_READ_IN_VEC1, BTC_READ_IN_VEC1rv, __VA_ARGS__)
#define RIZS_BTC_READ_IN_VEC1(...) rset(BTC_READ_IN_VEC1, 0, __VA_ARGS__)
#define RFLD_BTC_READ_IN_VEC1(fld) (*((const volatile BTC_READ_IN_VEC1_T *)0xb800a234)).f.fld

typedef union {
	struct {
		unsigned int in_vec:32; //0
	} f;
	unsigned int v;
} BTC_READ_IN_VEC_IDX_T;
#define BTC_READ_IN_VEC_IDXrv (*((regval)0xb800a238))
#define BTC_READ_IN_VEC_IDXdv (0x00000000)
#define RMOD_BTC_READ_IN_VEC_IDX(...) rset(BTC_READ_IN_VEC_IDX, BTC_READ_IN_VEC_IDXrv, __VA_ARGS__)
#define RIZS_BTC_READ_IN_VEC_IDX(...) rset(BTC_READ_IN_VEC_IDX, 0, __VA_ARGS__)
#define RFLD_BTC_READ_IN_VEC_IDX(fld) (*((const volatile BTC_READ_IN_VEC_IDX_T *)0xb800a238)).f.fld

/*-----------------------------------------------------
 Extraced from file_MEM_SRAM.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int lx_sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG0_ADDR_T;
#define CPU_SRAM_SEG0_ADDRrv (*((regval)0xb8004000))
#define CPU_SRAM_SEG0_ADDRdv (0x00000000)
#define RMOD_CPU_SRAM_SEG0_ADDR(...) rset(CPU_SRAM_SEG0_ADDR, CPU_SRAM_SEG0_ADDRrv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG0_ADDR(...) rset(CPU_SRAM_SEG0_ADDR, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG0_ADDR(fld) (*((const volatile CPU_SRAM_SEG0_ADDR_T *)0xb8004000)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //8
	} f;
	unsigned int v;
} CPU_SRAM_SEG0_SIZE_T;
#define CPU_SRAM_SEG0_SIZErv (*((regval)0xb8004004))
#define CPU_SRAM_SEG0_SIZEdv (0x00000008)
#define RMOD_CPU_SRAM_SEG0_SIZE(...) rset(CPU_SRAM_SEG0_SIZE, CPU_SRAM_SEG0_SIZErv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG0_SIZE(...) rset(CPU_SRAM_SEG0_SIZE, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG0_SIZE(fld) (*((const volatile CPU_SRAM_SEG0_SIZE_T *)0xb8004004)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:16; //0
		unsigned int base:8; //0
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG0_BASE_T;
#define CPU_SRAM_SEG0_BASErv (*((regval)0xb8004008))
#define CPU_SRAM_SEG0_BASEdv (0x00000000)
#define RMOD_CPU_SRAM_SEG0_BASE(...) rset(CPU_SRAM_SEG0_BASE, CPU_SRAM_SEG0_BASErv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG0_BASE(...) rset(CPU_SRAM_SEG0_BASE, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG0_BASE(fld) (*((const volatile CPU_SRAM_SEG0_BASE_T *)0xb8004008)).f.fld

typedef union {
	struct {
		unsigned int lx_sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG1_ADDR_T;
#define CPU_SRAM_SEG1_ADDRrv (*((regval)0xb8004010))
#define CPU_SRAM_SEG1_ADDRdv (0x00000000)
#define RMOD_CPU_SRAM_SEG1_ADDR(...) rset(CPU_SRAM_SEG1_ADDR, CPU_SRAM_SEG1_ADDRrv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG1_ADDR(...) rset(CPU_SRAM_SEG1_ADDR, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG1_ADDR(fld) (*((const volatile CPU_SRAM_SEG1_ADDR_T *)0xb8004010)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG1_SIZE_T;
#define CPU_SRAM_SEG1_SIZErv (*((regval)0xb8004014))
#define CPU_SRAM_SEG1_SIZEdv (0x00000000)
#define RMOD_CPU_SRAM_SEG1_SIZE(...) rset(CPU_SRAM_SEG1_SIZE, CPU_SRAM_SEG1_SIZErv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG1_SIZE(...) rset(CPU_SRAM_SEG1_SIZE, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG1_SIZE(fld) (*((const volatile CPU_SRAM_SEG1_SIZE_T *)0xb8004014)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:16; //0
		unsigned int base:8; //0
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG1_BASE_T;
#define CPU_SRAM_SEG1_BASErv (*((regval)0xb8004018))
#define CPU_SRAM_SEG1_BASEdv (0x00000000)
#define RMOD_CPU_SRAM_SEG1_BASE(...) rset(CPU_SRAM_SEG1_BASE, CPU_SRAM_SEG1_BASErv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG1_BASE(...) rset(CPU_SRAM_SEG1_BASE, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG1_BASE(fld) (*((const volatile CPU_SRAM_SEG1_BASE_T *)0xb8004018)).f.fld

typedef union {
	struct {
		unsigned int lx_sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG2_ADDR_T;
#define CPU_SRAM_SEG2_ADDRrv (*((regval)0xb8004020))
#define CPU_SRAM_SEG2_ADDRdv (0x00000000)
#define RMOD_CPU_SRAM_SEG2_ADDR(...) rset(CPU_SRAM_SEG2_ADDR, CPU_SRAM_SEG2_ADDRrv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG2_ADDR(...) rset(CPU_SRAM_SEG2_ADDR, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG2_ADDR(fld) (*((const volatile CPU_SRAM_SEG2_ADDR_T *)0xb8004020)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG2_SIZE_T;
#define CPU_SRAM_SEG2_SIZErv (*((regval)0xb8004024))
#define CPU_SRAM_SEG2_SIZEdv (0x00000000)
#define RMOD_CPU_SRAM_SEG2_SIZE(...) rset(CPU_SRAM_SEG2_SIZE, CPU_SRAM_SEG2_SIZErv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG2_SIZE(...) rset(CPU_SRAM_SEG2_SIZE, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG2_SIZE(fld) (*((const volatile CPU_SRAM_SEG2_SIZE_T *)0xb8004024)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:16; //0
		unsigned int base:8; //0
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG2_BASE_T;
#define CPU_SRAM_SEG2_BASErv (*((regval)0xb8004028))
#define CPU_SRAM_SEG2_BASEdv (0x00000000)
#define RMOD_CPU_SRAM_SEG2_BASE(...) rset(CPU_SRAM_SEG2_BASE, CPU_SRAM_SEG2_BASErv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG2_BASE(...) rset(CPU_SRAM_SEG2_BASE, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG2_BASE(fld) (*((const volatile CPU_SRAM_SEG2_BASE_T *)0xb8004028)).f.fld

typedef union {
	struct {
		unsigned int lx_sa:24; //0
		unsigned int mbz_0:7; //0
		unsigned int ensram:1; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG3_ADDR_T;
#define CPU_SRAM_SEG3_ADDRrv (*((regval)0xb8004030))
#define CPU_SRAM_SEG3_ADDRdv (0x00000000)
#define RMOD_CPU_SRAM_SEG3_ADDR(...) rset(CPU_SRAM_SEG3_ADDR, CPU_SRAM_SEG3_ADDRrv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG3_ADDR(...) rset(CPU_SRAM_SEG3_ADDR, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG3_ADDR(fld) (*((const volatile CPU_SRAM_SEG3_ADDR_T *)0xb8004030)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG3_SIZE_T;
#define CPU_SRAM_SEG3_SIZErv (*((regval)0xb8004034))
#define CPU_SRAM_SEG3_SIZEdv (0x00000000)
#define RMOD_CPU_SRAM_SEG3_SIZE(...) rset(CPU_SRAM_SEG3_SIZE, CPU_SRAM_SEG3_SIZErv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG3_SIZE(...) rset(CPU_SRAM_SEG3_SIZE, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG3_SIZE(fld) (*((const volatile CPU_SRAM_SEG3_SIZE_T *)0xb8004034)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:16; //0
		unsigned int base:8; //0
		unsigned int mbz_1:8; //0
	} f;
	unsigned int v;
} CPU_SRAM_SEG3_BASE_T;
#define CPU_SRAM_SEG3_BASErv (*((regval)0xb8004038))
#define CPU_SRAM_SEG3_BASEdv (0x00000000)
#define RMOD_CPU_SRAM_SEG3_BASE(...) rset(CPU_SRAM_SEG3_BASE, CPU_SRAM_SEG3_BASErv, __VA_ARGS__)
#define RIZS_CPU_SRAM_SEG3_BASE(...) rset(CPU_SRAM_SEG3_BASE, 0, __VA_ARGS__)
#define RFLD_CPU_SRAM_SEG3_BASE(fld) (*((const volatile CPU_SRAM_SEG3_BASE_T *)0xb8004038)).f.fld

typedef union {
        struct {
                unsigned int mbz_0:23; //0
                unsigned int sram_lx_pulse:1;
                unsigned int sram_oc1_pulse:1;
                unsigned int sram_oc0_pulse:1;
                unsigned int oc1_rom_pulse:1;
                unsigned int oc0_rom_pulse:1;
                unsigned int oc1_sram_pulse:2;
                unsigned int oc0_sram_pulse:2;
        } f;
        unsigned int v;
} CPU_SRAM_ASY_TIMING_T;
#define CPU_SRAM_ASY_TIMINGar (0xB80040F8)
#define CPU_SRAM_ASY_TIMINGrv (*((regval)CPU_SRAM_ASY_TIMINGar))
#define CPU_SRAM_ASY_TIMINGdv (0x000000C5)
#define RMOD_CPU_ASY_TIMING(...) rset(CPU_SRAM_ASY_TIMING, CPU_SRAM_ASY_TIMINGrv, __VA_ARGS__)
#define RIZS_CPU_ASY_TIMING(...) rset(CPU_SRAM_ASY_TIMING, 0, __VA_ARGS__)
#define RFLD_CPU_ASY_TIMING(fld) (*((const volatile CPU_SRAM_ASY_TIMING_T *)CPU_SRAM_ASY_TIMINGar)).f.fld

#endif  //_REGISTER_MAP_H_
