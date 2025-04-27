#include <soc.h>
#include <register_map.h>
#include <util.h>

#ifndef MDRAM_TEST_SECTION
#define MDRAM_TEST_SECTION __attribute__ ((section (".nor_only_area")))
#endif

/* Definitions for memory test error handing manner */
#define MT_SUCCESS (0)
#define MT_FAIL    (-1)

//The default seting of memory test error handling manner is non-blocking
//Using "Environment parameter" or "command flag setting" can change this
//"Environment parameter": setenv mt_freeze_block debug
//"command flag setting": mdram_test/mflash_test -b/-mt_block
static u32_t g_err_handle_block_mode = 1;

#define HANDLE_FAIL()	  \
	({ \
		printf("%s (%d) test failed.\n", __FUNCTION__, __LINE__); \
		if (!g_err_handle_block_mode) { \
			return; \
		}else{ \
			while(1); \
		} \
	})

#define _cache_flush()	_soc.bios.dcache_writeback_invalidate_all()

/*
 * DRAM TEST RANGES:
 * TEST_AREA1: From 0 ~ (UBOOT_BASE-SIZE_3MB)
 * TEST_AREA2: From (UBOOT_BASE+SIZE_3MB) ~ DRAM_END
 */
#define VA_TO_PA(VAddr) (VAddr&0x1FFFFFFF)	//Physical address
#define TEST_AREA_BASE1 (unsigned int)(0x80000000)
#define TEST_AREA_SIZE1	(get_dram_size())
#define CFG_DCACHE_SIZE (0x10)

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
	u32_t start_addr;
	u32_t test_size;
} ddr_cmd_parsing_info_t;

/*
 * Function Declaration
 */
void (*f)(void) = (void *) 0xbfc00000;

MDRAM_TEST_SECTION
static u32_t get_dram_size(void) {
	const unsigned char BNKCNTv[] = {1, 2, 3};
	const unsigned char BUSWIDv[] = {0, 1, 2};
	const unsigned char ROWCNTv[] = {11, 12, 13, 14, 15, 16};
	const unsigned char COLCNTv[] = {8, 9, 10, 11, 12};

	return 1 << (BNKCNTv[RFLD_DCR(bankcnt) & 0x3] +
	             BUSWIDv[RFLD_DCR(dbuswid) & 0x3] +
	             ROWCNTv[RFLD_DCR(rowcnt)  & 0xF] +
	             COLCNTv[RFLD_DCR(colcnt)  & 0xF]);
}

