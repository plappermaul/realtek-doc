#ifndef _MEMCNTLR_UTIL_
#define _MEMCNTLR_UTIL_

#ifndef MEMCNTLR_SECTION
#define MEMCNTLR_SECTION
#endif

#ifndef MEMCNTLR_DATA_SECTION
#define MEMCNTLR_DATA_SECTION
#endif

// translate ocd and odt
#define ZP_D3           (0)     // DDR3, CK/CMD/DQ/DQS
#define ZP_D3_PD3       (1)     // DDR3, PUPD3
#define ZP_D4_P5_CMD    (4)     // DDR4, 0.5V CK/CMD
#define ZP_D4_P8_DQ     (5)     // DDR4, 0.8V DQ/DQS
#define ZP_D4_P8_PD3    (6)     // DDR4, 0.8V PUPD3
#define ZP_LD3          (8)     // LPDDR3
#define ZP_X_OCD_ODT(ocd, odt, t)   (((ocd)<<24)|((odt)<<16)|(t))
#define ZP_GET_OCD(v)               (((v)>>24)&0xFF)
#define ZP_GET_ODT(v)               (((v)>>16)&0xFF)
#define ZP_GET_TYPE(v)              ((v)&0xFF)

#define DBG_PRINTF(...)         printf("DD: "__VA_ARGS__)
#define ERR_PRINTF(...)         printf("EE: "__VA_ARGS__)
#define WARN_PRINTF(...)        printf("WW: "__VA_ARGS__)
#define INFO_PRINTF(...)        printf("II: "__VA_ARGS__)
#define INDT_PRINTF(...)        printf("    "__VA_ARGS__)
#define FERR_PRINTF(...)        printf("fail!\nEE: "__VA_ARGS__)

#define _dcache_flush           _soc.bios.dcache_writeback_invalidate_all
#define _dcache_flush_range     _soc.bios.dcache_writeback_invalidate_range
#define _icache_flush           _soc.bios.icache_invalidate_all
#define _icache_flush_range     _soc.bios.icache_invalidate_range

u32_t cg_query_freq(u32_t);
#define GET_MEM_MHZ() cg_query_freq(1)

#define DDR_TYPE_SPIN       (0)
// from RXI-310 definition
#define DDR_TYPE_DDR3       (3)
#define DDR_TYPE_DDR4       (4)
#define DDR_TYPE_LPDDR3     (11)

#define DDR_ZQ_TYPE_CMD     (0)
#define DDR_ZQ_TYPE_DQS_DQ  (1)

enum {
    zq_dq_odt_sel=0,
    zq_dq_ocd_sel,
    zq_dqs_p_odt_sel,
    zq_dqs_n_odt_sel,
    zq_dqs_ocd_sel,
    zq_cke_ocd_sel,
    zq_adr_ocd_sel,
    zq_ck_ocd_sel,
    zq_end_sel
};

MEMCNTLR_SECTION void mc_info_probe(void);
MEMCNTLR_SECTION void mc_info_translation(void);
MEMCNTLR_SECTION void mc_info_to_reg(void);
MEMCNTLR_SECTION void mc_cntlr_zq_calibration(void);
MEMCNTLR_SECTION void mc_calibration(void);
MEMCNTLR_SECTION void mc_misc(void);
MEMCNTLR_SECTION void mc_result_decode(void);
MEMCNTLR_SECTION u32_t dpi_r480_calibration(void);
MEMCNTLR_SECTION u32_t dpi_zq_calibration(u32_t ocd_odt[]);
MEMCNTLR_SECTION void dpi_vref_dq_setting(void);
MEMCNTLR_SECTION void mc_rxi310_init(u8_t silence);
MEMCNTLR_SECTION u32_t get_ddr_type(void);
MEMCNTLR_SECTION u32_t get_dq32_status(void);

#include <init_result_helper.h>

