#ifndef __OTTO_SOC_H__
#define __OTTO_SOC_H__

#ifndef __ASSEMBLER__
#include <linux/types.h>
//typedef unsigned long int uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef uint64_t u64_t;
typedef uint32_t u32_t;
typedef uint16_t u16_t;
typedef uint8_t u8_t;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int s32_t;
typedef short s16_t;
typedef char s8_t;

typedef void  (fpv_t)(void);
typedef void  (fpv_s8_t)(s8_t);
typedef void  (fpv_s8_v_t)(s8_t, void *);
typedef void  (fpv_u32_t)(u32_t);
typedef void  (fpv_u32_u32_t) (u32_t, u32_t);
typedef void  (fpv_s32_t)(s32_t);
typedef s8_t  (fps8_t)(void);
typedef u32_t (fpu32_t)(void);
typedef u32_t (fpu32_u32_t)(u32_t);
typedef u32_t (fpu32_u32_u32_t) (u32_t, u32_t);
typedef s32_t (fps32_t)(void);

extern void invalidate_icache_all(void);
extern void invalidate_icache_range(unsigned long start, unsigned long end);
extern void writeback_invalidate_dcache_all(void);
extern void writeback_invalidate_dcache_range(unsigned long start, unsigned long end);
extern void writeback_dcache_all(void);
extern void writeback_dcache_range(unsigned long start, unsigned long end);
extern void invalidate_dcache_all(void);
extern void invalidate_dcache_range(unsigned long start, unsigned long end);
extern void writeback_invalidate_l23_all(void);
extern void writeback_invalidate_l23_range(unsigned long start, unsigned long end);
extern void writeback_l23_all(void);
extern void writeback_l23_range(unsigned long start, unsigned long end);
extern void invalidate_l23_all(void);
extern void invalidate_l23_range(unsigned long start, unsigned long end);


// primitive function
#ifndef REG32
#define REG32(addr)             (*((volatile uint32_t *)(addr)))
#endif /* #ifndef REG32 */
#define VZERO ((void *)0)
#define NORSF_CFLASH_BASE CONFIG_SYS_FLASH_BASE

// initialization result
typedef enum {
	INI_RES_UNINIT=0,
	INI_RES_OK=1,
	INI_RES_UNKNOWN_MODE,
	INI_RES_UNKNOWN_MODEL,
	INI_RES_FAIL,
	INI_RES_TEST_FAIL,
	INI_RES_DRAM_ZQ_CALI_FAIL,
	INI_RES_DRAM_SW_CALI_FAIL,
	PLL_RES_OK,
	PLL_RES_BAD_MODE,
	PLL_RES_BAD_OSC_FREQ,
	PLL_RES_FREQ_OUT_OF_RANGE,
} init_result_t;



/***********************************************
  * macro for virtual address to physical address
  ***********************************************/
#define UADDR(addr)             ((u32_t)(addr)|0x20000000)     // uncache address
#define CADDR(addr)             ((u32_t)(addr)&~0x20000000)    // cache address
#define PADDR(addr)             ((u32_t)(addr)&~0xE0000000)    // physical address



/* -----------------------------------
        symbol to exchange between lplr/plr, uboot, kernel
-------------------------------------- */
#define DID_GID_WIDTH   7
#define DID_CID_WIDTH   10
#define DID_FID_WIDTH   12
#define DID_DID_WIDTH   3
typedef struct {
        // when using mapped register field
    unsigned int group_id:DID_GID_WIDTH;
    unsigned int class_id:DID_CID_WIDTH;       // also ip-id
    unsigned int field_id:DID_FID_WIDTH;       // also function-id
    unsigned int desc_id:DID_DID_WIDTH;
} dot_id_t;
#define DESC_ID_BASE_ADDRESS    0
#define DESC_ID_POSITION                1       // byte 0: field shift, byte 1: field length, byte 2: access ability bits
        #define DESC_ID_AB_R            0
        #define DESC_ID_AB_W            1
        #define DESC_ID_AB_W1C          2
        #define DESC_ID_AB_W0C          3

typedef union {
    unsigned int        id;
    dot_id_t            dot_id;
} symb_key_t;

typedef struct {
    symb_key_t          key;
        union {
        void            *pvalue;
        unsigned int    ivalue;
    } v;
} symbol_table_entry_t;

/* -----------------------------------
	functions with run-level when initial
-------------------------------------- */
typedef struct {
    unsigned int        level;
    fpv_t               *fn;
} init_table_entry_t;

