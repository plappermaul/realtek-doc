#ifndef SOC_H
#define SOC_H

#define ICACHE_SIZE    0x4000
#define DCACHE_SIZE    0x4000
#define CACHELINE_SIZE 16

#define SRAM_BASE 0x9F000000
#define SRAM_SIZE 0x10000

#if (TBASE == 1)
#define FLASH_BASE 0x81000000
#else
#define FLASH_BASE 0x9FC00000
#endif

#define SPI_NOR_FLASH_MMIO_BASE0 (0x94000000)
#define SPI_NOR_FLASH_MMIO_SIZE0 (0x04000000) /* 64MB */
#define SPI_NOR_FLASH_MMIO_BASE1 (0x9FC00000)
#define SPI_NOR_FLASH_MMIO_SIZE1 (0x00400000) /* 4MB */

/* Choose the decompress algorithm builtin */
#define DECOMP_ALGO_LZMA 1
#define DECOMP_ALGO_GZIP 0

#define OTTO_NAND_FLASH    0
#define OTTO_NOR_SPI_FLASH 1

// DDR controller generation
#define OTTO_DRAM_GEN 2
#define OTTO_PLL_GEN 3

/* Returns current CPU cycle. */
#define OTTO_CPU_CYCLE() \
	({ \
		int __res; \
		__asm__ __volatile__("mfc0 %0, $9;" \
		                     : "=r" (__res)); \
		__res *= 2; \
		__res; \
	})

/* Get flash controller's address mode */
#define OTTO_FLASH_ADDR_MODE() \
	({ \
		int __res = 3; \
		*((volatile u32_t *)(0xbb000058)) = 0x3; \
		if (*((volatile u32_t *)(0xbb00100c)) & (1<<29)) { \
			__res = 4; \
		} \
		__res; \
	})

// using spare area to store more DRAM/FLASH database
#define ON_FLASH_VMA_GAP 0xc00000
#define SOC_NUM_DRAM_SPARE 2
#define SOC_NUM_FLASH_SPARE 2
#define PRINTF_SECTION SECTION_ON_FLASH
#define LZMA_SECTION SECTION_ON_FLASH
#define PLR_ENABLE_PLL_SET
//#define LPLR_WAIT_ICE_DELAY_LOOP 250000

#if (SOC_NUM_DRAM_SPARE == 0)
    #undef SOC_NUM_DRAM_SPARE
#endif

#if (SOC_NUM_FLASH_SPARE == 0)
    #undef SOC_NUM_FLASH_SPARE
#endif

#if (defined(SOC_NUM_DRAM_SPARE) || defined(SOC_NUM_FLASH_SPARE))
    #define USE_SOC_SPARE
#endif

#ifndef __ASSEMBLER__
#define ADDR_ALIGN(addr, algn) (((size_t)(addr)+((algn)-1))&(-(algn)))
#define CACHE_ALIGN(addr) ADDR_ALIGN(addr, CACHELINE_SIZE)

typedef unsigned int u32_t;
typedef unsigned short u16_t;
typedef unsigned char u8_t;
typedef int s32_t;
typedef short s16_t;
typedef char s8_t;

#ifndef REG32
#define REG32(addr) (*((volatile u32_t *)(addr)))
#endif /* #ifndef REG32 */

#define NUM_WORD(bits)          (((bits)+8*sizeof(u32_t)-1)/(8*sizeof(u32_t)))
#define VZERO                   ((void*)0)
#define SOC_ALIGN               __attribute__((aligned(4)))

#define SOC_A0                  0x0371
#define SOC_B0                  0x6266
#define SYSTEM_STARTUP_CADDR    0x9FC00000
#define SYSTEM_STARTUP_UADDR    0xBFC00000
#define DRAM_CADDR              0x80000000
#define LPLR_DMA_SIZE           4096
#define NUM_LPLR                4           // num of copies of preloader that booting DMA uses
#define MAX_COPY_BLR            2
#define MAX_SPARESPACE          128         // should cache aligned
#define MAX_BLOCKS              2048
#define MAX_PLR_CHUNK           128
#define MAX_BLR_CHUNK           4096
#define MAX_CHUNK_SIZE          2048
#define SOC_CONF_OFFSET         32          // define where soc_configuration_t
#define SOC_HEADER_VERSION      0x00000924  // which means 0.0.9.36

#define UADDR(addr)             ((size_t)(addr)|0x20000000)     // uncache address
#define CADDR(addr)             ((size_t)(addr)&~0x20000000)    // cache address
#ifndef PADDR /* Avoid re-defintion when being included into linux kernel */
#define PADDR(addr)             ((size_t)(addr)&~0xE0000000)    // physical address
#endif /* #ifndef PADDR */