INIT_RESULT_GROUP(probe,
                  MEM_PROBE_UNINIT,
                  MEM_PROBE_OK,
                  MEM_PROBE_FAIL,
                  MEM_PROBE_FUNC_MISSING);

INIT_RESULT_GROUP(xlat,                  
                  MEM_XLAT_UNINIT,
                  MEM_XLAT_OK,
                  MEM_DRAM_TYPE_ERROR,
                  MEM_BANK_NUM_ERROR,
                  MEM_BUS_WIDTH_ERROR,
                  MEM_ROW_NUM_ERROR,
                  MEM_PAGE_SIZE_ERROR,
                  MEM_BL_ERROR,
                  MEM_REF_NUM_ERROR,
                  MEM_RTTNOM_ERROR,
                  MEM_RTTWR_ERROR,
                  MEM_CL_ERROR,
                  MEM_CWL_ERROR	
                  );

INIT_RESULT_GROUP(cntlr_zq,
                  MEM_CNTLR_ZQ_R480_UNINIT,
                  MEM_CNTLR_R480_TIMEOUT,
                  MEM_CNTLR_ZQ_TIMEOUT,                  
                  MEM_CNTLR_R480_ERROR,
                  MEM_CNTLR_ZQ_ERROR,
                  MEM_CNTLR_ZQ_R480_OK);

INIT_RESULT_GROUP(to_reg,
                  MEM_TO_REG_UNINIT,
                  MEM_TO_REG_OK);

INIT_RESULT_GROUP(cal,
                  MEM_CAL_UNINIT,
                  MEM_CAL_OK,
                  MEM_CAL_FAIL);

/* BSTC */
#ifndef PACKED
#define PACKED          __attribute__ ((packed))
#endif

#define RXI310_BUS_WIDTH     (128)
#define RXI310_DQ_WIDTH      (16)	// FIXME, 32
// maximum bank, row, column support bit
#define RXI310_MAX_BANK_BIT  (4)     //4
#define RXI310_MAX_ROW_BIT   (17)    //17
#define RXI310_MAX_COL_BIT   (11)

#define BSTC_BASE            (0xBF800000)
#define BSTC_SRAM_CMD_BASE   (BSTC_BASE)
#define BSTC_SRAM_CMD_BUSW   (RXI310_MAX_BANK_BIT+RXI310_MAX_ROW_BIT+(RXI310_MAX_COL_BIT+1)+4+1)           // BusWidth=BANK+ROW+COL(DQ32 need +1)+BST_LEN+CMD
#define BSTC_SRAM_CMD_ENTY   (32)
#define BSTC_SRAM_CMD_SIZE   (SRAM_CMD_ENTY*SRAM_CMD_BUSW)

#define BSTC_SRAM_MSK_BASE   (BSTC_BASE+0x01000)
#define BSTC_SRAM_MSK_BUSW   (16)
#define BSTC_SRAM_MSK_ENTY   (32)
#define BSTC_SRAM_MSK_SIZE   (SRAM_MSK_ENTY*SRAM_MSK_BUSW)

#define BSTC_SRAM_WD_BASE    (BSTC_BASE+0x03800)
#define BSTC_SRAM_WD_BUSW    (128)
#define BSTC_SRAM_WD_ENTY    (128)
#define BSTC_SRAM_WD_SIZE    (SRAM_WD_BUSW*SRAM_WD_ENTY)

#define BSTC_SRAM_RG_BASE    (BSTC_BASE+0x0C000)
#define BSTC_SRAM_RG_BUSW    (128)
#define BSTC_SRAM_RG_ENTY    (128)
#define BSTC_SRAM_RG_SIZE    (SRAM_RG_BUSW*SRAM_RG_ENTY)

#define BSTC_SRAM_RD_BASE    (BSTC_BASE+0x14800)
#define BSTC_SRAM_RD_BUSW    (128)
#define BSTC_SRAM_RD_ENTY    (128)
#define BSTC_SRAM_RD_SIZE    (SRAM_RD_BUSW*SRAM_RD_ENTY)