struct  cpu_info_s;
typedef struct cpu_info_s cpu_info_t;
struct  pll_info_s;
typedef struct pll_info_s pll_info_t;
struct  peri_info_s;
typedef struct peri_info_s peri_info_t;
struct mmu_info_s;
typedef struct mmu_info_s mmu_info_t;

typedef struct {
    // make sure the above model is correct
    unsigned int            signature;
    // version = [31:24]: major version, [23:8]: regular rlz version, [7:0]: reserved
    unsigned int            version;

    #ifndef USE_32B_POINTER
        // export symbols to uboot
        symbol_table_entry_t    *export_symb_list;
        symbol_table_entry_t    *end_of_export_symb_list;

        // init function will be perfored during ROM-code and preloader phase
        init_table_entry_t      *init_func_list;
        init_table_entry_t      *end_of_init_func_list;
    #else
        u32_t               four_pointer_here[4];
    #endif
} soc_stru_header_t;


#define SIGNATURE_LPLR      0x33393633
#define SIGNATURE_PLR       0x30333433
#define SIGNATURE_PLR_FL    0x27317223
#define SIGNATURE_UBOOT     0x35323130
#define MAGIC_LOADER        0x6C6F6164  //Magic Number of Bootloader
#define MAGIC_SQUASH        0x68737173  //Magic Number of Kernel
#define UIMG_MAGIC_NUM      MAGIC_LOADER
#define MAGIC_UBOOT_2011    0x27051956

typedef struct {
    // --------------
    //      This structure is used to Communication between lpreloader and preloader only, not intent to change.
    //      expection: the export_func_list may be used by u-boot.
    // --------------
    soc_stru_header_t       header;

    // The irq entry point, if necessary
    // Saving context and preparing stack should be done in this isr
    // CAUTION: the position from the beginning should be exactly 4x6 = __24__ bytes (or 0x38 from 9fc0_0000)
    #ifndef USE_32B_POINTER
        fpv_t               *isr;                      // isr entry point;
    #else
        u32_t               one_pointer_here;
    #endif

    u32_t                   size_of_plr_load_firstly;   // used only for lplr+plr and plr tells lplr how many bytes to loader firstly. (in bytes)

    // character io, which is exported by plr and used by lplr
    fpv_s8_t                *uart_putc;
    fps32_t                 *uart_getc;
    fps32_t                 *uart_tstc;

    // basic cache operation
    fpv_t                   *dcache_writeback_invalidate_all;
    fpv_u32_u32_t           *dcache_writeback_invalidate_range;
    fpv_t                   *icache_invalidate_all;
    fpv_u32_u32_t           *icache_invalidate_range;
} basic_io_t;

typedef u8_t mac_addr_t[6];

typedef struct {
    u16_t bbi;
    u16_t reserved;
    u32_t signature;
    union {
        struct {
            u32_t idx_copy:16;
            u32_t idx_chunk:16;
        };
        u32_t chunk_num;
    };
    u32_t startup_num;
    u32_t total_chunk;
    u32_t num_copy;
} plr_oob_t;

typedef struct {
    plr_oob_t plr_oob;
    u8_t  dummy_array[sizeof(plr_oob_t)];    
} plr_oob_4kpage_t;

#define BCH_SECTOR_SIZE                 (512)
#define PLR_FL_PAGE_USAGE               (BCH_SECTOR_SIZE+BCH_SECTOR_SIZE)
#define BCH6_SYNDROME_SIZE              (10)
#define BCH12_SYNDROME_SIZE             (20)
#define BCH12_ECC_BUF_DUMMY_SIZE        (2)
#define PLR_FL_OOB_SIZE_PER_SECTOR      (6)
#define PLR_FL_DYMMY_SIZE_PER_SECTOR    (2)

typedef struct {
    char data0[BCH_SECTOR_SIZE];
    char oob0[PLR_FL_OOB_SIZE_PER_SECTOR];
    char syndrome0[BCH12_SYNDROME_SIZE];
    char dummy0_ff[PLR_FL_DYMMY_SIZE_PER_SECTOR];
    char data1[BCH_SECTOR_SIZE];
    char oob1[PLR_FL_OOB_SIZE_PER_SECTOR];
    char syndrome1[BCH12_SYNDROME_SIZE];
    char dummy1_ff[PLR_FL_DYMMY_SIZE_PER_SECTOR];
} plr_first_load_layout_t;

