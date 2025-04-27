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
 * $Revision: 83734 $
 * $Date: 2017-11-24 18:52:53 +0800 (Fri, 24 Nov 2017) $
 *
 * Purpose : Definition of Interrupt control API
 *
 * Feature : The file includes the following modules
 *           (1) Interrupt 
 *
 */
#ifndef __DRV_INTR_H__
#define __DRV_INTR_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/type.h>
#include <common/rt_type.h>
#include <common/util/rt_util.h>


/*
 * Symbol Definition
 */
/* for backward compatible */
#define drv_intr_swcoreIsrId_t                  drv_intr_isr_t
#define INTR_SWCORE_ISR_PORT_LINK_CHG           INTR_ISR_PORT_LINK_CHG
#define INTR_SWCORE_ISR_EXT_GPIO                INTR_ISR_EXT_GPIO
#define INTR_SWCORE_ISR_OAM_DYGASP              INTR_ISR_OAM_DYGASP
#define INTR_SWCORE_ISR_CCM                     INTR_ISR_CCM
#define drv_intr_swcoreImrEnable_set            drv_intr_imrEnable_set
#define drv_intr_swcoreIsrSts_get               drv_intr_isrSts_get

/* external gpio register number */
#define INTR_EXT_GPIO_REG_NUM       2   /* EXT_GPIO0 and EXT_GPIO1 */

typedef enum drv_intr_isr_s
{
    INTR_ISR_PORT_LINK_CHG = 0,
    INTR_ISR_EXT_GPIO,
    INTR_ISR_OAM_DYGASP,
    INTR_ISR_CCM,
    INTR_ISR_GPIO,
    INTR_ISR_END,
} drv_intr_isr_t;


typedef struct drv_isr_idRegBitMap_s
{
    uint32  reg_bit_mask;
    uint32  isr_id;
} drv_isr_idRegBitMap_t;


typedef struct drv_isr_data_s {
    union {
        uint32          oam_dygsp;  /* 1: interrupt pending; 0: normal. */
        uint32          ccm;        /* CCM bits */
        rtk_portmask_t  portmask;
        uint32          ext_gpio[INTR_EXT_GPIO_REG_NUM];
    } u;
} drv_intr_data_t;


/*
 * Data Declaration
 */
extern uint32      drv_intr_imrExtGpio0Data[];
extern uint32      drv_intr_imrExtGpio1Data[];

/*
 * Macro Declaration
 */
#define INTR_ISR_BIT(isr_id)                 (uint32) (0x1 << (isr_id))
#define INTR_ISR_CHK(_sts, isr_id)           (_sts & INTR_ISR_BIT(isr_id))


/*
 * Function Declaration
 */

/* Function Name:
 *      drv_intr_isrSts_get
 * Description:
 *      Get swcore Global ISR status
 * Input:
 *      unit - unit
 * Output:
 *      pisr_sts - ISR status bitmap. The ID is defined in drv_intr_isr_t.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
drv_intr_isrSts_get(uint32 unit, uint32 *pisr_sts);

/* Function Name:
 *      drv_intr_swcoreImrExtGpioPinEnable_set
 * Description:
 *      SWCORE externel GPIO pin enable set
 * Input:
 *      unit - unit
 *      pin - externel GPIO pin number
 *      enable - enable or disable IMR
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
drv_intr_swcoreImrExtGpioPinEnable_set(uint32 unit, uint32 pin, rtk_enable_t enable);


/* Function Name:
 *      drv_intr_imrEnable_set
 * Description:
 *      Enable/Disable a ISR
 * Input:
 *      unit - unit
 *      isrId - ISR function ID
 *      enable - enable or disable IMR
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
extern int32
drv_intr_imrEnable_set(uint32 unit, drv_intr_isr_t isrId, rtk_enable_t enable);


/* Function Name:
 *      drv_intr_swcoreSts_get
 * Description:
 *      Get ISR status by ISR function ID
 * Input:
 *      unit - unit
 *      isrId - ISR function ID
 * Output:
 *      pFunc_sts - ISR status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
extern int32
drv_intr_swcoreSts_get(uint32 unit, drv_intr_isr_t isrId, drv_intr_data_t *pFunc_sts);

/* Function Name:
 *      intr_probe
 * Description:
 *      Probe INTR module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
intr_probe(uint32 unit);


#endif /* __DRV_INTR_H__ */



