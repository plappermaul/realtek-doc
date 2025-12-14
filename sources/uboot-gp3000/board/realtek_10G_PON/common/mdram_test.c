/*
 * Include Files
 */
#include <common.h>
#include <linux/ctype.h>
#include <asm/io.h>

/*
 * DRAM TEST RANGES:
 * TEST_AREA1: From 0 ~ (UBOOT_BASE-SIZE_3MB)
 * TEST_AREA2: From (UBOOT_BASE+SIZE_3MB) ~ DRAM_END
 */
#define DRAM_SIZE  (readl(0x111100c) * 0x100000)
#define SIZE_3MB   (0x300000)
#define SIZE_16MB  (0x1000000)
#define SIZE_256MB (0x10000000)

#define SECURE_DRAM_START (0x0F000000)
#define SECURE_DRAM_END   (SECURE_DRAM_START + SIZE_16MB)

u32 g_bit_width;
u32 g_dram_type;
u32 g_ddr_size;
u32 g_ddr_freq;

/**************************************************
  * Offset 64MB is u-boot start
  * Offset 240MB ~ 256MB is reserved for Secure DRAM region
  * Bottom 26MB is reserved for u-boot relocation use
  **************************************************
  * AREA0 : 0x0000_0000 ~ (0x03D0_0000-1)
  * AREA1A: 0x0430_0000 ~ (0x0D60_0000-1)
  * AREA1B: 0x0430_0000 ~ (0x0F00_0000-1)
  * AREA2 : 0x1000_0000 ~
  **************************************************/
#define TEST_AREA0     (unsigned int)(0x00000000)
#define TEST_AREA0_END (unsigned int)(CONFIG_SYS_TEXT_BASE - SIZE_3MB - 1)
#define TEST_SIZE0     (unsigned int)(TEST_AREA0_END - TEST_AREA0 + 1)

#define HOLE0          (unsigned int)(TEST_AREA0 + TEST_SIZE0)
#define HOLE0_END      (unsigned int)(HOLE0 + SIZE_3MB*2 - 1)

#define TEST_AREA1A     (unsigned int)(CONFIG_SYS_TEXT_BASE + SIZE_3MB)
#define TEST_AREA1A_END (unsigned int)(SECURE_DRAM_START - (10*1024*1024) - 1)
#define TEST_SIZE1A     (unsigned int)(TEST_AREA1A_END - TEST_AREA1A  + 1)

#define HOLE1A          (unsigned int)(TEST_AREA1A + TEST_SIZE1A)
#define HOLE1A_END      (unsigned int)(SECURE_DRAM_END - 1)

#define TEST_AREA1B     (unsigned int)(CONFIG_SYS_TEXT_BASE + SIZE_3MB)
#define TEST_AREA1B_END (unsigned int)(SECURE_DRAM_START - 1)
#define TEST_SIZE1B     (unsigned int)(TEST_AREA1B_END - TEST_AREA1B  + 1)

#define HOLE1B          (unsigned int)(TEST_AREA1B + TEST_SIZE1B)
#define HOLE1B_END      (unsigned int)(SECURE_DRAM_END - 1)


#define TEST_AREA2     (unsigned int)(SECURE_DRAM_END)
#define TEST_AREA2_END (unsigned int)(DRAM_SIZE - (28*1024*1024) - 1)
#define TEST_SIZE2     (unsigned int)(TEST_AREA2_END - TEST_AREA2  + 1)

/* Definitions for debug & error handing manner */
#define MT_SUCCESS (0)
#define MT_FAIL    (-1)
#define MT_ILL_CMD (-2)
#define HANDLE_FAIL ({ return MT_FAIL; })

extern void flush_dcache_all(void);
#define _cache_flush() flush_dcache_all()

const unsigned int dram_patterns[] = {
  0x00000000, 0xffffffff, 0x55555555, 0xaaaaaaaa, 0x01234567,
  0x76543210, 0x89abcdef, 0xfedcba98, 0xA5A5A5A5, 0x5A5A5A5A,
  0xF0F0F0F0, 0x0F0F0F0F, 0xFF00FF00, 0x00FF00FF, 0x0000FFFF,
  0xFFFF0000, 0x00FFFF00, 0xFF0000FF, 0x5A5AA5A5, 0xA5A55A5A,
};

