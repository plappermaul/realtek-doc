/*
 * Driver for NAND support, Rick Bronson
 * borrowed heavily from:
 * (c) 1999 Machine Vision Holdings, Inc.
 * (c) 1999, 2000 David Woodhouse <dwmw2@infradead.org>
 *
 * Ported 'dynenv' to 'nand env.oob' command
 * (C) 2010 Nanometrics, Inc.
 * 'dynenv' -- Dynamic environment offset in NAND OOB
 * (C) Copyright 2006-2007 OpenMoko, Inc.
 * Added 16-bit nand support
 * (C) 2004 Texas Instruments
 *
 * Copyright 2010 Freescale Semiconductor
 * The portions of this file whose copyright is held by Freescale and which
 * are not considered a derived work of GPL v2-only code may be distributed
 * and/or modified under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 */

#include <common.h>
#include <linux/mtd/mtd.h>
#include <command.h>
#include <watchdog.h>
#include <malloc.h>
#include <asm/byteorder.h>
#include <jffs2/jffs2.h>
#include <nand.h>

#if defined(CONFIG_CMD_MTDPARTS)

/* partition handling routines */
int mtdparts_init(void);
int id_parse(const char *id, const char **ret_id, u8 *dev_type, u8 *dev_num);
int find_dev_and_part(const char *id, struct mtd_device **dev,
              u8 *part_num, struct part_info **part);
#endif
int dump_mem(unsigned int addr, int len);

extern void spi_nand_info(void) ;
extern int spi_nand_read_write(uint32_t offset, uint32_t length, u_char *buffer, char opt);
extern int spi_nand_write_ecc(uint32_t offset, uint32_t length, u_char *buffer);
extern int spi_nand_read_ecc(uint32_t offset, uint32_t length, u_char *buffer);
extern int spi_nand_erase(uint32_t offset, uint32_t length);
struct mtd_info* get_mtd_info(void);
#define nand_curr_device 0
#ifndef CONFIG_SYS_MAX_NAND_DEVICE
#define CONFIG_SYS_MAX_NAND_DEVICE 1
#endif

