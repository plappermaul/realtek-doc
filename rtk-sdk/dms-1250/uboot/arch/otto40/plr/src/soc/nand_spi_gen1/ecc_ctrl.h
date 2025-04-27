#ifndef ECC_CTRL_H
#define ECC_CTRL_H

#include <soc.h>
#include <reg_map_util.h>

/***********************************************
  * Driver Section Definitions
  ***********************************************/
#ifndef SECTION_NAND_SPI
    #define SECTION_NAND_SPI 
#endif


/***********************************************
  * ECC Controller Registers
  ***********************************************/
typedef union {
	struct {
		u32_t mbz_0:2; //0
		u32_t ecc_cfg:2; //0
		u32_t mbz_1:1; //0
		u32_t debug_select:3; //0
		u32_t mbz_2:1; //0
		u32_t rbo:1; //0
		u32_t wbo:1; //0
		u32_t ie:1; //0
		u32_t mbz_3:5; //0
		u32_t slv_endian:1; //0
		u32_t dma_endian:1; //0
		u32_t precise:1; //1
		u32_t mbz_4:10; //0
		u32_t lbc_bsz:2; //3
	} f;
	u32_t v;
} ECC_CFG_T;
#define ECC_CFGrv (*((regval)0xb801a600))
#define ECC_CFGdv (0x00001003)
#define RMOD_ECCFR(...) rset(ECC_CFG, ECC_CFGrv, __VA_ARGS__)
#define RFLD_ECCFR(fld) (*((const volatile ECC_CFG_T *)0xb801a600)).f.fld


typedef union {
	struct {
		u32_t mbz_0:31; //0
		u32_t dmawren:1; //0
	} f;
	u32_t v;
} ECC_DMA_TRG_T;
#define ECC_DMA_TRGrv (*((regval)0xb801a608))
#define ECC_DMA_TRGdv (0x00000000)
#define RMOD_ECDTR(...) rset(ECC_DMA_TRG, ECC_DMA_TRGrv, __VA_ARGS__)
#define RIZS_ECDTR(...) rset(ECC_DMA_TRG, 0, __VA_ARGS__)
#define RFLD_ECDTR(fld) (*((const volatile ECC_DMA_TRG_T *)0xb801a608)).f.fld


typedef union {
	struct {
		u32_t addr:32; //0
	} f;
	u32_t v;
} ECC_DMA_START_ADDR_T;
#define ECC_DMA_START_ADDRrv (*((regval)0xb801a60c))
#define ECC_DMA_START_ADDRdv (0x00000000)
#define RMOD_ECDSAR(...) rset(ECC_DMA_START_ADDR, ECC_DMA_START_ADDRrv, __VA_ARGS__)
#define RIZS_ECDSAR(...) rset(ECC_DMA_START_ADDR, 0, __VA_ARGS__)
#define RFLD_ECDSAR(fld) (*((const volatile ECC_DMA_START_ADDR_T *)0xb801a60c)).f.fld


typedef union {
	struct {
		u32_t addr:32; //0
	} f;
	u32_t v;
} ECC_DMA_TAG_ADDR_T;
#define ECC_DMA_TAG_ADDRrv (*((regval)0xb801a610))
#define ECC_DMA_TAG_ADDRdv (0x00000000)
#define RMOD_ECDTAR(...) rset(ECC_DMA_TAG_ADDR, ECC_DMA_TAG_ADDRrv, __VA_ARGS__)
#define RIZS_ECDTAR(...) rset(ECC_DMA_TAG_ADDR, 0, __VA_ARGS__)
#define RFLD_ECDTAR(fld) (*((const volatile ECC_DMA_TAG_ADDR_T *)0xb801a610)).f.fld


typedef union {
	struct {
		u32_t mbz_0:12; //0
		u32_t eccn:8; //0
		u32_t mbz_1:3; //0
		u32_t ecer:1; //0
		u32_t mbz_2:3; //0
		u32_t all_one:1; //0
		u32_t mbz_3:3; //0
		u32_t eos:1; //0
	} f;
	u32_t v;
} ECC_STS_T;
#define ECSRrv (*((regval)0xb801a614))
#define ECSRdv (0x00000000)
#define RFLD_ECSR(fld) (*((const volatile ECC_STS_T *)0xb801a614)).f.fld


/***********************************************
  * ECC Controller Status
  ***********************************************/
#define ECC_CTRL_ERR                 (0xECDEAD00)
#define ECC_DECODE_SUCCESS           (0)
#define ECC_DECODE_ALL_ONE           (0xECCFFFFF)
#define IS_ECC_DECODE_FAIL(eccsts)   (((eccsts&0xFFFFFF00)==ECC_CTRL_ERR)?1:0)


/***********************************************
  * Macro for ECC Driver
  ***********************************************/
#define ECC_KICKOFF(IS_ENCODE) RMOD_ECDTR(dmawren, (IS_ENCODE!=0))
#define SET_ECC_DMA_START_ADDR(phy_addr) RMOD_ECDSAR(addr ,phy_addr)
#define SET_ECC_DMA_TAG_ADDR(phy_addr)   RMOD_ECDTAR(addr, phy_addr)
#define WAIT_ECC_CTRLR_RDY() while(RFLD_ECSR(eos))


/***********************************************
  * BCH Size & Type Definition
  ***********************************************/
#define BCH_SECTOR_PER_PAGE (4)
#define BCH_TAG_SIZE        (6)


/***********************************************
  * Export Information
  ***********************************************/
s32_t ecc_engine_action(void *dma_addr, void *p_eccbuf, s32_t is_bch12, s32_t is_encode);
void ecc_encode_bch(nand_spi_flash_info_t *info, void *dma_addr, void *p_eccbuf);
s32_t ecc_decode_bch(nand_spi_flash_info_t *info, void *dma_addr, void *p_eccbuf);

#endif //#ifndef ECC_CTRL_H

