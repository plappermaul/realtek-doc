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
 * $Revision$
 * $Date$
 *
 * Purpose : All header files of customer API will be located in this file.
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Extern customer's API to other RTK processes.
 *
 */

#ifndef __RTK_CUSTOMER_API_H__
#define __RTK_CUSTOMER_API_H__

/*
 * Include Files
 *      Any customer added header file which will be called by other RTK
 *      processes that not include by customer API, please add the header
 *      files in this customer_api.h.
 */
#include <rtk/customer/customer_api_demo.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : customer */

/* Function Name:
 *      rtk_customer_api_init
 * Description:
 *      This API is hooked in RTK initial flow already, and customer can fill the
 *      initial process in this API.
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
 * 	  This API is exported to other kernel module, then other modules can
 *      initial the customer API part, too.
 * Changes:
 *      None
 */

extern int32
rtk_customer_api_init(void);


#endif /* __RTK_CUSTOMER_API_H__ */
