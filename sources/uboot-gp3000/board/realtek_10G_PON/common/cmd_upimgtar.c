/*
 * Command for updating vm.img format image (containing uImage and rootfs) into corresponding partition on luna platform.
 */

#include <common.h>
#include <malloc.h>
#include <asm/io.h>
//#include <environment.h>
#include <env.h>
#include <u-boot/crc.h>

#if defined(CONFIG_MTD_CORTINA_SERIAL_NOR)
#define CONFIG_SPINOR_FLASH 1
#endif
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
#if 0
#define TAR_KERNEL_NAME		"uImage"
#define TAR_KERNEL_NAME_SIZE	6

#define TAR_ROOTFS_NAME		"rootfs"
#define TAR_ROOTFS_NAME_SIZE	6

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

#define TAR_FPGAFS_NAME "fpgafs"
#define TAR_FPGAFS_NAME_SIZE 6

#define TAR_FRAMEWORK_NAME "framework.img"
#define TAR_FRAMEWORK_NAME_SIZE 13

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
#define UBI_FPGA_NAME "ubi_fpga"
#define UBI_FRAMEWORK1_NAME "ubi_framework1"
#define UBI_FRAMEWORK2_NAME "ubi_framework2"
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
int framework_location = 0;
int framework_size = 0;
int fpgafs_location = 0;
int fpagfs_size     = 0;


char fwu_ver[TAR_FWUVER_FILE_SIZE];	/* fwu_ver content */

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

			/* Find uImage, rootfs and fwu_ver */
			if (strncmp(hdr->name, TAR_DTB_NAME, TAR_DTB_NAME_SIZE) == 0) {
				dtb_location = location;
				dtb_size = img_size;
			}else
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
			else if (strncmp(hdr->name, TAR_ENV_NAME, TAR_ENV_NAME_SIZE) == 0) {
				env_location = location;
				env_size = img_size;
			}
			else if (strncmp(hdr->name, TAR_FPGAFS_NAME, TAR_FPGAFS_NAME_SIZE) == 0) {
				fpgafs_location = location;
				fpagfs_size = img_size;
			}
			else if (strncmp(hdr->name, TAR_FRAMEWORK_NAME, TAR_FRAMEWORK_NAME_SIZE) == 0){
				framework_location = location;
				framework_size = img_size;
			}

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


}
#undef MMC_PART_START_NAME
#undef MMC_PART_SIZE_NAME
#undef MMC_BLK_SIZE

#endif

static int do_flash_update(char *pf1_addr, char *pf1_size, unsigned int src, unsigned int src_size, char *part_name);
static int do_flash_update_part(char *part_name, unsigned int src, unsigned int src_size);

#define WRITE_TO_MMC_FLAG "fw_mmc"
static int do_upimgtar(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	char *endp;
	u8 *img;
	u32 size;
	uint32_t img_src;
	uint32_t img_size;
	int update_emmc_flag = 0;
	char cmd_buf[CMD_BUF_SZ] = {0};
#ifndef CONFIG_RTK_USE_ONE_UBI_DEVICE
	u32 part_base, part_sz;
#endif
#ifdef CONFIG_SPINOR_FLASH
	sprintf(cmd_buf, "sf probe");
	printf("CMD = %s\n", cmd_buf);
	run_command(cmd_buf, 0);
#endif
	char *value;

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

#if CONFIG_CMD_MMC
	value = env_get(WRITE_TO_MMC_FLAG);
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

	if ( dtb_location && kernel_location && rootfs_location) {
		printf("dtb image is at 0x%08x (size: 0x%08x)\n", dtb_location, dtb_size);
		printf("kernel image is at 0x%08x (size: 0x%08x)\n", kernel_location, kernel_size);
		printf("rootfs image is at 0x%08x (size: 0x%08x)\n", rootfs_location, rootfs_size);
		if (osgi_location)
			printf("osgi image is at 0x%08x (size: 0x%08x)\n", osgi_location, osgi_size);
		if (env_location)
			printf("env image is at 0x%08x (size: 0x%08x)\n", env_location, env_size);
		printf("Firmware Version: [%s]\n\n", fwu_ver);
	}


	/* Update DBT image */
	img_src = (unsigned long)(img + dtb_location);
	img_size = dtb_size;
#if CONFIG_CMD_MMC
	if(update_emmc_flag) {
		do_mmc_update(MMC_DTB0_NAME, (unsigned long)(img + dtb_location), dtb_size);
	}
#endif

	if(update_emmc_flag) goto END_DTB_UPDATE;

#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_DTB0_NAME, (unsigned long)(img + dtb_location), dtb_size);
#else

	if (do_flash_update_part(MTD_DTB0_NAME, img_src, img_size)) {
		ret = do_flash_update("pfl_fdt1", "pfl_fdt1_sz", img_src, img_size, MTD_DTB0_NAME);
		if(ret){
			printf("Error: update %s fail!\n", MTD_DTB0_NAME);
			goto END_DTB_UPDATE;
		}
	}
#endif
END_DTB_UPDATE:

#ifdef CONFIG_LUNA_MULTI_BOOT
#if CONFIG_CMD_MMC
	if (update_emmc_flag) {
		do_mmc_update(MMC_DTB1_NAME, (unsigned long)(img + dtb_location), dtb_size);
	}
#endif


	if (update_emmc_flag) goto END_DTB2_UPDATE;
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_DTB1_NAME, (unsigned long)(img + dtb_location), dtb_size);
#else

	/* Update 2nd DTB image */
	if (do_flash_update_part(MTD_DTB1_NAME, img_src, img_size)) {
		ret = do_flash_update("pfl_fdt2", "pfl_fdt2_sz", img_src, img_size, MTD_DTB1_NAME);
		if(ret){
			printf("Error: update %s fail!\n", MTD_DTB1_NAME);
			goto END_DTB2_UPDATE;
		}
	}
