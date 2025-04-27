#include "soc.h"
#include "dram/plr_plat_dep.h"
#include "dram/concur_test.h"
#include "concur_cpu.h"

#define INIT_VALUE (0xCACACACA)
#define BACKGROUND_VALUE (0xDEADDEAD)
#define GET_SEED 1
#define SET_SEED 0
//#define RANDOM_TEST
#define printf(...)
//static unsigned int nor_flash_patterns[] = {  0x00000000, 
//                0xffffffff, 
//                0x55555555, 
//                    0xaaaaaaaa, 
//                0x01234567, 
//                0x76543210, 
//                0x89abcdef,
//                0xfedcba98,
//             };
//#define DIFF_ROWS
//#define USE_BYTESET /* exclusive with DIFF_ROWS */
/*
        get_or_set = GET_SEED: get seed
        get_or_set = SET_SEED: set seed
*/
static void __srandom32(unsigned int *a1, unsigned int *a2, unsigned int *a3, int get_or_set)
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
        unsigned int s1, s2, s3;
        __srandom32(&s1, &s2, &s3, GET_SEED);

        s1 = TAUSWORTHE(s1, 13, 19, 4294967294UL, 12);
        s2 = TAUSWORTHE(s2, 2, 25, 4294967288UL, 4);
        s3 = TAUSWORTHE(s3, 3, 11, 4294967280UL, 17);

        __srandom32(&s1, &s2, &s3, SET_SEED);

        return (s1 ^ s2 ^ s3);
}

/* return write value */
unsigned int CPU_cache_flush_setting(unsigned int target_addr)
{
    unsigned int i;
    volatile unsigned int data __attribute__((unused));
    //volatile unsigned int test_times;
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
    pdata = (unsigned int *)target_addr;
    /* Dirtify DCache */
    for(i=0; i<DCACHE_SIZE; i = i+4)
    {
        *pdata = BACKGROUND_VALUE;
        pdata++;
    }

    pdata = (unsigned int *)target_addr;
    /* Read data into DCache */
    for(i=0; i<DCACHE_SIZE; i = i+4)
    {
        data = *pdata;
        pdata++;
    }
    

    pdata = (unsigned int *)target_addr;
    /* Dirtify DCache */
    for(i=0; i<DCACHE_SIZE; i = i+4)
    {
        *pdata = write_value;
        pdata++;
    }
    return write_value;
}


int CPU_cache_flush_checking(unsigned int write_value, unsigned int target_addr)
{
    volatile unsigned int *pdata;
    int i;

    pdata = (unsigned int *)target_addr;
    /* varify the data */
    for(i=0; i<DCACHE_SIZE; i = i+4)
    {
        if((*pdata) != write_value)
        {
            printf("\n %s line: %d pdata(%p) 0x%08x != 0x%08x\n", \
                __FUNCTION__, __LINE__, pdata, *pdata , write_value);
            return CHECKING_FAIL;
        }
        pdata++;
    }

    return CHECKING_PASS;
}

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
//#pragma packed(4)


void unaligned_test_kickoff(void)
{
    _cache_flush();
    return;
}

unsigned int unaligned_test_setting(unsigned int pattern, unsigned int target_addr)
{
    unsigned int i;
    volatile t_off_1 *off1;
    volatile t_off_2 *off2;
    volatile t_off_3 *off3;
    unsigned int dram_start;
    unsigned int dram_size;
    //unsigned int area_size;   

    dram_start = target_addr;
    dram_size =  0x8000;


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

int unaligned_test_checking(unsigned int pattern, unsigned int target_addr)
{
    unsigned int i;
    volatile t_off_1 *off1;
    volatile t_off_2 *off2;
    volatile t_off_3 *off3;
    unsigned int dram_start;
    unsigned int dram_size;
    //unsigned int area_size;   

    dram_start = target_addr;
    dram_size =  0x8000;


    off1 = (t_off_1 *)(dram_start + (dram_size));
    for(i=0;i<(dram_size/sizeof(t_off_1));i++){
        if((off1[i].w != pattern) || (off1[i].c1 != BYTE_C)){
                        printf("addr(%p)(0x%08x) != 0x%08x, addr(%p)(0x%08x) != 0x%08x %s, %d\n",\
                &off1[i].w, off1[i].w, pattern, &off1[i].c1, off1[i].c1, BYTE_C,\
                 __FUNCTION__, __LINE__);
            return CHECKING_FAIL;
        }
    }

    off2 = (t_off_2 *)(dram_start + (2*dram_size));
    for(i=0;i<(dram_size/sizeof(t_off_2));i++){
        if(off2[i].w != pattern || (off2[i].c1 != BYTE_C) || (off2[i].c2 != BYTE_D)){
                        printf("addr(%p)(0x%08x) != 0x%08x, addr(%p)(0x%08x) != 0x%08x \
                , addr(%p)(0x%08x) != 0x%08x %s, %d\n",\
                &off2[i].w, off2[i].w, pattern, &off2[i].c1, off2[i].c1, BYTE_C,\
                &off2[i].c2, off2[i].c2, BYTE_D, __FUNCTION__, __LINE__);
            return CHECKING_FAIL;
        }
    }

    off3 = (t_off_3 *)(dram_start + (3*dram_size));
    for(i=0;i<(dram_size/sizeof(t_off_3));i++){
        if(off3[i].w != pattern ||(off3[i].c1 != BYTE_C) || (off3[i].c2 != BYTE_D)\
            || (off3[i].c3 != BYTE_E)){
                        printf("addr(%p)(0x%08x) != 0x%08x, addr(%p)(0x%08x) != 0x%08x \
                , addr(%p)(0x%08x) != 0x%08x, addr(%p)(0x%08x) != 0x%08x %s, %d\n",\
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
                default:
                        return; //return NO_SUCH_CASE;
                        break;
        }
        return;
}

/*
 * Dispatch CPU Data setting
 * return value: 0 no such case. others, kick off value */
unsigned int CPU_setting(int caseNo, unsigned int target_addr)
{
        switch (caseNo)
        {

                case 0 ... 7:
                        return CPU_cache_flush_setting(target_addr);
                        break;
        case 8 ... 15:
            return unaligned_test_setting(unaligned_patterns[caseNo-8], target_addr);
            break;
                default:
                        return NO_SUCH_CASE;
                        break;
        }
        return NO_SUCH_CASE;
}


/*
 * Dispatch CPU Data checking
 * return value: -1 checking data fail. 0 no such case. 1 checking data pased.*/
int CPU_checking(int caseNo, unsigned int param, unsigned int target_addr)
{
        switch (caseNo)
        {
                case 0 ... 7:
                        return CPU_cache_flush_checking(param, target_addr);
            break;
        case 8 ... 15:
            return unaligned_test_checking(unaligned_patterns[caseNo-8], target_addr);
            break;
                default:
                        return NO_SUCH_CASE;
            break;
        }
}



