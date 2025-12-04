/*
 * Command for updating vm.img format image (containing uImage and rootfs) into corresponding partition on luna platform.
 * Author: bohungwu@realtek.com
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <asm/io.h>
//#include <environment.h>

#if defined(CONFIG_MTD_CORTINA_SERIAL_NOR)
#define CONFIG_SPINOR_FLASH 1
#endif

#define VMIMG_DEBUG

#if CONFIG_RTK_USE_ONE_UBI_DEVICE
#define UBI_MAIN_DEVICE_NAME "ubi_device"
#define UBI_DTB0_NAME "ubi_DTB0"
#define UBI_DTB1_NAME "ubi_DTB1"
#define UBI_K0_NAME "ubi_k0"
#define UBI_K1_NAME "ubi_k1"
#define UBI_R0_NAME "ubi_r0"
#define UBI_R1_NAME "ubi_r1"
#define UBI_FRAMEWORK1_NAME "ubi_framework1"
#define UBI_FRAMEWORK2_NAME "ubi_framework2"
#define UBI_APP_NAME "ubi_apps"
#define UBI_OSGI_NAME "ubi_osgi"
#define UBI_CONFIG_NAME "ubi_Config"
#define UBI_FPGA_NAME "ubi_fpga"
#endif

#if CONFIG_CMD_MMC
#define MMC_DTB0_NAME "DTB0"
#define MMC_DTB1_NAME "DTB1"
#define MMC_K0_NAME "k0"
#define MMC_K1_NAME "k1"
#define MMC_R0_NAME "r0"
#define MMC_R1_NAME "r1"
#define MMC_FRAMEWORK1_NAME "framework1"
#define MMC_FRAMEWORK2_NAME "framework2"
#define MMC_APP_NAME "apps"
#define MMC_OSGI_NAME "osgi"
#define MMC_ENV_NAME "env"
#define MMC_ENV2_NAME "env2"
#endif

#define MTD_DTB0_NAME "DTB0"
#define MTD_DTB1_NAME "DTB1"
#define MTD_K0_NAME "k0"
#define MTD_K1_NAME "k1"
#define MTD_R0_NAME "r0"
#define MTD_R1_NAME "r1"
#define MTD_FRAMEWORK1_NAME "framework1"
#define MTD_FRAMEWORK2_NAME "framework2"
#define MTD_APP_NAME "apps"
#define MTD_OSGI_NAME "osgi"
#define MTD_ENV_NAME "env"
#define MTD_ENV2_NAME "env2"
#define MTD_FPGA_NAME "fpgafs"


struct vmimg_hdr {
	u32 key;
	u32 load_addr;
	u32 img_sz;
	u32 entry_addr;
	u32 padding[11];
	u32 flash_base;
};

#ifdef VMIMG_DEBUG
static void dump_vmimg_hdr(struct vmimg_hdr *vmimg_hdr_p) {

	printf("key=0x%x\n", vmimg_hdr_p->key);
	printf("load_addr=0x%x\n", vmimg_hdr_p->load_addr);
	printf("img_sz=0x%x(%u)\n", vmimg_hdr_p->img_sz, vmimg_hdr_p->img_sz);
	printf("entry_addr=0x%x\n", vmimg_hdr_p->entry_addr);
	printf("flash_base=0x%x\n", vmimg_hdr_p->flash_base);
}
#else
#define dump_vmimg_hdr(x)
#endif

#define VMIMG_BOOT_KEY   (0xB0010001)
#define VMIMG_DTB_KEY    (0xa0000443)
#define VMIMG_KERNEL_KEY (0xa0000203)
#define VMIMG_ROOTFS_KEY (0xa0000403)
#define VMIMG_UBI_DEVICE_KEY (0xa0000503)
#define CMD_BUF_SZ (256)
#define VMIMG_CONFIG_KEY	(0xCF010002)
#define VMIMG_CFGPROV_KEY	(0xCF010202)
#define VMIMG_APPLICATION_KEY	(0xA0000003)
#define VMIMG_OSGI_KEY		(0xCAFEBABE)
#define VMIMG_BOOTENV_KEY    (0xB0020001)
#define VMIMG_VOLUME_KEY     (0xA0000603)
#define VMIMG_FPGA_KEY      (0xA0000423)

const u32 vmimg_key_list[] = {
	VMIMG_BOOT_KEY,
	VMIMG_DTB_KEY,
	VMIMG_KERNEL_KEY,
	VMIMG_ROOTFS_KEY,
	VMIMG_UBI_DEVICE_KEY,
	VMIMG_CONFIG_KEY,
	VMIMG_CFGPROV_KEY,
	VMIMG_APPLICATION_KEY,
	VMIMG_OSGI_KEY,
	VMIMG_BOOTENV_KEY,
	VMIMG_VOLUME_KEY,
	VMIMG_FPGA_KEY,
	0
};


#define ENV_CONFIG_PROVINCE	"cfg_province"
#define ENV_RESET_DEFAULT	"rst2dfl"
#define ENV_SYNC_DEFAULT	"sync_default_env"
#define CFG_PROV_SZ		(8)		/* Maximum Config Province Code */

static int update_boot_flag    = 0;
static int update_env_flag     = 0;
static int update_provice_flag = 0;
static int env_dirty_flag      = 0;
static char province_cfg[CFG_PROV_SZ+1];
#if CONFIG_CMD_MMC
static int update_emmc_flag = 0;
#endif

