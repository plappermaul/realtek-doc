//#include <asm/arch/bspchip.h>
//#include <asm/arch/rtk_soc_common.h>
//#include <asm/arch/memctl.h>
//#include "plr_dram_gen2_memctl.h"
#include <preloader.h>
#include "plr_dram_gen2.h"
#include "plr_dram_gen2_memctl.h"
#include "plr_plat_dep.h"

u32_t pll_query_freq(u32_t);

/*
 * Constant definition
 */

//typedef unsigned int u32_t;
/*  40p per tap delay, real 25p per tap.
             tDS    tDH
  DDR2 400   150p   275p
  DDR2 533   100p   225p
  DDR2 677   100p   175p
  DDR2 800   50p    125p
  DDR3 800   75p    150p

*/
#define MEMCTL_CALI_RETRY_LIMILT            (5)
#define MEMCTL_CALI_MIN_READ_WINDOW                     (7)
#define MEMCTL_CALI_MIN_WRITE_WINDOW                    (7)
#define CPU_DCACHE_SIZE                                 (0x8000)
#define MEMCTL_CALI_TARGET_LEN                          (CPU_DCACHE_SIZE * 2)
#define MEMCTL_CALI_FULL_SCAN_RESOLUTION                (2)
#define MEMCTL_CALI_WRITE_DELAY_START_TAP               (0)
#define MEMCTL_CALI_READ_DELAY_START_TAP                (0)
#define MEMCTL_DATA_PATTERN_8BIT                        (0x00FF00FF)
#define MEMCTL_DATA_PATTERN_16BIT                       (0x0000FFFF)
#define MEMCTL_CALI_TARGET_ADDR                         (0x80000000)
//#define _memctl_DCache_flush_invalidate         rlx5281_cache_flush_dcache
#define _memctl_DCache_flush_invalidate         plat_memctl_dcache_flush

/*
 * Function Declaration
 */

/* return 32bit verify status */

u32_t _verify_pattern(u32_t start_addr, u32_t len);
void _write_pattern(u32_t start_addr, u32_t len);
u32_t _memctl_set_phy_delay(u32_t bit_location, u32_t w_delay, u32_t r_delay);
void _memctl_set_phy_delay_all(u32_t w_delay, u32_t r_delay);
void _memctl_result_to_DQ_RW_Array(u32_t result, u32_t w_delay, u32_t r_delay, u32_t RW_array_addr[32][32]);
void _memctl_dq_rw_array_post_processing(u32_t w_start, u32_t r_start, u32_t resolution, u32_t **DQ_RW_Array);
void  _memctl_set_phy_delay_dqrf(u32_t bit_loc,u32_t max_w_seq_start,u32_t max_w_len,u32_t max_r_seq_start,u32_t max_r_len);
u32_t _memctl_find_proper_RW_dealy(u32_t resolution, u32_t w_start, u32_t r_start, u32_t DQ_RW_Array[32][32]);
void memctl_sync_write_buf(void);
extern void _memctl_update_phy_param(void);


/*
 * Function Implementation
 */


void memctl_sync_write_buf(void)
{
    *((volatile unsigned int *)0xB8001038) = 0x80000000;
    while(*((volatile unsigned int *)0xB8001038) & 0x80000000);
    return;
}

void _memctl_set_phy_delay_all(u32_t w_delay, u32_t r_delay)
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



void  _memctl_set_phy_delay_dqrf(u32_t bit_loc,u32_t max_w_seq_start,u32_t max_w_len,u32_t max_r_seq_start,u32_t max_r_len)
{
    volatile u32_t *ddcrdqr_base;

    ddcrdqr_base = (u32_t *)0xB8001510;

    ddcrdqr_base += bit_loc;

    if(get_memory_memctl_dq_write_delay_parameters(&bit_loc)){
       max_w_len = 0;
       max_w_seq_start = bit_loc;
    }

    *ddcrdqr_base = plat_memctl_calculate_dqrf_delay(max_w_seq_start, max_w_len, max_r_seq_start, max_r_len);

    _memctl_update_phy_param();

    return;
}

