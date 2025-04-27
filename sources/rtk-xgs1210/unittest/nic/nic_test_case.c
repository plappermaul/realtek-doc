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
 * $Revision: 82367 $
 * $Date: 2017-09-21 13:59:33 +0800 (Thu, 21 Sep 2017) $
 *
 * Purpose : Definition of NIC test APIs in the SDK
 *
 * Feature : NIC test APIs
 *
 */

/*
 * Include Files
 */
#include <dev_config.h>
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <ioal/mem32.h>
#include <private/drv/nic/nic_common.h>
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <hal/chipdef/allreg.h>
#include <hal/mac/reg.h>
#include <hal/chipdef/chip.h>
#include <hal/chipdef/driver.h>
#include <hal/common/halctrl.h>
#include <nic/nic_test_case.h>

static int gDBG_Dump_Rx = 1;
static int gDBG_Dump_Tx = 1;
static int gDBG_Show_PktDropRate = 1;

#define PKTBUF_ALLOC(size)    osal_alloc(size)
#define PKTBUF_FREE(pktbuf)   osal_free(pktbuf)

/*
 * Function Declaration
 */
static void
rtl83xx_nic_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    if (pPacket == NULL)
    {
        goto _exit;
    }
    osal_free(pCookie);
    PKTBUF_FREE(pPacket);

_exit:
    return;
}

/* Nic Testing Case */
int32
nic_case2_test(uint32 caseNo, uint32 unit)
{
    osal_printf("gDBG_Dump_Rx = %d\n", gDBG_Dump_Rx);
    osal_printf("gDBG_Dump_Tx = %d\n", gDBG_Dump_Tx);
    osal_printf("gDBG_Show_PktDropRate = %d\n", gDBG_Show_PktDropRate);

    return RT_ERR_OK;
}

int32
nic_case3_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    uint8 pkt1[] = { /* ARP: Who has 192.168.154.80? Tell 192.168.154.89 */
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x52, 0x45, 0x41, 0x4c, 0x30, 0x59, 0x08, 0x06, 0x00, 0x01,
        0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x52, 0x45, 0x41, 0x4c, 0x30, 0x59, 0xc0, 0xa8, 0x9a, 0x59,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x9a, 0x50, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
        };
    //struct rtl83xx_priv *priv = netdev_priv(ndev);
    uint8 *tx_buff;
    drv_nic_pkt_t *pPacket;

    RT_LOG(LOG_WARNING, MOD_UNITTEST, "=================== Tx one packet ===================\n");

    /* stopping send other packet */
    //netif_stop_queue(ndev);

    pPacket = PKTBUF_ALLOC(sizeof(drv_nic_pkt_t));
    if (pPacket == NULL)
    {
        /* out of memory */
        return RT_ERR_FAILED;   /* Failed */
    }

    tx_buff = osal_alloc(1600);
    if (tx_buff == NULL)
    {
        PKTBUF_FREE(pPacket);
        return RT_ERR_FAILED;
    }

//    if (CCL_P(skb)->wSize > 0)
    {
        pPacket->as_txtag = 1;

        pPacket->tx_tag.dst_port_mask       = (0x1 << 2);
        pPacket->tx_tag.fwd_type            = NIC_FWD_TYPE_LOGICAL;
        pPacket->tx_tag.as_priority         = 1;
        pPacket->tx_tag.priority            = 7;
        pPacket->tx_tag.l2_learning         = 0;
    }
