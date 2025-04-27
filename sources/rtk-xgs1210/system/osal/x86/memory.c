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
 * $Revision: 83221 $
 * $Date: 2017-11-08 13:40:12 +0800 (Wed, 08 Nov 2017) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : memory relative API
 *
 */

/*
 * Include Files
 */
#include <stdlib.h>
#include <osal/memory.h>

void *osal_alloc(uint32 size)
{
    return malloc((size_t)size);
}


void osal_free(void *pAddr)
{
    free(pAddr);
    pAddr = NULL;
    return;
}
