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
 * $Revision: 104516 $
 * $Date: 2020-02-26 11:30:04 +0800 (Wed, 26 Feb 2020) $
 *
 * Purpose : user main function for Linux
 *
 * Feature : 1) Init RTK Layer API
 *           2) Create SDK Main Thread
 *           3) Invoke SDK Diag Shell
 */


/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <ioal/ioal_init.h>
#include <drv/spi/spi.h>
#include <drv/nic/nic.h>
#include <osal/atomic.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/print.h>
#include <osal/sem.h>
#include <hwp/hw_profile.h>
#include <common/rtcore/rtcore_init.h>
#ifndef PHY_ONLY
  #include <rtk/init.h>
#else
  #include <phy_init.h>
#endif
#include <rtk/l2.h>
#include <rtk/mirror.h>
#include <rtk/vlan.h>
#include <drv/l2ntfy/l2ntfy.h>
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
#include <private/drv/nic_pci/nic_pci_9310.h>
#endif
#include <rtcore/user/rtcore_drv_usr.h>
#include <osal/time.h>
#include <rtk/switch.h>
#include <ioal/mem32.h>
/*
 * Symbol Definition
 */
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
#define NML_BUFF_SIZE         (NML_FRAME_SIZE_MAX + BUFF_RSVD_SIZE + BUFF_TAIL_SPACE)
#define BUFF_RSVD_SIZE        (NIC_9310_CPU_TAG_SIZE + 2)
#define BUFF_TAIL_SPACE       26
#define JUMBO_BUFF_SIZE       (JUMBO_FRAME_SIZE_MAX + BUFF_RSVD_SIZE + BUFF_TAIL_SPACE)
#else /* CONFIG_SDK_DRIVER_EXTC_NIC */
#define NML_BUFF_SIZE         (NML_FRAME_SIZE_MAX + BUFF_RSVD_SIZE + BUFF_TAIL_SPACE)
#define BUFF_RSVD_SIZE        (16 + 2)
#define BUFF_TAIL_SPACE       2
#define JUMBO_BUFF_SIZE       (JUMBO_FRAME_SIZE_MAX + BUFF_RSVD_SIZE + BUFF_TAIL_SPACE)
#endif/* CONFIG_SDK_DRIVER_EXTC_NIC */


/*
 * Data Declaration
 */
void    *ioaddr;
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
uintptr dma_vaddr;
uintptr dma_phy_addr;
uintptr tally_vaddr;
uintptr tally_phy_addr;
uintptr tx_desc_vaddr;
uintptr tx_desc_phy_addr;
uintptr rx_desc_vaddr;
uintptr rx_desc_phy_addr;
uintptr pkt_buf_vir_addres;
uintptr pkt_phy_addr;
static uint32 pkt_buf_stat[QUEUE_POOL_MAX_SIZE];
static uint32 jumbo_pkt_buf_stat[JUMBO_QUEUE_POOL_MAX_SIZE];

extern uint32 nml_rx_alloc_cnt;
extern uint32 nml_tx_alloc_cnt;
extern uint32 nml_free_cnt;
extern uint32 jumbo_rx_alloc_cnt;
extern uint32 jumbo_tx_alloc_cnt;
extern uint32 jumbo_free_cnt;
extern uint32 free_fail_cnt;
#elif defined(CONFIG_SDK_DRIVER_NIC)
static uint32 pkt_buf_vir_addres;
static uint32 pkt_buf_stat[QUEUE_POOL_MAX_SIZE];
static uint32 pkt_buf_type[QUEUE_POOL_MAX_SIZE];
static uint32 jumbo_pkt_buf_stat[JUMBO_QUEUE_POOL_MAX_SIZE];

extern uint32   usr_lb_sts[];
#endif/* CONFIG_SDK_DRIVER_EXTC_NIC */



#ifdef PHY_ONLY
/*
 * This table is used to describe your hardware board design, especially for mapping relation between port and phy.
 * Port related information
 * .mac_id      = port id.
 * .phy_idex    = used to indicate which PHY entry is used by this port in glued_phy_Descp[].
 * .eth         = Ethernet speed type (refer to rt_port_ethType_t).
 * .medi        = Port media type (refer to rt_port_medium_t).
 */
