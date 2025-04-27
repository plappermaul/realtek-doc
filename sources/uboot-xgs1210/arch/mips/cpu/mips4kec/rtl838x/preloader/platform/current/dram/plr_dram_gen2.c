#include <preloader.h>
#include "plr_dram_gen2.h"
#include "plr_dram_gen2_memctl.h"
#include "plr_plat_dep.h"
#include "concur_test.h"


//#define DRAM_PARAM_DEBUG

static void _memctl_delay_clkm_cycles(unsigned int delay_cycles);

#if 0
void _set_dmcr(unsigned int dmcr_value)
{
    /* Error cheching here ?*/
    REG32(DMCR) = dmcr_value;
    while(REG32(DMCR) & 0x80000000);

    return;
}

void memctlc_set_DRAM_buswidth(unsigned int buswidth)
{
    volatile unsigned int *dcr;
    unsigned int t_cas, dmcr_value;
    /* get DCR value */
    dcr = (unsigned int *)DCR;


    switch (buswidth){
        case 8:
            if(memctlc_is_DDR()){
                t_cas = ((REG32(DTR0) & (~(DTR0_CAS_MASK))) >> DTR0_CAS_FD_S);
                /*0:2.5, 1:2, 2:3*/
                if(t_cas == 0)
                    dmcr_value = 0x00100062;
                else if(t_cas == 1)
                    dmcr_value = 0x00100022;
                else if(t_cas == 2)
                    dmcr_value = 0x00100032;
                else{
                    printf("%s, %d: Error t_cas value(%d)\n", __FUNCTION__, __LINE__, t_cas);
                    return;
                }
                _set_dmcr(dmcr_value);
            }
            *dcr = (*dcr & (~((unsigned int)DCR_DBUSWID_MASK)));
            break;

        case 16:
            if(memctlc_is_DDR()){
                t_cas = ((REG32(DTR0) & (~(DTR0_CAS_MASK))) >> DTR0_CAS_FD_S);
                /*0:2.5, 1:2, 2:3*/
                if(t_cas == 0)
                    dmcr_value = 0x00100062;
                else if(t_cas == 1)
                    dmcr_value = 0x00100022;
                else if(t_cas == 2)
                    dmcr_value = 0x00100032;
                else{
                    printf("%s, %d: Error t_cas value(%d)\n", __FUNCTION__, __LINE__, t_cas);
                    return;
                }
                _set_dmcr(dmcr_value);
            }
            *dcr = (*dcr & (~((unsigned int)DCR_DBUSWID_MASK))) | (unsigned int)(1<<DCR_DBUSWID_FD_S);
            break;

        default:
            _memctl_debug_printf("%s, %d: Error buswidth value(%d)\n", __FUNCTION__, __LINE__, buswidth);
            break;
    }

    return;
}


void memctlc_set_DRAM_colnum(unsigned int col_num)
{
    volatile unsigned int *dcr;

    /* get DCR value */
    dcr = (unsigned int *)DCR;


    switch (col_num){
        case 256:
            *dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK)));
            break;

        case 512:
            *dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(1<<DCR_COLCNT_FD_S);
            break;

        case 1024:
            *dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(2<<DCR_COLCNT_FD_S);
            break;

        case 2048:
            *dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(3<<DCR_COLCNT_FD_S);
            break;

        case 4096:
            *dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(4<<DCR_COLCNT_FD_S);
            break;

        default:
            _memctl_debug_printf("%s, %d: Error column number value(%d)\n", __FUNCTION__, __LINE__, col_num);
            break;
    }

    return;


}
void memctlc_set_DRAM_rownum(unsigned int row_num)
{
    volatile unsigned int *dcr;

    /* get DCR value */
    dcr = (unsigned int *)DCR;


    switch (row_num){
        case 2048:
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK)));
            break;

        case 4096:
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(1<<DCR_ROWCNT_FD_S);
            break;

        case 8192:
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(2<<DCR_ROWCNT_FD_S);
            break;

        case 16384:
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(3<<DCR_ROWCNT_FD_S);
            break;

        case (32*1024):
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(4<<DCR_ROWCNT_FD_S);
            break;

        case (64*1024):
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(5<<DCR_ROWCNT_FD_S);
            break;

        default:
            _memctl_debug_printf("%s, %d: Error row number value(%d)\n", __FUNCTION__, __LINE__, row_num);
            break;
    }

    return;


}
void memctlc_set_DRAM_banknum(unsigned int bank_num)
{
    volatile unsigned int *dcr, *dmcr;

    /* get DCR value */
    dcr = (unsigned int *)DCR;
    dmcr = (unsigned int *)DMCR;


    if(memctlc_is_DDR()){
        switch (bank_num){
            case 2:
                *dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK)));
                break;

            case 4:
                *dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(1<<DCR_BANKCNT_FD_S);
                break;

            default:
                _memctl_debug_printf("%s, %d: Error DDR1 bank number value(%d)\n", __FUNCTION__, __LINE__, bank_num);
                break;
        }
    }else{
        switch (bank_num){
            case 4:
                *dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(1<<DCR_BANKCNT_FD_S);
                break;

            case 8:
                *dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(2<<DCR_BANKCNT_FD_S);
                break;

            default:
                _memctl_debug_printf("%s, %d: Error DDR2/3 bank number value(%d)\n", __FUNCTION__, __LINE__, bank_num);
                break;
        }
    }

    return;

}
void memctlc_set_DRAM_chipnum(unsigned int chip_num)
{
    volatile unsigned int *dcr;

    /* get DCR value */
    dcr = (unsigned int *)DCR;


    switch (chip_num){
        case 1:
            *dcr = (*dcr & (~((unsigned int)DCR_DCHIPSEL_MASK)));
            break;

        case 2:
            *dcr = (*dcr & (~((unsigned int)DCR_DCHIPSEL_MASK))) | (unsigned int)(1<<DCR_DCHIPSEL_FD_S);
            break;

        default:
            _memctl_debug_printf("%s, %d: Error chip number value(%d)\n", __FUNCTION__, __LINE__, chip_num);
            break;
    }

    return;

}


