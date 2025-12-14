/*
 * Copyright (C) 2020 Realtek
 */
#include <common.h>
#include <command.h>
#include <asm/io.h>

#define ENV_UBOOT_IMG_NAME	    "uboot_img_name"
#define LOADER_NAME_DDR3_16BIT  "_ddr3_16bit.img"
#define LOADER_NAME_DDR4_16BIT  "_ddr4_16bit.img"
#define LOADER_NAME_DDR4_32BIT  "_ddr4_32bit.img"
#define LOADER_NAME_UNDEFINED   ".img"


static u32 is_updated=0;

int do_update_loader_name(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret=0;
	u32 ddr_type;
	u32 mem_io;
    char *old_name;
    char new_name[64];

    if(is_updated != 0) return ret;

    ddr_type = readl(0x1111008);
    mem_io = (readl(0x1111014)==0?32:16);

    old_name = env_get(ENV_UBOOT_IMG_NAME);

    if(ddr_type == 3){
        sprintf(new_name, "%s%s", old_name, LOADER_NAME_DDR3_16BIT);
    }else if((ddr_type == 4)&&(mem_io==16)){
        sprintf(new_name, "%s%s", old_name, LOADER_NAME_DDR4_16BIT);
    }else if((ddr_type == 4)&&(mem_io==32)){
        sprintf(new_name, "%s%s", old_name, LOADER_NAME_DDR4_32BIT);
    }else{
        sprintf(new_name, "%s", LOADER_NAME_UNDEFINED);
    }

    ret = env_set(ENV_UBOOT_IMG_NAME, new_name);
    is_updated = 1;
    return ret;
}


U_BOOT_CMD(
    upldrname,  4,  1,  do_update_loader_name,
    "Get the full bootloader name with DRAM info",
	""
);

