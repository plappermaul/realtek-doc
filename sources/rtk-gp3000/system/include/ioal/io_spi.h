/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : I/O SPI read/write APIs in the SDK.
 *
 * Feature : I/O SPI read/write APIs
 *
 */

#ifndef __IO_SPI_H__
#define __IO_SPI_H__

/*
 * Include Files
 */

#include <common/type.h>
#include <soc/type.h>

/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

#if defined(LINUX_KERNEL_SPI_IO)
extern int io_spi_phyReg_write(uint8 phy_id,uint8 reg, uint16 value);
extern int io_spi_phyReg_read(uint8 phy_id,uint8 reg, uint16 *pValue);
extern int32 spi_init(void);
#endif

#endif  /* __IO_SPI_H__ */

