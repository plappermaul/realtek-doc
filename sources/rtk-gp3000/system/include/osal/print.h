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
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : 
 *
 */

#ifndef __OSAL_PRINT_H__
#define __OSAL_PRINT_H__

/*
 * Include Files
 */
#include <common/type.h>

/*
 * Symbol Definition
 */
#include <stdarg.h>


#if defined(CONFIG_SDK_KERNEL_LINUX) && defined(__KERNEL__)
#include <linux/kernel.h>
#define osal_printf     printk
#else
#include <stdio.h>         
#define osal_printf     printf
#endif
/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */
#if 0
/* Function Name:
 *      osal_printf
 * Description:
 *      print function
 * Input:
 *      pFmt, ... - input format with variable-length arguments.
 * Output:
 *      None
 * Return:
 *      Upon successful return, the function returns the number of 
 *      characters printed.
 * Note:
 *      None
 */
extern int32 osal_printf(uint8 *pFmt, ...);
#endif

#endif /* __OSAL_PRINT_H__ */

