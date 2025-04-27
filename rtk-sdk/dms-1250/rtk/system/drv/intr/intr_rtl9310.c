/* Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 93380 $
 * $Date: 2018-11-08 17:49:37 +0800 (Thu, 08 Nov 2018) $
 *
 * Purpose : Definition of Interrupt control API
 *
 * Feature : The file includes the following modules
 *           (1) Interrupt
 *
 */

 /*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <osal/isr.h>
#include <drv/intr/intr.h>
#include <private/drv/intr/intr_mapper.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <hwp/hw_profile.h>
#include <private/drv/swcore/swcore_rtl9310.h>
#include <private/drv/intr/intr_rtl9310.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
drv_isr_idRegBitMap_t rtl9310_isr_glb_reg_id_map [] = {
    { RTL9310_ISR_GLB_SRC_STS_ISR_GLB_LINK_CHG_MASK,   INTR_ISR_PORT_LINK_CHG },
    { RTL9310_ISR_GLB_SRC_STS_ISR_GLB_EXT_GPIO_MASK,   INTR_ISR_EXT_GPIO },
    { RTL9310_ISR_GLB_SRC_STS_ISR_GLB_OAM_DYGASP_MASK, INTR_ISR_OAM_DYGASP },
    { RTL9310_ISR_GLB_SRC_STS_ISR_GLB_CCM_MASK,        INTR_ISR_CCM },
};

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */




int32
rtl9310_intr_swcoreIsrSts_get(uint32 unit, uint32 *pisr_sts)
{
    uint32      reg_data = 0;
    int32       i, size;

    *pisr_sts = 0;
    ioal_mem32_read(unit, RTL9310_ISR_GLB_SRC_STS_ADDR, &reg_data);
    size = sizeof(rtl9310_isr_glb_reg_id_map)/sizeof(drv_isr_idRegBitMap_t);
    for (i = 0; i < size; i++)
    {
        if (reg_data & rtl9310_isr_glb_reg_id_map[i].reg_bit_mask)
        {
            *pisr_sts |=  INTR_ISR_BIT(rtl9310_isr_glb_reg_id_map[i].isr_id);
        }
    }

    return RT_ERR_OK;
}

int32
_rtl9310_intr_swcoreImrPortLinkEnable_set(uint32 unit, rtk_enable_t enable)
{
    if (enable == DISABLED)
    {
        ioal_mem32_write(unit, RTL9310_IMR_PORT_LINK_STS_CHG_ADDR(0), 0x0);
        ioal_mem32_write(unit, RTL9310_IMR_PORT_LINK_STS_CHG_ADDR(32), 0x0);
    }
    else
    {
        ioal_mem32_write(unit, RTL9310_IMR_PORT_LINK_STS_CHG_ADDR(0), 0xFFFFFFFF);
        ioal_mem32_write(unit, RTL9310_IMR_PORT_LINK_STS_CHG_ADDR(32), 0x00FFFFFF);
    }
    return RT_ERR_OK;
}

int32
_rtl9310_intr_swcoreImrExtGpioEnable_set(uint32 unit, rtk_enable_t enable)
{
    if (enable == DISABLED)
    {
        ioal_mem32_write(unit, RTL9310_IMR_EXT_GPIO_ADDR(0), 0x0);
        ioal_mem32_write(unit, RTL9310_IMR_EXT_GPIO_ADDR(32), 0x0);
    }
    else
    {
        ioal_mem32_write(unit, RTL9310_IMR_EXT_GPIO_ADDR(0), drv_intr_imrExtGpio0Data[unit]);
        ioal_mem32_write(unit, RTL9310_IMR_EXT_GPIO_ADDR(32), drv_intr_imrExtGpio1Data[unit]);
    }

    return RT_ERR_OK;
}

