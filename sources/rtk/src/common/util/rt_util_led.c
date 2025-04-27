/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:
 * $Date:
 *
 * Purpose : Define the led utility macro and function in the SDK.
 *
 * Feature : SDK common utility
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/util/rt_util.h>
#include <common/rt_error.h>
#include <hwp/hw_profile.h>
#include <hal/mac/reg.h>
#include <hal/chipdef/allreg.h>
#if defined(CONFIG_SDK_RTL8390)
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#endif
#if defined(CONFIG_SDK_RTL9300)
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#endif
#include <osal/print.h>
#include <common/util/rt_util_led.h>
#include <common/error.h>
#include <drv/gpio/generalCtrl_gpio.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32 led_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};

/*
 * Macro Definition
 */
#define LED_INIT_CHK(unit)                      if (led_init[unit] == INIT_NOT_COMPLETED) {\
                                                    rtk_boxID_led_init(unit);\
                                                    led_init[unit] = INIT_COMPLETED;\
                                                }

/*
 * Function Declaration
 */
void _rtk_swled_out(unsigned int unit, int index, unsigned int *bit)
{
    uint32 port;
#if defined(CONFIG_SDK_RTL8390)
    uint32 value;

    value = 1;
#endif

    HWP_PORT_TRAVS(unit, port)
    {
#if defined(CONFIG_SDK_RTL8390)
        if(HWP_8390_FAMILY_ID(unit))
        {
            uint32 field;

            if(0 == bit[port])
            {
                reg_array_write(unit, CYPRESS_LED_SW_P_CTRLr, port, REG_ARRAY_INDEX_NONE, &bit[port]);
            }
            else
            {
                switch (index)
                {
                    case 0:
                        field = CYPRESS_SW_COPR_LED0_MODEf;
                        break;
                    case 1:
                        field = CYPRESS_SW_COPR_LED1_MODEf;
                        break;
                    case 2:
                        field = CYPRESS_SW_COPR_LED2_MODEf;
                        break;
                    default:
                        return;

                }

                reg_array_field_write(unit, CYPRESS_LED_SW_P_CTRLr, port,
                                        REG_ARRAY_INDEX_NONE, field, &bit[port]);
            }
        }
#endif
    }
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_FAMILY_ID(unit))
    {
        value = 1;
        reg_write(unit, CYPRESS_LED_SW_CTRLr, &value);
    }
#endif
    return;
}

/* Function Name:
 *      rtk_swled_on
 * Description:
 *      light on the led in runtime
 * Input:
 *      unit   - unit id
 *      start  - start port
 *      range  - the range of light on led
 *      index  - the led index of the port
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 */
void rtk_swled_on(uint32 unit, uint32 start, uint32 range, uint32 index)
{
    int portNum;
    unsigned int bit[RTK_MAX_PORT_PER_UNIT];
#if defined(CONFIG_SDK_RTL8390)
    uint32 value;

    value = 0;
#endif

    if (NULL == HWP_SWITCH_DESCP(unit))
    {
        osal_printf("\n led test: Cant get board model\n");
        return;
    }

    for(portNum = start; portNum < (start + range); portNum++)
    {
        if (!HWP_PORT_EXIST(unit, portNum))
        {
            osal_printf("The port (%d) is invalid.\n", portNum);
            return;
        }
    }

    HWP_PORT_TRAVS(unit, portNum)
    {
        if ((index < 0) || (index > HWP_PORT_LED_NUM(unit, start)))
        {
            osal_printf("The led index (%d) is invalid.\n", index);
            return;
        }

#if defined(CONFIG_SDK_RTL8390)
        if(HWP_8390_FAMILY_ID(unit))
        {
            reg_array_field_read(unit, CYPRESS_LED_SW_P_EN_CTRLr, portNum,
                REG_ARRAY_INDEX_NONE, CYPRESS_SW_CTRL_LED_ENf, &value);
            value = 0x7;
            reg_array_field_write(unit, CYPRESS_LED_SW_P_EN_CTRLr, portNum,
                REG_ARRAY_INDEX_NONE, CYPRESS_SW_CTRL_LED_ENf, &value);
        }
#endif
        bit[portNum] = 0;

    }

    for(portNum = start; portNum < (start + range); portNum++)
    {
        bit[portNum] = 0x07;
    }
    _rtk_swled_out(unit, index, bit);

}

#ifdef CONFIG_SDK_RTL9300
/* Function Name:
 *      _rtl9300_boxID_led_on
 * Description:
 *      Set 9300 box id led.
 * Input:
 *      unit   - unit id
 *      box    - box id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 */
