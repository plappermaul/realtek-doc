/*
 * Include Files
 */
#include <soc.h>
#include <linux/ctype.h>
#include "memctl.h"

/* Definitions for memory test error handing manner */
#define MT_SUCCESS    (0)
#define MT_FAIL       (-1)

/* Definitions for memory test reset mode */
enum RESET_MODE{
	NO_RESET = 0,
	UBOOT_RESET = 1,
	WHOLE_CHIP_RESET = 2,
};

//The default seting of memory test error handling manner is non-blocking
//Using "Environment parameter" or "command flag setting" can change this
//"Environment parameter": setenv mt_freeze_block debug
//"command flag setting": mdram_test/mflash_test -b/-mt_block
static u32_t g_err_handle_block_mode=0;
static u32_t g_reset_flag=NO_RESET;
static char *pstring;
static char *fstring;

#define HANDLE_FAIL	  \
	({ \
		printf("%s (%d) test failed.\n", __FUNCTION__,__LINE__); \
		if(!g_err_handle_block_mode){ \
			return MT_FAIL; \
		}else{ \
			while(1); \
		} \
	})

#define _cache_flush	(((soc_t *)(0x9f000020))->bios).dcache_writeback_invalidate_all


extern void _machine_restart(void);

static uint32_t
dram_psize_b(void) {
	const uint8_t BNKCNTv[] = {1, 2, 3};
	const uint8_t BUSWIDv[] = {0, 1, 2};
	const uint8_t ROWCNTv[] = {11, 12, 13, 14, 15, 16};
	const uint8_t COLCNTv[] = {8, 9, 10, 11, 12};

	const uint32_t dcr = REG32(0xb8001004);
	uint32_t size = 1 << (BNKCNTv[(dcr >> 28) & 0x3] +
	                      BUSWIDv[(dcr >> 24) & 0x3] +
	                      ROWCNTv[(dcr >> 20) & 0xF] +
	                      COLCNTv[(dcr >> 16) & 0xF]);
	return size;
}

static uint32_t
dram_vsize_b(void) {
	const uint32_t max_dram_vsize_b = 256*1024*1024;
	return (dram_psize_b() > max_dram_vsize_b) ? max_dram_vsize_b : dram_psize_b();
}

/*
 * DRAM TEST RANGES:
 * TEST_AREA1: From 0 ~ (UBOOT_BASE - SIZE_3MB)
 * TEST_AREA2: From (UBOOT_BASE + SIZE_3MB) ~ (DRAM_END - SIZE_5MB)
 */
#define SIZE_3MB            (0x00300000)
#define SIZE_16MB           (0x01000000)
#define VA_TO_PA(VAddr)     (VAddr&0x1FFFFFFF)	//Physical address
#define TEST_AREA_BASE1     (unsigned int)(0x80000000)
#define TEST_AREA_BASE1_END (unsigned int)(CONFIG_SYS_TEXT_BASE - SIZE_3MB)
#define TEST_AREA_SIZE1	    (unsigned int)(TEST_AREA_BASE1_END&0x1FFFFFFF)
#define TEST_AREA_BASE2     (unsigned int)(CONFIG_SYS_TEXT_BASE + SIZE_3MB)
#define TEST_AREA_SIZE2     (unsigned int)(dram_vsize_b() - SIZE_16MB - (TEST_AREA_BASE2&0x1FFFFFFF))
#define TEST_AREA_BASE2_END (unsigned int)(TEST_AREA_BASE2 + TEST_AREA_SIZE2)

#define MEMCTL_DEBUG_PRINTF printf

