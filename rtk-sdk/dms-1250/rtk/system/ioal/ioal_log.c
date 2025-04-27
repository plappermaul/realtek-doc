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
#include <ioal/ioal_log.h>

#if !defined(CONFIG_SDK_EXTERNAL_CPU)
  #include <common/util/rt_util_system.h>
  #if !defined(__BOOTLOADER__) && defined(__KERNEL__)
    #include <linux/mtd/rtk_flash_common.h>
  #endif
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


/* Function Name:
 *      ioal_log_bootMsgLevel_get
 * Description:
 *      Tell SDK to enable or disable initial log message.
 *      (1) SDK will print initial post messages, if set *pInitMsgEnable = TRUE, otherwise please set *pInitMsgEnable = FALSE
 *      (2) SDK will print initial error messages, if set *pInitErrEnable = TRUE,  otherwise please set *pInitErrEnable = FALSE
 * Input:
 *      None
 * Output:
 *      pInitMsgEnable
 *      pInitErrEnable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
ioal_log_bootMsgLevel_get(uint32 *pInitMsgEnable, uint32 *pInitErrEnable)
{
#if defined(CONFIG_SDK_EXTERNAL_CPU)

    /* set a default value */
    *pInitMsgEnable = TRUE;
    *pInitErrEnable = TRUE;

    return RT_ERR_OK;

#else

  #define BOOT_MSG_LEVEL_STR              8
    int32       ret;
    char        valStr[BOOT_MSG_LEVEL_STR];
    char        name[] = "bootmsg";

    /* set a default value */
    *pInitMsgEnable = TRUE;
    *pInitErrEnable = TRUE;

    if ((ret = rt_util_flashEnv_get(name, valStr, sizeof(valStr))) != RT_ERR_OK)
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
#endif
}








