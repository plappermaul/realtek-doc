/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


/*
 * Boot support
 */
#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <u-boot/zlib.h>
#include <bzlib.h>
#include <environment.h>
#include <lmb.h>
#include <linux/ctype.h>
#include <asm/byteorder.h>
#include <linux/compiler.h>

#include <rtk_flash_common.h>
#include <turnkey/sysinfo.h>

#if defined(CONFIG_CMD_USB)
#include <usb.h>
#endif

#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif

#if defined(CONFIG_OF_LIBFDT)
#include <fdt.h>
#include <libfdt.h>
#include <fdt_support.h>
#endif

#ifdef CONFIG_LZMA
#include <lzma/LzmaTypes.h>
#include <lzma/LzmaDec.h>
#include <lzma/LzmaTools.h>
#endif /* CONFIG_LZMA */

#ifdef CONFIG_LZO
#include <linux/lzo.h>
#endif /* CONFIG_LZO */

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_SYS_BOOTM_LEN
#define CONFIG_SYS_BOOTM_LEN    0x800000    /* use 8MByte as default max gunzip size */
#endif

#ifdef CONFIG_BZIP2
extern void bz_internal_error(int);
#endif

#if defined(CONFIG_CMD_IMI)
static int image_info(unsigned long addr);
#endif

