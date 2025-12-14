/*
 * Command for updating vm.img format image (containing uImage and rootfs) into corresponding partition on luna platform.
 * Author: bohungwu@realtek.com
 */

#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <u-boot/crc.h>
//#include <environment.h>

enum { NAME_SIZE = 100 }; /* because gcc won't let me use 'static const int' */

/* POSIX tar Header Block, from POSIX 1003.1-1990  */
struct TarHeader
{
                                /* byte offset */
	char name[NAME_SIZE];         /*   0-99 */
	char mode[8];                 /* 100-107 */
	char uid[8];                  /* 108-115 */
	char gid[8];                  /* 116-123 */
	char size[12];                /* 124-135 */
	char mtime[12];               /* 136-147 */
	char chksum[8];               /* 148-155 */
	char typeflag;                /* 156-156 */
	char linkname[NAME_SIZE];     /* 157-256 */
	char magic[6];                /* 257-262 */
	char version[2];              /* 263-264 */
	char uname[32];               /* 265-296 */
	char gname[32];               /* 297-328 */
	char devmajor[8];             /* 329-336 */
	char devminor[8];             /* 337-344 */
	char prefix[155];             /* 345-499 */
	char padding[12];             /* 500-512 (pad to exactly the TAR_BLOCK_SIZE) */
};
typedef struct TarHeader TarHeader;

/* A few useful constants */
#define TAR_MAGIC          "ustar "        /* ustar and a null */
#define TAR_VERSION        " "           /* Be compatable with GNU tar format */
static const int TAR_MAGIC_LEN = 6;
static const int TAR_VERSION_LEN = 2;
static const int TAR_BLOCK_SIZE = 512;
#if 1
#define TAR_KERNEL_NAME		"uImage"
#define TAR_KERNEL_NAME_SIZE	6

#define TAR_ROOTFS_NAME		"rootfs"
#define TAR_ROOTFS_NAME_SIZE	6

#define TAR_OSGI_NAME		"osgi.img"
#define TAR_OSGI_NAME_SIZE	8

#define TAR_FWUVER_NAME		"fwu_ver"
#define TAR_FWUVER_NAME_SIZE	6
#define TAR_FWUVER_FILE_SIZE	32
#else  //G3
/*****************G3 ***********
fw_list="dtb rootfs kimage fwu_ver hw_ver"
********************************/
#define TAR_DTB_NAME        "dtb"
#define TAR_DTB_NAME_SIZE   3

#define TAR_KERNEL_NAME		"kimage"
#define TAR_KERNEL_NAME_SIZE	6

#define TAR_ROOTFS_NAME		"rootfs"
#define TAR_ROOTFS_NAME_SIZE	6

#define TAR_OSGI_NAME		"osgi.img"
#define TAR_OSGI_NAME_SIZE	8

#define TAR_FWUVER_NAME		"fwu_ver"
#define TAR_FWUVER_NAME_SIZE	6
#define TAR_FWUVER_FILE_SIZE	32

#define TAR_ENV_NAME "uboot-env.bin"
#define TAR_ENV_NAME_SIZE sizeof(TAR_ENV_NAME)

#endif

#if CONFIG_RTK_USE_ONE_UBI_DEVICE
#define UBI_MAIN_DEVICE_NAME "ubi_device"
#define UBI_DTB0_NAME "ubi_DTB0"
#define UBI_DTB1_NAME "ubi_DTB1"
#define UBI_K0_NAME "ubi_k0"
#define UBI_K1_NAME "ubi_k1"
#define UBI_R0_NAME "ubi_r0"
#define UBI_R1_NAME "ubi_r1"
#define UBI_OSGI_NAME "ubi_osgi"
#endif

int dtb_location = 0;		/* The DTB location in tar file */
int dtb_size = 0;
int kernel_location = 0;		/* The Kernel location in tar file */
int kernel_size = 0;
int rootfs_location = 0;		/* The rootfs location in tar file */
int rootfs_size = 0;
int osgi_location = 0;			/* The osgi location in tar file */
int osgi_size = 0;
int env_location = 0;
int env_size     = 0;


