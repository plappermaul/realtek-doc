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
 * $Revision: 97104 $
 * $Date: 2019-05-22 18:57:23 +0800 (Wed, 22 May 2019) $
 *
 * Purpose : user main function for Linux
 *
 * Feature : 1) Init RTK Layer API
 *           2) Create SDK Main Thread
 *           3) Invoke SDK Diag Shell
 */

#define LINUX_NIC_USER_MODE_SAMPLE

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <ioal/ioal_init.h>
#include <drv/spi/spi.h>
#include <drv/nic/nic.h>
#include <private/drv/intr/intr.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/print.h>
#include <osal/sem.h>
#include <hwp/hw_profile.h>
#include <common/rtcore/rtcore_init.h>
#include <rtk/init.h>
#include <rtk/l2.h>
#include <drv/l2ntfy/l2ntfy.h>


#if defined(LINUX_NIC_USER_MODE_SAMPLE)
#include <stdlib.h>
#include <rtcore/user/rtcore_drv_usr.h>
#endif
#include <osal/time.h>



/*
 * Symbol Definition
 */
#define NML_BUFF_SIZE         1632
#define BUFF_RSVD_SIZE        (16 + 2)

/*
 * Data Declaration
 */
#if defined(LINUX_NIC_USER_MODE_SAMPLE)
static uint32 pkt_buf_vir_addres;
static uint32 pkt_buf_stat[QUEUE_MAX_SIZE];
static uint32 pkt_buf_type[QUEUE_MAX_SIZE];
static uint32 jumbo_pkt_buf_stat[JUMBO_QUEUE_MAX_SIZE];
#endif
extern uint32   usr_lb_sts[];

/*
 * Function Declaration
 */
#if defined(LINUX_NIC_USER_MODE_SAMPLE)
int32 rtnic_pkt_buf_init(uint32 unit);
static int32 rtnic_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket);
static int32 rtnic_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket);
drv_nic_rx_t rtnic_rx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie);
void rtnic_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie);
static int32 rtnic_init(void);
#endif
#if defined(CONFIG_SDK_APP_DIAG)
extern int diag_main(int argc, char** argv);
#endif

static osal_mutex_t         pkt_sem;
#define PKT_SEM_LOCK()    \
do {\
    if (osal_sem_mutex_take(pkt_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        osal_printf("semaphore lock failed\n");\
    }\
} while(0)
#define PKT_SEM_UNLOCK()   \
do {\
    if (osal_sem_mutex_give(pkt_sem) != RT_ERR_OK)\
    {\
         osal_printf("semaphore unlock failed\n");\
    }\
} while(0)

int32 app_notifyHandler_example(uint32 unit, rtk_l2ntfy_usrEventArray_t *pEventCollectArray)
{
#ifdef CONFIG_SDK_SAMPLE
    uint32  num, i;
    rtk_l2ntfy_eventEntry_t *tmpPtr = 0;

    num = pEventCollectArray->entryNum;
    for (i = 0; i < num; i++)
    {
        tmpPtr = &pEventCollectArray->eventArray[i];

        osal_printf("type:%d vid:%d mac:%02x:%02x:%02x:%02x:%02x:%02x  slp:%d\n", tmpPtr->type, tmpPtr->fidVid,
                                                                                    tmpPtr->mac.octet[0], tmpPtr->mac.octet[1],
                                                                                    tmpPtr->mac.octet[2], tmpPtr->mac.octet[3], tmpPtr->mac.octet[4], tmpPtr->mac.octet[5], tmpPtr->slp);
    }
#endif

    return 0;
}

int32 app_notifyRunoutHandler_example(uint32 unit, rtk_l2ntfy_runOutType_t type)
{
    printf("%s():%d  runout type:%d\n", __FUNCTION__, __LINE__, type);
    return 0;
}



/* Function Name:
 *      sdk_main
 * Description:
 *      sdk main function
 * Input:
 *      data - may hold pointer or word
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
void sdk_main(void)
{
#if defined(CONFIG_SDK_APP_DIAG)

    /* Run diag shell */
    osal_printf("Start to run Diag Shell....\n\n");
    diag_main(0, NULL);
#else

    osal_printf("Please add your application here....\n\n");
#endif
} /* end of sdk_main */

