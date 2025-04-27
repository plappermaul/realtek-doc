/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 6417 $
 * $Date: 2009-10-15 09:38:58 +0800 (Thu, 15 Oct 2009) $
 *
 * Purpose : DRV APIs definition.
 *
 * Feature : Interrupt related API
 *
 */
#ifndef __INTR_RTL9310_H__
#define __INTR_RTL9310_H__


extern int32 rtl9310_intr_swcoreIsrSts_get(uint32 unit, uint32 *pisr_sts);
extern int32 rtl9310_intr_swcoreImrEnable_set(uint32 unit, drv_intr_isr_t isrId, rtk_enable_t enable);
extern int32 rtl9310_intr_swcoreSts_get(uint32 unit, drv_intr_isr_t isrId, drv_intr_data_t *pFunc_sts);


#endif /* __INTR_RTL9310_H__ */


