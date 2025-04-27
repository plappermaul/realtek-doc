
/*
 * Include Files
 */
 #include <preloader.h>
#include <soc.h>
#include <pblr.h>
#include "memctl.h"
#define _cache_flush	pblr_dc_flushall
#include "config.h"


/*Platform independent parameter*/
#ifdef CONFIG_MT_ERR_HANDLE
#define HANDLE_FAIL return -1;
#else
#define HANDLE_FAIL while(1);
#endif

/*Cases dependent parameter*/
#define TAREA_MAX		(2)
#define TAREA1_START	(0x80000000)
#define TAREA1_END		(CONFIG_SYS_TEXT_BASE - 0x300000)
#define TAREA2_START	(CONFIG_SYS_TEXT_BASE + 0x300000)
#define TAREA1_SIZE		(unsigned int)((TAREA1_END&0x1FFFFFFF))	// offset 0x0~TEXT BASE-0x300000
#define TAREA2_SIZE		(unsigned int)(plr_initdram(0)-(TAREA2_START&0x1FFFFFFF))	// offset+0x4000000~end
#define CFG_DCACHE_SIZE (0x20)

#define MEMCTL_DEBUG_PRINTF printf
//#define MEMCTL_DEBUG_PRINTF(...)

const unsigned int plr_dram_patterns[] =	{
					0xaaaaaaaa,
					0xcccccccc,
					0xf0f0f0f0,
					0xff00ff00,
					0xffff0000,
					0xffffffff,
					0x00000000,
					0x0000ffff,
					0x00ff00ff,
					0x0f0f0f0f,
					0x33333333,
					0x55555555
};

const unsigned int plr_line_plr_toggle_pattern[] = {
					0xF0F0F0F0,
					0x0F0F0F0F,
					0xFF00FF00,
					0x00FF00FF,
					0x00000000,
					0xFFFFFFFF,
					0x5A5A5A5A,
					0xA5A5A5A5
				};

const unsigned int plr_toggle_pattern[] = {
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
int plr_dram_normal_patterns(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    volatile u32_t *read_start;
    volatile u32_t *start;


    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(plr_dram_patterns)/sizeof(u32_t)); i++) {
		_cache_flush();

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start = (u32_t *)(UADDR((u32_t)start+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = plr_dram_patterns[i];
            start++;
        }

	_cache_flush();

        /* check data */  
        for(j=0; j < dram_size; j=j+4)
        {
	    start_value = (*read_start);
            if(start_value != plr_dram_patterns[i])
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, plr_dram_patterns[i],  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start--;
        }
	MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, plr_dram_patterns[i], (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);

    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}



int plr_dram_walking_of_1(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t walk_pattern;
    u32_t start_value;
    volatile u32_t *read_start;
    volatile u32_t *start;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i++)
    {

	_cache_flush();

        /* generate pattern */
        walk_pattern = (1 << i);

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start = (u32_t *)(UADDR((u32_t)start+dram_size-4));
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
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start--;
        }
	MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}



int plr_dram_walking_of_0(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t walk_pattern;
    volatile u32_t *start;
    volatile u32_t *read_start;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i++)
    {

	_cache_flush();

        /* generate pattern */
        walk_pattern = ~(1 << i);

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start = (u32_t *)(UADDR(((u32_t)start)+dram_size-4));
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
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start--;
        }
	MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}

