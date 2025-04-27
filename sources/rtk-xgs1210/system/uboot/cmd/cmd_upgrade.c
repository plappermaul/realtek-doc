/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 97073 $
 * $Date: 2019-05-22 10:34:25 +0800 (Wed, 22 May 2019) $
 *
 * Purpose : Define commands for U-Boot
 *
 * Feature : Upgrade loader and runtime commands for U-Boot
 *
 */

/*
 * Include Files
 */
#include <common.h>
#include <command.h>

#include <environment.h>
#include <spi_flash.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <image.h>
#include <rtk_flash_common.h>
#include <turnkey/sysinfo.h>


/*
 * Symbol Definition
 */
#ifndef CONFIG_ENV_SPI_BUS
#define CONFIG_ENV_SPI_BUS 0
#endif
#ifndef CONFIG_ENV_SPI_CS
#define CONFIG_ENV_SPI_CS  0
#endif
#ifndef CONFIG_ENV_SPI_MAX_HZ
#define CONFIG_ENV_SPI_MAX_HZ  1000000
#endif
#ifndef CONFIG_ENV_SPI_MODE
#define CONFIG_ENV_SPI_MODE    SPI_MODE_3
#endif

#define FILE_SOURCE_ADDR      0x81000000
#define FILE_SOURCE_ADDR_STR  "0x81000000"

#define DFLT_RUNTIME_FILENAME  "vmlinux.bix"
#define DFLT_LOADER_FILENAME   "u-boot.bin"
#define DFLT_ROM_FILENAME      "rom.bix"

#ifdef CONFIG_DUAL_IMAGE
#define ROM_SIZE (PARTITION_SIZE(FLASH_INDEX_LOADER) + PARTITION_SIZE(FLASH_INDEX_LOADER_BDINFO) + PARTITION_SIZE(FLASH_INDEX_SYSINFO) + PARTITION_SIZE(FLASH_INDEX_JFFS2_CFG) + PARTITION_SIZE(FLASH_INDEX_JFFS2_LOG) + PARTITION_SIZE(FLASH_INDEX_KERNEL) + PARTITION_SIZE(FLASH_INDEX_KERNEL2))
#else
#define ROM_SIZE (PARTITION_SIZE(FLASH_INDEX_LOADER) + PARTITION_SIZE(FLASH_INDEX_LOADER_BDINFO) + PARTITION_SIZE(FLASH_INDEX_SYSINFO) + PARTITION_SIZE(FLASH_INDEX_JFFS2_CFG) + PARTITION_SIZE(FLASH_INDEX_JFFS2_LOG) + PARTITION_SIZE(FLASH_INDEX_KERNEL))
#endif

#ifdef CONFIG_DUAL_IMAGE
#define UPGRADE_ACTION_USAGE \
    "upgrade loader [FILENAME]\n" \
    "    - upgrade loader with image validation, such as chip family, size and CRC.\n" \
    "    - FILENAME: Specify loader file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_LOADER_FILENAME "' will be used\n" \
    "upgrade loaderforce [FILENAME]\n" \
    "    - upgrade loader without image validation.\n" \
    "    - FILENAME: Specify loader file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_LOADER_FILENAME "' will be used\n" \
    "upgrade runtime1 [FILENAME]\n" \
    "    - upgrade runtime image to first runtime partition.\n" \
    "    - FILENAME: Specify runtime file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_RUNTIME_FILENAME "' will be used\n" \
    "upgrade runtime2 [FILENAME]\n" \
    "    - upgrade runtime image to second runtime partition.\n" \
    "    - FILENAME: Specify runtime file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_RUNTIME_FILENAME "' will be used\n" \
    "upgrade runtimeforce [FILENAME]\n" \
    "    - upgrade runtime image to first+second runtime partitions for single image.\n" \
    "    - FILENAME: Specify runtime file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_RUNTIME_FILENAME "' will be used\n" \
    "upgrade rom [FILENAME]\n" \
    "    - upgrade loader with image validation, such as chip family, size and CRC." \
    "      this command will skip BDINFO and SYSINFO partition\n" \
    "    - FILENAME: Specify rom file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_ROM_FILENAME "' will be used\n" \
    "upgrade romforce [FILENAME]\n" \
    "    - upgrade whole rom image without image validation.\n" \
    "    - FILENAME: Specify rom file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_ROM_FILENAME "' will be used\n"
