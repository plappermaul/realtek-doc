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
 * $Revision: 94134 $
 * $Date: 2018-12-07 18:40:51 +0800 (Fri, 07 Dec 2018) $
 *
 * Purpose : If the RTOS (Real-time Operation System) is not supported those kind
 *           of library, you can use the library functions.
 *
 * Feature : 1)Library for OS independent part
 *
 */

/*
 * Include Files
 */
#include <osal/lib.h>
#include <common/rt_type.h>
uint32 rtk_unit2devID[RTK_MAX_NUM_OF_UNIT]={0};
uint32 rtk_dev2unitID[RTK_MAX_NUM_OF_UNIT]={0};


/*
 * Function Declaration
 */
#if !defined(OS_LIB)
int  osal_strlen(const char *s)
{
    return strlen(s);
} /* osal_strlen */

int  osal_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
} /* osal_strcmp */

char *osal_strcpy(char *dst, const char *src)
{
    return strcpy(dst, src);
} /* osal_strcpy */

void *osal_memset(void *dst, int val, int len)
{
    return memset(dst, val, len);
} /* osal_memset */

void *osal_memcpy(void *dst, const void *src, int len)
{
    return memcpy(dst, src, len);
} /* osal_memcpy */

int  osal_memcmp(const void *mem1, const void *mem2, int len)
{
    return memcmp(mem1, mem2, len);
} /* osal_memcmp */
#endif
