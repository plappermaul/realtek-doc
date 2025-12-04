/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_GENERAL_H_
#define _MAC_AX_GENERAL_H_

#ifndef BIT
#define BIT(__n)        (1<<(__n))
#endif

#define Sleep(usec)     udelay(usec)

#define _ASSERT_(x)

typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef signed short    int16_t;
typedef signed int      int32_t;
#ifndef bool
typedef unsigned char   bool;
#endif
typedef	void* CRITICAL_SECTION;

#endif

