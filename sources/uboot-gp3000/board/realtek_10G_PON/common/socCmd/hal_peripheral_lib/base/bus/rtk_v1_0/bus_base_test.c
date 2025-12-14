/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "common.h"
#include "bus_base.h"
#include "bus_base_test.h"

HAL_BUS_OP HalBUSOp;
BUS_ADAPTER BUS_Adap;

extern VOID HalBUSOpInit(IN VOID *Data);

VOID    
BUSTestApp(
    IN  VOID    *BUSTestData
)
{
        PBUS_VERI_PARA pBUSVerParaTmp = (PBUS_VERI_PARA)BUSTestData;
        PHAL_BUS_OP pHalBUSOp = (PHAL_BUS_OP)&HalBUSOp;
        HalBUSOpInit(pHalBUSOp);

        //init
        pHalBUSOp->HalBUSInit(NULL);

        switch(pBUSVerParaTmp->VeriItem)
        {
                case VERI_ITEM0:
                        // socTest BUS 0 0
                        pHalBUSOp->HalBUSVeriItem0(pBUSVerParaTmp);
                        break;
                case VERI_ITEM1:
                        // socTest BUS 1 0
                        pHalBUSOp->HalBUSVeriItem1(pBUSVerParaTmp);
                        break;
                case VERI_ITEM2:
                        // socTest BUS 2 0
                        pHalBUSOp->HalBUSVeriItem2(pBUSVerParaTmp);
                        break;
                case VERI_ITEM3:
                        // socTest BUS 3 0
                        pHalBUSOp->HalBUSVeriItem3(pBUSVerParaTmp);
                        break;
                case VERI_ITEM4:
                        // socTest BUS 4 0
                        pHalBUSOp->HalBUSVeriItem4(pBUSVerParaTmp);
                        break;
                case VERI_ITEM5:
                        // socTest BUS 5 0
                        pHalBUSOp->HalBUSVeriItem5(pBUSVerParaTmp);
                        break;
                case VERI_ITEM6:
                        // socTest BUS 6 0
                        pHalBUSOp->HalBUSVeriItem6(pBUSVerParaTmp);
                        break;
                default:
                        break;
        }
}
