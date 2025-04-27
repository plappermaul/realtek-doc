#include <common.h>
#include <soc.h>
#include <spi_nand/spi_nand_common.h>
#include <spi_nand/spi_nand_blr_util.h>

static int snaf_test_and_set_block_health_status(int argc, char * const argv[]);
static int do_snaf_test_and_set_block_health_status(cmd_tbl_t *cmdtp,	int flag, int argc,
                                                    char * const argv[]);

typedef int32_t (*fps32_u32_u8p_t)(u32_t, u8_t *);

u32_t spi_nand_chip_size(void);
u32_t spi_nand_page_size(void);
u32_t spi_nand_block_size(void);
u32_t spi_nand_spare_size(void);
u32_t spi_nand_cs_count(void);
void spi_nand_init(void);

U_BOOT_CMD(nand_badblock, 6, 0, do_snaf_test_and_set_block_health_status,
           "test/set block health status",
           "[action] [target]\n"
           "  suported actions:\n"
           "    -b: mark the given block(s) as bad\n"
           "    -g: mark the given block(s) as good\n"
           "    -a: test and set health status of given block(s)\n"
           "    -e: like -a, except with ECC info (to be implemented)\n"
           "  supported targets:\n"
           "    <block_id>: apply actions to block ID\n"
           "          -all: apply actions to all blocks except block 0\n"
           "        -r m n: apply actions to blocks ranging from m ~ n\n"
           "  examples:\n"
           "    nand_badblock -a -all (erase all blocks but block 0 and mark bad blocks)\n"
           "    nand_badblock -g -r 10 15 (erase block 10 ~ 15)"
           );

#define BLK_G (1)
#define BLK_B (1 << 1)
#define BLK_E (1 << 2)

#ifndef BLOCK_PAGE_ADDR
#define BLOCK_PAGE_ADDR(block_0_1023, page_0_63) ((0x00<<16)|(block_0_1023<<6)|(page_0_63))
#endif

/* Adapted from loader, ./src/lib/misc/string.c, for atoi() */
#define inline_ascii_value(c, base) ({	  \
			char __ascii_c=c; \
			int __ascii_base=base; \
			int __ascii_r; \
			if ((__ascii_c<='9') && (__ascii_c>='0')) { \
				__ascii_r=__ascii_c-'0'; \
			} else { \
				__ascii_c|=32; \
				__ascii_r=((__ascii_base==16)&&(__ascii_c<='f')&&(__ascii_c>='a'))? \
					(__ascii_c-'a'+10): \
					-1; \
			} \
			__ascii_r; \
		})

static s32_t numlen(const u32_t n) {
	if (n > 999) {
		return 3;
	} else if ( n > 99) {
		return 2;
	} else if ( n > 9) {
		return 1;
	} else {
		return 0;
	}
}

static s32_t atoi(const char *v) {
	const char *orig_v = v;
	int base=10, d, r=0;
	if (*v=='\0') return 0;
	if ((v[0]=='0') && ((v[1]&~32)=='X')) {
		v+=2;
		base=16;
	}
	while ((d=inline_ascii_value(*(v++), base))>=0) {
		r *= base;
		r += d;
	}

	if ((orig_v[0] != '0') && (r == 0)) {
		r = -1;
	}
	return r;
}

static int
do_snaf_test_and_set_block_health_status(cmd_tbl_t *cmdtp,
                                         int flag,
                                         int argc,
                                         char * const argv[]) {
	return snaf_test_and_set_block_health_status(argc, argv);
}

static int snaf_tsbh_set_good(u32_t bid, u8_t *dummy __attribute__((unused))) {
	int ret = nasu_block_erase(BLOCK_PAGE_ADDR(bid, 0));

	if (ret == -1) {
		return BLK_E;
	} else {
		return BLK_G;
	}
}

static int snaf_tsbh_set_bad(u32_t bid, u8_t *buf) {
	nasu_page_write(buf, BLOCK_PAGE_ADDR(bid, 0));
	nasu_page_write(buf, BLOCK_PAGE_ADDR(bid, 1));

	return BLK_E;
}

static int snaf_tsbh_test_and_set(u32_t bid, u8_t *buf) {
	u32_t i, col;

	for (i=0; i<(spi_nand_block_size()/spi_nand_page_size()); i++) {
		nasu_page_read(buf, BLOCK_PAGE_ADDR(bid, i));
		for (col=0; col<spi_nand_page_size(); col++) {
			if (buf[col] == 0) {
				memset(buf, 0, spi_nand_page_size() + spi_nand_spare_size());
				snaf_tsbh_set_bad(bid, buf);
				return BLK_B;
			}
		}
	}
	return BLK_G;
}