#define BSTC_WR_CMD          (1)
#define BSTC_RD_CMD          (0)

// BSTC UTIL
#define BSTC_CMD_WRITE(addr, data)   REG32((addr))     = data.v[1];\
                                     REG32((addr+0x4)) = data.v[0];
                                
#define BSTC_VA2COL(info, addr) (((addr)&((1<<info.col_size)-1))>>4)
#define BSTC_VA2BNK(info, addr) (((addr)>>(info.col_size))&((1<<info.bnk_size)-1))
#define BSTC_VA2ROW(info, addr) (((addr)&0xFFFFFFF)>>(info.col_size+info.bnk_size))

#define BSTC_PA2COL(info, addr) (((addr)&((1<<info.col_size)-1))>>4)
#define BSTC_PA2BNK(info, addr) (((addr)>>(info.col_size))&((1<<info.bnk_size)-1))
#define BSTC_PA2ROW(info, addr) ((addr)>>(info.col_size+info.bnk_size))

#define get_col_size() ({ 8+RFLD_MISC(page_size); })
#define get_bnk_size() ({ 1+RFLD_MISC(bank_size); })

typedef union {
    struct {
        unsigned int dummy:(64-BSTC_SRAM_CMD_BUSW) PACKED;
        unsigned int bank:(RXI310_MAX_BANK_BIT) PACKED;
        unsigned int row:(RXI310_MAX_ROW_BIT) PACKED;
        unsigned int col:(RXI310_MAX_COL_BIT+1) PACKED;
        unsigned int bl:(4) PACKED;
        unsigned int cmd:(1) PACKED;
    }f;
    unsigned int v[2];
} BSTC_CMD_T;

typedef struct {
    unsigned int bnk_size;
    unsigned int col_size;
    unsigned int wr_ent_cnt;
    unsigned int rd_ent_cnt;
} BSTC_INFO_T;

//#define mc_disable_bstc()  RMOD_CSR(bstc_idle, 1, mem_idle, 0)
#define mc_disable_bstc()  ({ CSRrv=0x600; udelay(1000); })

typedef struct {
    unsigned int v;
    unsigned int z;
} ZQ_XLAT_T;

//#define PRINT_REG(reg)  printf("DD: (0x%08X)=0x%08X\n", reg##ar, reg##rv)
//#define PRINT_REG(reg)  

// assume: cache line size: 4 ways * 32Byte
#define UNROLLING_MCPY_128BYTE(src, dst)    ({  \
            *(dst)    = *(src);             \
            *(dst+8)  = *(src+8);           \
            *(dst+16) = *(src+16);          \
            *(dst+24) = *(src+24);          \
            *(dst+1)  = *(src+1);           \
            *(dst+2)  = *(src+2);           \
            *(dst+3)  = *(src+3);           \
            *(dst+4)  = *(src+4);           \
            *(dst+5)  = *(src+5);           \
            *(dst+6)  = *(src+6);           \
            *(dst+7)  = *(src+7);           \
            *(dst+9)  = *(src+9);           \
            *(dst+10) = *(src+10);          \
            *(dst+11) = *(src+11);          \
            *(dst+12) = *(src+12);          \
            *(dst+13) = *(src+13);          \
            *(dst+14) = *(src+14);          \
            *(dst+15) = *(src+15);          \
            *(dst+17) = *(src+17);          \
            *(dst+18) = *(src+18);          \
            *(dst+19) = *(src+19);          \
            *(dst+20) = *(src+20);          \
            *(dst+21) = *(src+21);          \
            *(dst+22) = *(src+22);          \
            *(dst+23) = *(src+23);          \
            *(dst+25) = *(src+25);          \
            *(dst+26) = *(src+26);          \
            *(dst+27) = *(src+27);          \
            *(dst+28) = *(src+28);          \
            *(dst+29) = *(src+29);          \
            *(dst+30) = *(src+30);          \
            *(dst+31) = *(src+31);          })            