#define PLR_FL_SIG_LOCATION(fl)         ((fl)->oob0)
#define PLR_FL_PAGE_INDEX_LOCATION(fl)  ((fl)->oob1)
#define PLR_FL_TOTAL_NUM_LOCATION(fl)   ((fl)->oob1+2)
#define PLR_FL_SIZE_PER_PAGE            (sizeof(plr_first_load_layout_t))
#define PLR_FL_GET_SIGNATURE(fl)        ({ u32_t *__plrsig=(u32_t*)PLR_FL_SIG_LOCATION(fl); *__plrsig; })
#define PLR_FL_GET_PAGE_INDEX(fl)       ({ u16_t *__idx=(u16_t*)PLR_FL_PAGE_INDEX_LOCATION(fl); *__idx; })
#define PLR_FL_GET_TOTAL_NUM_PAGE(fl)   ({ u16_t *__tnp=(u16_t*)PLR_FL_TOTAL_NUM_LOCATION(fl); *__tnp; })


#define BOOT_ROM_SPI_NAND   (1)
#define BOOT_ROM_ONFI       (3)
typedef struct boot_strap_info_s {
    u32_t  pin_boot_src:2;
    u32_t  pin_nafc_rc:2; 
    u32_t  pin_nafc_ac:2;
    u32_t  page_size:19;
    u32_t  addr_cycle:3;
    u32_t  cmd_cycle:2;
    u32_t  dram_type:2;
}boot_strap_info_t;


/* --------------------------------------------
   PERIPHERAL PARAMETERS
-------------------------------------------- */
struct peri_info_s {
    u32_t                   baudrate;
    mac_addr_t              mac_addr;
};

typedef union {
	struct {
		u32_t cal:8;
		u32_t to_reg:5;
		u32_t cntlr_zq:6;
		u32_t xlat:8;
		u32_t probe:5;
	} f;
	u32_t v;
} mem_init_result_t;

struct onfi_info_s;
struct spi_nand_flash_info_s;

// parameter structure
typedef struct {
    basic_io_t              bios;

    // state.chip
    u32_t                   cid, sid;

    //..........
    // share flash info between lpreloader and preloader for NAND case with rom-code
    // otherwise, it is used only within preloader;
    // Note, in case of sharing between uboot, the export_func_list mechanism should be applied.
    union{
        struct spi_nand_flash_info_s       *spi_nand_info;        
        struct onfi_info_s  *onfi_info;
    }flash_info;

	// bad block table: a bit array indicating the bad block when necessary (for NAND and SPI NAND flash)
    u32_t                   *bbt;

    // DDR info.
    // pointer to DDR parameters which from the DDR parameters data base
    void                    *dram_info;

	// pll info.
    // pointer to pll dbs
    pll_info_t              *pll_info;

    // mmu
    mmu_info_t              *mmu_info;

    // peri
    peri_info_t             *peri_info;

    // state.init
    mem_init_result_t       dram_init_result;
    u64_t                   dram_init_warning;
    init_result_t           flash_init_result;
} soc_t;

/* Symbol table materials */
#define ENDING_SYMB_ID      0xffffffff

#define _key_mask(k, w) ((k)&((1<<w)-1))

#define symb_dot_key(gid ,cid, fid, did) \
    (((((((_key_mask(gid, DID_GID_WIDTH) << DID_CID_WIDTH) | _key_mask(cid, DID_CID_WIDTH)) \
    << DID_FID_WIDTH) | _key_mask(fid, DID_FID_WIDTH)) ) << DID_DID_WIDTH) | _key_mask(did, DID_DID_WIDTH))

