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
 * $Revision$
 * $Date$
 *
 * Purpose : Net Virual Device for Control NIC API
 *
 * Feature : Provide the APIs to control Virtaul Device
 *
 */


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h> 
#include <module/netvd/netvd_config.h>


extern int update_ct(void);

int ctMonitorTime=DEFUALT_CT_PERIODIC_TIME;

struct task_struct *ctMonitor;

static int conntrack_monitor(void* arg) {

    unsigned int timeout;
    int count = 0;
    for(;;) {
        if (kthread_should_stop()) break;
        VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s(): %d\n", __FUNCTION__, count++);
	  update_ct();
        do {
            set_current_state(TASK_INTERRUPTIBLE);
            timeout = schedule_timeout(ctMonitorTime * HZ);
        } while(timeout);
    }
    VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s() break\n",__FUNCTION__);

    return 0;
}


int netvd_thread_init (void) {

	 int ret;
	ctMonitor = kthread_create(conntrack_monitor, NULL, "conntrack_monitor");
	if (IS_ERR(ctMonitor)) {
	    ret = PTR_ERR(ctMonitor);
	    ctMonitor = NULL;
	    goto out;
	}
	wake_up_process(ctMonitor);

    return 0;
out:
    VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"error=%d\n",ret);
    return ret;
}

void netvd_thread_exit(void)
{
    kthread_stop(ctMonitor);
}

