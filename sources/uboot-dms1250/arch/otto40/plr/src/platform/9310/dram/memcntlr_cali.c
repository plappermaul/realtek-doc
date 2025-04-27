#include <soc.h>
#include <dram/memcntlr.h>
#include <util.h>

extern mc_info_t meminfo;
extern void bp_barrier() __attribute__ ((far));

#define ORDEAL_CACHE_SIZE   (0x8000)
#define ORDEAL_UNIT_SIZE    (128)       // 128 Byte
#define ORDEAL_UNIT_LEN     (ORDEAL_UNIT_SIZE/4)       // 32 words
#define ORDEAL_ADDR         (0x81000000)//(0x81234560)
#define ORDEAL_LEN          (0x8000)
#define _dcache_flush       _soc.bios.dcache_writeback_invalidate_all
#define _dcache_flush_range _soc.bios.dcache_writeback_invalidate_range
#define _icache_flush       _soc.bios.icache_invalidate_all
#define _ABS(v)             (((v)<0)?(-(v)):(v))
#define ENTER_FORCE_X16_MODE    u32_t dcr = DCRrv; RMOD_DCR(dq_32, 0, half_dq, 1, rank2, 0); udelay(10); mc_rxi310_init(1);
#define EXIT_FORCE_X16_MODE     DCRrv = dcr; udelay(10); mc_rxi310_init(1);

u32_t pat_ary[ORDEAL_UNIT_LEN] __attribute__ ((aligned(128)));

MEMCNTLR_SECTION
int fixed_pattern_gen(u32_t *ary, u32_t pat)
{
    u32_t pat_cnt = 0;
    for (pat_cnt =0; pat_cnt < ORDEAL_UNIT_LEN; pat_cnt+=1) {
        ary[pat_cnt]   = pat;
    }
    return 0;
}

MEMCNTLR_SECTION
int prb7_pattern_gen(u32_t *ary)
{
    int bit_cnt;
    short dq_loop_07; // dq[07:00]
    short dq_loop_15; // dq[15:08]
    short dq_loop_23; // dq[23:16]
    short dq_loop_31; // dq[31:24]

    short dq_07=0; // dq[07:00]
    short dq_15=0; // dq[15:08]
    short dq_23=0; // dq[23:16]
    short dq_31=0; // dq[31:24]
    int dq;    // dq[31:00]

    int pat_cnt = 0;
    unsigned int data_array[4], in_bit[127];

    data_array[0] = 0xFE040000 | (CPU_GET_CP0_CYCLE_COUNT()&0x0000FFFF); //0xfe041851;
    data_array[1] = 0xE40000FA | (CPU_GET_CP0_CYCLE_COUNT()&0x00FFFF00); //0xe459d4fa;
    data_array[2] = 0x0000B5BD | (CPU_GET_CP0_CYCLE_COUNT()&0xFFFF0000); //0x1c49b5bd;
    data_array[3] = 0x8D2E0000 | (CPU_GET_CP0_CYCLE_COUNT()&0x0000FFFF); //0x8d2ee654;

    for (bit_cnt =0; bit_cnt<127; bit_cnt++) {
        in_bit[bit_cnt] = (data_array[(bit_cnt/32)] & (0x1 << (31-bit_cnt))) >> (31-bit_cnt);
        //INFO_PRINTF("in_bit[%d]=0x%08x\n", bit_cnt, in_bit[bit_cnt]);
    }

    // generate DQ[31:0]
    dq_loop_07 = 0;  // dq[07:00]
    dq_loop_15 = 32; // dq[15:08]
    dq_loop_23 = 64; // dq[23:16]
    dq_loop_31 = 96; // dq[31:24]


    for (pat_cnt =0; pat_cnt < ORDEAL_UNIT_LEN; pat_cnt++) {
        // dq[07:00]
        for(bit_cnt=7; bit_cnt >=0; bit_cnt--) {
            dq_07 = (in_bit[dq_loop_07] << bit_cnt) | dq_07;
            if(dq_loop_07 == 126){
                dq_loop_07 = 0;
            } else {
                dq_loop_07 = dq_loop_07 + 1;
            }
            //INFO_PRINTF("dq_07=0x%x\n", dq_07);
        }

        // dq[15:08]
        for(bit_cnt=7; bit_cnt >=0; bit_cnt--) {
            dq_15 = (in_bit[dq_loop_15] << bit_cnt) | dq_15;
            if(dq_loop_15 == 126){
                dq_loop_15 = 0;
            } else {
                dq_loop_15 = dq_loop_15 + 1;
            }
        }

        // dq[23:16]
        for(bit_cnt=7; bit_cnt >=0; bit_cnt--) {
            dq_23 = (in_bit[dq_loop_23] << bit_cnt) | dq_23;
            if(dq_loop_23 == 126){
                dq_loop_23 = 0;
            } else {
                dq_loop_23 = dq_loop_23 + 1;
            }
        }

        // dq[34:24]
        for(bit_cnt=7; bit_cnt >=0; bit_cnt--) {
            dq_31 = (in_bit[dq_loop_31] << bit_cnt) | dq_31;
            if(dq_loop_31 == 126){
                dq_loop_31 = 0;
            } else {
                dq_loop_31 = dq_loop_31 + 1;
            }
        }

        dq = (dq_31 <<24) | (dq_23 <<16) |(dq_15 <<8) | dq_07;
        //printf ("[%03d] 0x%08X\n", pat_cnt, dq);
        //printf (f_data, "%8x\n", dq);
        ary[pat_cnt] = dq;

        dq_07 = 0; // dq[07:00]
        dq_15 = 0; // dq[15:08]
        dq_23 = 0; // dq[23:16]
        dq_31 = 0; // dq[31:24]
    }
    return 0;
}

u32_t ocp_memory_ordeal(u32_t sAddr, u32_t len)
{
    //prb7_pattern_gen(pat_ary);
    //fixed_pattern_gen(pat_ary, (CPU_GET_CP0_CYCLE_COUNT()&0x1)?0x5A5A5A5A:0xA5A5A5A5);
    volatile u32_t *dst, *src;
    u32_t l = len, m;
    u32_t res = 0;
    dst = (volatile u32_t *)sAddr;
    src = pat_ary;
    do {
        m=ORDEAL_CACHE_SIZE;
        while(m>=ORDEAL_UNIT_SIZE) {
            // assume: cache line size: 4 ways * 32Byte
            UNROLLING_MCPY_128BYTE(src, dst);
            //DBG_PRINTF("dst(0x%08x)=0x%08x, pat(0x%08x)=0x%08x\n", dst, *dst, src, *src);
            m-=ORDEAL_UNIT_SIZE;
            dst+=ORDEAL_UNIT_LEN;
        }
        l-=ORDEAL_CACHE_SIZE;
        // cache flush
        _dcache_flush();
    } while(l>=ORDEAL_CACHE_SIZE);

    l = len;
    dst = (volatile u32_t *)(sAddr+len-4);
    while(l>=ORDEAL_UNIT_SIZE) {
        // assume: cache line size: 4 ways * 32Byte
        for(src=(volatile u32_t *)(pat_ary+ORDEAL_UNIT_LEN-1);(u32_t)src>=(u32_t)pat_ary;src--, dst--) {
            res |= *src ^ *dst;
            //DBG_PRINTF("dst(0x%08x)=0x%08x, pat(0x%08x)=0x%08x, res=0x%08x\n", dst, *dst, src, *src, res);
        }
        l-=ORDEAL_UNIT_SIZE;
    }
    return res;
}

u32_t ocp_memory_ordeal_64(u32_t sAddr, u32_t len)
{
    prb7_pattern_gen(pat_ary);
    volatile u32_t *dst, *src;
    u32_t l = len, m;
    u32_t res, res_0=0, res_1=0;
    dst = (volatile u32_t *)sAddr;
    src = pat_ary;
    while(l>=ORDEAL_CACHE_SIZE) {
        m=ORDEAL_CACHE_SIZE;
        while(m>=ORDEAL_UNIT_SIZE) {
            // assume: cache line size: 4 ways * 32Byte
            UNROLLING_MCPY_128BYTE(src, dst);
            m-=ORDEAL_UNIT_SIZE;
            dst+=ORDEAL_UNIT_LEN;
            //DBG_PRINTF("dst(0x%08x)=0x%08x, pat(0x%08x)=0x%08x\n", dst, *dst, src, *src);
        }
        l-=ORDEAL_CACHE_SIZE;
        // cache flush
        _dcache_flush();
    }

    l = len;
    dst = (volatile u32_t *)(sAddr+len-4);
    while(l>=ORDEAL_UNIT_SIZE) {
        // assume: cache line size: 4 ways * 32Byte
        for(src=(volatile u32_t *)(pat_ary+ORDEAL_UNIT_LEN-1);(u32_t)src>=(u32_t)pat_ary;src--, dst--) {
            res = *src ^ *dst;
#if 0
             // if addr 0x0 => bit 31~16: DQ 0~15 falling, bit 15~0 DQ 0~15 raising
            //    addr 0x4 => bit 31~16: DQ31~16 falling, bit 15~0 DQ31~16 raising
            if(0==(((u32_t)dst)&0x4)) {
                res_0 |= res;
            } else {
                res_1 |= res;
            }
#else
            // if bit 31~16: DQ31~16, bit 15~0: DQ15~0
            res_0 |= res&0xFFFF;
            res_1 |= res>>16;
#endif
            //DBG_PRINTF("dst(0x%08x)=0x%08x, pat(0x%08x)=0x%08x, res=0x%08x\n", dst, *dst, src, *src, res);
        }
        l-=ORDEAL_UNIT_SIZE;
    }
    /* if(res_0!=res_1) {
        DBG_PRINTF("res_0 = 0x%x, res_1 = 0x%x\n", res_0, res_1);
    } */
    //DBG_PRINTF("res_0 = 0x%08x, res_1 = 0x%08x", res_0, res_1);
    res_0 = (res_0>>16)|(res_0&0xFFFF); // DQ0~15
    res_1 = (res_1>>16)|(res_1&0xFFFF); // DQ16~31
    res = res_0|(res_1<<16);
    //printf("=> res= 0x%08x\n", res);
    //DBG_PRINTF("=> translate to res = 0x%x\n", res);
    return res;
}

extern void mc_enable_bstc(void);

