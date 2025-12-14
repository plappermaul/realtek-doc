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

#ifndef __IOAL_H__
#define __IOAL_H__

/*
 * Include Files
 */

#include <common/type.h>
#include <soc/type.h>
#include <ioal/io_rsp.h>
#include <ioal/io_mii.h>
/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */

#if defined(RTL_RLX_IO)
    #define IOAL_MEM32_WRITE(addr, val) io_rsp_memory_write(addr, val)
    #define IOAL_MEM32_READ(addr) io_rsp_memory_read(addr)
#else

    #if defined(LINUX_KERNEL_MDIO_IO) || defined(CYGWIN_MDIO_IO) || defined(CONFIG_LINUX_USER_SHELL)
        #define IOAL_MEM32_WRITE(addr, val) io_mii_memory_write(addr, val)
        #define IOAL_MEM32_READ(addr) io_mii_memory_read(addr)
    #else
        #define IOAL_MEM32_WRITE(addr, val) MEM32_WRITE(addr, val)
        #define IOAL_MEM32_READ(addr) MEM32_READ(addr)
    #endif
#endif



#if defined(CONFIG_SOC_ASICDRV_TEST)
    #define DRV_VIRTUAL_REG_SIZE		0x4000
#else
    #define DRV_VIRTUAL_REG_SIZE		0x900000
#endif


/*
 * Function Declaration
 */

#endif  /* __IOAL_H__ */