MDRAM_TEST_SECTION
void dram_normal_patterns(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i, j;
	u32_t start_value;
	volatile u32_t *read_start;
	volatile u32_t *start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < (sizeof(dram_patterns)/sizeof(u32_t)); i++) {
		_cache_flush();

		/* write pattern*/
		start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
		read_start = (u32_t *)(UADDR((u32_t)start+dram_size-4));
		for (j=0; j<dram_size; j=j+4) {
			*start = dram_patterns[i];
			start++;
		}

		_cache_flush();

		/* check data */
		for (j=0; j<dram_size; j=j+4) {
			start_value = (*read_start);
			if (start_value != dram_patterns[i]) {
				printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
				       (u32_t)read_start , start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL();
			}
			read_start--;
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
	return;
}

MDRAM_TEST_SECTION
void dram_walking_of_1(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i;
	int j;
	u32_t walk_pattern;
	u32_t start_value;
	volatile u32_t *read_start;
	volatile u32_t *start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i<32; i++) {
		_cache_flush();

		/* generate pattern */
		walk_pattern = (1 << i);

		/* write pattern*/
		start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
		read_start = (u32_t *)(UADDR((u32_t)start+dram_size-4));
		for(j=0; j < dram_size; j=j+4) {
			*start = walk_pattern;
			start++;
		}

		_cache_flush();

		/* check data */
		for (j=0; j<dram_size; j=j+4) {
			start_value = (*read_start);
			if (start_value != walk_pattern) {
				printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				       (u32_t)read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL();
			}
			read_start--;
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
	return;
}

MDRAM_TEST_SECTION
void dram_walking_of_0(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i;
	int j;
	u32_t start_value;
	u32_t walk_pattern;
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
			*start = walk_pattern;
			start++;
		}

		_cache_flush();

		/* check data */
		for (j=0; j < dram_size; j=j+4) {
			start_value = (*read_start);
			if(start_value != walk_pattern) {
				printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n", \
				       (u32_t)read_start , start_value, walk_pattern,  __FUNCTION__, __LINE__);
				HANDLE_FAIL();
			}
			read_start--;
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x passed\r", i, walk_pattern, (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
	return;
}

MDRAM_TEST_SECTION
void dram_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i;
	int j;
	u32_t start_value;
	u32_t read_start_addr;
	volatile u32_t *start;
	volatile u32_t *read_start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i=i+4) {
		_cache_flush();
		/* write pattern*/
		start		= (u32_t *)(dram_start + ((i/4)*dram_size)%(area_size));
		read_start 	= (u32_t *)((u32_t)start + dram_size-4);
		read_start_addr = ((u32_t)start);
		start = (u32_t *)(UADDR(((u32_t)start)));
		for(j=0; j < dram_size; j=j+4) {
			*start = (read_start_addr << i);
			start++;
			read_start_addr = read_start_addr + 4 ;
		}

		_cache_flush();
		read_start_addr = ((u32_t)read_start);
		/* check data reversing order */
		for(j=0; j < dram_size; j=j+4) {
			start_value = (*read_start);
			if(start_value != ((read_start_addr) << i)) {
				printf("\ndecr addr(0x%x): 0x%x != pattern(0x%x) %s, %d i=%d j=0x%x\n", \
				       (u32_t)read_start , start_value, ((read_start_addr) << i), \
				       __FUNCTION__, __LINE__, i,j);
				HANDLE_FAIL();
			}
			read_start_addr = read_start_addr - 4;
			read_start--;
		}

		read_start_addr += 4;
		read_start++;

		/* check data sequential order */
		for (j=0; j < dram_size; j=j+4) {
			start_value = (*read_start);
			if (start_value != ((read_start_addr) << i)) {
				printf("\nseq addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n", \
				       (u32_t)read_start , start_value, ((read_start_addr) << i), \
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL();
			}
			read_start_addr = read_start_addr + 4;
			read_start++;
		}
		MEMCTL_DEBUG_PRINTF("rotate %d 0x%x passed\r", i, (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
	return;
}

MDRAM_TEST_SECTION
void dram_com_addr_rot(u32_t dram_start, u32_t dram_size, u32_t area_size)
{
	int i;
	int j;
	u32_t start_value;
	u32_t read_start_addr;
	volatile u32_t *start;
	volatile u32_t *read_start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i=i+4) {
		_cache_flush();
		/* write pattern*/
		start		= (u32_t *)(dram_start + ((i/4)*dram_size)%(area_size));
		read_start 	= (u32_t *)((u32_t)start + dram_size-4);
		read_start_addr = ((u32_t)start);
		start = (u32_t *)(UADDR(((u32_t)start)));
		for(j=0; j < dram_size; j=j+4) {
			*start = ~(read_start_addr << i);
			start++;
			read_start_addr = read_start_addr + 4 ;
		}

		_cache_flush();
		read_start_addr = ((u32_t)read_start);
		/* check data reversing order */
		for(j=0; j<dram_size; j=j+4) {
			start_value = (*read_start);
			if (start_value != (~(read_start_addr << i))) {
				printf("\ndecr addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				       (u32_t)read_start , start_value, ~((read_start_addr) << i), \
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL();
			}
			read_start_addr = read_start_addr - 4;
			read_start--;
		}

		read_start_addr += 4;
		read_start++;

		/* check data sequential order */
		for(j=0; j<dram_size; j=j+4) {
			start_value = (*read_start);
			if (start_value != (~(read_start_addr << i))) {
				printf("\nseq addr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				       (u32_t)read_start , start_value, ~((read_start_addr) << i), \
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL();
			}
			read_start_addr = read_start_addr + 4;
			read_start++;
		}
		MEMCTL_DEBUG_PRINTF("~rotate %d 0x%x passed\r", i, (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
	return;
}

MDRAM_TEST_SECTION
void dram_half_word_access(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i;
	int j;
	u16_t h_word;
	u32_t start_value;
	volatile u16_t *start_h;
	volatile u32_t *start_w;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < (sizeof(dram_patterns)/sizeof(u32_t)); i++) {
		_cache_flush();

		/* write half-word pattern*/
		start_h = (u16_t *)(dram_start+ (i*dram_size)%(area_size));
		start_w = (u32_t *)(UADDR(dram_start+ (i*dram_size)%(area_size)+dram_size-4));
		for (j=0; j < dram_size; j=j+4) {
			h_word = (u16_t)(dram_patterns[i]);
			*(start_h+1) = h_word;
			h_word = (u16_t)(dram_patterns[i] >> 16);
			*start_h = h_word;
			start_h+=2;
		}

		_cache_flush();

		/* read word and check data */
		for(j=0; j < dram_size; j=j+4) {
			start_value = (*start_w);
			if(start_value != dram_patterns[i]) {
				printf("\naddr:0x%x(0x%x) != pattern(0x%x) %s, %d\n", \
				       (u32_t)start_w, start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL();
			}
			start_w--;
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], (u32_t)start_h);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
	return;
}

MDRAM_TEST_SECTION
void dram_byte_access(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i;
	int j;
	u8_t byte;
	u32_t start_value;
	volatile u8_t *start_b;
	volatile u32_t *start_w;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < (sizeof(dram_patterns)/sizeof(u32_t)); i++) {
		_cache_flush();

		/* write byte pattern*/
		start_w = (u32_t *)(UADDR(dram_start+(i*dram_size)%(area_size)+dram_size-4));
		start_b = (u8_t *)(dram_start+(i*dram_size)%(area_size));
		for (j=0; j<dram_size; j=j+4) {
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
			start_value = *start_w;
			if (start_value != dram_patterns[i]) {
				printf("\naddr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",\
				       (u32_t)start_w, start_value, dram_patterns[i],  __FUNCTION__, __LINE__);
				HANDLE_FAIL();
			}
			start_w--;
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i], (u32_t)start_b);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
	return;
}

MDRAM_TEST_SECTION
void memcpy_test(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	int i;
	int j;
	u32_t start_value;
	u32_t read_start_addr;
	volatile u32_t *start;
	volatile u32_t *read_start;

	MEMCTL_DEBUG_PRINTF("=======start %s test=======\r", __FUNCTION__);
	for (i=0; i < 32; i=i+4) {
		_cache_flush();

		/* write pattern*/
		start = (u32_t *)(dram_start + (i*dram_size)%(area_size));
		read_start_addr = ((u32_t)start + (dram_size-4));
		read_start = (u32_t *)(UADDR(((u32_t)start)+dram_size-4));
		for(j=0; j < dram_size; j=j+4) {
			*start = ((u32_t)start << i);
			start++;
		}

		inline_memcpy((char *)(dram_start+dram_size), (char *)(dram_start + (i*dram_size)%(area_size)), dram_size);

		_cache_flush();

		/* check uncached data */
		read_start = (u32_t *)(dram_start+dram_size+dram_size-4);
		for (j=0; j<dram_size; j=j+4) {
			start_value = (*read_start);
			if (start_value != ((read_start_addr) << i)) {
				printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",\
				       (u32_t)read_start , start_value, ((read_start_addr) << i), \
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL();
			}
			read_start = read_start - 1;
			read_start_addr = read_start_addr - 4;
		}
		MEMCTL_DEBUG_PRINTF("memcpy %d 0x%x passed\r", i, (u32_t)start);
	}
	MEMCTL_DEBUG_PRINTF("%s test completed.\r", __FUNCTION__);
	return;
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
MDRAM_TEST_SECTION
void unaligned_test(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	unsigned int i, j;
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
			off1[i].c1 = 0xcc;
			off1[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_1));i++){
			if((off1[i].w != dram_patterns[j]) || (off1[i].c1 != 0xcc)){
				printf("\noffset 1 error:addr(0x%x) write 0x%x, read 0x%x, c1(%02x)\n", \
				       (u32_t)&off1[i], dram_patterns[j], off1[i].w, off1[i].c1);
				HANDLE_FAIL();
			}
		}


		/* offset 2 bytes */
		off2 = (t_off_2 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_2));i++){
			off2[i].c1 = 0xcc;
			off2[i].c2 = 0xdd;
			off2[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_2));i++){
			if(off2[i].w != dram_patterns[j] || (off2[i].c1 != 0xcc) || (off2[i].c2 != 0xdd)){
				printf("\noffset 2 error:addr(0x%x) write 0x%x, read 0x%x, c1(0x%x), c2(0x%x)\n", \
				       (u32_t)&off2[i], dram_patterns[j], off2[i].w, off2[i].c1, off2[i].c2);
				printf("&dram_pattern[%d](0x%p) = 0x%x\r", j, &dram_patterns[j], dram_patterns[j]);
				HANDLE_FAIL();
			}
		}

		/* offset 3 bytes */
		off3 = (t_off_3 *)(dram_start + (j*dram_size)%(area_size));
		/* set value */
		for(i=0;i<(dram_size/sizeof(t_off_3));i++){
			off3[i].c1 = 0xcc;
			off3[i].c2 = 0xdd;
			off3[i].c3 = 0xee;
			off3[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for(i=0;i<(dram_size/sizeof(t_off_3));i++){
			if(off3[i].w != dram_patterns[j] ||(off3[i].c1 != 0xcc) || (off3[i].c2 != 0xdd) || (off3[i].c3 != 0xee)){
				printf("\noffset 3 error:addr(0x%x) write 0x%x, "\
				       "read 0x%x, c1(%02x), c2(%02x), c3(%02x)\n", \
				       (u32_t)&off1[i], dram_patterns[j], off3[i].w, off3[i].c1, \
				       off3[i].c2, off3[i].c3);
				HANDLE_FAIL();
			}
		}
		MEMCTL_DEBUG_PRINTF("pattern[%d](0x%x) 0x%x pass\r", j, dram_patterns[j], (u32_t)off3);

	}
	MEMCTL_DEBUG_PRINTF("%s test passed.\r", __FUNCTION__);
	return;
}

#define MCR_IPREF 0x00010001
#define MCR_DPREF 0x00010002
MDRAM_TEST_SECTION
void disable_DRAM_prefetch(void) {
	RMOD_MCR(ip_ref, 0,
	         dp_ref, 0);
	return;
}

MDRAM_TEST_SECTION
void enable_DRAM_prefech(unsigned int side_id) {
	if (side_id & MCR_IPREF) {
		RMOD_MCR(ip_ref, 0);
		RMOD_MCR(ip_ref, 1);
	}

	if (side_id & MCR_DPREF) {
		RMOD_MCR(dp_ref, 0);
		RMOD_MCR(dp_ref, 1);
	}
	return;
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
MDRAM_TEST_SECTION
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

MDRAM_TEST_SECTION
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

MDRAM_TEST_SECTION
int cache_flush_adj_addr(unsigned int addr_base, unsigned int run_times, \
                         unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	int retcode;

	retcode = MT_SUCCESS;

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
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , ((unsigned int)pdata));
							retcode = MT_FAIL;
						}
					pdata++;
				}
		}
	return (retcode);
}

MDRAM_TEST_SECTION
int cache_flush_adjacent(unsigned int addr_base, unsigned int run_times, \
                         unsigned int random, unsigned int byteset)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value, a, b, c;
	int retcode;
	unsigned char vbyte;

	retcode = MT_SUCCESS;
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
				inline_memset((void *)pdata, vbyte, CFG_DCACHE_SIZE);
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
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata++;
				}
		}
	return (retcode);
}