#else
#define UPGRADE_ACTION_USAGE \
    "upgrade loader [FILENAME]\n" \
    "    - upgrade loader with image validation, such as chip family, size and CRC.\n" \
    "    - FILENAME: Specify loader file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_LOADER_FILENAME "' will be used\n" \
    "upgrade loaderforce [FILENAME]\n" \
    "    - upgrade loader without image validation.\n" \
    "    - FILENAME: Specify loader file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_LOADER_FILENAME "' will be used\n" \
    "upgrade runtime [FILENAME]\n" \
    "    - upgrade runtime image to flash partition.\n" \
    "    - FILENAME: Specify runtime file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_RUNTIME_FILENAME "' will be used\n" \
    "upgrade rom [FILENAME]\n" \
    "    - upgrade loader with image validation, such as chip family, size and CRC." \
    "      this command will skip BDINFO and SYSINFO partition\n" \
    "    - FILENAME: Specify rom file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_ROM_FILENAME "' will be used\n" \
    "upgrade romforce [FILENAME]\n" \
    "    - upgrade whole rom image without image validation.\n" \
    "    - FILENAME: Specify rom file name on remote TFTP server.\n" \
    "                If not specified, '" DFLT_ROM_FILENAME "' will be used\n"
#endif


/*
 * Data Type Definition
 */
#ifndef CONFIG_BOOT_SPI_NAND
static struct spi_flash *env_flash = NULL;
#endif
struct rtk_partition_info_t rtk_flash_partition_size[4] = {
     [0] = {
         .flash_total_size = FLASH_SIZE_16MB,
#if defined(CONFIG_DUAL_IMAGE)
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x00680000U),
         .kernel2_offset = (0x00980000U),
         .kernel2_size = (0x00680000U)
#else
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x00d00000U),
         .kernel2_offset = (0x0U),
         .kernel2_size = (0x0U)
#endif
     },
     [1] = {
         .flash_total_size = FLASH_SIZE_32MB,
#if defined(CONFIG_DUAL_IMAGE)
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x00e80000U),
         .kernel2_offset = (0x01180000U),
         .kernel2_size = (0x00e80000U)
#else
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x01d00000U),
         .kernel2_offset = (0x0U),
         .kernel2_size = (0x0U)
#endif
     },
     [2] = {
         .flash_total_size = FLASH_SIZE_64MB,
#if defined(CONFIG_DUAL_IMAGE)
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x01e80000U),
         .kernel2_offset = (0x02180000U),
         .kernel2_size = (0x01e80000U)
#else
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x03d00000U),
         .kernel2_offset = (0x0U),
         .kernel2_size = (0x0U)
#endif
     },
     [3] = {
         .flash_total_size = FLASH_SIZE_128MB,
#if defined(CONFIG_DUAL_IMAGE)
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x03e80000U),
         .kernel2_offset = (0x04180000U),
         .kernel2_size = (0x03e80000U)
#else
         .loader_offset = (0x00000000U),
         .loader_size = (UNIT_SIZE*14),
         .bdinfo_offset = (0x000e0000U),
         .bdinfo_size = LOADER_BDINFO_SIZE,
         .sysinfo_offset = (0x000f0000U),
         .sysinfo_size = SYSINFO_SIZE,
         .jffs2_cfg_offset = (0x00100000U),
         .jffs2_cfg_size = (UNIT_SIZE*16),
         .jffs2_log_offset = (0x00200000U),
         .jffs2_log_size = (UNIT_SIZE*16),
         .kernel_offset = (0x00300000U),
         .kernel_size = (0x07d00000U),
         .kernel2_offset = (0x0U),
         .kernel2_size = (0x0U)
#endif
     }
 };


/*
 * Function Declaration
 */

extern int sdk_spi_flash_chip_size(int chip_index);
extern int sdk_spi_flash_chip_num(void);

