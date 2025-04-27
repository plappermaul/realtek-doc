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
 * Purpose : Definition APIåof Timer/Counter of chip
 *
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/type.h>
#include <common/debug/rt_log.h>
#include <hwp/hw_profile.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <private/drv/tc/tc_mapper.h>
#include <drv/tc/tc.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      drv_tc_init
 * Description:
 *      Initialize TC module.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_init(uint32 unit)
{
    RT_INIT_MSG("  TC init (unit %u)\n", unit);
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!TC_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return TC_CTRL(unit).init(unit);
}

/* Function Name:
 *      drv_tc_enable_set
 * Description:
 *      Enable/disale Timer/Counter of a chip
 * Input:
 *      unit            - unit id
 *      id              - the timer/counter ID
 *      enable          - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_enable_set(uint32 unit,drv_tc_id_t id, rtk_enable_t enable)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!TC_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return TC_CTRL(unit).tc_enable_set(unit, id, enable);
}

/* Function Name:
 *      drv_tc_mode_set
 * Description:
 *      Set timer mode or counter mode
 * Input:
 *      unit                - unit id
 *      id                  - the timer/counter ID
 *      mode                - timer mode or counter mode
 *      init_value          - the timer/counter ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_mode_set(uint32 unit, drv_tc_id_t id, drv_tc_mode_t mode)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!TC_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return TC_CTRL(unit).tcMode_set(unit, id, mode);
}

/* Function Name:
 *      drv_tc_divFactor_set
 * Description:
 *      Set divide factor
 * Input:
 *      unit            - unit id
 *      id              - the timer/counter ID
 *      divFactor       - the divide factor, Base clock = System_clock / divide factor, Both values 0x0000 and 0x0001 disable the clock.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_divFactor_set(uint32 unit, drv_tc_id_t id, uint32 divFactor)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!TC_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return TC_CTRL(unit).tcDivFactor_set(unit, id, divFactor);
}

/* Function Name:
 *      drv_tc_dataInitValue_set
 * Description:
 *      Set the timer or counter initial value
 * Input:
 *      unit                - unit id
 *      id                  - the timer/counter ID
 *      init_value          - the timer or counter initial value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_dataInitValue_set(uint32 unit, drv_tc_id_t id, uint32 init_value)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!TC_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return TC_CTRL(unit).tcDataInitValue_set(unit, id, init_value);
}


/* Function Name:
 *      drv_tc_intEnable_set
 * Description:
 *      Enable/Disable interrupt of this TC
 * Input:
 *      unit                - unit id
 *      id                  - the timer/counter ID
 *      value               - the read value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_intEnable_set(uint32 unit, drv_tc_id_t id, rtk_enable_t enable)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!TC_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return TC_CTRL(unit).tcIntEnable_set(unit,id,enable);
}

/* Function Name:
 *      drv_tc_intState_get
 * Description:
 *      Get the interrupt state, (got interrupt or not)
 * Input:
 *      unit                - unit id
 *      id                  - the timer/counter ID
 *      state               - interrupt got or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_intState_get(uint32 unit, drv_tc_id_t id, drv_tc_intState_t *state)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!TC_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return TC_CTRL(unit).tcIntState_get(unit, id, state);
}

/* Function Name:
 *      drv_tc_intState_clear
 * Description:
 *      Clear the interrupt state
 * Input:
 *      unit                - unit id
 *      id                  - the timer/counter ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_intState_clear(uint32 unit, drv_tc_id_t id)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!TC_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return TC_CTRL(unit).tcIntState_clear(unit, id);
}

/* Function Name:
 *      drv_tc_counterValue_get
 * Description:
 *      Get the current tick value of the timer/counter
 * Input:
 *      unit            - unit id
 *      id              - the timer/counter ID
 *      value           - the got value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize tc module before calling any tc APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_tc_counterValue_get(uint32 unit, drv_tc_id_t id,uint32 *value)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(!TC_CHK(unit), RT_ERR_CHIP_NOT_FOUND);

    return TC_CTRL(unit).tcCounterValue_get(unit, id, value);
}