int memctl_dram_para_set(unsigned int width, unsigned int row, unsigned int column, \
             unsigned int bank,  unsigned int cs)
{
    //unsigned int dcr_value;
    //volatile unsigned int *p_dcr;


    _memctl_debug_printf("%s, %d: width(%d), row(%d), column(%d), bank(%d), cs(%d)\n",\
                 __FUNCTION__, __LINE__, width, row, column, bank, cs);
    memctlc_set_DRAM_buswidth(width);
    memctlc_set_DRAM_colnum(column);
    memctlc_set_DRAM_rownum(row);
    memctlc_set_DRAM_banknum(bank);
    memctlc_set_DRAM_chipnum(cs);

    /* Reset PHY FIFO pointer */
    memctlc_dram_phy_reset();

    return MEMCTL_DRAM_PARAM_OK;
}


/* Function Name: 
 *  memctlc_dram_size_detect
 * Descripton:
 *  Detect the size of current DRAM Chip
 * Input:
 *  None
 * Output:
 *  None
 * Return:
 *  The number of bytes of current DRAM chips
 * Note:
 *  None
 */
unsigned int memctlc_dram_size_detect(void)
{
    //unsigned int i;
    unsigned int width, row, col, bk, cs, max_bk;
    volatile unsigned int *p_dcr;
    volatile unsigned int *test_addr0, *test_addr1;
    volatile unsigned int test_v0, test_v1, tmp;
    p_dcr       = (volatile unsigned int *)DCR;

    /* Intialize DRAM parameters */
    width = MEMCTL_DRAM_MIN_WIDTH;
    row   = MEMCTL_DRAM_MIN_ROWS ;
    col   = MEMCTL_DRAM_MIN_COLS ;
    cs    = MEMCTL_DRAM_MIN_CS;

    /* Configure to the maximun bank number */
    if(memctlc_is_DDR()){
        bk = MEMCTL_DRAM_DDR_MIN_BANK_NUM;
    }else if(memctlc_is_DDR2()){
        bk = MEMCTL_DRAM_DDR2_MIN_BANK_NUM;
    }else{
        bk = MEMCTL_DRAM_DDR3_MIN_BANK_NUM;
    }   


    /* 0. Buswidth detection */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_v0 = 0x12345678;
    test_v1 = 0x00000000;
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(MEMCTL_DRAM_MAX_WIDTH, row, col, bk , cs)){
        memctlc_dram_phy_reset();
        *test_addr0 = test_v1;
        *test_addr0 = test_v0;
        //_memctl_debug_printf("test_addr(0x%08x)!= test_v0(0x%08x)\n", *test_addr0, test_v0);
        if( test_v0 !=  *test_addr0 ){
            width = MEMCTL_DRAM_MIN_WIDTH;
        }else{
            width = MEMCTL_DRAM_MAX_WIDTH;
        }
    }


    /* 1. Chip detection */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_v0 = 0xCACA0000;
    test_v1 = 0xACAC0000;
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk , MEMCTL_DRAM_MAX_CS)){
        while(cs < MEMCTL_DRAM_MAX_CS){
            memctlc_dram_phy_reset();
            test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row*bk*cs);
            *test_addr0 = 0x0;
            *test_addr1 = 0x0;
            *test_addr0 = test_v0;
            *test_addr1 = test_v1;
            if( test_v0 ==  *test_addr0 ){
                if( test_v1 ==  *test_addr1 ){
                    cs = cs << 1;
                    test_v0++;
                    test_v1++;
                    continue;
                }
            }
            break;
        }
    }

    /* 2. Bank detction */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_v0 = 0x33330000;
    test_v1 = 0xCCCC0000;
    if(memctlc_is_DDR()){
        max_bk = MEMCTL_DRAM_DDR_MAX_BANK_NUM;
    }else if(memctlc_is_DDR2()){
        max_bk = MEMCTL_DRAM_DDR2_MAX_BANK_NUM;
    }else{
        max_bk = MEMCTL_DRAM_DDR3_MAX_BANK_NUM;
    }

    //_memctl_debug_printf("%s,%d: width(%d), row(%d), col(%d), max_bk(%d), cs(%d)\n", __FUNCTION__, __LINE__, width, row, col, max_bk, cs);
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, max_bk, cs)){
        while(bk < max_bk){
            memctlc_dram_phy_reset();
            test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row*bk);
            _memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
            _memctl_debug_printf("BK:(%p)\n", test_addr1);
            *test_addr0 = 0x0;
            *test_addr1 = 0x0;
            *test_addr0 = test_v0;
            *test_addr1 = test_v1;
            tmp = *test_addr0;
            tmp = *test_addr1;
            //_memctl_debug_printf("test_addr0(%p):0x%x\n", test_addr0, *test_addr0);
            //_memctl_debug_printf("test_addr1(%p):0x%x\n", test_addr1, *test_addr1);
            if( test_v0 ==  *test_addr0 ){
                if( test_v1 ==  *test_addr1 ){
                    bk = bk << 1;
                    test_v0++;
                    test_v1++;
                    continue;
                }
            }
            break;
        }
    }

    /* 3. Row detction */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_v0 = 0xCAFE0000;
    test_v1 = 0xDEAD0000;
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, MEMCTL_DRAM_MAX_ROWS, col, bk, cs)){
        while(row < MEMCTL_DRAM_MAX_ROWS){
            memctlc_dram_phy_reset();
            test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row);
            _memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
            _memctl_debug_printf("row:(%p)\n", test_addr1);
            *test_addr0 = test_v0;
            *test_addr1 = test_v1;
            tmp = *test_addr0;
            tmp = *test_addr1;
            _memctl_debug_printf("test_addr0(%p):0x%x\n", test_addr0, *test_addr0);
            _memctl_debug_printf("test_addr1(%p):0x%x\n", test_addr1, *test_addr1);
            _memctl_debug_printf("row = %d\n", row);
            if( test_v0 ==  *test_addr0 ){
                if( test_v1 ==  *test_addr1 ){
                    row = row << 1;
                    test_v0++;
                    test_v1++;
                    continue;
                }
            }
            break;
        }
    }

    /* 4. Column detection */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_v0 = 0x5A5A0000;
    test_v1 = 0xA5A50000;
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, MEMCTL_DRAM_MAX_COLS, bk, cs)){
        while(col < MEMCTL_DRAM_MAX_COLS){
            memctlc_dram_phy_reset();
            test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col);
            *test_addr0 = 0x0;
            *test_addr1 = 0x0;
            _memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
            _memctl_debug_printf("col:(%p)\n", test_addr1);
            *test_addr0 = test_v0;
            *test_addr1 = test_v1;
            tmp = *test_addr0;
            tmp = *test_addr1;
            _memctl_debug_printf("test_addr0(%p):0x%x\n", test_addr0, *test_addr0);
            _memctl_debug_printf("test_addr1(%p):0x%x\n", test_addr1, *test_addr1);
            if( test_v0 ==  *test_addr0 ){
                if( test_v1 ==  *test_addr1 ){
                    col = col << 1;
                    test_v0++;
                    test_v1++;
                    continue;
                }
            }
            break;
        }
    }

    /* 5. Width detction */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_addr1 = (volatile unsigned int *)(0xA0000000);
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk, cs)){
        while(width < MEMCTL_DRAM_MAX_WIDTH){
        memctlc_dram_phy_reset();
        *test_addr0 = 0x3333CCCC;
        __asm__ __volatile__("": : :"memory");
        if( 0x3333CCCC !=  *test_addr0 ){
            width = width >> 1;
            continue;
        }
        __asm__ __volatile__("": : :"memory");
        *test_addr1 = 0x12345678;
        __asm__ __volatile__("": : :"memory");
        if( 0x12345678 !=  *test_addr1 ){
            width = width >> 1;
            continue;
        }
        break;
        }
    }



    memctlc_dram_phy_reset();
    _memctl_debug_printf("DCR(%p): 0x%08x\n", p_dcr, *p_dcr);
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk, cs)){
        _memctl_debug_printf("Width   : %d\n", width);
        _memctl_debug_printf("Row     : %d\n", row);
        _memctl_debug_printf("Column    : %d\n", col);
        _memctl_debug_printf("Bank    : %d\n", bk);
        _memctl_debug_printf("Chip    : %d\n", cs);
        _memctl_debug_printf("total size: %x\n", (unsigned int)((width/8)*row*col*bk*cs));
        goto test_pass;
    }else{
        _memctl_debug_printf("Error! memctl_dram_para_set failed, function: %s, line:%d", __FUNCTION__, __LINE__);
        goto test_fail;
    }

