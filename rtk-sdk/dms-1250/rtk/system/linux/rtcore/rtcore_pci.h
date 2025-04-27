/*
 * Copyright (C) 2009-2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 97155 $
 * $Date: 2019-05-24 14:22:30 +0800 (Fri, 24 May 2019) $
 *
 * Purpose : PCI init
 *
 * Feature : PCI init
 *
 */
#ifndef __RTCORE_PCI_H__
#define __RTCORE_PCI_H__

/*
 * Include Files
 */
#define CONSIST_MEM_RSVD_SIZE           0x400000

typedef enum rtcore_pci_devId_s {
    RT_PCIDEV_SWCORE      = 0,
    RT_PCIDEV_NIC,
    RT_PCIDEV_NIC_DMA,
    RT_PCIDEV_END,
} rt_pcidevID_t;


typedef struct rtcore_pci_dev_s
{
    void                    *pdev;
    uintptr                 physical_base_addr;
    uintptr                 vir_base_addr;
    uintptr                 ioaddr;
    uint32                  mem_size;
} rtcore_pci_dev_t;


int32
rtcore_pci_info_get(rt_pcidevID_t pci_dev_id, rtcore_pci_dev_t *pci_info);

int32
rtcore_pci_init(void);

int32
rtcore_pci_exit(void);


#endif /* __RTCORE_PCI_H__ */