phy_hwp_portDescp_t  my_port_descp[] = {
        { .mac_id =  0,  .phy_idx = 0, .eth = HWP_XGE,   .medi = HWP_COPPER,},
        { .mac_id =  1,  .phy_idx = 0, .eth = HWP_XGE,   .medi = HWP_COPPER,},
        { .mac_id =  2,  .phy_idx = 0, .eth = HWP_XGE,   .medi = HWP_COPPER,},
        { .mac_id =  3,  .phy_idx = 0, .eth = HWP_XGE,   .medi = HWP_COPPER,},
        { .mac_id =  4,  .phy_idx = 1, .eth = HWP_XGE,   .medi = HWP_COPPER,},
        { .mac_id =  5,  .phy_idx = 1, .eth = HWP_XGE,   .medi = HWP_COPPER,},
        { .mac_id =  6,  .phy_idx = 1, .eth = HWP_XGE,   .medi = HWP_COPPER,},
        { .mac_id =  7,  .phy_idx = 1, .eth = HWP_XGE,   .medi = HWP_COPPER,},
        { .mac_id =  8,  .phy_idx = 2, .eth = HWP_XGE,   .medi = HWP_COPPER,},
        { .mac_id =  9,  .phy_idx = 3, .eth = HWP_XGE,   .medi = HWP_COPPER,},
        { .mac_id = HWP_END },
    };

/*
 * PHY related information
 * .chip        = PHY Chip model (refer to phy_type_t).
 * .mac_id      = The first port id of this PHY. For example, the 8218D is connected to
 *                port 0 ~ 7, then the .mac_id  = 0.
 * .phy_max     = The MAX port number of this PHY. For examplem the 8218D is an octet PHY,
 *                so this number is 8.
 */

phy_hwp_phyDescp_t     my_phy_Descp[] = {
        [0] = { .chip = RTK_PHYTYPE_RTL8264, .mac_id = 0, .phy_max = 4 },
        [1] = { .chip = RTK_PHYTYPE_RTL8264, .mac_id = 4, .phy_max = 4 },
        [2] = { .chip = RTK_PHYTYPE_RTL8261, .mac_id = 8, .phy_max = 1 },
        [3] = { .chip = RTK_PHYTYPE_RTL8261, .mac_id = 9, .phy_max = 1 },
        [4] = { .chip = HWP_END },
    };
#endif


/*
 * Macro Definition
 */
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC)
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
#endif


/*
 * Function Declaration
 */
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC)
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


