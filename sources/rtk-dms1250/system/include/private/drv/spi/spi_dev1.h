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
#ifndef __DRV_SPI_DEV1_H__
#define __DRV_SPI_DEV1_H__

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

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

int32 spi_dev1Reg_read(uint32 unit, spi_cs_t cs, uint32 mAddrs, uint32 *buff);
int32 spi_dev1Reg_write(uint32 unit, spi_cs_t cs, uint32 mAddrs, uint32 *buff);
int32 spi_dev1_init(uint32 unit);


#endif /* __DRV_SPI_DEV1_H__ */


