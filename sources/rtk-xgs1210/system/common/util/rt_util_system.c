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
 * $Revision: 96728 $
 * $Date: 2019-05-08 14:00:51 +0800 (Wed, 08 May 2019) $
 *
 * Purpose : Define the utility macro and function in the SDK.
 *
 * Feature : SDK common utility
 *
 */

/*
 * Include Files
 */

#include <common/rt_autoconf.h>
#include <ioal/mem32.h>
#include <common/error.h>
#include <osal/print.h>
#include <osal/memory.h>
#include <osal/print.h>
#include <osal/lib.h>

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>

#ifdef CONFIG_SDK_HW_PROFILE_PROBE_UBOOT_ENV
#if defined(CONFIG_SDK_KERNEL_LINUX) && !defined(__KERNEL__)
#include <mtd/mtd-user.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(__KERNEL__)
#include <linux/version.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/nand.h>
#include <mtd/mtd-abi.h>
#include <linux/slab.h>
#endif
#endif /* CONFIG_SDK_HW_PROFILE_PROBE_UBOOT_ENV */
#include <common/util/rt_util_system.h>

#ifdef CONFIG_SDK_HW_PROFILE_PROBE_GPIO
#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(__KERNEL__)
#include <linux/version.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/nand.h>
#include <mtd/mtd-abi.h>
#include <linux/slab.h>
#endif
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <hwp/hw_profile.h>
#if defined(CONFIG_SDK_RTL8231)
  #include <private/drv/rtl8231/rtl8231_probe.h>
#endif
#endif

#if !defined(__BOOTLOADER__) && defined(__KERNEL__)
#include <linux/mtd/rtk_flash_common.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

#ifdef CONFIG_SDK_HW_PROFILE_PROBE_GPIO
static const board_probe_gpio_t board_probe_pin_example = {
    .total_gpio      = BOARD_PROBE_PIN_NUMBER,
    .pin = {
            [0] = {HWP_GPIO_PROBE_USED_DEVICE_ID, 0, 18, GPIO_DIR_IN},  /* bit 0 */
            [1] = {HWP_GPIO_PROBE_USED_DEVICE_ID, 0, 19, GPIO_DIR_IN},  /* bit 1 */
            [2] = {HWP_GPIO_PROBE_USED_DEVICE_ID, 0, 23, GPIO_DIR_IN},  /* bit 2 */
    },
};
#endif

static uint8 ledModeInitSkip_value=LEDMODEINITSKIP_INIT_VALUE;


/*
 * Macro Definition
 */

/* ========================================================================= */
#define local static

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================
 * Table of CRC-32's of all single-byte values (made by make_crc_table)
 */
local const uint32 crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};


/* ========================================================================= */
/* Function Name:
 *      rt_util_crc32_cal
 * Description:
 *      Calculate crc32 checksum
 * Input:
 *      crc - crc checksum
 *      buf - data buffer
 *      len - data length
 * Output:
 *      None
 * Return:
 *      checksum
 * Note:
 *      None
 */
uint32 rt_util_crc32_cal(uint32 crc, const char *buf, uint32 len)
{
    if (NULL == buf)
        return 0L;

    crc = crc ^ 0xffffffffL;

    while (len >= 8)
    {
        DO8(buf);
        len -= 8;
    }

    if (len)
    {
        do {
            DO1(buf);
        } while (--len);
    }

    return crc ^ 0xffffffffL;
}