//    else
//    {
//        pPacket->as_txtag = 0;
//    }

    /* raw packet */
    pPacket->buf_id = (void *)NULL;
    pPacket->head = tx_buff;
    pPacket->data = tx_buff+8;
    pPacket->tail = tx_buff+8+64;
    pPacket->end = tx_buff+1600;
    pPacket->length = 60;
    pPacket->next = NULL;

    memcpy(pPacket->data, pkt1, 60);

    if (gDBG_Dump_Tx)
    {
        int i;
        int dump_len = 256;
        char buff[dump_len], temp_buff[16];

        osal_memset(buff, 0, sizeof(buff));\
        osal_memset(temp_buff, 0, sizeof(temp_buff));
        RT_LOG(LOG_WARNING, MOD_UNITTEST, "=== [TX] ================================================== Len: %d", pPacket->length);
        for (i=0; i<dump_len; i++) {
            if (i == (pPacket->length)) break;
            if (i % 16 == 0)
            {
                sprintf(buff, "[%04X] ", i);
                //RT_LOG(LOG_WARNING, MOD_UNITTEST, "[%04X] ", i);
            }
            sprintf(temp_buff, "%02X ", *(pPacket->data + i));
            strcat(buff, temp_buff);
            //RT_LOG(LOG_WARNING, MOD_UNITTEST, "%02X ", *(pPacket->data + i));
            if (i % 16 == 15)
            {
                RT_LOG(LOG_WARNING, MOD_UNITTEST, "%s", buff);
                osal_memset(buff, 0, sizeof(buff));
            }
        }
        if ((i % 16) != 0)
            RT_LOG(LOG_WARNING, MOD_UNITTEST, "%s", buff);
    }

    //if (RT_ERR_OK == drv_nic_pkt_tx(0, pPacket, rtl83xx_nic_tx_callback, (void *)ndev))
    if (RT_ERR_OK == drv_nic_pkt_tx(unit, pPacket, rtl83xx_nic_tx_callback, (void *)tx_buff))
    {
        //ndev->trans_start = jiffies;
    } else {
        PKTBUF_FREE(pPacket);

        //priv->ndev_stats.tx_errors++;
        //dev_kfree_skb(skb);
        //netif_start_queue(ndev);

        osal_free(tx_buff);

        return RT_ERR_FAILED;   /* Failed */
    }
#endif
    return RT_ERR_OK;   /* Success */
}

int32
nic_case13_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    uint8 pkt1[] = { /* ARP: Who has 192.168.154.80? Tell 192.168.154.89 */
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x52, 0x45, 0x41, 0x4c, 0x30, 0x59, 0x08, 0x06, 0x00, 0x01,
        0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x52, 0x45, 0x41, 0x4c, 0x30, 0x59, 0xc0, 0xa8, 0x9a, 0x59,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x9a, 0x50, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
        };
    //struct rtl83xx_priv *priv = netdev_priv(ndev);
    uint8 *tx_buff;
    drv_nic_pkt_t *pPacket;

    RT_LOG(LOG_WARNING, MOD_UNITTEST, "=================== Tx one packet ===================\n");

    /* stopping send other packet */
    //netif_stop_queue(ndev);

    pPacket = PKTBUF_ALLOC(sizeof(drv_nic_pkt_t));
    if (pPacket == NULL)
    {
        /* out of memory */
        return RT_ERR_FAILED;   /* Failed */
    }

    tx_buff = osal_alloc(1600);
    if (tx_buff == NULL)
    {
        PKTBUF_FREE(pPacket);
        return RT_ERR_FAILED;
    }

//    if (CCL_P(skb)->wSize > 0)
    {
        pPacket->as_txtag = 1;

        pPacket->tx_tag.dst_port_mask       = (0x1 << 24);
        pPacket->tx_tag.fwd_type            = NIC_FWD_TYPE_LOGICAL;
        pPacket->tx_tag.as_priority         = 1;
        pPacket->tx_tag.priority            = 7;
        pPacket->tx_tag.l2_learning         = 0;
    }