int32 app_notifyHandler_example(uint32 unit, rtk_l2ntfy_usrEventArray_t *pEventCollectArray)
{
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
    uint32  num, i;
    rtk_l2ntfy_eventEntry_t *tmpPtr = 0;
    rtk_enable_t    vlan_agg_ebl;

    rtk_vlan_aggrEnable_get(unit, &vlan_agg_ebl);

    num = pEventCollectArray->entryNum;
    for (i = 0; i < num; i++)
    {
        tmpPtr = &pEventCollectArray->eventArray[i];

        if (tmpPtr->type == L2NTFY_TYPE_EVENT_LOST || tmpPtr->type == L2NTFY_TYPE_LOCAL_BUF_RUNOUT)
        {
            osal_printf("%s():%d  event lost, restore learn\n", __FUNCTION__, __LINE__);
#if 0 /* TODO_sync37x: need to implement l2ntfy */
            drv_l2ntfy_l2LearningAge_restore(0, ARB_NTFY_0);
#endif
        }
        else if (tmpPtr->l2Tnl)
        {
            osal_printf("type:%s vid:%d mac:%02x:%02x:%02x:%02x:%02x:%02x  is_trk:%d devId:%d spa:%d  sablk:%d  dablk:%d  sttc:%d  nexthop:%d  sus:%d  l2_tnl_idx:%d\n",
                                                                                        tmpPtr->type == 1 ? "Add" : tmpPtr->type == 0 ? "Del" : "Update",
                                                                                        tmpPtr->fidVid,
                                                                                        tmpPtr->mac.octet[0], tmpPtr->mac.octet[1], tmpPtr->mac.octet[2],
                                                                                        tmpPtr->mac.octet[3], tmpPtr->mac.octet[4], tmpPtr->mac.octet[5],
                                                                                        tmpPtr->is_trk, tmpPtr->slp >> 6, tmpPtr->slp & 0x3f, tmpPtr->sablk,
                                                                                        tmpPtr->dablk, tmpPtr->sttc, tmpPtr->nexthop, tmpPtr->sus, tmpPtr->l2Tnl_idx);
        }
        else
        {
            if (vlan_agg_ebl)
            {
                osal_printf("type:%s vid:%d mac:%02x:%02x:%02x:%02x:%02x:%02x  is_trk:%d devId:%d spa:%d  sablk:%d  dablk:%d  sttc:%d  nexthop:%d  sus:%d  tagSts:%d  aPri:%d  aVid:%d\n",
                                                                                        tmpPtr->type == 1 ? "Add" : tmpPtr->type == 0 ? "Del" : "Update",
                                                                                        tmpPtr->fidVid,
                                                                                        tmpPtr->mac.octet[0], tmpPtr->mac.octet[1], tmpPtr->mac.octet[2],
                                                                                        tmpPtr->mac.octet[3], tmpPtr->mac.octet[4], tmpPtr->mac.octet[5],
                                                                                        tmpPtr->is_trk, tmpPtr->slp >> 6, tmpPtr->slp & 0x3f, tmpPtr->sablk,
                                                                                        tmpPtr->dablk, tmpPtr->sttc, tmpPtr->nexthop, tmpPtr->sus,
                                                                                        tmpPtr->tagSts, tmpPtr->agg_pri, tmpPtr->agg_vid);
            }
            else
            {
                osal_printf("type:%s vid:%d mac:%02x:%02x:%02x:%02x:%02x:%02x  is_trk:%d devId:%d spa:%d  sablk:%d  dablk:%d  sttc:%d  nexthop:%d  sus:%d  ecid:%d  aVid:%d\n",
                                                                                        tmpPtr->type == 1 ? "Add" : tmpPtr->type == 0 ? "Del" : "Update",
                                                                                        tmpPtr->fidVid,
                                                                                        tmpPtr->mac.octet[0], tmpPtr->mac.octet[1], tmpPtr->mac.octet[2],
                                                                                        tmpPtr->mac.octet[3], tmpPtr->mac.octet[4], tmpPtr->mac.octet[5],
                                                                                        tmpPtr->is_trk, tmpPtr->slp >> 6, tmpPtr->slp & 0x3f, tmpPtr->sablk,
                                                                                        tmpPtr->dablk, tmpPtr->sttc, tmpPtr->nexthop, tmpPtr->sus,
                                                                                        tmpPtr->ecid, tmpPtr->agg_vid);
            }
        }
    }

    return 0;
#else
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
    return 0;
#endif
}

int32 app_notifyRunoutHandler_example(uint32 unit, rtk_l2ntfy_runOutType_t type)
{
    printf("%s():%d  runout type:%d\n", __FUNCTION__, __LINE__, type);
    return 0;
}

#if defined(CONFIG_SFLOW_PG_THREAD)
int32 app_sflowStsChg_example(uint32 unit, rtk_sflow_stsChg_t type)
{
    if (type == SFLOW_DISABLE)
        printf("%s():%d  sflow disable\n", __FUNCTION__, __LINE__);
    else
        printf("%s():%d  sflow restore\n", __FUNCTION__, __LINE__);

    return 0;
}
#endif



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

    int32      ret   = RT_ERR_OK;

    /* Init RTK Layer API */
    RT_INIT_MSG("RTK User Space SDK Initialize\n");


#ifndef PHY_ONLY
    if ((ret = rtcore_init()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "rtcore_init failed!!\n");
        return ret;
    }

    if ((ret = rtk_init()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "rtk_init failed!!\n");
        return ret;
    }
#else
    {
        rtk_phy_initInfo_t initInfo;

        /* init */
        initInfo.port_desc = my_port_descp;
        initInfo.phy_desc = my_phy_Descp;
        if ((ret = rtk_init(&initInfo)) != RT_ERR_OK)
        {
            RT_INIT_ERR(ret, MOD_INIT, "rtk_init failed!!\n");
            return ret;
        }
    }
#endif
    RT_LOG(LOG_EVENT, MOD_INIT, "rtk_init Completed!!\n");

#if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC)
    if ((ret = rtnic_init()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "rtnic_init failed!!\n");
        return ret;
    }
    RT_LOG(LOG_EVENT, MOD_INIT, "rtnic_init Completed!!\n");
#endif

    rtcore_usr_init();

