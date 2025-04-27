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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition of Diag API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Dump whole registers and tables
 *
 */

#ifndef __RTK_CUSTOMER_API_DEMO_H__
#define __RTK_CUSTOMER_API_DEMO_H__

/*
 * Include Files
 */

/*
 * Symbol Definition
 */
typedef enum rtk_customer_apiType_e
{
    CUSTOMER_API_SHOW = 0,
    CUSTOMER_API_END
} rtk_customer_apiType_t;

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : customer */
/* Function Name:
 *      rtk_customer_add_api
 * Description:
 *      This is a demonstration API for customer.
 * Input:
 *      unit            - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID               - invalid unit id
 *      RT_ERR_OUT_OF_RANGE     - dumped table index is out of range
 * Applicable:
 *
 * Note:
 */

extern int32
rtk_customer_add_api(uint32 unit);

#endif /* __RTK_CUSTOMER_API_DEMO_H__ */
