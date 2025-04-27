/*
 * Copyright (C) 2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : Implementation of the NIC driver for U-Boot.
 *
 * Feature : NIC driver common layer
 */


/*
 * Include Files
 */
#include <ioal/mem32.h>
#include <osal/isr.h>
#include <osal/print.h>
#include <osal/spl.h>
#include <osal/memory.h>
#include <osal/lib.h>
#include <dev_config.h>
#include <drv/nic/nic.h>
#if defined(CONFIG_SDK_RTL8390)
  #include <private/drv/swcore/swcore_rtl8390.h>
#endif
#if defined(CONFIG_SDK_RTL8380)
  #include <private/drv/swcore/swcore_rtl8380.h>
#endif
#if defined(CONFIG_SDK_RTL9310)
  #include <private/drv/swcore/swcore_rtl9310.h>
#endif
#include <rtnic/rtnic_drv.h>
#include <hwp/hw_profile.h>
#include <net.h>
#include <common/debug/rt_log.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32 gUnit;
static uint32 memUsage;
rtk_dev_t rtkDev;


/*
 * Macro Definition
 */
#define IS_VALID_DEVICE_ID(_id) ((_id) < RTK_DEV_MAX)

/*
 * Function Declaration
 */
int32 uboot_rtnic_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket);

int32 osal_isr_register(uint32 dev_id, osal_isr_t isr, void *isr_param)
{

    if (!IS_VALID_DEVICE_ID(dev_id))
    {
        RT_INIT_MSG(" osal_isr_register: Invalid device index %u\n", dev_id);
        return RT_ERR_FAILED;
    }


    rtkDev.fIsr = isr;
    rtkDev.pIsr_param = isr_param;

    return RT_ERR_OK;
}


int32 osal_isr_unregister(uint32 dev_id)
{
    if (!IS_VALID_DEVICE_ID(dev_id))
    {
        RT_INIT_MSG(" osal_isr_register: Invalid device index %u\n", dev_id);
        return RT_ERR_FAILED;
    }

    rtkDev.fIsr = NULL;
    rtkDev.pIsr_param = NULL;

    return RT_ERR_OK;
}



/* Function Name:
 *      swnic_init
 * Description:
 *      net device initialization callback
 * Input:
 *      dev     ethernet net device
 * Output:
 *      None
 * Return:
 *
 * Note:
 *      None
 */
