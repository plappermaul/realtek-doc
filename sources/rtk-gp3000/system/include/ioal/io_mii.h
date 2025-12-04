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

#ifndef __IO_MII_H__
#define __IO_MII_H__

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
extern int io_mii_phy_reg_write(uint8 phy_id,uint8 reg, uint16 value);
extern int io_mii_phy_reg_read(uint8 phy_id,uint8 reg, uint16 *pValue);
extern uint32 io_mii_memory_read(uint32 memaddr);
extern int io_mii_memory_write(uint32 memaddr,uint32 data);
int io_mii_socMemory_write(uint32 memaddr,uint32 data);
uint32 io_mii_socMemory_read(uint32 memaddr);

#if defined(CYGWIN_MDIO_IO) || defined(LINUX_KERNEL_MDIO_IO)
extern void io_mii_init(void);
extern void io_mii_deinit(void);
#endif
#if defined(CYGWIN_MDIO_IO)
extern void MDC_PIN_DEF(int Power_bit, int CLK_bit, int CLK_EN_bit, int DA_bit, int DA_EN_bit);
#endif




#endif  /* __IO_RSP_H__ */