typedef struct {
    u16_t page_size;
    u16_t num_block;            //block number of flash.

    u16_t mtd_offset;
    u16_t bbi_dma_offset;       // the offset of bad block indicator (after dma read ,in dram area)

    u16_t bbi_raw_offset;       // the offset of bad block indicator (in nand flash)
    u16_t bbi_swap_offset;      // the offset of data replaced by bad block indicator (in dma read oob area)

    u16_t num_page_per_block;   // the real page numbers per block.
    u16_t num_pblr_block;       /* total number of good blocks for preloader and bootloader, the number should be
                                  'number of block for double preloader and bootloader' + 2, for example
                                  two preloader plus bootloader needs 3 blocks
                                  then num_pblr_block = 3 + 2 = 5 */
    u8_t enable_plr_hcheck;     // enable preloader/bootloader healthy check
    u8_t enable_plr_gc;         // enable garbage collection when health check needed (meanful only for enable_plr_hcheck=1
    u8_t cmd_set_id;
    u8_t addr_cycles;           // = {3,4,5} 3address ,4 address  5address cycle
    u8_t pblr_start_block;      //which block is ploader start store. value 1    (block 1)
    u8_t page_per_chunk;        // indicates how many pages every chunck has,   1 for normal, 4 for 512-byte page used
    
    u8_t TRHZ;
    u8_t TDH;
} SOC_ALIGN nand_flash_info_t;

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
} SOC_ALIGN nor_spi_flash_info_t;

#define NSPI_IO_SIO     0
#define NSPI_IO_DIO     1
#define NSPI_IO_QIO     2

#if (OTTO_NAND_FLASH == 1)
    typedef nand_flash_info_t    flash_info_t;
    #define SECTION_ON_FLASH
    #define CONST_ON_FLASH
    #define RTK_MTD_DEV_NAME "rtk_nand_mtd"
#elif (OTTO_NOR_SPI_FLASH == 1)
    typedef nor_spi_flash_info_t flash_info_t;
    #define RTK_MTD_DEV_NAME "rtk_spi_nor_mtd"
    #define SECTION_ON_FLASH __attribute__ ((section (".flash_text")))
    #define CONST_ON_FLASH __attribute__ ((section (".flash_data")))
#else
    typedef struct {} flash_info_t;
    #define SECTION_ON_FLASH 
    #define CONST_ON_FLASH
#endif

typedef struct {
    u32_t bootloader1_addr;     // the address storing the bootloader 1
    u32_t bootloader2_addr;     // the address storing the bootloader 2, if any 
    u32_t kernel1_addr;         // the address storing the kernel 1
    u32_t kernel2_addr;         // the address storing the kernel 1, if any 
    u32_t rootfs1_addr;         // the address storing the rootfs 2 
    u32_t rootfs2_addr;         // the address storing the rootfs 2, if any 
    u32_t env_addr;             // the address storing the environment variables used by bootloader
    u32_t env_size;             // the total size of the area storing environment variables 
    u32_t opt1_addr;            // the address storing the optional information 1, if any 
    u32_t opt2_addr;            // the address storing the optional information 2, if any 
    u32_t opt3_addr;            // the address storing the optional information 3, if any 
    u32_t opt4_addr;            // the address storing the optional information 4, if any 
    u32_t end_addr;
} SOC_ALIGN flash_layout_t;

typedef struct {
    u32_t dummy;
} SOC_ALIGN dram_gen1_info_t;

