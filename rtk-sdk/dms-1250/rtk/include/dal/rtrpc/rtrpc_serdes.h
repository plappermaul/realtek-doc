/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_SERDES_H__
#define __RTRPC_SERDES_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>

#define hal_serdes_reg_get                  rtrpc_hal_serdes_reg_get
#define hal_serdes_reg_set                  rtrpc_hal_serdes_reg_set

int32
rtrpc_hal_serdes_reg_get(uint32 unit, uint32 sdsId, uint32 page, uint32 reg, uint32 *pData);

int32
rtrpc_hal_serdes_reg_set(uint32 unit, uint32 sdsId, uint32 page, uint32 reg, uint32 data);


#endif
