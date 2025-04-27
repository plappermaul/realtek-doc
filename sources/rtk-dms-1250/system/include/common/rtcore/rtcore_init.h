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
 * Purpose : Definition of system init APIs in the SDK
 *
 * Feature : Initialize RTCORE modules
 *
 */

#ifndef __RTCORE_INIT_H__
#define __RTCORE_INIT_H__
/*
 * Include Files
 */
#include <common/type.h>

/*
 * Symbol Definition
 */

#define RTCORE_MMAP_REVMEM      (0x1)
#define RTCORE_MMAP_NICDMA      (0x2)


/*
 * Data Declaration
 */
extern uint32 rtk_unit2devID[];
extern uint32 rtk_dev2unitID[];

/*
 * Function Declaration
 */

/* Function Name:
 *      rtcore_init
 * Description:
 *      Initialize RTCORE module with the specified device
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      1. INIT must be initialized before using all of APIs in each modules
 */
extern int32
rtcore_init(void);

extern int32
rtcore_init_userModeLKM(void);

extern int32
rtcore_unit_attach(uint32 *unit, hwp_hwProfile_t *hwp, int local_unit_index);

extern void
rtcore_swcoreIntr_wakeup(uint32 *isr_glb_sts);

#endif /* __RTCORE_INIT_H__ */

