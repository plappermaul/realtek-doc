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
 * $Revision: 103112 $
 * $Date: 2019-12-27 16:17:03 +0800 (Fri, 27 Dec 2019) $
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

#if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

#include <common/debug/rt_log.h>
#include <ioal/ioal_init.h>
#include <osal/memory.h>
#include <osal/lib.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <hwp/hw_profile.h>
#include <ioal/mem32.h>
#include <rtcore/user/rtcore_drv_usr.h>



#if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
#include <common/rtcore/rtcore_init.h>
#include <rtcore/rtcore_pci.h>
#endif


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
ioal_db_t ioal_db[RTK_MAX_NUM_OF_UNIT];
#ifdef CONFIG_SDK_DRIVER_NIC_USER_MODE
uintptr ioal_lowMem_size, ioal_highMem_size, ioal_dma_size;
#endif

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
#if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
int32
rtcore_pci_info_get(rt_pcidevID_t pci_dev_id, rtcore_pci_dev_t *pci_info)
{
    int32 fd;
    rtcore_ioctl_t dio;
    rtcore_pci_dev_t *p;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = pci_dev_id;
    ioctl(fd, RTCORE_PCI_INFO_GET, &dio);
    if (dio.ret == RT_ERR_OK)
    {
        p = (rtcore_pci_dev_t *)&dio.data[1];
        osal_memcpy(pci_info, p, sizeof(rtcore_pci_dev_t));
    }

    close(fd);

    return dio.ret;

}
#endif /* CONFIG_SDK_DRIVER_EXTC_PCI */


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

        case IOAL_MEM_NIC:
            *pBaseAddr = ioal_db[unit].nic_base;
            break;

#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
        case IOAL_MEM_DMA:
            *pBaseAddr = ioal_db[unit].dma_base;
            break;

        case IOAL_MEM_DMA_PHY:
            *pBaseAddr = ioal_db[unit].dma_phy_base;
            break;
#else
        case IOAL_MEM_DMA:
            *pBaseAddr = ioal_db[unit].pkt_base;
            break;
#endif

#ifdef CONFIG_SDK_DRIVER_NIC_USER_MODE

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


