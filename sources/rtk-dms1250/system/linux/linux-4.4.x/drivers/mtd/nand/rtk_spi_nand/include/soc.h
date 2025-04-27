#ifndef __SOC_H__
#define __SOC_H__

#include "luna_cfg.h" /*Add by SD6*/

#if defined(__x86_64) || defined(__i386)
    #define USE_32B_POINTER 1
#endif

#define ICACHE_SIZE    0x4000
#define DCACHE_SIZE    0x4000

#define SRAM_BASE 0x9F000000
#define SRAM_SIZE 0x10000

#define ECC_G1_WITHOUT_DUMMY_READY  0

//#define CONFIG_MTD_CMDLINE_PARTS 1  /*Add by SD6*/

/* To strip compiler warning message */
#ifndef TBASE
#define TBASE 0
#endif

#if (TBASE == 1)
#define FLASH_BASE 0x81000000
#else
#define FLASH_BASE 0x9FC00000
#endif

#ifndef __ASSEMBLER__
// primitive types
#ifndef __T_STYLE_TYPE__
#define __T_STYLE_TYPE__
typedef unsigned long long u64_t;
typedef unsigned int u32_t;
typedef unsigned short u16_t;
typedef unsigned char u8_t;
typedef int s32_t;
typedef short s16_t;
typedef char s8_t;
#endif

// constants and features for C only
#ifndef VZERO
    #define VZERO               ((void*)0)
#endif

#define OTTO_NAND_FLASH    1
#define OTTO_NOR_SPI_FLASH 0

#define SOC_ALIGN               __attribute__((aligned(4)))

#endif //__ASSEMBLER__
// constants and features for both C and assembly
// ...


// BEGIN OF IMPORT INFO
// FORM info.in and arch.h in the platform directory
#ifndef __ASSEMBLER__
    #define OTTO_SRAM_START 0x9f000000
    #define OTTO_SRAM_SIZE 0x10000
    #define OTTO_SRAM_MASK 0xFFFF
    #define OTTO_KSEG2_BASE 0xC0000000
    #define CACHELINE_SIZE 0x20
    #define TLB_PAGE_SHIFT 12
    #define TLB_PAGE_SIZE (1 << 12)
    #define TLB_PAGE_MASK ((1 << 12) - 1)
    #define TLB_NUM_TLB_ENTRY 64
    #define TLB_NUM_TLB_MASK 0x3f
    #define OTTO_LPLR_ENTRY 0x9fc00000
    #define OTTO_LPLR_SIZE 0x20000
    #define OTTO_LPLR_CHECKSUM_OFFSET 0x1c
    #define OTTO_SPI_NAND_PLR_ENTRY 0x9f000000
    #define OTTO_PARALLEL_NAND_PLR_ENTRY 0x9f000000
    #define OTTO_SPI_NOR_PLR_ENTRY 0x9fc00000
    #define OTTO_HEADER_OFFSET 0x20
    #define UART_BASE_ADDR 0xb8002000
    #define MAX_PAGE_BUF_SIZE 2048
    #define MAX_OOB_BUF_SIZE 128
    #define MAX_ECC_BUF_SIZE 32
    #define MAX_BYTE_PER_DMA 0x10000
    #define MAX_PLR_FIRST_LOAD_PAGE_NUM 64
    #define MAX_PLR_FIRST_LOAD_PAGE 32
    #define CC_PLAT_FLAGS -march=5281 -mtune=5281 -Wno-unused-result
    #define AS_PLAT_FLAGS -march=5281 -mtune=5281


    #define NAND_SPI_USE_SIO

    #define SRAM_CTRL_INIT li t0, 0xB8001300; lui t1, %hi(OTTO_SRAM_START); lui t3, 0x1FFF; and t1, t1, t3; ori t1, t1, 0x1; li t3, 0xB8004000; lw t2, 4(t3); sw t1, 0(t0); sw t2, 4(t0); sw t1, 0(t3);
    #define _lplr_bios (*(basic_io_t*)(OTTO_LPLR_ENTRY+OTTO_HEADER_OFFSET))
    #define _lplr_header (_lplr_bios.header)