int plr_dram_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t read_start_addr;
    volatile u32_t *start;
    volatile u32_t *read_start;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+4)
    //for (i=0; i < 4; i=i+4)
    {

	_cache_flush();
        /* write pattern*/
        start		= (u32_t *)(dram_start + ((i/4)*dram_size)%(area_size));
        read_start 	= (u32_t *)((u32_t)start + dram_size-4);
        read_start_addr = ((u32_t)start);
        start = (u32_t *)(UADDR(((u32_t)start)));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = (read_start_addr << i);
            start++;
            read_start_addr = read_start_addr + 4 ;
        }

	_cache_flush();
	read_start_addr = ((u32_t)read_start);
        /* check data reversing order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != ((read_start_addr) << i))
            {
                 printf("decr addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, ((read_start_addr) << i), 
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start_addr = read_start_addr - 4;
            read_start--;
        }

	read_start_addr += 4;
        read_start++;

        /* check data sequential order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != ((read_start_addr) << i))
            {
                 printf("seq addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, ((read_start_addr) << i), \
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start_addr = read_start_addr + 4;
            read_start++;
        }

	MEMCTL_DEBUG_PRINTF("rotate %d 0x%x passed\r", i, (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}

int plr_dram_com_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t read_start_addr;
    volatile u32_t *start;
    volatile u32_t *read_start;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+4)
    //for (i=0; i < 4; i=i+4)
    {

	_cache_flush();
        /* write pattern*/
        start		= (u32_t *)(dram_start + ((i/4)*dram_size)%(area_size));
        read_start 	= (u32_t *)((u32_t)start + dram_size-4);
        read_start_addr = ((u32_t)start);
        start = (u32_t *)(UADDR(((u32_t)start)));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = ~(read_start_addr << i);
            start++;
            read_start_addr = read_start_addr + 4 ;
        }

	_cache_flush();
	read_start_addr = ((u32_t)read_start);
        /* check data reversing order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != (~(read_start_addr << i)))
            {
                 printf("decr addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, ~((read_start_addr) << i),
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start_addr = read_start_addr - 4;
            read_start--;
        }

	read_start_addr += 4;
        read_start++;

        /* check data sequential order */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != (~(read_start_addr << i)))
            {
                 printf("seq addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, ~((read_start_addr) << i), 
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start_addr = read_start_addr + 4;
            read_start++;
        }

	MEMCTL_DEBUG_PRINTF("~rotate %d 0x%x passed\r", i, (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}
#if 0
void plr_dram_com_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t read_start_addr;
    volatile u32_t *start;
    volatile u32_t *read_start;

    printf("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+4)
    {

	_cache_flush();

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start_addr = (u32_t)start + dram_size - 4;
        read_start = (u32_t *)(UADDR(((u32_t)start)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = ~(((u32_t)start) << i);
            start++;
        }

	_cache_flush();

        /* check data sequential order */  
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

	read_start_addr += 4;
        read_start++;
        /* check data reversing order */  
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
            read_start++;
            read_start_addr = read_start_addr + 4;
        }

	printf("~rotate %d 0x%x passed\r", i, start);

    }
    printf("%s test completed.\r", __FUNCTION__);
    return;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}
#endif
/*
 * write two half-words and read word.
 */
int plr_dram_half_word_access(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u16_t h_word;
    u32_t start_value;
    volatile u16_t *start_h;
    volatile u32_t *start_w;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(plr_dram_patterns)/sizeof(u32_t)); i++)
    {
	_cache_flush();

        /* write half-word pattern*/
        start_h = (u16_t *)(dram_start+ (i*dram_size)%(area_size));
        start_w = (u32_t *)(UADDR(dram_start+ (i*dram_size)%(area_size)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            h_word = (u16_t)(plr_dram_patterns[i]);
            *(start_h+1) = h_word;
            h_word = (u16_t)(plr_dram_patterns[i] >> 16);
            *start_h = h_word;
            start_h+=2;
        }

	_cache_flush();

        /* read word and check data */  
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*start_w);
            if(start_value != plr_dram_patterns[i])
            {
                 printf("addr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",
                       (u32_t)start_w, start_value, plr_dram_patterns[i],  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            start_w--;
        }
	MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, plr_dram_patterns[i], (u32_t)start_h);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;

}

int plr_dram_byte_access(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u8_t byte;
    u32_t start_value;
    volatile u8_t *start_b;
    volatile u32_t *start_w;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < (sizeof(plr_dram_patterns)/sizeof(u32_t)); i++)
    {

	_cache_flush();

        /* write byte pattern*/
        start_w = (u32_t *)(UADDR(dram_start+(i*dram_size)%(area_size)+dram_size-4));
        start_b = (u8_t *)(dram_start+(i*dram_size)%(area_size));
        for(j=0; j < dram_size; j=j+4)
        {
            byte = (u8_t)(plr_dram_patterns[i]);
            *(start_b+3) = byte;
            byte = (u8_t)(plr_dram_patterns[i] >> 8);
            *(start_b+2) = byte;
            byte = (u8_t)(plr_dram_patterns[i] >> 16);
            *(start_b+1) = byte;
            byte = (u8_t)(plr_dram_patterns[i] >> 24);
            *(start_b) = byte;
            start_b+=4;
        }

	_cache_flush();

        /* read word and check data */  
        for (j=0; j < dram_size; j=j+4)
        {
            start_value = *start_w;
            if (start_value != plr_dram_patterns[i])
            {
                 printf("addr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",
                       (u32_t)start_w, start_value, plr_dram_patterns[i],  __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            start_w--;
        }
	MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, plr_dram_patterns[i], (u32_t)start_b);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
test_fail:
    printf("%s test fails.\n", __FUNCTION__);
    HANDLE_FAIL;
}

int plr_memcpy_test(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    int i;
    int j;
    u32_t start_value;
    u32_t read_start_addr;
    volatile u32_t *start;
    volatile u32_t *read_start;

    MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
    for (i=0; i < 32; i=i+4)
    {

	_cache_flush();

        /* write pattern*/
        start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
        read_start_addr = ((u32_t)start + (dram_size-4));
        read_start = (u32_t *)(UADDR(((u32_t)start)+dram_size-4));
        for(j=0; j < dram_size; j=j+4)
        {
            *start = ((u32_t)start << i);
            start++;
        }


	memcpy((u8_t *)(dram_start+dram_size), (const u8_t *)(dram_start + (i*dram_size)%(area_size)), (u32_t)dram_size);

	_cache_flush();

        /* check uncached data */  
        read_start = (u32_t *)(dram_start+dram_size+dram_size-4);
        for(j=0; j < dram_size; j=j+4)
        {
            start_value = (*read_start);
            if(start_value != ((read_start_addr) << i))
            {
                 printf("addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
                        (u32_t)read_start , start_value, ((read_start_addr) << i), 
                        __FUNCTION__, __LINE__);
                 goto test_fail;
            }
            read_start = read_start - 1;
            read_start_addr = read_start_addr - 4;
        }
	MEMCTL_DEBUG_PRINTF("memcpy %d 0x%x passed\r", i, (u32_t)start);
    }
    MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
    return 0;
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

#pragma pack(4)
int plr_unaligned_test(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
	unsigned int i, j;
	volatile t_off_1 *off1;
	volatile t_off_2 *off2;
	volatile t_off_3 *off3;

	off1 = (t_off_1 *)dram_start;
	off2 = (t_off_2 *)dram_start;
	off3 = (t_off_3 *)dram_start;


	/* patterns loop */
	for(j=0; j<sizeof(plr_dram_patterns)/sizeof(unsigned int);j++)
	{
		/* offset 1 bytes */
         	off1 = (t_off_1 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_1));i++){
			off1[i].c1 = 0xcc;
			off1[i].w = plr_dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_1));i++){
			if((off1[i].w != plr_dram_patterns[j]) || (off1[i].c1 != 0xcc)){
				printf("%s(%d) ", __FUNCTION__, __LINE__);
				printf("offset 1 error:addr(0x%x) write 0x%x, read 0x%x, c1(%02x)\n", 
					(u32_t)&off1[i], plr_dram_patterns[j], off1[i].w, off1[i].c1);
				goto test_fail;
			}
		}
	
	
		/* offset 2 bytes */
         	off2 = (t_off_2 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_2));i++){
			off2[i].c1 = 0xcc;
			off2[i].c2 = 0xdd;
			off2[i].w = plr_dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_2));i++){
			if(off2[i].w != plr_dram_patterns[j] || (off2[i].c1 != 0xcc) || (off2[i].c2 != 0xdd)){
				printf("%s(%d) ", __FUNCTION__, __LINE__);
				printf("offset 2 error:addr(0x%x) write 0x%x, read 0x%x, c1(0x%x), c2(0x%x)\n", 
					(u32_t)&off2[i], plr_dram_patterns[j], off2[i].w, off2[i].c1, off2[i].c2);
				printf("&dram_pattern[%d](0x%p) = 0x%x\r", j, &plr_dram_patterns[j], plr_dram_patterns[j]);
				goto test_fail;
			}
		}

		/* offset 3 bytes */
         	off3 = (t_off_3 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_3));i++){
			off3[i].c1 = 0xcc;
			off3[i].c2 = 0xdd;
			off3[i].c3 = 0xee;
			off3[i].w = plr_dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_3));i++){
			if(off3[i].w != plr_dram_patterns[j] ||(off3[i].c1 != 0xcc) || (off3[i].c2 != 0xdd)\
			 || (off3[i].c3 != 0xee)){
				printf("%s(%d) ", __FUNCTION__, __LINE__);
				printf("offset 3 error:addr(0x%x) write 0x%x, "
					"read 0x%x, c1(%02x), c2(%02x), c3(%02x)\n", \
					(u32_t)&off1[i], plr_dram_patterns[j], off3[i].w, off3[i].c1, 
					off3[i].c2, off3[i].c3);
				goto test_fail;
			}
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", j, plr_dram_patterns[j], (u32_t)off3);

	}
   	 MEMCTL_DEBUG_PRINTF("%s test passed.\r", __FUNCTION__);
	 return 0;