/* Function Name:
 *      rt_util_flashEnv_get
 * Description:
 *      Get environment variable from flash
 * Input:
 *      name        - variable name
 *      buffLen     - Buffer length of pValue.
 * Output:
 *      pValue      - variable value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
int32 rt_util_flashEnv_get(char *name, char *pValue, int buffLen)
{
    int32 ret = RT_ERR_OK;

#if defined(__BOOTLOADER__)
    char *ptr = NULL;

    ptr = getenv(name);
    if(ptr == NULL)
    {
        return RT_ERR_FAILED;
    }
    osal_memcpy(pValue, ptr, buffLen);
#elif defined(CONFIG_SDK_KERNEL_LINUX)
 #if defined(__KERNEL__)
 #if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,20))
    struct mtd_info *mtd_dev = NULL;
    u_char    *env;
    int32   get_len;
    mtd_dev = get_mtd_device_nm("BDINFO");
    env = kmalloc((size_t)(mtd_dev->size), GFP_ATOMIC);
    mtd_read(mtd_dev, 0, (mtd_dev->size), &get_len, env);
    if (RT_ERR_OK != (ret = rt_util_flashVar_get(env, (mtd_dev->size), name, pValue, buffLen)))
    {
        kfree(env);
        return ret;
    }
    kfree(env);
 #else
    if (RT_ERR_OK != (ret = rt_util_flashVar_get((char *)(SPI_FLASH_BASE + rtk_flash_partition_offset_get(FLASH_INDEX_LOADER_BDINFO)), rtk_flash_partition_size_get(FLASH_INDEX_LOADER_BDINFO), name, pValue, buffLen)))
        return ret;
 #endif
 #else
    if (RT_ERR_OK != (ret = rt_util_flashVar_mtd_get(CONFIG_BDINFO_MTD_INDEX, name, pValue, buffLen)))
        return ret;
 #endif
#else
    /* non-linux */
#endif

    return ret;
}

#ifdef __KERNEL__

/* Function Name:
 *      rt_util_flashVar_get
 * Description:
 *      Get variable from specific flash area
 * Input:
 *      addr        - specific flash address
 *      len         - specific flash area length
 *      name        - variable name
 *      valueBufLen - Buffer length of pValue.
 * Output:
 *      pValue      - variable value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
int32 rt_util_flashVar_get(char *addr, uint32 len, char *name, char *pValue, int valueBufLen)
{
    char *buf = NULL;
    char *data_ptr = NULL;
    char *varname = NULL;
    char *para = NULL;
    int32 i = 0;
    int32 nxt = 0;
    int32 varval = 0;
    uint32 calchksum = 0;
    uint32 crc = 0;
    uint32 size = 0;
    char *read_addr = NULL;

    size = len;

    read_addr = addr;

    buf = osal_alloc(size);
    if(buf == NULL)
        return RT_ERR_FAILED;

    data_ptr = buf + 4; /* Skip CRC field */

    for(size = 0; size < len; size++, read_addr++)

    {
        *(buf + size) = REG8(read_addr);
    }

    crc = REG32(addr);

    calchksum = rt_util_crc32_cal(0, data_ptr, size - sizeof(crc));

    if(calchksum == crc)
    {
        para = osal_alloc(size);
        if(para == NULL){
            if(buf != NULL){
                osal_free(buf);
            }
            return RT_ERR_FAILED;
        }

        for (i = 0; *(data_ptr + i) != '\0'; i = nxt + 1)
        {
            osal_memset(para, 0, ENV_PARAMETER_LEN);
            varname = data_ptr + i;
            varval = 0;

            for (nxt = i; *(data_ptr + nxt) != '\0'; ++nxt)
            {
                if (nxt > size)
                {
                    if(buf != NULL){
                        osal_free(buf);
                    }
                    if(para != NULL){
                        osal_free(para);
                    }
                    return RT_ERR_FAILED;
                }

                if ('=' == *(data_ptr + nxt))
                {
                    osal_memcpy(para, varname, (nxt - i));
                    if (0 == osal_strcmp(name, para))
                    {
                        varval = nxt + 1;
                    }
                }
            }

            if (0 != varval)
            {
                if ((nxt - varval) >= valueBufLen)
                {
                    osal_printf("%s,%d: Error! Insufficient buffer size %d. %d bytes is required!\n", __FUNCTION__, __LINE__, valueBufLen, (nxt - varval));
                    if(buf != NULL){
                        osal_free(buf);
                    }
                    if(para != NULL){
                        osal_free(para);
                    }
                    return RT_ERR_FAILED;
                }

                osal_memcpy(pValue, data_ptr + varval, nxt - varval);

                pValue[nxt - varval] = '\0';
                if(buf != NULL){
                    osal_free(buf);
                }
                if(para != NULL){
                    osal_free(para);
                }
                return RT_ERR_OK;
            }
        }
    }
    if(buf != NULL){
        osal_free(buf);
    }
    if(para != NULL){
        osal_free(para);
    }
    return RT_ERR_FAILED;
}
#endif

