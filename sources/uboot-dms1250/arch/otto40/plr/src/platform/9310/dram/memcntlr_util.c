#include <soc.h>
#include <dram/memcntlr.h>
#include <dram/memcntlr_ag.h>
#include <util.h>
#include <osc.h>

extern mc_info_t meminfo;

/* memsize is valid after size detect */
//MEMCNTLR_SECTION
unsigned int get_ddr_size(void)
{
    return otto_sc.memsize<<20;
}

MEMCNTLR_SECTION
u32_t get_ddr_type(void)
{
    return meminfo.cntlr_opt->dram_type;
}

MEMCNTLR_SECTION
u32_t get_dq32_status(void)
{
    /* v, 0:force x16, 1:force x32, 2:from strapped pin*/
    u32_t v = meminfo.cntlr_opt->dq32_en;
    if(2==v) {
        v = RFLD_PSR0(ddr_data_buswidth);
    }
    return v;
}

MEMCNTLR_SECTION
void dpi_set_ddr_type(void)
{
    u32_t dt = get_ddr_type();
    switch(dt) {
        case DDR_TYPE_DDR4:
            RMOD_CRT_CTL(pin_mux_lpddr3, 0, pin_mux_ddr3, 0, dpi_lpddr_en, 0, dpi_ddr2_en, 0, dpi_ddr3_mode, 0);
            DBG_PRINTF("DDR4\n");
            break;
        case DDR_TYPE_LPDDR3:
            RMOD_CRT_CTL(pin_mux_lpddr3, 1, pin_mux_ddr3, 0, dpi_lpddr_en, 1, dpi_ddr2_en, 0, dpi_ddr3_mode, 1);
            DBG_PRINTF("LPDDR3\n");
            break;
        default:
        case DDR_TYPE_DDR3:
            RMOD_CRT_CTL(pin_mux_lpddr3, 0, pin_mux_ddr3, 1, dpi_lpddr_en, 0, dpi_ddr2_en, 0, dpi_ddr3_mode, 1);
            DBG_PRINTF("DDR3\n");
            break;
    }
}

MEMCNTLR_SECTION
u32_t dpi_r480_calibration(void)
{
    puts("II: R480K     ... ");
    u32_t err = MEM_CNTLR_ZQ_R480_UNINIT;

    u8_t ddr_type = get_ddr_type();
    u8_t nocd2, en_dmyi, vref_s;

    switch(ddr_type) {
        case DDR_TYPE_DDR4:
            nocd2=1;
            en_dmyi=1;
            vref_s=0x34;    // 62%
        break;
        case DDR_TYPE_LPDDR3:
            nocd2=0;
            en_dmyi=0;
            vref_s=0x28;    // 50%
        break;
        default:
        case DDR_TYPE_DDR3:
            nocd2=0;
            en_dmyi=0;
            vref_s=0x28;    // 50%
        break;
    }

    RMOD_PAD_CTRL_PROG(dzq_up_sel, 0, zctrl_clk_sel, 0x2, rzq_ext_r240, 0, dzq_auto_up, 1);

    RMOD_ZQ_NOCD2(zq_ena_nocd2_0, nocd2);
    RMOD_PAD_BUS_1(pad_bus_1, 0);
    RMOD_PAD_BUS_0(en_dmyi, en_dmyi, vref_sel, 1, vref_range, 1, vref_s, vref_s);
    RMOD_ZQ_PAD_CTRL(zq_vref_sel_ini, 1, zq_vref_s_ini, vref_s);
    // for DDR3 board issue
    RMOD_PAD_BUS_2(vref_s_2, vref_s, vref_s_1, vref_s, vref_s_0, vref_s);
    RMOD_DPI_CTRL_0(fw_set_mode, 2);

    //RMOD_DPI_CTRL_1(write_en_0, 1, fw_set_wr_dly, 1);
    DPI_CTRL_1rv = 0x3;
    udelay(1000);

    RMOD_PAD_CTRL_PROG(rzq_cal_en, 0x1);
    u32_t cnt=0;
    while(0==RFLD_PAD_RZCTRL_STATUS(rzq_cal_done)) {
        if(++cnt>0x100000) {udelay(1); err=MEM_CNTLR_R480_TIMEOUT; break;}
    }

    if(0x1F==RFLD_PAD_RZCTRL_STATUS(rzq_480code)) {
        err=MEM_CNTLR_R480_ERROR;
    }

    printf("done (ERR:%d, RES:0x%x)\n", err, PAD_RZCTRL_STATUSrv);

    RMOD_PAD_CTRL_PROG(rzq_cal_en, 0x0);
    RMOD_DPI_CTRL_0(fw_set_mode, 0);

    return err;
}

