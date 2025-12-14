/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_EFUSE_TEST_H_
#define _RTK_EFUSE_TEST_H_

typedef struct _EFUSE_VERI_PARA_ {
        u32         VeriItem;
        u32         Data;
        u32         Mask;
        u32         DataPort;
        u32         DataIdx;
}EFUSE_VERI_PARA,*PEFUSE_VERI_PARA;


typedef enum _EFUSE_VERI_ITEM_ {
        EFUSE_INIT_TEST = 1,    
	EFUSE_READ_TEST = 2,
        EFUSE_WRITE_TEST = 3,
        EFUSE_DUMP_TEST = 4,
        EFUSE_ISR_TEST = 5,
}EFUSE_VERI_ITEM, *PEFUSE_VERI_ITEM;

#endif
