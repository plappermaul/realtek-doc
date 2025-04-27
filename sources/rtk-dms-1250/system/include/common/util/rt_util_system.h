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
 * Purpose : Definition the common utilities in the SDK.
 *
 * Feature : type definition
 *
 */

#ifndef __BD_MODUEL_PROVIDER_H__
#define __BD_MODUEL_PROVIDER_H__

#ifdef CONFIG_SDK_HW_PROFILE_PROBE_GPIO
#include <drv/gpio/generalCtrl_gpio.h>
#endif


/*
 * Symbol Definition
 */
#ifdef CONFIG_SDK_HW_PROFILE_PROBE_GPIO
#define BOARD_PROBE_PIN_NUMBER              2
#define HWP_GPIO_PROBE_USED_UNIT_ID         0
#define HWP_GPIO_PROBE_USED_DEVICE_ID       1
#define HWP_GPIO_PROBE_USED_SWCORE_BASE     0xBB000000
#endif

#define ENV_PARAMETER_LEN                   1024

#define LEDMODEINITSKIP_LEN                 16
#define LEDMODEINITSKIP_INIT_VALUE          0x99
#define LEDMODEINITSKIP_YES                 1       /* the ledModeInitSkip in U-Boot: 1: skip; 0: as normal */
#define LEDMODEINITSKIP_NO                  0


/*
 * Data Type Declaration
 */
#ifdef CONFIG_SDK_HW_PROFILE_PROBE_GPIO
    typedef struct board_probe_gpio_pin_e
    {
        drv_generalCtrlGpio_devId_t    gpio_device_id; /*Use to check Internal/External GPIO*/
        int                            address;
        int                            gpio_id;
        drv_gpio_direction_t           direction;
    }board_probe_gpio_pin_t;


    typedef struct board_probe_gpio_e
    {
        int                        total_gpio;
        board_probe_gpio_pin_t     pin[BOARD_PROBE_PIN_NUMBER];
    }board_probe_gpio_t;
#endif

/*
 * Macro Definition
 */
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
extern int32 rt_util_flashVar_get(char *addr, uint32 len, char *name, char *pValue, int valueBufLen);
#endif
#ifndef __KERNEL__
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
extern int32 rt_util_flashVar_mtd_get(uint32 mtd_idx, char *name, char *pValue, int valueBufLen);
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
extern int32 rt_util_sysMac_get(rtk_mac_t *pMacAddr);
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
extern int32 rt_util_ascii2hex(char *pAscii, uint8 *pHexValue);

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
extern int32 rt_util_ascii2dec(char *pAscii, uint8 *pDecValue);

#ifdef CONFIG_SDK_HW_PROFILE_PROBE_GPIO
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
extern int32 rt_util_boardID_GPIO_init(void);

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
extern int32 rt_util_boardID_GPIO_get(uint32 *pPinValue);
#endif


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
int32 rt_util_ledInitFlag_get(uint8 *ledModeInitSkip);

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
int32 rt_util_flashEnv_get(char *name, char *pValue, int buffLen);

#endif /* __BD_MODUEL_PROVIDER_H__ */