void sdk_get_paritition_info(rtk_flash_partition_idx_t index, u32 *offset, u32 *size)
{
    u32 partition_idx = index;
#ifndef CONFIG_BOOT_SPI_NAND
    int flash_chip_num = sdk_spi_flash_chip_num();
#endif
    int flash_total_size = 0;
    unsigned int size_idx = SIZE_IDX_NOT_FOUND;
    unsigned int loop_idx;
#ifndef CONFIG_BOOT_SPI_NAND
    for(loop_idx = 0; loop_idx < flash_chip_num; loop_idx++)
        flash_total_size += sdk_spi_flash_chip_size(loop_idx);
#endif
    for(loop_idx = 0; loop_idx < (sizeof(rtk_flash_partition_size)/sizeof(rtk_flash_partition_size[0])); loop_idx++)
    {
        if(rtk_flash_partition_size[loop_idx].flash_total_size == flash_total_size){
            size_idx = loop_idx;
        }
    }

    if(size_idx == SIZE_IDX_NOT_FOUND)
        return;

    switch(partition_idx){
        case FLASH_INDEX_LOADER:			/* LOADER */
            *offset = rtk_flash_partition_size[size_idx].loader_offset;
            *size = rtk_flash_partition_size[size_idx].loader_size;
            break;
        case FLASH_INDEX_LOADER_BDINFO:		/* LOADER_BDINFO */
            *offset = rtk_flash_partition_size[size_idx].bdinfo_offset;
            *size = rtk_flash_partition_size[size_idx].bdinfo_size;
            break;
        case FLASH_INDEX_SYSINFO:			/* SYSINFO */
            *offset = rtk_flash_partition_size[size_idx].sysinfo_offset;
            *size = rtk_flash_partition_size[size_idx].sysinfo_size;
            break;
        case FLASH_INDEX_JFFS2_CFG:			/* JFFS2_CFG */
            *offset = rtk_flash_partition_size[size_idx].jffs2_cfg_offset;
            *size = rtk_flash_partition_size[size_idx].jffs2_cfg_size;
            break;
        case FLASH_INDEX_JFFS2_LOG:			/* JFFS2_LOG */
            *offset = rtk_flash_partition_size[size_idx].jffs2_log_offset;
            *size = rtk_flash_partition_size[size_idx].jffs2_log_size;
            break;
        case FLASH_INDEX_KERNEL:			/* KERNEL */
            *offset = rtk_flash_partition_size[size_idx].kernel_offset;
            *size = rtk_flash_partition_size[size_idx].kernel_size;
            break;
#if defined(CONFIG_DUAL_IMAGE)
        case FLASH_INDEX_KERNEL2:			/* KERNEL2 */
            *offset = rtk_flash_partition_size[size_idx].kernel2_offset;
            *size = rtk_flash_partition_size[size_idx].kernel2_size;
            break;
#endif
        default:
            break;
    }
    return;
}

unsigned int flash_partition_size_ret(rtk_flash_partition_idx_t partition)
{
    unsigned int size = 0;
    unsigned int offset = 0;

    sdk_get_paritition_info(partition, &offset, &size);

    return size;
}

unsigned int flash_partition_addr_ret(rtk_flash_partition_idx_t partition)
{
    unsigned int size = 0;
    unsigned int offset = 0;

    sdk_get_paritition_info(partition, &offset, &size);

    return offset;
}


static int _rtkNetworkOn(void)
{
    cmd_tbl_t *run;
    char *argu[4];

    run = find_cmd("rtk");
    if (NULL == run) {
        printf("rtk command is not supported!\n");
        return 1;
    }
    argu[0] = "rtk";
    argu[1] = "network";
    argu[2] = "on";
    argu[3] = NULL;

    return run->cmd(run, 0, 3, argu);
}

static int _tftpDownloadFile(char *filename)
{
    cmd_tbl_t *run;
    char *argu[4];

    if (_rtkNetworkOn())
        return 1;

    run = find_cmd("tftp");
    if (NULL == run) {
        printf("tftp command is not supported!\n");
        return 1;
    }
    argu[0] = "tftp";
    argu[1] = FILE_SOURCE_ADDR_STR;
    argu[2] = filename;
    argu[3] = NULL;

    return run->cmd(run, 0, 3, argu);
}