int32
rtl9310_intr_swcoreImrEnable_set(uint32 unit, drv_intr_isr_t isrId, rtk_enable_t enable)
{
    uint32      val;
    uint32      reg_data;

    if (isrId == INTR_ISR_OAM_DYGASP)
    {
        val = (enable == DISABLED) ? 0 : 1;
        ioal_mem32_read(unit, RTL9310_IMR_MISC_ADDR, &reg_data);
        reg_data = (reg_data & ~RTL9310_IMR_MISC_IMR_OAM_DYGASP_MASK) | ((val << RTL9310_IMR_MISC_IMR_OAM_DYGASP_OFFSET) & RTL9310_IMR_MISC_IMR_OAM_DYGASP_MASK);
        ioal_mem32_write(unit, RTL9310_IMR_MISC_ADDR, reg_data);
        return RT_ERR_OK;
    }
    else if (isrId == INTR_ISR_CCM)
    {
        val = (enable == DISABLED) ? 0x0 : 0xFFFF;
        ioal_mem32_read(unit, RTL9310_IMR_MISC_ADDR, &reg_data);
        reg_data = (reg_data & ~RTL9310_IMR_MISC_IMR_CCM_MASK) | ((val << RTL9310_IMR_MISC_IMR_CCM_OFFSET) & RTL9310_IMR_MISC_IMR_CCM_MASK);
        ioal_mem32_write(unit, RTL9310_IMR_MISC_ADDR, reg_data);
        return RT_ERR_OK;
    }
    else if (isrId == INTR_ISR_PORT_LINK_CHG)
    {
        return _rtl9310_intr_swcoreImrPortLinkEnable_set(unit, enable);
    }
    else if (isrId == INTR_ISR_EXT_GPIO)
    {
        return _rtl9310_intr_swcoreImrExtGpioEnable_set(unit, enable);
    }
    else
    {
        return RT_ERR_FAILED;
    }
}

int32
rtl9310_intr_swcoreSts_get(uint32 unit, drv_intr_isr_t isrId, drv_intr_data_t *pFunc_sts)
{
    uint32      reg_data = 0;

    osal_memset(pFunc_sts, 0, sizeof(drv_intr_data_t));
    if (isrId == INTR_ISR_OAM_DYGASP)
    {
        ioal_mem32_read(unit, RTL9310_ISR_MISC_ADDR, &reg_data);
        ioal_mem32_write(unit, RTL9310_ISR_MISC_ADDR, RTL9310_ISR_MISC_ISR_OAM_DYGASP_MASK);
        pFunc_sts->u.oam_dygsp = (reg_data & RTL9310_ISR_MISC_ISR_OAM_DYGASP_MASK) ? 1 : 0;
        return RT_ERR_OK;
    }
    else if (isrId == INTR_ISR_CCM)
    {
        ioal_mem32_read(unit, RTL9310_ISR_MISC_ADDR, &reg_data);
        ioal_mem32_write(unit, RTL9310_ISR_MISC_ADDR, RTL9310_ISR_MISC_ISR_CCM_MASK);
        pFunc_sts->u.ccm = (reg_data & RTL9310_ISR_MISC_ISR_CCM_MASK) >> RTL9310_ISR_MISC_ISR_CCM_OFFSET;
        return RT_ERR_OK;
    }
    else if (isrId == INTR_ISR_PORT_LINK_CHG)
    {
        ioal_mem32_read(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ISR_PORT_LINK_STS_CHG_OFFSET(0), &reg_data);
        RTK_PORTMASK_WORD_SET(pFunc_sts->u.portmask, 0, reg_data);
        ioal_mem32_write(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ISR_PORT_LINK_STS_CHG_OFFSET(0), reg_data);

        ioal_mem32_read(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ISR_PORT_LINK_STS_CHG_OFFSET(32), &reg_data);
        RTK_PORTMASK_WORD_SET(pFunc_sts->u.portmask, 1, reg_data);
        ioal_mem32_write(unit, RTL9310_ISR_PORT_LINK_STS_CHG_ISR_PORT_LINK_STS_CHG_OFFSET(32), reg_data);
        return RT_ERR_OK;
    }
    else if (isrId == INTR_ISR_EXT_GPIO)
    {
        ioal_mem32_read(unit, RTL9310_ISR_EXT_GPIO_ADDR(0), &reg_data);
        pFunc_sts->u.ext_gpio[0] = reg_data;
        ioal_mem32_write(unit, RTL9310_ISR_EXT_GPIO_ADDR(0), reg_data);

        ioal_mem32_read(unit, RTL9310_ISR_EXT_GPIO_ADDR(32), &reg_data);
        pFunc_sts->u.ext_gpio[1] = reg_data;
        ioal_mem32_write(unit, RTL9310_ISR_EXT_GPIO_ADDR(32), reg_data);
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_FAILED;
    }
}



