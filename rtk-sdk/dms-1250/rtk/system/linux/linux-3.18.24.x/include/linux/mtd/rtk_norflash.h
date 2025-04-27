/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 58581 $
 * $Date: 2015-05-12 15:02:48 +0800 (Tue, 12 May 2015) $
 *
 */

#ifndef _RTK_NORFLASH_H
#define _RTK_NORFLASH_H
#include <linux/mtd/rtk_flash_common.h>


#undef _DEBUG_
#ifdef _DEBUG_
    #define SHOWMESSAGE(msg, argv...) printk("#[%s][%d]" msg, __FILE__, __LINE__, ##argv)
#else
    #undef SHOEMESSAGE
    #define SHOWMESSAGE(msg, argv...)
#endif /*_DEBUG_*/

#define TRUE 1
#define BUSWIDTH 2

typedef unsigned long long  uint64;
typedef long long           int64;
typedef unsigned int        uint32;
typedef int                 int32;
typedef unsigned short      uint16;
typedef short               int16;
typedef unsigned char       uint8;
typedef char                int8;

#define FLASH_MAP_BOARD_INFO_ADDR 0x00004000

#define WINDOW_ADDR 0xBD000000

#define WINDOW_SIZE 0x02000000

#ifndef WINDOW_SIZE
    #error "FLASH size  Not Support"
#endif

#endif /*_RTK_SPIFLASH_H*/

