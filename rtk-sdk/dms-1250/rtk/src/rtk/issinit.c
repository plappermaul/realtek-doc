/*Description:
 *This file is used for upper application to init sdk
 */
#ifndef _ISS_INIT_C
#define _ISS_INIT_C

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <common/util/rt_util.h>
#include <ioal/ioal_init.h>
#include <drv/spi/spi.h>
#include <drv/nic/nic.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/print.h>
#include <osal/sem.h>
#include <hwp/hw_profile.h>
#include <common/rtcore/rtcore_init.h>
#include <rtk/init.h>
#include <rtk/l2.h>
#include <drv/l2ntfy/l2ntfy.h>
#include <stdlib.h>
#include <rtcore/user/rtcore_drv_usr.h>
#include <rtk/switch.h>
#include <rtk/issinit.h>
#include <ioal/mem32.h>
#include <hal/mac/serdes.h>

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


/*
 * Data Declaration
 */
static uint32 pkt_buf_vir_addres;
static uint32 pkt_buf_stat[QUEUE_POOL_MAX_SIZE];

/* 
 * Function Declaration  
 */
static int32 rtnic_pkt_buf_init(uint32 unit);
static int32 cm_iss_port_forward_init(void);

/* Function Name:
 *      iss_init_rtk
 * Description:
 *       init sdk by ISS
 * Input:
 * Output:
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */ 