test_fail:
   	 printf("%s test failed.\n", __FUNCTION__);
	 printf("test failed\n");
	 HANDLE_FAIL;

}



unsigned int plr_disable_DRAM_prefech(unsigned int side_id)
{
	unsigned int old_mcr;

	old_mcr = REG32(MCR);
	if( side_id & MCR_PREFETCH_INS_SIDE )
		REG32(MCR) = REG32(MCR) & ((unsigned int)MCR_PREFETCH_DIS_IMASK);
	if( side_id & MCR_PREFETCH_DATA_SIDE)
		REG32(MCR) = REG32(MCR) & ((unsigned int)MCR_PREFETCH_DIS_DMASK);

	return old_mcr;
}

unsigned int plr_enable_DRAM_prefech(unsigned int mode, unsigned int side_id)
{
	unsigned int old_mcr;
	unsigned int tmp_mode;

	tmp_mode = 0;
	old_mcr = REG32(MCR);


	if( side_id & MCR_PREFETCH_INS_SIDE ){
		plr_disable_DRAM_prefech(MCR_PREFETCH_INS_SIDE);
		if( mode == MCR_PREFETCH_OLD_MECH)
			REG32(MCR) = REG32(MCR) | MCR_PREFETCH_MODE_IOLD | MCR_PREFETCH_ENABLE_INS;
		else
			REG32(MCR) = REG32(MCR) | MCR_PREFETCH_MODE_INEW | MCR_PREFETCH_ENABLE_INS;

		
	}
	if( side_id & MCR_PREFETCH_DATA_SIDE ){
		plr_disable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE);
		if( mode == MCR_PREFETCH_OLD_MECH)
			REG32(MCR) = REG32(MCR) | MCR_PREFETCH_MODE_DOLD | MCR_PREFETCH_ENABLE_DATA;
		else
			REG32(MCR) = REG32(MCR) | MCR_PREFETCH_MODE_DNEW | MCR_PREFETCH_ENABLE_DATA;
	}

	return old_mcr;
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

