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

/*
 * Include Files
 */
#include <soc/type.h>
#include <ioal/mem32.h>
#include <private/drv/watchdog/watchdog_common.h>
#include <private/drv/watchdog/watchdog_mapper.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <osal/print.h>
#include <hwp/hw_profile.h>

/*
 * Symbol Definition
 */

/*
 * Data Type Definition
 */

/*
 * Data Declaration
 */
extern uint32 wdg_chipId[];
static uint8  watchdog_init_status[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};

wdg_reg_definition_t wdg_reg[WDG_CTRL_END] =
{
#if defined(CONFIG_SDK_RTL9310)
    {
        .wdog_ctrlr =
        {
            .reg_addr = RTL9310_WDT_CTRL_ADDR,
            .wdtCtrlr_wdt_clk_sc.mask = RTL9310_WDT_CTRL_WDT_CLK_SC_MASK,
            .wdtCtrlr_wdt_clk_sc.offset = RTL9310_WDT_CTRL_WDT_CLK_SC_OFFSET,
            .wdtCtrlr_wdt_e.mask = RTL9310_WDT_CTRL_WDT_E_MASK,
            .wdtCtrlr_wdt_e.offset = RTL9310_WDT_CTRL_WDT_E_OFFSET,
            .wdtCtrlr_ph1_to.mask = RTL9310_WDT_CTRL_PH1_TO_MASK,
            .wdtCtrlr_ph1_to.offset = RTL9310_WDT_CTRL_PH1_TO_OFFSET,
            .wdtCtrlr_ph2_to.mask = RTL9310_WDT_CTRL_PH2_TO_MASK,
            .wdtCtrlr_ph2_to.offset = RTL9310_WDT_CTRL_PH2_TO_OFFSET,
            .wdtCtrlr_reset_mode.mask = RTL9310_WDT_CTRL_WDT_RESET_MODE_MASK,
            .wdtCtrlr_reset_mode.offset = RTL9310_WDT_CTRL_WDT_RESET_MODE_OFFSET,
        },
        .wdtcntrr_addr =
        {
            .reg_addr = RTL9310_WDTCNTRR_ADDR,
            .wdog_cntr_wdt_kick.mask = RTL9310_WDTCNTRR_WDT_KICK_MASK,
            .wdog_cntr_wdt_kick.offset = RTL9310_WDTCNTRR_WDT_KICK_OFFSET,
        },

    },
#endif
#if defined(CONFIG_SDK_RTL9300)
    {
        .wdog_ctrlr =
        {
            .reg_addr = RTL9300_WDT_CTRL_ADDR,
            .wdtCtrlr_wdt_clk_sc.mask = RTL9300_WDT_CTRL_WDT_CLK_SC_MASK,
            .wdtCtrlr_wdt_clk_sc.offset = RTL9300_WDT_CTRL_WDT_CLK_SC_OFFSET,
            .wdtCtrlr_wdt_e.mask = RTL9300_WDT_CTRL_WDT_E_MASK,
            .wdtCtrlr_wdt_e.offset = RTL9300_WDT_CTRL_WDT_E_OFFSET,
            .wdtCtrlr_ph1_to.mask = RTL9300_WDT_CTRL_PH1_TO_MASK,
            .wdtCtrlr_ph1_to.offset = RTL9300_WDT_CTRL_PH1_TO_OFFSET,
            .wdtCtrlr_ph2_to.mask = RTL9300_WDT_CTRL_PH2_TO_MASK,
            .wdtCtrlr_ph2_to.offset = RTL9300_WDT_CTRL_PH2_TO_OFFSET,
            .wdtCtrlr_reset_mode.mask = RTL9300_WDT_CTRL_WDT_RESET_MODE_MASK,
            .wdtCtrlr_reset_mode.offset = RTL9300_WDT_CTRL_WDT_RESET_MODE_OFFSET,
        },
        .glbl_intr_msk =
        {
            .reg_addr = RTL9300_VPE0_GIMR_ADDR,
            .glbl_intr_msk_wdt_ph1_ie.mask = RTL9300_VPE0_GIMR_WDT_PH1TO_IE_MASK,
            .glbl_intr_msk_wdt_ph1_ie.offset = RTL9300_VPE0_GIMR_WDT_PH1TO_IE_OFFSET,
        },
        .wdtcntrr_addr =
        {
            .reg_addr = RTL9300_WDTCNTRR_ADDR,
            .wdog_cntr_wdt_kick.mask = RTL9300_WDTCNTRR_WDT_KICK_MASK,
            .wdog_cntr_wdt_kick.offset = RTL9300_WDTCNTRR_WDT_KICK_OFFSET,
        },

    },
#endif
#if defined(CONFIG_SDK_RTL8390)
    {
        .wdog_ctrlr =
        {
            .reg_addr = RTL8390_WDTCTRLR_ADDR,
            .wdtCtrlr_wdt_clk_sc.mask = RTL8390_WDTCTRLR_WDT_CLK_SC_MASK,
            .wdtCtrlr_wdt_clk_sc.offset = RTL8390_WDTCTRLR_WDT_CLK_SC_OFFSET,
            .wdtCtrlr_wdt_e.mask = RTL8390_WDTCTRLR_WDT_E_MASK,
            .wdtCtrlr_wdt_e.offset = RTL8390_WDTCTRLR_WDT_E_OFFSET,
            .wdtCtrlr_ph1_to.mask = RTL8390_WDTCTRLR_PH1_TO_MASK,
            .wdtCtrlr_ph1_to.offset = RTL8390_WDTCTRLR_PH1_TO_OFFSET,
            .wdtCtrlr_ph2_to.mask = RTL8390_WDTCTRLR_PH2_TO_MASK,
            .wdtCtrlr_ph2_to.offset = RTL8390_WDTCTRLR_PH2_TO_OFFSET,
            .wdtCtrlr_reset_mode.mask = RTL8390_WDTCTRLR_WDT_RESET_MODE_MASK,
            .wdtCtrlr_reset_mode.offset = RTL8390_WDTCTRLR_WDT_RESET_MODE_OFFSET,
        },
        .glbl_intr_msk =
        {
            .reg_addr = RTL8390_VPE0_GIMR_ADDR,
            .glbl_intr_msk_wdt_ph1_ie.mask = RTL8390_VPE0_GIMR_WDT_IE_MASK,
            .glbl_intr_msk_wdt_ph1_ie.offset = RTL8390_VPE0_GIMR_WDT_IE_OFFSET,
        },
        .wdtcntrr_addr =
        {
            .reg_addr = RTL8390_WDTCNTRR_ADDR,
            .wdog_cntr_wdt_kick.mask = RTL8390_WDTCNTRR_WDT_KICK_MASK,
            .wdog_cntr_wdt_kick.offset = RTL8390_WDTCNTRR_WDT_KICK_OFFSET,
        },
    },
#endif
#if defined(CONFIG_SDK_RTL8380)
    {
        .wdog_ctrlr =
        {
            .reg_addr = RTL8380_WDOG_CTRL_ADDR,
            .wdtCtrlr_wdt_clk_sc.mask = RTL8380_WDOG_CTRL_WDT_CLK_SC_MASK,
            .wdtCtrlr_wdt_clk_sc.offset = RTL8380_WDOG_CTRL_WDT_CLK_SC_OFFSET,
            .wdtCtrlr_wdt_e.mask = RTL8380_WDOG_CTRL_WDT_E_MASK,
            .wdtCtrlr_wdt_e.offset = RTL8380_WDOG_CTRL_WDT_E_OFFSET,
            .wdtCtrlr_ph1_to.mask = RTL8380_WDOG_CTRL_PH1_TO_MASK,
            .wdtCtrlr_ph1_to.offset = RTL8380_WDOG_CTRL_PH1_TO_OFFSET,
            .wdtCtrlr_ph2_to.mask = RTL8380_WDOG_CTRL_PH2_TO_MASK,
            .wdtCtrlr_ph2_to.offset = RTL8380_WDOG_CTRL_PH2_TO_OFFSET,
            .wdtCtrlr_reset_mode.mask = RTL8380_WDOG_CTRL_WDT_RESET_MODE_MASK,
            .wdtCtrlr_reset_mode.offset = RTL8380_WDOG_CTRL_WDT_RESET_MODE_OFFSET,
        },
        .glbl_intr_msk =
        {
            .reg_addr = RTL8380_GLBL_INTR_MSK_ADDR,
            .glbl_intr_msk_wdt_ph1_ie.mask = RTL8380_GLBL_INTR_MSK_WDT_PH1TO_IE_MASK,
            .glbl_intr_msk_wdt_ph1_ie.offset = RTL8380_GLBL_INTR_MSK_WDT_PH1TO_IE_OFFSET,
        },
        .wdtcntrr_addr =
        {
            .reg_addr = RTL8380_WDOG_CNTR_ADDR,
            .wdog_cntr_wdt_kick.mask = RTL8390_WDTCNTRR_WDT_KICK_MASK,
            .wdog_cntr_wdt_kick.offset = RTL8380_WDOG_CNTR_WDT_KICK_OFFSET,
        },
    },
#endif
};