#if defined(CONFIG_SDK_DRIVER_EXTC_NIC) && defined(CONFIG_SDK_DRIVER_L2NTFY)
    if ((ret = l2ntfy_probe(HWP_MY_UNIT_ID())) == RT_ERR_OK)
    {
        RT_ERR_CHK_EHDL(drv_l2ntfy_init(HWP_MY_UNIT_ID()), ret,
                {RT_INIT_ERR(ret, (MOD_INIT), "unit %u drv_l2ntfy_init fail %#x!\n", HWP_MY_UNIT_ID(), ret);});
    }
    RT_LOG(LOG_EVENT, MOD_INIT, "drv_l2ntfy_init Completed!!\n");
#endif

#if (defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC)) && defined(CONFIG_SDK_DRIVER_L2NTFY)
    drv_l2ntfy_register(HWP_MY_UNIT_ID(), app_notifyHandler_example, app_notifyRunoutHandler_example);
#endif

#if defined(CONFIG_SFLOW_PG_THREAD)
    rtk_mirror_sflowStsChg_register(HWP_MY_UNIT_ID(), app_sflowStsChg_example);
#endif

#if defined(CONFIG_SDK_RTL9300)
    /*CAMEOTAG:Added by Jincai on 2018.5.7,fix the bug that flow control is not workable on RTL930X for DIAG,
     *Involve the solution provided by Realtek to fix the issue!!
     *
     *- MAC_GLB_CTRL, REGISTER ADDRESS : 0xBB00C700 => BIT[26:25] = 0
     *BIT[26:25]: LIMIT_PAUSE_EN Enable limited TX pause packet number. After switch continuous transmit 128 pause frames in the
     *same port, switch will stop to transmit pause frame.
     *However, after the congestion lifting, re-enter congestion state, switch will continue to send pause frame.
     *0b0: disable, 0b1: 128 pause enable, 0b2: 32 pause enable, 0b3: reserve.
     *
     *- SC_P_EN, BASE ADDRESS : 0xBB003284 => BIT[3:0] = 0
     *PORT INDEX : 0 - 28
     *PORT OFFSET : 0x40
     *BIT[3:0] CNGST_SUST_TMR_LMT Congest sustain timer limited, unit in seconds.
     *If CNGST_TMR>=CNGST_SUST_TMR_LMT,this port will enter Special Congest State.
     *When CNGST_SUST_TMR_LMT!=0, special congest function is enabled.     
     */
    uint32     value = 0;
    uint32     reg   = 0;
    rtk_port_t port  = 0;
    rtk_switch_devInfo_t devInfo;
    osal_memset(&devInfo, 0, sizeof(devInfo));
    if ((ret = rtk_switch_deviceInfo_get(0, &devInfo)) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "Error: get switch device info failed!\n");
        return ret;
    }
    
    ioal_mem32_read(0, 0xC700, &value);
    value = value & 0xF9FFFFFF;
    ioal_mem32_write(0, 0xC700, value);
    
    reg = 0x3284;
    for (port = 0; port < RTK_MAX_NUM_OF_PORTS; port++)
    {
        if (RTK_PORTMASK_IS_PORT_SET(devInfo.ether.portmask, port))
        {
            ioal_mem32_write(0, (reg + 0x40*port), 0x0);
        }
    }
    /* CAMEOTAG:End */
#endif
	
    sdk_main();

    return RT_ERR_OK;
} /* end of main */


#if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC)
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
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
    uintptr addr;

    /* Get packet buffer start address that is mapped to user space */
    ioal_init_memRegion_get(HWP_MY_UNIT_ID(), IOAL_MEM_NIC, &addr);
    ioaddr = (void*)addr;
    ioal_init_memRegion_get(HWP_MY_UNIT_ID(), IOAL_MEM_DMA, &dma_vaddr);
    ioal_init_memRegion_get(HWP_MY_UNIT_ID(), IOAL_MEM_DMA_PHY, &dma_phy_addr);


    /* virtual addr */
    tally_vaddr         = dma_vaddr;
    tx_desc_vaddr       = tally_vaddr + TALLY_MEM_SIZE;
    rx_desc_vaddr       = tx_desc_vaddr + NUM_TX_DESC * sizeof(struct TxDesc);
    pkt_buf_vir_addres  = rx_desc_vaddr + NUM_RX_DESC * sizeof(struct RxDesc);

    /* physical addr */
    tally_phy_addr      = dma_phy_addr;
    tx_desc_phy_addr    = dma_phy_addr + 256;
    rx_desc_phy_addr    = tx_desc_phy_addr + NUM_TX_DESC * sizeof(struct TxDesc);
    pkt_phy_addr        = rx_desc_phy_addr + NUM_RX_DESC * sizeof(struct RxDesc);


    /* Create mutex semaphore */
    pkt_sem = osal_sem_mutex_create();

    /* Init buffer state */
    osal_memset(pkt_buf_stat, 0, sizeof(pkt_buf_stat));
    osal_memset(jumbo_pkt_buf_stat, 0, sizeof(jumbo_pkt_buf_stat));

    return RT_ERR_OK;
