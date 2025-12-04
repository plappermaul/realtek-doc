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
 *
 * Purpose : I/O read/write APIs in the SDK.
 *
 * Feature : I/O read/write APIs
 *
 */

#ifndef __IO_RSP_H__
#define __IO_RSP_H__

/*
 * Include Files
 */

#include <common/type.h>
#include <soc/type.h>
#include <osal/lib.h>
/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */
uint32 io_rsp_memory_read(uint32 memaddr);
int io_rsp_memory_write(uint32 memaddr,uint32 data);
void io_rsp_init(void);

#endif  /* __IO_RSP_H__ */

