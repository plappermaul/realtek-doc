#include <common.h>
#include <nand.h>
#include "cortina_spi_nand.h"

struct mtd_info *mtd;
u32 *wbuf = (u32 *)0x01000000;
u32 *rbuf = (u32 *)0x01010000;

#define BLOCK_OFFSET    (6)
#define ROW_ADDR(b, p)  ((b<<BLOCK_OFFSET)|p)
#define PATTERN1 ((ROW_ADDR(b, p)<<16) | i*4)
#define PATTERN4 (((ROW_ADDR(b, p)&0xFF)<<24)|ROW_ADDR(b, p))

#define VIR_OFF (b*info->block_size+p*info->page_size)
#define PHY_OFF spi_nand_get_phy_offset(mtd,(b*info->block_size+p*info->page_size))


typedef struct {
	u32 loops;
	u32 start_blk;
	u32 blk_count;
	u32 page_offset;
    u32 f_test_case;
    u8 f_reset;
    u8 f_bad;
} snaf_test_info_t;

#define SET_SEED 0
#define GET_SEED 1
/*
  get_or_set = GET_SEED: get seed
  get_or_set = SET_SEED: set seed
*/
static void __srandom32(u32 *a1, u32 *a2, u32 *a3, u32 get_or_set)
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

static u32 __random32(void)
{
#define TAUSWORTHE(s,a,b,c,d) ((s&c)<<d) ^ (((s <<a) ^ s)>>b)
    u32 s1, s2, s3;
    __srandom32(&s1, &s2, &s3, GET_SEED);

    s1 = TAUSWORTHE(s1, 13, 19, 4294967294UL, 12);
    s2 = TAUSWORTHE(s2, 2, 25, 4294967288UL, 4);
    s3 = TAUSWORTHE(s3, 3, 11, 4294967280UL, 17);

    __srandom32(&s1, &s2, &s3, SET_SEED);

    return (s1 ^ s2 ^ s3);
}

int snaf_pattern_test(struct spi_nand_info *info, snaf_test_info_t *ti)
{
    struct mtd_oob_ops ops = {0};
    int ret = 0;
    u32 b, p, i;
    u32 psize, sb, nb, np;
    psize = info->page_size;
    sb = ti->start_blk;
    nb = ti->start_blk+ti->blk_count;
    np = 64;
    u32 pattern[10] = {
        0xa55aa55a, 0x5a5aa5a5, 0xff00ff00, 0xaabbccdd, 0xace1ace1,
        0xa5a5a5a5, 0x5a5a5a5a, 0x00ff00ff, 0x24683579, 0x13572468,
    };
    u32 pat_num = sizeof(pattern)/sizeof(u32);

    printf("II: do %s... ",__FUNCTION__);
    for (b=sb; b<nb; b++) {
        // generate patterns
        for (i=0; i<psize/4; i++) {
            wbuf[i] = pattern[(i%pat_num)];
        }

        // verify patterns
        for (i=0; i<psize/4; i++) {
            if (pattern[(i%pat_num)] != wbuf[i]) {
                printf("\nEE: pattern in addr(0x%llx):0x%x !=0x%x\n", (uint64_t)&wbuf[i], wbuf[i], pattern[(i%pat_num)]);
                return -1;
            }
        }

        // Erase block
        ret = spi_nand_erase(info, (uint64_t)b*info->block_size, (uint64_t)info->block_size);
        if (ret) {
            printf("\nEE: Erase 0x%x fail\n",b*info->block_size);
            return -1;
        }

        // Check erased block
        for (p=0; p < np; p++) {
            memset(rbuf, 0, info->page_size);
            ops.datbuf = (u8 *)rbuf;
            ops.len = psize;
            spi_nand_read_oob(info, VIR_OFF, &ops);
            for(i=0; i<psize/4; i++) {
                if(rbuf[i]!= 0xFFFFFFFF) {
                    printf("\nEE: B[%04d], P[%04d], vir(0x%x), phy(0x%x), read data 0x%x!=ALL_ONE\n", b, p, VIR_OFF, PHY_OFF, rbuf[i]);
                    return -1;
                }
            }
        }

        printf("write BLK%04d...\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", b);
        for(p=0; p<np; p++) {
            ops.datbuf = (u8 *)wbuf;
            ops.len = psize;
            ret = spi_nand_write_oob(info, VIR_OFF, &ops);
            if(ret){
                printf("\nEE: Write 0x%x fail\n",VIR_OFF);
                return -1;
            }
        }
    }

    for(b=sb; b<nb; b++) {
        printf("verify BLK%04d...\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", b);
        // generate patterns
        for(i=0; i<psize/4; i++) {
            wbuf[i] = pattern[(i%pat_num)];
        }

        // verify patterns
        for(i=0; i<psize/4; i++) {
            if(pattern[(i%pat_num)] != wbuf[i]) {
                printf("\nEE: pattern in addr(0x%llx):0x%x !=0x%x\n", (uint64_t)&wbuf[i], wbuf[i], pattern[(i%pat_num)]);
                return 1;
            }
        }

        for(p=0; p<np; p++) {
            // data check
            memset(rbuf, 0, info->page_size);
            ops.datbuf = (u8 *)rbuf;
            ops.len = psize;
            spi_nand_read_oob(info, VIR_OFF, &ops);
            if(ret){
                printf("\nEE: Read 0x%x fail\n",VIR_OFF);
                return -1;
            }

            // verify data
            for(i=0; i<psize/4; i++) {
                if(rbuf[i]!= wbuf[i]) {
                    printf("\nEE: B[%04d], P[%04d], vir(0x%x), phy(0x%x), read data [%d]0x%x!=[%d]0x%x\n",
                    b, p, VIR_OFF, PHY_OFF, i, rbuf[i], i, wbuf[i]);
                    return 1;
                }
            }
        }
    }
    puts("done       \n");
    return 0;
}