/**************************
 * Command Parsing
 *************************/
typedef struct {
	u32 start_addr[3];
	u32 test_size[3];
	u8 area_num;
	u8 test_case;
	u16 test_loops;
} ddr_cmd_parsing_info_t;

/*
 * Function Declaration
 */
int dram_normal_patterns(u32 dram_start, u32 size_per_pat, u32 area_size) {
	u32 i, j;
	u32 read_value;
	volatile u32 *r_addr;
	volatile u32 *w_addr;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i = 0; i < (sizeof(dram_patterns) / sizeof(u32)); i++) {
		w_addr = (u32 *)(uintptr_t)(dram_start + (i * size_per_pat) % (area_size));
		r_addr = (u32 *)((uintptr_t)w_addr + size_per_pat - 4);

		/* Write pattern */
		_cache_flush();
		for (j = 0; j < size_per_pat; j = j + 4) {
			*w_addr = dram_patterns[i];
			w_addr++;
		}

		/* Check data */
		_cache_flush();
		for (j = 0; j < size_per_pat; j = j + 4) {

			read_value = (*r_addr);
			if (read_value != dram_patterns[i]) {
				printf("\naddr(0x%08x):0x%08x != pattern(0x%08x) %s, %d\n", (u32)(uintptr_t)r_addr, read_value, dram_patterns[i], __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			r_addr--;
		}
		printf("pattern[%d](0x%08x) 0x%08x pass\r", i, dram_patterns[i], (u32)(uintptr_t)w_addr);
	}
	printf("%s completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

/*
 * write two half-words and read word.
 */
int dram_half_word_access(u32 dram_start, u32 size_per_pat, u32 area_size) {
	u32 i, j;
	u16 h_word;
	u32 read_value, test_start;
	volatile u16 *waddr_h;
	volatile u32 *raddr_w;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i = 0; i < (sizeof(dram_patterns) / sizeof(u32)); i++) {

		waddr_h = (u16 *)(uintptr_t)(dram_start + (i * size_per_pat) % (area_size));
		raddr_w = (u32 *)(uintptr_t)(dram_start + (i * size_per_pat) % (area_size) + size_per_pat - 4);
		test_start = (u32)(uintptr_t)waddr_h;

		/* Write half-word pattern*/
		_cache_flush();
		for (j = 0; j < size_per_pat; j = j + 4) {

			h_word = (u16)(dram_patterns[i]);
			*waddr_h = h_word;
			h_word = (u16)(dram_patterns[i] >> 16);
			*(waddr_h + 1) = h_word;
			waddr_h += 2;
		}

		/* Read word and check data */
		_cache_flush();
		for (j = 0; j < size_per_pat; j = j + 4) {
			if ((u32)(uintptr_t)raddr_w < test_start) {
				break;
			}
			read_value = (*raddr_w);
			if (read_value != dram_patterns[i]) {
				printf("\naddr(0x%08x):0x%08x != pattern(0x%08x) %s, %d\n", (u32)(uintptr_t)raddr_w, read_value, dram_patterns[i], __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			raddr_w--;
		}
		printf("pattern[%d](0x%08x) 0x%08x pass\r", i, dram_patterns[i], (u32)(uintptr_t)waddr_h);
	}

	printf("%s completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

int dram_byte_access(u32 dram_start, u32 size_per_pat, u32 area_size) {
	int i, j;
	u8 byte;
	u32 read_value, test_start;
	volatile u8 *waddr_b;
	volatile u32 *raddr_w;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i = 0; i < (sizeof(dram_patterns) / sizeof(u32)); i++) {
		raddr_w = (u32 *)(uintptr_t)(dram_start + (i*size_per_pat)%(area_size) + (size_per_pat-4));
		waddr_b =  (u8 *)(uintptr_t)(dram_start + (i*size_per_pat)%(area_size));
		test_start = (u32)(uintptr_t)waddr_b;

		/* write byte pattern*/
		_cache_flush();
		for (j = 0; j < size_per_pat; j = j + 4) {
			byte = (u8)(dram_patterns[i]);
			*(waddr_b + 0) = byte;
			byte = (u8)(dram_patterns[i] >> 8);
			*(waddr_b + 1) = byte;
			byte = (u8)(dram_patterns[i] >> 16);
			*(waddr_b + 2) = byte;
			byte = (u8)(dram_patterns[i] >> 24);
			*(waddr_b + 3) = byte;
			waddr_b += 4;
		}

		/* read word and check data */
		_cache_flush();
		for (j = 0; j < size_per_pat; j = j + 4) {
			if ((u32)(uintptr_t)raddr_w < test_start) {
				break;
			}
			read_value = *raddr_w;
			if (read_value != dram_patterns[i]) {
				printf("\naddr(0x%08x):0x%08x != pattern(0x%08x) %s, %d\n", (u32)(uintptr_t)raddr_w, read_value, dram_patterns[i], __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			raddr_w--;
		}
		printf("pattern[%d](0x%08x) 0x%08x pass\r", i, dram_patterns[i], (u32)(uintptr_t)waddr_b);
	}

	printf("%s completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

#pragma pack(1)
/* Data structures used for testing unaligned load/store operations. */
typedef struct {
	unsigned char c1;
	unsigned int w;
} t_off_1;

typedef struct {
	unsigned char c1;
	unsigned char c2;
	unsigned int w;
} t_off_2;
typedef struct {
	unsigned char c1;
	unsigned char c2;
	unsigned char c3;
	unsigned int w;
} t_off_3;

#pragma pack(4)
int unaligned_test(u32 dram_start, u32 size_per_pat, u32 area_size) {
	unsigned int i, j;
	volatile t_off_1 *off1;
	volatile t_off_2 *off2;
	volatile t_off_3 *off3;

	off1 = (t_off_1 *)(uintptr_t)dram_start;
	off2 = (t_off_2 *)(uintptr_t)dram_start;
	off3 = (t_off_3 *)(uintptr_t)dram_start;

	/* patterns loop */
	for (j = 0; j < sizeof(dram_patterns) / sizeof(unsigned int); j++) {
		/* offset 1 bytes */
		off1 = (t_off_1 *)(uintptr_t)(dram_start + (j * size_per_pat) % (area_size));

		/* Write value */
		for (i = 0; i < (size_per_pat / sizeof(t_off_1)); i++) {
			off1[i].c1 = 0xcc;
			off1[i].w = dram_patterns[j];
		}

		/* check data */
		_cache_flush();
		for (i = 0; i < (size_per_pat / sizeof(t_off_1)); i++) {
			if ((off1[i].w != dram_patterns[j]) || (off1[i].c1 != 0xcc)) {
				printf("\noffset 1 error:addr(0x%08x) write 0x%08x, read 0x%08x, c1(%02x)\n", (u32)(uintptr_t)&off1[i], dram_patterns[j], off1[i].w, off1[i].c1);
				HANDLE_FAIL;
			}
		}

		/* offset 2 bytes */
		off2 = (t_off_2 *)(uintptr_t)(dram_start + (j * size_per_pat) % (area_size));

		/* Write value */
		for (i = 0; i < (size_per_pat / sizeof(t_off_2)); i++) {
			off2[i].c1 = 0xcc;
			off2[i].c2 = 0xdd;
			off2[i].w = dram_patterns[j];
		}

		/* check data */
		_cache_flush();
		for (i = 0; i < (size_per_pat / sizeof(t_off_2)); i++) {
			if (off2[i].w != dram_patterns[j] || (off2[i].c1 != 0xcc) ||
			    (off2[i].c2 != 0xdd)) {
				printf(
				  "\noffset 2 error:addr(0x%08x) write 0x%08x, read 0x%08x, c1(0x%08x), "
				  "c2(0x%08x)\n",
				  (u32)(uintptr_t)&off2[i], dram_patterns[j], off2[i].w, off2[i].c1,
				  off2[i].c2);
				printf("&dram_pattern[%d](0x%p) = 0x%08x\r", j, &dram_patterns[j], dram_patterns[j]);
				HANDLE_FAIL;
			}
		}

		/* offset 3 bytes */
		off3 = (t_off_3 *)(uintptr_t)(dram_start + (j * size_per_pat) % (area_size));

		/* Write value */
		for (i = 0; i < (size_per_pat / sizeof(t_off_3)); i++) {
			off3[i].c1 = 0xcc;
			off3[i].c2 = 0xdd;
			off3[i].c3 = 0xee;
			off3[i].w = dram_patterns[j];
		}

		/* check data */
		_cache_flush();
		for (i = 0; i < (size_per_pat / sizeof(t_off_3)); i++) {
			if (off3[i].w != dram_patterns[j] || (off3[i].c1 != 0xcc) ||
			    (off3[i].c2 != 0xdd) || (off3[i].c3 != 0xee)) {
				printf(
				  "\noffset 3 error:addr(0x%08x) write 0x%08x, "
				  "read 0x%08x, c1(%02x), c2(%02x), c3(%02x)\n",
				  (u32)(uintptr_t)&off3[i], dram_patterns[j], off3[i].w, off3[i].c1, off3[i].c2, off3[i].c3);
				HANDLE_FAIL;
			}
		}
		printf("pattern[%d](0x%08x) 0x%08x pass\r", j, dram_patterns[j], (u32)(uintptr_t)off3);
	}

	printf("%s test passed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

int _dram_test(u32 dram_start, u32 area_size)
{
    u32 i;
    u32 size_per_pat[2];
    u32 test_size[2];
    u32 start_addr[2];
    u32 total_dram_patterns = sizeof(dram_patterns) / sizeof(unsigned int);

    size_per_pat[0] = (area_size/total_dram_patterns)&0xFFFFFFF0;
    test_size[0] =  size_per_pat[0] * total_dram_patterns;
    test_size[1] = area_size - test_size[0];
    size_per_pat[1] = test_size[1];
    start_addr[0] = dram_start;
    start_addr[1] = start_addr[0] + test_size[0];

    for(i=0 ; i<2 ; i++) {
    	if (MT_FAIL == dram_normal_patterns(start_addr[i], size_per_pat[i], test_size[i]))
    		return MT_FAIL;

    	if (MT_FAIL == dram_half_word_access(start_addr[i], size_per_pat[i], test_size[i]))
    		return MT_FAIL;

    	if (MT_FAIL == dram_byte_access(start_addr[i], size_per_pat[i], test_size[i]))
    		return MT_FAIL;

        if (MT_FAIL == unaligned_test(start_addr[i], size_per_pat[i], test_size[i]))
            return MT_FAIL;
    }

	return MT_SUCCESS;
}

int ddr_cmd_parsing(int argc, char *const argv[], ddr_cmd_parsing_info_t *info)
{
	u32 i, loop_cnt_tmp;

#define ILL_CMD \
	({ \
		printf("EE: Illegal command (%d).\n\n\n", __LINE__); \
		return MT_ILL_CMD; \
	})

	/* Initialize the memory test parameters..... */
	info->area_num = 2;
	info->test_loops = 1;
	info->start_addr[0] = TEST_AREA0;
	info->test_size[0] = TEST_SIZE0;

    if (DRAM_SIZE == SIZE_256MB) {
        info->start_addr[1] = TEST_AREA1A;
        info->test_size[1]  = TEST_SIZE1A;
    } else if(DRAM_SIZE > SIZE_256MB){
        info->start_addr[1] = TEST_AREA1B;
        info->test_size[1]  = TEST_SIZE1B;
        info->area_num = 3;
        info->start_addr[2] = TEST_AREA2;
        info->test_size[2]  = TEST_SIZE2;
    } else {
        ILL_CMD;
    }

	/* Parse command flag for test range / test loops / mt error (non-)blocking
	 * mode */
	for (i = 1; i < argc;) {
		if ('-' != *argv[i]) ILL_CMD;

		if ((strcmp(argv[i], "-loops") == 0) || (strcmp(argv[i], "-l") == 0)) {
			if (((i + 1) >= argc) || (isxdigit(*argv[i + 1]) == 0)) ILL_CMD;

			loop_cnt_tmp = simple_strtoul(argv[i + 1], NULL, 10);
			info->test_loops = -1; /* set to maximal number for unsigned. */
			if (loop_cnt_tmp > info->test_loops) {
				puts("WW: loop count capped.\n");
				loop_cnt_tmp = info->test_loops;
			}
			info->test_loops = (loop_cnt_tmp == 0) ? 1 : loop_cnt_tmp;
			i = i + 2;
		} else if ((strcmp(argv[i], "-range") == 0) || (strcmp(argv[i], "-r") == 0)) {
			if (((i + 2) >= argc) || (isxdigit(*argv[i + 1]) == 0) || (isxdigit(*argv[i + 2]) == 0))
				ILL_CMD;

			u32 addr_tmp = simple_strtoul(argv[i + 1], NULL, 16);
			u32 size_tmp = simple_strtoul(argv[i + 2], NULL, 16);
			u32 addr_end = addr_tmp + size_tmp - 1;

			if (0 == size_tmp) {
				printf( "EE: Please assign the correct test range, <size> shouldn't be zero.\n");
				ILL_CMD;
			}
			if ((size_tmp > DRAM_SIZE) || (addr_tmp > DRAM_SIZE)) {
				printf("EE: Incorrect memory test rnage.\n");
				ILL_CMD;
			}

            if (DRAM_SIZE == SIZE_256MB) {
                if (addr_tmp <= TEST_AREA0_END) {
                    info->start_addr[0] = addr_tmp;

                    if (addr_end <= TEST_AREA0_END) {
                        info->area_num = 1;
                        info->test_size[0] = size_tmp;
                    } else if (addr_end <= HOLE0_END) {
                        info->area_num = 1;
                        info->test_size[0] = (TEST_AREA0_END + 1) - addr_tmp;
                    } else if (addr_end <= TEST_AREA1A_END) {
                        info->area_num = 2;
                        info->test_size[0] = (TEST_AREA0_END + 1) - addr_tmp;

                        info->start_addr[1] = TEST_AREA1A;
                        info->test_size[1] = (addr_end + 1) - TEST_AREA1A;
                    } else if (addr_end <= HOLE1A_END) {
                        info->area_num = 2;
                        info->test_size[0] = (TEST_AREA0_END + 1) - addr_tmp;

                        info->start_addr[1] = TEST_AREA1A;
                        info->test_size[1] = (TEST_AREA1A_END + 1) - TEST_AREA1A;
                    } else {
                        info->area_num = 3;
                        info->test_size[0] = (TEST_AREA0_END + 1) - addr_tmp;

                        info->start_addr[1] = TEST_AREA1A;
                        info->test_size[1] = TEST_SIZE1A;

                        info->start_addr[2] = TEST_AREA2;
                        info->test_size[2] = (addr_end > TEST_AREA2_END)? ((TEST_AREA2_END + 1) - TEST_AREA2) : ((addr_end + 1) - TEST_AREA2);
                    }
                 } else if (addr_tmp <= HOLE0_END){
                    if (addr_end <= HOLE0_END) {
                        info->area_num = 0;
                    } else if (addr_end <= TEST_AREA1A_END) {
                        info->area_num = 1;
                        info->start_addr[0] = TEST_AREA1A;
                        info->test_size[0] = (addr_end + 1) - TEST_AREA1A;
                    } else if (addr_end <= HOLE1A_END) {
                        info->area_num = 1;
                        info->start_addr[0] = TEST_AREA1A;
                        info->test_size[0] = (TEST_AREA1A_END + 1) - TEST_AREA1A;
                    } else {
                        info->area_num = 2;
                        info->start_addr[0] = TEST_AREA1A;
                        info->test_size[0] = TEST_SIZE1A;

                        info->start_addr[1] = TEST_AREA2;
                        info->test_size[1] = (addr_end > TEST_AREA2_END)? ((TEST_AREA2_END + 1) - TEST_AREA2) : ((addr_end + 1) - TEST_AREA2);
                    }
                } else if (addr_tmp <= TEST_AREA1A_END) {
                    info->start_addr[0] = addr_tmp;

                    if (addr_end <= TEST_AREA1A_END) {
                        info->area_num = 1;
                        info->test_size[0] = size_tmp;
                    } else if (addr_end <= HOLE1A_END) {
                        info->area_num = 1;
                        info->test_size[0] = (TEST_AREA1A_END + 1) - addr_tmp;
                    } else {
                        info->area_num = 2;
                        info->test_size[0] = (TEST_AREA1A_END + 1) - addr_tmp;

                        info->start_addr[1] = TEST_AREA2;
                        info->test_size[1] = (addr_end > TEST_AREA2_END)? ((TEST_AREA2_END + 1) - TEST_AREA2) : ((addr_end + 1) - TEST_AREA2);
                    }
                }else if (addr_tmp <= HOLE1A_END){
                    if (addr_end <= HOLE1A_END) {
                        info->area_num = 0;
                    } else {
                        info->area_num = 1;
                        info->start_addr[0] = TEST_AREA2;
                        info->test_size[0] = (addr_end > TEST_AREA2_END)? ((TEST_AREA2_END + 1) - TEST_AREA2) : ((addr_end + 1) - TEST_AREA2);
                    }
                } else {
                    info->area_num = 1;
                    info->start_addr[0] = TEST_AREA2;
                    info->test_size[0] = (addr_end > TEST_AREA2_END)? ((TEST_AREA2_END + 1) - TEST_AREA2) : ((addr_end + 1) - TEST_AREA2);
                }

            } else {
                if (addr_tmp <= TEST_AREA0_END) {
                    info->start_addr[0] = addr_tmp;

                    if (addr_end <= TEST_AREA0_END) {
                        info->area_num = 1;
                        info->test_size[0] = size_tmp;
                    } else if (addr_end <= HOLE0_END) {
                        info->area_num = 1;
                        info->test_size[0] = (TEST_AREA0_END + 1) - addr_tmp;
                    } else if (addr_end <= TEST_AREA1B_END) {
                        info->area_num = 2;
                        info->test_size[0] = (TEST_AREA0_END + 1) - addr_tmp;

                        info->start_addr[1] = TEST_AREA1B;
                        info->test_size[1] = (addr_end + 1) - TEST_AREA1B;
                    } else if (addr_end <= HOLE1B_END) {
                        info->area_num = 2;
                        info->test_size[0] = (TEST_AREA0_END + 1) - addr_tmp;

                        info->start_addr[1] = TEST_AREA1B;
                        info->test_size[1] = (TEST_AREA1B_END + 1) - TEST_AREA1B;
                    } else {
                        info->area_num = 3;
                        info->test_size[0] = (TEST_AREA0_END + 1) - addr_tmp;

                        info->start_addr[1] = TEST_AREA1B;
                        info->test_size[1] = TEST_SIZE1B;

                        info->start_addr[2] = TEST_AREA2;
                        info->test_size[2] = (addr_end > TEST_AREA2_END)? ((TEST_AREA2_END + 1) - TEST_AREA2) : ((addr_end + 1) - TEST_AREA2);
                    }
                 } else if (addr_tmp <= HOLE0_END){
                    if (addr_end <= HOLE0_END) {
                        info->area_num = 0;
                    } else if (addr_end <= TEST_AREA1B_END) {
                        info->area_num = 1;
                        info->start_addr[0] = TEST_AREA1B;
                        info->test_size[0] = (addr_end + 1) - TEST_AREA1B;
                    } else if (addr_end <= HOLE1B_END) {
                        info->area_num = 1;
                        info->start_addr[0] = TEST_AREA1B;
                        info->test_size[0] = (TEST_AREA1B_END + 1) - TEST_AREA1B;
                    } else {
                        info->area_num = 2;
                        info->start_addr[0] = TEST_AREA1B;
                        info->test_size[0] = TEST_SIZE1B;

                        info->start_addr[1] = TEST_AREA2;
                        info->test_size[1] = (addr_end > TEST_AREA2_END)? ((TEST_AREA2_END + 1) - TEST_AREA2) : ((addr_end + 1) - TEST_AREA2);
                    }
                } else if (addr_tmp <= TEST_AREA1B_END) {
                    info->start_addr[0] = addr_tmp;

                    if (addr_end <= TEST_AREA1B_END) {
                        info->area_num = 1;
                        info->test_size[0] = size_tmp;
                    } else if (addr_end <= HOLE1B_END) {
                        info->area_num = 1;
                        info->test_size[0] = (TEST_AREA1B_END + 1) - addr_tmp;
                    } else {
                        info->area_num = 2;
                        info->test_size[0] = (TEST_AREA1B_END + 1) - addr_tmp;

                        info->start_addr[1] = TEST_AREA2;
                        info->test_size[1] = (addr_end > TEST_AREA2_END)? ((TEST_AREA2_END + 1) - TEST_AREA2) : ((addr_end + 1) - TEST_AREA2);
                    }
                }else if (addr_tmp <= HOLE1B_END){
                    if (addr_end <= HOLE1B_END) {
                        info->area_num = 0;
                    } else {
                        info->area_num = 1;
                        info->start_addr[0] = TEST_AREA2;
                        info->test_size[0] = (addr_end > TEST_AREA2_END)? ((TEST_AREA2_END + 1) - TEST_AREA2) : ((addr_end + 1) - TEST_AREA2);
                    }
                } else {
                    info->area_num = 1;
                    info->start_addr[0] = TEST_AREA2;
                    info->test_size[0] = (addr_end > TEST_AREA2_END)? ((TEST_AREA2_END + 1) - TEST_AREA2) : ((addr_end + 1) - TEST_AREA2);
                }
            }
			i = i + 3;
		} else if (strcmp(argv[i], "-c") == 0) {
			info->test_case = simple_strtoul(argv[i + 1], NULL, 10);
			i = i + 2;
		} else {
			ILL_CMD;
		}
	}

	printf("II: # of iteration: %d\n", info->test_loops);
	return MT_SUCCESS;
}

int dram_memory_test(int argc, char *const argv[]) {
	u32 i, j;
	int retcode = MT_SUCCESS;
	ddr_cmd_parsing_info_t cmd_info;

    g_bit_width = readl(0x1111004);
    g_dram_type = readl(0x1111008);
    g_ddr_size  = readl(0x111100c);
    g_ddr_freq  = readl(0x1111010);

	retcode = ddr_cmd_parsing(argc, argv, &cmd_info);
	if (MT_ILL_CMD == retcode) return retcode;

	for (j = 0; j < cmd_info.test_loops; j++) {
		for (i = 0; i < cmd_info.area_num; i++) {
			printf("<Range %d: 0x%08x~0x%08x>\n", i + 1, cmd_info.start_addr[i],
			       cmd_info.start_addr[i] + cmd_info.test_size[i] - 1);

			switch (cmd_info.test_case) {
				/* [test_case]
				 *  0: The general case, including word/half word/byte/unalign test
				 *  1:
				 */
				case 2:
				case 1:
					printf("WW: Test Case Not Implemented Yet!!\n");
					break;

				case 0:
				default:
					retcode = _dram_test(cmd_info.start_addr[i], cmd_info.test_size[i]);
					if (MT_FAIL == retcode){
                        printf("\nEE: mdram_test failed.\n\n");
					    goto end;
					}
					break;
			}
		}

		printf("<mdram_test %d runs>                  \n\n", j + 1);
	}

end:
    writel(g_bit_width, 0x1111004);
    writel(g_dram_type, 0x1111008);
    writel(g_ddr_size,  0x111100c);
    writel(g_ddr_freq,  0x1111010);
	return MT_SUCCESS;
}

