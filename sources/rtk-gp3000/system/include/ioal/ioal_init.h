/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * Purpose : IOAL Layer Init Module
 *
 * Feature : IOAL Init Functions
 *
 */

/*
 * Include Files
 */
#include <common/error.h>


/*
 * Symbol Definition
 */
#if defined(LINUX_KERNEL_MDIO_IO) || defined(CYGWIN_MDIO_IO) || (defined(CONFIG_LINUX_USER_SHELL) && defined(FPGA_DEFINED))
#define SWCORE_VIRT_BASE    0x1B000000
#else
#define SWCORE_VIRT_BASE    0xBB000000
#endif

#define SOC_VIRT_BASE       0xB8000000

#define SWCORE_PHYS_BASE    0x1B000000
#define SOC_PHYS_BASE       0x18000000

#define SWCORE_MEM_SIZE     0x2000000
#define SOC_MEM_SIZE        0x5000
#define SRAM_MEM_SIZE       0x8000
#define DMA_MEM_SIZE        0x100000

typedef enum ioal_memRegion_e
{
    IOAL_MEM_SWCORE = 0,
    IOAL_MEM_SOC,
    IOAL_MEM_SRAM,
    IOAL_MEM_DMA,
    IOAL_MEM_END
} ioal_memRegion_t;

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */
#define SOC_MMAP_BASE() (soc_base)

#define SOC32_READ(addr, val) \
do{ \
    val = MEM32_READ(SOC_MMAP_BASE()+addr); \
}while(0)

#define SOC32_WRITE(addr, val) \
do{ \
    MEM32_WRITE(SOC_MMAP_BASE()+addr, val); \
}while(0)

#define SOC32_FIELD_READ(addr, offset, mask, val) \
do{ \
    val = (MEM32_READ(SOC_MMAP_BASE()+addr) & mask) >> offset; \
}while(0)

#define SOC32_FIELD_WRITE(addr, offset, mask, val) \
do{ \
    MEM32_WRITE(SOC_MMAP_BASE()+addr, (MEM32_READ(SOC_MMAP_BASE()+addr) & ~mask) | (val << offset)); \
}while(0)


/*
 * Function Declaration
 */

/* Function Name:
 *      ioal_init_memBase_get
 * Description:
 *      Get memory base address
 * Input:
 *      mem       - memory region
 * Output:
 *      pBaseAddr - pointer to the base address of memory region
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
ioal_init_memRegion_get(ioal_memRegion_t mem, uint32 *pBaseAddr);

/* Function Name:
 *      ioal_init
 * Description:
 *      Init SDK IOAL Layer
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
ioal_init(void);