int snaf_ramdom_val_test(struct spi_nand_info *info, snaf_test_info_t *ti)
{
    struct mtd_oob_ops ops = {0};
    int ret = 0;
    u32 b, p, i;
    u32 psize, sb, nb, np;
    psize = info->page_size;
    sb = ti->start_blk;
    nb = ti->start_blk+ti->blk_count;
    np = 64;
    unsigned int rs1, rs2, rs3;

    printf("II: do %s... ",__FUNCTION__);

    rs1=0x13243;rs2=0xaaa0bdd;rs3=0xfffbda0;
    __srandom32(&rs1, &rs2, &rs3, SET_SEED);
    for(b=sb; b<nb; b++) {
        // generate patterns
        for(i=0; i<psize/4; i++) {
            wbuf[i] = __random32();
        }

        // Erase block
        ret = spi_nand_erase(info, (uint64_t)b*info->block_size, (uint64_t)info->block_size);
        if(ret) return -1;

        // Check erased block
        for(p=0; p < np; p++) {
            memset(rbuf, 0, info->page_size);
            ops.datbuf = (u8 *)rbuf;
            ops.len = psize;
            spi_nand_read_oob(info, VIR_OFF, &ops);
            for(i=0; i<psize/4; i++) {
                if(rbuf[i]!= 0xFFFFFFFF) {
                    printf("EE: B[%04d], P[%04d], vir(0x%x), phy(0x%x), read data 0x%x!=ALL_ONE\n", b, p, VIR_OFF, PHY_OFF, rbuf[i]);
                    return -1;
                }
            }
        }

        printf("write BLK%04d...\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", b);
        for(p=0; p < np; p++) {
            ops.datbuf = (u8 *)wbuf;
            ops.len = psize;
            spi_nand_write_oob(info, VIR_OFF, &ops);
        }
    }

    rs1=0x13243;rs2=0xaaa0bdd;rs3=0xfffbda0;
    __srandom32(&rs1, &rs2, &rs3, SET_SEED);
    for(b=sb; b<nb; b++) {
        printf("verify BLK%04d...\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", b);
        // generate patterns
        for(i=0; i<psize/4; i++) {
            wbuf[i] = __random32();;
        }

        for(p=0; p < np; p++) {
            // data check
            memset(rbuf, 0, info->page_size);
            ops.datbuf = (u8 *)rbuf;
            ops.len = psize;
            spi_nand_read_oob(info, VIR_OFF, &ops);

            // verify data
            for(i=0; i<psize/4; i++) {
                if(rbuf[i]!= wbuf[i]) {
                    printf("EE: B[%04d], P[%04d], vir(0x%x), phy(0x%x), read data 0x%x!=0x%x\n",
                    b, p, VIR_OFF, PHY_OFF, rbuf[i], wbuf[i]);
                    return 1;
                }
            }

        }
    }
    puts("done             \n");
    return 0;
}

