#ifndef NAND_SPI_CTRL_H
#define NAND_SPI_CTRL_H

#include <reg_map_util.h>

/***********************************************
  * Driver Section Definitions
  ***********************************************/
#ifndef SECTION_NAND_SPI
    #define SECTION_NAND_SPI 
#endif
#ifndef SECTION_NAND_SPI_DATA
    #define SECTION_NAND_SPI_DATA 
#endif

/***********************************************
  *  SPI NAND Flash Controller Regiter Address
  ***********************************************/
typedef union {
	struct {
		u32_t mbz_0:3; //0
		u32_t nafc_nf:1; //0
		u32_t mbz_1:1; //0
		u32_t debug_select:3; //0
		u32_t mbz_2:1; //0
		u32_t rbo:1; //0
		u32_t wbo:1; //0
		u32_t dma_ie:1; //0
		u32_t mbz_3:5; //0
		u32_t slv_endian:1; //0
		u32_t dma_endian:1; //0
		u32_t precise:1; //1
		u32_t mbz_4:2; //0
		u32_t pipe_lat:2; //0
		u32_t mbz_5:1; //0
		u32_t spi_clk_div:3; //7
		u32_t mbz_6:2; //0
		u32_t lbc_bsz:2; //3
	} f;
	u32_t v;
} SNAFCFR_T;
#define SNAFCFRrv (*((regval)0xb801a400))
#define SNAFCFRdv (0x00001073)
#define RMOD_SNAFCFR(...) rset(SNAFCFR, SNAFCFRrv, __VA_ARGS__)
#define RIZS_SNAFCFR(...) rset(SNAFCFR, 0, __VA_ARGS__)
#define RFLD_SNAFCFR(fld) (*((const volatile SNAFCFR_T *)0xb801a400)).f.fld

typedef union {
	struct {
		u32_t mbz_0:31; //0
		u32_t cecs:1; //1
	} f;
	u32_t v;
} SNAFCSR_T;
#define SNAFCSRrv (*((regval)0xb801a404))
#define SNAFCSRdv (0x00000001)
#define RMOD_SNAFCSR(...) rset(SNAFCSR, SNAFCSRrv, __VA_ARGS__)
#define RIZS_SNAFCSR(...) rset(SNAFCSR, 0, __VA_ARGS__)
#define RFLD_SNAFCSR(fld) (*((const volatile SNAFCSR_T *)0xb801a404)).f.fld

typedef union {
	struct {
		u32_t mbz_0:2; //0
		u32_t w_io_width:2; //0
		u32_t mbz_1:26; //0
		u32_t w_len:2; //0
	} f;
	u32_t v;
} SNAFWCMR_T;
#define SNAFWCMRrv (*((regval)0xb801a408))
#define SNAFWCMRdv (0x00000000)
#define RMOD_SNAFWCMR(...) rset(SNAFWCMR, SNAFWCMRrv, __VA_ARGS__)
#define RIZS_SNAFWCMR(...) rset(SNAFWCMR, 0, __VA_ARGS__)
#define RFLD_SNAFWCMR(fld) (*((const volatile SNAFWCMR_T *)0xb801a408)).f.fld

typedef union {
	struct {
		u32_t mbz_0:2; //0
		u32_t r_io_width:2; //0
		u32_t mbz_1:26; //0
		u32_t r_len:2; //0
	} f;
	u32_t v;
} SNAFRCMR_T;
#define SNAFRCMRrv (*((regval)0xb801a40c))
#define SNAFRCMRdv (0x00000000)
#define RMOD_SNAFRCMR(...) rset(SNAFRCMR, SNAFRCMRrv, __VA_ARGS__)
#define RIZS_SNAFRCMR(...) rset(SNAFRCMR, 0, __VA_ARGS__)
#define RFLD_SNAFRCMR(fld) (*((const volatile SNAFRCMR_T *)0xb801a40c)).f.fld

typedef union {
	struct {
		u32_t rdata3:8; //0
		u32_t rdata2:8; //0
		u32_t rdata1:8; //0
		u32_t rdata0:8; //0
	} f;
	u32_t v;
} SNAFRDR_T;
#define SNAFRDRrv (*((regval)0xb801a410))
#define SNAFRDRdv (0x00000000)
#define RMOD_SNAFRDR(...) rset(SNAFRDR, SNAFRDRrv, __VA_ARGS__)
#define RIZS_SNAFRDR(...) rset(SNAFRDR, 0, __VA_ARGS__)
#define RFLD_SNAFRDR(fld) (*((const volatile SNAFRDR_T *)0xb801a410)).f.fld