#if defined(CONFIG_CMD_IMLS)
#include <flash.h>
#include <mtd/cfi_flash.h>
extern flash_info_t flash_info[]; /* info for FLASH chips */
static int do_imls(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif

#ifdef CONFIG_SILENT_CONSOLE
static void fixup_silent_linux(void);
#endif

static image_header_t *image_get_kernel(ulong img_addr, int verify);
#if defined(CONFIG_FIT)
static int fit_check_kernel(const void *fit, int os_noffset, int verify);
#endif

static void *boot_get_kernel(cmd_tbl_t *cmdtp, int flag, int argc,
        char * const argv[], bootm_headers_t *images,
        ulong *os_data, ulong *os_len);

/*
 *  Continue booting an OS image; caller already has:
 *  - copied image header to global variable `header'
 *  - checked header magic number, checksums (both header & image),
 *  - verified image architecture (PPC) and type (KERNEL or MULTI),
 *  - loaded (first part of) image to header load address,
 *  - disabled interrupts.
 */
typedef int boot_os_fn(int flag, int argc, char * const argv[],
        bootm_headers_t *images); /* pointers to os/initrd/fdt */

#ifdef CONFIG_BOOTM_LINUX
extern boot_os_fn do_bootm_linux;
#endif
#ifdef CONFIG_BOOTM_NETBSD
static boot_os_fn do_bootm_netbsd;
#endif
#if defined(CONFIG_LYNXKDI)
static boot_os_fn do_bootm_lynxkdi;
extern void lynxkdi_boot(image_header_t *);
#endif

#ifdef CONFIG_BOOTM_RTEMS
static boot_os_fn do_bootm_rtems;
#endif
#if defined(CONFIG_BOOTM_OSE)
static boot_os_fn do_bootm_ose;
#endif
#if defined(CONFIG_CMD_ELF)
static boot_os_fn do_bootm_vxworks;
static boot_os_fn do_bootm_qnxelf;
int do_bootvx(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_bootelf(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif
#if defined(CONFIG_INTEGRITY)
static boot_os_fn do_bootm_integrity;
#endif

static boot_os_fn *boot_os[] = {
#ifdef CONFIG_BOOTM_LINUX
    [IH_OS_LINUX] = do_bootm_linux,
#endif
#ifdef CONFIG_BOOTM_NETBSD
    [IH_OS_NETBSD] = do_bootm_netbsd,
#endif
#ifdef CONFIG_LYNXKDI
    [IH_OS_LYNXOS] = do_bootm_lynxkdi,
#endif
#ifdef CONFIG_BOOTM_RTEMS
    [IH_OS_RTEMS] = do_bootm_rtems,
#endif
#if defined(CONFIG_BOOTM_OSE)
    [IH_OS_OSE] = do_bootm_ose,
#endif
#if defined(CONFIG_CMD_ELF)
    [IH_OS_VXWORKS] = do_bootm_vxworks,
    [IH_OS_QNX] = do_bootm_qnxelf,
#endif
#ifdef CONFIG_INTEGRITY
    [IH_OS_INTEGRITY] = do_bootm_integrity,
#endif
};

bootm_headers_t images;        /* pointers to os/initrd/fdt images */

/* Allow for arch specific config before we boot */
void __arch_preboot_os(void)
{
    /* please define platform specific arch_preboot_os() */
}
void arch_preboot_os(void) __attribute__((weak, alias("__arch_preboot_os")));

#define IH_INITRD_ARCH IH_ARCH_DEFAULT

static void bootm_start_lmb(void)
{
#ifdef CONFIG_LMB
    ulong        mem_start;
    phys_size_t    mem_size;

    lmb_init(&images.lmb);

    mem_start = getenv_bootm_low();
    mem_size = getenv_bootm_size();

    lmb_add(&images.lmb, (phys_addr_t)mem_start, mem_size);

    arch_lmb_reserve(&images.lmb);
    board_lmb_reserve(&images.lmb);
#else
# define lmb_reserve(lmb, base, size)
#endif
}

static int bootm_start(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    void        *os_hdr;
    int        ret;

    memset((void *)&images, 0, sizeof(images));
    images.verify = getenv_yesno("verify");

    bootm_start_lmb();

    /* get kernel image header, start address and length */
    os_hdr = boot_get_kernel(cmdtp, flag, argc, argv,
            &images, &images.os.image_start, &images.os.image_len);
    if (images.os.image_len == 0) {
        puts("ERROR: can't get kernel image!\n");
        return 1;
    }

    /* get image parameters */
    switch (genimg_get_format(os_hdr)) {
        case IMAGE_FORMAT_LEGACY:
            images.os.type = image_get_type(os_hdr);
            images.os.comp = image_get_comp(os_hdr);
            images.os.os = image_get_os(os_hdr);

            images.os.end = image_get_image_end(os_hdr);
            images.os.load = image_get_load(os_hdr);
            break;
#if defined(CONFIG_FIT)
        case IMAGE_FORMAT_FIT:
            if (fit_image_get_type(images.fit_hdr_os,
                        images.fit_noffset_os, &images.os.type)) {
                puts("Can't get image type!\n");
                show_boot_progress(-109);
                return 1;
            }

            if (fit_image_get_comp(images.fit_hdr_os,
                        images.fit_noffset_os, &images.os.comp)) {
                puts("Can't get image compression!\n");
                show_boot_progress(-110);
                return 1;
            }

            if (fit_image_get_os(images.fit_hdr_os,
                        images.fit_noffset_os, &images.os.os)) {
                puts("Can't get image OS!\n");
                show_boot_progress(-111);
                return 1;
            }

            images.os.end = fit_get_end(images.fit_hdr_os);

            if (fit_image_get_load(images.fit_hdr_os, images.fit_noffset_os,
                        &images.os.load)) {
                puts("Can't get image load address!\n");
                show_boot_progress(-112);
                return 1;
            }
            break;
#endif
        default:
            puts("ERROR: unknown image format type!\n");
            return 1;
    }

    /* find kernel entry point */
    if (images.legacy_hdr_valid) {
        images.ep = image_get_ep(&images.legacy_hdr_os_copy);
#if defined(CONFIG_FIT)
    } else if (images.fit_uname_os) {
        ret = fit_image_get_entry(images.fit_hdr_os,
                images.fit_noffset_os, &images.ep);
        if (ret) {
            puts("Can't get entry point property!\n");
            return 1;
        }
#endif
    } else {
        puts("Could not find kernel entry point!\n");
        return 1;
    }

    if (images.os.type == IH_TYPE_KERNEL_NOLOAD) {
        images.os.load = images.os.image_start;
        images.ep += images.os.load;
    }

    if (((images.os.type == IH_TYPE_KERNEL) ||
                (images.os.type == IH_TYPE_KERNEL_NOLOAD) ||
                (images.os.type == IH_TYPE_MULTI)) &&
            (images.os.os == IH_OS_LINUX)) {
        /* find ramdisk */
        ret = boot_get_ramdisk(argc, argv, &images, IH_INITRD_ARCH,
                &images.rd_start, &images.rd_end);
        if (ret) {
            puts("Ramdisk image is corrupt or invalid\n");
            return 1;
        }

#if defined(CONFIG_OF_LIBFDT)
        /* find flattened device tree */
        ret = boot_get_fdt(flag, argc, argv, &images,
                &images.ft_addr, &images.ft_len);
        if (ret) {
            puts("Could not find a valid device tree\n");
            return 1;
        }

        set_working_fdt_addr(images.ft_addr);
#endif
    }

    images.os.start = (ulong)os_hdr;
    images.state = BOOTM_STATE_START;

    return 0;
}

#define BOOTM_ERR_RESET        -1
#define BOOTM_ERR_OVERLAP    -2
#define BOOTM_ERR_UNIMPLEMENTED    -3
static int bootm_load_os(image_info_t os, ulong *load_end, int boot_progress)
{
    uint8_t comp = os.comp;
    ulong load = os.load;
    ulong blob_start = os.start;
    ulong blob_end = os.end;
    ulong image_start = os.image_start;
    ulong image_len = os.image_len;
    __maybe_unused uint unc_len = CONFIG_SYS_BOOTM_LEN;
    int no_overlap = 0;
#if defined(CONFIG_LZMA) || defined(CONFIG_LZO)
    int ret;
#endif /* defined(CONFIG_LZMA) || defined(CONFIG_LZO) */

    const char *type_name = genimg_get_type_name(os.type);

    switch (comp) {
        case IH_COMP_NONE:
            if (load == blob_start || load == image_start) {
                printf("   XIP %s ... ", type_name);
                no_overlap = 1;
            } else {
                printf("   Loading %s ... ", type_name);
                memmove_wd((void *)load, (void *)image_start,
                        image_len, CHUNKSZ);
            }
            *load_end = load + image_len;
            puts("OK\n");
            break;
#ifdef CONFIG_GZIP
        case IH_COMP_GZIP:
            printf("   Uncompressing %s ... ", type_name);
            if (gunzip((void *)load, unc_len,
                        (uchar *)image_start, &image_len) != 0) {
                puts("GUNZIP: uncompress, out-of-mem or overwrite "
                        "error - must RESET board to recover\n");
                if (boot_progress)
                    show_boot_progress(-6);
                return BOOTM_ERR_RESET;
            }

            *load_end = load + image_len;
            break;
#endif /* CONFIG_GZIP */
#ifdef CONFIG_BZIP2
        case IH_COMP_BZIP2:
            printf("   Uncompressing %s ... ", type_name);
            /*
             * If we've got less than 4 MB of malloc() space,
             * use slower decompression algorithm which requires
             * at most 2300 KB of memory.
             */
            int i = BZ2_bzBuffToBuffDecompress((char *)load,
                    &unc_len, (char *)image_start, image_len,
                    CONFIG_SYS_MALLOC_LEN < (4096 * 1024), 0);
            if (i != BZ_OK) {
                printf("BUNZIP2: uncompress or overwrite error %d "
                        "- must RESET board to recover\n", i);
                if (boot_progress)
                    show_boot_progress(-6);
                return BOOTM_ERR_RESET;
            }

            *load_end = load + unc_len;
            break;
#endif /* CONFIG_BZIP2 */
#ifdef CONFIG_LZMA
        case IH_COMP_LZMA: {
                               SizeT lzma_len = unc_len;

#ifndef CAMEO_SIMPLE_DISPLAY
                               printf("   Uncompressing %s ... ", type_name);
#endif

                               ret = lzmaBuffToBuffDecompress(
                                       (unsigned char *)load, &lzma_len,
                                       (unsigned char *)image_start, image_len);
                               unc_len = lzma_len;
                               if (ret != SZ_OK) {
                                   printf("LZMA: uncompress or overwrite error %d "
                                           "- must RESET board to recover\n", ret);
                                   show_boot_progress(-6);
                                   return BOOTM_ERR_RESET;
                               }
                               *load_end = load + unc_len;
                               break;
                           }
#endif /* CONFIG_LZMA */
#ifdef CONFIG_LZO
        case IH_COMP_LZO:
                           printf("   Uncompressing %s ... ", type_name);

                           ret = lzop_decompress((const unsigned char *)image_start,
                                   image_len, (unsigned char *)load,
                                   &unc_len);
                           if (ret != LZO_E_OK) {
                               printf("LZO: uncompress or overwrite error %d "
                                       "- must RESET board to recover\n", ret);
                               if (boot_progress)
                                   show_boot_progress(-6);
                               return BOOTM_ERR_RESET;
                           }

                           *load_end = load + unc_len;
                           break;
#endif /* CONFIG_LZO */
        default:
                           printf("Unimplemented compression type %d\n", comp);
                           return BOOTM_ERR_UNIMPLEMENTED;
    }

    flush_cache(load, (*load_end - load) * sizeof(ulong));
#ifdef CAMEO_SIMPLE_DISPLAY
    printf("\nUncompressing %s .............................. 100 %%\n", type_name);
#else 
    puts("OK\n");
#endif
    debug("   kernel loaded at 0x%08lx, end = 0x%08lx\n", load, *load_end);
    if (boot_progress)
        show_boot_progress(7);

    if (!no_overlap && (load < blob_end) && (*load_end > blob_start)) {
        debug("images.os.start = 0x%lX, images.os.end = 0x%lx\n",
                blob_start, blob_end);
        debug("images.os.load = 0x%lx, load_end = 0x%lx\n", load,
                *load_end);

        return BOOTM_ERR_OVERLAP;
    }

    return 0;
}

static int bootm_start_standalone(ulong iflag, int argc, char * const argv[])
{
    char  *s;
    int   (*appl)(int, char * const []);

    /* Don't start if "autostart" is set to "no" */
    if (((s = getenv("autostart")) != NULL) && (strcmp(s, "no") == 0)) {
        char buf[32];
        sprintf(buf, "%lX", images.os.image_len);
        setenv("filesize", buf);
        return 0;
    }
    appl = (int (*)(int, char * const []))(ulong)ntohl(images.ep);
    (*appl)(argc-1, &argv[1]);
    return 0;
}

/* we overload the cmd field with our state machine info instead of a
 * function pointer */
static cmd_tbl_t cmd_bootm_sub[] = {
    U_BOOT_CMD_MKENT(start, 0, 1, (void *)BOOTM_STATE_START, "", ""),
    U_BOOT_CMD_MKENT(loados, 0, 1, (void *)BOOTM_STATE_LOADOS, "", ""),
#ifdef CONFIG_SYS_BOOT_RAMDISK_HIGH
    U_BOOT_CMD_MKENT(ramdisk, 0, 1, (void *)BOOTM_STATE_RAMDISK, "", ""),
#endif
#ifdef CONFIG_OF_LIBFDT
    U_BOOT_CMD_MKENT(fdt, 0, 1, (void *)BOOTM_STATE_FDT, "", ""),
#endif
    U_BOOT_CMD_MKENT(cmdline, 0, 1, (void *)BOOTM_STATE_OS_CMDLINE, "", ""),
    U_BOOT_CMD_MKENT(bdt, 0, 1, (void *)BOOTM_STATE_OS_BD_T, "", ""),
    U_BOOT_CMD_MKENT(prep, 0, 1, (void *)BOOTM_STATE_OS_PREP, "", ""),
    U_BOOT_CMD_MKENT(go, 0, 1, (void *)BOOTM_STATE_OS_GO, "", ""),
};

int do_bootm_subcommand(cmd_tbl_t *cmdtp, int flag, int argc,
        char * const argv[])
{
    int ret = 0;
    long state;
    cmd_tbl_t *c;
    boot_os_fn *boot_fn;

    c = find_cmd_tbl(argv[1], &cmd_bootm_sub[0], ARRAY_SIZE(cmd_bootm_sub));

    if (c) {
        state = (long)c->cmd;

        /* treat start special since it resets the state machine */
        if (state == BOOTM_STATE_START) {
            argc--;
            argv++;
            return bootm_start(cmdtp, flag, argc, argv);
        }
    } else {
        /* Unrecognized command */
        return cmd_usage(cmdtp);
    }

    if (images.state >= state) {
        printf("Trying to execute a command out of order\n");
        return cmd_usage(cmdtp);
    }

    images.state |= state;
    boot_fn = boot_os[images.os.os];

    switch (state) {
        ulong load_end;
        case BOOTM_STATE_START:
        /* should never occur */
        break;
        case BOOTM_STATE_LOADOS:
        ret = bootm_load_os(images.os, &load_end, 0);
        if (ret)
            return ret;

        lmb_reserve(&images.lmb, images.os.load,
                (load_end - images.os.load));
        break;
#ifdef CONFIG_SYS_BOOT_RAMDISK_HIGH
        case BOOTM_STATE_RAMDISK:
        {
            ulong rd_len = images.rd_end - images.rd_start;
            char str[17];

            ret = boot_ramdisk_high(&images.lmb, images.rd_start,
                    rd_len, &images.initrd_start, &images.initrd_end);
            if (ret)
                return ret;

            sprintf(str, "%lx", images.initrd_start);
            setenv("initrd_start", str);
            sprintf(str, "%lx", images.initrd_end);
            setenv("initrd_end", str);
        }
        break;
#endif
#if defined(CONFIG_OF_LIBFDT)
        case BOOTM_STATE_FDT:
        {
            boot_fdt_add_mem_rsv_regions(&images.lmb,
                    images.ft_addr);
            ret = boot_relocate_fdt(&images.lmb,
                    &images.ft_addr, &images.ft_len);
            break;
        }
#endif
        case BOOTM_STATE_OS_CMDLINE:
        ret = boot_fn(BOOTM_STATE_OS_CMDLINE, argc, argv, &images);
        if (ret)
            printf("cmdline subcommand not supported\n");
        break;
        case BOOTM_STATE_OS_BD_T:
        ret = boot_fn(BOOTM_STATE_OS_BD_T, argc, argv, &images);
        if (ret)
            printf("bdt subcommand not supported\n");
        break;
        case BOOTM_STATE_OS_PREP:
        ret = boot_fn(BOOTM_STATE_OS_PREP, argc, argv, &images);
        if (ret)
            printf("prep subcommand not supported\n");
        break;
        case BOOTM_STATE_OS_GO:
        disable_interrupts();
        arch_preboot_os();
        boot_fn(BOOTM_STATE_OS_GO, argc, argv, &images);
        break;
    }

    return ret;
}

/*******************************************************************/
/* bootm - boot application image from image in memory */
/*******************************************************************/

int do_bootm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    ulong        iflag;
    ulong        load_end = 0;
    int        ret;
    boot_os_fn    *boot_fn;
#ifdef CONFIG_NEEDS_MANUAL_RELOC
    static int relocated = 0;

    /* relocate boot function table */
    if (!relocated) {
        int i;
        for (i = 0; i < ARRAY_SIZE(boot_os); i++)
            if (boot_os[i] != NULL)
                boot_os[i] += gd->reloc_off;
        relocated = 1;
    }
#endif

    /* determine if we have a sub command */
    if (argc > 1) {
        char *endp;

        simple_strtoul(argv[1], &endp, 16);
        /* endp pointing to NULL means that argv[1] was just a
         * valid number, pass it along to the normal bootm processing
         *
         * If endp is ':' or '#' assume a FIT identifier so pass
         * along for normal processing.
         *
         * Right now we assume the first arg should never be '-'
         */
        if ((*endp != 0) && (*endp != ':') && (*endp != '#'))
            return do_bootm_subcommand(cmdtp, flag, argc, argv);
    }

    if (bootm_start(cmdtp, flag, argc, argv))
        return 1;

    /*
     * We have reached the point of no return: we are going to
     * overwrite all exception vector code, so we cannot easily
     * recover from any failures any more...
     */
    iflag = disable_interrupts();

#if defined(CONFIG_CMD_USB)
    /*
     * turn off USB to prevent the host controller from writing to the
     * SDRAM while Linux is booting. This could happen (at least for OHCI
     * controller), because the HCCA (Host Controller Communication Area)
     * lies within the SDRAM and the host controller writes continously to
     * this area (as busmaster!). The HccaFrameNumber is for example
     * updated every 1 ms within the HCCA structure in SDRAM! For more
     * details see the OpenHCI specification.
     */
    usb_stop();
#endif

    ret = bootm_load_os(images.os, &load_end, 1);

    if (ret < 0) {
        if (ret == BOOTM_ERR_RESET)
            do_reset(cmdtp, flag, argc, argv);
        if (ret == BOOTM_ERR_OVERLAP) {
            if (images.legacy_hdr_valid) {
                image_header_t *hdr;
                hdr = &images.legacy_hdr_os_copy;
                if (image_get_type(hdr) == IH_TYPE_MULTI)
                    puts("WARNING: legacy format multi "
                            "component image "
                            "overwritten\n");
            } else {
                puts("ERROR: new format image overwritten - "
                        "must RESET the board to recover\n");
                show_boot_progress(-113);
                do_reset(cmdtp, flag, argc, argv);
            }
        }
        if (ret == BOOTM_ERR_UNIMPLEMENTED) {
            if (iflag)
                enable_interrupts();
            show_boot_progress(-7);
            return 1;
        }
    }

    lmb_reserve(&images.lmb, images.os.load, (load_end - images.os.load));

    if (images.os.type == IH_TYPE_STANDALONE) {
        if (iflag)
            enable_interrupts();
        /* This may return when 'autostart' is 'no' */
        bootm_start_standalone(iflag, argc, argv);
        return 0;
    }

    show_boot_progress(8);

#ifdef CONFIG_SILENT_CONSOLE
    if (images.os.os == IH_OS_LINUX)
        fixup_silent_linux();
#endif

    boot_fn = boot_os[images.os.os];

    if (boot_fn == NULL) {
        if (iflag)
            enable_interrupts();
        printf("ERROR: booting os '%s' (%d) is not supported\n",
                genimg_get_os_name(images.os.os), images.os.os);
        show_boot_progress(-8);
        return 1;
    }

    arch_preboot_os();

    boot_fn(0, argc, argv, &images);

    show_boot_progress(-9);
#ifdef DEBUG
    puts("\n## Control returned to monitor - resetting...\n");
#endif
    do_reset(cmdtp, flag, argc, argv);

    return 1;
}

int bootm_maybe_autostart(cmd_tbl_t *cmdtp, const char *cmd)
{
    const char *ep = getenv("autostart");

    if (ep && !strcmp(ep, "yes")) {
        char *local_args[2];
        local_args[0] = (char *)cmd;
        local_args[1] = NULL;
        printf("Automatic boot of image at addr 0x%08lX ...\n", load_addr);
        return do_bootm(cmdtp, 0, 1, local_args);
    }

    return 0;
}

/**
 * image_get_kernel - verify legacy format kernel image
 * @img_addr: in RAM address of the legacy format image to be verified
 * @verify: data CRC verification flag
 *
 * image_get_kernel() verifies legacy image integrity and returns pointer to
 * legacy image header if image verification was completed successfully.
 *
 * returns:
 *     pointer to a legacy image header if valid image was found
 *     otherwise return NULL
 */
static image_header_t *image_get_kernel(ulong img_addr, int verify)
{
    image_header_t *hdr = (image_header_t *)img_addr;

    if (!image_check_magic(hdr)) {
        puts("Bad Magic Number\n");
        show_boot_progress(-1);
        return NULL;
    }
    show_boot_progress(2);

    if (!image_check_hcrc(hdr)) {
        puts("Bad Header Checksum\n");
        show_boot_progress(-2);
        return NULL;
    }

    show_boot_progress(3);
    image_print_contents(hdr);

    if (verify) {
#ifndef CAMEO_SIMPLE_DISPLAY
        puts("   Verifying Checksum ... ");
#endif
        if (!image_check_dcrc(hdr)) {
            CAMEO_SIMPLE_DISPLAY_PRINTF(("Bad Data CRC\n"));
            show_boot_progress(-3);
            return NULL;
        }
#ifndef CAMEO_SIMPLE_DISPLAY
        puts("OK\n");
#endif
    }
    show_boot_progress(4);

    if (!image_check_target_arch(hdr)) {
        printf("Unsupported Architecture 0x%x\n", image_get_arch(hdr));
        show_boot_progress(-4);
        return NULL;
    }
    return hdr;
}

/**
 * fit_check_kernel - verify FIT format kernel subimage
 * @fit_hdr: pointer to the FIT image header
 * os_noffset: kernel subimage node offset within FIT image
 * @verify: data CRC verification flag
 *
 * fit_check_kernel() verifies integrity of the kernel subimage and from
 * specified FIT image.
 *
 * returns:
 *     1, on success
 *     0, on failure
 */
#if defined(CONFIG_FIT)
static int fit_check_kernel(const void *fit, int os_noffset, int verify)
{
    fit_image_print(fit, os_noffset, "   ");

    if (verify) {
        puts("   Verifying Hash Integrity ... ");
        if (!fit_image_check_hashes(fit, os_noffset)) {
            puts("Bad Data Hash\n");
            show_boot_progress(-104);
            return 0;
        }
        puts("OK\n");
    }
    show_boot_progress(105);

    if (!fit_image_check_target_arch(fit, os_noffset)) {
        puts("Unsupported Architecture\n");
        show_boot_progress(-105);
        return 0;
    }

    show_boot_progress(106);
    if (!fit_image_check_type(fit, os_noffset, IH_TYPE_KERNEL) &&
            !fit_image_check_type(fit, os_noffset, IH_TYPE_KERNEL_NOLOAD)) {
        puts("Not a kernel image\n");
        show_boot_progress(-106);
        return 0;
    }

    show_boot_progress(107);
    return 1;
}
#endif /* CONFIG_FIT */

/**
 * boot_get_kernel - find kernel image
 * @os_data: pointer to a ulong variable, will hold os data start address
 * @os_len: pointer to a ulong variable, will hold os data length
 *
 * boot_get_kernel() tries to find a kernel image, verifies its integrity
 * and locates kernel data.
 *
 * returns:
 *     pointer to image header if valid image was found, plus kernel start
 *     address and length, otherwise NULL
 */
static void *boot_get_kernel(cmd_tbl_t *cmdtp, int flag, int argc,
        char * const argv[], bootm_headers_t *images, ulong *os_data,
        ulong *os_len)
{
    image_header_t    *hdr;
    ulong        img_addr;
#if defined(CONFIG_FIT)
    void        *fit_hdr;
    const char    *fit_uname_config = NULL;
    const char    *fit_uname_kernel = NULL;
    const void    *data;
    size_t        len;
    int        cfg_noffset;
    int        os_noffset;
#endif

    /* find out kernel image address */
    if (argc < 2) {
        img_addr = load_addr;
        debug("*  kernel: default image load address = 0x%08lx\n",
                load_addr);
#if defined(CONFIG_FIT)
    } else if (fit_parse_conf(argv[1], load_addr, &img_addr,
                &fit_uname_config)) {
        debug("*  kernel: config '%s' from image at 0x%08lx\n",
                fit_uname_config, img_addr);
    } else if (fit_parse_subimage(argv[1], load_addr, &img_addr,
                &fit_uname_kernel)) {
        debug("*  kernel: subimage '%s' from image at 0x%08lx\n",
                fit_uname_kernel, img_addr);
#endif
    } else {
        img_addr = simple_strtoul(argv[1], NULL, 16);
        debug("*  kernel: cmdline image address = 0x%08lx\n", img_addr);
    }

    show_boot_progress(1);

    /* copy from dataflash if needed */
    img_addr = genimg_get_image(img_addr);

    /* check image type, for FIT images get FIT kernel node */
    *os_data = *os_len = 0;
    switch (genimg_get_format((void *)img_addr)) {
        case IMAGE_FORMAT_LEGACY:
            CAMEO_SIMPLE_DISPLAY_PRINTF(("## Booting kernel from Legacy Image at %08lx ...\n",
                    img_addr));
            hdr = image_get_kernel(img_addr, images->verify);
            if (!hdr)
                return NULL;
            show_boot_progress(5);

            /* get os_data and os_len */
            switch (image_get_type(hdr)) {
                case IH_TYPE_KERNEL:
                case IH_TYPE_KERNEL_NOLOAD:
                    *os_data = image_get_data(hdr);
                    *os_len = image_get_data_size(hdr);
                    break;
                case IH_TYPE_MULTI:
                    image_multi_getimg(hdr, 0, os_data, os_len);
                    break;
                case IH_TYPE_STANDALONE:
                    *os_data = image_get_data(hdr);
                    *os_len = image_get_data_size(hdr);
                    break;
                default:
                    printf("Wrong Image Type for %s command\n",
                            cmdtp->name);
                    show_boot_progress(-5);
                    return NULL;
            }

            /*
             * copy image header to allow for image overwrites during
             * kernel decompression.
             */
            memmove(&images->legacy_hdr_os_copy, hdr,
                    sizeof(image_header_t));

            /* save pointer to image header */
            images->legacy_hdr_os = hdr;

            images->legacy_hdr_valid = 1;
            show_boot_progress(6);
            break;
#if defined(CONFIG_FIT)
        case IMAGE_FORMAT_FIT:
            fit_hdr = (void *)img_addr;
            printf("## Booting kernel from FIT Image at %08lx ...\n",
                    img_addr);

            if (!fit_check_format(fit_hdr)) {
                puts("Bad FIT kernel image format!\n");
                show_boot_progress(-100);
                return NULL;
            }
            show_boot_progress(100);

            if (!fit_uname_kernel) {
                /*
                 * no kernel image node unit name, try to get config
                 * node first. If config unit node name is NULL
                 * fit_conf_get_node() will try to find default config
                 * node
                 */
                show_boot_progress(101);
                cfg_noffset = fit_conf_get_node(fit_hdr,
                        fit_uname_config);
                if (cfg_noffset < 0) {
                    show_boot_progress(-101);
                    return NULL;
                }
                /* save configuration uname provided in the first
                 * bootm argument
                 */
                images->fit_uname_cfg = fdt_get_name(fit_hdr,
                        cfg_noffset,
                        NULL);
                printf("   Using '%s' configuration\n",
                        images->fit_uname_cfg);
                show_boot_progress(103);

                os_noffset = fit_conf_get_kernel_node(fit_hdr,
                        cfg_noffset);
                fit_uname_kernel = fit_get_name(fit_hdr, os_noffset,
                        NULL);
            } else {
                /* get kernel component image node offset */
                show_boot_progress(102);
                os_noffset = fit_image_get_node(fit_hdr,
                        fit_uname_kernel);
            }
            if (os_noffset < 0) {
                show_boot_progress(-103);
                return NULL;
            }

            printf("   Trying '%s' kernel subimage\n", fit_uname_kernel);

            show_boot_progress(104);
            if (!fit_check_kernel(fit_hdr, os_noffset, images->verify))
                return NULL;

            /* get kernel image data address and length */
            if (fit_image_get_data(fit_hdr, os_noffset, &data, &len)) {
                puts("Could not find kernel subimage data!\n");
                show_boot_progress(-107);
                return NULL;
            }
            show_boot_progress(108);

            *os_len = len;
            *os_data = (ulong)data;
            images->fit_hdr_os = fit_hdr;
            images->fit_uname_os = fit_uname_kernel;
            images->fit_noffset_os = os_noffset;
            break;
#endif
        default:
            printf("Wrong Image Format for %s command\n", cmdtp->name);
            show_boot_progress(-108);
            return NULL;
    }

    debug("   kernel data at 0x%08lx, len = 0x%08lx (%ld)\n",
            *os_data, *os_len, *os_len);

    return (void *)img_addr;
}

U_BOOT_CMD(
        bootm,    CONFIG_SYS_MAXARGS,    1,    do_bootm,
        "boot application image from memory",
        "[addr [arg ...]]\n    - boot application image stored in memory\n"
        "\tpassing arguments 'arg ...'; when booting a Linux kernel,\n"
        "\t'arg' can be the address of an initrd image\n"
#if defined(CONFIG_OF_LIBFDT)
        "\tWhen booting a Linux kernel which requires a flat device-tree\n"
        "\ta third argument is required which is the address of the\n"
        "\tdevice-tree blob. To boot that kernel without an initrd image,\n"
        "\tuse a '-' for the second argument. If you do not pass a third\n"
        "\ta bd_info struct will be passed instead\n"
#endif
#if defined(CONFIG_FIT)
        "\t\nFor the new multi component uImage format (FIT) addresses\n"
        "\tmust be extened to include component or configuration unit name:\n"
        "\taddr:<subimg_uname> - direct component image specification\n"
        "\taddr#<conf_uname>   - configuration specification\n"
        "\tUse iminfo command to get the list of existing component\n"
        "\timages and configurations.\n"
#endif
"\nSub-commands to do part of the bootm sequence.  The sub-commands "
"must be\n"
"issued in the order below (it's ok to not issue all sub-commands):\n"
"\tstart [addr [arg ...]]\n"
"\tloados  - load OS image\n"
#if defined(CONFIG_PPC) || defined(CONFIG_M68K) || defined(CONFIG_SPARC)
"\tramdisk - relocate initrd, set env initrd_start/initrd_end\n"
#endif
#if defined(CONFIG_OF_LIBFDT)
"\tfdt     - relocate flat device tree\n"
#endif
"\tcmdline - OS specific command line processing/setup\n"
"\tbdt     - OS specific bd_t processing\n"
"\tprep    - OS specific prep before relocation or go\n"
"\tgo      - start OS"
);

/*******************************************************************/
/* bootd - boot default image */
/*******************************************************************/
#if defined(CONFIG_CMD_BOOTD)
int do_bootd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int rcode = 0;

#ifndef CONFIG_SYS_HUSH_PARSER
    if (run_command(getenv("bootcmd"), flag) < 0)
        rcode = 1;
#else
    if (parse_string_outer(getenv("bootcmd"),
                FLAG_PARSE_SEMICOLON | FLAG_EXIT_FROM_LOOP) != 0)
        rcode = 1;
#endif
    return rcode;
}

U_BOOT_CMD(
        boot,    1,    1,    do_bootd,
        "boot default, i.e., run 'bootcmd'",
        ""
        );

/* keep old command name "bootd" for backward compatibility */
U_BOOT_CMD(
        bootd, 1,    1,    do_bootd,
        "boot default, i.e., run 'bootcmd'",
        ""
        );

#endif


/*******************************************************************/
/* iminfo - print header info for a requested image */
/*******************************************************************/
#if defined(CONFIG_CMD_IMI)
int do_iminfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int    arg;
    ulong    addr;
    int    rcode = 0;

    if (argc < 2) {
        return image_info(load_addr);
    }

    for (arg = 1; arg < argc; ++arg) {
        addr = simple_strtoul(argv[arg], NULL, 16);
        if (image_info(addr) != 0)
            rcode = 1;
    }
    return rcode;
}