int snaf_erase_without_hidden(struct spi_nand_info *info, snaf_test_info_t *ti)
{
    int ret = 0;
    u32 b, sb, nb;
    sb = ti->start_blk;
    nb = ti->start_blk+ti->blk_count;

    printf("II: do %s... 0x%x ~ 0x%x\n",__FUNCTION__, sb*info->block_size, nb*info->block_size);

    nand_acc_phy_offs_addr(1);
    for(b=sb; b<nb; b++) {
        // Erase block
        printf("erase BLK%04d...\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", b);
        ret = spi_nand_block_markunbad(info, (uint64_t)b*info->block_size);
        if(ret){
            printf("\nEE: block %d erase failed (offset = 0x%x)\n",b, b*info->block_size);
            nand_acc_phy_offs_addr(0);
            return -1;
        }
    }
    nand_acc_phy_offs_addr(0);
    puts("done             \n");
    return 0;
}

int snaf_address_pattern_test(struct spi_nand_info *info, snaf_test_info_t *ti)
{
    struct mtd_oob_ops ops = {0};
    int ret = 0;
    u32 b, p, i;
    u32 psize, sb, nb, np;
    psize = info->page_size;
    sb = ti->start_blk;
    nb = ti->start_blk+ti->blk_count;
    np = 64;

    printf("II: do %s... ",__FUNCTION__);

    for(b=sb; b<nb; b++) {
        ret = spi_nand_erase(info, (uint64_t)b*info->block_size, (uint64_t)info->block_size);
        if(ret) return -1;

        printf("write BLK%04d...\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", b);
        // Start write
        for(p=0; p < np; p++) {
            for(i=0; i<psize/4; i++) { wbuf[i] = PATTERN1; }
            ops.datbuf = (u8 *)wbuf;
            ops.len = psize;
            spi_nand_write_oob(info, VIR_OFF, &ops);
        }
    }

    for(b=sb; b<nb; b++) {
        printf("verify BLK%04d...\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", b);

        for(p=0; p < np; p++) {
            // data check
            memset(rbuf, 0, info->page_size);
            ops.datbuf = (u8 *)rbuf;
            ops.len = psize;
            spi_nand_read_oob(info, VIR_OFF, &ops);

            // verify data
            for(i=0; i<psize/4; i++) { wbuf[i] = PATTERN1; }
            for(i=0; i<psize/4; i++) {
                if(rbuf[i]!= wbuf[i]) {
                    printf("EE: B[%04d], P[%04d], vir(0x%x), phy(0x%x), read data (%d)0x%x!=(%d)0x%x\n",
                    b, p, VIR_OFF, PHY_OFF, i, rbuf[i], i, wbuf[i]);
                    return 1;
                }
            }

        }
    }

    puts("done             \n");
    return 0;
}

int snaf_virtual_to_physical_addr(struct spi_nand_info *info, snaf_test_info_t *ti)
{
    printf("VIR:0x%x => PHY:0x%x\n",ti->page_offset, spi_nand_get_phy_offset(mtd,ti->page_offset));
    return 0;
}

