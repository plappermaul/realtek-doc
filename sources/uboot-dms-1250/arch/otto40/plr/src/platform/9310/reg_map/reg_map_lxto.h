#ifndef _REG_MAP_LXTO_H
#define _REG_MAP_LXTO_H

/*-----------------------------------------------------
 Extraced from file_LB_TO_MONT.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int lx_tcen:1; //0x1
		unsigned int lx_tct:3; //0x7
		unsigned int mbz_0:28; //0x0
	} f;
	unsigned int v;
} LB0MTCR_T;
#define LB0MTCRar (0xB8005200)
#define LB0MTCRdv (0xF0000000)
#define LB0MTCRrv RVAL(LB0MTCR)
#define RMOD_LB0MTCR(...) RMOD(LB0MTCR, __VA_ARGS__)
#define RIZS_LB0MTCR(...) RIZS(LB0MTCR, __VA_ARGS__)
#define RFLD_LB0MTCR(fld) RFLD(LB0MTCR, fld)

typedef union {
	struct {
		unsigned int lx_ip:1; //0x0
		unsigned int mbz_0:31; //0x0
	} f;
	unsigned int v;
} LB0MTIR_T;
#define LB0MTIRar (0xB8005204)
#define LB0MTIRdv (0x00000000)
#define LB0MTIRrv RVAL(LB0MTIR)
#define RMOD_LB0MTIR(...) RMOD(LB0MTIR, __VA_ARGS__)
#define RIZS_LB0MTIR(...) RIZS(LB0MTIR, __VA_ARGS__)
#define RFLD_LB0MTIR(fld) RFLD(LB0MTIR, fld)

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0x0
	} f;
	unsigned int v;
} LB0MTMAR_T;
#define LB0MTMARar (0xB8005208)
#define LB0MTMARdv (0x00000000)
#define LB0MTMARrv RVAL(LB0MTMAR)
#define RMOD_LB0MTMAR(...) RMOD(LB0MTMAR, __VA_ARGS__)
#define RIZS_LB0MTMAR(...) RIZS(LB0MTMAR, __VA_ARGS__)
#define RFLD_LB0MTMAR(fld) RFLD(LB0MTMAR, fld)

typedef union {
	struct {
		unsigned int lx_tcen:1; //0x1
		unsigned int lx_tct:3; //0x7
		unsigned int mbz_0:28; //0x0
	} f;
	unsigned int v;
} LB0STCR_T;
#define LB0STCRar (0xB8005210)
#define LB0STCRdv (0xF0000000)
#define LB0STCRrv RVAL(LB0STCR)
#define RMOD_LB0STCR(...) RMOD(LB0STCR, __VA_ARGS__)
#define RIZS_LB0STCR(...) RIZS(LB0STCR, __VA_ARGS__)
#define RFLD_LB0STCR(fld) RFLD(LB0STCR, fld)

typedef union {
	struct {
		unsigned int lx_ip:1; //0x0
		unsigned int mbz_0:31; //0x0
	} f;
	unsigned int v;
} LB0STIR_T;
#define LB0STIRar (0xB8005214)
#define LB0STIRdv (0x00000000)
#define LB0STIRrv RVAL(LB0STIR)
#define RMOD_LB0STIR(...) RMOD(LB0STIR, __VA_ARGS__)
#define RIZS_LB0STIR(...) RIZS(LB0STIR, __VA_ARGS__)
#define RFLD_LB0STIR(fld) RFLD(LB0STIR, fld)

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0x0
	} f;
	unsigned int v;
} LB0STMAR_T;
#define LB0STMARar (0xB8005218)
#define LB0STMARdv (0x00000000)
#define LB0STMARrv RVAL(LB0STMAR)
#define RMOD_LB0STMAR(...) RMOD(LB0STMAR, __VA_ARGS__)
#define RIZS_LB0STMAR(...) RIZS(LB0STMAR, __VA_ARGS__)
#define RFLD_LB0STMAR(fld) RFLD(LB0STMAR, fld)

typedef union {
	struct {
		unsigned int lx_tcen:1; //0x1
		unsigned int lx_tct:3; //0x7
		unsigned int mbz_0:28; //0x0
	} f;
	unsigned int v;
} LB1MTCR_T;
#define LB1MTCRar (0xB8005220)
#define LB1MTCRdv (0xF0000000)
#define LB1MTCRrv RVAL(LB1MTCR)
#define RMOD_LB1MTCR(...) RMOD(LB1MTCR, __VA_ARGS__)
#define RIZS_LB1MTCR(...) RIZS(LB1MTCR, __VA_ARGS__)
#define RFLD_LB1MTCR(fld) RFLD(LB1MTCR, fld)

typedef union {
	struct {
		unsigned int lx_ip:1; //0x0
		unsigned int mbz_0:31; //0x0
	} f;
	unsigned int v;
} LB1MTIR_T;
#define LB1MTIRar (0xB8005224)
#define LB1MTIRdv (0x00000000)
#define LB1MTIRrv RVAL(LB1MTIR)
#define RMOD_LB1MTIR(...) RMOD(LB1MTIR, __VA_ARGS__)
#define RIZS_LB1MTIR(...) RIZS(LB1MTIR, __VA_ARGS__)
#define RFLD_LB1MTIR(fld) RFLD(LB1MTIR, fld)

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0x0
	} f;
	unsigned int v;
} LB1MTMAR_T;
#define LB1MTMARar (0xB8005228)
#define LB1MTMARdv (0x00000000)
#define LB1MTMARrv RVAL(LB1MTMAR)
#define RMOD_LB1MTMAR(...) RMOD(LB1MTMAR, __VA_ARGS__)
#define RIZS_LB1MTMAR(...) RIZS(LB1MTMAR, __VA_ARGS__)
#define RFLD_LB1MTMAR(fld) RFLD(LB1MTMAR, fld)

typedef union {
	struct {
		unsigned int lx_tcen:1; //0x1
		unsigned int lx_tct:3; //0x7
		unsigned int mbz_0:28; //0x0
	} f;
	unsigned int v;
} LB1STCR_T;
#define LB1STCRar (0xB8005230)
#define LB1STCRdv (0xF0000000)
#define LB1STCRrv RVAL(LB1STCR)
#define RMOD_LB1STCR(...) RMOD(LB1STCR, __VA_ARGS__)
#define RIZS_LB1STCR(...) RIZS(LB1STCR, __VA_ARGS__)
#define RFLD_LB1STCR(fld) RFLD(LB1STCR, fld)

typedef union {
	struct {
		unsigned int lx_ip:1; //0x0
		unsigned int mbz_0:31; //0x0
	} f;
	unsigned int v;
} LB1STIR_T;
#define LB1STIRar (0xB8005234)
#define LB1STIRdv (0x00000000)
#define LB1STIRrv RVAL(LB1STIR)
#define RMOD_LB1STIR(...) RMOD(LB1STIR, __VA_ARGS__)
#define RIZS_LB1STIR(...) RIZS(LB1STIR, __VA_ARGS__)
#define RFLD_LB1STIR(fld) RFLD(LB1STIR, fld)

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0x0
	} f;
	unsigned int v;
} LB1STMAR_T;
#define LB1STMARar (0xB8005238)
#define LB1STMARdv (0x00000000)
#define LB1STMARrv RVAL(LB1STMAR)
#define RMOD_LB1STMAR(...) RMOD(LB1STMAR, __VA_ARGS__)
#define RIZS_LB1STMAR(...) RIZS(LB1STMAR, __VA_ARGS__)
#define RFLD_LB1STMAR(fld) RFLD(LB1STMAR, fld)

typedef union {
	struct {
		unsigned int lx_tcen:1; //0x1
		unsigned int lx_tct:3; //0x7
		unsigned int mbz_0:28; //0x0
	} f;
	unsigned int v;
} LB2STCR_T;
#define LB2STCRar (0xB8005260)
#define LB2STCRdv (0xF0000000)
#define LB2STCRrv RVAL(LB2STCR)
#define RMOD_LB2STCR(...) RMOD(LB2STCR, __VA_ARGS__)
#define RIZS_LB2STCR(...) RIZS(LB2STCR, __VA_ARGS__)
#define RFLD_LB2STCR(fld) RFLD(LB2STCR, fld)

typedef union {
	struct {
		unsigned int lx_ip:1; //0x0
		unsigned int mbz_0:31; //0x0
	} f;
	unsigned int v;
} LB2STIR_T;
#define LB2STIRar (0xB8005264)
#define LB2STIRdv (0x00000000)
#define LB2STIRrv RVAL(LB2STIR)
#define RMOD_LB2STIR(...) RMOD(LB2STIR, __VA_ARGS__)
#define RIZS_LB2STIR(...) RIZS(LB2STIR, __VA_ARGS__)
#define RFLD_LB2STIR(fld) RFLD(LB2STIR, fld)

typedef union {
	struct {
		unsigned int lx_to_addr:32; //0x0
	} f;
	unsigned int v;
} LB2STMAR_T;
#define LB2STMARar (0xB8005268)
#define LB2STMARdv (0x00000000)
#define LB2STMARrv RVAL(LB2STMAR)
#define RMOD_LB2STMAR(...) RMOD(LB2STMAR, __VA_ARGS__)
#define RIZS_LB2STMAR(...) RIZS(LB2STMAR, __VA_ARGS__)
#define RFLD_LB2STMAR(fld) RFLD(LB2STMAR, fld)

#endif  //_REG_MAP_LXTO_H