#define STATIC_CAL_DATA_NUM 33
typedef struct {
    // the following fields are composed by the composer, soc.tcl doesn't fill in directly
    u32_t mcr;    //IPREF-23,DPREF-22
    u32_t dcr;    //BANKCNT-29:28,DBUSWID-25:24,ROWCNT-23:20,COLCNT-19:16,DCHIPSEL-15,FAST_RX-14,BSTREF-13
    u32_t dtr0;   //T_CAS-31:28,T_WR-27:24,T_CWL-23:20,T_RTP-19:16,T_WTR-15:12,T_REFI-11:8,T_REFI_UNIT-7:4
    u32_t dtr1;   //T_RP-28:24,T_RCD-20:16,T_RRD-12:8,T_FAWG-4:0
    u32_t dtr2;   //T_RFC-28:20,T_RAS-18:12
    u32_t mpmr0;  //PM_MODE-29:28,T_CKE-27:24,T_RSD-21:12,T_XSREF-9:0
    u32_t mpmr1;  //T_XARD-32:28,T_AXPD-27:24
    u32_t dider;  //DQS0_EN_HCLK-31,DQS0_EN_TAP-28:24,DQS1_EN_HCLK-23,DQS1_EN_TAP-20:16
    u32_t d23oscr;//ODT_ALWAYS_ON-31,TE_ALWAYS_ON-30
    u32_t daccr;  //AC_MODE-31,DQS_SE-30,DQS0_GROUP_TAP-20:16,DQS1_GROUP_TAP-12:8,AC_DYN_BPTR_CLR_EN-5,AC_BPTR_CLEAR-4,AC_DEBUG_SEL-3:0
    u32_t dacspcr;/* AC_SILEN_PERIOD_EN-31, AC_SILEN_TRIG-20, AC_SILEN_PERIOD_UNIT-19:16, AC_SILEN_PERIOD-15:8, AC_SILEN_LEN-7:0 */  
    u32_t dacspar;/* AC_SPS_DQ15R-31, AC_SPS_DQ14R-30, AC_SPS_DQ13R-29, AC_SPS_DQ12R-28, AC_SPS_DQ11R-27, AC_SPS_DQ10R-26, AC_SPS_DQ9R-25, AC_SPS_DQ8R-24, AC_SPS_DQ7R-23, AC_SPS_DQ6R-22, AC_SPS_DQ5R-21, AC_SPS_DQ4R-20, AC_SPS_DQ3R-19, AC_SPS_DQ2R-18, AC_SPS_DQ1R-17, AC_SPS_DQ0R-16, AC_SPS_DQ15F-15, AC_SPS_DQ14F-14, AC_SPS_DQ13F-13, AC_SPS_DQ12F-12, AC_SPS_DQ11F-11, AC_SPS_DQ10F-10, AC_SPS_DQ9F-9, AC_SPS_DQ8F-8, AC_SPS_DQ7F-7, AC_SPS_DQ6F-6, AC_SPS_DQ5F-5, AC_SPS_DQ4F-4, AC_SPS_DQ3F-3, AC_SPS_DQ2F-2, AC_SPS_DQ1F-1, AC_SPS_DQ0F-0 */ 

    // the following fields are composed by the composer, soc.tcl doesn't fill in directly
    /* Mode registers, follow JEDEC naming*/
    u32_t DDR1_mr;
    u32_t DDR1_emr;
    u32_t DDR2_mr;
    u32_t DDR2_emr1;
    u32_t DDR2_emr2;
    u32_t DDR2_emr3;
    u32_t DDR3_mr0;
    u32_t DDR3_mr1;
    u32_t DDR3_mr2;
    u32_t DDR3_mr3;
    
    // the following fields are inputed by soc.tcl
    u32_t static_cal_data_0; //0xb8001510
    u32_t static_cal_data_1; //0xb8001514
    u32_t static_cal_data_2; //0xb8001518
    u32_t static_cal_data_3; //0xb800151c
    u32_t static_cal_data_4; //0xb8001520
    u32_t static_cal_data_5; //0xb8001524
    u32_t static_cal_data_6; //0xb8001528
    u32_t static_cal_data_7; //0xb800152c
    u32_t static_cal_data_8; //0xb8001530
    u32_t static_cal_data_9; //0xb8001534
    u32_t static_cal_data_10;//0xb8001538
    u32_t static_cal_data_11;//0xb800153c
    u32_t static_cal_data_12;//0xb8001540
    u32_t static_cal_data_13;//0xb8001544
    u32_t static_cal_data_14;//0xb8001548
    u32_t static_cal_data_15;//0xb800154c
    u32_t static_cal_data_16;//0xb8001550
    u32_t static_cal_data_17;//0xb8001554
    u32_t static_cal_data_18;//0xb8001558
    u32_t static_cal_data_19;//0xb800155c
    u32_t static_cal_data_20;//0xb8001560
    u32_t static_cal_data_21;//0xb8001564
    u32_t static_cal_data_22;//0xb8001568
    u32_t static_cal_data_23;//0xb800156c
    u32_t static_cal_data_24;//0xb8001570
    u32_t static_cal_data_25;//0xb8001574
    u32_t static_cal_data_26;//0xb8001578
    u32_t static_cal_data_27;//0xb800157c
    u32_t static_cal_data_28;//0xb8001580
    u32_t static_cal_data_29;//0xb8001584
    u32_t static_cal_data_30;//0xb8001588
    u32_t static_cal_data_31;//0xb800158c
    u32_t static_cal_data_32;//0xb8001590
    
    u32_t zq_setting;    
    u8_t calibration_type;  //0-static, 1-software(static_cal_data is not used)
    u8_t tx_clk_phs_delay;
    u8_t clkm_delay;
    u8_t clkm90_delay;
    u8_t auto_calibration;  //0-disable, 1-enable. Calibration window auto-sliding. 
    u8_t drv_strength;      //0-normal, 1-reduced
} SOC_ALIGN dram_gen2_info_t;

