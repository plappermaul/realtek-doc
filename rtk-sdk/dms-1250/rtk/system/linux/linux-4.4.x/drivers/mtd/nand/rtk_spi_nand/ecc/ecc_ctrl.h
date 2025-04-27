#ifndef ECC_CTRL_H
#define ECC_CTRL_H

#include <soc.h>
#include <reg_map_util.h>
#include <ecc/ecc_struct.h>


/***********************************************
  * Driver Section Definitions
  ***********************************************/
#ifndef SECTION_ECC_CTRL
    #define SECTION_ECC_CTRL
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


typedef union {
    struct {
        u32_t mbz_0:31; //0
        u32_t dma_ip:1; //0
    } f;
    u32_t v;
} ECIR_T;
#define ECIRrv (*((regval)0xb801a618))
#define ECIRdv (0x00000000)
#define RMOD_ECIR(...) rset(ECIR, ECIRrv, __VA_ARGS__)
#define RIZS_ECIR(...) rset(ECIR, 0, __VA_ARGS__)
#define RFLD_ECIR(fld) (*((const volatile ECC_STS_T *)0xb801a618)).f.fld


/***********************************************
  * ECC Status
  ***********************************************/
#define ECC_ERROR_FLAG               (0xECDEAD00)

#define ECC_CTRL_ERR                 (ECC_ERROR_FLAG)
#define ECC_DECODE_SUCCESS           (0)
#define ECC_DECODE_ALL_ONE           (0xECCFFFFF)

#define ECC_USE_ODE_ERR              (ECC_ERROR_FLAG | 0xDE)
#define ECC_USE_ODE_SUCCESS          (ECC_DECODE_SUCCESS)
#define IS_ECC_DECODE_FAIL(eccsts)   (((eccsts&0xFFFFFF00)==ECC_ERROR_FLAG)?1:0)


/***********************************************
  * Macro for ECC Driver
  ***********************************************/
#define ECC_KICKOFF(IS_ENCODE) RMOD_ECDTR(dmawren, (IS_ENCODE!=0))
#define SET_ECC_DMA_START_ADDR(phy_addr) RMOD_ECDSAR(addr ,phy_addr)
#define SET_ECC_DMA_TAG_ADDR(phy_addr)   RMOD_ECDTAR(addr, phy_addr)
#if (ECC_G1_WITHOUT_DUMMY_READY == 1)
    #define ECC_FEATURE_SET(bch_ability) RMOD_ECCFR(ecc_cfg, bch_ability)
    #define WAIT_ECC_CTRLR_RDY() while(RFLD_ECSR(eos))
#else
    #define ECC_FEATURE_SET(bch_ability) do { \
            RMOD_ECCFR(ecc_cfg, bch_ability, \
                       ie,      1);          \
            RMOD_ECIR(dma_ip, 1);            \
        } while (0)
    #define WAIT_ECC_CTRLR_RDY() while(ECSRrv & 0x3)
#endif

/***********************************************
  * Function Pointer Definition
  ***********************************************/
typedef s32_t (ecc_engine_t)(u32_t ecc_ability, void *dma_addr, void *p_eccbuf, u32_t is_encode);
typedef void  (ecc_encode_t)(u32_t ecc_ability, void *dma_addr, void *p_eccbuf);
typedef s32_t (ecc_decode_t)(u32_t ecc_ability, void *dma_addr, void *p_eccbuf);


/***********************************************
  * Export Information
  ***********************************************/
s32_t ecc_engine_action(u32_t ecc_ability, void *dma_addr, void *p_eccbuf, u32_t is_encode);
void ecc_encode_bch_sector(u32_t ecc_ability, void *dma_addr, void *p_eccbuf, u32_t sector_per_page);
s32_t ecc_decode_bch_sector(u32_t ecc_ability, void *dma_addr, void *p_eccbuf, u32_t sector_per_page);
void ecc_encode_bch(u32_t ecc_ability, void *dma_addr, void *p_eccbuf);
s32_t ecc_decode_bch(u32_t ecc_ability, void *dma_addr, void *p_eccbuf);


/***********************************************
  * Software Check Patch Relative
  ***********************************************/
#define NOPX10() \
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;")

#define NOPX5() \
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");\
__asm__ __volatile__ ("nop;");

#define ALL_FF (0xFFFFFFFF)


#endif //#ifndef ECC_CTRL_H

