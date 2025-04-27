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
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/mac/reg.h>
#include <hal/mac/led/led_rtl8390.h>
#include <osal/print.h>
#include <common/util/rt_util_system.h>

 /*
 * Function Declaration
 */
uint32
_rtl8390_led_regDefault_init(uint32 unit)
{
    uint32 val = 0, i = 0;
    for(i = 0; i < RTK_MAX_NUM_OF_PORTS; i++)
    {
        /*set led default reg to 0*/
        reg_array_field_write(unit, CYPRESS_LED_FIB_PMASK_CTRLr, i, REG_ARRAY_INDEX_NONE, CYPRESS_FIB_PMASKf, &val);
        reg_array_field_write(unit, CYPRESS_LED_COPR_PMASK_CTRLr, i, REG_ARRAY_INDEX_NONE, CYPRESS_COPR_PMASKf, &val);
        reg_array_field_write(unit, CYPRESS_LED_COMBO_CTRLr, i, REG_ARRAY_INDEX_NONE, CYPRESS_LED_COMBOf, &val);
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8390_led_Config
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
int32 rtl8390_led_config(uint32 unit)
{
    uint32 i;
    uint32 active;
    uint32 val;

    _rtl8390_led_regDefault_init(unit);

    if(LED_ACTIVE_HIGH != HWP_LED_ACTIVE(unit))
        active = 0;/* Low */
    else
        active = 1;/* High */
    reg_field_write(unit, CYPRESS_LED_GLB_CTRLr, CYPRESS_LED_ACTIVEf, &active);

    /*config LED interface */
    if (LED_IF_SEL_NONE != HWP_LED_IF(unit))
    {
        switch (HWP_LED_IF(unit))
        {
            case LED_IF_SEL_SERIAL:
                i = 0;
                break;
            case LED_IF_SEL_SINGLE_COLOR_SCAN:
                i = 1;
                break;
            case LED_IF_SEL_BI_COLOR_SCAN:
                i = 2;
                break;
            default:
                osal_printf("Invalid LED IF SEL\n");
                return RT_ERR_FAILED;
        }
        reg_field_write(unit, CYPRESS_LED_GLB_CTRLr, CYPRESS_LED_IF_SELf, &i);

    }

    /* config LED number. the number of led is the same of each port*/
    HWP_PORT_TRAVS(unit, i)
    {
        if ((HWP_NONE == HWP_LED_COPR_SET(unit, i)) && (HWP_NONE == HWP_LED_FIB_SET(unit, i)))
            continue;
        if (HWP_PORT_LED_NUM(unit, i) <= RTK_MAX_LED_PER_PORT)
            val = HWP_PORT_LED_NUM(unit, i);
        else
            val = 0;
        reg_field_write(unit, CYPRESS_LED_GLB_CTRLr, CYPRESS_LED_NUM_SELf, &val);
        break;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, i){

        if ((HWP_NONE == HWP_LED_COPR_SET(unit, i)) && (HWP_NONE == HWP_LED_FIB_SET(unit, i)))
            continue;

        /*Specify copper led portmask*/
        if(HWP_LED_COPR_SET(unit, i) != HWP_NONE)
            val = 1;
        else
            val = 0;
        reg_array_field_write(unit, CYPRESS_LED_COPR_PMASK_CTRLr, i, REG_ARRAY_INDEX_NONE, CYPRESS_COPR_PMASKf, &val);

        /*Specify fiber led portmask*/
        if(HWP_LED_FIB_SET(unit, i) != HWP_NONE)
            val = 1;
        else
            val = 0;

        reg_array_field_write(unit, CYPRESS_LED_FIB_PMASK_CTRLr, i, REG_ARRAY_INDEX_NONE, CYPRESS_FIB_PMASKf, &val);

        /*Specify combo led portmask*/
        if (HWP_LED_LAYOUT(unit, i) == DOUBLE_SET)
            val = 0;
        else
            val = 1;
        reg_array_field_write(unit, CYPRESS_LED_COMBO_CTRLr, i, REG_ARRAY_INDEX_NONE, CYPRESS_LED_COMBOf, &val);

        /*Specify per port fiber LED set selection*/
        if (HWP_NONE != HWP_LED_COPR_SET(unit, i))
        {
            val =  HWP_LED_COPR_SET(unit, i);
            reg_array_field_write(unit, CYPRESS_LED_COPR_SET_SEL_CTRLr, i, REG_ARRAY_INDEX_NONE, CYPRESS_LED_COPR_SET_PSELf, &val);
        }
        if (HWP_NONE != HWP_LED_FIB_SET(unit, i))
        {
            val = HWP_LED_FIB_SET(unit, i);
            reg_array_field_write(unit, CYPRESS_LED_FIB_SET_SEL_CTRLr, i, REG_ARRAY_INDEX_NONE, CYPRESS_LED_FIB_SET_PSELf, &val);
        }
    }

    val = HWP_LED_MOD(unit, 0, 0);
    reg_field_write(unit, CYPRESS_LED_SET_0_1_CTRLr, CYPRESS_SET0_LED0_SELf, &val);
    val = HWP_LED_MOD(unit, 0, 1);
    reg_field_write(unit, CYPRESS_LED_SET_0_1_CTRLr, CYPRESS_SET0_LED1_SELf, &val);
    val = HWP_LED_MOD(unit, 0, 2);
    reg_field_write(unit, CYPRESS_LED_SET_0_1_CTRLr, CYPRESS_SET0_LED2_SELf, &val);
    val = HWP_LED_MOD(unit, 1, 0);
    reg_field_write(unit, CYPRESS_LED_SET_0_1_CTRLr, CYPRESS_SET1_LED0_SELf, &val);
    val = HWP_LED_MOD(unit, 1, 1);
    reg_field_write(unit, CYPRESS_LED_SET_0_1_CTRLr, CYPRESS_SET1_LED1_SELf, &val);
    val = HWP_LED_MOD(unit, 1, 2);
    reg_field_write(unit, CYPRESS_LED_SET_0_1_CTRLr, CYPRESS_SET1_LED2_SELf, &val);

    val = HWP_LED_MOD(unit, 2, 0);
    reg_field_write(unit, CYPRESS_LED_SET_2_3_CTRLr, CYPRESS_SET2_LED0_SELf, &val);
    val = HWP_LED_MOD(unit, 2, 1);
    reg_field_write(unit, CYPRESS_LED_SET_2_3_CTRLr, CYPRESS_SET2_LED1_SELf, &val);
    val = HWP_LED_MOD(unit, 2, 2);
    reg_field_write(unit, CYPRESS_LED_SET_2_3_CTRLr, CYPRESS_SET2_LED2_SELf, &val);
    val = HWP_LED_MOD(unit, 3, 0);
    reg_field_write(unit, CYPRESS_LED_SET_2_3_CTRLr, CYPRESS_SET3_LED0_SELf, &val);
    val = HWP_LED_MOD(unit, 3, 1);
    reg_field_write(unit, CYPRESS_LED_SET_2_3_CTRLr, CYPRESS_SET3_LED1_SELf, &val);
    val = HWP_LED_MOD(unit, 3, 2);
    reg_field_write(unit, CYPRESS_LED_SET_2_3_CTRLr, CYPRESS_SET3_LED2_SELf, &val);

    /* enable LED */
    val = 1;
    reg_field_write(unit, CYPRESS_LED_GLB_CTRLr, CYPRESS_LED_ENf, &val);

    return RT_ERR_OK;
}/* end of rtl8390_led_config */