MDRAM_TEST_SECTION
int cache_flush_dispersed (unsigned int addr_base, unsigned int run_times,\
                           unsigned int random, unsigned int byteset)
{
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
				inline_memset((void *)pdata, vbyte, CFG_DCACHE_SIZE);
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
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					data = *pdata++;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
				}
		}
	return (retcode);
}

MDRAM_TEST_SECTION
int cache_flush_adjacent_toggle_word(unsigned int addr_base, unsigned int run_times)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode;

	retcode = MT_SUCCESS;

	for(test_times = 0; test_times < run_times; test_times++)
		{
			write_value = toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];

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
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata++;
				}
		}
	return (retcode);
}

MDRAM_TEST_SECTION
int cache_flush_dispersed_toggle_word (unsigned int addr_base, unsigned int run_times)
{
	unsigned int i;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value;
	int retcode;

	retcode = MT_SUCCESS;

	for(test_times = 0; test_times < run_times; test_times++)
		{
			write_value = toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
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
			/* varify the data */
			for(i=0; i<CFG_DCACHE_SIZE; i = i+4)
				{
					i = i + 28;
					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata++;

					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata++;

					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata++;

					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata++;


					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata++;

					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata++;

					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata++;


					data = *pdata;
					if(data != write_value)
						{
							printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value);
							retcode = MT_FAIL;
						}
					pdata++;
					pdata = (unsigned int *)((unsigned int)pdata+(unsigned int)0x1000);
				}

		}
	return (retcode);
}

