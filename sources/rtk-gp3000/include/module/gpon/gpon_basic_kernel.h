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
 * * 
 * $Revision$
 * $Date$
 *
 * Purpose : the basic head file for linux kernel
 *
 * Feature : Provide the GPON Mac Device Driver the basic head file for linux kernel
 *
 */

#ifndef _GPON_BASIC_KERNEL_H_
#define _GPON_BASIC_KERNEL_H_

#include <linux/kernel.h>
#include <linux/version.h>
#include <asm/system.h>
#include <linux/types.h>

#include <linux/slab.h> //for kmalloc
#include <asm/uaccess.h>
#include <asm/semaphore.h> //for kernel semaphore


#include <linux/module.h> //for module
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include <linux/timer.h>
#include <linux/sched.h>


#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/seq_file.h>
#include <linux/poll.h>

#include <linux/interrupt.h>
#include <linux/device.h>

#include <asm/current.h> //current

#endif /* _GPON_BASIC_KERNEL_H_ */