void _rtl9300_unitID_led_set(uint32 unit, uint32 boxId)
{
#if defined(CONFIG_SDK_RTL8231)

    uint32 i;
    uint32 val;

    /*indirect access*/
    for(i = 0; i < RTL9300_BOX_ID_LED_NUM; i++)
    {
        val = (boxId & (1 << (RTL9300_BOX_ID_LED_NUM - i - 1))) >> (RTL9300_BOX_ID_LED_NUM - i - 1);
        drv_generalCtrlGPIO_dataBit_set(unit, RTL9300_BOX_ID_LED_GPIO_DEVID, (RTL9300_BOX_ID_LED_GPIO_BASE + i), val);
    }
#endif
}

/* Function Name:
 *      _rtl9300_boxID_led_init
 * Description:
 *      Initial box id led.
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 */
void _rtl9300_boxID_led_init(uint32 unit)
{
#if defined(CONFIG_SDK_RTL8231)

    drv_generalCtrlGpio_devId_t dev = RTL9300_BOX_ID_LED_GPIO_DEVID;
    drv_generalCtrlGpio_devConf_t dev_conf;
    drv_generalCtrlGpio_pinConf_t genCtrl_gpio;
    int32 i;

    osal_memset(&dev_conf, 0, sizeof(drv_generalCtrlGpio_devConf_t));

    dev_conf.ext_gpio.access_mode = EXT_GPIO_ACCESS_MODE_MDC;
    dev_conf.ext_gpio.page = 0;
    dev_conf.ext_gpio.address = 0;
    dev_conf.default_value = 1;
    dev_conf.direction = GPIO_DIR_OUT;

    drv_generalCtrlGPIO_dev_init(unit, dev, &dev_conf);

    for(i = 0; i < RTL9300_BOX_ID_LED_NUM; i++)
    {
        genCtrl_gpio.default_value = 0;
        genCtrl_gpio.direction = GPIO_DIR_OUT;
        genCtrl_gpio.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
        genCtrl_gpio.int_gpio.interruptEnable = GPIO_INT_DISABLE;
        genCtrl_gpio.ext_gpio.direction = GPIO_DIR_OUT;
        genCtrl_gpio.ext_gpio.debounce = 0;
        genCtrl_gpio.ext_gpio.inverter = 0;
        drv_generalCtrlGPIO_pin_init(unit, RTL9300_BOX_ID_LED_GPIO_DEVID, (RTL9300_BOX_ID_LED_GPIO_BASE + i), &genCtrl_gpio);
    }
    drv_generalCtrlGPIO_devEnable_set(unit, RTL9300_BOX_ID_LED_GPIO_DEVID, ENABLED);
#endif
}

/* Function Name:
 *      _rtl9300_masterLedEnable_set
 * Description:
 *      Set stacking master led state.
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 */
void _rtl9300_masterLedEnable_set(uint32 unit, rtk_enable_t enable)
{
    uint32 val = 0;

    if(ENABLED == enable)
    {
        val = 1;/*Enable system LED*/
        reg_field_write(unit, LONGAN_LED_GLB_CTRLr, LONGAN_SYS_LED_ENf, &val);

        val = 3;/*Light */
        reg_field_write(unit, LONGAN_LED_GLB_CTRLr, LONGAN_SYS_LED_MODEf, &val);
    }
    else
    {
        val = 0;/*Enable GPIO*/
        reg_field_write(unit, LONGAN_LED_GLB_CTRLr, LONGAN_SYS_LED_ENf, &val);
    }
}
#endif

/* Function Name:
 *      rtk_boxID_led_init
 * Description:
 *      Initial box id led.
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 */
int32 rtk_boxID_led_init(uint32 unit)
{

#ifdef CONFIG_SDK_RTL9300
    if (HWP_9300_FAMILY_ID(unit))
    {
        _rtl9300_boxID_led_init(unit);
    }
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_boxID_led_on
 * Description:
 *      Set the box id led.
 * Input:
 *      unit   - unit id
 *      box    - box id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 */
int32 rtk_boxID_led_set(uint32 unit, uint32 boxId)
{
    LED_INIT_CHK(unit);

#ifdef CONFIG_SDK_RTL9300
    if (HWP_9300_FAMILY_ID(unit))
    {
        _rtl9300_unitID_led_set(unit, boxId);
    }
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_masterLedEnable_set
 * Description:
 *      Set stacking master led state.
 * Input:
 *      unit   - unit id
 *      enable - master led state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 */
int32 rtk_masterLedEnable_set(uint32 unit, rtk_enable_t enable)
{

#ifdef CONFIG_SDK_RTL9300
    if (HWP_9300_FAMILY_ID(unit))
    {
        _rtl9300_masterLedEnable_set(unit, enable);
    }
#endif

    return RT_ERR_OK;
}