#if 1
MEMCNTLR_SECTION
u32_t bstc_sequential_test(u32_t sAddr, u32_t len) {
    u32_t con_addr, test_addr;
    u32_t cnt, data_cnt=0, idx;
    const u32_t burst=4;
    u32_t pat, err, res;

    BCR_T bcr;
    BSTC_CMD_T cmd = {{0}};

    BSTC_INFO_T info = { .col_size = get_col_size(),
                         .bnk_size = get_bnk_size(),
                         .wr_ent_cnt = BSTC_SRAM_CMD_ENTY/2,
                         .rd_ent_cnt = BSTC_SRAM_CMD_ENTY/2};

    //DBG_PRINTF("(%s) sAddr 0x%x, len 0x%x\n", __FUNCTION__, sAddr, len);
    for (test_addr=sAddr, res=0; test_addr<(sAddr+len);)
    {
        //DBG_PRINTF("test_addr 0x%x\n", test_addr);
        mc_enable_bstc();

        // set BCR
        bcr.f.rd_ex_cnt = burst*info.rd_ent_cnt*(RXI310_BUS_WIDTH/32);;
        bcr.f.at_err_stop = 0;
        bcr.f.dis_msk = 1;
        bcr.f.loop = 0; // loop mode
        bcr.f.cmp = 1;
        bcr.f.stop = 0;
        BCRrv = bcr.v;

        // set loop count
        RMOD_BCT(loop_cnt, 0);
        con_addr = BSTC_SRAM_CMD_BASE;
        for (cnt=0; cnt<info.wr_ent_cnt; cnt++, con_addr+=0x8) {
            // set CMD_SRAM
            cmd.f.bank = BSTC_VA2BNK(info, test_addr);
            cmd.f.col = BSTC_VA2COL(info, test_addr);
            cmd.f.row = BSTC_VA2ROW(info, test_addr);
            cmd.f.bl = burst;
            cmd.f.cmd = BSTC_WR_CMD;    // write
            BSTC_CMD_WRITE(con_addr, cmd);
            //DBG_PRINTF("con_addr 0x%x test_addr 0x%x\n", con_addr, test_addr);

            // set CMD_SRAM, follow write
            cmd.f.cmd = BSTC_RD_CMD;    // read
            BSTC_CMD_WRITE(con_addr+info.wr_ent_cnt*8, cmd);
            //DBG_PRINTF("con_addr 0x%x test_addr 0x%x\n", con_addr, test_addr);
            test_addr+= burst*(RXI310_BUS_WIDTH/8);
        }

        // prepare WD and RG
        con_addr = BSTC_SRAM_WD_BASE;
        for (cnt=0, idx=0; cnt<(info.wr_ent_cnt*burst); cnt++) {
            for (data_cnt=0; data_cnt<BSTC_SRAM_WD_BUSW; data_cnt+=32, con_addr+=4) { // 32-bit
                idx%=ORDEAL_UNIT_LEN;
                pat=pat_ary[idx++];
                REG32(con_addr)=pat;
                REG32(con_addr+(BSTC_SRAM_RG_BASE-BSTC_SRAM_WD_BASE))=pat;
                //DBG_PRINTF("0x%x = 0x%x (0x%x)\n", con_addr, pat, REG32(con_addr));
            }
        }

        // kick off BSTC
        udelay(1000);
        //INFO_PRINTF("kick off BSTC WRITE READ, addr=0x%x... ", test_addr);
        //RMOD_CCR(btt, 1, init, 0, dpit, 0);
        CCRrv = 0x4;

        while (0==RFLD_CCR(btt)){udelay(10); puts("*\b");};
        //puts("done\n");

        // disable BSTC
        mc_disable_bstc();
        udelay(1000);
        mc_rxi310_init(1);

        // check BST and BER

        //if(0!=BERrv) {
        err = BERrv;
        //ERR_PRINTF("error bit 0x%x\n", err);
            //BST_T bst = {.v=BSTrv};
            //ERR_PRINTF("error bit 0x%x, data received=%d, error count=%d, first error addr.=0x%x\n",
            //    err, bst.f.rd_in_st ,bst.f.err_cnt, bst.f.err_fst_th);
        //}
        res |= err;
        udelay(1);
    }
    return res;
}
#endif

//#define memory_ordeal(...)    ({ u32_t __r=ocp_memory_ordeal(ORDEAL_ADDR, ORDEAL_LEN); ocp_memory_ordeal_64(ORDEAL_ADDR, ORDEAL_LEN*2); __r;})
#define memory_ordeal(...)          ocp_memory_ordeal(ORDEAL_ADDR, ORDEAL_LEN)
#define bstc_memory_ordeal(...)     bstc_sequential_test(ORDEAL_ADDR, 0x400)
#define fixp_memory_ordeal(p)       ({ u32_t __r; fixed_pattern_gen(pat_ary, p); __r=ocp_memory_ordeal(ORDEAL_ADDR, ORDEAL_LEN); __r; })

#define POS_SCAN_CHECK(t, v)    ({ int _v=v;\
                                   if(31<=_v) { ADD_##t##_RSN(); _v=0; }\
                                   else { _v+=1; } _v;})
#define NEG_SCAN_CHECK(t, v)    ({ int _v=v;\
                                   if(0>=_v)  { SUB_##t##_RSN(); _v=31; }\
                                   else { _v-=1; } _v;})



inline
void mem_pll_disable(void) {    // do CLK_EN/OE disable
    //INFO_PRINTF("MEM PLL disable\n");
    RMOD_PLL_CTL0(dpi_mck_clk_en, 0x0);
    RMOD_PLL_CTL1(dpi_clk_oe, 0x0);
    //SYNC_ALL_DLY();
    udelay(1000);
}

inline
void mem_pll_enable(void) {     // do CLK_EN/OE enable
    //INFO_PRINTF("MEM PLL enable\n");
    RMOD_PLL_CTL0(dpi_mck_clk_en, 0x1FFF);
    RMOD_PLL_CTL1(dpi_clk_oe, 0x7);
    //SYNC_RD_DLY();  // try!?
    //SYNC_WR_DLY();
    SYNC_ALL_DLY();
    mc_rxi310_init(1);
    udelay(1000);
}

MEMCNTLR_SECTION
inline u32_t _find_upper_and_lower(u32_t d, int *u, int *l) {
    u32_t i, t;
    int s=-1, e=-1;
    *u=*l=0;
    for(i=0; i<32; i++) {
        t=(d>>i)&0x1;
        if((1==t) && (-1==s)) s=i;
        else if(0==t && (-1!=s)) { break; }
        else e=i;
    }
    if(-1==s) {
        ERR_PRINTF("invalid boundary: res 0x%08x, upper %d, lower %d\n", d, e, s);
        return 1;
    }
    *u=e, *l=s;
    return 0;
}

MEMCNTLR_SECTION
inline u32_t _find_longest_range(u32_t d0_31, u32_t d32_63, int *u, int *l) {
    u32_t i, t;
    int s=-1, e=-1;
    int _s=-1, _e=-1;
    int _w, w;
    *u=*l=0;
    for(i=0, _w=0, w=0; i<64; i++) {
        if(i>=32) { t=(d32_63>>(i-32))&0x1;}
        else { t=(d0_31>>i)&0x1; }

        if(-1==_s) {
            if(1==t) { _s=i; _e=i; }  // START
        } else if(((0==t)||(63==i))&&(-1!=_s)) { // END
            _w=_e-_s;
            if(_w>0&&_w>=w) {
                s=_s; e=_e; // record
                w=_w;
            }
            _s=-1; _e=-1;
        } else {
            _e=i;
        }
    }
    if(-1==s) {
        ERR_PRINTF("invalid boundary: upper %d, lower %d (0x%x and 0x%x)\n", e, s, d32_63, d0_31);
        return 1;
    }
    *u=e, *l=s;
    return 0;
}

MEMCNTLR_SECTION
inline void _dbg_print_hex(u32_t d0_31, u32_t d32_63, u32_t b) {
    int i, j=0, d;
    for(j=0; j<4; j++) {
        d=(j>1)?d32_63:d0_31;
        for(i=15; i>=0; i--) {
            printf("  %3d ", (i+j*16+(b-32)));
        }
        puts("\n");
        for(i=15; i>=0; i--) {
            printf("  %3d ", (d>>(i+(j&0x1)*16))&0x1);
        }
        puts("\n");
    }
}

MEMCNTLR_SECTION
int do_write_leveling(PI_CALI_T *info, u8_t dbg) {
    u32_t __attribute__((unused)) reg=1;
    u32_t i, j;
    u32_t res[4], r=0;

    u32_t dck_pi = RETRV_DCK_PI();
    u32_t dqs_pi = RETRV_DQS_PI();
    u32_t dqs_rsn = RETRV_DQS_RSN();
    u32_t _dqs_rsn;

    // assign for higher setting
    _dqs_rsn = 3;
    // turn on MR1
    u32_t mr1 =MR1rv;
    MR1rv = mr1 | (1<<7);
    mc_rxi310_init(1);

    /* scan 1st DQS PI */
    mem_pll_disable();
    APPLY_DCK_PI(0);
    APPLY_DQS_RSN(_dqs_rsn);
    mem_pll_enable();
    INFO_PRINTF("Write Leveling ... \n");

    for(i=0; i<2; i++, _dqs_rsn--) {
        memset((char *)res, 0, sizeof(res));
        APPLY_DQS_RSN(_dqs_rsn);
        for(j=0; j<32; j++) {
            mem_pll_disable();
            APPLY_DQS_PI(j);
            mem_pll_enable();
            // enable write leveling
            RMOD_WRLVL_CTRL(wrlvl_en, 1, wrlvl_trig, 1);
            udelay(1000);
            // read result,
            reg=WRLVL_RD_DATArv;
            res[0] |= (reg&0x1)<<j;
            res[1] |= ((reg>>8)&0x1)<<j;
            if(info->dq32) {
                res[2] |= ((reg>>16)&0x1)<<j;
                res[3] |= ((reg>>24)&0x1)<<j;
            }
        }
        info->raw_dqss[0][i]=res[0];  // DQS0
        info->raw_dqss[1][i]=res[1];  // DQS1
        if(info->dq32) {
            info->raw_dqss[2][i]=res[2]; // DQS2
            info->raw_dqss[3][i]=res[3]; // DQS3
        }
    }
    // turn off MR1
    MR1rv = mr1;
    mc_rxi310_init(1);

    // disable write leveling
    RMOD_WRLVL_CTRL(wrlvl_en, 0);

    int upper, lower;
    for(i=0; i<(2<<info->dq32); i++) {
        r=_find_longest_range(info->raw_dqss[i][1], info->raw_dqss[i][0], &upper, &lower);
        if(0!=r)  {
            ERR_PRINTF("%s failed\n", __FUNCTION__);
            return 1;
        }
        info->res_dqss[i]=lower-32;
        INDT_PRINTF("DQS%d: DQSS=%d\n", i, info->res_dqss[i]);
        if(dbg) {
            DBG_PRINTF("DQS%d res=0x%08x%08x, upper(%d), lower(%d)\n",
                    i, info->raw_dqss[i][0], info->raw_dqss[i][1], upper-32, lower-32);
            _dbg_print_hex(info->raw_dqss[i][1], info->raw_dqss[i][0], 0);
        }
        if(_ABS((info->res_dqss[0]-info->res_dqss[i]))>15) return 2; // more than half clock
    }

    mem_pll_disable();
    APPLY_DCK_PI(dck_pi);
    APPLY_DQS_PI(dqs_pi);
    APPLY_DQS_RSN(dqs_rsn);
    mem_pll_enable();

    return r;
}

#define DBG_DUMP_RSN()          DBG_PRINTF("dqs_rsn=%d, dq_rsn=%d, cs_rsn=%d, cmd_rsn=%d, rx_rsn=%d\n", info->dqs_rsn, info->dq_rsn, info->cs_rsn, info->cmd_rsn, info->rx_rsn)
#define DBG_DUMP_RSN_IN_REG()   DBG_PRINTF("AFIFO_STR_0=0x%08x\nDD: AFIFO_STR_1=0x%08x\nDD: AFIFO_STR_2=0x%08x\n", AFIFO_STR_0rv, AFIFO_STR_1rv, AFIFO_STR_2rv)
#define DQS_DQ_OFF      (8)