int swnic_init(struct eth_device *dev, bd_t *bis)
{
#if defined(CONFIG_SDK_DRIVER_NIC_R8390) || defined(CONFIG_SDK_DRIVER_NIC_R9310)
    uint32  temp;
#endif
    int32     ret;

#if defined(CONFIG_SDK_DRIVER_NIC_R8390)
    if (HWP_8390_50_FAMILY(gUnit))
    {
        ioal_mem32_write(gUnit, RTL8390_TBL_ACCESS_L2_CTRL_ADDR, 0x28000);
        ioal_mem32_read(gUnit, RTL8390_TBL_ACCESS_L2_DATA_ADDR(0), &temp);
        temp |= 0x80000000;
        ioal_mem32_write(gUnit, RTL8390_TBL_ACCESS_L2_DATA_ADDR(0), temp);
        ioal_mem32_write(gUnit, RTL8390_TBL_ACCESS_L2_CTRL_ADDR, 0x38000);
    }
#endif /* CONFIG_SDK_DRIVER_NIC_R8390 */

#if defined(CONFIG_SDK_DRIVER_NIC_R9310)
    ioal_mem32_field_read(gUnit, RTL9310_L2_UNKN_UC_FLD_PMSK_ADDR, 0, 0x1FFFFFF, &temp);
    temp |= 0x1000000;
    ioal_mem32_field_write(gUnit, RTL9310_L2_UNKN_UC_FLD_PMSK_ADDR, 0, 0x1FFFFFF, temp);

    /* Add CPU port to VLAN 1 */
    ioal_mem32_write(gUnit, RTL9310_TBL_ACCESS_CTRL_0_ADDR, 0x118001);
    ioal_mem32_read(gUnit, RTL9310_TBL_ACCESS_DATA_0_ADDR(0), &temp);
    temp |= 0x80000000;
    ioal_mem32_write(gUnit, RTL9310_TBL_ACCESS_DATA_0_ADDR(0), temp);
    ioal_mem32_write(gUnit, RTL9310_TBL_ACCESS_CTRL_0_ADDR, 0x198001);
#endif

    ret = drv_nic_rx_start(gUnit);
    if (ret != RT_ERR_OK)
    {
        osal_printf("NIC init Error! unit %u NIC chip family: %x\n", gUnit, HWP_CHIP_FAMILY_ID(gUnit));
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      swnic_recv
 * Description:
 *      net device halt callback
 * Input:
 *      dev     ethernet net device
 * Output:
 *      None
 * Return:
 *
 * Note:
 *      None
 */
void swnic_halt(struct eth_device *dev)
{
#if defined(CONFIG_SDK_DRIVER_NIC_R8390) || defined(CONFIG_SDK_DRIVER_NIC_R9310)
    uint32      val;
#endif
    int32       ret;

#if defined(CONFIG_SDK_DRIVER_NIC_R8390)
    if (HWP_8390_50_FAMILY(gUnit))
    {
        /* Remove the CPU port from Lookup Miss Flooding Portmask */
        ioal_mem32_write(gUnit, RTL8390_TBL_ACCESS_L2_CTRL_ADDR, 0x28000);
        ioal_mem32_read(gUnit, RTL8390_TBL_ACCESS_L2_DATA_ADDR(0), &val);
        val &= ~(0x80000000);
        ioal_mem32_write(gUnit, RTL8390_TBL_ACCESS_L2_DATA_ADDR(0), val);
        ioal_mem32_write(gUnit, RTL8390_TBL_ACCESS_L2_CTRL_ADDR, 0x38000);
    }
#endif /* CONFIG_SDK_DRIVER_NIC_R8390 */

#if defined(CONFIG_SDK_DRIVER_NIC_R9310)
    ioal_mem32_field_read(gUnit, RTL9310_L2_UNKN_UC_FLD_PMSK_ADDR, 0, 0x1FFFFFF, &val);
    val &= ~(0x1000000);
    ioal_mem32_field_write(gUnit, RTL9310_L2_UNKN_UC_FLD_PMSK_ADDR, 0, 0x1FFFFFF, val);
#endif

    ret = drv_nic_rx_stop(gUnit);
    if (ret != RT_ERR_OK)
    {
        osal_printf("unit %d NIC halt fail %x!\n", gUnit, ret);
    }
    return ;
}


/* Function Name:
 *      swnic_recv
 * Description:
 *      net device send packet callback
 * Input:
 *      dev     ethernet net device
 * Output:
 *      None
 * Return:
 *
 * Note:
 *      None
 */
int swnic_send(struct eth_device *dev, volatile void *pPayload, int length)
{
    int32 ret;
    drv_nic_pkt_t *pPacket;

    uboot_rtnic_pkt_alloc(gUnit, length, 0, &pPacket);



    /* setting the CPU tx tag */
    pPacket->as_txtag = 0;

    /* raw packet */
    osal_memcpy(pPacket->head, (char *)pPayload, length);
    pPacket->data = pPacket->head;
    pPacket->length = length;
    pPacket->tail = pPacket->data + pPacket->length;
    pPacket->end = pPacket->tail;
    pPacket->next = NULL;

    ret  = drv_nic_pkt_tx(gUnit, pPacket, NULL, NULL);

    return ret;
}


/* Function Name:
 *      swnic_recv
 * Description:
 *      net device receive packet callback
 * Input:
 *      dev     ethernet net device
 * Output:
 *      None
 * Return:
 *
 * Note:
 *      None
 */
int swnic_recv(struct eth_device *dev)
{
    osal_isr_t dev_isr;    /* used for cast isr */

    dev_isr = (osal_isr_t)rtkDev.fIsr;
    dev_isr(rtkDev.pIsr_param);

    return RT_ERR_OK;
}


/* Function Name:
 *      uboot_rtnic_pkt_alloc
 * Description:
 *
 * Input:
 *
 * Output:
 *      None
 * Return:
 *
 * Note:
 *      None
 */
int32 uboot_rtnic_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket)
{
    uint8* data;
    drv_nic_pkt_t *pPacket;

    pPacket = osal_alloc(sizeof(drv_nic_pkt_t));
    memUsage += sizeof(drv_nic_pkt_t);
    if (NULL == pPacket)
    {
        osal_printf("uboot: allocate nic packet buffer fail\n");
        return RT_ERR_FAILED;
    }

    /* Allocate more space than spcified, so that we can make the whole DMA block has its own cache lines, and guarantee the coherence futher*/
    data = osal_alloc(size);
    memUsage += size;
    if (NULL == data)
    {
        osal_free(pPacket);
        osal_printf("uboot: allocate skb fail   memUsage:%u\n", memUsage);
        return RT_ERR_FAILED;
    }

    osal_memset(pPacket, 0, sizeof(drv_nic_pkt_t));
    pPacket->head = data;
    pPacket->data = data;
    pPacket->tail = data;
    pPacket->end = data + size;
    pPacket->length = size;
    pPacket->next = NULL;


    *ppPacket = pPacket;

    return RT_ERR_OK;
}


/* Function Name:
 *      uboot_rtnic_pkt_free
 * Description:
 *
 * Input:
 *
 * Output:
 *      None
 * Return:
 *
 * Note:
 *      None
 */
int32 uboot_rtnic_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket)
{
    if (NULL == pPacket)
    {
        osal_printf("pPacket is NULL\n");
        return RT_ERR_FAILED;
    }

    if (NULL == pPacket->head)
    {
        osal_printf("payload is NULL\n");
        return RT_ERR_FAILED;
    }

    memUsage -= pPacket->length;
    osal_free(pPacket->head);
    memUsage -= sizeof(drv_nic_pkt_t);
    osal_free(pPacket);
    return RT_ERR_OK;
}