int check_vm_img(unsigned char *buffer, unsigned int size)
{
	struct vmimg_hdr *hdr;
	u32 key;
	int index = 0;
	int offset = 0;

	//printf("Is this a vm.img?\n");
	if (size < sizeof(struct vmimg_hdr))	return 0;

	while(offset < size)
	{
		hdr = (struct vmimg_hdr *)buffer;
		printf("#%d Key: 0x%08x\n", index, ntohl(hdr->key));
		key = ntohl(hdr->key);
		int list_idx = 0;
		while(vmimg_key_list[list_idx] != 0)
		{
			if (vmimg_key_list[list_idx] == key) {
				break;
			}
			list_idx++;
		}

		if(vmimg_key_list[list_idx] == 0)
			return 0;

		offset += ntohl(hdr->img_sz) + sizeof(struct vmimg_hdr);
		buffer = buffer + ntohl(hdr->img_sz) + sizeof(struct vmimg_hdr);
		if (ntohl(hdr->img_sz) % 4) {
			printf("Error: #%d Key: 0x%08x, size %d is not 4 alignment!\n", index, ntohl(hdr->key), ntohl(hdr->img_sz));
			return 0;
		}
		index++;
	}

	return 1;
}

#if CONFIG_RTK_USE_ONE_UBI_DEVICE
static int
do_ubi_update(char *vol_name,  unsigned int src, unsigned int src_size)
{
	char cmd_buf[CMD_BUF_SZ] = {0};
	int ret;
#if CONFIG_CMD_MMC
	if (update_emmc_flag) {
		return 0;
	}
#endif

	sprintf(cmd_buf, "setenv current_vol %s", vol_name);
	ret = run_command(cmd_buf, 0);
	sprintf(cmd_buf, "run check_vol");
	ret = run_command(cmd_buf, 0);
	if (ret == -1) {
		return ret;
	}
	//sprintf(cmd_buf, "setenv current_vol %s && run check_vol", vol_name);
	//run_command(cmd_buf, 0);
	sprintf(cmd_buf, "ubi write %x %s %x", src, vol_name, src_size);
	printf("CMD = %s\n", cmd_buf);
	ret = run_command(cmd_buf, 0);
	if (ret == -1) {
		return ret;
	}
	return 0;
}

static int
do_ubi_clean(char *vol_name)
{
	char cmd_buf[CMD_BUF_SZ] = {0};
	int ret;

#if CONFIG_CMD_MMC
	if (update_emmc_flag) {
		return 0;
	}
#endif

	sprintf(cmd_buf, "setenv current_vol %s", vol_name);
	ret = run_command(cmd_buf, 0);
	sprintf(cmd_buf, "run check_vol");
	ret = run_command(cmd_buf, 0);
	if (ret == -1) {
		return ret;
	}
	sprintf(cmd_buf, "ubi clean %s", vol_name);
	printf("CMD = %s\n", cmd_buf);
	ret = run_command(cmd_buf, 0);
	if (ret == -1) {
		return ret;
	}
	return 0;
}
#endif

#if CONFIG_CMD_MMC
int sscanf(const char *buf, const char *fmt, ...);
#define MMC_PART_START_NAME "par_st"
#define MMC_PART_SIZE_NAME "par_sz"
#define MMC_BLK_SIZE 512
static int
do_mmc_update(char *part_name, unsigned int src, unsigned int src_size)
{
	char cmd_buf[CMD_BUF_SZ] = {0};
	int ret;
	char *value = NULL;
	char *part_st = NULL;
	char *part_sz = NULL;
	unsigned int part_blk_cnt;
	unsigned int src_blk_cnt;

	//sprintf(cmd_buf, "setenv pname %s", part_name);
	//ret = run_command(cmd_buf, 0);
	snprintf(cmd_buf, sizeof(cmd_buf)-1,"partinfo info %s", part_name);
	ret = run_command(cmd_buf, 0);
	if (ret == -1) {
		printf("EE: run cmd '%s' error!\n", cmd_buf);
		return ret;
	}
	part_sz = env_get(MMC_PART_SIZE_NAME);
	if (NULL == part_sz) {
		printf("EE: Can't get partition size of %s!\n", part_name);
		return -1;
	}
	part_blk_cnt = simple_strtoul(part_sz, NULL, 16);
	//if (ret != 1) {
	//	printf("EE: Can't get partitions size of %s in %s!\n", part_name, part_sz );
	//	return -1;
	//}

	src_blk_cnt = (src_size+(MMC_BLK_SIZE-1))/MMC_BLK_SIZE;
	if (part_blk_cnt < src_blk_cnt) {
		printf("EE: %s partition block number is %d(%x) < %d(%x) image size\n",
				part_name, part_blk_cnt, part_blk_cnt,
				src_blk_cnt, src_blk_cnt);
		return -1;
	}

	part_st = env_get(MMC_PART_START_NAME);
	if (NULL == part_st) {
		printf("EE: Can't get partition start of %s!\n", part_name);
		return -1;
	}

	sprintf(cmd_buf, "mmc write %x %s %x", src, part_st, src_blk_cnt);
	printf("-------\n%s: %s\n", part_name, cmd_buf);
	ret = run_command(cmd_buf, 0);
	if (ret == -1) {
		printf("EE: run cmd '%s' error!\n", cmd_buf);
		return ret;
	}
	return ret;
}

