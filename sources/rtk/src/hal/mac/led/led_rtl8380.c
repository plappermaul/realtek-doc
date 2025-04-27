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
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/mac/reg.h>
#include <hal/mac/led/led_rtl8380.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <common/util/rt_util_system.h>


/* Function Name:
 *      rtl8380_led_config
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
int32 rtl8380_led_config(uint32 unit)
{
    uint32 val = 0;
    uint32 i = 0;
    uint32 j = 0;

    /*LED NUMBER*/
    /*Set Port0-Port23 LED Number*/
    HWP_PORT_TRAVS(unit, i)
    {
        if(HWP_PORT_LED_NUM(unit, i) > RTK_MAX_LED_PER_PORT)
        {
           val = 0;
        }
        else
        {
            for(j = 0; j < HWP_PORT_LED_NUM(unit, i); j++)
               val |= (1 << j);
        }
        break;

    }
    reg_field_write(unit, MAPLE_LED_GLB_CTRLr, MAPLE_LED_MASK_SEL_2_0f, &val);
    /*[SS-466] P27_24_LED_MASK_SEL and MAPLE_LED_MASK_SEL_2_0f must be the same when p24-27 are not used.*/
    reg_field_write(unit, MAPLE_LED_GLB_CTRLr, MAPLE_P27_24_LED_MASK_SELf, &val);

    /*Set Port24-Port27 LED Number*/
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, i)
    {
        if(i > 23)
        {
            val = 0;
            if(HWP_PORT_LED_NUM(unit, i) > RTK_MAX_LED_PER_PORT)
            {
               val = 0;
            }
            else
            {
                for (j = 0; j < HWP_PORT_LED_NUM(unit, i); j++)
                    val |= (1 << j);
            }
            /*[SS-466]when p24-27 are used, get value from hardware profile and renew register.*/
            reg_field_write(unit, MAPLE_LED_GLB_CTRLr, MAPLE_P27_24_LED_MASK_SELf, &val);
            break;
        }
    }

    /*Combo Port LED mode*/
    HWP_PORT_TRAVS(unit, i)
    {
       if ((HWP_LED_LAYOUT(unit, i) == DOUBLE_SET) && (i <= 23) && (i > 20))
       {
           val = 0x1;
           reg_field_write(unit, MAPLE_LED_GLB_CTRLr, MAPLE_COMBO_PORT_MODEf, &val);
           break;
       }
       if ((HWP_LED_LAYOUT(unit, i) == DOUBLE_SET) && (i > 23))
       {
           val = 0x2;
           reg_field_write(unit, MAPLE_LED_GLB_CTRLr, MAPLE_COMBO_PORT_MODEf, &val);
           break;
       }
    }

    /*LED MODE CONTROL*/
    val = HWP_LED_MOD(unit, 0, 0);
    reg_field_write(unit, MAPLE_LED_MODE_CTRLr, MAPLE_P23_0_LED0_MODE_SELf, &val);
    val = HWP_LED_MOD(unit, 0, 1);
    reg_field_write(unit, MAPLE_LED_MODE_CTRLr, MAPLE_P23_0_LED1_MODE_SELf, &val);
    val = HWP_LED_MOD(unit, 0, 2);
    reg_field_write(unit, MAPLE_LED_MODE_CTRLr, MAPLE_P23_0_LED2_MODE_SELf, &val);
    val = HWP_LED_MOD(unit, 1, 0);
    reg_field_write(unit, MAPLE_LED_MODE_CTRLr, MAPLE_P27_24_LED0_MODE_SELf, &val);
    val = HWP_LED_MOD(unit, 1, 1);
    reg_field_write(unit, MAPLE_LED_MODE_CTRLr, MAPLE_P27_24_LED1_MODE_SELf, &val);
    val = HWP_LED_MOD(unit, 1, 2);
    reg_field_write(unit, MAPLE_LED_MODE_CTRLr, MAPLE_P27_24_LED2_MODE_SELf, &val);

    /*Specify per port LED enable control register.*/
    val = 0;
    HWP_PORT_TRAVS(unit, i)
    {
       val |= (1 << i);
    }
    reg_write(unit, MAPLE_LED_P_EN_CTRLr, &val);

    /*Power on blinking 800m second*/
    val = 0x1;
    reg_field_write(unit, MAPLE_LED_MODE_SELr, MAPLE_PWR_ON_BLINK_SELf, &val);

    /*Config LED interface and enable led. */

    if (LED_IF_SEL_NONE != HWP_LED_IF(unit))
    {
        switch (HWP_LED_IF(unit))
        {
            case LED_IF_SEL_SERIAL:
                val = 0;
                break;
            case LED_IF_SEL_SINGLE_COLOR_SCAN:
                val = 1;
                break;
            case LED_IF_SEL_BI_COLOR_SCAN:
                val = 2;
                break;
            default:
                osal_printf("Invalid LED IF SEL\n");
                return RT_ERR_FAILED;
        }
        reg_field_write(unit, MAPLE_LED_MODE_SELr, MAPLE_LED_MODE_SELf, &val);
    }

    return RT_ERR_OK;

}/* end of rtl8380_led_config */