MEMCNTLR_SECTION
u32_t do_dck_pi_scan(PI_CALI_T *info, u8_t dbg)
{
    u32_t res, err;
    int idx, I, J, K, L, upper, lower;
    short pi_mv;

    /* scan DCK PI map (positive) */
    I = info->init_dck_pi;
    J = info->init_dqs_pi;
    K = info->init_dq_pi;
    L = info->init_cs_pi;
    mem_pll_disable();
    APPLY_DQS_RSN(info->dqs_rsn);   // +1 for prevent afifo=0
    APPLY_DQ_RSN(info->dq_rsn);
    APPLY_CS_RSN(info->cs_rsn);
    APPLY_CMD_RSN(info->cmd_rsn);
    APPLY_RX_RSN(info->rx_rsn);
    mem_pll_enable();

    if(dbg) {
        DBG_PRINTF("DCK_PI(positive): init DQS_PI=%d, DQ_PI=%d, CS_PI=%d\n", J, K, L);
        DBG_DUMP_RSN(); //DBG_DUMP_RSN_IN_REG();
    }

    for(idx=0, res=0; idx<32; idx++) {
        mem_pll_disable();
        APPLY_DCK_PI(I);
        APPLY_DQS_PI(J);
        APPLY_DQ_PI(K);
        APPLY_CS_PI(L);
        mem_pll_enable();

        err=memory_ordeal();
        if(0==err) {res|=1<<idx;}
        if(2==dbg) {
            DBG_PRINTF("[%3d] I=%2d, J=%2d, K=%2d, L=%2d, err=0x%x\n", idx+info->init_dck_pi, I, J, K, L, err);
        }

        if(idx<31) {
            I=POS_SCAN_CHECK(DCK, I);
            L=POS_SCAN_CHECK(CS,  L);
            J=POS_SCAN_CHECK(DQS, J);
            K=POS_SCAN_CHECK(DQ,  K);
        }
    }
    info->raw_dck_pi[0] = res;

    /* scan DCK PI map (negative) */

    mem_pll_disable();
    APPLY_DQS_RSN(info->dqs_rsn);
    APPLY_DQ_RSN(info->dq_rsn);
    APPLY_CS_RSN(info->cs_rsn);
    mem_pll_enable();
    I=NEG_SCAN_CHECK(DCK, info->init_dck_pi);
    L=NEG_SCAN_CHECK(CS,  info->init_cs_pi );
    J=NEG_SCAN_CHECK(DQS, info->init_dqs_pi);
    K=NEG_SCAN_CHECK(DQ,  info->init_dq_pi );

    if(dbg) {
        DBG_PRINTF("DCK_PI(negative): init DQS_PI=%d, DQ_PI=%d, CS_PI=%d\n", J, K, L);
        DBG_DUMP_RSN(); //DBG_DUMP_RSN_IN_REG();
    }

    for(idx=31, res=0; idx>=0; idx--) {
        mem_pll_disable();
        APPLY_DCK_PI(I);
        APPLY_DQS_PI(J);
        APPLY_DQ_PI(K);
        APPLY_CS_PI(L);
        mem_pll_enable();

        err=memory_ordeal();
        if(0==err) {res|=1<<idx;}
        if(2==dbg) {
            DBG_PRINTF("[%3d] I=%2d, J=%2d, K=%2d, L=%2d, err=0x%x\n", idx-32+info->init_dck_pi, I, J, K, L, err);
        }

        if(idx>0) {
            I=NEG_SCAN_CHECK(DCK, I);
            L=NEG_SCAN_CHECK(CS, L);
            J=NEG_SCAN_CHECK(DQS, J);
            K=NEG_SCAN_CHECK(DQ, K);
        }
    }
    info->raw_dck_pi[1] = res;

    if(0!=_find_longest_range(info->raw_dck_pi[1], info->raw_dck_pi[0], &upper, &lower)) {
        ERR_PRINTF("%s failed (res=0x%08x%08x)\n", __FUNCTION__, info->raw_dck_pi[0], info->raw_dck_pi[1]);
        return 1;
    }
    upper = upper-32+info->init_dck_pi;
    lower = lower-32+info->init_dck_pi;

    if(dbg) {
        _dbg_print_hex(info->raw_dck_pi[1], info->raw_dck_pi[0], info->init_dck_pi);
        DBG_PRINTF("res=0x%08x%08x, upper(%d), lower(%d)\n",
                info->raw_dck_pi[0], info->raw_dck_pi[1], upper, lower);
    }

    // check result
    info->res_dck_pi=lower+(upper-lower)/2;
    INDT_PRINTF("DCK_PI range: (%3d) %3d~%3d  \t==> DCK_PI=%3d, cs_rsn=%d, dqs_rsn=%d, dq_rsn=%d\n",
                 info->init_dck_pi, lower, upper, info->res_dck_pi, info->cs_rsn, info->dqs_rsn, info->dq_rsn);

    // check dck
    if(info->res_dck_pi<0) {
        info->res_dck_pi+=32;
        // dqs/dq/cs rsn -1 for move forward
        info->dqs_rsn--;
        info->dq_rsn--;
        info->cs_rsn--;
        pi_mv=-1;
    } else if(info->res_dck_pi>31) {
        info->res_dck_pi-=32;
        // dqs/dq/cs rsn +1 for move backward
        info->dqs_rsn++;
        info->dq_rsn++;
        info->cs_rsn++;
        pi_mv=1;
    } else { pi_mv=0; }
    info->init_cs_pi=info->res_dck_pi;

    int dqss_avg;
    for(idx=0, dqss_avg=0; idx<(2<<info->dq32); idx++) {
        dqss_avg += info->res_dqss[idx];
    }
    dqss_avg = info->res_dck_pi + (dqss_avg >> (1<<info->dq32));
    info->init_dqs_pi=info->res_dck_pi+dqss_avg;

    // check dqs boundary
    if(info->init_dqs_pi>31) {
        info->init_dqs_pi-=32;
        info->dqs_rsn++;
        info->dq_rsn++;
    } else if(info->init_dqs_pi<0) {
        info->init_dqs_pi+=32;
        info->dqs_rsn--;
        info->dq_rsn--;
    }

    // check dq boundary
    info->init_dq_pi=info->init_dqs_pi+DQS_DQ_OFF;
    if(info->init_dq_pi>31) {
        info->init_dq_pi-=32;
        info->dq_rsn++;
    } else if(info->init_dq_pi<0) {
        info->init_dq_pi+=32;
        info->dq_rsn--;
    }

    if(0!=pi_mv) {
        INDT_PRINTF("\t\t\t\t\t==> DCK_PI=%3d, cs_rsn=%d, dqs_rsn=%d, dq_rsn=%d\n",
                    info->res_dck_pi, info->cs_rsn, info->dqs_rsn, info->dq_rsn);
    }
    if(dbg) {
        DBG_PRINTF("DQS_PI: %d, DQ_PI: %d, CS_PI: %d\n", info->init_dqs_pi, info->init_dq_pi, info->init_cs_pi);
    }

    // restore setting
    mem_pll_disable();
    APPLY_DCK_PI(info->res_dck_pi);
    APPLY_DQS_PI(info->init_dqs_pi);
    APPLY_DQ_PI(info->init_dq_pi);
    APPLY_CS_PI(info->init_cs_pi);
    APPLY_DQS_RSN(info->dqs_rsn);
    APPLY_DQ_RSN(info->dq_rsn);
    APPLY_CS_RSN(info->cs_rsn);
    mem_pll_enable();

    if(dbg) {DBG_DUMP_RSN(); }//DBG_DUMP_RSN_IN_REG();}
    return 0;
}

MEMCNTLR_SECTION
u32_t do_cs_pi_scan(PI_CALI_T *info, u8_t dbg)
{
    u32_t res, err;
    int idx, L, upper, lower;
    short pi_mv;

    ENTER_FORCE_X16_MODE    // for DDR4, CS PI scan failed easily
    
     /* scan CS PI map (positive) */
    L = info->init_cs_pi;
    if(dbg) {
        DBG_PRINTF("CS_PI(positive): init CS_PI=%d\n", L);
    }

    for(idx=0, res=0; idx<32; idx++) {
        mem_pll_disable();
        APPLY_CS_PI(L);
        mem_pll_enable();

        err=memory_ordeal();
        if(0==err) {res|=(1<<idx);}
        if(2==dbg) {DBG_PRINTF("[%3d] L=%2d, err=0x%x\n", idx+info->init_cs_pi, L, err);}
        if(idx<31) {
            L=POS_SCAN_CHECK(CS, L);
        }
    }
    info->raw_cs_pi[0] = res;

    /* scan CS PI map (negative) */
    mem_pll_disable();
    APPLY_CS_RSN(info->cs_rsn);
    mem_pll_enable();
    L=NEG_SCAN_CHECK(CS, info->init_cs_pi);

    if(dbg) {
        DBG_PRINTF("CS_PI(negative): init CS_PI=%d\n", L);
        DBG_DUMP_RSN();//DBG_DUMP_RSN_IN_REG();
    }

    for(idx=31, res=0; idx>=0; idx--) {
        mem_pll_disable();
        APPLY_CS_PI(L);
        mem_pll_enable();

        err=memory_ordeal();
        if(0==err) {res|=(1<<idx);}
        if(2==dbg) {DBG_PRINTF("[%3d] L=%2d, err=0x%x\n", idx-32+info->init_cs_pi, L, err);}

        if(idx>0) {
            L=NEG_SCAN_CHECK(CS, L);
        }
    }

    info->raw_cs_pi[1] = res;

    if(0!=_find_longest_range(info->raw_cs_pi[1], info->raw_cs_pi[0], &upper, &lower))  {
        ERR_PRINTF("%s failed (res=0x%08x%08x)\n", __FUNCTION__, info->raw_cs_pi[0], info->raw_cs_pi[1]);
        res=1; goto cs_done;
    }
    upper = upper-32+info->init_cs_pi;
    lower = lower-32+info->init_cs_pi;

    if(dbg) {
        _dbg_print_hex(info->raw_cs_pi[1], info->raw_cs_pi[0], info->init_cs_pi);
        DBG_PRINTF("res=0x%08x%08x, upper(%d), lower(%d)\n", info->raw_cs_pi[0], info->raw_cs_pi[1], upper, lower);
    }

    // check result
    info->res_cs_pi=lower+(upper-lower)/2; //ROUND UP
    INDT_PRINTF("CS_PI range:  (%3d) %3d~%3d \t==> CS_PI =%3d, cs_rsn=%d\n", info->init_cs_pi, lower, upper, info->res_cs_pi, info->cs_rsn);

    if(info->res_cs_pi<0) {
        info->res_cs_pi+=32;
        info->cs_rsn--;
        pi_mv=-1;
    } else if(info->res_cs_pi>31) {
        info->res_cs_pi-=32;
        info->cs_rsn++;
        pi_mv=1;
    } else { pi_mv=0; }
    if(0!=pi_mv) {  INDT_PRINTF("\t\t\t\t\t==> CS_PI =%3d, cs_rsn=%d\n", info->res_cs_pi, info->cs_rsn); }

    mem_pll_disable();
    APPLY_CS_PI(info->res_cs_pi);
    APPLY_CS_RSN(info->cs_rsn);
    mem_pll_enable();

    if(dbg) {DBG_DUMP_RSN(); }//DBG_DUMP_RSN_IN_REG();}
 
    res=0;
cs_done: 
    EXIT_FORCE_X16_MODE
    return res;
}

