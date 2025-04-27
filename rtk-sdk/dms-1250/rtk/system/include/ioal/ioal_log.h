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
 * Purpose : log abstraction layer
 *
 * Feature : log abstraction layer
 *
 */
#ifndef __IOAL_LOG_H__
#define __IOAL_LOG_H__

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
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      ioal_log_bootMsgLevel_get
 * Description:
 *      return enable/disable statue for initial log message
 * Input:
 *      None
 * Output:
 *      pInitMsgEnable  - print initial post messages
 *      pInitErrEnable  - print initial error messages
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
ioal_log_bootMsgLevel_get(uint32 *pInitMsgEnable, uint32 *pInitErrEnable);

#endif /* __IOAL_LOG_H__ */


