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
 * Purpose : IOAL Layer Init Module
 *
 * Feature : IOAL Init Functions
 *
 */

/*
 * Include Files
 */
#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__


/*
 * Memeory Definition
 */

/* Size */
#define SWCORE_MEM_SIZE                 0x2000000
#define SOC_MEM_SIZE                    0x153FFF
#define SRAM_MEM_SIZE_128M              0x20000


#ifdef CONFIG_SDK_DRIVER_NIC_USER_MODE
#define DRAM_MEM_SIZE                   ioal_lowMem_size
#define DMA_MEM_SIZE                    ioal_dma_size  /* for NIC driver, L2NTFY driver DMA usage, for our Linux User Space example code */
#else
#define DRAM_MEM_SIZE                   0x10000000
#endif

/* Physical */
#define SWCORE_PHYS_BASE                0x1B000000
#define SOC_PHYS_BASE                   0x18000000
#define SRAM_PHYS_BASE                  0x1F000000
#define DRAM_PHYS_BASE                  0x00000000
#define DMA_PHYS_BASE                   (DRAM_PHYS_BASE + DRAM_MEM_SIZE)


/* Virtual */
#define SWCORE_VIRT_BASE                0xBB000000
#define SOC_VIRT_BASE                   0xB8000000
#define SRAM_VIRT_BASE                  0xBF000000
#define DRAM_CACHE_VIRT_BASE            0x80000000
#define DRAM_UNCACHE_VIRT_BASE          0xA0000000




#endif  /* __MEMORY_MAP_H__ */