#define DQS_PI_USE_WL
MEMCNTLR_SECTION
u32_t do_dqs_pi_scan(PI_CALI_T *info, u8_t dbg)
{
    int idx, pi_mv;
#ifndef DQS_PI_USE_WL   // using calibration result
    u32_t res, err;
    int J, K, upper, lower;

    /* scan DQS map (positive) */
    J = info->init_dqs_pi;
    K = info->init_dq_pi;
    if(dbg) {
        DBG_PRINTF("DQS_PI(positive): init DQS_PI=%d as DCK_PI, DQ_PI=%d\n", J, K);
        DBG_DUMP_RSN();
        //DBG_DUMP_RSN_IN_REG();
    }

    for(idx=0, res=0; idx<32; idx++) {
        mem_pll_disable();
        APPLY_DQS_PI(J);
        APPLY_DQ_PI(K);
        mem_pll_enable();

        err=memory_ordeal();
        if(0==err) {res|=(1<<idx);}
        if(2==dbg) {DBG_PRINTF("[%3d] J=%d, K=%d, err=0x%x\n", idx+info->init_dqs_pi, J, K, err);}
        udelay(1000);   // must! why?
        bp_barrier();

        if(idx<31) {
            J=POS_SCAN_CHECK(DQS, J);
            K=POS_SCAN_CHECK(DQ, K);
        }
    }
    info->raw_dqs_pi[0] = res;

    /* scan DQS map (negative) */
    mem_pll_disable();
    APPLY_DQS_RSN(info->dqs_rsn);
    APPLY_DQ_RSN(info->dq_rsn);
    mem_pll_enable();
    J=NEG_SCAN_CHECK(DQS, info->init_dqs_pi);
    K=NEG_SCAN_CHECK(DQ,  info->init_dq_pi);

    if(dbg) {
        DBG_PRINTF("DQS_PI(negtive): init DQS_PI=%d as DCK_PI, DQ_PI=%d\n", J, K);
        DBG_DUMP_RSN();
        //DBG_DUMP_RSN_IN_REG();
    }

    for(idx=31, res=0; idx>=0; idx--) {
        mem_pll_disable();
        APPLY_DQS_PI(J);
        APPLY_DQ_PI(K);
        mem_pll_enable();

        err=memory_ordeal();
        if(0==err) {res|=1<<idx;}
        if(2==dbg) {DBG_PRINTF("[%3d] J=%d, K=%d, err=0x%x\n", idx-32+info->init_dqs_pi, J, K, err);}

        if(idx>0) {
            J=NEG_SCAN_CHECK(DQS, J);
            K=NEG_SCAN_CHECK(DQ, K);
        }
    }
    info->raw_dqs_pi[1] = res;

    // check boundary
    if(0!=_find_longest_range(info->raw_dqs_pi[1], info->raw_dqs_pi[0], &upper, &lower))  {
        ERR_PRINTF("%s failed (res=0x%08x%08x)\n", __FUNCTION__, info->raw_dqs_pi[0], info->raw_dqs_pi[1]);
        return 1;
    }
    upper = upper-32+info->init_dqs_pi;
    lower = lower-32+info->init_dqs_pi;

    info->res_dqs_pi[0] = lower+(upper-lower)/2; // FIXME! check which one is better
    info->res_dqs_pi[1] = info->res_dqs_pi[0];
    if(info->dq32) {
        info->res_dqs_pi[3] = info->res_dqs_pi[2] = info->res_dqs_pi[0];
    }
    if(dbg) {
        _dbg_print_hex(info->raw_dqs_pi[1], info->raw_dqs_pi[0], info->init_dqs_pi);
        DBG_PRINTF("res=0x%08x%08x\n", info->raw_dqs_pi[0], info->raw_dqs_pi[1]);
        DBG_DUMP_RSN(); //DBG_DUMP_RSN_IN_REG();
    }
    INDT_PRINTF("DQS_PI range: (%3d) %3d~%3d \t==> DQS_PI=%3d, dqs_rsn=%d\n", info->init_dqs_pi, lower, upper, info->res_dqs_pi[0], info->dqs_rsn);

    // check DQS0 PI
    if(info->res_dqs_pi[0]<0) {
        info->res_dqs_pi[0]+=32;
        info->dqs_rsn--;
        info->dq_rsn--;
        pi_mv=-1;
    } else if(info->res_dqs_pi[0]>31) {
        info->res_dqs_pi[0]-=32;
        info->dqs_rsn++;
        info->dq_rsn++;
        pi_mv=1;
    } else { pi_mv=0; }

    info->init_dq_pi = info->res_dqs_pi[0] + DQS_DQ_OFF;
    if(info->init_dq_pi<0) {
        info->init_dq_pi+=32;
        info->dq_rsn--;
    } else if(info->init_dq_pi>31) {
        info->init_dq_pi-=32;
        info->dq_rsn++;
    }

    if(0!=pi_mv) { INDT_PRINTF("\t\t\t\t\t==> DQS_PI=%3d, dqs_rsn=%d\n", info->res_dqs_pi[0], info->dqs_rsn); }

#else   // using write leveling result
    // using cmd_rsn to restore dck
    info->dqs_rsn = info->cmd_rsn+1;
    info->dq_rsn = info->dqs_rsn;
    INDT_PRINTF("DQS_PI range: from write leveling\t==> dqs_rsn=%d, dq_rsn=%d\n", info->dqs_rsn, info->dq_rsn);

    // check DQS0~3 PI for only one rsn support, so we need to align them
    for(idx=0, pi_mv=0; idx<(2<<info->dq32); idx++) {
        pi_mv += info->res_dqss[idx];
    }
    pi_mv = info->res_dck_pi + (pi_mv >> (1<<info->dq32));

    // align rsn
    for(idx=0; idx<(2<<info->dq32); idx++) {
        info->res_dqs_pi[idx] = info->res_dck_pi+info->res_dqss[idx];
        INDT_PRINTF("\t\t\t\t\t    DQS%d_PI=%3d => ", idx, info->res_dqs_pi[idx]);

        if(pi_mv<0) {
            if(info->res_dqs_pi[idx]>=0) {
                info->res_dqs_pi[idx] = 31;
            } else {
                info->res_dqs_pi[idx] += 32;
            }
        } else if(pi_mv>31) {
            if(info->res_dqs_pi[idx]<=31) {
                info->res_dqs_pi[idx] = 0;
            } else {
                info->res_dqs_pi[idx] -= 32;
            }
        } else {
            if(info->res_dqs_pi[idx]<0) {
                info->res_dqs_pi[idx] = 0;
            } else if(info->res_dqs_pi[idx]>31) {
                info->res_dqs_pi[idx] = 31;
            }
        }
        printf("%3d\n", info->res_dqs_pi[idx]);
    }

    // check DQ PI and rsn
    // align rsn to DQS PI
    if(pi_mv<0) {
        info->dqs_rsn--;
        info->dq_rsn--;
    } else if(pi_mv>31) {
        info->dqs_rsn++;
        info->dq_rsn++;
    }

    info->init_dq_pi = info->res_dqs_pi[0] + DQS_DQ_OFF;
    if(info->init_dq_pi<0) {
        info->init_dq_pi+=32;
        info->dq_rsn--;
    } else if(info->init_dq_pi>31) {
        info->init_dq_pi-=32;
        info->dq_rsn++;
    }
    INDT_PRINTF("\t\t\t\t\t    dqs_rsn=%d, dq_rsn=%d\n", info->dqs_rsn, info->dq_rsn);
#endif

    // restore setting
    mem_pll_disable();
    APPLY_DQS_PI_0(info->res_dqs_pi[0]);
    APPLY_DQS_PI_1(info->res_dqs_pi[1]);
    APPLY_DQS_PI_2(info->res_dqs_pi[2]);
    APPLY_DQS_PI_3(info->res_dqs_pi[3]);
    APPLY_DQS_RSN(info->dqs_rsn);
    mem_pll_enable();

    if(dbg) {DBG_DUMP_RSN(); }//DBG_DUMP_RSN_IN_REG();}

    return 0;
}

MEMCNTLR_SECTION
u32_t do_dq_pi_scan(PI_CALI_T *info, u8_t dbg)
{
    u32_t res[4], err;
    int idx, K, upper, lower, pi_mv;

    /* scan DQ PI map (positive) */
    // fix CK PI & DQS PI from scan map, init DQ PI =DQS PI
    K=info->init_dq_pi=info->res_dqs_pi[0];
    info->dq_rsn=info->dqs_rsn;
    mem_pll_disable();
    APPLY_DQ_PI(info->init_dq_pi);
    APPLY_DQ_RSN(info->dq_rsn);
    mem_pll_enable();

    if(dbg) {
        DBG_PRINTF("DQ_PI(positive): init DQ_PI=%d \n", K);
        DBG_DUMP_RSN();//DBG_DUMP_RSN_IN_REG();
    }

    res[0]=res[1]=res[2]=res[3]=0;
    for(idx=0; idx<32; idx++) {
        //fixed_pattern_gen(pat_ary, (idx&0x1)?0x5A5A5A5A:0xA5A5A5A5);
        mem_pll_disable();
        APPLY_DQ_PI(K);
        mem_pll_enable();

        err=ocp_memory_ordeal_64(ORDEAL_ADDR, ORDEAL_LEN);//memory_ordeal();
        res[0]|=(0==(err&0xFF)?1:0)<<idx;
        res[1]|=(0==((err>>8)&0xFF)?1:0)<<idx;
        res[2]|=(0==((err>>16)&0xFF)?1:0)<<idx;
        res[3]|=(0==((err>>24)&0xFF)?1:0)<<idx;
        //if(0==err) {res|=1<<idx;}
        if(2==dbg) {DBG_PRINTF("[%d]K=%d, err=0x%x\n", idx, K, err);}

        if(idx<31) {
            K=POS_SCAN_CHECK(DQ, K);
        }
    }
    info->raw_dq_pi[0][0] = res[0];
    info->raw_dq_pi[1][0] = res[1];
    info->raw_dq_pi[2][0] = res[2];
    info->raw_dq_pi[3][0] = res[3];

    /* scan DQ PI map (negative) */
    mem_pll_disable();
    APPLY_DQ_PI(info->init_dq_pi);
    APPLY_DQ_RSN(info->dq_rsn);
    mem_pll_enable();
    K=NEG_SCAN_CHECK(DQ,  info->init_dq_pi);

    if(dbg) {
        DBG_PRINTF("DQ_PI(negative): init DQ_PI=%d\n", K);
        DBG_DUMP_RSN(); //DBG_DUMP_RSN_IN_REG();
    }

    res[0]=res[1]=res[2]=res[3]=0;
    for(idx=31; idx>=0; idx--) {
        mem_pll_disable();
        APPLY_DQ_PI(K);
        mem_pll_enable();

        err=ocp_memory_ordeal_64(ORDEAL_ADDR, ORDEAL_LEN);// memory_ordeal();
        res[0]|=(0==(err&0xFF)?1:0)<<idx;
        res[1]|=(0==((err>>8)&0xFF)?1:0)<<idx;
        res[2]|=(0==((err>>16)&0xFF)?1:0)<<idx;
        res[3]|=(0==((err>>24)&0xFF)?1:0)<<idx;
        if(2==dbg) {DBG_PRINTF("[%d] K=%d, err=0x%x\n", idx, K, err);}

        if(idx>0) {
            K=NEG_SCAN_CHECK(DQ, K);
        }
    }
    info->raw_dq_pi[0][1] = res[0];
    info->raw_dq_pi[1][1] = res[1];
    info->raw_dq_pi[2][1] = res[2];
    info->raw_dq_pi[3][1] = res[3];

    for(idx=0, pi_mv=0; idx<(2<<info->dq32); idx++) {
        if(_find_longest_range(info->raw_dq_pi[idx][1], info->raw_dq_pi[idx][0], &upper, &lower))  {
            ERR_PRINTF("%s failed (res=0x%08x%08x)\n", __FUNCTION__, info->raw_dq_pi[idx][0], info->raw_dq_pi[idx][1]);
            return 1;
        }
        if(dbg) { _dbg_print_hex(info->raw_dq_pi[idx][1], info->raw_dq_pi[idx][0], info->init_dq_pi); }
        // check boundary
        upper = upper-32+info->init_dq_pi;
        lower = lower-32+info->init_dq_pi;
        info->res_dq_pi[idx] = lower+(upper-lower)/2;
        INDT_PRINTF("DQ_PI range:  (%3d) %3d~%3d \t==> DQ_PI_%d =%3d, dq_rsn=%d\n", info->init_dq_pi, lower, upper, idx, info->res_dq_pi[idx], info->dq_rsn);
        pi_mv += info->res_dq_pi[idx];
    }
    pi_mv = pi_mv >> (1<<info->dq32);

    for(idx=0; idx<(2<<info->dq32); idx++) {
        INDT_PRINTF("\t\t\t\t\t    DQ_PI_%d=%3d => ", idx, info->res_dq_pi[idx]);

        if(pi_mv<0) {
            if(info->res_dq_pi[idx]>=0) {
                info->res_dq_pi[idx] = 31;
            } else {
                info->res_dq_pi[idx] += 32;
            }
        } else if(pi_mv>31) {
            if(info->res_dq_pi[idx]<=31) {
                info->res_dq_pi[idx] = 0;
            } else {
                info->res_dq_pi[idx] -= 32;
            }
        } else {
            if(info->res_dq_pi[idx]<0) {
                info->res_dq_pi[idx] = 0;
            } else if(info->res_dq_pi[idx]>31) {
                info->res_dq_pi[idx] = 31;
            }
        }
        printf("%3d\n", info->res_dq_pi[idx]);
    }

    // check  rsn
    if(pi_mv<0) {
        info->dq_rsn--;
    } else if(pi_mv>31) {
        info->dq_rsn++;
    }

    // apply setting
    mem_pll_disable();
    APPLY_DQ_PI_0(info->res_dq_pi[0]);
    APPLY_DQ_PI_1(info->res_dq_pi[1]);
    APPLY_DQ_PI_2(info->res_dq_pi[2]);
    APPLY_DQ_PI_3(info->res_dq_pi[3]);
    APPLY_DQS_RSN(info->dqs_rsn);
    APPLY_DQ_RSN(info->dq_rsn);
    APPLY_CMD_RSN(info->cmd_rsn);
    APPLY_RX_RSN(info->rx_rsn);
    mem_pll_enable();
    if(dbg) {DBG_DUMP_RSN(); } //DBG_DUMP_RSN_IN_REG();}

    return 0;
}


