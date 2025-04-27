#include <exports.h>
//#include <flash.h>
#include "concur_test.h"
#include "concur_cpu.h"

#define DCACHE_SIZE (0x2000)
#define INIT_VALUE (0xCACACACA)
#define BACKGROUND_VALUE (0xDEADDEAD)
#define GET_SEED 1
#define SET_SEED 0
#define RANDOM_TEST

static unsigned int nor_flash_patterns[] = {  0x00000000, 
			      0xffffffff, 
			      0x55555555, 
		    	      0xaaaaaaaa, 
			      0x01234567, 
			      0x76543210, 
			      0x89abcdef,
			      0xfedcba98,
		   	   };
//#define DIFF_ROWS
//#define USE_BYTESET /* exclusive with DIFF_ROWS */
/*
        get_or_set = GET_SEED: get seed
        get_or_set = SET_SEED: set seed
*/
static void __srandom32(int *a1, int *a2, int *a3, int get_or_set)
{
        static int s1, s2, s3;
        if(GET_SEED==get_or_set){
                *a1=s1;
                *a2=s2;
                *a3=s3;
        }else{
                s1 = *a1;
                s2 = *a2;
                s3 = *a3;
        }
}

static unsigned int __random32(void)
{
#define TAUSWORTHE(s,a,b,c,d) ((s&c)<<d) ^ (((s <<a) ^ s)>>b)
        int s1, s2, s3;
        __srandom32(&s1, &s2, &s3, GET_SEED);

        s1 = TAUSWORTHE(s1, 13, 19, 4294967294UL, 12);
        s2 = TAUSWORTHE(s2, 2, 25, 4294967288UL, 4);
        s3 = TAUSWORTHE(s3, 3, 11, 4294967280UL, 17);

        __srandom32(&s1, &s2, &s3, SET_SEED);

        return (s1 ^ s2 ^ s3);
}

/* return write value */
unsigned int CPU_cache_flush_setting(void)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value, a, b, c;

	/* 
	 * 8198 CPU configuraton:
	 * Dcache size 8KB
	 * ICache 16KB
	 * No L2 Cache
	 * Cache line 8 words
	 */
	write_value = INIT_VALUE;
	a=0x13243;b=0xaaa0bdd;c=0xfffbda0;
	__srandom32(&a, &b, &c, SET_SEED);
	write_value = __random32();
	//printf("\rtimes: %d, pattern: 0x%08x", test_times, write_value);
	pdata = (unsigned int *)TARGET_CPU_CASE_ADDR;
	/* Dirtify DCache */
	for(i=0; i<DCACHE_SIZE; i = i+4)
	{
		*pdata = BACKGROUND_VALUE;
		pdata++;
	}

	pdata = (unsigned int *)CACHED_CPU_CASE_ADDR;
	/* Read data into DCache */
	for(i=0; i<DCACHE_SIZE; i = i+4)
	{
		data = *pdata;
		pdata++;
	}
	

	pdata = (unsigned int *)CACHED_CPU_CASE_ADDR;
	/* Dirtify DCache */
	for(i=0; i<DCACHE_SIZE; i = i+4)
	{
		*pdata = write_value;
		pdata++;
	}
	return write_value;
}


int CPU_cache_flush_checking(unsigned int write_value)
{
	volatile unsigned int *pdata;
	int i;

	pdata = (unsigned int *)CACHED_CPU_CASE_ADDR;
	/* varify the data */
	for(i=0; i<DCACHE_SIZE; i = i+4)
	{
		if((*pdata) != write_value)
		{
			printf("\n %s line: %d pdata(0x%08x) 0x%08x != 0x%08x\n", \
				__FUNCTION__, __LINE__, pdata, *pdata , write_value);
			return CHECKING_FAIL;
		}
		pdata++;
	}

	return CHECKING_PASS;
}
#if 0
#define NOR_FLASH_START_ADDR (0xBD000000)
#define NOR_FLASH_SIZE       (0x10000)
#define NOR_FLASH_END_ADDR   (NOR_FLASH_START_ADDR+NOR_FLASH_SIZE-1)