test_pass:
    return (unsigned int)((width/8)*row*col*bk*cs);
test_fail:
    return 0;
}
#endif /*#if 0*/

int memctlc_ZQ_calibration(unsigned int auto_cali_value);

#if 0
unsigned int auto_cali_value[] = {
                                        0x000010fC, /* ODT 75ohm    OCD 60ohm */
                                        0x000011be, /* ODT 50ohm    OCD 50ohm */
                                        0x0000107c, /* ODT 60ohm    OCD 60ohm */
                                        0x00000530, /* ODT 80ohm    OCD 80ohm */
                                        0x000010ef, /* ODT 75ohm    OCD 75ohm */
                                        0x00002c9e, /* ODT 85.5ohm  OCD 87.27ohm */
                                        0x00002828, /* ODT 92.31ohm OCD 96ohm */
                                        0x000008fC, /* ODT 150ohm   OCD 60ohm */
                                        0x000036fe, /* ODT 50ohm    OCD 48ohm */
                                        0x00002ffc, /* ODT 50ohm    OCD 60ohm */
                                        0x000023a3, /* ODT 100ohm   OCD 100ohm */
                                        0x000008a1  /* ODT 150ohm   OCD 150ohm */
                                 };

/* OCD is prepared at composer and feed into mrs directly */
unsigned int get_memory_dram_ocd_parameters(unsigned int *para_array)
{
         para_array[0] = 60;
         return 1;//OK
}

/* ODT is prepared at composer and feed into mrs directly */
unsigned int get_memory_dram_odt_parameters(unsigned int *para_array)
{
        para_array[0] = 75;
        return 1;//OK
}
#endif