/* Function Name:
 *      uboot_rtnic_rx_callback
 * Description:
 *      Initialize NIC receive packet callback.
 * Input:
 *
 * Output:
 *      None
 * Return:
 *
 * Note:
 *      None
 */
drv_nic_rx_t uboot_rtnic_rx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    NetReceive(pPacket->data, pPacket->length);

    return NIC_RX_NOT_HANDLED;
}

/* Function Name:
 *      uboot_rtnic_drv_init
 * Description:
 *      Initialize NIC device.
 * Input:
 *      unit - unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK   - register ethernet device success
 * Note:
 *      None
 */
int uboot_rtnic_drv_init(uint32 unit)
{
    drv_nic_initCfg_t initCfg;
    int32             ret;
    struct eth_device *dev;


    RT_INIT_MSG("  Loader RTNIC Driver Module Initialize\n");
    gUnit = unit;

#if defined(CONFIG_SDK_DRIVER_NIC_R9310)
    ioal_mem32_field_write(gUnit, RTL9310_MEM_IGR_INIT_ADDR, RTL9310_MEM_IGR_INIT_LINK_LIST_INIT_OFFSET, RTL9310_MEM_IGR_INIT_LINK_LIST_INIT_MASK, 0x1);
    ioal_mem32_field_write(gUnit, RTL9310_MEM_ENCAP_INIT_ADDR, RTL9310_MEM_ENCAP_INIT_MEM_INIT_OFFSET, RTL9310_MEM_ENCAP_INIT_MEM_INIT_MASK, 0x1);
    ioal_mem32_field_write(gUnit, RTL9310_MEM_EGR_INIT_ADDR, RTL9310_MEM_EGR_INIT_MEM_INIT_OFFSET, RTL9310_MEM_EGR_INIT_MEM_INIT_MASK, 0x1);
    ioal_mem32_field_write(gUnit, RTL9310_MEM_MIB_INIT_ADDR, RTL9310_MEM_MIB_INIT_MEM_RST_OFFSET, RTL9310_MEM_MIB_INIT_MEM_RST_MASK, 0x1);
    ioal_mem32_write(gUnit, RTL9310_MEM_ALE_INIT_0_ADDR, 0xffffffff);
    ioal_mem32_write(gUnit, RTL9310_MEM_ALE_INIT_1_ADDR, 0xffffffff);
    ioal_mem32_write(gUnit, RTL9310_MEM_ALE_INIT_2_ADDR, 0xffffffff);
#endif

    /* NIC Initialization */
    initCfg.pkt_alloc = uboot_rtnic_pkt_alloc;
    initCfg.pkt_free = uboot_rtnic_pkt_free;
    initCfg.pkt_size = RTNIC_MAX_PKTLEN;
    if (RT_ERR_OK != (ret = drv_nic_init(unit, &initCfg)))
    {
        goto out;
    }
    if (RT_ERR_OK != drv_nic_rx_register(unit, 1, uboot_rtnic_rx_callback, 0, 0))
    {
        goto out;
    }

    /* Alloc memory to ethernet device info */
    dev = (struct eth_device *)malloc(sizeof *dev);
    if (NULL == dev)
    {
        ret = RT_ERR_FAILED;
        goto out;
    }

    /* Set the ethernet device info */
    if (HWP_9300_FAMILY_ID(unit))
        sprintf(dev->name, "rtl9300#0");
    else if (HWP_9310_FAMILY_ID(unit))
        sprintf(dev->name, "rtl9310#0");
    else if (HWP_8390_50_FAMILY(unit))
        sprintf(dev->name, "rtl8390#0");
    else if (HWP_8380_30_FAMILY(unit))
        sprintf(dev->name, "rtl8380#0");
    else
        sprintf(dev->name, "unknown#0");

    dev->priv = NULL;
    dev->iobase = 0;
    dev->init = swnic_init;
    dev->halt = swnic_halt;
    dev->send = swnic_send;
    dev->recv = swnic_recv;
    dev->write_hwaddr = NULL;

    /* Register the ethernet device */
    eth_register(dev);

    return RT_ERR_OK;

out:
#ifndef __RTNIC_MODULE__
    osal_printf("Init Switch Ethernet Driver....FAIL\n");
#endif
    return ret;
}

