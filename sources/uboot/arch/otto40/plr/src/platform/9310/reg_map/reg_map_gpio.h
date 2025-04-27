#ifndef _REG_MAP_GPIO_H
#define _REG_MAP_GPIO_H

/*-----------------------------------------------------
 Extraced from file_GPIO.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int pfc_a:8; //0xFF
		unsigned int pfc_b:8; //0xFF
		unsigned int pfc_c:8; //0xFF
		unsigned int pfc_d:8; //0xFF
	} f;
	unsigned int v;
} PABCD_CNR_T;
#define PABCD_CNRar (0xB8003300)
#define PABCD_CNRdv (0xFFFFFFFF)
#define PABCD_CNRrv RVAL(PABCD_CNR)
#define RMOD_PABCD_CNR(...) RMOD(PABCD_CNR, __VA_ARGS__)
#define RIZS_PABCD_CNR(...) RIZS(PABCD_CNR, __VA_ARGS__)
#define RFLD_PABCD_CNR(fld) RFLD(PABCD_CNR, fld)

typedef union {
	struct {
		unsigned int drc_a:8; //0x0
		unsigned int drc_b:8; //0x0
		unsigned int drc_c:8; //0x0
		unsigned int drc_d:8; //0x0
	} f;
	unsigned int v;
} PABCD_DIR_T;
#define PABCD_DIRar (0xB8003308)
#define PABCD_DIRdv (0x00000000)
#define PABCD_DIRrv RVAL(PABCD_DIR)
#define RMOD_PABCD_DIR(...) RMOD(PABCD_DIR, __VA_ARGS__)
#define RIZS_PABCD_DIR(...) RIZS(PABCD_DIR, __VA_ARGS__)
#define RFLD_PABCD_DIR(fld) RFLD(PABCD_DIR, fld)

typedef union {
	struct {
		unsigned int pd_a:8; //0x0
		unsigned int pd_b:8; //0x0
		unsigned int pd_c:8; //0x0
		unsigned int pd_d:8; //0x0
	} f;
	unsigned int v;
} PABCD_DAT_T;
#define PABCD_DATar (0xB800330C)
#define PABCD_DATdv (0x00000000)
#define PABCD_DATrv RVAL(PABCD_DAT)
#define RMOD_PABCD_DAT(...) RMOD(PABCD_DAT, __VA_ARGS__)
#define RIZS_PABCD_DAT(...) RIZS(PABCD_DAT, __VA_ARGS__)
#define RFLD_PABCD_DAT(fld) RFLD(PABCD_DAT, fld)

typedef union {
	struct {
		unsigned int ips_a:8; //0x0
		unsigned int ips_b:8; //0x0
		unsigned int ips_c:8; //0x0
		unsigned int ips_d:8; //0x0
	} f;
	unsigned int v;
} PABCD_ISR_T;
#define PABCD_ISRar (0xB8003310)
#define PABCD_ISRdv (0x00000000)
#define PABCD_ISRrv RVAL(PABCD_ISR)
#define RMOD_PABCD_ISR(...) RMOD(PABCD_ISR, __VA_ARGS__)
#define RIZS_PABCD_ISR(...) RIZS(PABCD_ISR, __VA_ARGS__)
#define RFLD_PABCD_ISR(fld) RFLD(PABCD_ISR, fld)

typedef union {
	struct {
		unsigned int pa7_im:2; //0x0
		unsigned int pa6_im:2; //0x0
		unsigned int pa5_im:2; //0x0
		unsigned int pa4_im:2; //0x0
		unsigned int pa3_im:2; //0x0
		unsigned int pa2_im:2; //0x0
		unsigned int pa1_im:2; //0x0
		unsigned int pa0_im:2; //0x0
		unsigned int pb7_im:2; //0x0
		unsigned int pb6_im:2; //0x0
		unsigned int pb5_im:2; //0x0
		unsigned int pb4_im:2; //0x0
		unsigned int pb3_im:2; //0x0
		unsigned int pb2_im:2; //0x0
		unsigned int pb1_im:2; //0x0
		unsigned int pb0_im:2; //0x0
	} f;
	unsigned int v;
} PAB_IMR_T;
#define PAB_IMRar (0xB8003314)
#define PAB_IMRdv (0x00000000)
#define PAB_IMRrv RVAL(PAB_IMR)
#define RMOD_PAB_IMR(...) RMOD(PAB_IMR, __VA_ARGS__)
#define RIZS_PAB_IMR(...) RIZS(PAB_IMR, __VA_ARGS__)
#define RFLD_PAB_IMR(fld) RFLD(PAB_IMR, fld)

typedef union {
	struct {
		unsigned int pc7_im:2; //0x0
		unsigned int pc6_im:2; //0x0
		unsigned int pc5_im:2; //0x0
		unsigned int pc4_im:2; //0x0
		unsigned int pc3_im:2; //0x0
		unsigned int pc2_im:2; //0x0
		unsigned int pc1_im:2; //0x0
		unsigned int pc0_im:2; //0x0
		unsigned int pd7_im:2; //0x0
		unsigned int pd6_im:2; //0x0
		unsigned int pd5_im:2; //0x0
		unsigned int pd4_im:2; //0x0
		unsigned int pd3_im:2; //0x0
		unsigned int pd2_im:2; //0x0
		unsigned int pd1_im:2; //0x0
		unsigned int pd0_im:2; //0x0
	} f;
	unsigned int v;
} PCD_IMR_T;
#define PCD_IMRar (0xB8003318)
#define PCD_IMRdv (0x00000000)
#define PCD_IMRrv RVAL(PCD_IMR)
#define RMOD_PCD_IMR(...) RMOD(PCD_IMR, __VA_ARGS__)
#define RIZS_PCD_IMR(...) RIZS(PCD_IMR, __VA_ARGS__)
#define RFLD_PCD_IMR(fld) RFLD(PCD_IMR, fld)

#endif // _REG_MAP_GPIO_H
