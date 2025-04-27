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
 * $Revision: 79989 $
 * $Date: 2017-06-23 15:37:59 +0800 (Fri, 23 Jun 2017) $
 *
 * Purpose : log level abstraction layer
 *
 * Feature : log level abstraction layer
 *
 */


/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <common/util/rt_util_system.h>
#include <ioal/ioal_log.h>
#if !defined(__BOOTLOADER__) && defined(__KERNEL__)
#include <linux/mtd/rtk_flash_common.h>
#endif

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

static int32
_ioal_log_bootMsgLevel_get(char *valBuf, int32 bufLen)
{
    char        name[] = "bootmsg";

    return rt_util_flashEnv_get(name, valBuf, bufLen);
}

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
int32
ioal_log_bootMsgLevel_get(uint32 *pInitMsgEnable, uint32 *pInitErrEnable)
{
#define BOOT_MSG_LEVEL_STR              8
    int32       ret;
    char        valStr[BOOT_MSG_LEVEL_STR];

    /* set a default value */
    *pInitMsgEnable = TRUE;
    *pInitErrEnable = TRUE;

    if ((ret = _ioal_log_bootMsgLevel_get(valStr, sizeof(valStr))) != RT_ERR_OK)
    {
        return ret;
    }

    if (!osal_strcmp(valStr, "0"))      //NONE
    {
        *pInitMsgEnable = FALSE;
        *pInitErrEnable = FALSE;
    }
    else if (!osal_strcmp(valStr, "1")) //FAIL
    {
        *pInitMsgEnable = FALSE;
        *pInitErrEnable = TRUE;
    }
    else if (!osal_strcmp(valStr, "2")) //ALL
    {
        *pInitMsgEnable = TRUE;
        *pInitErrEnable = TRUE;
    }
    else
    {

    }
    return RT_ERR_OK;
}








