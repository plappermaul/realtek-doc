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
 * $Revision:
 * $Date:
 *
 * Purpose : Config led in the SDK.
 *
 * Feature : mac driver service APIs
 *
 */

#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <hwp/hw_profile.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/mac/reg.h>
#include <hal/mac/led/led_rtl9300.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <common/util/rt_util_system.h>
#include <rtk/default.h>


 /*
 * Function Declaration
 */

uint32
_rtl9300_led_regDefault_init(uint32 unit)
{
    uint32 val = 0;

    /*set led default reg to 0*/
    reg_write(unit, LONGAN_LED_PORT_COPR_MASK_CTRLr, &val);
    reg_write(unit, LONGAN_LED_PORT_FIB_MASK_CTRLr, &val);
    reg_write(unit, LONGAN_LED_PORT_COMBO_MASK_CTRLr, &val);

    return RT_ERR_OK;
}

uint32 _rtl9300_led_num_config(uint32 unit)
{
    uint32 i = 0;
    uint32 val = 0;

    /* config LED number.*/
    HWP_PORT_TRAVS(unit, i)
    {
        if( HWP_PORT_LED_NUM(unit, i) <= RTK_MAX_LED_PER_PORT)
        {
            switch(HWP_PORT_LED_NUM(unit, i))
            {
                case 1:
                    val = 0;
                    break;
                case 2:
                    val = 1;
                    break;
                case 3:
                    val = 2;
                    break;
                case 4:
                    val = 3;
                    break;
                default:
                    val = 0;
                    break;
            }
            reg_array_field_write(unit, LONGAN_LED_PORT_NUM_CTRLr, i, REG_ARRAY_INDEX_NONE, LONGAN_LED_NUM_SELf, &val);
        }
        else
        {
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}
uint32 _rtl9300_led_interface_select(uint32 unit)
{
    uint32 active = 0;
    uint32 i, pwron_blink;

    if(LED_ACTIVE_HIGH != HWP_LED_ACTIVE(unit))
        active = 0;
    else
        active = 1;
    reg_field_write(unit, LONGAN_LED_GLB_CTRLr, LONGAN_LED_ACTIVEf, &active);

    /* power-on blinking select */
    pwron_blink = RTK_DEFAULT_LED_POWER_ON_BLINK;
    switch (pwron_blink)
    {
      case RTK_LED_PWR_ON_BLINK_DISABLE:
        i = 0x0;
        break;
      case RTK_LED_PWR_ON_BLINK_400MS:
        i = 0x1;
        break;
      case RTK_LED_PWR_ON_BLINK_800MS:
        i = 0x2;
        break;
      case RTK_LED_PWR_ON_BLINK_1600MS:
        i = 0x3;
        break;
      default:
        i = 0x2;
        break;
    } /* end switch */
    reg_field_write(unit, LONGAN_LED_GLB_CTRLr, LONGAN_PWR_ON_BLINK_SELf, &i);



    /*config LED interface */
    if (LED_IF_SEL_NONE != HWP_LED_IF(unit))
    {
        switch (HWP_LED_IF(unit))
        {
            case LED_IF_SEL_SERIAL:
                i = 1;
                break;
            case LED_IF_SEL_SINGLE_COLOR_SCAN:
                i = 2;
                break;
            case LED_IF_SEL_BI_COLOR_SCAN:
                i = 3;
                break;
            default:
                return RT_ERR_FAILED;
        }
        reg_field_write(unit, LONGAN_LED_GLB_CTRLr, LONGAN_LED_MODf, &i);
    }

    return RT_ERR_OK;
}

uint32 _rtl9300_led_portMsk_config(uint32 unit)
{
    uint32 val;
    uint32 port;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port){
        if ((HWP_NONE == HWP_LED_COPR_SET(unit, port)) && (HWP_NONE == HWP_LED_FIB_SET(unit, port)))
            continue;
        /*Specify copper led portmask*/
        if(HWP_LED_COPR_SET(unit, port) != HWP_NONE)
            val = 1;
        else
            val = 0;
        reg_array_field_write(unit, LONGAN_LED_PORT_COPR_MASK_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_LED_COPR_PMASKf, &val);

        /*Specify fiber led portmask*/
        if(HWP_LED_FIB_SET(unit, port) != HWP_NONE)
            val = 1;
        else
            val = 0;
        reg_array_field_write(unit, LONGAN_LED_PORT_FIB_MASK_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_LED_FIB_PMASKf, &val);

        /*Specify combo led portmask*/
        if (HWP_LED_LAYOUT(unit, port) == DOUBLE_SET)
            val = 0;
        else
            val = 1;
        reg_array_field_write(unit, LONGAN_LED_PORT_COMBO_MASK_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_LED_COMBO_PMASKf, &val);

        /*Specify per port fiber LED set selection*/
        if (HWP_LED_COPR_SET(unit, port) != HWP_NONE)
        {
            val =  HWP_LED_COPR_SET(unit, port);
            reg_array_field_write(unit, LONGAN_LED_PORT_COPR_SET_SEL_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_LED_COPR_SET_PSELf, &val);
        }
        if (HWP_LED_FIB_SET(unit, port) != HWP_NONE)
        {
            val = HWP_LED_FIB_SET(unit, port);
            reg_array_field_write(unit, LONGAN_LED_PORT_FIB_SET_SEL_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_LED_FIB_SET_PSELf, &val);
        }
    }

    return RT_ERR_OK;
}
uint32 _rtl9300_led_mode_config(uint32 unit)
{
    uint32 val;

    val = HWP_LED_MOD(unit, 0, 0);
    reg_field_write(unit, LONGAN_LED_SET0_0_CTRLr, LONGAN_SET0_LED0_SELf, &val);
    val = HWP_LED_MOD(unit, 0, 1);
    reg_field_write(unit, LONGAN_LED_SET0_0_CTRLr, LONGAN_SET0_LED1_SELf, &val);
    val = HWP_LED_MOD(unit, 0, 2);
    reg_field_write(unit, LONGAN_LED_SET0_1_CTRLr, LONGAN_SET0_LED2_SELf, &val);
    val = HWP_LED_MOD(unit, 0, 3);
    reg_field_write(unit, LONGAN_LED_SET0_1_CTRLr, LONGAN_SET0_LED3_SELf, &val);

    val = HWP_LED_MOD(unit, 1, 0);
    reg_field_write(unit, LONGAN_LED_SET1_0_CTRLr, LONGAN_SET1_LED0_SELf, &val);
    val = HWP_LED_MOD(unit, 1, 1);
    reg_field_write(unit, LONGAN_LED_SET1_0_CTRLr, LONGAN_SET1_LED1_SELf, &val);
    val = HWP_LED_MOD(unit, 1, 2);
    reg_field_write(unit, LONGAN_LED_SET1_1_CTRLr, LONGAN_SET1_LED2_SELf, &val);
    val = HWP_LED_MOD(unit, 1, 3);
    reg_field_write(unit, LONGAN_LED_SET1_1_CTRLr, LONGAN_SET1_LED3_SELf, &val);

    val = HWP_LED_MOD(unit, 2, 0);
    reg_field_write(unit, LONGAN_LED_SET2_0_CTRLr, LONGAN_SET2_LED0_SELf, &val);
    val = HWP_LED_MOD(unit, 2, 1);
    reg_field_write(unit, LONGAN_LED_SET2_0_CTRLr, LONGAN_SET2_LED1_SELf, &val);
    val = HWP_LED_MOD(unit, 2, 2);
    reg_field_write(unit, LONGAN_LED_SET2_1_CTRLr, LONGAN_SET2_LED2_SELf, &val);
    val = HWP_LED_MOD(unit, 2, 3);
    reg_field_write(unit, LONGAN_LED_SET2_1_CTRLr, LONGAN_SET2_LED3_SELf, &val);

    val = HWP_LED_MOD(unit, 3, 0);
    reg_field_write(unit, LONGAN_LED_SET3_0_CTRLr, LONGAN_SET3_LED0_SELf, &val);
    val = HWP_LED_MOD(unit, 3, 1);
    reg_field_write(unit, LONGAN_LED_SET3_0_CTRLr, LONGAN_SET3_LED1_SELf, &val);
    val = HWP_LED_MOD(unit, 3, 2);
    reg_field_write(unit, LONGAN_LED_SET3_1_CTRLr, LONGAN_SET3_LED2_SELf, &val);
    val = HWP_LED_MOD(unit, 3, 3);
    reg_field_write(unit, LONGAN_LED_SET3_1_CTRLr, LONGAN_SET3_LED3_SELf, &val);

    return RT_ERR_OK;
}
void _rtl9300_led_stack_sync_config(uint32 unit)
{
    uint32 val;
    if(HWP_CASCADE_MODE() == TRUE)
    {
        if (HWP_IS_CPU_UNIT(unit))
        {
            /*0b01:output the triger signal*/
            val = 1;
            reg_field_write(unit, LONGAN_LED_GLB_CTRLr, LONGAN_LED_STACKf, &val);
        }
        else
        {
            /*0b10:input the triger signal*/
            val = 2;
            reg_field_write(unit, LONGAN_LED_GLB_CTRLr, LONGAN_LED_STACKf, &val);
        }
        return;
    }

    val = 0;
    reg_field_write(unit, LONGAN_LED_GLB_CTRLr, LONGAN_LED_STACKf, &val);

    return;
}
/* Function Name:
 *      rtl9300_led_Config
 * Description:
 *      Initialize LED module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32 rtl9300_led_config(uint32 unit)
{
    uint32 ret = RT_ERR_FAILED;

    _rtl9300_led_stack_sync_config(unit);
    _rtl9300_led_regDefault_init(unit);
    _rtl9300_led_num_config(unit);
    _rtl9300_led_portMsk_config(unit);
    _rtl9300_led_mode_config(unit);

    /* enable LED */

    if ((ret = _rtl9300_led_interface_select(unit)) != RT_ERR_OK)
    {
        osal_printf("Invalid LED IF SEL\n");
        return ret;
    }

    return RT_ERR_OK;

}/* end of rtl9300_led_config */