//MCR register field offset
#define DG2_REG_OFFSET_IPREF 23
#define DG2_REG_OFFSET_DPREF 22
#define DG2_REG_MASK_IPREF (0x1 << DG2_REG_OFFSET_IPREF)
#define DG2_REG_MASK_DPREF (0x1 << DG2_REG_OFFSET_DPREF)
//DCR register field offset
#define DG2_REG_OFFSET_BANKCNT 28
#define DG2_REG_OFFSET_DBUSWID 24
#define DG2_REG_OFFSET_ROWCNT 20
#define DG2_REG_OFFSET_COLCNT 16
#define DG2_REG_OFFSET_DCHIPSEL 15
#define DG2_REG_OFFSET_FAST_RX 14
#define DG2_REG_OFFSET_BSTREF 13
#define DG2_REG_MASK_BANKCNT  (0x3 << DG2_REG_OFFSET_BANKCNT )
#define DG2_REG_MASK_DBUSWID  (0x3 << DG2_REG_OFFSET_DBUSWID )
#define DG2_REG_MASK_ROWCNT   (0xF << DG2_REG_OFFSET_ROWCNT  )
#define DG2_REG_MASK_COLCNT   (0xF << DG2_REG_OFFSET_COLCNT  )
#define DG2_REG_MASK_DCHIPSEL (0x1 << DG2_REG_OFFSET_DCHIPSEL)
#define DG2_REG_MASK_FAST_RX  (0x1 << DG2_REG_OFFSET_FAST_RX )
#define DG2_REG_MASK_BSTREF   (0x1 << DG2_REG_OFFSET_BSTREF  )
//DTR0 register field offset
#define DG2_REG_OFFSET_T_CAS 28
#define DG2_REG_OFFSET_T_WR 24
#define DG2_REG_OFFSET_T_CWL 20
#define DG2_REG_OFFSET_T_RTP 16
#define DG2_REG_OFFSET_T_WTR 12
#define DG2_REG_OFFSET_T_REFI 8
#define DG2_REG_OFFSET_T_REFI_UNIT 4
#define DG2_REG_MASK_T_CAS       (0xF << DG2_REG_OFFSET_T_CAS      )
#define DG2_REG_MASK_T_WR        (0xF << DG2_REG_OFFSET_T_WR       )
#define DG2_REG_MASK_T_CWL       (0xF << DG2_REG_OFFSET_T_CWL      )
#define DG2_REG_MASK_T_RTP       (0xF << DG2_REG_OFFSET_T_RTP      )
#define DG2_REG_MASK_T_WTR       (0xF << DG2_REG_OFFSET_T_WTR      )
#define DG2_REG_MASK_T_REFI      (0xF << DG2_REG_OFFSET_T_REFI     )
#define DG2_REG_MASK_T_REFI_UNIT (0xF << DG2_REG_OFFSET_T_REFI_UNIT)
//DTR1 register field offset
#define DG2_REG_OFFSET_T_RP 24
#define DG2_REG_OFFSET_T_RCD 16
#define DG2_REG_OFFSET_T_RRD 8
#define DG2_REG_OFFSET_T_FAWG 0
#define DG2_REG_MASK_T_RP   (0x1F << DG2_REG_OFFSET_T_RP  )
#define DG2_REG_MASK_T_RCD  (0x1F << DG2_REG_OFFSET_T_RCD )
#define DG2_REG_MASK_T_RRD  (0x1F << DG2_REG_OFFSET_T_RRD )
#define DG2_REG_MASK_T_FAWG (0x1F << DG2_REG_OFFSET_T_FAWG)
//DRT2 register field offset
#define DG2_REG_OFFSET_T_RFC 20
#define DG2_REG_OFFSET_T_RAS 12
#define DG2_REG_MASK_T_RFC (0xFF << DG2_REG_OFFSET_T_RFC)
#define DG2_REG_MASK_T_RAS (0x3F << DG2_REG_OFFSET_T_RAS)
//MPRM0 register field offset
#define DG2_REG_OFFSET_PM_MODE 28
#define DG2_REG_OFFSET_T_CKE 24
#define DG2_REG_OFFSET_T_RSD 12
#define DG2_REG_OFFSET_T_XSREF 0
#define DG2_REG_MASK_PM_MODE (0x3   << DG2_REG_OFFSET_PM_MODE)
#define DG2_REG_MASK_T_CKE   (0xF   << DG2_REG_OFFSET_T_CKE  )
#define DG2_REG_MASK_T_RSD   (0x3FF << DG2_REG_OFFSET_T_RSD  )
#define DG2_REG_MASK_T_XSREF (0x3FF << DG2_REG_OFFSET_T_XSREF)
//MPRM1
#define DG2_REG_OFFSET_T_XARD 28
#define DG2_REG_OFFSET_T_AXPD 24
#define DG2_REG_MASK_T_XARD (0xF << DG2_REG_OFFSET_T_XARD)
#define DG2_REG_MASK_T_AXPD (0xF << DG2_REG_OFFSET_T_AXPD)
//DIDER
#define DG2_REG_OFFSET_DQS0_EN_HCLK 31
#define DG2_REG_OFFSET_DQS0_EN_TAP 24
#define DG2_REG_OFFSET_DQS1_EN_HCLK 23
#define DG2_REG_OFFSET_DQS1_EN_TAP 16
#define DG2_REG_MASK_DQS0_EN_HCLK (0x1  << DG2_REG_OFFSET_DQS0_EN_HCLK)
#define DG2_REG_MASK_DQS0_EN_TAP  (0x1F << DG2_REG_OFFSET_DQS0_EN_TAP )
#define DG2_REG_MASK_DQS1_EN_HCLK (0x1  << DG2_REG_OFFSET_DQS1_EN_HCLK)
#define DG2_REG_MASK_DQS1_EN_TAP  (0x1F << DG2_REG_OFFSET_DQS1_EN_TAP )
//D23OSCR
#define DG2_REG_OFFSET_ODT_ALWAYS_ON 31
#define DG2_REG_OFFSET_TE_ALWAYS_ON 24
#define DG2_REG_MASK_ODT_ALWAYS_ON (0x1 << DG2_REG_OFFSET_ODT_ALWAYS_ON)
#define DG2_REG_MASK_TE_ALWAYS_ON  (0x1 << DG2_REG_OFFSET_TE_ALWAYS_ON )
//DACCR
#define DG2_REG_OFFSET_AC_MODE 31
#define DG2_REG_OFFSET_DQS_SE 30
#define DG2_REG_OFFSET_DQS0_GROUP_TAP 16
#define DG2_REG_OFFSET_DQS1_GROUP_TAP 8
#define DG2_REG_OFFSET_AC_DYN_BPTR_CLR_EN 5
#define DG2_REG_OFFSET_AC_BPTR_CLEAR 4
#define DG2_REG_OFFSET_AC_DEBUG_SEL 0
#define DG2_REG_MASK_AC_MODE            (0x1  << DG2_REG_OFFSET_AC_MODE            )
#define DG2_REG_MASK_DQS_SE             (0x1  << DG2_REG_OFFSET_DQS_SE             )
#define DG2_REG_MASK_DQS0_GROUP_TAP     (0x1F << DG2_REG_OFFSET_DQS0_GROUP_TAP     )
#define DG2_REG_MASK_DQS1_GROUP_TAP     (0x1F << DG2_REG_OFFSET_DQS1_GROUP_TAP     )
#define DG2_REG_MASK_AC_DYN_BPTR_CLR_EN (0x1  << DG2_REG_OFFSET_AC_DYN_BPTR_CLR_EN )
#define DG2_REG_MASK_AC_BPTR_CLEAR      (0x1  << DG2_REG_OFFSET_AC_BPTR_CLEAR      )
#define DG2_REG_MASK_AC_DEBUG_SEL       (0xF  << DG2_REG_OFFSET_AC_DEBUG_SEL       )
//DACSPCR
#define DG2_REG_OFFSET_AC_SILEN_PERIOD_EN 31
#define DG2_REG_OFFSET_AC_SILEN_TRIG 20
#define DG2_REG_OFFSET_AC_SILEN_PERIOD_UNIT 16
#define DG2_REG_OFFSET_AC_SILEN_PERIOD 8
#define DG2_REG_OFFSET_AC_SILEN_LEN 0
#define DG2_REG_MASK_AC_SILEN_PERIOD_EN   (0x1  << DG2_REG_OFFSET_AC_SILEN_PERIOD_EN  )
#define DG2_REG_MASK_AC_SILEN_TRIG        (0x1  << DG2_REG_OFFSET_AC_SILEN_TRIG       )
#define DG2_REG_MASK_AC_SILEN_PERIOD_UNIT (0xF  << DG2_REG_OFFSET_AC_SILEN_PERIOD_UNIT)
#define DG2_REG_MASK_AC_SILEN_PERIOD      (0xFF << DG2_REG_OFFSET_AC_SILEN_PERIOD     )
#define DG2_REG_MASK_AC_SILEN_LEN         (0xFF << DG2_REG_OFFSET_AC_SILEN_LEN        )
//DACSPSR
#define DG2_REG_OFFSET_AC_SPS_DQ15R 31
#define DG2_REG_OFFSET_AC_SPS_DQ14R 30
#define DG2_REG_OFFSET_AC_SPS_DQ13R 29
#define DG2_REG_OFFSET_AC_SPS_DQ12R 28
#define DG2_REG_OFFSET_AC_SPS_DQ11R 27
#define DG2_REG_OFFSET_AC_SPS_DQ10R 26
#define DG2_REG_OFFSET_AC_SPS_DQ9R  25
#define DG2_REG_OFFSET_AC_SPS_DQ8R  24
#define DG2_REG_OFFSET_AC_SPS_DQ7R  23
#define DG2_REG_OFFSET_AC_SPS_DQ6R  22
#define DG2_REG_OFFSET_AC_SPS_DQ5R  21
#define DG2_REG_OFFSET_AC_SPS_DQ4R  20
#define DG2_REG_OFFSET_AC_SPS_DQ3R  19
#define DG2_REG_OFFSET_AC_SPS_DQ2R  18
#define DG2_REG_OFFSET_AC_SPS_DQ1R  17
#define DG2_REG_OFFSET_AC_SPS_DQ0R  16
#define DG2_REG_OFFSET_AC_SPS_DQ15F 15
#define DG2_REG_OFFSET_AC_SPS_DQ14F 14
#define DG2_REG_OFFSET_AC_SPS_DQ13F 13
#define DG2_REG_OFFSET_AC_SPS_DQ12F 12
#define DG2_REG_OFFSET_AC_SPS_DQ11F 11
#define DG2_REG_OFFSET_AC_SPS_DQ10F 10
#define DG2_REG_OFFSET_AC_SPS_DQ9F  9
#define DG2_REG_OFFSET_AC_SPS_DQ8F  8
#define DG2_REG_OFFSET_AC_SPS_DQ7F  7
#define DG2_REG_OFFSET_AC_SPS_DQ6F  6
#define DG2_REG_OFFSET_AC_SPS_DQ5F  5
#define DG2_REG_OFFSET_AC_SPS_DQ4F  4
#define DG2_REG_OFFSET_AC_SPS_DQ3F  3
#define DG2_REG_OFFSET_AC_SPS_DQ2F  2
#define DG2_REG_OFFSET_AC_SPS_DQ1F  1
#define DG2_REG_OFFSET_AC_SPS_DQ0F  0
#define DG2_REG_MASK_AC_SPS_DQ15R (0x1 << DG2_REG_OFFSET_AC_SPS_DQ15R)
#define DG2_REG_MASK_AC_SPS_DQ14R (0x1 << DG2_REG_OFFSET_AC_SPS_DQ14R)
#define DG2_REG_MASK_AC_SPS_DQ13R (0x1 << DG2_REG_OFFSET_AC_SPS_DQ13R)
#define DG2_REG_MASK_AC_SPS_DQ12R (0x1 << DG2_REG_OFFSET_AC_SPS_DQ12R)
#define DG2_REG_MASK_AC_SPS_DQ11R (0x1 << DG2_REG_OFFSET_AC_SPS_DQ11R)
#define DG2_REG_MASK_AC_SPS_DQ10R (0x1 << DG2_REG_OFFSET_AC_SPS_DQ10R)
#define DG2_REG_MASK_AC_SPS_DQ9R  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ9R )
#define DG2_REG_MASK_AC_SPS_DQ8R  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ8R )
#define DG2_REG_MASK_AC_SPS_DQ7R  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ7R )
#define DG2_REG_MASK_AC_SPS_DQ6R  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ6R )
#define DG2_REG_MASK_AC_SPS_DQ5R  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ5R )
#define DG2_REG_MASK_AC_SPS_DQ4R  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ4R )
#define DG2_REG_MASK_AC_SPS_DQ3R  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ3R )
#define DG2_REG_MASK_AC_SPS_DQ2R  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ2R )
#define DG2_REG_MASK_AC_SPS_DQ1R  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ1R )
#define DG2_REG_MASK_AC_SPS_DQ0R  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ0R )
#define DG2_REG_MASK_AC_SPS_DQ15F (0x1 << DG2_REG_OFFSET_AC_SPS_DQ15F)
#define DG2_REG_MASK_AC_SPS_DQ14F (0x1 << DG2_REG_OFFSET_AC_SPS_DQ14F)
#define DG2_REG_MASK_AC_SPS_DQ13F (0x1 << DG2_REG_OFFSET_AC_SPS_DQ13F)
#define DG2_REG_MASK_AC_SPS_DQ12F (0x1 << DG2_REG_OFFSET_AC_SPS_DQ12F)
#define DG2_REG_MASK_AC_SPS_DQ11F (0x1 << DG2_REG_OFFSET_AC_SPS_DQ11F)
#define DG2_REG_MASK_AC_SPS_DQ10F (0x1 << DG2_REG_OFFSET_AC_SPS_DQ10F)
#define DG2_REG_MASK_AC_SPS_DQ9F  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ9F )
#define DG2_REG_MASK_AC_SPS_DQ8F  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ8F )
#define DG2_REG_MASK_AC_SPS_DQ7F  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ7F )
#define DG2_REG_MASK_AC_SPS_DQ6F  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ6F )
#define DG2_REG_MASK_AC_SPS_DQ5F  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ5F )
#define DG2_REG_MASK_AC_SPS_DQ4F  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ4F )
#define DG2_REG_MASK_AC_SPS_DQ3F  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ3F )
#define DG2_REG_MASK_AC_SPS_DQ2F  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ2F )
#define DG2_REG_MASK_AC_SPS_DQ1F  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ1F )
#define DG2_REG_MASK_AC_SPS_DQ0F  (0x1 << DG2_REG_OFFSET_AC_SPS_DQ0F )

