/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "diag.h"

#include "peripheral.h"

#include "basic_types.h"
#include "section_config.h"
#include "rtl_utility.h"

HAL_NFBI_OP HalNFBIOp;
NFBI_ADAPTER NFBIAdap;


VOID
NFBIHandleUser(VOID *Data)
{
	PNFBI_ADAPTER pHalNFBIAdap = (PNFBI_ADAPTER) Data;

    HalNFBIIrqClearCommon();

    printf("Get NFBI interrupt\n");


}

VOID
NFBITestApp(
)
{

    PHAL_NFBI_OP pHalNFBIOp = (PHAL_NFBI_OP) &HalNFBIOp;
    
    HalNFBIOpInit((VOID*) (pHalNFBIOp));	

	        NFBIAdap.IrqHandle.IrqFun = (IRQ_FUN) NFBIHandleUser;
	        NFBIAdap.IrqHandle.Priority = 0;

              pHalNFBIOp->HalNFBIInit(&NFBIAdap);
               pHalNFBIOp->HalNFBIIrqRegister(&NFBIAdap);


	
}