#define APPLY_DQS_IN_DLY(r, f, v)   ({ RMOD_##r##_0(f, v); RMOD_##r##_1(f, v); RMOD_##r##_2(f, v); RMOD_##r##_3(f, v); })
#define APPLY_DQ_DLY(r, f, v)       ({ RMOD_##r##_0(f, v); RMOD_##r##_1(f, v); RMOD_##r##_2(f, v); RMOD_##r##_3(f, v); })
#define APPLY_DM_DLY(f, v)          ({ RMOD_DQ_DLY_1(f, v); })
// dpi_post_pi_sel0/1: DCK0/1
#define APPLY_DCK_PI(v)         ({  RMOD_PLL_PI0(dpi_post_pi_sel0, v, dpi_post_pi_sel1, v); \
                                    u32_t __v=(v>15)?1:0;\
                                    RMOD_PLL_CTL1(dpi_oesync_op_sel_ck, __v, dpi_oesync_op_sel_ck_1, __v); })
#define RETRV_DCK_PI()          RFLD_PLL_PI0(dpi_post_pi_sel0)
// dpi_post_pi_sel2/3/4/5: DQS0/1/2/3
#define APPLY_DQS_PI_0(v)       ({  RMOD_PLL_PI0(dpi_post_pi_sel2, v); RMOD_PLL_CTL1(dpi_oesync_op_sel_dqs_0, (v>15)?1:0); })
#define APPLY_DQS_PI_1(v)       ({  RMOD_PLL_PI0(dpi_post_pi_sel3, v); RMOD_PLL_CTL1(dpi_oesync_op_sel_dqs_1, (v>15)?1:0); })
#define APPLY_DQS_PI_2(v)       ({  RMOD_PLL_PI1(dpi_post_pi_sel4, v); RMOD_PLL_CTL1(dpi_oesync_op_sel_dqs_2, (v>15)?1:0); })
#define APPLY_DQS_PI_3(v)       ({  RMOD_PLL_PI1(dpi_post_pi_sel5, v); RMOD_PLL_CTL1(dpi_oesync_op_sel_dqs_3, (v>15)?1:0); })
#define APPLY_DQS_PI(v)         ({  RMOD_PLL_PI0(dpi_post_pi_sel2, v, dpi_post_pi_sel3, v);\
                                    RMOD_PLL_PI1(dpi_post_pi_sel4, v, dpi_post_pi_sel5, v); \
                                    u32_t __v=(v>15)?1:0;\
                                    RMOD_PLL_CTL1(dpi_oesync_op_sel_dqs_0, __v, dpi_oesync_op_sel_dqs_1, __v, dpi_oesync_op_sel_dqs_2, __v, dpi_oesync_op_sel_dqs_3, __v); })
#define RETRV_DQS_PI_0()        RFLD_PLL_PI0(dpi_post_pi_sel2)
#define RETRV_DQS_PI_1()        RFLD_PLL_PI0(dpi_post_pi_sel3)
#define RETRV_DQS_PI_2()        RFLD_PLL_PI1(dpi_post_pi_sel4)
#define RETRV_DQS_PI_3()        RFLD_PLL_PI1(dpi_post_pi_sel5)
#define RETRV_DQS_PI()          RETRV_DQS_PI_0()
// dpi_post_pi_sel6/7/8/9: DQ0/1/2/3
#define APPLY_DQ_PI_0(v)        ({  RMOD_PLL_PI1(dpi_post_pi_sel6, v); RMOD_PLL_CTL1(dpi_oesync_op_sel_dq_0, (v>15)?1:0); })
#define APPLY_DQ_PI_1(v)        ({  RMOD_PLL_PI2(dpi_post_pi_sel7, v); RMOD_PLL_CTL1(dpi_oesync_op_sel_dq_1, (v>15)?1:0); })
#define APPLY_DQ_PI_2(v)        ({  RMOD_PLL_PI2(dpi_post_pi_sel8, v); RMOD_PLL_CTL1(dpi_oesync_op_sel_dq_2, (v>15)?1:0); })
#define APPLY_DQ_PI_3(v)        ({  RMOD_PLL_PI2(dpi_post_pi_sel9, v); RMOD_PLL_CTL1(dpi_oesync_op_sel_dq_3, (v>15)?1:0); })
#define APPLY_DQ_PI(v)          ({  RMOD_PLL_PI1(dpi_post_pi_sel6, v);\
                                    RMOD_PLL_PI2(dpi_post_pi_sel7, v, dpi_post_pi_sel8, v, dpi_post_pi_sel9, v); \
                                    u32_t __v=(v>15)?1:0;\
                                    RMOD_PLL_CTL1(dpi_oesync_op_sel_dq_0, __v, dpi_oesync_op_sel_dq_1, __v, dpi_oesync_op_sel_dq_2, __v, dpi_oesync_op_sel_dq_3, __v); })
