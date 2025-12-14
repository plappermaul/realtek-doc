/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision: 73516 $
 * $Date: 2016-11-22 15:14:09 +0800 (Tue, 22 Nov 2016) $
 *
 * Purpose : EPON polling kernel thread
 *
 * Feature : Use kernel thread to perodically polling the LOS state
 *
 */

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/delay.h>

/* For RTK APIs */
#include <common/rt_error.h>
#include <rtk/gponv2.h>
#include <rtk/ponmac.h>
#include <rtk/switch.h>
#include <module/gpon_pppoe_adv/gpon_pppoe_adv.h>
#include <module/gpon/gpon_defs.h>

#if defined(CONFIG_SDK_APOLLOMP)
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/raw/apollomp_raw_flowctrl.h>
#endif
#if defined(CONFIG_SDK_RTL9601B)
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_pbo.h>
#endif


#include <module/intr_bcaster/intr_bcaster.h>
/*
 * Symbol Definition
 */
#define SMALL_BW_THRESHOLD          (14)
#define SMALL_BW_OFFLOAD_THRESHOLD  (22)
#define GPON_TCONT_NUM (32)
#define QUE_CHECK_CNT (24)

typedef struct gpon_pppoe_tcont_s{
    uint32 tcont_valid;
    gpon_pppoe_mode_t pppoe_mode;
}gpon_pppoe_tcont_t;

typedef struct gpon_pppoe_db_s{
    gpon_pppoe_tcont_t tcont_info[GPON_TCONT_NUM];
    uint32 check_que;
    uint32 sent_notify;
    uint32 sent_offload_notify;
}gpon_pppoe_db_t;

/*
 * Data Declaration
 */

struct task_struct *pGponPppoelTask;
struct task_struct *pGponDbruTask;
static uint32 target_tcont;
static gpon_pppoe_db_t gpon_pppoe_db;
static uint32 tcont_max;
static rtk_gpon_omci_msg_t omci_msg;
static uint8 dummy_content[] = {0x0, 0x0, 0x11, 0x0a, 
                                0x0, 0x86, 0x0, 0x0, 
                                0x80, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0, 
                                0x0, 0x0, 0x0, 0x0,
                                0x0, 0x0, 0x0, 0x0,
                                0x0, 0x0, 0x0, 0x0,
                                0x0, 0x0, 0x0, 0x0,
                                0x0, 0x0, 0x0, 0x0,   
                                0x0, 0x0, 0x0, 0x0,
                                0x0, 0x0, 0x0, 0x28,
                                0x86, 0x60, 0x1b, 0x13};
uint32 rg_sw_rst=0;
/*
 * Macro Definition
 */

/*
 * Function Declaration
 */
int gpon_pppoe_db_clear(void)
{
    osal_memset(&gpon_pppoe_db, 0x0, sizeof(gpon_pppoe_db_t));
    return RT_ERR_OK;
}

int gpon_pppoe_mode_get(uint32 sid, gpon_pppoe_mode_t *pMode)
{
#if defined(CONFIG_SDK_APOLLOMP)
    rtk_ponmac_queue_t qcfg;
    uint32 tcontBitMask;
    int32 ret;
    
    if((ret = rtk_ponmac_flow2Queue_get(sid, &qcfg))!=RT_ERR_OK)
        return ret;

    gpon_pppoe_db.tcont_info[qcfg.schedulerId].tcont_valid = 1;
    gpon_pppoe_db.check_que = 1;
    target_tcont = qcfg.schedulerId;

    /* check current data */
    if((ret = rtk_gpon_smallBwTcont_get(SMALL_BW_THRESHOLD, &tcontBitMask))!=RT_ERR_OK)
        return ret;

    /* result is history | current */
    *pMode = gpon_pppoe_db.tcont_info[qcfg.schedulerId].pppoe_mode | ((tcontBitMask>>target_tcont)&0x1);
#endif
    return RT_ERR_OK;
}

