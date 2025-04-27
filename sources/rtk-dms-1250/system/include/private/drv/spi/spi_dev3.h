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
 * Purpose : DRV APIs definition.
 *
 * Feature : SPI relative API
 *
 */
#ifndef __DRV_SPI_DEV3_H__
#define __DRV_SPI_DEV3_H__

/*
* Include Files
*/
//#include <common/rt_error.h>

/*
* Symbol Definition
*/

/*
 * Data Declaration
 */
#define RTL9300_INT_SPI_CTRL1_DATA_WIDTH_OFFSET                     (19)
#define RTL9300_INT_SPI_CTRL1_DATA_WIDTH_MASK                       (0x1FF << RTL9300_INT_SPI_CTRL1_DATA_WIDTH_OFFSET)
#define RTL9300_INT_SPI_CTRL1_ADDR_WIDTH_OFFSET                     (17)
#define RTL9300_INT_SPI_CTRL1_ADDR_WIDTH_MASK                       (0x3 << RTL9300_INT_SPI_CTRL1_ADDR_WIDTH_OFFSET)
#define SPI_EXTRA_MDELAY                                            (spiCurr.extra_mdelay)
#define SPI_TARGET_CLK                                              (SPI_CLK_25M)

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

int32 spi_dev3Reg_read(uint32 unit, uint32 mAddrs, uint32 *buff);
int32 spi_dev3Reg_write(uint32 unit, uint32 mAddrs, uint32 *buff);
int32 spi_dev3_init(uint32 unit);


#endif /* __DRV_SPI_DEV3_H__ */


