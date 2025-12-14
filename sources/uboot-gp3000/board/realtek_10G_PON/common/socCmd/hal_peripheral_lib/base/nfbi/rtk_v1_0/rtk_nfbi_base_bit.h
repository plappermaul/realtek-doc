/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _NFBI_BASE_BIT_H_
#define _NFBI_BASE_BIT_H_

#define BIT_NFBI_NEW_MSG_COMING_MASK              	BIT(1)
#define BIT_SHIFT_NEW_MSG_COMING_MASK        	1
#define BIT_MASK_NEW_MSG_COMING_MASK         	0x1
#define BIT_NEW_MSG_COMING_MASK(x)      	(((x) & BIT_MASK_NEW_MSG_COMING_MASK) << BIT_SHIFT_NEW_MSG_COMING_MASK)

#endif //_NFBI_BASE_BIT_H_
