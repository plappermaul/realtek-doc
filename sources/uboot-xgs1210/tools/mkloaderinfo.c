/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 16483 $
 * $Date: 2011-03-25 18:02:52 +0800 (Fri, 25 Mar 2011) $
 *
 * Purpose : Add crc and size info to loader tail
 *
 * Feature : Add crc and size info to loader tail
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "compiler.h"

#include <configs/autoconf.h>
#include <linux/types.h>
#include <turnkey/sysinfo.h>

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

static uint32_t _edianChg(uint32_t input)
{
    uint32_t bytes[4];
    uint32_t output = 0;

    bytes[0] = (input >> 24 & 0xFF) << 0;
    bytes[1] = (input >> 16 & 0xFF) << 8;
    bytes[2] = (input >> 8 & 0xFF) << 16;
    bytes[3] = (input >> 0 & 0xFF) << 24;

    output |= bytes[0];
    output |= bytes[1];
    output |= bytes[2];
    output |= bytes[3];

    return output;
}

int
main(int argc, char *argv[])
{
    char *loader;
    FILE *fp = NULL;
    loader_tail_t loaderInfo;
    uint32_t size = 0;
    uint32_t crc = 0;
    uint32_t tcrc = 0;
    uint32_t chip = 0;

    if (argc > 1)
    {
        printf("Loader File: %s\n", argv[1]);

        if (NULL == (fp = fopen(argv[1], "r+")))
        {
            printf("Cannot open %s\n", argv[1]);
            return 0;
        }

        if (0 != fseek(fp, 0, SEEK_END))
        {
            fclose(fp);

            return 0;
        }

        /* Get Loader file size */
        size = ftell(fp);

        loader = malloc(size);
        fseek(fp, 0, 0);
        fread(loader, 1, size, fp);

        /* Calculate loader crc */
        crc = crc32(0, loader, size);
        free(loader);

        /* Get Chip Family */
        chip = _chipFamily();

        memset(&loaderInfo, 0, sizeof(loader_tail_t));
        memcpy(loaderInfo.lt_key, LOADER_TAIL, sizeof(loaderInfo.lt_key));
        loaderInfo.lt_chip = _edianChg(chip);
        loaderInfo.lt_crc = _edianChg(crc);
        loaderInfo.lt_size = _edianChg(size);

        /* Calculate loader tail crc */
        tcrc = crc32(0, &loaderInfo, sizeof(loader_tail_t));
        loaderInfo.lt_tcrc = _edianChg(tcrc);

        /* Print Tail Information */
        printf("Loader Chip: %x\n", chip);
        printf("Loader CRC: %x\n", crc);
        printf("Loader Size: %x\n", size);
        printf("Loader Tail CRC: %x\n", tcrc);

        /* Write tail */
        fwrite(&loaderInfo, 1, sizeof(loader_tail_t), fp);

        fclose(fp);

        printf("Write loader tail success\n");
    }

    return (0);
}