char fwu_ver[TAR_FWUVER_FILE_SIZE];	/* fwu_ver content */

#ifdef CONFIG_SINGLE_IMAGE
char *dual_image = NULL;
int image_type=0;
#define DUAL_IMAGE 1
#endif


#define ENV_SW_VERSION0		"sw_version0"
#define ENV_SW_VERSION1		"sw_version1"
#define ENV_SW_CRC0		"sw_crc0"
#define ENV_SW_CRC1		"sw_crc1"

//#define ENV_KERNEL0_CRC	"kernel0_crc"
//#define ENV_KERNEL1_CRC	"kernel1_crc"
//#define ENV_ROOTFS0_CRC	"rootfs0_crc"
//#define ENV_ROOTFS1_CRC	"rootfs1_crc"

#define CMD_BUF_SZ (256)
#define CRC32_STRING_SIZE	32

int check_img_tar(unsigned char *buffer, unsigned int size)
{
	TarHeader *hdr;

	//printf("Is this a img.tar? (size: 0x%08x)\n", size);
	if (size >= TAR_BLOCK_SIZE) {
		hdr = (TarHeader *)buffer;

		//printf("Magic/Version: [%s]/[%x]/[%x]\n", hdr->magic, hdr->version[0], hdr->version[1]);
		//if((strncmp(hdr->magic, TAR_MAGIC, TAR_MAGIC_LEN) == 0) && hdr->version[0] == 0x20 && hdr->version[1] == 0x0)
		//if(strncmp(hdr->magic, TAR_MAGIC TAR_VERSION, TAR_MAGIC_LEN + TAR_VERSION_LEN) == 0)
		if(strncmp(hdr->magic, TAR_MAGIC TAR_VERSION, TAR_MAGIC_LEN + TAR_VERSION_LEN) == 0)
			return 1;	/* This is a tar header */
	}

	return 0;
}

void scan_img_tar(unsigned char *buffer, unsigned int size)
{
	TarHeader *hdr;
	unsigned int location = 0,  img_size = 0;

	while((location + TAR_BLOCK_SIZE) < size) {
		hdr = (TarHeader *)(buffer + location);

		if(check_img_tar((unsigned char *)hdr, TAR_BLOCK_SIZE)){
			location += sizeof(TarHeader);
			img_size = simple_strtoul(hdr->size, 0, 8);
			printf("File in Tar: %s at 0x%08x (size: 0x%08x)\n", hdr->name, location, img_size);

#if 0 // don't have DTB
			/* Find uImage, rootfs and fwu_ver */
			if (strncmp(hdr->name, TAR_DTB_NAME, TAR_DTB_NAME_SIZE) == 0) {
				dtb_location = location;
				dtb_size = img_size;
			}else
#endif
			if (strncmp(hdr->name, TAR_KERNEL_NAME, TAR_KERNEL_NAME_SIZE) == 0) {
				kernel_location = location;
				kernel_size = img_size;
			}
			else if (strncmp(hdr->name, TAR_ROOTFS_NAME, TAR_ROOTFS_NAME_SIZE) == 0) {
				rootfs_location = location;
				rootfs_size = img_size;
			}
			else if (strncmp(hdr->name, TAR_FWUVER_NAME, TAR_FWUVER_NAME_SIZE) == 0) {
				//printf("Version Strings Size = %d\n", min((img_size - 1), TAR_FWUVER_FILE_SIZE));
				strncpy(fwu_ver, (char *)(buffer + location), min((img_size - 1), (unsigned int)TAR_FWUVER_FILE_SIZE));	/* The last char. is '\n' */
			}
			else if (strncmp(hdr->name, TAR_OSGI_NAME, TAR_OSGI_NAME_SIZE) == 0) {
				osgi_location = location;
				osgi_size = img_size;
			}
#if 0
			else if (strncmp(hdr->name, TAR_ENV_NAME, TAR_ENV_NAME_SIZE) == 0) {
				env_location = location;
				env_size = img_size;
			}
#endif

			location += img_size;
			location = (location + TAR_BLOCK_SIZE - 1) & ~0x1FFU;	/* 512 Bytes alignment  */
			//printf("Next File at 0x%08x\n", location);
		}
		else {
			return;
		}
	}

	return;
}