int plr_cache_flush_adj_addr(unsigned int addr_base, unsigned int run_times, \
			 unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	int retcode;

	retcode = 0;

	for(test_times = 0; test_times < run_times; test_times++)
	{
		MEMCTL_DEBUG_PRINTF("\rADDRESS WORDSET addr_base: 0x%08x times: %d, pattern: Address ", addr_base, test_times);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = (unsigned int)pdata;
			pdata++;
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			if(data != ((unsigned int)pdata))
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , ((unsigned int)pdata));
				retcode = -1;
			}
			pdata++;
		}
	}
    
	return (retcode);
}

int plr_cache_flush_adjacent(unsigned int addr_base, unsigned int run_times, \
			 unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode, a, b, c;
	unsigned char vbyte;

	retcode = 0;
	vbyte = 0;

	write_value = INIT_VALUE;
	a=0x13243;b=0xaaa0bdd;c=0xfffbda0;
	__srandom32(&a, &b, &c, SET_SEED);
	for(test_times = 0; test_times < run_times; test_times++)
	{
		if(random == 1){
			MEMCTL_DEBUG_PRINTF("\rRANDOM ");
			write_value = __random32();
		}
		else
			MEMCTL_DEBUG_PRINTF("\rFIXED  ");
		
		if(byteset == 1){
			MEMCTL_DEBUG_PRINTF("BYTESET ");
			vbyte = (unsigned char)write_value;
			write_value = ((unsigned int)vbyte | (((unsigned int)vbyte)<<8) \
			| (((unsigned int)vbyte)<<16) | (((unsigned int)vbyte)<<24));
		}
		else
			MEMCTL_DEBUG_PRINTF("WORDSET ");
	
		MEMCTL_DEBUG_PRINTF("addr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		if(byteset == 1){
			pblr_bset((void *)pdata, vbyte, CFG_DCACHE_SIZE);
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

		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;
		}
	}

	return (retcode);
}