#define CHECK_DQS_IN_DLY(r, f)      ({  u32_t __i, __r;   \
                                        u32_t t = RFLD_##r##_0(f);\
                                        for(__i=0, __r=0; __i<32; __i++) \
                                        {   APPLY_DQS_IN_DLY(r, f, __i); \
                                            SYNC_RD_DLY();   \
                                            if(0==memory_ordeal()) __r|=1<<__i; }\
                                            APPLY_DQS_IN_DLY(r, f, t); \
                                            SYNC_RD_DLY();   \
                                            __r; })

#define CHECK_DQ_DLY(r, f)          ({  u32_t __i, __r;   \
                                        for(__i=0, __r=0; __i<32; __i++) \
                                        {   APPLY_DQ_DLY(r, f, __i); \
                                            SYNC_RD_DLY();   \
                                            if(0==memory_ordeal()) __r|=1<<__i; }\
                                            APPLY_DQ_DLY(r, f, 0x0); \
                                            SYNC_RD_DLY();   \
                                            __r; })

#define CHECK_DM_DLY(f)             ({  u32_t __i, __r;   \
                                        for(__i=0, __r=0; __i<32; __i++) \
                                        {   RMOD_DQ_DLY_1(f, __i); \
                                            SYNC_RD_DLY();   \
                                            if(0==memory_ordeal()) __r|=1<<__i; }\
                                            APPLY_DM_DLY(f, 0x0); \
                                            SYNC_RD_DLY();   \
                                            __r; })

MEMCNTLR_SECTION
int _find_dq_result(u32_t raw_dq, u32_t rx, u8_t dbg)
{
    u32_t i;
    int s, e;
#if 0
    for(i=0, s=-1, e=-1; i<32; i++) {
        if(1==((raw_dq>>i)&0x1) && -1==s) s=i;
        else if(0==(raw_dq>>i) && -1!=s) { break; }
        else e=i;
    }

    if((-1==s)||(-1==e)) {
        ERR_PRINTF("[%s] upper %d, lower %d\n", __FUNCTION__, e, s);
        return 1;
    }
#else
    _find_upper_and_lower(raw_dq, &e, &s);
#endif
    if(dbg) { DBG_PRINTF("upper %d, lower %d\n", e, s);  }

    if(rx) {
        i = (s+e)/2;
    } else {
        i = s;
    }
    //DBG_PRINTF("setting as %d\n", i);
    return i;
}

MEMCNTLR_SECTION
u32_t do_rx_dq_delay_tap_scan(PI_CALI_T *info, u8_t dbg)
{
    u32_t idx;

    // make sure turn off "delay follow dq0"
    RMOD_READ_CTRL_1(rd_dly_follow_dq0, 0x0);
    RMOD_DPI_CTRL_0(fw_set_mode, 0x2, cal_set_mode, 0x3);

    // RX delay for each DQ scan
    INFO_PRINTF("RX DQ delay scan ...\n");
    info->raw_dqs_in_pos[0] = CHECK_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_0);
    info->raw_dqs_in_pos[1] = CHECK_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_1);
    info->raw_dqs_in_pos[2] = CHECK_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_2);
    info->raw_dqs_in_pos[3] = CHECK_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_3);
    info->raw_dqs_in_pos[4] = CHECK_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_4);
    info->raw_dqs_in_pos[5] = CHECK_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_5);
    info->raw_dqs_in_pos[6] = CHECK_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_6);
    info->raw_dqs_in_pos[7] = CHECK_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_7);
    info->raw_dqs_in_neg[0] = CHECK_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_0);
    info->raw_dqs_in_neg[1] = CHECK_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_1);
    info->raw_dqs_in_neg[2] = CHECK_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_2);
    info->raw_dqs_in_neg[3] = CHECK_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_3);
    info->raw_dqs_in_neg[4] = CHECK_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_4);
    info->raw_dqs_in_neg[5] = CHECK_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_5);
    info->raw_dqs_in_neg[6] = CHECK_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_6);
    info->raw_dqs_in_neg[7] = CHECK_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_7);

    info->res_dqs_in_pos[0] = _find_dq_result(info->raw_dqs_in_pos[0], 1, dbg);
    info->res_dqs_in_pos[1] = _find_dq_result(info->raw_dqs_in_pos[1], 1, dbg);
    info->res_dqs_in_pos[2] = _find_dq_result(info->raw_dqs_in_pos[2], 1, dbg);
    info->res_dqs_in_pos[3] = _find_dq_result(info->raw_dqs_in_pos[3], 1, dbg);
    info->res_dqs_in_pos[4] = _find_dq_result(info->raw_dqs_in_pos[4], 1, dbg);
    info->res_dqs_in_pos[5] = _find_dq_result(info->raw_dqs_in_pos[5], 1, dbg);
    info->res_dqs_in_pos[6] = _find_dq_result(info->raw_dqs_in_pos[6], 1, dbg);
    info->res_dqs_in_pos[7] = _find_dq_result(info->raw_dqs_in_pos[7], 1, dbg);
    info->res_dqs_in_neg[0] = _find_dq_result(info->raw_dqs_in_neg[0], 1, dbg);
    info->res_dqs_in_neg[1] = _find_dq_result(info->raw_dqs_in_neg[1], 1, dbg);
    info->res_dqs_in_neg[2] = _find_dq_result(info->raw_dqs_in_neg[2], 1, dbg);
    info->res_dqs_in_neg[3] = _find_dq_result(info->raw_dqs_in_neg[3], 1, dbg);
    info->res_dqs_in_neg[4] = _find_dq_result(info->raw_dqs_in_neg[4], 1, dbg);
    info->res_dqs_in_neg[5] = _find_dq_result(info->raw_dqs_in_neg[5], 1, dbg);
    info->res_dqs_in_neg[6] = _find_dq_result(info->raw_dqs_in_neg[6], 1, dbg);
    info->res_dqs_in_neg[7] = _find_dq_result(info->raw_dqs_in_neg[7], 1, dbg);

    APPLY_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_0, info->res_dqs_in_pos[0]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_1, info->res_dqs_in_pos[1]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_2, info->res_dqs_in_pos[2]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_3, info->res_dqs_in_pos[3]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_4, info->res_dqs_in_pos[4]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_5, info->res_dqs_in_pos[5]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_6, info->res_dqs_in_pos[6]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_7, info->res_dqs_in_pos[7]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_0, info->res_dqs_in_neg[0]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_1, info->res_dqs_in_neg[1]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_2, info->res_dqs_in_neg[2]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_3, info->res_dqs_in_neg[3]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_4, info->res_dqs_in_neg[4]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_5, info->res_dqs_in_neg[5]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_6, info->res_dqs_in_neg[6]);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_7, info->res_dqs_in_neg[7]);

    for(idx=0; idx<8; idx++){
        INDT_PRINTF("POS_DQ%d_0~3=%02d\tNEG_DQ%d_0~3=%02d\n",
                    idx, info->res_dqs_in_pos[idx], idx, info->res_dqs_in_neg[idx]);
        if(dbg) { DBG_PRINTF("RAW: POS_DQ%d=0x%08x\tNEG_DQ%d=0x%08x\n", idx, info->raw_dqs_in_pos[idx], idx, info->raw_dqs_in_neg[idx]); }
    }
    return 0;
}

MEMCNTLR_SECTION
u32_t do_tx_dq_delay_tap_scan(PI_CALI_T *info, u8_t dbg)
{
    u32_t idx;
    // scan TX delay tap
    INFO_PRINTF("TX DQ delay scan ... \n");
    //RMOD_DPI_CTRL_0(fw_set_mode, 0x2, cal_set_mode, 0x3);
    RMOD_DQ_DLY_1(dqs_dly_sel_0, 0x0, dqs_dly_sel_1, 0x0, dqs_dly_sel_2, 0x0, dqs_dly_sel_3, 0x0);
    RMOD_DQ_DLY_2(dqs_oe_dly_sel_0, 0x0, dqs_oe_dly_sel_1, 0x0, dqs_oe_dly_sel_2, 0x0, dqs_oe_dly_sel_3, 0x0);

    info->raw_dq_dly[0] = CHECK_DQ_DLY(DQ_DLY_0, dq0_dly_sel);
    info->raw_dq_dly[1] = CHECK_DQ_DLY(DQ_DLY_0, dq1_dly_sel);
    info->raw_dq_dly[2] = CHECK_DQ_DLY(DQ_DLY_0, dq2_dly_sel);
    info->raw_dq_dly[3] = CHECK_DQ_DLY(DQ_DLY_0, dq3_dly_sel);
    info->raw_dq_dly[4] = CHECK_DQ_DLY(DQ_DLY_0, dq4_dly_sel);
    info->raw_dq_dly[5] = CHECK_DQ_DLY(DQ_DLY_0, dq5_dly_sel);
    info->raw_dq_dly[6] = CHECK_DQ_DLY(DQ_DLY_0, dq6_dly_sel);
    info->raw_dq_dly[7] = CHECK_DQ_DLY(DQ_DLY_0, dq7_dly_sel);

    info->res_dq_dly[0] = _find_dq_result(info->raw_dq_dly[0], 0, dbg);
    info->res_dq_dly[1] = _find_dq_result(info->raw_dq_dly[1], 0, dbg);
    info->res_dq_dly[2] = _find_dq_result(info->raw_dq_dly[2], 0, dbg);
    info->res_dq_dly[3] = _find_dq_result(info->raw_dq_dly[3], 0, dbg);
    info->res_dq_dly[4] = _find_dq_result(info->raw_dq_dly[4], 0, dbg);
    info->res_dq_dly[5] = _find_dq_result(info->raw_dq_dly[5], 0, dbg);
    info->res_dq_dly[6] = _find_dq_result(info->raw_dq_dly[6], 0, dbg);
    info->res_dq_dly[7] = _find_dq_result(info->raw_dq_dly[7], 0, dbg);

    APPLY_DQ_DLY(DQ_DLY_0, dq0_dly_sel, info->res_dq_dly[0]);
    APPLY_DQ_DLY(DQ_DLY_0, dq1_dly_sel, info->res_dq_dly[1]);
    APPLY_DQ_DLY(DQ_DLY_0, dq2_dly_sel, info->res_dq_dly[2]);
    APPLY_DQ_DLY(DQ_DLY_0, dq3_dly_sel, info->res_dq_dly[3]);
    APPLY_DQ_DLY(DQ_DLY_0, dq4_dly_sel, info->res_dq_dly[4]);
    APPLY_DQ_DLY(DQ_DLY_0, dq5_dly_sel, info->res_dq_dly[5]);
    APPLY_DQ_DLY(DQ_DLY_0, dq6_dly_sel, info->res_dq_dly[6]);
    APPLY_DQ_DLY(DQ_DLY_0, dq7_dly_sel, info->res_dq_dly[7]);

    for(idx=0; idx<8; idx++){
        INDT_PRINTF("DQ%d_0~3=%02d\n", idx, info->res_dq_dly[idx]);
        if(dbg) { DBG_PRINTF("RAW: DQ%d=0x%08x\n", idx, info->raw_dq_dly[idx]); }
    }

    INFO_PRINTF("DM delay scan ...\n");
    info->raw_dm_dly[0] = CHECK_DM_DLY(dm_dly_sel_0);
    info->raw_dm_dly[1] = CHECK_DM_DLY(dm_dly_sel_1);
    info->raw_dm_dly[2] = CHECK_DM_DLY(dm_dly_sel_2);
    info->raw_dm_dly[3] = CHECK_DM_DLY(dm_dly_sel_3);

    info->res_dm_dly[0] = _find_dq_result(info->raw_dm_dly[0], 0, dbg);
    info->res_dm_dly[1] = _find_dq_result(info->raw_dm_dly[1], 0, dbg);
    info->res_dm_dly[2] = _find_dq_result(info->raw_dm_dly[2], 0, dbg);
    info->res_dm_dly[3] = _find_dq_result(info->raw_dm_dly[3], 0, dbg);

    APPLY_DM_DLY(dm_dly_sel_0, info->res_dm_dly[0]);
    APPLY_DM_DLY(dm_dly_sel_1, info->res_dm_dly[1]);
    APPLY_DM_DLY(dm_dly_sel_2, info->res_dm_dly[2]);
    APPLY_DM_DLY(dm_dly_sel_3, info->res_dm_dly[3]);

    for(idx=0; idx<4; idx++){
        INDT_PRINTF("DM%d=%02d\n", idx, info->res_dm_dly[idx]);
        if(dbg) { DBG_PRINTF("RAW: DM%02d=0x%08x\n", idx, info->raw_dm_dly[idx]); }
    }

    // restore cal_set_mode
    RMOD_DPI_CTRL_0(fw_set_mode, 0, cal_set_mode, 0);
    return 0;
}