#endif
END_DTB2_UPDATE:

#endif

	img_src  = (unsigned long)(img + kernel_location);
	img_size = kernel_size;
	/* Update kernel image */
#if CONFIG_CMD_MMC
	if (update_emmc_flag) {
		do_mmc_update(MMC_K0_NAME, (unsigned long)(img + kernel_location), kernel_size);
	}
#endif


	if (update_emmc_flag) {
		goto END_KERNEL_UPDATE;
	}
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_K0_NAME, (unsigned long)(img + kernel_location), kernel_size);
#else
	if (do_flash_update_part(MTD_K0_NAME, img_src, img_size)) {
		ret = do_flash_update("pfl_kernel1", "pfl_kernel1_sz", img_src, img_size, MTD_K0_NAME);
		if(ret){
			printf("Error: update %s fail!\n", MTD_K0_NAME);
			goto END_KERNEL_UPDATE;
		}
	}
#endif
END_KERNEL_UPDATE:

#ifdef CONFIG_LUNA_MULTI_BOOT
		/* Update 2nd kernel image */
#if CONFIG_CMD_MMC
	if(update_emmc_flag){
		do_mmc_update(MMC_K1_NAME, (unsigned long)(img + kernel_location), kernel_size);
	}
#endif

	if(update_emmc_flag){
		goto END_KERNEL2_UPDATE;
	}
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_K1_NAME, (unsigned long)(img + kernel_location), kernel_size);
#else
	if (do_flash_update_part(MTD_K1_NAME, img_src, img_size)) {
		ret = do_flash_update("pfl_kernel2", "pfl_kernel2_sz", img_src, img_size, MTD_K1_NAME);
		if(ret){
			printf("Error: update %s fail!\n", MTD_K1_NAME);
			goto END_KERNEL2_UPDATE;
		}
	}
#endif
END_KERNEL2_UPDATE:

#endif

	/* Rootfs image */
    img_src  = (unsigned long)(img + rootfs_location);
    img_size = rootfs_size;
#if CONFIG_CMD_MMC
	if(update_emmc_flag) {
		do_mmc_update(MMC_R0_NAME, (unsigned long)(img + rootfs_location), rootfs_size);
	}
#endif

	if(update_emmc_flag){
		goto END_ROOTFS_UPDATE;
	}
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_R0_NAME, (unsigned long)(img + rootfs_location), rootfs_size);
#else
	if (do_flash_update_part(MTD_R0_NAME, img_src, img_size)) {
		ret = do_flash_update("pfl_rootfs1", "pfl_rootfs1_sz", img_src, img_size, MTD_R0_NAME);
		if(ret){
			printf("Error: update %s fail!\n", MTD_R0_NAME);
			goto END_ROOTFS_UPDATE;
		}
	}
#endif
END_ROOTFS_UPDATE:

#ifdef CONFIG_LUNA_MULTI_BOOT
	/* 2nd Rootfs image */
#if CONFIG_CMD_MMC
	if(update_emmc_flag){
		do_mmc_update(MMC_R1_NAME, (unsigned long)(img + rootfs_location), rootfs_size);
	}
#endif

	if(update_emmc_flag){
		goto END_ROOTFS2_UPDATE;
	}
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
	do_ubi_update(UBI_R1_NAME, (unsigned long)(img + rootfs_location), rootfs_size);
#else
	if (do_flash_update_part(MTD_R1_NAME, img_src, img_size)) {
		ret = do_flash_update("pfl_rootfs2", "pfl_rootfs2_sz", img_src, img_size, MTD_R1_NAME);
		if(ret){
			printf("Error: update %s fail!\n", MTD_R1_NAME);
			goto END_ROOTFS2_UPDATE;
		}
	}
#endif
END_ROOTFS2_UPDATE:

#endif

    img_src  = (unsigned long)(img + osgi_location);
    img_size = osgi_size;
#if CONFIG_CMD_MMC
		if(osgi_location && update_emmc_flag){
			do_mmc_update(MMC_OSGI_NAME, (unsigned long)(img + osgi_location), osgi_size);
		}