MDRAM_TEST_SECTION
void cache_flush_adjacent_toggle_line128(unsigned int addr_base, unsigned int run_times) {
	unsigned int i, j;
	volatile unsigned int data;
	volatile unsigned int test_times;
	volatile unsigned int *pdata;
	unsigned int write_value[4];

	for (test_times=0; test_times<run_times; test_times++) {
		write_value[0] = line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		write_value[1] = line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		write_value[2] = ~line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		write_value[3] = ~line_toggle_pattern[(test_times%(sizeof(toggle_pattern)/sizeof(unsigned int)))];
		MEMCTL_DEBUG_PRINTF("\raddr_base: 0x%08x times: %d, pattern: 0x%08x ", addr_base, test_times, write_value[0]);
		pdata = (unsigned int *)(UADDR(addr_base));
		/* Initial DCache */
		for (i=0; i<CFG_DCACHE_SIZE; i = i+4) {
			*pdata = BACKGROUND_VALUE;
			pdata++;
		}

		pdata = (unsigned int *)(CADDR(addr_base));
		/* Read data into DCache */
		for(i=0; i<CFG_DCACHE_SIZE; i = i+4) {
			data = *pdata;
			pdata += 4;
		}

		/* Dirtify DCache */
		pdata = (unsigned int *)(CADDR(addr_base));
		for(i=0; i<CFG_DCACHE_SIZE; i = i+16) {
			*pdata++ = write_value[0];
			*pdata++ = write_value[1];
			*pdata++ = write_value[2];
			*pdata++ = write_value[3];
		}

		/* write back and invalidate DCache */
		_cache_flush();

		pdata = (unsigned int *)(CADDR(addr_base));
		/* varify the data */
		for (i=0; i<CFG_DCACHE_SIZE; i=i+16) {
			for (j=0; j<4; j++) {
				data = *pdata;
				if (data != write_value[j]) {
					printf("\npdata(0x%08x) 0x%08x != 0x%08x\n", (u32_t)pdata, data , write_value[j]);
					HANDLE_FAIL();
				}
				pdata++;
			}
		}
	}
	return;
}

