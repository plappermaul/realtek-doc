
/*
 * Include Files
 */
#include <common.h>
#include <exports.h>
#include "../../include/memctl.h"

static unsigned int dram_patterns[] =	{
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

static unsigned int line_toggle_pattern[] = {
					0xF0F0F0F0,
					0x0F0F0F0F,
					0xFF00FF00,
					0x00FF00FF,
					0x00000000,
					0xFFFFFFFF,
					0x5A5A5A5A,
					0xA5A5A5A5
				};

static unsigned int toggle_pattern[] = {
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


/*
 * Function Declaration
 */
extern unsigned int board_DRAM_freq_mhz(void);

/*Platform independent parameter*/
#define HANDLE_FAIL while(1);
/*Cases dependent parameter*/
#define TEST_AREA_SIZE (memctlc_dram_size() - 0x800000)

void dram_share_normal_patterns(uint32 write_addr, uint32 dram_size, uint32 area_size, uint32 read_addr)
{
    int i;
    int j;
    uint32 start_value;
    volatile uint32 *read_start;
    volatile uint32 *start;


    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(dram_patterns)/sizeof(uint32)); i++)
    //for (i=1; i < 2; i++)
    {
	_cache_flush();

        /* write pattern*/
        start = (uint32 *)(write_addr + (i*dram_size)%(area_size));
        read_start = (uint32 *)((uint32)read_addr + (i*dram_size)%(area_size)+ dram_size-4);
        for(j=0; j < dram_size; j=j+4)
        {
            *start = dram_patterns[i];
            start++;
        }

	_cache_flush();


        /* check data */  
        for(j=0; j < dram_size; j=j+4)
        {
	    start_value = (*read_start);
            if(start_value != dram_patterns[i])
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        read_start , start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start--;
        }
	printf("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], read_start);
    }
    printf("%s test completed.\r", __FUNCTION__);

    return;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}



void dram_share_walking_of_1(uint32 write_addr, uint32 dram_size, uint32 area_size, uint32 read_addr)
{
	int i;
	int j;
	uint32 walk_pattern;
	uint32 start_value;
	volatile uint32 *read_start;
	volatile uint32 *start;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i++)
	{
		_cache_flush();

	        /* generate pattern */
	        walk_pattern = (1 << i);

        	/* write pattern*/
	        start = (uint32 *)(write_addr + (i*dram_size)%(area_size));
	        read_start = (uint32 *)((uint32)read_addr + (i*dram_size)%(area_size)+ dram_size-4);

//        start = (uint32 *)(write_addr + (i*dram_size)%(area_size));
//        read_start = (uint32 *)(TO_UNCACHED_ADDR((uint32)start+dram_size-4));
	        for(j=0; j < dram_size; j=j+4)
	        {
	            *start = walk_pattern;
	            start++;
        	}

		_cache_flush();

        	/* check data */  
	        for(j=0; j < dram_size; j=j+4)
	        {
			start_value = (*read_start);
	        	if(start_value != walk_pattern)
            		{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
					read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				goto test_fail;
            		}
			read_start--;
		}
		printf("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, read_start);
	}
	printf("%s test completed.\r", __FUNCTION__);
	return;
test_fail:
	printf("%s test fails.\n", __FUNCTION__);
	HANDLE_FAIL;
}



void dram_share_walking_of_0(uint32 write_addr, uint32 dram_size, uint32 area_size, uint32 read_addr)
{
    int i;
    int j;
    uint32 start_value;
    uint32 walk_pattern;
    volatile uint32 *start;
    volatile uint32 *read_start;

    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i++)
    {

	_cache_flush();

        /* generate pattern */
        walk_pattern = ~(1 << i);

        /* write pattern*/
		start = (uint32 *)(write_addr + (i*dram_size)%(area_size));
		read_start = (uint32 *)((uint32)read_addr + (i*dram_size)%(area_size)+ dram_size-4);
       // start = (uint32 *)(write_addr + (i*dram_size)%(area_size));
       // read_start = (uint32 *)(TO_UNCACHED_ADDR(((uint32)start)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = walk_pattern;
            start++;
        }

	_cache_flush();

        /* check data */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != walk_pattern)
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start--;
        }
	printf("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, read_start);
    }
    printf("%s test completed.\r", __FUNCTION__);
    return;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}