#else
    uint32  i;

    /* Get packet buffer start address that is mapped to user space */
    ioal_init_memRegion_get(unit, IOAL_MEM_DMA, &pkt_buf_vir_addres);

    /* Create mutex semaphore */
    pkt_sem = osal_sem_mutex_create();

    /* Init buffer state */
    osal_memset(pkt_buf_stat, 0, sizeof(pkt_buf_stat));
    osal_memset(pkt_buf_type, 0, sizeof(pkt_buf_type));
    for (i = QUEUE_POOL_MAX_SIZE - QUEUE_RX_POOL_SIZE - QUEUE_TX_POOL_SIZE; i < QUEUE_POOL_MAX_SIZE; i++)
    {
        if (i < QUEUE_POOL_MAX_SIZE - QUEUE_TX_POOL_SIZE)
            pkt_buf_type[i] = PKTBUF_RX;
        else
            pkt_buf_type[i] = PKTBUF_TX;
    }

    osal_memset(jumbo_pkt_buf_stat, 0, sizeof(jumbo_pkt_buf_stat));

    return RT_ERR_OK;
#endif /* CONFIG_SDK_DRIVER_EXTC_NIC */
}
#endif

#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
/* Function Name:
 *      rtnic_rx_pkt_alloc
 * Description:
 *      RX packet allocation
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
static int32 rtnic_rx_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket)
{
    uint32 i;
    uint8 *pBuf = NULL;
    drv_nic_pkt_t *pPacket = NULL;

    RT_PARAM_CHK(NULL == ppPacket, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((size > JUMBO_FRAME_SIZE_MAX), RT_ERR_INPUT);

    pPacket = osal_alloc(sizeof(drv_nic_pkt_t));
    if (pPacket == NULL)
    {
        osal_printf("%s():%d  malloc fail\n", __FUNCTION__, __LINE__);
        return RT_ERR_MEM_ALLOC;
    }

    if (size > NML_FRAME_SIZE_MAX)
    {
        for(i = 0; i < JUMBO_QUEUE_RX_POOL_SIZE; i++)
        {
            if(FALSE == jumbo_pkt_buf_stat[i])
            {
                jumbo_pkt_buf_stat[i] = TRUE;
                break;
            }
        }

        if (JUMBO_QUEUE_RX_POOL_SIZE == i)
        {
            osal_printf("%s():%d  run out of memory for RX jumbo\n", __FUNCTION__, __LINE__);
            goto _alloc_fail_exit;
        }
        pBuf = (uint8 *)(pkt_buf_vir_addres + (NML_BUFF_SIZE * QUEUE_POOL_MAX_SIZE) + (JUMBO_BUFF_SIZE * i));
        jumbo_rx_alloc_cnt++;
    }
    else
    {
        for(i = 0; i < QUEUE_RX_POOL_SIZE; i++)
        {
            if(FALSE == pkt_buf_stat[i])
            {
                pkt_buf_stat[i] = TRUE;
                break;
            }
        }

        if (QUEUE_RX_POOL_SIZE == i)
        {
            osal_printf("%s():%d  run out of memory for RX normal packet\n", __FUNCTION__, __LINE__);
            goto _alloc_fail_exit;
        }
        pBuf = (uint8 *)(pkt_buf_vir_addres + (NML_BUFF_SIZE * i));
        nml_rx_alloc_cnt++;
    }


    osal_memset(pPacket, 0, sizeof(drv_nic_pkt_t));
    pPacket->head = (uint8 *)(pBuf);
    pPacket->data = (pPacket->head + BUFF_RSVD_SIZE);
    pPacket->length = 0;
    if (size > NML_FRAME_SIZE_MAX)
    {
        pPacket->buf_id = (void *)(uintptr)(i | FLAG_JUMBO_PKT);
        pPacket->tail = (pBuf + JUMBO_BUFF_SIZE);
    }
    else
    {
        pPacket->buf_id = (void *)(uintptr)i;
        pPacket->tail = (pBuf + NML_BUFF_SIZE);
    }
    pPacket->end = pPacket->tail;
    pPacket->next = NULL;

    *ppPacket = pPacket;
    return RT_ERR_OK;

_alloc_fail_exit:
    osal_free(pPacket);
    return RT_ERR_FAILED;
} /* end of rtnic_rx_pkt_alloc */
#endif /* CONFIG_SDK_DRIVER_EXTC_NIC */