#define RETRV_DQ_PI_0()         RFLD_PLL_PI1(dpi_post_pi_sel6)
#define RETRV_DQ_PI_1()         RFLD_PLL_PI1(dpi_post_pi_sel7)
#define RETRV_DQ_PI_2()         RFLD_PLL_PI1(dpi_post_pi_sel8)
#define RETRV_DQ_PI_3()         RFLD_PLL_PI1(dpi_post_pi_sel9)
#define RETRV_DQ_PI()           RFLD_PLL_PI1(dpi_post_pi_sel6)
// dpi_post_pi_sel10/11: CS0/CS1
#define APPLY_CS_PI(v)          ({  RMOD_PLL_PI2(dpi_post_pi_sel10, v);\
                                    RMOD_PLL_PI3(dpi_post_pi_sel11, v);\
                                    u32_t __v=(v>15)?1:0;\
                                    RMOD_PLL_CTL1(dpi_oesync_op_sel_cs, __v, dpi_oesync_op_sel_cs_1, __v); })
#define RETRV_CS_PI()           RFLD_PLL_PI2(dpi_post_pi_sel10)

// dqs threshold,
#define APPLY_DQS_RSN(v)        ({ RMOD_AFIFO_STR_0(dqs_rd_str_num_0, v, dqs_rd_str_num_1, v,   \
                                                    dqs_rd_str_num_2, v, dqs_rd_str_num_3, v);})
#define RETRV_DQS_RSN()         RFLD_AFIFO_STR_0(dqs_rd_str_num_0)
// dq threshold
#define APPLY_DQ_RSN(v)         ({ RMOD_AFIFO_STR_0(dq_rd_str_num_0, v, dq_rd_str_num_1, v,   \
                                                    dq_rd_str_num_2, v, dq_rd_str_num_3, v);})
#define RETRV_DQ_RSN()          RFLD_AFIFO_STR_0(dq_rd_str_num_0)
// cmd threshold
#define APPLY_CMD_RSN(v)        ({ RMOD_AFIFO_STR_1(cmd_rd_str_num, v); })
#define RETRV_CMD_RSN()         RFLD_AFIFO_STR_1(cmd_rd_str_num)
// cs threshold
#define APPLY_CS_RSN(v)         ({ RMOD_AFIFO_STR_1(cmd_ex_rd_str_num, v); })
#define RETRV_CS_RSN()          RFLD_AFIFO_STR_1(cmd_ex_rd_str_num)
// rx threshold
#define APPLY_RX_RSN(v)         ({ RMOD_AFIFO_STR_2(rx_rd_str_num_0, v, rx_rd_str_num_1, v,   \
                                                    rx_rd_str_num_2, v, rx_rd_str_num_3, v);})
#define RETRV_RX_RSN()          RFLD_AFIFO_STR_2(rx_rd_str_num_0)