void dram_share_addr_rot(uint32 write_addr, uint32 dram_size, uint32 area_size, uint32 read_addr)
{
	int i;
	int j;
	uint32 start_value;
	uint32 read_start_addr;
	volatile uint32 *start;
	volatile uint32 *read_start;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i=i+4)
	//for (i=0; i < 4; i=i+4)
	{
		_cache_flush();
		/* write pattern*/
		start = (uint32 *)(write_addr + ((i/4)*dram_size)%(area_size));
		read_start = (uint32 *)((uint32)read_addr + (((i/4)*dram_size)%(area_size)) + dram_size-4);
		read_start_addr = ((uint32)start + dram_size-4);
		
		//start = (uint32 *)(write_addr + ((i/4)*dram_size)%(area_size));
		//read_start_addr = ((uint32)start + dram_size-4);
		//read_start = (uint32 *)(TO_UNCACHED_ADDR(((uint32)start)+dram_size-4));
		for(j=0; j < dram_size; j=j+4)
		{
			*start = ((uint32)start << i);
			start++;
		}

		_cache_flush();

		/* check uncached data */  
		for(j=0; j < dram_size; j=j+4)
		{
			start_value = (*read_start);
			if(start_value != ((read_start_addr) << i))
			{
				printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
					read_start , start_value, ((read_start_addr) << i), \
					__FUNCTION__, __LINE__);
				goto test_fail;
			}
			read_start_addr = read_start_addr - 4;
			read_start--;
		}
		printf("rotate %d 0x%x passed\r", i, read_start);
	}
	printf("%s test completed.\r", __FUNCTION__);
	return;
test_fail:
	printf("%s test fails.\n", __FUNCTION__);
	HANDLE_FAIL;
}

void dram_share_com_addr_rot(uint32 write_addr, uint32 dram_size, uint32 area_size, uint32 read_addr)
{
    int i;
    int j;
    uint32 start_value;
    uint32 read_start_addr;
    volatile uint32 *start;
    volatile uint32 *read_start;

    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+4)
    {

	_cache_flush();

        /* write pattern*/

		start = (uint32 *)(write_addr+ ((i/4)*dram_size)%(area_size));
		read_start = (uint32 *)((uint32)read_addr + (((i/4)*dram_size)%(area_size)) + dram_size-4);
		read_start_addr = ((uint32)start + dram_size-4);


        //start = (uint32 *)(write_addr + (i*dram_size)%(area_size));
        //read_start_addr = (uint32)start + dram_size - 4;
        //read_start = (uint32 *)(TO_UNCACHED_ADDR(((uint32)start)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = ~(((uint32)start) << i);
            start++;
        }

	_cache_flush();

        /* check data */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != (~(read_start_addr << i)))
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        read_start , start_value, ~((read_start_addr) << i),\
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start--;
            read_start_addr = read_start_addr - 4;
        }
	printf("~rotate %d 0x%x passed\r", i, read_start);

    }
    printf("%s test completed.\r", __FUNCTION__);
    return;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}

/*
 * write two half-words and read word.
 */
void dram_share_half_word_access(uint32 write_addr, uint32 dram_size, uint32 area_size, uint32 read_addr)
{
    int i;
    int j;
    uint16 h_word;
    uint32 start_value;
    volatile uint16 *start_h;
    volatile uint32 *start_w;

    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(dram_patterns)/sizeof(uint32)); i++)
    {
	_cache_flush();

        /* write half-word pattern*/
        start_h = (uint16 *)(write_addr+ (i*dram_size)%(area_size));
        start_w = (uint32 *)(read_addr + (i*dram_size)%(area_size) + dram_size - 4 );
        //start_w = (uint32 *)(TO_UNCACHED_ADDR(write_addr + (i*dram_size)%(area_size)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            h_word = (uint16)(dram_patterns[i]);
            *(start_h+1) = h_word;
            h_word = (uint16)(dram_patterns[i] >> 16);
            *start_h = h_word;
            start_h+=2;
        }

	_cache_flush();

        /* read word and check data */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*start_w);
            if(start_value != dram_patterns[i])
            {
                 printf("addr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",\
                       start_w, start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            start_w--;
        }
	printf("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], start_h);
    }
    printf("%s test completed.\r", __FUNCTION__);
    return;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;

}