static int image_info(ulong addr)
{
    void *hdr = (void *)addr;

    printf("\n## Checking Image at %08lx ...\n", addr);

    switch (genimg_get_format(hdr)) {
        case IMAGE_FORMAT_LEGACY:
            puts("   Legacy image found\n");
            if (!image_check_magic(hdr)) {
                puts("   Bad Magic Number\n");
                return 1;
            }

#ifdef  IH_MAGIC_SHOW
            printf("   Magic Number: %08X\n", image_get_magic(hdr));
#endif

            if (!image_check_hcrc(hdr)) {
                puts("   Bad Header Checksum\n");
                return 1;
            }

            image_print_contents(hdr);

            puts("   Verifying Checksum ... ");
            if (!image_check_dcrc(hdr)) {
                puts("   Bad Data CRC\n");
                return 1;
            }
            puts("OK\n");
            return 0;
#if defined(CONFIG_FIT)
        case IMAGE_FORMAT_FIT:
            puts("   FIT image found\n");

            if (!fit_check_format(hdr)) {
                puts("Bad FIT image format!\n");
                return 1;
            }

            fit_print_contents(hdr);

            if (!fit_all_image_check_hashes(hdr)) {
                puts("Bad hash in FIT image!\n");
                return 1;
            }

            return 0;
#endif
        default:
            puts("Unknown image format!\n");
            break;
    }

    return 1;
}

