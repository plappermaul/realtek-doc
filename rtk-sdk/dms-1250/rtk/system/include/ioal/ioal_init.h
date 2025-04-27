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
 * $Revision: 100813 $
 * $Date: 2019-10-08 22:22:51 +0800 (Tue, 08 Oct 2019) $
 *
 * Purpose : IOAL Layer Init Module
 *
 * Feature : IOAL Init Functions
 *
 */

/*
 * Include Files
 */
#ifndef __IOAL_INIT_H__
#define __IOAL_INIT_H__

#include <common/memory_map.h>


#ifdef __BOOTLOADER__

#else
#include <common/rt_autoconf.h>
#endif
#include <soc/type.h>
#include <common/error.h>

/*
 * Symbol Definition
 */

#define RTL8380_DESC_MEM_SIZE           0x100000
#define RTL8380_PKT_MEM_SIZE            0x400000
#define RTL8390_DESC_MEM_SIZE           RTL8380_DESC_MEM_SIZE
#define RTL8390_PKT_MEM_SIZE            RTL8380_PKT_MEM_SIZE
#define RTL9300_DESC_MEM_SIZE           RTL8380_DESC_MEM_SIZE
#define RTL9300_PKT_MEM_SIZE            RTL8380_PKT_MEM_SIZE
#define RTL9310_DESC_MEM_SIZE           RTL8380_DESC_MEM_SIZE
#define RTL9310_PKT_MEM_SIZE            RTL8380_PKT_MEM_SIZE

#define RTL8380_L2NTFY_RING_MEM_SIZE    0
#define RTL8380_L2NTFY_BUF_MEM_SIZE     0
#define RTL8380_L2NTFY_USR_MEM_SIZE     0

#if defined(CONFIG_SDK_DRIVER_L2NTFY)

#define RTL8390_L2NTFY_RING_MEM_SIZE    0x2000
#define RTL8390_L2NTFY_BUF_MEM_SIZE     0x80000

#define RTL9300_L2NTFY_RING_MEM_SIZE    RTL8390_L2NTFY_RING_MEM_SIZE
#define RTL9300_L2NTFY_BUF_MEM_SIZE     RTL8390_L2NTFY_BUF_MEM_SIZE

#define RTL9310_L2NTFY_RING_MEM_SIZE    0x8000
#define RTL9310_L2NTFY_BUF_MEM_SIZE     0x100000

#else

#define RTL8390_L2NTFY_RING_MEM_SIZE    0
#define RTL8390_L2NTFY_BUF_MEM_SIZE     0

#define RTL9300_L2NTFY_RING_MEM_SIZE    0
#define RTL9300_L2NTFY_BUF_MEM_SIZE     0
#define RTL9300_L2NTFY_USR_MEM_SIZE     0

#define RTL9310_L2NTFY_RING_MEM_SIZE    0
#define RTL9310_L2NTFY_BUF_MEM_SIZE     0

#endif




#ifdef CONFIG_SDK_DRIVER_NIC_USER_MODE
#define RTL8380_DESC_PHYS_BASE          (ioal_lowMem_size)
#define RTL8380_PKT_PHYS_BASE           (RTL8380_DESC_PHYS_BASE + RTL8380_DESC_MEM_SIZE)

#define RTL8390_DESC_PHYS_BASE          (ioal_lowMem_size)
#define RTL8390_PKT_PHYS_BASE           (RTL8390_DESC_PHYS_BASE + RTL8390_DESC_MEM_SIZE)
#define RTL8390_L2NTFY_RING_PHYS_BASE   (RTL8390_PKT_PHYS_BASE + RTL8390_PKT_MEM_SIZE)
#define RTL8390_L2NTFY_BUF_PHYS_BASE    (RTL8390_L2NTFY_RING_PHYS_BASE + RTL8390_L2NTFY_RING_MEM_SIZE)
#define RTL8390_L2NTFY_USR_PHYS_BASE    (RTL8390_L2NTFY_BUF_PHYS_BASE + RTL8390_L2NTFY_BUF_MEM_SIZE)