static int snaf_tsbh_range(const u32_t s, const u32_t e, u8_t *bst,
                           fps32_u32_u8p_t fp, u8_t *buf) {
	u32_t b = s;
	s8_t *backward[] = {"\b", "\b\b", "\b\b\b", "\b\b\b\b"};

	printf(" %d ~ %d", s, s);
	while (b <= e) {
		bst[b] |= fp(b, buf);
		printf("%s%d", backward[numlen(b==s?b:b-1)], b);
		b++;
	}
	puts("\n");

	return 0;
}

static void snaf_tsbh_show_execution_result(const u32_t blk_num, u8_t *bst) {
	u32_t b;
	u32_t blk_checked_num;
	u32_t blk_bad_num;

	blk_checked_num = 0;
	blk_bad_num = 0;

	for (b=0; b<blk_num; b++) {
		if (bst[b]) {
			blk_checked_num++;
		}

		if (bst[b] & (BLK_B | BLK_E)) {
			blk_bad_num++;
		}
	}

	printf("II: %d blocks processed, %d blocks error/bad",
	       blk_checked_num, blk_bad_num);
	if (blk_bad_num) {
		puts(" [");
		for (b=0; b<blk_num; b++) {
			if (bst[b] & (BLK_B | BLK_E)) {
				printf("%d, ", b);
			}
		}
		puts("\b\b]");
	}
	puts("\n");

	return;
}

static int snaf_test_and_set_block_health_status(int argc, char * const argv[]) {
	const u32_t blk_num = spi_nand_cs_count() * spi_nand_chip_size() / spi_nand_block_size();
	u32_t blk_start, blk_end;
	u8_t option;
	u8_t *block_status_table;
	u8_t *rwbuf;

	if (spi_nand_page_size() != 2048) {
		printf("EE: page size is not 2048 bytes\n");
		return 1;
	}

	if (argc < 3) {
		printf("EE: argument number error\n");
		return 1;
	}

	if ((argv[1][0] != '-')) {
		printf("EE: unknown argument[1]\n");
		return 1;
	} else {
		option = argv[1][1];
	}

	if (argv[2][0] == '-') {
		if (strcmp(argv[2], "-all") == 0) {
			blk_start = 1;
			blk_end = blk_num - 1;
		} else if (strcmp(argv[2], "-r") == 0) {
			if (argc < 5) {
				printf("EE: argument number error\n");
				return 1;
			}
			blk_start = atoi(argv[3]);
			blk_end = atoi(argv[4]);
		} else {
			printf("EE: unknown target: %s\n", argv[2]);
			return 1;
		}
	} else {
		blk_start = atoi(argv[2]);
		blk_end = blk_start;
	}

	if ((blk_start == -1)      || (blk_end == -1)      ||
	    (blk_start >= blk_num) || (blk_end >= blk_num)) {
		printf("EE: block index error\n");
		return 1;
	}

	if (blk_end < blk_start) {
		blk_end = blk_start;
	}

	block_status_table = __builtin_alloca(blk_num);
	memset(block_status_table, 0, blk_num);

	rwbuf = __builtin_alloca(spi_nand_page_size() + spi_nand_spare_size());

	/* printf("II: NAND_BADBLOCK:\n" */
	/*        "\tchip size:  %u MB\n" */
	/*        "\tblock size: %u KB\n" */
	/*        "\tpage size:  %u B\n" */
	/*        "\tspare size: %u B\n", */
	/*        (spi_nand_chip_size() / 1024 / 1024), */
	/*        (spi_nand_block_size() / 1024), */
	/*        spi_nand_page_size(), */
	/*        spi_nand_spare_size()); */

	printf("II: erasing blocks");
	snaf_tsbh_range(blk_start, blk_end, block_status_table,
	                snaf_tsbh_set_good, rwbuf);

	switch (option) {
	case 'g':
		/* Erasing the given block is all -g needs */
		break;
	case 'b':
		memset(rwbuf, 0, spi_nand_page_size() + spi_nand_spare_size());
		printf("II: marking bad blocks");
		snaf_tsbh_range(blk_start, blk_end, block_status_table,
		                snaf_tsbh_set_bad, rwbuf);
		break;
	case 'a':
		printf("II: testing blocks");
		snaf_tsbh_range(blk_start, blk_end, block_status_table,
		                snaf_tsbh_test_and_set, rwbuf);
		break;
	case 'e':
		break;
	default:
		printf("EE: unknown action, %c\n", option);
		return 1;
	}

	snaf_tsbh_show_execution_result(blk_num, block_status_table);

	/* regenerate skip_table */
	spi_nand_init();

	return 0;
}