#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
/* Function Name:
 *      rtnic_pkt_alloc
 * Description:
 *      TX packet allocation
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
    uint32 i;
    uint8 *pBuf = NULL;
    drv_nic_pkt_t *pPacket = NULL;

    RT_PARAM_CHK(NULL == ppPacket, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((size > JUMBO_FRAME_SIZE_MAX), RT_ERR_INPUT);

    pPacket = osal_alloc(sizeof(drv_nic_pkt_t));
    if (pPacket == NULL)
    {
        osal_printf("%s():%d  malloc fail\n", __FUNCTION__, __LINE__);
        return RT_ERR_MEM_ALLOC;
    }

    PKT_SEM_LOCK();

    if (size > NML_FRAME_SIZE_MAX)
    {
        for(i = JUMBO_QUEUE_RX_POOL_SIZE; i < JUMBO_QUEUE_POOL_MAX_SIZE; i++)
        {
            if(FALSE == jumbo_pkt_buf_stat[i])
            {
                jumbo_pkt_buf_stat[i] = TRUE;
                break;
            }
        }

        if (JUMBO_QUEUE_POOL_MAX_SIZE == i)
        {
            osal_printf("%s():%d  run out of memory for TX jumbo\n", __FUNCTION__, __LINE__);
            goto _alloc_fail_exit;
        }
        pBuf = (uint8 *)(pkt_buf_vir_addres + (NML_BUFF_SIZE * QUEUE_POOL_MAX_SIZE) + (JUMBO_BUFF_SIZE * i));
        jumbo_tx_alloc_cnt++;
    }
    else
    {
        for(i = QUEUE_RX_POOL_SIZE; i < QUEUE_POOL_MAX_SIZE; i++)
        {
            if(FALSE == pkt_buf_stat[i])
            {
                pkt_buf_stat[i] = TRUE;
                break;
            }
        }

        if (QUEUE_POOL_MAX_SIZE == i)
        {
            osal_printf("%s():%d  run out of memory for TX normal packet\n", __FUNCTION__, __LINE__);
            goto _alloc_fail_exit;
        }
        pBuf = (uint8 *)(pkt_buf_vir_addres + (NML_BUFF_SIZE * i));
        nml_tx_alloc_cnt++;
    }

    PKT_SEM_UNLOCK();

    osal_memset(pPacket, 0, sizeof(drv_nic_pkt_t));
    pPacket->head = (uint8 *)(pBuf);
    pPacket->data = (pPacket->head + BUFF_RSVD_SIZE);
    pPacket->length = 0;
    if (size > NML_FRAME_SIZE_MAX)
    {
        pPacket->buf_id = (void *)(uintptr)(i | FLAG_JUMBO_PKT);
        pPacket->tail = (pBuf + JUMBO_BUFF_SIZE);
    }
    else
    {
        pPacket->buf_id = (void *)(uintptr)i;
        pPacket->tail = (pBuf + NML_BUFF_SIZE);
    }
    pPacket->end = pPacket->tail;
    pPacket->next = NULL;

    *ppPacket = pPacket;
    return RT_ERR_OK;

_alloc_fail_exit:
    osal_free(pPacket);
    PKT_SEM_UNLOCK();
    return RT_ERR_FAILED;
} /* end of rtnic_pkt_alloc */
#elif defined(CONFIG_SDK_DRIVER_NIC)
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
    const uint32    dPool = QUEUE_POOL_MAX_SIZE - QUEUE_RX_POOL_SIZE - QUEUE_TX_POOL_SIZE;  //dedicate pool for RX/TX
    uint32 i;
    uint8 *pBuf;
    drv_nic_pkt_t *pPacket;

    RT_PARAM_CHK((size > JUMBO_FRAME_SIZE_MAX), RT_ERR_INPUT);

    pPacket = osal_alloc(sizeof(drv_nic_pkt_t));
    if (pPacket == NULL)
    {
        osal_printf("rtnic_pkt_alloc: malloc fail\n");
        return RT_ERR_MEM_ALLOC;
    }

    PKT_SEM_LOCK();

    if (size > NML_FRAME_SIZE_MAX)
    {
        for(i = 0; i < JUMBO_QUEUE_POOL_MAX_SIZE; i++)
        {
            if(FALSE == jumbo_pkt_buf_stat[i])
            {
                jumbo_pkt_buf_stat[i] = TRUE;
                break;
            }
        }

        if (JUMBO_QUEUE_POOL_MAX_SIZE == i)
        {
            osal_printf("rtnic_pkt_alloc: run out of memory for jumbo\n");
            goto _alloc_fail_exit;
        }
        pBuf = (uint8 *)(pkt_buf_vir_addres + (NML_BUFF_SIZE * QUEUE_POOL_MAX_SIZE) + (JUMBO_FRAME_SIZE_MAX * i));
    }
    else
    {
        for(i = 0; i < QUEUE_POOL_MAX_SIZE; i++)
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

        if (QUEUE_POOL_MAX_SIZE == i)
        {
            osal_printf("rtnic_pkt_alloc: run out of memory\n");
            goto _alloc_fail_exit;
        }
        pBuf = (uint8 *)(pkt_buf_vir_addres + (NML_BUFF_SIZE * i));
    }

    PKT_SEM_UNLOCK();

    osal_memset(pPacket, 0, sizeof(drv_nic_pkt_t));
    pPacket->head = (uint8 *)(pBuf);
    pPacket->data = (pPacket->head + BUFF_RSVD_SIZE);
    pPacket->length = 0;
    if (size > NML_FRAME_SIZE_MAX)
    {
        pPacket->buf_id = (void *)(uintptr)(i | FLAG_JUMBO_PKT);
        pPacket->tail = (pBuf + JUMBO_BUFF_SIZE);
    }
    else
    {
        pPacket->buf_id = (void *)(uintptr)i;
        pPacket->tail = (pBuf + NML_BUFF_SIZE);
    }
    pPacket->end = pPacket->tail;
    pPacket->next = NULL;


    *ppPacket = pPacket;
    return RT_ERR_OK;