// define the group of utility functions
#define SGID_UTIL_FUNC                          'U'
    #define SCID_INIT                           1
        #define SF_RUN_INIT_LEVEL               symb_dot_key(SGID_UTIL_FUNC, SCID_INIT, 0x10, 0)
    #define SCID_CHAR_IO                        3
        #define SF_PRINTF                       symb_dot_key(SGID_UTIL_FUNC, SCID_CHAR_IO, 0x10, 0)
        #define SF_PROTO_PRINTF                 symb_dot_key(SGID_UTIL_FUNC, SCID_CHAR_IO, 0x10, 1)
    #define SCID_SYSTEM                         5
        #define SF_SYS_CACHE_I_INV              symb_dot_key(SGID_UTIL_FUNC, SCID_SYSTEM, 0x10, 0)
        #define SF_SYS_CACHE_D_FLUSH_INV        symb_dot_key(SGID_UTIL_FUNC, SCID_SYSTEM, 0x11, 0)
        #define SF_SYS_CACHE_D_FLUSH_INV_ALL    symb_dot_key(SGID_UTIL_FUNC, SCID_SYSTEM, 0x12, 0)
        #define SF_SYS_UDELAY                   symb_dot_key(SGID_UTIL_FUNC, SCID_SYSTEM, 0x20, 0)
        #define SF_SYS_MDELAY                   symb_dot_key(SGID_UTIL_FUNC, SCID_SYSTEM, 0x21, 0)
        #define SF_SYS_GET_TIMER                symb_dot_key(SGID_UTIL_FUNC, SCID_SYSTEM, 0x22, 0)
        #define SF_SYS_LX_TIMER_INIT            symb_dot_key(SGID_UTIL_FUNC, SCID_SYSTEM, 0x23, 0)
        #define SF_SYS_CG_DEV_FREQ              symb_dot_key(SGID_UTIL_FUNC, SCID_SYSTEM, 0x31, 0)
    #define SCID_MT_FUNC                        7
        #define SF_MT_NORMAL_TEST_PATTERN       symb_dot_key(SGID_UTIL_FUNC, SCID_MT_FUNC, 1, 0)
        #define SF_MT_WORD_PATTERN_ROTATE       symb_dot_key(SGID_UTIL_FUNC, SCID_MT_FUNC, 2, 0)
        #define SF_MT_HALFWORD_PATTERN_ROTATE   symb_dot_key(SGID_UTIL_FUNC, SCID_MT_FUNC, 3, 0)
        #define SF_MT_BYTE_PATTERN_ROTATE       symb_dot_key(SGID_UTIL_FUNC, SCID_MT_FUNC, 4, 0)
        #define SF_MT_UNALIGNED_WR              symb_dot_key(SGID_UTIL_FUNC, SCID_MT_FUNC, 5, 0)
        #define SF_MT_ADDR_ROT                  symb_dot_key(SGID_UTIL_FUNC, SCID_MT_FUNC, 6, 0)
        #define SF_MT_COM_ADDR_ROT              symb_dot_key(SGID_UTIL_FUNC, SCID_MT_FUNC, 7, 0)
        #define SF_MT_WALKING_OF_0              symb_dot_key(SGID_UTIL_FUNC, SCID_MT_FUNC, 8, 0)
        #define SF_MT_WALKING_OF_1              symb_dot_key(SGID_UTIL_FUNC, SCID_MT_FUNC, 9, 0)
        #define SF_MT_MEMCPY                    symb_dot_key(SGID_UTIL_FUNC, SCID_MT_FUNC, 10, 0)
    #define SCID_LIB                            9
        #define SF_LIB_LZMA_DECODE              symb_dot_key(SGID_UTIL_FUNC, SCID_LIB, 0, 0)
    #define SCID_STRING                         11
        #define SCID_STR_ATOI                   symb_dot_key(SGID_UTIL_FUNC, SCID_STRING, 1, 0)
        #define SCID_STR_STRCPY                 symb_dot_key(SGID_UTIL_FUNC, SCID_STRING, 2, 0)
        #define SCID_STR_STRLEN                 symb_dot_key(SGID_UTIL_FUNC, SCID_STRING, 3, 0)
        #define SCID_STR_STRCMP                 symb_dot_key(SGID_UTIL_FUNC, SCID_STRING, 4, 0)
        #define SCID_STR_MEMCPY                 symb_dot_key(SGID_UTIL_FUNC, SCID_STRING, 5, 0)
        #define SCID_STR_MEMSET                 symb_dot_key(SGID_UTIL_FUNC, SCID_STRING, 6, 0)
        #define SCID_STR_MASSCPY                symb_dot_key(SGID_UTIL_FUNC, SCID_STRING, 30, 0)
        