U_BOOT_CMD(
        iminfo,    CONFIG_SYS_MAXARGS,    1,    do_iminfo,
        "print header information for application image",
        "addr [addr ...]\n"
        "    - print header information for application image starting at\n"
        "      address 'addr' in memory; this includes verification of the\n"
        "      image contents (magic number, header and payload checksums)"
        );
#endif


/*******************************************************************/
/* imls - list all images found in flash */
/*******************************************************************/
#if defined(CONFIG_CMD_IMLS)
int do_imls(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    flash_info_t *info;
    int i, j;
    void *hdr;

    for (i = 0, info = &flash_info[0];
            i < CONFIG_SYS_MAX_FLASH_BANKS; ++i, ++info) {

        if (info->flash_id == FLASH_UNKNOWN)
            goto next_bank;
        for (j = 0; j < info->sector_count; ++j) {

            hdr = (void *)info->start[j];
            if (!hdr)
                goto next_sector;

            switch (genimg_get_format(hdr)) {
                case IMAGE_FORMAT_LEGACY:
                    if (!image_check_hcrc(hdr))
                        goto next_sector;

                    printf("Legacy Image at %08lX:\n", (ulong)hdr);
                    image_print_contents(hdr);

                    puts("   Verifying Checksum ... ");
                    if (!image_check_dcrc(hdr)) {
                        puts("Bad Data CRC\n");
                    } else {
                        puts("OK\n");
                    }
                    break;
#if defined(CONFIG_FIT)
                case IMAGE_FORMAT_FIT:
                    if (!fit_check_format(hdr))
                        goto next_sector;

                    printf("FIT Image at %08lX:\n", (ulong)hdr);
                    fit_print_contents(hdr);
                    break;
#endif
                default:
                    goto next_sector;
            }

next_sector:        ;
        }
next_bank:    ;
    }

    return (0);
}