void dram_share_byte_access(uint32 write_addr, uint32 dram_size, uint32 area_size, uint32 read_addr)
{
    int i;
    int j;
    uint8 byte;
    uint32 start_value;
    volatile uint8 *start_b;
    volatile uint32 *start_w;

    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(dram_patterns)/sizeof(uint32)); i++)
    {

	_cache_flush();

        /* write byte pattern*/
        //start_w = (uint32 *)(TO_UNCACHED_ADDR(write_addr +(i*dram_size)%(area_size)+dram_size-4));
        start_w = (uint32 *)(read_addr + (i*dram_size)%(area_size) + dram_size - 4);
        start_b = (uint8 *)(write_addr+(i*dram_size)%(area_size));
        for(j=0; j < dram_size; j=j+4)
        {
            byte = (uint8)(dram_patterns[i]);
            *(start_b+3) = byte;
            byte = (uint8)(dram_patterns[i] >> 8);
            *(start_b+2) = byte;
            byte = (uint8)(dram_patterns[i] >> 16);
            *(start_b+1) = byte;
            byte = (uint8)(dram_patterns[i] >> 24);
            *(start_b) = byte;
            start_b+=4;
        }

	_cache_flush();

        /* read word and check data */  
        for (j=0; j < dram_size; j=j+4)
        {
            start_value = *start_w;
            if (start_value != dram_patterns[i])
            {
                 printf("addr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",\
                       start_w, start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            start_w--;
        }
	printf("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], start_b);
    }
    printf("%s test completed.\r", __FUNCTION__);
    return;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}

void dram_share_memcpy_test(uint32 write_addr, uint32 dram_size, uint32 area_size, uint32 read_addr)
{
    int i;
    int j;
    uint32 start_value;
    uint32 read_start_addr;
    volatile uint32 *start;
    volatile uint32 *read_start;

    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+2)
    {

	_cache_flush();

        /* write pattern*/
        start = (uint32 *)(write_addr + (i*dram_size)%(area_size));
        read_start_addr = ((uint32)start + (dram_size-4));
        read_start = (uint32 *)((read_addr + (i*dram_size)%(area_size)) + dram_size - 4);
        //read_start = (uint32 *)(TO_UNCACHED_ADDR(((uint32)start)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = ((uint32)start << i);
            start++;
        }


	memcpy((char *)(write_addr+dram_size), (char *)(write_addr+ (i*dram_size)%(area_size)), dram_size);

	_cache_flush();

        /* check data */  
        read_start = (uint32 *)((read_addr + (i*dram_size)%(area_size)) + dram_size - 4);
        //read_start = (uint32 *)(write_addr+dram_size+dram_size-4);
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != ((read_start_addr) << i))
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
                        read_start , start_value, ((read_start_addr) << i), \
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start = read_start - 1;
            read_start_addr = read_start_addr - 4;
        }
	printf("memcpy %d 0x%x passed\r", i, start);
    }
    printf("%s test completed.\r", __FUNCTION__);
    return;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}


#pragma pack(1)
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

#pragma packed(4)
void dram_share_unaligned_test(uint32 write_addr, uint32 dram_size, uint32 area_size, uint32 read_addr)
{
	unsigned int i, j;
	volatile t_off_1 *off1;
	volatile t_off_2 *off2;
	volatile t_off_3 *off3;

	off1 = (t_off_1 *)write_addr;
	off2 = (t_off_2 *)write_addr;
	off3 = (t_off_3 *)write_addr;


	/* patterns loop */
	for(j=0; j<sizeof(dram_patterns)/sizeof(unsigned int);j++)
	{
		/* offset 1 bytes */
         	off1 = (t_off_1 *)(write_addr+ (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_1));i++){
			off1[i].c1 = 0xcc;
			off1[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
         	off1 = (t_off_1 *)(read_addr + (j*dram_size)%(area_size));
		for(i=0;i<(dram_size/sizeof(t_off_1));i++){
			if((off1[i].w != dram_patterns[j]) || (off1[i].c1 != 0xcc)){
				printf("%s(%d) ", __FUNCTION__, __LINE__);
				printf("offset 1 error:addr(0x%x) write 0x%x, read 0x%x, c1(%02x)\n", \
					&off1[i], dram_patterns[j], off1[i].w, off1[i].c1);
				goto test_fail;
			}
		}
	
	
		/* offset 2 bytes */
         	off2 = (t_off_2 *)(write_addr+ (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_2));i++){
			off2[i].c1 = 0xcc;
			off2[i].c2 = 0xdd;
			off2[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
         	off2 = (t_off_2 *)(read_addr + (j*dram_size)%(area_size));
		for(i=0;i<(dram_size/sizeof(t_off_2));i++){
			if(off2[i].w != dram_patterns[j] || (off2[i].c1 != 0xcc) || (off2[i].c2 != 0xdd)){
				printf("%s(%d) ", __FUNCTION__, __LINE__);
				printf("offset 2 error:addr(0x%x) write 0x%x, read 0x%x, c1(0x%x), c2(0x%x)\n", \
					&off2[i], dram_patterns[j], off2[i].w, off2[i].c1, off2[i].c2);
				printf("&dram_pattern[%d](0x%p) = 0x%x\n", j, &dram_patterns[j], dram_patterns[j]);
				goto test_fail;
			}
		}

		/* offset 3 bytes */
         	off3 = (t_off_3 *)(write_addr+ (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_3));i++){
			off3[i].c1 = 0xcc;
			off3[i].c2 = 0xdd;
			off3[i].c3 = 0xee;
			off3[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
         	off3 = (t_off_3 *)(read_addr + (j*dram_size)%(area_size));
		for(i=0;i<(dram_size/sizeof(t_off_3));i++){
			if(off3[i].w != dram_patterns[j] ||(off3[i].c1 != 0xcc) || (off3[i].c2 != 0xdd)\
			 || (off3[i].c3 != 0xee)){
				printf("%s(%d) ", __FUNCTION__, __LINE__);
				printf("offset 3 error:addr(0x%x) write 0x%x, "\
					"read 0x%x, c1(%02x), c2(%02x), c3(%02x)\n", \
					&off1[i], dram_patterns[j], off3[i].w, off3[i].c1, \
					off3[i].c2, off3[i].c3);
				goto test_fail;
			}
		}
		printf("pattern[%d](0x%x) 0x%x pass\r", j, dram_patterns[j], off3);

	}
   	 printf("%s test passed.\r", __FUNCTION__);
	 return;
test_fail:
   	 printf("%s test failed.\n", __FUNCTION__);
	 HANDLE_FAIL;

}




/*Cases dependent parameters*/
#define INIT_VALUE (0x5A5AA5A5)
#define BACKGROUND_VALUE (0xDEADDEAD)
#define GET_SEED 1
#define SET_SEED 0
#define RANDOM_TEST
#define TEST_TIMES (0x1)
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

//int dram_share_cache_flush_adj_addr(unsigned int addr_base, unsigned int run_times, \
			 unsigned int random, unsigned int byteset)
int dram_share_cache_flush_adj_addr(unsigned int write_addr_base, unsigned int read_addr_base, \
                         unsigned int run_times, unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata, pdata_write;
	int retcode;

	retcode = 0;

	for(test_times = 0; test_times < run_times; test_times++)
	{
		printf("\rADDRESS WORDSET write_addr_base: 0x%08x, read_addr_base: 0x%08x, times: %d, pattern: Address ", write_addr_base, read_addr_base, test_times);

		if(write_addr_base >= 0x80000000)
			pdata = (unsigned int *)(TO_UNCACHED_ADDR(write_addr_base));
		else
			pdata = (unsigned int *)(write_addr_base);

		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = (unsigned int)pdata;
			pdata++;
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(TO_CACHED_ADDR(read_addr_base));
		pdata_write = TO_CACHED_ADDR(write_addr_base);
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			//if(data != ((unsigned int)pdata))
			if(data != ((unsigned int)pdata_write))
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , pdata_write);
				retcode = -1;
			}
			pdata++;
			pdata_write+=4;;
		}
	}


	return (retcode);
}