#if (OTTO_DRAM_GEN == 1)
    typedef dram_gen1_info_t    dram_info_t;
#elif (OTTO_DRAM_GEN == 2)
    typedef dram_gen2_info_t    dram_info_t;
#else
    typedef struct {}           dram_info_t;
#endif

typedef struct {
    u32_t set_by;   // 1-software or 0-pin
    u32_t sysclk_control_reg_25mhz;//CPU/DSP/MEM
    u32_t lx_pll_sel_reg_25mhz;
    u32_t sysclk_control_reg_40mhz;//CPU/DSP/MEM
    u32_t lx_pll_sel_reg_40mhz;
    u32_t mckg_phs_sel_reg;
    u32_t mckg_freq_div_reg; 
} pll_gen1_info_t;

typedef struct {
    u32_t set_by;   // 1-software or 0-pin
    u32_t sys_cpu_pll_ctl0;
    u32_t sys_cpu_pll_ctl1;
    u32_t sys_mem_pll_ctl0;
    u32_t sys_mem_pll_ctl1;
    u32_t sys_lx_pll_ctl0;
    u32_t sys_lx_pll_ctl1;
} pll_gen2_info_t;

typedef struct {
    u32_t set_by;   // 1-software or 0-pin
    u32_t pll_cpu_ctl0;
    u32_t pll_cpu_ctl1;
    u32_t pll_cpu_misc_ctrl;
    u32_t pll_mem_ctl0;
    u32_t pll_mem_ctl1;
    u32_t pll_mem_misc_ctrl;
} pll_gen3_info_t;