//void memctlc_dram_phy_reset(void)
//{
//  volatile unsigned int *phy_ctl;
//
//  phy_ctl = (volatile unsigned int *)DACCR;
//  *phy_ctl = *phy_ctl & ((unsigned int) 0xFFFFFFEF);
//  *phy_ctl = *phy_ctl | ((unsigned int) 0x10);
//  _memctl_debug_printf("memctlc_dram_phy_reset: %p(0x%08x)\n", phy_ctl, *phy_ctl);
//
//  return;
//}

#if 0
void show_dram_config(void)
{
    unsigned int i;
    volatile unsigned int *phy_reg;
    _memctl_debug_printf_I("\n");
    _memctl_debug_printf_I("DDRKODL(0x%08x):0x%08x\n",\
                 DDRCKODL_A, REG32(DDRCKODL_A));
    _memctl_debug_printf_I("DCDR(0x%08x):0x%08x\n" "DDCR(0x%08x):0x%08x\n",\
                 DCDR, REG32(DCDR), DDCR, REG32(DDCR));
    _memctl_debug_printf_I("MCR (0x%08x):0x%08x, 0x%08x, 0x%08x, 0x%08x\n", \
                MCR, REG32(MCR), REG32(DCR), REG32(DTR0), REG32(DTR1));
    _memctl_debug_printf_I("DTR2(0x%08x):0x%08x\n", DTR2, REG32(DTR2));
    _memctl_debug_printf_I("PHY Registers(0x%08x):\n", DACCR);
    phy_reg = (volatile unsigned int *)DACCR;
    for(i=0;i<11;i++){
        _memctl_debug_printf_I("0x%08x:0x%08x, 0x%08x, 0x%08x, 0x%08x\n", \
                phy_reg, *(phy_reg), *(phy_reg+1), *(phy_reg+2), *(phy_reg+3) );
        phy_reg+=4;
    }
}
#endif /* #if 0 */

int memctlc_set_dqm_delay(void)
{
    volatile unsigned int *dcdqmr;

    dcdqmr = (volatile unsigned int *)DCDQMR;
    *dcdqmr = DRAMI.static_cal_data_32;

    return 1;
}

void memctlc_config_DRAM_size(void)
{
    //volatile unsigned int *dcr, dummy_read;
    volatile unsigned int *dcr;

    dcr = (volatile unsigned int *)DCR;

    *dcr = DRAMI.dcr;
#if 0 /* Remove size_auto_detection */
    if(DRAMI.size_auto_detection){
        memctlc_dram_size_detect();
        dummy_read = *dcr;
        *dcr = dummy_read;
    }else{
        *dcr = DRAMI.dcr;
    }
#endif /*#if 0*/

    /* if it's DDR3 16bit, enable DRAM buffer full mask */
    //if(memctlc_is_DDR3()){
    //    if(*dcr & 0x01000000){
    //        dcr = (volatile unsigned int *)MCR;
    //        *dcr = *dcr | 0x1e0;
    //    }else{
    //        dcr = (volatile unsigned int *)MCR;
    //        *dcr = *dcr & 0xFFFFFE1F;
    //    }
    //}
    /* Update from YUMC's verificatin on 6266, designer suggests turning on DRAM buffer full mask (data flow control) for DDR3 8/16 bit */
    if(memctlc_is_DDR3()){
        dcr = (volatile unsigned int *)MCR;
        *dcr = *dcr | 0x1e0;
    }

    return ;
}

init_result_t memctlc_ZQ_config(void)
{
    init_result_t ret;

    ret = INI_RES_OK;

    if(DRAMI.zq_setting == 0xDEADDEAD) { /* Use magic number 0xDEADDEAD to patch ZQ HW bug */
        _memctl_debug_printf_I("INFO: Applying ZQ result directly by plat_memctl_ZQ_force_config()\n");
        plat_memctl_ZQ_force_config();
    } else {
        /* ZQ calibration */
        if(MEMCTL_ZQ_CALI_PASS != memctlc_ZQ_calibration(DRAMI.zq_setting)) {
            ret = INI_RES_FAIL;
            _memctl_debug_printf_E("ZQ calibration failed\n");
        }
    }

    return ret;
}