static int
do_mmc_clean(char *part_name)
{
	char cmd_buf[CMD_BUF_SZ] = {0};
	int ret;
	char *value = NULL;
	char *part_st = NULL;
	char *part_sz = NULL;
	unsigned int part_blk_cnt;
	unsigned int src_blk_cnt;

	snprintf(cmd_buf, sizeof(cmd_buf)-1,"partinfo info %s", part_name);
	ret = run_command(cmd_buf, 0);
	if (ret == -1) {
		printf("EE: run cmd '%s' error!\n", cmd_buf);
		return ret;
	}

	part_sz = env_get(MMC_PART_SIZE_NAME);
	if (NULL == part_sz) {
		printf("EE: Can't get partition size of %s!\n", part_name);
		return -1;
	}

	part_st = env_get(MMC_PART_START_NAME);
	if (NULL == part_st) {
		printf("EE: Can't get partition start of %s!\n", part_name);
		return -1;
	}

	sprintf(cmd_buf, "mmc erase %s %s",  part_st, part_sz);
	printf("-------\n%s: %s\n", part_name, cmd_buf);
	ret = run_command(cmd_buf, 0);
	if (ret == -1) {
		printf("EE: run cmd '%s' error!\n", cmd_buf);
		return ret;
	}
	return ret;
}
#undef MMC_PART_START_NAME
#undef MMC_PART_SIZE_NAME
#undef MMC_BLK_SIZE

#endif

static int do_flash_update(char *pf1_addr, char *pf1_size, unsigned int src, unsigned int src_size){
	char *endp;
	u32 part_base;
	u32 part_sz;
	char cmd_buf[CMD_BUF_SZ] = {0};
	char *value = NULL;
#if CONFIG_CMD_MMC
	if (update_emmc_flag) {
		return 0;
	}
#endif


	value = env_get(pf1_addr);
	if (NULL == value) {
		printf("EE: address base %s doesn't exist!\n", pf1_addr);
		return -1;
	}
	part_base = simple_strtoul(value, &endp, 16);
	value = env_get(pf1_size);
	if (NULL == value) {
		printf("EE: size %s doesn't exist!\n", pf1_size);
		return -1;
	}
	part_sz = simple_strtoul(value, &endp, 16);

	if (0 == part_sz) {
		printf("EE: size %s is 0 in writing!\n", pf1_size);
		return -1;
	}

	#ifdef CONFIG_SPINOR_FLASH
	sprintf(cmd_buf, "sf erase %x +%x; sf write %x %x %x", part_base, part_sz, src, part_base, src_size);
	#else
	sprintf(cmd_buf, "nand erase %x %x; nand write %x %x %x", part_base, part_sz, src, part_base, src_size);
	#endif
	if(src_size> part_sz) {
		printf("Error: image size is larger than the partition, operation aborted\n");
		return -1;
	}
	printf("CMD = %s\n", cmd_buf);
	run_command(cmd_buf, 0);
	return 0;

}

static int do_flash_update_part(char *part_name, unsigned int src, unsigned int src_size)
{
	int ret;
	char cmd_buf[CMD_BUF_SZ] = {0};
#if CONFIG_CMD_MMC
	if (update_emmc_flag) {
		return 0;
	}
#endif

#ifdef CONFIG_SPINOR_FLASH
	return -1; // nor platform does not support get mtd partition info
#endif

	#ifdef CONFIG_SPINOR_FLASH
	snprintf(cmd_buf, sizeof(cmd_buf)-1, "sf erase %s && nand write %x %s %x", part_name, src, part_name, src_size);
	#else
	snprintf(cmd_buf, sizeof(cmd_buf)-1, "nand erase.part %s && nand write %x %s %x", part_name, src, part_name, src_size);
	#endif
	printf("CMD = %s\n", cmd_buf);
	ret = run_command(cmd_buf, 0);
	return ret;
}

#if !CONFIG_RTK_USE_ONE_UBI_DEVICE
static int
do_flash_clean(char *pf1_addr, char *pf1_size)
{
	char *endp;
	u32 part_base, part_sz;
	char cmd_buf[CMD_BUF_SZ] = {0};
	char *value = NULL;

	value = env_get(pf1_addr);
	if (NULL == value) {
		printf("EE: address base %s doesn't exist!", pf1_addr);
		return -1;
	}
	part_base = simple_strtoul(value, &endp, 16);
	value = env_get(pf1_size);
	if (NULL == value) {
		printf("EE: size %s doesn't exist!", pf1_size);
		return -1;
	}
	part_sz = simple_strtoul(value, &endp, 16);

	if (0 == part_sz) {
		printf("EE: size %s is 0 in erasing!", pf1_size);
		return -1;
	}

#ifdef CONFIG_OTTO_SNOF
	sprintf(cmd_buf, "sf erase %x +%x", part_base, part_sz);
#else
	sprintf(cmd_buf, "nand erase %x %x", part_base, part_sz);
#endif
	printf("CMD=%s\n", cmd_buf);
	run_command(cmd_buf, 0);

	return 0;
}
#endif