typedef union {
	struct {
		u32_t wdata3:8; //0
		u32_t wdata2:8; //0
		u32_t wdata1:8; //0
		u32_t wdata0:8; //0
	} f;
	u32_t v;
} SNAFWDR_T;
#define SNAFWDRrv (*((regval)0xb801a414))
#define SNAFWDRdv (0x00000000)
#define RMOD_SNAFWDR(...) rset(SNAFWDR, SNAFWDRrv, __VA_ARGS__)
#define RIZS_SNAFWDR(...) rset(SNAFWDR, 0, __VA_ARGS__)
#define RFLD_SNAFWDR(fld) (*((const volatile SNAFWDR_T *)0xb801a414)).f.fld

typedef union {
	struct {
		u32_t mbz_0:31; //0
		u32_t dmarwe:1; //0
	} f;
	u32_t v;
} SNAFDTR_T;
#define SNAFDTRrv (*((regval)0xb801a418))
#define SNAFDTRdv (0x00000000)
#define RMOD_SNAFDTR(...) rset(SNAFDTR, SNAFDTRrv, __VA_ARGS__)
#define RIZS_SNAFDTR(...) rset(SNAFDTR, 0, __VA_ARGS__)
#define RFLD_SNAFDTR(fld) (*((const volatile SNAFDTR_T *)0xb801a418)).f.fld

typedef union {
	struct {
		u32_t addr:32; //0
	} f;
	u32_t v;
} SNAFDRSAR_T;
#define SNAFDRSARrv (*((regval)0xb801a41c))
#define SNAFDRSARdv (0x00000000)
#define RMOD_SNAFDRSAR(...) rset(SNAFDRSAR, SNAFDRSARrv, __VA_ARGS__)
#define RIZS_SNAFDRSAR(...) rset(SNAFDRSAR, 0, __VA_ARGS__)
#define RFLD_SNAFDRSAR(fld) (*((const volatile SNAFDRSAR_T *)0xb801a41c)).f.fld

typedef union {
	struct {
		u32_t mbz_0:31; //0
		u32_t dma_ip:1; //0
	} f;
	u32_t v;
} SNAFDIR_T;
#define SNAFDIRrv (*((regval)0xb801a420))
#define SNAFDIRdv (0x00000000)
#define RMOD_SNAFDIR(...) rset(SNAFDIR, SNAFDIRrv, __VA_ARGS__)
#define RIZS_SNAFDIR(...) rset(SNAFDIR, 0, __VA_ARGS__)
#define RFLD_SNAFDIR(fld) (*((const volatile SNAFDIR_T *)0xb801a420)).f.fld

typedef union {
	struct {
		u32_t mbz_0:2; //0
		u32_t dma_io_width:2; //0
		u32_t mbz_1:8; //0
		u32_t len:20; //0
	} f;
	u32_t v;
} SNAFDLR_T;
#define SNAFDLRrv (*((regval)0xb801a424))
#define SNAFDLRdv (0x00000000)
#define RMOD_SNAFDLR(...) rset(SNAFDLR, SNAFDLRrv, __VA_ARGS__)
#define RIZS_SNAFDLR(...) rset(SNAFDLR, 0, __VA_ARGS__)
#define RFLD_SNAFDLR(fld) (*((const volatile SNAFDLR_T *)0xb801a424)).f.fld

typedef union {
	struct {
		u32_t mbz_0:12; //0
		u32_t clenc:20; //0
	} f;
	u32_t v;
} SNAFDCDSR_T;
#define SNAFDCDSRrv (*((regval)0xb801a428))
#define SNAFDCDSRdv (0x00000000)
#define RMOD_SNAFDCDSR(...) rset(SNAFDCDSR, SNAFDCDSRrv, __VA_ARGS__)
#define RIZS_SNAFDCDSR(...) rset(SNAFDCDSR, 0, __VA_ARGS__)
#define RFLD_SNAFDCDSR(fld) (*((const volatile SNAFDCDSR_T *)0xb801a428)).f.fld