const u32_t pat_ary[] = {
            0x00010000, 0x01234567, 0x00000000, 0x76543210,
            0xFFFFFFFF, 0x89abcdef, 0x0000FFFF, 0xfedcba98,
            0xFFFF0000, 0x00FF00FF, 0xFF00FF00, 0xF0F0F0F0,
            0x0F0F0F0F, 0x5A5AA5A5, 0xA5A55A5A, 0x5A5AA5A5,
            0xA5A55A5A, 0xA5A55A5A, 0x5A5AA5A5, 0xA5A55A5A,
            0x5A5AA5A5, 0x5555AAAA, 0xAAAA5555, 0x5555AAAA,
            0xAAAA5555, 0xAAAA5555, 0x5555AAAA, 0xAAAA5555,
            0x5555AAAA, 0xCC3333CC, 0x33CCCC33, 0xCCCC3333
           };


/* Data structures used for testing unaligned load/store operations. */
typedef struct{
	unsigned char c1;
	unsigned int w;
} t_off_1;
typedef struct{
	unsigned char c1;
	unsigned char c2;
	unsigned int w;
} t_off_2;
typedef struct{
	unsigned char c1;
	unsigned char c2;
	unsigned char c3;
	unsigned int w;
} t_off_3;


void _write_pattern_uls_off3B(u32_t start_addr, u32_t len)
{
    volatile u32_t *p_start, data_tmp __attribute__((unused));
    volatile t_off_3 *off3;
    u32_t i, ary_i = 0;

    /* In case of write through D-Cache mechanisim, read data in DCache */
    p_start = (volatile u32_t *)start_addr;
    for(i = 0; i < len; i += sizeof(u32_t)){
            data_tmp = *p_start;
    }

        /* Write data: offset 3 bytes */
	off3 = (t_off_3 *)start_addr;
	for(i=0;i<(len/sizeof(t_off_3));i++){
		off3[i].c1 = 0xcc;
		off3[i].c2 = 0xdd;
		off3[i].c3 = 0xee;
		off3[i].w = pat_ary[ary_i];
		ary_i = (ary_i+1) % (sizeof(pat_ary)/sizeof(u32_t));
	}
	_memctl_DCache_flush_invalidate();
	return;
}


u32_t _verify_pattern_uls_off3B(u32_t start_addr, u32_t len)
{
	volatile t_off_3 *off3;
	u32_t i, ary_i, err_result = 0;

       _memctl_DCache_flush_invalidate();

	/* check data: offset 3 bytes */
	off3 = (t_off_3 *)start_addr;
	ary_i = 0;
	for(i=0;i<(len/sizeof(t_off_3));i++){
		if(off3[i].w != pat_ary[ary_i] || (off3[i].c1 != 0xcc) || (off3[i].c2 != 0xdd) || (off3[i].c3 != 0xee)){
			err_result = 0xffffffff;
			goto end;
		}
		ary_i = (ary_i+1) % (sizeof(pat_ary)/sizeof(u32_t));		
	}
end:	
	return err_result;
}


void _write_pattern_uls_off2B(u32_t start_addr, u32_t len)
{
    volatile u32_t *p_start, data_tmp __attribute__((unused));
    volatile t_off_2 *off2;
    u32_t i, ary_i = 0;

    /* In case of write through D-Cache mechanisim, read data in DCache */
    p_start = (volatile u32_t *)start_addr;
    for(i = 0; i < len; i += sizeof(u32_t)){
            data_tmp = *p_start;
    }

        /* Write data: offset 2 bytes */
	off2 = (t_off_2 *)start_addr;
	for(i=0;i<(len/sizeof(t_off_2));i++){
		off2[i].c1 = 0xcc;
		off2[i].c2 = 0xdd;
		off2[i].w = pat_ary[ary_i];
		ary_i = (ary_i+1) % (sizeof(pat_ary)/sizeof(u32_t));
	}
	_memctl_DCache_flush_invalidate();
	return;
}