_alloc_fail_exit:
    osal_free(pPacket);
    PKT_SEM_UNLOCK();
    return RT_ERR_FAILED;
} /* end of rtnic_pkt_alloc */
#endif

#if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC)
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
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
    uint32 id;

    if (NULL == pPacket)
    {
        free_fail_cnt++;
        osal_printf("Error: pPacket is NULL at %s():%d\n", __FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }

    id = (uint32)(uintptr)pPacket->buf_id;

    if (FLAG_JUMBO_PKT & id)
    {
        if (FALSE == jumbo_pkt_buf_stat[id & ~(FLAG_JUMBO_PKT)])
        {
            osal_printf("Error: buffer for jumbo is NULL at %s():%d\n", __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }

        PKT_SEM_LOCK();
        jumbo_free_cnt++;
        PKT_SEM_UNLOCK();
    }
    else
    {
        if (FALSE == pkt_buf_stat[id])
        {
            osal_printf("Error: skb is NULL at %s():%d\n", __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }

        PKT_SEM_LOCK();
        nml_free_cnt++;
        PKT_SEM_UNLOCK();
    }

    if (FLAG_JUMBO_PKT & id)
        jumbo_pkt_buf_stat[id & ~(FLAG_JUMBO_PKT)] = FALSE;
    else
        pkt_buf_stat[id] = FALSE;

    osal_free(pPacket);

    return RT_ERR_OK;
#else
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
        if (FALSE == jumbo_pkt_buf_stat[id & ~(FLAG_JUMBO_PKT)])
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
        jumbo_pkt_buf_stat[id & ~(FLAG_JUMBO_PKT)] = FALSE;
    else
        pkt_buf_stat[id] = FALSE;

    osal_free(pPacket);

    PKT_SEM_UNLOCK();
    return RT_ERR_OK;
#endif
} /* end of rtnic_pkt_free */
#endif /* #if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC) */


#if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC)
drv_nic_rx_t rtnic_rx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
    int32  retval;

    if (NULL == pPacket)
    {
        osal_printf("Error: pPacket is NULL at %s():%d\n", __FUNCTION__, __LINE__);
        goto _exit;
    }

    if ((retval = drv_nic_pkt_free(unit, pPacket)) != RT_ERR_OK)
    {
        osal_printf("Error: Can't free RX packet buffer %s():%d\n", __FUNCTION__, __LINE__);
        goto _exit;
    }

    return NIC_RX_HANDLED_OWNED;

_exit:
    return NIC_RX_NOT_HANDLED;
#else
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
            if ((retval = rtnic_pkt_alloc(unit, JUMBO_FRAME_SIZE_MAX, PKTBUF_TX, &pTx_packet)) != RT_ERR_OK)
            {
                osal_printf("Error: Can't allocate TX packet buffer %s():%d\n", __FUNCTION__, __LINE__);
                goto _exit;
            }
            pTx_packet->buf_id = (void *)((uint32)pTx_packet->buf_id | FLAG_JUMBO_PKT);
        }
        else
        {
            if ((retval = rtnic_pkt_alloc(unit, NML_FRAME_SIZE_MAX, PKTBUF_TX, &pTx_packet)) != RT_ERR_OK)
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
#endif
} /* end of rtnic_rx_callback */
#endif /* #if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC) */


#if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC)
void rtnic_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    if (NULL == pPacket)
    {
        osal_printf("Error: pPacket is NULL at %s():%d\n", __FUNCTION__, __LINE__);
        return;
    }

#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
    drv_nic_pkt_free(unit, pPacket);
#else
    rtnic_pkt_free(unit, pPacket);
#endif
    return;
} /* end of rtnic_tx_callback */
#endif /* #if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC) */