const unsigned int dram_patterns[] =	{
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

const unsigned int line_toggle_pattern[] = {
	0xF0F0F0F0,
	0x0F0F0F0F,
	0xFF00FF00,
	0x00FF00FF,
	0x00000000,
	0xFFFFFFFF,
	0x5A5A5A5A,
	0xA5A5A5A5
};

const unsigned int toggle_pattern[] = {
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

/**************************
 * Command Parsing
 *************************/
typedef struct {
	u32_t start_addr[2];
	u32_t test_size[2];
	u8_t area_num;
	u16_t test_loops;
} ddr_cmd_parsing_info_t;


/*
 * Function Declaration
 */
//extern unsigned int board_DRAM_freq_mhz(void);

void (*f)(void) = (void *) 0xbfc00000;


int dram_normal_patterns(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i, j;
	u32_t start_value;
	/* This constraint is for the prefetch feature,
	   since it does NOT look over DRAM size and make it overflow. */
	unsigned int addr_lim = dram_start + area_size - 32 - 4;
	volatile u32_t *read_start;
	volatile u32_t *start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < (sizeof(dram_patterns)/sizeof(u32_t)); i++) {
		_cache_flush();

		/* write pattern*/
		start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
		read_start = (u32_t *)(UADDR((u32_t)start+dram_size-4));
		for(j=0; j < dram_size; j=j+4) {
			if ((u32_t)start > addr_lim) {
				read_start = (u32_t *)(UADDR((u32_t)start-4));
				dram_size = j;
				break;
			}
			*start = dram_patterns[i];
			start++;
		}

		_cache_flush();

		/* check data */
		for(j=0; j < dram_size; j=j+4) {
			start_value = (*read_start);
			if(start_value != dram_patterns[i]) {
				printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n", \
				       (u32_t)read_start , start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			read_start--;
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

int dram_walking_of_1(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i, j;
	u32_t walk_pattern, start_value;
	/* This constraint is for the prefetch feature,
	   since it does NOT look over DRAM size and make it overflow. */
	unsigned int addr_lim = dram_start + area_size - 32 - 4;
	volatile u32_t *read_start;
	volatile u32_t *start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i++) {
		_cache_flush();

		/* generate pattern */
		walk_pattern = (1 << i);

		/* write pattern*/
		start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
		read_start = (u32_t *)(UADDR((u32_t)start+dram_size-4));
		for (j=0; j < dram_size; j=j+4) {
			if ((u32_t)start > addr_lim) {
				read_start = (u32_t *)(UADDR((u32_t)start-4));
				dram_size = j;
				break;
			}
			*start = walk_pattern;
			start++;
		}

		_cache_flush();

		/* check data */
		for (j=0; j < dram_size; j=j+4) {
			start_value = (*read_start);
			if (start_value != walk_pattern) {
				printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n", \
				       (u32_t)read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			read_start--;
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}



int dram_walking_of_0(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i, j;
	u32_t start_value;
	u32_t walk_pattern;
	/* This constraint is for the prefetch feature,
	   since it does NOT look over DRAM size and make it overflow. */
	unsigned int addr_lim = dram_start + area_size - 32 - 4;
	volatile u32_t *start;
	volatile u32_t *read_start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i++) {
		_cache_flush();

		/* generate pattern */
		walk_pattern = ~(1 << i);

		/* write pattern*/
		start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
		read_start = (u32_t *)(UADDR(((u32_t)start)+dram_size-4));
		for (j=0; j < dram_size; j=j+4) {
			if ((u32_t)start > addr_lim) {
				read_start = (u32_t *)(UADDR((u32_t)start-4));
				dram_size = j;
				break;
			}
			*start = walk_pattern;
			start++;
		}

		_cache_flush();

		/* check data */
		for (j=0; j < dram_size; j=j+4) {
			start_value = (*read_start);
			if (start_value != walk_pattern) {
				printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n", \
				       (u32_t)read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			read_start--;
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

int dram_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i, j;
	/* This constraint is for the prefetch feature,
	   since it does NOT look over DRAM size and make it overflow. */
	unsigned int addr_lim = dram_start + area_size - 32 - 4;
	u32_t start_value, read_start_addr;
	volatile u32_t *start;
	volatile u32_t *read_start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i=i+4) {
		_cache_flush();
		/* write pattern*/
		start		= (u32_t *)(dram_start + ((i/4)*dram_size)%(area_size));
		read_start 	= (u32_t *)((u32_t)start + dram_size-4);
		if ((u32_t)read_start > addr_lim) {
			read_start = (u32_t *)addr_lim;
		}
		read_start_addr = ((u32_t)start);
		start = (u32_t *)(UADDR(((u32_t)start)));
		for(j=0; j < dram_size; j=j+4) {
			if (read_start_addr > addr_lim) break;
			*start = (read_start_addr << i);
			start++;
			read_start_addr = read_start_addr + 4 ;
		}

		_cache_flush();
		read_start_addr = ((u32_t)read_start);

		/* check data reversing order */
		for (; j > 0; j=j-4) {
			start_value = (*read_start);
			if(start_value != ((read_start_addr) << i)) {
				printf("\ndecr addr(0x%x): 0x%x != pattern(0x%x) %s, %d i=%d j=0x%x\n", \
				       (u32_t)read_start , start_value, ((read_start_addr) << i), \
				       __FUNCTION__, __LINE__, i,j);
				HANDLE_FAIL;
			}
			read_start_addr = read_start_addr - 4;
			read_start--;
		}

		read_start_addr += 4;
		read_start++;

		/* check data sequential order */
		for(j=0; j < dram_size; j=j+4) {
			if ((u32_t)read_start > addr_lim) break;
			start_value = (*read_start);
			if(start_value != ((read_start_addr) << i)) {
				printf("\nseq addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n", \
				       (u32_t)read_start , start_value, ((read_start_addr) << i), \
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			read_start_addr = read_start_addr + 4;
			read_start++;
		}
		MEMCTL_DEBUG_PRINTF("rotate %d 0x%x passed\r", i, (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

int dram_com_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i, j;
	/* This constraint is for the prefetch feature,
	   since it does NOT look over DRAM size and make it overflow. */
	unsigned int addr_lim = dram_start + area_size - 32 - 4;
	u32_t start_value, read_start_addr;
	volatile u32_t *start;
	volatile u32_t *read_start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i<32; i=i+4) {
		_cache_flush();
		/* write pattern*/
		start		= (u32_t *)(dram_start + ((i/4)*dram_size)%(area_size));
		read_start 	= (u32_t *)((u32_t)start + dram_size-4);
		if ((u32_t)read_start > addr_lim) {
			read_start = (u32_t *)addr_lim;
		}
		read_start_addr = ((u32_t)start);
		start = (u32_t *)(UADDR(((u32_t)start)));
		for (j=0; j<dram_size; j=j+4) {
			if (read_start_addr > addr_lim) break;
			*start = ~(read_start_addr << i);
			start++;
			read_start_addr = read_start_addr + 4 ;
		}

		_cache_flush();
		read_start_addr = ((u32_t)read_start);

		/* check data reversing order */
		for (; j > 0; j=j-4) {
			start_value = (*read_start);
			if (start_value != (~(read_start_addr << i))) {
				printf("\ndecr addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n", \
				       (u32_t)read_start , start_value, ~((read_start_addr) << i), \
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			read_start_addr = read_start_addr - 4;
			read_start--;
		}

		read_start_addr += 4;
		read_start++;

		/* check data sequential order */
		for (j=0; j<dram_size; j=j+4) {
			if ((u32_t)read_start > addr_lim) break;
			start_value = (*read_start);
			if(start_value != (~(read_start_addr << i))) {
				printf("\nseq addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n", \
				       (u32_t)read_start , start_value, ~((read_start_addr) << i), \
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			read_start_addr = read_start_addr + 4;
			read_start++;
		}
		MEMCTL_DEBUG_PRINTF("~rotate %d 0x%x passed\r", i, (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

/*
 * write two half-words and read word.
 */
int dram_half_word_access(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i, j;
	u16_t h_word;
	u32_t start_value, test_start;
	u32_t addr_lim = dram_start + area_size - 32 - 4;
	volatile u16_t *start_h;
	volatile u32_t *start_w;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < (sizeof(dram_patterns)/sizeof(u32_t)); i++) {
		_cache_flush();

		/* write half-word pattern*/
		start_h = (u16_t *)(dram_start+ (i*dram_size)%(area_size));
		start_w = (u32_t *)(UADDR(dram_start+ (i*dram_size)%(area_size)+dram_size-4));
		test_start = UADDR((u32_t)start_h);

		for(j=0; j < dram_size; j=j+4) {
			if ((u32_t)start_h > addr_lim) {
				start_w = (u32_t *)(UADDR((u32_t)start_h - 4) & (~(0x3)));
				break;
			}
			h_word = (u16_t)(dram_patterns[i]);
			*(start_h+1) = h_word;
			h_word = (u16_t)(dram_patterns[i] >> 16);
			*start_h = h_word;
			start_h+=2;
		}

		_cache_flush();

		/* read word and check data */
		for (j=0; j<dram_size; j=j+4) {
			if ((u32_t)start_w < test_start) {
				break;
			}
			start_value = (*start_w);
			if (start_value != dram_patterns[i]) {
				printf("\naddr:0x%x(0x%x) != pattern(0x%x) %s, %d\n", \
				       (u32_t)start_w, start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			start_w--;
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], (u32_t)start_h);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

int dram_byte_access(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i, j;
	u8_t byte;
	u32_t start_value, test_start;
	u32_t addr_lim = dram_start + area_size - 32 - 4;
	volatile u8_t *start_b;
	volatile u32_t *start_w;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i<(sizeof(dram_patterns)/sizeof(u32_t)); i++) {
		_cache_flush();

		/* write byte pattern*/
		start_w = (u32_t *)(UADDR(dram_start+(i*dram_size)%(area_size)+dram_size-4));
		start_b = (u8_t *)(dram_start+(i*dram_size)%(area_size));
		test_start = UADDR((u32_t)start_b);

		for (j=0; j<dram_size; j=j+4) {
			if ((u32_t)start_b > addr_lim) {
				start_w = (u32_t *)(UADDR((u32_t)start_b - 4) & (~(0x3)));
				break;
			}
			byte = (u8_t)(dram_patterns[i]);
			*(start_b+3) = byte;
			byte = (u8_t)(dram_patterns[i] >> 8);
			*(start_b+2) = byte;
			byte = (u8_t)(dram_patterns[i] >> 16);
			*(start_b+1) = byte;
			byte = (u8_t)(dram_patterns[i] >> 24);
			*(start_b) = byte;
			start_b+=4;
		}

		_cache_flush();

		/* read word and check data */
		for (j=0; j<dram_size; j=j+4) {
			if ((u32_t)start_w < test_start) {
				break;
			}
			start_value = *start_w;
			if (start_value != dram_patterns[i]) {
				printf("\naddr:0x%x(0x%x) != pattern(0x%x) %s, %d\n", \
				       (u32_t)start_w, start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			start_w--;
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], (u32_t)start_b);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

int memcpy_test(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i, j;
	u32_t start_value, read_start_addr;
	/* This constraint is for the prefetch feature,
	   since it does NOT look over DRAM size and make it overflow. */
	unsigned int addr_lim = dram_start + area_size - 32 - 4;
	volatile u32_t *start;
	volatile u32_t *read_start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i=i+4) {
		_cache_flush();

		/* write pattern*/
		start = (u32_t *)(dram_start + (i*dram_size)%(area_size));

		if (((u32_t)start + dram_size) > addr_lim) {
			break;
		}

		read_start_addr = ((u32_t)start + (dram_size-4));
		read_start = (u32_t *)(UADDR(((u32_t)start)+dram_size-4));
		for (j=0; j < dram_size; j=j+4) {
			*start = ((u32_t)start << i);
			start++;
		}

		memcpy((char *)(dram_start+dram_size), (char *)(dram_start + (i*dram_size)%(area_size)), dram_size);

		_cache_flush();

		/* check uncached data */
		read_start = (u32_t *)(dram_start+dram_size+dram_size-4);
		for (j=0; j<dram_size; j=j+4) {
			start_value = (*read_start);
			if (start_value != ((read_start_addr) << i)) {
				printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n", \
				       (u32_t)read_start , start_value, ((read_start_addr) << i), \
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			read_start = read_start - 1;
			read_start_addr = read_start_addr - 4;
		}
		MEMCTL_DEBUG_PRINTF("memcpy %d 0x%x passed\r", i, (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
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
int unaligned_test(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	unsigned int i, j;
	/* This constraint is for the prefetch feature,
	   since it does NOT look over DRAM size and make it overflow. */
	unsigned int addr_lim = dram_start + area_size - 32 - 4;
	volatile t_off_1 *off1;
	volatile t_off_2 *off2;
	volatile t_off_3 *off3;

	off1 = (t_off_1 *)dram_start;
	off2 = (t_off_2 *)dram_start;
	off3 = (t_off_3 *)dram_start;

	/* patterns loop */
	for(j=0; j<sizeof(dram_patterns)/sizeof(unsigned int);j++) {
		/* offset 1 bytes */
		off1 = (t_off_1 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_1));i++){
			if ((u32_t)(&off1[i].w) > addr_lim) break;
			off1[i].c1 = 0xcc;
			off1[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_1));i++){
			if ((u32_t)(&off1[i].w) > addr_lim) break;
			if((off1[i].w != dram_patterns[j]) || (off1[i].c1 != 0xcc)){
				printf("\noffset 1 error:addr(0x%x) write 0x%x, read 0x%x, c1(%02x)\n", \
				       (u32_t)&off1[i], dram_patterns[j], off1[i].w, off1[i].c1);
				HANDLE_FAIL;
			}
		}

		/* offset 2 bytes */
		off2 = (t_off_2 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_2));i++){
			if ((u32_t)(&off2[i].w) > addr_lim) break;
			off2[i].c1 = 0xcc;
			off2[i].c2 = 0xdd;
			off2[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_2));i++){
			if ((u32_t)(&off2[i].w) > addr_lim) break;
			if(off2[i].w != dram_patterns[j] || (off2[i].c1 != 0xcc) || (off2[i].c2 != 0xdd)){
				printf("\noffset 2 error:addr(0x%x) write 0x%x, read 0x%x, c1(0x%x), c2(0x%x)\n", \
				       (u32_t)&off2[i], dram_patterns[j], off2[i].w, off2[i].c1, off2[i].c2);
				printf("&dram_pattern[%d](0x%p) = 0x%x\r", j, &dram_patterns[j], dram_patterns[j]);
				HANDLE_FAIL;
			}
		}

		/* offset 3 bytes */
		off3 = (t_off_3 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_3));i++){
			if ((u32_t)(&off3[i].w) > addr_lim) break;
			off3[i].c1 = 0xcc;
			off3[i].c2 = 0xdd;
			off3[i].c3 = 0xee;
			off3[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_3));i++){
			if ((u32_t)(&off3[i].w) > addr_lim) break;
			if(off3[i].w != dram_patterns[j] ||(off3[i].c1 != 0xcc) || (off3[i].c2 != 0xdd) || (off3[i].c3 != 0xee)){
				printf("\noffset 3 error:addr(0x%x) write 0x%x, "\
				       "read 0x%x, c1(%02x), c2(%02x), c3(%02x)\n", \
				       (u32_t)&off1[i], dram_patterns[j], off3[i].w, off3[i].c1, \
				       off3[i].c2, off3[i].c3);
				HANDLE_FAIL;
			}
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", j, dram_patterns[j], (u32_t)off3);
	}
	MEMCTL_DEBUG_PRINTF("%s test passed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}


void disable_DRAM_prefech(unsigned int side_id)
{
	volatile u32_t *reg_mcr;

	reg_mcr = (volatile u32_t *)0xB8001000;

	if( side_id & MCR_PREFETCH_INS_SIDE )
		*reg_mcr =*reg_mcr & ((unsigned int)MCR_PREFETCH_DIS_IMASK);

	if( side_id & MCR_PREFETCH_DATA_SIDE)
		*reg_mcr =*reg_mcr & ((unsigned int)MCR_PREFETCH_DIS_DMASK);
}

void enable_DRAM_prefech(unsigned int side_id)
{
	volatile u32_t *reg_mcr;

	reg_mcr = (volatile u32_t *)0xB8001000;

	if( side_id & MCR_PREFETCH_INS_SIDE )
		{
			disable_DRAM_prefech(MCR_PREFETCH_INS_SIDE);
			*reg_mcr = *reg_mcr | MCR_PREFETCH_ENABLE_INS;
		}

	if( side_id & MCR_PREFETCH_DATA_SIDE )
		{
			disable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE);
			*reg_mcr = *reg_mcr | MCR_PREFETCH_ENABLE_DATA;
		}
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
static void __srandom32(unsigned int *a1, unsigned int *a2, unsigned int *a3, unsigned int get_or_set)
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

int cache_flush_adj_addr(unsigned int addr_base, unsigned int run_times,
                         unsigned int random,    unsigned int byteset,
                         unsigned int addr_lim) {
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	int retcode;

	retcode = MT_SUCCESS;

	for (test_times = 0; test_times < run_times; test_times++) {
		MEMCTL_DEBUG_PRINTF("ADDRESS WORDSET addr_base: 0x%08x times: %d, pattern: Address\r", addr_base, test_times);
		pdata = (unsigned int *)(UADDR(addr_base));
		addr_lim = UADDR(addr_lim);
		/* Initial DCache */
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			if ((u32_t)pdata > addr_lim) {
				break;
			}
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		addr_lim = CADDR(addr_lim);
		/* Read data into DCache */
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			if ((u32_t)pdata > addr_lim) {
				break;
			}
			data = *pdata;
			pdata++;
		}

		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		addr_lim = CADDR(addr_lim);
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			if ((u32_t)pdata > addr_lim) {
				break;
			}
			*pdata = (unsigned int)pdata;
			pdata++;
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
		addr_lim = CADDR(addr_lim);
		/* varify the data */
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			data = *pdata;
			if ((u32_t)pdata > addr_lim) {
				break;
			}
			if (data != ((unsigned int)pdata)) {
				printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
				       (u32_t)pdata, data , ((unsigned int)pdata)
				       ,  __FUNCTION__, __LINE__);
				retcode = MT_FAIL;
			}
			pdata++;
		}
	}
	return (retcode);
}

int cache_flush_adjacent(unsigned int addr_base, unsigned int run_times,
                         unsigned int random,    unsigned int byteset,
                         unsigned int addr_lim) {
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value, a, b, c;
	int retcode;
	unsigned char vbyte;

	retcode = MT_SUCCESS;

	if (UADDR(addr_base + CONFIG_SYS_DCACHE_SIZE) > UADDR(addr_lim)) {
		return retcode;
	}

	vbyte = 0;

	write_value = INIT_VALUE;
	a=0x13243;b=0xaaa0bdd;c=0xfffbda0;
	__srandom32(&a, &b, &c, SET_SEED);
	for (test_times = 0; test_times < run_times; test_times++) {
		if (random == 1) {
			MEMCTL_DEBUG_PRINTF("\rRANDOM ");
			write_value = __random32();
		} else {
			MEMCTL_DEBUG_PRINTF("\rFIXED  ");
		}

		if (byteset == 1) {
			MEMCTL_DEBUG_PRINTF("\rBYTESET ");
			vbyte = (unsigned char)write_value;
			write_value = ((unsigned int)vbyte | (((unsigned int)vbyte)<<8) |
			               (((unsigned int)vbyte)<<16) | (((unsigned int)vbyte)<<24));
		} else {
			MEMCTL_DEBUG_PRINTF("\rWORDSET ");
		}

		MEMCTL_DEBUG_PRINTF("addr_base: 0x%08x times: %d, pattern: 0x%08x\r", addr_base, test_times, write_value);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Initial DCache */
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			data = *pdata;
			pdata++;
		}

		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		if (byteset == 1) {
			memset((void *)pdata, vbyte, CONFIG_SYS_DCACHE_SIZE);
		} else {
			for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
				*pdata = write_value;
				pdata++;
			}
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i=i+4) {
			data = *pdata;
			if (data != write_value) {
				printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
				       (u32_t)pdata, data , write_value,
				       __FUNCTION__, __LINE__);
				retcode = MT_FAIL;
			}
			pdata++;
		}
	}
	return (retcode);
}


int cache_flush_dispersed (unsigned int addr_base, unsigned int run_times,
                           unsigned int random, unsigned int byteset,
                           unsigned int addr_lim) {
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value, a, b, c;
	int retcode;
	unsigned char vbyte;

	retcode = MT_SUCCESS;
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
				MEMCTL_DEBUG_PRINTF("RANDOM ");
				write_value = __random32();
			}
			else
				MEMCTL_DEBUG_PRINTF("FIXED  ");

			if(byteset == 1){
				MEMCTL_DEBUG_PRINTF("BYTESET ");
				vbyte = (unsigned char)write_value;
				write_value = ((unsigned int)vbyte | (((unsigned int)vbyte)<<8) \
				               | (((unsigned int)vbyte)<<16) | (((unsigned int)vbyte)<<24));
			}
			else
				MEMCTL_DEBUG_PRINTF("WORDSET ");

			MEMCTL_DEBUG_PRINTF("addr_base: 0x%08x times: %d, pattern: 0x%08x\r", addr_base, test_times, write_value);
			addr_lim = UADDR(addr_lim);
			pdata = (unsigned int *)(UADDR(addr_base));
			/* Dirtify DCache */
			for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4)
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

					if ((unsigned int)pdata >= addr_lim) {
						//MEMCTL_DEBUG_PRINTF("\n WW: pdata addr(0x%08x) >= addr_lim(0x%08x)\n",
						//    (unsigned int)pdata, addr_lim);
						break;
					}
				}

			addr_lim = CADDR(addr_lim);
			pdata = (unsigned int *)(CADDR(addr_base));
			/* Read data into DCache */
			for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4)
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

					if ((unsigned int)pdata >= addr_lim) {
						//MEMCTL_DEBUG_PRINTF("\n WW: pdata addr(0x%08x) >= addr_lim(0x%08x)\n",
						//    (unsigned int)pdata, addr_lim);
						break;
					}
				}

			addr_lim = CADDR(addr_lim);
			pdata = (unsigned int *)(CADDR(addr_base));
			if(byteset == 1){
				memset((void *)pdata, vbyte, CONFIG_SYS_DCACHE_SIZE);
			}
			/* Dirtify DCache */
			for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4)
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

					if ((unsigned int)pdata >= addr_lim) {
						//MEMCTL_DEBUG_PRINTF("\n WW: pdata addr(0x%08x) >= addr_lim(0x%08x)\n",
						//    (unsigned int)pdata, addr_lim);
						break;
					}
				}
			/* write back and invalidate DCache */
			_cache_flush();

			addr_lim = CADDR(addr_lim);
			pdata = (unsigned int *)(CADDR(addr_base));
			/* varify the data */
			for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4)
				{
					i = i + 28;
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);

					if ((unsigned int)pdata >= addr_lim) {
						//MEMCTL_DEBUG_PRINTF("\n WW: pdata addr(0x%08x) >= addr_lim(0x%08x)\n",
						//    (unsigned int)pdata, addr_lim);
						break;
					}
				}
		}
	return (retcode);
}

int cache_flush_adjacent_toggle_word(unsigned int addr_base, unsigned int run_times,
                                     unsigned int addr_lim) {
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode;

	retcode = MT_SUCCESS;

	for(test_times = 0; test_times < run_times; test_times++) {
		write_value = toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];

		MEMCTL_DEBUG_PRINTF("addr_base: 0x%08x times: %d, pattern: 0x%08x\r", addr_base, test_times, write_value);
		pdata = (unsigned int *)(UADDR(addr_base));
		addr_lim = UADDR(addr_lim);
		/* Initial DCache */
		for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			if ((unsigned int)pdata >= addr_lim) {
				break;
			}
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		addr_lim = CADDR(addr_lim);
		/* Read data into DCache */
		for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			if ((unsigned int)pdata >= addr_lim) {
				break;
			}
			data = *pdata;
			pdata++;
		}

		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		addr_lim = CADDR(addr_lim);
		for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			if ((unsigned int)pdata >= addr_lim) {
				break;
			}
			*pdata = write_value;
			pdata++;
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
		addr_lim = CADDR(addr_lim);
		/* varify the data */
		for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			if ((unsigned int)pdata >= addr_lim) {
				break;
			}
			data = *pdata;
			if(data != write_value) {
				printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
				       (u32_t)pdata, data , write_value,
				       __FUNCTION__, __LINE__);
				retcode = MT_FAIL;
			}
			pdata++;
		}
	}
	return (retcode);
}

int cache_flush_dispersed_toggle_word (unsigned int addr_base, unsigned int run_times,
                                       unsigned int addr_lim)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode;

	retcode = MT_SUCCESS;
	addr_lim -= 32;

	for(test_times = 0; test_times < run_times; test_times++)
		{
			write_value = toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
			MEMCTL_DEBUG_PRINTF("addr_base: 0x%08x times: %d, pattern: 0x%08x\r", addr_base, test_times, write_value);
			addr_lim = UADDR(addr_lim);
			pdata = (unsigned int *)(UADDR(addr_base));
			/* Dirtify DCache */
			for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4)
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

					if ((unsigned int)pdata >= addr_lim) {
						break;
					}
				}

			addr_lim = CADDR(addr_lim);
			pdata = (unsigned int *)(CADDR(addr_base));
			/* Read data into DCache */
			for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4)
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

					if ((unsigned int)pdata >= addr_lim) {
						break;
					}
				}


			addr_lim = CADDR(addr_lim);
			pdata = (unsigned int *)(CADDR(addr_base));
			/* Dirtify DCache */
			for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4)
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

					if ((unsigned int)pdata >= addr_lim) {
						break;
					}
				}

			/* write back and invalidate DCache */
			_cache_flush();

			addr_lim = CADDR(addr_lim);
			pdata = (unsigned int *)(CADDR(addr_base));
			/* varify the data */
			for(i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4)
				{
					i = i + 28;
					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					pdata++;

					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					pdata++;

					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					pdata++;

					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					pdata++;


					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					pdata++;

					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					pdata++;

					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					pdata++;


					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
							       (u32_t)pdata, data , write_value,
							       __FUNCTION__, __LINE__);
							retcode = MT_FAIL;
						}
					pdata++;
					pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);

					if ((unsigned int)pdata >= addr_lim) {
						break;
					}
				}

		}
	return (retcode);
}


