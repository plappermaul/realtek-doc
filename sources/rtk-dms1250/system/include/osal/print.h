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
#ifdef __BOOTLOADER__
  #ifdef __UBOOT__
    #include <common.h>
    #include <exports.h>
  #endif
#else
  #include <common/rt_autoconf.h>
#endif

#include <common/type.h>

/*
 * Symbol Definition
 */
#ifdef __BOOTLOADER__

  #define osal_printf                                   printf
  #define osal_sprintf(_buf, _fmt, args...)             sprintf((char *)_buf, _fmt, ##args)
  #define osal_snprintf(_buf, _size, _fmt, args...)     snprintf((char *)_buf, _size, _fmt, ##args)

#elif defined(__KERNEL__)

  #include <linux/kernel.h>
  #define osal_printf                                   printk
  #define osal_sprintf(_buf, _fmt, args...)             sprintf((char *)_buf, _fmt, ##args)
  #define osal_snprintf(_buf, _size, _fmt, args...)     snprintf((char *)_buf, _size, _fmt, ##args)
  #define osal_vsnprintf(str, size, format, ap)         vsnprintf((char *)str, (size_t)size, (const char *)format,ap)
  #define osal_sscanf                                   sscanf
  #define osal_strtol                                   strtol

#elif defined(CONFIG_SDK_KERNEL_LINUX)

  #include <stdio.h>
  #define osal_printf                                   printf
  #define osal_sprintf(_buf, _fmt, args...)             sprintf((char *)_buf, _fmt, ##args)
  #define osal_snprintf(_buf, _size, _fmt, args...)     snprintf((char *)_buf, _size, _fmt, ##args)
  #define osal_vsnprintf(str, size, format, ap)         vsnprintf((char *)str, (size_t)size, (const char *)format,ap)
  #define osal_sscanf                                   sscanf
  #define osal_strtol                                   strtol

#else //CONFIG_SDK_OS_KERNEL_OTHER


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
extern int32 osal_printf(char *pFmt, ...);
extern int osal_sprintf(char *str, const char *format, ...);
extern int osal_snprintf(char *str, unsigned int size, const char *format, ...);
extern int osal_vsnprintf(char *str, unsigned int size, const char *format, char * ap);
extern int osal_sscanf(const char *str, const char *format, ...);
extern long int osal_strtol(const char *nptr, char **endptr, int base);
#endif

#endif /* __OSAL_PRINT_H__ */