#define SGID_CONF_PARAMETER                     'C'
    #define SCID_STORAGE                        1
        #define SF_BOOT_STORAGE_TYPE            symb_dot_key(SGID_CONF_PARAMETER, SCID_STORAGE, 0x01, 0)
            #define                             BOOT_FROM_UNKNOWN   0x00 
            #define                             BOOT_FROM_NFBI      0x01
            #define                             BOOT_FROM_SPI_NOR   0x11
            #define                             BOOT_FROM_SPI_NAND  0x21
            #define                             BOOT_FROM_ONFI      0x31
    #define SCID_PARAMETERS                     3
        #define SP_NOR_SPIF_INFO                symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x10, 0)
        #define SPI_NAND_DEVICE_PROB_LIST_START symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 1)
        #define SPI_NAND_DEVICE_PROB_LIST_END   symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 2)
        #define SPI_NAND_SIO_CMD_INFO           symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 3)
        #define SPI_NAND_DIO_CMD_INFO           symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 4)
        #define SPI_NAND_DRV_VER                symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 5)
        #define ONFI_DEVICE_PROB_LIST_START     symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 6)
        #define ONFI_DEVICE_PROB_LIST_END       symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 7)
        #define SPI_NAND_X2_CMD_INFO            symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 8)
        #define ONFI_DRV_VER                    symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 9)
  

#define SGID_DEBUG_ANNOTATION                   'D'
    #define SCID_ENV                            1
        #define ENV_VCS_VERSION                 symb_dot_key(SGID_DEBUG_ANNOTATION, SCID_ENV, 0x200, 0)
        #define ENV_BUILD_DATE                  symb_dot_key(SGID_DEBUG_ANNOTATION, SCID_ENV, 0x200, 1)
        #define ENV_BUILD_KIT                   symb_dot_key(SGID_DEBUG_ANNOTATION, SCID_ENV, 0x200, 2)

#define SGID_SPI_NAND_FUNC                      'S'
    #define SCID_CTRLR                          1
        #define SNAF_PIO_WRITE_FUNC             symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 1, 0)
        #define SNAF_PIO_READ_FUNC              symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 2, 0)
        #define SNAF_PAGE_WRITE_FUNC            symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 3, 0)
        #define SNAF_PAGE_READ_FUNC             symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 4, 0)
        #define SNAF_PAGE_WRITE_ECC_FUNC        symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 5, 0)
        #define SNAF_PAGE_READ_ECC_FUNC         symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 6, 0)
        #define SNAF_PAGE_WRITE_ODE_FUNC        symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 7, 0)
        #define SNAF_PAGE_READ_ODE_FUNC         symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 8, 0)
        #define SNAF_SEND_INSTR_GEN_FUNC        symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 9, 0)
    #define SCID_NAND_CHIP                      2
        #define SNAF_BLOCK_ERASE_FUNC           symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 1, 0)
        #define SNAF_WAIT_NAND_OIP_FUNC         symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 2, 0)
        #define SNAF_RESET_SPI_NAND_FUNC        symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 3, 0)
        #define SNAF_SET_FEATURE_FUNC           symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 4, 0)
        #define SNAF_GET_FEATURE_FUNC           symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 5, 0)
        #define SNAF_READ_SPI_NAND_FUNC         symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 6, 0)
        #define SNAF_DISABLE_ODE_FUNC           symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 7, 0)
        #define SNAF_ENABLE_ODE_FUNC            symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 8, 0)
        #define SNAF_BLOCK_UNPROTECT_FUNC       symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 9, 0)
        #define SNAF_STACK2DIE_ADDR_TRANS_FUNC  symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 10,0)
 
#define SGID_ECC_CTRL_FUNC                      'E'
    #define SCID_ECC                            1
        #define ECC_BCH_ENCODE_FUNC             symb_dot_key(SGID_ECC_CTRL_FUNC, SCID_ECC, 1, 0)
        #define ECC_BCH_DECODE_FUNC             symb_dot_key(SGID_ECC_CTRL_FUNC, SCID_ECC, 2, 0)
        #define ECC_ENGINE_ACTION_FUNC          symb_dot_key(SGID_ECC_CTRL_FUNC, SCID_ECC, 3, 0)
        #define ECC_BCH_ENCODE_4KPAGE_FUNC      symb_dot_key(SGID_ECC_CTRL_FUNC, SCID_ECC, 4, 0)
        #define ECC_BCH_DECODE_4KPAGE_FUNC      symb_dot_key(SGID_ECC_CTRL_FUNC, SCID_ECC, 5, 0)
        