int plr_cache_flush_dispersed (unsigned int addr_base, unsigned int run_times,\
			   unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode, a, b, c;
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
			MEMCTL_DEBUG_PRINTF("\rRANDOM ");
			write_value = __random32();
		}
		else
			MEMCTL_DEBUG_PRINTF("\rFIXED  ");
			
		if(byteset == 1){
			MEMCTL_DEBUG_PRINTF("BYTESET ");
			vbyte = (unsigned char)write_value;
			write_value = ((unsigned int)vbyte | (((unsigned int)vbyte)<<8) \
			| (((unsigned int)vbyte)<<16) | (((unsigned int)vbyte)<<24));
		}
		else
			MEMCTL_DEBUG_PRINTF("WORDSET ");
		
		MEMCTL_DEBUG_PRINTF("addr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value);
		pdata = (unsigned int *)(UADDR(addr_base));
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

		pdata = (unsigned int *)(CADDR(addr_base));
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
	

		pdata = (unsigned int *)(CADDR(addr_base));
		if(byteset == 1){
			pblr_bset((void *)pdata, vbyte, CFG_DCACHE_SIZE);
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


		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			i = i + 28;
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			data = *pdata++;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);

		}
	}


	return (retcode);
}




int plr_cache_flush_adjacent_toggle_word(unsigned int addr_base, unsigned int run_times)
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
		write_value = plr_toggle_pattern[(test_times%(sizeof(plr_toggle_pattern)/sizeof(unsigned int)))];
	
		MEMCTL_DEBUG_PRINTF("\raddr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = write_value;
			pdata++;
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;
		}
	}


	return (retcode);
}


int plr_cache_flush_dispersed_toggle_word (unsigned int addr_base, unsigned int run_times)
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
		write_value = plr_toggle_pattern[(test_times%(sizeof(plr_toggle_pattern)/sizeof(unsigned int)))];
		MEMCTL_DEBUG_PRINTF("\raddr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value);
		pdata = (unsigned int *)(UADDR(addr_base));
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

		pdata = (unsigned int *)(CADDR(addr_base));
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
	

		pdata = (unsigned int *)(CADDR(addr_base));
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

		pdata = (unsigned int *)(CADDR(addr_base));
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
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;
		
			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;

			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;

			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;


			data = *pdata;
			if(data != write_value)
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
				retcode = -1;
			}
			pdata++;

                        pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);

		}

#endif
	}


	return (retcode);
}


int plr_cache_flush_adjacent_toggle_line128(unsigned int addr_base, unsigned int run_times)
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
		write_value[0] = plr_line_plr_toggle_pattern[(test_times%(sizeof(plr_toggle_pattern)/sizeof(unsigned int)))];
		write_value[1] = plr_line_plr_toggle_pattern[(test_times%(sizeof(plr_toggle_pattern)/sizeof(unsigned int)))];
		write_value[2] = ~plr_line_plr_toggle_pattern[(test_times%(sizeof(plr_toggle_pattern)/sizeof(unsigned int)))];
		write_value[3] = ~plr_line_plr_toggle_pattern[(test_times%(sizeof(plr_toggle_pattern)/sizeof(unsigned int)))];
		MEMCTL_DEBUG_PRINTF("\raddr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value[0]);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Initial DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
		{
			data = *pdata;
			pdata++;
		}
	
		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		for(i=0; i<CFG_DCACHE_SIZE; i = i+16)
		{
			*pdata++ = write_value[0];
			*pdata++ = write_value[1];
			*pdata++ = write_value[2];
			*pdata++ = write_value[3];
		}

		/* write back and invalidate DCache */
		_cache_flush();


		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+16)
		{
			data = *pdata;
			if(data != write_value[0])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value[0]);
				retcode = -1;
			}
			pdata++;
			data = *pdata;
			if(data != write_value[1])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value[1]);
				retcode = -1;
			}
			pdata++;
			data = *pdata;
			if(data != write_value[2])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value[2]);
				retcode = -1;
			}
			pdata++;
			data = *pdata;
			if(data != write_value[3])
			{
				printf("pdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value[3]);
				retcode = -1;
			}
			pdata++;
		}
	}

	return (retcode);
}