/* Function Name:
 *      main
 * Description:
 *      the normal application entry point
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None.
 */
int main(int argc, char** argv)
{
    int32  ret;

    /* Init RTK Layer API */
    RT_INIT_MSG("RTK User Space SDK Initialize\n");

    if ((ret = rtcore_init()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "rtcore_init failed!!\n");
        return ret;
    }

    if(HWP_CPU_EMBEDDED())
        drv_intr_init(HWP_MY_UNIT_ID());

    if ((ret = rtk_init()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "rtk_init failed!!\n");
        return ret;
    }
    RT_LOG(LOG_EVENT, MOD_INIT, "rtk_init Completed!!\n");

    if ((ret = rtnic_init()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "rtnic_init failed!!\n");
        return ret;
    }
    RT_LOG(LOG_EVENT, MOD_INIT, "rtnic_init Completed!!\n");

    rtcore_usr_init();

#ifdef CONFIG_SDK_SAMPLE
    drv_l2ntfy_register(HWP_MY_UNIT_ID(), app_notifyHandler_example, app_notifyRunoutHandler_example);
#endif

    sdk_main();

    return RT_ERR_OK;
} /* end of main */

#if defined(LINUX_NIC_USER_MODE_SAMPLE)
/* Function Name:
 *      rtnic_pkt_buf_init
 * Description:
 *      packet allocation
 * Input:
 *      unit     - unit id
 *      size     - alloc size
 *      flags    - alloc flags
 * Output:
 *      ppPacket - pointer buffer to the allocated packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 rtnic_pkt_buf_init(uint32 unit)
{
    uint32  i;

    /* Get packet buffer start address that is mapped to user space */
    ioal_init_memRegion_get(unit, IOAL_MEM_DMA, &pkt_buf_vir_addres);

    /* Create mutex semaphore */
    pkt_sem = osal_sem_mutex_create();

    /* Init buffer state */
    osal_memset(pkt_buf_stat, 0, sizeof(pkt_buf_stat));
    osal_memset(pkt_buf_type, 0, sizeof(pkt_buf_type));
    for (i = QUEUE_MAX_SIZE - QUEUE_RX_POOL_SIZE - QUEUE_TX_POOL_SIZE; i < QUEUE_MAX_SIZE; i++)
    {
        if (i < QUEUE_MAX_SIZE - QUEUE_TX_POOL_SIZE)
            pkt_buf_type[i] = PKTBUF_RX;
        else
            pkt_buf_type[i] = PKTBUF_TX;
    }

    osal_memset(jumbo_pkt_buf_stat, 0, sizeof(jumbo_pkt_buf_stat));

    return RT_ERR_OK;
}