static int
isblank(int c)
{
	return c == ' ' || c == '\t';
}
#ifndef strtoul
#define strtoul simple_strtoul
#endif
static char* trim(char *buf)
{
	int start=0;
	int index = 0;

	while (buf[index] && (isblank(buf[index])||buf[index] == '\n')){
		index++;
	}
	if (start == index) {
		while (buf[index] && !isblank(buf[index])&& buf[index] != '\n'){
			index++;
		}
		buf[index] = '\0';
	} else {
		while (buf[index] && !isblank(buf[index]) && buf[index] != '\n'){
			buf[start++] = buf[index++];
		}
		buf[start] = '\0';
	}
	return buf;
}

static int
do_volume_modify(char *buf, size_t buf_size)
{
	char ubi_name[128];
	char ubi_size[128];
	//int ret;
	char *endp;
	uint32_t size;
	size_t offset = 0;
	char *pat_ptr;
	char *str_ptr;
	int pat_len;
	str_ptr = buf;

	char cmd_buf[CMD_BUF_SZ] = {0};
	while(offset < buf_size)
	{
		pat_ptr = strchr(str_ptr, '=');
		if (NULL == pat_ptr) {
			break;
		}

		pat_len = (int)(pat_ptr - str_ptr);
		offset += pat_len+1;
		pat_len = (pat_len) > (sizeof(ubi_name)-1) ? sizeof(ubi_name)-1 : pat_len;
		memcpy(ubi_name, str_ptr, pat_len);
		ubi_name[pat_len] = '\0';
		trim(ubi_name);

		if (offset >= buf_size) {
			break;
		}

		str_ptr = pat_ptr + 1;
		pat_ptr = strchr(str_ptr, '\n');
		if (NULL == pat_ptr) {
			pat_len = buf_size - offset-1;
		} else {
			pat_len = (int)(pat_ptr - str_ptr);
		}
		offset += pat_len+1;
		pat_len = (pat_len) > (sizeof(ubi_name)-1) ? sizeof(ubi_name)-1 : pat_len;
		memcpy(ubi_size, str_ptr, pat_len);
		ubi_size[pat_len] = '\0';
		trim(ubi_size);
		str_ptr = pat_ptr + 1;

		size  = strtoul(ubi_size, &endp, 16);

		sprintf(cmd_buf, "setenv current_vol %s && run check_vol", ubi_name);
		printf("CMD=%s\n", cmd_buf);
		run_command(cmd_buf, 0);
		sprintf(cmd_buf, "ubi resize %s %x", ubi_name, size);
		printf("CMD=%s\n", cmd_buf);
		run_command(cmd_buf, 0);
	}

	return 0;
}

/*
	printf("key=0x%x\n", vmimg_hdr_p->key);
	printf("load_addr=0x%x\n", vmimg_hdr_p->load_addr);
	printf("img_sz=0x%x(%u)\n", vmimg_hdr_p->img_sz, vmimg_hdr_p->img_sz);
	printf("entry_addr=0x%x\n", vmimg_hdr_p->entry_addr);
	printf("flash_base=0x%x\n", vmimg_hdr_p->flash_base);
*/

void hdr_trans(void *img, struct vmimg_hdr *new_p){
	struct vmimg_hdr *hdr_p;
	hdr_p = (struct vmimg_hdr *) img;
	new_p->img_sz = ntohl(hdr_p->img_sz);
	new_p->key = ntohl(hdr_p->key);
	new_p->entry_addr = ntohl(hdr_p->entry_addr);
	new_p->flash_base = ntohl(hdr_p->flash_base);
}

