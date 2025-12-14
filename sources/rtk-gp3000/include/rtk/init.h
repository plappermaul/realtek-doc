/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of Init API
 *
 * Feature : Initialize All Layers of RTK Module
 *
 */

#ifndef __RTK_INIT_H__
#define __RTK_INIT_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */

#if defined(CONFIG_LUNA_G3_SERIES) && defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/kernel.h>
#include <linux/string.h>
extern uint32 ca_rtk_debug;
#define RTK_API_LOCK() if(ca_rtk_debug & 0x8) {printk("\033[1;31m %s %d enter\033[m\n",__FUNCTION__,__LINE__);} \
                    if(ca_rtk_debug & 0x10) {dump_stack();}
#define RTK_API_UNLOCK() if(ca_rtk_debug & 0x8) {printk("\033[1;31m %s %d exit\033[m\n",__FUNCTION__,__LINE__);} \
                    if(ca_rtk_debug & 0x10) {dump_stack();}

#else
#define RTK_API_LOCK()
#define RTK_API_UNLOCK()
#endif
/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_init
 * Description:
 *      Initialize the specified device
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
extern int32
rtk_init(void);

/* Function Name:
 *      rtk_deinit
 * Description:
 *      De-Initialize the driver, release irq
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
int32
rtk_deinit(void);


extern int32
rtk_core_init(void);

extern int32
rtk_all_module_init(void);


extern int32
rtk_system_init(void);

/* Function Name:
 *      rtk_init_without_pon
 * Description:
 *      Initialize the driver, without pon related driver
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
extern int32
rtk_init_without_pon(void);

#endif /* __RTK_INIT_H__ */
