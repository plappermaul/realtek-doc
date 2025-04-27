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
 * $Revision: 93380 $
 * $Date: 2018-11-08 17:49:37 +0800 (Thu, 08 Nov 2018) $
 *
 * Purpose : Realtek Switch SDK Core Module In User Space.
 *
 * Feature : Realtek Switch SDK Core Module In User Space.
 *
 */

#ifndef __RTCORE_DRV_USR_H__
#define __RTCORE_DRV_USR_H__
/*
 * Include Files
 */
#include <common/type.h>
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */
/* Function Name:
 *      rtcore_usr_init
 * Description:
 *      Initialize RTCORE user layer module.
 * Input:
 *      NONE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtcore_usr_init(void);


/* Function Name:
 *      rtcore_usr_intr_attach
 * Description:
 *      Connect interrupt with rtcore module
 * Input:
 *      unit          - unit id
 *      fHandler      - The interrupt handler function that is going to be attached.
 *                      It is required to be called with 'pHandler_param' argument.
 *      pHandler_param- The argument passed to 'fHandler' interrupt handler when interrupt happen.
 *      type          - The specific interrupt type that we wants to connect with.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
extern int32
rtcore_usr_intr_attach(uint32 unit, void (*fHandler)(void*), void *pHandler_param, rtcore_intr_type_t type);

/* Function Name:
 *      rtcore_usr_intr_detach
 * Description:
 *      Disconnect interrupt with rtcore module
 * Input:
 *      unit - unit id
 *      type - The specific interrupt type that we wants to disconnect.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
extern int32
rtcore_usr_intr_detach(uint32 unit, rtcore_intr_type_t type);

/* Function Name:
 *      rtcore_bsp_memsize_info
 * Description:
 *      Get current system High/Low/DMA size information for BSP.
 *      DMA area is attached to the end of Low memory address.
 * Input:
 *      None
 *      low_memory_size  - low memory size
 *      high_memory_size - high memory size
 *      dma_size         - DMA size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern
int32 rtcore_system_memsize_info(uint32 * low_memory_size, uint32 
* high_memory_size, uint32 * dma_size);

#endif /* __RTCORE_DRV_USR_H__ */