U_BOOT_CMD(
        imls,    1,        1,    do_imls,
        "list all images found in flash",
        "\n"
        "    - Prints information about all images found at sector\n"
        "      boundaries in flash."
        );
#endif

/*******************************************************************/
/* helper routines */
/*******************************************************************/
#ifdef CONFIG_SILENT_CONSOLE
static void fixup_silent_linux(void)
{
    char buf[256], *start, *end;
    char *cmdline = getenv("bootargs");

    /* Only fix cmdline when requested */
    if (!(gd->flags & GD_FLG_SILENT))
        return;

    debug("before silent fix-up: %s\n", cmdline);
    if (cmdline) {
        start = strstr(cmdline, "console=");
        if (start) {
            end = strchr(start, ' ');
            strncpy(buf, cmdline, (start - cmdline + 8));
            if (end)
                strcpy(buf + (start - cmdline + 8), end);
            else
                buf[start - cmdline + 8] = '\0';
        } else {
            strcpy(buf, cmdline);
            strcat(buf, " console=");
        }
    } else {
        strcpy(buf, "console=");
    }

    setenv("bootargs", buf);
    debug("after silent fix-up: %s\n", buf);
}
#endif /* CONFIG_SILENT_CONSOLE */


/*******************************************************************/
/* OS booting routines */
/*******************************************************************/