// DQS
#define ADD_DQS_RSN()           ({ u32_t _num; _num=RFLD_AFIFO_STR_0(dqs_rd_str_num_0)+1; APPLY_DQS_RSN(_num);})
#define SUB_DQS_RSN()           ({ u32_t _num; _num=RFLD_AFIFO_STR_0(dqs_rd_str_num_0)-1; APPLY_DQS_RSN(_num);})
// DQ
#define ADD_DQ_RSN()            ({ u32_t _num; _num=RFLD_AFIFO_STR_0(dq_rd_str_num_0)+1; APPLY_DQ_RSN(_num);})
#define SUB_DQ_RSN()            ({ u32_t _num; _num=RFLD_AFIFO_STR_0(dq_rd_str_num_0)-1; APPLY_DQ_RSN(_num);})
// CS
#define ADD_CS_RSN()            ({ u32_t _num; _num=RFLD_AFIFO_STR_1(cmd_ex_rd_str_num)+1; RMOD_AFIFO_STR_1(cmd_ex_rd_str_num, _num);})
#define SUB_CS_RSN()            ({ u32_t _num; _num=RFLD_AFIFO_STR_1(cmd_ex_rd_str_num)-1; RMOD_AFIFO_STR_1(cmd_ex_rd_str_num, _num);})
// RX
#define ADD_RX_RSN()            ({ u32_t _num; _num=RFLD_AFIFO_STR_2(rx_rd_str_num_0)+1; APPLY_RX_RSN(_num);})
#define SUB_RX_RSN()            ({ u32_t _num; _num=RFLD_AFIFO_STR_2(rx_rd_str_num_0)-1; APPLY_RX_RSN(_num);})
// DCK
#define ADD_DCK_RSN()           ({}) //({ SUB_DQS_RSN(); SUB_DQ_RSN(); SUB_CS_RSN(); SUB_RX_RSN();})
#define SUB_DCK_RSN()           ({}) //({ ADD_DQS_RSN(); ADD_DQ_RSN(); ADD_CS_RSN(); ADD_RX_RSN();})

#define RX_ODT_ALWAYS_ON(en)    ({  RMOD_READ_CTRL_2_0(odt_force_sel, en, odt_force_sig, en);\
                                    RMOD_READ_CTRL_2_1(odt_force_sel, en, odt_force_sig, en);\
                                    RMOD_READ_CTRL_2_2(odt_force_sel, en, odt_force_sig, en);\
                                    RMOD_READ_CTRL_2_3(odt_force_sel, en, odt_force_sig, en); })

#define SYNC_RD_DET()           ({ DPI_CTRL_1rv=0xC0; udelay(10); })
#define SYNC_CA_RAN()           ({ DPI_CTRL_1rv=0x30; udelay(10); })
#define SYNC_RD_DLY()           ({ DPI_CTRL_1rv=0xC;  udelay(10); })
#define SYNC_WR_DLY()           ({ DPI_CTRL_1rv=0x3;  udelay(10); })
#define SYNC_ALL_DLY()          ({ DPI_CTRL_1rv=0xFF; udelay(10); })

#define APPLY_TM_ODT_EN_OSD(v)  ({  u32_t _odd=v>>3, _sel=(v>>1)&0x3, _dly=v&0x1; \
                                    RMOD_READ_CTRL_2_0(tm_odt_en_odd, _odd, tm_odt_en_sel, _sel, tm_odt_en_dly, _dly); \
                                    RMOD_READ_CTRL_2_1(tm_odt_en_odd, _odd, tm_odt_en_sel, _sel, tm_odt_en_dly, _dly); \
                                    RMOD_READ_CTRL_2_2(tm_odt_en_odd, _odd, tm_odt_en_sel, _sel, tm_odt_en_dly, _dly); \
                                    RMOD_READ_CTRL_2_3(tm_odt_en_odd, _odd, tm_odt_en_sel, _sel, tm_odt_en_dly, _dly); })
#define APPLY_TM_ODT_EN(v)      ({  READ_CTRL_6_0rv=v; READ_CTRL_6_1rv=v; READ_CTRL_6_2rv=v; READ_CTRL_6_3rv=v; })

#endif