void save_fwu_version_env(void)
{
	char *old_ver;
	int dirty = 0;		/* 1: The env is modified */

	printf("Update SW Version: %s\n", fwu_ver);

	old_ver = env_get(ENV_SW_VERSION0);
	if(old_ver == NULL){
		printf("%s empty, set %s\n",ENV_SW_VERSION0, fwu_ver);
		env_set(ENV_SW_VERSION0, fwu_ver);
		dirty = 1;
	} else {
		if(strcmp(old_ver, fwu_ver)!=0){
			printf("old %s [%s] is different to new [%s], set new version\n", ENV_SW_VERSION0, old_ver, fwu_ver);
			env_set(ENV_SW_VERSION0, fwu_ver);
			dirty = 1;
		} //else
			//old and new version are the same, don't save env!
	}

#ifdef CONFIG_LUNA_MULTI_BOOT
	old_ver = env_get(ENV_SW_VERSION1);
	if(old_ver == NULL){
		printf("%s empty, set %s\n",ENV_SW_VERSION1, fwu_ver);
		env_set(ENV_SW_VERSION1, fwu_ver);
		dirty = 1;
	} else {
		if(strcmp(old_ver, fwu_ver) != 0){
			printf("old %s [%s] is different to new [%s], set new version\n", ENV_SW_VERSION1, old_ver, fwu_ver);
			env_set(ENV_SW_VERSION1, fwu_ver);
			dirty = 1;
		} //else
			//old and new version are the same, don't save env!
	}
#endif

	if (dirty)	env_save();
	return;
}