#ifdef CONFIG_BOOTM_NETBSD
static int do_bootm_netbsd(int flag, int argc, char * const argv[],
        bootm_headers_t *images)
{
    void (*loader)(bd_t *, image_header_t *, char *, char *);
    image_header_t *os_hdr, *hdr;
    ulong kernel_data, kernel_len;
    char *consdev;
    char *cmdline;

    if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
        return 1;

#if defined(CONFIG_FIT)
    if (!images->legacy_hdr_valid) {
        fit_unsupported_reset("NetBSD");
        return 1;
    }
#endif
    hdr = images->legacy_hdr_os;

    /*
     * Booting a (NetBSD) kernel image
     *
     * This process is pretty similar to a standalone application:
     * The (first part of an multi-) image must be a stage-2 loader,
     * which in turn is responsible for loading & invoking the actual
     * kernel.  The only differences are the parameters being passed:
     * besides the board info strucure, the loader expects a command
     * line, the name of the console device, and (optionally) the
     * address of the original image header.
     */
    os_hdr = NULL;
    if (image_check_type(&images->legacy_hdr_os_copy, IH_TYPE_MULTI)) {
        image_multi_getimg(hdr, 1, &kernel_data, &kernel_len);
        if (kernel_len)
            os_hdr = hdr;
    }

    consdev = "";
#if   defined(CONFIG_8xx_CONS_SMC1)
    consdev = "smc1";
#elif defined(CONFIG_8xx_CONS_SMC2)
    consdev = "smc2";
#elif defined(CONFIG_8xx_CONS_SCC2)
    consdev = "scc2";
#elif defined(CONFIG_8xx_CONS_SCC3)
    consdev = "scc3";
#endif

    if (argc > 2) {
        ulong len;
        int   i;

        for (i = 2, len = 0; i < argc; i += 1)
            len += strlen(argv[i]) + 1;
        cmdline = malloc(len);

        for (i = 2, len = 0; i < argc; i += 1) {
            if (i > 2)
                cmdline[len++] = ' ';
            strcpy(&cmdline[len], argv[i]);
            len += strlen(argv[i]);
        }
    } else if ((cmdline = getenv("bootargs")) == NULL) {
        cmdline = "";
    }

    loader = (void (*)(bd_t *, image_header_t *, char *, char *))images->ep;

    printf("## Transferring control to NetBSD stage-2 loader "
            "(at address %08lx) ...\n",
            (ulong)loader);

    show_boot_progress(15);

    /*
     * NetBSD Stage-2 Loader Parameters:
     *   r3: ptr to board info data
     *   r4: image address
     *   r5: console device
     *   r6: boot args string
     */
    (*loader)(gd->bd, os_hdr, consdev, cmdline);

    return 1;
}
#endif /* CONFIG_BOOTM_NETBSD*/

#ifdef CONFIG_LYNXKDI
static int do_bootm_lynxkdi(int flag, int argc, char * const argv[],
        bootm_headers_t *images)
{
    image_header_t *hdr = &images->legacy_hdr_os_copy;

    if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
        return 1;

#if defined(CONFIG_FIT)
    if (!images->legacy_hdr_valid) {
        fit_unsupported_reset("Lynx");
        return 1;
    }
#endif

    lynxkdi_boot((image_header_t *)hdr);

    return 1;
}
#endif /* CONFIG_LYNXKDI */

#ifdef CONFIG_BOOTM_RTEMS
static int do_bootm_rtems(int flag, int argc, char * const argv[],
        bootm_headers_t *images)
{
    void (*entry_point)(bd_t *);

    if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
        return 1;

#if defined(CONFIG_FIT)
    if (!images->legacy_hdr_valid) {
        fit_unsupported_reset("RTEMS");
        return 1;
    }
#endif

    entry_point = (void (*)(bd_t *))images->ep;

    printf("## Transferring control to RTEMS (at address %08lx) ...\n",
            (ulong)entry_point);

    show_boot_progress(15);

    /*
     * RTEMS Parameters:
     *   r3: ptr to board info data
     */
    (*entry_point)(gd->bd);

    return 1;
}
#endif /* CONFIG_BOOTM_RTEMS */

#if defined(CONFIG_BOOTM_OSE)
static int do_bootm_ose(int flag, int argc, char * const argv[],
        bootm_headers_t *images)
{
    void (*entry_point)(void);

    if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
        return 1;

#if defined(CONFIG_FIT)
    if (!images->legacy_hdr_valid) {
        fit_unsupported_reset("OSE");
        return 1;
    }
#endif

    entry_point = (void (*)(void))images->ep;

    printf("## Transferring control to OSE (at address %08lx) ...\n",
            (ulong)entry_point);

    show_boot_progress(15);

    /*
     * OSE Parameters:
     *   None
     */
    (*entry_point)();

    return 1;
}
#endif /* CONFIG_BOOTM_OSE */

#if defined(CONFIG_CMD_ELF)
static int do_bootm_vxworks(int flag, int argc, char * const argv[],
        bootm_headers_t *images)
{
    char str[80];

    if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
        return 1;

#if defined(CONFIG_FIT)
    if (!images->legacy_hdr_valid) {
        fit_unsupported_reset("VxWorks");
        return 1;
    }
#endif

    sprintf(str, "%lx", images->ep); /* write entry-point into string */
    setenv("loadaddr", str);
    do_bootvx(NULL, 0, 0, NULL);

    return 1;
}