void memctlc_DDR3_ZQ_long_calibration(void)
{
	volatile unsigned int *ddr3_zqccr;
	ddr3_zqccr = (volatile unsigned int *)D3ZQCCR;

	*ddr3_zqccr |= (1<<31);
	_memctl_delay_clkm_cycles(100);
	while(*ddr3_zqccr & (1<<31));

	return;

}
int memctlc_ZQ_calibration(unsigned int auto_cali_value)
{
    volatile unsigned int *dmcr, *daccr, *zq_cali_reg, *dpcr;
    volatile unsigned int *zq_cali_status_reg;
    volatile unsigned int *socpnr;
    unsigned int polling_limit;
    unsigned int reg_v, odtn, odtp, ocdn, ocdp;


    dmcr    = (volatile unsigned int *)DMCR;
    socpnr  = (volatile unsigned int *)SOCPNR;
    daccr   = (volatile unsigned int *)DACCR;
    dpcr    = (volatile unsigned int *)DPCR;
    zq_cali_reg         = (volatile unsigned int *)DDZQPCR;
    zq_cali_status_reg  = (volatile unsigned int *)DDZQPSR;

    /* Disable DRAM refresh operation */
    *dmcr = ((*dmcr | DMCR_DIS_DRAM_REF_MASK) & (~DMCR_MR_MODE_EN_MASK));


    /* Enable ZQ calibration model */
    if( (*socpnr == MEMCTL_SOCPNR_0399) || (*socpnr == MEMCTL_SOCPNR_0371)){
        /* Enable ZQ calibration model */
        *daccr = *daccr | 0x40000000;
        /* Trigger the calibration */
        *zq_cali_reg = auto_cali_value | 0x40000000;
    }else if((*socpnr == MEMCTL_SOCPNR_6275) || (*socpnr == MEMCTL_SOCPNR_6290)){
        /* Enable ZQ calibration model */
        if(memctlc_is_DDR3()){	// DDR 3
            *dpcr = *dpcr | 0xB;
        }else{			// DDR 1/2
            *dpcr = *dpcr | 0x3;
        }   
        /* Trigger the calibration */
        *zq_cali_reg = auto_cali_value | 0x80000000;            
    }else{
        /* Trigger the calibration */
        *zq_cali_reg = auto_cali_value | 0x80000000;
    }
    /* Polling to ready */
    polling_limit = 0x10000;
    while(*zq_cali_reg & 0x80000000){
        polling_limit--;
        if(polling_limit == 0){
#if (ZQ_TIMEOUT_RESET == 1)
	        printf("Warning: ZQ calibration timeout! Reset for try again.\n");
	        SYSTEM_RESET();
#else
	        printf("Warning: ZQ calibration timeout! Using default values instead.\n");
	        plat_memctl_ZQ_force_config();
	        goto static_zq_setting_done;
#endif
        }
    }

    /* Enable DRAM refresh operation */
    *dmcr = *dmcr &  (~DMCR_DIS_DRAM_REF_MASK) ;

    /* DDR3 SDRAM ZQ long calibration. */
    if(memctlc_is_DDR3()){
        memctlc_DDR3_ZQ_long_calibration();
    }

    /* Patch code for IO PAD */
    //plat_memctl_IO_PAD_patch();

    reg_v = *zq_cali_status_reg;
    if(reg_v & 0x20000000) {  
        odtp = ((reg_v >> 27) & 0x3);
        odtn = ((reg_v >> 25) & 0x3);
        ocdp = ((reg_v >> 23) & 0x3);
        ocdn = ((reg_v >> 21) & 0x3);
        printf("Result of ODTP/ODTN/OCDP/OCDN=%d/%d/%d/%d\n", odtp, odtn, ocdp, ocdn);  
        if((odtp != 0) || /* ODTP must be completed with no error */
           (odtn == 2) || /* ODTN must NOT be underflow (may tolerate code overflow error) */
           (ocdp == 1) || /* OCDP must NOT be overflow (may tolerate code underflow error) */
           (ocdn == 1))   /* OCDN must NOT be overflow (may tolerate code underflow error) */
        { 
            printf("EE: ZQ calibration failed\n");
        return MEMCTL_ZQ_CALI_FAIL; /* Error, calibration fail. */
        }
    }

	goto static_zq_setting_done; /*Remove warning message only*/

 static_zq_setting_done:
    return MEMCTL_ZQ_CALI_PASS; /* Pass, calibration pass.*/
}

void memctlc_config_DTR(void)
{
    printf("Setting DTR\n");
    REG32(DTR0_A)    = DRAMI.dtr0;
    REG32(DTR1_A)    = DRAMI.dtr1;
    REG32(DTR2_A)    = DRAMI.dtr2;

    return;
}


void memctlc_DRAM_DLL_reset(void)
{
#ifdef MEMCTL_DDR2_SUPPORT
    if(memctlc_is_DDR2()){
        _memctl_debug_printf("DDR2\n");
        memctlc_ddr2_dll_reset();
        return;
    }
#endif
#ifdef MEMCTL_DDR3_SUPPORT
    if(memctlc_is_DDR3()){
        _memctl_debug_printf("DDR3\n");
        memctlc_ddr3_dll_reset();
        return;
    }
#endif
#ifdef MEMCTL_DDR1_SUPPORT
    if(memctlc_is_DDR()){
        _memctl_debug_printf("DDR1\n");
        memctlc_ddr1_dll_reset();
        return;
    }
#endif
    return;
}


void memctlc_dram_phy_reset(void)
{
    REG32(DACCR_A) = REG32(DACCR_A) & ((u32_t) 0xFFFFFFEF);
    REG32(DACCR_A) = REG32(DACCR_A) | ((u32_t) 0x10);
    //_memctl_debug_printf("memctlc_dram_phy_reset: %p(0x%08x)\n", phy_ctl, *phy_ctl);
    
    return;
}


#if 0
#define MRS_DELAY_TIME 0x800
static void DDR1_setup_MRS(u32_t mr, u32_t emr) {
    u32_t delay_time;
    
    /* 1. Disable DLL */
    REG32(DMCR_A) = emr | DDR1_EMR1_DLL_DIS;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 2. Enable DLL */
    REG32(DMCR_A) = emr & (~DDR1_EMR1_DLL_DIS);
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 3. Reset DLL */
    REG32(DMCR_A) = mr | DDR1_MR_OP_RST_DLL ;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 4. Waiting 200 clock cycles */
    delay_time = MRS_DELAY_TIME;
    while(delay_time--);
    
    /* 5. Normal mode, avoid to reset DLL when updating phy params */
    REG32(DMCR_A) = mr;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 6. reset phy fifo */
    memctlc_dram_phy_reset();
}    