MEMCNTLR_SECTION
u32_t do_dqs_en_scan(PI_CALI_T *info, u8_t dbg)
{
    //ENTER_FORCE_X16_MODE

    INFO_PRINTF("DQS_EN scan ... \n");
    RMOD_DPI_CTRL_0(dqsen_mode, 0x0, fw_set_mode, 0x2);
    // RX ODT always on
    RX_ODT_ALWAYS_ON(1);

    u32_t dqs_psel_0 = DQS_P_ODT_SEL_0rv;
    u32_t dqs_psel_1 = DQS_P_ODT_SEL_1rv;
    u32_t dqs_nsel_0 = DQS_N_ODT_SEL_0rv;
    u32_t dqs_nsel_1 = DQS_N_ODT_SEL_1rv;

    // unbalance DQS ODT setting, reverse P and N
    DQS_P_ODT_SEL_0rv = dqs_nsel_0;
    DQS_P_ODT_SEL_1rv = dqs_nsel_1;
    DQS_N_ODT_SEL_0rv = dqs_psel_0;
    DQS_N_ODT_SEL_1rv = dqs_psel_1;
    SYNC_WR_DLY();

    u32_t i, j;
    u32_t reg __attribute__((unused));
    u32_t de, res;
    u32_t dqs_en_mask = info->dq32?0xF:0x3;
    for(i=0, de=0; i<=0x7F; i++) {
        READ_CTRL_0_0rv=i;
        READ_CTRL_0_1rv=i;
        READ_CTRL_0_2rv=i;
        READ_CTRL_0_3rv=i;
        udelay(10);
        SYNC_RD_DLY();


        for(j=0, res=0; j<10; j++) {
            //clear
            INT_CTRLrv=0x30;
            INT_STATUS_0rv=0x0;
            udelay(10);

            // check dqs_init, if 0
            reg=REG32(0xA0842100+j*0x20);
            udelay(10);
            res|=INT_STATUS_0rv;
            //DBG_PRINTF("INT_STATUS_0rv=0x%x, res=0x%x\n", INT_STATUS_0rv, res);
        }

        if(2==dbg) { DBG_PRINTF("i=%d, res=0x%x\n", i, res); }
        if(0==(res&dqs_en_mask)) { de=i; break;}
    }
    // apply
    if(DDR_TYPE_DDR4==get_ddr_type() && de>=4) de=3; // from test, FIXME if root cause is figured out
    info->res_dqs_en=de;
    INDT_PRINTF("DQS_EN=%d\n", info->res_dqs_en);
    READ_CTRL_0_0rv=info->res_dqs_en;
    READ_CTRL_0_1rv=info->res_dqs_en;
    READ_CTRL_0_2rv=info->res_dqs_en;
    READ_CTRL_0_3rv=info->res_dqs_en;
    SYNC_RD_DLY();

    // restore unbalance DQS ODT setting
    DQS_P_ODT_SEL_0rv = dqs_psel_0;
    DQS_P_ODT_SEL_1rv = dqs_psel_1;
    DQS_N_ODT_SEL_0rv = dqs_nsel_0;
    DQS_N_ODT_SEL_1rv = dqs_nsel_1;
    SYNC_WR_DLY();

// disable RX ODT always on
    RX_ODT_ALWAYS_ON(0);
    RMOD_DPI_CTRL_0(fw_set_mode, 0x0);

    //EXIT_FORCE_X16_MODE
    return 0;
}
MEMCNTLR_SECTION
u32_t do_rx_fifo_map_scan(PI_CALI_T *info, u8_t dbg)
{
    u32_t j, res;
    int upper, lower;
    RMOD_DPI_CTRL_0(fw_set_mode, 0x2);
    INFO_PRINTF("RX FIFO map scan ...\n");
    for(j=0, res=0; j<32; j++) {
        RMOD_READ_CTRL_1(tm_rd_fifo, j);
        SYNC_RD_DLY();
        udelay(1000);
        if(0==memory_ordeal()) res|=1<<j;
    }
    info->raw_rd_fifo = res;

    if(0!=_find_upper_and_lower(res, &upper, &lower)) return 1;
    info->res_rd_fifo = lower+1;    // +1 for add some buffer
    if(dbg) {
        DBG_PRINTF("res=0x%08x, range is %02d-%02d-%02d\n",
                info->raw_rd_fifo, lower, info->res_rd_fifo, upper);
    }
    // apply
    RMOD_READ_CTRL_1(tm_rd_fifo, info->res_rd_fifo);
    SYNC_RD_DLY();
    udelay(1000);
    // force reset fifo
    RMOD_DPI_CTRL_0(rst_fifo_mode, 0x3);
    udelay(1000);
    RMOD_DPI_CTRL_0(rst_fifo_mode, 0x0);
    INDT_PRINTF("RX_FIFO=%02d\n", info->res_rd_fifo);

    RMOD_DPI_CTRL_0(fw_set_mode, 0);
    return 0;
}

MEMCNTLR_SECTION
u32_t do_rx_odt_map_scan(PI_CALI_T *info, u8_t dbg)
{
    u32_t j, res;

    RMOD_DPI_CTRL_0(fw_set_mode, 0x2);
    INFO_PRINTF("RX ODT map scan ...\n");

    // ODT NOT always on
    RX_ODT_ALWAYS_ON(0);
    APPLY_TM_ODT_EN(0xFFFFFFFF);
    u32_t tmp, tm_odt_en;
#if 0   // 0.5T is too small, 2T might be OK
    u32_t rc6 = READ_CTRL_6_0rv;
    u32_t rc2 = READ_CTRL_2_0rv;
    INDT_PRINTF("Shrink the head...\n");
    for(j=0, res=0; j<31; j++) {
        tmp = (0xFFFFFFFF<<j)&0x7FFFFFFF;
        tm_odt_en = tmp|(0x80000000);
        // step a
        APPLY_TM_ODT_EN_OSD(0x0);
        APPLY_TM_ODT_EN(tm_odt_en);
        udelay(1000);
        res=memory_ordeal();
        DBG_PRINTF("[%02d] Step A: tm_odt_en=0x%08x, res=0x%08x\n", j, tm_odt_en, res);

        // step b
        APPLY_TM_ODT_EN_OSD(0x2);
        APPLY_TM_ODT_EN(tm_odt_en);
        udelay(1000);
        res=memory_ordeal();
        DBG_PRINTF("[%02d] Step B: tm_odt_en=0x%08x, res=0x%08x\n", j, tm_odt_en, res);

        // step c
        tm_odt_en = tmp;
        APPLY_TM_ODT_EN_OSD(0x9);
        APPLY_TM_ODT_EN(tm_odt_en);
        udelay(1000);
        res=memory_ordeal();
        DBG_PRINTF("[%02d] Step C: tm_odt_en=0x%08x, res=0x%08x\n", j, tm_odt_en, res);

         // step d
        APPLY_TM_ODT_EN_OSD(0xB);
        APPLY_TM_ODT_EN(tm_odt_en);
        udelay(1000);
        res=memory_ordeal();
        DBG_PRINTF("[%02d] Step D: tm_odt_en=0x%08x, res=0x%08x\n", j, tm_odt_en, res);
    }

    tmp = 0xFFFFFFFF;
    INDT_PRINTF("Shrink the tail...\n");
    for(j=0, res=0; j<31; j++) {
        // step e
        tm_odt_en = tmp>>j;
        APPLY_TM_ODT_EN_OSD(0x0);
        APPLY_TM_ODT_EN(tm_odt_en);
        udelay(1000);
        res=memory_ordeal();
        DBG_PRINTF("[%02d] Step E: tm_odt_en=0x%08x, res=0x%08x\n", j, tm_odt_en, res);

        // step f
        tm_odt_en = tmp>>(j+1);
        APPLY_TM_ODT_EN_OSD(0xC);
        APPLY_TM_ODT_EN(tm_odt_en);
        udelay(1000);
        res=memory_ordeal();
        DBG_PRINTF("[%02d] Step F: tm_odt_en=0x%08x, res=0x%08x\n", j, tm_odt_en, res);

        // step g
        APPLY_TM_ODT_EN_OSD(0x8);
        APPLY_TM_ODT_EN(tm_odt_en);
        udelay(1000);
        res=memory_ordeal();
        DBG_PRINTF("[%02d] Step G: tm_odt_en=0x%08x, res=0x%08x\n", j, tm_odt_en, res);

         // step h
        APPLY_TM_ODT_EN_OSD(0x4);
        APPLY_TM_ODT_EN(tm_odt_en);
        udelay(1000);
        res=memory_ordeal();
        DBG_PRINTF("[%02d] Step H: tm_odt_en=0x%08x, res=0x%08x\n", j, tm_odt_en, res);
    }
    // restore
    READ_CTRL_2_0rv = rc2;
    READ_CTRL_2_1rv = rc2;
    READ_CTRL_2_2rv = rc2;
    READ_CTRL_2_3rv = rc2;
    READ_CTRL_6_0rv = rc6;
    READ_CTRL_6_1rv = rc6;
    READ_CTRL_6_2rv = rc6;
    READ_CTRL_6_3rv = rc6;

#else // 2T
    int hu, hl, tu, tl;
    u32_t err;
    #define FIFO_MAP_SIZE  (32)
    // shrink from head
    for(j=0, res=0; j<FIFO_MAP_SIZE; j++) {
        if(7==j || 4==j) continue;  // DEADZONE!
        tmp = (0xFFFFFFFF<<j)&0x7FFFFFFF;
        tm_odt_en = (0xFFFFFFFF<<j);//tmp|(0x80000000);
        // step a
        APPLY_TM_ODT_EN_OSD(0x0);
        APPLY_TM_ODT_EN(tm_odt_en);

        // force reset fifo
        udelay(1000);
        bp_barrier();

        if(0==(err=memory_ordeal())) {res|=(1<<j);}
        if(2==dbg) { DBG_PRINTF("[%02d] Step A: tm_odt_en=0x%08x, err=0x%x\n", j, tm_odt_en, err); }
        //RX_ODT_ALWAYS_ON(1); err=memory_ordeal(); printf("DD: [%2d] err=0x%x\n", j, err);RX_ODT_ALWAYS_ON(0);
    }
    if(0!=_find_upper_and_lower(res, &hu, &hl)) return 1;
    //info->res_rd_odt_lo=u;
    if(dbg) {
        DBG_PRINTF("Shrink the head... res=0x%08x, boundary is %d-%d\n", res, hl, hu);
    }
    // shrink from tail
    tmp = 0xFFFFFFFF;
    for(j=(32-FIFO_MAP_SIZE), res=0; j<32; j++) {
        // step e
        tm_odt_en = tmp>>j;
        APPLY_TM_ODT_EN_OSD(0x0);
        APPLY_TM_ODT_EN(tm_odt_en);

        udelay(1000);
        bp_barrier();

        if(0==(err=memory_ordeal())) {res|=(1<<(31-j));}
        if(2==dbg) { DBG_PRINTF("[%02d] Step E: tm_odt_en=0x%08x, err=0x%x\n", 31-j, tm_odt_en, err); }
        //RX_ODT_ALWAYS_ON(1); err=memory_ordeal(); printf("DD: [%2d] err=0x%x\n", j, err);RX_ODT_ALWAYS_ON(0);
    }
    if(0!=_find_upper_and_lower(res, &tu, &tl)) { return 1; }
    if(dbg) {
        DBG_PRINTF("Shrink the tail... res=0x%08x, boundary is %d-%d\n", res, tl, tu);
    }

    #define ODT_EN_LEN  (4)
    info->res_rd_odt_hi = (tl<(31-ODT_EN_LEN))?(tl+ODT_EN_LEN):tl;
    info->res_rd_odt_lo = (hu>ODT_EN_LEN)?(hu-ODT_EN_LEN):0;

    // prepare the setting
    for(j=0, res=0; j<32; j++) {
        if((j>=info->res_rd_odt_lo) && (j<=info->res_rd_odt_hi)) {
            res|=1<<j;
        }
    }
    info->res_rd_odt_set=res;
    //APPLY_TM_ODT_EN_OSD(0x0);
    APPLY_TM_ODT_EN(info->res_rd_odt_set);
    bp_barrier();
    INDT_PRINTF("translated range is %02d-%02d, tm_odt_en = 0x%08x\n", info->res_rd_odt_lo, info->res_rd_odt_hi, info->res_rd_odt_set);
#endif
    // restore cal_set_mode and force reset fifo
    RMOD_DPI_CTRL_0(fw_set_mode, 0, cal_set_mode, 0, rst_fifo_mode, 0x3);
    udelay(1000);
    RMOD_DPI_CTRL_0(rst_fifo_mode, 0x0);
    udelay(1000);

    return 0;
}