void save_images_crc_env(unsigned char *img, unsigned int size)
{
	char *old;
	char new[CRC32_STRING_SIZE];
	ulong crc;
	int dirty = 0;		/* 1: The env is modified */

	printf("Update Image CRC32\n");

	/* Caculate img.tar crc32 */
	old = env_get(ENV_SW_CRC0);
	crc = crc32_wd(0, img, size, CHUNKSZ_CRC32);
	sprintf(new, "%08lx", crc);

	if(old == NULL){
		printf("%s empty, set %s\n", ENV_SW_CRC0, new);
		env_set(ENV_SW_CRC0, new);
		dirty = 1;
	} else {
		if(strcmp(old, new) != 0){
			printf("old %s [%s] is different to new [%s], set new crc\n", ENV_SW_CRC0, old, new);
			env_set(ENV_SW_CRC0, new);
			dirty = 1;
		} //else
			//old and new crc are the same, don't save env!
	}

#ifdef CONFIG_LUNA_MULTI_BOOT
	old = env_get(ENV_SW_CRC1);

	if(old == NULL){
		printf("%s empty, set %s\n", ENV_SW_CRC1, new);
		env_set(ENV_SW_CRC1, new);
		dirty = 1;
	} else {
		if(strcmp(old, new) != 0){
			printf("old %s [%s] is different to new [%s], set new crc\n", ENV_SW_CRC1, old, new);
			env_set(ENV_SW_CRC1, new);
			dirty = 1;
		} //else
			//old and new crc are the same, don't save env!
	}
#endif

#if 0
	/* Caculate uImage crc32 */
	old = env_get(ENV_KERNEL0_CRC);
	crc = crc32_wd(0, (const unsigned char *)(img + kernel_location), kernel_size, CHUNKSZ_CRC32);
	sprintf(new, "%08lx", crc);

	if(old == NULL){
		printf("%s empty, set %s\n", ENV_KERNEL0_CRC, new);
		env_set(ENV_KERNEL0_CRC, new);
		dirty = 1;
	} else {
		if(strcmp(old, new) != 0){
			printf("old %s [%s] is different to new [%s], set new crc\n", ENV_KERNEL0_CRC, old, new);
			env_set(ENV_KERNEL0_CRC, new);
			dirty = 1;
		} //else
			//old and new crc are the same, don't save env!
	}

#ifdef CONFIG_LUNA_MULTI_BOOT
	old = env_get(ENV_KERNEL1_CRC);

	if(old == NULL){
		printf("%s empty, set %s\n", ENV_KERNEL1_CRC, new);
		env_set(ENV_KERNEL1_CRC, new);
		dirty = 1;
	} else {
		if(strcmp(old, new) != 0){
			printf("old %s [%s] is different to new [%s], set new crc\n", ENV_KERNEL1_CRC, old, new);
			env_set(ENV_KERNEL1_CRC, new);
			dirty = 1;
		} //else
			//old and new crc are the same, don't save env!
	}
#endif

	/* Caculate rootfs crc32 */
	old = env_get(ENV_ROOTFS0_CRC);
	crc = crc32_wd(0, (const unsigned char *)(img + rootfs_location), rootfs_size, CHUNKSZ_CRC32);
	sprintf(new, "%08lx", crc);

	if(old == NULL){
		printf("%s empty, set %s\n", ENV_ROOTFS0_CRC, new);
		env_set(ENV_ROOTFS0_CRC, new);
		dirty = 1;
	} else {
		if(strcmp(old, new) != 0){
			printf("old %s [%s] is different to new [%s], set new crc\n", ENV_ROOTFS0_CRC, old, new);
			env_set(ENV_ROOTFS0_CRC, new);
			dirty = 1;
		} //else
			//old and new crc are the same, don't save env!
	}

#ifdef CONFIG_LUNA_MULTI_BOOT
	old = env_get(ENV_ROOTFS1_CRC);

	if(old == NULL){
		printf("%s empty, set %s\n", ENV_ROOTFS1_CRC, new);
		env_set(ENV_ROOTFS1_CRC, new);
		dirty = 1;
	} else {
		if(strcmp(old, new) != 0){
			printf("old %s [%s] is different to new [%s], set new crc\n", ENV_ROOTFS1_CRC, old, new);
			env_set(ENV_ROOTFS1_CRC, new);
			dirty = 1;
		} //else
			//old and new crc are the same, don't save env!
	}
#endif
#endif

	if (dirty)	env_save();
	return;
}

#if CONFIG_RTK_USE_ONE_UBI_DEVICE
static int
do_ubi_update(char *vol_name,  unsigned long src, unsigned long src_size)
{
	char cmd_buf[CMD_BUF_SZ] = {0};
	sprintf(cmd_buf, "setenv current_vol %s && run check_vol", vol_name);
	run_command(cmd_buf, 0);
	sprintf(cmd_buf, "ubi write %lx %s %lx", src, vol_name, src_size);
	printf("CMD = %s\n", cmd_buf);
	run_command(cmd_buf, 0);
	return 0;
}
#endif


static int do_flash_update(char *pf1_addr, char *pf1_size, unsigned int src, unsigned int src_size);
static int do_flash_update_part(char *part_name, unsigned int src, unsigned int src_size);