#if defined(CONFIG_SDK_KERNEL_LINUX) && !defined(__KERNEL__)

/* Function Name:
 *      rt_util_flashVar_mtd_get
 * Description:
 *      Get variable from specific MTD block
 * Input:
 *      mtd_idx     - specific MTD index
 *      len         - specific MTD length
 *      name        - variable name
 *      valueBufLen - Buffer length of pValue.
 * Output:
 *      pValue      - variable value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
int32 rt_util_flashVar_mtd_get(uint32 mtd_idx, char *name, char *pValue, int valueBufLen)
{
    char *buf = NULL;
    char *data_ptr = NULL;
    char *varname = NULL;
    char mtd_name[30];
    int32 i = 0;
    int32 fd = -1;
    int32 nxt = 0;
    int32 varval = 0;
    uint32 calchksum = 0;
    uint32 crc = 0;
    uint32 size = 0;
    mtd_info_t mtd_info;
    char *para = NULL;

    osal_memset(mtd_name, 0, sizeof(mtd_name));
    osal_sprintf(mtd_name, "/dev/mtdchar%d", (mtd_idx*2));

    if ((fd = open(mtd_name, O_RDONLY)) < 0)
            return RT_ERR_FAILED;

    /* Get MTD partition size information */
    if (ioctl(fd, MEMGETINFO, &mtd_info) < 0)
    {
        close(fd);
        return RT_ERR_FAILED;
    }

    size = mtd_info.size;

    buf = osal_alloc(size);
    if(buf == NULL)
    {
        close(fd);
        return RT_ERR_FAILED;
    }
    data_ptr = buf + 4; /* Skip CRC field */

    if (read(fd, buf, size) < 0)
    {
        close(fd);
        if(buf != NULL)
            osal_free(buf);

        return RT_ERR_FAILED;
    }

    close(fd);
    crc = ((buf[0] & 0xFF) << 24) + ((buf[1] & 0xFF) << 16) + ((buf[2] & 0xFF) << 8) + (buf[3] & 0xFF);

    calchksum = rt_util_crc32_cal(0, data_ptr, size - sizeof(crc));

    /* Check CRC */
    if (crc != calchksum)
    {
        if(buf != NULL)
            osal_free(buf);
        return RT_ERR_FAILED;
    }

    para = osal_alloc(ENV_PARAMETER_LEN);
    if(para == NULL){
        if(buf != NULL)
            osal_free(buf);
        return RT_ERR_FAILED;
    }

    for (i = 0; *(data_ptr + i) != '\0'; i = nxt + 1)
    {
        varname = data_ptr + i;
        varval = 0;

        osal_memset(para, 0, ENV_PARAMETER_LEN);

        for (nxt = i; *(data_ptr + nxt) != '\0'; ++nxt)
        {
            if (nxt > size)
            {
                if(buf != NULL)
                    osal_free(buf);
                if(para != NULL)
                    osal_free(para);
                return RT_ERR_FAILED;
            }

            if ('=' == *(data_ptr + nxt))
            {
                osal_memcpy(para, varname, (nxt - i));
                if (0 == osal_strcmp(name, para))
                {
                    varval = nxt + 1;
                }
            }
        }

        if (0 != varval)
        {
            if ((nxt - varval) >= valueBufLen)
            {
                osal_printf("%s,%d: Error! Insufficient buffer size %d. %d bytes is required!\n", __FUNCTION__, __LINE__, valueBufLen, (nxt - varval));
                if(buf != NULL){
                    osal_free(buf);
                }
                if(para != NULL){
                    osal_free(para);
                }
                return RT_ERR_FAILED;
            }

            osal_memcpy(pValue, data_ptr + varval, nxt - varval);
            pValue[nxt - varval] = '\0';
            if(buf != NULL){
                osal_free(buf);
            }
            if(para != NULL){
                osal_free(para);
            }
            return RT_ERR_OK;
        }
    }

    if(buf != NULL){
        osal_free(buf);
    }
    if(para != NULL){
        osal_free(para);
    }

    return RT_ERR_FAILED;
}
#endif

