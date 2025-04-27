#ifndef _REG_MAP_SPI_NAND_H
#define _REG_MAP_SPI_NAND_H
/*-----------------------------------------------------
 Extraced from file_SPI_NAND_CTRL.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int nafc_nf:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int debug_select:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int rbo:1; //0x0
		unsigned int wbo:1; //0x0
		unsigned int dma_ie:1; //0x0
		unsigned int mbz_3:5; //0x0
		unsigned int slv_endian:1; //0x0
		unsigned int dma_endian:1; //0x0
		unsigned int precise:1; //0x1
		unsigned int mbz_4:2; //0x0
		unsigned int pipe_lat:2; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int spi_clk_div:3; //0x7
		unsigned int mbz_6:2; //0x0
		unsigned int lbc_bsz:2; //0x3
	} f;
	unsigned int v;
} SNFCFR_T;
#define SNFCFRar (0xB801A400)
#define SNFCFRdv (0x00001073)
#define SNFCFRrv RVAL(SNFCFR)
#define RMOD_SNFCFR(...) RMOD(SNFCFR, __VA_ARGS__)
#define RIZS_SNFCFR(...) RIZS(SNFCFR, __VA_ARGS__)
#define RFLD_SNFCFR(fld) RFLD(SNFCFR, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int cecs1:1; //0x1
		unsigned int mbz_1:3; //0x0
		unsigned int cecs0:1; //0x1
	} f;
	unsigned int v;
} SNFCCR_T;
#define SNFCCRar (0xB801A404)
#define SNFCCRdv (0x00000011)
#define SNFCCRrv RVAL(SNFCCR)
#define RMOD_SNFCCR(...) RMOD(SNFCCR, __VA_ARGS__)
#define RIZS_SNFCCR(...) RIZS(SNFCCR, __VA_ARGS__)
#define RFLD_SNFCCR(fld) RFLD(SNFCCR, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int w_io_width:2; //0x0
		unsigned int mbz_1:26; //0x0
		unsigned int w_len:2; //0x0
	} f;
	unsigned int v;
} SNFWCMR_T;
#define SNFWCMRar (0xB801A408)
#define SNFWCMRdv (0x00000000)
#define SNFWCMRrv RVAL(SNFWCMR)
#define RMOD_SNFWCMR(...) RMOD(SNFWCMR, __VA_ARGS__)
#define RIZS_SNFWCMR(...) RIZS(SNFWCMR, __VA_ARGS__)
#define RFLD_SNFWCMR(fld) RFLD(SNFWCMR, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int r_io_width:2; //0x0
		unsigned int mbz_1:26; //0x0
		unsigned int r_len:2; //0x0
	} f;
	unsigned int v;
} SNFRCMR_T;
#define SNFRCMRar (0xB801A40C)
#define SNFRCMRdv (0x00000000)
#define SNFRCMRrv RVAL(SNFRCMR)
#define RMOD_SNFRCMR(...) RMOD(SNFRCMR, __VA_ARGS__)
#define RIZS_SNFRCMR(...) RIZS(SNFRCMR, __VA_ARGS__)
#define RFLD_SNFRCMR(fld) RFLD(SNFRCMR, fld)

typedef union {
	struct {
		unsigned int rdata3:8; //0x0
		unsigned int rdata2:8; //0x0
		unsigned int rdata1:8; //0x0
		unsigned int rdata0:8; //0x0
	} f;
	unsigned int v;
} SNFRDR_T;
#define SNFRDRar (0xB801A410)
#define SNFRDRdv (0x00000000)
#define SNFRDRrv RVAL(SNFRDR)
#define RMOD_SNFRDR(...) RMOD(SNFRDR, __VA_ARGS__)
#define RIZS_SNFRDR(...) RIZS(SNFRDR, __VA_ARGS__)
#define RFLD_SNFRDR(fld) RFLD(SNFRDR, fld)

typedef union {
	struct {
		unsigned int wdata3:8; //0x0
		unsigned int wdata2:8; //0x0
		unsigned int wdata1:8; //0x0
		unsigned int wdata0:8; //0x0
	} f;
	unsigned int v;
} SNFWDR_T;
#define SNFWDRar (0xB801A414)
#define SNFWDRdv (0x00000000)
#define SNFWDRrv RVAL(SNFWDR)
#define RMOD_SNFWDR(...) RMOD(SNFWDR, __VA_ARGS__)
#define RIZS_SNFWDR(...) RIZS(SNFWDR, __VA_ARGS__)
#define RFLD_SNFWDR(fld) RFLD(SNFWDR, fld)

typedef union {
	struct {
		unsigned int mbz_0:31; //0x0
		unsigned int dmarwe:1; //0x0
	} f;
	unsigned int v;
} SNFDTR_T;
#define SNFDTRar (0xB801A418)
#define SNFDTRdv (0x00000000)
#define SNFDTRrv RVAL(SNFDTR)
#define RMOD_SNFDTR(...) RMOD(SNFDTR, __VA_ARGS__)
#define RIZS_SNFDTR(...) RIZS(SNFDTR, __VA_ARGS__)
#define RFLD_SNFDTR(fld) RFLD(SNFDTR, fld)

typedef union {
	struct {
		unsigned int addr:32; //0x0
	} f;
	unsigned int v;
} SNFDRSAR_T;
#define SNFDRSARar (0xB801A41C)
#define SNFDRSARdv (0x00000000)
#define SNFDRSARrv RVAL(SNFDRSAR)
#define RMOD_SNFDRSAR(...) RMOD(SNFDRSAR, __VA_ARGS__)
#define RIZS_SNFDRSAR(...) RIZS(SNFDRSAR, __VA_ARGS__)
#define RFLD_SNFDRSAR(fld) RFLD(SNFDRSAR, fld)

typedef union {
	struct {
		unsigned int mbz_0:31; //0x0
		unsigned int dma_ip:1; //0x0
	} f;
	unsigned int v;
} SNFDIR_T;
#define SNFDIRar (0xB801A420)
#define SNFDIRdv (0x00000000)
#define SNFDIRrv RVAL(SNFDIR)
#define RMOD_SNFDIR(...) RMOD(SNFDIR, __VA_ARGS__)
#define RIZS_SNFDIR(...) RIZS(SNFDIR, __VA_ARGS__)
#define RFLD_SNFDIR(fld) RFLD(SNFDIR, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dma_io_width:2; //0x0
		unsigned int mbz_1:12; //0x0
		unsigned int len:16; //0x0
	} f;
	unsigned int v;
} SNFDLR_T;
#define SNFDLRar (0xB801A424)
#define SNFDLRdv (0x00000000)
#define SNFDLRrv RVAL(SNFDLR)
#define RMOD_SNFDLR(...) RMOD(SNFDLR, __VA_ARGS__)
#define RIZS_SNFDLR(...) RIZS(SNFDLR, __VA_ARGS__)
#define RFLD_SNFDLR(fld) RFLD(SNFDLR, fld)

typedef union {
	struct {
		unsigned int mbz_0:12; //0x0
		unsigned int clenc:20; //0x0
	} f;
	unsigned int v;
} SNFDCDSR_T;
#define SNFDCDSRar (0xB801A428)
#define SNFDCDSRdv (0x00000000)
#define SNFDCDSRrv RVAL(SNFDCDSR)
#define RMOD_SNFDCDSR(...) RMOD(SNFDCDSR, __VA_ARGS__)
#define RIZS_SNFDCDSR(...) RIZS(SNFDCDSR, __VA_ARGS__)
#define RFLD_SNFDCDSR(fld) RFLD(SNFDCDSR, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int cs1:1; //0x1
		unsigned int nfcos:1; //0x0
		unsigned int nfdrs:1; //0x0
		unsigned int nfdws:1; //0x0
		unsigned int cs0:1; //0x1
	} f;
	unsigned int v;
} SNFSR_T;
#define SNFSRar (0xB801A440)
#define SNFSRdv (0x00000011)
#define SNFSRrv RVAL(SNFSR)
#define RMOD_SNFSR(...) RMOD(SNFSR, __VA_ARGS__)
#define RIZS_SNFSR(...) RIZS(SNFSR, __VA_ARGS__)
#define RFLD_SNFSR(fld) RFLD(SNFSR, fld)

#endif // _REG_MAP_SPI_NAND_H
