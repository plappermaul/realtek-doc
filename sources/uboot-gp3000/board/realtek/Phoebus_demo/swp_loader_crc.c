#ifdef CONFIG_OTTO_SNAF

#include <common.h>
#include <swp.h>
#include <u-boot/crc.h>

#define MTD_PARTITION_BOOT_NAME		"boot"
#define ENV_TFTP_BASE_NAME		"tftp_base"
#define ENV_BOOTLOADER_SZ_NAME		"fl_boot_sz"
#define ENV_BOOTLOADER_CRC_NAME		"bootloader_crc"

void swp_bootloader_crc32(void){

	const int bufsz = 32;
	char c[bufsz], *endp;
	ulong ret, crc = 0, tftp_base = 0, boot_sz = 0;

	tftp_base = simple_strtoul(env_get(ENV_TFTP_BASE_NAME), &endp, 16);
	boot_sz = simple_strtoul(env_get(ENV_BOOTLOADER_SZ_NAME), &endp, 16);
	if (boot_sz == 0) {
		printf("Loader size is 0\n");
		return;
	}
	//printf("tftp_base is 0x%lx\n", tftp_base);
	//printf("fl_boot_sz is 0x%lx\n", boot_sz);

	ret = run_command("nand read ${"ENV_TFTP_BASE_NAME"} "MTD_PARTITION_BOOT_NAME"", 0);
	if (ret) {
		printf("SPI NAND flash read failed\n");
		return;
	}

	/* Do bootloader CRC32 */
	crc = crc32_wd (0, (const uchar *)tftp_base, boot_sz, CHUNKSZ_CRC32);
	printf ("CRC32 for %08lx ... %08lx ==> %08lx\n",(ulong)tftp_base , (ulong)(tftp_base + boot_sz - 1), crc);

	char *s = env_get(ENV_BOOTLOADER_CRC_NAME);
	sprintf(c, "%08lx", crc);

	if(s == NULL){
		printf(ENV_BOOTLOADER_CRC_NAME " is empty, set %s\n", c);
		env_set(ENV_BOOTLOADER_CRC_NAME, c);
		env_save();
	} else {
		if(strcmp(s,c)!=0){
			printf("old " ENV_BOOTLOADER_CRC_NAME " [%s] is different to new [%s], set new crc\n", s, c);
			env_set(ENV_BOOTLOADER_CRC_NAME, c);
			env_save();
		} //else
			//old and new crc are the same, don't save env!
	}
}

PATCH_REG(swp_bootloader_crc32, 6);
#endif