#ifndef CONFIG_BOOT_SPI_NAND
static int _compare(u32 flash_addr, size_t size, u32 offset)
{
    cmd_tbl_t *run;
    char *argu[5];
    char addrStr[16];
    char sizeStr[32];
    char fileSrcStr[16];

    run = find_cmd("cmp.b");
    if (NULL == run) {
        printf("cmp.b command is not supported!\n");
        return 1;
    }

    printf("Comparing file ......\n");
    osal_memset(addrStr, 0, sizeof(addrStr));
    osal_memset(sizeStr, 0, sizeof(sizeStr));
    osal_memset(fileSrcStr, 0, sizeof(fileSrcStr));

    sprintf(addrStr, "%x", flash_addr + CONFIG_SYS_FLASH_BASE);
    sprintf(sizeStr, "%x", (unsigned int)size);
    sprintf(fileSrcStr, "%x", FILE_SOURCE_ADDR + offset);

    argu[0] = "cmp.b";
    argu[1] = fileSrcStr;
    argu[2] = addrStr;
    argu[3] = sizeStr;
    argu[4] = NULL;

    return run->cmd(run, 0, 4, argu);
}
#endif
static int _write2flash(u32 flash_addr, size_t size, u32 offset)
{
    int ret = 0;
#ifndef CONFIG_BOOT_SPI_NAND
    if (!env_flash) {
        env_flash = spi_flash_probe(CONFIG_ENV_SPI_BUS,
                                    CONFIG_ENV_SPI_CS,
                                    CONFIG_ENV_SPI_MAX_HZ,
                                    CONFIG_ENV_SPI_MODE);
        if (!env_flash) {
            set_default_env("!spi_flash_probe() failed");
            return 1;
        }
    }

    //puts("Erasing SPI flash...");
    ret = spi_flash_erase(env_flash, flash_addr, size);
    if (ret)
    {
        //puts("FAIL\n");
        return ret;
    }

    //puts("Writing to SPI flash...");
    ret = spi_flash_write(env_flash, flash_addr, size, (void*)(FILE_SOURCE_ADDR + offset));
    if (ret)
    {
        //puts("FAIL\n");
        return ret;
    }

    ret = 0;
    //puts("DONE\n");

    if (_compare(flash_addr, size, offset))
    {
        return 1;
    }
#endif
    return ret;
}

static uint32_t _chipFamily(void)
{
#ifdef CONFIG_SDK_RTL8390
    return LOADER_TAIL_CHIP_839X;
#endif
#ifdef CONFIG_SDK_RTL8380
    return LOADER_TAIL_CHIP_838X;
#endif
#ifdef CONFIG_SDK_RTL9300
    return LOADER_TAIL_CHIP_930X;
#endif
#ifdef CONFIG_SDK_RTL9310
    return LOADER_TAIL_CHIP_931X;
#endif
}

static int _loaderCheck(char *loader)
{
    int size = flash_partition_size_ret(FLASH_INDEX_LOADER);
    u32 crc = 0;
    u32 tcrc = 0;
    u32 chip = _chipFamily();
    char tailKey[8];
    char *ptr = loader;
    char *pch = NULL;
    loader_tail_t tail;
    loader_tail_t tmp_tail;

    tailKey[0] = '#';
    tailKey[1] = '-';
    tailKey[2] = 'T';
    tailKey[3] = 'A';
    tailKey[4] = 'I';
    tailKey[5] = 'L';
    tailKey[6] = '-';
    tailKey[7] = '#';

    while (size > 0)
    {
        if (NULL != (pch = memchr(ptr, '#', 1)))
        {
            if (0 == memcmp(pch, tailKey, sizeof(tailKey)))
            {
                osal_memset(&tail, 0, sizeof(loader_tail_t));
                osal_memset(&tmp_tail, 0, sizeof(loader_tail_t));
                osal_memcpy(&tail, pch, sizeof(loader_tail_t));
                osal_memcpy(&tmp_tail, pch, sizeof(loader_tail_t));

                tmp_tail.lt_tcrc = 0;
                /* Calculate loader tail crc */
                tcrc = crc32(0, (unsigned char*)&tmp_tail, sizeof(loader_tail_t));
                crc = crc32(0, (unsigned char*)loader, (pch - loader));

                printf("  Loader Chip: %x\n", tail.lt_chip);
                printf("  Loader CRC: %x\n", tail.lt_crc);
                printf("  Loader Size: %x\n", tail.lt_size);
                printf("  Loader Tail CRC: %x\n", tail.lt_tcrc);

                if (tail.lt_chip != chip)
                {
                    printf("\nCurrent Loader Chip Family %x\n", chip);
                    return 1;
                }

                if (tail.lt_size != (pch - loader))
                {
                    printf("\nCalculate Loader Size %x\n", (pch - loader));
                    return 1;
                }

                if (tail.lt_crc != crc)
                {
                    printf("\nCalculate Loader CRC %x\n", crc);
                    return 1;
                }

                if (tcrc != tail.lt_tcrc)
                {
                    printf("\nCalculate Loader Tail CRC %x\n", tcrc);
                    return 1;
                }

                return 0;
            }
        }

        ptr += 1;
        size -= 1;
    }

    return 1;
}