int dram_share_cache_flush_adjacent(unsigned int write_addr_base, unsigned int read_addr_base, \
			 unsigned int run_times, unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value, a, b, c;
	int retcode;
	unsigned char vbyte;

	retcode = 0;
	vbyte = 0;

	write_value = INIT_VALUE;
	a=0x13243;b=0xaaa0bdd;c=0xfffbda0;
	__srandom32(&a, &b, &c, SET_SEED);
	for(test_times = 0; test_times < run_times; test_times++)
	{
		if(random == 1){
			printf("\rRANDOM ");
			write_value = __random32();
		}
		else
			printf("\rFIXED  ");
		
		if(byteset == 1){
			printf("BYTESET ");
			vbyte = (unsigned char)write_value;
			write_value = ((unsigned int)vbyte | (((unsigned int)vbyte)<<8) \
			| (((unsigned int)vbyte)<<16) | (((unsigned int)vbyte)<<24));
		}
		else
			printf("WORDSET ");
	
		printf("write_addr_base: 0x%08x read_addr_base: 0x%08x, times: %d, pattern: 0x%08x ", write_addr_base, read_addr_base, test_times, write_value);

		if(write_addr_base >= 0x80000000)
			pdata = (unsigned int *)(TO_UNCACHED_ADDR(write_addr_base));
		else
			pdata = (unsigned int *)(write_addr_base);

		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		if(byteset == 1){
			memset((void *)pdata, vbyte, CFG_DCACHE_SIZE);
		}
		else{

			for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
			{
				*pdata = write_value;
				pdata++;
			}
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(TO_CACHED_ADDR(read_addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			pdata++;
		}
	}


	return (retcode);
}


//int dram_share_cache_flush_dispersed (unsigned int addr_base, unsigned int run_times,\
			   unsigned int random, unsigned int byteset)
int dram_share_cache_flush_dispersed (unsigned int write_addr_base, unsigned int read_addr_base, \
                         unsigned int run_times, unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value, a, b, c;
	int retcode;
	unsigned char vbyte;

	retcode = 0;
	vbyte = 0;

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
	for(test_times = 0; test_times < run_times; test_times++)
	{

		if(random == 1)	{
			printf("\rRANDOM ");
			write_value = __random32();
		}
		else
			printf("\rFIXED  ");
			
		if(byteset == 1){
			printf("BYTESET ");
			vbyte = (unsigned char)write_value;
			write_value = ((unsigned int)vbyte | (((unsigned int)vbyte)<<8) \
			| (((unsigned int)vbyte)<<16) | (((unsigned int)vbyte)<<24));
		}
		else
			printf("WORDSET ");
		
		printf("write_addr_base: 0x%08x, read_addr_base: 0x%08x, times: %d, pattern: 0x%08x ",write_addr_base, read_addr_base, test_times, write_value);
		if(write_addr_base >= 0x80000000)
			pdata = (unsigned int *)(TO_UNCACHED_ADDR(write_addr_base));
		else
			pdata = (unsigned int *)(write_addr_base);

		/* Dirtify DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}

		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}
	

		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		if(byteset == 1){
			memset((void *)pdata, vbyte, CFG_DCACHE_SIZE);
		}
		/* Dirtify DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}
		/* write back and invalidate DCache */
		_cache_flush();


		pdata = (unsigned int *)(TO_CACHED_ADDR(read_addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x, \n", pdata, data , write_value, __FUNCTION__, __LINE__);
				retcode = -1;
			}
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);

		}
	}


	return (retcode);
}