/* Function Name:
 *      rtnic_pkt_alloc
 * Description:
 *      packet allocation
 * Input:
 *      unit     - unit id
 *      size     - alloc size
 *      flags    - alloc flags
 * Output:
 *      ppPacket - pointer buffer to the allocated packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 rtnic_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket)
{
    const uint32    dPool = QUEUE_MAX_SIZE - QUEUE_RX_POOL_SIZE - QUEUE_TX_POOL_SIZE;  //dedicate pool for RX/TX
    uint32 i;
    uint8 *pBuf;
    drv_nic_pkt_t *pPacket;

    PKT_SEM_LOCK();
    pPacket = osal_alloc(sizeof(drv_nic_pkt_t));
    if (pPacket == NULL)
    {
        osal_printf("rtnic_pkt_alloc: malloc fail\n");
        PKT_SEM_UNLOCK();
        return RT_ERR_MEM_ALLOC;
    }

    if (size > NML_BUFF_SIZE)
    {
        for(i = 0; i < JUMBO_QUEUE_MAX_SIZE; i++)
        {
            if(FALSE == jumbo_pkt_buf_stat[i])
            {
                jumbo_pkt_buf_stat[i] = TRUE;
                break;
            }
        }

        if (JUMBO_QUEUE_MAX_SIZE == i)
        {
            osal_printf("rtnic_pkt_alloc: run out of memory for jumbo\n");
            goto _alloc_fail_exit;
        }
        pBuf = (uint8 *)(pkt_buf_vir_addres + (NML_BUFF_SIZE * QUEUE_MAX_SIZE) + (JUMBO_BUFF_SIZE * i));
    }
    else
    {
        for(i = 0; i < QUEUE_MAX_SIZE; i++)
        {
            if(FALSE == pkt_buf_stat[i])
            {
                if (i >= dPool)
                {
                    if (pkt_buf_type[i] == (flags & 0x1))
                    {
                        pkt_buf_stat[i] = TRUE;
                        break;
                    }
                }
                else
                {
                    pkt_buf_stat[i] = TRUE;
                    break;
                }
            }
        }

        if (QUEUE_MAX_SIZE == i)
        {
            osal_printf("rtnic_pkt_alloc: run out of memory\n");
            goto _alloc_fail_exit;
        }
        pBuf = (uint8 *)(pkt_buf_vir_addres + (size * i));
    }


    osal_memset(pPacket, 0, sizeof(drv_nic_pkt_t));
    pPacket->head = (uint8 *)(pBuf);
    pPacket->data = (pPacket->head + BUFF_RSVD_SIZE);
    pPacket->tail = (pBuf + size);  /* No reserve any tail space */
    pPacket->end = pPacket->tail;
    pPacket->length = 0;
    pPacket->buf_id = (void *)i;
    pPacket->next = NULL;

    *ppPacket = pPacket;
    PKT_SEM_UNLOCK();
    return RT_ERR_OK;

_alloc_fail_exit:
    osal_free(pPacket);
    PKT_SEM_UNLOCK();
    return RT_ERR_FAILED;
} /* end of rtnic_pkt_alloc */

/* Function Name:
 *      rtnic_pkt_free
 * Description:
 *      free allocated packet
 * Input:
 *      unit    - unit id
 *      pPacket - pointer buffer to the packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 rtnic_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket)
{
    uint32 id;

    if (NULL == pPacket)
    {
        osal_printf("Error: pPacket is NULL at %s():%d\n", __FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }

    id = (uint32)pPacket->buf_id;

    PKT_SEM_LOCK();

    if (FLAG_JUMBO_PKT & id)
    {
        if (FALSE == jumbo_pkt_buf_stat[id])
        {
            osal_printf("Error: buffer for jumbo is NULL at %s():%d\n", __FUNCTION__, __LINE__);
            PKT_SEM_UNLOCK();
            return RT_ERR_FAILED;
        }
    }
    else
    {
        if (FALSE == pkt_buf_stat[id])
        {
            osal_printf("Error: skb is NULL at %s():%d\n", __FUNCTION__, __LINE__);
            PKT_SEM_UNLOCK();
            return RT_ERR_FAILED;
        }
    }

    if (FLAG_JUMBO_PKT & id)
        jumbo_pkt_buf_stat[id] = FALSE;
    else
        pkt_buf_stat[id] = FALSE;

    osal_free(pPacket);

    PKT_SEM_UNLOCK();
    return RT_ERR_OK;
} /* end of rtnic_pkt_free */