MDRAM_TEST_SECTION
void cache_flush_test(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	unsigned int addr_base;
	unsigned int test_times;
	test_times = TEST_TIMES;

	MEMCTL_DEBUG_PRINTF("test area size = 0x%08x\n", area_size);

	for(addr_base = dram_start;
	    addr_base < (dram_start + area_size);
	    addr_base = addr_base + dram_size) {
		cache_flush_adjacent(addr_base, test_times, 0, 0);
		cache_flush_adjacent(addr_base, test_times, 1, 0);
		cache_flush_dispersed(addr_base, test_times, 0, 0);
		cache_flush_dispersed(addr_base, test_times, 1, 0);

		cache_flush_adjacent(addr_base, test_times, 0, 1);
		cache_flush_adjacent(addr_base, test_times, 1, 1);
		cache_flush_dispersed(addr_base, test_times, 0, 1);
		cache_flush_dispersed(addr_base, test_times, 1, 1);

		cache_flush_adj_addr(addr_base, test_times, 1, 1);
		cache_flush_adjacent_toggle_word(addr_base, test_times);
		cache_flush_dispersed_toggle_word(addr_base, test_times);
		cache_flush_adjacent_toggle_line128(addr_base, test_times);
	}

	MEMCTL_DEBUG_PRINTF("\n");
	return;
}

