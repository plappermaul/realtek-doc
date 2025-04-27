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
 * $Revision: 91355 $
 * $Date: 2018-08-13 21:54:45 +0800 (Mon, 13 Aug 2018) $
 *
 * Purpose : Definition the prom.c prototype.
 *
 * Feature : Definition the prom.c prototype
 *
 */

#ifndef __RTK_BSP_PROM_H__
#define __RTK_BSP_PROM_H__

/* Function Name:
 *      prom_bsp_memsize_info
 * Description:
 *      Get Low/High/RTK_DMA size
 * Input:
 *      None
 * Output:
 *      low_memory_size     - Low Memory Size
 *      high_memory_size    - High Memory Size
 *      dma_reserved_size   - RTK DMA size
 * Return:
 *      None
 * Note:
 *
 */
extern void prom_bsp_memsize_info(unsigned int *low_memory_size, unsigned int *high_memory_size, unsigned int *dma_reserved_size);


#endif /* __RTK_BSP_PROM_H__ */