#if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC)
static int32 rtnic_init(void)
{
#if defined(CONFIG_SDK_DRIVER_EXTC_NIC)
    int32  ret;
    drv_nic_initCfg_t initcfg;


    initcfg.rx_pkt_alloc   = (drv_nic_pkt_alloc_f)rtnic_rx_pkt_alloc;
    initcfg.pkt_alloc   = (drv_nic_pkt_alloc_f)rtnic_pkt_alloc;
    initcfg.pkt_free    = (drv_nic_pkt_free_f)rtnic_pkt_free;
    initcfg.pkt_size    = NML_FRAME_SIZE_MAX;
    initcfg.jumbo_size  = JUMBO_FRAME_SIZE_MAX;

    rtnic_pkt_buf_init(HWP_MY_UNIT_ID());
    drv_nic_init(HWP_MY_UNIT_ID(), &initcfg);

    if ((ret = rtcore_usr_intr_attach(HWP_MY_UNIT_ID(), NULL, NULL, INTR_TYPE_NIC) != RT_ERR_OK))
    {
        osal_printf("%s():%d  RTK interrupt attach failed\n", __FUNCTION__, __LINE__);
    }

    if ((ret = nic_rx_thread_cb_register(HWP_MY_UNIT_ID(), 1, rtnic_rx_callback, NULL, 0) != RT_ERR_OK))
    {
        osal_printf("%s():%d  RTK System Init nic_rx_thread_cb_register fail\n", __FUNCTION__, __LINE__);
    }
    if ((ret = drv_nic_rx_start(HWP_MY_UNIT_ID()) != RT_ERR_OK))
    {
        osal_printf("%s():%d  NIC rx start fail\n", __FUNCTION__, __LINE__);
    }


    return ret;
#else
    int32  ret;
    drv_nic_initCfg_t initcfg;
#if defined(CONFIG_SDK_DRIVER_L2NTFY)
    uint32  local_unit;
    rtk_l2ntfy_dst_t  now_dst;
#endif

    initcfg.pkt_alloc = (drv_nic_pkt_alloc_f)rtnic_pkt_alloc;
    initcfg.pkt_free = (drv_nic_pkt_free_f)rtnic_pkt_free;
    initcfg.pkt_size = NML_FRAME_SIZE_MAX;
    initcfg.jumbo_size  = JUMBO_FRAME_SIZE_MAX;

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
#endif/* CONFIG_SDK_DRIVER_EXTC_NIC */
}
#endif /* #if defined(CONFIG_SDK_DRIVER_EXTC_NIC) || defined(CONFIG_SDK_DRIVER_NIC) */
