/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __HAL_CRYPTO_UTIL_ROM_H__
#define __HAL_CRYPTO_UTIL_ROM_H__


/**************************************************************************
 * definition
 **************************************************************************/



/**************************************************************************
 * ROM functions
 **************************************************************************/



extern _LONG_CALL_ void rtl_memset(u8 *pData, u8 data, int bytes);

extern _LONG_CALL_ void *rtl_memcpy( void *s1, const void *s2, unsigned int n);


#endif  // __HAL_CRYPTO_ROM_H__