u32_t _verify_pattern_uls_off2B(u32_t start_addr, u32_t len)
{
	volatile t_off_2 *off2;
	u32_t i, ary_i, err_result = 0;

       _memctl_DCache_flush_invalidate();

	/* check data: offset 2 bytes */
	off2 = (t_off_2 *)start_addr;
	ary_i = 0;
	for(i=0;i<(len/sizeof(t_off_2));i++){
		if(off2[i].w != pat_ary[ary_i] || (off2[i].c1 != 0xcc) || (off2[i].c2 != 0xdd)){
			err_result = 0xffffffff;
			goto end;
		}
		ary_i = (ary_i+1) % (sizeof(pat_ary)/sizeof(u32_t));		
	}
end:	
	return err_result;
}


void _write_pattern_uls_off1B(u32_t start_addr, u32_t len)
{
    volatile u32_t *p_start, data_tmp __attribute__((unused));
    volatile t_off_1 *off1;
    u32_t i, ary_i = 0;

    /* In case of write through D-Cache mechanisim, read data in DCache */
    p_start = (volatile u32_t *)start_addr;
    for(i = 0; i < len; i += sizeof(u32_t)){
            data_tmp = *p_start;
    }

        /* Write data: offset 1 bytes */
	off1 = (t_off_1 *)start_addr;
	for(i=0;i<(len/sizeof(t_off_1));i++){
		off1[i].c1 = 0xcc;
		off1[i].w = pat_ary[ary_i];
		ary_i = (ary_i+1) % (sizeof(pat_ary)/sizeof(u32_t));
	}
	_memctl_DCache_flush_invalidate();
	return;
}


u32_t _verify_pattern_uls_off1B(u32_t start_addr, u32_t len)
{
	volatile t_off_1 *off1;
	u32_t i, ary_i, err_result = 0;

       _memctl_DCache_flush_invalidate();

	/* check data: offset 1 bytes */
	off1 = (t_off_1 *)start_addr;
	ary_i = 0;
	for(i=0;i<(len/sizeof(t_off_1));i++){
		if((off1[i].w != pat_ary[ary_i]) || (off1[i].c1 != 0xcc)){
			err_result = 0xffffffff;
			goto end;
		}
		ary_i = (ary_i+1) % (sizeof(pat_ary)/sizeof(u32_t));		
	}
end:	
	return err_result;
}