static int do_bootm_qnxelf(int flag, int argc, char * const argv[],
        bootm_headers_t *images)
{
    char *local_args[2];
    char str[16];

    if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
        return 1;

#if defined(CONFIG_FIT)
    if (!images->legacy_hdr_valid) {
        fit_unsupported_reset("QNX");
        return 1;
    }
#endif

    sprintf(str, "%lx", images->ep); /* write entry-point into string */
    local_args[0] = argv[0];
    local_args[1] = str;    /* and provide it via the arguments */
    do_bootelf(NULL, 0, 2, local_args);

    return 1;
}
#endif

#ifdef CONFIG_INTEGRITY
static int do_bootm_integrity(int flag, int argc, char * const argv[],
        bootm_headers_t *images)
{
    void (*entry_point)(void);

    if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
        return 1;

#if defined(CONFIG_FIT)
    if (!images->legacy_hdr_valid) {
        fit_unsupported_reset("INTEGRITY");
        return 1;
    }
#endif

    entry_point = (void (*)(void))images->ep;

    printf("## Transferring control to INTEGRITY (at address %08lx) ...\n",
            (ulong)entry_point);

    show_boot_progress(15);

    /*
     * INTEGRITY Parameters:
     *   None
     */
    (*entry_point)();

    return 1;
}
#endif

#ifdef CAMEO_BOOT_SUPPORT
static int cameo_do_img_chksum(
        char *img_type,
        unsigned long img_offset,
        unsigned long img_size,
        unsigned long *img_chksum);
static int cameo_get_multi_env_vars(
        unsigned long *env_img_id,
        char *ethaddr,
        char *hw_version,
        unsigned long *run_config_id);
static int cameo_select_run_img(
        unsigned long img1_is_intact,
        unsigned long img2_is_intact,
        unsigned long env_img_id,
        unsigned long *run_img_id);

static int cameo_set_bootargs(
        unsigned long boot_chksum,
        unsigned long kernel_chksum,
        unsigned long rootfs_chksum,
#ifdef CONFIG_DUAL_IMAGE
        unsigned long kernel2_chksum,
        unsigned long rootfs2_chksum,
        unsigned long img_is_intact,
        unsigned long img2_is_intact,
#endif
        unsigned long run_img_id,
        char *ethaddr,
        char *bmodel_name,
        char *hw_version,
        unsigned long run_config_id);
static int cameo_run_kernel(unsigned long run_img_id);

/******************************************************************************
 ** do_bootf 
 **   boot application image from flash, apply to cameo products
 *******************************************************************************/
int do_bootf(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned long boot_chksum = 0;
    unsigned long kernel_chksum = 0;
    unsigned long rootfs_chksum = 0;
    unsigned long img_is_intact = 0;    /* 0: False, else: True */
#ifdef CONFIG_DUAL_IMAGE
    unsigned long kernel2_chksum = 0;
    unsigned long rootfs2_chksum = 0;
#endif
    unsigned long img2_is_intact = 0;
    unsigned long env_img_id = 0;
    unsigned long run_img_id = 0;
    unsigned long run_config_Id = 0;
    char ethaddr[CAMEO_STR_SZ] = {0};
    char bmodel_name[CAMEO_STR_SZ] = {0};
    char hw_version[CAMEO_STR_SZ] = {0};

#ifdef CAMEO_SIMPLE_DISPLAY
    printf("\nLoading Runtime Image.................................... 0 %%");
#endif

    cameo_do_img_chksum("BOOT",
            MTD_PART_BOOT_OFFSET, MTD_PART_BOOT_SIZE, &boot_chksum);

#ifdef CAMEO_SIMPLE_DISPLAY
    printf("\rLoading Runtime Image.................................... 30 %%");
#endif

    if (cameo_do_img_chksum("KERNEL",
                MTD_PART_KERNEL_OFFSET,
                MTD_PART_KERNEL_SIZE,
                &kernel_chksum) == CAMEO_SUCCESS
            && cameo_do_img_chksum("ROOTFS",
                MTD_PART_ROOTFS_OFFSET,
                MTD_PART_ROOTFS_SIZE,
                &rootfs_chksum) == CAMEO_SUCCESS)
    {
        img_is_intact = 1;
    }

#ifdef CAMEO_SIMPLE_DISPLAY
    printf("\rLoading Runtime Image.................................... 60 %%");
#endif
#ifdef CONFIG_DUAL_IMAGE
    if (cameo_do_img_chksum("KERNEL2",
                MTD_PART_KERNEL2_OFFSET,
                MTD_PART_KERNEL2_SIZE,
                &kernel2_chksum) == CAMEO_SUCCESS 
            && cameo_do_img_chksum("ROOTFS2",
                MTD_PART_ROOTFS2_OFFSET,
                MTD_PART_ROOTFS2_SIZE,
                &rootfs2_chksum) == CAMEO_SUCCESS)
    {
        img2_is_intact = 1;
    }
#endif

#ifdef CAMEO_SIMPLE_DISPLAY
    printf("\rLoading Runtime Image.................................... 80 %%");
#endif
    cameo_get_multi_env_vars(&env_img_id, ethaddr, hw_version, &run_config_Id);

    if (cameo_select_run_img(img_is_intact, img2_is_intact, 
                env_img_id, &run_img_id) == CAMEO_FAILURE)
    {
        return CAMEO_FAILURE;
    }
    /*get the board module name*/
    strncpy(bmodel_name, CAMEO_BOARD_MODULENAME, strlen(CAMEO_BOARD_MODULENAME));

    cameo_set_bootargs(boot_chksum, kernel_chksum, rootfs_chksum,
#ifdef CONFIG_DUAL_IMAGE
            kernel2_chksum, rootfs2_chksum,
            img_is_intact, img2_is_intact,
#endif
            run_img_id, ethaddr, bmodel_name, hw_version, run_config_Id);


#ifdef CAMEO_SIMPLE_DISPLAY
    printf("\rLoading Runtime Image.................................... 100 %%");
#endif
    cameo_run_kernel(run_img_id);

    return CAMEO_SUCCESS;
}

/******************************************************************************
 ** cameo_do_img_chksum 
 **   get and verify image checksum
 *******************************************************************************/
static int cameo_do_img_chksum(
        char *img_type,
        unsigned long img_offset,
        unsigned long img_size,
        unsigned long *img_chksum)
{
#define CAMEO_IMG_TAG_SZ            8   /* "CAMEOTAG" */
#define CAMEO_IMG_VER_SZ            4
#define CAMEO_IMG_SUM_SZ            4

    unsigned long computed_chksum = 0;
    unsigned long embedded_chksum = 0;
    unsigned char *data;
    char cmd[CAMEO_STR_SZ];
    int i;
    unsigned long img_tag_is_found = 0;

    sprintf(cmd, "sf probe 0; sf read 0x%lx 0x%lx 0x%lx",
            (unsigned long) CONFIG_LOADADDR, 
            img_offset, img_size);
    run_command(cmd, 0);

#ifndef CAMEO_SIMPLE_DISPLAY
    printf("Verifying %s ... ", img_type);
#endif

    for (data = (unsigned char *) CONFIG_LOADADDR; 
            data < (unsigned char *) (CONFIG_LOADADDR + img_size); data++)
    {
        if (*data == 'C'
                && *(data + 1) == 'A'
                && *(data + 2) == 'M'
                && *(data + 3) == 'E'
                && *(data + 4) == 'O'
                && *(data + 5) == 'T'
                && *(data + 6) == 'A'
                && *(data + 7) == 'G'
                && ((unsigned long) data & 0x0f) == 0)
        {
            img_tag_is_found = 1;

            memcpy(&embedded_chksum, data + 12, CAMEO_IMG_SUM_SZ);
            embedded_chksum = htonl(embedded_chksum);

            for (i = 0; i < CAMEO_IMG_TAG_SZ + CAMEO_IMG_VER_SZ; i++)
            {
                computed_chksum += (unsigned long) *(data + i);
            }

            break;
        }

        computed_chksum += (unsigned long) *data;
    }

    if (!img_tag_is_found)
    {
        /* DON'T contain "CAMEOTAG" string anywhere in the hardware,
           e.g., printf("%s: \"CAMEOTAG\" NOT found\n", p_img_type);
           it will result in spurious 'Bad Checksum' */
        printf("Image Tag NOT Found\n");
        return CAMEO_FAILURE;
    }

    if (embedded_chksum != computed_chksum)
    {
        printf("Bad Checksum, read 0x%08lx, computed 0x%08lx\n",
                embedded_chksum, computed_chksum);
        return CAMEO_FAILURE;
    }

    *img_chksum = embedded_chksum;
#ifndef CAMEO_SIMPLE_DISPLAY
    printf("OK\n");
#endif
    return CAMEO_SUCCESS;
}