int dram_share_cache_flush_adjacent_toggle_word(unsigned int write_addr_base, unsigned int read_addr_base, unsigned int run_times)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode;

	retcode = 0;

	for(test_times = 0; test_times < run_times; test_times++)
	{
		write_value = toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
	
		printf("\rwrite_addr_base: 0x%08x , read_addr_base: 0x%08x, times: %d, pattern: 0x%08x ", write_addr_base, read_addr_base, test_times, write_value);
		if(write_addr_base >= 0x80000000)
			pdata = (unsigned int *)(TO_UNCACHED_ADDR(write_addr_base));
		else
			pdata = (unsigned int *)(write_addr_base);

		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = write_value;
			pdata++;
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(TO_CACHED_ADDR(read_addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n", pdata, data , write_value, __FUNCTION__, __LINE__);
				retcode = -1;
			}
			pdata++;
		}
	}


	return (retcode);
}


int dram_share_cache_flush_dispersed_toggle_word (unsigned int write_addr_base, unsigned int read_addr_base, unsigned int run_times)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode;

	retcode = 0;

	for(test_times = 0; test_times < run_times; test_times++)
	{
		write_value = toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		printf("\rwrite_addr_base: 0x%08x, read_addr_base: 0x%08x, times: %d, pattern: 0x%08x ", write_addr_base, read_addr_base ,test_times, write_value);
		if(write_addr_base >= 0x80000000)
			pdata = (unsigned int *)(TO_UNCACHED_ADDR(write_addr_base));
		else
			pdata = (unsigned int *)(write_addr_base);
		/* Dirtify DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			*pdata++ = BACKGROUND_VALUE;
			pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}

		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        data = *pdata++;
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}
	

		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		/* Dirtify DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        *pdata++ = write_value;
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(TO_CACHED_ADDR(read_addr_base));
#if 0
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}

			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}

			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}

			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}


			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}


			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}


			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}


			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);

		}
#else
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			pdata++;
		
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			pdata++;

			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			pdata++;

			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value);
				retcode = -1;
			}
			pdata++;

                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);

		}

#endif
	}


	return (retcode);
}


int dram_share_cache_flush_adjacent_toggle_line128(unsigned int write_addr_base, unsigned int read_addr_base, unsigned int run_times)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value[4];
	int retcode;

	retcode = 0;

	for(test_times = 0; test_times < run_times; test_times++)
	{
		write_value[0] = line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		write_value[1] = line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		write_value[2] = ~line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		write_value[3] = ~line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		printf("\raddr_base: 0x%08x, read_addr_base: 0x%08x, times: %d, pattern: 0x%08x ", write_addr_base, read_addr_base, test_times, write_value[0]);
		if(write_addr_base >= 0x80000000)
			pdata = (unsigned int *)(TO_UNCACHED_ADDR(write_addr_base));
		else
			pdata = (unsigned int *)(write_addr_base);
		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(TO_CACHED_ADDR(write_addr_base));
		for(i=0; i<CFG_DCACHE_SIZE; i = i+16)
		{
			*pdata++ = write_value[0];
			*pdata++ = write_value[1];
			*pdata++ = write_value[2];
			*pdata++ = write_value[3];
		}

		/* write back and invalidate DCache */
		_cache_flush();


		pdata = (unsigned int *)(TO_CACHED_ADDR(read_addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+16)
		{
			data = *pdata;
			if(data != write_value[0])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value[0]);
				retcode = -1;
			}
			pdata++;
			data = *pdata;
			if(data != write_value[1])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value[1]);
				retcode = -1;
			}
			pdata++;
			data = *pdata;
			if(data != write_value[2])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value[2]);
				retcode = -1;
			}
			pdata++;
			data = *pdata;
			if(data != write_value[3])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", pdata, data , write_value[3]);
				retcode = -1;
			}
			pdata++;
		}
	}

	return (retcode);
}