#if defined(CONFIG_SDK_DRIVER_EXTC_PCI)
/* Function Name:
 *      _ioal_extc_pci_init
 * Description:
 *      Init ioal base for linux user space usage in external CPU with PCI interface environment
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
_ioal_extc_pci_init(uint32 unit)
{
    void    *pUserReg;
    rtcore_pci_dev_t pci_info;
    int pageSize = 0;


    /* swcore map memory */
    rtcore_pci_info_get(RT_PCIDEV_SWCORE, &pci_info);
    pUserReg = osal_mmap(MEM_DEV_NAME, pci_info.physical_base_addr, pci_info.mem_size);
    if (RT_ERR_FAILED == (uint32)((uintptr)pUserReg) )
    {
        osal_printf("%s(%d): swcore pci mmap failed (phy add=0x%x)\n", __FUNCTION__, __LINE__, (uint32)(uintptr)pci_info.physical_base_addr);
        return RT_ERR_FAILED;
    }
    ioal_db[unit].swcore_base = (uintptr)pUserReg;
    ioal_db[unit].swcore_size = pci_info.mem_size;


    /* NIC map memory */
    rtcore_pci_info_get(RT_PCIDEV_NIC, &pci_info);
    pUserReg = osal_mmap(MEM_DEV_NAME, pci_info.physical_base_addr, pci_info.mem_size);
    if (RT_ERR_FAILED == (uint32)((uintptr)pUserReg) )
    {
        osal_printf("%s(%d): nic pci mmap failed\n", __FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }
    ioal_db[unit].nic_base = (uintptr)pUserReg;


    /* NIC DMA map memory */
    rtcore_pci_info_get(RT_PCIDEV_NIC_DMA, &pci_info);

    /* Get memory page size. */
    pageSize = getpagesize();
    pUserReg = osal_mmap(RTCORE_DEV_NAME, RTCORE_MMAP_NICDMA * pageSize, pci_info.mem_size);
    if (RT_ERR_FAILED == (uint32)((uintptr)pUserReg) )
    {
        osal_printf("%s(%d): nic dma mmap failed\n", __FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }
    ioal_db[unit].dma_base = (uintptr)pUserReg;
    ioal_db[unit].dma_phy_base = pci_info.physical_base_addr;


    ioal_db[unit].initialized = 1;
    return RT_ERR_OK;
}

#else /* else of CONFIG_SDK_DRIVER_EXTC_PCI */

/* Function Name:
 *      _ioal_init
 * Description:
 *      Init ioal base for linux user space usage with internal CPU or external CPU with SPI/I2C interface environment
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
_ioal_init(uint32 unit)
{
    uint32 chip_id, chip_rev_id;
    void    *pUserReg;
#ifdef CONFIG_SDK_DRIVER_NIC_USER_MODE
    uint32  used_dma_size;
#endif

#ifdef CONFIG_SDK_DRIVER_NIC_USER_MODE
    rtcore_system_memsize_info(&ioal_lowMem_size, &ioal_highMem_size, &ioal_dma_size);

    /* check memory size definition */
    used_dma_size = (RTL8380_DESC_MEM_SIZE + RTL8380_PKT_MEM_SIZE + \
                     RTL8380_L2NTFY_RING_MEM_SIZE + RTL8380_L2NTFY_BUF_MEM_SIZE + RTL8380_L2NTFY_USR_MEM_SIZE);

    if ( ioal_dma_size < used_dma_size)
    {
        return RT_ERR_FAILED;
    }
#endif

    /* map memory */
    pUserReg = osal_mmap(MEM_DEV_NAME, SWCORE_PHYS_BASE, SWCORE_MEM_SIZE);
    if (RT_ERR_FAILED == (uint32)(uintptr)pUserReg)
        return RT_ERR_FAILED;
    ioal_db[unit].swcore_base = (uintptr)pUserReg;

    pUserReg = osal_mmap(MEM_DEV_NAME, SOC_PHYS_BASE, SOC_MEM_SIZE);
    if (RT_ERR_FAILED == (uint32)(uintptr)pUserReg)
        return RT_ERR_FAILED;
    ioal_db[unit].soc_base = (uintptr)pUserReg;

    pUserReg = osal_mmap(MEM_DEV_NAME, SRAM_PHYS_BASE, SRAM_MEM_SIZE_128M);
    if (RT_ERR_FAILED == (uint32)(uintptr)pUserReg)
        return RT_ERR_FAILED;
    ioal_db[unit].sram_base = (uintptr)pUserReg;

#ifdef CONFIG_SDK_DRIVER_NIC_USER_MODE
    pUserReg = osal_mmap(MEM_DEV_NAME, DMA_PHYS_BASE, DMA_MEM_SIZE);
    if (RT_ERR_FAILED == (uint32)(uintptr)pUserReg)
        return RT_ERR_FAILED;
    ioal_db[unit].dma_base = (uintptr)pUserReg;
#endif

    if( RT_ERR_OK != drv_swcore_cid_get(HWP_DEFHWP_UNIT_ID,&chip_id, &chip_rev_id) )
        return RT_ERR_FAILED;

#ifdef CONFIG_SDK_DRIVER_NIC_USER_MODE
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
#endif

    ioal_db[unit].initialized = 1;

    return RT_ERR_OK;
}


#endif /* CONFIG_SDK_DRIVER_EXTC_PCI */

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
    if(!HWP_USEDEFHWP())
        RT_INIT_MSG("  IOAL init\n");

    drv_swcore_ioalCB_register(ioal_mem32_read,ioal_mem32_write);

#if !defined(CONFIG_SDK_DRIVER_EXTC_PCI)
    return _ioal_init(unit);
#else
    return _ioal_extc_pci_init(unit);
#endif
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