int cache_flush_adjacent_toggle_line128(unsigned int addr_base, unsigned int run_times,
                                        unsigned int addr_lim) {
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value[4];
	int retcode;

	retcode = MT_SUCCESS;

	for (test_times = 0; test_times < run_times; test_times++) {
		write_value[0] = line_toggle_pattern[(test_times%(sizeof(line_toggle_pattern)/sizeof(unsigned int)))];
		write_value[1] = line_toggle_pattern[(test_times%(sizeof(line_toggle_pattern)/sizeof(unsigned int)))];
		write_value[2] = ~line_toggle_pattern[(test_times%(sizeof(line_toggle_pattern)/sizeof(unsigned int)))];
		write_value[3] = ~line_toggle_pattern[(test_times%(sizeof(line_toggle_pattern)/sizeof(unsigned int)))];
		MEMCTL_DEBUG_PRINTF("addr_base: 0x%08x times: %d, pattern: 0x%08x\r", addr_base, test_times, write_value[0]);
		pdata = (unsigned int *)(UADDR(addr_base));
		addr_lim = UADDR(addr_lim);
		/* Initial DCache */
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			if ((u32_t)pdata > addr_lim) {
				break;
			}
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		addr_lim = CADDR(addr_lim);
		/* Read data into DCache */
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+4) {
			if ((u32_t)pdata > addr_lim) {
				break;
			}
			data = *pdata;
			pdata++;
		}

		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		addr_lim = CADDR(addr_lim);
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+16) {
			if ((u32_t)pdata > addr_lim) {
				break;
			}
			*pdata++ = write_value[0];
			*pdata++ = write_value[1];
			*pdata++ = write_value[2];
			*pdata++ = write_value[3];
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
		addr_lim = CADDR(addr_lim);
		/* varify the data */
		for (i=0; i<CONFIG_SYS_DCACHE_SIZE; i = i+16) {
			if ((u32_t)pdata > addr_lim) {
				break;
			}
			data = *pdata;
			if (data != write_value[0]) {
				printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
				       (u32_t)pdata, data , write_value[0],
				       __FUNCTION__, __LINE__);
				retcode = MT_FAIL;
			}
			pdata++;
			data = *pdata;
			if (data != write_value[1]) {
				printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
				       (u32_t)pdata, data , write_value[1],
				       __FUNCTION__, __LINE__);
				retcode = MT_FAIL;
			}
			pdata++;
			data = *pdata;
			if(data != write_value[2]) {
				printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
				       (u32_t)pdata, data , write_value[2],
				       __FUNCTION__, __LINE__);
				retcode = MT_FAIL;
			}
			pdata++;
			data = *pdata;
			if (data != write_value[3]) {
				printf("\npdata(0x%08x) 0x%08x != 0x%08x, %s, %d\n",
				       (u32_t)pdata, data , write_value[3],
				       __FUNCTION__, __LINE__);
				retcode = MT_FAIL;
			}
			pdata++;
		}
	}
	return (retcode);
}