#if (OTTO_PLL_GEN == 1)
    typedef pll_gen1_info_t     pll_info_t;
#elif (OTTO_PLL_GEN == 2)
    typedef pll_gen2_info_t     pll_info_t;
#elif (OTTO_PLL_GEN == 3)
    typedef pll_gen3_info_t     pll_info_t;
#else
    typedef struct {}           pll_info_t;
#endif


typedef struct {
    /* All these values are for matching registers. */
    u16_t baudrate_divisor;      /* for UART0_DLL. */
} SOC_ALIGN peripheral_info_t;

typedef u8_t mac_addr_t[6];

typedef struct {
    mac_addr_t address;
} SOC_ALIGN mac_info_t;

#if defined(__x86_64) || defined(__i386)
    typedef u32_t spare_header_p;
    typedef u32_t void_p;
#else
    struct spare_header_s;
    typedef struct spare_header_s * spare_header_p;
    typedef void* void_p;
#endif

// for spare structure 
typedef struct spare_header_s {
    u32_t type;
    u32_t num_instances;
    void_p spare;
} spare_header_t;
// spare structure types
#define SST_DRAM                    0x4452414D
#define SST_FLASH                   0x464C5348
#define SST_END                     0x454E4420

typedef struct {
    u32_t               header_type;
    u32_t               header_ver;
    spare_header_p      spare_headers;              // used to identify if any spare spaces exists or where is the headers of space
    flash_info_t        flash_info;
    flash_layout_t      layout;
    dram_info_t         dram_info;
    pll_info_t          pll_info;
    peripheral_info_t   peri_info;
    mac_info_t          mac_info;
} SOC_ALIGN soc_t;
// ---------------------------
//        for header_type
// ---------------------------
// for flash type and interface
#define FIHT_SPI                    0x00000001
#define FIHT_PARALLEL               0x00000002
#define FIHT_NAND                   0x00000010
#define FIHT_NOR                    0x00000020