typedef struct {
    u16_t pocd;
    u16_t zprog;
} ZPTable_t;

ZPTable_t zpt_ddr3[] = {
    {  34, 0x3B },
    {  37, 0x38 },
    {  40, 0x36 },
    {  60, 0x18 },
    { 120, 0x0A },
    { 150, 0x08 },
    { 240, 0x05 },
    {   0, 0x00 }   // END
};

ZPTable_t zpt_ddr4_cmd[] = {
    {  34, 0x4F },
    {  40, 0x36 },
    {  60, 0x28 },
    { 120, 0x0A },
    { 150, 0x08 },
    { 240, 0x05 },
    {   0, 0x00 }   // END
};

ZPTable_t zpt_ddr4_0p8[] = {
    {  10, 0x124 }, // pocd 10, nocd 40
    {  18, 0x77  }, // pocd 18, nocd 75
    {  30, 0x38  }, // pocd 30, nocd 120
    {  34, 0x48  }, // pocd 48, nocd 137
    {  40, 0x30  }, // pocd 40, nocd 160
    {  60, 0x24  }, // pocd 60, nocd 240
    {  73, 0x21  }, // pocd 73.8, nocd 295.4
    {   0, 0x00 }   // END
};

MEMCNTLR_SECTION
u32_t _ZP_X_ZPROG(u32_t z, u8_t type)
{
    u32_t i, t;
    ZPTable_t *zpt;
    if(ZP_D4_P5_CMD==type) {
        zpt = zpt_ddr4_cmd;
    } else if(ZP_D4_P8_DQ==type || ZP_D4_P8_PD3==type) {
        zpt = zpt_ddr4_0p8;
    } else {
        zpt = zpt_ddr3;
    }

    if(z<zpt[0].pocd) return zpt[0].zprog;

    i=t=0;
    while(0!=zpt[i].pocd) {
        if(z==zpt[i].pocd) {
            t=i;
            break;
        } else if(z>zpt[i].pocd) {  // find next
            t=i;
        }
        i++;
    }
    return zpt[t].zprog;
}

MEMCNTLR_SECTION
u32_t ZP_X_ZPROG(u32_t ocd_odt)
{
    u8_t ocd = ZP_GET_OCD(ocd_odt);
    u8_t odt = ZP_GET_ODT(ocd_odt);
    u8_t type = ZP_GET_TYPE(ocd_odt);
    u16_t zp_odt, zp_pocd, zp_nocd;
    if(ZP_D4_P8_DQ==type || ZP_D4_P8_PD3==type) {
        zp_pocd = _ZP_X_ZPROG(ocd, type);
        zp_nocd = _ZP_X_ZPROG(ocd/4, type);
        zp_odt  = _ZP_X_ZPROG(odt, type);
    } else {    // DDR3, LPDDR3
        zp_pocd = _ZP_X_ZPROG(ocd, type);
        zp_nocd = zp_pocd;
        zp_odt  = _ZP_X_ZPROG(odt*2, type);
    }
    u32_t zprog = (zp_odt<<18) | (zp_pocd<<9) | zp_nocd;
    return zprog;
}