/* Function Name:
 *      rt_util_sysMac_get
 * Description:
 *      Get MAC address from specific u-boot environment area and translate
 *      to rtk_mac_t format.
 * Input:
 *      pMacAddr   - mac address buffer
 * Output:
 *      pMacAddr   - mac address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
int32 rt_util_sysMac_get(rtk_mac_t *pMacAddr)
{
    int32 	ret = RT_ERR_FAILED;
    char 	macAddress[18] = { 0 };
    int32 	macStrLength = 18;
    int32  	index;
    uint8   hex = 0, nibble_index = 0, byteValue = 0;


#if defined(__MODEL_USER__) || defined(CONFIG_SDK_MODEL_MODE)
    return RT_ERR_FAILED;
#endif
    if(RT_ERR_OK != (ret = rt_util_flashEnv_get("ethaddr", (char *)&macAddress, macStrLength)))
        return ret;

    for(index = 0; index < (macStrLength - 1); index++)
    {
        ret = rt_util_ascii2hex((char *)&macAddress[index],&hex);
        if(ret == RT_ERR_OK)
        {
            if(nibble_index != 0)
                byteValue = (byteValue << 4);
            byteValue += hex;
            if(nibble_index != 0)
                pMacAddr->octet[index/3] = byteValue;
            nibble_index++;
        }else{
            nibble_index = 0;
            byteValue = 0;
        }
    }
    return ret;
}


/* Function Name:
 *      rt_util_ledInitFlag_get
 * Description:
 *      Get LED Init flag from u-boot environment area
 * Input:
 *      None
 * Output:
 *      ledModeInitSkip   - the interger value of the definition in U-Boot env of ledModeInitSkip var.
 *                          1: true (SDK should skip LED init), 0: false
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
int32 rt_util_ledInitFlag_get(uint8 *ledModeInitSkip)
{
    int32 ret = RT_ERR_FAILED;
    char ledModeInitSkip_str[LEDMODEINITSKIP_LEN];

    if (LEDMODEINITSKIP_INIT_VALUE != ledModeInitSkip_value)
    {
        *ledModeInitSkip = ledModeInitSkip_value;
        return RT_ERR_OK;
    }

    ret = rt_util_flashEnv_get("ledModeInitSkip", ledModeInitSkip_str, LEDMODEINITSKIP_LEN);

    if (RT_ERR_OK == ret)
    {
        uint8 tmp8 = LEDMODEINITSKIP_INIT_VALUE;
        rt_util_ascii2dec(&ledModeInitSkip_str[0],&tmp8);
        if (tmp8 == 1)
            ledModeInitSkip_value = LEDMODEINITSKIP_YES;
        else
            ledModeInitSkip_value = LEDMODEINITSKIP_NO;
    }
    else
    {
        ledModeInitSkip_value = LEDMODEINITSKIP_NO;
    }

    *ledModeInitSkip = ledModeInitSkip_value;

    return ret;
}


/* Function Name:
 *      rt_util_ascii2hex
 * Description:
 *      Convert ASCII to Hex
 * Input:
 *      pAscii        - pointer to ascii charactor
 * Output:
 *      pHexValue     - hex value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
int32 rt_util_ascii2hex(char *pAscii, uint8 *pHexValue)
{
    char ch;

    ch = *pAscii;

    if((ch < '0' || ch > '9')&&(ch < 'A' || ch > 'F')&&(ch < 'a' || ch > 'f'))
        return RT_ERR_FAILED;

    if ((ch >= '0')&&(ch <= '9'))
        *pHexValue = (uint8)(ch - 0x30);
    if ((ch >= 'A')&&(ch <= 'F'))
        *pHexValue = (uint8)(ch - 0x37);
    if ((ch >= 'a')&&(ch <= 'f'))
        *pHexValue = (uint8)(ch - 0x57);

    return RT_ERR_OK;
}

/* Function Name:
 *      rt_util_ascii2dec
 * Description:
 *      Convert ASCII to decimal
 * Input:
 *      pAscii        - pointer to ascii charactor
 * Output:
 *      pDecValue     - Dec value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
int32 rt_util_ascii2dec(char *pAscii, uint8 *pDecValue)
{
    char ch;

    ch = *pAscii;

    if((ch < '0' || ch > '9'))
        return RT_ERR_FAILED;

    if ((ch >= '0')&&(ch <= '9'))
        *pDecValue = (uint8)(ch - 0x30);

    return RT_ERR_OK;
}

#ifdef CONFIG_SDK_HW_PROFILE_PROBE_GPIO
#if defined(CONFIG_SDK_RTL8231)
/* Function Name:
 *      extGPIO_hwp_mdcSem_callback
 * Description:
 *      Take/Give MDC/MDIO semaphore resource by lower layer in specified device.
 * Input:
 *      unit - unit id
 *      type - semaphore type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 * Note:
 *      The type value 0 mean lock the semaphore; 1 mean unlock the semaphore.
 */