int gpon_pppoe_thread(void *data)
{
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 tcontBitMask, tcontBitMask1;
    uint32 tcont;
    uint32 que;
    uint32 curr;
    uint32 busy;
    rtk_ponmac_queue_t omci_queue;
    rtk_gpon_onuState_t  state;
    int32 ret;

    while(!kthread_should_stop())
    {
        /* It is NOT possible to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(5 * HZ ); /* Wake up every 5 s*/

        if((ret = rtk_gpon_onuState_get(&state)) != RT_ERR_OK)
        {
            continue;
        }
        if(state != GPON_STATE_O5)
        {
            continue;
        }

        /* check bwmap for tconts for small BW and offload */
        if((ret = rtk_gpon_smallBwTcont_get(SMALL_BW_THRESHOLD, &tcontBitMask))==RT_ERR_OK)
        {
            if((ret = rtk_gpon_smallBwTcont_get(SMALL_BW_OFFLOAD_THRESHOLD, &tcontBitMask1))==RT_ERR_OK)
            {
            
                /*osal_printf("tcontBitMask=0x%x, tcontBitMask1=0x%x\n",tcontBitMask, tcontBitMask1);*/
                for (tcont = 0; tcont < tcont_max; tcont ++) 
                {
                    /* update db*/
                    if((tcontBitMask>>tcont)&0x1) /* small BW is first priority */
                    {
                        gpon_pppoe_db.tcont_info[tcont].pppoe_mode = GPON_PPPOE_MODE_SW;
                    }
                    else if((tcontBitMask1>>tcont)&0x1) /* offload is second priority */
                    {
                        if(gpon_pppoe_db.tcont_info[tcont].pppoe_mode != GPON_PPPOE_MODE_SW)
                        {
                            gpon_pppoe_db.tcont_info[tcont].pppoe_mode = GPON_PPPOE_MODE_SW_OFFLOAD;
                        }
                    }
                    /*osal_printf("mode[%d]=0x%x \n",tcont,gpon_pppoe_db.tcont_info[tcont].pppoe_mode);*/
    
                    /* check tcont mode for notify */
                    if(gpon_pppoe_db.tcont_info[tcont].tcont_valid) 
                    {
                        if((gpon_pppoe_db.tcont_info[tcont].pppoe_mode == GPON_PPPOE_MODE_SW) &&
                           (gpon_pppoe_db.sent_notify == 0))
                        {
                            osal_printf("send GPON_PPPOE_SUBTYPE_SW\n");
                            /* send event to notify APP */
                            queue_broadcast(MSG_TYPE_PPPOE_EVENT,GPON_PPPOE_SUBTYPE_SW, 0, 0);
                            gpon_pppoe_db.sent_notify = 1;
                        }
                        if((gpon_pppoe_db.tcont_info[tcont].pppoe_mode == GPON_PPPOE_MODE_SW_OFFLOAD) &&
                           (gpon_pppoe_db.sent_offload_notify == 0))
                        {
                            osal_printf("send GPON_PPPOE_SUBTYPE_SW\n");
                            /* send event to notify APP */
                            queue_broadcast(MSG_TYPE_PPPOE_EVENT,GPON_PPPOE_SUBTYPE_SW_OFFLOAD, 0, 0);
                            gpon_pppoe_db.sent_offload_notify = 1;
                        }
                    }
                }
            }
            else
            {
                osal_printf("rtk_gpon_smallBwTcont_get %d, ret=0x%x\n",SMALL_BW_OFFLOAD_THRESHOLD,ret);
            }
        }
        else
        {
            osal_printf("rtk_gpon_smallBwTcont_get %d, ret=0x%x\n",SMALL_BW_THRESHOLD,ret);
            /* O5 but valid entry, reconnect */
            if(ret == RT_ERR_ENTRY_NOTFOUND)
            {
                if((ret = rtk_gponapp_deActivate()) != RT_ERR_OK)
                {
                    osal_printf("rtk_gponapp_deActivate fail, ret=0x%x\n",ret);
                }
                if((ret = rtk_gponapp_activate(RTK_GPONMAC_INIT_STATE_O1)) != RT_ERR_OK)
                {
                    osal_printf("rtk_gponapp_activate fail, ret=0x%x\n",ret);
                }
            }
        }

        
        /* check queue status */
        if(gpon_pppoe_db.check_que)
        {
            /* send a OMCI AVC for check OMCI queue status */
            if ((ret = rtk_gponapp_omci_tx(&omci_msg)) != RT_ERR_OK)
            {
                continue;
            }
            /* switch to other queue at the first */
            que = 0;
            if ((ret = reg_field_write(APOLLOMP_QUEUE_SEL_INDr, APOLLOMP_QCNT_SEL_INDf, &que)) != RT_ERR_OK)
            {
                continue;
            }
            /* set to OMCI queue */
            que = (GPON_OMCI_TCONT_ID/8) * 32 + (GPON_OMCI_QUEUE_ID);        
            if ((ret = reg_field_write(APOLLOMP_QUEUE_SEL_INDr, APOLLOMP_QCNT_SEL_INDf, &que)) != RT_ERR_OK)
            {
                continue;
            }
            if ((ret = reg_field_read(APOLLOMP_FC_PON_Q_USED_PAGE_CNTr, APOLLOMP_Q_USED_PAGE_CNTf, &curr)) == RT_ERR_OK)
            {
                /*osal_printf("queue[%u] current=%u\n",que,curr);*/
                /* check current > check_count */
                if(curr > QUE_CHECK_CNT)
                {
                    osal_printf("drain out queue\n");
                    /*try drain out*/
                    omci_queue.schedulerId = GPON_OMCI_TCONT_ID;
                    omci_queue.queueId = GPON_OMCI_QUEUE_ID;
                    if((ret = rtk_ponmac_queueDrainOut_set(&omci_queue))==RT_ERR_OK)
                    {
                        if(reg_field_read(APOLLOMP_PONMAC_DRN_CTRLr, APOLLOMP_PON_DRN_BUSYf, &busy)==RT_ERR_OK)
                        {
                            if(busy == 1)
                            {
                                osal_printf("send GPON_PPPOE_SUBTYPE_RECOVER\n");
                                /* send event to notify APP */
                                queue_broadcast(MSG_TYPE_PPPOE_EVENT,GPON_PPPOE_SUBTYPE_RECOVER, 0, 0);
                            }
                        }
                    }
                }
            }
        }
    }
#endif
    return (RT_ERR_OK);
}

