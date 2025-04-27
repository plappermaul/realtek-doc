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
 * $Revision$
 * $Date$
 *
 * Purpose : Define commands for U-Boot
 *
 * Feature : Partition commands for U-Boot
 *
 */

/*
 * Include Files
 */

#include <common.h>
#include <command.h>

#include <environment.h>
#include <spi_flash.h>

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

typedef struct partition_s {
    char name[16];
} partition_t;


/*
 * Data Type Definition
 */
const partition_t partitions[] =
{
    {"LOADER"},
    {"BDINFO"},
    {"SYSINFO"},
    {"JFFS2_CFG"},
    {"JFFS2_LOG"},
    {"RUNTIME1"},
#ifdef CONFIG_DUAL_IMAGE
    {"RUNTIME2"},
#endif
    {""}
};

#ifndef CONFIG_BOOT_SPI_NAND
static struct spi_flash *env_flash = NULL;
#endif

/*
 * Function Declaration
 */

static int _flashErase(u32 flash_addr, size_t size)
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

    ret = 0;
    //puts("DONE\n");
#endif
    return ret;
}

int do_parterase (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret = 0;
    int partition = -1;
    int partsize = sizeof(partitions)/sizeof(partition_t) - 1;

    if (argc != 3)
        return cmd_usage(cmdtp);

    partition = simple_strtoul(argv[2], NULL, 10);

    if (0 == partition)
    {
        printf("Forbid to erase LOADER partition\n");
        return 0;
    }
    else if ((partition < 0) || (partition >= partsize))
    {
        printf("Partition %d is not exist\n", partition);
        return 0;
    }

    ret = _flashErase(PARTITION_ADDR(partition), PARTITION_SIZE(partition));

    return ret;
}

U_BOOT_CMD(
    flerase, 3, 0, do_parterase,
    "Erase flash partition",
    "index [index ...]\n"
    "    - Erase partition. Use flshow to get index of partition."
);


int do_partshow (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret = 0;
#ifndef CONFIG_BOOT_SPI_NAND
    int i = 0;

    printf("=============== FLASH Partition Layout ===============\n");
    printf("Index  Name       Size       Address\n");
    printf("------------------------------------------------------\n");
    for (i = 0; i < FLASH_INDEX_END; i++)
    {
        printf(" %-4d  %-10s 0x%-8x 0x%x-0x%x\n",
                   i,
                   partitions[i].name,
                   PARTITION_SIZE(i),
                   (PARTITION_ADDR(i) + CONFIG_SYS_FLASH_BASE),
                   (PARTITION_ADDR(i) + PARTITION_SIZE(i) + CONFIG_SYS_FLASH_BASE - 1));
    }
#endif
    printf("======================================================\n");

    return ret;
}

U_BOOT_CMD(
    flshow, 1, 0, do_partshow,
    "Show flash partition layout",
    ""
);
