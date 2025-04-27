#ifndef _MEMCNTLR_BSTC_H_
#define _MEMCNTLR_BSTC_H_

#define PACKED          __attribute__ ((packed))
#define MC_BASE         (0xB8143000)
#define BSTC_BASE       (0xBF800000)

#define MC_BUS_WIDTH    (128)
#define DQ_WIDTH        (16)
//#define BST_LEN         (BUS_WIDTH/(DQ_WIDTH*2))

// maximum bank, row, column support bit
#define MAX_BANK_BIT    (4)     //4
#define MAX_ROW_BIT     (17)    //17
#define MAX_COL_BIT     (11)

#define SRAM_CMD_BASE   (BSTC_BASE)
#define SRAM_CMD_BUSW   (MAX_BANK_BIT+MAX_ROW_BIT+(MAX_COL_BIT+1)+4+1)           // BusWidth=BANK+ROW+COL(DQ32 need +1)+BST_LEN+CMD
#define SRAM_CMD_ENTY   (32)
#define SRAM_CMD_SIZE   (SRAM_CMD_ENTY*SRAM_CMD_BUSW)

#define SRAM_MSK_BASE   (BSTC_BASE+0x01000)
#define SRAM_MSK_BUSW   (16)
#define SRAM_MSK_ENTY   (32)
#define SRAM_MSK_SIZE   (SRAM_MSK_ENTY*SRAM_MSK_BUSW)

#define SRAM_WD_BASE    (BSTC_BASE+0x03800)
#define SRAM_WD_BUSW    (128)
#define SRAM_WD_ENTY    (128)
#define SRAM_WD_SIZE    (SRAM_WD_BUSW*SRAM_WD_ENTY)

#define SRAM_RG_BASE    (BSTC_BASE+0x0C000)
#define SRAM_RG_BUSW    (128)
#define SRAM_RG_ENTY    (128)
#define SRAM_RG_SIZE    (SRAM_RG_BUSW*SRAM_RG_ENTY)

#define SRAM_RD_BASE    (BSTC_BASE+0x14800)
#define SRAM_RD_BUSW    (128)
#define SRAM_RD_ENTY    (128)
#define SRAM_RD_SIZE    (SRAM_RD_BUSW*SRAM_RD_ENTY)

#define WR_CMD       (1)
#define RD_CMD       (0)

typedef union {
    struct {
        unsigned int dummy:(64-SRAM_CMD_BUSW) PACKED;
        unsigned int bank:(MAX_BANK_BIT) PACKED;
        unsigned int row:(MAX_ROW_BIT) PACKED;
        unsigned int col:(MAX_COL_BIT+1) PACKED;
        unsigned int bl:(4) PACKED;
        unsigned int cmd:(1) PACKED;
    }f;
    unsigned int v[2];
} bstc_cmd_t;

typedef struct {
    unsigned int data0;
    unsigned int data1;
    unsigned int data2;
    unsigned int data3;
} bstc_data_t;

typedef struct {
    unsigned int bnk_size;
    unsigned int col_size;
    unsigned int wr_ent_cnt;
    unsigned int rd_ent_cnt;
} bstc_info_t;

#define CMD_WRITE(addr, data)   REG32((addr))     = data.v[1];\
                                REG32((addr+0x4)) = data.v[0];

#define mc_disable_bstc()  RMOD_CSR(bstc_idle, 1, mem_idle, 0)
void mc_enable_bstc(void);

#define get_col_size() ({ 8+RFLD_MISC(page_size); })
#define get_bnk_size() ({ 1+RFLD_MISC(bank_size); })

#define VA2COL(addr) (((addr)&((1<<info.col_size)-1))>>4)
#define VA2BNK(addr) (((addr)>>(info.col_size))&((1<<info.bnk_size)-1))
#define VA2ROW(addr) (((addr)&0x0FFFFFFF)>>(info.col_size+info.bnk_size))
#define PA2COL       VA2COL
#define PA2BNK       VA2BNK
#define PA2ROW(addr) (((addr)&0xFFFFFFFF)>>(info.col_size+info.bnk_size))

#endif //_MEMCNTLR_BSTC_H_