static void DDR2_setup_MRS(u32_t mr, u32_t emr1, u32_t emr2, u32_t emr3) {
    u32_t delay_time;

    /* 1. Disable DLL */
    REG32(DMCR_A) = emr1 | DDR2_EMR1_DLL_DIS;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 2. Enable DLL */
    REG32(DMCR_A) = emr1 & (~DDR2_EMR1_DLL_DIS);
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 3. Reset DLL */
    REG32(DMCR_A) = mr | DDR2_MR_DLL_RESET_YES ;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 4. Waiting 200 clock cycles */
    delay_time = MRS_DELAY_TIME;
    while(delay_time--);
    
    /* 5. Set EMR2 */
    REG32(DMCR_A) = emr2;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);

    REG32(DMCR_A) = emr3;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 6. reset phy fifo */
    memctlc_dram_phy_reset();
    
    return;

}

static void DDR3_setup_MRS(u32_t mr0, u32_t mr1, u32_t mr2, u32_t mr3) {
    u32_t delay_time;
    
    /* 1. Disable DLL */
    REG32(DMCR_A) = mr1 | DDR3_EMR1_DLL_DIS;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 2. Enable DLL */
    REG32(DMCR_A) = mr1 & (~DDR3_EMR1_DLL_DIS);
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 3. Reset DLL */
    REG32(DMCR_A) = mr0 | DDR3_MR_DLL_RESET_YES ;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 4. Waiting 200 clock cycles */
    delay_time = MRS_DELAY_TIME;
    while(delay_time--);
    
    /* 5. Set EMR2 */
    REG32(DMCR_A) = mr2;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 6. Set EMR3 */
    REG32(DMCR_A) = mr3;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 7. reset phy fifo */
    memctlc_dram_phy_reset();
    
    return;
}
#endif 

/* Return DRAM size in byte. We need not this for now. */
#if 0
__attribute__((__unused__))
static u32_t get_dram_size(void) {
    const u32_t BUSWID_val[] = {1, 2, 4, 0};
    const u32_t ROWCNT_val[] = {2048, 4096, 8192, 16384};
    const u32_t COLCNT_val[] = {256, 512, 1024, 2048, 4096, 0, 0, 0};
    const u32_t BANKCNT_val[] = {4, 8};

    const u32_t DCR = DCR_REG;
    const u32_t EDTCR = EDTCR_REG;

    const u32_t BUSWID_BYTE = BUSWID_val[(DCR >> 28) & 0x3];
    const u32_t ROWCNT = ROWCNT_val[(DCR >> 25) & 0x3];
    const u32_t COLCNT = COLCNT_val[(DCR >> 22) & 0x7];
    const u32_t BANKCNT = BANKCNT_val[(EDTCR >> 30) & 0x3];

    return (BUSWID_BYTE*ROWCNT*COLCNT*BANKCNT);
}
#endif //#if 0

extern u32_t DDR_Calibration(void);
static u32_t dram_software_calibration(void)
{
    u32_t ret;
    
    printf("Start claibation test...");//_memctl_debug_printf_I("Start claibation test\n");

    plat_memctl_input_sig_delay(DRAMI.clkm_delay, DRAMI.clkm90_delay, DRAMI.tx_clk_phs_delay);
    
    /* Reset DRAM DLL */
    memctlc_DRAM_DLL_reset();

    //memctls_init();
    ret = DDR_Calibration();
    if(ret != 0) {
        return ret;
    }

    memctlc_set_dqm_delay();

    return ret;
}

