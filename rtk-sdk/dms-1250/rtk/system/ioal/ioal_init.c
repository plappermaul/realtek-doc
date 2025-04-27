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
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <ioal/ioal_init.h>
#include <osal/memory.h>
#include <osal/lib.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <hwp/hw_profile.h>
#include <ioal/mem32.h>
#if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
#include <rtcore/rtcore_pci.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
ioal_db_t ioal_db[RTK_MAX_NUM_OF_UNIT];
uint32 ioal_init_status = INIT_NOT_COMPLETED; /* won't check, since ioal may init multiple times */


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
ioal_init_memRegion_get(uint32 unit, ioal_memRegion_t mem, uintptr *pBaseAddr)
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
#ifdef CONFIG_SDK_DRIVER_NIC_USER_MODE
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
#endif
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of ioal_init_memRegion_get */


/* Function Name:
 *      ioal_init
 * Description:
 *      Init ioal base for linux kernel space usage
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
#if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
    rtcore_pci_dev_t    p;

    if(!HWP_USEDEFHWP())
        RT_INIT_MSG("  IOAL init\n");

    drv_swcore_ioalCB_register(ioal_mem32_read,ioal_mem32_write);

    rtcore_pci_info_get(RT_PCIDEV_SWCORE,&p);
    ioal_db[unit].swcore_base   = p.ioaddr;
    ioal_db[unit].swcore_size   = p.mem_size;
#else
    if(!HWP_USEDEFHWP())
        RT_INIT_MSG("  IOAL init\n");

    drv_swcore_ioalCB_register(ioal_mem32_read,ioal_mem32_write);

    ioal_db[unit].swcore_base   = SWCORE_VIRT_BASE;
    ioal_db[unit].soc_base      = SOC_VIRT_BASE;
    ioal_db[unit].sram_base     = SRAM_VIRT_BASE;
#endif

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


int32
ioal_debug_show(void)
{
    int i;

    for(i=0;i<RTK_MAX_NUM_OF_UNIT;i++){

        osal_printf("ioal_db[%d].initialized     = %d\n",   i, ioal_db[i].initialized     );
        osal_printf("ioal_db[%d].swcore_base     = 0x%x\n", i, (uint32)ioal_db[i].swcore_base);
        osal_printf("ioal_db[%d].soc_base        = 0x%x\n", i, (uint32)ioal_db[i].soc_base  );
        //osal_printf("ioal_db[%d].sram_base       = 0x%x\n", i, ioal_db[i].sram_base       );
        //osal_printf("ioal_db[%d].dma_base        = 0x%x\n", i, ioal_db[i].dma_base        );
        //osal_printf("ioal_db[%d].pkt_base        = 0x%x\n", i, ioal_db[i].pkt_base        );
        //osal_printf("ioal_db[%d].l2ntfy_ring_base= 0x%x\n", i, ioal_db[i].l2ntfy_ring_base);
        //osal_printf("ioal_db[%d].l2ntfy_buf_base = 0x%x\n", i, ioal_db[i].l2ntfy_buf_base );
        //osal_printf("ioal_db[%d].l2ntfy_usr_base = 0x%x\n", i, ioal_db[i].l2ntfy_usr_base );
        //osal_printf("ioal_db[%d].desc_base       = 0x%x\n", i, ioal_db[i].desc_base       );
    }

    return RT_ERR_OK;
}




