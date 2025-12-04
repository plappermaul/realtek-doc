/*
 * Copyright (C) 2021 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : Definition of Init API
 *
 * Feature : Initialize All Layers of RTK Module
 *
 */

#ifndef __RT_INIT_H__
#define __RT_INIT_H__

/*
 * Include Files
 */

 /*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      rt_init_without_pon
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
rt_init_without_pon(void);

#endif /* __RT_INIT_H__ */