#ifdef DRAM_PARAM_DEBUG
static void dram_display_param_info(void) {
    printf("\n");
    printf("mcr               =0x%08x\n", DRAMI.mcr);
    printf("dcr               =0x%08x\n", DRAMI.dcr);
    printf("mpmr0             =0x%08x\n", DRAMI.mpmr0);
    printf("mpmr1             =0x%08x\n", DRAMI.mpmr1);
    printf("dider             =0x%08x\n", DRAMI.dider);
    printf("d23oscr           =0x%08x\n", DRAMI.d23oscr);
    printf("daccr             =0x%08x\n", DRAMI.daccr);
    printf("dacspcr           =0x%08x\n", DRAMI.dacspcr);
    printf("dacspar           =0x%08x\n", DRAMI.dacspar);
    if(DRAMI.calibration_type == 0) { /* Static calibration */
        printf("static_cal_data_0 =0x%08x\n", DRAMI.static_cal_data_0 );
        printf("static_cal_data_1 =0x%08x\n", DRAMI.static_cal_data_1 );
        printf("static_cal_data_2 =0x%08x\n", DRAMI.static_cal_data_2 );
        printf("static_cal_data_3 =0x%08x\n", DRAMI.static_cal_data_3 );
        printf("static_cal_data_4 =0x%08x\n", DRAMI.static_cal_data_4 );
        printf("static_cal_data_5 =0x%08x\n", DRAMI.static_cal_data_5 );
        printf("static_cal_data_6 =0x%08x\n", DRAMI.static_cal_data_6 );
        printf("static_cal_data_7 =0x%08x\n", DRAMI.static_cal_data_7 );
        printf("static_cal_data_8 =0x%08x\n", DRAMI.static_cal_data_8 );
        printf("static_cal_data_9 =0x%08x\n", DRAMI.static_cal_data_9 );
        printf("static_cal_data_10=0x%08x\n", DRAMI.static_cal_data_10);
        printf("static_cal_data_11=0x%08x\n", DRAMI.static_cal_data_11);
        printf("static_cal_data_12=0x%08x\n", DRAMI.static_cal_data_12);
        printf("static_cal_data_13=0x%08x\n", DRAMI.static_cal_data_13);
        printf("static_cal_data_14=0x%08x\n", DRAMI.static_cal_data_14);
        printf("static_cal_data_15=0x%08x\n", DRAMI.static_cal_data_15);
        printf("static_cal_data_16=0x%08x\n", DRAMI.static_cal_data_16);
        printf("static_cal_data_17=0x%08x\n", DRAMI.static_cal_data_17);
        printf("static_cal_data_18=0x%08x\n", DRAMI.static_cal_data_18);
        printf("static_cal_data_19=0x%08x\n", DRAMI.static_cal_data_19);
        printf("static_cal_data_20=0x%08x\n", DRAMI.static_cal_data_20);
        printf("static_cal_data_21=0x%08x\n", DRAMI.static_cal_data_21);
        printf("static_cal_data_22=0x%08x\n", DRAMI.static_cal_data_22);
        printf("static_cal_data_23=0x%08x\n", DRAMI.static_cal_data_23);
        printf("static_cal_data_24=0x%08x\n", DRAMI.static_cal_data_24);
        printf("static_cal_data_25=0x%08x\n", DRAMI.static_cal_data_25);
        printf("static_cal_data_26=0x%08x\n", DRAMI.static_cal_data_26);
        printf("static_cal_data_27=0x%08x\n", DRAMI.static_cal_data_27);
        printf("static_cal_data_28=0x%08x\n", DRAMI.static_cal_data_28);
        printf("static_cal_data_29=0x%08x\n", DRAMI.static_cal_data_29);
        printf("static_cal_data_30=0x%08x\n", DRAMI.static_cal_data_30);
        printf("static_cal_data_31=0x%08x\n", DRAMI.static_cal_data_31);
        printf("static_cal_data_32=0x%08x\n", DRAMI.static_cal_data_32);
    }

    printf("zq_setting        =0x%08x\n", DRAMI.zq_setting);
    //printf("size_auto_detection =0x%02x\n", DRAMI.size_auto_detection  );
    printf("calibration_type    =0x%02x\n", DRAMI.calibration_type     );
    printf("tx_clk_phs_delay    =0x%02x\n", DRAMI.tx_clk_phs_delay     );
    printf("clkm_delay          =0x%02x\n", DRAMI.clkm_delay           );
    printf("clkm90_delay        =0x%02x\n", DRAMI.clkm90_delay         );
    printf("auto_calibration    =0x%02x\n", DRAMI.auto_calibration     );
    printf("drv_strength        =0x%02x\n", DRAMI.drv_strength         );

    printf("dtr0                =0x%08x\n", DRAMI.dtr0);
    printf("dtr1                =0x%08x\n", DRAMI.dtr1);
    printf("dtr2                =0x%08x\n", DRAMI.dtr2);

    switch(REG32(MCR_A) & MCR_DRAMTYPE_MASK) {
    case  MCR_DRAMTYPE_DDR:
        printf("DDR1_mr  =0x%08x\n", DRAMI.DDR1_mr );
        printf("DDR1_emr =0x%08x\n", DRAMI.DDR1_emr);
        break;

    case  MCR_DRAMTYPE_DDR2:
        printf("DDR2_mr  =0x%08x\n", DRAMI.DDR2_mr );
        printf("DDR2_emr1=0x%08x\n", DRAMI.DDR2_emr1);
        printf("DDR2_emr2=0x%08x\n", DRAMI.DDR2_emr2);
        printf("DDR2_emr3=0x%08x\n", DRAMI.DDR2_emr3);
        break;

    case  MCR_DRAMTYPE_DDR3:
        printf("DDR3_mr0 =0x%08x\n", DRAMI.DDR3_mr0);
        printf("DDR3_mr1 =0x%08x\n", DRAMI.DDR3_mr1);
        printf("DDR3_mr2 =0x%08x\n", DRAMI.DDR3_mr2);
        printf("DDR3_mr3 =0x%08x\n", DRAMI.DDR3_mr3);
        break;

    default:
        printf("Error: Unknown DRAM TYPE strap pin setting.\n");
        break;      
    }
}
#endif //#ifdef DRAM_PARAM_DEBUG
    

static void dram_static_calibration(void) {
    u32_t i;
    const u32_t *src;
    u32_t *dst;
        
    /* Set clkm/m90/TX delay tap */
    plat_memctl_input_sig_delay(DRAMI.clkm_delay, DRAMI.clkm90_delay, DRAMI.tx_clk_phs_delay);

    src = &(DRAMI.static_cal_data_0);
    dst = (u32_t *)STATIC_CAL_DATA_BASE;
    for(i=0; i<STATIC_CAL_DATA_LEN; i++) {
        *dst++ = *src++;
    }

    memctlc_DRAM_DLL_reset();
#if 0
    /* Apply MRS */
    switch(REG32(MCR_A) & MCR_DRAMTYPE_MASK) {
    case  MCR_DRAMTYPE_DDR:
        printf("\nSetting MRS for DDR1\n");
        DDR1_setup_MRS(DRAMI.DDR1_mr, DRAMI.DDR1_emr);
        break;
    case  MCR_DRAMTYPE_DDR2:
        printf("\nSetting MRS for DDR2\n");
        DDR2_setup_MRS(DRAMI.DDR2_mr, DRAMI.DDR2_emr1, DRAMI.DDR2_emr2, DRAMI.DDR2_emr3);
        break;
    case  MCR_DRAMTYPE_DDR3:
        printf("\nSetting MRS for DDR3\n");
        DDR3_setup_MRS(DRAMI.DDR3_mr0, DRAMI.DDR3_mr1, DRAMI.DDR3_mr2, DRAMI.DDR3_mr3);
        break;
    default:
        printf("Error: Unknown DRAM TYPE strap pin setting.\n");
        break;      
    }
#endif /* #if 0 */    
}