// for DRAM controller generation
#define FIHT_DRAMCTRL_MASK          0x00000f00
#define FIHT_DRAMCTRL_SHIFT         8
#define FIHT_DRAMCTRL_UKNOWN        0x00000000
#define FIHT_DRAMCTRL_GEN1          0x00000100  // the version of SDR/DDR1/DDR2 200Mhz
#define FIHT_DRAMCTRL_GEN2          0x00000200  // the version of DDR1/DDR2 400Mhz/DDR3 400Mhz
#define FIHT_DRAMCTRL_GEN3          0x00000300
#define FIHT_DRAMCTRL_GEN4          0x00000400
#define FIHT_GET_DRAN_GEN(h)        (((h)&FIHT_DRAMCTRL_MASK)>>FIHT_DRAMCTRL_SHIFT)

// for PLL generation
#define FIHT_PLL_MASK               0x0003f000
#define FIHT_PLL_SHIFT              12
#define FIHT_PLL_UNKNOWN            0x00000000
#define FIHT_PLL_GEN1               0x00001000
#define FIHT_PLL_GEN2               0x00002000
#define FIHT_PLL_GEN3               0x00003000
#define FIHT_PLL_GEN4               0x00004000
#define FIHT_PLL_GEN5               0x00005000
#define FIHT_PLL_GEN6               0x00006000
#define FIHT_PLL_GEN7               0x00007000
#define FIHT_GET_PLL_GEN(h)         (((h)&FIHT_PLL_MASK)>>FIHT_PLL_SHIFT)

// for FTx
#define FIHT_FT_MASK                0xC0000000
#define FIHT_FT1                    0x40000000
#define FIHT_FT2                    0x80000000
#define FIHT_NORMAL                 0x00000000

#define FIHT_IS_FT1(ht)             (((ht)&FIHT_FT_MASK)==FIHT_FT1)
#define FIHT_IS_FT2(ht)             (((ht)&FIHT_FT_MASK)==FIHT_FT2)
#define FIHT_IS_NORMAL(ht)          (((ht)&FIHT_FT_MASK)==FIHT_NORMAL)
#define FIHT_DRAMCTRL_VER(ht)       ((ht)&FIHT_DRAMCTRL_MASK)
#define FIHT_DRAMCTRL_IS_GEN1(ht)   (FIHT_DRAMCTRL_VER(ht)==FIHT_DRAMCTRL_GEN1)
#define FIHT_DRAMCTRL_IS_GEN2(ht)   (FIHT_DRAMCTRL_VER(ht)==FIHT_DRAMCTRL_GEN2)
#define FIHT_DRAMCTRL_IS_GEN3(ht)   (FIHT_DRAMCTRL_VER(ht)==FIHT_DRAMCTRL_GEN3)
#define FIHT_DRAMCTRL_IS_GEN4(ht)   (FIHT_DRAMCTRL_VER(ht)==FIHT_DRAMCTRL_GEN4)


// which is recorded in the block 0, page 0, 2nd cache-line (offset = 0x20)
typedef struct {
    u32_t signature1;
    const soc_t soc;
    u32_t signature2;
} soc_configuration_t;
#define FC_SIG1 0x03710601
#define FC_SIG2 0x62668696

// oob for each pre-loader and boot-loader
typedef struct {
    u32_t signature;
    u16_t age;                  // this value count from 0xfffe (for first valid block)
    u16_t ver;                  // 0 was reserved
    u16_t chunk_id;
    u16_t num_chunk;
}oob_t ;

// oob signature
#define OOB_SIG_USED_PLR    0x505f4c52  // it stored data for pre-loader
#define OOB_SIG_USED_BLR    0x5f424c52  // it stored data for boot-loader
#define OOB_SIG_RESERVED    0x10400401
#define OOB_SIG_ERASED      0xffffffff
#define OOB_ERASED_AGE      0xffff

