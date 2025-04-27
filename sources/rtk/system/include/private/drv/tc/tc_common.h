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
 * Purpose : DRV APIs definition.
 *
 * Feature : I2C relative API
 *
 */
#ifndef __TC_COMMON_H__
#define __TC_COMMON_H__

/*
* Include Files
*/
#include <drv/tc/tc.h>
#include <soc/socdef/rtl8380_soc_reg.h>
#include <soc/socdef/rtl8390_soc_reg.h>
#include <soc/socdef/rtl9300_soc_reg.h>
#include <soc/socdef/rtl9310_soc_reg.h>

/*
* Symbol Definition
*/
#define tc_reg_DATA(unit, id) (TC_REG(unit).tmr_cntr_data_addr[id].reg_addr)
#define tc_reg_CNT(unit, id)  (TC_REG(unit).tmr_cntr_cntr_addr[id].reg_addr)
#define tc_reg_CTL(unit, id)  (TC_REG(unit).tmr_cntr_ctrl_addr[id].reg_addr)
#define tc_reg_INT(unit, id)  (TC_REG(unit).tmr_cntr_intr_addr[id].reg_addr)

/*
 * Data Declaration
 */
typedef struct tmr_cntr_addr_s
{
    uint32 reg_addr;
} tmr_cntr_addr_t;

typedef struct tc_reg_filed_s{
    uint32 mask;
    uint32 offset;
} tc_reg_filed_t;

typedef struct tc_reg_definition_s{
    tmr_cntr_addr_t tmr_cntr_data_addr[TC_ID_MAX];
    tmr_cntr_addr_t tmr_cntr_cntr_addr[TC_ID_MAX];
    tmr_cntr_addr_t tmr_cntr_ctrl_addr[TC_ID_MAX];
    tmr_cntr_addr_t tmr_cntr_intr_addr[TC_ID_MAX];
} tc_reg_definition_t;


/*
 * Macro Declaration
 */
extern tc_reg_definition_t tc_reg[];
#define TC_REG(unit)       tc_reg[tc_if[unit]]

/*
 * Function Declaration
 */
int32 tc_init(uint32 unit);
int32 tc_enable_set(uint32 unit, drv_tc_id_t id, rtk_enable_t enable);
int32 tc_mode_set(uint32 unit, drv_tc_id_t id, drv_tc_mode_t mode);
int32 tc_divFactor_set(uint32 unit, drv_tc_id_t id,uint32 divFactor);
int32 tc_dataInitValue_set(uint32 unit, drv_tc_id_t id, uint32 init_value);
int32 tc_intEnable_set(uint32 unit, drv_tc_id_t id, rtk_enable_t enable);
int32 tc_intState_get(uint32 unit, drv_tc_id_t id, drv_tc_intState_t *state);
int32 tc_intState_clear(uint32 unit, drv_tc_id_t id);
int32 tc_counterValue_get(uint32 unit, drv_tc_id_t id,uint32 *value);

#endif /* __TC_COMMON_H__ */

