#ifndef _REG_MAP_UART_H
#define _REG_MAP_UART_H

/*-----------------------------------------------------
 Extraced from file_UART0.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int rbr_thr_dll:8; //0x0
		unsigned int mbz_0:24; //0x0
	} f;
	unsigned int v;
} UART0_PBR_THR_DLL_T;
#define UART0_PBR_THR_DLLar (0xB8002000)
#define UART0_PBR_THR_DLLdv (0x00000000)
#define UART0_PBR_THR_DLLrv RVAL(UART0_PBR_THR_DLL)
#define RMOD_UART0_PBR_THR_DLL(...) RMOD(UART0_PBR_THR_DLL, __VA_ARGS__)
#define RIZS_UART0_PBR_THR_DLL(...) RIZS(UART0_PBR_THR_DLL, __VA_ARGS__)
#define RFLD_UART0_PBR_THR_DLL(fld) RFLD(UART0_PBR_THR_DLL, fld)

typedef union {
	struct {
		unsigned int ier_dlm:8; //0x0
		unsigned int mbz_0:24; //0x0
	} f;
	unsigned int v;
} UART0_IER_DLM_T;
#define UART0_IER_DLMar (0xB8002004)
#define UART0_IER_DLMdv (0x00000000)
#define UART0_IER_DLMrv RVAL(UART0_IER_DLM)
#define RMOD_UART0_IER_DLM(...) RMOD(UART0_IER_DLM, __VA_ARGS__)
#define RIZS_UART0_IER_DLM(...) RIZS(UART0_IER_DLM, __VA_ARGS__)
#define RFLD_UART0_IER_DLM(fld) RFLD(UART0_IER_DLM, fld)

typedef union {
	struct {
		unsigned int iir_fcr_1:2; //0x3
		unsigned int mbz_0:2; //0x0
		unsigned int iir_fcr_0:4; //0x1
		unsigned int mbz_1:24; //0x0
	} f;
	unsigned int v;
} UART0_IIR_FCR_T;
#define UART0_IIR_FCRar (0xB8002008)
#define UART0_IIR_FCRdv (0xC1000000)
#define UART0_IIR_FCRrv RVAL(UART0_IIR_FCR)
#define RMOD_UART0_IIR_FCR(...) RMOD(UART0_IIR_FCR, __VA_ARGS__)
#define RIZS_UART0_IIR_FCR(...) RIZS(UART0_IIR_FCR, __VA_ARGS__)
#define RFLD_UART0_IIR_FCR(fld) RFLD(UART0_IIR_FCR, fld)

typedef union {
	struct {
		unsigned int dlab:1; //0x0
		unsigned int brk:1; //0x0
		unsigned int eps:2; //0x0
		unsigned int pen:1; //0x0
		unsigned int stb:1; //0x0
		unsigned int wls_1:1; //0x1
		unsigned int wls_0:1; //0x1
		unsigned int mbz_0:24; //0x0
	} f;
	unsigned int v;
} UART0_LCR_T;
#define UART0_LCRar (0xB800200C)
#define UART0_LCRdv (0x03000000)
#define UART0_LCRrv RVAL(UART0_LCR)
#define RMOD_UART0_LCR(...) RMOD(UART0_LCR, __VA_ARGS__)
#define RIZS_UART0_LCR(...) RIZS(UART0_LCR, __VA_ARGS__)
#define RFLD_UART0_LCR(fld) RFLD(UART0_LCR, fld)

typedef union {
	struct {
		unsigned int eco_en:1; //0x0
		unsigned int lxclk_sel:1; //0x0
		unsigned int afe:1; //0x0
		unsigned int loop:1; //0x0
		unsigned int out2:1; //0x0
		unsigned int out1:1; //0x0
		unsigned int rts:1; //0x0
		unsigned int dtr:1; //0x0
		unsigned int mbz_1:24; //0x0
	} f;
	unsigned int v;
} UART0_MCR_T;
#define UART0_MCRar (0xB8002010)
#define UART0_MCRdv (0x00000000)
#define UART0_MCRrv RVAL(UART0_MCR)
#define RMOD_UART0_MCR(...) RMOD(UART0_MCR, __VA_ARGS__)
#define RIZS_UART0_MCR(...) RIZS(UART0_MCR, __VA_ARGS__)
#define RFLD_UART0_MCR(fld) RFLD(UART0_MCR, fld)

typedef union {
	struct {
		unsigned int rfe:1; //0x0
		unsigned int temt:1; //0x1
		unsigned int thre:1; //0x1
		unsigned int bi:1; //0x0
		unsigned int fe:1; //0x0
		unsigned int pe:1; //0x0
		unsigned int oe:1; //0x0
		unsigned int dr:1; //0x0
		unsigned int mbz_0:24; //0x0
	} f;
	unsigned int v;
} UART0_LSR_T;
#define UART0_LSRar (0xB8002014)
#define UART0_LSRdv (0x60000000)
#define UART0_LSRrv RVAL(UART0_LSR)
#define RMOD_UART0_LSR(...) RMOD(UART0_LSR, __VA_ARGS__)
#define RIZS_UART0_LSR(...) RIZS(UART0_LSR, __VA_ARGS__)
#define RFLD_UART0_LSR(fld) RFLD(UART0_LSR, fld)

typedef union {
	struct {
		unsigned int dcd:1; //0x0
		unsigned int ri:1; //0x0
		unsigned int dsr:1; //0x0
		unsigned int cts:1; //0x0
		unsigned int ddcd:1; //0x0
		unsigned int teri:1; //0x0
		unsigned int ddsr:1; //0x0
		unsigned int dcts:1; //0x0
		unsigned int mbz_0:24; //0x0
	} f;
	unsigned int v;
} UART0_MSR_T;
#define UART0_MSRar (0xB8002018)
#define UART0_MSRdv (0x00000000)
#define UART0_MSRrv RVAL(UART0_MSR)
#define RMOD_UART0_MSR(...) RMOD(UART0_MSR, __VA_ARGS__)
#define RIZS_UART0_MSR(...) RIZS(UART0_MSR, __VA_ARGS__)
#define RFLD_UART0_MSR(fld) RFLD(UART0_MSR, fld)

/*-----------------------------------------------------
 Extraced from file_UART1.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int rbr_thr_dll:8; //0x0
		unsigned int mbz_0:24; //0x0
	} f;
	unsigned int v;
} UART1_PBR_THR_DLL_T;
#define UART1_PBR_THR_DLLar (0xB8002100)
#define UART1_PBR_THR_DLLdv (0x00000000)
#define UART1_PBR_THR_DLLrv RVAL(UART1_PBR_THR_DLL)
#define RMOD_UART1_PBR_THR_DLL(...) RMOD(UART1_PBR_THR_DLL, __VA_ARGS__)
#define RIZS_UART1_PBR_THR_DLL(...) RIZS(UART1_PBR_THR_DLL, __VA_ARGS__)
#define RFLD_UART1_PBR_THR_DLL(fld) RFLD(UART1_PBR_THR_DLL, fld)

typedef union {
	struct {
		unsigned int ier_dlm:8; //0x0
		unsigned int mbz_0:24; //0x0
	} f;
	unsigned int v;
} UART1_IER_DLM_T;
#define UART1_IER_DLMar (0xB8002104)
#define UART1_IER_DLMdv (0x00000000)
#define UART1_IER_DLMrv RVAL(UART1_IER_DLM)
#define RMOD_UART1_IER_DLM(...) RMOD(UART1_IER_DLM, __VA_ARGS__)
#define RIZS_UART1_IER_DLM(...) RIZS(UART1_IER_DLM, __VA_ARGS__)
#define RFLD_UART1_IER_DLM(fld) RFLD(UART1_IER_DLM, fld)

typedef union {
	struct {
		unsigned int iir_fcr_1:2; //0x3
		unsigned int mbz_0:2; //0x0
		unsigned int iir_fcr_0:4; //0x1
		unsigned int mbz_1:24; //0x0
	} f;
	unsigned int v;
} UART1_IIR_FCR_T;
#define UART1_IIR_FCRar (0xB8002108)
#define UART1_IIR_FCRdv (0xC1000000)
#define UART1_IIR_FCRrv RVAL(UART1_IIR_FCR)
#define RMOD_UART1_IIR_FCR(...) RMOD(UART1_IIR_FCR, __VA_ARGS__)
#define RIZS_UART1_IIR_FCR(...) RIZS(UART1_IIR_FCR, __VA_ARGS__)
#define RFLD_UART1_IIR_FCR(fld) RFLD(UART1_IIR_FCR, fld)

typedef union {
	struct {
		unsigned int dlab:1; //0x0
		unsigned int brk:1; //0x0
		unsigned int eps:2; //0x0
		unsigned int pen:1; //0x0
		unsigned int stb:1; //0x0
		unsigned int wls_1:1; //0x1
		unsigned int wls_0:1; //0x1
		unsigned int mbz_0:24; //0x0
	} f;
	unsigned int v;
} UART1_LCR_T;
#define UART1_LCRar (0xB800210C)
#define UART1_LCRdv (0x03000000)
#define UART1_LCRrv RVAL(UART1_LCR)
#define RMOD_UART1_LCR(...) RMOD(UART1_LCR, __VA_ARGS__)
#define RIZS_UART1_LCR(...) RIZS(UART1_LCR, __VA_ARGS__)
#define RFLD_UART1_LCR(fld) RFLD(UART1_LCR, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int lxclk_sel:1; //0x0
		unsigned int afe:1; //0x0
		unsigned int loop:1; //0x0
		unsigned int out2:1; //0x0
		unsigned int out1:1; //0x0
		unsigned int rts:1; //0x0
		unsigned int dtr:1; //0x0
		unsigned int mbz_1:24; //0x0
	} f;
	unsigned int v;
} UART1_MCR_T;
#define UART1_MCRar (0xB8002110)
#define UART1_MCRdv (0x00000000)
#define UART1_MCRrv RVAL(UART1_MCR)
#define RMOD_UART1_MCR(...) RMOD(UART1_MCR, __VA_ARGS__)
#define RIZS_UART1_MCR(...) RIZS(UART1_MCR, __VA_ARGS__)
#define RFLD_UART1_MCR(fld) RFLD(UART1_MCR, fld)

typedef union {
	struct {
		unsigned int rfe:1; //0x0
		unsigned int temt:1; //0x1
		unsigned int thre:1; //0x1
		unsigned int bi:1; //0x0
		unsigned int fe:1; //0x0
		unsigned int pe:1; //0x0
		unsigned int oe:1; //0x0
		unsigned int dr:1; //0x0
		unsigned int mbz_0:24; //0x0
	} f;
	unsigned int v;
} UART1_LSR_T;
#define UART1_LSRar (0xB8002114)
#define UART1_LSRdv (0x60000000)
#define UART1_LSRrv RVAL(UART1_LSR)
#define RMOD_UART1_LSR(...) RMOD(UART1_LSR, __VA_ARGS__)
#define RIZS_UART1_LSR(...) RIZS(UART1_LSR, __VA_ARGS__)
#define RFLD_UART1_LSR(fld) RFLD(UART1_LSR, fld)

typedef union {
	struct {
		unsigned int dcd:1; //0x0
		unsigned int ri:1; //0x0
		unsigned int dsr:1; //0x0
		unsigned int cts:1; //0x0
		unsigned int ddcd:1; //0x0
		unsigned int teri:1; //0x0
		unsigned int ddsr:1; //0x0
		unsigned int dcts:1; //0x0
		unsigned int mbz_0:24; //0x0
	} f;
	unsigned int v;
} UART1_MSR_T;
#define UART1_MSRar (0xB8002118)
#define UART1_MSRdv (0x00000000)
#define UART1_MSRrv RVAL(UART1_MSR)
#define RMOD_UART1_MSR(...) RMOD(UART1_MSR, __VA_ARGS__)
#define RIZS_UART1_MSR(...) RIZS(UART1_MSR, __VA_ARGS__)
#define RFLD_UART1_MSR(fld) RFLD(UART1_MSR, fld)

#endif // _REG_MAP_UART_H