#define Z2OCD(z)         (2400/(40*((6>(z>>6))?(z>>6):6)+24*(((z>>5)&0x1)+((z>>4)&0x1))+2*(z&0xF)))
#define Z2OCD_D4_P8(z)   (1920/(40*((6>(z>>6))?(z>>6):6)+24*(((z>>5)&0x1)+((z>>4)&0x1))+2*(z&0xF)))

MEMCNTLR_SECTION
u32_t ZP_X_OCD(u32_t zp, u8_t type)
{
    u32_t ocd;
    if(ZP_D4_P8_DQ==type || ZP_D4_P8_PD3==type) {
        ocd = Z2OCD_D4_P8(zp);
    } else {    // DDR3, LPDDR3
        ocd = Z2OCD(zp);
    }
    return ocd;
}


MEMCNTLR_SECTION
u32_t dpi_zq_calibration(u32_t ocd_odt[])
{
    u32_t err = MEM_CNTLR_ZQ_R480_UNINIT;

    u8_t idx=0, type, ddr_type = get_ddr_type();
    u8_t nocd2, en_dmyi, vref_s, vref_range;
    u32_t zprog;

    while(0!=ocd_odt[idx]) {
        type = ZP_GET_TYPE(ocd_odt[idx]);
        INFO_PRINTF("ZQK TYP_%d ... ", type);
        if(ZP_D4_P5_CMD==type) {   // DDR4 CMD
            nocd2=1;
            en_dmyi=1;
            vref_s=0x28;    // 62%
            vref_range=1;
        } else if(ZP_D4_P8_DQ==type || ZP_D4_P8_PD3==type) {
            nocd2=0;
            en_dmyi=0;
            vref_s=0x28;
            vref_range=0;
        } else {        // DDR3, LPDDR3, DDR4 DQ/DQS
            nocd2=0;
            en_dmyi=0;
            vref_s=0x28;    // 50%
            vref_range=1;
        }
        zprog = ZP_X_ZPROG(ocd_odt[idx]);
        RMOD_PAD_CTRL_PROG(dzq_up_sel, idx, zctrl_clk_sel, 0x2, rzq_ext_r240, 0, dzq_auto_up, 1);
        RMOD_ZQ_NOCD2(zq_ena_nocd2_0, nocd2);
        RMOD_PAD_BUS_1(pad_bus_1, 0);
        RMOD_PAD_BUS_0(en_dmyi, en_dmyi, vref_sel, 1, vref_range, vref_range, vref_s, vref_s);
        RMOD_ZQ_PAD_CTRL(zq_vref_sel_ini, 1, zq_vref_s_ini, vref_s);

        RMOD_PAD_BUS_2(vref_s_2, vref_s, vref_s_1, vref_s, vref_s_0, vref_s);
        RMOD_DPI_CTRL_0(fw_set_mode, 2);
        DPI_CTRL_1rv = 0x3; // update from register to PHY
        udelay(1000);

        // apply zprog
        RMOD_PAD_CTRL_ZPROG_0(zprog, zprog);

        RMOD_PAD_CTRL_PROG(zctrl_start, 0x1);
        u32_t cnt=0;
        while(0==RFLD_PAD_ZCTRL_STATUS(cali_done)) {
            if(++cnt>0x10000) {udelay(1); err=MEM_CNTLR_ZQ_TIMEOUT; break;}
        }
        if(0!=RFLD_PAD_ZCTRL_STATUS(cali_status)) {
            err=MEM_CNTLR_ZQ_ERROR;
        }

        printf("done (ERR:%d, RES:0x%08x, ZPROG: 0x%x", err, PAD_ZCTRL_STATUSrv, zprog);
#if 1
        // for Debug
        if(ZP_D4_P8_DQ==type || ZP_D4_P8_PD3==type) {
            printf(", POCD:%d, NOCD:%d, ODT:%d", ZP_X_OCD((zprog>>9)&0x1FF, type), ZP_X_OCD(zprog&0x1FF, type)*4, ZP_X_OCD((zprog>>18)&0x1FF, type));
        } else {
            printf(", OCD:%d, ODT:%d", ZP_X_OCD(zprog&0x1FF, type), ZP_X_OCD((zprog>>18)&0x1FF, type)/2);
        }
#endif
        puts(")\n");
        RMOD_PAD_CTRL_PROG(zctrl_start, 0x0);
        udelay(1000);
        idx++;
    }

    // APPLY ODT/OCD Select
    if(DDR_TYPE_DDR3 == ddr_type) {
        DQ_ODT_SEL_0rv    = 0;
        DQ_ODT_SEL_1rv    = 0;
        DQ_OCD_SEL_0rv    = 0;
        DQ_OCD_SEL_1rv    = 0;
        DQS_P_ODT_SEL_0rv = 0x00110011;
        DQS_P_ODT_SEL_1rv = 0x00110011;
        DQS_N_ODT_SEL_0rv = 0x11001100;
        DQS_N_ODT_SEL_1rv = 0x11001100;
        DQS_OCD_SEL_0rv   = 0;
        DQS_OCD_SEL_1rv   = 0;
        CKE_OCD_SELrv     = 0;
        ADR_OCD_SELrv     = 0;
        CK_OCD_SELrv      = 0;
    } else if(DDR_TYPE_DDR4 == ddr_type) {
        DQ_ODT_SEL_0rv    = 0x77117711;
        DQ_ODT_SEL_1rv    = 0x77117711;
        DQ_OCD_SEL_0rv    = 0x11111111;
        DQ_OCD_SEL_1rv    = 0x11111111;
        DQS_P_ODT_SEL_0rv = 0x77117711;
        DQS_P_ODT_SEL_1rv = 0x77117711;
        DQS_N_ODT_SEL_0rv = 0x22222222;
        DQS_N_ODT_SEL_1rv = 0x22222222;
        DQS_OCD_SEL_0rv   = 0x11111111;
        DQS_OCD_SEL_1rv   = 0x11111111;
        CKE_OCD_SELrv     = 0;
        ADR_OCD_SELrv     = 0;
        CK_OCD_SELrv      = 0;
    }
    RMOD_DPI_CTRL_0(fw_set_mode, 0);
    return err;
}