//    else
//    {
//        pPacket->as_txtag = 0;
//    }

    /* raw packet */
    pPacket->buf_id = (void *)NULL;
    pPacket->head = tx_buff;
    pPacket->data = tx_buff+8;
    pPacket->tail = tx_buff+8+64;
    pPacket->end = tx_buff+1600;
    pPacket->length = 60;
    pPacket->next = NULL;

    memcpy(pPacket->data, pkt1, 60);

    if (gDBG_Dump_Tx)
    {
        int i;
        int dump_len = 256;
        char buff[dump_len], temp_buff[16];

        osal_memset(buff, 0, sizeof(buff));\
        osal_memset(temp_buff, 0, sizeof(temp_buff));
        RT_LOG(LOG_WARNING, MOD_UNITTEST, "=== [TX] ================================================== Len: %d", pPacket->length);
        for (i=0; i<dump_len; i++) {
            if (i == (pPacket->length)) break;
            if (i % 16 == 0)
            {
                sprintf(buff, "[%04X] ", i);
                //RT_LOG(LOG_WARNING, MOD_UNITTEST, "[%04X] ", i);
            }
            sprintf(temp_buff, "%02X ", *(pPacket->data + i));
            strcat(buff, temp_buff);
            //RT_LOG(LOG_WARNING, MOD_UNITTEST, "%02X ", *(pPacket->data + i));
            if (i % 16 == 15)
            {
                RT_LOG(LOG_WARNING, MOD_UNITTEST, "%s", buff);
                osal_memset(buff, 0, sizeof(buff));
            }
        }
        if ((i % 16) != 0)
            RT_LOG(LOG_WARNING, MOD_UNITTEST, "%s", buff);
    }

    //if (RT_ERR_OK == drv_nic_pkt_tx(0, pPacket, rtl83xx_nic_tx_callback, (void *)ndev))
    if (RT_ERR_OK == drv_nic_pkt_tx(unit, pPacket, rtl83xx_nic_tx_callback, (void *)tx_buff))
    {
        //ndev->trans_start = jiffies;
    } else {
        PKTBUF_FREE(pPacket);

        //priv->ndev_stats.tx_errors++;
        //dev_kfree_skb(skb);
        //netif_start_queue(ndev);

        osal_free(tx_buff);

        return RT_ERR_FAILED;   /* Failed */
    }
#endif
    return RT_ERR_OK;   /* Success */
}

int32
nic_case4_test(uint32 caseNo, uint32 unit)
{
    uint32 temp;

    ioal_mem32_read(unit, 0x3134, &temp);
    temp |= 0x40;
    ioal_mem32_write(unit, 0x3134, temp);
    return RT_ERR_OK;
}

int32
nic_case5_test(uint32 caseNo, uint32 unit)
{
    uint32 temp;

    ioal_mem32_read(unit, 0x3134, &temp);
    temp &= ~(0x40);
    ioal_mem32_write(unit, 0x3134, temp);
    return RT_ERR_OK;
}

int32
nic_case6_test(uint32 caseNo, uint32 unit)
{
    drv_nic_dbg_set(unit, DEBUG_TX_RAW_LEN_BIT | DEBUG_TX_CPU_TAG_BIT);
    return RT_ERR_OK;
}

int32
nic_case7_test(uint32 caseNo, uint32 unit)
{
    drv_nic_dbg_set(unit, DEBUG_RX_RAW_LEN_BIT | DEBUG_RX_CPU_TAG_BIT);
    return RT_ERR_OK;
}

int32
nic_case8_test(uint32 caseNo, uint32 unit)
{
    drv_nic_dbg_set(unit, DEBUG_TX_RAW_LEN_BIT | DEBUG_TX_CPU_TAG_BIT | DEBUG_RX_RAW_LEN_BIT | DEBUG_RX_CPU_TAG_BIT);
    return RT_ERR_OK;
}

int32
nic_case9_test(uint32 caseNo, uint32 unit)
{
    drv_nic_cntr_dump(unit);
    return RT_ERR_OK;
}

int32
nic_case10_test(uint32 caseNo, uint32 unit)
{
    drv_nic_ringbuf_dump(unit);
    return RT_ERR_OK;
}

int32
nic_case11_test(uint32 caseNo, uint32 unit)
{
    drv_nic_dbg_set(unit, 0);
    return RT_ERR_OK;
}

int32
nic_case12_test(uint32 caseNo, uint32 unit)
{
    drv_nic_cntr_clear(unit);
    return RT_ERR_OK;
}

