/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
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
 * $Date: 2019-02-22 15:14:09 +0800 (Tue, 22 Nov 2016) $
 *
 * Purpose : PON monitor kernel thread
 *
 * Feature : Use kernel thread to perodically polling the use-page and dump bwmap
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



/* For RTK APIs */
#include <common/rt_error.h>
#include <rtk/switch.h>
#include <rtk/pbo.h>
#include <rtk/gponv2.h>
#include <rtk/stat.h>
#include <ioal/mem32.h>
#include <osal/time.h>

#if defined(CONFIG_SDK_RTL9607C)
#include <dal/rtl9607c/dal_rtl9607c_pbo.h>
#include <dal/rtl9607c/dal_rtl9607c_gpon.h>
#endif
#if defined(CONFIG_SDK_RTL9602C)
#include <dal/rtl9602c/dal_rtl9602c_pbo.h>
#include <dal/rtl9602c/dal_rtl9602c_gpon.h>
#endif

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
struct task_struct *pDsPboQueueTask;
static uint32 chipId, rev, subtype;
static uint64 previous_octet_cnt;


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */


int dspbo_queue_thread(void *data)
{
    rtk_gpon_onuState_t state;
    uint32 sram, dram;
    uint64 octet_cnt;
#if defined(CONFIG_SDK_RTL9607C)
    rtl9607c_pbo_dsQueue_type_t q_type;
#endif
#if defined(CONFIG_SDK_RTL9602C)
    rtl9602c_pbo_dsQueue_type_t q_type;
#endif
    uint32 on_th, off_th;
    uint32 dspbo_page_threshold;
    uint32 dspbo_rate_threshold;
    int32 ret;

    while(!kthread_should_stop())
    {
        /* It is NOT possible to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout( 1*HZ ); /* Wake up every 1 s */

        /* only check in O5 */
        if((ret = rtk_gpon_onuState_get(&state)) != RT_ERR_OK)
        {
            continue;
        }
        if(state != GPON_STATE_O5)
        {
            continue;
        }
        
        switch(chipId)
        {
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                /* get ds pbo fc ignore threshold to calc dspbo_page_threshold and dspbo_rate_threshold */
                if((ret = rtl9607c_raw_pbo_dsFcIgnoreThreshold_get(&on_th, &off_th))!= RT_ERR_OK)
                {
                    printk("get current used page fail ret=0x%x",ret);
                    return -1;
                }
                dspbo_page_threshold = on_th/2;
#if defined(CONFIG_PON_PBO_DS_PAGE_SIZE_128)
                dspbo_rate_threshold = dspbo_page_threshold*128;
#elif defined(CONFIG_PON_PBO_DS_PAGE_SIZE_256)
                dspbo_rate_threshold = dspbo_page_threshold*256;
#elif defined(CONFIG_PON_PBO_DS_PAGE_SIZE_512)
                dspbo_rate_threshold = dspbo_page_threshold*512;
#else
                dspbo_rate_threshold = dspbo_page_threshold*128;
#endif
                /* monitor the current ds used page */
                if((ret = rtl9607c_raw_pbo_dsUsedPageCount_get(&sram, &dram))!= RT_ERR_OK)
                {
                    printk("get current used page fail ret=0x%x",ret);
                    return -1;
                }
                
                /* monitor the traffic rate from ds pbo to switch */
                if((ret = rtk_stat_port_get(HAL_GET_PON_PORT(), IF_IN_OCTETS_INDEX, &octet_cnt))!= RT_ERR_OK)
                {
                    printk("get pon port in octets counter fail ret=0x%x",ret);
                    return -1;
                }
                //printk("in_octet=%llu, previous_octet_cnt=%llu\n",octet_cnt, previous_octet_cnt);
        
                if(octet_cnt < previous_octet_cnt)
                {
                    previous_octet_cnt = octet_cnt;
                    continue;
                }
        
                if ((ret = rtl9607c_raw_pbo_dsSidToQueueMap_get(RTL9607C_GPON_OMCI_FLOW_ID, &q_type)) != RT_ERR_OK)
                {
                    printk("get OMCI queue fail ret=0x%x",ret);
                    return -1;
                }

                //printk("cur_used_page=%u, rate=%llu bytes, type=%d\n", dram, (octet_cnt - previous_octet_cnt), q_type);

                if((dram > dspbo_page_threshold) && ((octet_cnt - previous_octet_cnt) < dspbo_rate_threshold))
                { /* when used page more than th, and traffic rate lower than th, set the OMCI to high queue */
                    if(q_type != RTL9607C_PBO_DSQUEUE_TYPE_HIGH)
                    {
                        if ((ret = rtl9607c_raw_pbo_dsSidToQueueMap_set(RTL9607C_GPON_OMCI_FLOW_ID, RTL9607C_PBO_DSQUEUE_TYPE_HIGH)) != RT_ERR_OK)
                        {
                            printk("set OMCI to high queue fail ret=0x%x",ret);
                            return -1;
                        }
                    }
                }
                else if((dram < 100) || ((octet_cnt - previous_octet_cnt) > dspbo_rate_threshold))
                { /* when used page lower then a small value, or traffic rate more than th, set the OMCI in normal0 queue */
                    { 
                        if(q_type != RTL9607C_PBO_DSQUEUE_TYPE_NORMAL0)
                        {
                            if ((ret = rtl9607c_raw_pbo_dsSidToQueueMap_set(RTL9607C_GPON_OMCI_FLOW_ID, RTL9607C_PBO_DSQUEUE_TYPE_NORMAL0)) != RT_ERR_OK)
                            {
                                printk("set OMCI to normal0 queue fail ret=0x%x",ret);
                                return -1;
                            }
                        }
                    }
                }
                previous_octet_cnt = octet_cnt;
                break;
#endif

#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                /* get ds pbo fc ignore threshold to calc dspbo_page_threshold and dspbo_rate_threshold */
                if((ret = rtl9602c_raw_pbo_dsFcIgnoreThreshold_get(&on_th, &off_th))!= RT_ERR_OK)
                {
                    printk("get current used page fail ret=0x%x",ret);
                    return -1;
                }
                dspbo_page_threshold = on_th/2;
#if defined(CONFIG_PON_PBO_DS_PAGE_SIZE_128)
                dspbo_rate_threshold = dspbo_page_threshold*128;
#elif defined(CONFIG_PON_PBO_DS_PAGE_SIZE_256)
                dspbo_rate_threshold = dspbo_page_threshold*256;
#elif defined(CONFIG_PON_PBO_DS_PAGE_SIZE_512)
                dspbo_rate_threshold = dspbo_page_threshold*512;
#else
                dspbo_rate_threshold = dspbo_page_threshold*128;
#endif
                /* monitor the current ds used page */
                if((ret = rtl9602c_raw_pbo_dsUsedPageCount_get(&sram, &dram))!= RT_ERR_OK)
                {
                    printk("get current used page fail ret=0x%x",ret);
                    return -1;
                }
                
                /* monitor the traffic rate from ds pbo to switch */
                if((ret = rtk_stat_port_get(HAL_GET_PON_PORT(), IF_IN_OCTETS_INDEX, &octet_cnt))!= RT_ERR_OK)
                {
                    printk("get pon port in octets counter fail ret=0x%x",ret);
                    return -1;
                }

                //printk("in_octet=%llu, previous_octet_cnt=%llu\n",octet_cnt, previous_octet_cnt);
        
                if(octet_cnt < previous_octet_cnt)
                {
                    previous_octet_cnt = octet_cnt;
                    continue;
                }
        
                if ((ret = rtl9602c_raw_pbo_dsSidToQueueMap_get(RTL9602C_GPON_OMCI_FLOW_ID, &q_type)) != RT_ERR_OK)
                {
                    printk("get OMCI queue fail ret=0x%x",ret);
                    return -1;
                }

                //printk("cur_used_page=%u, rate=%llu bytes, type=%d\n", dram, (octet_cnt - previous_octet_cnt), q_type);

                if((dram > dspbo_page_threshold) && ((octet_cnt - previous_octet_cnt) < dspbo_rate_threshold))
                { /* when used page more than th, and traffic rate lower than th, set the OMCI to high queue */
                    if(q_type != RTL9602C_PBO_DSQUEUE_TYPE_HIGH)
                    {
                        if ((ret = rtl9602c_raw_pbo_dsSidToQueueMap_set(RTL9602C_GPON_OMCI_FLOW_ID, RTL9602C_PBO_DSQUEUE_TYPE_HIGH)) != RT_ERR_OK)
                        {
                            printk("set OMCI to high queue fail ret=0x%x",ret);
                            return -1;
                        }
                    }
                }
                else if((dram < 100) || ((octet_cnt - previous_octet_cnt) > dspbo_rate_threshold))
                { /* when used page lower then a small value, or traffic rate more than th, set the OMCI in normal0 queue */
                    if(q_type != RTL9602C_PBO_DSQUEUE_TYPE_NORMAL0)
                    {
                        if ((ret = rtl9602c_raw_pbo_dsSidToQueueMap_set(RTL9602C_GPON_OMCI_FLOW_ID, RTL9602C_PBO_DSQUEUE_TYPE_NORMAL0)) != RT_ERR_OK)
                        {
                            printk("set OMCI to normal0 queue fail ret=0x%x",ret);
                            return -1;
                        }
                    }
                }
                previous_octet_cnt = octet_cnt;
                break;
#endif

            default:
                sram=0;
                dram=0;
            break;
        }
    }

    return (RT_ERR_OK);
}


int __init dspbo_queue_init(void)
{
    previous_octet_cnt = 0;

    /* get chip type */
    if(rtk_switch_version_get(&chipId, &rev, &subtype) != RT_ERR_OK)
        return -1;

    if((chipId == RTL9607C_CHIP_ID) || (chipId == RTL9602C_CHIP_ID))
    {
        pDsPboQueueTask = kthread_run(dspbo_queue_thread, NULL, "pon_mon");
        if(IS_ERR(pDsPboQueueTask)) {
            printk("%s:%d pDsPboQueueTask failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pDsPboQueueTask));
        }
        else {
            printk("%s:%d pDsPboQueueTask complete!\n", __FILE__, __LINE__);
        }
    }

    return 0;
}

void __exit dspbo_queue_exit(void)
{
    kthread_stop(pDsPboQueueTask);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek DS PBO Monitor module");
MODULE_AUTHOR("Realtek");
module_init(dspbo_queue_init);
module_exit(dspbo_queue_exit);