#else //__ASSEMBLER__
    #define OTTO_SRAM_START 0x9f000000
    #define OTTO_SRAM_SIZE 0x10000
    #define OTTO_SRAM_MASK 0xFFFF
    #define OTTO_KSEG2_BASE 0xC0000000
    #define CACHELINE_SIZE 0x20
    #define TLB_PAGE_SHIFT 12
    #define TLB_PAGE_SIZE (1 << 12)
    #define TLB_PAGE_MASK ((1 << 12) - 1)
    #define TLB_NUM_TLB_ENTRY 64
    #define TLB_NUM_TLB_MASK 0x3f
    #define OTTO_LPLR_ENTRY 0x9fc00000
    #define OTTO_LPLR_SIZE 0x20000
    #define OTTO_LPLR_CHECKSUM_OFFSET 0x1c
    #define OTTO_SPI_NAND_PLR_ENTRY 0x9f000000
    #define OTTO_PARALLEL_NAND_PLR_ENTRY 0x9f000000
    #define OTTO_SPI_NOR_PLR_ENTRY 0x9fc00000
    #define OTTO_HEADER_OFFSET 0x20
    #define UART_BASE_ADDR 0xb8002000
    #define MAX_PAGE_BUF_SIZE 2048
    #define MAX_OOB_BUF_SIZE 128
    #define MAX_ECC_BUF_SIZE 32
    #define MAX_BYTE_PER_DMA 0x10000
    #define MAX_PLR_FIRST_LOAD_PAGE_NUM 64
    #define MAX_PLR_FIRST_LOAD_PAGE 32
    #define CC_PLAT_FLAGS -march=5281 -mtune=5281 -Wno-unused-result
    #define AS_PLAT_FLAGS -march=5281 -mtune=5281


    #define NAND_SPI_USE_SIO

    #define SRAM_CTRL_INIT li t0, 0xB8001300; lui t1, %hi(OTTO_SRAM_START); lui t3, 0x1FFF; and t1, t1, t3; ori t1, t1, 0x1; li t3, 0xB8004000; lw t2, 4(t3); sw t1, 0(t0); sw t2, 4(t0); sw t1, 0(t3);
    #define _lplr_bios (*(basic_io_t*)(OTTO_LPLR_ENTRY+OTTO_HEADER_OFFSET))
    #define _lplr_header (_lplr_bios.header)
#endif //__ASSEMBLER__

// END OF IMPORT INFO


#ifndef __ASSEMBLER__
// simple math
#define ADDR_ALIGN(addr, algn)          ({u32_t __addr=(u32_t)addr; u32_t __algn=(u32_t)algn; ((__addr+__algn-1)& -__algn);})
#define ADDR_ALIGN_LOWER(addr, algn)    ({u32_t __addr=(u32_t)addr; u32_t __algn=(u32_t)algn; (__addr& -__algn);})
#define CACHE_ALIGN(addr)               ADDR_ALIGN(addr, CACHELINE_SIZE)
#define NUM_WORD(bits)                  (((bits)+8*sizeof(u32_t)-1)/(8*sizeof(u32_t)))
#define COMBINE_U16_TO_U32(hh, lh)      ((((u32_t)((u16_t)hh))<<16)|((u16_t)lh))

typedef u32_t (plr_printf_t)(const char *, ...);
typedef void  (fpv_u32_u32_t) (u32_t, u32_t);
typedef u32_t  (fpu32_u32_u32_t) (u32_t, u32_t);
typedef void  (fpv_u32_t)(u32_t);
typedef u32_t (fpu32_u32_t)(u32_t);
typedef void  (fpv_s32_t)(s32_t);
typedef void  (fpv_s8_t)(s8_t);
typedef s8_t  (fps8_t)(void);
typedef s32_t (fps32_t)(void);
typedef void  (fpv_t)(void);
typedef void (fpv_s8_v_t)(s8_t, void *);