int32
nic_pausePktPerformance_test(uint32 caseNo, uint32 unit)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
    uint32  i, j, value;
    uint8 pkt1[] = { /* Pause frame */
        0x01, 0x80, 0xc2, 0x00, 0x00, 0x01, 0x00, 0x1A, 0x1B, 0x33, 0x44, 0x55, 0x88, 0x08, 0x00, 0x01,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
    uint8 *tx_buff;
    drv_nic_pkt_t *pPacket;
    osal_usecs_t    usec_before = 0, usec_after = 0;
    int32   ret = RT_ERR_FAILED;

    ret = osal_time_usecs_get(&usec_before);
    value = 1000;
    for (j = 0; j < 1000; j++)
    {
        for (i = 0; i < value; i++)
        {
            pPacket = PKTBUF_ALLOC(sizeof(drv_nic_pkt_t));
            if (pPacket == NULL)
            {
                /* out of memory */
                return RT_ERR_FAILED;   /* Failed */
            }

            tx_buff = osal_alloc(1600);
            if (tx_buff == NULL)
            {
                PKTBUF_FREE(pPacket);
                return RT_ERR_FAILED;
            }

            {
                pPacket->as_txtag = 1;

                pPacket->tx_tag.dst_port_mask       = (0x1 << 2);
                pPacket->tx_tag.fwd_type            = NIC_FWD_TYPE_LOGICAL;
                pPacket->tx_tag.as_priority         = 1;
                pPacket->tx_tag.priority            = 7;
                pPacket->tx_tag.l2_learning         = 0;
            }

            /* raw packet */
            pPacket->buf_id = (void *)NULL;
            pPacket->head = tx_buff;
            pPacket->data = tx_buff+8;
            pPacket->tail = tx_buff+8+64;
            pPacket->end = tx_buff+1600;
            pPacket->length = 60;
            pPacket->next = NULL;

            memcpy(pPacket->data, pkt1, 60);

            if (RT_ERR_OK == drv_nic_pkt_tx(unit, pPacket, rtl83xx_nic_tx_callback, (void *)tx_buff))
            {

            }
            else
            {
                PKTBUF_FREE(pPacket);
                osal_free(tx_buff);
                return RT_ERR_FAILED;   /* Failed */
            }
        }
    }
    ret = osal_time_usecs_get(&usec_after);
    osal_printf("Before usecond = %u\n", usec_before);
    osal_printf("After usecond = %u\n", usec_after);
    osal_printf("Send Pause frame Number: %u\n", value);
    osal_printf("Send one pause frame nanosecond (drv_nic_pkt_tx) = %u\n", (usec_after-usec_before)/value);
#endif

    return RT_ERR_OK;   /* Success */
}




int32
nic_8380_case0_test(uint32 caseNo, uint32 unit)
{
    uint8 pkt1[] =
          { /* ARP: Who has 192.168.154.80? Tell 192.168.154.89 */
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x52, 0x45, 0x41, 0x4c, 0x30, 0x59, 0x08, 0x06, 0x00, 0x01,
        0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x52, 0x45, 0x41, 0x4c, 0x30, 0x59, 0xc0, 0xa8, 0x9a, 0x59,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x9a, 0x50, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
        };
    uint8 *tx_buff;
    drv_nic_pkt_t *pPacket;

    RT_LOG(LOG_WARNING, MOD_UNITTEST, "=================== Tx one packet ===================\n");
    pPacket = PKTBUF_ALLOC(sizeof(drv_nic_pkt_t));
    if (pPacket == NULL)
    {
        /* out of memory */
        return RT_ERR_FAILED;   /* Failed */
    }

   osal_printf("Allocate drv_nic_pkt_t OK!\n");

    tx_buff = osal_alloc(1600);
    if (tx_buff == NULL)
    {
        PKTBUF_FREE(pPacket);
        return RT_ERR_FAILED;
    }

   osal_printf("Allocate pkt OK!\n");

    pPacket->as_txtag = 1;
    pPacket->tx_tag.fwd_type            = NIC_FWD_TYPE_LOGICAL;
    pPacket->tx_tag.dst_port_mask       = (0x1 << 24);
    pPacket->tx_tag.as_priority         = 1;
    pPacket->tx_tag.priority            = 7;

    /* raw packet */
    pPacket->buf_id = (void *)NULL;
    pPacket->head = tx_buff;
    pPacket->data = tx_buff+8;
    pPacket->tail = tx_buff+8+64;
    pPacket->end = tx_buff+1600;
    pPacket->length = 60;
    pPacket->next = NULL;

    memcpy(pPacket->data, pkt1, 60);

   osal_printf("Copy pkt OK!\n");

    if (RT_ERR_OK == drv_nic_pkt_tx(unit, pPacket, rtl83xx_nic_tx_callback, (void *)tx_buff))
    {

    }
    else
    {
        PKTBUF_FREE(pPacket);
        osal_free(tx_buff);
        return RT_ERR_FAILED;   /* Failed */
    }

   osal_printf("Send pkt OK!\n");

    return RT_ERR_OK;   /* Success */
}