typedef union {
    u8_t u8_oob[MAX_SPARESPACE];
    oob_t oob;
} spare_u;

typedef struct {
    spare_u spare;
    u8_t chunk_buf[0];
} nand_dma_buf_t;

// initialization result
typedef enum {
    INI_RES_OK,
    INI_RES_UNKNOWN_MODE,
    INI_RES_UNKNOWN_MODEL,
    INI_RES_FAIL,
    INI_RES_TEST_FAIL,
    INI_RES_UNINIT,
    INI_RES_DRAM_ZQ_CALI_FAIL,
    INI_RES_DRAM_SW_CALI_FAIL
} init_result_t;

typedef enum {
	PLL_RES_OK,
	PLL_RES_BAD_MODE,
	PLL_RES_BAD_OSC_FREQ,
	PLL_RES_FREQ_OUT_OF_RANGE,
} pll_result_t;

// define nand flash controller access unit
#define RS_V2_PAGE_SIZE     512
#define RS_V2_OOB_SIZE      6
#define RS_V2_ECC_SIZE      10
#define RS_V2_UNIT_SIZE     (RS_V2_PAGE_SIZE+RS_V2_OOB_SIZE+RS_V2_ECC_SIZE)


typedef int  (nand_read_write_page_t)(u8_t *page_buf, spare_u *spare, u32_t page_id);
typedef u8_t (nand_read_bbi_t)(u32_t bytes, u32_t chunk_id );
typedef int  (nand_erase_block_t)(u32_t chunk_id);
typedef int (nand_check_eccStatus_t) (void);
typedef int (nand_pio_write_t) (u32_t chunk_id,u32_t length, u8_t *buffer) ;

typedef u32_t  (pblr_printf_t)(const char *, ...);
typedef void  (fpv_u32_u32_t) (u32_t, u32_t);
typedef void  (fpv_u32_t)(u32_t);
typedef u32_t (fpu32_u32_t)(u32_t);
typedef void  (fpv_s32_t)(s32_t);
typedef void  (fpv_s8_t)(s8_t);
typedef s8_t  (fps8_v)(void);
typedef s32_t (fps32_v)(void);

typedef int  (nor_spi_read) (u32_t cid, u32_t offset, u32_t len, void *buf);
typedef int  (nor_spi_write)(u32_t cid, u32_t offset, u32_t len, const void *buf);
typedef int  (nor_spi_erase)(u32_t cid, u32_t offset);

typedef void* (memcpy_t)(u8_t *, const u8_t *, u32_t);
typedef void  (void_func)(void);

// in the SRAM, which is stored by preloader and used by bootloader and kernel
typedef struct  {
    const soc_t soc;

    // global variables when scan
    u32_t soc_id;
    u32_t bond_id;
    #if (OTTO_NAND_FLASH == 1)
        // NAND flash only 
        u32_t curr_ver;             // 0 was not valid
        u32_t plr_num_chunk;        //chunk number usage by preloader
        u32_t blr_num_chunk;        //chunk number usage by bootloader
        u32_t end_pblr_block;       //preloader end chunk number 
        u32_t chunk_size;
        u32_t num_chunk_per_block;
    
        // bad block tables
        u32_t bbt_valid:1;          // when BBT is built
        u32_t bbt[NUM_WORD(MAX_BLOCKS)];
    
        // function points, which is going to share with bootloader
        nand_read_write_page_t  *_nand_read_chunk;
        nand_read_bbi_t         *_nand_read_bbi;
        nand_read_write_page_t  *_nand_write_chunk;
        nand_erase_block_t      *_nand_erase_block;
    nand_check_eccStatus_t      *_nand_check_eccStatus;
        nand_pio_write_t        *_nand_pio_write;
    #elif (OTTO_NOR_SPI_FLASH == 1)
        nor_spi_erase *_nor_spi_erase;
        nor_spi_read  *_nor_spi_read;
        nor_spi_write *_nor_spi_write;
    #endif

    // fields to record the result of initialization 
    init_result_t   dram_init_result;
    init_result_t   flash_init_result;

    // function point for u-boot
    pblr_printf_t *_pblr_printf;
    fpv_u32_t     *_udelay;

    fpv_s32_t     *_uart_init;
    fpv_s8_t      *_uart_putc;
    fps8_v        *_uart_getc;
    fps32_v       *_uart_tstc;

    void_func     *_dcache_writeback_invalidate_all;
    void_func     *_icache_invalidate_all;


    fpu32_u32_t   *_pll_query_freq;
} parameter_to_bootloader_t;

// it should be at 0x9FC0_0000+0x8000-320)
// which means last area of 32KB-SRAM with cache aligned
#define sram_parameters (*(parameter_to_bootloader_t*)(SRAM_BASE + SRAM_SIZE - CACHE_ALIGN(sizeof(parameter_to_bootloader_t))))

/* PLL devices for querying frequency */
#define PLL_DEV_CPU 0
#define PLL_DEV_LX  1
#define PLL_DEV_MEM 2
#define PLL_DEV_DSP 3
#define PLL_MHZ_UNKNOWN 0xFFFFFFFF

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
#endif /* #ifndef __ASSEMBLER__ */
#endif
