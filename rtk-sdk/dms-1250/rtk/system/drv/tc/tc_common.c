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
 * $Revision$
 * $Date$
 *
 * Purpose : I2C master driver.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) i2c read and write
 *
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <private/drv/tc/tc_common.h>
#include <private/drv/tc/tc_mapper.h>
#include <ioal/mem32.h>
#include <drv/tc/tc.h>
#include <common/rt_type.h>
/*
 * Include Files
 */


/*
 * Symbol Definition
 */



/*
 * Data Declaration
 */
static uint32   tc_init_status[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};

tc_reg_definition_t tc_reg[TC_CTRL_END] =
{
#if defined(CONFIG_SDK_RTL8390)
    {
        .tmr_cntr_data_addr =
        {
            {.reg_addr = RTL8390_TC0DATA_ADDR},
            {.reg_addr = RTL8390_TC1DATA_ADDR},
            {.reg_addr = RTL8390_TC2DATA_ADDR},
            {.reg_addr = RTL8390_TC3DATA_ADDR},
            {.reg_addr = RTL8390_TC4DATA_ADDR},
        },
        .tmr_cntr_cntr_addr =
        {
            {.reg_addr = RTL8390_TC0CNT_ADDR},
            {.reg_addr = RTL8390_TC1CNT_ADDR},
            {.reg_addr = RTL8390_TC2CNT_ADDR},
            {.reg_addr = RTL8390_TC3CNT_ADDR},
            {.reg_addr = RTL8390_TC4CNT_ADDR},
        },
        .tmr_cntr_ctrl_addr =
        {
            {.reg_addr = RTL8390_TC0CTL_ADDR},
            {.reg_addr = RTL8390_TC1CTL_ADDR},
            {.reg_addr = RTL8390_TC2CTL_ADDR},
            {.reg_addr = RTL8390_TC3CTL_ADDR},
            {.reg_addr = RTL8390_TC4CTL_ADDR},
        },
        .tmr_cntr_intr_addr =
        {
            {.reg_addr = RTL8390_TC0INT_ADDR},
            {.reg_addr = RTL8390_TC1INT_ADDR},
            {.reg_addr = RTL8390_TC2INT_ADDR},
            {.reg_addr = RTL8390_TC3INT_ADDR},
            {.reg_addr = RTL8390_TC4INT_ADDR},
        },
    },
#endif
#if defined(CONFIG_SDK_RTL8380)
    {
        .tmr_cntr_data_addr =
        {
            {.reg_addr = RTL8380_TMR_CNTR_0_DATA_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_1_DATA_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_2_DATA_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_3_DATA_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_4_DATA_ADDR},
        },
        .tmr_cntr_cntr_addr =
        {
            {.reg_addr = RTL8380_TMR_CNTR_0_CNTR_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_1_CNTR_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_2_CNTR_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_3_CNTR_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_4_CNTR_ADDR},
        },
        .tmr_cntr_ctrl_addr =
        {
            {.reg_addr = RTL8380_TMR_CNTR_0_CTRL_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_1_CTRL_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_2_CTRL_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_3_CTRL_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_4_CTRL_ADDR},
        },
        .tmr_cntr_intr_addr =
        {
            {.reg_addr = RTL8380_TMR_CNTR_0_INTR_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_1_INTR_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_2_INTR_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_3_INTR_ADDR},
            {.reg_addr = RTL8380_TMR_CNTR_4_INTR_ADDR},
        },
    },
#endif
#if (defined(CONFIG_SDK_RTL9300))
{
    .tmr_cntr_data_addr =
    {
        {.reg_addr = RTL9300_TC0DATA_ADDR},
        {.reg_addr = RTL9300_TC1DATA_ADDR},
        {.reg_addr = RTL9300_TC2DATA_ADDR},
        {.reg_addr = RTL9300_TC3DATA_ADDR},
        {.reg_addr = RTL9300_TC4DATA_ADDR},
        {.reg_addr = RTL9300_TC5DATA_ADDR},
    },
    .tmr_cntr_cntr_addr =
    {
        {.reg_addr = RTL9300_TC0CNT_ADDR},
        {.reg_addr = RTL9300_TC1CNTR_ADDR},
        {.reg_addr = RTL9300_TC2CNTR_ADDR},
        {.reg_addr = RTL9300_TC3CNTR_ADDR},
        {.reg_addr = RTL9300_TC4CNTR_ADDR},
        {.reg_addr = RTL9300_TC5CNTR_ADDR},
    },
    .tmr_cntr_ctrl_addr =
    {
        {.reg_addr = RTL9300_TC0CTRL_ADDR},
        {.reg_addr = RTL9300_TC1CTRL_ADDR},
        {.reg_addr = RTL9300_TC2CTRL_ADDR},
        {.reg_addr = RTL9300_TC3CTRL_ADDR},
        {.reg_addr = RTL9300_TC4CTRL_ADDR},
        {.reg_addr = RTL9300_TC5CTRL_ADDR},
    },
    .tmr_cntr_intr_addr =
    {
        {.reg_addr = RTL9300_TC0INTR_ADDR},
        {.reg_addr = RTL9300_TC1INTR_ADDR},
        {.reg_addr = RTL9300_TC2INTR_ADDR},
        {.reg_addr = RTL9300_TC3INTR_ADDR},
        {.reg_addr = RTL9300_TC4INTR_ADDR},
        {.reg_addr = RTL9300_TC5INTR_ADDR},
    },
},
#endif
#if (defined(CONFIG_SDK_RTL9310))
{
    .tmr_cntr_data_addr =
    {
        {.reg_addr = RTL9310_TC0DATA_ADDR},
        {.reg_addr = RTL9310_TC1DATA_ADDR},
        {.reg_addr = RTL9310_TC2DATA_ADDR},
        {.reg_addr = RTL9310_TC3DATA_ADDR},
        {.reg_addr = RTL9310_TC4DATA_ADDR},
        {.reg_addr = RTL9310_TC5DATA_ADDR},
        {.reg_addr = RTL9310_TC6DATA_ADDR},
    },
    .tmr_cntr_cntr_addr =
    {
        {.reg_addr = RTL9310_TC0CNT_ADDR},
        {.reg_addr = RTL9310_TC1CNTR_ADDR},
        {.reg_addr = RTL9310_TC2CNTR_ADDR},
        {.reg_addr = RTL9310_TC3CNTR_ADDR},
        {.reg_addr = RTL9310_TC4CNTR_ADDR},
        {.reg_addr = RTL9310_TC5CNTR_ADDR},
        {.reg_addr = RTL9310_TC6CNTR_ADDR},
    },
    .tmr_cntr_ctrl_addr =
    {
        {.reg_addr = RTL9310_TC0CTRL_ADDR},
        {.reg_addr = RTL9310_TC1CTRL_ADDR},
        {.reg_addr = RTL9310_TC2CTRL_ADDR},
        {.reg_addr = RTL9310_TC3CTRL_ADDR},
        {.reg_addr = RTL9310_TC4CTRL_ADDR},
        {.reg_addr = RTL9310_TC5CTRL_ADDR},
        {.reg_addr = RTL9310_TC6CTRL_ADDR},
    },
    .tmr_cntr_intr_addr =
    {
        {.reg_addr = RTL9310_TC0INTR_ADDR},
        {.reg_addr = RTL9310_TC1INTR_ADDR},
        {.reg_addr = RTL9310_TC2INTR_ADDR},
        {.reg_addr = RTL9310_TC3INTR_ADDR},
        {.reg_addr = RTL9310_TC4INTR_ADDR},
        {.reg_addr = RTL9310_TC5INTR_ADDR},
        {.reg_addr = RTL9310_TC6INTR_ADDR},
    },
},
#endif

};