int iss_init_rtk(void)
{
	int32  ret;

    /* Init RTK Layer API */
    RT_INIT_MSG("RTK User Space SDK Initialize\n");

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
    RT_LOG(LOG_EVENT, MOD_INIT, "rtk_init Completed!!\n");

    if ((ret = iss_nic_init()) != RT_ERR_OK)
    {
        RT_INIT_ERR(ret, MOD_INIT, "rtnic_init failed!!\n");
        return ret;
    }
    RT_LOG(LOG_EVENT, MOD_INIT, "rtnic_init Completed!!\n");

    rtcore_usr_init();

    /* CAMEOTAG: added by zhifu on 2018/3/22, init all ports 
     * to forwarding state to make DUT can forward packets. */
    if (cm_iss_port_forward_init() != RT_ERR_OK)
    {
            RT_INIT_ERR(ret, MOD_INIT, "cm_iss_port_forward_init failed!!\n");

        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      iss_nic_init
 * Description:
 *      init nic for iss
 * Input:
 * Output:
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */ 
int32 iss_nic_init(void)
{
    int32  ret;
    drv_nic_initCfg_t initcfg;

    initcfg.pkt_alloc = (drv_nic_pkt_alloc_f)rtnic_pkt_alloc;
    initcfg.pkt_free = (drv_nic_pkt_free_f)rtnic_pkt_free;
    initcfg.pkt_size = DMA_PKT_BUF_ENTRY_SIZE;
    /* CAMEOTAG:add by Jiane on 2022-5-6 ticket:25148
     * DES:system crash when ping dut with jumbo frame."ping dut-ip -l 8000" for example
     * RootCause:jumbo_size not init, still zero.when _nic_isr_rxRoutine use it to alloc 
     *           memroy for packet rx and use the memory. It will cause crash.
     * Solution:init it as the max jumbo frame size.
     * */
    initcfg.jumbo_size = JUMBO_FRAME_SIZE_MAX;

    rtnic_pkt_buf_init(HWP_MY_UNIT_ID());
    RT_ERR_CHK(nic_probe(HWP_MY_UNIT_ID()), ret);
    drv_nic_init(HWP_MY_UNIT_ID(), &initcfg);

	/* Connect interrupt with rtcore module */
	if(HWP_NIC_SUPPORT(HWP_MY_UNIT_ID()))
        rtcore_usr_intr_attach(HWP_MY_UNIT_ID(), NULL, NULL, INTR_TYPE_NIC);

    return RT_ERR_OK;
}

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
static int32 rtnic_pkt_buf_init(uint32 unit)
{
    /* Get packet buffer start address that is mapped to user space */
    ioal_init_memRegion_get(unit, IOAL_MEM_DMA, &pkt_buf_vir_addres);

    /* Create mutex semaphore */
    pkt_sem = osal_sem_mutex_create();

    /* Init buffer state */
    osal_memset(pkt_buf_stat, 0, sizeof(pkt_buf_stat));

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
int32 rtnic_pkt_alloc(uint32 unit, int32 size, uint32 flags, drv_nic_pkt_t **ppPacket)
{
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

    for(i = 0; i < QUEUE_POOL_MAX_SIZE; i++)
    {
        if(FALSE == pkt_buf_stat[i])
        {
            pkt_buf_stat[i] = TRUE;
            break;
        }
    }

    if (QUEUE_POOL_MAX_SIZE == i)
    {
        osal_free(pPacket);
        osal_printf("rtnic_pkt_alloc: run out of memory\n");
        PKT_SEM_UNLOCK();
        return RT_ERR_FAILED;
    }

    pBuf = (uint8 *)(pkt_buf_vir_addres + (size * i));

    osal_memset(pPacket, 0, sizeof(drv_nic_pkt_t));
    pPacket->head = (uint8 *)(pBuf);
    pPacket->data = (pPacket->head + 18);
    pPacket->tail = (pBuf + size);  /* No reserve any tail space */
    pPacket->end = pPacket->tail;
    pPacket->length = 0;
    pPacket->buf_id = (void *)i;
    pPacket->next = NULL;

    *ppPacket = pPacket;
    PKT_SEM_UNLOCK();
    return RT_ERR_OK;
}



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
int32 rtnic_pkt_free(uint32 unit, drv_nic_pkt_t *pPacket)
{
    uint32 id;

    PKT_SEM_LOCK();
    id = (uint32)pPacket->buf_id;

    if (NULL == pPacket)
    {
        osal_printf("Error: pPacket is NULL at %s():%d\n", __FUNCTION__, __LINE__);
        PKT_SEM_UNLOCK();
        return RT_ERR_FAILED;
    }

    if (FALSE == pkt_buf_stat[id])
    {
        osal_printf("Error: skb is NULL at %s():%d\n", __FUNCTION__, __LINE__);
        PKT_SEM_UNLOCK();
        return RT_ERR_FAILED;
    }

    pkt_buf_stat[id] = FALSE;

    osal_free(pPacket);

    PKT_SEM_UNLOCK();
    return RT_ERR_OK;
} /* end of rtnic_pkt_free */

/* Enable forwarding state for all ports according to diag shell 
 * instructions provided by FAE. */
static int32 cm_iss_port_forward_init(void)
{
    uint32     value = 0;
    uint32     reg   = 0;
    rtk_port_t port  = 0;
    rtk_switch_devInfo_t devInfo;

    osal_memset(&devInfo, 0, sizeof(devInfo));
    if (rtk_switch_deviceInfo_get(0, &devInfo) != RT_ERR_OK)
    {
        osal_printf("Error: get switch device info failed at %s():%d\n",
                    __FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }  
    /* set port admin state to enable for all ports */
    for (port = 0; port < RTK_MAX_NUM_OF_PORTS; port++)
    {
        if (RTK_PORTMASK_IS_PORT_SET(devInfo.ether.portmask, port))
        {
#ifdef CAMEO_ENABLE_PORT_WANTED
            if (rtk_port_adminEnable_set(0, port, ENABLED) != RT_ERR_OK)
#else
            if (rtk_port_adminEnable_set(0, port, DISABLED) != RT_ERR_OK)
#endif
            {
                osal_printf("Error: set port admin failed at %s():%d\n",
                    __FUNCTION__, __LINE__);
                return RT_ERR_FAILED;
            }
            /* Set the max packet length to include tag length */
            if (RT_ERR_OK != rtk_switch_portMaxPktLenTagLenCntIncEnable_set(0, 
                                                                            port, 
                                                                            ENABLED))
            {
                osal_printf("Error: Set the max packet length to include tag length failed at %s():%d\n",
                    __FUNCTION__, __LINE__);
                return RT_ERR_FAILED;
            }

            /* CAMEOTAG:Added by Jincai on 2018.8.14,because sdk just support force 100f,
             * so set default force mode ability to 100f,otherwise it will return failed. */
             /*CAMEOTAG:Added macro by tonghong : Dump and ESX-200 series and teg-7102ws series only support 100M.*/
#if defined(CONFIG_SDK_PHY_CUST1)
          if (RT_ERR_OK != rtk_port_phyForceModeAbility_set(0, port, PORT_SPEED_100M, PORT_FULL_DUPLEX, DISABLED))
            {
                osal_printf("Error: Set force mode ability to 100f failed at %s():%d\n",
                    __FUNCTION__, __LINE__);
                return RT_ERR_FAILED;
            }
#endif

             /*CAMEOTAG:Added  by tonghong 2019/03/28 : When init tx/rx init is disable for 9301.So must enbale it first.*/
#ifdef CAMEO_TXRX_ENABLE_WANTED
		/*enable tx/rx when init*/
	   if (rtk_port_rxEnable_set(0, port, ENABLED) != RT_ERR_OK)	
		{
                osal_printf("Error: set port ex enable  at %s():%d\n",
                    __FUNCTION__, __LINE__);
                return RT_ERR_FAILED;
            }
		 /*enable tx/rx when init*/
	   if (rtk_port_txEnable_set(0, port, ENABLED) != RT_ERR_OK)	
		{
                osal_printf("Error: set port ex enable  at %s():%d\n",
                    __FUNCTION__, __LINE__);
                return RT_ERR_FAILED;
            }
#endif
        }
    }

       /*CAMEOTAG:Added by tonghong 2019/03/29 : 
       The fiber port mudium is  SerDes port , it must init it's 10gMedia first otherwist they can't work(From FAE).*/
#ifdef CAMEO_FIBER_PORT_ENABLE_WANTED
	port  = 0 ;
	HWP_SERDES_PORT_TRAVS(0, port)
	{
		if (rtk_port_10gMedia_set(0,port,PORT_10GMEDIA_FIBER_1G) != RT_ERR_OK)
		{
			osal_printf("\nError: set fiber port admin failed at %s():%d\n",  __FUNCTION__, __LINE__);
		 	return RT_ERR_FAILED;

		}
	}
#endif
#if defined(CONFIG_SDK_RTL9300)
    /*CAMEOTAG:Added by Jincai on 2018.5.7,fix the bug that flow control is not workable on RTL930X for ISS,
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
#endif    
/*CAMEOTAG:Add by tonghong 2019/6/21
The following init is special for the AQ phy drive
*/
#if defined(CONFIG_SDK_RTL9300) && defined(CONFIG_SDK_PHY_CUST1)
    /* Set SerDes registers */
    hal_serdes_reg_set(0, 2, 7, 17, 0x54f);
    hal_serdes_reg_set(0, 2, 6, 14, 0x55a);
    hal_serdes_reg_set(0, 2, 7, 16, 0x6003);
    hal_serdes_reg_set(0, 2, 6, 19, 0x68c1);
    hal_serdes_reg_set(0, 2, 6, 20, 0xf021);
    
    hal_serdes_reg_set(0, 3, 7, 17, 0x54f);
    hal_serdes_reg_set(0, 3, 6, 14, 0x55a);
    hal_serdes_reg_set(0, 3, 7, 16, 0x6003);
    hal_serdes_reg_set(0, 3, 6, 19, 0x68c1);
    hal_serdes_reg_set(0, 3, 6, 20, 0xf021);
    
    hal_serdes_reg_set(0, 4, 7, 17, 0x54f);
    hal_serdes_reg_set(0, 4, 6, 14, 0x55a);
    hal_serdes_reg_set(0, 4, 7, 16, 0x6003);
    hal_serdes_reg_set(0, 4, 6, 19, 0x68c1);
    hal_serdes_reg_set(0, 4, 6, 20, 0xf021);
    
    hal_serdes_reg_set(0, 5, 7, 17, 0x54f);
    hal_serdes_reg_set(0, 5, 6, 14, 0x55a);
    hal_serdes_reg_set(0, 5, 7, 16, 0x6003);
    hal_serdes_reg_set(0, 5, 6, 19, 0x68c1);
    hal_serdes_reg_set(0, 5, 6, 20, 0xf021);
    
    hal_serdes_reg_set(0, 6, 7, 17, 0x54f);
    hal_serdes_reg_set(0, 6, 6, 14, 0x55a);
    hal_serdes_reg_set(0, 6, 7, 16, 0x6003);
    hal_serdes_reg_set(0, 6, 6, 19, 0x68c1);
    hal_serdes_reg_set(0, 6, 6, 20, 0xf021);
    
    hal_serdes_reg_set(0, 7, 7, 17, 0x54f);
    hal_serdes_reg_set(0, 7, 6, 14, 0x55a);
    hal_serdes_reg_set(0, 7, 7, 16, 0x6003);
    hal_serdes_reg_set(0, 7, 6, 19, 0x68c1);
    hal_serdes_reg_set(0, 7, 6, 20, 0xf021);
    
    hal_serdes_reg_set(0, 8, 7, 17, 0x54f);
    hal_serdes_reg_set(0, 8, 6, 14, 0x55a);
    hal_serdes_reg_set(0, 8, 7, 16, 0x6003);
    hal_serdes_reg_set(0, 8, 6, 19, 0x68c1);
    hal_serdes_reg_set(0, 8, 6, 20, 0xf021);
    
    hal_serdes_reg_set(0, 9, 7, 17, 0x54f);
    hal_serdes_reg_set(0, 9, 6, 14, 0x55a);
    hal_serdes_reg_set(0, 9, 7, 16, 0x6003);
    hal_serdes_reg_set(0, 9, 6, 19, 0x68c1);
    hal_serdes_reg_set(0, 9, 6, 20, 0xf021);

    /* Set PHY MMD register data */
    rtk_port_phymaskMmdReg_set(0, &(devInfo.ether.portmask), 4, 0xc441, 0x8);
#endif

#if defined(CONFIG_SDK_RTL9310) && defined(CONFIG_SDK_PHY_CUST1)
    /* Set PHY MMD register data */
    rtk_port_phymaskMmdReg_set(0, &(devInfo.ether.portmask), 1, 0xe400, 0x6);
#endif


    return RT_ERR_OK;
}

#endif /*_ISS_INIT_C*/