#endif

	/* osgi image */
	if (osgi_location && (!update_emmc_flag)) {

#if CONFIG_RTK_USE_ONE_UBI_DEVICE
		ret = do_ubi_update(UBI_OSGI_NAME, (unsigned long)(img + osgi_location), osgi_size);
		if (ret) {
			goto END_OSGI_UPDATE;
		}
#else
			if (do_flash_update_part(MTD_OSGI_NAME, img_src, img_size)) {
					ret = do_flash_update("pfl_osgi", "pfl_osgi_sz", img_src, img_size, MTD_OSGI_NAME);
					if(ret){
							printf("Error: update %s fail!\n", MTD_OSGI_NAME);
							goto END_OSGI_UPDATE;
					}
			}
#endif
	}
END_OSGI_UPDATE:


	if(fpgafs_location) {
		img_src  = (unsigned long)(img+fpgafs_location);
		img_size = fpagfs_size;
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
		do_ubi_update(UBI_FPGA_NAME, (unsigned long)img_src, img_size);
#else
		if (do_flash_update_part(MTD_FPGA_NAME, img_src, img_size)) {
			ret = do_flash_update("pfl_fpgafs", "pfl_fpgafs_sz", img_src, img_size, MTD_FPGA_NAME);
			if(ret){
				printf("Error: update %s fail!\n", MTD_FPGA_NAME);
					goto END_FPGA_UPDATE;
			}
		}
#endif
	}
END_FPGA_UPDATE:

	if (framework_location){
		img_src  = (unsigned long)(img+framework_location);
		img_size = framework_size;
#if CONFIG_RTK_USE_ONE_UBI_DEVICE
		do_ubi_update(UBI_FRAMEWORK1_NAME, (unsigned long)img_src, img_size);
		do_ubi_update(UBI_FRAMEWORK2_NAME, (unsigned long)img_src, img_size);
#else
		if (do_flash_update_part(MTD_FRAMEWORK1_NAME, img_src, img_size)) {
			ret = do_flash_update("pfl_framework1", "pfl_framework1_sz", img_src, img_size, MTD_FRAMEWORK1_NAME);
			if(ret){
				printf("Error: update %s fail!\n", MTD_FRAMEWORK1_NAME);
					goto END_FRAMEWORK_UPDATE;
			}
		}
		if (do_flash_update_part(MTD_FRAMEWORK2_NAME, img_src, img_size)) {
			ret = do_flash_update("pfl_framework2", "pfl_framework2_sz", img_src, img_size, MTD_FRAMEWORK2_NAME);
			if(ret){
				printf("Error: update %s fail!\n", MTD_FRAMEWORK2_NAME);
					goto END_FRAMEWORK_UPDATE;
			}
		}
#endif
		sprintf(cmd_buf, "run erase_apps");
		ret = run_command(cmd_buf, 0);
	}
END_FRAMEWORK_UPDATE:

    img_src  = (unsigned long)(img + env_location);
    img_size = env_size;
#if CONFIG_CMD_MMC
		if (env_location && update_emmc_flag) {
			do_mmc_update(MMC_ENV_NAME, (unsigned long)(img + env_location), env_size);
#ifdef CONFIG_ENV_OFFSET_REDUND
			do_mmc_update(MMC_ENV2_NAME, (unsigned long)(img + env_location), env_size);
#endif
		}
#endif

	if (env_location && !update_emmc_flag) {
		if (do_flash_update_part("env", (unsigned long)(img + env_location), env_size)) {
			// try use address and size to update
			ret = do_flash_update("pfl_bootenv1", "pfl_bootenv1_sz", (unsigned long)(img + env_location), env_size, MTD_ENV_NAME);
			if(ret){
				printf("Error: update env1 fail!\n");
				goto done;
			}
		}
#ifdef CONFIG_ENV_OFFSET_REDUND
		if (do_flash_update_part("env2", (unsigned long)(img + env_location), env_size)) {
			// try use address and size to update
			ret = do_flash_update("pfl_bootenv2", "pfl_bootenv2_sz", (unsigned long)(img + env_location), env_size, MTD_ENV2_NAME);
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



static int do_flash_update(char *pf1_addr, char *pf1_size, unsigned int src, unsigned int src_size, char *part_name){
	char *endp;
	u32 part_base, part_sz;
	char cmd_buf[CMD_BUF_SZ] = {0};
	char *value = NULL;

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
	if (NULL != part_name) {
		printf("%s partition at 0x%08x, size=0x%08x\n", part_name, part_base, part_sz);
	}

	#ifdef CONFIG_SPINOR_FLASH
	sprintf(cmd_buf, "sf erase %x +%x; sf write %x %x %x", part_base, part_sz, src, part_base, src_size);
	#else
	sprintf(cmd_buf, "nand erase %x %x; nand write %x %x %x", part_base, part_sz, src, part_base, src_size);
	#endif
	if(src_size> part_sz) {
		printf("Error: image size is larger than the partition, operation aborted! partition size is %x, image size is %x\n", part_sz, src_size);
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