/******************************************************************************
 ** cameo_get_multi_env_vars 
 **   get multiple miscellaneous environment variables
 **   if not found in the 'env' partition, use default value 
 *******************************************************************************/
static int cameo_get_multi_env_vars(
        unsigned long *env_img_id,
        char *ethaddr,
        char *hw_version,
        unsigned long *run_config_id)
{
    char *envp;

    envp = getenv2("imageId");
    if (envp)
    {
        *env_img_id = simple_strtoul(envp, NULL, 10);
    }
    else
    {
        printf("No 'imageId', use default: %d\n", DEFAULT_ENV_IMG_ID);
        *env_img_id = DEFAULT_ENV_IMG_ID;
    }

    envp = getenv("ethaddr");
    if (envp)
    {
        strncpy(ethaddr, envp, sizeof(xstr(CONFIG_ETHADDR)));
    }
    else
    {
        printf("No 'ethaddr', use default: %s\n", xstr(CONFIG_ETHADDR));
        strncpy(ethaddr, xstr(CONFIG_ETHADDR), sizeof(xstr(CONFIG_ETHADDR)));
    }

    envp = getenv("hw_version");
    if (envp)
    {
        /*Use the envp length in case the HW version will be modified by bulk and length is changed with the original */
        strncpy(hw_version, envp, strlen(envp));
    }
    else
    {
        printf("No 'hw_version', use default: %s\n", DEFAULT_ENV_HW_VERSION);
        strncpy(hw_version, DEFAULT_ENV_HW_VERSION, 
                sizeof(DEFAULT_ENV_HW_VERSION));
    }

    envp = getenv2("configId");
    if (envp)
    {
        *run_config_id = simple_strtoul(envp, NULL, 10);
        if ((*run_config_id != 1) && (*run_config_id != 2))
        {
            printf("Invalid 'configId' %ld, please setenv2 to '1'/'2'\n", *run_config_id);
            *run_config_id = DEFAULT_ENV_CONFIG_ID;
        }
    }
    else
    {
        printf("No 'configId', please setenv2, Now use default: %d\n",
                DEFAULT_ENV_CONFIG_ID);
        *run_config_id = DEFAULT_ENV_CONFIG_ID;
    }

    return CAMEO_SUCCESS;
}

/******************************************************************************
 ** cameo_do_img_chksum 
 **   get and verify image checksum
 *******************************************************************************/
static int cameo_select_run_img(
        unsigned long img1_is_intact,
        unsigned long img2_is_intact,
        unsigned long env_img_id,
        unsigned long *run_img_id)
{
    if (!img1_is_intact && !img2_is_intact)
    {
        printf("Bad Image!!!\n");
        return CAMEO_FAILURE;
    }

    if (env_img_id == 1)
    {
        *run_img_id = img1_is_intact ? 1 : 2;
    }
    else if (env_img_id == 2)
    {
        *run_img_id = img2_is_intact ? 2 : 1;
    }
    else
    {
        *run_img_id = img1_is_intact ? 1 : 2;
        printf("Invalid 'imageId' %ld, use %ld\n", env_img_id, *run_img_id);
    }

    return CAMEO_SUCCESS;
}


/******************************************************************************
 ** cameo_set_bootargs 
 **   set bootargs variable which are passed to the Linux kernel
 **   as boot arguments (aka "command line")
 *******************************************************************************/
static int cameo_set_bootargs(
        unsigned long boot_chksum,
        unsigned long kernel_chksum,
        unsigned long rootfs_chksum,
#ifdef CONFIG_DUAL_IMAGE
        unsigned long kernel2_chksum,
        unsigned long rootfs2_chksum,
        unsigned long img_is_intact,
        unsigned long img2_is_intact,
#endif
        unsigned long run_img_id,
        char *ethaddr,
        char *bmodel_name,
        char *hw_version,
        unsigned long run_config_id)
{
    char bootargs[CAMEO_STR_SZ];

    sprintf(bootargs,
            "%s"
#ifdef CAMEO_SIMPLE_DISPLAY
            " loglevel=3"
#endif
            " csb=0x%08lx cso1=0x%08lx csf1=0x%08lx"
#ifdef CONFIG_DUAL_IMAGE
            " cso2=0x%08lx csf2=0x%08lx"
            "%s"
            "%s"
#endif
            " runimageId=%ld"
            " configId=%ld"
            " ethaddr=%s"
            " bid=%s/%s"
            " boot_version=%s",
            run_img_id == 1 ? KERNEL_BASIC_BOOTARGS : KERNEL2_BASIC_BOOTARGS,
            boot_chksum, kernel_chksum, rootfs_chksum,
#ifdef CONFIG_DUAL_IMAGE
            kernel2_chksum, rootfs2_chksum,
            !img_is_intact ? " abnormalId=1" : "",
            !img2_is_intact ? " abnormalId=2" : "",
#endif
            run_img_id,
            run_config_id,
            ethaddr,
            bmodel_name, hw_version, BOOTARGS_BOOT_VERSION);
#ifndef CAMEO_SIMPLE_DISPLAY
    printf("Board model: %s\n",bmodel_name);
    printf("Bootargs: %s\n", bootargs);
#endif
    setenv("bootargs", bootargs);

    return CAMEO_SUCCESS;
}


/******************************************************************************
 ** cameo_run_kernel 
 **   load and run image specified by 'run_img_id'
 *******************************************************************************/
static int cameo_run_kernel(unsigned long run_img_id)
{
    char cmd[CAMEO_STR_SZ];

    if (run_img_id == 1)
    {
        sprintf(cmd, "sf probe 0; sf read 0x%lx 0x%lx 0x%lx",
                (unsigned long) CONFIG_LOADADDR, 
                MTD_PART_KERNEL_OFFSET, MTD_PART_KERNEL_SIZE);
    }
    else
    {
        sprintf(cmd, "sf probe 0; sf read 0x%lx 0x%lx 0x%lx",
                (unsigned long) CONFIG_LOADADDR, 
                MTD_PART_KERNEL2_OFFSET, MTD_PART_KERNEL2_SIZE);
    }
    run_command(cmd, 0);

    sprintf(cmd, "bootm 0x%lx", (unsigned long) CONFIG_LOADADDR);
    run_command(cmd, 0);

    return CAMEO_SUCCESS;
}

U_BOOT_CMD(
        bootf, 1, 0, do_bootf,
        "boot application image from flash, apply to cameo products",
        ""
        );

#endif /* CAMEO_BOOT_SUPPORT */

