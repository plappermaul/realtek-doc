/*******************************************************************************
  This file contains D-Link Project specific common definitoins.
  This file should be included in board config header file (include/config.h):
  by adding the header path to the 'mkconfig'
 *******************************************************************************/
#ifndef __DLINK_CUSTOMER_H
#define __DLINK_CUSTOMER_H


#define CAMEO_BOOT_SUPPORT
#define CAMEO_SIMPLE_DISPLAY 1

/* here reserve 8 MB memory for sdk
 * in my experience, only need explicitly do it for realtek sdk */
#define CAMEO_RESERVE_MEM_EXPLICITLY

#define CAMEO_STR_SZ   512 
#define CAMEO_BOOT_VERSION "1.00.001"

#define xstr(s)    str(s)
#define str(s)    #s

#define CAMEO_SUCCESS               0
#define CAMEO_FAILURE               1

#define KER_BASIC_BOOTARGS          "console=ttyS0,115200 user_debug=31" \
    " noinitrd root=/dev/mtdblock6 rw rootfstype=squashfs"
#define KER2_BASIC_BOOTARGS         "console=ttyS0,115200 user_debug=31" \
    " noinitrd root=/dev/mtdblock8 rw rootfstype=squashfs"


#ifdef CAMEO_RESERVE_MEM_EXPLICITLY
#ifdef CONFIG_DUAL_IMAGE
/*CAMEOTAG: Remove "mem=504M " by jianan 20230328, CONFIG_AUTO_PROBE_DRAM_SIZE is defined, so no need to pass memory size info to kernel;
 * rtk_dma_size=8M is configured in kernel .config, so no need to define here.*/
#define KERNEL_BASIC_BOOTARGS        KER_BASIC_BOOTARGS
#define KERNEL2_BASIC_BOOTARGS       KER2_BASIC_BOOTARGS
#else
#define KERNEL_BASIC_BOOTARGS       "mem=120M " KER_BASIC_BOOTARGS
#define KERNEL2_BASIC_BOOTARGS      "mem=120M " KER2_BASIC_BOOTARGS
#endif /* CONFIG_DUAL_IMAGE */
#else
#define KERNEL_BASIC_BOOTARGS       KER_BASIC_BOOTARGS
#define KERNEL2_BASIC_BOOTARGS      KER2_BASIC_BOOTARGS
#endif

#define DEFAULT_ENV_IMG_ID          1
#define DEFAULT_ENV_CONFIG_ID       1
#define DEFAULT_ENV_BOARD_VERSION   1
#define DEFAULT_ENV_HW_VERSION      "A1"
#define DEFAULT_ENV_SERIAL_NUMBER   "QQABC12345600"
#define DEFAULT_ENV_BOARD_ID        "ProductTestingWanted"

/* env var init value */
/* config below values
   in uClinux/vendors/Realtek/SDK/XXX/config.u-boot
#define CONFIG_BOOTCOMMAND          "bootf"
#define CONFIG_ETHADDR              00:11:22:33:44:55
#define CONFIG_SERVERIP             10.90.90.1
#define CONFIG_IPADDR               10.90.90.90
#define CONFIG_BOOTDELAY            3
*/
#define CONFIG_LOADADDR             0x81000000
#define CONFIG_EXTRA_ENV_SETTINGS \
    "Board_Version=" xstr(DEFAULT_ENV_BOARD_VERSION) "\0" \
    "Serial_Number=" DEFAULT_ENV_SERIAL_NUMBER "\0" \
    "Board_ID=" DEFAULT_ENV_BOARD_ID "\0" \
    "entftp=rtk network on; " \
    "mw 0xBB00ca00 0x0fcb5500; mw 0xBB00ca1c 0x00000217; " \
    "run enp0" "\0" \
    "enp0=rtk sdsreg set 0 2 7 17 0x54f; rtk sdsreg set 0 2 6 14 0x55a; " \
    "rtk sdsreg set 0 2 7 16 0x6003; rtk sdsreg set 0 2 6 19 0x68c1; " \
    "rtk sdsreg set 0 2 6 20 0xf021; rtk phymmd set 0 0 4 0xc441 0x8" "\0" \
    ""

/* mtd partitions */
#define MTD_PART_BOOT_OFFSET        0UL
#define MTD_PART_BOOT_SIZE          0x000d0000UL
#define MTD_PART_KERNEL_OFFSET      0x00300000UL
#define MTD_PART_KERNEL_SIZE        0x00180000UL
#define MTD_PART_ROOTFS_OFFSET      0x00480000UL
#ifdef CONFIG_DUAL_IMAGE
#define MTD_PART_ROOTFS_SIZE        0x00d00000UL
#else
#define MTD_PART_ROOTFS_SIZE        0x00b80000UL
#endif /* CONFIG_DUAL_IMAGE */
#define MTD_PART_KERNEL2_OFFSET     0x01180000UL
#define MTD_PART_KERNEL2_SIZE       0x00180000UL
#define MTD_PART_ROOTFS2_OFFSET     0x01300000UL
#define MTD_PART_ROOTFS2_SIZE       0x00d00000UL

#define MTD_PART_BDINFO_OFFSET      0x000f0000UL
#define MTD_PART_BDINFO_SIZE        0x00010000UL


/*define cameo board info data and API*/

typedef struct
{
    unsigned long id;   /* board id */
    char *model;        /* board model string */
    char* hwversion;    /*hw version for module,support modify it*/
    char *demo;         /* board demo string */
} cameo_board_info_t;


cameo_board_info_t gcameoboardinfo  ;  /*global boardinfo*/
/*Define the board module name and hw version*/
#define CAMEO_BOARD_MODULENAME            cameo_get_board_modulename()
#define CAMEO_BOARD_HW_VERSION            cameo_get_board_hwversion()
#define CAMEO_BOARD_DEMONAME              cameo_get_board_demoname()

#define BOOTARGS_BOOT_VERSION "1.00.001"

int    cameo_get_board_info(void);
char*  cameo_get_board_modulename(void);
char*  cameo_get_board_hwversion(void);
char*  cameo_get_board_demoname(void);



/*END define for cameo board info data and API*/



#endif /* __DLINK_CUSTOMER_H */

