#include <soc.h>
#include <dram/autok/dram_autok.h>

/* Register Value */
#define DMCR_MRS_BUSY	(0x80000000)

#define MEMCTL_CALI_RETRY_LIMILT            			(5)
#define MEMCTL_CALI_MIN_READ_WINDOW                     (7)
#define MEMCTL_CALI_MIN_WRITE_WINDOW                    (7)
#define CPU_DCACHE_SIZE                                 (0x8000)
#define MEMCTL_CALI_TARGET_LEN                          (CPU_DCACHE_SIZE * 2)
#define MEMCTL_CALI_FULL_SCAN_RESOLUTION                (2)
#define MEMCTL_CALI_WRITE_DELAY_START_TAP               (0)
#define MEMCTL_CALI_READ_DELAY_START_TAP                (0)
#define MEMCTL_CALI_TARGET_ADDR                         (0x80000000)


#define _memctl_DCache_flush_invalidate dcache_wr_inv_all

u32_t pat_ary[] SECTION_RO = {
            0x00010000, 0x01234567, 0x00000000, 0x76543210,
            0xFFFFFFFF, 0x89abcdef, 0x0000FFFF, 0xfedcba98,
            0xFFFF0000, 0x00FF00FF, 0xFF00FF00, 0xF0F0F0F0,
            0x0F0F0F0F, 0x5A5AA5A5, 0xA5A55A5A, 0x5A5AA5A5,
            0xA5A55A5A, 0xA5A55A5A, 0x5A5AA5A5, 0xA5A55A5A,
            0x5A5AA5A5, 0x5555AAAA, 0xAAAA5555, 0x5555AAAA,
            0xAAAA5555, 0xAAAA5555, 0x5555AAAA, 0xAAAA5555,
            0x5555AAAA, 0xCC3333CC, 0x33CCCC33, 0xCCCC3333
           };

SECTION_AUTOK
static void _memctl_delay_clkm_cycles(unsigned int delay_cycles)
{
    volatile unsigned int *mcr, read_tmp;

    mcr = (unsigned int *)MCR;

    while(delay_cycles--){
            read_tmp = *mcr;
    }

    return;
}