MEMCNTLR_SECTION
void dpi_vref_dq_setting(void)
{
    // SK => 0x14
    //#define VREF_S      (0xA)   // (0x14)
    //#define VREF_R      (0x0)
    
    //#define VREF_S      (0x34)
    //#define VREF_R      (0x1)
    if(DDR_TYPE_DDR4==meminfo.cntlr_opt->dram_type) {
        #define VREF_S      (meminfo.dpi_opt->dpi_vref_s)
        #define VREF_R      (meminfo.dpi_opt->dpi_vref_r)
        RMOD_PAD_BUS_2(vref_s_0, VREF_S, vref_range_0, VREF_R,\
                       vref_s_1, VREF_S, vref_range_1, VREF_R,\
                       vref_s_2, VREF_S, vref_range_2, VREF_R);
        RMOD_DPI_CTRL_0(fw_set_mode, 2);
        DPI_CTRL_1rv = 0x3;
        udelay(1000);
        RMOD_DPI_CTRL_0(fw_set_mode, 0);
        INFO_PRINTF("DPI VREF_S:%d, VREF_R:%d\n", VREF_S, VREF_R);
    }
}

MEMCNTLR_SECTION
void dpi_set_2nd_cmd_group(void) {
    u32_t v = (meminfo.dpi_opt->cmd_grp2_en)?1:0;
    RMOD_PAD_OE_CTL(dck_1_oe, v);
    RMOD_PAD_OE_CTL_EX(odt_1_oe, v, cke_1_oe, v, cs_n_1_oe, v);
}

