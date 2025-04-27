#ifndef _REG_MAP_ECC_H
#define _REG_MAP_ECC_H
/*-----------------------------------------------------
 Extraced from file_ECC_CTRL.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int ecc_cfg:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int debug_select:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int rbo:1; //0x0
		unsigned int wbo:1; //0x0
		unsigned int ie:1; //0x0
		unsigned int mbz_3:5; //0x0
		unsigned int slv_endian:1; //0x0
		unsigned int dma_endian:1; //0x0
		unsigned int precise:1; //0x1
		unsigned int mbz_4:10; //0x0
		unsigned int lbc_bsz:2; //0x3
	} f;
	unsigned int v;
} ECCFR_T;
#define ECCFRar (0xB801A600)
#define ECCFRdv (0x00001003)
#define ECCFRrv RVAL(ECCFR)
#define RMOD_ECCFR(...) RMOD(ECCFR, __VA_ARGS__)
#define RIZS_ECCFR(...) RIZS(ECCFR, __VA_ARGS__)
#define RFLD_ECCFR(fld) RFLD(ECCFR, fld)

typedef union {
	struct {
		unsigned int mbz_0:31; //0x0
		unsigned int dmare:1; //0x0
	} f;
	unsigned int v;
} ECDTR_T;
#define ECDTRar (0xB801A608)
#define ECDTRdv (0x00000000)
#define ECDTRrv RVAL(ECDTR)
#define RMOD_ECDTR(...) RMOD(ECDTR, __VA_ARGS__)
#define RIZS_ECDTR(...) RIZS(ECDTR, __VA_ARGS__)
#define RFLD_ECDTR(fld) RFLD(ECDTR, fld)

typedef union {
	struct {
		unsigned int addr:32; //0x0
	} f;
	unsigned int v;
} ECDSAR_T;
#define ECDSARar (0xB801A60C)
#define ECDSARdv (0x00000000)
#define ECDSARrv RVAL(ECDSAR)
#define RMOD_ECDSAR(...) RMOD(ECDSAR, __VA_ARGS__)
#define RIZS_ECDSAR(...) RIZS(ECDSAR, __VA_ARGS__)
#define RFLD_ECDSAR(fld) RFLD(ECDSAR, fld)

typedef union {
	struct {
		unsigned int addr:32; //0x0
	} f;
	unsigned int v;
} ECDTAR_T;
#define ECDTARar (0xB801A610)
#define ECDTARdv (0x00000000)
#define ECDTARrv RVAL(ECDTAR)
#define RMOD_ECDTAR(...) RMOD(ECDTAR, __VA_ARGS__)
#define RIZS_ECDTAR(...) RIZS(ECDTAR, __VA_ARGS__)
#define RFLD_ECDTAR(fld) RFLD(ECDTAR, fld)

typedef union {
	struct {
		unsigned int mbz_0:12; //0x0
		unsigned int eccn:8; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int ecer:1; //0x0
		unsigned int mbz_2:3; //0x0
		unsigned int all_one:1; //0x0
		unsigned int mbz_3:3; //0x0
		unsigned int eos:1; //0x0
	} f;
	unsigned int v;
} ECSR_T;
#define ECSRar (0xB801A614)
#define ECSRdv (0x00000000)
#define ECSRrv RVAL(ECSR)
#define RMOD_ECSR(...) RMOD(ECSR, __VA_ARGS__)
#define RIZS_ECSR(...) RIZS(ECSR, __VA_ARGS__)
#define RFLD_ECSR(fld) RFLD(ECSR, fld)

typedef union {
	struct {
		unsigned int mbz_0:12; //0x0
		unsigned int dma_ip:20; //0x0
	} f;
	unsigned int v;
} ECIR_T;
#define ECIRar (0xB801A618)
#define ECIRdv (0x00000000)
#define ECIRrv RVAL(ECIR)
#define RMOD_ECIR(...) RMOD(ECIR, __VA_ARGS__)
#define RIZS_ECIR(...) RIZS(ECIR, __VA_ARGS__)
#define RFLD_ECIR(fld) RFLD(ECIR, fld)

typedef union {
	struct {
		unsigned int mbz_0:12; //0x0
		unsigned int clenc:20; //0x0
	} f;
	unsigned int v;
} ECDCDSR_T;
#define ECDCDSRar (0xB801A61C)
#define ECDCDSRdv (0x00000000)
#define ECDCDSRrv RVAL(ECDCDSR)
#define RMOD_ECDCDSR(...) RMOD(ECDCDSR, __VA_ARGS__)
#define RIZS_ECDCDSR(...) RIZS(ECDCDSR, __VA_ARGS__)
#define RFLD_ECDCDSR(fld) RFLD(ECDCDSR, fld)

typedef union {
	struct {
		unsigned int napr1_1:32; //0x0
	} f;
	unsigned int v;
} ECNAPR1_1_T;
#define ECNAPR1_1ar (0xB801A640)
#define ECNAPR1_1dv (0x00000000)
#define ECNAPR1_1rv RVAL(ECNAPR1_1)
#define RMOD_ECNAPR1_1(...) RMOD(ECNAPR1_1, __VA_ARGS__)
#define RIZS_ECNAPR1_1(...) RIZS(ECNAPR1_1, __VA_ARGS__)
#define RFLD_ECNAPR1_1(fld) RFLD(ECNAPR1_1, fld)

typedef union {
	struct {
		unsigned int napr1_2:32; //0x0
	} f;
	unsigned int v;
} ECNAPR1_2_T;
#define ECNAPR1_2ar (0xB801A644)
#define ECNAPR1_2dv (0x00000000)
#define ECNAPR1_2rv RVAL(ECNAPR1_2)
#define RMOD_ECNAPR1_2(...) RMOD(ECNAPR1_2, __VA_ARGS__)
#define RIZS_ECNAPR1_2(...) RIZS(ECNAPR1_2, __VA_ARGS__)
#define RFLD_ECNAPR1_2(fld) RFLD(ECNAPR1_2, fld)

typedef union {
	struct {
		unsigned int napr1_3:32; //0x0
	} f;
	unsigned int v;
} ECNAPR1_3_T;
#define ECNAPR1_3ar (0xB801A648)
#define ECNAPR1_3dv (0x00000000)
#define ECNAPR1_3rv RVAL(ECNAPR1_3)
#define RMOD_ECNAPR1_3(...) RMOD(ECNAPR1_3, __VA_ARGS__)
#define RIZS_ECNAPR1_3(...) RIZS(ECNAPR1_3, __VA_ARGS__)
#define RFLD_ECNAPR1_3(fld) RFLD(ECNAPR1_3, fld)

typedef union {
	struct {
		unsigned int napr2_1:32; //0x0
	} f;
	unsigned int v;
} ECNAPR2_1_T;
#define ECNAPR2_1ar (0xB801A64C)
#define ECNAPR2_1dv (0x00000000)
#define ECNAPR2_1rv RVAL(ECNAPR2_1)
#define RMOD_ECNAPR2_1(...) RMOD(ECNAPR2_1, __VA_ARGS__)
#define RIZS_ECNAPR2_1(...) RIZS(ECNAPR2_1, __VA_ARGS__)
#define RFLD_ECNAPR2_1(fld) RFLD(ECNAPR2_1, fld)

typedef union {
	struct {
		unsigned int napr2_2:32; //0x0
	} f;
	unsigned int v;
} ECNAPR2_2_T;
#define ECNAPR2_2ar (0xB801A650)
#define ECNAPR2_2dv (0x00000000)
#define ECNAPR2_2rv RVAL(ECNAPR2_2)
#define RMOD_ECNAPR2_2(...) RMOD(ECNAPR2_2, __VA_ARGS__)
#define RIZS_ECNAPR2_2(...) RIZS(ECNAPR2_2, __VA_ARGS__)
#define RFLD_ECNAPR2_2(fld) RFLD(ECNAPR2_2, fld)

#endif // _REG_MAP_ECC_H