#define APPLY_DDR_VREF(vref_s, vref_r)  ({  DDR4_MR6_T mr6 = { .v = MR6rv }; \
                                            mr6.f.vref_t_en = 1; \
                                            mr6.f.vref_t_range = vref_r; \
                                            mr6.f.vref_t_val = vref_s; \
                                            MR6rv = mr6.v; \
                                            udelay(1000);\
                                            mc_rxi310_init(1); })
#define APPLY_DPI_VREF(vref_s, vref_r)  ({  RMOD_PAD_BUS_2(vref_s_0, vref_s, vref_range_0, vref_r,\
                                                           vref_s_1, vref_s, vref_range_1, vref_r,\
                                                           vref_s_2, vref_s, vref_range_2, vref_r);\
                                            DPI_CTRL_1rv = 0x3;  \
                                            udelay(1000);\
                                            mc_rxi310_init(1); })

#define DDR_VREF_S_MAX  (50)
#define DDR_VREF_RANGE  (1)
#define DPI_VREF_S_MAX  (60)
#define DPI_VREF_RANGE  (1)

MEMCNTLR_SECTION
int do_vrefdq_tx_calibration(PI_CALI_T *info, u8_t dbg)
{
    int K, vref_s, idx;
    int _dq_pi = RETRV_DQ_PI();
    int _dq_rsn = RETRV_DQ_RSN();
    u32_t i;
    u32_t dq, res, cnt;
    RX_ODT_ALWAYS_ON(1);

    memset((char *)info->raw_vref_s_txk, 0, sizeof(info->raw_vref_s_txk));
    memset((char *)info->scr_vref_s_txk, 0, sizeof(info->scr_vref_s_txk));

    for(dq=0; dq<16; dq++) {
        INFO_PRINTF("DQ%02d", dq);
        printf("%c", dbg?'\n':'\r');
        for(vref_s=DDR_VREF_S_MAX, idx=0; vref_s>=0; vref_s-=4, idx++) {
            APPLY_DDR_VREF(vref_s, DDR_VREF_RANGE);
            if(dbg) { DBG_PRINTF("(%d) Vref_s = %02d:", DDR_VREF_RANGE, vref_s); }
            // modify dq PI
            for(i=0, K=0; i<32; i++) {
                mem_pll_disable();
                APPLY_DQ_PI(K);
                mem_pll_enable();
                info->raw_vref_s_txk[i]=fixp_memory_ordeal((i&0x1)?0x5A5AA5A5:0xA5A55A5A);

                if(i<31) {
                    K=POS_SCAN_CHECK(DQ, K);
                }
            }
            for(i=0, cnt=0; i<32; i++) {
                res = ((info->raw_vref_s_txk[i]>>dq)&0x1)|((info->raw_vref_s_txk[i]>>(dq+16))&0x1);   // FIXME, DQ16, DQ32
                if(0==res) { cnt++; }
                if(dbg) { printf(" %d", res); }
            }
            info->scr_vref_s_txk[idx]+=cnt;
            if(dbg) { printf(": %d\n", cnt); }
        }
    }

    int fl_s, fl_e;
    #define DQ_PI_FLOOR   (16*15)
    for(vref_s=DDR_VREF_S_MAX, fl_s=fl_e=-1, idx=0; vref_s>=0; vref_s-=4, idx++)
    {
        if(dbg) {
            DBG_PRINTF("(Vref_r=%d)Vref_s=%02d: %d(%c)\n", DDR_VREF_RANGE, vref_s, info->scr_vref_s_txk[idx], (info->scr_vref_s_txk[idx]>=DQ_PI_FLOOR)?'o':'x');
        }
        if(info->scr_vref_s_txk[idx]>=DQ_PI_FLOOR){
            if(-1==fl_s) {
                fl_s = vref_s;
            } else if(-1!=fl_s) {
                fl_e = vref_s;
            }
        }
    }
    if(-1==fl_s || -1==fl_e)
    {
        WARN_PRINTF("can't find vref_s\n");
        return -1;
    } else {
        vref_s = (fl_s+fl_e)/2;
        u32_t v = (vref_s*65)+((DDR_VREF_RANGE)?4500:6000);
        info->res_vref_s_txk=vref_s;
        info->res_vref_r_txk=DPI_VREF_RANGE;
        APPLY_DDR_VREF(info->res_vref_s_txk, info->res_vref_r_txk);
        INFO_PRINTF("DDR VREF_S: %2d~%2d => %2d (%02d.%02d)\%, VREF_R=%02d\n", fl_s, fl_e, info->res_vref_s_txk, v/100, v%100, info->res_vref_r_txk);
    }

    mem_pll_disable();
    APPLY_DQ_PI(_dq_pi);
    APPLY_DQ_RSN(_dq_rsn);
    mem_pll_enable();
    RX_ODT_ALWAYS_ON(0);

    return 0;
}


MEMCNTLR_SECTION
int do_vrefdq_rx_calibration(PI_CALI_T *info, u8_t dbg)
{
    u32_t dq, _t, cnt, res;
#if 0
    u32_t _dq, base, set, loc, t, off;
    volatile u32_t *r;
#endif
    u32_t i, idx;
    int vref_s;

    memset((char *)info->raw_vref_s_rxk, 0, sizeof(info->raw_vref_s_rxk));
    memset((char *)info->scr_vref_s_rxk, 0, sizeof(info->scr_vref_s_rxk));

    for(dq=0; dq<16; dq++) {
#if 0
        _dq = dq;
        set = _dq/8;     // 0~7, 8~15, 16~23, 24~31
        loc = (_dq >> 2)&0x1;    // 0~3 or 4~7
        _dq = _dq & 0x3;
        off = (8*_dq);
        base = DQS_IN_DLY_0_0ar + set*0x4 + loc*0x10;
        r = (volatile u32_t *)base;
        t = *r;
        printf("DQ %d => addr 0x%x = 0x%08x (set %d, loc %d), off %d (0x%2x)\n", dq, base, t, set, loc, _dq, (t>>off)&0x1F);
#endif
        INFO_PRINTF("DQ%02d", dq);
        printf("%c", dbg?'\n':'\r');
        for(vref_s=DPI_VREF_S_MAX, idx=0; vref_s>=0; vref_s-=4, idx++) {
            if(dbg) { DBG_PRINTF("(%d) Vref_s = %02d:", DDR_VREF_RANGE, vref_s); }
            //printf("%c", dbg?'\n':'\r');
            // apply Vref_DQ
            APPLY_DPI_VREF(vref_s, DPI_VREF_RANGE);

            for(i=0; i<32; i++) {
                //fixed_pattern_gen(pat_ary, (i&0x1)?pat[0]:pat[1]);
                #if 1
                _t = i | (i<<8) | (i<<16) | (i<<24);
                DQS_IN_DLY_0_0rv = _t;
                DQS_IN_DLY_0_1rv = _t;
                DQS_IN_DLY_0_2rv = _t;
                DQS_IN_DLY_0_3rv = _t;
                DQS_IN_DLY_1_0rv = _t;
                DQS_IN_DLY_1_1rv = _t;
                DQS_IN_DLY_1_2rv = _t;
                DQS_IN_DLY_1_3rv = _t;
                DQS_IN_DLY_2_0rv = _t;
                DQS_IN_DLY_2_1rv = _t;
                DQS_IN_DLY_2_2rv = _t;
                DQS_IN_DLY_2_3rv = _t;
                DQS_IN_DLY_3_0rv = _t;
                DQS_IN_DLY_3_1rv = _t;
                DQS_IN_DLY_3_2rv = _t;
                DQS_IN_DLY_3_3rv = _t;
                #else
                _t = (t&~(0x1F<<off))|(i<<off);
                *r = _t;
                #endif
                SYNC_RD_DLY();

                prb7_pattern_gen(pat_ary);
                info->raw_vref_s_rxk[i]=memory_ordeal();//fixp_memory_ordeal((i&0x1)?0x5A5A5A5A:0xA5A5A5A5);
                //printf("[%d]=0x%08x\n", i, info->raw_vref_s_rxk[i]);
            }

            for(i=0, cnt=0; i<32; i++) {
                res=(info->raw_vref_s_rxk[i]>>dq)&0x1;
                if(0==res) { cnt++; }
                if(dbg) { printf(" %d", res); }
            }
            info->scr_vref_s_rxk[idx]+=cnt;
            if(dbg) { printf(": %d\n", cnt); }

#if 0
            // apply original setting
            *r=t;
            SYNC_RD_DLY();
#endif
        }
    }
    // RMOD_DPI_CTRL_0(fw_set_mode, 0, cal_set_mode, 0);

    DQS_IN_DLY_0_0rv = 0x0B0B0B0B;
    DQS_IN_DLY_0_1rv = 0x0B0B0B0B;
    DQS_IN_DLY_0_2rv = 0x0B0B0B0B;
    DQS_IN_DLY_0_3rv = 0x0B0B0B0B;
    DQS_IN_DLY_1_0rv = 0x0B0B0B0B;
    DQS_IN_DLY_1_1rv = 0x0B0B0B0B;
    DQS_IN_DLY_1_2rv = 0x0B0B0B0B;
    DQS_IN_DLY_1_3rv = 0x0B0B0B0B;
    DQS_IN_DLY_2_0rv = 0x0B0B0B0B;
    DQS_IN_DLY_2_1rv = 0x0B0B0B0B;
    DQS_IN_DLY_2_2rv = 0x0B0B0B0B;
    DQS_IN_DLY_2_3rv = 0x0B0B0B0B;
    DQS_IN_DLY_3_0rv = 0x0B0B0B0B;
    DQS_IN_DLY_3_1rv = 0x0B0B0B0B;
    DQS_IN_DLY_3_2rv = 0x0B0B0B0B;
    DQS_IN_DLY_3_3rv = 0x0B0B0B0B;
    SYNC_RD_DLY();

    int fl_s, fl_e;
    #define RXD_FLOOR   (16*24)
    for(vref_s=DPI_VREF_S_MAX, fl_s=fl_e=-1, idx=0; vref_s>=0; vref_s-=4, idx++)
    {
        if(dbg) {
            DBG_PRINTF("(Vref_r=%d)Vref_s=%02d: %3d(%c)\n", DPI_VREF_RANGE, vref_s, info->scr_vref_s_rxk[idx], (info->scr_vref_s_rxk[idx]>=RXD_FLOOR)?'o':'x');
        }
        if(info->scr_vref_s_rxk[idx]>=RXD_FLOOR){
            if(-1==fl_s) {
                fl_s = vref_s;
            } else if(-1!=fl_s) {
                fl_e = vref_s;
            }
        }
    }
    if(-1==fl_s || -1==fl_e)
    {
        WARN_PRINTF("can't find vref_s\n");
        return -1;
    } else {
        vref_s = (fl_s+fl_e)/2;
        u32_t v = (DPI_VREF_RANGE)?(1000+vref_s*100):(6000+vref_s*50);
        info->res_vref_s_rxk=vref_s;
        info->res_vref_r_rxk=DPI_VREF_RANGE;
        APPLY_DPI_VREF(info->res_vref_s_rxk, info->res_vref_r_rxk);
        INFO_PRINTF("DPI VREF_S: %2d~%2d => %2d (%02d.%02d)\%, VREF_R=%02d\n", fl_s, fl_e, info->res_vref_s_rxk, v/100, v%100, info->res_vref_r_rxk);
    }
    return 0;
}