drv_nic_rx_t rtnic_rx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    int32  retval;
    drv_nic_pkt_t *pTx_packet;

    if (NULL == pPacket)
    {
        osal_printf("Error: pPacket is NULL at %s():%d\n", __FUNCTION__, __LINE__);
        goto _exit;
    }

    if (usr_lb_sts[unit])
    {
        if (FLAG_JUMBO_PKT & (uint32)pPacket->buf_id)
        {
            if ((retval = rtnic_pkt_alloc(unit, JUMBO_BUFF_SIZE, PKTBUF_TX, &pTx_packet)) != RT_ERR_OK)
            {
                osal_printf("Error: Can't allocate TX packet buffer %s():%d\n", __FUNCTION__, __LINE__);
                goto _exit;
            }
            pTx_packet->buf_id = (void *)((uint32)pTx_packet->buf_id | FLAG_JUMBO_PKT);
        }
        else
        {
            if ((retval = rtnic_pkt_alloc(unit, NML_BUFF_SIZE, PKTBUF_TX, &pTx_packet)) != RT_ERR_OK)
            {
                osal_printf("Error: Can't allocate TX packet buffer %s():%d\n", __FUNCTION__, __LINE__);
                goto _exit;
            }
        }


        osal_memcpy(pTx_packet->data, pPacket->data, pPacket->length);
        pTx_packet->tail = pTx_packet->data + pPacket->length;
        pTx_packet->length = pPacket->length;

        if ((retval = drv_nic_pkt_tx(unit, pTx_packet, rtnic_tx_callback, (void *)NULL)) != RT_ERR_OK)
        {
            osal_printf("Error: drv_nic_pkt_tx TX packet failed %s():%d\n", __FUNCTION__, __LINE__);
            osal_printf("Directly free pTx_packet\n");
            if ((retval = rtnic_pkt_free(unit, pTx_packet)) != RT_ERR_OK)
            {
                osal_printf("Error: Can't free TX packet buffer %s():%d\n", __FUNCTION__, __LINE__);
                goto _exit;
            }
            goto _exit;
        }
    }

    if ((retval = rtnic_pkt_free(unit, pPacket)) != RT_ERR_OK)
    {
        osal_printf("Error: Can't free RX packet buffer %s():%d\n", __FUNCTION__, __LINE__);
        goto _exit;
    }

    return NIC_RX_HANDLED_OWNED;

_exit:
    return NIC_RX_NOT_HANDLED;
} /* end of rtnic_rx_callback */


void rtnic_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    if (NULL == pPacket)
    {
        osal_printf("Error: pPacket is NULL at %s():%d\n", __FUNCTION__, __LINE__);
        return;
    }

    rtnic_pkt_free(unit, pPacket);

    return;
} /* end of rtnic_tx_callback */

static int32 rtnic_init(void)
{
    int32  ret;
    drv_nic_initCfg_t initcfg;
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
    uint32  local_unit;
    rtk_l2ntfy_dst_t  now_dst;
#endif

    initcfg.pkt_alloc = (drv_nic_pkt_alloc_f)rtnic_pkt_alloc;
    initcfg.pkt_free = (drv_nic_pkt_free_f)rtnic_pkt_free;
    initcfg.pkt_size = NML_BUFF_SIZE - BUFF_RSVD_SIZE;

    rtnic_pkt_buf_init(HWP_MY_UNIT_ID());
    RT_ERR_CHK(nic_probe(HWP_MY_UNIT_ID()), ret);
    drv_nic_init(HWP_MY_UNIT_ID(), &initcfg);

    if(HWP_NIC_SUPPORT(HWP_MY_UNIT_ID()))
        rtcore_usr_intr_attach(HWP_MY_UNIT_ID(), NULL, NULL, INTR_TYPE_NIC);

    drv_nic_rx_register(HWP_MY_UNIT_ID(), 1, rtnic_rx_callback, NULL, 0);
    drv_nic_rx_start(HWP_MY_UNIT_ID());

#if defined(CONFIG_SDK_DRIVER_L2NTFY)
    HWP_UNIT_TRAVS_LOCAL(local_unit)
    {
        if ((ret = l2ntfy_probe(local_unit)) == RT_ERR_OK)
        {
            RT_ERR_CHK_EHDL(drv_l2ntfy_init(local_unit), ret,
                    {RT_INIT_ERR(ret, (MOD_INIT), "unit %u drv_l2ntfy_init fail %d!\n", local_unit, ret);});
        }
    }

    drv_l2ntfy_dst_get(HWP_MY_UNIT_ID(), &now_dst);
    if((L2NTFY_DST_PKT_TO_LOCAL == now_dst)
        || (L2NTFY_DST_PKT_TO_MASTER == now_dst))
    {
        /* Register L2 notification Rx Handler */
        if (RT_ERR_OK != drv_nic_rx_register(HWP_MY_UNIT_ID(), L2NTFY_RX_HANDLER_PRIORITY, drv_l2ntfy_pkt_handler, NULL, 0))
        {
          RT_INIT_MSG("#cause: L2 notification Handler Registration Failed!!\n");
        }
    }
#endif

    return RT_ERR_OK;
}
#endif