int snaf_dump_with_hidden_page(struct spi_nand_info *info, snaf_test_info_t *ti)
{
	int i;
	u8 *datbuf, *oobbuf, *p;
	int ret = 0;
	u32 off = ti->page_offset;

    nand_acc_phy_offs_addr(1);

	datbuf = (u8 *)rbuf;
	oobbuf = (u8 *)wbuf;

	off &= ~(info->page_size - 1);
	loff_t addr = (loff_t) off;
	struct mtd_oob_ops ops;
	memset(&ops, 0, sizeof(ops));
	memset(datbuf, 0, info->page_size);
	memset(oobbuf, 0, info->oob_size);
	ops.datbuf = datbuf;
	ops.oobbuf = oobbuf;
	ops.len = info->page_size;
	ops.ooblen = info->oob_size;
	ops.mode = MTD_OPS_RAW;
	i = spi_nand_read_oob(info, addr, &ops);

	if (i < 0) {
		printf("Error (%d) reading page offset 0x%08x\n", i, off);
		ret = 1;
		return -1;
	}
	printf("Page offset 0x%08x dump:\n", off);


	i = info->page_size >> 4;
	p = datbuf;

	while (i--) {
		printf("\t%02x %02x %02x %02x %02x %02x %02x %02x"
		       "  %02x %02x %02x %02x %02x %02x %02x %02x\n",
		       p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
		       p[8], p[9], p[10], p[11], p[12], p[13], p[14],
		       p[15]);
		p += 16;
	}

	puts("OOB:\n");
	i = mtd->oobsize >> 3;
	p = oobbuf;
	while (i--) {
		printf("\t%02x %02x %02x %02x %02x %02x %02x %02x\n",
		       p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
		p += 8;
	}

    nand_acc_phy_offs_addr(0);
	return ret;
}


enum cmd_status {
    ERR_INVALD_CMD = -1,
    FUNC_DONE = 0xD,
    FUNC_MT = 0,
};

extern bool spi_nand_detect_onfi(struct spi_nand_info *info, u8 printout);

int cmd_parsing(int argc, char * const argv[], snaf_test_info_t *ti, struct spi_nand_info *info)
{
	u32 i=1, err;

    /* Special command, loop is not needed */
    if (0==(strcmp(argv[i],"-eh")) || 0==(strcmp(argv[i],"-erase_with_hidden")) ) {
        u32 blk, cnt;
        if((i+2) >= argc) { err=ERR_INVALD_CMD; goto error; }
        blk = simple_strtoul(argv[i+1], NULL, 10);
        cnt = simple_strtoul(argv[i+2], NULL, 10);
        printf("DD: input range %d %d\n", blk, cnt);
        if(blk >= info->block_size ) { err=ERR_INVALD_CMD; goto error; }
        if(0==cnt || cnt >= info->block_size ) { err=ERR_INVALD_CMD; goto error; }
        ti->start_blk=blk;
        ti->blk_count=cnt;
        i+=3;
        snaf_erase_without_hidden(info, ti);
        return FUNC_DONE;
    }  else if (0==(strcmp(argv[i],"-onfi"))) {
        i++;
        spi_nand_detect_onfi(info, 1);
        return FUNC_DONE;
    } else if (0==(strcmp(argv[i],"-dh"))) {
        ti->page_offset = simple_strtoul(argv[i+1], NULL, 16);
        i+=2;
        snaf_dump_with_hidden_page(info, ti);
        return FUNC_DONE;
    } else if (0==(strcmp(argv[i],"-v2p"))) {
        ti->page_offset = simple_strtoul(argv[i+1], NULL, 16);
        i+=2;
        snaf_virtual_to_physical_addr(info, ti);
        return FUNC_DONE;
    } else if( 0==strcmp(argv[i],"-mt") ) {
        i++;
    	while(i<argc) {
    		if ('-' != argv[i][0]) {
                err = ERR_INVALD_CMD; goto error;
    		}

            if( 0==(strcmp(argv[i],"-l")) || 0==(strcmp(argv[i],"-loops")) ) {
                u32 loop;
                if((i+1) >= argc) { err=ERR_INVALD_CMD; goto error; }
                loop = simple_strtoul(argv[i+1], NULL, 10);
                if (0==loop) { puts("WW: invalid loop count(reset to 1)\n"); loop=1; }
                ti->loops = loop;
                i+=2;
            } else if (0==(strcmp(argv[i],"-r")) || 0==(strcmp(argv[i],"-range")) ) {
                u32 blk, cnt;
                if((i+2) >= argc) { err=ERR_INVALD_CMD; goto error; }
                blk = simple_strtoul(argv[i+1], NULL, 10);
                cnt = simple_strtoul(argv[i+2], NULL, 10);
                printf("DD: input range %d %d\n", blk, cnt);
                if(blk >= info->block_size ) { err=ERR_INVALD_CMD; goto error; }
                if(0==cnt || cnt >= info->block_size ) { err=ERR_INVALD_CMD; goto error; }
                ti->start_blk=blk;
                ti->blk_count=cnt;
                i+=3;
            }else if (0==(strcmp(argv[i],"-tc")) || 0==(strcmp(argv[i],"-test_case")) ) {
                /* 0: address pattern (Default)
                           * 1: random_val
                           * 2: fixed pattern
                           * A: All test case
                           */
                ti->f_test_case=simple_strtoul(argv[i+1], NULL, 10);
                i+=2;
            }
        }
    }else {
        printf("WW: unknown command \"%s\" ignored.\n", argv[i]);
        return ERR_INVALD_CMD;
    }


    if (0==ti->blk_count) { puts("EE: input error\n"); return ERR_INVALD_CMD;}

    return FUNC_MT;
error:
    printf("EE: incomplete commands (type: %d)\n", err);
    return ERR_INVALD_CMD;
}


int do_snaf_test (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	mtd = get_nand_dev_by_index(0);
	if (!mtd)
		return 1;

	struct nand_chip *chip = mtd_to_nand(mtd);
    struct spi_nand_info *info = (struct spi_nand_info *)chip->priv;


    u32 i=1, ret=0;
    snaf_test_info_t ti={0};
    ti.loops = 1;

    ret = cmd_parsing(argc, argv, &ti, info);
    if(ret == FUNC_DONE) return 0;
    else if(ret == ERR_INVALD_CMD) return 1;

    printf("II: spi_nand test info:\n\tstart: BLK%04d, end: BLK%04d\n", ti.start_blk, ti.start_blk+ti.blk_count-1);

    while(i<=ti.loops) {
        printf("II: #%d test\n", i++);

        /* 0: address pattern(Default)
              * 1: random_val
              * 2: fixed pattern
              * 5: All test case
              */
        switch(ti.f_test_case){
            case 0:
                snaf_address_pattern_test(info, &ti);
                break;
            case 1:
                snaf_ramdom_val_test(info, &ti);
                break;
            case 2:
                snaf_pattern_test(info, &ti);
                break;
            case 5:
                snaf_ramdom_val_test(info, &ti);
                snaf_address_pattern_test(info, &ti);
                snaf_pattern_test(info, &ti);
                break;
        }
    }
    return 0;
}


U_BOOT_CMD(
    mspi_nand, 10, 1, do_snaf_test,
    "Realtek spi-nand flash engineer command.",
    "-mt -r/-range <start block> <block count> [-l/-loops <test loops>] [-tc/-test_case <0/1/2/5>]\n"\
    "    => Do spi-nand flash test.\n" \
    "    => [-tc/-test_case <0/1/2/5>]: 0:address_pattern, 1:ramdom_val, 2:pattern\n"
    "mspi_nand -onfi\n"\
    "    => Getting ONFI if exists.\n" \
    "mspi_nand -eh/-erase_with_hidden <start block> <block count>\n"\
    "    => Erase with hidden blocks\n" \
    "mspi_nand -dh/-dump_with_hidden <page_offset>\n"\
    "    => Dump page with hidden blocks\n" \
    "mspi_nand -v2p <page_offset>\n"\
    "    => Translate input address to physical offset\n" \
);
