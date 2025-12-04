/*
 * Include Files
 */
#include <common.h>
#include <linux/ctype.h>
#include <asm/io.h>

/* Definitions for debug & error handing manner */
#define MT_SUCCESS (0)
#define MT_FAIL (-1)
#define MT_ILL_CMD (-2)
#define HANDLE_FAIL ({ return MT_FAIL; })

#define _cache_flush()

/*
 * DRAM TEST RANGES:
 * TEST_AREA1: From 0 ~ (UBOOT_BASE-SIZE_3MB)
 * TEST_AREA2: From (UBOOT_BASE+SIZE_3MB) ~ DRAM_END
 */
#define DRAM_SIZE (readl(0x111100c) * 0x100000)
#define SIZE_3MB (0x300000)
#define TEST_AREA_BASE1 (unsigned int)(0x00000000)
#define TEST_AREA_BASE1_END (unsigned int)(CONFIG_SYS_TEXT_BASE - SIZE_3MB)
#define TEST_AREA_SIZE1 (unsigned int)(TEST_AREA_BASE1_END & 0x1FFFFFFF)
#define TEST_AREA_BASE2 (unsigned int)(CONFIG_SYS_TEXT_BASE + SIZE_3MB)
#define TEST_AREA_SIZE2 \
	(unsigned int)(DRAM_SIZE - (TEST_AREA_BASE2 & 0x1FFFFFFF))

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
	u32 start_addr[2];
	u32 test_size[2];
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
	volatile u32 *read_start;
	volatile u32 *start;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i = 0; i < (sizeof(dram_patterns) / sizeof(u32)); i++) {
		_cache_flush();

		/* write pattern*/
		start = (u32 *)(uintptr_t)(dram_start + (i * size_per_pat) % (area_size));
		read_start = (u32 *)((uintptr_t)start + size_per_pat - 4);
		for (j = 0; j < size_per_pat; j = j + 4) {
			*start = dram_patterns[i];
			start++;
		}

		_cache_flush();

		/* check data */
		for (j = 0; j < size_per_pat; j = j + 4) {
			read_value = (*read_start);
			if (read_value != dram_patterns[i]) {
				printf("\naddr(0x%x): 0x%x != pattern(0x%x) %s, %d\n",
				       (u32)(uintptr_t)read_start, read_value, dram_patterns[i],
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			read_start--;
		}
		printf("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i],
		       (u32)(uintptr_t)start);
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
	volatile u16 *start_h;
	volatile u32 *start_w;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i = 0; i < (sizeof(dram_patterns) / sizeof(u32)); i++) {
		_cache_flush();

		/* write half-word pattern*/
		start_h = (u16 *)(uintptr_t)(dram_start + (i * size_per_pat) % (area_size));
		start_w = (u32 *)(uintptr_t)(dram_start + (i * size_per_pat) % (area_size) +
		                             size_per_pat - 4);
		test_start = (u32)(uintptr_t)start_h;

		for (j = 0; j < size_per_pat; j = j + 4) {
			h_word = (u16)(dram_patterns[i]);
			*start_h = h_word;
			h_word = (u16)(dram_patterns[i] >> 16);
			*(start_h + 1) = h_word;
			start_h += 2;
		}

		_cache_flush();

		/* read word and check data */
		for (j = 0; j < size_per_pat; j = j + 4) {
			if ((u32)(uintptr_t)start_w < test_start) {
				break;
			}
			read_value = (*start_w);
			if (read_value != dram_patterns[i]) {
				printf("\naddr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",
				       (u32)(uintptr_t)start_w, read_value, dram_patterns[i],
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			start_w--;
		}
		printf("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i],
		       (u32)(uintptr_t)start_h);
	}
	printf("%s completed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

int dram_byte_access(u32 dram_start, u32 size_per_pat, u32 area_size) {
	int i, j;
	u8 byte;
	u32 read_value, test_start;
	volatile u8 *start_b;
	volatile u32 *start_w;

	printf("=======start %s test=======\r", __FUNCTION__);
	for (i = 0; i < (sizeof(dram_patterns) / sizeof(u32)); i++) {
		_cache_flush();

		/* write byte pattern*/
		start_w = (u32 *)(uintptr_t)(dram_start + (i * size_per_pat) % (area_size) +
		                             size_per_pat - 4);
		start_b = (u8 *)(uintptr_t)(dram_start + (i * size_per_pat) % (area_size));
		test_start = (u32)(uintptr_t)start_b;

		for (j = 0; j < size_per_pat; j = j + 4) {
			byte = (u8)(dram_patterns[i]);
			*(start_b + 0) = byte;
			byte = (u8)(dram_patterns[i] >> 8);
			*(start_b + 1) = byte;
			byte = (u8)(dram_patterns[i] >> 16);
			*(start_b + 2) = byte;
			byte = (u8)(dram_patterns[i] >> 24);
			*(start_b + 3) = byte;
			start_b += 4;
		}

		_cache_flush();

		/* read word and check data */
		for (j = 0; j < size_per_pat; j = j + 4) {
			if ((u32)(uintptr_t)start_w < test_start) {
				break;
			}
			read_value = *start_w;
			if (read_value != dram_patterns[i]) {
				printf("\naddr:0x%x(0x%x) != pattern(0x%x) %s, %d\n",
				       (u32)(uintptr_t)start_w, read_value, dram_patterns[i],
				       __FUNCTION__, __LINE__);
				HANDLE_FAIL;
			}
			start_w--;
		}
		printf("pattern[%d](0x%x) 0x%x pass\r", i, dram_patterns[i],
		       (u32)(uintptr_t)start_b);
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
		off1 =
		  (t_off_1 *)(uintptr_t)(dram_start + (j * size_per_pat) % (area_size));
		/* set value */
		for (i = 0; i < (size_per_pat / sizeof(t_off_1)); i++) {
			off1[i].c1 = 0xcc;
			off1[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for (i = 0; i < (size_per_pat / sizeof(t_off_1)); i++) {
			if ((off1[i].w != dram_patterns[j]) || (off1[i].c1 != 0xcc)) {
				printf("\noffset 1 error:addr(0x%x) write 0x%x, read 0x%x, c1(%02x)\n",
				       (u32)(uintptr_t)&off1[i], dram_patterns[j], off1[i].w,
				       off1[i].c1);
				HANDLE_FAIL;
			}
		}

		/* offset 2 bytes */
		off2 =
		  (t_off_2 *)(uintptr_t)(dram_start + (j * size_per_pat) % (area_size));
		/* set value */
		for (i = 0; i < (size_per_pat / sizeof(t_off_2)); i++) {
			off2[i].c1 = 0xcc;
			off2[i].c2 = 0xdd;
			off2[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for (i = 0; i < (size_per_pat / sizeof(t_off_2)); i++) {
			if (off2[i].w != dram_patterns[j] || (off2[i].c1 != 0xcc) ||
			    (off2[i].c2 != 0xdd)) {
				printf(
				  "\noffset 2 error:addr(0x%x) write 0x%x, read 0x%x, c1(0x%x), "
				  "c2(0x%x)\n",
				  (u32)(uintptr_t)&off2[i], dram_patterns[j], off2[i].w, off2[i].c1,
				  off2[i].c2);
				printf("&dram_pattern[%d](0x%p) = 0x%x\r", j, &dram_patterns[j],
				       dram_patterns[j]);
				HANDLE_FAIL;
			}
		}

		/* offset 3 bytes */
		off3 =
		  (t_off_3 *)(uintptr_t)(dram_start + (j * size_per_pat) % (area_size));
		/* set value */
		for (i = 0; i < (size_per_pat / sizeof(t_off_3)); i++) {
			off3[i].c1 = 0xcc;
			off3[i].c2 = 0xdd;
			off3[i].c3 = 0xee;
			off3[i].w = dram_patterns[j];
		}
		_cache_flush();
		/* check data */
		for (i = 0; i < (size_per_pat / sizeof(t_off_3)); i++) {
			if (off3[i].w != dram_patterns[j] || (off3[i].c1 != 0xcc) ||
			    (off3[i].c2 != 0xdd) || (off3[i].c3 != 0xee)) {
				printf(
				  "\noffset 3 error:addr(0x%x) write 0x%x, "
				  "read 0x%x, c1(%02x), c2(%02x), c3(%02x)\n",
				  (u32)(uintptr_t)&off1[i], dram_patterns[j], off3[i].w, off3[i].c1,
				  off3[i].c2, off3[i].c3);
				HANDLE_FAIL;
			}
		}
		printf("pattern[%d](0x%x) 0x%x pass\r", j, dram_patterns[j],
		       (u32)(uintptr_t)off3);
	}
	printf("%s test passed.                   \r", __FUNCTION__);
	return MT_SUCCESS;
}

int _dram_test(u32 dram_start, u32 size_per_pat, u32 area_size) {
	if (MT_FAIL == unaligned_test(dram_start, size_per_pat, area_size))
		return MT_FAIL;

	if (MT_FAIL == dram_byte_access(dram_start, size_per_pat, area_size))
		return MT_FAIL;

	if (MT_FAIL == dram_half_word_access(dram_start, size_per_pat, area_size))
		return MT_FAIL;

	if (MT_FAIL == dram_normal_patterns(dram_start, size_per_pat, area_size))
		return MT_FAIL;

	return MT_SUCCESS;
}

int ddr_cmd_parsing(int argc, char *const argv[],
                    ddr_cmd_parsing_info_t *info) {
	u32 i, loop_cnt_tmp;

#define ILL_CMD \
	({ \
		printf("EE: Illegal command (%d).\n", __LINE__); \
		return MT_ILL_CMD; \
	})

	/* Initialize the memory test parameters..... */
	info->area_num = 2;
	info->test_loops = 1;
	info->start_addr[0] = TEST_AREA_BASE1;
	info->start_addr[1] = TEST_AREA_BASE2;
	info->test_size[0] = TEST_AREA_SIZE1;
	info->test_size[1] = TEST_AREA_SIZE2;

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
		} else if ((strcmp(argv[i], "-range") == 0) ||
		           (strcmp(argv[i], "-r") == 0)) {
			if (((i + 2) >= argc) || (isxdigit(*argv[i + 1]) == 0) ||
			    (isxdigit(*argv[i + 2]) == 0))
				ILL_CMD;

			u32 addr_tmp = simple_strtoul(argv[i + 1], NULL, 10);
			u32 size_tmp = simple_strtoul(argv[i + 2], NULL, 10);
			if ((0 == addr_tmp) || (0 == size_tmp)) {
				printf(
				  "EE: Please assign the memory test range: -r <start address> "
				  "<size>.\n");
				return MT_FAIL;
			}
			if ((size_tmp > DRAM_SIZE) || (addr_tmp > DRAM_SIZE)) {
				printf("EE: Incorrect memory test rnage.\n");
				ILL_CMD;
			}

			info->area_num = 1;
			if (addr_tmp < TEST_AREA_SIZE1) {
				info->start_addr[0] = addr_tmp;
				info->test_size[0] = (addr_tmp + size_tmp >= TEST_AREA_BASE1_END) ?
				                       (TEST_AREA_BASE1_END - addr_tmp) :
				                       size_tmp;
			} else if (addr_tmp >= TEST_AREA_BASE2) {
				info->start_addr[0] = addr_tmp;
				info->test_size[0] = (addr_tmp + size_tmp >= DRAM_SIZE) ?
				                       (DRAM_SIZE - addr_tmp) :
				                       size_tmp;
			} else {
				printf("EE: The assigned test range is for stack and u-boot use.\n");
				ILL_CMD;
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
	u32 size_per_pattern = 0x10000;
	u32 i, j;
	int retcode = MT_SUCCESS;
	ddr_cmd_parsing_info_t cmd_info;

	retcode = ddr_cmd_parsing(argc, argv, &cmd_info);
	if (MT_ILL_CMD == retcode) return retcode;

	for (j = 0; j < cmd_info.test_loops; j++) {
		for (i = 0; i < cmd_info.area_num; i++) {
			printf("<Range %d: 0x%x~0x%x>\n", i + 1, cmd_info.start_addr[i],
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
					retcode = _dram_test(cmd_info.start_addr[i], size_per_pattern,
					                     cmd_info.test_size[i]);
					if (MT_FAIL == retcode) goto end;
					break;
			}
		}

		printf("<mdram_test %d runs>                  \n\n", j + 1);
	}

end:
	return retcode;
}