typedef union {
	struct {
		u32_t mbz_0:28; //0
		u32_t nfcos:1; //0
		u32_t nfdrs:1; //0
		u32_t nfdws:1; //0
		u32_t cs:1; //0
	} f;
	u32_t v;
} SNAFSR_T;
#define SNAFSRrv (*((regval)0xb801a440))
#define SNAFSRdv (0x00000000)
#define RMOD_SNAFSR(...) rset(SNAFSR, SNAFSRrv, __VA_ARGS__)
#define RIZS_SNAFSR(...) rset(SNAFSR, 0, __VA_ARGS__)
#define RFLD_SNAFSR(fld) (*((const volatile SNAFSR_T *)0xb801a440)).f.fld



/*************************************************************************
  *  Definitions of IO_WIDTH / Length / Status / PIO_Trigger / DMA_Trigger / CS_Select
  *************************************************************************/
#define	SIO_WIDTH 0
#define	DIO_WIDTH 1
#define	QIO_WIDTH 2
#define CMR_LEN(len) (len-1)
#define IO_WIDTH_LEN(io_width, len) (u32_t)((io_width<<28)|len)
#define WAIT_NAND_CTRLR_RDY() while(RFLD_SNAFSR(nfcos))
#define WAIT_DMA_CTRLR_RDY() while((SNAFSRrv&0x6)!=0)
#define DMA_READ_DIR  (0)
#define DMA_WRITE_DIR (1)


#define TRIG_CTRLR_PIO_WRITE(w_data, w_io_len) ({\
    WAIT_NAND_CTRLR_RDY();\
    SNAFWCMRrv= w_io_len;\
    SNAFWDRrv = w_data; /*Trigger PIO Write*/\
})

#define TRIG_CTRLR_PIO_READ(r_io_len) ({\
    WAIT_NAND_CTRLR_RDY(); \
    SNAFRCMRrv = r_io_len; /*Trigger PIO read*/ \
    WAIT_NAND_CTRLR_RDY();\
    SNAFRDRrv; /* return 4-BYTE data @REG32(SNFRDR)*/ \
})

#define TRIG_CTRLR_DMA_WRITE_READ(dma_phy_addr, dma_io_len, wr_dir)({\
    WAIT_NAND_CTRLR_RDY();\
    SNAFDRSARrv = dma_phy_addr;\
    SNAFDLRrv = dma_io_len;\
    WAIT_NAND_CTRLR_RDY();\
    SNAFDTRrv = wr_dir; /*Trigger DMA write or read*/\
    WAIT_DMA_CTRLR_RDY();\
})

#define ACT (0)
#define DEACT (1)
#define NAND_SPI_CS(act_deact)({\
    RMOD_SNAFCSR(cecs, act_deact);\
    while(RFLD_SNAFSR(cs)!=act_deact);\
})

#define ACTIVATE_NAND_SPI_CS() ({\
    WAIT_NAND_CTRLR_RDY();\
    NAND_SPI_CS(DEACT);\
    WAIT_NAND_CTRLR_RDY();\
    NAND_SPI_CS(ACT);\
})

#define DEACTIVE_NAND_SPI_CS() ({\
    WAIT_NAND_CTRLR_RDY();\
    NAND_SPI_CS(DEACT);\
    WAIT_NAND_CTRLR_RDY();\
})



/***********************************************
  *  SPI NAND Flash Controller Drivers & Function Pointer 
  ***********************************************/
extern nand_spi_model_info_t snaf_rom_general_model;

#define SNAF_DONT_CARE (0xFFFFFFFF)
u32_t _pio_raw_cmd(u32_t opcode, u32_t data, u32_t w_io_len, u32_t r_io_len);

void snaf_pio_read_data(nand_spi_flash_info_t *info, void *ram_addr, u32_t wr_bytes, u32_t blk_pge_addr, u32_t col_addr);
s32_t snaf_pio_write_data(nand_spi_flash_info_t *info, void *ram_addr, u32_t wr_bytes, u32_t blk_pge_addr, u32_t col_addr);
void snaf_page_read(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr);
s32_t snaf_page_write(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr);
s32_t snaf_page_read_with_ecc_decode(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf);
s32_t snaf_page_write_with_ecc_encode(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf);
#endif //#ifndef NAND_SPI_CTRL_H