MDRAM_TEST_SECTION
void _dram_test(u32_t dram_start, u32_t dram_size, u32_t area_size) {
	unaligned_test( dram_start, dram_size, area_size);
	dram_addr_rot(dram_start, dram_size, area_size);
	dram_com_addr_rot(dram_start, dram_size, area_size);
	dram_byte_access(dram_start, dram_size, area_size);
	dram_half_word_access(dram_start, dram_size, area_size);
	cache_flush_test( dram_start, dram_size, area_size);
	dram_normal_patterns(dram_start, dram_size, area_size);
	dram_walking_of_1(dram_start, dram_size, area_size);
	dram_walking_of_0(dram_start, dram_size, area_size);
	memcpy_test(dram_start, dram_size, area_size);
}

MDRAM_TEST_SECTION
void dram_test(void) {
	const u32_t size_per_pattern = 0x10000;
	ddr_cmd_parsing_info_t cmd_info;

	/*back the value of mcr*/
	u32_t ori_mcr = MCRrv;

	cmd_info.start_addr = TEST_AREA_BASE1;
	cmd_info.test_size  = TEST_AREA_SIZE1;

	MEMCTL_DEBUG_PRINTF("<Range %08x ~ %08x>\n",
	                    cmd_info.start_addr,
	                    cmd_info.start_addr + cmd_info.test_size - 1);
	_cache_flush();

	/*1. Uncached range */
	disable_DRAM_prefetch();
	MEMCTL_DEBUG_PRINTF("No prefetch, Uncached address, MCR = %08x\n", MCRrv);
	_dram_test(UADDR(cmd_info.start_addr), size_per_pattern, cmd_info.test_size);

	/*2. Cached range without prefetch */
	MEMCTL_DEBUG_PRINTF("\nNo prefetch, Cached address, MCR = %08x\n", MCRrv);
	_dram_test(CADDR(cmd_info.start_addr), size_per_pattern, cmd_info.test_size);

	/*3. Cached range with data prefetch mechanism */
	disable_DRAM_prefetch();
	enable_DRAM_prefech(MCR_DPREF);
	MEMCTL_DEBUG_PRINTF("\nDPREFETCH, Cached address, MCR = %08x\n", MCRrv);
	_dram_test(CADDR(cmd_info.start_addr), size_per_pattern, cmd_info.test_size);

	/*4. Cached range with instruction prefetch mechanism */
	disable_DRAM_prefetch();
	enable_DRAM_prefech(MCR_DPREF);
	MEMCTL_DEBUG_PRINTF("\nIPREFETCH, Cached address, MCR = %08x\n", MCRrv);
	_dram_test(CADDR(cmd_info.start_addr), size_per_pattern, cmd_info.test_size);

	/*5. Cached range with instruction/data prefetch mechanism */
	disable_DRAM_prefetch();
	enable_DRAM_prefech(MCR_DPREF | MCR_IPREF);
	MEMCTL_DEBUG_PRINTF("\nDPREFETCH/IPREFETCH, MCR = %08x\n", MCRrv);
	_dram_test(CADDR(cmd_info.start_addr),size_per_pattern,cmd_info.test_size);

	/* Recover the setting of MCR & the error blocking manner */
	*((volatile u32_t *)0xb8001000) = ori_mcr;

	MEMCTL_DEBUG_PRINTF("\n<Completed>\n");
	return;
}