/*
 * Macro Definition
 */

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
int32
watchdog_init(uint32 unit)
{
    int32   ret;
    drv_watchdog_threshold_t threshold;

    RT_INIT_REENTRY_CHK(watchdog_init_status[unit]);

    threshold.phase_1_threshold = 10;
    threshold.phase_2_threshold = 0;
    if ((ret = watchdog_threshold_set(unit, &threshold)) != RT_ERR_OK)
        return ret;
    watchdog_init_status[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of watchdog_init */

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
int32
watchdog_scale_set(uint32 unit, drv_watchdog_scale_t scale)
{
    uint32 newWDTCTRL = 0;
    uint32 val = 0;

    /* set WDT_CLK_SC field to be 2b 00 */
    ioal_soc_mem32_read(unit, WDG_REG(unit).wdog_ctrlr.reg_addr, &val);
    newWDTCTRL = val & (~(WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_clk_sc.mask));
    switch (scale)
    {
        case WATCHDOG_SCALE_1:                     /* fire after 1.7 sec */
            newWDTCTRL = (newWDTCTRL | GEN1_WDOG_CTRL_SCALE_1(unit)); /* 2b 00, 2^25 */
            break;

        case WATCHDOG_SCALE_2:                     /* fire after 3.4 sec */
            newWDTCTRL = (newWDTCTRL | GEN1_WDOG_CTRL_SCALE_2(unit)); /* 2b 01, 2^26 */
            break;

        case WATCHDOG_SCALE_3:                     /* fire after 6.7 sec */
            newWDTCTRL = (newWDTCTRL | GEN1_WDOG_CTRL_SCALE_3(unit)); /* 2b 10, 2^27 */
            break;

        case WATCHDOG_SCALE_4:                     /* fire after 13.4 sec */
            newWDTCTRL = (newWDTCTRL | GEN1_WDOG_CTRL_SCALE_4(unit)); /* 2b 11, 2^28 */
            break;

        default:
            return RT_ERR_FAILED;
    }
    ioal_soc_mem32_write(unit, WDG_REG(unit).wdog_ctrlr.reg_addr, newWDTCTRL);

    return RT_ERR_OK;
} /* end of watchdog_scale_set */

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
int32
watchdog_scale_get(uint32 unit, drv_watchdog_scale_t *pScale)
{
    uint32  val;

    /* parameter check */
    RT_PARAM_CHK((NULL == pScale), RT_ERR_NULL_POINTER);

    /* Get WDT_CLK_SC field */
    ioal_soc_mem32_read(unit, WDG_REG(unit).wdog_ctrlr.reg_addr, &val);
    *pScale = (val & WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_clk_sc.mask) >> WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_clk_sc.offset;

    return RT_ERR_OK;
} /* end of watchdog_scale_get */

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
int32
watchdog_enable_set(uint32 unit, uint32 enable)
{
    uint32  v_wdtcnr = 0;
    uint32  val = 0;

    /* Get old watchdog controller value */
    ioal_soc_mem32_read(unit, WDG_REG(unit).wdog_ctrlr.reg_addr, &v_wdtcnr);

    /* Clear watch dog enable/disable field. */
    v_wdtcnr = v_wdtcnr & (~(WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_e.mask));

    /* Set Reset Mode to Whole chip */
    v_wdtcnr = v_wdtcnr & (~(WDG_REG(unit).wdog_ctrlr.wdtCtrlr_reset_mode.mask));
    v_wdtcnr = v_wdtcnr | (GEN1_WDOG_CTRL_RESET_MODE_FULL_CHIP(unit));

    ioal_soc_mem32_write(unit, WDG_REG(unit).wdog_ctrlr.reg_addr, v_wdtcnr);

    /* Set watchdog enable/disable */
    switch (enable)
    {
        case DISABLED:          /* Disable */
            ioal_soc_mem32_write(unit, WDG_REG(unit).wdtcntrr_addr.reg_addr, v_wdtcnr);
            if (!HWP_9310_FAMILY_ID(unit))
            {
                ioal_soc_mem32_read(unit, WDG_REG(unit).glbl_intr_msk.reg_addr, &val);
                val = val & ~(WDG_REG(unit).glbl_intr_msk.glbl_intr_msk_wdt_ph1_ie.mask);
                ioal_soc_mem32_write(unit, WDG_REG(unit).glbl_intr_msk.reg_addr, val);
            }
            break;
        case ENABLED:           /* Enable */
            if (!HWP_9310_FAMILY_ID(unit))
            {
                ioal_soc_mem32_read(unit, WDG_REG(unit).glbl_intr_msk.reg_addr, &val);
                val = val | (WDG_REG(unit).glbl_intr_msk.glbl_intr_msk_wdt_ph1_ie.mask);
                ioal_soc_mem32_write(unit, WDG_REG(unit).glbl_intr_msk.reg_addr, val);
            }
            ioal_soc_mem32_write(unit, WDG_REG(unit).wdog_ctrlr.reg_addr, (v_wdtcnr | (1 << WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_e.offset)));
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of watchdog_enable_set */

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
int32
watchdog_enable_get(uint32 unit, uint32 *pEnable)
{
    uint32  v_wdtcnr = 0;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* Get watchdog controller value */
    ioal_soc_mem32_read(unit, WDG_REG(unit).wdog_ctrlr.reg_addr, &v_wdtcnr);
    (*pEnable) = (v_wdtcnr & (WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_e.mask)) >> WDG_REG(unit).wdog_ctrlr.wdtCtrlr_wdt_e.offset;

    return RT_ERR_OK;
} /* end of watchdog_enable_get */

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
int32
watchdog_kick(uint32 unit)
{
    uint32  val;

    /* Clear watchdog pending flag */
    ioal_soc_mem32_read(unit, WDG_REG(unit).wdtcntrr_addr.reg_addr, &val);
    val |= WDG_REG(unit).wdtcntrr_addr.wdog_cntr_wdt_kick.mask;
    ioal_soc_mem32_write(unit, WDG_REG(unit).wdtcntrr_addr.reg_addr, val);

    return RT_ERR_OK;
} /* end of watchdog_kick */

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
int32
watchdog_threshold_set(uint32 unit, drv_watchdog_threshold_t *pThreshold)
{
    uint32  v_wdtcnr = 0;

    /* parameter check */
    RT_PARAM_CHK((NULL == pThreshold), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pThreshold->phase_1_threshold > 31), RT_ERR_INPUT);
    RT_PARAM_CHK((pThreshold->phase_2_threshold > 31), RT_ERR_INPUT);

    /* Get old watchdog controller value */
    ioal_soc_mem32_read(unit, WDG_REG(unit).wdog_ctrlr.reg_addr, &v_wdtcnr);

    /* Set the PH1_TO & PH2_TO field. */
    v_wdtcnr = v_wdtcnr & (~(WDG_REG(unit).wdog_ctrlr.wdtCtrlr_ph1_to.mask));
    v_wdtcnr = v_wdtcnr | (pThreshold->phase_1_threshold << WDG_REG(unit).wdog_ctrlr.wdtCtrlr_ph1_to.offset);
    v_wdtcnr = v_wdtcnr & (~(WDG_REG(unit).wdog_ctrlr.wdtCtrlr_ph2_to.mask));
    v_wdtcnr = v_wdtcnr | (pThreshold->phase_2_threshold << WDG_REG(unit).wdog_ctrlr.wdtCtrlr_ph2_to.offset);

    ioal_soc_mem32_write(unit, WDG_REG(unit).wdog_ctrlr.reg_addr, v_wdtcnr);

    return RT_ERR_OK;
} /* end of watchdog_threshold_set */

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
int32
watchdog_threshold_get(uint32 unit, drv_watchdog_threshold_t *pThreshold)
{
    uint32  v_wdtcnr = 0;

    /* parameter check */
    RT_PARAM_CHK((NULL == pThreshold), RT_ERR_NULL_POINTER);

    /* Get old watchdog controller value */
    ioal_soc_mem32_read(unit, WDG_REG(unit).wdog_ctrlr.reg_addr, &v_wdtcnr);
    pThreshold->phase_1_threshold = (v_wdtcnr & WDG_REG(unit).wdog_ctrlr.wdtCtrlr_ph1_to.mask) >> WDG_REG(unit).wdog_ctrlr.wdtCtrlr_ph1_to.offset;
    pThreshold->phase_2_threshold = (v_wdtcnr & WDG_REG(unit).wdog_ctrlr.wdtCtrlr_ph2_to.mask) >> WDG_REG(unit).wdog_ctrlr.wdtCtrlr_ph2_to.offset;

    return RT_ERR_OK;
} /* end of watchdog_threshold_get */

