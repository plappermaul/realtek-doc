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
#include <private/drv/swcore/swcore_rtl8380.h>
#include <private/drv/intr/intr_rtl8380.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
drv_isr_idRegBitMap_t rtl8380_isr_glb_reg_id_map [] = {
    { RTL8380_ISR_GLB_SRC_ISR_GLB_LINK_CHG_MASK,  INTR_ISR_PORT_LINK_CHG },
};


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */




int32
rtl8380_intr_swcoreIsrSts_get(uint32 unit, uint32 *pisr_sts)
{
    uint32      reg_data = 0;
    int32       i, size;

    *pisr_sts = 0;
    ioal_mem32_read(unit, RTL8380_ISR_GLB_SRC_ADDR, &reg_data);
    size = sizeof(rtl8380_isr_glb_reg_id_map)/sizeof(drv_isr_idRegBitMap_t);
    for (i = 0; i < size; i++)
    {
        if (reg_data & rtl8380_isr_glb_reg_id_map[i].reg_bit_mask)
        {
            *pisr_sts |=  INTR_ISR_BIT(rtl8380_isr_glb_reg_id_map[i].isr_id);
        }
    }
    return RT_ERR_OK;
}

int32
_rtl8380_intr_swcoreImrPortLinkEnable_set(uint32 unit, rtk_enable_t enable)
{
    if (enable == DISABLED)
    {
        ioal_mem32_write(unit, RTL8380_IMR_PORT_LINK_STS_CHG_ADDR, 0x0);
    }
    else
    {
        ioal_mem32_write(unit, RTL8380_IMR_PORT_LINK_STS_CHG_ADDR, 0x0FFFFFFF);
    }

    return RT_ERR_OK;
}

int32
rtl8380_intr_swcoreImrEnable_set(uint32 unit, drv_intr_isr_t isrId, rtk_enable_t enable)
{
    if (isrId == INTR_ISR_PORT_LINK_CHG)
    {
        return _rtl8380_intr_swcoreImrPortLinkEnable_set(unit, enable);
    }
    else
    {
        return RT_ERR_FAILED;
    }
}

int32
rtl8380_intr_swcoreSts_get(uint32 unit, drv_intr_isr_t isrId, drv_intr_data_t *pFunc_sts)
{
    uint32      reg_data = 0;

    osal_memset(pFunc_sts, 0, sizeof(drv_intr_data_t));
    if (isrId == INTR_ISR_PORT_LINK_CHG)
    {
        ioal_mem32_read(unit, RTL8380_ISR_PORT_LINK_STS_CHG_ADDR, &reg_data);
        RTK_PORTMASK_WORD_SET(pFunc_sts->u.portmask, 0, reg_data);
        ioal_mem32_write(unit, RTL8380_ISR_PORT_LINK_STS_CHG_ADDR, reg_data);
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_FAILED;
    }
}