SECTION_AUTOK
void _memctl_update_phy_param(void)
{
    volatile unsigned int *dmcr;
    volatile unsigned int *dcr;
    volatile unsigned int *dacr;
    volatile unsigned int dacr_tmp1, dacr_tmp2;
    volatile unsigned int dmcr_tmp;
    volatile unsigned int loop_lim = 0;

    dmcr = (unsigned int *)DMCR;
    dcr = (unsigned int *)DCR;
    dacr = (unsigned int *)DACCR;

    /* Write DMCR register to sync the parameters to phy control. */
    dmcr_tmp = *dmcr;
    *dmcr = dmcr_tmp;
    _memctl_delay_clkm_cycles(10);

    /* Waiting for the completion of the update procedure. */
    while((*dmcr & ((unsigned int)DMCR_MRS_BUSY)) != 0) {
	    if (loop_lim++ > 100) {
		    puts("EE: DMCR does not respond. Reset...\n");
		    SYSTEM_RESET();
	    }
    }

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

SECTION_AUTOK
static void memctl_sync_write_buf(void)
{
	*((volatile unsigned int *)0xB8001038) = 0x80000000;
	while(*((volatile unsigned int *)0xB8001038) & 0x80000000);
	return;
}

SECTION_AUTOK
static void _write_pattern_1(u32_t start_addr, u32_t len)
{
	volatile u32_t *p_start, data_tmp;
	u32_t b_len;
	u32_t ary_i;


	/* In case of write through D-Cache mechanisim, read data in DCache */
	p_start = (volatile u32_t *)start_addr;
	for(b_len = 0; b_len < len; b_len += sizeof(u32_t)){
		data_tmp = *p_start;
	}

	/* Write data */
	p_start = (volatile u32_t *)start_addr;
	ary_i = 0;
	for(b_len = 0; b_len < len; b_len += sizeof(u32_t)){
		*p_start = pat_ary[ary_i];
		p_start++;
		ary_i = (ary_i+1) % (sizeof(pat_ary)/sizeof(u32_t));
	}

	_memctl_DCache_flush_invalidate();

	return;
}

SECTION_AUTOK
static u32_t _verify_pattern_1(u32_t start_addr, u32_t len)
{

	volatile u32_t *p_start, data_tmp;
	u32_t b_len, err_result;
	u32_t ary_i, pat_data;

	_memctl_DCache_flush_invalidate();

	err_result = 0;

	/* Read data */
	ary_i = 0;
	p_start = (volatile u32_t *)start_addr;
	for(b_len = 0; b_len < len; b_len += sizeof(u32_t)){
		data_tmp = *p_start;
		pat_data = pat_ary[ary_i];
		ary_i = (ary_i+1) % (sizeof(pat_ary)/sizeof(u32_t));
		err_result = err_result | ( (pat_data | data_tmp) & ( ~(pat_data & data_tmp)));
		if(err_result == 0xffffffff)
			return err_result;
		p_start++;
	}

	return err_result;
}


SECTION_AUTOK
static void _memctl_set_phy_delay_all(u32_t w_delay, u32_t r_delay)
{
	u32_t i_dq;
	volatile u32_t *ddcrdqr_base;

    ddcrdqr_base = (u32_t *)0xB8001510;

    //printf("%s:%d: wdelay(%d), r_delay(%d)\n", __FUNCTION__, __LINE__, w_delay, r_delay);
    for(i_dq = 0; i_dq < 32; i_dq++){
        *ddcrdqr_base = (w_delay << 24) | (r_delay << 8);
        ddcrdqr_base++;
    }

    _memctl_update_phy_param();

    return ;
}


extern unsigned int memctlc_DDR_Type(void);

SECTION_AUTOK
static void  _memctl_set_phy_delay_dqrf(u32_t bit_loc,u32_t max_w_seq_start,u32_t max_w_len,u32_t max_r_seq_start,u32_t max_r_len)
{
	volatile u32_t *ddcrdqr_base, *soc_id_reg;
	volatile u32_t *mcr;
	unsigned char r_delay_tap;
#if 1
	unsigned char w_delay_tap;
#endif
	u32_t ps_mul, ps_div;

	ddcrdqr_base = (volatile u32_t *)0xB8001510;
	soc_id_reg   = (volatile u32_t *)0xB80010FC;
	mcr = (volatile u32_t *)MCR;

	ddcrdqr_base += bit_loc;

#if 1 //we currently don't set write DQ delay taps in new mem-controller
	if(memctlc_DDR_Type() == IS_DDR2_SDRAM){		//for DDR2 setting
		if(REG32(SYSREG_MCKG_FREQ_DIV_REG)==0 && max_w_seq_start==0){
			if(bit_loc<8){			//LDQS
				switch(REG32(SYSREG_SYSCLK_CONTROL_REG)&0x1f){
					case 4:			//DRAM 400MHz
						if(max_w_len/4 >= 1)
							w_delay_tap = (max_w_len/4)-1;
						else
							w_delay_tap = max_w_len/4;
						break;
					case 3:			//DRAM 375MHz
					case 2:			//DRAM 350MHz
					default:		//DRAM 300MHz or under.
						w_delay_tap = max_w_len/4;

				}
			}else{		//HDQS
				switch(REG32(SYSREG_SYSCLK_CONTROL_REG)&0x1f){
					w_delay_tap = max_w_len/4;
					break;
					case 3:			//DRAM 375MHz
					w_delay_tap = (max_w_len/4)+2;
					break;
					case 4:			//DRAM 400MHz
					case 2:			//DRAM 350MHz
					default:		//DRAM 300MHz or under.
					w_delay_tap = max_w_len/4;
				}

			}
		}else{
			w_delay_tap = max_w_len/2;
		}
	}else{
		if(REG32(SYSREG_MCKG_FREQ_DIV_REG)==0 && max_w_seq_start==0){
			if(bit_loc<8){			//LDQS
				switch(REG32(SYSREG_SYSCLK_CONTROL_REG)&0x1f){
					case 4:			//DRAM 400MHz
						if(max_w_len/4 >= 3)
							w_delay_tap = (max_w_len/4)-3;
						else
							w_delay_tap = max_w_len/4;
						break;
					case 3:			//DRAM 375MHz
					case 2:			//DRAM 350MHz
					default:		//DRAM 300MHz or under.
						if(max_w_len/4 >= 2)
							w_delay_tap = (max_w_len/4)-2;
						else
							w_delay_tap = max_w_len/4;

				}
			}else{		//HDQS
				switch(REG32(SYSREG_SYSCLK_CONTROL_REG)&0x1f){
					case 4:			//DRAM 400MHz
						if(max_w_len/4 >= 4)
							w_delay_tap = (max_w_len/4)-4;
						else
							w_delay_tap = max_w_len/4;
						break;
					case 3:			//DRAM 375MHz
					case 2:			//DRAM 350MHz
					default:		//DRAM 300MHz or under.
						if(max_w_len/4 >= 2)
							w_delay_tap = (max_w_len/4)-2;
						else
							w_delay_tap = max_w_len/4;

				}

			}
		}else{
			w_delay_tap = max_w_len/2;
		}
	}
#endif //set write dq delay taps

	//for DDR2, DDR3 Read delay tap
	ps_mul = mc_akh_rx_win_sel_mul(1);
	ps_div = mc_akh_rx_win_sel_div(2);

	if(max_r_len>20){
		r_delay_tap = ((max_r_len*ps_mul) + (ps_div/2)) / ps_div;
	}else if(max_r_len>12 && max_r_seq_start==0){
		r_delay_tap = max_r_len-12;
	}else if(max_r_seq_start!=0){
		r_delay_tap = ((max_r_len*ps_mul) + (ps_div/2)) / ps_div;
	}else{
		r_delay_tap=0;
	}

#if 0 //we currently don't set write DQ delay taps in new mem-controller
	*ddcrdqr_base = (((max_w_seq_start + w_delay_tap) & 0x1f) << 24) | \
					(((max_r_seq_start + max_r_len - 1) & 0x1f) << 16) | \
					(((max_r_seq_start + (r_delay_tap)) & 0x1f) << 8) | \
					(((max_r_seq_start) & 0x1f) << 0);
#else
	*ddcrdqr_base = (((max_r_seq_start + max_r_len - 1) & 0x1f) << 16) | \
		(((max_r_seq_start + (r_delay_tap)) & 0x1f) << 8) | \
		(((max_r_seq_start) & 0x1f) << 0);
#endif

	_memctl_update_phy_param();
    printf("(0x%x) = 0x%x\n",ddcrdqr_base,*ddcrdqr_base);

	return;
}

//2014-10-23: Re-write the read delay window search algorithm for reducing stack size.
#if 0
SECTION_AUTOK
static void _memctl_result_to_DQ_RW_Array(u32_t result, u32_t w_delay, u32_t r_delay, u32_t RW_array_addr[32][32])
{
    /*
     * RW_array_addr [32]   [32] : [Rising 0~15, Falling 0~15] [w_delay]
     *              32bit  W_delay
     */

    u32_t bit_loc, correct_bit;


    /* We mark correct bit */
    result = ~result;

    if( (*((u32_t *)DCR) & 0x0f000000) == 0){ /* 8bit mode */
        result = ((result & 0xFF000000) >> 24) | ((result & 0x00FF0000)) | ((result & 0x0000FF00) >> 8) | ((result & 0x000000FF) << 16);
    }else{ /* 16bit mode */
        result = ((result & 0xFFFF0000) >> 16) | ((result & 0x0000FFFF) << 16);
    }



    for(bit_loc=0; bit_loc < 32; bit_loc++){
        correct_bit = (result >> bit_loc) & 0x1;
        RW_array_addr[bit_loc][w_delay] |= (correct_bit << r_delay);
        //printf("correct_bit(%d), RW_array_addr[%d][%d] = 0x%08x, bit_loc(%d)\n", correct_bit, bit_loc, w_delay, RW_array_addr[bit_loc][w_delay], bit_loc);
    }

    return;
}

SECTION_AUTOK
static u32_t _memctl_find_proper_RW_dealy(u32_t resolution, u32_t w_start, u32_t r_start, u32_t DQ_RW_Array[32][32])
{
    u32_t max_r_seq_start, max_r_len, r_delay, r_seq_start, r_len;
    u32_t max_w_seq_start, max_w_len, w_delay, w_seq_start, w_len, search_seq_start, bit_loc;
    u32_t bit_fail, mode_16bit=0;


    if(REG32(DCR) & 0x0F000000){
        mode_16bit = 1;
    }else{
        mode_16bit = 0;
    }

    bit_fail = 0;
    for(bit_loc = 0; bit_loc < 32; bit_loc++){
        if(mode_16bit == 0){
            if((bit_loc > 7) && (bit_loc < 16))
                    continue;
            if((bit_loc > 23) && (bit_loc < 32))
                    continue;
    }

        max_r_len = 0;
        max_r_seq_start = 0;
        max_w_len = 0;
        max_w_seq_start = 0;
        /* Searching for the max. sequetial read window. */
        for(w_delay = w_start; w_delay < 32; w_delay+=resolution){
            r_len = 0;
            r_seq_start = 0;
            search_seq_start = 1;
            for(r_delay = r_start; r_delay < 32; r_delay+=resolution){
                if(search_seq_start == 1){
                    if( (DQ_RW_Array[bit_loc][w_delay] >> r_delay) & 0x1 ){
                        r_seq_start = r_delay;
                        search_seq_start = 0;
                    }
                    if( (r_delay+resolution) >= 31 ){
                        r_len = 1;
                        if(r_len > max_r_len){
                            max_r_len = r_len;
                            max_r_seq_start = r_seq_start;
                            r_len = 0;
                            r_seq_start = r_delay + resolution;
                        }
                    }
                }else{
                    if( 0 == ((DQ_RW_Array[bit_loc][w_delay] >> r_delay) & 0x1) ){
                        r_len = r_delay - r_seq_start - resolution + 1;
                        if(r_len > max_r_len){
                                max_r_len = r_len;
                                max_r_seq_start = r_seq_start;
                                r_len = 0;
                                r_seq_start = r_delay + resolution;
                        }
                        search_seq_start = 1;
                    }else{
                        if((r_delay+resolution)  >= 31){
                            r_len = r_delay - r_seq_start + 1;
                            if(r_len > max_r_len){
                                max_r_len = r_len;
                                max_r_seq_start = r_seq_start;
                                r_len = 0;
                                r_seq_start = r_delay + resolution;
                            }
                        }
                    }
                }
            }
        }


        w_len = 0;
        w_seq_start = 0;
        search_seq_start = 1;
        /* Searching for the max. write delay window basing on max. read delay window. */
        for(r_delay = max_r_seq_start ; r_delay < (max_r_seq_start + max_r_len) ; r_delay += resolution){
            w_len = 0;
            w_seq_start = 0;
            search_seq_start = 1;
            for(w_delay = w_start; w_delay < 32; w_delay+=resolution){
                if(search_seq_start == 1){
                    if( (DQ_RW_Array[bit_loc][w_delay] >> r_delay) & 0x1 ){
                        w_seq_start = w_delay;
                        search_seq_start = 0;
                    }
                    if( (w_delay+resolution) >= 31 ){
                        w_len = 1;
                        if(w_len > max_w_len){
                            max_w_len = w_len;
                            max_w_seq_start = w_seq_start;
                            w_len = 0;
                            w_seq_start = w_delay + resolution;
                        }
                    }
                }else{
                    if( 0 == ((DQ_RW_Array[bit_loc][w_delay] >> r_delay) & 0x1) ){
                        w_len = w_delay - w_seq_start - resolution + 1;
                        if(w_len > max_w_len){
                            max_w_len = w_len;
                            max_w_seq_start = w_seq_start;
                            w_len = 0;
                            w_seq_start = w_delay + resolution;
                        }
                        search_seq_start = 1;
                    }else{
                        if((w_delay+resolution)  >= 31){
                            w_len = w_delay - w_seq_start + 1;
                            if(w_len > max_w_len){
                                max_w_len = w_len;
                                max_w_seq_start = w_seq_start;
                                w_len = 0;
                                w_seq_start = w_delay + resolution;
                            }
                        }
                    }
                }
            }
        }

        printf("bit:%02d, max_r_s(%d), max_r_l(%d), max_w_s(%d), max_w_len(%d)\n", bit_loc, max_r_seq_start, max_r_len,  max_w_seq_start, max_w_len);

		//write back the calibrated delay taps
		_memctl_set_phy_delay_dqrf(bit_loc, max_w_seq_start, max_w_len, max_r_seq_start, max_r_len);

#if 1 //mark for new preloader
		if((max_w_len <= MEMCTL_CALI_MIN_WRITE_WINDOW) || (max_r_len <= MEMCTL_CALI_MIN_READ_WINDOW)){
			printf("\nII: small max_w_len=%d max_r_len=%d, bit_loc=%d\n",max_w_len, max_r_len, bit_loc);
			bit_fail |= (1 << bit_loc);
			printf("EE: Window is too small...\n");
			SYSTEM_RESET();
		}
#endif
    }


    return bit_fail;
}


SECTION_AUTOK
static u32_t _DDR_Calibration_Full_Scan(u32_t target_addr, u32_t len, u32_t resolution, u32_t w_start, u32_t r_start)
{
    u32_t w_delay, r_delay, WR_Result;
    u32_t DQ_RW_Array[32][32];

    /* Initialize DQ_RW_Array */
    for(w_delay = 0; w_delay < 32; w_delay++){
            for(r_delay = 0; r_delay < 32; r_delay++){
                    DQ_RW_Array[w_delay][r_delay] = 0;
            }
    }

	for(w_delay = w_start; w_delay < 32; w_delay += resolution){
		for(r_delay = r_start; r_delay < 32; r_delay += resolution){
			_memctl_set_phy_delay_all(w_delay, r_delay);
			_write_pattern_1(target_addr, len);
			memctl_sync_write_buf();
			WR_Result = _verify_pattern_1(target_addr, len);

			_memctl_result_to_DQ_RW_Array(WR_Result, w_delay, r_delay, &DQ_RW_Array[0][0]);
		}
	}

    /* All scan result is in DQ_RW_Array, choose a proper delay tap setting. */
    if( 0 == _memctl_find_proper_RW_dealy( resolution, w_start, r_start, DQ_RW_Array)){
        return 0;/* Pass */
    }else{
        return 1;/* Fali */
    }

}
#endif

SECTION_AUTOK
static u32_t __DDR_Calibration_Full_Scan_Window(u32_t target_addr, u32_t len, u32_t resolution, u32_t w_start, u32_t r_start)
{
    u8_t max_r_seq_start_array[32], max_r_len_array[32];
    u8_t max_w_seq_start_array[32], max_w_len_array[32];
    u8_t seq_start_array[32];
    u8_t search_seq_start_array[32];
    u8_t w_delay, r_delay, bit_loc;
    u8_t is_this_bit_correct;
    u8_t mode_16bit;
    u8_t max_r_seq_start, max_r_len;
    u8_t max_w_seq_start, max_w_len;
    u8_t r_seq_start, r_len;
    u8_t w_seq_start, w_len;
    u8_t search_seq_start;
    u32_t WR_Result;

    if(REG32(DCR) & 0x0F000000){
        mode_16bit = 1;
    }else{
        mode_16bit = 0;
    }


    /**************************************************************
         ************ Searching for the max. sequetial read window.***********
         **************************************************************/
    for(bit_loc = 0; bit_loc < 32; bit_loc++){
        max_r_seq_start_array[bit_loc] = 0;
        max_r_len_array[bit_loc]       = 0;
    }

    for(w_delay = w_start; w_delay < 32; w_delay += resolution){
        //For each w_delay, it is a new search for each w_delay, so that reset the relative information to initial state.
        for(bit_loc = 0; bit_loc < 32; bit_loc++){
            seq_start_array[bit_loc]       = 0;
            search_seq_start_array[bit_loc]= 1;
        }

        for(r_delay = r_start; r_delay < 32; r_delay += resolution){
            _memctl_set_phy_delay_all(w_delay, r_delay);
            _write_pattern_1(target_addr, len);
            memctl_sync_write_buf();
            /* We mark correct bit */
            WR_Result = ~(_verify_pattern_1(target_addr, len));

            if( mode_16bit == 0){ /* 8bit mode */
                WR_Result = ((WR_Result & 0xFF000000) >> 24) | ((WR_Result & 0x00FF0000)) | ((WR_Result & 0x0000FF00) >> 8) | ((WR_Result & 0x000000FF) << 16);
            }else{ /* 16bit mode */
                WR_Result = ((WR_Result & 0xFFFF0000) >> 16) | ((WR_Result & 0x0000FFFF) << 16);
            }

            for(bit_loc = 0; bit_loc < 32; bit_loc++){
                if(mode_16bit == 0){
                    if((bit_loc > 7) && (bit_loc < 16))
                        continue;
                    if((bit_loc > 23) && (bit_loc < 32))
                        continue;
                }

                max_r_seq_start  = max_r_seq_start_array[bit_loc];
                max_r_len        = max_r_len_array[bit_loc];
                r_seq_start      = seq_start_array[bit_loc];
                search_seq_start = search_seq_start_array[bit_loc];
                is_this_bit_correct = ((WR_Result>>bit_loc) & 0x1);

                if(search_seq_start == 1){
                    if(is_this_bit_correct == 1){
                        r_seq_start = r_delay;
                        search_seq_start = 0;
                    }
                    if( (r_delay+resolution) >= 31 ){
                        r_len = 1;
                        if(r_len > max_r_len){
                            max_r_len = r_len;
                            max_r_seq_start = r_seq_start;
                            r_len = 0;
                            r_seq_start = r_delay + resolution;
                        }
                    }
                }else{
                    if(is_this_bit_correct == 0){
                        r_len = r_delay - r_seq_start - resolution + 1;
                        if(r_len > max_r_len){
                            max_r_len = r_len;
                            max_r_seq_start = r_seq_start;
                            r_len = 0;
                            r_seq_start = r_delay + resolution;
                        }
                        search_seq_start = 1;
                    }else{
                        if((r_delay+resolution) >= 31){
                            r_len = r_delay - r_seq_start + 1;
                            if(r_len > max_r_len){
                                max_r_len = r_len;
                                max_r_seq_start = r_seq_start;
                                r_len = 0;
                                r_seq_start = r_delay + resolution;
                            }
                        }
                    }
                }

                if((max_r_len >= max_r_len_array[bit_loc])&&(max_r_seq_start >= max_r_seq_start_array[bit_loc])){
                    max_r_seq_start_array[bit_loc] = max_r_seq_start;
                    max_r_len_array[bit_loc]       = max_r_len;
                }
                seq_start_array[bit_loc]     = r_seq_start;
                search_seq_start_array[bit_loc]= search_seq_start;
            }
        }
    }


    /******************************************************************
         Searching for the max. write delay window basing on max. read delay window.
         ******************************************************************/
    for(bit_loc = 0; bit_loc < 32; bit_loc++){
        max_w_seq_start_array[bit_loc] = 0;
        max_w_len_array[bit_loc]       = 0;
    }

    for(r_delay = max_r_seq_start ; r_delay < (max_r_seq_start + max_r_len) ; r_delay += resolution){
        //For each r_delay, it is a new search for each r_delay, so that reset the relative information to initial state.
        for(bit_loc = 0; bit_loc < 32; bit_loc++){
            seq_start_array[bit_loc]       = 0;
            search_seq_start_array[bit_loc]= 1;
        }

         for(w_delay = w_start; w_delay < 32; w_delay+=resolution){
            _memctl_set_phy_delay_all(w_delay, r_delay);
            _write_pattern_1(target_addr, len);
            memctl_sync_write_buf();
            /* We mark correct bit */
            WR_Result = ~(_verify_pattern_1(target_addr, len));

            if( mode_16bit == 0){ /* 8bit mode */
                WR_Result = ((WR_Result & 0xFF000000) >> 24) | ((WR_Result & 0x00FF0000)) | ((WR_Result & 0x0000FF00) >> 8) | ((WR_Result & 0x000000FF) << 16);
            }else{ /* 16bit mode */
                WR_Result = ((WR_Result & 0xFFFF0000) >> 16) | ((WR_Result & 0x0000FFFF) << 16);
            }

            for(bit_loc = 0; bit_loc < 32; bit_loc++){
                if(mode_16bit == 0){
                    if((bit_loc > 7) && (bit_loc < 16))
                        continue;
                    if((bit_loc > 23) && (bit_loc < 32))
                        continue;
                }

                max_w_seq_start  = max_w_seq_start_array[bit_loc];
                max_w_len        = max_w_len_array[bit_loc];
                w_seq_start      = seq_start_array[bit_loc];
                search_seq_start = search_seq_start_array[bit_loc];
                is_this_bit_correct = ((WR_Result>>bit_loc) & 0x1);

                if(search_seq_start == 1){
                    if(is_this_bit_correct == 1){
                        w_seq_start = w_delay;
                        search_seq_start = 0;
                    }
                    if( (w_delay+resolution) >= 31 ){
                        w_len = 1;
                        if(w_len > max_w_len){
                            max_w_len = w_len;
                            max_w_seq_start = w_seq_start;
                            w_len = 0;
                            w_seq_start = w_delay + resolution;
                        }
                    }
                }else{
                    if(is_this_bit_correct == 0){
                        w_len = w_delay - w_seq_start - resolution + 1;
                        if(w_len > max_w_len){
                            max_w_len = w_len;
                            max_w_seq_start = w_seq_start;
                            w_len = 0;
                            w_seq_start = w_delay + resolution;
                        }
                        search_seq_start = 1;
                    }else{
                        if((w_delay+resolution) >= 31){
                            w_len = w_delay - w_seq_start + 1;
                            if(w_len > max_w_len){
                                max_w_len = w_len;
                                max_w_seq_start = w_seq_start;
                                w_len = 0;
                                w_seq_start = w_delay + resolution;
                            }
                        }
                    }
                }

                if((max_w_len >= max_w_len_array[bit_loc])&&(max_w_seq_start >= max_w_seq_start_array[bit_loc])){
                    max_w_seq_start_array[bit_loc] = max_w_seq_start;
                    max_w_len_array[bit_loc]       = max_w_len;
                }
                seq_start_array[bit_loc]     = w_seq_start;
                search_seq_start_array[bit_loc]= search_seq_start;
            }
        }
    }

    for(bit_loc = 0; bit_loc < 32; bit_loc++){
        printf("bit:%02d, max_w_s(%02d), max_w_len(%02d), max_r_s(%02d), max_r_len(%02d): ", bit_loc, max_w_seq_start_array[bit_loc], max_w_len_array[bit_loc], max_r_seq_start_array[bit_loc], max_r_len_array[bit_loc]);
        //write back the calibrated delay taps
        //_memctl_set_phy_delay_dqrf(bit_loc, 0, 0, max_r_seq_start_array[bit_loc], max_r_len_array[bit_loc]);
		_memctl_set_phy_delay_dqrf(bit_loc, max_w_seq_start_array[bit_loc], max_w_len_array[bit_loc], max_r_seq_start_array[bit_loc], max_r_len_array[bit_loc]);
    }
    return 0;
}


SECTION_AUTOK
u32_t DDR_Calibration(unsigned char full_scan)
{
    u32_t target_addr, len, resolution, w_start, r_start;
    u32_t retry_limit;
    target_addr = MEMCTL_CALI_TARGET_ADDR;
    len         = MEMCTL_CALI_TARGET_LEN;
    resolution  = MEMCTL_CALI_FULL_SCAN_RESOLUTION;
    w_start     = MEMCTL_CALI_WRITE_DELAY_START_TAP;
    r_start     = MEMCTL_CALI_READ_DELAY_START_TAP;

#if 0 //we don't need this on 9601B, proved by the measurement of signal waveforms
    if(GET_MEM_MHZ() >= 300){
        /* Enable dynamic PHY FIFO Reset & DQS half clock cycle */
        //REG32(DACCR)= REG32(DACCR) | 0x20;
        //REG32(DIDER) = REG32(DIDER) | 0x80800000;
    }
#endif

    retry_limit = 0;

    /* Do a fully scan to choose a proper point. */
	if(full_scan){
#if 1
        while( 0 != __DDR_Calibration_Full_Scan_Window(target_addr, len, resolution, w_start, r_start)){
#else
    	while( 0 != _DDR_Calibration_Full_Scan(target_addr, len, resolution, w_start, r_start)){
#endif
	        /* Base on the proper point, we do a one dimension scan for the reliabilties. */
	        retry_limit++;
	        if(retry_limit > MEMCTL_CALI_RETRY_LIMILT){
				printf("%s-%d full scan Fail, need to define failure status. retry_limit:%d\n",retry_limit);
	        	return 1; /* Fail, need to define failure status. */
	        }
	    }
	}
    return 0;
}