unsigned int nor_flash_test_setting(unsigned int pattern)
{
    int j;
    volatile unsigned int *start;

       _cache_flush();
        /* write pattern*/
        start = (unsigned int*)TARGET_CPU_CASE_ADDR;
        for(j=0; j < NOR_FLASH_SIZE; j=j+4)
        {
            *start = pattern;
            start++;
        }
	return 0;
}
void nor_flash_test_kickoff(void)
{
#ifndef CONFIG_RTL0371S
        flash_sect_erase(NOR_FLASH_START_ADDR, NOR_FLASH_END_ADDR);
        flash_write(TARGET_CPU_CASE_ADDR, NOR_FLASH_START_ADDR, NOR_FLASH_SIZE);
#endif
}

int nor_flash_test_checking(unsigned int pattern)
{

#ifndef CONFGI_RTL0371S
	int j;
	volatile unsigned int *start;
	unsigned int start_value;

        start = (unsigned int*)(NOR_FLASH_START_ADDR);
        /* check nor flash data */  
        for(j=0; j < NOR_FLASH_SIZE; j=j+4)
        {
	    start_value = (*start);
            if(start_value != pattern)
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        start , start_value, pattern,  __FUNCTION__, __LINE__);
    		return CHECKING_FAIL;
            }
            start++;
        }
#endif
    return CHECKING_PASS;
}
#endif
unsigned int unaligned_patterns[] = {
                              0x00000000,
                              0xffffffff,
                              0x55555555,
                              0xaaaaaaaa,
                              0x01234567,
                              0x76543210,
                              0x89abcdef,
                              0xfedcba98,
                              0xA5A5A5A5,
                              0x5A5A5A5A,
                              0xF0F0F0F0,
                              0x0F0F0F0F,
                              0xFF00FF00,
                              0x00FF00FF,
                              0x0000FFFF,
                              0xFFFF0000,
                              0x00FFFF00,
                              0xFF0000FF,
                              0x5A5AA5A5,
                              0xA5A55A5A,
                           };

#pragma pack(1)
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

#define BYTE_C (0xcc)
#define BYTE_D (0xdd)
#define BYTE_E (0xee)
//#define BYTE_C (0x5a)
//#define BYTE_D (0xa5)
//#define BYTE_E (0x5a)
#pragma packed(4)


void unaligned_test_kickoff(void)
{
	_cache_flush();
	return;
}

unsigned int unaligned_test_setting(unsigned int pattern)
{
	unsigned int i;
	volatile t_off_1 *off1;
	volatile t_off_2 *off2;
	volatile t_off_3 *off3;
	unsigned int dram_start;
	unsigned int dram_size;
	unsigned int area_size;	

	dram_start = TARGET_CPU_CASE_ADDR;
	dram_size =  0x80000;


	/* offset 1 bytes */
	off1 = (t_off_1 *)(dram_start + (dram_size));
	/* set value */
	for(i=0;i<(dram_size/sizeof(t_off_1));i++){
		off1[i].c1 = BYTE_C;
		off1[i].w = pattern;
	}
	/* offset 2 bytes */
	off2 = (t_off_2 *)(dram_start + (2*dram_size));
	/* set value */
	for(i=0;i<(dram_size/sizeof(t_off_2));i++){
		off2[i].c1 = BYTE_C;
		off2[i].c2 = BYTE_D;
		off2[i].w = pattern;
	}
	/* offset 3 bytes */
	off3 = (t_off_3 *)(dram_start + (3*dram_size));
	/* set value */
	for(i=0;i<(dram_size/sizeof(t_off_3));i++){
		off3[i].c1 = BYTE_C;
		off3[i].c2 = BYTE_D;
		off3[i].c3 = BYTE_E;
		off3[i].w = pattern;
	}
	return 0;
}