MEMCNTLR_SECTION
void mc_enable_bstc(void)
{
    CSRrv = 0x300;      // enable BSTC
    BSTrv = 0;
    BCMrv = 0;
    RMOD_BCR(fus_rd, 1, fus_rg, 1, fus_wd, 1, fus_cmd, 1, crr, 1);  // clean error, flush cmd/wd/rg/rd sram
    CCRrv = 0x100;      // flush FIFO
    udelay(1000);
}


MEMCNTLR_SECTION
unsigned int mc_bstc_check_row(BSTC_INFO_T info, u32_t *ary, u32_t len)
{
    BCR_T bcr;
    BSTC_CMD_T cmd = {{0}};     // {{0}}, gcc bug 53119
    const u32_t burst = 1;
    u32_t idx, cnt;
    u8_t dq_factor;

    dq_factor = RFLD_DCR(dq_32)+1;
    //DBG_PRINTF("dq_fac=%d\n", dq_factor);

    mc_enable_bstc();

    // set BCR
    bcr.f.rd_ex_cnt = burst*len*(RXI310_BUS_WIDTH/32)*dq_factor;
    bcr.f.at_err_stop = 0;
    bcr.f.dis_msk = 1;
    bcr.f.loop = 0; //no loop mode
    bcr.f.cmp = 1;
    bcr.f.stop = 0;
    //DBG_PRINTF("BCR=0x%x\n", bcr.v);
    BCRrv = bcr.v;
    RMOD_BCT(loop_cnt, 0);

    u32_t con_addr = BSTC_SRAM_CMD_BASE;
    for (idx=0; idx<len; idx++, con_addr+=0x8) {
        // set CMD_SRAM
        cmd.f.bank = BSTC_PA2BNK(info, ary[idx]);
        cmd.f.col = BSTC_PA2COL(info, ary[idx]);
        cmd.f.row = BSTC_PA2ROW(info, ary[idx]);
        cmd.f.bl = burst;
        cmd.f.cmd = BSTC_WR_CMD;    // write
        BSTC_CMD_WRITE(con_addr, cmd);
        //DBG_PRINTF("con_addr=0x%x idx=%d ary[idx]=0x%08x, WR cmd.v=%x,%x\n", con_addr, idx, ary[idx], REG32(con_addr), REG32((con_addr+4)));

        // set CMD_SRAM,
        cmd.f.bank = BSTC_PA2BNK(info, ary[len-1-idx]);
        cmd.f.col = BSTC_PA2COL(info, ary[len-1-idx]);
        cmd.f.row = BSTC_PA2ROW(info, ary[len-1-idx]);
        cmd.f.cmd = BSTC_RD_CMD;    // read
        BSTC_CMD_WRITE(con_addr+len*8, cmd);
        //DBG_PRINTF("con_addr=0x%x idx=%d ary[idx]=0x%08x, RD cmd.v=%x,%x\n", con_addr+len*8, len-1-idx, ary[len-1-idx], REG32(con_addr+len*8), REG32((con_addr+len*8+4)));
    }

    // prepare WD and RG
    con_addr = BSTC_SRAM_WD_BASE;
    for (idx=0; idx<(len*burst); idx++) {
        #define WD_RG_GAP   (BSTC_SRAM_RG_BASE-BSTC_SRAM_WD_BASE)
        for (cnt=0; cnt<BSTC_SRAM_WD_BUSW*dq_factor; cnt+=32, con_addr+=4) { // 32-bit
            REG32(con_addr)=ary[idx];
            REG32(con_addr+WD_RG_GAP)=ary[len-1-idx];
            //DBG_PRINTF("WD 0x%x = 0x%08x, RG 0x%x = 0x%08x\n", con_addr, REG32(con_addr), con_addr+WD_RG_GAP, REG32(con_addr+WD_RG_GAP));
        }
    }

    //DBG_PRINTF("kick off BSTC WRITE READ... ");
    CCRrv = 0x4;
    while (0==RFLD_CCR(btt)){puts("*\b");};
    //DBG_PRINTF("done, BST=0x%x, BER=0x%x\n", BSTrv, BERrv);

    // bstc clear
    mc_disable_bstc();

    u32_t err = BERrv;
    for (idx=0, cnt=0; idx<15; idx++) {
        if(0!= ((1<<idx) & err)) {
            //DBG_PRINTF("bit%d error!\n", idx);
            cnt++;
        }
    }

    mc_rxi310_init(1);
    return (16-cnt);
}