int upgrade_loader (char *filename, int force)
{
    int ret = 0;

    printf("Upgrade loader image [%s]......\n", filename);

    /* Reset memory value to be the same as flash erased value */
    osal_memset((void*)FILE_SOURCE_ADDR, 0xff, PARTITION_SIZE(FLASH_INDEX_LOADER));

    if (_tftpDownloadFile(filename))
    {
        printf("Download loader image [%s] failed\n", filename);
        return 1;
    }

    if (!force)
    {
        if (_loaderCheck((void*)FILE_SOURCE_ADDR))
        {
            printf("Invalid loader image [%s]\n", filename);
            return 1;
        }
    }

    if (_write2flash(PARTITION_ADDR(FLASH_INDEX_LOADER), PARTITION_SIZE(FLASH_INDEX_LOADER), 0))
    {
        printf("Write loader image [%s] failed\n", filename);
        return 1;
    }

    printf("Upgrade loader image [%s] success\n", filename);

    return ret;
}

int upgrade_runtime (char *filename, int partition, int is_file_ready)
{
    int ret = 0;
    char partitionStr[32];
    void *hdr = (void *)FILE_SOURCE_ADDR;

    printf("Upgrade runtime image [%s]......\n", filename);

    if (!is_file_ready)
    {
        /* Reset memory value to be the same as flash erased value */
        osal_memset((void*)FILE_SOURCE_ADDR, 0xff, PARTITION_SIZE(FLASH_INDEX_KERNEL));

        /* Download runtime file */
        if (_tftpDownloadFile(filename))
        {
            printf("Download runtime image [%s] failed\n", filename);
            return 1;
        }
    }

    /* Check Image header CRC */
    if (!image_check_hcrc(hdr)) {
        printf("Bad Header Checksum\n");
        return 1;
    }

    image_print_contents(hdr);

    /* Check Image CRC */
    if (!image_check_dcrc(hdr)) {
        printf("Bad Data CRC\n");
        return 1;
    }

    /* Write to partition */
    osal_memset(partitionStr, 0, sizeof(partitionStr));
    if (0 == partition)
    {
#ifdef CONFIG_DUAL_IMAGE
        strcpy(partitionStr, "to partition 0 ");
#endif
        if (_write2flash(PARTITION_ADDR(FLASH_INDEX_KERNEL), PARTITION_SIZE(FLASH_INDEX_KERNEL), 0))
        {
            printf("Write runtime image [%s] %sfailed\n", filename, partitionStr);
            return 1;
        }
    }
#ifdef CONFIG_DUAL_IMAGE
    else if (1 == partition)
    {
        strcpy(partitionStr, "to partition 1 ");
        if (_write2flash(PARTITION_ADDR(FLASH_INDEX_KERNEL2), PARTITION_SIZE(FLASH_INDEX_KERNEL2), 0))
        {
            printf("Write runtime image [%s] %sfailed\n", filename, partitionStr);
            return 1;
        }
    }
    else if (-1 == partition)
    {
        strcpy(partitionStr, "to partition 1+2 ");
        if (_write2flash(PARTITION_ADDR(FLASH_INDEX_KERNEL), (PARTITION_SIZE(FLASH_INDEX_KERNEL) + PARTITION_SIZE(FLASH_INDEX_KERNEL2)), 0))
        {
            printf("Write runtime image [%s] %sfailed\n", filename, partitionStr);
            return 1;
        }
    }
#endif

    printf("Upgrade runtime image [%s] %ssuccess\n", filename, partitionStr);

    return ret;
}

int upgrade_rom (char *filename, int force)
{
    int ret = 0;

    printf("Upgrade rom image [%s]......\n", filename);

    /* Reset memory value to be the same as flash erased value */
    osal_memset((void*)FILE_SOURCE_ADDR, 0xff, ROM_SIZE);

    if (_tftpDownloadFile(filename))
    {
        printf("Download rom image [%s] failed\n", filename);
        return 1;
    }

    if (!force)
    {
        int offset = 0;

        if (_loaderCheck((void*)FILE_SOURCE_ADDR))
        {
            printf("Invalid rom image [%s]\n", filename);
            return 1;
        }

        if (_write2flash(flash_partition_addr_ret(FLASH_INDEX_LOADER), PARTITION_SIZE(FLASH_INDEX_LOADER), 0))
        {
            printf("Write loader image [%s] failed\n", filename);
            return 1;
        }

        offset = (PARTITION_SIZE(FLASH_INDEX_LOADER) + PARTITION_SIZE(FLASH_INDEX_LOADER_BDINFO) + PARTITION_SIZE(FLASH_INDEX_SYSINFO));
        if (_write2flash(PARTITION_ADDR(FLASH_INDEX_JFFS2_CFG), (ROM_SIZE - offset), offset))
        {
            printf("Write jffs2 and runtime image [%s] failed\n", filename);
            return 1;
        }
    }
    else
    {
        if (_write2flash(PARTITION_ADDR(FLASH_INDEX_LOADER), ROM_SIZE, 0))
        {
            printf("Write rom image [%s] failed\n", filename);
            return 1;
        }
    }

    printf("Upgrade rom image [%s] success\n", filename);

    return ret;
}


