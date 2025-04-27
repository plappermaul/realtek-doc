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
 * $Revision: 87217 $
 * $Date: 2018-04-17 19:14:25 +0800 (Tue, 17 Apr 2018) $
 *
 * Purpose : IOAL Layer Init Module
 *
 * Feature : IOAL Init Functions
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <ioal/ioal_init.h>
#include <osal/memory.h>
#include <osal/lib.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <hwp/hw_profile.h>
#include <ioal/mem32.h>
#include <rtcore/user/rtcore_drv_usr.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
ioal_db_t ioal_db[RTK_MAX_NUM_OF_UNIT];
uint32 ioal_lowMem_size, ioal_highMem_size, ioal_dma_size;


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      ioal_init_memRegion_get
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
int32
ioal_init_memRegion_get(uint32 unit, ioal_memRegion_t mem, uint32 *pBaseAddr)
{
    switch(mem)
    {
        case IOAL_MEM_SWCORE:
            *pBaseAddr = ioal_db[unit].swcore_base;
            break;

        case IOAL_MEM_SOC:
            *pBaseAddr = ioal_db[unit].soc_base;
            break;

        case IOAL_MEM_SRAM:
            *pBaseAddr = ioal_db[unit].sram_base;
            break;

        case IOAL_MEM_DMA:
            *pBaseAddr = ioal_db[unit].pkt_base;
            break;

        case IOAL_MEM_L2NTFY_RING:
            *pBaseAddr = ioal_db[unit].l2ntfy_ring_base;
            break;

        case IOAL_MEM_L2NTFY_BUF:
            *pBaseAddr = ioal_db[unit].l2ntfy_buf_base;
            break;

        case IOAL_MEM_L2NTFY_USR:
            *pBaseAddr = ioal_db[unit].l2ntfy_usr_base;
            break;

        case IOAL_MEM_DESC:
            *pBaseAddr = ioal_db[unit].desc_base;
            break;

        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of ioal_init_memRegion_get */


/* Function Name:
 *      ioal_init
 * Description:
 *      Init ioal base for linux user space usage
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
int32
ioal_init(uint32 unit)
{

    uint32 chip_id, chip_rev_id;
    void    *pUserReg;
    uint32  used_dma_size;

    if(!HWP_USEDEFHWP())
        RT_INIT_MSG("  IOAL init\n");

    drv_swcore_ioalCB_register(ioal_mem32_read,ioal_mem32_write);

    rtcore_system_memsize_info(&ioal_lowMem_size, &ioal_highMem_size, &ioal_dma_size);

    /* check memory size definition */
    used_dma_size = (RTL8380_DESC_MEM_SIZE + RTL8380_PKT_MEM_SIZE + \
                     RTL8380_L2NTFY_RING_MEM_SIZE + RTL8380_L2NTFY_BUF_MEM_SIZE + RTL8380_L2NTFY_USR_MEM_SIZE);

    if ( ioal_dma_size < used_dma_size)
    {
        return RT_ERR_FAILED;
    }

    /* map memory */
    pUserReg = osal_mmap(MEM_DEV_NAME, SWCORE_PHYS_BASE, SWCORE_MEM_SIZE);
    if (RT_ERR_FAILED == (uint32)pUserReg)
        return RT_ERR_FAILED;
    ioal_db[unit].swcore_base = (uint32)pUserReg;

    pUserReg = osal_mmap(MEM_DEV_NAME, SOC_PHYS_BASE, SOC_MEM_SIZE);
    if (RT_ERR_FAILED == (uint32)pUserReg)
        return RT_ERR_FAILED;
    ioal_db[unit].soc_base = (uint32)pUserReg;

    pUserReg = osal_mmap(MEM_DEV_NAME, SRAM_PHYS_BASE, SRAM_MEM_SIZE_128M);
    if (RT_ERR_FAILED == (uint32)pUserReg)
        return RT_ERR_FAILED;
    ioal_db[unit].sram_base = (uint32)pUserReg;

    pUserReg = osal_mmap(MEM_DEV_NAME, DMA_PHYS_BASE, DMA_MEM_SIZE);
    if (RT_ERR_FAILED == (uint32)pUserReg)
        return RT_ERR_FAILED;
    ioal_db[unit].dma_base = (uint32)pUserReg;

    if( RT_ERR_OK != drv_swcore_cid_get(HWP_DEFHWP_UNIT_ID,&chip_id, &chip_rev_id) )
        return RT_ERR_FAILED;

    switch(FAMILY_ID(chip_id))
    {
        case RTL8390_FAMILY_ID:
        case RTL8350_FAMILY_ID:
            ioal_db[unit].desc_base         = ioal_db[unit].dma_base;
            ioal_db[unit].pkt_base          = ioal_db[unit].desc_base + RTL8390_DESC_MEM_SIZE;
            ioal_db[unit].l2ntfy_ring_base  = ioal_db[unit].pkt_base  + RTL8390_PKT_MEM_SIZE;
            ioal_db[unit].l2ntfy_buf_base   = ioal_db[unit].l2ntfy_ring_base + RTL8390_L2NTFY_RING_MEM_SIZE;
            ioal_db[unit].l2ntfy_usr_base   = ioal_db[unit].l2ntfy_buf_base + RTL8390_L2NTFY_BUF_MEM_SIZE;
            break;
        case RTL8380_FAMILY_ID:
        case RTL8330_FAMILY_ID:
            ioal_db[unit].desc_base         = ioal_db[unit].dma_base;
            ioal_db[unit].pkt_base          = ioal_db[unit].desc_base + RTL8380_DESC_MEM_SIZE;
            ioal_db[unit].l2ntfy_ring_base  = ioal_db[unit].pkt_base  + RTL8380_PKT_MEM_SIZE;
            ioal_db[unit].l2ntfy_buf_base   = ioal_db[unit].l2ntfy_ring_base + RTL8380_L2NTFY_RING_MEM_SIZE;
            ioal_db[unit].l2ntfy_usr_base   = ioal_db[unit].l2ntfy_buf_base + RTL8380_L2NTFY_BUF_MEM_SIZE;
            break;
        case RTL9310_FAMILY_ID:
            ioal_db[unit].desc_base         = ioal_db[unit].dma_base;
            ioal_db[unit].pkt_base          = ioal_db[unit].desc_base + RTL9310_DESC_MEM_SIZE;
            ioal_db[unit].l2ntfy_ring_base  = ioal_db[unit].pkt_base  + RTL9310_PKT_MEM_SIZE;
            ioal_db[unit].l2ntfy_buf_base   = ioal_db[unit].l2ntfy_ring_base + RTL9310_L2NTFY_RING_MEM_SIZE;
            ioal_db[unit].l2ntfy_usr_base   = ioal_db[unit].l2ntfy_buf_base + RTL9310_L2NTFY_BUF_MEM_SIZE;
            break;
        case RTL9300_FAMILY_ID:
            ioal_db[unit].desc_base         = ioal_db[unit].dma_base;
            ioal_db[unit].pkt_base          = ioal_db[unit].desc_base + RTL9300_DESC_MEM_SIZE;
            ioal_db[unit].l2ntfy_ring_base  = ioal_db[unit].pkt_base  + RTL9300_PKT_MEM_SIZE;
            ioal_db[unit].l2ntfy_buf_base   = ioal_db[unit].l2ntfy_ring_base + RTL9300_L2NTFY_RING_MEM_SIZE;
            ioal_db[unit].l2ntfy_usr_base   = ioal_db[unit].l2ntfy_buf_base + RTL9300_L2NTFY_BUF_MEM_SIZE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    ioal_db[unit].initialized = 1;

    return RT_ERR_OK;

} /* end of ioal_init */

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
ioal_init_unitID_change(uint32 fromID, uint32 toID)
{

    if (ioal_db[toID].initialized != 0)
        return RT_ERR_FAILED;

    if (ioal_db[fromID].initialized == 0)
        return RT_ERR_FAILED;

    ioal_db[toID] = ioal_db[fromID];
    osal_memset((void *)&(ioal_db[fromID]), 0, sizeof(ioal_db_t));

    return RT_ERR_OK;
}




