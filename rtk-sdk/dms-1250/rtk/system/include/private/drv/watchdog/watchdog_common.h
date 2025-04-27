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
 * Purpose : Definition those public watchdog APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) mode set & get
 *            2) scale set & get
 */
#ifndef __WATCHDOG_GEN1_H__
#define __WATCHDOG_GEN1_H__

/*
 * Include Files
 */
#include <drv/watchdog/watchdog.h>
#include <soc/socdef/rtl8380_soc_reg.h>
#include <soc/socdef/rtl8390_soc_reg.h>
#include <soc/socdef/rtl9300_soc_reg.h>
#include <soc/socdef/rtl9310_soc_reg.h>
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
#define GEN1_WDOG_CTRL_SCALE_1(unit)                (0 << WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_clk_sc.offset)
#define GEN1_WDOG_CTRL_SCALE_2(unit)                (1 << WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_clk_sc.offset)
#define GEN1_WDOG_CTRL_SCALE_3(unit)                (2 << WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_clk_sc.offset)
#define GEN1_WDOG_CTRL_SCALE_4(unit)                (3 << WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_clk_sc.offset)
#define GEN1_WDOG_CTRL_RESET_MODE_FULL_CHIP(unit)   (0 << WDG_REG(unit).wdog_ctrlr.wdtCtrlr_reset_mode.offset)
#define GEN1_WDOG_CTRL_RESET_MODE_CPU_IPSEC(unit)   (1 << WDG_REG(unit).wdog_ctrlr.wdtCtrlr_reset_mode.offset)
#define GEN1_WDOG_CTRL_RESET_MODE_SOFTWARE(unit)    (2 << WDG_REG(unit).wdog_ctrlr.wdtCtrlr_reset_mode.offset)


/*
 * Data Type Definition
 */
typedef struct wdg_reg_filed_s
{
    uint32 mask;
    uint32 offset;
} wdg_reg_filed_t;

typedef struct wdog_ctrlr_s
{
    uint32 reg_addr;
    wdg_reg_filed_t wdtCtrlr_wdt_clk_sc;
    wdg_reg_filed_t wdtCtrlr_wdt_e;
    wdg_reg_filed_t wdtCtrlr_ph1_to;
    wdg_reg_filed_t wdtCtrlr_ph2_to;
    wdg_reg_filed_t wdtCtrlr_reset_mode;
} wdog_ctrlr_t;

typedef struct glbl_intr_msk_s
{
    uint32 reg_addr;
    wdg_reg_filed_t glbl_intr_msk_wdt_ph1_ie;
} glbl_intr_msk_t;

typedef struct wdtcntrr_addr_s
{
    uint32 reg_addr;
    wdg_reg_filed_t wdog_cntr_wdt_kick;
} wdtcntrr_addr_t;

typedef struct wdg_reg_definition_s
{
    wdog_ctrlr_t wdog_ctrlr;
    glbl_intr_msk_t glbl_intr_msk;
    wdtcntrr_addr_t wdtcntrr_addr;
} wdg_reg_definition_t;
/*
 * Data Declaration
 */

/*
 * Macro Definition
 */
extern wdg_reg_definition_t wdg_reg[];
#define WDG_REG(unit)       wdg_reg[wdg_if[unit]]

/*
 * Function Declaration
 */

/* Function Name:
 *      watchdog_init
 * Description:
 *      Init the watchdog module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
watchdog_init(uint32 unit);

/* Function Name:
 *      watchdog_scale_set
 * Description:
 *      Set watchdog expired period
 * Input:
 *      unit  - unit id
 *      scale - period scale
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - succeed in setting watchdog expired period.
 *      RT_ERR_FAILED - fail to set watchdog expired period.
 * Note:
 *      None
 */
extern int32
watchdog_scale_set(uint32 unit, drv_watchdog_scale_t scale);

/* Function Name:
 *      watchdog_scale_get
 * Description:
 *      Get watchdog expired period scale
 * Input:
 *      unit   - unit id
 * Output:
 *      pScale - period scale
 * Return:
 *      RT_ERR_OK           - get watchdog expired period scale successfully.
 *      RT_ERR_FAILED       - fail to get get watchdog expired period scale.
 *      RT_ERR_NULL_POINTER - pScale is a null pointer.
 * Note:
 *      None
 */
extern int32
watchdog_scale_get(uint32 unit, drv_watchdog_scale_t *pScale);

/* Function Name:
 *      watchdog_enable_set
 * Description:
 *      Set watchdog enable/disable
 * Input:
 *      unit   - unit id
 *      enable - enable or disable request
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - enable watchdog successfully.
 *      RT_ERR_FAILED - fail to enable watchdog.
 * Note:
 *      None
 */
extern int32
watchdog_enable_set(uint32 unit, uint32 enable);

/* Function Name:
 *      watchdog_enable_get
 * Description:
 *      Get watchdog enable/disable status
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - watchdog enable/disable status
 * Return:
 *      RT_ERR_OK           - get watchdog enable/disable status successfully.
 *      RT_ERR_NULL_POINTER - pEnable is a null pointer.
 * Note:
 *      None
 */
extern int32
watchdog_enable_get(uint32 unit, uint32 *pEnable);

/* Function Name:
 *      watchdog_kick
 * Description:
 *      Kick watchdog
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - kick watchdog successfully.
 * Note:
 *      None
 */
extern int32
watchdog_kick(uint32 unit);

/* Function Name:
 *      watchdog_threshold_set
 * Description:
 *      Set watchdog threshold counter of the specified device
 * Input:
 *      unit       - unit id
 *      pThreshold - watchdog threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - successfully.
 *      RT_ERR_NULL_POINTER - pThreshold is a null pointer.
 *      RT_ERR_INPUT        - invalid input argument
 * Note:
 *      None
 */
extern int32
watchdog_threshold_set(uint32 unit, drv_watchdog_threshold_t *pThreshold);

/* Function Name:
 *      watchdog_threshold_get
 * Description:
 *      Get watchdog threshold counter of the specified device
 * Input:
 *      unit       - unit id
 * Output:
 *      pThreshold - watchdog threshold
 * Return:
 *      RT_ERR_OK - successfully.
 *      RT_ERR_NULL_POINTER - pThreshold is a null pointer.
 * Note:
 *      None
 */
extern int32
watchdog_threshold_get(uint32 unit, drv_watchdog_threshold_t *pThreshold);

#endif /* __WATCHDOG_GEN1_H__ */