void _write_pattern_1(u32_t start_addr, u32_t len)
{
    volatile u32_t *p_start, data_tmp __attribute__((unused));
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


u32_t _verify_pattern_1(u32_t start_addr, u32_t len)
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



void _write_pattern(u32_t start_addr, u32_t len)
{
    volatile u32_t *p_start, data_tmp __attribute__((unused));
    u32_t b_len;
    u32_t data_8bit, data_16bit;

    data_8bit  = MEMCTL_DATA_PATTERN_8BIT;
    data_16bit = MEMCTL_DATA_PATTERN_16BIT;

    /* In case of write through D-Cache mechanisim, read data in DCache */
    p_start = (volatile u32_t *)start_addr;
    for(b_len = 0; b_len < len; b_len += sizeof(u32_t)){
        data_tmp = *p_start;
    }

    /* Write 8bit data */
    p_start = (volatile u32_t *)start_addr;
    for(b_len = 0; b_len < (len/2); b_len += sizeof(u32_t)){
        *p_start = data_8bit;
        p_start++;
    }

    /* Write 16bit data */
    for(b_len = 0; b_len < (len/2); b_len += sizeof(u32_t)){
        *p_start = data_16bit;
        p_start++;
    }

    _memctl_DCache_flush_invalidate();

    return;
}
u32_t _verify_pattern(u32_t start_addr, u32_t len)
{
    volatile u32_t *p_start, data_tmp;
    u32_t b_len, err_result;
    u32_t data_8bit, data_16bit;

    _memctl_DCache_flush_invalidate();

    data_8bit  = MEMCTL_DATA_PATTERN_8BIT;
    data_16bit = MEMCTL_DATA_PATTERN_16BIT;

    err_result = 0;

    /* Read 8bit data */
    p_start = (volatile u32_t *)start_addr;
    for(b_len = 0; b_len < (len/2); b_len += sizeof(u32_t)){
        data_tmp = *p_start;
        err_result = err_result | ( (data_8bit | data_tmp) & ( ~(data_8bit & data_tmp)));
        if(err_result == 0xffffffff)
                return err_result;
        p_start++;
    }

    /* Read 16bit data */
    for(b_len = 0; b_len < (len/2); b_len += sizeof(u32_t)){
        data_tmp = *p_start;
        err_result = err_result | ( (data_16bit | data_tmp) & ( ~(data_16bit & data_tmp)));
        if(err_result == 0xffffffff)
                return err_result;
        p_start++;
    }


    return err_result;
}


void _memctl_result_to_DQ_RW_Array(u32_t result, u32_t w_delay, u32_t r_delay, u32_t RW_array_addr[32][32])
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

u32_t _memctl_find_proper_RW_dealy(u32_t resolution, u32_t w_start, u32_t r_start, u32_t DQ_RW_Array[32][32])
{
    u32_t max_r_seq_start, max_r_len, r_delay, r_seq_start, r_len;
    u32_t max_w_seq_start, max_w_len, w_delay, w_seq_start, w_len, search_seq_start, bit_loc;
    u32_t bit_fail, wr_bit_fail, mode_16bit;


    if(REG32(DCR) & 0x0F000000){
        mode_16bit = 1;
    }else{
        mode_16bit = 0;
    }

    bit_fail = wr_bit_fail = 0;
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
        //printf("bit(%d):\n", bit_loc);
        /* Searching for the max. sequetial read window. */
        for(w_delay = w_start; w_delay < 32; w_delay+=resolution){
            r_len = 0;
            r_seq_start = 0;
            search_seq_start = 1;
            //printf("   w(%d) %08x\n", w_delay, DQ_RW_Array[bit_loc][w_delay]);
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

        //printf("bit:%02d, max_r_s(%d), max_r_l(%d), max_w_s(%d), max_w_len(%d)\n", bit_loc, max_r_seq_start, max_r_len,  max_w_seq_start, max_w_len);
        _memctl_set_phy_delay_dqrf(bit_loc, max_w_seq_start, max_w_len, max_r_seq_start, max_r_len);

        //if((max_w_len <= MEMCTL_CALI_MIN_WRITE_WINDOW) || (max_r_len <= MEMCTL_CALI_MIN_READ_WINDOW)){
        if((max_w_len <= MEMCTL_CALI_MIN_WRITE_WINDOW) ){
            //printf("\nII: small max_w_len=%d, bit_loc=%d\n", max_w_len, bit_loc);
            wr_bit_fail |= (1 << bit_loc);
        }
        if(max_r_len <= MEMCTL_CALI_MIN_READ_WINDOW){
            //printf("\nII: small max_r_len=%d, bit_loc=%d\n", max_r_len, bit_loc);
            bit_fail |= (1 << bit_loc);
        }
    }

	if(0xff00ff00==bit_fail) {
        //printf("II: this might be 8-bit\n");
        bit_fail=0x0;	
    } 
    if((0!=bit_fail)||(0!=wr_bit_fail)){
        printf("\nEE: calibration RD-0x%08x, WR-0x%08x", bit_fail, wr_bit_fail);
    }
    
    return bit_fail;
}

u32_t _DDR_Calibration_Full_Scan(u32_t target_addr, u32_t len, u32_t resolution, u32_t w_start, u32_t r_start)
{
    u32_t w_delay, r_delay, WR_Result;
    u32_t DQ_RW_Array[32][32];

    //printf("DQ_RW_Array: 0x%08p\n", &DQ_RW_Array[0][0]);

    /* Initialize DQ_RW_Array */
    for(w_delay = 0; w_delay < 32; w_delay++){
            for(r_delay = 0; r_delay < 32; r_delay++){
                    DQ_RW_Array[w_delay][r_delay] = 0;
            }
    }


    /* Fully scan whole delay tap. */
    for(w_delay = w_start; w_delay < 32; w_delay += resolution){
        for(r_delay = r_start; r_delay < 32; r_delay += resolution){
            _memctl_set_phy_delay_all(w_delay, r_delay);

            _write_pattern_uls_off1B(target_addr, len);
            memctl_sync_write_buf();
            WR_Result = _verify_pattern_uls_off1B(target_addr, len);

            _write_pattern_uls_off2B(target_addr, len);
            memctl_sync_write_buf();
            WR_Result = _verify_pattern_uls_off2B(target_addr, len);

            _write_pattern_uls_off3B(target_addr, len);
            memctl_sync_write_buf();
            WR_Result = _verify_pattern_uls_off3B(target_addr, len);

            _write_pattern_1(target_addr, len);
            memctl_sync_write_buf();
            WR_Result = _verify_pattern_1(target_addr, len);

            _memctl_result_to_DQ_RW_Array(WR_Result, w_delay, r_delay, DQ_RW_Array);
        }
    }

#if 0
    /* Resolution post processing. */
    if(resolution > 1){
            _memctl_dq_rw_array_post_processing(w_start, r_start, resolution, &DQ_RW_Array[0][0]);
    }
#endif

    /* All scan result is in DQ_RW_Array, choose a proper delay tap setting. */
    if( 0 == _memctl_find_proper_RW_dealy( resolution, w_start, r_start, DQ_RW_Array)){
        return 0;/* Pass */
    }else{
        return 1;/* Fali */
    }
}
#if 0
u32_t _DDR_Calibratoin_One_Dimension(void)
{

    /* 0. Prepare data pattern */
    /* 1. Scan Read delay */
    /* 2. Scan Write delay */
    /* 3. Make decision */


    return 0;/* Pass */
}
#endif

void _memctl_disable_hw_auto_cali(void)
{
    unsigned int i;
    volatile unsigned int *dacspcr, *dacdqr;

    dacspcr = (volatile unsigned int *)DACSPCR;
    dacdqr = (volatile unsigned int *)DACDQR;
    
    *dacspcr &= (~(1<<31));

    for(i=0;i<32;i++){
        *dacdqr &= (~(1<<31));
        dacdqr++;
    }

    _memctl_update_phy_param();

    return;
}


void _memctl_enable_hw_auto_cali(void)
{
    unsigned int i, sil_addr, sil_pat, c_max, c_min, cur_tap;
    unsigned int all_dq_c_max, all_dq_c_min;
    volatile unsigned int *dacspcr, *dacdqr, *d3zqccr;
    volatile unsigned int *dacspar, *pdata;

    /* 0. Set Register parameters */
    dacspcr = (volatile unsigned int *)DACSPCR;
    dacdqr  = (volatile unsigned int *)DACDQR;
    dacspar = (volatile unsigned int *)DACSPAR;
    d3zqccr = (volatile unsigned int *)D3ZQCCR;
    
    /* 1. Silence pattern address */
    sil_addr = DRAMI.dacspar;
    if(((sil_addr >= 0x80000000) && (sil_addr < 0x90000000)) 
    || ((sil_addr >= 0xa0000000) && (sil_addr < 0xb0000000))){
    }else{
        sil_addr = 0xa2000000;
    }
    *dacspar = sil_addr;

    /* 2. Silence pattern */
    /* Pattern 1: 0x5555aaaa/16bit 0x55aa55aa/8bit  
       Pattern 2: 0xffff0000/16bit 0xff00ff00/8bit */
    if(REG32(DCR) & 0x01000000) {
        //sil_pat = 0x5555aaaa;
        sil_pat = 0xffff0000;
    }else{
        //sil_pat = 0x55aa55aa;
        sil_pat = 0xff00ff00;
    }
    
    /* 3. Write silence pattern to DRAM */
    /* DDR2/8bit:  burst=4, data width=8bit  --> 4*1=4bytes 
       DDR2/16bit: burst=4, data width=16bit --> 4*2=8bytes 
       DDR3/8bit:  burst=8, data width=8bit  --> 8*1=8bytes 
       DDR3/16bit: burst=8, data width=16bit --> 8*2=16bytes
       We write slient pattern for 16bytes for covering above cases  */
    pdata = (volatile unsigned int *)(sil_addr | 0x20000000);
    for(i=0;i<4;i++){
        *pdata = sil_pat;
        pdata++;
    }

    /* 4. Enable short ZQ */
    if(memctlc_is_DDR3()){
        *d3zqccr |= (1<<30);
    }else{
        *d3zqccr &= (~(1<<30));
    }

    /* 5. Configure and enable all DQ auto calibration */
    all_dq_c_max = 10;/* The value is obtained by experiments */
    all_dq_c_min = 10;/* The value is obtained by experiments */

    for(i=0;i<32;i++){
        /* extract current */
        cur_tap = (*dacdqr & 0x0000FF00) >> 8;
        if((cur_tap+all_dq_c_max)>31){
            c_max = 31 << 16;
        }else{
            c_max = ((cur_tap+all_dq_c_max)<<16);
        }
        if(all_dq_c_min > cur_tap){
            c_min = 0;
        }else{
            c_min = (cur_tap-all_dq_c_min);
        }
        cur_tap = cur_tap << 8;

        *dacdqr = (1<<31) | ((*dacdqr & 0xFF000000) | c_max | cur_tap | c_min);

        dacdqr++;
    }

    /* 6. Enable auto calibraton control */
    *dacspcr = DRAMI.dacspcr; 

    /* 7. Update PHY parameter */
    _memctl_update_phy_param();

    return;
}

u32_t DDR_Calibration(void)
{
    u32_t target_addr, len, resolution, w_start, r_start;
    u32_t retry_limit;
    target_addr = MEMCTL_CALI_TARGET_ADDR;
    len         = MEMCTL_CALI_TARGET_LEN;
    resolution  = MEMCTL_CALI_FULL_SCAN_RESOLUTION;
    w_start     = MEMCTL_CALI_WRITE_DELAY_START_TAP;
    r_start     = MEMCTL_CALI_READ_DELAY_START_TAP;

    /* Configure Calibation mode: Digital delay line or Analog DLL */
#ifdef CONFIG_ANALOG_DLL_DELAY_LINE
    REG32(DACCR) = REG32(DACCR) & (~(1<<31)) & 0xBFFFFFFF;
#else
    REG32(DACCR) = (REG32(DACCR) | (1<<31)) & 0xBFFFFFFF;
#endif
    //if(board_DRAM_freq_mhz() > 300){
    if(pll_query_freq(PLL_DEV_MEM) >= 300){
        /* Enable dynamic PHY FIFO Reset & DQS half clock cycle */
        REG32(DACCR)= REG32(DACCR) | 0x20;
        REG32(DIDER) = REG32(DIDER) | 0x80800000;
    }

    retry_limit = 0;

    /* Do a fully scan to choose a proper point. */
    while( 0 != _DDR_Calibration_Full_Scan(target_addr, len, resolution, w_start, r_start)){
        /* Base on the proper point, we do a one dimension scan for the reliabilties. */
        //return _DDR_Calibration_One_Dimension();
        retry_limit++;
        if(retry_limit > MEMCTL_CALI_RETRY_LIMILT){
                return 1; /* Fail, need to define failure status. */
        }
    }

    if(DRAMI.dacspcr & MEMCTL_DACSPCR_PERIOD_EN_MASK) {
        _memctl_enable_hw_auto_cali();
    }else{
        _memctl_disable_hw_auto_cali();
    }

    return 0;
}