extern void mc_update_trfc(mc_info_t *, u32_t);
MEMCNTLR_SECTION
unsigned int mc_dram_size_detect(void)
{
                    //        8Gb,        4Gb,        2Gb,        1Gb,      512Mb
    u32_t array[] = {  0x3F801200, 0x1F801200, 0x0F801200, 0x07801200, 0x03801200 };
    u32_t row_size;
    BSTC_INFO_T info;

    // using x16 to do BSTC size dection
    u32_t dq32 = RFLD_DCR(dq_32);
    info.col_size = get_col_size()-dq32;
    info.bnk_size = get_bnk_size();
    info.wr_ent_cnt = info.rd_ent_cnt = BSTC_SRAM_CMD_ENTY/2;

    u32_t dcr = DCRrv;
    RMOD_DCR(dq_32, 0, half_dq, 1, rank2, 0);
    udelay(1000);
    mc_rxi310_init(1);

    if(0!=meminfo.cntlr_opt->rowcnt) {
        INFO_PRINTF("Static Size Apply ... ");
        row_size = meminfo.cntlr_opt->rowcnt;
    } else {
        INFO_PRINTF("Auto Size Detect ... ");
        row_size = mc_bstc_check_row(info, array, sizeof(array)/sizeof(u32_t));
    }

    u32_t sizeinMB = 1<<(info.col_size+info.bnk_size+row_size)>>20;

    // restore
    info.col_size += dq32;
    DCRrv = dcr;
    otto_sc.memsize = sizeinMB*(dq32+1);
    mc_update_trfc(&meminfo, sizeinMB);
    udelay(1000);
    mc_rxi310_init(1);

    printf("x%d %dMB (Bank:%d, Row:%d, Col:%d)\n",
        dq32?32:16, otto_sc.memsize, info.bnk_size, row_size, info.col_size);

    MR6rv = (info.bnk_size << 12) | (row_size << 6) | info.col_size;
    return 0;
}


MEMCNTLR_SECTION
void mc_mode_register_read(void)
{
    u32_t i, j, t, _t, mr3;
    u8_t mpr[4];
    volatile u16_t *reg;

    // modify to DQ16
    u32_t dcr = DCRrv;
    RMOD_DCR(dq_32, 0, half_dq, 1, rank2, 0);

    // modify MR3
    mr3=meminfo.register_set->mr3.v;

    MR3rv = mr3|0x6;
    mc_rxi310_init(1);
    u8_t col = get_col_size();
    u8_t ba_off = col+1;    // 1 for BL
    #define DRAM_UA     (0xA0000000)
    #define UI0_OFF     (0xE)
    // MPR0
    for(j=0; j<4; j++) {
        INFO_PRINTF("MPR%d: ", j);
        reg=(volatile u16_t *)(j*(1<<ba_off)+DRAM_UA+UI0_OFF);
        for(i=0, t=0; i<8; i++, reg--) {
            _t = (*reg&0x1);
            t |= _t<<i;
            printf("%d ", _t);
        }
        mpr[j] = t;
        printf(", = 0x%02x\n", mpr[j]);
    }

    // restore MR3
    MR3rv = mr3;
    DCRrv = dcr;
    mc_rxi310_init(1);
}

