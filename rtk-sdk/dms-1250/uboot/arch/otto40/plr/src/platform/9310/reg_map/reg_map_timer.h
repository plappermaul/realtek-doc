#ifndef _REG_MAP_TIMER_H
#define _REG_MAP_TIMER_H

/*-----------------------------------------------------
 Extraced from file_TIMER.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc0data:28; //0x0
	} f;
	unsigned int v;
} TC0DATA_T;
#define TC0DATAar (0xB8003200)
#define TC0DATAdv (0x00000000)
#define TC0DATArv RVAL(TC0DATA)
#define RMOD_TC0DATA(...) RMOD(TC0DATA, __VA_ARGS__)
#define RIZS_TC0DATA(...) RIZS(TC0DATA, __VA_ARGS__)
#define RFLD_TC0DATA(fld) RFLD(TC0DATA, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc0value:28; //0x0
	} f;
	unsigned int v;
} TC0CNT_T;
#define TC0CNTar (0xB8003204)
#define TC0CNTdv (0x00000000)
#define TC0CNTrv RVAL(TC0CNT)
#define RMOD_TC0CNT(...) RMOD(TC0CNT, __VA_ARGS__)
#define RIZS_TC0CNT(...) RIZS(TC0CNT, __VA_ARGS__)
#define RFLD_TC0CNT(fld) RFLD(TC0CNT, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int tc0en:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc0mode:1; //0x0
		unsigned int mbz_2:8; //0x0
		unsigned int tc0divfactor:16; //0x0
	} f;
	unsigned int v;
} TC0CTRL_T;
#define TC0CTRLar (0xB8003208)
#define TC0CTRLdv (0x00000000)
#define TC0CTRLrv RVAL(TC0CTRL)
#define RMOD_TC0CTRL(...) RMOD(TC0CTRL, __VA_ARGS__)
#define RIZS_TC0CTRL(...) RIZS(TC0CTRL, __VA_ARGS__)
#define RFLD_TC0CTRL(fld) RFLD(TC0CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tc0ie:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc0ip:1; //0x0
		unsigned int mbz_2:16; //0x0
	} f;
	unsigned int v;
} TC0INTR_T;
#define TC0INTRar (0xB800320C)
#define TC0INTRdv (0x00000000)
#define TC0INTRrv RVAL(TC0INTR)
#define RMOD_TC0INTR(...) RMOD(TC0INTR, __VA_ARGS__)
#define RIZS_TC0INTR(...) RIZS(TC0INTR, __VA_ARGS__)
#define RFLD_TC0INTR(fld) RFLD(TC0INTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc1data:28; //0x0
	} f;
	unsigned int v;
} TC1DATA_T;
#define TC1DATAar (0xB8003210)
#define TC1DATAdv (0x00000000)
#define TC1DATArv RVAL(TC1DATA)
#define RMOD_TC1DATA(...) RMOD(TC1DATA, __VA_ARGS__)
#define RIZS_TC1DATA(...) RIZS(TC1DATA, __VA_ARGS__)
#define RFLD_TC1DATA(fld) RFLD(TC1DATA, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc1value:28; //0x0
	} f;
	unsigned int v;
} TC1CNTR_T;
#define TC1CNTRar (0xB8003214)
#define TC1CNTRdv (0x00000000)
#define TC1CNTRrv RVAL(TC1CNTR)
#define RMOD_TC1CNTR(...) RMOD(TC1CNTR, __VA_ARGS__)
#define RIZS_TC1CNTR(...) RIZS(TC1CNTR, __VA_ARGS__)
#define RFLD_TC1CNTR(fld) RFLD(TC1CNTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int tc1en:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc1mode:1; //0x0
		unsigned int mbz_2:8; //0x0
		unsigned int tc1divfactor:16; //0x0
	} f;
	unsigned int v;
} TC1CTRL_T;
#define TC1CTRLar (0xB8003218)
#define TC1CTRLdv (0x00000000)
#define TC1CTRLrv RVAL(TC1CTRL)
#define RMOD_TC1CTRL(...) RMOD(TC1CTRL, __VA_ARGS__)
#define RIZS_TC1CTRL(...) RIZS(TC1CTRL, __VA_ARGS__)
#define RFLD_TC1CTRL(fld) RFLD(TC1CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tc1ie:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc1ip:1; //0x0
		unsigned int mbz_2:16; //0x0
	} f;
	unsigned int v;
} TC1INTR_T;
#define TC1INTRar (0xB800321C)
#define TC1INTRdv (0x00000000)
#define TC1INTRrv RVAL(TC1INTR)
#define RMOD_TC1INTR(...) RMOD(TC1INTR, __VA_ARGS__)
#define RIZS_TC1INTR(...) RIZS(TC1INTR, __VA_ARGS__)
#define RFLD_TC1INTR(fld) RFLD(TC1INTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc1data:28; //0x0
	} f;
	unsigned int v;
} TC2DATA_T;
#define TC2DATAar (0xB8003220)
#define TC2DATAdv (0x00000000)
#define TC2DATArv RVAL(TC2DATA)
#define RMOD_TC2DATA(...) RMOD(TC2DATA, __VA_ARGS__)
#define RIZS_TC2DATA(...) RIZS(TC2DATA, __VA_ARGS__)
#define RFLD_TC2DATA(fld) RFLD(TC2DATA, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc2value:28; //0x0
	} f;
	unsigned int v;
} TC2CNTR_T;
#define TC2CNTRar (0xB8003224)
#define TC2CNTRdv (0x00000000)
#define TC2CNTRrv RVAL(TC2CNTR)
#define RMOD_TC2CNTR(...) RMOD(TC2CNTR, __VA_ARGS__)
#define RIZS_TC2CNTR(...) RIZS(TC2CNTR, __VA_ARGS__)
#define RFLD_TC2CNTR(fld) RFLD(TC2CNTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int tc2en:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc2mode:1; //0x0
		unsigned int mbz_2:8; //0x0
		unsigned int tc2divfactor:16; //0x0
	} f;
	unsigned int v;
} TC2CTRL_T;
#define TC2CTRLar (0xB8003228)
#define TC2CTRLdv (0x00000000)
#define TC2CTRLrv RVAL(TC2CTRL)
#define RMOD_TC2CTRL(...) RMOD(TC2CTRL, __VA_ARGS__)
#define RIZS_TC2CTRL(...) RIZS(TC2CTRL, __VA_ARGS__)
#define RFLD_TC2CTRL(fld) RFLD(TC2CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tc2ie:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc2ip:1; //0x0
		unsigned int mbz_2:16; //0x0
	} f;
	unsigned int v;
} TC2INTR_T;
#define TC2INTRar (0xB800322C)
#define TC2INTRdv (0x00000000)
#define TC2INTRrv RVAL(TC2INTR)
#define RMOD_TC2INTR(...) RMOD(TC2INTR, __VA_ARGS__)
#define RIZS_TC2INTR(...) RIZS(TC2INTR, __VA_ARGS__)
#define RFLD_TC2INTR(fld) RFLD(TC2INTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc1data:28; //0x0
	} f;
	unsigned int v;
} TC3DATA_T;
#define TC3DATAar (0xB8003230)
#define TC3DATAdv (0x00000000)
#define TC3DATArv RVAL(TC3DATA)
#define RMOD_TC3DATA(...) RMOD(TC3DATA, __VA_ARGS__)
#define RIZS_TC3DATA(...) RIZS(TC3DATA, __VA_ARGS__)
#define RFLD_TC3DATA(fld) RFLD(TC3DATA, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc3value:28; //0x0
	} f;
	unsigned int v;
} TC3CNTR_T;
#define TC3CNTRar (0xB8003234)
#define TC3CNTRdv (0x00000000)
#define TC3CNTRrv RVAL(TC3CNTR)
#define RMOD_TC3CNTR(...) RMOD(TC3CNTR, __VA_ARGS__)
#define RIZS_TC3CNTR(...) RIZS(TC3CNTR, __VA_ARGS__)
#define RFLD_TC3CNTR(fld) RFLD(TC3CNTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int tc3en:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc3mode:1; //0x0
		unsigned int mbz_2:8; //0x0
		unsigned int tc3divfactor:16; //0x0
	} f;
	unsigned int v;
} TC3CTRL_T;
#define TC3CTRLar (0xB8003238)
#define TC3CTRLdv (0x00000000)
#define TC3CTRLrv RVAL(TC3CTRL)
#define RMOD_TC3CTRL(...) RMOD(TC3CTRL, __VA_ARGS__)
#define RIZS_TC3CTRL(...) RIZS(TC3CTRL, __VA_ARGS__)
#define RFLD_TC3CTRL(fld) RFLD(TC3CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tc3ie:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc3ip:1; //0x0
		unsigned int mbz_2:16; //0x0
	} f;
	unsigned int v;
} TC3INTR_T;
#define TC3INTRar (0xB800323C)
#define TC3INTRdv (0x00000000)
#define TC3INTRrv RVAL(TC3INTR)
#define RMOD_TC3INTR(...) RMOD(TC3INTR, __VA_ARGS__)
#define RIZS_TC3INTR(...) RIZS(TC3INTR, __VA_ARGS__)
#define RFLD_TC3INTR(fld) RFLD(TC3INTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc1data:28; //0x0
	} f;
	unsigned int v;
} TC4DATA_T;
#define TC4DATAar (0xB8003240)
#define TC4DATAdv (0x00000000)
#define TC4DATArv RVAL(TC4DATA)
#define RMOD_TC4DATA(...) RMOD(TC4DATA, __VA_ARGS__)
#define RIZS_TC4DATA(...) RIZS(TC4DATA, __VA_ARGS__)
#define RFLD_TC4DATA(fld) RFLD(TC4DATA, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc4value:28; //0x0
	} f;
	unsigned int v;
} TC4CNTR_T;
#define TC4CNTRar (0xB8003244)
#define TC4CNTRdv (0x00000000)
#define TC4CNTRrv RVAL(TC4CNTR)
#define RMOD_TC4CNTR(...) RMOD(TC4CNTR, __VA_ARGS__)
#define RIZS_TC4CNTR(...) RIZS(TC4CNTR, __VA_ARGS__)
#define RFLD_TC4CNTR(fld) RFLD(TC4CNTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int tc4en:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc4mode:1; //0x0
		unsigned int mbz_2:8; //0x0
		unsigned int tc4divfactor:16; //0x0
	} f;
	unsigned int v;
} TC4CTRL_T;
#define TC4CTRLar (0xB8003248)
#define TC4CTRLdv (0x00000000)
#define TC4CTRLrv RVAL(TC4CTRL)
#define RMOD_TC4CTRL(...) RMOD(TC4CTRL, __VA_ARGS__)
#define RIZS_TC4CTRL(...) RIZS(TC4CTRL, __VA_ARGS__)
#define RFLD_TC4CTRL(fld) RFLD(TC4CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tc4ie:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc4ip:1; //0x0
		unsigned int mbz_2:16; //0x0
	} f;
	unsigned int v;
} TC4INTR_T;
#define TC4INTRar (0xB800324C)
#define TC4INTRdv (0x00000000)
#define TC4INTRrv RVAL(TC4INTR)
#define RMOD_TC4INTR(...) RMOD(TC4INTR, __VA_ARGS__)
#define RIZS_TC4INTR(...) RIZS(TC4INTR, __VA_ARGS__)
#define RFLD_TC4INTR(fld) RFLD(TC4INTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc1data:28; //0x0
	} f;
	unsigned int v;
} TC5DATA_T;
#define TC5DATAar (0xB8003250)
#define TC5DATAdv (0x00000000)
#define TC5DATArv RVAL(TC5DATA)
#define RMOD_TC5DATA(...) RMOD(TC5DATA, __VA_ARGS__)
#define RIZS_TC5DATA(...) RIZS(TC5DATA, __VA_ARGS__)
#define RFLD_TC5DATA(fld) RFLD(TC5DATA, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc5value:28; //0x0
	} f;
	unsigned int v;
} TC5CNTR_T;
#define TC5CNTRar (0xB8003254)
#define TC5CNTRdv (0x00000000)
#define TC5CNTRrv RVAL(TC5CNTR)
#define RMOD_TC5CNTR(...) RMOD(TC5CNTR, __VA_ARGS__)
#define RIZS_TC5CNTR(...) RIZS(TC5CNTR, __VA_ARGS__)
#define RFLD_TC5CNTR(fld) RFLD(TC5CNTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int tc5en:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc5mode:1; //0x0
		unsigned int mbz_2:8; //0x0
		unsigned int tc5divfactor:16; //0x0
	} f;
	unsigned int v;
} TC5CTRL_T;
#define TC5CTRLar (0xB8003258)
#define TC5CTRLdv (0x00000000)
#define TC5CTRLrv RVAL(TC5CTRL)
#define RMOD_TC5CTRL(...) RMOD(TC5CTRL, __VA_ARGS__)
#define RIZS_TC5CTRL(...) RIZS(TC5CTRL, __VA_ARGS__)
#define RFLD_TC5CTRL(fld) RFLD(TC5CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tc5ie:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc5ip:1; //0x0
		unsigned int mbz_2:16; //0x0
	} f;
	unsigned int v;
} TC5INTR_T;
#define TC5INTRar (0xB800325C)
#define TC5INTRdv (0x00000000)
#define TC5INTRrv RVAL(TC5INTR)
#define RMOD_TC5INTR(...) RMOD(TC5INTR, __VA_ARGS__)
#define RIZS_TC5INTR(...) RIZS(TC5INTR, __VA_ARGS__)
#define RFLD_TC5INTR(fld) RFLD(TC5INTR, fld)

typedef union {
	struct {
		unsigned int wdt_kick:1; //0x0
		unsigned int mbz_0:31; //0x0
	} f;
	unsigned int v;
} WDTCNTRR_T;
#define WDTCNTRRar (0xB8003260)
#define WDTCNTRRdv (0x00000000)
#define WDTCNTRRrv RVAL(WDTCNTRR)
#define RMOD_WDTCNTRR(...) RMOD(WDTCNTRR, __VA_ARGS__)
#define RIZS_WDTCNTRR(...) RIZS(WDTCNTRR, __VA_ARGS__)
#define RFLD_WDTCNTRR(fld) RFLD(WDTCNTRR, fld)

typedef union {
	struct {
		unsigned int ph1_ip:1; //0x0
		unsigned int ph2_ip:1; //0x0
		unsigned int mbz_0:30; //0x0
	} f;
	unsigned int v;
} WDTINTRR_T;
#define WDTINTRRar (0xB8003264)
#define WDTINTRRdv (0x00000000)
#define WDTINTRRrv RVAL(WDTINTRR)
#define RMOD_WDTINTRR(...) RMOD(WDTINTRR, __VA_ARGS__)
#define RIZS_WDTINTRR(...) RIZS(WDTINTRR, __VA_ARGS__)
#define RFLD_WDTINTRR(fld) RFLD(WDTINTRR, fld)

typedef union {
	struct {
		unsigned int wdt_e:1; //0x0
		unsigned int wdt_clk_sc:2; //0x0
		unsigned int mbz_0:2; //0x0
		unsigned int ph1_to:5; //0x0
		unsigned int mbz_1:2; //0x0
		unsigned int ph2_to:5; //0x0
		unsigned int mbz_2:13; //0x0
		unsigned int wdt_reset_mode:2; //0x0
	} f;
	unsigned int v;
} WDT_CTRL_T;
#define WDT_CTRLar (0xB8003268)
#define WDT_CTRLdv (0x00000000)
#define WDT_CTRLrv RVAL(WDT_CTRL)
#define RMOD_WDT_CTRL(...) RMOD(WDT_CTRL, __VA_ARGS__)
#define RIZS_WDT_CTRL(...) RIZS(WDT_CTRL, __VA_ARGS__)
#define RFLD_WDT_CTRL(fld) RFLD(WDT_CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc6data:28; //0x0
	} f;
	unsigned int v;
} TC6DATA_T;
#define TC6DATAar (0xB8003270)
#define TC6DATAdv (0x00000000)
#define TC6DATArv RVAL(TC6DATA)
#define RMOD_TC6DATA(...) RMOD(TC6DATA, __VA_ARGS__)
#define RIZS_TC6DATA(...) RIZS(TC6DATA, __VA_ARGS__)
#define RFLD_TC6DATA(fld) RFLD(TC6DATA, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int tc6value:28; //0x0
	} f;
	unsigned int v;
} TC6CNTR_T;
#define TC6CNTRar (0xB8003274)
#define TC6CNTRdv (0x00000000)
#define TC6CNTRrv RVAL(TC6CNTR)
#define RMOD_TC6CNTR(...) RMOD(TC6CNTR, __VA_ARGS__)
#define RIZS_TC6CNTR(...) RIZS(TC6CNTR, __VA_ARGS__)
#define RFLD_TC6CNTR(fld) RFLD(TC6CNTR, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int tc6en:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc6mode:1; //0x0
		unsigned int mbz_2:8; //0x0
		unsigned int tc6divfactor:16; //0x0
	} f;
	unsigned int v;
} TC6CTRL_T;
#define TC6CTRLar (0xB8003278)
#define TC6CTRLdv (0x00000000)
#define TC6CTRLrv RVAL(TC6CTRL)
#define RMOD_TC6CTRL(...) RMOD(TC6CTRL, __VA_ARGS__)
#define RIZS_TC6CTRL(...) RIZS(TC6CTRL, __VA_ARGS__)
#define RFLD_TC6CTRL(fld) RFLD(TC6CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tc6ie:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tc6ip:1; //0x0
		unsigned int mbz_2:16; //0x0
	} f;
	unsigned int v;
} TC6INTR_T;
#define TC6INTRar (0xB800327C)
#define TC6INTRdv (0x00000000)
#define TC6INTRrv RVAL(TC6INTR)
#define RMOD_TC6INTR(...) RMOD(TC6INTR, __VA_ARGS__)
#define RIZS_TC6INTR(...) RIZS(TC6INTR, __VA_ARGS__)
#define RFLD_TC6INTR(fld) RFLD(TC6INTR, fld)

#endif  // _REG_MAP_TIMER_H
