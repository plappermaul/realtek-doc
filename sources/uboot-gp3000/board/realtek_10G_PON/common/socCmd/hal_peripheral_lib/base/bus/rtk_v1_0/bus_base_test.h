/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _BUS_TEST_H_
#define _BUS_TEST_H_

typedef struct _BUS_VERI_PARA_ {
        u32  VeriItem;
        u32  VeriSubItem;
        u32  VeriSubSubItem;
} BUS_VERI_PARA,*PBUS_VERI_PARA;


typedef enum _BUS_VERI_ITEM_ {
        VERI_ITEM0 = 0,
        VERI_ITEM1 = 1,
        VERI_ITEM2 = 2,
        VERI_ITEM3 = 3,
        VERI_ITEM4 = 4,
        VERI_ITEM5 = 5,
        VERI_ITEM6 = 6
} BUS_VERI_ITEM, *PBUS_VERI_ITEM;

typedef enum _BUS_VERI_SUBITEM_ {
        VERI_SUBITEM0 = 0,
        VERI_SUBITEM1 = 1,
        VERI_SUBITEM2 = 2,
        VERI_SUBITEM3 = 3,
        VERI_SUBITEM4 = 4,
        VERI_SUBITEM5 = 5,
        VERI_SUBITEM6 = 6,
        VERI_SUBITEM7 = 7,
} BUS_VERI_SUBITEM, *PBUS_VERI_SUBITEM;

typedef enum _BUS_VERI_SUBSUBITEM_ {
        VERI_SUBSUBITEM0        = 0,
        VERI_SUBSUBITEM1        = 1,
        VERI_SUBSUBITEM2        = 2,
        VERI_SUBSUBITEM3        = 3,
        VERI_SUBSUBITEM4        = 4,
        VERI_SUBSUBITEM5        = 5,
        VERI_SUBSUBITEM6        = 6,
        VERI_SUBSUBITEM7        = 7,
        VERI_SUBSUBITEM8        = 8,
        VERI_SUBSUBITEM9        = 9,
        VERI_SUBSUBITEM10       = 10,
        VERI_SUBSUBITEM11       = 11,
        VERI_SUBSUBITEM12       = 12,
        VERI_SUBSUBITEM13       = 13,
        VERI_SUBSUBITEM14       = 14,
        VERI_SUBSUBITEM15       = 15,
        VERI_SUBSUBITEM16       = 16,
        VERI_SUBSUBITEM17       = 17,
        VERI_SUBSUBITEM18       = 18,
        VERI_SUBSUBITEM19       = 19,
        VERI_SUBSUBITEM20       = 20
} BUS_VERI_SUBSUBITEM, *PBUS_VERI_SUBSUBITEM;

typedef struct _HAL_BUS_OP_ {
        BOOL (*HalBUSInit)(VOID *Data);
        VOID (*HalBUSVeriItem0)(VOID *Data);
        VOID (*HalBUSVeriItem1)(VOID *Data);
        VOID (*HalBUSVeriItem2)(VOID *Data);
        VOID (*HalBUSVeriItem3)(VOID *Data);
        VOID (*HalBUSVeriItem4)(VOID *Data);
        VOID (*HalBUSVeriItem5)(VOID *Data);
        VOID (*HalBUSVeriItem6)(VOID *Data);
} HAL_BUS_OP, *PHAL_BUS_OP;

typedef struct _BUS_ADAPTER_ {
    u8 test_item;
} BUS_ADAPTER, *PBUS_ADAPTER;

#endif