// primitive function
#ifndef REG32
#define REG32(addr)             (*((volatile u32_t *)(addr)))
#endif /* #ifndef REG32 */

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
#define DESC_ID_POSITION        1   // byte 0: field shift, byte 1: field length, byte 2: access ability bits
    #define DESC_ID_AB_R        0
    #define DESC_ID_AB_W        1
    #define DESC_ID_AB_W1C      2
    #define DESC_ID_AB_W0C      3

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

typedef int  (nor_spi_read) (u32_t cid, u32_t offset, u32_t len, void *buf);
typedef int  (nor_spi_write)(u32_t cid, u32_t offset, u32_t len, const void *buf);
typedef int  (nor_spi_erase)(u32_t cid, u32_t offset);

typedef struct {
    u8_t num_chips;
    u8_t addr_mode;             /* address mode, can only be 3 or 4. Read from strapped pin. */
    u8_t prefer_divisor;        /* CLK_M/divisor = SPI-F freq; values allowed: 2:2, 4:4, ..., 16: 16. */
    u8_t size_per_chip;         /* in power of 2, e.g., 17: 128KB, 18: 256KB, ..., 24: 16MB; */

    u8_t prefer_rx_delay0;
    u8_t prefer_rx_delay1;
    u8_t prefer_rx_delay2;
    u8_t prefer_rx_delay3;

    u8_t prefer_rd_cmd;         /* read command of the fastest mode for MMIO, if zero no fastest read. */
    u8_t prefer_rd_cmd_io;      /* 0: SIO, 1: DIO, 2: QIO */
    u8_t prefer_rd_dummy_c;     /* 0: 0, 2: 2, 4: 4, ... 14: 14 cycles */
    u8_t prefer_rd_addr_io;     /* 0: SIO, 1: DIO, 2: QIO */
    u8_t prefer_rd_data_io;     /* 0: SIO, 1: DIO, 2: QIO */

    u8_t wr_cmd;                /* the fastest write command supported, if zero no fastest read. */
    u8_t wr_cmd_io;             /* 0: SIO, 1: DIO, 2: QIO */
    u8_t wr_dummy_c;            /* 0: 0, 2: 2, 4: 4, ... 14: 14 cycles */
    u8_t wr_addr_io;            /* 0: SIO, 1: DIO, 2: QIO */
    u8_t wr_data_io;            /* 0: SIO, 1: DIO, 2: QIO */
    u8_t wr_boundary;           /* Max bytes per write cmd can do. Should be power of 2.
                                   0: No limit, 8: 256B, 9:512B, ... */

    u8_t erase_cmd;             /* cmd that can erase the min size of flash. */
    u8_t erase_unit;            /* bytes per erase cmd erases; in power of 2. */

    u8_t pm_method;             // method to switch to prefer mode, 0: NONE, 1: RWSR, 2: CMD, 3: R2W1SR, 4: EN0.
    u8_t pm_rdsr_cmd;           // command to read status for enabling prefer-mode. Needed by RWSR method.
    u8_t pm_rdsr2_cmd;          // command to read status 2 regster for enabling prefer-mode. Needed by R2W1SR method.
    u8_t pm_wrsr_cmd;           // command to write status. Needed by RWSR and R2W1SR method.
    u8_t pm_enable_cmd;         // command to enable prefer mode. Needed by CMD method.
    u16_t pm_enable_bits;       // bits ORed to SETed to status
    u8_t pm_status_len;         // bytes of status register which owns prefer-mode enable bits

    u8_t rdbusy_cmd;            // read busy bit command
    u8_t rdbusy_len;            // returned data size of read busy command
    u8_t rdbusy_loc;            // busy bit position
    u8_t rdbusy_polling_period; // in usec, in power of 2. period per read busy bit command. CPU should provide udelay().

    u8_t addr_4B_cmd;           // the command to enable 4-byte mode

    u32_t id;

    nor_spi_erase *_nor_spi_erase;
    nor_spi_read  *_nor_spi_read;
    nor_spi_write *_nor_spi_write;
} plr_nor_spi_info_t;

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