void memctlc_DBFM_enable(void)
{
    volatile unsigned int *mcr;

    mcr = (volatile unsigned int *)MCR;
    *mcr = *mcr |0x1e0;

    return;
}

void dram_setup(void) {
    u32_t reg_val;
    volatile unsigned int delay_loop;

#ifdef DRAM_PARAM_DEBUG
    dram_display_param_info();
#endif //#ifdef DRAM_PARAM_DEBUG

    /* MCR is mixed with FLASH setting, using read-modify-write to update it */
    reg_val = REG32(MCR_A);
    reg_val &= ~(MCR_IPREF_MASK | MCR_DPREF_MASK);
    reg_val |= DRAMI.mcr;
    REG32(MCR_A) = reg_val;
    /* Has not been fully verified (suggested by YUMC) */
    //REG32(MPMR0_A)   = DRAMI.mpmr0;
    //REG32(MPMR1_A)   = DRAMI.mpmr1;
    REG32(DIDER_A)   = DRAMI.dider;
    //REG32(D23OSCR_A) = DRAMI.d23oscr;
    REG32(DACCR_A)   = DRAMI.daccr;
    REG32(DACSPCR_A) = DRAMI.dacspcr;
    REG32(DACSPAR_A) = DRAMI.dacspar;

    plat_mem_clk_rev_check();

    /* Enable DRAM clock */
    plat_memctl_dramclk_en();

    memctlc_config_DRAM_size();

    /* Configure DRAM timing parameters */
    memctlc_config_DTR();

    if(memctlc_ZQ_config() != INI_RES_OK) {
        parameters.dram_init_result = INI_RES_DRAM_ZQ_CALI_FAIL;
        goto done;
    }

    /* Set data buffer full mask, enabled when memclk > 200MHz */
    if(plat_memctl_MEM_clock_MHz() > 200) {
        memctlc_DBFM_enable();    
    }

    if(DRAMI.calibration_type == 0) { /* Static calibration */
        printf("II: DRAM is set by static calibration... ");
        dram_static_calibration();
    } else {/* software calibration */
        printf("II: DRAM is set by software calibration... ");
        if(dram_software_calibration() == 0) {
            printf("PASSED\n");
        } else {            
            printf("FAILED\n");
            parameters.dram_init_result = INI_RES_DRAM_SW_CALI_FAIL; //Add new enum for calibration failure
            goto done;
        }
    }

    memctlc_dram_phy_reset();

    memctlc_config_DRAM_size();
  

    delay_loop = 0x80000;
    while(delay_loop--);


    /* A simple test */
#if (SELFTEST == 1)
    printf("DD: Simple DRAM test... ");
    *((volatile u32_t *)0x80000000) = 0x5AFE5AFE;
    if (*((volatile u32_t *)0x80000000) != 0x5AFE5AFE) {
        parameters.dram_init_result = INI_RES_TEST_FAIL;
        goto done;
    }

    *((volatile u32_t *)0xA0000000) = 0x05D650C0;
    if (*((volatile u32_t *)0xA0000000) != 0x05D650C0) {
        parameters.dram_init_result = INI_RES_TEST_FAIL;
        goto done;
    } else {
        /* This could be kinda weird, but coming this far means both $ and un$ tests are passed. */
        printf("OK\n");
        parameters.dram_init_result = INI_RES_OK;
    }
#endif

    parameters.dram_init_result = INI_RES_OK;
    
done:
     plat_memctl_show_dram_config();
    return;
}

static void _memctl_delay_clkm_cycles(unsigned int delay_cycles)
{
    volatile unsigned int *mcr, read_tmp __attribute__((unused));

    mcr = (unsigned int *)MCR;

    while(delay_cycles--){
            read_tmp = *mcr;
    }

    return;
}

void _memctl_update_phy_param(void)
{
    volatile unsigned int *dmcr;
    volatile unsigned int *dcr __attribute__((unused));
    volatile unsigned int *dacr;
    volatile unsigned int dacr_tmp1, dacr_tmp2;
    volatile unsigned int dmcr_tmp;
 
    dmcr = (unsigned int *)DMCR;
    dcr = (unsigned int *)DCR;
    dacr = (unsigned int *)DACCR;
 
    /* Write DMCR register to sync the parameters to phy control. */
    dmcr_tmp = *dmcr;
    *dmcr = dmcr_tmp;
    _memctl_delay_clkm_cycles(10);
    /* Waiting for the completion of the update procedure. */
    while((*dmcr & ((unsigned int)DMCR_MRS_BUSY)) != 0);
 
    __asm__ __volatile__("": : :"memory");
 
    /* reset phy buffer pointer */
    dacr_tmp1 = *dacr;
    dacr_tmp1 = dacr_tmp1 & (0xFFFFFFEF);
    dacr_tmp2 = dacr_tmp1 | (0x10);
    *dacr = dacr_tmp1 ;
 
    _memctl_delay_clkm_cycles(10);
    __asm__ __volatile__("": : :"memory");
    *dacr = dacr_tmp2 ;
 
    return;
}

unsigned int get_memory_memctl_dq_write_delay_parameters(unsigned int *para_array) 
{
    unsigned int ret;
    unsigned int reg_val;

    if( (*para_array) < 32) { 
        reg_val = *((&(DRAMI.static_cal_data_0)) + *para_array);
        *para_array = ( (reg_val & DACDQR_DQR_PHASE_SHIFT_90_MASK) >> DACDQR_DQR_PHASE_SHIFT_90_FD_S);
        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
}