#define SGID_ONFI_FUNC                          'O'
    #define SCID_ONFI_CTRLR                     1
        #define ONAF_PIO_READ_FUNC              symb_dot_key(SGID_ONFI_FUNC, SCID_ONFI_CTRLR, 1, 0)
        #define ONAF_PIO_WRITE_FUNC             symb_dot_key(SGID_ONFI_FUNC, SCID_ONFI_CTRLR, 2, 0)
        #define ONAF_PAGE_READ_FUNC             symb_dot_key(SGID_ONFI_FUNC, SCID_ONFI_CTRLR, 3, 0)
        #define ONAF_PAGE_WRITE_FUNC            symb_dot_key(SGID_ONFI_FUNC, SCID_ONFI_CTRLR, 4, 0)
        #define ONAF_PAGE_READ_ECC_FUNC         symb_dot_key(SGID_ONFI_FUNC, SCID_ONFI_CTRLR, 5, 0)
        #define ONAF_PAGE_WRITE_ECC_FUNC        symb_dot_key(SGID_ONFI_FUNC, SCID_ONFI_CTRLR, 6, 0)
        #define ONAF_PAGE_READ_ODE_FUNC         symb_dot_key(SGID_ONFI_FUNC, SCID_ONFI_CTRLR, 7, 0)
        #define ONAF_PAGE_WRITE_ODE_FUNC        symb_dot_key(SGID_ONFI_FUNC, SCID_ONFI_CTRLR, 8, 0)
    #define SGID_ONFI_CHIP                 2
        #define ONFI_RESET_NAND_CHIP_FUNC       symb_dot_key(SGID_ONFI_FUNC, SGID_ONFI_CHIP, 1, 0)
        #define ONFI_BLOCK_ERASE_FUNC           symb_dot_key(SGID_ONFI_FUNC, SGID_ONFI_CHIP, 2, 0)
        #define ONFI_WAIT_NAND_CHIP_FUNC        symb_dot_key(SGID_ONFI_FUNC, SGID_ONFI_CHIP, 3, 0)
        #define ONFI_STATUS_READ_FUNC           symb_dot_key(SGID_ONFI_FUNC, SGID_ONFI_CHIP, 4, 0)
        #define ONFI_CHK_PROG_ERASE_STS_FUNC    symb_dot_key(SGID_ONFI_FUNC, SGID_ONFI_CHIP, 5, 0)
        #define ONFI_READ_ID_FUNC               symb_dot_key(SGID_ONFI_FUNC, SGID_ONFI_CHIP, 6, 0)
        #define ONFI_SET_FEATURE_FUNC           symb_dot_key(SGID_ONFI_FUNC, SGID_ONFI_CHIP, 7, 0)
        #define ONFI_GET_FEATURE_FUNC           symb_dot_key(SGID_ONFI_FUNC, SGID_ONFI_CHIP, 8, 0)

#define SGID_VOIP_DSP_FUNC                      'V'
    #define SCID_VOIP_DSPRL                     0
        #define VOIP_LOG2_C                     symb_dot_key(SGID_VOIP_DSP_FUNC, SCID_VOIP_DSPRL, 1, 0)



// define the group of nand spi drivers
#define SGID_NAND_SPI                   'A'
    #define SCID_Mxxxx_READ_yyyy

// define the group of patch purpose
#define SGID_PATCH_FUNC                 'P'

// define the group of register and/or field remapping
#define SGID_REGFIELD_REMAP             'R'

// define class id's

/* For clock generation module. */
#define CG_DEV_OCP  0x00000000
#define CG_DEV_CPU0 0x00000000
#define CG_DEV_MEM  0x00000001
#define CG_DEV_LX   0x00000002
#define CG_DEV_SPIF 0x00000003
#define CG_DEV_CPU1 0x00000004

#define CG_OCP_WRONG  0x00000001
#define CG_MEM_WRONG  0x00000002
#define CG_LX_WRONG   0x00000004
#define CG_SPIF_WRONG 0x00000008

u32_t cg_query_freq(u32_t dev_type);

#define GET_CPU_MHZ()	(cg_query_freq(CG_DEV_CPU0))
#define GET_CPU0_MHZ	GET_CPU_MHZ
#define GET_CPU1_MHZ()	(cg_query_freq(CG_DEV_CPU1))
#define GET_MEM_MHZ()	(cg_query_freq(CG_DEV_MEM))
#define GET_LX_MHZ()	(cg_query_freq(CG_DEV_LX))
#define GET_SPIF_MHZ()	(cg_query_freq(CG_DEV_SPIF))

/* define bismarck version */
#define bismarck_ver 0x04000000  /* means 4.0 */

#define OTTO_SRAM_START         (0x9f000000)
#define OTTO_HEADER_OFFSET      (0x20)

#endif //__ASSEMBLER__
#include <common.h>
#endif //__OTTO_SOC_H__