int cache_flush_test(u32_t dram_start, u32_t dram_size, u32_t area_size) {
    int retcode=MT_SUCCESS;
    unsigned int addr_base;
    unsigned int test_times;
    /* This constraint is for the prefetch feature,
             since it does NOT look over DRAM size and make it overflow. */
    unsigned int addr_lim = dram_start + area_size - 32 - 4;
    test_times = TEST_TIMES;

    for (addr_base = dram_start;
        addr_base < (dram_start + area_size);
        addr_base = addr_base + dram_size) {

        retcode = cache_flush_dispersed(addr_base, test_times, 0, 0, addr_lim);
        if (retcode < 0) {
            printf("cache_flush_dispersed(0,0) error\n");
            HANDLE_FAIL;
        }
        retcode = cache_flush_dispersed(addr_base, test_times, 0, 1, addr_lim);
        if (retcode < 0) {
            printf("cache_flush_dispersed(0,1) error\n");
            HANDLE_FAIL;
        }
        retcode = cache_flush_dispersed(addr_base, test_times, 1, 0, addr_lim);
        if (retcode < 0) {
            printf("cache_flush_dispersed(1,0) error\n");
            HANDLE_FAIL;
        }
        retcode = cache_flush_dispersed(addr_base, test_times, 1, 1, addr_lim);
        if (retcode < 0) {
            printf("cache_flush_dispersed(1,1) error\n");
            HANDLE_FAIL;
        }

		retcode = cache_flush_adjacent(addr_base, test_times, 0, 0, addr_lim);
		if (retcode < 0) {
			printf("cache_flush_adjacent(0,0) error\n");
			HANDLE_FAIL;
		}
		retcode = cache_flush_adjacent(addr_base, test_times, 1, 0, addr_lim);
		if (retcode < 0) {
			printf("cache_flush_adjacent(0,1) error\n");
			HANDLE_FAIL;
		}
		retcode = cache_flush_adjacent(addr_base, test_times, 0, 1, addr_lim);
		if (retcode < 0) {
			printf("cache_flush_adjacent(1,0) error\n");
			HANDLE_FAIL;
		}
		retcode = cache_flush_adjacent(addr_base, test_times, 1, 1, addr_lim);
		if (retcode < 0) {
			printf("cache_flush_adjacent(1,1) error\n");
			HANDLE_FAIL;
		}
		retcode = cache_flush_adj_addr(addr_base, test_times, 1, 1, addr_lim);
		if (retcode < 0) {
			printf("cache_flush_adj_addr error\n");
			HANDLE_FAIL;
		}
		retcode = cache_flush_adjacent_toggle_word(addr_base, test_times, addr_lim);
		if (retcode < 0) {
			printf("cache_flush_adjacent_toggle_word error\n");
			HANDLE_FAIL;
		}
		retcode = cache_flush_dispersed_toggle_word(addr_base, test_times, addr_lim);
		if (retcode < 0) {
			printf("cache_flush_dispersed_toggle_word error\n");
			HANDLE_FAIL;
		}
		retcode = cache_flush_adjacent_toggle_line128(addr_base, test_times, addr_lim);
		if (retcode < 0) {
			printf("cache_flush_adjacent_toggle_line128 error\n");
			HANDLE_FAIL;
		}
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.                                  \r", __FUNCTION__);
	return retcode;
}

int _dram_test(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	if(MT_FAIL == unaligned_test( dram_start, dram_size, area_size))
		return MT_FAIL;

	if(MT_FAIL == dram_addr_rot(dram_start, dram_size, area_size))
		return MT_FAIL;

	if(MT_FAIL == dram_com_addr_rot(dram_start, dram_size, area_size))
		return MT_FAIL;

	if(MT_FAIL == dram_byte_access(dram_start, dram_size, area_size))
		return MT_FAIL;

	if(MT_FAIL == dram_half_word_access(dram_start, dram_size, area_size))
		return MT_FAIL;

	if(MT_FAIL == dram_normal_patterns(dram_start, dram_size, area_size))
		return MT_FAIL;

	if(MT_FAIL == dram_walking_of_1(dram_start, dram_size, area_size))
		return MT_FAIL;

	if(MT_FAIL == dram_walking_of_0(dram_start, dram_size, area_size))
		return MT_FAIL;

	if(MT_FAIL == memcpy_test(dram_start, dram_size, area_size))
		return MT_FAIL;

	if(MT_FAIL == cache_flush_test( dram_start, dram_size, area_size))
		return MT_FAIL;

	return MT_SUCCESS;
}


int ddr_cmd_parsing(int argc, char * const argv[], ddr_cmd_parsing_info_t *info) {
	u32_t i, loop_cnt_tmp;

#define ILL_CMD	  \
	({ \
		printf("ERR: Illegal command (%d).\n",__LINE__); \
		return MT_FAIL; \
	})

	/* Initialize the memory test parameters..... */
	g_err_handle_block_mode = 0;
	info->area_num      = 2;
	info->test_loops    = 1;
	info->start_addr[0] = TEST_AREA_BASE1;
	info->start_addr[1] = TEST_AREA_BASE2;
	info->test_size[0]  = TEST_AREA_SIZE1;
	info->test_size[1]  = TEST_AREA_SIZE2;

	pstring = VZERO;
	fstring = VZERO;
	/* Parse the environment parameter for mt (non-)blocking error mode */
	g_err_handle_block_mode = env_get_ulong("mt_block_e", 10, 0);
	/* Parse command flag for test range / test loops / mt error (non-)blocking mode */
	for (i=1; i<argc;){
		if('-' != *argv[i]) ILL_CMD;

		if((strcmp(argv[i],"-loops") == 0) || (strcmp(argv[i],"-l") == 0)){
			if(((i+1) >= argc) || (isxdigit(*argv[i+1])==0)) ILL_CMD;

			loop_cnt_tmp = simple_strtoul(argv[i+1], NULL, 10);
			info->test_loops = -1; /* set to maximal number for unsigned. */
			if (loop_cnt_tmp > info->test_loops) {
				puts("WW: loop count capped.\n");
				loop_cnt_tmp = info->test_loops;
			}
			info->test_loops = (loop_cnt_tmp == 0)? 1: loop_cnt_tmp;
			i = i+2;
		}else if((strcmp(argv[i],"-range") == 0) || (strcmp(argv[i],"-r") == 0)){
			if(((i+2) >= argc) || (isxdigit(*argv[i+1])==0) || (isxdigit(*argv[i+2])== 0)) ILL_CMD;

			u32_t addr_tmp = simple_strtoul(argv[i+1], NULL, 16);
			u32_t size_tmp = simple_strtoul(argv[i+2], NULL, 16);
			if((0 == addr_tmp) || (0 == size_tmp)){
				printf("ERR: Please assign the memory test range: -r <start address> <size>.\n");
				return MT_FAIL;
			}
			if((size_tmp > dram_vsize_b()) || (VA_TO_PA(addr_tmp) >= dram_vsize_b())){
				printf("ERR: Incorrect memory test rnage.\n");
				return MT_FAIL;
			}

			info->area_num=1;
			if(VA_TO_PA(addr_tmp) < TEST_AREA_SIZE1){
				info->start_addr[0]= addr_tmp;
				info->test_size[0] = (VA_TO_PA((addr_tmp+size_tmp)) >= VA_TO_PA(TEST_AREA_BASE1_END))?(VA_TO_PA(TEST_AREA_BASE1_END) - VA_TO_PA(addr_tmp)):size_tmp;
			}else if((VA_TO_PA(addr_tmp) >= VA_TO_PA(TEST_AREA_BASE2)) && (VA_TO_PA(addr_tmp) < VA_TO_PA(TEST_AREA_BASE2_END))){
				info->start_addr[0]= addr_tmp;
				info->test_size[0] = (VA_TO_PA((addr_tmp+size_tmp)) >= dram_vsize_b())?(dram_vsize_b()-VA_TO_PA(addr_tmp)):size_tmp;
			}else{
				printf("ERR: The assigned test range is for stack and u-boot use.\n");
                printf("Allowable: 0x%x~0x%x  & 0x%x~0x%x\n",TEST_AREA_BASE1, (TEST_AREA_BASE1_END-1), TEST_AREA_BASE2, (TEST_AREA_BASE2_END-1));
				return MT_FAIL;
			}
			i = i+3;
		}else if((strcmp (argv[i], "-block_e") == 0) || (strcmp (argv[i], "-b") == 0)){
			g_err_handle_block_mode = 1;
			i = i+1;
		}else if(strcmp(argv[i],"-reset") == 0){
			g_reset_flag = UBOOT_RESET;
			i = i+1;
		}else if(strcmp(argv[i],"-reset_all") == 0){
			g_reset_flag = WHOLE_CHIP_RESET;
			i = i+1;
		}else if(strcmp(argv[i],"-pstr") == 0){
			pstring = argv[i+1];
			i+=2;
		}else if(strcmp(argv[i],"-fstr") == 0){
			fstring = argv[i+1];
			i+=2;
		}else{
			ILL_CMD;
		}
	}

	if(VA_TO_PA(info->start_addr[1]) >= dram_vsize_b()){
		info->area_num = 1;
		puts("<Only one test area!>\n");
	}

	printf("II: # of iteration: %d\n", info->test_loops);

	return MT_SUCCESS;
}


int dram_test (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
	u32_t size_per_pattern=0x10000;
	u32_t i,j;
	int retcode = MT_SUCCESS;
	ddr_cmd_parsing_info_t cmd_info;

	/*back the value of mcr*/
	u32_t ori_mcr = REG32(MCR);

	if(MT_FAIL == ddr_cmd_parsing(argc, argv, &cmd_info))
		goto test_fail;

	for(j=0; j<cmd_info.test_loops; j++){
		for(i=0; i<cmd_info.area_num; i++){
			MEMCTL_DEBUG_PRINTF("<Range %d: 0x%x~0x%x>\n",i+1, cmd_info.start_addr[i], cmd_info.start_addr[i]+cmd_info.test_size[i]-1);
			_cache_flush();

			/*1. Uncached range */
			MEMCTL_DEBUG_PRINTF("Uncached with no prefetch, DRAM Test start = 0x%x\n",UADDR(cmd_info.start_addr[i]));
			disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );
			if(MT_FAIL == _dram_test(UADDR(cmd_info.start_addr[i]), size_per_pattern, cmd_info.test_size[i]))
				goto test_fail;

			/*2. Cached range without prefetch */
			disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );
			MEMCTL_DEBUG_PRINTF("Cached with no prefetch, DRAM Test start = 0x%x\n",CADDR(cmd_info.start_addr[i]));
			if(MT_FAIL == _dram_test(CADDR(cmd_info.start_addr[i]), size_per_pattern, cmd_info.test_size[i]))
				goto test_fail;

			/*3. Cached range with data prefetch mechanism */
			MEMCTL_DEBUG_PRINTF("Cached with data prefetch, DRAM Test start = 0x%x\n",CADDR(cmd_info.start_addr[i]));
			disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );
			enable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE);
			if(MT_FAIL == _dram_test(CADDR(cmd_info.start_addr[i]), size_per_pattern, cmd_info.test_size[i]))
				goto test_fail;

			/*4. Cached range with instruction prefetch mechanism */
			MEMCTL_DEBUG_PRINTF("Cached with ins. prefetch, DRAM Test start = 0x%x\n",CADDR(cmd_info.start_addr[i]));
			disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );
			enable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE);
			if(MT_FAIL == _dram_test(CADDR(cmd_info.start_addr[i]), size_per_pattern, cmd_info.test_size[i]))
				goto test_fail;

			/*5. Cached range with instruction/data prefetch mechanism */
			MEMCTL_DEBUG_PRINTF("Cached with data/ins. prefetch, DRAM Test start = 0x%x\n",CADDR(cmd_info.start_addr[i]));
			disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );
			enable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE);
			if(MT_FAIL == _dram_test(CADDR(cmd_info.start_addr[i]),size_per_pattern,cmd_info.test_size[i]))
				goto test_fail;

			disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE );
		}

		MEMCTL_DEBUG_PRINTF("<mdram_test %d runs>                  \n\n",j+1);
	}
	goto restore_setting;

 test_fail:
	retcode = MT_FAIL;
 restore_setting:
	/*Recover the setting of MCR & the error blocking manner */
	REG32(MCR) = ori_mcr;

	if(MT_SUCCESS== retcode){
		if (pstring) {
			printf("%s\n", pstring);
		}

		/* Reset if the command is sent from the command line */
		if(UBOOT_RESET == g_reset_flag){
			do_reset (NULL, 0, 0, NULL);
		}else if(WHOLE_CHIP_RESET == g_reset_flag){
			_machine_restart();
		}
	} else {
		if (fstring) {
			printf("\n%s\n", fstring);
		}
	}
	return retcode;
}


U_BOOT_CMD(
    mdram_test,   9,  1,  dram_test,
    "RTK DRAM Memory Test",
    "[-l/-loops <test loops>] [-r/-range <star address> <test size>] [-b/-block_e] [-reset <all>] [-pstr <pass msg>] [-fstr <fail msg>]\n"
    "  -r: Test the assigned range\n"
    "  -l: Totol test loops\n"
    "  -c: Assign the specific test case\n"
);