int gpon_egrPage_get(uint32 *pPage)
{
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 i;
    uint32 currPage, maxPage;
    int32 ret;

    *pPage = 0;
    
    for(i=0; i<10; i++)
    {
        if((ret = apollomp_raw_flowctrl_portEgressUsedPageCnt_get(HAL_GET_PON_PORT(), &currPage, &maxPage)) == RT_ERR_OK)
        {
            if(currPage > *pPage)
            {   
                *pPage = currPage;
            }
        }
        else
        {
            return ret;
        }
    }
#endif
    return (RT_ERR_OK);
}

int gpon_dbru_get(uint32 *pDbru)
{
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 data, i;
    uint32 tmp_dbr;
    int32 ret;

    *pDbru = 0;
    
    data = 0x86;
    if ((ret = reg_field_write(APOLLOMP_DEBUG_SELr, APOLLOMP_DBGO_SELf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }
    data = 0x602;
    if ((ret = reg_field_write(APOLLOMP_DBG_BLK_SELr, APOLLOMP_DBG_BLK_SELf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }
    osal_time_mdelay(1);
    for(i=0; i<100; i++)
    {
        if ((ret = reg_read(APOLLOMP_CHIP_DEBUG_OUTr, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
        //tmp_dbr = (data&0xff) + ((data>>8)&0xff);
        tmp_dbr = (data&0xff);
//osal_printf("data =%x\n", data);
        if(tmp_dbr > *pDbru)
        {
            *pDbru = tmp_dbr;
        }
    }

    data = 0x0;
    if ((ret = reg_field_write(APOLLOMP_DBG_BLK_SELr, APOLLOMP_DBG_BLK_SELf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }
    data = 0x0;
    if ((ret = reg_field_write(APOLLOMP_DEBUG_SELr, APOLLOMP_DBGO_SELf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }
#endif
    return (RT_ERR_OK);
}

#if defined(CONFIG_RTK_L34_ENABLE)
extern int RTK_RG_SOFTWARE_RESET(void);
#endif

int gpon_dbru_thread(void *data)
{
#if defined(CONFIG_SDK_RTL9601B)
    rtk_gpon_onuState_t  state;
    uint32 sram, dram;
    rtk_ponmac_queue_t tcontQueue;
    uint32 sid;
    int32 ret;

    osal_time_sleep(60);

    while(!kthread_should_stop())
    {
        /* It is NOT possible to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(5 * HZ ); /* Wake up every 5 s*/
        
        if((ret = rtk_gpon_onuState_get(&state)) != RT_ERR_OK)
        {
            continue;
        }
        if(state != GPON_STATE_O5)
        {
            continue;
        }

        if((ret = rtl9601b_raw_pbo_usedPageCount_get(&sram, &dram)) == RT_ERR_OK)
        {
            if((sram+dram) > 6000)
            {
                tcontQueue.schedulerId = 7;
                tcontQueue.queueId = 31;
                rtk_ponmac_queueDrainOut_set(&tcontQueue);
            }
        }
    }
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    rtk_gpon_onuState_t  state;
    uint32 currPage, maxPage;
    uint32 dbru;
    rtk_ponmac_queue_t tcontQueue;
    int32 ret;

    osal_time_sleep(60);

    rg_sw_rst=0;
    while(!kthread_should_stop())
    {
        /* It is NOT possible to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(5 * HZ ); /* Wake up every 5 s*/

        if((ret = rtk_gpon_onuState_get(&state)) != RT_ERR_OK)
        {
            rg_sw_rst=0;
            continue;
        }
        if(state != GPON_STATE_O5)
        {
            rg_sw_rst=0;
            continue;
        }

        if((ret = apollomp_raw_flowctrl_ponUsedPage_get(127, &currPage, &maxPage)) == RT_ERR_OK)
        {
            if(currPage > 1000)
            {
                tcontQueue.schedulerId = 31;
                tcontQueue.queueId = 31;
                rtk_ponmac_queueDrainOut_set(&tcontQueue);
            }
        }

        /* check total used-page is small, but dbru is more than 0xF0, do reset switch and recover setting */
        if((ret = gpon_egrPage_get(&currPage)) == RT_ERR_OK)
        {   
           //osal_printf("pon port egr page=%d\n",currPage);
            if(currPage <= 10)
            {
                if ((ret = gpon_dbru_get(&dbru)) == RT_ERR_OK)
                {
                    //osal_printf("get dbru=0x%x!\n", dbru);
                    if(dbru > 0xFC)
                    {
                        rg_sw_rst++;
                        osal_printf("rg_sw_rst=%d, curPage=%d, dbr=0x%x!\n", rg_sw_rst, currPage, dbru);
                    }
                    else
                    {
                        rg_sw_rst=0;
                    }
                }
                else
                {
                    osal_printf("gpon_dbru_get fail, ret=0x%x\n",ret);
                }
            }
            
        }
        else
        {
            osal_printf("gpon_egrPage_get fail, ret=0x%x\n",ret);
        }

        if(rg_sw_rst>12) /* do reset switch*/
        {
            uint32 key;

            rg_sw_rst=0;
#if 0
            if ((ret = rtk_switch_softwareMagicKey_get(&key)) != RT_ERR_OK)
            {
                osal_printf("rtk_switch_softwareMagicKey_get fail ret=0x%x\n",ret);
            }
#endif
#if defined(CONFIG_RTK_L34_ENABLE)
            ret = RTK_RG_SOFTWARE_RESET();
            osal_printf("do rg sw reset, %d!!!\n",ret);
#endif
#if 0
            if ((ret = rtk_switch_softwareMagicKey_set(key)) != RT_ERR_OK)
            {
                osal_printf("rtk_switch_softwareMagicKey_set fail ret=0x%x\n",ret);
            }
#endif
            osal_time_sleep(10);
            //return (RT_ERR_OK);
        }
    }
#endif
    return (RT_ERR_OK);
}


int __init gpon_pppoe_init(void)
{
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 chipId, rev, subtype;
    rtk_switch_devInfo_t devInfo;

    rtk_switch_version_get(&chipId, &rev, &subtype);
    if(APOLLOMP_CHIP_ID==chipId)
    {/*only allo need polling*/
        osal_memset(&gpon_pppoe_db, 0x0, sizeof(gpon_pppoe_db_t));
        
        if (RT_ERR_OK != rtk_switch_deviceInfo_get(&devInfo))
            return -1;
        tcont_max = devInfo.capacityInfo.gpon_tcont_max;
        
        /* init OMCI message */
        omci_msg.len = 48;
        osal_memcpy(omci_msg.msg, dummy_content, sizeof(dummy_content));

        pGponPppoelTask = kthread_run(gpon_pppoe_thread, NULL, "gpon_pppoe");
        if(IS_ERR(pGponPppoelTask))
        {
            printk("%s:%d gpon_pppoe_init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pGponPppoelTask));
        }
        else
        {
            printk("%s:%d gpon_pppoe_init complete!\n", __FILE__, __LINE__);
        }

        pGponDbruTask = kthread_run(gpon_dbru_thread, NULL, "gpon_dbru");
        if(IS_ERR(pGponDbruTask))
        {
            printk("%s:%d gpon_pppoe_init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pGponDbruTask));
        }
        else
        {
            printk("%s:%d gpon_pppoe_init complete!\n", __FILE__, __LINE__);
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9601B)
    pGponDbruTask = kthread_run(gpon_dbru_thread, NULL, "gpon_dbru");
    if(IS_ERR(pGponDbruTask))
    {
        printk("%s:%d gpon_pppoe_init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pGponDbruTask));
    }
    else
    {
        printk("%s:%d gpon_pppoe_init complete!\n", __FILE__, __LINE__);
    }
#endif
    return 0;
}

void __exit gpon_pppoe_exit(void)
{
#if defined(CONFIG_SDK_APOLLOMP)
    kthread_stop(pGponPppoelTask);
    kthread_stop(pGponDbruTask);
#endif
#if defined(CONFIG_SDK_RTL9601B)
    kthread_stop(pGponDbruTask);
#endif
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek GPON PPPoE Advance module");
MODULE_AUTHOR("Realtek");
module_init(gpon_pppoe_init);
module_exit(gpon_pppoe_exit);
EXPORT_SYMBOL(gpon_pppoe_db_clear);
EXPORT_SYMBOL(gpon_pppoe_mode_get);