static int do_upimgtar(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	char *endp;
	u8 *img;
	u32 size;
#ifndef CONFIG_RTK_USE_ONE_UBI_DEVICE
	char cmd_buf[CMD_BUF_SZ] = {0};
	u32 part_base, part_sz;
#endif
	/* need at least three arguments */
	if (argc < 3)
		goto usage;

	/* Retrieve imgi.tar location */
	img = (u8 *)simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		return -1;

	/* Retrieve img.tar size */
	size = (u32)simple_strtoul(argv[2], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		return -1;

	printf("img.tar is located at %p (size = 0x%08x)\n", img, size);
	scan_img_tar(img, size);

	if ( /*dtb_location &&*/ kernel_location && rootfs_location) {
		//printf("dtb image is at 0x%08x (size: 0x%08x)\n", dtb_location, dtb_size);
		printf("kernel image is at 0x%08x (size: 0x%08x)\n", kernel_location, kernel_size);
		printf("rootfs image is at 0x%08x (size: 0x%08x)\n", rootfs_location, rootfs_size);
		if (osgi_location)
			printf("osgi image is at 0x%08x (size: 0x%08x)\n", osgi_location, osgi_size);
		if (env_location)
			printf("env image is at 0x%08x (size: 0x%08x)\n", env_location, env_size);
		printf("Firmware Version: [%s]\n\n", fwu_ver);
	}

	#ifdef CONFIG_OTTO_SNOF
	printf("CMD = %s\n", "sf probe");
	run_command("sf probe", 0);
	#endif

#if 0 // don't have DTB partition
	/* Update DBT image */
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_DTB0_NAME, (unsigned long)(img + dtb_location), dtb_size);
#else
	if (NULL == env_get("fl_fdt1")) {
		printf("Error: env name fl_fdt1 not found\n");
		goto done;
	}
	part_base = simple_strtoul(env_get("fl_fdt1"), &endp, 16);
	if (NULL == env_get("fl_fdt1_sz")) {
		printf("Error: env name fl_fdt1_sz not found\n");
		goto done;
	}
	part_sz = simple_strtoul(env_get("fl_fdt1_sz"), &endp, 16);
	printf("DTB partition at 0x%08x, size=0x%08x\n", part_base, part_sz);

	#ifdef CONFIG_OTTO_SNOF
	sprintf(cmd_buf, "sf erase %x +%x; sf write %x %x %x", part_base, part_sz, (unsigned long)(img + dtb_location), part_base, dtb_size);
	#else
	sprintf(cmd_buf, "nand erase %x %x; nand write %x %x %x", part_base, part_sz, (unsigned long)(img + dtb_location), part_base, dtb_size);
	#endif
	if(dtb_size > part_sz) {
		printf("Error: image size is larger than the partition, operation aborted\n");
		goto done;
	}
	printf("CMD = %s\n", cmd_buf);
	run_command(cmd_buf, 0);
#endif

#ifdef CONFIG_LUNA_MULTI_BOOT
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_DTB1_NAME, (unsigned long)(img + dtb_location), dtb_size);
#else
		/* Update 2nd DTB image */
		if (NULL == env_get("fl_fdt2")) {
			printf("Error: env name fl_fdt2 not found\n");
			goto done;
		}
		part_base = simple_strtoul(env_get("fl_fdt2"), &endp, 16);
		if (NULL == env_get("fl_fdt2_sz")) {
			printf("Error: env name fl_fdt2_sz not found\n");
			goto done;
		}
		part_sz = simple_strtoul(env_get("fl_fdt2_sz"), &endp, 16);
		printf("DTB partition at 0x%08x, size=0x%08x\n", part_base, part_sz);
		#ifdef CONFIG_OTTO_SNOF
		sprintf(cmd_buf, "sf erase %x +%x; sf write %x %x %x", part_base, part_sz, (unsigned long)(img + dtb_location), part_base, dtb_size);
		#else
		sprintf(cmd_buf, "nand erase %x %x; nand write %x %x %x", part_base, part_sz, (unsigned long)(img + dtb_location), part_base, dtb_size);
		#endif
		if(dtb_size > part_sz) {
			printf("Error: image size is larger than the partition, operation aborted\n");
			goto done;
		}
		printf("CMD = %s\n", cmd_buf);
		run_command(cmd_buf, 0);
#endif