#define RTL9300_DESC_PHYS_BASE          (ioal_lowMem_size)
#define RTL9300_PKT_PHYS_BASE           (RTL9300_DESC_PHYS_BASE + RTL9300_DESC_MEM_SIZE)
#define RTL9300_L2NTFY_RING_PHYS_BASE   (RTL9300_PKT_PHYS_BASE + RTL9300_PKT_MEM_SIZE)
#define RTL9300_L2NTFY_BUF_PHYS_BASE    (RTL9300_L2NTFY_RING_PHYS_BASE + RTL9300_L2NTFY_RING_MEM_SIZE)
#define RTL9300_L2NTFY_USR_PHYS_BASE    (RTL9300_L2NTFY_BUF_PHYS_BASE + RTL9300_L2NTFY_BUF_MEM_SIZE)

#define RTL9310_DESC_PHYS_BASE          (ioal_lowMem_size)
#define RTL9310_PKT_PHYS_BASE           (RTL9310_DESC_PHYS_BASE + RTL9310_DESC_MEM_SIZE)
#define RTL9310_L2NTFY_RING_PHYS_BASE   (RTL9310_PKT_PHYS_BASE + RTL9310_PKT_MEM_SIZE)
#define RTL9310_L2NTFY_BUF_PHYS_BASE    (RTL9310_L2NTFY_RING_PHYS_BASE + RTL9310_L2NTFY_RING_MEM_SIZE)
#define RTL9310_L2NTFY_USR_PHYS_BASE    (RTL9310_L2NTFY_BUF_PHYS_BASE + RTL9310_L2NTFY_BUF_MEM_SIZE)
#endif



typedef enum ioal_memRegion_e
{
    IOAL_MEM_SWCORE = 0,
    IOAL_MEM_SOC,
    IOAL_MEM_SRAM,
    IOAL_MEM_NIC,
    IOAL_MEM_DMA,
    IOAL_MEM_DMA_PHY,
    IOAL_MEM_L2NTFY_RING,
    IOAL_MEM_L2NTFY_BUF,
    IOAL_MEM_L2NTFY_USR,
    IOAL_MEM_DESC,
    IOAL_MEM_END
} ioal_memRegion_t;

typedef struct ioal_db_s
{
    uint32 initialized;

    uintptr swcore_base;    /* switch core (MAC) register base address */
    uintptr soc_base;       /* internal CPU BSP register base address */
    uintptr sram_base;      /* internal SRAM base address */
    uintptr nic_base;       /* NIC function register base address */
    uintptr dma_base;       /* NIC DMA base address */
    uintptr dma_phy_base;   /* NIC DMA physical base address */

    /* dma_base[] area break down to following section */
    uintptr pkt_base;
    uintptr l2ntfy_ring_base;
    uintptr l2ntfy_buf_base;
    uintptr l2ntfy_usr_base;
    uintptr desc_base;

    uint32 swcore_size;
}ioal_db_t;

#ifdef CONFIG_SDK_DRIVER_NIC_USER_MODE
extern uintptr ioal_lowMem_size, ioal_highMem_size, ioal_dma_size;
#endif

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      ioal_init_memBase_get
 * Description:
 *      Get memory base address
 * Input:
 *      unit      - unit id
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
ioal_init_memRegion_get(uint32 unit, ioal_memRegion_t mem, uintptr *pBaseAddr);

/* Function Name:
 *      ioal_init
 * Description:
 *      Init SDK IOAL Layer
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
extern int32
ioal_init(uint32 unit);

/* Function Name:
 *      ioal_init_unitID_change
 * Description:
 *      Change a unit ID from fromID to toID, and delete fromID.
 * Input:
 *      fromID        - original unit ID
 *      toID          - new unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
ioal_init_unitID_change(uint32 fromID, uint32 toID);

int32
ioal_debug_show(void);


#endif  /* __IOAL_INIT_H__ */