//void dram_share_cache_flush_test(uint32 dram_start, uint32 dram_size, uint32 area_size)
void dram_share_cache_flush_test(uint32 write_addr, uint32 dram_size, uint32 area_size, uint32 read_addr)
{
	int retcode;
	unsigned int write_addr_base;
	unsigned int read_addr_base;
	unsigned int test_times;
	test_times = TEST_TIMES;


	for(write_addr_base = write_addr; write_addr_base < (write_addr + area_size) ;\
		write_addr_base = write_addr_base + dram_size)
	{
		retcode = dram_share_cache_flush_adjacent(write_addr_base , read_addr_base, test_times, 0, 0);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_adjacent(write_addr_base, read_addr_base, test_times, 1, 0);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_dispersed(write_addr_base, read_addr_base, test_times, 0, 0);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_dispersed(write_addr_base, read_addr_base, test_times, 1, 0);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_adjacent(write_addr_base, read_addr_base, test_times, 0, 1);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_adjacent(write_addr_base, read_addr_base, test_times, 1, 1);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_dispersed(write_addr_base, read_addr_base, test_times, 0, 1);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_dispersed(write_addr_base, read_addr_base, test_times, 1, 1);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_adj_addr(write_addr_base, read_addr_base, test_times, 1, 1);
		if(retcode < 0){
			printf("cache_flush_test error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_adjacent_toggle_word(write_addr_base, read_addr_base, test_times);
		if(retcode < 0){
			printf("cache_flush_adjacent_toggle_word error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_dispersed_toggle_word(write_addr_base, read_addr_base, test_times);
		if(retcode < 0){
			printf("cache_flush_dispersed_toggle_word error\n");
			goto test_fail;
		}
		retcode = dram_share_cache_flush_adjacent_toggle_line128(write_addr_base, read_addr_base, test_times);
		if(retcode < 0){
			printf("cache_flush_adjacent_toggle_line128 error\n");
			goto test_fail;
		}

		read_addr_base = read_addr_base + dram_size;
	}
	printf("\n");
	return;
test_fail:
	HANDLE_FAIL;
}
#define CPU0_ZONE0_OFFSET_REG 		(0xB8001700)
#define CPU0_ZONE0_MAX_ADDR_REG 	(0xB8001704)
#define CPU0_ZONE1_OFFSET_REG 		(0xB8001710)
#define CPU0_ZONE1_MAX_ADDR_REG 	(0xB8001714)
#define CPU0_ZONE2_OFFSET_REG		(0xB8001720)
#define CPU0_ZONE2_MAX_ADDR_REG 	(0xB8001724)

unsigned int get_zone1_addr(unsigned int zone0_addr)
{

	unsigned int zone0_offset, zone1_offset;


	zone0_offset = *((volatile unsigned int *)CPU0_ZONE0_OFFSET_REG);
	zone1_offset = *((volatile unsigned int *)CPU0_ZONE1_OFFSET_REG);

	if(zone0_addr>=0x80000000 && zone0_addr<0x90000000){
		if (0x80000000+zone1_offset > zone0_addr){
			return 0;
		}else{
			return (0x90000000 + (zone0_addr - 0x80000000 - zone1_offset ));
		}
	}

	if(zone0_addr>=0xA0000000 && zone0_addr<0xB0000000){
		if (0xA0000000+zone1_offset > zone0_addr){
			return 0;
		}else{
			return (0xB0000000 + (zone0_addr - 0xA0000000 - zone1_offset));
		}
	}
	else{
		printf("%s, %d: input error\n", __FUNCTION__, __LINE__);
		return 0;
	}
}

unsigned int get_zone2_addr(unsigned int zone0_addr)
{

	unsigned int zone0_offset, zone2_offset;


	zone0_offset = *((volatile unsigned int *)CPU0_ZONE0_OFFSET_REG);
	zone2_offset = *((volatile unsigned int *)CPU0_ZONE2_OFFSET_REG);

	if(zone0_addr>=0x80000000 && zone0_addr<0x90000000){
		if (0x80000000+zone2_offset > zone0_addr){
			return 0;
		}else{
			return (0x20000000 + (zone0_addr - 0x80000000 - zone2_offset ));
		}
	}

	if(zone0_addr>=0xA0000000 && zone0_addr<0xB0000000){
		if (0xA0000000+zone2_offset > zone0_addr){
			return 0;
		}else{
			return (0x20000000 + (zone0_addr - 0xA0000000 - zone2_offset));
		}
	}
	else{
		printf("%s, %d: input error\n", __FUNCTION__, __LINE__);
		return 0;
	}
}
void _dram_share_test(uint32 dram_start, uint32 dram_size, uint32 area_size)
{

	unsigned int zone1_offset;
	unsigned int zone2_offset;
	unsigned int zone1_addr;
	unsigned int zone2_addr;
	volatile unsigned int *init_ptr;

	for(zone1_offset = 0; zone1_offset < area_size; zone1_offset = zone1_offset + 0x100000){
		printf("zone_offset = 0x%08x\n", zone1_offset);
		zone2_offset = zone1_offset;
		*((volatile unsigned int *)(0xb8001710)) = zone1_offset;
		*((volatile unsigned int *)(0xb8001720)) = zone2_offset;
		/* 
		 * 1. Initialize the data out of zone 1 and zone 2 
		 */
		for(init_ptr = (volatile unsigned int *)dram_start; init_ptr < (unsigned int *)(dram_start + area_size); init_ptr++){
			*init_ptr = (unsigned int)init_ptr;
		}
		/* 
		 * 2. Zone 0 and Zone 2 test 
		 */
		/* return 0 if the input value doesn't fall in the zone 1 area. */
		zone2_addr = get_zone2_addr(dram_start+zone2_offset);
		if(zone2_addr!=0){
			dram_share_cache_flush_test(dram_start+zone2_offset, dram_size, area_size - zone2_offset, zone2_addr);
			dram_share_cache_flush_test(zone2_addr             , dram_size, area_size - zone2_offset, dram_start+zone2_offset);
			dram_share_unaligned_test(dram_start+zone2_offset, dram_size, area_size - zone2_offset, zone2_addr);
			dram_share_unaligned_test(zone2_addr             , dram_size, area_size - zone2_offset, dram_start+zone2_offset);
			dram_share_normal_patterns(dram_start+zone2_offset, dram_size, area_size - zone2_offset, zone2_addr);
			dram_share_normal_patterns(zone2_addr		  , dram_size, area_size - zone2_offset, dram_start+zone2_offset);
			dram_share_walking_of_1(dram_start+zone2_offset , dram_size, area_size - zone2_offset, zone2_addr);
			dram_share_walking_of_1(zone2_addr		, dram_size, area_size - zone2_offset, dram_start+zone2_offset);
			dram_share_walking_of_0(dram_start+zone2_offset , dram_size, area_size - zone2_offset, zone2_addr);
			dram_share_walking_of_0(zone2_addr		, dram_size, area_size - zone2_offset, dram_start+zone2_offset);
			dram_share_addr_rot(dram_start+zone2_offset	, dram_size, area_size - zone2_offset, zone2_addr);
			dram_share_addr_rot(zone2_addr		   	, dram_size, area_size - zone2_offset, dram_start+zone2_offset);
			dram_share_com_addr_rot(dram_start+zone2_offset , dram_size, area_size - zone2_offset, zone2_addr);
			dram_share_com_addr_rot(zone2_addr		, dram_size, area_size - zone2_offset, dram_start+zone2_offset);
			dram_share_half_word_access(zone2_addr              , dram_size, area_size - zone2_offset, dram_start+zone2_offset);
			dram_share_half_word_access(dram_start+zone2_offset , dram_size, area_size - zone2_offset, zone2_addr);
			dram_share_byte_access(zone2_addr              , dram_size, area_size - zone2_offset, dram_start+zone2_offset);
			dram_share_byte_access(dram_start+zone2_offset , dram_size, area_size - zone2_offset, zone2_addr);
			dram_share_memcpy_test(zone2_addr              , dram_size, area_size - zone2_offset, dram_start+zone2_offset);
			dram_share_memcpy_test(dram_start+zone2_offset , dram_size, area_size - zone2_offset, zone2_addr);
		}
		
		/*
		 * 3. Verify unused data in Zone 0 area.
		 */
		for(init_ptr = (volatile unsigned int *)dram_start; init_ptr < (unsigned int *)(dram_start + zone2_offset); init_ptr++){
			if(*init_ptr != (unsigned int)init_ptr){
				printf("Error: Function(%s), Line(%d): Zone0 address(0x%08p):(0x%08x) != initial value(0x%08x)\n", \
					__FUNCTION__, __LINE__, init_ptr, *init_ptr, init_ptr);
			}
		}
		

		/*
		 * 4. Zone 0 and Zone 1 test 
		 */
		/* return 0 if the input value doesn't fall in the zone 1 area. */
		zone1_addr = get_zone1_addr(dram_start+zone1_offset);
		if(zone1_addr!=0){
			dram_share_cache_flush_test(dram_start+zone1_offset, dram_size, area_size - zone1_offset, zone1_addr);
			dram_share_cache_flush_test(zone1_addr             , dram_size, area_size - zone1_offset, dram_start+zone1_offset);
			dram_share_unaligned_test(dram_start+zone1_offset, dram_size, area_size - zone1_offset, zone1_addr);
			dram_share_unaligned_test(zone1_addr             , dram_size, area_size - zone1_offset, dram_start+zone1_offset);
			dram_share_normal_patterns(dram_start+zone1_offset, dram_size, area_size - zone1_offset, zone1_addr);
			dram_share_normal_patterns(zone1_addr		  , dram_size, area_size - zone1_offset, dram_start+zone1_offset);
			dram_share_walking_of_1(dram_start+zone1_offset , dram_size, area_size - zone1_offset, zone1_addr);
			dram_share_walking_of_1(zone1_addr		, dram_size, area_size - zone1_offset, dram_start+zone1_offset);
			dram_share_walking_of_0(dram_start+zone1_offset , dram_size, area_size - zone1_offset, zone1_addr);
			dram_share_walking_of_0(zone1_addr		, dram_size, area_size - zone1_offset, dram_start+zone1_offset);
			dram_share_addr_rot(dram_start+zone1_offset	, dram_size, area_size - zone1_offset, zone1_addr);
			dram_share_addr_rot(zone1_addr		   	, dram_size, area_size - zone1_offset, dram_start+zone1_offset);
			dram_share_com_addr_rot(dram_start+zone1_offset , dram_size, area_size - zone1_offset, zone1_addr);
			dram_share_com_addr_rot(zone1_addr		, dram_size, area_size - zone1_offset, dram_start+zone1_offset);
			dram_share_half_word_access(zone1_addr              , dram_size, area_size - zone1_offset, dram_start+zone1_offset);
			dram_share_half_word_access(dram_start+zone1_offset , dram_size, area_size - zone1_offset, zone1_addr);
			dram_share_byte_access(zone1_addr              , dram_size, area_size - zone1_offset, dram_start+zone1_offset);
			dram_share_byte_access(dram_start+zone1_offset , dram_size, area_size - zone1_offset, zone1_addr);
			dram_share_memcpy_test(zone1_addr              , dram_size, area_size - zone1_offset, dram_start+zone1_offset);
			dram_share_memcpy_test(dram_start+zone1_offset , dram_size, area_size - zone1_offset, zone1_addr);
		}
		/*
		 * 5. Verify unused data in Zone 0 area.
		 */
		for(init_ptr = (volatile unsigned int *)dram_start; init_ptr < (unsigned int *)(dram_start + zone1_offset); init_ptr++ ){
			if(*init_ptr != (unsigned int)init_ptr){
				printf("Error: Function(%s), Line(%d): Zone0 address(0x%08p):(0x%08x) != initial value(0x%08x)\n", \
					__FUNCTION__, __LINE__, init_ptr, *init_ptr, init_ptr);
			}
		}
	}
	
	*((volatile unsigned int *)(0xb8001710)) = 0;
}

int dram_share_test (int flag, int argc, char *argv[])
{
        
	uint32 start_addr;
        uint32 dram_size;
        uint32 oldmcr;
	uint32 test_area_size;
        volatile uint32 *mcr;

	/* Zone 1 */
	/*back the value of mcr*/
	
	mcr = (uint32 *)MCR;
        start_addr = 0xa0000000;
        dram_size = 0x100000;
	if(TEST_AREA_SIZE<0x4000000)
		test_area_size = TEST_AREA_SIZE;
	else
		test_area_size = 0x3800000;

	/* disable prefetch mechanism */
	oldmcr = disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );

	_cache_flush();

	/*1. Uncached range */
        printf("No prefetch, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",\
                TO_UNCACHED_ADDR(start_addr), dram_size, *mcr);
        _dram_share_test(TO_UNCACHED_ADDR(start_addr), dram_size, test_area_size);

	/*2. Cached range without prefetch */
        printf("\nNo prefetch, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",\
                TO_CACHED_ADDR(start_addr), dram_size, *mcr);
        _dram_share_test(TO_CACHED_ADDR(start_addr), dram_size, test_area_size);

	disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );

	/*3. Cached range with data prefetch mechanism */
	enable_DRAM_prefech(MCR_PREFETCH_OLD_MECH, MCR_PREFETCH_DATA_SIDE);
        printf("\nEnable MCR_PREFETCH_DATA_SIDE, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",\
                TO_CACHED_ADDR(start_addr), dram_size, *mcr);
        _dram_share_test(TO_CACHED_ADDR(start_addr), dram_size, test_area_size);


	disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );

	/*4. Cached range with instruction prefetch mechanism */
	enable_DRAM_prefech(MCR_PREFETCH_OLD_MECH, MCR_PREFETCH_INS_SIDE);
        printf("\nEnable MCR_PREFETCH_INS_SIDE, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",\
               TO_CACHED_ADDR(start_addr), dram_size, *mcr);
        _dram_share_test(TO_CACHED_ADDR(start_addr), dram_size, test_area_size);

	disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );

	/*5. Cached range with instruction/data prefetch mechanism */
	enable_DRAM_prefech(MCR_PREFETCH_OLD_MECH, MCR_PREFETCH_DATA_SIDE | MCR_PREFETCH_INS_SIDE);
        printf("\nEnable MCR_PREFETCH_DATA_SIDE/MCR_PREFETCH_INS_SIDE, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",\
               TO_CACHED_ADDR(start_addr), dram_size, *mcr);
        _dram_share_test(TO_CACHED_ADDR(start_addr), dram_size, test_area_size);


	/* restore memory controller register */
        *mcr = oldmcr;

	return 0;
}
