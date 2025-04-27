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
 * $Revision: 91355 $
 * $Date: 2018-08-13 21:54:45 +0800 (Mon, 13 Aug 2018) $
 *
 * Purpose : Definition the basic types in the BSP.
 *
 * Feature : type definition
 *
 */

#ifndef __RTK_BSP_TYPE_H__
#define __RTK_BSP_TYPE_H__

/*
 * Symbol Definition
 */

#ifndef uint32
    typedef unsigned int        uint32;
#endif
#ifndef int32
    typedef signed int          int32;
#endif

#ifndef RTK_BSP_REG32
#define RTK_BSP_REG32(reg)		(*(volatile unsigned int   *)(reg))
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#endif /* __RTK_BSP_TYPE_H__ */