static int
do_env_image(void *img)
{
	struct vmimg_hdr hdr;
	hdr_trans(img, &hdr);
	if(hdr.key == VMIMG_BOOTENV_KEY) {
		printf("\nBOOT ENV image key consistent, start updating...\n");
	} else {
		printf("\nError: BOOT ENV key mismatch, key found in image is 0x%08x, but should be 0x%08x\n", hdr.key, VMIMG_BOOTENV_KEY);
		printf("Operation aborted\n");
		goto bootenv_error;
	}
#if CONFIG_CMD_MMC
	if(update_emmc_flag) {
			do_mmc_update(MMC_ENV_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	}
#endif
	if (do_flash_update_part("env", (unsigned int)(img + sizeof(hdr)), hdr.img_sz)) {
		// try use address and size to update
		if( do_flash_update("pfl_bootenv1", "pfl_bootenv1_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
			goto bootenv_error;
		}
	}
#ifdef CONFIG_ENV_OFFSET_REDUND
#if CONFIG_CMD_MMC
	if(update_emmc_flag) {
			do_mmc_update(MMC_ENV2_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	}
#endif
	if (do_flash_update_part("env2", (unsigned int)(img + sizeof(hdr)), hdr.img_sz)) {
		// try use address and size to update
		if( do_flash_update("pfl_bootenv2", "pfl_bootenv2_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
			goto bootenv_error;
		}
	}
#endif

  update_env_flag = 1;
	return hdr.img_sz;
bootenv_error:
	return -1;
}

static int
do_fpga_image(void *img)
{
	struct vmimg_hdr hdr;
	hdr_trans(img, &hdr);
	if(hdr.key == VMIMG_FPGA_KEY) {
		printf("\nFPGA image key consistent, start updating...\n");
	} else {
		printf("\nError: VMIMG_FPGA_KEY key mismatch, key found in image is 0x%08x, but should be 0x%08x\n", hdr.key, VMIMG_FPGA_KEY);
		printf("Operation aborted\n");
		goto fpga_error;
	}
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	int ret = do_ubi_update(UBI_FPGA_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	if (ret) goto fpga_error;

#else
	if (do_flash_update_part(MTD_FPGA_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz)) {
		if( do_flash_update("pfl_fpgafs", "pfl_fpgafs_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
			printf("Flash Update Operation aborted\n");
			goto fpga_error;
		}
	}
#endif
	return hdr.img_sz;
fpga_error:
	return -1;
}


static int
do_volume_image(void *img)
{
	struct vmimg_hdr hdr;
	hdr_trans(img, &hdr);
	if(hdr.key == VMIMG_VOLUME_KEY) {
		printf("\nVOLUME image key consistent, start updating...\n");
	} else {
		printf("\nError: VOLUME_KEY key mismatch, key found in image is 0x%08x, but should be 0x%08x\n", hdr.key, VMIMG_VOLUME_KEY);
		printf("Operation aborted\n");
		goto volume_error;
	}
	printf("addr = %x, size is %x\n", (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	char *img_ptr = img + sizeof(hdr);
	for (int i = 0; i < hdr.img_sz; ++i) {
		putc(*img_ptr);
		img_ptr++;
	}
	printf("\n-----------\n" );
	do_volume_modify( (char*)(unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	return hdr.img_sz;
volume_error:
	return -1;
}

static int
do_dtb_image(void *img)
{
	struct vmimg_hdr hdr;
	hdr_trans(img, &hdr);
	if(hdr.key == VMIMG_DTB_KEY) {
		printf("\nDTB image key consistent, start updating...\n");
	} else {
		printf("\nError: DTB key mismatch, key found in image is 0x%08x, but should be 0x%08x\n", hdr.key, VMIMG_DTB_KEY);
		printf("Operation aborted\n");
		goto dtb_error;
	}
#if CONFIG_CMD_MMC
	if(update_emmc_flag) {
			do_mmc_update(MMC_DTB0_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#ifdef CONFIG_LUNA_MULTI_BOOT
			do_mmc_update(MMC_DTB1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#endif
	}
#endif

#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_DTB0_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#ifdef CONFIG_LUNA_MULTI_BOOT
	do_ubi_update(UBI_DTB1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#endif
#else
	if( do_flash_update("pfl_fdt1", "pfl_fdt1_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		printf("Flash Update Operation aborted\n");
		goto dtb_error;
	}

#ifdef CONFIG_LUNA_MULTI_BOOT
	if( do_flash_update("pfl_fdt2", "pfl_fdt2_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto dtb_error;
	}
#endif
#endif

	return hdr.img_sz;
dtb_error:
	return -1;
}


static int
do_kernel_image(void *img)
{
	struct vmimg_hdr hdr;
	//struct vmimg_hdr *hdr_p1;

	//hdr_p1 = (struct vmimg_hdr *)img;
	hdr_trans(img, &hdr);
	if(hdr.key == VMIMG_KERNEL_KEY) {
		printf("\nKernel image key consistent, start updating...\n");
	} else {
		printf("\nError: Kernel key mismatch, key found in image is 0x%08x, but should be 0x%08x\n", hdr.key, VMIMG_KERNEL_KEY);
		printf("Operation aborted\n");
		goto kernel_error;
	}
#if CONFIG_CMD_MMC
	if(update_emmc_flag) {
			do_mmc_update(MMC_K0_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#ifdef CONFIG_LUNA_MULTI_BOOT
			do_mmc_update(MMC_K1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#endif
	}
#endif
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_K0_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#ifdef CONFIG_LUNA_MULTI_BOOT
	do_ubi_update(UBI_K1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#endif
#else
	if( do_flash_update("pfl_kernel1", "pfl_kernel1_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto kernel_error;
	}
#ifdef CONFIG_LUNA_MULTI_BOOT
	if( do_flash_update("pfl_kernel2", "pfl_kernel2_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto kernel_error;
	}
#endif
#endif

	return hdr.img_sz;
kernel_error:
	return -1;
}

static int
do_rootfs_image(void *img)
{
	struct vmimg_hdr hdr;
	hdr_trans(img, &hdr);
	if(hdr.key == VMIMG_ROOTFS_KEY) {
		printf("\nROOTFS image key consistent, start updating...\n");
	} else {
		printf("\nError: ROOTFS key mismatch, key found in image is 0x%08x, but should be 0x%08x\n", hdr.key, VMIMG_ROOTFS_KEY);
		printf("Operation aborted\n");
		goto rootfs_error;
	}

#if CONFIG_CMD_MMC
	if(update_emmc_flag) {
			do_mmc_update(MMC_R0_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#ifdef CONFIG_LUNA_MULTI_BOOT
			do_mmc_update(MMC_R1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#endif
	}
#endif
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_R0_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#ifdef CONFIG_LUNA_MULTI_BOOT
	do_ubi_update(UBI_R1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#endif
#else
	if( do_flash_update("pfl_rootfs1", "pfl_rootfs1_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto rootfs_error;
	}

#ifdef CONFIG_LUNA_MULTI_BOOT
	if( do_flash_update("pfl_rootfs2", "pfl_rootfs2_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto rootfs_error;
	}
#endif
#endif

	return hdr.img_sz;
rootfs_error:
	return -1;
}

static int
do_osgi_image(void *img)
{
	struct vmimg_hdr hdr;
	printf("\n\n====== [OSGI] Image is Loacted at %p ======\n", img);
	hdr_trans(img, &hdr);
	if(hdr.key == VMIMG_OSGI_KEY) {
		printf("\nOSGI image key consistent, start updating...\n");
	}
#if CONFIG_CMD_MMC
	if(update_emmc_flag) {
			do_mmc_update(MMC_OSGI_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	}
#endif
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	int ret = do_ubi_update(UBI_OSGI_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	if (ret) goto osgi_error;
#else  // if CONFIG_RTK_USE_ONE_UBI_DEVICE
	if( do_flash_update("pfl_osgi", "pfl_osgi_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto osgi_error;
	}
#endif

	return hdr.img_sz;
osgi_error:
	return -1;
}


static int
do_application_image(void *img)
{
	struct vmimg_hdr hdr;
	int ret;
	char cmd_buf[CMD_BUF_SZ] = {0};
	printf("\n\n====== [App] Image is Loacted at %p ======\n", img);
	hdr_trans(img, &hdr);
	if(hdr.key == VMIMG_APPLICATION_KEY) {
		printf("\nApplication image key consistent, start updating...\n");
	}
#if CONFIG_CMD_MMC
	if(update_emmc_flag) {
			ret = do_mmc_update(MMC_FRAMEWORK1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
			if (ret) goto application_error;
			ret = do_mmc_update(MMC_FRAMEWORK2_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
			if (ret) goto application_error;
			ret = do_mmc_clean(MMC_APP_NAME);
			if (ret) goto application_error;
	}
#endif
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	ret = do_ubi_update(UBI_FRAMEWORK1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	if (ret) goto application_error;
	ret = do_ubi_update(UBI_FRAMEWORK2_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	if (ret) goto application_error;
	ret = do_ubi_clean(UBI_APP_NAME);
	if (ret) goto application_error;
#else  // if CONFIG_RTK_USE_ONE_UBI_DEVICE
	if( do_flash_update("pfl_framework1", "pfl_framework1_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto application_error;
	}
	if( do_flash_update("pfl_framework2", "pfl_framework2_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto application_error;
	}
	if (do_flash_clean("pfl_apps", "pfl_apps_sz")) {
		goto application_error;
	}
#endif

	sprintf(cmd_buf, "run erase_apps");
	ret = run_command(cmd_buf, 0);

	return hdr.img_sz;
application_error:
	return -1;
}

static int
do_province_image(void *img)
{
	//int ret;
	int dirty;
	struct vmimg_hdr hdr;
	char *old_cfg_prov;
	char new_cfg_prov[CFG_PROV_SZ+1] = {0};
	int prov_len;
	hdr_trans(img, &hdr);
	printf("\n\n====== [Config Province] Image is Loacted at %p (%d) ======\n", img, hdr.img_sz);
	//prov_len = min(hdr.img_sz, CFG_PROV_SZ);
	prov_len = (hdr.img_sz > CFG_PROV_SZ)?CFG_PROV_SZ:hdr.img_sz;
	strncpy(new_cfg_prov, (char *)(img + sizeof(hdr)), prov_len);
	new_cfg_prov[prov_len] = '\0';
	strncpy(province_cfg, (char *)(img + sizeof(hdr)), prov_len);
  province_cfg[prov_len] = '\0';

	printf("Update cfg_province: [%s]\n", new_cfg_prov);
	old_cfg_prov = env_get(ENV_CONFIG_PROVINCE);
	if(old_cfg_prov == NULL){
		printf("%s empty, set %s\n",ENV_CONFIG_PROVINCE, new_cfg_prov);
		env_set(ENV_CONFIG_PROVINCE, new_cfg_prov);
    env_dirty_flag      = 1;
    update_provice_flag = 1;
	} else {
		if(strcmp(old_cfg_prov, new_cfg_prov)!=0){
			printf("old %s [%s] is different to new [%s], set new version\n", ENV_CONFIG_PROVINCE, old_cfg_prov, new_cfg_prov);
			env_set(ENV_CONFIG_PROVINCE, new_cfg_prov);
      env_dirty_flag      = 1;
      update_provice_flag = 1;
		} //else
		//old and new version are the same, don't save env!
	}


	return hdr.img_sz;

}

extern u32 swp_spi_nand_page_size(void);
static int
do_boot_image(void *img)
{
	char *value;
	struct vmimg_hdr hdr;
	char *endp;
	char cmd_buf[CMD_BUF_SZ] = {0};
	u32 part_base, part_sz;
	printf("\n\n====== [BOOT] Image is Loacted at %p ======\n", img);

	hdr_trans(img, &hdr);
	part_base = 0;
	value = env_get("pfl_boot_sz");
	if (NULL == value) {
		goto boot_error;
	}
	part_sz = simple_strtoul(value, &endp, 16);
	printf("boot partition at 0x%08x, size=0x%08x\n", part_base, part_sz);
	printf("boot image size=0x%08x(%u)\n", hdr.img_sz, hdr.img_sz);
	if(hdr.img_sz > part_sz) {
		printf("Error: image size is larger than the partition, operation aborted\n");
		goto boot_error;
	}

#ifdef CONFIG_CMD_MMC
	if (update_emmc_flag) goto END_UPDATE_FLASH_BOOT;
#endif


#ifdef CONFIG_OTTO_SNOF
	sprintf(cmd_buf, "sf erase %x +%x;sf write %x %x %x", part_base, part_sz, (unsigned int)(img + sizeof(hdr)), part_base, hdr.img_sz);
#else
#if  (defined CONFIG_TARGET_RTL9603CVD) || (defined CONFIG_TARGET_VD2) ||  (defined CONFIG_TARGET_PHOEBUS)
	if (2048 == swp_spi_nand_page_size()) {
		sprintf(cmd_buf, "nand erase %x %x && setexpr pagecnt %x / 840 && nand write.raw %x 0x0 ${pagecnt}", part_base, part_sz, hdr.img_sz, (unsigned int)(img + sizeof(hdr)));
	} else if (4096 == swp_spi_nand_page_size()) {
		sprintf(cmd_buf, "nand erase %x %x && setexpr pagecnt %x / 1080 && nand write.raw %x 0x0 ${pagecnt}", part_base, part_sz, hdr.img_sz, (unsigned int)(img + sizeof(hdr)));
	}
#else
	sprintf(cmd_buf, "nand erase %x %x && nand write %x %x %x", part_base, part_sz, (unsigned int)(img + sizeof(hdr)), part_base, hdr.img_sz);
#endif
#endif
#ifdef CONFIG_CMD_MMC
END_UPDATE_FLASH_BOOT:
#endif

#ifdef CONFIG_CMD_MMC
	if (update_emmc_flag) {
		sprintf(cmd_buf, "mmc write %x 0 %x", (unsigned int)(img + sizeof(hdr)), (hdr.img_sz+0x1FF)/0x200);
		printf("CMD=%s\n", cmd_buf);
		run_command(cmd_buf, 0);

		// try rebuild mmc partitions
		value = env_get("chk_wr_parts");
		if (NULL != value){
			sprintf(cmd_buf, "run chk_wr_parts");
			printf("CMD=%s\n", cmd_buf);
			run_command(cmd_buf, 0);
		}

	} else {
		printf("CMD=%s\n", cmd_buf);
		run_command(cmd_buf, 0);
	}
#else
	printf("CMD=%s\n", cmd_buf);
	run_command(cmd_buf, 0);
#endif

#if 0
	// Upgrage boot also reset default cs
	env_set(ENV_RESET_DEFAULT, "1");

	// Sync new bootloader default env in next boot up
	env_set(ENV_SYNC_DEFAULT, "1");
	//saveenv();
  env_dirty_flag   = 1;
  update_boot_flag = 1;
#endif

	return hdr.img_sz;
boot_error:
	return -1;
}

#if CONFIG_RTK_USE_ONE_UBI_DEVICE
#define UBI_READ_LEN_NAME "ubi_len_read"
static int
do_ubi_device_image(void *img)
{
	int ret;
	char cmd_buf[CMD_BUF_SZ] = {0};
	struct vmimg_hdr hdr;
	int backup_config_flag = 0;
	unsigned int config_back_address = -1;
	char *ubi_read_len = NULL;

	hdr_trans(img, &hdr);
	if(hdr.key == VMIMG_UBI_DEVICE_KEY) {
		printf("\nUBI Device image key consistent, start updating...\n");
	} else {
		printf("\nError: UBI Device key mismatch, key found in image is 0x%08x, but should be 0x%08x\n", hdr.key, VMIMG_UBI_DEVICE_KEY);
		printf("Operation aborted\n");
		goto ubi_device_image_error;
	}

	snprintf(cmd_buf, sizeof(cmd_buf), "setenv current_vol %s", UBI_CONFIG_NAME);
	ret = run_command(cmd_buf, 0);
	sprintf(cmd_buf, "run check_vol");
	ret = run_command(cmd_buf, 0);
	if (ret == -1) {
	} else {
		backup_config_flag = 1;
		env_set(UBI_READ_LEN_NAME, "");
		//backup config patition
		config_back_address = (unsigned int)(img + sizeof(hdr) + hdr.img_sz);

		snprintf(cmd_buf, sizeof(cmd_buf), "ubi read %x %s", config_back_address, UBI_CONFIG_NAME);
		printf("CMD = %s\n", cmd_buf);
		ret = run_command(cmd_buf, 0);
		if (ret == -1) {
			backup_config_flag = 0;
			printf("\nRead config partition %s fail!\n", UBI_CONFIG_NAME);
		}

		ubi_read_len = env_get(UBI_READ_LEN_NAME);
		if (NULL == ubi_read_len) {
			backup_config_flag = 0;
			printf("\nCan't get read length '%s' of config volume %s\n", UBI_READ_LEN_NAME, UBI_CONFIG_NAME);
		}

		sprintf(cmd_buf, "ubi detach");
		ret = run_command(cmd_buf, 0);
	}

	if (do_flash_update_part(UBI_MAIN_DEVICE_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz ) ) {
		if( do_flash_update("pfl_ubi_device", "pfl_ubi_device_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz ) ){
			goto ubi_device_image_error;
		}
	}

	if( backup_config_flag ) {
		snprintf(cmd_buf, sizeof(cmd_buf), "setenv current_vol %s", UBI_CONFIG_NAME);
		ret = run_command(cmd_buf, 0);
		sprintf(cmd_buf, "run check_vol");
		ret = run_command(cmd_buf, 0);

		if (ret == -1) {
			printf("Not %s found! Config backup fail!\n", UBI_CONFIG_NAME);
		} else {

			//backup config patition

			snprintf(cmd_buf, sizeof(cmd_buf), "ubi write %x %s %s", config_back_address, UBI_CONFIG_NAME, ubi_read_len);
			printf("CMD = %s\n", cmd_buf);
			ret = run_command(cmd_buf, 0);

			sprintf(cmd_buf, "ubi detach");
			ret = run_command(cmd_buf, 0);

		}
	}

	return hdr.img_sz;
ubi_device_image_error:
	return -1;

}
#endif

#define WRITE_TO_MMC_FLAG "fw_mmc"
static int do_upvmimg(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	char *endp;
	u8 *img;
	char cmd_buf[CMD_BUF_SZ] = {0};
	struct vmimg_hdr hdr;
	int prev_image_sz = 0; // -1 => error

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	/* Retrieve vmimg location */
	img = (u8 *)simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		return -1;
	printf("vmimg is located at %p\n", img);

#if CONFIG_CMD_MMC
	char *value = env_get(WRITE_TO_MMC_FLAG);
	if (NULL == value) {
		update_emmc_flag = 0;
		printf("%s is NULL!\n", WRITE_TO_MMC_FLAG);
	} else {
		printf("%s is \'%s\'\n", WRITE_TO_MMC_FLAG, value );
		if (simple_strtoul(value, &endp, 16)) {
			update_emmc_flag = 1;
		} else {
			update_emmc_flag = 0;
		}
	}
#endif

#ifdef CONFIG_SPINOR_FLASH
	sprintf(cmd_buf, "sf probe");
	printf("CMD = %s\n", cmd_buf);
	run_command(cmd_buf, 0);
#endif

	/****** vm.img *******************
	*** HDR|DTB|HDR|Kernel|HDR|ROOTFS
	**********************************/

	while(1) {

		int unknownImg = 0;
		hdr_trans(img, &hdr);
		dump_vmimg_hdr(&hdr);
		if (unknownImg) break;
		switch (hdr.key) {
		case VMIMG_BOOTENV_KEY:
			prev_image_sz = do_env_image(img);
			break;
		case VMIMG_FPGA_KEY:
			prev_image_sz = do_fpga_image(img);
			break;
		case VMIMG_VOLUME_KEY:
			prev_image_sz = do_volume_image(img);
			break;
		case VMIMG_DTB_KEY:
			prev_image_sz = do_dtb_image(img);
			break;
		case VMIMG_BOOT_KEY:
			prev_image_sz = do_boot_image(img);
			break;
		case VMIMG_KERNEL_KEY:
			prev_image_sz = do_kernel_image(img);
			break;
		case VMIMG_ROOTFS_KEY:
			prev_image_sz = do_rootfs_image(img);
			break;
		case VMIMG_CONFIG_KEY:
			printf("\n\n====== [Config] Image is Loacted at %p ======\n", img);
			printf("Config Image is NOT support!!!\n");
			prev_image_sz = hdr.img_sz;
			break;
		case VMIMG_CFGPROV_KEY:
			prev_image_sz = do_province_image(img);
			break;
		case VMIMG_APPLICATION_KEY:
			prev_image_sz = do_application_image(img);
			break;
		case VMIMG_OSGI_KEY:
			prev_image_sz = do_osgi_image(img);
			break;
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
		case VMIMG_UBI_DEVICE_KEY:
			prev_image_sz = do_ubi_device_image(img);
			break;
#endif
		default:
			printf("Unknown Key (%08X) is Loacted at %p\n", hdr.key, img);
			unknownImg = 1;
			prev_image_sz = -1;

			break;
		}
		if(prev_image_sz < 0){
			goto done;
		}

		img = (u8 *)(img + sizeof(struct vmimg_hdr) + prev_image_sz);
	}
done:
  if (update_env_flag) {
    env_load();
    if ( update_boot_flag){
      // Upgrage boot also reset default cs
      env_set(ENV_RESET_DEFAULT, "1");

      // Sync new bootloader default env in next boot up
      env_set(ENV_SYNC_DEFAULT, "1");

      env_dirty_flag = 1;
    }
    if (update_provice_flag) {
			env_set(ENV_CONFIG_PROVINCE, province_cfg);
      env_dirty_flag = 1;
    }
  }

/***** Remove env parameters *******/
#define ENV_SW_VERSION0		"sw_version0"
#define ENV_SW_VERSION1		"sw_version1"
#define ENV_SW_CRC0		"sw_crc0"
#define ENV_SW_CRC1		"sw_crc1"
	env_set(ENV_SW_VERSION0, NULL );
	env_set(ENV_SW_VERSION1, NULL );
	env_set(ENV_SW_CRC0, NULL );
	env_set(ENV_SW_CRC1, NULL );
  env_dirty_flag = 1;

  if(env_dirty_flag){
    env_save();
  }

	printf("Update vm.img Done\n");
	if (ret != -1)
		return ret;

usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	upvmimg,	2,	1,	do_upvmimg,
	"update kernel and rootfs by vmimg format on luna platform",
	"ADDRESS\n"
	"\n"
	"Description:\n"
	"Extract kernel and rootfs in vmimg image located at ADDRESS and write them into corresponding partitions.\n"
);