static int32
extGPIO_hwp_mdcSem_callback(uint32 unit, uint32 type)
{
    return RT_ERR_OK;
}
#endif


/* Function Name:
 *      rt_util_boardID_GPIO_envPrepare
 * Description:
 *      Prepare get boardID by GPIO environment
 * Input:
 *      N/A
 * Output:
 *      N/A
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
static int32 rt_util_boardID_GPIO_envPrepare(void)
{
    int32 preused_unit = HWP_GPIO_PROBE_USED_UNIT_ID;
    uint32  chip_id, chip_rev;
    int32 ret;

    ret = drv_swcore_cid_get(preused_unit, &chip_id, &chip_rev);
    if(ret != RT_ERR_OK)
        return RT_ERR_FAILED;

    HWP_CHIP_ID(preused_unit) = chip_id;
#if defined(CONFIG_SDK_DRIVER_GPIO)
    RT_ERR_CHK_EHDL(gpio_probe(preused_unit), ret,
            {RT_INIT_ERR(ret, (MOD_INIT), "unit %u gpio_probe fail %d!\n", preused_unit, ret);});
    RT_ERR_CHK_EHDL(drv_gpio_init(preused_unit), ret,
            {RT_INIT_ERR(ret, (MOD_INIT), "unit %u drv_gpio_init fail %d!\n", preused_unit, ret);});
#endif
#if defined(CONFIG_SDK_RTL8231)
    RT_ERR_CHK_EHDL(rtl8231_probe(preused_unit), ret,
            {RT_INIT_ERR(ret, (MOD_INIT), "unit %u rtl8231_probe fail %d!\n", preused_unit, ret);});
    RT_ERR_CHK_EHDL(drv_rtl8231_init(preused_unit), ret,
            {RT_INIT_ERR(ret, (MOD_INIT), "unit %u drv_rtl8231_init fail %d!\n", preused_unit, ret);});
    if ((ret = drv_rtl8231_mdcSem_register(preused_unit, extGPIO_hwp_mdcSem_callback)) != RT_ERR_OK)
        return RT_ERR_FAILED;
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rt_util_boardID_GPIO_envUninit
 * Description:
 *      Uninit get boardID by GPIO environment
 * Input:
 *      N/A
 * Output:
 *      N/A
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
static int32 rt_util_boardID_GPIO_envUninit(void)
{
#if defined(CONFIG_SDK_RTL8231)
    int32 preused_unit = HWP_DEFHWP_UNIT_ID;
    int32 ret;
    if ((ret = drv_rtl8231_mdcSem_unregister(preused_unit, extGPIO_hwp_mdcSem_callback)) != RT_ERR_OK)
        return RT_ERR_FAILED;
#endif
    return RT_ERR_OK;
}

/* Function Name:
 *      rt_util_boardID_GPIO_init
 * Description:
 *      Get GPIO pin values by define
 * Input:
 *      N/A
 * Output:
 *      pPinValue      - GPIO pins' value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
int32 rt_util_boardID_GPIO_init(void)
{
    int loop;
    int unit = HWP_DEFHWP_UNIT_ID;
    rtk_enable_t enable_flag;
    drv_generalCtrlGpio_devConf_t gpio_conf;
    drv_generalCtrlGpio_pinConf_t pin_conf;

    rt_util_boardID_GPIO_envPrepare();

    RT_LOG(LOG_INFO, (MOD_INIT), "GET board id by GPIO: Total PIN number = %d", board_probe_pin_example.total_gpio);
    for(loop = 0; loop < board_probe_pin_example.total_gpio; loop++)
    {
        RT_LOG(LOG_INFO, (MOD_INIT), "\nPIN[%d], device = %d, address = %d, pin = %d, direction = %d",loop,board_probe_pin_example.pin[loop].gpio_device_id,board_probe_pin_example.pin[loop].address,board_probe_pin_example.pin[loop].gpio_id, board_probe_pin_example.pin[loop].direction);
        if(board_probe_pin_example.pin[loop].gpio_device_id == GEN_GPIO_DEV_ID0_INTERNAL)
        {
        }
        else if(board_probe_pin_example.pin[loop].gpio_device_id < GEN_GPIO_DEV_ID_END)
        {
            osal_memset(&gpio_conf, 0, sizeof(gpio_conf));
            osal_memset(&pin_conf, 0, sizeof(pin_conf));

            gpio_conf.direction = board_probe_pin_example.pin[loop].direction;
            gpio_conf.ext_gpio.access_mode = EXT_GPIO_ACCESS_MODE_MDC;
            gpio_conf.ext_gpio.address = board_probe_pin_example.pin[loop].address;
            gpio_conf.ext_gpio.page = 0;

            drv_generalCtrlGPIO_dev_init(unit, board_probe_pin_example.pin[loop].gpio_device_id, &gpio_conf);
            pin_conf.ext_gpio.direction = board_probe_pin_example.pin[loop].direction;
            pin_conf.direction = board_probe_pin_example.pin[loop].direction;
            drv_generalCtrlGPIO_pin_init(unit, board_probe_pin_example.pin[loop].gpio_device_id, board_probe_pin_example.pin[loop].gpio_id, &pin_conf);
            enable_flag = ENABLED;
            drv_generalCtrlGPIO_devEnable_set(unit, board_probe_pin_example.pin[loop].gpio_device_id, enable_flag);
        }
        else
        {
            RT_LOG(LOG_WARNING, (MOD_INIT), " \nGPIO Device ID is out of range.\n");
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      rt_util_flashVar_get
 * Description:
 *      Get GPIO pin values by define
 * Input:
 *      N/A
 * Output:
 *      pPinValue      - GPIO pins' value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
int32 rt_util_boardID_GPIO_get(uint32 *pPinValue)
{
    int loop;
    int unit = HWP_DEFHWP_UNIT_ID;
    uint32 probe_value = 0, pin_value = 0;

    for(loop = 0; loop < board_probe_pin_example.total_gpio; loop++)
    {
        if(board_probe_pin_example.pin[loop].gpio_device_id == GEN_GPIO_DEV_ID0_INTERNAL)
        {
            drv_generalCtrlGPIO_dataBit_get(unit, board_probe_pin_example.pin[loop].gpio_device_id, board_probe_pin_example.pin[loop].gpio_id, &pin_value);
            probe_value |= (pin_value << loop);
        }
        else if(board_probe_pin_example.pin[loop].gpio_device_id < GEN_GPIO_DEV_ID_END)
        {
            drv_generalCtrlGPIO_dataBit_get(unit, board_probe_pin_example.pin[loop].gpio_device_id, board_probe_pin_example.pin[loop].gpio_id, &pin_value);
            probe_value |= (pin_value << loop);
        }
        else
        {
            RT_LOG(LOG_WARNING, (MOD_INIT), " \nGPIO Device ID is out of range.\n");
            rt_util_boardID_GPIO_envUninit();
            return RT_ERR_FAILED;
        }
    }
    *pPinValue = probe_value;
    RT_LOG(LOG_INFO, (MOD_INIT), "GET board id by GPIO: PINs value = 0x%08x", probe_value);

    rt_util_boardID_GPIO_envUninit();
    return RT_ERR_OK;
}
#endif