#endif
#endif


	/* Update kernel image */
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_K0_NAME, (unsigned long)(img + kernel_location), kernel_size);
#else
	if (NULL == env_get("fl_kernel1")) {
		printf("Error: env name fl_kernel1 not found\n");
		goto done;
	}
	part_base = simple_strtoul(env_get("fl_kernel1"), &endp, 16);
	if (NULL == env_get("fl_kernel1_sz")) {
		printf("Error: env name fl_kernel1_sz not found\n");
		goto done;
	}
	part_sz = simple_strtoul(env_get("fl_kernel1_sz"), &endp, 16);
	printf("kernel partition at 0x%08x, size=0x%08x\n", part_base, part_sz);

	#ifdef CONFIG_OTTO_SNOF
	sprintf(cmd_buf, "sf erase %x +%x; sf write %lx %x %x", part_base, part_sz, (unsigned long)(img + kernel_location), part_base, kernel_size);
	#else
	sprintf(cmd_buf, "nand erase %x %x; nand write %lx %x %x", part_base, part_sz, (unsigned long)(img + kernel_location), part_base, kernel_size);
	#endif
	if(kernel_size > part_sz) {
		printf("Error: image size is larger than the partition, operation aborted\n");
		goto done;
	}
	printf("CMD = %s\n", cmd_buf);
	run_command(cmd_buf, 0);
#endif

#ifdef CONFIG_LUNA_MULTI_BOOT
		/* Update 2nd kernel image */
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_K1_NAME, (unsigned long)(img + kernel_location), kernel_size);
#else
		if (NULL == env_get("fl_kernel2")) {
			printf("Error: env name fl_kernel2 not found\n");
			goto done;
		}
		part_base = simple_strtoul(env_get("fl_kernel2"), &endp, 16);
		if (NULL == env_get("fl_kernel2_sz")) {
			printf("Error: env name fl_kernel2_sz not found\n");
			goto done;
		}
		part_sz = simple_strtoul(env_get("fl_kernel2_sz"), &endp, 16);
		printf("kernel partition at 0x%08x, size=0x%08x\n", part_base, part_sz);
		#ifdef CONFIG_OTTO_SNOF
		sprintf(cmd_buf, "sf erase %x +%x; sf write %lx %x %x", part_base, part_sz, (unsigned long)(img + kernel_location), part_base, kernel_size);
		#else
		sprintf(cmd_buf, "nand erase %x %x; nand write %lx %x %x", part_base, part_sz, (unsigned long)(img + kernel_location), part_base, kernel_size);
		#endif
		if(kernel_size > part_sz) {
			printf("Error: image size is larger than the partition, operation aborted\n");
			goto done;
		}
		printf("CMD = %s\n", cmd_buf);
		run_command(cmd_buf, 0);
#endif

#endif
	/* Rootfs image */
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_R0_NAME, (unsigned long)(img + rootfs_location), rootfs_size);
#else
	if (NULL == env_get("fl_rootfs1")) {
		printf("Error: env name fl_rootfs1 not found\n");
		goto done;
	}
	part_base = simple_strtoul(env_get("fl_rootfs1"), &endp, 16);
	if (NULL == env_get("fl_rootfs1_sz")) {
		printf("Error: env name fl_rootfs1_sz not found\n");
		goto done;
	}
	part_sz = simple_strtoul(env_get("fl_rootfs1_sz"), &endp, 16);
	printf("rootfs partition at 0x%08x, size=0x%08x\n", part_base, part_sz);
	#ifdef CONFIG_OTTO_SNOF
	sprintf(cmd_buf, "sf erase %x +%x; sf write %lx %x %x", part_base, part_sz, (unsigned long)(img + rootfs_location), part_base, rootfs_size);
	#else
	sprintf(cmd_buf, "nand erase %x %x; nand write %lx %x %x", part_base, part_sz, (unsigned long)(img + rootfs_location), part_base, rootfs_size);
	#endif
	if(rootfs_size > part_sz) {
		printf("Error: image size is larger than the partition, operation aborted\n");
		goto done;
	}
	printf("CMD = %s\n", cmd_buf);
	run_command(cmd_buf, 0);
#endif