int unaligned_test_checking(unsigned int pattern)
{
	unsigned int i;
	volatile t_off_1 *off1;
	volatile t_off_2 *off2;
	volatile t_off_3 *off3;
	unsigned int dram_start;
	unsigned int dram_size;
	unsigned int area_size;	

	dram_start = TARGET_CPU_CASE_ADDR;
	dram_size =  0x80000;


	off1 = (t_off_1 *)(dram_start + (dram_size));
	for(i=0;i<(dram_size/sizeof(t_off_1));i++){
		if((off1[i].w != pattern) || (off1[i].c1 != BYTE_C)){
                        printf("addr(0x%08p)(0x%08x) != 0x%08x, addr(0x%08x)(0x%08x) != 0x%08x %s, %d\n",\
				&off1[i].w, off1[i].w, pattern, &off1[i].c1, off1[i].c1, BYTE_C,\
				 __FUNCTION__, __LINE__);
			return CHECKING_FAIL;
		}
	}

	off2 = (t_off_2 *)(dram_start + (2*dram_size));
	for(i=0;i<(dram_size/sizeof(t_off_2));i++){
		if(off2[i].w != pattern || (off2[i].c1 != BYTE_C) || (off2[i].c2 != BYTE_D)){
                        printf("addr(0x%08p)(0x%08x) != 0x%08x, addr(0x%08x)(0x%08x) != 0x%08x \
				, addr(0x%08x)(0x%08x) != 0x%08x %s, %d\n",\
				&off2[i].w, off2[i].w, pattern, &off2[i].c1, off2[i].c1, BYTE_C,\
				&off2[i].c2, off2[i].c2, BYTE_D, __FUNCTION__, __LINE__);
			return CHECKING_FAIL;
		}
	}

	off3 = (t_off_3 *)(dram_start + (3*dram_size));
	for(i=0;i<(dram_size/sizeof(t_off_3));i++){
		if(off3[i].w != pattern ||(off3[i].c1 != BYTE_C) || (off3[i].c2 != BYTE_D)\
			|| (off3[i].c3 != BYTE_E)){
                        printf("addr(0x%08p)(0x%08x) != 0x%08x, addr(0x%08x)(0x%08x) != 0x%08x \
				, addr(0x%08x)(0x%08x) != 0x%08x, addr(0x%08x)(0x%08x) != 0x%08x %s, %d\n",\
				&off3[i].w, off3[i].w, pattern, &off3[i].c1, off3[i].c1, BYTE_C,\
				&off3[i].c2, off3[i].c2, BYTE_D, &off3[i].c3, off3[i].c3, BYTE_E ,\
				__FUNCTION__, __LINE__);
			return CHECKING_FAIL;
		}
	}

	return CHECKING_PASS;

}

void CPU_KICKOFF(int caseNo)
{
        switch (caseNo)
        {
                case 0 ... 7:
                        _cache_flush();
                        break;
		case 8 ... 15:
			unaligned_test_kickoff();
			break;
#if 0
		case 16 ... 25:
			nor_flash_test_kickoff();
			break;
#endif
                default:
                        return NO_SUCH_CASE;
                        break;
        }
        return;
}

/*
 * Dispatch CPU Data setting
 * return value: 0 no such case. others, kick off value */
unsigned int CPU_setting(int caseNo)
{
        switch (caseNo)
        {

                case 0 ... 7:
                        return CPU_cache_flush_setting();
                        break;
		case 8 ... 15:
			return unaligned_test_setting(unaligned_patterns[caseNo-8]);
			break;
#if 0
		case 16 ... 25:
			return nor_flash_test_setting(nor_flash_patterns[caseNo-16]);
			break;
#endif
                default:
                        return NO_SUCH_CASE;
                        break;
        }
        return NO_SUCH_CASE;
}


/*
 * Dispatch CPU Data checking
 * return value: -1 checking data fail. 0 no such case. 1 checking data pased.*/
int CPU_checking(int caseNo, unsigned int param)
{
        switch (caseNo)
        {
                case 0 ... 7:
                        return CPU_cache_flush_checking(param);
			break;
		case 8 ... 15:
			return unaligned_test_checking(unaligned_patterns[caseNo-8]);
			break;
#if 0
		case 16 ... 25:
			return nor_flash_test_checking(nor_flash_patterns[caseNo-16]);
			break;
#endif
                default:
                        return NO_SUCH_CASE;
			break;
        }
}