int do_upgrade (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char filename[256];

    osal_memset(filename, 0, sizeof(filename));

    if (argc <= 1)
    {
        return cmd_usage(cmdtp);
    }
    else
    {
        if (0 == strcmp(argv[1], "loader"))
        {
            if (3 == argc)
            {
                if (strlen(argv[2]) >= sizeof(filename))
                {
                    osal_printf("Error: File name length too long!\n");
                    goto EXIT;
                }

                strncpy(filename, argv[2], sizeof(filename));
            }
            else
                strcpy(filename, DFLT_LOADER_FILENAME);

            upgrade_loader(filename, 0);
        }
        else if (0 == strcmp(argv[1], "loaderforce"))
        {
            if (3 == argc)
            {
                if (strlen(argv[2]) >= sizeof(filename))
                {
                    osal_printf("Error: File name length too long!\n");
                    goto EXIT;
                }

                strncpy(filename, argv[2], sizeof(filename));
            }
            else
                strcpy(filename, DFLT_LOADER_FILENAME);

            upgrade_loader(filename, 1);
        }
#ifdef CONFIG_DUAL_IMAGE
        else if (0 == strcmp(argv[1], "runtime1"))
        {
            if (3 == argc)
            {
                if (strlen(argv[2]) >= sizeof(filename))
                {
                    osal_printf("Error: File name length too long!\n");
                    goto EXIT;
                }

                strncpy(filename, argv[2], sizeof(filename));
            }
            else
                strcpy(filename, DFLT_RUNTIME_FILENAME);

            upgrade_runtime(filename, 0, 0);
        }
        else if (0 == strcmp(argv[1], "runtime2"))
        {
            if (3 == argc)
            {
                if (strlen(argv[2]) >= sizeof(filename))
                {
                    osal_printf("Error: File name length too long!\n");
                    goto EXIT;
                }

                strncpy(filename, argv[2], sizeof(filename));
            }
            else
                strcpy(filename, DFLT_RUNTIME_FILENAME);

            upgrade_runtime(filename, 1, 0);
        }
        else if (0 == strcmp(argv[1], "runtimeforce"))
        {
            if (3 == argc)
            {
                if (strlen(argv[2]) >= sizeof(filename))
                {
                    osal_printf("Error: File name length too long!\n");
                    goto EXIT;
                }

                strncpy(filename, argv[2], sizeof(filename));
            }
            else
                strcpy(filename, DFLT_RUNTIME_FILENAME);

            upgrade_runtime(filename, -1, 0);
        }
#else
        else if (0 == strcmp(argv[1], "runtime"))
        {
            if (3 == argc)
            {
                if (strlen(argv[2]) >= sizeof(filename))
                {
                    osal_printf("Error: File name length too long!\n");
                    goto EXIT;
                }

                strncpy(filename, argv[2], sizeof(filename));
            }
            else
                strcpy(filename, DFLT_RUNTIME_FILENAME);

            upgrade_runtime(filename, 0, 0);
        }
#endif
        else if (0 == strcmp(argv[1], "rom"))
        {
            if (3 == argc)
            {
                if (strlen(argv[2]) >= sizeof(filename))
                {
                    osal_printf("Error: File name length too long!\n");
                    goto EXIT;
                }

                strncpy(filename, argv[2], sizeof(filename));
            }
            else
                strcpy(filename, DFLT_ROM_FILENAME);

            upgrade_rom(filename, 0);
        }
        else if (0 == strcmp(argv[1], "romforce"))
        {
            if (3 == argc)
            {
                if (strlen(argv[2]) >= sizeof(filename))
                {
                    osal_printf("Error: File name length too long!\n");
                    goto EXIT;
                }

                strncpy(filename, argv[2], sizeof(filename));
            }
            else
                strcpy(filename, DFLT_ROM_FILENAME);

            upgrade_rom(filename, 1);
        }
        else
        {
            return cmd_usage(cmdtp);
        }
    }

  EXIT:
    return 0;

}

U_BOOT_CMD(
    upgrade, 3, 0, do_upgrade,
    "Upgrade loader or runtime image",
    UPGRADE_ACTION_USAGE
);