int plr_cache_flush_test(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
	int retcode;
	unsigned int addr_base;
	unsigned int test_times;
	test_times = TEST_TIMES;

	MEMCTL_DEBUG_PRINTF("test area size = 0x%08x\n",area_size);

	for(addr_base = dram_start; addr_base < (dram_start + area_size) ;\
		addr_base = addr_base + dram_size)
	//for(addr_base = dram_start+0x6c00000; addr_base < (dram_start + area_size) ;
	//	addr_base = addr_base + dram_size)
	{
		retcode = plr_cache_flush_adjacent(addr_base , test_times, 0, 0);
		if(retcode < 0){
			printf("plr_cache_flush_test error\n");
			goto test_fail;
		}
#if 1
		retcode = plr_cache_flush_adjacent(addr_base, test_times, 1, 0);
		if(retcode < 0){
			printf("plr_cache_flush_test error\n");
			goto test_fail;
		}
		retcode = plr_cache_flush_dispersed(addr_base, test_times, 0, 0);
		if(retcode < 0){
			printf("plr_cache_flush_test error\n");
			goto test_fail;
		}
#endif
#if 1
		retcode = plr_cache_flush_dispersed(addr_base, test_times, 1, 0);
		if(retcode < 0){
			printf("plr_cache_flush_test error\n");
			goto test_fail;
		}
		retcode = plr_cache_flush_adjacent(addr_base, test_times, 0, 1);
		if(retcode < 0){
			printf("plr_cache_flush_test error\n");
			goto test_fail;
		}
		retcode = plr_cache_flush_adjacent(addr_base, test_times, 1, 1);
		if(retcode < 0){
			printf("plr_cache_flush_test error\n");
			goto test_fail;
		}
		retcode = plr_cache_flush_dispersed(addr_base, test_times, 0, 1);
		if(retcode < 0){
			printf("plr_cache_flush_test error\n");
			goto test_fail;
		}
		retcode = plr_cache_flush_dispersed(addr_base, test_times, 1, 1);
		if(retcode < 0){
			printf("plr_cache_flush_test error\n");
			goto test_fail;
		}
		retcode = plr_cache_flush_adj_addr(addr_base, test_times, 1, 1);
		if(retcode < 0){
			printf("plr_cache_flush_test error\n");
			goto test_fail;
		}
		retcode = plr_cache_flush_adjacent_toggle_word(addr_base, test_times);
		if(retcode < 0){
			printf("plr_cache_flush_adjacent_toggle_word error\n");
			goto test_fail;
		}
		retcode = plr_cache_flush_dispersed_toggle_word(addr_base, test_times);
		if(retcode < 0){
			printf("plr_cache_flush_dispersed_toggle_word error\n");
			goto test_fail;
		}
		retcode = plr_cache_flush_adjacent_toggle_line128(addr_base, test_times);
		if(retcode < 0){
			printf("plr_cache_flush_adjacent_toggle_line128 error\n");
			goto test_fail;
		}
#endif
	}
	MEMCTL_DEBUG_PRINTF("\n");
	return 0;
test_fail:
	HANDLE_FAIL;
}

int _plrdram_test(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
    if( (0==plr_unaligned_test( dram_start, dram_size, area_size)) 
    	&& (0==plr_dram_addr_rot(dram_start, dram_size, area_size))
    	&& (0==plr_dram_com_addr_rot(dram_start, dram_size, area_size))
  	  	&& (0==plr_dram_byte_access(dram_start, dram_size, area_size))
  	  	&& (0==plr_dram_half_word_access(dram_start, dram_size, area_size))
  	  	&& (0==plr_cache_flush_test( dram_start, dram_size, area_size))
  	  	&& (0==plr_dram_normal_patterns(dram_start, dram_size, area_size))
  	  	&& (0==plr_dram_walking_of_1(dram_start, dram_size, area_size))
  	  	&& (0==plr_dram_walking_of_0(dram_start, dram_size, area_size))
  	  	&& (0==plr_memcpy_test(dram_start, dram_size, area_size))
  	  	&& (0==plr_dram_addr_rot(dram_start, area_size, area_size))
  	  	&& (0==plr_dram_com_addr_rot(dram_start, area_size, area_size)))
	{	return 0;     }
  	  	return -1;
     }
