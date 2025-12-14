/*
 * Command for updating vm.img format image (containing uImage and rootfs) into corresponding partition on luna platform.
 * Author: bohungwu@realtek.com
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <asm/io.h>
#include <u-boot/crc.h>
#include <linux/ctype.h>
#include <misc_setting.h>
//#include <environment.h>

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
#endif

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

const u32 vmimg_key_list[] = {
	VMIMG_BOOT_KEY,
	VMIMG_DTB_KEY,
	VMIMG_KERNEL_KEY,
	VMIMG_ROOTFS_KEY,
	VMIMG_CONFIG_KEY,
	VMIMG_CFGPROV_KEY,
	VMIMG_APPLICATION_KEY,
	VMIMG_OSGI_KEY,
	VMIMG_BOOTENV_KEY,
	VMIMG_VOLUME_KEY,
	VMIMG_UBI_DEVICE_KEY,
	0
};


#define ENV_CONFIG_PROVINCE	"cfg_province"
#define ENV_RESET_DEFAULT	"rst2dfl"
#define ENV_SYNC_DEFAULT	"sync_default_env"
#define CFG_PROV_SZ		(8)		/* Maximum Config Province Code */


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

static int do_flash_update(char *pf1_addr, char *pf1_size, unsigned int src, unsigned int src_size){
	char *endp;
	u32 part_base;
	u32 part_sz;
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
		printf("EE: size %s is 0 in writing!", pf1_size);
		return -1;
	}

	#ifdef CONFIG_OTTO_SNOF
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

	snprintf(cmd_buf, sizeof(cmd_buf)-1, "nand erase.part %s && nand write %x %s %x", part_name, src, part_name, src_size);
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
/*
static int
isblank(int c)
{
	return c == ' ' || c == '\t';
}
*/
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
	memcpy( new_p, img, sizeof(struct vmimg_hdr));
	hdr_p = new_p;
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
	if (do_flash_update_part("env", (unsigned int)(img + sizeof(hdr)), hdr.img_sz)) {
		// try use address and size to update
		if( do_flash_update("fl_bootenv1", "fl_bootenv1_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
			goto bootenv_error;
		}
	}
#ifdef CONFIG_ENV_OFFSET_REDUND
	if (do_flash_update_part("env2", (unsigned int)(img + sizeof(hdr)), hdr.img_sz)) {
		// try use address and size to update
		if( do_flash_update("fl_bootenv2", "fl_bootenv2_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
			goto bootenv_error;
		}
	}
#endif

	return hdr.img_sz;
bootenv_error:
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

#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_DTB0_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#ifdef CONFIG_LUNA_MULTI_BOOT
	do_ubi_update(UBI_DTB1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#endif
#else
	if( do_flash_update("fl_fdt1", "fl_fdt1_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		printf("Flash Update Operation aborted\n");
		goto dtb_error;
	}

#ifdef CONFIG_LUNA_MULTI_BOOT
	if( do_flash_update("fl_fdt2", "fl_fdt2_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
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
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_K0_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#ifdef CONFIG_LUNA_MULTI_BOOT
	do_ubi_update(UBI_K1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#endif
#else
	if( do_flash_update("fl_kernel1", "fl_kernel1_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto kernel_error;
	}
#ifdef CONFIG_LUNA_MULTI_BOOT
	if( do_flash_update("fl_kernel2", "fl_kernel2_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
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

#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_R0_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#ifdef CONFIG_LUNA_MULTI_BOOT
	do_ubi_update(UBI_R1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
#endif
#else
	if( do_flash_update("fl_rootfs1", "fl_rootfs1_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto rootfs_error;
	}

#ifdef CONFIG_LUNA_MULTI_BOOT
	if( do_flash_update("fl_rootfs2", "fl_rootfs2_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
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
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	int ret = do_ubi_update(UBI_OSGI_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	if (ret) goto osgi_error;
#else  // if CONFIG_RTK_USE_ONE_UBI_DEVICE
	if( do_flash_update("fl_osgi", "fl_osgi_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
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
	printf("\n\n====== [App] Image is Loacted at %p ======\n", img);
	hdr_trans(img, &hdr);
	if(hdr.key == VMIMG_APPLICATION_KEY) {
		printf("\nApplication image key consistent, start updating...\n");
	}
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	int ret = do_ubi_update(UBI_FRAMEWORK1_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	if (ret) goto application_error;
	ret = do_ubi_update(UBI_FRAMEWORK2_NAME, (unsigned int)(img + sizeof(hdr)), hdr.img_sz);
	if (ret) goto application_error;
	ret = do_ubi_clean(UBI_APP_NAME);
	if (ret) goto application_error;
#else  // if CONFIG_RTK_USE_ONE_UBI_DEVICE
	if( do_flash_update("fl_framework1", "fl_framework1_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto application_error;
	}
	if( do_flash_update("fl_framework2", "fl_framework2_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz) !=0 ){
		goto application_error;
	}
	if (do_flash_clean("fl_apps", "fl_apps_sz")) {
		goto application_error;
	}
#endif

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
	printf("Update cfg_province: [%s]\n", new_cfg_prov);
	old_cfg_prov = env_get(ENV_CONFIG_PROVINCE);
	if(old_cfg_prov == NULL){
		printf("%s empty, set %s\n",ENV_CONFIG_PROVINCE, new_cfg_prov);
		env_set(ENV_CONFIG_PROVINCE, new_cfg_prov);
		dirty = 1;
	} else {
		if(strcmp(old_cfg_prov, new_cfg_prov)!=0){
			printf("old %s [%s] is different to new [%s], set new version\n", ENV_CONFIG_PROVINCE, old_cfg_prov, new_cfg_prov);
			env_set(ENV_CONFIG_PROVINCE, new_cfg_prov);
			dirty = 1;
		} //else
		//old and new version are the same, don't save env!
	}

	if (dirty)	env_save();

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
	u32 part_base, part_sz, part_sz_raw ;
	printf("\n\n====== [BOOT] Image is Loacted at %p ======\n", img);

	hdr_trans(img, &hdr);
	part_base = 0;
	value = env_get("fl_boot_sz");
	if (NULL == value) {
		goto boot_error;
	}
	part_sz = simple_strtoul(value, &endp, 16);
	part_sz_raw = part_sz;
#ifndef CONFIG_OTTO_SNOF
#if  (defined CONFIG_TARGET_RTL9603CVD) || (defined CONFIG_TARGET_VD2) ||  (defined CONFIG_TARGET_PHOEBUS)
	if (2048 == swp_spi_nand_page_size()) {
		part_sz_raw = (part_sz/2048)*(2048+64);
	} else if (4096 == swp_spi_nand_page_size()) {
		part_sz_raw = (part_sz/4096)*(4096+128);
	}

	printf("boot partition at 0x%08x, size=0x%08x\n", part_base, part_sz_raw);
#endif
#else
	printf("boot partition at 0x%08x, size=0x%08x\n", part_base, part_sz);
#endif
	printf("boot image size=0x%08x(%u)\n", hdr.img_sz, hdr.img_sz);
	if(hdr.img_sz > part_sz_raw) {
		printf("Error: image size is larger than the partition, operation aborted\n");
		goto boot_error;
	}

#ifdef CONFIG_OTTO_SNOF
	sprintf(cmd_buf, "sf erase %x +%x;sf write.raw %x %x %x", part_base, part_sz, (unsigned int)(img + sizeof(hdr)), part_base, hdr.img_sz);
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
	printf("CMD=%s\n", cmd_buf);
	run_command(cmd_buf, 0);

#if 0
	// Upgrage boot also reset default cs
	env_set(ENV_RESET_DEFAULT, "1");

	// Sync new bootloader default env in next boot up
	env_set(ENV_SYNC_DEFAULT, "1");
	saveenv();
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
		if( do_flash_update("fl_ubi_device", "fl_ubi_device_sz", (unsigned int)(img + sizeof(hdr)), hdr.img_sz ) ){
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

#ifdef CONFIG_FIRMWARE_UPGRADE_MISTAKE_PROOFING

static void strtolower(char *str, int len)
{
    int i;
    for (i = 0; i<len; i++) {
        str[i] = tolower(str[i]);
    }
}

#define ODM_LEN 16
/* board odm id and name struct */
struct param_odm {
	unsigned int odm_id;
	unsigned char odm_name[ODM_LEN];    
};

static struct param_odm ODM_TBL[] = {
	{0,  "unified"    },
	{1,  "cmdc"       },
	{2,  "dongzhi"    },
	{3,  "mengchuang" },     //raisecom
	{4,  "raisecom"   },
	{5,  "unionman"   },
};

static int getODMNameById(unsigned int odmid, char* odmname)
{
	int c;
	for(c=0; c< sizeof(ODM_TBL)/sizeof(struct param_odm); c++){
		if(ODM_TBL[c].odm_id==odmid){
			memcpy(odmname, ODM_TBL[c].odm_name, ODM_LEN);
			return 1;
		}
	}
	return 0;
}

/*
fw whitelist format :
wl@start:soctype=...;manufacture=...;devtype=...;custom=...;province=...;wl@endxxxxxx(crc32result8位16进制数)
*/
static int checkFwWhiteList(char* headbuf, unsigned int headsize)
{
   	char soctype[64] =  {0};       //diag debug get version|tail -n 2 |head -n 1
   	char manufacturer[ODM_LEN] =  {0};  //mib get MIB_HW_BOARD_ID(bit 16~19  ((hw_boardid >> 16 ) & 0xf)
	char* devtype=NULL;            //cfg_devtype
	char* custom=NULL;             //cfg_custom
    char* province=NULL;           //cfg_province
	char* boardid=NULL;
	unsigned long hw_boardid=0;
    char* pstr=NULL;
    char check_soctype=0,check_manufacturer=0,check_devtype=0,check_custom=0,check_province=0;
	
	if ((OTTO_SID == PLR_SID_APRO) || (OTTO_SID == PLR_SID_APRO_GEN2)) {
		switch(OTTO_CID) {
		case ST_RTL9603C:
			memcpy(soctype,"RTL9603C",sizeof(soctype));
			break;			  
		case ST_RTL9607C:
			memcpy(soctype,"RTL9607C",sizeof(soctype));
			break;			  
		case ST_RTL9606C:
			memcpy(soctype,"RTL9606C",sizeof(soctype));
			break;			  
		default: 
			memcpy(soctype,"Unknown",sizeof(soctype));
			break;            
		}
	}else if (OTTO_SID == PLR_SID_9603CVD)
		memcpy(soctype,"RTL9603CVD",sizeof(soctype));

	strtolower(soctype,sizeof(soctype));

   	boardid = env_get("bootargs_boardid");
   	devtype =  env_get("env_devtype");
   	custom = env_get("env_custom");
   	province = env_get("env_province");

	hw_boardid = simple_strtoul(boardid, NULL, 10);
	
	if(getODMNameById((hw_boardid >> 16 ) & 0xf,manufacturer)==0){
		printf("%s:%d can't find ODM name by ODM_id(%lu)\n",__FUNCTION__,__LINE__,(hw_boardid >> 16 ) & 0xf);
		return -1;
	}

	if((strlen(soctype)==0) || (manufacturer==NULL) || (devtype==NULL) || (custom==NULL) || (province==NULL)){
		printf("%s:%d This device hasn't white list env info!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	//printf("%s:%d soctype=%s manufacturer=%s devtype=%s custom=%s province=%s\n",__FUNCTION__,__LINE__,soctype,manufacturer,devtype,custom,province);
    while((pstr=strtok(headbuf,";"))!=NULL ) {        
        if(pstr){
            //printf("\n%s:%d pstr=%s\n",__FUNCTION__,__LINE__,pstr);
			if(strstr(pstr,"soctype") && (strstr(pstr,soctype)||strstr(pstr,"ALL")))
				check_soctype=1;
			if(strstr(pstr,"manufacturer") && (strstr(pstr,manufacturer)||strstr(pstr,"ALL")))
				check_manufacturer=1;
			if(strstr(pstr,"devtype") && (strstr(pstr,devtype)||strstr(pstr,"ALL")))
				check_devtype=1;
			if(strstr(pstr,"custom") && (strstr(pstr,custom)||strstr(pstr,"ALL")))
				check_custom=1;
			if(strstr(pstr,"province") && (strstr(pstr,province)||strstr(pstr,"ALL")))
				check_province=1;	 
        }
        headbuf=NULL; 
    }

    
    if(check_soctype&&check_manufacturer&&check_devtype&&check_custom&&check_province){
        return 1;
    }else{
        printf("%s:%d check failure %d %d %d %d %d \n",__FUNCTION__,__LINE__,check_soctype,check_manufacturer,check_devtype,check_custom,check_province);
        return -1;
    }
        
}

/*
 *   FW whitelist header format :
 *   wl@start:soctype=...;manufacture=...;devtype=...;custom=...;province=...;wl@endxxxxxxxx(crc32result8位16进制数)
 *
 *	Check the whitelist on the head of firmware
 *	Return:
 *		 0  no exist whitelist header
 		 -1 for Fw whitelist exist but verify failure
 		 =headsize>0 for FW whitelist exist and verify sucess
 */

static int checkFwWhitelistHead(u8 *img)
{
    char headbuf[2048]= {0}, *pstr=NULL;
	int ret=0,headsize=0;
	unsigned long crc = crc32(0L, NULL, 0);
	char str_crcSum[16];
	char *strbuf = (char *)img;

	if(!strncmp(strbuf, "wl@start:", 9)){
		ret=-1;
		if((pstr = strstr(strbuf+9, "wl@end"))!=NULL){
		
			pstr=pstr+strlen("wl@end")-1;
			do{
				pstr++;
			}
			while(*pstr=='\0');
			headsize=pstr-strbuf;
			memcpy(headbuf, strbuf, headsize <(sizeof(headbuf)-1)?headsize:sizeof(headbuf)-1);
			headbuf[headsize] = '\0';	 //head +'\0'
			
			//check head crc32
			crc = crc32(crc, (unsigned char *)strbuf, headsize);
			sprintf(str_crcSum, "%08lx",crc);
			if(!memcmp(str_crcSum, pstr, 8)){
				printf("\n head check ok, str_crcsum=%s\n",str_crcSum);
				if(checkFwWhiteList(headbuf,headsize)>0){
					ret=headsize+8;
					printf("\n firmware whitelist head check ok ,fw offsize =%d\n",ret);
				}else{
					printf("\n firmware whitelist head check failed \n");
				}
			}else{
				printf("\n head check failure, str_crcsum=%s\n",str_crcSum);
			}
		}
	}
    return ret;
}

#endif

static int do_upvmimg(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	int index = 0;
	char *endp;
	u8 *img;
	//char cmd_buf[CMD_BUF_SZ] = {0};
	struct vmimg_hdr hdr;
	//u32 part_base;
	//u32 part_sz;
	int prev_image_sz = 0; // -1 => error

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	/* Retrieve vmimg location */
	img = (u8 *)simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		return -1;
	printf("vmimg is located at %p\n", img);
	
#ifdef CONFIG_FIRMWARE_UPGRADE_MISTAKE_PROOFING
	if((ret=checkFwWhitelistHead(img))<0){
		printf("\n\n====== Image header check failed ======\n");
		return -1;
	}else if(ret>0){
		img = (u8 *)(img + ret);
		printf("skip white list header, vmimg is relocated at %p\n", img);
	}
#endif

	#ifdef CONFIG_OTTO_SNOF
	printf("CMD = %s\n", "sf probe");
	run_command("sf probe", 0);
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
		if (prev_image_sz%4) {
			printf("Warning: #%d Key: 0x%08x, size %d is not 4 alignment!\n", index, (hdr.key), (hdr.img_sz));
		}
		index++;

		img = (u8 *)(img + sizeof(struct vmimg_hdr) + prev_image_sz);
	}
done:
/***** Remove env parameters *******/
#define ENV_SW_VERSION0		"sw_version0"
#define ENV_SW_VERSION1		"sw_version1"
#define ENV_SW_CRC0		"sw_crc0"
#define ENV_SW_CRC1		"sw_crc1"
	env_set(ENV_SW_VERSION0, NULL );
	env_set(ENV_SW_VERSION1, NULL );
	env_set(ENV_SW_CRC0, NULL );
	env_set(ENV_SW_CRC1, NULL );

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