MEMCNTLR_SECTION
void mc_result_show_dram_config(void)
{
    //mc_register_set_t *rs = meminfo.register_set;

    INFO_PRINTF("CCR: 0x%08x, DCR: 0x%08x, IOCR:0x%08x, CSR: 0x%08x\n",
        CCRrv, DCRrv, IOCRrv, CSRrv);
    INFO_PRINTF("TPR0:0x%08x, TPR1:0x%08x, TPR2:0x%08x, TPR3:0x%08x\n",
        TPR0rv, TPR1rv, TPR2rv, TPR3rv);
    INFO_PRINTF("MR0: 0x%08x, MR1: 0x%08x, MR2: 0x%08x, MR3: 0x%08x\n",
        MR0rv, MR1rv, MR2rv, MR3rv);
    INFO_PRINTF("MR4: 0x%08x, MR5: 0x%08x, MR6: 0x%08x\n",
        MR4rv, MR5rv, MR6rv);
    INFO_PRINTF("DRR: 0x%08x, MISC:0x%08x, MR_INFO: 0x%08x\n",
        DRRrv, MISCrv, MR_INFOrv);
    return;
}

MEMCNTLR_SECTION
u32_t mc_memtest(void)
{
    INFO_PRINTF("MEM_TEST ... ");
#define PHY_BOUND_LOW  (0x4)
#define PHY_BOUND_HIGH (0x10000000)    // 256MB
    u32_t addr, i, j;
    u32_t low, high;
    volatile u32_t *p;

    low = PHY_BOUND_LOW;
    // step 1: cache size memory test
    high = (low + 32*1024)|0x80000000;
    for(j=0, i=0x5A5A5A5A; j<2; j++) {
        p = (volatile u32_t *)(low|0x80000000);
        while((u32_t)p<high) {
            *(p++) = i;
        }
        _dcache_flush();
        p = (volatile u32_t *)(low|0x80000000);
        while((u32_t)p<high) {
            if(*p!=i) {
                FERR_PRINTF("memory test: 0x%x=0x%x(bad)!=0x%x(good)\n", p, *p, i);
                return 1;
            }
            p++;
        }
        i=~i;
    }

    // step 2: do address bit walking 1 and its complement test
    if(otto_sc.memsize<(PHY_BOUND_HIGH>>20)) {
        high = otto_sc.memsize << 20;
    } else {
        high = PHY_BOUND_HIGH;
    }
    for (j=0, i=low; j<2; j++) {
        while(i<high) { //256MB
            addr = (((j)?~i:i)&0x0FFFFFFC)|0xA0000000;
            p = (volatile u32_t *)addr;
            *p = addr;
            i = i << 1;
        }

        i = i >> 1;
        while(i>=low) {
            addr = (((j)?~i:i)&0x0FFFFFFC)|0xA0000000;
            p = (volatile u32_t *)addr;
            if (*p != addr) {
                FERR_PRINTF("address overlap: 0x%08x and 0x%08x\n", *p, addr);
                return 1;
            }
            i = i >> 1;
        }
        i = i << 1;
    }
    
    // step 3: do page traverse test
    u32_t step = (otto_sc.memsize<<20)>>(10+3); // column (A0~A9) + bank (BA0~BA2)
    high |= 0x80000000;
    for (j=0; j<2; j++) {
        addr = 0x80000000;
        i = 0x5A5A5A5A;  // pattern
        while(addr<high) {
            p = (volatile u32_t *)(addr);
            while((u32_t)p<(addr+1024)) {
                *(p++) = i;
            }
            i = ((i>>1)|(i&0x1)<<31);
            addr += step;
        }
        addr = 0x80000000;
        i = 0x5A5A5A5A;  // pattern
        while(addr<high) {
            p = (volatile u32_t *)(addr);
            while((u32_t)p<(addr+1024)) {
                if(*p != i) {
                    FERR_PRINTF("page traverse: 0x%08x=0x%08x (good: 0x%08x)\n", p, *p, i);
                    return 1;
                }
                p++;
            }
            i = ((i>>1)|(i&0x1)<<31);
            addr += step;
        }
    }
    puts("pass\n");
    return 0;
}

MEMCNTLR_SECTION
void mc_misc(void)
{
	mc_dram_size_detect();
    mc_memtest();
}

REG_INIT_FUNC(mc_misc, 20);