unsigned int plr_initdram(void);
unsigned int plr_initdram() {
#define DCR_REG (*((volatile int *)(DCR)))
//	const unsigned char BNKCNTv[] = {1, 2, 3};
//	const unsigned char BUSWIDv[] = {0, 1, 2};
//	const unsigned char ROWCNTv[] = {11, 12, 13, 14, 15, 16};
//	const unsigned char COLCNTv[] = {8, 9, 10, 11, 12};
	const unsigned char BNKCNTv[4] = {1, 2, 3, 0xff};
	const unsigned char BUSWIDv[4] = {0, 1, 2, 0xff};
	const unsigned char ROWCNTv[16] = {11, 12, 13, 14, 15, 16, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	const unsigned char COLCNTv[16] = {8, 9, 10, 11, 12, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	
	return 1 << (BNKCNTv[(DCR_REG >> 28) & 0x3] +
	             BUSWIDv[(DCR_REG >> 24) & 0x3] +
	             ROWCNTv[(DCR_REG >> 20) & 0xF] +
	             COLCNTv[(DCR_REG >> 16) & 0xF]);
}

int arch_dram_test_prepare(unsigned int *dram_start, unsigned int *size, unsigned int *phys_offset)
{
	*dram_start = CADDR(CONFIG_SYS_SDRAM_BASE);

	*size = plr_initdram();

	return 0;
}



static void move32(unsigned int *src, unsigned int *dest)
{
	*dest = *src;
}

static int plr_dram_test_dataline(unsigned int * pmem)
{
	unsigned int otherpattern = 0x01234567;
	unsigned int temp32 = 0;
	int num_patterns = (sizeof(plr_dram_patterns) / sizeof((plr_dram_patterns)[0]));
	int i,index;
	unsigned int phyiscal, pat;
	unsigned int dram_patterns[num_patterns+64];
	int ret = 0;

	for ( index = 0; index < num_patterns; index++) {
		move32((unsigned int *)&(plr_dram_patterns[index]), (unsigned int *)&dram_patterns[index]);
	}
	
	for (i=0; i<32;i++){
		dram_patterns[index++] = (1 << i);
	}
	
	for (i=0; i<32;i++){
		dram_patterns[index++] = ~(1 << i);
	}
	
	num_patterns = index;
	
	for ( i = 0; i < num_patterns; i++) {
		move32((unsigned int *)&(dram_patterns[i]), (unsigned int *)pmem++);
		/*
		 * Put a different pattern on the data lines: otherwise they
		 * may float int enough to read back what we wrote.
		 */
		move32((unsigned int *)&otherpattern, (unsigned int *)pmem--);
		move32((unsigned int *)pmem, (unsigned int *)&temp32);

#ifdef INJECT_DATA_ERRORS
		temp32 ^= 0x00008000;
#endif

		if (temp32 != dram_patterns[i]){
			pat = plr_dram_patterns[i] & 0xffffffff;

			phyiscal = temp32 & 0xffffffff;

			MEMCTL_DEBUG_PRINTF("Memory (date line) error at %08x, "
				  "wrote %08x, read %08x !\n",
					  pmem, pat, phyiscal);
			ret = -1;
		}
	}
	return ret;
}

static int plr_dram_test_addrline(unsigned int  *testaddr, unsigned int  *base, unsigned int  size)
{
	unsigned int  *target;
	unsigned int  *end = 0;
	unsigned int  readback;
	unsigned int  xor;
	int   ret = 0;

	*end = (*testaddr + size);

	for(xor = sizeof(unsigned int ); xor > 0; xor <<= 1) {
		target = (unsigned int  *)((unsigned int )testaddr ^ xor);
		if((target >= base) && (target < end)) {
			*testaddr = ~*target;
			readback  = *target;

#ifdef INJECT_ADDRESS_ERRORS
			if(xor == 0x00008000) {
				readback = *testaddr;
			}
#endif
			if(readback == *testaddr) {
				MEMCTL_DEBUG_PRINTF("Memory (address line) error at %08x<->%08x, "
					"XOR value %08x !\n",
					testaddr, target, xor);
				ret = -1;
			}
		}
	}
	return ret;
}

int plr_dram_test_lines(unsigned int start, unsigned int size)
{
	int ret = 0;
	unsigned int phys_offset = 0;
	unsigned int memsize, dram_start;

	arch_dram_test_prepare(&dram_start, &memsize, &phys_offset);
	if ((start < dram_start) || ((CADDR(start)+size) > (dram_start+memsize)))
  {
  	printf("\n plr_dram_test_lines arguments error ");
  	return -1;
  }
	
  MEMCTL_DEBUG_PRINTF("\n dram data/address line test from 0x%x to 0x%x \n",start,((start+size)-1));
  
  plr_disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );
  
	ret = plr_dram_test_dataline((unsigned int *)start);
	if (!ret)
		ret = plr_dram_test_addrline((unsigned int  *)start, (unsigned int  *)start,
				size);
	if (!ret)
		ret = plr_dram_test_addrline((unsigned int  *)(start+size-8),
				(unsigned int  *)start, size);
				
	plr_enable_DRAM_prefech(MCR_PREFETCH_OLD_MECH, MCR_PREFETCH_DATA_SIDE);
	MEMCTL_DEBUG_PRINTF("\n dram data/address line test %s \n",ret?"FAILED":"PASS");
	return ret;
}
int plr_dram_test(unsigned int start,unsigned int size,int flags)
{
	unsigned int start_addr, test_area_size;
  u32_t dram_size;
  u32_t l;
  volatile u32_t *mcr, oldmcr;
  int testLoops=1;

	unsigned int phys_offset = 0;
	unsigned int memsize, dram_start;

	arch_dram_test_prepare(&dram_start, &memsize, &phys_offset);
	if ((start < dram_start) || ((CADDR(start)+size) > (dram_start+memsize)))
  {
  	printf("\n dram_test arguments error ");
  	return -1;
  }
	
  MEMCTL_DEBUG_PRINTF("\n dram test from 0x%x to 0x%x \n",start,((start+size)-1));
  
  if (-1 == plr_dram_test_lines(start,size))
  	return -1;
  	
  if ((flags & 0x1)==0)
  	return 0;

  if ((flags & 0x2)==0x2)
  {
  	pblr_memcpy((u8_t *)(start+size),(u8_t *)UBOOT_RESERVED_START,(u32_t)UBOOT_RESERVED_SIZE);
	}
	/*back the value of mcr*/
	mcr = (u32_t *)MCR;
    oldmcr = *mcr;
    dram_size = 0x10000;

	for(l=1; l<=testLoops; l++) {
		MEMCTL_DEBUG_PRINTF("\n[Round %d]\n", l);

		start_addr = start;
		test_area_size = size;

		/* disable prefetch mechanism */
		plr_disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );
		_cache_flush();
		MEMCTL_DEBUG_PRINTF("Range [0x%x~0x%x]\n", start_addr, start_addr+test_area_size-1);

		/*1. Uncached range */
	    MEMCTL_DEBUG_PRINTF("No prefetch, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",
				    UADDR(start_addr), dram_size, *mcr);
	    if(-1 == _plrdram_test(UADDR(start_addr), dram_size, test_area_size))
            return -1;

		/*2. Cached range without prefetch */
	    MEMCTL_DEBUG_PRINTF("\nNo prefetch, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",
	                CADDR(start_addr), dram_size, *mcr);
	    if(-1 == _plrdram_test(CADDR(start_addr), dram_size, test_area_size))
            return -1;

		plr_disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );

		/*3. Cached range with data prefetch mechanism */
		plr_enable_DRAM_prefech(MCR_PREFETCH_OLD_MECH, MCR_PREFETCH_DATA_SIDE);
		MEMCTL_DEBUG_PRINTF("\nEnable MCR_PREFETCH_DATA_SIDE, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",
	    		    CADDR(start_addr), dram_size, *mcr);
	   if(-1 == _plrdram_test(CADDR(start_addr), dram_size, test_area_size))
        	return -1;

		plr_disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );

		/*4. Cached range with instruction prefetch mechanism */
		plr_enable_DRAM_prefech(MCR_PREFETCH_OLD_MECH, MCR_PREFETCH_INS_SIDE);
	    MEMCTL_DEBUG_PRINTF("\nEnable MCR_PREFETCH_INS_SIDE, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",
	               CADDR(start_addr), dram_size, *mcr);
	    if(-1 == _plrdram_test(CADDR(start_addr), dram_size, test_area_size))
            return -1;

		plr_disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );


		/*5. Cached range with instruction/data prefetch mechanism */
		plr_enable_DRAM_prefech(MCR_PREFETCH_OLD_MECH, MCR_PREFETCH_DATA_SIDE | MCR_PREFETCH_INS_SIDE);
	    MEMCTL_DEBUG_PRINTF("\nEnable MCR_PREFETCH_DATA_SIDE/MCR_PREFETCH_INS_SIDE, DRAM Test start = 0x%x, DRAM Test Size = 0x%x, MCR = 0x%x\n",
	               CADDR(start_addr), dram_size, *mcr);
	    if(-1 == _plrdram_test(CADDR(start_addr), dram_size, test_area_size))
           return -1;

		plr_disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );

	}
	/* restore memory controller register */
    *mcr = oldmcr;
	_cache_flush();
    MEMCTL_DEBUG_PRINTF("\n\n");
  if ((flags & 0x2)==0x2)
  {
  	pblr_memcpy((u8_t *)UBOOT_RESERVED_START,(u8_t *)(start+size),(u32_t)UBOOT_RESERVED_SIZE);
	}
	return 0;
}