static int nand_dump(nand_info_t *nand, ulong off, int only_oob, int repeat)
{
    int i;
    u_char *datbuf, *oobbuf, *p;
    static loff_t last;

    if (repeat)
        off = last + nand->writesize;

    last = off;

    datbuf = memalign(ARCH_DMA_MINALIGN, nand->writesize + nand->oobsize);
    oobbuf = memalign(ARCH_DMA_MINALIGN, nand->oobsize);

    if (!datbuf || !oobbuf) {
        puts("No memory for page buffer\n");
        return 1;
    }
    off &= ~(nand->writesize - 1);

    printk("dump off is %ld\n",off);
    loff_t addr = (loff_t) off;
    struct mtd_oob_ops ops;
    memset(&ops, 0, sizeof(ops));
    ops.datbuf = datbuf;
    ops.oobbuf = oobbuf; /* must exist, but oob data will be appended to ops.datbuf */
    ops.len = nand->writesize;
    ops.ooblen = nand->oobsize;
    ops.mode = MTD_OOB_RAW;
    printk("nand_dump 001 \r\n");
    i = nand->read_oob(nand, addr, &ops);
    if (i < 0) {
        printf("Error (%d) reading page %08lx\n", i, off);
        free(datbuf);
        free(oobbuf);
        return 1;
    }
    printf("Page %08lx dump:\n", off);
    i = nand->writesize >> 4;
    p = datbuf;

    while (i--) {
        if (!only_oob)
            printf("\t%02x %02x %02x %02x %02x %02x %02x %02x"
                   "  %02x %02x %02x %02x %02x %02x %02x %02x\n",
                   p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
                   p[8], p[9], p[10], p[11], p[12], p[13], p[14],
                   p[15]);
        p += 16;
    }
    puts("OOB:\n");
    i = nand->oobsize >> 3;
    while (i--) {
        printf("\t%02x %02x %02x %02x %02x %02x %02x %02x\n",
               p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
        p += 8;
    }
    free(datbuf);
    free(oobbuf);

    return 0;
}

/* ------------------------------------------------------------------------- */

static int set_dev(int dev)
{
    /*nand_info_t *nand = (nand_info_t *)get_mtd_info();
    if (dev < 0 || dev >= CONFIG_SYS_MAX_NAND_DEVICE ||
        !nand_info[dev].name) {
        puts("No such device\n");
        return -1;
    }

    if (nand_curr_device == dev)
        return 0;

    printf("Device %d: %s", dev, nand->name);
    puts("... is now current device\n");
    nand_curr_device = dev;

#ifdef CONFIG_SYS_NAND_SELECT_DEVICE
    board_nand_select_device(nand_info[dev].priv, dev);
#endif
    */
    return 0;
}

static inline int str2off(const char *p, loff_t *num)
{
    char *endptr;

    *num = simple_strtoull(p, &endptr, 16);
    return *p != '\0' && *endptr == '\0';
}

static inline int str2long(const char *p, ulong *num)
{
    char *endptr;

    *num = simple_strtoul(p, &endptr, 16);
    return *p != '\0' && *endptr == '\0';
}

static int get_part(const char *partname, int *idx, loff_t *off, loff_t *size)
{
#ifdef CONFIG_CMD_MTDPARTS
    struct mtd_device *dev;
    struct part_info *part;
    u8 pnum;
    int ret;

    ret = mtdparts_init();
    if (ret)
        return ret;

    ret = find_dev_and_part(partname, &dev, &pnum, &part);
    if (ret)
        return ret;

    if (dev->id->type != MTD_DEV_TYPE_NAND) {
        puts("not a NAND device\n");
        return -1;
    }

    *off = part->offset;
    *size = part->size;
    *idx = dev->id->num;

    ret = set_dev(*idx);
    if (ret)
        return ret;

    return 0;
#else
    puts("offset is not a number\n");
    return -1;
#endif
}

static int arg_off(const char *arg, int *idx, loff_t *off, loff_t *maxsize)
{
    nand_info_t *nand = (nand_info_t *)get_mtd_info();
    if (!str2off(arg, off))
        return get_part(arg, idx, off, maxsize);

    if (*off >= nand->size) {
        puts("Offset exceeds device limit\n");
        return -1;
    }

    *maxsize = nand->size - *off;
    return 0;
}

static int arg_off_size(int argc, char *const argv[], int *idx,
            loff_t *off, loff_t *size)
{
    nand_info_t *nand = (nand_info_t *)get_mtd_info();
    int ret;
    loff_t maxsize = 0;

    if (argc == 0) {
        *off = 0;
        *size = nand->size;
        goto print;
    }

    ret = arg_off(argv[0], idx, off, &maxsize);
    if (ret)
        return ret;

    if (argc == 1) {
        *size = maxsize;
        goto print;
    }

    if (!str2off(argv[1], size)) {
        printf("'%s' is not a number\n", argv[1]);
        return -1;
    }

    if (*size > maxsize) {
        puts("Size exceeds partition or device limit\n");
        return -1;
    }

print:
    printf("device %d ", *idx);
    if (*size == nand->size)
        puts("whole chip\n");
    else
        printf("offset 0x%llx, size 0x%llx\n",
               (unsigned long long)*off, (unsigned long long)*size);
    return 0;
}

#ifdef CONFIG_CMD_NAND_LOCK_UNLOCK
static void print_status(ulong start, ulong end, ulong erasesize, int status)
{
    printf("%08lx - %08lx: %08lx blocks %s%s%s\n",
        start,
        end - 1,
        (end - start) / erasesize,
        ((status & NAND_LOCK_STATUS_TIGHT) ?  "TIGHT " : ""),
        ((status & NAND_LOCK_STATUS_LOCK) ?  "LOCK " : ""),
        ((status & NAND_LOCK_STATUS_UNLOCK) ?  "UNLOCK " : ""));
}

static void do_nand_status(nand_info_t *nand)
{
    ulong block_start = 0;
    ulong off;
    int last_status = -1;

    struct nand_chip *nand_chip = nand->priv;
    /* check the WP bit */
    nand_chip->cmdfunc(nand, NAND_CMD_STATUS, -1, -1);
    printf("device is %swrite protected\n",
        (nand_chip->read_byte(nand) & 0x80 ?
        "NOT " : ""));

    for (off = 0; off < nand->size; off += nand->erasesize) {
        int s = nand_get_lock_status(nand, off);

        /* print message only if status has changed */
        if (s != last_status && off != 0) {
            print_status(block_start, off, nand->erasesize,
                    last_status);
            block_start = off;
        }
        last_status = s;
    }
    /* Print the last block info */
    print_status(block_start, off, nand->erasesize, last_status);
}
#endif

static void nand_print_and_set_info(int idx)
{
    nand_info_t *nand = (nand_info_t *)get_mtd_info();
    struct nand_chip *chip = nand->priv;
    const int bufsz = 32;
    char buf[bufsz];

    printf("Device %d: ", idx);
    if (chip->numchips > 1)
        printf("%dx ", chip->numchips);
    printf("%s, sector size %u KiB\n",
           nand->name, nand->erasesize >> 10);
    printf("  Page size  %8d b\n", nand->writesize);
    printf("  OOB size   %8d b\n", nand->oobsize);
    printf("  Erase size %8d b\n", nand->erasesize);


    /* Set geometry info */
    sprintf(buf, "%x", nand->writesize);
    setenv("nand_writesize", buf);

    sprintf(buf, "%x", nand->oobsize);
    setenv("nand_oobsize", buf);

    sprintf(buf, "%x", nand->erasesize);
    setenv("nand_erasesize", buf);
}

static int raw_access(nand_info_t *nand, ulong addr, loff_t off,
              ulong count, int read, int no_verify)
{
    int ret = 0;

    while (count--) {
        /* Raw access */
        mtd_oob_ops_t ops = {
            .datbuf = (u8 *)addr,
            .oobbuf = ((u8 *)addr) + nand->writesize,
            .len = nand->writesize,
            .ooblen = nand->oobsize,
            .mode = MTD_OOB_RAW
        };

        if (read) {
            ret = nand->read_oob(nand, off, &ops);
        } else {
            if (!no_verify) {
                if (nand_block_isbad(nand, off)) {
                    printf ("Skip bad block 0x%08llx\n",
                            off & ~(nand->erasesize - 1));
                    off += nand->erasesize - (off & (nand->erasesize - 1));
                    count++;
                    continue;
                }
            }
            ret = nand->write_oob(nand, off, &ops);
            //if (!ret && !no_verify)
            //    ret = nand_verify_page_oob(mtd, &ops, off);
        }

        if (ret) {
            printf("%s: error at offset %llx, ret %d\n",
                __func__, (long long)off, ret);
            break;
        }

        addr += nand->writesize + nand->oobsize;
        off += nand->writesize;
    }

    return ret;
}

/* Adjust a chip/partition size down for bad blocks so we don't
 * read/write past the end of a chip/partition by accident.
 */
static void adjust_size_for_badblocks(loff_t *size, loff_t offset, int dev)
{
    /* We grab the nand info object here fresh because this is usually
     * called after arg_off_size() which can change the value of dev.
     */
    nand_info_t *nand = (nand_info_t *)get_mtd_info();
    loff_t maxoffset = offset + *size;
    int badblocks = 0;

    /* count badblocks in NAND from offset to offset + size */
    for (; offset < maxoffset; offset += nand->erasesize) {
        if (nand_block_isbad(nand, offset))
            badblocks++;
    }
    /* adjust size if any bad blocks found */
    if (badblocks) {
        *size -= badblocks * nand->erasesize;
        printf("size adjusted to 0x%llx (%d bad blocks)\n",
               (unsigned long long)*size, badblocks);
    }
}

int do_spi_nand(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
    ulong addr;
    loff_t off, size;
	char *cmd, *s;
    nand_info_t *nand = (nand_info_t *)get_mtd_info();
#ifdef CONFIG_SYS_NAND_QUIET
    int quiet = CONFIG_SYS_NAND_QUIET;
#else
    int quiet = 0;
#endif
    const char *quiet_str = getenv("quiet");
    int dev = nand_curr_device;
    int repeat = flag & CMD_FLAG_REPEAT;

	/* at least two arguments please */
	if (argc<2)
		goto usage;

    if (quiet_str)
        quiet = simple_strtoul(quiet_str, NULL, 0) != 0;

	cmd = argv[1];

    /* Only "dump" is repeatable. */
    if (repeat && strcmp(cmd, "dump"))
        return 0;

    if (strcmp(cmd, "info") == 0) {
		spi_nand_info();
		return 0;
    }

    if (strcmp(cmd, "device") == 0) {
        if (argc < 3) {
            putc('\n');
            if (dev < 0 || dev >= CONFIG_SYS_MAX_NAND_DEVICE)
                puts("no devices available\n");
            else
                nand_print_and_set_info(dev);
            return 0;
        }

        dev = (int)simple_strtoul(argv[2], NULL, 10);
        set_dev(dev);

        return 0;
    }
        
    if (strcmp(cmd, "bad") == 0) {
        printf("\nDevice %d bad blocks:\n", dev);
        for (off = 0; off < nand->size; off += nand->erasesize){
            if (nand_block_isbad(nand, off))
                printf("  %08llx\n", (unsigned long long)off);
        }
        printf("NAND size is  %08llx, Erase size is  %08x\n",nand->size, nand->erasesize);
			return 0;
		}

    /*
     * Syntax is:
     *   0    1     2       3    4
     *   nand erase [clean] [off size]
     */
    if (strncmp(cmd, "erase", 5) == 0 || strncmp(cmd, "scrub", 5) == 0) {
        nand_erase_options_t opts;
        /* "clean" at index 2 means request to write cleanmarker */
        int clean = argc > 2 && !strcmp("clean", argv[2]);
        int scrub_yes = argc > 2 && !strcmp("-y", argv[2]);
        int o = (clean || scrub_yes) ? 3 : 2;
        int scrub = !strncmp(cmd, "scrub", 5);
        int spread = 0;
        int args = 2;
        const char *scrub_warn =
            "Warning: "
            "scrub option will erase all factory set bad blocks!\n"
            "         "
            "There is no reliable way to recover them.\n"
            "         "
            "Use this command only for testing purposes if you\n"
            "         "
            "are sure of what you are doing!\n"
            "\nReally scrub this NAND flash? <y/N>\n";

        if (cmd[5] != 0) {
            if (!strcmp(&cmd[5], ".spread")) {
                spread = 1;
            } else if (!strcmp(&cmd[5], ".part")) {
                args = 1;
            } else if (!strcmp(&cmd[5], ".chip")) {
                args = 0;
            } else {
                goto usage;
            }
        }

        /*
         * Don't allow missing arguments to cause full chip/partition
         * erases -- easy to do accidentally, e.g. with a misspelled
         * variable name.
         */
        if (argc != o + args)
            goto usage;

        printf("\nNAND %s: ", cmd);
        /* skip first two or three arguments, look for offset and size */
        if (arg_off_size(argc - o, argv + o, &dev, &off, &size) != 0)
            return 1;

        //nand = &nand_info[dev];

        memset(&opts, 0, sizeof(opts));
        opts.offset = off;
        opts.length = size;
        opts.jffs2  = clean;
        opts.quiet  = quiet;
        opts.spread = spread;

        if (scrub) {
            if (!scrub_yes)
                puts(scrub_warn);

            if (scrub_yes)
                opts.scrub = 1;
            else if (getc() == 'y') {
                puts("y");
                if (getc() == '\r')
                    opts.scrub = 1;
                else {
                    puts("scrub aborted\n");
                    return -1;
                }
            } else {
                puts("scrub aborted\n");
                return -1;
            }
        }
        ret = nand_erase_opts(nand, &opts);
        printf("%s\n", ret ? "ERROR" : "OK");

        return ret == 0 ? 0 : 1;
    }

    if (strncmp(cmd, "dump", 4) == 0) {
        if (argc < 3)
            goto usage;

        off = (int)simple_strtoul(argv[2], NULL, 16);
        ret = nand_dump(nand, off, !strcmp(&cmd[4], ".oob"), repeat);

        return ret == 0 ? 1 : 0;
    }

    if (strncmp(cmd, "read", 4) == 0 || strncmp(cmd, "write", 5) == 0) {
        size_t rwsize;
        ulong pagecount = 1;
        int read;
        int raw = 0;
        int no_verify = 0;

        if (argc < 4)
            goto usage;

        addr = (ulong)simple_strtoul(argv[2], NULL, 16);

        read = strncmp(cmd, "read", 4) == 0; /* 1 = read, 0 = write */
        printf("\nNAND %s: ", read ? "read" : "write");

		s = strchr(cmd, '.');

        if (s && !strncmp(s, ".raw", 4)) {
            char *_s = s;
            int raw_size = 0;
            uint32_t raw_unit_size = nand->writesize + nand->oobsize;
            raw = 1;

            while(_s) {
                _s = strchr(_s+1, '.');
                if (_s && !strncmp(_s, ".noverify", 9)) {
                    no_verify = 1;
                } else if (_s && !strncmp(_s, ".size", 5)) {
                    raw_size = 1;
                }
            }
            if (arg_off(argv[3], &dev, &off, &size))
                return 1;

            if (set_dev(dev))
                return 1;


            if (argc > 4 && !str2long(argv[4], &pagecount)) {
                printf("'%s' is not a number\n", argv[4]);
                return 1;
            }

            if (raw_size) {
                pagecount = (pagecount+(raw_unit_size-1))/raw_unit_size;
                printf("transfer size to %ld page(s),", pagecount);
            }

            if (pagecount * nand->writesize > size) {
                puts("Size exceeds partition or device limit\n");
                return -1;
            }

            rwsize = pagecount * (raw_unit_size);
		} else {
            if (arg_off_size(argc - 3, argv + 3, &dev, &off,
                         &size) != 0)
                return 1;

            if (set_dev(dev))
                return 1;

            /* size is unspecified */
            if (argc < 5)
                adjust_size_for_badblocks(&size, off, dev);
            rwsize = size;
        }
        if (!s || !strcmp(s, ".jffs2") ||
            !strcmp(s, ".e") || !strcmp(s, ".i")) {
            if (read)
                ret = nand_read_skip_bad(nand, off, &rwsize,
                             (u_char *)addr);
            else
                ret = nand_write_skip_bad(nand, off, &rwsize,
                              (u_char *)addr, 0);
#ifdef CONFIG_CMD_NAND_TRIMFFS
        } else if (!strcmp(s, ".trimffs")) {
            if (read) {
                printf("Unknown nand command suffix '%s'\n", s);
                return 1;
            }
            ret = nand_write_skip_bad(nand, off, &rwsize,
                        (u_char *)addr,
                        WITH_DROP_FFS);
#endif
#ifdef CONFIG_CMD_NAND_YAFFS
        } else if (!strcmp(s, ".yaffs")) {
            if (read) {
                printf("Unknown nand command suffix '%s'.\n", s);
                return 1;
            }
            ret = nand_write_skip_bad(nand, off, &rwsize,
                        (u_char *)addr, WITH_YAFFS_OOB);
#endif
        } else if (!strcmp(s, ".oob")) {
            /* out-of-band data */
            mtd_oob_ops_t ops = {
                .oobbuf = (u8 *)addr,
                .ooblen = rwsize,
                .mode = MTD_OOB_RAW
            };

            if (read)
                ret = nand->read_oob(nand, off, &ops);
            else
                ret = nand->write_oob(nand, off, &ops);
        } else if (raw) {
            ret = raw_access(nand, addr, off, pagecount, read,
                     no_verify);
        } else {
            printf("Unknown nand command suffix '%s'.\n", s);
            return 1;
		}
	
        printf(" %zu bytes %s: %s\n", rwsize,
               read ? "read" : "written", ret ? "ERROR" : "OK");

        return ret == 0 ? 0 : 1;
    }

    if (strcmp(cmd, "markbad") == 0) {
        argc -= 2;
        argv += 2;

        if (argc <= 0)
            goto usage;

        while (argc > 0) {
            addr = simple_strtoul(*argv, NULL, 16);

            if (nand->block_markbad(nand, addr)) {
                printf("block 0x%08lx NOT marked "
                    "as bad! ERROR %d\n",
                    addr, ret);
                ret = 1;
            } else {
                printf("block 0x%08lx successfully "
                    "marked as bad\n",
                    addr);
            }
            --argc;
            ++argv;
        }
		return ret;
	}

    if (strcmp(cmd, "biterr") == 0) {
        /* todo */
        return 1;
    }

usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
    spi_nand, CONFIG_SYS_MAXARGS, 1, do_spi_nand,
	"SPI-NAND sub-system",
	"info - show available NAND devices\n"
    "spi_nand device [dev] - show or set current device\n"
    "spi_nand read - addr off|partition size\n"
    "spi_nand write - addr off|partition size\n"
	"    read/write 'size' bytes starting at offset 'off'\n"
    "    to/from memory address 'addr', skipping bad blocks.\n"
    "spi_nand read.raw - addr off|partition [count]\n"
    "spi_nand write.raw[.noverify] - addr off|partition [count]\n"
    "    Use read.raw/write.raw to avoid ECC and access the flash as-is.\n"
    "    'count' is the page count\n"
#ifdef CONFIG_CMD_NAND_TRIMFFS
    "spi_nand write.trimffs - addr off|partition size\n"
    "    write 'size' bytes starting at offset 'off' from memory address\n"
    "    'addr', skipping bad blocks and dropping any pages at the end\n"
    "    of eraseblocks that contain only 0xFF\n"
#endif
#ifdef CONFIG_CMD_NAND_YAFFS
    "spi_nand write.yaffs - addr off|partition size\n"
    "    write 'size' bytes starting at offset 'off' with yaffs format\n"
    "    from memory address 'addr', skipping bad blocks.\n"
#endif
    "spi_nand erase[.spread] [clean] off size - erase 'size' bytes "
	"from offset 'off'\n"
    "    With '.spread', erase enough for given file size, otherwise,\n"
    "    'size' includes skipped bad blocks.\n"
    "spi_nand erase.part [clean] partition - erase entire mtd partition'\n"
    "spi_nand erase.chip [clean] - erase entire chip'\n"
    "spi_nand bad - show bad blocks\n"
    "spi_nand dump[.oob] off - dump page\n"
    "spi_nand scrub [-y] off size | scrub.part partition | scrub.chip\n"
    "    really clean NAND erasing bad blocks (UNSAFE)\n"
    "spi_nand markbad off [...] - mark bad block(s) at offset (UNSAFE)\n"
    "spi_nand biterr off - make a bit error at offset (UNSAFE)"
);

U_BOOT_CMD(
    nand, CONFIG_SYS_MAXARGS, 1, do_spi_nand,
    "NAND sub-system",
    "alias spi_nand"
);



static int nand_load_image(cmd_tbl_t *cmdtp, nand_info_t *nand,
               ulong offset, ulong addr, char *cmd)
{
    int r;
    char *s;
    size_t cnt;
    size_t temp_cnt;
    image_header_t *hdr;
#if defined(CONFIG_FIT)
    const void *fit_hdr = NULL;
#endif
printk("nand_load_image\n");
    s = strchr(cmd, '.');
    if (s != NULL &&
        (strcmp(s, ".jffs2") && strcmp(s, ".e") && strcmp(s, ".i"))) {
        printf("Unknown nand load suffix '%s'\n", s);
        show_boot_progress(-53);
        return 1;
    }

    printf("\nLoading from %s, offset 0x%lx\n", nand->name, offset);

    cnt = nand->writesize;
    r = nand_read_skip_bad(nand, offset, &cnt, (u_char *) addr);
    if (r) {
        puts("** Read error\n");
        show_boot_progress (-56);
        return 1;
    }
    show_boot_progress (56);

    switch (genimg_get_format ((void *)addr)) {
    case IMAGE_FORMAT_LEGACY:
        hdr = (image_header_t *)addr;

        show_boot_progress (57);
        image_print_contents (hdr);

        cnt = image_get_image_size (hdr);
        break;
#if defined(CONFIG_FIT)
    case IMAGE_FORMAT_FIT:
        fit_hdr = (const void *)addr;
        puts ("Fit image detected...\n");

        cnt = fit_get_size (fit_hdr);
        break;
#endif
    default:
        show_boot_progress (-57);
        puts ("** Unknown image type\n");
        return 1;
    }
    show_boot_progress (57);

    //Page size alignment

    printk("cnt=  0x%x\n",cnt);
    temp_cnt=cnt;

    if( cnt%nand->writesize ){
        temp_cnt +=(nand->writesize-(cnt%nand->writesize));
    }


    printk("temp_cnt=  0x%x\n",temp_cnt);

//    r = nand_read_skip_bad(nand, offset, &cnt, (u_char *) addr);
    r = nand_read_skip_bad(nand, offset, &temp_cnt, (u_char *) addr);

    if (r) {
        puts("** Read error\n");
        show_boot_progress (-58);
        return 1;
    }
    show_boot_progress (58);

#if defined(CONFIG_FIT)
    /* This cannot be done earlier, we need complete FIT image in RAM first */
    if (genimg_get_format ((void *)addr) == IMAGE_FORMAT_FIT) {
        if (!fit_check_format (fit_hdr)) {
            show_boot_progress (-150);
            puts ("** Bad FIT image format\n");
            return 1;
        }
        show_boot_progress (151);
        fit_print_contents (fit_hdr);
    }
#endif

    /* Loading ok, update default load address */

    load_addr = addr;

    return bootm_maybe_autostart(cmdtp, cmd);
}

int do_nandboot(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    nand_info_t *nand = (nand_info_t *)get_mtd_info();
    char *boot_device = NULL;
    int idx;
    ulong addr, offset = 0;
#if defined(CONFIG_CMD_MTDPARTS)
    struct mtd_device *dev;
    struct part_info *part;
    u8 pnum;

    if (argc >= 2) {
        char *p = (argc == 2) ? argv[1] : argv[2];
        if (!(str2long(p, &addr)) && (mtdparts_init() == 0) &&
            (find_dev_and_part(p, &dev, &pnum, &part) == 0)) {
            if (dev->id->type != MTD_DEV_TYPE_NAND) {
                puts("Not a NAND device\n");
                return 1;
            }
            if (argc > 3)
                goto usage;
            if (argc == 3)
                addr = simple_strtoul(argv[1], NULL, 16);
            else
                addr = CONFIG_SYS_LOAD_ADDR;
            return nand_load_image(cmdtp, nand,
                           part->offset, addr, argv[0]);
        }
    }
#endif

    show_boot_progress(52);
    switch (argc) {
    case 1:
        addr = CONFIG_SYS_LOAD_ADDR;
        boot_device = getenv("bootdevice");
        break;
    case 2:
        addr = simple_strtoul(argv[1], NULL, 16);
        boot_device = getenv("bootdevice");
        break;
    case 3:
        addr = simple_strtoul(argv[1], NULL, 16);
        boot_device = argv[2];
        break;
    case 4:
        addr = simple_strtoul(argv[1], NULL, 16);
        boot_device = argv[2];
        offset = simple_strtoul(argv[3], NULL, 16);
        break;
    default:
#if defined(CONFIG_CMD_MTDPARTS)
usage:
#endif
        show_boot_progress(-53);
        return cmd_usage(cmdtp);
    }

    show_boot_progress(53);
    if (!boot_device) {
        puts("\n** No boot device **\n");
        show_boot_progress(-54);
        return 1;
    }
    show_boot_progress(54);

    idx = simple_strtoul(boot_device, NULL, 16);

    if (idx < 0 || idx >= CONFIG_SYS_MAX_NAND_DEVICE || !nand->name) {
        printf("\n** Device %d not available\n", idx);
        show_boot_progress(-55);
        return 1;
    }
    show_boot_progress(55);

    return nand_load_image(cmdtp, nand, offset, addr, argv[0]);
}

U_BOOT_CMD(nboot, 4, 1, do_nandboot,
    "boot from NAND device",
    "[partition] | [[[loadAddr] dev] offset]"
);




int dump_mem(unsigned int addr, int len)
{
unsigned char    *raw,ch;
int            row,col,rowsz;

    raw = (unsigned char *)addr;
    if (len == 0) {
         rowsz = 20;
    }
    else {
        rowsz = (len + 15)/16;
    }
#ifdef BANK_AUTO_SWITCH
    unsigned int bank;
    bank = getbank();
    switch(bank)
    {
        case 1:
            addr += 0x400000;
            break;
        case 2:
            addr += 0x800000;
            break;
        case 3:
            addr += 0xC00000;
            break;
    }
#endif

    for (row=0;row<rowsz;row++)
    {
         // Address
         printf("0x%08X: ",(addr + row * 16));

         // Show HEX
         for (col=0;col<8;col++) {
             printf("%02X ",raw[col]);
         }
        //printf("- ");
         for (col=8;col<16;col++) {
             printf("%02X ",raw[col]);
         }

         // Show ASCII
         for (col=0;col<16;col++) {
             if ((raw[col] < 0x20) || (raw[col] > 0x7e)) {
                 ch = '.';
             }
             else {
                 if ((raw[col] == 0x25) || (raw[col] == 0x5c))
                     ch = '.';
                 else
                     ch = raw[col];
             }
             printf("%c",ch);
         }

         raw += 16;

         printf("\n\r");

    }    // end of for
    return 1;
}