MEMCNTLR_SECTION
u32_t dpi_vrefdq_calibration(PI_CALI_T *info, u8_t dbg)
{
    if(DDR_TYPE_DDR3==meminfo.cntlr_opt->dram_type) { return 0; }

    // using x16 for calibration
    ENTER_FORCE_X16_MODE

    u32_t r;
    RMOD_DPI_CTRL_0(fw_set_mode, 0x2, cal_set_mode, 0x3);

    r = do_vrefdq_tx_calibration(info, dbg);
    if(r!=0) {
        WARN_PRINTF("VREF_S TX calibration failed!\n");
    }
    r = do_vrefdq_rx_calibration(info, dbg);
    if(r!=0) {
        WARN_PRINTF("VREF_S RX calibration failed!\n");
    }

    RMOD_DPI_CTRL_0(fw_set_mode, 0, cal_set_mode, 0);

    EXIT_FORCE_X16_MODE
    return 0;
}

MEMCNTLR_SECTION
u32_t dpi_pi_scan(PI_CALI_T *info, u8_t dbg) {
    u32_t retry=0, res;
    /* setting init value */
    // CS using cmd_ex_rd_str_num
    RMOD_AFIFO_STR_SEL(cmd_rd_str_num_sel_pad_cs_n_1, 1, cmd_rd_str_num_sel_pad_cs_n, 1);
    // RX ODT always on
    RX_ODT_ALWAYS_ON(1);
    RMOD_DPI_CTRL_0(fw_set_mode, 0x2, cal_set_mode, 0x3);

wr_retry:
    // do write leveling first
    res = do_write_leveling(info, dbg);
    if(res) {
        if(retry++>3) goto exit;
        else goto wr_retry;
    }

    retry=0;
pi_retry:
    info->init_dqs_pi   = 0;
    info->init_dck_pi   = 0;   //info->init_dqs_pi - info->res_dqss[0];
    info->init_cs_pi    = info->init_dck_pi;
    info->init_dq_pi    = info->init_dqs_pi + DQS_DQ_OFF;
    info->dqs_rsn       = 3;    //RFLD_AFIFO_STR_0(dqs_rd_str_num_0);
    info->dq_rsn        = 3;    //RFLD_AFIFO_STR_0(dq_rd_str_num_0);
    info->cs_rsn        = 2;    //RFLD_AFIFO_STR_1(cmd_ex_rd_str_num);
    info->cmd_rsn       = 2;    //RFLD_AFIFO_STR_1(cmd_rd_str_num);
    info->rx_rsn        = 2;    //RFLD_AFIFO_STR_2(rx_rd_str_num_0);
    APPLY_DCK_PI(info->init_dck_pi);
    APPLY_DQS_PI(info->init_dqs_pi);
    APPLY_DQ_PI(info->init_dq_pi);
    APPLY_CS_PI(info->init_cs_pi);

    mem_pll_disable();
    APPLY_DQS_RSN(info->dqs_rsn);
    APPLY_DQ_RSN(info->dq_rsn);
    APPLY_CS_RSN(info->cs_rsn);
    APPLY_CMD_RSN(info->cmd_rsn);
    APPLY_RX_RSN(info->rx_rsn);
    mem_pll_enable();


    INFO_PRINTF("PI scan ... \n");
    if( do_dck_pi_scan(info, dbg)
        || do_cs_pi_scan(info, dbg)
        || do_dqs_pi_scan(info, dbg)
        || do_dq_pi_scan(info, dbg)) {
        if(retry++>3) { res=1; goto exit;}
        else { WARN_PRINTF("PI scan Retry %d\n", retry); goto pi_retry; }
    }

    // restore afifo related setting
    info->res_dqs_rd_str_num    = info->dqs_rsn;
    info->res_dq_rd_str_num     = info->dq_rsn;
    info->res_cmd_ex_rd_str_num = info->cs_rsn;
    info->res_cmd_rd_str_num    = info->cmd_rsn;
    info->res_rx_rd_str_num     = info->rx_rsn;

exit:
    // disable RX ODT always on
    RX_ODT_ALWAYS_ON(0);
    RMOD_DPI_CTRL_0(fw_set_mode, 0, cal_set_mode, 0);

    return res;
}

MEMCNTLR_SECTION
u32_t dpi_misc_scan(PI_CALI_T *info, u8_t dbg)
{
    u32_t retry=0;
    // check DQS_EN
    do_dqs_en_scan(info, dbg);

    // check RX DQ delay tap
    do_rx_dq_delay_tap_scan(info, dbg);

    // check TX DQ delay tap
    do_tx_dq_delay_tap_scan(info, dbg);

    // using x16 for calibration
    ENTER_FORCE_X16_MODE
    // check RX FIFO map
    do_rx_fifo_map_scan(info, dbg);
    EXIT_FORCE_X16_MODE
    
    retry=0;
RX_ODT_RETRY:
    // check RX ODT map
    if(do_rx_odt_map_scan(info, dbg)) {
        if(retry++>5) { return 5; }
        else { goto RX_ODT_RETRY; }
    }

    
    return 0;
}

extern void mc_dram_size_detect(void);
MEMCNTLR_SECTION
u32_t dpi_calibration(u8_t dbg) {


    PI_CALI_T info;
    memset((char *)&info, 0, sizeof(PI_CALI_T));

    // fill test Info.
    info.dq32 = get_dq32_status();
    INFO_PRINTF("do full dram calibration\n");
    prb7_pattern_gen(pat_ary);
    if( 0!=dpi_vrefdq_calibration(&info, dbg) ||
        0!=dpi_pi_scan(&info, dbg) ||
        0!=dpi_misc_scan(&info, dbg)
    ) {
        ERR_PRINTF("dram calibration failed !\n");
        goto end;
    }

    /* show result */

    INFO_PRINTF("===============================Calibration Result===============================\n");
    INDT_PRINTF(".dck_post_pi       = %d,\n", info.res_dck_pi);
    INDT_PRINTF(".cs_post_pi        = %d,\n", info.res_cs_pi);
    INDT_PRINTF(".dqs_post_pi_0     = %d,\n", info.res_dqs_pi[0]);
    INDT_PRINTF(".dqs_post_pi_1     = %d,\n", info.res_dqs_pi[1]);
    INDT_PRINTF(".dqs_post_pi_2     = %d,\n", info.res_dqs_pi[2]);
    INDT_PRINTF(".dqs_post_pi_3     = %d,\n", info.res_dqs_pi[3]);
    INDT_PRINTF(".dq_post_pi_0      = %d,\n", info.res_dq_pi[0]);
    INDT_PRINTF(".dq_post_pi_1      = %d,\n", info.res_dq_pi[1]);
    INDT_PRINTF(".dq_post_pi_2      = %d,\n", info.res_dq_pi[2]);
    INDT_PRINTF(".dq_post_pi_3      = %d,\n", info.res_dq_pi[3]);
    INDT_PRINTF(".rd_fifo           = %d,\n", info.res_rd_fifo);
    INDT_PRINTF(".dqs_en            = %d,\n", info.res_dqs_en);
    INDT_PRINTF(".rd_odt            = %d,\n", info.res_rd_odt_set);
    INDT_PRINTF(".rd_odt_odd        = 0,\n");
    INDT_PRINTF(".dqs_rd_str_num    = %d,\n", info.res_dqs_rd_str_num);
    INDT_PRINTF(".dq_rd_str_num     = %d,\n", info.res_dq_rd_str_num);
    INDT_PRINTF(".cmd_ex_rd_str_num = %d,\n", info.res_cmd_ex_rd_str_num);
    INDT_PRINTF(".cmd_rd_str_num    = %d,\n", info.res_cmd_rd_str_num);
    INDT_PRINTF(".rx_rd_str_num     = %d,\n", info.res_rx_rd_str_num);

    u32_t i;
    for(i=0; i<8; i++) {
        INDT_PRINTF(".rd_dly_pos_dq%d    = %d,\n", i, info.res_dqs_in_pos[i]);
    }
    for(i=0; i<8; i++) {
        INDT_PRINTF(".rd_dly_neg_dq%d    = %d,\n", i, info.res_dqs_in_neg[i]);
    }
    for(i=0; i<8; i++) {
        INDT_PRINTF(".dq%d_dly           = %d,\n", i, info.res_dq_dly[i]);
    }
    for(i=0; i<4; i++) {
        INDT_PRINTF(".dm%d_dly           = %d,\n", i, info.res_dm_dly[i]);
    }

    INDT_PRINTF(".ddr_vref_s        = %d,\n", info.res_vref_s_txk);
    INDT_PRINTF(".ddr_vref_r        = %d,\n", info.res_vref_r_txk);
    INDT_PRINTF(".dpi_vref_s        = %d,\n", info.res_vref_s_rxk);
    INDT_PRINTF(".dpi_vref_r        = %d,\n", info.res_vref_r_txk);
end:
    udelay(10);
    mc_rxi310_init(1);
    return 0;
}