int32 tc_init(uint32 unit)
{

    RT_INIT_REENTRY_CHK(tc_init_status[unit]);

    tc_init_status[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}

int32 tc_enable_set(uint32 unit,drv_tc_id_t id, rtk_enable_t enable)
{
    tc_ctlReg_t     ctr_reg;

    /* set control register */
    ioal_soc_mem32_read(unit, tc_reg_CTL(unit, id), (uint32 *)&ctr_reg);
    if (ENABLED == enable)
        ctr_reg.u.field.tcEnable = 1;
    else
        ctr_reg.u.field.tcEnable = 0;
    ioal_soc_mem32_write(unit, tc_reg_CTL(unit, id), *((uint32 *)&ctr_reg));

    return RT_ERR_OK;
}

int32 tc_mode_set(uint32 unit, drv_tc_id_t id, drv_tc_mode_t mode)
{
    tc_ctlReg_t     ctr_reg;

    ioal_soc_mem32_read(unit, tc_reg_CTL(unit, id), (uint32 *)&ctr_reg);
    ctr_reg.u.field.tcMode = mode;
    ioal_soc_mem32_write(unit, tc_reg_CTL(unit, id), *((uint32 *)&ctr_reg));

    return RT_ERR_OK;
}

int32 tc_divFactor_set(uint32 unit, drv_tc_id_t id,uint32 divFactor)
{
    tc_ctlReg_t     ctr_reg;

    ioal_soc_mem32_read(unit, tc_reg_CTL(unit, id), (uint32 *)&ctr_reg);
    ctr_reg.u.field.tcDivFactor = divFactor;
    ioal_soc_mem32_write(unit, tc_reg_CTL(unit, id), *((uint32 *)&ctr_reg));


    return RT_ERR_OK;
}

int32 tc_dataInitValue_set(uint32 unit, drv_tc_id_t id, uint32 init_value)
{
    tc_dataReg_t    data_reg;

    /* set data register */
    ioal_soc_mem32_read(unit, tc_reg_DATA(unit, id), (uint32 *)&data_reg);
    data_reg.u.field.tc_data = init_value;
    ioal_soc_mem32_write(unit, tc_reg_DATA(unit, id), *((uint32 *)&data_reg));

    return RT_ERR_OK;
}

int32 tc_intEnable_set(uint32 unit, drv_tc_id_t id, rtk_enable_t enable)
{
    tc_intReg_t    int_reg;

    /* set data register */
    ioal_soc_mem32_read(unit, tc_reg_INT(unit, id), (uint32 *)&int_reg);
    if (ENABLED == enable)
        int_reg.u.field.intEnable = 1;
    else
        int_reg.u.field.intEnable = 0;
    ioal_soc_mem32_write(unit, tc_reg_INT(unit, id), *((uint32 *)&int_reg));

    return RT_ERR_OK;
}

int32 tc_intState_get(uint32 unit, drv_tc_id_t id, drv_tc_intState_t *state)
{
    tc_intReg_t    int_reg;

    /* set data register */
    ioal_soc_mem32_read(unit, tc_reg_INT(unit, id), (uint32 *)&int_reg);
    if(int_reg.u.field.intPending==1)
        *state=TC_INTERRUPT_PENDING;
    else
        *state=TC_INTERRUPT_PENDING_NO;

    return RT_ERR_OK;
}

int32 tc_intState_clear(uint32 unit, drv_tc_id_t id)
{
    tc_intReg_t    int_reg;

    /* set data register */
    ioal_soc_mem32_read(unit, tc_reg_INT(unit, id), (uint32 *)&int_reg);
    int_reg.u.field.intPending=1;
    ioal_soc_mem32_write(unit, tc_reg_INT(unit, id), *((uint32 *)&int_reg));

    return RT_ERR_OK;
}


int32 tc_counterValue_get(uint32 unit, drv_tc_id_t id, uint32 *value)
{
    ioal_soc_mem32_read(unit, tc_reg_CNT(unit, id), value);

    return RT_ERR_OK;
}