// for set _pll_query_freq
#define PLL_DEV_LX0     0x00
#define PLL_DEV_LX1     0x01
#define PLL_DEV_LX2     0x02
#define PLL_DEV_LX3     0x03
#define PLL_DEV_LX4     0x04
#define PLL_DEV_LX5     0x05
#define PLL_DEV_CPU     0x10
#define PLL_DEV_DSP     0x11
#define PLL_DEV_MEM     0x20
#define PLL_MHZ_UNKNOWN 0xFFFFFFFF

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
#define PLR_FL_GET_TOTAL_NUM_PAGE(fl)   (*(u16_t*)PLR_FL_TOTAL_NUM_LOCATION(fl))


#define BOOT_ROM_SPI_NAND      (1)
#define BOOT_ROM_PARALLEL_NAND (3)
typedef struct boot_strap_info_s {
    u32_t  _pin_boot_src:2;
    u32_t  _pin_nafc_rc:2;
    u32_t  _pin_nafc_ac:2;
    u32_t  _page_size:20;
    u32_t  _addr_cycle:3;
    u32_t  _dram_type:2;
    u32_t  _dummy:1;
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

struct parallel_nand_flash_info_s;
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
        struct parallel_nand_flash_info_s  *parl_nand_info;
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



/*
 * Following structure comes from u-boog/include/image.h
 * for descibing format of U-Boot/Kernel images.
 * All data in network byte order (aka natural aka bigendian).
 */
/* Note, only NONE and LZMA are supported. */
#define UIH_COMP_NONE  0 /*  No   Compression Used */
#define UIH_COMP_GZIP  1 /* gzip  Compression Used */
#define UIH_COMP_BZIP2 2 /* bzip2 Compression Used */
#define UIH_COMP_LZMA  3 /* lzma  Compression Used */
#define UIH_COMP_LZO   4 /* lzo   Compression Used */

typedef struct uimage_header {
    u32_t ih_magic;    /* Image Header Magic Number */
    u32_t ih_hcrc;     /* Image Header CRC Checksum */
    u32_t ih_time;     /* Image Creation Timestamp */
    u32_t ih_size;     /* Image Data Size */
    u32_t ih_load;     /* Data Load Address */
    u32_t ih_ep;       /* Entry Point Address */
    u32_t ih_dcrc;     /* Image Data CRC Checksum */
    u8_t  ih_os;       /* Operating System */
    u8_t  ih_arch;     /* CPU architecture */
    u8_t  ih_type;     /* Image Type */
    u8_t  ih_comp;     /* Compression Type */
    u8_t  ih_name[32]; /* Image Name */
} uimage_header_t;

/* Symbol table materials */
#define ENDING_SYMB_ID      0xffffffff

#if 0
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
    #define SCID_PARAMETERS                     3
        #define SP_NOR_SPIF_INFO                symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x10, 0)
        #define SPI_NAND_DEVICE_PROB_LIST_START symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 1)
        #define SPI_NAND_DEVICE_PROB_LIST_END   symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 2)
        #define SPI_NAND_SIO_CMD_INFO           symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 3)
        #define SPI_NAND_DIO_CMD_INFO           symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 4)
        #define SPI_NAND_DRV_VER                symb_dot_key(SGID_CONF_PARAMETER, SCID_PARAMETERS, 0x101, 5)

#define SGID_SPI_NAND_FUNC                      'S'
    #define SCID_CTRLR                          1
        #define SNAF_PIO_WRITE_FUNC             symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 1, 0)
        #define SNAF_PIO_READ_FUNC              symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 2, 0)
        #define SNAF_PAGE_WRITE_FUNC            symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 3, 0)
        #define SNAF_PAGE_READ_FUNC             symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 4, 0)
        #define SNAF_PAGE_WRITE_ECC_FUNC        symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 5, 0)
        #define SNAF_PAGE_READ_ECC_FUNC         symb_dot_key(SGID_SPI_NAND_FUNC, SCID_CTRLR, 6, 0)
    #define SCID_NAND_CHIP                      2
        #define SNAF_BLOCK_ERASE_FUNC           symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 1, 0)
        #define SNAF_WAIT_NAND_OIP_FUNC         symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 2, 0)
        #define SNAF_RESET_SPI_NAND_FUNC        symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 3, 0)
        #define SNAF_SET_FEATURE_FUNC           symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 4, 0)
        #define SNAF_GET_FEATURE_FUNC           symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 5, 0)
        #define SNAF_READ_SPI_NAND_FUNC         symb_dot_key(SGID_SPI_NAND_FUNC, SCID_NAND_CHIP, 6, 0)

#define SGID_ECC_CTRL_FUNC                      'E'
    #define SCID_ECC                            1
        #define ECC_BCH_ENCODE_FUNC             symb_dot_key(SGID_ECC_CTRL_FUNC, SCID_ECC, 1, 0)
        #define ECC_BCH_DECODE_FUNC             symb_dot_key(SGID_ECC_CTRL_FUNC, SCID_ECC, 2, 0)
        #define ECC_ENGINE_ACTION_FUNC          symb_dot_key(SGID_ECC_CTRL_FUNC, SCID_ECC, 3, 0)

#define SGID_NAND_PARL_FUNC                     'P'
    #define SCID_PNAF_CTRLR                     1
        #define PNAF_PIO_WRITE_FUNC             symb_dot_key(SGID_NAND_PARL_FUNC, SCID_PNAF_CTRLR, 1, 0)
        #define PNAF_PIO_READ_FUNC              symb_dot_key(SGID_NAND_PARL_FUNC, SCID_PNAF_CTRLR, 2, 0)
        #define PNAF_PIO_WRITE_ECC_FUNC         symb_dot_key(SGID_NAND_PARL_FUNC, SCID_PNAF_CTRLR, 5, 0)
        #define PNAF_PIO_READ_ECC_FUNC          symb_dot_key(SGID_NAND_PARL_FUNC, SCID_PNAF_CTRLR, 6, 0)
    #define SCID_PNAF_NAND_CHIP                 2
        #define PNAF_BLOCK_ERASE_FUNC           symb_dot_key(SGID_NAND_PARL_FUNC, SCID_PNAF_NAND_CHIP, 1, 0)
        #define PNAF_WAIT_NAND_CHIP_FUNC        symb_dot_key(SGID_NAND_PARL_FUNC, SCID_PNAF_NAND_CHIP, 2, 0)
        #define PNAF_RESET_NAND_CHIP_FUNC       symb_dot_key(SGID_NAND_PARL_FUNC, SCID_PNAF_NAND_CHIP, 3, 0)
        #define PNAF_GET_STATUS_FUNC            symb_dot_key(SGID_NAND_PARL_FUNC, SCID_PNAF_NAND_CHIP, 5, 0)
        #define PNAF_READ_ID_FUNC               symb_dot_key(SGID_NAND_PARL_FUNC, SCID_PNAF_NAND_CHIP, 6, 0)
        #define PNAF_CHECK_PROGRAM_ERASE_STS_FUNC symb_dot_key(SGID_NAND_PARL_FUNC, SCID_PNAF_NAND_CHIP, 7, 0)

#define SGID_VOIP_DSP_FUNC                      'V'
    #define SCID_VOIP_DSPRL                     0
        #define VOIP_LOG2_C                     symb_dot_key(SGID_VOIP_DSP_FUNC, SCID_VOIP_DSPRL, 1, 0)
#endif


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
#define CG_DEV_MEM  0x00000001
#define CG_DEV_LX   0x00000002
#define CG_DEV_SPIF 0x00000003

#define CG_OCP_WRONG  0x00000001
#define CG_MEM_WRONG  0x00000002
#define CG_LX_WRONG   0x00000004
#define CG_SPIF_WRONG 0x00000008

u32_t cg_query_freq(u32_t dev_type);


#endif //__ASSEMBLER__
#endif //__SOC_H__