#ifdef CONFIG_LUNA_MULTI_BOOT
	/* 2nd Rootfs image */
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_R1_NAME, (unsigned long)(img + rootfs_location), rootfs_size);
#else
	if (NULL == env_get("fl_rootfs2")) {
		printf("Error: env name fl_rootfs2 not found\n");
		goto done;
	}
	part_base = simple_strtoul(env_get("fl_rootfs2"), &endp, 16);
	if (NULL == env_get("fl_rootfs2_sz")) {
		printf("Error: env name fl_rootfs2_sz not found\n");
		goto done;
	}
	part_sz = simple_strtoul(env_get("fl_rootfs2_sz"), &endp, 16);
	printf("rootfs partition at 0x%08x, size=0x%08x\n", part_base, part_sz);
	#ifdef CONFIG_OTTO_SNOF
	sprintf(cmd_buf, "sf erase %x +%x; sf write %lx %x %x", part_base, part_sz, (unsigned long)(img + rootfs_location), part_base, rootfs_size);
	#else
	sprintf(cmd_buf, "nand erase %x %x; nand write %lx %x %x", part_base, part_sz, (unsigned long)(img + rootfs_location), part_base, rootfs_size);
	#endif
	if(rootfs_size > part_sz) {
		printf("Error: image size is larger than the partition, operation aborted\n");
		goto done;
	}
	printf("CMD = %s\n", cmd_buf);
	run_command(cmd_buf, 0);
#endif

#endif
	/* osgi image */
	if (osgi_location) {
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
		ret = do_ubi_update(UBI_OSGI_NAME, (unsigned long)(img + osgi_location), osgi_size);
		if (ret) {
			goto done;
		}
#else
		if (NULL == env_get("fl_osgi")) {
			printf("Error: env name fl_osgi not found\n");
			goto done;
		}
		part_base = simple_strtoul(env_get("fl_osgi"), &endp, 16);
		if (NULL == env_get("fl_osgi_sz")) {
			printf("Error: env name fl_osgi_sz not found\n");
			goto done;
		}
		part_sz = simple_strtoul(env_get("fl_osgi_sz"), &endp, 16);
		printf("osgi partition at 0x%08x, size=0x%08x\n", part_base, part_sz);
		#ifdef CONFIG_OTTO_SNOF
		sprintf(cmd_buf, "sf erase %x +%x; sf write %lx %x %x", part_base, part_sz, (unsigned long)(img + osgi_location), part_base, osgi_size);
		#else
		sprintf(cmd_buf, "nand erase %x %x; nand write %lx %x %x", part_base, part_sz, (unsigned long)(img + osgi_location), part_base, osgi_size);
		#endif
		if(osgi_size > part_sz) {
			printf("Error: image size is larger than the partition, operation aborted\n");
			goto done;
		}
		printf("CMD = %s\n", cmd_buf);
		run_command(cmd_buf, 0);
#endif
	}

	if (env_location) {
		if (do_flash_update_part("env", (unsigned long)(img + env_location), env_size)) {
			// try use address and size to update
			ret = do_flash_update("fl_bootenv1", "fl_bootenv1_sz", (unsigned long)(img + env_location), env_size);
			if(ret){
				printf("Error: update env1 fail!\n");
				goto done;
			}
		}
#ifdef CONFIG_ENV_OFFSET_REDUND
		if (do_flash_update_part("env2", (unsigned long)(img + env_location), env_size)) {
			// try use address and size to update
			ret = do_flash_update("fl_bootenv2", "fl_bootenv2_sz", (unsigned long)(img + env_location), env_size);
			if(ret){
				printf("Error: update env2 fail!\n");
				goto done;
			}
		}
#endif

  }

	save_fwu_version_env();
	save_images_crc_env(img, size);

done:
	printf("Update img.tar Done\n");
	if (ret != -1)
		return ret;

usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	upimgtar,	3,	1,	do_upimgtar,
	"update kernel and rootfs by tar format on luna platform",
	"ADDRESS\n"
	"SIZE\n"
	"\n"
	"Description:\n"
	"Extract kernel and rootfs in tar file located at ADDRESS and write them into corresponding partitions.\n"
);



static int do_flash_update(char *pf1_addr, char *pf1_size, unsigned int src, unsigned int src_size){
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
